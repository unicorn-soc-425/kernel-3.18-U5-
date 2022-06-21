/*
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 2000, 2001, 2002 Andi Kleen, SuSE Labs
 */
#include <linux/kallsyms.h>
#include <linux/kprobes.h>
#include <linux/uaccess.h>
#include <linux/utsname.h>
#include <linux/hardirq.h>
#include <linux/kdebug.h>
#include <linux/module.h>
#include <linux/ptrace.h>
#include <linux/ftrace.h>
#include <linux/kexec.h>
#include <linux/bug.h>
#include <linux/nmi.h>
#include <linux/sysfs.h>
<<<<<<< HEAD

#include <asm/stacktrace.h>

=======
#include <linux/kasan.h>

#include <asm/stacktrace.h>
#include <asm/unwind.h>
>>>>>>> v4.9.227

int panic_on_unrecovered_nmi;
int panic_on_io_nmi;
unsigned int code_bytes = 64;
int kstack_depth_to_print = 3 * STACKSLOTS_PER_LINE;
static int die_counter;

<<<<<<< HEAD
static void printk_stack_address(unsigned long address, int reliable)
{
	pr_cont(" [<%p>] %s%pB\n",
		(void *)address, reliable ? "" : "? ", (void *)address);
=======
bool in_task_stack(unsigned long *stack, struct task_struct *task,
		   struct stack_info *info)
{
	unsigned long *begin = task_stack_page(task);
	unsigned long *end   = task_stack_page(task) + THREAD_SIZE;

	if (stack < begin || stack >= end)
		return false;

	info->type	= STACK_TYPE_TASK;
	info->begin	= begin;
	info->end	= end;
	info->next_sp	= NULL;

	return true;
}

static void printk_stack_address(unsigned long address, int reliable,
				 char *log_lvl)
{
	touch_nmi_watchdog();
	printk("%s [<%p>] %s%pB\n",
		log_lvl, (void *)address, reliable ? "" : "? ",
		(void *)address);
>>>>>>> v4.9.227
}

void printk_address(unsigned long address)
{
	pr_cont(" [<%p>] %pS\n", (void *)address, (void *)address);
}

<<<<<<< HEAD
#ifdef CONFIG_FUNCTION_GRAPH_TRACER
static void
print_ftrace_graph_addr(unsigned long addr, void *data,
			const struct stacktrace_ops *ops,
			struct thread_info *tinfo, int *graph)
{
	struct task_struct *task;
	unsigned long ret_addr;
	int index;

	if (addr != (unsigned long)return_to_handler)
		return;

	task = tinfo->task;
	index = task->curr_ret_stack;

	if (!task->ret_stack || index < *graph)
		return;

	index -= *graph;
	ret_addr = task->ret_stack[index].ret;

	ops->address(data, ret_addr, 1);

	(*graph)++;
}
#else
static inline void
print_ftrace_graph_addr(unsigned long addr, void *data,
			const struct stacktrace_ops *ops,
			struct thread_info *tinfo, int *graph)
{ }
#endif

/*
 * x86-64 can have up to three kernel stacks:
 * process stack
 * interrupt stack
 * severe exception (double fault, nmi, stack fault, debug, mce) hardware stack
 */

static inline int valid_stack_ptr(struct thread_info *tinfo,
			void *p, unsigned int size, void *end)
{
	void *t = tinfo;
	if (end) {
		if (p < end && p >= (end-THREAD_SIZE))
			return 1;
		else
			return 0;
	}
	return p > t && p < t + THREAD_SIZE - size;
}

unsigned long
print_context_stack(struct thread_info *tinfo,
		unsigned long *stack, unsigned long bp,
		const struct stacktrace_ops *ops, void *data,
		unsigned long *end, int *graph)
{
	struct stack_frame *frame = (struct stack_frame *)bp;

	while (valid_stack_ptr(tinfo, stack, sizeof(*stack), end)) {
		unsigned long addr;

		addr = *stack;
		if (__kernel_text_address(addr)) {
			if ((unsigned long) stack == bp + sizeof(long)) {
				ops->address(data, addr, 1);
				frame = frame->next_frame;
				bp = (unsigned long) frame;
			} else {
				ops->address(data, addr, 0);
			}
			print_ftrace_graph_addr(addr, data, ops, tinfo, graph);
		}
		stack++;
	}
	return bp;
}
EXPORT_SYMBOL_GPL(print_context_stack);

unsigned long
print_context_stack_bp(struct thread_info *tinfo,
		       unsigned long *stack, unsigned long bp,
		       const struct stacktrace_ops *ops, void *data,
		       unsigned long *end, int *graph)
{
	struct stack_frame *frame = (struct stack_frame *)bp;
	unsigned long *ret_addr = &frame->return_address;

	while (valid_stack_ptr(tinfo, ret_addr, sizeof(*ret_addr), end)) {
		unsigned long addr = *ret_addr;

		if (!__kernel_text_address(addr))
			break;

		ops->address(data, addr, 1);
		frame = frame->next_frame;
		ret_addr = &frame->return_address;
		print_ftrace_graph_addr(addr, data, ops, tinfo, graph);
	}

	return (unsigned long)frame;
}
EXPORT_SYMBOL_GPL(print_context_stack_bp);

static int print_trace_stack(void *data, char *name)
{
	printk("%s <%s> ", (char *)data, name);
	return 0;
}

/*
 * Print one address/symbol entries per line.
 */
static void print_trace_address(void *data, unsigned long addr, int reliable)
{
	touch_nmi_watchdog();
	printk(data);
	printk_stack_address(addr, reliable);
}

static const struct stacktrace_ops print_trace_ops = {
	.stack			= print_trace_stack,
	.address		= print_trace_address,
	.walk_stack		= print_context_stack,
};

void
show_trace_log_lvl(struct task_struct *task, struct pt_regs *regs,
		unsigned long *stack, unsigned long bp, char *log_lvl)
{
	printk("%sCall Trace:\n", log_lvl);
	dump_trace(task, regs, stack, bp, &print_trace_ops, log_lvl);
}

void show_trace(struct task_struct *task, struct pt_regs *regs,
		unsigned long *stack, unsigned long bp)
{
	show_trace_log_lvl(task, regs, stack, bp, "");
=======
void show_trace_log_lvl(struct task_struct *task, struct pt_regs *regs,
			unsigned long *stack, char *log_lvl)
{
	struct unwind_state state;
	struct stack_info stack_info = {0};
	unsigned long visit_mask = 0;
	int graph_idx = 0;

	printk("%sCall Trace:\n", log_lvl);

	unwind_start(&state, task, regs, stack);

	/*
	 * Iterate through the stacks, starting with the current stack pointer.
	 * Each stack has a pointer to the next one.
	 *
	 * x86-64 can have several stacks:
	 * - task stack
	 * - interrupt stack
	 * - HW exception stacks (double fault, nmi, debug, mce)
	 *
	 * x86-32 can have up to three stacks:
	 * - task stack
	 * - softirq stack
	 * - hardirq stack
	 */
	for (; stack; stack = stack_info.next_sp) {
		const char *str_begin, *str_end;

		/*
		 * If we overflowed the task stack into a guard page, jump back
		 * to the bottom of the usable stack.
		 */
		if (task_stack_page(task) - (void *)stack < PAGE_SIZE)
			stack = task_stack_page(task);

		if (get_stack_info(stack, task, &stack_info, &visit_mask))
			break;

		stack_type_str(stack_info.type, &str_begin, &str_end);
		if (str_begin)
			printk("%s <%s> ", log_lvl, str_begin);

		/*
		 * Scan the stack, printing any text addresses we find.  At the
		 * same time, follow proper stack frames with the unwinder.
		 *
		 * Addresses found during the scan which are not reported by
		 * the unwinder are considered to be additional clues which are
		 * sometimes useful for debugging and are prefixed with '?'.
		 * This also serves as a failsafe option in case the unwinder
		 * goes off in the weeds.
		 */
		for (; stack < stack_info.end; stack++) {
			unsigned long real_addr;
			int reliable = 0;
			unsigned long addr = READ_ONCE_NOCHECK(*stack);
			unsigned long *ret_addr_p =
				unwind_get_return_address_ptr(&state);

			if (!__kernel_text_address(addr))
				continue;

			if (stack == ret_addr_p)
				reliable = 1;

			/*
			 * When function graph tracing is enabled for a
			 * function, its return address on the stack is
			 * replaced with the address of an ftrace handler
			 * (return_to_handler).  In that case, before printing
			 * the "real" address, we want to print the handler
			 * address as an "unreliable" hint that function graph
			 * tracing was involved.
			 */
			real_addr = ftrace_graph_ret_addr(task, &graph_idx,
							  addr, stack);
			if (real_addr != addr)
				printk_stack_address(addr, 0, log_lvl);
			printk_stack_address(real_addr, reliable, log_lvl);

			if (!reliable)
				continue;

			/*
			 * Get the next frame from the unwinder.  No need to
			 * check for an error: if anything goes wrong, the rest
			 * of the addresses will just be printed as unreliable.
			 */
			unwind_next_frame(&state);
		}

		if (str_end)
			printk("%s <%s> ", log_lvl, str_end);
	}
>>>>>>> v4.9.227
}

void show_stack(struct task_struct *task, unsigned long *sp)
{
<<<<<<< HEAD
	unsigned long bp = 0;
	unsigned long stack;
=======
	task = task ? : current;
>>>>>>> v4.9.227

	/*
	 * Stack frames below this one aren't interesting.  Don't show them
	 * if we're printing for %current.
	 */
<<<<<<< HEAD
	if (!sp && (!task || task == current)) {
		sp = &stack;
		bp = stack_frame(current, NULL);
	}

	show_stack_log_lvl(task, NULL, sp, bp, "");
=======
	if (!sp && task == current)
		sp = get_stack_pointer(current, NULL);

	show_stack_log_lvl(task, NULL, sp, "");
}

void show_stack_regs(struct pt_regs *regs)
{
	show_stack_log_lvl(current, regs, NULL, "");
>>>>>>> v4.9.227
}

static arch_spinlock_t die_lock = __ARCH_SPIN_LOCK_UNLOCKED;
static int die_owner = -1;
static unsigned int die_nest_count;

unsigned long oops_begin(void)
{
	int cpu;
	unsigned long flags;

	oops_enter();

	/* racy, but better than risking deadlock. */
	raw_local_irq_save(flags);
	cpu = smp_processor_id();
	if (!arch_spin_trylock(&die_lock)) {
		if (cpu == die_owner)
			/* nested oops. should stop eventually */;
		else
			arch_spin_lock(&die_lock);
	}
	die_nest_count++;
	die_owner = cpu;
	console_verbose();
	bust_spinlocks(1);
	return flags;
}
EXPORT_SYMBOL_GPL(oops_begin);
NOKPROBE_SYMBOL(oops_begin);

<<<<<<< HEAD
=======
void __noreturn rewind_stack_do_exit(int signr);

>>>>>>> v4.9.227
void oops_end(unsigned long flags, struct pt_regs *regs, int signr)
{
	if (regs && kexec_should_crash(current))
		crash_kexec(regs);

	bust_spinlocks(0);
	die_owner = -1;
	add_taint(TAINT_DIE, LOCKDEP_NOW_UNRELIABLE);
	die_nest_count--;
	if (!die_nest_count)
		/* Nest count reaches zero, release the lock. */
		arch_spin_unlock(&die_lock);
	raw_local_irq_restore(flags);
	oops_exit();

	if (!signr)
		return;
	if (in_interrupt())
		panic("Fatal exception in interrupt");
	if (panic_on_oops)
		panic("Fatal exception");
<<<<<<< HEAD
	do_exit(signr);
=======

	/*
	 * We're not going to return, but we might be on an IST stack or
	 * have very little stack space left.  Rewind the stack and kill
	 * the task.
	 * Before we rewind the stack, we have to tell KASAN that we're going to
	 * reuse the task stack and that existing poisons are invalid.
	 */
	kasan_unpoison_task_stack(current);
	rewind_stack_do_exit(signr);
>>>>>>> v4.9.227
}
NOKPROBE_SYMBOL(oops_end);

int __die(const char *str, struct pt_regs *regs, long err)
{
#ifdef CONFIG_X86_32
	unsigned short ss;
	unsigned long sp;
#endif
	printk(KERN_DEFAULT
<<<<<<< HEAD
	       "%s: %04lx [#%d] ", str, err & 0xffff, ++die_counter);
#ifdef CONFIG_PREEMPT
	printk("PREEMPT ");
#endif
#ifdef CONFIG_SMP
	printk("SMP ");
#endif
#ifdef CONFIG_DEBUG_PAGEALLOC
	printk("DEBUG_PAGEALLOC ");
#endif
#ifdef CONFIG_KASAN
	printk("KASAN");
#endif
	printk("\n");
=======
	       "%s: %04lx [#%d]%s%s%s%s\n", str, err & 0xffff, ++die_counter,
	       IS_ENABLED(CONFIG_PREEMPT) ? " PREEMPT"         : "",
	       IS_ENABLED(CONFIG_SMP)     ? " SMP"             : "",
	       debug_pagealloc_enabled()  ? " DEBUG_PAGEALLOC" : "",
	       IS_ENABLED(CONFIG_KASAN)   ? " KASAN"           : "");

>>>>>>> v4.9.227
	if (notify_die(DIE_OOPS, str, regs, err,
			current->thread.trap_nr, SIGSEGV) == NOTIFY_STOP)
		return 1;

	print_modules();
	show_regs(regs);
#ifdef CONFIG_X86_32
<<<<<<< HEAD
	if (user_mode_vm(regs)) {
=======
	if (user_mode(regs)) {
>>>>>>> v4.9.227
		sp = regs->sp;
		ss = regs->ss & 0xffff;
	} else {
		sp = kernel_stack_pointer(regs);
		savesegment(ss, ss);
	}
	printk(KERN_EMERG "EIP: [<%08lx>] ", regs->ip);
	print_symbol("%s", regs->ip);
	printk(" SS:ESP %04x:%08lx\n", ss, sp);
#else
	/* Executive summary in case the oops scrolled away */
	printk(KERN_ALERT "RIP ");
	printk_address(regs->ip);
	printk(" RSP <%016lx>\n", regs->sp);
#endif
	return 0;
}
NOKPROBE_SYMBOL(__die);

/*
 * This is gone through when something in the kernel has done something bad
 * and is about to be terminated:
 */
void die(const char *str, struct pt_regs *regs, long err)
{
	unsigned long flags = oops_begin();
	int sig = SIGSEGV;

<<<<<<< HEAD
	if (!user_mode_vm(regs))
=======
	if (!user_mode(regs))
>>>>>>> v4.9.227
		report_bug(regs->ip, regs);

	if (__die(str, regs, err))
		sig = 0;
	oops_end(flags, regs, sig);
}

static int __init kstack_setup(char *s)
{
	ssize_t ret;
	unsigned long val;

	if (!s)
		return -EINVAL;

	ret = kstrtoul(s, 0, &val);
	if (ret)
		return ret;
	kstack_depth_to_print = val;
	return 0;
}
early_param("kstack", kstack_setup);

static int __init code_bytes_setup(char *s)
{
	ssize_t ret;
	unsigned long val;

	if (!s)
		return -EINVAL;

	ret = kstrtoul(s, 0, &val);
	if (ret)
		return ret;

	code_bytes = val;
	if (code_bytes > 8192)
		code_bytes = 8192;

	return 1;
}
__setup("code_bytes=", code_bytes_setup);
