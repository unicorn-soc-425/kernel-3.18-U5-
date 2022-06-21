/* memcontrol.h - Memory Controller
 *
 * Copyright IBM Corporation, 2007
 * Author Balbir Singh <balbir@linux.vnet.ibm.com>
 *
 * Copyright 2007 OpenVZ SWsoft Inc
 * Author: Pavel Emelianov <xemul@openvz.org>
 *
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

#ifndef _LINUX_MEMCONTROL_H
#define _LINUX_MEMCONTROL_H
#include <linux/cgroup.h>
#include <linux/vm_event_item.h>
#include <linux/hardirq.h>
#include <linux/jump_label.h>
<<<<<<< HEAD

struct mem_cgroup;
struct page_cgroup;
=======
#include <linux/page_counter.h>
#include <linux/vmpressure.h>
#include <linux/eventfd.h>
#include <linux/mmzone.h>
#include <linux/writeback.h>
#include <linux/page-flags.h>

struct mem_cgroup;
>>>>>>> v4.9.227
struct page;
struct mm_struct;
struct kmem_cache;

/*
 * The corresponding mem_cgroup_stat_names is defined in mm/memcontrol.c,
 * These two lists should keep in accord with each other.
 */
enum mem_cgroup_stat_index {
	/*
	 * For MEM_CONTAINER_TYPE_ALL, usage = pagecache + rss.
	 */
	MEM_CGROUP_STAT_CACHE,		/* # of pages charged as cache */
	MEM_CGROUP_STAT_RSS,		/* # of pages charged as anon rss */
	MEM_CGROUP_STAT_RSS_HUGE,	/* # of pages charged as anon huge */
	MEM_CGROUP_STAT_FILE_MAPPED,	/* # of pages charged as file rss */
<<<<<<< HEAD
	MEM_CGROUP_STAT_WRITEBACK,	/* # of pages under writeback */
	MEM_CGROUP_STAT_SWAP,		/* # of pages, swapped out */
	MEM_CGROUP_STAT_NSTATS,
};

struct mem_cgroup_reclaim_cookie {
	struct zone *zone;
=======
	MEM_CGROUP_STAT_DIRTY,          /* # of dirty pages in page cache */
	MEM_CGROUP_STAT_WRITEBACK,	/* # of pages under writeback */
	MEM_CGROUP_STAT_SWAP,		/* # of pages, swapped out */
	MEM_CGROUP_STAT_NSTATS,
	/* default hierarchy stats */
	MEMCG_KERNEL_STACK_KB = MEM_CGROUP_STAT_NSTATS,
	MEMCG_SLAB_RECLAIMABLE,
	MEMCG_SLAB_UNRECLAIMABLE,
	MEMCG_SOCK,
	MEMCG_NR_STAT,
};

struct mem_cgroup_reclaim_cookie {
	pg_data_t *pgdat;
>>>>>>> v4.9.227
	int priority;
	unsigned int generation;
};

<<<<<<< HEAD
#ifdef CONFIG_MEMCG
int mem_cgroup_try_charge(struct page *page, struct mm_struct *mm,
			  gfp_t gfp_mask, struct mem_cgroup **memcgp);
void mem_cgroup_commit_charge(struct page *page, struct mem_cgroup *memcg,
			      bool lrucare);
void mem_cgroup_cancel_charge(struct page *page, struct mem_cgroup *memcg);
void mem_cgroup_uncharge(struct page *page);
void mem_cgroup_uncharge_list(struct list_head *page_list);

void mem_cgroup_migrate(struct page *oldpage, struct page *newpage,
			bool lrucare);

struct lruvec *mem_cgroup_zone_lruvec(struct zone *, struct mem_cgroup *);
struct lruvec *mem_cgroup_page_lruvec(struct page *, struct zone *);

bool __mem_cgroup_same_or_subtree(const struct mem_cgroup *root_memcg,
				  struct mem_cgroup *memcg);
bool task_in_mem_cgroup(struct task_struct *task,
			const struct mem_cgroup *memcg);

extern struct mem_cgroup *try_get_mem_cgroup_from_page(struct page *page);
extern struct mem_cgroup *mem_cgroup_from_task(struct task_struct *p);

extern struct mem_cgroup *parent_mem_cgroup(struct mem_cgroup *memcg);
extern struct mem_cgroup *mem_cgroup_from_css(struct cgroup_subsys_state *css);

static inline
bool mm_match_cgroup(const struct mm_struct *mm, const struct mem_cgroup *memcg)
{
	struct mem_cgroup *task_memcg;
	bool match;

	rcu_read_lock();
	task_memcg = mem_cgroup_from_task(rcu_dereference(mm->owner));
	match = __mem_cgroup_same_or_subtree(memcg, task_memcg);
	rcu_read_unlock();
	return match;
}

