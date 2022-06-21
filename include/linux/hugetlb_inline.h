#ifndef _LINUX_HUGETLB_INLINE_H
#define _LINUX_HUGETLB_INLINE_H

#ifdef CONFIG_HUGETLB_PAGE

#include <linux/mm.h>

<<<<<<< HEAD
static inline int is_vm_hugetlb_page(struct vm_area_struct *vma)
=======
static inline bool is_vm_hugetlb_page(struct vm_area_struct *vma)
>>>>>>> v4.9.227
{
	return !!(vma->vm_flags & VM_HUGETLB);
}

#else

<<<<<<< HEAD
static inline int is_vm_hugetlb_page(struct vm_area_struct *vma)
{
	return 0;
=======
static inline bool is_vm_hugetlb_page(struct vm_area_struct *vma)
{
	return false;
>>>>>>> v4.9.227
}

#endif

#endif
