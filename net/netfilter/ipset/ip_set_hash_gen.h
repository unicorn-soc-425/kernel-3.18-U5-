/* Copyright (C) 2013 Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _IP_SET_HASH_GEN_H
#define _IP_SET_HASH_GEN_H

#include <linux/rcupdate.h>
#include <linux/jhash.h>
<<<<<<< HEAD
#include <linux/netfilter/ipset/ip_set_timeout.h>
#ifndef rcu_dereference_bh
#define rcu_dereference_bh(p)	rcu_dereference(p)
#endif
=======
#include <linux/types.h>
#include <linux/netfilter/ipset/ip_set_timeout.h>

#define __ipset_dereference_protected(p, c)	rcu_dereference_protected(p, c)
#define ipset_dereference_protected(p, set) \
	__ipset_dereference_protected(p, spin_is_locked(&(set)->lock))
>>>>>>> v4.9.227

#define rcu_dereference_bh_nfnl(p)	rcu_dereference_bh_check(p, 1)

/* Hashing which uses arrays to resolve clashing. The hash table is resized
 * (doubled) when searching becomes too long.
 * Internally jhash is used with the assumption that the size of the
<<<<<<< HEAD
 * stored data is a multiple of sizeof(u32). If storage supports timeout,
 * the timeout field must be the last one in the data structure - that field
 * is ignored when computing the hash key.
=======
 * stored data is a multiple of sizeof(u32).
>>>>>>> v4.9.227
 *
 * Readers and resizing
 *
 * Resizing can be triggered by userspace command only, and those
 * are serialized by the nfnl mutex. During resizing the set is
 * read-locked, so the only possible concurrent operations are
 * the kernel side readers. Those must be protected by proper RCU locking.
 */

/* Number of elements to store in an initial array block */
#define AHASH_INIT_SIZE			4
/* Max number of elements to store in an array block */
<<<<<<< HEAD
#define AHASH_MAX_SIZE			(3*AHASH_INIT_SIZE)
=======
#define AHASH_MAX_SIZE			(3 * AHASH_INIT_SIZE)
/* Max muber of elements in the array block when tuned */
#define AHASH_MAX_TUNED			64
>>>>>>> v4.9.227

/* Max number of elements can be tuned */
#ifdef IP_SET_HASH_WITH_MULTI
#define AHASH_MAX(h)			((h)->ahash_max)

static inline u8
tune_ahash_max(u8 curr, u32 multi)
{
	u32 n;

	if (multi < curr)
		return curr;

	n = curr + AHASH_INIT_SIZE;
	/* Currently, at listing one hash bucket must fit into a message.
	 * Therefore we have a hard limit here.
	 */
<<<<<<< HEAD
	return n > curr && n <= 64 ? n : curr;
}
=======
	return n > curr && n <= AHASH_MAX_TUNED ? n : curr;
}

>>>>>>> v4.9.227
#define TUNE_AHASH_MAX(h, multi)	\
	((h)->ahash_max = tune_ahash_max((h)->ahash_max, multi))
#else
#define AHASH_MAX(h)			AHASH_MAX_SIZE
#define TUNE_AHASH_MAX(h, multi)
#endif

/* A hash bucket */
struct hbucket {
<<<<<<< HEAD
	void *value;		/* the array of the values */
	u8 size;		/* size of the array */
	u8 pos;			/* position of the first free entry */
=======
	struct rcu_head rcu;	/* for call_rcu_bh */
	/* Which positions are used in the array */
	DECLARE_BITMAP(used, AHASH_MAX_TUNED);
	u8 size;		/* size of the array */
	u8 pos;			/* position of the first free entry */
	unsigned char value[0]	/* the array of the values */
		__aligned(__alignof__(u64));
>>>>>>> v4.9.227
};

/* The hash table: the table size stored here in order to make resizing easy */
struct htable {
<<<<<<< HEAD
	u8 htable_bits;		/* size of hash table == 2^htable_bits */
	struct hbucket bucket[0]; /* hashtable buckets */
};

#define hbucket(h, i)		(&((h)->bucket[i]))
=======
	atomic_t ref;		/* References for resizing */
	atomic_t uref;		/* References for dumping */
	u8 htable_bits;		/* size of hash table == 2^htable_bits */
	struct hbucket __rcu *bucket[0]; /* hashtable buckets */
};

#define hbucket(h, i)		((h)->bucket[i])
>>>>>>> v4.9.227

#ifndef IPSET_NET_COUNT
#define IPSET_NET_COUNT		1
#endif

/* Book-keeping of the prefixes added to the set */
struct net_prefixes {
<<<<<<< HEAD
	u32 nets[IPSET_NET_COUNT]; /* number of elements per cidr */
	u8 cidr[IPSET_NET_COUNT];  /* the different cidr values in the set */
=======
	u32 nets[IPSET_NET_COUNT]; /* number of elements for this cidr */
	u8 cidr[IPSET_NET_COUNT];  /* the cidr value */
>>>>>>> v4.9.227
};

/* Compute the hash table size */
static size_t
htable_size(u8 hbits)
{
	size_t hsize;

	/* We must fit both into u32 in jhash and size_t */
	if (hbits > 31)
		return 0;
	hsize = jhash_size(hbits);
<<<<<<< HEAD
	if ((((size_t)-1) - sizeof(struct htable))/sizeof(struct hbucket)
	    < hsize)
		return 0;

	return hsize * sizeof(struct hbucket) + sizeof(struct htable);
=======
	if ((((size_t)-1) - sizeof(struct htable)) / sizeof(struct hbucket *)
	    < hsize)
		return 0;

	return hsize * sizeof(struct hbucket *) + sizeof(struct htable);
>>>>>>> v4.9.227
}

/* Compute htable_bits from the user input parameter hashsize */
static u8
htable_bits(u32 hashsize)
{
	/* Assume that hashsize == 2^htable_bits */
	u8 bits = fls(hashsize - 1);
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
	if (jhash_size(bits) != hashsize)
		/* Round up to the first 2^n value */
		bits = fls(hashsize);

	return bits;
}

<<<<<<< HEAD
static int
hbucket_elem_add(struct hbucket *n, u8 ahash_max, size_t dsize)
{
	if (n->pos >= n->size) {
		void *tmp;

		if (n->size >= ahash_max)
			/* Trigger rehashing */
			return -EAGAIN;

		tmp = kzalloc((n->size + AHASH_INIT_SIZE) * dsize,
			      GFP_ATOMIC);
		if (!tmp)
			return -ENOMEM;
		if (n->size) {
			memcpy(tmp, n->value, n->size * dsize);
			kfree(n->value);
		}
		n->value = tmp;
		n->size += AHASH_INIT_SIZE;
	}
	return 0;
}

=======
>>>>>>> v4.9.227
#ifdef IP_SET_HASH_WITH_NETS
#if IPSET_NET_COUNT > 1
#define __CIDR(cidr, i)		(cidr[i])
#else
#define __CIDR(cidr, i)		(cidr)
#endif
<<<<<<< HEAD
#ifdef IP_SET_HASH_WITH_NETS_PACKED
/* When cidr is packed with nomatch, cidr - 1 is stored in the entry */
#define CIDR(cidr, i)		(__CIDR(cidr, i) + 1)
#else
#define CIDR(cidr, i)		(__CIDR(cidr, i))
#endif

#define SET_HOST_MASK(family)	(family == AF_INET ? 32 : 128)

#ifdef IP_SET_HASH_WITH_MULTI
#define NLEN(family)		(SET_HOST_MASK(family) + 1)
#else
#define NLEN(family)		SET_HOST_MASK(family)
=======

/* cidr + 1 is stored in net_prefixes to support /0 */
#define NCIDR_PUT(cidr)		((cidr) + 1)
#define NCIDR_GET(cidr)		((cidr) - 1)

#ifdef IP_SET_HASH_WITH_NETS_PACKED
/* When cidr is packed with nomatch, cidr - 1 is stored in the data entry */
#define DCIDR_PUT(cidr)		((cidr) - 1)
#define DCIDR_GET(cidr, i)	(__CIDR(cidr, i) + 1)
#else
#define DCIDR_PUT(cidr)		(cidr)
#define DCIDR_GET(cidr, i)	__CIDR(cidr, i)
#endif

#define INIT_CIDR(cidr, host_mask)	\
	DCIDR_PUT(((cidr) ? NCIDR_GET(cidr) : host_mask))

#define SET_HOST_MASK(family)	(family == AF_INET ? 32 : 128)

#ifdef IP_SET_HASH_WITH_NET0
/* cidr from 0 to SET_HOST_MASK() value and c = cidr + 1 */
#define NLEN(family)		(SET_HOST_MASK(family) + 1)
#define CIDR_POS(c)		((c) - 1)
#else
/* cidr from 1 to SET_HOST_MASK() value and c = cidr + 1 */
#define NLEN(family)		SET_HOST_MASK(family)
#define CIDR_POS(c)		((c) - 2)
>>>>>>> v4.9.227
#endif

