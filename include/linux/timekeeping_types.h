/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TIMEKEEPING_TYPES_H
#define _LINUX_TIMEKEEPING_TYPES_H

#include <linux/types.h>
#include <uapi/linux/time.h>

/**
 * struct tk_aux_mono_conv - Conversion parameters between CLOCK_AUX and CLOCK_MONOTONIC
 * @mono_base:			Timestamp of CLOCK_MONOTONIC when the parameters were captured.
 * @aux_base:			Timestamp of CLOCK_AUX when the parameters were captured.
 * @mono_mult:			Clocksource multiplicator of CLOCK_MONOTONIC.
 * @aux_mult:			Clocksource multiplicator of CLOCK_AUX.
 * @mono_to_aux_conv_mult:	CLOCK_MONOTONIC to CLOCK_AUX conversion multiplicator.
 *				Use together with %TK_AUX_MONO_CONV_SHIFT.
 * @aux_to_mono_conv_mult:	Inverse of @mono_to_aux_conv_mult.
 *
 * See the comment on tk_aux_capture_mono_conv() for details.
 */
struct tk_aux_mono_conv {
	ktime_t mono_base;
	ktime_t aux_base;
	u32     mono_mult;
	u32     aux_mult;
	u32	mono_to_aux_conv_mult;
	u32	aux_to_mono_conv_mult;
};

#define TK_POSSIBLE_AUX_CLOCKS	(IS_ENABLED(CONFIG_POSIX_AUX_CLOCKS) ? MAX_AUX_CLOCKS : 0)

/**
 * struct tk_clock_offsets - clock offsets to CLOCK_MONOTONIC
 * @clock_was_set_seq:	Sequence counter for clock updates.
 * @clock_aux_conv_seq:	Sequence counter for auxiliary clock conversion adjustments.
 * @clock_seq:		Combined sequence counter.
 * @offs_real:		Offset to CLOCK_REALTIME.
 * @offs_boot:		Offset to CLOCK_BOOTTIME.
 * @offs_tai:		Offset to CLOCK_TAI.
 * @conv_aux_valid:	The conversion parameter structure contains valid values.
 * @conv_aux:		Conversion parameters to CLOCK_AUX.
 */
struct tk_clock_offsets {
	union {
		struct {
			u32	clock_was_set_seq;
			u32	clock_aux_conv_seq;
		};
		u64		clock_seq;
	};
	ktime_t			offs_real;
	ktime_t			offs_boot;
	ktime_t			offs_tai;
	bool			conv_aux_valid[TK_POSSIBLE_AUX_CLOCKS];
	struct tk_aux_mono_conv	conv_aux[TK_POSSIBLE_AUX_CLOCKS];
};

#endif /* _LINUX_TIMEKEEPING_TYPES_H */
