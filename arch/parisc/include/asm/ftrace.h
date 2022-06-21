#ifndef _ASM_PARISC_FTRACE_H
#define _ASM_PARISC_FTRACE_H

#ifndef __ASSEMBLY__
extern void mcount(void);

<<<<<<< HEAD
/*
 * Stack of return addresses for functions of a thread.
 * Used in struct thread_info
 */
struct ftrace_ret_stack {
	unsigned long ret;
	unsigned long func;
	unsigned long long calltime;
};

/*
 * Primary handler of a function return.
 * It relays on ftrace_return_to_handler.
 * Defined in entry.S
 */
extern void return_to_handler(void);

=======
#define MCOUNT_INSN_SIZE 4

extern unsigned long sys_call_table[];
>>>>>>> v4.9.227

extern unsigned long return_address(unsigned int);

#define ftrace_return_address(n) return_address(n)

#endif /* __ASSEMBLY__ */

#endif /* _ASM_PARISC_FTRACE_H */
