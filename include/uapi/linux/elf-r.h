/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_LINUX_ELF_R_H
#define _UAPI_LINUX_ELF_R_H

/* Relocation constants. Architectures are ordered by their numeric EM_* values */

/*
 * Sparc ELF relocation types
 */
#define	R_SPARC_NONE		0
#define	R_SPARC_8		1
#define	R_SPARC_16		2
#define	R_SPARC_32		3
#define	R_SPARC_DISP8		4
#define	R_SPARC_DISP16		5
#define	R_SPARC_DISP32		6
#define	R_SPARC_WDISP30		7
#define	R_SPARC_WDISP22		8
#define	R_SPARC_HI22		9
#define	R_SPARC_22		10
#define	R_SPARC_13		11
#define	R_SPARC_LO10		12
#define	R_SPARC_GOT10		13
#define	R_SPARC_GOT13		14
#define	R_SPARC_GOT22		15
#define	R_SPARC_PC10		16
#define	R_SPARC_PC22		17
#define	R_SPARC_WPLT30		18
#define	R_SPARC_COPY		19
#define	R_SPARC_GLOB_DAT	20
#define	R_SPARC_JMP_SLOT	21
#define	R_SPARC_RELATIVE	22
#define	R_SPARC_UA32		23
#define R_SPARC_PLT32		24
#define R_SPARC_HIPLT22		25
#define R_SPARC_LOPLT10		26
#define R_SPARC_PCPLT32		27
#define R_SPARC_PCPLT22		28
#define R_SPARC_PCPLT10		29
#define R_SPARC_10		30
#define R_SPARC_11		31
#define R_SPARC_64		32
#define R_SPARC_OLO10		33
#define R_SPARC_WDISP16		40
#define R_SPARC_WDISP19		41
#define R_SPARC_7		43
#define R_SPARC_5		44
#define R_SPARC_6		45
#define R_SPARC_UA64		54

/* i386 relocation types */
#define R_386_NONE	0
#define R_386_32	1
#define R_386_PC32	2
#define R_386_GOT32	3
#define R_386_PLT32	4
#define R_386_COPY	5
#define R_386_GLOB_DAT	6
#define R_386_JMP_SLOT	7
#define R_386_RELATIVE	8
#define R_386_GOTOFF	9
#define R_386_GOTPC	10

/* 68k ELF relocation types */
#define R_68K_NONE	0
#define R_68K_32	1
#define R_68K_16	2
#define R_68K_8		3
#define R_68K_PC32	4
#define R_68K_PC16	5
#define R_68K_PC8	6
#define R_68K_GOT32	7
#define R_68K_GOT16	8
#define R_68K_GOT8	9
#define R_68K_GOT32O	10
#define R_68K_GOT16O	11
#define R_68K_GOT8O	12
#define R_68K_PLT32	13
#define R_68K_PLT16	14
#define R_68K_PLT8	15
#define R_68K_PLT32O	16
#define R_68K_PLT16O	17
#define R_68K_PLT8O	18
#define R_68K_COPY	19
#define R_68K_GLOB_DAT	20
#define R_68K_JMP_SLOT	21
#define R_68K_RELATIVE	22

/* MIPS relocations defined by the ABIs */
#define R_MIPS_NONE		0
#define R_MIPS_16		1
#define R_MIPS_32		2
#define R_MIPS_REL32		3
#define R_MIPS_26		4
#define R_MIPS_HI16		5
#define R_MIPS_LO16		6
#define R_MIPS_GPREL16		7
#define R_MIPS_LITERAL		8
#define R_MIPS_GOT16		9
#define R_MIPS_PC16		10
#define R_MIPS_CALL16		11
#define R_MIPS_GPREL32		12
/* The remaining relocs are defined on Irix, although they are not
   in the MIPS ELF ABI.	 */
#define R_MIPS_UNUSED1		13
#define R_MIPS_UNUSED2		14
#define R_MIPS_UNUSED3		15
#define R_MIPS_SHIFT5		16
#define R_MIPS_SHIFT6		17
#define R_MIPS_64		18
#define R_MIPS_GOT_DISP		19
#define R_MIPS_GOT_PAGE		20
#define R_MIPS_GOT_OFST		21
/*
 * The following two relocation types are specified in the MIPS ABI
 * conformance guide version 1.2 but not yet in the psABI.
 */
#define R_MIPS_GOTHI16		22
#define R_MIPS_GOTLO16		23
#define R_MIPS_SUB		24
#define R_MIPS_INSERT_A		25
#define R_MIPS_INSERT_B		26
#define R_MIPS_DELETE		27
#define R_MIPS_HIGHER		28
#define R_MIPS_HIGHEST		29
/*
 * The following two relocation types are specified in the MIPS ABI
 * conformance guide version 1.2 but not yet in the psABI.
 */
#define R_MIPS_CALLHI16		30
#define R_MIPS_CALLLO16		31
/*
 * Introduced for MIPSr6.
 */
#define R_MIPS_PC21_S2		60
#define R_MIPS_PC26_S2		61
/*
 * This range is reserved for vendor specific relocations.
 */
#define R_MIPS_LOVENDOR		100
#define R_MIPS_HIVENDOR		127

#define R_MIPS_PC32		248

