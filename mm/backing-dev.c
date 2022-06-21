
#include <linux/wait.h>
#include <linux/backing-dev.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/writeback.h>
#include <linux/device.h>
#include <trace/events/writeback.h>

static atomic_long_t bdi_seq = ATOMIC_LONG_INIT(0);

<<<<<<< HEAD
struct backing_dev_info default_backing_dev_info = {
	.name		= "default",
	.ra_pages	= VM_MAX_READAHEAD * 1024 / PAGE_CACHE_SIZE,
	.state		= 0,
	.capabilities	= BDI_CAP_MAP_COPY,
};
EXPORT_SYMBOL_GPL(default_backing_dev_info);

=======
>>>>>>> v4.9.227
struct backing_dev_info noop_backing_dev_info = {
	.name		= "noop",
	.capabilities	= BDI_CAP_NO_ACCT_AND_WRITEBACK,
};
EXPORT_SYMBOL_GPL(noop_backing_dev_info);

static struct class *bdi_class;

/*
 * bdi_lock protects updates to bdi_list. bdi_list has RCU reader side
 * locking.
 */
DEFINE_SPINLOCK(bdi_lock);
LIST_HEAD(bdi_list);

/* bdi_wq serves all asynchronous writeback tasks */
struct workqueue_struct *bdi_wq;

<<<<<<< HEAD
static void bdi_lock_two(struct bdi_writeback *wb1, struct bdi_writeback *wb2)
{
	if (wb1 < wb2) {
		spin_lock(&wb1->list_lock);
		spin_lock_nested(&wb2->list_lock, 1);
	} else {
		spin_lock(&wb2->list_lock);
		spin_lock_nested(&wb1->list_lock, 1);
	}
}

=======
>>>>>>> v4.9.227
#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#include <linux/seq_file.h>

static struct dentry *bdi_debug_root;

static void bdi_debug_init(void)
{
	bdi_debug_root = debugfs_create_dir("bdi", NULL);
}

static int bdi_debug_stats_show(struct seq_file *m, void *v)
{
	struct backing_dev_info *bdi = m->private;
	struct bdi_writeback *wb = &bdi->wb;
	unsigned long background_thresh;
	unsigned long dirty_thresh;
<<<<<<< HEAD
	unsigned long bdi_thresh;
=======
	unsigned long wb_thresh;
>>>>>>> v4.9.227
	unsigned long nr_dirty, nr_io, nr_more_io, nr_dirty_time;
	struct inode *inode;

	nr_dirty = nr_io = nr_more_io = nr_dirty_time = 0;
	spin_lock(&wb->list_lock);
<<<<<<< HEAD
	list_for_each_entry(inode, &wb->b_dirty, i_wb_list)
		nr_dirty++;
	list_for_each_entry(inode, &wb->b_io, i_wb_list)
		nr_io++;
	list_for_each_entry(inode, &wb->b_more_io, i_wb_list)
		nr_more_io++;
	list_for_each_entry(inode, &wb->b_dirty_time, i_wb_list)
=======
	list_for_each_entry(inode, &wb->b_dirty, i_io_list)
		nr_dirty++;
	list_for_each_entry(inode, &wb->b_io, i_io_list)
		nr_io++;
	list_for_each_entry(inode, &wb->b_more_io, i_io_list)
		nr_more_io++;
	list_for_each_entry(inode, &wb->b_dirty_time, i_io_list)
>>>>>>> v4.9.227
		if (inode->i_state & I_DIRTY_TIME)
			nr_dirty_time++;
	spin_unlock(&wb->list_lock);

	global_dirty_limits(&background_thresh, &dirty_thresh);
<<<<<<< HEAD
	bdi_thresh = bdi_dirty_limit(bdi, dirty_thresh);
=======
	wb_thresh = wb_calc_thresh(wb, dirty_thresh);
>>>>>>> v4.9.227

#define K(x) ((x) << (PAGE_SHIFT - 10))
	seq_printf(m,
		   "BdiWriteback:       %10lu kB\n"
		   "BdiReclaimable:     %10lu kB\n"
		   "BdiDirtyThresh:     %10lu kB\n"
		   "DirtyThresh:        %10lu kB\n"
		   "BackgroundThresh:   %10lu kB\n"
		   "BdiDirtied:         %10lu kB\n"
		   "BdiWritten:         %10lu kB\n"
		   "BdiWriteBandwidth:  %10lu kBps\n"
		   "b_dirty:            %10lu\n"
		   "b_io:               %10lu\n"
		   "b_more_io:          %10lu\n"
		   "b_dirty_time:       %10lu\n"
		   "bdi_list:           %10u\n"
		   "state:              %10lx\n",
<<<<<<< HEAD
		   (unsigned long) K(bdi_stat(bdi, BDI_WRITEBACK)),
		   (unsigned long) K(bdi_stat(bdi, BDI_RECLAIMABLE)),
		   K(bdi_thresh),
		   K(dirty_thresh),
		   K(background_thresh),
		   (unsigned long) K(bdi_stat(bdi, BDI_DIRTIED)),
		   (unsigned long) K(bdi_stat(bdi, BDI_WRITTEN)),
		   (unsigned long) K(bdi->write_bandwidth),
=======
		   (unsigned long) K(wb_stat(wb, WB_WRITEBACK)),
		   (unsigned long) K(wb_stat(wb, WB_RECLAIMABLE)),
		   K(wb_thresh),
		   K(dirty_thresh),
		   K(background_thresh),
		   (unsigned long) K(wb_stat(wb, WB_DIRTIED)),
		   (unsigned long) K(wb_stat(wb, WB_WRITTEN)),
		   (unsigned long) K(wb->write_bandwidth),
>>>>>>> v4.9.227
		   nr_dirty,
		   nr_io,
		   nr_more_io,
		   nr_dirty_time,
<<<<<<< HEAD
		   !list_empty(&bdi->bdi_list), bdi->state);
=======
		   !list_empty(&bdi->bdi_list), bdi->wb.state);
