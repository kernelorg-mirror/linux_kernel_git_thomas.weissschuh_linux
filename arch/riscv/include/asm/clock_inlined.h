/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_RISCV_CLOCK_INLINED_H
#define _ASM_RISCV_CLOCK_INLINED_H

#include <linux/jump_label.h>

#include <asm/csr.h>
#include <asm/sbi.h>
#include <asm/timex.h>

struct clocksource;

/*
 * It is guaranteed that all the timers across all the harts are synchronized
 * within one tick of each other, so while this could technically go
 * backwards when hopping between CPUs, practically it won't happen.
 */
static __always_inline u64 arch_inlined_clocksource_read(struct clocksource *cs)
{
	return get_cycles64();
}

struct clock_event_device;
DECLARE_STATIC_KEY_FALSE(riscv_sstc_available);

static __always_inline void
arch_inlined_clockevent_set_next_coupled(u64 cycles, struct clock_event_device *evt)
{
	u64 next_tval = cycles;

	if (static_branch_likely(&riscv_sstc_available)) {
#if defined(CONFIG_32BIT)
		csr_write(CSR_STIMECMP, ULONG_MAX);
		csr_write(CSR_STIMECMPH, next_tval >> 32);
		csr_write(CSR_STIMECMP, next_tval & 0xFFFFFFFF);
#else
		csr_write(CSR_STIMECMP, next_tval);
#endif
	} else
		sbi_set_timer(next_tval);
}

#endif /* _ASM_RISCV_CLOCK_INLINED_H */
