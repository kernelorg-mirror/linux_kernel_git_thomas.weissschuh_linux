/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_ASMARM_UCONTEXT_H
#define _UAPI_ASMARM_UCONTEXT_H

#include <asm/sigcontext.h>
#include <asm/signal.h>

/*
 * struct sigcontext only has room for the basic registers, but struct
 * ucontext now has room for all registers which need to be saved and
 * restored.  Coprocessor registers are stored in uc_regspace.  Each
 * coprocessor's saved state should start with a documented 32-bit magic
 * number, followed by a 32-bit word giving the coproccesor's saved size.
 * uc_regspace may be expanded if necessary, although this takes some
 * coordination with glibc.
 */

struct ucontext {
	unsigned long	  uc_flags;
	struct ucontext  *uc_link;
	stack_t		  uc_stack;
	struct sigcontext uc_mcontext;
	sigset_t	  uc_sigmask;
	/* Allow for uc_sigmask growth.  Glibc uses a 1024-bit sigset_t.  */
	int		  __unused[32 - (sizeof (sigset_t) / sizeof (int))];
	/* Last for extensibility.  Eight byte aligned because some
	   coprocessors require eight byte alignment.  */
	unsigned long	  uc_regspace[128] __attribute__((__aligned__(8)));
};

#endif
