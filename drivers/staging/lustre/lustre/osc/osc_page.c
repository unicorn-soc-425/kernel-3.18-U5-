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
 * Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.
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
 * Implementation of cl_page for OSC layer.
 *
 *   Author: Nikita Danilov <nikita.danilov@sun.com>
<<<<<<< HEAD
=======
 *   Author: Jinshan Xiong <jinshan.xiong@intel.com>
>>>>>>> v4.9.227
 */

#define DEBUG_SUBSYSTEM S_OSC

#include "osc_cl_internal.h"

<<<<<<< HEAD
static void osc_lru_del(struct client_obd *cli, struct osc_page *opg, bool del);
static void osc_lru_add(struct client_obd *cli, struct osc_page *opg);
=======
static void osc_lru_del(struct client_obd *cli, struct osc_page *opg);
static void osc_lru_use(struct client_obd *cli, struct osc_page *opg);
>>>>>>> v4.9.227
static int osc_lru_reserve(const struct lu_env *env, struct osc_object *obj,
			   struct osc_page *opg);

/** \addtogroup osc
 *  @{
 */

<<<<<<< HEAD
/*
 * Comment out osc_page_protected because it may sleep inside the
 * the client_obd_list_lock.
 * client_obd_list_lock -> osc_ap_completion -> osc_completion ->
 *   -> osc_page_protected -> osc_page_is_dlocked -> osc_match_base
 *   -> ldlm_lock_match -> sptlrpc_import_check_ctx -> sleep.
 */
#if 0
static int osc_page_is_dlocked(const struct lu_env *env,
			       const struct osc_page *opg,
			       enum cl_lock_mode mode, int pending, int unref)
{
	struct cl_page	 *page;
	struct osc_object      *obj;
	struct osc_thread_info *info;
	struct ldlm_res_id     *resname;
	struct lustre_handle   *lockh;
	ldlm_policy_data_t     *policy;
	ldlm_mode_t	     dlmmode;
	__u64                   flags;

	might_sleep();

	info = osc_env_info(env);
	resname = &info->oti_resname;
	policy = &info->oti_policy;
	lockh = &info->oti_handle;
	page = opg->ops_cl.cpl_page;
	obj = cl2osc(opg->ops_cl.cpl_obj);

	flags = LDLM_FL_TEST_LOCK | LDLM_FL_BLOCK_GRANTED;
	if (pending)
		flags |= LDLM_FL_CBPENDING;

	dlmmode = osc_cl_lock2ldlm(mode) | LCK_PW;
	osc_lock_build_res(env, obj, resname);
	osc_index2policy(policy, page->cp_obj, page->cp_index, page->cp_index);
	return osc_match_base(osc_export(obj), resname, LDLM_EXTENT, policy,
			      dlmmode, &flags, NULL, lockh, unref);
}

/**
 * Checks an invariant that a page in the cache is covered by a lock, as
 * needed.
 */
static int osc_page_protected(const struct lu_env *env,
			      const struct osc_page *opg,
			      enum cl_lock_mode mode, int unref)
{
	struct cl_object_header *hdr;
	struct cl_lock	  *scan;
	struct cl_page	  *page;
	struct cl_lock_descr    *descr;
	int result;

	LINVRNT(!opg->ops_temp);

	page = opg->ops_cl.cpl_page;
	if (page->cp_owner != NULL &&
	    cl_io_top(page->cp_owner)->ci_lockreq == CILR_NEVER)
		/*
		 * If IO is done without locks (liblustre, or lloop), lock is
		 * not required.
		 */
		result = 1;
	else
		/* otherwise check for a DLM lock */
	result = osc_page_is_dlocked(env, opg, mode, 1, unref);
	if (result == 0) {
		/* maybe this page is a part of a lockless io? */
		hdr = cl_object_header(opg->ops_cl.cpl_obj);
		descr = &osc_env_info(env)->oti_descr;
		descr->cld_mode = mode;
		descr->cld_start = page->cp_index;
		descr->cld_end   = page->cp_index;
		spin_lock(&hdr->coh_lock_guard);
		list_for_each_entry(scan, &hdr->coh_locks, cll_linkage) {
			/*
			 * Lock-less sub-lock has to be either in HELD state
			 * (when io is actively going on), or in CACHED state,
			 * when top-lock is being unlocked:
			 * cl_io_unlock()->cl_unuse()->...->lov_lock_unuse().
			 */
			if ((scan->cll_state == CLS_HELD ||
			     scan->cll_state == CLS_CACHED) &&
			    cl_lock_ext_match(&scan->cll_descr, descr)) {
				struct osc_lock *olck;

				olck = osc_lock_at(scan);
				result = osc_lock_is_lockless(olck);
				break;
			}
		}
		spin_unlock(&hdr->coh_lock_guard);
	}
	return result;
}
#else
static int osc_page_protected(const struct lu_env *env,
			      const struct osc_page *opg,
			      enum cl_lock_mode mode, int unref)
{
	return 1;
}
#endif

=======
>>>>>>> v4.9.227
/*****************************************************************************
 *
 * Page operations.
 *
 */
<<<<<<< HEAD
static void osc_page_fini(const struct lu_env *env,
			  struct cl_page_slice *slice)
{
	struct osc_page *opg = cl2osc_page(slice);
	CDEBUG(D_TRACE, "%p\n", opg);
	LASSERT(opg->ops_lock == NULL);
}

