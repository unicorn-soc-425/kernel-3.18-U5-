/*
 * Mutexes: blocking mutual exclusion locks
 *
 * started by Ingo Molnar:
 *
 *  Copyright (C) 2004, 2005, 2006 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *
 * This file contains mutex debugging related internal declarations,
 * prototypes and inline functions, for the CONFIG_DEBUG_MUTEXES case.
 * More details are in kernel/mutex-debug.c.
 */

/*
 * This must be called with lock->wait_lock held.
 */
extern void debug_mutex_lock_common(struct mutex *lock,
				    struct mutex_waiter *waiter);
extern void debug_mutex_wake_waiter(struct mutex *lock,
				    struct mutex_waiter *waiter);
extern void debug_mutex_free_waiter(struct mutex_waiter *waiter);
extern void debug_mutex_add_waiter(struct mutex *lock,
				   struct mutex_waiter *waiter,
<<<<<<< HEAD
				   struct thread_info *ti);
extern void mutex_remove_waiter(struct mutex *lock, struct mutex_waiter *waiter,
				struct thread_info *ti);
=======
				   struct task_struct *task);
extern void mutex_remove_waiter(struct mutex *lock, struct mutex_waiter *waiter,
				struct task_struct *task);
>>>>>>> v4.9.227
extern void debug_mutex_unlock(struct mutex *lock);
extern void debug_mutex_init(struct mutex *lock, const char *name,
			     struct lock_class_key *key);

static inline void mutex_set_owner(struct mutex *lock)
{
<<<<<<< HEAD
	lock->owner = current;
=======
	WRITE_ONCE(lock->owner, current);
>>>>>>> v4.9.227
}

static inline void mutex_clear_owner(struct mutex *lock)
{
<<<<<<< HEAD
	lock->owner = NULL;
=======
	WRITE_ONCE(lock->owner, NULL);
>>>>>>> v4.9.227
}

#define spin_lock_mutex(lock, flags)			\
	do {						\
		struct mutex *l = container_of(lock, struct mutex, wait_lock); \
							\
		DEBUG_LOCKS_WARN_ON(in_interrupt());	\
		local_irq_save(flags);			\
		arch_spin_lock(&(lock)->rlock.raw_lock);\
		DEBUG_LOCKS_WARN_ON(l->magic != l);	\
	} while (0)

#define spin_unlock_mutex(lock, flags)				\
	do {							\
		arch_spin_unlock(&(lock)->rlock.raw_lock);	\
		local_irq_restore(flags);			\
		preempt_check_resched();			\
	} while (0)
