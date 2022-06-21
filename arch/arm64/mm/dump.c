/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 * Debug helper to dump the current kernel pagetables of the system
 * so that we can see what the various memory ranges are set to.
 *
 * Derived from x86 and arm implementation:
 * (C) Copyright 2008 Intel Corporation
 *
 * Author: Arjan van de Ven <arjan@linux.intel.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */
#include <linux/debugfs.h>
<<<<<<< HEAD
#include <linux/fs.h>
#include <linux/io.h>
=======
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/init.h>
>>>>>>> v4.9.227
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/seq_file.h>

#include <asm/fixmap.h>
<<<<<<< HEAD
#include <asm/pgtable.h>

#define LOWEST_ADDR	(UL(0xffffffffffffffff) << VA_BITS)

struct addr_marker {
	unsigned long start_address;
	const char *name;
};

enum address_markers_idx {
	VMALLOC_START_NR = 0,
	VMALLOC_END_NR,
#ifdef CONFIG_SPARSEMEM_VMEMMAP
	VMEMMAP_START_NR,
	VMEMMAP_END_NR,
#endif
	PCI_START_NR,
	PCI_END_NR,
	FIXADDR_START_NR,
	FIXADDR_END_NR,
	MODULES_START_NR,
	MODUELS_END_NR,
	KERNEL_SPACE_NR,
};

static struct addr_marker address_markers[] = {
	{ VMALLOC_START,	"vmalloc() Area" },
	{ VMALLOC_END,		"vmalloc() End" },
#ifdef CONFIG_SPARSEMEM_VMEMMAP
	{ 0,			"vmemmap start" },
	{ 0,			"vmemmap end" },
#endif
	{ (unsigned long) PCI_IOBASE,		"PCI I/O start" },
	{ (unsigned long) PCI_IOBASE + SZ_16M,	"PCI I/O end" },
	{ FIXADDR_START,	"Fixmap start" },
	{ FIXADDR_TOP,		"Fixmap end" },
	{ MODULES_VADDR,	"Modules start" },
	{ MODULES_END,		"Modules end" },
	{ PAGE_OFFSET,		"Kernel Mapping" },
	{ -1,			NULL },
};

=======
#include <asm/kasan.h>
#include <asm/memory.h>
#include <asm/pgtable.h>
#include <asm/pgtable-hwdef.h>
#include <asm/ptdump.h>

static const struct addr_marker address_markers[] = {
#ifdef CONFIG_KASAN
	{ KASAN_SHADOW_START,		"Kasan shadow start" },
	{ KASAN_SHADOW_END,		"Kasan shadow end" },
#endif
	{ MODULES_VADDR,		"Modules start" },
	{ MODULES_END,			"Modules end" },
	{ VMALLOC_START,		"vmalloc() Area" },
	{ VMALLOC_END,			"vmalloc() End" },
	{ FIXADDR_START,		"Fixmap start" },
	{ FIXADDR_TOP,			"Fixmap end" },
	{ PCI_IO_START,			"PCI I/O start" },
	{ PCI_IO_END,			"PCI I/O end" },
#ifdef CONFIG_SPARSEMEM_VMEMMAP
	{ VMEMMAP_START,		"vmemmap start" },
	{ VMEMMAP_START + VMEMMAP_SIZE,	"vmemmap end" },
#endif
	{ PAGE_OFFSET,			"Linear Mapping" },
	{ -1,				NULL },
};

/*
 * The page dumper groups page table entries of the same type into a single
 * description. It uses pg_state to track the range information while
 * iterating over the pte entries. When the continuity is broken it then
 * dumps out a description of the range.
 */
>>>>>>> v4.9.227
struct pg_state {
	struct seq_file *seq;
	const struct addr_marker *marker;
	unsigned long start_address;
	unsigned level;
	u64 current_prot;
};

struct prot_bits {
	u64		mask;
	u64		val;
	const char	*set;
	const char	*clear;
};