/* x86-64 relocation types */
#define R_X86_64_NONE		0	/* No reloc */
#define R_X86_64_64		1	/* Direct 64 bit  */
#define R_X86_64_PC32		2	/* PC relative 32 bit signed */
#define R_X86_64_GOT32		3	/* 32 bit GOT entry */
#define R_X86_64_PLT32		4	/* 32 bit PLT address */
#define R_X86_64_COPY		5	/* Copy symbol at runtime */
#define R_X86_64_GLOB_DAT	6	/* Create GOT entry */
#define R_X86_64_JUMP_SLOT	7	/* Create PLT entry */
#define R_X86_64_RELATIVE	8	/* Adjust by program base */
#define R_X86_64_GOTPCREL	9	/* 32 bit signed pc relative
					   offset to GOT */
#define R_X86_64_32		10	/* Direct 32 bit zero extended */
#define R_X86_64_32S		11	/* Direct 32 bit sign extended */
#define R_X86_64_16		12	/* Direct 16 bit zero extended */
#define R_X86_64_PC16		13	/* 16 bit sign extended pc relative */
#define R_X86_64_8		14	/* Direct 8 bit sign extended  */
#define R_X86_64_PC8		15	/* 8 bit sign extended pc relative */
#define R_X86_64_PC64		24	/* Place relative 64-bit signed */

/* PowerPC relocations defined by the ABIs */
#define R_PPC_NONE		0
#define R_PPC_ADDR32		1	/* 32bit absolute address */
#define R_PPC_ADDR24		2	/* 26bit address, 2 bits ignored.  */
#define R_PPC_ADDR16		3	/* 16bit absolute address */
#define R_PPC_ADDR16_LO		4	/* lower 16bit of absolute address */
#define R_PPC_ADDR16_HI		5	/* high 16bit of absolute address */
#define R_PPC_ADDR16_HA		6	/* adjusted high 16bit */
#define R_PPC_ADDR14		7	/* 16bit address, 2 bits ignored */
#define R_PPC_ADDR14_BRTAKEN	8
#define R_PPC_ADDR14_BRNTAKEN	9
#define R_PPC_REL24		10	/* PC relative 26 bit */
#define R_PPC_REL14		11	/* PC relative 16 bit */
#define R_PPC_REL14_BRTAKEN	12
#define R_PPC_REL14_BRNTAKEN	13
#define R_PPC_GOT16		14
#define R_PPC_GOT16_LO		15
#define R_PPC_GOT16_HI		16
#define R_PPC_GOT16_HA		17
#define R_PPC_PLTREL24		18
#define R_PPC_COPY		19
#define R_PPC_GLOB_DAT		20
#define R_PPC_JMP_SLOT		21
#define R_PPC_RELATIVE		22
#define R_PPC_LOCAL24PC		23
#define R_PPC_UADDR32		24
#define R_PPC_UADDR16		25
#define R_PPC_REL32		26
#define R_PPC_PLT32		27
#define R_PPC_PLTREL32		28
#define R_PPC_PLT16_LO		29
#define R_PPC_PLT16_HI		30
#define R_PPC_PLT16_HA		31
#define R_PPC_SDAREL16		32
#define R_PPC_SECTOFF		33
#define R_PPC_SECTOFF_LO	34
#define R_PPC_SECTOFF_HI	35
#define R_PPC_SECTOFF_HA	36

/* PowerPC relocations defined for the TLS access ABI.  */
#define R_PPC_TLS		67 /* none	(sym+add)@tls */
#define R_PPC_DTPMOD32		68 /* word32	(sym+add)@dtpmod */
#define R_PPC_TPREL16		69 /* half16*	(sym+add)@tprel */
#define R_PPC_TPREL16_LO	70 /* half16	(sym+add)@tprel@l */
#define R_PPC_TPREL16_HI	71 /* half16	(sym+add)@tprel@h */
#define R_PPC_TPREL16_HA	72 /* half16	(sym+add)@tprel@ha */
#define R_PPC_TPREL32		73 /* word32	(sym+add)@tprel */
#define R_PPC_DTPREL16		74 /* half16*	(sym+add)@dtprel */
#define R_PPC_DTPREL16_LO	75 /* half16	(sym+add)@dtprel@l */
#define R_PPC_DTPREL16_HI	76 /* half16	(sym+add)@dtprel@h */
#define R_PPC_DTPREL16_HA	77 /* half16	(sym+add)@dtprel@ha */
#define R_PPC_DTPREL32		78 /* word32	(sym+add)@dtprel */
#define R_PPC_GOT_TLSGD16	79 /* half16*	(sym+add)@got@tlsgd */
#define R_PPC_GOT_TLSGD16_LO	80 /* half16	(sym+add)@got@tlsgd@l */
#define R_PPC_GOT_TLSGD16_HI	81 /* half16	(sym+add)@got@tlsgd@h */
#define R_PPC_GOT_TLSGD16_HA	82 /* half16	(sym+add)@got@tlsgd@ha */
#define R_PPC_GOT_TLSLD16	83 /* half16*	(sym+add)@got@tlsld */
#define R_PPC_GOT_TLSLD16_LO	84 /* half16	(sym+add)@got@tlsld@l */
#define R_PPC_GOT_TLSLD16_HI	85 /* half16	(sym+add)@got@tlsld@h */
#define R_PPC_GOT_TLSLD16_HA	86 /* half16	(sym+add)@got@tlsld@ha */
#define R_PPC_GOT_TPREL16	87 /* half16*	(sym+add)@got@tprel */
#define R_PPC_GOT_TPREL16_LO	88 /* half16	(sym+add)@got@tprel@l */
#define R_PPC_GOT_TPREL16_HI	89 /* half16	(sym+add)@got@tprel@h */
#define R_PPC_GOT_TPREL16_HA	90 /* half16	(sym+add)@got@tprel@ha */
#define R_PPC_GOT_DTPREL16	91 /* half16*	(sym+add)@got@dtprel */
#define R_PPC_GOT_DTPREL16_LO	92 /* half16*	(sym+add)@got@dtprel@l */
#define R_PPC_GOT_DTPREL16_HI	93 /* half16*	(sym+add)@got@dtprel@h */
#define R_PPC_GOT_DTPREL16_HA	94 /* half16*	(sym+add)@got@dtprel@ha */

