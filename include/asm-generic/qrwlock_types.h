#ifndef __ASM_GENERIC_QRWLOCK_TYPES_H
#define __ASM_GENERIC_QRWLOCK_TYPES_H

#include <linux/types.h>
#include <asm/spinlock_types.h>

/*
 * The queue read/write lock data structure
 */

typedef struct qrwlock {
	atomic_t		cnts;
<<<<<<< HEAD
	arch_spinlock_t		lock;
=======
	arch_spinlock_t		wait_lock;
>>>>>>> v4.9.227
} arch_rwlock_t;

#define	__ARCH_RW_LOCK_UNLOCKED {		\
	.cnts = ATOMIC_INIT(0),			\
<<<<<<< HEAD
	.lock = __ARCH_SPIN_LOCK_UNLOCKED,	\
=======
	.wait_lock = __ARCH_SPIN_LOCK_UNLOCKED,	\
>>>>>>> v4.9.227
}

#endif /* __ASM_GENERIC_QRWLOCK_TYPES_H */
