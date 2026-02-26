#include <avr/io.h>
#include <avr/interrupt.h>

#include "timings.h"

static volatile uint32_t timer1_overflow_counter = 0;

ISR(TIMER1_OVF_vect) {
	timer1_overflow_counter++;
}

void timer1_init(void) {
	TCCR1A |= (
		(1 << COM1A1) | // non-inverting PWM (OCR1A)
		(1 << COM1B1) | // non-inverting PWM (OCR1B)
		(0 << WGM10) | (1 << WGM11) // set 10-bit fast PWM mode
	);

	TCCR1B |= (
		(1 << WGM12) | (1 << WGM13) | // set 10-bit fast PWM mode
		(1 << CS11) // set prescaler = 8
	);

	ICR1 = TIMER_SIZE;

	TIMSK1 |= (1 << TOIE1); // enable overflow interrupt
}

static uint32_t ticks_to_ms(const uint32_t ticks) {
	return ticks * 1000UL / (F_CPU >> TIMER_PRESCALER_BITNESS);
}

uint32_t get_ticks(void) {
	// non-atomic access
	uint16_t tcnt1;
	uint32_t overflows;
	uint8_t sreg = SREG;
	
	cli(); // disable interrupts
	tcnt1 = TCNT1;
	overflows = timer1_overflow_counter;
	SREG = sreg; // restore interrupts

	return tcnt1 + overflows * TIMER_SIZE;
}

uint32_t get_time_ms(void) {
	return ticks_to_ms(get_ticks());
}
