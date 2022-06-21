/*
 * x86 specific code for irq_work
 *
<<<<<<< HEAD
 * Copyright (C) 2010 Red Hat, Inc., Peter Zijlstra <pzijlstr@redhat.com>
=======
 * Copyright (C) 2010 Red Hat, Inc., Peter Zijlstra
>>>>>>> v4.9.227
 */

#include <linux/kernel.h>
#include <linux/irq_work.h>
#include <linux/hardirq.h>
#include <asm/apic.h>
#include <asm/trace/irq_vectors.h>
<<<<<<< HEAD

static inline void irq_work_entering_irq(void)
{
	irq_enter();
	ack_APIC_irq();
}
=======
#include <linux/interrupt.h>
>>>>>>> v4.9.227

static inline void __smp_irq_work_interrupt(void)
{
	inc_irq_stat(apic_irq_work_irqs);
	irq_work_run();
}

<<<<<<< HEAD
__visible void smp_irq_work_interrupt(struct pt_regs *regs)
{
	irq_work_entering_irq();
=======
__visible void __irq_entry smp_irq_work_interrupt(struct pt_regs *regs)
{
	ipi_entering_ack_irq();
>>>>>>> v4.9.227
	__smp_irq_work_interrupt();
	exiting_irq();
}

<<<<<<< HEAD
__visible void smp_trace_irq_work_interrupt(struct pt_regs *regs)
{
	irq_work_entering_irq();
=======
__visible void __irq_entry smp_trace_irq_work_interrupt(struct pt_regs *regs)
{
	ipi_entering_ack_irq();
>>>>>>> v4.9.227
	trace_irq_work_entry(IRQ_WORK_VECTOR);
	__smp_irq_work_interrupt();
	trace_irq_work_exit(IRQ_WORK_VECTOR);
	exiting_irq();
}

void arch_irq_work_raise(void)
{
#ifdef CONFIG_X86_LOCAL_APIC
	if (!arch_irq_work_has_interrupt())
		return;

	apic->send_IPI_self(IRQ_WORK_VECTOR);
	apic_wait_icr_idle();
#endif
}