#else
#define NLEN(family)		0
#endif /* IP_SET_HASH_WITH_NETS */

#endif /* _IP_SET_HASH_GEN_H */

/* Family dependent templates */

#undef ahash_data
#undef mtype_data_equal
#undef mtype_do_data_match
#undef mtype_data_set_flags
<<<<<<< HEAD
=======
#undef mtype_data_reset_elem
>>>>>>> v4.9.227
#undef mtype_data_reset_flags
#undef mtype_data_netmask
#undef mtype_data_list
#undef mtype_data_next
#undef mtype_elem

#undef mtype_ahash_destroy
#undef mtype_ext_cleanup
#undef mtype_add_cidr
#undef mtype_del_cidr
#undef mtype_ahash_memsize
#undef mtype_flush
#undef mtype_destroy
<<<<<<< HEAD
#undef mtype_gc_init
=======
>>>>>>> v4.9.227
#undef mtype_same_set
#undef mtype_kadt
#undef mtype_uadt
#undef mtype

#undef mtype_add
#undef mtype_del
#undef mtype_test_cidrs
#undef mtype_test
<<<<<<< HEAD
=======
#undef mtype_uref
>>>>>>> v4.9.227
#undef mtype_expire
#undef mtype_resize
#undef mtype_head
#undef mtype_list
#undef mtype_gc
#undef mtype_gc_init
#undef mtype_variant
#undef mtype_data_match

#undef HKEY

#define mtype_data_equal	IPSET_TOKEN(MTYPE, _data_equal)
#ifdef IP_SET_HASH_WITH_NETS
#define mtype_do_data_match	IPSET_TOKEN(MTYPE, _do_data_match)
#else
#define mtype_do_data_match(d)	1
#endif
#define mtype_data_set_flags	IPSET_TOKEN(MTYPE, _data_set_flags)
#define mtype_data_reset_elem	IPSET_TOKEN(MTYPE, _data_reset_elem)
#define mtype_data_reset_flags	IPSET_TOKEN(MTYPE, _data_reset_flags)
#define mtype_data_netmask	IPSET_TOKEN(MTYPE, _data_netmask)
#define mtype_data_list		IPSET_TOKEN(MTYPE, _data_list)
#define mtype_data_next		IPSET_TOKEN(MTYPE, _data_next)
#define mtype_elem		IPSET_TOKEN(MTYPE, _elem)
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
#define mtype_ahash_destroy	IPSET_TOKEN(MTYPE, _ahash_destroy)
#define mtype_ext_cleanup	IPSET_TOKEN(MTYPE, _ext_cleanup)
#define mtype_add_cidr		IPSET_TOKEN(MTYPE, _add_cidr)
#define mtype_del_cidr		IPSET_TOKEN(MTYPE, _del_cidr)
#define mtype_ahash_memsize	IPSET_TOKEN(MTYPE, _ahash_memsize)
#define mtype_flush		IPSET_TOKEN(MTYPE, _flush)
#define mtype_destroy		IPSET_TOKEN(MTYPE, _destroy)
<<<<<<< HEAD
#define mtype_gc_init		IPSET_TOKEN(MTYPE, _gc_init)
=======
>>>>>>> v4.9.227
#define mtype_same_set		IPSET_TOKEN(MTYPE, _same_set)
#define mtype_kadt		IPSET_TOKEN(MTYPE, _kadt)
#define mtype_uadt		IPSET_TOKEN(MTYPE, _uadt)
#define mtype			MTYPE

#define mtype_add		IPSET_TOKEN(MTYPE, _add)
#define mtype_del		IPSET_TOKEN(MTYPE, _del)
#define mtype_test_cidrs	IPSET_TOKEN(MTYPE, _test_cidrs)
#define mtype_test		IPSET_TOKEN(MTYPE, _test)
<<<<<<< HEAD
=======
#define mtype_uref		IPSET_TOKEN(MTYPE, _uref)
>>>>>>> v4.9.227
#define mtype_expire		IPSET_TOKEN(MTYPE, _expire)
#define mtype_resize		IPSET_TOKEN(MTYPE, _resize)
#define mtype_head		IPSET_TOKEN(MTYPE, _head)
#define mtype_list		IPSET_TOKEN(MTYPE, _list)
#define mtype_gc		IPSET_TOKEN(MTYPE, _gc)
<<<<<<< HEAD
#define mtype_variant		IPSET_TOKEN(MTYPE, _variant)
#define mtype_data_match	IPSET_TOKEN(MTYPE, _data_match)

=======
#define mtype_gc_init		IPSET_TOKEN(MTYPE, _gc_init)
#define mtype_variant		IPSET_TOKEN(MTYPE, _variant)
#define mtype_data_match	IPSET_TOKEN(MTYPE, _data_match)

#ifndef MTYPE
#error "MTYPE is not defined!"
#endif

#ifndef HOST_MASK
#error "HOST_MASK is not defined!"
#endif

>>>>>>> v4.9.227
#ifndef HKEY_DATALEN
#define HKEY_DATALEN		sizeof(struct mtype_elem)
#endif

#define HKEY(data, initval, htable_bits)			\
<<<<<<< HEAD
(jhash2((u32 *)(data), HKEY_DATALEN/sizeof(u32), initval)	\
	& jhash_mask(htable_bits))

#ifndef htype
=======
(jhash2((u32 *)(data), HKEY_DATALEN / sizeof(u32), initval)	\
	& jhash_mask(htable_bits))

#ifndef htype
#ifndef HTYPE
#error "HTYPE is not defined!"
#endif /* HTYPE */
>>>>>>> v4.9.227
#define htype			HTYPE

/* The generic hash structure */
struct htype {
	struct htable __rcu *table; /* the hash table */
	u32 maxelem;		/* max elements in the hash */
	u32 elements;		/* current element (vs timeout) */
	u32 initval;		/* random jhash init value */
#ifdef IP_SET_HASH_WITH_MARKMASK
	u32 markmask;		/* markmask value for mark mask to store */
#endif
	struct timer_list gc;	/* garbage collection when timeout enabled */
	struct mtype_elem next; /* temporary storage for uadd */
#ifdef IP_SET_HASH_WITH_MULTI
	u8 ahash_max;		/* max elements in an array block */
#endif
#ifdef IP_SET_HASH_WITH_NETMASK
	u8 netmask;		/* netmask value for subnets to store */
#endif
<<<<<<< HEAD
#ifdef IP_SET_HASH_WITH_RBTREE
	struct rb_root rbtree;
#endif
=======
>>>>>>> v4.9.227
#ifdef IP_SET_HASH_WITH_NETS
	struct net_prefixes nets[0]; /* book-keeping of prefixes */
#endif
};
<<<<<<< HEAD
#endif

#ifdef IP_SET_HASH_WITH_NETS
/* Network cidr size book keeping when the hash stores different
 * sized networks */
=======
#endif /* htype */

#ifdef IP_SET_HASH_WITH_NETS
/* Network cidr size book keeping when the hash stores different
 * sized networks. cidr == real cidr + 1 to support /0.
 */
