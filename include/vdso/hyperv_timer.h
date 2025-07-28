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

#ifndef __VDSO_HYPERV_TIMER_H
#define __VDSO_HYPERV_TIMER_H

#include <linux/types.h>
#include <hyperv/hvgdk_mini.h>

#ifdef CONFIG_HYPERV_TIMER

#include <asm/hyperv_timer.h>

static __always_inline bool
hv_read_tsc_page_tsc(const struct ms_hyperv_tsc_page *tsc_pg,
		     u64 *cur_tsc, u64 *time)
{
	u64 scale, offset;
	u32 sequence;

	/*
	 * The protocol for reading Hyper-V TSC page is specified in Hypervisor
	 * Top-Level Functional Specification ver. 3.0 and above. To get the
	 * reference time we must do the following:
	 * - READ ReferenceTscSequence
	 *   A special '0' value indicates the time source is unreliable and we
	 *   need to use something else. The currently published specification
	 *   versions (up to 4.0b) contain a mistake and wrongly claim '-1'
	 *   instead of '0' as the special value, see commit c35b82ef0294.
	 * - ReferenceTime =
	 *        ((RDTSC() * ReferenceTscScale) >> 64) + ReferenceTscOffset
	 * - READ ReferenceTscSequence again. In case its value has changed
	 *   since our first reading we need to discard ReferenceTime and repeat
	 *   the whole sequence as the hypervisor was updating the page in
	 *   between.
	 */
	do {
		sequence = READ_ONCE(tsc_pg->tsc_sequence);
		if (!sequence)
			return false;
		/*
		 * Make sure we read sequence before we read other values from
		 * TSC page.
		 */
		smp_rmb();

		scale = READ_ONCE(tsc_pg->tsc_scale);
		offset = READ_ONCE(tsc_pg->tsc_offset);
		*cur_tsc = hv_get_raw_timer();

		/*
		 * Make sure we read sequence after we read all other values
		 * from TSC page.
		 */
		smp_rmb();

	} while (READ_ONCE(tsc_pg->tsc_sequence) != sequence);

	*time = mul_u64_u64_shr(*cur_tsc, scale, 64) + offset;
	return true;
}

#else /* CONFIG_HYPERV_TIMER */

static __always_inline bool
hv_read_tsc_page_tsc(const struct ms_hyperv_tsc_page *tsc_pg, u64 *cur_tsc, u64 *time)
{
	return false;
}

#endif /* CONFIG_HYPERV_TIMER */

#endif /* __VDSO_HYPERV_TIMER_H */
