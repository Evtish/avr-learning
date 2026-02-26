#pragma once

#include <stdint.h>
#include <stdbool.h>

#define BTN_SHORT_CLICK_TIME_MS			20UL // YOU CAN EDIT THE VALUE (the minimal recommended value is 20)
#define BTN_DEBOUNCE_CHECK_PERIOD_MS	5
#define BTN_DEBOUNCE_AMOUNT_TO_PASS		(BTN_SHORT_CLICK_TIME_MS / BTN_DEBOUNCE_CHECK_PERIOD_MS)

typedef struct {
	uint32_t last_call_time;
	uint16_t passed_debounce_count;
	volatile uint8_t *portx;	// PORTx
	volatile uint8_t *pinx;		// PINx
	uint8_t pin;				// Pxn (PORTxn)
	bool on_press;
	bool was_pressed;
} Button;

void button_init(const Button *btn);
void button_poll(Button *btn);
bool button_clicked(Button *btn);
