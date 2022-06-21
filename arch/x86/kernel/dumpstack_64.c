/*
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 2000, 2001, 2002 Andi Kleen, SuSE Labs
 */
#include <linux/kallsyms.h>
#include <linux/kprobes.h>
#include <linux/uaccess.h>
#include <linux/hardirq.h>
#include <linux/kdebug.h>
<<<<<<< HEAD
#include <linux/module.h>
=======
#include <linux/export.h>
>>>>>>> v4.9.227
#include <linux/ptrace.h>
#include <linux/kexec.h>
#include <linux/sysfs.h>
#include <linux/bug.h>
#include <linux/nmi.h>

#include <asm/stacktrace.h>

<<<<<<< HEAD

#define N_EXCEPTION_STACKS_END \
		(N_EXCEPTION_STACKS + DEBUG_STKSZ/EXCEPTION_STKSZ - 2)

static char x86_stack_ids[][8] = {
		[ DEBUG_STACK-1			]	= "#DB",
		[ NMI_STACK-1			]	= "NMI",
		[ DOUBLEFAULT_STACK-1		]	= "#DF",
		[ MCE_STACK-1			]	= "#MC",
#if DEBUG_STKSZ > EXCEPTION_STKSZ
		[ N_EXCEPTION_STACKS ...
		  N_EXCEPTION_STACKS_END	]	= "#DB[?]"
#endif
};

static unsigned long *in_exception_stack(unsigned cpu, unsigned long stack,
					 unsigned *usedp, char **idp)
{
	unsigned k;

	/*
	 * Iterate over all exception stacks, and figure out whether
	 * 'stack' is in one of them:
	 */
	for (k = 0; k < N_EXCEPTION_STACKS; k++) {
		unsigned long end = per_cpu(orig_ist, cpu).ist[k];
		/*
		 * Is 'stack' above this exception frame's end?
		 * If yes then skip to the next frame.
		 */
		if (stack >= end)
			continue;
		/*
		 * Is 'stack' above this exception frame's start address?
		 * If yes then we found the right frame.
		 */
		if (stack >= end - EXCEPTION_STKSZ) {
			/*
			 * Make sure we only iterate through an exception
			 * stack once. If it comes up for the second time
			 * then there's something wrong going on - just
			 * break out and return NULL:
			 */
			if (*usedp & (1U << k))
				break;
			*usedp |= 1U << k;
			*idp = x86_stack_ids[k];
			return (unsigned long *)end;
		}
		/*
		 * If this is a debug stack, and if it has a larger size than
		 * the usual exception stacks, then 'stack' might still
		 * be within the lower portion of the debug stack:
		 */
#if DEBUG_STKSZ > EXCEPTION_STKSZ
		if (k == DEBUG_STACK - 1 && stack >= end - DEBUG_STKSZ) {
			unsigned j = N_EXCEPTION_STACKS - 1;

			/*
			 * Black magic. A large debug stack is composed of
			 * multiple exception stack entries, which we
			 * iterate through now. Dont look:
			 */
			do {
				++j;
				end -= EXCEPTION_STKSZ;
				x86_stack_ids[j][4] = '1' +
						(j - N_EXCEPTION_STACKS);
			} while (stack < end - EXCEPTION_STKSZ);
			if (*usedp & (1U << j))
				break;
			*usedp |= 1U << j;
			*idp = x86_stack_ids[j];
			return (unsigned long *)end;
		}
#endif
	}
	return NULL;
}

static inline int
in_irq_stack(unsigned long *stack, unsigned long *irq_stack,
	     unsigned long *irq_stack_end)
{
	return (stack >= irq_stack && stack < irq_stack_end);
}

static const unsigned long irq_stack_size =
	(IRQ_STACK_SIZE - 64) / sizeof(unsigned long);

enum stack_type {
	STACK_IS_UNKNOWN,
	STACK_IS_NORMAL,
	STACK_IS_EXCEPTION,
	STACK_IS_IRQ,
};

