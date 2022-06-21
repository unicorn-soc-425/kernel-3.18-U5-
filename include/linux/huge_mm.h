#ifndef _LINUX_HUGE_MM_H
#define _LINUX_HUGE_MM_H

<<<<<<< HEAD
extern int do_huge_pmd_anonymous_page(struct mm_struct *mm,
				      struct vm_area_struct *vma,
				      unsigned long address, pmd_t *pmd,
				      unsigned int flags);
extern int copy_huge_pmd(struct mm_struct *dst_mm, struct mm_struct *src_mm,
			 pmd_t *dst_pmd, pmd_t *src_pmd, unsigned long addr,
			 struct vm_area_struct *vma);
extern void huge_pmd_set_accessed(struct mm_struct *mm,
				  struct vm_area_struct *vma,
				  unsigned long address, pmd_t *pmd,
				  pmd_t orig_pmd, int dirty);
extern int do_huge_pmd_wp_page(struct mm_struct *mm, struct vm_area_struct *vma,
			       unsigned long address, pmd_t *pmd,
			       pmd_t orig_pmd);
=======
extern int do_huge_pmd_anonymous_page(struct fault_env *fe);
extern int copy_huge_pmd(struct mm_struct *dst_mm, struct mm_struct *src_mm,
			 pmd_t *dst_pmd, pmd_t *src_pmd, unsigned long addr,
			 struct vm_area_struct *vma);
extern void huge_pmd_set_accessed(struct fault_env *fe, pmd_t orig_pmd);
extern int do_huge_pmd_wp_page(struct fault_env *fe, pmd_t orig_pmd);
>>>>>>> v4.9.227
extern struct page *follow_trans_huge_pmd(struct vm_area_struct *vma,
					  unsigned long addr,
					  pmd_t *pmd,
					  unsigned int flags);
<<<<<<< HEAD
=======
extern bool madvise_free_huge_pmd(struct mmu_gather *tlb,
			struct vm_area_struct *vma,
			pmd_t *pmd, unsigned long addr, unsigned long next);
>>>>>>> v4.9.227
extern int zap_huge_pmd(struct mmu_gather *tlb,
			struct vm_area_struct *vma,
			pmd_t *pmd, unsigned long addr);
extern int mincore_huge_pmd(struct vm_area_struct *vma, pmd_t *pmd,
			unsigned long addr, unsigned long end,
			unsigned char *vec);
