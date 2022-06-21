/*
 * Network block device - make block devices work over TCP
 *
 * Note that you can not swap over this thing, yet. Seems to work but
 * deadlocks sometimes - you can not swap over TCP in general.
 * 
 * Copyright 1997-2000, 2008 Pavel Machek <pavel@ucw.cz>
 * Parts copyright 2001 Steven Whitehouse <steve@chygwyn.com>
 *
 * This file is released under GPLv2 or later.
 *
 * (part of code stolen from loop.c)
 */

#include <linux/major.h>

#include <linux/blkdev.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/bio.h>
#include <linux/stat.h>
#include <linux/errno.h>
#include <linux/file.h>
#include <linux/ioctl.h>
#include <linux/mutex.h>
#include <linux/compiler.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <net/sock.h>
#include <linux/net.h>
#include <linux/kthread.h>
<<<<<<< HEAD
=======
#include <linux/types.h>
#include <linux/debugfs.h>
#include <linux/blk-mq.h>
>>>>>>> v4.9.227

#include <asm/uaccess.h>
#include <asm/types.h>

#include <linux/nbd.h>

<<<<<<< HEAD
#define NBD_MAGIC 0x68797548

#ifdef NDEBUG
#define dprintk(flags, fmt...)
#else /* NDEBUG */
#define dprintk(flags, fmt...) do { \
	if (debugflags & (flags)) printk(KERN_DEBUG fmt); \
} while (0)
#define DBG_IOCTL       0x0004
#define DBG_INIT        0x0010
#define DBG_EXIT        0x0020
#define DBG_BLKDEV      0x0100
#define DBG_RX          0x0200
#define DBG_TX          0x0400
static unsigned int debugflags;
#endif /* NDEBUG */
=======
#define NBD_TIMEDOUT			0
#define NBD_DISCONNECT_REQUESTED	1

struct nbd_device {
	u32 flags;
	unsigned long runtime_flags;
	struct socket * sock;	/* If == NULL, device is not ready, yet	*/
	int magic;

	struct blk_mq_tag_set tag_set;

	struct mutex tx_lock;
	struct gendisk *disk;
	loff_t blksize;
	loff_t bytesize;

	/* protects initialization and shutdown of the socket */
	spinlock_t sock_lock;
	struct task_struct *task_recv;
	struct task_struct *task_send;

#if IS_ENABLED(CONFIG_DEBUG_FS)
	struct dentry *dbg_dir;
#endif
};

struct nbd_cmd {
	struct nbd_device *nbd;
	struct list_head list;
};

#if IS_ENABLED(CONFIG_DEBUG_FS)
static struct dentry *nbd_dbg_dir;
#endif

#define nbd_name(nbd) ((nbd)->disk->disk_name)

#define NBD_MAGIC 0x68797548
>>>>>>> v4.9.227

static unsigned int nbds_max = 16;
static struct nbd_device *nbd_dev;
static int max_part;

<<<<<<< HEAD
/*
 * Use just one lock (or at most 1 per NIC). Two arguments for this:
 * 1. Each NIC is essentially a synchronization point for all servers
 *    accessed through that NIC so there's no need to have more locks
 *    than NICs anyway.
 * 2. More locks lead to more "Dirty cache line bouncing" which will slow
 *    down each lock to the point where they're actually slower than just
 *    a single lock.
 * Thanks go to Jens Axboe and Al Viro for their LKML emails explaining this!
 */
static DEFINE_SPINLOCK(nbd_lock);