>>>>>>> v4.9.227
static void
mtype_add_cidr(struct htype *h, u8 cidr, u8 nets_length, u8 n)
{
	int i, j;

	/* Add in increasing prefix order, so larger cidr first */
<<<<<<< HEAD
	for (i = 0, j = -1; i < nets_length && h->nets[i].nets[n]; i++) {
		if (j != -1)
			continue;
		else if (h->nets[i].cidr[n] < cidr)
			j = i;
		else if (h->nets[i].cidr[n] == cidr) {
			h->nets[i].nets[n]++;
=======
	for (i = 0, j = -1; i < nets_length && h->nets[i].cidr[n]; i++) {
		if (j != -1) {
			continue;
		} else if (h->nets[i].cidr[n] < cidr) {
			j = i;
		} else if (h->nets[i].cidr[n] == cidr) {
			h->nets[CIDR_POS(cidr)].nets[n]++;
>>>>>>> v4.9.227
			return;
		}
	}
	if (j != -1) {
<<<<<<< HEAD
		for (; i > j; i--) {
			h->nets[i].cidr[n] = h->nets[i - 1].cidr[n];
			h->nets[i].nets[n] = h->nets[i - 1].nets[n];
		}
	}
	h->nets[i].cidr[n] = cidr;
	h->nets[i].nets[n] = 1;
=======
		for (; i > j; i--)
			h->nets[i].cidr[n] = h->nets[i - 1].cidr[n];
	}
	h->nets[i].cidr[n] = cidr;
	h->nets[CIDR_POS(cidr)].nets[n] = 1;
>>>>>>> v4.9.227
}

static void
mtype_del_cidr(struct htype *h, u8 cidr, u8 nets_length, u8 n)
{
	u8 i, j, net_end = nets_length - 1;

	for (i = 0; i < nets_length; i++) {
<<<<<<< HEAD
	        if (h->nets[i].cidr[n] != cidr)
	                continue;
                if (h->nets[i].nets[n] > 1 || i == net_end ||
                    h->nets[i + 1].nets[n] == 0) {
                        h->nets[i].nets[n]--;
                        return;
                }
                for (j = i; j < net_end && h->nets[j].nets[n]; j++) {
		        h->nets[j].cidr[n] = h->nets[j + 1].cidr[n];
		        h->nets[j].nets[n] = h->nets[j + 1].nets[n];
                }
                h->nets[j].nets[n] = 0;
                return;
=======
		if (h->nets[i].cidr[n] != cidr)
			continue;
		h->nets[CIDR_POS(cidr)].nets[n]--;
		if (h->nets[CIDR_POS(cidr)].nets[n] > 0)
			return;
		for (j = i; j < net_end && h->nets[j].cidr[n]; j++)
			h->nets[j].cidr[n] = h->nets[j + 1].cidr[n];
		h->nets[j].cidr[n] = 0;
		return;
>>>>>>> v4.9.227
	}
}
#endif

/* Calculate the actual memory size of the set data */
static size_t
mtype_ahash_memsize(const struct htype *h, const struct htable *t,
		    u8 nets_length, size_t dsize)
{
	u32 i;
<<<<<<< HEAD
	size_t memsize = sizeof(*h)
			 + sizeof(*t)
#ifdef IP_SET_HASH_WITH_NETS
			 + sizeof(struct net_prefixes) * nets_length
#endif
			 + jhash_size(t->htable_bits) * sizeof(struct hbucket);

	for (i = 0; i < jhash_size(t->htable_bits); i++)
		memsize += t->bucket[i].size * dsize;
=======
	struct hbucket *n;
	size_t memsize = sizeof(*h) + sizeof(*t);

#ifdef IP_SET_HASH_WITH_NETS
	memsize += sizeof(struct net_prefixes) * nets_length;
#endif
	for (i = 0; i < jhash_size(t->htable_bits); i++) {
		n = rcu_dereference_bh(hbucket(t, i));
		if (!n)
			continue;
		memsize += sizeof(struct hbucket) + n->size * dsize;
	}
>>>>>>> v4.9.227

	return memsize;
}

/* Get the ith element from the array block n */
#define ahash_data(n, i, dsize)	\
	((struct mtype_elem *)((n)->value + ((i) * (dsize))))

static void
mtype_ext_cleanup(struct ip_set *set, struct hbucket *n)
{
	int i;

	for (i = 0; i < n->pos; i++)
<<<<<<< HEAD
		ip_set_ext_destroy(set, ahash_data(n, i, set->dsize));
=======
		if (test_bit(i, n->used))
			ip_set_ext_destroy(set, ahash_data(n, i, set->dsize));
>>>>>>> v4.9.227
}

/* Flush a hash type of set: destroy all elements */
static void
mtype_flush(struct ip_set *set)
{
	struct htype *h = set->data;
	struct htable *t;
	struct hbucket *n;
	u32 i;

<<<<<<< HEAD
	t = rcu_dereference_bh_nfnl(h->table);
	for (i = 0; i < jhash_size(t->htable_bits); i++) {
		n = hbucket(t, i);
		if (n->size) {
			if (set->extensions & IPSET_EXT_DESTROY)
				mtype_ext_cleanup(set, n);
			n->size = n->pos = 0;
			/* FIXME: use slab cache */
			kfree(n->value);
		}
=======
	t = ipset_dereference_protected(h->table, set);
	for (i = 0; i < jhash_size(t->htable_bits); i++) {
		n = __ipset_dereference_protected(hbucket(t, i), 1);
		if (!n)
			continue;
		if (set->extensions & IPSET_EXT_DESTROY)
			mtype_ext_cleanup(set, n);
		/* FIXME: use slab cache */
		rcu_assign_pointer(hbucket(t, i), NULL);
		kfree_rcu(n, rcu);
>>>>>>> v4.9.227
	}
#ifdef IP_SET_HASH_WITH_NETS
	memset(h->nets, 0, sizeof(struct net_prefixes) * NLEN(set->family));
#endif
	h->elements = 0;
}

/* Destroy the hashtable part of the set */
static void
mtype_ahash_destroy(struct ip_set *set, struct htable *t, bool ext_destroy)
{
	struct hbucket *n;
	u32 i;

	for (i = 0; i < jhash_size(t->htable_bits); i++) {
<<<<<<< HEAD
		n = hbucket(t, i);
		if (n->size) {
			if (set->extensions & IPSET_EXT_DESTROY && ext_destroy)
				mtype_ext_cleanup(set, n);
			/* FIXME: use slab cache */
			kfree(n->value);
		}
=======
		n = __ipset_dereference_protected(hbucket(t, i), 1);
		if (!n)
			continue;
		if (set->extensions & IPSET_EXT_DESTROY && ext_destroy)
			mtype_ext_cleanup(set, n);
		/* FIXME: use slab cache */
		kfree(n);
>>>>>>> v4.9.227
	}

	ip_set_free(t);
}

/* Destroy a hash type of set */
static void
mtype_destroy(struct ip_set *set)
{
	struct htype *h = set->data;

<<<<<<< HEAD
	if (set->extensions & IPSET_EXT_TIMEOUT)
		del_timer_sync(&h->gc);

	mtype_ahash_destroy(set, rcu_dereference_bh_nfnl(h->table), true);
#ifdef IP_SET_HASH_WITH_RBTREE
	rbtree_destroy(&h->rbtree);
#endif
=======
	if (SET_WITH_TIMEOUT(set))
		del_timer_sync(&h->gc);

	mtype_ahash_destroy(set,
			    __ipset_dereference_protected(h->table, 1), true);
>>>>>>> v4.9.227
	kfree(h);

	set->data = NULL;
}

static void
mtype_gc_init(struct ip_set *set, void (*gc)(unsigned long ul_set))
{
	struct htype *h = set->data;

	init_timer(&h->gc);
<<<<<<< HEAD
	h->gc.data = (unsigned long) set;
=======
	h->gc.data = (unsigned long)set;
>>>>>>> v4.9.227
	h->gc.function = gc;
	h->gc.expires = jiffies + IPSET_GC_PERIOD(set->timeout) * HZ;
	add_timer(&h->gc);
	pr_debug("gc initialized, run in every %u\n",
		 IPSET_GC_PERIOD(set->timeout));
}

static bool
mtype_same_set(const struct ip_set *a, const struct ip_set *b)
{
	const struct htype *x = a->data;
	const struct htype *y = b->data;

	/* Resizing changes htable_bits, so we ignore it */
	return x->maxelem == y->maxelem &&
	       a->timeout == b->timeout &&
#ifdef IP_SET_HASH_WITH_NETMASK
	       x->netmask == y->netmask &&
#endif
#ifdef IP_SET_HASH_WITH_MARKMASK
	       x->markmask == y->markmask &&
#endif
	       a->extensions == b->extensions;
}

/* Delete expired elements from the hashtable */
static void
mtype_expire(struct ip_set *set, struct htype *h, u8 nets_length, size_t dsize)
{
	struct htable *t;
<<<<<<< HEAD
	struct hbucket *n;
	struct mtype_elem *data;
	u32 i;
	int j;
=======
	struct hbucket *n, *tmp;
	struct mtype_elem *data;
	u32 i, j, d;
>>>>>>> v4.9.227
#ifdef IP_SET_HASH_WITH_NETS
	u8 k;
#endif

<<<<<<< HEAD
	rcu_read_lock_bh();
	t = rcu_dereference_bh(h->table);
	for (i = 0; i < jhash_size(t->htable_bits); i++) {
		n = hbucket(t, i);
		for (j = 0; j < n->pos; j++) {
			data = ahash_data(n, j, dsize);
			if (ip_set_timeout_expired(ext_timeout(data, set))) {
				pr_debug("expired %u/%u\n", i, j);
#ifdef IP_SET_HASH_WITH_NETS
				for (k = 0; k < IPSET_NET_COUNT; k++)
					mtype_del_cidr(h, CIDR(data->cidr, k),
						       nets_length, k);
#endif
				ip_set_ext_destroy(set, data);
				if (j != n->pos - 1)
					/* Not last one */
					memcpy(data,
					       ahash_data(n, n->pos - 1, dsize),
					       dsize);
				n->pos--;
				h->elements--;
			}
		}
		if (n->pos + AHASH_INIT_SIZE < n->size) {
			void *tmp = kzalloc((n->size - AHASH_INIT_SIZE)
					    * dsize,
					    GFP_ATOMIC);
			if (!tmp)
				/* Still try to delete expired elements */
				continue;
			n->size -= AHASH_INIT_SIZE;
			memcpy(tmp, n->value, n->size * dsize);
			kfree(n->value);
			n->value = tmp;
		}
	}
	rcu_read_unlock_bh();
=======
	t = ipset_dereference_protected(h->table, set);
	for (i = 0; i < jhash_size(t->htable_bits); i++) {
		n = __ipset_dereference_protected(hbucket(t, i), 1);
		if (!n)
			continue;
		for (j = 0, d = 0; j < n->pos; j++) {
			if (!test_bit(j, n->used)) {
				d++;
				continue;
			}
			data = ahash_data(n, j, dsize);
			if (ip_set_timeout_expired(ext_timeout(data, set))) {
				pr_debug("expired %u/%u\n", i, j);
				clear_bit(j, n->used);
				smp_mb__after_atomic();
#ifdef IP_SET_HASH_WITH_NETS
				for (k = 0; k < IPSET_NET_COUNT; k++)
					mtype_del_cidr(h,
						NCIDR_PUT(DCIDR_GET(data->cidr,
								    k)),
						nets_length, k);
#endif
				ip_set_ext_destroy(set, data);
				h->elements--;
				d++;
			}
		}
		if (d >= AHASH_INIT_SIZE) {
			if (d >= n->size) {
				rcu_assign_pointer(hbucket(t, i), NULL);
				kfree_rcu(n, rcu);
				continue;
			}
			tmp = kzalloc(sizeof(*tmp) +
				      (n->size - AHASH_INIT_SIZE) * dsize,
				      GFP_ATOMIC);
			if (!tmp)
				/* Still try to delete expired elements */
				continue;
			tmp->size = n->size - AHASH_INIT_SIZE;
			for (j = 0, d = 0; j < n->pos; j++) {
				if (!test_bit(j, n->used))
					continue;
				data = ahash_data(n, j, dsize);
				memcpy(tmp->value + d * dsize, data, dsize);
				set_bit(d, tmp->used);
				d++;
			}
			tmp->pos = d;
			rcu_assign_pointer(hbucket(t, i), tmp);
			kfree_rcu(n, rcu);
		}
	}
>>>>>>> v4.9.227
}

static void
mtype_gc(unsigned long ul_set)
{
<<<<<<< HEAD
	struct ip_set *set = (struct ip_set *) ul_set;
	struct htype *h = set->data;

	pr_debug("called\n");
	write_lock_bh(&set->lock);
	mtype_expire(set, h, NLEN(set->family), set->dsize);
	write_unlock_bh(&set->lock);
=======
	struct ip_set *set = (struct ip_set *)ul_set;
	struct htype *h = set->data;

	pr_debug("called\n");
	spin_lock_bh(&set->lock);
	mtype_expire(set, h, NLEN(set->family), set->dsize);
	spin_unlock_bh(&set->lock);
>>>>>>> v4.9.227

	h->gc.expires = jiffies + IPSET_GC_PERIOD(set->timeout) * HZ;
	add_timer(&h->gc);
}

/* Resize a hash: create a new hash table with doubling the hashsize
 * and inserting the elements to it. Repeat until we succeed or
<<<<<<< HEAD
 * fail due to memory pressures. */
=======
 * fail due to memory pressures.
 */
>>>>>>> v4.9.227
static int
mtype_resize(struct ip_set *set, bool retried)
{
	struct htype *h = set->data;
<<<<<<< HEAD
	struct htable *t, *orig = rcu_dereference_bh_nfnl(h->table);
	u8 htable_bits = orig->htable_bits;
#ifdef IP_SET_HASH_WITH_NETS
	u8 flags;
=======
	struct htable *t, *orig;
	u8 htable_bits;
	size_t dsize = set->dsize;
#ifdef IP_SET_HASH_WITH_NETS
	u8 flags;
	struct mtype_elem *tmp;
>>>>>>> v4.9.227
#endif
	struct mtype_elem *data;
	struct mtype_elem *d;
	struct hbucket *n, *m;
<<<<<<< HEAD
	u32 i, j;
	int ret;

	/* Try to cleanup once */
	if (SET_WITH_TIMEOUT(set) && !retried) {
		i = h->elements;
		write_lock_bh(&set->lock);
		mtype_expire(set, set->data, NLEN(set->family), set->dsize);
		write_unlock_bh(&set->lock);
		if (h->elements < i)
			return 0;
	}
=======
	u32 i, j, key;
	int ret;

#ifdef IP_SET_HASH_WITH_NETS
	tmp = kmalloc(dsize, GFP_KERNEL);
	if (!tmp)
		return -ENOMEM;
#endif
	rcu_read_lock_bh();
	orig = rcu_dereference_bh_nfnl(h->table);
	htable_bits = orig->htable_bits;
	rcu_read_unlock_bh();
>>>>>>> v4.9.227

retry:
	ret = 0;
	htable_bits++;
<<<<<<< HEAD
	pr_debug("attempt to resize set %s from %u to %u, t %p\n",
		 set->name, orig->htable_bits, htable_bits, orig);
=======
>>>>>>> v4.9.227
	if (!htable_bits) {
		/* In case we have plenty of memory :-) */
		pr_warn("Cannot increase the hashsize of set %s further\n",
			set->name);
<<<<<<< HEAD
		return -IPSET_ERR_HASH_FULL;
	}
	t = ip_set_alloc(sizeof(*t)
			 + jhash_size(htable_bits) * sizeof(struct hbucket));
	if (!t)
		return -ENOMEM;
	t->htable_bits = htable_bits;

	read_lock_bh(&set->lock);
	for (i = 0; i < jhash_size(orig->htable_bits); i++) {
		n = hbucket(orig, i);
		for (j = 0; j < n->pos; j++) {
			data = ahash_data(n, j, set->dsize);
#ifdef IP_SET_HASH_WITH_NETS
			flags = 0;
			mtype_data_reset_flags(data, &flags);
#endif
			m = hbucket(t, HKEY(data, h->initval, htable_bits));
			ret = hbucket_elem_add(m, AHASH_MAX(h), set->dsize);
			if (ret < 0) {
#ifdef IP_SET_HASH_WITH_NETS
				mtype_data_reset_flags(data, &flags);
#endif
				read_unlock_bh(&set->lock);
				mtype_ahash_destroy(set, t, false);
				if (ret == -EAGAIN)
					goto retry;
				return ret;
			}
			d = ahash_data(m, m->pos++, set->dsize);
			memcpy(d, data, set->dsize);
=======
		ret = -IPSET_ERR_HASH_FULL;
		goto out;
	}
	t = ip_set_alloc(htable_size(htable_bits));
	if (!t) {
		ret = -ENOMEM;
		goto out;
	}
	t->htable_bits = htable_bits;

	spin_lock_bh(&set->lock);
	orig = __ipset_dereference_protected(h->table, 1);
	/* There can't be another parallel resizing, but dumping is possible */
	atomic_set(&orig->ref, 1);
	atomic_inc(&orig->uref);
	pr_debug("attempt to resize set %s from %u to %u, t %p\n",
		 set->name, orig->htable_bits, htable_bits, orig);
	for (i = 0; i < jhash_size(orig->htable_bits); i++) {
		n = __ipset_dereference_protected(hbucket(orig, i), 1);
		if (!n)
			continue;
		for (j = 0; j < n->pos; j++) {
			if (!test_bit(j, n->used))
				continue;
			data = ahash_data(n, j, dsize);
#ifdef IP_SET_HASH_WITH_NETS
			/* We have readers running parallel with us,
			 * so the live data cannot be modified.
			 */
			flags = 0;
			memcpy(tmp, data, dsize);
			data = tmp;
			mtype_data_reset_flags(data, &flags);
#endif
			key = HKEY(data, h->initval, htable_bits);
			m = __ipset_dereference_protected(hbucket(t, key), 1);
			if (!m) {
				m = kzalloc(sizeof(*m) +
					    AHASH_INIT_SIZE * dsize,
					    GFP_ATOMIC);
				if (!m) {
					ret = -ENOMEM;
					goto cleanup;
				}
				m->size = AHASH_INIT_SIZE;
				RCU_INIT_POINTER(hbucket(t, key), m);
			} else if (m->pos >= m->size) {
				struct hbucket *ht;

				if (m->size >= AHASH_MAX(h)) {
					ret = -EAGAIN;
				} else {
					ht = kzalloc(sizeof(*ht) +
						(m->size + AHASH_INIT_SIZE)
						* dsize,
						GFP_ATOMIC);
					if (!ht)
						ret = -ENOMEM;
				}
				if (ret < 0)
					goto cleanup;
				memcpy(ht, m, sizeof(struct hbucket) +
					      m->size * dsize);
				ht->size = m->size + AHASH_INIT_SIZE;
				kfree(m);
				m = ht;
				RCU_INIT_POINTER(hbucket(t, key), ht);
			}
			d = ahash_data(m, m->pos, dsize);
			memcpy(d, data, dsize);
			set_bit(m->pos++, m->used);
>>>>>>> v4.9.227
#ifdef IP_SET_HASH_WITH_NETS
			mtype_data_reset_flags(d, &flags);
#endif
		}
	}
<<<<<<< HEAD

	rcu_assign_pointer(h->table, t);
	read_unlock_bh(&set->lock);
=======
	rcu_assign_pointer(h->table, t);

	spin_unlock_bh(&set->lock);
>>>>>>> v4.9.227

	/* Give time to other readers of the set */
	synchronize_rcu_bh();

	pr_debug("set %s resized from %u (%p) to %u (%p)\n", set->name,
		 orig->htable_bits, orig, t->htable_bits, t);
<<<<<<< HEAD
	mtype_ahash_destroy(set, orig, false);

	return 0;
}

/* Add an element to a hash and update the internal counters when succeeded,
 * otherwise report the proper error code. */
=======
	/* If there's nobody else dumping the table, destroy it */
	if (atomic_dec_and_test(&orig->uref)) {
		pr_debug("Table destroy by resize %p\n", orig);
		mtype_ahash_destroy(set, orig, false);
	}

out:
#ifdef IP_SET_HASH_WITH_NETS
	kfree(tmp);
#endif
	return ret;

cleanup:
	atomic_set(&orig->ref, 0);
	atomic_dec(&orig->uref);
	spin_unlock_bh(&set->lock);
	mtype_ahash_destroy(set, t, false);
	if (ret == -EAGAIN)
		goto retry;
	goto out;
}

/* Add an element to a hash and update the internal counters when succeeded,
 * otherwise report the proper error code.
 */
>>>>>>> v4.9.227
static int
mtype_add(struct ip_set *set, void *value, const struct ip_set_ext *ext,
	  struct ip_set_ext *mext, u32 flags)
{
	struct htype *h = set->data;
	struct htable *t;
	const struct mtype_elem *d = value;
	struct mtype_elem *data;
<<<<<<< HEAD
	struct hbucket *n;
	int i, ret = 0;
	int j = AHASH_MAX(h) + 1;
	bool flag_exist = flags & IPSET_FLAG_EXIST;
	u32 key, multi = 0;

	if (h->elements >= h->maxelem && SET_WITH_FORCEADD(set)) {
		rcu_read_lock_bh();
		t = rcu_dereference_bh(h->table);
		key = HKEY(value, h->initval, t->htable_bits);
		n = hbucket(t,key);
		if (n->pos) {
			/* Choosing the first entry in the array to replace */
			j = 0;
			goto reuse_slot;
		}
		rcu_read_unlock_bh();
	}
	if (SET_WITH_TIMEOUT(set) && h->elements >= h->maxelem)
		/* FIXME: when set is full, we slow down here */
		mtype_expire(set, h, NLEN(set->family), set->dsize);

	if (h->elements >= h->maxelem) {
		if (net_ratelimit())
			pr_warn("Set %s is full, maxelem %u reached\n",
				set->name, h->maxelem);
		return -IPSET_ERR_HASH_FULL;
	}

	rcu_read_lock_bh();
	t = rcu_dereference_bh(h->table);
	key = HKEY(value, h->initval, t->htable_bits);
	n = hbucket(t, key);
	for (i = 0; i < n->pos; i++) {
=======
	struct hbucket *n, *old = ERR_PTR(-ENOENT);
	int i, j = -1;
	bool flag_exist = flags & IPSET_FLAG_EXIST;
	bool deleted = false, forceadd = false, reuse = false;
	u32 key, multi = 0;

	if (h->elements >= h->maxelem) {
		if (SET_WITH_TIMEOUT(set))
			/* FIXME: when set is full, we slow down here */
			mtype_expire(set, h, NLEN(set->family), set->dsize);
		if (h->elements >= h->maxelem && SET_WITH_FORCEADD(set))
			forceadd = true;
	}

	t = ipset_dereference_protected(h->table, set);
	key = HKEY(value, h->initval, t->htable_bits);
	n = __ipset_dereference_protected(hbucket(t, key), 1);
	if (!n) {
		if (forceadd) {
			if (net_ratelimit())
				pr_warn("Set %s is full, maxelem %u reached\n",
					set->name, h->maxelem);
			return -IPSET_ERR_HASH_FULL;
		} else if (h->elements >= h->maxelem) {
			goto set_full;
		}
		old = NULL;
		n = kzalloc(sizeof(*n) + AHASH_INIT_SIZE * set->dsize,
			    GFP_ATOMIC);
		if (!n)
			return -ENOMEM;
		n->size = AHASH_INIT_SIZE;
		goto copy_elem;
	}
	for (i = 0; i < n->pos; i++) {
		if (!test_bit(i, n->used)) {
			/* Reuse first deleted entry */
			if (j == -1) {
				deleted = reuse = true;
				j = i;
			}
			continue;
		}
>>>>>>> v4.9.227
		data = ahash_data(n, i, set->dsize);
		if (mtype_data_equal(data, d, &multi)) {
			if (flag_exist ||
			    (SET_WITH_TIMEOUT(set) &&
			     ip_set_timeout_expired(ext_timeout(data, set)))) {
				/* Just the extensions could be overwritten */
				j = i;
<<<<<<< HEAD
				goto reuse_slot;
			} else {
				ret = -IPSET_ERR_EXIST;
				goto out;
			}
=======
				goto overwrite_extensions;
			}
			return -IPSET_ERR_EXIST;
>>>>>>> v4.9.227
		}
		/* Reuse first timed out entry */
		if (SET_WITH_TIMEOUT(set) &&
		    ip_set_timeout_expired(ext_timeout(data, set)) &&
<<<<<<< HEAD
		    j != AHASH_MAX(h) + 1)
			j = i;
	}
reuse_slot:
	if (j != AHASH_MAX(h) + 1) {
		/* Fill out reused slot */
		data = ahash_data(n, j, set->dsize);
#ifdef IP_SET_HASH_WITH_NETS
		for (i = 0; i < IPSET_NET_COUNT; i++) {
			mtype_del_cidr(h, CIDR(data->cidr, i),
				       NLEN(set->family), i);
			mtype_add_cidr(h, CIDR(d->cidr, i),
				       NLEN(set->family), i);
		}
#endif
		ip_set_ext_destroy(set, data);
	} else {
		/* Use/create a new slot */
		TUNE_AHASH_MAX(h, multi);
		ret = hbucket_elem_add(n, AHASH_MAX(h), set->dsize);
		if (ret != 0) {
			if (ret == -EAGAIN)
				mtype_data_next(&h->next, d);
			goto out;
		}
		data = ahash_data(n, n->pos++, set->dsize);
#ifdef IP_SET_HASH_WITH_NETS
		for (i = 0; i < IPSET_NET_COUNT; i++)
			mtype_add_cidr(h, CIDR(d->cidr, i), NLEN(set->family),
				       i);
#endif
		h->elements++;
	}
	memcpy(data, d, sizeof(struct mtype_elem));
#ifdef IP_SET_HASH_WITH_NETS
	mtype_data_set_flags(data, flags);
#endif
	if (SET_WITH_TIMEOUT(set))
		ip_set_timeout_set(ext_timeout(data, set), ext->timeout);
=======
		    j == -1) {
			j = i;
			reuse = true;
		}
	}
	if (reuse || forceadd) {
		data = ahash_data(n, j, set->dsize);
		if (!deleted) {
#ifdef IP_SET_HASH_WITH_NETS
			for (i = 0; i < IPSET_NET_COUNT; i++)
				mtype_del_cidr(h,
					NCIDR_PUT(DCIDR_GET(data->cidr, i)),
					NLEN(set->family), i);
#endif
			ip_set_ext_destroy(set, data);
			h->elements--;
		}
		goto copy_data;
	}
	if (h->elements >= h->maxelem)
		goto set_full;
	/* Create a new slot */
	if (n->pos >= n->size) {
		TUNE_AHASH_MAX(h, multi);
		if (n->size >= AHASH_MAX(h)) {
			/* Trigger rehashing */
			mtype_data_next(&h->next, d);
			return -EAGAIN;
		}
		old = n;
		n = kzalloc(sizeof(*n) +
			    (old->size + AHASH_INIT_SIZE) * set->dsize,
			    GFP_ATOMIC);
		if (!n)
			return -ENOMEM;
		memcpy(n, old, sizeof(struct hbucket) +
		       old->size * set->dsize);
		n->size = old->size + AHASH_INIT_SIZE;
	}

copy_elem:
	j = n->pos++;
	data = ahash_data(n, j, set->dsize);
copy_data:
	h->elements++;
#ifdef IP_SET_HASH_WITH_NETS
	for (i = 0; i < IPSET_NET_COUNT; i++)
		mtype_add_cidr(h, NCIDR_PUT(DCIDR_GET(d->cidr, i)),
			       NLEN(set->family), i);
#endif
	memcpy(data, d, sizeof(struct mtype_elem));
overwrite_extensions:
#ifdef IP_SET_HASH_WITH_NETS
	mtype_data_set_flags(data, flags);
#endif
>>>>>>> v4.9.227
	if (SET_WITH_COUNTER(set))
		ip_set_init_counter(ext_counter(data, set), ext);
	if (SET_WITH_COMMENT(set))
		ip_set_init_comment(ext_comment(data, set), ext);
	if (SET_WITH_SKBINFO(set))
		ip_set_init_skbinfo(ext_skbinfo(data, set), ext);
<<<<<<< HEAD

out:
	rcu_read_unlock_bh();
	return ret;
}