>>>>>>> v4.9.227
#undef K

	return 0;
}

static int bdi_debug_stats_open(struct inode *inode, struct file *file)
{
	return single_open(file, bdi_debug_stats_show, inode->i_private);
}

static const struct file_operations bdi_debug_stats_fops = {
	.open		= bdi_debug_stats_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static void bdi_debug_register(struct backing_dev_info *bdi, const char *name)
{
	bdi->debug_dir = debugfs_create_dir(name, bdi_debug_root);
	bdi->debug_stats = debugfs_create_file("stats", 0444, bdi->debug_dir,
					       bdi, &bdi_debug_stats_fops);
}

static void bdi_debug_unregister(struct backing_dev_info *bdi)
{
	debugfs_remove(bdi->debug_stats);
	debugfs_remove(bdi->debug_dir);
}
#else
static inline void bdi_debug_init(void)
{
}
static inline void bdi_debug_register(struct backing_dev_info *bdi,
				      const char *name)
{
}
static inline void bdi_debug_unregister(struct backing_dev_info *bdi)
{
}
#endif

static ssize_t read_ahead_kb_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	struct backing_dev_info *bdi = dev_get_drvdata(dev);
	unsigned long read_ahead_kb;
	ssize_t ret;
<<<<<<< HEAD
	static const char temp[] = "temporary ";

	if (strncmp(buf, temp, sizeof(temp) - 1) != 0)
		return count;

	buf += sizeof(temp) - 1;

	ret = kstrtoul(buf, 10, &read_ahead_kb);

=======

	ret = kstrtoul(buf, 10, &read_ahead_kb);
>>>>>>> v4.9.227
	if (ret < 0)
		return ret;

	bdi->ra_pages = read_ahead_kb >> (PAGE_SHIFT - 10);

	return count;
}

#define K(pages) ((pages) << (PAGE_SHIFT - 10))

#define BDI_SHOW(name, expr)						\
static ssize_t name##_show(struct device *dev,				\
			   struct device_attribute *attr, char *page)	\
{									\
	struct backing_dev_info *bdi = dev_get_drvdata(dev);		\
									\
	return snprintf(page, PAGE_SIZE-1, "%lld\n", (long long)expr);	\
}									\
static DEVICE_ATTR_RW(name);

BDI_SHOW(read_ahead_kb, K(bdi->ra_pages))

static ssize_t min_ratio_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct backing_dev_info *bdi = dev_get_drvdata(dev);
	unsigned int ratio;
	ssize_t ret;

	ret = kstrtouint(buf, 10, &ratio);
	if (ret < 0)
		return ret;

	ret = bdi_set_min_ratio(bdi, ratio);
	if (!ret)
		ret = count;

	return ret;
}
BDI_SHOW(min_ratio, bdi->min_ratio)

static ssize_t max_ratio_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct backing_dev_info *bdi = dev_get_drvdata(dev);
	unsigned int ratio;
	ssize_t ret;

	ret = kstrtouint(buf, 10, &ratio);
	if (ret < 0)
		return ret;

	ret = bdi_set_max_ratio(bdi, ratio);
	if (!ret)
		ret = count;

	return ret;
}
BDI_SHOW(max_ratio, bdi->max_ratio)

static ssize_t stable_pages_required_show(struct device *dev,
					  struct device_attribute *attr,
					  char *page)
{
	struct backing_dev_info *bdi = dev_get_drvdata(dev);

	return snprintf(page, PAGE_SIZE-1, "%d\n",
			bdi_cap_stable_pages_required(bdi) ? 1 : 0);
}
static DEVICE_ATTR_RO(stable_pages_required);

static struct attribute *bdi_dev_attrs[] = {
	&dev_attr_read_ahead_kb.attr,
	&dev_attr_min_ratio.attr,
	&dev_attr_max_ratio.attr,
	&dev_attr_stable_pages_required.attr,
	NULL,
};
ATTRIBUTE_GROUPS(bdi_dev);

static __init int bdi_class_init(void)
{
	bdi_class = class_create(THIS_MODULE, "bdi");
	if (IS_ERR(bdi_class))
		return PTR_ERR(bdi_class);

	bdi_class->dev_groups = bdi_dev_groups;
	bdi_debug_init();
	return 0;
}
postcore_initcall(bdi_class_init);

static int __init default_bdi_init(void)
{
	int err;

	bdi_wq = alloc_workqueue("writeback", WQ_MEM_RECLAIM | WQ_FREEZABLE |
					      WQ_UNBOUND | WQ_SYSFS, 0);
	if (!bdi_wq)
		return -ENOMEM;

<<<<<<< HEAD
	err = bdi_init(&default_backing_dev_info);
	if (!err)
		bdi_register(&default_backing_dev_info, NULL, "default");
=======
>>>>>>> v4.9.227
	err = bdi_init(&noop_backing_dev_info);

	return err;
}
subsys_initcall(default_bdi_init);

<<<<<<< HEAD
int bdi_has_dirty_io(struct backing_dev_info *bdi)
{
	return wb_has_dirty_io(&bdi->wb);
}

/*
 * This function is used when the first inode for this bdi is marked dirty. It
=======
/*
 * This function is used when the first inode for this wb is marked dirty. It
>>>>>>> v4.9.227
 * wakes-up the corresponding bdi thread which should then take care of the
 * periodic background write-out of dirty inodes. Since the write-out would
 * starts only 'dirty_writeback_interval' centisecs from now anyway, we just
 * set up a timer which wakes the bdi thread up later.
 *
 * Note, we wouldn't bother setting up the timer, but this function is on the
 * fast-path (used by '__mark_inode_dirty()'), so we save few context switches
 * by delaying the wake-up.
 *
 * We have to be careful not to postpone flush work if it is scheduled for
 * earlier. Thus we use queue_delayed_work().
 */
