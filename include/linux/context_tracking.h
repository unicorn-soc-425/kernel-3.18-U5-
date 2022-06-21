#ifndef _LINUX_CONTEXT_TRACKING_H
#define _LINUX_CONTEXT_TRACKING_H

#include <linux/sched.h>
#include <linux/vtime.h>
#include <linux/context_tracking_state.h>
#include <asm/ptrace.h>


#ifdef CONFIG_CONTEXT_TRACKING
extern void context_tracking_cpu_set(int cpu);

<<<<<<< HEAD
extern void context_tracking_user_enter(void);
extern void context_tracking_user_exit(void);
extern void __context_tracking_task_switch(struct task_struct *prev,
					   struct task_struct *next);
=======
/* Called with interrupts disabled.  */
extern void __context_tracking_enter(enum ctx_state state);
extern void __context_tracking_exit(enum ctx_state state);

extern void context_tracking_enter(enum ctx_state state);
extern void context_tracking_exit(enum ctx_state state);
extern void context_tracking_user_enter(void);
extern void context_tracking_user_exit(void);
>>>>>>> v4.9.227

static inline void user_enter(void)
{
	if (context_tracking_is_enabled())
<<<<<<< HEAD
		context_tracking_user_enter();
=======
		context_tracking_enter(CONTEXT_USER);
>>>>>>> v4.9.227

}
static inline void user_exit(void)
{
	if (context_tracking_is_enabled())
<<<<<<< HEAD
		context_tracking_user_exit();
=======
		context_tracking_exit(CONTEXT_USER);
}

/* Called with interrupts disabled.  */
static inline void user_enter_irqoff(void)
{
	if (context_tracking_is_enabled())
		__context_tracking_enter(CONTEXT_USER);

}
static inline void user_exit_irqoff(void)
{
	if (context_tracking_is_enabled())
		__context_tracking_exit(CONTEXT_USER);
>>>>>>> v4.9.227
}

static inline enum ctx_state exception_enter(void)
{
	enum ctx_state prev_ctx;

	if (!context_tracking_is_enabled())
		return 0;

	prev_ctx = this_cpu_read(context_tracking.state);
<<<<<<< HEAD
	context_tracking_user_exit();
=======
	if (prev_ctx != CONTEXT_KERNEL)
		context_tracking_exit(prev_ctx);
>>>>>>> v4.9.227

	return prev_ctx;
}

static inline void exception_exit(enum ctx_state prev_ctx)
{
	if (context_tracking_is_enabled()) {
<<<<<<< HEAD
		if (prev_ctx == IN_USER)
			context_tracking_user_enter();
	}
}

static inline void context_tracking_task_switch(struct task_struct *prev,
						struct task_struct *next)
{
	if (context_tracking_is_enabled())
		__context_tracking_task_switch(prev, next);
=======
		if (prev_ctx != CONTEXT_KERNEL)
			context_tracking_enter(prev_ctx);
	}
}


/**
 * ct_state() - return the current context tracking state if known
 *
 * Returns the current cpu's context tracking state if context tracking
 * is enabled.  If context tracking is disabled, returns
 * CONTEXT_DISABLED.  This should be used primarily for debugging.
 */
static inline enum ctx_state ct_state(void)
{
	return context_tracking_is_enabled() ?
		this_cpu_read(context_tracking.state) : CONTEXT_DISABLED;
>>>>>>> v4.9.227
}
#else
static inline void user_enter(void) { }
static inline void user_exit(void) { }
<<<<<<< HEAD
static inline enum ctx_state exception_enter(void) { return 0; }
static inline void exception_exit(enum ctx_state prev_ctx) { }
static inline void context_tracking_task_switch(struct task_struct *prev,
						struct task_struct *next) { }
#endif /* !CONFIG_CONTEXT_TRACKING */

=======
static inline void user_enter_irqoff(void) { }
static inline void user_exit_irqoff(void) { }
static inline enum ctx_state exception_enter(void) { return 0; }
static inline void exception_exit(enum ctx_state prev_ctx) { }
static inline enum ctx_state ct_state(void) { return CONTEXT_DISABLED; }
#endif /* !CONFIG_CONTEXT_TRACKING */

#define CT_WARN_ON(cond) WARN_ON(context_tracking_is_enabled() && (cond))
>>>>>>> v4.9.227

#ifdef CONFIG_CONTEXT_TRACKING_FORCE
extern void context_tracking_init(void);
#else
static inline void context_tracking_init(void) { }
#endif /* CONFIG_CONTEXT_TRACKING_FORCE */


#ifdef CONFIG_VIRT_CPU_ACCOUNTING_GEN
<<<<<<< HEAD
static inline void guest_enter(void)
{
	if (vtime_accounting_enabled())
		vtime_guest_enter(current);
	else
		current->flags |= PF_VCPU;
}

static inline void guest_exit(void)
{
	if (vtime_accounting_enabled())
=======
/* must be called with irqs disabled */
static inline void guest_enter_irqoff(void)
{
	if (vtime_accounting_cpu_enabled())
		vtime_guest_enter(current);
	else
		current->flags |= PF_VCPU;

	if (context_tracking_is_enabled())
		__context_tracking_enter(CONTEXT_GUEST);

	/* KVM does not hold any references to rcu protected data when it
	 * switches CPU into a guest mode. In fact switching to a guest mode
	 * is very similar to exiting to userspace from rcu point of view. In
	 * addition CPU may stay in a guest mode for quite a long time (up to
	 * one time slice). Lets treat guest mode as quiescent state, just like
	 * we do with user-mode execution.
	 */
	if (!context_tracking_cpu_is_enabled())
		rcu_virt_note_context_switch(smp_processor_id());
}

static inline void guest_exit_irqoff(void)
{
	if (context_tracking_is_enabled())
		__context_tracking_exit(CONTEXT_GUEST);

	if (vtime_accounting_cpu_enabled())
>>>>>>> v4.9.227
		vtime_guest_exit(current);
	else
		current->flags &= ~PF_VCPU;
}

#else
<<<<<<< HEAD
static inline void guest_enter(void)
=======
static inline void guest_enter_irqoff(void)
>>>>>>> v4.9.227
{
	/*
	 * This is running in ioctl context so its safe
	 * to assume that it's the stime pending cputime
	 * to flush.
	 */
	vtime_account_system(current);
	current->flags |= PF_VCPU;
<<<<<<< HEAD
}

static inline void guest_exit(void)
=======
	rcu_virt_note_context_switch(smp_processor_id());
}

static inline void guest_exit_irqoff(void)
>>>>>>> v4.9.227
{
	/* Flush the guest cputime we spent on the guest */
	vtime_account_system(current);
	current->flags &= ~PF_VCPU;
}
#endif /* CONFIG_VIRT_CPU_ACCOUNTING_GEN */

<<<<<<< HEAD
=======
static inline void guest_enter(void)
{
	unsigned long flags;

	local_irq_save(flags);
	guest_enter_irqoff();
	local_irq_restore(flags);
}

static inline void guest_exit(void)
{
	unsigned long flags;

	local_irq_save(flags);
	guest_exit_irqoff();
	local_irq_restore(flags);
}

>>>>>>> v4.9.227
#endif