/* Delete an element from the hash: swap it with the last element
 * and free up space if possible.
=======
	/* Must come last for the case when timed out entry is reused */
	if (SET_WITH_TIMEOUT(set))
		ip_set_timeout_set(ext_timeout(data, set), ext->timeout);
	smp_mb__before_atomic();
	set_bit(j, n->used);
	if (old != ERR_PTR(-ENOENT)) {
		rcu_assign_pointer(hbucket(t, key), n);
		if (old)
			kfree_rcu(old, rcu);
	}

	return 0;
set_full:
	if (net_ratelimit())
		pr_warn("Set %s is full, maxelem %u reached\n",
			set->name, h->maxelem);
	return -IPSET_ERR_HASH_FULL;
}

/* Delete an element from the hash and free up space if possible.
>>>>>>> v4.9.227
 */
static int
mtype_del(struct ip_set *set, void *value, const struct ip_set_ext *ext,
	  struct ip_set_ext *mext, u32 flags)
{
	struct htype *h = set->data;
	struct htable *t;
	const struct mtype_elem *d = value;
	struct mtype_elem *data;
	struct hbucket *n;
<<<<<<< HEAD
	int i, ret = -IPSET_ERR_EXIST;
#ifdef IP_SET_HASH_WITH_NETS
	u8 j;
#endif
	u32 key, multi = 0;

