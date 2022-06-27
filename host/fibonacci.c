#include <stdint.h>

#include "fibonacci.h"

// DEBUG includes, remove
#include <stdio.h>

int fibonacci(uint64_t target) {

	if (target >= 0 && target <= 3) return FIBONACCI_TRUE;

	uint64_t nums[3] = {0, 1, 0};

	for (uint64_t i = 0; i < -1; ++i) {
		nums[(i + 2) % 3] = nums[i % 3] + nums[(i + 1) % 3];
		if (nums[(i + 2) % 3] == target) return FIBONACCI_TRUE;
		if (nums[(i + 2) % 3] > target) return FIBONACCI_FALSE;
	}
	return FIBONACCI_TOO_BIG;
}

