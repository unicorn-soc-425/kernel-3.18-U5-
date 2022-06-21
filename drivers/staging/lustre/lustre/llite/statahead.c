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
 */

#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/pagemap.h>

#define DEBUG_SUBSYSTEM S_LLITE

#include "../include/obd_support.h"
<<<<<<< HEAD
#include "../include/lustre_lite.h"
=======
>>>>>>> v4.9.227
#include "../include/lustre_dlm.h"
#include "llite_internal.h"

#define SA_OMITTED_ENTRY_MAX 8ULL

<<<<<<< HEAD
typedef enum {
=======
enum se_stat {
>>>>>>> v4.9.227
	/** negative values are for error cases */
	SA_ENTRY_INIT = 0,      /** init entry */
	SA_ENTRY_SUCC = 1,      /** stat succeed */
	SA_ENTRY_INVA = 2,      /** invalid entry */
<<<<<<< HEAD
	SA_ENTRY_DEST = 3,      /** entry to be destroyed */
} se_stat_t;

struct ll_sa_entry {
	/* link into sai->sai_entries */
	struct list_head	      se_link;
	/* link into sai->sai_entries_{received,stated} */
	struct list_head	      se_list;
	/* link into sai hash table locally */
	struct list_head	      se_hash;
	/* entry reference count */
	atomic_t	    se_refcount;
=======
};

/*
 * sa_entry is not refcounted: statahead thread allocates it and do async stat,
 * and in async stat callback ll_statahead_interpret() will add it into
 * sai_interim_entries, later statahead thread will call sa_handle_callback() to
 * instantiate entry and move it into sai_entries, and then only scanner process
 * can access and free it.
 */
struct sa_entry {
	/* link into sai_interim_entries or sai_entries */
	struct list_head	      se_list;
	/* link into sai hash table locally */
	struct list_head	      se_hash;
>>>>>>> v4.9.227
	/* entry index in the sai */
	__u64		   se_index;
	/* low layer ldlm lock handle */
	__u64		   se_handle;
	/* entry status */
<<<<<<< HEAD
	se_stat_t	       se_stat;
=======
	enum se_stat		se_state;
>>>>>>> v4.9.227
	/* entry size, contains name */
	int		     se_size;
	/* pointer to async getattr enqueue info */
	struct md_enqueue_info *se_minfo;
	/* pointer to the async getattr request */
	struct ptlrpc_request  *se_req;
	/* pointer to the target inode */
	struct inode	   *se_inode;
	/* entry name */
	struct qstr	     se_qstr;
};

<<<<<<< HEAD
static unsigned int sai_generation = 0;
static DEFINE_SPINLOCK(sai_generation_lock);

static inline int ll_sa_entry_unhashed(struct ll_sa_entry *entry)
{
	return list_empty(&entry->se_hash);
}

/*
 * The entry only can be released by the caller, it is necessary to hold lock.
 */
static inline int ll_sa_entry_stated(struct ll_sa_entry *entry)
{
	smp_rmb();
	return (entry->se_stat != SA_ENTRY_INIT);
}

static inline int ll_sa_entry_hash(int val)
=======
static unsigned int sai_generation;
static DEFINE_SPINLOCK(sai_generation_lock);

/* sa_entry is ready to use */
static inline int sa_ready(struct sa_entry *entry)
{
	smp_rmb();
	return (entry->se_state != SA_ENTRY_INIT);
}

/* hash value to put in sai_cache */
static inline int sa_hash(int val)
>>>>>>> v4.9.227
{
	return val & LL_SA_CACHE_MASK;
}

<<<<<<< HEAD
/*
 * Insert entry to hash SA table.
 */
static inline void
ll_sa_entry_enhash(struct ll_statahead_info *sai, struct ll_sa_entry *entry)
{
	int i = ll_sa_entry_hash(entry->se_qstr.hash);
=======
/* hash entry into sai_cache */
static inline void
sa_rehash(struct ll_statahead_info *sai, struct sa_entry *entry)
{
	int i = sa_hash(entry->se_qstr.hash);
>>>>>>> v4.9.227

	spin_lock(&sai->sai_cache_lock[i]);
	list_add_tail(&entry->se_hash, &sai->sai_cache[i]);
	spin_unlock(&sai->sai_cache_lock[i]);
}

/*
 * Remove entry from SA table.
 */
static inline void
<<<<<<< HEAD
ll_sa_entry_unhash(struct ll_statahead_info *sai, struct ll_sa_entry *entry)
{
	int i = ll_sa_entry_hash(entry->se_qstr.hash);
=======
sa_unhash(struct ll_statahead_info *sai, struct sa_entry *entry)
{
	int i = sa_hash(entry->se_qstr.hash);
>>>>>>> v4.9.227

	spin_lock(&sai->sai_cache_lock[i]);
	list_del_init(&entry->se_hash);
	spin_unlock(&sai->sai_cache_lock[i]);
}

static inline int agl_should_run(struct ll_statahead_info *sai,
				 struct inode *inode)
{
<<<<<<< HEAD
	return (inode != NULL && S_ISREG(inode->i_mode) && sai->sai_agl_valid);
}

static inline struct ll_sa_entry *
sa_first_received_entry(struct ll_statahead_info *sai)
{
	return list_entry(sai->sai_entries_received.next,
			      struct ll_sa_entry, se_list);
}

static inline struct ll_inode_info *
agl_first_entry(struct ll_statahead_info *sai)
{
	return list_entry(sai->sai_entries_agl.next,
			      struct ll_inode_info, lli_agl_list);
}

=======
	return (inode && S_ISREG(inode->i_mode) && sai->sai_agl_valid);
}

/* statahead window is full */
>>>>>>> v4.9.227
static inline int sa_sent_full(struct ll_statahead_info *sai)
{
	return atomic_read(&sai->sai_cache_count) >= sai->sai_max;
}

<<<<<<< HEAD
static inline int sa_received_empty(struct ll_statahead_info *sai)
{
	return list_empty(&sai->sai_entries_received);
=======
/* got async stat replies */
static inline int sa_has_callback(struct ll_statahead_info *sai)
{
	return !list_empty(&sai->sai_interim_entries);
>>>>>>> v4.9.227
}

static inline int agl_list_empty(struct ll_statahead_info *sai)
{
<<<<<<< HEAD
	return list_empty(&sai->sai_entries_agl);
=======
	return list_empty(&sai->sai_agls);
>>>>>>> v4.9.227
}

/**
 * (1) hit ratio less than 80%
 * or
 * (2) consecutive miss more than 8
 * then means low hit.
 */
static inline int sa_low_hit(struct ll_statahead_info *sai)
{
	return ((sai->sai_hit > 7 && sai->sai_hit < 4 * sai->sai_miss) ||
		(sai->sai_consecutive_miss > 8));
}

/*
<<<<<<< HEAD
 * If the given index is behind of statahead window more than
=======
 * if the given index is behind of statahead window more than
>>>>>>> v4.9.227
 * SA_OMITTED_ENTRY_MAX, then it is old.
 */
static inline int is_omitted_entry(struct ll_statahead_info *sai, __u64 index)
{
	return ((__u64)sai->sai_max + index + SA_OMITTED_ENTRY_MAX <
		 sai->sai_index);
}

<<<<<<< HEAD
/*
 * Insert it into sai_entries tail when init.
 */
static struct ll_sa_entry *
ll_sa_entry_alloc(struct ll_statahead_info *sai, __u64 index,
		  const char *name, int len)
{
	struct ll_inode_info *lli;
	struct ll_sa_entry   *entry;
	int		   entry_size;
	char		 *dname;

	entry_size = sizeof(struct ll_sa_entry) + (len & ~3) + 4;
=======
/* allocate sa_entry and hash it to allow scanner process to find it */
static struct sa_entry *
sa_alloc(struct dentry *parent, struct ll_statahead_info *sai, __u64 index,
	 const char *name, int len)
{
	struct ll_inode_info *lli;
	struct sa_entry   *entry;
	int		   entry_size;
	char		 *dname;

	entry_size = sizeof(struct sa_entry) + (len & ~3) + 4;
>>>>>>> v4.9.227
	entry = kzalloc(entry_size, GFP_NOFS);
	if (unlikely(!entry))
		return ERR_PTR(-ENOMEM);

	CDEBUG(D_READA, "alloc sa entry %.*s(%p) index %llu\n",
	       len, name, entry, index);

	entry->se_index = index;
<<<<<<< HEAD

	/*
	 * Statahead entry reference rules:
	 *
	 * 1) When statahead entry is initialized, its reference is set as 2.
	 *    One reference is used by the directory scanner. When the scanner
	 *    searches the statahead cache for the given name, it can perform
	 *    lockless hash lookup (only the scanner can remove entry from hash
	 *    list), and once found, it needn't to call "atomic_inc()" for the
	 *    entry reference. So the performance is improved. After using the
	 *    statahead entry, the scanner will call "atomic_dec()" to drop the
	 *    reference held when initialization. If it is the last reference,
	 *    the statahead entry will be freed.
	 *
	 * 2) All other threads, including statahead thread and ptlrpcd thread,
	 *    when they process the statahead entry, the reference for target
	 *    should be held to guarantee the entry will not be released by the
	 *    directory scanner. After processing the entry, these threads will
	 *    drop the entry reference. If it is the last reference, the entry
	 *    will be freed.
	 *
	 *    The second reference when initializes the statahead entry is used
	 *    by the statahead thread, following the rule 2).
	 */
	atomic_set(&entry->se_refcount, 2);
	entry->se_stat = SA_ENTRY_INIT;
	entry->se_size = entry_size;
	dname = (char *)entry + sizeof(struct ll_sa_entry);
	memcpy(dname, name, len);
	dname[len] = 0;
	entry->se_qstr.hash = full_name_hash(name, len);
	entry->se_qstr.len = len;
	entry->se_qstr.name = dname;

	lli = ll_i2info(sai->sai_inode);
	spin_lock(&lli->lli_sa_lock);
	list_add_tail(&entry->se_link, &sai->sai_entries);
	INIT_LIST_HEAD(&entry->se_list);
	ll_sa_entry_enhash(sai, entry);
=======
	entry->se_state = SA_ENTRY_INIT;
	entry->se_size = entry_size;
	dname = (char *)entry + sizeof(struct sa_entry);
	memcpy(dname, name, len);
	dname[len] = 0;

	entry->se_qstr.hash = full_name_hash(parent, name, len);
	entry->se_qstr.len = len;
	entry->se_qstr.name = dname;

	lli = ll_i2info(sai->sai_dentry->d_inode);
	spin_lock(&lli->lli_sa_lock);
	INIT_LIST_HEAD(&entry->se_list);
	sa_rehash(sai, entry);
>>>>>>> v4.9.227
	spin_unlock(&lli->lli_sa_lock);

	atomic_inc(&sai->sai_cache_count);

	return entry;
}

<<<<<<< HEAD
/*
 * Used by the directory scanner to search entry with name.
 *
 * Only the caller can remove the entry from hash, so it is unnecessary to hold
 * hash lock. It is caller's duty to release the init refcount on the entry, so
 * it is also unnecessary to increase refcount on the entry.
 */
static struct ll_sa_entry *
ll_sa_entry_get_byname(struct ll_statahead_info *sai, const struct qstr *qstr)
{
	struct ll_sa_entry *entry;
	int i = ll_sa_entry_hash(qstr->hash);
=======
/* free sa_entry, which should have been unhashed and not in any list */
static void sa_free(struct ll_statahead_info *sai, struct sa_entry *entry)
{
	CDEBUG(D_READA, "free sa entry %.*s(%p) index %llu\n",
	       entry->se_qstr.len, entry->se_qstr.name, entry,
	       entry->se_index);

	LASSERT(list_empty(&entry->se_list));
	LASSERT(list_empty(&entry->se_hash));

	kfree(entry);
	atomic_dec(&sai->sai_cache_count);
}

/*
 * find sa_entry by name, used by directory scanner, lock is not needed because
 * only scanner can remove the entry from cache.
 */
static struct sa_entry *
sa_get(struct ll_statahead_info *sai, const struct qstr *qstr)
{
	struct sa_entry *entry;
	int i = sa_hash(qstr->hash);
>>>>>>> v4.9.227

	list_for_each_entry(entry, &sai->sai_cache[i], se_hash) {
		if (entry->se_qstr.hash == qstr->hash &&
		    entry->se_qstr.len == qstr->len &&
		    memcmp(entry->se_qstr.name, qstr->name, qstr->len) == 0)
			return entry;
	}
	return NULL;
}

<<<<<<< HEAD
/*
 * Used by the async getattr request callback to find entry with index.
 *
 * Inside lli_sa_lock to prevent others to change the list during the search.
 * It needs to increase entry refcount before returning to guarantee that the
 * entry cannot be freed by others.
 */
static struct ll_sa_entry *
ll_sa_entry_get_byindex(struct ll_statahead_info *sai, __u64 index)
{
	struct ll_sa_entry *entry;

	list_for_each_entry(entry, &sai->sai_entries, se_link) {
		if (entry->se_index == index) {
			LASSERT(atomic_read(&entry->se_refcount) > 0);
			atomic_inc(&entry->se_refcount);
			return entry;
		}
		if (entry->se_index > index)
			break;
	}
	return NULL;
}

static void ll_sa_entry_cleanup(struct ll_statahead_info *sai,
				 struct ll_sa_entry *entry)
{
	struct md_enqueue_info *minfo = entry->se_minfo;
	struct ptlrpc_request  *req   = entry->se_req;

=======
/* unhash and unlink sa_entry, and then free it */
static inline void
sa_kill(struct ll_statahead_info *sai, struct sa_entry *entry)
{
	struct ll_inode_info *lli = ll_i2info(sai->sai_dentry->d_inode);

	LASSERT(!list_empty(&entry->se_hash));
	LASSERT(!list_empty(&entry->se_list));
	LASSERT(sa_ready(entry));

	sa_unhash(sai, entry);

	spin_lock(&lli->lli_sa_lock);
	list_del_init(&entry->se_list);
	spin_unlock(&lli->lli_sa_lock);

	if (entry->se_inode)
		iput(entry->se_inode);

	sa_free(sai, entry);
}

/* called by scanner after use, sa_entry will be killed */
static void
sa_put(struct ll_statahead_info *sai, struct sa_entry *entry)
{
	struct sa_entry *tmp, *next;

	if (entry && entry->se_state == SA_ENTRY_SUCC) {
		struct ll_sb_info *sbi = ll_i2sbi(sai->sai_dentry->d_inode);

		sai->sai_hit++;
		sai->sai_consecutive_miss = 0;
		sai->sai_max = min(2 * sai->sai_max, sbi->ll_sa_max);
	} else {
		sai->sai_miss++;
		sai->sai_consecutive_miss++;
	}

	if (entry)
		sa_kill(sai, entry);

	/*
	 * kill old completed entries, only scanner process does this, no need
	 * to lock
	 */
	list_for_each_entry_safe(tmp, next, &sai->sai_entries, se_list) {
		if (!is_omitted_entry(sai, tmp->se_index))
			break;
		sa_kill(sai, tmp);
	}

	wake_up(&sai->sai_thread.t_ctl_waitq);
}

/*
 * update state and sort add entry to sai_entries by index, return true if
 * scanner is waiting on this entry.
 */
static bool
__sa_make_ready(struct ll_statahead_info *sai, struct sa_entry *entry, int ret)
{
	struct list_head *pos = &sai->sai_entries;
	__u64 index = entry->se_index;
	struct sa_entry *se;

	LASSERT(!sa_ready(entry));
	LASSERT(list_empty(&entry->se_list));

	list_for_each_entry_reverse(se, &sai->sai_entries, se_list) {
		if (se->se_index < entry->se_index) {
			pos = &se->se_list;
			break;
		}
	}
	list_add(&entry->se_list, pos);
	entry->se_state = ret < 0 ? SA_ENTRY_INVA : SA_ENTRY_SUCC;

	return (index == sai->sai_index_wait);
}

/*
 * release resources used in async stat RPC, update entry state and wakeup if
 * scanner process it waiting on this entry.
 */
static void
sa_make_ready(struct ll_statahead_info *sai, struct sa_entry *entry, int ret)
{
	struct ll_inode_info *lli = ll_i2info(sai->sai_dentry->d_inode);
	struct md_enqueue_info *minfo = entry->se_minfo;
	struct ptlrpc_request *req = entry->se_req;
	bool wakeup;

	/* release resources used in RPC */
>>>>>>> v4.9.227
	if (minfo) {
		entry->se_minfo = NULL;
		ll_intent_release(&minfo->mi_it);
		iput(minfo->mi_dir);
<<<<<<< HEAD
		OBD_FREE_PTR(minfo);
=======
		kfree(minfo);
>>>>>>> v4.9.227
	}

	if (req) {
		entry->se_req = NULL;
		ptlrpc_req_finished(req);
	}
<<<<<<< HEAD
}

static void ll_sa_entry_put(struct ll_statahead_info *sai,
			     struct ll_sa_entry *entry)
{
	if (atomic_dec_and_test(&entry->se_refcount)) {
		CDEBUG(D_READA, "free sa entry %.*s(%p) index %llu\n",
		       entry->se_qstr.len, entry->se_qstr.name, entry,
		       entry->se_index);

		LASSERT(list_empty(&entry->se_link));
		LASSERT(list_empty(&entry->se_list));
		LASSERT(ll_sa_entry_unhashed(entry));

		ll_sa_entry_cleanup(sai, entry);
		if (entry->se_inode)
			iput(entry->se_inode);

		OBD_FREE(entry, entry->se_size);
		atomic_dec(&sai->sai_cache_count);
	}
}

static inline void
do_sa_entry_fini(struct ll_statahead_info *sai, struct ll_sa_entry *entry)
{
	struct ll_inode_info *lli = ll_i2info(sai->sai_inode);

	LASSERT(!ll_sa_entry_unhashed(entry));
	LASSERT(!list_empty(&entry->se_link));

	ll_sa_entry_unhash(sai, entry);

	spin_lock(&lli->lli_sa_lock);
	entry->se_stat = SA_ENTRY_DEST;
	list_del_init(&entry->se_link);
	if (likely(!list_empty(&entry->se_list)))
		list_del_init(&entry->se_list);
	spin_unlock(&lli->lli_sa_lock);

	ll_sa_entry_put(sai, entry);
}

/*
 * Delete it from sai_entries_stated list when fini.
 */
static void
ll_sa_entry_fini(struct ll_statahead_info *sai, struct ll_sa_entry *entry)
{
	struct ll_sa_entry *pos, *next;

	if (entry)
		do_sa_entry_fini(sai, entry);

	/* drop old entry, only 'scanner' process does this, no need to lock */
	list_for_each_entry_safe(pos, next, &sai->sai_entries, se_link) {
		if (!is_omitted_entry(sai, pos->se_index))
			break;
		do_sa_entry_fini(sai, pos);
	}
}

/*
 * Inside lli_sa_lock.
 */
static void
do_sa_entry_to_stated(struct ll_statahead_info *sai,
		      struct ll_sa_entry *entry, se_stat_t stat)
{
	struct ll_sa_entry *se;
	struct list_head	 *pos = &sai->sai_entries_stated;

	if (!list_empty(&entry->se_list))
		list_del_init(&entry->se_list);

	list_for_each_entry_reverse(se, &sai->sai_entries_stated, se_list) {
		if (se->se_index < entry->se_index) {
			pos = &se->se_list;
			break;
		}
	}

	list_add(&entry->se_list, pos);
	entry->se_stat = stat;
}

/*
 * Move entry to sai_entries_stated and sort with the index.
 * \retval 1    -- entry to be destroyed.
 * \retval 0    -- entry is inserted into stated list.
 */
static int
ll_sa_entry_to_stated(struct ll_statahead_info *sai,
		      struct ll_sa_entry *entry, se_stat_t stat)
{
	struct ll_inode_info *lli = ll_i2info(sai->sai_inode);
	int		   ret = 1;

	ll_sa_entry_cleanup(sai, entry);

	spin_lock(&lli->lli_sa_lock);
	if (likely(entry->se_stat != SA_ENTRY_DEST)) {
		do_sa_entry_to_stated(sai, entry, stat);
		ret = 0;
	}
	spin_unlock(&lli->lli_sa_lock);

	return ret;
}

/*
 * Insert inode into the list of sai_entries_agl.
 */
=======

	spin_lock(&lli->lli_sa_lock);
	wakeup = __sa_make_ready(sai, entry, ret);
	spin_unlock(&lli->lli_sa_lock);

	if (wakeup)
		wake_up(&sai->sai_waitq);
}

/* Insert inode into the list of sai_agls. */
>>>>>>> v4.9.227
static void ll_agl_add(struct ll_statahead_info *sai,
		       struct inode *inode, int index)
{
	struct ll_inode_info *child  = ll_i2info(inode);
<<<<<<< HEAD
	struct ll_inode_info *parent = ll_i2info(sai->sai_inode);
=======
	struct ll_inode_info *parent = ll_i2info(sai->sai_dentry->d_inode);
>>>>>>> v4.9.227
	int		   added  = 0;

	spin_lock(&child->lli_agl_lock);
	if (child->lli_agl_index == 0) {
		child->lli_agl_index = index;
		spin_unlock(&child->lli_agl_lock);

		LASSERT(list_empty(&child->lli_agl_list));

		igrab(inode);
		spin_lock(&parent->lli_agl_lock);
<<<<<<< HEAD
		if (agl_list_empty(sai))
			added = 1;
		list_add_tail(&child->lli_agl_list, &sai->sai_entries_agl);
=======
		if (list_empty(&sai->sai_agls))
			added = 1;
		list_add_tail(&child->lli_agl_list, &sai->sai_agls);
>>>>>>> v4.9.227
		spin_unlock(&parent->lli_agl_lock);
	} else {
		spin_unlock(&child->lli_agl_lock);
	}

	if (added > 0)
		wake_up(&sai->sai_agl_thread.t_ctl_waitq);
}

<<<<<<< HEAD
static struct ll_statahead_info *ll_sai_alloc(void)
{
=======
/* allocate sai */
static struct ll_statahead_info *ll_sai_alloc(struct dentry *dentry)
{
	struct ll_inode_info *lli = ll_i2info(dentry->d_inode);
>>>>>>> v4.9.227
	struct ll_statahead_info *sai;
	int		       i;

	sai = kzalloc(sizeof(*sai), GFP_NOFS);
	if (!sai)
		return NULL;

<<<<<<< HEAD
	atomic_set(&sai->sai_refcount, 1);

	spin_lock(&sai_generation_lock);
	sai->sai_generation = ++sai_generation;
	if (unlikely(sai_generation == 0))
		sai->sai_generation = ++sai_generation;
	spin_unlock(&sai_generation_lock);

=======
	sai->sai_dentry = dget(dentry);
	atomic_set(&sai->sai_refcount, 1);

>>>>>>> v4.9.227
	sai->sai_max = LL_SA_RPC_MIN;
	sai->sai_index = 1;
	init_waitqueue_head(&sai->sai_waitq);
	init_waitqueue_head(&sai->sai_thread.t_ctl_waitq);
	init_waitqueue_head(&sai->sai_agl_thread.t_ctl_waitq);

<<<<<<< HEAD
	INIT_LIST_HEAD(&sai->sai_entries);
	INIT_LIST_HEAD(&sai->sai_entries_received);
	INIT_LIST_HEAD(&sai->sai_entries_stated);
	INIT_LIST_HEAD(&sai->sai_entries_agl);
=======
	INIT_LIST_HEAD(&sai->sai_interim_entries);
	INIT_LIST_HEAD(&sai->sai_entries);
	INIT_LIST_HEAD(&sai->sai_agls);
>>>>>>> v4.9.227

	for (i = 0; i < LL_SA_CACHE_SIZE; i++) {
		INIT_LIST_HEAD(&sai->sai_cache[i]);
		spin_lock_init(&sai->sai_cache_lock[i]);
	}
	atomic_set(&sai->sai_cache_count, 0);

<<<<<<< HEAD
	return sai;
}

static inline struct ll_statahead_info *
ll_sai_get(struct ll_statahead_info *sai)
{
	atomic_inc(&sai->sai_refcount);
	return sai;
}

static void ll_sai_put(struct ll_statahead_info *sai)
{
	struct inode	 *inode = sai->sai_inode;
	struct ll_inode_info *lli   = ll_i2info(inode);

	if (atomic_dec_and_lock(&sai->sai_refcount, &lli->lli_sa_lock)) {
		struct ll_sa_entry *entry, *next;

		if (unlikely(atomic_read(&sai->sai_refcount) > 0)) {
			/* It is race case, the interpret callback just hold
			 * a reference count */
			spin_unlock(&lli->lli_sa_lock);
			return;
		}

		LASSERT(lli->lli_opendir_key == NULL);
		LASSERT(thread_is_stopped(&sai->sai_thread));
		LASSERT(thread_is_stopped(&sai->sai_agl_thread));

		lli->lli_sai = NULL;
		lli->lli_opendir_pid = 0;
		spin_unlock(&lli->lli_sa_lock);

		if (sai->sai_sent > sai->sai_replied)
			CDEBUG(D_READA, "statahead for dir "DFID
			      " does not finish: [sent:%llu] [replied:%llu]\n",
			      PFID(&lli->lli_fid),
			      sai->sai_sent, sai->sai_replied);

		list_for_each_entry_safe(entry, next,
					     &sai->sai_entries, se_link)
			do_sa_entry_fini(sai, entry);

		LASSERT(list_empty(&sai->sai_entries));
		LASSERT(sa_received_empty(sai));
		LASSERT(list_empty(&sai->sai_entries_stated));

		LASSERT(atomic_read(&sai->sai_cache_count) == 0);
		LASSERT(agl_list_empty(sai));

		iput(inode);
		OBD_FREE_PTR(sai);
	}
}

/* Do NOT forget to drop inode refcount when into sai_entries_agl. */
=======
	spin_lock(&sai_generation_lock);
	lli->lli_sa_generation = ++sai_generation;
	if (unlikely(!sai_generation))
		lli->lli_sa_generation = ++sai_generation;
	spin_unlock(&sai_generation_lock);

	return sai;
}

/* free sai */
static inline void ll_sai_free(struct ll_statahead_info *sai)
{
	LASSERT(sai->sai_dentry);
	dput(sai->sai_dentry);
	kfree(sai);
}

/*
 * take refcount of sai if sai for @dir exists, which means statahead is on for
 * this directory.
 */
static inline struct ll_statahead_info *ll_sai_get(struct inode *dir)
{
	struct ll_inode_info *lli = ll_i2info(dir);
	struct ll_statahead_info *sai = NULL;

	spin_lock(&lli->lli_sa_lock);
	sai = lli->lli_sai;
	if (sai)
		atomic_inc(&sai->sai_refcount);
	spin_unlock(&lli->lli_sa_lock);

	return sai;
}

/*
 * put sai refcount after use, if refcount reaches zero, free sai and sa_entries
 * attached to it.
 */
static void ll_sai_put(struct ll_statahead_info *sai)
{
	struct ll_inode_info *lli = ll_i2info(sai->sai_dentry->d_inode);

	if (atomic_dec_and_lock(&sai->sai_refcount, &lli->lli_sa_lock)) {
		struct ll_sb_info *sbi = ll_i2sbi(sai->sai_dentry->d_inode);
		struct sa_entry *entry, *next;

		lli->lli_sai = NULL;
		spin_unlock(&lli->lli_sa_lock);

		LASSERT(thread_is_stopped(&sai->sai_thread));
		LASSERT(thread_is_stopped(&sai->sai_agl_thread));
		LASSERT(sai->sai_sent == sai->sai_replied);
		LASSERT(!sa_has_callback(sai));

		list_for_each_entry_safe(entry, next, &sai->sai_entries,
					 se_list)
			sa_kill(sai, entry);

		LASSERT(atomic_read(&sai->sai_cache_count) == 0);
		LASSERT(list_empty(&sai->sai_agls));

		ll_sai_free(sai);
		atomic_dec(&sbi->ll_sa_running);
	}
}

/* Do NOT forget to drop inode refcount when into sai_agls. */
>>>>>>> v4.9.227
static void ll_agl_trigger(struct inode *inode, struct ll_statahead_info *sai)
{
	struct ll_inode_info *lli   = ll_i2info(inode);
	__u64		 index = lli->lli_agl_index;
	int		   rc;

	LASSERT(list_empty(&lli->lli_agl_list));

	/* AGL maybe fall behind statahead with one entry */
	if (is_omitted_entry(sai, index + 1)) {
		lli->lli_agl_index = 0;
		iput(inode);
		return;
	}

	/* Someone is in glimpse (sync or async), do nothing. */
	rc = down_write_trylock(&lli->lli_glimpse_sem);
	if (rc == 0) {
		lli->lli_agl_index = 0;
		iput(inode);
		return;
	}

	/*
	 * Someone triggered glimpse within 1 sec before.
	 * 1) The former glimpse succeeded with glimpse lock granted by OST, and
	 *    if the lock is still cached on client, AGL needs to do nothing. If
	 *    it is cancelled by other client, AGL maybe cannot obtain new lock
	 *    for no glimpse callback triggered by AGL.
	 * 2) The former glimpse succeeded, but OST did not grant glimpse lock.
	 *    Under such case, it is quite possible that the OST will not grant
	 *    glimpse lock for AGL also.
	 * 3) The former glimpse failed, compared with other two cases, it is
	 *    relative rare. AGL can ignore such case, and it will not muchly
	 *    affect the performance.
	 */
	if (lli->lli_glimpse_time != 0 &&
	    time_before(cfs_time_shift(-1), lli->lli_glimpse_time)) {
		up_write(&lli->lli_glimpse_sem);
		lli->lli_agl_index = 0;
		iput(inode);
		return;
	}

	CDEBUG(D_READA, "Handling (init) async glimpse: inode = "
	       DFID", idx = %llu\n", PFID(&lli->lli_fid), index);

	cl_agl(inode);
	lli->lli_agl_index = 0;
	lli->lli_glimpse_time = cfs_time_current();
	up_write(&lli->lli_glimpse_sem);

	CDEBUG(D_READA, "Handled (init) async glimpse: inode= "
	       DFID", idx = %llu, rc = %d\n",
	       PFID(&lli->lli_fid), index, rc);

	iput(inode);
}

<<<<<<< HEAD
static void ll_post_statahead(struct ll_statahead_info *sai)
{
	struct inode	   *dir   = sai->sai_inode;
	struct inode	   *child;
	struct ll_inode_info   *lli   = ll_i2info(dir);
	struct ll_sa_entry     *entry;
=======
/*
 * prepare inode for sa entry, add it into agl list, now sa_entry is ready
 * to be used by scanner process.
 */
static void sa_instantiate(struct ll_statahead_info *sai,
			   struct sa_entry *entry)
{
	struct inode *dir = sai->sai_dentry->d_inode;
	struct inode	   *child;
>>>>>>> v4.9.227
	struct md_enqueue_info *minfo;
	struct lookup_intent   *it;
	struct ptlrpc_request  *req;
	struct mdt_body	*body;
	int		     rc    = 0;

<<<<<<< HEAD
	spin_lock(&lli->lli_sa_lock);
	if (unlikely(sa_received_empty(sai))) {
		spin_unlock(&lli->lli_sa_lock);
		return;
	}
	entry = sa_first_received_entry(sai);
	atomic_inc(&entry->se_refcount);
	list_del_init(&entry->se_list);
	spin_unlock(&lli->lli_sa_lock);

=======
>>>>>>> v4.9.227
	LASSERT(entry->se_handle != 0);

	minfo = entry->se_minfo;
	it = &minfo->mi_it;
	req = entry->se_req;
	body = req_capsule_server_get(&req->rq_pill, &RMF_MDT_BODY);
<<<<<<< HEAD
	if (body == NULL) {
=======
	if (!body) {
>>>>>>> v4.9.227
		rc = -EFAULT;
		goto out;
	}

	child = entry->se_inode;
<<<<<<< HEAD
	if (child == NULL) {
=======
	if (!child) {
>>>>>>> v4.9.227
		/*
		 * lookup.
		 */
		LASSERT(fid_is_zero(&minfo->mi_data.op_fid2));

		/* XXX: No fid in reply, this is probably cross-ref case.
<<<<<<< HEAD
		 * SA can't handle it yet. */
		if (body->valid & OBD_MD_MDS) {
=======
		 * SA can't handle it yet.
		 */
		if (body->mbo_valid & OBD_MD_MDS) {
>>>>>>> v4.9.227
			rc = -EAGAIN;
			goto out;
		}
	} else {
		/*
		 * revalidate.
		 */
		/* unlinked and re-created with the same name */
<<<<<<< HEAD
		if (unlikely(!lu_fid_eq(&minfo->mi_data.op_fid2, &body->fid1))){
=======
		if (unlikely(!lu_fid_eq(&minfo->mi_data.op_fid2, &body->mbo_fid1))) {
>>>>>>> v4.9.227
			entry->se_inode = NULL;
			iput(child);
			child = NULL;
		}
	}

<<<<<<< HEAD
	it->d.lustre.it_lock_handle = entry->se_handle;
=======
	it->it_lock_handle = entry->se_handle;
>>>>>>> v4.9.227
	rc = md_revalidate_lock(ll_i2mdexp(dir), it, ll_inode2fid(dir), NULL);
	if (rc != 1) {
		rc = -EAGAIN;
		goto out;
	}

	rc = ll_prep_inode(&child, req, dir->i_sb, it);
	if (rc)
		goto out;

<<<<<<< HEAD
	CDEBUG(D_DLMTRACE, "setting l_data to inode %p (%lu/%u)\n",
	       child, child->i_ino, child->i_generation);
=======
	CDEBUG(D_READA, "%s: setting %.*s" DFID " l_data to inode %p\n",
	       ll_get_fsname(child->i_sb, NULL, 0),
	       entry->se_qstr.len, entry->se_qstr.name,
	       PFID(ll_inode2fid(child)), child);
>>>>>>> v4.9.227
	ll_set_lock_data(ll_i2sbi(dir)->ll_md_exp, child, it, NULL);

	entry->se_inode = child;

	if (agl_should_run(sai, child))
		ll_agl_add(sai, child, entry->se_index);

out:
<<<<<<< HEAD
	/* The "ll_sa_entry_to_stated()" will drop related ldlm ibits lock
	 * reference count by calling "ll_intent_drop_lock()" in spite of the
	 * above operations failed or not. Do not worry about calling
	 * "ll_intent_drop_lock()" more than once. */
	rc = ll_sa_entry_to_stated(sai, entry,
				   rc < 0 ? SA_ENTRY_INVA : SA_ENTRY_SUCC);
	if (rc == 0 && entry->se_index == sai->sai_index_wait)
		wake_up(&sai->sai_waitq);
	ll_sa_entry_put(sai, entry);
}

=======
	/*
	 * sa_make_ready() will drop ldlm ibits lock refcount by calling
	 * ll_intent_drop_lock() in spite of failures. Do not worry about
	 * calling ll_intent_drop_lock() more than once.
	 */
	sa_make_ready(sai, entry, rc);
}

/* once there are async stat replies, instantiate sa_entry from replies */
static void sa_handle_callback(struct ll_statahead_info *sai)
{
	struct ll_inode_info *lli;

	lli = ll_i2info(sai->sai_dentry->d_inode);

	while (sa_has_callback(sai)) {
		struct sa_entry *entry;

		spin_lock(&lli->lli_sa_lock);
		if (unlikely(!sa_has_callback(sai))) {
			spin_unlock(&lli->lli_sa_lock);
			break;
		}
		entry = list_entry(sai->sai_interim_entries.next,
				   struct sa_entry, se_list);
		list_del_init(&entry->se_list);
		spin_unlock(&lli->lli_sa_lock);

		sa_instantiate(sai, entry);
	}
}

/*
 * callback for async stat, because this is called in ptlrpcd context, we only
 * put sa_entry in sai_cb_entries list, and let sa_handle_callback() to really
 * prepare inode and instantiate sa_entry later.
 */
>>>>>>> v4.9.227
static int ll_statahead_interpret(struct ptlrpc_request *req,
				  struct md_enqueue_info *minfo, int rc)
{
	struct lookup_intent     *it  = &minfo->mi_it;
	struct inode	     *dir = minfo->mi_dir;
	struct ll_inode_info     *lli = ll_i2info(dir);
<<<<<<< HEAD
	struct ll_statahead_info *sai = NULL;
	struct ll_sa_entry       *entry;
	int		       wakeup;
=======
	struct ll_statahead_info *sai = lli->lli_sai;
	struct sa_entry *entry = (struct sa_entry *)minfo->mi_cbdata;
	__u64 handle = 0;
	bool wakeup;
>>>>>>> v4.9.227

	if (it_disposition(it, DISP_LOOKUP_NEG))
		rc = -ENOENT;

<<<<<<< HEAD
	spin_lock(&lli->lli_sa_lock);
	/* stale entry */
	if (unlikely(lli->lli_sai == NULL ||
		     lli->lli_sai->sai_generation != minfo->mi_generation)) {
		spin_unlock(&lli->lli_sa_lock);
		rc = -ESTALE;
		goto out;
	} else {
		sai = ll_sai_get(lli->lli_sai);
		if (unlikely(!thread_is_running(&sai->sai_thread))) {
			sai->sai_replied++;
			spin_unlock(&lli->lli_sa_lock);
			rc = -EBADFD;
			goto out;
		}

		entry = ll_sa_entry_get_byindex(sai, minfo->mi_cbdata);
		if (entry == NULL) {
			sai->sai_replied++;
			spin_unlock(&lli->lli_sa_lock);
			rc = -EIDRM;
			goto out;
		}

		if (rc != 0) {
			do_sa_entry_to_stated(sai, entry, SA_ENTRY_INVA);
			wakeup = (entry->se_index == sai->sai_index_wait);
		} else {
			entry->se_minfo = minfo;
			entry->se_req = ptlrpc_request_addref(req);
			/* Release the async ibits lock ASAP to avoid deadlock
			 * when statahead thread tries to enqueue lock on parent
			 * for readpage and other tries to enqueue lock on child
			 * with parent's lock held, for example: unlink. */
			entry->se_handle = it->d.lustre.it_lock_handle;
			ll_intent_drop_lock(it);
			wakeup = sa_received_empty(sai);
			list_add_tail(&entry->se_list,
					  &sai->sai_entries_received);
		}
		sai->sai_replied++;
		spin_unlock(&lli->lli_sa_lock);

		ll_sa_entry_put(sai, entry);
		if (wakeup)
			wake_up(&sai->sai_thread.t_ctl_waitq);
	}

out:
	if (rc != 0) {
		ll_intent_release(it);
		iput(dir);
		OBD_FREE_PTR(minfo);
	}
	if (sai != NULL)
		ll_sai_put(sai);
	return rc;
}

static void sa_args_fini(struct md_enqueue_info *minfo,
=======
	/*
	 * because statahead thread will wait for all inflight RPC to finish,
	 * sai should be always valid, no need to refcount
	 */
	LASSERT(sai);
	LASSERT(!thread_is_stopped(&sai->sai_thread));
	LASSERT(entry);

	CDEBUG(D_READA, "sa_entry %.*s rc %d\n",
	       entry->se_qstr.len, entry->se_qstr.name, rc);

	if (rc) {
		ll_intent_release(it);
		iput(dir);
		kfree(minfo);
	} else {
		/*
		 * release ibits lock ASAP to avoid deadlock when statahead
		 * thread enqueues lock on parent in readdir and another
		 * process enqueues lock on child with parent lock held, eg.
		 * unlink.
		 */
		handle = it->it_lock_handle;
		ll_intent_drop_lock(it);
	}

	spin_lock(&lli->lli_sa_lock);
	if (rc) {
		wakeup = __sa_make_ready(sai, entry, rc);
	} else {
		entry->se_minfo = minfo;
		entry->se_req = ptlrpc_request_addref(req);
		/*
		 * Release the async ibits lock ASAP to avoid deadlock
		 * when statahead thread tries to enqueue lock on parent
		 * for readpage and other tries to enqueue lock on child
		 * with parent's lock held, for example: unlink.
		 */
		entry->se_handle = handle;
		wakeup = !sa_has_callback(sai);
		list_add_tail(&entry->se_list, &sai->sai_interim_entries);
	}
	sai->sai_replied++;

	if (wakeup)
		wake_up(&sai->sai_thread.t_ctl_waitq);
	spin_unlock(&lli->lli_sa_lock);

	return rc;
}

/* finish async stat RPC arguments */
static void sa_fini_data(struct md_enqueue_info *minfo,
>>>>>>> v4.9.227
			 struct ldlm_enqueue_info *einfo)
{
	LASSERT(minfo && einfo);
	iput(minfo->mi_dir);
<<<<<<< HEAD
	capa_put(minfo->mi_data.op_capa1);
	capa_put(minfo->mi_data.op_capa2);
	OBD_FREE_PTR(minfo);
	OBD_FREE_PTR(einfo);
}

/**
 * There is race condition between "capa_put" and "ll_statahead_interpret" for
 * accessing "op_data.op_capa[1,2]" as following:
 * "capa_put" releases "op_data.op_capa[1,2]"'s reference count after calling
 * "md_intent_getattr_async". But "ll_statahead_interpret" maybe run first, and
 * fill "op_data.op_capa[1,2]" as POISON, then cause "capa_put" access invalid
 * "ocapa". So here reserve "op_data.op_capa[1,2]" in "pcapa" before calling
 * "md_intent_getattr_async".
 */
static int sa_args_init(struct inode *dir, struct inode *child,
			struct ll_sa_entry *entry, struct md_enqueue_info **pmi,
			struct ldlm_enqueue_info **pei,
			struct obd_capa **pcapa)
{
	struct qstr	      *qstr = &entry->se_qstr;
	struct ll_inode_info     *lli  = ll_i2info(dir);
=======
	kfree(minfo);
	kfree(einfo);
}

/**
 * prepare arguments for async stat RPC.
 */
static int sa_prep_data(struct inode *dir, struct inode *child,
			struct sa_entry *entry, struct md_enqueue_info **pmi,
			struct ldlm_enqueue_info **pei)
{
	const struct qstr      *qstr = &entry->se_qstr;
>>>>>>> v4.9.227
	struct md_enqueue_info   *minfo;
	struct ldlm_enqueue_info *einfo;
	struct md_op_data	*op_data;

	einfo = kzalloc(sizeof(*einfo), GFP_NOFS);
	if (!einfo)
		return -ENOMEM;

	minfo = kzalloc(sizeof(*minfo), GFP_NOFS);
	if (!minfo) {
<<<<<<< HEAD
		OBD_FREE_PTR(einfo);
=======
		kfree(einfo);
>>>>>>> v4.9.227
		return -ENOMEM;
	}

	op_data = ll_prep_md_op_data(&minfo->mi_data, dir, child, qstr->name,
				     qstr->len, 0, LUSTRE_OPC_ANY, NULL);
	if (IS_ERR(op_data)) {
<<<<<<< HEAD
		OBD_FREE_PTR(einfo);
		OBD_FREE_PTR(minfo);
=======
		kfree(einfo);
		kfree(minfo);
>>>>>>> v4.9.227
		return PTR_ERR(op_data);
	}

	minfo->mi_it.it_op = IT_GETATTR;
	minfo->mi_dir = igrab(dir);
	minfo->mi_cb = ll_statahead_interpret;
<<<<<<< HEAD
	minfo->mi_generation = lli->lli_sai->sai_generation;
	minfo->mi_cbdata = entry->se_index;
=======
	minfo->mi_cbdata = entry;
>>>>>>> v4.9.227

	einfo->ei_type   = LDLM_IBITS;
	einfo->ei_mode   = it_to_lock_mode(&minfo->mi_it);
	einfo->ei_cb_bl  = ll_md_blocking_ast;
	einfo->ei_cb_cp  = ldlm_completion_ast;
	einfo->ei_cb_gl  = NULL;
	einfo->ei_cbdata = NULL;

	*pmi = minfo;
	*pei = einfo;
<<<<<<< HEAD
	pcapa[0] = op_data->op_capa1;
	pcapa[1] = op_data->op_capa2;
=======
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int do_sa_lookup(struct inode *dir, struct ll_sa_entry *entry)
{
	struct md_enqueue_info   *minfo;
	struct ldlm_enqueue_info *einfo;
	struct obd_capa	  *capas[2];
	int		       rc;

	rc = sa_args_init(dir, NULL, entry, &minfo, &einfo, capas);
=======
/* async stat for file not found in dcache */
static int sa_lookup(struct inode *dir, struct sa_entry *entry)
{
	struct md_enqueue_info   *minfo;
	struct ldlm_enqueue_info *einfo;
	int		       rc;

	rc = sa_prep_data(dir, NULL, entry, &minfo, &einfo);
>>>>>>> v4.9.227
	if (rc)
		return rc;

	rc = md_intent_getattr_async(ll_i2mdexp(dir), minfo, einfo);
<<<<<<< HEAD
	if (!rc) {
		capa_put(capas[0]);
		capa_put(capas[1]);
	} else {
		sa_args_fini(minfo, einfo);
	}
=======
	if (rc)
		sa_fini_data(minfo, einfo);
>>>>>>> v4.9.227

	return rc;
}

/**
<<<<<<< HEAD
 * similar to ll_revalidate_it().
 * \retval      1 -- dentry valid
 * \retval      0 -- will send stat-ahead request
 * \retval others -- prepare stat-ahead request failed
 */
static int do_sa_revalidate(struct inode *dir, struct ll_sa_entry *entry,
			    struct dentry *dentry)
{
	struct inode	     *inode = dentry->d_inode;
	struct lookup_intent      it = { .it_op = IT_GETATTR,
					 .d.lustre.it_lock_handle = 0 };
	struct md_enqueue_info   *minfo;
	struct ldlm_enqueue_info *einfo;
	struct obd_capa	  *capas[2];
	int rc;

	if (unlikely(inode == NULL))
=======
 * async stat for file found in dcache, similar to .revalidate
 *
 * \retval	1 dentry valid, no RPC sent
 * \retval	0 dentry invalid, will send async stat RPC
 * \retval	negative number upon error
 */
static int sa_revalidate(struct inode *dir, struct sa_entry *entry,
			 struct dentry *dentry)
{
	struct inode	     *inode = d_inode(dentry);
	struct lookup_intent      it = { .it_op = IT_GETATTR,
					 .it_lock_handle = 0 };
	struct md_enqueue_info   *minfo;
	struct ldlm_enqueue_info *einfo;
	int rc;

	if (unlikely(!inode))
>>>>>>> v4.9.227
		return 1;

	if (d_mountpoint(dentry))
		return 1;

	entry->se_inode = igrab(inode);
	rc = md_revalidate_lock(ll_i2mdexp(dir), &it, ll_inode2fid(inode),
				NULL);
	if (rc == 1) {
<<<<<<< HEAD
		entry->se_handle = it.d.lustre.it_lock_handle;
=======
		entry->se_handle = it.it_lock_handle;
>>>>>>> v4.9.227
		ll_intent_release(&it);
		return 1;
	}

<<<<<<< HEAD
	rc = sa_args_init(dir, inode, entry, &minfo, &einfo, capas);
=======
	rc = sa_prep_data(dir, inode, entry, &minfo, &einfo);
>>>>>>> v4.9.227
	if (rc) {
		entry->se_inode = NULL;
		iput(inode);
		return rc;
	}

	rc = md_intent_getattr_async(ll_i2mdexp(dir), minfo, einfo);
<<<<<<< HEAD
	if (!rc) {
		capa_put(capas[0]);
		capa_put(capas[1]);
	} else {
		entry->se_inode = NULL;
		iput(inode);
		sa_args_fini(minfo, einfo);
=======
	if (rc) {
		entry->se_inode = NULL;
		iput(inode);
		sa_fini_data(minfo, einfo);
>>>>>>> v4.9.227
	}

	return rc;
}

<<<<<<< HEAD
static void ll_statahead_one(struct dentry *parent, const char* entry_name,
			     int entry_name_len)
{
	struct inode	     *dir    = parent->d_inode;
	struct ll_inode_info     *lli    = ll_i2info(dir);
	struct ll_statahead_info *sai    = lli->lli_sai;
	struct dentry	    *dentry = NULL;
	struct ll_sa_entry       *entry;
	int		       rc;
	int		       rc1;

	entry = ll_sa_entry_alloc(sai, sai->sai_index, entry_name,
				  entry_name_len);
=======
/* async stat for file with @name */
static void sa_statahead(struct dentry *parent, const char *name, int len)
{
	struct inode	     *dir    = d_inode(parent);
	struct ll_inode_info     *lli    = ll_i2info(dir);
	struct ll_statahead_info *sai    = lli->lli_sai;
	struct dentry	    *dentry = NULL;
	struct sa_entry *entry;
	int		       rc;

	entry = sa_alloc(parent, sai, sai->sai_index, name, len);
>>>>>>> v4.9.227
	if (IS_ERR(entry))
		return;

	dentry = d_lookup(parent, &entry->se_qstr);
	if (!dentry) {
<<<<<<< HEAD
		rc = do_sa_lookup(dir, entry);
	} else {
		rc = do_sa_revalidate(dir, entry, dentry);
		if (rc == 1 && agl_should_run(sai, dentry->d_inode))
			ll_agl_add(sai, dentry->d_inode, entry->se_index);
	}

	if (dentry != NULL)
		dput(dentry);

	if (rc) {
		rc1 = ll_sa_entry_to_stated(sai, entry,
					rc < 0 ? SA_ENTRY_INVA : SA_ENTRY_SUCC);
		if (rc1 == 0 && entry->se_index == sai->sai_index_wait)
			wake_up(&sai->sai_waitq);
	} else {
		sai->sai_sent++;
	}

	sai->sai_index++;
	/* drop one refcount on entry by ll_sa_entry_alloc */
	ll_sa_entry_put(sai, entry);
}

static int ll_agl_thread(void *arg)
{
	struct dentry	    *parent = (struct dentry *)arg;
	struct inode	     *dir    = parent->d_inode;
	struct ll_inode_info     *plli   = ll_i2info(dir);
	struct ll_inode_info     *clli;
	struct ll_sb_info	*sbi    = ll_i2sbi(dir);
	struct ll_statahead_info *sai    = ll_sai_get(plli->lli_sai);
	struct ptlrpc_thread     *thread = &sai->sai_agl_thread;
	struct l_wait_info	lwi    = { 0 };

	thread->t_pid = current_pid();
	CDEBUG(D_READA, "agl thread started: sai %p, parent %.*s\n",
	       sai, parent->d_name.len, parent->d_name.name);
=======
		rc = sa_lookup(dir, entry);
	} else {
		rc = sa_revalidate(dir, entry, dentry);
		if (rc == 1 && agl_should_run(sai, d_inode(dentry)))
			ll_agl_add(sai, d_inode(dentry), entry->se_index);
	}

	if (dentry)
		dput(dentry);

	if (rc)
		sa_make_ready(sai, entry, rc);
	else
		sai->sai_sent++;

	sai->sai_index++;
}

/* async glimpse (agl) thread main function */
static int ll_agl_thread(void *arg)
{
	struct dentry	    *parent = arg;
	struct inode	     *dir    = d_inode(parent);
	struct ll_inode_info     *plli   = ll_i2info(dir);
	struct ll_inode_info     *clli;
	struct ll_sb_info	*sbi    = ll_i2sbi(dir);
	struct ll_statahead_info *sai;
	struct ptlrpc_thread *thread;
	struct l_wait_info	lwi    = { 0 };

	sai = ll_sai_get(dir);
	thread = &sai->sai_agl_thread;
	thread->t_pid = current_pid();
	CDEBUG(D_READA, "agl thread started: sai %p, parent %pd\n",
	       sai, parent);
>>>>>>> v4.9.227

	atomic_inc(&sbi->ll_agl_total);
	spin_lock(&plli->lli_agl_lock);
	sai->sai_agl_valid = 1;
	if (thread_is_init(thread))
		/* If someone else has changed the thread state
		 * (e.g. already changed to SVC_STOPPING), we can't just
<<<<<<< HEAD
		 * blindly overwrite that setting. */
=======
		 * blindly overwrite that setting.
		 */
>>>>>>> v4.9.227
		thread_set_flags(thread, SVC_RUNNING);
	spin_unlock(&plli->lli_agl_lock);
	wake_up(&thread->t_ctl_waitq);

	while (1) {
		l_wait_event(thread->t_ctl_waitq,
<<<<<<< HEAD
			     !agl_list_empty(sai) ||
=======
			     !list_empty(&sai->sai_agls) ||
>>>>>>> v4.9.227
			     !thread_is_running(thread),
			     &lwi);

		if (!thread_is_running(thread))
			break;

		spin_lock(&plli->lli_agl_lock);
		/* The statahead thread maybe help to process AGL entries,
<<<<<<< HEAD
		 * so check whether list empty again. */
		if (!agl_list_empty(sai)) {
			clli = agl_first_entry(sai);
=======
		 * so check whether list empty again.
		 */
		if (!list_empty(&sai->sai_agls)) {
			clli = list_entry(sai->sai_agls.next,
					  struct ll_inode_info, lli_agl_list);
>>>>>>> v4.9.227
			list_del_init(&clli->lli_agl_list);
			spin_unlock(&plli->lli_agl_lock);
			ll_agl_trigger(&clli->lli_vfs_inode, sai);
		} else {
			spin_unlock(&plli->lli_agl_lock);
		}
	}

	spin_lock(&plli->lli_agl_lock);
	sai->sai_agl_valid = 0;
<<<<<<< HEAD
	while (!agl_list_empty(sai)) {
		clli = agl_first_entry(sai);
=======
	while (!list_empty(&sai->sai_agls)) {
		clli = list_entry(sai->sai_agls.next,
				  struct ll_inode_info, lli_agl_list);
>>>>>>> v4.9.227
		list_del_init(&clli->lli_agl_list);
		spin_unlock(&plli->lli_agl_lock);
		clli->lli_agl_index = 0;
		iput(&clli->lli_vfs_inode);
		spin_lock(&plli->lli_agl_lock);
	}
	thread_set_flags(thread, SVC_STOPPED);
	spin_unlock(&plli->lli_agl_lock);
	wake_up(&thread->t_ctl_waitq);
	ll_sai_put(sai);
<<<<<<< HEAD
	CDEBUG(D_READA, "agl thread stopped: sai %p, parent %.*s\n",
	       sai, parent->d_name.len, parent->d_name.name);
	return 0;
}

=======
	CDEBUG(D_READA, "agl thread stopped: sai %p, parent %pd\n",
	       sai, parent);
	return 0;
}

/* start agl thread */
>>>>>>> v4.9.227
static void ll_start_agl(struct dentry *parent, struct ll_statahead_info *sai)
{
	struct ptlrpc_thread *thread = &sai->sai_agl_thread;
	struct l_wait_info    lwi    = { 0 };
	struct ll_inode_info  *plli;
	struct task_struct *task;

<<<<<<< HEAD
	CDEBUG(D_READA, "start agl thread: sai %p, parent %.*s\n",
	       sai, parent->d_name.len, parent->d_name.name);

	plli = ll_i2info(parent->d_inode);
	task = kthread_run(ll_agl_thread, parent,
			       "ll_agl_%u", plli->lli_opendir_pid);
=======
	CDEBUG(D_READA, "start agl thread: sai %p, parent %pd\n",
	       sai, parent);

	plli = ll_i2info(d_inode(parent));
	task = kthread_run(ll_agl_thread, parent, "ll_agl_%u",
			   plli->lli_opendir_pid);
>>>>>>> v4.9.227
	if (IS_ERR(task)) {
		CERROR("can't start ll_agl thread, rc: %ld\n", PTR_ERR(task));
		thread_set_flags(thread, SVC_STOPPED);
		return;
	}

	l_wait_event(thread->t_ctl_waitq,
		     thread_is_running(thread) || thread_is_stopped(thread),
		     &lwi);
}

<<<<<<< HEAD
static int ll_statahead_thread(void *arg)
{
	struct dentry	    *parent = (struct dentry *)arg;
	struct inode	     *dir    = parent->d_inode;
	struct ll_inode_info     *plli   = ll_i2info(dir);
	struct ll_inode_info     *clli;
	struct ll_sb_info	*sbi    = ll_i2sbi(dir);
	struct ll_statahead_info *sai    = ll_sai_get(plli->lli_sai);
	struct ptlrpc_thread     *thread = &sai->sai_thread;
	struct ptlrpc_thread *agl_thread = &sai->sai_agl_thread;
	struct page	      *page;
	__u64		     pos    = 0;
	int		       first  = 0;
	int		       rc     = 0;
	struct ll_dir_chain       chain;
	struct l_wait_info	lwi    = { 0 };

	thread->t_pid = current_pid();
	CDEBUG(D_READA, "statahead thread starting: sai %p, parent %.*s\n",
	       sai, parent->d_name.len, parent->d_name.name);
=======
/* statahead thread main function */
static int ll_statahead_thread(void *arg)
{
	struct dentry	    *parent = arg;
	struct inode	     *dir    = d_inode(parent);
	struct ll_inode_info     *lli   = ll_i2info(dir);
	struct ll_sb_info	*sbi    = ll_i2sbi(dir);
	struct ll_statahead_info *sai;
	struct ptlrpc_thread *sa_thread;
	struct ptlrpc_thread *agl_thread;
	struct page	      *page = NULL;
	__u64		     pos    = 0;
	int		       first  = 0;
	int		       rc     = 0;
	struct md_op_data *op_data;
	struct l_wait_info	lwi    = { 0 };

	sai = ll_sai_get(dir);
	sa_thread = &sai->sai_thread;
	agl_thread = &sai->sai_agl_thread;
	sa_thread->t_pid = current_pid();
	CDEBUG(D_READA, "statahead thread starting: sai %p, parent %pd\n",
	       sai, parent);

	op_data = ll_prep_md_op_data(NULL, dir, dir, NULL, 0, 0,
				     LUSTRE_OPC_ANY, dir);
	if (IS_ERR(op_data)) {
		rc = PTR_ERR(op_data);
		goto out;
	}

	op_data->op_max_pages = ll_i2sbi(dir)->ll_md_brw_pages;
>>>>>>> v4.9.227

	if (sbi->ll_flags & LL_SBI_AGL_ENABLED)
		ll_start_agl(parent, sai);

	atomic_inc(&sbi->ll_sa_total);
<<<<<<< HEAD
	spin_lock(&plli->lli_sa_lock);
	if (thread_is_init(thread))
		/* If someone else has changed the thread state
		 * (e.g. already changed to SVC_STOPPING), we can't just
		 * blindly overwrite that setting. */
		thread_set_flags(thread, SVC_RUNNING);
	spin_unlock(&plli->lli_sa_lock);
	wake_up(&thread->t_ctl_waitq);

	ll_dir_chain_init(&chain);
	page = ll_get_dir_page(dir, pos, &chain);

	while (1) {
		struct lu_dirpage *dp;
		struct lu_dirent  *ent;

		if (IS_ERR(page)) {
			rc = PTR_ERR(page);
			CDEBUG(D_READA, "error reading dir "DFID" at %llu/%llu: [rc %d] [parent %u]\n",
			       PFID(ll_inode2fid(dir)), pos, sai->sai_index,
			       rc, plli->lli_opendir_pid);
			goto out;
		}

		dp = page_address(page);
		for (ent = lu_dirent_start(dp); ent != NULL;
=======
	spin_lock(&lli->lli_sa_lock);
	if (thread_is_init(sa_thread))
		/* If someone else has changed the thread state
		 * (e.g. already changed to SVC_STOPPING), we can't just
		 * blindly overwrite that setting.
		 */
		thread_set_flags(sa_thread, SVC_RUNNING);
	spin_unlock(&lli->lli_sa_lock);
	wake_up(&sa_thread->t_ctl_waitq);

	while (pos != MDS_DIR_END_OFF && thread_is_running(sa_thread)) {
		struct lu_dirpage *dp;
		struct lu_dirent  *ent;

		sai->sai_in_readpage = 1;
		page = ll_get_dir_page(dir, op_data, pos);
		sai->sai_in_readpage = 0;
		if (IS_ERR(page)) {
			rc = PTR_ERR(page);
			CDEBUG(D_READA, "error reading dir "DFID" at %llu/%llu: opendir_pid = %u: rc = %d\n",
			       PFID(ll_inode2fid(dir)), pos, sai->sai_index,
			       lli->lli_opendir_pid, rc);
			break;
		}

		dp = page_address(page);
		for (ent = lu_dirent_start(dp);
		     ent && thread_is_running(sa_thread) && !sa_low_hit(sai);
>>>>>>> v4.9.227
		     ent = lu_dirent_next(ent)) {
			__u64 hash;
			int namelen;
			char *name;

			hash = le64_to_cpu(ent->lde_hash);
			if (unlikely(hash < pos))
				/*
				 * Skip until we find target hash value.
				 */
				continue;

			namelen = le16_to_cpu(ent->lde_namelen);
			if (unlikely(namelen == 0))
				/*
				 * Skip dummy record.
				 */
				continue;

			name = ent->lde_name;
			if (name[0] == '.') {
				if (namelen == 1) {
					/*
					 * skip "."
					 */
					continue;
				} else if (name[1] == '.' && namelen == 2) {
					/*
					 * skip ".."
					 */
					continue;
				} else if (!sai->sai_ls_all) {
					/*
					 * skip hidden files.
					 */
					sai->sai_skip_hidden++;
					continue;
				}
			}

			/*
			 * don't stat-ahead first entry.
			 */
			if (unlikely(++first == 1))
				continue;

<<<<<<< HEAD
keep_it:
			l_wait_event(thread->t_ctl_waitq,
				     !sa_sent_full(sai) ||
				     !sa_received_empty(sai) ||
				     !agl_list_empty(sai) ||
				     !thread_is_running(thread),
				     &lwi);

interpret_it:
			while (!sa_received_empty(sai))
				ll_post_statahead(sai);

			if (unlikely(!thread_is_running(thread))) {
				ll_release_page(page, 0);
				rc = 0;
				goto out;
			}

			/* If no window for metadata statahead, but there are
			 * some AGL entries to be triggered, then try to help
			 * to process the AGL entries. */
			if (sa_sent_full(sai)) {
				spin_lock(&plli->lli_agl_lock);
				while (!agl_list_empty(sai)) {
					clli = agl_first_entry(sai);
					list_del_init(&clli->lli_agl_list);
					spin_unlock(&plli->lli_agl_lock);
					ll_agl_trigger(&clli->lli_vfs_inode,
						       sai);

					if (!sa_received_empty(sai))
						goto interpret_it;

					if (unlikely(
						!thread_is_running(thread))) {
						ll_release_page(page, 0);
						rc = 0;
						goto out;
					}

					if (!sa_sent_full(sai))
						goto do_it;

					spin_lock(&plli->lli_agl_lock);
				}
				spin_unlock(&plli->lli_agl_lock);

				goto keep_it;
			}

do_it:
			ll_statahead_one(parent, name, namelen);
		}
		pos = le64_to_cpu(dp->ldp_hash_end);
		if (pos == MDS_DIR_END_OFF) {
			/*
			 * End of directory reached.
			 */
			ll_release_page(page, 0);
			while (1) {
				l_wait_event(thread->t_ctl_waitq,
					     !sa_received_empty(sai) ||
					     sai->sai_sent == sai->sai_replied||
					     !thread_is_running(thread),
					     &lwi);

				while (!sa_received_empty(sai))
					ll_post_statahead(sai);

				if (unlikely(!thread_is_running(thread))) {
					rc = 0;
					goto out;
				}

				if (sai->sai_sent == sai->sai_replied &&
				    sa_received_empty(sai))
					break;
			}

			spin_lock(&plli->lli_agl_lock);
			while (!agl_list_empty(sai) &&
			       thread_is_running(thread)) {
				clli = agl_first_entry(sai);
				list_del_init(&clli->lli_agl_list);
				spin_unlock(&plli->lli_agl_lock);
				ll_agl_trigger(&clli->lli_vfs_inode, sai);
				spin_lock(&plli->lli_agl_lock);
			}
			spin_unlock(&plli->lli_agl_lock);

			rc = 0;
			goto out;
		} else if (1) {
			/*
			 * chain is exhausted.
			 * Normal case: continue to the next page.
			 */
			ll_release_page(page, le32_to_cpu(dp->ldp_flags) &
					      LDF_COLLIDE);
			page = ll_get_dir_page(dir, pos, &chain);
		} else {
			LASSERT(le32_to_cpu(dp->ldp_flags) & LDF_COLLIDE);
			ll_release_page(page, 1);
			/*
			 * go into overflow page.
			 */
		}
	}

out:
	if (sai->sai_agl_valid) {
		spin_lock(&plli->lli_agl_lock);
		thread_set_flags(agl_thread, SVC_STOPPING);
		spin_unlock(&plli->lli_agl_lock);
=======
			/* wait for spare statahead window */
			do {
				l_wait_event(sa_thread->t_ctl_waitq,
					     !sa_sent_full(sai) ||
					     sa_has_callback(sai) ||
					     !list_empty(&sai->sai_agls) ||
					     !thread_is_running(sa_thread),
					     &lwi);
				sa_handle_callback(sai);

				spin_lock(&lli->lli_agl_lock);
				while (sa_sent_full(sai) &&
				       !agl_list_empty(sai)) {
					struct ll_inode_info *clli;

					clli = list_entry(sai->sai_agls.next,
							  struct ll_inode_info, lli_agl_list);
					list_del_init(&clli->lli_agl_list);
					spin_unlock(&lli->lli_agl_lock);

					ll_agl_trigger(&clli->lli_vfs_inode,
						       sai);

					spin_lock(&lli->lli_agl_lock);
				}
				spin_unlock(&lli->lli_agl_lock);
			} while (sa_sent_full(sai) &&
				 thread_is_running(sa_thread));

			sa_statahead(parent, name, namelen);
		}

		pos = le64_to_cpu(dp->ldp_hash_end);
		ll_release_page(dir, page,
				le32_to_cpu(dp->ldp_flags) & LDF_COLLIDE);

		if (sa_low_hit(sai)) {
			rc = -EFAULT;
			atomic_inc(&sbi->ll_sa_wrong);
			CDEBUG(D_READA, "Statahead for dir "DFID" hit ratio too low: hit/miss %llu/%llu, sent/replied %llu/%llu, stopping statahead thread: pid %d\n",
			       PFID(&lli->lli_fid), sai->sai_hit,
			       sai->sai_miss, sai->sai_sent,
			       sai->sai_replied, current_pid());
			break;
		}
	}
	ll_finish_md_op_data(op_data);

	if (rc < 0) {
		spin_lock(&lli->lli_sa_lock);
		thread_set_flags(sa_thread, SVC_STOPPING);
		lli->lli_sa_enabled = 0;
		spin_unlock(&lli->lli_sa_lock);
	}

	/*
	 * statahead is finished, but statahead entries need to be cached, wait
	 * for file release to stop me.
	 */
	while (thread_is_running(sa_thread)) {
		l_wait_event(sa_thread->t_ctl_waitq,
			     sa_has_callback(sai) ||
			     !agl_list_empty(sai) ||
			     !thread_is_running(sa_thread),
			     &lwi);

		sa_handle_callback(sai);
	}
out:
	if (sai->sai_agl_valid) {
		spin_lock(&lli->lli_agl_lock);
		thread_set_flags(agl_thread, SVC_STOPPING);
		spin_unlock(&lli->lli_agl_lock);
>>>>>>> v4.9.227
		wake_up(&agl_thread->t_ctl_waitq);

		CDEBUG(D_READA, "stop agl thread: sai %p pid %u\n",
		       sai, (unsigned int)agl_thread->t_pid);
		l_wait_event(agl_thread->t_ctl_waitq,
			     thread_is_stopped(agl_thread),
			     &lwi);
	} else {
		/* Set agl_thread flags anyway. */
<<<<<<< HEAD
		thread_set_flags(&sai->sai_agl_thread, SVC_STOPPED);
	}
	ll_dir_chain_fini(&chain);
	spin_lock(&plli->lli_sa_lock);
	if (!sa_received_empty(sai)) {
		thread_set_flags(thread, SVC_STOPPING);
		spin_unlock(&plli->lli_sa_lock);

		/* To release the resources held by received entries. */
		while (!sa_received_empty(sai))
			ll_post_statahead(sai);

		spin_lock(&plli->lli_sa_lock);
	}
	thread_set_flags(thread, SVC_STOPPED);
	spin_unlock(&plli->lli_sa_lock);
	wake_up(&sai->sai_waitq);
	wake_up(&thread->t_ctl_waitq);
	ll_sai_put(sai);
	dput(parent);
	CDEBUG(D_READA, "statahead thread stopped: sai %p, parent %.*s\n",
	       sai, parent->d_name.len, parent->d_name.name);
	return rc;
}

/**
 * called in ll_file_release().
 */
void ll_stop_statahead(struct inode *dir, void *key)
{
	struct ll_inode_info *lli = ll_i2info(dir);

	if (unlikely(key == NULL))
		return;

	spin_lock(&lli->lli_sa_lock);
	if (lli->lli_opendir_key != key || lli->lli_opendir_pid == 0) {
		spin_unlock(&lli->lli_sa_lock);
		return;
	}

	lli->lli_opendir_key = NULL;

	if (lli->lli_sai) {
		struct l_wait_info lwi = { 0 };
		struct ptlrpc_thread *thread = &lli->lli_sai->sai_thread;

		if (!thread_is_stopped(thread)) {
			thread_set_flags(thread, SVC_STOPPING);
			spin_unlock(&lli->lli_sa_lock);
			wake_up(&thread->t_ctl_waitq);

			CDEBUG(D_READA, "stop statahead thread: sai %p pid %u\n",
			       lli->lli_sai, (unsigned int)thread->t_pid);
			l_wait_event(thread->t_ctl_waitq,
				     thread_is_stopped(thread),
				     &lwi);
		} else {
			spin_unlock(&lli->lli_sa_lock);
		}

		/*
		 * Put the ref which was held when first statahead_enter.
		 * It maybe not the last ref for some statahead requests
		 * maybe inflight.
		 */
		ll_sai_put(lli->lli_sai);
	} else {
		lli->lli_opendir_pid = 0;
		spin_unlock(&lli->lli_sa_lock);
	}
=======
		thread_set_flags(agl_thread, SVC_STOPPED);
	}

	/*
	 * wait for inflight statahead RPCs to finish, and then we can free sai
	 * safely because statahead RPC will access sai data
	 */
	while (sai->sai_sent != sai->sai_replied) {
		/* in case we're not woken up, timeout wait */
		lwi = LWI_TIMEOUT(msecs_to_jiffies(MSEC_PER_SEC >> 3),
				  NULL, NULL);
		l_wait_event(sa_thread->t_ctl_waitq,
			     sai->sai_sent == sai->sai_replied, &lwi);
	}

	/* release resources held by statahead RPCs */
	sa_handle_callback(sai);

	spin_lock(&lli->lli_sa_lock);
	thread_set_flags(sa_thread, SVC_STOPPED);
	spin_unlock(&lli->lli_sa_lock);

	CDEBUG(D_READA, "statahead thread stopped: sai %p, parent %pd\n",
	       sai, parent);

	wake_up(&sai->sai_waitq);
	wake_up(&sa_thread->t_ctl_waitq);
	ll_sai_put(sai);

	return rc;
}

/* authorize opened dir handle @key to statahead */
void ll_authorize_statahead(struct inode *dir, void *key)
{
	struct ll_inode_info *lli = ll_i2info(dir);

	spin_lock(&lli->lli_sa_lock);
	if (!lli->lli_opendir_key && !lli->lli_sai) {
		/*
		 * if lli_sai is not NULL, it means previous statahead is not
		 * finished yet, we'd better not start a new statahead for now.
		 */
		LASSERT(!lli->lli_opendir_pid);
		lli->lli_opendir_key = key;
		lli->lli_opendir_pid = current_pid();
		lli->lli_sa_enabled = 1;
	}
	spin_unlock(&lli->lli_sa_lock);
}

/*
 * deauthorize opened dir handle @key to statahead, but statahead thread may
 * still be running, notify it to quit.
 */
void ll_deauthorize_statahead(struct inode *dir, void *key)
{
	struct ll_inode_info *lli = ll_i2info(dir);
	struct ll_statahead_info *sai;

	LASSERT(lli->lli_opendir_key == key);
	LASSERT(lli->lli_opendir_pid);

	CDEBUG(D_READA, "deauthorize statahead for "DFID"\n",
	       PFID(&lli->lli_fid));

	spin_lock(&lli->lli_sa_lock);
	lli->lli_opendir_key = NULL;
	lli->lli_opendir_pid = 0;
	lli->lli_sa_enabled = 0;
	sai = lli->lli_sai;
	if (sai && thread_is_running(&sai->sai_thread)) {
		/*
		 * statahead thread may not quit yet because it needs to cache
		 * entries, now it's time to tell it to quit.
		 */
		thread_set_flags(&sai->sai_thread, SVC_STOPPING);
		wake_up(&sai->sai_thread.t_ctl_waitq);
	}
	spin_unlock(&lli->lli_sa_lock);
>>>>>>> v4.9.227
}

enum {
	/**
	 * not first dirent, or is "."
	 */
<<<<<<< HEAD
	LS_NONE_FIRST_DE = 0,
=======
	LS_NOT_FIRST_DE = 0,
>>>>>>> v4.9.227
	/**
	 * the first non-hidden dirent
	 */
	LS_FIRST_DE,
	/**
	 * the first hidden dirent, that is "."
	 */
	LS_FIRST_DOT_DE
};

<<<<<<< HEAD
static int is_first_dirent(struct inode *dir, struct dentry *dentry)
{
	struct ll_dir_chain   chain;
	struct qstr	  *target = &dentry->d_name;
	struct page	  *page;
	__u64		 pos    = 0;
	int		   dot_de;
	int		   rc     = LS_NONE_FIRST_DE;

	ll_dir_chain_init(&chain);
	page = ll_get_dir_page(dir, pos, &chain);
=======
/* file is first dirent under @dir */
static int is_first_dirent(struct inode *dir, struct dentry *dentry)
{
	const struct qstr  *target = &dentry->d_name;
	struct md_op_data *op_data;
	struct page	  *page;
	__u64		 pos    = 0;
	int		   dot_de;
	int rc = LS_NOT_FIRST_DE;

	op_data = ll_prep_md_op_data(NULL, dir, dir, NULL, 0, 0,
				     LUSTRE_OPC_ANY, dir);
	if (IS_ERR(op_data))
		return PTR_ERR(op_data);
	/**
	 * FIXME choose the start offset of the readdir
	 */
	op_data->op_max_pages = ll_i2sbi(dir)->ll_md_brw_pages;

	page = ll_get_dir_page(dir, op_data, pos);
>>>>>>> v4.9.227

	while (1) {
		struct lu_dirpage *dp;
		struct lu_dirent  *ent;

		if (IS_ERR(page)) {
			struct ll_inode_info *lli = ll_i2info(dir);

			rc = PTR_ERR(page);
<<<<<<< HEAD
			CERROR("error reading dir "DFID" at %llu: [rc %d] [parent %u]\n",
			       PFID(ll_inode2fid(dir)), pos,
			       rc, lli->lli_opendir_pid);
=======
			CERROR("%s: error reading dir "DFID" at %llu: opendir_pid = %u : rc = %d\n",
			       ll_get_fsname(dir->i_sb, NULL, 0),
			       PFID(ll_inode2fid(dir)), pos,
			       lli->lli_opendir_pid, rc);
>>>>>>> v4.9.227
			break;
		}

		dp = page_address(page);
<<<<<<< HEAD
		for (ent = lu_dirent_start(dp); ent != NULL;
=======
		for (ent = lu_dirent_start(dp); ent;
>>>>>>> v4.9.227
		     ent = lu_dirent_next(ent)) {
			__u64 hash;
			int namelen;
			char *name;

			hash = le64_to_cpu(ent->lde_hash);
			/* The ll_get_dir_page() can return any page containing
<<<<<<< HEAD
			 * the given hash which may be not the start hash. */
=======
			 * the given hash which may be not the start hash.
			 */
>>>>>>> v4.9.227
			if (unlikely(hash < pos))
				continue;

			namelen = le16_to_cpu(ent->lde_namelen);
			if (unlikely(namelen == 0))
				/*
				 * skip dummy record.
				 */
				continue;

			name = ent->lde_name;
			if (name[0] == '.') {
				if (namelen == 1)
					/*
					 * skip "."
					 */
					continue;
				else if (name[1] == '.' && namelen == 2)
					/*
					 * skip ".."
					 */
					continue;
				else
					dot_de = 1;
			} else {
				dot_de = 0;
			}

			if (dot_de && target->name[0] != '.') {
				CDEBUG(D_READA, "%.*s skip hidden file %.*s\n",
				       target->len, target->name,
				       namelen, name);
				continue;
			}

			if (target->len != namelen ||
			    memcmp(target->name, name, namelen) != 0)
<<<<<<< HEAD
				rc = LS_NONE_FIRST_DE;
=======
				rc = LS_NOT_FIRST_DE;
>>>>>>> v4.9.227
			else if (!dot_de)
				rc = LS_FIRST_DE;
			else
				rc = LS_FIRST_DOT_DE;

<<<<<<< HEAD
			ll_release_page(page, 0);
=======
			ll_release_page(dir, page, false);
>>>>>>> v4.9.227
			goto out;
		}
		pos = le64_to_cpu(dp->ldp_hash_end);
		if (pos == MDS_DIR_END_OFF) {
			/*
			 * End of directory reached.
			 */
<<<<<<< HEAD
			ll_release_page(page, 0);
			break;
		} else if (1) {
=======
			ll_release_page(dir, page, false);
			goto out;
		} else {
>>>>>>> v4.9.227
			/*
			 * chain is exhausted
			 * Normal case: continue to the next page.
			 */
<<<<<<< HEAD
			ll_release_page(page, le32_to_cpu(dp->ldp_flags) &
					      LDF_COLLIDE);
			page = ll_get_dir_page(dir, pos, &chain);
		} else {
			/*
			 * go into overflow page.
			 */
			LASSERT(le32_to_cpu(dp->ldp_flags) & LDF_COLLIDE);
			ll_release_page(page, 1);
		}
	}

out:
	ll_dir_chain_fini(&chain);
	return rc;
}

static void
ll_sai_unplug(struct ll_statahead_info *sai, struct ll_sa_entry *entry)
{
	struct ptlrpc_thread *thread = &sai->sai_thread;
	struct ll_sb_info    *sbi    = ll_i2sbi(sai->sai_inode);
	int		   hit;

	if (entry != NULL && entry->se_stat == SA_ENTRY_SUCC)
		hit = 1;
	else
		hit = 0;

	ll_sa_entry_fini(sai, entry);
	if (hit) {
		sai->sai_hit++;
		sai->sai_consecutive_miss = 0;
		sai->sai_max = min(2 * sai->sai_max, sbi->ll_sa_max);
	} else {
		struct ll_inode_info *lli = ll_i2info(sai->sai_inode);

		sai->sai_miss++;
		sai->sai_consecutive_miss++;
		if (sa_low_hit(sai) && thread_is_running(thread)) {
			atomic_inc(&sbi->ll_sa_wrong);
			CDEBUG(D_READA, "Statahead for dir "DFID" hit "
			       "ratio too low: hit/miss %llu/%llu"
			       ", sent/replied %llu/%llu, stopping "
			       "statahead thread\n",
			       PFID(&lli->lli_fid), sai->sai_hit,
			       sai->sai_miss, sai->sai_sent,
			       sai->sai_replied);
			spin_lock(&lli->lli_sa_lock);
			if (!thread_is_stopped(thread))
				thread_set_flags(thread, SVC_STOPPING);
			spin_unlock(&lli->lli_sa_lock);
		}
	}

	if (!thread_is_stopped(thread))
		wake_up(&thread->t_ctl_waitq);
}

/**
 * Start statahead thread if this is the first dir entry.
 * Otherwise if a thread is started already, wait it until it is ahead of me.
 * \retval 1       -- find entry with lock in cache, the caller needs to do
 *		    nothing.
 * \retval 0       -- find entry in cache, but without lock, the caller needs
 *		    refresh from MDS.
 * \retval others  -- the caller need to process as non-statahead.
 */
int do_statahead_enter(struct inode *dir, struct dentry **dentryp,
		       int only_unplug)
{
	struct ll_inode_info     *lli   = ll_i2info(dir);
	struct ll_statahead_info *sai   = lli->lli_sai;
	struct dentry	    *parent;
	struct ll_sa_entry       *entry;
	struct ptlrpc_thread     *thread;
	struct l_wait_info	lwi   = { 0 };
	int		       rc    = 0;
	struct ll_inode_info     *plli;

	LASSERT(lli->lli_opendir_pid == current_pid());

	if (sai) {
		thread = &sai->sai_thread;
		if (unlikely(thread_is_stopped(thread) &&
			     list_empty(&sai->sai_entries_stated))) {
			/* to release resource */
			ll_stop_statahead(dir, lli->lli_opendir_key);
			return -EAGAIN;
		}

		if ((*dentryp)->d_name.name[0] == '.') {
			if (sai->sai_ls_all ||
			    sai->sai_miss_hidden >= sai->sai_skip_hidden) {
				/*
				 * Hidden dentry is the first one, or statahead
				 * thread does not skip so many hidden dentries
				 * before "sai_ls_all" enabled as below.
				 */
			} else {
				if (!sai->sai_ls_all)
					/*
					 * It maybe because hidden dentry is not
					 * the first one, "sai_ls_all" was not
					 * set, then "ls -al" missed. Enable
					 * "sai_ls_all" for such case.
					 */
					sai->sai_ls_all = 1;

				/*
				 * Such "getattr" has been skipped before
				 * "sai_ls_all" enabled as above.
				 */
				sai->sai_miss_hidden++;
				return -EAGAIN;
			}
		}

		entry = ll_sa_entry_get_byname(sai, &(*dentryp)->d_name);
		if (entry == NULL || only_unplug) {
			ll_sai_unplug(sai, entry);
			return entry ? 1 : -EAGAIN;
		}

		if (!ll_sa_entry_stated(entry)) {
			sai->sai_index_wait = entry->se_index;
			lwi = LWI_TIMEOUT_INTR(cfs_time_seconds(30), NULL,
					       LWI_ON_SIGNAL_NOOP, NULL);
			rc = l_wait_event(sai->sai_waitq,
					  ll_sa_entry_stated(entry) ||
					  thread_is_stopped(thread),
					  &lwi);
			if (rc < 0) {
				ll_sai_unplug(sai, entry);
				return -EAGAIN;
			}
		}

		if (entry->se_stat == SA_ENTRY_SUCC &&
		    entry->se_inode != NULL) {
			struct inode *inode = entry->se_inode;
			struct lookup_intent it = { .it_op = IT_GETATTR,
						    .d.lustre.it_lock_handle =
						     entry->se_handle };
			__u64 bits;

			rc = md_revalidate_lock(ll_i2mdexp(dir), &it,
						ll_inode2fid(inode), &bits);
			if (rc == 1) {
				if ((*dentryp)->d_inode == NULL) {
					struct dentry *alias;

					alias = ll_splice_alias(inode,
								   *dentryp);
					if (IS_ERR(alias)) {
						ll_sai_unplug(sai, entry);
						return PTR_ERR(alias);
					}
					*dentryp = alias;
				} else if ((*dentryp)->d_inode != inode) {
					/* revalidate, but inode is recreated */
					CDEBUG(D_READA,
					      "stale dentry %.*s inode %lu/%u, "
					      "statahead inode %lu/%u\n",
					      (*dentryp)->d_name.len,
					      (*dentryp)->d_name.name,
					      (*dentryp)->d_inode->i_ino,
					      (*dentryp)->d_inode->i_generation,
					      inode->i_ino,
					      inode->i_generation);
					ll_sai_unplug(sai, entry);
					return -ESTALE;
				} else {
					iput(inode);
				}
				entry->se_inode = NULL;

				if ((bits & MDS_INODELOCK_LOOKUP) &&
				    d_lustre_invalid(*dentryp))
					d_lustre_revalidate(*dentryp);
				ll_intent_release(&it);
			}
		}

		ll_sai_unplug(sai, entry);
		return rc;
	}

	/* I am the "lli_opendir_pid" owner, only me can set "lli_sai". */
	rc = is_first_dirent(dir, *dentryp);
	if (rc == LS_NONE_FIRST_DE) {
		/* It is not "ls -{a}l" operation, no need statahead for it. */
		rc = -EAGAIN;
		goto out;
	}

	sai = ll_sai_alloc();
	if (sai == NULL) {
=======
			ll_release_page(dir, page,
					le32_to_cpu(dp->ldp_flags) &
					LDF_COLLIDE);
			page = ll_get_dir_page(dir, op_data, pos);
		}
	}
out:
	ll_finish_md_op_data(op_data);
	return rc;
}

/**
 * revalidate @dentryp from statahead cache
 *
 * \param[in]  dir	parent directory
 * \param[in]  sai	sai structure
 * \param[out] dentryp	pointer to dentry which will be revalidated
 * \param[in]  unplug	unplug statahead window only (normally for negative
 *			dentry)
 * \retval		1 on success, dentry is saved in @dentryp
 * \retval		0 if revalidation failed (no proper lock on client)
 * \retval		negative number upon error
 */
static int revalidate_statahead_dentry(struct inode *dir,
				       struct ll_statahead_info *sai,
				       struct dentry **dentryp,
				       bool unplug)
{
	struct sa_entry *entry = NULL;
	struct l_wait_info lwi = { 0 };
	struct ll_dentry_data *ldd;
	struct ll_inode_info *lli;
	int rc = 0;

	if ((*dentryp)->d_name.name[0] == '.') {
		if (sai->sai_ls_all ||
		    sai->sai_miss_hidden >= sai->sai_skip_hidden) {
			/*
			 * Hidden dentry is the first one, or statahead
			 * thread does not skip so many hidden dentries
			 * before "sai_ls_all" enabled as below.
			 */
		} else {
			if (!sai->sai_ls_all)
				/*
				 * It maybe because hidden dentry is not
				 * the first one, "sai_ls_all" was not
				 * set, then "ls -al" missed. Enable
				 * "sai_ls_all" for such case.
				 */
				sai->sai_ls_all = 1;

			/*
			 * Such "getattr" has been skipped before
			 * "sai_ls_all" enabled as above.
			 */
			sai->sai_miss_hidden++;
			return -EAGAIN;
		}
	}

	if (unplug) {
		rc = 1;
		goto out_unplug;
	}

	entry = sa_get(sai, &(*dentryp)->d_name);
	if (!entry) {
		rc = -EAGAIN;
		goto out_unplug;
	}

	/* if statahead is busy in readdir, help it do post-work */
	if (!sa_ready(entry) && sai->sai_in_readpage)
		sa_handle_callback(sai);

	if (!sa_ready(entry)) {
		sai->sai_index_wait = entry->se_index;
		lwi = LWI_TIMEOUT_INTR(cfs_time_seconds(30), NULL,
				       LWI_ON_SIGNAL_NOOP, NULL);
		rc = l_wait_event(sai->sai_waitq, sa_ready(entry), &lwi);
		if (rc < 0) {
			/*
			 * entry may not be ready, so it may be used by inflight
			 * statahead RPC, don't free it.
			 */
			entry = NULL;
			rc = -EAGAIN;
			goto out_unplug;
		}
	}

	if (entry->se_state == SA_ENTRY_SUCC && entry->se_inode) {
		struct inode *inode = entry->se_inode;
		struct lookup_intent it = { .it_op = IT_GETATTR,
					    .it_lock_handle = entry->se_handle };
		__u64 bits;

		rc = md_revalidate_lock(ll_i2mdexp(dir), &it,
					ll_inode2fid(inode), &bits);
		if (rc == 1) {
			if (!(*dentryp)->d_inode) {
				struct dentry *alias;

				alias = ll_splice_alias(inode, *dentryp);
				if (IS_ERR(alias)) {
					rc = PTR_ERR(alias);
					goto out_unplug;
				}
				*dentryp = alias;
				/**
				 * statahead prepared this inode, transfer inode
				 * refcount from sa_entry to dentry
				 */
				entry->se_inode = NULL;
			} else if ((*dentryp)->d_inode != inode) {
				/* revalidate, but inode is recreated */
				CDEBUG(D_READA,
				       "%s: stale dentry %pd inode "DFID", statahead inode "DFID"\n",
				       ll_get_fsname((*dentryp)->d_inode->i_sb,
						     NULL, 0),
				       *dentryp,
				       PFID(ll_inode2fid((*dentryp)->d_inode)),
				       PFID(ll_inode2fid(inode)));
				rc = -ESTALE;
				goto out_unplug;
			}

			if ((bits & MDS_INODELOCK_LOOKUP) &&
			    d_lustre_invalid(*dentryp))
				d_lustre_revalidate(*dentryp);
			ll_intent_release(&it);
		}
	}
out_unplug:
	/*
	 * statahead cached sa_entry can be used only once, and will be killed
	 * right after use, so if lookup/revalidate accessed statahead cache,
	 * set dentry ldd_sa_generation to parent lli_sa_generation, later if we
	 * stat this file again, we know we've done statahead before, see
	 * dentry_may_statahead().
	 */
	ldd = ll_d2d(*dentryp);
	lli = ll_i2info(dir);
	/* ldd can be NULL if llite lookup failed. */
	if (ldd)
		ldd->lld_sa_generation = lli->lli_sa_generation;
	sa_put(sai, entry);
	return rc;
}

/**
 * start statahead thread
 *
 * \param[in] dir	parent directory
 * \param[in] dentry	dentry that triggers statahead, normally the first
 *			dirent under @dir
 * \retval		-EAGAIN on success, because when this function is
 *			called, it's already in lookup call, so client should
 *			do it itself instead of waiting for statahead thread
 *			to do it asynchronously.
 * \retval		negative number upon error
 */
static int start_statahead_thread(struct inode *dir, struct dentry *dentry)
{
	struct ll_inode_info *lli = ll_i2info(dir);
	struct ll_statahead_info *sai = NULL;
	struct l_wait_info lwi = { 0 };
	struct ptlrpc_thread *thread;
	struct task_struct *task;
	struct dentry *parent = dentry->d_parent;
	int rc;

	/* I am the "lli_opendir_pid" owner, only me can set "lli_sai". */
	rc = is_first_dirent(dir, dentry);
	if (rc == LS_NOT_FIRST_DE) {
		/* It is not "ls -{a}l" operation, no need statahead for it. */
		rc = -EFAULT;
		goto out;
	}

	sai = ll_sai_alloc(parent);
	if (!sai) {
>>>>>>> v4.9.227
		rc = -ENOMEM;
		goto out;
	}

	sai->sai_ls_all = (rc == LS_FIRST_DOT_DE);
<<<<<<< HEAD
	sai->sai_inode = igrab(dir);
	if (unlikely(sai->sai_inode == NULL)) {
		CWARN("Do not start stat ahead on dying inode "DFID"\n",
		      PFID(&lli->lli_fid));
		rc = -ESTALE;
		goto out;
	}

	/* get parent reference count here, and put it in ll_statahead_thread */
	parent = dget((*dentryp)->d_parent);
	if (unlikely(sai->sai_inode != parent->d_inode)) {
		struct ll_inode_info *nlli = ll_i2info(parent->d_inode);

		CWARN("Race condition, someone changed %.*s just now: "
		      "old parent "DFID", new parent "DFID"\n",
		      (*dentryp)->d_name.len, (*dentryp)->d_name.name,
		      PFID(&lli->lli_fid), PFID(&nlli->lli_fid));
		dput(parent);
		iput(sai->sai_inode);
		rc = -EAGAIN;
		goto out;
	}

	CDEBUG(D_READA, "start statahead thread: sai %p, parent %.*s\n",
	       sai, parent->d_name.len, parent->d_name.name);

	/* The sai buffer already has one reference taken at allocation time,
	 * but as soon as we expose the sai by attaching it to the lli that
	 * default reference can be dropped by another thread calling
	 * ll_stop_statahead. We need to take a local reference to protect
	 * the sai buffer while we intend to access it. */
	ll_sai_get(sai);
	lli->lli_sai = sai;

	plli = ll_i2info(parent->d_inode);
	rc = PTR_ERR(kthread_run(ll_statahead_thread, parent,
				 "ll_sa_%u", plli->lli_opendir_pid));
	thread = &sai->sai_thread;
	if (IS_ERR_VALUE(rc)) {
		CERROR("can't start ll_sa thread, rc: %d\n", rc);
		dput(parent);
		lli->lli_opendir_key = NULL;
		thread_set_flags(thread, SVC_STOPPED);
		thread_set_flags(&sai->sai_agl_thread, SVC_STOPPED);
		/* Drop both our own local reference and the default
		 * reference from allocation time. */
		ll_sai_put(sai);
		ll_sai_put(sai);
		LASSERT(lli->lli_sai == NULL);
		return -EAGAIN;
=======
	/*
	 * if current lli_opendir_key was deauthorized, or dir re-opened by
	 * another process, don't start statahead, otherwise the newly spawned
	 * statahead thread won't be notified to quit.
	 */
	spin_lock(&lli->lli_sa_lock);
	if (unlikely(lli->lli_sai || lli->lli_opendir_key ||
		     lli->lli_opendir_pid != current->pid)) {
		spin_unlock(&lli->lli_sa_lock);
		rc = -EPERM;
		goto out;
	}
	lli->lli_sai = sai;
	spin_unlock(&lli->lli_sa_lock);

	atomic_inc(&ll_i2sbi(parent->d_inode)->ll_sa_running);

	CDEBUG(D_READA, "start statahead thread: [pid %d] [parent %pd]\n",
	       current_pid(), parent);

	task = kthread_run(ll_statahead_thread, parent, "ll_sa_%u",
			   lli->lli_opendir_pid);
	thread = &sai->sai_thread;
	if (IS_ERR(task)) {
		rc = PTR_ERR(task);
		CERROR("can't start ll_sa thread, rc : %d\n", rc);
		goto out;
>>>>>>> v4.9.227
	}

	l_wait_event(thread->t_ctl_waitq,
		     thread_is_running(thread) || thread_is_stopped(thread),
		     &lwi);
	ll_sai_put(sai);

	/*
	 * We don't stat-ahead for the first dirent since we are already in
	 * lookup.
	 */
	return -EAGAIN;

out:
<<<<<<< HEAD
	if (sai != NULL)
		OBD_FREE_PTR(sai);
	spin_lock(&lli->lli_sa_lock);
	lli->lli_opendir_key = NULL;
	lli->lli_opendir_pid = 0;
	spin_unlock(&lli->lli_sa_lock);
	return rc;
}
=======
	/*
	 * once we start statahead thread failed, disable statahead so
	 * that subsequent stat won't waste time to try it.
	 */
	spin_lock(&lli->lli_sa_lock);
	lli->lli_sa_enabled = 0;
	lli->lli_sai = NULL;
	spin_unlock(&lli->lli_sa_lock);
	if (sai)
		ll_sai_free(sai);
	return rc;
}

/**
 * statahead entry function, this is called when client getattr on a file, it
 * will start statahead thread if this is the first dir entry, else revalidate
 * dentry from statahead cache.
 *
 * \param[in]  dir	parent directory
 * \param[out] dentryp	dentry to getattr
 * \param[in]  unplug	unplug statahead window only (normally for negative
 *			dentry)
 * \retval		1 on success
 * \retval		0 revalidation from statahead cache failed, caller needs
 *			to getattr from server directly
 * \retval		negative number on error, caller often ignores this and
 *			then getattr from server
 */
int ll_statahead(struct inode *dir, struct dentry **dentryp, bool unplug)
{
	struct ll_statahead_info *sai;

	sai = ll_sai_get(dir);
	if (sai) {
		int rc;

		rc = revalidate_statahead_dentry(dir, sai, dentryp, unplug);
		CDEBUG(D_READA, "revalidate statahead %pd: %d.\n",
		       *dentryp, rc);
		ll_sai_put(sai);
		return rc;
	}
	return start_statahead_thread(dir, *dentryp);
}
>>>>>>> v4.9.227