/* PowerPC64 relocations defined by the ABIs */
#define R_PPC64_NONE    R_PPC_NONE
#define R_PPC64_ADDR32  R_PPC_ADDR32  /* 32bit absolute address.  */
#define R_PPC64_ADDR24  R_PPC_ADDR24  /* 26bit address, word aligned.  */
#define R_PPC64_ADDR16  R_PPC_ADDR16  /* 16bit absolute address. */
#define R_PPC64_ADDR16_LO R_PPC_ADDR16_LO /* lower 16bits of abs. address.  */
#define R_PPC64_ADDR16_HI R_PPC_ADDR16_HI /* high 16bits of abs. address. */
#define R_PPC64_ADDR16_HA R_PPC_ADDR16_HA /* adjusted high 16bits.  */
#define R_PPC64_ADDR14 R_PPC_ADDR14   /* 16bit address, word aligned.  */
#define R_PPC64_ADDR14_BRTAKEN  R_PPC_ADDR14_BRTAKEN
#define R_PPC64_ADDR14_BRNTAKEN R_PPC_ADDR14_BRNTAKEN
#define R_PPC64_REL24   R_PPC_REL24 /* PC relative 26 bit, word aligned.  */
#define R_PPC64_REL14   R_PPC_REL14 /* PC relative 16 bit. */
#define R_PPC64_REL14_BRTAKEN   R_PPC_REL14_BRTAKEN
#define R_PPC64_REL14_BRNTAKEN  R_PPC_REL14_BRNTAKEN
#define R_PPC64_GOT16     R_PPC_GOT16
#define R_PPC64_GOT16_LO  R_PPC_GOT16_LO
#define R_PPC64_GOT16_HI  R_PPC_GOT16_HI
#define R_PPC64_GOT16_HA  R_PPC_GOT16_HA

#define R_PPC64_COPY      R_PPC_COPY
#define R_PPC64_GLOB_DAT  R_PPC_GLOB_DAT
#define R_PPC64_JMP_SLOT  R_PPC_JMP_SLOT
#define R_PPC64_RELATIVE  R_PPC_RELATIVE

#define R_PPC64_UADDR32   R_PPC_UADDR32
#define R_PPC64_UADDR16   R_PPC_UADDR16
#define R_PPC64_REL32     R_PPC_REL32
#define R_PPC64_PLT32     R_PPC_PLT32
#define R_PPC64_PLTREL32  R_PPC_PLTREL32
#define R_PPC64_PLT16_LO  R_PPC_PLT16_LO
#define R_PPC64_PLT16_HI  R_PPC_PLT16_HI
#define R_PPC64_PLT16_HA  R_PPC_PLT16_HA

#define R_PPC64_SECTOFF     R_PPC_SECTOFF
#define R_PPC64_SECTOFF_LO  R_PPC_SECTOFF_LO
#define R_PPC64_SECTOFF_HI  R_PPC_SECTOFF_HI
#define R_PPC64_SECTOFF_HA  R_PPC_SECTOFF_HA
#define R_PPC64_ADDR30          37  /* word30 (S + A - P) >> 2.  */
#define R_PPC64_ADDR64          38  /* doubleword64 S + A.  */
#define R_PPC64_ADDR16_HIGHER   39  /* half16 #higher(S + A).  */
#define R_PPC64_ADDR16_HIGHERA  40  /* half16 #highera(S + A).  */
#define R_PPC64_ADDR16_HIGHEST  41  /* half16 #highest(S + A).  */
#define R_PPC64_ADDR16_HIGHESTA 42  /* half16 #highesta(S + A). */
#define R_PPC64_UADDR64     43  /* doubleword64 S + A.  */
#define R_PPC64_REL64       44  /* doubleword64 S + A - P.  */
#define R_PPC64_PLT64       45  /* doubleword64 L + A.  */
#define R_PPC64_PLTREL64    46  /* doubleword64 L + A - P.  */
#define R_PPC64_TOC16       47  /* half16* S + A - .TOC.  */
#define R_PPC64_TOC16_LO    48  /* half16 #lo(S + A - .TOC.).  */
#define R_PPC64_TOC16_HI    49  /* half16 #hi(S + A - .TOC.).  */
#define R_PPC64_TOC16_HA    50  /* half16 #ha(S + A - .TOC.).  */
#define R_PPC64_TOC         51  /* doubleword64 .TOC. */
#define R_PPC64_PLTGOT16    52  /* half16* M + A.  */
#define R_PPC64_PLTGOT16_LO 53  /* half16 #lo(M + A).  */
#define R_PPC64_PLTGOT16_HI 54  /* half16 #hi(M + A).  */
#define R_PPC64_PLTGOT16_HA 55  /* half16 #ha(M + A).  */

