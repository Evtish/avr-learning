#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum EepromStatus {
	EEPROM_IN_PROGRESS,
	EEPROM_DONE
} EepromStatus;

void eeprom_init(void);
EepromStatus eeprom_write(const uint8_t *data, const uint16_t address, const size_t n);
EepromStatus eeprom_read(uint8_t *data, const uint16_t address, const size_t n);
