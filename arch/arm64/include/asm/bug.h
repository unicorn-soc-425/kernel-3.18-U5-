<<<<<<< HEAD
#ifndef _ASMARM_BUG_H
#define _ASMARM_BUG_H

#include <linux/linkage.h>

#ifdef CONFIG_BUG

#define BUG_INSTR_VALUE 0xe7f001f2
#define BUG_INSTR_TYPE ".word "

#define BUG() _BUG(__FILE__, __LINE__, BUG_INSTR_VALUE)
#define _BUG(file, line, value) __BUG(file, line, value)

#ifdef CONFIG_DEBUG_BUGVERBOSE

/*
 * The extra indirection is to ensure that the __FILE__ string comes through
 * OK. Many version of gcc do not support the asm %c parameter which would be
 * preferable to this unpleasantness. We use mergeable string sections to
 * avoid multiple copies of the string appearing in the kernel image.
 */

#define __BUG(__file, __line, __value)				\
do {								\
	asm volatile("1:\t" BUG_INSTR_TYPE #__value "\n"	\
		".pushsection .rodata.str, \"aMS\", 1\n"	\
		"2:\t.asciz " #__file "\n"			\
		".popsection\n"					\
		".pushsection __bug_table,\"a\"\n"		\
		"3:\t.quad 1b, 2b\n"				\
		"\t.word " #__line ", 0\n"			\
		".popsection");					\
	unreachable();						\
} while (0)

#else  /* not CONFIG_DEBUG_BUGVERBOSE */

#define __BUG(__file, __line, __value)				\
do {								\
	asm volatile(BUG_INSTR_TYPE #__value);			\
	unreachable();						\
} while (0)
#endif  /* CONFIG_DEBUG_BUGVERBOSE */

#define HAVE_ARCH_BUG
#endif  /* CONFIG_BUG */

#include <asm-generic/bug.h>

#endif
=======
/*
 * Copyright (C) 2015  ARM Limited
 * Author: Dave Martin <Dave.Martin@arm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ARCH_ARM64_ASM_BUG_H
#define _ARCH_ARM64_ASM_BUG_H

#include <asm/brk-imm.h>

#ifdef CONFIG_DEBUG_BUGVERBOSE
#define _BUGVERBOSE_LOCATION(file, line) __BUGVERBOSE_LOCATION(file, line)
#define __BUGVERBOSE_LOCATION(file, line)				\
		".pushsection .rodata.str,\"aMS\",@progbits,1\n"	\
	"2:	.string \"" file "\"\n\t"				\
		".popsection\n\t"					\
									\
		".long 2b - 0b\n\t"					\
		".short " #line "\n\t"
#else
#define _BUGVERBOSE_LOCATION(file, line)
#endif

#ifdef CONFIG_GENERIC_BUG

#define __BUG_ENTRY(flags) 				\
		".pushsection __bug_table,\"a\"\n\t"	\
		".align 2\n\t"				\
	"0:	.long 1f - 0b\n\t"			\
_BUGVERBOSE_LOCATION(__FILE__, __LINE__)		\
		".short " #flags "\n\t"			\
		".popsection\n"				\
	"1:	"
#else
#define __BUG_ENTRY(flags) ""
#endif

#define __BUG_FLAGS(flags)				\
	asm volatile (					\
		__BUG_ENTRY(flags)			\
		"brk %[imm]" :: [imm] "i" (BUG_BRK_IMM)	\
	);


#define BUG() do {					\
	__BUG_FLAGS(0);					\
	unreachable();					\
} while (0)

#define __WARN_TAINT(taint) 				\
	__BUG_FLAGS(BUGFLAG_TAINT(taint))

#define HAVE_ARCH_BUG

#include <asm-generic/bug.h>

#endif /* ! _ARCH_ARM64_ASM_BUG_H */
>>>>>>> v4.9.227
