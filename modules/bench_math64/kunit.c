#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/string_choices.h>

#include <kunit/test.h>

#include "bench_math64.h"
#include "kunit_shim.h"

static void benchmark_report(struct kunit *test, const char *name, ktime_t duration)
{
	kunit_info(test, "%-18s %10lluns\n", name, ktime_to_ns(duration));
}

static void mult_shift64(struct kunit *test)
{
	ktime_t start, end, baseline;
	u32 mult, shift, divisor;
	u64 iterations, input;

	iterations = 1000000ULL;
	input = (0x5678ULL << 32) + 0x1234;
	mult = 0x12345678;
	shift = 14;
	divisor = 0xffffffff;

	/* Prevent any attempts of optimizations */
	OPTIMIZER_HIDE_VAR(iterations);
	OPTIMIZER_HIDE_VAR(input);
	OPTIMIZER_HIDE_VAR(mult);
	OPTIMIZER_HIDE_VAR(shift);
	OPTIMIZER_HIDE_VAR(divisor);

	kunit_info(test, "iterations: %llu\n", iterations);
	kunit_info(test, "int128: %s\n", str_yes_no(IS_ENABLED(CONFIG_ARCH_SUPPORTS_INT128)));

	u64 tmp;
	if (!check_mul_overflow(input, mult, &tmp))
		KUNIT_FAIL(test, "multiplication is expected to overflow 64bits");

	scoped_guard(irq) {
		start = ktime_get();
		for (u64 i = 0; i < iterations; i++) {
			OPTIMIZER_HIDE_VAR(i);
			bench_noop();
		}
		end = ktime_get();
	}

	baseline = end - start;


	scoped_guard(irq) {
		start = ktime_get();
		for (u64 i = 0; i < iterations; i++) {
			OPTIMIZER_HIDE_VAR(i);
			bench_div_u64(input, divisor);
		}
		end = ktime_get();
	}

	benchmark_report(test, "division", end - start - baseline);


	scoped_guard(irq) {
		start = ktime_get();
		for (u64 i = 0; i < iterations; i++) {
			OPTIMIZER_HIDE_VAR(i);
			bench_opencoded_mul_shift(input, mult, shift);
		}
		end = ktime_get();
	}

	benchmark_report(test, "mul shift", end - start - baseline);


	scoped_guard(irq) {
		start = ktime_get();
		for (u64 i = 0; i < iterations; i++) {
			OPTIMIZER_HIDE_VAR(i);
			bench_mul_u64_u32_shr(input, mult, shift);
		}
		end = ktime_get();
	}

	benchmark_report(test, "mul_u64_u32_shr()", end - start - baseline);
}

static struct kunit_case bench_math64_test_cases[] = {
	KUNIT_CASE_SLOW(mult_shift64),
	{}
};

static struct kunit_suite bench_math64_test_suite = {
	.name		= "benchmark_math64",
	.test_cases	= bench_math64_test_cases,
};

kunit_test_suite(bench_math64_test_suite);
MODULE_DESCRIPTION("math64 benchmark");
MODULE_LICENSE("GPL");
