/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * Copyright 2010-2011 Paul Mackerras, IBM Corp. <paulus@au1.ibm.com>
 */

#include <linux/types.h>
#include <linux/string.h>
#include <linux/kvm.h>
#include <linux/kvm_host.h>
#include <linux/hugetlb.h>
#include <linux/module.h>
<<<<<<< HEAD
=======
#include <linux/log2.h>
>>>>>>> v4.9.227

#include <asm/tlbflush.h>
#include <asm/kvm_ppc.h>
#include <asm/kvm_book3s.h>
<<<<<<< HEAD
#include <asm/mmu-hash64.h>
=======
#include <asm/book3s/64/mmu-hash.h>
>>>>>>> v4.9.227
#include <asm/hvcall.h>
#include <asm/synch.h>
#include <asm/ppc-opcode.h>

/* Translate address of a vmalloc'd thing to a linear map address */
static void *real_vmalloc_addr(void *x)
{
	unsigned long addr = (unsigned long) x;
	pte_t *p;
<<<<<<< HEAD

	p = find_linux_pte_or_hugepte(swapper_pg_dir, addr, NULL);
	if (!p || !pte_present(*p))
		return NULL;
	/* assume we don't have huge pages in vmalloc space... */
=======
	/*
	 * assume we don't have huge pages in vmalloc space...
	 * So don't worry about THP collapse/split. Called
	 * Only in realmode, hence won't need irq_save/restore.
	 */
	p = __find_linux_pte_or_hugepte(swapper_pg_dir, addr, NULL, NULL);
	if (!p || !pte_present(*p))
		return NULL;
>>>>>>> v4.9.227
	addr = (pte_pfn(*p) << PAGE_SHIFT) | (addr & ~PAGE_MASK);
	return __va(addr);
}

/* Return 1 if we need to do a global tlbie, 0 if we can use tlbiel */
static int global_invalidates(struct kvm *kvm, unsigned long flags)
{
	int global;

	/*
	 * If there is only one vcore, and it's currently running,
	 * as indicated by local_paca->kvm_hstate.kvm_vcpu being set,
	 * we can use tlbiel as long as we mark all other physical
	 * cores as potentially having stale TLB entries for this lpid.
<<<<<<< HEAD
	 * If we're not using MMU notifiers, we never take pages away
	 * from the guest, so we can use tlbiel if requested.
=======
>>>>>>> v4.9.227
	 * Otherwise, don't use tlbiel.
	 */
	if (kvm->arch.online_vcores == 1 && local_paca->kvm_hstate.kvm_vcpu)
		global = 0;
<<<<<<< HEAD
	else if (kvm->arch.using_mmu_notifiers)
		global = 1;
	else
		global = !(flags & H_LOCAL);
=======
	else
		global = 1;
>>>>>>> v4.9.227

	if (!global) {
		/* any other core might now have stale TLB entries... */
		smp_wmb();
		cpumask_setall(&kvm->arch.need_tlb_flush);
		cpumask_clear_cpu(local_paca->kvm_hstate.kvm_vcore->pcpu,
				  &kvm->arch.need_tlb_flush);
	}

	return global;
}

/*
 * Add this HPTE into the chain for the real page.
 * Must be called with the chain locked; it unlocks the chain.
 */
void kvmppc_add_revmap_chain(struct kvm *kvm, struct revmap_entry *rev,
			     unsigned long *rmap, long pte_index, int realmode)
{
	struct revmap_entry *head, *tail;
	unsigned long i;

	if (*rmap & KVMPPC_RMAP_PRESENT) {
		i = *rmap & KVMPPC_RMAP_INDEX;
		head = &kvm->arch.revmap[i];
		if (realmode)
			head = real_vmalloc_addr(head);
		tail = &kvm->arch.revmap[head->back];
		if (realmode)
			tail = real_vmalloc_addr(tail);
		rev->forw = i;
		rev->back = head->back;
		tail->forw = pte_index;
		head->back = pte_index;
	} else {
		rev->forw = rev->back = pte_index;
		*rmap = (*rmap & ~KVMPPC_RMAP_INDEX) |
			pte_index | KVMPPC_RMAP_PRESENT;
	}
	unlock_rmap(rmap);
}
EXPORT_SYMBOL_GPL(kvmppc_add_revmap_chain);

<<<<<<< HEAD
=======
/* Update the changed page order field of an rmap entry */
void kvmppc_update_rmap_change(unsigned long *rmap, unsigned long psize)
{
	unsigned long order;

	if (!psize)
		return;
	order = ilog2(psize);
	order <<= KVMPPC_RMAP_CHG_SHIFT;
	if (order > (*rmap & KVMPPC_RMAP_CHG_ORDER))
		*rmap = (*rmap & ~KVMPPC_RMAP_CHG_ORDER) | order;
}
EXPORT_SYMBOL_GPL(kvmppc_update_rmap_change);

/* Returns a pointer to the revmap entry for the page mapped by a HPTE */
static unsigned long *revmap_for_hpte(struct kvm *kvm, unsigned long hpte_v,
				      unsigned long hpte_gr)
{
	struct kvm_memory_slot *memslot;
	unsigned long *rmap;
	unsigned long gfn;

	gfn = hpte_rpn(hpte_gr, hpte_page_size(hpte_v, hpte_gr));
	memslot = __gfn_to_memslot(kvm_memslots_raw(kvm), gfn);
	if (!memslot)
		return NULL;

	rmap = real_vmalloc_addr(&memslot->arch.rmap[gfn - memslot->base_gfn]);
	return rmap;
}

