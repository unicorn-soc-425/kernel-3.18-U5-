/*
 * __put_user functions.
 *
 * (C) Copyright 2005 Linus Torvalds
 * (C) Copyright 2005 Andi Kleen
 * (C) Copyright 2008 Glauber Costa
 *
 * These functions have a non-standard call interface
 * to make them more efficient, especially as they
 * return an error value in addition to the "real"
 * return value.
 */
#include <linux/linkage.h>
<<<<<<< HEAD
#include <asm/dwarf2.h>
=======
>>>>>>> v4.9.227
#include <asm/thread_info.h>
#include <asm/errno.h>
#include <asm/asm.h>
#include <asm/smap.h>
<<<<<<< HEAD
=======
#include <asm/export.h>
>>>>>>> v4.9.227


/*
 * __put_user_X
 *
 * Inputs:	%eax[:%edx] contains the data
 *		%ecx contains the address
 *
 * Outputs:	%eax is error code (0 or -EFAULT)
 *
 * These functions should not modify any other registers,
 * as they get called from within inline assembly.
 */

<<<<<<< HEAD
#define ENTER	CFI_STARTPROC ; \
		GET_THREAD_INFO(%_ASM_BX)
#define EXIT	ASM_CLAC ;	\
		ret ;		\
		CFI_ENDPROC
=======
#define ENTER	mov PER_CPU_VAR(current_task), %_ASM_BX
#define EXIT	ASM_CLAC ;	\
		ret
>>>>>>> v4.9.227

.text
ENTRY(__put_user_1)
	ENTER
<<<<<<< HEAD
	cmp TI_addr_limit(%_ASM_BX),%_ASM_CX
=======
	cmp TASK_addr_limit(%_ASM_BX),%_ASM_CX
>>>>>>> v4.9.227
	jae bad_put_user
	ASM_STAC
1:	movb %al,(%_ASM_CX)
	xor %eax,%eax
	EXIT
ENDPROC(__put_user_1)
<<<<<<< HEAD

ENTRY(__put_user_2)
	ENTER
	mov TI_addr_limit(%_ASM_BX),%_ASM_BX
=======
EXPORT_SYMBOL(__put_user_1)

ENTRY(__put_user_2)
	ENTER
	mov TASK_addr_limit(%_ASM_BX),%_ASM_BX
>>>>>>> v4.9.227
	sub $1,%_ASM_BX
	cmp %_ASM_BX,%_ASM_CX
	jae bad_put_user
	ASM_STAC
2:	movw %ax,(%_ASM_CX)
	xor %eax,%eax
	EXIT
ENDPROC(__put_user_2)
<<<<<<< HEAD

ENTRY(__put_user_4)
	ENTER
	mov TI_addr_limit(%_ASM_BX),%_ASM_BX
=======
EXPORT_SYMBOL(__put_user_2)

ENTRY(__put_user_4)
	ENTER
	mov TASK_addr_limit(%_ASM_BX),%_ASM_BX
>>>>>>> v4.9.227
	sub $3,%_ASM_BX
	cmp %_ASM_BX,%_ASM_CX
	jae bad_put_user
	ASM_STAC
3:	movl %eax,(%_ASM_CX)
	xor %eax,%eax
	EXIT
ENDPROC(__put_user_4)
<<<<<<< HEAD

ENTRY(__put_user_8)
	ENTER
	mov TI_addr_limit(%_ASM_BX),%_ASM_BX
=======
EXPORT_SYMBOL(__put_user_4)

ENTRY(__put_user_8)
	ENTER
	mov TASK_addr_limit(%_ASM_BX),%_ASM_BX
>>>>>>> v4.9.227
	sub $7,%_ASM_BX
	cmp %_ASM_BX,%_ASM_CX
	jae bad_put_user
	ASM_STAC
4:	mov %_ASM_AX,(%_ASM_CX)
#ifdef CONFIG_X86_32
5:	movl %edx,4(%_ASM_CX)
#endif
	xor %eax,%eax
	EXIT
ENDPROC(__put_user_8)
<<<<<<< HEAD

bad_put_user:
	CFI_STARTPROC
=======
EXPORT_SYMBOL(__put_user_8)

bad_put_user:
>>>>>>> v4.9.227
	movl $-EFAULT,%eax
	EXIT
END(bad_put_user)

	_ASM_EXTABLE(1b,bad_put_user)
	_ASM_EXTABLE(2b,bad_put_user)
	_ASM_EXTABLE(3b,bad_put_user)
	_ASM_EXTABLE(4b,bad_put_user)
#ifdef CONFIG_X86_32
	_ASM_EXTABLE(5b,bad_put_user)
#endif
