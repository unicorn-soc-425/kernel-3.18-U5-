#ifndef _ASM_IRQ_WORK_H
#define _ASM_IRQ_WORK_H

<<<<<<< HEAD
#include <asm/processor.h>

static inline bool arch_irq_work_has_interrupt(void)
{
	return cpu_has_apic;
=======
#include <asm/cpufeature.h>

static inline bool arch_irq_work_has_interrupt(void)
{
	return boot_cpu_has(X86_FEATURE_APIC);
>>>>>>> v4.9.227
}

#endif /* _ASM_IRQ_WORK_H */