	rcu_read_lock_bh();
	t = rcu_dereference_bh(h->table);
	key = HKEY(value, h->initval, t->htable_bits);
	n = hbucket(t, key);
	for (i = 0; i < n->pos; i++) {
		data = ahash_data(n, i, set->dsize);
=======
	int i, j, k, ret = -IPSET_ERR_EXIST;
	u32 key, multi = 0;
	size_t dsize = set->dsize;

	t = ipset_dereference_protected(h->table, set);
	key = HKEY(value, h->initval, t->htable_bits);
	n = __ipset_dereference_protected(hbucket(t, key), 1);
	if (!n)
		goto out;
	for (i = 0, k = 0; i < n->pos; i++) {
		if (!test_bit(i, n->used)) {
			k++;
			continue;
		}
		data = ahash_data(n, i, dsize);
>>>>>>> v4.9.227
		if (!mtype_data_equal(data, d, &multi))
			continue;
		if (SET_WITH_TIMEOUT(set) &&
		    ip_set_timeout_expired(ext_timeout(data, set)))
			goto out;
<<<<<<< HEAD
		if (i != n->pos - 1)
			/* Not last one */
			memcpy(data, ahash_data(n, n->pos - 1, set->dsize),
			       set->dsize);

		n->pos--;
		h->elements--;
#ifdef IP_SET_HASH_WITH_NETS
		for (j = 0; j < IPSET_NET_COUNT; j++)
			mtype_del_cidr(h, CIDR(d->cidr, j), NLEN(set->family),
				       j);
#endif
		ip_set_ext_destroy(set, data);
		if (n->pos + AHASH_INIT_SIZE < n->size) {
			void *tmp = kzalloc((n->size - AHASH_INIT_SIZE)
					    * set->dsize,
					    GFP_ATOMIC);
			if (!tmp) {
				ret = 0;
				goto out;
			}
			n->size -= AHASH_INIT_SIZE;
			memcpy(tmp, n->value, n->size * set->dsize);
			kfree(n->value);
			n->value = tmp;
		}
		ret = 0;
=======

		ret = 0;
		clear_bit(i, n->used);
		smp_mb__after_atomic();
		if (i + 1 == n->pos)
			n->pos--;
		h->elements--;
#ifdef IP_SET_HASH_WITH_NETS
		for (j = 0; j < IPSET_NET_COUNT; j++)
			mtype_del_cidr(h, NCIDR_PUT(DCIDR_GET(d->cidr, j)),
				       NLEN(set->family), j);
#endif
		ip_set_ext_destroy(set, data);

		for (; i < n->pos; i++) {
			if (!test_bit(i, n->used))
				k++;
		}
		if (n->pos == 0 && k == 0) {
			rcu_assign_pointer(hbucket(t, key), NULL);
			kfree_rcu(n, rcu);
		} else if (k >= AHASH_INIT_SIZE) {
			struct hbucket *tmp = kzalloc(sizeof(*tmp) +
					(n->size - AHASH_INIT_SIZE) * dsize,
					GFP_ATOMIC);
			if (!tmp)
				goto out;
			tmp->size = n->size - AHASH_INIT_SIZE;
			for (j = 0, k = 0; j < n->pos; j++) {
				if (!test_bit(j, n->used))
					continue;
				data = ahash_data(n, j, dsize);
				memcpy(tmp->value + k * dsize, data, dsize);
				set_bit(j, tmp->used);
				k++;
			}
			tmp->pos = k;
			rcu_assign_pointer(hbucket(t, key), tmp);
			kfree_rcu(n, rcu);
		}
>>>>>>> v4.9.227
		goto out;
	}

out:
<<<<<<< HEAD
	rcu_read_unlock_bh();
=======
>>>>>>> v4.9.227
	return ret;
}

static inline int
mtype_data_match(struct mtype_elem *data, const struct ip_set_ext *ext,
		 struct ip_set_ext *mext, struct ip_set *set, u32 flags)
{
	if (SET_WITH_COUNTER(set))
		ip_set_update_counter(ext_counter(data, set),
				      ext, mext, flags);
	if (SET_WITH_SKBINFO(set))
		ip_set_get_skbinfo(ext_skbinfo(data, set),
				   ext, mext, flags);
	return mtype_do_data_match(data);
}

#ifdef IP_SET_HASH_WITH_NETS
/* Special test function which takes into account the different network
<<<<<<< HEAD
 * sizes added to the set */
=======
 * sizes added to the set
 */
>>>>>>> v4.9.227
static int
mtype_test_cidrs(struct ip_set *set, struct mtype_elem *d,
		 const struct ip_set_ext *ext,
		 struct ip_set_ext *mext, u32 flags)
{
	struct htype *h = set->data;
	struct htable *t = rcu_dereference_bh(h->table);
	struct hbucket *n;
	struct mtype_elem *data;
#if IPSET_NET_COUNT == 2
	struct mtype_elem orig = *d;
	int i, j = 0, k;
#else
	int i, j = 0;
#endif
	u32 key, multi = 0;
	u8 nets_length = NLEN(set->family);

