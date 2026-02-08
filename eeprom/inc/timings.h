#pragma once

#include <stdint.h>

#include "pwm.h"

#define TIMER_SIZE PWM_MAX
#define TIMER_PRESCALER_BITNESS 3

void timer1_init(void);
uint32_t get_ticks(void);
uint32_t get_time_ms(void);
