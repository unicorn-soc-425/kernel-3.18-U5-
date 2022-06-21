/*
 * GPL HEADER START
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 only,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License version 2 for more details (a copy is included
 * in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; If not, see
<<<<<<< HEAD
 * http://www.sun.com/software/products/lustre/docs/GPLv2.pdf
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
=======
 * http://www.gnu.org/licenses/gpl-2.0.html
>>>>>>> v4.9.227
 *
 * GPL HEADER END
 */
/*
 * Copyright (c) 2007, 2010, Oracle and/or its affiliates. All rights reserved.
 * Use is subject to license terms.
 *
 * Copyright (c) 2012, 2013, Intel Corporation.
 */
/*
 * This file is part of Lustre, http://www.lustre.org/
 * Lustre is a trademark of Sun Microsystems, Inc.
 *
 * lustre/fld/fld_cache.c
 *
 * FLD (Fids Location Database)
 *
 * Author: Pravin Shelar <pravin.shelar@sun.com>
 * Author: Yury Umanets <umka@clusterfs.com>
 */

#define DEBUG_SUBSYSTEM S_FLD

#include "../../include/linux/libcfs/libcfs.h"
#include <linux/module.h>
#include <asm/div64.h>

#include "../include/obd.h"
#include "../include/obd_class.h"
#include "../include/lustre_ver.h"
#include "../include/obd_support.h"
#include "../include/lprocfs_status.h"

<<<<<<< HEAD
#include "../include/dt_object.h"
=======
>>>>>>> v4.9.227
#include "../include/lustre_req_layout.h"
#include "../include/lustre_fld.h"
#include "fld_internal.h"

/**
 * create fld cache.
 */
struct fld_cache *fld_cache_init(const char *name,
				 int cache_size, int cache_threshold)
{
	struct fld_cache *cache;

<<<<<<< HEAD
	LASSERT(name != NULL);
	LASSERT(cache_threshold < cache_size);

	OBD_ALLOC_PTR(cache);
	if (cache == NULL)
=======
	LASSERT(name);
	LASSERT(cache_threshold < cache_size);

	cache = kzalloc(sizeof(*cache), GFP_NOFS);
	if (!cache)
>>>>>>> v4.9.227
		return ERR_PTR(-ENOMEM);

	INIT_LIST_HEAD(&cache->fci_entries_head);
	INIT_LIST_HEAD(&cache->fci_lru);

	cache->fci_cache_count = 0;
	rwlock_init(&cache->fci_lock);

	strlcpy(cache->fci_name, name,
		sizeof(cache->fci_name));

	cache->fci_cache_size = cache_size;
	cache->fci_threshold = cache_threshold;

	/* Init fld cache info. */
	memset(&cache->fci_stat, 0, sizeof(cache->fci_stat));

	CDEBUG(D_INFO, "%s: FLD cache - Size: %d, Threshold: %d\n",
	       cache->fci_name, cache_size, cache_threshold);

	return cache;
}

/**
 * destroy fld cache.
 */
void fld_cache_fini(struct fld_cache *cache)
{
	__u64 pct;

<<<<<<< HEAD
	LASSERT(cache != NULL);
=======
	LASSERT(cache);
>>>>>>> v4.9.227
	fld_cache_flush(cache);

	if (cache->fci_stat.fst_count > 0) {
		pct = cache->fci_stat.fst_cache * 100;
		do_div(pct, cache->fci_stat.fst_count);
	} else {
		pct = 0;
	}

	CDEBUG(D_INFO, "FLD cache statistics (%s):\n", cache->fci_name);
	CDEBUG(D_INFO, "  Total reqs: %llu\n", cache->fci_stat.fst_count);
	CDEBUG(D_INFO, "  Cache reqs: %llu\n", cache->fci_stat.fst_cache);
	CDEBUG(D_INFO, "  Cache hits: %llu%%\n", pct);

<<<<<<< HEAD
	OBD_FREE_PTR(cache);
=======
	kfree(cache);
>>>>>>> v4.9.227
}

/**
 * delete given node from list.
 */
