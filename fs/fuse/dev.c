/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2008  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

#include "fuse_i.h"
<<<<<<< HEAD
#include "fuse_shortcircuit.h"
=======
>>>>>>> v4.9.227

#include <linux/init.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/uio.h>
#include <linux/miscdevice.h>
<<<<<<< HEAD
#include <linux/namei.h>
=======
>>>>>>> v4.9.227
#include <linux/pagemap.h>
#include <linux/file.h>
#include <linux/slab.h>
#include <linux/pipe_fs_i.h>
#include <linux/swap.h>
#include <linux/splice.h>
<<<<<<< HEAD
#include <linux/aio.h>
#include <linux/freezer.h>
=======
>>>>>>> v4.9.227

MODULE_ALIAS_MISCDEV(FUSE_MINOR);
MODULE_ALIAS("devname:fuse");

static struct kmem_cache *fuse_req_cachep;

<<<<<<< HEAD
static struct fuse_conn *fuse_get_conn(struct file *file)
=======
static struct fuse_dev *fuse_get_dev(struct file *file)
>>>>>>> v4.9.227
{
	/*
	 * Lockless access is OK, because file->private data is set
	 * once during mount and is valid until the file is released.
	 */
<<<<<<< HEAD
	return file->private_data;
=======
	return ACCESS_ONCE(file->private_data);
>>>>>>> v4.9.227
}

static void fuse_request_init(struct fuse_req *req, struct page **pages,
			      struct fuse_page_desc *page_descs,
			      unsigned npages)
{
	memset(req, 0, sizeof(*req));
	memset(pages, 0, sizeof(*pages) * npages);
	memset(page_descs, 0, sizeof(*page_descs) * npages);
	INIT_LIST_HEAD(&req->list);
	INIT_LIST_HEAD(&req->intr_entry);
	init_waitqueue_head(&req->waitq);
	atomic_set(&req->count, 1);
	req->pages = pages;
	req->page_descs = page_descs;
	req->max_pages = npages;
<<<<<<< HEAD
=======
	__set_bit(FR_PENDING, &req->flags);
>>>>>>> v4.9.227
}

static struct fuse_req *__fuse_request_alloc(unsigned npages, gfp_t flags)
{
	struct fuse_req *req = kmem_cache_alloc(fuse_req_cachep, flags);
	if (req) {
		struct page **pages;
		struct fuse_page_desc *page_descs;

		if (npages <= FUSE_REQ_INLINE_PAGES) {
			pages = req->inline_pages;
			page_descs = req->inline_page_descs;
		} else {
			pages = kmalloc(sizeof(struct page *) * npages, flags);
			page_descs = kmalloc(sizeof(struct fuse_page_desc) *
					     npages, flags);
		}

		if (!pages || !page_descs) {
			kfree(pages);
			kfree(page_descs);
			kmem_cache_free(fuse_req_cachep, req);
			return NULL;
		}

		fuse_request_init(req, pages, page_descs, npages);
	}
	return req;
}

struct fuse_req *fuse_request_alloc(unsigned npages)
{
	return __fuse_request_alloc(npages, GFP_KERNEL);
}
EXPORT_SYMBOL_GPL(fuse_request_alloc);

struct fuse_req *fuse_request_alloc_nofs(unsigned npages)
{
	return __fuse_request_alloc(npages, GFP_NOFS);
}

void fuse_request_free(struct fuse_req *req)
{
	if (req->pages != req->inline_pages) {
		kfree(req->pages);
		kfree(req->page_descs);
	}
	kmem_cache_free(fuse_req_cachep, req);
}

<<<<<<< HEAD
static void block_sigs(sigset_t *oldset)
{
	sigset_t mask;

	siginitsetinv(&mask, sigmask(SIGKILL));
	sigprocmask(SIG_BLOCK, &mask, oldset);
}

static void restore_sigs(sigset_t *oldset)
{
	sigprocmask(SIG_SETMASK, oldset, NULL);
}

=======
>>>>>>> v4.9.227
void __fuse_get_request(struct fuse_req *req)
{
	atomic_inc(&req->count);
}

/* Must be called with > 1 refcount */
static void __fuse_put_request(struct fuse_req *req)
{
	BUG_ON(atomic_read(&req->count) < 2);
	atomic_dec(&req->count);
}

static void fuse_req_init_context(struct fuse_req *req)
{
	req->in.h.uid = from_kuid_munged(&init_user_ns, current_fsuid());
	req->in.h.gid = from_kgid_munged(&init_user_ns, current_fsgid());
	req->in.h.pid = current->pid;
}

<<<<<<< HEAD
=======
void fuse_set_initialized(struct fuse_conn *fc)
{
	/* Make sure stores before this are seen on another CPU */
	smp_wmb();
	fc->initialized = 1;
}

>>>>>>> v4.9.227
static bool fuse_block_alloc(struct fuse_conn *fc, bool for_background)
{
	return !fc->initialized || (for_background && fc->blocked);
}

<<<<<<< HEAD
=======
static void fuse_drop_waiting(struct fuse_conn *fc)
{
	/*
	 * lockess check of fc->connected is okay, because atomic_dec_and_test()
	 * provides a memory barrier mached with the one in fuse_wait_aborted()
	 * to ensure no wake-up is missed.
	 */
	if (atomic_dec_and_test(&fc->num_waiting) &&
	    !READ_ONCE(fc->connected)) {
		/* wake up aborters */
		wake_up_all(&fc->blocked_waitq);
	}
}

>>>>>>> v4.9.227
static struct fuse_req *__fuse_get_req(struct fuse_conn *fc, unsigned npages,
				       bool for_background)
{
	struct fuse_req *req;
	int err;
	atomic_inc(&fc->num_waiting);

	if (fuse_block_alloc(fc, for_background)) {
<<<<<<< HEAD
		sigset_t oldset;
		int intr;

		block_sigs(&oldset);
		intr = wait_event_interruptible_exclusive(fc->blocked_waitq,
				!fuse_block_alloc(fc, for_background));
		restore_sigs(&oldset);
		err = -EINTR;
		if (intr)
			goto out;
	}
=======
		err = -EINTR;
		if (wait_event_killable_exclusive(fc->blocked_waitq,
				!fuse_block_alloc(fc, for_background)))
			goto out;
	}
	/* Matches smp_wmb() in fuse_set_initialized() */
	smp_rmb();
>>>>>>> v4.9.227

	err = -ENOTCONN;
	if (!fc->connected)
		goto out;

<<<<<<< HEAD
=======
	err = -ECONNREFUSED;
	if (fc->conn_error)
		goto out;

>>>>>>> v4.9.227
	req = fuse_request_alloc(npages);
	err = -ENOMEM;
	if (!req) {
		if (for_background)
			wake_up(&fc->blocked_waitq);
		goto out;
	}

	fuse_req_init_context(req);
<<<<<<< HEAD
	req->waiting = 1;
	req->background = for_background;
	return req;

 out:
	atomic_dec(&fc->num_waiting);
=======
	__set_bit(FR_WAITING, &req->flags);
	if (for_background)
		__set_bit(FR_BACKGROUND, &req->flags);

	return req;

 out:
	fuse_drop_waiting(fc);
>>>>>>> v4.9.227
	return ERR_PTR(err);
}

struct fuse_req *fuse_get_req(struct fuse_conn *fc, unsigned npages)
{
	return __fuse_get_req(fc, npages, false);
}
EXPORT_SYMBOL_GPL(fuse_get_req);

struct fuse_req *fuse_get_req_for_background(struct fuse_conn *fc,
					     unsigned npages)
{
	return __fuse_get_req(fc, npages, true);
}
EXPORT_SYMBOL_GPL(fuse_get_req_for_background);

/*
 * Return request in fuse_file->reserved_req.  However that may
 * currently be in use.  If that is the case, wait for it to become
 * available.
 */
static struct fuse_req *get_reserved_req(struct fuse_conn *fc,
					 struct file *file)
{
	struct fuse_req *req = NULL;
	struct fuse_file *ff = file->private_data;

	do {
		wait_event(fc->reserved_req_waitq, ff->reserved_req);
		spin_lock(&fc->lock);
		if (ff->reserved_req) {
			req = ff->reserved_req;
			ff->reserved_req = NULL;
			req->stolen_file = get_file(file);
		}
		spin_unlock(&fc->lock);
	} while (!req);

	return req;
}

/*
 * Put stolen request back into fuse_file->reserved_req
 */
static void put_reserved_req(struct fuse_conn *fc, struct fuse_req *req)
{
	struct file *file = req->stolen_file;
	struct fuse_file *ff = file->private_data;

	spin_lock(&fc->lock);
	fuse_request_init(req, req->pages, req->page_descs, req->max_pages);
	BUG_ON(ff->reserved_req);
	ff->reserved_req = req;
	wake_up_all(&fc->reserved_req_waitq);
	spin_unlock(&fc->lock);
	fput(file);
}

/*
 * Gets a requests for a file operation, always succeeds
 *
 * This is used for sending the FLUSH request, which must get to
 * userspace, due to POSIX locks which may need to be unlocked.
 *
 * If allocation fails due to OOM, use the reserved request in
 * fuse_file.
 *
 * This is very unlikely to deadlock accidentally, since the
 * filesystem should not have it's own file open.  If deadlock is
 * intentional, it can still be broken by "aborting" the filesystem.
 */
struct fuse_req *fuse_get_req_nofail_nopages(struct fuse_conn *fc,
					     struct file *file)
{
	struct fuse_req *req;

	atomic_inc(&fc->num_waiting);
	wait_event(fc->blocked_waitq, fc->initialized);
<<<<<<< HEAD
=======
	/* Matches smp_wmb() in fuse_set_initialized() */
	smp_rmb();
>>>>>>> v4.9.227
	req = fuse_request_alloc(0);
	if (!req)
		req = get_reserved_req(fc, file);

	fuse_req_init_context(req);
<<<<<<< HEAD
	req->waiting = 1;
	req->background = 0;
=======
	__set_bit(FR_WAITING, &req->flags);
	__clear_bit(FR_BACKGROUND, &req->flags);
>>>>>>> v4.9.227
	return req;
}

