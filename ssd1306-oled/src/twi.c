#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "twi.h"

#define SDA PC4
#define SCL PC5

#define TWSR_STATUS_CODE_MASK	0xF8
#define TWSR_STATUS_CODE		(TWSR & TWSR_STATUS_CODE_MASK)

#define SCL_FREQUENCY_HZ	400000UL
#define TWBR_VALUE			((F_CPU / SCL_FREQUENCY_HZ - 16) / 2)

#define SLAVE_ADDRESS	0b1101000 // DS3231 RTC
#define SLA_W			(SLAVE_ADDRESS << 1)
#define SLA_R			((SLAVE_ADDRESS << 1) | 1)

#define CODE_START				0x08
#define CODE_REPEATED_START		0x10
#define CODE_SLA_W_ACK			0x18
#define CODE_TRANSMIT_DATA_ACK	0x28
#define CODE_TRANSMIT_DATA_NACK 0x30
#define CODE_SLA_R_ACK			0x40
#define CODE_RECEIVE_DATA_ACK	0x50
#define CODE_RECEIVE_DATA_NACK	0x58

#define TWI_AMOUNT_OF_RESET_CYCLES			100
#define TWI_AMOUNT_OF_TRANSMISSION_ATTEMPTS 100
#define TWI_DELAY_US						5
#define TWI_FAILURE							0x01

// inspired by this: https://github.com/Naguissa/uRTCLib/issues/42#issue-2229505594
// fix unsynchronized TWI bus (TWI_SUCCESS - success, TWI_FAILURE - failure), you need to manually enable TWI after calling the function
static uint8_t twi_reset(void) {
	TWCR &= ~(1 << TWEN); // disable TWI

	DDRC &= ~((1 << SDA) | (1 << SCL)); // configure SDA and SCL as input pins
	PORTC |= (1 << SDA) | (1 << SCL); // enable pull-up resistors
	_delay_us(TWI_DELAY_US);

	if (!(PINC & (1 << SDA))) { // check if TWI is in unknown state
		for (uint8_t i = 0; !(PINC & (1 << SDA)); i++) {
			DDRC |= (1 << SCL); // configure SCL as an output pin
			PORTC &= ~(1 << SCL); // SCL low
			_delay_us(TWI_DELAY_US);
		
			DDRC &= ~(1 << SCL); // configure SCL as an input pin
			PORTC |= (1 << SCL); // enable a pull-up resistor (SCL high)
			_delay_us(TWI_DELAY_US);

			if (i >= TWI_AMOUNT_OF_RESET_CYCLES)
				return TWI_FAILURE; // TWI bus is still unsynchronized
		}

		// START
		DDRC &= ~(1 << SCL); // configure SCL as an input pin
		PORTC |= (1 << SCL); // enable a pull-up resistor (SCL high)
		_delay_us(TWI_DELAY_US);
		DDRC |= (1 << SDA); // configure SDA as an output pin
		PORTC &= ~(1 << SDA); // SDA low
		_delay_us(TWI_DELAY_US);

		// STOP
		// DDRC &= ~(1 << SDA); // configure SDA as an input pin
		// PORTC |= (1 << SDA); // enable a pull-up resistor (SDA high)
		// _delay_us(TWI_DELAY_US);
	}

	return TWI_SUCCESS; // TWI bus became synchronized
}

void twi_init(void) {
	if (twi_reset() == TWI_FAILURE)
		return;

	TWBR = TWBR_VALUE; // frequency

	TWSR = ~((1 << TWPS1) | (1 << TWPS0)); // prescaler = 1
}

static void twi_start(void) { TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWEN); }

static void twi_stop(void) { TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN); }

static void twi_enable_ack(void) { TWCR = (1 << TWEA) | (1 << TWINT) | (1 << TWEN); }

// clears TWINT bit
static void twi_clear(void) { TWCR = (1 << TWINT) | (1 << TWEN); }

// waits for TWINT bit set
static void twi_wait(void) { while (!(TWCR & (1 << TWINT))); }

