/*
 * include/asm-xtensa/io.h
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2001 - 2005 Tensilica Inc.
 */

#ifndef _XTENSA_IO_H
#define _XTENSA_IO_H

#ifdef __KERNEL__
#include <asm/byteorder.h>
#include <asm/page.h>
#include <asm/vectors.h>
#include <linux/bug.h>
#include <linux/kernel.h>

#include <linux/types.h>

#define IOADDR(x)		(XCHAL_KIO_BYPASS_VADDR + (x))
#define IO_SPACE_LIMIT ~0

#ifdef CONFIG_MMU

<<<<<<< HEAD
#if XCHAL_HAVE_PTP_MMU && XCHAL_HAVE_SPANNING_WAY && defined(CONFIG_OF)
extern unsigned long xtensa_kio_paddr;

static inline unsigned long xtensa_get_kio_paddr(void)
{
	return xtensa_kio_paddr;
}
#endif

/*
 * Return the virtual address for the specified bus memory.
 * Note that we currently don't support any address outside the KIO segment.
=======
void __iomem *xtensa_ioremap_nocache(unsigned long addr, unsigned long size);
void __iomem *xtensa_ioremap_cache(unsigned long addr, unsigned long size);
void xtensa_iounmap(volatile void __iomem *addr);

/*
 * Return the virtual address for the specified bus memory.
>>>>>>> v4.9.227
 */
static inline void __iomem *ioremap_nocache(unsigned long offset,
		unsigned long size)
{
	if (offset >= XCHAL_KIO_PADDR
	    && offset - XCHAL_KIO_PADDR < XCHAL_KIO_SIZE)
		return (void*)(offset-XCHAL_KIO_PADDR+XCHAL_KIO_BYPASS_VADDR);
	else
<<<<<<< HEAD
		BUG();
=======
		return xtensa_ioremap_nocache(offset, size);
>>>>>>> v4.9.227
}

static inline void __iomem *ioremap_cache(unsigned long offset,
		unsigned long size)
{
	if (offset >= XCHAL_KIO_PADDR
	    && offset - XCHAL_KIO_PADDR < XCHAL_KIO_SIZE)
		return (void*)(offset-XCHAL_KIO_PADDR+XCHAL_KIO_CACHED_VADDR);
	else
<<<<<<< HEAD
		BUG();
}

#define ioremap_wc ioremap_nocache
=======
		return xtensa_ioremap_cache(offset, size);
}
#define ioremap_cache ioremap_cache

#define ioremap_wc ioremap_nocache
#define ioremap_wt ioremap_nocache
>>>>>>> v4.9.227

static inline void __iomem *ioremap(unsigned long offset, unsigned long size)
{
	return ioremap_nocache(offset, size);
}

static inline void iounmap(volatile void __iomem *addr)
{
<<<<<<< HEAD
=======
	unsigned long va = (unsigned long) addr;

	if (!(va >= XCHAL_KIO_CACHED_VADDR &&
	      va - XCHAL_KIO_CACHED_VADDR < XCHAL_KIO_SIZE) &&
	    !(va >= XCHAL_KIO_BYPASS_VADDR &&
	      va - XCHAL_KIO_BYPASS_VADDR < XCHAL_KIO_SIZE))
		xtensa_iounmap(addr);
>>>>>>> v4.9.227
}

#define virt_to_bus     virt_to_phys
#define bus_to_virt     phys_to_virt

#endif /* CONFIG_MMU */

<<<<<<< HEAD
/*
 * Generic I/O
 */
#define readb_relaxed readb
#define readw_relaxed readw
#define readl_relaxed readl

=======
>>>>>>> v4.9.227
#endif	/* __KERNEL__ */

#include <asm-generic/io.h>

#endif	/* _XTENSA_IO_H */
