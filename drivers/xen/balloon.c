/******************************************************************************
 * Xen balloon driver - enables returning/claiming memory to/from Xen.
 *
 * Copyright (c) 2003, B Dragovic
 * Copyright (c) 2003-2004, M Williamson, K Fraser
 * Copyright (c) 2005 Dan M. Smith, IBM Corporation
 * Copyright (c) 2010 Daniel Kiper
 *
 * Memory hotplug support was written by Daniel Kiper. Work on
 * it was sponsored by Google under Google Summer of Code 2010
 * program. Jeremy Fitzhardinge from Citrix was the mentor for
 * this project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation; or, when distributed
 * separately from the Linux kernel or incorporated into other
 * software packages, subject to the following license:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this source file (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#define pr_fmt(fmt) "xen:" KBUILD_MODNAME ": " fmt

#include <linux/cpu.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/errno.h>
<<<<<<< HEAD
#include <linux/module.h>
=======
>>>>>>> v4.9.227
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/pagemap.h>
#include <linux/highmem.h>
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/gfp.h>
#include <linux/notifier.h>
#include <linux/memory.h>
#include <linux/memory_hotplug.h>
#include <linux/percpu-defs.h>
<<<<<<< HEAD
=======
#include <linux/slab.h>
#include <linux/sysctl.h>
>>>>>>> v4.9.227

#include <asm/page.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>
#include <asm/tlb.h>

#include <asm/xen/hypervisor.h>
#include <asm/xen/hypercall.h>

#include <xen/xen.h>
#include <xen/interface/xen.h>
#include <xen/interface/memory.h>
#include <xen/balloon.h>
#include <xen/features.h>
#include <xen/page.h>

<<<<<<< HEAD
=======
static int xen_hotplug_unpopulated;

#ifdef CONFIG_XEN_BALLOON_MEMORY_HOTPLUG

static int zero;
static int one = 1;

static struct ctl_table balloon_table[] = {
	{
		.procname	= "hotplug_unpopulated",
		.data		= &xen_hotplug_unpopulated,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1         = &zero,
		.extra2         = &one,
	},
	{ }
};

static struct ctl_table balloon_root[] = {
	{
		.procname	= "balloon",
		.mode		= 0555,
		.child		= balloon_table,
	},
	{ }
};

static struct ctl_table xen_root[] = {
	{
		.procname	= "xen",
		.mode		= 0555,
		.child		= balloon_root,
	},
	{ }
};

#endif

/*
 * Use one extent per PAGE_SIZE to avoid to break down the page into
 * multiple frame.
 */
#define EXTENT_ORDER (fls(XEN_PFN_PER_PAGE) - 1)

>>>>>>> v4.9.227
/*
 * balloon_process() state:
 *
 * BP_DONE: done or nothing to do,
<<<<<<< HEAD
=======
 * BP_WAIT: wait to be rescheduled,
>>>>>>> v4.9.227
 * BP_EAGAIN: error, go to sleep,
 * BP_ECANCELED: error, balloon operation canceled.
 */

enum bp_state {
	BP_DONE,
<<<<<<< HEAD
=======
	BP_WAIT,
>>>>>>> v4.9.227
	BP_EAGAIN,
	BP_ECANCELED
};


static DEFINE_MUTEX(balloon_mutex);

struct balloon_stats balloon_stats;
EXPORT_SYMBOL_GPL(balloon_stats);

/* We increase/decrease in batches which fit in a page */
<<<<<<< HEAD
static xen_pfn_t frame_list[PAGE_SIZE / sizeof(unsigned long)];
static DEFINE_PER_CPU(struct page *, balloon_scratch_page);
=======
static xen_pfn_t frame_list[PAGE_SIZE / sizeof(xen_pfn_t)];
>>>>>>> v4.9.227


/* List of ballooned pages, threaded through the mem_map array. */
static LIST_HEAD(ballooned_pages);
<<<<<<< HEAD
=======
static DECLARE_WAIT_QUEUE_HEAD(balloon_wq);
>>>>>>> v4.9.227

/* Main work function, always executed in process context. */
static void balloon_process(struct work_struct *work);
static DECLARE_DELAYED_WORK(balloon_worker, balloon_process);

/* When ballooning out (allocating memory to return to Xen) we don't really
   want the kernel to try too hard since that can trigger the oom killer. */
