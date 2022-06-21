/*
 * Resizable, Scalable, Concurrent Hash Table
 *
<<<<<<< HEAD
 * Copyright (c) 2014 Thomas Graf <tgraf@suug.ch>
 * Copyright (c) 2008-2014 Patrick McHardy <kaber@trash.net>
 *
 * Based on the following paper:
 * https://www.usenix.org/legacy/event/atc11/tech/final_files/Triplett.pdf
 *
 * Code partially derived from nft_hash
=======
 * Copyright (c) 2015 Herbert Xu <herbert@gondor.apana.org.au>
 * Copyright (c) 2014-2015 Thomas Graf <tgraf@suug.ch>
 * Copyright (c) 2008-2014 Patrick McHardy <kaber@trash.net>
 *
 * Code partially derived from nft_hash
 * Rewritten with rehash code from br_multicast plus single list
 * pointer as suggested by Josh Triplett
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

<<<<<<< HEAD
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/log2.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/hash.h>
#include <linux/random.h>
#include <linux/rhashtable.h>

#define HASH_DEFAULT_SIZE	64UL
#define HASH_MIN_SIZE		4UL

#define ASSERT_RHT_MUTEX(HT) BUG_ON(!lockdep_rht_mutex_is_held(HT))

#ifdef CONFIG_PROVE_LOCKING
int lockdep_rht_mutex_is_held(const struct rhashtable *ht)
{
	return ht->p.mutex_is_held();
}
EXPORT_SYMBOL_GPL(lockdep_rht_mutex_is_held);
#endif

static void *rht_obj(const struct rhashtable *ht, const struct rhash_head *he)
{
	return (void *) he - ht->p.head_offset;
}

static u32 __hashfn(const struct rhashtable *ht, const void *key,
		      u32 len, u32 hsize)
{
	u32 h;

	h = ht->p.hashfn(key, len, ht->p.hash_rnd);

	return h & (hsize - 1);
}

/**
 * rhashtable_hashfn - compute hash for key of given length
 * @ht:		hash table to compute for
 * @key:	pointer to key
 * @len:	length of key
 *
 * Computes the hash value using the hash function provided in the 'hashfn'
 * of struct rhashtable_params. The returned value is guaranteed to be
 * smaller than the number of buckets in the hash table.
 *
 * The caller must ensure that no concurrent table mutations occur.
 */
u32 rhashtable_hashfn(const struct rhashtable *ht, const void *key, u32 len)
{
	struct bucket_table *tbl = rht_dereference_rcu(ht->tbl, ht);

	return __hashfn(ht, key, len, tbl->size);
}
EXPORT_SYMBOL_GPL(rhashtable_hashfn);

static u32 obj_hashfn(const struct rhashtable *ht, const void *ptr, u32 hsize)
{
	if (unlikely(!ht->p.key_len)) {
		u32 h;

		h = ht->p.obj_hashfn(ptr, ht->p.hash_rnd);

		return h & (hsize - 1);
	}

	return __hashfn(ht, ptr + ht->p.key_offset, ht->p.key_len, hsize);
}

/**
 * rhashtable_obj_hashfn - compute hash for hashed object
 * @ht:		hash table to compute for
 * @ptr:	pointer to hashed object
 *
 * Computes the hash value using the hash function `hashfn` respectively
 * 'obj_hashfn' depending on whether the hash table is set up to work with
 * a fixed length key. The returned value is guaranteed to be smaller than
 * the number of buckets in the hash table.
 *
 * The caller must ensure that no concurrent table mutations occur.
 */
u32 rhashtable_obj_hashfn(const struct rhashtable *ht, void *ptr)
{
	struct bucket_table *tbl = rht_dereference_rcu(ht->tbl, ht);

	return obj_hashfn(ht, ptr, tbl->size);
}
EXPORT_SYMBOL_GPL(rhashtable_obj_hashfn);

static u32 head_hashfn(const struct rhashtable *ht,
		       const struct rhash_head *he, u32 hsize)
{
	return obj_hashfn(ht, rht_obj(ht, he), hsize);
}

static struct bucket_table *bucket_table_alloc(size_t nbuckets, gfp_t flags)
{
	struct bucket_table *tbl;
	size_t size;

	size = sizeof(*tbl) + nbuckets * sizeof(tbl->buckets[0]);
	tbl = kzalloc(size, flags);
	if (tbl == NULL)
		tbl = vzalloc(size);

=======
#include <linux/atomic.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/log2.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/jhash.h>
#include <linux/random.h>
#include <linux/rhashtable.h>
#include <linux/err.h>
#include <linux/export.h>

#define HASH_DEFAULT_SIZE	64UL
#define HASH_MIN_SIZE		4U
#define BUCKET_LOCKS_PER_CPU	32UL

static u32 head_hashfn(struct rhashtable *ht,
		       const struct bucket_table *tbl,
		       const struct rhash_head *he)
{
	return rht_head_hashfn(ht, tbl, he, ht->p);
}

#ifdef CONFIG_PROVE_LOCKING
#define ASSERT_RHT_MUTEX(HT) BUG_ON(!lockdep_rht_mutex_is_held(HT))

int lockdep_rht_mutex_is_held(struct rhashtable *ht)
{
	return (debug_locks) ? lockdep_is_held(&ht->mutex) : 1;
}
EXPORT_SYMBOL_GPL(lockdep_rht_mutex_is_held);

int lockdep_rht_bucket_is_held(const struct bucket_table *tbl, u32 hash)
{
	spinlock_t *lock = rht_bucket_lock(tbl, hash);

	return (debug_locks) ? lockdep_is_held(lock) : 1;
}
EXPORT_SYMBOL_GPL(lockdep_rht_bucket_is_held);
#else
#define ASSERT_RHT_MUTEX(HT)
#endif


static int alloc_bucket_locks(struct rhashtable *ht, struct bucket_table *tbl,
			      gfp_t gfp)
{
	unsigned int i, size;
#if defined(CONFIG_PROVE_LOCKING)
	unsigned int nr_pcpus = 2;
#else
	unsigned int nr_pcpus = num_possible_cpus();
#endif

	nr_pcpus = min_t(unsigned int, nr_pcpus, 64UL);
	size = roundup_pow_of_two(nr_pcpus * ht->p.locks_mul);

	/* Never allocate more than 0.5 locks per bucket */
	size = min_t(unsigned int, size, tbl->size >> 1);

	if (sizeof(spinlock_t) != 0) {
		tbl->locks = NULL;
#ifdef CONFIG_NUMA
		if (size * sizeof(spinlock_t) > PAGE_SIZE &&
		    gfp == GFP_KERNEL)
			tbl->locks = vmalloc(size * sizeof(spinlock_t));
#endif
		if (gfp != GFP_KERNEL)
			gfp |= __GFP_NOWARN | __GFP_NORETRY;

		if (!tbl->locks)
			tbl->locks = kmalloc_array(size, sizeof(spinlock_t),
						   gfp);
		if (!tbl->locks)
			return -ENOMEM;
		for (i = 0; i < size; i++)
			spin_lock_init(&tbl->locks[i]);
	}
	tbl->locks_mask = size - 1;

	return 0;
}

static void bucket_table_free(const struct bucket_table *tbl)
{
	if (tbl)
		kvfree(tbl->locks);

	kvfree(tbl);
}

static void bucket_table_free_rcu(struct rcu_head *head)
{
	bucket_table_free(container_of(head, struct bucket_table, rcu));
}

static struct bucket_table *bucket_table_alloc(struct rhashtable *ht,
					       size_t nbuckets,
					       gfp_t gfp)
{
	struct bucket_table *tbl = NULL;
	size_t size;
	int i;

	size = sizeof(*tbl) + nbuckets * sizeof(tbl->buckets[0]);
	if (size <= (PAGE_SIZE << PAGE_ALLOC_COSTLY_ORDER) ||
	    gfp != GFP_KERNEL)
		tbl = kzalloc(size, gfp | __GFP_NOWARN | __GFP_NORETRY);
	if (tbl == NULL && gfp == GFP_KERNEL)
		tbl = vzalloc(size);
>>>>>>> v4.9.227
	if (tbl == NULL)
		return NULL;

	tbl->size = nbuckets;

<<<<<<< HEAD
	return tbl;
}

static void bucket_table_free(const struct bucket_table *tbl)
{
	kvfree(tbl);
}

/**
 * rht_grow_above_75 - returns true if nelems > 0.75 * table-size
 * @ht:		hash table
 * @new_size:	new table size
 */
bool rht_grow_above_75(const struct rhashtable *ht, size_t new_size)
{
	/* Expand table when exceeding 75% load */
	return ht->nelems > (new_size / 4 * 3);
}
EXPORT_SYMBOL_GPL(rht_grow_above_75);

/**
 * rht_shrink_below_30 - returns true if nelems < 0.3 * table-size
 * @ht:		hash table
 * @new_size:	new table size
 */
bool rht_shrink_below_30(const struct rhashtable *ht, size_t new_size)
{
	/* Shrink table beneath 30% load */
	return ht->nelems < (new_size * 3 / 10);
}
EXPORT_SYMBOL_GPL(rht_shrink_below_30);

static void hashtable_chain_unzip(const struct rhashtable *ht,
				  const struct bucket_table *new_tbl,
				  struct bucket_table *old_tbl, size_t n)
{
	struct rhash_head *he, *p, *next;
	unsigned int h;