static const struct prot_bits pte_bits[] = {
	{
<<<<<<< HEAD
=======
		.mask	= PTE_VALID,
		.val	= PTE_VALID,
		.set	= " ",
		.clear	= "F",
	}, {
>>>>>>> v4.9.227
		.mask	= PTE_USER,
		.val	= PTE_USER,
		.set	= "USR",
		.clear	= "   ",
	}, {
		.mask	= PTE_RDONLY,
		.val	= PTE_RDONLY,
		.set	= "ro",
		.clear	= "RW",
	}, {
		.mask	= PTE_PXN,
		.val	= PTE_PXN,
		.set	= "NX",
		.clear	= "x ",
	}, {
		.mask	= PTE_SHARED,
		.val	= PTE_SHARED,
		.set	= "SHD",
		.clear	= "   ",
	}, {
		.mask	= PTE_AF,
		.val	= PTE_AF,
		.set	= "AF",
		.clear	= "  ",
	}, {
		.mask	= PTE_NG,
		.val	= PTE_NG,
		.set	= "NG",
		.clear	= "  ",
	}, {
<<<<<<< HEAD
=======
		.mask	= PTE_CONT,
		.val	= PTE_CONT,
		.set	= "CON",
		.clear	= "   ",
	}, {
		.mask	= PTE_TABLE_BIT,
		.val	= PTE_TABLE_BIT,
		.set	= "   ",
		.clear	= "BLK",
	}, {
>>>>>>> v4.9.227
		.mask	= PTE_UXN,
		.val	= PTE_UXN,
		.set	= "UXN",
	}, {
		.mask	= PTE_ATTRINDX_MASK,
		.val	= PTE_ATTRINDX(MT_DEVICE_nGnRnE),
		.set	= "DEVICE/nGnRnE",
	}, {
		.mask	= PTE_ATTRINDX_MASK,
		.val	= PTE_ATTRINDX(MT_DEVICE_nGnRE),
		.set	= "DEVICE/nGnRE",
	}, {
		.mask	= PTE_ATTRINDX_MASK,
		.val	= PTE_ATTRINDX(MT_DEVICE_GRE),
		.set	= "DEVICE/GRE",
	}, {
		.mask	= PTE_ATTRINDX_MASK,
		.val	= PTE_ATTRINDX(MT_NORMAL_NC),
		.set	= "MEM/NORMAL-NC",
	}, {
		.mask	= PTE_ATTRINDX_MASK,
		.val	= PTE_ATTRINDX(MT_NORMAL),
		.set	= "MEM/NORMAL",
	}
};

struct pg_level {
	const struct prot_bits *bits;
<<<<<<< HEAD
=======
	const char *name;
>>>>>>> v4.9.227
	size_t num;
	u64 mask;
};

static struct pg_level pg_level[] = {
	{
	}, { /* pgd */
<<<<<<< HEAD
		.bits	= pte_bits,
		.num	= ARRAY_SIZE(pte_bits),
	}, { /* pud */
		.bits	= pte_bits,
		.num	= ARRAY_SIZE(pte_bits),
	}, { /* pmd */
		.bits	= pte_bits,
		.num	= ARRAY_SIZE(pte_bits),
	}, { /* pte */
=======
		.name	= "PGD",
		.bits	= pte_bits,
		.num	= ARRAY_SIZE(pte_bits),
	}, { /* pud */
		.name	= (CONFIG_PGTABLE_LEVELS > 3) ? "PUD" : "PGD",
		.bits	= pte_bits,
		.num	= ARRAY_SIZE(pte_bits),
	}, { /* pmd */
		.name	= (CONFIG_PGTABLE_LEVELS > 2) ? "PMD" : "PGD",
		.bits	= pte_bits,
		.num	= ARRAY_SIZE(pte_bits),
	}, { /* pte */
		.name	= "PTE",
>>>>>>> v4.9.227
		.bits	= pte_bits,
		.num	= ARRAY_SIZE(pte_bits),
	},
};

