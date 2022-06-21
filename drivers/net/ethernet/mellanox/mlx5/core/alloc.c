/*
<<<<<<< HEAD
 * Copyright (c) 2013, Mellanox Technologies inc.  All rights reserved.
=======
 * Copyright (c) 2013-2015, Mellanox Technologies. All rights reserved.
>>>>>>> v4.9.227
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
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
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

#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/export.h>
#include <linux/bitmap.h>
#include <linux/dma-mapping.h>
#include <linux/vmalloc.h>
#include <linux/mlx5/driver.h>

#include "mlx5_core.h"

<<<<<<< HEAD
/* Handling for queue buffers -- we allocate a bunch of memory and
 * register it in a memory region at HCA virtual address 0.  If the
 * requested size is > max_direct, we split the allocation into
 * multiple pages, so we don't require too much contiguous memory.
 */

int mlx5_buf_alloc(struct mlx5_core_dev *dev, int size, int max_direct,
		   struct mlx5_buf *buf)
=======
struct mlx5_db_pgdir {
	struct list_head	list;
	unsigned long	       *bitmap;
	__be32		       *db_page;
	dma_addr_t		db_dma;
};

/* Handling for queue buffers -- we allocate a bunch of memory and
 * register it in a memory region at HCA virtual address 0.
 */

static void *mlx5_dma_zalloc_coherent_node(struct mlx5_core_dev *dev,
					   size_t size, dma_addr_t *dma_handle,
					   int node)
{
	struct mlx5_priv *priv = &dev->priv;
	int original_node;
	void *cpu_handle;

	mutex_lock(&priv->alloc_mutex);
	original_node = dev_to_node(&dev->pdev->dev);
	set_dev_node(&dev->pdev->dev, node);
	cpu_handle = dma_zalloc_coherent(&dev->pdev->dev, size,
					 dma_handle, GFP_KERNEL);
	set_dev_node(&dev->pdev->dev, original_node);
	mutex_unlock(&priv->alloc_mutex);
	return cpu_handle;
}

int mlx5_buf_alloc_node(struct mlx5_core_dev *dev, int size,
			struct mlx5_buf *buf, int node)
>>>>>>> v4.9.227
{
	dma_addr_t t;

	buf->size = size;
<<<<<<< HEAD
	if (size <= max_direct) {
		buf->nbufs        = 1;
		buf->npages       = 1;
		buf->page_shift   = (u8)get_order(size) + PAGE_SHIFT;
		buf->direct.buf   = dma_zalloc_coherent(&dev->pdev->dev,
							size, &t, GFP_KERNEL);
		if (!buf->direct.buf)
			return -ENOMEM;

		buf->direct.map = t;

		while (t & ((1 << buf->page_shift) - 1)) {
			--buf->page_shift;
			buf->npages *= 2;
		}
	} else {
		int i;

		buf->direct.buf  = NULL;
		buf->nbufs       = (size + PAGE_SIZE - 1) / PAGE_SIZE;
		buf->npages      = buf->nbufs;
		buf->page_shift  = PAGE_SHIFT;
		buf->page_list   = kcalloc(buf->nbufs, sizeof(*buf->page_list),
					   GFP_KERNEL);
		if (!buf->page_list)
			return -ENOMEM;

		for (i = 0; i < buf->nbufs; i++) {
			buf->page_list[i].buf =
				dma_zalloc_coherent(&dev->pdev->dev, PAGE_SIZE,
						    &t, GFP_KERNEL);
			if (!buf->page_list[i].buf)
				goto err_free;

			buf->page_list[i].map = t;
		}

		if (BITS_PER_LONG == 64) {
			struct page **pages;
			pages = kmalloc(sizeof(*pages) * buf->nbufs, GFP_KERNEL);
			if (!pages)
				goto err_free;
			for (i = 0; i < buf->nbufs; i++)
				pages[i] = virt_to_page(buf->page_list[i].buf);
			buf->direct.buf = vmap(pages, buf->nbufs, VM_MAP, PAGE_KERNEL);
			kfree(pages);
			if (!buf->direct.buf)
				goto err_free;
		}
	}

	return 0;

err_free:
	mlx5_buf_free(dev, buf);

	return -ENOMEM;
=======
	buf->npages       = 1;
	buf->page_shift   = (u8)get_order(size) + PAGE_SHIFT;
	buf->direct.buf   = mlx5_dma_zalloc_coherent_node(dev, size,
							  &t, node);
	if (!buf->direct.buf)
		return -ENOMEM;

	buf->direct.map = t;

	while (t & ((1 << buf->page_shift) - 1)) {
		--buf->page_shift;
		buf->npages *= 2;
	}

	return 0;
}

