#ifndef _ASM_POWERPC_LINKAGE_H
#define _ASM_POWERPC_LINKAGE_H

<<<<<<< HEAD
#ifdef CONFIG_PPC64
#if !defined(_CALL_ELF) || _CALL_ELF != 2
=======
#include <asm/types.h>

#ifdef PPC64_ELF_ABI_v1
>>>>>>> v4.9.227
#define cond_syscall(x) \
	asm ("\t.weak " #x "\n\t.set " #x ", sys_ni_syscall\n"		\
	     "\t.weak ." #x "\n\t.set ." #x ", .sys_ni_syscall\n")
#define SYSCALL_ALIAS(alias, name)					\
	asm ("\t.globl " #alias "\n\t.set " #alias ", " #name "\n"	\
	     "\t.globl ." #alias "\n\t.set ." #alias ", ." #name)
#endif
<<<<<<< HEAD
#endif
=======
>>>>>>> v4.9.227

#endif	/* _ASM_POWERPC_LINKAGE_H */