static void dump_prot(struct pg_state *st, const struct prot_bits *bits,
			size_t num)
{
	unsigned i;

	for (i = 0; i < num; i++, bits++) {
		const char *s;

		if ((st->current_prot & bits->mask) == bits->val)
			s = bits->set;
		else
			s = bits->clear;

		if (s)
			seq_printf(st->seq, " %s", s);
	}
}

static void note_page(struct pg_state *st, unsigned long addr, unsigned level,
				u64 val)
{
	static const char units[] = "KMGTPE";
	u64 prot = val & pg_level[level].mask;

<<<<<<< HEAD
	if (addr < LOWEST_ADDR)
		return;

=======
>>>>>>> v4.9.227
	if (!st->level) {
		st->level = level;
		st->current_prot = prot;
		st->start_address = addr;
		seq_printf(st->seq, "---[ %s ]---\n", st->marker->name);
	} else if (prot != st->current_prot || level != st->level ||
		   addr >= st->marker[1].start_address) {
		const char *unit = units;
		unsigned long delta;

		if (st->current_prot) {
<<<<<<< HEAD
			seq_printf(st->seq, "0x%16lx-0x%16lx   ",
=======
			seq_printf(st->seq, "0x%016lx-0x%016lx   ",
>>>>>>> v4.9.227
				   st->start_address, addr);

			delta = (addr - st->start_address) >> 10;
			while (!(delta & 1023) && unit[1]) {
				delta >>= 10;
				unit++;
			}
<<<<<<< HEAD
			seq_printf(st->seq, "%9lu%c", delta, *unit);
=======
			seq_printf(st->seq, "%9lu%c %s", delta, *unit,
				   pg_level[st->level].name);
>>>>>>> v4.9.227
			if (pg_level[st->level].bits)
				dump_prot(st, pg_level[st->level].bits,
					  pg_level[st->level].num);
			seq_puts(st->seq, "\n");
		}

		if (addr >= st->marker[1].start_address) {
			st->marker++;
			seq_printf(st->seq, "---[ %s ]---\n", st->marker->name);
		}

		st->start_address = addr;
		st->current_prot = prot;
		st->level = level;
	}

	if (addr >= st->marker[1].start_address) {
		st->marker++;
		seq_printf(st->seq, "---[ %s ]---\n", st->marker->name);
	}

}

static void walk_pte(struct pg_state *st, pmd_t *pmd, unsigned long start)
{
<<<<<<< HEAD
	pte_t *pte = pte_offset_kernel(pmd, 0);
=======
	pte_t *pte = pte_offset_kernel(pmd, 0UL);
>>>>>>> v4.9.227
	unsigned long addr;
	unsigned i;

	for (i = 0; i < PTRS_PER_PTE; i++, pte++) {
		addr = start + i * PAGE_SIZE;
		note_page(st, addr, 4, pte_val(*pte));
	}
}

static void walk_pmd(struct pg_state *st, pud_t *pud, unsigned long start)
{
<<<<<<< HEAD
	pmd_t *pmd = pmd_offset(pud, 0);
=======
	pmd_t *pmd = pmd_offset(pud, 0UL);
>>>>>>> v4.9.227
	unsigned long addr;
	unsigned i;

	for (i = 0; i < PTRS_PER_PMD; i++, pmd++) {
		addr = start + i * PMD_SIZE;
<<<<<<< HEAD
		if (pmd_none(*pmd) || pmd_sect(*pmd) || pmd_bad(*pmd))
			note_page(st, addr, 3, pmd_val(*pmd));
		else
			walk_pte(st, pmd, addr);
=======
		if (pmd_none(*pmd) || pmd_sect(*pmd)) {
			note_page(st, addr, 3, pmd_val(*pmd));
		} else {
			BUG_ON(pmd_bad(*pmd));
			walk_pte(st, pmd, addr);
		}
>>>>>>> v4.9.227
	}
}

static void walk_pud(struct pg_state *st, pgd_t *pgd, unsigned long start)
{
<<<<<<< HEAD
	pud_t *pud = pud_offset(pgd, 0);
=======
	pud_t *pud = pud_offset(pgd, 0UL);
>>>>>>> v4.9.227
	unsigned long addr;
	unsigned i;

	for (i = 0; i < PTRS_PER_PUD; i++, pud++) {
		addr = start + i * PUD_SIZE;
<<<<<<< HEAD
		if (pud_none(*pud) || pud_sect(*pud) || pud_bad(*pud))
			note_page(st, addr, 2, pud_val(*pud));
		else
			walk_pmd(st, pud, addr);
	}
}

static void walk_pgd(struct pg_state *st, struct mm_struct *mm, unsigned long start)
{
	pgd_t *pgd = pgd_offset(mm, 0);
=======
		if (pud_none(*pud) || pud_sect(*pud)) {
			note_page(st, addr, 2, pud_val(*pud));
		} else {
			BUG_ON(pud_bad(*pud));
			walk_pmd(st, pud, addr);
		}
	}
}

