/*
 *  Copyright (C) 2011 Texas Instruments Incorporated
 *  Author: Mark Salter <msalter@redhat.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/scatterlist.h>

#include <asm/cacheflush.h>

static void c6x_dma_sync(dma_addr_t handle, size_t size,
			 enum dma_data_direction dir)
{
	unsigned long paddr = handle;

	BUG_ON(!valid_dma_direction(dir));

	switch (dir) {
	case DMA_FROM_DEVICE:
		L2_cache_block_invalidate(paddr, paddr + size);
		break;
	case DMA_TO_DEVICE:
		L2_cache_block_writeback(paddr, paddr + size);
		break;
	case DMA_BIDIRECTIONAL:
		L2_cache_block_writeback_invalidate(paddr, paddr + size);
		break;
	default:
		break;
	}
}

<<<<<<< HEAD
dma_addr_t dma_map_single(struct device *dev, void *ptr, size_t size,
			  enum dma_data_direction dir)
{
	dma_addr_t addr = virt_to_phys(ptr);

	c6x_dma_sync(addr, size, dir);

	debug_dma_map_page(dev, virt_to_page(ptr),
			   (unsigned long)ptr & ~PAGE_MASK, size,
			   dir, addr, true);
	return addr;
}
EXPORT_SYMBOL(dma_map_single);


void dma_unmap_single(struct device *dev, dma_addr_t handle,
		      size_t size, enum dma_data_direction dir)
{
	c6x_dma_sync(handle, size, dir);

	debug_dma_unmap_page(dev, handle, size, dir, true);
}
EXPORT_SYMBOL(dma_unmap_single);


int dma_map_sg(struct device *dev, struct scatterlist *sglist,
	       int nents, enum dma_data_direction dir)
=======
static dma_addr_t c6x_dma_map_page(struct device *dev, struct page *page,
		unsigned long offset, size_t size, enum dma_data_direction dir,
		unsigned long attrs)
{
	dma_addr_t handle = virt_to_phys(page_address(page) + offset);

	c6x_dma_sync(handle, size, dir);
	return handle;
}

static void c6x_dma_unmap_page(struct device *dev, dma_addr_t handle,
		size_t size, enum dma_data_direction dir, unsigned long attrs)
{
	c6x_dma_sync(handle, size, dir);
}

static int c6x_dma_map_sg(struct device *dev, struct scatterlist *sglist,
		int nents, enum dma_data_direction dir, unsigned long attrs)
>>>>>>> v4.9.227
{
	struct scatterlist *sg;
	int i;

<<<<<<< HEAD
	for_each_sg(sglist, sg, nents, i)
		sg->dma_address = dma_map_single(dev, sg_virt(sg), sg->length,
						 dir);

	debug_dma_map_sg(dev, sglist, nents, nents, dir);

	return nents;
}
EXPORT_SYMBOL(dma_map_sg);


void dma_unmap_sg(struct device *dev, struct scatterlist *sglist,
		  int nents, enum dma_data_direction dir)
=======
	for_each_sg(sglist, sg, nents, i) {
		sg->dma_address = sg_phys(sg);
		c6x_dma_sync(sg->dma_address, sg->length, dir);
	}

	return nents;
}

static void c6x_dma_unmap_sg(struct device *dev, struct scatterlist *sglist,
		  int nents, enum dma_data_direction dir, unsigned long attrs)
>>>>>>> v4.9.227
{
	struct scatterlist *sg;
	int i;

	for_each_sg(sglist, sg, nents, i)
<<<<<<< HEAD
		dma_unmap_single(dev, sg_dma_address(sg), sg->length, dir);

	debug_dma_unmap_sg(dev, sglist,	nents, dir);
}
EXPORT_SYMBOL(dma_unmap_sg);

void dma_sync_single_for_cpu(struct device *dev, dma_addr_t handle,
			     size_t size, enum dma_data_direction dir)
{
	c6x_dma_sync(handle, size, dir);

	debug_dma_sync_single_for_cpu(dev, handle, size, dir);
}
EXPORT_SYMBOL(dma_sync_single_for_cpu);


void dma_sync_single_for_device(struct device *dev, dma_addr_t handle,
				size_t size, enum dma_data_direction dir)
{
	c6x_dma_sync(handle, size, dir);

	debug_dma_sync_single_for_device(dev, handle, size, dir);
}
EXPORT_SYMBOL(dma_sync_single_for_device);


void dma_sync_sg_for_cpu(struct device *dev, struct scatterlist *sglist,
			 int nents, enum dma_data_direction dir)
=======
		c6x_dma_sync(sg_dma_address(sg), sg->length, dir);

}

static void c6x_dma_sync_single_for_cpu(struct device *dev, dma_addr_t handle,
		size_t size, enum dma_data_direction dir)
{
	c6x_dma_sync(handle, size, dir);

}

static void c6x_dma_sync_single_for_device(struct device *dev,
		dma_addr_t handle, size_t size, enum dma_data_direction dir)
{
	c6x_dma_sync(handle, size, dir);

}

static void c6x_dma_sync_sg_for_cpu(struct device *dev,
		struct scatterlist *sglist, int nents,
		enum dma_data_direction dir)
>>>>>>> v4.9.227
{
	struct scatterlist *sg;
	int i;

	for_each_sg(sglist, sg, nents, i)
<<<<<<< HEAD
		dma_sync_single_for_cpu(dev, sg_dma_address(sg),
					sg->length, dir);

	debug_dma_sync_sg_for_cpu(dev, sglist, nents, dir);
}
EXPORT_SYMBOL(dma_sync_sg_for_cpu);


void dma_sync_sg_for_device(struct device *dev, struct scatterlist *sglist,
			    int nents, enum dma_data_direction dir)
=======
		c6x_dma_sync_single_for_cpu(dev, sg_dma_address(sg),
					sg->length, dir);

}

static void c6x_dma_sync_sg_for_device(struct device *dev,
		struct scatterlist *sglist, int nents,
		enum dma_data_direction dir)
>>>>>>> v4.9.227
{
	struct scatterlist *sg;
	int i;

	for_each_sg(sglist, sg, nents, i)
<<<<<<< HEAD
		dma_sync_single_for_device(dev, sg_dma_address(sg),
					   sg->length, dir);

	debug_dma_sync_sg_for_device(dev, sglist, nents, dir);
}
EXPORT_SYMBOL(dma_sync_sg_for_device);

=======
		c6x_dma_sync_single_for_device(dev, sg_dma_address(sg),
					   sg->length, dir);

}

struct dma_map_ops c6x_dma_ops = {
	.alloc			= c6x_dma_alloc,
	.free			= c6x_dma_free,
	.map_page		= c6x_dma_map_page,
	.unmap_page		= c6x_dma_unmap_page,
	.map_sg			= c6x_dma_map_sg,
	.unmap_sg		= c6x_dma_unmap_sg,
	.sync_single_for_device	= c6x_dma_sync_single_for_device,
	.sync_single_for_cpu	= c6x_dma_sync_single_for_cpu,
	.sync_sg_for_device	= c6x_dma_sync_sg_for_device,
	.sync_sg_for_cpu	= c6x_dma_sync_sg_for_cpu,
};
EXPORT_SYMBOL(c6x_dma_ops);
>>>>>>> v4.9.227

/* Number of entries preallocated for DMA-API debugging */
#define PREALLOC_DMA_DEBUG_ENTRIES (1 << 16)

static int __init dma_init(void)
{
	dma_debug_init(PREALLOC_DMA_DEBUG_ENTRIES);

	return 0;
}
fs_initcall(dma_init);
