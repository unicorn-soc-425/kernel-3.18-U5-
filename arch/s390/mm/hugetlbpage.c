/*
 *  IBM System z Huge TLB Page Support for Kernel.
 *
<<<<<<< HEAD
 *    Copyright IBM Corp. 2007
 *    Author(s): Gerald Schaefer <gerald.schaefer@de.ibm.com>
 */

#include <linux/mm.h>
#include <linux/hugetlb.h>

static inline pmd_t __pte_to_pmd(pte_t pte)
{
	pmd_t pmd;

	/*
	 * Convert encoding		  pte bits	   pmd bits
	 *				.IR...wrdytp	dy..R...I...wr
	 * empty			.10...000000 -> 00..0...1...00
	 * prot-none, clean, old	.11...000001 -> 00..1...1...00
	 * prot-none, clean, young	.11...000101 -> 01..1...1...00
	 * prot-none, dirty, old	.10...001001 -> 10..1...1...00
	 * prot-none, dirty, young	.10...001101 -> 11..1...1...00
	 * read-only, clean, old	.11...010001 -> 00..1...1...01
	 * read-only, clean, young	.01...010101 -> 01..1...0...01
	 * read-only, dirty, old	.11...011001 -> 10..1...1...01
	 * read-only, dirty, young	.01...011101 -> 11..1...0...01
	 * read-write, clean, old	.11...110001 -> 00..0...1...11
	 * read-write, clean, young	.01...110101 -> 01..0...0...11
	 * read-write, dirty, old	.10...111001 -> 10..0...1...11
	 * read-write, dirty, young	.00...111101 -> 11..0...0...11
	 */
	if (pte_present(pte)) {
		pmd_val(pmd) = pte_val(pte) & PAGE_MASK;
		pmd_val(pmd) |= (pte_val(pte) & _PAGE_READ) >> 4;
		pmd_val(pmd) |= (pte_val(pte) & _PAGE_WRITE) >> 4;
		pmd_val(pmd) |=	(pte_val(pte) & _PAGE_INVALID) >> 5;
		pmd_val(pmd) |= (pte_val(pte) & _PAGE_PROTECT);
		pmd_val(pmd) |= (pte_val(pte) & _PAGE_DIRTY) << 10;
		pmd_val(pmd) |= (pte_val(pte) & _PAGE_YOUNG) << 10;
	} else
		pmd_val(pmd) = _SEGMENT_ENTRY_INVALID;
	return pmd;
}

