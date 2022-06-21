/* pci-dma.c: Dynamic DMA mapping support for the FRV CPUs that have MMUs
 *
 * Copyright (C) 2004 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <linux/export.h>
#include <linux/highmem.h>
#include <linux/scatterlist.h>
#include <asm/io.h>

<<<<<<< HEAD
void *dma_alloc_coherent(struct device *hwdev, size_t size, dma_addr_t *dma_handle, gfp_t gfp)
=======
static void *frv_dma_alloc(struct device *hwdev, size_t size,
		dma_addr_t *dma_handle, gfp_t gfp, unsigned long attrs)
>>>>>>> v4.9.227
{
	void *ret;

	ret = consistent_alloc(gfp, size, dma_handle);
	if (ret)
		memset(ret, 0, size);

	return ret;
}

<<<<<<< HEAD
EXPORT_SYMBOL(dma_alloc_coherent);

void dma_free_coherent(struct device *hwdev, size_t size, void *vaddr, dma_addr_t dma_handle)
=======
static void frv_dma_free(struct device *hwdev, size_t size, void *vaddr,
		dma_addr_t dma_handle, unsigned long attrs)
>>>>>>> v4.9.227
{
	consistent_free(vaddr);
}

<<<<<<< HEAD
EXPORT_SYMBOL(dma_free_coherent);

dma_addr_t dma_map_single(struct device *dev, void *ptr, size_t size,
			  enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);

	frv_cache_wback_inv((unsigned long) ptr, (unsigned long) ptr + size);

	return virt_to_bus(ptr);
}

EXPORT_SYMBOL(dma_map_single);

int dma_map_sg(struct device *dev, struct scatterlist *sg, int nents,
	       enum dma_data_direction direction)
=======
static int frv_dma_map_sg(struct device *dev, struct scatterlist *sglist,
		int nents, enum dma_data_direction direction,
		unsigned long attrs)
>>>>>>> v4.9.227
{
	unsigned long dampr2;
	void *vaddr;
	int i;
<<<<<<< HEAD
=======
	struct scatterlist *sg;
>>>>>>> v4.9.227

	BUG_ON(direction == DMA_NONE);

	dampr2 = __get_DAMPR(2);

<<<<<<< HEAD
	for (i = 0; i < nents; i++) {
		vaddr = kmap_atomic_primary(sg_page(&sg[i]));
=======
	for_each_sg(sglist, sg, nents, i) {
		vaddr = kmap_atomic_primary(sg_page(sg));
>>>>>>> v4.9.227

		frv_dcache_writeback((unsigned long) vaddr,
				     (unsigned long) vaddr + PAGE_SIZE);

	}

	kunmap_atomic_primary(vaddr);
	if (dampr2) {
		__set_DAMPR(2, dampr2);
		__set_IAMPR(2, dampr2);
	}

	return nents;
}

<<<<<<< HEAD
EXPORT_SYMBOL(dma_map_sg);

dma_addr_t dma_map_page(struct device *dev, struct page *page, unsigned long offset,
			size_t size, enum dma_data_direction direction)
{
	BUG_ON(direction == DMA_NONE);
=======
static dma_addr_t frv_dma_map_page(struct device *dev, struct page *page,
		unsigned long offset, size_t size,
		enum dma_data_direction direction, unsigned long attrs)
{
>>>>>>> v4.9.227
	flush_dcache_page(page);
	return (dma_addr_t) page_to_phys(page) + offset;
}

<<<<<<< HEAD
EXPORT_SYMBOL(dma_map_page);
=======
static void frv_dma_sync_single_for_device(struct device *dev,
		dma_addr_t dma_handle, size_t size,
		enum dma_data_direction direction)
{
	flush_write_buffers();
}

static void frv_dma_sync_sg_for_device(struct device *dev,
		struct scatterlist *sg, int nelems,
		enum dma_data_direction direction)
{
	flush_write_buffers();
}


static int frv_dma_supported(struct device *dev, u64 mask)
{
        /*
         * we fall back to GFP_DMA when the mask isn't all 1s,
         * so we can't guarantee allocations that must be
         * within a tighter range than GFP_DMA..
         */
        if (mask < 0x00ffffff)
                return 0;
	return 1;
}

struct dma_map_ops frv_dma_ops = {
	.alloc			= frv_dma_alloc,
	.free			= frv_dma_free,
	.map_page		= frv_dma_map_page,
	.map_sg			= frv_dma_map_sg,
	.sync_single_for_device	= frv_dma_sync_single_for_device,
	.sync_sg_for_device	= frv_dma_sync_sg_for_device,
	.dma_supported		= frv_dma_supported,
};
EXPORT_SYMBOL(frv_dma_ops);
>>>>>>> v4.9.227
