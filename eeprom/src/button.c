#include "gpio.h"
#include "timings.h"
#include "button.h"

void button_init(const Button *btn) {
	gpio_input_init(btn->portx, btn->pin);
}

static bool button_is_pressed(const Button *btn) {
	return (*(btn->pinx) & (1 << btn->pin)) == btn->on_press;
}

static void button_update(Button* btn) {
	bool btn_is_pressed = button_is_pressed(btn);

	if (btn_is_pressed) {
		if (!btn->was_pressed)
			btn->passed_debounce_count = 1;
		else if (btn->passed_debounce_count >= 1)
			btn->passed_debounce_count++;
	}
	else
		btn->passed_debounce_count = 0;

	btn->was_pressed = btn_is_pressed;
}

void button_poll(Button *btn) {
	uint32_t time_now = get_time_ms();
	if (time_now - btn->last_call_time >= BTN_DEBOUNCE_CHECK_PERIOD_MS) {
		btn->last_call_time = time_now;
		button_update(btn);
	}
}

bool button_clicked(Button *btn) {
	if (btn->passed_debounce_count >= BTN_DEBOUNCE_AMOUNT_TO_PASS) {
		btn->passed_debounce_count = 0;
		return true;
	}
	return false;
}
