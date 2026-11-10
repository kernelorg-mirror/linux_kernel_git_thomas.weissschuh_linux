/* SPDX-License-Identifier: GPL-2.0 */
#ifndef ASM_VDSO_GETTIMEOFDAY_H
#define ASM_VDSO_GETTIMEOFDAY_H
#define VDSO_DELTA_NOMASK 1

#include <asm/syscall.h>
#include <asm/timex.h>
#include <asm/unistd.h>
#include <linux/compiler.h>


static inline u64 __arch_get_hw_counter(s32 clock_mode, const struct vdso_time_data *vd)
{
	return get_tod_clock() - vd->arch_data.tod_delta;
}

static __always_inline
long clock_gettime_fallback(clockid_t clkid, struct __kernel_timespec *ts)
{
	return syscall2(__NR_clock_gettime, (long)clkid, (long)ts);
}

#endif
