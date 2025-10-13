// SPDX-License-Identifier: GPL-2.0
/*
 * ldt_gdt.c - Test cases for LDT and GDT access
 * Copyright (c) 2011-2015 Andrew Lutomirski
 */

#define _GNU_SOURCE

#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <dlfcn.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <stdbool.h>
#include <limits.h>

#include "vdso_config.h"
#include "vdso_call.h"
#include "../kselftest.h"
#include "../clock-helpers.h"

static const char **name;

#ifndef __NR_clock_gettime64
#define __NR_clock_gettime64	403
#endif

#ifndef __kernel_timespec
struct __kernel_timespec {
	long long	tv_sec;
	long long	tv_nsec;
};
#endif

/* max length of lines in /proc/self/maps - anything longer is skipped here */
#define MAPS_LINE_LEN 128

int nerrs = 0;

typedef int (*vgettime_t)(clockid_t, struct timespec *);

vgettime_t vdso_clock_gettime;

typedef int (*vgettime64_t)(clockid_t, struct __kernel_timespec *);

vgettime64_t vdso_clock_gettime64;

typedef long (*vgtod_t)(struct timeval *tv, struct timezone *tz);

vgtod_t vdso_gettimeofday;

typedef long (*getcpu_t)(unsigned *, unsigned *, void *);

getcpu_t vgetcpu;
getcpu_t vdso_getcpu;

static void *vsyscall_getcpu(void)
{
#ifdef __x86_64__
	FILE *maps;
	char line[MAPS_LINE_LEN];
	bool found = false;

	maps = fopen("/proc/self/maps", "r");
	if (!maps) /* might still be present, but ignore it here, as we test vDSO not vsyscall */
		return NULL;

	while (fgets(line, MAPS_LINE_LEN, maps)) {
		char r, x;
		void *start, *end;
		char name[MAPS_LINE_LEN];

		/* sscanf() is safe here as strlen(name) >= strlen(line) */
		if (sscanf(line, "%p-%p %c-%cp %*x %*x:%*x %*u %s",
			   &start, &end, &r, &x, name) != 5)
			continue;

		if (strcmp(name, "[vsyscall]"))
			continue;

		/* assume entries are OK, as we test vDSO here not vsyscall */
		found = true;
		break;
	}

	fclose(maps);

	if (!found) {
		ksft_print_msg("Warning: failed to find vsyscall getcpu\n");
		return NULL;
	}
	return (void *) (0xffffffffff600800);
#else
	return NULL;
#endif
}


static void fill_function_pointers(void)
{
	void *vdso = dlopen("linux-vdso.so.1",
			    RTLD_LAZY | RTLD_LOCAL | RTLD_NOLOAD);
	if (!vdso)
		vdso = dlopen("linux-gate.so.1",
			      RTLD_LAZY | RTLD_LOCAL | RTLD_NOLOAD);
	if (!vdso)
		vdso = dlopen("linux-vdso32.so.1",
			      RTLD_LAZY | RTLD_LOCAL | RTLD_NOLOAD);
	if (!vdso)
		vdso = dlopen("linux-vdso64.so.1",
			      RTLD_LAZY | RTLD_LOCAL | RTLD_NOLOAD);
	if (!vdso) {
		ksft_print_msg("Warning: failed to find vDSO\n");
		return;
	}

	vdso_getcpu = (getcpu_t)dlsym(vdso, name[4]);
	if (!vdso_getcpu)
		ksft_print_msg("Warning: failed to find getcpu in vDSO\n");

	vgetcpu = (getcpu_t) vsyscall_getcpu();

	vdso_clock_gettime = (vgettime_t)dlsym(vdso, name[1]);
	if (!vdso_clock_gettime)
		ksft_print_msg("Warning: failed to find clock_gettime in vDSO\n");

#if defined(VDSO_32BIT)
	vdso_clock_gettime64 = (vgettime64_t)dlsym(vdso, name[5]);
	if (!vdso_clock_gettime64)
		ksft_print_msg("Warning: failed to find clock_gettime64 in vDSO\n");
#endif

	vdso_gettimeofday = (vgtod_t)dlsym(vdso, name[0]);
	if (!vdso_gettimeofday)
		ksft_print_msg("Warning: failed to find gettimeofday in vDSO\n");

}

static long sys_getcpu(unsigned * cpu, unsigned * node,
		       void* cache)
{
	return syscall(__NR_getcpu, cpu, node, cache);
}

static inline int sys_clock_gettime(clockid_t id, struct timespec *ts)
{
	return syscall(__NR_clock_gettime, id, ts);
}

static inline int sys_clock_gettime64(clockid_t id, struct __kernel_timespec *ts)
{
	return syscall(__NR_clock_gettime64, id, ts);
}

static inline int sys_gettimeofday(struct timeval *tv, struct timezone *tz)
{
	return syscall(__NR_gettimeofday, tv, tz);
}

