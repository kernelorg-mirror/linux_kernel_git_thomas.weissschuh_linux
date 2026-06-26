/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_HRTIMER_BASES_H
#define _LINUX_HRTIMER_BASES_H

#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/timekeeping_types.h>
#include <linux/timerqueue.h>
#include <linux/seqlock.h>

#ifdef CONFIG_64BIT
# define __hrtimer_clock_base_align	____cacheline_aligned
#else
# define __hrtimer_clock_base_align
#endif

/**
 * struct hrtimer_clock_base - the timer base for a specific clock
 * @cpu_base:		per cpu clock base
 * @index:		clock type index for per_cpu support when moving a
 *			timer to a base on another cpu.
 * @clockid:		clock id for per_cpu support
 * @seq:		seqcount around __run_hrtimer
 * @expires_next:	Absolute time of the next event in this clock base
 * @running:		pointer to the currently running hrtimer
 * @active:		red black tree root node for the active timers
 * @offset:		Pointer to the offset of this clock to the monotonic base.
 * @aux_mono_conv:	Conversion parameters between CLOCK_AUX and CLOCK_MONOTONIC.
 */
struct hrtimer_clock_base {
	struct hrtimer_cpu_base		*cpu_base;
	const unsigned int		index;
	const clockid_t			clockid;
	const bool			*enabled;
	seqcount_raw_spinlock_t		seq;
	ktime_t				expires_next;
	struct hrtimer			*running;
	struct timerqueue_linked_head	active;
	union {
		const ktime_t			*offset;
		const struct tk_aux_mono_conv	*aux_mono_conv;
	};
} __hrtimer_clock_base_align;

enum hrtimer_base_type {
	HRTIMER_BASE_MONOTONIC,
	HRTIMER_BASE_REALTIME,
	HRTIMER_BASE_BOOTTIME,
	HRTIMER_BASE_TAI,
#ifdef CONFIG_POSIX_AUX_CLOCKS
	HRTIMER_BASE_AUX0,
	HRTIMER_BASE_AUX1,
	HRTIMER_BASE_AUX2,
	HRTIMER_BASE_AUX3,
	HRTIMER_BASE_AUX4,
	HRTIMER_BASE_AUX5,
	HRTIMER_BASE_AUX6,
	HRTIMER_BASE_AUX7,
#endif
	HRTIMER_BASE_MONOTONIC_SOFT,
	HRTIMER_BASE_REALTIME_SOFT,
	HRTIMER_BASE_BOOTTIME_SOFT,
	HRTIMER_BASE_TAI_SOFT,
#ifdef CONFIG_POSIX_AUX_CLOCKS
	HRTIMER_BASE_AUX0_SOFT,
	HRTIMER_BASE_AUX1_SOFT,
	HRTIMER_BASE_AUX2_SOFT,
	HRTIMER_BASE_AUX3_SOFT,
	HRTIMER_BASE_AUX4_SOFT,
	HRTIMER_BASE_AUX5_SOFT,
	HRTIMER_BASE_AUX6_SOFT,
	HRTIMER_BASE_AUX7_SOFT,
#endif
	HRTIMER_MAX_CLOCK_BASES
};

#ifndef CONFIG_POSIX_AUX_CLOCKS
/* Unresolvable dummy symbols */
extern int HRTIMER_BASE_AUX0, HRTIMER_BASE_AUX0_SOFT;
#endif

/**
 * struct hrtimer_cpu_base - the per cpu clock bases
 * @lock:			lock protecting the base and associated clock bases and timers
 * @cpu:			cpu number
 * @active_bases:		Bitfield to mark bases with active timers
 * @tk_offsets:			Timekeeping clock offsets to CLOCK_MONOTONIC
 * @offs_none:			Always zero for CLOCK_MONOTONIC hrtimer_clock_base::offset.
 * @hres_active:		State of high resolution mode
 * @deferred_rearm:		A deferred rearm is pending
 * @deferred_needs_update:	The deferred rearm must re-evaluate the first timer
 * @hang_detected:		The last hrtimer interrupt detected a hang
 * @softirq_activated:		displays, if the softirq is raised - update of softirq
 *				related settings is not required then.
 * @nr_events:			Total number of hrtimer interrupt events
 * @nr_retries:			Total number of hrtimer interrupt retries
 * @nr_hangs:			Total number of hrtimer interrupt hangs
 * @max_hang_time:		Maximum time spent in hrtimer_interrupt
 * @softirq_expiry_lock:	Lock which is taken while softirq based hrtimer are expired
 * @online:			CPU is online from an hrtimers point of view
 * @enabled_core:		Always true for hrtimer_clock_base::enabled of core clocks.
 * @timer_waiters:		A hrtimer_cancel() waiters for the timer callback to finish.
 * @expires_next:		Absolute time of the next event, is required for remote
 *				hrtimer enqueue; it is the total first expiry time (hard
 *				and soft hrtimer are taken into account)
 * @next_timer:			Pointer to the first expiring timer
 * @softirq_expires_next:	Time to check, if soft queues needs also to be expired
 * @softirq_next_timer:		Pointer to the first expiring softirq based timer
 * @deferred_expires_next:	Cached expires next value for deferred rearm
 * @clock_base:			Array of clock bases for this cpu
 *
 * Note: next_timer is just an optimization for __remove_hrtimer().
 *	 Do not dereference the pointer because it is not reliable on
 *	 cross cpu removals.
 */
struct hrtimer_cpu_base {
	raw_spinlock_t			lock;
	unsigned int			cpu;
	unsigned int			active_bases;
	struct tk_clock_offsets		tk_offsets;
	const ktime_t			offs_none;
	bool				hres_active;
	bool				deferred_rearm;
	bool				deferred_needs_update;
	bool				hang_detected;
	bool				softirq_activated;
	bool				online;
	const bool			enabled_core;
#ifdef CONFIG_HIGH_RES_TIMERS
	unsigned int			nr_events;
	unsigned short			nr_retries;
	unsigned short			nr_hangs;
	unsigned int			max_hang_time;
#endif
#ifdef CONFIG_PREEMPT_RT
	spinlock_t			softirq_expiry_lock;
	atomic_t			timer_waiters;
#endif
	ktime_t				expires_next;
	struct hrtimer			*next_timer;
	ktime_t				softirq_expires_next;
	struct hrtimer			*softirq_next_timer;
	ktime_t				deferred_expires_next;
	struct hrtimer_clock_base	clock_base[HRTIMER_MAX_CLOCK_BASES];
	call_single_data_t		csd;
} ____cacheline_aligned;


/*
 * Helper function to check, whether the timer is running the callback
 * function
 */
static inline int hrtimer_callback_running(struct hrtimer *timer)
{
	return timer->base->running == timer;
}

#endif