#ifndef NDEBUG
static const char *ioctl_cmd_to_ascii(int cmd)
{
	switch (cmd) {
	case NBD_SET_SOCK: return "set-sock";
	case NBD_SET_BLKSIZE: return "set-blksize";
	case NBD_SET_SIZE: return "set-size";
	case NBD_SET_TIMEOUT: return "set-timeout";
	case NBD_SET_FLAGS: return "set-flags";
	case NBD_DO_IT: return "do-it";
	case NBD_CLEAR_SOCK: return "clear-sock";
	case NBD_CLEAR_QUE: return "clear-que";
	case NBD_PRINT_DEBUG: return "print-debug";
	case NBD_SET_SIZE_BLOCKS: return "set-size-blocks";
	case NBD_DISCONNECT: return "disconnect";
	case BLKROSET: return "set-read-only";
	case BLKFLSBUF: return "flush-buffer-cache";
	}
	return "unknown";
=======
static inline struct device *nbd_to_dev(struct nbd_device *nbd)
{
	return disk_to_dev(nbd->disk);
}

static bool nbd_is_connected(struct nbd_device *nbd)
{
	return !!nbd->task_recv;
>>>>>>> v4.9.227
}

static const char *nbdcmd_to_ascii(int cmd)
{
	switch (cmd) {
	case  NBD_CMD_READ: return "read";
	case NBD_CMD_WRITE: return "write";
	case  NBD_CMD_DISC: return "disconnect";
	case NBD_CMD_FLUSH: return "flush";
	case  NBD_CMD_TRIM: return "trim/discard";
	}
	return "invalid";
}
<<<<<<< HEAD
#endif /* NDEBUG */

static void nbd_end_request(struct request *req)
{
	int error = req->errors ? -EIO : 0;
	struct request_queue *q = req->q;
	unsigned long flags;

	dprintk(DBG_BLKDEV, "%s: request %p: %s\n", req->rq_disk->disk_name,
			req, error ? "failed" : "done");

	spin_lock_irqsave(q->queue_lock, flags);
	__blk_end_request_all(req, error);
	spin_unlock_irqrestore(q->queue_lock, flags);
}

static void sock_shutdown(struct nbd_device *nbd, int lock)
{
	/* Forcibly shutdown the socket causing all listeners
	 * to error
	 *
	 * FIXME: This code is duplicated from sys_shutdown, but
	 * there should be a more generic interface rather than
	 * calling socket ops directly here */
	if (lock)
		mutex_lock(&nbd->tx_lock);
	if (nbd->sock) {
		dev_warn(disk_to_dev(nbd->disk), "shutting down socket\n");
		kernel_sock_shutdown(nbd->sock, SHUT_RDWR);
		nbd->sock = NULL;
	}
	if (lock)
		mutex_unlock(&nbd->tx_lock);
}

static void nbd_xmit_timeout(unsigned long arg)
{
	struct task_struct *task = (struct task_struct *)arg;

	printk(KERN_WARNING "nbd: killing hung xmit (%s, pid: %d)\n",
		task->comm, task->pid);
	force_sig(SIGKILL, task);
=======

static int nbd_size_clear(struct nbd_device *nbd, struct block_device *bdev)
{
	bd_set_size(bdev, 0);
	set_capacity(nbd->disk, 0);
	kobject_uevent(&nbd_to_dev(nbd)->kobj, KOBJ_CHANGE);

	return 0;
}

static void nbd_size_update(struct nbd_device *nbd, struct block_device *bdev)
{
	blk_queue_logical_block_size(nbd->disk->queue, nbd->blksize);
	blk_queue_physical_block_size(nbd->disk->queue, nbd->blksize);
	bd_set_size(bdev, nbd->bytesize);
	set_blocksize(bdev, nbd->blksize);
	set_capacity(nbd->disk, nbd->bytesize >> 9);
	kobject_uevent(&nbd_to_dev(nbd)->kobj, KOBJ_CHANGE);
}

static void nbd_size_set(struct nbd_device *nbd, struct block_device *bdev,
			loff_t blocksize, loff_t nr_blocks)
{
	nbd->blksize = blocksize;
	nbd->bytesize = blocksize * nr_blocks;
	if (nbd_is_connected(nbd))
		nbd_size_update(nbd, bdev);
}

static void nbd_end_request(struct nbd_cmd *cmd)
{
	struct nbd_device *nbd = cmd->nbd;
	struct request *req = blk_mq_rq_from_pdu(cmd);
	int error = req->errors ? -EIO : 0;

	dev_dbg(nbd_to_dev(nbd), "request %p: %s\n", cmd,
		error ? "failed" : "done");

	blk_mq_complete_request(req, error);
}

/*
 * Forcibly shutdown the socket causing all listeners to error
 */
static void sock_shutdown(struct nbd_device *nbd)
{
	struct socket *sock;

	spin_lock(&nbd->sock_lock);

	if (!nbd->sock) {
		spin_unlock(&nbd->sock_lock);
		return;
	}

	sock = nbd->sock;
	dev_warn(disk_to_dev(nbd->disk), "shutting down socket\n");
	nbd->sock = NULL;
	spin_unlock(&nbd->sock_lock);

	kernel_sock_shutdown(sock, SHUT_RDWR);
	sockfd_put(sock);
}

static enum blk_eh_timer_return nbd_xmit_timeout(struct request *req,
						 bool reserved)
{
	struct nbd_cmd *cmd = blk_mq_rq_to_pdu(req);
	struct nbd_device *nbd = cmd->nbd;
	struct socket *sock = NULL;

	spin_lock(&nbd->sock_lock);

	set_bit(NBD_TIMEDOUT, &nbd->runtime_flags);

	if (nbd->sock) {
		sock = nbd->sock;
		get_file(sock->file);
	}

	spin_unlock(&nbd->sock_lock);
	if (sock) {
		kernel_sock_shutdown(sock, SHUT_RDWR);
		sockfd_put(sock);
	}

	req->errors++;
	dev_err(nbd_to_dev(nbd), "Connection timed out, shutting down connection\n");
	return BLK_EH_HANDLED;
>>>>>>> v4.9.227
}

/*
 *  Send or receive packet.
 */
static int sock_xmit(struct nbd_device *nbd, int send, void *buf, int size,
		int msg_flags)
{
	struct socket *sock = nbd->sock;
	int result;
	struct msghdr msg;
	struct kvec iov;
<<<<<<< HEAD
	sigset_t blocked, oldset;
=======
>>>>>>> v4.9.227
	unsigned long pflags = current->flags;

	if (unlikely(!sock)) {
		dev_err(disk_to_dev(nbd->disk),
			"Attempted %s on closed socket in sock_xmit\n",
			(send ? "send" : "recv"));
		return -EINVAL;
	}

<<<<<<< HEAD
	/* Allow interception of SIGKILL only
	 * Don't allow other signals to interrupt the transmission */
	siginitsetinv(&blocked, sigmask(SIGKILL));
	sigprocmask(SIG_SETMASK, &blocked, &oldset);

=======
>>>>>>> v4.9.227
	current->flags |= PF_MEMALLOC;
	do {
		sock->sk->sk_allocation = GFP_NOIO | __GFP_MEMALLOC;
		iov.iov_base = buf;
		iov.iov_len = size;
		msg.msg_name = NULL;
		msg.msg_namelen = 0;
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		msg.msg_flags = msg_flags | MSG_NOSIGNAL;

<<<<<<< HEAD
		if (send) {
			struct timer_list ti;

			if (nbd->xmit_timeout) {
				init_timer(&ti);
				ti.function = nbd_xmit_timeout;
				ti.data = (unsigned long)current;
				ti.expires = jiffies + nbd->xmit_timeout;
				add_timer(&ti);
			}
			result = kernel_sendmsg(sock, &msg, &iov, 1, size);
			if (nbd->xmit_timeout)
				del_timer_sync(&ti);
		} else
			result = kernel_recvmsg(sock, &msg, &iov, 1, size,
						msg.msg_flags);

		if (signal_pending(current)) {
			siginfo_t info;
			printk(KERN_WARNING "nbd (pid %d: %s) got signal %d\n",
				task_pid_nr(current), current->comm,
				dequeue_signal_lock(current, &current->blocked, &info));
			result = -EINTR;
			sock_shutdown(nbd, !send);
			break;
		}

=======
		if (send)
			result = kernel_sendmsg(sock, &msg, &iov, 1, size);
		else
			result = kernel_recvmsg(sock, &msg, &iov, 1, size,
						msg.msg_flags);

>>>>>>> v4.9.227
		if (result <= 0) {
			if (result == 0)
				result = -EPIPE; /* short read */
			break;
		}
		size -= result;
		buf += result;
	} while (size > 0);

<<<<<<< HEAD
	sigprocmask(SIG_SETMASK, &oldset, NULL);
=======
>>>>>>> v4.9.227
	tsk_restore_flags(current, pflags, PF_MEMALLOC);

	return result;
}

static inline int sock_send_bvec(struct nbd_device *nbd, struct bio_vec *bvec,
		int flags)
{
	int result;
	void *kaddr = kmap(bvec->bv_page);
	result = sock_xmit(nbd, 1, kaddr + bvec->bv_offset,
			   bvec->bv_len, flags);
	kunmap(bvec->bv_page);
	return result;
}

/* always call with the tx_lock held */
<<<<<<< HEAD
static int nbd_send_req(struct nbd_device *nbd, struct request *req)
{
	int result, flags;
	struct nbd_request request;
	unsigned long size = blk_rq_bytes(req);

	memset(&request, 0, sizeof(request));
	request.magic = htonl(NBD_REQUEST_MAGIC);
	request.type = htonl(nbd_cmd(req));

	if (nbd_cmd(req) != NBD_CMD_FLUSH && nbd_cmd(req) != NBD_CMD_DISC) {
		request.from = cpu_to_be64((u64)blk_rq_pos(req) << 9);
		request.len = htonl(size);
	}
	memcpy(request.handle, &req, sizeof(req));

	dprintk(DBG_TX, "%s: request %p: sending control (%s@%llu,%uB)\n",
			nbd->disk->disk_name, req,
			nbdcmd_to_ascii(nbd_cmd(req)),
			(unsigned long long)blk_rq_pos(req) << 9,
			blk_rq_bytes(req));
	result = sock_xmit(nbd, 1, &request, sizeof(request),
			(nbd_cmd(req) == NBD_CMD_WRITE) ? MSG_MORE : 0);
	if (result <= 0) {
		dev_err(disk_to_dev(nbd->disk),
			"Send control failed (result %d)\n", result);
		goto error_out;
	}

	if (nbd_cmd(req) == NBD_CMD_WRITE) {
		struct req_iterator iter;
		struct bio_vec bvec;
		/*
		 * we are really probing at internals to determine
		 * whether to set MSG_MORE or not...
		 */
		rq_for_each_segment(bvec, req, iter) {
			flags = 0;
			if (!rq_iter_last(bvec, iter))
				flags = MSG_MORE;
			dprintk(DBG_TX, "%s: request %p: sending %d bytes data\n",
					nbd->disk->disk_name, req, bvec.bv_len);
=======
static int nbd_send_cmd(struct nbd_device *nbd, struct nbd_cmd *cmd)
{
	struct request *req = blk_mq_rq_from_pdu(cmd);
	int result;
	struct nbd_request request;
	unsigned long size = blk_rq_bytes(req);
	struct bio *bio;
	u32 type;

	if (req->cmd_type == REQ_TYPE_DRV_PRIV)
		type = NBD_CMD_DISC;
	else if (req_op(req) == REQ_OP_DISCARD)
		type = NBD_CMD_TRIM;
	else if (req_op(req) == REQ_OP_FLUSH)
		type = NBD_CMD_FLUSH;
	else if (rq_data_dir(req) == WRITE)
		type = NBD_CMD_WRITE;
	else
		type = NBD_CMD_READ;

	memset(&request, 0, sizeof(request));
	request.magic = htonl(NBD_REQUEST_MAGIC);
	request.type = htonl(type);
	if (type != NBD_CMD_FLUSH && type != NBD_CMD_DISC) {
		request.from = cpu_to_be64((u64)blk_rq_pos(req) << 9);
		request.len = htonl(size);
	}
	memcpy(request.handle, &req->tag, sizeof(req->tag));

	dev_dbg(nbd_to_dev(nbd), "request %p: sending control (%s@%llu,%uB)\n",
		cmd, nbdcmd_to_ascii(type),
		(unsigned long long)blk_rq_pos(req) << 9, blk_rq_bytes(req));
	result = sock_xmit(nbd, 1, &request, sizeof(request),
			(type == NBD_CMD_WRITE) ? MSG_MORE : 0);
	if (result <= 0) {
		dev_err(disk_to_dev(nbd->disk),
			"Send control failed (result %d)\n", result);
		return -EIO;
	}

	if (type != NBD_CMD_WRITE)
		return 0;

	bio = req->bio;
	while (bio) {
		struct bio *next = bio->bi_next;
		struct bvec_iter iter;
		struct bio_vec bvec;

		bio_for_each_segment(bvec, bio, iter) {
			bool is_last = !next && bio_iter_last(bvec, iter);
			int flags = is_last ? 0 : MSG_MORE;

			dev_dbg(nbd_to_dev(nbd), "request %p: sending %d bytes data\n",
				cmd, bvec.bv_len);
>>>>>>> v4.9.227
			result = sock_send_bvec(nbd, &bvec, flags);
			if (result <= 0) {
				dev_err(disk_to_dev(nbd->disk),
					"Send data failed (result %d)\n",
					result);
<<<<<<< HEAD
				goto error_out;
			}
		}
	}
	return 0;

error_out:
	return -EIO;
}

static struct request *nbd_find_request(struct nbd_device *nbd,
					struct request *xreq)
{
	struct request *req, *tmp;
	int err;

	err = wait_event_interruptible(nbd->active_wq, nbd->active_req != xreq);
	if (unlikely(err))
		goto out;

	spin_lock(&nbd->queue_lock);
	list_for_each_entry_safe(req, tmp, &nbd->queue_head, queuelist) {
		if (req != xreq)
			continue;
		list_del_init(&req->queuelist);
		spin_unlock(&nbd->queue_lock);
		return req;
	}
	spin_unlock(&nbd->queue_lock);

	err = -ENOENT;

out:
	return ERR_PTR(err);
=======
				return -EIO;
			}
			/*
			 * The completion might already have come in,
			 * so break for the last one instead of letting
			 * the iterator do it. This prevents use-after-free
			 * of the bio.
			 */
			if (is_last)
				break;
		}
		bio = next;
	}
	return 0;
>>>>>>> v4.9.227
}

static inline int sock_recv_bvec(struct nbd_device *nbd, struct bio_vec *bvec)
{
	int result;
	void *kaddr = kmap(bvec->bv_page);
	result = sock_xmit(nbd, 0, kaddr + bvec->bv_offset, bvec->bv_len,
			MSG_WAITALL);
	kunmap(bvec->bv_page);
	return result;
}

/* NULL returned = something went wrong, inform userspace */
<<<<<<< HEAD
static struct request *nbd_read_stat(struct nbd_device *nbd)
{
	int result;
	struct nbd_reply reply;
	struct request *req;
=======
static struct nbd_cmd *nbd_read_stat(struct nbd_device *nbd)
{
	int result;
	struct nbd_reply reply;
	struct nbd_cmd *cmd;
	struct request *req = NULL;
	u16 hwq;
	int tag;
>>>>>>> v4.9.227

	reply.magic = 0;
	result = sock_xmit(nbd, 0, &reply, sizeof(reply), MSG_WAITALL);
	if (result <= 0) {
		dev_err(disk_to_dev(nbd->disk),
			"Receive control failed (result %d)\n", result);
<<<<<<< HEAD
		goto harderror;
=======
		return ERR_PTR(result);
>>>>>>> v4.9.227
	}

	if (ntohl(reply.magic) != NBD_REPLY_MAGIC) {
		dev_err(disk_to_dev(nbd->disk), "Wrong magic (0x%lx)\n",
				(unsigned long)ntohl(reply.magic));
<<<<<<< HEAD
		result = -EPROTO;
		goto harderror;
	}

	req = nbd_find_request(nbd, *(struct request **)reply.handle);
	if (IS_ERR(req)) {
		result = PTR_ERR(req);
		if (result != -ENOENT)
			goto harderror;

		dev_err(disk_to_dev(nbd->disk), "Unexpected reply (%p)\n",
			reply.handle);
		result = -EBADR;
		goto harderror;
	}
=======
		return ERR_PTR(-EPROTO);
	}

	memcpy(&tag, reply.handle, sizeof(int));

	hwq = blk_mq_unique_tag_to_hwq(tag);
	if (hwq < nbd->tag_set.nr_hw_queues)
		req = blk_mq_tag_to_rq(nbd->tag_set.tags[hwq],
				       blk_mq_unique_tag_to_tag(tag));
	if (!req || !blk_mq_request_started(req)) {
		dev_err(disk_to_dev(nbd->disk), "Unexpected reply (%d) %p\n",
			tag, req);
		return ERR_PTR(-ENOENT);
	}
	cmd = blk_mq_rq_to_pdu(req);
>>>>>>> v4.9.227

	if (ntohl(reply.error)) {
		dev_err(disk_to_dev(nbd->disk), "Other side returned error (%d)\n",
			ntohl(reply.error));
		req->errors++;
<<<<<<< HEAD
		return req;
	}

	dprintk(DBG_RX, "%s: request %p: got reply\n",
			nbd->disk->disk_name, req);
	if (nbd_cmd(req) == NBD_CMD_READ) {
=======
		return cmd;
	}

	dev_dbg(nbd_to_dev(nbd), "request %p: got reply\n", cmd);
	if (rq_data_dir(req) != WRITE) {
>>>>>>> v4.9.227
		struct req_iterator iter;
		struct bio_vec bvec;

		rq_for_each_segment(bvec, req, iter) {
			result = sock_recv_bvec(nbd, &bvec);
			if (result <= 0) {
				dev_err(disk_to_dev(nbd->disk), "Receive data failed (result %d)\n",
					result);
				req->errors++;
<<<<<<< HEAD
				return req;
			}
			dprintk(DBG_RX, "%s: request %p: got %d bytes data\n",
				nbd->disk->disk_name, req, bvec.bv_len);
		}
	}
	return req;
harderror:
	nbd->harderror = result;
	return NULL;
=======
				return cmd;
			}
			dev_dbg(nbd_to_dev(nbd), "request %p: got %d bytes data\n",
				cmd, bvec.bv_len);
		}
	}
	return cmd;
