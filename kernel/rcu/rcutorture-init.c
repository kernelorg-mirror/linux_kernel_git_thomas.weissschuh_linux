// SPDX-License-Identifier: GPL-2.0+

#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

volatile unsigned long delaycount;

int main(int argc, char *argv[])
{
	int i;
	struct timeval tv;
	struct timeval tvb;

	printf("Torture-test rudimentary init program started, command line:\n");
	for (i = 0; i < argc; i++)
		printf(" %s", argv[i]);
	printf("\n");
	for (;;) {
		sleep(1);
		/* Need some userspace time. */
		if (gettimeofday(&tvb, NULL))
			continue;
		do {
			for (i = 0; i < 1000 * 100; i++)
				delaycount = i * i;
			if (gettimeofday(&tv, NULL))
				break;
			tv.tv_sec -= tvb.tv_sec;
			if (tv.tv_sec > 1)
				break;
			tv.tv_usec += tv.tv_sec * 1000 * 1000;
			tv.tv_usec -= tvb.tv_usec;
		} while (tv.tv_usec < 1000);
	}
	return 0;
}
