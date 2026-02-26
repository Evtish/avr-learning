#include <math.h>

#include "myutils.h"

#define ALMOST_ZERO_F 1e-5f

bool equalf(const float a, const float b) {
	return fabsf(a - b) <= ALMOST_ZERO_F;
}

// static float clampf(const float val, const float low, const float high) {
// 	return fminf(fmaxf(val, low), high);
// }

/* assuming:
 * val in low_in..high_in
 * high_in > low_in
 * high_out > low_out
 */
float ftranspose(const float val, const float low_in, const float high_in, const float low_out, const float high_out) {
// 	const float d_in = high_in - low_in, d_out = high_out - low_out;
// 	const float res = low_out + (val - low_in) * (d_out / d_in);
// 	return clampf(res, low_out, high_out);
	return low_out + (val - low_in) * (high_out - low_out) / (high_in - low_in);
}
