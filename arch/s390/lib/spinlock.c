/*
 *    Out of line spinlock code.
 *
 *    Copyright IBM Corp. 2004, 2006
 *    Author(s): Martin Schwidefsky (schwidefsky@de.ibm.com)
 */

#include <linux/types.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/smp.h>
#include <asm/io.h>

<<<<<<< HEAD
int spin_retry = 1000;
=======
int spin_retry = -1;

static int __init spin_retry_init(void)
{
	if (spin_retry < 0)
		spin_retry = MACHINE_HAS_CAD ? 10 : 1000;
	return 0;
}
early_initcall(spin_retry_init);
>>>>>>> v4.9.227

/**
 * spin_retry= parameter
 */
static int __init spin_retry_setup(char *str)
{
	spin_retry = simple_strtoul(str, &str, 0);
	return 1;
}
__setup("spin_retry=", spin_retry_setup);

<<<<<<< HEAD
=======
static inline void _raw_compare_and_delay(unsigned int *lock, unsigned int old)
{
	asm(".insn rsy,0xeb0000000022,%0,0,%1" : : "d" (old), "Q" (*lock));
}

static inline int cpu_is_preempted(int cpu)
{
	if (test_cpu_flag_of(CIF_ENABLED_WAIT, cpu))
		return 0;
	if (smp_vcpu_scheduled(cpu))
		return 0;
	return 1;
}

