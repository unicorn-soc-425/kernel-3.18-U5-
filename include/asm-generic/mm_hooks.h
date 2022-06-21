/*
<<<<<<< HEAD
 * Define generic no-op hooks for arch_dup_mmap and arch_exit_mmap, to
 * be included in asm-FOO/mmu_context.h for any arch FOO which doesn't
 * need to hook these.
=======
 * Define generic no-op hooks for arch_dup_mmap, arch_exit_mmap
 * and arch_unmap to be included in asm-FOO/mmu_context.h for any
 * arch FOO which doesn't need to hook these.
>>>>>>> v4.9.227
 */
#ifndef _ASM_GENERIC_MM_HOOKS_H
#define _ASM_GENERIC_MM_HOOKS_H

static inline void arch_dup_mmap(struct mm_struct *oldmm,
				 struct mm_struct *mm)
{
}

static inline void arch_exit_mmap(struct mm_struct *mm)
{
}

<<<<<<< HEAD
=======
static inline void arch_unmap(struct mm_struct *mm,
			struct vm_area_struct *vma,
			unsigned long start, unsigned long end)
{
}

static inline void arch_bprm_mm_init(struct mm_struct *mm,
				     struct vm_area_struct *vma)
{
}

static inline bool arch_vma_access_permitted(struct vm_area_struct *vma,
		bool write, bool execute, bool foreign)
{
	/* by default, allow everything */
	return true;
}

static inline bool arch_pte_access_permitted(pte_t pte, bool write)
{
	/* by default, allow everything */
	return true;
}
>>>>>>> v4.9.227
#endif	/* _ASM_GENERIC_MM_HOOKS_H */
