// SPDX-License-Identifier: GPL-2.0-only
/*
 * Test cases for linux/foreach_macros.h.
 */

#include <linux/bits.h>
#include <linux/foreach_macros.h>
#include <linux/module.h>

#include <kunit/test.h>

static void foreach(struct kunit *test)
{
#define FOREACH_BIT(x) BIT(x) |

	static const u64 empty_args	= FOREACH(FOREACH_BIT) FOREACH_END(0);
	static const u64 one_arg	= FOREACH(FOREACH_BIT, 7) FOREACH_END(0);
	static const u64 many_args	= FOREACH(FOREACH_BIT, 1, 2, 3, 15, 30, 63) FOREACH_END(0);

	KUNIT_EXPECT_EQ(test, empty_args, 0);
	KUNIT_EXPECT_EQ(test, one_arg, BIT(7));
	KUNIT_EXPECT_EQ(test, many_args, BIT(1) | BIT(2) | BIT(3) | BIT(15) | BIT(30) | BIT(63));

#undef FOREACH_BIT
}

static void foreach_s1(struct kunit *test)
{
#define FOREACH_CONDITIONAL_BIT(s1, x) (x >= s1 ? BIT(x) : 0) |

	static const u64 exp = FOREACH_S1(5, FOREACH_CONDITIONAL_BIT,
					  3, 4, 5, 6) FOREACH_END(0);

	KUNIT_EXPECT_EQ(test, exp, BIT(5) | BIT(6));

#undef FOREACH_CONDITIONAL_BIT
}

static void foreach_s2(struct kunit *test)
{
#define FOREACH_CONDITIONAL_BIT(s1, s2, x) (x >= s1 && x <= s2 ? BIT(x) : 0) |

	static const u64 exp = FOREACH_S2(4, 5, FOREACH_CONDITIONAL_BIT,
					  3, 4, 5, 6) FOREACH_END(0);

	KUNIT_EXPECT_EQ(test, exp, BIT(4) | BIT(5));

#undef FOREACH_CONDITIONAL_BIT
}

static struct kunit_case foreach_macros_test_cases[] = {
	KUNIT_CASE(foreach),
	KUNIT_CASE(foreach_s1),
	KUNIT_CASE(foreach_s2),
	{}
};

static struct kunit_suite foreach_macros_test_suite = {
	.name		= "foreach_macros",
	.test_cases	= foreach_macros_test_cases,
};

kunit_test_suites(&foreach_macros_test_suite);

MODULE_DESCRIPTION("Test cases for linux/foreach_macros.h");
MODULE_AUTHOR("Thomas Weißschuh <linux@weissschuh.net");
MODULE_LICENSE("GPL");