static enum stack_type
analyze_stack(int cpu, struct task_struct *task, unsigned long *stack,
	      unsigned long **stack_end, unsigned long *irq_stack,
	      unsigned *used, char **id)
{
	unsigned long addr;

	addr = ((unsigned long)stack & (~(THREAD_SIZE - 1)));
	if ((unsigned long)task_stack_page(task) == addr)
		return STACK_IS_NORMAL;

	*stack_end = in_exception_stack(cpu, (unsigned long)stack,
					used, id);
	if (*stack_end)
		return STACK_IS_EXCEPTION;

	if (!irq_stack)
		return STACK_IS_NORMAL;

	*stack_end = irq_stack;
	irq_stack = irq_stack - irq_stack_size;

	if (in_irq_stack(stack, irq_stack, *stack_end))
		return STACK_IS_IRQ;

	return STACK_IS_UNKNOWN;
}

/*
 * x86-64 can have up to three kernel stacks:
 * process stack
 * interrupt stack
 * severe exception (double fault, nmi, stack fault, debug, mce) hardware stack
 */

void dump_trace(struct task_struct *task, struct pt_regs *regs,
		unsigned long *stack, unsigned long bp,
		const struct stacktrace_ops *ops, void *data)
{
	const unsigned cpu = get_cpu();
	struct thread_info *tinfo;
	unsigned long *irq_stack = (unsigned long *)per_cpu(irq_stack_ptr, cpu);
	unsigned long dummy;
	unsigned used = 0;
	int graph = 0;
	int done = 0;

	if (!task)
		task = current;

	if (!stack) {
		if (regs)
			stack = (unsigned long *)regs->sp;
		else if (task != current)
			stack = (unsigned long *)task->thread.sp;
		else
			stack = &dummy;
	}

	if (!bp)
		bp = stack_frame(task, regs);
	/*
	 * Print function call entries in all stacks, starting at the
	 * current stack address. If the stacks consist of nested
	 * exceptions
	 */
	tinfo = task_thread_info(task);
	while (!done) {
		unsigned long *stack_end;
		enum stack_type stype;
		char *id;

		stype = analyze_stack(cpu, task, stack, &stack_end,
				      irq_stack, &used, &id);

		/* Default finish unless specified to continue */
		done = 1;

		switch (stype) {

		/* Break out early if we are on the thread stack */
		case STACK_IS_NORMAL:
			break;

		case STACK_IS_EXCEPTION:

			if (ops->stack(data, id) < 0)
				break;

			bp = ops->walk_stack(tinfo, stack, bp, ops,
					     data, stack_end, &graph);
			ops->stack(data, "<EOE>");
			/*
			 * We link to the next stack via the
			 * second-to-last pointer (index -2 to end) in the
			 * exception stack:
			 */
			stack = (unsigned long *) stack_end[-2];
			done = 0;
			break;

		case STACK_IS_IRQ:

			if (ops->stack(data, "IRQ") < 0)
				break;
			bp = ops->walk_stack(tinfo, stack, bp,
				     ops, data, stack_end, &graph);
			/*
			 * We link to the next stack (which would be
			 * the process stack normally) the last
			 * pointer (index -1 to end) in the IRQ stack:
			 */
			stack = (unsigned long *) (stack_end[-1]);
			irq_stack = NULL;
			ops->stack(data, "EOI");
			done = 0;
			break;

		case STACK_IS_UNKNOWN:
			ops->stack(data, "UNK");
			break;
		}
	}

	/*
	 * This handles the process stack:
	 */
	bp = ops->walk_stack(tinfo, stack, bp, ops, data, NULL, &graph);
	put_cpu();
}
EXPORT_SYMBOL(dump_trace);

void
show_stack_log_lvl(struct task_struct *task, struct pt_regs *regs,
		   unsigned long *sp, unsigned long bp, char *log_lvl)
=======
static char *exception_stack_names[N_EXCEPTION_STACKS] = {
		[ DOUBLEFAULT_STACK-1	]	= "#DF",
		[ NMI_STACK-1		]	= "NMI",
		[ DEBUG_STACK-1		]	= "#DB",
		[ MCE_STACK-1		]	= "#MC",
};