extern struct cgroup_subsys_state *mem_cgroup_css(struct mem_cgroup *memcg);
=======
enum mem_cgroup_events_index {
	MEM_CGROUP_EVENTS_PGPGIN,	/* # of pages paged in */
	MEM_CGROUP_EVENTS_PGPGOUT,	/* # of pages paged out */
	MEM_CGROUP_EVENTS_PGFAULT,	/* # of page-faults */
	MEM_CGROUP_EVENTS_PGMAJFAULT,	/* # of major page-faults */
	MEM_CGROUP_EVENTS_NSTATS,
	/* default hierarchy events */
	MEMCG_LOW = MEM_CGROUP_EVENTS_NSTATS,
	MEMCG_HIGH,
	MEMCG_MAX,
	MEMCG_OOM,
	MEMCG_NR_EVENTS,
};

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

#ifdef CONFIG_MEMCG

#define MEM_CGROUP_ID_SHIFT	16
#define MEM_CGROUP_ID_MAX	USHRT_MAX

struct mem_cgroup_id {
	int id;
	atomic_t ref;
};

struct mem_cgroup_stat_cpu {
	long count[MEMCG_NR_STAT];
	unsigned long events[MEMCG_NR_EVENTS];
	unsigned long nr_page_events;
	unsigned long targets[MEM_CGROUP_NTARGETS];
};

struct mem_cgroup_reclaim_iter {
	struct mem_cgroup *position;
	/* scan generation, increased every round-trip */
	unsigned int generation;
};

/*
 * per-zone information in memory controller.
 */
struct mem_cgroup_per_node {
	struct lruvec		lruvec;
	unsigned long		lru_zone_size[MAX_NR_ZONES][NR_LRU_LISTS];

	struct mem_cgroup_reclaim_iter	iter[DEF_PRIORITY + 1];

	struct rb_node		tree_node;	/* RB tree node */
	unsigned long		usage_in_excess;/* Set to the value by which */
						/* the soft limit is exceeded*/
	bool			on_tree;
	struct mem_cgroup	*memcg;		/* Back pointer, we cannot */
						/* use container_of	   */
};

struct mem_cgroup_threshold {
	struct eventfd_ctx *eventfd;
	unsigned long threshold;
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

enum memcg_kmem_state {
	KMEM_NONE,
	KMEM_ALLOCATED,
	KMEM_ONLINE,
};

/*
 * The memory controller data structure. The memory controller controls both
 * page cache and RSS per cgroup. We would eventually like to provide
 * statistics based on the statistics developed by Rik Van Riel for clock-pro,
 * to help the administrator determine what knobs to tune.
 */
struct mem_cgroup {
	struct cgroup_subsys_state css;

	/* Private memcg ID. Used to ID objects that outlive the cgroup */
	struct mem_cgroup_id id;

	/* Accounted resources */
	struct page_counter memory;
	struct page_counter swap;

	/* Legacy consumer-oriented counters */
	struct page_counter memsw;
	struct page_counter kmem;
	struct page_counter tcpmem;

	/* Normal memory consumption range */
	unsigned long low;
	unsigned long high;

	/* Range enforcement for interrupt charges */
	struct work_struct high_work;

	unsigned long soft_limit;

	/* vmpressure notifications */
	struct vmpressure vmpressure;

	/*
	 * Should the accounting and control be hierarchical, per subtree?
	 */
	bool use_hierarchy;

	/* protected by memcg_oom_lock */
	bool		oom_lock;
	int		under_oom;

	int	swappiness;
	/* OOM-Killer disable */
	int		oom_kill_disable;

	/* handle for "memory.events" */
	struct cgroup_file events_file;

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
	atomic_t		moving_account;
	/* taken only while moving_account > 0 */
	spinlock_t		move_lock;
	struct task_struct	*move_lock_task;
	unsigned long		move_lock_flags;
	/*
	 * percpu counter.
	 */
	struct mem_cgroup_stat_cpu __percpu *stat;

	unsigned long		socket_pressure;

	/* Legacy tcp memory accounting */
	bool			tcpmem_active;
	int			tcpmem_pressure;

#ifndef CONFIG_SLOB
        /* Index in the kmem_cache->memcg_params.memcg_caches array */
	int kmemcg_id;
	enum memcg_kmem_state kmem_state;
#endif

	int last_scanned_node;
#if MAX_NUMNODES > 1
	nodemask_t	scan_nodes;
	atomic_t	numainfo_events;
	atomic_t	numainfo_updating;
#endif

#ifdef CONFIG_CGROUP_WRITEBACK
	struct list_head cgwb_list;
	struct wb_domain cgwb_domain;
#endif

	/* List of events which userspace want to receive */
	struct list_head event_list;
	spinlock_t event_list_lock;