static void osc_page_transfer_get(struct osc_page *opg, const char *label)
{
	struct cl_page *page = cl_page_top(opg->ops_cl.cpl_page);
=======
static void osc_page_transfer_get(struct osc_page *opg, const char *label)
{
	struct cl_page *page = opg->ops_cl.cpl_page;
>>>>>>> v4.9.227

	LASSERT(!opg->ops_transfer_pinned);
	cl_page_get(page);
	lu_ref_add_atomic(&page->cp_reference, label, page);
	opg->ops_transfer_pinned = 1;
}

static void osc_page_transfer_put(const struct lu_env *env,
				  struct osc_page *opg)
{
<<<<<<< HEAD
	struct cl_page *page = cl_page_top(opg->ops_cl.cpl_page);

	if (opg->ops_transfer_pinned) {
		lu_ref_del(&page->cp_reference, "transfer", page);
		opg->ops_transfer_pinned = 0;
=======
	struct cl_page *page = opg->ops_cl.cpl_page;

	if (opg->ops_transfer_pinned) {
		opg->ops_transfer_pinned = 0;
		lu_ref_del(&page->cp_reference, "transfer", page);
>>>>>>> v4.9.227
		cl_page_put(env, page);
	}
}

/**
 * This is called once for every page when it is submitted for a transfer
 * either opportunistic (osc_page_cache_add()), or immediate
 * (osc_page_submit()).
 */
static void osc_page_transfer_add(const struct lu_env *env,
				  struct osc_page *opg, enum cl_req_type crt)
{
	struct osc_object *obj = cl2osc(opg->ops_cl.cpl_obj);

<<<<<<< HEAD
	/* ops_lru and ops_inflight share the same field, so take it from LRU
	 * first and then use it as inflight. */
	osc_lru_del(osc_cli(obj), opg, false);
=======
	osc_lru_use(osc_cli(obj), opg);
>>>>>>> v4.9.227

	spin_lock(&obj->oo_seatbelt);
	list_add(&opg->ops_inflight, &obj->oo_inflight[crt]);
	opg->ops_submitter = current;
	spin_unlock(&obj->oo_seatbelt);
}

<<<<<<< HEAD
static int osc_page_cache_add(const struct lu_env *env,
			      const struct cl_page_slice *slice,
			      struct cl_io *io)
{
	struct osc_io   *oio = osc_env_io(env);
	struct osc_page *opg = cl2osc_page(slice);
	int result;

	LINVRNT(osc_page_protected(env, opg, CLM_WRITE, 0));

=======
int osc_page_cache_add(const struct lu_env *env,
		       const struct cl_page_slice *slice, struct cl_io *io)
{
	struct osc_page *opg = cl2osc_page(slice);
	int result;

>>>>>>> v4.9.227
	osc_page_transfer_get(opg, "transfer\0cache");
	result = osc_queue_async_io(env, io, opg);
	if (result != 0)
		osc_page_transfer_put(env, opg);
	else
		osc_page_transfer_add(env, opg, CRT_WRITE);

<<<<<<< HEAD
	/* for sync write, kernel will wait for this page to be flushed before
	 * osc_io_end() is called, so release it earlier.
	 * for mkwrite(), it's known there is no further pages. */
	if (cl_io_is_sync_write(io) || cl_io_is_mkwrite(io)) {
		if (oio->oi_active != NULL) {
			osc_extent_release(env, oio->oi_active);
			oio->oi_active = NULL;
		}
	}

=======
>>>>>>> v4.9.227
	return result;
}

void osc_index2policy(ldlm_policy_data_t *policy, const struct cl_object *obj,
		      pgoff_t start, pgoff_t end)
{
	memset(policy, 0, sizeof(*policy));
	policy->l_extent.start = cl_offset(obj, start);
<<<<<<< HEAD
	policy->l_extent.end   = cl_offset(obj, end + 1) - 1;
}

static int osc_page_addref_lock(const struct lu_env *env,
				struct osc_page *opg,
				struct cl_lock *lock)
{
	struct osc_lock *olock;
	int	      rc;

	LASSERT(opg->ops_lock == NULL);

	olock = osc_lock_at(lock);
	if (atomic_inc_return(&olock->ols_pageref) <= 0) {
		atomic_dec(&olock->ols_pageref);
		rc = -ENODATA;
	} else {
		cl_lock_get(lock);
		opg->ops_lock = lock;
		rc = 0;
	}
	return rc;
}

static void osc_page_putref_lock(const struct lu_env *env,
				 struct osc_page *opg)
{
	struct cl_lock  *lock = opg->ops_lock;
	struct osc_lock *olock;

	LASSERT(lock != NULL);
	olock = osc_lock_at(lock);

	atomic_dec(&olock->ols_pageref);
	opg->ops_lock = NULL;

	cl_lock_put(env, lock);
=======
	policy->l_extent.end = cl_offset(obj, end + 1) - 1;
>>>>>>> v4.9.227
}

static int osc_page_is_under_lock(const struct lu_env *env,
				  const struct cl_page_slice *slice,
<<<<<<< HEAD
				  struct cl_io *unused)
{
	struct cl_lock *lock;
	int	     result = -ENODATA;

	lock = cl_lock_at_page(env, slice->cpl_obj, slice->cpl_page,
			       NULL, 1, 0);
	if (lock != NULL) {
		if (osc_page_addref_lock(env, cl2osc_page(slice), lock) == 0)
			result = -EBUSY;
		cl_lock_put(env, lock);
=======
				  struct cl_io *unused, pgoff_t *max_index)
{
	struct osc_page *opg = cl2osc_page(slice);
	struct ldlm_lock *dlmlock;
	int result = -ENODATA;

	dlmlock = osc_dlmlock_at_pgoff(env, cl2osc(slice->cpl_obj),
				       osc_index(opg), 1, 0);
	if (dlmlock) {
		*max_index = cl_index(slice->cpl_obj,
				      dlmlock->l_policy_data.l_extent.end);
		LDLM_LOCK_PUT(dlmlock);
		result = 0;
>>>>>>> v4.9.227
	}
	return result;
}

<<<<<<< HEAD
static void osc_page_disown(const struct lu_env *env,
			    const struct cl_page_slice *slice,
			    struct cl_io *io)
{
	struct osc_page *opg = cl2osc_page(slice);

	if (unlikely(opg->ops_lock))
		osc_page_putref_lock(env, opg);
}

static void osc_page_completion_read(const struct lu_env *env,
				     const struct cl_page_slice *slice,
				     int ioret)
{
	struct osc_page   *opg = cl2osc_page(slice);
	struct osc_object *obj = cl2osc(opg->ops_cl.cpl_obj);

	if (likely(opg->ops_lock))
		osc_page_putref_lock(env, opg);
	osc_lru_add(osc_cli(obj), opg);
}

static void osc_page_completion_write(const struct lu_env *env,
				      const struct cl_page_slice *slice,
				      int ioret)
{
	struct osc_page   *opg = cl2osc_page(slice);
	struct osc_object *obj = cl2osc(slice->cpl_obj);

	osc_lru_add(osc_cli(obj), opg);
}

static int osc_page_fail(const struct lu_env *env,
			 const struct cl_page_slice *slice,
			 struct cl_io *unused)
{
	/*
	 * Cached read?
	 */
	LBUG();
	return 0;
}


=======
>>>>>>> v4.9.227
static const char *osc_list(struct list_head *head)
{
	return list_empty(head) ? "-" : "+";
}

static inline unsigned long osc_submit_duration(struct osc_page *opg)
{
	if (opg->ops_submit_time == 0)
		return 0;

	return (cfs_time_current() - opg->ops_submit_time);
}

static int osc_page_print(const struct lu_env *env,
			  const struct cl_page_slice *slice,
			  void *cookie, lu_printer_t printer)
{
<<<<<<< HEAD
	struct osc_page       *opg = cl2osc_page(slice);
	struct osc_async_page *oap = &opg->ops_oap;
	struct osc_object     *obj = cl2osc(slice->cpl_obj);
	struct client_obd     *cli = &osc_export(obj)->exp_obd->u.cli;

	return (*printer)(env, cookie, LUSTRE_OSC_NAME"-page@%p: "
			  "1< %#x %d %u %s %s > "
			  "2< %llu %u %u %#x %#x | %p %p %p > "
			  "3< %s %p %d %lu %d > "
			  "4< %d %d %d %lu %s | %s %s %s %s > "
			  "5< %s %s %s %s | %d %s | %d %s %s>\n",
			  opg,
=======
	struct osc_page *opg = cl2osc_page(slice);
	struct osc_async_page *oap = &opg->ops_oap;
	struct osc_object *obj = cl2osc(slice->cpl_obj);
	struct client_obd *cli = &osc_export(obj)->exp_obd->u.cli;

	return (*printer)(env, cookie, LUSTRE_OSC_NAME "-page@%p %lu: 1< %#x %d %u %s %s > 2< %llu %u %u %#x %#x | %p %p %p > 3< %s %p %d %lu %d > 4< %d %d %d %lu %s | %s %s %s %s > 5< %s %s %s %s | %d %s | %d %s %s>\n",
			  opg, osc_index(opg),
>>>>>>> v4.9.227
			  /* 1 */
			  oap->oap_magic, oap->oap_cmd,
			  oap->oap_interrupted,
			  osc_list(&oap->oap_pending_item),
			  osc_list(&oap->oap_rpc_item),
			  /* 2 */
			  oap->oap_obj_off, oap->oap_page_off, oap->oap_count,
			  oap->oap_async_flags, oap->oap_brw_flags,
			  oap->oap_request, oap->oap_cli, obj,
			  /* 3 */
			  osc_list(&opg->ops_inflight),
			  opg->ops_submitter, opg->ops_transfer_pinned,
			  osc_submit_duration(opg), opg->ops_srvlock,
			  /* 4 */
			  cli->cl_r_in_flight, cli->cl_w_in_flight,
			  cli->cl_max_rpcs_in_flight,
			  cli->cl_avail_grant,
			  osc_list(&cli->cl_cache_waiters),
			  osc_list(&cli->cl_loi_ready_list),
			  osc_list(&cli->cl_loi_hp_ready_list),
			  osc_list(&cli->cl_loi_write_list),
			  osc_list(&cli->cl_loi_read_list),
			  /* 5 */
			  osc_list(&obj->oo_ready_item),
			  osc_list(&obj->oo_hp_ready_item),
			  osc_list(&obj->oo_write_item),
			  osc_list(&obj->oo_read_item),
			  atomic_read(&obj->oo_nr_reads),
			  osc_list(&obj->oo_reading_exts),
			  atomic_read(&obj->oo_nr_writes),
			  osc_list(&obj->oo_hp_exts),
			  osc_list(&obj->oo_urgent_exts));
}

static void osc_page_delete(const struct lu_env *env,
			    const struct cl_page_slice *slice)
{
<<<<<<< HEAD
	struct osc_page   *opg = cl2osc_page(slice);
	struct osc_object *obj = cl2osc(opg->ops_cl.cpl_obj);
	int rc;

	LINVRNT(opg->ops_temp || osc_page_protected(env, opg, CLM_READ, 1));

=======
	struct osc_page *opg = cl2osc_page(slice);
	struct osc_object *obj = cl2osc(opg->ops_cl.cpl_obj);
	int rc;

>>>>>>> v4.9.227
	CDEBUG(D_TRACE, "%p\n", opg);
	osc_page_transfer_put(env, opg);
	rc = osc_teardown_async_page(env, obj, opg);
	if (rc) {
<<<<<<< HEAD
		CL_PAGE_DEBUG(D_ERROR, env, cl_page_top(slice->cpl_page),
=======
		CL_PAGE_DEBUG(D_ERROR, env, slice->cpl_page,
>>>>>>> v4.9.227
			      "Trying to teardown failed: %d\n", rc);
		LASSERT(0);
	}

	spin_lock(&obj->oo_seatbelt);
<<<<<<< HEAD
	if (opg->ops_submitter != NULL) {
=======
	if (opg->ops_submitter) {
>>>>>>> v4.9.227
		LASSERT(!list_empty(&opg->ops_inflight));
		list_del_init(&opg->ops_inflight);
		opg->ops_submitter = NULL;
	}
	spin_unlock(&obj->oo_seatbelt);

<<<<<<< HEAD
	osc_lru_del(osc_cli(obj), opg, true);
}

void osc_page_clip(const struct lu_env *env, const struct cl_page_slice *slice,
		   int from, int to)
{
	struct osc_page       *opg = cl2osc_page(slice);
	struct osc_async_page *oap = &opg->ops_oap;

	LINVRNT(osc_page_protected(env, opg, CLM_READ, 0));

	opg->ops_from = from;
	opg->ops_to   = to;
=======
	osc_lru_del(osc_cli(obj), opg);

	if (slice->cpl_page->cp_type == CPT_CACHEABLE) {
		void *value;

		spin_lock(&obj->oo_tree_lock);
		value = radix_tree_delete(&obj->oo_tree, osc_index(opg));
		if (value)
			--obj->oo_npages;
		spin_unlock(&obj->oo_tree_lock);

		LASSERT(ergo(value, value == opg));
	}
}

static void osc_page_clip(const struct lu_env *env,
			  const struct cl_page_slice *slice, int from, int to)
{
	struct osc_page *opg = cl2osc_page(slice);
	struct osc_async_page *oap = &opg->ops_oap;

	opg->ops_from = from;
	opg->ops_to = to;
>>>>>>> v4.9.227
	spin_lock(&oap->oap_lock);
	oap->oap_async_flags |= ASYNC_COUNT_STABLE;
	spin_unlock(&oap->oap_lock);
}

static int osc_page_cancel(const struct lu_env *env,
			   const struct cl_page_slice *slice)
{
	struct osc_page *opg = cl2osc_page(slice);
	int rc = 0;

<<<<<<< HEAD
	LINVRNT(osc_page_protected(env, opg, CLM_READ, 0));

	/* Check if the transferring against this page
	 * is completed, or not even queued. */
=======
	/* Check if the transferring against this page
	 * is completed, or not even queued.
	 */
>>>>>>> v4.9.227
	if (opg->ops_transfer_pinned)
		/* FIXME: may not be interrupted.. */
		rc = osc_cancel_async_page(env, opg);
	LASSERT(ergo(rc == 0, opg->ops_transfer_pinned == 0));
	return rc;
}

static int osc_page_flush(const struct lu_env *env,
			  const struct cl_page_slice *slice,
			  struct cl_io *io)
{
	struct osc_page *opg = cl2osc_page(slice);
<<<<<<< HEAD
	int rc = 0;
=======
	int rc;
>>>>>>> v4.9.227

	rc = osc_flush_async_page(env, io, opg);
	return rc;
}

static const struct cl_page_operations osc_page_ops = {
<<<<<<< HEAD
	.cpo_fini	  = osc_page_fini,
	.cpo_print	 = osc_page_print,
	.cpo_delete	= osc_page_delete,
	.cpo_is_under_lock = osc_page_is_under_lock,
	.cpo_disown	= osc_page_disown,
	.io = {
		[CRT_READ] = {
			.cpo_cache_add  = osc_page_fail,
			.cpo_completion = osc_page_completion_read
		},
		[CRT_WRITE] = {
			.cpo_cache_add  = osc_page_cache_add,
			.cpo_completion = osc_page_completion_write
		}
	},
=======
	.cpo_print	 = osc_page_print,
	.cpo_delete	= osc_page_delete,
	.cpo_is_under_lock = osc_page_is_under_lock,
>>>>>>> v4.9.227
	.cpo_clip	   = osc_page_clip,
	.cpo_cancel	 = osc_page_cancel,
	.cpo_flush	  = osc_page_flush
};

int osc_page_init(const struct lu_env *env, struct cl_object *obj,
<<<<<<< HEAD
		struct cl_page *page, struct page *vmpage)
{
	struct osc_object *osc = cl2osc(obj);
	struct osc_page   *opg = cl_object_page_slice(obj, page);
	int result;

	opg->ops_from = 0;
	opg->ops_to   = PAGE_CACHE_SIZE;

	result = osc_prep_async_page(osc, opg, vmpage,
					cl_offset(obj, page->cp_index));
	if (result == 0) {
		struct osc_io *oio = osc_env_io(env);
		opg->ops_srvlock = osc_io_srvlock(oio);
		cl_page_slice_add(page, &opg->ops_cl, obj,
				&osc_page_ops);
	}
	/*
	 * Cannot assert osc_page_protected() here as read-ahead
	 * creates temporary pages outside of a lock.
	 */
	/* ops_inflight and ops_lru are the same field, but it doesn't
	 * hurt to initialize it twice :-) */
=======
		  struct cl_page *page, pgoff_t index)
{
	struct osc_object *osc = cl2osc(obj);
	struct osc_page *opg = cl_object_page_slice(obj, page);
	int result;

	opg->ops_from = 0;
	opg->ops_to = PAGE_SIZE;

	result = osc_prep_async_page(osc, opg, page->cp_vmpage,
				     cl_offset(obj, index));
	if (result == 0) {
		struct osc_io *oio = osc_env_io(env);

		opg->ops_srvlock = osc_io_srvlock(oio);
		cl_page_slice_add(page, &opg->ops_cl, obj, index,
				  &osc_page_ops);
	}
	/* ops_inflight and ops_lru are the same field, but it doesn't
	 * hurt to initialize it twice :-)
	 */
>>>>>>> v4.9.227
	INIT_LIST_HEAD(&opg->ops_inflight);
	INIT_LIST_HEAD(&opg->ops_lru);

	/* reserve an LRU space for this page */
<<<<<<< HEAD
	if (page->cp_type == CPT_CACHEABLE && result == 0)
		result = osc_lru_reserve(env, osc, opg);
=======
	if (page->cp_type == CPT_CACHEABLE && result == 0) {
		result = osc_lru_reserve(env, osc, opg);
		if (result == 0) {
			spin_lock(&osc->oo_tree_lock);
			result = radix_tree_insert(&osc->oo_tree, index, opg);
			if (result == 0)
				++osc->oo_npages;
			spin_unlock(&osc->oo_tree_lock);
			LASSERT(result == 0);
		}
	}
>>>>>>> v4.9.227

	return result;
}

/**
 * Helper function called by osc_io_submit() for every page in an immediate
 * transfer (i.e., transferred synchronously).
 */
void osc_page_submit(const struct lu_env *env, struct osc_page *opg,
		     enum cl_req_type crt, int brw_flags)
{
	struct osc_async_page *oap = &opg->ops_oap;
<<<<<<< HEAD
	struct osc_object     *obj = oap->oap_obj;

	LINVRNT(osc_page_protected(env, opg,
				   crt == CRT_WRITE ? CLM_WRITE : CLM_READ, 1));

	LASSERTF(oap->oap_magic == OAP_MAGIC, "Bad oap magic: oap %p, "
		 "magic 0x%x\n", oap, oap->oap_magic);
	LASSERT(oap->oap_async_flags & ASYNC_READY);
	LASSERT(oap->oap_async_flags & ASYNC_COUNT_STABLE);

	oap->oap_cmd       = crt == CRT_WRITE ? OBD_BRW_WRITE : OBD_BRW_READ;
	oap->oap_page_off  = opg->ops_from;
	oap->oap_count     = opg->ops_to - opg->ops_from;
	oap->oap_brw_flags = OBD_BRW_SYNC | brw_flags;

	if (!client_is_remote(osc_export(obj)) &&
			capable(CFS_CAP_SYS_RESOURCE)) {
=======

	LASSERTF(oap->oap_magic == OAP_MAGIC, "Bad oap magic: oap %p, magic 0x%x\n",
		 oap, oap->oap_magic);
	LASSERT(oap->oap_async_flags & ASYNC_READY);
	LASSERT(oap->oap_async_flags & ASYNC_COUNT_STABLE);

	oap->oap_cmd = crt == CRT_WRITE ? OBD_BRW_WRITE : OBD_BRW_READ;
	oap->oap_page_off = opg->ops_from;
	oap->oap_count = opg->ops_to - opg->ops_from;
	oap->oap_brw_flags = brw_flags | OBD_BRW_SYNC;

	if (capable(CFS_CAP_SYS_RESOURCE)) {
>>>>>>> v4.9.227
		oap->oap_brw_flags |= OBD_BRW_NOQUOTA;
		oap->oap_cmd |= OBD_BRW_NOQUOTA;
	}

	opg->ops_submit_time = cfs_time_current();
	osc_page_transfer_get(opg, "transfer\0imm");
	osc_page_transfer_add(env, opg, crt);
}

/* --------------- LRU page management ------------------ */

/* OSC is a natural place to manage LRU pages as applications are specialized
 * to write OSC by OSC. Ideally, if one OSC is used more frequently it should
 * occupy more LRU slots. On the other hand, we should avoid using up all LRU
 * slots (client_obd::cl_lru_left) otherwise process has to be put into sleep
 * for free LRU slots - this will be very bad so the algorithm requires each
 * OSC to free slots voluntarily to maintain a reasonable number of free slots
 * at any time.
 */

static DECLARE_WAIT_QUEUE_HEAD(osc_lru_waitq);
<<<<<<< HEAD
static atomic_t osc_lru_waiters = ATOMIC_INIT(0);
/* LRU pages are freed in batch mode. OSC should at least free this
 * number of pages to avoid running out of LRU budget, and.. */
static const int lru_shrink_min = 2 << (20 - PAGE_CACHE_SHIFT);  /* 2M */
/* free this number at most otherwise it will take too long time to finish. */
static const int lru_shrink_max = 32 << (20 - PAGE_CACHE_SHIFT); /* 32M */
=======
/* LRU pages are freed in batch mode. OSC should at least free this
 * number of pages to avoid running out of LRU budget, and..
 */
static const int lru_shrink_min = 2 << (20 - PAGE_SHIFT);  /* 2M */
/* free this number at most otherwise it will take too long time to finish. */
static const int lru_shrink_max = 8 << (20 - PAGE_SHIFT); /* 8M */
>>>>>>> v4.9.227

/* Check if we can free LRU slots from this OSC. If there exists LRU waiters,
 * we should free slots aggressively. In this way, slots are freed in a steady
 * step to maintain fairness among OSCs.
 *
<<<<<<< HEAD
 * Return how many LRU pages should be freed. */
static int osc_cache_too_much(struct client_obd *cli)
{
	struct cl_client_cache *cache = cli->cl_cache;
	int pages = atomic_read(&cli->cl_lru_in_list) >> 1;

	if (atomic_read(&osc_lru_waiters) > 0 &&
	    atomic_read(cli->cl_lru_left) < lru_shrink_max)
		/* drop lru pages aggressively */
		return min(pages, lru_shrink_max);

	/* if it's going to run out LRU slots, we should free some, but not
	 * too much to maintain fairness among OSCs. */
	if (atomic_read(cli->cl_lru_left) < cache->ccc_lru_max >> 4) {
		unsigned long tmp;

		tmp = cache->ccc_lru_max / atomic_read(&cache->ccc_users);
		if (pages > tmp)
			return min(pages, lru_shrink_max);

		return pages > lru_shrink_min ? lru_shrink_min : 0;
	}
=======
 * Return how many LRU pages should be freed.
 */
static int osc_cache_too_much(struct client_obd *cli)
{
	struct cl_client_cache *cache = cli->cl_cache;
	long pages = atomic_long_read(&cli->cl_lru_in_list);
	unsigned long budget;

	budget = cache->ccc_lru_max / (atomic_read(&cache->ccc_users) - 2);

	/* if it's going to run out LRU slots, we should free some, but not
	 * too much to maintain fairness among OSCs.
	 */
	if (atomic_long_read(cli->cl_lru_left) < cache->ccc_lru_max >> 4) {
		if (pages >= budget)
			return lru_shrink_max;
		else if (pages >= budget / 2)
			return lru_shrink_min;
	} else if (pages >= budget * 2) {
		return lru_shrink_min;
	}
	return 0;
}

int lru_queue_work(const struct lu_env *env, void *data)
{
	struct client_obd *cli = data;

	CDEBUG(D_CACHE, "Run LRU work for client obd %p.\n", cli);

	if (osc_cache_too_much(cli))
		osc_lru_shrink(env, cli, lru_shrink_max, true);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
/* Return how many pages are not discarded in @pvec. */
static int discard_pagevec(const struct lu_env *env, struct cl_io *io,
			   struct cl_page **pvec, int max_index)
{
	int count;
	int i;

	for (count = 0, i = 0; i < max_index; i++) {
		struct cl_page *page = pvec[i];
		if (cl_page_own_try(env, io, page) == 0) {
			/* free LRU page only if nobody is using it.
			 * This check is necessary to avoid freeing the pages
			 * having already been removed from LRU and pinned
			 * for IO. */
			if (!cl_page_in_use(page)) {
				cl_page_unmap(env, io, page);
				cl_page_discard(env, io, page);
				++count;
			}
			cl_page_disown(env, io, page);
		}
		cl_page_put(env, page);
		pvec[i] = NULL;
	}
	return max_index - count;
=======
void osc_lru_add_batch(struct client_obd *cli, struct list_head *plist)
{
	LIST_HEAD(lru);
	struct osc_async_page *oap;
	long npages = 0;

	list_for_each_entry(oap, plist, oap_pending_item) {
		struct osc_page *opg = oap2osc_page(oap);

		if (!opg->ops_in_lru)
			continue;

		++npages;
		LASSERT(list_empty(&opg->ops_lru));
		list_add(&opg->ops_lru, &lru);
	}

	if (npages > 0) {
		spin_lock(&cli->cl_lru_list_lock);
		list_splice_tail(&lru, &cli->cl_lru_list);
		atomic_long_sub(npages, &cli->cl_lru_busy);
		atomic_long_add(npages, &cli->cl_lru_in_list);
		spin_unlock(&cli->cl_lru_list_lock);

		/* XXX: May set force to be true for better performance */
		if (osc_cache_too_much(cli))
			(void)ptlrpcd_queue_work(cli->cl_lru_work);
	}
}

static void __osc_lru_del(struct client_obd *cli, struct osc_page *opg)
{
	LASSERT(atomic_long_read(&cli->cl_lru_in_list) > 0);
	list_del_init(&opg->ops_lru);
	atomic_long_dec(&cli->cl_lru_in_list);
}

/**
 * Page is being destroyed. The page may be not in LRU list, if the transfer
 * has never finished(error occurred).
 */
static void osc_lru_del(struct client_obd *cli, struct osc_page *opg)
{
	if (opg->ops_in_lru) {
		spin_lock(&cli->cl_lru_list_lock);
		if (!list_empty(&opg->ops_lru)) {
			__osc_lru_del(cli, opg);
		} else {
			LASSERT(atomic_long_read(&cli->cl_lru_busy) > 0);
			atomic_long_dec(&cli->cl_lru_busy);
		}
		spin_unlock(&cli->cl_lru_list_lock);

		atomic_long_inc(cli->cl_lru_left);
		/* this is a great place to release more LRU pages if
		 * this osc occupies too many LRU pages and kernel is
		 * stealing one of them.
		 */
		if (!memory_pressure_get())
			(void)ptlrpcd_queue_work(cli->cl_lru_work);
		wake_up(&osc_lru_waitq);
	} else {
		LASSERT(list_empty(&opg->ops_lru));
	}
}

/**
 * Delete page from LRUlist for redirty.
 */
static void osc_lru_use(struct client_obd *cli, struct osc_page *opg)
{
	/* If page is being transferred for the first time,
	 * ops_lru should be empty
	 */
	if (opg->ops_in_lru && !list_empty(&opg->ops_lru)) {
		spin_lock(&cli->cl_lru_list_lock);
		__osc_lru_del(cli, opg);
		spin_unlock(&cli->cl_lru_list_lock);
		atomic_long_inc(&cli->cl_lru_busy);
	}
}

static void discard_pagevec(const struct lu_env *env, struct cl_io *io,
			    struct cl_page **pvec, int max_index)
{
	int i;

	for (i = 0; i < max_index; i++) {
		struct cl_page *page = pvec[i];

		LASSERT(cl_page_is_owned(page, io));
		cl_page_discard(env, io, page);
		cl_page_disown(env, io, page);
		cl_page_put(env, page);

		pvec[i] = NULL;
	}
}

/**
 * Check if a cl_page can be released, i.e, it's not being used.
 *
 * If unstable account is turned on, bulk transfer may hold one refcount
 * for recovery so we need to check vmpage refcount as well; otherwise,
 * even we can destroy cl_page but the corresponding vmpage can't be reused.
 */
static inline bool lru_page_busy(struct client_obd *cli, struct cl_page *page)
{
	if (cl_page_in_use_noref(page))
		return true;

	if (cli->cl_cache->ccc_unstable_check) {
		struct page *vmpage = cl_page_vmpage(page);

		/* vmpage have two known users: cl_page and VM page cache */
		if (page_count(vmpage) - page_mapcount(vmpage) > 2)
			return true;
	}
	return false;
>>>>>>> v4.9.227
}

/**
 * Drop @target of pages from LRU at most.
 */
<<<<<<< HEAD
int osc_lru_shrink(struct client_obd *cli, int target)
{
	struct cl_env_nest nest;
	struct lu_env *env;
=======
long osc_lru_shrink(const struct lu_env *env, struct client_obd *cli,
		    long target, bool force)
{
>>>>>>> v4.9.227
	struct cl_io *io;
	struct cl_object *clobj = NULL;
	struct cl_page **pvec;
	struct osc_page *opg;
	int maxscan = 0;
<<<<<<< HEAD
	int count = 0;
	int index = 0;
	int rc = 0;

	LASSERT(atomic_read(&cli->cl_lru_in_list) >= 0);
	if (atomic_read(&cli->cl_lru_in_list) == 0 || target <= 0)
		return 0;

	env = cl_env_nested_get(&nest);
	if (IS_ERR(env))
		return PTR_ERR(env);

	pvec = osc_env_info(env)->oti_pvec;
	io = &osc_env_info(env)->oti_io;

	client_obd_list_lock(&cli->cl_lru_list_lock);
	atomic_inc(&cli->cl_lru_shrinkers);
	maxscan = min(target << 1, atomic_read(&cli->cl_lru_in_list));
	while (!list_empty(&cli->cl_lru_list)) {
		struct cl_page *page;
=======
	long count = 0;
	int index = 0;
	int rc = 0;

	LASSERT(atomic_long_read(&cli->cl_lru_in_list) >= 0);
	if (atomic_long_read(&cli->cl_lru_in_list) == 0 || target <= 0)
		return 0;

	if (!force) {
		if (atomic_read(&cli->cl_lru_shrinkers) > 0)
			return -EBUSY;

		if (atomic_inc_return(&cli->cl_lru_shrinkers) > 1) {
			atomic_dec(&cli->cl_lru_shrinkers);
			return -EBUSY;
		}
	} else {
		atomic_inc(&cli->cl_lru_shrinkers);
	}

	pvec = (struct cl_page **)osc_env_info(env)->oti_pvec;
	io = &osc_env_info(env)->oti_io;

	spin_lock(&cli->cl_lru_list_lock);
	maxscan = min(target << 1, atomic_long_read(&cli->cl_lru_in_list));
	while (!list_empty(&cli->cl_lru_list)) {
		struct cl_page *page;
		bool will_free = false;
>>>>>>> v4.9.227

		if (--maxscan < 0)
			break;

		opg = list_entry(cli->cl_lru_list.next, struct osc_page,
<<<<<<< HEAD
				     ops_lru);
		page = cl_page_top(opg->ops_cl.cpl_page);
		if (cl_page_in_use_noref(page)) {
=======
				 ops_lru);
		page = opg->ops_cl.cpl_page;
		if (lru_page_busy(cli, page)) {
>>>>>>> v4.9.227
			list_move_tail(&opg->ops_lru, &cli->cl_lru_list);
			continue;
		}

<<<<<<< HEAD
		LASSERT(page->cp_obj != NULL);
=======
		LASSERT(page->cp_obj);
>>>>>>> v4.9.227
		if (clobj != page->cp_obj) {
			struct cl_object *tmp = page->cp_obj;

			cl_object_get(tmp);
<<<<<<< HEAD
			client_obd_list_unlock(&cli->cl_lru_list_lock);

			if (clobj != NULL) {
				count -= discard_pagevec(env, io, pvec, index);
=======
			spin_unlock(&cli->cl_lru_list_lock);

			if (clobj) {
				discard_pagevec(env, io, pvec, index);
>>>>>>> v4.9.227
				index = 0;

				cl_io_fini(env, io);
				cl_object_put(env, clobj);
				clobj = NULL;
			}

			clobj = tmp;
			io->ci_obj = clobj;
			io->ci_ignore_layout = 1;
			rc = cl_io_init(env, io, CIT_MISC, clobj);

<<<<<<< HEAD
			client_obd_list_lock(&cli->cl_lru_list_lock);
=======
			spin_lock(&cli->cl_lru_list_lock);
>>>>>>> v4.9.227

			if (rc != 0)
				break;

			++maxscan;
			continue;
		}

<<<<<<< HEAD
		/* move this page to the end of list as it will be discarded
		 * soon. The page will be finally removed from LRU list in
		 * osc_page_delete().  */
		list_move_tail(&opg->ops_lru, &cli->cl_lru_list);

		/* it's okay to grab a refcount here w/o holding lock because
		 * it has to grab cl_lru_list_lock to delete the page. */
		cl_page_get(page);
		pvec[index++] = page;
		if (++count >= target)
			break;

		if (unlikely(index == OTI_PVEC_SIZE)) {
			client_obd_list_unlock(&cli->cl_lru_list_lock);
			count -= discard_pagevec(env, io, pvec, index);
			index = 0;

			client_obd_list_lock(&cli->cl_lru_list_lock);
		}
	}
	client_obd_list_unlock(&cli->cl_lru_list_lock);

	if (clobj != NULL) {
		count -= discard_pagevec(env, io, pvec, index);
=======
		if (cl_page_own_try(env, io, page) == 0) {
			if (!lru_page_busy(cli, page)) {
				/* remove it from lru list earlier to avoid
				 * lock contention
				 */
				__osc_lru_del(cli, opg);
				opg->ops_in_lru = 0; /* will be discarded */

				cl_page_get(page);
				will_free = true;
			} else {
				cl_page_disown(env, io, page);
			}
		}

		if (!will_free) {
			list_move_tail(&opg->ops_lru, &cli->cl_lru_list);
			continue;
		}

		/* Don't discard and free the page with cl_lru_list held */
		pvec[index++] = page;
		if (unlikely(index == OTI_PVEC_SIZE)) {
			spin_unlock(&cli->cl_lru_list_lock);
			discard_pagevec(env, io, pvec, index);
			index = 0;

			spin_lock(&cli->cl_lru_list_lock);
		}

		if (++count >= target)
			break;
	}
	spin_unlock(&cli->cl_lru_list_lock);

	if (clobj) {
		discard_pagevec(env, io, pvec, index);
>>>>>>> v4.9.227

		cl_io_fini(env, io);
		cl_object_put(env, clobj);
	}
<<<<<<< HEAD
	cl_env_nested_put(&nest, env);

	atomic_dec(&cli->cl_lru_shrinkers);
	return count > 0 ? count : rc;
}

static void osc_lru_add(struct client_obd *cli, struct osc_page *opg)
{
	bool wakeup = false;

	if (!opg->ops_in_lru)
		return;

	atomic_dec(&cli->cl_lru_busy);
	client_obd_list_lock(&cli->cl_lru_list_lock);
	if (list_empty(&opg->ops_lru)) {
		list_move_tail(&opg->ops_lru, &cli->cl_lru_list);
		atomic_inc_return(&cli->cl_lru_in_list);
		wakeup = atomic_read(&osc_lru_waiters) > 0;
	}
	client_obd_list_unlock(&cli->cl_lru_list_lock);

	if (wakeup) {
		osc_lru_shrink(cli, osc_cache_too_much(cli));
		wake_up_all(&osc_lru_waitq);
	}
}

/* delete page from LRUlist. The page can be deleted from LRUlist for two
 * reasons: redirtied or deleted from page cache. */
static void osc_lru_del(struct client_obd *cli, struct osc_page *opg, bool del)
{
	if (opg->ops_in_lru) {
		client_obd_list_lock(&cli->cl_lru_list_lock);
		if (!list_empty(&opg->ops_lru)) {
			LASSERT(atomic_read(&cli->cl_lru_in_list) > 0);
			list_del_init(&opg->ops_lru);
			atomic_dec(&cli->cl_lru_in_list);
			if (!del)
				atomic_inc(&cli->cl_lru_busy);
		} else if (del) {
			LASSERT(atomic_read(&cli->cl_lru_busy) > 0);
			atomic_dec(&cli->cl_lru_busy);
		}
		client_obd_list_unlock(&cli->cl_lru_list_lock);
		if (del) {
			atomic_inc(cli->cl_lru_left);
			/* this is a great place to release more LRU pages if
			 * this osc occupies too many LRU pages and kernel is
			 * stealing one of them.
			 * cl_lru_shrinkers is to avoid recursive call in case
			 * we're already in the context of osc_lru_shrink(). */
			if (atomic_read(&cli->cl_lru_shrinkers) == 0 &&
			    !memory_pressure_get())
				osc_lru_shrink(cli, osc_cache_too_much(cli));
			wake_up(&osc_lru_waitq);
		}
	} else {
		LASSERT(list_empty(&opg->ops_lru));
	}
}

static inline int max_to_shrink(struct client_obd *cli)
{
	return min(atomic_read(&cli->cl_lru_in_list) >> 1, lru_shrink_max);
}

static int osc_lru_reclaim(struct client_obd *cli)
{
	struct cl_client_cache *cache = cli->cl_cache;
	int max_scans;
	int rc;

	LASSERT(cache != NULL);
	LASSERT(!list_empty(&cache->ccc_lru));

	rc = osc_lru_shrink(cli, lru_shrink_min);
	if (rc != 0) {
		CDEBUG(D_CACHE, "%s: Free %d pages from own LRU: %p.\n",
			cli->cl_import->imp_obd->obd_name, rc, cli);
		return rc;
	}

	CDEBUG(D_CACHE, "%s: cli %p no free slots, pages: %d, busy: %d.\n",
		cli->cl_import->imp_obd->obd_name, cli,
		atomic_read(&cli->cl_lru_in_list),
		atomic_read(&cli->cl_lru_busy));

	/* Reclaim LRU slots from other client_obd as it can't free enough
	 * from its own. This should rarely happen. */
	spin_lock(&cache->ccc_lru_lock);
	cache->ccc_lru_shrinkers++;
	list_move_tail(&cli->cl_lru_osc, &cache->ccc_lru);

	max_scans = atomic_read(&cache->ccc_users);
	while (--max_scans > 0 && !list_empty(&cache->ccc_lru)) {
		cli = list_entry(cache->ccc_lru.next, struct client_obd,
					cl_lru_osc);

		CDEBUG(D_CACHE, "%s: cli %p LRU pages: %d, busy: %d.\n",
			cli->cl_import->imp_obd->obd_name, cli,
			atomic_read(&cli->cl_lru_in_list),
			atomic_read(&cli->cl_lru_busy));

		list_move_tail(&cli->cl_lru_osc, &cache->ccc_lru);
		if (atomic_read(&cli->cl_lru_in_list) > 0) {
			spin_unlock(&cache->ccc_lru_lock);

			rc = osc_lru_shrink(cli, max_to_shrink(cli));
=======

	atomic_dec(&cli->cl_lru_shrinkers);
	if (count > 0) {
		atomic_long_add(count, cli->cl_lru_left);
		wake_up_all(&osc_lru_waitq);
	}
	return count > 0 ? count : rc;
}

long osc_lru_reclaim(struct client_obd *cli)
{
	struct cl_env_nest nest;
	struct lu_env *env;
	struct cl_client_cache *cache = cli->cl_cache;
	int max_scans;
	long rc = 0;

	LASSERT(cache);

	env = cl_env_nested_get(&nest);
	if (IS_ERR(env))
		return 0;

	rc = osc_lru_shrink(env, cli, osc_cache_too_much(cli), false);
	if (rc != 0) {
		if (rc == -EBUSY)
			rc = 0;

		CDEBUG(D_CACHE, "%s: Free %ld pages from own LRU: %p.\n",
		       cli->cl_import->imp_obd->obd_name, rc, cli);
		goto out;
	}

	CDEBUG(D_CACHE, "%s: cli %p no free slots, pages: %ld, busy: %ld.\n",
	       cli->cl_import->imp_obd->obd_name, cli,
	       atomic_long_read(&cli->cl_lru_in_list),
	       atomic_long_read(&cli->cl_lru_busy));

	/* Reclaim LRU slots from other client_obd as it can't free enough
	 * from its own. This should rarely happen.
	 */
	spin_lock(&cache->ccc_lru_lock);
	LASSERT(!list_empty(&cache->ccc_lru));

	cache->ccc_lru_shrinkers++;
	list_move_tail(&cli->cl_lru_osc, &cache->ccc_lru);

	max_scans = atomic_read(&cache->ccc_users) - 2;
	while (--max_scans > 0 && !list_empty(&cache->ccc_lru)) {
		cli = list_entry(cache->ccc_lru.next, struct client_obd,
				 cl_lru_osc);

		CDEBUG(D_CACHE, "%s: cli %p LRU pages: %ld, busy: %ld.\n",
		       cli->cl_import->imp_obd->obd_name, cli,
		       atomic_long_read(&cli->cl_lru_in_list),
		       atomic_long_read(&cli->cl_lru_busy));

		list_move_tail(&cli->cl_lru_osc, &cache->ccc_lru);
		if (osc_cache_too_much(cli) > 0) {
			spin_unlock(&cache->ccc_lru_lock);

			rc = osc_lru_shrink(env, cli, osc_cache_too_much(cli),
					    true);
>>>>>>> v4.9.227
			spin_lock(&cache->ccc_lru_lock);
			if (rc != 0)
				break;
		}
	}
	spin_unlock(&cache->ccc_lru_lock);

<<<<<<< HEAD
	CDEBUG(D_CACHE, "%s: cli %p freed %d pages.\n",
		cli->cl_import->imp_obd->obd_name, cli, rc);
	return rc;
}

=======
out:
	cl_env_nested_put(&nest, env);
	CDEBUG(D_CACHE, "%s: cli %p freed %ld pages.\n",
	       cli->cl_import->imp_obd->obd_name, cli, rc);
	return rc;
}

/**
 * osc_lru_reserve() is called to reserve an LRU slot for a cl_page.
 *
 * Usually the LRU slots are reserved in osc_io_iter_rw_init().
 * Only in the case that the LRU slots are in extreme shortage, it should
 * have reserved enough slots for an IO.
 */
>>>>>>> v4.9.227
static int osc_lru_reserve(const struct lu_env *env, struct osc_object *obj,
			   struct osc_page *opg)
{
	struct l_wait_info lwi = LWI_INTR(LWI_ON_SIGNAL_NOOP, NULL);
<<<<<<< HEAD
	struct client_obd *cli = osc_cli(obj);
	int rc = 0;

	if (cli->cl_cache == NULL) /* shall not be in LRU */
		return 0;

	LASSERT(atomic_read(cli->cl_lru_left) >= 0);
	while (!atomic_add_unless(cli->cl_lru_left, -1, 0)) {
		int gen;

=======
	struct osc_io *oio = osc_env_io(env);
	struct client_obd *cli = osc_cli(obj);
	int rc = 0;

	if (!cli->cl_cache) /* shall not be in LRU */
		return 0;

	if (oio->oi_lru_reserved > 0) {
		--oio->oi_lru_reserved;
		goto out;
	}

	LASSERT(atomic_long_read(cli->cl_lru_left) >= 0);
	while (!atomic_long_add_unless(cli->cl_lru_left, -1, 0)) {
>>>>>>> v4.9.227
		/* run out of LRU spaces, try to drop some by itself */
		rc = osc_lru_reclaim(cli);
		if (rc < 0)
			break;
		if (rc > 0)
			continue;

		cond_resched();

<<<<<<< HEAD
		/* slowest case, all of caching pages are busy, notifying
		 * other OSCs that we're lack of LRU slots. */
		atomic_inc(&osc_lru_waiters);

		gen = atomic_read(&cli->cl_lru_in_list);
		rc = l_wait_event(osc_lru_waitq,
				atomic_read(cli->cl_lru_left) > 0 ||
				(atomic_read(&cli->cl_lru_in_list) > 0 &&
				 gen != atomic_read(&cli->cl_lru_in_list)),
				&lwi);

		atomic_dec(&osc_lru_waiters);
=======
		rc = l_wait_event(osc_lru_waitq,
				  atomic_long_read(cli->cl_lru_left) > 0,
				  &lwi);

>>>>>>> v4.9.227
		if (rc < 0)
			break;
	}

<<<<<<< HEAD
	if (rc >= 0) {
		atomic_inc(&cli->cl_lru_busy);
=======
out:
	if (rc >= 0) {
		atomic_long_inc(&cli->cl_lru_busy);
>>>>>>> v4.9.227
		opg->ops_in_lru = 1;
		rc = 0;
	}

	return rc;
}

<<<<<<< HEAD
=======
/**
 * Atomic operations are expensive. We accumulate the accounting for the
 * same page pgdat to get better performance.
 * In practice this can work pretty good because the pages in the same RPC
 * are likely from the same page zone.
 */
static inline void unstable_page_accounting(struct ptlrpc_bulk_desc *desc,
					    int factor)
{
	int page_count = desc->bd_iov_count;
	pg_data_t *last = NULL;
	int count = 0;
	int i;

	for (i = 0; i < page_count; i++) {
		pg_data_t *pgdat = page_pgdat(desc->bd_iov[i].bv_page);

		if (likely(pgdat == last)) {
			++count;
			continue;
		}

		if (count > 0) {
			mod_node_page_state(pgdat, NR_UNSTABLE_NFS,
					    factor * count);
			count = 0;
		}
		last = pgdat;
		++count;
	}
	if (count > 0)
		mod_node_page_state(last, NR_UNSTABLE_NFS, factor * count);
}

static inline void add_unstable_page_accounting(struct ptlrpc_bulk_desc *desc)
{
	unstable_page_accounting(desc, 1);
}

static inline void dec_unstable_page_accounting(struct ptlrpc_bulk_desc *desc)
{
	unstable_page_accounting(desc, -1);
}

/**
 * Performs "unstable" page accounting. This function balances the
 * increment operations performed in osc_inc_unstable_pages. It is
 * registered as the RPC request callback, and is executed when the
 * bulk RPC is committed on the server. Thus at this point, the pages
 * involved in the bulk transfer are no longer considered unstable.
 *
 * If this function is called, the request should have been committed
 * or req:rq_unstable must have been set; it implies that the unstable
 * statistic have been added.
 */
void osc_dec_unstable_pages(struct ptlrpc_request *req)
{
	struct client_obd *cli = &req->rq_import->imp_obd->u.cli;
	struct ptlrpc_bulk_desc *desc = req->rq_bulk;
	int page_count = desc->bd_iov_count;
	long unstable_count;

	LASSERT(page_count >= 0);
	dec_unstable_page_accounting(desc);

	unstable_count = atomic_long_sub_return(page_count,
						&cli->cl_unstable_count);
	LASSERT(unstable_count >= 0);

	unstable_count = atomic_long_sub_return(page_count,
						&cli->cl_cache->ccc_unstable_nr);
	LASSERT(unstable_count >= 0);
	if (!unstable_count)
		wake_up_all(&cli->cl_cache->ccc_unstable_waitq);

	if (osc_cache_too_much(cli))
		(void)ptlrpcd_queue_work(cli->cl_lru_work);
}

/**
 * "unstable" page accounting. See: osc_dec_unstable_pages.
 */
void osc_inc_unstable_pages(struct ptlrpc_request *req)
{
	struct client_obd *cli  = &req->rq_import->imp_obd->u.cli;
	struct ptlrpc_bulk_desc *desc = req->rq_bulk;
	long page_count = desc->bd_iov_count;

	/* No unstable page tracking */
	if (!cli->cl_cache || !cli->cl_cache->ccc_unstable_check)
		return;

	add_unstable_page_accounting(desc);
	atomic_long_add(page_count, &cli->cl_unstable_count);
	atomic_long_add(page_count, &cli->cl_cache->ccc_unstable_nr);

	/*
	 * If the request has already been committed (i.e. brw_commit
	 * called via rq_commit_cb), we need to undo the unstable page
	 * increments we just performed because rq_commit_cb wont be
	 * called again.
	 */
	spin_lock(&req->rq_lock);
	if (unlikely(req->rq_committed)) {
		spin_unlock(&req->rq_lock);

		osc_dec_unstable_pages(req);
	} else {
		req->rq_unstable = 1;
		spin_unlock(&req->rq_lock);
	}
}

/**
 * Check if it piggybacks SOFT_SYNC flag to OST from this OSC.
 * This function will be called by every BRW RPC so it's critical
 * to make this function fast.
 */
bool osc_over_unstable_soft_limit(struct client_obd *cli)
{
	long unstable_nr, osc_unstable_count;

	/* Can't check cli->cl_unstable_count, therefore, no soft limit */
	if (!cli->cl_cache || !cli->cl_cache->ccc_unstable_check)
		return false;

	osc_unstable_count = atomic_long_read(&cli->cl_unstable_count);
	unstable_nr = atomic_long_read(&cli->cl_cache->ccc_unstable_nr);

	CDEBUG(D_CACHE,
	       "%s: cli: %p unstable pages: %lu, osc unstable pages: %lu\n",
	       cli->cl_import->imp_obd->obd_name, cli,
	       unstable_nr, osc_unstable_count);

	/*
	 * If the LRU slots are in shortage - 25% remaining AND this OSC
	 * has one full RPC window of unstable pages, it's a good chance
	 * to piggyback a SOFT_SYNC flag.
	 * Please notice that the OST won't take immediate response for the
	 * SOFT_SYNC request so active OSCs will have more chance to carry
	 * the flag, this is reasonable.
	 */
	return unstable_nr > cli->cl_cache->ccc_lru_max >> 2 &&
	       osc_unstable_count > cli->cl_max_pages_per_rpc *
				    cli->cl_max_rpcs_in_flight;
}

>>>>>>> v4.9.227
/** @} osc */
