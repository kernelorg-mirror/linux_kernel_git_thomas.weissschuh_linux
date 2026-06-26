#define _GNU_SOURCE

#include <time.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/timex.h>
#include <sys/wait.h>

#define CLOCK_AUX 16

static void print_now(const struct timespec *now_aux)
{
	struct timespec now_mono;
	int ret;

	ret = clock_gettime(CLOCK_MONOTONIC, &now_mono);
	if (ret)
		err(1, "clock_gettime()");

	printf("Aux:  %llu.%09llu\n", (unsigned long long)now_aux->tv_sec, (unsigned long long)now_aux->tv_nsec);
	printf("Mono: %llu.%09llu\n", (unsigned long long)now_mono.tv_sec, (unsigned long long)now_mono.tv_nsec);
}

int main(void)
{
	const clockid_t clock = CLOCK_AUX + 0;
	struct timex timex = {};
	struct timespec ts;
	int ret, status;

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	if (1) {
		ret = clock_adjtime(CLOCK_REALTIME, &timex);
		if (ret == -1)
			err(1, "clock_adjtime() read");

		/*
		timex.modes = ADJ_TICK;
		timex.tick = 6000;
		ret = clock_adjtime(CLOCK_REALTIME, &timex);
		if (ret == -1)
			err(1, "clock_adjtime() write realtime");
	*/

		timex.modes = ADJ_TICK;
		timex.tick = 11000;

		ret = clock_adjtime(clock, &timex);
		if (ret == -1)
			err(1, "clock_adjtime() write");

		printf("Frequency CLOCK_AUX: %ld\n", timex.freq);
	}

	ret = clock_gettime(clock, &ts);
	if (ret)
		err(1, "clock_gettime()");
	print_now(&ts);

	ts.tv_sec += 2;
	ts.tv_nsec = 0;
	printf("Sleeping absolute until %llu.%09llu\n", (unsigned long long)ts.tv_sec, (unsigned long long)ts.tv_nsec);
	ret = clock_nanosleep(clock, TIMER_ABSTIME, &ts, NULL);
	errno = ret;
	if (ret)
		err(1, "clock_nanosleep()");

	ret = clock_gettime(clock, &ts);
	if (ret)
		err(1, "clock_gettime()");
	print_now(&ts);

	printf("Sleeping relative for 2 seconds\n");

	ts.tv_sec = 2;
	ts.tv_nsec = 0;
	ret = clock_nanosleep(clock, 0, &ts, &ts);
	if (ret)
		err(1, "clock_nanosleep()");

	ret = clock_gettime(clock, &ts);
	if (ret)
		err(1, "clock_gettime()");
	print_now(&ts);

	/* Test adjtime() */
	pid_t pid = fork();
	if (pid == -1)
		err(1, "fork()");

	if (pid == 0) {
		/* In the child */
		ts.tv_sec = 1;
		ts.tv_nsec = 0;
		ret = clock_nanosleep(clock, 0, &ts, NULL);
		if (ret)
			exit(1);

		struct timex timex = {
			.modes = ADJ_SETOFFSET,
			.time = {
				.tv_sec = 95,
			},
		};
		ret = clock_adjtime(clock, &timex);
		if (ret == -1)
			exit(errno);

		exit(0);
	}

	ts.tv_sec += 100;
	ts.tv_nsec = 0;
	printf("Sleeping to %lld seconds\n", (long long)ts.tv_sec);
	ret = clock_nanosleep(clock, TIMER_ABSTIME, &ts, NULL);
	errno = ret;
	if (ret)
		err(1, "clock_nanosleep()");

	ret = waitpid(pid, &status, 0);
	if (ret == -1)
		err(1, "waitpid()");

	if (!WIFEXITED(status))
		errx(1, "child did not exit");

	if (WEXITSTATUS(status) != 0)
		errx(1, "child exit=%d", WEXITSTATUS(status));

	ret = clock_gettime(clock, &ts);
	if (ret)
		err(1, "clock_gettime()");
	print_now(&ts);

	return 0;
}

