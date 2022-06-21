/*
 * Copyright (c) 2004, 2005, 2006 Voltaire, Inc. All rights reserved.
 * Copyright (c) 2013-2014 Mellanox Technologies. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *	- Redistributions of source code must retain the above
 *	  copyright notice, this list of conditions and the following
 *	  disclaimer.
 *
 *	- Redistributions in binary form must reproduce the above
 *	  copyright notice, this list of conditions and the following
 *	  disclaimer in the documentation and/or other materials
 *	  provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/scatterlist.h>

#include "iscsi_iser.h"
<<<<<<< HEAD

#define ISER_KMALLOC_THRESHOLD 0x20000 /* 128K - kmalloc limit */

/**
 * iser_start_rdma_unaligned_sg
 */
static int iser_start_rdma_unaligned_sg(struct iscsi_iser_task *iser_task,
					struct iser_data_buf *data,
					struct iser_data_buf *data_copy,
					enum iser_data_dir cmd_dir)
{
	struct ib_device *dev = iser_task->iser_conn->ib_conn.device->ib_device;
	struct scatterlist *sgl = (struct scatterlist *)data->buf;
	struct scatterlist *sg;
	char *mem = NULL;
	unsigned long  cmd_data_len = 0;
	int dma_nents, i;

	for_each_sg(sgl, sg, data->size, i)
		cmd_data_len += ib_sg_dma_len(dev, sg);

	if (cmd_data_len > ISER_KMALLOC_THRESHOLD)
		mem = (void *)__get_free_pages(GFP_ATOMIC,
		      ilog2(roundup_pow_of_two(cmd_data_len)) - PAGE_SHIFT);
	else
		mem = kmalloc(cmd_data_len, GFP_ATOMIC);

	if (mem == NULL) {
		iser_err("Failed to allocate mem size %d %d for copying sglist\n",
			 data->size, (int)cmd_data_len);
		return -ENOMEM;
	}

	if (cmd_dir == ISER_DIR_OUT) {
		/* copy the unaligned sg the buffer which is used for RDMA */
		char *p, *from;

		sgl = (struct scatterlist *)data->buf;
		p = mem;
		for_each_sg(sgl, sg, data->size, i) {
			from = kmap_atomic(sg_page(sg));
			memcpy(p,
			       from + sg->offset,
			       sg->length);
			kunmap_atomic(from);
			p += sg->length;
		}
	}

	sg_init_one(&data_copy->sg_single, mem, cmd_data_len);
	data_copy->buf = &data_copy->sg_single;
	data_copy->size = 1;
	data_copy->copy_buf = mem;

	dma_nents = ib_dma_map_sg(dev, &data_copy->sg_single, 1,
				  (cmd_dir == ISER_DIR_OUT) ?
				  DMA_TO_DEVICE : DMA_FROM_DEVICE);
	BUG_ON(dma_nents == 0);

	data_copy->dma_nents = dma_nents;
	data_copy->data_len = cmd_data_len;

	return 0;
}

/**
 * iser_finalize_rdma_unaligned_sg
 */

void iser_finalize_rdma_unaligned_sg(struct iscsi_iser_task *iser_task,
				     struct iser_data_buf *data,
				     struct iser_data_buf *data_copy,
				     enum iser_data_dir cmd_dir)
{
	struct ib_device *dev;
	unsigned long  cmd_data_len;

	dev = iser_task->iser_conn->ib_conn.device->ib_device;

	ib_dma_unmap_sg(dev, &data_copy->sg_single, 1,
			(cmd_dir == ISER_DIR_OUT) ?
			DMA_TO_DEVICE : DMA_FROM_DEVICE);

	if (cmd_dir == ISER_DIR_IN) {
		char *mem;
		struct scatterlist *sgl, *sg;
		unsigned char *p, *to;
		unsigned int sg_size;
		int i;

		/* copy back read RDMA to unaligned sg */
		mem = data_copy->copy_buf;

		sgl = (struct scatterlist *)data->buf;
		sg_size = data->size;

		p = mem;
		for_each_sg(sgl, sg, sg_size, i) {
			to = kmap_atomic(sg_page(sg));
			memcpy(to + sg->offset,
			       p,
			       sg->length);
			kunmap_atomic(to);
			p += sg->length;
		}
	}

	cmd_data_len = data->data_len;

	if (cmd_data_len > ISER_KMALLOC_THRESHOLD)
		free_pages((unsigned long)data_copy->copy_buf,
			   ilog2(roundup_pow_of_two(cmd_data_len)) - PAGE_SHIFT);
	else
		kfree(data_copy->copy_buf);

	data_copy->copy_buf = NULL;
}

#define IS_4K_ALIGNED(addr)	((((unsigned long)addr) & ~MASK_4K) == 0)

/**
 * iser_sg_to_page_vec - Translates scatterlist entries to physical addresses
 * and returns the length of resulting physical address array (may be less than
 * the original due to possible compaction).
 *
 * we build a "page vec" under the assumption that the SG meets the RDMA
 * alignment requirements. Other then the first and last SG elements, all
 * the "internal" elements can be compacted into a list whose elements are
 * dma addresses of physical pages. The code supports also the weird case
 * where --few fragments of the same page-- are present in the SG as
 * consecutive elements. Also, it handles one entry SG.
 */

static int iser_sg_to_page_vec(struct iser_data_buf *data,
			       struct ib_device *ibdev, u64 *pages,
			       int *offset, int *data_size)
{
	struct scatterlist *sg, *sgl = (struct scatterlist *)data->buf;
	u64 start_addr, end_addr, page, chunk_start = 0;
	unsigned long total_sz = 0;
	unsigned int dma_len;
	int i, new_chunk, cur_page, last_ent = data->dma_nents - 1;

	/* compute the offset of first element */
	*offset = (u64) sgl[0].offset & ~MASK_4K;

	new_chunk = 1;
	cur_page  = 0;
	for_each_sg(sgl, sg, data->dma_nents, i) {
		start_addr = ib_sg_dma_address(ibdev, sg);
		if (new_chunk)
			chunk_start = start_addr;
		dma_len = ib_sg_dma_len(ibdev, sg);
		end_addr = start_addr + dma_len;
		total_sz += dma_len;

		/* collect page fragments until aligned or end of SG list */
		if (!IS_4K_ALIGNED(end_addr) && i < last_ent) {
			new_chunk = 0;
			continue;
		}
		new_chunk = 1;

		/* address of the first page in the contiguous chunk;
		   masking relevant for the very first SG entry,
		   which might be unaligned */
		page = chunk_start & MASK_4K;
		do {
			pages[cur_page++] = page;
			page += SIZE_4K;
		} while (page < end_addr);
	}

	*data_size = total_sz;
	iser_dbg("page_vec->data_size:%d cur_page %d\n",
		 *data_size, cur_page);
	return cur_page;
}