	/* Old bucket empty, no work needed. */
	p = rht_dereference(old_tbl->buckets[n], ht);
	if (!p)
		return;

	/* Advance the old bucket pointer one or more times until it
	 * reaches a node that doesn't hash to the same bucket as the
	 * previous node p. Call the previous node p;
	 */
	h = head_hashfn(ht, p, new_tbl->size);
	rht_for_each(he, p->next, ht) {
		if (head_hashfn(ht, he, new_tbl->size) != h)
			break;
		p = he;
	}
	RCU_INIT_POINTER(old_tbl->buckets[n], p->next);

	/* Find the subsequent node which does hash to the same
	 * bucket as node P, or NULL if no such node exists.
	 */
	next = NULL;
	if (he) {
		rht_for_each(he, he->next, ht) {
			if (head_hashfn(ht, he, new_tbl->size) == h) {
				next = he;
				break;
			}
		}
	}

	/* Set p's next pointer to that subsequent node pointer,
	 * bypassing the nodes which do not hash to p's bucket
	 */
	RCU_INIT_POINTER(p->next, next);
=======
	if (alloc_bucket_locks(ht, tbl, gfp) < 0) {
		bucket_table_free(tbl);
		return NULL;
	}

	INIT_LIST_HEAD(&tbl->walkers);

	get_random_bytes(&tbl->hash_rnd, sizeof(tbl->hash_rnd));

	for (i = 0; i < nbuckets; i++)
		INIT_RHT_NULLS_HEAD(tbl->buckets[i], ht, i);

	return tbl;
}

static struct bucket_table *rhashtable_last_table(struct rhashtable *ht,
						  struct bucket_table *tbl)
{
	struct bucket_table *new_tbl;

	do {
		new_tbl = tbl;
		tbl = rht_dereference_rcu(tbl->future_tbl, ht);
	} while (tbl);

	return new_tbl;
}

static int rhashtable_rehash_one(struct rhashtable *ht, unsigned int old_hash)
{
	struct bucket_table *old_tbl = rht_dereference(ht->tbl, ht);
	struct bucket_table *new_tbl = rhashtable_last_table(ht,
		rht_dereference_rcu(old_tbl->future_tbl, ht));
	struct rhash_head __rcu **pprev = &old_tbl->buckets[old_hash];
	int err = -ENOENT;
	struct rhash_head *head, *next, *entry;
	spinlock_t *new_bucket_lock;
	unsigned int new_hash;

	rht_for_each(entry, old_tbl, old_hash) {
		err = 0;
		next = rht_dereference_bucket(entry->next, old_tbl, old_hash);

		if (rht_is_a_nulls(next))
			break;

		pprev = &entry->next;
	}

	if (err)
		goto out;

	new_hash = head_hashfn(ht, new_tbl, entry);

	new_bucket_lock = rht_bucket_lock(new_tbl, new_hash);

	spin_lock_nested(new_bucket_lock, SINGLE_DEPTH_NESTING);
	head = rht_dereference_bucket(new_tbl->buckets[new_hash],
				      new_tbl, new_hash);

	RCU_INIT_POINTER(entry->next, head);

	rcu_assign_pointer(new_tbl->buckets[new_hash], entry);
	spin_unlock(new_bucket_lock);

	rcu_assign_pointer(*pprev, next);

out:
	return err;
}

static void rhashtable_rehash_chain(struct rhashtable *ht,
				    unsigned int old_hash)
{
	struct bucket_table *old_tbl = rht_dereference(ht->tbl, ht);
	spinlock_t *old_bucket_lock;

	old_bucket_lock = rht_bucket_lock(old_tbl, old_hash);

	spin_lock_bh(old_bucket_lock);
	while (!rhashtable_rehash_one(ht, old_hash))
		;
	old_tbl->rehash++;
	spin_unlock_bh(old_bucket_lock);
}

static int rhashtable_rehash_attach(struct rhashtable *ht,
				    struct bucket_table *old_tbl,
				    struct bucket_table *new_tbl)
{
	/* Protect future_tbl using the first bucket lock. */
	spin_lock_bh(old_tbl->locks);

	/* Did somebody beat us to it? */
	if (rcu_access_pointer(old_tbl->future_tbl)) {
		spin_unlock_bh(old_tbl->locks);
		return -EEXIST;
	}

	/* Make insertions go into the new, empty table right away. Deletions
	 * and lookups will be attempted in both tables until we synchronize.
	 */
	rcu_assign_pointer(old_tbl->future_tbl, new_tbl);

	spin_unlock_bh(old_tbl->locks);

