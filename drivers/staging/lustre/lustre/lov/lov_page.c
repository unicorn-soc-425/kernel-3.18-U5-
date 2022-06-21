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
 * Implementation of cl_page for LOV layer.
 *
 *   Author: Nikita Danilov <nikita.danilov@sun.com>
<<<<<<< HEAD
=======
 *   Author: Jinshan Xiong <jinshan.xiong@intel.com>
>>>>>>> v4.9.227
 */

#define DEBUG_SUBSYSTEM S_LOV

#include "lov_cl_internal.h"

/** \addtogroup lov
 *  @{
 */

/*****************************************************************************
 *
 * Lov page operations.
 *
 */

<<<<<<< HEAD
static int lov_page_invariant(const struct cl_page_slice *slice)
{
	const struct cl_page  *page = slice->cpl_page;
	const struct cl_page  *sub  = lov_sub_page(slice);

	return ergo(sub != NULL,
		    page->cp_child == sub &&
		    sub->cp_parent == page &&
		    page->cp_state == sub->cp_state);
}

static void lov_page_fini(const struct lu_env *env,
			  struct cl_page_slice *slice)
{
	struct cl_page  *sub = lov_sub_page(slice);

	LINVRNT(lov_page_invariant(slice));

	if (sub != NULL) {
		LASSERT(sub->cp_state == CPS_FREEING);
		lu_ref_del(&sub->cp_reference, "lov", sub->cp_parent);
		sub->cp_parent = NULL;
		slice->cpl_page->cp_child = NULL;
		cl_page_put(env, sub);
	}
}

static int lov_page_own(const struct lu_env *env,
			const struct cl_page_slice *slice, struct cl_io *io,
			int nonblock)
{
	struct lov_io     *lio = lov_env_io(env);
	struct lov_io_sub *sub;

	LINVRNT(lov_page_invariant(slice));
	LINVRNT(!cl2lov_page(slice)->lps_invalid);

	sub = lov_page_subio(env, lio, slice);
	if (!IS_ERR(sub)) {
		lov_sub_page(slice)->cp_owner = sub->sub_io;
		lov_sub_put(sub);
	} else
		LBUG(); /* Arrgh */
	return 0;
}

static void lov_page_assume(const struct lu_env *env,
			    const struct cl_page_slice *slice, struct cl_io *io)
{
	lov_page_own(env, slice, io, 0);
}

static int lov_page_cache_add(const struct lu_env *env,
			      const struct cl_page_slice *slice,
			      struct cl_io *io)
{
	struct lov_io     *lio = lov_env_io(env);
	struct lov_io_sub *sub;
	int rc = 0;

	LINVRNT(lov_page_invariant(slice));
	LINVRNT(!cl2lov_page(slice)->lps_invalid);

	sub = lov_page_subio(env, lio, slice);
	if (!IS_ERR(sub)) {
		rc = cl_page_cache_add(sub->sub_env, sub->sub_io,
				       slice->cpl_page->cp_child, CRT_WRITE);
		lov_sub_put(sub);
	} else {
		rc = PTR_ERR(sub);
		CL_PAGE_DEBUG(D_ERROR, env, slice->cpl_page, "rc = %d\n", rc);
	}
	return rc;
}

static int lov_page_print(const struct lu_env *env,
			  const struct cl_page_slice *slice,
			  void *cookie, lu_printer_t printer)
{
	struct lov_page *lp = cl2lov_page(slice);

	return (*printer)(env, cookie, LUSTRE_LOV_NAME"-page@%p\n", lp);
}

static const struct cl_page_operations lov_page_ops = {
	.cpo_fini   = lov_page_fini,
	.cpo_own    = lov_page_own,
	.cpo_assume = lov_page_assume,
	.io = {
		[CRT_WRITE] = {
			.cpo_cache_add = lov_page_cache_add
		}
	},
	.cpo_print  = lov_page_print
};

static void lov_empty_page_fini(const struct lu_env *env,
				struct cl_page_slice *slice)
{
	LASSERT(slice->cpl_page->cp_child == NULL);
}

int lov_page_init_raid0(const struct lu_env *env, struct cl_object *obj,
			struct cl_page *page, struct page *vmpage)
=======
/**
 * Adjust the stripe index by layout of raid0. @max_index is the maximum
 * page index covered by an underlying DLM lock.
 * This function converts max_index from stripe level to file level, and make
 * sure it's not beyond one stripe.
 */
static int lov_raid0_page_is_under_lock(const struct lu_env *env,
					const struct cl_page_slice *slice,
					struct cl_io *unused,
					pgoff_t *max_index)
{
	struct lov_object *loo = cl2lov(slice->cpl_obj);
	struct lov_layout_raid0 *r0 = lov_r0(loo);
	pgoff_t index = *max_index;
	unsigned int pps; /* pages per stripe */

	CDEBUG(D_READA, DFID "*max_index = %lu, nr = %d\n",
	       PFID(lu_object_fid(lov2lu(loo))), index, r0->lo_nr);

	if (index == 0) /* the page is not covered by any lock */
		return 0;

	if (r0->lo_nr == 1) /* single stripe file */
		return 0;

	/* max_index is stripe level, convert it into file level */
	if (index != CL_PAGE_EOF) {
		int stripeno = lov_page_stripe(slice->cpl_page);
		*max_index = lov_stripe_pgoff(loo->lo_lsm, index, stripeno);
	}

	/* calculate the end of current stripe */
	pps = loo->lo_lsm->lsm_stripe_size >> PAGE_SHIFT;
	index = slice->cpl_index + pps - slice->cpl_index % pps - 1;

	CDEBUG(D_READA, DFID "*max_index = %lu, index = %lu, pps = %u, stripe_size = %u, stripe no = %u, page index = %lu\n",
	       PFID(lu_object_fid(lov2lu(loo))), *max_index, index, pps,
	       loo->lo_lsm->lsm_stripe_size, lov_page_stripe(slice->cpl_page),
	       slice->cpl_index);

	/* never exceed the end of the stripe */
	*max_index = min_t(pgoff_t, *max_index, index);
	return 0;
}

