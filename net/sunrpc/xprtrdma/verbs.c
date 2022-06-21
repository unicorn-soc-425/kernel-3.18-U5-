/*
 * Copyright (c) 2003-2007 Network Appliance, Inc. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the BSD-type
 * license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *      Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *      Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 *      Neither the name of the Network Appliance, Inc. nor the names of
 *      its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written
 *      permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * verbs.c
 *
 * Encapsulates the major functions managing:
 *  o adapters
 *  o endpoints
 *  o connections
 *  o buffer memory
 */

#include <linux/interrupt.h>
#include <linux/slab.h>
<<<<<<< HEAD
#include <asm/bitops.h>
=======
#include <linux/prefetch.h>
#include <linux/sunrpc/addr.h>
#include <linux/sunrpc/svc_rdma.h>
#include <asm/bitops.h>
#include <linux/module.h> /* try_module_get()/module_put() */
>>>>>>> v4.9.227

#include "xprt_rdma.h"

/*
 * Globals/Macros
 */

<<<<<<< HEAD
#ifdef RPC_DEBUG
# define RPCDBG_FACILITY	RPCDBG_TRANS
#endif

static void rpcrdma_reset_frmrs(struct rpcrdma_ia *);

=======
#if IS_ENABLED(CONFIG_SUNRPC_DEBUG)
# define RPCDBG_FACILITY	RPCDBG_TRANS
#endif

>>>>>>> v4.9.227
/*
 * internal functions
 */

<<<<<<< HEAD
/*
 * handle replies in tasklet context, using a single, global list
 * rdma tasklet function -- just turn around and call the func
 * for all replies on the list
 */

static DEFINE_SPINLOCK(rpcrdma_tk_lock_g);
static LIST_HEAD(rpcrdma_tasklets_g);

static void
rpcrdma_run_tasklet(unsigned long data)
{
	struct rpcrdma_rep *rep;
	void (*func)(struct rpcrdma_rep *);
	unsigned long flags;

	data = data;
	spin_lock_irqsave(&rpcrdma_tk_lock_g, flags);
	while (!list_empty(&rpcrdma_tasklets_g)) {
		rep = list_entry(rpcrdma_tasklets_g.next,
				 struct rpcrdma_rep, rr_list);
		list_del(&rep->rr_list);
		func = rep->rr_func;
		rep->rr_func = NULL;
		spin_unlock_irqrestore(&rpcrdma_tk_lock_g, flags);

		if (func)
			func(rep);
		else
			rpcrdma_recv_buffer_put(rep);

		spin_lock_irqsave(&rpcrdma_tk_lock_g, flags);
	}
	spin_unlock_irqrestore(&rpcrdma_tk_lock_g, flags);
}

static DECLARE_TASKLET(rpcrdma_tasklet_g, rpcrdma_run_tasklet, 0UL);
=======
static struct workqueue_struct *rpcrdma_receive_wq;

int
rpcrdma_alloc_wq(void)
{
	struct workqueue_struct *recv_wq;

	recv_wq = alloc_workqueue("xprtrdma_receive",
				  WQ_MEM_RECLAIM | WQ_UNBOUND | WQ_HIGHPRI,
				  0);
	if (!recv_wq)
		return -ENOMEM;

	rpcrdma_receive_wq = recv_wq;
	return 0;
}

void
rpcrdma_destroy_wq(void)
{
	struct workqueue_struct *wq;

	if (rpcrdma_receive_wq) {
		wq = rpcrdma_receive_wq;
		rpcrdma_receive_wq = NULL;
		destroy_workqueue(wq);
	}
}
>>>>>>> v4.9.227

