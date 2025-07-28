/* SPDX-License-Identifier: GPL-2.0 */

/*
 * Definitions for the clocksource provided by the Hyper-V
 * hypervisor to guest VMs, as described in the Hyper-V Top
 * Level Functional Spec (TLFS).
 *
 * Copyright (C) 2019, Microsoft, Inc.
 *
 * Author:  Michael Kelley <mikelley@microsoft.com>
 */

#ifndef __CLKSOURCE_HYPERV_TIMER_H
#define __CLKSOURCE_HYPERV_TIMER_H

#include <linux/clocksource.h>
#include <linux/math64.h>
#include <hyperv/hvhdk.h>
#include <vdso/hyperv_timer.h>

#define HV_MAX_MAX_DELTA_TICKS 0xffffffff
#define HV_MIN_DELTA_TICKS 1

#ifdef CONFIG_HYPERV_TIMER

/* Routines called by the VMbus driver */
extern int hv_stimer_alloc(bool have_percpu_irqs);
extern int hv_stimer_cleanup(unsigned int cpu);
extern void hv_stimer_legacy_init(unsigned int cpu, int sint);
extern void hv_stimer_legacy_cleanup(unsigned int cpu);
extern void hv_stimer_global_cleanup(void);
extern void hv_stimer0_isr(void);

extern void hv_init_clocksource(void);
extern void hv_remap_tsc_clocksource(void);

extern unsigned long hv_get_tsc_pfn(void);
extern struct ms_hyperv_tsc_page *hv_get_tsc_page(void);

extern void hv_adj_sched_clock_offset(u64 offset);

#else /* CONFIG_HYPERV_TIMER */
static inline unsigned long hv_get_tsc_pfn(void)
{
	return 0;
}

static inline struct ms_hyperv_tsc_page *hv_get_tsc_page(void)
{
	return NULL;
}

static inline int hv_stimer_cleanup(unsigned int cpu) { return 0; }
static inline void hv_stimer_legacy_init(unsigned int cpu, int sint) {}
static inline void hv_stimer_legacy_cleanup(unsigned int cpu) {}
static inline void hv_stimer_global_cleanup(void) {}
static inline void hv_stimer0_isr(void) {}

#endif /* CONFIG_HYPERV_TIMER */

#endif