<<<<<<< HEAD
void fld_cache_entry_delete(struct fld_cache *cache,
			    struct fld_cache_entry *node)
=======
static void fld_cache_entry_delete(struct fld_cache *cache,
				   struct fld_cache_entry *node)
>>>>>>> v4.9.227
{
	list_del(&node->fce_list);
	list_del(&node->fce_lru);
	cache->fci_cache_count--;
<<<<<<< HEAD
	OBD_FREE_PTR(node);
=======
	kfree(node);
>>>>>>> v4.9.227
}

/**
 * fix list by checking new entry with NEXT entry in order.
 */
static void fld_fix_new_list(struct fld_cache *cache)
{
	struct fld_cache_entry *f_curr;
	struct fld_cache_entry *f_next;
	struct lu_seq_range *c_range;
	struct lu_seq_range *n_range;
	struct list_head *head = &cache->fci_entries_head;

restart_fixup:

	list_for_each_entry_safe(f_curr, f_next, head, fce_list) {
		c_range = &f_curr->fce_range;
		n_range = &f_next->fce_range;

		LASSERT(range_is_sane(c_range));
		if (&f_next->fce_list == head)
			break;

		if (c_range->lsr_flags != n_range->lsr_flags)
			continue;

		LASSERTF(c_range->lsr_start <= n_range->lsr_start,
			 "cur lsr_start "DRANGE" next lsr_start "DRANGE"\n",
			 PRANGE(c_range), PRANGE(n_range));

		/* check merge possibility with next range */
		if (c_range->lsr_end == n_range->lsr_start) {
			if (c_range->lsr_index != n_range->lsr_index)
				continue;
			n_range->lsr_start = c_range->lsr_start;
			fld_cache_entry_delete(cache, f_curr);
			continue;
		}

		/* check if current range overlaps with next range. */
		if (n_range->lsr_start < c_range->lsr_end) {
			if (c_range->lsr_index == n_range->lsr_index) {
				n_range->lsr_start = c_range->lsr_start;
				n_range->lsr_end = max(c_range->lsr_end,
						       n_range->lsr_end);
				fld_cache_entry_delete(cache, f_curr);
			} else {
				if (n_range->lsr_end <= c_range->lsr_end) {
					*n_range = *c_range;
					fld_cache_entry_delete(cache, f_curr);
<<<<<<< HEAD
				} else
					n_range->lsr_start = c_range->lsr_end;
			}

			/* we could have overlap over next
			 * range too. better restart. */
=======
				} else {
					n_range->lsr_start = c_range->lsr_end;
				}
			}

			/* we could have overlap over next
			 * range too. better restart.
			 */
>>>>>>> v4.9.227
			goto restart_fixup;
		}

		/* kill duplicates */
		if (c_range->lsr_start == n_range->lsr_start &&
		    c_range->lsr_end == n_range->lsr_end)
			fld_cache_entry_delete(cache, f_curr);
	}
}

/**
 * add node to fld cache
 */
static inline void fld_cache_entry_add(struct fld_cache *cache,
				       struct fld_cache_entry *f_new,
				       struct list_head *pos)
{
	list_add(&f_new->fce_list, pos);
	list_add(&f_new->fce_lru, &cache->fci_lru);

	cache->fci_cache_count++;
	fld_fix_new_list(cache);
}

/**
 * Check if cache needs to be shrunk. If so - do it.
 * Remove one entry in list and so on until cache is shrunk enough.
 */
static int fld_cache_shrink(struct fld_cache *cache)
{
	struct fld_cache_entry *flde;
	struct list_head *curr;
	int num = 0;

<<<<<<< HEAD
	LASSERT(cache != NULL);

=======
>>>>>>> v4.9.227
	if (cache->fci_cache_count < cache->fci_cache_size)
		return 0;

	curr = cache->fci_lru.prev;

	while (cache->fci_cache_count + cache->fci_threshold >
	       cache->fci_cache_size && curr != &cache->fci_lru) {
<<<<<<< HEAD

=======
>>>>>>> v4.9.227
		flde = list_entry(curr, struct fld_cache_entry, fce_lru);
		curr = curr->prev;
		fld_cache_entry_delete(cache, flde);
		num++;
	}

	CDEBUG(D_INFO, "%s: FLD cache - Shrunk by %d entries\n",
<<<<<<< HEAD
			cache->fci_name, num);
=======
	       cache->fci_name, num);