/**
 * iser_data_buf_aligned_len - Tries to determine the maximal correctly aligned
 * for RDMA sub-list of a scatter-gather list of memory buffers, and  returns
 * the number of entries which are aligned correctly. Supports the case where
 * consecutive SG elements are actually fragments of the same physcial page.
 */
static int iser_data_buf_aligned_len(struct iser_data_buf *data,
				      struct ib_device *ibdev)
{
	struct scatterlist *sgl, *sg, *next_sg = NULL;
	u64 start_addr, end_addr;
	int i, ret_len, start_check = 0;

	if (data->dma_nents == 1)
		return 1;

	sgl = (struct scatterlist *)data->buf;
	start_addr  = ib_sg_dma_address(ibdev, sgl);

	for_each_sg(sgl, sg, data->dma_nents, i) {
		if (start_check && !IS_4K_ALIGNED(start_addr))
			break;

		next_sg = sg_next(sg);
		if (!next_sg)
			break;

		end_addr    = start_addr + ib_sg_dma_len(ibdev, sg);
		start_addr  = ib_sg_dma_address(ibdev, next_sg);

		if (end_addr == start_addr) {
			start_check = 0;
			continue;
		} else
			start_check = 1;

		if (!IS_4K_ALIGNED(end_addr))
			break;
	}
	ret_len = (next_sg) ? i : i+1;
	iser_dbg("Found %d aligned entries out of %d in sg:0x%p\n",
		 ret_len, data->dma_nents, data);
	return ret_len;
=======
static
int iser_fast_reg_fmr(struct iscsi_iser_task *iser_task,
		      struct iser_data_buf *mem,
		      struct iser_reg_resources *rsc,
		      struct iser_mem_reg *mem_reg);
static
int iser_fast_reg_mr(struct iscsi_iser_task *iser_task,
		     struct iser_data_buf *mem,
		     struct iser_reg_resources *rsc,
		     struct iser_mem_reg *mem_reg);

static const struct iser_reg_ops fastreg_ops = {
	.alloc_reg_res	= iser_alloc_fastreg_pool,
	.free_reg_res	= iser_free_fastreg_pool,
	.reg_mem	= iser_fast_reg_mr,
	.unreg_mem	= iser_unreg_mem_fastreg,
	.reg_desc_get	= iser_reg_desc_get_fr,
	.reg_desc_put	= iser_reg_desc_put_fr,
};

static const struct iser_reg_ops fmr_ops = {
	.alloc_reg_res	= iser_alloc_fmr_pool,
	.free_reg_res	= iser_free_fmr_pool,
	.reg_mem	= iser_fast_reg_fmr,
	.unreg_mem	= iser_unreg_mem_fmr,
	.reg_desc_get	= iser_reg_desc_get_fmr,
	.reg_desc_put	= iser_reg_desc_put_fmr,
};

void iser_reg_comp(struct ib_cq *cq, struct ib_wc *wc)
{
	iser_err_comp(wc, "memreg");
}

int iser_assign_reg_ops(struct iser_device *device)
{
	struct ib_device *ib_dev = device->ib_device;

	/* Assign function handles  - based on FMR support */
	if (ib_dev->alloc_fmr && ib_dev->dealloc_fmr &&
	    ib_dev->map_phys_fmr && ib_dev->unmap_fmr) {
		iser_info("FMR supported, using FMR for registration\n");
		device->reg_ops = &fmr_ops;
	} else if (ib_dev->attrs.device_cap_flags & IB_DEVICE_MEM_MGT_EXTENSIONS) {
		iser_info("FastReg supported, using FastReg for registration\n");
		device->reg_ops = &fastreg_ops;
		device->remote_inv_sup = iser_always_reg;
	} else {
		iser_err("IB device does not support FMRs nor FastRegs, can't register memory\n");
		return -1;
	}

	return 0;
}

struct iser_fr_desc *
iser_reg_desc_get_fr(struct ib_conn *ib_conn)
{
	struct iser_fr_pool *fr_pool = &ib_conn->fr_pool;
	struct iser_fr_desc *desc;
	unsigned long flags;

	spin_lock_irqsave(&fr_pool->lock, flags);
	desc = list_first_entry(&fr_pool->list,
				struct iser_fr_desc, list);
	list_del(&desc->list);
	spin_unlock_irqrestore(&fr_pool->lock, flags);

	return desc;
}

void
iser_reg_desc_put_fr(struct ib_conn *ib_conn,
		     struct iser_fr_desc *desc)
{
	struct iser_fr_pool *fr_pool = &ib_conn->fr_pool;
	unsigned long flags;

	spin_lock_irqsave(&fr_pool->lock, flags);
	list_add(&desc->list, &fr_pool->list);
	spin_unlock_irqrestore(&fr_pool->lock, flags);
}

struct iser_fr_desc *
iser_reg_desc_get_fmr(struct ib_conn *ib_conn)
{
	struct iser_fr_pool *fr_pool = &ib_conn->fr_pool;

	return list_first_entry(&fr_pool->list,
				struct iser_fr_desc, list);
}

void
iser_reg_desc_put_fmr(struct ib_conn *ib_conn,
		      struct iser_fr_desc *desc)
{
>>>>>>> v4.9.227
}

static void iser_data_buf_dump(struct iser_data_buf *data,
			       struct ib_device *ibdev)
{
<<<<<<< HEAD
	struct scatterlist *sgl = (struct scatterlist *)data->buf;
	struct scatterlist *sg;
	int i;