<<<<<<< HEAD
void bdi_wakeup_thread_delayed(struct backing_dev_info *bdi)
=======
void wb_wakeup_delayed(struct bdi_writeback *wb)
>>>>>>> v4.9.227
{
	unsigned long timeout;

	timeout = msecs_to_jiffies(dirty_writeback_interval * 10);
<<<<<<< HEAD
	spin_lock_bh(&bdi->wb_lock);
	if (test_bit(BDI_registered, &bdi->state))
		queue_delayed_work(bdi_wq, &bdi->wb.dwork, timeout);
	spin_unlock_bh(&bdi->wb_lock);
}

/*
 * Remove bdi from bdi_list, and ensure that it is no longer visible
 */
static void bdi_remove_from_list(struct backing_dev_info *bdi)
{
	spin_lock_bh(&bdi_lock);
	list_del_rcu(&bdi->bdi_list);
	spin_unlock_bh(&bdi_lock);

	synchronize_rcu_expedited();
}

=======
	spin_lock_bh(&wb->work_lock);
	if (test_bit(WB_registered, &wb->state))
		queue_delayed_work(bdi_wq, &wb->dwork, timeout);
	spin_unlock_bh(&wb->work_lock);
}

/*
 * Initial write bandwidth: 100 MB/s
 */
#define INIT_BW		(100 << (20 - PAGE_SHIFT))

static int wb_init(struct bdi_writeback *wb, struct backing_dev_info *bdi,
		   int blkcg_id, gfp_t gfp)
{
	int i, err;

	memset(wb, 0, sizeof(*wb));

	wb->bdi = bdi;
	wb->last_old_flush = jiffies;
	INIT_LIST_HEAD(&wb->b_dirty);
	INIT_LIST_HEAD(&wb->b_io);
	INIT_LIST_HEAD(&wb->b_more_io);
	INIT_LIST_HEAD(&wb->b_dirty_time);
	spin_lock_init(&wb->list_lock);

	wb->bw_time_stamp = jiffies;
	wb->balanced_dirty_ratelimit = INIT_BW;
	wb->dirty_ratelimit = INIT_BW;
	wb->write_bandwidth = INIT_BW;
	wb->avg_write_bandwidth = INIT_BW;

	spin_lock_init(&wb->work_lock);
	INIT_LIST_HEAD(&wb->work_list);
	INIT_DELAYED_WORK(&wb->dwork, wb_workfn);

	wb->congested = wb_congested_get_create(bdi, blkcg_id, gfp);
	if (!wb->congested)
		return -ENOMEM;

	err = fprop_local_init_percpu(&wb->completions, gfp);
	if (err)
		goto out_put_cong;

	for (i = 0; i < NR_WB_STAT_ITEMS; i++) {
		err = percpu_counter_init(&wb->stat[i], 0, gfp);
		if (err)
			goto out_destroy_stat;
	}

	return 0;

out_destroy_stat:
	while (i--)
		percpu_counter_destroy(&wb->stat[i]);
	fprop_local_destroy_percpu(&wb->completions);
out_put_cong:
	wb_congested_put(wb->congested);
	return err;
}

/*
 * Remove bdi from the global list and shutdown any threads we have running
 */
static void wb_shutdown(struct bdi_writeback *wb)
{
	/* Make sure nobody queues further work */
	spin_lock_bh(&wb->work_lock);
	if (!test_and_clear_bit(WB_registered, &wb->state)) {
		spin_unlock_bh(&wb->work_lock);
		return;
	}
	spin_unlock_bh(&wb->work_lock);

	/*
	 * Drain work list and shutdown the delayed_work.  !WB_registered
	 * tells wb_workfn() that @wb is dying and its work_list needs to
	 * be drained no matter what.
	 */
	mod_delayed_work(bdi_wq, &wb->dwork, 0);
	flush_delayed_work(&wb->dwork);
	WARN_ON(!list_empty(&wb->work_list));
}

static void wb_exit(struct bdi_writeback *wb)
{
	int i;

	WARN_ON(delayed_work_pending(&wb->dwork));

	for (i = 0; i < NR_WB_STAT_ITEMS; i++)
		percpu_counter_destroy(&wb->stat[i]);

	fprop_local_destroy_percpu(&wb->completions);
	wb_congested_put(wb->congested);
}

#ifdef CONFIG_CGROUP_WRITEBACK

#include <linux/memcontrol.h>

/*
 * cgwb_lock protects bdi->cgwb_tree, bdi->cgwb_congested_tree,
 * blkcg->cgwb_list, and memcg->cgwb_list.  bdi->cgwb_tree is also RCU
 * protected.  cgwb_release_wait is used to wait for the completion of cgwb
 * releases from bdi destruction path.
 */
static DEFINE_SPINLOCK(cgwb_lock);
static DECLARE_WAIT_QUEUE_HEAD(cgwb_release_wait);

/**
 * wb_congested_get_create - get or create a wb_congested
 * @bdi: associated bdi
 * @blkcg_id: ID of the associated blkcg
 * @gfp: allocation mask
 *
 * Look up the wb_congested for @blkcg_id on @bdi.  If missing, create one.
 * The returned wb_congested has its reference count incremented.  Returns
 * NULL on failure.
 */
struct bdi_writeback_congested *
wb_congested_get_create(struct backing_dev_info *bdi, int blkcg_id, gfp_t gfp)
{
	struct bdi_writeback_congested *new_congested = NULL, *congested;
	struct rb_node **node, *parent;
	unsigned long flags;
retry:
	spin_lock_irqsave(&cgwb_lock, flags);

	node = &bdi->cgwb_congested_tree.rb_node;
	parent = NULL;

	while (*node != NULL) {
		parent = *node;
		congested = container_of(parent, struct bdi_writeback_congested,
					 rb_node);
		if (congested->blkcg_id < blkcg_id)
			node = &parent->rb_left;
		else if (congested->blkcg_id > blkcg_id)
			node = &parent->rb_right;
		else
			goto found;
	}

	if (new_congested) {
		/* !found and storage for new one already allocated, insert */
		congested = new_congested;
		new_congested = NULL;
		rb_link_node(&congested->rb_node, parent, node);
		rb_insert_color(&congested->rb_node, &bdi->cgwb_congested_tree);
		goto found;
	}

	spin_unlock_irqrestore(&cgwb_lock, flags);

	/* allocate storage for new one and retry */
	new_congested = kzalloc(sizeof(*new_congested), gfp);
	if (!new_congested)
		return NULL;

	atomic_set(&new_congested->refcnt, 0);
	new_congested->bdi = bdi;
	new_congested->blkcg_id = blkcg_id;
	goto retry;

found:
	atomic_inc(&congested->refcnt);
	spin_unlock_irqrestore(&cgwb_lock, flags);
	kfree(new_congested);
	return congested;
}