#define GFP_BALLOON \
	(GFP_HIGHUSER | __GFP_NOWARN | __GFP_NORETRY | __GFP_NOMEMALLOC)

static void scrub_page(struct page *page)
{
#ifdef CONFIG_XEN_SCRUB_PAGES
	clear_highpage(page);
#endif
}

/* balloon_append: add the given page to the balloon. */
static void __balloon_append(struct page *page)
{
	/* Lowmem is re-populated first, so highmem pages go at list tail. */
	if (PageHighMem(page)) {
		list_add_tail(&page->lru, &ballooned_pages);
		balloon_stats.balloon_high++;
	} else {
		list_add(&page->lru, &ballooned_pages);
		balloon_stats.balloon_low++;
	}
<<<<<<< HEAD
=======
	wake_up(&balloon_wq);
>>>>>>> v4.9.227
}

static void balloon_append(struct page *page)
{
	__balloon_append(page);
	adjust_managed_page_count(page, -1);
}

/* balloon_retrieve: rescue a page from the balloon, if it is not empty. */
<<<<<<< HEAD
static struct page *balloon_retrieve(bool prefer_highmem)
=======
static struct page *balloon_retrieve(bool require_lowmem)
>>>>>>> v4.9.227
{
	struct page *page;

	if (list_empty(&ballooned_pages))
		return NULL;

<<<<<<< HEAD
	if (prefer_highmem)
		page = list_entry(ballooned_pages.prev, struct page, lru);
	else
		page = list_entry(ballooned_pages.next, struct page, lru);
=======
	page = list_entry(ballooned_pages.next, struct page, lru);
	if (require_lowmem && PageHighMem(page))
		return NULL;
>>>>>>> v4.9.227
	list_del(&page->lru);

	if (PageHighMem(page))
		balloon_stats.balloon_high--;
	else
		balloon_stats.balloon_low--;

	adjust_managed_page_count(page, 1);

	return page;
}

static struct page *balloon_next_page(struct page *page)
{
	struct list_head *next = page->lru.next;
	if (next == &ballooned_pages)
		return NULL;
	return list_entry(next, struct page, lru);
}

static enum bp_state update_schedule(enum bp_state state)
{
<<<<<<< HEAD
=======
	if (state == BP_WAIT)
		return BP_WAIT;

>>>>>>> v4.9.227
	if (state == BP_ECANCELED)
		return BP_ECANCELED;

	if (state == BP_DONE) {
		balloon_stats.schedule_delay = 1;
		balloon_stats.retry_count = 1;
		return BP_DONE;
	}

	++balloon_stats.retry_count;

	if (balloon_stats.max_retry_count != RETRY_UNLIMITED &&
			balloon_stats.retry_count > balloon_stats.max_retry_count) {
		balloon_stats.schedule_delay = 1;
		balloon_stats.retry_count = 1;
		return BP_ECANCELED;
	}

	balloon_stats.schedule_delay <<= 1;

	if (balloon_stats.schedule_delay > balloon_stats.max_schedule_delay)
		balloon_stats.schedule_delay = balloon_stats.max_schedule_delay;

	return BP_EAGAIN;
}

#ifdef CONFIG_XEN_BALLOON_MEMORY_HOTPLUG
<<<<<<< HEAD
static long current_credit(void)
{
	return balloon_stats.target_pages - balloon_stats.current_pages -
		balloon_stats.hotplug_pages;
}

static bool balloon_is_inflated(void)
{
	if (balloon_stats.balloon_low || balloon_stats.balloon_high ||
			balloon_stats.balloon_hotplug)
		return true;
	else
		return false;
}

/*
 * reserve_additional_memory() adds memory region of size >= credit above
 * max_pfn. New region is section aligned and size is modified to be multiple
 * of section size. Those features allow optimal use of address space and
 * establish proper alignment when this function is called first time after
 * boot (last section not fully populated at boot time contains unused memory
 * pages with PG_reserved bit not set; online_pages_range() does not allow page
 * onlining in whole range if first onlined page does not have PG_reserved
 * bit set). Real size of added memory is established at page onlining stage.
 */

static enum bp_state reserve_additional_memory(long credit)
{
	int nid, rc;
	u64 hotplug_start_paddr;
	unsigned long balloon_hotplug = credit;

