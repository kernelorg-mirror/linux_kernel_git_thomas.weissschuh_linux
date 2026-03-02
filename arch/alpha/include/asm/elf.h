/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_ALPHA_ELF_H
#define __ASM_ALPHA_ELF_H

#include <asm/auxvec.h>
#include <asm/special_insns.h>

/* Special values for the st_other field in the symbol table.  */

#define STO_ALPHA_NOPV		0x80
#define STO_ALPHA_STD_GPLOAD	0x88

#define SHF_ALPHA_GPREL		0x10000000

/* Legal values for e_flags field of Elf64_Ehdr.  */

#define EF_ALPHA_32BIT		1	/* All addresses are below 2GB */

/*
 * ELF register definitions..
 */

/*
 * The OSF/1 version of <sys/procfs.h> makes gregset_t 46 entries long.
 * I have no idea why that is so.  For now, we just leave it at 33
 * (32 general regs + processor status word). 
 */
#define ELF_NGREG	33
#define ELF_NFPREG	32

typedef unsigned long elf_greg_t;
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

typedef double elf_fpreg_t;
typedef elf_fpreg_t elf_fpregset_t[ELF_NFPREG];

/*
 * This is used to ensure we don't load something for the wrong architecture.
 */
#define elf_check_arch(x) (((x)->e_machine == EM_ALPHA) && !((x)->e_flags & EF_ALPHA_32BIT))

/*
 * These are used to set parameters in the core dumps.
 */
#define ELF_CLASS	ELFCLASS64
#define ELF_DATA	ELFDATA2LSB
#define ELF_ARCH	EM_ALPHA

#define ELF_EXEC_PAGESIZE	8192

/* This is the location that an ET_DYN program is loaded if exec'ed.  Typical
   use of this is to invoke "./ld.so someprog" to test out a new version of
   the loader.  We need to make sure that it is out of the way of the program
   that it will "exec", and that there is sufficient room for the brk.  */

#define ELF_ET_DYN_BASE		(TASK_UNMAPPED_BASE + 0x1000000)

/* $0 is set by ld.so to a pointer to a function which might be 
   registered using atexit.  This provides a mean for the dynamic
   linker to call DT_FINI functions for shared libraries that have
   been loaded before the code runs.

   So that we can use the same startup file with static executables,
   we start programs with a value of 0 to indicate that there is no
   such function.  */

#define ELF_PLAT_INIT(_r, load_addr)	_r->r0 = 0

/* The registers are laid out in pt_regs for PAL and syscall
   convenience.  Re-order them for the linear elf_gregset_t.  */

struct pt_regs;
struct thread_info;
struct task_struct;
extern void dump_elf_thread(elf_greg_t *dest, struct pt_regs *pt,
			    struct thread_info *ti);
#define ELF_CORE_COPY_REGS(DEST, REGS) \
	dump_elf_thread(DEST, REGS, current_thread_info());

/* Similar, but for a thread other than current.  */

extern int dump_elf_task(elf_greg_t *dest, struct task_struct *task);
#define ELF_CORE_COPY_TASK_REGS(TASK, DEST) \
	dump_elf_task(*(DEST), TASK)

/* This yields a mask that user programs can use to figure out what
   instruction set this CPU supports.  This is trivial on Alpha, 
   but not so on other machines. */

#define ELF_HWCAP  (~amask(-1))

/* This yields a string that ld.so will use to load implementation
   specific libraries for optimization.  This is more specific in
   intent than poking at uname or /proc/cpuinfo.  */

#define ELF_PLATFORM				\
({						\
	enum implver_enum i_ = implver();	\
	( i_ == IMPLVER_EV5 ? "ev56"			\
	: amask (AMASK_CIX) ? "ev6" : "ev67");	\
})

extern int alpha_l1i_cacheshape;
extern int alpha_l1d_cacheshape;
extern int alpha_l2_cacheshape;
extern int alpha_l3_cacheshape;

/* update AT_VECTOR_SIZE_ARCH if the number of NEW_AUX_ENT entries changes */
#define ARCH_DLINFO						\
  do {								\
    NEW_AUX_ENT(AT_L1I_CACHESHAPE, alpha_l1i_cacheshape);	\
    NEW_AUX_ENT(AT_L1D_CACHESHAPE, alpha_l1d_cacheshape);	\
    NEW_AUX_ENT(AT_L2_CACHESHAPE, alpha_l2_cacheshape);		\
    NEW_AUX_ENT(AT_L3_CACHESHAPE, alpha_l3_cacheshape);		\
  } while (0)

#endif /* __ASM_ALPHA_ELF_H */
