#include <linux/linkage.h>
<<<<<<< HEAD
=======
#include <asm/export.h>
>>>>>>> v4.9.227

	.text
	.align	32
ENTRY(__arch_hweight8)
	ba,pt	%xcc, __sw_hweight8
	 nop
	nop
ENDPROC(__arch_hweight8)
<<<<<<< HEAD
=======
EXPORT_SYMBOL(__arch_hweight8)
>>>>>>> v4.9.227
	.section	.popc_3insn_patch, "ax"
	.word		__arch_hweight8
	sllx		%o0, 64-8, %g1
	retl
	 popc		%g1, %o0
	.previous

ENTRY(__arch_hweight16)
	ba,pt	%xcc, __sw_hweight16
	 nop
	nop
ENDPROC(__arch_hweight16)
<<<<<<< HEAD
=======
EXPORT_SYMBOL(__arch_hweight16)
>>>>>>> v4.9.227
	.section	.popc_3insn_patch, "ax"
	.word		__arch_hweight16
	sllx		%o0, 64-16, %g1
	retl
	 popc		%g1, %o0
	.previous

ENTRY(__arch_hweight32)
	ba,pt	%xcc, __sw_hweight32
	 nop
	nop
ENDPROC(__arch_hweight32)
<<<<<<< HEAD
=======
EXPORT_SYMBOL(__arch_hweight32)
>>>>>>> v4.9.227
	.section	.popc_3insn_patch, "ax"
	.word		__arch_hweight32
	sllx		%o0, 64-32, %g1
	retl
	 popc		%g1, %o0
	.previous

ENTRY(__arch_hweight64)
	ba,pt	%xcc, __sw_hweight64
	 nop
	nop
ENDPROC(__arch_hweight64)
<<<<<<< HEAD
=======
EXPORT_SYMBOL(__arch_hweight64)
>>>>>>> v4.9.227
	.section	.popc_3insn_patch, "ax"
	.word		__arch_hweight64
	retl
	 popc		%o0, %o0
	nop
	.previous