>>>>>>> v4.9.227
}

static ssize_t pid_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct gendisk *disk = dev_to_disk(dev);
<<<<<<< HEAD

	return sprintf(buf, "%ld\n",
		(long) ((struct nbd_device *)disk->private_data)->pid);
=======
	struct nbd_device *nbd = (struct nbd_device *)disk->private_data;

	return sprintf(buf, "%d\n", task_pid_nr(nbd->task_recv));
>>>>>>> v4.9.227
}

static struct device_attribute pid_attr = {
	.attr = { .name = "pid", .mode = S_IRUGO},
	.show = pid_show,
};

<<<<<<< HEAD
static int nbd_do_it(struct nbd_device *nbd)
{
	struct request *req;
=======
static int nbd_thread_recv(struct nbd_device *nbd, struct block_device *bdev)
{
	struct nbd_cmd *cmd;
>>>>>>> v4.9.227
	int ret;

	BUG_ON(nbd->magic != NBD_MAGIC);

	sk_set_memalloc(nbd->sock->sk);
<<<<<<< HEAD
	nbd->pid = task_pid_nr(current);
	ret = device_create_file(disk_to_dev(nbd->disk), &pid_attr);
	if (ret) {
		dev_err(disk_to_dev(nbd->disk), "device_create_file failed!\n");
		nbd->pid = 0;
		return ret;
	}

	while ((req = nbd_read_stat(nbd)) != NULL)
		nbd_end_request(req);

	device_remove_file(disk_to_dev(nbd->disk), &pid_attr);
	nbd->pid = 0;
	return 0;
=======

	ret = device_create_file(disk_to_dev(nbd->disk), &pid_attr);
	if (ret) {
		dev_err(disk_to_dev(nbd->disk), "device_create_file failed!\n");
		return ret;
	}

	nbd_size_update(nbd, bdev);

	while (1) {
		cmd = nbd_read_stat(nbd);
		if (IS_ERR(cmd)) {
			ret = PTR_ERR(cmd);
			break;
		}

		nbd_end_request(cmd);
	}

	nbd_size_clear(nbd, bdev);

	device_remove_file(disk_to_dev(nbd->disk), &pid_attr);
	return ret;
}

static void nbd_clear_req(struct request *req, void *data, bool reserved)
{
	struct nbd_cmd *cmd;

	if (!blk_mq_request_started(req))
		return;
	cmd = blk_mq_rq_to_pdu(req);
	req->errors++;
	nbd_end_request(cmd);
>>>>>>> v4.9.227
}

static void nbd_clear_que(struct nbd_device *nbd)
{
<<<<<<< HEAD
	struct request *req;

=======
>>>>>>> v4.9.227
	BUG_ON(nbd->magic != NBD_MAGIC);

	/*
	 * Because we have set nbd->sock to NULL under the tx_lock, all
<<<<<<< HEAD
	 * modifications to the list must have completed by now.  For
	 * the same reason, the active_req must be NULL.
	 *
	 * As a consequence, we don't need to take the spin lock while
	 * purging the list here.
	 */
	BUG_ON(nbd->sock);
	BUG_ON(nbd->active_req);

	while (!list_empty(&nbd->queue_head)) {
		req = list_entry(nbd->queue_head.next, struct request,
				 queuelist);
		list_del_init(&req->queuelist);
		req->errors++;
		nbd_end_request(req);
	}

	while (!list_empty(&nbd->waiting_queue)) {
		req = list_entry(nbd->waiting_queue.next, struct request,
				 queuelist);
		list_del_init(&req->queuelist);
		req->errors++;
		nbd_end_request(req);
	}
}


static void nbd_handle_req(struct nbd_device *nbd, struct request *req)
{
	if (req->cmd_type != REQ_TYPE_FS)
		goto error_out;

	nbd_cmd(req) = NBD_CMD_READ;
	if (rq_data_dir(req) == WRITE) {
		if ((req->cmd_flags & REQ_DISCARD)) {
			WARN_ON(!(nbd->flags & NBD_FLAG_SEND_TRIM));
			nbd_cmd(req) = NBD_CMD_TRIM;
		} else
			nbd_cmd(req) = NBD_CMD_WRITE;
		if (nbd->flags & NBD_FLAG_READ_ONLY) {
			dev_err(disk_to_dev(nbd->disk),
				"Write on read-only\n");
			goto error_out;
		}
	}

	if (req->cmd_flags & REQ_FLUSH) {
		BUG_ON(unlikely(blk_rq_sectors(req)));
		nbd_cmd(req) = NBD_CMD_FLUSH;
=======
	 * modifications to the list must have completed by now.
	 */
	BUG_ON(nbd->sock);

	blk_mq_tagset_busy_iter(&nbd->tag_set, nbd_clear_req, NULL);
	dev_dbg(disk_to_dev(nbd->disk), "queue cleared\n");
}


static void nbd_handle_cmd(struct nbd_cmd *cmd)
{
	struct request *req = blk_mq_rq_from_pdu(cmd);
	struct nbd_device *nbd = cmd->nbd;

	if (req->cmd_type != REQ_TYPE_FS)
		goto error_out;

	if (rq_data_dir(req) == WRITE &&
	    (nbd->flags & NBD_FLAG_READ_ONLY)) {
		dev_err(disk_to_dev(nbd->disk),
			"Write on read-only\n");
		goto error_out;
>>>>>>> v4.9.227
	}

	req->errors = 0;

	mutex_lock(&nbd->tx_lock);
<<<<<<< HEAD
=======
	nbd->task_send = current;
>>>>>>> v4.9.227
	if (unlikely(!nbd->sock)) {
		mutex_unlock(&nbd->tx_lock);
		dev_err(disk_to_dev(nbd->disk),
			"Attempted send on closed socket\n");
		goto error_out;
	}

<<<<<<< HEAD
	nbd->active_req = req;

	if (nbd_send_req(nbd, req) != 0) {
		dev_err(disk_to_dev(nbd->disk), "Request send failed\n");
		req->errors++;
		nbd_end_request(req);
	} else {
		spin_lock(&nbd->queue_lock);
		list_add_tail(&req->queuelist, &nbd->queue_head);
		spin_unlock(&nbd->queue_lock);
	}

	nbd->active_req = NULL;
	mutex_unlock(&nbd->tx_lock);
	wake_up_all(&nbd->active_wq);
=======
	if (nbd_send_cmd(nbd, cmd) != 0) {
		dev_err(disk_to_dev(nbd->disk), "Request send failed\n");
		req->errors++;
		nbd_end_request(cmd);
	}

	nbd->task_send = NULL;
	mutex_unlock(&nbd->tx_lock);
>>>>>>> v4.9.227

	return;

error_out:
	req->errors++;
<<<<<<< HEAD
	nbd_end_request(req);
}

static int nbd_thread(void *data)
{
	struct nbd_device *nbd = data;
	struct request *req;

	set_user_nice(current, MIN_NICE);
	while (!kthread_should_stop() || !list_empty(&nbd->waiting_queue)) {
		/* wait for something to do */
		wait_event_interruptible(nbd->waiting_wq,
					 kthread_should_stop() ||
					 !list_empty(&nbd->waiting_queue));

		/* extract request */
		if (list_empty(&nbd->waiting_queue))
			continue;

		spin_lock_irq(&nbd->queue_lock);
		req = list_entry(nbd->waiting_queue.next, struct request,
				 queuelist);
		list_del_init(&req->queuelist);
		spin_unlock_irq(&nbd->queue_lock);

		/* handle request */
		nbd_handle_req(nbd, req);
	}
	return 0;
}

/*
 * We always wait for result of write, for now. It would be nice to make it optional
 * in future
 * if ((rq_data_dir(req) == WRITE) && (nbd->flags & NBD_WRITE_NOCHK))
 *   { printk( "Warning: Ignoring result!\n"); nbd_end_request( req ); }
 */

static void do_nbd_request(struct request_queue *q)
		__releases(q->queue_lock) __acquires(q->queue_lock)
{
	struct request *req;
	
	while ((req = blk_fetch_request(q)) != NULL) {
		struct nbd_device *nbd;

		spin_unlock_irq(q->queue_lock);

		dprintk(DBG_BLKDEV, "%s: request %p: dequeued (flags=%x)\n",
				req->rq_disk->disk_name, req, req->cmd_type);

		nbd = req->rq_disk->private_data;

		BUG_ON(nbd->magic != NBD_MAGIC);

		if (unlikely(!nbd->sock)) {
			dev_err_ratelimited(disk_to_dev(nbd->disk),
					    "Attempted send on closed socket\n");
			req->errors++;
			nbd_end_request(req);
			spin_lock_irq(q->queue_lock);
			continue;
		}

		spin_lock_irq(&nbd->queue_lock);
		list_add_tail(&req->queuelist, &nbd->waiting_queue);
		spin_unlock_irq(&nbd->queue_lock);

		wake_up(&nbd->waiting_wq);

		spin_lock_irq(q->queue_lock);
	}
}

=======
	nbd_end_request(cmd);
}

static int nbd_queue_rq(struct blk_mq_hw_ctx *hctx,
			const struct blk_mq_queue_data *bd)
{
	struct nbd_cmd *cmd = blk_mq_rq_to_pdu(bd->rq);

	blk_mq_start_request(bd->rq);
	nbd_handle_cmd(cmd);
	return BLK_MQ_RQ_QUEUE_OK;
}

static int nbd_set_socket(struct nbd_device *nbd, struct socket *sock)
{
	int ret = 0;

	spin_lock_irq(&nbd->sock_lock);

	if (nbd->sock) {
		ret = -EBUSY;
		goto out;
	}

	nbd->sock = sock;

out:
	spin_unlock_irq(&nbd->sock_lock);

	return ret;
}

/* Reset all properties of an NBD device */
static void nbd_reset(struct nbd_device *nbd)
{
	nbd->runtime_flags = 0;
	nbd->blksize = 1024;
	nbd->bytesize = 0;
	set_capacity(nbd->disk, 0);
	nbd->flags = 0;
	nbd->tag_set.timeout = 0;
	queue_flag_clear_unlocked(QUEUE_FLAG_DISCARD, nbd->disk->queue);
}

static void nbd_bdev_reset(struct block_device *bdev)
{
	set_device_ro(bdev, false);
	bdev->bd_inode->i_size = 0;
	if (max_part > 0) {
		blkdev_reread_part(bdev);
		bdev->bd_invalidated = 1;
	}
}

static void nbd_parse_flags(struct nbd_device *nbd, struct block_device *bdev)
{
	if (nbd->flags & NBD_FLAG_READ_ONLY)
		set_device_ro(bdev, true);
	if (nbd->flags & NBD_FLAG_SEND_TRIM)
		queue_flag_set_unlocked(QUEUE_FLAG_DISCARD, nbd->disk->queue);
	if (nbd->flags & NBD_FLAG_SEND_FLUSH)
		blk_queue_write_cache(nbd->disk->queue, true, false);
	else
		blk_queue_write_cache(nbd->disk->queue, false, false);
}

static int nbd_dev_dbg_init(struct nbd_device *nbd);
static void nbd_dev_dbg_close(struct nbd_device *nbd);

>>>>>>> v4.9.227
/* Must be called with tx_lock held */

static int __nbd_ioctl(struct block_device *bdev, struct nbd_device *nbd,
		       unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case NBD_DISCONNECT: {
<<<<<<< HEAD
		struct request sreq;
=======
		struct request *sreq;
>>>>>>> v4.9.227

		dev_info(disk_to_dev(nbd->disk), "NBD_DISCONNECT\n");
		if (!nbd->sock)
			return -EINVAL;

<<<<<<< HEAD
		mutex_unlock(&nbd->tx_lock);
		fsync_bdev(bdev);
		mutex_lock(&nbd->tx_lock);
		blk_rq_init(NULL, &sreq);
		sreq.cmd_type = REQ_TYPE_SPECIAL;
		nbd_cmd(&sreq) = NBD_CMD_DISC;

		/* Check again after getting mutex back.  */
		if (!nbd->sock)
			return -EINVAL;

		nbd->disconnect = 1;

		nbd_send_req(nbd, &sreq);
		return 0;
	}
 
	case NBD_CLEAR_SOCK: {
		struct socket *sock = nbd->sock;
		nbd->sock = NULL;
		nbd_clear_que(nbd);
		BUG_ON(!list_empty(&nbd->queue_head));
		BUG_ON(!list_empty(&nbd->waiting_queue));
		kill_bdev(bdev);
		if (sock)
			sockfd_put(sock);
		return 0;
	}

	case NBD_SET_SOCK: {
		struct socket *sock;
		int err;
		if (nbd->sock)
			return -EBUSY;
		sock = sockfd_lookup(arg, &err);
		if (sock) {
			nbd->sock = sock;
			if (max_part > 0)
				bdev->bd_invalidated = 1;
			nbd->disconnect = 0; /* we're connected now */
			return 0;
		}
		return -EINVAL;
	}

	case NBD_SET_BLKSIZE:
		nbd->blksize = arg;
		nbd->bytesize &= ~(nbd->blksize-1);
		bdev->bd_inode->i_size = nbd->bytesize;
		set_blocksize(bdev, nbd->blksize);
		set_capacity(nbd->disk, nbd->bytesize >> 9);
		return 0;

	case NBD_SET_SIZE:
		nbd->bytesize = arg & ~(nbd->blksize-1);
		bdev->bd_inode->i_size = nbd->bytesize;
		set_blocksize(bdev, nbd->blksize);
		set_capacity(nbd->disk, nbd->bytesize >> 9);
		return 0;

	case NBD_SET_TIMEOUT:
		nbd->xmit_timeout = arg * HZ;
=======
		sreq = blk_mq_alloc_request(bdev_get_queue(bdev), WRITE, 0);
		if (IS_ERR(sreq))
			return -ENOMEM;

		mutex_unlock(&nbd->tx_lock);
		fsync_bdev(bdev);
		mutex_lock(&nbd->tx_lock);
		sreq->cmd_type = REQ_TYPE_DRV_PRIV;

		/* Check again after getting mutex back.  */
		if (!nbd->sock) {
			blk_mq_free_request(sreq);
			return -EINVAL;
		}

		set_bit(NBD_DISCONNECT_REQUESTED, &nbd->runtime_flags);

		nbd_send_cmd(nbd, blk_mq_rq_to_pdu(sreq));
		blk_mq_free_request(sreq);
		return 0;
	}
 
	case NBD_CLEAR_SOCK:
		sock_shutdown(nbd);
		nbd_clear_que(nbd);
		kill_bdev(bdev);
		return 0;

	case NBD_SET_SOCK: {
		int err;
		struct socket *sock = sockfd_lookup(arg, &err);

		if (!sock)
			return err;

		err = nbd_set_socket(nbd, sock);
		if (!err && max_part)
			bdev->bd_invalidated = 1;

		return err;
	}

	case NBD_SET_BLKSIZE: {
		loff_t bsize = div_s64(nbd->bytesize, arg);

		nbd_size_set(nbd, bdev, arg, bsize);
		return 0;
	}

	case NBD_SET_SIZE:
		nbd_size_set(nbd, bdev, nbd->blksize,
			     div_s64(arg, nbd->blksize));
		return 0;
	case NBD_SET_SIZE_BLOCKS:
		nbd_size_set(nbd, bdev, nbd->blksize, arg);
		return 0;
	case NBD_SET_TIMEOUT:
		if (arg) {
			nbd->tag_set.timeout = arg * HZ;
			blk_queue_rq_timeout(nbd->disk->queue, arg * HZ);
		}
>>>>>>> v4.9.227
		return 0;

	case NBD_SET_FLAGS:
		nbd->flags = arg;
		return 0;

<<<<<<< HEAD
	case NBD_SET_SIZE_BLOCKS:
		nbd->bytesize = ((u64) arg) * nbd->blksize;
		bdev->bd_inode->i_size = nbd->bytesize;
		set_blocksize(bdev, nbd->blksize);
		set_capacity(nbd->disk, nbd->bytesize >> 9);
		return 0;

	case NBD_DO_IT: {
		struct task_struct *thread;
		struct socket *sock;
		int error;

		if (nbd->pid)
=======
	case NBD_DO_IT: {
		int error;

		if (nbd->task_recv)
>>>>>>> v4.9.227
			return -EBUSY;
		if (!nbd->sock)
			return -EINVAL;

<<<<<<< HEAD
		mutex_unlock(&nbd->tx_lock);

		if (nbd->flags & NBD_FLAG_READ_ONLY)
			set_device_ro(bdev, true);
		if (nbd->flags & NBD_FLAG_SEND_TRIM)
			queue_flag_set_unlocked(QUEUE_FLAG_DISCARD,
				nbd->disk->queue);
		if (nbd->flags & NBD_FLAG_SEND_FLUSH)
			blk_queue_flush(nbd->disk->queue, REQ_FLUSH);
		else
			blk_queue_flush(nbd->disk->queue, 0);

		thread = kthread_create(nbd_thread, nbd, "%s",
					nbd->disk->disk_name);
		if (IS_ERR(thread)) {
			mutex_lock(&nbd->tx_lock);
			return PTR_ERR(thread);
		}
		wake_up_process(thread);
		error = nbd_do_it(nbd);
		kthread_stop(thread);

		mutex_lock(&nbd->tx_lock);
		if (error)
			return error;
		sock_shutdown(nbd, 0);
		sock = nbd->sock;
		nbd->sock = NULL;
		nbd_clear_que(nbd);
		dev_warn(disk_to_dev(nbd->disk), "queue cleared\n");
		kill_bdev(bdev);
		queue_flag_clear_unlocked(QUEUE_FLAG_DISCARD, nbd->disk->queue);
		set_device_ro(bdev, false);
		if (sock)
			sockfd_put(sock);
		nbd->flags = 0;
		nbd->bytesize = 0;
		bdev->bd_inode->i_size = 0;
		set_capacity(nbd->disk, 0);
		if (max_part > 0)
			ioctl_by_bdev(bdev, BLKRRPART, 0);
		if (nbd->disconnect) /* user requested, ignore socket errors */
			return 0;
		return nbd->harderror;
=======
		/* We have to claim the device under the lock */
		nbd->task_recv = current;
		mutex_unlock(&nbd->tx_lock);

		nbd_parse_flags(nbd, bdev);

		nbd_dev_dbg_init(nbd);
		error = nbd_thread_recv(nbd, bdev);
		nbd_dev_dbg_close(nbd);

		mutex_lock(&nbd->tx_lock);
		nbd->task_recv = NULL;

		sock_shutdown(nbd);
		nbd_clear_que(nbd);
		kill_bdev(bdev);
		nbd_bdev_reset(bdev);

		/* user requested, ignore socket errors */
		if (test_bit(NBD_DISCONNECT_REQUESTED, &nbd->runtime_flags))
			error = 0;
		if (test_bit(NBD_TIMEDOUT, &nbd->runtime_flags))
			error = -ETIMEDOUT;

		nbd_reset(nbd);

		return error;
>>>>>>> v4.9.227
	}

	case NBD_CLEAR_QUE:
		/*
		 * This is for compatibility only.  The queue is always cleared
		 * by NBD_DO_IT or NBD_CLEAR_SOCK.
		 */
		return 0;

	case NBD_PRINT_DEBUG:
<<<<<<< HEAD
		dev_info(disk_to_dev(nbd->disk),
			"next = %p, prev = %p, head = %p\n",
			nbd->queue_head.next, nbd->queue_head.prev,
			&nbd->queue_head);
=======
		/*
		 * For compatibility only, we no longer keep a list of
		 * outstanding requests.
		 */
>>>>>>> v4.9.227
		return 0;
	}
	return -ENOTTY;
}