	pr_debug("test by nets\n");
<<<<<<< HEAD
	for (; j < nets_length && h->nets[j].nets[0] && !multi; j++) {
#if IPSET_NET_COUNT == 2
		mtype_data_reset_elem(d, &orig);
		mtype_data_netmask(d, h->nets[j].cidr[0], false);
		for (k = 0; k < nets_length && h->nets[k].nets[1] && !multi;
		     k++) {
			mtype_data_netmask(d, h->nets[k].cidr[1], true);
#else
		mtype_data_netmask(d, h->nets[j].cidr[0]);
#endif
		key = HKEY(d, h->initval, t->htable_bits);
		n = hbucket(t, key);
		for (i = 0; i < n->pos; i++) {
=======
	for (; j < nets_length && h->nets[j].cidr[0] && !multi; j++) {
#if IPSET_NET_COUNT == 2
		mtype_data_reset_elem(d, &orig);
		mtype_data_netmask(d, NCIDR_GET(h->nets[j].cidr[0]), false);
		for (k = 0; k < nets_length && h->nets[k].cidr[1] && !multi;
		     k++) {
			mtype_data_netmask(d, NCIDR_GET(h->nets[k].cidr[1]),
					   true);
#else
		mtype_data_netmask(d, NCIDR_GET(h->nets[j].cidr[0]));
#endif
		key = HKEY(d, h->initval, t->htable_bits);
		n =  rcu_dereference_bh(hbucket(t, key));
		if (!n)
			continue;
		for (i = 0; i < n->pos; i++) {
			if (!test_bit(i, n->used))
				continue;
>>>>>>> v4.9.227
			data = ahash_data(n, i, set->dsize);
			if (!mtype_data_equal(data, d, &multi))
				continue;
			if (SET_WITH_TIMEOUT(set)) {
				if (!ip_set_timeout_expired(
						ext_timeout(data, set)))
					return mtype_data_match(data, ext,
								mext, set,
								flags);
#ifdef IP_SET_HASH_WITH_MULTI
				multi = 0;
#endif
			} else
				return mtype_data_match(data, ext,
							mext, set, flags);
		}
#if IPSET_NET_COUNT == 2
		}
#endif
	}
	return 0;
}
#endif

/* Test whether the element is added to the set */
static int
mtype_test(struct ip_set *set, void *value, const struct ip_set_ext *ext,
	   struct ip_set_ext *mext, u32 flags)
{
	struct htype *h = set->data;
	struct htable *t;
	struct mtype_elem *d = value;
	struct hbucket *n;
	struct mtype_elem *data;
	int i, ret = 0;
	u32 key, multi = 0;

<<<<<<< HEAD
	rcu_read_lock_bh();
	t = rcu_dereference_bh(h->table);
#ifdef IP_SET_HASH_WITH_NETS
	/* If we test an IP address and not a network address,
	 * try all possible network sizes */
	for (i = 0; i < IPSET_NET_COUNT; i++)
		if (CIDR(d->cidr, i) != SET_HOST_MASK(set->family))
=======
	t = rcu_dereference_bh(h->table);
#ifdef IP_SET_HASH_WITH_NETS
	/* If we test an IP address and not a network address,
	 * try all possible network sizes
	 */
	for (i = 0; i < IPSET_NET_COUNT; i++)
		if (DCIDR_GET(d->cidr, i) != SET_HOST_MASK(set->family))
>>>>>>> v4.9.227
			break;
	if (i == IPSET_NET_COUNT) {
		ret = mtype_test_cidrs(set, d, ext, mext, flags);
		goto out;
	}
#endif

	key = HKEY(d, h->initval, t->htable_bits);
<<<<<<< HEAD
	n = hbucket(t, key);
	for (i = 0; i < n->pos; i++) {
=======
	n = rcu_dereference_bh(hbucket(t, key));
	if (!n) {
		ret = 0;
		goto out;
	}
	for (i = 0; i < n->pos; i++) {
		if (!test_bit(i, n->used))
			continue;
>>>>>>> v4.9.227
		data = ahash_data(n, i, set->dsize);
		if (mtype_data_equal(data, d, &multi) &&
		    !(SET_WITH_TIMEOUT(set) &&
		      ip_set_timeout_expired(ext_timeout(data, set)))) {
			ret = mtype_data_match(data, ext, mext, set, flags);
			goto out;
		}
	}
out:
<<<<<<< HEAD
	rcu_read_unlock_bh();
=======
>>>>>>> v4.9.227
	return ret;
}

/* Reply a HEADER request: fill out the header part of the set */
static int
mtype_head(struct ip_set *set, struct sk_buff *skb)
{
	const struct htype *h = set->data;
	const struct htable *t;
	struct nlattr *nested;
	size_t memsize;
<<<<<<< HEAD

	t = rcu_dereference_bh_nfnl(h->table);
	memsize = mtype_ahash_memsize(h, t, NLEN(set->family), set->dsize);
=======
	u8 htable_bits;

	rcu_read_lock_bh();
	t = rcu_dereference_bh_nfnl(h->table);
	memsize = mtype_ahash_memsize(h, t, NLEN(set->family), set->dsize);
	htable_bits = t->htable_bits;
	rcu_read_unlock_bh();
>>>>>>> v4.9.227

	nested = ipset_nest_start(skb, IPSET_ATTR_DATA);
	if (!nested)
		goto nla_put_failure;
	if (nla_put_net32(skb, IPSET_ATTR_HASHSIZE,
<<<<<<< HEAD
			  htonl(jhash_size(t->htable_bits))) ||
=======
			  htonl(jhash_size(htable_bits))) ||
>>>>>>> v4.9.227
	    nla_put_net32(skb, IPSET_ATTR_MAXELEM, htonl(h->maxelem)))
		goto nla_put_failure;
#ifdef IP_SET_HASH_WITH_NETMASK
	if (h->netmask != HOST_MASK &&
	    nla_put_u8(skb, IPSET_ATTR_NETMASK, h->netmask))
		goto nla_put_failure;
#endif
#ifdef IP_SET_HASH_WITH_MARKMASK
	if (nla_put_u32(skb, IPSET_ATTR_MARKMASK, h->markmask))
		goto nla_put_failure;
#endif
<<<<<<< HEAD
	if (nla_put_net32(skb, IPSET_ATTR_REFERENCES, htonl(set->ref - 1)) ||
=======
	if (nla_put_net32(skb, IPSET_ATTR_REFERENCES, htonl(set->ref)) ||
>>>>>>> v4.9.227
	    nla_put_net32(skb, IPSET_ATTR_MEMSIZE, htonl(memsize)))
		goto nla_put_failure;
	if (unlikely(ip_set_put_flags(skb, set)))
		goto nla_put_failure;
	ipset_nest_end(skb, nested);

	return 0;
nla_put_failure:
	return -EMSGSIZE;
}

<<<<<<< HEAD
=======
/* Make possible to run dumping parallel with resizing */
static void
mtype_uref(struct ip_set *set, struct netlink_callback *cb, bool start)
{
	struct htype *h = set->data;
	struct htable *t;

	if (start) {
		rcu_read_lock_bh();
		t = rcu_dereference_bh_nfnl(h->table);
		atomic_inc(&t->uref);
		cb->args[IPSET_CB_PRIVATE] = (unsigned long)t;
		rcu_read_unlock_bh();
	} else if (cb->args[IPSET_CB_PRIVATE]) {
		t = (struct htable *)cb->args[IPSET_CB_PRIVATE];
		if (atomic_dec_and_test(&t->uref) && atomic_read(&t->ref)) {
			/* Resizing didn't destroy the hash table */
			pr_debug("Table destroy by dump: %p\n", t);
			mtype_ahash_destroy(set, t, false);
		}
		cb->args[IPSET_CB_PRIVATE] = 0;
	}
}

>>>>>>> v4.9.227
/* Reply a LIST/SAVE request: dump the elements of the specified set */
static int
mtype_list(const struct ip_set *set,
	   struct sk_buff *skb, struct netlink_callback *cb)
{
<<<<<<< HEAD
	const struct htype *h = set->data;
	const struct htable *t = rcu_dereference_bh_nfnl(h->table);
=======
	const struct htable *t;
>>>>>>> v4.9.227
	struct nlattr *atd, *nested;
	const struct hbucket *n;
	const struct mtype_elem *e;
	u32 first = cb->args[IPSET_CB_ARG0];
	/* We assume that one hash bucket fills into one page */
	void *incomplete;
<<<<<<< HEAD
	int i;
=======
	int i, ret = 0;
>>>>>>> v4.9.227

	atd = ipset_nest_start(skb, IPSET_ATTR_ADT);
	if (!atd)
		return -EMSGSIZE;
<<<<<<< HEAD
	pr_debug("list hash set %s\n", set->name);
	for (; cb->args[IPSET_CB_ARG0] < jhash_size(t->htable_bits);
	     cb->args[IPSET_CB_ARG0]++) {
		incomplete = skb_tail_pointer(skb);
		n = hbucket(t, cb->args[IPSET_CB_ARG0]);
		pr_debug("cb->arg bucket: %lu, t %p n %p\n",
			 cb->args[IPSET_CB_ARG0], t, n);
		for (i = 0; i < n->pos; i++) {
=======

	pr_debug("list hash set %s\n", set->name);
	t = (const struct htable *)cb->args[IPSET_CB_PRIVATE];
	/* Expire may replace a hbucket with another one */
	rcu_read_lock();
	for (; cb->args[IPSET_CB_ARG0] < jhash_size(t->htable_bits);
	     cb->args[IPSET_CB_ARG0]++) {
		incomplete = skb_tail_pointer(skb);
		n = rcu_dereference(hbucket(t, cb->args[IPSET_CB_ARG0]));
		pr_debug("cb->arg bucket: %lu, t %p n %p\n",
			 cb->args[IPSET_CB_ARG0], t, n);
		if (!n)
			continue;
		for (i = 0; i < n->pos; i++) {
			if (!test_bit(i, n->used))
				continue;
>>>>>>> v4.9.227
			e = ahash_data(n, i, set->dsize);
			if (SET_WITH_TIMEOUT(set) &&
			    ip_set_timeout_expired(ext_timeout(e, set)))
				continue;
			pr_debug("list hash %lu hbucket %p i %u, data %p\n",
				 cb->args[IPSET_CB_ARG0], n, i, e);
			nested = ipset_nest_start(skb, IPSET_ATTR_DATA);
			if (!nested) {
				if (cb->args[IPSET_CB_ARG0] == first) {
					nla_nest_cancel(skb, atd);
<<<<<<< HEAD
					return -EMSGSIZE;
				} else
					goto nla_put_failure;
=======
					ret = -EMSGSIZE;
					goto out;
				}
				goto nla_put_failure;
>>>>>>> v4.9.227
			}
			if (mtype_data_list(skb, e))
				goto nla_put_failure;
			if (ip_set_put_extensions(skb, set, e, true))
				goto nla_put_failure;
			ipset_nest_end(skb, nested);
		}
	}
	ipset_nest_end(skb, atd);
	/* Set listing finished */
	cb->args[IPSET_CB_ARG0] = 0;

<<<<<<< HEAD
	return 0;
=======
	goto out;
>>>>>>> v4.9.227

nla_put_failure:
	nlmsg_trim(skb, incomplete);
	if (unlikely(first == cb->args[IPSET_CB_ARG0])) {
		pr_warn("Can't list set %s: one bucket does not fit into a message. Please report it!\n",
			set->name);
		cb->args[IPSET_CB_ARG0] = 0;
<<<<<<< HEAD
		return -EMSGSIZE;
	}
	ipset_nest_end(skb, atd);
	return 0;
=======
		ret = -EMSGSIZE;
	} else {
		ipset_nest_end(skb, atd);
	}
out:
	rcu_read_unlock();
	return ret;
>>>>>>> v4.9.227
}

static int
IPSET_TOKEN(MTYPE, _kadt)(struct ip_set *set, const struct sk_buff *skb,
<<<<<<< HEAD
	    const struct xt_action_param *par,
	    enum ipset_adt adt, struct ip_set_adt_opt *opt);

static int
IPSET_TOKEN(MTYPE, _uadt)(struct ip_set *set, struct nlattr *tb[],
	    enum ipset_adt adt, u32 *lineno, u32 flags, bool retried);
=======
			  const struct xt_action_param *par,
			  enum ipset_adt adt, struct ip_set_adt_opt *opt);

static int
IPSET_TOKEN(MTYPE, _uadt)(struct ip_set *set, struct nlattr *tb[],
			  enum ipset_adt adt, u32 *lineno, u32 flags,
			  bool retried);
>>>>>>> v4.9.227

static const struct ip_set_type_variant mtype_variant = {
	.kadt	= mtype_kadt,
	.uadt	= mtype_uadt,
	.adt	= {
		[IPSET_ADD] = mtype_add,
		[IPSET_DEL] = mtype_del,
		[IPSET_TEST] = mtype_test,
	},
	.destroy = mtype_destroy,
	.flush	= mtype_flush,
	.head	= mtype_head,
	.list	= mtype_list,
<<<<<<< HEAD
=======
	.uref	= mtype_uref,
>>>>>>> v4.9.227
	.resize	= mtype_resize,
	.same_set = mtype_same_set,
};

#ifdef IP_SET_EMIT_CREATE
static int
IPSET_TOKEN(HTYPE, _create)(struct net *net, struct ip_set *set,
			    struct nlattr *tb[], u32 flags)
{
	u32 hashsize = IPSET_DEFAULT_HASHSIZE, maxelem = IPSET_DEFAULT_MAXELEM;
#ifdef IP_SET_HASH_WITH_MARKMASK
	u32 markmask;
#endif
	u8 hbits;
#ifdef IP_SET_HASH_WITH_NETMASK
	u8 netmask;
#endif
	size_t hsize;
<<<<<<< HEAD
	struct HTYPE *h;
=======
	struct htype *h;
>>>>>>> v4.9.227
	struct htable *t;

#ifndef IP_SET_PROTO_UNDEF
	if (!(set->family == NFPROTO_IPV4 || set->family == NFPROTO_IPV6))
		return -IPSET_ERR_INVALID_FAMILY;
#endif

#ifdef IP_SET_HASH_WITH_MARKMASK
	markmask = 0xffffffff;
#endif
#ifdef IP_SET_HASH_WITH_NETMASK
	netmask = set->family == NFPROTO_IPV4 ? 32 : 128;
	pr_debug("Create set %s with family %s\n",
		 set->name, set->family == NFPROTO_IPV4 ? "inet" : "inet6");
#endif

	if (unlikely(!ip_set_optattr_netorder(tb, IPSET_ATTR_HASHSIZE) ||
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_MAXELEM) ||
<<<<<<< HEAD
#ifdef IP_SET_HASH_WITH_MARKMASK
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_MARKMASK) ||
#endif
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_TIMEOUT) ||
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_CADT_FLAGS)))
		return -IPSET_ERR_PROTOCOL;
=======
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_TIMEOUT) ||
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_CADT_FLAGS)))
		return -IPSET_ERR_PROTOCOL;