/**
 * wb_congested_put - put a wb_congested
 * @congested: wb_congested to put
 *
 * Put @congested and destroy it if the refcnt reaches zero.
 */
void wb_congested_put(struct bdi_writeback_congested *congested)
{
	unsigned long flags;

	local_irq_save(flags);
	if (!atomic_dec_and_lock(&congested->refcnt, &cgwb_lock)) {
		local_irq_restore(flags);
		return;
	}

	/* bdi might already have been destroyed leaving @congested unlinked */
	if (congested->bdi) {
		rb_erase(&congested->rb_node,
			 &congested->bdi->cgwb_congested_tree);
		congested->bdi = NULL;
	}

	spin_unlock_irqrestore(&cgwb_lock, flags);
	kfree(congested);
}

static void cgwb_release_workfn(struct work_struct *work)
{
	struct bdi_writeback *wb = container_of(work, struct bdi_writeback,
						release_work);
	struct backing_dev_info *bdi = wb->bdi;

	spin_lock_irq(&cgwb_lock);
	list_del_rcu(&wb->bdi_node);
	spin_unlock_irq(&cgwb_lock);

	wb_shutdown(wb);

	css_put(wb->memcg_css);
	css_put(wb->blkcg_css);

	fprop_local_destroy_percpu(&wb->memcg_completions);
	percpu_ref_exit(&wb->refcnt);
	wb_exit(wb);
	kfree_rcu(wb, rcu);

	if (atomic_dec_and_test(&bdi->usage_cnt))
		wake_up_all(&cgwb_release_wait);
}

static void cgwb_release(struct percpu_ref *refcnt)
{
	struct bdi_writeback *wb = container_of(refcnt, struct bdi_writeback,
						refcnt);
	schedule_work(&wb->release_work);
}

static void cgwb_kill(struct bdi_writeback *wb)
{
	lockdep_assert_held(&cgwb_lock);

	WARN_ON(!radix_tree_delete(&wb->bdi->cgwb_tree, wb->memcg_css->id));
	list_del(&wb->memcg_node);
	list_del(&wb->blkcg_node);
	percpu_ref_kill(&wb->refcnt);
}

static int cgwb_create(struct backing_dev_info *bdi,
		       struct cgroup_subsys_state *memcg_css, gfp_t gfp)
{
	struct mem_cgroup *memcg;
	struct cgroup_subsys_state *blkcg_css;
	struct blkcg *blkcg;
	struct list_head *memcg_cgwb_list, *blkcg_cgwb_list;
	struct bdi_writeback *wb;
	unsigned long flags;
	int ret = 0;

	memcg = mem_cgroup_from_css(memcg_css);
	blkcg_css = cgroup_get_e_css(memcg_css->cgroup, &io_cgrp_subsys);
	blkcg = css_to_blkcg(blkcg_css);
	memcg_cgwb_list = mem_cgroup_cgwb_list(memcg);
	blkcg_cgwb_list = &blkcg->cgwb_list;

	/* look up again under lock and discard on blkcg mismatch */
	spin_lock_irqsave(&cgwb_lock, flags);
	wb = radix_tree_lookup(&bdi->cgwb_tree, memcg_css->id);
	if (wb && wb->blkcg_css != blkcg_css) {
		cgwb_kill(wb);
		wb = NULL;
	}
	spin_unlock_irqrestore(&cgwb_lock, flags);
	if (wb)
		goto out_put;

	/* need to create a new one */
	wb = kmalloc(sizeof(*wb), gfp);
	if (!wb)
		return -ENOMEM;

	ret = wb_init(wb, bdi, blkcg_css->id, gfp);
	if (ret)
		goto err_free;

	ret = percpu_ref_init(&wb->refcnt, cgwb_release, 0, gfp);
	if (ret)
		goto err_wb_exit;

	ret = fprop_local_init_percpu(&wb->memcg_completions, gfp);
	if (ret)
		goto err_ref_exit;

	wb->memcg_css = memcg_css;
	wb->blkcg_css = blkcg_css;
	INIT_WORK(&wb->release_work, cgwb_release_workfn);
	set_bit(WB_registered, &wb->state);

	/*
	 * The root wb determines the registered state of the whole bdi and
	 * memcg_cgwb_list and blkcg_cgwb_list's next pointers indicate
	 * whether they're still online.  Don't link @wb if any is dead.
	 * See wb_memcg_offline() and wb_blkcg_offline().
	 */
	ret = -ENODEV;
	spin_lock_irqsave(&cgwb_lock, flags);
	if (test_bit(WB_registered, &bdi->wb.state) &&
	    blkcg_cgwb_list->next && memcg_cgwb_list->next) {
		/* we might have raced another instance of this function */
		ret = radix_tree_insert(&bdi->cgwb_tree, memcg_css->id, wb);
		if (!ret) {
			atomic_inc(&bdi->usage_cnt);
			list_add_tail_rcu(&wb->bdi_node, &bdi->wb_list);
			list_add(&wb->memcg_node, memcg_cgwb_list);
			list_add(&wb->blkcg_node, blkcg_cgwb_list);
			css_get(memcg_css);
			css_get(blkcg_css);
		}
	}
	spin_unlock_irqrestore(&cgwb_lock, flags);
	if (ret) {
		if (ret == -EEXIST)
			ret = 0;
		goto err_fprop_exit;
	}
	goto out_put;

err_fprop_exit:
	fprop_local_destroy_percpu(&wb->memcg_completions);
err_ref_exit:
	percpu_ref_exit(&wb->refcnt);
err_wb_exit:
	wb_exit(wb);
err_free:
	kfree(wb);
out_put:
	css_put(blkcg_css);
	return ret;
}

