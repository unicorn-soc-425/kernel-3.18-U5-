/*
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __ASM_DMA_MAPPING_H
#define __ASM_DMA_MAPPING_H

#ifdef __KERNEL__

#include <linux/types.h>
#include <linux/vmalloc.h>

<<<<<<< HEAD
#include <asm-generic/dma-coherent.h>

=======
>>>>>>> v4.9.227
#include <xen/xen.h>
#include <asm/xen/hypervisor.h>

#define DMA_ERROR_CODE	(~(dma_addr_t)0)
<<<<<<< HEAD
extern const struct dma_map_ops *dma_ops;
extern const struct dma_map_ops coherent_swiotlb_dma_ops;
extern const struct dma_map_ops noncoherent_swiotlb_dma_ops;

static inline const struct dma_map_ops *__generic_dma_ops(struct device *dev)
{
	if (unlikely(!dev) || !dev->archdata.dma_ops)
		return dma_ops;
	else
		return dev->archdata.dma_ops;
}

static inline const struct dma_map_ops *get_dma_ops(struct device *dev)
=======
extern struct dma_map_ops dummy_dma_ops;

static inline struct dma_map_ops *__generic_dma_ops(struct device *dev)
{
	if (dev && dev->archdata.dma_ops)
		return dev->archdata.dma_ops;

	/*
	 * We expect no ISA devices, and all other DMA masters are expected to
	 * have someone call arch_setup_dma_ops at device creation time.
	 */
	return &dummy_dma_ops;
}

static inline struct dma_map_ops *get_dma_ops(struct device *dev)
>>>>>>> v4.9.227
{
	if (xen_initial_domain())
		return xen_dma_ops;
	else
		return __generic_dma_ops(dev);
}

<<<<<<< HEAD
static inline void set_dma_ops(struct device *dev,
			const struct dma_map_ops *ops)
{
	dev->archdata.dma_ops = ops;
}

static inline int set_arch_dma_coherent_ops(struct device *dev)
{
	set_dma_ops(dev, &coherent_swiotlb_dma_ops);
	return 0;
}
#define set_arch_dma_coherent_ops	set_arch_dma_coherent_ops

#include <asm-generic/dma-mapping-common.h>