	for_each_sg(sgl, sg, data->dma_nents, i)
=======
	struct scatterlist *sg;
	int i;

	for_each_sg(data->sg, sg, data->dma_nents, i)
>>>>>>> v4.9.227
		iser_dbg("sg[%d] dma_addr:0x%lX page:0x%p "
			 "off:0x%x sz:0x%x dma_len:0x%x\n",
			 i, (unsigned long)ib_sg_dma_address(ibdev, sg),
			 sg_page(sg), sg->offset,
			 sg->length, ib_sg_dma_len(ibdev, sg));
}

static void iser_dump_page_vec(struct iser_page_vec *page_vec)
{
	int i;

<<<<<<< HEAD
	iser_err("page vec length %d data size %d\n",
		 page_vec->length, page_vec->data_size);
	for (i = 0; i < page_vec->length; i++)
		iser_err("%d %lx\n",i,(unsigned long)page_vec->pages[i]);
}

static void iser_page_vec_build(struct iser_data_buf *data,
				struct iser_page_vec *page_vec,
				struct ib_device *ibdev)
{
	int page_vec_len = 0;

	page_vec->length = 0;
	page_vec->offset = 0;

	iser_dbg("Translating sg sz: %d\n", data->dma_nents);
	page_vec_len = iser_sg_to_page_vec(data, ibdev, page_vec->pages,
					   &page_vec->offset,
					   &page_vec->data_size);
	iser_dbg("sg len %d page_vec_len %d\n", data->dma_nents, page_vec_len);

	page_vec->length = page_vec_len;

	if (page_vec_len * SIZE_4K < page_vec->data_size) {
		iser_err("page_vec too short to hold this SG\n");
		iser_data_buf_dump(data, ibdev);
		iser_dump_page_vec(page_vec);
		BUG();
	}
=======
	iser_err("page vec npages %d data length %d\n",
		 page_vec->npages, page_vec->fake_mr.length);
	for (i = 0; i < page_vec->npages; i++)
		iser_err("vec[%d]: %llx\n", i, page_vec->pages[i]);
>>>>>>> v4.9.227
}

int iser_dma_map_task_data(struct iscsi_iser_task *iser_task,
			    struct iser_data_buf *data,
			    enum iser_data_dir iser_dir,
			    enum dma_data_direction dma_dir)
{
	struct ib_device *dev;

	iser_task->dir[iser_dir] = 1;
	dev = iser_task->iser_conn->ib_conn.device->ib_device;

<<<<<<< HEAD
	data->dma_nents = ib_dma_map_sg(dev, data->buf, data->size, dma_dir);
=======
	data->dma_nents = ib_dma_map_sg(dev, data->sg, data->size, dma_dir);
>>>>>>> v4.9.227
	if (data->dma_nents == 0) {
		iser_err("dma_map_sg failed!!!\n");
		return -EINVAL;
	}
	return 0;
}

void iser_dma_unmap_task_data(struct iscsi_iser_task *iser_task,
			      struct iser_data_buf *data,
			      enum dma_data_direction dir)
{
	struct ib_device *dev;

	dev = iser_task->iser_conn->ib_conn.device->ib_device;
<<<<<<< HEAD
	ib_dma_unmap_sg(dev, data->buf, data->size, dir);
}

static int fall_to_bounce_buf(struct iscsi_iser_task *iser_task,
			      struct ib_device *ibdev,
			      struct iser_data_buf *mem,
			      struct iser_data_buf *mem_copy,
			      enum iser_data_dir cmd_dir,
			      int aligned_len)
{
	struct iscsi_conn    *iscsi_conn = iser_task->iser_conn->iscsi_conn;

	iscsi_conn->fmr_unalign_cnt++;
	iser_warn("rdma alignment violation (%d/%d aligned) or FMR not supported\n",
		  aligned_len, mem->size);

	if (iser_debug_level > 0)
		iser_data_buf_dump(mem, ibdev);

	/* unmap the command data before accessing it */
	iser_dma_unmap_task_data(iser_task, mem,
				 (cmd_dir == ISER_DIR_OUT) ?
				 DMA_TO_DEVICE : DMA_FROM_DEVICE);

	/* allocate copy buf, if we are writing, copy the */
	/* unaligned scatterlist, dma map the copy        */
	if (iser_start_rdma_unaligned_sg(iser_task, mem, mem_copy, cmd_dir) != 0)
		return -ENOMEM;
=======
	ib_dma_unmap_sg(dev, data->sg, data->size, dir);
}

static int
iser_reg_dma(struct iser_device *device, struct iser_data_buf *mem,
	     struct iser_mem_reg *reg)
{
	struct scatterlist *sg = mem->sg;

	reg->sge.lkey = device->pd->local_dma_lkey;
	/*
	 * FIXME: rework the registration code path to differentiate
	 * rkey/lkey use cases
	 */

	if (device->pd->flags & IB_PD_UNSAFE_GLOBAL_RKEY)
		reg->rkey = device->pd->unsafe_global_rkey;
	else
		reg->rkey = 0;
	reg->sge.addr = ib_sg_dma_address(device->ib_device, &sg[0]);
	reg->sge.length = ib_sg_dma_len(device->ib_device, &sg[0]);

	iser_dbg("Single DMA entry: lkey=0x%x, rkey=0x%x, addr=0x%llx,"
		 " length=0x%x\n", reg->sge.lkey, reg->rkey,
		 reg->sge.addr, reg->sge.length);

	return 0;
}

static int iser_set_page(struct ib_mr *mr, u64 addr)
{
	struct iser_page_vec *page_vec =
		container_of(mr, struct iser_page_vec, fake_mr);

	page_vec->pages[page_vec->npages++] = addr;

	return 0;
}

static
int iser_fast_reg_fmr(struct iscsi_iser_task *iser_task,
		      struct iser_data_buf *mem,
		      struct iser_reg_resources *rsc,
		      struct iser_mem_reg *reg)
{
	struct ib_conn *ib_conn = &iser_task->iser_conn->ib_conn;
	struct iser_device *device = ib_conn->device;
	struct iser_page_vec *page_vec = rsc->page_vec;
	struct ib_fmr_pool *fmr_pool = rsc->fmr_pool;
	struct ib_pool_fmr *fmr;
	int ret, plen;

