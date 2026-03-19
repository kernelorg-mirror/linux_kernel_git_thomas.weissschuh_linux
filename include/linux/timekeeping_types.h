/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TIMEKEEPING_TYPES_H
#define _LINUX_TIMEKEEPING_TYPES_H

#include <linux/types.h>

struct tk_clock_offsets {
	ktime_t			offs_none;
	ktime_t			offs_real;
	ktime_t			offs_boot;
	ktime_t			offs_tai;
};

#endif /* _LINUX_TIMEKEEPING_TYPES_H */
