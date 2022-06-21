/*
 * x86 semaphore implementation.
 *
 * (C) Copyright 1999 Linus Torvalds
 *
 * Portions Copyright 1999 Red Hat, Inc.
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *
 * rw semaphores implemented November 1999 by Benjamin LaHaise <bcrl@kvack.org>
 */

#include <linux/linkage.h>
#include <asm/alternative-asm.h>
<<<<<<< HEAD
#include <asm/dwarf2.h>
=======
#include <asm/frame.h>
>>>>>>> v4.9.227

#define __ASM_HALF_REG(reg)	__ASM_SEL(reg, e##reg)
#define __ASM_HALF_SIZE(inst)	__ASM_SEL(inst##w, inst##l)

#ifdef CONFIG_X86_32

/*
 * The semaphore operations have a special calling sequence that
 * allow us to do a simpler in-line version of them. These routines
 * need to convert that sequence back into the C sequence when
 * there is contention on the semaphore.
 *
 * %eax contains the semaphore pointer on entry. Save the C-clobbered
<<<<<<< HEAD
 * registers (%eax, %edx and %ecx) except %eax whish is either a return
 * value or just clobbered..
 */

#define save_common_regs \
	pushl_cfi %ecx; CFI_REL_OFFSET ecx, 0

#define restore_common_regs \
	popl_cfi %ecx; CFI_RESTORE ecx
=======
 * registers (%eax, %edx and %ecx) except %eax which is either a return
 * value or just gets clobbered. Same is true for %edx so make sure GCC
 * reloads it after the slow path, by making it hold a temporary, for
 * example see ____down_write().
 */

#define save_common_regs \
	pushl %ecx

#define restore_common_regs \
	popl %ecx
>>>>>>> v4.9.227

	/* Avoid uglifying the argument copying x86-64 needs to do. */
	.macro movq src, dst
	.endm

#else

/*
 * x86-64 rwsem wrappers
 *
 * This interfaces the inline asm code to the slow-path
 * C routines. We need to save the call-clobbered regs
 * that the asm does not mark as clobbered, and move the
 * argument from %rax to %rdi.
 *
 * NOTE! We don't need to save %rax, because the functions
 * will always return the semaphore pointer in %rax (which
 * is also the input argument to these helpers)
 *
 * The following can clobber %rdx because the asm clobbers it:
 *   call_rwsem_down_write_failed
 *   call_rwsem_wake
 * but %rdi, %rsi, %rcx, %r8-r11 always need saving.
 */

#define save_common_regs \
<<<<<<< HEAD
	pushq_cfi %rdi; CFI_REL_OFFSET rdi, 0; \
	pushq_cfi %rsi; CFI_REL_OFFSET rsi, 0; \
	pushq_cfi %rcx; CFI_REL_OFFSET rcx, 0; \
	pushq_cfi %r8;  CFI_REL_OFFSET r8,  0; \
	pushq_cfi %r9;  CFI_REL_OFFSET r9,  0; \
	pushq_cfi %r10; CFI_REL_OFFSET r10, 0; \
	pushq_cfi %r11; CFI_REL_OFFSET r11, 0

#define restore_common_regs \
	popq_cfi %r11; CFI_RESTORE r11; \
	popq_cfi %r10; CFI_RESTORE r10; \
	popq_cfi %r9;  CFI_RESTORE r9; \
	popq_cfi %r8;  CFI_RESTORE r8; \
	popq_cfi %rcx; CFI_RESTORE rcx; \
	popq_cfi %rsi; CFI_RESTORE rsi; \
	popq_cfi %rdi; CFI_RESTORE rdi
=======
	pushq %rdi; \
	pushq %rsi; \
	pushq %rcx; \
	pushq %r8;  \
	pushq %r9;  \
	pushq %r10; \
	pushq %r11

#define restore_common_regs \
	popq %r11; \
	popq %r10; \
	popq %r9; \
	popq %r8; \
	popq %rcx; \
	popq %rsi; \
	popq %rdi
>>>>>>> v4.9.227

#endif

/* Fix up special calling conventions */
ENTRY(call_rwsem_down_read_failed)
<<<<<<< HEAD
	CFI_STARTPROC
	save_common_regs
	__ASM_SIZE(push,_cfi) %__ASM_REG(dx)
	CFI_REL_OFFSET __ASM_REG(dx), 0
	movq %rax,%rdi
	call rwsem_down_read_failed
	__ASM_SIZE(pop,_cfi) %__ASM_REG(dx)
	CFI_RESTORE __ASM_REG(dx)
	restore_common_regs
	ret
	CFI_ENDPROC
ENDPROC(call_rwsem_down_read_failed)

ENTRY(call_rwsem_down_write_failed)
	CFI_STARTPROC
=======
	FRAME_BEGIN
	save_common_regs
	__ASM_SIZE(push,) %__ASM_REG(dx)
	movq %rax,%rdi
	call rwsem_down_read_failed
	__ASM_SIZE(pop,) %__ASM_REG(dx)
	restore_common_regs
	FRAME_END
	ret
ENDPROC(call_rwsem_down_read_failed)

ENTRY(call_rwsem_down_write_failed)
	FRAME_BEGIN
>>>>>>> v4.9.227
	save_common_regs
	movq %rax,%rdi
	call rwsem_down_write_failed
	restore_common_regs
<<<<<<< HEAD
	ret
	CFI_ENDPROC
ENDPROC(call_rwsem_down_write_failed)

ENTRY(call_rwsem_wake)
	CFI_STARTPROC
=======
	FRAME_END
	ret
ENDPROC(call_rwsem_down_write_failed)

ENTRY(call_rwsem_down_write_failed_killable)
	FRAME_BEGIN
	save_common_regs
	movq %rax,%rdi
	call rwsem_down_write_failed_killable
	restore_common_regs
	FRAME_END
	ret
ENDPROC(call_rwsem_down_write_failed_killable)

ENTRY(call_rwsem_wake)
	FRAME_BEGIN
>>>>>>> v4.9.227
	/* do nothing if still outstanding active readers */
	__ASM_HALF_SIZE(dec) %__ASM_HALF_REG(dx)
	jnz 1f
	save_common_regs
	movq %rax,%rdi
	call rwsem_wake
	restore_common_regs
<<<<<<< HEAD
1:	ret
	CFI_ENDPROC
ENDPROC(call_rwsem_wake)

ENTRY(call_rwsem_downgrade_wake)
	CFI_STARTPROC
	save_common_regs
	__ASM_SIZE(push,_cfi) %__ASM_REG(dx)
	CFI_REL_OFFSET __ASM_REG(dx), 0
	movq %rax,%rdi
	call rwsem_downgrade_wake
	__ASM_SIZE(pop,_cfi) %__ASM_REG(dx)
	CFI_RESTORE __ASM_REG(dx)
	restore_common_regs
	ret
	CFI_ENDPROC
=======
1:	FRAME_END
	ret
ENDPROC(call_rwsem_wake)

ENTRY(call_rwsem_downgrade_wake)
	FRAME_BEGIN
	save_common_regs
	__ASM_SIZE(push,) %__ASM_REG(dx)
	movq %rax,%rdi
	call rwsem_downgrade_wake
	__ASM_SIZE(pop,) %__ASM_REG(dx)
	restore_common_regs
	FRAME_END
	ret
>>>>>>> v4.9.227
ENDPROC(call_rwsem_downgrade_wake)
