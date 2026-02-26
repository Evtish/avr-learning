#include <string.h>

//#include "myutils.h"
#include "twi.h"
#include "oled.h"

#define DISPLAY_W 128
#define DISPLAY_H 64

#define DISPLAY_BUFFER_SIZE (DISPLAY_W * DISPLAY_H / 8)

uint8_t oled_init(void) {
	const uint8_t display_conf[] = {
		0b00000000,	// control byte (D/C# - command)
		0xA8,		// set MUX ratio
		0x3F,		// set MUX ratio
		0xD3,		// set display offset
		0x00,		// set display offset
		0x40,		// set display start line
		0xA0,		// set segment re-map
		0xC0,		// set COM output scan direction
		0x20, 0x01,	// set vertical addressing mode
		0xDA,		// set COM pins hardware configuration
		0x02,		// set COM pins hardware configuration
		0x81,		// set contrast control
		0x7F,		// set contrast control
		0xA4,		// disable entire display on
		0xA6,		// set normal display
		0xD5,		// set osc frequency
		0x80,		// set osc frequency
		0x8D,		// enable charge pump regulator
		0x14,		// enable charge pump regulator
		0xAF		// display on
	};

	return twi_transmit_bytes(display_conf, sizeof(display_conf));
}

uint8_t oled_clean(void) {
	uint8_t buffer[DISPLAY_BUFFER_SIZE + 1];
	buffer[0] = 0b01000000;
	memset(buffer + 1, 0x00, DISPLAY_BUFFER_SIZE);

	return twi_transmit_bytes(buffer, sizeof(buffer));
}
