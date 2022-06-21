#include <linux/atomic.h>
#include <linux/rwsem.h>
#include <linux/percpu.h>
#include <linux/wait.h>
#include <linux/lockdep.h>
#include <linux/percpu-rwsem.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/errno.h>

<<<<<<< HEAD
int __percpu_init_rwsem(struct percpu_rw_semaphore *brw,
			const char *name, struct lock_class_key *rwsem_key)
{
	brw->fast_read_ctr = alloc_percpu(int);
	if (unlikely(!brw->fast_read_ctr))
		return -ENOMEM;

	/* ->rw_sem represents the whole percpu_rw_semaphore for lockdep */
	__init_rwsem(&brw->rw_sem, name, rwsem_key);
	atomic_set(&brw->write_ctr, 0);
	atomic_set(&brw->slow_read_ctr, 0);
	init_waitqueue_head(&brw->write_waitq);
	return 0;
}

void percpu_free_rwsem(struct percpu_rw_semaphore *brw)
{
	free_percpu(brw->fast_read_ctr);
	brw->fast_read_ctr = NULL; /* catch use after free bugs */
}

/*
 * This is the fast-path for down_read/up_read, it only needs to ensure
 * there is no pending writer (atomic_read(write_ctr) == 0) and inc/dec the
 * fast per-cpu counter. The writer uses synchronize_sched_expedited() to
 * serialize with the preempt-disabled section below.
 *
 * The nontrivial part is that we should guarantee acquire/release semantics
 * in case when
 *
 *	R_W: down_write() comes after up_read(), the writer should see all
 *	     changes done by the reader
 * or
 *	W_R: down_read() comes after up_write(), the reader should see all
 *	     changes done by the writer
 *
 * If this helper fails the callers rely on the normal rw_semaphore and
 * atomic_dec_and_test(), so in this case we have the necessary barriers.
 *
 * But if it succeeds we do not have any barriers, atomic_read(write_ctr) or
 * __this_cpu_add() below can be reordered with any LOAD/STORE done by the
 * reader inside the critical section. See the comments in down_write and
 * up_write below.
 */
static bool update_fast_ctr(struct percpu_rw_semaphore *brw, unsigned int val)
{
	bool success = false;

	preempt_disable();
	if (likely(!atomic_read(&brw->write_ctr))) {
		__this_cpu_add(*brw->fast_read_ctr, val);
		success = true;
	}
	preempt_enable();

	return success;
}

/*
 * Like the normal down_read() this is not recursive, the writer can
 * come after the first percpu_down_read() and create the deadlock.
 *
 * Note: returns with lock_is_held(brw->rw_sem) == T for lockdep,
 * percpu_up_read() does rwsem_release(). This pairs with the usage
 * of ->rw_sem in percpu_down/up_write().
 */
void percpu_down_read(struct percpu_rw_semaphore *brw)
{
	might_sleep();
	if (likely(update_fast_ctr(brw, +1))) {
		rwsem_acquire_read(&brw->rw_sem.dep_map, 0, 0, _RET_IP_);
		return;
	}

	down_read(&brw->rw_sem);
	atomic_inc(&brw->slow_read_ctr);
	/* avoid up_read()->rwsem_release() */
	__up_read(&brw->rw_sem);
}

void percpu_up_read(struct percpu_rw_semaphore *brw)
{
	rwsem_release(&brw->rw_sem.dep_map, 1, _RET_IP_);

	if (likely(update_fast_ctr(brw, -1)))
		return;

	/* false-positive is possible but harmless */
	if (atomic_dec_and_test(&brw->slow_read_ctr))
		wake_up_all(&brw->write_waitq);
}

static int clear_fast_ctr(struct percpu_rw_semaphore *brw)
{
	unsigned int sum = 0;
	int cpu;

	for_each_possible_cpu(cpu) {
		sum += per_cpu(*brw->fast_read_ctr, cpu);
		per_cpu(*brw->fast_read_ctr, cpu) = 0;
	}

	return sum;
}

