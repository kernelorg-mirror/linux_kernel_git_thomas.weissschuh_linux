#include <linux/init.h>

#include <kunit/test.h>
#include <kunit/uapi.h>

static void mremap_vdso(struct kunit *test)
{
	KUNIT_UAPI_EMBED_BLOB(x86_test_mremap_vdso, "x86_test_mremap_vdso");

	kunit_uapi_run_kselftest(test, &x86_test_mremap_vdso);
}

static struct kunit_case x86_test_cases[] = {
	KUNIT_CASE(mremap_vdso),
	{}
};

static struct kunit_suite x86_test_suite = {
	.name		= "x86",
	.test_cases	= x86_test_cases,
};
kunit_test_suites(&x86_test_suite);