#ifdef IP_SET_HASH_WITH_MARKMASK
	/* Separated condition in order to avoid directive in argument list */
	if (unlikely(!ip_set_optattr_netorder(tb, IPSET_ATTR_MARKMASK)))
		return -IPSET_ERR_PROTOCOL;
#endif
>>>>>>> v4.9.227

	if (tb[IPSET_ATTR_HASHSIZE]) {
		hashsize = ip_set_get_h32(tb[IPSET_ATTR_HASHSIZE]);
		if (hashsize < IPSET_MIMINAL_HASHSIZE)
			hashsize = IPSET_MIMINAL_HASHSIZE;
	}

	if (tb[IPSET_ATTR_MAXELEM])
		maxelem = ip_set_get_h32(tb[IPSET_ATTR_MAXELEM]);

#ifdef IP_SET_HASH_WITH_NETMASK
	if (tb[IPSET_ATTR_NETMASK]) {
		netmask = nla_get_u8(tb[IPSET_ATTR_NETMASK]);

		if ((set->family == NFPROTO_IPV4 && netmask > 32) ||
		    (set->family == NFPROTO_IPV6 && netmask > 128) ||
		    netmask == 0)
			return -IPSET_ERR_INVALID_NETMASK;
	}
#endif
#ifdef IP_SET_HASH_WITH_MARKMASK
	if (tb[IPSET_ATTR_MARKMASK]) {
<<<<<<< HEAD
		markmask = ntohl(nla_get_u32(tb[IPSET_ATTR_MARKMASK]));
=======
		markmask = ntohl(nla_get_be32(tb[IPSET_ATTR_MARKMASK]));
>>>>>>> v4.9.227

		if (markmask == 0)
			return -IPSET_ERR_INVALID_MARKMASK;
	}
