<<<<<<< HEAD
#ifdef __ASSEMBLY__

#include <asm/asm.h>
#include <asm/dwarf2.h>

/* The annotation hides the frame from the unwinder and makes it look
   like a ordinary ebp save/restore. This avoids some special cases for
   frame pointer later */
#ifdef CONFIG_FRAME_POINTER
	.macro FRAME
	__ASM_SIZE(push,_cfi)	%__ASM_REG(bp)
	CFI_REL_OFFSET		__ASM_REG(bp), 0
	__ASM_SIZE(mov)		%__ASM_REG(sp), %__ASM_REG(bp)
	.endm
	.macro ENDFRAME
	__ASM_SIZE(pop,_cfi)	%__ASM_REG(bp)
	CFI_RESTORE		__ASM_REG(bp)
	.endm
#else
	.macro FRAME
	.endm
	.macro ENDFRAME
	.endm
#endif

#endif  /*  __ASSEMBLY__  */
=======
#ifndef _ASM_X86_FRAME_H
#define _ASM_X86_FRAME_H

#include <asm/asm.h>

/*
 * These are stack frame creation macros.  They should be used by every
 * callable non-leaf asm function to make kernel stack traces more reliable.
 */

#ifdef CONFIG_FRAME_POINTER

#ifdef __ASSEMBLY__

.macro FRAME_BEGIN
	push %_ASM_BP
	_ASM_MOV %_ASM_SP, %_ASM_BP
.endm

.macro FRAME_END
	pop %_ASM_BP
.endm

#else /* !__ASSEMBLY__ */

#define FRAME_BEGIN				\
	"push %" _ASM_BP "\n"			\
	_ASM_MOV "%" _ASM_SP ", %" _ASM_BP "\n"

#define FRAME_END "pop %" _ASM_BP "\n"

#endif /* __ASSEMBLY__ */

#define FRAME_OFFSET __ASM_SEL(4, 8)

#else /* !CONFIG_FRAME_POINTER */

#define FRAME_BEGIN
#define FRAME_END
#define FRAME_OFFSET 0

#endif /* CONFIG_FRAME_POINTER */

#endif /* _ASM_X86_FRAME_H */
>>>>>>> v4.9.227
