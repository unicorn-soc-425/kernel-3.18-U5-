#ifndef _PARISC_TLBFLUSH_H
#define _PARISC_TLBFLUSH_H

/* TLB flushing routines.... */

#include <linux/mm.h>
#include <linux/sched.h>
#include <asm/mmu_context.h>


/* This is for the serialisation of PxTLB broadcasts.  At least on the
 * N class systems, only one PxTLB inter processor broadcast can be
 * active at any one time on the Merced bus.  This tlb purge
 * synchronisation is fairly lightweight and harmless so we activate
 * it on all systems not just the N class.
<<<<<<< HEAD
=======

 * It is also used to ensure PTE updates are atomic and consistent
 * with the TLB.
>>>>>>> v4.9.227
 */
extern spinlock_t pa_tlb_lock;

#define purge_tlb_start(flags)	spin_lock_irqsave(&pa_tlb_lock, flags)
#define purge_tlb_end(flags)	spin_unlock_irqrestore(&pa_tlb_lock, flags)

extern void flush_tlb_all(void);
extern void flush_tlb_all_local(void *);

#define smp_flush_tlb_all()	flush_tlb_all()

<<<<<<< HEAD
/*
 * flush_tlb_mm()
 *
 * XXX This code is NOT valid for HP-UX compatibility processes,
 * (although it will probably work 99% of the time). HP-UX
 * processes are free to play with the space id's and save them
 * over long periods of time, etc. so we have to preserve the
 * space and just flush the entire tlb. We need to check the
 * personality in order to do that, but the personality is not
 * currently being set correctly.
 *
 * Of course, Linux processes could do the same thing, but
 * we don't support that (and the compilers, dynamic linker,
 * etc. do not do that).
=======
int __flush_tlb_range(unsigned long sid,
	unsigned long start, unsigned long end);

#define flush_tlb_range(vma, start, end) \
	__flush_tlb_range((vma)->vm_mm->context, start, end)

#define flush_tlb_kernel_range(start, end) \
	__flush_tlb_range(0, start, end)

/*
 * flush_tlb_mm()
 *
 * The code to switch to a new context is NOT valid for processes
 * which play with the space id's.  Thus, we have to preserve the
 * space and just flush the entire tlb.  However, the compilers,
 * dynamic linker, etc, do not manipulate space id's, so there
 * could be a significant performance benefit in switching contexts
 * and not flushing the whole tlb.
>>>>>>> v4.9.227
 */

static inline void flush_tlb_mm(struct mm_struct *mm)
{
	BUG_ON(mm == &init_mm); /* Should never happen */

#if 1 || defined(CONFIG_SMP)
<<<<<<< HEAD
	flush_tlb_all();
#else
	/* FIXME: currently broken, causing space id and protection ids
	 *  to go out of sync, resulting in faults on userspace accesses.
=======
	/* Except for very small threads, flushing the whole TLB is
	 * faster than using __flush_tlb_range.  The pdtlb and pitlb
	 * instructions are very slow because of the TLB broadcast.
	 * It might be faster to do local range flushes on all CPUs
	 * on PA 2.0 systems.
	 */
	flush_tlb_all();
#else
	/* FIXME: currently broken, causing space id and protection ids
	 * to go out of sync, resulting in faults on userspace accesses.
	 * This approach needs further investigation since running many
	 * small applications (e.g., GCC testsuite) is faster on HP-UX.
>>>>>>> v4.9.227
	 */
	if (mm) {
		if (mm->context != 0)
			free_sid(mm->context);
		mm->context = alloc_sid();
		if (mm == current->active_mm)
			load_context(mm->context);
	}
#endif
}

static inline void flush_tlb_page(struct vm_area_struct *vma,
	unsigned long addr)
{
	unsigned long flags, sid;

<<<<<<< HEAD
	/* For one page, it's not worth testing the split_tlb variable */

	mb();
=======
>>>>>>> v4.9.227
	sid = vma->vm_mm->context;
	purge_tlb_start(flags);
	mtsp(sid, 1);
	pdtlb(addr);
<<<<<<< HEAD
	pitlb(addr);
	purge_tlb_end(flags);
}

void __flush_tlb_range(unsigned long sid,
	unsigned long start, unsigned long end);

#define flush_tlb_range(vma,start,end) __flush_tlb_range((vma)->vm_mm->context,start,end)

#define flush_tlb_kernel_range(start, end) __flush_tlb_range(0,start,end)

=======
	if (unlikely(split_tlb))
		pitlb(addr);
	purge_tlb_end(flags);
}
>>>>>>> v4.9.227
#endif
