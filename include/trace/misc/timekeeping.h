/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Display helpers for timekeeping structures
 */

#include <uapi/linux/time.h>

#ifdef CONFIG_POSIX_AUX_CLOCKS

#define _aux_clock_clockid_symbols					\
		{ CLOCK_AUX + 0,	"CLOCK_AUX0"		},	\
		{ CLOCK_AUX + 1,	"CLOCK_AUX1"		},	\
		{ CLOCK_AUX + 2,	"CLOCK_AUX2"		},	\
		{ CLOCK_AUX + 3,	"CLOCK_AUX3"		},	\
		{ CLOCK_AUX + 4,	"CLOCK_AUX4"		},	\
		{ CLOCK_AUX + 5,	"CLOCK_AUX5"		},	\
		{ CLOCK_AUX + 6,	"CLOCK_AUX6"		},	\
		{ CLOCK_AUX + 7,	"CLOCK_AUX7"		},

static_assert(CLOCK_AUX + 7 == CLOCK_AUX_LAST);

#else /* !CONFIG_POSIX_AUX_CLOCKS */

#define _aux_clock_clockid_symbols

#endif /* CONFIG_POSIX_AUX_CLOCKS */

#define decode_clockid(type)						\
	__print_symbolic(type,						\
		{ CLOCK_REALTIME,	"CLOCK_REALTIME"	},	\
		{ CLOCK_MONOTONIC,	"CLOCK_MONOTONIC"	},	\
		{ CLOCK_BOOTTIME,	"CLOCK_BOOTTIME"	},	\
		{ CLOCK_TAI,		"CLOCK_TAI"		},	\
		_aux_clock_clockid_symbols				\
	)