	hotplug_start_paddr = PFN_PHYS(SECTION_ALIGN_UP(max_pfn));
	balloon_hotplug = round_up(balloon_hotplug, PAGES_PER_SECTION);
	nid = memory_add_physaddr_to_nid(hotplug_start_paddr);

#ifdef CONFIG_XEN_HAVE_PVMMU
=======
static void release_memory_resource(struct resource *resource)
{
	if (!resource)
		return;

	/*
	 * No need to reset region to identity mapped since we now
	 * know that no I/O can be in this region
	 */
	release_resource(resource);
	kfree(resource);
}

static struct resource *additional_memory_resource(phys_addr_t size)
{
	struct resource *res;
	int ret;

	res = kzalloc(sizeof(*res), GFP_KERNEL);
	if (!res)
		return NULL;

	res->name = "System RAM";
	res->flags = IORESOURCE_SYSTEM_RAM | IORESOURCE_BUSY;

	ret = allocate_resource(&iomem_resource, res,
				size, 0, -1,
				PAGES_PER_SECTION * PAGE_SIZE, NULL, NULL);
	if (ret < 0) {
		pr_err("Cannot allocate new System RAM resource\n");
		kfree(res);
		return NULL;
	}

#ifdef CONFIG_SPARSEMEM
	{
		unsigned long limit = 1UL << (MAX_PHYSMEM_BITS - PAGE_SHIFT);
		unsigned long pfn = res->start >> PAGE_SHIFT;

		if (pfn > limit) {
			pr_err("New System RAM resource outside addressable RAM (%lu > %lu)\n",
			       pfn, limit);
			release_memory_resource(res);
			return NULL;
		}
	}
#endif

	return res;
}

static enum bp_state reserve_additional_memory(void)
{
	long credit;
	struct resource *resource;
	int nid, rc;
	unsigned long balloon_hotplug;

	credit = balloon_stats.target_pages + balloon_stats.target_unpopulated
		- balloon_stats.total_pages;

	/*
	 * Already hotplugged enough pages?  Wait for them to be
	 * onlined.
	 */
	if (credit <= 0)
		return BP_WAIT;

	balloon_hotplug = round_up(credit, PAGES_PER_SECTION);

	resource = additional_memory_resource(balloon_hotplug * PAGE_SIZE);
	if (!resource)
		goto err;