static int nbd_ioctl(struct block_device *bdev, fmode_t mode,
		     unsigned int cmd, unsigned long arg)
{
	struct nbd_device *nbd = bdev->bd_disk->private_data;
	int error;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	BUG_ON(nbd->magic != NBD_MAGIC);

<<<<<<< HEAD
	/* Anyone capable of this syscall can do *real bad* things */
	dprintk(DBG_IOCTL, "%s: nbd_ioctl cmd=%s(0x%x) arg=%lu\n",
		nbd->disk->disk_name, ioctl_cmd_to_ascii(cmd), cmd, arg);

=======
>>>>>>> v4.9.227
	mutex_lock(&nbd->tx_lock);
	error = __nbd_ioctl(bdev, nbd, cmd, arg);
	mutex_unlock(&nbd->tx_lock);

	return error;
}

static const struct block_device_operations nbd_fops =
{
	.owner =	THIS_MODULE,
	.ioctl =	nbd_ioctl,
<<<<<<< HEAD
=======
	.compat_ioctl =	nbd_ioctl,
};

#if IS_ENABLED(CONFIG_DEBUG_FS)

static int nbd_dbg_tasks_show(struct seq_file *s, void *unused)
{
	struct nbd_device *nbd = s->private;

	if (nbd->task_recv)
		seq_printf(s, "recv: %d\n", task_pid_nr(nbd->task_recv));
	if (nbd->task_send)
		seq_printf(s, "send: %d\n", task_pid_nr(nbd->task_send));

	return 0;
}