/*
 * A writer increments ->write_ctr to force the readers to switch to the
 * slow mode, note the atomic_read() check in update_fast_ctr().
 *
 * After that the readers can only inc/dec the slow ->slow_read_ctr counter,
 * ->fast_read_ctr is stable. Once the writer moves its sum into the slow
 * counter it represents the number of active readers.
 *
 * Finally the writer takes ->rw_sem for writing and blocks the new readers,
 * then waits until the slow counter becomes zero.
 */
void percpu_down_write(struct percpu_rw_semaphore *brw)
{
	/* tell update_fast_ctr() there is a pending writer */
	atomic_inc(&brw->write_ctr);
	/*
	 * 1. Ensures that write_ctr != 0 is visible to any down_read/up_read
	 *    so that update_fast_ctr() can't succeed.
	 *
	 * 2. Ensures we see the result of every previous this_cpu_add() in
	 *    update_fast_ctr().
	 *
	 * 3. Ensures that if any reader has exited its critical section via
	 *    fast-path, it executes a full memory barrier before we return.
	 *    See R_W case in the comment above update_fast_ctr().
	 */
	synchronize_sched_expedited();

	/* exclude other writers, and block the new readers completely */
	down_write(&brw->rw_sem);

	/* nobody can use fast_read_ctr, move its sum into slow_read_ctr */
	atomic_add(clear_fast_ctr(brw), &brw->slow_read_ctr);

	/* wait for all readers to complete their percpu_up_read() */
	wait_event(brw->write_waitq, !atomic_read(&brw->slow_read_ctr));
}

void percpu_up_write(struct percpu_rw_semaphore *brw)
{
	/* release the lock, but the readers can't use the fast-path */
	up_write(&brw->rw_sem);
	/*
	 * Insert the barrier before the next fast-path in down_read,
	 * see W_R case in the comment above update_fast_ctr().
	 */
	synchronize_sched_expedited();
	/* the last writer unblocks update_fast_ctr() */
	atomic_dec(&brw->write_ctr);
}
=======
int __percpu_init_rwsem(struct percpu_rw_semaphore *sem,
			const char *name, struct lock_class_key *rwsem_key)
{
	sem->read_count = alloc_percpu(int);
	if (unlikely(!sem->read_count))
		return -ENOMEM;

	/* ->rw_sem represents the whole percpu_rw_semaphore for lockdep */
	rcu_sync_init(&sem->rss, RCU_SCHED_SYNC);
	__init_rwsem(&sem->rw_sem, name, rwsem_key);
	init_waitqueue_head(&sem->writer);
	sem->readers_block = 0;
	return 0;
}
EXPORT_SYMBOL_GPL(__percpu_init_rwsem);

void percpu_free_rwsem(struct percpu_rw_semaphore *sem)
{
	/*
	 * XXX: temporary kludge. The error path in alloc_super()
	 * assumes that percpu_free_rwsem() is safe after kzalloc().
	 */
	if (!sem->read_count)
		return;

	rcu_sync_dtor(&sem->rss);
	free_percpu(sem->read_count);
	sem->read_count = NULL; /* catch use after free bugs */
}
EXPORT_SYMBOL_GPL(percpu_free_rwsem);