static inline dma_addr_t phys_to_dma(struct device *dev, phys_addr_t paddr)
{
	return (dma_addr_t)paddr;
=======
void arch_setup_dma_ops(struct device *dev, u64 dma_base, u64 size,
			const struct iommu_ops *iommu, bool coherent);
#define arch_setup_dma_ops	arch_setup_dma_ops

#ifdef CONFIG_IOMMU_DMA
void arch_teardown_dma_ops(struct device *dev);
#define arch_teardown_dma_ops	arch_teardown_dma_ops
#endif

/* do not use this function in a driver */
static inline bool is_device_dma_coherent(struct device *dev)
{
	if (!dev)
		return false;
	return dev->archdata.dma_coherent;
}

static inline dma_addr_t phys_to_dma(struct device *dev, phys_addr_t paddr)
{
	dma_addr_t dev_addr = (dma_addr_t)paddr;

	return dev_addr - ((dma_addr_t)dev->dma_pfn_offset << PAGE_SHIFT);
>>>>>>> v4.9.227
}

static inline phys_addr_t dma_to_phys(struct device *dev, dma_addr_t dev_addr)
{
<<<<<<< HEAD
	return (phys_addr_t)dev_addr;
}

static inline int dma_mapping_error(struct device *dev, dma_addr_t dev_addr)
{
	const struct dma_map_ops *ops = get_dma_ops(dev);
	debug_dma_mapping_error(dev, dev_addr);
	return ops->mapping_error(dev, dev_addr);
}

static inline int dma_supported(struct device *dev, u64 mask)
{
	const struct dma_map_ops *ops = get_dma_ops(dev);
	return ops->dma_supported(dev, mask);
}

static inline int dma_set_mask(struct device *dev, u64 mask)
{
	if (!dev->dma_mask || !dma_supported(dev, mask))
		return -EIO;
	*dev->dma_mask = mask;

	return 0;
=======
	phys_addr_t paddr = (phys_addr_t)dev_addr;

	return paddr + ((phys_addr_t)dev->dma_pfn_offset << PAGE_SHIFT);
>>>>>>> v4.9.227
}

static inline bool dma_capable(struct device *dev, dma_addr_t addr, size_t size)
{
	if (!dev->dma_mask)
<<<<<<< HEAD
		return 0;
=======
		return false;
>>>>>>> v4.9.227

	return addr + size - 1 <= *dev->dma_mask;
}

static inline void dma_mark_clean(void *addr, size_t size)
{
}

<<<<<<< HEAD
#define dma_alloc_coherent(d, s, h, f)	dma_alloc_attrs(d, s, h, f, NULL)
#define dma_free_coherent(d, s, h, f)	dma_free_attrs(d, s, h, f, NULL)

static inline void *dma_alloc_attrs(struct device *dev, size_t size,
				    dma_addr_t *dma_handle, gfp_t flags,
				    struct dma_attrs *attrs)
{
	const struct dma_map_ops *ops = get_dma_ops(dev);
	void *vaddr;

	if (dma_alloc_from_coherent(dev, size, dma_handle, &vaddr))
		return vaddr;

	vaddr = ops->alloc(dev, size, dma_handle, flags, attrs);
	debug_dma_alloc_coherent(dev, size, *dma_handle, vaddr);
	return vaddr;
}

static inline void dma_free_attrs(struct device *dev, size_t size,
				  void *vaddr, dma_addr_t dev_addr,
				  struct dma_attrs *attrs)
{
	const struct dma_map_ops *ops = get_dma_ops(dev);

	if (dma_release_from_coherent(dev, get_order(size), vaddr))
		return;

	debug_dma_free_coherent(dev, size, vaddr, dev_addr);
	ops->free(dev, size, vaddr, dev_addr, attrs);
}

static inline void *dma_alloc_nonconsistent(struct device *dev, size_t size,
					dma_addr_t *dma_handle, gfp_t flag)
{
	DEFINE_DMA_ATTRS(attrs);
	dma_set_attr(DMA_ATTR_NON_CONSISTENT, &attrs);
	return dma_alloc_attrs(dev, size, dma_handle, flag, &attrs);
}

static inline void dma_free_nonconsistent(struct device *dev, size_t size,
					void *cpu_addr, dma_addr_t dma_handle)
{
	DEFINE_DMA_ATTRS(attrs);
	dma_set_attr(DMA_ATTR_NON_CONSISTENT, &attrs);
	return dma_free_attrs(dev, size, cpu_addr, dma_handle, &attrs);
}

static inline int dma_mmap_nonconsistent(struct device *dev,
		struct vm_area_struct *vma, void *cpu_addr,
		dma_addr_t dma_addr, size_t size)
{
	DEFINE_DMA_ATTRS(attrs);
	dma_set_attr(DMA_ATTR_NON_CONSISTENT, &attrs);
	return dma_mmap_attrs(dev, vma, cpu_addr, dma_addr, size, &attrs);
}


/*
 * There is no dma_cache_sync() implementation, so just return NULL here.
 */
static inline void *dma_alloc_noncoherent(struct device *dev, size_t size,
					  dma_addr_t *handle, gfp_t flags)
{
	return NULL;
}

static inline void dma_free_noncoherent(struct device *dev, size_t size,
					void *cpu_addr, dma_addr_t handle)
{
}
=======
/* Override for dma_max_pfn() */
static inline unsigned long dma_max_pfn(struct device *dev)
{
	dma_addr_t dma_max = (dma_addr_t)*dev->dma_mask;

	return (ulong)dma_to_phys(dev, dma_max) >> PAGE_SHIFT;
}
#define dma_max_pfn(dev) dma_max_pfn(dev)
>>>>>>> v4.9.227

#endif	/* __KERNEL__ */
#endif	/* __ASM_DMA_MAPPING_H */
