#ifndef __ASM_SECCOMP_H

#include <linux/unistd.h>

<<<<<<< HEAD
#define __NR_seccomp_read __NR_read
#define __NR_seccomp_write __NR_write
#define __NR_seccomp_exit __NR_exit
#define __NR_seccomp_sigreturn __NR_rt_sigreturn

/*
 * Kludge alert:
 *
 * The generic seccomp code currently allows only a single compat ABI.	Until
 * this is fixed we priorize O32 as the compat ABI over N32.
 */
#ifdef CONFIG_MIPS32_O32

#define __NR_seccomp_read_32		4003
#define __NR_seccomp_write_32		4004
#define __NR_seccomp_exit_32		4001
#define __NR_seccomp_sigreturn_32	4193	/* rt_sigreturn */

#elif defined(CONFIG_MIPS32_N32)

#define __NR_seccomp_read_32		6000
#define __NR_seccomp_write_32		6001
#define __NR_seccomp_exit_32		6058
#define __NR_seccomp_sigreturn_32	6211	/* rt_sigreturn */

#endif /* CONFIG_MIPS32_O32 */
=======
#ifdef CONFIG_COMPAT
static inline const int *get_compat_mode1_syscalls(void)
{
	static const int syscalls_O32[] = {
		__NR_O32_Linux + 3, __NR_O32_Linux + 4,
		__NR_O32_Linux + 1, __NR_O32_Linux + 193,
		0, /* null terminated */
	};
	static const int syscalls_N32[] = {
		__NR_N32_Linux + 0, __NR_N32_Linux + 1,
		__NR_N32_Linux + 58, __NR_N32_Linux + 211,
		0, /* null terminated */
	};

	if (IS_ENABLED(CONFIG_MIPS32_O32) && test_thread_flag(TIF_32BIT_REGS))
		return syscalls_O32;

	if (IS_ENABLED(CONFIG_MIPS32_N32))
		return syscalls_N32;

	BUG();
}

#define get_compat_mode1_syscalls get_compat_mode1_syscalls

#endif /* CONFIG_COMPAT */

#include <asm-generic/seccomp.h>
>>>>>>> v4.9.227

#endif /* __ASM_SECCOMP_H */