static unsigned long exception_stack_sizes[N_EXCEPTION_STACKS] = {
	[0 ... N_EXCEPTION_STACKS - 1]		= EXCEPTION_STKSZ,
	[DEBUG_STACK - 1]			= DEBUG_STKSZ
};

void stack_type_str(enum stack_type type, const char **begin, const char **end)
{
	BUILD_BUG_ON(N_EXCEPTION_STACKS != 4);

	switch (type) {
	case STACK_TYPE_IRQ:
		*begin = "IRQ";
		*end   = "EOI";
		break;
	case STACK_TYPE_EXCEPTION ... STACK_TYPE_EXCEPTION_LAST:
		*begin = exception_stack_names[type - STACK_TYPE_EXCEPTION];
		*end   = "EOE";
		break;
	default:
		*begin = NULL;
		*end   = NULL;
	}
}

static bool in_exception_stack(unsigned long *stack, struct stack_info *info)
{
	unsigned long *begin, *end;
	struct pt_regs *regs;
	unsigned k;

	BUILD_BUG_ON(N_EXCEPTION_STACKS != 4);

	for (k = 0; k < N_EXCEPTION_STACKS; k++) {
		end   = (unsigned long *)raw_cpu_ptr(&orig_ist)->ist[k];
		begin = end - (exception_stack_sizes[k] / sizeof(long));
		regs  = (struct pt_regs *)end - 1;

		if (stack < begin || stack >= end)
			continue;

		info->type	= STACK_TYPE_EXCEPTION + k;
		info->begin	= begin;
		info->end	= end;
		info->next_sp	= (unsigned long *)regs->sp;

		return true;
	}

	return false;
}

static bool in_irq_stack(unsigned long *stack, struct stack_info *info)
{
	unsigned long *end   = (unsigned long *)this_cpu_read(irq_stack_ptr);
	unsigned long *begin = end - (IRQ_STACK_SIZE / sizeof(long));

	/*
	 * This is a software stack, so 'end' can be a valid stack pointer.
	 * It just means the stack is empty.
	 */
	if (stack < begin || stack > end)
		return false;

	info->type	= STACK_TYPE_IRQ;
	info->begin	= begin;
	info->end	= end;

	/*
	 * The next stack pointer is the first thing pushed by the entry code
	 * after switching to the irq stack.
	 */
	info->next_sp = (unsigned long *)*(end - 1);

	return true;
}

int get_stack_info(unsigned long *stack, struct task_struct *task,
		   struct stack_info *info, unsigned long *visit_mask)
{
	if (!stack)
		goto unknown;

	task = task ? : current;

	if (in_task_stack(stack, task, info))
		goto recursion_check;

	if (task != current)
		goto unknown;

	if (in_exception_stack(stack, info))
		goto recursion_check;

	if (in_irq_stack(stack, info))
		goto recursion_check;

	goto unknown;

recursion_check:
	/*
	 * Make sure we don't iterate through any given stack more than once.
	 * If it comes up a second time then there's something wrong going on:
	 * just break out and report an unknown stack type.
	 */
	if (visit_mask) {
		if (*visit_mask & (1UL << info->type))
			goto unknown;
		*visit_mask |= 1UL << info->type;
	}

	return 0;

unknown:
	info->type = STACK_TYPE_UNKNOWN;
	return -EINVAL;
}

void show_stack_log_lvl(struct task_struct *task, struct pt_regs *regs,
			unsigned long *sp, char *log_lvl)