	struct mem_cgroup_per_node *nodeinfo[0];
	/* WARNING: nodeinfo must be the last member here */
};

extern struct mem_cgroup *root_mem_cgroup;

static inline bool mem_cgroup_disabled(void)
{
	return !cgroup_subsys_enabled(memory_cgrp_subsys);
}

/**
 * mem_cgroup_events - count memory events against a cgroup
 * @memcg: the memory cgroup
 * @idx: the event index
 * @nr: the number of events to account for
 */
static inline void mem_cgroup_events(struct mem_cgroup *memcg,
		       enum mem_cgroup_events_index idx,
		       unsigned int nr)
{
	this_cpu_add(memcg->stat->events[idx], nr);
	cgroup_file_notify(&memcg->events_file);
}

bool mem_cgroup_low(struct mem_cgroup *root, struct mem_cgroup *memcg);

int mem_cgroup_try_charge(struct page *page, struct mm_struct *mm,
			  gfp_t gfp_mask, struct mem_cgroup **memcgp,
			  bool compound);
void mem_cgroup_commit_charge(struct page *page, struct mem_cgroup *memcg,
			      bool lrucare, bool compound);
void mem_cgroup_cancel_charge(struct page *page, struct mem_cgroup *memcg,
		bool compound);
void mem_cgroup_uncharge(struct page *page);
void mem_cgroup_uncharge_list(struct list_head *page_list);

void mem_cgroup_migrate(struct page *oldpage, struct page *newpage);

static struct mem_cgroup_per_node *
mem_cgroup_nodeinfo(struct mem_cgroup *memcg, int nid)
{
	return memcg->nodeinfo[nid];
}

/**
 * mem_cgroup_lruvec - get the lru list vector for a node or a memcg zone
 * @node: node of the wanted lruvec
 * @memcg: memcg of the wanted lruvec
 *
 * Returns the lru list vector holding pages for a given @node or a given
 * @memcg and @zone. This can be the node lruvec, if the memory controller
 * is disabled.
 */
static inline struct lruvec *mem_cgroup_lruvec(struct pglist_data *pgdat,
				struct mem_cgroup *memcg)
{
	struct mem_cgroup_per_node *mz;
	struct lruvec *lruvec;

	if (mem_cgroup_disabled()) {
		lruvec = node_lruvec(pgdat);
		goto out;
	}

	mz = mem_cgroup_nodeinfo(memcg, pgdat->node_id);
	lruvec = &mz->lruvec;
out:
	/*
	 * Since a node can be onlined after the mem_cgroup was created,
	 * we have to be prepared to initialize lruvec->pgdat here;
	 * and if offlined then reonlined, we need to reinitialize it.
	 */
	if (unlikely(lruvec->pgdat != pgdat))
		lruvec->pgdat = pgdat;
	return lruvec;
}

struct lruvec *mem_cgroup_page_lruvec(struct page *, struct pglist_data *);

bool task_in_mem_cgroup(struct task_struct *task, struct mem_cgroup *memcg);
struct mem_cgroup *mem_cgroup_from_task(struct task_struct *p);

static inline
struct mem_cgroup *mem_cgroup_from_css(struct cgroup_subsys_state *css){
	return css ? container_of(css, struct mem_cgroup, css) : NULL;
}

#define mem_cgroup_from_counter(counter, member)	\
	container_of(counter, struct mem_cgroup, member)
>>>>>>> v4.9.227

struct mem_cgroup *mem_cgroup_iter(struct mem_cgroup *,
				   struct mem_cgroup *,
				   struct mem_cgroup_reclaim_cookie *);
void mem_cgroup_iter_break(struct mem_cgroup *, struct mem_cgroup *);
<<<<<<< HEAD
=======
int mem_cgroup_scan_tasks(struct mem_cgroup *,
			  int (*)(struct task_struct *, void *), void *);

static inline unsigned short mem_cgroup_id(struct mem_cgroup *memcg)
{
	if (mem_cgroup_disabled())
		return 0;

	return memcg->id.id;
}
struct mem_cgroup *mem_cgroup_from_id(unsigned short id);

/**
 * parent_mem_cgroup - find the accounting parent of a memcg
 * @memcg: memcg whose parent to find
 *
 * Returns the parent memcg, or NULL if this is the root or the memory
 * controller is in legacy no-hierarchy mode.
 */
static inline struct mem_cgroup *parent_mem_cgroup(struct mem_cgroup *memcg)
{
	if (!memcg->memory.parent)
		return NULL;
	return mem_cgroup_from_counter(memcg->memory.parent, memory);
}

static inline bool mem_cgroup_is_descendant(struct mem_cgroup *memcg,
			      struct mem_cgroup *root)
{
	if (root == memcg)
		return true;
	if (!root->use_hierarchy)
		return false;
	return cgroup_is_descendant(memcg->css.cgroup, root->css.cgroup);
}

static inline bool mm_match_cgroup(struct mm_struct *mm,
				   struct mem_cgroup *memcg)
{
	struct mem_cgroup *task_memcg;
	bool match = false;

