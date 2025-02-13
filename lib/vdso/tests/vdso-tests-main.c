#include <linux/init.h>

#include <kunit/test.h>
#include <kunit/uapi.h>

static void abi(struct kunit *test)
{
	KUNIT_UAPI_EMBED_BLOB(vdso_test_abi, "vdso_test_abi");

	kunit_uapi_run_kselftest(test, &vdso_test_abi);
}

static void chacha(struct kunit *test)
{
	KUNIT_UAPI_EMBED_BLOB(vdso_test_chacha, "vdso_test_chacha");

	kunit_uapi_run_kselftest(test, &vdso_test_chacha);
}

static void clock_getres(struct kunit *test)
{
	if (IS_ENABLED(CONFIG_CC_CAN_LINK_STATIC)) {
		KUNIT_UAPI_EMBED_BLOB(vdso_test_clock_getres, "vdso_test_clock_getres");
		kunit_uapi_run_kselftest(test, &vdso_test_clock_getres);
	} else {
		kunit_skip(test, "nolibc does not call vDSO implicitly");
	}
}

static void correctness(struct kunit *test)
{

	if (IS_ENABLED(CONFIG_CC_CAN_LINK_STATIC)) {
		KUNIT_UAPI_EMBED_BLOB(vdso_test_correctness, "vdso_test_correctness");
		kunit_uapi_run_kselftest(test, &vdso_test_correctness);
	} else {
		kunit_skip(test, "test requires a real libc");
	}
}

static void getcpu(struct kunit *test)
{
	KUNIT_UAPI_EMBED_BLOB(vdso_test_getcpu, "vdso_test_getcpu");

	kunit_uapi_run_kselftest(test, &vdso_test_getcpu);
}

static void getrandom(struct kunit *test)
{

	if (IS_ENABLED(CONFIG_CC_CAN_LINK_STATIC)) {
		KUNIT_UAPI_EMBED_BLOB(vdso_test_getrandom, "vdso_test_getrandom");
		kunit_uapi_run_kselftest(test, &vdso_test_getrandom);
	} else {
		kunit_skip(test, "test requires a real libc");
	}
}

static void gettimeofday(struct kunit *test)
{
	KUNIT_UAPI_EMBED_BLOB(vdso_test_gettimeofday, "vdso_test_gettimeofday");

	kunit_uapi_run_kselftest(test, &vdso_test_gettimeofday);
}

static struct kunit_case vdso_test_cases[] = {
	KUNIT_CASE(abi),
	KUNIT_CASE_SLOW(chacha),
	KUNIT_CASE(clock_getres),
	KUNIT_CASE(correctness),
	KUNIT_CASE(getcpu),
	KUNIT_CASE(getrandom),
	KUNIT_CASE(gettimeofday),
	{}
};

static struct kunit_suite vdso_test_suite = {
	.name		= "vdso",
	.test_cases	= vdso_test_cases,
};
kunit_test_suites(&vdso_test_suite);