#define R_PPC64_ADDR16_DS      56 /* half16ds* (S + A) >> 2.  */
#define R_PPC64_ADDR16_LO_DS   57 /* half16ds  #lo(S + A) >> 2.  */
#define R_PPC64_GOT16_DS       58 /* half16ds* (G + A) >> 2.  */
#define R_PPC64_GOT16_LO_DS    59 /* half16ds  #lo(G + A) >> 2.  */
#define R_PPC64_PLT16_LO_DS    60 /* half16ds  #lo(L + A) >> 2.  */
#define R_PPC64_SECTOFF_DS     61 /* half16ds* (R + A) >> 2.  */
#define R_PPC64_SECTOFF_LO_DS  62 /* half16ds  #lo(R + A) >> 2.  */
#define R_PPC64_TOC16_DS       63 /* half16ds* (S + A - .TOC.) >> 2.  */
#define R_PPC64_TOC16_LO_DS    64 /* half16ds  #lo(S + A - .TOC.) >> 2.  */
#define R_PPC64_PLTGOT16_DS    65 /* half16ds* (M + A) >> 2.  */
#define R_PPC64_PLTGOT16_LO_DS 66 /* half16ds  #lo(M + A) >> 2.  */

/* PowerPC64 relocations defined for the TLS access ABI.  */
#define R_PPC64_TLS		67 /* none	(sym+add)@tls */
#define R_PPC64_DTPMOD64	68 /* doubleword64 (sym+add)@dtpmod */
#define R_PPC64_TPREL16		69 /* half16*	(sym+add)@tprel */
#define R_PPC64_TPREL16_LO	70 /* half16	(sym+add)@tprel@l */
#define R_PPC64_TPREL16_HI	71 /* half16	(sym+add)@tprel@h */
#define R_PPC64_TPREL16_HA	72 /* half16	(sym+add)@tprel@ha */
#define R_PPC64_TPREL64		73 /* doubleword64 (sym+add)@tprel */
#define R_PPC64_DTPREL16	74 /* half16*	(sym+add)@dtprel */
#define R_PPC64_DTPREL16_LO	75 /* half16	(sym+add)@dtprel@l */
#define R_PPC64_DTPREL16_HI	76 /* half16	(sym+add)@dtprel@h */
#define R_PPC64_DTPREL16_HA	77 /* half16	(sym+add)@dtprel@ha */
#define R_PPC64_DTPREL64	78 /* doubleword64 (sym+add)@dtprel */
#define R_PPC64_GOT_TLSGD16	79 /* half16*	(sym+add)@got@tlsgd */
#define R_PPC64_GOT_TLSGD16_LO	80 /* half16	(sym+add)@got@tlsgd@l */
#define R_PPC64_GOT_TLSGD16_HI	81 /* half16	(sym+add)@got@tlsgd@h */
#define R_PPC64_GOT_TLSGD16_HA	82 /* half16	(sym+add)@got@tlsgd@ha */
#define R_PPC64_GOT_TLSLD16	83 /* half16*	(sym+add)@got@tlsld */
#define R_PPC64_GOT_TLSLD16_LO	84 /* half16	(sym+add)@got@tlsld@l */
#define R_PPC64_GOT_TLSLD16_HI	85 /* half16	(sym+add)@got@tlsld@h */
#define R_PPC64_GOT_TLSLD16_HA	86 /* half16	(sym+add)@got@tlsld@ha */
#define R_PPC64_GOT_TPREL16_DS	87 /* half16ds*	(sym+add)@got@tprel */
#define R_PPC64_GOT_TPREL16_LO_DS 88 /* half16ds (sym+add)@got@tprel@l */
#define R_PPC64_GOT_TPREL16_HI	89 /* half16	(sym+add)@got@tprel@h */
#define R_PPC64_GOT_TPREL16_HA	90 /* half16	(sym+add)@got@tprel@ha */
#define R_PPC64_GOT_DTPREL16_DS	91 /* half16ds*	(sym+add)@got@dtprel */
#define R_PPC64_GOT_DTPREL16_LO_DS 92 /* half16ds (sym+add)@got@dtprel@l */
#define R_PPC64_GOT_DTPREL16_HI	93 /* half16	(sym+add)@got@dtprel@h */
#define R_PPC64_GOT_DTPREL16_HA	94 /* half16	(sym+add)@got@dtprel@ha */
#define R_PPC64_TPREL16_DS	95 /* half16ds*	(sym+add)@tprel */
#define R_PPC64_TPREL16_LO_DS	96 /* half16ds	(sym+add)@tprel@l */
#define R_PPC64_TPREL16_HIGHER	97 /* half16	(sym+add)@tprel@higher */
#define R_PPC64_TPREL16_HIGHERA	98 /* half16	(sym+add)@tprel@highera */
#define R_PPC64_TPREL16_HIGHEST	99 /* half16	(sym+add)@tprel@highest */
#define R_PPC64_TPREL16_HIGHESTA 100 /* half16	(sym+add)@tprel@highesta */
#define R_PPC64_DTPREL16_DS	101 /* half16ds* (sym+add)@dtprel */
#define R_PPC64_DTPREL16_LO_DS	102 /* half16ds	(sym+add)@dtprel@l */
#define R_PPC64_DTPREL16_HIGHER	103 /* half16	(sym+add)@dtprel@higher */
#define R_PPC64_DTPREL16_HIGHERA 104 /* half16	(sym+add)@dtprel@highera */
#define R_PPC64_DTPREL16_HIGHEST 105 /* half16	(sym+add)@dtprel@highest */
#define R_PPC64_DTPREL16_HIGHESTA 106 /* half16	(sym+add)@dtprel@highesta */
#define R_PPC64_TLSGD		107
#define R_PPC64_TLSLD		108
#define R_PPC64_TOCSAVE		109

