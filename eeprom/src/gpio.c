#include "gpio.h"

// set pin as output
void gpio_output_init(volatile uint8_t* ddrx, const uint8_t pin) {
	*ddrx |= (1 << pin);
}

// turn on pull-up resistor
void gpio_input_init(volatile uint8_t* portx, const uint8_t pin) {
	*portx |= (1 << pin);
}