static void test_getcpu(void)
{
	ksft_print_msg("Testing getcpu...\n");

	for (int cpu = 0; ; cpu++) {
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(cpu, &cpuset);
		if (sched_setaffinity(0, sizeof(cpuset), &cpuset) != 0)
			break;

		unsigned cpu_sys, cpu_vdso, cpu_vsys,
			node_sys, node_vdso, node_vsys;
		long ret_sys, ret_vdso = 1, ret_vsys = 1;
		unsigned node;

		ret_sys = sys_getcpu(&cpu_sys, &node_sys, 0);
		if (vdso_getcpu)
			ret_vdso = VDSO_CALL(vdso_getcpu, 3, &cpu_vdso, &node_vdso, 0);
		if (vgetcpu)
			ret_vsys = vgetcpu(&cpu_vsys, &node_vsys, 0);

		if (!ret_sys)
			node = node_sys;
		else if (!ret_vdso)
			node = node_vdso;
		else if (!ret_vsys)
			node = node_vsys;

		bool ok = true;
		if (!ret_sys && (cpu_sys != cpu || node_sys != node))
			ok = false;
		if (!ret_vdso && (cpu_vdso != cpu || node_vdso != node))
			ok = false;
		if (!ret_vsys && (cpu_vsys != cpu || node_vsys != node))
			ok = false;

		ksft_print_msg("CPU %u:", cpu);
		if (!ret_sys)
			ksft_print_msg(" syscall: cpu %u, node %u", cpu_sys, node_sys);
		if (!ret_vdso)
			ksft_print_msg(" vdso: cpu %u, node %u", cpu_vdso, node_vdso);
		if (!ret_vsys)
			ksft_print_msg(" vsyscall: cpu %u, node %u", cpu_vsys, node_vsys);
		ksft_print_msg("\n");

		if (!ok) {
			ksft_test_result_fail("getcpu()\n");
			return;
		}
	}

	ksft_test_result_pass("getcpu()\n");
}

static bool ts_leq(const struct timespec *a, const struct timespec *b)
{
	if (a->tv_sec != b->tv_sec)
		return a->tv_sec < b->tv_sec;
	else
		return a->tv_nsec <= b->tv_nsec;
}

static bool ts64_leq(const struct __kernel_timespec *a,
		     const struct __kernel_timespec *b)
{
	if (a->tv_sec != b->tv_sec)
		return a->tv_sec < b->tv_sec;
	else
		return a->tv_nsec <= b->tv_nsec;
}

static bool tv_leq(const struct timeval *a, const struct timeval *b)
{
	if (a->tv_sec != b->tv_sec)
		return a->tv_sec < b->tv_sec;
	else
		return a->tv_usec <= b->tv_usec;
}

static const clockid_t vdso_clocks[] = {
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
	/* Also test some invalid clock ids */
	10 /* CLOCK_SGI_CYCLE */,
	-1,
	INT_MIN,
	INT_MAX,
};

static void test_one_clock_gettime(int clock)
{
	const char *name = clock_name(clock);
	struct timespec start, vdso, end;
	int vdso_ret, end_ret;

	if (!vdso_clock_gettime) {
		ksft_test_result_skip("%s: no __vdso_clock_gettime\n", name);
		return;
	}

	ksft_print_msg("Testing clock_gettime for clock %s (%d)...\n", name, clock);

	if (sys_clock_gettime(clock, &start) < 0) {
		if (errno == EINVAL) {
			vdso_ret = VDSO_CALL(vdso_clock_gettime, 2, clock, &vdso);
			if (vdso_ret == -EINVAL)
				ksft_test_result_skip("%s: no such clock\n", name);
			else
				ksft_test_result_fail("%s: no such clock, "
						      "but __vdso_clock_gettime returned %d\n",
						      name, vdso_ret);
		} else {
			ksft_test_result_skip("WARN clock_gettime(%s) syscall returned error %d\n",
					      name, errno);
		}
		return;
	}

	vdso_ret = VDSO_CALL(vdso_clock_gettime, 2, clock, &vdso);
	end_ret = sys_clock_gettime(clock, &end);

	if (vdso_ret != 0 || end_ret != 0) {
		ksft_test_result_fail("%s: vDSO returned %d, syscall errno=%d\n",
				      name, vdso_ret, errno);
		return;
	}

	ksft_print_msg("%llu.%09ld %llu.%09ld %llu.%09ld\n",
		       (unsigned long long)start.tv_sec, start.tv_nsec,
		       (unsigned long long)vdso.tv_sec, vdso.tv_nsec,
		       (unsigned long long)end.tv_sec, end.tv_nsec);

	if (!ts_leq(&start, &vdso) || !ts_leq(&vdso, &end)) {
		ksft_test_result_fail("%s: times are out of sequence\n", name);
		return;
	}

	ksft_test_result_pass("clock_gettime(%s)\n", name);
}

