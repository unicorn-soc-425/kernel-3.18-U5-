/*
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 2000, 2001, 2002 Andi Kleen, SuSE Labs
 *
 *  Pentium III FXSR, SSE support
 *	Gareth Hughes <gareth@valinux.com>, May 2000
 */

/*
 * Handle hardware traps and faults.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/context_tracking.h>
#include <linux/interrupt.h>
#include <linux/kallsyms.h>
#include <linux/spinlock.h>
#include <linux/kprobes.h>
#include <linux/uaccess.h>
#include <linux/kdebug.h>
#include <linux/kgdb.h>
#include <linux/kernel.h>
<<<<<<< HEAD
#include <linux/module.h>
=======
#include <linux/export.h>
>>>>>>> v4.9.227
#include <linux/ptrace.h>
#include <linux/uprobes.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/kexec.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/bug.h>
#include <linux/nmi.h>
#include <linux/mm.h>
#include <linux/smp.h>
#include <linux/io.h>

#ifdef CONFIG_EISA
#include <linux/ioport.h>
#include <linux/eisa.h>
#endif

#if defined(CONFIG_EDAC)
#include <linux/edac.h>
#endif

#include <asm/kmemcheck.h>
#include <asm/stacktrace.h>
#include <asm/processor.h>
#include <asm/debugreg.h>
#include <linux/atomic.h>
<<<<<<< HEAD
#include <asm/ftrace.h>
#include <asm/traps.h>
#include <asm/desc.h>
#include <asm/i387.h>
#include <asm/fpu-internal.h>
=======
#include <asm/text-patching.h>
#include <asm/ftrace.h>
#include <asm/traps.h>
#include <asm/desc.h>
#include <asm/fpu/internal.h>
>>>>>>> v4.9.227
#include <asm/mce.h>
#include <asm/fixmap.h>
#include <asm/mach_traps.h>
#include <asm/alternative.h>
<<<<<<< HEAD
=======
#include <asm/fpu/xstate.h>
#include <asm/trace/mpx.h>
#include <asm/mpx.h>
#include <asm/vm86.h>
>>>>>>> v4.9.227

#ifdef CONFIG_X86_64
#include <asm/x86_init.h>
#include <asm/pgalloc.h>
#include <asm/proto.h>

/* No need to be aligned, but done to keep all IDTs defined the same way. */
gate_desc debug_idt_table[NR_VECTORS] __page_aligned_bss;
#else
#include <asm/processor-flags.h>
#include <asm/setup.h>
<<<<<<< HEAD

asmlinkage int system_call(void);
=======
#include <asm/proto.h>
>>>>>>> v4.9.227
#endif

/* Must be page-aligned because the real IDT is used in a fixmap. */
gate_desc idt_table[NR_VECTORS] __page_aligned_bss;

DECLARE_BITMAP(used_vectors, NR_VECTORS);
EXPORT_SYMBOL_GPL(used_vectors);

<<<<<<< HEAD
static inline void conditional_sti(struct pt_regs *regs)
=======
static inline void cond_local_irq_enable(struct pt_regs *regs)
>>>>>>> v4.9.227
{
	if (regs->flags & X86_EFLAGS_IF)
		local_irq_enable();
}

<<<<<<< HEAD
static inline void preempt_conditional_sti(struct pt_regs *regs)
{
	preempt_count_inc();
	if (regs->flags & X86_EFLAGS_IF)
		local_irq_enable();
}

static inline void conditional_cli(struct pt_regs *regs)
=======
static inline void cond_local_irq_disable(struct pt_regs *regs)
>>>>>>> v4.9.227
{
	if (regs->flags & X86_EFLAGS_IF)
		local_irq_disable();
}

