/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TIMEKEEPING_TYPES_H
#define _LINUX_TIMEKEEPING_TYPES_H

#include <linux/types.h>

struct tk_clock_offsets {
	union {
		u64		clock_seq;
		struct {
			u32	clock_was_set_seq;
			u32	clock_aux_offs_seq;
		};
	};
	ktime_t			offs_none;
	ktime_t			offs_real;
	ktime_t			offs_boot;
	ktime_t			offs_tai;
#ifdef CONFIG_POSIX_AUX_CLOCKS
	ktime_t			offs_aux[MAX_AUX_CLOCKS];
#else
	ktime_t			offs_aux[0];
#endif
};

#endif /* _LINUX_TIMEKEEPING_TYPES_H */