	page_vec->npages = 0;
	page_vec->fake_mr.page_size = SIZE_4K;
	plen = ib_sg_to_pages(&page_vec->fake_mr, mem->sg,
			      mem->dma_nents, NULL, iser_set_page);
	if (unlikely(plen < mem->dma_nents)) {
		iser_err("page vec too short to hold this SG\n");
		iser_data_buf_dump(mem, device->ib_device);
		iser_dump_page_vec(page_vec);
		return -EINVAL;
	}

	fmr  = ib_fmr_pool_map_phys(fmr_pool, page_vec->pages,
				    page_vec->npages, page_vec->pages[0]);
	if (IS_ERR(fmr)) {
		ret = PTR_ERR(fmr);
		iser_err("ib_fmr_pool_map_phys failed: %d\n", ret);
		return ret;
	}

	reg->sge.lkey = fmr->fmr->lkey;
	reg->rkey = fmr->fmr->rkey;
	reg->sge.addr = page_vec->fake_mr.iova;
	reg->sge.length = page_vec->fake_mr.length;
	reg->mem_h = fmr;

	iser_dbg("fmr reg: lkey=0x%x, rkey=0x%x, addr=0x%llx,"
		 " length=0x%x\n", reg->sge.lkey, reg->rkey,
		 reg->sge.addr, reg->sge.length);
>>>>>>> v4.9.227

	return 0;
}

/**
<<<<<<< HEAD
 * iser_reg_rdma_mem_fmr - Registers memory intended for RDMA,
 * using FMR (if possible) obtaining rkey and va
 *
 * returns 0 on success, errno code on failure
 */
int iser_reg_rdma_mem_fmr(struct iscsi_iser_task *iser_task,
			  enum iser_data_dir cmd_dir)
{
	struct ib_conn *ib_conn = &iser_task->iser_conn->ib_conn;
	struct iser_device   *device = ib_conn->device;
	struct ib_device     *ibdev = device->ib_device;
	struct iser_data_buf *mem = &iser_task->data[cmd_dir];
	struct iser_regd_buf *regd_buf;
	int aligned_len;
	int err;
	int i;
	struct scatterlist *sg;

	regd_buf = &iser_task->rdma_regd[cmd_dir];

	aligned_len = iser_data_buf_aligned_len(mem, ibdev);
	if (aligned_len != mem->dma_nents) {
		err = fall_to_bounce_buf(iser_task, ibdev, mem,
					 &iser_task->data_copy[cmd_dir],
					 cmd_dir, aligned_len);
		if (err) {
			iser_err("failed to allocate bounce buffer\n");
			return err;
		}
		mem = &iser_task->data_copy[cmd_dir];
	}

	/* if there a single dma entry, FMR is not needed */
	if (mem->dma_nents == 1) {
		sg = (struct scatterlist *)mem->buf;

		regd_buf->reg.lkey = device->mr->lkey;
		regd_buf->reg.rkey = device->mr->rkey;
		regd_buf->reg.len  = ib_sg_dma_len(ibdev, &sg[0]);
		regd_buf->reg.va   = ib_sg_dma_address(ibdev, &sg[0]);
		regd_buf->reg.is_mr = 0;

		iser_dbg("PHYSICAL Mem.register: lkey: 0x%08X rkey: 0x%08X  "
			 "va: 0x%08lX sz: %ld]\n",
			 (unsigned int)regd_buf->reg.lkey,
			 (unsigned int)regd_buf->reg.rkey,
			 (unsigned long)regd_buf->reg.va,
			 (unsigned long)regd_buf->reg.len);
	} else { /* use FMR for multiple dma entries */
		iser_page_vec_build(mem, ib_conn->fmr.page_vec, ibdev);
		err = iser_reg_page_vec(ib_conn, ib_conn->fmr.page_vec,
					&regd_buf->reg);
		if (err && err != -EAGAIN) {
			iser_data_buf_dump(mem, ibdev);
			iser_err("mem->dma_nents = %d (dlength = 0x%x)\n",
				 mem->dma_nents,
				 ntoh24(iser_task->desc.iscsi_header.dlength));
			iser_err("page_vec: data_size = 0x%x, length = %d, offset = 0x%x\n",
				 ib_conn->fmr.page_vec->data_size,
				 ib_conn->fmr.page_vec->length,
				 ib_conn->fmr.page_vec->offset);
			for (i = 0; i < ib_conn->fmr.page_vec->length; i++)
				iser_err("page_vec[%d] = 0x%llx\n", i,
					 (unsigned long long)ib_conn->fmr.page_vec->pages[i]);
		}
		if (err)
			return err;
	}
	return 0;
}

static inline void
=======
 * Unregister (previosuly registered using FMR) memory.
 * If memory is non-FMR does nothing.
 */
void iser_unreg_mem_fmr(struct iscsi_iser_task *iser_task,
			enum iser_data_dir cmd_dir)
{
	struct iser_mem_reg *reg = &iser_task->rdma_reg[cmd_dir];
	int ret;

	if (!reg->mem_h)
		return;

	iser_dbg("PHYSICAL Mem.Unregister mem_h %p\n", reg->mem_h);

	ret = ib_fmr_pool_unmap((struct ib_pool_fmr *)reg->mem_h);
	if (ret)
		iser_err("ib_fmr_pool_unmap failed %d\n", ret);

	reg->mem_h = NULL;
}

void iser_unreg_mem_fastreg(struct iscsi_iser_task *iser_task,
			    enum iser_data_dir cmd_dir)
{
	struct iser_device *device = iser_task->iser_conn->ib_conn.device;
	struct iser_mem_reg *reg = &iser_task->rdma_reg[cmd_dir];

	if (!reg->mem_h)
		return;