static void test_clock_gettime(void)
{
	for (int clock = 0; clock < ARRAY_SIZE(vdso_clocks); clock++)
		test_one_clock_gettime(vdso_clocks[clock]);
}

static void test_one_clock_gettime64(int clock)
{
	struct __kernel_timespec start, vdso, end;
	const char *name = clock_name(clock);
	int vdso_ret, end_ret;

	if (!vdso_clock_gettime64) {
		ksft_test_result_skip("%s: no __vdso_clock_gettime64\n", name);
		return;
	}

	ksft_print_msg("Testing clock_gettime64 for clock %s (%d)...\n", name, clock);

	if (sys_clock_gettime64(clock, &start) < 0) {
		if (errno == EINVAL) {
			vdso_ret = VDSO_CALL(vdso_clock_gettime64, 2, clock, &vdso);
			if (vdso_ret == -EINVAL)
				ksft_test_result_skip("%s: no such clock\n", clock_name(clock));
			else
				ksft_test_result_fail("%s: no such clock, "
						      "but __vdso_clock_gettime64 returned %d\n",
						      name, vdso_ret);
		} else {
			ksft_test_result_skip("%s: syscall returned error %d\n",
					      name, errno);
		}
		return;
	}

	vdso_ret = VDSO_CALL(vdso_clock_gettime64, 2, clock, &vdso);
	end_ret = sys_clock_gettime64(clock, &end);

	if (vdso_ret != 0 || end_ret != 0) {
		ksft_test_result_fail("%s: vDSO returned %d, syscall errno=%d\n",
				      name, vdso_ret, errno);
		return;
	}

	ksft_print_msg("%llu.%09lld %llu.%09lld %llu.%09lld\n",
		       (unsigned long long)start.tv_sec, start.tv_nsec,
		       (unsigned long long)vdso.tv_sec, vdso.tv_nsec,
		       (unsigned long long)end.tv_sec, end.tv_nsec);

	if (!ts64_leq(&start, &vdso) || !ts64_leq(&vdso, &end)) {
		ksft_test_result_fail("%s times are out of sequence\n", name);
		return;
	}

	ksft_test_result_pass("clock_gettime64(%s)\n", name);
}

static void test_clock_gettime64(void)
{
	for (int clock = 0; clock < ARRAY_SIZE(vdso_clocks); clock++)
		test_one_clock_gettime64(vdso_clocks[clock]);
}

static void test_gettimeofday(void)
{
	struct timeval start, vdso, end;
	struct timezone sys_tz, vdso_tz;
	int vdso_ret, end_ret;

	if (!vdso_gettimeofday) {
		ksft_test_result_skip("gettimeofday: no __vdso_gettimeofday\n");
		return;
	}

	ksft_print_msg("Testing gettimeofday...\n");

	if (sys_gettimeofday(&start, &sys_tz) < 0) {
		ksft_test_result_fail("sys_gettimeofday failed (%d)\n", errno);
		return;
	}

	vdso_ret = VDSO_CALL(vdso_gettimeofday, 2, &vdso, &vdso_tz);
	end_ret = sys_gettimeofday(&end, NULL);

	if (vdso_ret != 0 || end_ret != 0) {
		ksft_test_result_fail("gettimeofday: vDSO returned %d, syscall errno=%d\n",
				      vdso_ret, errno);
		return;
	}

	ksft_print_msg("\t%llu.%06ld %llu.%06ld %llu.%06ld\n",
		       (unsigned long long)start.tv_sec, start.tv_usec,
		       (unsigned long long)vdso.tv_sec, vdso.tv_usec,
		       (unsigned long long)end.tv_sec, end.tv_usec);

	if (!tv_leq(&start, &vdso) || !tv_leq(&vdso, &end)) {
		ksft_test_result_fail("gettimeofday: times are out of sequence\n");
		return;
	}

	if (sys_tz.tz_minuteswest != vdso_tz.tz_minuteswest ||
	    sys_tz.tz_dsttime != vdso_tz.tz_dsttime) {
		ksft_test_result_fail("gettimeofday: timezones do not match\n");
	}

	/* And make sure that passing NULL for tz doesn't crash. */
	VDSO_CALL(vdso_gettimeofday, 2, &vdso, NULL);

	ksft_test_result_pass("gettimeofday()\n");
}

int main(int argc, char **argv)
{
	name = (const char **)&names[VDSO_NAMES];

	fill_function_pointers();

	ksft_print_header();
	ksft_set_plan(32);

	test_clock_gettime();
	test_clock_gettime64();
	test_gettimeofday();

	/*
	 * Test getcpu() last so that, if something goes wrong setting affinity,
	 * we still run the other tests.
	 */
	test_getcpu();

	ksft_finished();
}
