#ifndef _ASM_X86_MMU_CONTEXT_H
#define _ASM_X86_MMU_CONTEXT_H

#include <asm/desc.h>
#include <linux/atomic.h>
#include <linux/mm_types.h>
<<<<<<< HEAD
=======
#include <linux/pkeys.h>
>>>>>>> v4.9.227

#include <trace/events/tlb.h>

#include <asm/pgalloc.h>
#include <asm/tlbflush.h>
#include <asm/paravirt.h>
<<<<<<< HEAD
#ifndef CONFIG_PARAVIRT
#include <asm-generic/mm_hooks.h>

=======
#include <asm/mpx.h>

extern atomic64_t last_mm_ctx_id;

#ifndef CONFIG_PARAVIRT
>>>>>>> v4.9.227
static inline void paravirt_activate_mm(struct mm_struct *prev,
					struct mm_struct *next)
{
}
#endif	/* !CONFIG_PARAVIRT */

<<<<<<< HEAD
=======
#ifdef CONFIG_PERF_EVENTS
extern struct static_key rdpmc_always_available;

static inline void load_mm_cr4(struct mm_struct *mm)
{
	if (static_key_false(&rdpmc_always_available) ||
	    atomic_read(&mm->context.perf_rdpmc_allowed))
		cr4_set_bits(X86_CR4_PCE);
	else
		cr4_clear_bits(X86_CR4_PCE);
}
#else
static inline void load_mm_cr4(struct mm_struct *mm) {}
#endif

#ifdef CONFIG_MODIFY_LDT_SYSCALL
>>>>>>> v4.9.227
/*
 * ldt_structs can be allocated, used, and freed, but they are never
 * modified while live.
 */
struct ldt_struct {
	/*
	 * Xen requires page-aligned LDTs with special permissions.  This is
	 * needed to prevent us from installing evil descriptors such as
	 * call gates.  On native, we could merge the ldt_struct and LDT
	 * allocations, but it's not worth trying to optimize.
	 */
	struct desc_struct *entries;
	int size;
};

