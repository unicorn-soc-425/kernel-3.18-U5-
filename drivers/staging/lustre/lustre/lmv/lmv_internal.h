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
 * Copyright (c) 2004, 2010, Oracle and/or its affiliates. All rights reserved.
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
 */

#ifndef _LMV_INTERNAL_H_
#define _LMV_INTERNAL_H_

#include "../include/lustre/lustre_idl.h"
#include "../include/obd.h"
<<<<<<< HEAD

#define LMV_MAX_TGT_COUNT 128

#define lmv_init_lock(lmv)   mutex_lock(&lmv->init_mutex);
#define lmv_init_unlock(lmv) mutex_unlock(&lmv->init_mutex);

=======
#include "../include/lustre_lmv.h"

#define LMV_MAX_TGT_COUNT 128

>>>>>>> v4.9.227
#define LL_IT2STR(it)					\
	((it) ? ldlm_it2str((it)->it_op) : "0")

int lmv_check_connect(struct obd_device *obd);

int lmv_intent_lock(struct obd_export *exp, struct md_op_data *op_data,
<<<<<<< HEAD
		    void *lmm, int lmmsize, struct lookup_intent *it,
		    int flags, struct ptlrpc_request **reqp,
		    ldlm_blocking_callback cb_blocking,
		    __u64 extra_lock_flags);

int lmv_intent_lookup(struct obd_export *exp, struct md_op_data *op_data,
		      void *lmm, int lmmsize, struct lookup_intent *it,
		      int flags, struct ptlrpc_request **reqp,
		      ldlm_blocking_callback cb_blocking,
		      __u64 extra_lock_flags);

int lmv_intent_open(struct obd_export *exp, struct md_op_data *op_data,
		    void *lmm, int lmmsize, struct lookup_intent *it,
		    int flags, struct ptlrpc_request **reqp,
		    ldlm_blocking_callback cb_blocking,
		    __u64 extra_lock_flags);

int lmv_blocking_ast(struct ldlm_lock *, struct ldlm_lock_desc *,
		     void *, int);
int lmv_fld_lookup(struct lmv_obd *lmv, const struct lu_fid *fid, u32 *mds);
int __lmv_fid_alloc(struct lmv_obd *lmv, struct lu_fid *fid, u32 mds);
int lmv_fid_alloc(struct obd_export *exp, struct lu_fid *fid,
		  struct md_op_data *op_data);

static inline struct lmv_stripe_md *lmv_get_mea(struct ptlrpc_request *req)
{
	struct mdt_body	 *body;
	struct lmv_stripe_md    *mea;

	LASSERT(req != NULL);

	body = req_capsule_server_get(&req->rq_pill, &RMF_MDT_BODY);

	if (!body || !S_ISDIR(body->mode) || !body->eadatasize)
		return NULL;

	mea = req_capsule_server_sized_get(&req->rq_pill, &RMF_MDT_MD,
					   body->eadatasize);
	LASSERT(mea != NULL);

	if (mea->mea_count == 0)
		return NULL;
	if (mea->mea_magic != MEA_MAGIC_LAST_CHAR &&
		mea->mea_magic != MEA_MAGIC_ALL_CHARS &&
		mea->mea_magic != MEA_MAGIC_HASH_SEGMENT)
		return NULL;

	return mea;
}

static inline int lmv_get_easize(struct lmv_obd *lmv)
{
	return sizeof(struct lmv_stripe_md) +
		lmv->desc.ld_tgt_count *
		sizeof(struct lu_fid);
}

