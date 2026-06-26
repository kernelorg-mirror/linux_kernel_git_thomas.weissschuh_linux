/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM timekeeping

#if !defined(_TRACE_TIMEKEEPING_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_TIMEKEEPING_H

#include <linux/tracepoint.h>
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

#endif /*  _TRACE_TIMEKEEPING_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
