/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Numascale NumaConnect-Specific Header file
 *
 * Copyright (C) 2011 Numascale AS. All rights reserved.
 *
 * Send feedback to <support@numascale.com>
 *
 */

#ifndef _ASM_X86_NUMACHIP_NUMACHIP_CSR_H
#define _ASM_X86_NUMACHIP_NUMACHIP_CSR_H

<<<<<<< HEAD
#include <linux/numa.h>
#include <linux/percpu.h>
#include <linux/io.h>
#include <linux/swab.h>
#include <asm/types.h>
#include <asm/processor.h>
=======
#include <linux/smp.h>
#include <linux/io.h>
>>>>>>> v4.9.227

#define CSR_NODE_SHIFT		16
#define CSR_NODE_BITS(p)	(((unsigned long)(p)) << CSR_NODE_SHIFT)
#define CSR_NODE_MASK		0x0fff		/* 4K nodes */

/* 32K CSR space, b15 indicates geo/non-geo */
#define CSR_OFFSET_MASK	0x7fffUL
<<<<<<< HEAD

/* Global CSR space covers all 4K possible nodes with 64K CSR space per node */
#define NUMACHIP_GCSR_BASE	0x3fff00000000ULL
#define NUMACHIP_GCSR_LIM	0x3fff0fffffffULL
#define NUMACHIP_GCSR_SIZE	(NUMACHIP_GCSR_LIM - NUMACHIP_GCSR_BASE + 1)
=======
#define CSR_G0_NODE_IDS (0x008 + (0 << 12))
#define CSR_G3_EXT_IRQ_GEN (0x030 + (3 << 12))
>>>>>>> v4.9.227

/*
 * Local CSR space starts in global CSR space with "nodeid" = 0xfff0, however
 * when using the direct mapping on x86_64, both start and size needs to be
 * aligned with PMD_SIZE which is 2M
 */
#define NUMACHIP_LCSR_BASE	0x3ffffe000000ULL
#define NUMACHIP_LCSR_LIM	0x3fffffffffffULL
#define NUMACHIP_LCSR_SIZE	(NUMACHIP_LCSR_LIM - NUMACHIP_LCSR_BASE + 1)
<<<<<<< HEAD

static inline void *gcsr_address(int node, unsigned long offset)
{
	return __va(NUMACHIP_GCSR_BASE | (1UL << 15) |
		CSR_NODE_BITS(node & CSR_NODE_MASK) | (offset & CSR_OFFSET_MASK));
}
=======
#define NUMACHIP_LAPIC_BITS	8
>>>>>>> v4.9.227

static inline void *lcsr_address(unsigned long offset)
{
	return __va(NUMACHIP_LCSR_BASE | (1UL << 15) |
		CSR_NODE_BITS(0xfff0) | (offset & CSR_OFFSET_MASK));
}

<<<<<<< HEAD
static inline unsigned int read_gcsr(int node, unsigned long offset)
{
	return swab32(readl(gcsr_address(node, offset)));
}

static inline void write_gcsr(int node, unsigned long offset, unsigned int val)
{
	writel(swab32(val), gcsr_address(node, offset));
}

=======
>>>>>>> v4.9.227
static inline unsigned int read_lcsr(unsigned long offset)
{
	return swab32(readl(lcsr_address(offset)));
}

static inline void write_lcsr(unsigned long offset, unsigned int val)
{
	writel(swab32(val), lcsr_address(offset));
}

<<<<<<< HEAD
/* ========================================================================= */
/*                   CSR_G0_STATE_CLEAR                                      */
/* ========================================================================= */

#define CSR_G0_STATE_CLEAR (0x000 + (0 << 12))
union numachip_csr_g0_state_clear {
	unsigned int v;
	struct numachip_csr_g0_state_clear_s {
		unsigned int _state:2;
		unsigned int _rsvd_2_6:5;
		unsigned int _lost:1;
		unsigned int _rsvd_8_31:24;
	} s;
};

/* ========================================================================= */
/*                   CSR_G0_NODE_IDS                                         */
/* ========================================================================= */

