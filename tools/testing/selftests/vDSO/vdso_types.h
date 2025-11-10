/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2025 Thomas Weißschuh <thomas.weissschuh@linutronix.de>, Linutronix GmbH
 *
 * Types to use with vDSO functions.
 *
 * The types used by the vDSO functions do not necessarily match the ones used
 * by libc. The kernel's UAPI headers do provide definitions for those types
 * but are often not compatible with libc which applications need to use.
 * As the types differ between platforms listing them manually is a lot of work
 * and error prone. Instead hack around the incompatibilities of the libc and
 * UAPI headers and use the UAPI types.
 */
#ifndef __VDSO_TYPES_H__
#define __VDSO_TYPES_H__

/*
 * Include the headers from libc first to not override any of its types later.
 */
#include <time.h>
#include <sys/time.h>

/*
 * Avoid collisions.
 */
#define timeval kernel_timeval_moved
#define itimerspec kernel_itimerspec_moved
#define itimerval kernel_itimerval_moved
#define timezone kernel_timezone_moved

/*
 * Get the UAPI types.
 */
#include <linux/time.h>

#undef timeval
#undef itimerspec
#undef itimerval
#undef timezone

/*
 * The UAPI headers do not provide their own 'struct __kernel_timezone'.
 * 'struct timezone' is the one from libc.
 */
struct kernel_timezone {
	int	tz_minuteswest;
	int	tz_dsttime;
};

#include <linux/version.h>

/*
 * UAPI headers from the libc may be older and not provide these.
 */
#if KERNEL_VERSION(5, 5, 0) > LINUX_VERSION_CODE
typedef __kernel_long_t		__kernel_old_time_t;

struct __kernel_old_timespec {
	__kernel_old_time_t	tv_sec;
	__kernel_long_t		tv_nsec;
};
#endif

typedef long (*vdso_gettimeofday_t)(struct __kernel_old_timeval *tv, struct kernel_timezone *tz);
typedef long (*vdso_clock_gettime_t)(__kernel_clockid_t clk_id, struct __kernel_old_timespec *ts);
typedef long (*vdso_clock_gettime64_t)(__kernel_clockid_t clk_id, struct __kernel_timespec *ts);
typedef long (*vdso_clock_getres_t)(__kernel_clockid_t clk_id, struct __kernel_old_timespec *ts);
typedef long (*vdso_clock_getres_time64_t)(__kernel_clockid_t clk_id, struct __kernel_timespec *ts);
typedef __kernel_old_time_t (*vdso_time_t)(__kernel_old_time_t *t);

#endif /* __VDSO_TYPES_H__ */