static int nbd_dbg_tasks_open(struct inode *inode, struct file *file)
{
	return single_open(file, nbd_dbg_tasks_show, inode->i_private);
}

static const struct file_operations nbd_dbg_tasks_ops = {
	.open = nbd_dbg_tasks_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int nbd_dbg_flags_show(struct seq_file *s, void *unused)
{
	struct nbd_device *nbd = s->private;
	u32 flags = nbd->flags;

	seq_printf(s, "Hex: 0x%08x\n\n", flags);

	seq_puts(s, "Known flags:\n");

	if (flags & NBD_FLAG_HAS_FLAGS)
		seq_puts(s, "NBD_FLAG_HAS_FLAGS\n");
	if (flags & NBD_FLAG_READ_ONLY)
		seq_puts(s, "NBD_FLAG_READ_ONLY\n");
	if (flags & NBD_FLAG_SEND_FLUSH)
		seq_puts(s, "NBD_FLAG_SEND_FLUSH\n");
	if (flags & NBD_FLAG_SEND_TRIM)
		seq_puts(s, "NBD_FLAG_SEND_TRIM\n");

	return 0;
}

static int nbd_dbg_flags_open(struct inode *inode, struct file *file)
{
	return single_open(file, nbd_dbg_flags_show, inode->i_private);
}

static const struct file_operations nbd_dbg_flags_ops = {
	.open = nbd_dbg_flags_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int nbd_dev_dbg_init(struct nbd_device *nbd)
{
	struct dentry *dir;

	if (!nbd_dbg_dir)
		return -EIO;

	dir = debugfs_create_dir(nbd_name(nbd), nbd_dbg_dir);
	if (!dir) {
		dev_err(nbd_to_dev(nbd), "Failed to create debugfs dir for '%s'\n",
			nbd_name(nbd));
		return -EIO;
	}
	nbd->dbg_dir = dir;

	debugfs_create_file("tasks", 0444, dir, nbd, &nbd_dbg_tasks_ops);
	debugfs_create_u64("size_bytes", 0444, dir, &nbd->bytesize);
	debugfs_create_u32("timeout", 0444, dir, &nbd->tag_set.timeout);
	debugfs_create_u64("blocksize", 0444, dir, &nbd->blksize);
	debugfs_create_file("flags", 0444, dir, nbd, &nbd_dbg_flags_ops);

	return 0;
}

static void nbd_dev_dbg_close(struct nbd_device *nbd)
{
	debugfs_remove_recursive(nbd->dbg_dir);
}

static int nbd_dbg_init(void)
{
	struct dentry *dbg_dir;

	dbg_dir = debugfs_create_dir("nbd", NULL);
	if (!dbg_dir)
		return -EIO;

	nbd_dbg_dir = dbg_dir;

	return 0;
}

static void nbd_dbg_close(void)
{
	debugfs_remove_recursive(nbd_dbg_dir);
}

#else  /* IS_ENABLED(CONFIG_DEBUG_FS) */

static int nbd_dev_dbg_init(struct nbd_device *nbd)
{
	return 0;
}

static void nbd_dev_dbg_close(struct nbd_device *nbd)
{
}

static int nbd_dbg_init(void)
{
	return 0;
}

static void nbd_dbg_close(void)
{
}

#endif

static int nbd_init_request(void *data, struct request *rq,
			    unsigned int hctx_idx, unsigned int request_idx,
			    unsigned int numa_node)
{
	struct nbd_cmd *cmd = blk_mq_rq_to_pdu(rq);

	cmd->nbd = data;
	INIT_LIST_HEAD(&cmd->list);
	return 0;
}

static struct blk_mq_ops nbd_mq_ops = {
	.queue_rq	= nbd_queue_rq,
	.init_request	= nbd_init_request,
	.timeout	= nbd_xmit_timeout,
>>>>>>> v4.9.227
};

/*
 * And here should be modules and kernel interface 
 *  (Just smiley confuses emacs :-)
 */

static int __init nbd_init(void)
{
	int err = -ENOMEM;
	int i;
	int part_shift;

	BUILD_BUG_ON(sizeof(struct nbd_request) != 28);

	if (max_part < 0) {
		printk(KERN_ERR "nbd: max_part must be >= 0\n");
		return -EINVAL;
	}

	part_shift = 0;
	if (max_part > 0) {
		part_shift = fls(max_part);

		/*
		 * Adjust max_part according to part_shift as it is exported
		 * to user space so that user can know the max number of
		 * partition kernel should be able to manage.
		 *
		 * Note that -1 is required because partition 0 is reserved
		 * for the whole disk.
		 */
		max_part = (1UL << part_shift) - 1;
	}

	if ((1UL << part_shift) > DISK_MAX_PARTS)
		return -EINVAL;

	if (nbds_max > 1UL << (MINORBITS - part_shift))
		return -EINVAL;

	nbd_dev = kcalloc(nbds_max, sizeof(*nbd_dev), GFP_KERNEL);
	if (!nbd_dev)
		return -ENOMEM;

	for (i = 0; i < nbds_max; i++) {
<<<<<<< HEAD
=======
		struct request_queue *q;
>>>>>>> v4.9.227
		struct gendisk *disk = alloc_disk(1 << part_shift);
		if (!disk)
			goto out;
		nbd_dev[i].disk = disk;
<<<<<<< HEAD
=======

		nbd_dev[i].tag_set.ops = &nbd_mq_ops;
		nbd_dev[i].tag_set.nr_hw_queues = 1;
		nbd_dev[i].tag_set.queue_depth = 128;
		nbd_dev[i].tag_set.numa_node = NUMA_NO_NODE;
		nbd_dev[i].tag_set.cmd_size = sizeof(struct nbd_cmd);
		nbd_dev[i].tag_set.flags = BLK_MQ_F_SHOULD_MERGE |
			BLK_MQ_F_SG_MERGE | BLK_MQ_F_BLOCKING;
		nbd_dev[i].tag_set.driver_data = &nbd_dev[i];

		err = blk_mq_alloc_tag_set(&nbd_dev[i].tag_set);
		if (err) {
			put_disk(disk);
			goto out;
		}

>>>>>>> v4.9.227
		/*
		 * The new linux 2.5 block layer implementation requires
		 * every gendisk to have its very own request_queue struct.
		 * These structs are big so we dynamically allocate them.
		 */
<<<<<<< HEAD
		disk->queue = blk_init_queue(do_nbd_request, &nbd_lock);
		if (!disk->queue) {
			put_disk(disk);
			goto out;
		}
=======
		q = blk_mq_init_queue(&nbd_dev[i].tag_set);
		if (IS_ERR(q)) {
			blk_mq_free_tag_set(&nbd_dev[i].tag_set);
			put_disk(disk);
			goto out;
		}
		disk->queue = q;

>>>>>>> v4.9.227
		/*
		 * Tell the block layer that we are not a rotational device
		 */
		queue_flag_set_unlocked(QUEUE_FLAG_NONROT, disk->queue);
		queue_flag_clear_unlocked(QUEUE_FLAG_ADD_RANDOM, disk->queue);
		disk->queue->limits.discard_granularity = 512;
<<<<<<< HEAD
		disk->queue->limits.max_discard_sectors = UINT_MAX;
=======
		blk_queue_max_discard_sectors(disk->queue, UINT_MAX);
>>>>>>> v4.9.227
		disk->queue->limits.discard_zeroes_data = 0;
		blk_queue_max_hw_sectors(disk->queue, 65536);
		disk->queue->limits.max_sectors = 256;
	}

	if (register_blkdev(NBD_MAJOR, "nbd")) {
		err = -EIO;
		goto out;
	}

	printk(KERN_INFO "nbd: registered device at major %d\n", NBD_MAJOR);
<<<<<<< HEAD
	dprintk(DBG_INIT, "nbd: debugflags=0x%x\n", debugflags);
=======

	nbd_dbg_init();
>>>>>>> v4.9.227

	for (i = 0; i < nbds_max; i++) {
		struct gendisk *disk = nbd_dev[i].disk;
		nbd_dev[i].magic = NBD_MAGIC;
<<<<<<< HEAD
		INIT_LIST_HEAD(&nbd_dev[i].waiting_queue);
		spin_lock_init(&nbd_dev[i].queue_lock);
		INIT_LIST_HEAD(&nbd_dev[i].queue_head);
		mutex_init(&nbd_dev[i].tx_lock);
		init_waitqueue_head(&nbd_dev[i].active_wq);
		init_waitqueue_head(&nbd_dev[i].waiting_wq);
		nbd_dev[i].blksize = 1024;
		nbd_dev[i].bytesize = 0;
=======
		spin_lock_init(&nbd_dev[i].sock_lock);
		mutex_init(&nbd_dev[i].tx_lock);
>>>>>>> v4.9.227
		disk->major = NBD_MAJOR;
		disk->first_minor = i << part_shift;
		disk->fops = &nbd_fops;
		disk->private_data = &nbd_dev[i];
		sprintf(disk->disk_name, "nbd%d", i);
<<<<<<< HEAD
		set_capacity(disk, 0);
=======
		nbd_reset(&nbd_dev[i]);
>>>>>>> v4.9.227
		add_disk(disk);
	}

	return 0;
out:
	while (i--) {
<<<<<<< HEAD
=======
		blk_mq_free_tag_set(&nbd_dev[i].tag_set);
>>>>>>> v4.9.227
		blk_cleanup_queue(nbd_dev[i].disk->queue);
		put_disk(nbd_dev[i].disk);
	}
	kfree(nbd_dev);
	return err;
}

static void __exit nbd_cleanup(void)
{
	int i;
<<<<<<< HEAD
=======

	nbd_dbg_close();

>>>>>>> v4.9.227
	for (i = 0; i < nbds_max; i++) {
		struct gendisk *disk = nbd_dev[i].disk;
		nbd_dev[i].magic = 0;
		if (disk) {
			del_gendisk(disk);
			blk_cleanup_queue(disk->queue);
<<<<<<< HEAD
=======
			blk_mq_free_tag_set(&nbd_dev[i].tag_set);
>>>>>>> v4.9.227
			put_disk(disk);
		}
	}
	unregister_blkdev(NBD_MAJOR, "nbd");
	kfree(nbd_dev);
	printk(KERN_INFO "nbd: unregistered device at major %d\n", NBD_MAJOR);
}

module_init(nbd_init);
module_exit(nbd_cleanup);

MODULE_DESCRIPTION("Network Block Device");
MODULE_LICENSE("GPL");

module_param(nbds_max, int, 0444);
MODULE_PARM_DESC(nbds_max, "number of network block devices to initialize (default: 16)");
module_param(max_part, int, 0444);
MODULE_PARM_DESC(max_part, "number of partitions per device (default: 0)");
<<<<<<< HEAD
#ifndef NDEBUG
module_param(debugflags, int, 0644);
MODULE_PARM_DESC(debugflags, "flags for controlling debug output");
#endif
=======
>>>>>>> v4.9.227
