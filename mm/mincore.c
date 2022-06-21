/*
 *	linux/mm/mincore.c
 *
 * Copyright (C) 1994-2006  Linus Torvalds
 */

/*
 * The mincore() system call.
 */
#include <linux/pagemap.h>
#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/syscalls.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/hugetlb.h>

#include <asm/uaccess.h>
#include <asm/pgtable.h>

<<<<<<< HEAD
static void mincore_hugetlb_page_range(struct vm_area_struct *vma,
				unsigned long addr, unsigned long end,
				unsigned char *vec)
{
#ifdef CONFIG_HUGETLB_PAGE
	struct hstate *h;

	h = hstate_vma(vma);
	while (1) {
		unsigned char present;
		pte_t *ptep;
		/*
		 * Huge pages are always in RAM for now, but
		 * theoretically it needs to be checked.
		 */
		ptep = huge_pte_offset(current->mm,
				       addr & huge_page_mask(h));
		present = ptep && !huge_pte_none(huge_ptep_get(ptep));
		while (1) {
			*vec = present;
			vec++;
			addr += PAGE_SIZE;
			if (addr == end)
				return;
			/* check hugepage border */
			if (!(addr & ~huge_page_mask(h)))
				break;
		}
	}
#else
	BUG();
#endif
=======
static int mincore_hugetlb(pte_t *pte, unsigned long hmask, unsigned long addr,
			unsigned long end, struct mm_walk *walk)
{
#ifdef CONFIG_HUGETLB_PAGE
	unsigned char present;
	unsigned char *vec = walk->private;

	/*
	 * Hugepages under user process are always in RAM and never
	 * swapped out, but theoretically it needs to be checked.
	 */
	present = pte && !huge_pte_none(huge_ptep_get(pte));
	for (; addr != end; vec++, addr += PAGE_SIZE)
		*vec = present;
	walk->private = vec;
#else
	BUG();
#endif
	return 0;
>>>>>>> v4.9.227
}

/*
 * Later we can get more picky about what "in core" means precisely.
 * For now, simply check to see if the page is in the page cache,
 * and is up to date; i.e. that no page-in operation would be required
 * at this time if an application were to map and access this page.
 */
static unsigned char mincore_page(struct address_space *mapping, pgoff_t pgoff)
{
	unsigned char present = 0;
	struct page *page;

	/*
	 * When tmpfs swaps out a page from a file, any process mapping that
	 * file will not get a swp_entry_t in its pte, but rather it is like
	 * any other file mapping (ie. marked !present and faulted in with
	 * tmpfs's .fault). So swapped out tmpfs mappings are tested here.
	 */
#ifdef CONFIG_SWAP
	if (shmem_mapping(mapping)) {
		page = find_get_entry(mapping, pgoff);
		/*
		 * shmem/tmpfs may return swap: account for swapcache
		 * page too.
		 */
		if (radix_tree_exceptional_entry(page)) {
			swp_entry_t swp = radix_to_swp_entry(page);
<<<<<<< HEAD
			page = find_get_page(swap_address_space(swp), swp.val);
=======
			page = find_get_page(swap_address_space(swp),
					     swp_offset(swp));
>>>>>>> v4.9.227
		}
	} else
		page = find_get_page(mapping, pgoff);
#else
	page = find_get_page(mapping, pgoff);
#endif
	if (page) {
		present = PageUptodate(page);
<<<<<<< HEAD
		page_cache_release(page);
=======
		put_page(page);
>>>>>>> v4.9.227
	}

	return present;
}

<<<<<<< HEAD
static void mincore_unmapped_range(struct vm_area_struct *vma,
				unsigned long addr, unsigned long end,
				unsigned char *vec)
=======
static int __mincore_unmapped_range(unsigned long addr, unsigned long end,
				struct vm_area_struct *vma, unsigned char *vec)
>>>>>>> v4.9.227
{
	unsigned long nr = (end - addr) >> PAGE_SHIFT;
	int i;

	if (vma->vm_file) {
		pgoff_t pgoff;

		pgoff = linear_page_index(vma, addr);
		for (i = 0; i < nr; i++, pgoff++)
			vec[i] = mincore_page(vma->vm_file->f_mapping, pgoff);
	} else {
		for (i = 0; i < nr; i++)
			vec[i] = 0;
	}
<<<<<<< HEAD
}

static void mincore_pte_range(struct vm_area_struct *vma, pmd_t *pmd,
			unsigned long addr, unsigned long end,
			unsigned char *vec)
{
	unsigned long next;
	spinlock_t *ptl;
	pte_t *ptep;

	ptep = pte_offset_map_lock(vma->vm_mm, pmd, addr, &ptl);
	do {
		pte_t pte = *ptep;
		pgoff_t pgoff;

		next = addr + PAGE_SIZE;
		if (pte_none(pte))
			mincore_unmapped_range(vma, addr, next, vec);
		else if (pte_present(pte))
			*vec = 1;
		else if (pte_file(pte)) {
			pgoff = pte_to_pgoff(pte);
			*vec = mincore_page(vma->vm_file->f_mapping, pgoff);
		} else { /* pte is a swap entry */
			swp_entry_t entry = pte_to_swp_entry(pte);

			if (is_migration_entry(entry)) {
				/* migration entries are always uptodate */
				*vec = 1;
			} else {
#ifdef CONFIG_SWAP
				pgoff = entry.val;
				*vec = mincore_page(swap_address_space(entry),
					pgoff);
=======
	return nr;
}

static int mincore_unmapped_range(unsigned long addr, unsigned long end,
				   struct mm_walk *walk)
{
	walk->private += __mincore_unmapped_range(addr, end,
						  walk->vma, walk->private);
	return 0;
}

static int mincore_pte_range(pmd_t *pmd, unsigned long addr, unsigned long end,
			struct mm_walk *walk)
{
	spinlock_t *ptl;
	struct vm_area_struct *vma = walk->vma;
	pte_t *ptep;
	unsigned char *vec = walk->private;
	int nr = (end - addr) >> PAGE_SHIFT;

	ptl = pmd_trans_huge_lock(pmd, vma);
	if (ptl) {
		memset(vec, 1, nr);
		spin_unlock(ptl);
		goto out;
	}

	if (pmd_trans_unstable(pmd)) {
		__mincore_unmapped_range(addr, end, vma, vec);
		goto out;
	}

	ptep = pte_offset_map_lock(walk->mm, pmd, addr, &ptl);
	for (; addr != end; ptep++, addr += PAGE_SIZE) {
		pte_t pte = *ptep;

		if (pte_none(pte))
			__mincore_unmapped_range(addr, addr + PAGE_SIZE,
						 vma, vec);
		else if (pte_present(pte))
			*vec = 1;
		else { /* pte is a swap entry */
			swp_entry_t entry = pte_to_swp_entry(pte);

			if (non_swap_entry(entry)) {
				/*
				 * migration or hwpoison entries are always
				 * uptodate
				 */
				*vec = 1;
			} else {
#ifdef CONFIG_SWAP
				*vec = mincore_page(swap_address_space(entry),
						    swp_offset(entry));
>>>>>>> v4.9.227
#else
				WARN_ON(1);
				*vec = 1;
#endif
			}
		}
		vec++;
<<<<<<< HEAD
	} while (ptep++, addr = next, addr != end);
	pte_unmap_unlock(ptep - 1, ptl);
}

static void mincore_pmd_range(struct vm_area_struct *vma, pud_t *pud,
			unsigned long addr, unsigned long end,
			unsigned char *vec)
{
	unsigned long next;
	pmd_t *pmd;

	pmd = pmd_offset(pud, addr);
	do {
		next = pmd_addr_end(addr, end);
		if (pmd_trans_huge(*pmd)) {
			if (mincore_huge_pmd(vma, pmd, addr, next, vec)) {
				vec += (next - addr) >> PAGE_SHIFT;
				continue;
			}
			/* fall through */
		}
		if (pmd_none_or_trans_huge_or_clear_bad(pmd))
			mincore_unmapped_range(vma, addr, next, vec);
		else
			mincore_pte_range(vma, pmd, addr, next, vec);
		vec += (next - addr) >> PAGE_SHIFT;
	} while (pmd++, addr = next, addr != end);
}

static void mincore_pud_range(struct vm_area_struct *vma, pgd_t *pgd,
			unsigned long addr, unsigned long end,
			unsigned char *vec)
{
	unsigned long next;
	pud_t *pud;

	pud = pud_offset(pgd, addr);
	do {
		next = pud_addr_end(addr, end);
		if (pud_none_or_clear_bad(pud))
			mincore_unmapped_range(vma, addr, next, vec);
		else
			mincore_pmd_range(vma, pud, addr, next, vec);
		vec += (next - addr) >> PAGE_SHIFT;
	} while (pud++, addr = next, addr != end);
}

static void mincore_page_range(struct vm_area_struct *vma,
			unsigned long addr, unsigned long end,
			unsigned char *vec)
{
	unsigned long next;
	pgd_t *pgd;

	pgd = pgd_offset(vma->vm_mm, addr);
	do {
		next = pgd_addr_end(addr, end);
		if (pgd_none_or_clear_bad(pgd))
			mincore_unmapped_range(vma, addr, next, vec);
		else
			mincore_pud_range(vma, pgd, addr, next, vec);
		vec += (next - addr) >> PAGE_SHIFT;
	} while (pgd++, addr = next, addr != end);
=======
	}
	pte_unmap_unlock(ptep - 1, ptl);
out:
	walk->private += nr;
	cond_resched();
	return 0;
}

static inline bool can_do_mincore(struct vm_area_struct *vma)
{
	if (vma_is_anonymous(vma))
		return true;
	if (!vma->vm_file)
		return false;
	/*
	 * Reveal pagecache information only for non-anonymous mappings that
	 * correspond to the files the calling process could (if tried) open
	 * for writing; otherwise we'd be including shared non-exclusive
	 * mappings, which opens a side channel.
	 */
	return inode_owner_or_capable(file_inode(vma->vm_file)) ||
		inode_permission(file_inode(vma->vm_file), MAY_WRITE) == 0;
>>>>>>> v4.9.227
}

/*
 * Do a chunk of "sys_mincore()". We've already checked
 * all the arguments, we hold the mmap semaphore: we should
 * just return the amount of info we're asked for.
 */
static long do_mincore(unsigned long addr, unsigned long pages, unsigned char *vec)
{
	struct vm_area_struct *vma;
	unsigned long end;
<<<<<<< HEAD
=======
	int err;
	struct mm_walk mincore_walk = {
		.pmd_entry = mincore_pte_range,
		.pte_hole = mincore_unmapped_range,
		.hugetlb_entry = mincore_hugetlb,
		.private = vec,
	};
>>>>>>> v4.9.227

	vma = find_vma(current->mm, addr);
	if (!vma || addr < vma->vm_start)
		return -ENOMEM;
<<<<<<< HEAD

	end = min(vma->vm_end, addr + (pages << PAGE_SHIFT));

	if (is_vm_hugetlb_page(vma))
		mincore_hugetlb_page_range(vma, addr, end, vec);
	else
		mincore_page_range(vma, addr, end, vec);

=======
	end = min(vma->vm_end, addr + (pages << PAGE_SHIFT));
	if (!can_do_mincore(vma)) {
		unsigned long pages = DIV_ROUND_UP(end - addr, PAGE_SIZE);
		memset(vec, 1, pages);
		return pages;
	}
	mincore_walk.mm = vma->vm_mm;
	err = walk_page_range(addr, end, &mincore_walk);
	if (err < 0)
		return err;
>>>>>>> v4.9.227
	return (end - addr) >> PAGE_SHIFT;
}

/*
 * The mincore(2) system call.
 *
 * mincore() returns the memory residency status of the pages in the
 * current process's address space specified by [addr, addr + len).
 * The status is returned in a vector of bytes.  The least significant
 * bit of each byte is 1 if the referenced page is in memory, otherwise
 * it is zero.
 *
 * Because the status of a page can change after mincore() checks it
 * but before it returns to the application, the returned vector may
 * contain stale information.  Only locked pages are guaranteed to
 * remain in memory.
 *
 * return values:
 *  zero    - success
 *  -EFAULT - vec points to an illegal address
<<<<<<< HEAD
 *  -EINVAL - addr is not a multiple of PAGE_CACHE_SIZE
=======
 *  -EINVAL - addr is not a multiple of PAGE_SIZE
>>>>>>> v4.9.227
 *  -ENOMEM - Addresses in the range [addr, addr + len] are
 *		invalid for the address space of this process, or
 *		specify one or more pages which are not currently
 *		mapped
 *  -EAGAIN - A kernel resource was temporarily unavailable.
 */
SYSCALL_DEFINE3(mincore, unsigned long, start, size_t, len,
		unsigned char __user *, vec)
{
	long retval;
	unsigned long pages;
	unsigned char *tmp;

	/* Check the start address: needs to be page-aligned.. */
<<<<<<< HEAD
 	if (start & ~PAGE_CACHE_MASK)
=======
	if (start & ~PAGE_MASK)
>>>>>>> v4.9.227
		return -EINVAL;

	/* ..and we need to be passed a valid user-space range */
	if (!access_ok(VERIFY_READ, (void __user *) start, len))
		return -ENOMEM;

<<<<<<< HEAD
	/* This also avoids any overflows on PAGE_CACHE_ALIGN */
	pages = len >> PAGE_SHIFT;
	pages += (len & ~PAGE_MASK) != 0;
=======
	/* This also avoids any overflows on PAGE_ALIGN */
	pages = len >> PAGE_SHIFT;
	pages += (offset_in_page(len)) != 0;
>>>>>>> v4.9.227

	if (!access_ok(VERIFY_WRITE, vec, pages))
		return -EFAULT;

	tmp = (void *) __get_free_page(GFP_USER);
	if (!tmp)
		return -EAGAIN;

	retval = 0;
	while (pages) {
		/*
		 * Do at most PAGE_SIZE entries per iteration, due to
		 * the temporary buffer size.
		 */
		down_read(&current->mm->mmap_sem);
		retval = do_mincore(start, min(pages, PAGE_SIZE), tmp);
		up_read(&current->mm->mmap_sem);

		if (retval <= 0)
			break;
		if (copy_to_user(vec, tmp, retval)) {
			retval = -EFAULT;
			break;
		}
		pages -= retval;
		vec += retval;
		start += retval << PAGE_SHIFT;
		retval = 0;
	}
	free_page((unsigned long) tmp);
	return retval;
}