/*returns the exit code:
TWI_SUCCESS:	successfully transmitted
other:			error*/
uint8_t twi_receive_bytes(uint8_t *buf, const uint8_t start_address, const uint8_t amount_of_bytes) {
	if (amount_of_bytes == 0) return TWI_SUCCESS; // there is nothing to receive

	uint8_t status_code = 0;

	// transmit START
	twi_start();
	twi_wait();
	if (TWSR_STATUS_CODE != CODE_START) {
		status_code = TWSR_STATUS_CODE;
		twi_stop();
		return status_code;
	}

	// transmit SLA+W
	TWDR = SLA_W;
	twi_clear();
	twi_wait();
	if (TWSR_STATUS_CODE != CODE_SLA_W_ACK) {
		status_code = TWSR_STATUS_CODE;
		twi_stop();
		return status_code;
	}

	// transmit WORD ADDRESS
	TWDR = start_address;
	twi_clear();
	twi_wait();
	if (TWSR_STATUS_CODE != CODE_TRANSMIT_DATA_ACK) {
		status_code = TWSR_STATUS_CODE;
		twi_stop();
		return status_code;
	}

	// transmit REPEATED START
	twi_start();
	twi_wait();
	if (TWSR_STATUS_CODE != CODE_REPEATED_START) {
		status_code = TWSR_STATUS_CODE;
		twi_stop();
		return status_code;
	}

	// transmit SLA+R
	TWDR = SLA_R;
	twi_clear();
	twi_wait();
	if (TWSR_STATUS_CODE != CODE_SLA_R_ACK) {
		status_code = TWSR_STATUS_CODE;
		twi_stop();
		return status_code;
	}
	
	// receive DATA
	for (uint8_t i = 0; i < amount_of_bytes; i++) {
		if (i < amount_of_bytes - 1)
			twi_enable_ack(); // enable returning of ACK if this is not the last byte
		else
			twi_clear();
		twi_wait();

		if (TWSR_STATUS_CODE != CODE_RECEIVE_DATA_ACK && TWSR_STATUS_CODE != CODE_RECEIVE_DATA_NACK) {
			status_code = TWSR_STATUS_CODE;
			twi_stop();
			return status_code;
		}

		buf[i] = TWDR;
	}

	// transmit STOP
	twi_stop();
	return TWI_SUCCESS;
}

/*returns the exit code:
TWI_SUCCESS:	successfully transmitted
other:			error*/
uint8_t twi_transmit_bytes(const uint8_t *buf, const uint8_t start_address, const uint8_t amount_of_bytes) {
	if (amount_of_bytes == 0) return TWI_SUCCESS; // there is nothing to transmit

	uint8_t status_code = 0;

	// transmit START
	twi_start();
	twi_wait();

	if (TWSR_STATUS_CODE != CODE_START) {
		status_code = TWSR_STATUS_CODE;
		twi_stop();
		return status_code;
	}
	// transmit SLA+W
	TWDR = SLA_W;
	twi_clear();
	twi_wait();

	if (TWSR_STATUS_CODE != CODE_SLA_W_ACK) {
		status_code = TWSR_STATUS_CODE;
		twi_stop();
		return status_code;
	}
	// transmit WORD ADDRESS
	TWDR = start_address;
	twi_clear();
	twi_wait();

	if (TWSR_STATUS_CODE != CODE_TRANSMIT_DATA_ACK) {
		status_code = TWSR_STATUS_CODE;
		twi_stop();
		return status_code;
	}

	// transmit DATA
	for (uint8_t i = 0, attempts = 0; i < amount_of_bytes;) {
		TWDR = buf[i];
		twi_clear();
		twi_wait();

		switch (TWSR_STATUS_CODE) {
			// the current byte was successfully transmitted, go to the next one
			case CODE_TRANSMIT_DATA_ACK:
				attempts = 0;
				i++;
				break;
			// the current byte transmittion was failed, try again
			case CODE_TRANSMIT_DATA_NACK:
				if (attempts <= TWI_AMOUNT_OF_TRANSMISSION_ATTEMPTS) { // else go to default
					attempts++;
					break;
				}
				__attribute__ ((fallthrough)); // to disable -Wimplicit-fallthrough warning
			// all other states are treated as error
			default:
				attempts = 0;
				status_code = TWSR_STATUS_CODE;
				twi_stop();
				return status_code; 
				break;
		}
	}

	// transmit STOP
	twi_stop();
	return TWI_SUCCESS;
}
