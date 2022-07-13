#include <stdint.h>

#include "fibonacci.h"

// DEBUG includes, remove
#include <stdio.h>


uint16_t fibonacci_calc(uint16_t target) {

	if (target == 0) return FIBONACCI_ZERO;
	if (target >= 1 && target <= 3) return FIBONACCI_TRUE;

	uint16_t nums[3] = {0, 1, 0};
	uint16_t buf;

	for (uint16_t i = 0; i < 65535; ++i) {
		nums[(i + 2) % 3] = nums[i % 3] + nums[(i + 1) % 3];
		buf = nums[(i + 2) % 3];
		if (buf == target) {
			return FIBONACCI_TRUE;
		}
		if (buf > target) {
			return FIBONACCI_FALSE;
		}
	}
	return FIBONACCI_TOO_BIG;
}

