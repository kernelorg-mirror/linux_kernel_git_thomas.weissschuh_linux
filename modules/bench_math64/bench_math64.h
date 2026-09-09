#include <linux/types.h>

void bench_noop(void);
u64 bench_div_u64(u64 input, u32 divisor);
u64 bench_opencoded_mul_shift(u64 input, u32 mult, u32 shift);
u64 bench_mul_u64_u32_shr(u64 input, u32 mult, u32 shift);