>>>>>>> v4.9.227
/* Remove this HPTE from the chain for a real page */
static void remove_revmap_chain(struct kvm *kvm, long pte_index,
				struct revmap_entry *rev,
				unsigned long hpte_v, unsigned long hpte_r)
{
	struct revmap_entry *next, *prev;
<<<<<<< HEAD
	unsigned long gfn, ptel, head;
	struct kvm_memory_slot *memslot;
=======
	unsigned long ptel, head;
>>>>>>> v4.9.227
	unsigned long *rmap;
	unsigned long rcbits;

	rcbits = hpte_r & (HPTE_R_R | HPTE_R_C);
	ptel = rev->guest_rpte |= rcbits;
<<<<<<< HEAD
	gfn = hpte_rpn(ptel, hpte_page_size(hpte_v, ptel));
	memslot = __gfn_to_memslot(kvm_memslots_raw(kvm), gfn);
	if (!memslot)
		return;

	rmap = real_vmalloc_addr(&memslot->arch.rmap[gfn - memslot->base_gfn]);
=======
	rmap = revmap_for_hpte(kvm, hpte_v, ptel);
	if (!rmap)
		return;
>>>>>>> v4.9.227
	lock_rmap(rmap);

	head = *rmap & KVMPPC_RMAP_INDEX;
	next = real_vmalloc_addr(&kvm->arch.revmap[rev->forw]);
	prev = real_vmalloc_addr(&kvm->arch.revmap[rev->back]);
	next->back = rev->back;
	prev->forw = rev->forw;
	if (head == pte_index) {
		head = rev->forw;
		if (head == pte_index)
			*rmap &= ~(KVMPPC_RMAP_PRESENT | KVMPPC_RMAP_INDEX);
		else
			*rmap = (*rmap & ~KVMPPC_RMAP_INDEX) | head;
	}
	*rmap |= rcbits << KVMPPC_RMAP_RC_SHIFT;
<<<<<<< HEAD
	unlock_rmap(rmap);
}

static pte_t lookup_linux_pte_and_update(pgd_t *pgdir, unsigned long hva,
			      int writing, unsigned long *pte_sizep)
{
	pte_t *ptep;
	unsigned long ps = *pte_sizep;
	unsigned int hugepage_shift;

	ptep = find_linux_pte_or_hugepte(pgdir, hva, &hugepage_shift);
	if (!ptep)
		return __pte(0);
	if (hugepage_shift)
		*pte_sizep = 1ul << hugepage_shift;
	else
		*pte_sizep = PAGE_SIZE;
	if (ps > *pte_sizep)
		return __pte(0);
	return kvmppc_read_update_linux_pte(ptep, writing, hugepage_shift);
}

static inline void unlock_hpte(__be64 *hpte, unsigned long hpte_v)
{
	asm volatile(PPC_RELEASE_BARRIER "" : : : "memory");
	hpte[0] = cpu_to_be64(hpte_v);
}

=======
	if (rcbits & HPTE_R_C)
		kvmppc_update_rmap_change(rmap, hpte_page_size(hpte_v, hpte_r));
	unlock_rmap(rmap);
}