	device->reg_ops->reg_desc_put(&iser_task->iser_conn->ib_conn,
				     reg->mem_h);
	reg->mem_h = NULL;
}

static void
>>>>>>> v4.9.227
iser_set_dif_domain(struct scsi_cmnd *sc, struct ib_sig_attrs *sig_attrs,
		    struct ib_sig_domain *domain)
{
	domain->sig_type = IB_SIG_TYPE_T10_DIF;
<<<<<<< HEAD
	domain->sig.dif.pi_interval = sc->device->sector_size;
	domain->sig.dif.ref_tag = scsi_get_lba(sc) & 0xffffffff;
=======
	domain->sig.dif.pi_interval = scsi_prot_interval(sc);
	domain->sig.dif.ref_tag = scsi_prot_ref_tag(sc);
>>>>>>> v4.9.227
	/*
	 * At the moment we hard code those, but in the future
	 * we will take them from sc.
	 */
	domain->sig.dif.apptag_check_mask = 0xffff;
	domain->sig.dif.app_escape = true;
	domain->sig.dif.ref_escape = true;
<<<<<<< HEAD
	if (scsi_get_prot_type(sc) == SCSI_PROT_DIF_TYPE1 ||
	    scsi_get_prot_type(sc) == SCSI_PROT_DIF_TYPE2)
=======
	if (sc->prot_flags & SCSI_PROT_REF_INCREMENT)
>>>>>>> v4.9.227
		domain->sig.dif.ref_remap = true;
};

static int
iser_set_sig_attrs(struct scsi_cmnd *sc, struct ib_sig_attrs *sig_attrs)
{
	switch (scsi_get_prot_op(sc)) {
	case SCSI_PROT_WRITE_INSERT:
	case SCSI_PROT_READ_STRIP:
		sig_attrs->mem.sig_type = IB_SIG_TYPE_NONE;
		iser_set_dif_domain(sc, sig_attrs, &sig_attrs->wire);
		sig_attrs->wire.sig.dif.bg_type = IB_T10DIF_CRC;
		break;
	case SCSI_PROT_READ_INSERT:
	case SCSI_PROT_WRITE_STRIP:
		sig_attrs->wire.sig_type = IB_SIG_TYPE_NONE;
		iser_set_dif_domain(sc, sig_attrs, &sig_attrs->mem);
<<<<<<< HEAD
		/*
		 * At the moment we use this modparam to tell what is
		 * the memory bg_type, in the future we will take it
		 * from sc.
		 */
		sig_attrs->mem.sig.dif.bg_type = iser_pi_guard ? IB_T10DIF_CSUM :
						 IB_T10DIF_CRC;
=======
		sig_attrs->mem.sig.dif.bg_type = sc->prot_flags & SCSI_PROT_IP_CHECKSUM ?
						IB_T10DIF_CSUM : IB_T10DIF_CRC;
>>>>>>> v4.9.227
		break;
	case SCSI_PROT_READ_PASS:
	case SCSI_PROT_WRITE_PASS:
		iser_set_dif_domain(sc, sig_attrs, &sig_attrs->wire);
		sig_attrs->wire.sig.dif.bg_type = IB_T10DIF_CRC;
		iser_set_dif_domain(sc, sig_attrs, &sig_attrs->mem);
<<<<<<< HEAD
		/*
		 * At the moment we use this modparam to tell what is
		 * the memory bg_type, in the future we will take it
		 * from sc.
		 */
		sig_attrs->mem.sig.dif.bg_type = iser_pi_guard ? IB_T10DIF_CSUM :
						 IB_T10DIF_CRC;
=======
		sig_attrs->mem.sig.dif.bg_type = sc->prot_flags & SCSI_PROT_IP_CHECKSUM ?
						IB_T10DIF_CSUM : IB_T10DIF_CRC;
>>>>>>> v4.9.227
		break;
	default:
		iser_err("Unsupported PI operation %d\n",
			 scsi_get_prot_op(sc));
		return -EINVAL;
	}

	return 0;
}

<<<<<<< HEAD
static int
iser_set_prot_checks(struct scsi_cmnd *sc, u8 *mask)
{
	switch (scsi_get_prot_type(sc)) {
	case SCSI_PROT_DIF_TYPE0:
		break;
	case SCSI_PROT_DIF_TYPE1:
	case SCSI_PROT_DIF_TYPE2:
		*mask = ISER_CHECK_GUARD | ISER_CHECK_REFTAG;
		break;
	case SCSI_PROT_DIF_TYPE3:
		*mask = ISER_CHECK_GUARD;
		break;
	default:
		iser_err("Unsupported protection type %d\n",
			 scsi_get_prot_type(sc));
		return -EINVAL;
	}

	return 0;
=======
static inline void
iser_set_prot_checks(struct scsi_cmnd *sc, u8 *mask)
{
	*mask = 0;
	if (sc->prot_flags & SCSI_PROT_REF_CHECK)
		*mask |= ISER_CHECK_REFTAG;
	if (sc->prot_flags & SCSI_PROT_GUARD_CHECK)
		*mask |= ISER_CHECK_GUARD;
}

static inline void
iser_inv_rkey(struct ib_send_wr *inv_wr,
	      struct ib_mr *mr,
	      struct ib_cqe *cqe)
{
	inv_wr->opcode = IB_WR_LOCAL_INV;
	inv_wr->wr_cqe = cqe;
	inv_wr->ex.invalidate_rkey = mr->rkey;
	inv_wr->send_flags = 0;
	inv_wr->num_sge = 0;
>>>>>>> v4.9.227
}

static int
iser_reg_sig_mr(struct iscsi_iser_task *iser_task,
<<<<<<< HEAD
		struct fast_reg_descriptor *desc, struct ib_sge *data_sge,
		struct ib_sge *prot_sge, struct ib_sge *sig_sge)
{
	struct ib_conn *ib_conn = &iser_task->iser_conn->ib_conn;
	struct iser_pi_context *pi_ctx = desc->pi_ctx;
	struct ib_send_wr sig_wr, inv_wr;
	struct ib_send_wr *bad_wr, *wr = NULL;
	struct ib_sig_attrs sig_attrs;
	int ret;
	u32 key;

	memset(&sig_attrs, 0, sizeof(sig_attrs));
	ret = iser_set_sig_attrs(iser_task->sc, &sig_attrs);
	if (ret)
		goto err;

	ret = iser_set_prot_checks(iser_task->sc, &sig_attrs.check_mask);
	if (ret)
		goto err;

	if (!(desc->reg_indicators & ISER_SIG_KEY_VALID)) {
		memset(&inv_wr, 0, sizeof(inv_wr));
		inv_wr.opcode = IB_WR_LOCAL_INV;
		inv_wr.wr_id = ISER_FASTREG_LI_WRID;
		inv_wr.ex.invalidate_rkey = pi_ctx->sig_mr->rkey;
		wr = &inv_wr;
		/* Bump the key */
		key = (u8)(pi_ctx->sig_mr->rkey & 0x000000FF);
		ib_update_fast_reg_key(pi_ctx->sig_mr, ++key);
	}

	memset(&sig_wr, 0, sizeof(sig_wr));
	sig_wr.opcode = IB_WR_REG_SIG_MR;
	sig_wr.wr_id = ISER_FASTREG_LI_WRID;
	sig_wr.sg_list = data_sge;
	sig_wr.num_sge = 1;
	sig_wr.wr.sig_handover.sig_attrs = &sig_attrs;
	sig_wr.wr.sig_handover.sig_mr = pi_ctx->sig_mr;
	if (scsi_prot_sg_count(iser_task->sc))
		sig_wr.wr.sig_handover.prot = prot_sge;
	sig_wr.wr.sig_handover.access_flags = IB_ACCESS_LOCAL_WRITE |
					      IB_ACCESS_REMOTE_READ |
					      IB_ACCESS_REMOTE_WRITE;

	if (!wr)
		wr = &sig_wr;
	else
		wr->next = &sig_wr;

	ret = ib_post_send(ib_conn->qp, wr, &bad_wr);
	if (ret) {
		iser_err("reg_sig_mr failed, ret:%d\n", ret);
		goto err;
	}
	desc->reg_indicators &= ~ISER_SIG_KEY_VALID;

	sig_sge->lkey = pi_ctx->sig_mr->lkey;
	sig_sge->addr = 0;
	sig_sge->length = data_sge->length + prot_sge->length;
	if (scsi_get_prot_op(iser_task->sc) == SCSI_PROT_WRITE_INSERT ||
	    scsi_get_prot_op(iser_task->sc) == SCSI_PROT_READ_STRIP) {
		sig_sge->length += (data_sge->length /
				   iser_task->sc->device->sector_size) * 8;
	}

	iser_dbg("sig_sge: addr: 0x%llx  length: %u lkey: 0x%x\n",
		 sig_sge->addr, sig_sge->length,
		 sig_sge->lkey);
=======
		struct iser_pi_context *pi_ctx,
		struct iser_mem_reg *data_reg,
		struct iser_mem_reg *prot_reg,
		struct iser_mem_reg *sig_reg)
{
	struct iser_tx_desc *tx_desc = &iser_task->desc;
	struct ib_sig_attrs *sig_attrs = &tx_desc->sig_attrs;
	struct ib_cqe *cqe = &iser_task->iser_conn->ib_conn.reg_cqe;
	struct ib_sig_handover_wr *wr;
	struct ib_mr *mr = pi_ctx->sig_mr;
	int ret;

	memset(sig_attrs, 0, sizeof(*sig_attrs));
	ret = iser_set_sig_attrs(iser_task->sc, sig_attrs);
	if (ret)
		goto err;

	iser_set_prot_checks(iser_task->sc, &sig_attrs->check_mask);

	if (pi_ctx->sig_mr_valid)
		iser_inv_rkey(iser_tx_next_wr(tx_desc), mr, cqe);

	ib_update_fast_reg_key(mr, ib_inc_rkey(mr->rkey));

	wr = sig_handover_wr(iser_tx_next_wr(tx_desc));
	wr->wr.opcode = IB_WR_REG_SIG_MR;
	wr->wr.wr_cqe = cqe;
	wr->wr.sg_list = &data_reg->sge;
	wr->wr.num_sge = 1;
	wr->wr.send_flags = 0;
	wr->sig_attrs = sig_attrs;
	wr->sig_mr = mr;
	if (scsi_prot_sg_count(iser_task->sc))
		wr->prot = &prot_reg->sge;
	else
		wr->prot = NULL;
	wr->access_flags = IB_ACCESS_LOCAL_WRITE |
			   IB_ACCESS_REMOTE_READ |
			   IB_ACCESS_REMOTE_WRITE;
	pi_ctx->sig_mr_valid = 1;

	sig_reg->sge.lkey = mr->lkey;
	sig_reg->rkey = mr->rkey;
	sig_reg->sge.addr = 0;
	sig_reg->sge.length = scsi_transfer_length(iser_task->sc);

	iser_dbg("lkey=0x%x rkey=0x%x addr=0x%llx length=%u\n",
		 sig_reg->sge.lkey, sig_reg->rkey, sig_reg->sge.addr,
		 sig_reg->sge.length);
>>>>>>> v4.9.227
err:
	return ret;
}

static int iser_fast_reg_mr(struct iscsi_iser_task *iser_task,
<<<<<<< HEAD
			    struct iser_regd_buf *regd_buf,
			    struct iser_data_buf *mem,
			    enum iser_reg_indicator ind,
			    struct ib_sge *sge)
{
	struct fast_reg_descriptor *desc = regd_buf->reg.mem_h;
	struct ib_conn *ib_conn = &iser_task->iser_conn->ib_conn;
	struct iser_device *device = ib_conn->device;
	struct ib_device *ibdev = device->ib_device;
	struct ib_mr *mr;
	struct ib_fast_reg_page_list *frpl;
	struct ib_send_wr fastreg_wr, inv_wr;
	struct ib_send_wr *bad_wr, *wr = NULL;
	u8 key;
	int ret, offset, size, plen;

	/* if there a single dma entry, dma mr suffices */
	if (mem->dma_nents == 1) {
		struct scatterlist *sg = (struct scatterlist *)mem->buf;

		sge->lkey = device->mr->lkey;
		sge->addr   = ib_sg_dma_address(ibdev, &sg[0]);
		sge->length  = ib_sg_dma_len(ibdev, &sg[0]);

		iser_dbg("Single DMA entry: lkey=0x%x, addr=0x%llx, length=0x%x\n",
			 sge->lkey, sge->addr, sge->length);
		return 0;
	}

	if (ind == ISER_DATA_KEY_VALID) {
		mr = desc->data_mr;
		frpl = desc->data_frpl;
	} else {
		mr = desc->pi_ctx->prot_mr;
		frpl = desc->pi_ctx->prot_frpl;
	}

	plen = iser_sg_to_page_vec(mem, device->ib_device, frpl->page_list,
				   &offset, &size);
	if (plen * SIZE_4K < size) {
		iser_err("fast reg page_list too short to hold this SG\n");
		return -EINVAL;
	}

	if (!(desc->reg_indicators & ind)) {
		memset(&inv_wr, 0, sizeof(inv_wr));
		inv_wr.wr_id = ISER_FASTREG_LI_WRID;
		inv_wr.opcode = IB_WR_LOCAL_INV;
		inv_wr.ex.invalidate_rkey = mr->rkey;
		wr = &inv_wr;
		/* Bump the key */
		key = (u8)(mr->rkey & 0x000000FF);
		ib_update_fast_reg_key(mr, ++key);
	}

	/* Prepare FASTREG WR */
	memset(&fastreg_wr, 0, sizeof(fastreg_wr));
	fastreg_wr.wr_id = ISER_FASTREG_LI_WRID;
	fastreg_wr.opcode = IB_WR_FAST_REG_MR;
	fastreg_wr.wr.fast_reg.iova_start = frpl->page_list[0] + offset;
	fastreg_wr.wr.fast_reg.page_list = frpl;
	fastreg_wr.wr.fast_reg.page_list_len = plen;
	fastreg_wr.wr.fast_reg.page_shift = SHIFT_4K;
	fastreg_wr.wr.fast_reg.length = size;
	fastreg_wr.wr.fast_reg.rkey = mr->rkey;
	fastreg_wr.wr.fast_reg.access_flags = (IB_ACCESS_LOCAL_WRITE  |
					       IB_ACCESS_REMOTE_WRITE |
					       IB_ACCESS_REMOTE_READ);

	if (!wr)
		wr = &fastreg_wr;
	else
		wr->next = &fastreg_wr;

	ret = ib_post_send(ib_conn->qp, wr, &bad_wr);
	if (ret) {
		iser_err("fast registration failed, ret:%d\n", ret);
		return ret;
	}
	desc->reg_indicators &= ~ind;

	sge->lkey = mr->lkey;
	sge->addr = frpl->page_list[0] + offset;
	sge->length = size;

	return ret;
}

/**
 * iser_reg_rdma_mem_fastreg - Registers memory intended for RDMA,
 * using Fast Registration WR (if possible) obtaining rkey and va
 *
 * returns 0 on success, errno code on failure
 */
int iser_reg_rdma_mem_fastreg(struct iscsi_iser_task *iser_task,
			      enum iser_data_dir cmd_dir)
{
	struct ib_conn *ib_conn = &iser_task->iser_conn->ib_conn;
	struct iser_device *device = ib_conn->device;
	struct ib_device *ibdev = device->ib_device;
	struct iser_data_buf *mem = &iser_task->data[cmd_dir];
	struct iser_regd_buf *regd_buf = &iser_task->rdma_regd[cmd_dir];
	struct fast_reg_descriptor *desc = NULL;
	struct ib_sge data_sge;
	int err, aligned_len;
	unsigned long flags;

	aligned_len = iser_data_buf_aligned_len(mem, ibdev);
	if (aligned_len != mem->dma_nents) {
		err = fall_to_bounce_buf(iser_task, ibdev, mem,
					 &iser_task->data_copy[cmd_dir],
					 cmd_dir, aligned_len);
		if (err) {
			iser_err("failed to allocate bounce buffer\n");
			return err;
		}
		mem = &iser_task->data_copy[cmd_dir];
	}

	if (mem->dma_nents != 1 ||
	    scsi_get_prot_op(iser_task->sc) != SCSI_PROT_NORMAL) {
		spin_lock_irqsave(&ib_conn->lock, flags);
		desc = list_first_entry(&ib_conn->fastreg.pool,
					struct fast_reg_descriptor, list);
		list_del(&desc->list);
		spin_unlock_irqrestore(&ib_conn->lock, flags);
		regd_buf->reg.mem_h = desc;
	}

	err = iser_fast_reg_mr(iser_task, regd_buf, mem,
			       ISER_DATA_KEY_VALID, &data_sge);
	if (err)
		goto err_reg;

	if (scsi_get_prot_op(iser_task->sc) != SCSI_PROT_NORMAL) {
		struct ib_sge prot_sge, sig_sge;

		memset(&prot_sge, 0, sizeof(prot_sge));
		if (scsi_prot_sg_count(iser_task->sc)) {
			mem = &iser_task->prot[cmd_dir];
			aligned_len = iser_data_buf_aligned_len(mem, ibdev);
			if (aligned_len != mem->dma_nents) {
				err = fall_to_bounce_buf(iser_task, ibdev, mem,
							 &iser_task->prot_copy[cmd_dir],
							 cmd_dir, aligned_len);
				if (err) {
					iser_err("failed to allocate bounce buffer\n");
					return err;
				}
				mem = &iser_task->prot_copy[cmd_dir];
			}

			err = iser_fast_reg_mr(iser_task, regd_buf, mem,
					       ISER_PROT_KEY_VALID, &prot_sge);
			if (err)
				goto err_reg;
		}

		err = iser_reg_sig_mr(iser_task, desc, &data_sge,
				      &prot_sge, &sig_sge);
		if (err) {
			iser_err("Failed to register signature mr\n");
			return err;
		}
		desc->reg_indicators |= ISER_FASTREG_PROTECTED;

		regd_buf->reg.lkey = sig_sge.lkey;
		regd_buf->reg.rkey = desc->pi_ctx->sig_mr->rkey;
		regd_buf->reg.va = sig_sge.addr;
		regd_buf->reg.len = sig_sge.length;
		regd_buf->reg.is_mr = 1;
	} else {
		if (desc) {
			regd_buf->reg.rkey = desc->data_mr->rkey;
			regd_buf->reg.is_mr = 1;
		} else {
			regd_buf->reg.rkey = device->mr->rkey;
			regd_buf->reg.is_mr = 0;
		}

		regd_buf->reg.lkey = data_sge.lkey;
		regd_buf->reg.va = data_sge.addr;
		regd_buf->reg.len = data_sge.length;
	}

	return 0;
err_reg:
	if (desc) {
		spin_lock_irqsave(&ib_conn->lock, flags);
		list_add_tail(&desc->list, &ib_conn->fastreg.pool);
		spin_unlock_irqrestore(&ib_conn->lock, flags);
	}

	return err;
}
=======
			    struct iser_data_buf *mem,
			    struct iser_reg_resources *rsc,
			    struct iser_mem_reg *reg)
{
	struct iser_tx_desc *tx_desc = &iser_task->desc;
	struct ib_cqe *cqe = &iser_task->iser_conn->ib_conn.reg_cqe;
	struct ib_mr *mr = rsc->mr;
	struct ib_reg_wr *wr;
	int n;