void fuse_put_request(struct fuse_conn *fc, struct fuse_req *req)
{
	if (atomic_dec_and_test(&req->count)) {
<<<<<<< HEAD
		if (unlikely(req->background)) {
=======
		if (test_bit(FR_BACKGROUND, &req->flags)) {
>>>>>>> v4.9.227
			/*
			 * We get here in the unlikely case that a background
			 * request was allocated but not sent
			 */
			spin_lock(&fc->lock);
			if (!fc->blocked)
				wake_up(&fc->blocked_waitq);
			spin_unlock(&fc->lock);
		}

<<<<<<< HEAD
		if (req->waiting)
			atomic_dec(&fc->num_waiting);
=======
		if (test_bit(FR_WAITING, &req->flags)) {
			__clear_bit(FR_WAITING, &req->flags);
			fuse_drop_waiting(fc);
		}
>>>>>>> v4.9.227

		if (req->stolen_file)
			put_reserved_req(fc, req);
		else
			fuse_request_free(req);
	}
}
EXPORT_SYMBOL_GPL(fuse_put_request);

static unsigned len_args(unsigned numargs, struct fuse_arg *args)
{
	unsigned nbytes = 0;
	unsigned i;

	for (i = 0; i < numargs; i++)
		nbytes += args[i].size;

	return nbytes;
}

<<<<<<< HEAD
static u64 fuse_get_unique(struct fuse_conn *fc)
{
	fc->reqctr++;
	/* zero is special */
	if (fc->reqctr == 0)
		fc->reqctr = 1;

	return fc->reqctr;
}

static void queue_request(struct fuse_conn *fc, struct fuse_req *req)
{
	req->in.h.len = sizeof(struct fuse_in_header) +
		len_args(req->in.numargs, (struct fuse_arg *) req->in.args);
	list_add_tail(&req->list, &fc->pending);
	req->state = FUSE_REQ_PENDING;
	if (!req->waiting) {
		req->waiting = 1;
		atomic_inc(&fc->num_waiting);
	}
	wake_up(&fc->waitq);
	kill_fasync(&fc->fasync, SIGIO, POLL_IN);
=======
static u64 fuse_get_unique(struct fuse_iqueue *fiq)
{
	return ++fiq->reqctr;
}

static void queue_request(struct fuse_iqueue *fiq, struct fuse_req *req)
{
	req->in.h.len = sizeof(struct fuse_in_header) +
		len_args(req->in.numargs, (struct fuse_arg *) req->in.args);
	list_add_tail(&req->list, &fiq->pending);
	wake_up_locked(&fiq->waitq);
	kill_fasync(&fiq->fasync, SIGIO, POLL_IN);
>>>>>>> v4.9.227
}

void fuse_queue_forget(struct fuse_conn *fc, struct fuse_forget_link *forget,
		       u64 nodeid, u64 nlookup)
{
<<<<<<< HEAD
	forget->forget_one.nodeid = nodeid;
	forget->forget_one.nlookup = nlookup;

	spin_lock(&fc->lock);
	if (fc->connected) {
		fc->forget_list_tail->next = forget;
		fc->forget_list_tail = forget;
		wake_up(&fc->waitq);
		kill_fasync(&fc->fasync, SIGIO, POLL_IN);
	} else {
		kfree(forget);
	}
	spin_unlock(&fc->lock);
=======
	struct fuse_iqueue *fiq = &fc->iq;

	forget->forget_one.nodeid = nodeid;
	forget->forget_one.nlookup = nlookup;

	spin_lock(&fiq->waitq.lock);
	if (fiq->connected) {
		fiq->forget_list_tail->next = forget;
		fiq->forget_list_tail = forget;
		wake_up_locked(&fiq->waitq);
		kill_fasync(&fiq->fasync, SIGIO, POLL_IN);
	} else {
		kfree(forget);
	}
	spin_unlock(&fiq->waitq.lock);
>>>>>>> v4.9.227
}

static void flush_bg_queue(struct fuse_conn *fc)
{
	while (fc->active_background < fc->max_background &&
	       !list_empty(&fc->bg_queue)) {
		struct fuse_req *req;
<<<<<<< HEAD
=======
		struct fuse_iqueue *fiq = &fc->iq;
>>>>>>> v4.9.227

		req = list_entry(fc->bg_queue.next, struct fuse_req, list);
		list_del(&req->list);
		fc->active_background++;
<<<<<<< HEAD
		req->in.h.unique = fuse_get_unique(fc);
		queue_request(fc, req);
=======
		spin_lock(&fiq->waitq.lock);
		req->in.h.unique = fuse_get_unique(fiq);
		queue_request(fiq, req);
		spin_unlock(&fiq->waitq.lock);
>>>>>>> v4.9.227
	}
}

/*
 * This function is called when a request is finished.  Either a reply
 * has arrived or it was aborted (and not yet sent) or some error
 * occurred during communication with userspace, or the device file
 * was closed.  The requester thread is woken up (if still waiting),
 * the 'end' callback is called if given, else the reference to the
 * request is released
<<<<<<< HEAD
 *
 * Called with fc->lock, unlocks it
 */
static void request_end(struct fuse_conn *fc, struct fuse_req *req)
__releases(fc->lock)
{
	void (*end) (struct fuse_conn *, struct fuse_req *) = req->end;
	req->end = NULL;
	list_del(&req->list);
	list_del(&req->intr_entry);
	req->state = FUSE_REQ_FINISHED;
	if (req->background) {
		req->background = 0;

		if (fc->num_background == fc->max_background)
			fc->blocked = 0;

		/* Wake up next waiter, if any */
		if (!fc->blocked && waitqueue_active(&fc->blocked_waitq))
			wake_up(&fc->blocked_waitq);
=======
 */
static void request_end(struct fuse_conn *fc, struct fuse_req *req)
{
	struct fuse_iqueue *fiq = &fc->iq;

	if (test_and_set_bit(FR_FINISHED, &req->flags))
		goto put_request;

	spin_lock(&fiq->waitq.lock);
	list_del_init(&req->intr_entry);
	spin_unlock(&fiq->waitq.lock);
	WARN_ON(test_bit(FR_PENDING, &req->flags));
	WARN_ON(test_bit(FR_SENT, &req->flags));
	if (test_bit(FR_BACKGROUND, &req->flags)) {
		spin_lock(&fc->lock);
		clear_bit(FR_BACKGROUND, &req->flags);
		if (fc->num_background == fc->max_background) {
			fc->blocked = 0;
			wake_up(&fc->blocked_waitq);
		} else if (!fc->blocked) {
			/*
			 * Wake up next waiter, if any.  It's okay to use
			 * waitqueue_active(), as we've already synced up
			 * fc->blocked with waiters with the wake_up() call
			 * above.
			 */
			if (waitqueue_active(&fc->blocked_waitq))
				wake_up(&fc->blocked_waitq);
		}
>>>>>>> v4.9.227

		if (fc->num_background == fc->congestion_threshold &&
		    fc->connected && fc->bdi_initialized) {
			clear_bdi_congested(&fc->bdi, BLK_RW_SYNC);
			clear_bdi_congested(&fc->bdi, BLK_RW_ASYNC);
		}
		fc->num_background--;
		fc->active_background--;
		flush_bg_queue(fc);
<<<<<<< HEAD
	}
	spin_unlock(&fc->lock);
	wake_up(&req->waitq);
	if (end)
		end(fc, req);
	fuse_put_request(fc, req);
}

static void wait_answer_interruptible(struct fuse_conn *fc,
				      struct fuse_req *req)
__releases(fc->lock)
__acquires(fc->lock)
{
	if (signal_pending(current))
		return;

	spin_unlock(&fc->lock);
	wait_event_interruptible(req->waitq, req->state == FUSE_REQ_FINISHED);
	spin_lock(&fc->lock);
}

static void queue_interrupt(struct fuse_conn *fc, struct fuse_req *req)
{
	list_add_tail(&req->intr_entry, &fc->interrupts);
	wake_up(&fc->waitq);
	kill_fasync(&fc->fasync, SIGIO, POLL_IN);
}

static void request_wait_answer(struct fuse_conn *fc, struct fuse_req *req)
__releases(fc->lock)
__acquires(fc->lock)
{
	if (!fc->no_interrupt) {
		/* Any signal may interrupt this */
		wait_answer_interruptible(fc, req);

		if (req->aborted)
			goto aborted;
		if (req->state == FUSE_REQ_FINISHED)
			return;

		req->interrupted = 1;
		if (req->state == FUSE_REQ_SENT)
			queue_interrupt(fc, req);
	}

	if (!req->force) {
		sigset_t oldset;

		/* Only fatal signals may interrupt this */
		block_sigs(&oldset);
		wait_answer_interruptible(fc, req);
		restore_sigs(&oldset);

		if (req->aborted)
			goto aborted;
		if (req->state == FUSE_REQ_FINISHED)
			return;

		/* Request is not yet in userspace, bail out */
		if (req->state == FUSE_REQ_PENDING) {
			list_del(&req->list);
=======
		spin_unlock(&fc->lock);
	}
	wake_up(&req->waitq);
	if (req->end)
		req->end(fc, req);
put_request:
	fuse_put_request(fc, req);
}

static void queue_interrupt(struct fuse_iqueue *fiq, struct fuse_req *req)
{
	spin_lock(&fiq->waitq.lock);
	if (test_bit(FR_FINISHED, &req->flags)) {
		spin_unlock(&fiq->waitq.lock);
		return;
	}
	if (list_empty(&req->intr_entry)) {
		list_add_tail(&req->intr_entry, &fiq->interrupts);
		wake_up_locked(&fiq->waitq);
	}
	spin_unlock(&fiq->waitq.lock);
	kill_fasync(&fiq->fasync, SIGIO, POLL_IN);
}

static void request_wait_answer(struct fuse_conn *fc, struct fuse_req *req)
{
	struct fuse_iqueue *fiq = &fc->iq;
	int err;

	if (!fc->no_interrupt) {
		/* Any signal may interrupt this */
		err = wait_event_interruptible(req->waitq,
					test_bit(FR_FINISHED, &req->flags));
		if (!err)
			return;

		set_bit(FR_INTERRUPTED, &req->flags);
		/* matches barrier in fuse_dev_do_read() */
		smp_mb__after_atomic();
		if (test_bit(FR_SENT, &req->flags))
			queue_interrupt(fiq, req);
	}

	if (!test_bit(FR_FORCE, &req->flags)) {
		/* Only fatal signals may interrupt this */
		err = wait_event_killable(req->waitq,
					test_bit(FR_FINISHED, &req->flags));
		if (!err)
			return;

		spin_lock(&fiq->waitq.lock);
		/* Request is not yet in userspace, bail out */
		if (test_bit(FR_PENDING, &req->flags)) {
			list_del(&req->list);
			spin_unlock(&fiq->waitq.lock);
>>>>>>> v4.9.227
			__fuse_put_request(req);
			req->out.h.error = -EINTR;
			return;
		}
<<<<<<< HEAD
=======
		spin_unlock(&fiq->waitq.lock);
>>>>>>> v4.9.227
	}

	/*
	 * Either request is already in userspace, or it was forced.
	 * Wait it out.
	 */
<<<<<<< HEAD
	spin_unlock(&fc->lock);

	while (req->state != FUSE_REQ_FINISHED)
		wait_event_freezable(req->waitq,
				     req->state == FUSE_REQ_FINISHED);
	spin_lock(&fc->lock);

	if (!req->aborted)
		return;

 aborted:
	BUG_ON(req->state != FUSE_REQ_FINISHED);
	if (req->locked) {
		/* This is uninterruptible sleep, because data is
		   being copied to/from the buffers of req.  During
		   locked state, there mustn't be any filesystem
		   operation (e.g. page fault), since that could lead
		   to deadlock */
		spin_unlock(&fc->lock);
		wait_event(req->waitq, !req->locked);
		spin_lock(&fc->lock);
	}
=======
	wait_event(req->waitq, test_bit(FR_FINISHED, &req->flags));
>>>>>>> v4.9.227
}

static void __fuse_request_send(struct fuse_conn *fc, struct fuse_req *req)
{
<<<<<<< HEAD
	BUG_ON(req->background);
	spin_lock(&fc->lock);
	if (!fc->connected)
		req->out.h.error = -ENOTCONN;
	else if (fc->conn_error)
		req->out.h.error = -ECONNREFUSED;
	else {
		req->in.h.unique = fuse_get_unique(fc);
		queue_request(fc, req);
		/* acquire extra reference, since request is still needed
		   after request_end() */
		__fuse_get_request(req);

		request_wait_answer(fc, req);
	}
	spin_unlock(&fc->lock);
=======
	struct fuse_iqueue *fiq = &fc->iq;

	BUG_ON(test_bit(FR_BACKGROUND, &req->flags));
	spin_lock(&fiq->waitq.lock);
	if (!fiq->connected) {
		spin_unlock(&fiq->waitq.lock);
		req->out.h.error = -ENOTCONN;
	} else {
		req->in.h.unique = fuse_get_unique(fiq);
		queue_request(fiq, req);
		/* acquire extra reference, since request is still needed
		   after request_end() */
		__fuse_get_request(req);
		spin_unlock(&fiq->waitq.lock);

		request_wait_answer(fc, req);
		/* Pairs with smp_wmb() in request_end() */
		smp_rmb();
	}
>>>>>>> v4.9.227
}

void fuse_request_send(struct fuse_conn *fc, struct fuse_req *req)
{
<<<<<<< HEAD
	req->isreply = 1;
=======
	__set_bit(FR_ISREPLY, &req->flags);
	if (!test_bit(FR_WAITING, &req->flags)) {
		__set_bit(FR_WAITING, &req->flags);
		atomic_inc(&fc->num_waiting);
	}
>>>>>>> v4.9.227
	__fuse_request_send(fc, req);
}
EXPORT_SYMBOL_GPL(fuse_request_send);

<<<<<<< HEAD
static void fuse_request_send_nowait_locked(struct fuse_conn *fc,
					    struct fuse_req *req)
{
	BUG_ON(!req->background);
=======
static void fuse_adjust_compat(struct fuse_conn *fc, struct fuse_args *args)
{
	if (fc->minor < 4 && args->in.h.opcode == FUSE_STATFS)
		args->out.args[0].size = FUSE_COMPAT_STATFS_SIZE;

	if (fc->minor < 9) {
		switch (args->in.h.opcode) {
		case FUSE_LOOKUP:
		case FUSE_CREATE:
		case FUSE_MKNOD:
		case FUSE_MKDIR:
		case FUSE_SYMLINK:
		case FUSE_LINK:
			args->out.args[0].size = FUSE_COMPAT_ENTRY_OUT_SIZE;
			break;
		case FUSE_GETATTR:
		case FUSE_SETATTR:
			args->out.args[0].size = FUSE_COMPAT_ATTR_OUT_SIZE;
			break;
		}
	}
	if (fc->minor < 12) {
		switch (args->in.h.opcode) {
		case FUSE_CREATE:
			args->in.args[0].size = sizeof(struct fuse_open_in);
			break;
		case FUSE_MKNOD:
			args->in.args[0].size = FUSE_COMPAT_MKNOD_IN_SIZE;
			break;
		}
	}
}

ssize_t fuse_simple_request(struct fuse_conn *fc, struct fuse_args *args)
{
	struct fuse_req *req;
	ssize_t ret;

	req = fuse_get_req(fc, 0);
	if (IS_ERR(req))
		return PTR_ERR(req);

	/* Needs to be done after fuse_get_req() so that fc->minor is valid */
	fuse_adjust_compat(fc, args);

	req->in.h.opcode = args->in.h.opcode;
	req->in.h.nodeid = args->in.h.nodeid;
	req->in.numargs = args->in.numargs;
	memcpy(req->in.args, args->in.args,
	       args->in.numargs * sizeof(struct fuse_in_arg));
	req->out.argvar = args->out.argvar;
	req->out.numargs = args->out.numargs;
	memcpy(req->out.args, args->out.args,
	       args->out.numargs * sizeof(struct fuse_arg));
	fuse_request_send(fc, req);
	ret = req->out.h.error;
	if (!ret && args->out.argvar) {
		BUG_ON(args->out.numargs != 1);
		ret = req->out.args[0].size;
	}
	fuse_put_request(fc, req);

	return ret;
}

/*
 * Called under fc->lock
 *
 * fc->connected must have been checked previously
 */
void fuse_request_send_background_locked(struct fuse_conn *fc,
					 struct fuse_req *req)
{
	BUG_ON(!test_bit(FR_BACKGROUND, &req->flags));
	if (!test_bit(FR_WAITING, &req->flags)) {
		__set_bit(FR_WAITING, &req->flags);
		atomic_inc(&fc->num_waiting);
	}
	__set_bit(FR_ISREPLY, &req->flags);
>>>>>>> v4.9.227
	fc->num_background++;
	if (fc->num_background == fc->max_background)
		fc->blocked = 1;
	if (fc->num_background == fc->congestion_threshold &&
	    fc->bdi_initialized) {
		set_bdi_congested(&fc->bdi, BLK_RW_SYNC);
		set_bdi_congested(&fc->bdi, BLK_RW_ASYNC);
	}
	list_add_tail(&req->list, &fc->bg_queue);
	flush_bg_queue(fc);
}

<<<<<<< HEAD
static void fuse_request_send_nowait(struct fuse_conn *fc, struct fuse_req *req)
{
	spin_lock(&fc->lock);
	if (fc->connected) {
		fuse_request_send_nowait_locked(fc, req);
		spin_unlock(&fc->lock);
	} else {
		req->out.h.error = -ENOTCONN;
		request_end(fc, req);
	}
}

void fuse_request_send_background(struct fuse_conn *fc, struct fuse_req *req)
{
	req->isreply = 1;
	fuse_request_send_nowait(fc, req);
=======
void fuse_request_send_background(struct fuse_conn *fc, struct fuse_req *req)
{
	BUG_ON(!req->end);
	spin_lock(&fc->lock);
	if (fc->connected) {
		fuse_request_send_background_locked(fc, req);
		spin_unlock(&fc->lock);
	} else {
		spin_unlock(&fc->lock);
		req->out.h.error = -ENOTCONN;
		req->end(fc, req);
		fuse_put_request(fc, req);
	}
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(fuse_request_send_background);

static int fuse_request_send_notify_reply(struct fuse_conn *fc,
					  struct fuse_req *req, u64 unique)
{
	int err = -ENODEV;
<<<<<<< HEAD

	req->isreply = 0;
	req->in.h.unique = unique;
	spin_lock(&fc->lock);
	if (fc->connected) {
		queue_request(fc, req);
		err = 0;
	}
	spin_unlock(&fc->lock);
=======
	struct fuse_iqueue *fiq = &fc->iq;

	__clear_bit(FR_ISREPLY, &req->flags);
	req->in.h.unique = unique;
	spin_lock(&fiq->waitq.lock);
	if (fiq->connected) {
		queue_request(fiq, req);
		err = 0;
	}
	spin_unlock(&fiq->waitq.lock);
>>>>>>> v4.9.227

	return err;
}

<<<<<<< HEAD
/*
 * Called under fc->lock
 *
 * fc->connected must have been checked previously
 */
void fuse_request_send_background_locked(struct fuse_conn *fc,
					 struct fuse_req *req)
{
	req->isreply = 1;
	fuse_request_send_nowait_locked(fc, req);
}

=======
>>>>>>> v4.9.227
void fuse_force_forget(struct file *file, u64 nodeid)
{
	struct inode *inode = file_inode(file);
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_req *req;
	struct fuse_forget_in inarg;

	memset(&inarg, 0, sizeof(inarg));
	inarg.nlookup = 1;
	req = fuse_get_req_nofail_nopages(fc, file);
	req->in.h.opcode = FUSE_FORGET;
	req->in.h.nodeid = nodeid;
	req->in.numargs = 1;
	req->in.args[0].size = sizeof(inarg);
	req->in.args[0].value = &inarg;
<<<<<<< HEAD
	req->isreply = 0;
=======
	__clear_bit(FR_ISREPLY, &req->flags);
>>>>>>> v4.9.227
	__fuse_request_send(fc, req);
	/* ignore errors */
	fuse_put_request(fc, req);
}

/*
 * Lock the request.  Up to the next unlock_request() there mustn't be
 * anything that could cause a page-fault.  If the request was already
 * aborted bail out.
 */
<<<<<<< HEAD
static int lock_request(struct fuse_conn *fc, struct fuse_req *req)
{
	int err = 0;
	if (req) {
		spin_lock(&fc->lock);
		if (req->aborted)
			err = -ENOENT;
		else
			req->locked = 1;
		spin_unlock(&fc->lock);
=======
static int lock_request(struct fuse_req *req)
{
	int err = 0;
	if (req) {
		spin_lock(&req->waitq.lock);
		if (test_bit(FR_ABORTED, &req->flags))
			err = -ENOENT;
		else
			set_bit(FR_LOCKED, &req->flags);
		spin_unlock(&req->waitq.lock);
>>>>>>> v4.9.227
	}
	return err;
}

/*
<<<<<<< HEAD
 * Unlock request.  If it was aborted during being locked, the
 * requester thread is currently waiting for it to be unlocked, so
 * wake it up.
 */
static void unlock_request(struct fuse_conn *fc, struct fuse_req *req)
{
	if (req) {
		spin_lock(&fc->lock);
		req->locked = 0;
		if (req->aborted)
			wake_up(&req->waitq);
		spin_unlock(&fc->lock);
	}
}

struct fuse_copy_state {
	struct fuse_conn *fc;
	int write;
	struct fuse_req *req;
	const struct iovec *iov;
=======
 * Unlock request.  If it was aborted while locked, caller is responsible
 * for unlocking and ending the request.
 */
static int unlock_request(struct fuse_req *req)
{
	int err = 0;
	if (req) {
		spin_lock(&req->waitq.lock);
		if (test_bit(FR_ABORTED, &req->flags))
			err = -ENOENT;
		else
			clear_bit(FR_LOCKED, &req->flags);
		spin_unlock(&req->waitq.lock);
	}
	return err;
}

struct fuse_copy_state {
	int write;
	struct fuse_req *req;
	struct iov_iter *iter;
>>>>>>> v4.9.227
	struct pipe_buffer *pipebufs;
	struct pipe_buffer *currbuf;
	struct pipe_inode_info *pipe;
	unsigned long nr_segs;
<<<<<<< HEAD
	unsigned long seglen;
	unsigned long addr;
=======
>>>>>>> v4.9.227
	struct page *pg;
	unsigned len;
	unsigned offset;
	unsigned move_pages:1;
};

<<<<<<< HEAD
static void fuse_copy_init(struct fuse_copy_state *cs, struct fuse_conn *fc,
			   int write,
			   const struct iovec *iov, unsigned long nr_segs)
{
	memset(cs, 0, sizeof(*cs));
	cs->fc = fc;
	cs->write = write;
	cs->iov = iov;
	cs->nr_segs = nr_segs;
=======
static void fuse_copy_init(struct fuse_copy_state *cs, int write,
			   struct iov_iter *iter)
{
	memset(cs, 0, sizeof(*cs));
	cs->write = write;
	cs->iter = iter;
>>>>>>> v4.9.227
}

/* Unmap and put previous page of userspace buffer */
static void fuse_copy_finish(struct fuse_copy_state *cs)
{
	if (cs->currbuf) {
		struct pipe_buffer *buf = cs->currbuf;

		if (cs->write)
			buf->len = PAGE_SIZE - cs->len;
		cs->currbuf = NULL;
	} else if (cs->pg) {
		if (cs->write) {
			flush_dcache_page(cs->pg);
			set_page_dirty_lock(cs->pg);
		}
		put_page(cs->pg);
	}
	cs->pg = NULL;
}

/*
 * Get another pagefull of userspace buffer, and map it to kernel
 * address space, and lock request
 */
static int fuse_copy_fill(struct fuse_copy_state *cs)
{
	struct page *page;
	int err;

<<<<<<< HEAD
	unlock_request(cs->fc, cs->req);
=======
	err = unlock_request(cs->req);
	if (err)
		return err;

>>>>>>> v4.9.227
	fuse_copy_finish(cs);
	if (cs->pipebufs) {
		struct pipe_buffer *buf = cs->pipebufs;

		if (!cs->write) {
<<<<<<< HEAD
			err = buf->ops->confirm(cs->pipe, buf);
=======
			err = pipe_buf_confirm(cs->pipe, buf);
>>>>>>> v4.9.227
			if (err)
				return err;

			BUG_ON(!cs->nr_segs);
			cs->currbuf = buf;
			cs->pg = buf->page;
			cs->offset = buf->offset;
			cs->len = buf->len;
			cs->pipebufs++;
			cs->nr_segs--;
		} else {
			if (cs->nr_segs == cs->pipe->buffers)
				return -EIO;

			page = alloc_page(GFP_HIGHUSER);
			if (!page)
				return -ENOMEM;

			buf->page = page;
			buf->offset = 0;
			buf->len = 0;

			cs->currbuf = buf;
			cs->pg = page;
			cs->offset = 0;
			cs->len = PAGE_SIZE;
			cs->pipebufs++;
			cs->nr_segs++;
		}
	} else {
<<<<<<< HEAD
		if (!cs->seglen) {
			BUG_ON(!cs->nr_segs);
			cs->seglen = cs->iov[0].iov_len;
			cs->addr = (unsigned long) cs->iov[0].iov_base;
			cs->iov++;
			cs->nr_segs--;
		}
		err = get_user_pages_fast(cs->addr, 1, cs->write, &page);
		if (err < 0)
			return err;
		BUG_ON(err != 1);
		cs->pg = page;
		cs->offset = cs->addr % PAGE_SIZE;
		cs->len = min(PAGE_SIZE - cs->offset, cs->seglen);
		cs->seglen -= cs->len;
		cs->addr += cs->len;
	}

	return lock_request(cs->fc, cs->req);
=======
		size_t off;
		err = iov_iter_get_pages(cs->iter, &page, PAGE_SIZE, 1, &off);
		if (err < 0)
			return err;
		BUG_ON(!err);
		cs->len = err;
		cs->offset = off;
		cs->pg = page;
		iov_iter_advance(cs->iter, err);
	}

	return lock_request(cs->req);
>>>>>>> v4.9.227
}

/* Do as much copy to/from userspace buffer as we can */
static int fuse_copy_do(struct fuse_copy_state *cs, void **val, unsigned *size)
{
	unsigned ncpy = min(*size, cs->len);
	if (val) {
		void *pgaddr = kmap_atomic(cs->pg);
		void *buf = pgaddr + cs->offset;

		if (cs->write)
			memcpy(buf, *val, ncpy);
		else
			memcpy(*val, buf, ncpy);

		kunmap_atomic(pgaddr);
		*val += ncpy;
	}
	*size -= ncpy;
	cs->len -= ncpy;
	cs->offset += ncpy;
	return ncpy;
}

static int fuse_check_page(struct page *page)
{
	if (page_mapcount(page) ||
	    page->mapping != NULL ||
	    page_count(page) != 1 ||
	    (page->flags & PAGE_FLAGS_CHECK_AT_PREP &
	     ~(1 << PG_locked |
	       1 << PG_referenced |
	       1 << PG_uptodate |
	       1 << PG_lru |
	       1 << PG_active |
	       1 << PG_reclaim))) {
		printk(KERN_WARNING "fuse: trying to steal weird page\n");
		printk(KERN_WARNING "  page=%p index=%li flags=%08lx, count=%i, mapcount=%i, mapping=%p\n", page, page->index, page->flags, page_count(page), page_mapcount(page), page->mapping);
		return 1;
	}
	return 0;
}

static int fuse_try_move_page(struct fuse_copy_state *cs, struct page **pagep)
{
	int err;
	struct page *oldpage = *pagep;
	struct page *newpage;
	struct pipe_buffer *buf = cs->pipebufs;

<<<<<<< HEAD
	unlock_request(cs->fc, cs->req);
	fuse_copy_finish(cs);

	err = buf->ops->confirm(cs->pipe, buf);
=======
	err = unlock_request(cs->req);
	if (err)
		return err;

	fuse_copy_finish(cs);

	err = pipe_buf_confirm(cs->pipe, buf);
>>>>>>> v4.9.227
	if (err)
		return err;

	BUG_ON(!cs->nr_segs);
	cs->currbuf = buf;
	cs->len = buf->len;
	cs->pipebufs++;
	cs->nr_segs--;

	if (cs->len != PAGE_SIZE)
		goto out_fallback;

<<<<<<< HEAD
	if (buf->ops->steal(cs->pipe, buf) != 0)
=======
	if (pipe_buf_steal(cs->pipe, buf) != 0)
>>>>>>> v4.9.227
		goto out_fallback;

	newpage = buf->page;

	if (!PageUptodate(newpage))
		SetPageUptodate(newpage);

	ClearPageMappedToDisk(newpage);

	if (fuse_check_page(newpage) != 0)
		goto out_fallback_unlock;

	/*
	 * This is a new and locked page, it shouldn't be mapped or
	 * have any special flags on it
	 */
	if (WARN_ON(page_mapped(oldpage)))
		goto out_fallback_unlock;
	if (WARN_ON(page_has_private(oldpage)))
		goto out_fallback_unlock;
	if (WARN_ON(PageDirty(oldpage) || PageWriteback(oldpage)))
		goto out_fallback_unlock;
	if (WARN_ON(PageMlocked(oldpage)))
		goto out_fallback_unlock;

	err = replace_page_cache_page(oldpage, newpage, GFP_KERNEL);
	if (err) {
		unlock_page(newpage);
		return err;
	}

<<<<<<< HEAD
	page_cache_get(newpage);
=======
	get_page(newpage);
>>>>>>> v4.9.227

	if (!(buf->flags & PIPE_BUF_FLAG_LRU))
		lru_cache_add_file(newpage);

	err = 0;
<<<<<<< HEAD
	spin_lock(&cs->fc->lock);
	if (cs->req->aborted)
		err = -ENOENT;
	else
		*pagep = newpage;
	spin_unlock(&cs->fc->lock);

	if (err) {
		unlock_page(newpage);
		page_cache_release(newpage);
=======
	spin_lock(&cs->req->waitq.lock);
	if (test_bit(FR_ABORTED, &cs->req->flags))
		err = -ENOENT;
	else
		*pagep = newpage;
	spin_unlock(&cs->req->waitq.lock);

	if (err) {
		unlock_page(newpage);
		put_page(newpage);
>>>>>>> v4.9.227
		return err;
	}

	unlock_page(oldpage);
<<<<<<< HEAD
	page_cache_release(oldpage);
=======
	put_page(oldpage);
>>>>>>> v4.9.227
	cs->len = 0;

	return 0;

out_fallback_unlock:
	unlock_page(newpage);
out_fallback:
	cs->pg = buf->page;
	cs->offset = buf->offset;

<<<<<<< HEAD
	err = lock_request(cs->fc, cs->req);
=======
	err = lock_request(cs->req);
>>>>>>> v4.9.227
	if (err)
		return err;

	return 1;
}

static int fuse_ref_page(struct fuse_copy_state *cs, struct page *page,
			 unsigned offset, unsigned count)
{
	struct pipe_buffer *buf;
<<<<<<< HEAD
=======
	int err;
>>>>>>> v4.9.227

	if (cs->nr_segs == cs->pipe->buffers)
		return -EIO;

<<<<<<< HEAD
	unlock_request(cs->fc, cs->req);
	fuse_copy_finish(cs);

	buf = cs->pipebufs;
	page_cache_get(page);
=======
	err = unlock_request(cs->req);
	if (err)
		return err;

	fuse_copy_finish(cs);

	buf = cs->pipebufs;
	get_page(page);
>>>>>>> v4.9.227
	buf->page = page;
	buf->offset = offset;
	buf->len = count;

	cs->pipebufs++;
	cs->nr_segs++;
	cs->len = 0;

	return 0;
}

/*
 * Copy a page in the request to/from the userspace buffer.  Must be
 * done atomically
 */
static int fuse_copy_page(struct fuse_copy_state *cs, struct page **pagep,
			  unsigned offset, unsigned count, int zeroing)
{
	int err;
	struct page *page = *pagep;

	if (page && zeroing && count < PAGE_SIZE)
		clear_highpage(page);

	while (count) {
		if (cs->write && cs->pipebufs && page) {
			return fuse_ref_page(cs, page, offset, count);
		} else if (!cs->len) {
			if (cs->move_pages && page &&
			    offset == 0 && count == PAGE_SIZE) {
				err = fuse_try_move_page(cs, pagep);
				if (err <= 0)
					return err;
			} else {
				err = fuse_copy_fill(cs);
				if (err)
					return err;
			}
		}
		if (page) {
			void *mapaddr = kmap_atomic(page);
			void *buf = mapaddr + offset;
			offset += fuse_copy_do(cs, &buf, &count);
			kunmap_atomic(mapaddr);
		} else
			offset += fuse_copy_do(cs, NULL, &count);
	}
	if (page && !cs->write)
		flush_dcache_page(page);
	return 0;
}

/* Copy pages in the request to/from userspace buffer */
static int fuse_copy_pages(struct fuse_copy_state *cs, unsigned nbytes,
			   int zeroing)
{
	unsigned i;
	struct fuse_req *req = cs->req;

	for (i = 0; i < req->num_pages && (nbytes || zeroing); i++) {
		int err;
		unsigned offset = req->page_descs[i].offset;
		unsigned count = min(nbytes, req->page_descs[i].length);

		err = fuse_copy_page(cs, &req->pages[i], offset, count,
				     zeroing);
		if (err)
			return err;

		nbytes -= count;
	}
	return 0;
}

/* Copy a single argument in the request to/from userspace buffer */
static int fuse_copy_one(struct fuse_copy_state *cs, void *val, unsigned size)
{
	while (size) {
		if (!cs->len) {
			int err = fuse_copy_fill(cs);
			if (err)
				return err;
		}
		fuse_copy_do(cs, &val, &size);
	}
	return 0;
}

/* Copy request arguments to/from userspace buffer */
static int fuse_copy_args(struct fuse_copy_state *cs, unsigned numargs,
			  unsigned argpages, struct fuse_arg *args,
			  int zeroing)
{
	int err = 0;
	unsigned i;

	for (i = 0; !err && i < numargs; i++)  {
		struct fuse_arg *arg = &args[i];
		if (i == numargs - 1 && argpages)
			err = fuse_copy_pages(cs, arg->size, zeroing);
		else
			err = fuse_copy_one(cs, arg->value, arg->size);
	}
	return err;
}

<<<<<<< HEAD
static int forget_pending(struct fuse_conn *fc)
{
	return fc->forget_list_head.next != NULL;
}

static int request_pending(struct fuse_conn *fc)
{
	return !list_empty(&fc->pending) || !list_empty(&fc->interrupts) ||
		forget_pending(fc);
}

/* Wait until a request is available on the pending list */
static void request_wait(struct fuse_conn *fc)
__releases(fc->lock)
__acquires(fc->lock)
{
	DECLARE_WAITQUEUE(wait, current);

	add_wait_queue_exclusive(&fc->waitq, &wait);
	while (fc->connected && !request_pending(fc)) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (signal_pending(current))
			break;

		spin_unlock(&fc->lock);
		schedule();
		spin_lock(&fc->lock);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&fc->waitq, &wait);
=======
static int forget_pending(struct fuse_iqueue *fiq)
{
	return fiq->forget_list_head.next != NULL;
}

static int request_pending(struct fuse_iqueue *fiq)
{
	return !list_empty(&fiq->pending) || !list_empty(&fiq->interrupts) ||
		forget_pending(fiq);
>>>>>>> v4.9.227
}

/*
 * Transfer an interrupt request to userspace
 *
 * Unlike other requests this is assembled on demand, without a need
 * to allocate a separate fuse_req structure.
 *
<<<<<<< HEAD
 * Called with fc->lock held, releases it
 */
static int fuse_read_interrupt(struct fuse_conn *fc, struct fuse_copy_state *cs,
			       size_t nbytes, struct fuse_req *req)
__releases(fc->lock)
=======
 * Called with fiq->waitq.lock held, releases it
 */
static int fuse_read_interrupt(struct fuse_iqueue *fiq,
			       struct fuse_copy_state *cs,
			       size_t nbytes, struct fuse_req *req)
__releases(fiq->waitq.lock)
>>>>>>> v4.9.227
{
	struct fuse_in_header ih;
	struct fuse_interrupt_in arg;
	unsigned reqsize = sizeof(ih) + sizeof(arg);
	int err;

	list_del_init(&req->intr_entry);
<<<<<<< HEAD
	req->intr_unique = fuse_get_unique(fc);
=======
	req->intr_unique = fuse_get_unique(fiq);
>>>>>>> v4.9.227
	memset(&ih, 0, sizeof(ih));
	memset(&arg, 0, sizeof(arg));
	ih.len = reqsize;
	ih.opcode = FUSE_INTERRUPT;
	ih.unique = req->intr_unique;
	arg.unique = req->in.h.unique;

<<<<<<< HEAD
	spin_unlock(&fc->lock);
=======
	spin_unlock(&fiq->waitq.lock);
>>>>>>> v4.9.227
	if (nbytes < reqsize)
		return -EINVAL;

	err = fuse_copy_one(cs, &ih, sizeof(ih));
	if (!err)
		err = fuse_copy_one(cs, &arg, sizeof(arg));
	fuse_copy_finish(cs);

	return err ? err : reqsize;
}

<<<<<<< HEAD
static struct fuse_forget_link *dequeue_forget(struct fuse_conn *fc,
					       unsigned max,
					       unsigned *countp)
{
	struct fuse_forget_link *head = fc->forget_list_head.next;
=======
static struct fuse_forget_link *dequeue_forget(struct fuse_iqueue *fiq,
					       unsigned max,
					       unsigned *countp)
{
	struct fuse_forget_link *head = fiq->forget_list_head.next;
>>>>>>> v4.9.227
	struct fuse_forget_link **newhead = &head;
	unsigned count;

	for (count = 0; *newhead != NULL && count < max; count++)
		newhead = &(*newhead)->next;

<<<<<<< HEAD
	fc->forget_list_head.next = *newhead;
	*newhead = NULL;
	if (fc->forget_list_head.next == NULL)
		fc->forget_list_tail = &fc->forget_list_head;
=======
	fiq->forget_list_head.next = *newhead;
	*newhead = NULL;
	if (fiq->forget_list_head.next == NULL)
		fiq->forget_list_tail = &fiq->forget_list_head;
>>>>>>> v4.9.227

	if (countp != NULL)
		*countp = count;

	return head;
}

<<<<<<< HEAD
static int fuse_read_single_forget(struct fuse_conn *fc,
				   struct fuse_copy_state *cs,
				   size_t nbytes)
__releases(fc->lock)
{
	int err;
	struct fuse_forget_link *forget = dequeue_forget(fc, 1, NULL);
=======
static int fuse_read_single_forget(struct fuse_iqueue *fiq,
				   struct fuse_copy_state *cs,
				   size_t nbytes)
__releases(fiq->waitq.lock)
{
	int err;
	struct fuse_forget_link *forget = dequeue_forget(fiq, 1, NULL);
>>>>>>> v4.9.227
	struct fuse_forget_in arg = {
		.nlookup = forget->forget_one.nlookup,
	};
	struct fuse_in_header ih = {
		.opcode = FUSE_FORGET,
		.nodeid = forget->forget_one.nodeid,
<<<<<<< HEAD
		.unique = fuse_get_unique(fc),
		.len = sizeof(ih) + sizeof(arg),
	};

	spin_unlock(&fc->lock);
=======
		.unique = fuse_get_unique(fiq),
		.len = sizeof(ih) + sizeof(arg),
	};

	spin_unlock(&fiq->waitq.lock);
>>>>>>> v4.9.227
	kfree(forget);
	if (nbytes < ih.len)
		return -EINVAL;

	err = fuse_copy_one(cs, &ih, sizeof(ih));
	if (!err)
		err = fuse_copy_one(cs, &arg, sizeof(arg));
	fuse_copy_finish(cs);

	if (err)
		return err;

	return ih.len;
}

<<<<<<< HEAD
static int fuse_read_batch_forget(struct fuse_conn *fc,
				   struct fuse_copy_state *cs, size_t nbytes)
__releases(fc->lock)
=======
static int fuse_read_batch_forget(struct fuse_iqueue *fiq,
				   struct fuse_copy_state *cs, size_t nbytes)
__releases(fiq->waitq.lock)
>>>>>>> v4.9.227
{
	int err;
	unsigned max_forgets;
	unsigned count;
	struct fuse_forget_link *head;
	struct fuse_batch_forget_in arg = { .count = 0 };
	struct fuse_in_header ih = {
		.opcode = FUSE_BATCH_FORGET,
<<<<<<< HEAD
		.unique = fuse_get_unique(fc),
=======
		.unique = fuse_get_unique(fiq),
>>>>>>> v4.9.227
		.len = sizeof(ih) + sizeof(arg),
	};

	if (nbytes < ih.len) {
<<<<<<< HEAD
		spin_unlock(&fc->lock);
=======
		spin_unlock(&fiq->waitq.lock);
>>>>>>> v4.9.227
		return -EINVAL;
	}

	max_forgets = (nbytes - ih.len) / sizeof(struct fuse_forget_one);
<<<<<<< HEAD
	head = dequeue_forget(fc, max_forgets, &count);
	spin_unlock(&fc->lock);
=======
	head = dequeue_forget(fiq, max_forgets, &count);
	spin_unlock(&fiq->waitq.lock);
>>>>>>> v4.9.227

	arg.count = count;
	ih.len += count * sizeof(struct fuse_forget_one);
	err = fuse_copy_one(cs, &ih, sizeof(ih));
	if (!err)
		err = fuse_copy_one(cs, &arg, sizeof(arg));

	while (head) {
		struct fuse_forget_link *forget = head;

		if (!err) {
			err = fuse_copy_one(cs, &forget->forget_one,
					    sizeof(forget->forget_one));
		}
		head = forget->next;
		kfree(forget);
	}

	fuse_copy_finish(cs);

	if (err)
		return err;

	return ih.len;
}

<<<<<<< HEAD
static int fuse_read_forget(struct fuse_conn *fc, struct fuse_copy_state *cs,
			    size_t nbytes)
__releases(fc->lock)
{
	if (fc->minor < 16 || fc->forget_list_head.next->next == NULL)
		return fuse_read_single_forget(fc, cs, nbytes);
	else
		return fuse_read_batch_forget(fc, cs, nbytes);
=======
static int fuse_read_forget(struct fuse_conn *fc, struct fuse_iqueue *fiq,
			    struct fuse_copy_state *cs,
			    size_t nbytes)
__releases(fiq->waitq.lock)
{
	if (fc->minor < 16 || fiq->forget_list_head.next->next == NULL)
		return fuse_read_single_forget(fiq, cs, nbytes);
	else
		return fuse_read_batch_forget(fiq, cs, nbytes);
>>>>>>> v4.9.227
}

/*
 * Read a single request into the userspace filesystem's buffer.  This
 * function waits until a request is available, then removes it from
 * the pending list and copies request data to userspace buffer.  If
 * no reply is needed (FORGET) or request has been aborted or there
 * was an error during the copying then it's finished by calling
 * request_end().  Otherwise add it to the processing list, and set
 * the 'sent' flag.
 */
<<<<<<< HEAD
static ssize_t fuse_dev_do_read(struct fuse_conn *fc, struct file *file,
				struct fuse_copy_state *cs, size_t nbytes)
{
	int err;
=======
static ssize_t fuse_dev_do_read(struct fuse_dev *fud, struct file *file,
				struct fuse_copy_state *cs, size_t nbytes)
{
	ssize_t err;
	struct fuse_conn *fc = fud->fc;
	struct fuse_iqueue *fiq = &fc->iq;
	struct fuse_pqueue *fpq = &fud->pq;
>>>>>>> v4.9.227
	struct fuse_req *req;
	struct fuse_in *in;
	unsigned reqsize;

 restart:
<<<<<<< HEAD
	spin_lock(&fc->lock);
	err = -EAGAIN;
	if ((file->f_flags & O_NONBLOCK) && fc->connected &&
	    !request_pending(fc))
		goto err_unlock;

	request_wait(fc);
	err = -ENODEV;
	if (!fc->connected)
		goto err_unlock;
	err = -ERESTARTSYS;
	if (!request_pending(fc))
		goto err_unlock;

	if (!list_empty(&fc->interrupts)) {
		req = list_entry(fc->interrupts.next, struct fuse_req,
				 intr_entry);
		return fuse_read_interrupt(fc, cs, nbytes, req);
	}

	if (forget_pending(fc)) {
		if (list_empty(&fc->pending) || fc->forget_batch-- > 0)
			return fuse_read_forget(fc, cs, nbytes);

		if (fc->forget_batch <= -8)
			fc->forget_batch = 16;
	}

	req = list_entry(fc->pending.next, struct fuse_req, list);
	req->state = FUSE_REQ_READING;
	list_move(&req->list, &fc->io);
=======
	spin_lock(&fiq->waitq.lock);
	err = -EAGAIN;
	if ((file->f_flags & O_NONBLOCK) && fiq->connected &&
	    !request_pending(fiq))
		goto err_unlock;

	err = wait_event_interruptible_exclusive_locked(fiq->waitq,
				!fiq->connected || request_pending(fiq));
	if (err)
		goto err_unlock;

	err = -ENODEV;
	if (!fiq->connected)
		goto err_unlock;

	if (!list_empty(&fiq->interrupts)) {
		req = list_entry(fiq->interrupts.next, struct fuse_req,
				 intr_entry);
		return fuse_read_interrupt(fiq, cs, nbytes, req);
	}

	if (forget_pending(fiq)) {
		if (list_empty(&fiq->pending) || fiq->forget_batch-- > 0)
			return fuse_read_forget(fc, fiq, cs, nbytes);

		if (fiq->forget_batch <= -8)
			fiq->forget_batch = 16;
	}

	req = list_entry(fiq->pending.next, struct fuse_req, list);
	clear_bit(FR_PENDING, &req->flags);
	list_del_init(&req->list);
	spin_unlock(&fiq->waitq.lock);
>>>>>>> v4.9.227

	in = &req->in;
	reqsize = in->h.len;
	/* If request is too large, reply with an error and restart the read */
	if (nbytes < reqsize) {
		req->out.h.error = -EIO;
		/* SETXATTR is special, since it may contain too large data */
		if (in->h.opcode == FUSE_SETXATTR)
			req->out.h.error = -E2BIG;
		request_end(fc, req);
		goto restart;
	}
<<<<<<< HEAD
	spin_unlock(&fc->lock);
=======
	spin_lock(&fpq->lock);
	list_add(&req->list, &fpq->io);
	spin_unlock(&fpq->lock);
>>>>>>> v4.9.227
	cs->req = req;
	err = fuse_copy_one(cs, &in->h, sizeof(in->h));
	if (!err)
		err = fuse_copy_args(cs, in->numargs, in->argpages,
				     (struct fuse_arg *) in->args, 0);
	fuse_copy_finish(cs);
<<<<<<< HEAD
	spin_lock(&fc->lock);
	req->locked = 0;
	if (req->aborted) {
		request_end(fc, req);
		return -ENODEV;
	}
	if (err) {
		req->out.h.error = -EIO;
		request_end(fc, req);
		return err;
	}
	if (!req->isreply)
		request_end(fc, req);
	else {
		req->state = FUSE_REQ_SENT;
		list_move_tail(&req->list, &fc->processing);
		if (req->interrupted)
			queue_interrupt(fc, req);
		spin_unlock(&fc->lock);
	}
	return reqsize;

 err_unlock:
	spin_unlock(&fc->lock);
	return err;
}

static ssize_t fuse_dev_read(struct kiocb *iocb, const struct iovec *iov,
			      unsigned long nr_segs, loff_t pos)
{
	struct fuse_copy_state cs;
	struct file *file = iocb->ki_filp;
	struct fuse_conn *fc = fuse_get_conn(file);
	if (!fc)
		return -EPERM;

	fuse_copy_init(&cs, fc, 1, iov, nr_segs);

	return fuse_dev_do_read(fc, file, &cs, iov_length(iov, nr_segs));
=======
	spin_lock(&fpq->lock);
	clear_bit(FR_LOCKED, &req->flags);
	if (!fpq->connected) {
		err = -ENODEV;
		goto out_end;
	}
	if (err) {
		req->out.h.error = -EIO;
		goto out_end;
	}
	if (!test_bit(FR_ISREPLY, &req->flags)) {
		err = reqsize;
		goto out_end;
	}
	list_move_tail(&req->list, &fpq->processing);
	__fuse_get_request(req);
	set_bit(FR_SENT, &req->flags);
	spin_unlock(&fpq->lock);
	/* matches barrier in request_wait_answer() */
	smp_mb__after_atomic();
	if (test_bit(FR_INTERRUPTED, &req->flags))
		queue_interrupt(fiq, req);
	fuse_put_request(fc, req);

	return reqsize;

out_end:
	if (!test_bit(FR_PRIVATE, &req->flags))
		list_del_init(&req->list);
	spin_unlock(&fpq->lock);
	request_end(fc, req);
	return err;

 err_unlock:
	spin_unlock(&fiq->waitq.lock);
	return err;
}

static int fuse_dev_open(struct inode *inode, struct file *file)
{
	/*
	 * The fuse device's file's private_data is used to hold
	 * the fuse_conn(ection) when it is mounted, and is used to
	 * keep track of whether the file has been mounted already.
	 */
	file->private_data = NULL;
	return 0;
}

static ssize_t fuse_dev_read(struct kiocb *iocb, struct iov_iter *to)
{
	struct fuse_copy_state cs;
	struct file *file = iocb->ki_filp;
	struct fuse_dev *fud = fuse_get_dev(file);

	if (!fud)
		return -EPERM;

	if (!iter_is_iovec(to))
		return -EINVAL;

	fuse_copy_init(&cs, 1, to);

	return fuse_dev_do_read(fud, file, &cs, iov_iter_count(to));
>>>>>>> v4.9.227
}

static ssize_t fuse_dev_splice_read(struct file *in, loff_t *ppos,
				    struct pipe_inode_info *pipe,
				    size_t len, unsigned int flags)
{
<<<<<<< HEAD
	int ret;
	int page_nr = 0;
	int do_wakeup = 0;
	struct pipe_buffer *bufs;
	struct fuse_copy_state cs;
	struct fuse_conn *fc = fuse_get_conn(in);
	if (!fc)
=======
	int total, ret;
	int page_nr = 0;
	struct pipe_buffer *bufs;
	struct fuse_copy_state cs;
	struct fuse_dev *fud = fuse_get_dev(in);

	if (!fud)
>>>>>>> v4.9.227
		return -EPERM;

	bufs = kmalloc(pipe->buffers * sizeof(struct pipe_buffer), GFP_KERNEL);
	if (!bufs)
		return -ENOMEM;

<<<<<<< HEAD
	fuse_copy_init(&cs, fc, 1, NULL, 0);
	cs.pipebufs = bufs;
	cs.pipe = pipe;
	ret = fuse_dev_do_read(fc, in, &cs, len);
	if (ret < 0)
		goto out;

	ret = 0;
	pipe_lock(pipe);

	if (!pipe->readers) {
		send_sig(SIGPIPE, current, 0);
		if (!ret)
			ret = -EPIPE;
		goto out_unlock;
	}

	if (pipe->nrbufs + cs.nr_segs > pipe->buffers) {
		ret = -EIO;
		goto out_unlock;
	}

	while (page_nr < cs.nr_segs) {
		int newbuf = (pipe->curbuf + pipe->nrbufs) & (pipe->buffers - 1);
		struct pipe_buffer *buf = pipe->bufs + newbuf;

		buf->page = bufs[page_nr].page;
		buf->offset = bufs[page_nr].offset;
		buf->len = bufs[page_nr].len;
=======
	fuse_copy_init(&cs, 1, NULL);
	cs.pipebufs = bufs;
	cs.pipe = pipe;
	ret = fuse_dev_do_read(fud, in, &cs, len);
	if (ret < 0)
		goto out;

	if (pipe->nrbufs + cs.nr_segs > pipe->buffers) {
		ret = -EIO;
		goto out;
	}

	for (ret = total = 0; page_nr < cs.nr_segs; total += ret) {
>>>>>>> v4.9.227
		/*
		 * Need to be careful about this.  Having buf->ops in module
		 * code can Oops if the buffer persists after module unload.
		 */
<<<<<<< HEAD
		buf->ops = &nosteal_pipe_buf_ops;

		pipe->nrbufs++;
		page_nr++;
		ret += buf->len;

		if (pipe->files)
			do_wakeup = 1;
	}

out_unlock:
	pipe_unlock(pipe);

	if (do_wakeup) {
		smp_mb();
		if (waitqueue_active(&pipe->wait))
			wake_up_interruptible(&pipe->wait);
		kill_fasync(&pipe->fasync_readers, SIGIO, POLL_IN);
	}

out:
	for (; page_nr < cs.nr_segs; page_nr++)
		page_cache_release(bufs[page_nr].page);
=======
		bufs[page_nr].ops = &nosteal_pipe_buf_ops;
		bufs[page_nr].flags = 0;
		ret = add_to_pipe(pipe, &bufs[page_nr++]);
		if (unlikely(ret < 0))
			break;
	}
	if (total)
		ret = total;
out:
	for (; page_nr < cs.nr_segs; page_nr++)
		put_page(bufs[page_nr].page);
>>>>>>> v4.9.227

	kfree(bufs);
	return ret;
}

static int fuse_notify_poll(struct fuse_conn *fc, unsigned int size,
			    struct fuse_copy_state *cs)
{
	struct fuse_notify_poll_wakeup_out outarg;
	int err = -EINVAL;

	if (size != sizeof(outarg))
		goto err;

	err = fuse_copy_one(cs, &outarg, sizeof(outarg));
	if (err)
		goto err;

	fuse_copy_finish(cs);
	return fuse_notify_poll_wakeup(fc, &outarg);

err:
	fuse_copy_finish(cs);
	return err;
}

static int fuse_notify_inval_inode(struct fuse_conn *fc, unsigned int size,
				   struct fuse_copy_state *cs)
{
	struct fuse_notify_inval_inode_out outarg;
	int err = -EINVAL;

	if (size != sizeof(outarg))
		goto err;

	err = fuse_copy_one(cs, &outarg, sizeof(outarg));
	if (err)
		goto err;
	fuse_copy_finish(cs);

	down_read(&fc->killsb);
	err = -ENOENT;
	if (fc->sb) {
		err = fuse_reverse_inval_inode(fc->sb, outarg.ino,
					       outarg.off, outarg.len);
	}
	up_read(&fc->killsb);
	return err;

err:
	fuse_copy_finish(cs);
	return err;
}

static int fuse_notify_inval_entry(struct fuse_conn *fc, unsigned int size,
				   struct fuse_copy_state *cs)
{
	struct fuse_notify_inval_entry_out outarg;
	int err = -ENOMEM;
	char *buf;
	struct qstr name;

	buf = kzalloc(FUSE_NAME_MAX + 1, GFP_KERNEL);
	if (!buf)
		goto err;

	err = -EINVAL;
	if (size < sizeof(outarg))
		goto err;

	err = fuse_copy_one(cs, &outarg, sizeof(outarg));
	if (err)
		goto err;

	err = -ENAMETOOLONG;
	if (outarg.namelen > FUSE_NAME_MAX)
		goto err;

	err = -EINVAL;
	if (size != sizeof(outarg) + outarg.namelen + 1)
		goto err;

	name.name = buf;
	name.len = outarg.namelen;
	err = fuse_copy_one(cs, buf, outarg.namelen + 1);
	if (err)
		goto err;
	fuse_copy_finish(cs);
	buf[outarg.namelen] = 0;
<<<<<<< HEAD
	name.hash = full_name_hash(name.name, name.len);
=======
>>>>>>> v4.9.227

	down_read(&fc->killsb);
	err = -ENOENT;
	if (fc->sb)
		err = fuse_reverse_inval_entry(fc->sb, outarg.parent, 0, &name);
	up_read(&fc->killsb);
	kfree(buf);
	return err;

err:
	kfree(buf);
	fuse_copy_finish(cs);
	return err;
}

static int fuse_notify_delete(struct fuse_conn *fc, unsigned int size,
			      struct fuse_copy_state *cs)
{
	struct fuse_notify_delete_out outarg;
	int err = -ENOMEM;
	char *buf;
	struct qstr name;

	buf = kzalloc(FUSE_NAME_MAX + 1, GFP_KERNEL);
	if (!buf)
		goto err;

	err = -EINVAL;
	if (size < sizeof(outarg))
		goto err;

	err = fuse_copy_one(cs, &outarg, sizeof(outarg));
	if (err)
		goto err;

	err = -ENAMETOOLONG;
	if (outarg.namelen > FUSE_NAME_MAX)
		goto err;

	err = -EINVAL;
	if (size != sizeof(outarg) + outarg.namelen + 1)
		goto err;

	name.name = buf;
	name.len = outarg.namelen;
	err = fuse_copy_one(cs, buf, outarg.namelen + 1);
	if (err)
		goto err;
	fuse_copy_finish(cs);
	buf[outarg.namelen] = 0;
<<<<<<< HEAD
	name.hash = full_name_hash(name.name, name.len);
=======
>>>>>>> v4.9.227

	down_read(&fc->killsb);
	err = -ENOENT;
	if (fc->sb)
		err = fuse_reverse_inval_entry(fc->sb, outarg.parent,
					       outarg.child, &name);
	up_read(&fc->killsb);
	kfree(buf);
	return err;

err:
	kfree(buf);
	fuse_copy_finish(cs);
	return err;
}

static int fuse_notify_store(struct fuse_conn *fc, unsigned int size,
			     struct fuse_copy_state *cs)
{
	struct fuse_notify_store_out outarg;
	struct inode *inode;
	struct address_space *mapping;
	u64 nodeid;
	int err;
	pgoff_t index;
	unsigned int offset;
	unsigned int num;
	loff_t file_size;
	loff_t end;

	err = -EINVAL;
	if (size < sizeof(outarg))
		goto out_finish;

	err = fuse_copy_one(cs, &outarg, sizeof(outarg));
	if (err)
		goto out_finish;

	err = -EINVAL;
	if (size - sizeof(outarg) != outarg.size)
		goto out_finish;

	nodeid = outarg.nodeid;

	down_read(&fc->killsb);

	err = -ENOENT;
	if (!fc->sb)
		goto out_up_killsb;

	inode = ilookup5(fc->sb, nodeid, fuse_inode_eq, &nodeid);
	if (!inode)
		goto out_up_killsb;

	mapping = inode->i_mapping;
<<<<<<< HEAD
	index = outarg.offset >> PAGE_CACHE_SHIFT;
	offset = outarg.offset & ~PAGE_CACHE_MASK;
=======
	index = outarg.offset >> PAGE_SHIFT;
	offset = outarg.offset & ~PAGE_MASK;
>>>>>>> v4.9.227
	file_size = i_size_read(inode);
	end = outarg.offset + outarg.size;
	if (end > file_size) {
		file_size = end;
		fuse_write_update_size(inode, file_size);
	}

	num = outarg.size;
	while (num) {
		struct page *page;
		unsigned int this_num;

		err = -ENOMEM;
		page = find_or_create_page(mapping, index,
					   mapping_gfp_mask(mapping));
		if (!page)
			goto out_iput;

<<<<<<< HEAD
		this_num = min_t(unsigned, num, PAGE_CACHE_SIZE - offset);
		err = fuse_copy_page(cs, &page, offset, this_num, 0);
		if (!err && offset == 0 &&
		    (this_num == PAGE_CACHE_SIZE || file_size == end))
			SetPageUptodate(page);
		unlock_page(page);
		page_cache_release(page);
=======
		this_num = min_t(unsigned, num, PAGE_SIZE - offset);
		err = fuse_copy_page(cs, &page, offset, this_num, 0);
		if (!err && offset == 0 &&
		    (this_num == PAGE_SIZE || file_size == end))
			SetPageUptodate(page);
		unlock_page(page);
		put_page(page);
>>>>>>> v4.9.227

		if (err)
			goto out_iput;

		num -= this_num;
		offset = 0;
		index++;
	}

	err = 0;

out_iput:
	iput(inode);
out_up_killsb:
	up_read(&fc->killsb);
out_finish:
	fuse_copy_finish(cs);
	return err;
}

static void fuse_retrieve_end(struct fuse_conn *fc, struct fuse_req *req)
{
	release_pages(req->pages, req->num_pages, false);
}

static int fuse_retrieve(struct fuse_conn *fc, struct inode *inode,
			 struct fuse_notify_retrieve_out *outarg)
{
	int err;
	struct address_space *mapping = inode->i_mapping;
	struct fuse_req *req;
	pgoff_t index;
	loff_t file_size;
	unsigned int num;
	unsigned int offset;
	size_t total_len = 0;
	int num_pages;

<<<<<<< HEAD
	offset = outarg->offset & ~PAGE_CACHE_MASK;
	file_size = i_size_read(inode);

	num = outarg->size;
=======
	offset = outarg->offset & ~PAGE_MASK;
	file_size = i_size_read(inode);

	num = min(outarg->size, fc->max_write);
>>>>>>> v4.9.227
	if (outarg->offset > file_size)
		num = 0;
	else if (outarg->offset + num > file_size)
		num = file_size - outarg->offset;

	num_pages = (num + offset + PAGE_SIZE - 1) >> PAGE_SHIFT;
	num_pages = min(num_pages, FUSE_MAX_PAGES_PER_REQ);

	req = fuse_get_req(fc, num_pages);
	if (IS_ERR(req))
		return PTR_ERR(req);

	req->in.h.opcode = FUSE_NOTIFY_REPLY;
	req->in.h.nodeid = outarg->nodeid;
	req->in.numargs = 2;
	req->in.argpages = 1;
<<<<<<< HEAD
	req->page_descs[0].offset = offset;
	req->end = fuse_retrieve_end;

	index = outarg->offset >> PAGE_CACHE_SHIFT;
=======
	req->end = fuse_retrieve_end;

	index = outarg->offset >> PAGE_SHIFT;
>>>>>>> v4.9.227

	while (num && req->num_pages < num_pages) {
		struct page *page;
		unsigned int this_num;

		page = find_get_page(mapping, index);
		if (!page)
			break;

<<<<<<< HEAD
		this_num = min_t(unsigned, num, PAGE_CACHE_SIZE - offset);
		req->pages[req->num_pages] = page;
=======
		this_num = min_t(unsigned, num, PAGE_SIZE - offset);
		req->pages[req->num_pages] = page;
		req->page_descs[req->num_pages].offset = offset;
>>>>>>> v4.9.227
		req->page_descs[req->num_pages].length = this_num;
		req->num_pages++;

		offset = 0;
		num -= this_num;
		total_len += this_num;
		index++;
	}
	req->misc.retrieve_in.offset = outarg->offset;
	req->misc.retrieve_in.size = total_len;
	req->in.args[0].size = sizeof(req->misc.retrieve_in);
	req->in.args[0].value = &req->misc.retrieve_in;
	req->in.args[1].size = total_len;

	err = fuse_request_send_notify_reply(fc, req, outarg->notify_unique);
<<<<<<< HEAD
	if (err)
		fuse_retrieve_end(fc, req);
=======
	if (err) {
		fuse_retrieve_end(fc, req);
		fuse_put_request(fc, req);
	}
>>>>>>> v4.9.227

	return err;
}

static int fuse_notify_retrieve(struct fuse_conn *fc, unsigned int size,
				struct fuse_copy_state *cs)
{
	struct fuse_notify_retrieve_out outarg;
	struct inode *inode;
	int err;

	err = -EINVAL;
	if (size != sizeof(outarg))
		goto copy_finish;

	err = fuse_copy_one(cs, &outarg, sizeof(outarg));
	if (err)
		goto copy_finish;

	fuse_copy_finish(cs);

	down_read(&fc->killsb);
	err = -ENOENT;
	if (fc->sb) {
		u64 nodeid = outarg.nodeid;

		inode = ilookup5(fc->sb, nodeid, fuse_inode_eq, &nodeid);
		if (inode) {
			err = fuse_retrieve(fc, inode, &outarg);
			iput(inode);
		}
	}
	up_read(&fc->killsb);

	return err;

copy_finish:
	fuse_copy_finish(cs);
	return err;
}

static int fuse_notify(struct fuse_conn *fc, enum fuse_notify_code code,
		       unsigned int size, struct fuse_copy_state *cs)
{
	/* Don't try to move pages (yet) */
	cs->move_pages = 0;

	switch (code) {
	case FUSE_NOTIFY_POLL:
		return fuse_notify_poll(fc, size, cs);

	case FUSE_NOTIFY_INVAL_INODE:
		return fuse_notify_inval_inode(fc, size, cs);

	case FUSE_NOTIFY_INVAL_ENTRY:
		return fuse_notify_inval_entry(fc, size, cs);

	case FUSE_NOTIFY_STORE:
		return fuse_notify_store(fc, size, cs);

	case FUSE_NOTIFY_RETRIEVE:
		return fuse_notify_retrieve(fc, size, cs);

	case FUSE_NOTIFY_DELETE:
		return fuse_notify_delete(fc, size, cs);

	default:
		fuse_copy_finish(cs);
		return -EINVAL;
	}
}

/* Look up request on processing list by unique ID */
<<<<<<< HEAD
static struct fuse_req *request_find(struct fuse_conn *fc, u64 unique)
{
	struct fuse_req *req;

	list_for_each_entry(req, &fc->processing, list) {
=======
static struct fuse_req *request_find(struct fuse_pqueue *fpq, u64 unique)
{
	struct fuse_req *req;

	list_for_each_entry(req, &fpq->processing, list) {
>>>>>>> v4.9.227
		if (req->in.h.unique == unique || req->intr_unique == unique)
			return req;
	}
	return NULL;
}

static int copy_out_args(struct fuse_copy_state *cs, struct fuse_out *out,
			 unsigned nbytes)
{
	unsigned reqsize = sizeof(struct fuse_out_header);

	if (out->h.error)
		return nbytes != reqsize ? -EINVAL : 0;

	reqsize += len_args(out->numargs, out->args);

	if (reqsize < nbytes || (reqsize > nbytes && !out->argvar))
		return -EINVAL;
	else if (reqsize > nbytes) {
		struct fuse_arg *lastarg = &out->args[out->numargs-1];
		unsigned diffsize = reqsize - nbytes;
		if (diffsize > lastarg->size)
			return -EINVAL;
		lastarg->size -= diffsize;
	}
	return fuse_copy_args(cs, out->numargs, out->argpages, out->args,
			      out->page_zeroing);
}

/*
 * Write a single reply to a request.  First the header is copied from
 * the write buffer.  The request is then searched on the processing
 * list by the unique ID found in the header.  If found, then remove
 * it from the list and copy the rest of the buffer to the request.
 * The request is finished by calling request_end()
 */
<<<<<<< HEAD
static ssize_t fuse_dev_do_write(struct fuse_conn *fc,
				 struct fuse_copy_state *cs, size_t nbytes)
{
	int err;
=======
static ssize_t fuse_dev_do_write(struct fuse_dev *fud,
				 struct fuse_copy_state *cs, size_t nbytes)
{
	int err;
	struct fuse_conn *fc = fud->fc;
	struct fuse_pqueue *fpq = &fud->pq;
>>>>>>> v4.9.227
	struct fuse_req *req;
	struct fuse_out_header oh;

	if (nbytes < sizeof(struct fuse_out_header))
		return -EINVAL;

	err = fuse_copy_one(cs, &oh, sizeof(oh));
	if (err)
		goto err_finish;

	err = -EINVAL;
	if (oh.len != nbytes)
		goto err_finish;

	/*
	 * Zero oh.unique indicates unsolicited notification message
	 * and error contains notification code.
	 */
	if (!oh.unique) {
		err = fuse_notify(fc, oh.error, nbytes - sizeof(oh), cs);
		return err ? err : nbytes;
	}

	err = -EINVAL;
	if (oh.error <= -1000 || oh.error > 0)
		goto err_finish;

<<<<<<< HEAD
	spin_lock(&fc->lock);
	err = -ENOENT;
	if (!fc->connected)
		goto err_unlock;

	req = request_find(fc, oh.unique);
	if (!req)
		goto err_unlock;

	if (req->aborted) {
		spin_unlock(&fc->lock);
		fuse_copy_finish(cs);
		spin_lock(&fc->lock);
		request_end(fc, req);
		return -ENOENT;
	}
	/* Is it an interrupt reply? */
	if (req->intr_unique == oh.unique) {
		err = -EINVAL;
		if (nbytes != sizeof(struct fuse_out_header))
			goto err_unlock;
=======
	spin_lock(&fpq->lock);
	err = -ENOENT;
	if (!fpq->connected)
		goto err_unlock_pq;

	req = request_find(fpq, oh.unique);
	if (!req)
		goto err_unlock_pq;

	/* Is it an interrupt reply? */
	if (req->intr_unique == oh.unique) {
		__fuse_get_request(req);
		spin_unlock(&fpq->lock);

		err = -EINVAL;
		if (nbytes != sizeof(struct fuse_out_header)) {
			fuse_put_request(fc, req);
			goto err_finish;
		}
>>>>>>> v4.9.227

		if (oh.error == -ENOSYS)
			fc->no_interrupt = 1;
		else if (oh.error == -EAGAIN)
<<<<<<< HEAD
			queue_interrupt(fc, req);

		spin_unlock(&fc->lock);
=======
			queue_interrupt(&fc->iq, req);
		fuse_put_request(fc, req);

>>>>>>> v4.9.227
		fuse_copy_finish(cs);
		return nbytes;
	}

<<<<<<< HEAD
	req->state = FUSE_REQ_WRITING;
	list_move(&req->list, &fc->io);
	req->out.h = oh;
	req->locked = 1;
	cs->req = req;
	if (!req->out.page_replace)
		cs->move_pages = 0;
	spin_unlock(&fc->lock);

	err = copy_out_args(cs, &req->out, nbytes);
	if (req->in.h.opcode == FUSE_CANONICAL_PATH) {
		char *path = (char *)req->out.args[0].value;

		path[req->out.args[0].size - 1] = 0;
		req->out.h.error = kern_path(path, 0, req->canonical_path);
	}
	fuse_copy_finish(cs);

	fuse_setup_shortcircuit(fc, req);

	spin_lock(&fc->lock);
	req->locked = 0;
	if (!err) {
		if (req->aborted)
			err = -ENOENT;
	} else if (!req->aborted)
		req->out.h.error = -EIO;
=======
	clear_bit(FR_SENT, &req->flags);
	list_move(&req->list, &fpq->io);
	req->out.h = oh;
	set_bit(FR_LOCKED, &req->flags);
	spin_unlock(&fpq->lock);
	cs->req = req;
	if (!req->out.page_replace)
		cs->move_pages = 0;

	err = copy_out_args(cs, &req->out, nbytes);
	fuse_copy_finish(cs);

	spin_lock(&fpq->lock);
	clear_bit(FR_LOCKED, &req->flags);
	if (!fpq->connected)
		err = -ENOENT;
	else if (err)
		req->out.h.error = -EIO;
	if (!test_bit(FR_PRIVATE, &req->flags))
		list_del_init(&req->list);
	spin_unlock(&fpq->lock);

>>>>>>> v4.9.227
	request_end(fc, req);

	return err ? err : nbytes;

<<<<<<< HEAD
 err_unlock:
	spin_unlock(&fc->lock);
=======
 err_unlock_pq:
	spin_unlock(&fpq->lock);
>>>>>>> v4.9.227
 err_finish:
	fuse_copy_finish(cs);
	return err;
}

<<<<<<< HEAD
static ssize_t fuse_dev_write(struct kiocb *iocb, const struct iovec *iov,
			      unsigned long nr_segs, loff_t pos)
{
	struct fuse_copy_state cs;
	struct fuse_conn *fc = fuse_get_conn(iocb->ki_filp);
	if (!fc)
		return -EPERM;

	fuse_copy_init(&cs, fc, 0, iov, nr_segs);

	return fuse_dev_do_write(fc, &cs, iov_length(iov, nr_segs));
=======
static ssize_t fuse_dev_write(struct kiocb *iocb, struct iov_iter *from)
{
	struct fuse_copy_state cs;
	struct fuse_dev *fud = fuse_get_dev(iocb->ki_filp);

	if (!fud)
		return -EPERM;

	if (!iter_is_iovec(from))
		return -EINVAL;

	fuse_copy_init(&cs, 0, from);

	return fuse_dev_do_write(fud, &cs, iov_iter_count(from));
>>>>>>> v4.9.227
}

static ssize_t fuse_dev_splice_write(struct pipe_inode_info *pipe,
				     struct file *out, loff_t *ppos,
				     size_t len, unsigned int flags)
{
	unsigned nbuf;
	unsigned idx;
	struct pipe_buffer *bufs;
	struct fuse_copy_state cs;
<<<<<<< HEAD
	struct fuse_conn *fc;
	size_t rem;
	ssize_t ret;

	fc = fuse_get_conn(out);
	if (!fc)
=======
	struct fuse_dev *fud;
	size_t rem;
	ssize_t ret;

	fud = fuse_get_dev(out);
	if (!fud)
>>>>>>> v4.9.227
		return -EPERM;

	pipe_lock(pipe);

	bufs = kmalloc(pipe->buffers * sizeof(struct pipe_buffer), GFP_KERNEL);
	if (!bufs) {
		pipe_unlock(pipe);
		return -ENOMEM;
	}

	nbuf = 0;
	rem = 0;
	for (idx = 0; idx < pipe->nrbufs && rem < len; idx++)
		rem += pipe->bufs[(pipe->curbuf + idx) & (pipe->buffers - 1)].len;

	ret = -EINVAL;
<<<<<<< HEAD
	if (rem < len) {
		pipe_unlock(pipe);
		goto out;
	}
=======
	if (rem < len)
		goto out_free;
>>>>>>> v4.9.227

	rem = len;
	while (rem) {
		struct pipe_buffer *ibuf;
		struct pipe_buffer *obuf;

		BUG_ON(nbuf >= pipe->buffers);
		BUG_ON(!pipe->nrbufs);
		ibuf = &pipe->bufs[pipe->curbuf];
		obuf = &bufs[nbuf];

		if (rem >= ibuf->len) {
			*obuf = *ibuf;
			ibuf->ops = NULL;
			pipe->curbuf = (pipe->curbuf + 1) & (pipe->buffers - 1);
			pipe->nrbufs--;
		} else {
<<<<<<< HEAD
			ibuf->ops->get(pipe, ibuf);
=======
			if (!pipe_buf_get(pipe, ibuf))
				goto out_free;

>>>>>>> v4.9.227
			*obuf = *ibuf;
			obuf->flags &= ~PIPE_BUF_FLAG_GIFT;
			obuf->len = rem;
			ibuf->offset += obuf->len;
			ibuf->len -= obuf->len;
		}
		nbuf++;
		rem -= obuf->len;
	}
	pipe_unlock(pipe);

<<<<<<< HEAD
	fuse_copy_init(&cs, fc, 0, NULL, nbuf);
	cs.pipebufs = bufs;
=======
	fuse_copy_init(&cs, 0, NULL);
	cs.pipebufs = bufs;
	cs.nr_segs = nbuf;
>>>>>>> v4.9.227
	cs.pipe = pipe;

	if (flags & SPLICE_F_MOVE)
		cs.move_pages = 1;

<<<<<<< HEAD
	ret = fuse_dev_do_write(fc, &cs, len);

	for (idx = 0; idx < nbuf; idx++) {
		struct pipe_buffer *buf = &bufs[idx];
		buf->ops->release(pipe, buf);
	}
out:
=======
	ret = fuse_dev_do_write(fud, &cs, len);

	pipe_lock(pipe);
out_free:
	for (idx = 0; idx < nbuf; idx++)
		pipe_buf_release(pipe, &bufs[idx]);
	pipe_unlock(pipe);

>>>>>>> v4.9.227
	kfree(bufs);
	return ret;
}

static unsigned fuse_dev_poll(struct file *file, poll_table *wait)
{
	unsigned mask = POLLOUT | POLLWRNORM;
<<<<<<< HEAD
	struct fuse_conn *fc = fuse_get_conn(file);
	if (!fc)
		return POLLERR;

	poll_wait(file, &fc->waitq, wait);

	spin_lock(&fc->lock);
	if (!fc->connected)
		mask = POLLERR;
	else if (request_pending(fc))
		mask |= POLLIN | POLLRDNORM;
	spin_unlock(&fc->lock);
=======
	struct fuse_iqueue *fiq;
	struct fuse_dev *fud = fuse_get_dev(file);

	if (!fud)
		return POLLERR;

	fiq = &fud->fc->iq;
	poll_wait(file, &fiq->waitq, wait);

	spin_lock(&fiq->waitq.lock);
	if (!fiq->connected)
		mask = POLLERR;
	else if (request_pending(fiq))
		mask |= POLLIN | POLLRDNORM;
	spin_unlock(&fiq->waitq.lock);
>>>>>>> v4.9.227

	return mask;
}

/*
 * Abort all requests on the given list (pending or processing)
 *
 * This function releases and reacquires fc->lock
 */
static void end_requests(struct fuse_conn *fc, struct list_head *head)
<<<<<<< HEAD
__releases(fc->lock)
__acquires(fc->lock)
=======
>>>>>>> v4.9.227
{
	while (!list_empty(head)) {
		struct fuse_req *req;
		req = list_entry(head->next, struct fuse_req, list);
		req->out.h.error = -ECONNABORTED;
<<<<<<< HEAD
		request_end(fc, req);
		spin_lock(&fc->lock);
	}
}

/*
 * Abort requests under I/O
 *
 * The requests are set to aborted and finished, and the request
 * waiter is woken up.  This will make request_wait_answer() wait
 * until the request is unlocked and then return.
 *
 * If the request is asynchronous, then the end function needs to be
 * called after waiting for the request to be unlocked (if it was
 * locked).
 */
static void end_io_requests(struct fuse_conn *fc)
__releases(fc->lock)
__acquires(fc->lock)
{
	while (!list_empty(&fc->io)) {
		struct fuse_req *req =
			list_entry(fc->io.next, struct fuse_req, list);
		void (*end) (struct fuse_conn *, struct fuse_req *) = req->end;

		req->aborted = 1;
		req->out.h.error = -ECONNABORTED;
		req->state = FUSE_REQ_FINISHED;
		list_del_init(&req->list);
		wake_up(&req->waitq);
		if (end) {
			req->end = NULL;
			__fuse_get_request(req);
			spin_unlock(&fc->lock);
			wait_event(req->waitq, !req->locked);
			end(fc, req);
			fuse_put_request(fc, req);
			spin_lock(&fc->lock);
		}
	}
}

static void end_queued_requests(struct fuse_conn *fc)
__releases(fc->lock)
__acquires(fc->lock)
{
	fc->max_background = UINT_MAX;
	flush_bg_queue(fc);
	end_requests(fc, &fc->pending);
	end_requests(fc, &fc->processing);
	while (forget_pending(fc))
		kfree(dequeue_forget(fc, 1, NULL));
}

=======
		clear_bit(FR_SENT, &req->flags);
		list_del_init(&req->list);
		request_end(fc, req);
	}
}

>>>>>>> v4.9.227
static void end_polls(struct fuse_conn *fc)
{
	struct rb_node *p;

	p = rb_first(&fc->polled_files);

	while (p) {
		struct fuse_file *ff;
		ff = rb_entry(p, struct fuse_file, polled_node);
		wake_up_interruptible_all(&ff->poll_wait);

		p = rb_next(p);
	}
}

/*
 * Abort all requests.
 *
<<<<<<< HEAD
 * Emergency exit in case of a malicious or accidental deadlock, or
 * just a hung filesystem.
 *
 * The same effect is usually achievable through killing the
 * filesystem daemon and all users of the filesystem.  The exception
 * is the combination of an asynchronous request and the tricky
 * deadlock (see Documentation/filesystems/fuse.txt).
 *
 * During the aborting, progression of requests from the pending and
 * processing lists onto the io list, and progression of new requests
 * onto the pending list is prevented by req->connected being false.
 *
 * Progression of requests under I/O to the processing list is
 * prevented by the req->aborted flag being true for these requests.
 * For this reason requests on the io list must be aborted first.
 */
void fuse_abort_conn(struct fuse_conn *fc)
{
	spin_lock(&fc->lock);
	if (fc->connected) {
		fc->connected = 0;
		fc->blocked = 0;
		fc->initialized = 1;
		end_io_requests(fc);
		end_queued_requests(fc);
		end_polls(fc);
		wake_up_all(&fc->waitq);
		wake_up_all(&fc->blocked_waitq);
		kill_fasync(&fc->fasync, SIGIO, POLL_IN);
	}
	spin_unlock(&fc->lock);
}
EXPORT_SYMBOL_GPL(fuse_abort_conn);

int fuse_dev_release(struct inode *inode, struct file *file)
{
	struct fuse_conn *fc = fuse_get_conn(file);
	if (fc) {
		spin_lock(&fc->lock);
		fc->connected = 0;
		fc->blocked = 0;
		fc->initialized = 1;
		end_queued_requests(fc);
		end_polls(fc);
		wake_up_all(&fc->blocked_waitq);
		spin_unlock(&fc->lock);
		fuse_conn_put(fc);
	}

=======
 * Emergency exit in case of a malicious or accidental deadlock, or just a hung
 * filesystem.
 *
 * The same effect is usually achievable through killing the filesystem daemon
 * and all users of the filesystem.  The exception is the combination of an
 * asynchronous request and the tricky deadlock (see
 * Documentation/filesystems/fuse.txt).
 *
 * Aborting requests under I/O goes as follows: 1: Separate out unlocked
 * requests, they should be finished off immediately.  Locked requests will be
 * finished after unlock; see unlock_request(). 2: Finish off the unlocked
 * requests.  It is possible that some request will finish before we can.  This
 * is OK, the request will in that case be removed from the list before we touch
 * it.
 */
void fuse_abort_conn(struct fuse_conn *fc)
{
	struct fuse_iqueue *fiq = &fc->iq;

	spin_lock(&fc->lock);
	if (fc->connected) {
		struct fuse_dev *fud;
		struct fuse_req *req, *next;
		LIST_HEAD(to_end1);
		LIST_HEAD(to_end2);

		fc->connected = 0;
		fc->blocked = 0;
		fuse_set_initialized(fc);
		list_for_each_entry(fud, &fc->devices, entry) {
			struct fuse_pqueue *fpq = &fud->pq;

			spin_lock(&fpq->lock);
			fpq->connected = 0;
			list_for_each_entry_safe(req, next, &fpq->io, list) {
				req->out.h.error = -ECONNABORTED;
				spin_lock(&req->waitq.lock);
				set_bit(FR_ABORTED, &req->flags);
				if (!test_bit(FR_LOCKED, &req->flags)) {
					set_bit(FR_PRIVATE, &req->flags);
					__fuse_get_request(req);
					list_move(&req->list, &to_end1);
				}
				spin_unlock(&req->waitq.lock);
			}
			list_splice_init(&fpq->processing, &to_end2);
			spin_unlock(&fpq->lock);
		}
		fc->max_background = UINT_MAX;
		flush_bg_queue(fc);

		spin_lock(&fiq->waitq.lock);
		fiq->connected = 0;
		list_splice_init(&fiq->pending, &to_end2);
		list_for_each_entry(req, &to_end2, list)
			clear_bit(FR_PENDING, &req->flags);
		while (forget_pending(fiq))
			kfree(dequeue_forget(fiq, 1, NULL));
		wake_up_all_locked(&fiq->waitq);
		spin_unlock(&fiq->waitq.lock);
		kill_fasync(&fiq->fasync, SIGIO, POLL_IN);
		end_polls(fc);
		wake_up_all(&fc->blocked_waitq);
		spin_unlock(&fc->lock);

		while (!list_empty(&to_end1)) {
			req = list_first_entry(&to_end1, struct fuse_req, list);
			list_del_init(&req->list);
			request_end(fc, req);
		}
		end_requests(fc, &to_end2);
	} else {
		spin_unlock(&fc->lock);
	}
}
EXPORT_SYMBOL_GPL(fuse_abort_conn);

void fuse_wait_aborted(struct fuse_conn *fc)
{
	/* matches implicit memory barrier in fuse_drop_waiting() */
	smp_mb();
	wait_event(fc->blocked_waitq, atomic_read(&fc->num_waiting) == 0);
}

int fuse_dev_release(struct inode *inode, struct file *file)
{
	struct fuse_dev *fud = fuse_get_dev(file);

	if (fud) {
		struct fuse_conn *fc = fud->fc;
		struct fuse_pqueue *fpq = &fud->pq;
		LIST_HEAD(to_end);

		spin_lock(&fpq->lock);
		WARN_ON(!list_empty(&fpq->io));
		list_splice_init(&fpq->processing, &to_end);
		spin_unlock(&fpq->lock);

		end_requests(fc, &to_end);

		/* Are we the last open device? */
		if (atomic_dec_and_test(&fc->dev_count)) {
			WARN_ON(fc->iq.fasync != NULL);
			fuse_abort_conn(fc);
		}
		fuse_dev_free(fud);
	}
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL_GPL(fuse_dev_release);

static int fuse_dev_fasync(int fd, struct file *file, int on)
{
<<<<<<< HEAD
	struct fuse_conn *fc = fuse_get_conn(file);
	if (!fc)
		return -EPERM;

	/* No locking - fasync_helper does its own locking */
	return fasync_helper(fd, file, on, &fc->fasync);
=======
	struct fuse_dev *fud = fuse_get_dev(file);

	if (!fud)
		return -EPERM;

	/* No locking - fasync_helper does its own locking */
	return fasync_helper(fd, file, on, &fud->fc->iq.fasync);
}

static int fuse_device_clone(struct fuse_conn *fc, struct file *new)
{
	struct fuse_dev *fud;

	if (new->private_data)
		return -EINVAL;

	fud = fuse_dev_alloc(fc);
	if (!fud)
		return -ENOMEM;

	new->private_data = fud;
	atomic_inc(&fc->dev_count);

	return 0;
}

static long fuse_dev_ioctl(struct file *file, unsigned int cmd,
			   unsigned long arg)
{
	int err = -ENOTTY;

	if (cmd == FUSE_DEV_IOC_CLONE) {
		int oldfd;

		err = -EFAULT;
		if (!get_user(oldfd, (__u32 __user *) arg)) {
			struct file *old = fget(oldfd);

			err = -EINVAL;
			if (old) {
				struct fuse_dev *fud = NULL;

				/*
				 * Check against file->f_op because CUSE
				 * uses the same ioctl handler.
				 */
				if (old->f_op == file->f_op &&
				    old->f_cred->user_ns == file->f_cred->user_ns)
					fud = fuse_get_dev(old);

				if (fud) {
					mutex_lock(&fuse_mutex);
					err = fuse_device_clone(fud->fc, file);
					mutex_unlock(&fuse_mutex);
				}
				fput(old);
			}
		}
	}
	return err;
>>>>>>> v4.9.227
}

const struct file_operations fuse_dev_operations = {
	.owner		= THIS_MODULE,
<<<<<<< HEAD
	.llseek		= no_llseek,
	.read		= do_sync_read,
	.aio_read	= fuse_dev_read,
	.splice_read	= fuse_dev_splice_read,
	.write		= do_sync_write,
	.aio_write	= fuse_dev_write,
=======
	.open		= fuse_dev_open,
	.llseek		= no_llseek,
	.read_iter	= fuse_dev_read,
	.splice_read	= fuse_dev_splice_read,
	.write_iter	= fuse_dev_write,
>>>>>>> v4.9.227
	.splice_write	= fuse_dev_splice_write,
	.poll		= fuse_dev_poll,
	.release	= fuse_dev_release,
	.fasync		= fuse_dev_fasync,
<<<<<<< HEAD
=======
	.unlocked_ioctl = fuse_dev_ioctl,
	.compat_ioctl   = fuse_dev_ioctl,
>>>>>>> v4.9.227
};
EXPORT_SYMBOL_GPL(fuse_dev_operations);

static struct miscdevice fuse_miscdevice = {
	.minor = FUSE_MINOR,
	.name  = "fuse",
	.fops = &fuse_dev_operations,
};

int __init fuse_dev_init(void)
{
	int err = -ENOMEM;
	fuse_req_cachep = kmem_cache_create("fuse_request",
					    sizeof(struct fuse_req),
					    0, 0, NULL);
	if (!fuse_req_cachep)
		goto out;

	err = misc_register(&fuse_miscdevice);
	if (err)
		goto out_cache_clean;

	return 0;

 out_cache_clean:
	kmem_cache_destroy(fuse_req_cachep);
 out:
	return err;
}

void fuse_dev_cleanup(void)
{
	misc_deregister(&fuse_miscdevice);
	kmem_cache_destroy(fuse_req_cachep);
}
