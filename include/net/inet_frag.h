#ifndef __NET_FRAG_H__
#define __NET_FRAG_H__

<<<<<<< HEAD
#include <linux/percpu_counter.h>

struct netns_frags {
	/* The percpu_counter "mem" need to be cacheline aligned.
	 *  mem.count must not share cacheline with other writers
	 */
	struct percpu_counter   mem ____cacheline_aligned_in_smp;

	/* sysctls */
	int			timeout;
	int			high_thresh;
	int			low_thresh;
=======
#include <linux/rhashtable.h>

struct netns_frags {
	/* sysctls */
	long			high_thresh;
	long			low_thresh;
	int			timeout;
	int			max_dist;
	struct inet_frags	*f;

	struct rhashtable       rhashtable ____cacheline_aligned_in_smp;

	/* Keep atomic mem on separate cachelines in structs that include it */
	atomic_long_t		mem ____cacheline_aligned_in_smp;
>>>>>>> v4.9.227
};

/**
 * fragment queue flags
 *
 * @INET_FRAG_FIRST_IN: first fragment has arrived
 * @INET_FRAG_LAST_IN: final fragment has arrived
 * @INET_FRAG_COMPLETE: frag queue has been processed and is due for destruction
<<<<<<< HEAD
 * @INET_FRAG_EVICTED: frag queue is being evicted
=======
>>>>>>> v4.9.227
 */
enum {
	INET_FRAG_FIRST_IN	= BIT(0),
	INET_FRAG_LAST_IN	= BIT(1),
	INET_FRAG_COMPLETE	= BIT(2),
<<<<<<< HEAD
	INET_FRAG_EVICTED	= BIT(3)
=======
};

struct frag_v4_compare_key {
	__be32		saddr;
	__be32		daddr;
	u32		user;
	u32		vif;
	__be16		id;
	u16		protocol;
};

struct frag_v6_compare_key {
	struct in6_addr	saddr;
	struct in6_addr	daddr;
	u32		user;
	__be32		id;
	u32		iif;
>>>>>>> v4.9.227
};

/**
 * struct inet_frag_queue - fragment queue
 *
<<<<<<< HEAD
 * @lock: spinlock protecting the queue
 * @timer: queue expiration timer
 * @list: hash bucket list
 * @refcnt: reference count of the queue
 * @fragments: received fragments head
 * @fragments_tail: received fragments tail
=======
 * @node: rhash node
 * @key: keys identifying this frag.
 * @timer: queue expiration timer
 * @lock: spinlock protecting this frag
 * @refcnt: reference count of the queue
 * @fragments: received fragments head
 * @rb_fragments: received fragments rb-tree root
 * @fragments_tail: received fragments tail
 * @last_run_head: the head of the last "run". see ip_fragment.c
>>>>>>> v4.9.227
 * @stamp: timestamp of the last received fragment
 * @len: total length of the original datagram
 * @meat: length of received fragments so far
 * @flags: fragment queue flags
<<<<<<< HEAD
 * @max_size: (ipv4 only) maximum received fragment size with IP_DF set
 * @net: namespace that this frag belongs to
 */
struct inet_frag_queue {
	spinlock_t		lock;
	struct timer_list	timer;
	struct hlist_node	list;
	atomic_t		refcnt;
	struct sk_buff		*fragments;
	struct sk_buff		*fragments_tail;
=======
 * @max_size: maximum received fragment size
 * @net: namespace that this frag belongs to
 * @rcu: rcu head for freeing deferall
 */
struct inet_frag_queue {
	struct rhash_head	node;
	union {
		struct frag_v4_compare_key v4;
		struct frag_v6_compare_key v6;
	} key;
	struct timer_list	timer;
	spinlock_t		lock;
	atomic_t		refcnt;
	struct sk_buff		*fragments;  /* used in 6lopwpan IPv6. */
	struct rb_root		rb_fragments; /* Used in IPv4/IPv6. */
	struct sk_buff		*fragments_tail;
	struct sk_buff		*last_run_head;
>>>>>>> v4.9.227
	ktime_t			stamp;
	int			len;
	int			meat;
	__u8			flags;
	u16			max_size;
<<<<<<< HEAD
	struct netns_frags	*net;
};

#define INETFRAGS_HASHSZ	1024

/* averaged:
 * max_depth = default ipfrag_high_thresh / INETFRAGS_HASHSZ /
 *	       rounded up (SKB_TRUELEN(0) + sizeof(struct ipq or
 *	       struct frag_queue))
 */
#define INETFRAGS_MAXDEPTH	128

struct inet_frag_bucket {
	struct hlist_head	chain;
	spinlock_t		chain_lock;
};

struct inet_frags {
	struct inet_frag_bucket	hash[INETFRAGS_HASHSZ];

	struct work_struct	frags_work;
	unsigned int next_bucket;
	unsigned long last_rebuild_jiffies;
	bool rebuild;

	/* The first call to hashfn is responsible to initialize
	 * rnd. This is best done with net_get_random_once.
	 *
	 * rnd_seqlock is used to let hash insertion detect
	 * when it needs to re-lookup the hash chain to use.
	 */
	u32			rnd;
	seqlock_t		rnd_seqlock;
	int			qsize;

