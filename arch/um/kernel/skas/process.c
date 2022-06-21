/*
 * Copyright (C) 2002 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 * Licensed under the GPL
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <as-layout.h>
#include <kern.h>
#include <os.h>
#include <skas.h>

<<<<<<< HEAD
int new_mm(unsigned long stack)
{
	int fd, err;

	fd = os_open_file("/proc/mm", of_cloexec(of_write(OPENFLAGS())), 0);
	if (fd < 0)
		return fd;

	if (skas_needs_stub) {
		err = map_stub_pages(fd, STUB_CODE, STUB_DATA, stack);
		if (err) {
			os_close_file(fd);
			return err;
		}
	}

	return fd;
}

=======
>>>>>>> v4.9.227
extern void start_kernel(void);

static int __init start_kernel_proc(void *unused)
{
	int pid;

	block_signals();
	pid = os_getpid();

	cpu_tasks[0].pid = pid;
	cpu_tasks[0].task = current;
<<<<<<< HEAD
#ifdef CONFIG_SMP
	init_cpu_online(get_cpu_mask(0));
#endif
=======

>>>>>>> v4.9.227
	start_kernel();
	return 0;
}

extern int userspace_pid[];

extern char cpu0_irqstack[];

int __init start_uml(void)
{
	stack_protections((unsigned long) &cpu0_irqstack);
	set_sigstack(cpu0_irqstack, THREAD_SIZE);
<<<<<<< HEAD
	if (proc_mm) {
		userspace_pid[0] = start_userspace(0);
		if (userspace_pid[0] < 0) {
			printf("start_uml - start_userspace returned %d\n",
			       userspace_pid[0]);
			exit(1);
		}
	}
=======
>>>>>>> v4.9.227

	init_new_thread_signals();

	init_task.thread.request.u.thread.proc = start_kernel_proc;
	init_task.thread.request.u.thread.arg = NULL;
	return start_idle_thread(task_stack_page(&init_task),
				 &init_task.thread.switch_buf);
}

unsigned long current_stub_stack(void)
{
	if (current->mm == NULL)
		return 0;

	return current->mm->context.id.stack;
}