#define R_PPC64_REL24_NOTOC	116
#define R_PPC64_ENTRY		118

#define R_PPC64_PCREL34		132
#define R_PPC64_GOT_PCREL34	133

#define R_PPC64_REL16		249
#define R_PPC64_REL16_LO	250
#define R_PPC64_REL16_HI	251
#define R_PPC64_REL16_HA	252

/* arm relocation types */
#define R_ARM_NONE		0
#define R_ARM_PC24		1
#define R_ARM_ABS32		2
#define R_ARM_REL32		3
#define R_ARM_CALL		28
#define R_ARM_JUMP24		29
#define R_ARM_TARGET1		38
#define R_ARM_V4BX		40
#define R_ARM_PREL31		42
#define R_ARM_MOVW_ABS_NC	43
#define R_ARM_MOVT_ABS		44
#define R_ARM_MOVW_PREL_NC	45
#define R_ARM_MOVT_PREL		46
#define R_ARM_ALU_PC_G0_NC	57
#define R_ARM_ALU_PC_G1_NC	59
#define R_ARM_LDR_PC_G2		63

#define R_ARM_THM_CALL		10
#define R_ARM_THM_JUMP24	30
#define R_ARM_THM_MOVW_ABS_NC	47
#define R_ARM_THM_MOVT_ABS	48
#define R_ARM_THM_MOVW_PREL_NC	49
#define R_ARM_THM_MOVT_PREL	50

/* SH (particularly SHcompact) relocation types  */
#define	R_SH_NONE		0
#define	R_SH_DIR32		1
#define	R_SH_REL32		2
#define	R_SH_DIR8WPN		3
#define	R_SH_IND12W		4
#define	R_SH_DIR8WPL		5
#define	R_SH_DIR8WPZ		6
#define	R_SH_DIR8BP		7
#define	R_SH_DIR8W		8
#define	R_SH_DIR8L		9
#define	R_SH_SWITCH16		25
#define	R_SH_SWITCH32		26
#define	R_SH_USES		27
#define	R_SH_COUNT		28
#define	R_SH_ALIGN		29
#define	R_SH_CODE		30
#define	R_SH_DATA		31
#define	R_SH_LABEL		32
#define	R_SH_SWITCH8		33
#define	R_SH_GNU_VTINHERIT	34
#define	R_SH_GNU_VTENTRY	35
#define	R_SH_TLS_GD_32		144
#define	R_SH_TLS_LD_32		145
#define	R_SH_TLS_LDO_32		146
#define	R_SH_TLS_IE_32		147
#define	R_SH_TLS_LE_32		148
#define	R_SH_TLS_DTPMOD32	149
#define	R_SH_TLS_DTPOFF32	150
#define	R_SH_TLS_TPOFF32	151
#define	R_SH_GOT32		160
#define	R_SH_PLT32		161
#define	R_SH_COPY		162
#define	R_SH_GLOB_DAT		163
#define	R_SH_JMP_SLOT		164
#define	R_SH_RELATIVE		165
#define	R_SH_GOTOFF		166
#define	R_SH_GOTPC		167

/* FDPIC relocs */
#define R_SH_GOT20		201
#define R_SH_GOTOFF20		202
#define R_SH_GOTFUNCDESC	203
#define R_SH_GOTFUNCDESC20	204
#define R_SH_GOTOFFFUNCDESC	205
#define R_SH_GOTOFFFUNCDESC20	206
#define R_SH_FUNCDESC		207
#define R_SH_FUNCDESC_VALUE	208

/* SHmedia relocs */
#define R_SH_IMM_LOW16		246
#define R_SH_IMM_LOW16_PCREL	247
#define R_SH_IMM_MEDLOW16	248
#define R_SH_IMM_MEDLOW16_PCREL	249