static int lov_raid0_page_print(const struct lu_env *env,
				const struct cl_page_slice *slice,
				void *cookie, lu_printer_t printer)
{
	struct lov_page *lp = cl2lov_page(slice);

	return (*printer)(env, cookie, LUSTRE_LOV_NAME "-page@%p, raid0\n", lp);
}

static const struct cl_page_operations lov_raid0_page_ops = {
	.cpo_is_under_lock = lov_raid0_page_is_under_lock,
	.cpo_print  = lov_raid0_page_print
};

int lov_page_init_raid0(const struct lu_env *env, struct cl_object *obj,
			struct cl_page *page, pgoff_t index)
>>>>>>> v4.9.227
{
	struct lov_object *loo = cl2lov(obj);
	struct lov_layout_raid0 *r0 = lov_r0(loo);
	struct lov_io     *lio = lov_env_io(env);
<<<<<<< HEAD
	struct cl_page    *subpage;
	struct cl_object  *subobj;
=======
	struct cl_object  *subobj;
	struct cl_object  *o;
>>>>>>> v4.9.227
	struct lov_io_sub *sub;
	struct lov_page   *lpg = cl_object_page_slice(obj, page);
	loff_t	     offset;
	u64	    suboff;
	int		stripe;
	int		rc;

<<<<<<< HEAD
	offset = cl_offset(obj, page->cp_index);
	stripe = lov_stripe_number(loo->lo_lsm, offset);
	LASSERT(stripe < r0->lo_nr);
	rc = lov_stripe_offset(loo->lo_lsm, offset, stripe,
				   &suboff);
	LASSERT(rc == 0);

	lpg->lps_invalid = 1;
	cl_page_slice_add(page, &lpg->lps_cl, obj, &lov_page_ops);

	sub = lov_sub_get(env, lio, stripe);
	if (IS_ERR(sub)) {
		rc = PTR_ERR(sub);
		goto out;
	}

	subobj = lovsub2cl(r0->lo_sub[stripe]);
	subpage = cl_page_find_sub(sub->sub_env, subobj,
				   cl_index(subobj, suboff), vmpage, page);
	lov_sub_put(sub);
	if (IS_ERR(subpage)) {
		rc = PTR_ERR(subpage);
		goto out;
	}

	if (likely(subpage->cp_parent == page)) {
		lu_ref_add(&subpage->cp_reference, "lov", page);
		lpg->lps_invalid = 0;
		rc = 0;
	} else {
		CL_PAGE_DEBUG(D_ERROR, env, page, "parent page\n");
		CL_PAGE_DEBUG(D_ERROR, env, subpage, "child page\n");
		LASSERT(0);
	}

out:
	return rc;
}


static const struct cl_page_operations lov_empty_page_ops = {
	.cpo_fini   = lov_empty_page_fini,
	.cpo_print  = lov_page_print
};

int lov_page_init_empty(const struct lu_env *env, struct cl_object *obj,
			struct cl_page *page, struct page *vmpage)
=======
	offset = cl_offset(obj, index);
	stripe = lov_stripe_number(loo->lo_lsm, offset);
	LASSERT(stripe < r0->lo_nr);
	rc = lov_stripe_offset(loo->lo_lsm, offset, stripe, &suboff);
	LASSERT(rc == 0);

	lpg->lps_stripe = stripe;
	cl_page_slice_add(page, &lpg->lps_cl, obj, index, &lov_raid0_page_ops);

	sub = lov_sub_get(env, lio, stripe);
	if (IS_ERR(sub))
		return PTR_ERR(sub);

	subobj = lovsub2cl(r0->lo_sub[stripe]);
	list_for_each_entry(o, &subobj->co_lu.lo_header->loh_layers,
			    co_lu.lo_linkage) {
		if (o->co_ops->coo_page_init) {
			rc = o->co_ops->coo_page_init(sub->sub_env, o, page,
						      cl_index(subobj, suboff));
			if (rc != 0)
				break;
		}
	}
	lov_sub_put(sub);

	return rc;
}

static int lov_empty_page_print(const struct lu_env *env,
				const struct cl_page_slice *slice,
				void *cookie, lu_printer_t printer)
{
	struct lov_page *lp = cl2lov_page(slice);

	return (*printer)(env, cookie, LUSTRE_LOV_NAME "-page@%p, empty.\n",
			  lp);
}

static const struct cl_page_operations lov_empty_page_ops = {
	.cpo_print = lov_empty_page_print
};

int lov_page_init_empty(const struct lu_env *env, struct cl_object *obj,
			struct cl_page *page, pgoff_t index)
>>>>>>> v4.9.227
{
	struct lov_page *lpg = cl_object_page_slice(obj, page);
	void *addr;

<<<<<<< HEAD
	cl_page_slice_add(page, &lpg->lps_cl, obj, &lov_empty_page_ops);
	addr = kmap(vmpage);
	memset(addr, 0, cl_page_size(obj));
	kunmap(vmpage);
=======
	cl_page_slice_add(page, &lpg->lps_cl, obj, index, &lov_empty_page_ops);
	addr = kmap(page->cp_vmpage);
	memset(addr, 0, cl_page_size(obj));
	kunmap(page->cp_vmpage);
>>>>>>> v4.9.227
	cl_page_export(env, page, 1);
	return 0;
}

<<<<<<< HEAD

=======
>>>>>>> v4.9.227
/** @} lov */
