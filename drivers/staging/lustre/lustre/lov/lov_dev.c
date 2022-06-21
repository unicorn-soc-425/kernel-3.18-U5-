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
 * Copyright (c) 2012, Intel Corporation.
=======
 * Copyright (c) 2012, 2015, Intel Corporation.
>>>>>>> v4.9.227
 */
/*
 * This file is part of Lustre, http://www.lustre.org/
 * Lustre is a trademark of Sun Microsystems, Inc.
 *
 * Implementation of cl_device and cl_device_type for LOV layer.
 *
 *   Author: Nikita Danilov <nikita.danilov@sun.com>
 */

#define DEBUG_SUBSYSTEM S_LOV

/* class_name2obd() */
#include "../include/obd_class.h"

#include "lov_cl_internal.h"
#include "lov_internal.h"

<<<<<<< HEAD

=======
>>>>>>> v4.9.227
struct kmem_cache *lov_lock_kmem;
struct kmem_cache *lov_object_kmem;
struct kmem_cache *lov_thread_kmem;
struct kmem_cache *lov_session_kmem;
struct kmem_cache *lov_req_kmem;

struct kmem_cache *lovsub_lock_kmem;
struct kmem_cache *lovsub_object_kmem;
struct kmem_cache *lovsub_req_kmem;

struct kmem_cache *lov_lock_link_kmem;

/** Lock class of lov_device::ld_mutex. */
<<<<<<< HEAD
struct lock_class_key cl_lov_device_mutex_class;
=======
static struct lock_class_key cl_lov_device_mutex_class;
>>>>>>> v4.9.227

struct lu_kmem_descr lov_caches[] = {
	{
		.ckd_cache = &lov_lock_kmem,
		.ckd_name  = "lov_lock_kmem",
		.ckd_size  = sizeof(struct lov_lock)
	},
	{
		.ckd_cache = &lov_object_kmem,
		.ckd_name  = "lov_object_kmem",
		.ckd_size  = sizeof(struct lov_object)
	},
	{
		.ckd_cache = &lov_thread_kmem,
		.ckd_name  = "lov_thread_kmem",
		.ckd_size  = sizeof(struct lov_thread_info)
	},
	{
		.ckd_cache = &lov_session_kmem,
		.ckd_name  = "lov_session_kmem",
		.ckd_size  = sizeof(struct lov_session)
	},
	{
		.ckd_cache = &lov_req_kmem,
		.ckd_name  = "lov_req_kmem",
		.ckd_size  = sizeof(struct lov_req)
	},
	{
		.ckd_cache = &lovsub_lock_kmem,
		.ckd_name  = "lovsub_lock_kmem",
		.ckd_size  = sizeof(struct lovsub_lock)
	},
	{
		.ckd_cache = &lovsub_object_kmem,
		.ckd_name  = "lovsub_object_kmem",
		.ckd_size  = sizeof(struct lovsub_object)
	},
	{
		.ckd_cache = &lovsub_req_kmem,
		.ckd_name  = "lovsub_req_kmem",
		.ckd_size  = sizeof(struct lovsub_req)
	},
	{
		.ckd_cache = &lov_lock_link_kmem,
		.ckd_name  = "lov_lock_link_kmem",
		.ckd_size  = sizeof(struct lov_lock_link)
	},
	{
		.ckd_cache = NULL
	}
};

/*****************************************************************************
 *
 * Lov transfer operations.
 *
 */

static void lov_req_completion(const struct lu_env *env,
			       const struct cl_req_slice *slice, int ioret)
{
	struct lov_req *lr;

	lr = cl2lov_req(slice);
<<<<<<< HEAD
	OBD_SLAB_FREE_PTR(lr, lov_req_kmem);
=======
	kmem_cache_free(lov_req_kmem, lr);
>>>>>>> v4.9.227
}

static const struct cl_req_operations lov_req_ops = {
	.cro_completion = lov_req_completion
};

/*****************************************************************************
 *
 * Lov device and device type functions.
 *
 */

