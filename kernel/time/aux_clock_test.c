// SPDX-License-Identifier: GPL-2.0

#include <linux/clocksource.h>
#include <linux/types.h>

#include <kunit/test.h>

#include "timekeeping.h"

/* See kernel/time/timekeeping.c */
#define TK_AUX_MONO_CONV_MAX_DELTA_NS		(180LL * NSEC_PER_SEC)

struct mono_conv_test_case {
	ktime_t	mono_base;
	ktime_t	aux_base;
	s8	speed_difference_percentage;
	struct {
		ktime_t mono, aux;
	}	timestamps[8];
};

static const struct mono_conv_test_case mono_conv_test_cases[] = {
	/* Identical clocks */
	{
		.mono_base			= 0,
		.aux_base			= 0,
		.speed_difference_percentage	= 0,
		.timestamps			= {
			{ .mono = 50,		.aux = 50 },
			{ .mono = 100,		.aux = 100 },
			{ .mono = 110,		.aux = 110 },
			{ .mono = 120,		.aux = 120 },
			{ .mono = 130,		.aux = 130 },
			{ .mono = 1000000000,	.aux = 1000000000 },
			{ .mono = 20000000000,	.aux = 20000000000 },
			{ .mono = 300000000000,	.aux = 300000000000 },
		},
	},
	/* Different bases */
	{
		.mono_base			= 100,
		.aux_base			= 150,
		.speed_difference_percentage	= 0,
		.timestamps			= {
			{ .mono = 50,		.aux = 100 },
			{ .mono = 100,		.aux = 150 },
			{ .mono = 110,		.aux = 160 },
			{ .mono = 120,		.aux = 170 },
			{ .mono = 130,		.aux = 180 },
			{ .mono = 1000000000,	.aux = 1000000050 },
			{ .mono = 20000000000,	.aux = 20000000050 },
			{ .mono = 300000000000,	.aux = 300000000050 },
		},
	},
	/* Aux clock runs faster */
	{
		.mono_base			= 100,
		.aux_base			= 150,
		.speed_difference_percentage	= 10,
		.timestamps			= {
			{ .mono = 50,		.aux = 95 },
			{ .mono = 100	,	.aux = 150 },
			{ .mono = 110	,	.aux = 161 },
			{ .mono = 120	,	.aux = 172 },
			{ .mono = 130	,	.aux = 183 },
			{ .mono = 1000000000,	.aux = 1100000040 },
			{ .mono = 20000000000,	.aux = 22000000040 },
			{ .mono = 300000000000,	.aux = 330000000040 },
		},
	},
	/* Aux clock runs slower */
	{
		.mono_base			= 100,
		.aux_base			= 150,
		.speed_difference_percentage	= -10,
		.timestamps			= {
			{ .mono = 50,		.aux = 105 },
			{ .mono = 100,		.aux = 150 },
			{ .mono = 110,		.aux = 159 },
			{ .mono = 120,		.aux = 168 },
			{ .mono = 130,		.aux = 177 },
			{ .mono = 1000000000,	.aux = 900000060 },
			{ .mono = 20000000000,	.aux = 18000000060 },
			{ .mono = 300000000000,	.aux = 270000000060 },
		},
	},
};

static void mono_conv_test_desc(const struct mono_conv_test_case *params, char *desc)
{
	snprintf(desc, KUNIT_PARAM_DESC_SIZE,
		 "mono_base=%4lld aux_base=%4lld speed_difference=%3d%%",
		 params->mono_base, params->aux_base, params->speed_difference_percentage);
}

KUNIT_ARRAY_PARAM(mono_conv, mono_conv_test_cases, mono_conv_test_desc);

static void setup_aux_mono_conv(struct tk_aux_mono_conv *conv, s8 speed_difference_percentage,
				ktime_t mono_base, ktime_t aux_base)
{
	u32 mono_mult, aux_mult, shift;

	clocks_calc_mult_shift(&mono_mult, &shift, 1, 1, 200);
	aux_mult = div_u64((u64)mono_mult * (100 + speed_difference_percentage), 100);

	tk_aux_capture_mono_conv(conv, mono_base, mono_mult, aux_base, aux_mult);
}

#define AUX_MONO_CONV_EXPECT_CLOSE(_test, _accuracy, _actual, _expected)	\
({										\
	typeof(_expected) actual, expected, lower_bound, upper_bound;		\
										\
	actual = (_actual);							\
	expected = (_expected);							\
	lower_bound = expected - div_u64(expected, (_accuracy)) - 1;		\
	upper_bound = expected + div_u64(expected, (_accuracy));		\
										\
	KUNIT_EXPECT_TRUE_MSG((_test),						\
			      actual >= lower_bound && actual <= upper_bound,	\
			      "where\n"						\
				"\tlower_bound=%lld\n"				\
				"\tactual=%lld\n"				\
				"\tupper_bound=%lld",				\
				lower_bound, actual, upper_bound);		\
})

