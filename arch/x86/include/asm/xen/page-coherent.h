#ifndef _ASM_X86_XEN_PAGE_COHERENT_H
#define _ASM_X86_XEN_PAGE_COHERENT_H

#include <asm/page.h>
<<<<<<< HEAD
#include <linux/dma-attrs.h>
=======
>>>>>>> v4.9.227
#include <linux/dma-mapping.h>

static inline void *xen_alloc_coherent_pages(struct device *hwdev, size_t size,
		dma_addr_t *dma_handle, gfp_t flags,
<<<<<<< HEAD
		struct dma_attrs *attrs)
=======
		unsigned long attrs)
>>>>>>> v4.9.227
{
	void *vstart = (void*)__get_free_pages(flags, get_order(size));
	*dma_handle = virt_to_phys(vstart);
	return vstart;
}

static inline void xen_free_coherent_pages(struct device *hwdev, size_t size,
		void *cpu_addr, dma_addr_t dma_handle,
<<<<<<< HEAD
		struct dma_attrs *attrs)
=======
		unsigned long attrs)
>>>>>>> v4.9.227
{
	free_pages((unsigned long) cpu_addr, get_order(size));
}

static inline void xen_dma_map_page(struct device *hwdev, struct page *page,
<<<<<<< HEAD
	     unsigned long offset, size_t size, enum dma_data_direction dir,
	     struct dma_attrs *attrs) { }

static inline void xen_dma_unmap_page(struct device *hwdev, dma_addr_t handle,
		size_t size, enum dma_data_direction dir,
		struct dma_attrs *attrs) { }
=======
	     dma_addr_t dev_addr, unsigned long offset, size_t size,
	     enum dma_data_direction dir, unsigned long attrs) { }

static inline void xen_dma_unmap_page(struct device *hwdev, dma_addr_t handle,
		size_t size, enum dma_data_direction dir,
		unsigned long attrs) { }
>>>>>>> v4.9.227

static inline void xen_dma_sync_single_for_cpu(struct device *hwdev,
		dma_addr_t handle, size_t size, enum dma_data_direction dir) { }

static inline void xen_dma_sync_single_for_device(struct device *hwdev,
		dma_addr_t handle, size_t size, enum dma_data_direction dir) { }

#endif /* _ASM_X86_XEN_PAGE_COHERENT_H */
