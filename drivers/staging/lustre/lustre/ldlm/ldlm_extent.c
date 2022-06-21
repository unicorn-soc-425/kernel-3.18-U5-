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
 * Copyright (c) 2010, 2012, Intel Corporation.
 */
/*
 * This file is part of Lustre, http://www.lustre.org/
 * Lustre is a trademark of Sun Microsystems, Inc.
 *
 * lustre/ldlm/ldlm_extent.c
 *
 * Author: Peter Braam <braam@clusterfs.com>
 * Author: Phil Schwan <phil@clusterfs.com>
 */

/**
 * This file contains implementation of EXTENT lock type
 *
 * EXTENT lock type is for locking a contiguous range of values, represented
 * by 64-bit starting and ending offsets (inclusive). There are several extent
 * lock modes, some of which may be mutually incompatible. Extent locks are
 * considered incompatible if their modes are incompatible and their extents
 * intersect.  See the lock mode compatibility matrix in lustre_dlm.h.
 */

#define DEBUG_SUBSYSTEM S_LDLM
#include "../../include/linux/libcfs/libcfs.h"
#include "../include/lustre_dlm.h"
#include "../include/obd_support.h"
#include "../include/obd.h"
#include "../include/obd_class.h"
#include "../include/lustre_lib.h"
#include "ldlm_internal.h"

<<<<<<< HEAD

=======
>>>>>>> v4.9.227
/* When a lock is cancelled by a client, the KMS may undergo change if this
 * is the "highest lock".  This function returns the new KMS value.
 * Caller must hold lr_lock already.
 *
<<<<<<< HEAD
 * NB: A lock on [x,y] protects a KMS of up to y + 1 bytes! */
=======
 * NB: A lock on [x,y] protects a KMS of up to y + 1 bytes!
 */
>>>>>>> v4.9.227
__u64 ldlm_extent_shift_kms(struct ldlm_lock *lock, __u64 old_kms)
{
	struct ldlm_resource *res = lock->l_resource;
	struct list_head *tmp;
	struct ldlm_lock *lck;
	__u64 kms = 0;

	/* don't let another thread in ldlm_extent_shift_kms race in
	 * just after we finish and take our lock into account in its
<<<<<<< HEAD
	 * calculation of the kms */
	lock->l_flags |= LDLM_FL_KMS_IGNORE;
=======
	 * calculation of the kms
	 */
	ldlm_set_kms_ignore(lock);
>>>>>>> v4.9.227

	list_for_each(tmp, &res->lr_granted) {
		lck = list_entry(tmp, struct ldlm_lock, l_res_link);

<<<<<<< HEAD
		if (lck->l_flags & LDLM_FL_KMS_IGNORE)
=======
		if (ldlm_is_kms_ignore(lck))
>>>>>>> v4.9.227
			continue;

		if (lck->l_policy_data.l_extent.end >= old_kms)
			return old_kms;

		/* This extent _has_ to be smaller than old_kms (checked above)
<<<<<<< HEAD
		 * so kms can only ever be smaller or the same as old_kms. */
=======
		 * so kms can only ever be smaller or the same as old_kms.
		 */
>>>>>>> v4.9.227
		if (lck->l_policy_data.l_extent.end + 1 > kms)
			kms = lck->l_policy_data.l_extent.end + 1;
	}
	LASSERTF(kms <= old_kms, "kms %llu old_kms %llu\n", kms, old_kms);

	return kms;
}
EXPORT_SYMBOL(ldlm_extent_shift_kms);

struct kmem_cache *ldlm_interval_slab;
<<<<<<< HEAD
=======

/* interval tree, for LDLM_EXTENT. */
static void ldlm_interval_attach(struct ldlm_interval *n, struct ldlm_lock *l)
{
	LASSERT(!l->l_tree_node);
	LASSERT(l->l_resource->lr_type == LDLM_EXTENT);

	list_add_tail(&l->l_sl_policy, &n->li_group);
	l->l_tree_node = n;
}

>>>>>>> v4.9.227
struct ldlm_interval *ldlm_interval_alloc(struct ldlm_lock *lock)
{
	struct ldlm_interval *node;

	LASSERT(lock->l_resource->lr_type == LDLM_EXTENT);
<<<<<<< HEAD
	OBD_SLAB_ALLOC_PTR_GFP(node, ldlm_interval_slab, GFP_NOFS);
	if (node == NULL)
=======
	node = kmem_cache_zalloc(ldlm_interval_slab, GFP_NOFS);
	if (!node)
>>>>>>> v4.9.227
		return NULL;

	INIT_LIST_HEAD(&node->li_group);
	ldlm_interval_attach(node, lock);
	return node;
}

void ldlm_interval_free(struct ldlm_interval *node)
{
	if (node) {
		LASSERT(list_empty(&node->li_group));
		LASSERT(!interval_is_intree(&node->li_node));
<<<<<<< HEAD
		OBD_SLAB_FREE(node, ldlm_interval_slab, sizeof(*node));
	}
}

/* interval tree, for LDLM_EXTENT. */
void ldlm_interval_attach(struct ldlm_interval *n,
			  struct ldlm_lock *l)
{
	LASSERT(l->l_tree_node == NULL);
	LASSERT(l->l_resource->lr_type == LDLM_EXTENT);