<<<<<<< HEAD
static inline void preempt_conditional_cli(struct pt_regs *regs)
{
	if (regs->flags & X86_EFLAGS_IF)
		local_irq_disable();
	preempt_count_dec();
=======
/*
 * In IST context, we explicitly disable preemption.  This serves two
 * purposes: it makes it much less likely that we would accidentally
 * schedule in IST context and it will force a warning if we somehow
 * manage to schedule by accident.
 */
void ist_enter(struct pt_regs *regs)
{
	if (user_mode(regs)) {
		RCU_LOCKDEP_WARN(!rcu_is_watching(), "entry code didn't wake RCU");
	} else {
		/*
		 * We might have interrupted pretty much anything.  In
		 * fact, if we're a machine check, we can even interrupt
		 * NMI processing.  We don't want in_nmi() to return true,
		 * but we need to notify RCU.
		 */
		rcu_nmi_enter();
	}

	preempt_disable();

	/* This code is a bit fragile.  Test it. */
	RCU_LOCKDEP_WARN(!rcu_is_watching(), "ist_enter didn't work");
}

void ist_exit(struct pt_regs *regs)
{
	preempt_enable_no_resched();

	if (!user_mode(regs))
		rcu_nmi_exit();
}

/**
 * ist_begin_non_atomic() - begin a non-atomic section in an IST exception
 * @regs:	regs passed to the IST exception handler
 *
 * IST exception handlers normally cannot schedule.  As a special
 * exception, if the exception interrupted userspace code (i.e.
 * user_mode(regs) would return true) and the exception was not
 * a double fault, it can be safe to schedule.  ist_begin_non_atomic()
 * begins a non-atomic section within an ist_enter()/ist_exit() region.
 * Callers are responsible for enabling interrupts themselves inside
 * the non-atomic section, and callers must call ist_end_non_atomic()
 * before ist_exit().
 */
void ist_begin_non_atomic(struct pt_regs *regs)
{
	BUG_ON(!user_mode(regs));

	/*
	 * Sanity check: we need to be on the normal thread stack.  This
	 * will catch asm bugs and any attempt to use ist_preempt_enable
	 * from double_fault.
	 */
	BUG_ON((unsigned long)(current_top_of_stack() -
			       current_stack_pointer) >= THREAD_SIZE);

	preempt_enable_no_resched();
}

/**
 * ist_end_non_atomic() - begin a non-atomic section in an IST exception
 *
 * Ends a non-atomic section started with ist_begin_non_atomic().
 */
void ist_end_non_atomic(void)
{
	preempt_disable();
>>>>>>> v4.9.227
}

static nokprobe_inline int
do_trap_no_signal(struct task_struct *tsk, int trapnr, char *str,
		  struct pt_regs *regs,	long error_code)
{
<<<<<<< HEAD
#ifdef CONFIG_X86_32
	if (regs->flags & X86_VM_MASK) {
=======
	if (v8086_mode(regs)) {
>>>>>>> v4.9.227
		/*
		 * Traps 0, 1, 3, 4, and 5 should be forwarded to vm86.
		 * On nmi (interrupt 2), do_trap should not be called.
		 */
		if (trapnr < X86_TRAP_UD) {
			if (!handle_vm86_trap((struct kernel_vm86_regs *) regs,
						error_code, trapnr))
				return 0;
		}
		return -1;
	}
<<<<<<< HEAD
#endif
	if (!user_mode(regs)) {
		if (!fixup_exception(regs)) {
=======

	if (!user_mode(regs)) {
		if (!fixup_exception(regs, trapnr)) {
>>>>>>> v4.9.227
			tsk->thread.error_code = error_code;
			tsk->thread.trap_nr = trapnr;
			die(str, regs, error_code);
		}
		return 0;
	}

	return -1;
}

static siginfo_t *fill_trap_info(struct pt_regs *regs, int signr, int trapnr,
				siginfo_t *info)
{
	unsigned long siaddr;
	int sicode;

	switch (trapnr) {
	default:
		return SEND_SIG_PRIV;

	case X86_TRAP_DE:
		sicode = FPE_INTDIV;
		siaddr = uprobe_get_trap_addr(regs);
		break;
	case X86_TRAP_UD:
		sicode = ILL_ILLOPN;
		siaddr = uprobe_get_trap_addr(regs);
		break;
	case X86_TRAP_AC:
		sicode = BUS_ADRALN;
		siaddr = 0;
		break;
	}

	info->si_signo = signr;
	info->si_errno = 0;
	info->si_code = sicode;
	info->si_addr = (void __user *)siaddr;
	return info;
}

static void
do_trap(int trapnr, int signr, char *str, struct pt_regs *regs,
	long error_code, siginfo_t *info)
{
	struct task_struct *tsk = current;


	if (!do_trap_no_signal(tsk, trapnr, str, regs, error_code))
		return;
	/*
	 * We want error_code and trap_nr set for userspace faults and
	 * kernelspace faults which result in die(), but not
	 * kernelspace faults which are fixed up.  die() gives the
	 * process no chance to handle the signal and notice the
	 * kernel fault information, so that won't result in polluting
	 * the information about previously queued, but not yet
	 * delivered, faults.  See also do_general_protection below.
	 */
	tsk->thread.error_code = error_code;
	tsk->thread.trap_nr = trapnr;

<<<<<<< HEAD
#ifdef CONFIG_X86_64
=======
>>>>>>> v4.9.227
	if (show_unhandled_signals && unhandled_signal(tsk, signr) &&
	    printk_ratelimit()) {
		pr_info("%s[%d] trap %s ip:%lx sp:%lx error:%lx",
			tsk->comm, tsk->pid, str,
			regs->ip, regs->sp, error_code);
		print_vma_addr(" in ", regs->ip);
		pr_cont("\n");
	}
<<<<<<< HEAD
#endif
=======
>>>>>>> v4.9.227

	force_sig_info(signr, info ?: SEND_SIG_PRIV, tsk);
}
NOKPROBE_SYMBOL(do_trap);

static void do_error_trap(struct pt_regs *regs, long error_code, char *str,
			  unsigned long trapnr, int signr)
{
<<<<<<< HEAD
	enum ctx_state prev_state = exception_enter();
	siginfo_t info;

	if (notify_die(DIE_TRAP, str, regs, error_code, trapnr, signr) !=
			NOTIFY_STOP) {
		conditional_sti(regs);
		do_trap(trapnr, signr, str, regs, error_code,
			fill_trap_info(regs, signr, trapnr, &info));
	}

	exception_exit(prev_state);
=======
	siginfo_t info;

	RCU_LOCKDEP_WARN(!rcu_is_watching(), "entry code didn't wake RCU");

	if (notify_die(DIE_TRAP, str, regs, error_code, trapnr, signr) !=
			NOTIFY_STOP) {
		cond_local_irq_enable(regs);
		do_trap(trapnr, signr, str, regs, error_code,
			fill_trap_info(regs, signr, trapnr, &info));
	}
>>>>>>> v4.9.227
}

#define DO_ERROR(trapnr, signr, str, name)				\
dotraplinkage void do_##name(struct pt_regs *regs, long error_code)	\
{									\
	do_error_trap(regs, error_code, str, trapnr, signr);		\
}

DO_ERROR(X86_TRAP_DE,     SIGFPE,  "divide error",		divide_error)
DO_ERROR(X86_TRAP_OF,     SIGSEGV, "overflow",			overflow)
<<<<<<< HEAD
DO_ERROR(X86_TRAP_BR,     SIGSEGV, "bounds",			bounds)
=======
>>>>>>> v4.9.227
DO_ERROR(X86_TRAP_UD,     SIGILL,  "invalid opcode",		invalid_op)
DO_ERROR(X86_TRAP_OLD_MF, SIGFPE,  "coprocessor segment overrun",coprocessor_segment_overrun)
DO_ERROR(X86_TRAP_TS,     SIGSEGV, "invalid TSS",		invalid_TSS)
DO_ERROR(X86_TRAP_NP,     SIGBUS,  "segment not present",	segment_not_present)
DO_ERROR(X86_TRAP_SS,     SIGBUS,  "stack segment",		stack_segment)
DO_ERROR(X86_TRAP_AC,     SIGBUS,  "alignment check",		alignment_check)

<<<<<<< HEAD
=======
#ifdef CONFIG_VMAP_STACK
__visible void __noreturn handle_stack_overflow(const char *message,
						struct pt_regs *regs,
						unsigned long fault_address)
{
	printk(KERN_EMERG "BUG: stack guard page was hit at %p (stack is %p..%p)\n",
		 (void *)fault_address, current->stack,
		 (char *)current->stack + THREAD_SIZE - 1);
	die(message, regs, 0);

	/* Be absolutely certain we don't return. */
	panic(message);
}
#endif

>>>>>>> v4.9.227
#ifdef CONFIG_X86_64
/* Runs on IST stack */
dotraplinkage void do_double_fault(struct pt_regs *regs, long error_code)
{
	static const char str[] = "double fault";
	struct task_struct *tsk = current;
<<<<<<< HEAD
=======
#ifdef CONFIG_VMAP_STACK
	unsigned long cr2;
#endif
>>>>>>> v4.9.227

#ifdef CONFIG_X86_ESPFIX64
	extern unsigned char native_irq_return_iret[];

	/*
	 * If IRET takes a non-IST fault on the espfix64 stack, then we
	 * end up promoting it to a doublefault.  In that case, modify
	 * the stack to make it look like we just entered the #GP
	 * handler from user space, similar to bad_iret.
<<<<<<< HEAD
=======
	 *
	 * No need for ist_enter here because we don't use RCU.
>>>>>>> v4.9.227
	 */
	if (((long)regs->sp >> PGDIR_SHIFT) == ESPFIX_PGD_ENTRY &&
		regs->cs == __KERNEL_CS &&
		regs->ip == (unsigned long)native_irq_return_iret)
	{
		struct pt_regs *normal_regs = task_pt_regs(current);

		/* Fake a #GP(0) from userspace. */
		memmove(&normal_regs->ip, (void *)regs->sp, 5*8);
		normal_regs->orig_ax = 0;  /* Missing (lost) #GP error code */
		regs->ip = (unsigned long)general_protection;
		regs->sp = (unsigned long)&normal_regs->orig_ax;
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
		return;
	}
#endif

<<<<<<< HEAD
	exception_enter();
	/* Return not checked because double check cannot be ignored */
=======
	ist_enter(regs);
>>>>>>> v4.9.227
	notify_die(DIE_TRAP, str, regs, error_code, X86_TRAP_DF, SIGSEGV);

	tsk->thread.error_code = error_code;
	tsk->thread.trap_nr = X86_TRAP_DF;

<<<<<<< HEAD
=======
#ifdef CONFIG_VMAP_STACK
	/*
	 * If we overflow the stack into a guard page, the CPU will fail
	 * to deliver #PF and will send #DF instead.  Similarly, if we
	 * take any non-IST exception while too close to the bottom of
	 * the stack, the processor will get a page fault while
	 * delivering the exception and will generate a double fault.
	 *
	 * According to the SDM (footnote in 6.15 under "Interrupt 14 -
	 * Page-Fault Exception (#PF):
	 *
	 *   Processors update CR2 whenever a page fault is detected. If a
	 *   second page fault occurs while an earlier page fault is being
	 *   deliv- ered, the faulting linear address of the second fault will
	 *   overwrite the contents of CR2 (replacing the previous
	 *   address). These updates to CR2 occur even if the page fault
	 *   results in a double fault or occurs during the delivery of a
	 *   double fault.
	 *
	 * The logic below has a small possibility of incorrectly diagnosing
	 * some errors as stack overflows.  For example, if the IDT or GDT
	 * gets corrupted such that #GP delivery fails due to a bad descriptor
	 * causing #GP and we hit this condition while CR2 coincidentally
	 * points to the stack guard page, we'll think we overflowed the
	 * stack.  Given that we're going to panic one way or another
	 * if this happens, this isn't necessarily worth fixing.
	 *
	 * If necessary, we could improve the test by only diagnosing
	 * a stack overflow if the saved RSP points within 47 bytes of
	 * the bottom of the stack: if RSP == tsk_stack + 48 and we
	 * take an exception, the stack is already aligned and there
	 * will be enough room SS, RSP, RFLAGS, CS, RIP, and a
	 * possible error code, so a stack overflow would *not* double
	 * fault.  With any less space left, exception delivery could
	 * fail, and, as a practical matter, we've overflowed the
	 * stack even if the actual trigger for the double fault was
	 * something else.
	 */
	cr2 = read_cr2();
	if ((unsigned long)task_stack_page(tsk) - 1 - cr2 < PAGE_SIZE)
		handle_stack_overflow("kernel stack overflow (double-fault)", regs, cr2);
#endif

>>>>>>> v4.9.227
#ifdef CONFIG_DOUBLEFAULT
	df_debug(regs, error_code);
#endif
	/*
	 * This is always a kernel trap and never fixable (and thus must
	 * never return).
	 */
	for (;;)
		die(str, regs, error_code);
}
#endif

<<<<<<< HEAD
=======
dotraplinkage void do_bounds(struct pt_regs *regs, long error_code)
{
	const struct mpx_bndcsr *bndcsr;
	siginfo_t *info;

	RCU_LOCKDEP_WARN(!rcu_is_watching(), "entry code didn't wake RCU");
	if (notify_die(DIE_TRAP, "bounds", regs, error_code,
			X86_TRAP_BR, SIGSEGV) == NOTIFY_STOP)
		return;
	cond_local_irq_enable(regs);

	if (!user_mode(regs))
		die("bounds", regs, error_code);

	if (!cpu_feature_enabled(X86_FEATURE_MPX)) {
		/* The exception is not from Intel MPX */
		goto exit_trap;
	}

	/*
	 * We need to look at BNDSTATUS to resolve this exception.
	 * A NULL here might mean that it is in its 'init state',
	 * which is all zeros which indicates MPX was not
	 * responsible for the exception.
	 */
	bndcsr = get_xsave_field_ptr(XFEATURE_MASK_BNDCSR);
	if (!bndcsr)
		goto exit_trap;

	trace_bounds_exception_mpx(bndcsr);
	/*
	 * The error code field of the BNDSTATUS register communicates status
	 * information of a bound range exception #BR or operation involving
	 * bound directory.
	 */
	switch (bndcsr->bndstatus & MPX_BNDSTA_ERROR_CODE) {
	case 2:	/* Bound directory has invalid entry. */
		if (mpx_handle_bd_fault())
			goto exit_trap;
		break; /* Success, it was handled */
	case 1: /* Bound violation. */
		info = mpx_generate_siginfo(regs);
		if (IS_ERR(info)) {
			/*
			 * We failed to decode the MPX instruction.  Act as if
			 * the exception was not caused by MPX.
			 */
			goto exit_trap;
		}
		/*
		 * Success, we decoded the instruction and retrieved
		 * an 'info' containing the address being accessed
		 * which caused the exception.  This information
		 * allows and application to possibly handle the
		 * #BR exception itself.
		 */
		do_trap(X86_TRAP_BR, SIGSEGV, "bounds", regs, error_code, info);
		kfree(info);
		break;
	case 0: /* No exception caused by Intel MPX operations. */
		goto exit_trap;
	default:
		die("bounds", regs, error_code);
	}

	return;

exit_trap:
	/*
	 * This path out is for all the cases where we could not
	 * handle the exception in some way (like allocating a
	 * table or telling userspace about it.  We will also end
	 * up here if the kernel has MPX turned off at compile
	 * time..
	 */
	do_trap(X86_TRAP_BR, SIGSEGV, "bounds", regs, error_code, NULL);
}

>>>>>>> v4.9.227
dotraplinkage void
do_general_protection(struct pt_regs *regs, long error_code)
{
	struct task_struct *tsk;
<<<<<<< HEAD
	enum ctx_state prev_state;

	prev_state = exception_enter();
	conditional_sti(regs);

#ifdef CONFIG_X86_32
	if (regs->flags & X86_VM_MASK) {
		local_irq_enable();
		handle_vm86_fault((struct kernel_vm86_regs *) regs, error_code);
		goto exit;
	}
#endif

	tsk = current;
	if (!user_mode(regs)) {
		if (fixup_exception(regs))
			goto exit;
=======

	RCU_LOCKDEP_WARN(!rcu_is_watching(), "entry code didn't wake RCU");
	cond_local_irq_enable(regs);

	if (v8086_mode(regs)) {
		local_irq_enable();
		handle_vm86_fault((struct kernel_vm86_regs *) regs, error_code);
		return;
	}

	tsk = current;
	if (!user_mode(regs)) {
		if (fixup_exception(regs, X86_TRAP_GP))
			return;
>>>>>>> v4.9.227

		tsk->thread.error_code = error_code;
		tsk->thread.trap_nr = X86_TRAP_GP;
		if (notify_die(DIE_GPF, "general protection fault", regs, error_code,
			       X86_TRAP_GP, SIGSEGV) != NOTIFY_STOP)
			die("general protection fault", regs, error_code);
<<<<<<< HEAD
		goto exit;
=======
		return;
>>>>>>> v4.9.227
	}

	tsk->thread.error_code = error_code;
	tsk->thread.trap_nr = X86_TRAP_GP;

	if (show_unhandled_signals && unhandled_signal(tsk, SIGSEGV) &&
			printk_ratelimit()) {
		pr_info("%s[%d] general protection ip:%lx sp:%lx error:%lx",
			tsk->comm, task_pid_nr(tsk),
			regs->ip, regs->sp, error_code);
		print_vma_addr(" in ", regs->ip);
		pr_cont("\n");
	}

	force_sig_info(SIGSEGV, SEND_SIG_PRIV, tsk);
<<<<<<< HEAD
exit:
	exception_exit(prev_state);
}
NOKPROBE_SYMBOL(do_general_protection);

/* May run on IST stack. */
dotraplinkage void notrace do_int3(struct pt_regs *regs, long error_code)
{
	enum ctx_state prev_state;

=======
}
NOKPROBE_SYMBOL(do_general_protection);

dotraplinkage void notrace do_int3(struct pt_regs *regs, long error_code)
{
>>>>>>> v4.9.227
#ifdef CONFIG_DYNAMIC_FTRACE
	/*
	 * ftrace must be first, everything else may cause a recursive crash.
	 * See note by declaration of modifying_ftrace_code in ftrace.c
	 */
	if (unlikely(atomic_read(&modifying_ftrace_code)) &&
	    ftrace_int3_handler(regs))
		return;
#endif
	if (poke_int3_handler(regs))
		return;

<<<<<<< HEAD
	prev_state = exception_enter();
=======
	/*
	 * Use ist_enter despite the fact that we don't use an IST stack.
	 * We can be called from a kprobe in non-CONTEXT_KERNEL kernel
	 * mode or even during context tracking state changes.
	 *
	 * This means that we can't schedule.  That's okay.
	 */
	ist_enter(regs);

	RCU_LOCKDEP_WARN(!rcu_is_watching(), "entry code didn't wake RCU");
>>>>>>> v4.9.227
#ifdef CONFIG_KGDB_LOW_LEVEL_TRAP
	if (kgdb_ll_trap(DIE_INT3, "int3", regs, error_code, X86_TRAP_BP,
				SIGTRAP) == NOTIFY_STOP)
		goto exit;
#endif /* CONFIG_KGDB_LOW_LEVEL_TRAP */

#ifdef CONFIG_KPROBES
	if (kprobe_int3_handler(regs))
		goto exit;
#endif

	if (notify_die(DIE_INT3, "int3", regs, error_code, X86_TRAP_BP,
			SIGTRAP) == NOTIFY_STOP)
		goto exit;

<<<<<<< HEAD
	/*
	 * Let others (NMI) know that the debug stack is in use
	 * as we may switch to the interrupt stack.
	 */
	debug_stack_usage_inc();
	preempt_conditional_sti(regs);
	do_trap(X86_TRAP_BP, SIGTRAP, "int3", regs, error_code, NULL);
	preempt_conditional_cli(regs);
	debug_stack_usage_dec();
exit:
	exception_exit(prev_state);
=======
	preempt_disable();
	cond_local_irq_enable(regs);
	do_trap(X86_TRAP_BP, SIGTRAP, "int3", regs, error_code, NULL);
	cond_local_irq_disable(regs);
	preempt_enable_no_resched();
exit:
	ist_exit(regs);
>>>>>>> v4.9.227
}
NOKPROBE_SYMBOL(do_int3);

#ifdef CONFIG_X86_64
/*
<<<<<<< HEAD
 * Help handler running on IST stack to switch back to user stack
 * for scheduling or signal handling. The actual stack switch is done in
 * entry.S
 */
asmlinkage __visible notrace struct pt_regs *sync_regs(struct pt_regs *eregs)
{
	struct pt_regs *regs = eregs;
	/* Did already sync */
	if (eregs == (struct pt_regs *)eregs->sp)
		;
	/* Exception from user space */
	else if (user_mode(eregs))
		regs = task_pt_regs(current);
	/*
	 * Exception from kernel and interrupts are enabled. Move to
	 * kernel process stack.
	 */
	else if (eregs->flags & X86_EFLAGS_IF)
		regs = (struct pt_regs *)(eregs->sp -= sizeof(struct pt_regs));
	if (eregs != regs)
		*regs = *eregs;
=======
 * Help handler running on IST stack to switch off the IST stack if the
 * interrupted code was in user mode. The actual stack switch is done in
 * entry_64.S
 */
asmlinkage __visible notrace struct pt_regs *sync_regs(struct pt_regs *eregs)
{
	struct pt_regs *regs = task_pt_regs(current);
	*regs = *eregs;
>>>>>>> v4.9.227
	return regs;
}
NOKPROBE_SYMBOL(sync_regs);

struct bad_iret_stack {
	void *error_entry_ret;
	struct pt_regs regs;
};

asmlinkage __visible notrace
struct bad_iret_stack *fixup_bad_iret(struct bad_iret_stack *s)
{
	/*
	 * This is called from entry_64.S early in handling a fault
	 * caused by a bad iret to user mode.  To handle the fault
	 * correctly, we want move our stack frame to task_pt_regs
	 * and we want to pretend that the exception came from the
	 * iret target.
	 */
	struct bad_iret_stack *new_stack =
		container_of(task_pt_regs(current),
			     struct bad_iret_stack, regs);

	/* Copy the IRET target to the new stack. */
	memmove(&new_stack->regs.ip, (void *)s->regs.sp, 5*8);

	/* Copy the remainder of the stack from the current stack. */
	memmove(new_stack, s, offsetof(struct bad_iret_stack, regs.ip));

<<<<<<< HEAD
	BUG_ON(!user_mode_vm(&new_stack->regs));
=======
	BUG_ON(!user_mode(&new_stack->regs));
>>>>>>> v4.9.227
	return new_stack;
}
NOKPROBE_SYMBOL(fixup_bad_iret);
#endif

<<<<<<< HEAD
=======
static bool is_sysenter_singlestep(struct pt_regs *regs)
{
	/*
	 * We don't try for precision here.  If we're anywhere in the region of
	 * code that can be single-stepped in the SYSENTER entry path, then
	 * assume that this is a useless single-step trap due to SYSENTER
	 * being invoked with TF set.  (We don't know in advance exactly
	 * which instructions will be hit because BTF could plausibly
	 * be set.)
	 */
#ifdef CONFIG_X86_32
	return (regs->ip - (unsigned long)__begin_SYSENTER_singlestep_region) <
		(unsigned long)__end_SYSENTER_singlestep_region -
		(unsigned long)__begin_SYSENTER_singlestep_region;
#elif defined(CONFIG_IA32_EMULATION)
	return (regs->ip - (unsigned long)entry_SYSENTER_compat) <
		(unsigned long)__end_entry_SYSENTER_compat -
		(unsigned long)entry_SYSENTER_compat;
#else
	return false;
#endif
}

>>>>>>> v4.9.227
/*
 * Our handling of the processor debug registers is non-trivial.
 * We do not clear them on entry and exit from the kernel. Therefore
 * it is possible to get a watchpoint trap here from inside the kernel.
 * However, the code in ./ptrace.c has ensured that the user can
 * only set watchpoints on userspace addresses. Therefore the in-kernel
 * watchpoint trap can only occur in code which is reading/writing
 * from user space. Such code must not hold kernel locks (since it
 * can equally take a page fault), therefore it is safe to call
 * force_sig_info even though that claims and releases locks.
 *
 * Code in ./signal.c ensures that the debug control register
 * is restored before we deliver any signal, and therefore that
 * user code runs with the correct debug control register even though
 * we clear it here.
 *
 * Being careful here means that we don't have to be as careful in a
 * lot of more complicated places (task switching can be a bit lazy
 * about restoring all the debug state, and ptrace doesn't have to
 * find every occurrence of the TF bit that could be saved away even
 * by user code)
 *
 * May run on IST stack.
 */
dotraplinkage void do_debug(struct pt_regs *regs, long error_code)
{
	struct task_struct *tsk = current;
<<<<<<< HEAD
	enum ctx_state prev_state;
=======
>>>>>>> v4.9.227
	int user_icebp = 0;
	unsigned long dr6;
	int si_code;

<<<<<<< HEAD
	prev_state = exception_enter();

	get_debugreg(dr6, 6);
=======
	ist_enter(regs);

	get_debugreg(dr6, 6);
	/*
	 * The Intel SDM says:
	 *
	 *   Certain debug exceptions may clear bits 0-3. The remaining
	 *   contents of the DR6 register are never cleared by the
	 *   processor. To avoid confusion in identifying debug
	 *   exceptions, debug handlers should clear the register before
	 *   returning to the interrupted task.
	 *
	 * Keep it simple: clear DR6 immediately.
	 */
	set_debugreg(0, 6);
>>>>>>> v4.9.227

	/* Filter out all the reserved bits which are preset to 1 */
	dr6 &= ~DR6_RESERVED;

	/*
<<<<<<< HEAD
=======
	 * The SDM says "The processor clears the BTF flag when it
	 * generates a debug exception."  Clear TIF_BLOCKSTEP to keep
	 * TIF_BLOCKSTEP in sync with the hardware BTF flag.
	 */
	clear_tsk_thread_flag(tsk, TIF_BLOCKSTEP);

	if (unlikely(!user_mode(regs) && (dr6 & DR_STEP) &&
		     is_sysenter_singlestep(regs))) {
		dr6 &= ~DR_STEP;
		if (!dr6)
			goto exit;
		/*
		 * else we might have gotten a single-step trap and hit a
		 * watchpoint at the same time, in which case we should fall
		 * through and handle the watchpoint.
		 */
	}

	/*
>>>>>>> v4.9.227
	 * If dr6 has no reason to give us about the origin of this trap,
	 * then it's very likely the result of an icebp/int01 trap.
	 * User wants a sigtrap for that.
	 */
	if (!dr6 && user_mode(regs))
		user_icebp = 1;

<<<<<<< HEAD
	/* Catch kmemcheck conditions first of all! */
	if ((dr6 & DR_STEP) && kmemcheck_trap(regs))
		goto exit;

	/* DR6 may or may not be cleared by the CPU */
	set_debugreg(0, 6);

	/*
	 * The processor cleared BTF, so don't mark that we need it set.
	 */
	clear_tsk_thread_flag(tsk, TIF_BLOCKSTEP);

=======
	/* Catch kmemcheck conditions! */
	if ((dr6 & DR_STEP) && kmemcheck_trap(regs))
		goto exit;

>>>>>>> v4.9.227
	/* Store the virtualized DR6 value */
	tsk->thread.debugreg6 = dr6;

#ifdef CONFIG_KPROBES
	if (kprobe_debug_handler(regs))
		goto exit;
#endif

	if (notify_die(DIE_DEBUG, "debug", regs, (long)&dr6, error_code,
							SIGTRAP) == NOTIFY_STOP)
		goto exit;

	/*
	 * Let others (NMI) know that the debug stack is in use
	 * as we may switch to the interrupt stack.
	 */
	debug_stack_usage_inc();

	/* It's safe to allow irq's after DR6 has been saved */
<<<<<<< HEAD
	preempt_conditional_sti(regs);

	if (regs->flags & X86_VM_MASK) {
		handle_vm86_trap((struct kernel_vm86_regs *) regs, error_code,
					X86_TRAP_DB);
		preempt_conditional_cli(regs);
=======
	preempt_disable();
	cond_local_irq_enable(regs);

	if (v8086_mode(regs)) {
		handle_vm86_trap((struct kernel_vm86_regs *) regs, error_code,
					X86_TRAP_DB);
		cond_local_irq_disable(regs);
		preempt_enable_no_resched();
>>>>>>> v4.9.227
		debug_stack_usage_dec();
		goto exit;
	}

<<<<<<< HEAD
	/*
	 * Single-stepping through system calls: ignore any exceptions in
	 * kernel space, but re-enable TF when returning to user mode.
	 *
	 * We already checked v86 mode above, so we can check for kernel mode
	 * by just checking the CPL of CS.
	 */
	if ((dr6 & DR_STEP) && !user_mode(regs)) {
=======
	if (WARN_ON_ONCE((dr6 & DR_STEP) && !user_mode(regs))) {
		/*
		 * Historical junk that used to handle SYSENTER single-stepping.
		 * This should be unreachable now.  If we survive for a while
		 * without anyone hitting this warning, we'll turn this into
		 * an oops.
		 */
>>>>>>> v4.9.227
		tsk->thread.debugreg6 &= ~DR_STEP;
		set_tsk_thread_flag(tsk, TIF_SINGLESTEP);
		regs->flags &= ~X86_EFLAGS_TF;
	}
	si_code = get_si_code(tsk->thread.debugreg6);
	if (tsk->thread.debugreg6 & (DR_STEP | DR_TRAP_BITS) || user_icebp)
		send_sigtrap(tsk, regs, error_code, si_code);
<<<<<<< HEAD
	preempt_conditional_cli(regs);
	debug_stack_usage_dec();

exit:
	exception_exit(prev_state);
=======
	cond_local_irq_disable(regs);
	preempt_enable_no_resched();
	debug_stack_usage_dec();

exit:
#if defined(CONFIG_X86_32)
	/*
	 * This is the most likely code path that involves non-trivial use
	 * of the SYSENTER stack.  Check that we haven't overrun it.
	 */
	WARN(this_cpu_read(cpu_tss.SYSENTER_stack_canary) != STACK_END_MAGIC,
	     "Overran or corrupted SYSENTER stack\n");
#endif
	ist_exit(regs);
>>>>>>> v4.9.227
}
NOKPROBE_SYMBOL(do_debug);

/*
 * Note that we play around with the 'TS' bit in an attempt to get
 * the correct behaviour even in the presence of the asynchronous
 * IRQ13 behaviour
 */
static void math_error(struct pt_regs *regs, int error_code, int trapnr)
{
	struct task_struct *task = current;
<<<<<<< HEAD
	siginfo_t info;
	unsigned short err;
	char *str = (trapnr == X86_TRAP_MF) ? "fpu exception" :
						"simd exception";

	if (notify_die(DIE_TRAP, str, regs, error_code, trapnr, SIGFPE) == NOTIFY_STOP)
		return;
	conditional_sti(regs);

	if (!user_mode_vm(regs))
	{
		if (!fixup_exception(regs)) {
			task->thread.error_code = error_code;
			task->thread.trap_nr = trapnr;
			die(str, regs, error_code);
		}
=======
	struct fpu *fpu = &task->thread.fpu;
	siginfo_t info;
	char *str = (trapnr == X86_TRAP_MF) ? "fpu exception" :
						"simd exception";

	cond_local_irq_enable(regs);

	if (!user_mode(regs)) {
		if (fixup_exception(regs, trapnr))
			return;

		task->thread.error_code = error_code;
		task->thread.trap_nr = trapnr;

		if (notify_die(DIE_TRAP, str, regs, error_code,
					trapnr, SIGFPE) != NOTIFY_STOP)
			die(str, regs, error_code);
>>>>>>> v4.9.227
		return;
	}

	/*
	 * Save the info for the exception handler and clear the error.
	 */
<<<<<<< HEAD
	save_init_fpu(task);
	task->thread.trap_nr = trapnr;
	task->thread.error_code = error_code;
	info.si_signo = SIGFPE;
	info.si_errno = 0;
	info.si_addr = (void __user *)uprobe_get_trap_addr(regs);
	if (trapnr == X86_TRAP_MF) {
		unsigned short cwd, swd;
		/*
		 * (~cwd & swd) will mask out exceptions that are not set to unmasked
		 * status.  0x3f is the exception bits in these regs, 0x200 is the
		 * C1 reg you need in case of a stack fault, 0x040 is the stack
		 * fault bit.  We should only be taking one exception at a time,
		 * so if this combination doesn't produce any single exception,
		 * then we have a bad program that isn't synchronizing its FPU usage
		 * and it will suffer the consequences since we won't be able to
		 * fully reproduce the context of the exception
		 */
		cwd = get_fpu_cwd(task);
		swd = get_fpu_swd(task);

		err = swd & ~cwd;
	} else {
		/*
		 * The SIMD FPU exceptions are handled a little differently, as there
		 * is only a single status/control register.  Thus, to determine which
		 * unmasked exception was caught we must mask the exception mask bits
		 * at 0x1f80, and then use these to mask the exception bits at 0x3f.
		 */
		unsigned short mxcsr = get_fpu_mxcsr(task);
		err = ~(mxcsr >> 7) & mxcsr;
	}

	if (err & 0x001) {	/* Invalid op */
		/*
		 * swd & 0x240 == 0x040: Stack Underflow
		 * swd & 0x240 == 0x240: Stack Overflow
		 * User must clear the SF bit (0x40) if set
		 */
		info.si_code = FPE_FLTINV;
	} else if (err & 0x004) { /* Divide by Zero */
		info.si_code = FPE_FLTDIV;
	} else if (err & 0x008) { /* Overflow */
		info.si_code = FPE_FLTOVF;
	} else if (err & 0x012) { /* Denormal, Underflow */
		info.si_code = FPE_FLTUND;
	} else if (err & 0x020) { /* Precision */
		info.si_code = FPE_FLTRES;
	} else {
		/*
		 * If we're using IRQ 13, or supposedly even some trap
		 * X86_TRAP_MF implementations, it's possible
		 * we get a spurious trap, which is not an error.
		 */
		return;
	}
=======
	fpu__save(fpu);

	task->thread.trap_nr	= trapnr;
	task->thread.error_code = error_code;
	info.si_signo		= SIGFPE;
	info.si_errno		= 0;
	info.si_addr		= (void __user *)uprobe_get_trap_addr(regs);

	info.si_code = fpu__exception_code(fpu, trapnr);

	/* Retry when we get spurious exceptions: */
	if (!info.si_code)
		return;

>>>>>>> v4.9.227
	force_sig_info(SIGFPE, &info, task);
}

dotraplinkage void do_coprocessor_error(struct pt_regs *regs, long error_code)
{
<<<<<<< HEAD
	enum ctx_state prev_state;

	prev_state = exception_enter();
	math_error(regs, error_code, X86_TRAP_MF);
	exception_exit(prev_state);
=======
	RCU_LOCKDEP_WARN(!rcu_is_watching(), "entry code didn't wake RCU");
	math_error(regs, error_code, X86_TRAP_MF);
>>>>>>> v4.9.227
}

dotraplinkage void
do_simd_coprocessor_error(struct pt_regs *regs, long error_code)
{
<<<<<<< HEAD
	enum ctx_state prev_state;

	prev_state = exception_enter();
	math_error(regs, error_code, X86_TRAP_XF);
	exception_exit(prev_state);
=======
	RCU_LOCKDEP_WARN(!rcu_is_watching(), "entry code didn't wake RCU");
	math_error(regs, error_code, X86_TRAP_XF);
>>>>>>> v4.9.227
}

dotraplinkage void
do_spurious_interrupt_bug(struct pt_regs *regs, long error_code)
{
<<<<<<< HEAD
	conditional_sti(regs);
#if 0
	/* No need to warn about this any longer. */
	pr_info("Ignoring P6 Local APIC Spurious Interrupt Bug...\n");
#endif
}

asmlinkage __visible void __attribute__((weak)) smp_thermal_interrupt(void)
{
}

asmlinkage __visible void __attribute__((weak)) smp_threshold_interrupt(void)
{
}

/*
 * 'math_state_restore()' saves the current math information in the
 * old math state array, and gets the new ones from the current task
 *
 * Careful.. There are problems with IBM-designed IRQ13 behaviour.
 * Don't touch unless you *really* know how it works.
 *
 * Must be called with kernel preemption disabled (eg with local
 * local interrupts as in the case of do_device_not_available).
 */
void math_state_restore(void)
{
	struct task_struct *tsk = current;

	if (!tsk_used_math(tsk)) {
		local_irq_enable();
		/*
		 * does a slab alloc which can sleep
		 */
		if (init_fpu(tsk)) {
			/*
			 * ran out of memory!
			 */
			do_group_exit(SIGKILL);
			return;
		}
		local_irq_disable();
	}

	__thread_fpu_begin(tsk);

	/*
	 * Paranoid restore. send a SIGSEGV if we fail to restore the state.
	 */
	if (unlikely(restore_fpu_checking(tsk))) {
		drop_init_fpu(tsk);
		force_sig_info(SIGSEGV, SEND_SIG_PRIV, tsk);
		return;
	}

	tsk->thread.fpu_counter++;
}
EXPORT_SYMBOL_GPL(math_state_restore);

dotraplinkage void
do_device_not_available(struct pt_regs *regs, long error_code)
{
	enum ctx_state prev_state;

	prev_state = exception_enter();
	BUG_ON(use_eager_fpu());

#ifdef CONFIG_MATH_EMULATION
	if (read_cr0() & X86_CR0_EM) {
		struct math_emu_info info = { };

		conditional_sti(regs);

		info.regs = regs;
		math_emulate(&info);
		exception_exit(prev_state);
		return;
	}
#endif
	math_state_restore(); /* interrupts still off */
#ifdef CONFIG_X86_32
	conditional_sti(regs);
#endif
	exception_exit(prev_state);
=======
	cond_local_irq_enable(regs);
}

dotraplinkage void
do_device_not_available(struct pt_regs *regs, long error_code)
{
	RCU_LOCKDEP_WARN(!rcu_is_watching(), "entry code didn't wake RCU");

#ifdef CONFIG_MATH_EMULATION
	if (!boot_cpu_has(X86_FEATURE_FPU) && (read_cr0() & X86_CR0_EM)) {
		struct math_emu_info info = { };

		cond_local_irq_enable(regs);

		info.regs = regs;
		math_emulate(&info);
		return;
	}
#endif
	fpu__restore(&current->thread.fpu); /* interrupts still off */
#ifdef CONFIG_X86_32
	cond_local_irq_enable(regs);
#endif
>>>>>>> v4.9.227
}
NOKPROBE_SYMBOL(do_device_not_available);

#ifdef CONFIG_X86_32
dotraplinkage void do_iret_error(struct pt_regs *regs, long error_code)
{
	siginfo_t info;
<<<<<<< HEAD
	enum ctx_state prev_state;

	prev_state = exception_enter();
=======

	RCU_LOCKDEP_WARN(!rcu_is_watching(), "entry code didn't wake RCU");
>>>>>>> v4.9.227
	local_irq_enable();

	info.si_signo = SIGILL;
	info.si_errno = 0;
	info.si_code = ILL_BADSTK;
	info.si_addr = NULL;
	if (notify_die(DIE_TRAP, "iret exception", regs, error_code,
			X86_TRAP_IRET, SIGILL) != NOTIFY_STOP) {
		do_trap(X86_TRAP_IRET, SIGILL, "iret exception", regs, error_code,
			&info);
	}
<<<<<<< HEAD
	exception_exit(prev_state);
=======
>>>>>>> v4.9.227
}
#endif

/* Set of traps needed for early debugging. */
void __init early_trap_init(void)
{
<<<<<<< HEAD
	set_intr_gate_ist(X86_TRAP_DB, &debug, DEBUG_STACK);
	/* int3 can be called from all */
	set_system_intr_gate_ist(X86_TRAP_BP, &int3, DEBUG_STACK);
=======
	/*
	 * Don't use IST to set DEBUG_STACK as it doesn't work until TSS
	 * is ready in cpu_init() <-- trap_init(). Before trap_init(),
	 * CPU runs at ring 0 so it is impossible to hit an invalid
	 * stack.  Using the original stack works well enough at this
	 * early stage. DEBUG_STACK will be equipped after cpu_init() in
	 * trap_init().
	 *
	 * We don't need to set trace_idt_table like set_intr_gate(),
	 * since we don't have trace_debug and it will be reset to
	 * 'debug' in trap_init() by set_intr_gate_ist().
	 */
	set_intr_gate_notrace(X86_TRAP_DB, debug);
	/* int3 can be called from all */
	set_system_intr_gate(X86_TRAP_BP, &int3);
>>>>>>> v4.9.227
#ifdef CONFIG_X86_32
	set_intr_gate(X86_TRAP_PF, page_fault);
#endif
	load_idt(&idt_descr);
}

void __init early_trap_pf_init(void)
{
#ifdef CONFIG_X86_64
	set_intr_gate(X86_TRAP_PF, page_fault);
#endif
}

void __init trap_init(void)
{
	int i;

#ifdef CONFIG_EISA
	void __iomem *p = early_ioremap(0x0FFFD9, 4);

	if (readl(p) == 'E' + ('I'<<8) + ('S'<<16) + ('A'<<24))
		EISA_bus = 1;
	early_iounmap(p, 4);
#endif

	set_intr_gate(X86_TRAP_DE, divide_error);
	set_intr_gate_ist(X86_TRAP_NMI, &nmi, NMI_STACK);
	/* int4 can be called from all */
	set_system_intr_gate(X86_TRAP_OF, &overflow);
	set_intr_gate(X86_TRAP_BR, bounds);
	set_intr_gate(X86_TRAP_UD, invalid_op);
	set_intr_gate(X86_TRAP_NM, device_not_available);
#ifdef CONFIG_X86_32
	set_task_gate(X86_TRAP_DF, GDT_ENTRY_DOUBLEFAULT_TSS);
#else
	set_intr_gate_ist(X86_TRAP_DF, &double_fault, DOUBLEFAULT_STACK);
#endif
	set_intr_gate(X86_TRAP_OLD_MF, coprocessor_segment_overrun);
	set_intr_gate(X86_TRAP_TS, invalid_TSS);
	set_intr_gate(X86_TRAP_NP, segment_not_present);
	set_intr_gate(X86_TRAP_SS, stack_segment);
	set_intr_gate(X86_TRAP_GP, general_protection);
	set_intr_gate(X86_TRAP_SPURIOUS, spurious_interrupt_bug);
	set_intr_gate(X86_TRAP_MF, coprocessor_error);
	set_intr_gate(X86_TRAP_AC, alignment_check);
#ifdef CONFIG_X86_MCE
	set_intr_gate_ist(X86_TRAP_MC, &machine_check, MCE_STACK);
#endif
	set_intr_gate(X86_TRAP_XF, simd_coprocessor_error);

	/* Reserve all the builtin and the syscall vector: */
	for (i = 0; i < FIRST_EXTERNAL_VECTOR; i++)
		set_bit(i, used_vectors);

#ifdef CONFIG_IA32_EMULATION
<<<<<<< HEAD
	set_system_intr_gate(IA32_SYSCALL_VECTOR, ia32_syscall);
=======
	set_system_intr_gate(IA32_SYSCALL_VECTOR, entry_INT80_compat);
>>>>>>> v4.9.227
	set_bit(IA32_SYSCALL_VECTOR, used_vectors);
#endif

#ifdef CONFIG_X86_32
<<<<<<< HEAD
	set_system_trap_gate(SYSCALL_VECTOR, &system_call);
	set_bit(SYSCALL_VECTOR, used_vectors);
=======
	set_system_intr_gate(IA32_SYSCALL_VECTOR, entry_INT80_32);
	set_bit(IA32_SYSCALL_VECTOR, used_vectors);
>>>>>>> v4.9.227
#endif

	/*
	 * Set the IDT descriptor to a fixed read-only location, so that the
	 * "sidt" instruction will not leak the location of the kernel, and
	 * to defend the IDT against arbitrary memory write vulnerabilities.
	 * It will be reloaded in cpu_init() */
	__set_fixmap(FIX_RO_IDT, __pa_symbol(idt_table), PAGE_KERNEL_RO);
	idt_descr.address = fix_to_virt(FIX_RO_IDT);

	/*
	 * Should be a barrier for any external CPU state:
	 */
	cpu_init();

<<<<<<< HEAD
=======
	/*
	 * X86_TRAP_DB was installed in early_trap_init(). However,
	 * IST works only after cpu_init() loads TSS. See comments
	 * in early_trap_init().
	 */
	set_intr_gate_ist(X86_TRAP_DB, &debug, DEBUG_STACK);

>>>>>>> v4.9.227
	x86_init.irqs.trap_init();

#ifdef CONFIG_X86_64
	memcpy(&debug_idt_table, &idt_table, IDT_ENTRIES * 16);
	set_nmi_gate(X86_TRAP_DB, &debug);
<<<<<<< HEAD
	set_nmi_gate(X86_TRAP_BP, &int3);
=======
>>>>>>> v4.9.227
#endif
}