>>>>>>> v4.9.227
long kvmppc_do_h_enter(struct kvm *kvm, unsigned long flags,
		       long pte_index, unsigned long pteh, unsigned long ptel,
		       pgd_t *pgdir, bool realmode, unsigned long *pte_idx_ret)
{
	unsigned long i, pa, gpa, gfn, psize;
	unsigned long slot_fn, hva;
	__be64 *hpte;
	struct revmap_entry *rev;
	unsigned long g_ptel;
	struct kvm_memory_slot *memslot;
<<<<<<< HEAD
	unsigned long *physp, pte_size;
	unsigned long is_io;
	unsigned long *rmap;
	pte_t pte;
	unsigned int writing;
	unsigned long mmu_seq;
	unsigned long rcbits;
=======
	unsigned hpage_shift;
	bool is_ci;
	unsigned long *rmap;
	pte_t *ptep;
	unsigned int writing;
	unsigned long mmu_seq;
	unsigned long rcbits, irq_flags = 0;
>>>>>>> v4.9.227

	psize = hpte_page_size(pteh, ptel);
	if (!psize)
		return H_PARAMETER;
	writing = hpte_is_writable(ptel);
	pteh &= ~(HPTE_V_HVLOCK | HPTE_V_ABSENT | HPTE_V_VALID);
	ptel &= ~HPTE_GR_RESERVED;
	g_ptel = ptel;

	/* used later to detect if we might have been invalidated */
	mmu_seq = kvm->mmu_notifier_seq;
	smp_rmb();

	/* Find the memslot (if any) for this address */
	gpa = (ptel & HPTE_R_RPN) & ~(psize - 1);
	gfn = gpa >> PAGE_SHIFT;
	memslot = __gfn_to_memslot(kvm_memslots_raw(kvm), gfn);
	pa = 0;
<<<<<<< HEAD
	is_io = ~0ul;
	rmap = NULL;
	if (!(memslot && !(memslot->flags & KVM_MEMSLOT_INVALID))) {
		/* PPC970 can't do emulated MMIO */
		if (!cpu_has_feature(CPU_FTR_ARCH_206))
			return H_PARAMETER;
=======
	is_ci = false;
	rmap = NULL;
	if (!(memslot && !(memslot->flags & KVM_MEMSLOT_INVALID))) {
>>>>>>> v4.9.227
		/* Emulated MMIO - mark this with key=31 */
		pteh |= HPTE_V_ABSENT;
		ptel |= HPTE_R_KEY_HI | HPTE_R_KEY_LO;
		goto do_insert;
	}

	/* Check if the requested page fits entirely in the memslot. */
	if (!slot_is_aligned(memslot, psize))
		return H_PARAMETER;
	slot_fn = gfn - memslot->base_gfn;
	rmap = &memslot->arch.rmap[slot_fn];

<<<<<<< HEAD
	if (!kvm->arch.using_mmu_notifiers) {
		physp = memslot->arch.slot_phys;
		if (!physp)
			return H_PARAMETER;
		physp += slot_fn;
		if (realmode)
			physp = real_vmalloc_addr(physp);
		pa = *physp;
		if (!pa)
			return H_TOO_HARD;
		is_io = pa & (HPTE_R_I | HPTE_R_W);
		pte_size = PAGE_SIZE << (pa & KVMPPC_PAGE_ORDER_MASK);
		pa &= PAGE_MASK;
		pa |= gpa & ~PAGE_MASK;
	} else {
		/* Translate to host virtual address */
		hva = __gfn_to_hva_memslot(memslot, gfn);

		/* Look up the Linux PTE for the backing page */
		pte_size = psize;
		pte = lookup_linux_pte_and_update(pgdir, hva, writing,
						  &pte_size);
		if (pte_present(pte) && !pte_numa(pte)) {
			if (writing && !pte_write(pte))
				/* make the actual HPTE be read-only */
				ptel = hpte_make_readonly(ptel);
			is_io = hpte_cache_bits(pte_val(pte));
			pa = pte_pfn(pte) << PAGE_SHIFT;
			pa |= hva & (pte_size - 1);
			pa |= gpa & ~PAGE_MASK;
		}
	}

	if (pte_size < psize)
		return H_PARAMETER;
=======
	/* Translate to host virtual address */
	hva = __gfn_to_hva_memslot(memslot, gfn);
	/*
	 * If we had a page table table change after lookup, we would
	 * retry via mmu_notifier_retry.
	 */
	if (realmode)
		ptep = __find_linux_pte_or_hugepte(pgdir, hva, NULL,
						   &hpage_shift);
	else {
		local_irq_save(irq_flags);
		ptep = find_linux_pte_or_hugepte(pgdir, hva, NULL,
						 &hpage_shift);
	}
	if (ptep) {
		pte_t pte;
		unsigned int host_pte_size;

		if (hpage_shift)
			host_pte_size = 1ul << hpage_shift;
		else
			host_pte_size = PAGE_SIZE;
		/*
		 * We should always find the guest page size
		 * to <= host page size, if host is using hugepage
		 */
		if (host_pte_size < psize) {
			if (!realmode)
				local_irq_restore(flags);
			return H_PARAMETER;
		}
		pte = kvmppc_read_update_linux_pte(ptep, writing);
		if (pte_present(pte) && !pte_protnone(pte)) {
			if (writing && !pte_write(pte))
				/* make the actual HPTE be read-only */
				ptel = hpte_make_readonly(ptel);
			is_ci = pte_ci(pte);
			pa = pte_pfn(pte) << PAGE_SHIFT;
			pa |= hva & (host_pte_size - 1);
			pa |= gpa & ~PAGE_MASK;
		}
	}
	if (!realmode)
		local_irq_restore(irq_flags);
>>>>>>> v4.9.227

	ptel &= ~(HPTE_R_PP0 - psize);
	ptel |= pa;

	if (pa)
		pteh |= HPTE_V_VALID;
	else
		pteh |= HPTE_V_ABSENT;

<<<<<<< HEAD
	/* Check WIMG */
	if (is_io != ~0ul && !hpte_cache_flags_ok(ptel, is_io)) {
		if (is_io)
=======
	/*If we had host pte mapping then  Check WIMG */
	if (ptep && !hpte_cache_flags_ok(ptel, is_ci)) {
		if (is_ci)
>>>>>>> v4.9.227
			return H_PARAMETER;
		/*
		 * Allow guest to map emulated device memory as
		 * uncacheable, but actually make it cacheable.
		 */
		ptel &= ~(HPTE_R_W|HPTE_R_I|HPTE_R_G);
		ptel |= HPTE_R_M;
	}

	/* Find and lock the HPTEG slot to use */
 do_insert:
	if (pte_index >= kvm->arch.hpt_npte)
		return H_PARAMETER;
	if (likely((flags & H_EXACT) == 0)) {
		pte_index &= ~7UL;
		hpte = (__be64 *)(kvm->arch.hpt_virt + (pte_index << 4));
		for (i = 0; i < 8; ++i) {
			if ((be64_to_cpu(*hpte) & HPTE_V_VALID) == 0 &&
			    try_lock_hpte(hpte, HPTE_V_HVLOCK | HPTE_V_VALID |
					  HPTE_V_ABSENT))
				break;
			hpte += 2;
		}
		if (i == 8) {
			/*
			 * Since try_lock_hpte doesn't retry (not even stdcx.
			 * failures), it could be that there is a free slot
			 * but we transiently failed to lock it.  Try again,
			 * actually locking each slot and checking it.
			 */
			hpte -= 16;
			for (i = 0; i < 8; ++i) {
				u64 pte;
				while (!try_lock_hpte(hpte, HPTE_V_HVLOCK))
					cpu_relax();
<<<<<<< HEAD
				pte = be64_to_cpu(*hpte);
				if (!(pte & (HPTE_V_VALID | HPTE_V_ABSENT)))
					break;
				*hpte &= ~cpu_to_be64(HPTE_V_HVLOCK);
=======
				pte = be64_to_cpu(hpte[0]);
				if (!(pte & (HPTE_V_VALID | HPTE_V_ABSENT)))
					break;
				__unlock_hpte(hpte, pte);
>>>>>>> v4.9.227
				hpte += 2;
			}
			if (i == 8)
				return H_PTEG_FULL;
		}
		pte_index += i;
	} else {
		hpte = (__be64 *)(kvm->arch.hpt_virt + (pte_index << 4));
		if (!try_lock_hpte(hpte, HPTE_V_HVLOCK | HPTE_V_VALID |
				   HPTE_V_ABSENT)) {
			/* Lock the slot and check again */
			u64 pte;

			while (!try_lock_hpte(hpte, HPTE_V_HVLOCK))
				cpu_relax();
<<<<<<< HEAD
			pte = be64_to_cpu(*hpte);
			if (pte & (HPTE_V_VALID | HPTE_V_ABSENT)) {
				*hpte &= ~cpu_to_be64(HPTE_V_HVLOCK);
=======
			pte = be64_to_cpu(hpte[0]);
			if (pte & (HPTE_V_VALID | HPTE_V_ABSENT)) {
				__unlock_hpte(hpte, pte);
>>>>>>> v4.9.227
				return H_PTEG_FULL;
			}
		}
	}

	/* Save away the guest's idea of the second HPTE dword */
	rev = &kvm->arch.revmap[pte_index];
	if (realmode)
		rev = real_vmalloc_addr(rev);
	if (rev) {
		rev->guest_rpte = g_ptel;
		note_hpte_modification(kvm, rev);
	}

	/* Link HPTE into reverse-map chain */
	if (pteh & HPTE_V_VALID) {
		if (realmode)
			rmap = real_vmalloc_addr(rmap);
		lock_rmap(rmap);
		/* Check for pending invalidations under the rmap chain lock */
<<<<<<< HEAD
		if (kvm->arch.using_mmu_notifiers &&
		    mmu_notifier_retry(kvm, mmu_seq)) {
=======
		if (mmu_notifier_retry(kvm, mmu_seq)) {
>>>>>>> v4.9.227
			/* inval in progress, write a non-present HPTE */
			pteh |= HPTE_V_ABSENT;
			pteh &= ~HPTE_V_VALID;
			unlock_rmap(rmap);
		} else {
			kvmppc_add_revmap_chain(kvm, rev, rmap, pte_index,
						realmode);
			/* Only set R/C in real HPTE if already set in *rmap */
			rcbits = *rmap >> KVMPPC_RMAP_RC_SHIFT;
			ptel &= rcbits | ~(HPTE_R_R | HPTE_R_C);
		}
	}

	hpte[1] = cpu_to_be64(ptel);

	/* Write the first HPTE dword, unlocking the HPTE and making it valid */
	eieio();
<<<<<<< HEAD
	hpte[0] = cpu_to_be64(pteh);
=======
	__unlock_hpte(hpte, pteh);
>>>>>>> v4.9.227
	asm volatile("ptesync" : : : "memory");

	*pte_idx_ret = pte_index;
	return H_SUCCESS;
}
EXPORT_SYMBOL_GPL(kvmppc_do_h_enter);

long kvmppc_h_enter(struct kvm_vcpu *vcpu, unsigned long flags,
		    long pte_index, unsigned long pteh, unsigned long ptel)
{
	return kvmppc_do_h_enter(vcpu->kvm, flags, pte_index, pteh, ptel,
				 vcpu->arch.pgdir, true, &vcpu->arch.gpr[4]);
}

#ifdef __BIG_ENDIAN__
#define LOCK_TOKEN	(*(u32 *)(&get_paca()->lock_token))
#else
#define LOCK_TOKEN	(*(u32 *)(&get_paca()->paca_index))
#endif

static inline int try_lock_tlbie(unsigned int *lock)
{
	unsigned int tmp, old;
	unsigned int token = LOCK_TOKEN;

	asm volatile("1:lwarx	%1,0,%2\n"
		     "	cmpwi	cr0,%1,0\n"
		     "	bne	2f\n"
		     "  stwcx.	%3,0,%2\n"
		     "	bne-	1b\n"
		     "  isync\n"
		     "2:"
		     : "=&r" (tmp), "=&r" (old)
		     : "r" (lock), "r" (token)
		     : "cc", "memory");
	return old == 0;
}

<<<<<<< HEAD
/*
 * tlbie/tlbiel is a bit different on the PPC970 compared to later
 * processors such as POWER7; the large page bit is in the instruction
 * not RB, and the top 16 bits and the bottom 12 bits of the VA
 * in RB must be 0.
 */
static void do_tlbies_970(struct kvm *kvm, unsigned long *rbvalues,
			  long npages, int global, bool need_sync)
{
	long i;

	if (global) {
		while (!try_lock_tlbie(&kvm->arch.tlbie_lock))
			cpu_relax();
		if (need_sync)
			asm volatile("ptesync" : : : "memory");
		for (i = 0; i < npages; ++i) {
			unsigned long rb = rbvalues[i];

			if (rb & 1)		/* large page */
				asm volatile("tlbie %0,1" : :
					     "r" (rb & 0x0000fffffffff000ul));
			else
				asm volatile("tlbie %0,0" : :
					     "r" (rb & 0x0000fffffffff000ul));
		}
		asm volatile("eieio; tlbsync; ptesync" : : : "memory");
		kvm->arch.tlbie_lock = 0;
	} else {
		if (need_sync)
			asm volatile("ptesync" : : : "memory");
		for (i = 0; i < npages; ++i) {
			unsigned long rb = rbvalues[i];

			if (rb & 1)		/* large page */
				asm volatile("tlbiel %0,1" : :
					     "r" (rb & 0x0000fffffffff000ul));
			else
				asm volatile("tlbiel %0,0" : :
					     "r" (rb & 0x0000fffffffff000ul));
		}
		asm volatile("ptesync" : : : "memory");
	}
}

=======
>>>>>>> v4.9.227
static void do_tlbies(struct kvm *kvm, unsigned long *rbvalues,
		      long npages, int global, bool need_sync)
{
	long i;

<<<<<<< HEAD
	if (cpu_has_feature(CPU_FTR_ARCH_201)) {
		/* PPC970 tlbie instruction is a bit different */
		do_tlbies_970(kvm, rbvalues, npages, global, need_sync);
		return;
	}
=======
>>>>>>> v4.9.227
	if (global) {
		while (!try_lock_tlbie(&kvm->arch.tlbie_lock))
			cpu_relax();
		if (need_sync)
			asm volatile("ptesync" : : : "memory");
		for (i = 0; i < npages; ++i)
			asm volatile(PPC_TLBIE(%1,%0) : :
				     "r" (rbvalues[i]), "r" (kvm->arch.lpid));
		asm volatile("eieio; tlbsync; ptesync" : : : "memory");
		kvm->arch.tlbie_lock = 0;
	} else {
		if (need_sync)
			asm volatile("ptesync" : : : "memory");
		for (i = 0; i < npages; ++i)
			asm volatile("tlbiel %0" : : "r" (rbvalues[i]));
		asm volatile("ptesync" : : : "memory");
	}
}

long kvmppc_do_h_remove(struct kvm *kvm, unsigned long flags,
			unsigned long pte_index, unsigned long avpn,
			unsigned long *hpret)
{
	__be64 *hpte;
	unsigned long v, r, rb;
	struct revmap_entry *rev;
	u64 pte;

	if (pte_index >= kvm->arch.hpt_npte)
		return H_PARAMETER;
	hpte = (__be64 *)(kvm->arch.hpt_virt + (pte_index << 4));
	while (!try_lock_hpte(hpte, HPTE_V_HVLOCK))
		cpu_relax();
	pte = be64_to_cpu(hpte[0]);
	if ((pte & (HPTE_V_ABSENT | HPTE_V_VALID)) == 0 ||
	    ((flags & H_AVPN) && (pte & ~0x7fUL) != avpn) ||
	    ((flags & H_ANDCOND) && (pte & avpn) != 0)) {
<<<<<<< HEAD
		hpte[0] &= ~cpu_to_be64(HPTE_V_HVLOCK);
=======
		__unlock_hpte(hpte, pte);
>>>>>>> v4.9.227
		return H_NOT_FOUND;
	}

	rev = real_vmalloc_addr(&kvm->arch.revmap[pte_index]);
	v = pte & ~HPTE_V_HVLOCK;
	if (v & HPTE_V_VALID) {
<<<<<<< HEAD
		u64 pte1;

		pte1 = be64_to_cpu(hpte[1]);
		hpte[0] &= ~cpu_to_be64(HPTE_V_VALID);
		rb = compute_tlbie_rb(v, pte1, pte_index);
		do_tlbies(kvm, &rb, 1, global_invalidates(kvm, flags), true);
		/* Read PTE low word after tlbie to get final R/C values */
		remove_revmap_chain(kvm, pte_index, rev, v, pte1);
=======
		hpte[0] &= ~cpu_to_be64(HPTE_V_VALID);
		rb = compute_tlbie_rb(v, be64_to_cpu(hpte[1]), pte_index);
		do_tlbies(kvm, &rb, 1, global_invalidates(kvm, flags), true);
		/*
		 * The reference (R) and change (C) bits in a HPT
		 * entry can be set by hardware at any time up until
		 * the HPTE is invalidated and the TLB invalidation
		 * sequence has completed.  This means that when
		 * removing a HPTE, we need to re-read the HPTE after
		 * the invalidation sequence has completed in order to
		 * obtain reliable values of R and C.
		 */
		remove_revmap_chain(kvm, pte_index, rev, v,
				    be64_to_cpu(hpte[1]));
>>>>>>> v4.9.227
	}
	r = rev->guest_rpte & ~HPTE_GR_RESERVED;
	note_hpte_modification(kvm, rev);
	unlock_hpte(hpte, 0);

<<<<<<< HEAD
=======
	if (v & HPTE_V_ABSENT)
		v = (v & ~HPTE_V_ABSENT) | HPTE_V_VALID;
>>>>>>> v4.9.227
	hpret[0] = v;
	hpret[1] = r;
	return H_SUCCESS;
}
EXPORT_SYMBOL_GPL(kvmppc_do_h_remove);

long kvmppc_h_remove(struct kvm_vcpu *vcpu, unsigned long flags,
		     unsigned long pte_index, unsigned long avpn)
{
	return kvmppc_do_h_remove(vcpu->kvm, flags, pte_index, avpn,
				  &vcpu->arch.gpr[4]);
}

long kvmppc_h_bulk_remove(struct kvm_vcpu *vcpu)
{
	struct kvm *kvm = vcpu->kvm;
	unsigned long *args = &vcpu->arch.gpr[4];
	__be64 *hp, *hptes[4];
	unsigned long tlbrb[4];
	long int i, j, k, n, found, indexes[4];
	unsigned long flags, req, pte_index, rcbits;
	int global;
	long int ret = H_SUCCESS;
	struct revmap_entry *rev, *revs[4];
	u64 hp0;

	global = global_invalidates(kvm, 0);
	for (i = 0; i < 4 && ret == H_SUCCESS; ) {
		n = 0;
		for (; i < 4; ++i) {
			j = i * 2;
			pte_index = args[j];
			flags = pte_index >> 56;
			pte_index &= ((1ul << 56) - 1);
			req = flags >> 6;
			flags &= 3;
			if (req == 3) {		/* no more requests */
				i = 4;
				break;
			}
			if (req != 1 || flags == 3 ||
			    pte_index >= kvm->arch.hpt_npte) {
				/* parameter error */
				args[j] = ((0xa0 | flags) << 56) + pte_index;
				ret = H_PARAMETER;
				break;
			}
			hp = (__be64 *) (kvm->arch.hpt_virt + (pte_index << 4));
			/* to avoid deadlock, don't spin except for first */
			if (!try_lock_hpte(hp, HPTE_V_HVLOCK)) {
				if (n)
					break;
				while (!try_lock_hpte(hp, HPTE_V_HVLOCK))
					cpu_relax();
			}
			found = 0;
			hp0 = be64_to_cpu(hp[0]);
			if (hp0 & (HPTE_V_ABSENT | HPTE_V_VALID)) {
				switch (flags & 3) {
				case 0:		/* absolute */
					found = 1;
					break;
				case 1:		/* andcond */
					if (!(hp0 & args[j + 1]))
						found = 1;
					break;
				case 2:		/* AVPN */
					if ((hp0 & ~0x7fUL) == args[j + 1])
						found = 1;
					break;
				}
			}
			if (!found) {
				hp[0] &= ~cpu_to_be64(HPTE_V_HVLOCK);
				args[j] = ((0x90 | flags) << 56) + pte_index;
				continue;
			}

			args[j] = ((0x80 | flags) << 56) + pte_index;
			rev = real_vmalloc_addr(&kvm->arch.revmap[pte_index]);
			note_hpte_modification(kvm, rev);

			if (!(hp0 & HPTE_V_VALID)) {
				/* insert R and C bits from PTE */
				rcbits = rev->guest_rpte & (HPTE_R_R|HPTE_R_C);
				args[j] |= rcbits << (56 - 5);
				hp[0] = 0;
				continue;
			}

			/* leave it locked */
			hp[0] &= ~cpu_to_be64(HPTE_V_VALID);
			tlbrb[n] = compute_tlbie_rb(be64_to_cpu(hp[0]),
				be64_to_cpu(hp[1]), pte_index);
			indexes[n] = j;
			hptes[n] = hp;
			revs[n] = rev;
			++n;
		}

		if (!n)
			break;

		/* Now that we've collected a batch, do the tlbies */
		do_tlbies(kvm, tlbrb, n, global, true);

		/* Read PTE low words after tlbie to get final R/C values */
		for (k = 0; k < n; ++k) {
			j = indexes[k];
			pte_index = args[j] & ((1ul << 56) - 1);
			hp = hptes[k];
			rev = revs[k];
			remove_revmap_chain(kvm, pte_index, rev,
				be64_to_cpu(hp[0]), be64_to_cpu(hp[1]));
			rcbits = rev->guest_rpte & (HPTE_R_R|HPTE_R_C);
			args[j] |= rcbits << (56 - 5);
<<<<<<< HEAD
			hp[0] = 0;
=======
			__unlock_hpte(hp, 0);
>>>>>>> v4.9.227
		}
	}

	return ret;
}

long kvmppc_h_protect(struct kvm_vcpu *vcpu, unsigned long flags,
		      unsigned long pte_index, unsigned long avpn,
		      unsigned long va)
{
	struct kvm *kvm = vcpu->kvm;
	__be64 *hpte;
	struct revmap_entry *rev;
	unsigned long v, r, rb, mask, bits;
	u64 pte;

	if (pte_index >= kvm->arch.hpt_npte)
		return H_PARAMETER;

	hpte = (__be64 *)(kvm->arch.hpt_virt + (pte_index << 4));
	while (!try_lock_hpte(hpte, HPTE_V_HVLOCK))
		cpu_relax();
	pte = be64_to_cpu(hpte[0]);
	if ((pte & (HPTE_V_ABSENT | HPTE_V_VALID)) == 0 ||
	    ((flags & H_AVPN) && (pte & ~0x7fUL) != avpn)) {
<<<<<<< HEAD
		hpte[0] &= ~cpu_to_be64(HPTE_V_HVLOCK);
=======
		__unlock_hpte(hpte, pte);
>>>>>>> v4.9.227
		return H_NOT_FOUND;
	}

	v = pte;
	bits = (flags << 55) & HPTE_R_PP0;
	bits |= (flags << 48) & HPTE_R_KEY_HI;
	bits |= flags & (HPTE_R_PP | HPTE_R_N | HPTE_R_KEY_LO);

	/* Update guest view of 2nd HPTE dword */
	mask = HPTE_R_PP0 | HPTE_R_PP | HPTE_R_N |
		HPTE_R_KEY_HI | HPTE_R_KEY_LO;
	rev = real_vmalloc_addr(&kvm->arch.revmap[pte_index]);
	if (rev) {
		r = (rev->guest_rpte & ~mask) | bits;
		rev->guest_rpte = r;
		note_hpte_modification(kvm, rev);
	}
<<<<<<< HEAD
	r = (be64_to_cpu(hpte[1]) & ~mask) | bits;

	/* Update HPTE */
	if (v & HPTE_V_VALID) {
		rb = compute_tlbie_rb(v, r, pte_index);
		hpte[0] = cpu_to_be64(v & ~HPTE_V_VALID);
		do_tlbies(kvm, &rb, 1, global_invalidates(kvm, flags), true);
		/*
		 * If the host has this page as readonly but the guest
		 * wants to make it read/write, reduce the permissions.
		 * Checking the host permissions involves finding the
		 * memslot and then the Linux PTE for the page.
		 */
		if (hpte_is_writable(r) && kvm->arch.using_mmu_notifiers) {
			unsigned long psize, gfn, hva;
			struct kvm_memory_slot *memslot;
			pgd_t *pgdir = vcpu->arch.pgdir;
			pte_t pte;

			psize = hpte_page_size(v, r);
			gfn = ((r & HPTE_R_RPN) & ~(psize - 1)) >> PAGE_SHIFT;
			memslot = __gfn_to_memslot(kvm_memslots_raw(kvm), gfn);
			if (memslot) {
				hva = __gfn_to_hva_memslot(memslot, gfn);
				pte = lookup_linux_pte_and_update(pgdir, hva,
								  1, &psize);
				if (pte_present(pte) && !pte_write(pte))
					r = hpte_make_readonly(r);
			}
		}
	}
	hpte[1] = cpu_to_be64(r);
	eieio();
	hpte[0] = cpu_to_be64(v & ~HPTE_V_HVLOCK);
=======

	/* Update HPTE */
	if (v & HPTE_V_VALID) {
		/*
		 * If the page is valid, don't let it transition from
		 * readonly to writable.  If it should be writable, we'll
		 * take a trap and let the page fault code sort it out.
		 */
		pte = be64_to_cpu(hpte[1]);
		r = (pte & ~mask) | bits;
		if (hpte_is_writable(r) && !hpte_is_writable(pte))
			r = hpte_make_readonly(r);
		/* If the PTE is changing, invalidate it first */
		if (r != pte) {
			rb = compute_tlbie_rb(v, r, pte_index);
			hpte[0] = cpu_to_be64((v & ~HPTE_V_VALID) |
					      HPTE_V_ABSENT);
			do_tlbies(kvm, &rb, 1, global_invalidates(kvm, flags),
				  true);
			/* Don't lose R/C bit updates done by hardware */
			r |= be64_to_cpu(hpte[1]) & (HPTE_R_R | HPTE_R_C);
			hpte[1] = cpu_to_be64(r);
		}
	}
	unlock_hpte(hpte, v & ~HPTE_V_HVLOCK);
>>>>>>> v4.9.227
	asm volatile("ptesync" : : : "memory");
	return H_SUCCESS;
}

long kvmppc_h_read(struct kvm_vcpu *vcpu, unsigned long flags,
		   unsigned long pte_index)
{
	struct kvm *kvm = vcpu->kvm;
	__be64 *hpte;
	unsigned long v, r;
	int i, n = 1;
	struct revmap_entry *rev = NULL;

	if (pte_index >= kvm->arch.hpt_npte)
		return H_PARAMETER;
	if (flags & H_READ_4) {
		pte_index &= ~3;
		n = 4;
	}
	rev = real_vmalloc_addr(&kvm->arch.revmap[pte_index]);
	for (i = 0; i < n; ++i, ++pte_index) {
		hpte = (__be64 *)(kvm->arch.hpt_virt + (pte_index << 4));
		v = be64_to_cpu(hpte[0]) & ~HPTE_V_HVLOCK;
		r = be64_to_cpu(hpte[1]);
		if (v & HPTE_V_ABSENT) {
			v &= ~HPTE_V_ABSENT;
			v |= HPTE_V_VALID;
		}
		if (v & HPTE_V_VALID) {
			r = rev[i].guest_rpte | (r & (HPTE_R_R | HPTE_R_C));
			r &= ~HPTE_GR_RESERVED;
		}
		vcpu->arch.gpr[4 + i * 2] = v;
		vcpu->arch.gpr[5 + i * 2] = r;
	}
	return H_SUCCESS;
}

<<<<<<< HEAD
=======
long kvmppc_h_clear_ref(struct kvm_vcpu *vcpu, unsigned long flags,
			unsigned long pte_index)
{
	struct kvm *kvm = vcpu->kvm;
	__be64 *hpte;
	unsigned long v, r, gr;
	struct revmap_entry *rev;
	unsigned long *rmap;
	long ret = H_NOT_FOUND;

	if (pte_index >= kvm->arch.hpt_npte)
		return H_PARAMETER;

	rev = real_vmalloc_addr(&kvm->arch.revmap[pte_index]);
	hpte = (__be64 *)(kvm->arch.hpt_virt + (pte_index << 4));
	while (!try_lock_hpte(hpte, HPTE_V_HVLOCK))
		cpu_relax();
	v = be64_to_cpu(hpte[0]);
	r = be64_to_cpu(hpte[1]);
	if (!(v & (HPTE_V_VALID | HPTE_V_ABSENT)))
		goto out;

	gr = rev->guest_rpte;
	if (rev->guest_rpte & HPTE_R_R) {
		rev->guest_rpte &= ~HPTE_R_R;
		note_hpte_modification(kvm, rev);
	}
	if (v & HPTE_V_VALID) {
		gr |= r & (HPTE_R_R | HPTE_R_C);
		if (r & HPTE_R_R) {
			kvmppc_clear_ref_hpte(kvm, hpte, pte_index);
			rmap = revmap_for_hpte(kvm, v, gr);
			if (rmap) {
				lock_rmap(rmap);
				*rmap |= KVMPPC_RMAP_REFERENCED;
				unlock_rmap(rmap);
			}
		}
	}
	vcpu->arch.gpr[4] = gr;
	ret = H_SUCCESS;
 out:
	unlock_hpte(hpte, v & ~HPTE_V_HVLOCK);
	return ret;
}

long kvmppc_h_clear_mod(struct kvm_vcpu *vcpu, unsigned long flags,
			unsigned long pte_index)
{
	struct kvm *kvm = vcpu->kvm;
	__be64 *hpte;
	unsigned long v, r, gr;
	struct revmap_entry *rev;
	unsigned long *rmap;
	long ret = H_NOT_FOUND;

	if (pte_index >= kvm->arch.hpt_npte)
		return H_PARAMETER;

	rev = real_vmalloc_addr(&kvm->arch.revmap[pte_index]);
	hpte = (__be64 *)(kvm->arch.hpt_virt + (pte_index << 4));
	while (!try_lock_hpte(hpte, HPTE_V_HVLOCK))
		cpu_relax();
	v = be64_to_cpu(hpte[0]);
	r = be64_to_cpu(hpte[1]);
	if (!(v & (HPTE_V_VALID | HPTE_V_ABSENT)))
		goto out;

	gr = rev->guest_rpte;
	if (gr & HPTE_R_C) {
		rev->guest_rpte &= ~HPTE_R_C;
		note_hpte_modification(kvm, rev);
	}
	if (v & HPTE_V_VALID) {
		/* need to make it temporarily absent so C is stable */
		hpte[0] |= cpu_to_be64(HPTE_V_ABSENT);
		kvmppc_invalidate_hpte(kvm, hpte, pte_index);
		r = be64_to_cpu(hpte[1]);
		gr |= r & (HPTE_R_R | HPTE_R_C);
		if (r & HPTE_R_C) {
			unsigned long psize = hpte_page_size(v, r);
			hpte[1] = cpu_to_be64(r & ~HPTE_R_C);
			eieio();
			rmap = revmap_for_hpte(kvm, v, gr);
			if (rmap) {
				lock_rmap(rmap);
				*rmap |= KVMPPC_RMAP_CHANGED;
				kvmppc_update_rmap_change(rmap, psize);
				unlock_rmap(rmap);
			}
		}
	}
	vcpu->arch.gpr[4] = gr;
	ret = H_SUCCESS;
 out:
	unlock_hpte(hpte, v & ~HPTE_V_HVLOCK);
	return ret;
}

>>>>>>> v4.9.227
void kvmppc_invalidate_hpte(struct kvm *kvm, __be64 *hptep,
			unsigned long pte_index)
{
	unsigned long rb;

	hptep[0] &= ~cpu_to_be64(HPTE_V_VALID);
	rb = compute_tlbie_rb(be64_to_cpu(hptep[0]), be64_to_cpu(hptep[1]),
			      pte_index);
	do_tlbies(kvm, &rb, 1, 1, true);
}
EXPORT_SYMBOL_GPL(kvmppc_invalidate_hpte);

void kvmppc_clear_ref_hpte(struct kvm *kvm, __be64 *hptep,
			   unsigned long pte_index)
{
	unsigned long rb;
	unsigned char rbyte;

	rb = compute_tlbie_rb(be64_to_cpu(hptep[0]), be64_to_cpu(hptep[1]),
			      pte_index);
	rbyte = (be64_to_cpu(hptep[1]) & ~HPTE_R_R) >> 8;
	/* modify only the second-last byte, which contains the ref bit */
	*((char *)hptep + 14) = rbyte;
	do_tlbies(kvm, &rb, 1, 1, false);
}
EXPORT_SYMBOL_GPL(kvmppc_clear_ref_hpte);

static int slb_base_page_shift[4] = {
	24,	/* 16M */
	16,	/* 64k */
	34,	/* 16G */
	20,	/* 1M, unsupported */
};

/* When called from virtmode, this func should be protected by
 * preempt_disable(), otherwise, the holding of HPTE_V_HVLOCK
 * can trigger deadlock issue.
 */
long kvmppc_hv_find_lock_hpte(struct kvm *kvm, gva_t eaddr, unsigned long slb_v,
			      unsigned long valid)
{
	unsigned int i;
	unsigned int pshift;
	unsigned long somask;
	unsigned long vsid, hash;
	unsigned long avpn;
	__be64 *hpte;
	unsigned long mask, val;
	unsigned long v, r;

	/* Get page shift, work out hash and AVPN etc. */
	mask = SLB_VSID_B | HPTE_V_AVPN | HPTE_V_SECONDARY;
	val = 0;
	pshift = 12;
	if (slb_v & SLB_VSID_L) {
		mask |= HPTE_V_LARGE;
		val |= HPTE_V_LARGE;
		pshift = slb_base_page_shift[(slb_v & SLB_VSID_LP) >> 4];
	}
	if (slb_v & SLB_VSID_B_1T) {
		somask = (1UL << 40) - 1;
		vsid = (slb_v & ~SLB_VSID_B) >> SLB_VSID_SHIFT_1T;
		vsid ^= vsid << 25;
	} else {
		somask = (1UL << 28) - 1;
		vsid = (slb_v & ~SLB_VSID_B) >> SLB_VSID_SHIFT;
	}
	hash = (vsid ^ ((eaddr & somask) >> pshift)) & kvm->arch.hpt_mask;
	avpn = slb_v & ~(somask >> 16);	/* also includes B */
	avpn |= (eaddr & somask) >> 16;

	if (pshift >= 24)
		avpn &= ~((1UL << (pshift - 16)) - 1);
	else
		avpn &= ~0x7fUL;
	val |= avpn;

	for (;;) {
		hpte = (__be64 *)(kvm->arch.hpt_virt + (hash << 7));

		for (i = 0; i < 16; i += 2) {
			/* Read the PTE racily */
			v = be64_to_cpu(hpte[i]) & ~HPTE_V_HVLOCK;

			/* Check valid/absent, hash, segment size and AVPN */
			if (!(v & valid) || (v & mask) != val)
				continue;

			/* Lock the PTE and read it under the lock */
			while (!try_lock_hpte(&hpte[i], HPTE_V_HVLOCK))
				cpu_relax();
			v = be64_to_cpu(hpte[i]) & ~HPTE_V_HVLOCK;
			r = be64_to_cpu(hpte[i+1]);

			/*
			 * Check the HPTE again, including base page size
			 */
			if ((v & valid) && (v & mask) == val &&
			    hpte_base_page_size(v, r) == (1ul << pshift))
				/* Return with the HPTE still locked */
				return (hash << 3) + (i >> 1);

<<<<<<< HEAD
			/* Unlock and move on */
			hpte[i] = cpu_to_be64(v);
=======
			__unlock_hpte(&hpte[i], v);
>>>>>>> v4.9.227
		}

		if (val & HPTE_V_SECONDARY)
			break;
		val |= HPTE_V_SECONDARY;
		hash = hash ^ kvm->arch.hpt_mask;
	}
	return -1;
}
EXPORT_SYMBOL(kvmppc_hv_find_lock_hpte);

/*
 * Called in real mode to check whether an HPTE not found fault
 * is due to accessing a paged-out page or an emulated MMIO page,
 * or if a protection fault is due to accessing a page that the
 * guest wanted read/write access to but which we made read-only.
 * Returns a possibly modified status (DSISR) value if not
 * (i.e. pass the interrupt to the guest),
 * -1 to pass the fault up to host kernel mode code, -2 to do that
 * and also load the instruction word (for MMIO emulation),
 * or 0 if we should make the guest retry the access.
 */
long kvmppc_hpte_hv_fault(struct kvm_vcpu *vcpu, unsigned long addr,
			  unsigned long slb_v, unsigned int status, bool data)
{
	struct kvm *kvm = vcpu->kvm;
	long int index;
	unsigned long v, r, gr;
	__be64 *hpte;
	unsigned long valid;
	struct revmap_entry *rev;
	unsigned long pp, key;

	/* For protection fault, expect to find a valid HPTE */
	valid = HPTE_V_VALID;
	if (status & DSISR_NOHPTE)
		valid |= HPTE_V_ABSENT;

	index = kvmppc_hv_find_lock_hpte(kvm, addr, slb_v, valid);
	if (index < 0) {
		if (status & DSISR_NOHPTE)
			return status;	/* there really was no HPTE */
		return 0;		/* for prot fault, HPTE disappeared */
	}
	hpte = (__be64 *)(kvm->arch.hpt_virt + (index << 4));
	v = be64_to_cpu(hpte[0]) & ~HPTE_V_HVLOCK;
	r = be64_to_cpu(hpte[1]);
	rev = real_vmalloc_addr(&kvm->arch.revmap[index]);
	gr = rev->guest_rpte;

	unlock_hpte(hpte, v);

	/* For not found, if the HPTE is valid by now, retry the instruction */
	if ((status & DSISR_NOHPTE) && (v & HPTE_V_VALID))
		return 0;

	/* Check access permissions to the page */
	pp = gr & (HPTE_R_PP0 | HPTE_R_PP);
	key = (vcpu->arch.shregs.msr & MSR_PR) ? SLB_VSID_KP : SLB_VSID_KS;
	status &= ~DSISR_NOHPTE;	/* DSISR_NOHPTE == SRR1_ISI_NOPT */
	if (!data) {
		if (gr & (HPTE_R_N | HPTE_R_G))
			return status | SRR1_ISI_N_OR_G;
		if (!hpte_read_permission(pp, slb_v & key))
			return status | SRR1_ISI_PROT;
	} else if (status & DSISR_ISSTORE) {
		/* check write permission */
		if (!hpte_write_permission(pp, slb_v & key))
			return status | DSISR_PROTFAULT;
	} else {
		if (!hpte_read_permission(pp, slb_v & key))
			return status | DSISR_PROTFAULT;
	}

	/* Check storage key, if applicable */
	if (data && (vcpu->arch.shregs.msr & MSR_DR)) {
		unsigned int perm = hpte_get_skey_perm(gr, vcpu->arch.amr);
		if (status & DSISR_ISSTORE)
			perm >>= 1;
		if (perm & 1)
			return status | DSISR_KEYFAULT;
	}

	/* Save HPTE info for virtual-mode handler */
	vcpu->arch.pgfault_addr = addr;
	vcpu->arch.pgfault_index = index;
	vcpu->arch.pgfault_hpte[0] = v;
	vcpu->arch.pgfault_hpte[1] = r;

	/* Check the storage key to see if it is possibly emulated MMIO */
	if (data && (vcpu->arch.shregs.msr & MSR_IR) &&
	    (r & (HPTE_R_KEY_HI | HPTE_R_KEY_LO)) ==
	    (HPTE_R_KEY_HI | HPTE_R_KEY_LO))
		return -2;	/* MMIO emulation - load instr word */

	return -1;		/* send fault up to host kernel mode */
}