/**
 * wb_get_create - get wb for a given memcg, create if necessary
 * @bdi: target bdi
 * @memcg_css: cgroup_subsys_state of the target memcg (must have positive ref)
 * @gfp: allocation mask to use
 *
 * Try to get the wb for @memcg_css on @bdi.  If it doesn't exist, try to
 * create one.  The returned wb has its refcount incremented.
 *
 * This function uses css_get() on @memcg_css and thus expects its refcnt
 * to be positive on invocation.  IOW, rcu_read_lock() protection on
 * @memcg_css isn't enough.  try_get it before calling this function.
 *
 * A wb is keyed by its associated memcg.  As blkcg implicitly enables
 * memcg on the default hierarchy, memcg association is guaranteed to be
 * more specific (equal or descendant to the associated blkcg) and thus can
 * identify both the memcg and blkcg associations.
 *
 * Because the blkcg associated with a memcg may change as blkcg is enabled
 * and disabled closer to root in the hierarchy, each wb keeps track of
 * both the memcg and blkcg associated with it and verifies the blkcg on
 * each lookup.  On mismatch, the existing wb is discarded and a new one is
 * created.
 */
struct bdi_writeback *wb_get_create(struct backing_dev_info *bdi,
				    struct cgroup_subsys_state *memcg_css,
				    gfp_t gfp)
{
	struct bdi_writeback *wb;

	might_sleep_if(gfpflags_allow_blocking(gfp));

	if (!memcg_css->parent)
		return &bdi->wb;

	do {
		rcu_read_lock();
		wb = radix_tree_lookup(&bdi->cgwb_tree, memcg_css->id);
		if (wb) {
			struct cgroup_subsys_state *blkcg_css;

			/* see whether the blkcg association has changed */
			blkcg_css = cgroup_get_e_css(memcg_css->cgroup,
						     &io_cgrp_subsys);
			if (unlikely(wb->blkcg_css != blkcg_css ||
				     !wb_tryget(wb)))
				wb = NULL;
			css_put(blkcg_css);
		}
		rcu_read_unlock();
	} while (!wb && !cgwb_create(bdi, memcg_css, gfp));

	return wb;
}

static int cgwb_bdi_init(struct backing_dev_info *bdi)
{
	int ret;

	INIT_RADIX_TREE(&bdi->cgwb_tree, GFP_ATOMIC);
	bdi->cgwb_congested_tree = RB_ROOT;
	atomic_set(&bdi->usage_cnt, 1);
	init_rwsem(&bdi->wb_switch_rwsem);

	ret = wb_init(&bdi->wb, bdi, 1, GFP_KERNEL);
	if (!ret) {
		bdi->wb.memcg_css = &root_mem_cgroup->css;
		bdi->wb.blkcg_css = blkcg_root_css;
	}
	return ret;
}

static void cgwb_bdi_destroy(struct backing_dev_info *bdi)
{
	struct radix_tree_iter iter;
	struct rb_node *rbn;
	void **slot;

	WARN_ON(test_bit(WB_registered, &bdi->wb.state));

	spin_lock_irq(&cgwb_lock);

	radix_tree_for_each_slot(slot, &bdi->cgwb_tree, &iter, 0)
		cgwb_kill(*slot);

	while ((rbn = rb_first(&bdi->cgwb_congested_tree))) {
		struct bdi_writeback_congested *congested =
			rb_entry(rbn, struct bdi_writeback_congested, rb_node);

		rb_erase(rbn, &bdi->cgwb_congested_tree);
		congested->bdi = NULL;	/* mark @congested unlinked */
	}

	spin_unlock_irq(&cgwb_lock);

	/*
	 * All cgwb's and their congested states must be shutdown and
	 * released before returning.  Drain the usage counter to wait for
	 * all cgwb's and cgwb_congested's ever created on @bdi.
	 */
	atomic_dec(&bdi->usage_cnt);
	wait_event(cgwb_release_wait, !atomic_read(&bdi->usage_cnt));
}

/**
 * wb_memcg_offline - kill all wb's associated with a memcg being offlined
 * @memcg: memcg being offlined
 *
 * Also prevents creation of any new wb's associated with @memcg.
 */
void wb_memcg_offline(struct mem_cgroup *memcg)
{
	LIST_HEAD(to_destroy);
	struct list_head *memcg_cgwb_list = mem_cgroup_cgwb_list(memcg);
	struct bdi_writeback *wb, *next;

	spin_lock_irq(&cgwb_lock);
	list_for_each_entry_safe(wb, next, memcg_cgwb_list, memcg_node)
		cgwb_kill(wb);
	memcg_cgwb_list->next = NULL;	/* prevent new wb's */
	spin_unlock_irq(&cgwb_lock);
}

/**
 * wb_blkcg_offline - kill all wb's associated with a blkcg being offlined
 * @blkcg: blkcg being offlined
 *
 * Also prevents creation of any new wb's associated with @blkcg.
 */
void wb_blkcg_offline(struct blkcg *blkcg)
{
	LIST_HEAD(to_destroy);
	struct bdi_writeback *wb, *next;

	spin_lock_irq(&cgwb_lock);
	list_for_each_entry_safe(wb, next, &blkcg->cgwb_list, blkcg_node)
		cgwb_kill(wb);
	blkcg->cgwb_list.next = NULL;	/* prevent new wb's */
	spin_unlock_irq(&cgwb_lock);
}

