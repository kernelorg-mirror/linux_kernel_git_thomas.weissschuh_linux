#include "../../../../../../tools/testing/selftests/kselftest.h"
#include "test_utils.h"

#include <sched.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void test_nanosleep(void)
{
	struct timespec sleep_time;
	pid_t pid;
	int ret;

	pid = fork();
	switch (pid) {
	case -1:
		ksft_test_result_fail("%s: failed to fork: %s\n", __func__, strerror(errno));
		return;
	case 0:
		sleep_time.tv_sec = 2;
		sleep_time.tv_nsec = 100;

		ret = clock_nanosleep(CLOCK_REALTIME, 0, &sleep_time, NULL);
		_exit(ret != 0);
	default:
		waitpid(pid, &ret, 0);
		ksft_print_msg("ret 0x%x\n", ret);
	}

	ksft_test_result(ret == 0, "%s\n", __func__);
}

static void become_realtime(void)
{
	struct sched_param p = {
		.sched_priority = 1,
	};
	int ret;

	ret = sched_setscheduler(0, SCHED_FIFO, &p);
	if (ret == -1) {
		ksft_print_header();
		ksft_set_plan(1);
		ksft_test_result_fail("Failed to become realtime: %s\n", strerror(errno));
		ksft_finished();
	}
}

int main(void)
{
	int ret;

	become_realtime();

	ret = rtapp_enable();
	if (ret) {
		ksft_print_header();
		ksft_set_plan(1);
		ksft_test_result_fail("Failed to enable rtapp: %s\n", strerror(errno));
		ksft_finished();
	}

	ksft_print_header();
	ksft_set_plan(1);

	test_nanosleep();

	ksft_finished();
}