/* s390 relocations defined by the ABIs */
#define R_390_NONE		0	/* No reloc.  */
#define R_390_8			1	/* Direct 8 bit.  */
#define R_390_12		2	/* Direct 12 bit.  */
#define R_390_16		3	/* Direct 16 bit.  */
#define R_390_32		4	/* Direct 32 bit.  */
#define R_390_PC32		5	/* PC relative 32 bit.	*/
#define R_390_GOT12		6	/* 12 bit GOT offset.  */
#define R_390_GOT32		7	/* 32 bit GOT offset.  */
#define R_390_PLT32		8	/* 32 bit PC relative PLT address.  */
#define R_390_COPY		9	/* Copy symbol at runtime.  */
#define R_390_GLOB_DAT		10	/* Create GOT entry.  */
#define R_390_JMP_SLOT		11	/* Create PLT entry.  */
#define R_390_RELATIVE		12	/* Adjust by program base.  */
#define R_390_GOTOFF32		13	/* 32 bit offset to GOT.	 */
#define R_390_GOTPC		14	/* 32 bit PC rel. offset to GOT.  */
#define R_390_GOT16		15	/* 16 bit GOT offset.  */
#define R_390_PC16		16	/* PC relative 16 bit.	*/
#define R_390_PC16DBL		17	/* PC relative 16 bit shifted by 1.  */
#define R_390_PLT16DBL		18	/* 16 bit PC rel. PLT shifted by 1.  */
#define R_390_PC32DBL		19	/* PC relative 32 bit shifted by 1.  */
#define R_390_PLT32DBL		20	/* 32 bit PC rel. PLT shifted by 1.  */
#define R_390_GOTPCDBL		21	/* 32 bit PC rel. GOT shifted by 1.  */
#define R_390_64		22	/* Direct 64 bit.  */
#define R_390_PC64		23	/* PC relative 64 bit.	*/
#define R_390_GOT64		24	/* 64 bit GOT offset.  */
#define R_390_PLT64		25	/* 64 bit PC relative PLT address.  */
#define R_390_GOTENT		26	/* 32 bit PC rel. to GOT entry >> 1. */
#define R_390_GOTOFF16		27	/* 16 bit offset to GOT. */
#define R_390_GOTOFF64		28	/* 64 bit offset to GOT. */
#define R_390_GOTPLT12		29	/* 12 bit offset to jump slot.	*/
#define R_390_GOTPLT16		30	/* 16 bit offset to jump slot.	*/
#define R_390_GOTPLT32		31	/* 32 bit offset to jump slot.	*/
#define R_390_GOTPLT64		32	/* 64 bit offset to jump slot.	*/
#define R_390_GOTPLTENT		33	/* 32 bit rel. offset to jump slot.  */
#define R_390_PLTOFF16		34	/* 16 bit offset from GOT to PLT. */
#define R_390_PLTOFF32		35	/* 32 bit offset from GOT to PLT. */
#define R_390_PLTOFF64		36	/* 16 bit offset from GOT to PLT. */
#define R_390_TLS_LOAD		37	/* Tag for load insn in TLS code. */
#define R_390_TLS_GDCALL	38	/* Tag for function call in general
                                           dynamic TLS code.  */
#define R_390_TLS_LDCALL	39	/* Tag for function call in local
                                           dynamic TLS code.  */
#define R_390_TLS_GD32		40	/* Direct 32 bit for general dynamic
                                           thread local data.  */
#define R_390_TLS_GD64		41	/* Direct 64 bit for general dynamic
                                           thread local data.  */
#define R_390_TLS_GOTIE12	42	/* 12 bit GOT offset for static TLS
                                           block offset.  */
#define R_390_TLS_GOTIE32	43	/* 32 bit GOT offset for static TLS
                                           block offset.  */
#define R_390_TLS_GOTIE64	44	/* 64 bit GOT offset for static TLS
                                           block offset.  */
#define R_390_TLS_LDM32		45	/* Direct 32 bit for local dynamic
                                           thread local data in LD code.  */
#define R_390_TLS_LDM64		46	/* Direct 64 bit for local dynamic
                                           thread local data in LD code.  */
#define R_390_TLS_IE32		47	/* 32 bit address of GOT entry for
                                           negated static TLS block offset.  */
#define R_390_TLS_IE64		48	/* 64 bit address of GOT entry for
                                           negated static TLS block offset.  */
#define R_390_TLS_IEENT		49	/* 32 bit rel. offset to GOT entry for
                                           negated static TLS block offset.  */
#define R_390_TLS_LE32		50	/* 32 bit negated offset relative to
                                           static TLS block.  */
#define R_390_TLS_LE64		51	/* 64 bit negated offset relative to
                                           static TLS block.  */
#define R_390_TLS_LDO32		52	/* 32 bit offset relative to TLS
                                           block.  */
#define R_390_TLS_LDO64		53	/* 64 bit offset relative to TLS
                                           block.  */
#define R_390_TLS_DTPMOD	54	/* ID of module containing symbol.  */
#define R_390_TLS_DTPOFF	55	/* Offset in TLS block.  */
#define R_390_TLS_TPOFF		56	/* Negate offset in static TLS
                                           block.  */
#define R_390_20		57	/* Direct 20 bit.  */
#define R_390_GOT20		58	/* 20 bit GOT offset.  */
#define R_390_GOTPLT20		59	/* 20 bit offset to jump slot.  */
#define R_390_TLS_GOTIE20	60	/* 20 bit GOT offset for static TLS
					   block offset.  */