static inline pte_t __pmd_to_pte(pmd_t pmd)
{
	pte_t pte;

	/*
	 * Convert encoding		   pmd bits	    pte bits
	 *				dy..R...I...wr	  .IR...wrdytp
	 * empty			00..0...1...00 -> .10...001100
	 * prot-none, clean, old	00..0...1...00 -> .10...000001
	 * prot-none, clean, young	01..0...1...00 -> .10...000101
	 * prot-none, dirty, old	10..0...1...00 -> .10...001001
	 * prot-none, dirty, young	11..0...1...00 -> .10...001101
	 * read-only, clean, old	00..1...1...01 -> .11...010001
	 * read-only, clean, young	01..1...1...01 -> .11...010101
	 * read-only, dirty, old	10..1...1...01 -> .11...011001
	 * read-only, dirty, young	11..1...1...01 -> .11...011101
	 * read-write, clean, old	00..0...1...11 -> .10...110001
	 * read-write, clean, young	01..0...1...11 -> .10...110101
	 * read-write, dirty, old	10..0...1...11 -> .10...111001
	 * read-write, dirty, young	11..0...1...11 -> .10...111101
	 */
	if (pmd_present(pmd)) {
		pte_val(pte) = pmd_val(pmd) & _SEGMENT_ENTRY_ORIGIN_LARGE;
		pte_val(pte) |= _PAGE_LARGE | _PAGE_PRESENT;
		pte_val(pte) |= (pmd_val(pmd) & _SEGMENT_ENTRY_READ) << 4;
		pte_val(pte) |= (pmd_val(pmd) & _SEGMENT_ENTRY_WRITE) << 4;
		pte_val(pte) |= (pmd_val(pmd) & _SEGMENT_ENTRY_INVALID) << 5;
		pte_val(pte) |= (pmd_val(pmd) & _SEGMENT_ENTRY_PROTECT);
		pmd_val(pmd) |= (pte_val(pte) & _PAGE_DIRTY) << 10;
		pmd_val(pmd) |= (pte_val(pte) & _PAGE_YOUNG) << 10;
=======
 *    Copyright IBM Corp. 2007,2016
 *    Author(s): Gerald Schaefer <gerald.schaefer@de.ibm.com>
 */

#define KMSG_COMPONENT "hugetlb"
#define pr_fmt(fmt) KMSG_COMPONENT ": " fmt

#include <linux/mm.h>
#include <linux/hugetlb.h>

/*
 * If the bit selected by single-bit bitmask "a" is set within "x", move
 * it to the position indicated by single-bit bitmask "b".
 */
#define move_set_bit(x, a, b)	(((x) & (a)) >> ilog2(a) << ilog2(b))

static inline unsigned long __pte_to_rste(pte_t pte)
{
	unsigned long rste;

	/*
	 * Convert encoding		  pte bits	pmd / pud bits
	 *				lIR.uswrdy.p	dy..R...I...wr
	 * empty			010.000000.0 -> 00..0...1...00
	 * prot-none, clean, old	111.000000.1 -> 00..1...1...00
	 * prot-none, clean, young	111.000001.1 -> 01..1...1...00
	 * prot-none, dirty, old	111.000010.1 -> 10..1...1...00
	 * prot-none, dirty, young	111.000011.1 -> 11..1...1...00
	 * read-only, clean, old	111.000100.1 -> 00..1...1...01
	 * read-only, clean, young	101.000101.1 -> 01..1...0...01
	 * read-only, dirty, old	111.000110.1 -> 10..1...1...01
	 * read-only, dirty, young	101.000111.1 -> 11..1...0...01
	 * read-write, clean, old	111.001100.1 -> 00..1...1...11
	 * read-write, clean, young	101.001101.1 -> 01..1...0...11
	 * read-write, dirty, old	110.001110.1 -> 10..0...1...11
	 * read-write, dirty, young	100.001111.1 -> 11..0...0...11
	 * HW-bits: R read-only, I invalid
	 * SW-bits: p present, y young, d dirty, r read, w write, s special,
	 *	    u unused, l large
	 */
	if (pte_present(pte)) {
		rste = pte_val(pte) & PAGE_MASK;
		rste |= move_set_bit(pte_val(pte), _PAGE_READ,
				     _SEGMENT_ENTRY_READ);
		rste |= move_set_bit(pte_val(pte), _PAGE_WRITE,
				     _SEGMENT_ENTRY_WRITE);
		rste |= move_set_bit(pte_val(pte), _PAGE_INVALID,
				     _SEGMENT_ENTRY_INVALID);
		rste |= move_set_bit(pte_val(pte), _PAGE_PROTECT,
				     _SEGMENT_ENTRY_PROTECT);
		rste |= move_set_bit(pte_val(pte), _PAGE_DIRTY,
				     _SEGMENT_ENTRY_DIRTY);
		rste |= move_set_bit(pte_val(pte), _PAGE_YOUNG,
				     _SEGMENT_ENTRY_YOUNG);
#ifdef CONFIG_MEM_SOFT_DIRTY
		rste |= move_set_bit(pte_val(pte), _PAGE_SOFT_DIRTY,
				     _SEGMENT_ENTRY_SOFT_DIRTY);
#endif
	} else
		rste = _SEGMENT_ENTRY_INVALID;
	return rste;
}

static inline pte_t __rste_to_pte(unsigned long rste)
{
	int present;
	pte_t pte;

	if ((rste & _REGION_ENTRY_TYPE_MASK) == _REGION_ENTRY_TYPE_R3)
		present = pud_present(__pud(rste));
	else
		present = pmd_present(__pmd(rste));

	/*
	 * Convert encoding		pmd / pud bits	    pte bits
	 *				dy..R...I...wr	  lIR.uswrdy.p
	 * empty			00..0...1...00 -> 010.000000.0
	 * prot-none, clean, old	00..1...1...00 -> 111.000000.1
	 * prot-none, clean, young	01..1...1...00 -> 111.000001.1
	 * prot-none, dirty, old	10..1...1...00 -> 111.000010.1
	 * prot-none, dirty, young	11..1...1...00 -> 111.000011.1
	 * read-only, clean, old	00..1...1...01 -> 111.000100.1
	 * read-only, clean, young	01..1...0...01 -> 101.000101.1
	 * read-only, dirty, old	10..1...1...01 -> 111.000110.1
	 * read-only, dirty, young	11..1...0...01 -> 101.000111.1
	 * read-write, clean, old	00..1...1...11 -> 111.001100.1
	 * read-write, clean, young	01..1...0...11 -> 101.001101.1
	 * read-write, dirty, old	10..0...1...11 -> 110.001110.1
	 * read-write, dirty, young	11..0...0...11 -> 100.001111.1
	 * HW-bits: R read-only, I invalid
	 * SW-bits: p present, y young, d dirty, r read, w write, s special,
	 *	    u unused, l large
	 */
	if (present) {
		pte_val(pte) = rste & _SEGMENT_ENTRY_ORIGIN_LARGE;
		pte_val(pte) |= _PAGE_LARGE | _PAGE_PRESENT;
		pte_val(pte) |= move_set_bit(rste, _SEGMENT_ENTRY_READ,
					     _PAGE_READ);
		pte_val(pte) |= move_set_bit(rste, _SEGMENT_ENTRY_WRITE,
					     _PAGE_WRITE);
		pte_val(pte) |= move_set_bit(rste, _SEGMENT_ENTRY_INVALID,
					     _PAGE_INVALID);
		pte_val(pte) |= move_set_bit(rste, _SEGMENT_ENTRY_PROTECT,
					     _PAGE_PROTECT);
		pte_val(pte) |= move_set_bit(rste, _SEGMENT_ENTRY_DIRTY,
					     _PAGE_DIRTY);
		pte_val(pte) |= move_set_bit(rste, _SEGMENT_ENTRY_YOUNG,
					     _PAGE_YOUNG);
#ifdef CONFIG_MEM_SOFT_DIRTY
		pte_val(pte) |= move_set_bit(rste, _SEGMENT_ENTRY_SOFT_DIRTY,
					     _PAGE_DIRTY);
#endif
>>>>>>> v4.9.227
	} else
		pte_val(pte) = _PAGE_INVALID;
	return pte;
}

void set_huge_pte_at(struct mm_struct *mm, unsigned long addr,
		     pte_t *ptep, pte_t pte)
{
<<<<<<< HEAD
	pmd_t pmd;

	pmd = __pte_to_pmd(pte);
	if (!MACHINE_HAS_HPAGE) {
		/* Emulated huge ptes loose the dirty and young bit */
		pmd_val(pmd) &= ~_SEGMENT_ENTRY_ORIGIN;
		pmd_val(pmd) |= pte_page(pte)[1].index;
	} else
		pmd_val(pmd) |= _SEGMENT_ENTRY_LARGE;
	*(pmd_t *) ptep = pmd;
=======
	unsigned long rste = __pte_to_rste(pte);

	/* Set correct table type for 2G hugepages */
	if ((pte_val(*ptep) & _REGION_ENTRY_TYPE_MASK) == _REGION_ENTRY_TYPE_R3)
		rste |= _REGION_ENTRY_TYPE_R3 | _REGION3_ENTRY_LARGE;
	else
		rste |= _SEGMENT_ENTRY_LARGE;
	pte_val(*ptep) = rste;
>>>>>>> v4.9.227
}

pte_t huge_ptep_get(pte_t *ptep)
{
<<<<<<< HEAD
	unsigned long origin;
	pmd_t pmd;

	pmd = *(pmd_t *) ptep;
	if (!MACHINE_HAS_HPAGE && pmd_present(pmd)) {
		origin = pmd_val(pmd) & _SEGMENT_ENTRY_ORIGIN;
		pmd_val(pmd) &= ~_SEGMENT_ENTRY_ORIGIN;
		pmd_val(pmd) |= *(unsigned long *) origin;
		/* Emulated huge ptes are young and dirty by definition */
		pmd_val(pmd) |= _SEGMENT_ENTRY_YOUNG | _SEGMENT_ENTRY_DIRTY;
	}
	return __pmd_to_pte(pmd);
=======
	return __rste_to_pte(pte_val(*ptep));
>>>>>>> v4.9.227
}

pte_t huge_ptep_get_and_clear(struct mm_struct *mm,
			      unsigned long addr, pte_t *ptep)
{
<<<<<<< HEAD
	pmd_t *pmdp = (pmd_t *) ptep;
	pte_t pte = huge_ptep_get(ptep);

	pmdp_flush_direct(mm, addr, pmdp);
	pmd_val(*pmdp) = _SEGMENT_ENTRY_EMPTY;
	return pte;
}

int arch_prepare_hugepage(struct page *page)
{
	unsigned long addr = page_to_phys(page);
	pte_t pte;
	pte_t *ptep;
	int i;

	if (MACHINE_HAS_HPAGE)
		return 0;

	ptep = (pte_t *) pte_alloc_one(&init_mm, addr);
	if (!ptep)
		return -ENOMEM;

	pte_val(pte) = addr;
	for (i = 0; i < PTRS_PER_PTE; i++) {
		set_pte_at(&init_mm, addr + i * PAGE_SIZE, ptep + i, pte);
		pte_val(pte) += PAGE_SIZE;
	}
	page[1].index = (unsigned long) ptep;
	return 0;
}

void arch_release_hugepage(struct page *page)
{
	pte_t *ptep;

	if (MACHINE_HAS_HPAGE)
		return;

	ptep = (pte_t *) page[1].index;
	if (!ptep)
		return;
	clear_table((unsigned long *) ptep, _PAGE_INVALID,
		    PTRS_PER_PTE * sizeof(pte_t));
	page_table_free(&init_mm, (unsigned long *) ptep);
	page[1].index = 0;
}

=======
	pte_t pte = huge_ptep_get(ptep);
	pmd_t *pmdp = (pmd_t *) ptep;
	pud_t *pudp = (pud_t *) ptep;

	if ((pte_val(*ptep) & _REGION_ENTRY_TYPE_MASK) == _REGION_ENTRY_TYPE_R3)
		pudp_xchg_direct(mm, addr, pudp, __pud(_REGION3_ENTRY_EMPTY));
	else
		pmdp_xchg_direct(mm, addr, pmdp, __pmd(_SEGMENT_ENTRY_EMPTY));
	return pte;
}

>>>>>>> v4.9.227
pte_t *huge_pte_alloc(struct mm_struct *mm,
			unsigned long addr, unsigned long sz)
{
	pgd_t *pgdp;
	pud_t *pudp;
	pmd_t *pmdp = NULL;

	pgdp = pgd_offset(mm, addr);
	pudp = pud_alloc(mm, pgdp, addr);
<<<<<<< HEAD
	if (pudp)
		pmdp = pmd_alloc(mm, pudp, addr);
=======
	if (pudp) {
		if (sz == PUD_SIZE)
			return (pte_t *) pudp;
		else if (sz == PMD_SIZE)
			pmdp = pmd_alloc(mm, pudp, addr);
	}
>>>>>>> v4.9.227
	return (pte_t *) pmdp;
}

pte_t *huge_pte_offset(struct mm_struct *mm, unsigned long addr)
{
	pgd_t *pgdp;
	pud_t *pudp;
	pmd_t *pmdp = NULL;

	pgdp = pgd_offset(mm, addr);
	if (pgd_present(*pgdp)) {
		pudp = pud_offset(pgdp, addr);
<<<<<<< HEAD
		if (pud_present(*pudp))
			pmdp = pmd_offset(pudp, addr);
=======
		if (pud_present(*pudp)) {
			if (pud_large(*pudp))
				return (pte_t *) pudp;
			pmdp = pmd_offset(pudp, addr);
		}
>>>>>>> v4.9.227
	}
	return (pte_t *) pmdp;
}

<<<<<<< HEAD
int huge_pmd_unshare(struct mm_struct *mm, unsigned long *addr, pte_t *ptep)
{
	return 0;
}

int pmd_huge(pmd_t pmd)
{
	if (!MACHINE_HAS_HPAGE)
		return 0;

	return !!(pmd_val(pmd) & _SEGMENT_ENTRY_LARGE);
=======
int pmd_huge(pmd_t pmd)
{
	return pmd_large(pmd);
>>>>>>> v4.9.227
}

int pud_huge(pud_t pud)
{
<<<<<<< HEAD
	return 0;
}
=======
	return pud_large(pud);
}

struct page *
follow_huge_pud(struct mm_struct *mm, unsigned long address,
		pud_t *pud, int flags)
{
	if (flags & FOLL_GET)
		return NULL;

	return pud_page(*pud) + ((address & ~PUD_MASK) >> PAGE_SHIFT);
}

static __init int setup_hugepagesz(char *opt)
{
	unsigned long size;
	char *string = opt;

	size = memparse(opt, &opt);
	if (MACHINE_HAS_EDAT1 && size == PMD_SIZE) {
		hugetlb_add_hstate(PMD_SHIFT - PAGE_SHIFT);
	} else if (MACHINE_HAS_EDAT2 && size == PUD_SIZE) {
		hugetlb_add_hstate(PUD_SHIFT - PAGE_SHIFT);
	} else {
		hugetlb_bad_size();
		pr_err("hugepagesz= specifies an unsupported page size %s\n",
			string);
		return 0;
	}
	return 1;
}
__setup("hugepagesz=", setup_hugepagesz);
>>>>>>> v4.9.227