static void *lov_key_init(const struct lu_context *ctx,
			  struct lu_context_key *key)
{
	struct lov_thread_info *info;

<<<<<<< HEAD
	OBD_SLAB_ALLOC_PTR_GFP(info, lov_thread_kmem, GFP_NOFS);
	if (info != NULL)
		INIT_LIST_HEAD(&info->lti_closure.clc_list);
	else
=======
	info = kmem_cache_zalloc(lov_thread_kmem, GFP_NOFS);
	if (!info)
>>>>>>> v4.9.227
		info = ERR_PTR(-ENOMEM);
	return info;
}

static void lov_key_fini(const struct lu_context *ctx,
			 struct lu_context_key *key, void *data)
{
	struct lov_thread_info *info = data;
<<<<<<< HEAD
	LINVRNT(list_empty(&info->lti_closure.clc_list));
	OBD_SLAB_FREE_PTR(info, lov_thread_kmem);
=======

	kmem_cache_free(lov_thread_kmem, info);
>>>>>>> v4.9.227
}

struct lu_context_key lov_key = {
	.lct_tags = LCT_CL_THREAD,
	.lct_init = lov_key_init,
	.lct_fini = lov_key_fini
};

static void *lov_session_key_init(const struct lu_context *ctx,
				  struct lu_context_key *key)
{
	struct lov_session *info;

<<<<<<< HEAD
	OBD_SLAB_ALLOC_PTR_GFP(info, lov_session_kmem, GFP_NOFS);
	if (info == NULL)
=======
	info = kmem_cache_zalloc(lov_session_kmem, GFP_NOFS);
	if (!info)
>>>>>>> v4.9.227
		info = ERR_PTR(-ENOMEM);
	return info;
}

static void lov_session_key_fini(const struct lu_context *ctx,
				 struct lu_context_key *key, void *data)
{
	struct lov_session *info = data;
<<<<<<< HEAD
	OBD_SLAB_FREE_PTR(info, lov_session_kmem);
=======

	kmem_cache_free(lov_session_kmem, info);
>>>>>>> v4.9.227
}

struct lu_context_key lov_session_key = {
	.lct_tags = LCT_SESSION,
	.lct_init = lov_session_key_init,
	.lct_fini = lov_session_key_fini
};

/* type constructor/destructor: lov_type_{init,fini,start,stop}() */
LU_TYPE_INIT_FINI(lov, &lov_key, &lov_session_key);

static struct lu_device *lov_device_fini(const struct lu_env *env,
					 struct lu_device *d)
{
	int i;
	struct lov_device *ld = lu2lov_dev(d);

<<<<<<< HEAD
	LASSERT(ld->ld_lov != NULL);
	if (ld->ld_target == NULL)
=======
	LASSERT(ld->ld_lov);
	if (!ld->ld_target)
>>>>>>> v4.9.227
		return NULL;

