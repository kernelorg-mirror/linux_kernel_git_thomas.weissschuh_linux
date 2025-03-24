#include <linux/init.h>

#include <kunit/test.h>
#include <kunit/uapi.h>

static void clock_nanosleep(struct kunit *test)
{
	if (IS_ENABLED(CONFIG_CC_CAN_LINK_STATIC)) {
		KUNIT_UAPI_EMBED_BLOB(timens_test_clock_nanosleep, "timens_test_clock_nanosleep");
		kunit_uapi_run_kselftest(test, &timens_test_clock_nanosleep);
	} else {
		kunit_skip(test, "no nolibc");
	}
}

static void exec(struct kunit *test)
{
	KUNIT_UAPI_EMBED_BLOB(timens_test_exec, "timens_test_exec");

	kunit_uapi_run_kselftest(test, &timens_test_exec);
}

static void futex(struct kunit *test)
{
	KUNIT_UAPI_EMBED_BLOB(timens_test_futex, "timens_test_futex");

	kunit_uapi_run_kselftest(test, &timens_test_futex);
}

static void gettime_perf(struct kunit *test)
{

	if (IS_ENABLED(CONFIG_CC_CAN_LINK_STATIC)) {
		KUNIT_UAPI_EMBED_BLOB(timens_test_gettime_perf, "timens_test_gettime_perf");
		kunit_uapi_run_kselftest(test, &timens_test_gettime_perf);
	} else {
		kunit_skip(test, "no nolibc");
	}
}

static void procfs(struct kunit *test)
{
	KUNIT_UAPI_EMBED_BLOB(timens_test_procfs, "timens_test_procfs");

	kunit_uapi_run_kselftest(test, &timens_test_procfs);
}

static void timens(struct kunit *test)
{
	if (IS_ENABLED(CONFIG_CC_CAN_LINK_STATIC)) {
		KUNIT_UAPI_EMBED_BLOB(timens_test_timens, "timens_test_timens");
		kunit_uapi_run_kselftest(test, &timens_test_timens);
	} else {
		kunit_skip(test, "no nolibc");
	}
}

static void timer(struct kunit *test)
{
	KUNIT_UAPI_EMBED_BLOB(timens_test_timer, "timens_test_timer");

	kunit_uapi_run_kselftest(test, &timens_test_timer);
}

static void timerfd(struct kunit *test)
{
	KUNIT_UAPI_EMBED_BLOB(timens_test_timerfd, "timens_test_timerfd");

	kunit_uapi_run_kselftest(test, &timens_test_timerfd);
}

static void vfork_exec(struct kunit *test)
{
	if (IS_ENABLED(CONFIG_CC_CAN_LINK_STATIC)) {
		KUNIT_UAPI_EMBED_BLOB(timens_test_vfork_exec, "timens_test_vfork_exec");
		kunit_uapi_run_kselftest(test, &timens_test_vfork_exec);
	} else {
		kunit_skip(test, "no nolibc");
	}
}

static struct kunit_case timens_test_cases[] = {
	KUNIT_CASE(clock_nanosleep),
	KUNIT_CASE(exec),
	KUNIT_CASE(futex),
	KUNIT_CASE_SLOW(gettime_perf),
	KUNIT_CASE(procfs),
	KUNIT_CASE(timens),
	KUNIT_CASE(timer),
	KUNIT_CASE(timerfd),
	KUNIT_CASE(vfork_exec),
	{}
};

static struct kunit_suite timens_test_suite = {
	.name		= "timens",
	.test_cases	= timens_test_cases,
};
kunit_test_suites(&timens_test_suite);
