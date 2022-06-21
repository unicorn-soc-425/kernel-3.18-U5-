/*
 * GPL HEADER START
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 only,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License version 2 for more details (a copy is included
 * in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; If not, see
<<<<<<< HEAD
 * http://www.sun.com/software/products/lustre/docs/GPLv2.pdf
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
=======
 * http://www.gnu.org/licenses/gpl-2.0.html
>>>>>>> v4.9.227
 *
 * GPL HEADER END
 */
/*
 * Copyright (c) 2007, 2010, Oracle and/or its affiliates. All rights reserved.
 * Use is subject to license terms.
 *
<<<<<<< HEAD
 * Copyright (c) 2012, Intel Corporation.
=======
 * Copyright (c) 2012, 2015, Intel Corporation.
>>>>>>> v4.9.227
 */
/*
 * This file is part of Lustre, http://www.lustre.org/
 * Lustre is a trademark of Sun Microsystems, Inc.
 *
 * lnet/selftest/rpc.c
 *
 * Author: Isaac Huang <isaac@clusterfs.com>
 *
 * 2012-05-13: Liang Zhen <liang@whamcloud.com>
 * - percpt data for service to improve smp performance
 * - code cleanup
 */

#define DEBUG_SUBSYSTEM S_LNET

#include "selftest.h"

