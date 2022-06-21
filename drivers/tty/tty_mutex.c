#include <linux/tty.h>
#include <linux/module.h>
#include <linux/kallsyms.h>
#include <linux/semaphore.h>
#include <linux/sched.h>

/* Legacy tty mutex glue */

<<<<<<< HEAD
enum {
	TTY_MUTEX_NORMAL,
	TTY_MUTEX_NESTED,
};

=======
>>>>>>> v4.9.227
/*
 * Getting the big tty mutex.
 */

<<<<<<< HEAD
static void __lockfunc tty_lock_nested(struct tty_struct *tty,
				       unsigned int subclass)
{
	if (tty->magic != TTY_MAGIC) {
		pr_err("L Bad %p\n", tty);
		WARN_ON(1);
		return;
	}
	tty_kref_get(tty);
	mutex_lock_nested(&tty->legacy_mutex, subclass);
}

void __lockfunc tty_lock(struct tty_struct *tty)
{
	return tty_lock_nested(tty, TTY_MUTEX_NORMAL);
}
EXPORT_SYMBOL(tty_lock);

void __lockfunc tty_unlock(struct tty_struct *tty)
{
	if (tty->magic != TTY_MAGIC) {
		pr_err("U Bad %p\n", tty);
		WARN_ON(1);
		return;
	}
=======
void tty_lock(struct tty_struct *tty)
{
	if (WARN(tty->magic != TTY_MAGIC, "L Bad %p\n", tty))
		return;
	tty_kref_get(tty);
	mutex_lock(&tty->legacy_mutex);
}
EXPORT_SYMBOL(tty_lock);

int tty_lock_interruptible(struct tty_struct *tty)
{
	int ret;

	if (WARN(tty->magic != TTY_MAGIC, "L Bad %p\n", tty))
		return -EIO;
	tty_kref_get(tty);
	ret = mutex_lock_interruptible(&tty->legacy_mutex);
	if (ret)
		tty_kref_put(tty);
	return ret;
}

void tty_unlock(struct tty_struct *tty)
{
	if (WARN(tty->magic != TTY_MAGIC, "U Bad %p\n", tty))
		return;
>>>>>>> v4.9.227
	mutex_unlock(&tty->legacy_mutex);
	tty_kref_put(tty);
}
EXPORT_SYMBOL(tty_unlock);

<<<<<<< HEAD
/*
 * Getting the big tty mutex for a pair of ttys with lock ordering
 * On a non pty/tty pair tty2 can be NULL which is just fine.
 */
void __lockfunc tty_lock_pair(struct tty_struct *tty,
					struct tty_struct *tty2)
{
	if (tty < tty2) {
		tty_lock(tty);
		tty_lock_nested(tty2, TTY_MUTEX_NESTED);
	} else {
		if (tty2 && tty2 != tty)
			tty_lock(tty2);
		tty_lock_nested(tty, TTY_MUTEX_NESTED);
	}
}
EXPORT_SYMBOL(tty_lock_pair);

void __lockfunc tty_unlock_pair(struct tty_struct *tty,
						struct tty_struct *tty2)
{
	tty_unlock(tty);
	if (tty2 && tty2 != tty)
		tty_unlock(tty2);
}
EXPORT_SYMBOL(tty_unlock_pair);
=======
void tty_lock_slave(struct tty_struct *tty)
{
	if (tty && tty != tty->link)
		tty_lock(tty);
}

void tty_unlock_slave(struct tty_struct *tty)
{
	if (tty && tty != tty->link)
		tty_unlock(tty);
}

void tty_set_lock_subclass(struct tty_struct *tty)
{
	lockdep_set_subclass(&tty->legacy_mutex, TTY_LOCK_SLAVE);
}
>>>>>>> v4.9.227