	return 0;
}

static int rhashtable_rehash_table(struct rhashtable *ht)
{
	struct bucket_table *old_tbl = rht_dereference(ht->tbl, ht);
	struct bucket_table *new_tbl;
	struct rhashtable_walker *walker;
	unsigned int old_hash;

	new_tbl = rht_dereference(old_tbl->future_tbl, ht);
	if (!new_tbl)
		return 0;

	for (old_hash = 0; old_hash < old_tbl->size; old_hash++) {
		rhashtable_rehash_chain(ht, old_hash);
		cond_resched();
	}

	/* Publish the new table pointer. */
	rcu_assign_pointer(ht->tbl, new_tbl);

	spin_lock(&ht->lock);
	list_for_each_entry(walker, &old_tbl->walkers, list)
		walker->tbl = NULL;
	spin_unlock(&ht->lock);

	/* Wait for readers. All new readers will see the new
	 * table, and thus no references to the old table will
	 * remain.
	 */
	call_rcu(&old_tbl->rcu, bucket_table_free_rcu);

	return rht_dereference(new_tbl->future_tbl, ht) ? -EAGAIN : 0;
>>>>>>> v4.9.227
}

/**
 * rhashtable_expand - Expand hash table while allowing concurrent lookups
 * @ht:		the hash table to expand
<<<<<<< HEAD
 * @flags:	allocation flags
 *
 * A secondary bucket array is allocated and the hash entries are migrated
 * while keeping them on both lists until the end of the RCU grace period.
=======
 *
 * A secondary bucket array is allocated and the hash entries are migrated.
>>>>>>> v4.9.227
 *
 * This function may only be called in a context where it is safe to call
 * synchronize_rcu(), e.g. not within a rcu_read_lock() section.
 *
<<<<<<< HEAD
 * The caller must ensure that no concurrent table mutations take place.
 * It is however valid to have concurrent lookups if they are RCU protected.
 */
int rhashtable_expand(struct rhashtable *ht, gfp_t flags)
{
	struct bucket_table *new_tbl, *old_tbl = rht_dereference(ht->tbl, ht);
	struct rhash_head *he;
	unsigned int i, h;
	bool complete;

	ASSERT_RHT_MUTEX(ht);

	if (ht->p.max_shift && ht->shift >= ht->p.max_shift)
		return 0;

	new_tbl = bucket_table_alloc(old_tbl->size * 2, flags);
	if (new_tbl == NULL)
		return -ENOMEM;

	ht->shift++;

	/* For each new bucket, search the corresponding old bucket
	 * for the first entry that hashes to the new bucket, and
	 * link the new bucket to that entry. Since all the entries
	 * which will end up in the new bucket appear in the same
	 * old bucket, this constructs an entirely valid new hash
	 * table, but with multiple buckets "zipped" together into a
	 * single imprecise chain.
	 */
	for (i = 0; i < new_tbl->size; i++) {
		h = i & (old_tbl->size - 1);
		rht_for_each(he, old_tbl->buckets[h], ht) {
			if (head_hashfn(ht, he, new_tbl->size) == i) {
				RCU_INIT_POINTER(new_tbl->buckets[i], he);
				break;
			}
		}
	}

	/* Publish the new table pointer. Lookups may now traverse
	 * the new table, but they will not benefit from any
	 * additional efficiency until later steps unzip the buckets.
	 */
	rcu_assign_pointer(ht->tbl, new_tbl);

	/* Unzip interleaved hash chains */
	do {
		/* Wait for readers. All new readers will see the new
		 * table, and thus no references to the old table will
		 * remain.
		 */
		synchronize_rcu();

		/* For each bucket in the old table (each of which
		 * contains items from multiple buckets of the new
		 * table): ...
		 */
		complete = true;
		for (i = 0; i < old_tbl->size; i++) {
			hashtable_chain_unzip(ht, new_tbl, old_tbl, i);
			if (old_tbl->buckets[i] != NULL)
				complete = false;
		}
	} while (!complete);

	bucket_table_free(old_tbl);
	return 0;
}
EXPORT_SYMBOL_GPL(rhashtable_expand);
=======
 * The caller must ensure that no concurrent resizing occurs by holding
 * ht->mutex.
 *
 * It is valid to have concurrent insertions and deletions protected by per
 * bucket locks or concurrent RCU protected lookups and traversals.
 */
static int rhashtable_expand(struct rhashtable *ht)
{
	struct bucket_table *new_tbl, *old_tbl = rht_dereference(ht->tbl, ht);
	int err;

	ASSERT_RHT_MUTEX(ht);

	old_tbl = rhashtable_last_table(ht, old_tbl);

	new_tbl = bucket_table_alloc(ht, old_tbl->size * 2, GFP_KERNEL);
	if (new_tbl == NULL)
		return -ENOMEM;

	err = rhashtable_rehash_attach(ht, old_tbl, new_tbl);
	if (err)
		bucket_table_free(new_tbl);

	return err;
}
>>>>>>> v4.9.227

/**
 * rhashtable_shrink - Shrink hash table while allowing concurrent lookups
 * @ht:		the hash table to shrink
<<<<<<< HEAD
 * @flags:	allocation flags
 *
 * This function may only be called in a context where it is safe to call
 * synchronize_rcu(), e.g. not within a rcu_read_lock() section.
 *
 * The caller must ensure that no concurrent table mutations take place.
 * It is however valid to have concurrent lookups if they are RCU protected.
 */
int rhashtable_shrink(struct rhashtable *ht, gfp_t flags)
{
	struct bucket_table *ntbl, *tbl = rht_dereference(ht->tbl, ht);
	struct rhash_head __rcu **pprev;
	unsigned int i;

	ASSERT_RHT_MUTEX(ht);

	if (ht->shift <= ht->p.min_shift)
		return 0;

	ntbl = bucket_table_alloc(tbl->size / 2, flags);
	if (ntbl == NULL)
		return -ENOMEM;

	ht->shift--;

	/* Link each bucket in the new table to the first bucket
	 * in the old table that contains entries which will hash
	 * to the new bucket.
	 */
	for (i = 0; i < ntbl->size; i++) {
		ntbl->buckets[i] = tbl->buckets[i];

		/* Link each bucket in the new table to the first bucket
		 * in the old table that contains entries which will hash
		 * to the new bucket.
		 */
		for (pprev = &ntbl->buckets[i]; *pprev != NULL;
		     pprev = &rht_dereference(*pprev, ht)->next)
			;
		RCU_INIT_POINTER(*pprev, tbl->buckets[i + ntbl->size]);
	}

	/* Publish the new, valid hash table */
	rcu_assign_pointer(ht->tbl, ntbl);

	/* Wait for readers. No new readers will have references to the
	 * old hash table.
	 */
	synchronize_rcu();

	bucket_table_free(tbl);

	return 0;
}
EXPORT_SYMBOL_GPL(rhashtable_shrink);

/**
 * rhashtable_insert - insert object into hash hash table
 * @ht:		hash table
 * @obj:	pointer to hash head inside object
 * @flags:	allocation flags (table expansion)
 *
 * Will automatically grow the table via rhashtable_expand() if the the
 * grow_decision function specified at rhashtable_init() returns true.
 *
 * The caller must ensure that no concurrent table mutations occur. It is
 * however valid to have concurrent lookups if they are RCU protected.
 */
void rhashtable_insert(struct rhashtable *ht, struct rhash_head *obj,
		       gfp_t flags)
{
	struct bucket_table *tbl = rht_dereference(ht->tbl, ht);
	u32 hash;

	ASSERT_RHT_MUTEX(ht);

	hash = head_hashfn(ht, obj, tbl->size);
	RCU_INIT_POINTER(obj->next, tbl->buckets[hash]);
	rcu_assign_pointer(tbl->buckets[hash], obj);
	ht->nelems++;

	if (ht->p.grow_decision && ht->p.grow_decision(ht, tbl->size))
		rhashtable_expand(ht, flags);
}
EXPORT_SYMBOL_GPL(rhashtable_insert);

/**
 * rhashtable_remove_pprev - remove object from hash table given previous element
 * @ht:		hash table
 * @obj:	pointer to hash head inside object
 * @pprev:	pointer to previous element
 * @flags:	allocation flags (table expansion)
 *
 * Identical to rhashtable_remove() but caller is alreayd aware of the element
 * in front of the element to be deleted. This is in particular useful for
 * deletion when combined with walking or lookup.
 */
void rhashtable_remove_pprev(struct rhashtable *ht, struct rhash_head *obj,
			     struct rhash_head __rcu **pprev, gfp_t flags)
{
	struct bucket_table *tbl = rht_dereference(ht->tbl, ht);

