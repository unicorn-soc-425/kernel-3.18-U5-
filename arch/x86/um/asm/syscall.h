#ifndef __UM_ASM_SYSCALL_H
#define __UM_ASM_SYSCALL_H

<<<<<<< HEAD
#include <uapi/linux/audit.h>

=======
#include <asm/syscall-generic.h>
#include <uapi/linux/audit.h>

typedef asmlinkage long (*sys_call_ptr_t)(unsigned long, unsigned long,
					  unsigned long, unsigned long,
					  unsigned long, unsigned long);

>>>>>>> v4.9.227
static inline int syscall_get_arch(void)
{
#ifdef CONFIG_X86_32
	return AUDIT_ARCH_I386;
#else
	return AUDIT_ARCH_X86_64;
#endif
}

#endif /* __UM_ASM_SYSCALL_H */