	if (rsc->mr_valid)
		iser_inv_rkey(iser_tx_next_wr(tx_desc), mr, cqe);

	ib_update_fast_reg_key(mr, ib_inc_rkey(mr->rkey));

	n = ib_map_mr_sg(mr, mem->sg, mem->dma_nents, NULL, SIZE_4K);
	if (unlikely(n != mem->dma_nents)) {
		iser_err("failed to map sg (%d/%d)\n",
			 n, mem->dma_nents);
		return n < 0 ? n : -EINVAL;
	}

	wr = reg_wr(iser_tx_next_wr(tx_desc));
	wr->wr.opcode = IB_WR_REG_MR;
	wr->wr.wr_cqe = cqe;
	wr->wr.send_flags = 0;
	wr->wr.num_sge = 0;
	wr->mr = mr;
	wr->key = mr->rkey;
	wr->access = IB_ACCESS_LOCAL_WRITE  |
		     IB_ACCESS_REMOTE_WRITE |
		     IB_ACCESS_REMOTE_READ;

	rsc->mr_valid = 1;

	reg->sge.lkey = mr->lkey;
	reg->rkey = mr->rkey;
	reg->sge.addr = mr->iova;
	reg->sge.length = mr->length;

	iser_dbg("lkey=0x%x rkey=0x%x addr=0x%llx length=0x%x\n",
		 reg->sge.lkey, reg->rkey, reg->sge.addr, reg->sge.length);