static inline struct lmv_tgt_desc *
lmv_get_target(struct lmv_obd *lmv, u32 mds)
{
	int count = lmv->desc.ld_tgt_count;
	int i;

	for (i = 0; i < count; i++) {
		if (lmv->tgts[i] == NULL)
			continue;

		if (lmv->tgts[i]->ltd_idx == mds)
			return lmv->tgts[i];
=======
		    struct lookup_intent *it, struct ptlrpc_request **reqp,
		    ldlm_blocking_callback cb_blocking,
		    __u64 extra_lock_flags);

int lmv_fld_lookup(struct lmv_obd *lmv, const struct lu_fid *fid, u32 *mds);
int __lmv_fid_alloc(struct lmv_obd *lmv, struct lu_fid *fid, u32 mds);
int lmv_fid_alloc(const struct lu_env *env, struct obd_export *exp,
		  struct lu_fid *fid, struct md_op_data *op_data);

int lmv_unpack_md(struct obd_export *exp, struct lmv_stripe_md **lsmp,
		  const union lmv_mds_md *lmm, int stripe_count);

int lmv_revalidate_slaves(struct obd_export *exp,
			  const struct lmv_stripe_md *lsm,
			  ldlm_blocking_callback cb_blocking,
			  int extra_lock_flags);

static inline struct obd_device *lmv2obd_dev(struct lmv_obd *lmv)
{
	return container_of0(lmv, struct obd_device, u.lmv);
}

static inline struct lmv_tgt_desc *
lmv_get_target(struct lmv_obd *lmv, u32 mdt_idx, int *index)
{
	int i;

	for (i = 0; i < lmv->desc.ld_tgt_count; i++) {
		if (!lmv->tgts[i])
			continue;

		if (lmv->tgts[i]->ltd_idx == mdt_idx) {
			if (index)
				*index = i;
			return lmv->tgts[i];
		}
>>>>>>> v4.9.227
	}

	return ERR_PTR(-ENODEV);
}

<<<<<<< HEAD
static inline struct lmv_tgt_desc *
lmv_find_target(struct lmv_obd *lmv, const struct lu_fid *fid)
{
	u32 mds = 0;
	int rc;

	if (lmv->desc.ld_tgt_count > 1) {
		rc = lmv_fld_lookup(lmv, fid, &mds);
		if (rc)
			return ERR_PTR(rc);
	}

	return lmv_get_target(lmv, mds);
=======
static inline int
lmv_find_target_index(struct lmv_obd *lmv, const struct lu_fid *fid)
{
	struct lmv_tgt_desc *ltd;
	u32 mdt_idx = 0;
	int index = 0;

	if (lmv->desc.ld_tgt_count > 1) {
		int rc;

		rc = lmv_fld_lookup(lmv, fid, &mdt_idx);
		if (rc < 0)
			return rc;
	}

	ltd = lmv_get_target(lmv, mdt_idx, &index);
	if (IS_ERR(ltd))
		return PTR_ERR(ltd);

	return index;
}

static inline struct lmv_tgt_desc *
lmv_find_target(struct lmv_obd *lmv, const struct lu_fid *fid)
{
	int index;

	index = lmv_find_target_index(lmv, fid);
	if (index < 0)
		return ERR_PTR(index);

	return lmv->tgts[index];
}

static inline int lmv_stripe_md_size(int stripe_count)
{
	struct lmv_stripe_md *lsm;

	return sizeof(*lsm) + stripe_count * sizeof(lsm->lsm_md_oinfo[0]);
}

int lmv_name_to_stripe_index(enum lmv_hash_type hashtype,
			     unsigned int max_mdt_index,
			     const char *name, int namelen);

static inline const struct lmv_oinfo *
lsm_name_to_stripe_info(const struct lmv_stripe_md *lsm, const char *name,
			int namelen)
{
	int stripe_index;

	stripe_index = lmv_name_to_stripe_index(lsm->lsm_md_hash_type,
						lsm->lsm_md_stripe_count,
						name, namelen);
	if (stripe_index < 0)
		return ERR_PTR(stripe_index);

	LASSERTF(stripe_index < lsm->lsm_md_stripe_count,
		 "stripe_index = %d, stripe_count = %d hash_type = %x name = %.*s\n",
		 stripe_index, lsm->lsm_md_stripe_count,
		 lsm->lsm_md_hash_type, namelen, name);

	return &lsm->lsm_md_oinfo[stripe_index];
}

static inline bool lmv_need_try_all_stripes(const struct lmv_stripe_md *lsm)
{
	return !lmv_is_known_hash_type(lsm->lsm_md_hash_type) ||
	       lsm->lsm_md_hash_type & LMV_HASH_FLAG_MIGRATION;
>>>>>>> v4.9.227
}

struct lmv_tgt_desc
*lmv_locate_mds(struct lmv_obd *lmv, struct md_op_data *op_data,
		struct lu_fid *fid);
/* lproc_lmv.c */
<<<<<<< HEAD
#if defined(CONFIG_PROC_FS)
void lprocfs_lmv_init_vars(struct lprocfs_static_vars *lvars);
#else
static inline void lprocfs_lmv_init_vars(struct lprocfs_static_vars *lvars)
{
	memset(lvars, 0, sizeof(*lvars));
}
#endif
extern struct file_operations lmv_proc_target_fops;
=======
void lprocfs_lmv_init_vars(struct lprocfs_static_vars *lvars);

extern const struct file_operations lmv_proc_target_fops;
>>>>>>> v4.9.227

#endif
