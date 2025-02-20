
#include <linux/blob.h>

#include <kunit/uapi.h>
#include <kunit/test.h>

static void auxclock(struct kunit *test)
{
	kunit_uapi_run_kselftest(test, BLOB(test_auxclock));
}

static struct kunit_case timer_test_cases[] = {
	KUNIT_CASE(auxclock),
	{}
};

static struct kunit_suite timer_suite = {
	.name = "timer",
	.test_cases = timer_test_cases,
};
kunit_test_suites(&timer_suite);