int mlx5_buf_alloc(struct mlx5_core_dev *dev, int size, struct mlx5_buf *buf)
{
	return mlx5_buf_alloc_node(dev, size, buf, dev->priv.numa_node);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(mlx5_buf_alloc);

void mlx5_buf_free(struct mlx5_core_dev *dev, struct mlx5_buf *buf)
{
<<<<<<< HEAD
	int i;

	if (buf->nbufs == 1)
		dma_free_coherent(&dev->pdev->dev, buf->size, buf->direct.buf,
				  buf->direct.map);
	else {
		if (BITS_PER_LONG == 64 && buf->direct.buf)
			vunmap(buf->direct.buf);

		for (i = 0; i < buf->nbufs; i++)
			if (buf->page_list[i].buf)
				dma_free_coherent(&dev->pdev->dev, PAGE_SIZE,
						  buf->page_list[i].buf,
						  buf->page_list[i].map);
		kfree(buf->page_list);
	}
}
EXPORT_SYMBOL_GPL(mlx5_buf_free);

static struct mlx5_db_pgdir *mlx5_alloc_db_pgdir(struct device *dma_device)
{
=======
	dma_free_coherent(&dev->pdev->dev, buf->size, buf->direct.buf,
			  buf->direct.map);
}
EXPORT_SYMBOL_GPL(mlx5_buf_free);

static struct mlx5_db_pgdir *mlx5_alloc_db_pgdir(struct mlx5_core_dev *dev,
						 int node)
{
	u32 db_per_page = PAGE_SIZE / cache_line_size();
>>>>>>> v4.9.227
	struct mlx5_db_pgdir *pgdir;

	pgdir = kzalloc(sizeof(*pgdir), GFP_KERNEL);
	if (!pgdir)
		return NULL;

<<<<<<< HEAD
	bitmap_fill(pgdir->bitmap, MLX5_DB_PER_PAGE);
	pgdir->db_page = dma_alloc_coherent(dma_device, PAGE_SIZE,
					    &pgdir->db_dma, GFP_KERNEL);
	if (!pgdir->db_page) {
=======
	pgdir->bitmap = kcalloc(BITS_TO_LONGS(db_per_page),
				sizeof(unsigned long),
				GFP_KERNEL);

	if (!pgdir->bitmap) {
		kfree(pgdir);
		return NULL;
	}

	bitmap_fill(pgdir->bitmap, db_per_page);

	pgdir->db_page = mlx5_dma_zalloc_coherent_node(dev, PAGE_SIZE,
						       &pgdir->db_dma, node);
	if (!pgdir->db_page) {
		kfree(pgdir->bitmap);
>>>>>>> v4.9.227
		kfree(pgdir);
		return NULL;
	}

	return pgdir;
}

static int mlx5_alloc_db_from_pgdir(struct mlx5_db_pgdir *pgdir,
				    struct mlx5_db *db)
{
<<<<<<< HEAD
	int offset;
	int i;

	i = find_first_bit(pgdir->bitmap, MLX5_DB_PER_PAGE);
	if (i >= MLX5_DB_PER_PAGE)
=======
	u32 db_per_page = PAGE_SIZE / cache_line_size();
	int offset;
	int i;

	i = find_first_bit(pgdir->bitmap, db_per_page);
	if (i >= db_per_page)
>>>>>>> v4.9.227
		return -ENOMEM;

	__clear_bit(i, pgdir->bitmap);

	db->u.pgdir = pgdir;
	db->index   = i;
<<<<<<< HEAD
	offset = db->index * L1_CACHE_BYTES;
	db->db      = pgdir->db_page + offset / sizeof(*pgdir->db_page);
	db->dma     = pgdir->db_dma  + offset;

	return 0;
}

int mlx5_db_alloc(struct mlx5_core_dev *dev, struct mlx5_db *db)
=======
	offset = db->index * cache_line_size();
	db->db      = pgdir->db_page + offset / sizeof(*pgdir->db_page);
	db->dma     = pgdir->db_dma  + offset;

	db->db[0] = 0;
	db->db[1] = 0;

	return 0;
}

int mlx5_db_alloc_node(struct mlx5_core_dev *dev, struct mlx5_db *db, int node)
>>>>>>> v4.9.227
{
	struct mlx5_db_pgdir *pgdir;
	int ret = 0;

	mutex_lock(&dev->priv.pgdir_mutex);

	list_for_each_entry(pgdir, &dev->priv.pgdir_list, list)
		if (!mlx5_alloc_db_from_pgdir(pgdir, db))
			goto out;

<<<<<<< HEAD
	pgdir = mlx5_alloc_db_pgdir(&(dev->pdev->dev));
=======
	pgdir = mlx5_alloc_db_pgdir(dev, node);
>>>>>>> v4.9.227
	if (!pgdir) {
		ret = -ENOMEM;
		goto out;
	}

	list_add(&pgdir->list, &dev->priv.pgdir_list);

	/* This should never fail -- we just allocated an empty page: */
	WARN_ON(mlx5_alloc_db_from_pgdir(pgdir, db));

out:
	mutex_unlock(&dev->priv.pgdir_mutex);

	return ret;
}
<<<<<<< HEAD
=======
EXPORT_SYMBOL_GPL(mlx5_db_alloc_node);

int mlx5_db_alloc(struct mlx5_core_dev *dev, struct mlx5_db *db)
{
	return mlx5_db_alloc_node(dev, db, dev->priv.numa_node);
}
>>>>>>> v4.9.227
EXPORT_SYMBOL_GPL(mlx5_db_alloc);

void mlx5_db_free(struct mlx5_core_dev *dev, struct mlx5_db *db)
{
<<<<<<< HEAD
=======
	u32 db_per_page = PAGE_SIZE / cache_line_size();
>>>>>>> v4.9.227
	mutex_lock(&dev->priv.pgdir_mutex);

	__set_bit(db->index, db->u.pgdir->bitmap);

<<<<<<< HEAD
	if (bitmap_full(db->u.pgdir->bitmap, MLX5_DB_PER_PAGE)) {
		dma_free_coherent(&(dev->pdev->dev), PAGE_SIZE,
				  db->u.pgdir->db_page, db->u.pgdir->db_dma);
		list_del(&db->u.pgdir->list);
=======
	if (bitmap_full(db->u.pgdir->bitmap, db_per_page)) {
		dma_free_coherent(&(dev->pdev->dev), PAGE_SIZE,
				  db->u.pgdir->db_page, db->u.pgdir->db_dma);
		list_del(&db->u.pgdir->list);
		kfree(db->u.pgdir->bitmap);
>>>>>>> v4.9.227
		kfree(db->u.pgdir);
	}

	mutex_unlock(&dev->priv.pgdir_mutex);
}
EXPORT_SYMBOL_GPL(mlx5_db_free);


void mlx5_fill_page_array(struct mlx5_buf *buf, __be64 *pas)
{
	u64 addr;
	int i;

	for (i = 0; i < buf->npages; i++) {
<<<<<<< HEAD
		if (buf->nbufs == 1)
			addr = buf->direct.map + (i << buf->page_shift);
		else
			addr = buf->page_list[i].map;
=======
		addr = buf->direct.map + (i << buf->page_shift);
>>>>>>> v4.9.227

		pas[i] = cpu_to_be64(addr);
	}
}
EXPORT_SYMBOL_GPL(mlx5_fill_page_array);