>>>>>>> v4.9.227
{
	unsigned long *irq_stack_end;
	unsigned long *irq_stack;
	unsigned long *stack;
<<<<<<< HEAD
	int cpu;
	int i;

	preempt_disable();
	cpu = smp_processor_id();

	irq_stack_end	= (unsigned long *)(per_cpu(irq_stack_ptr, cpu));
	irq_stack	= (unsigned long *)(per_cpu(irq_stack_ptr, cpu) - IRQ_STACK_SIZE);

	/*
	 * Debugging aid: "show_stack(NULL, NULL);" prints the
	 * back trace for this cpu:
	 */
	if (sp == NULL) {
		if (task)
			sp = (unsigned long *)task->thread.sp;
		else
			sp = (unsigned long *)&sp;
	}

	stack = sp;
	for (i = 0; i < kstack_depth_to_print; i++) {
=======
	int i;

	if (!try_get_task_stack(task))
		return;

	irq_stack_end = (unsigned long *)this_cpu_read(irq_stack_ptr);
	irq_stack     = irq_stack_end - (IRQ_STACK_SIZE / sizeof(long));

	sp = sp ? : get_stack_pointer(task, regs);

	stack = sp;
	for (i = 0; i < kstack_depth_to_print; i++) {
		unsigned long word;

>>>>>>> v4.9.227
		if (stack >= irq_stack && stack <= irq_stack_end) {
			if (stack == irq_stack_end) {
				stack = (unsigned long *) (irq_stack_end[-1]);
				pr_cont(" <EOI> ");
			}
		} else {
<<<<<<< HEAD
		if (((long) stack & (THREAD_SIZE-1)) == 0)
			break;
		}
		if (i && ((i % STACKSLOTS_PER_LINE) == 0))
			pr_cont("\n");
		pr_cont(" %016lx", *stack++);
		touch_nmi_watchdog();
	}
	preempt_enable();

	pr_cont("\n");
	show_trace_log_lvl(task, regs, sp, bp, log_lvl);
=======
		if (kstack_end(stack))
			break;
		}

		if (probe_kernel_address(stack, word))
			break;

		if ((i % STACKSLOTS_PER_LINE) == 0) {
			if (i != 0)
				pr_cont("\n");
			printk("%s %016lx", log_lvl, word);
		} else
			pr_cont(" %016lx", word);

		stack++;
		touch_nmi_watchdog();
	}

	pr_cont("\n");
	show_trace_log_lvl(task, regs, sp, log_lvl);

	put_task_stack(task);
>>>>>>> v4.9.227
}

void show_regs(struct pt_regs *regs)
{
	int i;
<<<<<<< HEAD
	unsigned long sp;

	sp = regs->sp;
=======

>>>>>>> v4.9.227
	show_regs_print_info(KERN_DEFAULT);
	__show_regs(regs, 1);

	/*
	 * When in-kernel, we also print out the stack and code at the
	 * time of the fault..
	 */
	if (!user_mode(regs)) {
		unsigned int code_prologue = code_bytes * 43 / 64;
		unsigned int code_len = code_bytes;
		unsigned char c;
		u8 *ip;

		printk(KERN_DEFAULT "Stack:\n");
<<<<<<< HEAD
		show_stack_log_lvl(NULL, regs, (unsigned long *)sp,
				   0, KERN_DEFAULT);
=======
		show_stack_log_lvl(current, regs, NULL, KERN_DEFAULT);
>>>>>>> v4.9.227

		printk(KERN_DEFAULT "Code: ");

		ip = (u8 *)regs->ip - code_prologue;
		if (ip < (u8 *)PAGE_OFFSET || probe_kernel_address(ip, c)) {
			/* try starting at IP */
			ip = (u8 *)regs->ip;
			code_len = code_len - code_prologue + 1;
		}
		for (i = 0; i < code_len; i++, ip++) {
			if (ip < (u8 *)PAGE_OFFSET ||
					probe_kernel_address(ip, c)) {
				pr_cont(" Bad RIP value.");
				break;
			}
			if (ip == (u8 *)regs->ip)
				pr_cont("<%02x> ", c);
			else
				pr_cont("%02x ", c);
		}
	}
	pr_cont("\n");
}

int is_valid_bugaddr(unsigned long ip)
{
	unsigned short ud2;

	if (__copy_from_user(&ud2, (const void __user *) ip, sizeof(ud2)))
		return 0;

	return ud2 == 0x0b0f;
}
