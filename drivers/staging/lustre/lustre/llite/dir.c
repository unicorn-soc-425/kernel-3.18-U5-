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
 * Copyright (c) 2002, 2010, Oracle and/or its affiliates. All rights reserved.
 * Use is subject to license terms.
 *
<<<<<<< HEAD
 * Copyright (c) 2011, 2012, Intel Corporation.
=======
 * Copyright (c) 2011, 2015, Intel Corporation.
>>>>>>> v4.9.227
 */
/*
 * This file is part of Lustre, http://www.lustre.org/
 * Lustre is a trademark of Sun Microsystems, Inc.
 *
 * lustre/llite/dir.c
 *
 * Directory code for lustre client.
 */

#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/mm.h>
<<<<<<< HEAD
#include <asm/uaccess.h>
=======
#include <linux/uaccess.h>
>>>>>>> v4.9.227
#include <linux/buffer_head.h>   /* for wait_on_buffer */
#include <linux/pagevec.h>
#include <linux/prefetch.h>

#define DEBUG_SUBSYSTEM S_LLITE

#include "../include/obd_support.h"
#include "../include/obd_class.h"
<<<<<<< HEAD
#include "../include/lustre_lib.h"
#include "../include/lustre/lustre_idl.h"
#include "../include/lustre_lite.h"
#include "../include/lustre_dlm.h"
#include "../include/lustre_fid.h"
=======
#include "../include/lustre/lustre_ioctl.h"
#include "../include/lustre_lib.h"
#include "../include/lustre_dlm.h"
#include "../include/lustre_fid.h"
#include "../include/lustre_kernelcomm.h"
>>>>>>> v4.9.227
#include "llite_internal.h"

/*
 * (new) readdir implementation overview.
 *
 * Original lustre readdir implementation cached exact copy of raw directory
 * pages on the client. These pages were indexed in client page cache by
 * logical offset in the directory file. This design, while very simple and
 * intuitive had some inherent problems:
 *
 *     . it implies that byte offset to the directory entry serves as a
 *     telldir(3)/seekdir(3) cookie, but that offset is not stable: in
 *     ext3/htree directory entries may move due to splits, and more
 *     importantly,
 *
 *     . it is incompatible with the design of split directories for cmd3,
 *     that assumes that names are distributed across nodes based on their
 *     hash, and so readdir should be done in hash order.
 *
 * New readdir implementation does readdir in hash order, and uses hash of a
 * file name as a telldir/seekdir cookie. This led to number of complications:
 *
 *     . hash is not unique, so it cannot be used to index cached directory
 *     pages on the client (note, that it requires a whole pageful of hash
 *     collided entries to cause two pages to have identical hashes);
 *
 *     . hash is not unique, so it cannot, strictly speaking, be used as an
 *     entry cookie. ext3/htree has the same problem and lustre implementation
 *     mimics their solution: seekdir(hash) positions directory at the first
 *     entry with the given hash.
 *
 * Client side.
 *
 * 0. caching
 *
 * Client caches directory pages using hash of the first entry as an index. As
 * noted above hash is not unique, so this solution doesn't work as is:
 * special processing is needed for "page hash chains" (i.e., sequences of
 * pages filled with entries all having the same hash value).
 *
 * First, such chains have to be detected. To this end, server returns to the
 * client the hash of the first entry on the page next to one returned. When
 * client detects that this hash is the same as hash of the first entry on the
 * returned page, page hash collision has to be handled. Pages in the
 * hash chain, except first one, are termed "overflow pages".
 *
 * Solution to index uniqueness problem is to not cache overflow
 * pages. Instead, when page hash collision is detected, all overflow pages
 * from emerging chain are immediately requested from the server and placed in
 * a special data structure (struct ll_dir_chain). This data structure is used
 * by ll_readdir() to process entries from overflow pages. When readdir
 * invocation finishes, overflow pages are discarded. If page hash collision
 * chain weren't completely processed, next call to readdir will again detect
 * page hash collision, again read overflow pages in, process next portion of
 * entries and again discard the pages. This is not as wasteful as it looks,
 * because, given reasonable hash, page hash collisions are extremely rare.
 *
 * 1. directory positioning
 *
 * When seekdir(hash) is called, original
 *
 *
 *
 *
 *
 *
 *
 *
 * Server.
 *
 * identification of and access to overflow pages
 *
 * page format
 *
 * Page in MDS_READPAGE RPC is packed in LU_PAGE_SIZE, and each page contains
 * a header lu_dirpage which describes the start/end hash, and whether this
 * page is empty (contains no dir entry) or hash collide with next page.
 * After client receives reply, several pages will be integrated into dir page
<<<<<<< HEAD
 * in PAGE_CACHE_SIZE (if PAGE_CACHE_SIZE greater than LU_PAGE_SIZE), and the
 * lu_dirpage for this integrated page will be adjusted. See
 * lmv_adjust_dirpages().
 *
 */

/* returns the page unlocked, but with a reference */
static int ll_dir_filler(void *_hash, struct page *page0)
{
	struct inode *inode = page0->mapping->host;
	int hash64 = ll_i2sbi(inode)->ll_flags & LL_SBI_64BIT_HASH;
	struct obd_export *exp = ll_i2sbi(inode)->ll_md_exp;
	struct ptlrpc_request *request;
	struct mdt_body *body;
	struct md_op_data *op_data;
	__u64 hash = *((__u64 *)_hash);
	struct page **page_pool;
	struct page *page;
	struct lu_dirpage *dp;
	int max_pages = ll_i2sbi(inode)->ll_md_brw_size >> PAGE_CACHE_SHIFT;
	int nrdpgs = 0; /* number of pages read actually */
	int npages;
	int i;
	int rc;

	CDEBUG(D_VFSTRACE, "VFS Op:inode=%lu/%u(%p) hash %llu\n",
	       inode->i_ino, inode->i_generation, inode, hash);

	LASSERT(max_pages > 0 && max_pages <= MD_MAX_BRW_PAGES);

	page_pool = kzalloc(sizeof(page) * max_pages, GFP_NOFS);
	if (page_pool) {
		page_pool[0] = page0;
	} else {
		page_pool = &page0;
		max_pages = 1;
	}
	for (npages = 1; npages < max_pages; npages++) {
		page = page_cache_alloc_cold(inode->i_mapping);
		if (!page)
			break;
		page_pool[npages] = page;
	}

	op_data = ll_prep_md_op_data(NULL, inode, NULL, NULL, 0, 0,
				     LUSTRE_OPC_ANY, NULL);
	op_data->op_npages = npages;
	op_data->op_offset = hash;
	rc = md_readpage(exp, op_data, page_pool, &request);
	ll_finish_md_op_data(op_data);
	if (rc == 0) {
		body = req_capsule_server_get(&request->rq_pill, &RMF_MDT_BODY);
		/* Checked by mdc_readpage() */
		LASSERT(body != NULL);

		if (body->valid & OBD_MD_FLSIZE)
			cl_isize_write(inode, body->size);

		nrdpgs = (request->rq_bulk->bd_nob_transferred+PAGE_CACHE_SIZE-1)
			 >> PAGE_CACHE_SHIFT;
		SetPageUptodate(page0);
	}
	unlock_page(page0);
	ptlrpc_req_finished(request);

	CDEBUG(D_VFSTRACE, "read %d/%d pages\n", nrdpgs, npages);

	ll_pagevec_init(&lru_pvec, 0);
	for (i = 1; i < npages; i++) {
		unsigned long offset;
		int ret;

		page = page_pool[i];

		if (rc < 0 || i >= nrdpgs) {
			page_cache_release(page);
			continue;
		}

		SetPageUptodate(page);

		dp = kmap(page);
		hash = le64_to_cpu(dp->ldp_hash_start);
		kunmap(page);

		offset = hash_x_index(hash, hash64);

		prefetchw(&page->flags);
		ret = add_to_page_cache_lru(page, inode->i_mapping, offset,
					    GFP_KERNEL);
		if (ret == 0) {
			unlock_page(page);
			if (ll_pagevec_add(&lru_pvec, page) == 0)
				ll_pagevec_lru_add_file(&lru_pvec);
		} else {
			CDEBUG(D_VFSTRACE, "page %lu add to page cache failed:"
			       " %d\n", offset, ret);
		}
		page_cache_release(page);
	}
	ll_pagevec_lru_add_file(&lru_pvec);

	if (page_pool != &page0)
		OBD_FREE(page_pool, sizeof(struct page *) * max_pages);
	return rc;
}

static void ll_check_page(struct inode *dir, struct page *page)
{
	/* XXX: check page format later */
	SetPageChecked(page);
}

void ll_release_page(struct page *page, int remove)
{
	kunmap(page);
	if (remove) {
		lock_page(page);
		if (likely(page->mapping != NULL))
			truncate_complete_page(page->mapping, page);
		unlock_page(page);
	}
	page_cache_release(page);
}

/*
 * Find, kmap and return page that contains given hash.
 */
static struct page *ll_dir_page_locate(struct inode *dir, __u64 *hash,
				       __u64 *start, __u64 *end)
{
	int hash64 = ll_i2sbi(dir)->ll_flags & LL_SBI_64BIT_HASH;
	struct address_space *mapping = dir->i_mapping;
	/*
	 * Complement of hash is used as an index so that
	 * radix_tree_gang_lookup() can be used to find a page with starting
	 * hash _smaller_ than one we are looking for.
	 */
	unsigned long offset = hash_x_index(*hash, hash64);
	struct page *page;
	int found;

	TREE_READ_LOCK_IRQ(mapping);
	found = radix_tree_gang_lookup(&mapping->page_tree,
				       (void **)&page, offset, 1);
	if (found > 0) {
		struct lu_dirpage *dp;

		page_cache_get(page);
		TREE_READ_UNLOCK_IRQ(mapping);
		/*
		 * In contrast to find_lock_page() we are sure that directory
		 * page cannot be truncated (while DLM lock is held) and,
		 * hence, can avoid restart.
		 *
		 * In fact, page cannot be locked here at all, because
		 * ll_dir_filler() does synchronous io.
		 */
		wait_on_page_locked(page);
		if (PageUptodate(page)) {
			dp = kmap(page);
			if (BITS_PER_LONG == 32 && hash64) {
				*start = le64_to_cpu(dp->ldp_hash_start) >> 32;
				*end   = le64_to_cpu(dp->ldp_hash_end) >> 32;
				*hash  = *hash >> 32;
			} else {
				*start = le64_to_cpu(dp->ldp_hash_start);
				*end   = le64_to_cpu(dp->ldp_hash_end);
			}
			LASSERTF(*start <= *hash, "start = %#llx,end = %#llx,hash = %#llx\n",
				 *start, *end, *hash);
			CDEBUG(D_VFSTRACE, "page %lu [%llu %llu], hash %llu\n",
			       offset, *start, *end, *hash);
			if (*hash > *end) {
				ll_release_page(page, 0);
				page = NULL;
			} else if (*end != *start && *hash == *end) {
				/*
				 * upon hash collision, remove this page,
				 * otherwise put page reference, and
				 * ll_get_dir_page() will issue RPC to fetch
				 * the page we want.
				 */
				ll_release_page(page,
				    le32_to_cpu(dp->ldp_flags) & LDF_COLLIDE);
				page = NULL;
			}
		} else {
			page_cache_release(page);
			page = ERR_PTR(-EIO);
		}

	} else {
		TREE_READ_UNLOCK_IRQ(mapping);
		page = NULL;
	}
	return page;
}

struct page *ll_get_dir_page(struct inode *dir, __u64 hash,
			     struct ll_dir_chain *chain)
{
	ldlm_policy_data_t policy = {.l_inodebits = {MDS_INODELOCK_UPDATE} };
	struct address_space *mapping = dir->i_mapping;
	struct lustre_handle lockh;
	struct lu_dirpage *dp;
	struct page *page;
	ldlm_mode_t mode;
	int rc;
	__u64 start = 0;
	__u64 end = 0;
	__u64 lhash = hash;
	struct ll_inode_info *lli = ll_i2info(dir);
	int hash64 = ll_i2sbi(dir)->ll_flags & LL_SBI_64BIT_HASH;

	mode = LCK_PR;
	rc = md_lock_match(ll_i2sbi(dir)->ll_md_exp, LDLM_FL_BLOCK_GRANTED,
			   ll_inode2fid(dir), LDLM_IBITS, &policy, mode, &lockh);
	if (!rc) {
		struct ldlm_enqueue_info einfo = {
			.ei_type = LDLM_IBITS,
			.ei_mode = mode,
			.ei_cb_bl = ll_md_blocking_ast,
			.ei_cb_cp = ldlm_completion_ast,
		};
		struct lookup_intent it = { .it_op = IT_READDIR };
		struct ptlrpc_request *request;
		struct md_op_data *op_data;

		op_data = ll_prep_md_op_data(NULL, dir, dir, NULL, 0, 0,
		LUSTRE_OPC_ANY, NULL);
		if (IS_ERR(op_data))
			return (void *)op_data;

		rc = md_enqueue(ll_i2sbi(dir)->ll_md_exp, &einfo, &it,
				op_data, &lockh, NULL, 0, NULL, 0);

		ll_finish_md_op_data(op_data);

		request = (struct ptlrpc_request *)it.d.lustre.it_data;
		if (request)
			ptlrpc_req_finished(request);
		if (rc < 0) {
			CERROR("lock enqueue: "DFID" at %llu: rc %d\n",
				PFID(ll_inode2fid(dir)), hash, rc);
			return ERR_PTR(rc);
		}

		CDEBUG(D_INODE, "setting lr_lvb_inode to inode %p (%lu/%u)\n",
		       dir, dir->i_ino, dir->i_generation);
		md_set_lock_data(ll_i2sbi(dir)->ll_md_exp,
				 &it.d.lustre.it_lock_handle, dir, NULL);
	} else {
		/* for cross-ref object, l_ast_data of the lock may not be set,
		 * we reset it here */
		md_set_lock_data(ll_i2sbi(dir)->ll_md_exp, &lockh.cookie,
				 dir, NULL);
	}
	ldlm_lock_dump_handle(D_OTHER, &lockh);