	rcu_read_lock();
	task_memcg = mem_cgroup_from_task(rcu_dereference(mm->owner));
	if (task_memcg)
		match = mem_cgroup_is_descendant(task_memcg, memcg);
	rcu_read_unlock();
	return match;
}

struct cgroup_subsys_state *mem_cgroup_css_from_page(struct page *page);
ino_t page_cgroup_ino(struct page *page);

static inline bool mem_cgroup_online(struct mem_cgroup *memcg)
{
	if (mem_cgroup_disabled())
		return true;
	return !!(memcg->css.flags & CSS_ONLINE);
}
>>>>>>> v4.9.227

/*
 * For memory reclaim.
 */
<<<<<<< HEAD
int mem_cgroup_inactive_anon_is_low(struct lruvec *lruvec);
int mem_cgroup_select_victim_node(struct mem_cgroup *memcg);
unsigned long mem_cgroup_get_lru_size(struct lruvec *lruvec, enum lru_list);
void mem_cgroup_update_lru_size(struct lruvec *, enum lru_list, int);
extern void mem_cgroup_print_oom_info(struct mem_cgroup *memcg,
					struct task_struct *p);

static inline void mem_cgroup_oom_enable(void)
{
	WARN_ON(current->memcg_oom.may_oom);
	current->memcg_oom.may_oom = 1;
=======
int mem_cgroup_select_victim_node(struct mem_cgroup *memcg);

void mem_cgroup_update_lru_size(struct lruvec *lruvec, enum lru_list lru,
		int zid, int nr_pages);

unsigned long mem_cgroup_node_nr_lru_pages(struct mem_cgroup *memcg,
					   int nid, unsigned int lru_mask);

static inline
unsigned long mem_cgroup_get_lru_size(struct lruvec *lruvec, enum lru_list lru)
{
	struct mem_cgroup_per_node *mz;
	unsigned long nr_pages = 0;
	int zid;

	mz = container_of(lruvec, struct mem_cgroup_per_node, lruvec);
	for (zid = 0; zid < MAX_NR_ZONES; zid++)
		nr_pages += mz->lru_zone_size[zid][lru];
	return nr_pages;
}

static inline
unsigned long mem_cgroup_get_zone_lru_size(struct lruvec *lruvec,
		enum lru_list lru, int zone_idx)
{
	struct mem_cgroup_per_node *mz;

	mz = container_of(lruvec, struct mem_cgroup_per_node, lruvec);
	return mz->lru_zone_size[zone_idx][lru];
}

void mem_cgroup_handle_over_high(void);

unsigned long mem_cgroup_get_limit(struct mem_cgroup *memcg);

void mem_cgroup_print_oom_info(struct mem_cgroup *memcg,
				struct task_struct *p);

static inline void mem_cgroup_oom_enable(void)
{
	WARN_ON(current->memcg_may_oom);
	current->memcg_may_oom = 1;
>>>>>>> v4.9.227
}

static inline void mem_cgroup_oom_disable(void)
{
<<<<<<< HEAD
	WARN_ON(!current->memcg_oom.may_oom);
	current->memcg_oom.may_oom = 0;
=======
	WARN_ON(!current->memcg_may_oom);
	current->memcg_may_oom = 0;
>>>>>>> v4.9.227
}

static inline bool task_in_memcg_oom(struct task_struct *p)
{
<<<<<<< HEAD
	return p->memcg_oom.memcg;
=======
	return p->memcg_in_oom;
>>>>>>> v4.9.227
}

bool mem_cgroup_oom_synchronize(bool wait);

#ifdef CONFIG_MEMCG_SWAP
extern int do_swap_account;
#endif

<<<<<<< HEAD
static inline bool mem_cgroup_disabled(void)
{
	if (memory_cgrp_subsys.disabled)
		return true;
	return false;
}

struct mem_cgroup *mem_cgroup_begin_page_stat(struct page *page, bool *locked,
					      unsigned long *flags);
void mem_cgroup_end_page_stat(struct mem_cgroup *memcg, bool *locked,
			      unsigned long *flags);
void mem_cgroup_update_page_stat(struct mem_cgroup *memcg,
				 enum mem_cgroup_stat_index idx, int val);

static inline void mem_cgroup_inc_page_stat(struct mem_cgroup *memcg,
					    enum mem_cgroup_stat_index idx)
{
	mem_cgroup_update_page_stat(memcg, idx, 1);
}

static inline void mem_cgroup_dec_page_stat(struct mem_cgroup *memcg,
					    enum mem_cgroup_stat_index idx)
{
	mem_cgroup_update_page_stat(memcg, idx, -1);
}

unsigned long mem_cgroup_soft_limit_reclaim(struct zone *zone, int order,
						gfp_t gfp_mask,
						unsigned long *total_scanned);

void __mem_cgroup_count_vm_event(struct mm_struct *mm, enum vm_event_item idx);
static inline void mem_cgroup_count_vm_event(struct mm_struct *mm,
					     enum vm_event_item idx)
{
	if (mem_cgroup_disabled())
		return;
	__mem_cgroup_count_vm_event(mm, idx);
=======
void lock_page_memcg(struct page *page);
void unlock_page_memcg(struct page *page);

/**
 * mem_cgroup_update_page_stat - update page state statistics
 * @page: the page
 * @idx: page state item to account
 * @val: number of pages (positive or negative)
 *
 * The @page must be locked or the caller must use lock_page_memcg()
 * to prevent double accounting when the page is concurrently being
 * moved to another memcg:
 *
 *   lock_page(page) or lock_page_memcg(page)
 *   if (TestClearPageState(page))
 *     mem_cgroup_update_page_stat(page, state, -1);
 *   unlock_page(page) or unlock_page_memcg(page)
 */
static inline void mem_cgroup_update_page_stat(struct page *page,
				 enum mem_cgroup_stat_index idx, int val)
{
	VM_BUG_ON(!(rcu_read_lock_held() || PageLocked(page)));

	if (page->mem_cgroup)
		this_cpu_add(page->mem_cgroup->stat->count[idx], val);
}

static inline void mem_cgroup_inc_page_stat(struct page *page,
					    enum mem_cgroup_stat_index idx)
{
	mem_cgroup_update_page_stat(page, idx, 1);
}

static inline void mem_cgroup_dec_page_stat(struct page *page,
					    enum mem_cgroup_stat_index idx)
{
	mem_cgroup_update_page_stat(page, idx, -1);
}

unsigned long mem_cgroup_soft_limit_reclaim(pg_data_t *pgdat, int order,
						gfp_t gfp_mask,
						unsigned long *total_scanned);

static inline void mem_cgroup_count_vm_event(struct mm_struct *mm,
					     enum vm_event_item idx)
{
	struct mem_cgroup *memcg;

	if (mem_cgroup_disabled())
		return;

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
>>>>>>> v4.9.227
}
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
void mem_cgroup_split_huge_fixup(struct page *head);
#endif

<<<<<<< HEAD
#ifdef CONFIG_DEBUG_VM
bool mem_cgroup_bad_page_check(struct page *page);
void mem_cgroup_print_bad_page(struct page *page);
#endif
#else /* CONFIG_MEMCG */
struct mem_cgroup;

static inline int mem_cgroup_try_charge(struct page *page, struct mm_struct *mm,
					gfp_t gfp_mask,
					struct mem_cgroup **memcgp)
=======
#else /* CONFIG_MEMCG */

#define MEM_CGROUP_ID_SHIFT	0
#define MEM_CGROUP_ID_MAX	0

struct mem_cgroup;

static inline bool mem_cgroup_disabled(void)
{
	return true;
}

static inline void mem_cgroup_events(struct mem_cgroup *memcg,
				     enum mem_cgroup_events_index idx,
				     unsigned int nr)
{
}

static inline bool mem_cgroup_low(struct mem_cgroup *root,
				  struct mem_cgroup *memcg)
{
	return false;
}

static inline int mem_cgroup_try_charge(struct page *page, struct mm_struct *mm,
					gfp_t gfp_mask,
					struct mem_cgroup **memcgp,
					bool compound)
>>>>>>> v4.9.227
{
	*memcgp = NULL;
	return 0;
}

static inline void mem_cgroup_commit_charge(struct page *page,
					    struct mem_cgroup *memcg,
<<<<<<< HEAD
					    bool lrucare)
=======
					    bool lrucare, bool compound)
>>>>>>> v4.9.227
{
}

static inline void mem_cgroup_cancel_charge(struct page *page,
<<<<<<< HEAD
					    struct mem_cgroup *memcg)
=======
					    struct mem_cgroup *memcg,
					    bool compound)
>>>>>>> v4.9.227
{
}

static inline void mem_cgroup_uncharge(struct page *page)
{
}

static inline void mem_cgroup_uncharge_list(struct list_head *page_list)
{
}

<<<<<<< HEAD
static inline void mem_cgroup_migrate(struct page *oldpage,
				      struct page *newpage,
				      bool lrucare)
{
}

static inline struct lruvec *mem_cgroup_zone_lruvec(struct zone *zone,
						    struct mem_cgroup *memcg)
{
	return &zone->lruvec;
}

static inline struct lruvec *mem_cgroup_page_lruvec(struct page *page,
						    struct zone *zone)
{
	return &zone->lruvec;
}

static inline struct mem_cgroup *try_get_mem_cgroup_from_page(struct page *page)
{
	return NULL;
=======
static inline void mem_cgroup_migrate(struct page *old, struct page *new)
{
}

static inline struct lruvec *mem_cgroup_lruvec(struct pglist_data *pgdat,
				struct mem_cgroup *memcg)
{
	return node_lruvec(pgdat);
}

static inline struct lruvec *mem_cgroup_page_lruvec(struct page *page,
						    struct pglist_data *pgdat)
{
	return &pgdat->lruvec;
>>>>>>> v4.9.227
}

static inline bool mm_match_cgroup(struct mm_struct *mm,
		struct mem_cgroup *memcg)
{
	return true;
}

static inline bool task_in_mem_cgroup(struct task_struct *task,
				      const struct mem_cgroup *memcg)
{
	return true;
}

<<<<<<< HEAD
static inline struct cgroup_subsys_state
		*mem_cgroup_css(struct mem_cgroup *memcg)
{
	return NULL;
}

=======
>>>>>>> v4.9.227
static inline struct mem_cgroup *
mem_cgroup_iter(struct mem_cgroup *root,
		struct mem_cgroup *prev,
		struct mem_cgroup_reclaim_cookie *reclaim)
{
	return NULL;
}

static inline void mem_cgroup_iter_break(struct mem_cgroup *root,
					 struct mem_cgroup *prev)
{
}

<<<<<<< HEAD
static inline bool mem_cgroup_disabled(void)
{
	return true;
}

static inline int
mem_cgroup_inactive_anon_is_low(struct lruvec *lruvec)
{
	return 1;
=======
static inline int mem_cgroup_scan_tasks(struct mem_cgroup *memcg,
		int (*fn)(struct task_struct *, void *), void *arg)
{
	return 0;
}

static inline unsigned short mem_cgroup_id(struct mem_cgroup *memcg)
{
	return 0;
}

static inline struct mem_cgroup *mem_cgroup_from_id(unsigned short id)
{
	WARN_ON_ONCE(id);
	/* XXX: This should always return root_mem_cgroup */
	return NULL;
}

static inline bool mem_cgroup_online(struct mem_cgroup *memcg)
{
	return true;
>>>>>>> v4.9.227
}

static inline unsigned long
mem_cgroup_get_lru_size(struct lruvec *lruvec, enum lru_list lru)
{
	return 0;
}
<<<<<<< HEAD

static inline void
mem_cgroup_update_lru_size(struct lruvec *lruvec, enum lru_list lru,
			      int increment)
{
=======
static inline
unsigned long mem_cgroup_get_zone_lru_size(struct lruvec *lruvec,
		enum lru_list lru, int zone_idx)
{
	return 0;
}

static inline unsigned long
mem_cgroup_node_nr_lru_pages(struct mem_cgroup *memcg,
			     int nid, unsigned int lru_mask)
{
	return 0;
}

static inline unsigned long mem_cgroup_get_limit(struct mem_cgroup *memcg)
{
	return 0;
>>>>>>> v4.9.227
}

static inline void
mem_cgroup_print_oom_info(struct mem_cgroup *memcg, struct task_struct *p)
{
}

<<<<<<< HEAD
static inline struct mem_cgroup *mem_cgroup_begin_page_stat(struct page *page,
					bool *locked, unsigned long *flags)
{
	return NULL;
}

static inline void mem_cgroup_end_page_stat(struct mem_cgroup *memcg,
					bool *locked, unsigned long *flags)
=======
static inline void lock_page_memcg(struct page *page)
{
}

static inline void unlock_page_memcg(struct page *page)
{
}

static inline void mem_cgroup_handle_over_high(void)
>>>>>>> v4.9.227
{
}

static inline void mem_cgroup_oom_enable(void)
{
}

static inline void mem_cgroup_oom_disable(void)
{
}

static inline bool task_in_memcg_oom(struct task_struct *p)
{
	return false;
}

static inline bool mem_cgroup_oom_synchronize(bool wait)
{
	return false;
}

<<<<<<< HEAD
static inline void mem_cgroup_inc_page_stat(struct mem_cgroup *memcg,
=======
static inline void mem_cgroup_update_page_stat(struct page *page,
					       enum mem_cgroup_stat_index idx,
					       int nr)
{
}

static inline void mem_cgroup_inc_page_stat(struct page *page,
>>>>>>> v4.9.227
					    enum mem_cgroup_stat_index idx)
{
}

<<<<<<< HEAD
static inline void mem_cgroup_dec_page_stat(struct mem_cgroup *memcg,
=======
static inline void mem_cgroup_dec_page_stat(struct page *page,
>>>>>>> v4.9.227
					    enum mem_cgroup_stat_index idx)
{
}

static inline
<<<<<<< HEAD
unsigned long mem_cgroup_soft_limit_reclaim(struct zone *zone, int order,
=======
unsigned long mem_cgroup_soft_limit_reclaim(pg_data_t *pgdat, int order,
>>>>>>> v4.9.227
					    gfp_t gfp_mask,
					    unsigned long *total_scanned)
{
	return 0;
}

static inline void mem_cgroup_split_huge_fixup(struct page *head)
{
}

static inline
void mem_cgroup_count_vm_event(struct mm_struct *mm, enum vm_event_item idx)
{
}
#endif /* CONFIG_MEMCG */

<<<<<<< HEAD
#if !defined(CONFIG_MEMCG) || !defined(CONFIG_DEBUG_VM)
static inline bool
mem_cgroup_bad_page_check(struct page *page)
{
	return false;
}

static inline void
mem_cgroup_print_bad_page(struct page *page)
{
}
#endif

enum {
	UNDER_LIMIT,
	SOFT_LIMIT,
	OVER_LIMIT,
};

struct sock;
#if defined(CONFIG_INET) && defined(CONFIG_MEMCG_KMEM)
void sock_update_memcg(struct sock *sk);
void sock_release_memcg(struct sock *sk);
#else
static inline void sock_update_memcg(struct sock *sk)
{
}
static inline void sock_release_memcg(struct sock *sk)
{
}
#endif /* CONFIG_INET && CONFIG_MEMCG_KMEM */

#ifdef CONFIG_MEMCG_KMEM
extern struct static_key memcg_kmem_enabled_key;

extern int memcg_limited_groups_array_size;
=======
#ifdef CONFIG_CGROUP_WRITEBACK

struct list_head *mem_cgroup_cgwb_list(struct mem_cgroup *memcg);
struct wb_domain *mem_cgroup_wb_domain(struct bdi_writeback *wb);
void mem_cgroup_wb_stats(struct bdi_writeback *wb, unsigned long *pfilepages,
			 unsigned long *pheadroom, unsigned long *pdirty,
			 unsigned long *pwriteback);

#else	/* CONFIG_CGROUP_WRITEBACK */

static inline struct wb_domain *mem_cgroup_wb_domain(struct bdi_writeback *wb)
{
	return NULL;
}

static inline void mem_cgroup_wb_stats(struct bdi_writeback *wb,
				       unsigned long *pfilepages,
				       unsigned long *pheadroom,
				       unsigned long *pdirty,
				       unsigned long *pwriteback)
{
}

#endif	/* CONFIG_CGROUP_WRITEBACK */

struct sock;
bool mem_cgroup_charge_skmem(struct mem_cgroup *memcg, unsigned int nr_pages);
void mem_cgroup_uncharge_skmem(struct mem_cgroup *memcg, unsigned int nr_pages);
#ifdef CONFIG_MEMCG
extern struct static_key_false memcg_sockets_enabled_key;
#define mem_cgroup_sockets_enabled static_branch_unlikely(&memcg_sockets_enabled_key)
void mem_cgroup_sk_alloc(struct sock *sk);
void mem_cgroup_sk_free(struct sock *sk);
static inline bool mem_cgroup_under_socket_pressure(struct mem_cgroup *memcg)
{
	if (!cgroup_subsys_on_dfl(memory_cgrp_subsys) && memcg->tcpmem_pressure)
		return true;
	do {
		if (time_before(jiffies, memcg->socket_pressure))
			return true;
	} while ((memcg = parent_mem_cgroup(memcg)));
	return false;
}
#else
#define mem_cgroup_sockets_enabled 0
static inline void mem_cgroup_sk_alloc(struct sock *sk) { };
static inline void mem_cgroup_sk_free(struct sock *sk) { };
static inline bool mem_cgroup_under_socket_pressure(struct mem_cgroup *memcg)
{
	return false;
}
#endif

struct kmem_cache *memcg_kmem_get_cache(struct kmem_cache *cachep);
void memcg_kmem_put_cache(struct kmem_cache *cachep);
int memcg_kmem_charge_memcg(struct page *page, gfp_t gfp, int order,
			    struct mem_cgroup *memcg);
int memcg_kmem_charge(struct page *page, gfp_t gfp, int order);
void memcg_kmem_uncharge(struct page *page, int order);

#if defined(CONFIG_MEMCG) && !defined(CONFIG_SLOB)
extern struct static_key_false memcg_kmem_enabled_key;

extern int memcg_nr_cache_ids;
void memcg_get_cache_ids(void);
void memcg_put_cache_ids(void);
>>>>>>> v4.9.227

/*
 * Helper macro to loop through all memcg-specific caches. Callers must still
 * check if the cache is valid (it is either valid or NULL).
 * the slab_mutex must be held when looping through those caches
 */
#define for_each_memcg_cache_index(_idx)	\
<<<<<<< HEAD
	for ((_idx) = 0; (_idx) < memcg_limited_groups_array_size; (_idx)++)

static inline bool memcg_kmem_enabled(void)
{
	return static_key_false(&memcg_kmem_enabled_key);
}

/*
 * In general, we'll do everything in our power to not incur in any overhead
 * for non-memcg users for the kmem functions. Not even a function call, if we
 * can avoid it.
 *
 * Therefore, we'll inline all those functions so that in the best case, we'll
 * see that kmemcg is off for everybody and proceed quickly.  If it is on,
 * we'll still do most of the flag checking inline. We check a lot of
 * conditions, but because they are pretty simple, they are expected to be
 * fast.
 */
bool __memcg_kmem_newpage_charge(gfp_t gfp, struct mem_cgroup **memcg,
					int order);
void __memcg_kmem_commit_charge(struct page *page,
				       struct mem_cgroup *memcg, int order);
void __memcg_kmem_uncharge_pages(struct page *page, int order);

int memcg_cache_id(struct mem_cgroup *memcg);

void memcg_update_array_size(int num_groups);

struct kmem_cache *
__memcg_kmem_get_cache(struct kmem_cache *cachep, gfp_t gfp);

int __memcg_charge_slab(struct kmem_cache *cachep, gfp_t gfp, int order);
void __memcg_uncharge_slab(struct kmem_cache *cachep, int order);

int __memcg_cleanup_cache_params(struct kmem_cache *s);

/**
 * memcg_kmem_newpage_charge: verify if a new kmem allocation is allowed.
 * @gfp: the gfp allocation flags.
 * @memcg: a pointer to the memcg this was charged against.
 * @order: allocation order.
 *
 * returns true if the memcg where the current task belongs can hold this
 * allocation.
 *
 * We return true automatically if this allocation is not to be accounted to
 * any memcg.
 */
static inline bool
memcg_kmem_newpage_charge(gfp_t gfp, struct mem_cgroup **memcg, int order)
{
	if (!memcg_kmem_enabled())
		return true;

	/*
	 * __GFP_NOFAIL allocations will move on even if charging is not
	 * possible. Therefore we don't even try, and have this allocation
	 * unaccounted. We could in theory charge it with
	 * res_counter_charge_nofail, but we hope those allocations are rare,
	 * and won't be worth the trouble.
	 */
	if (gfp & __GFP_NOFAIL)
		return true;
	if (in_interrupt() || (!current->mm) || (current->flags & PF_KTHREAD))
		return true;

	/* If the test is dying, just let it go. */
	if (unlikely(fatal_signal_pending(current)))
		return true;

	return __memcg_kmem_newpage_charge(gfp, memcg, order);
}

/**
 * memcg_kmem_uncharge_pages: uncharge pages from memcg
 * @page: pointer to struct page being freed
 * @order: allocation order.
 *
 * there is no need to specify memcg here, since it is embedded in page_cgroup
 */
static inline void
memcg_kmem_uncharge_pages(struct page *page, int order)
{
	if (memcg_kmem_enabled())
		__memcg_kmem_uncharge_pages(page, order);
}

/**
 * memcg_kmem_commit_charge: embeds correct memcg in a page
 * @page: pointer to struct page recently allocated
 * @memcg: the memcg structure we charged against
 * @order: allocation order.
 *
 * Needs to be called after memcg_kmem_newpage_charge, regardless of success or
 * failure of the allocation. if @page is NULL, this function will revert the
 * charges. Otherwise, it will commit the memcg given by @memcg to the
 * corresponding page_cgroup.
 */
static inline void
memcg_kmem_commit_charge(struct page *page, struct mem_cgroup *memcg, int order)
{
	if (memcg_kmem_enabled() && memcg)
		__memcg_kmem_commit_charge(page, memcg, order);
}

/**
 * memcg_kmem_get_cache: selects the correct per-memcg cache for allocation
 * @cachep: the original global kmem cache
 * @gfp: allocation flags.
 *
 * All memory allocated from a per-memcg cache is charged to the owner memcg.
 */
static __always_inline struct kmem_cache *
memcg_kmem_get_cache(struct kmem_cache *cachep, gfp_t gfp)
{
	if (!memcg_kmem_enabled())
		return cachep;
	if (gfp & __GFP_NOFAIL)
		return cachep;
	if (in_interrupt() || (!current->mm) || (current->flags & PF_KTHREAD))
		return cachep;
	if (unlikely(fatal_signal_pending(current)))
		return cachep;

	return __memcg_kmem_get_cache(cachep, gfp);
}
=======
	for ((_idx) = 0; (_idx) < memcg_nr_cache_ids; (_idx)++)

static inline bool memcg_kmem_enabled(void)
{
	return static_branch_unlikely(&memcg_kmem_enabled_key);
}

/*
 * helper for accessing a memcg's index. It will be used as an index in the
 * child cache array in kmem_cache, and also to derive its name. This function
 * will return -1 when this is not a kmem-limited memcg.
 */
static inline int memcg_cache_id(struct mem_cgroup *memcg)
{
	return memcg ? memcg->kmemcg_id : -1;
}

/**
 * memcg_kmem_update_page_stat - update kmem page state statistics
 * @page: the page
 * @idx: page state item to account
 * @val: number of pages (positive or negative)
 */
static inline void memcg_kmem_update_page_stat(struct page *page,
				enum mem_cgroup_stat_index idx, int val)
{
	if (memcg_kmem_enabled() && page->mem_cgroup)
		this_cpu_add(page->mem_cgroup->stat->count[idx], val);
}

>>>>>>> v4.9.227
#else
#define for_each_memcg_cache_index(_idx)	\
	for (; NULL; )

static inline bool memcg_kmem_enabled(void)
{
	return false;
}

<<<<<<< HEAD
static inline bool
memcg_kmem_newpage_charge(gfp_t gfp, struct mem_cgroup **memcg, int order)
{
	return true;
}

static inline void memcg_kmem_uncharge_pages(struct page *page, int order)
{
}

static inline void
memcg_kmem_commit_charge(struct page *page, struct mem_cgroup *memcg, int order)
{
}

=======
>>>>>>> v4.9.227
static inline int memcg_cache_id(struct mem_cgroup *memcg)
{
	return -1;
}

<<<<<<< HEAD
static inline struct kmem_cache *
memcg_kmem_get_cache(struct kmem_cache *cachep, gfp_t gfp)
{
	return cachep;
}
#endif /* CONFIG_MEMCG_KMEM */
#endif /* _LINUX_MEMCONTROL_H */

=======
static inline void memcg_get_cache_ids(void)
{
}

static inline void memcg_put_cache_ids(void)
{
}

static inline void memcg_kmem_update_page_stat(struct page *page,
				enum mem_cgroup_stat_index idx, int val)
{
}
#endif /* CONFIG_MEMCG && !CONFIG_SLOB */

#endif /* _LINUX_MEMCONTROL_H */
>>>>>>> v4.9.227