<<<<<<< HEAD
extern int move_huge_pmd(struct vm_area_struct *vma,
			 struct vm_area_struct *new_vma,
			 unsigned long old_addr,
=======
extern bool move_huge_pmd(struct vm_area_struct *vma, unsigned long old_addr,
>>>>>>> v4.9.227
			 unsigned long new_addr, unsigned long old_end,
			 pmd_t *old_pmd, pmd_t *new_pmd);
extern int change_huge_pmd(struct vm_area_struct *vma, pmd_t *pmd,
			unsigned long addr, pgprot_t newprot,
			int prot_numa);
<<<<<<< HEAD

enum transparent_hugepage_flag {
	TRANSPARENT_HUGEPAGE_FLAG,
	TRANSPARENT_HUGEPAGE_REQ_MADV_FLAG,
	TRANSPARENT_HUGEPAGE_DEFRAG_FLAG,
=======
int vmf_insert_pfn_pmd(struct vm_area_struct *, unsigned long addr, pmd_t *,
			pfn_t pfn, bool write);
enum transparent_hugepage_flag {
	TRANSPARENT_HUGEPAGE_FLAG,
	TRANSPARENT_HUGEPAGE_REQ_MADV_FLAG,
	TRANSPARENT_HUGEPAGE_DEFRAG_DIRECT_FLAG,
	TRANSPARENT_HUGEPAGE_DEFRAG_KSWAPD_FLAG,
>>>>>>> v4.9.227
	TRANSPARENT_HUGEPAGE_DEFRAG_REQ_MADV_FLAG,
	TRANSPARENT_HUGEPAGE_DEFRAG_KHUGEPAGED_FLAG,
	TRANSPARENT_HUGEPAGE_USE_ZERO_PAGE_FLAG,
#ifdef CONFIG_DEBUG_VM
	TRANSPARENT_HUGEPAGE_DEBUG_COW_FLAG,
#endif
};

<<<<<<< HEAD
enum page_check_address_pmd_flag {
	PAGE_CHECK_ADDRESS_PMD_FLAG,
	PAGE_CHECK_ADDRESS_PMD_NOTSPLITTING_FLAG,
	PAGE_CHECK_ADDRESS_PMD_SPLITTING_FLAG,
};
extern pmd_t *page_check_address_pmd(struct page *page,
				     struct mm_struct *mm,
				     unsigned long address,
				     enum page_check_address_pmd_flag flag,
				     spinlock_t **ptl);
=======
struct kobject;
struct kobj_attribute;

extern ssize_t single_hugepage_flag_store(struct kobject *kobj,
				 struct kobj_attribute *attr,
				 const char *buf, size_t count,
				 enum transparent_hugepage_flag flag);
extern ssize_t single_hugepage_flag_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf,
				enum transparent_hugepage_flag flag);
extern struct kobj_attribute shmem_enabled_attr;
>>>>>>> v4.9.227

#define HPAGE_PMD_ORDER (HPAGE_PMD_SHIFT-PAGE_SHIFT)
#define HPAGE_PMD_NR (1<<HPAGE_PMD_ORDER)

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
<<<<<<< HEAD
=======
struct page *follow_devmap_pmd(struct vm_area_struct *vma, unsigned long addr,
		pmd_t *pmd, int flags);

>>>>>>> v4.9.227
#define HPAGE_PMD_SHIFT PMD_SHIFT
#define HPAGE_PMD_SIZE	((1UL) << HPAGE_PMD_SHIFT)
#define HPAGE_PMD_MASK	(~(HPAGE_PMD_SIZE - 1))

extern bool is_vma_temporary_stack(struct vm_area_struct *vma);

#define transparent_hugepage_enabled(__vma)				\
	((transparent_hugepage_flags &					\
	  (1<<TRANSPARENT_HUGEPAGE_FLAG) ||				\
	  (transparent_hugepage_flags &					\
	   (1<<TRANSPARENT_HUGEPAGE_REQ_MADV_FLAG) &&			\
	   ((__vma)->vm_flags & VM_HUGEPAGE))) &&			\
	 !((__vma)->vm_flags & VM_NOHUGEPAGE) &&			\
	 !is_vma_temporary_stack(__vma))
<<<<<<< HEAD
#define transparent_hugepage_defrag(__vma)				\
	((transparent_hugepage_flags &					\
	  (1<<TRANSPARENT_HUGEPAGE_DEFRAG_FLAG)) ||			\
	 (transparent_hugepage_flags &					\
	  (1<<TRANSPARENT_HUGEPAGE_DEFRAG_REQ_MADV_FLAG) &&		\
	  (__vma)->vm_flags & VM_HUGEPAGE))
=======
>>>>>>> v4.9.227
#define transparent_hugepage_use_zero_page()				\
	(transparent_hugepage_flags &					\
	 (1<<TRANSPARENT_HUGEPAGE_USE_ZERO_PAGE_FLAG))
#ifdef CONFIG_DEBUG_VM
#define transparent_hugepage_debug_cow()				\
	(transparent_hugepage_flags &					\
	 (1<<TRANSPARENT_HUGEPAGE_DEBUG_COW_FLAG))
#else /* CONFIG_DEBUG_VM */
#define transparent_hugepage_debug_cow() 0
#endif /* CONFIG_DEBUG_VM */

extern unsigned long transparent_hugepage_flags;
<<<<<<< HEAD
extern int split_huge_page_to_list(struct page *page, struct list_head *list);
=======

extern unsigned long thp_get_unmapped_area(struct file *filp,
		unsigned long addr, unsigned long len, unsigned long pgoff,
		unsigned long flags);

extern void prep_transhuge_page(struct page *page);
extern void free_transhuge_page(struct page *page);

int split_huge_page_to_list(struct page *page, struct list_head *list);
>>>>>>> v4.9.227
static inline int split_huge_page(struct page *page)
{
	return split_huge_page_to_list(page, NULL);
}
<<<<<<< HEAD
extern void __split_huge_page_pmd(struct vm_area_struct *vma,
		unsigned long address, pmd_t *pmd);
#define split_huge_page_pmd(__vma, __address, __pmd)			\
	do {								\
		pmd_t *____pmd = (__pmd);				\
		if (unlikely(pmd_trans_huge(*____pmd)))			\
			__split_huge_page_pmd(__vma, __address,		\
					____pmd);			\
	}  while (0)
#define wait_split_huge_page(__anon_vma, __pmd)				\
	do {								\
		pmd_t *____pmd = (__pmd);				\
		anon_vma_lock_write(__anon_vma);			\
		anon_vma_unlock_write(__anon_vma);			\
		BUG_ON(pmd_trans_splitting(*____pmd) ||			\
		       pmd_trans_huge(*____pmd));			\
	} while (0)
extern void split_huge_page_pmd_mm(struct mm_struct *mm, unsigned long address,
		pmd_t *pmd);
#if HPAGE_PMD_ORDER >= MAX_ORDER
#error "hugepages can't be allocated by the buddy allocator"
#endif
extern int hugepage_madvise(struct vm_area_struct *vma,
			    unsigned long *vm_flags, int advice);
extern void __vma_adjust_trans_huge(struct vm_area_struct *vma,
				    unsigned long start,
				    unsigned long end,
				    long adjust_next);
extern int __pmd_trans_huge_lock(pmd_t *pmd, struct vm_area_struct *vma,
		spinlock_t **ptl);
/* mmap_sem must be held on entry */
static inline int pmd_trans_huge_lock(pmd_t *pmd, struct vm_area_struct *vma,
		spinlock_t **ptl)
{
	VM_BUG_ON_VMA(!rwsem_is_locked(&vma->vm_mm->mmap_sem), vma);
	if (pmd_trans_huge(*pmd))
		return __pmd_trans_huge_lock(pmd, vma, ptl);
	else
		return 0;
}
static inline void vma_adjust_trans_huge(struct vm_area_struct *vma,
					 unsigned long start,
					 unsigned long end,
					 long adjust_next)
{
	if (!vma->anon_vma || vma->vm_ops)
		return;
	__vma_adjust_trans_huge(vma, start, end, adjust_next);
=======
void deferred_split_huge_page(struct page *page);

void __split_huge_pmd(struct vm_area_struct *vma, pmd_t *pmd,
		unsigned long address, bool freeze, struct page *page);

#define split_huge_pmd(__vma, __pmd, __address)				\
	do {								\
		pmd_t *____pmd = (__pmd);				\
		if (pmd_trans_huge(*____pmd)				\
					|| pmd_devmap(*____pmd))	\
			__split_huge_pmd(__vma, __pmd, __address,	\
						false, NULL);		\
	}  while (0)


void split_huge_pmd_address(struct vm_area_struct *vma, unsigned long address,
		bool freeze, struct page *page);

extern int hugepage_madvise(struct vm_area_struct *vma,
			    unsigned long *vm_flags, int advice);
extern void vma_adjust_trans_huge(struct vm_area_struct *vma,
				    unsigned long start,
				    unsigned long end,
				    long adjust_next);
extern spinlock_t *__pmd_trans_huge_lock(pmd_t *pmd,
		struct vm_area_struct *vma);
/* mmap_sem must be held on entry */
static inline spinlock_t *pmd_trans_huge_lock(pmd_t *pmd,
		struct vm_area_struct *vma)
{
	VM_BUG_ON_VMA(!rwsem_is_locked(&vma->vm_mm->mmap_sem), vma);
	if (pmd_trans_huge(*pmd) || pmd_devmap(*pmd))
		return __pmd_trans_huge_lock(pmd, vma);
	else
		return NULL;
>>>>>>> v4.9.227
}
static inline int hpage_nr_pages(struct page *page)
{
	if (unlikely(PageTransHuge(page)))
		return HPAGE_PMD_NR;
	return 1;
}

<<<<<<< HEAD
extern int do_huge_pmd_numa_page(struct mm_struct *mm, struct vm_area_struct *vma,
				unsigned long addr, pmd_t pmd, pmd_t *pmdp);
=======
extern int do_huge_pmd_numa_page(struct fault_env *fe, pmd_t orig_pmd);

extern struct page *huge_zero_page;

static inline bool is_huge_zero_page(struct page *page)
{
	return ACCESS_ONCE(huge_zero_page) == page;
}

static inline bool is_huge_zero_pmd(pmd_t pmd)
{
	return is_huge_zero_page(pmd_page(pmd));
}

struct page *mm_get_huge_zero_page(struct mm_struct *mm);
void mm_put_huge_zero_page(struct mm_struct *mm);

#define mk_huge_pmd(page, prot) pmd_mkhuge(mk_pmd(page, prot))
>>>>>>> v4.9.227

#else /* CONFIG_TRANSPARENT_HUGEPAGE */
#define HPAGE_PMD_SHIFT ({ BUILD_BUG(); 0; })
#define HPAGE_PMD_MASK ({ BUILD_BUG(); 0; })
#define HPAGE_PMD_SIZE ({ BUILD_BUG(); 0; })

#define hpage_nr_pages(x) 1

#define transparent_hugepage_enabled(__vma) 0

<<<<<<< HEAD
#define transparent_hugepage_flags 0UL
=======
static inline void prep_transhuge_page(struct page *page) {}

#define transparent_hugepage_flags 0UL

#define thp_get_unmapped_area	NULL

>>>>>>> v4.9.227
static inline int
split_huge_page_to_list(struct page *page, struct list_head *list)
{
	return 0;
}
static inline int split_huge_page(struct page *page)
{
	return 0;
}
<<<<<<< HEAD
#define split_huge_page_pmd(__vma, __address, __pmd)	\
	do { } while (0)
#define wait_split_huge_page(__anon_vma, __pmd)	\
	do { } while (0)
#define split_huge_page_pmd_mm(__mm, __address, __pmd)	\
	do { } while (0)
=======
static inline void deferred_split_huge_page(struct page *page) {}
#define split_huge_pmd(__vma, __pmd, __address)	\
	do { } while (0)

static inline void split_huge_pmd_address(struct vm_area_struct *vma,
		unsigned long address, bool freeze, struct page *page) {}

>>>>>>> v4.9.227
static inline int hugepage_madvise(struct vm_area_struct *vma,
				   unsigned long *vm_flags, int advice)
{
	BUG();
	return 0;
}
static inline void vma_adjust_trans_huge(struct vm_area_struct *vma,
					 unsigned long start,
					 unsigned long end,
					 long adjust_next)
{
}
<<<<<<< HEAD
static inline int pmd_trans_huge_lock(pmd_t *pmd, struct vm_area_struct *vma,
		spinlock_t **ptl)
=======
static inline spinlock_t *pmd_trans_huge_lock(pmd_t *pmd,
		struct vm_area_struct *vma)
{
	return NULL;
}

static inline int do_huge_pmd_numa_page(struct fault_env *fe, pmd_t orig_pmd)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static inline int do_huge_pmd_numa_page(struct mm_struct *mm, struct vm_area_struct *vma,
					unsigned long addr, pmd_t pmd, pmd_t *pmdp)
{
	return 0;
}

=======
static inline bool is_huge_zero_page(struct page *page)
{
	return false;
}

static inline void mm_put_huge_zero_page(struct mm_struct *mm)
{
	return;
}

static inline struct page *follow_devmap_pmd(struct vm_area_struct *vma,
		unsigned long addr, pmd_t *pmd, int flags)
{
	return NULL;
}
>>>>>>> v4.9.227
#endif /* CONFIG_TRANSPARENT_HUGEPAGE */

#endif /* _LINUX_HUGE_MM_H */