	mutex_lock(&lli->lli_readdir_mutex);
	page = ll_dir_page_locate(dir, &lhash, &start, &end);
	if (IS_ERR(page)) {
		CERROR("dir page locate: "DFID" at %llu: rc %ld\n",
		       PFID(ll_inode2fid(dir)), lhash, PTR_ERR(page));
		goto out_unlock;
	} else if (page != NULL) {
		/*
		 * XXX nikita: not entirely correct handling of a corner case:
		 * suppose hash chain of entries with hash value HASH crosses
		 * border between pages P0 and P1. First both P0 and P1 are
		 * cached, seekdir() is called for some entry from the P0 part
		 * of the chain. Later P0 goes out of cache. telldir(HASH)
		 * happens and finds P1, as it starts with matching hash
		 * value. Remaining entries from P0 part of the chain are
		 * skipped. (Is that really a bug?)
		 *
		 * Possible solutions: 0. don't cache P1 is such case, handle
		 * it as an "overflow" page. 1. invalidate all pages at
		 * once. 2. use HASH|1 as an index for P1.
		 */
		goto hash_collision;
	}

	page = read_cache_page(mapping, hash_x_index(hash, hash64),
			       ll_dir_filler, &lhash);
	if (IS_ERR(page)) {
		CERROR("read cache page: "DFID" at %llu: rc %ld\n",
		       PFID(ll_inode2fid(dir)), hash, PTR_ERR(page));
		goto out_unlock;
	}

	wait_on_page_locked(page);
	(void)kmap(page);
	if (!PageUptodate(page)) {
		CERROR("page not updated: "DFID" at %llu: rc %d\n",
		       PFID(ll_inode2fid(dir)), hash, -5);
		goto fail;
	}
	if (!PageChecked(page))
		ll_check_page(dir, page);
	if (PageError(page)) {
		CERROR("page error: "DFID" at %llu: rc %d\n",
		       PFID(ll_inode2fid(dir)), hash, -5);
		goto fail;
	}
hash_collision:
	dp = page_address(page);
	if (BITS_PER_LONG == 32 && hash64) {
		start = le64_to_cpu(dp->ldp_hash_start) >> 32;
		end   = le64_to_cpu(dp->ldp_hash_end) >> 32;
		lhash = hash >> 32;
	} else {
		start = le64_to_cpu(dp->ldp_hash_start);
		end   = le64_to_cpu(dp->ldp_hash_end);
		lhash = hash;
	}
	if (end == start) {
		LASSERT(start == lhash);
		CWARN("Page-wide hash collision: %llu\n", end);
		if (BITS_PER_LONG == 32 && hash64)
			CWARN("Real page-wide hash collision at [%llu %llu] with hash %llu\n",
			      le64_to_cpu(dp->ldp_hash_start),
			      le64_to_cpu(dp->ldp_hash_end), hash);
		/*
		 * Fetch whole overflow chain...
		 *
		 * XXX not yet.
		 */
		goto fail;
	}
out_unlock:
	mutex_unlock(&lli->lli_readdir_mutex);
	ldlm_lock_decref(&lockh, mode);
	return page;

fail:
	ll_release_page(page, 1);
	page = ERR_PTR(-EIO);
	goto out_unlock;
}