>>>>>>> v4.9.227
void arch_spin_lock_wait(arch_spinlock_t *lp)
{
	unsigned int cpu = SPINLOCK_LOCKVAL;
	unsigned int owner;
<<<<<<< HEAD
	int count;

=======
	int count, first_diag;

	first_diag = 1;
>>>>>>> v4.9.227
	while (1) {
		owner = ACCESS_ONCE(lp->lock);
		/* Try to get the lock if it is free. */
		if (!owner) {
			if (_raw_compare_and_swap(&lp->lock, 0, cpu))
				return;
			continue;
		}
<<<<<<< HEAD
		/* Check if the lock owner is running. */
		if (!smp_vcpu_scheduled(~owner)) {
			smp_yield_cpu(~owner);
=======
		/* First iteration: check if the lock owner is running. */
		if (first_diag && cpu_is_preempted(~owner)) {
			smp_yield_cpu(~owner);
			first_diag = 0;
>>>>>>> v4.9.227
			continue;
		}
		/* Loop for a while on the lock value. */
		count = spin_retry;
		do {
<<<<<<< HEAD
=======
			if (MACHINE_HAS_CAD)
				_raw_compare_and_delay(&lp->lock, owner);
>>>>>>> v4.9.227
			owner = ACCESS_ONCE(lp->lock);
		} while (owner && count-- > 0);
		if (!owner)
			continue;
		/*
		 * For multiple layers of hypervisors, e.g. z/VM + LPAR
<<<<<<< HEAD
		 * yield the CPU if the lock is still unavailable.
		 */
		if (!MACHINE_IS_LPAR)
			smp_yield_cpu(~owner);
=======
		 * yield the CPU unconditionally. For LPAR rely on the
		 * sense running status.
		 */
		if (!MACHINE_IS_LPAR || cpu_is_preempted(~owner)) {
			smp_yield_cpu(~owner);
			first_diag = 0;
		}
>>>>>>> v4.9.227
	}
}
EXPORT_SYMBOL(arch_spin_lock_wait);

void arch_spin_lock_wait_flags(arch_spinlock_t *lp, unsigned long flags)
{
	unsigned int cpu = SPINLOCK_LOCKVAL;
	unsigned int owner;
<<<<<<< HEAD
	int count;

	local_irq_restore(flags);
=======
	int count, first_diag;

	local_irq_restore(flags);
	first_diag = 1;
>>>>>>> v4.9.227
	while (1) {
		owner = ACCESS_ONCE(lp->lock);
		/* Try to get the lock if it is free. */
		if (!owner) {
			local_irq_disable();
			if (_raw_compare_and_swap(&lp->lock, 0, cpu))
				return;
			local_irq_restore(flags);
<<<<<<< HEAD
		}
		/* Check if the lock owner is running. */
		if (!smp_vcpu_scheduled(~owner)) {
			smp_yield_cpu(~owner);
=======
			continue;
		}
		/* Check if the lock owner is running. */
		if (first_diag && cpu_is_preempted(~owner)) {
			smp_yield_cpu(~owner);
			first_diag = 0;
>>>>>>> v4.9.227
			continue;
		}
		/* Loop for a while on the lock value. */
		count = spin_retry;
		do {
<<<<<<< HEAD
=======
			if (MACHINE_HAS_CAD)
				_raw_compare_and_delay(&lp->lock, owner);
>>>>>>> v4.9.227
			owner = ACCESS_ONCE(lp->lock);
		} while (owner && count-- > 0);
		if (!owner)
			continue;
		/*
		 * For multiple layers of hypervisors, e.g. z/VM + LPAR
<<<<<<< HEAD
		 * yield the CPU if the lock is still unavailable.
		 */
		if (!MACHINE_IS_LPAR)
			smp_yield_cpu(~owner);
=======
		 * yield the CPU unconditionally. For LPAR rely on the
		 * sense running status.
		 */
		if (!MACHINE_IS_LPAR || cpu_is_preempted(~owner)) {
			smp_yield_cpu(~owner);
			first_diag = 0;
		}
>>>>>>> v4.9.227
	}
}
EXPORT_SYMBOL(arch_spin_lock_wait_flags);

int arch_spin_trylock_retry(arch_spinlock_t *lp)
{
<<<<<<< HEAD
	int count;

	for (count = spin_retry; count > 0; count--)
		if (arch_spin_trylock_once(lp))
			return 1;
=======
	unsigned int cpu = SPINLOCK_LOCKVAL;
	unsigned int owner;
	int count;

	for (count = spin_retry; count > 0; count--) {
		owner = ACCESS_ONCE(lp->lock);
		/* Try to get the lock if it is free. */
		if (!owner) {
			if (_raw_compare_and_swap(&lp->lock, 0, cpu))
				return 1;
		} else if (MACHINE_HAS_CAD)
			_raw_compare_and_delay(&lp->lock, owner);
	}
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL(arch_spin_trylock_retry);

void _raw_read_lock_wait(arch_rwlock_t *rw)
{
	unsigned int owner, old;
	int count = spin_retry;

#ifdef CONFIG_HAVE_MARCH_Z196_FEATURES
	__RAW_LOCK(&rw->lock, -1, __RAW_OP_ADD);
#endif
	owner = 0;
	while (1) {
		if (count-- <= 0) {
<<<<<<< HEAD
			if (owner && !smp_vcpu_scheduled(~owner))
=======
			if (owner && cpu_is_preempted(~owner))
>>>>>>> v4.9.227
				smp_yield_cpu(~owner);
			count = spin_retry;
		}
		old = ACCESS_ONCE(rw->lock);
		owner = ACCESS_ONCE(rw->owner);
<<<<<<< HEAD
		if ((int) old < 0)
			continue;
=======
		if ((int) old < 0) {
			if (MACHINE_HAS_CAD)
				_raw_compare_and_delay(&rw->lock, old);
			continue;
		}
>>>>>>> v4.9.227
		if (_raw_compare_and_swap(&rw->lock, old, old + 1))
			return;
	}
}
EXPORT_SYMBOL(_raw_read_lock_wait);

int _raw_read_trylock_retry(arch_rwlock_t *rw)
{
	unsigned int old;
	int count = spin_retry;

	while (count-- > 0) {
		old = ACCESS_ONCE(rw->lock);
<<<<<<< HEAD
		if ((int) old < 0)
			continue;
=======
		if ((int) old < 0) {
			if (MACHINE_HAS_CAD)
				_raw_compare_and_delay(&rw->lock, old);
			continue;
		}
>>>>>>> v4.9.227
		if (_raw_compare_and_swap(&rw->lock, old, old + 1))
			return 1;
	}
	return 0;
}
EXPORT_SYMBOL(_raw_read_trylock_retry);

#ifdef CONFIG_HAVE_MARCH_Z196_FEATURES

void _raw_write_lock_wait(arch_rwlock_t *rw, unsigned int prev)
{
	unsigned int owner, old;
	int count = spin_retry;

	owner = 0;
	while (1) {
		if (count-- <= 0) {
<<<<<<< HEAD
			if (owner && !smp_vcpu_scheduled(~owner))
=======
			if (owner && cpu_is_preempted(~owner))
>>>>>>> v4.9.227
				smp_yield_cpu(~owner);
			count = spin_retry;
		}
		old = ACCESS_ONCE(rw->lock);
		owner = ACCESS_ONCE(rw->owner);
<<<<<<< HEAD
		smp_rmb();
=======
		smp_mb();
>>>>>>> v4.9.227
		if ((int) old >= 0) {
			prev = __RAW_LOCK(&rw->lock, 0x80000000, __RAW_OP_OR);
			old = prev;
		}
		if ((old & 0x7fffffff) == 0 && (int) prev >= 0)
			break;
<<<<<<< HEAD
=======
		if (MACHINE_HAS_CAD)
			_raw_compare_and_delay(&rw->lock, old);
>>>>>>> v4.9.227
	}
}
EXPORT_SYMBOL(_raw_write_lock_wait);

#else /* CONFIG_HAVE_MARCH_Z196_FEATURES */

void _raw_write_lock_wait(arch_rwlock_t *rw)
{
	unsigned int owner, old, prev;
	int count = spin_retry;

	prev = 0x80000000;
	owner = 0;
	while (1) {
		if (count-- <= 0) {
<<<<<<< HEAD
			if (owner && !smp_vcpu_scheduled(~owner))
=======
			if (owner && cpu_is_preempted(~owner))
>>>>>>> v4.9.227
				smp_yield_cpu(~owner);
			count = spin_retry;
		}
		old = ACCESS_ONCE(rw->lock);
		owner = ACCESS_ONCE(rw->owner);
		if ((int) old >= 0 &&
		    _raw_compare_and_swap(&rw->lock, old, old | 0x80000000))
			prev = old;
		else
<<<<<<< HEAD
			smp_rmb();
		if ((old & 0x7fffffff) == 0 && (int) prev >= 0)
			break;
=======
			smp_mb();
		if ((old & 0x7fffffff) == 0 && (int) prev >= 0)
			break;
		if (MACHINE_HAS_CAD)
			_raw_compare_and_delay(&rw->lock, old);
>>>>>>> v4.9.227
	}
}
EXPORT_SYMBOL(_raw_write_lock_wait);

#endif /* CONFIG_HAVE_MARCH_Z196_FEATURES */

int _raw_write_trylock_retry(arch_rwlock_t *rw)
{
	unsigned int old;
	int count = spin_retry;

	while (count-- > 0) {
		old = ACCESS_ONCE(rw->lock);
<<<<<<< HEAD
		if (old)
			continue;
=======
		if (old) {
			if (MACHINE_HAS_CAD)
				_raw_compare_and_delay(&rw->lock, old);
			continue;
		}
>>>>>>> v4.9.227
		if (_raw_compare_and_swap(&rw->lock, 0, 0x80000000))
			return 1;
	}
	return 0;
}
EXPORT_SYMBOL(_raw_write_trylock_retry);

void arch_lock_relax(unsigned int cpu)
{
	if (!cpu)
		return;
<<<<<<< HEAD
	if (MACHINE_IS_LPAR && smp_vcpu_scheduled(~cpu))
=======
	if (MACHINE_IS_LPAR && !cpu_is_preempted(~cpu))
>>>>>>> v4.9.227
		return;
	smp_yield_cpu(~cpu);
}
EXPORT_SYMBOL(arch_lock_relax);
