/*
<<<<<<< HEAD
 * Queue read/write lock
=======
 * Queued read/write locks
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * (C) Copyright 2013-2014 Hewlett-Packard Development Company, L.P.
 *
 * Authors: Waiman Long <waiman.long@hp.com>
 */
#include <linux/smp.h>
#include <linux/bug.h>
#include <linux/cpumask.h>
#include <linux/percpu.h>
#include <linux/hardirq.h>
#include <asm/qrwlock.h>

<<<<<<< HEAD
=======
/*
 * This internal data structure is used for optimizing access to some of
 * the subfields within the atomic_t cnts.
 */
struct __qrwlock {
	union {
		atomic_t cnts;
		struct {
#ifdef __LITTLE_ENDIAN
			u8 wmode;	/* Writer mode   */
			u8 rcnts[3];	/* Reader counts */
#else
			u8 rcnts[3];	/* Reader counts */
			u8 wmode;	/* Writer mode   */
#endif
		};
	};
	arch_spinlock_t	lock;
};

>>>>>>> v4.9.227
/**
 * rspin_until_writer_unlock - inc reader count & spin until writer is gone
 * @lock  : Pointer to queue rwlock structure
 * @writer: Current queue rwlock writer status byte
 *
 * In interrupt context or at the head of the queue, the reader will just
 * increment the reader count & wait until the writer releases the lock.
 */
static __always_inline void
rspin_until_writer_unlock(struct qrwlock *lock, u32 cnts)
{
	while ((cnts & _QW_WMASK) == _QW_LOCKED) {
		cpu_relax_lowlatency();
<<<<<<< HEAD
		cnts = smp_load_acquire((u32 *)&lock->cnts);
=======
		cnts = atomic_read_acquire(&lock->cnts);
>>>>>>> v4.9.227
	}
}

/**
<<<<<<< HEAD
 * queue_read_lock_slowpath - acquire read lock of a queue rwlock
 * @lock: Pointer to queue rwlock structure
 */
void queue_read_lock_slowpath(struct qrwlock *lock)
{
	u32 cnts;

=======
 * queued_read_lock_slowpath - acquire read lock of a queue rwlock
 * @lock: Pointer to queue rwlock structure
 * @cnts: Current qrwlock lock value
 */
void queued_read_lock_slowpath(struct qrwlock *lock, u32 cnts)
{
>>>>>>> v4.9.227
	/*
	 * Readers come here when they cannot get the lock without waiting
	 */
	if (unlikely(in_interrupt())) {
		/*
<<<<<<< HEAD
		 * Readers in interrupt context will spin until the lock is
		 * available without waiting in the queue.
		 */
		cnts = smp_load_acquire((u32 *)&lock->cnts);
=======
		 * Readers in interrupt context will get the lock immediately
		 * if the writer is just waiting (not holding the lock yet).
		 * The rspin_until_writer_unlock() function returns immediately
		 * in this case. Otherwise, they will spin (with ACQUIRE
		 * semantics) until the lock is available without waiting in
		 * the queue.
		 */
>>>>>>> v4.9.227
		rspin_until_writer_unlock(lock, cnts);
		return;
	}
	atomic_sub(_QR_BIAS, &lock->cnts);

	/*
	 * Put the reader into the wait queue
	 */
<<<<<<< HEAD
	arch_spin_lock(&lock->lock);

	/*
	 * At the head of the wait queue now, wait until the writer state
	 * goes to 0 and then try to increment the reader count and get
	 * the lock. It is possible that an incoming writer may steal the
	 * lock in the interim, so it is necessary to check the writer byte
	 * to make sure that the write lock isn't taken.
	 */
	while (atomic_read(&lock->cnts) & _QW_WMASK)
		cpu_relax_lowlatency();

	cnts = atomic_add_return(_QR_BIAS, &lock->cnts) - _QR_BIAS;
=======
	arch_spin_lock(&lock->wait_lock);

	/*
	 * The ACQUIRE semantics of the following spinning code ensure
	 * that accesses can't leak upwards out of our subsequent critical
	 * section in the case that the lock is currently held for write.
	 */
	cnts = atomic_fetch_add_acquire(_QR_BIAS, &lock->cnts);
>>>>>>> v4.9.227
	rspin_until_writer_unlock(lock, cnts);

	/*
	 * Signal the next one in queue to become queue head
	 */
<<<<<<< HEAD
	arch_spin_unlock(&lock->lock);
}
EXPORT_SYMBOL(queue_read_lock_slowpath);

/**
 * queue_write_lock_slowpath - acquire write lock of a queue rwlock
 * @lock : Pointer to queue rwlock structure
 */
void queue_write_lock_slowpath(struct qrwlock *lock)
=======
	arch_spin_unlock(&lock->wait_lock);
}
EXPORT_SYMBOL(queued_read_lock_slowpath);

/**
 * queued_write_lock_slowpath - acquire write lock of a queue rwlock
 * @lock : Pointer to queue rwlock structure
 */
void queued_write_lock_slowpath(struct qrwlock *lock)
>>>>>>> v4.9.227
{
	u32 cnts;

	/* Put the writer into the wait queue */
<<<<<<< HEAD
	arch_spin_lock(&lock->lock);

	/* Try to acquire the lock directly if no reader is present */
	if (!atomic_read(&lock->cnts) &&
	    (atomic_cmpxchg(&lock->cnts, 0, _QW_LOCKED) == 0))
=======
	arch_spin_lock(&lock->wait_lock);

	/* Try to acquire the lock directly if no reader is present */
	if (!atomic_read(&lock->cnts) &&
	    (atomic_cmpxchg_acquire(&lock->cnts, 0, _QW_LOCKED) == 0))
>>>>>>> v4.9.227
		goto unlock;

	/*
	 * Set the waiting flag to notify readers that a writer is pending,
	 * or wait for a previous writer to go away.
	 */
	for (;;) {
<<<<<<< HEAD
		cnts = atomic_read(&lock->cnts);
		if (!(cnts & _QW_WMASK) &&
		    (atomic_cmpxchg(&lock->cnts, cnts,
				    cnts | _QW_WAITING) == cnts))
=======
		struct __qrwlock *l = (struct __qrwlock *)lock;

		if (!READ_ONCE(l->wmode) &&
		   (cmpxchg_relaxed(&l->wmode, 0, _QW_WAITING) == 0))
>>>>>>> v4.9.227
			break;

		cpu_relax_lowlatency();
	}

	/* When no more readers, set the locked flag */
	for (;;) {
		cnts = atomic_read(&lock->cnts);
		if ((cnts == _QW_WAITING) &&
<<<<<<< HEAD
		    (atomic_cmpxchg(&lock->cnts, _QW_WAITING,
				    _QW_LOCKED) == _QW_WAITING))
=======
		    (atomic_cmpxchg_acquire(&lock->cnts, _QW_WAITING,
					    _QW_LOCKED) == _QW_WAITING))
>>>>>>> v4.9.227
			break;

		cpu_relax_lowlatency();
	}
unlock:
<<<<<<< HEAD
	arch_spin_unlock(&lock->lock);
}
EXPORT_SYMBOL(queue_write_lock_slowpath);
=======
	arch_spin_unlock(&lock->wait_lock);
}
EXPORT_SYMBOL(queued_write_lock_slowpath);
>>>>>>> v4.9.227