	nid = memory_add_physaddr_to_nid(resource->start);

#ifdef CONFIG_XEN_HAVE_PVMMU
	/*
	 * We don't support PV MMU when Linux and Xen is using
	 * different page granularity.
	 */
	BUILD_BUG_ON(XEN_PAGE_SIZE != PAGE_SIZE);

>>>>>>> v4.9.227
        /*
         * add_memory() will build page tables for the new memory so
         * the p2m must contain invalid entries so the correct
         * non-present PTEs will be written.
         *
         * If a failure occurs, the original (identity) p2m entries
         * are not restored since this region is now known not to
         * conflict with any devices.
<<<<<<< HEAD
         */
	if (!xen_feature(XENFEAT_auto_translated_physmap)) {
		unsigned long pfn, i;

		pfn = PFN_DOWN(hotplug_start_paddr);
		for (i = 0; i < balloon_hotplug; i++) {
			if (!set_phys_to_machine(pfn + i, INVALID_P2M_ENTRY)) {
				pr_warn("set_phys_to_machine() failed, no memory added\n");
				return BP_ECANCELED;
=======
         */ 
	if (!xen_feature(XENFEAT_auto_translated_physmap)) {
		unsigned long pfn, i;

		pfn = PFN_DOWN(resource->start);
		for (i = 0; i < balloon_hotplug; i++) {
			if (!set_phys_to_machine(pfn + i, INVALID_P2M_ENTRY)) {
				pr_warn("set_phys_to_machine() failed, no memory added\n");
				goto err;
>>>>>>> v4.9.227
			}
                }
	}
#endif

<<<<<<< HEAD
	rc = add_memory(nid, hotplug_start_paddr, balloon_hotplug << PAGE_SHIFT);

	if (rc) {
		pr_warn("Cannot add additional memory (%i)\n", rc);
		return BP_ECANCELED;
	}

	balloon_hotplug -= credit;

	balloon_stats.hotplug_pages += credit;
	balloon_stats.balloon_hotplug = balloon_hotplug;

	return BP_DONE;
=======
	/*
	 * add_memory_resource() will call online_pages() which in its turn
	 * will call xen_online_page() callback causing deadlock if we don't
	 * release balloon_mutex here. Unlocking here is safe because the
	 * callers drop the mutex before trying again.
	 */
	mutex_unlock(&balloon_mutex);
	/* add_memory_resource() requires the device_hotplug lock */
	lock_device_hotplug();
	rc = add_memory_resource(nid, resource, memhp_auto_online);
	unlock_device_hotplug();
	mutex_lock(&balloon_mutex);

	if (rc) {
		pr_warn("Cannot add additional memory (%i)\n", rc);
		goto err;
	}

	balloon_stats.total_pages += balloon_hotplug;

	return BP_WAIT;
  err:
	release_memory_resource(resource);
	return BP_ECANCELED;
>>>>>>> v4.9.227
}

static void xen_online_page(struct page *page)
{
	__online_page_set_limits(page);

	mutex_lock(&balloon_mutex);

	__balloon_append(page);

<<<<<<< HEAD
	if (balloon_stats.hotplug_pages)
		--balloon_stats.hotplug_pages;
	else
		--balloon_stats.balloon_hotplug;

=======
>>>>>>> v4.9.227
	mutex_unlock(&balloon_mutex);
}

static int xen_memory_notifier(struct notifier_block *nb, unsigned long val, void *v)
{
	if (val == MEM_ONLINE)
		schedule_delayed_work(&balloon_worker, 0);

	return NOTIFY_OK;
}

static struct notifier_block xen_memory_nb = {
	.notifier_call = xen_memory_notifier,
	.priority = 0
};
#else
<<<<<<< HEAD
static long current_credit(void)
{
	unsigned long target = balloon_stats.target_pages;

	target = min(target,
		     balloon_stats.current_pages +
		     balloon_stats.balloon_low +
		     balloon_stats.balloon_high);

	return target - balloon_stats.current_pages;
=======
static enum bp_state reserve_additional_memory(void)
{
	balloon_stats.target_pages = balloon_stats.current_pages +
				     balloon_stats.target_unpopulated;
	return BP_ECANCELED;
}
#endif /* CONFIG_XEN_BALLOON_MEMORY_HOTPLUG */

static long current_credit(void)
{
	return balloon_stats.target_pages - balloon_stats.current_pages;
>>>>>>> v4.9.227
}

static bool balloon_is_inflated(void)
{
<<<<<<< HEAD
	if (balloon_stats.balloon_low || balloon_stats.balloon_high)
		return true;
	else
		return false;
}

static enum bp_state reserve_additional_memory(long credit)
{
	balloon_stats.target_pages = balloon_stats.current_pages;
	return BP_DONE;
}
#endif /* CONFIG_XEN_BALLOON_MEMORY_HOTPLUG */

static enum bp_state increase_reservation(unsigned long nr_pages)
{
	int rc;
	unsigned long  pfn, i;
	struct page   *page;
	struct xen_memory_reservation reservation = {
		.address_bits = 0,
		.extent_order = 0,
		.domid        = DOMID_SELF
	};

#ifdef CONFIG_XEN_BALLOON_MEMORY_HOTPLUG
	if (!balloon_stats.balloon_low && !balloon_stats.balloon_high) {
		nr_pages = min(nr_pages, balloon_stats.balloon_hotplug);
		balloon_stats.hotplug_pages += nr_pages;
		balloon_stats.balloon_hotplug -= nr_pages;
		return BP_DONE;
	}
#endif

=======
	return balloon_stats.balloon_low || balloon_stats.balloon_high;
}

static enum bp_state increase_reservation(unsigned long nr_pages)
{
	int rc;
	unsigned long i;
	struct page   *page;
	struct xen_memory_reservation reservation = {
		.address_bits = 0,
		.extent_order = EXTENT_ORDER,
		.domid        = DOMID_SELF
	};

>>>>>>> v4.9.227
	if (nr_pages > ARRAY_SIZE(frame_list))
		nr_pages = ARRAY_SIZE(frame_list);

	page = list_first_entry_or_null(&ballooned_pages, struct page, lru);
	for (i = 0; i < nr_pages; i++) {
		if (!page) {
			nr_pages = i;
			break;
		}
<<<<<<< HEAD
		frame_list[i] = page_to_pfn(page);
=======

		/* XENMEM_populate_physmap requires a PFN based on Xen
		 * granularity.
		 */
		frame_list[i] = page_to_xen_pfn(page);
>>>>>>> v4.9.227
		page = balloon_next_page(page);
	}

	set_xen_guest_handle(reservation.extent_start, frame_list);
	reservation.nr_extents = nr_pages;
	rc = HYPERVISOR_memory_op(XENMEM_populate_physmap, &reservation);
	if (rc <= 0)
		return BP_EAGAIN;

	for (i = 0; i < rc; i++) {
		page = balloon_retrieve(false);
		BUG_ON(page == NULL);

<<<<<<< HEAD
		pfn = page_to_pfn(page);

#ifdef CONFIG_XEN_HAVE_PVMMU
		if (!xen_feature(XENFEAT_auto_translated_physmap)) {
=======
#ifdef CONFIG_XEN_HAVE_PVMMU
		/*
		 * We don't support PV MMU when Linux and Xen is using
		 * different page granularity.
		 */
		BUILD_BUG_ON(XEN_PAGE_SIZE != PAGE_SIZE);

		if (!xen_feature(XENFEAT_auto_translated_physmap)) {
			unsigned long pfn = page_to_pfn(page);

>>>>>>> v4.9.227
			set_phys_to_machine(pfn, frame_list[i]);

			/* Link back into the page tables if not highmem. */
			if (!PageHighMem(page)) {
				int ret;
				ret = HYPERVISOR_update_va_mapping(
						(unsigned long)__va(pfn << PAGE_SHIFT),
						mfn_pte(frame_list[i], PAGE_KERNEL),
						0);
				BUG_ON(ret);
			}
		}
#endif

		/* Relinquish the page back to the allocator. */
		__free_reserved_page(page);
	}

	balloon_stats.current_pages += rc;

	return BP_DONE;
}

static enum bp_state decrease_reservation(unsigned long nr_pages, gfp_t gfp)
{
	enum bp_state state = BP_DONE;
<<<<<<< HEAD
	unsigned long  pfn, i;
	struct page   *page;
	int ret;
	struct xen_memory_reservation reservation = {
		.address_bits = 0,
		.extent_order = 0,
		.domid        = DOMID_SELF
	};

#ifdef CONFIG_XEN_BALLOON_MEMORY_HOTPLUG
	if (balloon_stats.hotplug_pages) {
		nr_pages = min(nr_pages, balloon_stats.hotplug_pages);
		balloon_stats.hotplug_pages -= nr_pages;
		balloon_stats.balloon_hotplug += nr_pages;
		return BP_DONE;
	}
#endif
=======
	unsigned long i;
	struct page *page, *tmp;
	int ret;
	struct xen_memory_reservation reservation = {
		.address_bits = 0,
		.extent_order = EXTENT_ORDER,
		.domid        = DOMID_SELF
	};
	LIST_HEAD(pages);
>>>>>>> v4.9.227

	if (nr_pages > ARRAY_SIZE(frame_list))
		nr_pages = ARRAY_SIZE(frame_list);

	for (i = 0; i < nr_pages; i++) {
		page = alloc_page(gfp);
		if (page == NULL) {
			nr_pages = i;
			state = BP_EAGAIN;
			break;
		}
		scrub_page(page);
<<<<<<< HEAD

		frame_list[i] = page_to_pfn(page);
=======
		list_add(&page->lru, &pages);
>>>>>>> v4.9.227
	}

	/*
	 * Ensure that ballooned highmem pages don't have kmaps.
	 *
	 * Do this before changing the p2m as kmap_flush_unused()
	 * reads PTEs to obtain pages (and hence needs the original
	 * p2m entry).
	 */
	kmap_flush_unused();

<<<<<<< HEAD
	/* Update direct mapping, invalidate P2M, and add to balloon. */
	for (i = 0; i < nr_pages; i++) {
		pfn = frame_list[i];
		frame_list[i] = pfn_to_mfn(pfn);
		page = pfn_to_page(pfn);

#ifdef CONFIG_XEN_HAVE_PVMMU
		/*
		 * Ballooned out frames are effectively replaced with
		 * a scratch frame.  Ensure direct mappings and the
		 * p2m are consistent.
		 */
		if (!xen_feature(XENFEAT_auto_translated_physmap)) {
			if (!PageHighMem(page)) {
				struct page *scratch_page = get_balloon_scratch_page();

				ret = HYPERVISOR_update_va_mapping(
						(unsigned long)__va(pfn << PAGE_SHIFT),
						pfn_pte(page_to_pfn(scratch_page),
							PAGE_KERNEL_RO), 0);
				BUG_ON(ret);

				put_balloon_scratch_page();
=======
	/*
	 * Setup the frame, update direct mapping, invalidate P2M,
	 * and add to balloon.
	 */
	i = 0;
	list_for_each_entry_safe(page, tmp, &pages, lru) {
		/* XENMEM_decrease_reservation requires a GFN */
		frame_list[i++] = xen_page_to_gfn(page);

#ifdef CONFIG_XEN_HAVE_PVMMU
		/*
		 * We don't support PV MMU when Linux and Xen is using
		 * different page granularity.
		 */
		BUILD_BUG_ON(XEN_PAGE_SIZE != PAGE_SIZE);

		if (!xen_feature(XENFEAT_auto_translated_physmap)) {
			unsigned long pfn = page_to_pfn(page);

			if (!PageHighMem(page)) {
				ret = HYPERVISOR_update_va_mapping(
						(unsigned long)__va(pfn << PAGE_SHIFT),
						__pte_ma(0), 0);
				BUG_ON(ret);
>>>>>>> v4.9.227
			}
			__set_phys_to_machine(pfn, INVALID_P2M_ENTRY);
		}
#endif
<<<<<<< HEAD
=======
		list_del(&page->lru);
>>>>>>> v4.9.227

		balloon_append(page);
	}

	flush_tlb_all();

	set_xen_guest_handle(reservation.extent_start, frame_list);
	reservation.nr_extents   = nr_pages;
	ret = HYPERVISOR_memory_op(XENMEM_decrease_reservation, &reservation);
	BUG_ON(ret != nr_pages);

	balloon_stats.current_pages -= nr_pages;

	return state;
}

/*
<<<<<<< HEAD
 * We avoid multiple worker processes conflicting via the balloon mutex.
=======
 * As this is a work item it is guaranteed to run as a single instance only.
>>>>>>> v4.9.227
 * We may of course race updates of the target counts (which are protected
 * by the balloon lock), or with changes to the Xen hard limit, but we will
 * recover from these in time.
 */
static void balloon_process(struct work_struct *work)
{
	enum bp_state state = BP_DONE;
	long credit;

<<<<<<< HEAD
	mutex_lock(&balloon_mutex);

	do {
=======

	do {
		mutex_lock(&balloon_mutex);

>>>>>>> v4.9.227
		credit = current_credit();

		if (credit > 0) {
			if (balloon_is_inflated())
				state = increase_reservation(credit);
			else
<<<<<<< HEAD
				state = reserve_additional_memory(credit);
		}

		if (credit < 0)
			state = decrease_reservation(-credit, GFP_BALLOON);

		state = update_schedule(state);

#ifndef CONFIG_PREEMPT
		if (need_resched())
			schedule();
#endif
=======
				state = reserve_additional_memory();
		}

		if (credit < 0) {
			long n_pages;

			n_pages = min(-credit, si_mem_available());
			state = decrease_reservation(n_pages, GFP_BALLOON);
			if (state == BP_DONE && n_pages != -credit &&
			    n_pages < totalreserve_pages)
				state = BP_EAGAIN;
		}

		state = update_schedule(state);

		mutex_unlock(&balloon_mutex);

		cond_resched();

>>>>>>> v4.9.227
	} while (credit && state == BP_DONE);

	/* Schedule more work if there is some still to be done. */
	if (state == BP_EAGAIN)
		schedule_delayed_work(&balloon_worker, balloon_stats.schedule_delay * HZ);
<<<<<<< HEAD

	mutex_unlock(&balloon_mutex);
}

struct page *get_balloon_scratch_page(void)
{
	struct page *ret = get_cpu_var(balloon_scratch_page);
	BUG_ON(ret == NULL);
	return ret;
}

void put_balloon_scratch_page(void)
{
	put_cpu_var(balloon_scratch_page);
=======
>>>>>>> v4.9.227
}

/* Resets the Xen limit, sets new target, and kicks off processing. */
void balloon_set_new_target(unsigned long target)
{
	/* No need for lock. Not read-modify-write updates. */
	balloon_stats.target_pages = target;
	schedule_delayed_work(&balloon_worker, 0);
}
EXPORT_SYMBOL_GPL(balloon_set_new_target);

<<<<<<< HEAD
=======
static int add_ballooned_pages(int nr_pages)
{
	enum bp_state st;

	if (xen_hotplug_unpopulated) {
		st = reserve_additional_memory();
		if (st != BP_ECANCELED) {
			mutex_unlock(&balloon_mutex);
			wait_event(balloon_wq,
				   !list_empty(&ballooned_pages));
			mutex_lock(&balloon_mutex);
			return 0;
		}
	}

	if (si_mem_available() < nr_pages)
		return -ENOMEM;

	st = decrease_reservation(nr_pages, GFP_USER);
	if (st != BP_DONE)
		return -ENOMEM;

	return 0;
}

>>>>>>> v4.9.227
/**
 * alloc_xenballooned_pages - get pages that have been ballooned out
 * @nr_pages: Number of pages to get
 * @pages: pages returned
<<<<<<< HEAD
 * @highmem: allow highmem pages
 * @return 0 on success, error otherwise
 */
int alloc_xenballooned_pages(int nr_pages, struct page **pages, bool highmem)
{
	int pgno = 0;
	struct page *page;
	mutex_lock(&balloon_mutex);
	while (pgno < nr_pages) {
		page = balloon_retrieve(highmem);
		if (page && (highmem || !PageHighMem(page))) {
			pages[pgno++] = page;
		} else {
			enum bp_state st;
			if (page)
				balloon_append(page);
			st = decrease_reservation(nr_pages - pgno,
					highmem ? GFP_HIGHUSER : GFP_USER);
			if (st != BP_DONE)
=======
 * @return 0 on success, error otherwise
 */
int alloc_xenballooned_pages(int nr_pages, struct page **pages)
{
	int pgno = 0;
	struct page *page;
	int ret;

	mutex_lock(&balloon_mutex);

	balloon_stats.target_unpopulated += nr_pages;

	while (pgno < nr_pages) {
		page = balloon_retrieve(true);
		if (page) {
			pages[pgno++] = page;
#ifdef CONFIG_XEN_HAVE_PVMMU
			/*
			 * We don't support PV MMU when Linux and Xen is using
			 * different page granularity.
			 */
			BUILD_BUG_ON(XEN_PAGE_SIZE != PAGE_SIZE);

			ret = xen_alloc_p2m_entry(page_to_pfn(page));
			if (ret < 0)
				goto out_undo;
#endif
		} else {
			ret = add_ballooned_pages(nr_pages - pgno);
			if (ret < 0)
>>>>>>> v4.9.227
				goto out_undo;
		}
	}
	mutex_unlock(&balloon_mutex);
	return 0;
 out_undo:
<<<<<<< HEAD
	while (pgno)
		balloon_append(pages[--pgno]);
	/* Free the memory back to the kernel soon */
	schedule_delayed_work(&balloon_worker, 0);
	mutex_unlock(&balloon_mutex);
	return -ENOMEM;
=======
	mutex_unlock(&balloon_mutex);
	free_xenballooned_pages(pgno, pages);
	return ret;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(alloc_xenballooned_pages);

/**
 * free_xenballooned_pages - return pages retrieved with get_ballooned_pages
 * @nr_pages: Number of pages
 * @pages: pages to return
 */
void free_xenballooned_pages(int nr_pages, struct page **pages)
{
	int i;

	mutex_lock(&balloon_mutex);

	for (i = 0; i < nr_pages; i++) {
		if (pages[i])
			balloon_append(pages[i]);
	}

<<<<<<< HEAD
=======
	balloon_stats.target_unpopulated -= nr_pages;

>>>>>>> v4.9.227
	/* The balloon may be too large now. Shrink it if needed. */
	if (current_credit())
		schedule_delayed_work(&balloon_worker, 0);

	mutex_unlock(&balloon_mutex);
}
EXPORT_SYMBOL(free_xenballooned_pages);

static void __init balloon_add_region(unsigned long start_pfn,
				      unsigned long pages)
{
	unsigned long pfn, extra_pfn_end;
	struct page *page;

	/*
	 * If the amount of usable memory has been limited (e.g., with
	 * the 'mem' command line parameter), don't add pages beyond
	 * this limit.
	 */
	extra_pfn_end = min(max_pfn, start_pfn + pages);

	for (pfn = start_pfn; pfn < extra_pfn_end; pfn++) {
		page = pfn_to_page(pfn);
		/* totalram_pages and totalhigh_pages do not
		   include the boot-time balloon extension, so
		   don't subtract from it. */
		__balloon_append(page);
	}
<<<<<<< HEAD
}

static int alloc_balloon_scratch_page(int cpu)
{
	if (per_cpu(balloon_scratch_page, cpu) != NULL)
		return 0;

	per_cpu(balloon_scratch_page, cpu) = alloc_page(GFP_KERNEL);
	if (per_cpu(balloon_scratch_page, cpu) == NULL) {
		pr_warn("Failed to allocate balloon_scratch_page for cpu %d\n", cpu);
		return -ENOMEM;
	}

	return 0;
}


static int balloon_cpu_notify(struct notifier_block *self,
				    unsigned long action, void *hcpu)
{
	int cpu = (long)hcpu;
	switch (action) {
	case CPU_UP_PREPARE:
		if (alloc_balloon_scratch_page(cpu))
			return NOTIFY_BAD;
		break;
	default:
		break;
	}
	return NOTIFY_OK;
}

static struct notifier_block balloon_cpu_notifier = {
	.notifier_call	= balloon_cpu_notify,
};

static int __init balloon_init(void)
{
	int i, cpu;
=======

	balloon_stats.total_pages += extra_pfn_end - start_pfn;
}

static int __init balloon_init(void)
{
	int i;
>>>>>>> v4.9.227

	if (!xen_domain())
		return -ENODEV;

<<<<<<< HEAD
	if (!xen_feature(XENFEAT_auto_translated_physmap)) {
		register_cpu_notifier(&balloon_cpu_notifier);

		get_online_cpus();
		for_each_online_cpu(cpu) {
			if (alloc_balloon_scratch_page(cpu)) {
				put_online_cpus();
				unregister_cpu_notifier(&balloon_cpu_notifier);
				return -ENOMEM;
			}
		}
		put_online_cpus();
	}

=======
>>>>>>> v4.9.227
	pr_info("Initialising balloon driver\n");

	balloon_stats.current_pages = xen_pv_domain()
		? min(xen_start_info->nr_pages - xen_released_pages, max_pfn)
		: get_num_physpages();
	balloon_stats.target_pages  = balloon_stats.current_pages;
	balloon_stats.balloon_low   = 0;
	balloon_stats.balloon_high  = 0;
<<<<<<< HEAD
=======
	balloon_stats.total_pages   = balloon_stats.current_pages;
>>>>>>> v4.9.227

	balloon_stats.schedule_delay = 1;
	balloon_stats.max_schedule_delay = 32;
	balloon_stats.retry_count = 1;
<<<<<<< HEAD
	balloon_stats.max_retry_count = RETRY_UNLIMITED;

#ifdef CONFIG_XEN_BALLOON_MEMORY_HOTPLUG
	balloon_stats.hotplug_pages = 0;
	balloon_stats.balloon_hotplug = 0;

	set_online_page_callback(&xen_online_page);
	register_memory_notifier(&xen_memory_nb);
=======
	balloon_stats.max_retry_count = 4;

#ifdef CONFIG_XEN_BALLOON_MEMORY_HOTPLUG
	set_online_page_callback(&xen_online_page);
	register_memory_notifier(&xen_memory_nb);
	register_sysctl_table(xen_root);
>>>>>>> v4.9.227
#endif

	/*
	 * Initialize the balloon with pages from the extra memory
	 * regions (see arch/x86/xen/setup.c).
	 */
	for (i = 0; i < XEN_EXTRA_MEM_MAX_REGIONS; i++)
<<<<<<< HEAD
		if (xen_extra_mem[i].size)
			balloon_add_region(PFN_UP(xen_extra_mem[i].start),
					   PFN_DOWN(xen_extra_mem[i].size));

	return 0;
}

subsys_initcall(balloon_init);

static int __init balloon_clear(void)
{
	int cpu;

	for_each_possible_cpu(cpu)
		per_cpu(balloon_scratch_page, cpu) = NULL;

	return 0;
}
early_initcall(balloon_clear);

MODULE_LICENSE("GPL");
=======
		if (xen_extra_mem[i].n_pfns)
			balloon_add_region(xen_extra_mem[i].start_pfn,
					   xen_extra_mem[i].n_pfns);

	return 0;
}
subsys_initcall(balloon_init);
>>>>>>> v4.9.227
