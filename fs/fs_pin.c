#include <linux/fs.h>
<<<<<<< HEAD
#include <linux/slab.h>
#include <linux/fs_pin.h>
#include "internal.h"
#include "mount.h"

static void pin_free_rcu(struct rcu_head *head)
{
	kfree(container_of(head, struct fs_pin, rcu));
}

static DEFINE_SPINLOCK(pin_lock);

void pin_put(struct fs_pin *p)
{
	if (atomic_long_dec_and_test(&p->count))
		call_rcu(&p->rcu, pin_free_rcu);
}

=======
#include <linux/sched.h>
#include <linux/slab.h>
#include "internal.h"
#include "mount.h"

static DEFINE_SPINLOCK(pin_lock);

>>>>>>> v4.9.227
void pin_remove(struct fs_pin *pin)
{
	spin_lock(&pin_lock);
	hlist_del_init(&pin->m_list);
	hlist_del_init(&pin->s_list);
	spin_unlock(&pin_lock);
<<<<<<< HEAD
=======
	spin_lock_irq(&pin->wait.lock);
	pin->done = 1;
	wake_up_locked(&pin->wait);
	spin_unlock_irq(&pin->wait.lock);
}

void pin_insert_group(struct fs_pin *pin, struct vfsmount *m, struct hlist_head *p)
{
	spin_lock(&pin_lock);
	if (p)
		hlist_add_head(&pin->s_list, p);
	hlist_add_head(&pin->m_list, &real_mount(m)->mnt_pins);
	spin_unlock(&pin_lock);
>>>>>>> v4.9.227
}

void pin_insert(struct fs_pin *pin, struct vfsmount *m)
{
<<<<<<< HEAD
	spin_lock(&pin_lock);
	hlist_add_head(&pin->s_list, &m->mnt_sb->s_pins);
	hlist_add_head(&pin->m_list, &real_mount(m)->mnt_pins);
	spin_unlock(&pin_lock);
=======
	pin_insert_group(pin, m, &m->mnt_sb->s_pins);
}

void pin_kill(struct fs_pin *p)
{
	wait_queue_t wait;

	if (!p) {
		rcu_read_unlock();
		return;
	}
	init_wait(&wait);
	spin_lock_irq(&p->wait.lock);
	if (likely(!p->done)) {
		p->done = -1;
		spin_unlock_irq(&p->wait.lock);
		rcu_read_unlock();
		p->kill(p);
		return;
	}
	if (p->done > 0) {
		spin_unlock_irq(&p->wait.lock);
		rcu_read_unlock();
		return;
	}
	__add_wait_queue(&p->wait, &wait);
	while (1) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		spin_unlock_irq(&p->wait.lock);
		rcu_read_unlock();
		schedule();
		rcu_read_lock();
		if (likely(list_empty(&wait.task_list)))
			break;
		/* OK, we know p couldn't have been freed yet */
		spin_lock_irq(&p->wait.lock);
		if (p->done > 0) {
			spin_unlock_irq(&p->wait.lock);
			break;
		}
	}
	rcu_read_unlock();
>>>>>>> v4.9.227
}

void mnt_pin_kill(struct mount *m)
{
	while (1) {
		struct hlist_node *p;
<<<<<<< HEAD
		struct fs_pin *pin;
=======
>>>>>>> v4.9.227
		rcu_read_lock();
		p = ACCESS_ONCE(m->mnt_pins.first);
		if (!p) {
			rcu_read_unlock();
			break;
		}
<<<<<<< HEAD
		pin = hlist_entry(p, struct fs_pin, m_list);
		if (!atomic_long_inc_not_zero(&pin->count)) {
			rcu_read_unlock();
			cpu_relax();
			continue;
		}
		rcu_read_unlock();
		pin->kill(pin);
	}
}

void sb_pin_kill(struct super_block *sb)
{
	while (1) {
		struct hlist_node *p;
		struct fs_pin *pin;
		rcu_read_lock();
		p = ACCESS_ONCE(sb->s_pins.first);
		if (!p) {
			rcu_read_unlock();
			break;
		}
		pin = hlist_entry(p, struct fs_pin, s_list);
		if (!atomic_long_inc_not_zero(&pin->count)) {
			rcu_read_unlock();
			cpu_relax();
			continue;
		}
		rcu_read_unlock();
		pin->kill(pin);
=======
		pin_kill(hlist_entry(p, struct fs_pin, m_list));
	}
}

void group_pin_kill(struct hlist_head *p)
{
	while (1) {
		struct hlist_node *q;
		rcu_read_lock();
		q = ACCESS_ONCE(p->first);
		if (!q) {
			rcu_read_unlock();
			break;
		}
		pin_kill(hlist_entry(q, struct fs_pin, s_list));
>>>>>>> v4.9.227
	}
}
