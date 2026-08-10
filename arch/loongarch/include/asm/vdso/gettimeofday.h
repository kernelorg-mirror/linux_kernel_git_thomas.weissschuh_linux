/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Author: Huacai Chen <chenhuacai@loongson.cn>
 *
 * Copyright (C) 2020-2022 Loongson Technology Corporation Limited
 */
#ifndef __ASM_VDSO_GETTIMEOFDAY_H
#define __ASM_VDSO_GETTIMEOFDAY_H

#ifndef __ASSEMBLER__

#include <asm/unistd.h>
#include <asm/vdso/vdso.h>

#define VDSO_HAS_CLOCK_GETRES		1

#if __BITS_PER_LONG == 64

#define __VDSO_SYSCALL_NR(_32, _64) _64

static __always_inline long gettimeofday_fallback(
				struct __kernel_old_timeval *_tv,
				struct timezone *_tz)
{
	register struct __kernel_old_timeval *tv asm("a0") = _tv;
	register struct timezone *tz asm("a1") = _tz;
	register long nr asm("a7") = __NR_gettimeofday;
	register long ret asm("a0");

	asm volatile(
	"       syscall 0\n"
	: "=r" (ret)
	: "r" (nr), "r" (tv), "r" (tz)
	: "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
	  "$t8", "memory");

	return ret;
}

static __always_inline u64 __arch_get_hw_counter(s32 clock_mode,
						 const struct vdso_time_data *vd)
{
	uint64_t count;

	__asm__ __volatile__(
	"	rdtime.d %0, $zero\n"
	: "=r" (count));

	return count;
}

#else /* __BITS_PER_LONG == 32 */

#define __VDSO_SYSCALL_NR(_32, _64) _32

static __always_inline
long clock_gettime32_fallback(clockid_t _clkid, struct old_timespec32 *_ts)
{
	BUILD_BUG();
}

static __always_inline
long clock_getres32_fallback(clockid_t _clkid, struct old_timespec32 *_ts)
{
	BUILD_BUG();
}

static __always_inline
long gettimeofday_fallback(struct __kernel_old_timeval *_tv, struct timezone *_tz)
{
	BUILD_BUG();
}

static __always_inline u64 __arch_get_hw_counter(s32 clock_mode,
						 const struct vdso_time_data *vd)
{
	uint32_t lo, hi, check;

	do {
		__asm__ __volatile__(
		"	rdtimeh.w %0, $zero\n"
		: "=r" (hi));

		__asm__ __volatile__(
		"	rdtimel.w %0, $zero\n"
		: "=r" (lo));

		__asm__ __volatile__(
		"	rdtimeh.w %0, $zero\n"
		: "=r" (check));
	} while (unlikely(hi != check));

	return ((u64)hi << 32) | lo;
}

#endif /* __BITS_PER_LONG */

static __always_inline long clock_gettime_fallback(
					clockid_t _clkid,
					struct __kernel_timespec *_ts)
{
	register clockid_t clkid asm("a0") = _clkid;
	register struct __kernel_timespec *ts asm("a1") = _ts;
	register long nr asm("a7") = __VDSO_SYSCALL_NR(__NR_clock_gettime64, __NR_clock_gettime);
	register long ret asm("a0");

	asm volatile(
	"       syscall 0\n"
	: "=r" (ret)
	: "r" (nr), "r" (clkid), "r" (ts)
	: "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
	  "$t8", "memory");

	return ret;
}

static __always_inline int clock_getres_fallback(
					clockid_t _clkid,
					struct __kernel_timespec *_ts)
{
	register clockid_t clkid asm("a0") = _clkid;
	register struct __kernel_timespec *ts asm("a1") = _ts;
	register long nr asm("a7") = __VDSO_SYSCALL_NR(__NR_clock_getres_time64, __NR_clock_getres);
	register long ret asm("a0");

	asm volatile(
	"       syscall 0\n"
	: "=r" (ret)
	: "r" (nr), "r" (clkid), "r" (ts)
	: "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
	  "$t8", "memory");

	return ret;
}

#endif /* !__ASSEMBLER__ */

#endif /* __ASM_VDSO_GETTIMEOFDAY_H */
