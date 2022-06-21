/*
<<<<<<< HEAD
 * drivers/staging/android/ion/ion_page_pool.c
=======
 * drivers/staging/android/ion/ion_mem_pool.c
>>>>>>> v4.9.227
 *
 * Copyright (C) 2011 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/debugfs.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/list.h>
<<<<<<< HEAD
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/swap.h>
#include <linux/vmalloc.h>
=======
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/swap.h>
>>>>>>> v4.9.227
#include "ion_priv.h"

static void *ion_page_pool_alloc_pages(struct ion_page_pool *pool)
{
<<<<<<< HEAD
	struct page *page;

	page = alloc_pages(pool->gfp_mask & ~__GFP_ZERO, pool->order);

	if (!page)
		return NULL;

	if (pool->gfp_mask & __GFP_ZERO)
		if (msm_ion_heap_high_order_page_zero(page, pool->order))
			goto error_free_pages;

	ion_page_pool_alloc_set_cache_policy(pool, page);

	return page;
error_free_pages:
	__free_pages(page, pool->order);
	return NULL;
=======
	struct page *page = alloc_pages(pool->gfp_mask, pool->order);

	if (!page)
		return NULL;
	if (!pool->cached)
		ion_pages_sync_for_device(NULL, page, PAGE_SIZE << pool->order,
					  DMA_BIDIRECTIONAL);
	return page;
>>>>>>> v4.9.227
}

static void ion_page_pool_free_pages(struct ion_page_pool *pool,
				     struct page *page)
{
<<<<<<< HEAD
	ion_page_pool_free_set_cache_policy(pool, page);
	__free_pages(page, pool->order);
}

static int ion_page_pool_add(struct ion_page_pool *pool, struct page *page,
				bool prefetch)
=======
	__free_pages(page, pool->order);
}

static int ion_page_pool_add(struct ion_page_pool *pool, struct page *page)
>>>>>>> v4.9.227
{
	mutex_lock(&pool->mutex);
	if (PageHighMem(page)) {
		list_add_tail(&page->lru, &pool->high_items);
		pool->high_count++;
	} else {
		list_add_tail(&page->lru, &pool->low_items);
		pool->low_count++;
	}
<<<<<<< HEAD
	if (!prefetch)
		pool->nr_unreserved++;

=======
>>>>>>> v4.9.227
	mutex_unlock(&pool->mutex);
	return 0;
}

<<<<<<< HEAD
static struct page *ion_page_pool_remove(struct ion_page_pool *pool, bool high,
					bool prefetch)
=======
static struct page *ion_page_pool_remove(struct ion_page_pool *pool, bool high)
>>>>>>> v4.9.227
{
	struct page *page;

	if (high) {
		BUG_ON(!pool->high_count);
		page = list_first_entry(&pool->high_items, struct page, lru);
		pool->high_count--;
	} else {
		BUG_ON(!pool->low_count);
		page = list_first_entry(&pool->low_items, struct page, lru);
		pool->low_count--;
	}

<<<<<<< HEAD
	if (prefetch) {
		BUG_ON(!pool->nr_unreserved);
		pool->nr_unreserved--;
	}
	pool->nr_unreserved = min_t(int, pool->high_count + pool->low_count,
						pool->nr_unreserved);

=======
>>>>>>> v4.9.227
	list_del(&page->lru);
	return page;
}

<<<<<<< HEAD
void *ion_page_pool_alloc(struct ion_page_pool *pool, bool *from_pool)
=======
struct page *ion_page_pool_alloc(struct ion_page_pool *pool)
>>>>>>> v4.9.227
{
	struct page *page = NULL;

	BUG_ON(!pool);

<<<<<<< HEAD
	*from_pool = true;

	if (mutex_trylock(&pool->mutex)) {
		if (pool->high_count)
			page = ion_page_pool_remove(pool, true, false);
		else if (pool->low_count)
			page = ion_page_pool_remove(pool, false, false);
		mutex_unlock(&pool->mutex);
	}
	if (!page) {
		page = ion_page_pool_alloc_pages(pool);
		*from_pool = false;
	}
	return page;
}

void *ion_page_pool_prefetch(struct ion_page_pool *pool, bool *from_pool)
{
	struct page *page = NULL;

	BUG_ON(!pool);

	*from_pool = true;

	if (mutex_trylock(&pool->mutex)) {
		if (pool->high_count && pool->nr_unreserved > 0)
			page = ion_page_pool_remove(pool, true, true);
		else if (pool->low_count && pool->nr_unreserved > 0)
			page = ion_page_pool_remove(pool, false, true);
		mutex_unlock(&pool->mutex);
	}
	if (!page) {
		page = ion_page_pool_alloc_pages(pool);
		*from_pool = false;
	}
	return page;
}
/*
 * Tries to allocate from only the specified Pool and returns NULL otherwise
 */