#else	/* CONFIG_CGROUP_WRITEBACK */

static int cgwb_bdi_init(struct backing_dev_info *bdi)
{
	int err;

	bdi->wb_congested = kzalloc(sizeof(*bdi->wb_congested), GFP_KERNEL);
	if (!bdi->wb_congested)
		return -ENOMEM;

	atomic_set(&bdi->wb_congested->refcnt, 1);

	err = wb_init(&bdi->wb, bdi, 1, GFP_KERNEL);
	if (err) {
		wb_congested_put(bdi->wb_congested);
		return err;
	}
	return 0;
}

static void cgwb_bdi_destroy(struct backing_dev_info *bdi)
{
	wb_congested_put(bdi->wb_congested);
}

#endif	/* CONFIG_CGROUP_WRITEBACK */

int bdi_init(struct backing_dev_info *bdi)
{
	int ret;

	bdi->dev = NULL;

	bdi->min_ratio = 0;
	bdi->max_ratio = 100;
	bdi->max_prop_frac = FPROP_FRAC_BASE;
	INIT_LIST_HEAD(&bdi->bdi_list);
	INIT_LIST_HEAD(&bdi->wb_list);
	init_waitqueue_head(&bdi->wb_waitq);

	ret = cgwb_bdi_init(bdi);

	list_add_tail_rcu(&bdi->wb.bdi_node, &bdi->wb_list);

	return ret;
}
EXPORT_SYMBOL(bdi_init);

>>>>>>> v4.9.227
int bdi_register(struct backing_dev_info *bdi, struct device *parent,
		const char *fmt, ...)
{
	va_list args;
	struct device *dev;

	if (bdi->dev)	/* The driver needs to use separate queues per device */
		return 0;

	va_start(args, fmt);
	dev = device_create_vargs(bdi_class, parent, MKDEV(0, 0), bdi, fmt, args);
	va_end(args);
	if (IS_ERR(dev))
		return PTR_ERR(dev);

	bdi->dev = dev;

	bdi_debug_register(bdi, dev_name(dev));
<<<<<<< HEAD
	set_bit(BDI_registered, &bdi->state);
=======
	set_bit(WB_registered, &bdi->wb.state);
>>>>>>> v4.9.227

	spin_lock_bh(&bdi_lock);
	list_add_tail_rcu(&bdi->bdi_list, &bdi_list);
	spin_unlock_bh(&bdi_lock);

	trace_writeback_bdi_register(bdi);
	return 0;
}
EXPORT_SYMBOL(bdi_register);

int bdi_register_dev(struct backing_dev_info *bdi, dev_t dev)
{
	return bdi_register(bdi, NULL, "%u:%u", MAJOR(dev), MINOR(dev));
}
EXPORT_SYMBOL(bdi_register_dev);

<<<<<<< HEAD
/*
 * Remove bdi from the global list and shutdown any threads we have running
 */
static void bdi_wb_shutdown(struct backing_dev_info *bdi)
{
	if (!bdi_cap_writeback_dirty(bdi))
		return;

	/*
	 * Make sure nobody finds us on the bdi_list anymore
	 */
	bdi_remove_from_list(bdi);

	/* Make sure nobody queues further work */
	spin_lock_bh(&bdi->wb_lock);
	clear_bit(BDI_registered, &bdi->state);
	spin_unlock_bh(&bdi->wb_lock);

	/*
	 * Drain work list and shutdown the delayed_work.  At this point,
	 * @bdi->bdi_list is empty telling bdi_Writeback_workfn() that @bdi
	 * is dying and its work_list needs to be drained no matter what.
	 */
	mod_delayed_work(bdi_wq, &bdi->wb.dwork, 0);
	flush_delayed_work(&bdi->wb.dwork);
	WARN_ON(!list_empty(&bdi->work_list));
	WARN_ON(delayed_work_pending(&bdi->wb.dwork));
}

/*
 * This bdi is going away now, make sure that no super_blocks point to it
 */
