/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM timekeeping

#if !defined(_TRACE_TIMEKEEPING_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_TIMEKEEPING_H

#include <linux/tracepoint.h>
#include <linux/timekeeping_types.h>
#include <trace/misc/timekeeping.h>

TRACE_EVENT(timekeeping_auxclock_deviation,

	TP_PROTO(clockid_t clock, ktime_t deviation),

	TP_ARGS(clock, deviation),

	TP_STRUCT__entry(
		__field( clockid_t,	clock		)
		__field( ktime_t,	deviation	)
	),

	TP_fast_assign(
		__entry->clock		= clock;
		__entry->deviation	= deviation;
	),

	TP_printk("clock=%s deviation=%lldns",
		  decode_clockid(__entry->clock), __entry->deviation)
);

TRACE_EVENT(timekeeping_auxclock_conversion_update,

	TP_PROTO(clockid_t clock, bool clock_valid, const struct tk_aux_mono_conv *conv),

	TP_ARGS(clock, clock_valid, conv),

	TP_STRUCT__entry(
		__field(	clockid_t,			clock		)
		__field(	bool,				clock_valid	)
		__field_struct(	struct tk_aux_mono_conv,	conv		)
	),

	TP_fast_assign(
		__entry->clock		= clock;
		__entry->clock_valid	= clock_valid;
		__entry->conv		= *conv;
	),

	TP_printk("clock=%s valid=%d mono_base=%lld aux_base=%lld mono_mult=%u aux_mult=%u",
		  decode_clockid(__entry->clock), __entry->clock_valid, __entry->conv.mono_base,
		  __entry->conv.aux_base, __entry->conv.mono_mult, __entry->conv.aux_mult)
);

#endif /*  _TRACE_TIMEKEEPING_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