>>>>>>> v4.9.227

	return 0;
}

/**
 * kill all fld cache entries.
 */
void fld_cache_flush(struct fld_cache *cache)
{
	write_lock(&cache->fci_lock);
	cache->fci_cache_size = 0;
	fld_cache_shrink(cache);
	write_unlock(&cache->fci_lock);
}

/**
 * punch hole in existing range. divide this range and add new
 * entry accordingly.
 */

<<<<<<< HEAD
void fld_cache_punch_hole(struct fld_cache *cache,
			  struct fld_cache_entry *f_curr,
			  struct fld_cache_entry *f_new)
=======
static void fld_cache_punch_hole(struct fld_cache *cache,
				 struct fld_cache_entry *f_curr,
				 struct fld_cache_entry *f_new)
>>>>>>> v4.9.227
{
	const struct lu_seq_range *range = &f_new->fce_range;
	const u64 new_start  = range->lsr_start;
	const u64 new_end  = range->lsr_end;
	struct fld_cache_entry *fldt;

<<<<<<< HEAD
	OBD_ALLOC_GFP(fldt, sizeof(*fldt), GFP_ATOMIC);
	if (!fldt) {
		OBD_FREE_PTR(f_new);
=======
	fldt = kzalloc(sizeof(*fldt), GFP_ATOMIC);
	if (!fldt) {
		kfree(f_new);
>>>>>>> v4.9.227
		/* overlap is not allowed, so dont mess up list. */
		return;
	}
	/*  break f_curr RANGE into three RANGES:
	 *	f_curr, f_new , fldt
	 */

	/* f_new = *range */

	/* fldt */
	fldt->fce_range.lsr_start = new_end;
	fldt->fce_range.lsr_end = f_curr->fce_range.lsr_end;
	fldt->fce_range.lsr_index = f_curr->fce_range.lsr_index;

	/* f_curr */
	f_curr->fce_range.lsr_end = new_start;

	/* add these two entries to list */
	fld_cache_entry_add(cache, f_new, &f_curr->fce_list);
	fld_cache_entry_add(cache, fldt, &f_new->fce_list);

	/* no need to fixup */
}

/**
 * handle range overlap in fld cache.
 */
static void fld_cache_overlap_handle(struct fld_cache *cache,
<<<<<<< HEAD
				struct fld_cache_entry *f_curr,
				struct fld_cache_entry *f_new)
=======
				     struct fld_cache_entry *f_curr,
				     struct fld_cache_entry *f_new)