static void
rpcrdma_qp_async_error_upcall(struct ib_event *event, void *context)
{
	struct rpcrdma_ep *ep = context;

<<<<<<< HEAD
	dprintk("RPC:       %s: QP error %X on device %s ep %p\n",
		__func__, event->event, event->device->name, context);
	if (ep->rep_connected == 1) {
		ep->rep_connected = -EIO;
		ep->rep_func(ep);
=======
	pr_err("RPC:       %s: %s on device %s ep %p\n",
	       __func__, ib_event_msg(event->event),
		event->device->name, context);
	if (ep->rep_connected == 1) {
		ep->rep_connected = -EIO;
		rpcrdma_conn_func(ep);
>>>>>>> v4.9.227
		wake_up_all(&ep->rep_connect_wait);
	}
}

<<<<<<< HEAD
static void
rpcrdma_cq_async_error_upcall(struct ib_event *event, void *context)
{
	struct rpcrdma_ep *ep = context;

	dprintk("RPC:       %s: CQ error %X on device %s ep %p\n",
		__func__, event->event, event->device->name, context);
	if (ep->rep_connected == 1) {
		ep->rep_connected = -EIO;
		ep->rep_func(ep);
		wake_up_all(&ep->rep_connect_wait);
	}
}

static void
rpcrdma_sendcq_process_wc(struct ib_wc *wc)
{
	struct rpcrdma_mw *frmr = (struct rpcrdma_mw *)(unsigned long)wc->wr_id;

	dprintk("RPC:       %s: frmr %p status %X opcode %d\n",
		__func__, frmr, wc->status, wc->opcode);

	if (wc->wr_id == 0ULL)
		return;
	if (wc->status != IB_WC_SUCCESS)
		frmr->r.frmr.fr_state = FRMR_IS_STALE;
}

static int
rpcrdma_sendcq_poll(struct ib_cq *cq, struct rpcrdma_ep *ep)
{
	struct ib_wc *wcs;
	int budget, count, rc;

	budget = RPCRDMA_WC_BUDGET / RPCRDMA_POLLSIZE;
	do {
		wcs = ep->rep_send_wcs;

		rc = ib_poll_cq(cq, RPCRDMA_POLLSIZE, wcs);
		if (rc <= 0)
			return rc;

		count = rc;
		while (count-- > 0)
			rpcrdma_sendcq_process_wc(wcs++);
	} while (rc == RPCRDMA_POLLSIZE && --budget);
	return 0;
}

/*
 * Handle send, fast_reg_mr, and local_inv completions.
 *
 * Send events are typically suppressed and thus do not result
 * in an upcall. Occasionally one is signaled, however. This
 * prevents the provider's completion queue from wrapping and
 * losing a completion.
 */
static void
rpcrdma_sendcq_upcall(struct ib_cq *cq, void *cq_context)
{
	struct rpcrdma_ep *ep = (struct rpcrdma_ep *)cq_context;
	int rc;

	rc = rpcrdma_sendcq_poll(cq, ep);
	if (rc) {
		dprintk("RPC:       %s: ib_poll_cq failed: %i\n",
			__func__, rc);
		return;
	}

	rc = ib_req_notify_cq(cq,
			IB_CQ_NEXT_COMP | IB_CQ_REPORT_MISSED_EVENTS);
	if (rc == 0)
		return;
	if (rc < 0) {
		dprintk("RPC:       %s: ib_req_notify_cq failed: %i\n",
			__func__, rc);
		return;
	}

	rpcrdma_sendcq_poll(cq, ep);
}

static void
rpcrdma_recvcq_process_wc(struct ib_wc *wc, struct list_head *sched_list)
{
	struct rpcrdma_rep *rep =
			(struct rpcrdma_rep *)(unsigned long)wc->wr_id;

	dprintk("RPC:       %s: rep %p status %X opcode %X length %u\n",
		__func__, rep, wc->status, wc->opcode, wc->byte_len);

	if (wc->status != IB_WC_SUCCESS) {
		rep->rr_len = ~0U;
		goto out_schedule;
	}
	if (wc->opcode != IB_WC_RECV)
		return;

	rep->rr_len = wc->byte_len;
	ib_dma_sync_single_for_cpu(rdmab_to_ia(rep->rr_buffer)->ri_id->device,
			rep->rr_iov.addr, rep->rr_len, DMA_FROM_DEVICE);

	if (rep->rr_len >= 16) {
		struct rpcrdma_msg *p = (struct rpcrdma_msg *)rep->rr_base;
		unsigned int credits = ntohl(p->rm_credit);

		if (credits == 0)
			credits = 1;	/* don't deadlock */
		else if (credits > rep->rr_buffer->rb_max_requests)
			credits = rep->rr_buffer->rb_max_requests;
		atomic_set(&rep->rr_buffer->rb_credits, credits);
	}

out_schedule:
	list_add_tail(&rep->rr_list, sched_list);
}

static int
rpcrdma_recvcq_poll(struct ib_cq *cq, struct rpcrdma_ep *ep)
{
	struct list_head sched_list;
	struct ib_wc *wcs;
	int budget, count, rc;
	unsigned long flags;

	INIT_LIST_HEAD(&sched_list);
	budget = RPCRDMA_WC_BUDGET / RPCRDMA_POLLSIZE;
	do {
		wcs = ep->rep_recv_wcs;

		rc = ib_poll_cq(cq, RPCRDMA_POLLSIZE, wcs);
		if (rc <= 0)
			goto out_schedule;

		count = rc;
		while (count-- > 0)
			rpcrdma_recvcq_process_wc(wcs++, &sched_list);
	} while (rc == RPCRDMA_POLLSIZE && --budget);
	rc = 0;

out_schedule:
	spin_lock_irqsave(&rpcrdma_tk_lock_g, flags);
	list_splice_tail(&sched_list, &rpcrdma_tasklets_g);
	spin_unlock_irqrestore(&rpcrdma_tk_lock_g, flags);
	tasklet_schedule(&rpcrdma_tasklet_g);
	return rc;
}

/*
 * Handle receive completions.
 *
 * It is reentrant but processes single events in order to maintain
 * ordering of receives to keep server credits.
 *
 * It is the responsibility of the scheduled tasklet to return
 * recv buffers to the pool. NOTE: this affects synchronization of
 * connection shutdown. That is, the structures required for
 * the completion of the reply handler must remain intact until
 * all memory has been reclaimed.
 */
static void
rpcrdma_recvcq_upcall(struct ib_cq *cq, void *cq_context)
{
	struct rpcrdma_ep *ep = (struct rpcrdma_ep *)cq_context;
	int rc;

	rc = rpcrdma_recvcq_poll(cq, ep);
	if (rc) {
		dprintk("RPC:       %s: ib_poll_cq failed: %i\n",
			__func__, rc);
		return;
	}

	rc = ib_req_notify_cq(cq,
			IB_CQ_NEXT_COMP | IB_CQ_REPORT_MISSED_EVENTS);
	if (rc == 0)
		return;
	if (rc < 0) {
		dprintk("RPC:       %s: ib_req_notify_cq failed: %i\n",
			__func__, rc);
		return;
	}

	rpcrdma_recvcq_poll(cq, ep);
}

static void
rpcrdma_flush_cqs(struct rpcrdma_ep *ep)
{
	rpcrdma_recvcq_upcall(ep->rep_attr.recv_cq, ep);
	rpcrdma_sendcq_upcall(ep->rep_attr.send_cq, ep);
}

#ifdef RPC_DEBUG
static const char * const conn[] = {
	"address resolved",
	"address error",
	"route resolved",
	"route error",
	"connect request",
	"connect response",
	"connect error",
	"unreachable",
	"rejected",
	"established",
	"disconnected",
	"device removal",
	"multicast join",
	"multicast error",
	"address change",
	"timewait exit",
};

#define CONNECTION_MSG(status)						\
	((status) < ARRAY_SIZE(conn) ?					\
		conn[(status)] : "unrecognized connection error")
#endif

=======
/**
 * rpcrdma_wc_send - Invoked by RDMA provider for each polled Send WC
 * @cq:	completion queue (ignored)
 * @wc:	completed WR
 *
 */
static void
rpcrdma_wc_send(struct ib_cq *cq, struct ib_wc *wc)
{
	/* WARNING: Only wr_cqe and status are reliable at this point */
	if (wc->status != IB_WC_SUCCESS && wc->status != IB_WC_WR_FLUSH_ERR)
		pr_err("rpcrdma: Send: %s (%u/0x%x)\n",
		       ib_wc_status_msg(wc->status),
		       wc->status, wc->vendor_err);
}

/* Perform basic sanity checking to avoid using garbage
 * to update the credit grant value.
 */
static void
rpcrdma_update_granted_credits(struct rpcrdma_rep *rep)
{
	struct rpcrdma_msg *rmsgp = rdmab_to_msg(rep->rr_rdmabuf);
	struct rpcrdma_buffer *buffer = &rep->rr_rxprt->rx_buf;
	u32 credits;

	if (rep->rr_len < RPCRDMA_HDRLEN_ERR)
		return;

	credits = be32_to_cpu(rmsgp->rm_credit);
	if (credits == 0)
		credits = 1;	/* don't deadlock */
	else if (credits > buffer->rb_max_requests)
		credits = buffer->rb_max_requests;

	atomic_set(&buffer->rb_credits, credits);
}

/**
 * rpcrdma_wc_receive - Invoked by RDMA provider for each polled Receive WC
 * @cq:	completion queue (ignored)
 * @wc:	completed WR
 *
 */
static void
rpcrdma_wc_receive(struct ib_cq *cq, struct ib_wc *wc)
{
	struct ib_cqe *cqe = wc->wr_cqe;
	struct rpcrdma_rep *rep = container_of(cqe, struct rpcrdma_rep,
					       rr_cqe);

	/* WARNING: Only wr_id and status are reliable at this point */
	if (wc->status != IB_WC_SUCCESS)
		goto out_fail;

	/* status == SUCCESS means all fields in wc are trustworthy */
	if (wc->opcode != IB_WC_RECV)
		return;

	dprintk("RPC:       %s: rep %p opcode 'recv', length %u: success\n",
		__func__, rep, wc->byte_len);

	rep->rr_len = wc->byte_len;
	rep->rr_wc_flags = wc->wc_flags;
	rep->rr_inv_rkey = wc->ex.invalidate_rkey;

	ib_dma_sync_single_for_cpu(rep->rr_device,
				   rdmab_addr(rep->rr_rdmabuf),
				   rep->rr_len, DMA_FROM_DEVICE);

	rpcrdma_update_granted_credits(rep);

out_schedule:
	queue_work(rpcrdma_receive_wq, &rep->rr_work);
	return;

out_fail:
	if (wc->status != IB_WC_WR_FLUSH_ERR)
		pr_err("rpcrdma: Recv: %s (%u/0x%x)\n",
		       ib_wc_status_msg(wc->status),
		       wc->status, wc->vendor_err);
	rep->rr_len = RPCRDMA_BAD_LEN;
	goto out_schedule;
}

static void
rpcrdma_update_connect_private(struct rpcrdma_xprt *r_xprt,
			       struct rdma_conn_param *param)
{
	struct rpcrdma_create_data_internal *cdata = &r_xprt->rx_data;
	const struct rpcrdma_connect_private *pmsg = param->private_data;
	unsigned int rsize, wsize;

	/* Default settings for RPC-over-RDMA Version One */
	r_xprt->rx_ia.ri_reminv_expected = false;
	r_xprt->rx_ia.ri_implicit_roundup = xprt_rdma_pad_optimize;
	rsize = RPCRDMA_V1_DEF_INLINE_SIZE;
	wsize = RPCRDMA_V1_DEF_INLINE_SIZE;

	if (pmsg &&
	    pmsg->cp_magic == rpcrdma_cmp_magic &&
	    pmsg->cp_version == RPCRDMA_CMP_VERSION) {
		r_xprt->rx_ia.ri_reminv_expected = true;
		r_xprt->rx_ia.ri_implicit_roundup = true;
		rsize = rpcrdma_decode_buffer_size(pmsg->cp_send_size);
		wsize = rpcrdma_decode_buffer_size(pmsg->cp_recv_size);
	}

	if (rsize < cdata->inline_rsize)
		cdata->inline_rsize = rsize;
	if (wsize < cdata->inline_wsize)
		cdata->inline_wsize = wsize;
	dprintk("RPC:       %s: max send %u, max recv %u\n",
		__func__, cdata->inline_wsize, cdata->inline_rsize);
	rpcrdma_set_max_header_sizes(r_xprt);
}

>>>>>>> v4.9.227
static int
rpcrdma_conn_upcall(struct rdma_cm_id *id, struct rdma_cm_event *event)
{
	struct rpcrdma_xprt *xprt = id->context;
	struct rpcrdma_ia *ia = &xprt->rx_ia;
	struct rpcrdma_ep *ep = &xprt->rx_ep;
<<<<<<< HEAD
#ifdef RPC_DEBUG
	struct sockaddr_in *addr = (struct sockaddr_in *) &ep->rep_remote_addr;
#endif
	struct ib_qp_attr attr;
	struct ib_qp_init_attr iattr;
=======
#if IS_ENABLED(CONFIG_SUNRPC_DEBUG)
	struct sockaddr *sap = (struct sockaddr *)&ep->rep_remote_addr;
#endif
	struct ib_qp_attr *attr = &ia->ri_qp_attr;
	struct ib_qp_init_attr *iattr = &ia->ri_qp_init_attr;
>>>>>>> v4.9.227
	int connstate = 0;

	switch (event->event) {
	case RDMA_CM_EVENT_ADDR_RESOLVED:
	case RDMA_CM_EVENT_ROUTE_RESOLVED:
		ia->ri_async_rc = 0;
		complete(&ia->ri_done);
		break;
	case RDMA_CM_EVENT_ADDR_ERROR:
		ia->ri_async_rc = -EHOSTUNREACH;
		dprintk("RPC:       %s: CM address resolution error, ep 0x%p\n",
			__func__, ep);
		complete(&ia->ri_done);
		break;
	case RDMA_CM_EVENT_ROUTE_ERROR:
		ia->ri_async_rc = -ENETUNREACH;
		dprintk("RPC:       %s: CM route resolution error, ep 0x%p\n",
			__func__, ep);
		complete(&ia->ri_done);
		break;
	case RDMA_CM_EVENT_ESTABLISHED:
		connstate = 1;
<<<<<<< HEAD
		ib_query_qp(ia->ri_id->qp, &attr,
			IB_QP_MAX_QP_RD_ATOMIC | IB_QP_MAX_DEST_RD_ATOMIC,
			&iattr);
		dprintk("RPC:       %s: %d responder resources"
			" (%d initiator)\n",
			__func__, attr.max_dest_rd_atomic, attr.max_rd_atomic);
=======
		ib_query_qp(ia->ri_id->qp, attr,
			    IB_QP_MAX_QP_RD_ATOMIC | IB_QP_MAX_DEST_RD_ATOMIC,
			    iattr);
		dprintk("RPC:       %s: %d responder resources"
			" (%d initiator)\n",
			__func__, attr->max_dest_rd_atomic,
			attr->max_rd_atomic);
		rpcrdma_update_connect_private(xprt, &event->param.conn);
>>>>>>> v4.9.227
		goto connected;
	case RDMA_CM_EVENT_CONNECT_ERROR:
		connstate = -ENOTCONN;
		goto connected;
	case RDMA_CM_EVENT_UNREACHABLE:
		connstate = -ENETDOWN;
		goto connected;
	case RDMA_CM_EVENT_REJECTED:
		connstate = -ECONNREFUSED;
		goto connected;
	case RDMA_CM_EVENT_DISCONNECTED:
		connstate = -ECONNABORTED;
		goto connected;
	case RDMA_CM_EVENT_DEVICE_REMOVAL:
		connstate = -ENODEV;
connected:
<<<<<<< HEAD
		atomic_set(&rpcx_to_rdmax(ep->rep_xprt)->rx_buf.rb_credits, 1);
		dprintk("RPC:       %s: %sconnected\n",
					__func__, connstate > 0 ? "" : "dis");
		ep->rep_connected = connstate;
		ep->rep_func(ep);
		wake_up_all(&ep->rep_connect_wait);
		/*FALLTHROUGH*/
	default:
		dprintk("RPC:       %s: %pI4:%u (ep 0x%p): %s\n",
			__func__, &addr->sin_addr.s_addr,
			ntohs(addr->sin_port), ep,
			CONNECTION_MSG(event->event));
		break;
	}

#ifdef RPC_DEBUG
	if (connstate == 1) {
		int ird = attr.max_dest_rd_atomic;
		int tird = ep->rep_remote_cma.responder_resources;
		printk(KERN_INFO "rpcrdma: connection to %pI4:%u "
			"on %s, memreg %d slots %d ird %d%s\n",
			&addr->sin_addr.s_addr,
			ntohs(addr->sin_port),
			ia->ri_id->device->name,
			ia->ri_memreg_strategy,
			xprt->rx_buf.rb_max_requests,
			ird, ird < 4 && ird < tird / 2 ? " (low!)" : "");
	} else if (connstate < 0) {
		printk(KERN_INFO "rpcrdma: connection to %pI4:%u closed (%d)\n",
			&addr->sin_addr.s_addr,
			ntohs(addr->sin_port),
			connstate);
=======
		dprintk("RPC:       %s: %sconnected\n",
					__func__, connstate > 0 ? "" : "dis");
		atomic_set(&xprt->rx_buf.rb_credits, 1);
		ep->rep_connected = connstate;
		rpcrdma_conn_func(ep);
		wake_up_all(&ep->rep_connect_wait);
		/*FALLTHROUGH*/
	default:
		dprintk("RPC:       %s: %pIS:%u (ep 0x%p): %s\n",
			__func__, sap, rpc_get_port(sap), ep,
			rdma_event_msg(event->event));
		break;
	}

#if IS_ENABLED(CONFIG_SUNRPC_DEBUG)
	if (connstate == 1) {
		int ird = attr->max_dest_rd_atomic;
		int tird = ep->rep_remote_cma.responder_resources;

		pr_info("rpcrdma: connection to %pIS:%u on %s, memreg '%s', %d credits, %d responders%s\n",
			sap, rpc_get_port(sap),
			ia->ri_device->name,
			ia->ri_ops->ro_displayname,
			xprt->rx_buf.rb_max_requests,
			ird, ird < 4 && ird < tird / 2 ? " (low!)" : "");
	} else if (connstate < 0) {
		pr_info("rpcrdma: connection to %pIS:%u closed (%d)\n",
			sap, rpc_get_port(sap), connstate);
>>>>>>> v4.9.227
	}
#endif

	return 0;
}

<<<<<<< HEAD
=======
static void rpcrdma_destroy_id(struct rdma_cm_id *id)
{
	if (id) {
		module_put(id->device->owner);
		rdma_destroy_id(id);
	}
}

>>>>>>> v4.9.227
static struct rdma_cm_id *
rpcrdma_create_id(struct rpcrdma_xprt *xprt,
			struct rpcrdma_ia *ia, struct sockaddr *addr)
{
	struct rdma_cm_id *id;
	int rc;

	init_completion(&ia->ri_done);

<<<<<<< HEAD
	id = rdma_create_id(rpcrdma_conn_upcall, xprt, RDMA_PS_TCP, IB_QPT_RC);
=======
	id = rdma_create_id(&init_net, rpcrdma_conn_upcall, xprt, RDMA_PS_TCP,
			    IB_QPT_RC);
>>>>>>> v4.9.227
	if (IS_ERR(id)) {
		rc = PTR_ERR(id);
		dprintk("RPC:       %s: rdma_create_id() failed %i\n",
			__func__, rc);
		return id;
	}

	ia->ri_async_rc = -ETIMEDOUT;
	rc = rdma_resolve_addr(id, NULL, addr, RDMA_RESOLVE_TIMEOUT);
	if (rc) {
		dprintk("RPC:       %s: rdma_resolve_addr() failed %i\n",
			__func__, rc);
		goto out;
	}
	wait_for_completion_interruptible_timeout(&ia->ri_done,
				msecs_to_jiffies(RDMA_RESOLVE_TIMEOUT) + 1);
<<<<<<< HEAD
=======

	/* FIXME:
	 * Until xprtrdma supports DEVICE_REMOVAL, the provider must
	 * be pinned while there are active NFS/RDMA mounts to prevent
	 * hangs and crashes at umount time.
	 */
	if (!ia->ri_async_rc && !try_module_get(id->device->owner)) {
		dprintk("RPC:       %s: Failed to get device module\n",
			__func__);
		ia->ri_async_rc = -ENODEV;
	}
>>>>>>> v4.9.227
	rc = ia->ri_async_rc;
	if (rc)
		goto out;

	ia->ri_async_rc = -ETIMEDOUT;
	rc = rdma_resolve_route(id, RDMA_RESOLVE_TIMEOUT);
	if (rc) {
		dprintk("RPC:       %s: rdma_resolve_route() failed %i\n",
			__func__, rc);
<<<<<<< HEAD
		goto out;
=======
		goto put;
>>>>>>> v4.9.227
	}
	wait_for_completion_interruptible_timeout(&ia->ri_done,
				msecs_to_jiffies(RDMA_RESOLVE_TIMEOUT) + 1);
	rc = ia->ri_async_rc;
	if (rc)
<<<<<<< HEAD
		goto out;

	return id;

=======
		goto put;

	return id;
put:
	module_put(id->device->owner);
>>>>>>> v4.9.227
out:
	rdma_destroy_id(id);
	return ERR_PTR(rc);
}

/*
<<<<<<< HEAD
 * Drain any cq, prior to teardown.
 */
static void
rpcrdma_clean_cq(struct ib_cq *cq)
{
	struct ib_wc wc;
	int count = 0;

	while (1 == ib_poll_cq(cq, 1, &wc))
		++count;

	if (count)
		dprintk("RPC:       %s: flushed %d events (last 0x%x)\n",
			__func__, count, wc.opcode);
}

/*
=======
>>>>>>> v4.9.227
 * Exported functions.
 */

/*
 * Open and initialize an Interface Adapter.
 *  o initializes fields of struct rpcrdma_ia, including
 *    interface and provider attributes and protection zone.
 */
int
rpcrdma_ia_open(struct rpcrdma_xprt *xprt, struct sockaddr *addr, int memreg)
{
<<<<<<< HEAD
	int rc, mem_priv;
	struct rpcrdma_ia *ia = &xprt->rx_ia;
	struct ib_device_attr *devattr = &ia->ri_devattr;
=======
	struct rpcrdma_ia *ia = &xprt->rx_ia;
	int rc;
>>>>>>> v4.9.227

	ia->ri_id = rpcrdma_create_id(xprt, ia, addr);
	if (IS_ERR(ia->ri_id)) {
		rc = PTR_ERR(ia->ri_id);
		goto out1;
	}
<<<<<<< HEAD

	ia->ri_pd = ib_alloc_pd(ia->ri_id->device);
	if (IS_ERR(ia->ri_pd)) {
		rc = PTR_ERR(ia->ri_pd);
		dprintk("RPC:       %s: ib_alloc_pd() failed %i\n",
			__func__, rc);
		goto out2;
	}

	rc = ib_query_device(ia->ri_id->device, devattr);
	if (rc) {
		dprintk("RPC:       %s: ib_query_device failed %d\n",
			__func__, rc);
		goto out2;
	}

	if (devattr->device_cap_flags & IB_DEVICE_LOCAL_DMA_LKEY) {
		ia->ri_have_dma_lkey = 1;
		ia->ri_dma_lkey = ia->ri_id->device->local_dma_lkey;
	}

	if (memreg == RPCRDMA_FRMR) {
		/* Requires both frmr reg and local dma lkey */
		if (((devattr->device_cap_flags &
		     (IB_DEVICE_MEM_MGT_EXTENSIONS|IB_DEVICE_LOCAL_DMA_LKEY)) !=
		    (IB_DEVICE_MEM_MGT_EXTENSIONS|IB_DEVICE_LOCAL_DMA_LKEY)) ||
		      (devattr->max_fast_reg_page_list_len == 0)) {
			dprintk("RPC:       %s: FRMR registration "
				"not supported by HCA\n", __func__);
			memreg = RPCRDMA_MTHCAFMR;
		} else {
			/* Mind the ia limit on FRMR page list depth */
			ia->ri_max_frmr_depth = min_t(unsigned int,
				RPCRDMA_MAX_DATA_SEGS,
				devattr->max_fast_reg_page_list_len);
		}
	}
	if (memreg == RPCRDMA_MTHCAFMR) {
		if (!ia->ri_id->device->alloc_fmr) {
			dprintk("RPC:       %s: MTHCAFMR registration "
				"not supported by HCA\n", __func__);
			memreg = RPCRDMA_ALLPHYSICAL;
		}
	}

	/*
	 * Optionally obtain an underlying physical identity mapping in
	 * order to do a memory window-based bind. This base registration
	 * is protected from remote access - that is enabled only by binding
	 * for the specific bytes targeted during each RPC operation, and
	 * revoked after the corresponding completion similar to a storage
	 * adapter.
	 */
	switch (memreg) {
	case RPCRDMA_FRMR:
		break;
	case RPCRDMA_ALLPHYSICAL:
		mem_priv = IB_ACCESS_LOCAL_WRITE |
				IB_ACCESS_REMOTE_WRITE |
				IB_ACCESS_REMOTE_READ;
		goto register_setup;
	case RPCRDMA_MTHCAFMR:
		if (ia->ri_have_dma_lkey)
			break;
		mem_priv = IB_ACCESS_LOCAL_WRITE;
	register_setup:
		ia->ri_bind_mem = ib_get_dma_mr(ia->ri_pd, mem_priv);
		if (IS_ERR(ia->ri_bind_mem)) {
			printk(KERN_ALERT "%s: ib_get_dma_mr for "
				"phys register failed with %lX\n",
				__func__, PTR_ERR(ia->ri_bind_mem));
			rc = -ENOMEM;
			goto out2;
		}
		break;
	default:
		printk(KERN_ERR "RPC: Unsupported memory "
				"registration mode: %d\n", memreg);
		rc = -ENOMEM;
		goto out2;
	}
	dprintk("RPC:       %s: memory registration strategy is %d\n",
		__func__, memreg);

	/* Else will do memory reg/dereg for each chunk */
	ia->ri_memreg_strategy = memreg;

	rwlock_init(&ia->ri_qplock);
	return 0;
out2:
	rdma_destroy_id(ia->ri_id);
=======
	ia->ri_device = ia->ri_id->device;

	ia->ri_pd = ib_alloc_pd(ia->ri_device, 0);
	if (IS_ERR(ia->ri_pd)) {
		rc = PTR_ERR(ia->ri_pd);
		pr_err("rpcrdma: ib_alloc_pd() returned %d\n", rc);
		goto out2;
	}

	switch (memreg) {
	case RPCRDMA_FRMR:
		if (frwr_is_supported(ia)) {
			ia->ri_ops = &rpcrdma_frwr_memreg_ops;
			break;
		}
		/*FALLTHROUGH*/
	case RPCRDMA_MTHCAFMR:
		if (fmr_is_supported(ia)) {
			ia->ri_ops = &rpcrdma_fmr_memreg_ops;
			break;
		}
		/*FALLTHROUGH*/
	default:
		pr_err("rpcrdma: Unsupported memory registration mode: %d\n",
		       memreg);
		rc = -EINVAL;
		goto out3;
	}

	return 0;

out3:
	ib_dealloc_pd(ia->ri_pd);
	ia->ri_pd = NULL;
out2:
	rpcrdma_destroy_id(ia->ri_id);
>>>>>>> v4.9.227
	ia->ri_id = NULL;
out1:
	return rc;
}

/*
 * Clean up/close an IA.
 *   o if event handles and PD have been initialized, free them.
 *   o close the IA
 */
void
rpcrdma_ia_close(struct rpcrdma_ia *ia)
{
<<<<<<< HEAD
	int rc;

	dprintk("RPC:       %s: entering\n", __func__);
	if (ia->ri_bind_mem != NULL) {
		rc = ib_dereg_mr(ia->ri_bind_mem);
		dprintk("RPC:       %s: ib_dereg_mr returned %i\n",
			__func__, rc);
	}
	if (ia->ri_id != NULL && !IS_ERR(ia->ri_id)) {
		if (ia->ri_id->qp)
			rdma_destroy_qp(ia->ri_id);
		rdma_destroy_id(ia->ri_id);
		ia->ri_id = NULL;
	}
	if (ia->ri_pd != NULL && !IS_ERR(ia->ri_pd)) {
		rc = ib_dealloc_pd(ia->ri_pd);
		dprintk("RPC:       %s: ib_dealloc_pd returned %i\n",
			__func__, rc);
	}
=======
	dprintk("RPC:       %s: entering\n", __func__);
	if (ia->ri_id != NULL && !IS_ERR(ia->ri_id)) {
		if (ia->ri_id->qp)
			rdma_destroy_qp(ia->ri_id);
		rpcrdma_destroy_id(ia->ri_id);
		ia->ri_id = NULL;
	}

	/* If the pd is still busy, xprtrdma missed freeing a resource */
	if (ia->ri_pd && !IS_ERR(ia->ri_pd))
		ib_dealloc_pd(ia->ri_pd);
>>>>>>> v4.9.227
}

/*
 * Create unconnected endpoint.
 */
int
rpcrdma_ep_create(struct rpcrdma_ep *ep, struct rpcrdma_ia *ia,
<<<<<<< HEAD
				struct rpcrdma_create_data_internal *cdata)
{
	struct ib_device_attr *devattr = &ia->ri_devattr;
	struct ib_cq *sendcq, *recvcq;
	int rc, err;

	/* check provider's send/recv wr limits */
	if (cdata->max_requests > devattr->max_qp_wr)
		cdata->max_requests = devattr->max_qp_wr;

	ep->rep_attr.event_handler = rpcrdma_qp_async_error_upcall;
	ep->rep_attr.qp_context = ep;
	/* send_cq and recv_cq initialized below */
	ep->rep_attr.srq = NULL;
	ep->rep_attr.cap.max_send_wr = cdata->max_requests;
	switch (ia->ri_memreg_strategy) {
	case RPCRDMA_FRMR: {
		int depth = 7;

		/* Add room for frmr register and invalidate WRs.
		 * 1. FRMR reg WR for head
		 * 2. FRMR invalidate WR for head
		 * 3. N FRMR reg WRs for pagelist
		 * 4. N FRMR invalidate WRs for pagelist
		 * 5. FRMR reg WR for tail
		 * 6. FRMR invalidate WR for tail
		 * 7. The RDMA_SEND WR
		 */

		/* Calculate N if the device max FRMR depth is smaller than
		 * RPCRDMA_MAX_DATA_SEGS.
		 */
		if (ia->ri_max_frmr_depth < RPCRDMA_MAX_DATA_SEGS) {
			int delta = RPCRDMA_MAX_DATA_SEGS -
				    ia->ri_max_frmr_depth;

			do {
				depth += 2; /* FRMR reg + invalidate */
				delta -= ia->ri_max_frmr_depth;
			} while (delta > 0);

		}
		ep->rep_attr.cap.max_send_wr *= depth;
		if (ep->rep_attr.cap.max_send_wr > devattr->max_qp_wr) {
			cdata->max_requests = devattr->max_qp_wr / depth;
			if (!cdata->max_requests)
				return -EINVAL;
			ep->rep_attr.cap.max_send_wr = cdata->max_requests *
						       depth;
		}
		break;
	}
	default:
		break;
	}
	ep->rep_attr.cap.max_recv_wr = cdata->max_requests;
	ep->rep_attr.cap.max_send_sge = (cdata->padding ? 4 : 2);
=======
		  struct rpcrdma_create_data_internal *cdata)
{
	struct rpcrdma_connect_private *pmsg = &ep->rep_cm_private;
	unsigned int max_qp_wr, max_sge;
	struct ib_cq *sendcq, *recvcq;
	int rc;

	max_sge = min_t(unsigned int, ia->ri_device->attrs.max_sge,
			RPCRDMA_MAX_SEND_SGES);
	if (max_sge < RPCRDMA_MIN_SEND_SGES) {
		pr_warn("rpcrdma: HCA provides only %d send SGEs\n", max_sge);
		return -ENOMEM;
	}
	ia->ri_max_send_sges = max_sge - RPCRDMA_MIN_SEND_SGES;

	if (ia->ri_device->attrs.max_qp_wr <= RPCRDMA_BACKWARD_WRS) {
		dprintk("RPC:       %s: insufficient wqe's available\n",
			__func__);
		return -ENOMEM;
	}
	max_qp_wr = ia->ri_device->attrs.max_qp_wr - RPCRDMA_BACKWARD_WRS - 1;

	/* check provider's send/recv wr limits */
	if (cdata->max_requests > max_qp_wr)
		cdata->max_requests = max_qp_wr;

	ep->rep_attr.event_handler = rpcrdma_qp_async_error_upcall;
	ep->rep_attr.qp_context = ep;
	ep->rep_attr.srq = NULL;
	ep->rep_attr.cap.max_send_wr = cdata->max_requests;
	ep->rep_attr.cap.max_send_wr += RPCRDMA_BACKWARD_WRS;
	ep->rep_attr.cap.max_send_wr += 1;	/* drain cqe */
	rc = ia->ri_ops->ro_open(ia, ep, cdata);
	if (rc)
		return rc;
	ep->rep_attr.cap.max_recv_wr = cdata->max_requests;
	ep->rep_attr.cap.max_recv_wr += RPCRDMA_BACKWARD_WRS;
	ep->rep_attr.cap.max_recv_wr += 1;	/* drain cqe */
	ep->rep_attr.cap.max_send_sge = max_sge;
>>>>>>> v4.9.227
	ep->rep_attr.cap.max_recv_sge = 1;
	ep->rep_attr.cap.max_inline_data = 0;
	ep->rep_attr.sq_sig_type = IB_SIGNAL_REQ_WR;
	ep->rep_attr.qp_type = IB_QPT_RC;
	ep->rep_attr.port_num = ~0;

	dprintk("RPC:       %s: requested max: dtos: send %d recv %d; "
		"iovs: send %d recv %d\n",
		__func__,
		ep->rep_attr.cap.max_send_wr,
		ep->rep_attr.cap.max_recv_wr,
		ep->rep_attr.cap.max_send_sge,
		ep->rep_attr.cap.max_recv_sge);

	/* set trigger for requesting send completion */
	ep->rep_cqinit = ep->rep_attr.cap.max_send_wr/2 - 1;
	if (ep->rep_cqinit <= 2)
<<<<<<< HEAD
		ep->rep_cqinit = 0;
	INIT_CQCOUNT(ep);
	ep->rep_ia = ia;
	init_waitqueue_head(&ep->rep_connect_wait);
	INIT_DELAYED_WORK(&ep->rep_connect_worker, rpcrdma_connect_worker);

	sendcq = ib_create_cq(ia->ri_id->device, rpcrdma_sendcq_upcall,
				  rpcrdma_cq_async_error_upcall, ep,
				  ep->rep_attr.cap.max_send_wr + 1, 0);
=======
		ep->rep_cqinit = 0;	/* always signal? */
	rpcrdma_init_cqcount(ep, 0);
	init_waitqueue_head(&ep->rep_connect_wait);
	INIT_DELAYED_WORK(&ep->rep_connect_worker, rpcrdma_connect_worker);

	sendcq = ib_alloc_cq(ia->ri_device, NULL,
			     ep->rep_attr.cap.max_send_wr + 1,
			     0, IB_POLL_SOFTIRQ);
>>>>>>> v4.9.227
	if (IS_ERR(sendcq)) {
		rc = PTR_ERR(sendcq);
		dprintk("RPC:       %s: failed to create send CQ: %i\n",
			__func__, rc);
		goto out1;
	}

<<<<<<< HEAD
	rc = ib_req_notify_cq(sendcq, IB_CQ_NEXT_COMP);
	if (rc) {
		dprintk("RPC:       %s: ib_req_notify_cq failed: %i\n",
			__func__, rc);
		goto out2;
	}

	recvcq = ib_create_cq(ia->ri_id->device, rpcrdma_recvcq_upcall,
				  rpcrdma_cq_async_error_upcall, ep,
				  ep->rep_attr.cap.max_recv_wr + 1, 0);
=======
	recvcq = ib_alloc_cq(ia->ri_device, NULL,
			     ep->rep_attr.cap.max_recv_wr + 1,
			     0, IB_POLL_SOFTIRQ);
>>>>>>> v4.9.227
	if (IS_ERR(recvcq)) {
		rc = PTR_ERR(recvcq);
		dprintk("RPC:       %s: failed to create recv CQ: %i\n",
			__func__, rc);
		goto out2;
	}

<<<<<<< HEAD
	rc = ib_req_notify_cq(recvcq, IB_CQ_NEXT_COMP);
	if (rc) {
		dprintk("RPC:       %s: ib_req_notify_cq failed: %i\n",
			__func__, rc);
		ib_destroy_cq(recvcq);
		goto out2;
	}

=======
>>>>>>> v4.9.227
	ep->rep_attr.send_cq = sendcq;
	ep->rep_attr.recv_cq = recvcq;

	/* Initialize cma parameters */
<<<<<<< HEAD

	/* RPC/RDMA does not use private data */
	ep->rep_remote_cma.private_data = NULL;
	ep->rep_remote_cma.private_data_len = 0;

	/* Client offers RDMA Read but does not initiate */
	ep->rep_remote_cma.initiator_depth = 0;
	if (devattr->max_qp_rd_atom > 32)	/* arbitrary but <= 255 */
		ep->rep_remote_cma.responder_resources = 32;
	else
		ep->rep_remote_cma.responder_resources =
						devattr->max_qp_rd_atom;

	ep->rep_remote_cma.retry_count = 7;
=======
	memset(&ep->rep_remote_cma, 0, sizeof(ep->rep_remote_cma));

	/* Prepare RDMA-CM private message */
	pmsg->cp_magic = rpcrdma_cmp_magic;
	pmsg->cp_version = RPCRDMA_CMP_VERSION;
	pmsg->cp_flags |= ia->ri_ops->ro_send_w_inv_ok;
	pmsg->cp_send_size = rpcrdma_encode_buffer_size(cdata->inline_wsize);
	pmsg->cp_recv_size = rpcrdma_encode_buffer_size(cdata->inline_rsize);
	ep->rep_remote_cma.private_data = pmsg;
	ep->rep_remote_cma.private_data_len = sizeof(*pmsg);

	/* Client offers RDMA Read but does not initiate */
	ep->rep_remote_cma.initiator_depth = 0;
	if (ia->ri_device->attrs.max_qp_rd_atom > 32)	/* arbitrary but <= 255 */
		ep->rep_remote_cma.responder_resources = 32;
	else
		ep->rep_remote_cma.responder_resources =
						ia->ri_device->attrs.max_qp_rd_atom;

	/* Limit transport retries so client can detect server
	 * GID changes quickly. RPC layer handles re-establishing
	 * transport connection and retransmission.
	 */
	ep->rep_remote_cma.retry_count = 6;

	/* RPC-over-RDMA handles its own flow control. In addition,
	 * make all RNR NAKs visible so we know that RPC-over-RDMA
	 * flow control is working correctly (no NAKs should be seen).
	 */
>>>>>>> v4.9.227
	ep->rep_remote_cma.flow_control = 0;
	ep->rep_remote_cma.rnr_retry_count = 0;

	return 0;

out2:
<<<<<<< HEAD
	err = ib_destroy_cq(sendcq);
	if (err)
		dprintk("RPC:       %s: ib_destroy_cq returned %i\n",
			__func__, err);
=======
	ib_free_cq(sendcq);
>>>>>>> v4.9.227
out1:
	return rc;
}

/*
 * rpcrdma_ep_destroy
 *
 * Disconnect and destroy endpoint. After this, the only
 * valid operations on the ep are to free it (if dynamically
 * allocated) or re-create it.
 */
void
rpcrdma_ep_destroy(struct rpcrdma_ep *ep, struct rpcrdma_ia *ia)
{
<<<<<<< HEAD
	int rc;

=======
>>>>>>> v4.9.227
	dprintk("RPC:       %s: entering, connected is %d\n",
		__func__, ep->rep_connected);

	cancel_delayed_work_sync(&ep->rep_connect_worker);

	if (ia->ri_id->qp) {
		rpcrdma_ep_disconnect(ep, ia);
		rdma_destroy_qp(ia->ri_id);
		ia->ri_id->qp = NULL;
	}

<<<<<<< HEAD
	/* padding - could be done in rpcrdma_buffer_destroy... */
	if (ep->rep_pad_mr) {
		rpcrdma_deregister_internal(ia, ep->rep_pad_mr, &ep->rep_pad);
		ep->rep_pad_mr = NULL;
	}

	rpcrdma_clean_cq(ep->rep_attr.recv_cq);
	rc = ib_destroy_cq(ep->rep_attr.recv_cq);
	if (rc)
		dprintk("RPC:       %s: ib_destroy_cq returned %i\n",
			__func__, rc);

	rpcrdma_clean_cq(ep->rep_attr.send_cq);
	rc = ib_destroy_cq(ep->rep_attr.send_cq);
	if (rc)
		dprintk("RPC:       %s: ib_destroy_cq returned %i\n",
			__func__, rc);
=======
	ib_free_cq(ep->rep_attr.recv_cq);
	ib_free_cq(ep->rep_attr.send_cq);
>>>>>>> v4.9.227
}

/*
 * Connect unconnected endpoint.
 */
int
rpcrdma_ep_connect(struct rpcrdma_ep *ep, struct rpcrdma_ia *ia)
{
	struct rdma_cm_id *id, *old;
	int rc = 0;
	int retry_count = 0;

	if (ep->rep_connected != 0) {
		struct rpcrdma_xprt *xprt;
retry:
		dprintk("RPC:       %s: reconnecting...\n", __func__);

		rpcrdma_ep_disconnect(ep, ia);
<<<<<<< HEAD
		rpcrdma_flush_cqs(ep);

		if (ia->ri_memreg_strategy == RPCRDMA_FRMR)
			rpcrdma_reset_frmrs(ia);
=======
>>>>>>> v4.9.227

		xprt = container_of(ia, struct rpcrdma_xprt, rx_ia);
		id = rpcrdma_create_id(xprt, ia,
				(struct sockaddr *)&xprt->rx_data.addr);
		if (IS_ERR(id)) {
			rc = -EHOSTUNREACH;
			goto out;
		}
		/* TEMP TEMP TEMP - fail if new device:
		 * Deregister/remarshal *all* requests!
		 * Close and recreate adapter, pd, etc!
		 * Re-determine all attributes still sane!
		 * More stuff I haven't thought of!
		 * Rrrgh!
		 */
<<<<<<< HEAD
		if (ia->ri_id->device != id->device) {
			printk("RPC:       %s: can't reconnect on "
				"different device!\n", __func__);
			rdma_destroy_id(id);
=======
		if (ia->ri_device != id->device) {
			printk("RPC:       %s: can't reconnect on "
				"different device!\n", __func__);
			rpcrdma_destroy_id(id);
>>>>>>> v4.9.227
			rc = -ENETUNREACH;
			goto out;
		}
		/* END TEMP */
		rc = rdma_create_qp(id, ia->ri_pd, &ep->rep_attr);
		if (rc) {
			dprintk("RPC:       %s: rdma_create_qp failed %i\n",
				__func__, rc);
<<<<<<< HEAD
			rdma_destroy_id(id);
=======
			rpcrdma_destroy_id(id);
>>>>>>> v4.9.227
			rc = -ENETUNREACH;
			goto out;
		}

<<<<<<< HEAD
		write_lock(&ia->ri_qplock);
		old = ia->ri_id;
		ia->ri_id = id;
		write_unlock(&ia->ri_qplock);

		rdma_destroy_qp(old);
		rdma_destroy_id(old);
=======
		old = ia->ri_id;
		ia->ri_id = id;

		rdma_destroy_qp(old);
		rpcrdma_destroy_id(old);
>>>>>>> v4.9.227
	} else {
		dprintk("RPC:       %s: connecting...\n", __func__);
		rc = rdma_create_qp(ia->ri_id, ia->ri_pd, &ep->rep_attr);
		if (rc) {
			dprintk("RPC:       %s: rdma_create_qp failed %i\n",
				__func__, rc);
			/* do not update ep->rep_connected */
			return -ENETUNREACH;
		}
	}

	ep->rep_connected = 0;

	rc = rdma_connect(ia->ri_id, &ep->rep_remote_cma);
	if (rc) {
		dprintk("RPC:       %s: rdma_connect() failed with %i\n",
				__func__, rc);
		goto out;
	}

	wait_event_interruptible(ep->rep_connect_wait, ep->rep_connected != 0);

	/*
	 * Check state. A non-peer reject indicates no listener
	 * (ECONNREFUSED), which may be a transient state. All
	 * others indicate a transport condition which has already
	 * undergone a best-effort.
	 */
	if (ep->rep_connected == -ECONNREFUSED &&
	    ++retry_count <= RDMA_CONNECT_RETRY_MAX) {
		dprintk("RPC:       %s: non-peer_reject, retry\n", __func__);
		goto retry;
	}
	if (ep->rep_connected <= 0) {
		/* Sometimes, the only way to reliably connect to remote
		 * CMs is to use same nonzero values for ORD and IRD. */
		if (retry_count++ <= RDMA_CONNECT_RETRY_MAX + 1 &&
		    (ep->rep_remote_cma.responder_resources == 0 ||
		     ep->rep_remote_cma.initiator_depth !=
				ep->rep_remote_cma.responder_resources)) {
			if (ep->rep_remote_cma.responder_resources == 0)
				ep->rep_remote_cma.responder_resources = 1;
			ep->rep_remote_cma.initiator_depth =
				ep->rep_remote_cma.responder_resources;
			goto retry;
		}
		rc = ep->rep_connected;
	} else {
<<<<<<< HEAD
		dprintk("RPC:       %s: connected\n", __func__);
=======
		struct rpcrdma_xprt *r_xprt;
		unsigned int extras;

		dprintk("RPC:       %s: connected\n", __func__);

		r_xprt = container_of(ia, struct rpcrdma_xprt, rx_ia);
		extras = r_xprt->rx_buf.rb_bc_srv_max_requests;

		if (extras) {
			rc = rpcrdma_ep_post_extra_recv(r_xprt, extras);
			if (rc) {
				pr_warn("%s: rpcrdma_ep_post_extra_recv: %i\n",
					__func__, rc);
				rc = 0;
			}
		}
>>>>>>> v4.9.227
	}

out:
	if (rc)
		ep->rep_connected = rc;
	return rc;
}

/*
 * rpcrdma_ep_disconnect
 *
 * This is separate from destroy to facilitate the ability
 * to reconnect without recreating the endpoint.
 *
 * This call is not reentrant, and must not be made in parallel
 * on the same endpoint.
 */
void
rpcrdma_ep_disconnect(struct rpcrdma_ep *ep, struct rpcrdma_ia *ia)
{
	int rc;

<<<<<<< HEAD
	rpcrdma_flush_cqs(ep);
=======
>>>>>>> v4.9.227
	rc = rdma_disconnect(ia->ri_id);
	if (!rc) {
		/* returns without wait if not connected */
		wait_event_interruptible(ep->rep_connect_wait,
							ep->rep_connected != 1);
		dprintk("RPC:       %s: after wait, %sconnected\n", __func__,
			(ep->rep_connected == 1) ? "still " : "dis");
	} else {
		dprintk("RPC:       %s: rdma_disconnect %i\n", __func__, rc);
		ep->rep_connected = rc;
	}
<<<<<<< HEAD
}

static int
rpcrdma_init_fmrs(struct rpcrdma_ia *ia, struct rpcrdma_buffer *buf)
{
	int mr_access_flags = IB_ACCESS_REMOTE_WRITE | IB_ACCESS_REMOTE_READ;
	struct ib_fmr_attr fmr_attr = {
		.max_pages	= RPCRDMA_MAX_DATA_SEGS,
		.max_maps	= 1,
		.page_shift	= PAGE_SHIFT
	};
	struct rpcrdma_mw *r;
	int i, rc;

	i = (buf->rb_max_requests + 1) * RPCRDMA_MAX_SEGS;
	dprintk("RPC:       %s: initalizing %d FMRs\n", __func__, i);

	while (i--) {
		r = kzalloc(sizeof(*r), GFP_KERNEL);
		if (r == NULL)
			return -ENOMEM;

		r->r.fmr = ib_alloc_fmr(ia->ri_pd, mr_access_flags, &fmr_attr);
		if (IS_ERR(r->r.fmr)) {
			rc = PTR_ERR(r->r.fmr);
			dprintk("RPC:       %s: ib_alloc_fmr failed %i\n",
				__func__, rc);
			goto out_free;
		}

		list_add(&r->mw_list, &buf->rb_mws);
		list_add(&r->mw_all, &buf->rb_all);
	}
	return 0;

out_free:
	kfree(r);
	return rc;
}

static int
rpcrdma_init_frmrs(struct rpcrdma_ia *ia, struct rpcrdma_buffer *buf)
{
	struct rpcrdma_frmr *f;
	struct rpcrdma_mw *r;
	int i, rc;

	i = (buf->rb_max_requests + 1) * RPCRDMA_MAX_SEGS;
	dprintk("RPC:       %s: initalizing %d FRMRs\n", __func__, i);

	while (i--) {
		r = kzalloc(sizeof(*r), GFP_KERNEL);
		if (r == NULL)
			return -ENOMEM;
		f = &r->r.frmr;

		f->fr_mr = ib_alloc_fast_reg_mr(ia->ri_pd,
						ia->ri_max_frmr_depth);
		if (IS_ERR(f->fr_mr)) {
			rc = PTR_ERR(f->fr_mr);
			dprintk("RPC:       %s: ib_alloc_fast_reg_mr "
				"failed %i\n", __func__, rc);
			goto out_free;
		}

		f->fr_pgl = ib_alloc_fast_reg_page_list(ia->ri_id->device,
							ia->ri_max_frmr_depth);
		if (IS_ERR(f->fr_pgl)) {
			rc = PTR_ERR(f->fr_pgl);
			dprintk("RPC:       %s: ib_alloc_fast_reg_page_list "
				"failed %i\n", __func__, rc);

			ib_dereg_mr(f->fr_mr);
			goto out_free;
		}

		list_add(&r->mw_list, &buf->rb_mws);
		list_add(&r->mw_all, &buf->rb_all);
	}

	return 0;

out_free:
	kfree(r);
=======

	ib_drain_qp(ia->ri_id->qp);
}

static void
rpcrdma_mr_recovery_worker(struct work_struct *work)
{
	struct rpcrdma_buffer *buf = container_of(work, struct rpcrdma_buffer,
						  rb_recovery_worker.work);
	struct rpcrdma_mw *mw;

	spin_lock(&buf->rb_recovery_lock);
	while (!list_empty(&buf->rb_stale_mrs)) {
		mw = list_first_entry(&buf->rb_stale_mrs,
				      struct rpcrdma_mw, mw_list);
		list_del_init(&mw->mw_list);
		spin_unlock(&buf->rb_recovery_lock);

		dprintk("RPC:       %s: recovering MR %p\n", __func__, mw);
		mw->mw_xprt->rx_ia.ri_ops->ro_recover_mr(mw);

		spin_lock(&buf->rb_recovery_lock);
	}
	spin_unlock(&buf->rb_recovery_lock);
}

void
rpcrdma_defer_mr_recovery(struct rpcrdma_mw *mw)
{
	struct rpcrdma_xprt *r_xprt = mw->mw_xprt;
	struct rpcrdma_buffer *buf = &r_xprt->rx_buf;

	spin_lock(&buf->rb_recovery_lock);
	list_add(&mw->mw_list, &buf->rb_stale_mrs);
	spin_unlock(&buf->rb_recovery_lock);

	schedule_delayed_work(&buf->rb_recovery_worker, 0);
}

static void
rpcrdma_create_mrs(struct rpcrdma_xprt *r_xprt)
{
	struct rpcrdma_buffer *buf = &r_xprt->rx_buf;
	struct rpcrdma_ia *ia = &r_xprt->rx_ia;
	unsigned int count;
	LIST_HEAD(free);
	LIST_HEAD(all);

	for (count = 0; count < 32; count++) {
		struct rpcrdma_mw *mw;
		int rc;

		mw = kzalloc(sizeof(*mw), GFP_KERNEL);
		if (!mw)
			break;

		rc = ia->ri_ops->ro_init_mr(ia, mw);
		if (rc) {
			kfree(mw);
			break;
		}

		mw->mw_xprt = r_xprt;

		list_add(&mw->mw_list, &free);
		list_add(&mw->mw_all, &all);
	}

	spin_lock(&buf->rb_mwlock);
	list_splice(&free, &buf->rb_mws);
	list_splice(&all, &buf->rb_all);
	r_xprt->rx_stats.mrs_allocated += count;
	spin_unlock(&buf->rb_mwlock);

	dprintk("RPC:       %s: created %u MRs\n", __func__, count);
}

static void
rpcrdma_mr_refresh_worker(struct work_struct *work)
{
	struct rpcrdma_buffer *buf = container_of(work, struct rpcrdma_buffer,
						  rb_refresh_worker.work);
	struct rpcrdma_xprt *r_xprt = container_of(buf, struct rpcrdma_xprt,
						   rx_buf);

	rpcrdma_create_mrs(r_xprt);
}

struct rpcrdma_req *
rpcrdma_create_req(struct rpcrdma_xprt *r_xprt)
{
	struct rpcrdma_buffer *buffer = &r_xprt->rx_buf;
	struct rpcrdma_req *req;

	req = kzalloc(sizeof(*req), GFP_KERNEL);
	if (req == NULL)
		return ERR_PTR(-ENOMEM);

	INIT_LIST_HEAD(&req->rl_free);
	spin_lock(&buffer->rb_reqslock);
	list_add(&req->rl_all, &buffer->rb_allreqs);
	spin_unlock(&buffer->rb_reqslock);
	req->rl_cqe.done = rpcrdma_wc_send;
	req->rl_buffer = &r_xprt->rx_buf;
	INIT_LIST_HEAD(&req->rl_registered);
	req->rl_send_wr.next = NULL;
	req->rl_send_wr.wr_cqe = &req->rl_cqe;
	req->rl_send_wr.sg_list = req->rl_send_sge;
	req->rl_send_wr.opcode = IB_WR_SEND;
	return req;
}

/**
 * rpcrdma_create_rep - Allocate an rpcrdma_rep object
 * @r_xprt: controlling transport
 *
 * Returns 0 on success or a negative errno on failure.
 */
int
 rpcrdma_create_rep(struct rpcrdma_xprt *r_xprt)
{
	struct rpcrdma_create_data_internal *cdata = &r_xprt->rx_data;
	struct rpcrdma_buffer *buf = &r_xprt->rx_buf;
	struct rpcrdma_ia *ia = &r_xprt->rx_ia;
	struct rpcrdma_rep *rep;
	int rc;

	rc = -ENOMEM;
	rep = kzalloc(sizeof(*rep), GFP_KERNEL);
	if (rep == NULL)
		goto out;

	rep->rr_rdmabuf = rpcrdma_alloc_regbuf(cdata->inline_rsize,
					       DMA_FROM_DEVICE, GFP_KERNEL);
	if (IS_ERR(rep->rr_rdmabuf)) {
		rc = PTR_ERR(rep->rr_rdmabuf);
		goto out_free;
	}

	rep->rr_device = ia->ri_device;
	rep->rr_cqe.done = rpcrdma_wc_receive;
	rep->rr_rxprt = r_xprt;
	INIT_WORK(&rep->rr_work, rpcrdma_reply_handler);
	rep->rr_recv_wr.next = NULL;
	rep->rr_recv_wr.wr_cqe = &rep->rr_cqe;
	rep->rr_recv_wr.sg_list = &rep->rr_rdmabuf->rg_iov;
	rep->rr_recv_wr.num_sge = 1;

	spin_lock(&buf->rb_lock);
	list_add(&rep->rr_list, &buf->rb_recv_bufs);
	spin_unlock(&buf->rb_lock);
	return 0;

out_free:
	kfree(rep);
out:
	dprintk("RPC:       %s: reply buffer %d alloc failed\n",
		__func__, rc);
>>>>>>> v4.9.227
	return rc;
}

int
<<<<<<< HEAD
rpcrdma_buffer_create(struct rpcrdma_buffer *buf, struct rpcrdma_ep *ep,
	struct rpcrdma_ia *ia, struct rpcrdma_create_data_internal *cdata)
{
	char *p;
	size_t len, rlen, wlen;
	int i, rc;

	buf->rb_max_requests = cdata->max_requests;
	spin_lock_init(&buf->rb_lock);
	atomic_set(&buf->rb_credits, 1);

	/* Need to allocate:
	 *   1.  arrays for send and recv pointers
	 *   2.  arrays of struct rpcrdma_req to fill in pointers
	 *   3.  array of struct rpcrdma_rep for replies
	 *   4.  padding, if any
	 * Send/recv buffers in req/rep need to be registered
	 */
	len = buf->rb_max_requests *
		(sizeof(struct rpcrdma_req *) + sizeof(struct rpcrdma_rep *));
	len += cdata->padding;

	p = kzalloc(len, GFP_KERNEL);
	if (p == NULL) {
		dprintk("RPC:       %s: req_t/rep_t/pad kzalloc(%zd) failed\n",
			__func__, len);
		rc = -ENOMEM;
		goto out;
	}
	buf->rb_pool = p;	/* for freeing it later */

	buf->rb_send_bufs = (struct rpcrdma_req **) p;
	p = (char *) &buf->rb_send_bufs[buf->rb_max_requests];
	buf->rb_recv_bufs = (struct rpcrdma_rep **) p;
	p = (char *) &buf->rb_recv_bufs[buf->rb_max_requests];

	/*
	 * Register the zeroed pad buffer, if any.
	 */
	if (cdata->padding) {
		rc = rpcrdma_register_internal(ia, p, cdata->padding,
					    &ep->rep_pad_mr, &ep->rep_pad);
		if (rc)
			goto out;
	}
	p += cdata->padding;

	INIT_LIST_HEAD(&buf->rb_mws);
	INIT_LIST_HEAD(&buf->rb_all);
	switch (ia->ri_memreg_strategy) {
	case RPCRDMA_FRMR:
		rc = rpcrdma_init_frmrs(ia, buf);
		if (rc)
			goto out;
		break;
	case RPCRDMA_MTHCAFMR:
		rc = rpcrdma_init_fmrs(ia, buf);
		if (rc)
			goto out;
		break;
	default:
		break;
	}

	/*
	 * Allocate/init the request/reply buffers. Doing this
	 * using kmalloc for now -- one for each buf.
	 */
	wlen = 1 << fls(cdata->inline_wsize + sizeof(struct rpcrdma_req));
	rlen = 1 << fls(cdata->inline_rsize + sizeof(struct rpcrdma_rep));
	dprintk("RPC:       %s: wlen = %zu, rlen = %zu\n",
		__func__, wlen, rlen);

	for (i = 0; i < buf->rb_max_requests; i++) {
		struct rpcrdma_req *req;
		struct rpcrdma_rep *rep;

		req = kmalloc(wlen, GFP_KERNEL);
		if (req == NULL) {
			dprintk("RPC:       %s: request buffer %d alloc"
				" failed\n", __func__, i);
			rc = -ENOMEM;
			goto out;
		}
		memset(req, 0, sizeof(struct rpcrdma_req));
		buf->rb_send_bufs[i] = req;
		buf->rb_send_bufs[i]->rl_buffer = buf;

		rc = rpcrdma_register_internal(ia, req->rl_base,
				wlen - offsetof(struct rpcrdma_req, rl_base),
				&buf->rb_send_bufs[i]->rl_handle,
				&buf->rb_send_bufs[i]->rl_iov);
		if (rc)
			goto out;

		buf->rb_send_bufs[i]->rl_size = wlen -
						sizeof(struct rpcrdma_req);

		rep = kmalloc(rlen, GFP_KERNEL);
		if (rep == NULL) {
			dprintk("RPC:       %s: reply buffer %d alloc failed\n",
				__func__, i);
			rc = -ENOMEM;
			goto out;
		}
		memset(rep, 0, sizeof(struct rpcrdma_rep));
		buf->rb_recv_bufs[i] = rep;
		buf->rb_recv_bufs[i]->rr_buffer = buf;

		rc = rpcrdma_register_internal(ia, rep->rr_base,
				rlen - offsetof(struct rpcrdma_rep, rr_base),
				&buf->rb_recv_bufs[i]->rr_handle,
				&buf->rb_recv_bufs[i]->rr_iov);
		if (rc)
			goto out;

	}
	dprintk("RPC:       %s: max_requests %d\n",
		__func__, buf->rb_max_requests);
	/* done */
=======
rpcrdma_buffer_create(struct rpcrdma_xprt *r_xprt)
{
	struct rpcrdma_buffer *buf = &r_xprt->rx_buf;
	int i, rc;

	buf->rb_max_requests = r_xprt->rx_data.max_requests;
	buf->rb_bc_srv_max_requests = 0;
	atomic_set(&buf->rb_credits, 1);
	spin_lock_init(&buf->rb_mwlock);
	spin_lock_init(&buf->rb_lock);
	spin_lock_init(&buf->rb_recovery_lock);
	INIT_LIST_HEAD(&buf->rb_mws);
	INIT_LIST_HEAD(&buf->rb_all);
	INIT_LIST_HEAD(&buf->rb_stale_mrs);
	INIT_DELAYED_WORK(&buf->rb_refresh_worker,
			  rpcrdma_mr_refresh_worker);
	INIT_DELAYED_WORK(&buf->rb_recovery_worker,
			  rpcrdma_mr_recovery_worker);

	rpcrdma_create_mrs(r_xprt);

	INIT_LIST_HEAD(&buf->rb_send_bufs);
	INIT_LIST_HEAD(&buf->rb_allreqs);
	spin_lock_init(&buf->rb_reqslock);
	for (i = 0; i < buf->rb_max_requests; i++) {
		struct rpcrdma_req *req;

		req = rpcrdma_create_req(r_xprt);
		if (IS_ERR(req)) {
			dprintk("RPC:       %s: request buffer %d alloc"
				" failed\n", __func__, i);
			rc = PTR_ERR(req);
			goto out;
		}
		req->rl_backchannel = false;
		list_add(&req->rl_free, &buf->rb_send_bufs);
	}

	INIT_LIST_HEAD(&buf->rb_recv_bufs);
	for (i = 0; i <= buf->rb_max_requests; i++) {
		rc = rpcrdma_create_rep(r_xprt);
		if (rc)
			goto out;
	}

>>>>>>> v4.9.227
	return 0;
out:
	rpcrdma_buffer_destroy(buf);
	return rc;
}

<<<<<<< HEAD
static void
rpcrdma_destroy_fmrs(struct rpcrdma_buffer *buf)
{
	struct rpcrdma_mw *r;
	int rc;

	while (!list_empty(&buf->rb_all)) {
		r = list_entry(buf->rb_all.next, struct rpcrdma_mw, mw_all);
		list_del(&r->mw_all);
		list_del(&r->mw_list);

		rc = ib_dealloc_fmr(r->r.fmr);
		if (rc)
			dprintk("RPC:       %s: ib_dealloc_fmr failed %i\n",
				__func__, rc);

		kfree(r);
	}
}

static void
rpcrdma_destroy_frmrs(struct rpcrdma_buffer *buf)
{
	struct rpcrdma_mw *r;
	int rc;

	while (!list_empty(&buf->rb_all)) {
		r = list_entry(buf->rb_all.next, struct rpcrdma_mw, mw_all);
		list_del(&r->mw_all);
		list_del(&r->mw_list);

		rc = ib_dereg_mr(r->r.frmr.fr_mr);
		if (rc)
			dprintk("RPC:       %s: ib_dereg_mr failed %i\n",
				__func__, rc);
		ib_free_fast_reg_page_list(r->r.frmr.fr_pgl);

		kfree(r);
	}
=======
static struct rpcrdma_req *
rpcrdma_buffer_get_req_locked(struct rpcrdma_buffer *buf)
{
	struct rpcrdma_req *req;

	req = list_first_entry(&buf->rb_send_bufs,
			       struct rpcrdma_req, rl_free);
	list_del(&req->rl_free);
	return req;
}

static struct rpcrdma_rep *
rpcrdma_buffer_get_rep_locked(struct rpcrdma_buffer *buf)
{
	struct rpcrdma_rep *rep;

	rep = list_first_entry(&buf->rb_recv_bufs,
			       struct rpcrdma_rep, rr_list);
	list_del(&rep->rr_list);
	return rep;
}

static void
rpcrdma_destroy_rep(struct rpcrdma_rep *rep)
{
	rpcrdma_free_regbuf(rep->rr_rdmabuf);
	kfree(rep);
}

void
rpcrdma_destroy_req(struct rpcrdma_req *req)
{
	rpcrdma_free_regbuf(req->rl_recvbuf);
	rpcrdma_free_regbuf(req->rl_sendbuf);
	rpcrdma_free_regbuf(req->rl_rdmabuf);
	kfree(req);
}

static void
rpcrdma_destroy_mrs(struct rpcrdma_buffer *buf)
{
	struct rpcrdma_xprt *r_xprt = container_of(buf, struct rpcrdma_xprt,
						   rx_buf);
	struct rpcrdma_ia *ia = rdmab_to_ia(buf);
	struct rpcrdma_mw *mw;
	unsigned int count;

	count = 0;
	spin_lock(&buf->rb_mwlock);
	while (!list_empty(&buf->rb_all)) {
		mw = list_entry(buf->rb_all.next, struct rpcrdma_mw, mw_all);
		list_del(&mw->mw_all);

		spin_unlock(&buf->rb_mwlock);
		ia->ri_ops->ro_release_mr(mw);
		count++;
		spin_lock(&buf->rb_mwlock);
	}
	spin_unlock(&buf->rb_mwlock);
	r_xprt->rx_stats.mrs_allocated = 0;

	dprintk("RPC:       %s: released %u MRs\n", __func__, count);
>>>>>>> v4.9.227
}

void
rpcrdma_buffer_destroy(struct rpcrdma_buffer *buf)
{
<<<<<<< HEAD
	struct rpcrdma_ia *ia = rdmab_to_ia(buf);
	int i;

	/* clean up in reverse order from create
	 *   1.  recv mr memory (mr free, then kfree)
	 *   2.  send mr memory (mr free, then kfree)
	 *   3.  MWs
	 */
	dprintk("RPC:       %s: entering\n", __func__);

	for (i = 0; i < buf->rb_max_requests; i++) {
		if (buf->rb_recv_bufs && buf->rb_recv_bufs[i]) {
			rpcrdma_deregister_internal(ia,
					buf->rb_recv_bufs[i]->rr_handle,
					&buf->rb_recv_bufs[i]->rr_iov);
			kfree(buf->rb_recv_bufs[i]);
		}
		if (buf->rb_send_bufs && buf->rb_send_bufs[i]) {
			rpcrdma_deregister_internal(ia,
					buf->rb_send_bufs[i]->rl_handle,
					&buf->rb_send_bufs[i]->rl_iov);
			kfree(buf->rb_send_bufs[i]);
		}
	}

	switch (ia->ri_memreg_strategy) {
	case RPCRDMA_FRMR:
		rpcrdma_destroy_frmrs(buf);
		break;
	case RPCRDMA_MTHCAFMR:
		rpcrdma_destroy_fmrs(buf);
		break;
	default:
		break;
	}

	kfree(buf->rb_pool);
}

/* After a disconnect, a flushed FAST_REG_MR can leave an FRMR in
 * an unusable state. Find FRMRs in this state and dereg / reg
 * each.  FRMRs that are VALID and attached to an rpcrdma_req are
 * also torn down.
 *
 * This gives all in-use FRMRs a fresh rkey and leaves them INVALID.
 *
 * This is invoked only in the transport connect worker in order
 * to serialize with rpcrdma_register_frmr_external().
 */
static void
rpcrdma_reset_frmrs(struct rpcrdma_ia *ia)
{
	struct rpcrdma_xprt *r_xprt =
				container_of(ia, struct rpcrdma_xprt, rx_ia);
	struct rpcrdma_buffer *buf = &r_xprt->rx_buf;
	struct list_head *pos;
	struct rpcrdma_mw *r;
	int rc;

	list_for_each(pos, &buf->rb_all) {
		r = list_entry(pos, struct rpcrdma_mw, mw_all);

		if (r->r.frmr.fr_state == FRMR_IS_INVALID)
			continue;

		rc = ib_dereg_mr(r->r.frmr.fr_mr);
		if (rc)
			dprintk("RPC:       %s: ib_dereg_mr failed %i\n",
				__func__, rc);
		ib_free_fast_reg_page_list(r->r.frmr.fr_pgl);

		r->r.frmr.fr_mr = ib_alloc_fast_reg_mr(ia->ri_pd,
					ia->ri_max_frmr_depth);
		if (IS_ERR(r->r.frmr.fr_mr)) {
			rc = PTR_ERR(r->r.frmr.fr_mr);
			dprintk("RPC:       %s: ib_alloc_fast_reg_mr"
				" failed %i\n", __func__, rc);
			continue;
		}
		r->r.frmr.fr_pgl = ib_alloc_fast_reg_page_list(
					ia->ri_id->device,
					ia->ri_max_frmr_depth);
		if (IS_ERR(r->r.frmr.fr_pgl)) {
			rc = PTR_ERR(r->r.frmr.fr_pgl);
			dprintk("RPC:       %s: "
				"ib_alloc_fast_reg_page_list "
				"failed %i\n", __func__, rc);

			ib_dereg_mr(r->r.frmr.fr_mr);
			continue;
		}
		r->r.frmr.fr_state = FRMR_IS_INVALID;
	}
}

/* "*mw" can be NULL when rpcrdma_buffer_get_mrs() fails, leaving
 * some req segments uninitialized.
 */
static void
rpcrdma_buffer_put_mr(struct rpcrdma_mw **mw, struct rpcrdma_buffer *buf)
{
	if (*mw) {
		list_add_tail(&(*mw)->mw_list, &buf->rb_mws);
		*mw = NULL;
	}
}

/* Cycle mw's back in reverse order, and "spin" them.
 * This delays and scrambles reuse as much as possible.
 */
static void
rpcrdma_buffer_put_mrs(struct rpcrdma_req *req, struct rpcrdma_buffer *buf)
{
	struct rpcrdma_mr_seg *seg = req->rl_segments;
	struct rpcrdma_mr_seg *seg1 = seg;
	int i;

	for (i = 1, seg++; i < RPCRDMA_MAX_SEGS; seg++, i++)
		rpcrdma_buffer_put_mr(&seg->mr_chunk.rl_mw, buf);
	rpcrdma_buffer_put_mr(&seg1->mr_chunk.rl_mw, buf);
}

static void
rpcrdma_buffer_put_sendbuf(struct rpcrdma_req *req, struct rpcrdma_buffer *buf)
{
	buf->rb_send_bufs[--buf->rb_send_index] = req;
	req->rl_niovs = 0;
	if (req->rl_reply) {
		buf->rb_recv_bufs[--buf->rb_recv_index] = req->rl_reply;
		req->rl_reply->rr_func = NULL;
		req->rl_reply = NULL;
	}
}

/* rpcrdma_unmap_one() was already done by rpcrdma_deregister_frmr_external().
 * Redo only the ib_post_send().
 */
static void
rpcrdma_retry_local_inv(struct rpcrdma_mw *r, struct rpcrdma_ia *ia)
{
	struct rpcrdma_xprt *r_xprt =
				container_of(ia, struct rpcrdma_xprt, rx_ia);
	struct ib_send_wr invalidate_wr, *bad_wr;
	int rc;

	dprintk("RPC:       %s: FRMR %p is stale\n", __func__, r);

	/* When this FRMR is re-inserted into rb_mws, it is no longer stale */
	r->r.frmr.fr_state = FRMR_IS_INVALID;

	memset(&invalidate_wr, 0, sizeof(invalidate_wr));
	invalidate_wr.wr_id = (unsigned long)(void *)r;
	invalidate_wr.opcode = IB_WR_LOCAL_INV;
	invalidate_wr.ex.invalidate_rkey = r->r.frmr.fr_mr->rkey;
	DECR_CQCOUNT(&r_xprt->rx_ep);

	dprintk("RPC:       %s: frmr %p invalidating rkey %08x\n",
		__func__, r, r->r.frmr.fr_mr->rkey);

	read_lock(&ia->ri_qplock);
	rc = ib_post_send(ia->ri_id->qp, &invalidate_wr, &bad_wr);
	read_unlock(&ia->ri_qplock);
	if (rc) {
		/* Force rpcrdma_buffer_get() to retry */
		r->r.frmr.fr_state = FRMR_IS_STALE;
		dprintk("RPC:       %s: ib_post_send failed, %i\n",
			__func__, rc);
	}
}

static void
rpcrdma_retry_flushed_linv(struct list_head *stale,
			   struct rpcrdma_buffer *buf)
{
	struct rpcrdma_ia *ia = rdmab_to_ia(buf);
	struct list_head *pos;
	struct rpcrdma_mw *r;
	unsigned long flags;

	list_for_each(pos, stale) {
		r = list_entry(pos, struct rpcrdma_mw, mw_list);
		rpcrdma_retry_local_inv(r, ia);
	}

	spin_lock_irqsave(&buf->rb_lock, flags);
	list_splice_tail(stale, &buf->rb_mws);
	spin_unlock_irqrestore(&buf->rb_lock, flags);
}

static struct rpcrdma_req *
rpcrdma_buffer_get_frmrs(struct rpcrdma_req *req, struct rpcrdma_buffer *buf,
			 struct list_head *stale)
{
	struct rpcrdma_mw *r;
	int i;

	i = RPCRDMA_MAX_SEGS - 1;
	while (!list_empty(&buf->rb_mws)) {
		r = list_entry(buf->rb_mws.next,
			       struct rpcrdma_mw, mw_list);
		list_del(&r->mw_list);
		if (r->r.frmr.fr_state == FRMR_IS_STALE) {
			list_add(&r->mw_list, stale);
			continue;
		}
		req->rl_segments[i].mr_chunk.rl_mw = r;
		if (unlikely(i-- == 0))
			return req;	/* Success */
	}

	/* Not enough entries on rb_mws for this req */
	rpcrdma_buffer_put_sendbuf(req, buf);
	rpcrdma_buffer_put_mrs(req, buf);
	return NULL;
}

static struct rpcrdma_req *
rpcrdma_buffer_get_fmrs(struct rpcrdma_req *req, struct rpcrdma_buffer *buf)
{
	struct rpcrdma_mw *r;
	int i;

	i = RPCRDMA_MAX_SEGS - 1;
	while (!list_empty(&buf->rb_mws)) {
		r = list_entry(buf->rb_mws.next,
			       struct rpcrdma_mw, mw_list);
		list_del(&r->mw_list);
		req->rl_segments[i].mr_chunk.rl_mw = r;
		if (unlikely(i-- == 0))
			return req;	/* Success */
	}

	/* Not enough entries on rb_mws for this req */
	rpcrdma_buffer_put_sendbuf(req, buf);
	rpcrdma_buffer_put_mrs(req, buf);
	return NULL;
=======
	cancel_delayed_work_sync(&buf->rb_recovery_worker);
	cancel_delayed_work_sync(&buf->rb_refresh_worker);

	while (!list_empty(&buf->rb_recv_bufs)) {
		struct rpcrdma_rep *rep;

		rep = rpcrdma_buffer_get_rep_locked(buf);
		rpcrdma_destroy_rep(rep);
	}
	buf->rb_send_count = 0;

	spin_lock(&buf->rb_reqslock);
	while (!list_empty(&buf->rb_allreqs)) {
		struct rpcrdma_req *req;

		req = list_first_entry(&buf->rb_allreqs,
				       struct rpcrdma_req, rl_all);
		list_del(&req->rl_all);

		spin_unlock(&buf->rb_reqslock);
		rpcrdma_destroy_req(req);
		spin_lock(&buf->rb_reqslock);
	}
	spin_unlock(&buf->rb_reqslock);
	buf->rb_recv_count = 0;

	rpcrdma_destroy_mrs(buf);
}

struct rpcrdma_mw *
rpcrdma_get_mw(struct rpcrdma_xprt *r_xprt)
{
	struct rpcrdma_buffer *buf = &r_xprt->rx_buf;
	struct rpcrdma_mw *mw = NULL;

	spin_lock(&buf->rb_mwlock);
	if (!list_empty(&buf->rb_mws)) {
		mw = list_first_entry(&buf->rb_mws,
				      struct rpcrdma_mw, mw_list);
		list_del_init(&mw->mw_list);
	}
	spin_unlock(&buf->rb_mwlock);

	if (!mw)
		goto out_nomws;
	return mw;

out_nomws:
	dprintk("RPC:       %s: no MWs available\n", __func__);
	schedule_delayed_work(&buf->rb_refresh_worker, 0);

	/* Allow the reply handler and refresh worker to run */
	cond_resched();

	return NULL;
}

void
rpcrdma_put_mw(struct rpcrdma_xprt *r_xprt, struct rpcrdma_mw *mw)
{
	struct rpcrdma_buffer *buf = &r_xprt->rx_buf;

	spin_lock(&buf->rb_mwlock);
	list_add_tail(&mw->mw_list, &buf->rb_mws);
	spin_unlock(&buf->rb_mwlock);
}

static struct rpcrdma_rep *
rpcrdma_buffer_get_rep(struct rpcrdma_buffer *buffers)
{
	/* If an RPC previously completed without a reply (say, a
	 * credential problem or a soft timeout occurs) then hold off
	 * on supplying more Receive buffers until the number of new
	 * pending RPCs catches up to the number of posted Receives.
	 */
	if (unlikely(buffers->rb_send_count < buffers->rb_recv_count))
		return NULL;

	if (unlikely(list_empty(&buffers->rb_recv_bufs)))
		return NULL;
	buffers->rb_recv_count++;
	return rpcrdma_buffer_get_rep_locked(buffers);
>>>>>>> v4.9.227
}

/*
 * Get a set of request/reply buffers.
 *
<<<<<<< HEAD
 * Reply buffer (if needed) is attached to send buffer upon return.
 * Rule:
 *    rb_send_index and rb_recv_index MUST always be pointing to the
 *    *next* available buffer (non-NULL). They are incremented after
 *    removing buffers, and decremented *before* returning them.
=======
 * Reply buffer (if available) is attached to send buffer upon return.
>>>>>>> v4.9.227
 */
struct rpcrdma_req *
rpcrdma_buffer_get(struct rpcrdma_buffer *buffers)
{
<<<<<<< HEAD
	struct rpcrdma_ia *ia = rdmab_to_ia(buffers);
	struct list_head stale;
	struct rpcrdma_req *req;
	unsigned long flags;

	spin_lock_irqsave(&buffers->rb_lock, flags);
	if (buffers->rb_send_index == buffers->rb_max_requests) {
		spin_unlock_irqrestore(&buffers->rb_lock, flags);
		dprintk("RPC:       %s: out of request buffers\n", __func__);
		return ((struct rpcrdma_req *)NULL);
	}

	req = buffers->rb_send_bufs[buffers->rb_send_index];
	if (buffers->rb_send_index < buffers->rb_recv_index) {
		dprintk("RPC:       %s: %d extra receives outstanding (ok)\n",
			__func__,
			buffers->rb_recv_index - buffers->rb_send_index);
		req->rl_reply = NULL;
	} else {
		req->rl_reply = buffers->rb_recv_bufs[buffers->rb_recv_index];
		buffers->rb_recv_bufs[buffers->rb_recv_index++] = NULL;
	}
	buffers->rb_send_bufs[buffers->rb_send_index++] = NULL;

	INIT_LIST_HEAD(&stale);
	switch (ia->ri_memreg_strategy) {
	case RPCRDMA_FRMR:
		req = rpcrdma_buffer_get_frmrs(req, buffers, &stale);
		break;
	case RPCRDMA_MTHCAFMR:
		req = rpcrdma_buffer_get_fmrs(req, buffers);
		break;
	default:
		break;
	}
	spin_unlock_irqrestore(&buffers->rb_lock, flags);
	if (!list_empty(&stale))
		rpcrdma_retry_flushed_linv(&stale, buffers);
	return req;
=======
	struct rpcrdma_req *req;

	spin_lock(&buffers->rb_lock);
	if (list_empty(&buffers->rb_send_bufs))
		goto out_reqbuf;
	buffers->rb_send_count++;
	req = rpcrdma_buffer_get_req_locked(buffers);
	req->rl_reply = rpcrdma_buffer_get_rep(buffers);
	spin_unlock(&buffers->rb_lock);
	return req;

out_reqbuf:
	spin_unlock(&buffers->rb_lock);
	pr_warn("RPC:       %s: out of request buffers\n", __func__);
	return NULL;
>>>>>>> v4.9.227
}

/*
 * Put request/reply buffers back into pool.
 * Pre-decrement counter/array index.
 */
void
rpcrdma_buffer_put(struct rpcrdma_req *req)
{
	struct rpcrdma_buffer *buffers = req->rl_buffer;
<<<<<<< HEAD
	struct rpcrdma_ia *ia = rdmab_to_ia(buffers);
	unsigned long flags;

	spin_lock_irqsave(&buffers->rb_lock, flags);
	rpcrdma_buffer_put_sendbuf(req, buffers);
	switch (ia->ri_memreg_strategy) {
	case RPCRDMA_FRMR:
	case RPCRDMA_MTHCAFMR:
		rpcrdma_buffer_put_mrs(req, buffers);
		break;
	default:
		break;
	}
	spin_unlock_irqrestore(&buffers->rb_lock, flags);
=======
	struct rpcrdma_rep *rep = req->rl_reply;

	req->rl_send_wr.num_sge = 0;
	req->rl_reply = NULL;

	spin_lock(&buffers->rb_lock);
	buffers->rb_send_count--;
	list_add_tail(&req->rl_free, &buffers->rb_send_bufs);
	if (rep) {
		buffers->rb_recv_count--;
		list_add_tail(&rep->rr_list, &buffers->rb_recv_bufs);
	}
	spin_unlock(&buffers->rb_lock);
>>>>>>> v4.9.227
}

/*
 * Recover reply buffers from pool.
<<<<<<< HEAD
 * This happens when recovering from error conditions.
 * Post-increment counter/array index.
=======
 * This happens when recovering from disconnect.
>>>>>>> v4.9.227
 */
void
rpcrdma_recv_buffer_get(struct rpcrdma_req *req)
{
	struct rpcrdma_buffer *buffers = req->rl_buffer;
<<<<<<< HEAD
	unsigned long flags;

	if (req->rl_iov.length == 0)	/* special case xprt_rdma_allocate() */
		buffers = ((struct rpcrdma_req *) buffers)->rl_buffer;
	spin_lock_irqsave(&buffers->rb_lock, flags);
	if (buffers->rb_recv_index < buffers->rb_max_requests) {
		req->rl_reply = buffers->rb_recv_bufs[buffers->rb_recv_index];
		buffers->rb_recv_bufs[buffers->rb_recv_index++] = NULL;
	}
	spin_unlock_irqrestore(&buffers->rb_lock, flags);
=======

	spin_lock(&buffers->rb_lock);
	req->rl_reply = rpcrdma_buffer_get_rep(buffers);
	spin_unlock(&buffers->rb_lock);
>>>>>>> v4.9.227
}

/*
 * Put reply buffers back into pool when not attached to
 * request. This happens in error conditions.
 */
void
rpcrdma_recv_buffer_put(struct rpcrdma_rep *rep)
{
<<<<<<< HEAD
	struct rpcrdma_buffer *buffers = rep->rr_buffer;
	unsigned long flags;

	rep->rr_func = NULL;
	spin_lock_irqsave(&buffers->rb_lock, flags);
	buffers->rb_recv_bufs[--buffers->rb_recv_index] = rep;
	spin_unlock_irqrestore(&buffers->rb_lock, flags);
}

/*
 * Wrappers for internal-use kmalloc memory registration, used by buffer code.
 */

int
rpcrdma_register_internal(struct rpcrdma_ia *ia, void *va, int len,
				struct ib_mr **mrp, struct ib_sge *iov)
{
	struct ib_phys_buf ipb;
	struct ib_mr *mr;
	int rc;

	/*
	 * All memory passed here was kmalloc'ed, therefore phys-contiguous.
	 */
	iov->addr = ib_dma_map_single(ia->ri_id->device,
			va, len, DMA_BIDIRECTIONAL);
	if (ib_dma_mapping_error(ia->ri_id->device, iov->addr))
		return -ENOMEM;

	iov->length = len;

	if (ia->ri_have_dma_lkey) {
		*mrp = NULL;
		iov->lkey = ia->ri_dma_lkey;
		return 0;
	} else if (ia->ri_bind_mem != NULL) {
		*mrp = NULL;
		iov->lkey = ia->ri_bind_mem->lkey;
		return 0;
	}

	ipb.addr = iov->addr;
	ipb.size = iov->length;
	mr = ib_reg_phys_mr(ia->ri_pd, &ipb, 1,
			IB_ACCESS_LOCAL_WRITE, &iov->addr);

	dprintk("RPC:       %s: phys convert: 0x%llx "
			"registered 0x%llx length %d\n",
			__func__, (unsigned long long)ipb.addr,
			(unsigned long long)iov->addr, len);

	if (IS_ERR(mr)) {
		*mrp = NULL;
		rc = PTR_ERR(mr);
		dprintk("RPC:       %s: failed with %i\n", __func__, rc);
	} else {
		*mrp = mr;
		iov->lkey = mr->lkey;
		rc = 0;
	}

	return rc;
}

int
rpcrdma_deregister_internal(struct rpcrdma_ia *ia,
				struct ib_mr *mr, struct ib_sge *iov)
{
	int rc;

	ib_dma_unmap_single(ia->ri_id->device,
			iov->addr, iov->length, DMA_BIDIRECTIONAL);

	if (NULL == mr)
		return 0;

	rc = ib_dereg_mr(mr);
	if (rc)
		dprintk("RPC:       %s: ib_dereg_mr failed %i\n", __func__, rc);
	return rc;
}

/*
 * Wrappers for chunk registration, shared by read/write chunk code.
 */

static void
rpcrdma_map_one(struct rpcrdma_ia *ia, struct rpcrdma_mr_seg *seg, int writing)
{
	seg->mr_dir = writing ? DMA_FROM_DEVICE : DMA_TO_DEVICE;
	seg->mr_dmalen = seg->mr_len;
	if (seg->mr_page)
		seg->mr_dma = ib_dma_map_page(ia->ri_id->device,
				seg->mr_page, offset_in_page(seg->mr_offset),
				seg->mr_dmalen, seg->mr_dir);
	else
		seg->mr_dma = ib_dma_map_single(ia->ri_id->device,
				seg->mr_offset,
				seg->mr_dmalen, seg->mr_dir);
	if (ib_dma_mapping_error(ia->ri_id->device, seg->mr_dma)) {
		dprintk("RPC:       %s: mr_dma %llx mr_offset %p mr_dma_len %zu\n",
			__func__,
			(unsigned long long)seg->mr_dma,
			seg->mr_offset, seg->mr_dmalen);
	}
}

static void
rpcrdma_unmap_one(struct rpcrdma_ia *ia, struct rpcrdma_mr_seg *seg)
{
	if (seg->mr_page)
		ib_dma_unmap_page(ia->ri_id->device,
				seg->mr_dma, seg->mr_dmalen, seg->mr_dir);
	else
		ib_dma_unmap_single(ia->ri_id->device,
				seg->mr_dma, seg->mr_dmalen, seg->mr_dir);
}

static int
rpcrdma_register_frmr_external(struct rpcrdma_mr_seg *seg,
			int *nsegs, int writing, struct rpcrdma_ia *ia,
			struct rpcrdma_xprt *r_xprt)
{
	struct rpcrdma_mr_seg *seg1 = seg;
	struct rpcrdma_mw *mw = seg1->mr_chunk.rl_mw;
	struct rpcrdma_frmr *frmr = &mw->r.frmr;
	struct ib_mr *mr = frmr->fr_mr;
	struct ib_send_wr fastreg_wr, *bad_wr;
	u8 key;
	int len, pageoff;
	int i, rc;
	int seg_len;
	u64 pa;
	int page_no;

	pageoff = offset_in_page(seg1->mr_offset);
	seg1->mr_offset -= pageoff;	/* start of page */
	seg1->mr_len += pageoff;
	len = -pageoff;
	if (*nsegs > ia->ri_max_frmr_depth)
		*nsegs = ia->ri_max_frmr_depth;
	for (page_no = i = 0; i < *nsegs;) {
		rpcrdma_map_one(ia, seg, writing);
		pa = seg->mr_dma;
		for (seg_len = seg->mr_len; seg_len > 0; seg_len -= PAGE_SIZE) {
			frmr->fr_pgl->page_list[page_no++] = pa;
			pa += PAGE_SIZE;
		}
		len += seg->mr_len;
		++seg;
		++i;
		/* Check for holes */
		if ((i < *nsegs && offset_in_page(seg->mr_offset)) ||
		    offset_in_page((seg-1)->mr_offset + (seg-1)->mr_len))
			break;
	}
	dprintk("RPC:       %s: Using frmr %p to map %d segments\n",
		__func__, mw, i);

	frmr->fr_state = FRMR_IS_VALID;

	memset(&fastreg_wr, 0, sizeof(fastreg_wr));
	fastreg_wr.wr_id = (unsigned long)(void *)mw;
	fastreg_wr.opcode = IB_WR_FAST_REG_MR;
	fastreg_wr.wr.fast_reg.iova_start = seg1->mr_dma;
	fastreg_wr.wr.fast_reg.page_list = frmr->fr_pgl;
	fastreg_wr.wr.fast_reg.page_list_len = page_no;
	fastreg_wr.wr.fast_reg.page_shift = PAGE_SHIFT;
	fastreg_wr.wr.fast_reg.length = page_no << PAGE_SHIFT;
	if (fastreg_wr.wr.fast_reg.length < len) {
		rc = -EIO;
		goto out_err;
	}

	/* Bump the key */
	key = (u8)(mr->rkey & 0x000000FF);
	ib_update_fast_reg_key(mr, ++key);

	fastreg_wr.wr.fast_reg.access_flags = (writing ?
				IB_ACCESS_REMOTE_WRITE | IB_ACCESS_LOCAL_WRITE :
				IB_ACCESS_REMOTE_READ);
	fastreg_wr.wr.fast_reg.rkey = mr->rkey;
	DECR_CQCOUNT(&r_xprt->rx_ep);

	rc = ib_post_send(ia->ri_id->qp, &fastreg_wr, &bad_wr);
	if (rc) {
		dprintk("RPC:       %s: failed ib_post_send for register,"
			" status %i\n", __func__, rc);
		ib_update_fast_reg_key(mr, --key);
		goto out_err;
	} else {
		seg1->mr_rkey = mr->rkey;
		seg1->mr_base = seg1->mr_dma + pageoff;
		seg1->mr_nsegs = i;
		seg1->mr_len = len;
	}
	*nsegs = i;
	return 0;
out_err:
	frmr->fr_state = FRMR_IS_INVALID;
	while (i--)
		rpcrdma_unmap_one(ia, --seg);
	return rc;
}

static int
rpcrdma_deregister_frmr_external(struct rpcrdma_mr_seg *seg,
			struct rpcrdma_ia *ia, struct rpcrdma_xprt *r_xprt)
{
	struct rpcrdma_mr_seg *seg1 = seg;
	struct ib_send_wr invalidate_wr, *bad_wr;
	int rc;

	seg1->mr_chunk.rl_mw->r.frmr.fr_state = FRMR_IS_INVALID;

	memset(&invalidate_wr, 0, sizeof invalidate_wr);
	invalidate_wr.wr_id = (unsigned long)(void *)seg1->mr_chunk.rl_mw;
	invalidate_wr.opcode = IB_WR_LOCAL_INV;
	invalidate_wr.ex.invalidate_rkey = seg1->mr_chunk.rl_mw->r.frmr.fr_mr->rkey;
	DECR_CQCOUNT(&r_xprt->rx_ep);

	read_lock(&ia->ri_qplock);
	while (seg1->mr_nsegs--)
		rpcrdma_unmap_one(ia, seg++);
	rc = ib_post_send(ia->ri_id->qp, &invalidate_wr, &bad_wr);
	read_unlock(&ia->ri_qplock);
	if (rc) {
		/* Force rpcrdma_buffer_get() to retry */
		seg1->mr_chunk.rl_mw->r.frmr.fr_state = FRMR_IS_STALE;
		dprintk("RPC:       %s: failed ib_post_send for invalidate,"
			" status %i\n", __func__, rc);
	}
	return rc;
}

static int
rpcrdma_register_fmr_external(struct rpcrdma_mr_seg *seg,
			int *nsegs, int writing, struct rpcrdma_ia *ia)
{
	struct rpcrdma_mr_seg *seg1 = seg;
	u64 physaddrs[RPCRDMA_MAX_DATA_SEGS];
	int len, pageoff, i, rc;

	pageoff = offset_in_page(seg1->mr_offset);
	seg1->mr_offset -= pageoff;	/* start of page */
	seg1->mr_len += pageoff;
	len = -pageoff;
	if (*nsegs > RPCRDMA_MAX_DATA_SEGS)
		*nsegs = RPCRDMA_MAX_DATA_SEGS;
	for (i = 0; i < *nsegs;) {
		rpcrdma_map_one(ia, seg, writing);
		physaddrs[i] = seg->mr_dma;
		len += seg->mr_len;
		++seg;
		++i;
		/* Check for holes */
		if ((i < *nsegs && offset_in_page(seg->mr_offset)) ||
		    offset_in_page((seg-1)->mr_offset + (seg-1)->mr_len))
			break;
	}
	rc = ib_map_phys_fmr(seg1->mr_chunk.rl_mw->r.fmr,
				physaddrs, i, seg1->mr_dma);
	if (rc) {
		dprintk("RPC:       %s: failed ib_map_phys_fmr "
			"%u@0x%llx+%i (%d)... status %i\n", __func__,
			len, (unsigned long long)seg1->mr_dma,
			pageoff, i, rc);
		while (i--)
			rpcrdma_unmap_one(ia, --seg);
	} else {
		seg1->mr_rkey = seg1->mr_chunk.rl_mw->r.fmr->rkey;
		seg1->mr_base = seg1->mr_dma + pageoff;
		seg1->mr_nsegs = i;
		seg1->mr_len = len;
	}
	*nsegs = i;
	return rc;
}

static int
rpcrdma_deregister_fmr_external(struct rpcrdma_mr_seg *seg,
			struct rpcrdma_ia *ia)
{
	struct rpcrdma_mr_seg *seg1 = seg;
	LIST_HEAD(l);
	int rc;

	list_add(&seg1->mr_chunk.rl_mw->r.fmr->list, &l);
	rc = ib_unmap_fmr(&l);
	read_lock(&ia->ri_qplock);
	while (seg1->mr_nsegs--)
		rpcrdma_unmap_one(ia, seg++);
	read_unlock(&ia->ri_qplock);
	if (rc)
		dprintk("RPC:       %s: failed ib_unmap_fmr,"
			" status %i\n", __func__, rc);
	return rc;
}

int
rpcrdma_register_external(struct rpcrdma_mr_seg *seg,
			int nsegs, int writing, struct rpcrdma_xprt *r_xprt)
{
	struct rpcrdma_ia *ia = &r_xprt->rx_ia;
	int rc = 0;

	switch (ia->ri_memreg_strategy) {

	case RPCRDMA_ALLPHYSICAL:
		rpcrdma_map_one(ia, seg, writing);
		seg->mr_rkey = ia->ri_bind_mem->rkey;
		seg->mr_base = seg->mr_dma;
		seg->mr_nsegs = 1;
		nsegs = 1;
		break;

	/* Registration using frmr registration */
	case RPCRDMA_FRMR:
		rc = rpcrdma_register_frmr_external(seg, &nsegs, writing, ia, r_xprt);
		break;

	/* Registration using fmr memory registration */
	case RPCRDMA_MTHCAFMR:
		rc = rpcrdma_register_fmr_external(seg, &nsegs, writing, ia);
		break;

	default:
		return -1;
	}
	if (rc)
		return -1;

	return nsegs;
}

int
rpcrdma_deregister_external(struct rpcrdma_mr_seg *seg,
		struct rpcrdma_xprt *r_xprt)
{
	struct rpcrdma_ia *ia = &r_xprt->rx_ia;
	int nsegs = seg->mr_nsegs, rc;

	switch (ia->ri_memreg_strategy) {

	case RPCRDMA_ALLPHYSICAL:
		read_lock(&ia->ri_qplock);
		rpcrdma_unmap_one(ia, seg);
		read_unlock(&ia->ri_qplock);
		break;

	case RPCRDMA_FRMR:
		rc = rpcrdma_deregister_frmr_external(seg, ia, r_xprt);
		break;

	case RPCRDMA_MTHCAFMR:
		rc = rpcrdma_deregister_fmr_external(seg, ia);
		break;

	default:
		break;
	}
	return nsegs;
=======
	struct rpcrdma_buffer *buffers = &rep->rr_rxprt->rx_buf;

	spin_lock(&buffers->rb_lock);
	buffers->rb_recv_count--;
	list_add_tail(&rep->rr_list, &buffers->rb_recv_bufs);
	spin_unlock(&buffers->rb_lock);
}

/**
 * rpcrdma_alloc_regbuf - allocate and DMA-map memory for SEND/RECV buffers
 * @size: size of buffer to be allocated, in bytes
 * @direction: direction of data movement
 * @flags: GFP flags
 *
 * Returns an ERR_PTR, or a pointer to a regbuf, a buffer that
 * can be persistently DMA-mapped for I/O.
 *
 * xprtrdma uses a regbuf for posting an outgoing RDMA SEND, or for
 * receiving the payload of RDMA RECV operations. During Long Calls
 * or Replies they may be registered externally via ro_map.
 */
struct rpcrdma_regbuf *
rpcrdma_alloc_regbuf(size_t size, enum dma_data_direction direction,
		     gfp_t flags)
{
	struct rpcrdma_regbuf *rb;

	rb = kmalloc(sizeof(*rb) + size, flags);
	if (rb == NULL)
		return ERR_PTR(-ENOMEM);

	rb->rg_device = NULL;
	rb->rg_direction = direction;
	rb->rg_iov.length = size;

	return rb;
}

/**
 * __rpcrdma_map_regbuf - DMA-map a regbuf
 * @ia: controlling rpcrdma_ia
 * @rb: regbuf to be mapped
 */
bool
__rpcrdma_dma_map_regbuf(struct rpcrdma_ia *ia, struct rpcrdma_regbuf *rb)
{
	if (rb->rg_direction == DMA_NONE)
		return false;

	rb->rg_iov.addr = ib_dma_map_single(ia->ri_device,
					    (void *)rb->rg_base,
					    rdmab_length(rb),
					    rb->rg_direction);
	if (ib_dma_mapping_error(ia->ri_device, rdmab_addr(rb)))
		return false;

	rb->rg_device = ia->ri_device;
	rb->rg_iov.lkey = ia->ri_pd->local_dma_lkey;
	return true;
}

static void
rpcrdma_dma_unmap_regbuf(struct rpcrdma_regbuf *rb)
{
	if (!rpcrdma_regbuf_is_mapped(rb))
		return;

	ib_dma_unmap_single(rb->rg_device, rdmab_addr(rb),
			    rdmab_length(rb), rb->rg_direction);
	rb->rg_device = NULL;
}

/**
 * rpcrdma_free_regbuf - deregister and free registered buffer
 * @rb: regbuf to be deregistered and freed
 */
void
rpcrdma_free_regbuf(struct rpcrdma_regbuf *rb)
{
	if (!rb)
		return;

	rpcrdma_dma_unmap_regbuf(rb);
	kfree(rb);
>>>>>>> v4.9.227
}

/*
 * Prepost any receive buffer, then post send.
 *
 * Receive buffer is donated to hardware, reclaimed upon recv completion.
 */
int
rpcrdma_ep_post(struct rpcrdma_ia *ia,
		struct rpcrdma_ep *ep,
		struct rpcrdma_req *req)
{
<<<<<<< HEAD
	struct ib_send_wr send_wr, *send_wr_fail;
	struct rpcrdma_rep *rep = req->rl_reply;
	int rc;

	if (rep) {
		rc = rpcrdma_ep_post_recv(ia, ep, rep);
		if (rc)
			goto out;
		req->rl_reply = NULL;
	}

	send_wr.next = NULL;
	send_wr.wr_id = 0ULL;	/* no send cookie */
	send_wr.sg_list = req->rl_send_iov;
	send_wr.num_sge = req->rl_niovs;
	send_wr.opcode = IB_WR_SEND;
	if (send_wr.num_sge == 4)	/* no need to sync any pad (constant) */
		ib_dma_sync_single_for_device(ia->ri_id->device,
			req->rl_send_iov[3].addr, req->rl_send_iov[3].length,
			DMA_TO_DEVICE);
	ib_dma_sync_single_for_device(ia->ri_id->device,
		req->rl_send_iov[1].addr, req->rl_send_iov[1].length,
		DMA_TO_DEVICE);
	ib_dma_sync_single_for_device(ia->ri_id->device,
		req->rl_send_iov[0].addr, req->rl_send_iov[0].length,
		DMA_TO_DEVICE);

	if (DECR_CQCOUNT(ep) > 0)
		send_wr.send_flags = 0;
	else { /* Provider must take a send completion every now and then */
		INIT_CQCOUNT(ep);
		send_wr.send_flags = IB_SEND_SIGNALED;
	}

	rc = ib_post_send(ia->ri_id->qp, &send_wr, &send_wr_fail);
	if (rc)
		dprintk("RPC:       %s: ib_post_send returned %i\n", __func__,
			rc);
out:
	return rc;
}

/*
 * (Re)post a receive buffer.
 */
int
rpcrdma_ep_post_recv(struct rpcrdma_ia *ia,
		     struct rpcrdma_ep *ep,
		     struct rpcrdma_rep *rep)
{
	struct ib_recv_wr recv_wr, *recv_wr_fail;
	int rc;

	recv_wr.next = NULL;
	recv_wr.wr_id = (u64) (unsigned long) rep;
	recv_wr.sg_list = &rep->rr_iov;
	recv_wr.num_sge = 1;

	ib_dma_sync_single_for_cpu(ia->ri_id->device,
		rep->rr_iov.addr, rep->rr_iov.length, DMA_BIDIRECTIONAL);

	rc = ib_post_recv(ia->ri_id->qp, &recv_wr, &recv_wr_fail);

	if (rc)
		dprintk("RPC:       %s: ib_post_recv returned %i\n", __func__,
			rc);
	return rc;
}

/* Physical mapping means one Read/Write list entry per-page.
 * All list entries must fit within an inline buffer
 *
 * NB: The server must return a Write list for NFS READ,
 *     which has the same constraint. Factor in the inline
 *     rsize as well.
 */
static size_t
rpcrdma_physical_max_payload(struct rpcrdma_xprt *r_xprt)
{
	struct rpcrdma_create_data_internal *cdata = &r_xprt->rx_data;
	unsigned int inline_size, pages;

	inline_size = min_t(unsigned int,
			    cdata->inline_wsize, cdata->inline_rsize);
	inline_size -= RPCRDMA_HDRLEN_MIN;
	pages = inline_size / sizeof(struct rpcrdma_segment);
	return pages << PAGE_SHIFT;
}

static size_t
rpcrdma_mr_max_payload(struct rpcrdma_xprt *r_xprt)
{
	return RPCRDMA_MAX_DATA_SEGS << PAGE_SHIFT;
}

size_t
rpcrdma_max_payload(struct rpcrdma_xprt *r_xprt)
{
	size_t result;

	switch (r_xprt->rx_ia.ri_memreg_strategy) {
	case RPCRDMA_ALLPHYSICAL:
		result = rpcrdma_physical_max_payload(r_xprt);
		break;
	default:
		result = rpcrdma_mr_max_payload(r_xprt);
	}
	return result;
}
=======
	struct ib_send_wr *send_wr = &req->rl_send_wr;
	struct ib_send_wr *send_wr_fail;
	int rc;

	if (req->rl_reply) {
		rc = rpcrdma_ep_post_recv(ia, req->rl_reply);
		if (rc)
			return rc;
		req->rl_reply = NULL;
	}

	dprintk("RPC:       %s: posting %d s/g entries\n",
		__func__, send_wr->num_sge);

	rpcrdma_set_signaled(ep, send_wr);
	rc = ib_post_send(ia->ri_id->qp, send_wr, &send_wr_fail);
	if (rc)
		goto out_postsend_err;
	return 0;

out_postsend_err:
	pr_err("rpcrdma: RDMA Send ib_post_send returned %i\n", rc);
	return -ENOTCONN;
}

int
rpcrdma_ep_post_recv(struct rpcrdma_ia *ia,
		     struct rpcrdma_rep *rep)
{
	struct ib_recv_wr *recv_wr_fail;
	int rc;

	if (!rpcrdma_dma_map_regbuf(ia, rep->rr_rdmabuf))
		goto out_map;
	rc = ib_post_recv(ia->ri_id->qp, &rep->rr_recv_wr, &recv_wr_fail);
	if (rc)
		goto out_postrecv;
	return 0;

out_map:
	pr_err("rpcrdma: failed to DMA map the Receive buffer\n");
	return -EIO;

out_postrecv:
	pr_err("rpcrdma: ib_post_recv returned %i\n", rc);
	return -ENOTCONN;
}

/**
 * rpcrdma_ep_post_extra_recv - Post buffers for incoming backchannel requests
 * @r_xprt: transport associated with these backchannel resources
 * @min_reqs: minimum number of incoming requests expected
 *
 * Returns zero if all requested buffers were posted, or a negative errno.
 */
int
rpcrdma_ep_post_extra_recv(struct rpcrdma_xprt *r_xprt, unsigned int count)
{
	struct rpcrdma_buffer *buffers = &r_xprt->rx_buf;
	struct rpcrdma_ia *ia = &r_xprt->rx_ia;
	struct rpcrdma_rep *rep;
	int rc;

	while (count--) {
		spin_lock(&buffers->rb_lock);
		if (list_empty(&buffers->rb_recv_bufs))
			goto out_reqbuf;
		rep = rpcrdma_buffer_get_rep_locked(buffers);
		spin_unlock(&buffers->rb_lock);

		rc = rpcrdma_ep_post_recv(ia, rep);
		if (rc)
			goto out_rc;
	}

	return 0;

out_reqbuf:
	spin_unlock(&buffers->rb_lock);
	pr_warn("%s: no extra receive buffers\n", __func__);
	return -ENOMEM;

out_rc:
	rpcrdma_recv_buffer_put(rep);
	return rc;
}
>>>>>>> v4.9.227
