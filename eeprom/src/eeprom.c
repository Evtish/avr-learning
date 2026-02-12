#include <avr/io.h>
#include <avr/interrupt.h>

// #include "spm.h"
#include "eeprom.h"

static volatile bool eeprom_ready = false;

ISR(EE_READY_vect) {
	eeprom_ready = true;
}

void eeprom_init(void) {
	uint8_t sreg = SREG;
	cli(); // disable interrupts
	EEAR = 0; // clear the address register (non-atomic access)
	SREG = sreg; // restore interrupts

	EECR = (
		~((1 << EEPM1) | (1 << EEPM0)) | // erase and write as one atomic operation
		(1 << EERIE) // EEPROM ready interrupt
	);
}

static void eeprom_write_byte(const uint8_t data, const uint16_t address) {
	uint8_t sreg = SREG;
	cli(); // disable interrupts

	EEAR = address; // write address
	EEDR = data; // write data
	EECR |= (1 << EEMPE); // master write enable
	EECR |= (1 << EEPE); // start writing

	SREG = sreg; // restore interrupts
}

EepromStatus eeprom_write(const uint8_t *data, const uint16_t address, const size_t n) {
	// TODO: figure out the SPM stuff (datasheet: 26.8.4 - p.234)
	if (!(eeprom_ready))// && spm_ready))
		return EEPROM_IN_PROGRESS;
	
	static size_t i = 0;
	if (i < n) {
		eeprom_write_byte(data[i], address + sizeof(data[0]) * i);
		eeprom_ready = false;
		// spm_ready = false;
		if (i >= n - 1) {
			i = 0;
			return EEPROM_DONE;
		}
		i++;
	}
	return EEPROM_IN_PROGRESS;
}

static uint8_t eeprom_read_byte(const uint16_t address) {
	uint8_t sreg = SREG;
	cli(); // disable interrupts
	EEAR = address; // write address (non-atomic access)
	SREG = sreg; // restore interrupts

	EECR |= (1 << EERE); // start writing
	return EEDR; // return data
}

EepromStatus eeprom_read(uint8_t *data, const uint16_t address, const size_t n) {
	if (!eeprom_ready)
		return EEPROM_IN_PROGRESS;
	
	static size_t i = 0;
	if (i < n) {
		data[i] = eeprom_read_byte(address + sizeof(data[0]) * i);
		eeprom_ready = false;
		if (i >= n - 1) {
			i = 0;
			return EEPROM_DONE;
		}
		i++;
	}
	return EEPROM_IN_PROGRESS;
}