#endif

	hsize = sizeof(*h);
#ifdef IP_SET_HASH_WITH_NETS
<<<<<<< HEAD
	hsize += sizeof(struct net_prefixes) *
		(set->family == NFPROTO_IPV4 ? 32 : 128);
=======
	hsize += sizeof(struct net_prefixes) * NLEN(set->family);
>>>>>>> v4.9.227
#endif
	h = kzalloc(hsize, GFP_KERNEL);
	if (!h)
		return -ENOMEM;

	h->maxelem = maxelem;
#ifdef IP_SET_HASH_WITH_NETMASK
	h->netmask = netmask;
#endif
#ifdef IP_SET_HASH_WITH_MARKMASK
	h->markmask = markmask;
#endif
	get_random_bytes(&h->initval, sizeof(h->initval));
	set->timeout = IPSET_NO_TIMEOUT;

	hbits = htable_bits(hashsize);
	hsize = htable_size(hbits);
	if (hsize == 0) {
		kfree(h);
		return -ENOMEM;
	}
	t = ip_set_alloc(hsize);
	if (!t) {
		kfree(h);
		return -ENOMEM;
	}
	t->htable_bits = hbits;
	rcu_assign_pointer(h->table, t);

	set->data = h;
#ifndef IP_SET_PROTO_UNDEF
	if (set->family == NFPROTO_IPV4) {
#endif
		set->variant = &IPSET_TOKEN(HTYPE, 4_variant);
		set->dsize = ip_set_elem_len(set, tb,
<<<<<<< HEAD
				sizeof(struct IPSET_TOKEN(HTYPE, 4_elem)));
=======
			sizeof(struct IPSET_TOKEN(HTYPE, 4_elem)),
			__alignof__(struct IPSET_TOKEN(HTYPE, 4_elem)));
>>>>>>> v4.9.227
#ifndef IP_SET_PROTO_UNDEF
	} else {
		set->variant = &IPSET_TOKEN(HTYPE, 6_variant);
		set->dsize = ip_set_elem_len(set, tb,
<<<<<<< HEAD
				sizeof(struct IPSET_TOKEN(HTYPE, 6_elem)));
=======
			sizeof(struct IPSET_TOKEN(HTYPE, 6_elem)),
			__alignof__(struct IPSET_TOKEN(HTYPE, 6_elem)));
>>>>>>> v4.9.227
	}
#endif
	if (tb[IPSET_ATTR_TIMEOUT]) {
		set->timeout = ip_set_timeout_uget(tb[IPSET_ATTR_TIMEOUT]);
#ifndef IP_SET_PROTO_UNDEF
		if (set->family == NFPROTO_IPV4)
#endif
			IPSET_TOKEN(HTYPE, 4_gc_init)(set,
				IPSET_TOKEN(HTYPE, 4_gc));
#ifndef IP_SET_PROTO_UNDEF
		else
			IPSET_TOKEN(HTYPE, 6_gc_init)(set,
				IPSET_TOKEN(HTYPE, 6_gc));
#endif
	}
	pr_debug("create %s hashsize %u (%u) maxelem %u: %p(%p)\n",
		 set->name, jhash_size(t->htable_bits),
		 t->htable_bits, h->maxelem, set->data, t);

	return 0;
}
#endif /* IP_SET_EMIT_CREATE */
<<<<<<< HEAD
=======

#undef HKEY_DATALEN
>>>>>>> v4.9.227
