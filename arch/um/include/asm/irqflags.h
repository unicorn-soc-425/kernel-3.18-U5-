#ifndef __UM_IRQFLAGS_H
#define __UM_IRQFLAGS_H

extern int get_signals(void);
extern int set_signals(int enable);
extern void block_signals(void);
extern void unblock_signals(void);

<<<<<<< HEAD
=======
#define arch_local_save_flags arch_local_save_flags
>>>>>>> v4.9.227
static inline unsigned long arch_local_save_flags(void)
{
	return get_signals();
}

<<<<<<< HEAD
=======
#define arch_local_irq_restore arch_local_irq_restore
>>>>>>> v4.9.227
static inline void arch_local_irq_restore(unsigned long flags)
{
	set_signals(flags);
}

<<<<<<< HEAD
=======
#define arch_local_irq_enable arch_local_irq_enable
>>>>>>> v4.9.227
static inline void arch_local_irq_enable(void)
{
	unblock_signals();
}

<<<<<<< HEAD
=======
#define arch_local_irq_disable arch_local_irq_disable
>>>>>>> v4.9.227
static inline void arch_local_irq_disable(void)
{
	block_signals();
}

<<<<<<< HEAD
static inline unsigned long arch_local_irq_save(void)
{
	unsigned long flags;
	flags = arch_local_save_flags();
	arch_local_irq_disable();
	return flags;
}

static inline bool arch_irqs_disabled(void)
{
	return arch_local_save_flags() == 0;
}
=======
#define ARCH_IRQ_DISABLED	0
#define ARCh_IRQ_ENABLED	(SIGIO|SIGVTALRM)

#include <asm-generic/irqflags.h>
>>>>>>> v4.9.227

#endif