#define CSR_G0_NODE_IDS (0x008 + (0 << 12))
union numachip_csr_g0_node_ids {
	unsigned int v;
	struct numachip_csr_g0_node_ids_s {
		unsigned int _initialid:16;
		unsigned int _nodeid:12;
		unsigned int _rsvd_28_31:4;
	} s;
};

/* ========================================================================= */
/*                   CSR_G3_EXT_IRQ_GEN                                      */
/* ========================================================================= */

#define CSR_G3_EXT_IRQ_GEN (0x030 + (3 << 12))
union numachip_csr_g3_ext_irq_gen {
	unsigned int v;
	struct numachip_csr_g3_ext_irq_gen_s {
		unsigned int _vector:8;
		unsigned int _msgtype:3;
		unsigned int _index:5;
		unsigned int _destination_apic_id:16;
	} s;
};

/* ========================================================================= */
/*                   CSR_G3_EXT_IRQ_STATUS                                   */
/* ========================================================================= */

#define CSR_G3_EXT_IRQ_STATUS (0x034 + (3 << 12))
union numachip_csr_g3_ext_irq_status {
	unsigned int v;
	struct numachip_csr_g3_ext_irq_status_s {
		unsigned int _result:32;
	} s;
};

/* ========================================================================= */
/*                   CSR_G3_EXT_IRQ_DEST                                     */
/* ========================================================================= */

#define CSR_G3_EXT_IRQ_DEST (0x038 + (3 << 12))
union numachip_csr_g3_ext_irq_dest {
	unsigned int v;
	struct numachip_csr_g3_ext_irq_dest_s {
		unsigned int _irq:8;
		unsigned int _rsvd_8_31:24;
	} s;
};

/* ========================================================================= */
/*                   CSR_G3_NC_ATT_MAP_SELECT                                */
/* ========================================================================= */

#define CSR_G3_NC_ATT_MAP_SELECT (0x7fc + (3 << 12))
union numachip_csr_g3_nc_att_map_select {
	unsigned int v;
	struct numachip_csr_g3_nc_att_map_select_s {
		unsigned int _upper_address_bits:4;
		unsigned int _select_ram:4;
		unsigned int _rsvd_8_31:24;
	} s;
};

/* ========================================================================= */
/*                   CSR_G3_NC_ATT_MAP_SELECT_0-255                          */
/* ========================================================================= */

#define CSR_G3_NC_ATT_MAP_SELECT_0 (0x800 + (3 << 12))

#endif /* _ASM_X86_NUMACHIP_NUMACHIP_CSR_H */

=======
/*
 * On NumaChip2, local CSR space is 16MB and starts at fixed offset below 4G
 */

#define NUMACHIP2_LCSR_BASE       0xf0000000UL
#define NUMACHIP2_LCSR_SIZE       0x1000000UL
#define NUMACHIP2_APIC_ICR        0x100000
#define NUMACHIP2_TIMER_DEADLINE  0x200000
#define NUMACHIP2_TIMER_INT       0x200008
#define NUMACHIP2_TIMER_NOW       0x200018
#define NUMACHIP2_TIMER_RESET     0x200020

static inline void __iomem *numachip2_lcsr_address(unsigned long offset)
{
	return (void __iomem *)__va(NUMACHIP2_LCSR_BASE |
		(offset & (NUMACHIP2_LCSR_SIZE - 1)));
}

static inline u32 numachip2_read32_lcsr(unsigned long offset)
{
	return readl(numachip2_lcsr_address(offset));
}

static inline u64 numachip2_read64_lcsr(unsigned long offset)
{
	return readq(numachip2_lcsr_address(offset));
}

static inline void numachip2_write32_lcsr(unsigned long offset, u32 val)
{
	writel(val, numachip2_lcsr_address(offset));
}

static inline void numachip2_write64_lcsr(unsigned long offset, u64 val)
{
	writeq(val, numachip2_lcsr_address(offset));
}

static inline unsigned int numachip2_timer(void)
{
	return (smp_processor_id() % 48) << 6;
}

#endif /* _ASM_X86_NUMACHIP_NUMACHIP_CSR_H */
>>>>>>> v4.9.227