static void bdi_prune_sb(struct backing_dev_info *bdi)
{
	struct super_block *sb;

	spin_lock(&sb_lock);
	list_for_each_entry(sb, &super_blocks, s_list) {
		if (sb->s_bdi == bdi)
			sb->s_bdi = &default_backing_dev_info;
	}
	spin_unlock(&sb_lock);
=======
int bdi_register_owner(struct backing_dev_info *bdi, struct device *owner)
{
	int rc;

	rc = bdi_register(bdi, NULL, "%u:%u", MAJOR(owner->devt),
			MINOR(owner->devt));
	if (rc)
		return rc;
	bdi->owner = owner;
	get_device(owner);
	return 0;
}
EXPORT_SYMBOL(bdi_register_owner);

/*
 * Remove bdi from bdi_list, and ensure that it is no longer visible
 */
static void bdi_remove_from_list(struct backing_dev_info *bdi)
{
	spin_lock_bh(&bdi_lock);
	list_del_rcu(&bdi->bdi_list);
	spin_unlock_bh(&bdi_lock);

	synchronize_rcu_expedited();
>>>>>>> v4.9.227
}

void bdi_unregister(struct backing_dev_info *bdi)
{
<<<<<<< HEAD
	if (bdi->dev) {
		bdi_set_min_ratio(bdi, 0);
		trace_writeback_bdi_unregister(bdi);
		bdi_prune_sb(bdi);

		bdi_wb_shutdown(bdi);
=======
	/* make sure nobody finds us on the bdi_list anymore */
	bdi_remove_from_list(bdi);
	wb_shutdown(&bdi->wb);
	cgwb_bdi_destroy(bdi);

	if (bdi->dev) {
>>>>>>> v4.9.227
		bdi_debug_unregister(bdi);
		device_unregister(bdi->dev);
		bdi->dev = NULL;
	}
<<<<<<< HEAD
}
EXPORT_SYMBOL(bdi_unregister);

static void bdi_wb_init(struct bdi_writeback *wb, struct backing_dev_info *bdi)
{
	memset(wb, 0, sizeof(*wb));

	wb->bdi = bdi;
	wb->last_old_flush = jiffies;
	INIT_LIST_HEAD(&wb->b_dirty);
	INIT_LIST_HEAD(&wb->b_io);
	INIT_LIST_HEAD(&wb->b_more_io);
	INIT_LIST_HEAD(&wb->b_dirty_time);
	spin_lock_init(&wb->list_lock);
	INIT_DELAYED_WORK(&wb->dwork, bdi_writeback_workfn);
}

/*
 * Initial write bandwidth: 100 MB/s
 */
#define INIT_BW		(100 << (20 - PAGE_SHIFT))

int bdi_init(struct backing_dev_info *bdi)
{
	int i, err;

	bdi->dev = NULL;

	bdi->min_ratio = 0;
	bdi->max_ratio = 100;
	bdi->max_prop_frac = FPROP_FRAC_BASE;
	spin_lock_init(&bdi->wb_lock);
	INIT_LIST_HEAD(&bdi->bdi_list);
	INIT_LIST_HEAD(&bdi->work_list);

	bdi_wb_init(&bdi->wb, bdi);

	bdi->last_thresh = 0;
	bdi->last_nr_dirty = 0;
	bdi->paused_total = 0;

	for (i = 0; i < NR_BDI_STAT_ITEMS; i++) {
		err = percpu_counter_init(&bdi->bdi_stat[i], 0, GFP_KERNEL);
		if (err)
			goto err;
	}

	bdi->dirty_exceeded = 0;

	bdi->bw_time_stamp = jiffies;
	bdi->written_stamp = 0;

	bdi->balanced_dirty_ratelimit = INIT_BW;
	bdi->dirty_ratelimit = INIT_BW;
	bdi->write_bandwidth = INIT_BW;
	bdi->avg_write_bandwidth = INIT_BW;

	err = fprop_local_init_percpu(&bdi->completions, GFP_KERNEL);

	if (err) {
err:
		while (i--)
			percpu_counter_destroy(&bdi->bdi_stat[i]);
	}

	return err;
}
EXPORT_SYMBOL(bdi_init);

void bdi_destroy(struct backing_dev_info *bdi)
{
	int i;

	/*
	 * Splice our entries to the default_backing_dev_info.  This
	 * condition shouldn't happen.  @wb must be empty at this point and
	 * dirty inodes on it might cause other issues.  This workaround is
	 * added by ce5f8e779519 ("writeback: splice dirty inode entries to
	 * default bdi on bdi_destroy()") without root-causing the issue.
	 *
	 * http://lkml.kernel.org/g/1253038617-30204-11-git-send-email-jens.axboe@oracle.com
	 * http://thread.gmane.org/gmane.linux.file-systems/35341/focus=35350
	 *
	 * We should probably add WARN_ON() to find out whether it still
	 * happens and track it down if so.
	 */
	if (bdi_has_dirty_io(bdi)) {
		struct bdi_writeback *dst = &default_backing_dev_info.wb;

		bdi_lock_two(&bdi->wb, dst);
		list_splice(&bdi->wb.b_dirty, &dst->b_dirty);
		list_splice(&bdi->wb.b_io, &dst->b_io);
		list_splice(&bdi->wb.b_more_io, &dst->b_more_io);
		spin_unlock(&bdi->wb.list_lock);
		spin_unlock(&dst->list_lock);
	}

	bdi_unregister(bdi);

	WARN_ON(delayed_work_pending(&bdi->wb.dwork));

	for (i = 0; i < NR_BDI_STAT_ITEMS; i++)
		percpu_counter_destroy(&bdi->bdi_stat[i]);

	fprop_local_destroy_percpu(&bdi->completions);
=======

	if (bdi->owner) {
		put_device(bdi->owner);
		bdi->owner = NULL;
	}
}

void bdi_exit(struct backing_dev_info *bdi)
{
	WARN_ON_ONCE(bdi->dev);
	wb_exit(&bdi->wb);
}

void bdi_destroy(struct backing_dev_info *bdi)
{
	bdi_unregister(bdi);
	bdi_exit(bdi);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(bdi_destroy);

/*
 * For use from filesystems to quickly init and register a bdi associated
 * with dirty writeback
 */
<<<<<<< HEAD
int bdi_setup_and_register(struct backing_dev_info *bdi, char *name,
			   unsigned int cap)
=======
int bdi_setup_and_register(struct backing_dev_info *bdi, char *name)
>>>>>>> v4.9.227
{
	int err;

	bdi->name = name;
<<<<<<< HEAD
	bdi->capabilities = cap;
=======
	bdi->capabilities = 0;
>>>>>>> v4.9.227
	err = bdi_init(bdi);
	if (err)
		return err;

	err = bdi_register(bdi, NULL, "%.28s-%ld", name,
			   atomic_long_inc_return(&bdi_seq));
	if (err) {
		bdi_destroy(bdi);
		return err;
	}

	return 0;
}
EXPORT_SYMBOL(bdi_setup_and_register);

static wait_queue_head_t congestion_wqh[2] = {
		__WAIT_QUEUE_HEAD_INITIALIZER(congestion_wqh[0]),
		__WAIT_QUEUE_HEAD_INITIALIZER(congestion_wqh[1])
	};
<<<<<<< HEAD
static atomic_t nr_bdi_congested[2];

void clear_bdi_congested(struct backing_dev_info *bdi, int sync)
{
	enum bdi_state bit;
	wait_queue_head_t *wqh = &congestion_wqh[sync];

	bit = sync ? BDI_sync_congested : BDI_async_congested;
	if (test_and_clear_bit(bit, &bdi->state))
		atomic_dec(&nr_bdi_congested[sync]);
=======
static atomic_t nr_wb_congested[2];

void clear_wb_congested(struct bdi_writeback_congested *congested, int sync)
{
	wait_queue_head_t *wqh = &congestion_wqh[sync];
	enum wb_congested_state bit;

	bit = sync ? WB_sync_congested : WB_async_congested;
	if (test_and_clear_bit(bit, &congested->state))
		atomic_dec(&nr_wb_congested[sync]);
>>>>>>> v4.9.227
	smp_mb__after_atomic();
	if (waitqueue_active(wqh))
		wake_up(wqh);
}
<<<<<<< HEAD
EXPORT_SYMBOL(clear_bdi_congested);

void set_bdi_congested(struct backing_dev_info *bdi, int sync)
{
	enum bdi_state bit;

	bit = sync ? BDI_sync_congested : BDI_async_congested;
	if (!test_and_set_bit(bit, &bdi->state))
		atomic_inc(&nr_bdi_congested[sync]);
}
EXPORT_SYMBOL(set_bdi_congested);
=======
EXPORT_SYMBOL(clear_wb_congested);

void set_wb_congested(struct bdi_writeback_congested *congested, int sync)
{
	enum wb_congested_state bit;

	bit = sync ? WB_sync_congested : WB_async_congested;
	if (!test_and_set_bit(bit, &congested->state))
		atomic_inc(&nr_wb_congested[sync]);
}
EXPORT_SYMBOL(set_wb_congested);
>>>>>>> v4.9.227

/**
 * congestion_wait - wait for a backing_dev to become uncongested
 * @sync: SYNC or ASYNC IO
 * @timeout: timeout in jiffies
 *
 * Waits for up to @timeout jiffies for a backing_dev (any backing_dev) to exit
 * write congestion.  If no backing_devs are congested then just wait for the
 * next write to be completed.
 */
long congestion_wait(int sync, long timeout)
{
	long ret;
	unsigned long start = jiffies;
	DEFINE_WAIT(wait);
	wait_queue_head_t *wqh = &congestion_wqh[sync];

	prepare_to_wait(wqh, &wait, TASK_UNINTERRUPTIBLE);
	ret = io_schedule_timeout(timeout);
	finish_wait(wqh, &wait);

	trace_writeback_congestion_wait(jiffies_to_usecs(timeout),
					jiffies_to_usecs(jiffies - start));

	return ret;
}
EXPORT_SYMBOL(congestion_wait);

/**
<<<<<<< HEAD
 * wait_iff_congested - Conditionally wait for a backing_dev to become uncongested or a zone to complete writes
 * @zone: A zone to check if it is heavily congested
=======
 * wait_iff_congested - Conditionally wait for a backing_dev to become uncongested or a pgdat to complete writes
 * @pgdat: A pgdat to check if it is heavily congested
>>>>>>> v4.9.227
 * @sync: SYNC or ASYNC IO
 * @timeout: timeout in jiffies
 *
 * In the event of a congested backing_dev (any backing_dev) and the given
<<<<<<< HEAD
 * @zone has experienced recent congestion, this waits for up to @timeout
 * jiffies for either a BDI to exit congestion of the given @sync queue
 * or a write to complete.
 *
 * In the absence of zone congestion, cond_resched() is called to yield
=======
 * @pgdat has experienced recent congestion, this waits for up to @timeout
 * jiffies for either a BDI to exit congestion of the given @sync queue
 * or a write to complete.
 *
 * In the absence of pgdat congestion, cond_resched() is called to yield
>>>>>>> v4.9.227
 * the processor if necessary but otherwise does not sleep.
 *
 * The return value is 0 if the sleep is for the full timeout. Otherwise,
 * it is the number of jiffies that were still remaining when the function
 * returned. return_value == timeout implies the function did not sleep.
 */
<<<<<<< HEAD
long wait_iff_congested(struct zone *zone, int sync, long timeout)
=======
long wait_iff_congested(struct pglist_data *pgdat, int sync, long timeout)
>>>>>>> v4.9.227
{
	long ret;
	unsigned long start = jiffies;
	DEFINE_WAIT(wait);
	wait_queue_head_t *wqh = &congestion_wqh[sync];

	/*
	 * If there is no congestion, or heavy congestion is not being
<<<<<<< HEAD
	 * encountered in the current zone, yield if necessary instead
	 * of sleeping on the congestion queue
	 */
	if (atomic_read(&nr_bdi_congested[sync]) == 0 ||
	    !test_bit(ZONE_CONGESTED, &zone->flags)) {
=======
	 * encountered in the current pgdat, yield if necessary instead
	 * of sleeping on the congestion queue
	 */
	if (atomic_read(&nr_wb_congested[sync]) == 0 ||
	    !test_bit(PGDAT_CONGESTED, &pgdat->flags)) {
>>>>>>> v4.9.227
		cond_resched();

		/* In case we scheduled, work out time remaining */
		ret = timeout - (jiffies - start);
		if (ret < 0)
			ret = 0;

		goto out;
	}

	/* Sleep until uncongested or a write happens */
	prepare_to_wait(wqh, &wait, TASK_UNINTERRUPTIBLE);
	ret = io_schedule_timeout(timeout);
	finish_wait(wqh, &wait);

out:
	trace_writeback_wait_iff_congested(jiffies_to_usecs(timeout),
					jiffies_to_usecs(jiffies - start));

	return ret;
}
EXPORT_SYMBOL(wait_iff_congested);

int pdflush_proc_obsolete(struct ctl_table *table, int write,
			void __user *buffer, size_t *lenp, loff_t *ppos)
{
	char kbuf[] = "0\n";

	if (*ppos || *lenp < sizeof(kbuf)) {
		*lenp = 0;
		return 0;
	}

	if (copy_to_user(buffer, kbuf, sizeof(kbuf)))
		return -EFAULT;
<<<<<<< HEAD
	printk_once(KERN_WARNING "%s exported in /proc is scheduled for removal\n",
			table->procname);
=======
	pr_warn_once("%s exported in /proc is scheduled for removal\n",
		     table->procname);
>>>>>>> v4.9.227

	*lenp = 2;
	*ppos += *lenp;
	return 2;
}
