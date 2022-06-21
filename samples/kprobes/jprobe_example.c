/*
 * Here's a sample kernel module showing the use of jprobes to dump
<<<<<<< HEAD
 * the arguments of do_fork().
=======
 * the arguments of _do_fork().
>>>>>>> v4.9.227
 *
 * For more information on theory of operation of jprobes, see
 * Documentation/kprobes.txt
 *
 * Build and insert the kernel module as done in the kprobe example.
 * You will see the trace data in /var/log/messages and on the
<<<<<<< HEAD
 * console whenever do_fork() is invoked to create a new process.
=======
 * console whenever _do_fork() is invoked to create a new process.
>>>>>>> v4.9.227
 * (Some messages may be suppressed if syslogd is configured to
 * eliminate duplicate messages.)
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>

/*
<<<<<<< HEAD
 * Jumper probe for do_fork.
=======
 * Jumper probe for _do_fork.
>>>>>>> v4.9.227
 * Mirror principle enables access to arguments of the probed routine
 * from the probe handler.
 */

<<<<<<< HEAD
/* Proxy routine having the same arguments as actual do_fork() routine */
static long jdo_fork(unsigned long clone_flags, unsigned long stack_start,
	      unsigned long stack_size, int __user *parent_tidptr,
	      int __user *child_tidptr)
=======
/* Proxy routine having the same arguments as actual _do_fork() routine */
static long j_do_fork(unsigned long clone_flags, unsigned long stack_start,
	      unsigned long stack_size, int __user *parent_tidptr,
	      int __user *child_tidptr, unsigned long tls)
>>>>>>> v4.9.227
{
	pr_info("jprobe: clone_flags = 0x%lx, stack_start = 0x%lx "
		"stack_size = 0x%lx\n", clone_flags, stack_start, stack_size);

	/* Always end with a call to jprobe_return(). */
	jprobe_return();
	return 0;
}

static struct jprobe my_jprobe = {
<<<<<<< HEAD
	.entry			= jdo_fork,
	.kp = {
		.symbol_name	= "do_fork",
=======
	.entry			= j_do_fork,
	.kp = {
		.symbol_name	= "_do_fork",
>>>>>>> v4.9.227
	},
};

static int __init jprobe_init(void)
{
	int ret;

	ret = register_jprobe(&my_jprobe);
	if (ret < 0) {
<<<<<<< HEAD
		printk(KERN_INFO "register_jprobe failed, returned %d\n", ret);
		return -1;
	}
	printk(KERN_INFO "Planted jprobe at %p, handler addr %p\n",
=======
		pr_err("register_jprobe failed, returned %d\n", ret);
		return -1;
	}
	pr_info("Planted jprobe at %p, handler addr %p\n",
>>>>>>> v4.9.227
	       my_jprobe.kp.addr, my_jprobe.entry);
	return 0;
}

static void __exit jprobe_exit(void)
{
	unregister_jprobe(&my_jprobe);
<<<<<<< HEAD
	printk(KERN_INFO "jprobe at %p unregistered\n", my_jprobe.kp.addr);
=======
	pr_info("jprobe at %p unregistered\n", my_jprobe.kp.addr);
>>>>>>> v4.9.227
}

module_init(jprobe_init)
module_exit(jprobe_exit)
MODULE_LICENSE("GPL");
