/*
 * Dump R4x00 TLB for debugging purposes.
 *
 * Copyright (C) 1994, 1995 by Waldorf Electronics, written by Ralf Baechle.
 * Copyright (C) 1999 by Silicon Graphics, Inc.
 */
#include <linux/kernel.h>
#include <linux/mm.h>

<<<<<<< HEAD
=======
#include <asm/hazards.h>
>>>>>>> v4.9.227
#include <asm/mipsregs.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/tlbdebug.h>

<<<<<<< HEAD
=======
void dump_tlb_regs(void)
{
	const int field = 2 * sizeof(unsigned long);

	pr_info("Index    : %0x\n", read_c0_index());
	pr_info("PageMask : %0x\n", read_c0_pagemask());
	if (cpu_has_guestid)
		pr_info("GuestCtl1: %0x\n", read_c0_guestctl1());
	pr_info("EntryHi  : %0*lx\n", field, read_c0_entryhi());
	pr_info("EntryLo0 : %0*lx\n", field, read_c0_entrylo0());
	pr_info("EntryLo1 : %0*lx\n", field, read_c0_entrylo1());
	pr_info("Wired    : %0x\n", read_c0_wired());
	switch (current_cpu_type()) {
	case CPU_R10000:
	case CPU_R12000:
	case CPU_R14000:
	case CPU_R16000:
		pr_info("FrameMask: %0x\n", read_c0_framemask());
		break;
	}
	if (cpu_has_small_pages || cpu_has_rixi || cpu_has_xpa)
		pr_info("PageGrain: %0x\n", read_c0_pagegrain());
	if (cpu_has_htw) {
		pr_info("PWField  : %0*lx\n", field, read_c0_pwfield());
		pr_info("PWSize   : %0*lx\n", field, read_c0_pwsize());
		pr_info("PWCtl    : %0x\n", read_c0_pwctl());
	}
}

>>>>>>> v4.9.227
static inline const char *msk2str(unsigned int mask)
{
	switch (mask) {
	case PM_4K:	return "4kb";
	case PM_16K:	return "16kb";
	case PM_64K:	return "64kb";
	case PM_256K:	return "256kb";
#ifdef CONFIG_CPU_CAVIUM_OCTEON
	case PM_8K:	return "8kb";
	case PM_32K:	return "32kb";
	case PM_128K:	return "128kb";
	case PM_512K:	return "512kb";
	case PM_2M:	return "2Mb";
	case PM_8M:	return "8Mb";
	case PM_32M:	return "32Mb";
#endif
#ifndef CONFIG_CPU_VR41XX
	case PM_1M:	return "1Mb";
	case PM_4M:	return "4Mb";
	case PM_16M:	return "16Mb";
	case PM_64M:	return "64Mb";
	case PM_256M:	return "256Mb";
	case PM_1G:	return "1Gb";
#endif
	}
	return "";
}

<<<<<<< HEAD
#define BARRIER()					\
	__asm__ __volatile__(				\
		".set\tnoreorder\n\t"			\
		"nop;nop;nop;nop;nop;nop;nop\n\t"	\
		".set\treorder");

