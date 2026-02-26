#pragma once

#include <stdint.h>

#define PWM_MAX 1023

uint16_t pwm_transpose(const float brightness); // transpose brightness 0..1 -> 0..PWM_MAX with gamma correction
void pwm_shift_brightness(float *brightness, const float shift);