<<<<<<< HEAD
typedef enum {
=======
enum srpc_state {
>>>>>>> v4.9.227
	SRPC_STATE_NONE,
	SRPC_STATE_NI_INIT,
	SRPC_STATE_EQ_INIT,
	SRPC_STATE_RUNNING,
	SRPC_STATE_STOPPING,
<<<<<<< HEAD
} srpc_state_t;

struct smoketest_rpc {
	spinlock_t	 rpc_glock;	/* global lock */
	srpc_service_t	*rpc_services[SRPC_SERVICE_MAX_ID + 1];
	lnet_handle_eq_t rpc_lnet_eq;	/* _the_ LNet event queue */
	srpc_state_t	 rpc_state;
=======
};

static struct smoketest_rpc {
	spinlock_t	 rpc_glock;	/* global lock */
	struct srpc_service	*rpc_services[SRPC_SERVICE_MAX_ID + 1];
	lnet_handle_eq_t rpc_lnet_eq;	/* _the_ LNet event queue */
	enum srpc_state	 rpc_state;
>>>>>>> v4.9.227
	srpc_counters_t	 rpc_counters;
	__u64		 rpc_matchbits;	/* matchbits counter */
} srpc_data;

static inline int
srpc_serv_portal(int svc_id)
{
	return svc_id < SRPC_FRAMEWORK_SERVICE_MAX_ID ?
	       SRPC_FRAMEWORK_REQUEST_PORTAL : SRPC_REQUEST_PORTAL;
}

/* forward ref's */
<<<<<<< HEAD
int srpc_handle_rpc (swi_workitem_t *wi);

void srpc_get_counters (srpc_counters_t *cnt)
=======
int srpc_handle_rpc(struct swi_workitem *wi);

void srpc_get_counters(srpc_counters_t *cnt)
>>>>>>> v4.9.227
{
	spin_lock(&srpc_data.rpc_glock);
	*cnt = srpc_data.rpc_counters;
	spin_unlock(&srpc_data.rpc_glock);
}

<<<<<<< HEAD
void srpc_set_counters (const srpc_counters_t *cnt)
=======
void srpc_set_counters(const srpc_counters_t *cnt)
>>>>>>> v4.9.227
{
	spin_lock(&srpc_data.rpc_glock);
	srpc_data.rpc_counters = *cnt;
	spin_unlock(&srpc_data.rpc_glock);
}

<<<<<<< HEAD
int
srpc_add_bulk_page(srpc_bulk_t *bk, struct page *pg, int i, int nob)
{
	nob = min(nob, (int)PAGE_CACHE_SIZE);
=======
static int
srpc_add_bulk_page(struct srpc_bulk *bk, struct page *pg, int i, int nob)
{
	nob = min_t(int, nob, PAGE_SIZE);
>>>>>>> v4.9.227

	LASSERT(nob > 0);
	LASSERT(i >= 0 && i < bk->bk_niov);

<<<<<<< HEAD
	bk->bk_iovs[i].kiov_offset = 0;
	bk->bk_iovs[i].kiov_page   = pg;
	bk->bk_iovs[i].kiov_len    = nob;
=======
	bk->bk_iovs[i].bv_offset = 0;
	bk->bk_iovs[i].bv_page = pg;
	bk->bk_iovs[i].bv_len = nob;
>>>>>>> v4.9.227
	return nob;
}

void
<<<<<<< HEAD
srpc_free_bulk (srpc_bulk_t *bk)
{
	int	 i;
	struct page *pg;

	LASSERT (bk != NULL);

	for (i = 0; i < bk->bk_niov; i++) {
		pg = bk->bk_iovs[i].kiov_page;
		if (pg == NULL) break;
=======
srpc_free_bulk(struct srpc_bulk *bk)
{
	int i;
	struct page *pg;

	LASSERT(bk);

	for (i = 0; i < bk->bk_niov; i++) {
		pg = bk->bk_iovs[i].bv_page;
		if (!pg)
			break;
>>>>>>> v4.9.227

		__free_page(pg);
	}

<<<<<<< HEAD
	LIBCFS_FREE(bk, offsetof(srpc_bulk_t, bk_iovs[bk->bk_niov]));
	return;
}

srpc_bulk_t *
srpc_alloc_bulk(int cpt, unsigned bulk_npg, unsigned bulk_len, int sink)
{
	srpc_bulk_t  *bk;
	int	      i;
=======
	LIBCFS_FREE(bk, offsetof(struct srpc_bulk, bk_iovs[bk->bk_niov]));
}

struct srpc_bulk *
srpc_alloc_bulk(int cpt, unsigned bulk_npg, unsigned bulk_len, int sink)
{
	struct srpc_bulk *bk;
	int i;
>>>>>>> v4.9.227

	LASSERT(bulk_npg > 0 && bulk_npg <= LNET_MAX_IOV);

	LIBCFS_CPT_ALLOC(bk, lnet_cpt_table(), cpt,
<<<<<<< HEAD
			 offsetof(srpc_bulk_t, bk_iovs[bulk_npg]));
	if (bk == NULL) {
=======
			 offsetof(struct srpc_bulk, bk_iovs[bulk_npg]));
	if (!bk) {
>>>>>>> v4.9.227
		CERROR("Can't allocate descriptor for %d pages\n", bulk_npg);
		return NULL;
	}

<<<<<<< HEAD
	memset(bk, 0, offsetof(srpc_bulk_t, bk_iovs[bulk_npg]));
	bk->bk_sink   = sink;
	bk->bk_len    = bulk_len;
	bk->bk_niov   = bulk_npg;

	for (i = 0; i < bulk_npg; i++) {
		struct page *pg;
		int	    nob;

		pg = alloc_pages_node(cfs_cpt_spread_node(lnet_cpt_table(), cpt),
				      GFP_IOFS, 0);
		if (pg == NULL) {
=======
	memset(bk, 0, offsetof(struct srpc_bulk, bk_iovs[bulk_npg]));
	bk->bk_sink = sink;
	bk->bk_len = bulk_len;
	bk->bk_niov = bulk_npg;

	for (i = 0; i < bulk_npg; i++) {
		struct page *pg;
		int nob;

		pg = alloc_pages_node(cfs_cpt_spread_node(lnet_cpt_table(), cpt),
				      GFP_KERNEL, 0);
		if (!pg) {
>>>>>>> v4.9.227
			CERROR("Can't allocate page %d of %d\n", i, bulk_npg);
			srpc_free_bulk(bk);
			return NULL;
		}

		nob = srpc_add_bulk_page(bk, pg, i, bulk_len);
		bulk_len -= nob;
	}

	return bk;
}

static inline __u64
<<<<<<< HEAD
srpc_next_id (void)
=======
srpc_next_id(void)
>>>>>>> v4.9.227
{
	__u64 id;

	spin_lock(&srpc_data.rpc_glock);
	id = srpc_data.rpc_matchbits++;
	spin_unlock(&srpc_data.rpc_glock);
	return id;
}

<<<<<<< HEAD
void
=======
static void
>>>>>>> v4.9.227
srpc_init_server_rpc(struct srpc_server_rpc *rpc,
		     struct srpc_service_cd *scd,
		     struct srpc_buffer *buffer)
{
	memset(rpc, 0, sizeof(*rpc));
	swi_init_workitem(&rpc->srpc_wi, rpc, srpc_handle_rpc,
			  srpc_serv_is_framework(scd->scd_svc) ?
			  lst_sched_serial : lst_sched_test[scd->scd_cpt]);

	rpc->srpc_ev.ev_fired = 1; /* no event expected now */

<<<<<<< HEAD
	rpc->srpc_scd      = scd;
	rpc->srpc_reqstbuf = buffer;
	rpc->srpc_peer     = buffer->buf_peer;
	rpc->srpc_self     = buffer->buf_self;
=======
	rpc->srpc_scd = scd;
	rpc->srpc_reqstbuf = buffer;
	rpc->srpc_peer = buffer->buf_peer;
	rpc->srpc_self = buffer->buf_self;
>>>>>>> v4.9.227
	LNetInvalidateHandle(&rpc->srpc_replymdh);
}

static void
srpc_service_fini(struct srpc_service *svc)
{
<<<<<<< HEAD
	struct srpc_service_cd	*scd;
	struct srpc_server_rpc	*rpc;
	struct srpc_buffer	*buf;
	struct list_head		*q;
	int			i;

	if (svc->sv_cpt_data == NULL)
=======
	struct srpc_service_cd *scd;
	struct srpc_server_rpc *rpc;
	struct srpc_buffer *buf;
	struct list_head *q;
	int i;

	if (!svc->sv_cpt_data)
>>>>>>> v4.9.227
		return;

	cfs_percpt_for_each(scd, i, svc->sv_cpt_data) {
		while (1) {
			if (!list_empty(&scd->scd_buf_posted))
				q = &scd->scd_buf_posted;
			else if (!list_empty(&scd->scd_buf_blocked))
				q = &scd->scd_buf_blocked;
			else
				break;

			while (!list_empty(q)) {
<<<<<<< HEAD
				buf = list_entry(q->next,
						     struct srpc_buffer,
						     buf_list);
=======
				buf = list_entry(q->next, struct srpc_buffer,
						 buf_list);
>>>>>>> v4.9.227
				list_del(&buf->buf_list);
				LIBCFS_FREE(buf, sizeof(*buf));
			}
		}

		LASSERT(list_empty(&scd->scd_rpc_active));

		while (!list_empty(&scd->scd_rpc_free)) {
			rpc = list_entry(scd->scd_rpc_free.next,
<<<<<<< HEAD
					     struct srpc_server_rpc,
					     srpc_list);
=======
					 struct srpc_server_rpc,
					 srpc_list);
>>>>>>> v4.9.227
			list_del(&rpc->srpc_list);
			LIBCFS_FREE(rpc, sizeof(*rpc));
		}
	}

	cfs_percpt_free(svc->sv_cpt_data);
	svc->sv_cpt_data = NULL;
}

static int
srpc_service_nrpcs(struct srpc_service *svc)
{
	int nrpcs = svc->sv_wi_total / svc->sv_ncpts;

	return srpc_serv_is_framework(svc) ?
	       max(nrpcs, SFW_FRWK_WI_MIN) : max(nrpcs, SFW_TEST_WI_MIN);
}

int srpc_add_buffer(struct swi_workitem *wi);

static int
srpc_service_init(struct srpc_service *svc)
{
<<<<<<< HEAD
	struct srpc_service_cd	*scd;
	struct srpc_server_rpc	*rpc;
	int			nrpcs;
	int			i;
	int			j;
=======
	struct srpc_service_cd *scd;
	struct srpc_server_rpc *rpc;
	int nrpcs;
	int i;
	int j;
>>>>>>> v4.9.227

	svc->sv_shuttingdown = 0;

	svc->sv_cpt_data = cfs_percpt_alloc(lnet_cpt_table(),
<<<<<<< HEAD
					    sizeof(struct srpc_service_cd));
	if (svc->sv_cpt_data == NULL)
=======
					    sizeof(**svc->sv_cpt_data));
	if (!svc->sv_cpt_data)
>>>>>>> v4.9.227
		return -ENOMEM;

	svc->sv_ncpts = srpc_serv_is_framework(svc) ?
			1 : cfs_cpt_number(lnet_cpt_table());
	nrpcs = srpc_service_nrpcs(svc);

	cfs_percpt_for_each(scd, i, svc->sv_cpt_data) {
		scd->scd_cpt = i;
		scd->scd_svc = svc;
		spin_lock_init(&scd->scd_lock);
		INIT_LIST_HEAD(&scd->scd_rpc_free);
		INIT_LIST_HEAD(&scd->scd_rpc_active);
		INIT_LIST_HEAD(&scd->scd_buf_posted);
		INIT_LIST_HEAD(&scd->scd_buf_blocked);

		scd->scd_ev.ev_data = scd;
		scd->scd_ev.ev_type = SRPC_REQUEST_RCVD;

<<<<<<< HEAD
		/* NB: don't use lst_sched_serial for adding buffer,
		 * see details in srpc_service_add_buffers() */
		swi_init_workitem(&scd->scd_buf_wi, scd,
				  srpc_add_buffer, lst_sched_test[i]);

		if (i != 0 && srpc_serv_is_framework(svc)) {
			/* NB: framework service only needs srpc_service_cd for
			 * one partition, but we allocate for all to make
			 * it easier to implement, it will waste a little
			 * memory but nobody should care about this */
=======
		/*
		 * NB: don't use lst_sched_serial for adding buffer,
		 * see details in srpc_service_add_buffers()
		 */
		swi_init_workitem(&scd->scd_buf_wi, scd,
				  srpc_add_buffer, lst_sched_test[i]);

		if (i && srpc_serv_is_framework(svc)) {
			/*
			 * NB: framework service only needs srpc_service_cd for
			 * one partition, but we allocate for all to make
			 * it easier to implement, it will waste a little
			 * memory but nobody should care about this
			 */
>>>>>>> v4.9.227
			continue;
		}

		for (j = 0; j < nrpcs; j++) {
			LIBCFS_CPT_ALLOC(rpc, lnet_cpt_table(),
					 i, sizeof(*rpc));
<<<<<<< HEAD
			if (rpc == NULL) {
=======
			if (!rpc) {
>>>>>>> v4.9.227
				srpc_service_fini(svc);
				return -ENOMEM;
			}
			list_add(&rpc->srpc_list, &scd->scd_rpc_free);
		}
	}

	return 0;
}

int
srpc_add_service(struct srpc_service *sv)
{
	int id = sv->sv_id;

	LASSERT(0 <= id && id <= SRPC_SERVICE_MAX_ID);

<<<<<<< HEAD
	if (srpc_service_init(sv) != 0)
=======
	if (srpc_service_init(sv))
>>>>>>> v4.9.227
		return -ENOMEM;

	spin_lock(&srpc_data.rpc_glock);

	LASSERT(srpc_data.rpc_state == SRPC_STATE_RUNNING);

<<<<<<< HEAD
	if (srpc_data.rpc_services[id] != NULL) {
=======
	if (srpc_data.rpc_services[id]) {
>>>>>>> v4.9.227
		spin_unlock(&srpc_data.rpc_glock);
		goto failed;
	}

	srpc_data.rpc_services[id] = sv;
	spin_unlock(&srpc_data.rpc_glock);

	CDEBUG(D_NET, "Adding service: id %d, name %s\n", id, sv->sv_name);
	return 0;

 failed:
	srpc_service_fini(sv);
	return -EBUSY;
}

int
<<<<<<< HEAD
srpc_remove_service (srpc_service_t *sv)
=======
srpc_remove_service(struct srpc_service *sv)
>>>>>>> v4.9.227
{
	int id = sv->sv_id;

	spin_lock(&srpc_data.rpc_glock);

	if (srpc_data.rpc_services[id] != sv) {
		spin_unlock(&srpc_data.rpc_glock);
		return -ENOENT;
	}

	srpc_data.rpc_services[id] = NULL;
	spin_unlock(&srpc_data.rpc_glock);
	return 0;
}

<<<<<<< HEAD
int
srpc_post_passive_rdma(int portal, int local, __u64 matchbits, void *buf,
		       int len, int options, lnet_process_id_t peer,
		       lnet_handle_md_t *mdh, srpc_event_t *ev)
{
	int		 rc;
	lnet_md_t	 md;
=======
static int
srpc_post_passive_rdma(int portal, int local, __u64 matchbits, void *buf,
		       int len, int options, lnet_process_id_t peer,
		       lnet_handle_md_t *mdh, struct srpc_event *ev)
{
	int rc;
	lnet_md_t md;
>>>>>>> v4.9.227
	lnet_handle_me_t meh;

	rc = LNetMEAttach(portal, peer, matchbits, 0, LNET_UNLINK,
			  local ? LNET_INS_LOCAL : LNET_INS_AFTER, &meh);
<<<<<<< HEAD
	if (rc != 0) {
		CERROR ("LNetMEAttach failed: %d\n", rc);
		LASSERT (rc == -ENOMEM);
=======
	if (rc) {
		CERROR("LNetMEAttach failed: %d\n", rc);
		LASSERT(rc == -ENOMEM);
>>>>>>> v4.9.227
		return -ENOMEM;
	}

	md.threshold = 1;
<<<<<<< HEAD
	md.user_ptr  = ev;
	md.start     = buf;
	md.length    = len;
	md.options   = options;
	md.eq_handle = srpc_data.rpc_lnet_eq;

	rc = LNetMDAttach(meh, md, LNET_UNLINK, mdh);
	if (rc != 0) {
		CERROR ("LNetMDAttach failed: %d\n", rc);
		LASSERT (rc == -ENOMEM);

		rc = LNetMEUnlink(meh);
		LASSERT (rc == 0);
		return -ENOMEM;
	}

	CDEBUG (D_NET,
		"Posted passive RDMA: peer %s, portal %d, matchbits %#llx\n",
		libcfs_id2str(peer), portal, matchbits);
	return 0;
}

int
srpc_post_active_rdma(int portal, __u64 matchbits, void *buf, int len,
		      int options, lnet_process_id_t peer, lnet_nid_t self,
		      lnet_handle_md_t *mdh, srpc_event_t *ev)
{
	int       rc;
	lnet_md_t md;

	md.user_ptr  = ev;
	md.start     = buf;
	md.length    = len;
	md.eq_handle = srpc_data.rpc_lnet_eq;
	md.threshold = ((options & LNET_MD_OP_GET) != 0) ? 2 : 1;
	md.options   = options & ~(LNET_MD_OP_PUT | LNET_MD_OP_GET);

	rc = LNetMDBind(md, LNET_UNLINK, mdh);
	if (rc != 0) {
		CERROR ("LNetMDBind failed: %d\n", rc);
		LASSERT (rc == -ENOMEM);
		return -ENOMEM;
	}

	/* this is kind of an abuse of the LNET_MD_OP_{PUT,GET} options.
	 * they're only meaningful for MDs attached to an ME (i.e. passive
	 * buffers... */
	if ((options & LNET_MD_OP_PUT) != 0) {
		rc = LNetPut(self, *mdh, LNET_NOACK_REQ, peer,
			     portal, matchbits, 0, 0);
	} else {
		LASSERT ((options & LNET_MD_OP_GET) != 0);
=======
	md.user_ptr = ev;
	md.start = buf;
	md.length = len;
	md.options = options;
	md.eq_handle = srpc_data.rpc_lnet_eq;

	rc = LNetMDAttach(meh, md, LNET_UNLINK, mdh);
	if (rc) {
		CERROR("LNetMDAttach failed: %d\n", rc);
		LASSERT(rc == -ENOMEM);

		rc = LNetMEUnlink(meh);
		LASSERT(!rc);
		return -ENOMEM;
	}

	CDEBUG(D_NET, "Posted passive RDMA: peer %s, portal %d, matchbits %#llx\n",
	       libcfs_id2str(peer), portal, matchbits);
	return 0;
}

static int
srpc_post_active_rdma(int portal, __u64 matchbits, void *buf, int len,
		      int options, lnet_process_id_t peer, lnet_nid_t self,
		      lnet_handle_md_t *mdh, struct srpc_event *ev)
{
	int rc;
	lnet_md_t md;

	md.user_ptr = ev;
	md.start = buf;
	md.length = len;
	md.eq_handle = srpc_data.rpc_lnet_eq;
	md.threshold = options & LNET_MD_OP_GET ? 2 : 1;
	md.options = options & ~(LNET_MD_OP_PUT | LNET_MD_OP_GET);

	rc = LNetMDBind(md, LNET_UNLINK, mdh);
	if (rc) {
		CERROR("LNetMDBind failed: %d\n", rc);
		LASSERT(rc == -ENOMEM);
		return -ENOMEM;
	}

	/*
	 * this is kind of an abuse of the LNET_MD_OP_{PUT,GET} options.
	 * they're only meaningful for MDs attached to an ME (i.e. passive
	 * buffers...
	 */
	if (options & LNET_MD_OP_PUT) {
		rc = LNetPut(self, *mdh, LNET_NOACK_REQ, peer,
			     portal, matchbits, 0, 0);
	} else {
		LASSERT(options & LNET_MD_OP_GET);
>>>>>>> v4.9.227

		rc = LNetGet(self, *mdh, peer, portal, matchbits, 0);
	}

<<<<<<< HEAD
	if (rc != 0) {
		CERROR ("LNet%s(%s, %d, %lld) failed: %d\n",
			((options & LNET_MD_OP_PUT) != 0) ? "Put" : "Get",
			libcfs_id2str(peer), portal, matchbits, rc);

		/* The forthcoming unlink event will complete this operation
		 * with failure, so fall through and return success here.
		 */
		rc = LNetMDUnlink(*mdh);
		LASSERT (rc == 0);
	} else {
		CDEBUG (D_NET,
			"Posted active RDMA: peer %s, portal %u, matchbits %#llx\n",
			libcfs_id2str(peer), portal, matchbits);
=======
	if (rc) {
		CERROR("LNet%s(%s, %d, %lld) failed: %d\n",
		       options & LNET_MD_OP_PUT ? "Put" : "Get",
		       libcfs_id2str(peer), portal, matchbits, rc);

		/*
		 * The forthcoming unlink event will complete this operation
		 * with failure, so fall through and return success here.
		 */
		rc = LNetMDUnlink(*mdh);
		LASSERT(!rc);
	} else {
		CDEBUG(D_NET, "Posted active RDMA: peer %s, portal %u, matchbits %#llx\n",
		       libcfs_id2str(peer), portal, matchbits);
>>>>>>> v4.9.227
	}
	return 0;
}

<<<<<<< HEAD
int
srpc_post_active_rqtbuf(lnet_process_id_t peer, int service, void *buf,
			int len, lnet_handle_md_t *mdh, srpc_event_t *ev)
{
	return srpc_post_active_rdma(srpc_serv_portal(service), service,
				     buf, len, LNET_MD_OP_PUT, peer,
				     LNET_NID_ANY, mdh, ev);
}

int
srpc_post_passive_rqtbuf(int service, int local, void *buf, int len,
			 lnet_handle_md_t *mdh, srpc_event_t *ev)
{
	lnet_process_id_t any = {0};
=======
static int
srpc_post_passive_rqtbuf(int service, int local, void *buf, int len,
			 lnet_handle_md_t *mdh, struct srpc_event *ev)
{
	lnet_process_id_t any = { 0 };
>>>>>>> v4.9.227

	any.nid = LNET_NID_ANY;
	any.pid = LNET_PID_ANY;

	return srpc_post_passive_rdma(srpc_serv_portal(service),
				      local, service, buf, len,
				      LNET_MD_OP_PUT, any, mdh, ev);
}

<<<<<<< HEAD
int
srpc_service_post_buffer(struct srpc_service_cd *scd, struct srpc_buffer *buf)
{
	struct srpc_service	*sv = scd->scd_svc;
	struct srpc_msg		*msg = &buf->buf_msg;
	int			rc;
=======
static int
srpc_service_post_buffer(struct srpc_service_cd *scd, struct srpc_buffer *buf)
__must_hold(&scd->scd_lock)
{
	struct srpc_service *sv = scd->scd_svc;
	struct srpc_msg	*msg = &buf->buf_msg;
	int rc;
>>>>>>> v4.9.227

	LNetInvalidateHandle(&buf->buf_mdh);
	list_add(&buf->buf_list, &scd->scd_buf_posted);
	scd->scd_buf_nposted++;
	spin_unlock(&scd->scd_lock);

	rc = srpc_post_passive_rqtbuf(sv->sv_id,
				      !srpc_serv_is_framework(sv),
				      msg, sizeof(*msg), &buf->buf_mdh,
				      &scd->scd_ev);

<<<<<<< HEAD
	/* At this point, a RPC (new or delayed) may have arrived in
	 * msg and its event handler has been called. So we must add
	 * buf to scd_buf_posted _before_ dropping scd_lock */

	spin_lock(&scd->scd_lock);

	if (rc == 0) {
=======
	/*
	 * At this point, a RPC (new or delayed) may have arrived in
	 * msg and its event handler has been called. So we must add
	 * buf to scd_buf_posted _before_ dropping scd_lock
	 */
	spin_lock(&scd->scd_lock);

	if (!rc) {
>>>>>>> v4.9.227
		if (!sv->sv_shuttingdown)
			return 0;

		spin_unlock(&scd->scd_lock);
<<<<<<< HEAD
		/* srpc_shutdown_service might have tried to unlink me
		 * when my buf_mdh was still invalid */
=======
		/*
		 * srpc_shutdown_service might have tried to unlink me
		 * when my buf_mdh was still invalid
		 */
>>>>>>> v4.9.227
		LNetMDUnlink(buf->buf_mdh);
		spin_lock(&scd->scd_lock);
		return 0;
	}

	scd->scd_buf_nposted--;
	if (sv->sv_shuttingdown)
		return rc; /* don't allow to change scd_buf_posted */

	list_del(&buf->buf_list);
	spin_unlock(&scd->scd_lock);

	LIBCFS_FREE(buf, sizeof(*buf));

	spin_lock(&scd->scd_lock);
	return rc;
}

int
srpc_add_buffer(struct swi_workitem *wi)
{
<<<<<<< HEAD
	struct srpc_service_cd	*scd = wi->swi_workitem.wi_data;
	struct srpc_buffer	*buf;
	int			rc = 0;

	/* it's called by workitem scheduler threads, these threads
	 * should have been set CPT affinity, so buffers will be posted
	 * on CPT local list of Portal */
=======
	struct srpc_service_cd *scd = wi->swi_workitem.wi_data;
	struct srpc_buffer *buf;
	int rc = 0;

	/*
	 * it's called by workitem scheduler threads, these threads
	 * should have been set CPT affinity, so buffers will be posted
	 * on CPT local list of Portal
	 */
>>>>>>> v4.9.227
	spin_lock(&scd->scd_lock);

	while (scd->scd_buf_adjust > 0 &&
	       !scd->scd_svc->sv_shuttingdown) {
		scd->scd_buf_adjust--; /* consume it */
		scd->scd_buf_posting++;

		spin_unlock(&scd->scd_lock);

		LIBCFS_ALLOC(buf, sizeof(*buf));
<<<<<<< HEAD
		if (buf == NULL) {
=======
		if (!buf) {
>>>>>>> v4.9.227
			CERROR("Failed to add new buf to service: %s\n",
			       scd->scd_svc->sv_name);
			spin_lock(&scd->scd_lock);
			rc = -ENOMEM;
			break;
		}

		spin_lock(&scd->scd_lock);
		if (scd->scd_svc->sv_shuttingdown) {
			spin_unlock(&scd->scd_lock);
			LIBCFS_FREE(buf, sizeof(*buf));

			spin_lock(&scd->scd_lock);
			rc = -ESHUTDOWN;
			break;
		}

		rc = srpc_service_post_buffer(scd, buf);
<<<<<<< HEAD
		if (rc != 0)
=======
		if (rc)
>>>>>>> v4.9.227
			break; /* buf has been freed inside */

		LASSERT(scd->scd_buf_posting > 0);
		scd->scd_buf_posting--;
		scd->scd_buf_total++;
<<<<<<< HEAD
		scd->scd_buf_low = MAX(2, scd->scd_buf_total / 4);
	}

	if (rc != 0) {
		scd->scd_buf_err_stamp = get_seconds();
=======
		scd->scd_buf_low = max(2, scd->scd_buf_total / 4);
	}

	if (rc) {
		scd->scd_buf_err_stamp = ktime_get_real_seconds();
>>>>>>> v4.9.227
		scd->scd_buf_err = rc;

		LASSERT(scd->scd_buf_posting > 0);
		scd->scd_buf_posting--;
	}

	spin_unlock(&scd->scd_lock);
	return 0;
}

int
srpc_service_add_buffers(struct srpc_service *sv, int nbuffer)
{
<<<<<<< HEAD
	struct srpc_service_cd	*scd;
	int			rc = 0;
	int			i;
=======
	struct srpc_service_cd *scd;
	int rc = 0;
	int i;
>>>>>>> v4.9.227

	LASSERTF(nbuffer > 0, "nbuffer must be positive: %d\n", nbuffer);

	cfs_percpt_for_each(scd, i, sv->sv_cpt_data) {
		spin_lock(&scd->scd_lock);

		scd->scd_buf_err = 0;
		scd->scd_buf_err_stamp = 0;
		scd->scd_buf_posting = 0;
		scd->scd_buf_adjust = nbuffer;
		/* start to post buffers */
		swi_schedule_workitem(&scd->scd_buf_wi);
		spin_unlock(&scd->scd_lock);

		/* framework service only post buffer for one partition  */
		if (srpc_serv_is_framework(sv))
			break;
	}

	cfs_percpt_for_each(scd, i, sv->sv_cpt_data) {
		spin_lock(&scd->scd_lock);
		/*
		 * NB: srpc_service_add_buffers() can be called inside
		 * thread context of lst_sched_serial, and we don't normally
		 * allow to sleep inside thread context of WI scheduler
		 * because it will block current scheduler thread from doing
		 * anything else, even worse, it could deadlock if it's
		 * waiting on result from another WI of the same scheduler.
		 * However, it's safe at here because scd_buf_wi is scheduled
		 * by thread in a different WI scheduler (lst_sched_test),
		 * so we don't have any risk of deadlock, though this could
		 * block all WIs pending on lst_sched_serial for a moment
		 * which is not good but not fatal.
		 */
<<<<<<< HEAD
		lst_wait_until(scd->scd_buf_err != 0 ||
			       (scd->scd_buf_adjust == 0 &&
				scd->scd_buf_posting == 0),
			       scd->scd_lock, "waiting for adding buffer\n");

		if (scd->scd_buf_err != 0 && rc == 0)
=======
		lst_wait_until(scd->scd_buf_err ||
			       (!scd->scd_buf_adjust &&
				!scd->scd_buf_posting),
			       scd->scd_lock, "waiting for adding buffer\n");

		if (scd->scd_buf_err && !rc)
>>>>>>> v4.9.227
			rc = scd->scd_buf_err;

		spin_unlock(&scd->scd_lock);
	}

	return rc;
}

void
srpc_service_remove_buffers(struct srpc_service *sv, int nbuffer)
{
<<<<<<< HEAD
	struct srpc_service_cd	*scd;
	int			num;
	int			i;
=======
	struct srpc_service_cd *scd;
	int num;
	int i;
>>>>>>> v4.9.227

	LASSERT(!sv->sv_shuttingdown);

	cfs_percpt_for_each(scd, i, sv->sv_cpt_data) {
		spin_lock(&scd->scd_lock);

		num = scd->scd_buf_total + scd->scd_buf_posting;
		scd->scd_buf_adjust -= min(nbuffer, num);

		spin_unlock(&scd->scd_lock);
	}
}

/* returns 1 if sv has finished, otherwise 0 */
int
srpc_finish_service(struct srpc_service *sv)
{
<<<<<<< HEAD
	struct srpc_service_cd	*scd;
	struct srpc_server_rpc	*rpc;
	int			i;
=======
	struct srpc_service_cd *scd;
	struct srpc_server_rpc *rpc;
	int i;
>>>>>>> v4.9.227

	LASSERT(sv->sv_shuttingdown); /* srpc_shutdown_service called */

	cfs_percpt_for_each(scd, i, sv->sv_cpt_data) {
		spin_lock(&scd->scd_lock);
		if (!swi_deschedule_workitem(&scd->scd_buf_wi)) {
			spin_unlock(&scd->scd_lock);
			return 0;
		}

		if (scd->scd_buf_nposted > 0) {
<<<<<<< HEAD
			CDEBUG(D_NET, "waiting for %d posted buffers to unlink",
=======
			CDEBUG(D_NET, "waiting for %d posted buffers to unlink\n",
>>>>>>> v4.9.227
			       scd->scd_buf_nposted);
			spin_unlock(&scd->scd_lock);
			return 0;
		}

		if (list_empty(&scd->scd_rpc_active)) {
			spin_unlock(&scd->scd_lock);
			continue;
		}

		rpc = list_entry(scd->scd_rpc_active.next,
<<<<<<< HEAD
				     struct srpc_server_rpc, srpc_list);
		CNETERR("Active RPC %p on shutdown: sv %s, peer %s, "
			"wi %s scheduled %d running %d, "
			"ev fired %d type %d status %d lnet %d\n",
=======
				 struct srpc_server_rpc, srpc_list);
		CNETERR("Active RPC %p on shutdown: sv %s, peer %s, wi %s scheduled %d running %d, ev fired %d type %d status %d lnet %d\n",
>>>>>>> v4.9.227
			rpc, sv->sv_name, libcfs_id2str(rpc->srpc_peer),
			swi_state2str(rpc->srpc_wi.swi_state),
			rpc->srpc_wi.swi_workitem.wi_scheduled,
			rpc->srpc_wi.swi_workitem.wi_running,
			rpc->srpc_ev.ev_fired, rpc->srpc_ev.ev_type,
			rpc->srpc_ev.ev_status, rpc->srpc_ev.ev_lnet);
		spin_unlock(&scd->scd_lock);
		return 0;
	}

	/* no lock needed from now on */
	srpc_service_fini(sv);
	return 1;
}

/* called with sv->sv_lock held */
<<<<<<< HEAD
void
srpc_service_recycle_buffer(struct srpc_service_cd *scd, srpc_buffer_t *buf)
{
	if (!scd->scd_svc->sv_shuttingdown && scd->scd_buf_adjust >= 0) {
		if (srpc_service_post_buffer(scd, buf) != 0) {
=======
static void
srpc_service_recycle_buffer(struct srpc_service_cd *scd, struct srpc_buffer *buf)
__must_hold(&scd->scd_lock)
{
	if (!scd->scd_svc->sv_shuttingdown && scd->scd_buf_adjust >= 0) {
		if (srpc_service_post_buffer(scd, buf)) {
>>>>>>> v4.9.227
			CWARN("Failed to post %s buffer\n",
			      scd->scd_svc->sv_name);
		}
		return;
	}

	/* service is shutting down, or we want to recycle some buffers */
	scd->scd_buf_total--;

	if (scd->scd_buf_adjust < 0) {
		scd->scd_buf_adjust++;
		if (scd->scd_buf_adjust < 0 &&
<<<<<<< HEAD
		    scd->scd_buf_total == 0 && scd->scd_buf_posting == 0) {
=======
		    !scd->scd_buf_total && !scd->scd_buf_posting) {
>>>>>>> v4.9.227
			CDEBUG(D_INFO,
			       "Try to recycle %d buffers but nothing left\n",
			       scd->scd_buf_adjust);
			scd->scd_buf_adjust = 0;
		}
	}

	spin_unlock(&scd->scd_lock);
	LIBCFS_FREE(buf, sizeof(*buf));
	spin_lock(&scd->scd_lock);
}

void
srpc_abort_service(struct srpc_service *sv)
{
<<<<<<< HEAD
	struct srpc_service_cd	*scd;
	struct srpc_server_rpc	*rpc;
	int			i;
=======
	struct srpc_service_cd *scd;
	struct srpc_server_rpc *rpc;
	int i;
>>>>>>> v4.9.227

	CDEBUG(D_NET, "Aborting service: id %d, name %s\n",
	       sv->sv_id, sv->sv_name);

	cfs_percpt_for_each(scd, i, sv->sv_cpt_data) {
		spin_lock(&scd->scd_lock);

<<<<<<< HEAD
		/* schedule in-flight RPCs to notice the abort, NB:
		 * racing with incoming RPCs; complete fix should make test
		 * RPCs carry session ID in its headers */
=======
		/*
		 * schedule in-flight RPCs to notice the abort, NB:
		 * racing with incoming RPCs; complete fix should make test
		 * RPCs carry session ID in its headers
		 */
>>>>>>> v4.9.227
		list_for_each_entry(rpc, &scd->scd_rpc_active, srpc_list) {
			rpc->srpc_aborted = 1;
			swi_schedule_workitem(&rpc->srpc_wi);
		}

		spin_unlock(&scd->scd_lock);
	}
}

void
<<<<<<< HEAD
srpc_shutdown_service(srpc_service_t *sv)
{
	struct srpc_service_cd	*scd;
	struct srpc_server_rpc	*rpc;
	srpc_buffer_t		*buf;
	int			i;
=======
srpc_shutdown_service(struct srpc_service *sv)
{
	struct srpc_service_cd *scd;
	struct srpc_server_rpc *rpc;
	struct srpc_buffer *buf;
	int i;
>>>>>>> v4.9.227

	CDEBUG(D_NET, "Shutting down service: id %d, name %s\n",
	       sv->sv_id, sv->sv_name);

	cfs_percpt_for_each(scd, i, sv->sv_cpt_data)
		spin_lock(&scd->scd_lock);

	sv->sv_shuttingdown = 1; /* i.e. no new active RPC */

	cfs_percpt_for_each(scd, i, sv->sv_cpt_data)
		spin_unlock(&scd->scd_lock);

	cfs_percpt_for_each(scd, i, sv->sv_cpt_data) {
		spin_lock(&scd->scd_lock);

		/* schedule in-flight RPCs to notice the shutdown */
		list_for_each_entry(rpc, &scd->scd_rpc_active, srpc_list)
			swi_schedule_workitem(&rpc->srpc_wi);

		spin_unlock(&scd->scd_lock);

<<<<<<< HEAD
		/* OK to traverse scd_buf_posted without lock, since no one
		 * touches scd_buf_posted now */
=======
		/*
		 * OK to traverse scd_buf_posted without lock, since no one
		 * touches scd_buf_posted now
		 */
>>>>>>> v4.9.227
		list_for_each_entry(buf, &scd->scd_buf_posted, buf_list)
			LNetMDUnlink(buf->buf_mdh);
	}
}

<<<<<<< HEAD
int
srpc_send_request (srpc_client_rpc_t *rpc)
{
	srpc_event_t *ev = &rpc->crpc_reqstev;
	int	   rc;

	ev->ev_fired = 0;
	ev->ev_data  = rpc;
	ev->ev_type  = SRPC_REQUEST_SENT;

	rc = srpc_post_active_rqtbuf(rpc->crpc_dest, rpc->crpc_service,
				     &rpc->crpc_reqstmsg, sizeof(srpc_msg_t),
				     &rpc->crpc_reqstmdh, ev);
	if (rc != 0) {
		LASSERT (rc == -ENOMEM);
=======
static int
srpc_send_request(struct srpc_client_rpc *rpc)
{
	struct srpc_event *ev = &rpc->crpc_reqstev;
	int rc;

	ev->ev_fired = 0;
	ev->ev_data = rpc;
	ev->ev_type = SRPC_REQUEST_SENT;

	 rc = srpc_post_active_rdma(srpc_serv_portal(rpc->crpc_service),
				    rpc->crpc_service, &rpc->crpc_reqstmsg,
				    sizeof(struct srpc_msg), LNET_MD_OP_PUT,
				    rpc->crpc_dest, LNET_NID_ANY,
				    &rpc->crpc_reqstmdh, ev);
	if (rc) {
		LASSERT(rc == -ENOMEM);
>>>>>>> v4.9.227
		ev->ev_fired = 1;  /* no more event expected */
	}
	return rc;
}

<<<<<<< HEAD
int
srpc_prepare_reply (srpc_client_rpc_t *rpc)
{
	srpc_event_t *ev = &rpc->crpc_replyev;
	__u64	*id = &rpc->crpc_reqstmsg.msg_body.reqst.rpyid;
	int	   rc;

	ev->ev_fired = 0;
	ev->ev_data  = rpc;
	ev->ev_type  = SRPC_REPLY_RCVD;
=======
static int
srpc_prepare_reply(struct srpc_client_rpc *rpc)
{
	struct srpc_event *ev = &rpc->crpc_replyev;
	__u64 *id = &rpc->crpc_reqstmsg.msg_body.reqst.rpyid;
	int rc;

	ev->ev_fired = 0;
	ev->ev_data = rpc;
	ev->ev_type = SRPC_REPLY_RCVD;
>>>>>>> v4.9.227

	*id = srpc_next_id();

	rc = srpc_post_passive_rdma(SRPC_RDMA_PORTAL, 0, *id,
<<<<<<< HEAD
				    &rpc->crpc_replymsg, sizeof(srpc_msg_t),
				    LNET_MD_OP_PUT, rpc->crpc_dest,
				    &rpc->crpc_replymdh, ev);
	if (rc != 0) {
		LASSERT (rc == -ENOMEM);
=======
				    &rpc->crpc_replymsg,
				    sizeof(struct srpc_msg),
				    LNET_MD_OP_PUT, rpc->crpc_dest,
				    &rpc->crpc_replymdh, ev);
	if (rc) {
		LASSERT(rc == -ENOMEM);
>>>>>>> v4.9.227
		ev->ev_fired = 1;  /* no more event expected */
	}
	return rc;
}

<<<<<<< HEAD
int
srpc_prepare_bulk (srpc_client_rpc_t *rpc)
{
	srpc_bulk_t  *bk = &rpc->crpc_bulk;
	srpc_event_t *ev = &rpc->crpc_bulkev;
	__u64	*id = &rpc->crpc_reqstmsg.msg_body.reqst.bulkid;
	int	   rc;
	int	   opt;

	LASSERT (bk->bk_niov <= LNET_MAX_IOV);

	if (bk->bk_niov == 0) return 0; /* nothing to do */
=======
static int
srpc_prepare_bulk(struct srpc_client_rpc *rpc)
{
	struct srpc_bulk *bk = &rpc->crpc_bulk;
	struct srpc_event *ev = &rpc->crpc_bulkev;
	__u64 *id = &rpc->crpc_reqstmsg.msg_body.reqst.bulkid;
	int rc;
	int opt;

	LASSERT(bk->bk_niov <= LNET_MAX_IOV);

	if (!bk->bk_niov)
		return 0; /* nothing to do */
>>>>>>> v4.9.227

	opt = bk->bk_sink ? LNET_MD_OP_PUT : LNET_MD_OP_GET;
	opt |= LNET_MD_KIOV;

	ev->ev_fired = 0;
<<<<<<< HEAD
	ev->ev_data  = rpc;
	ev->ev_type  = SRPC_BULK_REQ_RCVD;
=======
	ev->ev_data = rpc;
	ev->ev_type = SRPC_BULK_REQ_RCVD;
>>>>>>> v4.9.227

	*id = srpc_next_id();

	rc = srpc_post_passive_rdma(SRPC_RDMA_PORTAL, 0, *id,
				    &bk->bk_iovs[0], bk->bk_niov, opt,
				    rpc->crpc_dest, &bk->bk_mdh, ev);
<<<<<<< HEAD
	if (rc != 0) {
		LASSERT (rc == -ENOMEM);
=======
	if (rc) {
		LASSERT(rc == -ENOMEM);
>>>>>>> v4.9.227
		ev->ev_fired = 1;  /* no more event expected */
	}
	return rc;
}

<<<<<<< HEAD
int
srpc_do_bulk (srpc_server_rpc_t *rpc)
{
	srpc_event_t  *ev = &rpc->srpc_ev;
	srpc_bulk_t   *bk = rpc->srpc_bulk;
	__u64	  id = rpc->srpc_reqstbuf->buf_msg.msg_body.reqst.bulkid;
	int	    rc;
	int	    opt;

	LASSERT (bk != NULL);
=======
static int
srpc_do_bulk(struct srpc_server_rpc *rpc)
{
	struct srpc_event *ev = &rpc->srpc_ev;
	struct srpc_bulk *bk = rpc->srpc_bulk;
	__u64 id = rpc->srpc_reqstbuf->buf_msg.msg_body.reqst.bulkid;
	int rc;
	int opt;

	LASSERT(bk);
>>>>>>> v4.9.227

	opt = bk->bk_sink ? LNET_MD_OP_GET : LNET_MD_OP_PUT;
	opt |= LNET_MD_KIOV;

	ev->ev_fired = 0;
<<<<<<< HEAD
	ev->ev_data  = rpc;
	ev->ev_type  = bk->bk_sink ? SRPC_BULK_GET_RPLD : SRPC_BULK_PUT_SENT;
=======
	ev->ev_data = rpc;
	ev->ev_type = bk->bk_sink ? SRPC_BULK_GET_RPLD : SRPC_BULK_PUT_SENT;
>>>>>>> v4.9.227

	rc = srpc_post_active_rdma(SRPC_RDMA_PORTAL, id,
				   &bk->bk_iovs[0], bk->bk_niov, opt,
				   rpc->srpc_peer, rpc->srpc_self,
				   &bk->bk_mdh, ev);
<<<<<<< HEAD
	if (rc != 0)
=======
	if (rc)
>>>>>>> v4.9.227
		ev->ev_fired = 1;  /* no more event expected */
	return rc;
}

/* only called from srpc_handle_rpc */
<<<<<<< HEAD
void
srpc_server_rpc_done(srpc_server_rpc_t *rpc, int status)
{
	struct srpc_service_cd	*scd = rpc->srpc_scd;
	struct srpc_service	*sv  = scd->scd_svc;
	srpc_buffer_t		*buffer;

	LASSERT (status != 0 || rpc->srpc_wi.swi_state == SWI_STATE_DONE);

	rpc->srpc_status = status;

	CDEBUG_LIMIT (status == 0 ? D_NET : D_NETERROR,
		"Server RPC %p done: service %s, peer %s, status %s:%d\n",
		rpc, sv->sv_name, libcfs_id2str(rpc->srpc_peer),
		swi_state2str(rpc->srpc_wi.swi_state), status);

	if (status != 0) {
=======
static void
srpc_server_rpc_done(struct srpc_server_rpc *rpc, int status)
{
	struct srpc_service_cd *scd = rpc->srpc_scd;
	struct srpc_service *sv = scd->scd_svc;
	struct srpc_buffer *buffer;

	LASSERT(status || rpc->srpc_wi.swi_state == SWI_STATE_DONE);

	rpc->srpc_status = status;

	CDEBUG_LIMIT(!status ? D_NET : D_NETERROR,
		     "Server RPC %p done: service %s, peer %s, status %s:%d\n",
		     rpc, sv->sv_name, libcfs_id2str(rpc->srpc_peer),
		     swi_state2str(rpc->srpc_wi.swi_state), status);

	if (status) {
>>>>>>> v4.9.227
		spin_lock(&srpc_data.rpc_glock);
		srpc_data.rpc_counters.rpcs_dropped++;
		spin_unlock(&srpc_data.rpc_glock);
	}

<<<<<<< HEAD
	if (rpc->srpc_done != NULL)
		(*rpc->srpc_done) (rpc);
	LASSERT(rpc->srpc_bulk == NULL);

	spin_lock(&scd->scd_lock);

	if (rpc->srpc_reqstbuf != NULL) {
		/* NB might drop sv_lock in srpc_service_recycle_buffer, but
		 * sv won't go away for scd_rpc_active must not be empty */
=======
	if (rpc->srpc_done)
		(*rpc->srpc_done) (rpc);
	LASSERT(!rpc->srpc_bulk);

	spin_lock(&scd->scd_lock);

	if (rpc->srpc_reqstbuf) {
		/*
		 * NB might drop sv_lock in srpc_service_recycle_buffer, but
		 * sv won't go away for scd_rpc_active must not be empty
		 */
>>>>>>> v4.9.227
		srpc_service_recycle_buffer(scd, rpc->srpc_reqstbuf);
		rpc->srpc_reqstbuf = NULL;
	}

	list_del(&rpc->srpc_list); /* from scd->scd_rpc_active */

	/*
	 * No one can schedule me now since:
	 * - I'm not on scd_rpc_active.
	 * - all LNet events have been fired.
	 * Cancel pending schedules and prevent future schedule attempts:
	 */
	LASSERT(rpc->srpc_ev.ev_fired);
	swi_exit_workitem(&rpc->srpc_wi);

	if (!sv->sv_shuttingdown && !list_empty(&scd->scd_buf_blocked)) {
		buffer = list_entry(scd->scd_buf_blocked.next,
<<<<<<< HEAD
					srpc_buffer_t, buf_list);
=======
				    struct srpc_buffer, buf_list);
>>>>>>> v4.9.227
		list_del(&buffer->buf_list);

		srpc_init_server_rpc(rpc, scd, buffer);
		list_add_tail(&rpc->srpc_list, &scd->scd_rpc_active);
		swi_schedule_workitem(&rpc->srpc_wi);
	} else {
		list_add(&rpc->srpc_list, &scd->scd_rpc_free);
	}

	spin_unlock(&scd->scd_lock);
<<<<<<< HEAD
	return;
=======
>>>>>>> v4.9.227
}

/* handles an incoming RPC */
int
<<<<<<< HEAD
srpc_handle_rpc(swi_workitem_t *wi)
{
	struct srpc_server_rpc	*rpc = wi->swi_workitem.wi_data;
	struct srpc_service_cd	*scd = rpc->srpc_scd;
	struct srpc_service	*sv = scd->scd_svc;
	srpc_event_t		*ev = &rpc->srpc_ev;
	int			rc = 0;
=======
srpc_handle_rpc(struct swi_workitem *wi)
{
	struct srpc_server_rpc *rpc = wi->swi_workitem.wi_data;
	struct srpc_service_cd *scd = rpc->srpc_scd;
	struct srpc_service *sv = scd->scd_svc;
	struct srpc_event *ev = &rpc->srpc_ev;
	int rc = 0;
>>>>>>> v4.9.227

	LASSERT(wi == &rpc->srpc_wi);

	spin_lock(&scd->scd_lock);

	if (sv->sv_shuttingdown || rpc->srpc_aborted) {
		spin_unlock(&scd->scd_lock);

<<<<<<< HEAD
		if (rpc->srpc_bulk != NULL)
=======
		if (rpc->srpc_bulk)
>>>>>>> v4.9.227
			LNetMDUnlink(rpc->srpc_bulk->bk_mdh);
		LNetMDUnlink(rpc->srpc_replymdh);

		if (ev->ev_fired) { /* no more event, OK to finish */
			srpc_server_rpc_done(rpc, -ESHUTDOWN);
			return 1;
		}
		return 0;
	}

	spin_unlock(&scd->scd_lock);

	switch (wi->swi_state) {
	default:
<<<<<<< HEAD
		LBUG ();
	case SWI_STATE_NEWBORN: {
		srpc_msg_t	   *msg;
		srpc_generic_reply_t *reply;
=======
		LBUG();
	case SWI_STATE_NEWBORN: {
		struct srpc_msg *msg;
		struct srpc_generic_reply *reply;
>>>>>>> v4.9.227

		msg = &rpc->srpc_reqstbuf->buf_msg;
		reply = &rpc->srpc_replymsg.msg_body.reply;

<<<<<<< HEAD
		if (msg->msg_magic == 0) {
=======
		if (!msg->msg_magic) {
>>>>>>> v4.9.227
			/* moaned already in srpc_lnet_ev_handler */
			srpc_server_rpc_done(rpc, EBADMSG);
			return 1;
		}

		srpc_unpack_msg_hdr(msg);
		if (msg->msg_version != SRPC_MSG_VERSION) {
			CWARN("Version mismatch: %u, %u expected, from %s\n",
			      msg->msg_version, SRPC_MSG_VERSION,
			      libcfs_id2str(rpc->srpc_peer));
			reply->status = EPROTO;
			/* drop through and send reply */
		} else {
			reply->status = 0;
			rc = (*sv->sv_handler)(rpc);
<<<<<<< HEAD
			LASSERT(reply->status == 0 || !rpc->srpc_bulk);
			if (rc != 0) {
=======
			LASSERT(!reply->status || !rpc->srpc_bulk);
			if (rc) {
>>>>>>> v4.9.227
				srpc_server_rpc_done(rpc, rc);
				return 1;
			}
		}

		wi->swi_state = SWI_STATE_BULK_STARTED;

<<<<<<< HEAD
		if (rpc->srpc_bulk != NULL) {
			rc = srpc_do_bulk(rpc);
			if (rc == 0)
				return 0; /* wait for bulk */

			LASSERT (ev->ev_fired);
=======
		if (rpc->srpc_bulk) {
			rc = srpc_do_bulk(rpc);
			if (!rc)
				return 0; /* wait for bulk */

			LASSERT(ev->ev_fired);
>>>>>>> v4.9.227
			ev->ev_status = rc;
		}
	}
	case SWI_STATE_BULK_STARTED:
<<<<<<< HEAD
		LASSERT (rpc->srpc_bulk == NULL || ev->ev_fired);

		if (rpc->srpc_bulk != NULL) {
			rc = ev->ev_status;

			if (sv->sv_bulk_ready != NULL)
				rc = (*sv->sv_bulk_ready) (rpc, rc);

			if (rc != 0) {
=======
		LASSERT(!rpc->srpc_bulk || ev->ev_fired);

		if (rpc->srpc_bulk) {
			rc = ev->ev_status;

			if (sv->sv_bulk_ready)
				rc = (*sv->sv_bulk_ready) (rpc, rc);

			if (rc) {
>>>>>>> v4.9.227
				srpc_server_rpc_done(rpc, rc);
				return 1;
			}
		}

		wi->swi_state = SWI_STATE_REPLY_SUBMITTED;
		rc = srpc_send_reply(rpc);
<<<<<<< HEAD
		if (rc == 0)
=======
		if (!rc)
>>>>>>> v4.9.227
			return 0; /* wait for reply */
		srpc_server_rpc_done(rpc, rc);
		return 1;

	case SWI_STATE_REPLY_SUBMITTED:
		if (!ev->ev_fired) {
			CERROR("RPC %p: bulk %p, service %d\n",
			       rpc, rpc->srpc_bulk, sv->sv_id);
			CERROR("Event: status %d, type %d, lnet %d\n",
			       ev->ev_status, ev->ev_type, ev->ev_lnet);
<<<<<<< HEAD
			LASSERT (ev->ev_fired);
=======
			LASSERT(ev->ev_fired);
>>>>>>> v4.9.227
		}

		wi->swi_state = SWI_STATE_DONE;
		srpc_server_rpc_done(rpc, ev->ev_status);
		return 1;
	}

	return 0;
}

<<<<<<< HEAD
void
srpc_client_rpc_expired (void *data)
{
	srpc_client_rpc_t *rpc = data;

	CWARN ("Client RPC expired: service %d, peer %s, timeout %d.\n",
	       rpc->crpc_service, libcfs_id2str(rpc->crpc_dest),
	       rpc->crpc_timeout);
=======
static void
srpc_client_rpc_expired(void *data)
{
	struct srpc_client_rpc *rpc = data;

	CWARN("Client RPC expired: service %d, peer %s, timeout %d.\n",
	      rpc->crpc_service, libcfs_id2str(rpc->crpc_dest),
	      rpc->crpc_timeout);
>>>>>>> v4.9.227

	spin_lock(&rpc->crpc_lock);

	rpc->crpc_timeout = 0;
	srpc_abort_rpc(rpc, -ETIMEDOUT);

	spin_unlock(&rpc->crpc_lock);

	spin_lock(&srpc_data.rpc_glock);
	srpc_data.rpc_counters.rpcs_expired++;
	spin_unlock(&srpc_data.rpc_glock);
}

<<<<<<< HEAD
inline void
srpc_add_client_rpc_timer (srpc_client_rpc_t *rpc)
{
	stt_timer_t *timer = &rpc->crpc_timer;

	if (rpc->crpc_timeout == 0) return;

	INIT_LIST_HEAD(&timer->stt_list);
	timer->stt_data    = rpc;
	timer->stt_func    = srpc_client_rpc_expired;
	timer->stt_expires = cfs_time_add(rpc->crpc_timeout,
					  get_seconds());
	stt_add_timer(timer);
	return;
=======
static void
srpc_add_client_rpc_timer(struct srpc_client_rpc *rpc)
{
	struct stt_timer *timer = &rpc->crpc_timer;

	if (!rpc->crpc_timeout)
		return;

	INIT_LIST_HEAD(&timer->stt_list);
	timer->stt_data	= rpc;
	timer->stt_func	= srpc_client_rpc_expired;
	timer->stt_expires = ktime_get_real_seconds() + rpc->crpc_timeout;
	stt_add_timer(timer);
>>>>>>> v4.9.227
}

/*
 * Called with rpc->crpc_lock held.
 *
 * Upon exit the RPC expiry timer is not queued and the handler is not
<<<<<<< HEAD
 * running on any CPU. */
void
srpc_del_client_rpc_timer (srpc_client_rpc_t *rpc)
{
	/* timer not planted or already exploded */
	if (rpc->crpc_timeout == 0)
=======
 * running on any CPU.
 */
static void
srpc_del_client_rpc_timer(struct srpc_client_rpc *rpc)
{
	/* timer not planted or already exploded */
	if (!rpc->crpc_timeout)
>>>>>>> v4.9.227
		return;

	/* timer successfully defused */
	if (stt_del_timer(&rpc->crpc_timer))
		return;

	/* timer detonated, wait for it to explode */
<<<<<<< HEAD
	while (rpc->crpc_timeout != 0) {
=======
	while (rpc->crpc_timeout) {
>>>>>>> v4.9.227
		spin_unlock(&rpc->crpc_lock);

		schedule();

		spin_lock(&rpc->crpc_lock);
	}
}

<<<<<<< HEAD
void
srpc_client_rpc_done (srpc_client_rpc_t *rpc, int status)
{
	swi_workitem_t *wi = &rpc->crpc_wi;

	LASSERT(status != 0 || wi->swi_state == SWI_STATE_DONE);
=======
static void
srpc_client_rpc_done(struct srpc_client_rpc *rpc, int status)
{
	struct swi_workitem *wi = &rpc->crpc_wi;

	LASSERT(status || wi->swi_state == SWI_STATE_DONE);
>>>>>>> v4.9.227

	spin_lock(&rpc->crpc_lock);

	rpc->crpc_closed = 1;
<<<<<<< HEAD
	if (rpc->crpc_status == 0)
=======
	if (!rpc->crpc_status)
>>>>>>> v4.9.227
		rpc->crpc_status = status;

	srpc_del_client_rpc_timer(rpc);

<<<<<<< HEAD
	CDEBUG_LIMIT ((status == 0) ? D_NET : D_NETERROR,
		"Client RPC done: service %d, peer %s, status %s:%d:%d\n",
		rpc->crpc_service, libcfs_id2str(rpc->crpc_dest),
		swi_state2str(wi->swi_state), rpc->crpc_aborted, status);
=======
	CDEBUG_LIMIT(!status ? D_NET : D_NETERROR,
		     "Client RPC done: service %d, peer %s, status %s:%d:%d\n",
		     rpc->crpc_service, libcfs_id2str(rpc->crpc_dest),
		     swi_state2str(wi->swi_state), rpc->crpc_aborted, status);
>>>>>>> v4.9.227

	/*
	 * No one can schedule me now since:
	 * - RPC timer has been defused.
	 * - all LNet events have been fired.
	 * - crpc_closed has been set, preventing srpc_abort_rpc from
	 *   scheduling me.
	 * Cancel pending schedules and prevent future schedule attempts:
	 */
<<<<<<< HEAD
	LASSERT (!srpc_event_pending(rpc));
=======
	LASSERT(!srpc_event_pending(rpc));
>>>>>>> v4.9.227
	swi_exit_workitem(wi);

	spin_unlock(&rpc->crpc_lock);

	(*rpc->crpc_done)(rpc);
<<<<<<< HEAD
	return;
=======
>>>>>>> v4.9.227
}

/* sends an outgoing RPC */
int
<<<<<<< HEAD
srpc_send_rpc (swi_workitem_t *wi)
{
	int		rc = 0;
	srpc_client_rpc_t *rpc;
	srpc_msg_t	*reply;
	int		do_bulk;

	LASSERT(wi != NULL);

	rpc = wi->swi_workitem.wi_data;

	LASSERT (rpc != NULL);
	LASSERT (wi == &rpc->crpc_wi);
=======
srpc_send_rpc(struct swi_workitem *wi)
{
	int rc = 0;
	struct srpc_client_rpc *rpc;
	struct srpc_msg *reply;
	int do_bulk;

	LASSERT(wi);

	rpc = wi->swi_workitem.wi_data;

	LASSERT(rpc);
	LASSERT(wi == &rpc->crpc_wi);
>>>>>>> v4.9.227

	reply = &rpc->crpc_replymsg;
	do_bulk = rpc->crpc_bulk.bk_niov > 0;

	spin_lock(&rpc->crpc_lock);

	if (rpc->crpc_aborted) {
		spin_unlock(&rpc->crpc_lock);
		goto abort;
	}

	spin_unlock(&rpc->crpc_lock);

	switch (wi->swi_state) {
	default:
<<<<<<< HEAD
		LBUG ();
	case SWI_STATE_NEWBORN:
		LASSERT (!srpc_event_pending(rpc));

		rc = srpc_prepare_reply(rpc);
		if (rc != 0) {
=======
		LBUG();
	case SWI_STATE_NEWBORN:
		LASSERT(!srpc_event_pending(rpc));

		rc = srpc_prepare_reply(rpc);
		if (rc) {
>>>>>>> v4.9.227
			srpc_client_rpc_done(rpc, rc);
			return 1;
		}

		rc = srpc_prepare_bulk(rpc);
<<<<<<< HEAD
		if (rc != 0) break;
=======
		if (rc)
			break;
>>>>>>> v4.9.227

		wi->swi_state = SWI_STATE_REQUEST_SUBMITTED;
		rc = srpc_send_request(rpc);
		break;

	case SWI_STATE_REQUEST_SUBMITTED:
<<<<<<< HEAD
		/* CAVEAT EMPTOR: rqtev, rpyev, and bulkev may come in any
		 * order; however, they're processed in a strict order:
		 * rqt, rpy, and bulk. */
		if (!rpc->crpc_reqstev.ev_fired) break;

		rc = rpc->crpc_reqstev.ev_status;
		if (rc != 0) break;
=======
		/*
		 * CAVEAT EMPTOR: rqtev, rpyev, and bulkev may come in any
		 * order; however, they're processed in a strict order:
		 * rqt, rpy, and bulk.
		 */
		if (!rpc->crpc_reqstev.ev_fired)
			break;

		rc = rpc->crpc_reqstev.ev_status;
		if (rc)
			break;
>>>>>>> v4.9.227

		wi->swi_state = SWI_STATE_REQUEST_SENT;
		/* perhaps more events, fall thru */
	case SWI_STATE_REQUEST_SENT: {
<<<<<<< HEAD
		srpc_msg_type_t type = srpc_service2reply(rpc->crpc_service);

		if (!rpc->crpc_replyev.ev_fired) break;

		rc = rpc->crpc_replyev.ev_status;
		if (rc != 0) break;
=======
		enum srpc_msg_type type = srpc_service2reply(rpc->crpc_service);

		if (!rpc->crpc_replyev.ev_fired)
			break;

		rc = rpc->crpc_replyev.ev_status;
		if (rc)
			break;
>>>>>>> v4.9.227

		srpc_unpack_msg_hdr(reply);
		if (reply->msg_type != type ||
		    (reply->msg_magic != SRPC_MSG_MAGIC &&
		     reply->msg_magic != __swab32(SRPC_MSG_MAGIC))) {
<<<<<<< HEAD
			CWARN ("Bad message from %s: type %u (%d expected),"
			       " magic %u (%d expected).\n",
			       libcfs_id2str(rpc->crpc_dest),
			       reply->msg_type, type,
			       reply->msg_magic, SRPC_MSG_MAGIC);
=======
			CWARN("Bad message from %s: type %u (%d expected), magic %u (%d expected).\n",
			      libcfs_id2str(rpc->crpc_dest),
			      reply->msg_type, type,
			      reply->msg_magic, SRPC_MSG_MAGIC);
>>>>>>> v4.9.227
			rc = -EBADMSG;
			break;
		}

<<<<<<< HEAD
		if (do_bulk && reply->msg_body.reply.status != 0) {
			CWARN ("Remote error %d at %s, unlink bulk buffer in "
			       "case peer didn't initiate bulk transfer\n",
			       reply->msg_body.reply.status,
			       libcfs_id2str(rpc->crpc_dest));
=======
		if (do_bulk && reply->msg_body.reply.status) {
			CWARN("Remote error %d at %s, unlink bulk buffer in case peer didn't initiate bulk transfer\n",
			      reply->msg_body.reply.status,
			      libcfs_id2str(rpc->crpc_dest));
>>>>>>> v4.9.227
			LNetMDUnlink(rpc->crpc_bulk.bk_mdh);
		}

		wi->swi_state = SWI_STATE_REPLY_RECEIVED;
	}
	case SWI_STATE_REPLY_RECEIVED:
<<<<<<< HEAD
		if (do_bulk && !rpc->crpc_bulkev.ev_fired) break;

		rc = do_bulk ? rpc->crpc_bulkev.ev_status : 0;

		/* Bulk buffer was unlinked due to remote error. Clear error
		 * since reply buffer still contains valid data.
		 * NB rpc->crpc_done shouldn't look into bulk data in case of
		 * remote error. */
		if (do_bulk && rpc->crpc_bulkev.ev_lnet == LNET_EVENT_UNLINK &&
		    rpc->crpc_status == 0 && reply->msg_body.reply.status != 0)
=======
		if (do_bulk && !rpc->crpc_bulkev.ev_fired)
			break;

		rc = do_bulk ? rpc->crpc_bulkev.ev_status : 0;

		/*
		 * Bulk buffer was unlinked due to remote error. Clear error
		 * since reply buffer still contains valid data.
		 * NB rpc->crpc_done shouldn't look into bulk data in case of
		 * remote error.
		 */
		if (do_bulk && rpc->crpc_bulkev.ev_lnet == LNET_EVENT_UNLINK &&
		    !rpc->crpc_status && reply->msg_body.reply.status)
>>>>>>> v4.9.227
			rc = 0;

		wi->swi_state = SWI_STATE_DONE;
		srpc_client_rpc_done(rpc, rc);
		return 1;
	}

<<<<<<< HEAD
	if (rc != 0) {
=======
	if (rc) {
>>>>>>> v4.9.227
		spin_lock(&rpc->crpc_lock);
		srpc_abort_rpc(rpc, rc);
		spin_unlock(&rpc->crpc_lock);
	}

abort:
	if (rpc->crpc_aborted) {
		LNetMDUnlink(rpc->crpc_reqstmdh);
		LNetMDUnlink(rpc->crpc_replymdh);
		LNetMDUnlink(rpc->crpc_bulk.bk_mdh);

		if (!srpc_event_pending(rpc)) {
			srpc_client_rpc_done(rpc, -EINTR);
			return 1;
		}
	}
	return 0;
}

<<<<<<< HEAD
srpc_client_rpc_t *
srpc_create_client_rpc (lnet_process_id_t peer, int service,
			int nbulkiov, int bulklen,
			void (*rpc_done)(srpc_client_rpc_t *),
			void (*rpc_fini)(srpc_client_rpc_t *), void *priv)
{
	srpc_client_rpc_t *rpc;

	LIBCFS_ALLOC(rpc, offsetof(srpc_client_rpc_t,
				   crpc_bulk.bk_iovs[nbulkiov]));
	if (rpc == NULL)
=======
struct srpc_client_rpc *
srpc_create_client_rpc(lnet_process_id_t peer, int service,
		       int nbulkiov, int bulklen,
		       void (*rpc_done)(struct srpc_client_rpc *),
		       void (*rpc_fini)(struct srpc_client_rpc *), void *priv)
{
	struct srpc_client_rpc *rpc;

	LIBCFS_ALLOC(rpc, offsetof(struct srpc_client_rpc,
				   crpc_bulk.bk_iovs[nbulkiov]));
	if (!rpc)
>>>>>>> v4.9.227
		return NULL;

	srpc_init_client_rpc(rpc, peer, service, nbulkiov,
			     bulklen, rpc_done, rpc_fini, priv);
	return rpc;
}

/* called with rpc->crpc_lock held */
void
<<<<<<< HEAD
srpc_abort_rpc (srpc_client_rpc_t *rpc, int why)
{
	LASSERT (why != 0);

	if (rpc->crpc_aborted || /* already aborted */
	    rpc->crpc_closed)    /* callback imminent */
		return;

	CDEBUG (D_NET,
		"Aborting RPC: service %d, peer %s, state %s, why %d\n",
		rpc->crpc_service, libcfs_id2str(rpc->crpc_dest),
		swi_state2str(rpc->crpc_wi.swi_state), why);

	rpc->crpc_aborted = 1;
	rpc->crpc_status  = why;
	swi_schedule_workitem(&rpc->crpc_wi);
	return;
=======
srpc_abort_rpc(struct srpc_client_rpc *rpc, int why)
{
	LASSERT(why);

	if (rpc->crpc_aborted ||	/* already aborted */
	    rpc->crpc_closed)		/* callback imminent */
		return;

	CDEBUG(D_NET, "Aborting RPC: service %d, peer %s, state %s, why %d\n",
	       rpc->crpc_service, libcfs_id2str(rpc->crpc_dest),
	       swi_state2str(rpc->crpc_wi.swi_state), why);

	rpc->crpc_aborted = 1;
	rpc->crpc_status = why;
	swi_schedule_workitem(&rpc->crpc_wi);
>>>>>>> v4.9.227
}

/* called with rpc->crpc_lock held */
void
<<<<<<< HEAD
srpc_post_rpc (srpc_client_rpc_t *rpc)
{
	LASSERT (!rpc->crpc_aborted);
	LASSERT (srpc_data.rpc_state == SRPC_STATE_RUNNING);

	CDEBUG (D_NET, "Posting RPC: peer %s, service %d, timeout %d\n",
		libcfs_id2str(rpc->crpc_dest), rpc->crpc_service,
		rpc->crpc_timeout);

	srpc_add_client_rpc_timer(rpc);
	swi_schedule_workitem(&rpc->crpc_wi);
	return;
}


int
srpc_send_reply(struct srpc_server_rpc *rpc)
{
	srpc_event_t		*ev = &rpc->srpc_ev;
	struct srpc_msg		*msg = &rpc->srpc_replymsg;
	struct srpc_buffer	*buffer = rpc->srpc_reqstbuf;
	struct srpc_service_cd	*scd = rpc->srpc_scd;
	struct srpc_service	*sv = scd->scd_svc;
	__u64			rpyid;
	int			rc;

	LASSERT(buffer != NULL);
=======
srpc_post_rpc(struct srpc_client_rpc *rpc)
{
	LASSERT(!rpc->crpc_aborted);
	LASSERT(srpc_data.rpc_state == SRPC_STATE_RUNNING);

	CDEBUG(D_NET, "Posting RPC: peer %s, service %d, timeout %d\n",
	       libcfs_id2str(rpc->crpc_dest), rpc->crpc_service,
	       rpc->crpc_timeout);

	srpc_add_client_rpc_timer(rpc);
	swi_schedule_workitem(&rpc->crpc_wi);
}

int
srpc_send_reply(struct srpc_server_rpc *rpc)
{
	struct srpc_event *ev = &rpc->srpc_ev;
	struct srpc_msg *msg = &rpc->srpc_replymsg;
	struct srpc_buffer *buffer = rpc->srpc_reqstbuf;
	struct srpc_service_cd *scd = rpc->srpc_scd;
	struct srpc_service *sv = scd->scd_svc;
	__u64 rpyid;
	int rc;

	LASSERT(buffer);
>>>>>>> v4.9.227
	rpyid = buffer->buf_msg.msg_body.reqst.rpyid;

	spin_lock(&scd->scd_lock);

	if (!sv->sv_shuttingdown && !srpc_serv_is_framework(sv)) {
<<<<<<< HEAD
		/* Repost buffer before replying since test client
		 * might send me another RPC once it gets the reply */
		if (srpc_service_post_buffer(scd, buffer) != 0)
=======
		/*
		 * Repost buffer before replying since test client
		 * might send me another RPC once it gets the reply
		 */
		if (srpc_service_post_buffer(scd, buffer))
>>>>>>> v4.9.227
			CWARN("Failed to repost %s buffer\n", sv->sv_name);
		rpc->srpc_reqstbuf = NULL;
	}

	spin_unlock(&scd->scd_lock);

	ev->ev_fired = 0;
<<<<<<< HEAD
	ev->ev_data  = rpc;
	ev->ev_type  = SRPC_REPLY_SENT;

	msg->msg_magic   = SRPC_MSG_MAGIC;
	msg->msg_version = SRPC_MSG_VERSION;
	msg->msg_type    = srpc_service2reply(sv->sv_id);
=======
	ev->ev_data = rpc;
	ev->ev_type = SRPC_REPLY_SENT;

	msg->msg_magic = SRPC_MSG_MAGIC;
	msg->msg_version = SRPC_MSG_VERSION;
	msg->msg_type = srpc_service2reply(sv->sv_id);
>>>>>>> v4.9.227

	rc = srpc_post_active_rdma(SRPC_RDMA_PORTAL, rpyid, msg,
				   sizeof(*msg), LNET_MD_OP_PUT,
				   rpc->srpc_peer, rpc->srpc_self,
				   &rpc->srpc_replymdh, ev);
<<<<<<< HEAD
	if (rc != 0)
		ev->ev_fired = 1;  /* no more event expected */
=======
	if (rc)
		ev->ev_fired = 1; /* no more event expected */
>>>>>>> v4.9.227
	return rc;
}

/* when in kernel always called with LNET_LOCK() held, and in thread context */
<<<<<<< HEAD
void
srpc_lnet_ev_handler(lnet_event_t *ev)
{
	struct srpc_service_cd	*scd;
	srpc_event_t      *rpcev = ev->md.user_ptr;
	srpc_client_rpc_t *crpc;
	srpc_server_rpc_t *srpc;
	srpc_buffer_t     *buffer;
	srpc_service_t    *sv;
	srpc_msg_t	*msg;
	srpc_msg_type_t    type;

	LASSERT (!in_interrupt());

	if (ev->status != 0) {
		spin_lock(&srpc_data.rpc_glock);
		srpc_data.rpc_counters.errors++;
		spin_unlock(&srpc_data.rpc_glock);
=======
static void
srpc_lnet_ev_handler(lnet_event_t *ev)
{
	struct srpc_service_cd *scd;
	struct srpc_event *rpcev = ev->md.user_ptr;
	struct srpc_client_rpc *crpc;
	struct srpc_server_rpc *srpc;
	struct srpc_buffer *buffer;
	struct srpc_service *sv;
	struct srpc_msg *msg;
	enum srpc_msg_type type;

	LASSERT(!in_interrupt());

	if (ev->status) {
		__u32 errors;

		spin_lock(&srpc_data.rpc_glock);
		if (ev->status != -ECANCELED) /* cancellation is not error */
			srpc_data.rpc_counters.errors++;
		errors = srpc_data.rpc_counters.errors;
		spin_unlock(&srpc_data.rpc_glock);

		CNETERR("LNet event status %d type %d, RPC errors %u\n",
			ev->status, ev->type, errors);
>>>>>>> v4.9.227
	}

	rpcev->ev_lnet = ev->type;

	switch (rpcev->ev_type) {
	default:
		CERROR("Unknown event: status %d, type %d, lnet %d\n",
		       rpcev->ev_status, rpcev->ev_type, rpcev->ev_lnet);
<<<<<<< HEAD
		LBUG ();
	case SRPC_REQUEST_SENT:
		if (ev->status == 0 && ev->type != LNET_EVENT_UNLINK) {
=======
		LBUG();
	case SRPC_REQUEST_SENT:
		if (!ev->status && ev->type != LNET_EVENT_UNLINK) {
>>>>>>> v4.9.227
			spin_lock(&srpc_data.rpc_glock);
			srpc_data.rpc_counters.rpcs_sent++;
			spin_unlock(&srpc_data.rpc_glock);
		}
	case SRPC_REPLY_RCVD:
	case SRPC_BULK_REQ_RCVD:
		crpc = rpcev->ev_data;

		if (rpcev != &crpc->crpc_reqstev &&
		    rpcev != &crpc->crpc_replyev &&
		    rpcev != &crpc->crpc_bulkev) {
			CERROR("rpcev %p, crpc %p, reqstev %p, replyev %p, bulkev %p\n",
			       rpcev, crpc, &crpc->crpc_reqstev,
			       &crpc->crpc_replyev, &crpc->crpc_bulkev);
			CERROR("Bad event: status %d, type %d, lnet %d\n",
			       rpcev->ev_status, rpcev->ev_type, rpcev->ev_lnet);
<<<<<<< HEAD
			LBUG ();
=======
			LBUG();
>>>>>>> v4.9.227
		}

		spin_lock(&crpc->crpc_lock);

<<<<<<< HEAD
		LASSERT(rpcev->ev_fired == 0);
		rpcev->ev_fired  = 1;
=======
		LASSERT(!rpcev->ev_fired);
		rpcev->ev_fired = 1;
>>>>>>> v4.9.227
		rpcev->ev_status = (ev->type == LNET_EVENT_UNLINK) ?
						-EINTR : ev->status;
		swi_schedule_workitem(&crpc->crpc_wi);

		spin_unlock(&crpc->crpc_lock);
		break;

	case SRPC_REQUEST_RCVD:
		scd = rpcev->ev_data;
		sv = scd->scd_svc;

		LASSERT(rpcev == &scd->scd_ev);

		spin_lock(&scd->scd_lock);

<<<<<<< HEAD
		LASSERT (ev->unlinked);
		LASSERT (ev->type == LNET_EVENT_PUT ||
			 ev->type == LNET_EVENT_UNLINK);
		LASSERT (ev->type != LNET_EVENT_UNLINK ||
			 sv->sv_shuttingdown);

		buffer = container_of(ev->md.start, srpc_buffer_t, buf_msg);
=======
		LASSERT(ev->unlinked);
		LASSERT(ev->type == LNET_EVENT_PUT ||
			ev->type == LNET_EVENT_UNLINK);
		LASSERT(ev->type != LNET_EVENT_UNLINK ||
			sv->sv_shuttingdown);

		buffer = container_of(ev->md.start, struct srpc_buffer, buf_msg);
>>>>>>> v4.9.227
		buffer->buf_peer = ev->initiator;
		buffer->buf_self = ev->target.nid;

		LASSERT(scd->scd_buf_nposted > 0);
		scd->scd_buf_nposted--;

		if (sv->sv_shuttingdown) {
<<<<<<< HEAD
			/* Leave buffer on scd->scd_buf_nposted since
			 * srpc_finish_service needs to traverse it. */
=======
			/*
			 * Leave buffer on scd->scd_buf_nposted since
			 * srpc_finish_service needs to traverse it.
			 */
>>>>>>> v4.9.227
			spin_unlock(&scd->scd_lock);
			break;
		}

<<<<<<< HEAD
		if (scd->scd_buf_err_stamp != 0 &&
		    scd->scd_buf_err_stamp < get_seconds()) {
=======
		if (scd->scd_buf_err_stamp &&
		    scd->scd_buf_err_stamp < ktime_get_real_seconds()) {
>>>>>>> v4.9.227
			/* re-enable adding buffer */
			scd->scd_buf_err_stamp = 0;
			scd->scd_buf_err = 0;
		}

<<<<<<< HEAD
		if (scd->scd_buf_err == 0 && /* adding buffer is enabled */
		    scd->scd_buf_adjust == 0 &&
		    scd->scd_buf_nposted < scd->scd_buf_low) {
			scd->scd_buf_adjust = MAX(scd->scd_buf_total / 2,
=======
		if (!scd->scd_buf_err &&	/* adding buffer is enabled */
		    !scd->scd_buf_adjust &&
		    scd->scd_buf_nposted < scd->scd_buf_low) {
			scd->scd_buf_adjust = max(scd->scd_buf_total / 2,
>>>>>>> v4.9.227
						  SFW_TEST_WI_MIN);
			swi_schedule_workitem(&scd->scd_buf_wi);
		}

		list_del(&buffer->buf_list); /* from scd->scd_buf_posted */
		msg = &buffer->buf_msg;
		type = srpc_service2request(sv->sv_id);

<<<<<<< HEAD
		if (ev->status != 0 || ev->mlength != sizeof(*msg) ||
=======
		if (ev->status || ev->mlength != sizeof(*msg) ||
>>>>>>> v4.9.227
		    (msg->msg_type != type &&
		     msg->msg_type != __swab32(type)) ||
		    (msg->msg_magic != SRPC_MSG_MAGIC &&
		     msg->msg_magic != __swab32(SRPC_MSG_MAGIC))) {
<<<<<<< HEAD
			CERROR ("Dropping RPC (%s) from %s: "
				"status %d mlength %d type %u magic %u.\n",
				sv->sv_name, libcfs_id2str(ev->initiator),
				ev->status, ev->mlength,
				msg->msg_type, msg->msg_magic);

			/* NB can't call srpc_service_recycle_buffer here since
			 * it may call LNetM[DE]Attach. The invalid magic tells
			 * srpc_handle_rpc to drop this RPC */
=======
			CERROR("Dropping RPC (%s) from %s: status %d mlength %d type %u magic %u.\n",
			       sv->sv_name, libcfs_id2str(ev->initiator),
			       ev->status, ev->mlength,
			       msg->msg_type, msg->msg_magic);

			/*
			 * NB can't call srpc_service_recycle_buffer here since
			 * it may call LNetM[DE]Attach. The invalid magic tells
			 * srpc_handle_rpc to drop this RPC
			 */
>>>>>>> v4.9.227
			msg->msg_magic = 0;
		}

		if (!list_empty(&scd->scd_rpc_free)) {
			srpc = list_entry(scd->scd_rpc_free.next,
<<<<<<< HEAD
					      struct srpc_server_rpc,
					      srpc_list);
=======
					  struct srpc_server_rpc,
					  srpc_list);
>>>>>>> v4.9.227
			list_del(&srpc->srpc_list);

			srpc_init_server_rpc(srpc, scd, buffer);
			list_add_tail(&srpc->srpc_list,
<<<<<<< HEAD
					  &scd->scd_rpc_active);
			swi_schedule_workitem(&srpc->srpc_wi);
		} else {
			list_add_tail(&buffer->buf_list,
					  &scd->scd_buf_blocked);
=======
				      &scd->scd_rpc_active);
			swi_schedule_workitem(&srpc->srpc_wi);
		} else {
			list_add_tail(&buffer->buf_list,
				      &scd->scd_buf_blocked);
>>>>>>> v4.9.227
		}

		spin_unlock(&scd->scd_lock);

		spin_lock(&srpc_data.rpc_glock);
		srpc_data.rpc_counters.rpcs_rcvd++;
		spin_unlock(&srpc_data.rpc_glock);
		break;

	case SRPC_BULK_GET_RPLD:
<<<<<<< HEAD
		LASSERT (ev->type == LNET_EVENT_SEND ||
			 ev->type == LNET_EVENT_REPLY ||
			 ev->type == LNET_EVENT_UNLINK);
=======
		LASSERT(ev->type == LNET_EVENT_SEND ||
			ev->type == LNET_EVENT_REPLY ||
			ev->type == LNET_EVENT_UNLINK);
>>>>>>> v4.9.227

		if (!ev->unlinked)
			break; /* wait for final event */

	case SRPC_BULK_PUT_SENT:
<<<<<<< HEAD
		if (ev->status == 0 && ev->type != LNET_EVENT_UNLINK) {
=======
		if (!ev->status && ev->type != LNET_EVENT_UNLINK) {
>>>>>>> v4.9.227
			spin_lock(&srpc_data.rpc_glock);

			if (rpcev->ev_type == SRPC_BULK_GET_RPLD)
				srpc_data.rpc_counters.bulk_get += ev->mlength;
			else
				srpc_data.rpc_counters.bulk_put += ev->mlength;

			spin_unlock(&srpc_data.rpc_glock);
		}
	case SRPC_REPLY_SENT:
		srpc = rpcev->ev_data;
<<<<<<< HEAD
		scd  = srpc->srpc_scd;
=======
		scd = srpc->srpc_scd;
>>>>>>> v4.9.227

		LASSERT(rpcev == &srpc->srpc_ev);

		spin_lock(&scd->scd_lock);

<<<<<<< HEAD
		rpcev->ev_fired  = 1;
=======
		rpcev->ev_fired = 1;
>>>>>>> v4.9.227
		rpcev->ev_status = (ev->type == LNET_EVENT_UNLINK) ?
				   -EINTR : ev->status;
		swi_schedule_workitem(&srpc->srpc_wi);

		spin_unlock(&scd->scd_lock);
		break;
	}
}

<<<<<<< HEAD

int
srpc_startup (void)
=======
int
srpc_startup(void)
>>>>>>> v4.9.227
{
	int rc;

	memset(&srpc_data, 0, sizeof(struct smoketest_rpc));
	spin_lock_init(&srpc_data.rpc_glock);

	/* 1 second pause to avoid timestamp reuse */
	set_current_state(TASK_UNINTERRUPTIBLE);
	schedule_timeout(cfs_time_seconds(1));
<<<<<<< HEAD
	srpc_data.rpc_matchbits = ((__u64) get_seconds()) << 48;

	srpc_data.rpc_state = SRPC_STATE_NONE;

	rc = LNetNIInit(LUSTRE_SRV_LNET_PID);
	if (rc < 0) {
		CERROR ("LNetNIInit() has failed: %d\n", rc);
=======
	srpc_data.rpc_matchbits = ((__u64)ktime_get_real_seconds()) << 48;

	srpc_data.rpc_state = SRPC_STATE_NONE;

	rc = LNetNIInit(LNET_PID_LUSTRE);
	if (rc < 0) {
		CERROR("LNetNIInit() has failed: %d\n", rc);
>>>>>>> v4.9.227
		return rc;
	}

	srpc_data.rpc_state = SRPC_STATE_NI_INIT;

	LNetInvalidateHandle(&srpc_data.rpc_lnet_eq);
	rc = LNetEQAlloc(0, srpc_lnet_ev_handler, &srpc_data.rpc_lnet_eq);
<<<<<<< HEAD
	if (rc != 0) {
=======
	if (rc) {
>>>>>>> v4.9.227
		CERROR("LNetEQAlloc() has failed: %d\n", rc);
		goto bail;
	}

	rc = LNetSetLazyPortal(SRPC_FRAMEWORK_REQUEST_PORTAL);
<<<<<<< HEAD
	LASSERT(rc == 0);
	rc = LNetSetLazyPortal(SRPC_REQUEST_PORTAL);
	LASSERT(rc == 0);
=======
	LASSERT(!rc);
	rc = LNetSetLazyPortal(SRPC_REQUEST_PORTAL);
	LASSERT(!rc);
>>>>>>> v4.9.227

	srpc_data.rpc_state = SRPC_STATE_EQ_INIT;

	rc = stt_startup();

bail:
<<<<<<< HEAD
	if (rc != 0)
=======
	if (rc)
>>>>>>> v4.9.227
		srpc_shutdown();
	else
		srpc_data.rpc_state = SRPC_STATE_RUNNING;

	return rc;
}

void
<<<<<<< HEAD
srpc_shutdown (void)
=======
srpc_shutdown(void)
>>>>>>> v4.9.227
{
	int i;
	int rc;
	int state;

	state = srpc_data.rpc_state;
	srpc_data.rpc_state = SRPC_STATE_STOPPING;

	switch (state) {
	default:
<<<<<<< HEAD
		LBUG ();
=======
		LBUG();
>>>>>>> v4.9.227
	case SRPC_STATE_RUNNING:
		spin_lock(&srpc_data.rpc_glock);

		for (i = 0; i <= SRPC_SERVICE_MAX_ID; i++) {
<<<<<<< HEAD
			srpc_service_t *sv = srpc_data.rpc_services[i];

			LASSERTF (sv == NULL,
				  "service not empty: id %d, name %s\n",
				  i, sv->sv_name);
=======
			struct srpc_service *sv = srpc_data.rpc_services[i];

			LASSERTF(!sv, "service not empty: id %d, name %s\n",
				 i, sv->sv_name);
>>>>>>> v4.9.227
		}

		spin_unlock(&srpc_data.rpc_glock);

		stt_shutdown();

	case SRPC_STATE_EQ_INIT:
		rc = LNetClearLazyPortal(SRPC_FRAMEWORK_REQUEST_PORTAL);
		rc = LNetClearLazyPortal(SRPC_REQUEST_PORTAL);
<<<<<<< HEAD
		LASSERT (rc == 0);
		rc = LNetEQFree(srpc_data.rpc_lnet_eq);
		LASSERT (rc == 0); /* the EQ should have no user by now */
=======
		LASSERT(!rc);
		rc = LNetEQFree(srpc_data.rpc_lnet_eq);
		LASSERT(!rc); /* the EQ should have no user by now */
>>>>>>> v4.9.227

	case SRPC_STATE_NI_INIT:
		LNetNIFini();
	}
<<<<<<< HEAD

	return;
=======
>>>>>>> v4.9.227
}