static void mono_conv(struct kunit *test)
{
	const struct mono_conv_test_case *params = test->param_value;
	const u64 accuracy = 10000000;
	struct tk_aux_mono_conv conv;

	setup_aux_mono_conv(&conv, params->speed_difference_percentage,
			    params->mono_base, params->aux_base);

	for (size_t i = 0; i < ARRAY_SIZE(params->timestamps); i++) {
		ktime_t mono, aux, expected_aux, converted_mono, converted_aux;
		bool is_before;

		mono = params->timestamps[i].mono;
		aux = params->timestamps[i].aux;
		kunit_info(test, "timestamp mono=%lld aux=%lld\n", mono, aux);

		/* Sanity check against explicit divisions */
		expected_aux = mono - conv.mono_base;
		expected_aux *= 100 + params->speed_difference_percentage;
		expected_aux = div_s64(expected_aux, 100);
		expected_aux += conv.aux_base;

		KUNIT_ASSERT_EQ(test, aux, expected_aux);

		if (abs(mono - conv.mono_base) < TK_AUX_MONO_CONV_MAX_DELTA_NS) {
			converted_aux = ktime_mono_to_aux(mono, &conv);
			AUX_MONO_CONV_EXPECT_CLOSE(test, accuracy, converted_aux, aux);
		}

		is_before = ktime_aux_before_mono_and_convert(&conv, aux,
							      mono - div_u64(mono, accuracy) - 1,
							      &converted_mono);
		KUNIT_EXPECT_FALSE(test, is_before);

		/* Seed an obviously wrong value */
		converted_mono = KTIME_MIN;

		is_before = ktime_aux_before_mono_and_convert(&conv, aux,
							      mono + div_u64(mono, accuracy) + 1,
							      &converted_mono);
		KUNIT_EXPECT_TRUE(test, is_before);
		AUX_MONO_CONV_EXPECT_CLOSE(test, accuracy, converted_mono, mono);
	}

}

/* Mostly for development only. */
static void mono_conv_edge_cases(struct kunit *test)
{
	ktime_t mono_base, aux_base, small_delta, large_delta;
	struct tk_aux_mono_conv conv;
	ktime_t converted_mono;
	bool is_before;

	mono_base = 0;
	aux_base = 0;
	small_delta = 5 * NSEC_PER_SEC;
	/* 1 year */
	large_delta = 365 * 86400 * NSEC_PER_SEC;

	setup_aux_mono_conv(&conv, 5, mono_base, aux_base);

	/* delta = 0 */
	is_before = ktime_aux_before_mono_and_convert(&conv, aux_base, mono_base, &converted_mono);
	KUNIT_EXPECT_FALSE(test, is_before);

	is_before = ktime_aux_before_mono_and_convert(&conv, aux_base, mono_base + 1,
						      &converted_mono);
	KUNIT_EXPECT_TRUE(test, is_before);
	KUNIT_EXPECT_EQ(test, converted_mono, mono_base);

	/* positive small delta */
	is_before = ktime_aux_before_mono_and_convert(&conv, aux_base + small_delta, mono_base,
						      &converted_mono);
	KUNIT_EXPECT_FALSE(test, is_before);

	is_before = ktime_aux_before_mono_and_convert(&conv, aux_base, mono_base + small_delta,
						      &converted_mono);
	KUNIT_EXPECT_TRUE(test, is_before);
	KUNIT_EXPECT_EQ(test, converted_mono, mono_base);

	/* positive large delta */
	is_before = ktime_aux_before_mono_and_convert(&conv, aux_base + large_delta, mono_base,
						      &converted_mono);
	KUNIT_EXPECT_FALSE(test, is_before);

	is_before = ktime_aux_before_mono_and_convert(&conv, aux_base, mono_base + large_delta,
						      &converted_mono);
	KUNIT_EXPECT_TRUE(test, is_before);
	KUNIT_EXPECT_EQ(test, converted_mono, mono_base);

	/* negative small delta */
	is_before = ktime_aux_before_mono_and_convert(&conv, aux_base - small_delta, mono_base,
						      &converted_mono);
	KUNIT_EXPECT_TRUE(test, is_before);
	KUNIT_EXPECT_EQ(test, converted_mono, -4761904702);

	is_before = ktime_aux_before_mono_and_convert(&conv, aux_base, mono_base - small_delta,
						      &converted_mono);
	KUNIT_EXPECT_FALSE(test, is_before);

	/* negative large delta */
	is_before = ktime_aux_before_mono_and_convert(&conv, aux_base - large_delta, mono_base,
						      &converted_mono);
	KUNIT_EXPECT_TRUE(test, is_before);
	KUNIT_EXPECT_EQ(test, converted_mono, -30034285333871842);

	is_before = ktime_aux_before_mono_and_convert(&conv, aux_base, mono_base - large_delta,
						      &converted_mono);
	KUNIT_EXPECT_FALSE(test, is_before);
}

static struct kunit_case auxclock_test_cases[] = {
	KUNIT_CASE_PARAM(mono_conv, mono_conv_gen_params),
	KUNIT_CASE(mono_conv_edge_cases),
	{}
};

static struct kunit_suite auxclock_test_suite = {
	.name		= "auxclock",
	.test_cases	= auxclock_test_cases,
};

kunit_test_suite(auxclock_test_suite);
