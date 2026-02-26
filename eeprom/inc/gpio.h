#pragma once

#include <stdint.h>

void gpio_output_init(volatile uint8_t* ddrx, const uint8_t pin);
void gpio_input_init(volatile uint8_t* portx, const uint8_t pin);
