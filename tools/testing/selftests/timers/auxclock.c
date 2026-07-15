// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 Linutronix GmbH, Thomas Weißschuh <thomas.weissschuh@linutronix.de>
 *
 * Selftests for auxiliary clock peculiarities.
 */
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/wait.h>

#include "clock-helpers.h"
#include "kselftest.h"

static int configure_auxclock(clockid_t clock, bool enable)
{
	char enable_path[100];
	int saved_errno;
	ssize_t w;
	int fd;

	sprintf(enable_path, "/sys/kernel/time/aux_clocks/%d/aux_clock_enable", clock - CLOCK_AUX);

	fd = open(enable_path, O_RDWR);
	if (fd == -1)
		return errno;

	w = write(fd, enable ? "1" : "0", 1);
	saved_errno = errno;
	close(fd);
	if (w != 1)
		return saved_errno;

	return 0;
}

#define CONFIGURE_AUXCLOCK_OR_RETURN(_clock, _enable) do {		\
	int ret;							\
									\
	/* Disable first for a clean slate. */				\
	ret = configure_auxclock((_clock), false);			\
	if (!ret && (_enable))						\
		ret = configure_auxclock((_clock), true);		\
									\
	if (ret == ENOENT || ret == EACCES) {				\
		ksft_test_result_skip("%s: configure_clock(): %s\n",	\
				      __func__, strerror(ret));		\
		return;							\
	} else if (ret) {						\
		ksft_test_result_fail("%s: configure_clock(): %s\n",	\
				      __func__, strerror(ret));		\
		return;							\
	}								\
} while (0)

#define CLOCK_GETTIME_OR_RETURN(_clock, _ts) do {			\
	int ret = clock_gettime(_clock, _ts);				\
	if (ret) {							\
		ksft_test_result_fail("%s: clock_gettime(): %s\n",	\
				      __func__, strerror(ret));		\
		return;							\
	}								\
} while(0)

static inline int64_t calcdiff_ns(const struct timespec *t1, const struct timespec *t2)
{
	int64_t diff;

	diff = NSEC_PER_SEC * (int64_t)((int) t1->tv_sec - (int) t2->tv_sec);
	diff += ((int) t1->tv_nsec - (int) t2->tv_nsec);
	return diff;
}

#define VALIDATE_TIMESPEC_DIFFERENCE_NS(_ns, _first, _second) do {			\
	int64_t diff_ns;								\
											\
	diff_ns = calcdiff_ns((_second), (_first));					\
											\
	ksft_print_msg("%s wakeup took %"PRId64"ns\n",	__func__, diff_ns);		\
											\
	if (diff_ns < 0 || (diff_ns > (_ns))) {						\
		ksft_test_result_fail("%s wakeup took too long\n", __func__);		\
		return;									\
	}										\
} while(0)


/* Sleeping on a currently disabled clock returns instantly. */
static void test_nanosleep_disabled_clock(clockid_t clock)
{
	struct timespec aux_sleep, start, end;
	int ret;

	CONFIGURE_AUXCLOCK_OR_RETURN(clock, false);
	CLOCK_GETTIME_OR_RETURN(CLOCK_MONOTONIC, &start);

	/* Long timeout to catch cases where the abortion does not work */
	aux_sleep.tv_sec = 100000;
	aux_sleep.tv_nsec = 0;
	ret = clock_nanosleep(clock, 0, &aux_sleep, NULL);
	if (ret != ENODEV) {
		ksft_test_result_fail("%s: clock_nanosleep() unexpected return value: %d/%s\n",
				      __func__, ret, strerror(ret));
		return;
	}

	CLOCK_GETTIME_OR_RETURN(CLOCK_MONOTONIC, &end);
	VALIDATE_TIMESPEC_DIFFERENCE_NS(20 * NSEC_PER_MSEC, &start, &end);

	ksft_test_result_pass("%s\n", __func__);
}

/* In-progress sleeps return as soon as the clock is disabled. */
static void test_nanosleep_disabled_clock_while_sleeping(clockid_t clock)
{
	struct timespec mono_expiration, mono_now, aux_sleep;
	int wait_status;
	pid_t pid;
	int ret;

	CONFIGURE_AUXCLOCK_OR_RETURN(clock, true);

	CLOCK_GETTIME_OR_RETURN(CLOCK_MONOTONIC, &mono_expiration);
	/* Wait a bit before disabling the clock */
	mono_expiration.tv_sec += 2;
	mono_expiration.tv_nsec = 0;

	pid = fork();
	if (pid == -1) {
		ksft_test_result_fail("%s: fork(): %m\n", __func__);
		return;
	}

	if (pid == 0) {
		ret = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &mono_expiration, NULL);
		if (ret)
			exit(ret);

		ret = configure_auxclock(clock, false);
		exit(ret);
	}

	/* Long timeout to catch cases where the abortion does not work */
	aux_sleep.tv_sec = 100000;
	aux_sleep.tv_nsec = 0;
	ret = clock_nanosleep(clock, 0, &aux_sleep, NULL);
	if (ret != ENODEV) {
		ksft_test_result_fail("%s: clock_nanosleep() unexpected return value: %d/%s\n",
				      __func__, ret, strerror(ret));
		return;
	}

	pid = waitpid(pid, &wait_status, 0);
	if (pid == -1 || !WIFEXITED(wait_status) || WEXITSTATUS(wait_status)) {
		ksft_test_result_fail("%s: waitpid()\n", __func__);
		return;
	}

	CLOCK_GETTIME_OR_RETURN(CLOCK_MONOTONIC, &mono_now);
	VALIDATE_TIMESPEC_DIFFERENCE_NS(20 * NSEC_PER_MSEC, &mono_expiration, &mono_now);

	ksft_test_result_pass("%s\n", __func__);
}

int main(int argc, char **argv)
{
	clockid_t clock = CLOCK_AUX + 0;

	ksft_print_header();
	ksft_set_plan(2);

	test_nanosleep_disabled_clock(clock);
	test_nanosleep_disabled_clock_while_sleeping(clock);
	ksft_finished();
}