>>>>>>> v4.9.227
{
	const struct lu_seq_range *range = &f_new->fce_range;
	const u64 new_start  = range->lsr_start;
	const u64 new_end  = range->lsr_end;
	const u32 mdt = range->lsr_index;

	/* this is overlap case, these case are checking overlapping with
<<<<<<< HEAD
	 * prev range only. fixup will handle overlapping with next range. */
=======
	 * prev range only. fixup will handle overlapping with next range.
	 */
>>>>>>> v4.9.227

	if (f_curr->fce_range.lsr_index == mdt) {
		f_curr->fce_range.lsr_start = min(f_curr->fce_range.lsr_start,
						  new_start);

		f_curr->fce_range.lsr_end = max(f_curr->fce_range.lsr_end,
						new_end);

<<<<<<< HEAD
		OBD_FREE_PTR(f_new);
=======
		kfree(f_new);
>>>>>>> v4.9.227
		fld_fix_new_list(cache);

	} else if (new_start <= f_curr->fce_range.lsr_start &&
			f_curr->fce_range.lsr_end <= new_end) {
		/* case 1: new range completely overshadowed existing range.
<<<<<<< HEAD
		 *	 e.g. whole range migrated. update fld cache entry */

		f_curr->fce_range = *range;
		OBD_FREE_PTR(f_new);
=======
		 *	 e.g. whole range migrated. update fld cache entry
		 */

		f_curr->fce_range = *range;
		kfree(f_new);
>>>>>>> v4.9.227
		fld_fix_new_list(cache);

	} else if (f_curr->fce_range.lsr_start < new_start &&
			new_end < f_curr->fce_range.lsr_end) {
		/* case 2: new range fit within existing range. */

		fld_cache_punch_hole(cache, f_curr, f_new);

	} else  if (new_end <= f_curr->fce_range.lsr_end) {
		/* case 3: overlap:
		 *	 [new_start [c_start  new_end)  c_end)
		 */

		LASSERT(new_start <= f_curr->fce_range.lsr_start);

		f_curr->fce_range.lsr_start = new_end;
		fld_cache_entry_add(cache, f_new, f_curr->fce_list.prev);

	} else if (f_curr->fce_range.lsr_start <= new_start) {
		/* case 4: overlap:
		 *	 [c_start [new_start c_end) new_end)
		 */

		LASSERT(f_curr->fce_range.lsr_end <= new_end);

		f_curr->fce_range.lsr_end = new_start;
		fld_cache_entry_add(cache, f_new, &f_curr->fce_list);
	} else
		CERROR("NEW range ="DRANGE" curr = "DRANGE"\n",
		       PRANGE(range), PRANGE(&f_curr->fce_range));
}

struct fld_cache_entry
*fld_cache_entry_create(const struct lu_seq_range *range)
{
	struct fld_cache_entry *f_new;

	LASSERT(range_is_sane(range));

<<<<<<< HEAD
	OBD_ALLOC_PTR(f_new);
=======
	f_new = kzalloc(sizeof(*f_new), GFP_NOFS);
>>>>>>> v4.9.227
	if (!f_new)
		return ERR_PTR(-ENOMEM);

	f_new->fce_range = *range;
	return f_new;
}

/**
 * Insert FLD entry in FLD cache.
 *
 * This function handles all cases of merging and breaking up of
 * ranges.
 */
<<<<<<< HEAD
int fld_cache_insert_nolock(struct fld_cache *cache,
			    struct fld_cache_entry *f_new)
=======
static int fld_cache_insert_nolock(struct fld_cache *cache,
				   struct fld_cache_entry *f_new)
>>>>>>> v4.9.227
{
	struct fld_cache_entry *f_curr;
	struct fld_cache_entry *n;
	struct list_head *head;
	struct list_head *prev = NULL;
	const u64 new_start  = f_new->fce_range.lsr_start;
	const u64 new_end  = f_new->fce_range.lsr_end;
	__u32 new_flags  = f_new->fce_range.lsr_flags;

	/*
	 * Duplicate entries are eliminated in insert op.
	 * So we don't need to search new entry before starting
	 * insertion loop.
	 */

	if (!cache->fci_no_shrink)
		fld_cache_shrink(cache);

	head = &cache->fci_entries_head;

	list_for_each_entry_safe(f_curr, n, head, fce_list) {
		/* add list if next is end of list */
		if (new_end < f_curr->fce_range.lsr_start ||
<<<<<<< HEAD
		   (new_end == f_curr->fce_range.lsr_start &&
		    new_flags != f_curr->fce_range.lsr_flags))
=======
		    (new_end == f_curr->fce_range.lsr_start &&
		     new_flags != f_curr->fce_range.lsr_flags))
>>>>>>> v4.9.227
			break;

		prev = &f_curr->fce_list;
		/* check if this range is to left of new range. */
		if (new_start < f_curr->fce_range.lsr_end &&
		    new_flags == f_curr->fce_range.lsr_flags) {
			fld_cache_overlap_handle(cache, f_curr, f_new);
			goto out;
		}
	}

<<<<<<< HEAD
	if (prev == NULL)
=======
	if (!prev)
>>>>>>> v4.9.227
		prev = head;

	CDEBUG(D_INFO, "insert range "DRANGE"\n", PRANGE(&f_new->fce_range));
	/* Add new entry to cache and lru list. */
	fld_cache_entry_add(cache, f_new, prev);
