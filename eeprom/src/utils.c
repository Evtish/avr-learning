#include <math.h>

#include "utils.h"

#define ALMOST_ZERO_F 1e-5f

bool equalf(const float a, const float b) {
	return fabs(a - b) <= ALMOST_ZERO_F;
}

static float limitf(const float val, const float low, const float high) {
	if (val > high)
		return high;
	else if (val < low)
		return low;
	else
		return val;
}

float ftranspose(const float val, const float low_in, const float high_in, const float low_out, const float high_out) {
	float d_in = high_in - low_in, d_out = high_out - low_out;
	float res = low_out + (val - low_in) * (d_out / d_in);
	return limitf(res, low_out, high_out); // TODO: do i really need limitf?
}
