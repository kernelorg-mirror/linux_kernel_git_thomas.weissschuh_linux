/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_RISCV_CLOCK_INLINED_H
#define _ASM_RISCV_CLOCK_INLINED_H

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

#endif /* _ASM_RISCV_CLOCK_INLINED_H */