out:
	return 0;
}

int fld_cache_insert(struct fld_cache *cache,
		     const struct lu_seq_range *range)
{
	struct fld_cache_entry	*flde;
	int rc;

	flde = fld_cache_entry_create(range);
	if (IS_ERR(flde))
		return PTR_ERR(flde);

	write_lock(&cache->fci_lock);
	rc = fld_cache_insert_nolock(cache, flde);
	write_unlock(&cache->fci_lock);
	if (rc)
<<<<<<< HEAD
		OBD_FREE_PTR(flde);
=======
		kfree(flde);
>>>>>>> v4.9.227

	return rc;
}

<<<<<<< HEAD
void fld_cache_delete_nolock(struct fld_cache *cache,
		      const struct lu_seq_range *range)
{
	struct fld_cache_entry *flde;
	struct fld_cache_entry *tmp;
	struct list_head *head;

	head = &cache->fci_entries_head;
	list_for_each_entry_safe(flde, tmp, head, fce_list) {
		/* add list if next is end of list */
		if (range->lsr_start == flde->fce_range.lsr_start ||
		   (range->lsr_end == flde->fce_range.lsr_end &&
		    range->lsr_flags == flde->fce_range.lsr_flags)) {
			fld_cache_entry_delete(cache, flde);
			break;
		}
	}
}

=======
>>>>>>> v4.9.227
/**
 * Delete FLD entry in FLD cache.
 *
 */
<<<<<<< HEAD
void fld_cache_delete(struct fld_cache *cache,
		      const struct lu_seq_range *range)
{
	write_lock(&cache->fci_lock);
	fld_cache_delete_nolock(cache, range);
	write_unlock(&cache->fci_lock);
}
=======
>>>>>>> v4.9.227

struct fld_cache_entry
*fld_cache_entry_lookup_nolock(struct fld_cache *cache,
			      struct lu_seq_range *range)
{
	struct fld_cache_entry *flde;
	struct fld_cache_entry *got = NULL;
	struct list_head *head;

	head = &cache->fci_entries_head;
	list_for_each_entry(flde, head, fce_list) {
		if (range->lsr_start == flde->fce_range.lsr_start ||
<<<<<<< HEAD
		   (range->lsr_end == flde->fce_range.lsr_end &&
		    range->lsr_flags == flde->fce_range.lsr_flags)) {
=======
		    (range->lsr_end == flde->fce_range.lsr_end &&
		     range->lsr_flags == flde->fce_range.lsr_flags)) {
>>>>>>> v4.9.227
			got = flde;
			break;
		}
	}

	return got;
}

/**
 * lookup \a seq sequence for range in fld cache.
 */
struct fld_cache_entry
*fld_cache_entry_lookup(struct fld_cache *cache, struct lu_seq_range *range)
{
	struct fld_cache_entry *got = NULL;

	read_lock(&cache->fci_lock);
	got = fld_cache_entry_lookup_nolock(cache, range);
	read_unlock(&cache->fci_lock);
	return got;
}

/**
 * lookup \a seq sequence for range in fld cache.
 */
int fld_cache_lookup(struct fld_cache *cache,
		     const u64 seq, struct lu_seq_range *range)
{
	struct fld_cache_entry *flde;
	struct fld_cache_entry *prev = NULL;
	struct list_head *head;

	read_lock(&cache->fci_lock);
	head = &cache->fci_entries_head;

	cache->fci_stat.fst_count++;
	list_for_each_entry(flde, head, fce_list) {
		if (flde->fce_range.lsr_start > seq) {
<<<<<<< HEAD
			if (prev != NULL)
=======
			if (prev)
>>>>>>> v4.9.227
				*range = prev->fce_range;
			break;
		}

		prev = flde;
		if (range_within(&flde->fce_range, seq)) {
			*range = flde->fce_range;

			cache->fci_stat.fst_cache++;
			read_unlock(&cache->fci_lock);
			return 0;
		}
	}
	read_unlock(&cache->fci_lock);
	return -ENOENT;
}
