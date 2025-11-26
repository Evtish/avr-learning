#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_PIN PORTB1

#define TIMER1_PRESCALER_PWR_INDX 8  // 2 ^ TIMER1_PRESCALER_PWR_INDX is timer1 prescaler
#define OCR1A_VAL (F_CPU >> TIMER1_PRESCALER_PWR_INDX)  // actually this equals F_CPU / 2 ^ TIMER1_PRESCALER_PWR_INDX

// toggle LED if TIMER1 equals OCR1A value
// ISR(TIMER1_COMPA_vect) {
//     PORTB ^= (1 << LED_PIN);
// }

int main(void) {
    // set pins as output
    DDRB |= (1 << LED_PIN);
    // DDRD |= (1 << OCR1A_PIN);

    // set the value to which timer is compared
    OCR1AH = (OCR1A_VAL >> 8);  // high byte
    OCR1AL = (OCR1A_VAL & 0xFF);  // low byte
    // OCR1A = OCR1A_VAL;

    // TCCR1A |= (1 << COM1A0);
    TCCR1B |= (1 << WGM12) | (1 << CS12);  // set timer to CTC mode and set timer prescaler

    sei();  // allow interrupts
    // SREG |= SREG_I;

    TIMSK1 |= (1 << OCIE1A);  // enable 'clean timer on compare' interrupts

    // toggle LED if TIMER1 equals OCR1A value
    if (TIFR1 | ~(1 << OCF1A)) {
        PORTD ^= (1 << LED_PIN);
    }

    while (1) {}  // infinite loop
    return 0;
}

// T = 2 ^ timer_bit_capacity * timer_prescaler / freq
// timer_prescaler = freq * T / 2 ^ timer_bit_capacity
