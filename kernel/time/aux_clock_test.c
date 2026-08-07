// SPDX-License-Identifier: GPL-2.0

#include <linux/clocksource.h>
#include <linux/types.h>

#include <kunit/test.h>

#include "timekeeping.h"

/* See kernel/time/timekeeping.c */
#define TK_AUX_MONO_CONV_MAX_DELTA_NS		(180LL * NSEC_PER_SEC)

static ktime_t tk_aux_mono_conv_error(ktime_t kt)
{
	/* For TK_AUX_MONO_CONV_SHIFT = 26 */
	return  div_u64(kt * 2, 100000000);
}

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
			{ .mono = 100,		.aux = 150 },
			{ .mono = 110,		.aux = 161 },
			{ .mono = 120,		.aux = 172 },
			{ .mono = 130,		.aux = 183 },
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

#define AUX_MONO_CONV_EXPECT_CLOSE(_test, _actual, _expected)			\
({										\
	ktime_t actual, expected, lower_bound, upper_bound, error;		\
										\
	actual = (_actual);							\
	expected = (_expected);							\
	error = tk_aux_mono_conv_error(expected);				\
	lower_bound = expected - error - 1;					\
	upper_bound = expected + error;						\
										\
	KUNIT_EXPECT_TRUE_MSG((_test),						\
			      actual >= lower_bound && actual <= upper_bound,	\
			      "where\n"						\
				"\tlower_bound = %lld\n"			\
				"\tactual      = %lld\n"			\
				"\tupper_bound = %lld",				\
				lower_bound, actual, upper_bound);		\
})

static void mono_conv(struct kunit *test)
{
	const struct mono_conv_test_case *params = test->param_value;
	struct tk_aux_mono_conv conv;

	setup_aux_mono_conv(&conv, params->speed_difference_percentage,
			    params->mono_base, params->aux_base);

	for (size_t i = 0; i < ARRAY_SIZE(params->timestamps); i++) {
		ktime_t mono, aux, expected_mono, expected_aux, converted_mono, converted_aux;

		mono = params->timestamps[i].mono;
		aux = params->timestamps[i].aux;
		kunit_info(test, "timestamp mono=%lld aux=%lld\n", mono, aux);

		/* Sanity check against explicit divisions. */
		expected_aux = mono - conv.mono_base;
		expected_aux *= 100 + params->speed_difference_percentage;
		expected_aux = div_s64(expected_aux, 100);
		expected_aux += conv.aux_base;

		KUNIT_ASSERT_EQ(test, aux, expected_aux);

		if (abs(mono - conv.mono_base) <= TK_AUX_MONO_CONV_MAX_DELTA_NS) {
			converted_aux = ktime_mono_to_aux(&conv, mono);
			AUX_MONO_CONV_EXPECT_CLOSE(test, converted_aux, aux);
		} else {
			/* Larger values are illegal to pass. */
		}

		converted_mono = ktime_aux_to_mono(&conv, aux);
		if (abs(aux - conv.aux_base) <= TK_AUX_MONO_CONV_MAX_DELTA_NS) {
			AUX_MONO_CONV_EXPECT_CLOSE(test, converted_mono, mono);
		} else {
			/* Large values are clamped. */
			expected_mono = TK_AUX_MONO_CONV_MAX_DELTA_NS;
			expected_mono *= 100;
			expected_mono = div_s64(expected_mono,
						100 + params->speed_difference_percentage);
			expected_mono += conv.mono_base;

			AUX_MONO_CONV_EXPECT_CLOSE(test, converted_mono, expected_mono);
		}
	}

}

static struct kunit_case auxclock_test_cases[] = {
	KUNIT_CASE_PARAM(mono_conv, mono_conv_gen_params),
	{}
};

static struct kunit_suite auxclock_test_suite = {
	.name		= "auxclock",
	.test_cases	= auxclock_test_cases,
};

kunit_test_suite(auxclock_test_suite);