static void walk_pgd(struct pg_state *st, struct mm_struct *mm,
		     unsigned long start)
{
	pgd_t *pgd = pgd_offset(mm, 0UL);
>>>>>>> v4.9.227
	unsigned i;
	unsigned long addr;

	for (i = 0; i < PTRS_PER_PGD; i++, pgd++) {
		addr = start + i * PGDIR_SIZE;
<<<<<<< HEAD
		if (pgd_none(*pgd) || pgd_bad(*pgd))
			note_page(st, addr, 1, pgd_val(*pgd));
		else
			walk_pud(st, pgd, addr);
=======
		if (pgd_none(*pgd)) {
			note_page(st, addr, 1, pgd_val(*pgd));
		} else {
			BUG_ON(pgd_bad(*pgd));
			walk_pud(st, pgd, addr);
		}
>>>>>>> v4.9.227
	}
}

static int ptdump_show(struct seq_file *m, void *v)
{
<<<<<<< HEAD
	struct pg_state st = {
		.seq = m,
		.marker = address_markers,
	};

	walk_pgd(&st, &init_mm, LOWEST_ADDR);
=======
	struct ptdump_info *info = m->private;
	struct pg_state st = {
		.seq = m,
		.marker = info->markers,
	};

	walk_pgd(&st, info->mm, info->base_addr);
>>>>>>> v4.9.227

	note_page(&st, 0, 0, 0);
	return 0;
}

static int ptdump_open(struct inode *inode, struct file *file)
{
<<<<<<< HEAD
	return single_open(file, ptdump_show, NULL);
=======
	return single_open(file, ptdump_show, inode->i_private);
>>>>>>> v4.9.227
}

static const struct file_operations ptdump_fops = {
	.open		= ptdump_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

<<<<<<< HEAD
static int ptdump_init(void)
=======
int ptdump_register(struct ptdump_info *info, const char *name)
>>>>>>> v4.9.227
{
	struct dentry *pe;
	unsigned i, j;

	for (i = 0; i < ARRAY_SIZE(pg_level); i++)
		if (pg_level[i].bits)
			for (j = 0; j < pg_level[i].num; j++)
				pg_level[i].mask |= pg_level[i].bits[j].mask;

<<<<<<< HEAD
	address_markers[VMEMMAP_START_NR].start_address =
				(unsigned long)virt_to_page(PAGE_OFFSET);
	address_markers[VMEMMAP_END_NR].start_address =
				(unsigned long)virt_to_page(high_memory);

	pe = debugfs_create_file("kernel_page_tables", 0400, NULL, NULL,
				 &ptdump_fops);
	return pe ? 0 : -ENOMEM;
}
=======
	pe = debugfs_create_file(name, 0400, NULL, info, &ptdump_fops);
	return pe ? 0 : -ENOMEM;
}

static struct ptdump_info kernel_ptdump_info = {
	.mm		= &init_mm,
	.markers	= address_markers,
	.base_addr	= VA_START,
};

static int ptdump_init(void)
{
	return ptdump_register(&kernel_ptdump_info, "kernel_page_tables");
}
>>>>>>> v4.9.227
device_initcall(ptdump_init);
