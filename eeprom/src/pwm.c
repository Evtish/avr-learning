#include <math.h>

#include "myutils.h"
#include "pwm.h"

#define GAMMA 0.45f

uint16_t pwm_transpose(const float brightness) {
	return ftranspose(powf(brightness, GAMMA), 0, 1, 0, PWM_MAX);
}

void pwm_shift_brightness(float *brightness, const float shift) {
	if (shift < 0.0f && *brightness + 2 * shift < 0.0f)
		*brightness = 0.0f;
	else if (*brightness + 2 * shift > 1.0f)
		*brightness = 1.0f;
	else
		*brightness += shift;
}
