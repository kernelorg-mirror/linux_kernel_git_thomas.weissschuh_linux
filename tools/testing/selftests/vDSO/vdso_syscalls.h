/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2025 Thomas Weißschuh <thomas.weissschuh@linutronix.de>, Linutronix GmbH
 *
 * System call wrappers to use for vDSO testing.
 *
 * vDSO calls are expected to return the same data as the equivalent system call.
 * To ensure this the tests need to trigger system calls. Calling into libc may
 * silently use the vDSO, so explicit system calls are necessary.
 * Not all system calls are available on all platforms, so some fallback logic
 * is needed. Use __NR_ constants from the kernel's UAPI headers over SYS_ from
 * ones from libc to avoid any potential interference from libc.
 * Always prefer the 64-bit time variants of the system calls as 32-bit ones
 * may not be present on the platform or disabled in the kernel configuration.
 */
#ifndef __VDSO_SYSCALLS_H__
#define __VDSO_SYSCALLS_H__

#include "vdso_types.h"

#include <stddef.h>
#include <sys/syscall.h>
#include <linux/unistd.h>

#define typeof_member(T, m)	typeof(((T*)0)->m)
#define sizeof_member(T, m)	sizeof(typeof_member(T, m))

/*
 * To keep the fallback logic simple we assume that, although the types between
 * the wrapper and the system call are different, they are compatible.
 * Validate that assumption.
 */
#define ASSERT_MEMBER_COMPATIBLE(T1, T2, m)					\
	do {									\
		_Static_assert(sizeof_member(T1, m) == sizeof_member(T2, m));	\
		_Static_assert(offsetof(T1, m) == offsetof(T2, m));		\
	} while(0)

#define ASSERT_TIMESPEC_COMPATIBLE(ts1, ts2)					\
	do {									\
		_Static_assert(sizeof(ts1) == sizeof(ts2));			\
		ASSERT_MEMBER_COMPATIBLE(typeof(ts1), typeof(ts2), tv_sec);	\
		ASSERT_MEMBER_COMPATIBLE(typeof(ts1), typeof(ts2), tv_nsec);	\
	} while(0)

static inline
int sys_clock_getres(__kernel_clockid_t clock, struct __kernel_timespec *ts)
{
#ifdef __NR_clock_getres_time64
	return syscall(__NR_clock_getres_time64, clock, ts);
#else
	ASSERT_TIMESPEC_COMPATIBLE(*ts, struct __kernel_old_timespec);
	return syscall(__NR_clock_getres, clock, ts);
#endif
}

static inline
int sys_clock_gettime(__kernel_clockid_t clock, struct __kernel_timespec *ts)
{
#ifdef __NR_clock_gettime64
	return syscall(__NR_clock_gettime64, clock, ts);
#else
	ASSERT_TIMESPEC_COMPATIBLE(*ts, struct __kernel_old_timespec);
	return syscall(__NR_clock_gettime, clock, ts);
#endif
}

static inline
int sys_gettimeofday(struct __kernel_old_timeval *tv, struct kernel_timezone *tz)
{
#ifdef __NR_gettimeofday
	return syscall(__NR_gettimeofday, tv, tz);
#else
	errno = ENOSYS;
	return -1;
#endif
}

static inline
__kernel_old_time_t sys_time(__kernel_old_time_t *tloc)
{
#ifdef __NR_time
	return syscall(__NR_time, tloc);
#else
	errno = ENOSYS;
	return -1;
#endif
}

#endif /* __VDSO_SYSCALLS_H__ */
