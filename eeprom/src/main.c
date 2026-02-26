#include <stdint.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "myutils.h"
#include "button.h"
#include "gpio.h"
#include "pwm.h"
#include "timings.h"
// #include "spm.h"
#include "eeprom.h"

#define L_BTN_PIN PD3
#define R_BTN_PIN PD2
#define W_LED_PIN PB1
#define Y_LED_PIN PB2

#define W_LED_PWM OCR1A
#define Y_LED_PWM OCR1B

#define FIRST_TIME_KEY_VAL				((uint8_t) 40)
#define FIRST_TIME_KEY_EEPROM_ADDRESS	0x0000

#define BRIGHTNESS_DEFAULT_VAL			1.0f
#define BRIGHTNESS_EEPROM_ADDRESS		(FIRST_TIME_KEY_EEPROM_ADDRESS + sizeof(FIRST_TIME_KEY_VAL))

#define STEP_COUNT		7
#define BRIGHTNESS_STEP (1 / (float) STEP_COUNT)

int main(void) {
	float brightness = BRIGHTNESS_DEFAULT_VAL;
	float prev_brightness = brightness;
	uint8_t brightness_buffer[sizeof(brightness)];
	uint8_t first_time_key = 0;

	Button l_btn = {
		.last_call_time = 0,
		.passed_debounce_count = 0,
		.portx = &PORTD,
		.pinx = &PIND,
		.pin = L_BTN_PIN,
		.on_press = false,
		.was_pressed = false
	};
	Button r_btn = {
		.last_call_time = 0,
		.passed_debounce_count = 0,
		.portx = &PORTD,
		.pinx = &PIND,
		.pin = R_BTN_PIN,
		.on_press = false,
		.was_pressed = false
	};

	button_init(&l_btn);
	button_init(&r_btn);

	gpio_output_init(&DDRB, W_LED_PIN);
	gpio_output_init(&DDRB, Y_LED_PIN);

	eeprom_init();

	// spm_init();

	timer1_init();

	sei();

	while (eeprom_read(
		&first_time_key,
		FIRST_TIME_KEY_EEPROM_ADDRESS,
		sizeof(first_time_key)
	) != EEPROM_DONE);
	if (first_time_key == FIRST_TIME_KEY_VAL) {
		// there is the key so there is the brightness as well in EEPROM
		while (eeprom_read(
			brightness_buffer,
			BRIGHTNESS_EEPROM_ADDRESS,
			sizeof(brightness_buffer)
		) != EEPROM_DONE);
		brightness = *((float *) brightness_buffer);
		prev_brightness = brightness;
	}
	// the first boot ever
	else {
		first_time_key = FIRST_TIME_KEY_VAL;
		while (eeprom_write(
			&first_time_key,
			FIRST_TIME_KEY_EEPROM_ADDRESS,
			sizeof(first_time_key)
		) != EEPROM_DONE);
	}

	while (true) {
		button_poll(&l_btn);
		button_poll(&r_btn);

		if (button_clicked(&l_btn))
			pwm_shift_brightness(&brightness, BRIGHTNESS_STEP);
		if (button_clicked(&r_btn))
			pwm_shift_brightness(&brightness, -BRIGHTNESS_STEP);

		W_LED_PWM = pwm_transpose(brightness);
		Y_LED_PWM = pwm_transpose(1.0f - brightness);

		if (!equalf(brightness, prev_brightness) && eeprom_write(
			(uint8_t *) &brightness,
			BRIGHTNESS_EEPROM_ADDRESS,
			sizeof(brightness)
		) == EEPROM_DONE) {
			prev_brightness = brightness;
		}
	}

	return 0;
}
