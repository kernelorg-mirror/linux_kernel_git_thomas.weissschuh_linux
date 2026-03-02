/* SPDX-License-Identifier: GPL-2.0+ WITH Linux-syscall-note */
/*
 * Copyright (C) 2011 Tobias Klauser <tklauser@distanz.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef _UAPI_ASM_NIOS2_ELF_H
#define _UAPI_ASM_NIOS2_ELF_H

#include <linux/ptrace.h>

typedef unsigned long elf_greg_t;

#define ELF_NGREG		49
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

typedef unsigned long elf_fpregset_t;

/*
 * These are used to set parameters in the core dumps.
 */
#define ELF_CLASS	ELFCLASS32
#define ELF_DATA	ELFDATA2LSB
#define ELF_ARCH	EM_ALTERA_NIOS2

#endif /* _UAPI_ASM_NIOS2_ELF_H */