	return 0;
}

static int
iser_reg_prot_sg(struct iscsi_iser_task *task,
		 struct iser_data_buf *mem,
		 struct iser_fr_desc *desc,
		 bool use_dma_key,
		 struct iser_mem_reg *reg)
{
	struct iser_device *device = task->iser_conn->ib_conn.device;

	if (use_dma_key)
		return iser_reg_dma(device, mem, reg);

	return device->reg_ops->reg_mem(task, mem, &desc->pi_ctx->rsc, reg);
}

static int
iser_reg_data_sg(struct iscsi_iser_task *task,
		 struct iser_data_buf *mem,
		 struct iser_fr_desc *desc,
		 bool use_dma_key,
		 struct iser_mem_reg *reg)
{
	struct iser_device *device = task->iser_conn->ib_conn.device;

	if (use_dma_key)
		return iser_reg_dma(device, mem, reg);

	return device->reg_ops->reg_mem(task, mem, &desc->rsc, reg);
}

int iser_reg_rdma_mem(struct iscsi_iser_task *task,
		      enum iser_data_dir dir,
		      bool all_imm)
{
	struct ib_conn *ib_conn = &task->iser_conn->ib_conn;
	struct iser_device *device = ib_conn->device;
	struct iser_data_buf *mem = &task->data[dir];
	struct iser_mem_reg *reg = &task->rdma_reg[dir];
	struct iser_mem_reg *data_reg;
	struct iser_fr_desc *desc = NULL;
	bool use_dma_key;
	int err;

	use_dma_key = mem->dma_nents == 1 && (all_imm || !iser_always_reg) &&
		      scsi_get_prot_op(task->sc) == SCSI_PROT_NORMAL;

	if (!use_dma_key) {
		desc = device->reg_ops->reg_desc_get(ib_conn);
		reg->mem_h = desc;
	}

	if (scsi_get_prot_op(task->sc) == SCSI_PROT_NORMAL)
		data_reg = reg;
	else
		data_reg = &task->desc.data_reg;

	err = iser_reg_data_sg(task, mem, desc, use_dma_key, data_reg);
	if (unlikely(err))
		goto err_reg;

	if (scsi_get_prot_op(task->sc) != SCSI_PROT_NORMAL) {
		struct iser_mem_reg *prot_reg = &task->desc.prot_reg;

		if (scsi_prot_sg_count(task->sc)) {
			mem = &task->prot[dir];
			err = iser_reg_prot_sg(task, mem, desc,
					       use_dma_key, prot_reg);
			if (unlikely(err))
				goto err_reg;
		}

		err = iser_reg_sig_mr(task, desc->pi_ctx, data_reg,
				      prot_reg, reg);
		if (unlikely(err))
			goto err_reg;

		desc->pi_ctx->sig_protected = 1;
	}

	return 0;

err_reg:
	if (desc)
		device->reg_ops->reg_desc_put(ib_conn, desc);

	return err;
}

void iser_unreg_rdma_mem(struct iscsi_iser_task *task,
			 enum iser_data_dir dir)
{
	struct iser_device *device = task->iser_conn->ib_conn.device;

	device->reg_ops->unreg_mem(task, dir);
}
>>>>>>> v4.9.227