	lov_foreach_target(ld, i) {
		struct lovsub_device *lsd;

		lsd = ld->ld_target[i];
<<<<<<< HEAD
		if (lsd != NULL) {
=======
		if (lsd) {
>>>>>>> v4.9.227
			cl_stack_fini(env, lovsub2cl_dev(lsd));
			ld->ld_target[i] = NULL;
		}
	}
	return NULL;
}

static int lov_device_init(const struct lu_env *env, struct lu_device *d,
			   const char *name, struct lu_device *next)
{
	struct lov_device *ld = lu2lov_dev(d);
	int i;
	int rc = 0;

<<<<<<< HEAD
	LASSERT(d->ld_site != NULL);
	if (ld->ld_target == NULL)
=======
	LASSERT(d->ld_site);
	if (!ld->ld_target)
>>>>>>> v4.9.227
		return rc;

	lov_foreach_target(ld, i) {
		struct lovsub_device *lsd;
		struct cl_device     *cl;
		struct lov_tgt_desc  *desc;

		desc = ld->ld_lov->lov_tgts[i];
<<<<<<< HEAD
		if (desc == NULL)
=======
		if (!desc)
>>>>>>> v4.9.227
			continue;

		cl = cl_type_setup(env, d->ld_site, &lovsub_device_type,
				   desc->ltd_obd->obd_lu_dev);
		if (IS_ERR(cl)) {
			rc = PTR_ERR(cl);
			break;
		}
		lsd = cl2lovsub_dev(cl);
		lsd->acid_idx = i;
		lsd->acid_super = ld;
		ld->ld_target[i] = lsd;
	}

	if (rc)
		lov_device_fini(env, d);
	else
		ld->ld_flags |= LOV_DEV_INITIALIZED;

	return rc;
}

static int lov_req_init(const struct lu_env *env, struct cl_device *dev,
			struct cl_req *req)
{
	struct lov_req *lr;
	int result;

<<<<<<< HEAD
	OBD_SLAB_ALLOC_PTR_GFP(lr, lov_req_kmem, GFP_NOFS);
	if (lr != NULL) {
		cl_req_slice_add(req, &lr->lr_cl, dev, &lov_req_ops);
		result = 0;
	} else
		result = -ENOMEM;
=======
	lr = kmem_cache_zalloc(lov_req_kmem, GFP_NOFS);
	if (lr) {
		cl_req_slice_add(req, &lr->lr_cl, dev, &lov_req_ops);
		result = 0;
	} else {
		result = -ENOMEM;
	}
>>>>>>> v4.9.227
	return result;
}

static const struct cl_device_operations lov_cl_ops = {
	.cdo_req_init = lov_req_init
};

static void lov_emerg_free(struct lov_device_emerg **emrg, int nr)
{
	int i;

	for (i = 0; i < nr; ++i) {
		struct lov_device_emerg *em;

		em = emrg[i];
<<<<<<< HEAD
		if (em != NULL) {
			LASSERT(em->emrg_page_list.pl_nr == 0);
			if (em->emrg_env != NULL)
				cl_env_put(em->emrg_env, &em->emrg_refcheck);
			OBD_FREE_PTR(em);
		}
	}
	OBD_FREE(emrg, nr * sizeof(emrg[0]));
=======
		if (em) {
			LASSERT(em->emrg_page_list.pl_nr == 0);
			if (em->emrg_env)
				cl_env_put(em->emrg_env, &em->emrg_refcheck);
			kfree(em);
		}
	}
	kfree(emrg);
>>>>>>> v4.9.227
}

static struct lu_device *lov_device_free(const struct lu_env *env,
					 struct lu_device *d)
{
	struct lov_device *ld = lu2lov_dev(d);
	const int	  nr = ld->ld_target_nr;

	cl_device_fini(lu2cl_dev(d));
<<<<<<< HEAD
	if (ld->ld_target != NULL)
		OBD_FREE(ld->ld_target, nr * sizeof(ld->ld_target[0]));
	if (ld->ld_emrg != NULL)
		lov_emerg_free(ld->ld_emrg, nr);
	OBD_FREE_PTR(ld);
=======
	kfree(ld->ld_target);
	if (ld->ld_emrg)
		lov_emerg_free(ld->ld_emrg, nr);
	kfree(ld);
>>>>>>> v4.9.227
	return NULL;
}