int __percpu_down_read(struct percpu_rw_semaphore *sem, int try)
{
	/*
	 * Due to having preemption disabled the decrement happens on
	 * the same CPU as the increment, avoiding the
	 * increment-on-one-CPU-and-decrement-on-another problem.
	 *
	 * If the reader misses the writer's assignment of readers_block, then
	 * the writer is guaranteed to see the reader's increment.
	 *
	 * Conversely, any readers that increment their sem->read_count after
	 * the writer looks are guaranteed to see the readers_block value,
	 * which in turn means that they are guaranteed to immediately
	 * decrement their sem->read_count, so that it doesn't matter that the
	 * writer missed them.
	 */

	smp_mb(); /* A matches D */

	/*
	 * If !readers_block the critical section starts here, matched by the
	 * release in percpu_up_write().
	 */
	if (likely(!smp_load_acquire(&sem->readers_block)))
		return 1;

	/*
	 * Per the above comment; we still have preemption disabled and
	 * will thus decrement on the same CPU as we incremented.
	 */
	__percpu_up_read(sem);

	if (try)
		return 0;

	/*
	 * We either call schedule() in the wait, or we'll fall through
	 * and reschedule on the preempt_enable() in percpu_down_read().
	 */
	preempt_enable_no_resched();

	/*
	 * Avoid lockdep for the down/up_read() we already have them.
	 */
	__down_read(&sem->rw_sem);
	this_cpu_inc(*sem->read_count);
	__up_read(&sem->rw_sem);

	preempt_disable();
	return 1;
}
EXPORT_SYMBOL_GPL(__percpu_down_read);

void __percpu_up_read(struct percpu_rw_semaphore *sem)
{
	smp_mb(); /* B matches C */
	/*
	 * In other words, if they see our decrement (presumably to aggregate
	 * zero, as that is the only time it matters) they will also see our
	 * critical section.
	 */
	__this_cpu_dec(*sem->read_count);

	/* Prod writer to recheck readers_active */
	wake_up(&sem->writer);
}
EXPORT_SYMBOL_GPL(__percpu_up_read);

#define per_cpu_sum(var)						\
({									\
	typeof(var) __sum = 0;						\
	int cpu;							\
	compiletime_assert_atomic_type(__sum);				\
	for_each_possible_cpu(cpu)					\
		__sum += per_cpu(var, cpu);				\
	__sum;								\
})

/*
 * Return true if the modular sum of the sem->read_count per-CPU variable is
 * zero.  If this sum is zero, then it is stable due to the fact that if any
 * newly arriving readers increment a given counter, they will immediately
 * decrement that same counter.
 */
static bool readers_active_check(struct percpu_rw_semaphore *sem)
{
	if (per_cpu_sum(*sem->read_count) != 0)
		return false;

	/*
	 * If we observed the decrement; ensure we see the entire critical
	 * section.
	 */

	smp_mb(); /* C matches B */

	return true;
}

void percpu_down_write(struct percpu_rw_semaphore *sem)
{
	/* Notify readers to take the slow path. */
	rcu_sync_enter(&sem->rss);

	down_write(&sem->rw_sem);

	/*
	 * Notify new readers to block; up until now, and thus throughout the
	 * longish rcu_sync_enter() above, new readers could still come in.
	 */
	WRITE_ONCE(sem->readers_block, 1);

	smp_mb(); /* D matches A */

	/*
	 * If they don't see our writer of readers_block, then we are
	 * guaranteed to see their sem->read_count increment, and therefore
	 * will wait for them.
	 */

	/* Wait for all now active readers to complete. */
	wait_event(sem->writer, readers_active_check(sem));
}
EXPORT_SYMBOL_GPL(percpu_down_write);

void percpu_up_write(struct percpu_rw_semaphore *sem)
{
	/*
	 * Signal the writer is done, no fast path yet.
	 *
	 * One reason that we cannot just immediately flip to readers_fast is
	 * that new readers might fail to see the results of this writer's
	 * critical section.
	 *
	 * Therefore we force it through the slow path which guarantees an
	 * acquire and thereby guarantees the critical section's consistency.
	 */
	smp_store_release(&sem->readers_block, 0);

	/*
	 * Release the write lock, this will allow readers back in the game.
	 */
	up_write(&sem->rw_sem);

	/*
	 * Once this completes (at least one RCU-sched grace period hence) the
	 * reader fast path will be available again. Safe to use outside the
	 * exclusive write lock because its counting.
	 */
	rcu_sync_exit(&sem->rss);
}
EXPORT_SYMBOL_GPL(percpu_up_write);
>>>>>>> v4.9.227
