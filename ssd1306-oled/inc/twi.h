#pragma once

#include <stdint.h>
#include <stddef.h>

#define TWI_FAILURE	0x01
#define TWI_SUCCESS 0xFF // because 0x00 is a TWI status code

void twi_init(void);
uint8_t twi_receive_bytes(uint8_t *buf, const size_t amount_of_bytes);
uint8_t twi_transmit_bytes(const uint8_t *buf, const size_t amount_of_bytes);
