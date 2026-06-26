/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TIMEKEEPING_TYPES_H
#define _LINUX_TIMEKEEPING_TYPES_H

#include <linux/types.h>

/**
 * struct tk_clock_offsets - clock offsets to CLOCK_MONOTONIC
 * @clock_was_set_seq:	Sequence counter for clock updates.
 * @offs_real:		Offset to CLOCK_REALTIME.
 * @offs_boot:		Offset to CLOCK_BOOTTIME.
 * @offs_tai:		Offset to CLOCK_TAI.
 */
struct tk_clock_offsets {
	u32			clock_was_set_seq;
	ktime_t			offs_real;
	ktime_t			offs_boot;
	ktime_t			offs_tai;
};

#endif /* _LINUX_TIMEKEEPING_TYPES_H */
