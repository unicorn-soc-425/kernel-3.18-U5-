/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (c) 2010 Cavium Networks, Inc.
 */
#ifndef _ASM_MIPS_JUMP_LABEL_H
#define _ASM_MIPS_JUMP_LABEL_H

<<<<<<< HEAD
#include <linux/types.h>

#ifdef __KERNEL__
=======
#ifndef __ASSEMBLY__

#include <linux/types.h>
>>>>>>> v4.9.227

#define JUMP_LABEL_NOP_SIZE 4

#ifdef CONFIG_64BIT
#define WORD_INSN ".dword"
#else
#define WORD_INSN ".word"
#endif

#ifdef CONFIG_CPU_MICROMIPS
<<<<<<< HEAD
#define NOP_INSN "nop32"
#else
#define NOP_INSN "nop"
#endif

static __always_inline bool arch_static_branch(struct static_key *key)
{
	asm_volatile_goto("1:\t" NOP_INSN "\n\t"
		"nop\n\t"
		".pushsection __jump_table,  \"aw\"\n\t"
		WORD_INSN " 1b, %l[l_yes], %0\n\t"
		".popsection\n\t"
		: :  "i" (key) : : l_yes);
=======
#define B_INSN "b32"
#else
#define B_INSN "b"
#endif

static __always_inline bool arch_static_branch(struct static_key *key, bool branch)
{
	asm_volatile_goto("1:\t" B_INSN " 2f\n\t"
		"2:\tnop\n\t"
		".pushsection __jump_table,  \"aw\"\n\t"
		WORD_INSN " 1b, %l[l_yes], %0\n\t"
		".popsection\n\t"
		: :  "i" (&((char *)key)[branch]) : : l_yes);

>>>>>>> v4.9.227
	return false;
l_yes:
	return true;
}

<<<<<<< HEAD
#endif /* __KERNEL__ */
=======
static __always_inline bool arch_static_branch_jump(struct static_key *key, bool branch)
{
	asm_volatile_goto("1:\tj %l[l_yes]\n\t"
		"nop\n\t"
		".pushsection __jump_table,  \"aw\"\n\t"
		WORD_INSN " 1b, %l[l_yes], %0\n\t"
		".popsection\n\t"
		: :  "i" (&((char *)key)[branch]) : : l_yes);

	return false;
l_yes:
	return true;
}
>>>>>>> v4.9.227

#ifdef CONFIG_64BIT
typedef u64 jump_label_t;
#else
typedef u32 jump_label_t;
#endif

struct jump_entry {
	jump_label_t code;
	jump_label_t target;
	jump_label_t key;
};

<<<<<<< HEAD
=======
#endif  /* __ASSEMBLY__ */
>>>>>>> v4.9.227
#endif /* _ASM_MIPS_JUMP_LABEL_H */