/* The OR1K relocation types... not all relevant for module loader */
#define R_OR1K_NONE		0
#define R_OR1K_32		1
#define R_OR1K_16		2
#define R_OR1K_8		3
#define R_OR1K_LO_16_IN_INSN	4
#define R_OR1K_HI_16_IN_INSN	5
#define R_OR1K_INSN_REL_26	6
#define R_OR1K_GNU_VTENTRY	7
#define R_OR1K_GNU_VTINHERIT	8
#define R_OR1K_32_PCREL		9
#define R_OR1K_16_PCREL		10
#define R_OR1K_8_PCREL		11
#define R_OR1K_GOTPC_HI16	12
#define R_OR1K_GOTPC_LO16	13
#define R_OR1K_GOT16		14
#define R_OR1K_PLT26		15
#define R_OR1K_GOTOFF_HI16	16
#define R_OR1K_GOTOFF_LO16	17
#define R_OR1K_COPY		18
#define R_OR1K_GLOB_DAT		19
#define R_OR1K_JMP_SLOT		20
#define R_OR1K_RELATIVE		21
#define R_OR1K_TLS_GD_HI16	22
#define R_OR1K_TLS_GD_LO16	23
#define R_OR1K_TLS_LDM_HI16	24
#define R_OR1K_TLS_LDM_LO16	25
#define R_OR1K_TLS_LDO_HI16	26
#define R_OR1K_TLS_LDO_LO16	27
#define R_OR1K_TLS_IE_HI16	28
#define R_OR1K_TLS_IE_LO16	29
#define R_OR1K_TLS_LE_HI16	30
#define R_OR1K_TLS_LE_LO16	31
#define R_OR1K_TLS_TPOFF	32
#define R_OR1K_TLS_DTPOFF	33
#define R_OR1K_TLS_DTPMOD	34
#define R_OR1K_AHI16		35
#define R_OR1K_GOTOFF_AHI16	36
#define R_OR1K_TLS_IE_AHI16	37
#define R_OR1K_TLS_LE_AHI16	38
#define R_OR1K_SLO16		39
#define R_OR1K_GOTOFF_SLO16	40
#define R_OR1K_TLS_LE_SLO16	41
#define R_OR1K_PCREL_PG21	42
#define R_OR1K_GOT_PG21		43
#define R_OR1K_TLS_GD_PG21	44
#define R_OR1K_TLS_LDM_PG21	45
#define R_OR1K_TLS_IE_PG21	46
#define R_OR1K_LO13		47
#define R_OR1K_GOT_LO13		48
#define R_OR1K_TLS_GD_LO13	49
#define R_OR1K_TLS_LDM_LO13	50
#define R_OR1K_TLS_IE_LO13	51
#define R_OR1K_SLO13		52
#define R_OR1K_PLTA26		53
#define R_OR1K_GOT_AHI16	54

/* Old relocation names */
#define R_OR32_NONE	R_OR1K_NONE
#define R_OR32_32	R_OR1K_32
#define R_OR32_16	R_OR1K_16
#define R_OR32_8	R_OR1K_8
#define R_OR32_CONST	R_OR1K_LO_16_IN_INSN
#define R_OR32_CONSTH	R_OR1K_HI_16_IN_INSN
#define R_OR32_JUMPTARG	R_OR1K_INSN_REL_26
#define R_OR32_VTENTRY	R_OR1K_GNU_VTENTRY
#define R_OR32_VTINHERIT R_OR1K_GNU_VTINHERIT

/* Xtensa relocations defined by the ABIs */
#define R_XTENSA_NONE           0
#define R_XTENSA_32             1
#define R_XTENSA_RTLD           2
#define R_XTENSA_GLOB_DAT       3
#define R_XTENSA_JMP_SLOT       4
#define R_XTENSA_RELATIVE       5
#define R_XTENSA_PLT            6
#define R_XTENSA_OP0            8
#define R_XTENSA_OP1            9
#define R_XTENSA_OP2            10
#define R_XTENSA_ASM_EXPAND	11
#define R_XTENSA_ASM_SIMPLIFY	12
#define R_XTENSA_GNU_VTINHERIT	15
#define R_XTENSA_GNU_VTENTRY	16
#define R_XTENSA_DIFF8		17
#define R_XTENSA_DIFF16		18
#define R_XTENSA_DIFF32		19
#define R_XTENSA_SLOT0_OP	20
#define R_XTENSA_SLOT1_OP	21
#define R_XTENSA_SLOT2_OP	22
#define R_XTENSA_SLOT3_OP	23
#define R_XTENSA_SLOT4_OP	24
#define R_XTENSA_SLOT5_OP	25
#define R_XTENSA_SLOT6_OP	26
#define R_XTENSA_SLOT7_OP	27
#define R_XTENSA_SLOT8_OP	28
#define R_XTENSA_SLOT9_OP	29
#define R_XTENSA_SLOT10_OP	30
#define R_XTENSA_SLOT11_OP	31
#define R_XTENSA_SLOT12_OP	32
#define R_XTENSA_SLOT13_OP	33
#define R_XTENSA_SLOT14_OP	34
#define R_XTENSA_SLOT0_ALT	35
#define R_XTENSA_SLOT1_ALT	36
#define R_XTENSA_SLOT2_ALT	37
#define R_XTENSA_SLOT3_ALT	38
#define R_XTENSA_SLOT4_ALT	39
#define R_XTENSA_SLOT5_ALT	40
#define R_XTENSA_SLOT6_ALT	41
#define R_XTENSA_SLOT7_ALT	42
#define R_XTENSA_SLOT8_ALT	43
#define R_XTENSA_SLOT9_ALT	44
#define R_XTENSA_SLOT10_ALT	45
#define R_XTENSA_SLOT11_ALT	46
#define R_XTENSA_SLOT12_ALT	47
#define R_XTENSA_SLOT13_ALT	48
#define R_XTENSA_SLOT14_ALT	49

/* Hexagon relocations */
  /* V2 */
