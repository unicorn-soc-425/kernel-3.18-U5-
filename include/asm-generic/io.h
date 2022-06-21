/* Generic I/O port emulation, based on MN10300 code
 *
 * Copyright (C) 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */
#ifndef __ASM_GENERIC_IO_H
#define __ASM_GENERIC_IO_H

#include <asm/page.h> /* I/O is all done through memory accesses */
<<<<<<< HEAD
=======
#include <linux/string.h> /* for memset() and memcpy() */
>>>>>>> v4.9.227
#include <linux/types.h>

#ifdef CONFIG_GENERIC_IOMAP
#include <asm-generic/iomap.h>
#endif

#include <asm-generic/pci_iomap.h>

#ifndef mmiowb
#define mmiowb() do {} while (0)
#endif

<<<<<<< HEAD
/*****************************************************************************/
/*
 * readX/writeX() are used to access memory mapped devices. On some
 * architectures the memory mapped IO stuff needs to be accessed
 * differently. On the simple architectures, we just read/write the
 * memory location directly.
 */
#ifndef __raw_readb
static inline u8 __raw_readb(const volatile void __iomem *addr)
{
	return *(const volatile u8 __force *) addr;
=======
/*
 * __raw_{read,write}{b,w,l,q}() access memory in native endianness.
 *
 * On some architectures memory mapped IO needs to be accessed differently.
 * On the simple architectures, we just read/write the memory location
 * directly.
 */

#ifndef __raw_readb
#define __raw_readb __raw_readb
static inline u8 __raw_readb(const volatile void __iomem *addr)
{
	return *(const volatile u8 __force *)addr;
>>>>>>> v4.9.227
}
#endif

#ifndef __raw_readw
<<<<<<< HEAD
static inline u16 __raw_readw(const volatile void __iomem *addr)
{
	return *(const volatile u16 __force *) addr;
=======
#define __raw_readw __raw_readw
static inline u16 __raw_readw(const volatile void __iomem *addr)
{
	return *(const volatile u16 __force *)addr;
>>>>>>> v4.9.227
}
#endif

#ifndef __raw_readl
<<<<<<< HEAD
static inline u32 __raw_readl(const volatile void __iomem *addr)
{
	return *(const volatile u32 __force *) addr;
}
#endif

#define readb __raw_readb

=======
#define __raw_readl __raw_readl
static inline u32 __raw_readl(const volatile void __iomem *addr)
{
	return *(const volatile u32 __force *)addr;
}
#endif

#ifdef CONFIG_64BIT
#ifndef __raw_readq
#define __raw_readq __raw_readq
static inline u64 __raw_readq(const volatile void __iomem *addr)
{
	return *(const volatile u64 __force *)addr;
}
#endif
#endif /* CONFIG_64BIT */

#ifndef __raw_writeb
#define __raw_writeb __raw_writeb
static inline void __raw_writeb(u8 value, volatile void __iomem *addr)
{
	*(volatile u8 __force *)addr = value;
}
#endif

#ifndef __raw_writew
#define __raw_writew __raw_writew
static inline void __raw_writew(u16 value, volatile void __iomem *addr)
{
	*(volatile u16 __force *)addr = value;
}
#endif

#ifndef __raw_writel
#define __raw_writel __raw_writel
static inline void __raw_writel(u32 value, volatile void __iomem *addr)
{
	*(volatile u32 __force *)addr = value;
}
#endif

#ifdef CONFIG_64BIT
#ifndef __raw_writeq
#define __raw_writeq __raw_writeq
static inline void __raw_writeq(u64 value, volatile void __iomem *addr)
{
	*(volatile u64 __force *)addr = value;
}
#endif
#endif /* CONFIG_64BIT */

/*
 * {read,write}{b,w,l,q}() access little endian memory and return result in
 * native endianness.
 */

#ifndef readb
#define readb readb
static inline u8 readb(const volatile void __iomem *addr)
{
	return __raw_readb(addr);
}
#endif

#ifndef readw
>>>>>>> v4.9.227
#define readw readw
static inline u16 readw(const volatile void __iomem *addr)
{
	return __le16_to_cpu(__raw_readw(addr));
}
<<<<<<< HEAD

=======
#endif

#ifndef readl
>>>>>>> v4.9.227
#define readl readl
static inline u32 readl(const volatile void __iomem *addr)
{
	return __le32_to_cpu(__raw_readl(addr));
}
<<<<<<< HEAD

#ifndef __raw_writeb
static inline void __raw_writeb(u8 b, volatile void __iomem *addr)
{
	*(volatile u8 __force *) addr = b;
}
#endif

#ifndef __raw_writew
static inline void __raw_writew(u16 b, volatile void __iomem *addr)
{
	*(volatile u16 __force *) addr = b;
}
#endif

#ifndef __raw_writel
static inline void __raw_writel(u32 b, volatile void __iomem *addr)
{
	*(volatile u32 __force *) addr = b;
}
#endif

#define writeb __raw_writeb
#define writew(b,addr) __raw_writew(__cpu_to_le16(b),addr)
#define writel(b,addr) __raw_writel(__cpu_to_le32(b),addr)

#ifdef CONFIG_64BIT
#ifndef __raw_readq
static inline u64 __raw_readq(const volatile void __iomem *addr)
{
	return *(const volatile u64 __force *) addr;
}
#endif

=======
#endif

#ifdef CONFIG_64BIT
#ifndef readq
>>>>>>> v4.9.227
#define readq readq
static inline u64 readq(const volatile void __iomem *addr)
{
	return __le64_to_cpu(__raw_readq(addr));
}
<<<<<<< HEAD

#ifndef __raw_writeq
static inline void __raw_writeq(u64 b, volatile void __iomem *addr)
{
	*(volatile u64 __force *) addr = b;
}
#endif

#define writeq(b, addr) __raw_writeq(__cpu_to_le64(b), addr)
#endif /* CONFIG_64BIT */

#ifndef PCI_IOBASE
#define PCI_IOBASE ((void __iomem *) 0)
#endif

/*****************************************************************************/
/*
 * traditional input/output functions
 */

static inline u8 inb(unsigned long addr)
{
	return readb(addr + PCI_IOBASE);
}

static inline u16 inw(unsigned long addr)
{
	return readw(addr + PCI_IOBASE);
}

static inline u32 inl(unsigned long addr)
{
	return readl(addr + PCI_IOBASE);
}

static inline void outb(u8 b, unsigned long addr)
{
	writeb(b, addr + PCI_IOBASE);
}

static inline void outw(u16 b, unsigned long addr)
{
	writew(b, addr + PCI_IOBASE);
}

static inline void outl(u32 b, unsigned long addr)
{
	writel(b, addr + PCI_IOBASE);
}

#define inb_p(addr)	inb(addr)
#define inw_p(addr)	inw(addr)
#define inl_p(addr)	inl(addr)
#define outb_p(x, addr)	outb((x), (addr))
#define outw_p(x, addr)	outw((x), (addr))
#define outl_p(x, addr)	outl((x), (addr))

#ifndef insb
static inline void insb(unsigned long addr, void *buffer, int count)
{
	if (count) {
		u8 *buf = buffer;
		do {
			u8 x = __raw_readb(addr + PCI_IOBASE);
=======
#endif
#endif /* CONFIG_64BIT */

#ifndef writeb
#define writeb writeb
static inline void writeb(u8 value, volatile void __iomem *addr)
{
	__raw_writeb(value, addr);
}
#endif

#ifndef writew
#define writew writew
static inline void writew(u16 value, volatile void __iomem *addr)
{
	__raw_writew(cpu_to_le16(value), addr);
}
#endif

#ifndef writel
#define writel writel
static inline void writel(u32 value, volatile void __iomem *addr)
{
	__raw_writel(__cpu_to_le32(value), addr);
}
#endif

#ifdef CONFIG_64BIT
#ifndef writeq
#define writeq writeq
static inline void writeq(u64 value, volatile void __iomem *addr)
{
	__raw_writeq(__cpu_to_le64(value), addr);
}
#endif
#endif /* CONFIG_64BIT */

/*
 * {read,write}{b,w,l,q}_relaxed() are like the regular version, but
 * are not guaranteed to provide ordering against spinlocks or memory
 * accesses.
 */
#ifndef readb_relaxed
#define readb_relaxed readb
#endif

#ifndef readw_relaxed
#define readw_relaxed readw
#endif

#ifndef readl_relaxed
#define readl_relaxed readl
#endif

#if defined(readq) && !defined(readq_relaxed)
#define readq_relaxed readq
#endif

#ifndef writeb_relaxed
#define writeb_relaxed writeb
#endif

#ifndef writew_relaxed
#define writew_relaxed writew
#endif

#ifndef writel_relaxed
#define writel_relaxed writel
#endif

#if defined(writeq) && !defined(writeq_relaxed)
#define writeq_relaxed writeq
#endif

/*
 * {read,write}s{b,w,l,q}() repeatedly access the same memory address in
 * native endianness in 8-, 16-, 32- or 64-bit chunks (@count times).
 */
#ifndef readsb
#define readsb readsb
static inline void readsb(const volatile void __iomem *addr, void *buffer,
			  unsigned int count)
{
	if (count) {
		u8 *buf = buffer;

		do {
			u8 x = __raw_readb(addr);
>>>>>>> v4.9.227
			*buf++ = x;
		} while (--count);
	}
}
#endif

<<<<<<< HEAD
#ifndef insw
static inline void insw(unsigned long addr, void *buffer, int count)
{
	if (count) {
		u16 *buf = buffer;
		do {
			u16 x = __raw_readw(addr + PCI_IOBASE);
=======
#ifndef readsw
#define readsw readsw
static inline void readsw(const volatile void __iomem *addr, void *buffer,
			  unsigned int count)
{
	if (count) {
		u16 *buf = buffer;

		do {
			u16 x = __raw_readw(addr);
>>>>>>> v4.9.227
			*buf++ = x;
		} while (--count);
	}
}
#endif

<<<<<<< HEAD
#ifndef insl
static inline void insl(unsigned long addr, void *buffer, int count)
{
	if (count) {
		u32 *buf = buffer;
		do {
			u32 x = __raw_readl(addr + PCI_IOBASE);
=======
#ifndef readsl
#define readsl readsl
static inline void readsl(const volatile void __iomem *addr, void *buffer,
			  unsigned int count)
{
	if (count) {
		u32 *buf = buffer;

		do {
			u32 x = __raw_readl(addr);
>>>>>>> v4.9.227
			*buf++ = x;
		} while (--count);
	}
}
#endif

<<<<<<< HEAD
#ifndef outsb
static inline void outsb(unsigned long addr, const void *buffer, int count)
{
	if (count) {
		const u8 *buf = buffer;
		do {
			__raw_writeb(*buf++, addr + PCI_IOBASE);
=======
#ifdef CONFIG_64BIT
#ifndef readsq
#define readsq readsq
static inline void readsq(const volatile void __iomem *addr, void *buffer,
			  unsigned int count)
{
	if (count) {
		u64 *buf = buffer;

		do {
			u64 x = __raw_readq(addr);
			*buf++ = x;
		} while (--count);
	}
}
#endif
#endif /* CONFIG_64BIT */

#ifndef writesb
#define writesb writesb
static inline void writesb(volatile void __iomem *addr, const void *buffer,
			   unsigned int count)
{
	if (count) {
		const u8 *buf = buffer;

		do {
			__raw_writeb(*buf++, addr);
>>>>>>> v4.9.227
		} while (--count);
	}
}
#endif

<<<<<<< HEAD
#ifndef outsw
static inline void outsw(unsigned long addr, const void *buffer, int count)
{
	if (count) {
		const u16 *buf = buffer;
		do {
			__raw_writew(*buf++, addr + PCI_IOBASE);
=======
#ifndef writesw
#define writesw writesw
static inline void writesw(volatile void __iomem *addr, const void *buffer,
			   unsigned int count)
{
	if (count) {
		const u16 *buf = buffer;

		do {
			__raw_writew(*buf++, addr);
>>>>>>> v4.9.227
		} while (--count);
	}
}
#endif

<<<<<<< HEAD
#ifndef outsl
static inline void outsl(unsigned long addr, const void *buffer, int count)
{
	if (count) {
		const u32 *buf = buffer;
		do {
			__raw_writel(*buf++, addr + PCI_IOBASE);
=======
#ifndef writesl
#define writesl writesl
static inline void writesl(volatile void __iomem *addr, const void *buffer,
			   unsigned int count)
{
	if (count) {
		const u32 *buf = buffer;

		do {
			__raw_writel(*buf++, addr);
>>>>>>> v4.9.227
		} while (--count);
	}
}
#endif

<<<<<<< HEAD
#ifndef CONFIG_GENERIC_IOMAP
#define ioread8(addr)		readb(addr)
#define ioread16(addr)		readw(addr)
#define ioread16be(addr)	__be16_to_cpu(__raw_readw(addr))
#define ioread32(addr)		readl(addr)
#define ioread32be(addr)	__be32_to_cpu(__raw_readl(addr))

#define iowrite8(v, addr)	writeb((v), (addr))
#define iowrite16(v, addr)	writew((v), (addr))
#define iowrite16be(v, addr)	__raw_writew(__cpu_to_be16(v), addr)
#define iowrite32(v, addr)	writel((v), (addr))
#define iowrite32be(v, addr)	__raw_writel(__cpu_to_be32(v), addr)

#define ioread8_rep(p, dst, count) \
	insb((unsigned long) (p), (dst), (count))
#define ioread16_rep(p, dst, count) \
	insw((unsigned long) (p), (dst), (count))
#define ioread32_rep(p, dst, count) \
	insl((unsigned long) (p), (dst), (count))

#define iowrite8_rep(p, src, count) \
	outsb((unsigned long) (p), (src), (count))
#define iowrite16_rep(p, src, count) \
	outsw((unsigned long) (p), (src), (count))
#define iowrite32_rep(p, src, count) \
	outsl((unsigned long) (p), (src), (count))
#endif /* CONFIG_GENERIC_IOMAP */
=======
#ifdef CONFIG_64BIT
#ifndef writesq
#define writesq writesq
static inline void writesq(volatile void __iomem *addr, const void *buffer,
			   unsigned int count)
{
	if (count) {
		const u64 *buf = buffer;

		do {
			__raw_writeq(*buf++, addr);
		} while (--count);
	}
}
#endif
#endif /* CONFIG_64BIT */

#ifndef PCI_IOBASE
#define PCI_IOBASE ((void __iomem *)0)
#endif
>>>>>>> v4.9.227

#ifndef IO_SPACE_LIMIT
#define IO_SPACE_LIMIT 0xffff
#endif

<<<<<<< HEAD
#ifdef __KERNEL__

#include <linux/vmalloc.h>
#define __io_virt(x) ((void __force *) (x))
=======
/*
 * {in,out}{b,w,l}() access little endian I/O. {in,out}{b,w,l}_p() can be
 * implemented on hardware that needs an additional delay for I/O accesses to
 * take effect.
 */

#ifndef inb
#define inb inb
static inline u8 inb(unsigned long addr)
{
	return readb(PCI_IOBASE + addr);
}
#endif

#ifndef inw
#define inw inw
static inline u16 inw(unsigned long addr)
{
	return readw(PCI_IOBASE + addr);
}
#endif

#ifndef inl
#define inl inl
static inline u32 inl(unsigned long addr)
{
	return readl(PCI_IOBASE + addr);
}
#endif

#ifndef outb
#define outb outb
static inline void outb(u8 value, unsigned long addr)
{
	writeb(value, PCI_IOBASE + addr);
}
#endif

#ifndef outw
#define outw outw
static inline void outw(u16 value, unsigned long addr)
{
	writew(value, PCI_IOBASE + addr);
}
#endif

#ifndef outl
#define outl outl
static inline void outl(u32 value, unsigned long addr)
{
	writel(value, PCI_IOBASE + addr);
}
#endif

#ifndef inb_p
#define inb_p inb_p
static inline u8 inb_p(unsigned long addr)
{
	return inb(addr);
}
#endif

#ifndef inw_p
#define inw_p inw_p
static inline u16 inw_p(unsigned long addr)
{
	return inw(addr);
}
#endif

#ifndef inl_p
#define inl_p inl_p
static inline u32 inl_p(unsigned long addr)
{
	return inl(addr);
}
#endif

#ifndef outb_p
#define outb_p outb_p
static inline void outb_p(u8 value, unsigned long addr)
{
	outb(value, addr);
}
#endif

#ifndef outw_p
#define outw_p outw_p
static inline void outw_p(u16 value, unsigned long addr)
{
	outw(value, addr);
}
#endif

#ifndef outl_p
#define outl_p outl_p
static inline void outl_p(u32 value, unsigned long addr)
{
	outl(value, addr);
}
#endif

/*
 * {in,out}s{b,w,l}{,_p}() are variants of the above that repeatedly access a
 * single I/O port multiple times.
 */

#ifndef insb
#define insb insb
static inline void insb(unsigned long addr, void *buffer, unsigned int count)
{
	readsb(PCI_IOBASE + addr, buffer, count);
}
#endif

#ifndef insw
#define insw insw
static inline void insw(unsigned long addr, void *buffer, unsigned int count)
{
	readsw(PCI_IOBASE + addr, buffer, count);
}
#endif

#ifndef insl
#define insl insl
static inline void insl(unsigned long addr, void *buffer, unsigned int count)
{
	readsl(PCI_IOBASE + addr, buffer, count);
}
#endif

#ifndef outsb
#define outsb outsb
static inline void outsb(unsigned long addr, const void *buffer,
			 unsigned int count)
{
	writesb(PCI_IOBASE + addr, buffer, count);
}
#endif

#ifndef outsw
#define outsw outsw
static inline void outsw(unsigned long addr, const void *buffer,
			 unsigned int count)
{
	writesw(PCI_IOBASE + addr, buffer, count);
}
#endif

#ifndef outsl
#define outsl outsl
static inline void outsl(unsigned long addr, const void *buffer,
			 unsigned int count)
{
	writesl(PCI_IOBASE + addr, buffer, count);
}
#endif

#ifndef insb_p
#define insb_p insb_p
static inline void insb_p(unsigned long addr, void *buffer, unsigned int count)
{
	insb(addr, buffer, count);
}
#endif

#ifndef insw_p
#define insw_p insw_p
static inline void insw_p(unsigned long addr, void *buffer, unsigned int count)
{
	insw(addr, buffer, count);
}
#endif

#ifndef insl_p
#define insl_p insl_p
static inline void insl_p(unsigned long addr, void *buffer, unsigned int count)
{
	insl(addr, buffer, count);
}
#endif

#ifndef outsb_p
#define outsb_p outsb_p
static inline void outsb_p(unsigned long addr, const void *buffer,
			   unsigned int count)
{
	outsb(addr, buffer, count);
}
#endif

#ifndef outsw_p
#define outsw_p outsw_p
static inline void outsw_p(unsigned long addr, const void *buffer,
			   unsigned int count)
{
	outsw(addr, buffer, count);
}
#endif

#ifndef outsl_p
#define outsl_p outsl_p
static inline void outsl_p(unsigned long addr, const void *buffer,
			   unsigned int count)
{
	outsl(addr, buffer, count);
}
#endif

#ifndef CONFIG_GENERIC_IOMAP
#ifndef ioread8
#define ioread8 ioread8
static inline u8 ioread8(const volatile void __iomem *addr)
{
	return readb(addr);
}
#endif

#ifndef ioread16
#define ioread16 ioread16
static inline u16 ioread16(const volatile void __iomem *addr)
{
	return readw(addr);
}
#endif

#ifndef ioread32
#define ioread32 ioread32
static inline u32 ioread32(const volatile void __iomem *addr)
{
	return readl(addr);
}
#endif

#ifdef CONFIG_64BIT
#ifndef ioread64
#define ioread64 ioread64
static inline u64 ioread64(const volatile void __iomem *addr)
{
	return readq(addr);
}
#endif
#endif /* CONFIG_64BIT */

#ifndef iowrite8
#define iowrite8 iowrite8
static inline void iowrite8(u8 value, volatile void __iomem *addr)
{
	writeb(value, addr);
}
#endif

#ifndef iowrite16
#define iowrite16 iowrite16
static inline void iowrite16(u16 value, volatile void __iomem *addr)
{
	writew(value, addr);
}
#endif

#ifndef iowrite32
#define iowrite32 iowrite32
static inline void iowrite32(u32 value, volatile void __iomem *addr)
{
	writel(value, addr);
}
#endif

#ifdef CONFIG_64BIT
#ifndef iowrite64
#define iowrite64 iowrite64
static inline void iowrite64(u64 value, volatile void __iomem *addr)
{
	writeq(value, addr);
}
#endif
#endif /* CONFIG_64BIT */

#ifndef ioread16be
#define ioread16be ioread16be
static inline u16 ioread16be(const volatile void __iomem *addr)
{
	return swab16(readw(addr));
}
#endif

#ifndef ioread32be
#define ioread32be ioread32be
static inline u32 ioread32be(const volatile void __iomem *addr)
{
	return swab32(readl(addr));
}
#endif

#ifdef CONFIG_64BIT
#ifndef ioread64be
#define ioread64be ioread64be
static inline u64 ioread64be(const volatile void __iomem *addr)
{
	return swab64(readq(addr));
}
#endif
#endif /* CONFIG_64BIT */

#ifndef iowrite16be
#define iowrite16be iowrite16be
static inline void iowrite16be(u16 value, void volatile __iomem *addr)
{
	writew(swab16(value), addr);
}
#endif

#ifndef iowrite32be
#define iowrite32be iowrite32be
static inline void iowrite32be(u32 value, volatile void __iomem *addr)
{
	writel(swab32(value), addr);
}
#endif

#ifdef CONFIG_64BIT
#ifndef iowrite64be
#define iowrite64be iowrite64be
static inline void iowrite64be(u64 value, volatile void __iomem *addr)
{
	writeq(swab64(value), addr);
}
#endif
#endif /* CONFIG_64BIT */

#ifndef ioread8_rep
#define ioread8_rep ioread8_rep
static inline void ioread8_rep(const volatile void __iomem *addr, void *buffer,
			       unsigned int count)
{
	readsb(addr, buffer, count);
}
#endif

#ifndef ioread16_rep
#define ioread16_rep ioread16_rep
static inline void ioread16_rep(const volatile void __iomem *addr,
				void *buffer, unsigned int count)
{
	readsw(addr, buffer, count);
}
#endif

#ifndef ioread32_rep
#define ioread32_rep ioread32_rep
static inline void ioread32_rep(const volatile void __iomem *addr,
				void *buffer, unsigned int count)
{
	readsl(addr, buffer, count);
}
#endif

#ifdef CONFIG_64BIT
#ifndef ioread64_rep
#define ioread64_rep ioread64_rep
static inline void ioread64_rep(const volatile void __iomem *addr,
				void *buffer, unsigned int count)
{
	readsq(addr, buffer, count);
}
#endif
#endif /* CONFIG_64BIT */

#ifndef iowrite8_rep
#define iowrite8_rep iowrite8_rep
static inline void iowrite8_rep(volatile void __iomem *addr,
				const void *buffer,
				unsigned int count)
{
	writesb(addr, buffer, count);
}
#endif

#ifndef iowrite16_rep
#define iowrite16_rep iowrite16_rep
static inline void iowrite16_rep(volatile void __iomem *addr,
				 const void *buffer,
				 unsigned int count)
{
	writesw(addr, buffer, count);
}
#endif

#ifndef iowrite32_rep
#define iowrite32_rep iowrite32_rep
static inline void iowrite32_rep(volatile void __iomem *addr,
				 const void *buffer,
				 unsigned int count)
{
	writesl(addr, buffer, count);
}
#endif

#ifdef CONFIG_64BIT
#ifndef iowrite64_rep
#define iowrite64_rep iowrite64_rep
static inline void iowrite64_rep(volatile void __iomem *addr,
				 const void *buffer,
				 unsigned int count)
{
	writesq(addr, buffer, count);
}
#endif
#endif /* CONFIG_64BIT */
#endif /* CONFIG_GENERIC_IOMAP */

#ifdef __KERNEL__

#include <linux/vmalloc.h>
#define __io_virt(x) ((void __force *)(x))
>>>>>>> v4.9.227

#ifndef CONFIG_GENERIC_IOMAP
struct pci_dev;
extern void __iomem *pci_iomap(struct pci_dev *dev, int bar, unsigned long max);

#ifndef pci_iounmap
<<<<<<< HEAD
=======
#define pci_iounmap pci_iounmap
>>>>>>> v4.9.227
static inline void pci_iounmap(struct pci_dev *dev, void __iomem *p)
{
}
#endif
#endif /* CONFIG_GENERIC_IOMAP */

/*
 * Change virtual addresses to physical addresses and vv.
 * These are pretty trivial
 */
#ifndef virt_to_phys
<<<<<<< HEAD
=======
#define virt_to_phys virt_to_phys
>>>>>>> v4.9.227
static inline unsigned long virt_to_phys(volatile void *address)
{
	return __pa((unsigned long)address);
}
<<<<<<< HEAD

=======
#endif

#ifndef phys_to_virt
#define phys_to_virt phys_to_virt
>>>>>>> v4.9.227
static inline void *phys_to_virt(unsigned long address)
{
	return __va(address);
}
#endif

<<<<<<< HEAD
=======
/**
 * DOC: ioremap() and ioremap_*() variants
 *
 * If you have an IOMMU your architecture is expected to have both ioremap()
 * and iounmap() implemented otherwise the asm-generic helpers will provide a
 * direct mapping.
 *
 * There are ioremap_*() call variants, if you have no IOMMU we naturally will
 * default to direct mapping for all of them, you can override these defaults.
 * If you have an IOMMU you are highly encouraged to provide your own
 * ioremap variant implementation as there currently is no safe architecture
 * agnostic default. To avoid possible improper behaviour default asm-generic
 * ioremap_*() variants all return NULL when an IOMMU is available. If you've
 * defined your own ioremap_*() variant you must then declare your own
 * ioremap_*() variant as defined to itself to avoid the default NULL return.
 */

#ifdef CONFIG_MMU

#ifndef ioremap_uc
#define ioremap_uc ioremap_uc
static inline void __iomem *ioremap_uc(phys_addr_t offset, size_t size)
{
	return NULL;
}
#endif

#else /* !CONFIG_MMU */

>>>>>>> v4.9.227
/*
 * Change "struct page" to physical address.
 *
 * This implementation is for the no-MMU case only... if you have an MMU
 * you'll need to provide your own definitions.
 */
<<<<<<< HEAD
#ifndef CONFIG_MMU
static inline void __iomem *ioremap(phys_addr_t offset, unsigned long size)
{
	return (void __iomem*) (unsigned long)offset;
}

#define __ioremap(offset, size, flags)	ioremap(offset, size)

#ifndef ioremap_nocache
#define ioremap_nocache ioremap
#endif

#ifndef ioremap_wc
#define ioremap_wc ioremap_nocache
#endif

static inline void iounmap(void __iomem *addr)
{
}
=======

#ifndef ioremap
#define ioremap ioremap
static inline void __iomem *ioremap(phys_addr_t offset, size_t size)
{
	return (void __iomem *)(unsigned long)offset;
}
#endif

#ifndef __ioremap
#define __ioremap __ioremap
static inline void __iomem *__ioremap(phys_addr_t offset, size_t size,
				      unsigned long flags)
{
	return ioremap(offset, size);
}
#endif

#ifndef ioremap_nocache
#define ioremap_nocache ioremap_nocache
static inline void __iomem *ioremap_nocache(phys_addr_t offset, size_t size)
{
	return ioremap(offset, size);
}
#endif

#ifndef ioremap_uc
#define ioremap_uc ioremap_uc
static inline void __iomem *ioremap_uc(phys_addr_t offset, size_t size)
{
	return ioremap_nocache(offset, size);
}
#endif

#ifndef ioremap_wc
#define ioremap_wc ioremap_wc
static inline void __iomem *ioremap_wc(phys_addr_t offset, size_t size)
{
	return ioremap_nocache(offset, size);
}
#endif

#ifndef ioremap_wt
#define ioremap_wt ioremap_wt
static inline void __iomem *ioremap_wt(phys_addr_t offset, size_t size)
{
	return ioremap_nocache(offset, size);
}
#endif

#ifndef iounmap
#define iounmap iounmap

static inline void iounmap(void __iomem *addr)
{
}
#endif
>>>>>>> v4.9.227
#endif /* CONFIG_MMU */

#ifdef CONFIG_HAS_IOPORT_MAP
#ifndef CONFIG_GENERIC_IOMAP
<<<<<<< HEAD
=======
#ifndef ioport_map
#define ioport_map ioport_map
>>>>>>> v4.9.227
static inline void __iomem *ioport_map(unsigned long port, unsigned int nr)
{
	return PCI_IOBASE + (port & IO_SPACE_LIMIT);
}
<<<<<<< HEAD

static inline void ioport_unmap(void __iomem *p)
{
}
=======
#endif

#ifndef ioport_unmap
#define ioport_unmap ioport_unmap
static inline void ioport_unmap(void __iomem *p)
{
}
#endif
>>>>>>> v4.9.227
#else /* CONFIG_GENERIC_IOMAP */
extern void __iomem *ioport_map(unsigned long port, unsigned int nr);
extern void ioport_unmap(void __iomem *p);
#endif /* CONFIG_GENERIC_IOMAP */
#endif /* CONFIG_HAS_IOPORT_MAP */

#ifndef xlate_dev_kmem_ptr
<<<<<<< HEAD
#define xlate_dev_kmem_ptr(p)	p
#endif
#ifndef xlate_dev_mem_ptr
#define xlate_dev_mem_ptr(p)	__va(p)
=======
#define xlate_dev_kmem_ptr xlate_dev_kmem_ptr
static inline void *xlate_dev_kmem_ptr(void *addr)
{
	return addr;
}
#endif

#ifndef xlate_dev_mem_ptr
#define xlate_dev_mem_ptr xlate_dev_mem_ptr
static inline void *xlate_dev_mem_ptr(phys_addr_t addr)
{
	return __va(addr);
}
#endif

#ifndef unxlate_dev_mem_ptr
#define unxlate_dev_mem_ptr unxlate_dev_mem_ptr
static inline void unxlate_dev_mem_ptr(phys_addr_t phys, void *addr)
{
}
>>>>>>> v4.9.227
#endif

#ifdef CONFIG_VIRT_TO_BUS
#ifndef virt_to_bus
<<<<<<< HEAD
static inline unsigned long virt_to_bus(volatile void *address)
{
	return ((unsigned long) address);
=======
static inline unsigned long virt_to_bus(void *address)
{
	return (unsigned long)address;
>>>>>>> v4.9.227
}

static inline void *bus_to_virt(unsigned long address)
{
<<<<<<< HEAD
	return (void *) address;
=======
	return (void *)address;
>>>>>>> v4.9.227
}
#endif
#endif

#ifndef memset_io
<<<<<<< HEAD
#define memset_io(a, b, c)	memset(__io_virt(a), (b), (c))
#endif

#ifndef memcpy_fromio
#define memcpy_fromio(a, b, c)	memcpy((a), __io_virt(b), (c))
#endif
#ifndef memcpy_toio
#define memcpy_toio(a, b, c)	memcpy(__io_virt(a), (b), (c))
=======
#define memset_io memset_io
static inline void memset_io(volatile void __iomem *addr, int value,
			     size_t size)
{
	memset(__io_virt(addr), value, size);
}
#endif

#ifndef memcpy_fromio
#define memcpy_fromio memcpy_fromio
static inline void memcpy_fromio(void *buffer,
				 const volatile void __iomem *addr,
				 size_t size)
{
	memcpy(buffer, __io_virt(addr), size);
}
#endif

#ifndef memcpy_toio
#define memcpy_toio memcpy_toio
static inline void memcpy_toio(volatile void __iomem *addr, const void *buffer,
			       size_t size)
{
	memcpy(__io_virt(addr), buffer, size);
}
>>>>>>> v4.9.227
#endif

#endif /* __KERNEL__ */

#endif /* __ASM_GENERIC_IO_H */
