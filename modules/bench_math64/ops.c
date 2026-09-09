#include <linux/types.h>
#include <linux/math64.h>

#include "bench_math64.h"

void bench_noop(void)
{
}

u64 bench_div_u64(u64 input, u32 divisor)
{
	return div_u64(input, divisor);
}

u64 bench_opencoded_mul_shift(u64 input, u32 mult, u32 shift)
{
	return (input * mult) >> shift;
}

u64 bench_mul_u64_u32_shr(u64 input, u32 mult, u32 shift)
{
	return mul_u64_u32_shr(input, mult, shift);
}
