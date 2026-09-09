#include <linux/init.h>

#include <kunit/test.h>

#if !IS_ENABLED(CONFIG_KUNIT)
#undef kunit_info
#define kunit_info(test, ...)	pr_info(__VA_ARGS__)

#undef KUNIT_FAIL
#define KUNIT_FAIL(test, ...)	do {	\
	pr_warn(__VA_ARGS__);		\
	return;				\
} while (0)

#undef kunit_test_suite
#define kunit_test_suite(suite)				\
static int __init init(void)				\
{							\
	struct kunit_case *c;				\
							\
	for (c = (suite).test_cases; c->name; c++)	\
		c->run_case(NULL);			\
							\
	return 0;					\
}							\
module_init(init);					\
							\
static void __exit exit_module(void)			\
{							\
}							\
module_exit(exit_module);				\

#endif /* CONFIG_KUNIT */