int ll_dir_read(struct inode *inode, struct dir_context *ctx)
{
	struct ll_inode_info *info       = ll_i2info(inode);
	struct ll_sb_info    *sbi	= ll_i2sbi(inode);
	__u64		   pos		= ctx->pos;
	int		   api32      = ll_need_32bit_api(sbi);
	int		   hash64     = sbi->ll_flags & LL_SBI_64BIT_HASH;
	struct page	  *page;
	struct ll_dir_chain   chain;
	int		   done = 0;
	int		   rc = 0;

	ll_dir_chain_init(&chain);

	page = ll_get_dir_page(inode, pos, &chain);
=======
 * in PAGE_SIZE (if PAGE_SIZE greater than LU_PAGE_SIZE), and the lu_dirpage
 * for this integrated page will be adjusted. See lmv_adjust_dirpages().
 *
 */
struct page *ll_get_dir_page(struct inode *dir, struct md_op_data *op_data,
			     __u64 offset)
{
	struct md_callback cb_op;
	struct page *page;
	int rc;

	cb_op.md_blocking_ast = ll_md_blocking_ast;
	rc = md_read_page(ll_i2mdexp(dir), op_data, &cb_op, offset, &page);
	if (rc)
		return ERR_PTR(rc);

	return page;
}

void ll_release_page(struct inode *inode, struct page *page, bool remove)
{
	kunmap(page);

	/*
	 * Always remove the page for striped dir, because the page is
	 * built from temporarily in LMV layer
	 */
	if (inode && S_ISDIR(inode->i_mode) &&
	    ll_i2info(inode)->lli_lsm_md) {
		__free_page(page);
		return;
	}

	if (remove) {
		lock_page(page);
		if (likely(page->mapping))
			truncate_complete_page(page->mapping, page);
		unlock_page(page);
	}
	put_page(page);
}

/**
 * return IF_* type for given lu_dirent entry.
 * IF_* flag shld be converted to particular OS file type in
 * platform llite module.
 */
static __u16 ll_dirent_type_get(struct lu_dirent *ent)
{
	__u16 type = 0;
	struct luda_type *lt;
	int len = 0;

	if (le32_to_cpu(ent->lde_attrs) & LUDA_TYPE) {
		const unsigned int align = sizeof(struct luda_type) - 1;

		len = le16_to_cpu(ent->lde_namelen);
		len = (len + align) & ~align;
		lt = (void *)ent->lde_name + len;
		type = IFTODT(le16_to_cpu(lt->lt_type));
	}
	return type;
}

int ll_dir_read(struct inode *inode, __u64 *ppos, struct md_op_data *op_data,
		struct dir_context *ctx)
{
	struct ll_sb_info    *sbi	= ll_i2sbi(inode);
	__u64		   pos		= *ppos;
	int		   is_api32 = ll_need_32bit_api(sbi);
	int		   is_hash64 = sbi->ll_flags & LL_SBI_64BIT_HASH;
	struct page	  *page;
	bool		   done = false;
	int		   rc = 0;

	page = ll_get_dir_page(inode, op_data, pos);
>>>>>>> v4.9.227

	while (rc == 0 && !done) {
		struct lu_dirpage *dp;
		struct lu_dirent  *ent;
<<<<<<< HEAD

		if (!IS_ERR(page)) {
			/*
			 * If page is empty (end of directory is reached),
			 * use this value.
			 */
			__u64 hash = MDS_DIR_END_OFF;
			__u64 next;

			dp = page_address(page);
			for (ent = lu_dirent_start(dp); ent != NULL && !done;
			     ent = lu_dirent_next(ent)) {
				__u16	  type;
				int	    namelen;
				struct lu_fid  fid;
				__u64	  lhash;
				__u64	  ino;

				/*
				 * XXX: implement correct swabbing here.
				 */

				hash = le64_to_cpu(ent->lde_hash);
				if (hash < pos)
					/*
					 * Skip until we find target hash
					 * value.
					 */
					continue;

				namelen = le16_to_cpu(ent->lde_namelen);
				if (namelen == 0)
					/*
					 * Skip dummy record.
					 */
					continue;

				if (api32 && hash64)
					lhash = hash >> 32;
				else
					lhash = hash;
				fid_le_to_cpu(&fid, &ent->lde_fid);
				ino = cl_fid_build_ino(&fid, api32);
				type = ll_dirent_type_get(ent);
				ctx->pos = lhash;
				/* For 'll_nfs_get_name_filldir()', it will try
				 * to access the 'ent' through its 'lde_name',
				 * so the parameter 'name' for 'ctx->actor()'
				 * must be part of the 'ent'.
				 */
				done = !dir_emit(ctx, ent->lde_name,
						 namelen, ino, type);
			}
			next = le64_to_cpu(dp->ldp_hash_end);
			if (!done) {
				pos = next;
				if (pos == MDS_DIR_END_OFF) {
					/*
					 * End of directory reached.
					 */
					done = 1;
					ll_release_page(page, 0);
				} else if (1 /* chain is exhausted*/) {
					/*
					 * Normal case: continue to the next
					 * page.
					 */
					ll_release_page(page,
					    le32_to_cpu(dp->ldp_flags) &
							LDF_COLLIDE);
					next = pos;
					page = ll_get_dir_page(inode, pos,
							       &chain);
				} else {
					/*
					 * go into overflow page.
					 */
					LASSERT(le32_to_cpu(dp->ldp_flags) &
						LDF_COLLIDE);
					ll_release_page(page, 1);
				}
			} else {
				pos = hash;
				ll_release_page(page, 0);
			}
		} else {
			rc = PTR_ERR(page);
			CERROR("error reading dir "DFID" at %lu: rc %d\n",
			       PFID(&info->lli_fid), (unsigned long)pos, rc);
=======
		__u64 hash;
		__u64 next;

		if (IS_ERR(page)) {
			rc = PTR_ERR(page);
			break;
		}

		hash = MDS_DIR_END_OFF;
		dp = page_address(page);
		for (ent = lu_dirent_start(dp); ent && !done;
		     ent = lu_dirent_next(ent)) {
			__u16	  type;
			int	    namelen;
			struct lu_fid  fid;
			__u64	  lhash;
			__u64	  ino;

			hash = le64_to_cpu(ent->lde_hash);
			if (hash < pos)
				/*
				 * Skip until we find target hash
				 * value.
				 */
				continue;

			namelen = le16_to_cpu(ent->lde_namelen);
			if (namelen == 0)
				/*
				 * Skip dummy record.
				 */
				continue;

			if (is_api32 && is_hash64)
				lhash = hash >> 32;
			else
				lhash = hash;
			fid_le_to_cpu(&fid, &ent->lde_fid);
			ino = cl_fid_build_ino(&fid, is_api32);
			type = ll_dirent_type_get(ent);
			ctx->pos = lhash;
			/* For 'll_nfs_get_name_filldir()', it will try
			 * to access the 'ent' through its 'lde_name',
			 * so the parameter 'name' for 'ctx->actor()'
			 * must be part of the 'ent'.
			 */
			done = !dir_emit(ctx, ent->lde_name,
					 namelen, ino, type);
		}

		if (done) {
			pos = hash;
			ll_release_page(inode, page, false);
			break;
		}

		next = le64_to_cpu(dp->ldp_hash_end);
		pos = next;
		if (pos == MDS_DIR_END_OFF) {
			/*
			 * End of directory reached.
			 */
			done = 1;
			ll_release_page(inode, page, false);
		} else {
			/*
			 * Normal case: continue to the next
			 * page.
			 */
			ll_release_page(inode, page,
					le32_to_cpu(dp->ldp_flags) &
					LDF_COLLIDE);
			next = pos;
			page = ll_get_dir_page(inode, op_data, pos);
>>>>>>> v4.9.227
		}
	}

	ctx->pos = pos;
<<<<<<< HEAD
	ll_dir_chain_fini(&chain);
=======
>>>>>>> v4.9.227
	return rc;
}

static int ll_readdir(struct file *filp, struct dir_context *ctx)
{
<<<<<<< HEAD
	struct inode		*inode	= filp->f_dentry->d_inode;
	struct ll_file_data	*lfd	= LUSTRE_FPRIVATE(filp);
	struct ll_sb_info	*sbi	= ll_i2sbi(inode);
	int			hash64	= sbi->ll_flags & LL_SBI_64BIT_HASH;
	int			api32	= ll_need_32bit_api(sbi);
	int			rc;

	CDEBUG(D_VFSTRACE, "VFS Op:inode=%lu/%u(%p) pos %lu/%llu "
	       " 32bit_api %d\n", inode->i_ino, inode->i_generation,
	       inode, (unsigned long)lfd->lfd_pos, i_size_read(inode), api32);

	if (lfd->lfd_pos == MDS_DIR_END_OFF) {
=======
	struct inode		*inode	= file_inode(filp);
	struct ll_file_data	*lfd	= LUSTRE_FPRIVATE(filp);
	struct ll_sb_info	*sbi	= ll_i2sbi(inode);
	__u64 pos = lfd ? lfd->lfd_pos : 0;
	int			hash64	= sbi->ll_flags & LL_SBI_64BIT_HASH;
	int			api32	= ll_need_32bit_api(sbi);
	struct md_op_data *op_data;
	int			rc;

	CDEBUG(D_VFSTRACE, "VFS Op:inode="DFID"(%p) pos/size %lu/%llu 32bit_api %d\n",
	       PFID(ll_inode2fid(inode)), inode, (unsigned long)pos,
	       i_size_read(inode), api32);

	if (pos == MDS_DIR_END_OFF) {
>>>>>>> v4.9.227
		/*
		 * end-of-file.
		 */
		rc = 0;
		goto out;
	}

<<<<<<< HEAD
	ctx->pos = lfd->lfd_pos;
	rc = ll_dir_read(inode, ctx);
	lfd->lfd_pos = ctx->pos;
	if (ctx->pos == MDS_DIR_END_OFF) {
		if (api32)
			ctx->pos = LL_DIR_END_OFF_32BIT;
		else
			ctx->pos = LL_DIR_END_OFF;
	} else {
		if (api32 && hash64)
			ctx->pos >>= 32;
	}
=======
	op_data = ll_prep_md_op_data(NULL, inode, inode, NULL, 0, 0,
				     LUSTRE_OPC_ANY, inode);
	if (IS_ERR(op_data)) {
		rc = PTR_ERR(op_data);
		goto out;
	}

	if (unlikely(op_data->op_mea1)) {
		/*
		 * This is only needed for striped dir to fill ..,
		 * see lmv_read_page
		 */
		if (file_dentry(filp)->d_parent &&
		    file_dentry(filp)->d_parent->d_inode) {
			__u64 ibits = MDS_INODELOCK_UPDATE;
			struct inode *parent;

			parent = file_dentry(filp)->d_parent->d_inode;
			if (ll_have_md_lock(parent, &ibits, LCK_MINMODE))
				op_data->op_fid3 = *ll_inode2fid(parent);
		}

		/*
		 * If it can not find in cache, do lookup .. on the master
		 * object
		 */
		if (fid_is_zero(&op_data->op_fid3)) {
			rc = ll_dir_get_parent_fid(inode, &op_data->op_fid3);
			if (rc) {
				ll_finish_md_op_data(op_data);
				return rc;
			}
		}
	}
	op_data->op_max_pages = sbi->ll_md_brw_pages;
	ctx->pos = pos;
	rc = ll_dir_read(inode, &pos, op_data, ctx);
	pos = ctx->pos;
	if (lfd)
		lfd->lfd_pos = pos;

	if (pos == MDS_DIR_END_OFF) {
		if (api32)
			pos = LL_DIR_END_OFF_32BIT;
		else
			pos = LL_DIR_END_OFF;
	} else {
		if (api32 && hash64)
			pos >>= 32;
	}
	ctx->pos = pos;
	ll_finish_md_op_data(op_data);
>>>>>>> v4.9.227
	filp->f_version = inode->i_version;

out:
	if (!rc)
		ll_stats_ops_tally(sbi, LPROC_LL_READDIR, 1);

	return rc;
}

static int ll_send_mgc_param(struct obd_export *mgc, char *string)
{
	struct mgs_send_param *msp;
	int rc = 0;

	msp = kzalloc(sizeof(*msp), GFP_NOFS);
	if (!msp)
		return -ENOMEM;

<<<<<<< HEAD
	strncpy(msp->mgs_param, string, MGS_PARAM_MAXLEN);
=======
	strlcpy(msp->mgs_param, string, sizeof(msp->mgs_param));
>>>>>>> v4.9.227
	rc = obd_set_info_async(NULL, mgc, sizeof(KEY_SET_INFO), KEY_SET_INFO,
				sizeof(struct mgs_send_param), msp, NULL);
	if (rc)
		CERROR("Failed to set parameter: %d\n", rc);
<<<<<<< HEAD
	OBD_FREE_PTR(msp);
=======
	kfree(msp);
>>>>>>> v4.9.227

	return rc;
}

<<<<<<< HEAD
int ll_dir_setdirstripe(struct inode *dir, struct lmv_user_md *lump,
			char *filename)
{
	struct ptlrpc_request *request = NULL;
	struct md_op_data *op_data;
	struct ll_sb_info *sbi = ll_i2sbi(dir);
	int mode;
	int err;

	mode = (0755 & (S_IRWXUGO|S_ISVTX) & ~current->fs->umask) | S_IFDIR;
	op_data = ll_prep_md_op_data(NULL, dir, NULL, filename,
				     strlen(filename), mode, LUSTRE_OPC_MKDIR,
=======
/**
 * Create striped directory with specified stripe(@lump)
 *
 * param[in] parent	the parent of the directory.
 * param[in] lump	the specified stripes.
 * param[in] dirname	the name of the directory.
 * param[in] mode	the specified mode of the directory.
 *
 * retval		=0 if striped directory is being created successfully.
 *			<0 if the creation is failed.
 */
static int ll_dir_setdirstripe(struct inode *parent, struct lmv_user_md *lump,
			       const char *dirname, umode_t mode)
{
	struct ptlrpc_request *request = NULL;
	struct md_op_data *op_data;
	struct ll_sb_info *sbi = ll_i2sbi(parent);
	int err;

	if (unlikely(lump->lum_magic != LMV_USER_MAGIC))
		return -EINVAL;

	CDEBUG(D_VFSTRACE, "VFS Op:inode="DFID"(%p) name %s stripe_offset %d, stripe_count: %u\n",
	       PFID(ll_inode2fid(parent)), parent, dirname,
	       (int)lump->lum_stripe_offset, lump->lum_stripe_count);

	if (lump->lum_magic != cpu_to_le32(LMV_USER_MAGIC))
		lustre_swab_lmv_user_md(lump);

	if (!IS_POSIXACL(parent) || !exp_connect_umask(ll_i2mdexp(parent)))
		mode &= ~current_umask();
	mode = (mode & (S_IRWXUGO | S_ISVTX)) | S_IFDIR;
	op_data = ll_prep_md_op_data(NULL, parent, NULL, dirname,
				     strlen(dirname), mode, LUSTRE_OPC_MKDIR,
>>>>>>> v4.9.227
				     lump);
	if (IS_ERR(op_data)) {
		err = PTR_ERR(op_data);
		goto err_exit;
	}

	op_data->op_cli_flags |= CLI_SET_MEA;
	err = md_create(sbi->ll_md_exp, op_data, lump, sizeof(*lump), mode,
			from_kuid(&init_user_ns, current_fsuid()),
			from_kgid(&init_user_ns, current_fsgid()),
			cfs_curproc_cap_pack(), 0, &request);
	ll_finish_md_op_data(op_data);
	if (err)
		goto err_exit;
err_exit:
	ptlrpc_req_finished(request);
	return err;
}

int ll_dir_setstripe(struct inode *inode, struct lov_user_md *lump,
		     int set_default)
{
	struct ll_sb_info *sbi = ll_i2sbi(inode);
	struct md_op_data *op_data;
	struct ptlrpc_request *req = NULL;
	int rc = 0;
	struct lustre_sb_info *lsi = s2lsi(inode->i_sb);
	struct obd_device *mgc = lsi->lsi_mgc;
	int lum_size;

<<<<<<< HEAD
	if (lump != NULL) {
=======
	if (lump) {
>>>>>>> v4.9.227
		/*
		 * This is coming from userspace, so should be in
		 * local endian.  But the MDS would like it in little
		 * endian, so we swab it before we send it.
		 */
		switch (lump->lmm_magic) {
		case LOV_USER_MAGIC_V1: {
			if (lump->lmm_magic != cpu_to_le32(LOV_USER_MAGIC_V1))
				lustre_swab_lov_user_md_v1(lump);
			lum_size = sizeof(struct lov_user_md_v1);
			break;
		}
		case LOV_USER_MAGIC_V3: {
			if (lump->lmm_magic != cpu_to_le32(LOV_USER_MAGIC_V3))
				lustre_swab_lov_user_md_v3(
					(struct lov_user_md_v3 *)lump);
			lum_size = sizeof(struct lov_user_md_v3);
			break;
		}
<<<<<<< HEAD
		default: {
			CDEBUG(D_IOCTL, "bad userland LOV MAGIC:"
					" %#08x != %#08x nor %#08x\n",
					lump->lmm_magic, LOV_USER_MAGIC_V1,
					LOV_USER_MAGIC_V3);
=======
		case LMV_USER_MAGIC: {
			if (lump->lmm_magic != cpu_to_le32(LMV_USER_MAGIC))
				lustre_swab_lmv_user_md(
					(struct lmv_user_md *)lump);
			lum_size = sizeof(struct lmv_user_md);
			break;
		}
		default: {
			CDEBUG(D_IOCTL, "bad userland LOV MAGIC: %#08x != %#08x nor %#08x\n",
			       lump->lmm_magic, LOV_USER_MAGIC_V1,
			       LOV_USER_MAGIC_V3);
>>>>>>> v4.9.227
			return -EINVAL;
		}
		}
	} else {
		lum_size = sizeof(struct lov_user_md_v1);
	}

	op_data = ll_prep_md_op_data(NULL, inode, NULL, NULL, 0, 0,
				     LUSTRE_OPC_ANY, NULL);
	if (IS_ERR(op_data))
		return PTR_ERR(op_data);

<<<<<<< HEAD
	if (lump != NULL && lump->lmm_magic == cpu_to_le32(LMV_USER_MAGIC))
		op_data->op_cli_flags |= CLI_SET_MEA;

=======
>>>>>>> v4.9.227
	/* swabbing is done in lov_setstripe() on server side */
	rc = md_setattr(sbi->ll_md_exp, op_data, lump, lum_size,
			NULL, 0, &req, NULL);
	ll_finish_md_op_data(op_data);
	ptlrpc_req_finished(req);
	if (rc) {
		if (rc != -EPERM && rc != -EACCES)
			CERROR("mdc_setattr fails: rc = %d\n", rc);
	}

	/* In the following we use the fact that LOV_USER_MAGIC_V1 and
<<<<<<< HEAD
	 LOV_USER_MAGIC_V3 have the same initial fields so we do not
	 need to make the distinction between the 2 versions */
=======
	 * LOV_USER_MAGIC_V3 have the same initial fields so we do not
	 * need to make the distinction between the 2 versions
	 */
>>>>>>> v4.9.227
	if (set_default && mgc->u.cli.cl_mgc_mgsexp) {
		char *param = NULL;
		char *buf;

		param = kzalloc(MGS_PARAM_MAXLEN, GFP_NOFS);
<<<<<<< HEAD
		if (!param) {
			rc = -ENOMEM;
			goto end;
		}
=======
		if (!param)
			return -ENOMEM;
>>>>>>> v4.9.227

		buf = param;
		/* Get fsname and assume devname to be -MDT0000. */
		ll_get_fsname(inode->i_sb, buf, MTI_NAME_MAXLEN);
		strcat(buf, "-MDT0000.lov");
		buf += strlen(buf);

		/* Set root stripesize */
		sprintf(buf, ".stripesize=%u",
			lump ? le32_to_cpu(lump->lmm_stripe_size) : 0);
		rc = ll_send_mgc_param(mgc->u.cli.cl_mgc_mgsexp, param);
		if (rc)
			goto end;

		/* Set root stripecount */
		sprintf(buf, ".stripecount=%hd",
			lump ? le16_to_cpu(lump->lmm_stripe_count) : 0);
		rc = ll_send_mgc_param(mgc->u.cli.cl_mgc_mgsexp, param);
		if (rc)
			goto end;

		/* Set root stripeoffset */
		sprintf(buf, ".stripeoffset=%hd",
			lump ? le16_to_cpu(lump->lmm_stripe_offset) :
			(typeof(lump->lmm_stripe_offset))(-1));
		rc = ll_send_mgc_param(mgc->u.cli.cl_mgc_mgsexp, param);

end:
<<<<<<< HEAD
		if (param != NULL)
			OBD_FREE(param, MGS_PARAM_MAXLEN);
=======
		kfree(param);
>>>>>>> v4.9.227
	}
	return rc;
}

<<<<<<< HEAD
int ll_dir_getstripe(struct inode *inode, struct lov_mds_md **lmmp,
		     int *lmm_size, struct ptlrpc_request **request)
=======
/**
 * This function will be used to get default LOV/LMV/Default LMV
 * @valid will be used to indicate which stripe it will retrieve
 *	OBD_MD_MEA		LMV stripe EA
 *	OBD_MD_DEFAULT_MEA	Default LMV stripe EA
 *	otherwise		Default LOV EA.
 * Each time, it can only retrieve 1 stripe EA
 **/
int ll_dir_getstripe(struct inode *inode, void **plmm, int *plmm_size,
		     struct ptlrpc_request **request, u64 valid)
>>>>>>> v4.9.227
{
	struct ll_sb_info *sbi = ll_i2sbi(inode);
	struct mdt_body   *body;
	struct lov_mds_md *lmm = NULL;
	struct ptlrpc_request *req = NULL;
	int rc, lmmsize;
	struct md_op_data *op_data;

<<<<<<< HEAD
	rc = ll_get_default_mdsize(sbi, &lmmsize);
=======
	rc = ll_get_max_mdsize(sbi, &lmmsize);
>>>>>>> v4.9.227
	if (rc)
		return rc;

	op_data = ll_prep_md_op_data(NULL, inode, NULL, NULL,
				     0, lmmsize, LUSTRE_OPC_ANY,
				     NULL);
	if (IS_ERR(op_data))
		return PTR_ERR(op_data);

	op_data->op_valid = OBD_MD_FLEASIZE | OBD_MD_FLDIREA;
	rc = md_getattr(sbi->ll_md_exp, op_data, &req);
	ll_finish_md_op_data(op_data);
	if (rc < 0) {
<<<<<<< HEAD
		CDEBUG(D_INFO, "md_getattr failed on inode "
		       "%lu/%u: rc %d\n", inode->i_ino,
		       inode->i_generation, rc);
=======
		CDEBUG(D_INFO, "md_getattr failed on inode "DFID": rc %d\n",
		       PFID(ll_inode2fid(inode)), rc);
>>>>>>> v4.9.227
		goto out;
	}

	body = req_capsule_server_get(&req->rq_pill, &RMF_MDT_BODY);
<<<<<<< HEAD
	LASSERT(body != NULL);

	lmmsize = body->eadatasize;

	if (!(body->valid & (OBD_MD_FLEASIZE | OBD_MD_FLDIREA)) ||
=======

	lmmsize = body->mbo_eadatasize;

	if (!(body->mbo_valid & (OBD_MD_FLEASIZE | OBD_MD_FLDIREA)) ||
>>>>>>> v4.9.227
	    lmmsize == 0) {
		rc = -ENODATA;
		goto out;
	}

	lmm = req_capsule_server_sized_get(&req->rq_pill,
					   &RMF_MDT_MD, lmmsize);
<<<<<<< HEAD
	LASSERT(lmm != NULL);
=======
	LASSERT(lmm);
>>>>>>> v4.9.227

	/*
	 * This is coming from the MDS, so is probably in
	 * little endian.  We convert it to host endian before
	 * passing it to userspace.
	 */
	/* We don't swab objects for directories */
	switch (le32_to_cpu(lmm->lmm_magic)) {
	case LOV_MAGIC_V1:
<<<<<<< HEAD
		if (LOV_MAGIC != cpu_to_le32(LOV_MAGIC))
			lustre_swab_lov_user_md_v1((struct lov_user_md_v1 *)lmm);
		break;
	case LOV_MAGIC_V3:
		if (LOV_MAGIC != cpu_to_le32(LOV_MAGIC))
			lustre_swab_lov_user_md_v3((struct lov_user_md_v3 *)lmm);
		break;
=======
		if (cpu_to_le32(LOV_MAGIC) != LOV_MAGIC)
			lustre_swab_lov_user_md_v1((struct lov_user_md_v1 *)lmm);
		break;
	case LOV_MAGIC_V3:
		if (cpu_to_le32(LOV_MAGIC) != LOV_MAGIC)
			lustre_swab_lov_user_md_v3((struct lov_user_md_v3 *)lmm);
		break;
	case LMV_MAGIC_V1:
		if (cpu_to_le32(LMV_MAGIC) != LMV_MAGIC)
			lustre_swab_lmv_mds_md((union lmv_mds_md *)lmm);
		break;
	case LMV_USER_MAGIC:
		if (cpu_to_le32(LMV_USER_MAGIC) != LMV_USER_MAGIC)
			lustre_swab_lmv_user_md((struct lmv_user_md *)lmm);
		break;
>>>>>>> v4.9.227
	default:
		CERROR("unknown magic: %lX\n", (unsigned long)lmm->lmm_magic);
		rc = -EPROTO;
	}
out:
<<<<<<< HEAD
	*lmmp = lmm;
	*lmm_size = lmmsize;
=======
	*plmm = lmm;
	*plmm_size = lmmsize;
>>>>>>> v4.9.227
	*request = req;
	return rc;
}

<<<<<<< HEAD
=======
int ll_get_mdt_idx_by_fid(struct ll_sb_info *sbi, const struct lu_fid *fid)
{
	struct md_op_data *op_data;
	int mdt_index, rc;

	op_data = kzalloc(sizeof(*op_data), GFP_NOFS);
	if (!op_data)
		return -ENOMEM;

	op_data->op_flags |= MF_GET_MDT_IDX;
	op_data->op_fid1 = *fid;
	rc = md_getattr(sbi->ll_md_exp, op_data, NULL);
	mdt_index = op_data->op_mds;
	kvfree(op_data);
	if (rc < 0)
		return rc;

	return mdt_index;
}

>>>>>>> v4.9.227
/*
 *  Get MDT index for the inode.
 */
int ll_get_mdt_idx(struct inode *inode)
{
<<<<<<< HEAD
	struct ll_sb_info *sbi = ll_i2sbi(inode);
	struct md_op_data *op_data;
	int rc, mdtidx;

	op_data = ll_prep_md_op_data(NULL, inode, NULL, NULL, 0,
				     0, LUSTRE_OPC_ANY, NULL);
	if (IS_ERR(op_data))
		return PTR_ERR(op_data);

	op_data->op_flags |= MF_GET_MDT_IDX;
	rc = md_getattr(sbi->ll_md_exp, op_data, NULL);
	mdtidx = op_data->op_mds;
	ll_finish_md_op_data(op_data);
	if (rc < 0) {
		CDEBUG(D_INFO, "md_getattr_name: %d\n", rc);
		return rc;
	}
	return mdtidx;
=======
	return ll_get_mdt_idx_by_fid(ll_i2sbi(inode), ll_inode2fid(inode));
>>>>>>> v4.9.227
}

/**
 * Generic handler to do any pre-copy work.
 *
<<<<<<< HEAD
 * It send a first hsm_progress (with extent length == 0) to coordinator as a
=======
 * It sends a first hsm_progress (with extent length == 0) to coordinator as a
>>>>>>> v4.9.227
 * first information for it that real work has started.
 *
 * Moreover, for a ARCHIVE request, it will sample the file data version and
 * store it in \a copy.
 *
 * \return 0 on success.
 */
static int ll_ioc_copy_start(struct super_block *sb, struct hsm_copy *copy)
{
	struct ll_sb_info		*sbi = ll_s2sbi(sb);
	struct hsm_progress_kernel	 hpk;
	int				 rc;

	/* Forge a hsm_progress based on data from copy. */
	hpk.hpk_fid = copy->hc_hai.hai_fid;
	hpk.hpk_cookie = copy->hc_hai.hai_cookie;
	hpk.hpk_extent.offset = copy->hc_hai.hai_extent.offset;
	hpk.hpk_extent.length = 0;
	hpk.hpk_flags = 0;
	hpk.hpk_errval = 0;
	hpk.hpk_data_version = 0;

<<<<<<< HEAD

=======
>>>>>>> v4.9.227
	/* For archive request, we need to read the current file version. */
	if (copy->hc_hai.hai_action == HSMA_ARCHIVE) {
		struct inode	*inode;
		__u64		 data_version = 0;

		/* Get inode for this fid */
		inode = search_inode_for_lustre(sb, &copy->hc_hai.hai_fid);
		if (IS_ERR(inode)) {
			hpk.hpk_flags |= HP_FLAG_RETRY;
			/* hpk_errval is >= 0 */
			hpk.hpk_errval = -PTR_ERR(inode);
			rc = PTR_ERR(inode);
			goto progress;
		}

		/* Read current file data version */
<<<<<<< HEAD
		rc = ll_data_version(inode, &data_version, 1);
=======
		rc = ll_data_version(inode, &data_version, LL_DV_RD_FLUSH);
>>>>>>> v4.9.227
		iput(inode);
		if (rc != 0) {
			CDEBUG(D_HSM, "Could not read file data version of "
				      DFID" (rc = %d). Archive request (%#llx) could not be done.\n",
				      PFID(&copy->hc_hai.hai_fid), rc,
				      copy->hc_hai.hai_cookie);
			hpk.hpk_flags |= HP_FLAG_RETRY;
			/* hpk_errval must be >= 0 */
			hpk.hpk_errval = -rc;
			goto progress;
		}

<<<<<<< HEAD
		/* Store it the hsm_copy for later copytool use.
		 * Always modified even if no lsm. */
=======
		/* Store in the hsm_copy for later copytool use.
		 * Always modified even if no lsm.
		 */
>>>>>>> v4.9.227
		copy->hc_data_version = data_version;
	}

progress:
<<<<<<< HEAD
=======
	/* On error, the request should be considered as completed */
	if (hpk.hpk_errval > 0)
		hpk.hpk_flags |= HP_FLAG_COMPLETED;
>>>>>>> v4.9.227
	rc = obd_iocontrol(LL_IOC_HSM_PROGRESS, sbi->ll_md_exp, sizeof(hpk),
			   &hpk, NULL);

	return rc;
}

/**
 * Generic handler to do any post-copy work.
 *
 * It will send the last hsm_progress update to coordinator to inform it
 * that copy is finished and whether it was successful or not.
 *
 * Moreover,
 * - for ARCHIVE request, it will sample the file data version and compare it
 *   with the version saved in ll_ioc_copy_start(). If they do not match, copy
 *   will be considered as failed.
 * - for RESTORE request, it will sample the file data version and send it to
 *   coordinator which is useful if the file was imported as 'released'.
 *
 * \return 0 on success.
 */
static int ll_ioc_copy_end(struct super_block *sb, struct hsm_copy *copy)
{
	struct ll_sb_info		*sbi = ll_s2sbi(sb);
	struct hsm_progress_kernel	 hpk;
	int				 rc;

	/* If you modify the logic here, also check llapi_hsm_copy_end(). */
	/* Take care: copy->hc_hai.hai_action, len, gid and data are not
	 * initialized if copy_end was called with copy == NULL.
	 */

	/* Forge a hsm_progress based on data from copy. */
	hpk.hpk_fid = copy->hc_hai.hai_fid;
	hpk.hpk_cookie = copy->hc_hai.hai_cookie;
	hpk.hpk_extent = copy->hc_hai.hai_extent;
	hpk.hpk_flags = copy->hc_flags | HP_FLAG_COMPLETED;
	hpk.hpk_errval = copy->hc_errval;
	hpk.hpk_data_version = 0;

	/* For archive request, we need to check the file data was not changed.
	 *
	 * For restore request, we need to send the file data version, this is
	 * useful when the file was created using hsm_import.
	 */
	if (((copy->hc_hai.hai_action == HSMA_ARCHIVE) ||
	     (copy->hc_hai.hai_action == HSMA_RESTORE)) &&
	    (copy->hc_errval == 0)) {
		struct inode	*inode;
		__u64		 data_version = 0;

		/* Get lsm for this fid */
		inode = search_inode_for_lustre(sb, &copy->hc_hai.hai_fid);
		if (IS_ERR(inode)) {
			hpk.hpk_flags |= HP_FLAG_RETRY;
			/* hpk_errval must be >= 0 */
			hpk.hpk_errval = -PTR_ERR(inode);
			rc = PTR_ERR(inode);
			goto progress;
		}

<<<<<<< HEAD
		rc = ll_data_version(inode, &data_version,
				     copy->hc_hai.hai_action == HSMA_ARCHIVE);
		iput(inode);
		if (rc) {
			CDEBUG(D_HSM, "Could not read file data version. "
				      "Request could not be confirmed.\n");
=======
		rc = ll_data_version(inode, &data_version, LL_DV_RD_FLUSH);
		iput(inode);
		if (rc) {
			CDEBUG(D_HSM, "Could not read file data version. Request could not be confirmed.\n");
>>>>>>> v4.9.227
			if (hpk.hpk_errval == 0)
				hpk.hpk_errval = -rc;
			goto progress;
		}

<<<<<<< HEAD
		/* Store it the hsm_copy for later copytool use.
		 * Always modified even if no lsm. */
		hpk.hpk_data_version = data_version;

		/* File could have been stripped during archiving, so we need
		 * to check anyway. */
		if ((copy->hc_hai.hai_action == HSMA_ARCHIVE) &&
		    (copy->hc_data_version != data_version)) {
			CDEBUG(D_HSM, "File data version mismatched. "
			      "File content was changed during archiving. "
=======
		/* Store in the hsm_copy for later copytool use.
		 * Always modified even if no lsm.
		 */
		hpk.hpk_data_version = data_version;

		/* File could have been stripped during archiving, so we need
		 * to check anyway.
		 */
		if ((copy->hc_hai.hai_action == HSMA_ARCHIVE) &&
		    (copy->hc_data_version != data_version)) {
			CDEBUG(D_HSM, "File data version mismatched. File content was changed during archiving. "
>>>>>>> v4.9.227
			       DFID", start:%#llx current:%#llx\n",
			       PFID(&copy->hc_hai.hai_fid),
			       copy->hc_data_version, data_version);
			/* File was changed, send error to cdt. Do not ask for
			 * retry because if a file is modified frequently,
			 * the cdt will loop on retried archive requests.
			 * The policy engine will ask for a new archive later
			 * when the file will not be modified for some tunable
<<<<<<< HEAD
			 * time */
=======
			 * time
			 */
>>>>>>> v4.9.227
			/* we do not notify caller */
			hpk.hpk_flags &= ~HP_FLAG_RETRY;
			/* hpk_errval must be >= 0 */
			hpk.hpk_errval = EBUSY;
		}
<<<<<<< HEAD

=======
>>>>>>> v4.9.227
	}

progress:
	rc = obd_iocontrol(LL_IOC_HSM_PROGRESS, sbi->ll_md_exp, sizeof(hpk),
			   &hpk, NULL);

	return rc;
}

<<<<<<< HEAD

=======
>>>>>>> v4.9.227
static int copy_and_ioctl(int cmd, struct obd_export *exp,
			  const void __user *data, size_t size)
{
	void *copy;
	int rc;

<<<<<<< HEAD
	copy = kzalloc(size, GFP_NOFS);
	if (!copy)
		return -ENOMEM;

	if (copy_from_user(copy, data, size)) {
		rc = -EFAULT;
		goto out;
	}

	rc = obd_iocontrol(cmd, exp, size, copy, NULL);
out:
	OBD_FREE(copy, size);
=======
	copy = memdup_user(data, size);
	if (IS_ERR(copy))
		return PTR_ERR(copy);

	rc = obd_iocontrol(cmd, exp, size, copy, NULL);
	kfree(copy);
>>>>>>> v4.9.227

	return rc;
}

static int quotactl_ioctl(struct ll_sb_info *sbi, struct if_quotactl *qctl)
{
	int cmd = qctl->qc_cmd;
	int type = qctl->qc_type;
	int id = qctl->qc_id;
	int valid = qctl->qc_valid;
	int rc = 0;

	switch (cmd) {
	case LUSTRE_Q_INVALIDATE:
	case LUSTRE_Q_FINVALIDATE:
	case Q_QUOTAON:
	case Q_QUOTAOFF:
	case Q_SETQUOTA:
	case Q_SETINFO:
<<<<<<< HEAD
		if (!capable(CFS_CAP_SYS_ADMIN) ||
		    sbi->ll_flags & LL_SBI_RMT_CLIENT)
=======
		if (!capable(CFS_CAP_SYS_ADMIN))
>>>>>>> v4.9.227
			return -EPERM;
		break;
	case Q_GETQUOTA:
		if (((type == USRQUOTA &&
		      !uid_eq(current_euid(), make_kuid(&init_user_ns, id))) ||
		     (type == GRPQUOTA &&
		      !in_egroup_p(make_kgid(&init_user_ns, id)))) &&
<<<<<<< HEAD
		    (!capable(CFS_CAP_SYS_ADMIN) ||
		     sbi->ll_flags & LL_SBI_RMT_CLIENT))
=======
		      !capable(CFS_CAP_SYS_ADMIN))
>>>>>>> v4.9.227
			return -EPERM;
		break;
	case Q_GETINFO:
		break;
	default:
		CERROR("unsupported quotactl op: %#x\n", cmd);
		return -ENOTTY;
	}

	if (valid != QC_GENERAL) {
<<<<<<< HEAD
		if (sbi->ll_flags & LL_SBI_RMT_CLIENT)
			return -EOPNOTSUPP;

=======
>>>>>>> v4.9.227
		if (cmd == Q_GETINFO)
			qctl->qc_cmd = Q_GETOINFO;
		else if (cmd == Q_GETQUOTA)
			qctl->qc_cmd = Q_GETOQUOTA;
		else
			return -EINVAL;

		switch (valid) {
		case QC_MDTIDX:
			rc = obd_iocontrol(OBD_IOC_QUOTACTL, sbi->ll_md_exp,
					   sizeof(*qctl), qctl, NULL);
			break;
		case QC_OSTIDX:
			rc = obd_iocontrol(OBD_IOC_QUOTACTL, sbi->ll_dt_exp,
					   sizeof(*qctl), qctl, NULL);
			break;
		case QC_UUID:
			rc = obd_iocontrol(OBD_IOC_QUOTACTL, sbi->ll_md_exp,
					   sizeof(*qctl), qctl, NULL);
			if (rc == -EAGAIN)
				rc = obd_iocontrol(OBD_IOC_QUOTACTL,
						   sbi->ll_dt_exp,
						   sizeof(*qctl), qctl, NULL);
			break;
		default:
			rc = -EINVAL;
			break;
		}

		if (rc)
			return rc;

		qctl->qc_cmd = cmd;
	} else {
		struct obd_quotactl *oqctl;

		oqctl = kzalloc(sizeof(*oqctl), GFP_NOFS);
		if (!oqctl)
			return -ENOMEM;

		QCTL_COPY(oqctl, qctl);
		rc = obd_quotactl(sbi->ll_md_exp, oqctl);
		if (rc) {
			if (rc != -EALREADY && cmd == Q_QUOTAON) {
				oqctl->qc_cmd = Q_QUOTAOFF;
				obd_quotactl(sbi->ll_md_exp, oqctl);
			}
<<<<<<< HEAD
			OBD_FREE_PTR(oqctl);
			return rc;
		}
		/* If QIF_SPACE is not set, client should collect the
		 * space usage from OSSs by itself */
=======
			kfree(oqctl);
			return rc;
		}
		/* If QIF_SPACE is not set, client should collect the
		 * space usage from OSSs by itself
		 */
>>>>>>> v4.9.227
		if (cmd == Q_GETQUOTA &&
		    !(oqctl->qc_dqblk.dqb_valid & QIF_SPACE) &&
		    !oqctl->qc_dqblk.dqb_curspace) {
			struct obd_quotactl *oqctl_tmp;

			oqctl_tmp = kzalloc(sizeof(*oqctl_tmp), GFP_NOFS);
			if (!oqctl_tmp) {
				rc = -ENOMEM;
				goto out;
			}

			oqctl_tmp->qc_cmd = Q_GETOQUOTA;
			oqctl_tmp->qc_id = oqctl->qc_id;
			oqctl_tmp->qc_type = oqctl->qc_type;

			/* collect space usage from OSTs */
			oqctl_tmp->qc_dqblk.dqb_curspace = 0;
			rc = obd_quotactl(sbi->ll_dt_exp, oqctl_tmp);
			if (!rc || rc == -EREMOTEIO) {
				oqctl->qc_dqblk.dqb_curspace =
					oqctl_tmp->qc_dqblk.dqb_curspace;
				oqctl->qc_dqblk.dqb_valid |= QIF_SPACE;
			}

			/* collect space & inode usage from MDTs */
			oqctl_tmp->qc_dqblk.dqb_curspace = 0;
			oqctl_tmp->qc_dqblk.dqb_curinodes = 0;
			rc = obd_quotactl(sbi->ll_md_exp, oqctl_tmp);
			if (!rc || rc == -EREMOTEIO) {
				oqctl->qc_dqblk.dqb_curspace +=
					oqctl_tmp->qc_dqblk.dqb_curspace;
				oqctl->qc_dqblk.dqb_curinodes =
					oqctl_tmp->qc_dqblk.dqb_curinodes;
				oqctl->qc_dqblk.dqb_valid |= QIF_INODES;
			} else {
				oqctl->qc_dqblk.dqb_valid &= ~QIF_SPACE;
			}

<<<<<<< HEAD
			OBD_FREE_PTR(oqctl_tmp);
		}
out:
		QCTL_COPY(qctl, oqctl);
		OBD_FREE_PTR(oqctl);
=======
			kfree(oqctl_tmp);
		}
out:
		QCTL_COPY(qctl, oqctl);
		kfree(oqctl);
>>>>>>> v4.9.227
	}

	return rc;
}

<<<<<<< HEAD
static char *
ll_getname(const char __user *filename)
{
	int ret = 0, len;
	char *tmp = __getname();

	if (!tmp)
		return ERR_PTR(-ENOMEM);

	len = strncpy_from_user(tmp, filename, PATH_MAX);
	if (len == 0)
		ret = -ENOENT;
	else if (len > PATH_MAX)
		ret = -ENAMETOOLONG;

	if (ret) {
		__putname(tmp);
=======
/* This function tries to get a single name component,
 * to send to the server. No actual path traversal involved,
 * so we limit to NAME_MAX
 */
static char *ll_getname(const char __user *filename)
{
	int ret = 0, len;
	char *tmp;

	tmp = kzalloc(NAME_MAX + 1, GFP_KERNEL);
	if (!tmp)
		return ERR_PTR(-ENOMEM);

	len = strncpy_from_user(tmp, filename, NAME_MAX + 1);
	if (len < 0)
		ret = len;
	else if (len == 0)
		ret = -ENOENT;
	else if (len > NAME_MAX && tmp[NAME_MAX] != 0)
		ret = -ENAMETOOLONG;

	if (ret) {
		kfree(tmp);
>>>>>>> v4.9.227
		tmp =  ERR_PTR(ret);
	}
	return tmp;
}

<<<<<<< HEAD
#define ll_putname(filename) __putname(filename)

static long ll_dir_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct inode *inode = file->f_dentry->d_inode;
=======
#define ll_putname(filename) kfree(filename)

static long ll_dir_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct inode *inode = file_inode(file);
>>>>>>> v4.9.227
	struct ll_sb_info *sbi = ll_i2sbi(inode);
	struct obd_ioctl_data *data;
	int rc = 0;

<<<<<<< HEAD
	CDEBUG(D_VFSTRACE, "VFS Op:inode=%lu/%u(%p), cmd=%#x\n",
	       inode->i_ino, inode->i_generation, inode, cmd);
=======
	CDEBUG(D_VFSTRACE, "VFS Op:inode="DFID"(%p), cmd=%#x\n",
	       PFID(ll_inode2fid(inode)), inode, cmd);
>>>>>>> v4.9.227

	/* asm-ppc{,64} declares TCGETS, et. al. as type 't' not 'T' */
	if (_IOC_TYPE(cmd) == 'T' || _IOC_TYPE(cmd) == 't') /* tty ioctls */
		return -ENOTTY;

	ll_stats_ops_tally(ll_i2sbi(inode), LPROC_LL_IOCTL, 1);
	switch (cmd) {
	case FSFILT_IOC_GETFLAGS:
	case FSFILT_IOC_SETFLAGS:
		return ll_iocontrol(inode, file, cmd, arg);
	case FSFILT_IOC_GETVERSION_OLD:
	case FSFILT_IOC_GETVERSION:
<<<<<<< HEAD
		return put_user(inode->i_generation, (int *)arg);
=======
		return put_user(inode->i_generation, (int __user *)arg);
>>>>>>> v4.9.227
	/* We need to special case any other ioctls we want to handle,
	 * to send them to the MDS/OST as appropriate and to properly
	 * network encode the arg field.
	case FSFILT_IOC_SETVERSION_OLD:
	case FSFILT_IOC_SETVERSION:
	*/
	case LL_IOC_GET_MDTIDX: {
		int mdtidx;

		mdtidx = ll_get_mdt_idx(inode);
		if (mdtidx < 0)
			return mdtidx;

<<<<<<< HEAD
		if (put_user((int)mdtidx, (int *)arg))
=======
		if (put_user((int)mdtidx, (int __user *)arg))
>>>>>>> v4.9.227
			return -EFAULT;

		return 0;
	}
	case IOC_MDC_LOOKUP: {
<<<<<<< HEAD
		struct ptlrpc_request *request = NULL;
		int namelen, len = 0;
		char *buf = NULL;
		char *filename;
		struct md_op_data *op_data;

		rc = obd_ioctl_getdata(&buf, &len, (void *)arg);
=======
		int namelen, len = 0;
		char *buf = NULL;
		char *filename;

		rc = obd_ioctl_getdata(&buf, &len, (void __user *)arg);
>>>>>>> v4.9.227
		if (rc)
			return rc;
		data = (void *)buf;

		filename = data->ioc_inlbuf1;
		namelen = strlen(filename);

		if (namelen < 1) {
			CDEBUG(D_INFO, "IOC_MDC_LOOKUP missing filename\n");
			rc = -EINVAL;
			goto out_free;
		}

<<<<<<< HEAD
		op_data = ll_prep_md_op_data(NULL, inode, NULL, filename, namelen,
					     0, LUSTRE_OPC_ANY, NULL);
		if (IS_ERR(op_data)) {
			rc = PTR_ERR(op_data);
			goto out_free;
		}

		op_data->op_valid = OBD_MD_FLID;
		rc = md_getattr_name(sbi->ll_md_exp, op_data, &request);
		ll_finish_md_op_data(op_data);
		if (rc < 0) {
			CDEBUG(D_INFO, "md_getattr_name: %d\n", rc);
			goto out_free;
		}
		ptlrpc_req_finished(request);
=======
		rc = ll_get_fid_by_name(inode, filename, namelen, NULL);
		if (rc < 0) {
			CERROR("%s: lookup %.*s failed: rc = %d\n",
			       ll_get_fsname(inode->i_sb, NULL, 0), namelen,
			       filename, rc);
			goto out_free;
		}
>>>>>>> v4.9.227
out_free:
		obd_ioctl_freedata(buf, len);
		return rc;
	}
	case LL_IOC_LMV_SETSTRIPE: {
		struct lmv_user_md  *lum;
		char		*buf = NULL;
		char		*filename;
		int		 namelen = 0;
		int		 lumlen = 0;
<<<<<<< HEAD
		int		 len;
		int		 rc;

		rc = obd_ioctl_getdata(&buf, &len, (void *)arg);
=======
		umode_t mode;
		int		 len;
		int		 rc;

		rc = obd_ioctl_getdata(&buf, &len, (void __user *)arg);
>>>>>>> v4.9.227
		if (rc)
			return rc;

		data = (void *)buf;
<<<<<<< HEAD
		if (data->ioc_inlbuf1 == NULL || data->ioc_inlbuf2 == NULL ||
=======
		if (!data->ioc_inlbuf1 || !data->ioc_inlbuf2 ||
>>>>>>> v4.9.227
		    data->ioc_inllen1 == 0 || data->ioc_inllen2 == 0) {
			rc = -EINVAL;
			goto lmv_out_free;
		}

		filename = data->ioc_inlbuf1;
		namelen = data->ioc_inllen1;

		if (namelen < 1) {
			CDEBUG(D_INFO, "IOC_MDC_LOOKUP missing filename\n");
			rc = -EINVAL;
			goto lmv_out_free;
		}
		lum = (struct lmv_user_md *)data->ioc_inlbuf2;
		lumlen = data->ioc_inllen2;

		if (lum->lum_magic != LMV_USER_MAGIC ||
		    lumlen != sizeof(*lum)) {
			CERROR("%s: wrong lum magic %x or size %d: rc = %d\n",
			       filename, lum->lum_magic, lumlen, -EFAULT);
			rc = -EINVAL;
			goto lmv_out_free;
		}

<<<<<<< HEAD
		/**
		 * ll_dir_setdirstripe will be used to set dir stripe
		 *  mdc_create--->mdt_reint_create (with dirstripe)
		 */
		rc = ll_dir_setdirstripe(inode, lum, filename);
lmv_out_free:
		obd_ioctl_freedata(buf, len);
		return rc;

=======
#if OBD_OCD_VERSION(2, 9, 50, 0) > LUSTRE_VERSION_CODE
		mode = data->ioc_type != 0 ? data->ioc_type : S_IRWXUGO;
#else
		mode = data->ioc_type;
#endif
		rc = ll_dir_setdirstripe(inode, lum, filename, mode);
lmv_out_free:
		obd_ioctl_freedata(buf, len);
		return rc;
	}
	case LL_IOC_LMV_SET_DEFAULT_STRIPE: {
		struct lmv_user_md __user *ulump;
		struct lmv_user_md lum;
		int rc;

		ulump = (struct lmv_user_md __user *)arg;
		if (copy_from_user(&lum, ulump, sizeof(lum)))
			return -EFAULT;

		if (lum.lum_magic != LMV_USER_MAGIC)
			return -EINVAL;

		rc = ll_dir_setstripe(inode, (struct lov_user_md *)&lum, 0);

		return rc;
>>>>>>> v4.9.227
	}
	case LL_IOC_LOV_SETSTRIPE: {
		struct lov_user_md_v3 lumv3;
		struct lov_user_md_v1 *lumv1 = (struct lov_user_md_v1 *)&lumv3;
<<<<<<< HEAD
		struct lov_user_md_v1 *lumv1p = (struct lov_user_md_v1 *)arg;
		struct lov_user_md_v3 *lumv3p = (struct lov_user_md_v3 *)arg;
=======
		struct lov_user_md_v1 __user *lumv1p = (void __user *)arg;
		struct lov_user_md_v3 __user *lumv3p = (void __user *)arg;
>>>>>>> v4.9.227

		int set_default = 0;

		LASSERT(sizeof(lumv3) == sizeof(*lumv3p));
		LASSERT(sizeof(lumv3.lmm_objects[0]) ==
			sizeof(lumv3p->lmm_objects[0]));
		/* first try with v1 which is smaller than v3 */
		if (copy_from_user(lumv1, lumv1p, sizeof(*lumv1)))
			return -EFAULT;

<<<<<<< HEAD
		if ((lumv1->lmm_magic == LOV_USER_MAGIC_V3) ) {
=======
		if (lumv1->lmm_magic == LOV_USER_MAGIC_V3) {
>>>>>>> v4.9.227
			if (copy_from_user(&lumv3, lumv3p, sizeof(lumv3)))
				return -EFAULT;
		}

<<<<<<< HEAD
		if (inode->i_sb->s_root == file->f_dentry)
=======
		if (is_root_inode(inode))
>>>>>>> v4.9.227
			set_default = 1;

		/* in v1 and v3 cases lumv1 points to data */
		rc = ll_dir_setstripe(inode, lumv1, set_default);

		return rc;
	}
	case LL_IOC_LMV_GETSTRIPE: {
<<<<<<< HEAD
		struct lmv_user_md *lump = (struct lmv_user_md *)arg;
		struct lmv_user_md lum;
		struct lmv_user_md *tmp;
		int lum_size;
		int rc = 0;
		int mdtindex;

		if (copy_from_user(&lum, lump, sizeof(struct lmv_user_md)))
			return -EFAULT;

		if (lum.lum_magic != LMV_MAGIC_V1)
			return -EINVAL;

		lum_size = lmv_user_md_size(1, LMV_MAGIC_V1);
		tmp = kzalloc(lum_size, GFP_NOFS);
		if (!tmp) {
			rc = -ENOMEM;
			goto free_lmv;
		}

		*tmp = lum;
		tmp->lum_type = LMV_STRIPE_TYPE;
		tmp->lum_stripe_count = 1;
		mdtindex = ll_get_mdt_idx(inode);
		if (mdtindex < 0) {
			rc = -ENOMEM;
			goto free_lmv;
		}

		tmp->lum_stripe_offset = mdtindex;
		tmp->lum_objects[0].lum_mds = mdtindex;
		memcpy(&tmp->lum_objects[0].lum_fid, ll_inode2fid(inode),
		       sizeof(struct lu_fid));
		if (copy_to_user((void *)arg, tmp, lum_size)) {
			rc = -EFAULT;
			goto free_lmv;
		}
free_lmv:
		if (tmp)
			OBD_FREE(tmp, lum_size);
		return rc;
	}
	case LL_IOC_REMOVE_ENTRY: {
		char		*filename = NULL;
		int		 namelen = 0;
		int		 rc;

		/* Here is a little hack to avoid sending REINT_RMENTRY to
		 * unsupported server, which might crash the server(LU-2730),
		 * Because both LVB_TYPE and REINT_RMENTRY will be supported
		 * on 2.4, we use OBD_CONNECT_LVB_TYPE to detect whether the
		 * server will support REINT_RMENTRY XXX*/
		if (!(exp_connect_flags(sbi->ll_md_exp) & OBD_CONNECT_LVB_TYPE))
			return -ENOTSUPP;

		filename = ll_getname((const char *)arg);
		if (IS_ERR(filename))
			return PTR_ERR(filename);

		namelen = strlen(filename);
		if (namelen < 1) {
			rc = -EINVAL;
			goto out_rmdir;
		}

		rc = ll_rmdir_entry(inode, filename, namelen);
out_rmdir:
		if (filename)
			ll_putname(filename);
		return rc;
	}
	case LL_IOC_LOV_SWAP_LAYOUTS:
		return -EPERM;
	case LL_IOC_OBD_STATFS:
		return ll_obd_statfs(inode, (void *)arg);
=======
		struct lmv_user_md __user *ulmv;
		struct lmv_user_md lum;
		struct ptlrpc_request *request = NULL;
		struct lmv_user_md *tmp = NULL;
		union lmv_mds_md *lmm = NULL;
		u64 valid = 0;
		int stripe_count;
		int mdt_index;
		int lum_size;
		int lmmsize;
		int rc;
		int i;

		ulmv = (struct lmv_user_md __user *)arg;
		if (copy_from_user(&lum, ulmv, sizeof(*ulmv)))
			return -EFAULT;

		/*
		 * lum_magic will indicate which stripe the ioctl will like
		 * to get, LMV_MAGIC_V1 is for normal LMV stripe, LMV_USER_MAGIC
		 * is for default LMV stripe
		 */
		if (lum.lum_magic == LMV_MAGIC_V1)
			valid |= OBD_MD_MEA;
		else if (lum.lum_magic == LMV_USER_MAGIC)
			valid |= OBD_MD_DEFAULT_MEA;
		else
			return -EINVAL;

		rc = ll_dir_getstripe(inode, (void **)&lmm, &lmmsize, &request,
				      valid);
		if (rc)
			goto finish_req;

		/* Get default LMV EA */
		if (lum.lum_magic == LMV_USER_MAGIC) {
			if (rc)
				goto finish_req;

			if (lmmsize > sizeof(*ulmv)) {
				rc = -EINVAL;
				goto finish_req;
			}

			if (copy_to_user(ulmv, lmm, lmmsize))
				rc = -EFAULT;

			goto finish_req;
		}

		stripe_count = lmv_mds_md_stripe_count_get(lmm);
		lum_size = lmv_user_md_size(stripe_count, LMV_MAGIC_V1);
		tmp = kzalloc(lum_size, GFP_NOFS);
		if (!tmp) {
			rc = -ENOMEM;
			goto finish_req;
		}

		mdt_index = ll_get_mdt_idx(inode);
		if (mdt_index < 0) {
			rc = -ENOMEM;
			goto out_tmp;
		}
		tmp->lum_magic = LMV_MAGIC_V1;
		tmp->lum_stripe_count = 0;
		tmp->lum_stripe_offset = mdt_index;
		for (i = 0; i < stripe_count; i++) {
			struct lu_fid fid;

			fid_le_to_cpu(&fid, &lmm->lmv_md_v1.lmv_stripe_fids[i]);
			mdt_index = ll_get_mdt_idx_by_fid(sbi, &fid);
			if (mdt_index < 0) {
				rc = mdt_index;
				goto out_tmp;
			}
			tmp->lum_objects[i].lum_mds = mdt_index;
			tmp->lum_objects[i].lum_fid = fid;
			tmp->lum_stripe_count++;
		}

		if (copy_to_user(ulmv, tmp, lum_size)) {
			rc = -EFAULT;
			goto out_tmp;
		}
out_tmp:
		kfree(tmp);
finish_req:
		ptlrpc_req_finished(request);
		return rc;
	}

	case LL_IOC_LOV_SWAP_LAYOUTS:
		return -EPERM;
	case IOC_OBD_STATFS:
		return ll_obd_statfs(inode, (void __user *)arg);
>>>>>>> v4.9.227
	case LL_IOC_LOV_GETSTRIPE:
	case LL_IOC_MDC_GETINFO:
	case IOC_MDC_GETFILEINFO:
	case IOC_MDC_GETFILESTRIPE: {
		struct ptlrpc_request *request = NULL;
<<<<<<< HEAD
		struct lov_user_md *lump;
=======
		struct lov_user_md __user *lump;
>>>>>>> v4.9.227
		struct lov_mds_md *lmm = NULL;
		struct mdt_body *body;
		char *filename = NULL;
		int lmmsize;

		if (cmd == IOC_MDC_GETFILEINFO ||
		    cmd == IOC_MDC_GETFILESTRIPE) {
<<<<<<< HEAD
			filename = ll_getname((const char *)arg);
=======
			filename = ll_getname((const char __user *)arg);
>>>>>>> v4.9.227
			if (IS_ERR(filename))
				return PTR_ERR(filename);

			rc = ll_lov_getstripe_ea_info(inode, filename, &lmm,
						      &lmmsize, &request);
		} else {
<<<<<<< HEAD
			rc = ll_dir_getstripe(inode, &lmm, &lmmsize, &request);
=======
			rc = ll_dir_getstripe(inode, (void **)&lmm, &lmmsize,
					      &request, 0);
>>>>>>> v4.9.227
		}

		if (request) {
			body = req_capsule_server_get(&request->rq_pill,
						      &RMF_MDT_BODY);
<<<<<<< HEAD
			LASSERT(body != NULL);
=======
			LASSERT(body);
>>>>>>> v4.9.227
		} else {
			goto out_req;
		}

		if (rc < 0) {
			if (rc == -ENODATA && (cmd == IOC_MDC_GETFILEINFO ||
					       cmd == LL_IOC_MDC_GETINFO)) {
				rc = 0;
				goto skip_lmm;
<<<<<<< HEAD
			}
			else
				goto out_req;
=======
			} else {
				goto out_req;
			}
>>>>>>> v4.9.227
		}

		if (cmd == IOC_MDC_GETFILESTRIPE ||
		    cmd == LL_IOC_LOV_GETSTRIPE) {
<<<<<<< HEAD
			lump = (struct lov_user_md *)arg;
		} else {
			struct lov_user_mds_data *lmdp;
			lmdp = (struct lov_user_mds_data *)arg;
=======
			lump = (struct lov_user_md __user *)arg;
		} else {
			struct lov_user_mds_data __user *lmdp;

			lmdp = (struct lov_user_mds_data __user *)arg;
>>>>>>> v4.9.227
			lump = &lmdp->lmd_lmm;
		}
		if (copy_to_user(lump, lmm, lmmsize)) {
			if (copy_to_user(lump, lmm, sizeof(*lump))) {
				rc = -EFAULT;
				goto out_req;
			}
			rc = -EOVERFLOW;
		}
skip_lmm:
		if (cmd == IOC_MDC_GETFILEINFO || cmd == LL_IOC_MDC_GETINFO) {
<<<<<<< HEAD
			struct lov_user_mds_data *lmdp;
			lstat_t st = { 0 };

			st.st_dev     = inode->i_sb->s_dev;
			st.st_mode    = body->mode;
			st.st_nlink   = body->nlink;
			st.st_uid     = body->uid;
			st.st_gid     = body->gid;
			st.st_rdev    = body->rdev;
			st.st_size    = body->size;
			st.st_blksize = PAGE_CACHE_SIZE;
			st.st_blocks  = body->blocks;
			st.st_atime   = body->atime;
			st.st_mtime   = body->mtime;
			st.st_ctime   = body->ctime;
			st.st_ino     = inode->i_ino;

			lmdp = (struct lov_user_mds_data *)arg;
=======
			struct lov_user_mds_data __user *lmdp;
			lstat_t st = { 0 };

			st.st_dev     = inode->i_sb->s_dev;
			st.st_mode    = body->mbo_mode;
			st.st_nlink   = body->mbo_nlink;
			st.st_uid     = body->mbo_uid;
			st.st_gid     = body->mbo_gid;
			st.st_rdev    = body->mbo_rdev;
			st.st_size    = body->mbo_size;
			st.st_blksize = PAGE_SIZE;
			st.st_blocks  = body->mbo_blocks;
			st.st_atime   = body->mbo_atime;
			st.st_mtime   = body->mbo_mtime;
			st.st_ctime   = body->mbo_ctime;
			st.st_ino     = cl_fid_build_ino(&body->mbo_fid1,
							 sbi->ll_flags &
							 LL_SBI_32BIT_API);

			lmdp = (struct lov_user_mds_data __user *)arg;
>>>>>>> v4.9.227
			if (copy_to_user(&lmdp->lmd_st, &st, sizeof(st))) {
				rc = -EFAULT;
				goto out_req;
			}
		}

out_req:
		ptlrpc_req_finished(request);
		if (filename)
			ll_putname(filename);
		return rc;
	}
	case IOC_LOV_GETINFO: {
<<<<<<< HEAD
		struct lov_user_mds_data *lumd;
		struct lov_stripe_md *lsm;
		struct lov_user_md *lum;
=======
		struct lov_user_mds_data __user *lumd;
		struct lov_stripe_md *lsm;
		struct lov_user_md __user *lum;
>>>>>>> v4.9.227
		struct lov_mds_md *lmm;
		int lmmsize;
		lstat_t st;

<<<<<<< HEAD
		lumd = (struct lov_user_mds_data *)arg;
=======
		lumd = (struct lov_user_mds_data __user *)arg;
>>>>>>> v4.9.227
		lum = &lumd->lmd_lmm;

		rc = ll_get_max_mdsize(sbi, &lmmsize);
		if (rc)
			return rc;

<<<<<<< HEAD
		OBD_ALLOC_LARGE(lmm, lmmsize);
		if (lmm == NULL)
=======
		lmm = libcfs_kvzalloc(lmmsize, GFP_NOFS);
		if (!lmm)
>>>>>>> v4.9.227
			return -ENOMEM;
		if (copy_from_user(lmm, lum, lmmsize)) {
			rc = -EFAULT;
			goto free_lmm;
		}

		switch (lmm->lmm_magic) {
		case LOV_USER_MAGIC_V1:
<<<<<<< HEAD
			if (LOV_USER_MAGIC_V1 == cpu_to_le32(LOV_USER_MAGIC_V1))
=======
			if (cpu_to_le32(LOV_USER_MAGIC_V1) == LOV_USER_MAGIC_V1)
>>>>>>> v4.9.227
				break;
			/* swab objects first so that stripes num will be sane */
			lustre_swab_lov_user_md_objects(
				((struct lov_user_md_v1 *)lmm)->lmm_objects,
				((struct lov_user_md_v1 *)lmm)->lmm_stripe_count);
			lustre_swab_lov_user_md_v1((struct lov_user_md_v1 *)lmm);
			break;
		case LOV_USER_MAGIC_V3:
<<<<<<< HEAD
			if (LOV_USER_MAGIC_V3 == cpu_to_le32(LOV_USER_MAGIC_V3))
=======
			if (cpu_to_le32(LOV_USER_MAGIC_V3) == LOV_USER_MAGIC_V3)
>>>>>>> v4.9.227
				break;
			/* swab objects first so that stripes num will be sane */
			lustre_swab_lov_user_md_objects(
				((struct lov_user_md_v3 *)lmm)->lmm_objects,
				((struct lov_user_md_v3 *)lmm)->lmm_stripe_count);
			lustre_swab_lov_user_md_v3((struct lov_user_md_v3 *)lmm);
			break;
		default:
			rc = -EINVAL;
			goto free_lmm;
		}

		rc = obd_unpackmd(sbi->ll_dt_exp, &lsm, lmm, lmmsize);
		if (rc < 0) {
			rc = -ENOMEM;
			goto free_lmm;
		}

		/* Perform glimpse_size operation. */
		memset(&st, 0, sizeof(st));

		rc = ll_glimpse_ioctl(sbi, lsm, &st);
		if (rc)
			goto free_lsm;

		if (copy_to_user(&lumd->lmd_st, &st, sizeof(st))) {
			rc = -EFAULT;
			goto free_lsm;
		}

free_lsm:
		obd_free_memmd(sbi->ll_dt_exp, &lsm);
free_lmm:
<<<<<<< HEAD
		OBD_FREE_LARGE(lmm, lmmsize);
		return rc;
	}
	case OBD_IOC_LLOG_CATINFO: {
		return -EOPNOTSUPP;
	}
=======
		kvfree(lmm);
		return rc;
	}
>>>>>>> v4.9.227
	case OBD_IOC_QUOTACHECK: {
		struct obd_quotactl *oqctl;
		int error = 0;

<<<<<<< HEAD
		if (!capable(CFS_CAP_SYS_ADMIN) ||
		    sbi->ll_flags & LL_SBI_RMT_CLIENT)
=======
		if (!capable(CFS_CAP_SYS_ADMIN))
>>>>>>> v4.9.227
			return -EPERM;

		oqctl = kzalloc(sizeof(*oqctl), GFP_NOFS);
		if (!oqctl)
			return -ENOMEM;
		oqctl->qc_type = arg;
		rc = obd_quotacheck(sbi->ll_md_exp, oqctl);
		if (rc < 0) {
			CDEBUG(D_INFO, "md_quotacheck failed: rc %d\n", rc);
			error = rc;
		}

		rc = obd_quotacheck(sbi->ll_dt_exp, oqctl);
		if (rc < 0)
			CDEBUG(D_INFO, "obd_quotacheck failed: rc %d\n", rc);

<<<<<<< HEAD
		OBD_FREE_PTR(oqctl);
=======
		kfree(oqctl);
>>>>>>> v4.9.227
		return error ?: rc;
	}
	case OBD_IOC_POLL_QUOTACHECK: {
		struct if_quotacheck *check;

<<<<<<< HEAD
		if (!capable(CFS_CAP_SYS_ADMIN) ||
		    sbi->ll_flags & LL_SBI_RMT_CLIENT)
=======
		if (!capable(CFS_CAP_SYS_ADMIN))
>>>>>>> v4.9.227
			return -EPERM;

		check = kzalloc(sizeof(*check), GFP_NOFS);
		if (!check)
			return -ENOMEM;

		rc = obd_iocontrol(cmd, sbi->ll_md_exp, 0, (void *)check,
				   NULL);
		if (rc) {
			CDEBUG(D_QUOTA, "mdc ioctl %d failed: %d\n", cmd, rc);
<<<<<<< HEAD
			if (copy_to_user((void *)arg, check,
					     sizeof(*check)))
=======
			if (copy_to_user((void __user *)arg, check,
					 sizeof(*check)))
>>>>>>> v4.9.227
				CDEBUG(D_QUOTA, "copy_to_user failed\n");
			goto out_poll;
		}

		rc = obd_iocontrol(cmd, sbi->ll_dt_exp, 0, (void *)check,
				   NULL);
		if (rc) {
			CDEBUG(D_QUOTA, "osc ioctl %d failed: %d\n", cmd, rc);
<<<<<<< HEAD
			if (copy_to_user((void *)arg, check,
					     sizeof(*check)))
=======
			if (copy_to_user((void __user *)arg, check,
					 sizeof(*check)))
>>>>>>> v4.9.227
				CDEBUG(D_QUOTA, "copy_to_user failed\n");
			goto out_poll;
		}
out_poll:
<<<<<<< HEAD
		OBD_FREE_PTR(check);
		return rc;
	}
#if LUSTRE_VERSION_CODE < OBD_OCD_VERSION(2, 7, 50, 0)
	case LL_IOC_QUOTACTL_18: {
		/* copy the old 1.x quota struct for internal use, then copy
		 * back into old format struct.  For 1.8 compatibility. */
		struct if_quotactl_18 *qctl_18;
		struct if_quotactl *qctl_20;

		qctl_18 = kzalloc(sizeof(*qctl_18), GFP_NOFS);
		if (!qctl_18)
			return -ENOMEM;

		qctl_20 = kzalloc(sizeof(*qctl_20), GFP_NOFS);
		if (!qctl_20) {
			rc = -ENOMEM;
			goto out_quotactl_18;
		}

		if (copy_from_user(qctl_18, (void *)arg, sizeof(*qctl_18))) {
			rc = -ENOMEM;
			goto out_quotactl_20;
		}

		QCTL_COPY(qctl_20, qctl_18);
		qctl_20->qc_idx = 0;

		/* XXX: dqb_valid was borrowed as a flag to mark that
		 *      only mds quota is wanted */
		if (qctl_18->qc_cmd == Q_GETQUOTA &&
		    qctl_18->qc_dqblk.dqb_valid) {
			qctl_20->qc_valid = QC_MDTIDX;
			qctl_20->qc_dqblk.dqb_valid = 0;
		} else if (qctl_18->obd_uuid.uuid[0] != '\0') {
			qctl_20->qc_valid = QC_UUID;
			qctl_20->obd_uuid = qctl_18->obd_uuid;
		} else {
			qctl_20->qc_valid = QC_GENERAL;
		}

		rc = quotactl_ioctl(sbi, qctl_20);

		if (rc == 0) {
			QCTL_COPY(qctl_18, qctl_20);
			qctl_18->obd_uuid = qctl_20->obd_uuid;

			if (copy_to_user((void *)arg, qctl_18,
					     sizeof(*qctl_18)))
				rc = -EFAULT;
		}

out_quotactl_20:
		OBD_FREE_PTR(qctl_20);
out_quotactl_18:
		OBD_FREE_PTR(qctl_18);
		return rc;
	}
#else
#warning "remove old LL_IOC_QUOTACTL_18 compatibility code"
#endif /* LUSTRE_VERSION_CODE < OBD_OCD_VERSION(2, 7, 50, 0) */
	case LL_IOC_QUOTACTL: {
=======
		kfree(check);
		return rc;
	}
	case OBD_IOC_QUOTACTL: {
>>>>>>> v4.9.227
		struct if_quotactl *qctl;

		qctl = kzalloc(sizeof(*qctl), GFP_NOFS);
		if (!qctl)
			return -ENOMEM;

<<<<<<< HEAD
		if (copy_from_user(qctl, (void *)arg, sizeof(*qctl))) {
=======
		if (copy_from_user(qctl, (void __user *)arg, sizeof(*qctl))) {
>>>>>>> v4.9.227
			rc = -EFAULT;
			goto out_quotactl;
		}

		rc = quotactl_ioctl(sbi, qctl);

<<<<<<< HEAD
		if (rc == 0 && copy_to_user((void *)arg, qctl, sizeof(*qctl)))
			rc = -EFAULT;

out_quotactl:
		OBD_FREE_PTR(qctl);
=======
		if (rc == 0 && copy_to_user((void __user *)arg, qctl,
					    sizeof(*qctl)))
			rc = -EFAULT;

out_quotactl:
		kfree(qctl);
>>>>>>> v4.9.227
		return rc;
	}
	case OBD_IOC_GETDTNAME:
	case OBD_IOC_GETMDNAME:
		return ll_get_obd_name(inode, cmd, arg);
	case LL_IOC_FLUSHCTX:
		return ll_flush_ctx(inode);
<<<<<<< HEAD
#ifdef CONFIG_FS_POSIX_ACL
	case LL_IOC_RMTACL: {
	    if (sbi->ll_flags & LL_SBI_RMT_CLIENT &&
		inode == inode->i_sb->s_root->d_inode) {
		struct ll_file_data *fd = LUSTRE_FPRIVATE(file);

		LASSERT(fd != NULL);
		rc = rct_add(&sbi->ll_rct, current_pid(), arg);
		if (!rc)
			fd->fd_flags |= LL_FILE_RMTACL;
		return rc;
	    } else
		return 0;
	}
#endif
=======
>>>>>>> v4.9.227
	case LL_IOC_GETOBDCOUNT: {
		int count, vallen;
		struct obd_export *exp;

<<<<<<< HEAD
		if (copy_from_user(&count, (int *)arg, sizeof(int)))
=======
		if (copy_from_user(&count, (int __user *)arg, sizeof(int)))
>>>>>>> v4.9.227
			return -EFAULT;

		/* get ost count when count is zero, get mdt count otherwise */
		exp = count ? sbi->ll_md_exp : sbi->ll_dt_exp;
		vallen = sizeof(count);
		rc = obd_get_info(NULL, exp, sizeof(KEY_TGT_COUNT),
				  KEY_TGT_COUNT, &vallen, &count, NULL);
		if (rc) {
			CERROR("get target count failed: %d\n", rc);
			return rc;
		}

<<<<<<< HEAD
		if (copy_to_user((int *)arg, &count, sizeof(int)))
=======
		if (copy_to_user((int __user *)arg, &count, sizeof(int)))
>>>>>>> v4.9.227
			return -EFAULT;

		return 0;
	}
	case LL_IOC_PATH2FID:
<<<<<<< HEAD
		if (copy_to_user((void *)arg, ll_inode2fid(inode),
				     sizeof(struct lu_fid)))
			return -EFAULT;
		return 0;
	case LL_IOC_GET_CONNECT_FLAGS: {
		return obd_iocontrol(cmd, sbi->ll_md_exp, 0, NULL, (void *)arg);
	}
	case OBD_IOC_CHANGELOG_SEND:
	case OBD_IOC_CHANGELOG_CLEAR:
		rc = copy_and_ioctl(cmd, sbi->ll_md_exp, (void *)arg,
				    sizeof(struct ioc_changelog));
		return rc;
	case OBD_IOC_FID2PATH:
		return ll_fid2path(inode, (void *)arg);
=======
		if (copy_to_user((void __user *)arg, ll_inode2fid(inode),
				 sizeof(struct lu_fid)))
			return -EFAULT;
		return 0;
	case LL_IOC_GET_CONNECT_FLAGS: {
		return obd_iocontrol(cmd, sbi->ll_md_exp, 0, NULL,
				     (void __user *)arg);
	}
	case OBD_IOC_CHANGELOG_SEND:
	case OBD_IOC_CHANGELOG_CLEAR:
		if (!capable(CFS_CAP_SYS_ADMIN))
			return -EPERM;

		rc = copy_and_ioctl(cmd, sbi->ll_md_exp, (void __user *)arg,
				    sizeof(struct ioc_changelog));
		return rc;
	case OBD_IOC_FID2PATH:
		return ll_fid2path(inode, (void __user *)arg);
	case LL_IOC_GETPARENT:
		return ll_getparent(file, (void __user *)arg);
	case LL_IOC_FID2MDTIDX: {
		struct obd_export *exp = ll_i2mdexp(inode);
		struct lu_fid fid;
		__u32 index;

		if (copy_from_user(&fid, (const struct lu_fid __user *)arg,
				   sizeof(fid)))
			return -EFAULT;

		/* Call mdc_iocontrol */
		rc = obd_iocontrol(LL_IOC_FID2MDTIDX, exp, sizeof(fid), &fid,
				   &index);
		if (rc)
			return rc;

		return index;
	}
>>>>>>> v4.9.227
	case LL_IOC_HSM_REQUEST: {
		struct hsm_user_request	*hur;
		ssize_t			 totalsize;

<<<<<<< HEAD
		hur = kzalloc(sizeof(*hur), GFP_NOFS);
		if (!hur)
			return -ENOMEM;

		/* We don't know the true size yet; copy the fixed-size part */
		if (copy_from_user(hur, (void *)arg, sizeof(*hur))) {
			OBD_FREE_PTR(hur);
			return -EFAULT;
		}

		/* Compute the whole struct size */
		totalsize = hur_len(hur);
		OBD_FREE_PTR(hur);
=======
		hur = memdup_user((void __user *)arg, sizeof(*hur));
		if (IS_ERR(hur))
			return PTR_ERR(hur);

		/* Compute the whole struct size */
		totalsize = hur_len(hur);
		kfree(hur);
>>>>>>> v4.9.227
		if (totalsize < 0)
			return -E2BIG;

		/* Final size will be more than double totalsize */
		if (totalsize >= MDS_MAXREQSIZE / 3)
			return -E2BIG;

<<<<<<< HEAD
		OBD_ALLOC_LARGE(hur, totalsize);
		if (hur == NULL)
			return -ENOMEM;

		/* Copy the whole struct */
		if (copy_from_user(hur, (void *)arg, totalsize)) {
			OBD_FREE_LARGE(hur, totalsize);
=======
		hur = libcfs_kvzalloc(totalsize, GFP_NOFS);
		if (!hur)
			return -ENOMEM;

		/* Copy the whole struct */
		if (copy_from_user(hur, (void __user *)arg, totalsize)) {
			kvfree(hur);
>>>>>>> v4.9.227
			return -EFAULT;
		}

		if (hur->hur_request.hr_action == HUA_RELEASE) {
			const struct lu_fid *fid;
			struct inode *f;
			int i;

			for (i = 0; i < hur->hur_request.hr_itemcount; i++) {
				fid = &hur->hur_user_item[i].hui_fid;
				f = search_inode_for_lustre(inode->i_sb, fid);
				if (IS_ERR(f)) {
					rc = PTR_ERR(f);
					break;
				}

				rc = ll_hsm_release(f);
				iput(f);
				if (rc != 0)
					break;
			}
		} else {
			rc = obd_iocontrol(cmd, ll_i2mdexp(inode), totalsize,
					   hur, NULL);
		}

<<<<<<< HEAD
		OBD_FREE_LARGE(hur, totalsize);
=======
		kvfree(hur);
>>>>>>> v4.9.227

		return rc;
	}
	case LL_IOC_HSM_PROGRESS: {
		struct hsm_progress_kernel	hpk;
		struct hsm_progress		hp;

<<<<<<< HEAD
		if (copy_from_user(&hp, (void *)arg, sizeof(hp)))
=======
		if (copy_from_user(&hp, (void __user *)arg, sizeof(hp)))
>>>>>>> v4.9.227
			return -EFAULT;

		hpk.hpk_fid = hp.hp_fid;
		hpk.hpk_cookie = hp.hp_cookie;
		hpk.hpk_extent = hp.hp_extent;
		hpk.hpk_flags = hp.hp_flags;
		hpk.hpk_errval = hp.hp_errval;
		hpk.hpk_data_version = 0;

		/* File may not exist in Lustre; all progress
<<<<<<< HEAD
		 * reported to Lustre root */
=======
		 * reported to Lustre root
		 */
>>>>>>> v4.9.227
		rc = obd_iocontrol(cmd, sbi->ll_md_exp, sizeof(hpk), &hpk,
				   NULL);
		return rc;
	}
	case LL_IOC_HSM_CT_START:
<<<<<<< HEAD
		rc = copy_and_ioctl(cmd, sbi->ll_md_exp, (void *)arg,
=======
		if (!capable(CFS_CAP_SYS_ADMIN))
			return -EPERM;

		rc = copy_and_ioctl(cmd, sbi->ll_md_exp, (void __user *)arg,
>>>>>>> v4.9.227
				    sizeof(struct lustre_kernelcomm));
		return rc;

	case LL_IOC_HSM_COPY_START: {
		struct hsm_copy	*copy;
		int		 rc;

<<<<<<< HEAD
		copy = kzalloc(sizeof(*copy), GFP_NOFS);
		if (!copy)
			return -ENOMEM;
		if (copy_from_user(copy, (char *)arg, sizeof(*copy))) {
			OBD_FREE_PTR(copy);
			return -EFAULT;
		}

		rc = ll_ioc_copy_start(inode->i_sb, copy);
		if (copy_to_user((char *)arg, copy, sizeof(*copy)))
			rc = -EFAULT;

		OBD_FREE_PTR(copy);
=======
		copy = memdup_user((char __user *)arg, sizeof(*copy));
		if (IS_ERR(copy))
			return PTR_ERR(copy);

		rc = ll_ioc_copy_start(inode->i_sb, copy);
		if (copy_to_user((char __user *)arg, copy, sizeof(*copy)))
			rc = -EFAULT;

		kfree(copy);
>>>>>>> v4.9.227
		return rc;
	}
	case LL_IOC_HSM_COPY_END: {
		struct hsm_copy	*copy;
		int		 rc;

<<<<<<< HEAD
		copy = kzalloc(sizeof(*copy), GFP_NOFS);
		if (!copy)
			return -ENOMEM;
		if (copy_from_user(copy, (char *)arg, sizeof(*copy))) {
			OBD_FREE_PTR(copy);
			return -EFAULT;
		}

		rc = ll_ioc_copy_end(inode->i_sb, copy);
		if (copy_to_user((char *)arg, copy, sizeof(*copy)))
			rc = -EFAULT;

		OBD_FREE_PTR(copy);
		return rc;
	}
	default:
		return obd_iocontrol(cmd, sbi->ll_dt_exp, 0, NULL, (void *)arg);
=======
		copy = memdup_user((char __user *)arg, sizeof(*copy));
		if (IS_ERR(copy))
			return PTR_ERR(copy);

		rc = ll_ioc_copy_end(inode->i_sb, copy);
		if (copy_to_user((char __user *)arg, copy, sizeof(*copy)))
			rc = -EFAULT;

		kfree(copy);
		return rc;
	}
	case LL_IOC_MIGRATE: {
		char *buf = NULL;
		const char *filename;
		int namelen = 0;
		int len;
		int rc;
		int mdtidx;

		rc = obd_ioctl_getdata(&buf, &len, (void __user *)arg);
		if (rc < 0)
			return rc;

		data = (struct obd_ioctl_data *)buf;
		if (!data->ioc_inlbuf1 || !data->ioc_inlbuf2 ||
		    !data->ioc_inllen1 || !data->ioc_inllen2) {
			rc = -EINVAL;
			goto migrate_free;
		}

		filename = data->ioc_inlbuf1;
		namelen = data->ioc_inllen1;
		if (namelen < 1 || namelen != strlen(filename) + 1) {
			rc = -EINVAL;
			goto migrate_free;
		}

		if (data->ioc_inllen2 != sizeof(mdtidx)) {
			rc = -EINVAL;
			goto migrate_free;
		}
		mdtidx = *(int *)data->ioc_inlbuf2;

		rc = ll_migrate(inode, file, mdtidx, filename, namelen - 1);
migrate_free:
		obd_ioctl_freedata(buf, len);

		return rc;
	}

	default:
		return obd_iocontrol(cmd, sbi->ll_dt_exp, 0, NULL,
				     (void __user *)arg);
>>>>>>> v4.9.227
	}
}

static loff_t ll_dir_seek(struct file *file, loff_t offset, int origin)
{
	struct inode *inode = file->f_mapping->host;
	struct ll_file_data *fd = LUSTRE_FPRIVATE(file);
	struct ll_sb_info *sbi = ll_i2sbi(inode);
	int api32 = ll_need_32bit_api(sbi);
	loff_t ret = -EINVAL;

<<<<<<< HEAD
	mutex_lock(&inode->i_mutex);
	switch (origin) {
		case SEEK_SET:
			break;
		case SEEK_CUR:
			offset += file->f_pos;
			break;
		case SEEK_END:
			if (offset > 0)
				goto out;
			if (api32)
				offset += LL_DIR_END_OFF_32BIT;
			else
				offset += LL_DIR_END_OFF;
			break;
		default:
			goto out;
=======
	switch (origin) {
	case SEEK_SET:
		break;
	case SEEK_CUR:
		offset += file->f_pos;
		break;
	case SEEK_END:
		if (offset > 0)
			goto out;
		if (api32)
			offset += LL_DIR_END_OFF_32BIT;
		else
			offset += LL_DIR_END_OFF;
		break;
	default:
		goto out;
>>>>>>> v4.9.227
	}

	if (offset >= 0 &&
	    ((api32 && offset <= LL_DIR_END_OFF_32BIT) ||
	     (!api32 && offset <= LL_DIR_END_OFF))) {
		if (offset != file->f_pos) {
			if ((api32 && offset == LL_DIR_END_OFF_32BIT) ||
			    (!api32 && offset == LL_DIR_END_OFF))
				fd->lfd_pos = MDS_DIR_END_OFF;
			else if (api32 && sbi->ll_flags & LL_SBI_64BIT_HASH)
				fd->lfd_pos = offset << 32;
			else
				fd->lfd_pos = offset;
			file->f_pos = offset;
			file->f_version = 0;
		}
		ret = offset;
	}
	goto out;

out:
<<<<<<< HEAD
	mutex_unlock(&inode->i_mutex);
=======
>>>>>>> v4.9.227
	return ret;
}

static int ll_dir_open(struct inode *inode, struct file *file)
{
	return ll_file_open(inode, file);
}

static int ll_dir_release(struct inode *inode, struct file *file)
{
	return ll_file_release(inode, file);
}

const struct file_operations ll_dir_operations = {
	.llseek   = ll_dir_seek,
	.open     = ll_dir_open,
	.release  = ll_dir_release,
	.read     = generic_read_dir,
<<<<<<< HEAD
	.iterate  = ll_readdir,
=======
	.iterate_shared  = ll_readdir,
>>>>>>> v4.9.227
	.unlocked_ioctl   = ll_dir_ioctl,
	.fsync    = ll_fsync,
};
