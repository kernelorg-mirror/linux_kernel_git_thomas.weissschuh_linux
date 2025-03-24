#include <linux/init.h>

#include <kunit/test.h>
#include <kunit/uapi.h>

static void posix_timers(struct kunit *test)
{
	if (IS_ENABLED(CONFIG_CC_CAN_LINK_STATIC)) {
		KUNIT_UAPI_EMBED_BLOB(timer_test_posix_timers, "timer_test_posix_timers");
		kunit_uapi_run_kselftest(test, &timer_test_posix_timers);
	} else {
		kunit_skip(test, "foo");
	}
}

static struct kunit_case timer_test_cases[] = {
	KUNIT_CASE_SLOW(posix_timers),
	{}
};

static struct kunit_suite timer_test_suite = {
	.name		= "timer",
	.test_cases	= timer_test_cases,
};
kunit_test_suites(&timer_test_suite);
