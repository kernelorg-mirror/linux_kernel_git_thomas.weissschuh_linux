/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_UM_CPUFEATURE_H
#define _ASM_UM_CPUFEATURE_H

#include <asm/processor.h>

#if defined(__KERNEL__) && !defined(__ASSEMBLER__)

#include <asm/asm.h>

extern const char * const x86_cap_flags[NCAPINTS*32];
#define X86_CAP_FMT "%s"
#define x86_cap_flag(flag) x86_cap_flags[flag]

#define test_cpu_cap(c, bit)						\
	 test_bit(bit, (unsigned long *)((c)->x86_capability))

#define cpu_has(c, bit)							\
	 test_cpu_cap(c, bit)

#define set_cpu_cap(c, bit)	set_bit(bit, (unsigned long *)((c)->x86_capability))

#endif /* defined(__KERNEL__) && !defined(__ASSEMBLER__) */
#endif /* _ASM_UM_CPUFEATURE_H */
