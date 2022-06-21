#ifndef __ASM_CPUIDLE_H
#define __ASM_CPUIDLE_H

<<<<<<< HEAD
#ifdef CONFIG_CPU_IDLE
extern int cpu_init_idle(unsigned int cpu);
#else
static inline int cpu_init_idle(unsigned int cpu)
=======
#include <asm/proc-fns.h>

#ifdef CONFIG_CPU_IDLE
extern int arm_cpuidle_init(unsigned int cpu);
extern int arm_cpuidle_suspend(int index);
#else
static inline int arm_cpuidle_init(unsigned int cpu)
{
	return -EOPNOTSUPP;
}

static inline int arm_cpuidle_suspend(int index)
>>>>>>> v4.9.227
{
	return -EOPNOTSUPP;
}
#endif
<<<<<<< HEAD

=======
>>>>>>> v4.9.227
#endif
