#ifndef _ASM_X86_ESPFIX_H
#define _ASM_X86_ESPFIX_H

#ifdef CONFIG_X86_64

#include <asm/percpu.h>

DECLARE_PER_CPU_READ_MOSTLY(unsigned long, espfix_stack);
DECLARE_PER_CPU_READ_MOSTLY(unsigned long, espfix_waddr);

extern void init_espfix_bsp(void);
<<<<<<< HEAD
extern void init_espfix_ap(void);
=======
extern void init_espfix_ap(int cpu);
>>>>>>> v4.9.227

#endif /* CONFIG_X86_64 */

#endif /* _ASM_X86_ESPFIX_H */