#define R_HEXAGON_NONE           0
#define R_HEXAGON_B22_PCREL      1
#define R_HEXAGON_B15_PCREL      2
#define R_HEXAGON_B7_PCREL       3
#define R_HEXAGON_LO16           4
#define R_HEXAGON_HI16           5
#define R_HEXAGON_32             6
#define R_HEXAGON_16             7
#define R_HEXAGON_8              8
#define R_HEXAGON_GPREL16_0      9
#define R_HEXAGON_GPREL16_1     10
#define R_HEXAGON_GPREL16_2     11
#define R_HEXAGON_GPREL16_3     12
#define R_HEXAGON_HL16          13
  /* V3 */
#define R_HEXAGON_B13_PCREL     14
  /* V4 */
#define R_HEXAGON_B9_PCREL      15
  /* V4 (extenders) */
#define R_HEXAGON_B32_PCREL_X   16
#define R_HEXAGON_32_6_X        17
  /* V4 (extended) */
#define R_HEXAGON_B22_PCREL_X   18
#define R_HEXAGON_B15_PCREL_X   19
#define R_HEXAGON_B13_PCREL_X   20
#define R_HEXAGON_B9_PCREL_X    21
#define R_HEXAGON_B7_PCREL_X    22
#define R_HEXAGON_16_X          23
#define R_HEXAGON_12_X          24
#define R_HEXAGON_11_X          25
#define R_HEXAGON_10_X          26
#define R_HEXAGON_9_X           27
#define R_HEXAGON_8_X           28
#define R_HEXAGON_7_X           29
#define R_HEXAGON_6_X           30
  /* V2 PIC */
#define R_HEXAGON_32_PCREL      31
#define R_HEXAGON_COPY          32
#define R_HEXAGON_GLOB_DAT      33
#define R_HEXAGON_JMP_SLOT      34
#define R_HEXAGON_RELATIVE      35
#define R_HEXAGON_PLT_B22_PCREL 36
#define R_HEXAGON_GOTOFF_LO16   37
#define R_HEXAGON_GOTOFF_HI16   38
#define R_HEXAGON_GOTOFF_32     39
#define R_HEXAGON_GOT_LO16      40
#define R_HEXAGON_GOT_HI16      41
#define R_HEXAGON_GOT_32        42
#define R_HEXAGON_GOT_16        43

/* AArch64 static relocation types */

/* Miscellaneous. */
#define R_AARCH64_NONE			256

/* Data. */
#define R_AARCH64_ABS64			257
#define R_AARCH64_ABS32			258
#define R_AARCH64_ABS16			259
#define R_AARCH64_PREL64		260
#define R_AARCH64_PREL32		261
#define R_AARCH64_PREL16		262

/* Instructions. */
#define R_AARCH64_MOVW_UABS_G0		263
#define R_AARCH64_MOVW_UABS_G0_NC	264
#define R_AARCH64_MOVW_UABS_G1		265
#define R_AARCH64_MOVW_UABS_G1_NC	266
#define R_AARCH64_MOVW_UABS_G2		267
#define R_AARCH64_MOVW_UABS_G2_NC	268
#define R_AARCH64_MOVW_UABS_G3		269

#define R_AARCH64_MOVW_SABS_G0		270
#define R_AARCH64_MOVW_SABS_G1		271
#define R_AARCH64_MOVW_SABS_G2		272

#define R_AARCH64_LD_PREL_LO19		273
#define R_AARCH64_ADR_PREL_LO21		274
#define R_AARCH64_ADR_PREL_PG_HI21	275
#define R_AARCH64_ADR_PREL_PG_HI21_NC	276
#define R_AARCH64_ADD_ABS_LO12_NC	277
#define R_AARCH64_LDST8_ABS_LO12_NC	278

#define R_AARCH64_TSTBR14		279
#define R_AARCH64_CONDBR19		280
#define R_AARCH64_JUMP26		282
#define R_AARCH64_CALL26		283
#define R_AARCH64_LDST16_ABS_LO12_NC	284
#define R_AARCH64_LDST32_ABS_LO12_NC	285
#define R_AARCH64_LDST64_ABS_LO12_NC	286
#define R_AARCH64_LDST128_ABS_LO12_NC	299

#define R_AARCH64_MOVW_PREL_G0		287
#define R_AARCH64_MOVW_PREL_G0_NC	288
#define R_AARCH64_MOVW_PREL_G1		289
#define R_AARCH64_MOVW_PREL_G1_NC	290
#define R_AARCH64_MOVW_PREL_G2		291
#define R_AARCH64_MOVW_PREL_G2_NC	292
#define R_AARCH64_MOVW_PREL_G3		293

#define R_AARCH64_RELATIVE		1027

/* Microblaze Relocations */
#define R_MICROBLAZE_NONE 0
#define R_MICROBLAZE_32 1
#define R_MICROBLAZE_32_PCREL 2
#define R_MICROBLAZE_64_PCREL 3
#define R_MICROBLAZE_32_PCREL_LO 4
#define R_MICROBLAZE_64 5
#define R_MICROBLAZE_32_LO 6
#define R_MICROBLAZE_SRO32 7
#define R_MICROBLAZE_SRW32 8
#define R_MICROBLAZE_64_NONE 9
#define R_MICROBLAZE_32_SYM_OP_SYM 10

#endif /* _UAPI_LINUX_ELF_R_H */
