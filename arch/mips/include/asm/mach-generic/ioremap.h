/*
 *	include/asm-mips/mach-generic/ioremap.h
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */
#ifndef __ASM_MACH_GENERIC_IOREMAP_H
#define __ASM_MACH_GENERIC_IOREMAP_H

#include <linux/types.h>

/*
 * Allow physical addresses to be fixed up to help peripherals located
 * outside the low 32-bit range -- generic pass-through version.
 */
<<<<<<< HEAD
static inline phys_t fixup_bigphys_addr(phys_t phys_addr, phys_t size)
=======
static inline phys_addr_t fixup_bigphys_addr(phys_addr_t phys_addr, phys_addr_t size)
>>>>>>> v4.9.227
{
	return phys_addr;
}

<<<<<<< HEAD
static inline void __iomem *plat_ioremap(phys_t offset, unsigned long size,
=======
static inline void __iomem *plat_ioremap(phys_addr_t offset, unsigned long size,
>>>>>>> v4.9.227
	unsigned long flags)
{
	return NULL;
}

static inline int plat_iounmap(const volatile void __iomem *addr)
{
	return 0;
}

#endif /* __ASM_MACH_GENERIC_IOREMAP_H */
