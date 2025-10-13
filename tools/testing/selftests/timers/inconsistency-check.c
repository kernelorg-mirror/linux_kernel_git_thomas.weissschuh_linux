/* Time inconsistency check test
 *		by: john stultz (johnstul@us.ibm.com)
 *		(C) Copyright IBM 2003, 2004, 2005, 2012
 *		(C) Copyright Linaro Limited 2015
 *		Licensed under the GPLv2
 *
 *  To build:
 *	$ gcc inconsistency-check.c -o inconsistency-check -lrt
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 */



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <string.h>
#include <signal.h>
#include <include/vdso/time64.h>
#include "../kselftest.h"
#include "../clock-helpers.h"

#define CALLS_PER_LOOP 64

/* returns 1 if a <= b, 0 otherwise */
static inline int in_order(struct timespec a, struct timespec b)
{
	/* use unsigned to avoid false positives on 2038 rollover */
	if ((unsigned long)a.tv_sec < (unsigned long)b.tv_sec)
		return 1;
	if ((unsigned long)a.tv_sec > (unsigned long)b.tv_sec)
		return 0;
	if (a.tv_nsec > b.tv_nsec)
		return 0;
	return 1;
}



int consistency_test(int clock_type, unsigned long seconds)
{
	struct timespec list[CALLS_PER_LOOP];
	int i, inconsistent;
	long now, then;
	time_t t;
	char *start_str;

	clock_gettime(clock_type, &list[0]);
	now = then = list[0].tv_sec;

	/* timestamp start of test */
	t = time(0);
	start_str = ctime(&t);

	while (seconds == -1 || now - then < seconds) {
		inconsistent = -1;

		/* Fill list */
		for (i = 0; i < CALLS_PER_LOOP; i++)
			clock_gettime(clock_type, &list[i]);

		/* Check for inconsistencies */
		for (i = 0; i < CALLS_PER_LOOP - 1; i++)
			if (!in_order(list[i], list[i+1]))
				inconsistent = i;

		/* display inconsistency */
		if (inconsistent >= 0) {
			unsigned long long delta;

			ksft_print_msg("\%s\n", start_str);
			for (i = 0; i < CALLS_PER_LOOP; i++) {
				if (i == inconsistent)
					ksft_print_msg("--------------------\n");
				ksft_print_msg("%lu:%lu\n", list[i].tv_sec,
							list[i].tv_nsec);
				if (i == inconsistent + 1)
					ksft_print_msg("--------------------\n");
			}
			delta = list[inconsistent].tv_sec * NSEC_PER_SEC;
			delta += list[inconsistent].tv_nsec;
			delta -= list[inconsistent+1].tv_sec * NSEC_PER_SEC;
			delta -= list[inconsistent+1].tv_nsec;
			ksft_print_msg("Delta: %llu ns\n", delta);
			fflush(0);
			/* timestamp inconsistency*/
			t = time(0);
			ksft_print_msg("%s\n", ctime(&t));
			return -1;
		}
		now = list[0].tv_sec;
	}
	return 0;
}

static const clockid_t all_clocks[] = {
	CLOCK_REALTIME,
	CLOCK_MONOTONIC,
	CLOCK_PROCESS_CPUTIME_ID,
	CLOCK_THREAD_CPUTIME_ID,
	CLOCK_MONOTONIC_RAW,
	CLOCK_REALTIME_COARSE,
	CLOCK_MONOTONIC_COARSE,
	CLOCK_BOOTTIME,
	CLOCK_REALTIME_ALARM,
	CLOCK_BOOTTIME_ALARM,
	CLOCK_TAI,
};

int main(int argc, char *argv[])
{
	size_t num_tested_clocks = ARRAY_SIZE(all_clocks);
	const clockid_t *tested_clocks = all_clocks;
	clockid_t clockid, userclock;
	size_t clock_index;
	struct timespec ts;
	int runtime = 10;
	int opt;

	/* Process arguments */
	while ((opt = getopt(argc, argv, "t:c:")) != -1) {
		switch (opt) {
		case 't':
			runtime = atoi(optarg);
			break;
		case 'c':
			userclock = atoi(optarg);
			tested_clocks = &userclock;
			num_tested_clocks = 1;
			break;
		default:
			printf("Usage: %s [-t <secs>] [-c <clockid>]\n", argv[0]);
			printf("	-t: Number of seconds to run\n");
			printf("	-c: clockid to use (default, all clockids)\n");
			exit(-1);
		}
	}

	setbuf(stdout, NULL);

	ksft_print_header();
	ksft_set_plan(num_tested_clocks);

	for (clock_index = 0; clock_index < num_tested_clocks; clock_index++) {
		clockid = tested_clocks[clock_index];

		if (clock_gettime(clockid, &ts)) {
			ksft_test_result_skip("%-31s\n", clock_name(clockid));
			continue;
		}

		if (consistency_test(clockid, runtime)) {
			ksft_test_result_fail("%-31s\n", clock_name(clockid));
			ksft_exit_fail();
		} else {
			ksft_test_result_pass("%-31s\n", clock_name(clockid));
		}
	}
	ksft_exit_pass();
}
