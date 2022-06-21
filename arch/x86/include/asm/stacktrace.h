/*
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *  Copyright (C) 2000, 2001, 2002 Andi Kleen, SuSE Labs
 */

#ifndef _ASM_X86_STACKTRACE_H
#define _ASM_X86_STACKTRACE_H

#include <linux/uaccess.h>
#include <linux/ptrace.h>
<<<<<<< HEAD

extern int kstack_depth_to_print;

struct thread_info;
struct stacktrace_ops;

typedef unsigned long (*walk_stack_t)(struct thread_info *tinfo,
				      unsigned long *stack,
				      unsigned long bp,
				      const struct stacktrace_ops *ops,
				      void *data,
				      unsigned long *end,
				      int *graph);

extern unsigned long
print_context_stack(struct thread_info *tinfo,
		    unsigned long *stack, unsigned long bp,
		    const struct stacktrace_ops *ops, void *data,
		    unsigned long *end, int *graph);

extern unsigned long
print_context_stack_bp(struct thread_info *tinfo,
		       unsigned long *stack, unsigned long bp,
		       const struct stacktrace_ops *ops, void *data,
		       unsigned long *end, int *graph);

/* Generic stack tracer with callbacks */

struct stacktrace_ops {
	void (*address)(void *data, unsigned long address, int reliable);
	/* On negative return stop dumping */
	int (*stack)(void *data, char *name);
	walk_stack_t	walk_stack;
};

void dump_trace(struct task_struct *tsk, struct pt_regs *regs,
		unsigned long *stack, unsigned long bp,
		const struct stacktrace_ops *ops, void *data);

#ifdef CONFIG_X86_32
#define STACKSLOTS_PER_LINE 8
#define get_bp(bp) asm("movl %%ebp, %0" : "=r" (bp) :)
#else
#define STACKSLOTS_PER_LINE 4
#define get_bp(bp) asm("movq %%rbp, %0" : "=r" (bp) :)
#endif

#ifdef CONFIG_FRAME_POINTER
static inline unsigned long
stack_frame(struct task_struct *task, struct pt_regs *regs)
{
	unsigned long bp;

	if (regs)
		return regs->bp;

	if (task == current) {
		/* Grab bp right from our regs */
		get_bp(bp);
		return bp;
	}

	/* bp is the last reg pushed by switch_to */
	return *(unsigned long *)task->thread.sp;
}
#else
static inline unsigned long
stack_frame(struct task_struct *task, struct pt_regs *regs)
{
	return 0;
}
#endif

extern void
show_trace_log_lvl(struct task_struct *task, struct pt_regs *regs,
		   unsigned long *stack, unsigned long bp, char *log_lvl);

extern void
show_stack_log_lvl(struct task_struct *task, struct pt_regs *regs,
		   unsigned long *sp, unsigned long bp, char *log_lvl);
=======
#include <asm/switch_to.h>

enum stack_type {
	STACK_TYPE_UNKNOWN,
	STACK_TYPE_TASK,
	STACK_TYPE_IRQ,
	STACK_TYPE_SOFTIRQ,
	STACK_TYPE_EXCEPTION,
	STACK_TYPE_EXCEPTION_LAST = STACK_TYPE_EXCEPTION + N_EXCEPTION_STACKS-1,
};

struct stack_info {
	enum stack_type type;
	unsigned long *begin, *end, *next_sp;
};

bool in_task_stack(unsigned long *stack, struct task_struct *task,
		   struct stack_info *info);

int get_stack_info(unsigned long *stack, struct task_struct *task,
		   struct stack_info *info, unsigned long *visit_mask);

void stack_type_str(enum stack_type type, const char **begin,
		    const char **end);

static inline bool on_stack(struct stack_info *info, void *addr, size_t len)
{
	void *begin = info->begin;
	void *end   = info->end;

	return (info->type != STACK_TYPE_UNKNOWN &&
		addr >= begin && addr < end &&
		addr + len > begin && addr + len <= end);
}

extern int kstack_depth_to_print;

#ifdef CONFIG_X86_32
#define STACKSLOTS_PER_LINE 8
#else
#define STACKSLOTS_PER_LINE 4
#endif

#ifdef CONFIG_FRAME_POINTER
static inline unsigned long *
get_frame_pointer(struct task_struct *task, struct pt_regs *regs)
{
	struct inactive_task_frame *frame;

	if (regs)
		return (unsigned long *)regs->bp;

	if (task == current)
		return __builtin_frame_address(0);

	frame = (struct inactive_task_frame *)task->thread.sp;
	return (unsigned long *)READ_ONCE_NOCHECK(frame->bp);
}
#else
static inline unsigned long *
get_frame_pointer(struct task_struct *task, struct pt_regs *regs)
{
	return NULL;
}
#endif /* CONFIG_FRAME_POINTER */

static inline unsigned long *
get_stack_pointer(struct task_struct *task, struct pt_regs *regs)
{
	if (regs)
		return (unsigned long *)kernel_stack_pointer(regs);

	if (task == current)
		return __builtin_frame_address(0);

	return (unsigned long *)task->thread.sp;
}

void show_trace_log_lvl(struct task_struct *task, struct pt_regs *regs,
			unsigned long *stack, char *log_lvl);

void show_stack_log_lvl(struct task_struct *task, struct pt_regs *regs,
			unsigned long *sp, char *log_lvl);
>>>>>>> v4.9.227

extern unsigned int code_bytes;

/* The form of the top of the frame on the stack */
struct stack_frame {
	struct stack_frame *next_frame;
	unsigned long return_address;
};

struct stack_frame_ia32 {
    u32 next_frame;
    u32 return_address;
};

static inline unsigned long caller_frame_pointer(void)
{
	struct stack_frame *frame;

<<<<<<< HEAD
	get_bp(frame);
=======
	frame = __builtin_frame_address(0);
>>>>>>> v4.9.227

#ifdef CONFIG_FRAME_POINTER
	frame = frame->next_frame;
#endif

	return (unsigned long)frame;
}

#endif /* _ASM_X86_STACKTRACE_H */