<<<<<<< HEAD
static inline void load_mm_ldt(struct mm_struct *mm)
{
=======
/*
 * Used for LDT copy/destruction.
 */
int init_new_context_ldt(struct task_struct *tsk, struct mm_struct *mm);
void destroy_context_ldt(struct mm_struct *mm);
#else	/* CONFIG_MODIFY_LDT_SYSCALL */
static inline int init_new_context_ldt(struct task_struct *tsk,
				       struct mm_struct *mm)
{
	return 0;
}
static inline void destroy_context_ldt(struct mm_struct *mm) {}
#endif

static inline void load_mm_ldt(struct mm_struct *mm)
{
#ifdef CONFIG_MODIFY_LDT_SYSCALL
>>>>>>> v4.9.227
	struct ldt_struct *ldt;

	/* lockless_dereference synchronizes with smp_store_release */
	ldt = lockless_dereference(mm->context.ldt);

	/*
	 * Any change to mm->context.ldt is followed by an IPI to all
	 * CPUs with the mm active.  The LDT will not be freed until
	 * after the IPI is handled by all such CPUs.  This means that,
	 * if the ldt_struct changes before we return, the values we see
	 * will be safe, and the new values will be loaded before we run
	 * any user code.
	 *
	 * NB: don't try to convert this to use RCU without extreme care.
	 * We would still need IRQs off, because we don't want to change
	 * the local LDT after an IPI loaded a newer value than the one
	 * that we can see.
	 */

	if (unlikely(ldt))
		set_ldt(ldt->entries, ldt->size);
	else
		clear_LDT();
<<<<<<< HEAD
=======
#else
	clear_LDT();
#endif
>>>>>>> v4.9.227

	DEBUG_LOCKS_WARN_ON(preemptible());
}

<<<<<<< HEAD
/*
 * Used for LDT copy/destruction.
 */
int init_new_context(struct task_struct *tsk, struct mm_struct *mm);
void destroy_context(struct mm_struct *mm);


static inline void enter_lazy_tlb(struct mm_struct *mm, struct task_struct *tsk)
{
#ifdef CONFIG_SMP
	if (this_cpu_read(cpu_tlbstate.state) == TLBSTATE_OK)
		this_cpu_write(cpu_tlbstate.state, TLBSTATE_LAZY);
#endif
}

static inline void switch_mm(struct mm_struct *prev, struct mm_struct *next,
			     struct task_struct *tsk)
{
	unsigned cpu = smp_processor_id();

	if (likely(prev != next)) {
#ifdef CONFIG_SMP
		this_cpu_write(cpu_tlbstate.state, TLBSTATE_OK);
		this_cpu_write(cpu_tlbstate.active_mm, next);
#endif
		cpumask_set_cpu(cpu, mm_cpumask(next));

		/* Re-load page tables */
		load_cr3(next->pgd);
		trace_tlb_flush(TLB_FLUSH_ON_TASK_SWITCH, TLB_FLUSH_ALL);

		/* Stop flush ipis for the previous mm */
		cpumask_clear_cpu(cpu, mm_cpumask(prev));

		/* Load the LDT, if the LDT is different: */
		if (unlikely(prev->context.ldt != next->context.ldt))
			load_mm_ldt(next);
	}
#ifdef CONFIG_SMP
	  else {
		this_cpu_write(cpu_tlbstate.state, TLBSTATE_OK);
		BUG_ON(this_cpu_read(cpu_tlbstate.active_mm) != next);

		if (!cpumask_test_cpu(cpu, mm_cpumask(next))) {
			/*
			 * On established mms, the mm_cpumask is only changed
			 * from irq context, from ptep_clear_flush() while in
			 * lazy tlb mode, and here. Irqs are blocked during
			 * schedule, protecting us from simultaneous changes.
			 */
			cpumask_set_cpu(cpu, mm_cpumask(next));
			/*
			 * We were in lazy tlb mode and leave_mm disabled
			 * tlb flush IPI delivery. We must reload CR3
			 * to make sure to use no freed page tables.
			 */
			load_cr3(next->pgd);
			trace_tlb_flush(TLB_FLUSH_ON_TASK_SWITCH, TLB_FLUSH_ALL);
			load_mm_ldt(next);
		}
	}
#endif
}
=======
static inline void enter_lazy_tlb(struct mm_struct *mm, struct task_struct *tsk)
{
	if (this_cpu_read(cpu_tlbstate.state) == TLBSTATE_OK)
		this_cpu_write(cpu_tlbstate.state, TLBSTATE_LAZY);
}

static inline int init_new_context(struct task_struct *tsk,
				   struct mm_struct *mm)
{
	mm->context.ctx_id = atomic64_inc_return(&last_mm_ctx_id);

	#ifdef CONFIG_X86_INTEL_MEMORY_PROTECTION_KEYS
	if (cpu_feature_enabled(X86_FEATURE_OSPKE)) {
		/* pkey 0 is the default and allocated implicitly */
		mm->context.pkey_allocation_map = 0x1;
		/* -1 means unallocated or invalid */
		mm->context.execute_only_pkey = -1;
	}
	#endif
	return init_new_context_ldt(tsk, mm);
}
static inline void destroy_context(struct mm_struct *mm)
{
	destroy_context_ldt(mm);
}

extern void switch_mm(struct mm_struct *prev, struct mm_struct *next,
		      struct task_struct *tsk);

extern void switch_mm_irqs_off(struct mm_struct *prev, struct mm_struct *next,
			       struct task_struct *tsk);
#define switch_mm_irqs_off switch_mm_irqs_off
>>>>>>> v4.9.227

#define activate_mm(prev, next)			\
do {						\
	paravirt_activate_mm((prev), (next));	\
	switch_mm((prev), (next), NULL);	\
} while (0);

#ifdef CONFIG_X86_32
#define deactivate_mm(tsk, mm)			\
do {						\
	lazy_load_gs(0);			\
} while (0)
#else
#define deactivate_mm(tsk, mm)			\
do {						\
	load_gs_index(0);			\
	loadsegment(fs, 0);			\
} while (0)
#endif

<<<<<<< HEAD
=======
static inline void arch_dup_mmap(struct mm_struct *oldmm,
				 struct mm_struct *mm)
{
	paravirt_arch_dup_mmap(oldmm, mm);
}

static inline void arch_exit_mmap(struct mm_struct *mm)
{
	paravirt_arch_exit_mmap(mm);
}

#ifdef CONFIG_X86_64
static inline bool is_64bit_mm(struct mm_struct *mm)
{
	return	!IS_ENABLED(CONFIG_IA32_EMULATION) ||
		!(mm->context.ia32_compat == TIF_IA32);
}
#else
static inline bool is_64bit_mm(struct mm_struct *mm)
{
	return false;
}
#endif

static inline void arch_bprm_mm_init(struct mm_struct *mm,
		struct vm_area_struct *vma)
{
	mpx_mm_init(mm);
}

static inline void arch_unmap(struct mm_struct *mm, struct vm_area_struct *vma,
			      unsigned long start, unsigned long end)
{
	/*
	 * mpx_notify_unmap() goes and reads a rarely-hot
	 * cacheline in the mm_struct.  That can be expensive
	 * enough to be seen in profiles.
	 *
	 * The mpx_notify_unmap() call and its contents have been
	 * observed to affect munmap() performance on hardware
	 * where MPX is not present.
	 *
	 * The unlikely() optimizes for the fast case: no MPX
	 * in the CPU, or no MPX use in the process.  Even if
	 * we get this wrong (in the unlikely event that MPX
	 * is widely enabled on some system) the overhead of
	 * MPX itself (reading bounds tables) is expected to
	 * overwhelm the overhead of getting this unlikely()
	 * consistently wrong.
	 */
	if (unlikely(cpu_feature_enabled(X86_FEATURE_MPX)))
		mpx_notify_unmap(mm, vma, start, end);
}

#ifdef CONFIG_X86_INTEL_MEMORY_PROTECTION_KEYS
static inline int vma_pkey(struct vm_area_struct *vma)
{
	unsigned long vma_pkey_mask = VM_PKEY_BIT0 | VM_PKEY_BIT1 |
				      VM_PKEY_BIT2 | VM_PKEY_BIT3;

	return (vma->vm_flags & vma_pkey_mask) >> VM_PKEY_SHIFT;
}
#else
static inline int vma_pkey(struct vm_area_struct *vma)
{
	return 0;
}
#endif

static inline bool __pkru_allows_pkey(u16 pkey, bool write)
{
	u32 pkru = read_pkru();

	if (!__pkru_allows_read(pkru, pkey))
		return false;
	if (write && !__pkru_allows_write(pkru, pkey))
		return false;

	return true;
}

/*
 * We only want to enforce protection keys on the current process
 * because we effectively have no access to PKRU for other
 * processes or any way to tell *which * PKRU in a threaded
 * process we could use.
 *
 * So do not enforce things if the VMA is not from the current
 * mm, or if we are in a kernel thread.
 */
static inline bool vma_is_foreign(struct vm_area_struct *vma)
{
	if (!current->mm)
		return true;
	/*
	 * Should PKRU be enforced on the access to this VMA?  If
	 * the VMA is from another process, then PKRU has no
	 * relevance and should not be enforced.
	 */
	if (current->mm != vma->vm_mm)
		return true;

	return false;
}

static inline bool arch_vma_access_permitted(struct vm_area_struct *vma,
		bool write, bool execute, bool foreign)
{
	/* pkeys never affect instruction fetches */
	if (execute)
		return true;
	/* allow access if the VMA is not one from this process */
	if (foreign || vma_is_foreign(vma))
		return true;
	return __pkru_allows_pkey(vma_pkey(vma), write);
}

static inline bool arch_pte_access_permitted(pte_t pte, bool write)
{
	return __pkru_allows_pkey(pte_flags_pkey(pte_flags(pte)), write);
}
>>>>>>> v4.9.227
#endif /* _ASM_X86_MMU_CONTEXT_H */
