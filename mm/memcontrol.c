/* memcontrol.c - Memory Controller
 *
 * Copyright IBM Corporation, 2007
 * Author Balbir Singh <balbir@linux.vnet.ibm.com>
 *
 * Copyright 2007 OpenVZ SWsoft Inc
 * Author: Pavel Emelianov <xemul@openvz.org>
 *
 * Memory thresholds
 * Copyright (C) 2009 Nokia Corporation
 * Author: Kirill A. Shutemov
 *
 * Kernel Memory Controller
 * Copyright (C) 2012 Parallels Inc. and Google Inc.
 * Authors: Glauber Costa and Suleiman Souhlal
 *
<<<<<<< HEAD
=======
 * Native page reclaim
 * Charge lifetime sanitation
 * Lockless page tracking & accounting
 * Unified hierarchy configuration model
 * Copyright (C) 2015 Red Hat, Inc., Johannes Weiner
 *
>>>>>>> v4.9.227
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

<<<<<<< HEAD
#include <linux/res_counter.h>
=======
#include <linux/page_counter.h>
>>>>>>> v4.9.227
#include <linux/memcontrol.h>
#include <linux/cgroup.h>
#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/pagemap.h>
#include <linux/smp.h>
#include <linux/page-flags.h>
#include <linux/backing-dev.h>
#include <linux/bit_spinlock.h>
#include <linux/rcupdate.h>
#include <linux/limits.h>
#include <linux/export.h>
#include <linux/mutex.h>
#include <linux/rbtree.h>
#include <linux/slab.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/spinlock.h>
#include <linux/eventfd.h>
#include <linux/poll.h>
#include <linux/sort.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/vmpressure.h>
#include <linux/mm_inline.h>
<<<<<<< HEAD
#include <linux/page_cgroup.h>
=======
#include <linux/swap_cgroup.h>
>>>>>>> v4.9.227
#include <linux/cpu.h>
#include <linux/oom.h>
#include <linux/lockdep.h>
#include <linux/file.h>
<<<<<<< HEAD
#include "internal.h"
#include <net/sock.h>
#include <net/ip.h>
#include <net/tcp_memcontrol.h>
=======
#include <linux/tracehook.h>
#include "internal.h"
#include <net/sock.h>
#include <net/ip.h>
>>>>>>> v4.9.227
#include "slab.h"

#include <asm/uaccess.h>

#include <trace/events/vmscan.h>

struct cgroup_subsys memory_cgrp_subsys __read_mostly;
EXPORT_SYMBOL(memory_cgrp_subsys);

<<<<<<< HEAD
#define MEM_CGROUP_RECLAIM_RETRIES	5
static struct mem_cgroup *root_mem_cgroup __read_mostly;

#ifdef CONFIG_MEMCG_SWAP
/* Turned on only when memory cgroup is enabled && really_do_swap_account = 1 */
int do_swap_account __read_mostly;

/* for remember boot option*/
#ifdef CONFIG_MEMCG_SWAP_ENABLED
static int really_do_swap_account __initdata = 1;
#else
static int really_do_swap_account __initdata;
#endif

=======
struct mem_cgroup *root_mem_cgroup __read_mostly;

#define MEM_CGROUP_RECLAIM_RETRIES	5

/* Socket memory accounting disabled? */
static bool cgroup_memory_nosocket;

/* Kernel memory accounting disabled? */
static bool cgroup_memory_nokmem;

/* Whether the swap controller is active */
#ifdef CONFIG_MEMCG_SWAP
int do_swap_account __read_mostly;
>>>>>>> v4.9.227
#else
#define do_swap_account		0
#endif

<<<<<<< HEAD
=======
/* Whether legacy memory+swap accounting is active */
static bool do_memsw_account(void)
{
	return !cgroup_subsys_on_dfl(memory_cgrp_subsys) && do_swap_account;
}
>>>>>>> v4.9.227

static const char * const mem_cgroup_stat_names[] = {
	"cache",
	"rss",
	"rss_huge",
	"mapped_file",
<<<<<<< HEAD
=======
	"dirty",
>>>>>>> v4.9.227
	"writeback",
	"swap",
};

<<<<<<< HEAD
enum mem_cgroup_events_index {
	MEM_CGROUP_EVENTS_PGPGIN,	/* # of pages paged in */
	MEM_CGROUP_EVENTS_PGPGOUT,	/* # of pages paged out */
	MEM_CGROUP_EVENTS_PGFAULT,	/* # of page-faults */
	MEM_CGROUP_EVENTS_PGMAJFAULT,	/* # of major page-faults */
	MEM_CGROUP_EVENTS_NSTATS,
};

=======
>>>>>>> v4.9.227
static const char * const mem_cgroup_events_names[] = {
	"pgpgin",
	"pgpgout",
	"pgfault",
	"pgmajfault",
};

static const char * const mem_cgroup_lru_names[] = {
	"inactive_anon",
	"active_anon",
	"inactive_file",
	"active_file",
	"unevictable",
};

<<<<<<< HEAD
/*
 * Per memcg event counter is incremented at every pagein/pageout. With THP,
 * it will be incremated by the number of pages. This counter is used for
 * for trigger some periodic events. This is straightforward and better
 * than using jiffies etc. to handle periodic memcg event.
 */
enum mem_cgroup_events_target {
	MEM_CGROUP_TARGET_THRESH,
	MEM_CGROUP_TARGET_SOFTLIMIT,
	MEM_CGROUP_TARGET_NUMAINFO,
	MEM_CGROUP_NTARGETS,
};
=======
>>>>>>> v4.9.227
#define THRESHOLDS_EVENTS_TARGET 128
#define SOFTLIMIT_EVENTS_TARGET 1024
#define NUMAINFO_EVENTS_TARGET	1024

<<<<<<< HEAD
struct mem_cgroup_stat_cpu {
	long count[MEM_CGROUP_STAT_NSTATS];
	unsigned long events[MEM_CGROUP_EVENTS_NSTATS];
	unsigned long nr_page_events;
	unsigned long targets[MEM_CGROUP_NTARGETS];
};

struct mem_cgroup_reclaim_iter {
	/*
	 * last scanned hierarchy member. Valid only if last_dead_count
	 * matches memcg->dead_count of the hierarchy root group.
	 */
	struct mem_cgroup *last_visited;
	int last_dead_count;

	/* scan generation, increased every round-trip */
	unsigned int generation;
};

/*
 * per-zone information in memory controller.
 */
struct mem_cgroup_per_zone {
	struct lruvec		lruvec;
	unsigned long		lru_size[NR_LRU_LISTS];

	struct mem_cgroup_reclaim_iter reclaim_iter[DEF_PRIORITY + 1];

	struct rb_node		tree_node;	/* RB tree node */
	unsigned long long	usage_in_excess;/* Set to the value by which */
						/* the soft limit is exceeded*/
	bool			on_tree;
	struct mem_cgroup	*memcg;		/* Back pointer, we cannot */
						/* use container_of	   */
};

struct mem_cgroup_per_node {
	struct mem_cgroup_per_zone zoneinfo[MAX_NR_ZONES];
};

=======
>>>>>>> v4.9.227
/*
 * Cgroups above their limits are maintained in a RB-Tree, independent of
 * their hierarchy representation
 */

<<<<<<< HEAD
struct mem_cgroup_tree_per_zone {
=======
struct mem_cgroup_tree_per_node {
>>>>>>> v4.9.227
	struct rb_root rb_root;
	spinlock_t lock;
};

<<<<<<< HEAD
struct mem_cgroup_tree_per_node {
	struct mem_cgroup_tree_per_zone rb_tree_per_zone[MAX_NR_ZONES];
};

=======
>>>>>>> v4.9.227
struct mem_cgroup_tree {
	struct mem_cgroup_tree_per_node *rb_tree_per_node[MAX_NUMNODES];
};

static struct mem_cgroup_tree soft_limit_tree __read_mostly;

<<<<<<< HEAD
struct mem_cgroup_threshold {
	struct eventfd_ctx *eventfd;
	u64 threshold;
};

/* For threshold */
struct mem_cgroup_threshold_ary {
	/* An array index points to threshold just below or equal to usage. */
	int current_threshold;
	/* Size of entries[] */
	unsigned int size;
	/* Array of thresholds */
	struct mem_cgroup_threshold entries[0];
};

struct mem_cgroup_thresholds {
	/* Primary thresholds array */
	struct mem_cgroup_threshold_ary *primary;
	/*
	 * Spare threshold array.
	 * This is needed to make mem_cgroup_unregister_event() "never fail".
	 * It must be able to store at least primary->size - 1 entries.
	 */
	struct mem_cgroup_threshold_ary *spare;
};

=======
>>>>>>> v4.9.227
/* for OOM */
struct mem_cgroup_eventfd_list {
	struct list_head list;
	struct eventfd_ctx *eventfd;
};

/*
 * cgroup_event represents events which userspace want to receive.
 */
struct mem_cgroup_event {
	/*
	 * memcg which the event belongs to.
	 */
	struct mem_cgroup *memcg;
	/*
	 * eventfd to signal userspace about the event.
	 */
	struct eventfd_ctx *eventfd;
	/*
	 * Each of these stored in a list by the cgroup.
	 */
	struct list_head list;
	/*
	 * register_event() callback will be used to add new userspace
	 * waiter for changes related to this event.  Use eventfd_signal()
	 * on eventfd to send notification to userspace.
	 */
	int (*register_event)(struct mem_cgroup *memcg,
			      struct eventfd_ctx *eventfd, const char *args);
	/*
	 * unregister_event() callback will be called when userspace closes
	 * the eventfd or on cgroup removing.  This callback must be set,
	 * if you want provide notification functionality.
	 */
	void (*unregister_event)(struct mem_cgroup *memcg,
				 struct eventfd_ctx *eventfd);
	/*
	 * All fields below needed to unregister event when
	 * userspace closes eventfd.
	 */
	poll_table pt;
	wait_queue_head_t *wqh;
	wait_queue_t wait;
	struct work_struct remove;
};

static void mem_cgroup_threshold(struct mem_cgroup *memcg);
static void mem_cgroup_oom_notify(struct mem_cgroup *memcg);

<<<<<<< HEAD
/*
 * The memory controller data structure. The memory controller controls both
 * page cache and RSS per cgroup. We would eventually like to provide
 * statistics based on the statistics developed by Rik Van Riel for clock-pro,
 * to help the administrator determine what knobs to tune.
 *
 * TODO: Add a water mark for the memory controller. Reclaim will begin when
 * we hit the water mark. May be even add a low water mark, such that
 * no reclaim occurs from a cgroup at it's low water mark, this is
 * a feature that will be implemented much later in the future.
 */
struct mem_cgroup {
	struct cgroup_subsys_state css;
	/*
	 * the counter to account for memory usage
	 */
	struct res_counter res;

	/* vmpressure notifications */
	struct vmpressure vmpressure;

	/* css_online() has been completed */
	int initialized;

	/*
	 * the counter to account for mem+swap usage.
	 */
	struct res_counter memsw;

	/*
	 * the counter to account for kernel memory usage.
	 */
	struct res_counter kmem;
	/*
	 * Should the accounting and control be hierarchical, per subtree?
	 */
	bool use_hierarchy;
	unsigned long kmem_account_flags; /* See KMEM_ACCOUNTED_*, below */

	bool		oom_lock;
	atomic_t	under_oom;
	atomic_t	oom_wakeups;

	int	swappiness;
	/* OOM-Killer disable */
	int		oom_kill_disable;

	/* protect arrays of thresholds */
	struct mutex thresholds_lock;

	/* thresholds for memory usage. RCU-protected */
	struct mem_cgroup_thresholds thresholds;

	/* thresholds for mem+swap usage. RCU-protected */
	struct mem_cgroup_thresholds memsw_thresholds;

	/* For oom notifier event fd */
	struct list_head oom_notify;

	/*
	 * Should we move charges of a task when a task is moved into this
	 * mem_cgroup ? And what type of charges should we move ?
	 */
	unsigned long move_charge_at_immigrate;
	/*
	 * set > 0 if pages under this cgroup are moving to other cgroup.
	 */
	atomic_t	moving_account;
	/* taken only while moving_account > 0 */
	spinlock_t	move_lock;
	/*
	 * percpu counter.
	 */
	struct mem_cgroup_stat_cpu __percpu *stat;
	spinlock_t pcp_counter_lock;

	atomic_t	dead_count;
#if defined(CONFIG_MEMCG_KMEM) && defined(CONFIG_INET)
	struct cg_proto tcp_mem;
#endif
#if defined(CONFIG_MEMCG_KMEM)
	/* analogous to slab_common's slab_caches list, but per-memcg;
	 * protected by memcg_slab_mutex */
	struct list_head memcg_slab_caches;
        /* Index in the kmem_cache->memcg_params->memcg_caches array */
	int kmemcg_id;
#endif

	int last_scanned_node;
#if MAX_NUMNODES > 1
	nodemask_t	scan_nodes;
	atomic_t	numainfo_events;
	atomic_t	numainfo_updating;
#endif

	/* List of events which userspace want to receive */
	struct list_head event_list;
	spinlock_t event_list_lock;

	struct mem_cgroup_per_node *nodeinfo[0];
	/* WARNING: nodeinfo must be the last member here */
};

/* internal only representation about the status of kmem accounting. */
enum {
	KMEM_ACCOUNTED_ACTIVE, /* accounted by this cgroup itself */
	KMEM_ACCOUNTED_DEAD, /* dead memcg with pending kmem charges */
};

#ifdef CONFIG_MEMCG_KMEM
static inline void memcg_kmem_set_active(struct mem_cgroup *memcg)
{
	set_bit(KMEM_ACCOUNTED_ACTIVE, &memcg->kmem_account_flags);
}

static bool memcg_kmem_is_active(struct mem_cgroup *memcg)
{
	return test_bit(KMEM_ACCOUNTED_ACTIVE, &memcg->kmem_account_flags);
}

static void memcg_kmem_mark_dead(struct mem_cgroup *memcg)
{
	/*
	 * Our caller must use css_get() first, because memcg_uncharge_kmem()
	 * will call css_put() if it sees the memcg is dead.
	 */
	smp_wmb();
	if (test_bit(KMEM_ACCOUNTED_ACTIVE, &memcg->kmem_account_flags))
		set_bit(KMEM_ACCOUNTED_DEAD, &memcg->kmem_account_flags);
}

static bool memcg_kmem_test_and_clear_dead(struct mem_cgroup *memcg)
{
	return test_and_clear_bit(KMEM_ACCOUNTED_DEAD,
				  &memcg->kmem_account_flags);
}
#endif

/* Stuffs for move charges at task migration. */
/*
 * Types of charges to be moved. "move_charge_at_immitgrate" and
 * "immigrate_flags" are treated as a left-shifted bitmap of these types.
 */
enum move_type {
	MOVE_CHARGE_TYPE_ANON,	/* private anonymous page and swap of it */
	MOVE_CHARGE_TYPE_FILE,	/* file page(including tmpfs) and swap of it */
	NR_MOVE_TYPE,
};
=======
/* Stuffs for move charges at task migration. */
/*
 * Types of charges to be moved.
 */
#define MOVE_ANON	0x1U
#define MOVE_FILE	0x2U
#define MOVE_MASK	(MOVE_ANON | MOVE_FILE)
>>>>>>> v4.9.227

/* "mc" and its members are protected by cgroup_mutex */
static struct move_charge_struct {
	spinlock_t	  lock; /* for from, to */
<<<<<<< HEAD
	struct mem_cgroup *from;
	struct mem_cgroup *to;
	unsigned long immigrate_flags;
=======
	struct mm_struct  *mm;
	struct mem_cgroup *from;
	struct mem_cgroup *to;
	unsigned long flags;
>>>>>>> v4.9.227
	unsigned long precharge;
	unsigned long moved_charge;
	unsigned long moved_swap;
	struct task_struct *moving_task;	/* a task moving charges */
	wait_queue_head_t waitq;		/* a waitq for other context */
} mc = {
	.lock = __SPIN_LOCK_UNLOCKED(mc.lock),
	.waitq = __WAIT_QUEUE_HEAD_INITIALIZER(mc.waitq),
};

<<<<<<< HEAD
static bool move_anon(void)
{
	return test_bit(MOVE_CHARGE_TYPE_ANON, &mc.immigrate_flags);
}

static bool move_file(void)
{
	return test_bit(MOVE_CHARGE_TYPE_FILE, &mc.immigrate_flags);
}

=======
>>>>>>> v4.9.227
/*
 * Maximum loops in mem_cgroup_hierarchical_reclaim(), used for soft
 * limit reclaim to prevent infinite loops, if they ever occur.
 */
#define	MEM_CGROUP_MAX_RECLAIM_LOOPS		100
#define	MEM_CGROUP_MAX_SOFT_LIMIT_RECLAIM_LOOPS	2

enum charge_type {
	MEM_CGROUP_CHARGE_TYPE_CACHE = 0,
	MEM_CGROUP_CHARGE_TYPE_ANON,
	MEM_CGROUP_CHARGE_TYPE_SWAPOUT,	/* for accounting swapcache */
	MEM_CGROUP_CHARGE_TYPE_DROP,	/* a page was unused swap cache */
	NR_CHARGE_TYPE,
};

/* for encoding cft->private value on file */
enum res_type {
	_MEM,
	_MEMSWAP,
	_OOM_TYPE,
	_KMEM,
<<<<<<< HEAD
=======
	_TCP,
>>>>>>> v4.9.227
};

#define MEMFILE_PRIVATE(x, val)	((x) << 16 | (val))
#define MEMFILE_TYPE(val)	((val) >> 16 & 0xffff)
#define MEMFILE_ATTR(val)	((val) & 0xffff)
/* Used for OOM nofiier */
#define OOM_CONTROL		(0)

<<<<<<< HEAD
/*
 * The memcg_create_mutex will be held whenever a new cgroup is created.
 * As a consequence, any change that needs to protect against new child cgroups
 * appearing has to hold it as well.
 */
static DEFINE_MUTEX(memcg_create_mutex);

struct mem_cgroup *mem_cgroup_from_css(struct cgroup_subsys_state *s)
{
	return s ? container_of(s, struct mem_cgroup, css) : NULL;
}

=======
>>>>>>> v4.9.227
/* Some nice accessors for the vmpressure. */
struct vmpressure *memcg_to_vmpressure(struct mem_cgroup *memcg)
{
	if (!memcg)
		memcg = root_mem_cgroup;
	return &memcg->vmpressure;
}

struct cgroup_subsys_state *vmpressure_to_css(struct vmpressure *vmpr)
{
	return &container_of(vmpr, struct mem_cgroup, vmpressure)->css;
}

static inline bool mem_cgroup_is_root(struct mem_cgroup *memcg)
{
	return (memcg == root_mem_cgroup);
}

<<<<<<< HEAD
/*
 * We restrict the id in the range of [1, 65535], so it can fit into
 * an unsigned short.
 */
#define MEM_CGROUP_ID_MAX	USHRT_MAX

static inline unsigned short mem_cgroup_id(struct mem_cgroup *memcg)
{
	return memcg->css.id;
}

static inline struct mem_cgroup *mem_cgroup_from_id(unsigned short id)
{
	struct cgroup_subsys_state *css;

	css = css_from_id(id, &memory_cgrp_subsys);
	return mem_cgroup_from_css(css);
}

/* Writing them here to avoid exposing memcg's inner layout */
#if defined(CONFIG_INET) && defined(CONFIG_MEMCG_KMEM)

void sock_update_memcg(struct sock *sk)
{
	if (mem_cgroup_sockets_enabled) {
		struct mem_cgroup *memcg;
		struct cg_proto *cg_proto;

		BUG_ON(!sk->sk_prot->proto_cgroup);

		/* Socket cloning can throw us here with sk_cgrp already
		 * filled. It won't however, necessarily happen from
		 * process context. So the test for root memcg given
		 * the current task's memcg won't help us in this case.
		 *
		 * Respecting the original socket's memcg is a better
		 * decision in this case.
		 */
		if (sk->sk_cgrp) {
			BUG_ON(mem_cgroup_is_root(sk->sk_cgrp->memcg));
			css_get(&sk->sk_cgrp->memcg->css);
			return;
		}

		rcu_read_lock();
		memcg = mem_cgroup_from_task(current);
		cg_proto = sk->sk_prot->proto_cgroup(memcg);
		if (!mem_cgroup_is_root(memcg) &&
		    memcg_proto_active(cg_proto) &&
		    css_tryget_online(&memcg->css)) {
			sk->sk_cgrp = cg_proto;
		}
		rcu_read_unlock();
	}
}
EXPORT_SYMBOL(sock_update_memcg);

void sock_release_memcg(struct sock *sk)
{
	if (mem_cgroup_sockets_enabled && sk->sk_cgrp) {
		struct mem_cgroup *memcg;
		WARN_ON(!sk->sk_cgrp->memcg);
		memcg = sk->sk_cgrp->memcg;
		css_put(&sk->sk_cgrp->memcg->css);
	}
}

struct cg_proto *tcp_proto_cgroup(struct mem_cgroup *memcg)
{
	if (!memcg || mem_cgroup_is_root(memcg))
		return NULL;

	return &memcg->tcp_mem;
}
EXPORT_SYMBOL(tcp_proto_cgroup);

static void disarm_sock_keys(struct mem_cgroup *memcg)
{
	if (!memcg_proto_activated(&memcg->tcp_mem))
		return;
	static_key_slow_dec(&memcg_socket_limit_enabled);
}
#else
static void disarm_sock_keys(struct mem_cgroup *memcg)
{
}
#endif

#ifdef CONFIG_MEMCG_KMEM
/*
 * This will be the memcg's index in each cache's ->memcg_params->memcg_caches.
=======
#ifndef CONFIG_SLOB
/*
 * This will be the memcg's index in each cache's ->memcg_params.memcg_caches.
>>>>>>> v4.9.227
 * The main reason for not using cgroup id for this:
 *  this works better in sparse environments, where we have a lot of memcgs,
 *  but only a few kmem-limited. Or also, if we have, for instance, 200
 *  memcgs, and none but the 200th is kmem-limited, we'd have to have a
 *  200 entry array for that.
 *
<<<<<<< HEAD
 * The current size of the caches array is stored in
 * memcg_limited_groups_array_size.  It will double each time we have to
 * increase it.
 */
static DEFINE_IDA(kmem_limited_groups);
int memcg_limited_groups_array_size;
=======
 * The current size of the caches array is stored in memcg_nr_cache_ids. It
 * will double each time we have to increase it.
 */
static DEFINE_IDA(memcg_cache_ida);
int memcg_nr_cache_ids;

/* Protects memcg_nr_cache_ids */
static DECLARE_RWSEM(memcg_cache_ids_sem);

void memcg_get_cache_ids(void)
{
	down_read(&memcg_cache_ids_sem);
}

void memcg_put_cache_ids(void)
{
	up_read(&memcg_cache_ids_sem);
}
>>>>>>> v4.9.227

/*
 * MIN_SIZE is different than 1, because we would like to avoid going through
 * the alloc/free process all the time. In a small machine, 4 kmem-limited
 * cgroups is a reasonable guess. In the future, it could be a parameter or
 * tunable, but that is strictly not necessary.
 *
 * MAX_SIZE should be as large as the number of cgrp_ids. Ideally, we could get
 * this constant directly from cgroup, but it is understandable that this is
 * better kept as an internal representation in cgroup.c. In any case, the
 * cgrp_id space is not getting any smaller, and we don't have to necessarily
 * increase ours as well if it increases.
 */
#define MEMCG_CACHES_MIN_SIZE 4
#define MEMCG_CACHES_MAX_SIZE MEM_CGROUP_ID_MAX

/*
 * A lot of the calls to the cache allocation functions are expected to be
 * inlined by the compiler. Since the calls to memcg_kmem_get_cache are
 * conditional to this static branch, we'll have to allow modules that does
 * kmem_cache_alloc and the such to see this symbol as well
 */
<<<<<<< HEAD
struct static_key memcg_kmem_enabled_key;
EXPORT_SYMBOL(memcg_kmem_enabled_key);

static void memcg_free_cache_id(int id);

static void disarm_kmem_keys(struct mem_cgroup *memcg)
{
	if (memcg_kmem_is_active(memcg)) {
		static_key_slow_dec(&memcg_kmem_enabled_key);
		memcg_free_cache_id(memcg->kmemcg_id);
	}
	/*
	 * This check can't live in kmem destruction function,
	 * since the charges will outlive the cgroup
	 */
	WARN_ON(res_counter_read_u64(&memcg->kmem, RES_USAGE) != 0);
}
#else
static void disarm_kmem_keys(struct mem_cgroup *memcg)
{
}
#endif /* CONFIG_MEMCG_KMEM */

static void disarm_static_keys(struct mem_cgroup *memcg)
{
	disarm_sock_keys(memcg);
	disarm_kmem_keys(memcg);
}

static void drain_all_stock_async(struct mem_cgroup *memcg);

static struct mem_cgroup_per_zone *
mem_cgroup_zone_zoneinfo(struct mem_cgroup *memcg, struct zone *zone)
{
	int nid = zone_to_nid(zone);
	int zid = zone_idx(zone);

	return &memcg->nodeinfo[nid]->zoneinfo[zid];
}

struct cgroup_subsys_state *mem_cgroup_css(struct mem_cgroup *memcg)
{
	return &memcg->css;
}

static struct mem_cgroup_per_zone *
mem_cgroup_page_zoneinfo(struct mem_cgroup *memcg, struct page *page)
{
	int nid = page_to_nid(page);
	int zid = page_zonenum(page);

	return &memcg->nodeinfo[nid]->zoneinfo[zid];
}

static struct mem_cgroup_tree_per_zone *
soft_limit_tree_node_zone(int nid, int zid)
{
	return &soft_limit_tree.rb_tree_per_node[nid]->rb_tree_per_zone[zid];
}

static struct mem_cgroup_tree_per_zone *
soft_limit_tree_from_page(struct page *page)
{
	int nid = page_to_nid(page);
	int zid = page_zonenum(page);

	return &soft_limit_tree.rb_tree_per_node[nid]->rb_tree_per_zone[zid];
}

static void __mem_cgroup_insert_exceeded(struct mem_cgroup_per_zone *mz,
					 struct mem_cgroup_tree_per_zone *mctz,
					 unsigned long long new_usage_in_excess)
{
	struct rb_node **p = &mctz->rb_root.rb_node;
	struct rb_node *parent = NULL;
	struct mem_cgroup_per_zone *mz_node;
=======
DEFINE_STATIC_KEY_FALSE(memcg_kmem_enabled_key);
EXPORT_SYMBOL(memcg_kmem_enabled_key);

#endif /* !CONFIG_SLOB */

/**
 * mem_cgroup_css_from_page - css of the memcg associated with a page
 * @page: page of interest
 *
 * If memcg is bound to the default hierarchy, css of the memcg associated
 * with @page is returned.  The returned css remains associated with @page
 * until it is released.
 *
 * If memcg is bound to a traditional hierarchy, the css of root_mem_cgroup
 * is returned.
 */
struct cgroup_subsys_state *mem_cgroup_css_from_page(struct page *page)
{
	struct mem_cgroup *memcg;

	memcg = page->mem_cgroup;

	if (!memcg || !cgroup_subsys_on_dfl(memory_cgrp_subsys))
		memcg = root_mem_cgroup;

	return &memcg->css;
}

/**
 * page_cgroup_ino - return inode number of the memcg a page is charged to
 * @page: the page
 *
 * Look up the closest online ancestor of the memory cgroup @page is charged to
 * and return its inode number or 0 if @page is not charged to any cgroup. It
 * is safe to call this function without holding a reference to @page.
 *
 * Note, this function is inherently racy, because there is nothing to prevent
 * the cgroup inode from getting torn down and potentially reallocated a moment
 * after page_cgroup_ino() returns, so it only should be used by callers that
 * do not care (such as procfs interfaces).
 */
ino_t page_cgroup_ino(struct page *page)
{
	struct mem_cgroup *memcg;
	unsigned long ino = 0;

	rcu_read_lock();
	memcg = READ_ONCE(page->mem_cgroup);
	while (memcg && !(memcg->css.flags & CSS_ONLINE))
		memcg = parent_mem_cgroup(memcg);
	if (memcg)
		ino = cgroup_ino(memcg->css.cgroup);
	rcu_read_unlock();
	return ino;
}

static struct mem_cgroup_per_node *
mem_cgroup_page_nodeinfo(struct mem_cgroup *memcg, struct page *page)
{
	int nid = page_to_nid(page);

	return memcg->nodeinfo[nid];
}

static struct mem_cgroup_tree_per_node *
soft_limit_tree_node(int nid)
{
	return soft_limit_tree.rb_tree_per_node[nid];
}

static struct mem_cgroup_tree_per_node *
soft_limit_tree_from_page(struct page *page)
{
	int nid = page_to_nid(page);

	return soft_limit_tree.rb_tree_per_node[nid];
}

static void __mem_cgroup_insert_exceeded(struct mem_cgroup_per_node *mz,
					 struct mem_cgroup_tree_per_node *mctz,
					 unsigned long new_usage_in_excess)
{
	struct rb_node **p = &mctz->rb_root.rb_node;
	struct rb_node *parent = NULL;
	struct mem_cgroup_per_node *mz_node;
>>>>>>> v4.9.227

	if (mz->on_tree)
		return;

	mz->usage_in_excess = new_usage_in_excess;
	if (!mz->usage_in_excess)
		return;
	while (*p) {
		parent = *p;
<<<<<<< HEAD
		mz_node = rb_entry(parent, struct mem_cgroup_per_zone,
=======
		mz_node = rb_entry(parent, struct mem_cgroup_per_node,
>>>>>>> v4.9.227
					tree_node);
		if (mz->usage_in_excess < mz_node->usage_in_excess)
			p = &(*p)->rb_left;
		/*
		 * We can't avoid mem cgroups that are over their soft
		 * limit by the same amount
		 */
		else if (mz->usage_in_excess >= mz_node->usage_in_excess)
			p = &(*p)->rb_right;
	}
	rb_link_node(&mz->tree_node, parent, p);
	rb_insert_color(&mz->tree_node, &mctz->rb_root);
	mz->on_tree = true;
}

<<<<<<< HEAD
static void __mem_cgroup_remove_exceeded(struct mem_cgroup_per_zone *mz,
					 struct mem_cgroup_tree_per_zone *mctz)
=======
static void __mem_cgroup_remove_exceeded(struct mem_cgroup_per_node *mz,
					 struct mem_cgroup_tree_per_node *mctz)
>>>>>>> v4.9.227
{
	if (!mz->on_tree)
		return;
	rb_erase(&mz->tree_node, &mctz->rb_root);
	mz->on_tree = false;
}

<<<<<<< HEAD
static void mem_cgroup_remove_exceeded(struct mem_cgroup_per_zone *mz,
				       struct mem_cgroup_tree_per_zone *mctz)
=======
static void mem_cgroup_remove_exceeded(struct mem_cgroup_per_node *mz,
				       struct mem_cgroup_tree_per_node *mctz)
>>>>>>> v4.9.227
{
	unsigned long flags;

	spin_lock_irqsave(&mctz->lock, flags);
	__mem_cgroup_remove_exceeded(mz, mctz);
	spin_unlock_irqrestore(&mctz->lock, flags);
}

<<<<<<< HEAD

static void mem_cgroup_update_tree(struct mem_cgroup *memcg, struct page *page)
{
	unsigned long long excess;
	struct mem_cgroup_per_zone *mz;
	struct mem_cgroup_tree_per_zone *mctz;

	mctz = soft_limit_tree_from_page(page);
=======
static unsigned long soft_limit_excess(struct mem_cgroup *memcg)
{
	unsigned long nr_pages = page_counter_read(&memcg->memory);
	unsigned long soft_limit = READ_ONCE(memcg->soft_limit);
	unsigned long excess = 0;

	if (nr_pages > soft_limit)
		excess = nr_pages - soft_limit;

	return excess;
}

static void mem_cgroup_update_tree(struct mem_cgroup *memcg, struct page *page)
{
	unsigned long excess;
	struct mem_cgroup_per_node *mz;
	struct mem_cgroup_tree_per_node *mctz;

	mctz = soft_limit_tree_from_page(page);
	if (!mctz)
		return;
>>>>>>> v4.9.227
	/*
	 * Necessary to update all ancestors when hierarchy is used.
	 * because their event counter is not touched.
	 */
	for (; memcg; memcg = parent_mem_cgroup(memcg)) {
<<<<<<< HEAD
		mz = mem_cgroup_page_zoneinfo(memcg, page);
		excess = res_counter_soft_limit_excess(&memcg->res);
=======
		mz = mem_cgroup_page_nodeinfo(memcg, page);
		excess = soft_limit_excess(memcg);
>>>>>>> v4.9.227
		/*
		 * We have to update the tree if mz is on RB-tree or
		 * mem is over its softlimit.
		 */
		if (excess || mz->on_tree) {
			unsigned long flags;

			spin_lock_irqsave(&mctz->lock, flags);
			/* if on-tree, remove it */
			if (mz->on_tree)
				__mem_cgroup_remove_exceeded(mz, mctz);
			/*
			 * Insert again. mz->usage_in_excess will be updated.
			 * If excess is 0, no tree ops.
			 */
			__mem_cgroup_insert_exceeded(mz, mctz, excess);
			spin_unlock_irqrestore(&mctz->lock, flags);
		}
	}
}

static void mem_cgroup_remove_from_trees(struct mem_cgroup *memcg)
{
<<<<<<< HEAD
	struct mem_cgroup_tree_per_zone *mctz;
	struct mem_cgroup_per_zone *mz;
	int nid, zid;

	for_each_node(nid) {
		for (zid = 0; zid < MAX_NR_ZONES; zid++) {
			mz = &memcg->nodeinfo[nid]->zoneinfo[zid];
			mctz = soft_limit_tree_node_zone(nid, zid);
			mem_cgroup_remove_exceeded(mz, mctz);
		}
	}
}

static struct mem_cgroup_per_zone *
__mem_cgroup_largest_soft_limit_node(struct mem_cgroup_tree_per_zone *mctz)
{
	struct rb_node *rightmost = NULL;
	struct mem_cgroup_per_zone *mz;
=======
	struct mem_cgroup_tree_per_node *mctz;
	struct mem_cgroup_per_node *mz;
	int nid;

	for_each_node(nid) {
		mz = mem_cgroup_nodeinfo(memcg, nid);
		mctz = soft_limit_tree_node(nid);
		if (mctz)
			mem_cgroup_remove_exceeded(mz, mctz);
	}
}

static struct mem_cgroup_per_node *
__mem_cgroup_largest_soft_limit_node(struct mem_cgroup_tree_per_node *mctz)
{
	struct rb_node *rightmost = NULL;
	struct mem_cgroup_per_node *mz;
>>>>>>> v4.9.227

retry:
	mz = NULL;
	rightmost = rb_last(&mctz->rb_root);
	if (!rightmost)
		goto done;		/* Nothing to reclaim from */

<<<<<<< HEAD
	mz = rb_entry(rightmost, struct mem_cgroup_per_zone, tree_node);
=======
	mz = rb_entry(rightmost, struct mem_cgroup_per_node, tree_node);
>>>>>>> v4.9.227
	/*
	 * Remove the node now but someone else can add it back,
	 * we will to add it back at the end of reclaim to its correct
	 * position in the tree.
	 */
	__mem_cgroup_remove_exceeded(mz, mctz);
<<<<<<< HEAD
	if (!res_counter_soft_limit_excess(&mz->memcg->res) ||
=======
	if (!soft_limit_excess(mz->memcg) ||
>>>>>>> v4.9.227
	    !css_tryget_online(&mz->memcg->css))
		goto retry;
done:
	return mz;
}

<<<<<<< HEAD
static struct mem_cgroup_per_zone *
mem_cgroup_largest_soft_limit_node(struct mem_cgroup_tree_per_zone *mctz)
{
	struct mem_cgroup_per_zone *mz;
=======
static struct mem_cgroup_per_node *
mem_cgroup_largest_soft_limit_node(struct mem_cgroup_tree_per_node *mctz)
{
	struct mem_cgroup_per_node *mz;
>>>>>>> v4.9.227

	spin_lock_irq(&mctz->lock);
	mz = __mem_cgroup_largest_soft_limit_node(mctz);
	spin_unlock_irq(&mctz->lock);
	return mz;
}

/*
<<<<<<< HEAD
=======
 * Return page count for single (non recursive) @memcg.
 *
>>>>>>> v4.9.227
 * Implementation Note: reading percpu statistics for memcg.
 *
 * Both of vmstat[] and percpu_counter has threshold and do periodic
 * synchronization to implement "quick" read. There are trade-off between
 * reading cost and precision of value. Then, we may have a chance to implement
<<<<<<< HEAD
 * a periodic synchronizion of counter in memcg's counter.
=======
 * a periodic synchronization of counter in memcg's counter.
>>>>>>> v4.9.227
 *
 * But this _read() function is used for user interface now. The user accounts
 * memory usage by memory cgroup and he _always_ requires exact value because
 * he accounts memory. Even if we provide quick-and-fuzzy read, we always
 * have to visit all online cpus and make sum. So, for now, unnecessary
 * synchronization is not implemented. (just implemented for cpu hotplug)
 *
 * If there are kernel internal actions which can make use of some not-exact
 * value, and reading all cpu value can be performance bottleneck in some
<<<<<<< HEAD
 * common workload, threashold and synchonization as vmstat[] should be
 * implemented.
 */
static long mem_cgroup_read_stat(struct mem_cgroup *memcg,
				 enum mem_cgroup_stat_index idx)
=======
 * common workload, threshold and synchronization as vmstat[] should be
 * implemented.
 */
static unsigned long
mem_cgroup_read_stat(struct mem_cgroup *memcg, enum mem_cgroup_stat_index idx)
>>>>>>> v4.9.227
{
	long val = 0;
	int cpu;

<<<<<<< HEAD
	for_each_possible_cpu(cpu)
		val += per_cpu(memcg->stat->count[idx], cpu);
=======
	/* Per-cpu values can be negative, use a signed accumulator */
	for_each_possible_cpu(cpu)
		val += per_cpu(memcg->stat->count[idx], cpu);
	/*
	 * Summing races with updates, so val may be negative.  Avoid exposing
	 * transient negative values.
	 */
	if (val < 0)
		val = 0;
>>>>>>> v4.9.227
	return val;
}

static unsigned long mem_cgroup_read_events(struct mem_cgroup *memcg,
					    enum mem_cgroup_events_index idx)
{
	unsigned long val = 0;
	int cpu;

	for_each_possible_cpu(cpu)
		val += per_cpu(memcg->stat->events[idx], cpu);
	return val;
}

static void mem_cgroup_charge_statistics(struct mem_cgroup *memcg,
					 struct page *page,
<<<<<<< HEAD
					 int nr_pages)
=======
					 bool compound, int nr_pages)
>>>>>>> v4.9.227
{
	/*
	 * Here, RSS means 'mapped anon' and anon's SwapCache. Shmem/tmpfs is
	 * counted as CACHE even if it's on ANON LRU.
	 */
	if (PageAnon(page))
		__this_cpu_add(memcg->stat->count[MEM_CGROUP_STAT_RSS],
				nr_pages);
	else
		__this_cpu_add(memcg->stat->count[MEM_CGROUP_STAT_CACHE],
				nr_pages);

<<<<<<< HEAD
	if (PageTransHuge(page))
		__this_cpu_add(memcg->stat->count[MEM_CGROUP_STAT_RSS_HUGE],
				nr_pages);
=======
	if (compound) {
		VM_BUG_ON_PAGE(!PageTransHuge(page), page);
		__this_cpu_add(memcg->stat->count[MEM_CGROUP_STAT_RSS_HUGE],
				nr_pages);
	}
>>>>>>> v4.9.227

	/* pagein of a big page is an event. So, ignore page size */
	if (nr_pages > 0)
		__this_cpu_inc(memcg->stat->events[MEM_CGROUP_EVENTS_PGPGIN]);
	else {
		__this_cpu_inc(memcg->stat->events[MEM_CGROUP_EVENTS_PGPGOUT]);
		nr_pages = -nr_pages; /* for event */
	}

	__this_cpu_add(memcg->stat->nr_page_events, nr_pages);
}

<<<<<<< HEAD
unsigned long mem_cgroup_get_lru_size(struct lruvec *lruvec, enum lru_list lru)
{
	struct mem_cgroup_per_zone *mz;

	mz = container_of(lruvec, struct mem_cgroup_per_zone, lruvec);
	return mz->lru_size[lru];
}

static unsigned long mem_cgroup_node_nr_lru_pages(struct mem_cgroup *memcg,
						  int nid,
						  unsigned int lru_mask)
{
	unsigned long nr = 0;
	int zid;

	VM_BUG_ON((unsigned)nid >= nr_node_ids);

	for (zid = 0; zid < MAX_NR_ZONES; zid++) {
		struct mem_cgroup_per_zone *mz;
		enum lru_list lru;

		for_each_lru(lru) {
			if (!(BIT(lru) & lru_mask))
				continue;
			mz = &memcg->nodeinfo[nid]->zoneinfo[zid];
			nr += mz->lru_size[lru];
		}
=======
unsigned long mem_cgroup_node_nr_lru_pages(struct mem_cgroup *memcg,
					   int nid, unsigned int lru_mask)
{
	struct lruvec *lruvec = mem_cgroup_lruvec(NODE_DATA(nid), memcg);
	unsigned long nr = 0;
	enum lru_list lru;

	VM_BUG_ON((unsigned)nid >= nr_node_ids);

	for_each_lru(lru) {
		if (!(BIT(lru) & lru_mask))
			continue;
		nr += mem_cgroup_get_lru_size(lruvec, lru);
>>>>>>> v4.9.227
	}
	return nr;
}

static unsigned long mem_cgroup_nr_lru_pages(struct mem_cgroup *memcg,
			unsigned int lru_mask)
{
	unsigned long nr = 0;
	int nid;

	for_each_node_state(nid, N_MEMORY)
		nr += mem_cgroup_node_nr_lru_pages(memcg, nid, lru_mask);
	return nr;
}

static bool mem_cgroup_event_ratelimit(struct mem_cgroup *memcg,
				       enum mem_cgroup_events_target target)
{
	unsigned long val, next;

	val = __this_cpu_read(memcg->stat->nr_page_events);
	next = __this_cpu_read(memcg->stat->targets[target]);
	/* from time_after() in jiffies.h */
	if ((long)next - (long)val < 0) {
		switch (target) {
		case MEM_CGROUP_TARGET_THRESH:
			next = val + THRESHOLDS_EVENTS_TARGET;
			break;
		case MEM_CGROUP_TARGET_SOFTLIMIT:
			next = val + SOFTLIMIT_EVENTS_TARGET;
			break;
		case MEM_CGROUP_TARGET_NUMAINFO:
			next = val + NUMAINFO_EVENTS_TARGET;
			break;
		default:
			break;
		}
		__this_cpu_write(memcg->stat->targets[target], next);
		return true;
	}
	return false;
}

/*
 * Check events in order.
 *
 */
static void memcg_check_events(struct mem_cgroup *memcg, struct page *page)
{
	/* threshold event is triggered in finer grain than soft limit */
	if (unlikely(mem_cgroup_event_ratelimit(memcg,
						MEM_CGROUP_TARGET_THRESH))) {
		bool do_softlimit;
		bool do_numainfo __maybe_unused;

		do_softlimit = mem_cgroup_event_ratelimit(memcg,
						MEM_CGROUP_TARGET_SOFTLIMIT);
#if MAX_NUMNODES > 1
		do_numainfo = mem_cgroup_event_ratelimit(memcg,
						MEM_CGROUP_TARGET_NUMAINFO);
#endif
		mem_cgroup_threshold(memcg);
		if (unlikely(do_softlimit))
			mem_cgroup_update_tree(memcg, page);
#if MAX_NUMNODES > 1
		if (unlikely(do_numainfo))
			atomic_inc(&memcg->numainfo_events);
#endif
	}
}

struct mem_cgroup *mem_cgroup_from_task(struct task_struct *p)
{
	/*
	 * mm_update_next_owner() may clear mm->owner to NULL
	 * if it races with swapoff, page migration, etc.
	 * So this can be called with p == NULL.
	 */
	if (unlikely(!p))
		return NULL;

	return mem_cgroup_from_css(task_css(p, memory_cgrp_id));
}
<<<<<<< HEAD
=======
EXPORT_SYMBOL(mem_cgroup_from_task);
>>>>>>> v4.9.227

static struct mem_cgroup *get_mem_cgroup_from_mm(struct mm_struct *mm)
{
	struct mem_cgroup *memcg = NULL;

	rcu_read_lock();
	do {
		/*
		 * Page cache insertions can happen withou an
		 * actual mm context, e.g. during disk probing
		 * on boot, loopback IO, acct() writes etc.
		 */
		if (unlikely(!mm))
			memcg = root_mem_cgroup;
		else {
			memcg = mem_cgroup_from_task(rcu_dereference(mm->owner));
			if (unlikely(!memcg))
				memcg = root_mem_cgroup;
		}
<<<<<<< HEAD
	} while (!css_tryget_online(&memcg->css));
=======
	} while (!css_tryget(&memcg->css));
>>>>>>> v4.9.227
	rcu_read_unlock();
	return memcg;
}

<<<<<<< HEAD
/*
 * Returns a next (in a pre-order walk) alive memcg (with elevated css
 * ref. count) or NULL if the whole root's subtree has been visited.
 *
 * helper function to be used by mem_cgroup_iter
 */
static struct mem_cgroup *__mem_cgroup_iter_next(struct mem_cgroup *root,
		struct mem_cgroup *last_visited)
{
	struct cgroup_subsys_state *prev_css, *next_css;

	prev_css = last_visited ? &last_visited->css : NULL;
skip_node:
	next_css = css_next_descendant_pre(prev_css, &root->css);

	/*
	 * Even if we found a group we have to make sure it is
	 * alive. css && !memcg means that the groups should be
	 * skipped and we should continue the tree walk.
	 * last_visited css is safe to use because it is
	 * protected by css_get and the tree walk is rcu safe.
	 *
	 * We do not take a reference on the root of the tree walk
	 * because we might race with the root removal when it would
	 * be the only node in the iterated hierarchy and mem_cgroup_iter
	 * would end up in an endless loop because it expects that at
	 * least one valid node will be returned. Root cannot disappear
	 * because caller of the iterator should hold it already so
	 * skipping css reference should be safe.
	 */
	if (next_css) {
		struct mem_cgroup *memcg = mem_cgroup_from_css(next_css);

		if (next_css == &root->css)
			return memcg;

		if (css_tryget_online(next_css)) {
			/*
			 * Make sure the memcg is initialized:
			 * mem_cgroup_css_online() orders the the
			 * initialization against setting the flag.
			 */
			if (smp_load_acquire(&memcg->initialized))
				return memcg;
			css_put(next_css);
		}

		prev_css = next_css;
		goto skip_node;
	}

	return NULL;
}

static void mem_cgroup_iter_invalidate(struct mem_cgroup *root)
{
	/*
	 * When a group in the hierarchy below root is destroyed, the
	 * hierarchy iterator can no longer be trusted since it might
	 * have pointed to the destroyed group.  Invalidate it.
	 */
	atomic_inc(&root->dead_count);
}

static struct mem_cgroup *
mem_cgroup_iter_load(struct mem_cgroup_reclaim_iter *iter,
		     struct mem_cgroup *root,
		     int *sequence)
{
	struct mem_cgroup *position = NULL;
	/*
	 * A cgroup destruction happens in two stages: offlining and
	 * release.  They are separated by a RCU grace period.
	 *
	 * If the iterator is valid, we may still race with an
	 * offlining.  The RCU lock ensures the object won't be
	 * released, tryget will fail if we lost the race.
	 */
	*sequence = atomic_read(&root->dead_count);
	if (iter->last_dead_count == *sequence) {
		smp_rmb();
		position = iter->last_visited;

		/*
		 * We cannot take a reference to root because we might race
		 * with root removal and returning NULL would end up in
		 * an endless loop on the iterator user level when root
		 * would be returned all the time.
		 */
		if (position && position != root &&
		    !css_tryget_online(&position->css))
			position = NULL;
	}
	return position;
}

static void mem_cgroup_iter_update(struct mem_cgroup_reclaim_iter *iter,
				   struct mem_cgroup *last_visited,
				   struct mem_cgroup *new_position,
				   struct mem_cgroup *root,
				   int sequence)
{
	/* root reference counting symmetric to mem_cgroup_iter_load */
	if (last_visited && last_visited != root)
		css_put(&last_visited->css);
	/*
	 * We store the sequence count from the time @last_visited was
	 * loaded successfully instead of rereading it here so that we
	 * don't lose destruction events in between.  We could have
	 * raced with the destruction of @new_position after all.
	 */
	iter->last_visited = new_position;
	smp_wmb();
	iter->last_dead_count = sequence;
}

=======
>>>>>>> v4.9.227
/**
 * mem_cgroup_iter - iterate over memory cgroup hierarchy
 * @root: hierarchy root
 * @prev: previously returned memcg, NULL on first invocation
 * @reclaim: cookie for shared reclaim walks, NULL for full walks
 *
 * Returns references to children of the hierarchy below @root, or
 * @root itself, or %NULL after a full round-trip.
 *
 * Caller must pass the return value in @prev on subsequent
 * invocations for reference counting, or use mem_cgroup_iter_break()
 * to cancel a hierarchy walk before the round-trip is complete.
 *
 * Reclaimers can specify a zone and a priority level in @reclaim to
 * divide up the memcgs in the hierarchy among all concurrent
 * reclaimers operating on the same zone and priority.
 */
struct mem_cgroup *mem_cgroup_iter(struct mem_cgroup *root,
				   struct mem_cgroup *prev,
				   struct mem_cgroup_reclaim_cookie *reclaim)
{
<<<<<<< HEAD
	struct mem_cgroup *memcg = NULL;
	struct mem_cgroup *last_visited = NULL;
=======
	struct mem_cgroup_reclaim_iter *uninitialized_var(iter);
	struct cgroup_subsys_state *css = NULL;
	struct mem_cgroup *memcg = NULL;
	struct mem_cgroup *pos = NULL;
>>>>>>> v4.9.227

	if (mem_cgroup_disabled())
		return NULL;

	if (!root)
		root = root_mem_cgroup;

	if (prev && !reclaim)
<<<<<<< HEAD
		last_visited = prev;

	if (!root->use_hierarchy && root != root_mem_cgroup) {
		if (prev)
			goto out_css_put;
=======
		pos = prev;

	if (!root->use_hierarchy && root != root_mem_cgroup) {
		if (prev)
			goto out;
>>>>>>> v4.9.227
		return root;
	}

	rcu_read_lock();
<<<<<<< HEAD
	while (!memcg) {
		struct mem_cgroup_reclaim_iter *uninitialized_var(iter);
		int uninitialized_var(seq);

		if (reclaim) {
			struct mem_cgroup_per_zone *mz;

			mz = mem_cgroup_zone_zoneinfo(root, reclaim->zone);
			iter = &mz->reclaim_iter[reclaim->priority];
			if (prev && reclaim->generation != iter->generation) {
				iter->last_visited = NULL;
				goto out_unlock;
			}

			last_visited = mem_cgroup_iter_load(iter, root, &seq);
		}

		memcg = __mem_cgroup_iter_next(root, last_visited);

		if (reclaim) {
			mem_cgroup_iter_update(iter, last_visited, memcg, root,
					seq);

			if (!memcg)
				iter->generation++;
			else if (!prev && memcg)
				reclaim->generation = iter->generation;
		}

		if (prev && !memcg)
			goto out_unlock;
	}
out_unlock:
	rcu_read_unlock();
out_css_put:
=======

	if (reclaim) {
		struct mem_cgroup_per_node *mz;

		mz = mem_cgroup_nodeinfo(root, reclaim->pgdat->node_id);
		iter = &mz->iter[reclaim->priority];

		if (prev && reclaim->generation != iter->generation)
			goto out_unlock;

		while (1) {
			pos = READ_ONCE(iter->position);
			if (!pos || css_tryget(&pos->css))
				break;
			/*
			 * css reference reached zero, so iter->position will
			 * be cleared by ->css_released. However, we should not
			 * rely on this happening soon, because ->css_released
			 * is called from a work queue, and by busy-waiting we
			 * might block it. So we clear iter->position right
			 * away.
			 */
			(void)cmpxchg(&iter->position, pos, NULL);
		}
	}

	if (pos)
		css = &pos->css;

	for (;;) {
		css = css_next_descendant_pre(css, &root->css);
		if (!css) {
			/*
			 * Reclaimers share the hierarchy walk, and a
			 * new one might jump in right at the end of
			 * the hierarchy - make sure they see at least
			 * one group and restart from the beginning.
			 */
			if (!prev)
				continue;
			break;
		}

		/*
		 * Verify the css and acquire a reference.  The root
		 * is provided by the caller, so we know it's alive
		 * and kicking, and don't take an extra reference.
		 */
		memcg = mem_cgroup_from_css(css);

		if (css == &root->css)
			break;

		if (css_tryget(css))
			break;

		memcg = NULL;
	}

	if (reclaim) {
		/*
		 * The position could have already been updated by a competing
		 * thread, so check that the value hasn't changed since we read
		 * it to avoid reclaiming from the same cgroup twice.
		 */
		(void)cmpxchg(&iter->position, pos, memcg);

		if (pos)
			css_put(&pos->css);

		if (!memcg)
			iter->generation++;
		else if (!prev)
			reclaim->generation = iter->generation;
	}

out_unlock:
	rcu_read_unlock();
out:
>>>>>>> v4.9.227
	if (prev && prev != root)
		css_put(&prev->css);

	return memcg;
}

/**
 * mem_cgroup_iter_break - abort a hierarchy walk prematurely
 * @root: hierarchy root
 * @prev: last visited hierarchy member as returned by mem_cgroup_iter()
 */
void mem_cgroup_iter_break(struct mem_cgroup *root,
			   struct mem_cgroup *prev)
{
	if (!root)
		root = root_mem_cgroup;
	if (prev && prev != root)
		css_put(&prev->css);
}

<<<<<<< HEAD
=======
static void __invalidate_reclaim_iterators(struct mem_cgroup *from,
					struct mem_cgroup *dead_memcg)
{
	struct mem_cgroup_reclaim_iter *iter;
	struct mem_cgroup_per_node *mz;
	int nid;
	int i;

	for_each_node(nid) {
		mz = mem_cgroup_nodeinfo(from, nid);
		for (i = 0; i <= DEF_PRIORITY; i++) {
			iter = &mz->iter[i];
			cmpxchg(&iter->position,
				dead_memcg, NULL);
		}
	}
}

static void invalidate_reclaim_iterators(struct mem_cgroup *dead_memcg)
{
	struct mem_cgroup *memcg = dead_memcg;
	struct mem_cgroup *last;

	do {
		__invalidate_reclaim_iterators(memcg, dead_memcg);
		last = memcg;
	} while ((memcg = parent_mem_cgroup(memcg)));

	/*
	 * When cgruop1 non-hierarchy mode is used,
	 * parent_mem_cgroup() does not walk all the way up to the
	 * cgroup root (root_mem_cgroup). So we have to handle
	 * dead_memcg from cgroup root separately.
	 */
	if (last != root_mem_cgroup)
		__invalidate_reclaim_iterators(root_mem_cgroup,
						dead_memcg);
}

>>>>>>> v4.9.227
/*
 * Iteration constructs for visiting all cgroups (under a tree).  If
 * loops are exited prematurely (break), mem_cgroup_iter_break() must
 * be used for reference counting.
 */
#define for_each_mem_cgroup_tree(iter, root)		\
	for (iter = mem_cgroup_iter(root, NULL, NULL);	\
	     iter != NULL;				\
	     iter = mem_cgroup_iter(root, iter, NULL))

#define for_each_mem_cgroup(iter)			\
	for (iter = mem_cgroup_iter(NULL, NULL, NULL);	\
	     iter != NULL;				\
	     iter = mem_cgroup_iter(NULL, iter, NULL))

<<<<<<< HEAD
void __mem_cgroup_count_vm_event(struct mm_struct *mm, enum vm_event_item idx)
{
	struct mem_cgroup *memcg;

	rcu_read_lock();
	memcg = mem_cgroup_from_task(rcu_dereference(mm->owner));
	if (unlikely(!memcg))
		goto out;

	switch (idx) {
	case PGFAULT:
		this_cpu_inc(memcg->stat->events[MEM_CGROUP_EVENTS_PGFAULT]);
		break;
	case PGMAJFAULT:
		this_cpu_inc(memcg->stat->events[MEM_CGROUP_EVENTS_PGMAJFAULT]);
		break;
	default:
		BUG();
	}
out:
	rcu_read_unlock();
}
EXPORT_SYMBOL(__mem_cgroup_count_vm_event);

/**
 * mem_cgroup_zone_lruvec - get the lru list vector for a zone and memcg
 * @zone: zone of the wanted lruvec
 * @memcg: memcg of the wanted lruvec
 *
 * Returns the lru list vector holding pages for the given @zone and
 * @mem.  This can be the global zone lruvec, if the memory controller
 * is disabled.
 */
struct lruvec *mem_cgroup_zone_lruvec(struct zone *zone,
				      struct mem_cgroup *memcg)
{
	struct mem_cgroup_per_zone *mz;
	struct lruvec *lruvec;

	if (mem_cgroup_disabled()) {
		lruvec = &zone->lruvec;
		goto out;
	}

	mz = mem_cgroup_zone_zoneinfo(memcg, zone);
	lruvec = &mz->lruvec;
out:
	/*
	 * Since a node can be onlined after the mem_cgroup was created,
	 * we have to be prepared to initialize lruvec->zone here;
	 * and if offlined then reonlined, we need to reinitialize it.
	 */
	if (unlikely(lruvec->zone != zone))
		lruvec->zone = zone;
	return lruvec;
}

/**
 * mem_cgroup_page_lruvec - return lruvec for adding an lru page
 * @page: the page
 * @zone: zone of the page
 */
struct lruvec *mem_cgroup_page_lruvec(struct page *page, struct zone *zone)
{
	struct mem_cgroup_per_zone *mz;
	struct mem_cgroup *memcg;
	struct page_cgroup *pc;
	struct lruvec *lruvec;

	if (mem_cgroup_disabled()) {
		lruvec = &zone->lruvec;
		goto out;
	}

	pc = lookup_page_cgroup(page);
	memcg = pc->mem_cgroup;

	/*
	 * Surreptitiously switch any uncharged offlist page to root:
	 * an uncharged page off lru does nothing to secure
	 * its former mem_cgroup from sudden removal.
	 *
	 * Our caller holds lru_lock, and PageCgroupUsed is updated
	 * under page_cgroup lock: between them, they make all uses
	 * of pc->mem_cgroup safe.
	 */
	if (!PageLRU(page) && !PageCgroupUsed(pc) && memcg != root_mem_cgroup)
		pc->mem_cgroup = memcg = root_mem_cgroup;

	mz = mem_cgroup_page_zoneinfo(memcg, page);
=======
/**
 * mem_cgroup_scan_tasks - iterate over tasks of a memory cgroup hierarchy
 * @memcg: hierarchy root
 * @fn: function to call for each task
 * @arg: argument passed to @fn
 *
 * This function iterates over tasks attached to @memcg or to any of its
 * descendants and calls @fn for each task. If @fn returns a non-zero
 * value, the function breaks the iteration loop and returns the value.
 * Otherwise, it will iterate over all tasks and return 0.
 *
 * This function must not be called for the root memory cgroup.
 */
int mem_cgroup_scan_tasks(struct mem_cgroup *memcg,
			  int (*fn)(struct task_struct *, void *), void *arg)
{
	struct mem_cgroup *iter;
	int ret = 0;

	BUG_ON(memcg == root_mem_cgroup);

	for_each_mem_cgroup_tree(iter, memcg) {
		struct css_task_iter it;
		struct task_struct *task;

		css_task_iter_start(&iter->css, &it);
		while (!ret && (task = css_task_iter_next(&it)))
			ret = fn(task, arg);
		css_task_iter_end(&it);
		if (ret) {
			mem_cgroup_iter_break(memcg, iter);
			break;
		}
	}
	return ret;
}

/**
 * mem_cgroup_page_lruvec - return lruvec for isolating/putting an LRU page
 * @page: the page
 * @zone: zone of the page
 *
 * This function is only safe when following the LRU page isolation
 * and putback protocol: the LRU lock must be held, and the page must
 * either be PageLRU() or the caller must have isolated/allocated it.
 */
struct lruvec *mem_cgroup_page_lruvec(struct page *page, struct pglist_data *pgdat)
{
	struct mem_cgroup_per_node *mz;
	struct mem_cgroup *memcg;
	struct lruvec *lruvec;

	if (mem_cgroup_disabled()) {
		lruvec = &pgdat->lruvec;
		goto out;
	}

	memcg = page->mem_cgroup;
	/*
	 * Swapcache readahead pages are added to the LRU - and
	 * possibly migrated - before they are charged.
	 */
	if (!memcg)
		memcg = root_mem_cgroup;

	mz = mem_cgroup_page_nodeinfo(memcg, page);
>>>>>>> v4.9.227
	lruvec = &mz->lruvec;
out:
	/*
	 * Since a node can be onlined after the mem_cgroup was created,
	 * we have to be prepared to initialize lruvec->zone here;
	 * and if offlined then reonlined, we need to reinitialize it.
	 */
<<<<<<< HEAD
	if (unlikely(lruvec->zone != zone))
		lruvec->zone = zone;
=======
	if (unlikely(lruvec->pgdat != pgdat))
		lruvec->pgdat = pgdat;
>>>>>>> v4.9.227
	return lruvec;
}

/**
 * mem_cgroup_update_lru_size - account for adding or removing an lru page
 * @lruvec: mem_cgroup per zone lru vector
 * @lru: index of lru list the page is sitting on
<<<<<<< HEAD
 * @nr_pages: positive when adding or negative when removing
 *
 * This function must be called when a page is added to or removed from an
 * lru list.
 */
void mem_cgroup_update_lru_size(struct lruvec *lruvec, enum lru_list lru,
				int nr_pages)
{
	struct mem_cgroup_per_zone *mz;
	unsigned long *lru_size;
=======
 * @zid: zone id of the accounted pages
 * @nr_pages: positive when adding or negative when removing
 *
 * This function must be called under lru_lock, just before a page is added
 * to or just after a page is removed from an lru list (that ordering being
 * so as to allow it to check that lru_size 0 is consistent with list_empty).
 */
void mem_cgroup_update_lru_size(struct lruvec *lruvec, enum lru_list lru,
				int zid, int nr_pages)
{
	struct mem_cgroup_per_node *mz;
	unsigned long *lru_size;
	long size;
>>>>>>> v4.9.227

	if (mem_cgroup_disabled())
		return;

<<<<<<< HEAD
	mz = container_of(lruvec, struct mem_cgroup_per_zone, lruvec);
	lru_size = mz->lru_size + lru;
	*lru_size += nr_pages;
	VM_BUG_ON((long)(*lru_size) < 0);
}

/*
 * Checks whether given mem is same or in the root_mem_cgroup's
 * hierarchy subtree
 */
bool __mem_cgroup_same_or_subtree(const struct mem_cgroup *root_memcg,
				  struct mem_cgroup *memcg)
{
	if (root_memcg == memcg)
		return true;
	if (!root_memcg->use_hierarchy || !memcg)
		return false;
	return cgroup_is_descendant(memcg->css.cgroup, root_memcg->css.cgroup);
}

static bool mem_cgroup_same_or_subtree(const struct mem_cgroup *root_memcg,
				       struct mem_cgroup *memcg)
{
	bool ret;

	rcu_read_lock();
	ret = __mem_cgroup_same_or_subtree(root_memcg, memcg);
	rcu_read_unlock();
	return ret;
}

bool task_in_mem_cgroup(struct task_struct *task,
			const struct mem_cgroup *memcg)
{
	struct mem_cgroup *curr = NULL;
=======
	mz = container_of(lruvec, struct mem_cgroup_per_node, lruvec);
	lru_size = &mz->lru_zone_size[zid][lru];

	if (nr_pages < 0)
		*lru_size += nr_pages;

	size = *lru_size;
	if (WARN_ONCE(size < 0,
		"%s(%p, %d, %d): lru_size %ld\n",
		__func__, lruvec, lru, nr_pages, size)) {
		VM_BUG_ON(1);
		*lru_size = 0;
	}

	if (nr_pages > 0)
		*lru_size += nr_pages;
}

bool task_in_mem_cgroup(struct task_struct *task, struct mem_cgroup *memcg)
{
	struct mem_cgroup *task_memcg;
>>>>>>> v4.9.227
	struct task_struct *p;
	bool ret;

	p = find_lock_task_mm(task);
	if (p) {
<<<<<<< HEAD
		curr = get_mem_cgroup_from_mm(p->mm);
=======
		task_memcg = get_mem_cgroup_from_mm(p->mm);
>>>>>>> v4.9.227
		task_unlock(p);
	} else {
		/*
		 * All threads may have already detached their mm's, but the oom
		 * killer still needs to detect if they have already been oom
		 * killed to prevent needlessly killing additional tasks.
		 */
		rcu_read_lock();
<<<<<<< HEAD
		curr = mem_cgroup_from_task(task);
		if (curr)
			css_get(&curr->css);
		rcu_read_unlock();
	}
	/*
	 * We should check use_hierarchy of "memcg" not "curr". Because checking
	 * use_hierarchy of "curr" here make this function true if hierarchy is
	 * enabled in "curr" and "curr" is a child of "memcg" in *cgroup*
	 * hierarchy(even if use_hierarchy is disabled in "memcg").
	 */
	ret = mem_cgroup_same_or_subtree(memcg, curr);
	css_put(&curr->css);
	return ret;
}

int mem_cgroup_inactive_anon_is_low(struct lruvec *lruvec)
{
	unsigned long inactive_ratio;
	unsigned long inactive;
	unsigned long active;
	unsigned long gb;

	inactive = mem_cgroup_get_lru_size(lruvec, LRU_INACTIVE_ANON);
	active = mem_cgroup_get_lru_size(lruvec, LRU_ACTIVE_ANON);

	gb = (inactive + active) >> (30 - PAGE_SHIFT);
	if (gb)
		inactive_ratio = int_sqrt(10 * gb);
	else
		inactive_ratio = 1;

	return inactive * inactive_ratio < active;
}

#define mem_cgroup_from_res_counter(counter, member)	\
	container_of(counter, struct mem_cgroup, member)

=======
		task_memcg = mem_cgroup_from_task(task);
		css_get(&task_memcg->css);
		rcu_read_unlock();
	}
	ret = mem_cgroup_is_descendant(task_memcg, memcg);
	css_put(&task_memcg->css);
	return ret;
}

>>>>>>> v4.9.227
/**
 * mem_cgroup_margin - calculate chargeable space of a memory cgroup
 * @memcg: the memory cgroup
 *
 * Returns the maximum amount of memory @mem can be charged with, in
 * pages.
 */
static unsigned long mem_cgroup_margin(struct mem_cgroup *memcg)
{
<<<<<<< HEAD
	unsigned long long margin;

	margin = res_counter_margin(&memcg->res);
	if (do_swap_account)
		margin = min(margin, res_counter_margin(&memcg->memsw));
	return margin >> PAGE_SHIFT;
}

int mem_cgroup_swappiness(struct mem_cgroup *memcg)
{
#if defined(CONFIG_MEMCG_FORCE_USE_VM_SWAPPINESS)
	return vm_swappiness;
#else
	/* root ? */
	if (mem_cgroup_disabled() || !memcg->css.parent)
		return vm_swappiness;

	return memcg->swappiness;
#endif
}

/*
 * memcg->moving_account is used for checking possibility that some thread is
 * calling move_account(). When a thread on CPU-A starts moving pages under
 * a memcg, other threads should check memcg->moving_account under
 * rcu_read_lock(), like this:
 *
 *         CPU-A                                    CPU-B
 *                                              rcu_read_lock()
 *         memcg->moving_account+1              if (memcg->mocing_account)
 *                                                   take heavy locks.
 *         synchronize_rcu()                    update something.
 *                                              rcu_read_unlock()
 *         start move here.
 */

static void mem_cgroup_start_move(struct mem_cgroup *memcg)
{
	atomic_inc(&memcg->moving_account);
	synchronize_rcu();
}

static void mem_cgroup_end_move(struct mem_cgroup *memcg)
{
	/*
	 * Now, mem_cgroup_clear_mc() may call this function with NULL.
	 * We check NULL in callee rather than caller.
	 */
	if (memcg)
		atomic_dec(&memcg->moving_account);
=======
	unsigned long margin = 0;
	unsigned long count;
	unsigned long limit;

	count = page_counter_read(&memcg->memory);
	limit = READ_ONCE(memcg->memory.limit);
	if (count < limit)
		margin = limit - count;

	if (do_memsw_account()) {
		count = page_counter_read(&memcg->memsw);
		limit = READ_ONCE(memcg->memsw.limit);
		if (count <= limit)
			margin = min(margin, limit - count);
		else
			margin = 0;
	}

	return margin;
>>>>>>> v4.9.227
}

/*
 * A routine for checking "mem" is under move_account() or not.
 *
 * Checking a cgroup is mc.from or mc.to or under hierarchy of
 * moving cgroups. This is for waiting at high-memory pressure
 * caused by "move".
 */
static bool mem_cgroup_under_move(struct mem_cgroup *memcg)
{
	struct mem_cgroup *from;
	struct mem_cgroup *to;
	bool ret = false;
	/*
	 * Unlike task_move routines, we access mc.to, mc.from not under
	 * mutual exclusion by cgroup_mutex. Here, we take spinlock instead.
	 */
	spin_lock(&mc.lock);
	from = mc.from;
	to = mc.to;
	if (!from)
		goto unlock;

<<<<<<< HEAD
	ret = mem_cgroup_same_or_subtree(memcg, from)
		|| mem_cgroup_same_or_subtree(memcg, to);
=======
	ret = mem_cgroup_is_descendant(from, memcg) ||
		mem_cgroup_is_descendant(to, memcg);
>>>>>>> v4.9.227
unlock:
	spin_unlock(&mc.lock);
	return ret;
}

static bool mem_cgroup_wait_acct_move(struct mem_cgroup *memcg)
{
	if (mc.moving_task && current != mc.moving_task) {
		if (mem_cgroup_under_move(memcg)) {
			DEFINE_WAIT(wait);
			prepare_to_wait(&mc.waitq, &wait, TASK_INTERRUPTIBLE);
			/* moving charge context might have finished. */
			if (mc.moving_task)
				schedule();
			finish_wait(&mc.waitq, &wait);
			return true;
		}
	}
	return false;
}

<<<<<<< HEAD
/*
 * Take this lock when
 * - a code tries to modify page's memcg while it's USED.
 * - a code tries to modify page state accounting in a memcg.
 */
static void move_lock_mem_cgroup(struct mem_cgroup *memcg,
				  unsigned long *flags)
{
	spin_lock_irqsave(&memcg->move_lock, *flags);
}

static void move_unlock_mem_cgroup(struct mem_cgroup *memcg,
				unsigned long *flags)
{
	spin_unlock_irqrestore(&memcg->move_lock, *flags);
}

=======
>>>>>>> v4.9.227
#define K(x) ((x) << (PAGE_SHIFT-10))
/**
 * mem_cgroup_print_oom_info: Print OOM information relevant to memory controller.
 * @memcg: The memory cgroup that went over limit
 * @p: Task that is going to be killed
 *
 * NOTE: @memcg and @p's mem_cgroup can be different when hierarchy is
 * enabled
 */
void mem_cgroup_print_oom_info(struct mem_cgroup *memcg, struct task_struct *p)
{
<<<<<<< HEAD
	/* oom_info_lock ensures that parallel ooms do not interleave */
	static DEFINE_MUTEX(oom_info_lock);
	struct mem_cgroup *iter;
	unsigned int i;

	if (!p)
		return;

	mutex_lock(&oom_info_lock);
	rcu_read_lock();

	pr_info("Task in ");
	pr_cont_cgroup_path(task_cgroup(p, memory_cgrp_id));
	pr_cont(" killed as a result of limit of ");
=======
	struct mem_cgroup *iter;
	unsigned int i;

	rcu_read_lock();

	if (p) {
		pr_info("Task in ");
		pr_cont_cgroup_path(task_cgroup(p, memory_cgrp_id));
		pr_cont(" killed as a result of limit of ");
	} else {
		pr_info("Memory limit reached of cgroup ");
	}

>>>>>>> v4.9.227
	pr_cont_cgroup_path(memcg->css.cgroup);
	pr_cont("\n");

	rcu_read_unlock();

<<<<<<< HEAD
	pr_info("memory: usage %llukB, limit %llukB, failcnt %llu\n",
		res_counter_read_u64(&memcg->res, RES_USAGE) >> 10,
		res_counter_read_u64(&memcg->res, RES_LIMIT) >> 10,
		res_counter_read_u64(&memcg->res, RES_FAILCNT));
	pr_info("memory+swap: usage %llukB, limit %llukB, failcnt %llu\n",
		res_counter_read_u64(&memcg->memsw, RES_USAGE) >> 10,
		res_counter_read_u64(&memcg->memsw, RES_LIMIT) >> 10,
		res_counter_read_u64(&memcg->memsw, RES_FAILCNT));
	pr_info("kmem: usage %llukB, limit %llukB, failcnt %llu\n",
		res_counter_read_u64(&memcg->kmem, RES_USAGE) >> 10,
		res_counter_read_u64(&memcg->kmem, RES_LIMIT) >> 10,
		res_counter_read_u64(&memcg->kmem, RES_FAILCNT));
=======
	pr_info("memory: usage %llukB, limit %llukB, failcnt %lu\n",
		K((u64)page_counter_read(&memcg->memory)),
		K((u64)memcg->memory.limit), memcg->memory.failcnt);
	pr_info("memory+swap: usage %llukB, limit %llukB, failcnt %lu\n",
		K((u64)page_counter_read(&memcg->memsw)),
		K((u64)memcg->memsw.limit), memcg->memsw.failcnt);
	pr_info("kmem: usage %llukB, limit %llukB, failcnt %lu\n",
		K((u64)page_counter_read(&memcg->kmem)),
		K((u64)memcg->kmem.limit), memcg->kmem.failcnt);
>>>>>>> v4.9.227

	for_each_mem_cgroup_tree(iter, memcg) {
		pr_info("Memory cgroup stats for ");
		pr_cont_cgroup_path(iter->css.cgroup);
		pr_cont(":");

		for (i = 0; i < MEM_CGROUP_STAT_NSTATS; i++) {
			if (i == MEM_CGROUP_STAT_SWAP && !do_swap_account)
				continue;
<<<<<<< HEAD
			pr_cont(" %s:%ldKB", mem_cgroup_stat_names[i],
=======
			pr_cont(" %s:%luKB", mem_cgroup_stat_names[i],
>>>>>>> v4.9.227
				K(mem_cgroup_read_stat(iter, i)));
		}

		for (i = 0; i < NR_LRU_LISTS; i++)
			pr_cont(" %s:%luKB", mem_cgroup_lru_names[i],
				K(mem_cgroup_nr_lru_pages(iter, BIT(i))));

		pr_cont("\n");
	}
<<<<<<< HEAD
	mutex_unlock(&oom_info_lock);
=======
>>>>>>> v4.9.227
}

/*
 * This function returns the number of memcg under hierarchy tree. Returns
 * 1(self count) if no children.
 */
static int mem_cgroup_count_children(struct mem_cgroup *memcg)
{
	int num = 0;
	struct mem_cgroup *iter;

	for_each_mem_cgroup_tree(iter, memcg)
		num++;
	return num;
}

/*
 * Return the memory (and swap, if configured) limit for a memcg.
 */
<<<<<<< HEAD
static u64 mem_cgroup_get_limit(struct mem_cgroup *memcg)
{
	u64 limit;

	limit = res_counter_read_u64(&memcg->res, RES_LIMIT);

	/*
	 * Do not consider swap space if we cannot swap due to swappiness
	 */
	if (mem_cgroup_swappiness(memcg)) {
		u64 memsw;

		limit += total_swap_pages << PAGE_SHIFT;
		memsw = res_counter_read_u64(&memcg->memsw, RES_LIMIT);

		/*
		 * If memsw is finite and limits the amount of swap space
		 * available to this memcg, return that limit.
		 */
		limit = min(limit, memsw);
	}

	return limit;
}

static void mem_cgroup_out_of_memory(struct mem_cgroup *memcg, gfp_t gfp_mask,
				     int order)
{
	struct mem_cgroup *iter;
	unsigned long chosen_points = 0;
	unsigned long totalpages;
	unsigned int points = 0;
	struct task_struct *chosen = NULL;

	/*
	 * If current has a pending SIGKILL or is exiting, then automatically
	 * select it.  The goal is to allow it to allocate so that it may
	 * quickly exit and free its memory.
	 */
	if (fatal_signal_pending(current) || current->flags & PF_EXITING) {
		set_thread_flag(TIF_MEMDIE);
		return;
	}

	check_panic_on_oom(CONSTRAINT_MEMCG, gfp_mask, order, NULL);
	totalpages = mem_cgroup_get_limit(memcg) >> PAGE_SHIFT ? : 1;
	for_each_mem_cgroup_tree(iter, memcg) {
		struct css_task_iter it;
		struct task_struct *task;

		css_task_iter_start(&iter->css, &it);
		while ((task = css_task_iter_next(&it))) {
			switch (oom_scan_process_thread(task, totalpages, NULL,
							false)) {
			case OOM_SCAN_SELECT:
				if (chosen)
					put_task_struct(chosen);
				chosen = task;
				chosen_points = ULONG_MAX;
				get_task_struct(chosen);
				/* fall through */
			case OOM_SCAN_CONTINUE:
				continue;
			case OOM_SCAN_ABORT:
				css_task_iter_end(&it);
				mem_cgroup_iter_break(memcg, iter);
				if (chosen)
					put_task_struct(chosen);
				return;
			case OOM_SCAN_OK:
				break;
			};
			points = oom_badness(task, memcg, NULL, totalpages);
			if (!points || points < chosen_points)
				continue;
			/* Prefer thread group leaders for display purposes */
			if (points == chosen_points &&
			    thread_group_leader(chosen))
				continue;

			if (chosen)
				put_task_struct(chosen);
			chosen = task;
			chosen_points = points;
			get_task_struct(chosen);
		}
		css_task_iter_end(&it);
	}

	if (!chosen)
		return;
	points = chosen_points * 1000 / totalpages;
	oom_kill_process(chosen, gfp_mask, order, points, totalpages, memcg,
			 NULL, "Memory cgroup out of memory");
}

=======
unsigned long mem_cgroup_get_limit(struct mem_cgroup *memcg)
{
	unsigned long limit;

	limit = memcg->memory.limit;
	if (mem_cgroup_swappiness(memcg)) {
		unsigned long memsw_limit;
		unsigned long swap_limit;

		memsw_limit = memcg->memsw.limit;
		swap_limit = memcg->swap.limit;
		swap_limit = min(swap_limit, (unsigned long)total_swap_pages);
		limit = min(limit + swap_limit, memsw_limit);
	}
	return limit;
}

static bool mem_cgroup_out_of_memory(struct mem_cgroup *memcg, gfp_t gfp_mask,
				     int order)
{
	struct oom_control oc = {
		.zonelist = NULL,
		.nodemask = NULL,
		.memcg = memcg,
		.gfp_mask = gfp_mask,
		.order = order,
	};
	bool ret;

	mutex_lock(&oom_lock);
	ret = out_of_memory(&oc);
	mutex_unlock(&oom_lock);
	return ret;
}

#if MAX_NUMNODES > 1

>>>>>>> v4.9.227
/**
 * test_mem_cgroup_node_reclaimable
 * @memcg: the target memcg
 * @nid: the node ID to be checked.
 * @noswap : specify true here if the user wants flle only information.
 *
 * This function returns whether the specified memcg contains any
 * reclaimable pages on a node. Returns true if there are any reclaimable
 * pages in the node.
 */
static bool test_mem_cgroup_node_reclaimable(struct mem_cgroup *memcg,
		int nid, bool noswap)
{
	if (mem_cgroup_node_nr_lru_pages(memcg, nid, LRU_ALL_FILE))
		return true;
	if (noswap || !total_swap_pages)
		return false;
	if (mem_cgroup_node_nr_lru_pages(memcg, nid, LRU_ALL_ANON))
		return true;
	return false;

}
<<<<<<< HEAD
#if MAX_NUMNODES > 1
=======
>>>>>>> v4.9.227

/*
 * Always updating the nodemask is not very good - even if we have an empty
 * list or the wrong list here, we can start from some node and traverse all
 * nodes based on the zonelist. So update the list loosely once per 10 secs.
 *
 */
static void mem_cgroup_may_update_nodemask(struct mem_cgroup *memcg)
{
	int nid;
	/*
	 * numainfo_events > 0 means there was at least NUMAINFO_EVENTS_TARGET
	 * pagein/pageout changes since the last update.
	 */
	if (!atomic_read(&memcg->numainfo_events))
		return;
	if (atomic_inc_return(&memcg->numainfo_updating) > 1)
		return;

	/* make a nodemask where this memcg uses memory from */
	memcg->scan_nodes = node_states[N_MEMORY];

	for_each_node_mask(nid, node_states[N_MEMORY]) {

		if (!test_mem_cgroup_node_reclaimable(memcg, nid, false))
			node_clear(nid, memcg->scan_nodes);
	}

	atomic_set(&memcg->numainfo_events, 0);
	atomic_set(&memcg->numainfo_updating, 0);
}

/*
 * Selecting a node where we start reclaim from. Because what we need is just
 * reducing usage counter, start from anywhere is O,K. Considering
 * memory reclaim from current node, there are pros. and cons.
 *
 * Freeing memory from current node means freeing memory from a node which
 * we'll use or we've used. So, it may make LRU bad. And if several threads
 * hit limits, it will see a contention on a node. But freeing from remote
 * node means more costs for memory reclaim because of memory latency.
 *
 * Now, we use round-robin. Better algorithm is welcomed.
 */
int mem_cgroup_select_victim_node(struct mem_cgroup *memcg)
{
	int node;

	mem_cgroup_may_update_nodemask(memcg);
	node = memcg->last_scanned_node;

<<<<<<< HEAD
	node = next_node(node, memcg->scan_nodes);
	if (node == MAX_NUMNODES)
		node = first_node(memcg->scan_nodes);
	/*
	 * We call this when we hit limit, not when pages are added to LRU.
	 * No LRU may hold pages because all pages are UNEVICTABLE or
	 * memcg is too small and all pages are not on LRU. In that case,
	 * we use curret node.
=======
	node = next_node_in(node, memcg->scan_nodes);
	/*
	 * mem_cgroup_may_update_nodemask might have seen no reclaimmable pages
	 * last time it really checked all the LRUs due to rate limiting.
	 * Fallback to the current node in that case for simplicity.
>>>>>>> v4.9.227
	 */
	if (unlikely(node == MAX_NUMNODES))
		node = numa_node_id();

	memcg->last_scanned_node = node;
	return node;
}
<<<<<<< HEAD

/*
 * Check all nodes whether it contains reclaimable pages or not.
 * For quick scan, we make use of scan_nodes. This will allow us to skip
 * unused nodes. But scan_nodes is lazily updated and may not cotain
 * enough new information. We need to do double check.
 */
static bool mem_cgroup_reclaimable(struct mem_cgroup *memcg, bool noswap)
{
	int nid;

	/*
	 * quick check...making use of scan_node.
	 * We can skip unused nodes.
	 */
	if (!nodes_empty(memcg->scan_nodes)) {
		for (nid = first_node(memcg->scan_nodes);
		     nid < MAX_NUMNODES;
		     nid = next_node(nid, memcg->scan_nodes)) {

			if (test_mem_cgroup_node_reclaimable(memcg, nid, noswap))
				return true;
		}
	}
	/*
	 * Check rest of nodes.
	 */
	for_each_node_state(nid, N_MEMORY) {
		if (node_isset(nid, memcg->scan_nodes))
			continue;
		if (test_mem_cgroup_node_reclaimable(memcg, nid, noswap))
			return true;
	}
	return false;
}

=======
>>>>>>> v4.9.227
#else
int mem_cgroup_select_victim_node(struct mem_cgroup *memcg)
{
	return 0;
}
<<<<<<< HEAD

static bool mem_cgroup_reclaimable(struct mem_cgroup *memcg, bool noswap)
{
	return test_mem_cgroup_node_reclaimable(memcg, 0, noswap);
}
#endif

static int mem_cgroup_soft_reclaim(struct mem_cgroup *root_memcg,
				   struct zone *zone,
=======
#endif

static int mem_cgroup_soft_reclaim(struct mem_cgroup *root_memcg,
				   pg_data_t *pgdat,
>>>>>>> v4.9.227
				   gfp_t gfp_mask,
				   unsigned long *total_scanned)
{
	struct mem_cgroup *victim = NULL;
	int total = 0;
	int loop = 0;
	unsigned long excess;
	unsigned long nr_scanned;
	struct mem_cgroup_reclaim_cookie reclaim = {
<<<<<<< HEAD
		.zone = zone,
		.priority = 0,
	};

	excess = res_counter_soft_limit_excess(&root_memcg->res) >> PAGE_SHIFT;
=======
		.pgdat = pgdat,
		.priority = 0,
	};

	excess = soft_limit_excess(root_memcg);
>>>>>>> v4.9.227

	while (1) {
		victim = mem_cgroup_iter(root_memcg, victim, &reclaim);
		if (!victim) {
			loop++;
			if (loop >= 2) {
				/*
				 * If we have not been able to reclaim
				 * anything, it might because there are
				 * no reclaimable pages under this hierarchy
				 */
				if (!total)
					break;
				/*
				 * We want to do more targeted reclaim.
				 * excess >> 2 is not to excessive so as to
				 * reclaim too much, nor too less that we keep
				 * coming back to reclaim from this cgroup
				 */
				if (total >= (excess >> 2) ||
					(loop > MEM_CGROUP_MAX_RECLAIM_LOOPS))
					break;
			}
			continue;
		}
<<<<<<< HEAD
		if (!mem_cgroup_reclaimable(victim, false))
			continue;
		total += mem_cgroup_shrink_node_zone(victim, gfp_mask, false,
						     zone, &nr_scanned);
		*total_scanned += nr_scanned;
		if (!res_counter_soft_limit_excess(&root_memcg->res))
=======
		total += mem_cgroup_shrink_node(victim, gfp_mask, false,
					pgdat, &nr_scanned);
		*total_scanned += nr_scanned;
		if (!soft_limit_excess(root_memcg))
>>>>>>> v4.9.227
			break;
	}
	mem_cgroup_iter_break(root_memcg, victim);
	return total;
}

#ifdef CONFIG_LOCKDEP
static struct lockdep_map memcg_oom_lock_dep_map = {
	.name = "memcg_oom_lock",
};
#endif

static DEFINE_SPINLOCK(memcg_oom_lock);

/*
 * Check OOM-Killer is already running under our hierarchy.
 * If someone is running, return false.
 */
static bool mem_cgroup_oom_trylock(struct mem_cgroup *memcg)
{
	struct mem_cgroup *iter, *failed = NULL;

	spin_lock(&memcg_oom_lock);

	for_each_mem_cgroup_tree(iter, memcg) {
		if (iter->oom_lock) {
			/*
			 * this subtree of our hierarchy is already locked
			 * so we cannot give a lock.
			 */
			failed = iter;
			mem_cgroup_iter_break(memcg, iter);
			break;
		} else
			iter->oom_lock = true;
	}

	if (failed) {
		/*
		 * OK, we failed to lock the whole subtree so we have
		 * to clean up what we set up to the failing subtree
		 */
		for_each_mem_cgroup_tree(iter, memcg) {
			if (iter == failed) {
				mem_cgroup_iter_break(memcg, iter);
				break;
			}
			iter->oom_lock = false;
		}
	} else
		mutex_acquire(&memcg_oom_lock_dep_map, 0, 1, _RET_IP_);

	spin_unlock(&memcg_oom_lock);

	return !failed;
}

static void mem_cgroup_oom_unlock(struct mem_cgroup *memcg)
{
	struct mem_cgroup *iter;

	spin_lock(&memcg_oom_lock);
	mutex_release(&memcg_oom_lock_dep_map, 1, _RET_IP_);
	for_each_mem_cgroup_tree(iter, memcg)
		iter->oom_lock = false;
	spin_unlock(&memcg_oom_lock);
}

static void mem_cgroup_mark_under_oom(struct mem_cgroup *memcg)
{
	struct mem_cgroup *iter;

<<<<<<< HEAD
	for_each_mem_cgroup_tree(iter, memcg)
		atomic_inc(&iter->under_oom);
=======
	spin_lock(&memcg_oom_lock);
	for_each_mem_cgroup_tree(iter, memcg)
		iter->under_oom++;
	spin_unlock(&memcg_oom_lock);
>>>>>>> v4.9.227
}

static void mem_cgroup_unmark_under_oom(struct mem_cgroup *memcg)
{
	struct mem_cgroup *iter;

	/*
	 * When a new child is created while the hierarchy is under oom,
<<<<<<< HEAD
	 * mem_cgroup_oom_lock() may not be called. We have to use
	 * atomic_add_unless() here.
	 */
	for_each_mem_cgroup_tree(iter, memcg)
		atomic_add_unless(&iter->under_oom, -1, 0);
=======
	 * mem_cgroup_oom_lock() may not be called. Watch for underflow.
	 */
	spin_lock(&memcg_oom_lock);
	for_each_mem_cgroup_tree(iter, memcg)
		if (iter->under_oom > 0)
			iter->under_oom--;
	spin_unlock(&memcg_oom_lock);
>>>>>>> v4.9.227
}

static DECLARE_WAIT_QUEUE_HEAD(memcg_oom_waitq);

struct oom_wait_info {
	struct mem_cgroup *memcg;
	wait_queue_t	wait;
};

static int memcg_oom_wake_function(wait_queue_t *wait,
	unsigned mode, int sync, void *arg)
{
	struct mem_cgroup *wake_memcg = (struct mem_cgroup *)arg;
	struct mem_cgroup *oom_wait_memcg;
	struct oom_wait_info *oom_wait_info;

	oom_wait_info = container_of(wait, struct oom_wait_info, wait);
	oom_wait_memcg = oom_wait_info->memcg;

<<<<<<< HEAD
	/*
	 * Both of oom_wait_info->memcg and wake_memcg are stable under us.
	 * Then we can use css_is_ancestor without taking care of RCU.
	 */
	if (!mem_cgroup_same_or_subtree(oom_wait_memcg, wake_memcg)
		&& !mem_cgroup_same_or_subtree(wake_memcg, oom_wait_memcg))
=======
	if (!mem_cgroup_is_descendant(wake_memcg, oom_wait_memcg) &&
	    !mem_cgroup_is_descendant(oom_wait_memcg, wake_memcg))
>>>>>>> v4.9.227
		return 0;
	return autoremove_wake_function(wait, mode, sync, arg);
}

<<<<<<< HEAD
static void memcg_wakeup_oom(struct mem_cgroup *memcg)
{
	atomic_inc(&memcg->oom_wakeups);
	/* for filtering, pass "memcg" as argument. */
	__wake_up(&memcg_oom_waitq, TASK_NORMAL, 0, memcg);
}

static void memcg_oom_recover(struct mem_cgroup *memcg)
{
	if (memcg && atomic_read(&memcg->under_oom))
		memcg_wakeup_oom(memcg);
=======
static void memcg_oom_recover(struct mem_cgroup *memcg)
{
	/*
	 * For the following lockless ->under_oom test, the only required
	 * guarantee is that it must see the state asserted by an OOM when
	 * this function is called as a result of userland actions
	 * triggered by the notification of the OOM.  This is trivially
	 * achieved by invoking mem_cgroup_mark_under_oom() before
	 * triggering notification.
	 */
	if (memcg && memcg->under_oom)
		__wake_up(&memcg_oom_waitq, TASK_NORMAL, 0, memcg);
>>>>>>> v4.9.227
}

static void mem_cgroup_oom(struct mem_cgroup *memcg, gfp_t mask, int order)
{
<<<<<<< HEAD
	if (!current->memcg_oom.may_oom)
=======
	if (!current->memcg_may_oom)
>>>>>>> v4.9.227
		return;
	/*
	 * We are in the middle of the charge context here, so we
	 * don't want to block when potentially sitting on a callstack
	 * that holds all kinds of filesystem and mm locks.
	 *
	 * Also, the caller may handle a failed allocation gracefully
	 * (like optional page cache readahead) and so an OOM killer
	 * invocation might not even be necessary.
	 *
	 * That's why we don't do anything here except remember the
	 * OOM context and then deal with it at the end of the page
	 * fault when the stack is unwound, the locks are released,
	 * and when we know whether the fault was overall successful.
	 */
	css_get(&memcg->css);
<<<<<<< HEAD
	current->memcg_oom.memcg = memcg;
	current->memcg_oom.gfp_mask = mask;
	current->memcg_oom.order = order;
=======
	current->memcg_in_oom = memcg;
	current->memcg_oom_gfp_mask = mask;
	current->memcg_oom_order = order;
>>>>>>> v4.9.227
}

/**
 * mem_cgroup_oom_synchronize - complete memcg OOM handling
 * @handle: actually kill/wait or just clean up the OOM state
 *
 * This has to be called at the end of a page fault if the memcg OOM
 * handler was enabled.
 *
 * Memcg supports userspace OOM handling where failed allocations must
 * sleep on a waitqueue until the userspace task resolves the
 * situation.  Sleeping directly in the charge context with all kinds
 * of locks held is not a good idea, instead we remember an OOM state
 * in the task and mem_cgroup_oom_synchronize() has to be called at
 * the end of the page fault to complete the OOM handling.
 *
 * Returns %true if an ongoing memcg OOM situation was detected and
 * completed, %false otherwise.
 */
bool mem_cgroup_oom_synchronize(bool handle)
{
<<<<<<< HEAD
	struct mem_cgroup *memcg = current->memcg_oom.memcg;
=======
	struct mem_cgroup *memcg = current->memcg_in_oom;
>>>>>>> v4.9.227
	struct oom_wait_info owait;
	bool locked;

	/* OOM is global, do not handle */
	if (!memcg)
		return false;

	if (!handle)
		goto cleanup;

	owait.memcg = memcg;
	owait.wait.flags = 0;
	owait.wait.func = memcg_oom_wake_function;
	owait.wait.private = current;
	INIT_LIST_HEAD(&owait.wait.task_list);

	prepare_to_wait(&memcg_oom_waitq, &owait.wait, TASK_KILLABLE);
	mem_cgroup_mark_under_oom(memcg);

	locked = mem_cgroup_oom_trylock(memcg);

	if (locked)
		mem_cgroup_oom_notify(memcg);

	if (locked && !memcg->oom_kill_disable) {
		mem_cgroup_unmark_under_oom(memcg);
		finish_wait(&memcg_oom_waitq, &owait.wait);
<<<<<<< HEAD
		mem_cgroup_out_of_memory(memcg, current->memcg_oom.gfp_mask,
					 current->memcg_oom.order);
=======
		mem_cgroup_out_of_memory(memcg, current->memcg_oom_gfp_mask,
					 current->memcg_oom_order);
>>>>>>> v4.9.227
	} else {
		schedule();
		mem_cgroup_unmark_under_oom(memcg);
		finish_wait(&memcg_oom_waitq, &owait.wait);
	}

	if (locked) {
		mem_cgroup_oom_unlock(memcg);
		/*
		 * There is no guarantee that an OOM-lock contender
		 * sees the wakeups triggered by the OOM kill
		 * uncharges.  Wake any sleepers explicitely.
		 */
		memcg_oom_recover(memcg);
	}
cleanup:
<<<<<<< HEAD
	current->memcg_oom.memcg = NULL;
=======
	current->memcg_in_oom = NULL;
>>>>>>> v4.9.227
	css_put(&memcg->css);
	return true;
}

/**
<<<<<<< HEAD
 * mem_cgroup_begin_page_stat - begin a page state statistics transaction
 * @page: page that is going to change accounted state
 * @locked: &memcg->move_lock slowpath was taken
 * @flags: IRQ-state flags for &memcg->move_lock
 *
 * This function must mark the beginning of an accounted page state
 * change to prevent double accounting when the page is concurrently
 * being moved to another memcg:
 *
 *   memcg = mem_cgroup_begin_page_stat(page, &locked, &flags);
 *   if (TestClearPageState(page))
 *     mem_cgroup_update_page_stat(memcg, state, -1);
 *   mem_cgroup_end_page_stat(memcg, locked, flags);
 *
 * The RCU lock is held throughout the transaction.  The fast path can
 * get away without acquiring the memcg->move_lock (@locked is false)
 * because page moving starts with an RCU grace period.
 *
 * The RCU lock also protects the memcg from being freed when the page
 * state that is going to change is the only thing preventing the page
 * from being uncharged.  E.g. end-writeback clearing PageWriteback(),
 * which allows migration to go ahead and uncharge the page before the
 * account transaction might be complete.
 */
struct mem_cgroup *mem_cgroup_begin_page_stat(struct page *page,
					      bool *locked,
					      unsigned long *flags)
{
	struct mem_cgroup *memcg;
	struct page_cgroup *pc;

	rcu_read_lock();

	if (mem_cgroup_disabled())
		return NULL;

	pc = lookup_page_cgroup(page);
again:
	memcg = pc->mem_cgroup;
	if (unlikely(!memcg || !PageCgroupUsed(pc)))
		return NULL;

	*locked = false;
	if (atomic_read(&memcg->moving_account) <= 0)
		return memcg;

	move_lock_mem_cgroup(memcg, flags);
	if (memcg != pc->mem_cgroup || !PageCgroupUsed(pc)) {
		move_unlock_mem_cgroup(memcg, flags);
		goto again;
	}
	*locked = true;

	return memcg;
}

/**
 * mem_cgroup_end_page_stat - finish a page state statistics transaction
 * @memcg: the memcg that was accounted against
 * @locked: value received from mem_cgroup_begin_page_stat()
 * @flags: value received from mem_cgroup_begin_page_stat()
 */
void mem_cgroup_end_page_stat(struct mem_cgroup *memcg, bool *locked,
			      unsigned long *flags)
{
	if (memcg && *locked)
		move_unlock_mem_cgroup(memcg, flags);

	rcu_read_unlock();
}

/**
 * mem_cgroup_update_page_stat - update page state statistics
 * @memcg: memcg to account against
 * @idx: page state item to account
 * @val: number of pages (positive or negative)
 *
 * See mem_cgroup_begin_page_stat() for locking requirements.
 */
void mem_cgroup_update_page_stat(struct mem_cgroup *memcg,
				 enum mem_cgroup_stat_index idx, int val)
{
	VM_BUG_ON(!rcu_read_lock_held());

	if (memcg)
		this_cpu_add(memcg->stat->count[idx], val);
}
=======
 * lock_page_memcg - lock a page->mem_cgroup binding
 * @page: the page
 *
 * This function protects unlocked LRU pages from being moved to
 * another cgroup and stabilizes their page->mem_cgroup binding.
 */
void lock_page_memcg(struct page *page)
{
	struct mem_cgroup *memcg;
	unsigned long flags;

	/*
	 * The RCU lock is held throughout the transaction.  The fast
	 * path can get away without acquiring the memcg->move_lock
	 * because page moving starts with an RCU grace period.
	 */
	rcu_read_lock();

	if (mem_cgroup_disabled())
		return;
again:
	memcg = page->mem_cgroup;
	if (unlikely(!memcg))
		return;

	if (atomic_read(&memcg->moving_account) <= 0)
		return;

	spin_lock_irqsave(&memcg->move_lock, flags);
	if (memcg != page->mem_cgroup) {
		spin_unlock_irqrestore(&memcg->move_lock, flags);
		goto again;
	}

	/*
	 * When charge migration first begins, we can have locked and
	 * unlocked page stat updates happening concurrently.  Track
	 * the task who has the lock for unlock_page_memcg().
	 */
	memcg->move_lock_task = current;
	memcg->move_lock_flags = flags;

	return;
}
EXPORT_SYMBOL(lock_page_memcg);

/**
 * unlock_page_memcg - unlock a page->mem_cgroup binding
 * @page: the page
 */
void unlock_page_memcg(struct page *page)
{
	struct mem_cgroup *memcg = page->mem_cgroup;

	if (memcg && memcg->move_lock_task == current) {
		unsigned long flags = memcg->move_lock_flags;

		memcg->move_lock_task = NULL;
		memcg->move_lock_flags = 0;

		spin_unlock_irqrestore(&memcg->move_lock, flags);
	}

	rcu_read_unlock();
}
EXPORT_SYMBOL(unlock_page_memcg);
>>>>>>> v4.9.227

/*
 * size of first charge trial. "32" comes from vmscan.c's magic value.
 * TODO: maybe necessary to use big numbers in big irons.
 */
#define CHARGE_BATCH	32U
struct memcg_stock_pcp {
	struct mem_cgroup *cached; /* this never be root cgroup */
	unsigned int nr_pages;
	struct work_struct work;
	unsigned long flags;
#define FLUSHING_CACHED_CHARGE	0
};
static DEFINE_PER_CPU(struct memcg_stock_pcp, memcg_stock);
static DEFINE_MUTEX(percpu_charge_mutex);

/**
 * consume_stock: Try to consume stocked charge on this cpu.
 * @memcg: memcg to consume from.
 * @nr_pages: how many pages to charge.
 *
 * The charges will only happen if @memcg matches the current cpu's memcg
 * stock, and at least @nr_pages are available in that stock.  Failure to
 * service an allocation will refill the stock.
 *
 * returns true if successful, false otherwise.
 */
static bool consume_stock(struct mem_cgroup *memcg, unsigned int nr_pages)
{
	struct memcg_stock_pcp *stock;
<<<<<<< HEAD
	bool ret = true;

	if (nr_pages > CHARGE_BATCH)
		return false;

	stock = &get_cpu_var(memcg_stock);
	if (memcg == stock->cached && stock->nr_pages >= nr_pages)
		stock->nr_pages -= nr_pages;
	else /* need to call res_counter_charge */
		ret = false;
	put_cpu_var(memcg_stock);
=======
	unsigned long flags;
	bool ret = false;

	if (nr_pages > CHARGE_BATCH)
		return ret;

	local_irq_save(flags);

	stock = this_cpu_ptr(&memcg_stock);
	if (memcg == stock->cached && stock->nr_pages >= nr_pages) {
		stock->nr_pages -= nr_pages;
		ret = true;
	}

	local_irq_restore(flags);

>>>>>>> v4.9.227
	return ret;
}

/*
<<<<<<< HEAD
 * Returns stocks cached in percpu to res_counter and reset cached information.
=======
 * Returns stocks cached in percpu and reset cached information.
>>>>>>> v4.9.227
 */
static void drain_stock(struct memcg_stock_pcp *stock)
{
	struct mem_cgroup *old = stock->cached;

	if (stock->nr_pages) {
<<<<<<< HEAD
		unsigned long bytes = stock->nr_pages * PAGE_SIZE;

		res_counter_uncharge(&old->res, bytes);
		if (do_swap_account)
			res_counter_uncharge(&old->memsw, bytes);
=======
		page_counter_uncharge(&old->memory, stock->nr_pages);
		if (do_memsw_account())
			page_counter_uncharge(&old->memsw, stock->nr_pages);
		css_put_many(&old->css, stock->nr_pages);
>>>>>>> v4.9.227
		stock->nr_pages = 0;
	}
	stock->cached = NULL;
}

<<<<<<< HEAD
/*
 * This must be called under preempt disabled or must be called by
 * a thread which is pinned to local cpu.
 */
static void drain_local_stock(struct work_struct *dummy)
{
	struct memcg_stock_pcp *stock = this_cpu_ptr(&memcg_stock);
	drain_stock(stock);
	clear_bit(FLUSHING_CACHED_CHARGE, &stock->flags);
}

static void __init memcg_stock_init(void)
{
	int cpu;

	for_each_possible_cpu(cpu) {
		struct memcg_stock_pcp *stock =
					&per_cpu(memcg_stock, cpu);
		INIT_WORK(&stock->work, drain_local_stock);
	}
}

/*
 * Cache charges(val) which is from res_counter, to local per_cpu area.
=======
static void drain_local_stock(struct work_struct *dummy)
{
	struct memcg_stock_pcp *stock;
	unsigned long flags;

	local_irq_save(flags);

	stock = this_cpu_ptr(&memcg_stock);
	drain_stock(stock);
	clear_bit(FLUSHING_CACHED_CHARGE, &stock->flags);

	local_irq_restore(flags);
}

/*
 * Cache charges(val) to local per_cpu area.
>>>>>>> v4.9.227
 * This will be consumed by consume_stock() function, later.
 */
static void refill_stock(struct mem_cgroup *memcg, unsigned int nr_pages)
{
<<<<<<< HEAD
	struct memcg_stock_pcp *stock = &get_cpu_var(memcg_stock);

=======
	struct memcg_stock_pcp *stock;
	unsigned long flags;

	local_irq_save(flags);

	stock = this_cpu_ptr(&memcg_stock);
>>>>>>> v4.9.227
	if (stock->cached != memcg) { /* reset if necessary */
		drain_stock(stock);
		stock->cached = memcg;
	}
	stock->nr_pages += nr_pages;
<<<<<<< HEAD
	put_cpu_var(memcg_stock);
=======

	local_irq_restore(flags);
>>>>>>> v4.9.227
}

/*
 * Drains all per-CPU charge caches for given root_memcg resp. subtree
<<<<<<< HEAD
 * of the hierarchy under it. sync flag says whether we should block
 * until the work is done.
 */
static void drain_all_stock(struct mem_cgroup *root_memcg, bool sync)
{
	int cpu, curcpu;

=======
 * of the hierarchy under it.
 */
static void drain_all_stock(struct mem_cgroup *root_memcg)
{
	int cpu, curcpu;

	/* If someone's already draining, avoid adding running more workers. */
	if (!mutex_trylock(&percpu_charge_mutex))
		return;
>>>>>>> v4.9.227
	/* Notify other cpus that system-wide "drain" is running */
	get_online_cpus();
	curcpu = get_cpu();
	for_each_online_cpu(cpu) {
		struct memcg_stock_pcp *stock = &per_cpu(memcg_stock, cpu);
		struct mem_cgroup *memcg;

		memcg = stock->cached;
		if (!memcg || !stock->nr_pages)
			continue;
<<<<<<< HEAD
		if (!mem_cgroup_same_or_subtree(root_memcg, memcg))
=======
		if (!mem_cgroup_is_descendant(memcg, root_memcg))
>>>>>>> v4.9.227
			continue;
		if (!test_and_set_bit(FLUSHING_CACHED_CHARGE, &stock->flags)) {
			if (cpu == curcpu)
				drain_local_stock(&stock->work);
			else
				schedule_work_on(cpu, &stock->work);
		}
	}
	put_cpu();
<<<<<<< HEAD

	if (!sync)
		goto out;

	for_each_online_cpu(cpu) {
		struct memcg_stock_pcp *stock = &per_cpu(memcg_stock, cpu);
		if (test_bit(FLUSHING_CACHED_CHARGE, &stock->flags))
			flush_work(&stock->work);
	}
out:
	put_online_cpus();
}

/*
 * Tries to drain stocked charges in other cpus. This function is asynchronous
 * and just put a work per cpu for draining localy on each cpu. Caller can
 * expects some charges will be back to res_counter later but cannot wait for
 * it.
 */
static void drain_all_stock_async(struct mem_cgroup *root_memcg)
{
	/*
	 * If someone calls draining, avoid adding more kworker runs.
	 */
	if (!mutex_trylock(&percpu_charge_mutex))
		return;
	drain_all_stock(root_memcg, false);
	mutex_unlock(&percpu_charge_mutex);
}

/* This is a synchronous drain interface. */
static void drain_all_stock_sync(struct mem_cgroup *root_memcg)
{
	/* called when force_empty is called */
	mutex_lock(&percpu_charge_mutex);
	drain_all_stock(root_memcg, true);
=======
	put_online_cpus();
>>>>>>> v4.9.227
	mutex_unlock(&percpu_charge_mutex);
}

static int memcg_cpu_hotplug_callback(struct notifier_block *nb,
					unsigned long action,
					void *hcpu)
{
	int cpu = (unsigned long)hcpu;
	struct memcg_stock_pcp *stock;

	if (action == CPU_ONLINE)
		return NOTIFY_OK;

	if (action != CPU_DEAD && action != CPU_DEAD_FROZEN)
		return NOTIFY_OK;

	stock = &per_cpu(memcg_stock, cpu);
	drain_stock(stock);
	return NOTIFY_OK;
}

<<<<<<< HEAD
=======
static void reclaim_high(struct mem_cgroup *memcg,
			 unsigned int nr_pages,
			 gfp_t gfp_mask)
{
	do {
		if (page_counter_read(&memcg->memory) <= memcg->high)
			continue;
		mem_cgroup_events(memcg, MEMCG_HIGH, 1);
		try_to_free_mem_cgroup_pages(memcg, nr_pages, gfp_mask, true);
	} while ((memcg = parent_mem_cgroup(memcg)));
}

static void high_work_func(struct work_struct *work)
{
	struct mem_cgroup *memcg;

	memcg = container_of(work, struct mem_cgroup, high_work);
	reclaim_high(memcg, CHARGE_BATCH, GFP_KERNEL);
}

/*
 * Scheduled by try_charge() to be executed from the userland return path
 * and reclaims memory over the high limit.
 */
void mem_cgroup_handle_over_high(void)
{
	unsigned int nr_pages = current->memcg_nr_pages_over_high;
	struct mem_cgroup *memcg;

	if (likely(!nr_pages))
		return;

	memcg = get_mem_cgroup_from_mm(current->mm);
	reclaim_high(memcg, nr_pages, GFP_KERNEL);
	css_put(&memcg->css);
	current->memcg_nr_pages_over_high = 0;
}

>>>>>>> v4.9.227
static int try_charge(struct mem_cgroup *memcg, gfp_t gfp_mask,
		      unsigned int nr_pages)
{
	unsigned int batch = max(CHARGE_BATCH, nr_pages);
	int nr_retries = MEM_CGROUP_RECLAIM_RETRIES;
	struct mem_cgroup *mem_over_limit;
<<<<<<< HEAD
	struct res_counter *fail_res;
	unsigned long nr_reclaimed;
	unsigned long long size;
	bool may_swap = true;
	bool drained = false;
	int ret = 0;

	if (mem_cgroup_is_root(memcg))
		goto done;
retry:
	if (consume_stock(memcg, nr_pages))
		goto done;

	size = batch * PAGE_SIZE;
	if (!do_swap_account ||
	    !res_counter_charge(&memcg->memsw, size, &fail_res)) {
		if (!res_counter_charge(&memcg->res, size, &fail_res))
			goto done_restock;
		if (do_swap_account)
			res_counter_uncharge(&memcg->memsw, size);
		mem_over_limit = mem_cgroup_from_res_counter(fail_res, res);
	} else {
		mem_over_limit = mem_cgroup_from_res_counter(fail_res, memsw);
=======
	struct page_counter *counter;
	unsigned long nr_reclaimed;
	bool may_swap = true;
	bool drained = false;

	if (mem_cgroup_is_root(memcg))
		return 0;
retry:
	if (consume_stock(memcg, nr_pages))
		return 0;

	if (!do_memsw_account() ||
	    page_counter_try_charge(&memcg->memsw, batch, &counter)) {
		if (page_counter_try_charge(&memcg->memory, batch, &counter))
			goto done_restock;
		if (do_memsw_account())
			page_counter_uncharge(&memcg->memsw, batch);
		mem_over_limit = mem_cgroup_from_counter(counter, memory);
	} else {
		mem_over_limit = mem_cgroup_from_counter(counter, memsw);
>>>>>>> v4.9.227
		may_swap = false;
	}

	if (batch > nr_pages) {
		batch = nr_pages;
		goto retry;
	}

	/*
	 * Unlike in global OOM situations, memcg is not in a physical
	 * memory shortage.  Allow dying and OOM-killed tasks to
	 * bypass the last charges so that they can exit quickly and
	 * free their memory.
	 */
	if (unlikely(test_thread_flag(TIF_MEMDIE) ||
		     fatal_signal_pending(current) ||
		     current->flags & PF_EXITING))
<<<<<<< HEAD
		goto bypass;
=======
		goto force;

	/*
	 * Prevent unbounded recursion when reclaim operations need to
	 * allocate memory. This might exceed the limits temporarily,
	 * but we prefer facilitating memory reclaim and getting back
	 * under the limit over triggering OOM kills in these cases.
	 */
	if (unlikely(current->flags & PF_MEMALLOC))
		goto force;
>>>>>>> v4.9.227

	if (unlikely(task_in_memcg_oom(current)))
		goto nomem;

<<<<<<< HEAD
	if (!(gfp_mask & __GFP_WAIT))
		goto nomem;

=======
	if (!gfpflags_allow_blocking(gfp_mask))
		goto nomem;

	mem_cgroup_events(mem_over_limit, MEMCG_MAX, 1);

>>>>>>> v4.9.227
	nr_reclaimed = try_to_free_mem_cgroup_pages(mem_over_limit, nr_pages,
						    gfp_mask, may_swap);

	if (mem_cgroup_margin(mem_over_limit) >= nr_pages)
		goto retry;

	if (!drained) {
<<<<<<< HEAD
		drain_all_stock_async(mem_over_limit);
=======
		drain_all_stock(mem_over_limit);
>>>>>>> v4.9.227
		drained = true;
		goto retry;
	}

	if (gfp_mask & __GFP_NORETRY)
		goto nomem;
	/*
	 * Even though the limit is exceeded at this point, reclaim
	 * may have been able to free some pages.  Retry the charge
	 * before killing the task.
	 *
	 * Only for regular pages, though: huge pages are rather
	 * unlikely to succeed so close to the limit, and we fall back
	 * to regular pages anyway in case of failure.
	 */
	if (nr_reclaimed && nr_pages <= (1 << PAGE_ALLOC_COSTLY_ORDER))
		goto retry;
	/*
	 * At task move, charge accounts can be doubly counted. So, it's
	 * better to wait until the end of task_move if something is going on.
	 */
	if (mem_cgroup_wait_acct_move(mem_over_limit))
		goto retry;

	if (nr_retries--)
		goto retry;

	if (gfp_mask & __GFP_NOFAIL)
<<<<<<< HEAD
		goto bypass;

	if (fatal_signal_pending(current))
		goto bypass;

	mem_cgroup_oom(mem_over_limit, gfp_mask, get_order(nr_pages));
nomem:
	if (!(gfp_mask & __GFP_NOFAIL))
		return -ENOMEM;
bypass:
	return -EINTR;

done_restock:
	if (batch > nr_pages)
		refill_stock(memcg, batch - nr_pages);
done:
	return ret;
=======
		goto force;

	if (fatal_signal_pending(current))
		goto force;

	mem_cgroup_events(mem_over_limit, MEMCG_OOM, 1);

	mem_cgroup_oom(mem_over_limit, gfp_mask,
		       get_order(nr_pages * PAGE_SIZE));
nomem:
	if (!(gfp_mask & __GFP_NOFAIL))
		return -ENOMEM;
force:
	/*
	 * The allocation either can't fail or will lead to more memory
	 * being freed very soon.  Allow memory usage go over the limit
	 * temporarily by force charging it.
	 */
	page_counter_charge(&memcg->memory, nr_pages);
	if (do_memsw_account())
		page_counter_charge(&memcg->memsw, nr_pages);
	css_get_many(&memcg->css, nr_pages);

	return 0;

done_restock:
	css_get_many(&memcg->css, batch);
	if (batch > nr_pages)
		refill_stock(memcg, batch - nr_pages);

	/*
	 * If the hierarchy is above the normal consumption range, schedule
	 * reclaim on returning to userland.  We can perform reclaim here
	 * if __GFP_RECLAIM but let's always punt for simplicity and so that
	 * GFP_KERNEL can consistently be used during reclaim.  @memcg is
	 * not recorded as it most likely matches current's and won't
	 * change in the meantime.  As high limit is checked again before
	 * reclaim, the cost of mismatch is negligible.
	 */
	do {
		if (page_counter_read(&memcg->memory) > memcg->high) {
			/* Don't bother a random interrupted task */
			if (in_interrupt()) {
				schedule_work(&memcg->high_work);
				break;
			}
			current->memcg_nr_pages_over_high += batch;
			set_notify_resume(current);
			break;
		}
	} while ((memcg = parent_mem_cgroup(memcg)));

	return 0;
>>>>>>> v4.9.227
}

static void cancel_charge(struct mem_cgroup *memcg, unsigned int nr_pages)
{
<<<<<<< HEAD
	unsigned long bytes = nr_pages * PAGE_SIZE;

	if (mem_cgroup_is_root(memcg))
		return;

	res_counter_uncharge(&memcg->res, bytes);
	if (do_swap_account)
		res_counter_uncharge(&memcg->memsw, bytes);
}

/*
 * Cancel chrages in this cgroup....doesn't propagate to parent cgroup.
 * This is useful when moving usage to parent cgroup.
 */
static void __mem_cgroup_cancel_local_charge(struct mem_cgroup *memcg,
					unsigned int nr_pages)
{
	unsigned long bytes = nr_pages * PAGE_SIZE;

	if (mem_cgroup_is_root(memcg))
		return;

	res_counter_uncharge_until(&memcg->res, memcg->res.parent, bytes);
	if (do_swap_account)
		res_counter_uncharge_until(&memcg->memsw,
						memcg->memsw.parent, bytes);
}

/*
 * A helper function to get mem_cgroup from ID. must be called under
 * rcu_read_lock().  The caller is responsible for calling
 * css_tryget_online() if the mem_cgroup is used for charging. (dropping
 * refcnt from swap can be called against removed memcg.)
 */
static struct mem_cgroup *mem_cgroup_lookup(unsigned short id)
{
	/* ID 0 is unused ID */
	if (!id)
		return NULL;
	return mem_cgroup_from_id(id);
}

/*
 * try_get_mem_cgroup_from_page - look up page's memcg association
 * @page: the page
 *
 * Look up, get a css reference, and return the memcg that owns @page.
 *
 * The page must be locked to prevent racing with swap-in and page
 * cache charges.  If coming from an unlocked page table, the caller
 * must ensure the page is on the LRU or this can race with charging.
 */
struct mem_cgroup *try_get_mem_cgroup_from_page(struct page *page)
{
	struct mem_cgroup *memcg = NULL;
	struct page_cgroup *pc;
	unsigned short id;
	swp_entry_t ent;

	VM_BUG_ON_PAGE(!PageLocked(page), page);

	pc = lookup_page_cgroup(page);
	if (PageCgroupUsed(pc)) {
		memcg = pc->mem_cgroup;
		if (memcg && !css_tryget_online(&memcg->css))
			memcg = NULL;
	} else if (PageSwapCache(page)) {
		ent.val = page_private(page);
		id = lookup_swap_cgroup_id(ent);
		rcu_read_lock();
		memcg = mem_cgroup_lookup(id);
		if (memcg && !css_tryget_online(&memcg->css))
			memcg = NULL;
		rcu_read_unlock();
	}
	return memcg;
=======
	if (mem_cgroup_is_root(memcg))
		return;

	page_counter_uncharge(&memcg->memory, nr_pages);
	if (do_memsw_account())
		page_counter_uncharge(&memcg->memsw, nr_pages);

	css_put_many(&memcg->css, nr_pages);
>>>>>>> v4.9.227
}

static void lock_page_lru(struct page *page, int *isolated)
{
	struct zone *zone = page_zone(page);

<<<<<<< HEAD
	spin_lock_irq(&zone->lru_lock);
	if (PageLRU(page)) {
		struct lruvec *lruvec;

		lruvec = mem_cgroup_page_lruvec(page, zone);
=======
	spin_lock_irq(zone_lru_lock(zone));
	if (PageLRU(page)) {
		struct lruvec *lruvec;

		lruvec = mem_cgroup_page_lruvec(page, zone->zone_pgdat);
>>>>>>> v4.9.227
		ClearPageLRU(page);
		del_page_from_lru_list(page, lruvec, page_lru(page));
		*isolated = 1;
	} else
		*isolated = 0;
}

static void unlock_page_lru(struct page *page, int isolated)
{
	struct zone *zone = page_zone(page);

	if (isolated) {
		struct lruvec *lruvec;

<<<<<<< HEAD
		lruvec = mem_cgroup_page_lruvec(page, zone);
=======
		lruvec = mem_cgroup_page_lruvec(page, zone->zone_pgdat);
>>>>>>> v4.9.227
		VM_BUG_ON_PAGE(PageLRU(page), page);
		SetPageLRU(page);
		add_page_to_lru_list(page, lruvec, page_lru(page));
	}
<<<<<<< HEAD
	spin_unlock_irq(&zone->lru_lock);
=======
	spin_unlock_irq(zone_lru_lock(zone));
>>>>>>> v4.9.227
}

static void commit_charge(struct page *page, struct mem_cgroup *memcg,
			  bool lrucare)
{
<<<<<<< HEAD
	struct page_cgroup *pc = lookup_page_cgroup(page);
	int isolated;

	VM_BUG_ON_PAGE(PageCgroupUsed(pc), page);
	/*
	 * we don't need page_cgroup_lock about tail pages, becase they are not
	 * accessed by any other context at this point.
	 */
=======
	int isolated;

	VM_BUG_ON_PAGE(page->mem_cgroup, page);
>>>>>>> v4.9.227

	/*
	 * In some cases, SwapCache and FUSE(splice_buf->radixtree), the page
	 * may already be on some other mem_cgroup's LRU.  Take care of it.
	 */
	if (lrucare)
		lock_page_lru(page, &isolated);

	/*
	 * Nobody should be changing or seriously looking at
<<<<<<< HEAD
	 * pc->mem_cgroup and pc->flags at this point:
=======
	 * page->mem_cgroup at this point:
>>>>>>> v4.9.227
	 *
	 * - the page is uncharged
	 *
	 * - the page is off-LRU
	 *
	 * - an anonymous fault has exclusive page access, except for
	 *   a locked page table
	 *
	 * - a page cache insertion, a swapin fault, or a migration
	 *   have the page locked
	 */
<<<<<<< HEAD
	pc->mem_cgroup = memcg;
	pc->flags = PCG_USED | PCG_MEM | (do_swap_account ? PCG_MEMSW : 0);
=======
	page->mem_cgroup = memcg;
>>>>>>> v4.9.227

	if (lrucare)
		unlock_page_lru(page, isolated);
}

<<<<<<< HEAD
static DEFINE_MUTEX(set_limit_mutex);

#ifdef CONFIG_MEMCG_KMEM
/*
 * The memcg_slab_mutex is held whenever a per memcg kmem cache is created or
 * destroyed. It protects memcg_caches arrays and memcg_slab_caches lists.
 */
static DEFINE_MUTEX(memcg_slab_mutex);

static DEFINE_MUTEX(activate_kmem_mutex);

/*
 * This is a bit cumbersome, but it is rarely used and avoids a backpointer
 * in the memcg_cache_params struct.
 */
static struct kmem_cache *memcg_params_to_cache(struct memcg_cache_params *p)
{
	struct kmem_cache *cachep;

	VM_BUG_ON(p->is_root_cache);
	cachep = p->root_cache;
	return cache_from_memcg_idx(cachep, memcg_cache_id(p->memcg));
}

#ifdef CONFIG_SLABINFO
static int mem_cgroup_slabinfo_read(struct seq_file *m, void *v)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(seq_css(m));
	struct memcg_cache_params *params;

	if (!memcg_kmem_is_active(memcg))
		return -EIO;

	print_slabinfo_header(m);

	mutex_lock(&memcg_slab_mutex);
	list_for_each_entry(params, &memcg->memcg_slab_caches, list)
		cache_show(memcg_params_to_cache(params), m);
	mutex_unlock(&memcg_slab_mutex);

	return 0;
}
#endif

static int memcg_charge_kmem(struct mem_cgroup *memcg, gfp_t gfp, u64 size)
{
	struct res_counter *fail_res;
	int ret = 0;

	ret = res_counter_charge(&memcg->kmem, size, &fail_res);
	if (ret)
		return ret;

	ret = try_charge(memcg, gfp, size >> PAGE_SHIFT);
	if (ret == -EINTR)  {
		/*
		 * try_charge() chose to bypass to root due to OOM kill or
		 * fatal signal.  Since our only options are to either fail
		 * the allocation or charge it to this cgroup, do it as a
		 * temporary condition. But we can't fail. From a kmem/slab
		 * perspective, the cache has already been selected, by
		 * mem_cgroup_kmem_get_cache(), so it is too late to change
		 * our minds.
		 *
		 * This condition will only trigger if the task entered
		 * memcg_charge_kmem in a sane state, but was OOM-killed
		 * during try_charge() above. Tasks that were already dying
		 * when the allocation triggers should have been already
		 * directed to the root cgroup in memcontrol.h
		 */
		res_counter_charge_nofail(&memcg->res, size, &fail_res);
		if (do_swap_account)
			res_counter_charge_nofail(&memcg->memsw, size,
						  &fail_res);
		ret = 0;
	} else if (ret)
		res_counter_uncharge(&memcg->kmem, size);

	return ret;
}

static void memcg_uncharge_kmem(struct mem_cgroup *memcg, u64 size)
{
	res_counter_uncharge(&memcg->res, size);
	if (do_swap_account)
		res_counter_uncharge(&memcg->memsw, size);

	/* Not down to 0 */
	if (res_counter_uncharge(&memcg->kmem, size))
		return;

	/*
	 * Releases a reference taken in kmem_cgroup_css_offline in case
	 * this last uncharge is racing with the offlining code or it is
	 * outliving the memcg existence.
	 *
	 * The memory barrier imposed by test&clear is paired with the
	 * explicit one in memcg_kmem_mark_dead().
	 */
	if (memcg_kmem_test_and_clear_dead(memcg))
		css_put(&memcg->css);
}

/*
 * helper for acessing a memcg's index. It will be used as an index in the
 * child cache array in kmem_cache, and also to derive its name. This function
 * will return -1 when this is not a kmem-limited memcg.
 */
int memcg_cache_id(struct mem_cgroup *memcg)
{
	return memcg ? memcg->kmemcg_id : -1;
}

=======
#ifndef CONFIG_SLOB
>>>>>>> v4.9.227
static int memcg_alloc_cache_id(void)
{
	int id, size;
	int err;

<<<<<<< HEAD
	id = ida_simple_get(&kmem_limited_groups,
=======
	id = ida_simple_get(&memcg_cache_ida,
>>>>>>> v4.9.227
			    0, MEMCG_CACHES_MAX_SIZE, GFP_KERNEL);
	if (id < 0)
		return id;

<<<<<<< HEAD
	if (id < memcg_limited_groups_array_size)
=======
	if (id < memcg_nr_cache_ids)
>>>>>>> v4.9.227
		return id;

	/*
	 * There's no space for the new id in memcg_caches arrays,
	 * so we have to grow them.
	 */
<<<<<<< HEAD
=======
	down_write(&memcg_cache_ids_sem);
>>>>>>> v4.9.227

	size = 2 * (id + 1);
	if (size < MEMCG_CACHES_MIN_SIZE)
		size = MEMCG_CACHES_MIN_SIZE;
	else if (size > MEMCG_CACHES_MAX_SIZE)
		size = MEMCG_CACHES_MAX_SIZE;

<<<<<<< HEAD
	mutex_lock(&memcg_slab_mutex);
	err = memcg_update_all_caches(size);
	mutex_unlock(&memcg_slab_mutex);

	if (err) {
		ida_simple_remove(&kmem_limited_groups, id);
=======
	err = memcg_update_all_caches(size);
	if (!err)
		err = memcg_update_all_list_lrus(size);
	if (!err)
		memcg_nr_cache_ids = size;

	up_write(&memcg_cache_ids_sem);

	if (err) {
		ida_simple_remove(&memcg_cache_ida, id);
>>>>>>> v4.9.227
		return err;
	}
	return id;
}

static void memcg_free_cache_id(int id)
{
<<<<<<< HEAD
	ida_simple_remove(&kmem_limited_groups, id);
}

/*
 * We should update the current array size iff all caches updates succeed. This
 * can only be done from the slab side. The slab mutex needs to be held when
 * calling this.
 */
void memcg_update_array_size(int num)
{
	memcg_limited_groups_array_size = num;
}

static void memcg_register_cache(struct mem_cgroup *memcg,
				 struct kmem_cache *root_cache)
{
	static char memcg_name_buf[NAME_MAX + 1]; /* protected by
						     memcg_slab_mutex */
	struct kmem_cache *cachep;
	int id;

	lockdep_assert_held(&memcg_slab_mutex);

	id = memcg_cache_id(memcg);

	/*
	 * Since per-memcg caches are created asynchronously on first
	 * allocation (see memcg_kmem_get_cache()), several threads can try to
	 * create the same cache, but only one of them may succeed.
	 */
	if (cache_from_memcg_idx(root_cache, id))
		return;

	cgroup_name(memcg->css.cgroup, memcg_name_buf, NAME_MAX + 1);
	cachep = memcg_create_kmem_cache(memcg, root_cache, memcg_name_buf);
	/*
	 * If we could not create a memcg cache, do not complain, because
	 * that's not critical at all as we can always proceed with the root
	 * cache.
	 */
	if (!cachep)
		return;

	css_get(&memcg->css);
	list_add(&cachep->memcg_params->list, &memcg->memcg_slab_caches);

	/*
	 * Since readers won't lock (see cache_from_memcg_idx()), we need a
	 * barrier here to ensure nobody will see the kmem_cache partially
	 * initialized.
	 */
	smp_wmb();

	BUG_ON(root_cache->memcg_params->memcg_caches[id]);
	root_cache->memcg_params->memcg_caches[id] = cachep;
}

static void memcg_unregister_cache(struct kmem_cache *cachep)
{
	struct kmem_cache *root_cache;
	struct mem_cgroup *memcg;
	int id;

	lockdep_assert_held(&memcg_slab_mutex);

	BUG_ON(is_root_cache(cachep));

	root_cache = cachep->memcg_params->root_cache;
	memcg = cachep->memcg_params->memcg;
	id = memcg_cache_id(memcg);

	BUG_ON(root_cache->memcg_params->memcg_caches[id] != cachep);
	root_cache->memcg_params->memcg_caches[id] = NULL;

	list_del(&cachep->memcg_params->list);

	kmem_cache_destroy(cachep);

	/* drop the reference taken in memcg_register_cache */
	css_put(&memcg->css);
}

/*
 * During the creation a new cache, we need to disable our accounting mechanism
 * altogether. This is true even if we are not creating, but rather just
 * enqueing new caches to be created.
 *
 * This is because that process will trigger allocations; some visible, like
 * explicit kmallocs to auxiliary data structures, name strings and internal
 * cache structures; some well concealed, like INIT_WORK() that can allocate
 * objects during debug.
 *
 * If any allocation happens during memcg_kmem_get_cache, we will recurse back
 * to it. This may not be a bounded recursion: since the first cache creation
 * failed to complete (waiting on the allocation), we'll just try to create the
 * cache again, failing at the same point.
 *
 * memcg_kmem_get_cache is prepared to abort after seeing a positive count of
 * memcg_kmem_skip_account. So we enclose anything that might allocate memory
 * inside the following two functions.
 */
static inline void memcg_stop_kmem_account(void)
{
	VM_BUG_ON(!current->mm);
	current->memcg_kmem_skip_account++;
}

static inline void memcg_resume_kmem_account(void)
{
	VM_BUG_ON(!current->mm);
	current->memcg_kmem_skip_account--;
}

int __memcg_cleanup_cache_params(struct kmem_cache *s)
{
	struct kmem_cache *c;
	int i, failed = 0;

	mutex_lock(&memcg_slab_mutex);
	for_each_memcg_cache_index(i) {
		c = cache_from_memcg_idx(s, i);
		if (!c)
			continue;

		memcg_unregister_cache(c);

		if (cache_from_memcg_idx(s, i))
			failed++;
	}
	mutex_unlock(&memcg_slab_mutex);
	return failed;
}

static void memcg_unregister_all_caches(struct mem_cgroup *memcg)
{
	struct kmem_cache *cachep;
	struct memcg_cache_params *params, *tmp;

	if (!memcg_kmem_is_active(memcg))
		return;

	mutex_lock(&memcg_slab_mutex);
	list_for_each_entry_safe(params, tmp, &memcg->memcg_slab_caches, list) {
		cachep = memcg_params_to_cache(params);
		kmem_cache_shrink(cachep);
		if (atomic_read(&cachep->memcg_params->nr_pages) == 0)
			memcg_unregister_cache(cachep);
	}
	mutex_unlock(&memcg_slab_mutex);
}

struct memcg_register_cache_work {
=======
	ida_simple_remove(&memcg_cache_ida, id);
}

struct memcg_kmem_cache_create_work {
>>>>>>> v4.9.227
	struct mem_cgroup *memcg;
	struct kmem_cache *cachep;
	struct work_struct work;
};

<<<<<<< HEAD
static void memcg_register_cache_func(struct work_struct *w)
{
	struct memcg_register_cache_work *cw =
		container_of(w, struct memcg_register_cache_work, work);
	struct mem_cgroup *memcg = cw->memcg;
	struct kmem_cache *cachep = cw->cachep;

	mutex_lock(&memcg_slab_mutex);
	memcg_register_cache(memcg, cachep);
	mutex_unlock(&memcg_slab_mutex);
=======
static struct workqueue_struct *memcg_kmem_cache_create_wq;

static void memcg_kmem_cache_create_func(struct work_struct *w)
{
	struct memcg_kmem_cache_create_work *cw =
		container_of(w, struct memcg_kmem_cache_create_work, work);
	struct mem_cgroup *memcg = cw->memcg;
	struct kmem_cache *cachep = cw->cachep;

	memcg_create_kmem_cache(memcg, cachep);
>>>>>>> v4.9.227

	css_put(&memcg->css);
	kfree(cw);
}

/*
 * Enqueue the creation of a per-memcg kmem_cache.
 */
<<<<<<< HEAD
static void __memcg_schedule_register_cache(struct mem_cgroup *memcg,
					    struct kmem_cache *cachep)
{
	struct memcg_register_cache_work *cw;

	cw = kmalloc(sizeof(*cw), GFP_NOWAIT);
	if (cw == NULL) {
		css_put(&memcg->css);
		return;
	}

	cw->memcg = memcg;
	cw->cachep = cachep;

	INIT_WORK(&cw->work, memcg_register_cache_func);
	schedule_work(&cw->work);
}

static void memcg_schedule_register_cache(struct mem_cgroup *memcg,
					  struct kmem_cache *cachep)
=======
static void __memcg_schedule_kmem_cache_create(struct mem_cgroup *memcg,
					       struct kmem_cache *cachep)
{
	struct memcg_kmem_cache_create_work *cw;

	cw = kmalloc(sizeof(*cw), GFP_NOWAIT);
	if (!cw)
		return;

	css_get(&memcg->css);

	cw->memcg = memcg;
	cw->cachep = cachep;
	INIT_WORK(&cw->work, memcg_kmem_cache_create_func);

	queue_work(memcg_kmem_cache_create_wq, &cw->work);
}

static void memcg_schedule_kmem_cache_create(struct mem_cgroup *memcg,
					     struct kmem_cache *cachep)
>>>>>>> v4.9.227
{
	/*
	 * We need to stop accounting when we kmalloc, because if the
	 * corresponding kmalloc cache is not yet created, the first allocation
<<<<<<< HEAD
	 * in __memcg_schedule_register_cache will recurse.
=======
	 * in __memcg_schedule_kmem_cache_create will recurse.
>>>>>>> v4.9.227
	 *
	 * However, it is better to enclose the whole function. Depending on
	 * the debugging options enabled, INIT_WORK(), for instance, can
	 * trigger an allocation. This too, will make us recurse. Because at
	 * this point we can't allow ourselves back into memcg_kmem_get_cache,
	 * the safest choice is to do it like this, wrapping the whole function.
	 */
<<<<<<< HEAD
	memcg_stop_kmem_account();
	__memcg_schedule_register_cache(memcg, cachep);
	memcg_resume_kmem_account();
}

int __memcg_charge_slab(struct kmem_cache *cachep, gfp_t gfp, int order)
{
	int res;

	res = memcg_charge_kmem(cachep->memcg_params->memcg, gfp,
				PAGE_SIZE << order);
	if (!res)
		atomic_add(1 << order, &cachep->memcg_params->nr_pages);
	return res;
}

void __memcg_uncharge_slab(struct kmem_cache *cachep, int order)
{
	memcg_uncharge_kmem(cachep->memcg_params->memcg, PAGE_SIZE << order);
	atomic_sub(1 << order, &cachep->memcg_params->nr_pages);
}

/*
 * Return the kmem_cache we're supposed to use for a slab allocation.
 * We try to use the current memcg's version of the cache.
 *
 * If the cache does not exist yet, if we are the first user of it,
 * we either create it immediately, if possible, or create it asynchronously
 * in a workqueue.
 * In the latter case, we will let the current allocation go through with
 * the original cache.
 *
 * Can't be called in interrupt context or from kernel threads.
 * This function needs to be called with rcu_read_lock() held.
 */
struct kmem_cache *__memcg_kmem_get_cache(struct kmem_cache *cachep,
					  gfp_t gfp)
{
	struct mem_cgroup *memcg;
	struct kmem_cache *memcg_cachep;

	VM_BUG_ON(!cachep->memcg_params);
	VM_BUG_ON(!cachep->memcg_params->is_root_cache);

	if (!current->mm || current->memcg_kmem_skip_account)
		return cachep;

	rcu_read_lock();
	memcg = mem_cgroup_from_task(rcu_dereference(current->mm->owner));

	if (!memcg_kmem_is_active(memcg))
		goto out;

	memcg_cachep = cache_from_memcg_idx(cachep, memcg_cache_id(memcg));
	if (likely(memcg_cachep)) {
		cachep = memcg_cachep;
		goto out;
	}

	/* The corresponding put will be done in the workqueue. */
	if (!css_tryget_online(&memcg->css))
		goto out;
	rcu_read_unlock();
=======
	current->memcg_kmem_skip_account = 1;
	__memcg_schedule_kmem_cache_create(memcg, cachep);
	current->memcg_kmem_skip_account = 0;
}

static inline bool memcg_kmem_bypass(void)
{
	if (in_interrupt() || !current->mm || (current->flags & PF_KTHREAD))
		return true;
	return false;
}

/**
 * memcg_kmem_get_cache: select the correct per-memcg cache for allocation
 * @cachep: the original global kmem cache
 *
 * Return the kmem_cache we're supposed to use for a slab allocation.
 * We try to use the current memcg's version of the cache.
 *
 * If the cache does not exist yet, if we are the first user of it, we
 * create it asynchronously in a workqueue and let the current allocation
 * go through with the original cache.
 *
 * This function takes a reference to the cache it returns to assure it
 * won't get destroyed while we are working with it. Once the caller is
 * done with it, memcg_kmem_put_cache() must be called to release the
 * reference.
 */
struct kmem_cache *memcg_kmem_get_cache(struct kmem_cache *cachep)
{
	struct mem_cgroup *memcg;
	struct kmem_cache *memcg_cachep;
	int kmemcg_id;

	VM_BUG_ON(!is_root_cache(cachep));

	if (memcg_kmem_bypass())
		return cachep;

	if (current->memcg_kmem_skip_account)
		return cachep;

	memcg = get_mem_cgroup_from_mm(current->mm);
	kmemcg_id = READ_ONCE(memcg->kmemcg_id);
	if (kmemcg_id < 0)
		goto out;

	memcg_cachep = cache_from_memcg_idx(cachep, kmemcg_id);
	if (likely(memcg_cachep))
		return memcg_cachep;
>>>>>>> v4.9.227

	/*
	 * If we are in a safe context (can wait, and not in interrupt
	 * context), we could be be predictable and return right away.
	 * This would guarantee that the allocation being performed
	 * already belongs in the new cache.
	 *
	 * However, there are some clashes that can arrive from locking.
	 * For instance, because we acquire the slab_mutex while doing
	 * memcg_create_kmem_cache, this means no further allocation
	 * could happen with the slab_mutex held. So it's better to
	 * defer everything.
	 */
<<<<<<< HEAD
	memcg_schedule_register_cache(memcg, cachep);
	return cachep;
out:
	rcu_read_unlock();
	return cachep;
}

/*
 * We need to verify if the allocation against current->mm->owner's memcg is
 * possible for the given order. But the page is not allocated yet, so we'll
 * need a further commit step to do the final arrangements.
 *
 * It is possible for the task to switch cgroups in this mean time, so at
 * commit time, we can't rely on task conversion any longer.  We'll then use
 * the handle argument to return to the caller which cgroup we should commit
 * against. We could also return the memcg directly and avoid the pointer
 * passing, but a boolean return value gives better semantics considering
 * the compiled-out case as well.
 *
 * Returning true means the allocation is possible.
 */
bool
__memcg_kmem_newpage_charge(gfp_t gfp, struct mem_cgroup **_memcg, int order)
{
	struct mem_cgroup *memcg;
	int ret;

	*_memcg = NULL;

	/*
	 * Disabling accounting is only relevant for some specific memcg
	 * internal allocations. Therefore we would initially not have such
	 * check here, since direct calls to the page allocator that are
	 * accounted to kmemcg (alloc_kmem_pages and friends) only happen
	 * outside memcg core. We are mostly concerned with cache allocations,
	 * and by having this test at memcg_kmem_get_cache, we are already able
	 * to relay the allocation to the root cache and bypass the memcg cache
	 * altogether.
	 *
	 * There is one exception, though: the SLUB allocator does not create
	 * large order caches, but rather service large kmallocs directly from
	 * the page allocator. Therefore, the following sequence when backed by
	 * the SLUB allocator:
	 *
	 *	memcg_stop_kmem_account();
	 *	kmalloc(<large_number>)
	 *	memcg_resume_kmem_account();
	 *
	 * would effectively ignore the fact that we should skip accounting,
	 * since it will drive us directly to this function without passing
	 * through the cache selector memcg_kmem_get_cache. Such large
	 * allocations are extremely rare but can happen, for instance, for the
	 * cache arrays. We bring this test here.
	 */
	if (!current->mm || current->memcg_kmem_skip_account)
		return true;

	memcg = get_mem_cgroup_from_mm(current->mm);

	if (!memcg_kmem_is_active(memcg)) {
		css_put(&memcg->css);
		return true;
	}

	ret = memcg_charge_kmem(memcg, gfp, PAGE_SIZE << order);
	if (!ret)
		*_memcg = memcg;

	css_put(&memcg->css);
	return (ret == 0);
}

void __memcg_kmem_commit_charge(struct page *page, struct mem_cgroup *memcg,
			      int order)
{
	struct page_cgroup *pc;

	VM_BUG_ON(mem_cgroup_is_root(memcg));

	/* The page allocation failed. Revert */
	if (!page) {
		memcg_uncharge_kmem(memcg, PAGE_SIZE << order);
		return;
	}
	/*
	 * The page is freshly allocated and not visible to any
	 * outside callers yet.  Set up pc non-atomically.
	 */
	pc = lookup_page_cgroup(page);
	pc->mem_cgroup = memcg;
	pc->flags = PCG_USED;
}

void __memcg_kmem_uncharge_pages(struct page *page, int order)
{
	struct mem_cgroup *memcg = NULL;
	struct page_cgroup *pc;


	pc = lookup_page_cgroup(page);
	if (!PageCgroupUsed(pc))
		return;

	memcg = pc->mem_cgroup;
	pc->flags = 0;

	/*
	 * We trust that only if there is a memcg associated with the page, it
	 * is a valid allocation
	 */
=======
	memcg_schedule_kmem_cache_create(memcg, cachep);
out:
	css_put(&memcg->css);
	return cachep;
}

/**
 * memcg_kmem_put_cache: drop reference taken by memcg_kmem_get_cache
 * @cachep: the cache returned by memcg_kmem_get_cache
 */
void memcg_kmem_put_cache(struct kmem_cache *cachep)
{
	if (!is_root_cache(cachep))
		css_put(&cachep->memcg_params.memcg->css);
}

/**
 * memcg_kmem_charge: charge a kmem page
 * @page: page to charge
 * @gfp: reclaim mode
 * @order: allocation order
 * @memcg: memory cgroup to charge
 *
 * Returns 0 on success, an error code on failure.
 */
int memcg_kmem_charge_memcg(struct page *page, gfp_t gfp, int order,
			    struct mem_cgroup *memcg)
{
	unsigned int nr_pages = 1 << order;
	struct page_counter *counter;
	int ret;

	ret = try_charge(memcg, gfp, nr_pages);
	if (ret)
		return ret;

	if (!cgroup_subsys_on_dfl(memory_cgrp_subsys) &&
	    !page_counter_try_charge(&memcg->kmem, nr_pages, &counter)) {

		/*
		 * Enforce __GFP_NOFAIL allocation because callers are not
		 * prepared to see failures and likely do not have any failure
		 * handling code.
		 */
		if (gfp & __GFP_NOFAIL) {
			page_counter_charge(&memcg->kmem, nr_pages);
			return 0;
		}
		cancel_charge(memcg, nr_pages);
		return -ENOMEM;
	}

	page->mem_cgroup = memcg;

	return 0;
}

/**
 * memcg_kmem_charge: charge a kmem page to the current memory cgroup
 * @page: page to charge
 * @gfp: reclaim mode
 * @order: allocation order
 *
 * Returns 0 on success, an error code on failure.
 */
int memcg_kmem_charge(struct page *page, gfp_t gfp, int order)
{
	struct mem_cgroup *memcg;
	int ret = 0;

	if (memcg_kmem_bypass())
		return 0;

	memcg = get_mem_cgroup_from_mm(current->mm);
	if (!mem_cgroup_is_root(memcg)) {
		ret = memcg_kmem_charge_memcg(page, gfp, order, memcg);
		if (!ret)
			__SetPageKmemcg(page);
	}
	css_put(&memcg->css);
	return ret;
}
/**
 * memcg_kmem_uncharge: uncharge a kmem page
 * @page: page to uncharge
 * @order: allocation order
 */
void memcg_kmem_uncharge(struct page *page, int order)
{
	struct mem_cgroup *memcg = page->mem_cgroup;
	unsigned int nr_pages = 1 << order;

>>>>>>> v4.9.227
	if (!memcg)
		return;

	VM_BUG_ON_PAGE(mem_cgroup_is_root(memcg), page);
<<<<<<< HEAD
	memcg_uncharge_kmem(memcg, PAGE_SIZE << order);
}
#else
static inline void memcg_unregister_all_caches(struct mem_cgroup *memcg)
{
}
#endif /* CONFIG_MEMCG_KMEM */
=======

	if (!cgroup_subsys_on_dfl(memory_cgrp_subsys))
		page_counter_uncharge(&memcg->kmem, nr_pages);

	page_counter_uncharge(&memcg->memory, nr_pages);
	if (do_memsw_account())
		page_counter_uncharge(&memcg->memsw, nr_pages);

	page->mem_cgroup = NULL;

	/* slab pages do not have PageKmemcg flag set */
	if (PageKmemcg(page))
		__ClearPageKmemcg(page);

	css_put_many(&memcg->css, nr_pages);
}
#endif /* !CONFIG_SLOB */
>>>>>>> v4.9.227

#ifdef CONFIG_TRANSPARENT_HUGEPAGE

/*
 * Because tail pages are not marked as "used", set it. We're under
<<<<<<< HEAD
 * zone->lru_lock, 'splitting on pmd' and compound_lock.
 * charge/uncharge will be never happen and move_account() is done under
 * compound_lock(), so we don't have to take care of races.
 */
void mem_cgroup_split_huge_fixup(struct page *head)
{
	struct page_cgroup *head_pc = lookup_page_cgroup(head);
	struct page_cgroup *pc;
	struct mem_cgroup *memcg;
=======
 * zone_lru_lock and migration entries setup in all page mappings.
 */
void mem_cgroup_split_huge_fixup(struct page *head)
{
>>>>>>> v4.9.227
	int i;

	if (mem_cgroup_disabled())
		return;

<<<<<<< HEAD
	memcg = head_pc->mem_cgroup;
	for (i = 1; i < HPAGE_PMD_NR; i++) {
		pc = head_pc + i;
		pc->mem_cgroup = memcg;
		pc->flags = head_pc->flags;
	}
	__this_cpu_sub(memcg->stat->count[MEM_CGROUP_STAT_RSS_HUGE],
=======
	for (i = 1; i < HPAGE_PMD_NR; i++)
		head[i].mem_cgroup = head->mem_cgroup;

	__this_cpu_sub(head->mem_cgroup->stat->count[MEM_CGROUP_STAT_RSS_HUGE],
>>>>>>> v4.9.227
		       HPAGE_PMD_NR);
}
#endif /* CONFIG_TRANSPARENT_HUGEPAGE */

<<<<<<< HEAD
/**
 * mem_cgroup_move_account - move account of the page
 * @page: the page
 * @nr_pages: number of regular pages (>1 for huge pages)
 * @pc:	page_cgroup of the page.
 * @from: mem_cgroup which the page is moved from.
 * @to:	mem_cgroup which the page is moved to. @from != @to.
 *
 * The caller must confirm following.
 * - page is not on LRU (isolate_page() is useful.)
 * - compound_lock is held when nr_pages > 1
 *
 * This function doesn't do "charge" to new cgroup and doesn't do "uncharge"
 * from old cgroup.
 */
static int mem_cgroup_move_account(struct page *page,
				   unsigned int nr_pages,
				   struct page_cgroup *pc,
				   struct mem_cgroup *from,
				   struct mem_cgroup *to)
{
	unsigned long flags;
	int ret;

	VM_BUG_ON(from == to);
	VM_BUG_ON_PAGE(PageLRU(page), page);
	/*
	 * The page is isolated from LRU. So, collapse function
	 * will not handle this page. But page splitting can happen.
	 * Do this check under compound_page_lock(). The caller should
	 * hold it.
	 */
	ret = -EBUSY;
	if (nr_pages > 1 && !PageTransHuge(page))
		goto out;

	/*
	 * Prevent mem_cgroup_migrate() from looking at pc->mem_cgroup
	 * of its source page while we change it: page migration takes
	 * both pages off the LRU, but page cache replacement doesn't.
	 */
	if (!trylock_page(page))
		goto out;

	ret = -EINVAL;
	if (!PageCgroupUsed(pc) || pc->mem_cgroup != from)
		goto out_unlock;

	move_lock_mem_cgroup(from, &flags);

	if (!PageAnon(page) && page_mapped(page)) {
		__this_cpu_sub(from->stat->count[MEM_CGROUP_STAT_FILE_MAPPED],
			       nr_pages);
		__this_cpu_add(to->stat->count[MEM_CGROUP_STAT_FILE_MAPPED],
			       nr_pages);
	}

	if (PageWriteback(page)) {
		__this_cpu_sub(from->stat->count[MEM_CGROUP_STAT_WRITEBACK],
			       nr_pages);
		__this_cpu_add(to->stat->count[MEM_CGROUP_STAT_WRITEBACK],
			       nr_pages);
	}

	/*
	 * It is safe to change pc->mem_cgroup here because the page
	 * is referenced, charged, and isolated - we can't race with
	 * uncharging, charging, migration, or LRU putback.
	 */

	/* caller should have done css_get */
	pc->mem_cgroup = to;
	move_unlock_mem_cgroup(from, &flags);
	ret = 0;

	local_irq_disable();
	mem_cgroup_charge_statistics(to, page, nr_pages);
	memcg_check_events(to, page);
	mem_cgroup_charge_statistics(from, page, -nr_pages);
	memcg_check_events(from, page);
	local_irq_enable();
out_unlock:
	unlock_page(page);
out:
	return ret;
}

/**
 * mem_cgroup_move_parent - moves page to the parent group
 * @page: the page to move
 * @pc: page_cgroup of the page
 * @child: page's cgroup
 *
 * move charges to its parent or the root cgroup if the group has no
 * parent (aka use_hierarchy==0).
 * Although this might fail (get_page_unless_zero, isolate_lru_page or
 * mem_cgroup_move_account fails) the failure is always temporary and
 * it signals a race with a page removal/uncharge or migration. In the
 * first case the page is on the way out and it will vanish from the LRU
 * on the next attempt and the call should be retried later.
 * Isolation from the LRU fails only if page has been isolated from
 * the LRU since we looked at it and that usually means either global
 * reclaim or migration going on. The page will either get back to the
 * LRU or vanish.
 * Finaly mem_cgroup_move_account fails only if the page got uncharged
 * (!PageCgroupUsed) or moved to a different group. The page will
 * disappear in the next attempt.
 */
static int mem_cgroup_move_parent(struct page *page,
				  struct page_cgroup *pc,
				  struct mem_cgroup *child)
{
	struct mem_cgroup *parent;
	unsigned int nr_pages;
	unsigned long uninitialized_var(flags);
	int ret;

	VM_BUG_ON(mem_cgroup_is_root(child));

	ret = -EBUSY;
	if (!get_page_unless_zero(page))
		goto out;
	if (isolate_lru_page(page))
		goto put;

	nr_pages = hpage_nr_pages(page);

	parent = parent_mem_cgroup(child);
	/*
	 * If no parent, move charges to root cgroup.
	 */
	if (!parent)
		parent = root_mem_cgroup;

	if (nr_pages > 1) {
		VM_BUG_ON_PAGE(!PageTransHuge(page), page);
		flags = compound_lock_irqsave(page);
	}

	ret = mem_cgroup_move_account(page, nr_pages,
				pc, child, parent);
	if (!ret)
		__mem_cgroup_cancel_local_charge(child, nr_pages);

	if (nr_pages > 1)
		compound_unlock_irqrestore(page, flags);
	putback_lru_page(page);
put:
	put_page(page);
out:
	return ret;
}

=======
>>>>>>> v4.9.227
#ifdef CONFIG_MEMCG_SWAP
static void mem_cgroup_swap_statistics(struct mem_cgroup *memcg,
					 bool charge)
{
	int val = (charge) ? 1 : -1;
	this_cpu_add(memcg->stat->count[MEM_CGROUP_STAT_SWAP], val);
}

/**
 * mem_cgroup_move_swap_account - move swap charge and swap_cgroup's record.
 * @entry: swap entry to be moved
 * @from:  mem_cgroup which the entry is moved from
 * @to:  mem_cgroup which the entry is moved to
 *
 * It succeeds only when the swap_cgroup's record for this entry is the same
 * as the mem_cgroup's id of @from.
 *
 * Returns 0 on success, -EINVAL on failure.
 *
<<<<<<< HEAD
 * The caller must have charged to @to, IOW, called res_counter_charge() about
=======
 * The caller must have charged to @to, IOW, called page_counter_charge() about
>>>>>>> v4.9.227
 * both res and memsw, and called css_get().
 */
static int mem_cgroup_move_swap_account(swp_entry_t entry,
				struct mem_cgroup *from, struct mem_cgroup *to)
{
	unsigned short old_id, new_id;

	old_id = mem_cgroup_id(from);
	new_id = mem_cgroup_id(to);

	if (swap_cgroup_cmpxchg(entry, old_id, new_id) == old_id) {
		mem_cgroup_swap_statistics(from, false);
		mem_cgroup_swap_statistics(to, true);
<<<<<<< HEAD
		/*
		 * This function is only called from task migration context now.
		 * It postpones res_counter and refcount handling till the end
		 * of task migration(mem_cgroup_clear_mc()) for performance
		 * improvement. But we cannot postpone css_get(to)  because if
		 * the process that has been moved to @to does swap-in, the
		 * refcount of @to might be decreased to 0.
		 *
		 * We are in attach() phase, so the cgroup is guaranteed to be
		 * alive, so we can just call css_get().
		 */
		css_get(&to->css);
=======
>>>>>>> v4.9.227
		return 0;
	}
	return -EINVAL;
}
#else
static inline int mem_cgroup_move_swap_account(swp_entry_t entry,
				struct mem_cgroup *from, struct mem_cgroup *to)
{
	return -EINVAL;
}
#endif

<<<<<<< HEAD
#ifdef CONFIG_DEBUG_VM
static struct page_cgroup *lookup_page_cgroup_used(struct page *page)
{
	struct page_cgroup *pc;

	pc = lookup_page_cgroup(page);
	/*
	 * Can be NULL while feeding pages into the page allocator for
	 * the first time, i.e. during boot or memory hotplug;
	 * or when mem_cgroup_disabled().
	 */
	if (likely(pc) && PageCgroupUsed(pc))
		return pc;
	return NULL;
}

bool mem_cgroup_bad_page_check(struct page *page)
{
	if (mem_cgroup_disabled())
		return false;

	return lookup_page_cgroup_used(page) != NULL;
}

void mem_cgroup_print_bad_page(struct page *page)
{
	struct page_cgroup *pc;

	pc = lookup_page_cgroup_used(page);
	if (pc) {
		pr_alert("pc:%p pc->flags:%lx pc->mem_cgroup:%p\n",
			 pc, pc->flags, pc->mem_cgroup);
	}
}
#endif

static int mem_cgroup_resize_limit(struct mem_cgroup *memcg,
				unsigned long long val)
{
	int retry_count;
	int ret = 0;
	int children = mem_cgroup_count_children(memcg);
	u64 curusage, oldusage;
	int enlarge;
=======
static DEFINE_MUTEX(memcg_limit_mutex);

static int mem_cgroup_resize_limit(struct mem_cgroup *memcg,
				   unsigned long limit)
{
	unsigned long curusage;
	unsigned long oldusage;
	bool enlarge = false;
	int retry_count;
	int ret;
>>>>>>> v4.9.227

	/*
	 * For keeping hierarchical_reclaim simple, how long we should retry
	 * is depends on callers. We set our retry-count to be function
	 * of # of children which we should visit in this loop.
	 */
<<<<<<< HEAD
	retry_count = MEM_CGROUP_RECLAIM_RETRIES * children;

	oldusage = res_counter_read_u64(&memcg->res, RES_USAGE);

	enlarge = 0;
	while (retry_count) {
=======
	retry_count = MEM_CGROUP_RECLAIM_RETRIES *
		      mem_cgroup_count_children(memcg);

	oldusage = page_counter_read(&memcg->memory);

	do {
>>>>>>> v4.9.227
		if (signal_pending(current)) {
			ret = -EINTR;
			break;
		}
<<<<<<< HEAD
		/*
		 * Rather than hide all in some function, I do this in
		 * open coded manner. You see what this really does.
		 * We have to guarantee memcg->res.limit <= memcg->memsw.limit.
		 */
		mutex_lock(&set_limit_mutex);
		if (res_counter_read_u64(&memcg->memsw, RES_LIMIT) < val) {
			ret = -EINVAL;
			mutex_unlock(&set_limit_mutex);
			break;
		}

		if (res_counter_read_u64(&memcg->res, RES_LIMIT) < val)
			enlarge = 1;

		ret = res_counter_set_limit(&memcg->res, val);
		mutex_unlock(&set_limit_mutex);
=======

		mutex_lock(&memcg_limit_mutex);
		if (limit > memcg->memsw.limit) {
			mutex_unlock(&memcg_limit_mutex);
			ret = -EINVAL;
			break;
		}
		if (limit > memcg->memory.limit)
			enlarge = true;
		ret = page_counter_limit(&memcg->memory, limit);
		mutex_unlock(&memcg_limit_mutex);
>>>>>>> v4.9.227

		if (!ret)
			break;

		try_to_free_mem_cgroup_pages(memcg, 1, GFP_KERNEL, true);

<<<<<<< HEAD
		curusage = res_counter_read_u64(&memcg->res, RES_USAGE);
=======
		curusage = page_counter_read(&memcg->memory);
>>>>>>> v4.9.227
		/* Usage is reduced ? */
		if (curusage >= oldusage)
			retry_count--;
		else
			oldusage = curusage;
<<<<<<< HEAD
	}
=======
	} while (retry_count);

>>>>>>> v4.9.227
	if (!ret && enlarge)
		memcg_oom_recover(memcg);

	return ret;
}

static int mem_cgroup_resize_memsw_limit(struct mem_cgroup *memcg,
<<<<<<< HEAD
					unsigned long long val)
{
	int retry_count;
	u64 oldusage, curusage;
	int children = mem_cgroup_count_children(memcg);
	int ret = -EBUSY;
	int enlarge = 0;

	/* see mem_cgroup_resize_res_limit */
	retry_count = children * MEM_CGROUP_RECLAIM_RETRIES;
	oldusage = res_counter_read_u64(&memcg->memsw, RES_USAGE);
	while (retry_count) {
=======
					 unsigned long limit)
{
	unsigned long curusage;
	unsigned long oldusage;
	bool enlarge = false;
	int retry_count;
	int ret;

	/* see mem_cgroup_resize_res_limit */
	retry_count = MEM_CGROUP_RECLAIM_RETRIES *
		      mem_cgroup_count_children(memcg);

	oldusage = page_counter_read(&memcg->memsw);

	do {
>>>>>>> v4.9.227
		if (signal_pending(current)) {
			ret = -EINTR;
			break;
		}
<<<<<<< HEAD
		/*
		 * Rather than hide all in some function, I do this in
		 * open coded manner. You see what this really does.
		 * We have to guarantee memcg->res.limit <= memcg->memsw.limit.
		 */
		mutex_lock(&set_limit_mutex);
		if (res_counter_read_u64(&memcg->res, RES_LIMIT) > val) {
			ret = -EINVAL;
			mutex_unlock(&set_limit_mutex);
			break;
		}
		if (res_counter_read_u64(&memcg->memsw, RES_LIMIT) < val)
			enlarge = 1;
		ret = res_counter_set_limit(&memcg->memsw, val);
		mutex_unlock(&set_limit_mutex);
=======

		mutex_lock(&memcg_limit_mutex);
		if (limit < memcg->memory.limit) {
			mutex_unlock(&memcg_limit_mutex);
			ret = -EINVAL;
			break;
		}
		if (limit > memcg->memsw.limit)
			enlarge = true;
		ret = page_counter_limit(&memcg->memsw, limit);
		mutex_unlock(&memcg_limit_mutex);
>>>>>>> v4.9.227

		if (!ret)
			break;

		try_to_free_mem_cgroup_pages(memcg, 1, GFP_KERNEL, false);

<<<<<<< HEAD
		curusage = res_counter_read_u64(&memcg->memsw, RES_USAGE);
=======
		curusage = page_counter_read(&memcg->memsw);
>>>>>>> v4.9.227
		/* Usage is reduced ? */
		if (curusage >= oldusage)
			retry_count--;
		else
			oldusage = curusage;
<<<<<<< HEAD
	}
	if (!ret && enlarge)
		memcg_oom_recover(memcg);
	return ret;
}

unsigned long mem_cgroup_soft_limit_reclaim(struct zone *zone, int order,
=======
	} while (retry_count);

	if (!ret && enlarge)
		memcg_oom_recover(memcg);

	return ret;
}

unsigned long mem_cgroup_soft_limit_reclaim(pg_data_t *pgdat, int order,
>>>>>>> v4.9.227
					    gfp_t gfp_mask,
					    unsigned long *total_scanned)
{
	unsigned long nr_reclaimed = 0;
<<<<<<< HEAD
	struct mem_cgroup_per_zone *mz, *next_mz = NULL;
	unsigned long reclaimed;
	int loop = 0;
	struct mem_cgroup_tree_per_zone *mctz;
	unsigned long long excess;
=======
	struct mem_cgroup_per_node *mz, *next_mz = NULL;
	unsigned long reclaimed;
	int loop = 0;
	struct mem_cgroup_tree_per_node *mctz;
	unsigned long excess;
>>>>>>> v4.9.227
	unsigned long nr_scanned;

	if (order > 0)
		return 0;

<<<<<<< HEAD
	mctz = soft_limit_tree_node_zone(zone_to_nid(zone), zone_idx(zone));
=======
	mctz = soft_limit_tree_node(pgdat->node_id);

	/*
	 * Do not even bother to check the largest node if the root
	 * is empty. Do it lockless to prevent lock bouncing. Races
	 * are acceptable as soft limit is best effort anyway.
	 */
	if (!mctz || RB_EMPTY_ROOT(&mctz->rb_root))
		return 0;

>>>>>>> v4.9.227
	/*
	 * This loop can run a while, specially if mem_cgroup's continuously
	 * keep exceeding their soft limit and putting the system under
	 * pressure
	 */
	do {
		if (next_mz)
			mz = next_mz;
		else
			mz = mem_cgroup_largest_soft_limit_node(mctz);
		if (!mz)
			break;

		nr_scanned = 0;
<<<<<<< HEAD
		reclaimed = mem_cgroup_soft_reclaim(mz->memcg, zone,
=======
		reclaimed = mem_cgroup_soft_reclaim(mz->memcg, pgdat,
>>>>>>> v4.9.227
						    gfp_mask, &nr_scanned);
		nr_reclaimed += reclaimed;
		*total_scanned += nr_scanned;
		spin_lock_irq(&mctz->lock);
<<<<<<< HEAD
=======
		__mem_cgroup_remove_exceeded(mz, mctz);
>>>>>>> v4.9.227

		/*
		 * If we failed to reclaim anything from this memory cgroup
		 * it is time to move on to the next cgroup
		 */
		next_mz = NULL;
<<<<<<< HEAD
		if (!reclaimed) {
			do {
				/*
				 * Loop until we find yet another one.
				 *
				 * By the time we get the soft_limit lock
				 * again, someone might have aded the
				 * group back on the RB tree. Iterate to
				 * make sure we get a different mem.
				 * mem_cgroup_largest_soft_limit_node returns
				 * NULL if no other cgroup is present on
				 * the tree
				 */
				next_mz =
				__mem_cgroup_largest_soft_limit_node(mctz);
				if (next_mz == mz)
					css_put(&next_mz->memcg->css);
				else /* next_mz == NULL or other memcg */
					break;
			} while (1);
		}
		__mem_cgroup_remove_exceeded(mz, mctz);
		excess = res_counter_soft_limit_excess(&mz->memcg->res);
=======
		if (!reclaimed)
			next_mz = __mem_cgroup_largest_soft_limit_node(mctz);

		excess = soft_limit_excess(mz->memcg);
>>>>>>> v4.9.227
		/*
		 * One school of thought says that we should not add
		 * back the node to the tree if reclaim returns 0.
		 * But our reclaim could return 0, simply because due
		 * to priority we are exposing a smaller subset of
		 * memory to reclaim from. Consider this as a longer
		 * term TODO.
		 */
		/* If excess == 0, no tree ops */
		__mem_cgroup_insert_exceeded(mz, mctz, excess);
		spin_unlock_irq(&mctz->lock);
		css_put(&mz->memcg->css);
		loop++;
		/*
		 * Could not reclaim anything and there are no more
		 * mem cgroups to try or we seem to be looping without
		 * reclaiming anything.
		 */
		if (!nr_reclaimed &&
			(next_mz == NULL ||
			loop > MEM_CGROUP_MAX_SOFT_LIMIT_RECLAIM_LOOPS))
			break;
	} while (!nr_reclaimed);
	if (next_mz)
		css_put(&next_mz->memcg->css);
	return nr_reclaimed;
}

<<<<<<< HEAD
/**
 * mem_cgroup_force_empty_list - clears LRU of a group
 * @memcg: group to clear
 * @node: NUMA node
 * @zid: zone id
 * @lru: lru to to clear
 *
 * Traverse a specified page_cgroup list and try to drop them all.  This doesn't
 * reclaim the pages page themselves - pages are moved to the parent (or root)
 * group.
 */
static void mem_cgroup_force_empty_list(struct mem_cgroup *memcg,
				int node, int zid, enum lru_list lru)
{
	struct lruvec *lruvec;
	unsigned long flags;
	struct list_head *list;
	struct page *busy;
	struct zone *zone;

	zone = &NODE_DATA(node)->node_zones[zid];
	lruvec = mem_cgroup_zone_lruvec(zone, memcg);
	list = &lruvec->lists[lru];

	busy = NULL;
	do {
		struct page_cgroup *pc;
		struct page *page;

		spin_lock_irqsave(&zone->lru_lock, flags);
		if (list_empty(list)) {
			spin_unlock_irqrestore(&zone->lru_lock, flags);
			break;
		}
		page = list_entry(list->prev, struct page, lru);
		if (busy == page) {
			list_move(&page->lru, list);
			busy = NULL;
			spin_unlock_irqrestore(&zone->lru_lock, flags);
			continue;
		}
		spin_unlock_irqrestore(&zone->lru_lock, flags);

		pc = lookup_page_cgroup(page);

		if (mem_cgroup_move_parent(page, pc, memcg)) {
			/* found lock contention or "pc" is obsolete. */
			busy = page;
		} else
			busy = NULL;
		cond_resched();
	} while (!list_empty(list));
}

/*
 * make mem_cgroup's charge to be 0 if there is no task by moving
 * all the charges and pages to the parent.
 * This enables deleting this mem_cgroup.
 *
 * Caller is responsible for holding css reference on the memcg.
 */
static void mem_cgroup_reparent_charges(struct mem_cgroup *memcg)
{
	int node, zid;
	u64 usage;

	do {
		/* This is for making all *used* pages to be on LRU. */
		lru_add_drain_all();
		drain_all_stock_sync(memcg);
		mem_cgroup_start_move(memcg);
		for_each_node_state(node, N_MEMORY) {
			for (zid = 0; zid < MAX_NR_ZONES; zid++) {
				enum lru_list lru;
				for_each_lru(lru) {
					mem_cgroup_force_empty_list(memcg,
							node, zid, lru);
				}
			}
		}
		mem_cgroup_end_move(memcg);
		memcg_oom_recover(memcg);
		cond_resched();

		/*
		 * Kernel memory may not necessarily be trackable to a specific
		 * process. So they are not migrated, and therefore we can't
		 * expect their value to drop to 0 here.
		 * Having res filled up with kmem only is enough.
		 *
		 * This is a safety check because mem_cgroup_force_empty_list
		 * could have raced with mem_cgroup_replace_page_cache callers
		 * so the lru seemed empty but the page could have been added
		 * right after the check. RES_USAGE should be safe as we always
		 * charge before adding to the LRU.
		 */
		usage = res_counter_read_u64(&memcg->res, RES_USAGE) -
			res_counter_read_u64(&memcg->kmem, RES_USAGE);
	} while (usage > 0);
}

=======
>>>>>>> v4.9.227
/*
 * Test whether @memcg has children, dead or alive.  Note that this
 * function doesn't care whether @memcg has use_hierarchy enabled and
 * returns %true if there are child csses according to the cgroup
 * hierarchy.  Testing use_hierarchy is the caller's responsiblity.
 */
static inline bool memcg_has_children(struct mem_cgroup *memcg)
{
	bool ret;

<<<<<<< HEAD
	/*
	 * The lock does not prevent addition or deletion of children, but
	 * it prevents a new child from being initialized based on this
	 * parent in css_online(), so it's enough to decide whether
	 * hierarchically inherited attributes can still be changed or not.
	 */
	lockdep_assert_held(&memcg_create_mutex);

=======
>>>>>>> v4.9.227
	rcu_read_lock();
	ret = css_next_child(NULL, &memcg->css);
	rcu_read_unlock();
	return ret;
}

/*
<<<<<<< HEAD
 * Reclaims as many pages from the given memcg as possible and moves
 * the rest to the parent.
=======
 * Reclaims as many pages from the given memcg as possible.
>>>>>>> v4.9.227
 *
 * Caller is responsible for holding css reference for memcg.
 */
static int mem_cgroup_force_empty(struct mem_cgroup *memcg)
{
	int nr_retries = MEM_CGROUP_RECLAIM_RETRIES;

	/* we call try-to-free pages for make this cgroup empty */
	lru_add_drain_all();
	/* try to free all pages in this cgroup */
<<<<<<< HEAD
	while (nr_retries && res_counter_read_u64(&memcg->res, RES_USAGE) > 0) {
=======
	while (nr_retries && page_counter_read(&memcg->memory)) {
>>>>>>> v4.9.227
		int progress;

		if (signal_pending(current))
			return -EINTR;

		progress = try_to_free_mem_cgroup_pages(memcg, 1,
							GFP_KERNEL, true);
		if (!progress) {
			nr_retries--;
			/* maybe some writeback is necessary */
			congestion_wait(BLK_RW_ASYNC, HZ/10);
		}

	}

	return 0;
}

static ssize_t mem_cgroup_force_empty_write(struct kernfs_open_file *of,
					    char *buf, size_t nbytes,
					    loff_t off)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(of_css(of));

	if (mem_cgroup_is_root(memcg))
		return -EINVAL;
	return mem_cgroup_force_empty(memcg) ?: nbytes;
}

static u64 mem_cgroup_hierarchy_read(struct cgroup_subsys_state *css,
				     struct cftype *cft)
{
	return mem_cgroup_from_css(css)->use_hierarchy;
}

static int mem_cgroup_hierarchy_write(struct cgroup_subsys_state *css,
				      struct cftype *cft, u64 val)
{
	int retval = 0;
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);
	struct mem_cgroup *parent_memcg = mem_cgroup_from_css(memcg->css.parent);

<<<<<<< HEAD
	mutex_lock(&memcg_create_mutex);

	if (memcg->use_hierarchy == val)
		goto out;
=======
	if (memcg->use_hierarchy == val)
		return 0;
>>>>>>> v4.9.227

	/*
	 * If parent's use_hierarchy is set, we can't make any modifications
	 * in the child subtrees. If it is unset, then the change can
	 * occur, provided the current cgroup has no children.
	 *
	 * For the root cgroup, parent_mem is NULL, we allow value to be
	 * set if there are no children.
	 */
	if ((!parent_memcg || !parent_memcg->use_hierarchy) &&
				(val == 1 || val == 0)) {
		if (!memcg_has_children(memcg))
			memcg->use_hierarchy = val;
		else
			retval = -EBUSY;
	} else
		retval = -EINVAL;

<<<<<<< HEAD
out:
	mutex_unlock(&memcg_create_mutex);

	return retval;
}

static unsigned long mem_cgroup_recursive_stat(struct mem_cgroup *memcg,
					       enum mem_cgroup_stat_index idx)
{
	struct mem_cgroup *iter;
	long val = 0;

	/* Per-cpu values can be negative, use a signed accumulator */
	for_each_mem_cgroup_tree(iter, memcg)
		val += mem_cgroup_read_stat(iter, idx);

	if (val < 0) /* race ? */
		val = 0;
	return val;
}

static inline u64 mem_cgroup_usage(struct mem_cgroup *memcg, bool swap)
{
	u64 val;

	if (!mem_cgroup_is_root(memcg)) {
		if (!swap)
			return res_counter_read_u64(&memcg->res, RES_USAGE);
		else
			return res_counter_read_u64(&memcg->memsw, RES_USAGE);
	}

	/*
	 * Transparent hugepages are still accounted for in MEM_CGROUP_STAT_RSS
	 * as well as in MEM_CGROUP_STAT_RSS_HUGE.
	 */
	val = mem_cgroup_recursive_stat(memcg, MEM_CGROUP_STAT_CACHE);
	val += mem_cgroup_recursive_stat(memcg, MEM_CGROUP_STAT_RSS);

	if (swap)
		val += mem_cgroup_recursive_stat(memcg, MEM_CGROUP_STAT_SWAP);

	return val << PAGE_SHIFT;
}

=======
	return retval;
}

static void tree_stat(struct mem_cgroup *memcg, unsigned long *stat)
{
	struct mem_cgroup *iter;
	int i;

	memset(stat, 0, sizeof(*stat) * MEMCG_NR_STAT);

	for_each_mem_cgroup_tree(iter, memcg) {
		for (i = 0; i < MEMCG_NR_STAT; i++)
			stat[i] += mem_cgroup_read_stat(iter, i);
	}
}

static void tree_events(struct mem_cgroup *memcg, unsigned long *events)
{
	struct mem_cgroup *iter;
	int i;

	memset(events, 0, sizeof(*events) * MEMCG_NR_EVENTS);

	for_each_mem_cgroup_tree(iter, memcg) {
		for (i = 0; i < MEMCG_NR_EVENTS; i++)
			events[i] += mem_cgroup_read_events(iter, i);
	}
}

static unsigned long mem_cgroup_usage(struct mem_cgroup *memcg, bool swap)
{
	unsigned long val = 0;

	if (mem_cgroup_is_root(memcg)) {
		struct mem_cgroup *iter;

		for_each_mem_cgroup_tree(iter, memcg) {
			val += mem_cgroup_read_stat(iter,
					MEM_CGROUP_STAT_CACHE);
			val += mem_cgroup_read_stat(iter,
					MEM_CGROUP_STAT_RSS);
			if (swap)
				val += mem_cgroup_read_stat(iter,
						MEM_CGROUP_STAT_SWAP);
		}
	} else {
		if (!swap)
			val = page_counter_read(&memcg->memory);
		else
			val = page_counter_read(&memcg->memsw);
	}
	return val;
}

enum {
	RES_USAGE,
	RES_LIMIT,
	RES_MAX_USAGE,
	RES_FAILCNT,
	RES_SOFT_LIMIT,
};
>>>>>>> v4.9.227

static u64 mem_cgroup_read_u64(struct cgroup_subsys_state *css,
			       struct cftype *cft)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);
<<<<<<< HEAD
	enum res_type type = MEMFILE_TYPE(cft->private);
	int name = MEMFILE_ATTR(cft->private);

	switch (type) {
	case _MEM:
		if (name == RES_USAGE)
			return mem_cgroup_usage(memcg, false);
		return res_counter_read_u64(&memcg->res, name);
	case _MEMSWAP:
		if (name == RES_USAGE)
			return mem_cgroup_usage(memcg, true);
		return res_counter_read_u64(&memcg->memsw, name);
	case _KMEM:
		return res_counter_read_u64(&memcg->kmem, name);
		break;
=======
	struct page_counter *counter;

	switch (MEMFILE_TYPE(cft->private)) {
	case _MEM:
		counter = &memcg->memory;
		break;
	case _MEMSWAP:
		counter = &memcg->memsw;
		break;
	case _KMEM:
		counter = &memcg->kmem;
		break;
	case _TCP:
		counter = &memcg->tcpmem;
		break;
	default:
		BUG();
	}

	switch (MEMFILE_ATTR(cft->private)) {
	case RES_USAGE:
		if (counter == &memcg->memory)
			return (u64)mem_cgroup_usage(memcg, false) * PAGE_SIZE;
		if (counter == &memcg->memsw)
			return (u64)mem_cgroup_usage(memcg, true) * PAGE_SIZE;
		return (u64)page_counter_read(counter) * PAGE_SIZE;
	case RES_LIMIT:
		return (u64)counter->limit * PAGE_SIZE;
	case RES_MAX_USAGE:
		return (u64)counter->watermark * PAGE_SIZE;
	case RES_FAILCNT:
		return counter->failcnt;
	case RES_SOFT_LIMIT:
		return (u64)memcg->soft_limit * PAGE_SIZE;
>>>>>>> v4.9.227
	default:
		BUG();
	}
}

<<<<<<< HEAD
#ifdef CONFIG_MEMCG_KMEM
/* should be called with activate_kmem_mutex held */
static int __memcg_activate_kmem(struct mem_cgroup *memcg,
				 unsigned long long limit)
{
	int err = 0;
	int memcg_id;

	if (memcg_kmem_is_active(memcg))
		return 0;

	/*
	 * We are going to allocate memory for data shared by all memory
	 * cgroups so let's stop accounting here.
	 */
	memcg_stop_kmem_account();

	/*
	 * For simplicity, we won't allow this to be disabled.  It also can't
	 * be changed if the cgroup has children already, or if tasks had
	 * already joined.
	 *
	 * If tasks join before we set the limit, a person looking at
	 * kmem.usage_in_bytes will have no way to determine when it took
	 * place, which makes the value quite meaningless.
	 *
	 * After it first became limited, changes in the value of the limit are
	 * of course permitted.
	 */
	mutex_lock(&memcg_create_mutex);
	if (cgroup_has_tasks(memcg->css.cgroup) ||
	    (memcg->use_hierarchy && memcg_has_children(memcg)))
		err = -EBUSY;
	mutex_unlock(&memcg_create_mutex);
	if (err)
		goto out;

	memcg_id = memcg_alloc_cache_id();
	if (memcg_id < 0) {
		err = memcg_id;
		goto out;
	}

	memcg->kmemcg_id = memcg_id;
	INIT_LIST_HEAD(&memcg->memcg_slab_caches);

	/*
	 * We couldn't have accounted to this cgroup, because it hasn't got the
	 * active bit set yet, so this should succeed.
	 */
	err = res_counter_set_limit(&memcg->kmem, limit);
	VM_BUG_ON(err);

	static_key_slow_inc(&memcg_kmem_enabled_key);
	/*
	 * Setting the active bit after enabling static branching will
	 * guarantee no one starts accounting before all call sites are
	 * patched.
	 */
	memcg_kmem_set_active(memcg);
out:
	memcg_resume_kmem_account();
	return err;
}

static int memcg_activate_kmem(struct mem_cgroup *memcg,
			       unsigned long long limit)
{
	int ret;

	mutex_lock(&activate_kmem_mutex);
	ret = __memcg_activate_kmem(memcg, limit);
	mutex_unlock(&activate_kmem_mutex);
	return ret;
}

static int memcg_update_kmem_limit(struct mem_cgroup *memcg,
				   unsigned long long val)
{
	int ret;

	if (!memcg_kmem_is_active(memcg))
		ret = memcg_activate_kmem(memcg, val);
	else
		ret = res_counter_set_limit(&memcg->kmem, val);
	return ret;
}

static int memcg_propagate_kmem(struct mem_cgroup *memcg)
{
	int ret = 0;
	struct mem_cgroup *parent = parent_mem_cgroup(memcg);

	if (!parent)
		return 0;

	mutex_lock(&activate_kmem_mutex);
	/*
	 * If the parent cgroup is not kmem-active now, it cannot be activated
	 * after this point, because it has at least one child already.
	 */
	if (memcg_kmem_is_active(parent))
		ret = __memcg_activate_kmem(memcg, RES_COUNTER_MAX);
	mutex_unlock(&activate_kmem_mutex);
	return ret;
}
#else
static int memcg_update_kmem_limit(struct mem_cgroup *memcg,
				   unsigned long long val)
{
	return -EINVAL;
}
#endif /* CONFIG_MEMCG_KMEM */
=======
#ifndef CONFIG_SLOB
static int memcg_online_kmem(struct mem_cgroup *memcg)
{
	int memcg_id;

	if (cgroup_memory_nokmem)
		return 0;

	BUG_ON(memcg->kmemcg_id >= 0);
	BUG_ON(memcg->kmem_state);

	memcg_id = memcg_alloc_cache_id();
	if (memcg_id < 0)
		return memcg_id;

	static_branch_inc(&memcg_kmem_enabled_key);
	/*
	 * A memory cgroup is considered kmem-online as soon as it gets
	 * kmemcg_id. Setting the id after enabling static branching will
	 * guarantee no one starts accounting before all call sites are
	 * patched.
	 */
	memcg->kmemcg_id = memcg_id;
	memcg->kmem_state = KMEM_ONLINE;

	return 0;
}

static void memcg_offline_kmem(struct mem_cgroup *memcg)
{
	struct cgroup_subsys_state *css;
	struct mem_cgroup *parent, *child;
	int kmemcg_id;

	if (memcg->kmem_state != KMEM_ONLINE)
		return;
	/*
	 * Clear the online state before clearing memcg_caches array
	 * entries. The slab_mutex in memcg_deactivate_kmem_caches()
	 * guarantees that no cache will be created for this cgroup
	 * after we are done (see memcg_create_kmem_cache()).
	 */
	memcg->kmem_state = KMEM_ALLOCATED;

	memcg_deactivate_kmem_caches(memcg);

	kmemcg_id = memcg->kmemcg_id;
	BUG_ON(kmemcg_id < 0);

	parent = parent_mem_cgroup(memcg);
	if (!parent)
		parent = root_mem_cgroup;

	/*
	 * Change kmemcg_id of this cgroup and all its descendants to the
	 * parent's id, and then move all entries from this cgroup's list_lrus
	 * to ones of the parent. After we have finished, all list_lrus
	 * corresponding to this cgroup are guaranteed to remain empty. The
	 * ordering is imposed by list_lru_node->lock taken by
	 * memcg_drain_all_list_lrus().
	 */
	rcu_read_lock(); /* can be called from css_free w/o cgroup_mutex */
	css_for_each_descendant_pre(css, &memcg->css) {
		child = mem_cgroup_from_css(css);
		BUG_ON(child->kmemcg_id != kmemcg_id);
		child->kmemcg_id = parent->kmemcg_id;
		if (!memcg->use_hierarchy)
			break;
	}
	rcu_read_unlock();

	memcg_drain_all_list_lrus(kmemcg_id, parent->kmemcg_id);

	memcg_free_cache_id(kmemcg_id);
}

static void memcg_free_kmem(struct mem_cgroup *memcg)
{
	/* css_alloc() failed, offlining didn't happen */
	if (unlikely(memcg->kmem_state == KMEM_ONLINE))
		memcg_offline_kmem(memcg);

	if (memcg->kmem_state == KMEM_ALLOCATED) {
		memcg_destroy_kmem_caches(memcg);
		static_branch_dec(&memcg_kmem_enabled_key);
		WARN_ON(page_counter_read(&memcg->kmem));
	}
}
#else
static int memcg_online_kmem(struct mem_cgroup *memcg)
{
	return 0;
}
static void memcg_offline_kmem(struct mem_cgroup *memcg)
{
}
static void memcg_free_kmem(struct mem_cgroup *memcg)
{
}
#endif /* !CONFIG_SLOB */

static int memcg_update_kmem_limit(struct mem_cgroup *memcg,
				   unsigned long limit)
{
	int ret;

	mutex_lock(&memcg_limit_mutex);
	ret = page_counter_limit(&memcg->kmem, limit);
	mutex_unlock(&memcg_limit_mutex);
	return ret;
}

static int memcg_update_tcp_limit(struct mem_cgroup *memcg, unsigned long limit)
{
	int ret;

	mutex_lock(&memcg_limit_mutex);

	ret = page_counter_limit(&memcg->tcpmem, limit);
	if (ret)
		goto out;

	if (!memcg->tcpmem_active) {
		/*
		 * The active flag needs to be written after the static_key
		 * update. This is what guarantees that the socket activation
		 * function is the last one to run. See mem_cgroup_sk_alloc()
		 * for details, and note that we don't mark any socket as
		 * belonging to this memcg until that flag is up.
		 *
		 * We need to do this, because static_keys will span multiple
		 * sites, but we can't control their order. If we mark a socket
		 * as accounted, but the accounting functions are not patched in
		 * yet, we'll lose accounting.
		 *
		 * We never race with the readers in mem_cgroup_sk_alloc(),
		 * because when this value change, the code to process it is not
		 * patched in yet.
		 */
		static_branch_inc(&memcg_sockets_enabled_key);
		memcg->tcpmem_active = true;
	}
out:
	mutex_unlock(&memcg_limit_mutex);
	return ret;
}
>>>>>>> v4.9.227

/*
 * The user of this function is...
 * RES_LIMIT.
 */
static ssize_t mem_cgroup_write(struct kernfs_open_file *of,
				char *buf, size_t nbytes, loff_t off)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(of_css(of));
<<<<<<< HEAD
	enum res_type type;
	int name;
	unsigned long long val;
	int ret;

	buf = strstrip(buf);
	type = MEMFILE_TYPE(of_cft(of)->private);
	name = MEMFILE_ATTR(of_cft(of)->private);

	switch (name) {
=======
	unsigned long nr_pages;
	int ret;

	buf = strstrip(buf);
	ret = page_counter_memparse(buf, "-1", &nr_pages);
	if (ret)
		return ret;

	switch (MEMFILE_ATTR(of_cft(of)->private)) {
>>>>>>> v4.9.227
	case RES_LIMIT:
		if (mem_cgroup_is_root(memcg)) { /* Can't set limit on root */
			ret = -EINVAL;
			break;
		}
<<<<<<< HEAD
		/* This function does all necessary parse...reuse it */
		ret = res_counter_memparse_write_strategy(buf, &val);
		if (ret)
			break;
		if (type == _MEM)
			ret = mem_cgroup_resize_limit(memcg, val);
		else if (type == _MEMSWAP)
			ret = mem_cgroup_resize_memsw_limit(memcg, val);
		else if (type == _KMEM)
			ret = memcg_update_kmem_limit(memcg, val);
		else
			return -EINVAL;
		break;
	case RES_SOFT_LIMIT:
		ret = res_counter_memparse_write_strategy(buf, &val);
		if (ret)
			break;
		/*
		 * For memsw, soft limits are hard to implement in terms
		 * of semantics, for now, we support soft limits for
		 * control without swap
		 */
		if (type == _MEM)
			ret = res_counter_set_soft_limit(&memcg->res, val);
		else
			ret = -EINVAL;
		break;
	default:
		ret = -EINVAL; /* should be BUG() ? */
=======
		switch (MEMFILE_TYPE(of_cft(of)->private)) {
		case _MEM:
			ret = mem_cgroup_resize_limit(memcg, nr_pages);
			break;
		case _MEMSWAP:
			ret = mem_cgroup_resize_memsw_limit(memcg, nr_pages);
			break;
		case _KMEM:
			ret = memcg_update_kmem_limit(memcg, nr_pages);
			break;
		case _TCP:
			ret = memcg_update_tcp_limit(memcg, nr_pages);
			break;
		}
		break;
	case RES_SOFT_LIMIT:
		memcg->soft_limit = nr_pages;
		ret = 0;
>>>>>>> v4.9.227
		break;
	}
	return ret ?: nbytes;
}

<<<<<<< HEAD
static void memcg_get_hierarchical_limit(struct mem_cgroup *memcg,
		unsigned long long *mem_limit, unsigned long long *memsw_limit)
{
	unsigned long long min_limit, min_memsw_limit, tmp;

	min_limit = res_counter_read_u64(&memcg->res, RES_LIMIT);
	min_memsw_limit = res_counter_read_u64(&memcg->memsw, RES_LIMIT);
	if (!memcg->use_hierarchy)
		goto out;

	while (memcg->css.parent) {
		memcg = mem_cgroup_from_css(memcg->css.parent);
		if (!memcg->use_hierarchy)
			break;
		tmp = res_counter_read_u64(&memcg->res, RES_LIMIT);
		min_limit = min(min_limit, tmp);
		tmp = res_counter_read_u64(&memcg->memsw, RES_LIMIT);
		min_memsw_limit = min(min_memsw_limit, tmp);
	}
out:
	*mem_limit = min_limit;
	*memsw_limit = min_memsw_limit;
}

=======
>>>>>>> v4.9.227
static ssize_t mem_cgroup_reset(struct kernfs_open_file *of, char *buf,
				size_t nbytes, loff_t off)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(of_css(of));
<<<<<<< HEAD
	int name;
	enum res_type type;

	type = MEMFILE_TYPE(of_cft(of)->private);
	name = MEMFILE_ATTR(of_cft(of)->private);

	switch (name) {
	case RES_MAX_USAGE:
		if (type == _MEM)
			res_counter_reset_max(&memcg->res);
		else if (type == _MEMSWAP)
			res_counter_reset_max(&memcg->memsw);
		else if (type == _KMEM)
			res_counter_reset_max(&memcg->kmem);
		else
			return -EINVAL;
		break;
	case RES_FAILCNT:
		if (type == _MEM)
			res_counter_reset_failcnt(&memcg->res);
		else if (type == _MEMSWAP)
			res_counter_reset_failcnt(&memcg->memsw);
		else if (type == _KMEM)
			res_counter_reset_failcnt(&memcg->kmem);
		else
			return -EINVAL;
		break;
=======
	struct page_counter *counter;

	switch (MEMFILE_TYPE(of_cft(of)->private)) {
	case _MEM:
		counter = &memcg->memory;
		break;
	case _MEMSWAP:
		counter = &memcg->memsw;
		break;
	case _KMEM:
		counter = &memcg->kmem;
		break;
	case _TCP:
		counter = &memcg->tcpmem;
		break;
	default:
		BUG();
	}

	switch (MEMFILE_ATTR(of_cft(of)->private)) {
	case RES_MAX_USAGE:
		page_counter_reset_watermark(counter);
		break;
	case RES_FAILCNT:
		counter->failcnt = 0;
		break;
	default:
		BUG();
>>>>>>> v4.9.227
	}

	return nbytes;
}

static u64 mem_cgroup_move_charge_read(struct cgroup_subsys_state *css,
					struct cftype *cft)
{
	return mem_cgroup_from_css(css)->move_charge_at_immigrate;
}

#ifdef CONFIG_MMU
static int mem_cgroup_move_charge_write(struct cgroup_subsys_state *css,
					struct cftype *cft, u64 val)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);

<<<<<<< HEAD
	if (val >= (1 << NR_MOVE_TYPE))
=======
	if (val & ~MOVE_MASK)
>>>>>>> v4.9.227
		return -EINVAL;

	/*
	 * No kind of locking is needed in here, because ->can_attach() will
	 * check this value once in the beginning of the process, and then carry
	 * on with stale data. This means that changes to this value will only
	 * affect task migrations starting after the change.
	 */
	memcg->move_charge_at_immigrate = val;
	return 0;
}
#else
static int mem_cgroup_move_charge_write(struct cgroup_subsys_state *css,
					struct cftype *cft, u64 val)
{
	return -ENOSYS;
}
#endif

#ifdef CONFIG_NUMA
static int memcg_numa_stat_show(struct seq_file *m, void *v)
{
	struct numa_stat {
		const char *name;
		unsigned int lru_mask;
	};

	static const struct numa_stat stats[] = {
		{ "total", LRU_ALL },
		{ "file", LRU_ALL_FILE },
		{ "anon", LRU_ALL_ANON },
		{ "unevictable", BIT(LRU_UNEVICTABLE) },
	};
	const struct numa_stat *stat;
	int nid;
	unsigned long nr;
	struct mem_cgroup *memcg = mem_cgroup_from_css(seq_css(m));

	for (stat = stats; stat < stats + ARRAY_SIZE(stats); stat++) {
		nr = mem_cgroup_nr_lru_pages(memcg, stat->lru_mask);
		seq_printf(m, "%s=%lu", stat->name, nr);
		for_each_node_state(nid, N_MEMORY) {
			nr = mem_cgroup_node_nr_lru_pages(memcg, nid,
							  stat->lru_mask);
			seq_printf(m, " N%d=%lu", nid, nr);
		}
		seq_putc(m, '\n');
	}

	for (stat = stats; stat < stats + ARRAY_SIZE(stats); stat++) {
		struct mem_cgroup *iter;

		nr = 0;
		for_each_mem_cgroup_tree(iter, memcg)
			nr += mem_cgroup_nr_lru_pages(iter, stat->lru_mask);
		seq_printf(m, "hierarchical_%s=%lu", stat->name, nr);
		for_each_node_state(nid, N_MEMORY) {
			nr = 0;
			for_each_mem_cgroup_tree(iter, memcg)
				nr += mem_cgroup_node_nr_lru_pages(
					iter, nid, stat->lru_mask);
			seq_printf(m, " N%d=%lu", nid, nr);
		}
		seq_putc(m, '\n');
	}

	return 0;
}
#endif /* CONFIG_NUMA */

<<<<<<< HEAD
static inline void mem_cgroup_lru_names_not_uptodate(void)
{
	BUILD_BUG_ON(ARRAY_SIZE(mem_cgroup_lru_names) != NR_LRU_LISTS);
}

static int memcg_stat_show(struct seq_file *m, void *v)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(seq_css(m));
	struct mem_cgroup *mi;
	unsigned int i;

	for (i = 0; i < MEM_CGROUP_STAT_NSTATS; i++) {
		if (i == MEM_CGROUP_STAT_SWAP && !do_swap_account)
			continue;
		seq_printf(m, "%s %ld\n", mem_cgroup_stat_names[i],
=======
static int memcg_stat_show(struct seq_file *m, void *v)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(seq_css(m));
	unsigned long memory, memsw;
	struct mem_cgroup *mi;
	unsigned int i;

	BUILD_BUG_ON(ARRAY_SIZE(mem_cgroup_stat_names) !=
		     MEM_CGROUP_STAT_NSTATS);
	BUILD_BUG_ON(ARRAY_SIZE(mem_cgroup_events_names) !=
		     MEM_CGROUP_EVENTS_NSTATS);
	BUILD_BUG_ON(ARRAY_SIZE(mem_cgroup_lru_names) != NR_LRU_LISTS);

	for (i = 0; i < MEM_CGROUP_STAT_NSTATS; i++) {
		if (i == MEM_CGROUP_STAT_SWAP && !do_memsw_account())
			continue;
		seq_printf(m, "%s %lu\n", mem_cgroup_stat_names[i],
>>>>>>> v4.9.227
			   mem_cgroup_read_stat(memcg, i) * PAGE_SIZE);
	}

	for (i = 0; i < MEM_CGROUP_EVENTS_NSTATS; i++)
		seq_printf(m, "%s %lu\n", mem_cgroup_events_names[i],
			   mem_cgroup_read_events(memcg, i));

	for (i = 0; i < NR_LRU_LISTS; i++)
		seq_printf(m, "%s %lu\n", mem_cgroup_lru_names[i],
			   mem_cgroup_nr_lru_pages(memcg, BIT(i)) * PAGE_SIZE);

	/* Hierarchical information */
<<<<<<< HEAD
	{
		unsigned long long limit, memsw_limit;
		memcg_get_hierarchical_limit(memcg, &limit, &memsw_limit);
		seq_printf(m, "hierarchical_memory_limit %llu\n", limit);
		if (do_swap_account)
			seq_printf(m, "hierarchical_memsw_limit %llu\n",
				   memsw_limit);
	}

	for (i = 0; i < MEM_CGROUP_STAT_NSTATS; i++) {
		long long val = 0;

		if (i == MEM_CGROUP_STAT_SWAP && !do_swap_account)
			continue;
		for_each_mem_cgroup_tree(mi, memcg)
			val += mem_cgroup_read_stat(mi, i) * PAGE_SIZE;
		seq_printf(m, "total_%s %lld\n", mem_cgroup_stat_names[i], val);
=======
	memory = memsw = PAGE_COUNTER_MAX;
	for (mi = memcg; mi; mi = parent_mem_cgroup(mi)) {
		memory = min(memory, mi->memory.limit);
		memsw = min(memsw, mi->memsw.limit);
	}
	seq_printf(m, "hierarchical_memory_limit %llu\n",
		   (u64)memory * PAGE_SIZE);
	if (do_memsw_account())
		seq_printf(m, "hierarchical_memsw_limit %llu\n",
			   (u64)memsw * PAGE_SIZE);

	for (i = 0; i < MEM_CGROUP_STAT_NSTATS; i++) {
		unsigned long long val = 0;

		if (i == MEM_CGROUP_STAT_SWAP && !do_memsw_account())
			continue;
		for_each_mem_cgroup_tree(mi, memcg)
			val += mem_cgroup_read_stat(mi, i) * PAGE_SIZE;
		seq_printf(m, "total_%s %llu\n", mem_cgroup_stat_names[i], val);
>>>>>>> v4.9.227
	}

	for (i = 0; i < MEM_CGROUP_EVENTS_NSTATS; i++) {
		unsigned long long val = 0;

		for_each_mem_cgroup_tree(mi, memcg)
			val += mem_cgroup_read_events(mi, i);
		seq_printf(m, "total_%s %llu\n",
			   mem_cgroup_events_names[i], val);
	}

	for (i = 0; i < NR_LRU_LISTS; i++) {
		unsigned long long val = 0;

		for_each_mem_cgroup_tree(mi, memcg)
			val += mem_cgroup_nr_lru_pages(mi, BIT(i)) * PAGE_SIZE;
		seq_printf(m, "total_%s %llu\n", mem_cgroup_lru_names[i], val);
	}

#ifdef CONFIG_DEBUG_VM
	{
<<<<<<< HEAD
		int nid, zid;
		struct mem_cgroup_per_zone *mz;
=======
		pg_data_t *pgdat;
		struct mem_cgroup_per_node *mz;
>>>>>>> v4.9.227
		struct zone_reclaim_stat *rstat;
		unsigned long recent_rotated[2] = {0, 0};
		unsigned long recent_scanned[2] = {0, 0};

<<<<<<< HEAD
		for_each_online_node(nid)
			for (zid = 0; zid < MAX_NR_ZONES; zid++) {
				mz = &memcg->nodeinfo[nid]->zoneinfo[zid];
				rstat = &mz->lruvec.reclaim_stat;

				recent_rotated[0] += rstat->recent_rotated[0];
				recent_rotated[1] += rstat->recent_rotated[1];
				recent_scanned[0] += rstat->recent_scanned[0];
				recent_scanned[1] += rstat->recent_scanned[1];
			}
=======
		for_each_online_pgdat(pgdat) {
			mz = mem_cgroup_nodeinfo(memcg, pgdat->node_id);
			rstat = &mz->lruvec.reclaim_stat;

			recent_rotated[0] += rstat->recent_rotated[0];
			recent_rotated[1] += rstat->recent_rotated[1];
			recent_scanned[0] += rstat->recent_scanned[0];
			recent_scanned[1] += rstat->recent_scanned[1];
		}
>>>>>>> v4.9.227
		seq_printf(m, "recent_rotated_anon %lu\n", recent_rotated[0]);
		seq_printf(m, "recent_rotated_file %lu\n", recent_rotated[1]);
		seq_printf(m, "recent_scanned_anon %lu\n", recent_scanned[0]);
		seq_printf(m, "recent_scanned_file %lu\n", recent_scanned[1]);
	}
#endif

	return 0;
}
<<<<<<< HEAD
static u64 mem_cgroup_vmpressure_read(struct cgroup_subsys_state *css,
				      struct cftype *cft)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);
	struct vmpressure *vmpr = memcg_to_vmpressure(memcg);
	unsigned long vmpressure;

	vmpressure = vmpr->pressure;

	return vmpressure;
}
=======

>>>>>>> v4.9.227
static u64 mem_cgroup_swappiness_read(struct cgroup_subsys_state *css,
				      struct cftype *cft)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);

	return mem_cgroup_swappiness(memcg);
}

static int mem_cgroup_swappiness_write(struct cgroup_subsys_state *css,
				       struct cftype *cft, u64 val)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);

<<<<<<< HEAD
	if ((val > 200) || ((val > 100) && !css->parent))
=======
	if (val > 100)
>>>>>>> v4.9.227
		return -EINVAL;

	if (css->parent)
		memcg->swappiness = val;
	else
		vm_swappiness = val;

	return 0;
}

static void __mem_cgroup_threshold(struct mem_cgroup *memcg, bool swap)
{
	struct mem_cgroup_threshold_ary *t;
<<<<<<< HEAD
	u64 usage;
=======
	unsigned long usage;
>>>>>>> v4.9.227
	int i;

	rcu_read_lock();
	if (!swap)
		t = rcu_dereference(memcg->thresholds.primary);
	else
		t = rcu_dereference(memcg->memsw_thresholds.primary);

	if (!t)
		goto unlock;

	usage = mem_cgroup_usage(memcg, swap);

	/*
	 * current_threshold points to threshold just below or equal to usage.
	 * If it's not true, a threshold was crossed after last
	 * call of __mem_cgroup_threshold().
	 */
	i = t->current_threshold;

	/*
	 * Iterate backward over array of thresholds starting from
	 * current_threshold and check if a threshold is crossed.
	 * If none of thresholds below usage is crossed, we read
	 * only one element of the array here.
	 */
	for (; i >= 0 && unlikely(t->entries[i].threshold > usage); i--)
		eventfd_signal(t->entries[i].eventfd, 1);

	/* i = current_threshold + 1 */
	i++;

	/*
	 * Iterate forward over array of thresholds starting from
	 * current_threshold+1 and check if a threshold is crossed.
	 * If none of thresholds above usage is crossed, we read
	 * only one element of the array here.
	 */
	for (; i < t->size && unlikely(t->entries[i].threshold <= usage); i++)
		eventfd_signal(t->entries[i].eventfd, 1);

	/* Update current_threshold */
	t->current_threshold = i - 1;
unlock:
	rcu_read_unlock();
}

static void mem_cgroup_threshold(struct mem_cgroup *memcg)
{
	while (memcg) {
		__mem_cgroup_threshold(memcg, false);
<<<<<<< HEAD
		if (do_swap_account)
=======
		if (do_memsw_account())
>>>>>>> v4.9.227
			__mem_cgroup_threshold(memcg, true);

		memcg = parent_mem_cgroup(memcg);
	}
}

static int compare_thresholds(const void *a, const void *b)
{
	const struct mem_cgroup_threshold *_a = a;
	const struct mem_cgroup_threshold *_b = b;

	if (_a->threshold > _b->threshold)
		return 1;

	if (_a->threshold < _b->threshold)
		return -1;

	return 0;
}

static int mem_cgroup_oom_notify_cb(struct mem_cgroup *memcg)
{
	struct mem_cgroup_eventfd_list *ev;

	spin_lock(&memcg_oom_lock);

	list_for_each_entry(ev, &memcg->oom_notify, list)
		eventfd_signal(ev->eventfd, 1);

	spin_unlock(&memcg_oom_lock);
	return 0;
}

static void mem_cgroup_oom_notify(struct mem_cgroup *memcg)
{
	struct mem_cgroup *iter;

	for_each_mem_cgroup_tree(iter, memcg)
		mem_cgroup_oom_notify_cb(iter);
}

static int __mem_cgroup_usage_register_event(struct mem_cgroup *memcg,
	struct eventfd_ctx *eventfd, const char *args, enum res_type type)
{
	struct mem_cgroup_thresholds *thresholds;
	struct mem_cgroup_threshold_ary *new;
<<<<<<< HEAD
	u64 threshold, usage;
	int i, size, ret;

	ret = res_counter_memparse_write_strategy(args, &threshold);
=======
	unsigned long threshold;
	unsigned long usage;
	int i, size, ret;

	ret = page_counter_memparse(args, "-1", &threshold);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	mutex_lock(&memcg->thresholds_lock);

	if (type == _MEM) {
		thresholds = &memcg->thresholds;
		usage = mem_cgroup_usage(memcg, false);
	} else if (type == _MEMSWAP) {
		thresholds = &memcg->memsw_thresholds;
		usage = mem_cgroup_usage(memcg, true);
	} else
		BUG();

	/* Check if a threshold crossed before adding a new one */
	if (thresholds->primary)
		__mem_cgroup_threshold(memcg, type == _MEMSWAP);

	size = thresholds->primary ? thresholds->primary->size + 1 : 1;

	/* Allocate memory for new array of thresholds */
	new = kmalloc(sizeof(*new) + size * sizeof(struct mem_cgroup_threshold),
			GFP_KERNEL);
	if (!new) {
		ret = -ENOMEM;
		goto unlock;
	}
	new->size = size;

	/* Copy thresholds (if any) to new array */
	if (thresholds->primary) {
		memcpy(new->entries, thresholds->primary->entries, (size - 1) *
				sizeof(struct mem_cgroup_threshold));
	}

	/* Add new threshold */
	new->entries[size - 1].eventfd = eventfd;
	new->entries[size - 1].threshold = threshold;

	/* Sort thresholds. Registering of new threshold isn't time-critical */
	sort(new->entries, size, sizeof(struct mem_cgroup_threshold),
			compare_thresholds, NULL);

	/* Find current threshold */
	new->current_threshold = -1;
	for (i = 0; i < size; i++) {
		if (new->entries[i].threshold <= usage) {
			/*
			 * new->current_threshold will not be used until
			 * rcu_assign_pointer(), so it's safe to increment
			 * it here.
			 */
			++new->current_threshold;
		} else
			break;
	}

	/* Free old spare buffer and save old primary buffer as spare */
	kfree(thresholds->spare);
	thresholds->spare = thresholds->primary;

	rcu_assign_pointer(thresholds->primary, new);

	/* To be sure that nobody uses thresholds */
	synchronize_rcu();

unlock:
	mutex_unlock(&memcg->thresholds_lock);

	return ret;
}

static int mem_cgroup_usage_register_event(struct mem_cgroup *memcg,
	struct eventfd_ctx *eventfd, const char *args)
{
	return __mem_cgroup_usage_register_event(memcg, eventfd, args, _MEM);
}

static int memsw_cgroup_usage_register_event(struct mem_cgroup *memcg,
	struct eventfd_ctx *eventfd, const char *args)
{
	return __mem_cgroup_usage_register_event(memcg, eventfd, args, _MEMSWAP);
}

static void __mem_cgroup_usage_unregister_event(struct mem_cgroup *memcg,
	struct eventfd_ctx *eventfd, enum res_type type)
{
	struct mem_cgroup_thresholds *thresholds;
	struct mem_cgroup_threshold_ary *new;
<<<<<<< HEAD
	u64 usage;
	int i, j, size;
=======
	unsigned long usage;
	int i, j, size, entries;
>>>>>>> v4.9.227

	mutex_lock(&memcg->thresholds_lock);

	if (type == _MEM) {
		thresholds = &memcg->thresholds;
		usage = mem_cgroup_usage(memcg, false);
	} else if (type == _MEMSWAP) {
		thresholds = &memcg->memsw_thresholds;
		usage = mem_cgroup_usage(memcg, true);
	} else
		BUG();

	if (!thresholds->primary)
		goto unlock;

	/* Check if a threshold crossed before removing */
	__mem_cgroup_threshold(memcg, type == _MEMSWAP);

	/* Calculate new number of threshold */
<<<<<<< HEAD
	size = 0;
	for (i = 0; i < thresholds->primary->size; i++) {
		if (thresholds->primary->entries[i].eventfd != eventfd)
			size++;
=======
	size = entries = 0;
	for (i = 0; i < thresholds->primary->size; i++) {
		if (thresholds->primary->entries[i].eventfd != eventfd)
			size++;
		else
			entries++;
>>>>>>> v4.9.227
	}

	new = thresholds->spare;

<<<<<<< HEAD
=======
	/* If no items related to eventfd have been cleared, nothing to do */
	if (!entries)
		goto unlock;

>>>>>>> v4.9.227
	/* Set thresholds array to NULL if we don't have thresholds */
	if (!size) {
		kfree(new);
		new = NULL;
		goto swap_buffers;
	}

	new->size = size;

	/* Copy thresholds and find current threshold */
	new->current_threshold = -1;
	for (i = 0, j = 0; i < thresholds->primary->size; i++) {
		if (thresholds->primary->entries[i].eventfd == eventfd)
			continue;

		new->entries[j] = thresholds->primary->entries[i];
		if (new->entries[j].threshold <= usage) {
			/*
			 * new->current_threshold will not be used
			 * until rcu_assign_pointer(), so it's safe to increment
			 * it here.
			 */
			++new->current_threshold;
		}
		j++;
	}

swap_buffers:
	/* Swap primary and spare array */
	thresholds->spare = thresholds->primary;

	rcu_assign_pointer(thresholds->primary, new);

	/* To be sure that nobody uses thresholds */
	synchronize_rcu();

	/* If all events are unregistered, free the spare array */
	if (!new) {
		kfree(thresholds->spare);
		thresholds->spare = NULL;
	}
unlock:
	mutex_unlock(&memcg->thresholds_lock);
}

static void mem_cgroup_usage_unregister_event(struct mem_cgroup *memcg,
	struct eventfd_ctx *eventfd)
{
	return __mem_cgroup_usage_unregister_event(memcg, eventfd, _MEM);
}

static void memsw_cgroup_usage_unregister_event(struct mem_cgroup *memcg,
	struct eventfd_ctx *eventfd)
{
	return __mem_cgroup_usage_unregister_event(memcg, eventfd, _MEMSWAP);
}

static int mem_cgroup_oom_register_event(struct mem_cgroup *memcg,
	struct eventfd_ctx *eventfd, const char *args)
{
	struct mem_cgroup_eventfd_list *event;

	event = kmalloc(sizeof(*event),	GFP_KERNEL);
	if (!event)
		return -ENOMEM;

	spin_lock(&memcg_oom_lock);

	event->eventfd = eventfd;
	list_add(&event->list, &memcg->oom_notify);

	/* already in OOM ? */
<<<<<<< HEAD
	if (atomic_read(&memcg->under_oom))
=======
	if (memcg->under_oom)
>>>>>>> v4.9.227
		eventfd_signal(eventfd, 1);
	spin_unlock(&memcg_oom_lock);

	return 0;
}

static void mem_cgroup_oom_unregister_event(struct mem_cgroup *memcg,
	struct eventfd_ctx *eventfd)
{
	struct mem_cgroup_eventfd_list *ev, *tmp;

	spin_lock(&memcg_oom_lock);

	list_for_each_entry_safe(ev, tmp, &memcg->oom_notify, list) {
		if (ev->eventfd == eventfd) {
			list_del(&ev->list);
			kfree(ev);
		}
	}

	spin_unlock(&memcg_oom_lock);
}

static int mem_cgroup_oom_control_read(struct seq_file *sf, void *v)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(seq_css(sf));

	seq_printf(sf, "oom_kill_disable %d\n", memcg->oom_kill_disable);
<<<<<<< HEAD
	seq_printf(sf, "under_oom %d\n", (bool)atomic_read(&memcg->under_oom));
=======
	seq_printf(sf, "under_oom %d\n", (bool)memcg->under_oom);
>>>>>>> v4.9.227
	return 0;
}

static int mem_cgroup_oom_control_write(struct cgroup_subsys_state *css,
	struct cftype *cft, u64 val)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);

	/* cannot set to root cgroup and only 0 and 1 are allowed */
	if (!css->parent || !((val == 0) || (val == 1)))
		return -EINVAL;

	memcg->oom_kill_disable = val;
	if (!val)
		memcg_oom_recover(memcg);

	return 0;
}

<<<<<<< HEAD
#ifdef CONFIG_MEMCG_KMEM
static int memcg_init_kmem(struct mem_cgroup *memcg, struct cgroup_subsys *ss)
{
	int ret;

	memcg->kmemcg_id = -1;
	ret = memcg_propagate_kmem(memcg);
	if (ret)
		return ret;

	return mem_cgroup_sockets_init(memcg, ss);
}

static void memcg_destroy_kmem(struct mem_cgroup *memcg)
{
	mem_cgroup_sockets_destroy(memcg);
}

static void kmem_cgroup_css_offline(struct mem_cgroup *memcg)
{
	if (!memcg_kmem_is_active(memcg))
		return;

	/*
	 * kmem charges can outlive the cgroup. In the case of slab
	 * pages, for instance, a page contain objects from various
	 * processes. As we prevent from taking a reference for every
	 * such allocation we have to be careful when doing uncharge
	 * (see memcg_uncharge_kmem) and here during offlining.
	 *
	 * The idea is that that only the _last_ uncharge which sees
	 * the dead memcg will drop the last reference. An additional
	 * reference is taken here before the group is marked dead
	 * which is then paired with css_put during uncharge resp. here.
	 *
	 * Although this might sound strange as this path is called from
	 * css_offline() when the referencemight have dropped down to 0 and
	 * shouldn't be incremented anymore (css_tryget_online() would
	 * fail) we do not have other options because of the kmem
	 * allocations lifetime.
	 */
	css_get(&memcg->css);

	memcg_kmem_mark_dead(memcg);

	if (res_counter_read_u64(&memcg->kmem, RES_USAGE) != 0)
		return;

	if (memcg_kmem_test_and_clear_dead(memcg))
		css_put(&memcg->css);
}
#else
static int memcg_init_kmem(struct mem_cgroup *memcg, struct cgroup_subsys *ss)
=======
#ifdef CONFIG_CGROUP_WRITEBACK

struct list_head *mem_cgroup_cgwb_list(struct mem_cgroup *memcg)
{
	return &memcg->cgwb_list;
}

static int memcg_wb_domain_init(struct mem_cgroup *memcg, gfp_t gfp)
{
	return wb_domain_init(&memcg->cgwb_domain, gfp);
}

static void memcg_wb_domain_exit(struct mem_cgroup *memcg)
{
	wb_domain_exit(&memcg->cgwb_domain);
}

static void memcg_wb_domain_size_changed(struct mem_cgroup *memcg)
{
	wb_domain_size_changed(&memcg->cgwb_domain);
}

struct wb_domain *mem_cgroup_wb_domain(struct bdi_writeback *wb)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(wb->memcg_css);

	if (!memcg->css.parent)
		return NULL;

	return &memcg->cgwb_domain;
}

/**
 * mem_cgroup_wb_stats - retrieve writeback related stats from its memcg
 * @wb: bdi_writeback in question
 * @pfilepages: out parameter for number of file pages
 * @pheadroom: out parameter for number of allocatable pages according to memcg
 * @pdirty: out parameter for number of dirty pages
 * @pwriteback: out parameter for number of pages under writeback
 *
 * Determine the numbers of file, headroom, dirty, and writeback pages in
 * @wb's memcg.  File, dirty and writeback are self-explanatory.  Headroom
 * is a bit more involved.
 *
 * A memcg's headroom is "min(max, high) - used".  In the hierarchy, the
 * headroom is calculated as the lowest headroom of itself and the
 * ancestors.  Note that this doesn't consider the actual amount of
 * available memory in the system.  The caller should further cap
 * *@pheadroom accordingly.
 */
void mem_cgroup_wb_stats(struct bdi_writeback *wb, unsigned long *pfilepages,
			 unsigned long *pheadroom, unsigned long *pdirty,
			 unsigned long *pwriteback)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(wb->memcg_css);
	struct mem_cgroup *parent;

	*pdirty = mem_cgroup_read_stat(memcg, MEM_CGROUP_STAT_DIRTY);

	/* this should eventually include NR_UNSTABLE_NFS */
	*pwriteback = mem_cgroup_read_stat(memcg, MEM_CGROUP_STAT_WRITEBACK);
	*pfilepages = mem_cgroup_nr_lru_pages(memcg, (1 << LRU_INACTIVE_FILE) |
						     (1 << LRU_ACTIVE_FILE));
	*pheadroom = PAGE_COUNTER_MAX;

	while ((parent = parent_mem_cgroup(memcg))) {
		unsigned long ceiling = min(memcg->memory.limit, memcg->high);
		unsigned long used = page_counter_read(&memcg->memory);

		*pheadroom = min(*pheadroom, ceiling - min(ceiling, used));
		memcg = parent;
	}
}

#else	/* CONFIG_CGROUP_WRITEBACK */

static int memcg_wb_domain_init(struct mem_cgroup *memcg, gfp_t gfp)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static void memcg_destroy_kmem(struct mem_cgroup *memcg)
{
}

static void kmem_cgroup_css_offline(struct mem_cgroup *memcg)
{
}
#endif
=======
static void memcg_wb_domain_exit(struct mem_cgroup *memcg)
{
}

static void memcg_wb_domain_size_changed(struct mem_cgroup *memcg)
{
}

#endif	/* CONFIG_CGROUP_WRITEBACK */
>>>>>>> v4.9.227

/*
 * DO NOT USE IN NEW FILES.
 *
 * "cgroup.event_control" implementation.
 *
 * This is way over-engineered.  It tries to support fully configurable
 * events for each user.  Such level of flexibility is completely
 * unnecessary especially in the light of the planned unified hierarchy.
 *
 * Please deprecate this and replace with something simpler if at all
 * possible.
 */

/*
 * Unregister event and free resources.
 *
 * Gets called from workqueue.
 */
static void memcg_event_remove(struct work_struct *work)
{
	struct mem_cgroup_event *event =
		container_of(work, struct mem_cgroup_event, remove);
	struct mem_cgroup *memcg = event->memcg;

	remove_wait_queue(event->wqh, &event->wait);

	event->unregister_event(memcg, event->eventfd);

	/* Notify userspace the event is going away. */
	eventfd_signal(event->eventfd, 1);

	eventfd_ctx_put(event->eventfd);
	kfree(event);
	css_put(&memcg->css);
}

/*
 * Gets called on POLLHUP on eventfd when user closes it.
 *
 * Called with wqh->lock held and interrupts disabled.
 */
static int memcg_event_wake(wait_queue_t *wait, unsigned mode,
			    int sync, void *key)
{
	struct mem_cgroup_event *event =
		container_of(wait, struct mem_cgroup_event, wait);
	struct mem_cgroup *memcg = event->memcg;
	unsigned long flags = (unsigned long)key;

	if (flags & POLLHUP) {
		/*
		 * If the event has been detached at cgroup removal, we
		 * can simply return knowing the other side will cleanup
		 * for us.
		 *
		 * We can't race against event freeing since the other
		 * side will require wqh->lock via remove_wait_queue(),
		 * which we hold.
		 */
		spin_lock(&memcg->event_list_lock);
		if (!list_empty(&event->list)) {
			list_del_init(&event->list);
			/*
			 * We are in atomic context, but cgroup_event_remove()
			 * may sleep, so we have to call it in workqueue.
			 */
			schedule_work(&event->remove);
		}
		spin_unlock(&memcg->event_list_lock);
	}

	return 0;
}

static void memcg_event_ptable_queue_proc(struct file *file,
		wait_queue_head_t *wqh, poll_table *pt)
{
	struct mem_cgroup_event *event =
		container_of(pt, struct mem_cgroup_event, pt);

	event->wqh = wqh;
	add_wait_queue(wqh, &event->wait);
}

/*
 * DO NOT USE IN NEW FILES.
 *
 * Parse input and register new cgroup event handler.
 *
 * Input must be in format '<event_fd> <control_fd> <args>'.
 * Interpretation of args is defined by control file implementation.
 */
static ssize_t memcg_write_event_control(struct kernfs_open_file *of,
					 char *buf, size_t nbytes, loff_t off)
{
	struct cgroup_subsys_state *css = of_css(of);
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);
	struct mem_cgroup_event *event;
	struct cgroup_subsys_state *cfile_css;
	unsigned int efd, cfd;
	struct fd efile;
	struct fd cfile;
	const char *name;
	char *endp;
	int ret;

	buf = strstrip(buf);

	efd = simple_strtoul(buf, &endp, 10);
	if (*endp != ' ')
		return -EINVAL;
	buf = endp + 1;

	cfd = simple_strtoul(buf, &endp, 10);
	if ((*endp != ' ') && (*endp != '\0'))
		return -EINVAL;
	buf = endp + 1;

	event = kzalloc(sizeof(*event), GFP_KERNEL);
	if (!event)
		return -ENOMEM;

	event->memcg = memcg;
	INIT_LIST_HEAD(&event->list);
	init_poll_funcptr(&event->pt, memcg_event_ptable_queue_proc);
	init_waitqueue_func_entry(&event->wait, memcg_event_wake);
	INIT_WORK(&event->remove, memcg_event_remove);

	efile = fdget(efd);
	if (!efile.file) {
		ret = -EBADF;
		goto out_kfree;
	}

	event->eventfd = eventfd_ctx_fileget(efile.file);
	if (IS_ERR(event->eventfd)) {
		ret = PTR_ERR(event->eventfd);
		goto out_put_efile;
	}

	cfile = fdget(cfd);
	if (!cfile.file) {
		ret = -EBADF;
		goto out_put_eventfd;
	}

	/* the process need read permission on control file */
	/* AV: shouldn't we check that it's been opened for read instead? */
	ret = inode_permission(file_inode(cfile.file), MAY_READ);
	if (ret < 0)
		goto out_put_cfile;

	/*
	 * Determine the event callbacks and set them in @event.  This used
	 * to be done via struct cftype but cgroup core no longer knows
	 * about these events.  The following is crude but the whole thing
	 * is for compatibility anyway.
	 *
	 * DO NOT ADD NEW FILES.
	 */
<<<<<<< HEAD
	name = cfile.file->f_dentry->d_name.name;
=======
	name = cfile.file->f_path.dentry->d_name.name;
>>>>>>> v4.9.227

	if (!strcmp(name, "memory.usage_in_bytes")) {
		event->register_event = mem_cgroup_usage_register_event;
		event->unregister_event = mem_cgroup_usage_unregister_event;
	} else if (!strcmp(name, "memory.oom_control")) {
		event->register_event = mem_cgroup_oom_register_event;
		event->unregister_event = mem_cgroup_oom_unregister_event;
	} else if (!strcmp(name, "memory.pressure_level")) {
		event->register_event = vmpressure_register_event;
		event->unregister_event = vmpressure_unregister_event;
	} else if (!strcmp(name, "memory.memsw.usage_in_bytes")) {
		event->register_event = memsw_cgroup_usage_register_event;
		event->unregister_event = memsw_cgroup_usage_unregister_event;
	} else {
		ret = -EINVAL;
		goto out_put_cfile;
	}

	/*
	 * Verify @cfile should belong to @css.  Also, remaining events are
	 * automatically removed on cgroup destruction but the removal is
	 * asynchronous, so take an extra ref on @css.
	 */
<<<<<<< HEAD
	cfile_css = css_tryget_online_from_dir(cfile.file->f_dentry->d_parent,
=======
	cfile_css = css_tryget_online_from_dir(cfile.file->f_path.dentry->d_parent,
>>>>>>> v4.9.227
					       &memory_cgrp_subsys);
	ret = -EINVAL;
	if (IS_ERR(cfile_css))
		goto out_put_cfile;
	if (cfile_css != css) {
		css_put(cfile_css);
		goto out_put_cfile;
	}

	ret = event->register_event(memcg, event->eventfd, buf);
	if (ret)
		goto out_put_css;

	efile.file->f_op->poll(efile.file, &event->pt);

	spin_lock(&memcg->event_list_lock);
	list_add(&event->list, &memcg->event_list);
	spin_unlock(&memcg->event_list_lock);

	fdput(cfile);
	fdput(efile);

	return nbytes;

out_put_css:
	css_put(css);
out_put_cfile:
	fdput(cfile);
out_put_eventfd:
	eventfd_ctx_put(event->eventfd);
out_put_efile:
	fdput(efile);
out_kfree:
	kfree(event);

	return ret;
}

<<<<<<< HEAD
static struct cftype mem_cgroup_files[] = {
=======
static struct cftype mem_cgroup_legacy_files[] = {
>>>>>>> v4.9.227
	{
		.name = "usage_in_bytes",
		.private = MEMFILE_PRIVATE(_MEM, RES_USAGE),
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "max_usage_in_bytes",
		.private = MEMFILE_PRIVATE(_MEM, RES_MAX_USAGE),
		.write = mem_cgroup_reset,
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "limit_in_bytes",
		.private = MEMFILE_PRIVATE(_MEM, RES_LIMIT),
		.write = mem_cgroup_write,
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "soft_limit_in_bytes",
		.private = MEMFILE_PRIVATE(_MEM, RES_SOFT_LIMIT),
		.write = mem_cgroup_write,
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "failcnt",
		.private = MEMFILE_PRIVATE(_MEM, RES_FAILCNT),
		.write = mem_cgroup_reset,
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "stat",
		.seq_show = memcg_stat_show,
	},
	{
		.name = "force_empty",
		.write = mem_cgroup_force_empty_write,
	},
	{
		.name = "use_hierarchy",
		.write_u64 = mem_cgroup_hierarchy_write,
		.read_u64 = mem_cgroup_hierarchy_read,
	},
	{
		.name = "cgroup.event_control",		/* XXX: for compat */
		.write = memcg_write_event_control,
<<<<<<< HEAD
		.flags = CFTYPE_NO_PREFIX,
		.mode = S_IWUGO,
=======
		.flags = CFTYPE_NO_PREFIX | CFTYPE_WORLD_WRITABLE,
>>>>>>> v4.9.227
	},
	{
		.name = "swappiness",
		.read_u64 = mem_cgroup_swappiness_read,
		.write_u64 = mem_cgroup_swappiness_write,
	},
	{
		.name = "move_charge_at_immigrate",
		.read_u64 = mem_cgroup_move_charge_read,
		.write_u64 = mem_cgroup_move_charge_write,
	},
	{
		.name = "oom_control",
		.seq_show = mem_cgroup_oom_control_read,
		.write_u64 = mem_cgroup_oom_control_write,
		.private = MEMFILE_PRIVATE(_OOM_TYPE, OOM_CONTROL),
	},
	{
		.name = "pressure_level",
	},
<<<<<<< HEAD
	{
		.name = "vmpressure",
		.read_u64 = mem_cgroup_vmpressure_read,
	},
=======
>>>>>>> v4.9.227
#ifdef CONFIG_NUMA
	{
		.name = "numa_stat",
		.seq_show = memcg_numa_stat_show,
	},
#endif
<<<<<<< HEAD
#ifdef CONFIG_MEMCG_KMEM
=======
>>>>>>> v4.9.227
	{
		.name = "kmem.limit_in_bytes",
		.private = MEMFILE_PRIVATE(_KMEM, RES_LIMIT),
		.write = mem_cgroup_write,
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "kmem.usage_in_bytes",
		.private = MEMFILE_PRIVATE(_KMEM, RES_USAGE),
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "kmem.failcnt",
		.private = MEMFILE_PRIVATE(_KMEM, RES_FAILCNT),
		.write = mem_cgroup_reset,
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "kmem.max_usage_in_bytes",
		.private = MEMFILE_PRIVATE(_KMEM, RES_MAX_USAGE),
		.write = mem_cgroup_reset,
		.read_u64 = mem_cgroup_read_u64,
	},
#ifdef CONFIG_SLABINFO
	{
		.name = "kmem.slabinfo",
<<<<<<< HEAD
		.seq_show = mem_cgroup_slabinfo_read,
	},
#endif
#endif
	{ },	/* terminate */
};

#ifdef CONFIG_MEMCG_SWAP
static struct cftype memsw_cgroup_files[] = {
	{
		.name = "memsw.usage_in_bytes",
		.private = MEMFILE_PRIVATE(_MEMSWAP, RES_USAGE),
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "memsw.max_usage_in_bytes",
		.private = MEMFILE_PRIVATE(_MEMSWAP, RES_MAX_USAGE),
		.write = mem_cgroup_reset,
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "memsw.limit_in_bytes",
		.private = MEMFILE_PRIVATE(_MEMSWAP, RES_LIMIT),
=======
		.seq_start = slab_start,
		.seq_next = slab_next,
		.seq_stop = slab_stop,
		.seq_show = memcg_slab_show,
	},
#endif
	{
		.name = "kmem.tcp.limit_in_bytes",
		.private = MEMFILE_PRIVATE(_TCP, RES_LIMIT),
>>>>>>> v4.9.227
		.write = mem_cgroup_write,
		.read_u64 = mem_cgroup_read_u64,
	},
	{
<<<<<<< HEAD
		.name = "memsw.failcnt",
		.private = MEMFILE_PRIVATE(_MEMSWAP, RES_FAILCNT),
=======
		.name = "kmem.tcp.usage_in_bytes",
		.private = MEMFILE_PRIVATE(_TCP, RES_USAGE),
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "kmem.tcp.failcnt",
		.private = MEMFILE_PRIVATE(_TCP, RES_FAILCNT),
		.write = mem_cgroup_reset,
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "kmem.tcp.max_usage_in_bytes",
		.private = MEMFILE_PRIVATE(_TCP, RES_MAX_USAGE),
>>>>>>> v4.9.227
		.write = mem_cgroup_reset,
		.read_u64 = mem_cgroup_read_u64,
	},
	{ },	/* terminate */
};
<<<<<<< HEAD
#endif
static int alloc_mem_cgroup_per_zone_info(struct mem_cgroup *memcg, int node)
{
	struct mem_cgroup_per_node *pn;
	struct mem_cgroup_per_zone *mz;
	int zone, tmp = node;
=======

/*
 * Private memory cgroup IDR
 *
 * Swap-out records and page cache shadow entries need to store memcg
 * references in constrained space, so we maintain an ID space that is
 * limited to 16 bit (MEM_CGROUP_ID_MAX), limiting the total number of
 * memory-controlled cgroups to 64k.
 *
 * However, there usually are many references to the oflline CSS after
 * the cgroup has been destroyed, such as page cache or reclaimable
 * slab objects, that don't need to hang on to the ID. We want to keep
 * those dead CSS from occupying IDs, or we might quickly exhaust the
 * relatively small ID space and prevent the creation of new cgroups
 * even when there are much fewer than 64k cgroups - possibly none.
 *
 * Maintain a private 16-bit ID space for memcg, and allow the ID to
 * be freed and recycled when it's no longer needed, which is usually
 * when the CSS is offlined.
 *
 * The only exception to that are records of swapped out tmpfs/shmem
 * pages that need to be attributed to live ancestors on swapin. But
 * those references are manageable from userspace.
 */

static DEFINE_IDR(mem_cgroup_idr);

static void mem_cgroup_id_remove(struct mem_cgroup *memcg)
{
	if (memcg->id.id > 0) {
		idr_remove(&mem_cgroup_idr, memcg->id.id);
		memcg->id.id = 0;
	}
}

static void mem_cgroup_id_get_many(struct mem_cgroup *memcg, unsigned int n)
{
	VM_BUG_ON(atomic_read(&memcg->id.ref) <= 0);
	atomic_add(n, &memcg->id.ref);
}

static void mem_cgroup_id_put_many(struct mem_cgroup *memcg, unsigned int n)
{
	VM_BUG_ON(atomic_read(&memcg->id.ref) < n);
	if (atomic_sub_and_test(n, &memcg->id.ref)) {
		mem_cgroup_id_remove(memcg);

		/* Memcg ID pins CSS */
		css_put(&memcg->css);
	}
}

static inline void mem_cgroup_id_get(struct mem_cgroup *memcg)
{
	mem_cgroup_id_get_many(memcg, 1);
}

static inline void mem_cgroup_id_put(struct mem_cgroup *memcg)
{
	mem_cgroup_id_put_many(memcg, 1);
}

/**
 * mem_cgroup_from_id - look up a memcg from a memcg id
 * @id: the memcg id to look up
 *
 * Caller must hold rcu_read_lock().
 */
struct mem_cgroup *mem_cgroup_from_id(unsigned short id)
{
	WARN_ON_ONCE(!rcu_read_lock_held());
	return idr_find(&mem_cgroup_idr, id);
}

static int alloc_mem_cgroup_per_node_info(struct mem_cgroup *memcg, int node)
{
	struct mem_cgroup_per_node *pn;
	int tmp = node;
>>>>>>> v4.9.227
	/*
	 * This routine is called against possible nodes.
	 * But it's BUG to call kmalloc() against offline node.
	 *
	 * TODO: this routine can waste much memory for nodes which will
	 *       never be onlined. It's better to use memory hotplug callback
	 *       function.
	 */
	if (!node_state(node, N_NORMAL_MEMORY))
		tmp = -1;
	pn = kzalloc_node(sizeof(*pn), GFP_KERNEL, tmp);
	if (!pn)
		return 1;

<<<<<<< HEAD
	for (zone = 0; zone < MAX_NR_ZONES; zone++) {
		mz = &pn->zoneinfo[zone];
		lruvec_init(&mz->lruvec);
		mz->usage_in_excess = 0;
		mz->on_tree = false;
		mz->memcg = memcg;
	}
=======
	lruvec_init(&pn->lruvec);
	pn->usage_in_excess = 0;
	pn->on_tree = false;
	pn->memcg = memcg;

>>>>>>> v4.9.227
	memcg->nodeinfo[node] = pn;
	return 0;
}

<<<<<<< HEAD
static void free_mem_cgroup_per_zone_info(struct mem_cgroup *memcg, int node)
=======
static void free_mem_cgroup_per_node_info(struct mem_cgroup *memcg, int node)
>>>>>>> v4.9.227
{
	kfree(memcg->nodeinfo[node]);
}

<<<<<<< HEAD
=======
static void __mem_cgroup_free(struct mem_cgroup *memcg)
{
	int node;

	for_each_node(node)
		free_mem_cgroup_per_node_info(memcg, node);
	free_percpu(memcg->stat);
	kfree(memcg);
}

static void mem_cgroup_free(struct mem_cgroup *memcg)
{
	memcg_wb_domain_exit(memcg);
	__mem_cgroup_free(memcg);
}

>>>>>>> v4.9.227
static struct mem_cgroup *mem_cgroup_alloc(void)
{
	struct mem_cgroup *memcg;
	size_t size;
<<<<<<< HEAD
=======
	int node;
>>>>>>> v4.9.227

	size = sizeof(struct mem_cgroup);
	size += nr_node_ids * sizeof(struct mem_cgroup_per_node *);

	memcg = kzalloc(size, GFP_KERNEL);
	if (!memcg)
		return NULL;

<<<<<<< HEAD
	memcg->stat = alloc_percpu(struct mem_cgroup_stat_cpu);
	if (!memcg->stat)
		goto out_free;
	spin_lock_init(&memcg->pcp_counter_lock);
	return memcg;

out_free:
	kfree(memcg);
	return NULL;
}

/*
 * At destroying mem_cgroup, references from swap_cgroup can remain.
 * (scanning all at force_empty is too costly...)
 *
 * Instead of clearing all references at force_empty, we remember
 * the number of reference from swap_cgroup and free mem_cgroup when
 * it goes down to 0.
 *
 * Removal of cgroup itself succeeds regardless of refs from swap.
 */

static void __mem_cgroup_free(struct mem_cgroup *memcg)
{
	int node;

	mem_cgroup_remove_from_trees(memcg);

	for_each_node(node)
		free_mem_cgroup_per_zone_info(memcg, node);

	free_percpu(memcg->stat);

	/*
	 * We need to make sure that (at least for now), the jump label
	 * destruction code runs outside of the cgroup lock. This is because
	 * get_online_cpus(), which is called from the static_branch update,
	 * can't be called inside the cgroup_lock. cpusets are the ones
	 * enforcing this dependency, so if they ever change, we might as well.
	 *
	 * schedule_work() will guarantee this happens. Be careful if you need
	 * to move this code around, and make sure it is outside
	 * the cgroup_lock.
	 */
	disarm_static_keys(memcg);
	kfree(memcg);
}

/*
 * Returns the parent mem_cgroup in memcgroup hierarchy with hierarchy enabled.
 */
struct mem_cgroup *parent_mem_cgroup(struct mem_cgroup *memcg)
{
	if (!memcg->res.parent)
		return NULL;
	return mem_cgroup_from_res_counter(memcg->res.parent, res);
}
EXPORT_SYMBOL(parent_mem_cgroup);

static void __init mem_cgroup_soft_limit_tree_init(void)
{
	struct mem_cgroup_tree_per_node *rtpn;
	struct mem_cgroup_tree_per_zone *rtpz;
	int tmp, node, zone;

	for_each_node(node) {
		tmp = node;
		if (!node_state(node, N_NORMAL_MEMORY))
			tmp = -1;
		rtpn = kzalloc_node(sizeof(*rtpn), GFP_KERNEL, tmp);
		BUG_ON(!rtpn);

		soft_limit_tree.rb_tree_per_node[node] = rtpn;

		for (zone = 0; zone < MAX_NR_ZONES; zone++) {
			rtpz = &rtpn->rb_tree_per_zone[zone];
			rtpz->rb_root = RB_ROOT;
			spin_lock_init(&rtpz->lock);
		}
	}
}

static struct cgroup_subsys_state * __ref
mem_cgroup_css_alloc(struct cgroup_subsys_state *parent_css)
{
	struct mem_cgroup *memcg;
	long error = -ENOMEM;
	int node;

	memcg = mem_cgroup_alloc();
	if (!memcg)
		return ERR_PTR(error);

	for_each_node(node)
		if (alloc_mem_cgroup_per_zone_info(memcg, node))
			goto free_out;

	/* root ? */
	if (parent_css == NULL) {
		root_mem_cgroup = memcg;
		res_counter_init(&memcg->res, NULL);
		res_counter_init(&memcg->memsw, NULL);
		res_counter_init(&memcg->kmem, NULL);
	}

	memcg->last_scanned_node = MAX_NUMNODES;
	INIT_LIST_HEAD(&memcg->oom_notify);
	memcg->move_charge_at_immigrate = 0;
=======
	memcg->id.id = idr_alloc(&mem_cgroup_idr, NULL,
				 1, MEM_CGROUP_ID_MAX,
				 GFP_KERNEL);
	if (memcg->id.id < 0)
		goto fail;

	memcg->stat = alloc_percpu(struct mem_cgroup_stat_cpu);
	if (!memcg->stat)
		goto fail;

	for_each_node(node)
		if (alloc_mem_cgroup_per_node_info(memcg, node))
			goto fail;

	if (memcg_wb_domain_init(memcg, GFP_KERNEL))
		goto fail;

	INIT_WORK(&memcg->high_work, high_work_func);
	memcg->last_scanned_node = MAX_NUMNODES;
	INIT_LIST_HEAD(&memcg->oom_notify);
>>>>>>> v4.9.227
	mutex_init(&memcg->thresholds_lock);
	spin_lock_init(&memcg->move_lock);
	vmpressure_init(&memcg->vmpressure);
	INIT_LIST_HEAD(&memcg->event_list);
	spin_lock_init(&memcg->event_list_lock);
<<<<<<< HEAD

	return &memcg->css;

free_out:
	__mem_cgroup_free(memcg);
	return ERR_PTR(error);
}

static int
mem_cgroup_css_online(struct cgroup_subsys_state *css)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);
	struct mem_cgroup *parent = mem_cgroup_from_css(css->parent);
	int ret;

	if (css->id > MEM_CGROUP_ID_MAX)
		return -ENOSPC;

	if (!parent)
		return 0;

	mutex_lock(&memcg_create_mutex);

	memcg->use_hierarchy = parent->use_hierarchy;
	memcg->oom_kill_disable = parent->oom_kill_disable;
	memcg->swappiness = mem_cgroup_swappiness(parent);

	if (parent->use_hierarchy) {
		res_counter_init(&memcg->res, &parent->res);
		res_counter_init(&memcg->memsw, &parent->memsw);
		res_counter_init(&memcg->kmem, &parent->kmem);

		/*
		 * No need to take a reference to the parent because cgroup
		 * core guarantees its existence.
		 */
	} else {
		res_counter_init(&memcg->res, NULL);
		res_counter_init(&memcg->memsw, NULL);
		res_counter_init(&memcg->kmem, NULL);
=======
	memcg->socket_pressure = jiffies;
#ifndef CONFIG_SLOB
	memcg->kmemcg_id = -1;
#endif
#ifdef CONFIG_CGROUP_WRITEBACK
	INIT_LIST_HEAD(&memcg->cgwb_list);
#endif
	idr_replace(&mem_cgroup_idr, memcg, memcg->id.id);
	return memcg;
fail:
	mem_cgroup_id_remove(memcg);
	__mem_cgroup_free(memcg);
	return NULL;
}

static struct cgroup_subsys_state * __ref
mem_cgroup_css_alloc(struct cgroup_subsys_state *parent_css)
{
	struct mem_cgroup *parent = mem_cgroup_from_css(parent_css);
	struct mem_cgroup *memcg;
	long error = -ENOMEM;

	memcg = mem_cgroup_alloc();
	if (!memcg)
		return ERR_PTR(error);

	memcg->high = PAGE_COUNTER_MAX;
	memcg->soft_limit = PAGE_COUNTER_MAX;
	if (parent) {
		memcg->swappiness = mem_cgroup_swappiness(parent);
		memcg->oom_kill_disable = parent->oom_kill_disable;
	}
	if (parent && parent->use_hierarchy) {
		memcg->use_hierarchy = true;
		page_counter_init(&memcg->memory, &parent->memory);
		page_counter_init(&memcg->swap, &parent->swap);
		page_counter_init(&memcg->memsw, &parent->memsw);
		page_counter_init(&memcg->kmem, &parent->kmem);
		page_counter_init(&memcg->tcpmem, &parent->tcpmem);
	} else {
		page_counter_init(&memcg->memory, NULL);
		page_counter_init(&memcg->swap, NULL);
		page_counter_init(&memcg->memsw, NULL);
		page_counter_init(&memcg->kmem, NULL);
		page_counter_init(&memcg->tcpmem, NULL);
>>>>>>> v4.9.227
		/*
		 * Deeper hierachy with use_hierarchy == false doesn't make
		 * much sense so let cgroup subsystem know about this
		 * unfortunate state in our controller.
		 */
		if (parent != root_mem_cgroup)
			memory_cgrp_subsys.broken_hierarchy = true;
	}
<<<<<<< HEAD
	mutex_unlock(&memcg_create_mutex);

	ret = memcg_init_kmem(memcg, &memory_cgrp_subsys);
	if (ret)
		return ret;

	/*
	 * Make sure the memcg is initialized: mem_cgroup_iter()
	 * orders reading memcg->initialized against its callers
	 * reading the memcg members.
	 */
	smp_store_release(&memcg->initialized, 1);

	return 0;
}

/*
 * Announce all parents that a group from their hierarchy is gone.
 */
static void mem_cgroup_invalidate_reclaim_iterators(struct mem_cgroup *memcg)
{
	struct mem_cgroup *parent = memcg;

	while ((parent = parent_mem_cgroup(parent)))
		mem_cgroup_iter_invalidate(parent);

	/*
	 * if the root memcg is not hierarchical we have to check it
	 * explicitely.
	 */
	if (!root_mem_cgroup->use_hierarchy)
		mem_cgroup_iter_invalidate(root_mem_cgroup);
=======

	/* The following stuff does not apply to the root */
	if (!parent) {
		root_mem_cgroup = memcg;
		return &memcg->css;
	}

	error = memcg_online_kmem(memcg);
	if (error)
		goto fail;

	if (cgroup_subsys_on_dfl(memory_cgrp_subsys) && !cgroup_memory_nosocket)
		static_branch_inc(&memcg_sockets_enabled_key);

	return &memcg->css;
fail:
	mem_cgroup_id_remove(memcg);
	mem_cgroup_free(memcg);
	return ERR_PTR(-ENOMEM);
}

static int mem_cgroup_css_online(struct cgroup_subsys_state *css)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);

	/* Online state pins memcg ID, memcg ID pins CSS */
	atomic_set(&memcg->id.ref, 1);
	css_get(css);
	return 0;
>>>>>>> v4.9.227
}

static void mem_cgroup_css_offline(struct cgroup_subsys_state *css)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);
	struct mem_cgroup_event *event, *tmp;
<<<<<<< HEAD
	struct cgroup_subsys_state *iter;
=======
>>>>>>> v4.9.227

	/*
	 * Unregister events and notify userspace.
	 * Notify userspace about cgroup removing only after rmdir of cgroup
	 * directory to avoid race between userspace and kernelspace.
	 */
	spin_lock(&memcg->event_list_lock);
	list_for_each_entry_safe(event, tmp, &memcg->event_list, list) {
		list_del_init(&event->list);
		schedule_work(&event->remove);
	}
	spin_unlock(&memcg->event_list_lock);

<<<<<<< HEAD
	kmem_cgroup_css_offline(memcg);

	mem_cgroup_invalidate_reclaim_iterators(memcg);

	/*
	 * This requires that offlining is serialized.  Right now that is
	 * guaranteed because css_killed_work_fn() holds the cgroup_mutex.
	 */
	css_for_each_descendant_post(iter, css)
		mem_cgroup_reparent_charges(mem_cgroup_from_css(iter));

	memcg_unregister_all_caches(memcg);
	vmpressure_cleanup(&memcg->vmpressure);
=======
	memcg_offline_kmem(memcg);
	wb_memcg_offline(memcg);

	mem_cgroup_id_put(memcg);
}

static void mem_cgroup_css_released(struct cgroup_subsys_state *css)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);

	invalidate_reclaim_iterators(memcg);
>>>>>>> v4.9.227
}

static void mem_cgroup_css_free(struct cgroup_subsys_state *css)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);
<<<<<<< HEAD
	/*
	 * XXX: css_offline() would be where we should reparent all
	 * memory to prepare the cgroup for destruction.  However,
	 * memcg does not do css_tryget_online() and res_counter charging
	 * under the same RCU lock region, which means that charging
	 * could race with offlining.  Offlining only happens to
	 * cgroups with no tasks in them but charges can show up
	 * without any tasks from the swapin path when the target
	 * memcg is looked up from the swapout record and not from the
	 * current task as it usually is.  A race like this can leak
	 * charges and put pages with stale cgroup pointers into
	 * circulation:
	 *
	 * #0                        #1
	 *                           lookup_swap_cgroup_id()
	 *                           rcu_read_lock()
	 *                           mem_cgroup_lookup()
	 *                           css_tryget_online()
	 *                           rcu_read_unlock()
	 * disable css_tryget_online()
	 * call_rcu()
	 *   offline_css()
	 *     reparent_charges()
	 *                           res_counter_charge()
	 *                           css_put()
	 *                             css_free()
	 *                           pc->mem_cgroup = dead memcg
	 *                           add page to lru
	 *
	 * The bulk of the charges are still moved in offline_css() to
	 * avoid pinning a lot of pages in case a long-term reference
	 * like a swapout record is deferring the css_free() to long
	 * after offlining.  But this makes sure we catch any charges
	 * made after offlining:
	 */
	mem_cgroup_reparent_charges(memcg);

	memcg_destroy_kmem(memcg);
	__mem_cgroup_free(memcg);
=======

	if (cgroup_subsys_on_dfl(memory_cgrp_subsys) && !cgroup_memory_nosocket)
		static_branch_dec(&memcg_sockets_enabled_key);

	if (!cgroup_subsys_on_dfl(memory_cgrp_subsys) && memcg->tcpmem_active)
		static_branch_dec(&memcg_sockets_enabled_key);

	vmpressure_cleanup(&memcg->vmpressure);
	cancel_work_sync(&memcg->high_work);
	mem_cgroup_remove_from_trees(memcg);
	memcg_free_kmem(memcg);
	mem_cgroup_free(memcg);
>>>>>>> v4.9.227
}

/**
 * mem_cgroup_css_reset - reset the states of a mem_cgroup
 * @css: the target css
 *
 * Reset the states of the mem_cgroup associated with @css.  This is
 * invoked when the userland requests disabling on the default hierarchy
 * but the memcg is pinned through dependency.  The memcg should stop
 * applying policies and should revert to the vanilla state as it may be
 * made visible again.
 *
 * The current implementation only resets the essential configurations.
 * This needs to be expanded to cover all the visible parts.
 */
static void mem_cgroup_css_reset(struct cgroup_subsys_state *css)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);

<<<<<<< HEAD
	mem_cgroup_resize_limit(memcg, ULLONG_MAX);
	mem_cgroup_resize_memsw_limit(memcg, ULLONG_MAX);
	memcg_update_kmem_limit(memcg, ULLONG_MAX);
	res_counter_set_soft_limit(&memcg->res, ULLONG_MAX);
=======
	page_counter_limit(&memcg->memory, PAGE_COUNTER_MAX);
	page_counter_limit(&memcg->swap, PAGE_COUNTER_MAX);
	page_counter_limit(&memcg->memsw, PAGE_COUNTER_MAX);
	page_counter_limit(&memcg->kmem, PAGE_COUNTER_MAX);
	page_counter_limit(&memcg->tcpmem, PAGE_COUNTER_MAX);
	memcg->low = 0;
	memcg->high = PAGE_COUNTER_MAX;
	memcg->soft_limit = PAGE_COUNTER_MAX;
	memcg_wb_domain_size_changed(memcg);
>>>>>>> v4.9.227
}

#ifdef CONFIG_MMU
/* Handlers for move charge at task migration. */
static int mem_cgroup_do_precharge(unsigned long count)
{
	int ret;

<<<<<<< HEAD
	/* Try a single bulk charge without reclaim first */
	ret = try_charge(mc.to, GFP_KERNEL & ~__GFP_WAIT, count);
=======
	/* Try a single bulk charge without reclaim first, kswapd may wake */
	ret = try_charge(mc.to, GFP_KERNEL & ~__GFP_DIRECT_RECLAIM, count);
>>>>>>> v4.9.227
	if (!ret) {
		mc.precharge += count;
		return ret;
	}
<<<<<<< HEAD
	if (ret == -EINTR) {
		cancel_charge(root_mem_cgroup, count);
		return ret;
	}

	/* Try charges one by one with reclaim */
	while (count--) {
		ret = try_charge(mc.to, GFP_KERNEL & ~__GFP_NORETRY, 1);
		/*
		 * In case of failure, any residual charges against
		 * mc.to will be dropped by mem_cgroup_clear_mc()
		 * later on.  However, cancel any charges that are
		 * bypassed to root right away or they'll be lost.
		 */
		if (ret == -EINTR)
			cancel_charge(root_mem_cgroup, 1);
=======

	/* Try charges one by one with reclaim, but do not retry */
	while (count--) {
		ret = try_charge(mc.to, GFP_KERNEL | __GFP_NORETRY, 1);
>>>>>>> v4.9.227
		if (ret)
			return ret;
		mc.precharge++;
		cond_resched();
	}
	return 0;
}

<<<<<<< HEAD
=======
union mc_target {
	struct page	*page;
	swp_entry_t	ent;
};

enum mc_target_type {
	MC_TARGET_NONE = 0,
	MC_TARGET_PAGE,
	MC_TARGET_SWAP,
};

static struct page *mc_handle_present_pte(struct vm_area_struct *vma,
						unsigned long addr, pte_t ptent)
{
	struct page *page = vm_normal_page(vma, addr, ptent);

	if (!page || !page_mapped(page))
		return NULL;
	if (PageAnon(page)) {
		if (!(mc.flags & MOVE_ANON))
			return NULL;
	} else {
		if (!(mc.flags & MOVE_FILE))
			return NULL;
	}
	if (!get_page_unless_zero(page))
		return NULL;

	return page;
}

#ifdef CONFIG_SWAP
static struct page *mc_handle_swap_pte(struct vm_area_struct *vma,
			pte_t ptent, swp_entry_t *entry)
{
	struct page *page = NULL;
	swp_entry_t ent = pte_to_swp_entry(ptent);

	if (!(mc.flags & MOVE_ANON) || non_swap_entry(ent))
		return NULL;
	/*
	 * Because lookup_swap_cache() updates some statistics counter,
	 * we call find_get_page() with swapper_space directly.
	 */
	page = find_get_page(swap_address_space(ent), swp_offset(ent));
	if (do_memsw_account())
		entry->val = ent.val;

	return page;
}
#else
static struct page *mc_handle_swap_pte(struct vm_area_struct *vma,
			pte_t ptent, swp_entry_t *entry)
{
	return NULL;
}
#endif

static struct page *mc_handle_file_pte(struct vm_area_struct *vma,
			unsigned long addr, pte_t ptent, swp_entry_t *entry)
{
	struct page *page = NULL;
	struct address_space *mapping;
	pgoff_t pgoff;

	if (!vma->vm_file) /* anonymous vma */
		return NULL;
	if (!(mc.flags & MOVE_FILE))
		return NULL;

	mapping = vma->vm_file->f_mapping;
	pgoff = linear_page_index(vma, addr);

	/* page is moved even if it's not RSS of this task(page-faulted). */
#ifdef CONFIG_SWAP
	/* shmem/tmpfs may report page out on swap: account for that too. */
	if (shmem_mapping(mapping)) {
		page = find_get_entry(mapping, pgoff);
		if (radix_tree_exceptional_entry(page)) {
			swp_entry_t swp = radix_to_swp_entry(page);
			if (do_memsw_account())
				*entry = swp;
			page = find_get_page(swap_address_space(swp),
					     swp_offset(swp));
		}
	} else
		page = find_get_page(mapping, pgoff);
#else
	page = find_get_page(mapping, pgoff);
#endif
	return page;
}

/**
 * mem_cgroup_move_account - move account of the page
 * @page: the page
 * @compound: charge the page as compound or small page
 * @from: mem_cgroup which the page is moved from.
 * @to:	mem_cgroup which the page is moved to. @from != @to.
 *
 * The caller must make sure the page is not on LRU (isolate_page() is useful.)
 *
 * This function doesn't do "charge" to new cgroup and doesn't do "uncharge"
 * from old cgroup.
 */
static int mem_cgroup_move_account(struct page *page,
				   bool compound,
				   struct mem_cgroup *from,
				   struct mem_cgroup *to)
{
	unsigned long flags;
	unsigned int nr_pages = compound ? hpage_nr_pages(page) : 1;
	int ret;
	bool anon;

	VM_BUG_ON(from == to);
	VM_BUG_ON_PAGE(PageLRU(page), page);
	VM_BUG_ON(compound && !PageTransHuge(page));

	/*
	 * Prevent mem_cgroup_migrate() from looking at
	 * page->mem_cgroup of its source page while we change it.
	 */
	ret = -EBUSY;
	if (!trylock_page(page))
		goto out;

	ret = -EINVAL;
	if (page->mem_cgroup != from)
		goto out_unlock;

	anon = PageAnon(page);

	spin_lock_irqsave(&from->move_lock, flags);

	if (!anon && page_mapped(page)) {
		__this_cpu_sub(from->stat->count[MEM_CGROUP_STAT_FILE_MAPPED],
			       nr_pages);
		__this_cpu_add(to->stat->count[MEM_CGROUP_STAT_FILE_MAPPED],
			       nr_pages);
	}

	/*
	 * move_lock grabbed above and caller set from->moving_account, so
	 * mem_cgroup_update_page_stat() will serialize updates to PageDirty.
	 * So mapping should be stable for dirty pages.
	 */
	if (!anon && PageDirty(page)) {
		struct address_space *mapping = page_mapping(page);

		if (mapping_cap_account_dirty(mapping)) {
			__this_cpu_sub(from->stat->count[MEM_CGROUP_STAT_DIRTY],
				       nr_pages);
			__this_cpu_add(to->stat->count[MEM_CGROUP_STAT_DIRTY],
				       nr_pages);
		}
	}

	if (PageWriteback(page)) {
		__this_cpu_sub(from->stat->count[MEM_CGROUP_STAT_WRITEBACK],
			       nr_pages);
		__this_cpu_add(to->stat->count[MEM_CGROUP_STAT_WRITEBACK],
			       nr_pages);
	}

	/*
	 * It is safe to change page->mem_cgroup here because the page
	 * is referenced, charged, and isolated - we can't race with
	 * uncharging, charging, migration, or LRU putback.
	 */

	/* caller should have done css_get */
	page->mem_cgroup = to;
	spin_unlock_irqrestore(&from->move_lock, flags);

	ret = 0;

	local_irq_disable();
	mem_cgroup_charge_statistics(to, page, compound, nr_pages);
	memcg_check_events(to, page);
	mem_cgroup_charge_statistics(from, page, compound, -nr_pages);
	memcg_check_events(from, page);
	local_irq_enable();
out_unlock:
	unlock_page(page);
out:
	return ret;
}

>>>>>>> v4.9.227
/**
 * get_mctgt_type - get target type of moving charge
 * @vma: the vma the pte to be checked belongs
 * @addr: the address corresponding to the pte to be checked
 * @ptent: the pte to be checked
 * @target: the pointer the target page or swap ent will be stored(can be NULL)
 *
 * Returns
 *   0(MC_TARGET_NONE): if the pte is not a target for move charge.
 *   1(MC_TARGET_PAGE): if the page corresponding to this pte is a target for
 *     move charge. if @target is not NULL, the page is stored in target->page
 *     with extra refcnt got(Callers should handle it).
 *   2(MC_TARGET_SWAP): if the swap entry corresponding to this pte is a
 *     target for charge migration. if @target is not NULL, the entry is stored
 *     in target->ent.
 *
 * Called with pte lock held.
 */
<<<<<<< HEAD
union mc_target {
	struct page	*page;
	swp_entry_t	ent;
};

enum mc_target_type {
	MC_TARGET_NONE = 0,
	MC_TARGET_PAGE,
	MC_TARGET_SWAP,
};

static struct page *mc_handle_present_pte(struct vm_area_struct *vma,
						unsigned long addr, pte_t ptent)
{
	struct page *page = vm_normal_page(vma, addr, ptent);

	if (!page || !page_mapped(page))
		return NULL;
	if (PageAnon(page)) {
		/* we don't move shared anon */
		if (!move_anon())
			return NULL;
	} else if (!move_file())
		/* we ignore mapcount for file pages */
		return NULL;
	if (!get_page_unless_zero(page))
		return NULL;

	return page;
}

#ifdef CONFIG_SWAP
static struct page *mc_handle_swap_pte(struct vm_area_struct *vma,
			unsigned long addr, pte_t ptent, swp_entry_t *entry)
{
	struct page *page = NULL;
	swp_entry_t ent = pte_to_swp_entry(ptent);

	if (!move_anon() || non_swap_entry(ent))
		return NULL;
	/*
	 * Because lookup_swap_cache() updates some statistics counter,
	 * we call find_get_page() with swapper_space directly.
	 */
	page = find_get_page(swap_address_space(ent), ent.val);
	if (do_swap_account)
		entry->val = ent.val;

	return page;
}
#else
static struct page *mc_handle_swap_pte(struct vm_area_struct *vma,
			unsigned long addr, pte_t ptent, swp_entry_t *entry)
{
	return NULL;
}
#endif

static struct page *mc_handle_file_pte(struct vm_area_struct *vma,
			unsigned long addr, pte_t ptent, swp_entry_t *entry)
{
	struct page *page = NULL;
	struct address_space *mapping;
	pgoff_t pgoff;

	if (!vma->vm_file) /* anonymous vma */
		return NULL;
	if (!move_file())
		return NULL;

	mapping = vma->vm_file->f_mapping;
	if (pte_none(ptent))
		pgoff = linear_page_index(vma, addr);
	else /* pte_file(ptent) is true */
		pgoff = pte_to_pgoff(ptent);

	/* page is moved even if it's not RSS of this task(page-faulted). */
#ifdef CONFIG_SWAP
	/* shmem/tmpfs may report page out on swap: account for that too. */
	if (shmem_mapping(mapping)) {
		page = find_get_entry(mapping, pgoff);
		if (radix_tree_exceptional_entry(page)) {
			swp_entry_t swp = radix_to_swp_entry(page);
			if (do_swap_account)
				*entry = swp;
			page = find_get_page(swap_address_space(swp), swp.val);
		}
	} else
		page = find_get_page(mapping, pgoff);
#else
	page = find_get_page(mapping, pgoff);
#endif
	return page;
}
=======
>>>>>>> v4.9.227

static enum mc_target_type get_mctgt_type(struct vm_area_struct *vma,
		unsigned long addr, pte_t ptent, union mc_target *target)
{
	struct page *page = NULL;
<<<<<<< HEAD
	struct page_cgroup *pc;
=======
>>>>>>> v4.9.227
	enum mc_target_type ret = MC_TARGET_NONE;
	swp_entry_t ent = { .val = 0 };

	if (pte_present(ptent))
		page = mc_handle_present_pte(vma, addr, ptent);
	else if (is_swap_pte(ptent))
<<<<<<< HEAD
		page = mc_handle_swap_pte(vma, addr, ptent, &ent);
	else if (pte_none(ptent) || pte_file(ptent))
=======
		page = mc_handle_swap_pte(vma, ptent, &ent);
	else if (pte_none(ptent))
>>>>>>> v4.9.227
		page = mc_handle_file_pte(vma, addr, ptent, &ent);

	if (!page && !ent.val)
		return ret;
	if (page) {
<<<<<<< HEAD
		pc = lookup_page_cgroup(page);
		/*
		 * Do only loose check w/o serialization.
		 * mem_cgroup_move_account() checks the pc is valid or
		 * not under LRU exclusion.
		 */
		if (PageCgroupUsed(pc) && pc->mem_cgroup == mc.from) {
=======
		/*
		 * Do only loose check w/o serialization.
		 * mem_cgroup_move_account() checks the page is valid or
		 * not under LRU exclusion.
		 */
		if (page->mem_cgroup == mc.from) {
>>>>>>> v4.9.227
			ret = MC_TARGET_PAGE;
			if (target)
				target->page = page;
		}
		if (!ret || !target)
			put_page(page);
	}
	/* There is a swap entry and a page doesn't exist or isn't charged */
	if (ent.val && !ret &&
	    mem_cgroup_id(mc.from) == lookup_swap_cgroup_id(ent)) {
		ret = MC_TARGET_SWAP;
		if (target)
			target->ent = ent;
	}
	return ret;
}

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
/*
 * We don't consider swapping or file mapped pages because THP does not
 * support them for now.
 * Caller should make sure that pmd_trans_huge(pmd) is true.
 */
static enum mc_target_type get_mctgt_type_thp(struct vm_area_struct *vma,
		unsigned long addr, pmd_t pmd, union mc_target *target)
{
	struct page *page = NULL;
<<<<<<< HEAD
	struct page_cgroup *pc;
=======
>>>>>>> v4.9.227
	enum mc_target_type ret = MC_TARGET_NONE;

	page = pmd_page(pmd);
	VM_BUG_ON_PAGE(!page || !PageHead(page), page);
<<<<<<< HEAD
	if (!move_anon())
		return ret;
	pc = lookup_page_cgroup(page);
	if (PageCgroupUsed(pc) && pc->mem_cgroup == mc.from) {
=======
	if (!(mc.flags & MOVE_ANON))
		return ret;
	if (page->mem_cgroup == mc.from) {
>>>>>>> v4.9.227
		ret = MC_TARGET_PAGE;
		if (target) {
			get_page(page);
			target->page = page;
		}
	}
	return ret;
}
#else
static inline enum mc_target_type get_mctgt_type_thp(struct vm_area_struct *vma,
		unsigned long addr, pmd_t pmd, union mc_target *target)
{
	return MC_TARGET_NONE;
}
#endif

static int mem_cgroup_count_precharge_pte_range(pmd_t *pmd,
					unsigned long addr, unsigned long end,
					struct mm_walk *walk)
{
<<<<<<< HEAD
	struct vm_area_struct *vma = walk->private;
	pte_t *pte;
	spinlock_t *ptl;

	if (pmd_trans_huge_lock(pmd, vma, &ptl) == 1) {
=======
	struct vm_area_struct *vma = walk->vma;
	pte_t *pte;
	spinlock_t *ptl;

	ptl = pmd_trans_huge_lock(pmd, vma);
	if (ptl) {
>>>>>>> v4.9.227
		if (get_mctgt_type_thp(vma, addr, *pmd, NULL) == MC_TARGET_PAGE)
			mc.precharge += HPAGE_PMD_NR;
		spin_unlock(ptl);
		return 0;
	}

	if (pmd_trans_unstable(pmd))
		return 0;
	pte = pte_offset_map_lock(vma->vm_mm, pmd, addr, &ptl);
	for (; addr != end; pte++, addr += PAGE_SIZE)
		if (get_mctgt_type(vma, addr, *pte, NULL))
			mc.precharge++;	/* increment precharge temporarily */
	pte_unmap_unlock(pte - 1, ptl);
	cond_resched();

	return 0;
}

static unsigned long mem_cgroup_count_precharge(struct mm_struct *mm)
{
	unsigned long precharge;
<<<<<<< HEAD
	struct vm_area_struct *vma;

	down_read(&mm->mmap_sem);
	for (vma = mm->mmap; vma; vma = vma->vm_next) {
		struct mm_walk mem_cgroup_count_precharge_walk = {
			.pmd_entry = mem_cgroup_count_precharge_pte_range,
			.mm = mm,
			.private = vma,
		};
		if (is_vm_hugetlb_page(vma))
			continue;
		walk_page_range(vma->vm_start, vma->vm_end,
					&mem_cgroup_count_precharge_walk);
	}
=======

	struct mm_walk mem_cgroup_count_precharge_walk = {
		.pmd_entry = mem_cgroup_count_precharge_pte_range,
		.mm = mm,
	};
	down_read(&mm->mmap_sem);
	walk_page_range(0, mm->highest_vm_end,
			&mem_cgroup_count_precharge_walk);
>>>>>>> v4.9.227
	up_read(&mm->mmap_sem);

	precharge = mc.precharge;
	mc.precharge = 0;

	return precharge;
}

static int mem_cgroup_precharge_mc(struct mm_struct *mm)
{
	unsigned long precharge = mem_cgroup_count_precharge(mm);

	VM_BUG_ON(mc.moving_task);
	mc.moving_task = current;
	return mem_cgroup_do_precharge(precharge);
}

/* cancels all extra charges on mc.from and mc.to, and wakes up all waiters. */
static void __mem_cgroup_clear_mc(void)
{
	struct mem_cgroup *from = mc.from;
	struct mem_cgroup *to = mc.to;
<<<<<<< HEAD
	int i;
=======
>>>>>>> v4.9.227

	/* we must uncharge all the leftover precharges from mc.to */
	if (mc.precharge) {
		cancel_charge(mc.to, mc.precharge);
		mc.precharge = 0;
	}
	/*
	 * we didn't uncharge from mc.from at mem_cgroup_move_account(), so
	 * we must uncharge here.
	 */
	if (mc.moved_charge) {
		cancel_charge(mc.from, mc.moved_charge);
		mc.moved_charge = 0;
	}
	/* we must fixup refcnts and charges */
	if (mc.moved_swap) {
		/* uncharge swap account from the old cgroup */
		if (!mem_cgroup_is_root(mc.from))
<<<<<<< HEAD
			res_counter_uncharge(&mc.from->memsw,
					     PAGE_SIZE * mc.moved_swap);

		for (i = 0; i < mc.moved_swap; i++)
			css_put(&mc.from->css);

		/*
		 * we charged both to->res and to->memsw, so we should
		 * uncharge to->res.
		 */
		if (!mem_cgroup_is_root(mc.to))
			res_counter_uncharge(&mc.to->res,
					     PAGE_SIZE * mc.moved_swap);
		/* we've already done css_get(mc.to) */
=======
			page_counter_uncharge(&mc.from->memsw, mc.moved_swap);

		mem_cgroup_id_put_many(mc.from, mc.moved_swap);

		/*
		 * we charged both to->memory and to->memsw, so we
		 * should uncharge to->memory.
		 */
		if (!mem_cgroup_is_root(mc.to))
			page_counter_uncharge(&mc.to->memory, mc.moved_swap);

		mem_cgroup_id_get_many(mc.to, mc.moved_swap);
		css_put_many(&mc.to->css, mc.moved_swap);

>>>>>>> v4.9.227
		mc.moved_swap = 0;
	}
	memcg_oom_recover(from);
	memcg_oom_recover(to);
	wake_up_all(&mc.waitq);
}

static void mem_cgroup_clear_mc(void)
{
<<<<<<< HEAD
	struct mem_cgroup *from = mc.from;
=======
	struct mm_struct *mm = mc.mm;
>>>>>>> v4.9.227

	/*
	 * we must clear moving_task before waking up waiters at the end of
	 * task migration.
	 */
	mc.moving_task = NULL;
	__mem_cgroup_clear_mc();
	spin_lock(&mc.lock);
	mc.from = NULL;
	mc.to = NULL;
<<<<<<< HEAD
	spin_unlock(&mc.lock);
	mem_cgroup_end_move(from);
}

static int mem_cgroup_can_attach(struct cgroup_subsys_state *css,
				 struct cgroup_taskset *tset)
{
	struct task_struct *p = cgroup_taskset_first(tset);
	int ret = 0;
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);
	unsigned long move_charge_at_immigrate;
=======
	mc.mm = NULL;
	spin_unlock(&mc.lock);

	mmput(mm);
}

static int mem_cgroup_can_attach(struct cgroup_taskset *tset)
{
	struct cgroup_subsys_state *css;
	struct mem_cgroup *memcg = NULL; /* unneeded init to make gcc happy */
	struct mem_cgroup *from;
	struct task_struct *leader, *p;
	struct mm_struct *mm;
	unsigned long move_flags;
	int ret = 0;

	/* charge immigration isn't supported on the default hierarchy */
	if (cgroup_subsys_on_dfl(memory_cgrp_subsys))
		return 0;

	/*
	 * Multi-process migrations only happen on the default hierarchy
	 * where charge immigration is not used.  Perform charge
	 * immigration if @tset contains a leader and whine if there are
	 * multiple.
	 */
	p = NULL;
	cgroup_taskset_for_each_leader(leader, css, tset) {
		WARN_ON_ONCE(p);
		p = leader;
		memcg = mem_cgroup_from_css(css);
	}
	if (!p)
		return 0;
>>>>>>> v4.9.227

	/*
	 * We are now commited to this value whatever it is. Changes in this
	 * tunable will only affect upcoming migrations, not the current one.
	 * So we need to save it, and keep it going.
	 */
<<<<<<< HEAD
	move_charge_at_immigrate  = memcg->move_charge_at_immigrate;
	if (move_charge_at_immigrate) {
		struct mm_struct *mm;
		struct mem_cgroup *from = mem_cgroup_from_task(p);

		VM_BUG_ON(from == memcg);

		mm = get_task_mm(p);
		if (!mm)
			return 0;
		/* We move charges only when we move a owner of the mm */
		if (mm->owner == p) {
			VM_BUG_ON(mc.from);
			VM_BUG_ON(mc.to);
			VM_BUG_ON(mc.precharge);
			VM_BUG_ON(mc.moved_charge);
			VM_BUG_ON(mc.moved_swap);
			mem_cgroup_start_move(from);
			spin_lock(&mc.lock);
			mc.from = from;
			mc.to = memcg;
			mc.immigrate_flags = move_charge_at_immigrate;
			spin_unlock(&mc.lock);
			/* We set mc.moving_task later */

			ret = mem_cgroup_precharge_mc(mm);
			if (ret)
				mem_cgroup_clear_mc();
		}
=======
	move_flags = READ_ONCE(memcg->move_charge_at_immigrate);
	if (!move_flags)
		return 0;

	from = mem_cgroup_from_task(p);

	VM_BUG_ON(from == memcg);

	mm = get_task_mm(p);
	if (!mm)
		return 0;
	/* We move charges only when we move a owner of the mm */
	if (mm->owner == p) {
		VM_BUG_ON(mc.from);
		VM_BUG_ON(mc.to);
		VM_BUG_ON(mc.precharge);
		VM_BUG_ON(mc.moved_charge);
		VM_BUG_ON(mc.moved_swap);

		spin_lock(&mc.lock);
		mc.mm = mm;
		mc.from = from;
		mc.to = memcg;
		mc.flags = move_flags;
		spin_unlock(&mc.lock);
		/* We set mc.moving_task later */

		ret = mem_cgroup_precharge_mc(mm);
		if (ret)
			mem_cgroup_clear_mc();
	} else {
>>>>>>> v4.9.227
		mmput(mm);
	}
	return ret;
}

<<<<<<< HEAD
static void mem_cgroup_cancel_attach(struct cgroup_subsys_state *css,
				     struct cgroup_taskset *tset)
{
	mem_cgroup_clear_mc();
=======
static void mem_cgroup_cancel_attach(struct cgroup_taskset *tset)
{
	if (mc.to)
		mem_cgroup_clear_mc();
>>>>>>> v4.9.227
}

static int mem_cgroup_move_charge_pte_range(pmd_t *pmd,
				unsigned long addr, unsigned long end,
				struct mm_walk *walk)
{
	int ret = 0;
<<<<<<< HEAD
	struct vm_area_struct *vma = walk->private;
=======
	struct vm_area_struct *vma = walk->vma;
>>>>>>> v4.9.227
	pte_t *pte;
	spinlock_t *ptl;
	enum mc_target_type target_type;
	union mc_target target;
	struct page *page;
<<<<<<< HEAD
	struct page_cgroup *pc;

	/*
	 * We don't take compound_lock() here but no race with splitting thp
	 * happens because:
	 *  - if pmd_trans_huge_lock() returns 1, the relevant thp is not
	 *    under splitting, which means there's no concurrent thp split,
	 *  - if another thread runs into split_huge_page() just after we
	 *    entered this if-block, the thread must wait for page table lock
	 *    to be unlocked in __split_huge_page_splitting(), where the main
	 *    part of thp split is not executed yet.
	 */
	if (pmd_trans_huge_lock(pmd, vma, &ptl) == 1) {
=======

	ptl = pmd_trans_huge_lock(pmd, vma);
	if (ptl) {
>>>>>>> v4.9.227
		if (mc.precharge < HPAGE_PMD_NR) {
			spin_unlock(ptl);
			return 0;
		}
		target_type = get_mctgt_type_thp(vma, addr, *pmd, &target);
		if (target_type == MC_TARGET_PAGE) {
			page = target.page;
			if (!isolate_lru_page(page)) {
<<<<<<< HEAD
				pc = lookup_page_cgroup(page);
				if (!mem_cgroup_move_account(page, HPAGE_PMD_NR,
							pc, mc.from, mc.to)) {
=======
				if (!mem_cgroup_move_account(page, true,
							     mc.from, mc.to)) {
>>>>>>> v4.9.227
					mc.precharge -= HPAGE_PMD_NR;
					mc.moved_charge += HPAGE_PMD_NR;
				}
				putback_lru_page(page);
			}
			put_page(page);
		}
		spin_unlock(ptl);
		return 0;
	}

	if (pmd_trans_unstable(pmd))
		return 0;
retry:
	pte = pte_offset_map_lock(vma->vm_mm, pmd, addr, &ptl);
	for (; addr != end; addr += PAGE_SIZE) {
		pte_t ptent = *(pte++);
		swp_entry_t ent;

		if (!mc.precharge)
			break;

		switch (get_mctgt_type(vma, addr, ptent, &target)) {
		case MC_TARGET_PAGE:
			page = target.page;
<<<<<<< HEAD
			if (isolate_lru_page(page))
				goto put;
			pc = lookup_page_cgroup(page);
			if (!mem_cgroup_move_account(page, 1, pc,
						     mc.from, mc.to)) {
=======
			/*
			 * We can have a part of the split pmd here. Moving it
			 * can be done but it would be too convoluted so simply
			 * ignore such a partial THP and keep it in original
			 * memcg. There should be somebody mapping the head.
			 */
			if (PageTransCompound(page))
				goto put;
			if (isolate_lru_page(page))
				goto put;
			if (!mem_cgroup_move_account(page, false,
						mc.from, mc.to)) {
>>>>>>> v4.9.227
				mc.precharge--;
				/* we uncharge from mc.from later. */
				mc.moved_charge++;
			}
			putback_lru_page(page);
put:			/* get_mctgt_type() gets the page */
			put_page(page);
			break;
		case MC_TARGET_SWAP:
			ent = target.ent;
			if (!mem_cgroup_move_swap_account(ent, mc.from, mc.to)) {
				mc.precharge--;
				/* we fixup refcnts and charges later. */
				mc.moved_swap++;
			}
			break;
		default:
			break;
		}
	}
	pte_unmap_unlock(pte - 1, ptl);
	cond_resched();

	if (addr != end) {
		/*
		 * We have consumed all precharges we got in can_attach().
		 * We try charge one by one, but don't do any additional
		 * charges to mc.to if we have failed in charge once in attach()
		 * phase.
		 */
		ret = mem_cgroup_do_precharge(1);
		if (!ret)
			goto retry;
	}

	return ret;
}

<<<<<<< HEAD
static void mem_cgroup_move_charge(struct mm_struct *mm)
{
	struct vm_area_struct *vma;

	lru_add_drain_all();
retry:
	if (unlikely(!down_read_trylock(&mm->mmap_sem))) {
=======
static void mem_cgroup_move_charge(void)
{
	struct mm_walk mem_cgroup_move_charge_walk = {
		.pmd_entry = mem_cgroup_move_charge_pte_range,
		.mm = mc.mm,
	};

	lru_add_drain_all();
	/*
	 * Signal lock_page_memcg() to take the memcg's move_lock
	 * while we're moving its pages to another memcg. Then wait
	 * for already started RCU-only updates to finish.
	 */
	atomic_inc(&mc.from->moving_account);
	synchronize_rcu();
retry:
	if (unlikely(!down_read_trylock(&mc.mm->mmap_sem))) {
>>>>>>> v4.9.227
		/*
		 * Someone who are holding the mmap_sem might be waiting in
		 * waitq. So we cancel all extra charges, wake up all waiters,
		 * and retry. Because we cancel precharges, we might not be able
		 * to move enough charges, but moving charge is a best-effort
		 * feature anyway, so it wouldn't be a big problem.
		 */
		__mem_cgroup_clear_mc();
		cond_resched();
		goto retry;
	}
<<<<<<< HEAD
	for (vma = mm->mmap; vma; vma = vma->vm_next) {
		int ret;
		struct mm_walk mem_cgroup_move_charge_walk = {
			.pmd_entry = mem_cgroup_move_charge_pte_range,
			.mm = mm,
			.private = vma,
		};
		if (is_vm_hugetlb_page(vma))
			continue;
		ret = walk_page_range(vma->vm_start, vma->vm_end,
						&mem_cgroup_move_charge_walk);
		if (ret)
			/*
			 * means we have consumed all precharges and failed in
			 * doing additional charge. Just abandon here.
			 */
			break;
	}
	up_read(&mm->mmap_sem);
}

static void mem_cgroup_move_task(struct cgroup_subsys_state *css,
				 struct cgroup_taskset *tset)
{
	struct task_struct *p = cgroup_taskset_first(tset);
	struct mm_struct *mm = get_task_mm(p);

	if (mm) {
		if (mc.to)
			mem_cgroup_move_charge(mm);
		mmput(mm);
	}
	if (mc.to)
		mem_cgroup_clear_mc();
}
#else	/* !CONFIG_MMU */
static int mem_cgroup_can_attach(struct cgroup_subsys_state *css,
				 struct cgroup_taskset *tset)
{
	return 0;
}
static void mem_cgroup_cancel_attach(struct cgroup_subsys_state *css,
				     struct cgroup_taskset *tset)
{
}
static void mem_cgroup_move_task(struct cgroup_subsys_state *css,
				 struct cgroup_taskset *tset)
=======
	/*
	 * When we have consumed all precharges and failed in doing
	 * additional charge, the page walk just aborts.
	 */
	walk_page_range(0, mc.mm->highest_vm_end, &mem_cgroup_move_charge_walk);

	up_read(&mc.mm->mmap_sem);
	atomic_dec(&mc.from->moving_account);
}

static void mem_cgroup_move_task(void)
{
	if (mc.to) {
		mem_cgroup_move_charge();
		mem_cgroup_clear_mc();
	}
}
#else	/* !CONFIG_MMU */
static int mem_cgroup_can_attach(struct cgroup_taskset *tset)
{
	return 0;
}
static void mem_cgroup_cancel_attach(struct cgroup_taskset *tset)
{
}
static void mem_cgroup_move_task(void)
>>>>>>> v4.9.227
{
}
#endif

/*
 * Cgroup retains root cgroups across [un]mount cycles making it necessary
 * to verify whether we're attached to the default hierarchy on each mount
 * attempt.
 */
static void mem_cgroup_bind(struct cgroup_subsys_state *root_css)
{
	/*
	 * use_hierarchy is forced on the default hierarchy.  cgroup core
	 * guarantees that @root doesn't have any children, so turning it
	 * on for the root memcg is enough.
	 */
<<<<<<< HEAD
	if (cgroup_on_dfl(root_css->cgroup))
		mem_cgroup_from_css(root_css)->use_hierarchy = true;
}

=======
	if (cgroup_subsys_on_dfl(memory_cgrp_subsys))
		root_mem_cgroup->use_hierarchy = true;
	else
		root_mem_cgroup->use_hierarchy = false;
}

static u64 memory_current_read(struct cgroup_subsys_state *css,
			       struct cftype *cft)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);

	return (u64)page_counter_read(&memcg->memory) * PAGE_SIZE;
}

static int memory_low_show(struct seq_file *m, void *v)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(seq_css(m));
	unsigned long low = READ_ONCE(memcg->low);

	if (low == PAGE_COUNTER_MAX)
		seq_puts(m, "max\n");
	else
		seq_printf(m, "%llu\n", (u64)low * PAGE_SIZE);

	return 0;
}

static ssize_t memory_low_write(struct kernfs_open_file *of,
				char *buf, size_t nbytes, loff_t off)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(of_css(of));
	unsigned long low;
	int err;

	buf = strstrip(buf);
	err = page_counter_memparse(buf, "max", &low);
	if (err)
		return err;

	memcg->low = low;

	return nbytes;
}

static int memory_high_show(struct seq_file *m, void *v)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(seq_css(m));
	unsigned long high = READ_ONCE(memcg->high);

	if (high == PAGE_COUNTER_MAX)
		seq_puts(m, "max\n");
	else
		seq_printf(m, "%llu\n", (u64)high * PAGE_SIZE);

	return 0;
}

static ssize_t memory_high_write(struct kernfs_open_file *of,
				 char *buf, size_t nbytes, loff_t off)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(of_css(of));
	unsigned long nr_pages;
	unsigned long high;
	int err;

	buf = strstrip(buf);
	err = page_counter_memparse(buf, "max", &high);
	if (err)
		return err;

	memcg->high = high;

	nr_pages = page_counter_read(&memcg->memory);
	if (nr_pages > high)
		try_to_free_mem_cgroup_pages(memcg, nr_pages - high,
					     GFP_KERNEL, true);

	memcg_wb_domain_size_changed(memcg);
	return nbytes;
}

static int memory_max_show(struct seq_file *m, void *v)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(seq_css(m));
	unsigned long max = READ_ONCE(memcg->memory.limit);

	if (max == PAGE_COUNTER_MAX)
		seq_puts(m, "max\n");
	else
		seq_printf(m, "%llu\n", (u64)max * PAGE_SIZE);

	return 0;
}

static ssize_t memory_max_write(struct kernfs_open_file *of,
				char *buf, size_t nbytes, loff_t off)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(of_css(of));
	unsigned int nr_reclaims = MEM_CGROUP_RECLAIM_RETRIES;
	bool drained = false;
	unsigned long max;
	int err;

	buf = strstrip(buf);
	err = page_counter_memparse(buf, "max", &max);
	if (err)
		return err;

	xchg(&memcg->memory.limit, max);

	for (;;) {
		unsigned long nr_pages = page_counter_read(&memcg->memory);

		if (nr_pages <= max)
			break;

		if (signal_pending(current)) {
			err = -EINTR;
			break;
		}

		if (!drained) {
			drain_all_stock(memcg);
			drained = true;
			continue;
		}

		if (nr_reclaims) {
			if (!try_to_free_mem_cgroup_pages(memcg, nr_pages - max,
							  GFP_KERNEL, true))
				nr_reclaims--;
			continue;
		}

		mem_cgroup_events(memcg, MEMCG_OOM, 1);
		if (!mem_cgroup_out_of_memory(memcg, GFP_KERNEL, 0))
			break;
	}

	memcg_wb_domain_size_changed(memcg);
	return nbytes;
}

static int memory_events_show(struct seq_file *m, void *v)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(seq_css(m));

	seq_printf(m, "low %lu\n", mem_cgroup_read_events(memcg, MEMCG_LOW));
	seq_printf(m, "high %lu\n", mem_cgroup_read_events(memcg, MEMCG_HIGH));
	seq_printf(m, "max %lu\n", mem_cgroup_read_events(memcg, MEMCG_MAX));
	seq_printf(m, "oom %lu\n", mem_cgroup_read_events(memcg, MEMCG_OOM));

	return 0;
}

static int memory_stat_show(struct seq_file *m, void *v)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(seq_css(m));
	unsigned long stat[MEMCG_NR_STAT];
	unsigned long events[MEMCG_NR_EVENTS];
	int i;

	/*
	 * Provide statistics on the state of the memory subsystem as
	 * well as cumulative event counters that show past behavior.
	 *
	 * This list is ordered following a combination of these gradients:
	 * 1) generic big picture -> specifics and details
	 * 2) reflecting userspace activity -> reflecting kernel heuristics
	 *
	 * Current memory state:
	 */

	tree_stat(memcg, stat);
	tree_events(memcg, events);

	seq_printf(m, "anon %llu\n",
		   (u64)stat[MEM_CGROUP_STAT_RSS] * PAGE_SIZE);
	seq_printf(m, "file %llu\n",
		   (u64)stat[MEM_CGROUP_STAT_CACHE] * PAGE_SIZE);
	seq_printf(m, "kernel_stack %llu\n",
		   (u64)stat[MEMCG_KERNEL_STACK_KB] * 1024);
	seq_printf(m, "slab %llu\n",
		   (u64)(stat[MEMCG_SLAB_RECLAIMABLE] +
			 stat[MEMCG_SLAB_UNRECLAIMABLE]) * PAGE_SIZE);
	seq_printf(m, "sock %llu\n",
		   (u64)stat[MEMCG_SOCK] * PAGE_SIZE);

	seq_printf(m, "file_mapped %llu\n",
		   (u64)stat[MEM_CGROUP_STAT_FILE_MAPPED] * PAGE_SIZE);
	seq_printf(m, "file_dirty %llu\n",
		   (u64)stat[MEM_CGROUP_STAT_DIRTY] * PAGE_SIZE);
	seq_printf(m, "file_writeback %llu\n",
		   (u64)stat[MEM_CGROUP_STAT_WRITEBACK] * PAGE_SIZE);

	for (i = 0; i < NR_LRU_LISTS; i++) {
		struct mem_cgroup *mi;
		unsigned long val = 0;

		for_each_mem_cgroup_tree(mi, memcg)
			val += mem_cgroup_nr_lru_pages(mi, BIT(i));
		seq_printf(m, "%s %llu\n",
			   mem_cgroup_lru_names[i], (u64)val * PAGE_SIZE);
	}

	seq_printf(m, "slab_reclaimable %llu\n",
		   (u64)stat[MEMCG_SLAB_RECLAIMABLE] * PAGE_SIZE);
	seq_printf(m, "slab_unreclaimable %llu\n",
		   (u64)stat[MEMCG_SLAB_UNRECLAIMABLE] * PAGE_SIZE);

	/* Accumulated memory events */

	seq_printf(m, "pgfault %lu\n",
		   events[MEM_CGROUP_EVENTS_PGFAULT]);
	seq_printf(m, "pgmajfault %lu\n",
		   events[MEM_CGROUP_EVENTS_PGMAJFAULT]);

	return 0;
}

static struct cftype memory_files[] = {
	{
		.name = "current",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_u64 = memory_current_read,
	},
	{
		.name = "low",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = memory_low_show,
		.write = memory_low_write,
	},
	{
		.name = "high",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = memory_high_show,
		.write = memory_high_write,
	},
	{
		.name = "max",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = memory_max_show,
		.write = memory_max_write,
	},
	{
		.name = "events",
		.flags = CFTYPE_NOT_ON_ROOT,
		.file_offset = offsetof(struct mem_cgroup, events_file),
		.seq_show = memory_events_show,
	},
	{
		.name = "stat",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = memory_stat_show,
	},
	{ }	/* terminate */
};

>>>>>>> v4.9.227
struct cgroup_subsys memory_cgrp_subsys = {
	.css_alloc = mem_cgroup_css_alloc,
	.css_online = mem_cgroup_css_online,
	.css_offline = mem_cgroup_css_offline,
<<<<<<< HEAD
=======
	.css_released = mem_cgroup_css_released,
>>>>>>> v4.9.227
	.css_free = mem_cgroup_css_free,
	.css_reset = mem_cgroup_css_reset,
	.can_attach = mem_cgroup_can_attach,
	.cancel_attach = mem_cgroup_cancel_attach,
<<<<<<< HEAD
	.attach = mem_cgroup_move_task,
	.bind = mem_cgroup_bind,
	.legacy_cftypes = mem_cgroup_files,
	.early_init = 0,
};

#ifdef CONFIG_MEMCG_SWAP
static int __init enable_swap_account(char *s)
{
	if (!strcmp(s, "1"))
		really_do_swap_account = 1;
	else if (!strcmp(s, "0"))
		really_do_swap_account = 0;
	return 1;
}
__setup("swapaccount=", enable_swap_account);

static void __init memsw_file_init(void)
{
	WARN_ON(cgroup_add_legacy_cftypes(&memory_cgrp_subsys,
					  memsw_cgroup_files));
}

static void __init enable_swap_cgroup(void)
{
	if (!mem_cgroup_disabled() && really_do_swap_account) {
		do_swap_account = 1;
		memsw_file_init();
	}
}

#else
static void __init enable_swap_cgroup(void)
{
}
#endif

#ifdef CONFIG_MEMCG_SWAP
/**
 * mem_cgroup_swapout - transfer a memsw charge to swap
 * @page: page whose memsw charge to transfer
 * @entry: swap entry to move the charge to
 *
 * Transfer the memsw charge of @page to @entry.
 */
void mem_cgroup_swapout(struct page *page, swp_entry_t entry)
{
	struct page_cgroup *pc;
	unsigned short oldid;

	VM_BUG_ON_PAGE(PageLRU(page), page);
	VM_BUG_ON_PAGE(page_count(page), page);

	if (!do_swap_account)
		return;

	pc = lookup_page_cgroup(page);

	/* Readahead page, never charged */
	if (!PageCgroupUsed(pc))
		return;

	VM_BUG_ON_PAGE(!(pc->flags & PCG_MEMSW), page);

	oldid = swap_cgroup_record(entry, mem_cgroup_id(pc->mem_cgroup));
	VM_BUG_ON_PAGE(oldid, page);

	pc->flags &= ~PCG_MEMSW;
	css_get(&pc->mem_cgroup->css);
	mem_cgroup_swap_statistics(pc->mem_cgroup, true);
}

/**
 * mem_cgroup_uncharge_swap - uncharge a swap entry
 * @entry: swap entry to uncharge
 *
 * Drop the memsw charge associated with @entry.
 */
void mem_cgroup_uncharge_swap(swp_entry_t entry)
{
	struct mem_cgroup *memcg;
	unsigned short id;

	if (!do_swap_account)
		return;

	id = swap_cgroup_record(entry, 0);
	rcu_read_lock();
	memcg = mem_cgroup_lookup(id);
	if (memcg) {
		if (!mem_cgroup_is_root(memcg))
			res_counter_uncharge(&memcg->memsw, PAGE_SIZE);
		mem_cgroup_swap_statistics(memcg, false);
		css_put(&memcg->css);
	}
	rcu_read_unlock();
}
#endif
=======
	.post_attach = mem_cgroup_move_task,
	.bind = mem_cgroup_bind,
	.dfl_cftypes = memory_files,
	.legacy_cftypes = mem_cgroup_legacy_files,
	.early_init = 0,
};

/**
 * mem_cgroup_low - check if memory consumption is below the normal range
 * @root: the highest ancestor to consider
 * @memcg: the memory cgroup to check
 *
 * Returns %true if memory consumption of @memcg, and that of all
 * configurable ancestors up to @root, is below the normal range.
 */
bool mem_cgroup_low(struct mem_cgroup *root, struct mem_cgroup *memcg)
{
	if (mem_cgroup_disabled())
		return false;

	/*
	 * The toplevel group doesn't have a configurable range, so
	 * it's never low when looked at directly, and it is not
	 * considered an ancestor when assessing the hierarchy.
	 */

	if (memcg == root_mem_cgroup)
		return false;

	if (page_counter_read(&memcg->memory) >= memcg->low)
		return false;

	while (memcg != root) {
		memcg = parent_mem_cgroup(memcg);

		if (memcg == root_mem_cgroup)
			break;

		if (page_counter_read(&memcg->memory) >= memcg->low)
			return false;
	}
	return true;
}
>>>>>>> v4.9.227

/**
 * mem_cgroup_try_charge - try charging a page
 * @page: page to charge
 * @mm: mm context of the victim
 * @gfp_mask: reclaim mode
 * @memcgp: charged memcg return
<<<<<<< HEAD
=======
 * @compound: charge the page as compound or small page
>>>>>>> v4.9.227
 *
 * Try to charge @page to the memcg that @mm belongs to, reclaiming
 * pages according to @gfp_mask if necessary.
 *
 * Returns 0 on success, with *@memcgp pointing to the charged memcg.
 * Otherwise, an error code is returned.
 *
 * After page->mapping has been set up, the caller must finalize the
 * charge with mem_cgroup_commit_charge().  Or abort the transaction
 * with mem_cgroup_cancel_charge() in case page instantiation fails.
 */
int mem_cgroup_try_charge(struct page *page, struct mm_struct *mm,
<<<<<<< HEAD
			  gfp_t gfp_mask, struct mem_cgroup **memcgp)
{
	struct mem_cgroup *memcg = NULL;
	unsigned int nr_pages = 1;
=======
			  gfp_t gfp_mask, struct mem_cgroup **memcgp,
			  bool compound)
{
	struct mem_cgroup *memcg = NULL;
	unsigned int nr_pages = compound ? hpage_nr_pages(page) : 1;
>>>>>>> v4.9.227
	int ret = 0;

	if (mem_cgroup_disabled())
		goto out;

	if (PageSwapCache(page)) {
<<<<<<< HEAD
		struct page_cgroup *pc = lookup_page_cgroup(page);
=======
>>>>>>> v4.9.227
		/*
		 * Every swap fault against a single page tries to charge the
		 * page, bail as early as possible.  shmem_unuse() encounters
		 * already charged pages, too.  The USED bit is protected by
		 * the page lock, which serializes swap cache removal, which
		 * in turn serializes uncharging.
		 */
<<<<<<< HEAD
		if (PageCgroupUsed(pc))
			goto out;
	}

	if (PageTransHuge(page)) {
		nr_pages <<= compound_order(page);
		VM_BUG_ON_PAGE(!PageTransHuge(page), page);
	}

	if (do_swap_account && PageSwapCache(page))
		memcg = try_get_mem_cgroup_from_page(page);
=======
		VM_BUG_ON_PAGE(!PageLocked(page), page);
		if (page->mem_cgroup)
			goto out;

		if (do_swap_account) {
			swp_entry_t ent = { .val = page_private(page), };
			unsigned short id = lookup_swap_cgroup_id(ent);

			rcu_read_lock();
			memcg = mem_cgroup_from_id(id);
			if (memcg && !css_tryget_online(&memcg->css))
				memcg = NULL;
			rcu_read_unlock();
		}
	}

>>>>>>> v4.9.227
	if (!memcg)
		memcg = get_mem_cgroup_from_mm(mm);

	ret = try_charge(memcg, gfp_mask, nr_pages);

	css_put(&memcg->css);
<<<<<<< HEAD

	if (ret == -EINTR) {
		memcg = root_mem_cgroup;
		ret = 0;
	}
=======
>>>>>>> v4.9.227
out:
	*memcgp = memcg;
	return ret;
}

/**
 * mem_cgroup_commit_charge - commit a page charge
 * @page: page to charge
 * @memcg: memcg to charge the page to
 * @lrucare: page might be on LRU already
<<<<<<< HEAD
=======
 * @compound: charge the page as compound or small page
>>>>>>> v4.9.227
 *
 * Finalize a charge transaction started by mem_cgroup_try_charge(),
 * after page->mapping has been set up.  This must happen atomically
 * as part of the page instantiation, i.e. under the page table lock
 * for anonymous pages, under the page lock for page and swap cache.
 *
 * In addition, the page must not be on the LRU during the commit, to
 * prevent racing with task migration.  If it might be, use @lrucare.
 *
 * Use mem_cgroup_cancel_charge() to cancel the transaction instead.
 */
void mem_cgroup_commit_charge(struct page *page, struct mem_cgroup *memcg,
<<<<<<< HEAD
			      bool lrucare)
{
	unsigned int nr_pages = 1;
=======
			      bool lrucare, bool compound)
{
	unsigned int nr_pages = compound ? hpage_nr_pages(page) : 1;
>>>>>>> v4.9.227

	VM_BUG_ON_PAGE(!page->mapping, page);
	VM_BUG_ON_PAGE(PageLRU(page) && !lrucare, page);

	if (mem_cgroup_disabled())
		return;
	/*
	 * Swap faults will attempt to charge the same page multiple
	 * times.  But reuse_swap_page() might have removed the page
	 * from swapcache already, so we can't check PageSwapCache().
	 */
	if (!memcg)
		return;

	commit_charge(page, memcg, lrucare);

<<<<<<< HEAD
	if (PageTransHuge(page)) {
		nr_pages <<= compound_order(page);
		VM_BUG_ON_PAGE(!PageTransHuge(page), page);
	}

	local_irq_disable();
	mem_cgroup_charge_statistics(memcg, page, nr_pages);
	memcg_check_events(memcg, page);
	local_irq_enable();

	if (do_swap_account && PageSwapCache(page)) {
=======
	local_irq_disable();
	mem_cgroup_charge_statistics(memcg, page, compound, nr_pages);
	memcg_check_events(memcg, page);
	local_irq_enable();

	if (do_memsw_account() && PageSwapCache(page)) {
>>>>>>> v4.9.227
		swp_entry_t entry = { .val = page_private(page) };
		/*
		 * The swap entry might not get freed for a long time,
		 * let's not wait for it.  The page already received a
		 * memory+swap charge, drop the swap entry duplicate.
		 */
		mem_cgroup_uncharge_swap(entry);
	}
}

/**
 * mem_cgroup_cancel_charge - cancel a page charge
 * @page: page to charge
 * @memcg: memcg to charge the page to
<<<<<<< HEAD
 *
 * Cancel a charge transaction started by mem_cgroup_try_charge().
 */
void mem_cgroup_cancel_charge(struct page *page, struct mem_cgroup *memcg)
{
	unsigned int nr_pages = 1;
=======
 * @compound: charge the page as compound or small page
 *
 * Cancel a charge transaction started by mem_cgroup_try_charge().
 */
void mem_cgroup_cancel_charge(struct page *page, struct mem_cgroup *memcg,
		bool compound)
{
	unsigned int nr_pages = compound ? hpage_nr_pages(page) : 1;
>>>>>>> v4.9.227

	if (mem_cgroup_disabled())
		return;
	/*
	 * Swap faults will attempt to charge the same page multiple
	 * times.  But reuse_swap_page() might have removed the page
	 * from swapcache already, so we can't check PageSwapCache().
	 */
	if (!memcg)
		return;

<<<<<<< HEAD
	if (PageTransHuge(page)) {
		nr_pages <<= compound_order(page);
		VM_BUG_ON_PAGE(!PageTransHuge(page), page);
	}

=======
>>>>>>> v4.9.227
	cancel_charge(memcg, nr_pages);
}

static void uncharge_batch(struct mem_cgroup *memcg, unsigned long pgpgout,
<<<<<<< HEAD
			   unsigned long nr_mem, unsigned long nr_memsw,
			   unsigned long nr_anon, unsigned long nr_file,
			   unsigned long nr_huge, struct page *dummy_page)
{
	unsigned long flags;

	if (!mem_cgroup_is_root(memcg)) {
		if (nr_mem)
			res_counter_uncharge(&memcg->res,
					     nr_mem * PAGE_SIZE);
		if (nr_memsw)
			res_counter_uncharge(&memcg->memsw,
					     nr_memsw * PAGE_SIZE);
=======
			   unsigned long nr_anon, unsigned long nr_file,
			   unsigned long nr_huge, unsigned long nr_kmem,
			   struct page *dummy_page)
{
	unsigned long nr_pages = nr_anon + nr_file + nr_kmem;
	unsigned long flags;

	if (!mem_cgroup_is_root(memcg)) {
		page_counter_uncharge(&memcg->memory, nr_pages);
		if (do_memsw_account())
			page_counter_uncharge(&memcg->memsw, nr_pages);
		if (!cgroup_subsys_on_dfl(memory_cgrp_subsys) && nr_kmem)
			page_counter_uncharge(&memcg->kmem, nr_kmem);
>>>>>>> v4.9.227
		memcg_oom_recover(memcg);
	}

	local_irq_save(flags);
	__this_cpu_sub(memcg->stat->count[MEM_CGROUP_STAT_RSS], nr_anon);
	__this_cpu_sub(memcg->stat->count[MEM_CGROUP_STAT_CACHE], nr_file);
	__this_cpu_sub(memcg->stat->count[MEM_CGROUP_STAT_RSS_HUGE], nr_huge);
	__this_cpu_add(memcg->stat->events[MEM_CGROUP_EVENTS_PGPGOUT], pgpgout);
<<<<<<< HEAD
	__this_cpu_add(memcg->stat->nr_page_events, nr_anon + nr_file);
	memcg_check_events(memcg, dummy_page);
	local_irq_restore(flags);
=======
	__this_cpu_add(memcg->stat->nr_page_events, nr_pages);
	memcg_check_events(memcg, dummy_page);
	local_irq_restore(flags);

	if (!mem_cgroup_is_root(memcg))
		css_put_many(&memcg->css, nr_pages);
>>>>>>> v4.9.227
}

static void uncharge_list(struct list_head *page_list)
{
	struct mem_cgroup *memcg = NULL;
<<<<<<< HEAD
	unsigned long nr_memsw = 0;
	unsigned long nr_anon = 0;
	unsigned long nr_file = 0;
	unsigned long nr_huge = 0;
	unsigned long pgpgout = 0;
	unsigned long nr_mem = 0;
	struct list_head *next;
	struct page *page;

	next = page_list->next;
	do {
		unsigned int nr_pages = 1;
		struct page_cgroup *pc;

=======
	unsigned long nr_anon = 0;
	unsigned long nr_file = 0;
	unsigned long nr_huge = 0;
	unsigned long nr_kmem = 0;
	unsigned long pgpgout = 0;
	struct list_head *next;
	struct page *page;

	/*
	 * Note that the list can be a single page->lru; hence the
	 * do-while loop instead of a simple list_for_each_entry().
	 */
	next = page_list->next;
	do {
>>>>>>> v4.9.227
		page = list_entry(next, struct page, lru);
		next = page->lru.next;

		VM_BUG_ON_PAGE(PageLRU(page), page);
		VM_BUG_ON_PAGE(!PageHWPoison(page) && page_count(page), page);

<<<<<<< HEAD
		pc = lookup_page_cgroup(page);
		if (!PageCgroupUsed(pc))
=======
		if (!page->mem_cgroup)
>>>>>>> v4.9.227
			continue;

		/*
		 * Nobody should be changing or seriously looking at
<<<<<<< HEAD
		 * pc->mem_cgroup and pc->flags at this point, we have
		 * fully exclusive access to the page.
		 */

		if (memcg != pc->mem_cgroup) {
			if (memcg) {
				uncharge_batch(memcg, pgpgout, nr_mem, nr_memsw,
					       nr_anon, nr_file, nr_huge, page);
				pgpgout = nr_mem = nr_memsw = 0;
				nr_anon = nr_file = nr_huge = 0;
			}
			memcg = pc->mem_cgroup;
		}

		if (PageTransHuge(page)) {
			nr_pages <<= compound_order(page);
			VM_BUG_ON_PAGE(!PageTransHuge(page), page);
			nr_huge += nr_pages;
		}

		if (PageAnon(page))
			nr_anon += nr_pages;
		else
			nr_file += nr_pages;

		if (pc->flags & PCG_MEM)
			nr_mem += nr_pages;
		if (pc->flags & PCG_MEMSW)
			nr_memsw += nr_pages;
		pc->flags = 0;

		pgpgout++;
	} while (next != page_list);

	if (memcg)
		uncharge_batch(memcg, pgpgout, nr_mem, nr_memsw,
			       nr_anon, nr_file, nr_huge, page);
=======
		 * page->mem_cgroup at this point, we have fully
		 * exclusive access to the page.
		 */

		if (memcg != page->mem_cgroup) {
			if (memcg) {
				uncharge_batch(memcg, pgpgout, nr_anon, nr_file,
					       nr_huge, nr_kmem, page);
				pgpgout = nr_anon = nr_file =
					nr_huge = nr_kmem = 0;
			}
			memcg = page->mem_cgroup;
		}

		if (!PageKmemcg(page)) {
			unsigned int nr_pages = 1;

			if (PageTransHuge(page)) {
				nr_pages <<= compound_order(page);
				nr_huge += nr_pages;
			}
			if (PageAnon(page))
				nr_anon += nr_pages;
			else
				nr_file += nr_pages;
			pgpgout++;
		} else {
			nr_kmem += 1 << compound_order(page);
			__ClearPageKmemcg(page);
		}

		page->mem_cgroup = NULL;
	} while (next != page_list);

	if (memcg)
		uncharge_batch(memcg, pgpgout, nr_anon, nr_file,
			       nr_huge, nr_kmem, page);
>>>>>>> v4.9.227
}

/**
 * mem_cgroup_uncharge - uncharge a page
 * @page: page to uncharge
 *
 * Uncharge a page previously charged with mem_cgroup_try_charge() and
 * mem_cgroup_commit_charge().
 */
void mem_cgroup_uncharge(struct page *page)
{
<<<<<<< HEAD
	struct page_cgroup *pc;

=======
>>>>>>> v4.9.227
	if (mem_cgroup_disabled())
		return;

	/* Don't touch page->lru of any random page, pre-check: */
<<<<<<< HEAD
	pc = lookup_page_cgroup(page);
	if (!PageCgroupUsed(pc))
=======
	if (!page->mem_cgroup)
>>>>>>> v4.9.227
		return;

	INIT_LIST_HEAD(&page->lru);
	uncharge_list(&page->lru);
}

/**
 * mem_cgroup_uncharge_list - uncharge a list of page
 * @page_list: list of pages to uncharge
 *
 * Uncharge a list of pages previously charged with
 * mem_cgroup_try_charge() and mem_cgroup_commit_charge().
 */
void mem_cgroup_uncharge_list(struct list_head *page_list)
{
	if (mem_cgroup_disabled())
		return;

	if (!list_empty(page_list))
		uncharge_list(page_list);
}

/**
<<<<<<< HEAD
 * mem_cgroup_migrate - migrate a charge to another page
 * @oldpage: currently charged page
 * @newpage: page to transfer the charge to
 * @lrucare: either or both pages might be on the LRU already
 *
 * Migrate the charge from @oldpage to @newpage.
 *
 * Both pages must be locked, @newpage->mapping must be set up.
 */
void mem_cgroup_migrate(struct page *oldpage, struct page *newpage,
			bool lrucare)
{
	struct page_cgroup *pc;
	int isolated;

	VM_BUG_ON_PAGE(!PageLocked(oldpage), oldpage);
	VM_BUG_ON_PAGE(!PageLocked(newpage), newpage);
	VM_BUG_ON_PAGE(!lrucare && PageLRU(oldpage), oldpage);
	VM_BUG_ON_PAGE(!lrucare && PageLRU(newpage), newpage);
=======
 * mem_cgroup_migrate - charge a page's replacement
 * @oldpage: currently circulating page
 * @newpage: replacement page
 *
 * Charge @newpage as a replacement page for @oldpage. @oldpage will
 * be uncharged upon free.
 *
 * Both pages must be locked, @newpage->mapping must be set up.
 */
void mem_cgroup_migrate(struct page *oldpage, struct page *newpage)
{
	struct mem_cgroup *memcg;
	unsigned int nr_pages;
	bool compound;
	unsigned long flags;

	VM_BUG_ON_PAGE(!PageLocked(oldpage), oldpage);
	VM_BUG_ON_PAGE(!PageLocked(newpage), newpage);
>>>>>>> v4.9.227
	VM_BUG_ON_PAGE(PageAnon(oldpage) != PageAnon(newpage), newpage);
	VM_BUG_ON_PAGE(PageTransHuge(oldpage) != PageTransHuge(newpage),
		       newpage);

	if (mem_cgroup_disabled())
		return;

	/* Page cache replacement: new page already charged? */
<<<<<<< HEAD
	pc = lookup_page_cgroup(newpage);
	if (PageCgroupUsed(pc))
		return;

	/* Re-entrant migration: old page already uncharged? */
	pc = lookup_page_cgroup(oldpage);
	if (!PageCgroupUsed(pc))
		return;

	VM_BUG_ON_PAGE(!(pc->flags & PCG_MEM), oldpage);
	VM_BUG_ON_PAGE(do_swap_account && !(pc->flags & PCG_MEMSW), oldpage);

	if (lrucare)
		lock_page_lru(oldpage, &isolated);

	pc->flags = 0;

	if (lrucare)
		unlock_page_lru(oldpage, isolated);

	commit_charge(newpage, pc->mem_cgroup, lrucare);
}

=======
	if (newpage->mem_cgroup)
		return;

	/* Swapcache readahead pages can get replaced before being charged */
	memcg = oldpage->mem_cgroup;
	if (!memcg)
		return;

	/* Force-charge the new page. The old one will be freed soon */
	compound = PageTransHuge(newpage);
	nr_pages = compound ? hpage_nr_pages(newpage) : 1;

	page_counter_charge(&memcg->memory, nr_pages);
	if (do_memsw_account())
		page_counter_charge(&memcg->memsw, nr_pages);
	css_get_many(&memcg->css, nr_pages);

	commit_charge(newpage, memcg, false);

	local_irq_save(flags);
	mem_cgroup_charge_statistics(memcg, newpage, compound, nr_pages);
	memcg_check_events(memcg, newpage);
	local_irq_restore(flags);
}

DEFINE_STATIC_KEY_FALSE(memcg_sockets_enabled_key);
EXPORT_SYMBOL(memcg_sockets_enabled_key);

void mem_cgroup_sk_alloc(struct sock *sk)
{
	struct mem_cgroup *memcg;

	if (!mem_cgroup_sockets_enabled)
		return;

	/*
	 * Socket cloning can throw us here with sk_memcg already
	 * filled. It won't however, necessarily happen from
	 * process context. So the test for root memcg given
	 * the current task's memcg won't help us in this case.
	 *
	 * Respecting the original socket's memcg is a better
	 * decision in this case.
	 */
	if (sk->sk_memcg) {
		BUG_ON(mem_cgroup_is_root(sk->sk_memcg));
		css_get(&sk->sk_memcg->css);
		return;
	}

	/* Do not associate the sock with unrelated interrupted task's memcg. */
	if (in_interrupt())
		return;

	rcu_read_lock();
	memcg = mem_cgroup_from_task(current);
	if (memcg == root_mem_cgroup)
		goto out;
	if (!cgroup_subsys_on_dfl(memory_cgrp_subsys) && !memcg->tcpmem_active)
		goto out;
	if (css_tryget_online(&memcg->css))
		sk->sk_memcg = memcg;
out:
	rcu_read_unlock();
}

void mem_cgroup_sk_free(struct sock *sk)
{
	if (sk->sk_memcg)
		css_put(&sk->sk_memcg->css);
}

/**
 * mem_cgroup_charge_skmem - charge socket memory
 * @memcg: memcg to charge
 * @nr_pages: number of pages to charge
 *
 * Charges @nr_pages to @memcg. Returns %true if the charge fit within
 * @memcg's configured limit, %false if the charge had to be forced.
 */
bool mem_cgroup_charge_skmem(struct mem_cgroup *memcg, unsigned int nr_pages)
{
	gfp_t gfp_mask = GFP_KERNEL;

	if (!cgroup_subsys_on_dfl(memory_cgrp_subsys)) {
		struct page_counter *fail;

		if (page_counter_try_charge(&memcg->tcpmem, nr_pages, &fail)) {
			memcg->tcpmem_pressure = 0;
			return true;
		}
		page_counter_charge(&memcg->tcpmem, nr_pages);
		memcg->tcpmem_pressure = 1;
		return false;
	}

	/* Don't block in the packet receive path */
	if (in_softirq())
		gfp_mask = GFP_NOWAIT;

	this_cpu_add(memcg->stat->count[MEMCG_SOCK], nr_pages);

	if (try_charge(memcg, gfp_mask, nr_pages) == 0)
		return true;

	try_charge(memcg, gfp_mask|__GFP_NOFAIL, nr_pages);
	return false;
}

/**
 * mem_cgroup_uncharge_skmem - uncharge socket memory
 * @memcg - memcg to uncharge
 * @nr_pages - number of pages to uncharge
 */
void mem_cgroup_uncharge_skmem(struct mem_cgroup *memcg, unsigned int nr_pages)
{
	if (!cgroup_subsys_on_dfl(memory_cgrp_subsys)) {
		page_counter_uncharge(&memcg->tcpmem, nr_pages);
		return;
	}

	this_cpu_sub(memcg->stat->count[MEMCG_SOCK], nr_pages);

	page_counter_uncharge(&memcg->memory, nr_pages);
	css_put_many(&memcg->css, nr_pages);
}

static int __init cgroup_memory(char *s)
{
	char *token;

	while ((token = strsep(&s, ",")) != NULL) {
		if (!*token)
			continue;
		if (!strcmp(token, "nosocket"))
			cgroup_memory_nosocket = true;
		if (!strcmp(token, "nokmem"))
			cgroup_memory_nokmem = true;
	}
	return 0;
}
__setup("cgroup.memory=", cgroup_memory);

>>>>>>> v4.9.227
/*
 * subsys_initcall() for memory controller.
 *
 * Some parts like hotcpu_notifier() have to be initialized from this context
 * because of lock dependencies (cgroup_lock -> cpu hotplug) but basically
 * everything that doesn't depend on a specific mem_cgroup structure should
 * be initialized from here.
 */
static int __init mem_cgroup_init(void)
{
<<<<<<< HEAD
	hotcpu_notifier(memcg_cpu_hotplug_callback, 0);
	enable_swap_cgroup();
	mem_cgroup_soft_limit_tree_init();
	memcg_stock_init();
	return 0;
}
subsys_initcall(mem_cgroup_init);
=======
	int cpu, node;

#ifndef CONFIG_SLOB
	/*
	 * Kmem cache creation is mostly done with the slab_mutex held,
	 * so use a special workqueue to avoid stalling all worker
	 * threads in case lots of cgroups are created simultaneously.
	 */
	memcg_kmem_cache_create_wq =
		alloc_ordered_workqueue("memcg_kmem_cache_create", 0);
	BUG_ON(!memcg_kmem_cache_create_wq);
#endif

	hotcpu_notifier(memcg_cpu_hotplug_callback, 0);

	for_each_possible_cpu(cpu)
		INIT_WORK(&per_cpu_ptr(&memcg_stock, cpu)->work,
			  drain_local_stock);

	for_each_node(node) {
		struct mem_cgroup_tree_per_node *rtpn;

		rtpn = kzalloc_node(sizeof(*rtpn), GFP_KERNEL,
				    node_online(node) ? node : NUMA_NO_NODE);

		rtpn->rb_root = RB_ROOT;
		spin_lock_init(&rtpn->lock);
		soft_limit_tree.rb_tree_per_node[node] = rtpn;
	}

	return 0;
}
subsys_initcall(mem_cgroup_init);

#ifdef CONFIG_MEMCG_SWAP
static struct mem_cgroup *mem_cgroup_id_get_online(struct mem_cgroup *memcg)
{
	while (!atomic_inc_not_zero(&memcg->id.ref)) {
		/*
		 * The root cgroup cannot be destroyed, so it's refcount must
		 * always be >= 1.
		 */
		if (WARN_ON_ONCE(memcg == root_mem_cgroup)) {
			VM_BUG_ON(1);
			break;
		}
		memcg = parent_mem_cgroup(memcg);
		if (!memcg)
			memcg = root_mem_cgroup;
	}
	return memcg;
}

/**
 * mem_cgroup_swapout - transfer a memsw charge to swap
 * @page: page whose memsw charge to transfer
 * @entry: swap entry to move the charge to
 *
 * Transfer the memsw charge of @page to @entry.
 */
void mem_cgroup_swapout(struct page *page, swp_entry_t entry)
{
	struct mem_cgroup *memcg, *swap_memcg;
	unsigned short oldid;

	VM_BUG_ON_PAGE(PageLRU(page), page);
	VM_BUG_ON_PAGE(page_count(page), page);

	if (!do_memsw_account())
		return;

	memcg = page->mem_cgroup;

	/* Readahead page, never charged */
	if (!memcg)
		return;

	/*
	 * In case the memcg owning these pages has been offlined and doesn't
	 * have an ID allocated to it anymore, charge the closest online
	 * ancestor for the swap instead and transfer the memory+swap charge.
	 */
	swap_memcg = mem_cgroup_id_get_online(memcg);
	oldid = swap_cgroup_record(entry, mem_cgroup_id(swap_memcg));
	VM_BUG_ON_PAGE(oldid, page);
	mem_cgroup_swap_statistics(swap_memcg, true);

	page->mem_cgroup = NULL;

	if (!mem_cgroup_is_root(memcg))
		page_counter_uncharge(&memcg->memory, 1);

	if (memcg != swap_memcg) {
		if (!mem_cgroup_is_root(swap_memcg))
			page_counter_charge(&swap_memcg->memsw, 1);
		page_counter_uncharge(&memcg->memsw, 1);
	}

	/*
	 * Interrupts should be disabled here because the caller holds the
	 * mapping->tree_lock lock which is taken with interrupts-off. It is
	 * important here to have the interrupts disabled because it is the
	 * only synchronisation we have for udpating the per-CPU variables.
	 */
	VM_BUG_ON(!irqs_disabled());
	mem_cgroup_charge_statistics(memcg, page, false, -1);
	memcg_check_events(memcg, page);

	if (!mem_cgroup_is_root(memcg))
		css_put(&memcg->css);
}

/*
 * mem_cgroup_try_charge_swap - try charging a swap entry
 * @page: page being added to swap
 * @entry: swap entry to charge
 *
 * Try to charge @entry to the memcg that @page belongs to.
 *
 * Returns 0 on success, -ENOMEM on failure.
 */
int mem_cgroup_try_charge_swap(struct page *page, swp_entry_t entry)
{
	struct mem_cgroup *memcg;
	struct page_counter *counter;
	unsigned short oldid;

	if (!cgroup_subsys_on_dfl(memory_cgrp_subsys) || !do_swap_account)
		return 0;

	memcg = page->mem_cgroup;

	/* Readahead page, never charged */
	if (!memcg)
		return 0;

	memcg = mem_cgroup_id_get_online(memcg);

	if (!mem_cgroup_is_root(memcg) &&
	    !page_counter_try_charge(&memcg->swap, 1, &counter)) {
		mem_cgroup_id_put(memcg);
		return -ENOMEM;
	}

	oldid = swap_cgroup_record(entry, mem_cgroup_id(memcg));
	VM_BUG_ON_PAGE(oldid, page);
	mem_cgroup_swap_statistics(memcg, true);

	return 0;
}

/**
 * mem_cgroup_uncharge_swap - uncharge a swap entry
 * @entry: swap entry to uncharge
 *
 * Drop the swap charge associated with @entry.
 */
void mem_cgroup_uncharge_swap(swp_entry_t entry)
{
	struct mem_cgroup *memcg;
	unsigned short id;

	if (!do_swap_account)
		return;

	id = swap_cgroup_record(entry, 0);
	rcu_read_lock();
	memcg = mem_cgroup_from_id(id);
	if (memcg) {
		if (!mem_cgroup_is_root(memcg)) {
			if (cgroup_subsys_on_dfl(memory_cgrp_subsys))
				page_counter_uncharge(&memcg->swap, 1);
			else
				page_counter_uncharge(&memcg->memsw, 1);
		}
		mem_cgroup_swap_statistics(memcg, false);
		mem_cgroup_id_put(memcg);
	}
	rcu_read_unlock();
}

long mem_cgroup_get_nr_swap_pages(struct mem_cgroup *memcg)
{
	long nr_swap_pages = get_nr_swap_pages();

	if (!do_swap_account || !cgroup_subsys_on_dfl(memory_cgrp_subsys))
		return nr_swap_pages;
	for (; memcg != root_mem_cgroup; memcg = parent_mem_cgroup(memcg))
		nr_swap_pages = min_t(long, nr_swap_pages,
				      READ_ONCE(memcg->swap.limit) -
				      page_counter_read(&memcg->swap));
	return nr_swap_pages;
}

bool mem_cgroup_swap_full(struct page *page)
{
	struct mem_cgroup *memcg;

	VM_BUG_ON_PAGE(!PageLocked(page), page);

	if (vm_swap_full())
		return true;
	if (!do_swap_account || !cgroup_subsys_on_dfl(memory_cgrp_subsys))
		return false;

	memcg = page->mem_cgroup;
	if (!memcg)
		return false;

	for (; memcg != root_mem_cgroup; memcg = parent_mem_cgroup(memcg))
		if (page_counter_read(&memcg->swap) * 2 >= memcg->swap.limit)
			return true;

	return false;
}

/* for remember boot option*/
#ifdef CONFIG_MEMCG_SWAP_ENABLED
static int really_do_swap_account __initdata = 1;
#else
static int really_do_swap_account __initdata;
#endif

static int __init enable_swap_account(char *s)
{
	if (!strcmp(s, "1"))
		really_do_swap_account = 1;
	else if (!strcmp(s, "0"))
		really_do_swap_account = 0;
	return 1;
}
__setup("swapaccount=", enable_swap_account);

static u64 swap_current_read(struct cgroup_subsys_state *css,
			     struct cftype *cft)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(css);

	return (u64)page_counter_read(&memcg->swap) * PAGE_SIZE;
}

static int swap_max_show(struct seq_file *m, void *v)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(seq_css(m));
	unsigned long max = READ_ONCE(memcg->swap.limit);

	if (max == PAGE_COUNTER_MAX)
		seq_puts(m, "max\n");
	else
		seq_printf(m, "%llu\n", (u64)max * PAGE_SIZE);

	return 0;
}

static ssize_t swap_max_write(struct kernfs_open_file *of,
			      char *buf, size_t nbytes, loff_t off)
{
	struct mem_cgroup *memcg = mem_cgroup_from_css(of_css(of));
	unsigned long max;
	int err;

	buf = strstrip(buf);
	err = page_counter_memparse(buf, "max", &max);
	if (err)
		return err;

	mutex_lock(&memcg_limit_mutex);
	err = page_counter_limit(&memcg->swap, max);
	mutex_unlock(&memcg_limit_mutex);
	if (err)
		return err;

	return nbytes;
}

static struct cftype swap_files[] = {
	{
		.name = "swap.current",
		.flags = CFTYPE_NOT_ON_ROOT,
		.read_u64 = swap_current_read,
	},
	{
		.name = "swap.max",
		.flags = CFTYPE_NOT_ON_ROOT,
		.seq_show = swap_max_show,
		.write = swap_max_write,
	},
	{ }	/* terminate */
};

static struct cftype memsw_cgroup_files[] = {
	{
		.name = "memsw.usage_in_bytes",
		.private = MEMFILE_PRIVATE(_MEMSWAP, RES_USAGE),
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "memsw.max_usage_in_bytes",
		.private = MEMFILE_PRIVATE(_MEMSWAP, RES_MAX_USAGE),
		.write = mem_cgroup_reset,
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "memsw.limit_in_bytes",
		.private = MEMFILE_PRIVATE(_MEMSWAP, RES_LIMIT),
		.write = mem_cgroup_write,
		.read_u64 = mem_cgroup_read_u64,
	},
	{
		.name = "memsw.failcnt",
		.private = MEMFILE_PRIVATE(_MEMSWAP, RES_FAILCNT),
		.write = mem_cgroup_reset,
		.read_u64 = mem_cgroup_read_u64,
	},
	{ },	/* terminate */
};

static int __init mem_cgroup_swap_init(void)
{
	if (!mem_cgroup_disabled() && really_do_swap_account) {
		do_swap_account = 1;
		WARN_ON(cgroup_add_dfl_cftypes(&memory_cgrp_subsys,
					       swap_files));
		WARN_ON(cgroup_add_legacy_cftypes(&memory_cgrp_subsys,
						  memsw_cgroup_files));
	}
	return 0;
}
subsys_initcall(mem_cgroup_swap_init);

#endif /* CONFIG_MEMCG_SWAP */
>>>>>>> v4.9.227