	ASSERT_RHT_MUTEX(ht);

	RCU_INIT_POINTER(*pprev, obj->next);
	ht->nelems--;

	if (ht->p.shrink_decision &&
	    ht->p.shrink_decision(ht, tbl->size))
		rhashtable_shrink(ht, flags);
}
EXPORT_SYMBOL_GPL(rhashtable_remove_pprev);

/**
 * rhashtable_remove - remove object from hash table
 * @ht:		hash table
 * @obj:	pointer to hash head inside object
 * @flags:	allocation flags (table expansion)
 *
 * Since the hash chain is single linked, the removal operation needs to
 * walk the bucket chain upon removal. The removal operation is thus
 * considerable slow if the hash table is not correctly sized.
 *
 * Will automatically shrink the table via rhashtable_expand() if the the
 * shrink_decision function specified at rhashtable_init() returns true.
 *
 * The caller must ensure that no concurrent table mutations occur. It is
 * however valid to have concurrent lookups if they are RCU protected.
 */
bool rhashtable_remove(struct rhashtable *ht, struct rhash_head *obj,
		       gfp_t flags)
{
	struct bucket_table *tbl = rht_dereference(ht->tbl, ht);
	struct rhash_head __rcu **pprev;
	struct rhash_head *he;
	u32 h;

	ASSERT_RHT_MUTEX(ht);

	h = head_hashfn(ht, obj, tbl->size);

	pprev = &tbl->buckets[h];
	rht_for_each(he, tbl->buckets[h], ht) {
		if (he != obj) {
			pprev = &he->next;
			continue;
		}

		rhashtable_remove_pprev(ht, he, pprev, flags);
		return true;
	}

	return false;
}
EXPORT_SYMBOL_GPL(rhashtable_remove);

/**
 * rhashtable_lookup - lookup key in hash table
 * @ht:		hash table
 * @key:	pointer to key
 *
 * Computes the hash value for the key and traverses the bucket chain looking
 * for a entry with an identical key. The first matching entry is returned.
 *
 * This lookup function may only be used for fixed key hash table (key_len
 * paramter set). It will BUG() if used inappropriately.
 *
 * Lookups may occur in parallel with hash mutations as long as the lookup is
 * guarded by rcu_read_lock(). The caller must take care of this.
 */
void *rhashtable_lookup(const struct rhashtable *ht, const void *key)
{
	const struct bucket_table *tbl = rht_dereference_rcu(ht->tbl, ht);
	struct rhash_head *he;
	u32 h;

	BUG_ON(!ht->p.key_len);

	h = __hashfn(ht, key, ht->p.key_len, tbl->size);
	rht_for_each_rcu(he, tbl->buckets[h], ht) {
		if (memcmp(rht_obj(ht, he) + ht->p.key_offset, key,
			   ht->p.key_len))
			continue;
		return (void *) he - ht->p.head_offset;
=======
 *
 * This function shrinks the hash table to fit, i.e., the smallest
 * size would not cause it to expand right away automatically.
 *
 * The caller must ensure that no concurrent resizing occurs by holding
 * ht->mutex.
 *
 * The caller must ensure that no concurrent table mutations take place.
 * It is however valid to have concurrent lookups if they are RCU protected.
 *
 * It is valid to have concurrent insertions and deletions protected by per
 * bucket locks or concurrent RCU protected lookups and traversals.
 */
static int rhashtable_shrink(struct rhashtable *ht)
{
	struct bucket_table *new_tbl, *old_tbl = rht_dereference(ht->tbl, ht);
	unsigned int nelems = atomic_read(&ht->nelems);
	unsigned int size = 0;
	int err;

	ASSERT_RHT_MUTEX(ht);

	if (nelems)
		size = roundup_pow_of_two(nelems * 3 / 2);
	if (size < ht->p.min_size)
		size = ht->p.min_size;

	if (old_tbl->size <= size)
		return 0;

	if (rht_dereference(old_tbl->future_tbl, ht))
		return -EEXIST;

	new_tbl = bucket_table_alloc(ht, size, GFP_KERNEL);
	if (new_tbl == NULL)
		return -ENOMEM;

	err = rhashtable_rehash_attach(ht, old_tbl, new_tbl);
	if (err)
		bucket_table_free(new_tbl);

	return err;
}

static void rht_deferred_worker(struct work_struct *work)
{
	struct rhashtable *ht;
	struct bucket_table *tbl;
	int err = 0;

	ht = container_of(work, struct rhashtable, run_work);
	mutex_lock(&ht->mutex);

	tbl = rht_dereference(ht->tbl, ht);
	tbl = rhashtable_last_table(ht, tbl);

	if (rht_grow_above_75(ht, tbl))
		rhashtable_expand(ht);
	else if (ht->p.automatic_shrinking && rht_shrink_below_30(ht, tbl))
		rhashtable_shrink(ht);

	err = rhashtable_rehash_table(ht);

	mutex_unlock(&ht->mutex);

	if (err)
		schedule_work(&ht->run_work);
}

static int rhashtable_insert_rehash(struct rhashtable *ht,
				    struct bucket_table *tbl)
{
	struct bucket_table *old_tbl;
	struct bucket_table *new_tbl;
	unsigned int size;
	int err;

	old_tbl = rht_dereference_rcu(ht->tbl, ht);

	size = tbl->size;

	err = -EBUSY;

	if (rht_grow_above_75(ht, tbl))
		size *= 2;
	/* Do not schedule more than one rehash */
	else if (old_tbl != tbl)
		goto fail;

	err = -ENOMEM;

	new_tbl = bucket_table_alloc(ht, size, GFP_ATOMIC);
	if (new_tbl == NULL)
		goto fail;

	err = rhashtable_rehash_attach(ht, tbl, new_tbl);
	if (err) {
		bucket_table_free(new_tbl);
		if (err == -EEXIST)
			err = 0;
	} else
		schedule_work(&ht->run_work);

	return err;

fail:
	/* Do not fail the insert if someone else did a rehash. */
	if (likely(rcu_dereference_raw(tbl->future_tbl)))
		return 0;

	/* Schedule async rehash to retry allocation in process context. */
	if (err == -ENOMEM)
		schedule_work(&ht->run_work);

	return err;
}

static void *rhashtable_lookup_one(struct rhashtable *ht,
				   struct bucket_table *tbl, unsigned int hash,
				   const void *key, struct rhash_head *obj)
{
	struct rhashtable_compare_arg arg = {
		.ht = ht,
		.key = key,
	};
	struct rhash_head __rcu **pprev;
	struct rhash_head *head;
	int elasticity;

	elasticity = ht->elasticity;
	pprev = &tbl->buckets[hash];
	rht_for_each(head, tbl, hash) {
		struct rhlist_head *list;
		struct rhlist_head *plist;

		elasticity--;
		if (!key ||
		    (ht->p.obj_cmpfn ?
		     ht->p.obj_cmpfn(&arg, rht_obj(ht, head)) :
		     rhashtable_compare(&arg, rht_obj(ht, head)))) {
			pprev = &head->next;
			continue;
		}

		if (!ht->rhlist)
			return rht_obj(ht, head);

		list = container_of(obj, struct rhlist_head, rhead);
		plist = container_of(head, struct rhlist_head, rhead);

		RCU_INIT_POINTER(list->next, plist);
		head = rht_dereference_bucket(head->next, tbl, hash);
		RCU_INIT_POINTER(list->rhead.next, head);
		rcu_assign_pointer(*pprev, obj);

		return NULL;
	}

	if (elasticity <= 0)
		return ERR_PTR(-EAGAIN);

	return ERR_PTR(-ENOENT);
}

static struct bucket_table *rhashtable_insert_one(struct rhashtable *ht,
						  struct bucket_table *tbl,
						  unsigned int hash,
						  struct rhash_head *obj,
						  void *data)
{
	struct bucket_table *new_tbl;
	struct rhash_head *head;

	if (!IS_ERR_OR_NULL(data))
		return ERR_PTR(-EEXIST);

	if (PTR_ERR(data) != -EAGAIN && PTR_ERR(data) != -ENOENT)
		return ERR_CAST(data);

	new_tbl = rcu_dereference(tbl->future_tbl);
	if (new_tbl)
		return new_tbl;

	if (PTR_ERR(data) != -ENOENT)
		return ERR_CAST(data);

	if (unlikely(rht_grow_above_max(ht, tbl)))
		return ERR_PTR(-E2BIG);

	if (unlikely(rht_grow_above_100(ht, tbl)))
		return ERR_PTR(-EAGAIN);

	head = rht_dereference_bucket(tbl->buckets[hash], tbl, hash);

	RCU_INIT_POINTER(obj->next, head);
	if (ht->rhlist) {
		struct rhlist_head *list;

		list = container_of(obj, struct rhlist_head, rhead);
		RCU_INIT_POINTER(list->next, NULL);
	}

	rcu_assign_pointer(tbl->buckets[hash], obj);

	atomic_inc(&ht->nelems);
	if (rht_grow_above_75(ht, tbl))
		schedule_work(&ht->run_work);

	return NULL;
}

static void *rhashtable_try_insert(struct rhashtable *ht, const void *key,
				   struct rhash_head *obj)
{
	struct bucket_table *new_tbl;
	struct bucket_table *tbl;
	unsigned int hash;
	spinlock_t *lock;
	void *data;

	tbl = rcu_dereference(ht->tbl);

	/* All insertions must grab the oldest table containing
	 * the hashed bucket that is yet to be rehashed.
	 */
	for (;;) {
		hash = rht_head_hashfn(ht, tbl, obj, ht->p);
		lock = rht_bucket_lock(tbl, hash);
		spin_lock_bh(lock);

		if (tbl->rehash <= hash)
			break;

		spin_unlock_bh(lock);
		tbl = rcu_dereference(tbl->future_tbl);
	}

	data = rhashtable_lookup_one(ht, tbl, hash, key, obj);
	new_tbl = rhashtable_insert_one(ht, tbl, hash, obj, data);
	if (PTR_ERR(new_tbl) != -EEXIST)
		data = ERR_CAST(new_tbl);

	while (!IS_ERR_OR_NULL(new_tbl)) {
		tbl = new_tbl;
		hash = rht_head_hashfn(ht, tbl, obj, ht->p);
		spin_lock_nested(rht_bucket_lock(tbl, hash),
				 SINGLE_DEPTH_NESTING);

		data = rhashtable_lookup_one(ht, tbl, hash, key, obj);
		new_tbl = rhashtable_insert_one(ht, tbl, hash, obj, data);
		if (PTR_ERR(new_tbl) != -EEXIST)
			data = ERR_CAST(new_tbl);

		spin_unlock(rht_bucket_lock(tbl, hash));
	}

	spin_unlock_bh(lock);

	if (PTR_ERR(data) == -EAGAIN)
		data = ERR_PTR(rhashtable_insert_rehash(ht, tbl) ?:
			       -EAGAIN);

	return data;
}

void *rhashtable_insert_slow(struct rhashtable *ht, const void *key,
			     struct rhash_head *obj)
{
	void *data;

	do {
		rcu_read_lock();
		data = rhashtable_try_insert(ht, key, obj);
		rcu_read_unlock();
	} while (PTR_ERR(data) == -EAGAIN);

	return data;
}
EXPORT_SYMBOL_GPL(rhashtable_insert_slow);

/**
 * rhashtable_walk_enter - Initialise an iterator
 * @ht:		Table to walk over
 * @iter:	Hash table Iterator
 *
 * This function prepares a hash table walk.
 *
 * Note that if you restart a walk after rhashtable_walk_stop you
 * may see the same object twice.  Also, you may miss objects if
 * there are removals in between rhashtable_walk_stop and the next
 * call to rhashtable_walk_start.
 *
 * For a completely stable walk you should construct your own data
 * structure outside the hash table.
 *
 * This function may sleep so you must not call it from interrupt
 * context or with spin locks held.
 *
 * You must call rhashtable_walk_exit after this function returns.
 */
void rhashtable_walk_enter(struct rhashtable *ht, struct rhashtable_iter *iter)
{
	iter->ht = ht;
	iter->p = NULL;
	iter->slot = 0;
	iter->skip = 0;

	spin_lock(&ht->lock);
	iter->walker.tbl =
		rcu_dereference_protected(ht->tbl, lockdep_is_held(&ht->lock));
	list_add(&iter->walker.list, &iter->walker.tbl->walkers);
	spin_unlock(&ht->lock);
}
EXPORT_SYMBOL_GPL(rhashtable_walk_enter);

/**
 * rhashtable_walk_exit - Free an iterator
 * @iter:	Hash table Iterator
 *
 * This function frees resources allocated by rhashtable_walk_init.
 */
void rhashtable_walk_exit(struct rhashtable_iter *iter)
{
	spin_lock(&iter->ht->lock);
	if (iter->walker.tbl)
		list_del(&iter->walker.list);
	spin_unlock(&iter->ht->lock);
}
EXPORT_SYMBOL_GPL(rhashtable_walk_exit);

/**
 * rhashtable_walk_start - Start a hash table walk
 * @iter:	Hash table iterator
 *
 * Start a hash table walk.  Note that we take the RCU lock in all
 * cases including when we return an error.  So you must always call
 * rhashtable_walk_stop to clean up.
 *
 * Returns zero if successful.
 *
 * Returns -EAGAIN if resize event occured.  Note that the iterator
 * will rewind back to the beginning and you may use it immediately
 * by calling rhashtable_walk_next.
 */
int rhashtable_walk_start(struct rhashtable_iter *iter)
	__acquires(RCU)
{
	struct rhashtable *ht = iter->ht;

	rcu_read_lock();

	spin_lock(&ht->lock);
	if (iter->walker.tbl)
		list_del(&iter->walker.list);
	spin_unlock(&ht->lock);

	if (!iter->walker.tbl) {
		iter->walker.tbl = rht_dereference_rcu(ht->tbl, ht);
		return -EAGAIN;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(rhashtable_walk_start);

/**
 * rhashtable_walk_next - Return the next object and advance the iterator
 * @iter:	Hash table iterator
 *
 * Note that you must call rhashtable_walk_stop when you are finished
 * with the walk.
 *
 * Returns the next object or NULL when the end of the table is reached.
 *
 * Returns -EAGAIN if resize event occured.  Note that the iterator
 * will rewind back to the beginning and you may continue to use it.
 */
void *rhashtable_walk_next(struct rhashtable_iter *iter)
{
	struct bucket_table *tbl = iter->walker.tbl;
	struct rhlist_head *list = iter->list;
	struct rhashtable *ht = iter->ht;
	struct rhash_head *p = iter->p;
	bool rhlist = ht->rhlist;

	if (p) {
		if (!rhlist || !(list = rcu_dereference(list->next))) {
			p = rcu_dereference(p->next);
			list = container_of(p, struct rhlist_head, rhead);
		}
		goto next;
	}

	for (; iter->slot < tbl->size; iter->slot++) {
		int skip = iter->skip;

		rht_for_each_rcu(p, tbl, iter->slot) {
			if (rhlist) {
				list = container_of(p, struct rhlist_head,
						    rhead);
				do {
					if (!skip)
						goto next;
					skip--;
					list = rcu_dereference(list->next);
				} while (list);

				continue;
			}
			if (!skip)
				break;
			skip--;
		}

next:
		if (!rht_is_a_nulls(p)) {
			iter->skip++;
			iter->p = p;
			iter->list = list;
			return rht_obj(ht, rhlist ? &list->rhead : p);
		}

		iter->skip = 0;
	}

	iter->p = NULL;

	/* Ensure we see any new tables. */
	smp_rmb();

	iter->walker.tbl = rht_dereference_rcu(tbl->future_tbl, ht);
	if (iter->walker.tbl) {
		iter->slot = 0;
		iter->skip = 0;
		return ERR_PTR(-EAGAIN);
>>>>>>> v4.9.227
	}

	return NULL;
}
<<<<<<< HEAD
EXPORT_SYMBOL_GPL(rhashtable_lookup);

/**
 * rhashtable_lookup_compare - search hash table with compare function
 * @ht:		hash table
 * @key:	pointer to key
 * @compare:	compare function, must return true on match
 * @arg:	argument passed on to compare function
 *
 * Traverses the bucket chain behind the provided hash value and calls the
 * specified compare function for each entry.
 *
 * Lookups may occur in parallel with hash mutations as long as the lookup is
 * guarded by rcu_read_lock(). The caller must take care of this.
 *
 * Returns the first entry on which the compare function returned true.
 */
void *rhashtable_lookup_compare(const struct rhashtable *ht, void *key,
				bool (*compare)(void *, void *), void *arg)
{
	const struct bucket_table *tbl = rht_dereference_rcu(ht->tbl, ht);
	struct rhash_head *he;
	u32 hash;

	hash = __hashfn(ht, key, ht->p.key_len, tbl->size);

	rht_for_each_rcu(he, tbl->buckets[hash], ht) {
		if (!compare(rht_obj(ht, he), arg))
			continue;
		return (void *) he - ht->p.head_offset;
	}

	return NULL;
}
EXPORT_SYMBOL_GPL(rhashtable_lookup_compare);

static size_t rounded_hashtable_size(struct rhashtable_params *params)
{
	return max(roundup_pow_of_two(params->nelem_hint * 4 / 3),
		   1UL << params->min_shift);
=======
EXPORT_SYMBOL_GPL(rhashtable_walk_next);

/**
 * rhashtable_walk_stop - Finish a hash table walk
 * @iter:	Hash table iterator
 *
 * Finish a hash table walk.
 */
void rhashtable_walk_stop(struct rhashtable_iter *iter)
	__releases(RCU)
{
	struct rhashtable *ht;
	struct bucket_table *tbl = iter->walker.tbl;

	if (!tbl)
		goto out;

	ht = iter->ht;

	spin_lock(&ht->lock);
	if (tbl->rehash < tbl->size)
		list_add(&iter->walker.list, &tbl->walkers);
	else
		iter->walker.tbl = NULL;
	spin_unlock(&ht->lock);

	iter->p = NULL;

out:
	rcu_read_unlock();
}
EXPORT_SYMBOL_GPL(rhashtable_walk_stop);

static size_t rounded_hashtable_size(const struct rhashtable_params *params)
{
	size_t retsize;

	if (params->nelem_hint)
		retsize = max(roundup_pow_of_two(params->nelem_hint * 4 / 3),
			      (unsigned long)params->min_size);
	else
		retsize = max(HASH_DEFAULT_SIZE,
			      (unsigned long)params->min_size);

	return retsize;
}

static u32 rhashtable_jhash2(const void *key, u32 length, u32 seed)
{
	return jhash2(key, length, seed);
>>>>>>> v4.9.227
}

/**
 * rhashtable_init - initialize a new hash table
 * @ht:		hash table to be initialized
 * @params:	configuration parameters
 *
 * Initializes a new hash table based on the provided configuration
 * parameters. A table can be configured either with a variable or
 * fixed length key:
 *
 * Configuration Example 1: Fixed length keys
 * struct test_obj {
 *	int			key;
 *	void *			my_member;
 *	struct rhash_head	node;
 * };
 *
 * struct rhashtable_params params = {
 *	.head_offset = offsetof(struct test_obj, node),
 *	.key_offset = offsetof(struct test_obj, key),
 *	.key_len = sizeof(int),
<<<<<<< HEAD
 *	.hashfn = arch_fast_hash,
 *	.mutex_is_held = &my_mutex_is_held,
=======
 *	.hashfn = jhash,
 *	.nulls_base = (1U << RHT_BASE_SHIFT),
>>>>>>> v4.9.227
 * };
 *
 * Configuration Example 2: Variable length keys
 * struct test_obj {
 *	[...]
 *	struct rhash_head	node;
 * };
 *
<<<<<<< HEAD
 * u32 my_hash_fn(const void *data, u32 seed)
=======
 * u32 my_hash_fn(const void *data, u32 len, u32 seed)
>>>>>>> v4.9.227
 * {
 *	struct test_obj *obj = data;
 *
 *	return [... hash ...];
 * }
 *
 * struct rhashtable_params params = {
 *	.head_offset = offsetof(struct test_obj, node),
<<<<<<< HEAD
 *	.hashfn = arch_fast_hash,
 *	.obj_hashfn = my_hash_fn,
 *	.mutex_is_held = &my_mutex_is_held,
 * };
 */
int rhashtable_init(struct rhashtable *ht, struct rhashtable_params *params)
=======
 *	.hashfn = jhash,
 *	.obj_hashfn = my_hash_fn,
 * };
 */
int rhashtable_init(struct rhashtable *ht,
		    const struct rhashtable_params *params)
>>>>>>> v4.9.227
{
	struct bucket_table *tbl;
	size_t size;

<<<<<<< HEAD
	size = HASH_DEFAULT_SIZE;

	if ((params->key_len && !params->hashfn) ||
	    (!params->key_len && !params->obj_hashfn))
		return -EINVAL;

	params->min_shift = max_t(size_t, params->min_shift,
				  ilog2(HASH_MIN_SIZE));

	if (params->nelem_hint)
		size = rounded_hashtable_size(params);

	tbl = bucket_table_alloc(size, GFP_KERNEL);
	if (tbl == NULL)
		return -ENOMEM;

	memset(ht, 0, sizeof(*ht));
	ht->shift = ilog2(tbl->size);
	memcpy(&ht->p, params, sizeof(*params));
	RCU_INIT_POINTER(ht->tbl, tbl);

	if (!ht->p.hash_rnd)
		get_random_bytes(&ht->p.hash_rnd, sizeof(ht->p.hash_rnd));
=======
	if ((!params->key_len && !params->obj_hashfn) ||
	    (params->obj_hashfn && !params->obj_cmpfn))
		return -EINVAL;

	if (params->nulls_base && params->nulls_base < (1U << RHT_BASE_SHIFT))
		return -EINVAL;

	memset(ht, 0, sizeof(*ht));
	mutex_init(&ht->mutex);
	spin_lock_init(&ht->lock);
	memcpy(&ht->p, params, sizeof(*params));

	if (params->min_size)
		ht->p.min_size = roundup_pow_of_two(params->min_size);

	if (params->max_size)
		ht->p.max_size = rounddown_pow_of_two(params->max_size);

	if (params->insecure_max_entries)
		ht->p.insecure_max_entries =
			rounddown_pow_of_two(params->insecure_max_entries);
	else
		ht->p.insecure_max_entries = ht->p.max_size * 2;

	ht->p.min_size = max(ht->p.min_size, HASH_MIN_SIZE);

	size = rounded_hashtable_size(&ht->p);

	/* The maximum (not average) chain length grows with the
	 * size of the hash table, at a rate of (log N)/(log log N).
	 * The value of 16 is selected so that even if the hash
	 * table grew to 2^32 you would not expect the maximum
	 * chain length to exceed it unless we are under attack
	 * (or extremely unlucky).
	 *
	 * As this limit is only to detect attacks, we don't need
	 * to set it to a lower value as you'd need the chain
	 * length to vastly exceed 16 to have any real effect
	 * on the system.
	 */
	if (!params->insecure_elasticity)
		ht->elasticity = 16;

	if (params->locks_mul)
		ht->p.locks_mul = roundup_pow_of_two(params->locks_mul);
	else
		ht->p.locks_mul = BUCKET_LOCKS_PER_CPU;

	ht->key_len = ht->p.key_len;
	if (!params->hashfn) {
		ht->p.hashfn = jhash;

		if (!(ht->key_len & (sizeof(u32) - 1))) {
			ht->key_len /= sizeof(u32);
			ht->p.hashfn = rhashtable_jhash2;
		}
	}

	tbl = bucket_table_alloc(ht, size, GFP_KERNEL);
	if (tbl == NULL)
		return -ENOMEM;

	atomic_set(&ht->nelems, 0);

	RCU_INIT_POINTER(ht->tbl, tbl);

	INIT_WORK(&ht->run_work, rht_deferred_worker);
>>>>>>> v4.9.227

	return 0;
}
EXPORT_SYMBOL_GPL(rhashtable_init);

/**
<<<<<<< HEAD
 * rhashtable_destroy - destroy hash table
 * @ht:		the hash table to destroy
 *
 * Frees the bucket array. This function is not rcu safe, therefore the caller
 * has to make sure that no resizing may happen by unpublishing the hashtable
 * and waiting for the quiescent cycle before releasing the bucket array.
 */
void rhashtable_destroy(const struct rhashtable *ht)
{
	bucket_table_free(ht->tbl);
}
EXPORT_SYMBOL_GPL(rhashtable_destroy);

/**************************************************************************
 * Self Test
 **************************************************************************/

#ifdef CONFIG_TEST_RHASHTABLE

#define TEST_HT_SIZE	8
#define TEST_ENTRIES	2048
#define TEST_PTR	((void *) 0xdeadbeef)
#define TEST_NEXPANDS	4

static int test_mutex_is_held(void)
{
	return 1;
}

struct test_obj {
	void			*ptr;
	int			value;
	struct rhash_head	node;
};

static int __init test_rht_lookup(struct rhashtable *ht)
{
	unsigned int i;

	for (i = 0; i < TEST_ENTRIES * 2; i++) {
		struct test_obj *obj;
		bool expected = !(i % 2);
		u32 key = i;

		obj = rhashtable_lookup(ht, &key);

		if (expected && !obj) {
			pr_warn("Test failed: Could not find key %u\n", key);
			return -ENOENT;
		} else if (!expected && obj) {
			pr_warn("Test failed: Unexpected entry found for key %u\n",
				key);
			return -EEXIST;
		} else if (expected && obj) {
			if (obj->ptr != TEST_PTR || obj->value != i) {
				pr_warn("Test failed: Lookup value mismatch %p!=%p, %u!=%u\n",
					obj->ptr, TEST_PTR, obj->value, i);
				return -EINVAL;
			}
		}
	}

	return 0;
}

static void test_bucket_stats(struct rhashtable *ht,
				     struct bucket_table *tbl,
				     bool quiet)
{
	unsigned int cnt, i, total = 0;
	struct test_obj *obj;

	for (i = 0; i < tbl->size; i++) {
		cnt = 0;

		if (!quiet)
			pr_info(" [%#4x/%zu]", i, tbl->size);

		rht_for_each_entry_rcu(obj, tbl->buckets[i], node) {
			cnt++;
			total++;
			if (!quiet)
				pr_cont(" [%p],", obj);
		}

		if (!quiet)
			pr_cont("\n  [%#x] first element: %p, chain length: %u\n",
				i, tbl->buckets[i], cnt);
	}

	pr_info("  Traversal complete: counted=%u, nelems=%zu, entries=%d\n",
		total, ht->nelems, TEST_ENTRIES);
}

static int __init test_rhashtable(struct rhashtable *ht)
{
	struct bucket_table *tbl;
	struct test_obj *obj, *next;
	int err;
	unsigned int i;

	/*
	 * Insertion Test:
	 * Insert TEST_ENTRIES into table with all keys even numbers
	 */
	pr_info("  Adding %d keys\n", TEST_ENTRIES);
	for (i = 0; i < TEST_ENTRIES; i++) {
		struct test_obj *obj;

		obj = kzalloc(sizeof(*obj), GFP_KERNEL);
		if (!obj) {
			err = -ENOMEM;
			goto error;
		}

		obj->ptr = TEST_PTR;
		obj->value = i * 2;

		rhashtable_insert(ht, &obj->node, GFP_KERNEL);
	}

	rcu_read_lock();
	tbl = rht_dereference_rcu(ht->tbl, ht);
	test_bucket_stats(ht, tbl, true);
	test_rht_lookup(ht);
	rcu_read_unlock();

	for (i = 0; i < TEST_NEXPANDS; i++) {
		pr_info("  Table expansion iteration %u...\n", i);
		rhashtable_expand(ht, GFP_KERNEL);

		rcu_read_lock();
		pr_info("  Verifying lookups...\n");
		test_rht_lookup(ht);
		rcu_read_unlock();
	}

	for (i = 0; i < TEST_NEXPANDS; i++) {
		pr_info("  Table shrinkage iteration %u...\n", i);
		rhashtable_shrink(ht, GFP_KERNEL);

		rcu_read_lock();
		pr_info("  Verifying lookups...\n");
		test_rht_lookup(ht);
		rcu_read_unlock();
	}

	pr_info("  Deleting %d keys\n", TEST_ENTRIES);
	for (i = 0; i < TEST_ENTRIES; i++) {
		u32 key = i * 2;

		obj = rhashtable_lookup(ht, &key);
		BUG_ON(!obj);

		rhashtable_remove(ht, &obj->node, GFP_KERNEL);
		kfree(obj);
	}

	return 0;

error:
	tbl = rht_dereference_rcu(ht->tbl, ht);
	for (i = 0; i < tbl->size; i++)
		rht_for_each_entry_safe(obj, next, tbl->buckets[i], ht, node)
			kfree(obj);

	return err;
}

static int __init test_rht_init(void)
{
	struct rhashtable ht;
	struct rhashtable_params params = {
		.nelem_hint = TEST_HT_SIZE,
		.head_offset = offsetof(struct test_obj, node),
		.key_offset = offsetof(struct test_obj, value),
		.key_len = sizeof(int),
		.hashfn = arch_fast_hash,
		.mutex_is_held = &test_mutex_is_held,
		.grow_decision = rht_grow_above_75,
		.shrink_decision = rht_shrink_below_30,
	};
	int err;

	pr_info("Running resizable hashtable tests...\n");

	err = rhashtable_init(&ht, &params);
	if (err < 0) {
		pr_warn("Test failed: Unable to initialize hashtable: %d\n",
			err);
		return err;
	}

	err = test_rhashtable(&ht);

	rhashtable_destroy(&ht);

	return err;
}

subsys_initcall(test_rht_init);

#endif /* CONFIG_TEST_RHASHTABLE */
=======
 * rhltable_init - initialize a new hash list table
 * @hlt:	hash list table to be initialized
 * @params:	configuration parameters
 *
 * Initializes a new hash list table.
 *
 * See documentation for rhashtable_init.
 */
int rhltable_init(struct rhltable *hlt, const struct rhashtable_params *params)
{
	int err;

	/* No rhlist NULLs marking for now. */
	if (params->nulls_base)
		return -EINVAL;

	err = rhashtable_init(&hlt->ht, params);
	hlt->ht.rhlist = true;
	return err;
}
EXPORT_SYMBOL_GPL(rhltable_init);

static void rhashtable_free_one(struct rhashtable *ht, struct rhash_head *obj,
				void (*free_fn)(void *ptr, void *arg),
				void *arg)
{
	struct rhlist_head *list;

	if (!ht->rhlist) {
		free_fn(rht_obj(ht, obj), arg);
		return;
	}

	list = container_of(obj, struct rhlist_head, rhead);
	do {
		obj = &list->rhead;
		list = rht_dereference(list->next, ht);
		free_fn(rht_obj(ht, obj), arg);
	} while (list);
}

/**
 * rhashtable_free_and_destroy - free elements and destroy hash table
 * @ht:		the hash table to destroy
 * @free_fn:	callback to release resources of element
 * @arg:	pointer passed to free_fn
 *
 * Stops an eventual async resize. If defined, invokes free_fn for each
 * element to releasal resources. Please note that RCU protected
 * readers may still be accessing the elements. Releasing of resources
 * must occur in a compatible manner. Then frees the bucket array.
 *
 * This function will eventually sleep to wait for an async resize
 * to complete. The caller is responsible that no further write operations
 * occurs in parallel.
 */
void rhashtable_free_and_destroy(struct rhashtable *ht,
				 void (*free_fn)(void *ptr, void *arg),
				 void *arg)
{
	const struct bucket_table *tbl;
	unsigned int i;

	cancel_work_sync(&ht->run_work);

	mutex_lock(&ht->mutex);
	tbl = rht_dereference(ht->tbl, ht);
	if (free_fn) {
		for (i = 0; i < tbl->size; i++) {
			struct rhash_head *pos, *next;

			cond_resched();
			for (pos = rht_dereference(tbl->buckets[i], ht),
			     next = !rht_is_a_nulls(pos) ?
					rht_dereference(pos->next, ht) : NULL;
			     !rht_is_a_nulls(pos);
			     pos = next,
			     next = !rht_is_a_nulls(pos) ?
					rht_dereference(pos->next, ht) : NULL)
				rhashtable_free_one(ht, pos, free_fn, arg);
		}
	}

	bucket_table_free(tbl);
	mutex_unlock(&ht->mutex);
}
EXPORT_SYMBOL_GPL(rhashtable_free_and_destroy);

void rhashtable_destroy(struct rhashtable *ht)
{
	return rhashtable_free_and_destroy(ht, NULL, NULL);
}
EXPORT_SYMBOL_GPL(rhashtable_destroy);
>>>>>>> v4.9.227