static void dump_tlb(int first, int last)
{
	unsigned long s_entryhi, entryhi, asid;
	unsigned long long entrylo0, entrylo1;
	unsigned int s_index, s_pagemask, pagemask, c0, c1, i;
=======
static void dump_tlb(int first, int last)
{
	unsigned long s_entryhi, entryhi, asid;
	unsigned long long entrylo0, entrylo1, pa;
	unsigned int s_index, s_pagemask, s_guestctl1 = 0;
	unsigned int pagemask, guestctl1 = 0, c0, c1, i;
	unsigned long asidmask = cpu_asid_mask(&current_cpu_data);
	int asidwidth = DIV_ROUND_UP(ilog2(asidmask) + 1, 4);
#ifdef CONFIG_32BIT
	bool xpa = cpu_has_xpa && (read_c0_pagegrain() & PG_ELPA);
	int pwidth = xpa ? 11 : 8;
	int vwidth = 8;
#else
	bool xpa = false;
	int pwidth = 11;
	int vwidth = 11;
#endif
>>>>>>> v4.9.227

	s_pagemask = read_c0_pagemask();
	s_entryhi = read_c0_entryhi();
	s_index = read_c0_index();
<<<<<<< HEAD
	asid = s_entryhi & 0xff;

	for (i = first; i <= last; i++) {
		write_c0_index(i);
		BARRIER();
		tlb_read();
		BARRIER();
=======
	asid = s_entryhi & asidmask;
	if (cpu_has_guestid)
		s_guestctl1 = read_c0_guestctl1();

	for (i = first; i <= last; i++) {
		write_c0_index(i);
		mtc0_tlbr_hazard();
		tlb_read();
		tlb_read_hazard();
>>>>>>> v4.9.227
		pagemask = read_c0_pagemask();
		entryhi	 = read_c0_entryhi();
		entrylo0 = read_c0_entrylo0();
		entrylo1 = read_c0_entrylo1();
<<<<<<< HEAD

		/* Unused entries have a virtual address of CKSEG0.  */
		if ((entryhi & ~0x1ffffUL) != CKSEG0
		    && (entryhi & 0xff) == asid) {
#ifdef CONFIG_32BIT
			int width = 8;
#else
			int width = 11;
#endif
			/*
			 * Only print entries in use
			 */
			printk("Index: %2d pgmask=%s ", i, msk2str(pagemask));

			c0 = (entrylo0 >> 3) & 7;
			c1 = (entrylo1 >> 3) & 7;

			printk("va=%0*lx asid=%02lx\n",
			       width, (entryhi & ~0x1fffUL),
			       entryhi & 0xff);
			printk("\t[pa=%0*llx c=%d d=%d v=%d g=%d] ",
			       width,
			       (entrylo0 << 6) & PAGE_MASK, c0,
			       (entrylo0 & 4) ? 1 : 0,
			       (entrylo0 & 2) ? 1 : 0,
			       (entrylo0 & 1) ? 1 : 0);
			printk("[pa=%0*llx c=%d d=%d v=%d g=%d]\n",
			       width,
			       (entrylo1 << 6) & PAGE_MASK, c1,
			       (entrylo1 & 4) ? 1 : 0,
			       (entrylo1 & 2) ? 1 : 0,
			       (entrylo1 & 1) ? 1 : 0);
		}
=======
		if (cpu_has_guestid)
			guestctl1 = read_c0_guestctl1();

		/* EHINV bit marks entire entry as invalid */
		if (cpu_has_tlbinv && entryhi & MIPS_ENTRYHI_EHINV)
			continue;
		/*
		 * Prior to tlbinv, unused entries have a virtual address of
		 * CKSEG0.
		 */
		if ((entryhi & ~0x1ffffUL) == CKSEG0)
			continue;
		/*
		 * ASID takes effect in absence of G (global) bit.
		 * We check both G bits, even though architecturally they should
		 * match one another, because some revisions of the SB1 core may
		 * leave only a single G bit set after a machine check exception
		 * due to duplicate TLB entry.
		 */
		if (!((entrylo0 | entrylo1) & ENTRYLO_G) &&
		    (entryhi & asidmask) != asid)
			continue;

		/*
		 * Only print entries in use
		 */
		printk("Index: %2d pgmask=%s ", i, msk2str(pagemask));

		c0 = (entrylo0 & ENTRYLO_C) >> ENTRYLO_C_SHIFT;
		c1 = (entrylo1 & ENTRYLO_C) >> ENTRYLO_C_SHIFT;

		pr_cont("va=%0*lx asid=%0*lx",
			vwidth, (entryhi & ~0x1fffUL),
			asidwidth, entryhi & asidmask);
		if (cpu_has_guestid)
			pr_cont(" gid=%02lx",
				(guestctl1 & MIPS_GCTL1_RID)
					>> MIPS_GCTL1_RID_SHIFT);
		/* RI/XI are in awkward places, so mask them off separately */
		pa = entrylo0 & ~(MIPS_ENTRYLO_RI | MIPS_ENTRYLO_XI);
		if (xpa)
			pa |= (unsigned long long)readx_c0_entrylo0() << 30;
		pa = (pa << 6) & PAGE_MASK;
		pr_cont("\n\t[");
		if (cpu_has_rixi)
			pr_cont("ri=%d xi=%d ",
				(entrylo0 & MIPS_ENTRYLO_RI) ? 1 : 0,
				(entrylo0 & MIPS_ENTRYLO_XI) ? 1 : 0);
		pr_cont("pa=%0*llx c=%d d=%d v=%d g=%d] [",
			pwidth, pa, c0,
			(entrylo0 & ENTRYLO_D) ? 1 : 0,
			(entrylo0 & ENTRYLO_V) ? 1 : 0,
			(entrylo0 & ENTRYLO_G) ? 1 : 0);
		/* RI/XI are in awkward places, so mask them off separately */
		pa = entrylo1 & ~(MIPS_ENTRYLO_RI | MIPS_ENTRYLO_XI);
		if (xpa)
			pa |= (unsigned long long)readx_c0_entrylo1() << 30;
		pa = (pa << 6) & PAGE_MASK;
		if (cpu_has_rixi)
			pr_cont("ri=%d xi=%d ",
				(entrylo1 & MIPS_ENTRYLO_RI) ? 1 : 0,
				(entrylo1 & MIPS_ENTRYLO_XI) ? 1 : 0);
		pr_cont("pa=%0*llx c=%d d=%d v=%d g=%d]\n",
			pwidth, pa, c1,
			(entrylo1 & ENTRYLO_D) ? 1 : 0,
			(entrylo1 & ENTRYLO_V) ? 1 : 0,
			(entrylo1 & ENTRYLO_G) ? 1 : 0);
>>>>>>> v4.9.227
	}
	printk("\n");

	write_c0_entryhi(s_entryhi);
	write_c0_index(s_index);
	write_c0_pagemask(s_pagemask);
<<<<<<< HEAD
=======
	if (cpu_has_guestid)
		write_c0_guestctl1(s_guestctl1);
>>>>>>> v4.9.227
}

void dump_tlb_all(void)
{
	dump_tlb(0, current_cpu_data.tlbsize - 1);
}