void *ion_page_pool_alloc_pool_only(struct ion_page_pool *pool)
{
	struct page *page = NULL;

	BUG_ON(!pool);

	if (mutex_trylock(&pool->mutex)) {
		if (pool->high_count)
			page = ion_page_pool_remove(pool, true, false);
		else if (pool->low_count)
			page = ion_page_pool_remove(pool, false, false);
		mutex_unlock(&pool->mutex);
	}
=======
	mutex_lock(&pool->mutex);
	if (pool->high_count)
		page = ion_page_pool_remove(pool, true);
	else if (pool->low_count)
		page = ion_page_pool_remove(pool, false);
	mutex_unlock(&pool->mutex);

	if (!page)
		page = ion_page_pool_alloc_pages(pool);
>>>>>>> v4.9.227

	return page;
}

<<<<<<< HEAD
void ion_page_pool_free(struct ion_page_pool *pool, struct page *page,
			bool prefetch)
=======
void ion_page_pool_free(struct ion_page_pool *pool, struct page *page)
>>>>>>> v4.9.227
{
	int ret;

	BUG_ON(pool->order != compound_order(page));

<<<<<<< HEAD
	ret = ion_page_pool_add(pool, page, prefetch);
	/* FIXME? For a secure page, not hyp unassigned in this err path */
=======
	ret = ion_page_pool_add(pool, page);
>>>>>>> v4.9.227
	if (ret)
		ion_page_pool_free_pages(pool, page);
}

<<<<<<< HEAD
void ion_page_pool_free_immediate(struct ion_page_pool *pool, struct page *page)
{
	ion_page_pool_free_pages(pool, page);
}

int ion_page_pool_total(struct ion_page_pool *pool, bool high)
=======
static int ion_page_pool_total(struct ion_page_pool *pool, bool high)
>>>>>>> v4.9.227
{
	int count = pool->low_count;

	if (high)
		count += pool->high_count;

	return count << pool->order;
}

int ion_page_pool_shrink(struct ion_page_pool *pool, gfp_t gfp_mask,
<<<<<<< HEAD
				int nr_to_scan)
=======
			 int nr_to_scan)
>>>>>>> v4.9.227
{
	int freed = 0;
	bool high;

	if (current_is_kswapd())
		high = true;
	else
		high = !!(gfp_mask & __GFP_HIGHMEM);

	if (nr_to_scan == 0)
		return ion_page_pool_total(pool, high);

	while (freed < nr_to_scan) {
		struct page *page;

		mutex_lock(&pool->mutex);
		if (pool->low_count) {
<<<<<<< HEAD
			page = ion_page_pool_remove(pool, false, false);
		} else if (high && pool->high_count) {
			page = ion_page_pool_remove(pool, true, false);
=======
			page = ion_page_pool_remove(pool, false);
		} else if (high && pool->high_count) {
			page = ion_page_pool_remove(pool, true);
>>>>>>> v4.9.227
		} else {
			mutex_unlock(&pool->mutex);
			break;
		}
		mutex_unlock(&pool->mutex);
		ion_page_pool_free_pages(pool, page);
		freed += (1 << pool->order);
	}

	return freed;
}

<<<<<<< HEAD
struct ion_page_pool *ion_page_pool_create(gfp_t gfp_mask, unsigned int order)
{
	struct ion_page_pool *pool = kmalloc(sizeof(struct ion_page_pool),
					     GFP_KERNEL);
=======
struct ion_page_pool *ion_page_pool_create(gfp_t gfp_mask, unsigned int order,
					   bool cached)
{
	struct ion_page_pool *pool = kmalloc(sizeof(*pool), GFP_KERNEL);

>>>>>>> v4.9.227
	if (!pool)
		return NULL;
	pool->high_count = 0;
	pool->low_count = 0;
<<<<<<< HEAD
	pool->nr_unreserved = 0;
=======
>>>>>>> v4.9.227
	INIT_LIST_HEAD(&pool->low_items);
	INIT_LIST_HEAD(&pool->high_items);
	pool->gfp_mask = gfp_mask | __GFP_COMP;
	pool->order = order;
	mutex_init(&pool->mutex);
	plist_node_init(&pool->list, order);
<<<<<<< HEAD
=======
	if (cached)
		pool->cached = true;
>>>>>>> v4.9.227

	return pool;
}

void ion_page_pool_destroy(struct ion_page_pool *pool)
{
	kfree(pool);
}

static int __init ion_page_pool_init(void)
{
	return 0;
}
<<<<<<< HEAD

static void __exit ion_page_pool_exit(void)
{
}

module_init(ion_page_pool_init);
module_exit(ion_page_pool_exit);
=======
device_initcall(ion_page_pool_init);
>>>>>>> v4.9.227