	list_add_tail(&l->l_sl_policy, &n->li_group);
	l->l_tree_node = n;
}

=======
		kmem_cache_free(ldlm_interval_slab, node);
	}
}

>>>>>>> v4.9.227
struct ldlm_interval *ldlm_interval_detach(struct ldlm_lock *l)
{
	struct ldlm_interval *n = l->l_tree_node;

<<<<<<< HEAD
	if (n == NULL)
=======
	if (!n)
>>>>>>> v4.9.227
		return NULL;

	LASSERT(!list_empty(&n->li_group));
	l->l_tree_node = NULL;
	list_del_init(&l->l_sl_policy);

	return list_empty(&n->li_group) ? n : NULL;
}

<<<<<<< HEAD
static inline int lock_mode_to_index(ldlm_mode_t mode)
=======
static inline int lock_mode_to_index(enum ldlm_mode mode)
>>>>>>> v4.9.227
{
	int index;

	LASSERT(mode != 0);
<<<<<<< HEAD
	LASSERT(IS_PO2(mode));
	for (index = -1; mode; index++, mode >>= 1) ;
=======
	LASSERT(is_power_of_2(mode));
	for (index = -1; mode; index++)
		mode >>= 1;
>>>>>>> v4.9.227
	LASSERT(index < LCK_MODE_NUM);
	return index;
}

/** Add newly granted lock into interval tree for the resource. */
void ldlm_extent_add_lock(struct ldlm_resource *res,
			  struct ldlm_lock *lock)
{
	struct interval_node *found, **root;
	struct ldlm_interval *node;
	struct ldlm_extent *extent;
	int idx;

	LASSERT(lock->l_granted_mode == lock->l_req_mode);

	node = lock->l_tree_node;
<<<<<<< HEAD
	LASSERT(node != NULL);
=======
	LASSERT(node);
>>>>>>> v4.9.227
	LASSERT(!interval_is_intree(&node->li_node));

	idx = lock_mode_to_index(lock->l_granted_mode);
	LASSERT(lock->l_granted_mode == 1 << idx);
	LASSERT(lock->l_granted_mode == res->lr_itree[idx].lit_mode);

	/* node extent initialize */
	extent = &lock->l_policy_data.l_extent;
	interval_set(&node->li_node, extent->start, extent->end);

	root = &res->lr_itree[idx].lit_root;
	found = interval_insert(&node->li_node, root);
	if (found) { /* The policy group found. */
<<<<<<< HEAD
		struct ldlm_interval *tmp = ldlm_interval_detach(lock);
		LASSERT(tmp != NULL);
=======
		struct ldlm_interval *tmp;

		tmp = ldlm_interval_detach(lock);
>>>>>>> v4.9.227
		ldlm_interval_free(tmp);
		ldlm_interval_attach(to_ldlm_interval(found), lock);
	}
	res->lr_itree[idx].lit_size++;

	/* even though we use interval tree to manage the extent lock, we also
<<<<<<< HEAD
	 * add the locks into grant list, for debug purpose, .. */
=======
	 * add the locks into grant list, for debug purpose, ..
	 */
>>>>>>> v4.9.227
	ldlm_resource_add_lock(res, &res->lr_granted, lock);
}

/** Remove cancelled lock from resource interval tree. */
void ldlm_extent_unlink_lock(struct ldlm_lock *lock)
{
	struct ldlm_resource *res = lock->l_resource;
	struct ldlm_interval *node = lock->l_tree_node;
	struct ldlm_interval_tree *tree;
	int idx;

	if (!node || !interval_is_intree(&node->li_node)) /* duplicate unlink */
		return;

	idx = lock_mode_to_index(lock->l_granted_mode);
	LASSERT(lock->l_granted_mode == 1 << idx);
	tree = &res->lr_itree[idx];

<<<<<<< HEAD
	LASSERT(tree->lit_root != NULL); /* assure the tree is not null */
=======
	LASSERT(tree->lit_root); /* assure the tree is not null */
>>>>>>> v4.9.227

	tree->lit_size--;
	node = ldlm_interval_detach(lock);
	if (node) {
		interval_erase(&node->li_node, &tree->lit_root);
		ldlm_interval_free(node);
	}
}

void ldlm_extent_policy_wire_to_local(const ldlm_wire_policy_data_t *wpolicy,
<<<<<<< HEAD
				     ldlm_policy_data_t *lpolicy)
=======
				      ldlm_policy_data_t *lpolicy)
>>>>>>> v4.9.227
{
	memset(lpolicy, 0, sizeof(*lpolicy));
	lpolicy->l_extent.start = wpolicy->l_extent.start;
	lpolicy->l_extent.end = wpolicy->l_extent.end;
	lpolicy->l_extent.gid = wpolicy->l_extent.gid;
}

void ldlm_extent_policy_local_to_wire(const ldlm_policy_data_t *lpolicy,
<<<<<<< HEAD
				     ldlm_wire_policy_data_t *wpolicy)
=======
				      ldlm_wire_policy_data_t *wpolicy)
>>>>>>> v4.9.227
{
	memset(wpolicy, 0, sizeof(*wpolicy));
	wpolicy->l_extent.start = lpolicy->l_extent.start;
	wpolicy->l_extent.end = lpolicy->l_extent.end;
	wpolicy->l_extent.gid = lpolicy->l_extent.gid;
}