static void lov_cl_del_target(const struct lu_env *env, struct lu_device *dev,
			      __u32 index)
{
	struct lov_device *ld = lu2lov_dev(dev);

<<<<<<< HEAD
	if (ld->ld_target[index] != NULL) {
=======
	if (ld->ld_target[index]) {
>>>>>>> v4.9.227
		cl_stack_fini(env, lovsub2cl_dev(ld->ld_target[index]));
		ld->ld_target[index] = NULL;
	}
}

static struct lov_device_emerg **lov_emerg_alloc(int nr)
{
	struct lov_device_emerg **emerg;
	int i;
	int result;

<<<<<<< HEAD
	OBD_ALLOC(emerg, nr * sizeof(emerg[0]));
	if (emerg == NULL)
=======
	emerg = kcalloc(nr, sizeof(emerg[0]), GFP_NOFS);
	if (!emerg)
>>>>>>> v4.9.227
		return ERR_PTR(-ENOMEM);
	for (result = i = 0; i < nr && result == 0; i++) {
		struct lov_device_emerg *em;

<<<<<<< HEAD
		OBD_ALLOC_PTR(em);
		if (em != NULL) {
			emerg[i] = em;
			cl_page_list_init(&em->emrg_page_list);
			em->emrg_env = cl_env_alloc(&em->emrg_refcheck,
						    LCT_REMEMBER|LCT_NOREF);
			if (!IS_ERR(em->emrg_env))
				em->emrg_env->le_ctx.lc_cookie = 0x2;
			else {
				result = PTR_ERR(em->emrg_env);
				em->emrg_env = NULL;
			}
		} else
			result = -ENOMEM;
=======
		em = kzalloc(sizeof(*em), GFP_NOFS);
		if (em) {
			emerg[i] = em;
			cl_page_list_init(&em->emrg_page_list);
			em->emrg_env = cl_env_alloc(&em->emrg_refcheck,
						    LCT_REMEMBER | LCT_NOREF);
			if (!IS_ERR(em->emrg_env)) {
				em->emrg_env->le_ctx.lc_cookie = 0x2;
			} else {
				result = PTR_ERR(em->emrg_env);
				em->emrg_env = NULL;
			}
		} else {
			result = -ENOMEM;
		}
>>>>>>> v4.9.227
	}
	if (result != 0) {
		lov_emerg_free(emerg, nr);
		emerg = ERR_PTR(result);
	}
	return emerg;
}

static int lov_expand_targets(const struct lu_env *env, struct lov_device *dev)
{
	int   result;
	__u32 tgt_size;
	__u32 sub_size;

	result = 0;
	tgt_size = dev->ld_lov->lov_tgt_size;
	sub_size = dev->ld_target_nr;
	if (sub_size < tgt_size) {
		struct lovsub_device    **newd;
		struct lov_device_emerg **emerg;
		const size_t	      sz   = sizeof(newd[0]);

		emerg = lov_emerg_alloc(tgt_size);
		if (IS_ERR(emerg))
			return PTR_ERR(emerg);

<<<<<<< HEAD
		OBD_ALLOC(newd, tgt_size * sz);
		if (newd != NULL) {
			mutex_lock(&dev->ld_mutex);
			if (sub_size > 0) {
				memcpy(newd, dev->ld_target, sub_size * sz);
				OBD_FREE(dev->ld_target, sub_size * sz);
=======
		newd = kcalloc(tgt_size, sz, GFP_NOFS);
		if (newd) {
			mutex_lock(&dev->ld_mutex);
			if (sub_size > 0) {
				memcpy(newd, dev->ld_target, sub_size * sz);
				kfree(dev->ld_target);
>>>>>>> v4.9.227
			}
			dev->ld_target    = newd;
			dev->ld_target_nr = tgt_size;

<<<<<<< HEAD
			if (dev->ld_emrg != NULL)
=======
			if (dev->ld_emrg)
>>>>>>> v4.9.227
				lov_emerg_free(dev->ld_emrg, sub_size);
			dev->ld_emrg = emerg;
			mutex_unlock(&dev->ld_mutex);
		} else {
			lov_emerg_free(emerg, tgt_size);
			result = -ENOMEM;
		}
	}
	return result;
}

static int lov_cl_add_target(const struct lu_env *env, struct lu_device *dev,
			     __u32 index)
{
	struct obd_device    *obd = dev->ld_obd;
	struct lov_device    *ld  = lu2lov_dev(dev);
	struct lov_tgt_desc  *tgt;
	struct lovsub_device *lsd;
	struct cl_device     *cl;
	int rc;

	obd_getref(obd);

	tgt = obd->u.lov.lov_tgts[index];
<<<<<<< HEAD
	LASSERT(tgt != NULL);
	LASSERT(tgt->ltd_obd != NULL);
=======
>>>>>>> v4.9.227

	if (!tgt->ltd_obd->obd_set_up) {
		CERROR("Target %s not set up\n", obd_uuid2str(&tgt->ltd_uuid));
		return -EINVAL;
	}

	rc = lov_expand_targets(env, ld);
	if (rc == 0 && ld->ld_flags & LOV_DEV_INITIALIZED) {
<<<<<<< HEAD
		LASSERT(dev->ld_site != NULL);
=======
		LASSERT(dev->ld_site);
>>>>>>> v4.9.227

		cl = cl_type_setup(env, dev->ld_site, &lovsub_device_type,
				   tgt->ltd_obd->obd_lu_dev);
		if (!IS_ERR(cl)) {
			lsd = cl2lovsub_dev(cl);
			lsd->acid_idx = index;
			lsd->acid_super = ld;
			ld->ld_target[index] = lsd;
		} else {
			CERROR("add failed (%d), deleting %s\n", rc,
			       obd_uuid2str(&tgt->ltd_uuid));
			lov_cl_del_target(env, dev, index);
			rc = PTR_ERR(cl);
		}
	}
	obd_putref(obd);
	return rc;
}

static int lov_process_config(const struct lu_env *env,
			      struct lu_device *d, struct lustre_cfg *cfg)
{
	struct obd_device *obd = d->ld_obd;
	int cmd;
	int rc;
	int gen;
	__u32 index;

	obd_getref(obd);

	cmd = cfg->lcfg_command;
	rc = lov_process_config_base(d->ld_obd, cfg, &index, &gen);
	if (rc == 0) {
		switch (cmd) {
		case LCFG_LOV_ADD_OBD:
		case LCFG_LOV_ADD_INA:
			rc = lov_cl_add_target(env, d, index);
			if (rc != 0)
				lov_del_target(d->ld_obd, index, NULL, 0);
			break;
		case LCFG_LOV_DEL_OBD:
			lov_cl_del_target(env, d, index);
			break;
		}
	}
	obd_putref(obd);
	return rc;
}

static const struct lu_device_operations lov_lu_ops = {
	.ldo_object_alloc      = lov_object_alloc,
	.ldo_process_config    = lov_process_config,
};

static struct lu_device *lov_device_alloc(const struct lu_env *env,
					  struct lu_device_type *t,
					  struct lustre_cfg *cfg)
{
	struct lu_device *d;
	struct lov_device *ld;
	struct obd_device *obd;
	int rc;

<<<<<<< HEAD
	OBD_ALLOC_PTR(ld);
	if (ld == NULL)
=======
	ld = kzalloc(sizeof(*ld), GFP_NOFS);
	if (!ld)
>>>>>>> v4.9.227
		return ERR_PTR(-ENOMEM);

	cl_device_init(&ld->ld_cl, t);
	d = lov2lu_dev(ld);
	d->ld_ops	= &lov_lu_ops;
	ld->ld_cl.cd_ops = &lov_cl_ops;

	mutex_init(&ld->ld_mutex);
	lockdep_set_class(&ld->ld_mutex, &cl_lov_device_mutex_class);

	/* setup the LOV OBD */
	obd = class_name2obd(lustre_cfg_string(cfg, 0));
<<<<<<< HEAD
	LASSERT(obd != NULL);
=======
	LASSERT(obd);
>>>>>>> v4.9.227
	rc = lov_setup(obd, cfg);
	if (rc) {
		lov_device_free(env, d);
		return ERR_PTR(rc);
	}

	ld->ld_lov = &obd->u.lov;
	return d;
}

static const struct lu_device_type_operations lov_device_type_ops = {
	.ldto_init = lov_type_init,
	.ldto_fini = lov_type_fini,

	.ldto_start = lov_type_start,
	.ldto_stop  = lov_type_stop,

	.ldto_device_alloc = lov_device_alloc,
	.ldto_device_free  = lov_device_free,

	.ldto_device_init    = lov_device_init,
	.ldto_device_fini    = lov_device_fini
};

struct lu_device_type lov_device_type = {
	.ldt_tags     = LU_DEVICE_CL,
	.ldt_name     = LUSTRE_LOV_NAME,
	.ldt_ops      = &lov_device_type_ops,
	.ldt_ctx_tags = LCT_CL_THREAD
};
<<<<<<< HEAD
EXPORT_SYMBOL(lov_device_type);
=======
>>>>>>> v4.9.227

/** @} lov */