	unsigned int		(*hashfn)(const struct inet_frag_queue *);
	bool			(*match)(const struct inet_frag_queue *q,
					 const void *arg);
	void			(*constructor)(struct inet_frag_queue *q,
					       const void *arg);
	void			(*destructor)(struct inet_frag_queue *);
	void			(*skb_free)(struct sk_buff *);
	void			(*frag_expire)(unsigned long data);
	struct kmem_cache	*frags_cachep;
	const char		*frags_cache_name;
=======
	struct netns_frags      *net;
	struct rcu_head		rcu;
};

struct inet_frags {
	int			qsize;

	void			(*constructor)(struct inet_frag_queue *q,
					       const void *arg);
	void			(*destructor)(struct inet_frag_queue *);
	void			(*frag_expire)(unsigned long data);
	struct kmem_cache	*frags_cachep;
	const char		*frags_cache_name;
	struct rhashtable_params rhash_params;
>>>>>>> v4.9.227
};

int inet_frags_init(struct inet_frags *);
void inet_frags_fini(struct inet_frags *);

<<<<<<< HEAD
void inet_frags_init_net(struct netns_frags *nf);
void inet_frags_exit_net(struct netns_frags *nf, struct inet_frags *f);

void inet_frag_kill(struct inet_frag_queue *q, struct inet_frags *f);
void inet_frag_destroy(struct inet_frag_queue *q, struct inet_frags *f);
struct inet_frag_queue *inet_frag_find(struct netns_frags *nf,
		struct inet_frags *f, void *key, unsigned int hash);

void inet_frag_maybe_warn_overflow(struct inet_frag_queue *q,
				   const char *prefix);

static inline void inet_frag_put(struct inet_frag_queue *q, struct inet_frags *f)
{
	if (atomic_dec_and_test(&q->refcnt))
		inet_frag_destroy(q, f);
=======
static inline int inet_frags_init_net(struct netns_frags *nf)
{
	atomic_long_set(&nf->mem, 0);
	return rhashtable_init(&nf->rhashtable, &nf->f->rhash_params);
}
void inet_frags_exit_net(struct netns_frags *nf);

void inet_frag_kill(struct inet_frag_queue *q);
void inet_frag_destroy(struct inet_frag_queue *q);
struct inet_frag_queue *inet_frag_find(struct netns_frags *nf, void *key);

/* Free all skbs in the queue; return the sum of their truesizes. */
unsigned int inet_frag_rbtree_purge(struct rb_root *root);

static inline void inet_frag_put(struct inet_frag_queue *q)
{
	if (atomic_dec_and_test(&q->refcnt))
		inet_frag_destroy(q);
>>>>>>> v4.9.227
}

/* Memory Tracking Functions. */

<<<<<<< HEAD
/* The default percpu_counter batch size is not big enough to scale to
 * fragmentation mem acct sizes.
 * The mem size of a 64K fragment is approx:
 *  (44 fragments * 2944 truesize) + frag_queue struct(200) = 129736 bytes
 */
static unsigned int frag_percpu_counter_batch = 130000;

static inline int frag_mem_limit(struct netns_frags *nf)
{
	return percpu_counter_read(&nf->mem);
}

static inline void sub_frag_mem_limit(struct inet_frag_queue *q, int i)
{
	__percpu_counter_add(&q->net->mem, -i, frag_percpu_counter_batch);
}

static inline void add_frag_mem_limit(struct inet_frag_queue *q, int i)
{
	__percpu_counter_add(&q->net->mem, i, frag_percpu_counter_batch);
}

static inline void init_frag_mem_limit(struct netns_frags *nf)
{
	percpu_counter_init(&nf->mem, 0, GFP_KERNEL);
}

static inline unsigned int sum_frag_mem_limit(struct netns_frags *nf)
{
	unsigned int res;

	local_bh_disable();
	res = percpu_counter_sum_positive(&nf->mem);
	local_bh_enable();

	return res;
=======
static inline long frag_mem_limit(const struct netns_frags *nf)
{
	return atomic_long_read(&nf->mem);
}

static inline void sub_frag_mem_limit(struct netns_frags *nf, long val)
{
	atomic_long_sub(val, &nf->mem);
}

static inline void add_frag_mem_limit(struct netns_frags *nf, long val)
{
	atomic_long_add(val, &nf->mem);
>>>>>>> v4.9.227
}

/* RFC 3168 support :
 * We want to check ECN values of all fragments, do detect invalid combinations.
 * In ipq->ecn, we store the OR value of each ip4_frag_ecn() fragment value.
 */
#define	IPFRAG_ECN_NOT_ECT	0x01 /* one frag had ECN_NOT_ECT */
#define	IPFRAG_ECN_ECT_1	0x02 /* one frag had ECN_ECT_1 */
#define	IPFRAG_ECN_ECT_0	0x04 /* one frag had ECN_ECT_0 */
#define	IPFRAG_ECN_CE		0x08 /* one frag had ECN_CE */

extern const u8 ip_frag_ecn_table[16];

<<<<<<< HEAD
=======
/* Return values of inet_frag_queue_insert() */
#define IPFRAG_OK	0
#define IPFRAG_DUP	1
#define IPFRAG_OVERLAP	2
int inet_frag_queue_insert(struct inet_frag_queue *q, struct sk_buff *skb,
			   int offset, int end);
void *inet_frag_reasm_prepare(struct inet_frag_queue *q, struct sk_buff *skb,
			      struct sk_buff *parent);
void inet_frag_reasm_finish(struct inet_frag_queue *q, struct sk_buff *head,
			    void *reasm_data);
struct sk_buff *inet_frag_pull_head(struct inet_frag_queue *q);

>>>>>>> v4.9.227
#endif
