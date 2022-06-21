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
 * Copyright (c) 2011, 2012, Intel Corporation.
=======
 * Copyright (c) 2011, 2015, Intel Corporation.
>>>>>>> v4.9.227
 */
/*
 * This file is part of Lustre, http://www.lustre.org/
 * Lustre is a trademark of Sun Microsystems, Inc.
 *
 * lnet/klnds/o2iblnd/o2iblnd.c
 *
 * Author: Eric Barton <eric@bartonsoftware.com>
 */

<<<<<<< HEAD
#include "o2iblnd.h"
#include <asm/div64.h>

static lnd_t the_o2iblnd = {
	.lnd_type       = O2IBLND,
	.lnd_startup    = kiblnd_startup,
	.lnd_shutdown   = kiblnd_shutdown,
	.lnd_ctl	= kiblnd_ctl,
	.lnd_query      = kiblnd_query,
	.lnd_send       = kiblnd_send,
	.lnd_recv       = kiblnd_recv,
};

kib_data_t	      kiblnd_data;

static __u32
kiblnd_cksum(void *ptr, int nob)
{
	char  *c  = ptr;
	__u32  sum = 0;
=======
#include <asm/div64.h>
#include <asm/page.h>
#include "o2iblnd.h"

static lnd_t the_o2iblnd;

struct kib_data kiblnd_data;

static __u32 kiblnd_cksum(void *ptr, int nob)
{
	char *c = ptr;
	__u32 sum = 0;
>>>>>>> v4.9.227

	while (nob-- > 0)
		sum = ((sum << 1) | (sum >> 31)) + *c++;

	/* ensure I don't return 0 (== no checksum) */
<<<<<<< HEAD
	return (sum == 0) ? 1 : sum;
}

static char *
kiblnd_msgtype2str(int type)
=======
	return !sum ? 1 : sum;
}

static char *kiblnd_msgtype2str(int type)
>>>>>>> v4.9.227
{
	switch (type) {
	case IBLND_MSG_CONNREQ:
		return "CONNREQ";

	case IBLND_MSG_CONNACK:
		return "CONNACK";

	case IBLND_MSG_NOOP:
		return "NOOP";

	case IBLND_MSG_IMMEDIATE:
		return "IMMEDIATE";

	case IBLND_MSG_PUT_REQ:
		return "PUT_REQ";

	case IBLND_MSG_PUT_NAK:
		return "PUT_NAK";

	case IBLND_MSG_PUT_ACK:
		return "PUT_ACK";

	case IBLND_MSG_PUT_DONE:
		return "PUT_DONE";

	case IBLND_MSG_GET_REQ:
		return "GET_REQ";

	case IBLND_MSG_GET_DONE:
		return "GET_DONE";

	default:
		return "???";
	}
}

<<<<<<< HEAD
static int
kiblnd_msgtype2size(int type)
{
	const int hdr_size = offsetof(kib_msg_t, ibm_u);
=======
static int kiblnd_msgtype2size(int type)
{
	const int hdr_size = offsetof(struct kib_msg, ibm_u);
>>>>>>> v4.9.227

	switch (type) {
	case IBLND_MSG_CONNREQ:
	case IBLND_MSG_CONNACK:
<<<<<<< HEAD
		return hdr_size + sizeof(kib_connparams_t);
=======
		return hdr_size + sizeof(struct kib_connparams);
>>>>>>> v4.9.227

	case IBLND_MSG_NOOP:
		return hdr_size;

	case IBLND_MSG_IMMEDIATE:
<<<<<<< HEAD
		return offsetof(kib_msg_t, ibm_u.immediate.ibim_payload[0]);

	case IBLND_MSG_PUT_REQ:
		return hdr_size + sizeof(kib_putreq_msg_t);

	case IBLND_MSG_PUT_ACK:
		return hdr_size + sizeof(kib_putack_msg_t);

	case IBLND_MSG_GET_REQ:
		return hdr_size + sizeof(kib_get_msg_t);
=======
		return offsetof(struct kib_msg, ibm_u.immediate.ibim_payload[0]);

	case IBLND_MSG_PUT_REQ:
		return hdr_size + sizeof(struct kib_putreq_msg);

	case IBLND_MSG_PUT_ACK:
		return hdr_size + sizeof(struct kib_putack_msg);

	case IBLND_MSG_GET_REQ:
		return hdr_size + sizeof(struct kib_get_msg);
>>>>>>> v4.9.227

	case IBLND_MSG_PUT_NAK:
	case IBLND_MSG_PUT_DONE:
	case IBLND_MSG_GET_DONE:
<<<<<<< HEAD
		return hdr_size + sizeof(kib_completion_msg_t);
=======
		return hdr_size + sizeof(struct kib_completion_msg);
>>>>>>> v4.9.227
	default:
		return -1;
	}
}

<<<<<<< HEAD
static int
kiblnd_unpack_rd(kib_msg_t *msg, int flip)
{
	kib_rdma_desc_t   *rd;
	int		nob;
	int		n;
	int		i;

	LASSERT (msg->ibm_type == IBLND_MSG_GET_REQ ||
		 msg->ibm_type == IBLND_MSG_PUT_ACK);
=======
static int kiblnd_unpack_rd(struct kib_msg *msg, int flip)
{
	struct kib_rdma_desc *rd;
	int msg_size;
	int nob;
	int n;
	int i;

	LASSERT(msg->ibm_type == IBLND_MSG_GET_REQ ||
		msg->ibm_type == IBLND_MSG_PUT_ACK);
>>>>>>> v4.9.227

	rd = msg->ibm_type == IBLND_MSG_GET_REQ ?
			      &msg->ibm_u.get.ibgm_rd :
			      &msg->ibm_u.putack.ibpam_rd;

	if (flip) {
		__swab32s(&rd->rd_key);
		__swab32s(&rd->rd_nfrags);
	}

	n = rd->rd_nfrags;

<<<<<<< HEAD
	if (n <= 0 || n > IBLND_MAX_RDMA_FRAGS) {
		CERROR("Bad nfrags: %d, should be 0 < n <= %d\n",
		       n, IBLND_MAX_RDMA_FRAGS);
		return 1;
	}

	nob = offsetof (kib_msg_t, ibm_u) +
=======
	nob = offsetof(struct kib_msg, ibm_u) +
>>>>>>> v4.9.227
	      kiblnd_rd_msg_size(rd, msg->ibm_type, n);

	if (msg->ibm_nob < nob) {
		CERROR("Short %s: %d(%d)\n",
		       kiblnd_msgtype2str(msg->ibm_type), msg->ibm_nob, nob);
		return 1;
	}

<<<<<<< HEAD
=======
	msg_size = kiblnd_rd_size(rd);
	if (msg_size <= 0 || msg_size > LNET_MAX_PAYLOAD) {
		CERROR("Bad msg_size: %d, should be 0 < n <= %d\n",
		       msg_size, LNET_MAX_PAYLOAD);
		return 1;
	}

>>>>>>> v4.9.227
	if (!flip)
		return 0;

	for (i = 0; i < n; i++) {
		__swab32s(&rd->rd_frags[i].rf_nob);
		__swab64s(&rd->rd_frags[i].rf_addr);
	}

	return 0;
}

<<<<<<< HEAD
void
kiblnd_pack_msg (lnet_ni_t *ni, kib_msg_t *msg, int version,
		 int credits, lnet_nid_t dstnid, __u64 dststamp)
{
	kib_net_t *net = ni->ni_data;

	/* CAVEAT EMPTOR! all message fields not set here should have been
	 * initialised previously. */
=======
void kiblnd_pack_msg(lnet_ni_t *ni, struct kib_msg *msg, int version,
		     int credits, lnet_nid_t dstnid, __u64 dststamp)
{
	struct kib_net *net = ni->ni_data;

	/*
	 * CAVEAT EMPTOR! all message fields not set here should have been
	 * initialised previously.
	 */
>>>>>>> v4.9.227
	msg->ibm_magic    = IBLND_MSG_MAGIC;
	msg->ibm_version  = version;
	/*   ibm_type */
	msg->ibm_credits  = credits;
	/*   ibm_nob */
	msg->ibm_cksum    = 0;
	msg->ibm_srcnid   = ni->ni_nid;
	msg->ibm_srcstamp = net->ibn_incarnation;
	msg->ibm_dstnid   = dstnid;
	msg->ibm_dststamp = dststamp;

	if (*kiblnd_tunables.kib_cksum) {
		/* NB ibm_cksum zero while computing cksum */
		msg->ibm_cksum = kiblnd_cksum(msg, msg->ibm_nob);
	}
}

<<<<<<< HEAD
int
kiblnd_unpack_msg(kib_msg_t *msg, int nob)
{
	const int hdr_size = offsetof(kib_msg_t, ibm_u);
	__u32     msg_cksum;
	__u16     version;
	int       msg_nob;
	int       flip;
=======
int kiblnd_unpack_msg(struct kib_msg *msg, int nob)
{
	const int hdr_size = offsetof(struct kib_msg, ibm_u);
	__u32 msg_cksum;
	__u16 version;
	int msg_nob;
	int flip;
>>>>>>> v4.9.227

	/* 6 bytes are enough to have received magic + version */
	if (nob < 6) {
		CERROR("Short message: %d\n", nob);
		return -EPROTO;
	}

	if (msg->ibm_magic == IBLND_MSG_MAGIC) {
		flip = 0;
	} else if (msg->ibm_magic == __swab32(IBLND_MSG_MAGIC)) {
		flip = 1;
	} else {
		CERROR("Bad magic: %08x\n", msg->ibm_magic);
		return -EPROTO;
	}

	version = flip ? __swab16(msg->ibm_version) : msg->ibm_version;
	if (version != IBLND_MSG_VERSION &&
	    version != IBLND_MSG_VERSION_1) {
		CERROR("Bad version: %x\n", version);
		return -EPROTO;
	}

	if (nob < hdr_size) {
		CERROR("Short message: %d\n", nob);
		return -EPROTO;
	}

	msg_nob = flip ? __swab32(msg->ibm_nob) : msg->ibm_nob;
	if (msg_nob > nob) {
		CERROR("Short message: got %d, wanted %d\n", nob, msg_nob);
		return -EPROTO;
	}

<<<<<<< HEAD
	/* checksum must be computed with ibm_cksum zero and BEFORE anything
	 * gets flipped */
	msg_cksum = flip ? __swab32(msg->ibm_cksum) : msg->ibm_cksum;
	msg->ibm_cksum = 0;
	if (msg_cksum != 0 &&
=======
	/*
	 * checksum must be computed with ibm_cksum zero and BEFORE anything
	 * gets flipped
	 */
	msg_cksum = flip ? __swab32(msg->ibm_cksum) : msg->ibm_cksum;
	msg->ibm_cksum = 0;
	if (msg_cksum &&
>>>>>>> v4.9.227
	    msg_cksum != kiblnd_cksum(msg, msg_nob)) {
		CERROR("Bad checksum\n");
		return -EPROTO;
	}

	msg->ibm_cksum = msg_cksum;

	if (flip) {
		/* leave magic unflipped as a clue to peer endianness */
		msg->ibm_version = version;
<<<<<<< HEAD
		CLASSERT (sizeof(msg->ibm_type) == 1);
		CLASSERT (sizeof(msg->ibm_credits) == 1);
=======
		CLASSERT(sizeof(msg->ibm_type) == 1);
		CLASSERT(sizeof(msg->ibm_credits) == 1);
>>>>>>> v4.9.227
		msg->ibm_nob     = msg_nob;
		__swab64s(&msg->ibm_srcnid);
		__swab64s(&msg->ibm_srcstamp);
		__swab64s(&msg->ibm_dstnid);
		__swab64s(&msg->ibm_dststamp);
	}

	if (msg->ibm_srcnid == LNET_NID_ANY) {
		CERROR("Bad src nid: %s\n", libcfs_nid2str(msg->ibm_srcnid));
		return -EPROTO;
	}

	if (msg_nob < kiblnd_msgtype2size(msg->ibm_type)) {
		CERROR("Short %s: %d(%d)\n", kiblnd_msgtype2str(msg->ibm_type),
		       msg_nob, kiblnd_msgtype2size(msg->ibm_type));
		return -EPROTO;
	}

	switch (msg->ibm_type) {
	default:
		CERROR("Unknown message type %x\n", msg->ibm_type);
		return -EPROTO;

	case IBLND_MSG_NOOP:
	case IBLND_MSG_IMMEDIATE:
	case IBLND_MSG_PUT_REQ:
		break;

	case IBLND_MSG_PUT_ACK:
	case IBLND_MSG_GET_REQ:
		if (kiblnd_unpack_rd(msg, flip))
			return -EPROTO;
		break;

	case IBLND_MSG_PUT_NAK:
	case IBLND_MSG_PUT_DONE:
	case IBLND_MSG_GET_DONE:
		if (flip)
			__swab32s(&msg->ibm_u.completion.ibcm_status);
		break;

	case IBLND_MSG_CONNREQ:
	case IBLND_MSG_CONNACK:
		if (flip) {
			__swab16s(&msg->ibm_u.connparams.ibcp_queue_depth);
			__swab16s(&msg->ibm_u.connparams.ibcp_max_frags);
			__swab32s(&msg->ibm_u.connparams.ibcp_max_msg_size);
		}
		break;
	}
	return 0;
}

<<<<<<< HEAD
int
kiblnd_create_peer(lnet_ni_t *ni, kib_peer_t **peerp, lnet_nid_t nid)
{
	kib_peer_t	*peer;
	kib_net_t	*net = ni->ni_data;
	int		cpt = lnet_cpt_of_nid(nid);
	unsigned long   flags;

	LASSERT(net != NULL);
	LASSERT(nid != LNET_NID_ANY);

	LIBCFS_CPT_ALLOC(peer, lnet_cpt_table(), cpt, sizeof(*peer));
	if (peer == NULL) {
=======
int kiblnd_create_peer(lnet_ni_t *ni, struct kib_peer **peerp, lnet_nid_t nid)
{
	struct kib_peer *peer;
	struct kib_net *net = ni->ni_data;
	int cpt = lnet_cpt_of_nid(nid);
	unsigned long flags;

	LASSERT(net);
	LASSERT(nid != LNET_NID_ANY);

	LIBCFS_CPT_ALLOC(peer, lnet_cpt_table(), cpt, sizeof(*peer));
	if (!peer) {
>>>>>>> v4.9.227
		CERROR("Cannot allocate peer\n");
		return -ENOMEM;
	}

<<<<<<< HEAD
	memset(peer, 0, sizeof(*peer));	 /* zero flags etc */

=======
>>>>>>> v4.9.227
	peer->ibp_ni = ni;
	peer->ibp_nid = nid;
	peer->ibp_error = 0;
	peer->ibp_last_alive = 0;
<<<<<<< HEAD
=======
	peer->ibp_max_frags = kiblnd_cfg_rdma_frags(peer->ibp_ni);
	peer->ibp_queue_depth = ni->ni_peertxcredits;
>>>>>>> v4.9.227
	atomic_set(&peer->ibp_refcount, 1);  /* 1 ref for caller */

	INIT_LIST_HEAD(&peer->ibp_list);     /* not in the peer table yet */
	INIT_LIST_HEAD(&peer->ibp_conns);
	INIT_LIST_HEAD(&peer->ibp_tx_queue);

	write_lock_irqsave(&kiblnd_data.kib_global_lock, flags);

	/* always called with a ref on ni, which prevents ni being shutdown */
<<<<<<< HEAD
	LASSERT (net->ibn_shutdown == 0);
=======
	LASSERT(!net->ibn_shutdown);
>>>>>>> v4.9.227

	/* npeers only grows with the global lock held */
	atomic_inc(&net->ibn_npeers);

	write_unlock_irqrestore(&kiblnd_data.kib_global_lock, flags);

	*peerp = peer;
	return 0;
}

<<<<<<< HEAD
void
kiblnd_destroy_peer (kib_peer_t *peer)
{
	kib_net_t *net = peer->ibp_ni->ni_data;

	LASSERT (net != NULL);
	LASSERT (atomic_read(&peer->ibp_refcount) == 0);
	LASSERT (!kiblnd_peer_active(peer));
	LASSERT (peer->ibp_connecting == 0);
	LASSERT (peer->ibp_accepting == 0);
	LASSERT (list_empty(&peer->ibp_conns));
	LASSERT (list_empty(&peer->ibp_tx_queue));

	LIBCFS_FREE(peer, sizeof(*peer));

	/* NB a peer's connections keep a reference on their peer until
	 * they are destroyed, so we can be assured that _all_ state to do
	 * with this peer has been cleaned up when its refcount drops to
	 * zero. */
	atomic_dec(&net->ibn_npeers);
}

kib_peer_t *
kiblnd_find_peer_locked (lnet_nid_t nid)
{
	/* the caller is responsible for accounting the additional reference
	 * that this creates */
	struct list_head       *peer_list = kiblnd_nid2peerlist(nid);
	struct list_head       *tmp;
	kib_peer_t       *peer;

	list_for_each (tmp, peer_list) {

		peer = list_entry(tmp, kib_peer_t, ibp_list);

		LASSERT (peer->ibp_connecting > 0 || /* creating conns */
			 peer->ibp_accepting > 0 ||
			 !list_empty(&peer->ibp_conns));  /* active conn */
=======
void kiblnd_destroy_peer(struct kib_peer *peer)
{
	struct kib_net *net = peer->ibp_ni->ni_data;

	LASSERT(net);
	LASSERT(!atomic_read(&peer->ibp_refcount));
	LASSERT(!kiblnd_peer_active(peer));
	LASSERT(kiblnd_peer_idle(peer));
	LASSERT(list_empty(&peer->ibp_tx_queue));

	LIBCFS_FREE(peer, sizeof(*peer));

	/*
	 * NB a peer's connections keep a reference on their peer until
	 * they are destroyed, so we can be assured that _all_ state to do
	 * with this peer has been cleaned up when its refcount drops to
	 * zero.
	 */
	atomic_dec(&net->ibn_npeers);
}

struct kib_peer *kiblnd_find_peer_locked(lnet_nid_t nid)
{
	/*
	 * the caller is responsible for accounting the additional reference
	 * that this creates
	 */
	struct list_head *peer_list = kiblnd_nid2peerlist(nid);
	struct list_head *tmp;
	struct kib_peer *peer;

	list_for_each(tmp, peer_list) {
		peer = list_entry(tmp, struct kib_peer, ibp_list);
		LASSERT(!kiblnd_peer_idle(peer));
>>>>>>> v4.9.227

		if (peer->ibp_nid != nid)
			continue;

		CDEBUG(D_NET, "got peer [%p] -> %s (%d) version: %x\n",
		       peer, libcfs_nid2str(nid),
		       atomic_read(&peer->ibp_refcount),
		       peer->ibp_version);
		return peer;
	}
	return NULL;
}

<<<<<<< HEAD
void
kiblnd_unlink_peer_locked (kib_peer_t *peer)
{
	LASSERT (list_empty(&peer->ibp_conns));

	LASSERT (kiblnd_peer_active(peer));
=======
void kiblnd_unlink_peer_locked(struct kib_peer *peer)
{
	LASSERT(list_empty(&peer->ibp_conns));

	LASSERT(kiblnd_peer_active(peer));
>>>>>>> v4.9.227
	list_del_init(&peer->ibp_list);
	/* lose peerlist's ref */
	kiblnd_peer_decref(peer);
}

<<<<<<< HEAD
static int
kiblnd_get_peer_info(lnet_ni_t *ni, int index,
		      lnet_nid_t *nidp, int *count)
{
	kib_peer_t	    *peer;
	struct list_head	    *ptmp;
	int		    i;
	unsigned long	  flags;
=======
static int kiblnd_get_peer_info(lnet_ni_t *ni, int index,
				lnet_nid_t *nidp, int *count)
{
	struct kib_peer *peer;
	struct list_head *ptmp;
	int i;
	unsigned long flags;
>>>>>>> v4.9.227

	read_lock_irqsave(&kiblnd_data.kib_global_lock, flags);

	for (i = 0; i < kiblnd_data.kib_peer_hash_size; i++) {
<<<<<<< HEAD

		list_for_each (ptmp, &kiblnd_data.kib_peers[i]) {

			peer = list_entry(ptmp, kib_peer_t, ibp_list);
			LASSERT (peer->ibp_connecting > 0 ||
				 peer->ibp_accepting > 0 ||
				 !list_empty(&peer->ibp_conns));
=======
		list_for_each(ptmp, &kiblnd_data.kib_peers[i]) {
			peer = list_entry(ptmp, struct kib_peer, ibp_list);
			LASSERT(!kiblnd_peer_idle(peer));
>>>>>>> v4.9.227

			if (peer->ibp_ni != ni)
				continue;

			if (index-- > 0)
				continue;

			*nidp = peer->ibp_nid;
			*count = atomic_read(&peer->ibp_refcount);

			read_unlock_irqrestore(&kiblnd_data.kib_global_lock,
					       flags);
			return 0;
		}
	}

	read_unlock_irqrestore(&kiblnd_data.kib_global_lock, flags);
	return -ENOENT;
}

<<<<<<< HEAD
static void
kiblnd_del_peer_locked(kib_peer_t *peer)
{
	struct list_head	   *ctmp;
	struct list_head	   *cnxt;
	kib_conn_t	   *conn;
=======
static void kiblnd_del_peer_locked(struct kib_peer *peer)
{
	struct list_head *ctmp;
	struct list_head *cnxt;
	struct kib_conn *conn;
>>>>>>> v4.9.227

	if (list_empty(&peer->ibp_conns)) {
		kiblnd_unlink_peer_locked(peer);
	} else {
<<<<<<< HEAD
		list_for_each_safe (ctmp, cnxt, &peer->ibp_conns) {
			conn = list_entry(ctmp, kib_conn_t, ibc_list);
=======
		list_for_each_safe(ctmp, cnxt, &peer->ibp_conns) {
			conn = list_entry(ctmp, struct kib_conn, ibc_list);
>>>>>>> v4.9.227

			kiblnd_close_conn_locked(conn, 0);
		}
		/* NB closing peer's last conn unlinked it. */
	}
<<<<<<< HEAD
	/* NB peer now unlinked; might even be freed if the peer table had the
	 * last ref on it. */
}

static int
kiblnd_del_peer(lnet_ni_t *ni, lnet_nid_t nid)
{
	LIST_HEAD	 (zombies);
	struct list_head	    *ptmp;
	struct list_head	    *pnxt;
	kib_peer_t	    *peer;
	int		    lo;
	int		    hi;
	int		    i;
	unsigned long	  flags;
	int		    rc = -ENOENT;
=======
	/*
	 * NB peer now unlinked; might even be freed if the peer table had the
	 * last ref on it.
	 */
}

static int kiblnd_del_peer(lnet_ni_t *ni, lnet_nid_t nid)
{
	LIST_HEAD(zombies);
	struct list_head *ptmp;
	struct list_head *pnxt;
	struct kib_peer *peer;
	int lo;
	int hi;
	int i;
	unsigned long flags;
	int rc = -ENOENT;
>>>>>>> v4.9.227

	write_lock_irqsave(&kiblnd_data.kib_global_lock, flags);

	if (nid != LNET_NID_ANY) {
<<<<<<< HEAD
		lo = hi = kiblnd_nid2peerlist(nid) - kiblnd_data.kib_peers;
=======
		lo = kiblnd_nid2peerlist(nid) - kiblnd_data.kib_peers;
		hi = kiblnd_nid2peerlist(nid) - kiblnd_data.kib_peers;
>>>>>>> v4.9.227
	} else {
		lo = 0;
		hi = kiblnd_data.kib_peer_hash_size - 1;
	}

	for (i = lo; i <= hi; i++) {
<<<<<<< HEAD
		list_for_each_safe (ptmp, pnxt, &kiblnd_data.kib_peers[i]) {
			peer = list_entry(ptmp, kib_peer_t, ibp_list);
			LASSERT (peer->ibp_connecting > 0 ||
				 peer->ibp_accepting > 0 ||
				 !list_empty(&peer->ibp_conns));
=======
		list_for_each_safe(ptmp, pnxt, &kiblnd_data.kib_peers[i]) {
			peer = list_entry(ptmp, struct kib_peer, ibp_list);
			LASSERT(!kiblnd_peer_idle(peer));
>>>>>>> v4.9.227

			if (peer->ibp_ni != ni)
				continue;

			if (!(nid == LNET_NID_ANY || peer->ibp_nid == nid))
				continue;

			if (!list_empty(&peer->ibp_tx_queue)) {
<<<<<<< HEAD
				LASSERT (list_empty(&peer->ibp_conns));

				list_splice_init(&peer->ibp_tx_queue,
						     &zombies);
=======
				LASSERT(list_empty(&peer->ibp_conns));

				list_splice_init(&peer->ibp_tx_queue,
						 &zombies);
>>>>>>> v4.9.227
			}

			kiblnd_del_peer_locked(peer);
			rc = 0;	 /* matched something */
		}
	}

	write_unlock_irqrestore(&kiblnd_data.kib_global_lock, flags);

	kiblnd_txlist_done(ni, &zombies, -EIO);

	return rc;
}

<<<<<<< HEAD
static kib_conn_t *
kiblnd_get_conn_by_idx(lnet_ni_t *ni, int index)
{
	kib_peer_t	    *peer;
	struct list_head	    *ptmp;
	kib_conn_t	    *conn;
	struct list_head	    *ctmp;
	int		    i;
	unsigned long	  flags;
=======
static struct kib_conn *kiblnd_get_conn_by_idx(lnet_ni_t *ni, int index)
{
	struct kib_peer *peer;
	struct list_head *ptmp;
	struct kib_conn *conn;
	struct list_head *ctmp;
	int i;
	unsigned long flags;
>>>>>>> v4.9.227

	read_lock_irqsave(&kiblnd_data.kib_global_lock, flags);

	for (i = 0; i < kiblnd_data.kib_peer_hash_size; i++) {
<<<<<<< HEAD
		list_for_each (ptmp, &kiblnd_data.kib_peers[i]) {

			peer = list_entry(ptmp, kib_peer_t, ibp_list);
			LASSERT (peer->ibp_connecting > 0 ||
				 peer->ibp_accepting > 0 ||
				 !list_empty(&peer->ibp_conns));
=======
		list_for_each(ptmp, &kiblnd_data.kib_peers[i]) {
			peer = list_entry(ptmp, struct kib_peer, ibp_list);
			LASSERT(!kiblnd_peer_idle(peer));
>>>>>>> v4.9.227

			if (peer->ibp_ni != ni)
				continue;

<<<<<<< HEAD
			list_for_each (ctmp, &peer->ibp_conns) {
				if (index-- > 0)
					continue;

				conn = list_entry(ctmp, kib_conn_t,
						      ibc_list);
				kiblnd_conn_addref(conn);
				read_unlock_irqrestore(&kiblnd_data.kib_global_lock,
						       flags);
=======
			list_for_each(ctmp, &peer->ibp_conns) {
				if (index-- > 0)
					continue;

				conn = list_entry(ctmp, struct kib_conn,
						  ibc_list);
				kiblnd_conn_addref(conn);
				read_unlock_irqrestore(
					&kiblnd_data.kib_global_lock,
					flags);
>>>>>>> v4.9.227
				return conn;
			}
		}
	}

	read_unlock_irqrestore(&kiblnd_data.kib_global_lock, flags);
	return NULL;
}

<<<<<<< HEAD
int
kiblnd_translate_mtu(int value)
=======
int kiblnd_translate_mtu(int value)
>>>>>>> v4.9.227
{
	switch (value) {
	default:
		return -1;
	case 0:
		return 0;
	case 256:
		return IB_MTU_256;
	case 512:
		return IB_MTU_512;
	case 1024:
		return IB_MTU_1024;
	case 2048:
		return IB_MTU_2048;
	case 4096:
		return IB_MTU_4096;
	}
}

<<<<<<< HEAD
static void
kiblnd_setup_mtu_locked(struct rdma_cm_id *cmid)
{
	int	   mtu;

	/* XXX There is no path record for iWARP, set by netdev->change_mtu? */
	if (cmid->route.path_rec == NULL)
		return;

	mtu = kiblnd_translate_mtu(*kiblnd_tunables.kib_ib_mtu);
	LASSERT (mtu >= 0);
	if (mtu != 0)
		cmid->route.path_rec->mtu = mtu;
}

static int
kiblnd_get_completion_vector(kib_conn_t *conn, int cpt)
{
	cpumask_t	*mask;
	int		vectors;
	int		off;
	int		i;
	lnet_nid_t	nid = conn->ibc_peer->ibp_nid;
=======
static void kiblnd_setup_mtu_locked(struct rdma_cm_id *cmid)
{
	int mtu;

	/* XXX There is no path record for iWARP, set by netdev->change_mtu? */
	if (!cmid->route.path_rec)
		return;

	mtu = kiblnd_translate_mtu(*kiblnd_tunables.kib_ib_mtu);
	LASSERT(mtu >= 0);
	if (mtu)
		cmid->route.path_rec->mtu = mtu;
}

static int kiblnd_get_completion_vector(struct kib_conn *conn, int cpt)
{
	cpumask_t *mask;
	int vectors;
	int off;
	int i;
	lnet_nid_t nid = conn->ibc_peer->ibp_nid;
>>>>>>> v4.9.227

	vectors = conn->ibc_cmid->device->num_comp_vectors;
	if (vectors <= 1)
		return 0;

	mask = cfs_cpt_cpumask(lnet_cpt_table(), cpt);
<<<<<<< HEAD
	if (mask == NULL)
		return 0;

	/* hash NID to CPU id in this partition... */
	off = do_div(nid, cpus_weight(*mask));
	for_each_cpu_mask(i, *mask) {
		if (off-- == 0)
=======
	if (!mask)
		return 0;

	/* hash NID to CPU id in this partition... */
	off = do_div(nid, cpumask_weight(mask));
	for_each_cpu(i, mask) {
		if (!off--)
>>>>>>> v4.9.227
			return i % vectors;
	}

	LBUG();
	return 1;
}

<<<<<<< HEAD
kib_conn_t *
kiblnd_create_conn(kib_peer_t *peer, struct rdma_cm_id *cmid,
		   int state, int version)
{
	/* CAVEAT EMPTOR:
=======
struct kib_conn *kiblnd_create_conn(struct kib_peer *peer, struct rdma_cm_id *cmid,
				    int state, int version)
{
	/*
	 * CAVEAT EMPTOR:
>>>>>>> v4.9.227
	 * If the new conn is created successfully it takes over the caller's
	 * ref on 'peer'.  It also "owns" 'cmid' and destroys it when it itself
	 * is destroyed.  On failure, the caller's ref on 'peer' remains and
	 * she must dispose of 'cmid'.  (Actually I'd block forever if I tried
	 * to destroy 'cmid' here since I'm called from the CM which still has
<<<<<<< HEAD
	 * its ref on 'cmid'). */
	rwlock_t		*glock = &kiblnd_data.kib_global_lock;
	kib_net_t	      *net = peer->ibp_ni->ni_data;
	kib_dev_t	      *dev;
	struct ib_qp_init_attr *init_qp_attr;
	struct kib_sched_info	*sched;
	kib_conn_t		*conn;
	struct ib_cq		*cq;
	unsigned long		flags;
	int			cpt;
	int			rc;
	int			i;

	LASSERT(net != NULL);
=======
	 * its ref on 'cmid').
	 */
	rwlock_t *glock = &kiblnd_data.kib_global_lock;
	struct kib_net *net = peer->ibp_ni->ni_data;
	struct kib_dev *dev;
	struct ib_qp_init_attr *init_qp_attr;
	struct kib_sched_info *sched;
	struct ib_cq_init_attr cq_attr = {};
	struct kib_conn *conn;
	struct ib_cq *cq;
	unsigned long flags;
	int cpt;
	int rc;
	int i;

	LASSERT(net);
>>>>>>> v4.9.227
	LASSERT(!in_interrupt());

	dev = net->ibn_dev;

	cpt = lnet_cpt_of_nid(peer->ibp_nid);
	sched = kiblnd_data.kib_scheds[cpt];

	LASSERT(sched->ibs_nthreads > 0);

	LIBCFS_CPT_ALLOC(init_qp_attr, lnet_cpt_table(), cpt,
			 sizeof(*init_qp_attr));
<<<<<<< HEAD
	if (init_qp_attr == NULL) {
=======
	if (!init_qp_attr) {
>>>>>>> v4.9.227
		CERROR("Can't allocate qp_attr for %s\n",
		       libcfs_nid2str(peer->ibp_nid));
		goto failed_0;
	}

	LIBCFS_CPT_ALLOC(conn, lnet_cpt_table(), cpt, sizeof(*conn));
<<<<<<< HEAD
	if (conn == NULL) {
=======
	if (!conn) {
>>>>>>> v4.9.227
		CERROR("Can't allocate connection for %s\n",
		       libcfs_nid2str(peer->ibp_nid));
		goto failed_1;
	}

	conn->ibc_state = IBLND_CONN_INIT;
	conn->ibc_version = version;
	conn->ibc_peer = peer;		  /* I take the caller's ref */
	cmid->context = conn;		   /* for future CM callbacks */
	conn->ibc_cmid = cmid;
<<<<<<< HEAD
=======
	conn->ibc_max_frags = peer->ibp_max_frags;
	conn->ibc_queue_depth = peer->ibp_queue_depth;
>>>>>>> v4.9.227

	INIT_LIST_HEAD(&conn->ibc_early_rxs);
	INIT_LIST_HEAD(&conn->ibc_tx_noops);
	INIT_LIST_HEAD(&conn->ibc_tx_queue);
	INIT_LIST_HEAD(&conn->ibc_tx_queue_rsrvd);
	INIT_LIST_HEAD(&conn->ibc_tx_queue_nocred);
	INIT_LIST_HEAD(&conn->ibc_active_txs);
	spin_lock_init(&conn->ibc_lock);

	LIBCFS_CPT_ALLOC(conn->ibc_connvars, lnet_cpt_table(), cpt,
			 sizeof(*conn->ibc_connvars));
<<<<<<< HEAD
	if (conn->ibc_connvars == NULL) {
=======
	if (!conn->ibc_connvars) {
>>>>>>> v4.9.227
		CERROR("Can't allocate in-progress connection state\n");
		goto failed_2;
	}

	write_lock_irqsave(glock, flags);
	if (dev->ibd_failover) {
		write_unlock_irqrestore(glock, flags);
		CERROR("%s: failover in progress\n", dev->ibd_ifname);
		goto failed_2;
	}

	if (dev->ibd_hdev->ibh_ibdev != cmid->device) {
		/* wakeup failover thread and teardown connection */
		if (kiblnd_dev_can_failover(dev)) {
			list_add_tail(&dev->ibd_fail_list,
				      &kiblnd_data.kib_failed_devs);
			wake_up(&kiblnd_data.kib_failover_waitq);
		}

		write_unlock_irqrestore(glock, flags);
		CERROR("cmid HCA(%s), kib_dev(%s) need failover\n",
		       cmid->device->name, dev->ibd_ifname);
		goto failed_2;
	}

	kiblnd_hdev_addref_locked(dev->ibd_hdev);
	conn->ibc_hdev = dev->ibd_hdev;

	kiblnd_setup_mtu_locked(cmid);

	write_unlock_irqrestore(glock, flags);

	LIBCFS_CPT_ALLOC(conn->ibc_rxs, lnet_cpt_table(), cpt,
<<<<<<< HEAD
			 IBLND_RX_MSGS(version) * sizeof(kib_rx_t));
	if (conn->ibc_rxs == NULL) {
=======
			 IBLND_RX_MSGS(conn) * sizeof(struct kib_rx));
	if (!conn->ibc_rxs) {
>>>>>>> v4.9.227
		CERROR("Cannot allocate RX buffers\n");
		goto failed_2;
	}

	rc = kiblnd_alloc_pages(&conn->ibc_rx_pages, cpt,
<<<<<<< HEAD
				IBLND_RX_MSG_PAGES(version));
	if (rc != 0)
=======
				IBLND_RX_MSG_PAGES(conn));
	if (rc)
>>>>>>> v4.9.227
		goto failed_2;

	kiblnd_map_rx_descs(conn);

<<<<<<< HEAD
	cq = ib_create_cq(cmid->device,
			  kiblnd_cq_completion, kiblnd_cq_event, conn,
			  IBLND_CQ_ENTRIES(version),
			  kiblnd_get_completion_vector(conn, cpt));
	if (IS_ERR(cq)) {
		CERROR("Can't create CQ: %ld, cqe: %d\n",
		       PTR_ERR(cq), IBLND_CQ_ENTRIES(version));
=======
	cq_attr.cqe = IBLND_CQ_ENTRIES(conn);
	cq_attr.comp_vector = kiblnd_get_completion_vector(conn, cpt);
	cq = ib_create_cq(cmid->device,
			  kiblnd_cq_completion, kiblnd_cq_event, conn,
			  &cq_attr);
	if (IS_ERR(cq)) {
		CERROR("Failed to create CQ with %d CQEs: %ld\n",
		       IBLND_CQ_ENTRIES(conn), PTR_ERR(cq));
>>>>>>> v4.9.227
		goto failed_2;
	}

	conn->ibc_cq = cq;

	rc = ib_req_notify_cq(cq, IB_CQ_NEXT_COMP);
<<<<<<< HEAD
	if (rc != 0) {
		CERROR("Can't request completion notificiation: %d\n", rc);
=======
	if (rc) {
		CERROR("Can't request completion notification: %d\n", rc);
>>>>>>> v4.9.227
		goto failed_2;
	}

	init_qp_attr->event_handler = kiblnd_qp_event;
	init_qp_attr->qp_context = conn;
<<<<<<< HEAD
	init_qp_attr->cap.max_send_wr = IBLND_SEND_WRS(version);
	init_qp_attr->cap.max_recv_wr = IBLND_RECV_WRS(version);
=======
	init_qp_attr->cap.max_send_wr = IBLND_SEND_WRS(conn);
	init_qp_attr->cap.max_recv_wr = IBLND_RECV_WRS(conn);
>>>>>>> v4.9.227
	init_qp_attr->cap.max_send_sge = 1;
	init_qp_attr->cap.max_recv_sge = 1;
	init_qp_attr->sq_sig_type = IB_SIGNAL_REQ_WR;
	init_qp_attr->qp_type = IB_QPT_RC;
	init_qp_attr->send_cq = cq;
	init_qp_attr->recv_cq = cq;

	conn->ibc_sched = sched;

	rc = rdma_create_qp(cmid, conn->ibc_hdev->ibh_pd, init_qp_attr);
<<<<<<< HEAD
	if (rc != 0) {
=======
	if (rc) {
>>>>>>> v4.9.227
		CERROR("Can't create QP: %d, send_wr: %d, recv_wr: %d\n",
		       rc, init_qp_attr->cap.max_send_wr,
		       init_qp_attr->cap.max_recv_wr);
		goto failed_2;
	}

	LIBCFS_FREE(init_qp_attr, sizeof(*init_qp_attr));

	/* 1 ref for caller and each rxmsg */
<<<<<<< HEAD
	atomic_set(&conn->ibc_refcount, 1 + IBLND_RX_MSGS(version));
	conn->ibc_nrx = IBLND_RX_MSGS(version);

	/* post receives */
	for (i = 0; i < IBLND_RX_MSGS(version); i++) {
		rc = kiblnd_post_rx(&conn->ibc_rxs[i],
				    IBLND_POSTRX_NO_CREDIT);
		if (rc != 0) {
=======
	atomic_set(&conn->ibc_refcount, 1 + IBLND_RX_MSGS(conn));
	conn->ibc_nrx = IBLND_RX_MSGS(conn);

	/* post receives */
	for (i = 0; i < IBLND_RX_MSGS(conn); i++) {
		rc = kiblnd_post_rx(&conn->ibc_rxs[i],
				    IBLND_POSTRX_NO_CREDIT);
		if (rc) {
>>>>>>> v4.9.227
			CERROR("Can't post rxmsg: %d\n", rc);

			/* Make posted receives complete */
			kiblnd_abort_receives(conn);

<<<<<<< HEAD
			/* correct # of posted buffers
			 * NB locking needed now I'm racing with completion */
			spin_lock_irqsave(&sched->ibs_lock, flags);
			conn->ibc_nrx -= IBLND_RX_MSGS(version) - i;
			spin_unlock_irqrestore(&sched->ibs_lock, flags);

			/* cmid will be destroyed by CM(ofed) after cm_callback
			 * returned, so we can't refer it anymore
			 * (by kiblnd_connd()->kiblnd_destroy_conn) */
=======
			/*
			 * correct # of posted buffers
			 * NB locking needed now I'm racing with completion
			 */
			spin_lock_irqsave(&sched->ibs_lock, flags);
			conn->ibc_nrx -= IBLND_RX_MSGS(conn) - i;
			spin_unlock_irqrestore(&sched->ibs_lock, flags);

			/*
			 * cmid will be destroyed by CM(ofed) after cm_callback
			 * returned, so we can't refer it anymore
			 * (by kiblnd_connd()->kiblnd_destroy_conn)
			 */
>>>>>>> v4.9.227
			rdma_destroy_qp(conn->ibc_cmid);
			conn->ibc_cmid = NULL;

			/* Drop my own and unused rxbuffer refcounts */
<<<<<<< HEAD
			while (i++ <= IBLND_RX_MSGS(version))
=======
			while (i++ <= IBLND_RX_MSGS(conn))
>>>>>>> v4.9.227
				kiblnd_conn_decref(conn);

			return NULL;
		}
	}

	/* Init successful! */
<<<<<<< HEAD
	LASSERT (state == IBLND_CONN_ACTIVE_CONNECT ||
		 state == IBLND_CONN_PASSIVE_WAIT);
=======
	LASSERT(state == IBLND_CONN_ACTIVE_CONNECT ||
		state == IBLND_CONN_PASSIVE_WAIT);
>>>>>>> v4.9.227
	conn->ibc_state = state;

	/* 1 more conn */
	atomic_inc(&net->ibn_nconns);
	return conn;

 failed_2:
	kiblnd_destroy_conn(conn);
<<<<<<< HEAD
=======
	LIBCFS_FREE(conn, sizeof(*conn));
>>>>>>> v4.9.227
 failed_1:
	LIBCFS_FREE(init_qp_attr, sizeof(*init_qp_attr));
 failed_0:
	return NULL;
}

<<<<<<< HEAD
void
kiblnd_destroy_conn (kib_conn_t *conn)
{
	struct rdma_cm_id *cmid = conn->ibc_cmid;
	kib_peer_t	*peer = conn->ibc_peer;
	int		rc;

	LASSERT (!in_interrupt());
	LASSERT (atomic_read(&conn->ibc_refcount) == 0);
	LASSERT (list_empty(&conn->ibc_early_rxs));
	LASSERT (list_empty(&conn->ibc_tx_noops));
	LASSERT (list_empty(&conn->ibc_tx_queue));
	LASSERT (list_empty(&conn->ibc_tx_queue_rsrvd));
	LASSERT (list_empty(&conn->ibc_tx_queue_nocred));
	LASSERT (list_empty(&conn->ibc_active_txs));
	LASSERT (conn->ibc_noops_posted == 0);
	LASSERT (conn->ibc_nsends_posted == 0);
=======
void kiblnd_destroy_conn(struct kib_conn *conn)
{
	struct rdma_cm_id *cmid = conn->ibc_cmid;
	struct kib_peer *peer = conn->ibc_peer;
	int rc;

	LASSERT(!in_interrupt());
	LASSERT(!atomic_read(&conn->ibc_refcount));
	LASSERT(list_empty(&conn->ibc_early_rxs));
	LASSERT(list_empty(&conn->ibc_tx_noops));
	LASSERT(list_empty(&conn->ibc_tx_queue));
	LASSERT(list_empty(&conn->ibc_tx_queue_rsrvd));
	LASSERT(list_empty(&conn->ibc_tx_queue_nocred));
	LASSERT(list_empty(&conn->ibc_active_txs));
	LASSERT(!conn->ibc_noops_posted);
	LASSERT(!conn->ibc_nsends_posted);
>>>>>>> v4.9.227

	switch (conn->ibc_state) {
	default:
		/* conn must be completely disengaged from the network */
		LBUG();

	case IBLND_CONN_DISCONNECTED:
		/* connvars should have been freed already */
<<<<<<< HEAD
		LASSERT (conn->ibc_connvars == NULL);
=======
		LASSERT(!conn->ibc_connvars);
>>>>>>> v4.9.227
		break;

	case IBLND_CONN_INIT:
		break;
	}

	/* conn->ibc_cmid might be destroyed by CM already */
<<<<<<< HEAD
	if (cmid != NULL && cmid->qp != NULL)
		rdma_destroy_qp(cmid);

	if (conn->ibc_cq != NULL) {
		rc = ib_destroy_cq(conn->ibc_cq);
		if (rc != 0)
			CWARN("Error destroying CQ: %d\n", rc);
	}

	if (conn->ibc_rx_pages != NULL)
		kiblnd_unmap_rx_descs(conn);

	if (conn->ibc_rxs != NULL) {
		LIBCFS_FREE(conn->ibc_rxs,
			    IBLND_RX_MSGS(conn->ibc_version) * sizeof(kib_rx_t));
	}

	if (conn->ibc_connvars != NULL)
		LIBCFS_FREE(conn->ibc_connvars, sizeof(*conn->ibc_connvars));

	if (conn->ibc_hdev != NULL)
=======
	if (cmid && cmid->qp)
		rdma_destroy_qp(cmid);

	if (conn->ibc_cq) {
		rc = ib_destroy_cq(conn->ibc_cq);
		if (rc)
			CWARN("Error destroying CQ: %d\n", rc);
	}

	if (conn->ibc_rx_pages)
		kiblnd_unmap_rx_descs(conn);

	if (conn->ibc_rxs) {
		LIBCFS_FREE(conn->ibc_rxs,
			    IBLND_RX_MSGS(conn) * sizeof(struct kib_rx));
	}

	if (conn->ibc_connvars)
		LIBCFS_FREE(conn->ibc_connvars, sizeof(*conn->ibc_connvars));

	if (conn->ibc_hdev)
>>>>>>> v4.9.227
		kiblnd_hdev_decref(conn->ibc_hdev);

	/* See CAVEAT EMPTOR above in kiblnd_create_conn */
	if (conn->ibc_state != IBLND_CONN_INIT) {
<<<<<<< HEAD
		kib_net_t *net = peer->ibp_ni->ni_data;
=======
		struct kib_net *net = peer->ibp_ni->ni_data;
>>>>>>> v4.9.227

		kiblnd_peer_decref(peer);
		rdma_destroy_id(cmid);
		atomic_dec(&net->ibn_nconns);
	}
<<<<<<< HEAD

	LIBCFS_FREE(conn, sizeof(*conn));
}

int
kiblnd_close_peer_conns_locked (kib_peer_t *peer, int why)
{
	kib_conn_t	     *conn;
	struct list_head	     *ctmp;
	struct list_head	     *cnxt;
	int		     count = 0;

	list_for_each_safe (ctmp, cnxt, &peer->ibp_conns) {
		conn = list_entry(ctmp, kib_conn_t, ibc_list);

		CDEBUG(D_NET, "Closing conn -> %s, "
			      "version: %x, reason: %d\n",
=======
}

int kiblnd_close_peer_conns_locked(struct kib_peer *peer, int why)
{
	struct kib_conn *conn;
	struct list_head *ctmp;
	struct list_head *cnxt;
	int count = 0;

	list_for_each_safe(ctmp, cnxt, &peer->ibp_conns) {
		conn = list_entry(ctmp, struct kib_conn, ibc_list);

		CDEBUG(D_NET, "Closing conn -> %s, version: %x, reason: %d\n",
>>>>>>> v4.9.227
		       libcfs_nid2str(peer->ibp_nid),
		       conn->ibc_version, why);

		kiblnd_close_conn_locked(conn, why);
		count++;
	}

	return count;
}

<<<<<<< HEAD
int
kiblnd_close_stale_conns_locked (kib_peer_t *peer,
				 int version, __u64 incarnation)
{
	kib_conn_t	     *conn;
	struct list_head	     *ctmp;
	struct list_head	     *cnxt;
	int		     count = 0;

	list_for_each_safe (ctmp, cnxt, &peer->ibp_conns) {
		conn = list_entry(ctmp, kib_conn_t, ibc_list);
=======
int kiblnd_close_stale_conns_locked(struct kib_peer *peer,
				    int version, __u64 incarnation)
{
	struct kib_conn *conn;
	struct list_head *ctmp;
	struct list_head *cnxt;
	int count = 0;

	list_for_each_safe(ctmp, cnxt, &peer->ibp_conns) {
		conn = list_entry(ctmp, struct kib_conn, ibc_list);
>>>>>>> v4.9.227

		if (conn->ibc_version     == version &&
		    conn->ibc_incarnation == incarnation)
			continue;

<<<<<<< HEAD
		CDEBUG(D_NET, "Closing stale conn -> %s version: %x, "
			      "incarnation:%#llx(%x, %#llx)\n",
=======
		CDEBUG(D_NET,
		       "Closing stale conn -> %s version: %x, incarnation:%#llx(%x, %#llx)\n",
>>>>>>> v4.9.227
		       libcfs_nid2str(peer->ibp_nid),
		       conn->ibc_version, conn->ibc_incarnation,
		       version, incarnation);

		kiblnd_close_conn_locked(conn, -ESTALE);
		count++;
	}

	return count;
}

<<<<<<< HEAD
static int
kiblnd_close_matching_conns(lnet_ni_t *ni, lnet_nid_t nid)
{
	kib_peer_t	     *peer;
	struct list_head	     *ptmp;
	struct list_head	     *pnxt;
	int		     lo;
	int		     hi;
	int		     i;
	unsigned long	   flags;
	int		     count = 0;

	write_lock_irqsave(&kiblnd_data.kib_global_lock, flags);

	if (nid != LNET_NID_ANY)
		lo = hi = kiblnd_nid2peerlist(nid) - kiblnd_data.kib_peers;
	else {
=======
static int kiblnd_close_matching_conns(lnet_ni_t *ni, lnet_nid_t nid)
{
	struct kib_peer *peer;
	struct list_head *ptmp;
	struct list_head *pnxt;
	int lo;
	int hi;
	int i;
	unsigned long flags;
	int count = 0;

	write_lock_irqsave(&kiblnd_data.kib_global_lock, flags);

	if (nid != LNET_NID_ANY) {
		lo = kiblnd_nid2peerlist(nid) - kiblnd_data.kib_peers;
		hi = kiblnd_nid2peerlist(nid) - kiblnd_data.kib_peers;
	} else {
>>>>>>> v4.9.227
		lo = 0;
		hi = kiblnd_data.kib_peer_hash_size - 1;
	}

	for (i = lo; i <= hi; i++) {
<<<<<<< HEAD
		list_for_each_safe (ptmp, pnxt, &kiblnd_data.kib_peers[i]) {

			peer = list_entry(ptmp, kib_peer_t, ibp_list);
			LASSERT (peer->ibp_connecting > 0 ||
				 peer->ibp_accepting > 0 ||
				 !list_empty(&peer->ibp_conns));
=======
		list_for_each_safe(ptmp, pnxt, &kiblnd_data.kib_peers[i]) {
			peer = list_entry(ptmp, struct kib_peer, ibp_list);
			LASSERT(!kiblnd_peer_idle(peer));
>>>>>>> v4.9.227

			if (peer->ibp_ni != ni)
				continue;

			if (!(nid == LNET_NID_ANY || nid == peer->ibp_nid))
				continue;

			count += kiblnd_close_peer_conns_locked(peer, 0);
		}
	}

	write_unlock_irqrestore(&kiblnd_data.kib_global_lock, flags);

	/* wildcards always succeed */
	if (nid == LNET_NID_ANY)
		return 0;

<<<<<<< HEAD
	return (count == 0) ? -ENOENT : 0;
}

int
kiblnd_ctl(lnet_ni_t *ni, unsigned int cmd, void *arg)
{
	struct libcfs_ioctl_data *data = arg;
	int		       rc = -EINVAL;

	switch (cmd) {
	case IOC_LIBCFS_GET_PEER: {
		lnet_nid_t   nid = 0;
		int	  count = 0;

		rc = kiblnd_get_peer_info(ni, data->ioc_count,
					  &nid, &count);
		data->ioc_nid    = nid;
		data->ioc_count  = count;
=======
	return !count ? -ENOENT : 0;
}

static int kiblnd_ctl(lnet_ni_t *ni, unsigned int cmd, void *arg)
{
	struct libcfs_ioctl_data *data = arg;
	int rc = -EINVAL;

	switch (cmd) {
	case IOC_LIBCFS_GET_PEER: {
		lnet_nid_t nid = 0;
		int count = 0;

		rc = kiblnd_get_peer_info(ni, data->ioc_count,
					  &nid, &count);
		data->ioc_nid   = nid;
		data->ioc_count = count;
>>>>>>> v4.9.227
		break;
	}

	case IOC_LIBCFS_DEL_PEER: {
		rc = kiblnd_del_peer(ni, data->ioc_nid);
		break;
	}
	case IOC_LIBCFS_GET_CONN: {
<<<<<<< HEAD
		kib_conn_t *conn;

		rc = 0;
		conn = kiblnd_get_conn_by_idx(ni, data->ioc_count);
		if (conn == NULL) {
=======
		struct kib_conn *conn;

		rc = 0;
		conn = kiblnd_get_conn_by_idx(ni, data->ioc_count);
		if (!conn) {
>>>>>>> v4.9.227
			rc = -ENOENT;
			break;
		}

<<<<<<< HEAD
		LASSERT (conn->ibc_cmid != NULL);
		data->ioc_nid = conn->ibc_peer->ibp_nid;
		if (conn->ibc_cmid->route.path_rec == NULL)
=======
		LASSERT(conn->ibc_cmid);
		data->ioc_nid = conn->ibc_peer->ibp_nid;
		if (!conn->ibc_cmid->route.path_rec)
>>>>>>> v4.9.227
			data->ioc_u32[0] = 0; /* iWarp has no path MTU */
		else
			data->ioc_u32[0] =
			ib_mtu_enum_to_int(conn->ibc_cmid->route.path_rec->mtu);
		kiblnd_conn_decref(conn);
		break;
	}
	case IOC_LIBCFS_CLOSE_CONNECTION: {
		rc = kiblnd_close_matching_conns(ni, data->ioc_nid);
		break;
	}

	default:
		break;
	}

	return rc;
}

<<<<<<< HEAD
void
kiblnd_query (lnet_ni_t *ni, lnet_nid_t nid, unsigned long *when)
{
	unsigned long	last_alive = 0;
	unsigned long	now = cfs_time_current();
	rwlock_t	*glock = &kiblnd_data.kib_global_lock;
	kib_peer_t	*peer;
	unsigned long	flags;
=======
static void kiblnd_query(lnet_ni_t *ni, lnet_nid_t nid, unsigned long *when)
{
	unsigned long last_alive = 0;
	unsigned long now = cfs_time_current();
	rwlock_t *glock = &kiblnd_data.kib_global_lock;
	struct kib_peer *peer;
	unsigned long flags;
>>>>>>> v4.9.227

	read_lock_irqsave(glock, flags);

	peer = kiblnd_find_peer_locked(nid);
<<<<<<< HEAD
	if (peer != NULL) {
		LASSERT (peer->ibp_connecting > 0 || /* creating conns */
			 peer->ibp_accepting > 0 ||
			 !list_empty(&peer->ibp_conns));  /* active conn */
		last_alive = peer->ibp_last_alive;
	}

	read_unlock_irqrestore(glock, flags);

	if (last_alive != 0)
		*when = last_alive;

	/* peer is not persistent in hash, trigger peer creation
	 * and connection establishment with a NULL tx */
	if (peer == NULL)
=======
	if (peer)
		last_alive = peer->ibp_last_alive;

	read_unlock_irqrestore(glock, flags);

	if (last_alive)
		*when = last_alive;

	/*
	 * peer is not persistent in hash, trigger peer creation
	 * and connection establishment with a NULL tx
	 */
	if (!peer)
>>>>>>> v4.9.227
		kiblnd_launch_tx(ni, NULL, nid);

	CDEBUG(D_NET, "Peer %s %p, alive %ld secs ago\n",
	       libcfs_nid2str(nid), peer,
	       last_alive ? cfs_duration_sec(now - last_alive) : -1);
<<<<<<< HEAD
	return;
}

void
kiblnd_free_pages(kib_pages_t *p)
{
	int	npages = p->ibp_npages;
	int	i;

	for (i = 0; i < npages; i++) {
		if (p->ibp_pages[i] != NULL)
			__free_page(p->ibp_pages[i]);
	}

	LIBCFS_FREE(p, offsetof(kib_pages_t, ibp_pages[npages]));
}

int
kiblnd_alloc_pages(kib_pages_t **pp, int cpt, int npages)
{
	kib_pages_t	*p;
	int		i;

	LIBCFS_CPT_ALLOC(p, lnet_cpt_table(), cpt,
			 offsetof(kib_pages_t, ibp_pages[npages]));
	if (p == NULL) {
=======
}

static void kiblnd_free_pages(struct kib_pages *p)
{
	int npages = p->ibp_npages;
	int i;

	for (i = 0; i < npages; i++) {
		if (p->ibp_pages[i])
			__free_page(p->ibp_pages[i]);
	}

	LIBCFS_FREE(p, offsetof(struct kib_pages, ibp_pages[npages]));
}

int kiblnd_alloc_pages(struct kib_pages **pp, int cpt, int npages)
{
	struct kib_pages *p;
	int i;

	LIBCFS_CPT_ALLOC(p, lnet_cpt_table(), cpt,
			 offsetof(struct kib_pages, ibp_pages[npages]));
	if (!p) {
>>>>>>> v4.9.227
		CERROR("Can't allocate descriptor for %d pages\n", npages);
		return -ENOMEM;
	}

<<<<<<< HEAD
	memset(p, 0, offsetof(kib_pages_t, ibp_pages[npages]));
=======
	memset(p, 0, offsetof(struct kib_pages, ibp_pages[npages]));
>>>>>>> v4.9.227
	p->ibp_npages = npages;

	for (i = 0; i < npages; i++) {
		p->ibp_pages[i] = alloc_pages_node(
				    cfs_cpt_spread_node(lnet_cpt_table(), cpt),
				    GFP_NOFS, 0);
<<<<<<< HEAD
		if (p->ibp_pages[i] == NULL) {
=======
		if (!p->ibp_pages[i]) {
>>>>>>> v4.9.227
			CERROR("Can't allocate page %d of %d\n", i, npages);
			kiblnd_free_pages(p);
			return -ENOMEM;
		}
	}

	*pp = p;
	return 0;
}

<<<<<<< HEAD
void
kiblnd_unmap_rx_descs(kib_conn_t *conn)
{
	kib_rx_t *rx;
	int       i;

	LASSERT (conn->ibc_rxs != NULL);
	LASSERT (conn->ibc_hdev != NULL);

	for (i = 0; i < IBLND_RX_MSGS(conn->ibc_version); i++) {
		rx = &conn->ibc_rxs[i];

		LASSERT (rx->rx_nob >= 0); /* not posted */
=======
void kiblnd_unmap_rx_descs(struct kib_conn *conn)
{
	struct kib_rx *rx;
	int i;

	LASSERT(conn->ibc_rxs);
	LASSERT(conn->ibc_hdev);

	for (i = 0; i < IBLND_RX_MSGS(conn); i++) {
		rx = &conn->ibc_rxs[i];

		LASSERT(rx->rx_nob >= 0); /* not posted */
>>>>>>> v4.9.227

		kiblnd_dma_unmap_single(conn->ibc_hdev->ibh_ibdev,
					KIBLND_UNMAP_ADDR(rx, rx_msgunmap,
							  rx->rx_msgaddr),
					IBLND_MSG_SIZE, DMA_FROM_DEVICE);
	}

	kiblnd_free_pages(conn->ibc_rx_pages);

	conn->ibc_rx_pages = NULL;
}

<<<<<<< HEAD
void
kiblnd_map_rx_descs(kib_conn_t *conn)
{
	kib_rx_t       *rx;
	struct page    *pg;
	int	     pg_off;
	int	     ipg;
	int	     i;

	for (pg_off = ipg = i = 0;
	     i < IBLND_RX_MSGS(conn->ibc_version); i++) {
=======
void kiblnd_map_rx_descs(struct kib_conn *conn)
{
	struct kib_rx *rx;
	struct page *pg;
	int pg_off;
	int ipg;
	int i;

	for (pg_off = ipg = i = 0; i < IBLND_RX_MSGS(conn); i++) {
>>>>>>> v4.9.227
		pg = conn->ibc_rx_pages->ibp_pages[ipg];
		rx = &conn->ibc_rxs[i];

		rx->rx_conn = conn;
<<<<<<< HEAD
		rx->rx_msg = (kib_msg_t *)(((char *)page_address(pg)) + pg_off);

		rx->rx_msgaddr = kiblnd_dma_map_single(conn->ibc_hdev->ibh_ibdev,
						       rx->rx_msg, IBLND_MSG_SIZE,
						       DMA_FROM_DEVICE);
		LASSERT (!kiblnd_dma_mapping_error(conn->ibc_hdev->ibh_ibdev,
						   rx->rx_msgaddr));
=======
		rx->rx_msg = (struct kib_msg *)(((char *)page_address(pg)) + pg_off);

		rx->rx_msgaddr = kiblnd_dma_map_single(conn->ibc_hdev->ibh_ibdev,
						       rx->rx_msg,
						       IBLND_MSG_SIZE,
						       DMA_FROM_DEVICE);
		LASSERT(!kiblnd_dma_mapping_error(conn->ibc_hdev->ibh_ibdev,
						  rx->rx_msgaddr));
>>>>>>> v4.9.227
		KIBLND_UNMAP_ADDR_SET(rx, rx_msgunmap, rx->rx_msgaddr);

		CDEBUG(D_NET, "rx %d: %p %#llx(%#llx)\n",
		       i, rx->rx_msg, rx->rx_msgaddr,
<<<<<<< HEAD
		       lnet_page2phys(pg) + pg_off);

		pg_off += IBLND_MSG_SIZE;
		LASSERT (pg_off <= PAGE_SIZE);
=======
		       (__u64)(page_to_phys(pg) + pg_off));

		pg_off += IBLND_MSG_SIZE;
		LASSERT(pg_off <= PAGE_SIZE);
>>>>>>> v4.9.227

		if (pg_off == PAGE_SIZE) {
			pg_off = 0;
			ipg++;
<<<<<<< HEAD
			LASSERT (ipg <= IBLND_RX_MSG_PAGES(conn->ibc_version));
=======
			LASSERT(ipg <= IBLND_RX_MSG_PAGES(conn));
>>>>>>> v4.9.227
		}
	}
}

<<<<<<< HEAD
static void
kiblnd_unmap_tx_pool(kib_tx_pool_t *tpo)
{
	kib_hca_dev_t  *hdev = tpo->tpo_hdev;
	kib_tx_t       *tx;
	int	     i;

	LASSERT (tpo->tpo_pool.po_allocated == 0);

	if (hdev == NULL)
=======
static void kiblnd_unmap_tx_pool(struct kib_tx_pool *tpo)
{
	struct kib_hca_dev *hdev = tpo->tpo_hdev;
	struct kib_tx *tx;
	int i;

	LASSERT(!tpo->tpo_pool.po_allocated);

	if (!hdev)
>>>>>>> v4.9.227
		return;

	for (i = 0; i < tpo->tpo_pool.po_size; i++) {
		tx = &tpo->tpo_tx_descs[i];
		kiblnd_dma_unmap_single(hdev->ibh_ibdev,
					KIBLND_UNMAP_ADDR(tx, tx_msgunmap,
							  tx->tx_msgaddr),
					IBLND_MSG_SIZE, DMA_TO_DEVICE);
	}

	kiblnd_hdev_decref(hdev);
	tpo->tpo_hdev = NULL;
}

<<<<<<< HEAD
static kib_hca_dev_t *
kiblnd_current_hdev(kib_dev_t *dev)
{
	kib_hca_dev_t *hdev;
	unsigned long  flags;
	int	    i = 0;
=======
static struct kib_hca_dev *kiblnd_current_hdev(struct kib_dev *dev)
{
	struct kib_hca_dev *hdev;
	unsigned long flags;
	int i = 0;
>>>>>>> v4.9.227

	read_lock_irqsave(&kiblnd_data.kib_global_lock, flags);
	while (dev->ibd_failover) {
		read_unlock_irqrestore(&kiblnd_data.kib_global_lock, flags);
<<<<<<< HEAD
		if (i++ % 50 == 0)
			CDEBUG(D_NET, "%s: Wait for failover\n",
			       dev->ibd_ifname);
=======
		if (!(i++ % 50))
			CDEBUG(D_NET, "%s: Wait for failover\n",
			       dev->ibd_ifname);
		set_current_state(TASK_INTERRUPTIBLE);
>>>>>>> v4.9.227
		schedule_timeout(cfs_time_seconds(1) / 100);

		read_lock_irqsave(&kiblnd_data.kib_global_lock, flags);
	}

	kiblnd_hdev_addref_locked(dev->ibd_hdev);
	hdev = dev->ibd_hdev;

	read_unlock_irqrestore(&kiblnd_data.kib_global_lock, flags);

	return hdev;
}

<<<<<<< HEAD
static void
kiblnd_map_tx_pool(kib_tx_pool_t *tpo)
{
	kib_pages_t    *txpgs = tpo->tpo_tx_pages;
	kib_pool_t     *pool  = &tpo->tpo_pool;
	kib_net_t      *net   = pool->po_owner->ps_net;
	kib_dev_t      *dev;
	struct page    *page;
	kib_tx_t       *tx;
	int	     page_offset;
	int	     ipage;
	int	     i;

	LASSERT (net != NULL);
=======
static void kiblnd_map_tx_pool(struct kib_tx_pool *tpo)
{
	struct kib_pages *txpgs = tpo->tpo_tx_pages;
	struct kib_pool *pool = &tpo->tpo_pool;
	struct kib_net *net = pool->po_owner->ps_net;
	struct kib_dev *dev;
	struct page *page;
	struct kib_tx *tx;
	int page_offset;
	int ipage;
	int i;

	LASSERT(net);
>>>>>>> v4.9.227

	dev = net->ibn_dev;

	/* pre-mapped messages are not bigger than 1 page */
<<<<<<< HEAD
	CLASSERT (IBLND_MSG_SIZE <= PAGE_SIZE);

	/* No fancy arithmetic when we do the buffer calculations */
	CLASSERT (PAGE_SIZE % IBLND_MSG_SIZE == 0);
=======
	CLASSERT(IBLND_MSG_SIZE <= PAGE_SIZE);

	/* No fancy arithmetic when we do the buffer calculations */
	CLASSERT(!(PAGE_SIZE % IBLND_MSG_SIZE));
>>>>>>> v4.9.227

	tpo->tpo_hdev = kiblnd_current_hdev(dev);

	for (ipage = page_offset = i = 0; i < pool->po_size; i++) {
		page = txpgs->ibp_pages[ipage];
		tx = &tpo->tpo_tx_descs[i];

<<<<<<< HEAD
		tx->tx_msg = (kib_msg_t *)(((char *)page_address(page)) +
=======
		tx->tx_msg = (struct kib_msg *)(((char *)page_address(page)) +
>>>>>>> v4.9.227
					   page_offset);

		tx->tx_msgaddr = kiblnd_dma_map_single(
			tpo->tpo_hdev->ibh_ibdev, tx->tx_msg,
			IBLND_MSG_SIZE, DMA_TO_DEVICE);
<<<<<<< HEAD
		LASSERT (!kiblnd_dma_mapping_error(tpo->tpo_hdev->ibh_ibdev,
						   tx->tx_msgaddr));
=======
		LASSERT(!kiblnd_dma_mapping_error(tpo->tpo_hdev->ibh_ibdev,
						  tx->tx_msgaddr));
>>>>>>> v4.9.227
		KIBLND_UNMAP_ADDR_SET(tx, tx_msgunmap, tx->tx_msgaddr);

		list_add(&tx->tx_list, &pool->po_free_list);

		page_offset += IBLND_MSG_SIZE;
<<<<<<< HEAD
		LASSERT (page_offset <= PAGE_SIZE);
=======
		LASSERT(page_offset <= PAGE_SIZE);
>>>>>>> v4.9.227

		if (page_offset == PAGE_SIZE) {
			page_offset = 0;
			ipage++;
<<<<<<< HEAD
			LASSERT (ipage <= txpgs->ibp_npages);
=======
			LASSERT(ipage <= txpgs->ibp_npages);
>>>>>>> v4.9.227
		}
	}
}

<<<<<<< HEAD
struct ib_mr *
kiblnd_find_dma_mr(kib_hca_dev_t *hdev, __u64 addr, __u64 size)
{
	__u64   index;

	LASSERT (hdev->ibh_mrs[0] != NULL);

	if (hdev->ibh_nmrs == 1)
		return hdev->ibh_mrs[0];

	index = addr >> hdev->ibh_mr_shift;

	if (index <  hdev->ibh_nmrs &&
	    index == ((addr + size - 1) >> hdev->ibh_mr_shift))
		return hdev->ibh_mrs[index];

	return NULL;
}

struct ib_mr *
kiblnd_find_rd_dma_mr(kib_hca_dev_t *hdev, kib_rdma_desc_t *rd)
{
	struct ib_mr *prev_mr;
	struct ib_mr *mr;
	int	   i;

	LASSERT (hdev->ibh_mrs[0] != NULL);

	if (*kiblnd_tunables.kib_map_on_demand > 0 &&
	    *kiblnd_tunables.kib_map_on_demand <= rd->rd_nfrags)
		return NULL;

	if (hdev->ibh_nmrs == 1)
		return hdev->ibh_mrs[0];

	for (i = 0, mr = prev_mr = NULL;
	     i < rd->rd_nfrags; i++) {
		mr = kiblnd_find_dma_mr(hdev,
					rd->rd_frags[i].rf_addr,
					rd->rd_frags[i].rf_nob);
		if (prev_mr == NULL)
			prev_mr = mr;

		if (mr == NULL || prev_mr != mr) {
			/* Can't covered by one single MR */
			mr = NULL;
			break;
		}
	}

	return mr;
}

static void
kiblnd_destroy_fmr_pool(kib_fmr_pool_t *pool)
{
	LASSERT (pool->fpo_map_count == 0);

	if (pool->fpo_fmr_pool != NULL)
		ib_destroy_fmr_pool(pool->fpo_fmr_pool);

	if (pool->fpo_hdev != NULL)
		kiblnd_hdev_decref(pool->fpo_hdev);

	LIBCFS_FREE(pool, sizeof(kib_fmr_pool_t));
}

static void
kiblnd_destroy_fmr_pool_list(struct list_head *head)
{
	kib_fmr_pool_t *pool;

	while (!list_empty(head)) {
		pool = list_entry(head->next, kib_fmr_pool_t, fpo_list);
		list_del(&pool->fpo_list);
		kiblnd_destroy_fmr_pool(pool);
	}
}

static int kiblnd_fmr_pool_size(int ncpts)
{
	int size = *kiblnd_tunables.kib_fmr_pool_size / ncpts;
=======
struct ib_mr *kiblnd_find_rd_dma_mr(struct lnet_ni *ni, struct kib_rdma_desc *rd,
				    int negotiated_nfrags)
{
	struct kib_net *net = ni->ni_data;
	struct kib_hca_dev *hdev = net->ibn_dev->ibd_hdev;
	struct lnet_ioctl_config_o2iblnd_tunables *tunables;
	__u16 nfrags;
	int mod;

	tunables = &ni->ni_lnd_tunables->lt_tun_u.lt_o2ib;
	mod = tunables->lnd_map_on_demand;
	nfrags = (negotiated_nfrags != -1) ? negotiated_nfrags : mod;

	LASSERT(hdev->ibh_mrs);

	if (mod > 0 && nfrags <= rd->rd_nfrags)
		return NULL;

	return hdev->ibh_mrs;
}

static void kiblnd_destroy_fmr_pool(struct kib_fmr_pool *fpo)
{
	LASSERT(!fpo->fpo_map_count);

	if (fpo->fpo_is_fmr) {
		if (fpo->fmr.fpo_fmr_pool)
			ib_destroy_fmr_pool(fpo->fmr.fpo_fmr_pool);
	} else {
		struct kib_fast_reg_descriptor *frd, *tmp;
		int i = 0;

		list_for_each_entry_safe(frd, tmp, &fpo->fast_reg.fpo_pool_list,
					 frd_list) {
			list_del(&frd->frd_list);
			ib_dereg_mr(frd->frd_mr);
			LIBCFS_FREE(frd, sizeof(*frd));
			i++;
		}
		if (i < fpo->fast_reg.fpo_pool_size)
			CERROR("FastReg pool still has %d regions registered\n",
			       fpo->fast_reg.fpo_pool_size - i);
	}

	if (fpo->fpo_hdev)
		kiblnd_hdev_decref(fpo->fpo_hdev);

	LIBCFS_FREE(fpo, sizeof(*fpo));
}

static void kiblnd_destroy_fmr_pool_list(struct list_head *head)
{
	struct kib_fmr_pool *fpo, *tmp;

	list_for_each_entry_safe(fpo, tmp, head, fpo_list) {
		list_del(&fpo->fpo_list);
		kiblnd_destroy_fmr_pool(fpo);
	}
}

static int
kiblnd_fmr_pool_size(struct lnet_ioctl_config_o2iblnd_tunables *tunables,
		     int ncpts)
{
	int size = tunables->lnd_fmr_pool_size / ncpts;
>>>>>>> v4.9.227

	return max(IBLND_FMR_POOL, size);
}

<<<<<<< HEAD
static int kiblnd_fmr_flush_trigger(int ncpts)
{
	int size = *kiblnd_tunables.kib_fmr_flush_trigger / ncpts;
=======
static int
kiblnd_fmr_flush_trigger(struct lnet_ioctl_config_o2iblnd_tunables *tunables,
			 int ncpts)
{
	int size = tunables->lnd_fmr_flush_trigger / ncpts;
>>>>>>> v4.9.227

	return max(IBLND_FMR_POOL_FLUSH, size);
}

<<<<<<< HEAD
static int
kiblnd_create_fmr_pool(kib_fmr_poolset_t *fps, kib_fmr_pool_t **pp_fpo)
{
	/* FMR pool for RDMA */
	kib_dev_t	       *dev = fps->fps_net->ibn_dev;
	kib_fmr_pool_t	  *fpo;
	struct ib_fmr_pool_param param = {
		.max_pages_per_fmr = LNET_MAX_PAYLOAD/PAGE_SIZE,
		.page_shift	= PAGE_SHIFT,
		.access	    = (IB_ACCESS_LOCAL_WRITE |
				      IB_ACCESS_REMOTE_WRITE),
		.pool_size	   = fps->fps_pool_size,
		.dirty_watermark   = fps->fps_flush_trigger,
		.flush_function    = NULL,
		.flush_arg	 = NULL,
		.cache	     = !!*kiblnd_tunables.kib_fmr_cache};
	int rc;

	LIBCFS_CPT_ALLOC(fpo, lnet_cpt_table(), fps->fps_cpt, sizeof(*fpo));
	if (fpo == NULL)
		return -ENOMEM;

	fpo->fpo_hdev = kiblnd_current_hdev(dev);

	fpo->fpo_fmr_pool = ib_create_fmr_pool(fpo->fpo_hdev->ibh_pd, &param);
	if (IS_ERR(fpo->fpo_fmr_pool)) {
		rc = PTR_ERR(fpo->fpo_fmr_pool);
		CERROR("Failed to create FMR pool: %d\n", rc);

		kiblnd_hdev_decref(fpo->fpo_hdev);
		LIBCFS_FREE(fpo, sizeof(kib_fmr_pool_t));
		return rc;
	}

	fpo->fpo_deadline = cfs_time_shift(IBLND_POOL_DEADLINE);
	fpo->fpo_owner    = fps;
	*pp_fpo = fpo;

	return 0;
}

static void
kiblnd_fail_fmr_poolset(kib_fmr_poolset_t *fps, struct list_head *zombies)
{
	if (fps->fps_net == NULL) /* intialized? */
=======
static int kiblnd_alloc_fmr_pool(struct kib_fmr_poolset *fps, struct kib_fmr_pool *fpo)
{
	struct ib_fmr_pool_param param = {
		.max_pages_per_fmr = LNET_MAX_PAYLOAD / PAGE_SIZE,
		.page_shift        = PAGE_SHIFT,
		.access            = (IB_ACCESS_LOCAL_WRITE |
				      IB_ACCESS_REMOTE_WRITE),
		.pool_size         = fps->fps_pool_size,
		.dirty_watermark   = fps->fps_flush_trigger,
		.flush_function    = NULL,
		.flush_arg         = NULL,
		.cache             = !!fps->fps_cache };
	int rc = 0;

	fpo->fmr.fpo_fmr_pool = ib_create_fmr_pool(fpo->fpo_hdev->ibh_pd,
						   &param);
	if (IS_ERR(fpo->fmr.fpo_fmr_pool)) {
		rc = PTR_ERR(fpo->fmr.fpo_fmr_pool);
		if (rc != -ENOSYS)
			CERROR("Failed to create FMR pool: %d\n", rc);
		else
			CERROR("FMRs are not supported\n");
	}

	return rc;
}

static int kiblnd_alloc_freg_pool(struct kib_fmr_poolset *fps, struct kib_fmr_pool *fpo)
{
	struct kib_fast_reg_descriptor *frd, *tmp;
	int i, rc;

	INIT_LIST_HEAD(&fpo->fast_reg.fpo_pool_list);
	fpo->fast_reg.fpo_pool_size = 0;
	for (i = 0; i < fps->fps_pool_size; i++) {
		LIBCFS_CPT_ALLOC(frd, lnet_cpt_table(), fps->fps_cpt,
				 sizeof(*frd));
		if (!frd) {
			CERROR("Failed to allocate a new fast_reg descriptor\n");
			rc = -ENOMEM;
			goto out;
		}

		frd->frd_mr = ib_alloc_mr(fpo->fpo_hdev->ibh_pd,
					  IB_MR_TYPE_MEM_REG,
					  LNET_MAX_PAYLOAD / PAGE_SIZE);
		if (IS_ERR(frd->frd_mr)) {
			rc = PTR_ERR(frd->frd_mr);
			CERROR("Failed to allocate ib_alloc_mr: %d\n", rc);
			frd->frd_mr = NULL;
			goto out_middle;
		}

		frd->frd_valid = true;

		list_add_tail(&frd->frd_list, &fpo->fast_reg.fpo_pool_list);
		fpo->fast_reg.fpo_pool_size++;
	}

	return 0;

out_middle:
	if (frd->frd_mr)
		ib_dereg_mr(frd->frd_mr);
	LIBCFS_FREE(frd, sizeof(*frd));

out:
	list_for_each_entry_safe(frd, tmp, &fpo->fast_reg.fpo_pool_list,
				 frd_list) {
		list_del(&frd->frd_list);
		ib_dereg_mr(frd->frd_mr);
		LIBCFS_FREE(frd, sizeof(*frd));
	}

	return rc;
}

static int kiblnd_create_fmr_pool(struct kib_fmr_poolset *fps,
				  struct kib_fmr_pool **pp_fpo)
{
	struct kib_dev *dev = fps->fps_net->ibn_dev;
	struct ib_device_attr *dev_attr;
	struct kib_fmr_pool *fpo;
	int rc;

	LIBCFS_CPT_ALLOC(fpo, lnet_cpt_table(), fps->fps_cpt, sizeof(*fpo));
	if (!fpo)
		return -ENOMEM;

	fpo->fpo_hdev = kiblnd_current_hdev(dev);
	dev_attr = &fpo->fpo_hdev->ibh_ibdev->attrs;

	/* Check for FMR or FastReg support */
	fpo->fpo_is_fmr = 0;
	if (fpo->fpo_hdev->ibh_ibdev->alloc_fmr &&
	    fpo->fpo_hdev->ibh_ibdev->dealloc_fmr &&
	    fpo->fpo_hdev->ibh_ibdev->map_phys_fmr &&
	    fpo->fpo_hdev->ibh_ibdev->unmap_fmr) {
		LCONSOLE_INFO("Using FMR for registration\n");
		fpo->fpo_is_fmr = 1;
	} else if (dev_attr->device_cap_flags & IB_DEVICE_MEM_MGT_EXTENSIONS) {
		LCONSOLE_INFO("Using FastReg for registration\n");
	} else {
		rc = -ENOSYS;
		LCONSOLE_ERROR_MSG(rc, "IB device does not support FMRs nor FastRegs, can't register memory\n");
		goto out_fpo;
	}

	if (fpo->fpo_is_fmr)
		rc = kiblnd_alloc_fmr_pool(fps, fpo);
	else
		rc = kiblnd_alloc_freg_pool(fps, fpo);
	if (rc)
		goto out_fpo;

	fpo->fpo_deadline = cfs_time_shift(IBLND_POOL_DEADLINE);
	fpo->fpo_owner = fps;
	*pp_fpo = fpo;

	return 0;

out_fpo:
	kiblnd_hdev_decref(fpo->fpo_hdev);
	LIBCFS_FREE(fpo, sizeof(*fpo));
	return rc;
}

static void kiblnd_fail_fmr_poolset(struct kib_fmr_poolset *fps,
				    struct list_head *zombies)
{
	if (!fps->fps_net) /* intialized? */
>>>>>>> v4.9.227
		return;

	spin_lock(&fps->fps_lock);

	while (!list_empty(&fps->fps_pool_list)) {
<<<<<<< HEAD
		kib_fmr_pool_t *fpo = list_entry(fps->fps_pool_list.next,
						 kib_fmr_pool_t, fpo_list);
		fpo->fpo_failed = 1;
		list_del(&fpo->fpo_list);
		if (fpo->fpo_map_count == 0)
=======
		struct kib_fmr_pool *fpo = list_entry(fps->fps_pool_list.next,
						 struct kib_fmr_pool, fpo_list);
		fpo->fpo_failed = 1;
		list_del(&fpo->fpo_list);
		if (!fpo->fpo_map_count)
>>>>>>> v4.9.227
			list_add(&fpo->fpo_list, zombies);
		else
			list_add(&fpo->fpo_list, &fps->fps_failed_pool_list);
	}

	spin_unlock(&fps->fps_lock);
}

<<<<<<< HEAD
static void
kiblnd_fini_fmr_poolset(kib_fmr_poolset_t *fps)
{
	if (fps->fps_net != NULL) { /* initialized? */
=======
static void kiblnd_fini_fmr_poolset(struct kib_fmr_poolset *fps)
{
	if (fps->fps_net) { /* initialized? */
>>>>>>> v4.9.227
		kiblnd_destroy_fmr_pool_list(&fps->fps_failed_pool_list);
		kiblnd_destroy_fmr_pool_list(&fps->fps_pool_list);
	}
}

static int
<<<<<<< HEAD
kiblnd_init_fmr_poolset(kib_fmr_poolset_t *fps, int cpt, kib_net_t *net,
			int pool_size, int flush_trigger)
{
	kib_fmr_pool_t *fpo;
	int	     rc;

	memset(fps, 0, sizeof(kib_fmr_poolset_t));

	fps->fps_net = net;
	fps->fps_cpt = cpt;
	fps->fps_pool_size = pool_size;
	fps->fps_flush_trigger = flush_trigger;
=======
kiblnd_init_fmr_poolset(struct kib_fmr_poolset *fps, int cpt, int ncpts,
			struct kib_net *net,
			struct lnet_ioctl_config_o2iblnd_tunables *tunables)
{
	struct kib_fmr_pool *fpo;
	int rc;

	memset(fps, 0, sizeof(*fps));

	fps->fps_net = net;
	fps->fps_cpt = cpt;

	fps->fps_pool_size = kiblnd_fmr_pool_size(tunables, ncpts);
	fps->fps_flush_trigger = kiblnd_fmr_flush_trigger(tunables, ncpts);
	fps->fps_cache = tunables->lnd_fmr_cache;

>>>>>>> v4.9.227
	spin_lock_init(&fps->fps_lock);
	INIT_LIST_HEAD(&fps->fps_pool_list);
	INIT_LIST_HEAD(&fps->fps_failed_pool_list);

	rc = kiblnd_create_fmr_pool(fps, &fpo);
<<<<<<< HEAD
	if (rc == 0)
=======
	if (!rc)
>>>>>>> v4.9.227
		list_add_tail(&fpo->fpo_list, &fps->fps_pool_list);

	return rc;
}

<<<<<<< HEAD
static int
kiblnd_fmr_pool_is_idle(kib_fmr_pool_t *fpo, unsigned long now)
{
	if (fpo->fpo_map_count != 0) /* still in use */
=======
static int kiblnd_fmr_pool_is_idle(struct kib_fmr_pool *fpo, unsigned long now)
{
	if (fpo->fpo_map_count) /* still in use */
>>>>>>> v4.9.227
		return 0;
	if (fpo->fpo_failed)
		return 1;
	return cfs_time_aftereq(now, fpo->fpo_deadline);
}

<<<<<<< HEAD
void
kiblnd_fmr_pool_unmap(kib_fmr_t *fmr, int status)
{
	LIST_HEAD     (zombies);
	kib_fmr_pool_t    *fpo = fmr->fmr_pool;
	kib_fmr_poolset_t *fps = fpo->fpo_owner;
	unsigned long	 now = cfs_time_current();
	kib_fmr_pool_t    *tmp;
	int		rc;

	rc = ib_fmr_pool_unmap(fmr->fmr_pfmr);
	LASSERT (rc == 0);

	if (status != 0) {
		rc = ib_flush_fmr_pool(fpo->fpo_fmr_pool);
		LASSERT (rc == 0);
	}

	fmr->fmr_pool = NULL;
	fmr->fmr_pfmr = NULL;

	spin_lock(&fps->fps_lock);
	fpo->fpo_map_count --;  /* decref the pool */
=======
static int
kiblnd_map_tx_pages(struct kib_tx *tx, struct kib_rdma_desc *rd)
{
	__u64 *pages = tx->tx_pages;
	struct kib_hca_dev *hdev;
	int npages;
	int size;
	int i;

	hdev = tx->tx_pool->tpo_hdev;

	for (i = 0, npages = 0; i < rd->rd_nfrags; i++) {
		for (size = 0; size <  rd->rd_frags[i].rf_nob;
		     size += hdev->ibh_page_size) {
			pages[npages++] = (rd->rd_frags[i].rf_addr &
					   hdev->ibh_page_mask) + size;
		}
	}

	return npages;
}

void kiblnd_fmr_pool_unmap(struct kib_fmr *fmr, int status)
{
	LIST_HEAD(zombies);
	struct kib_fmr_pool *fpo = fmr->fmr_pool;
	struct kib_fmr_poolset *fps;
	unsigned long now = cfs_time_current();
	struct kib_fmr_pool *tmp;
	int rc;

	if (!fpo)
		return;

	fps = fpo->fpo_owner;
	if (fpo->fpo_is_fmr) {
		if (fmr->fmr_pfmr) {
			rc = ib_fmr_pool_unmap(fmr->fmr_pfmr);
			LASSERT(!rc);
			fmr->fmr_pfmr = NULL;
		}

		if (status) {
			rc = ib_flush_fmr_pool(fpo->fmr.fpo_fmr_pool);
			LASSERT(!rc);
		}
	} else {
		struct kib_fast_reg_descriptor *frd = fmr->fmr_frd;

		if (frd) {
			frd->frd_valid = false;
			spin_lock(&fps->fps_lock);
			list_add_tail(&frd->frd_list, &fpo->fast_reg.fpo_pool_list);
			spin_unlock(&fps->fps_lock);
			fmr->fmr_frd = NULL;
		}
	}
	fmr->fmr_pool = NULL;

	spin_lock(&fps->fps_lock);
	fpo->fpo_map_count--;  /* decref the pool */
>>>>>>> v4.9.227

	list_for_each_entry_safe(fpo, tmp, &fps->fps_pool_list, fpo_list) {
		/* the first pool is persistent */
		if (fps->fps_pool_list.next == &fpo->fpo_list)
			continue;

		if (kiblnd_fmr_pool_is_idle(fpo, now)) {
			list_move(&fpo->fpo_list, &zombies);
<<<<<<< HEAD
			fps->fps_version ++;
=======
			fps->fps_version++;
>>>>>>> v4.9.227
		}
	}
	spin_unlock(&fps->fps_lock);

	if (!list_empty(&zombies))
		kiblnd_destroy_fmr_pool_list(&zombies);
}

<<<<<<< HEAD
int
kiblnd_fmr_pool_map(kib_fmr_poolset_t *fps, __u64 *pages, int npages,
		    __u64 iov, kib_fmr_t *fmr)
{
	struct ib_pool_fmr *pfmr;
	kib_fmr_pool_t     *fpo;
	__u64	       version;
	int		 rc;
=======
int kiblnd_fmr_pool_map(struct kib_fmr_poolset *fps, struct kib_tx *tx,
			struct kib_rdma_desc *rd, __u32 nob, __u64 iov,
			struct kib_fmr *fmr)
{
	__u64 *pages = tx->tx_pages;
	bool is_rx = (rd != tx->tx_rd);
        bool tx_pages_mapped = 0;
	struct kib_fmr_pool *fpo;
	int npages = 0;
	__u64 version;
	int rc;
>>>>>>> v4.9.227

 again:
	spin_lock(&fps->fps_lock);
	version = fps->fps_version;
	list_for_each_entry(fpo, &fps->fps_pool_list, fpo_list) {
		fpo->fpo_deadline = cfs_time_shift(IBLND_POOL_DEADLINE);
		fpo->fpo_map_count++;
<<<<<<< HEAD
		spin_unlock(&fps->fps_lock);

		pfmr = ib_fmr_pool_map_phys(fpo->fpo_fmr_pool,
					    pages, npages, iov);
		if (likely(!IS_ERR(pfmr))) {
			fmr->fmr_pool = fpo;
			fmr->fmr_pfmr = pfmr;
			return 0;
=======

		if (fpo->fpo_is_fmr) {
			struct ib_pool_fmr *pfmr;

			spin_unlock(&fps->fps_lock);

			if (!tx_pages_mapped) {
				npages = kiblnd_map_tx_pages(tx, rd);
				tx_pages_mapped = 1;
			}

			pfmr = ib_fmr_pool_map_phys(fpo->fmr.fpo_fmr_pool,
						    pages, npages, iov);
			if (likely(!IS_ERR(pfmr))) {
				fmr->fmr_key = is_rx ? pfmr->fmr->rkey :
						       pfmr->fmr->lkey;
				fmr->fmr_frd = NULL;
				fmr->fmr_pfmr = pfmr;
				fmr->fmr_pool = fpo;
				return 0;
			}
			rc = PTR_ERR(pfmr);
		} else {
			if (!list_empty(&fpo->fast_reg.fpo_pool_list)) {
				struct kib_fast_reg_descriptor *frd;
				struct ib_reg_wr *wr;
				struct ib_mr *mr;
				int n;

				frd = list_first_entry(&fpo->fast_reg.fpo_pool_list,
						       struct kib_fast_reg_descriptor,
						       frd_list);
				list_del(&frd->frd_list);
				spin_unlock(&fps->fps_lock);

				mr = frd->frd_mr;

				if (!frd->frd_valid) {
					__u32 key = is_rx ? mr->rkey : mr->lkey;
					struct ib_send_wr *inv_wr;

					inv_wr = &frd->frd_inv_wr;
					memset(inv_wr, 0, sizeof(*inv_wr));
					inv_wr->opcode = IB_WR_LOCAL_INV;
					inv_wr->wr_id = IBLND_WID_MR;
					inv_wr->ex.invalidate_rkey = key;

					/* Bump the key */
					key = ib_inc_rkey(key);
					ib_update_fast_reg_key(mr, key);
				}

				n = ib_map_mr_sg(mr, tx->tx_frags,
						 tx->tx_nfrags, NULL, PAGE_SIZE);
				if (unlikely(n != tx->tx_nfrags)) {
					CERROR("Failed to map mr %d/%d elements\n",
					       n, tx->tx_nfrags);
					return n < 0 ? n : -EINVAL;
				}

				mr->iova = iov;

				/* Prepare FastReg WR */
				wr = &frd->frd_fastreg_wr;
				memset(wr, 0, sizeof(*wr));
				wr->wr.opcode = IB_WR_REG_MR;
				wr->wr.wr_id = IBLND_WID_MR;
				wr->wr.num_sge = 0;
				wr->wr.send_flags = 0;
				wr->mr = mr;
				wr->key = is_rx ? mr->rkey : mr->lkey;
				wr->access = (IB_ACCESS_LOCAL_WRITE |
					      IB_ACCESS_REMOTE_WRITE);

				fmr->fmr_key = is_rx ? mr->rkey : mr->lkey;
				fmr->fmr_frd = frd;
				fmr->fmr_pfmr = NULL;
				fmr->fmr_pool = fpo;
				return 0;
			}
			spin_unlock(&fps->fps_lock);
			rc = -EBUSY;
>>>>>>> v4.9.227
		}

		spin_lock(&fps->fps_lock);
		fpo->fpo_map_count--;
<<<<<<< HEAD
		if (PTR_ERR(pfmr) != -EAGAIN) {
			spin_unlock(&fps->fps_lock);
			return PTR_ERR(pfmr);
=======
		if (rc != -EAGAIN) {
			spin_unlock(&fps->fps_lock);
			return rc;
>>>>>>> v4.9.227
		}

		/* EAGAIN and ... */
		if (version != fps->fps_version) {
			spin_unlock(&fps->fps_lock);
			goto again;
		}
	}

	if (fps->fps_increasing) {
		spin_unlock(&fps->fps_lock);
<<<<<<< HEAD
		CDEBUG(D_NET, "Another thread is allocating new "
		       "FMR pool, waiting for her to complete\n");
		schedule();
		goto again;

=======
		CDEBUG(D_NET, "Another thread is allocating new FMR pool, waiting for her to complete\n");
		schedule();
		goto again;
>>>>>>> v4.9.227
	}

	if (time_before(cfs_time_current(), fps->fps_next_retry)) {
		/* someone failed recently */
		spin_unlock(&fps->fps_lock);
		return -EAGAIN;
	}

	fps->fps_increasing = 1;
	spin_unlock(&fps->fps_lock);

	CDEBUG(D_NET, "Allocate new FMR pool\n");
	rc = kiblnd_create_fmr_pool(fps, &fpo);
	spin_lock(&fps->fps_lock);
	fps->fps_increasing = 0;
<<<<<<< HEAD
	if (rc == 0) {
=======
	if (!rc) {
>>>>>>> v4.9.227
		fps->fps_version++;
		list_add_tail(&fpo->fpo_list, &fps->fps_pool_list);
	} else {
		fps->fps_next_retry = cfs_time_shift(IBLND_POOL_RETRY);
	}
	spin_unlock(&fps->fps_lock);

	goto again;
}

<<<<<<< HEAD
static void
kiblnd_fini_pool(kib_pool_t *pool)
{
	LASSERT (list_empty(&pool->po_free_list));
	LASSERT (pool->po_allocated == 0);
=======
static void kiblnd_fini_pool(struct kib_pool *pool)
{
	LASSERT(list_empty(&pool->po_free_list));
	LASSERT(!pool->po_allocated);
>>>>>>> v4.9.227

	CDEBUG(D_NET, "Finalize %s pool\n", pool->po_owner->ps_name);
}

<<<<<<< HEAD
static void
kiblnd_init_pool(kib_poolset_t *ps, kib_pool_t *pool, int size)
{
	CDEBUG(D_NET, "Initialize %s pool\n", ps->ps_name);

	memset(pool, 0, sizeof(kib_pool_t));
=======
static void kiblnd_init_pool(struct kib_poolset *ps, struct kib_pool *pool, int size)
{
	CDEBUG(D_NET, "Initialize %s pool\n", ps->ps_name);

	memset(pool, 0, sizeof(*pool));
>>>>>>> v4.9.227
	INIT_LIST_HEAD(&pool->po_free_list);
	pool->po_deadline = cfs_time_shift(IBLND_POOL_DEADLINE);
	pool->po_owner    = ps;
	pool->po_size     = size;
}

<<<<<<< HEAD
static void
kiblnd_destroy_pool_list(struct list_head *head)
{
	kib_pool_t *pool;

	while (!list_empty(head)) {
		pool = list_entry(head->next, kib_pool_t, po_list);
		list_del(&pool->po_list);

		LASSERT (pool->po_owner != NULL);
=======
static void kiblnd_destroy_pool_list(struct list_head *head)
{
	struct kib_pool *pool;

	while (!list_empty(head)) {
		pool = list_entry(head->next, struct kib_pool, po_list);
		list_del(&pool->po_list);

		LASSERT(pool->po_owner);
>>>>>>> v4.9.227
		pool->po_owner->ps_pool_destroy(pool);
	}
}

<<<<<<< HEAD
static void
kiblnd_fail_poolset(kib_poolset_t *ps, struct list_head *zombies)
{
	if (ps->ps_net == NULL) /* intialized? */
=======
static void kiblnd_fail_poolset(struct kib_poolset *ps, struct list_head *zombies)
{
	if (!ps->ps_net) /* intialized? */
>>>>>>> v4.9.227
		return;

	spin_lock(&ps->ps_lock);
	while (!list_empty(&ps->ps_pool_list)) {
<<<<<<< HEAD
		kib_pool_t *po = list_entry(ps->ps_pool_list.next,
					    kib_pool_t, po_list);
		po->po_failed = 1;
		list_del(&po->po_list);
		if (po->po_allocated == 0)
=======
		struct kib_pool *po = list_entry(ps->ps_pool_list.next,
					    struct kib_pool, po_list);
		po->po_failed = 1;
		list_del(&po->po_list);
		if (!po->po_allocated)
>>>>>>> v4.9.227
			list_add(&po->po_list, zombies);
		else
			list_add(&po->po_list, &ps->ps_failed_pool_list);
	}
	spin_unlock(&ps->ps_lock);
}

<<<<<<< HEAD
static void
kiblnd_fini_poolset(kib_poolset_t *ps)
{
	if (ps->ps_net != NULL) { /* initialized? */
=======
static void kiblnd_fini_poolset(struct kib_poolset *ps)
{
	if (ps->ps_net) { /* initialized? */
>>>>>>> v4.9.227
		kiblnd_destroy_pool_list(&ps->ps_failed_pool_list);
		kiblnd_destroy_pool_list(&ps->ps_pool_list);
	}
}

<<<<<<< HEAD
static int
kiblnd_init_poolset(kib_poolset_t *ps, int cpt,
		    kib_net_t *net, char *name, int size,
		    kib_ps_pool_create_t po_create,
		    kib_ps_pool_destroy_t po_destroy,
		    kib_ps_node_init_t nd_init,
		    kib_ps_node_fini_t nd_fini)
{
	kib_pool_t	*pool;
	int		rc;

	memset(ps, 0, sizeof(kib_poolset_t));

	ps->ps_cpt	    = cpt;
	ps->ps_net	  = net;
=======
static int kiblnd_init_poolset(struct kib_poolset *ps, int cpt,
			       struct kib_net *net, char *name, int size,
			       kib_ps_pool_create_t po_create,
			       kib_ps_pool_destroy_t po_destroy,
			       kib_ps_node_init_t nd_init,
			       kib_ps_node_fini_t nd_fini)
{
	struct kib_pool *pool;
	int rc;

	memset(ps, 0, sizeof(*ps));

	ps->ps_cpt          = cpt;
	ps->ps_net          = net;
>>>>>>> v4.9.227
	ps->ps_pool_create  = po_create;
	ps->ps_pool_destroy = po_destroy;
	ps->ps_node_init    = nd_init;
	ps->ps_node_fini    = nd_fini;
	ps->ps_pool_size    = size;
	if (strlcpy(ps->ps_name, name, sizeof(ps->ps_name))
	    >= sizeof(ps->ps_name))
		return -E2BIG;
	spin_lock_init(&ps->ps_lock);
	INIT_LIST_HEAD(&ps->ps_pool_list);
	INIT_LIST_HEAD(&ps->ps_failed_pool_list);

	rc = ps->ps_pool_create(ps, size, &pool);
<<<<<<< HEAD
	if (rc == 0)
=======
	if (!rc)
>>>>>>> v4.9.227
		list_add(&pool->po_list, &ps->ps_pool_list);
	else
		CERROR("Failed to create the first pool for %s\n", ps->ps_name);

	return rc;
}

<<<<<<< HEAD
static int
kiblnd_pool_is_idle(kib_pool_t *pool, unsigned long now)
{
	if (pool->po_allocated != 0) /* still in use */
=======
static int kiblnd_pool_is_idle(struct kib_pool *pool, unsigned long now)
{
	if (pool->po_allocated) /* still in use */
>>>>>>> v4.9.227
		return 0;
	if (pool->po_failed)
		return 1;
	return cfs_time_aftereq(now, pool->po_deadline);
}

<<<<<<< HEAD
void
kiblnd_pool_free_node(kib_pool_t *pool, struct list_head *node)
{
	LIST_HEAD  (zombies);
	kib_poolset_t  *ps = pool->po_owner;
	kib_pool_t     *tmp;
	unsigned long      now = cfs_time_current();

	spin_lock(&ps->ps_lock);

	if (ps->ps_node_fini != NULL)
		ps->ps_node_fini(pool, node);

	LASSERT (pool->po_allocated > 0);
	list_add(node, &pool->po_free_list);
	pool->po_allocated --;
=======
void kiblnd_pool_free_node(struct kib_pool *pool, struct list_head *node)
{
	LIST_HEAD(zombies);
	struct kib_poolset *ps = pool->po_owner;
	struct kib_pool *tmp;
	unsigned long now = cfs_time_current();

	spin_lock(&ps->ps_lock);

	if (ps->ps_node_fini)
		ps->ps_node_fini(pool, node);

	LASSERT(pool->po_allocated > 0);
	list_add(node, &pool->po_free_list);
	pool->po_allocated--;
>>>>>>> v4.9.227

	list_for_each_entry_safe(pool, tmp, &ps->ps_pool_list, po_list) {
		/* the first pool is persistent */
		if (ps->ps_pool_list.next == &pool->po_list)
			continue;

		if (kiblnd_pool_is_idle(pool, now))
			list_move(&pool->po_list, &zombies);
	}
	spin_unlock(&ps->ps_lock);

	if (!list_empty(&zombies))
		kiblnd_destroy_pool_list(&zombies);
}

<<<<<<< HEAD
struct list_head *
kiblnd_pool_alloc_node(kib_poolset_t *ps)
{
	struct list_head	    *node;
	kib_pool_t	    *pool;
	int		    rc;
=======
struct list_head *kiblnd_pool_alloc_node(struct kib_poolset *ps)
{
	struct list_head *node;
	struct kib_pool *pool;
	unsigned int interval = 1;
	unsigned long time_before;
	unsigned int trips = 0;
	int rc;
>>>>>>> v4.9.227

 again:
	spin_lock(&ps->ps_lock);
	list_for_each_entry(pool, &ps->ps_pool_list, po_list) {
		if (list_empty(&pool->po_free_list))
			continue;

<<<<<<< HEAD
		pool->po_allocated ++;
=======
		pool->po_allocated++;
>>>>>>> v4.9.227
		pool->po_deadline = cfs_time_shift(IBLND_POOL_DEADLINE);
		node = pool->po_free_list.next;
		list_del(node);

<<<<<<< HEAD
		if (ps->ps_node_init != NULL) {
=======
		if (ps->ps_node_init) {
>>>>>>> v4.9.227
			/* still hold the lock */
			ps->ps_node_init(pool, node);
		}
		spin_unlock(&ps->ps_lock);
		return node;
	}

	/* no available tx pool and ... */
	if (ps->ps_increasing) {
		/* another thread is allocating a new pool */
		spin_unlock(&ps->ps_lock);
<<<<<<< HEAD
		CDEBUG(D_NET, "Another thread is allocating new "
		       "%s pool, waiting for her to complete\n",
		       ps->ps_name);
		schedule();
=======
		trips++;
		CDEBUG(D_NET, "Another thread is allocating new %s pool, waiting %d HZs for her to complete. trips = %d\n",
		       ps->ps_name, interval, trips);

		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(interval);
		if (interval < cfs_time_seconds(1))
			interval *= 2;

>>>>>>> v4.9.227
		goto again;
	}

	if (time_before(cfs_time_current(), ps->ps_next_retry)) {
		/* someone failed recently */
		spin_unlock(&ps->ps_lock);
		return NULL;
	}

	ps->ps_increasing = 1;
	spin_unlock(&ps->ps_lock);

	CDEBUG(D_NET, "%s pool exhausted, allocate new pool\n", ps->ps_name);
<<<<<<< HEAD

	rc = ps->ps_pool_create(ps, ps->ps_pool_size, &pool);

	spin_lock(&ps->ps_lock);
	ps->ps_increasing = 0;
	if (rc == 0) {
=======
	time_before = cfs_time_current();
	rc = ps->ps_pool_create(ps, ps->ps_pool_size, &pool);
	CDEBUG(D_NET, "ps_pool_create took %lu HZ to complete",
	       cfs_time_current() - time_before);

	spin_lock(&ps->ps_lock);
	ps->ps_increasing = 0;
	if (!rc) {
>>>>>>> v4.9.227
		list_add_tail(&pool->po_list, &ps->ps_pool_list);
	} else {
		ps->ps_next_retry = cfs_time_shift(IBLND_POOL_RETRY);
		CERROR("Can't allocate new %s pool because out of memory\n",
		       ps->ps_name);
	}
	spin_unlock(&ps->ps_lock);

	goto again;
}

<<<<<<< HEAD
void
kiblnd_pmr_pool_unmap(kib_phys_mr_t *pmr)
{
	kib_pmr_pool_t      *ppo = pmr->pmr_pool;
	struct ib_mr	*mr  = pmr->pmr_mr;

	pmr->pmr_mr = NULL;
	kiblnd_pool_free_node(&ppo->ppo_pool, &pmr->pmr_list);
	if (mr != NULL)
		ib_dereg_mr(mr);
}

int
kiblnd_pmr_pool_map(kib_pmr_poolset_t *pps, kib_hca_dev_t *hdev,
		    kib_rdma_desc_t *rd, __u64 *iova, kib_phys_mr_t **pp_pmr)
{
	kib_phys_mr_t *pmr;
	struct list_head    *node;
	int	    rc;
	int	    i;

	node = kiblnd_pool_alloc_node(&pps->pps_poolset);
	if (node == NULL) {
		CERROR("Failed to allocate PMR descriptor\n");
		return -ENOMEM;
	}

	pmr = container_of(node, kib_phys_mr_t, pmr_list);
	if (pmr->pmr_pool->ppo_hdev != hdev) {
		kiblnd_pool_free_node(&pmr->pmr_pool->ppo_pool, node);
		return -EAGAIN;
	}

	for (i = 0; i < rd->rd_nfrags; i ++) {
		pmr->pmr_ipb[i].addr = rd->rd_frags[i].rf_addr;
		pmr->pmr_ipb[i].size = rd->rd_frags[i].rf_nob;
	}

	pmr->pmr_mr = ib_reg_phys_mr(hdev->ibh_pd,
				     pmr->pmr_ipb, rd->rd_nfrags,
				     IB_ACCESS_LOCAL_WRITE |
				     IB_ACCESS_REMOTE_WRITE,
				     iova);
	if (!IS_ERR(pmr->pmr_mr)) {
		pmr->pmr_iova = *iova;
		*pp_pmr = pmr;
		return 0;
	}

	rc = PTR_ERR(pmr->pmr_mr);
	CERROR("Failed ib_reg_phys_mr: %d\n", rc);

	pmr->pmr_mr = NULL;
	kiblnd_pool_free_node(&pmr->pmr_pool->ppo_pool, node);

	return rc;
}

static void
kiblnd_destroy_pmr_pool(kib_pool_t *pool)
{
	kib_pmr_pool_t *ppo = container_of(pool, kib_pmr_pool_t, ppo_pool);
	kib_phys_mr_t  *pmr;

	LASSERT (pool->po_allocated == 0);

	while (!list_empty(&pool->po_free_list)) {
		pmr = list_entry(pool->po_free_list.next,
				     kib_phys_mr_t, pmr_list);

		LASSERT (pmr->pmr_mr == NULL);
		list_del(&pmr->pmr_list);

		if (pmr->pmr_ipb != NULL) {
			LIBCFS_FREE(pmr->pmr_ipb,
				    IBLND_MAX_RDMA_FRAGS *
				    sizeof(struct ib_phys_buf));
		}

		LIBCFS_FREE(pmr, sizeof(kib_phys_mr_t));
	}

	kiblnd_fini_pool(pool);
	if (ppo->ppo_hdev != NULL)
		kiblnd_hdev_decref(ppo->ppo_hdev);

	LIBCFS_FREE(ppo, sizeof(kib_pmr_pool_t));
}

static inline int kiblnd_pmr_pool_size(int ncpts)
{
	int size = *kiblnd_tunables.kib_pmr_pool_size / ncpts;

	return max(IBLND_PMR_POOL, size);
}

static int
kiblnd_create_pmr_pool(kib_poolset_t *ps, int size, kib_pool_t **pp_po)
{
	struct kib_pmr_pool	*ppo;
	struct kib_pool		*pool;
	kib_phys_mr_t		*pmr;
	int			i;

	LIBCFS_CPT_ALLOC(ppo, lnet_cpt_table(),
			 ps->ps_cpt, sizeof(kib_pmr_pool_t));
	if (ppo == NULL) {
		CERROR("Failed to allocate PMR pool\n");
		return -ENOMEM;
	}

	pool = &ppo->ppo_pool;
	kiblnd_init_pool(ps, pool, size);

	for (i = 0; i < size; i++) {
		LIBCFS_CPT_ALLOC(pmr, lnet_cpt_table(),
				 ps->ps_cpt, sizeof(kib_phys_mr_t));
		if (pmr == NULL)
			break;

		pmr->pmr_pool = ppo;
		LIBCFS_CPT_ALLOC(pmr->pmr_ipb, lnet_cpt_table(), ps->ps_cpt,
				 IBLND_MAX_RDMA_FRAGS * sizeof(*pmr->pmr_ipb));
		if (pmr->pmr_ipb == NULL)
			break;

		list_add(&pmr->pmr_list, &pool->po_free_list);
	}

	if (i < size) {
		ps->ps_pool_destroy(pool);
		return -ENOMEM;
	}

	ppo->ppo_hdev = kiblnd_current_hdev(ps->ps_net->ibn_dev);
	*pp_po = pool;
	return 0;
}

static void
kiblnd_destroy_tx_pool(kib_pool_t *pool)
{
	kib_tx_pool_t  *tpo = container_of(pool, kib_tx_pool_t, tpo_pool);
	int	     i;

	LASSERT (pool->po_allocated == 0);

	if (tpo->tpo_tx_pages != NULL) {
=======
static void kiblnd_destroy_tx_pool(struct kib_pool *pool)
{
	struct kib_tx_pool *tpo = container_of(pool, struct kib_tx_pool, tpo_pool);
	int i;

	LASSERT(!pool->po_allocated);

	if (tpo->tpo_tx_pages) {
>>>>>>> v4.9.227
		kiblnd_unmap_tx_pool(tpo);
		kiblnd_free_pages(tpo->tpo_tx_pages);
	}

<<<<<<< HEAD
	if (tpo->tpo_tx_descs == NULL)
		goto out;

	for (i = 0; i < pool->po_size; i++) {
		kib_tx_t *tx = &tpo->tpo_tx_descs[i];

		list_del(&tx->tx_list);
		if (tx->tx_pages != NULL)
			LIBCFS_FREE(tx->tx_pages,
				    LNET_MAX_IOV *
				    sizeof(*tx->tx_pages));
		if (tx->tx_frags != NULL)
			LIBCFS_FREE(tx->tx_frags,
				    IBLND_MAX_RDMA_FRAGS *
					    sizeof(*tx->tx_frags));
		if (tx->tx_wrq != NULL)
			LIBCFS_FREE(tx->tx_wrq,
				    (1 + IBLND_MAX_RDMA_FRAGS) *
				    sizeof(*tx->tx_wrq));
		if (tx->tx_sge != NULL)
			LIBCFS_FREE(tx->tx_sge,
				    (1 + IBLND_MAX_RDMA_FRAGS) *
				    sizeof(*tx->tx_sge));
		if (tx->tx_rd != NULL)
			LIBCFS_FREE(tx->tx_rd,
				    offsetof(kib_rdma_desc_t,
=======
	if (!tpo->tpo_tx_descs)
		goto out;

	for (i = 0; i < pool->po_size; i++) {
		struct kib_tx *tx = &tpo->tpo_tx_descs[i];

		list_del(&tx->tx_list);
		if (tx->tx_pages)
			LIBCFS_FREE(tx->tx_pages,
				    LNET_MAX_IOV *
				    sizeof(*tx->tx_pages));
		if (tx->tx_frags)
			LIBCFS_FREE(tx->tx_frags,
				    (1 + IBLND_MAX_RDMA_FRAGS) *
				     sizeof(*tx->tx_frags));
		if (tx->tx_wrq)
			LIBCFS_FREE(tx->tx_wrq,
				    (1 + IBLND_MAX_RDMA_FRAGS) *
				    sizeof(*tx->tx_wrq));
		if (tx->tx_sge)
			LIBCFS_FREE(tx->tx_sge,
				    (1 + IBLND_MAX_RDMA_FRAGS) *
				    sizeof(*tx->tx_sge));
		if (tx->tx_rd)
			LIBCFS_FREE(tx->tx_rd,
				    offsetof(struct kib_rdma_desc,
>>>>>>> v4.9.227
					     rd_frags[IBLND_MAX_RDMA_FRAGS]));
	}

	LIBCFS_FREE(tpo->tpo_tx_descs,
<<<<<<< HEAD
		    pool->po_size * sizeof(kib_tx_t));
out:
	kiblnd_fini_pool(pool);
	LIBCFS_FREE(tpo, sizeof(kib_tx_pool_t));
=======
		    pool->po_size * sizeof(struct kib_tx));
out:
	kiblnd_fini_pool(pool);
	LIBCFS_FREE(tpo, sizeof(*tpo));
>>>>>>> v4.9.227
}

static int kiblnd_tx_pool_size(int ncpts)
{
	int ntx = *kiblnd_tunables.kib_ntx / ncpts;

	return max(IBLND_TX_POOL, ntx);
}

<<<<<<< HEAD
static int
kiblnd_create_tx_pool(kib_poolset_t *ps, int size, kib_pool_t **pp_po)
{
	int	    i;
	int	    npg;
	kib_pool_t    *pool;
	kib_tx_pool_t *tpo;

	LIBCFS_CPT_ALLOC(tpo, lnet_cpt_table(), ps->ps_cpt, sizeof(*tpo));
	if (tpo == NULL) {
=======
static int kiblnd_create_tx_pool(struct kib_poolset *ps, int size,
				 struct kib_pool **pp_po)
{
	int i;
	int npg;
	struct kib_pool *pool;
	struct kib_tx_pool *tpo;

	LIBCFS_CPT_ALLOC(tpo, lnet_cpt_table(), ps->ps_cpt, sizeof(*tpo));
	if (!tpo) {
>>>>>>> v4.9.227
		CERROR("Failed to allocate TX pool\n");
		return -ENOMEM;
	}

	pool = &tpo->tpo_pool;
	kiblnd_init_pool(ps, pool, size);
	tpo->tpo_tx_descs = NULL;
	tpo->tpo_tx_pages = NULL;

	npg = (size * IBLND_MSG_SIZE + PAGE_SIZE - 1) / PAGE_SIZE;
<<<<<<< HEAD
	if (kiblnd_alloc_pages(&tpo->tpo_tx_pages, ps->ps_cpt, npg) != 0) {
		CERROR("Can't allocate tx pages: %d\n", npg);
		LIBCFS_FREE(tpo, sizeof(kib_tx_pool_t));
=======
	if (kiblnd_alloc_pages(&tpo->tpo_tx_pages, ps->ps_cpt, npg)) {
		CERROR("Can't allocate tx pages: %d\n", npg);
		LIBCFS_FREE(tpo, sizeof(*tpo));
>>>>>>> v4.9.227
		return -ENOMEM;
	}

	LIBCFS_CPT_ALLOC(tpo->tpo_tx_descs, lnet_cpt_table(), ps->ps_cpt,
<<<<<<< HEAD
			 size * sizeof(kib_tx_t));
	if (tpo->tpo_tx_descs == NULL) {
=======
			 size * sizeof(struct kib_tx));
	if (!tpo->tpo_tx_descs) {
>>>>>>> v4.9.227
		CERROR("Can't allocate %d tx descriptors\n", size);
		ps->ps_pool_destroy(pool);
		return -ENOMEM;
	}

<<<<<<< HEAD
	memset(tpo->tpo_tx_descs, 0, size * sizeof(kib_tx_t));

	for (i = 0; i < size; i++) {
		kib_tx_t *tx = &tpo->tpo_tx_descs[i];

		tx->tx_pool = tpo;
		if (ps->ps_net->ibn_fmr_ps != NULL) {
			LIBCFS_CPT_ALLOC(tx->tx_pages,
					 lnet_cpt_table(), ps->ps_cpt,
					 LNET_MAX_IOV * sizeof(*tx->tx_pages));
			if (tx->tx_pages == NULL)
=======
	memset(tpo->tpo_tx_descs, 0, size * sizeof(struct kib_tx));

	for (i = 0; i < size; i++) {
		struct kib_tx *tx = &tpo->tpo_tx_descs[i];

		tx->tx_pool = tpo;
		if (ps->ps_net->ibn_fmr_ps) {
			LIBCFS_CPT_ALLOC(tx->tx_pages,
					 lnet_cpt_table(), ps->ps_cpt,
					 LNET_MAX_IOV * sizeof(*tx->tx_pages));
			if (!tx->tx_pages)
>>>>>>> v4.9.227
				break;
		}

		LIBCFS_CPT_ALLOC(tx->tx_frags, lnet_cpt_table(), ps->ps_cpt,
<<<<<<< HEAD
				 IBLND_MAX_RDMA_FRAGS * sizeof(*tx->tx_frags));
		if (tx->tx_frags == NULL)
			break;

		sg_init_table(tx->tx_frags, IBLND_MAX_RDMA_FRAGS);
=======
				 (1 + IBLND_MAX_RDMA_FRAGS) *
				 sizeof(*tx->tx_frags));
		if (!tx->tx_frags)
			break;

		sg_init_table(tx->tx_frags, IBLND_MAX_RDMA_FRAGS + 1);
>>>>>>> v4.9.227

		LIBCFS_CPT_ALLOC(tx->tx_wrq, lnet_cpt_table(), ps->ps_cpt,
				 (1 + IBLND_MAX_RDMA_FRAGS) *
				 sizeof(*tx->tx_wrq));
<<<<<<< HEAD
		if (tx->tx_wrq == NULL)
=======
		if (!tx->tx_wrq)
>>>>>>> v4.9.227
			break;

		LIBCFS_CPT_ALLOC(tx->tx_sge, lnet_cpt_table(), ps->ps_cpt,
				 (1 + IBLND_MAX_RDMA_FRAGS) *
				 sizeof(*tx->tx_sge));
<<<<<<< HEAD
		if (tx->tx_sge == NULL)
			break;

		LIBCFS_CPT_ALLOC(tx->tx_rd, lnet_cpt_table(), ps->ps_cpt,
				 offsetof(kib_rdma_desc_t,
					  rd_frags[IBLND_MAX_RDMA_FRAGS]));
		if (tx->tx_rd == NULL)
=======
		if (!tx->tx_sge)
			break;

		LIBCFS_CPT_ALLOC(tx->tx_rd, lnet_cpt_table(), ps->ps_cpt,
				 offsetof(struct kib_rdma_desc,
					  rd_frags[IBLND_MAX_RDMA_FRAGS]));
		if (!tx->tx_rd)
>>>>>>> v4.9.227
			break;
	}

	if (i == size) {
		kiblnd_map_tx_pool(tpo);
		*pp_po = pool;
		return 0;
	}

	ps->ps_pool_destroy(pool);
	return -ENOMEM;
}

<<<<<<< HEAD
static void
kiblnd_tx_init(kib_pool_t *pool, struct list_head *node)
{
	kib_tx_poolset_t *tps = container_of(pool->po_owner, kib_tx_poolset_t,
					     tps_poolset);
	kib_tx_t	 *tx  = list_entry(node, kib_tx_t, tx_list);

	tx->tx_cookie = tps->tps_next_tx_cookie ++;
}

static void
kiblnd_net_fini_pools(kib_net_t *net)
{
	int	i;

	cfs_cpt_for_each(i, lnet_cpt_table()) {
		kib_tx_poolset_t	*tps;
		kib_fmr_poolset_t	*fps;
		kib_pmr_poolset_t	*pps;

		if (net->ibn_tx_ps != NULL) {
=======
static void kiblnd_tx_init(struct kib_pool *pool, struct list_head *node)
{
	struct kib_tx_poolset *tps = container_of(pool->po_owner,
						  struct kib_tx_poolset,
						  tps_poolset);
	struct kib_tx *tx = list_entry(node, struct kib_tx, tx_list);

	tx->tx_cookie = tps->tps_next_tx_cookie++;
}

static void kiblnd_net_fini_pools(struct kib_net *net)
{
	int i;

	cfs_cpt_for_each(i, lnet_cpt_table()) {
		struct kib_tx_poolset *tps;
		struct kib_fmr_poolset *fps;

		if (net->ibn_tx_ps) {
>>>>>>> v4.9.227
			tps = net->ibn_tx_ps[i];
			kiblnd_fini_poolset(&tps->tps_poolset);
		}

<<<<<<< HEAD
		if (net->ibn_fmr_ps != NULL) {
			fps = net->ibn_fmr_ps[i];
			kiblnd_fini_fmr_poolset(fps);
		}

		if (net->ibn_pmr_ps != NULL) {
			pps = net->ibn_pmr_ps[i];
			kiblnd_fini_poolset(&pps->pps_poolset);
		}
	}

	if (net->ibn_tx_ps != NULL) {
=======
		if (net->ibn_fmr_ps) {
			fps = net->ibn_fmr_ps[i];
			kiblnd_fini_fmr_poolset(fps);
		}
	}

	if (net->ibn_tx_ps) {
>>>>>>> v4.9.227
		cfs_percpt_free(net->ibn_tx_ps);
		net->ibn_tx_ps = NULL;
	}

<<<<<<< HEAD
	if (net->ibn_fmr_ps != NULL) {
		cfs_percpt_free(net->ibn_fmr_ps);
		net->ibn_fmr_ps = NULL;
	}

	if (net->ibn_pmr_ps != NULL) {
		cfs_percpt_free(net->ibn_pmr_ps);
		net->ibn_pmr_ps = NULL;
	}
}

static int
kiblnd_net_init_pools(kib_net_t *net, __u32 *cpts, int ncpts)
{
	unsigned long	flags;
	int		cpt;
	int		rc;
	int		i;

	read_lock_irqsave(&kiblnd_data.kib_global_lock, flags);
	if (*kiblnd_tunables.kib_map_on_demand == 0 &&
	    net->ibn_dev->ibd_hdev->ibh_nmrs == 1) {
		read_unlock_irqrestore(&kiblnd_data.kib_global_lock,
					   flags);
=======
	if (net->ibn_fmr_ps) {
		cfs_percpt_free(net->ibn_fmr_ps);
		net->ibn_fmr_ps = NULL;
	}
}

static int kiblnd_net_init_pools(struct kib_net *net, lnet_ni_t *ni, __u32 *cpts,
				 int ncpts)
{
	struct lnet_ioctl_config_o2iblnd_tunables *tunables;
	unsigned long flags;
	int cpt;
	int rc;
	int i;

	tunables = &ni->ni_lnd_tunables->lt_tun_u.lt_o2ib;

	read_lock_irqsave(&kiblnd_data.kib_global_lock, flags);
	if (!tunables->lnd_map_on_demand) {
		read_unlock_irqrestore(&kiblnd_data.kib_global_lock, flags);
>>>>>>> v4.9.227
		goto create_tx_pool;
	}

	read_unlock_irqrestore(&kiblnd_data.kib_global_lock, flags);

<<<<<<< HEAD
	if (*kiblnd_tunables.kib_fmr_pool_size <
	    *kiblnd_tunables.kib_ntx / 4) {
		CERROR("Can't set fmr pool size (%d) < ntx / 4(%d)\n",
		       *kiblnd_tunables.kib_fmr_pool_size,
=======
	if (tunables->lnd_fmr_pool_size < *kiblnd_tunables.kib_ntx / 4) {
		CERROR("Can't set fmr pool size (%d) < ntx / 4(%d)\n",
		       tunables->lnd_fmr_pool_size,
>>>>>>> v4.9.227
		       *kiblnd_tunables.kib_ntx / 4);
		rc = -EINVAL;
		goto failed;
	}

<<<<<<< HEAD
	/* TX pool must be created later than FMR/PMR, see LU-2268
	 * for details */
	LASSERT(net->ibn_tx_ps == NULL);

	/* premapping can fail if ibd_nmr > 1, so we always create
	 * FMR/PMR pool and map-on-demand if premapping failed */

	net->ibn_fmr_ps = cfs_percpt_alloc(lnet_cpt_table(),
					   sizeof(kib_fmr_poolset_t));
	if (net->ibn_fmr_ps == NULL) {
=======
	/*
	 * TX pool must be created later than FMR, see LU-2268
	 * for details
	 */
	LASSERT(!net->ibn_tx_ps);

	/*
	 * premapping can fail if ibd_nmr > 1, so we always create
	 * FMR pool and map-on-demand if premapping failed
	 *
	 * cfs_precpt_alloc is creating an array of struct kib_fmr_poolset
	 * The number of struct kib_fmr_poolsets create is equal to the
	 * number of CPTs that exist, i.e net->ibn_fmr_ps[cpt].
	 */
	net->ibn_fmr_ps = cfs_percpt_alloc(lnet_cpt_table(),
					   sizeof(struct kib_fmr_poolset));
	if (!net->ibn_fmr_ps) {
>>>>>>> v4.9.227
		CERROR("Failed to allocate FMR pool array\n");
		rc = -ENOMEM;
		goto failed;
	}

	for (i = 0; i < ncpts; i++) {
<<<<<<< HEAD
		cpt = (cpts == NULL) ? i : cpts[i];
		rc = kiblnd_init_fmr_poolset(net->ibn_fmr_ps[cpt], cpt, net,
					     kiblnd_fmr_pool_size(ncpts),
					     kiblnd_fmr_flush_trigger(ncpts));
		if (rc == -ENOSYS && i == 0) /* no FMR */
			break; /* create PMR pool */

		if (rc != 0) { /* a real error */
=======
		cpt = !cpts ? i : cpts[i];
		rc = kiblnd_init_fmr_poolset(net->ibn_fmr_ps[cpt], cpt, ncpts,
					     net, tunables);
		if (rc) {
>>>>>>> v4.9.227
			CERROR("Can't initialize FMR pool for CPT %d: %d\n",
			       cpt, rc);
			goto failed;
		}
	}

<<<<<<< HEAD
	if (i > 0) {
		LASSERT(i == ncpts);
		goto create_tx_pool;
	}

	cfs_percpt_free(net->ibn_fmr_ps);
	net->ibn_fmr_ps = NULL;

	CWARN("Device does not support FMR, failing back to PMR\n");

	if (*kiblnd_tunables.kib_pmr_pool_size <
	    *kiblnd_tunables.kib_ntx / 4) {
		CERROR("Can't set pmr pool size (%d) < ntx / 4(%d)\n",
		       *kiblnd_tunables.kib_pmr_pool_size,
		       *kiblnd_tunables.kib_ntx / 4);
		rc = -EINVAL;
		goto failed;
	}

	net->ibn_pmr_ps = cfs_percpt_alloc(lnet_cpt_table(),
					   sizeof(kib_pmr_poolset_t));
	if (net->ibn_pmr_ps == NULL) {
		CERROR("Failed to allocate PMR pool array\n");
		rc = -ENOMEM;
		goto failed;
	}

	for (i = 0; i < ncpts; i++) {
		cpt = (cpts == NULL) ? i : cpts[i];
		rc = kiblnd_init_poolset(&net->ibn_pmr_ps[cpt]->pps_poolset,
					 cpt, net, "PMR",
					 kiblnd_pmr_pool_size(ncpts),
					 kiblnd_create_pmr_pool,
					 kiblnd_destroy_pmr_pool, NULL, NULL);
		if (rc != 0) {
			CERROR("Can't initialize PMR pool for CPT %d: %d\n",
			       cpt, rc);
			goto failed;
		}
	}

 create_tx_pool:
	net->ibn_tx_ps = cfs_percpt_alloc(lnet_cpt_table(),
					  sizeof(kib_tx_poolset_t));
	if (net->ibn_tx_ps == NULL) {
=======
	if (i > 0)
		LASSERT(i == ncpts);

 create_tx_pool:
	/*
	 * cfs_precpt_alloc is creating an array of struct kib_tx_poolset
	 * The number of struct kib_tx_poolsets create is equal to the
	 * number of CPTs that exist, i.e net->ibn_tx_ps[cpt].
	 */
	net->ibn_tx_ps = cfs_percpt_alloc(lnet_cpt_table(),
					  sizeof(struct kib_tx_poolset));
	if (!net->ibn_tx_ps) {
>>>>>>> v4.9.227
		CERROR("Failed to allocate tx pool array\n");
		rc = -ENOMEM;
		goto failed;
	}

	for (i = 0; i < ncpts; i++) {
<<<<<<< HEAD
		cpt = (cpts == NULL) ? i : cpts[i];
=======
		cpt = !cpts ? i : cpts[i];
>>>>>>> v4.9.227
		rc = kiblnd_init_poolset(&net->ibn_tx_ps[cpt]->tps_poolset,
					 cpt, net, "TX",
					 kiblnd_tx_pool_size(ncpts),
					 kiblnd_create_tx_pool,
					 kiblnd_destroy_tx_pool,
					 kiblnd_tx_init, NULL);
<<<<<<< HEAD
		if (rc != 0) {
=======
		if (rc) {
>>>>>>> v4.9.227
			CERROR("Can't initialize TX pool for CPT %d: %d\n",
			       cpt, rc);
			goto failed;
		}
	}

	return 0;
 failed:
	kiblnd_net_fini_pools(net);
<<<<<<< HEAD
	LASSERT(rc != 0);
	return rc;
}

static int
kiblnd_hdev_get_attr(kib_hca_dev_t *hdev)
{
	struct ib_device_attr *attr;
	int		    rc;

	/* It's safe to assume a HCA can handle a page size
	 * matching that of the native system */
=======
	LASSERT(rc);
	return rc;
}

static int kiblnd_hdev_get_attr(struct kib_hca_dev *hdev)
{
	/*
	 * It's safe to assume a HCA can handle a page size
	 * matching that of the native system
	 */
>>>>>>> v4.9.227
	hdev->ibh_page_shift = PAGE_SHIFT;
	hdev->ibh_page_size  = 1 << PAGE_SHIFT;
	hdev->ibh_page_mask  = ~((__u64)hdev->ibh_page_size - 1);

<<<<<<< HEAD
	LIBCFS_ALLOC(attr, sizeof(*attr));
	if (attr == NULL) {
		CERROR("Out of memory\n");
		return -ENOMEM;
	}

	rc = ib_query_device(hdev->ibh_ibdev, attr);
	if (rc == 0)
		hdev->ibh_mr_size = attr->max_mr_size;

	LIBCFS_FREE(attr, sizeof(*attr));

	if (rc != 0) {
		CERROR("Failed to query IB device: %d\n", rc);
		return rc;
	}

=======
	hdev->ibh_mr_size = hdev->ibh_ibdev->attrs.max_mr_size;
>>>>>>> v4.9.227
	if (hdev->ibh_mr_size == ~0ULL) {
		hdev->ibh_mr_shift = 64;
		return 0;
	}

<<<<<<< HEAD
	for (hdev->ibh_mr_shift = 0;
	     hdev->ibh_mr_shift < 64; hdev->ibh_mr_shift ++) {
		if (hdev->ibh_mr_size == (1ULL << hdev->ibh_mr_shift) ||
		    hdev->ibh_mr_size == (1ULL << hdev->ibh_mr_shift) - 1)
			return 0;
	}

=======
>>>>>>> v4.9.227
	CERROR("Invalid mr size: %#llx\n", hdev->ibh_mr_size);
	return -EINVAL;
}

<<<<<<< HEAD
static void
kiblnd_hdev_cleanup_mrs(kib_hca_dev_t *hdev)
{
	int     i;

	if (hdev->ibh_nmrs == 0 || hdev->ibh_mrs == NULL)
		return;

	for (i = 0; i < hdev->ibh_nmrs; i++) {
		if (hdev->ibh_mrs[i] == NULL)
			break;

		ib_dereg_mr(hdev->ibh_mrs[i]);
	}

	LIBCFS_FREE(hdev->ibh_mrs, sizeof(*hdev->ibh_mrs) * hdev->ibh_nmrs);
	hdev->ibh_mrs  = NULL;
	hdev->ibh_nmrs = 0;
}

void
kiblnd_hdev_destroy(kib_hca_dev_t *hdev)
{
	kiblnd_hdev_cleanup_mrs(hdev);

	if (hdev->ibh_pd != NULL)
		ib_dealloc_pd(hdev->ibh_pd);

	if (hdev->ibh_cmid != NULL)
=======
static void kiblnd_hdev_cleanup_mrs(struct kib_hca_dev *hdev)
{
	if (!hdev->ibh_mrs)
		return;

	ib_dereg_mr(hdev->ibh_mrs);

	hdev->ibh_mrs = NULL;
}

void kiblnd_hdev_destroy(struct kib_hca_dev *hdev)
{
	kiblnd_hdev_cleanup_mrs(hdev);

	if (hdev->ibh_pd)
		ib_dealloc_pd(hdev->ibh_pd);

	if (hdev->ibh_cmid)
>>>>>>> v4.9.227
		rdma_destroy_id(hdev->ibh_cmid);

	LIBCFS_FREE(hdev, sizeof(*hdev));
}

<<<<<<< HEAD
static int
kiblnd_hdev_setup_mrs(kib_hca_dev_t *hdev)
{
	struct ib_mr *mr;
	int	   i;
	int	   rc;
	__u64	 mm_size;
	__u64	 mr_size;
	int	   acflags = IB_ACCESS_LOCAL_WRITE |
				IB_ACCESS_REMOTE_WRITE;

	rc = kiblnd_hdev_get_attr(hdev);
	if (rc != 0)
		return rc;

	if (hdev->ibh_mr_shift == 64) {
		LIBCFS_ALLOC(hdev->ibh_mrs, 1 * sizeof(*hdev->ibh_mrs));
		if (hdev->ibh_mrs == NULL) {
			CERROR("Failed to allocate MRs table\n");
			return -ENOMEM;
		}

		hdev->ibh_mrs[0] = NULL;
		hdev->ibh_nmrs   = 1;

		mr = ib_get_dma_mr(hdev->ibh_pd, acflags);
		if (IS_ERR(mr)) {
			CERROR("Failed ib_get_dma_mr : %ld\n", PTR_ERR(mr));
			kiblnd_hdev_cleanup_mrs(hdev);
			return PTR_ERR(mr);
		}

		hdev->ibh_mrs[0] = mr;

		goto out;
	}

	mr_size = (1ULL << hdev->ibh_mr_shift);
	mm_size = (unsigned long)high_memory - PAGE_OFFSET;

	hdev->ibh_nmrs = (int)((mm_size + mr_size - 1) >> hdev->ibh_mr_shift);

	if (hdev->ibh_mr_shift < 32 || hdev->ibh_nmrs > 1024) {
		/* it's 4T..., assume we will re-code at that time */
		CERROR("Can't support memory size: x%#llx with MR size: x%#llx\n",
		       mm_size, mr_size);
		return -EINVAL;
	}

	/* create an array of MRs to cover all memory */
	LIBCFS_ALLOC(hdev->ibh_mrs, sizeof(*hdev->ibh_mrs) * hdev->ibh_nmrs);
	if (hdev->ibh_mrs == NULL) {
		CERROR("Failed to allocate MRs' table\n");
		return -ENOMEM;
	}

	for (i = 0; i < hdev->ibh_nmrs; i++) {
		struct ib_phys_buf ipb;
		__u64	      iova;

		ipb.size = hdev->ibh_mr_size;
		ipb.addr = i * mr_size;
		iova     = ipb.addr;

		mr = ib_reg_phys_mr(hdev->ibh_pd, &ipb, 1, acflags, &iova);
		if (IS_ERR(mr)) {
			CERROR("Failed ib_reg_phys_mr addr %#llx size %#llx : %ld\n",
			       ipb.addr, ipb.size, PTR_ERR(mr));
			kiblnd_hdev_cleanup_mrs(hdev);
			return PTR_ERR(mr);
		}

		LASSERT (iova == ipb.addr);

		hdev->ibh_mrs[i] = mr;
	}

out:
	if (hdev->ibh_mr_size != ~0ULL || hdev->ibh_nmrs != 1)
		LCONSOLE_INFO("Register global MR array, MR size: %#llx, array size: %d\n",
			      hdev->ibh_mr_size, hdev->ibh_nmrs);
	return 0;
}

static int
kiblnd_dummy_callback(struct rdma_cm_id *cmid, struct rdma_cm_event *event)
{       /* DUMMY */
	return 0;
}

static int
kiblnd_dev_need_failover(kib_dev_t *dev)
{
	struct rdma_cm_id  *cmid;
	struct sockaddr_in  srcaddr;
	struct sockaddr_in  dstaddr;
	int		 rc;

	if (dev->ibd_hdev == NULL || /* initializing */
	    dev->ibd_hdev->ibh_cmid == NULL || /* listener is dead */
	    *kiblnd_tunables.kib_dev_failover > 1) /* debugging */
		return 1;

	/* XXX: it's UGLY, but I don't have better way to find
=======
static int kiblnd_hdev_setup_mrs(struct kib_hca_dev *hdev)
{
	struct ib_mr *mr;
	int rc;
	int acflags = IB_ACCESS_LOCAL_WRITE | IB_ACCESS_REMOTE_WRITE;

	rc = kiblnd_hdev_get_attr(hdev);
	if (rc)
		return rc;

	mr = ib_get_dma_mr(hdev->ibh_pd, acflags);
	if (IS_ERR(mr)) {
		CERROR("Failed ib_get_dma_mr : %ld\n", PTR_ERR(mr));
		kiblnd_hdev_cleanup_mrs(hdev);
		return PTR_ERR(mr);
	}

	hdev->ibh_mrs = mr;

	return 0;
}

/* DUMMY */
static int kiblnd_dummy_callback(struct rdma_cm_id *cmid,
				 struct rdma_cm_event *event)
{
	return 0;
}

static int kiblnd_dev_need_failover(struct kib_dev *dev)
{
	struct rdma_cm_id *cmid;
	struct sockaddr_in srcaddr;
	struct sockaddr_in dstaddr;
	int rc;

	if (!dev->ibd_hdev || /* initializing */
	    !dev->ibd_hdev->ibh_cmid || /* listener is dead */
	    *kiblnd_tunables.kib_dev_failover > 1) /* debugging */
		return 1;

	/*
	 * XXX: it's UGLY, but I don't have better way to find
>>>>>>> v4.9.227
	 * ib-bonding HCA failover because:
	 *
	 * a. no reliable CM event for HCA failover...
	 * b. no OFED API to get ib_device for current net_device...
	 *
	 * We have only two choices at this point:
	 *
	 * a. rdma_bind_addr(), it will conflict with listener cmid
<<<<<<< HEAD
	 * b. rdma_resolve_addr() to zero addr */
=======
	 * b. rdma_resolve_addr() to zero addr
	 */
>>>>>>> v4.9.227
	cmid = kiblnd_rdma_create_id(kiblnd_dummy_callback, dev, RDMA_PS_TCP,
				     IB_QPT_RC);
	if (IS_ERR(cmid)) {
		rc = PTR_ERR(cmid);
		CERROR("Failed to create cmid for failover: %d\n", rc);
		return rc;
	}

	memset(&srcaddr, 0, sizeof(srcaddr));
<<<<<<< HEAD
	srcaddr.sin_family      = AF_INET;
=======
	srcaddr.sin_family = AF_INET;
>>>>>>> v4.9.227
	srcaddr.sin_addr.s_addr = (__force u32)htonl(dev->ibd_ifip);

	memset(&dstaddr, 0, sizeof(dstaddr));
	dstaddr.sin_family = AF_INET;
	rc = rdma_resolve_addr(cmid, (struct sockaddr *)&srcaddr,
			       (struct sockaddr *)&dstaddr, 1);
<<<<<<< HEAD
	if (rc != 0 || cmid->device == NULL) {
=======
	if (rc || !cmid->device) {
>>>>>>> v4.9.227
		CERROR("Failed to bind %s:%pI4h to device(%p): %d\n",
		       dev->ibd_ifname, &dev->ibd_ifip,
		       cmid->device, rc);
		rdma_destroy_id(cmid);
		return rc;
	}

<<<<<<< HEAD
	if (dev->ibd_hdev->ibh_ibdev == cmid->device) {
		/* don't need device failover */
		rdma_destroy_id(cmid);
		return 0;
	}

	return 1;
}

int
kiblnd_dev_failover(kib_dev_t *dev)
{
	LIST_HEAD      (zombie_tpo);
	LIST_HEAD      (zombie_ppo);
	LIST_HEAD      (zombie_fpo);
	struct rdma_cm_id  *cmid  = NULL;
	kib_hca_dev_t      *hdev  = NULL;
	kib_hca_dev_t      *old;
	struct ib_pd       *pd;
	kib_net_t	  *net;
	struct sockaddr_in  addr;
	unsigned long       flags;
	int		 rc = 0;
	int		    i;

	LASSERT (*kiblnd_tunables.kib_dev_failover > 1 ||
		 dev->ibd_can_failover ||
		 dev->ibd_hdev == NULL);
=======
	rc = dev->ibd_hdev->ibh_ibdev != cmid->device; /* true for failover */
	rdma_destroy_id(cmid);

	return rc;
}

int kiblnd_dev_failover(struct kib_dev *dev)
{
	LIST_HEAD(zombie_tpo);
	LIST_HEAD(zombie_ppo);
	LIST_HEAD(zombie_fpo);
	struct rdma_cm_id *cmid  = NULL;
	struct kib_hca_dev *hdev  = NULL;
	struct ib_pd *pd;
	struct kib_net *net;
	struct sockaddr_in addr;
	unsigned long flags;
	int rc = 0;
	int i;

	LASSERT(*kiblnd_tunables.kib_dev_failover > 1 ||
		dev->ibd_can_failover || !dev->ibd_hdev);
>>>>>>> v4.9.227

	rc = kiblnd_dev_need_failover(dev);
	if (rc <= 0)
		goto out;

<<<<<<< HEAD
	if (dev->ibd_hdev != NULL &&
	    dev->ibd_hdev->ibh_cmid != NULL) {
		/* XXX it's not good to close old listener at here,
		 * because we can fail to create new listener.
		 * But we have to close it now, otherwise rdma_bind_addr
		 * will return EADDRINUSE... How crap! */
		write_lock_irqsave(&kiblnd_data.kib_global_lock, flags);

		cmid = dev->ibd_hdev->ibh_cmid;
		/* make next schedule of kiblnd_dev_need_failover()
		 * return 1 for me */
=======
	if (dev->ibd_hdev &&
	    dev->ibd_hdev->ibh_cmid) {
		/*
		 * XXX it's not good to close old listener at here,
		 * because we can fail to create new listener.
		 * But we have to close it now, otherwise rdma_bind_addr
		 * will return EADDRINUSE... How crap!
		 */
		write_lock_irqsave(&kiblnd_data.kib_global_lock, flags);

		cmid = dev->ibd_hdev->ibh_cmid;
		/*
		 * make next schedule of kiblnd_dev_need_failover()
		 * return 1 for me
		 */
>>>>>>> v4.9.227
		dev->ibd_hdev->ibh_cmid  = NULL;
		write_unlock_irqrestore(&kiblnd_data.kib_global_lock, flags);

		rdma_destroy_id(cmid);
	}

	cmid = kiblnd_rdma_create_id(kiblnd_cm_callback, dev, RDMA_PS_TCP,
				     IB_QPT_RC);
	if (IS_ERR(cmid)) {
		rc = PTR_ERR(cmid);
		CERROR("Failed to create cmid for failover: %d\n", rc);
		goto out;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = (__force u32)htonl(dev->ibd_ifip);
	addr.sin_port	= htons(*kiblnd_tunables.kib_service);

	/* Bind to failover device or port */
	rc = rdma_bind_addr(cmid, (struct sockaddr *)&addr);
<<<<<<< HEAD
	if (rc != 0 || cmid->device == NULL) {
=======
	if (rc || !cmid->device) {
>>>>>>> v4.9.227
		CERROR("Failed to bind %s:%pI4h to device(%p): %d\n",
		       dev->ibd_ifname, &dev->ibd_ifip,
		       cmid->device, rc);
		rdma_destroy_id(cmid);
		goto out;
	}

	LIBCFS_ALLOC(hdev, sizeof(*hdev));
<<<<<<< HEAD
	if (hdev == NULL) {
=======
	if (!hdev) {
>>>>>>> v4.9.227
		CERROR("Failed to allocate kib_hca_dev\n");
		rdma_destroy_id(cmid);
		rc = -ENOMEM;
		goto out;
	}

	atomic_set(&hdev->ibh_ref, 1);
	hdev->ibh_dev   = dev;
	hdev->ibh_cmid  = cmid;
	hdev->ibh_ibdev = cmid->device;

<<<<<<< HEAD
	pd = ib_alloc_pd(cmid->device);
=======
	pd = ib_alloc_pd(cmid->device, 0);
>>>>>>> v4.9.227
	if (IS_ERR(pd)) {
		rc = PTR_ERR(pd);
		CERROR("Can't allocate PD: %d\n", rc);
		goto out;
	}

	hdev->ibh_pd = pd;

	rc = rdma_listen(cmid, 0);
<<<<<<< HEAD
	if (rc != 0) {
=======
	if (rc) {
>>>>>>> v4.9.227
		CERROR("Can't start new listener: %d\n", rc);
		goto out;
	}

	rc = kiblnd_hdev_setup_mrs(hdev);
<<<<<<< HEAD
	if (rc != 0) {
=======
	if (rc) {
>>>>>>> v4.9.227
		CERROR("Can't setup device: %d\n", rc);
		goto out;
	}

	write_lock_irqsave(&kiblnd_data.kib_global_lock, flags);

<<<<<<< HEAD
	old = dev->ibd_hdev;
	dev->ibd_hdev = hdev; /* take over the refcount */
	hdev = old;
=======
	swap(dev->ibd_hdev, hdev); /* take over the refcount */
>>>>>>> v4.9.227

	list_for_each_entry(net, &dev->ibd_nets, ibn_list) {
		cfs_cpt_for_each(i, lnet_cpt_table()) {
			kiblnd_fail_poolset(&net->ibn_tx_ps[i]->tps_poolset,
					    &zombie_tpo);

<<<<<<< HEAD
			if (net->ibn_fmr_ps != NULL) {
				kiblnd_fail_fmr_poolset(net->ibn_fmr_ps[i],
							&zombie_fpo);

			} else if (net->ibn_pmr_ps != NULL) {
				kiblnd_fail_poolset(&net->ibn_pmr_ps[i]->
						    pps_poolset, &zombie_ppo);
			}
=======
			if (net->ibn_fmr_ps)
				kiblnd_fail_fmr_poolset(net->ibn_fmr_ps[i],
							&zombie_fpo);
>>>>>>> v4.9.227
		}
	}

	write_unlock_irqrestore(&kiblnd_data.kib_global_lock, flags);
 out:
	if (!list_empty(&zombie_tpo))
		kiblnd_destroy_pool_list(&zombie_tpo);
	if (!list_empty(&zombie_ppo))
		kiblnd_destroy_pool_list(&zombie_ppo);
	if (!list_empty(&zombie_fpo))
		kiblnd_destroy_fmr_pool_list(&zombie_fpo);
<<<<<<< HEAD
	if (hdev != NULL)
		kiblnd_hdev_decref(hdev);

	if (rc != 0)
=======
	if (hdev)
		kiblnd_hdev_decref(hdev);

	if (rc)
>>>>>>> v4.9.227
		dev->ibd_failed_failover++;
	else
		dev->ibd_failed_failover = 0;

	return rc;
}

<<<<<<< HEAD
void
kiblnd_destroy_dev (kib_dev_t *dev)
{
	LASSERT (dev->ibd_nnets == 0);
	LASSERT (list_empty(&dev->ibd_nets));
=======
void kiblnd_destroy_dev(struct kib_dev *dev)
{
	LASSERT(!dev->ibd_nnets);
	LASSERT(list_empty(&dev->ibd_nets));
>>>>>>> v4.9.227

	list_del(&dev->ibd_fail_list);
	list_del(&dev->ibd_list);

<<<<<<< HEAD
	if (dev->ibd_hdev != NULL)
=======
	if (dev->ibd_hdev)
>>>>>>> v4.9.227
		kiblnd_hdev_decref(dev->ibd_hdev);

	LIBCFS_FREE(dev, sizeof(*dev));
}

<<<<<<< HEAD
static kib_dev_t *
kiblnd_create_dev(char *ifname)
{
	struct net_device *netdev;
	kib_dev_t	 *dev;
	__u32	      netmask;
	__u32	      ip;
	int		up;
	int		rc;

	rc = libcfs_ipif_query(ifname, &up, &ip, &netmask);
	if (rc != 0) {
=======
static struct kib_dev *kiblnd_create_dev(char *ifname)
{
	struct net_device *netdev;
	struct kib_dev *dev;
	__u32 netmask;
	__u32 ip;
	int up;
	int rc;

	rc = lnet_ipif_query(ifname, &up, &ip, &netmask);
	if (rc) {
>>>>>>> v4.9.227
		CERROR("Can't query IPoIB interface %s: %d\n",
		       ifname, rc);
		return NULL;
	}

	if (!up) {
		CERROR("Can't query IPoIB interface %s: it's down\n", ifname);
		return NULL;
	}

	LIBCFS_ALLOC(dev, sizeof(*dev));
<<<<<<< HEAD
	if (dev == NULL)
		return NULL;

	netdev = dev_get_by_name(&init_net, ifname);
	if (netdev == NULL) {
=======
	if (!dev)
		return NULL;

	netdev = dev_get_by_name(&init_net, ifname);
	if (!netdev) {
>>>>>>> v4.9.227
		dev->ibd_can_failover = 0;
	} else {
		dev->ibd_can_failover = !!(netdev->flags & IFF_MASTER);
		dev_put(netdev);
	}

	INIT_LIST_HEAD(&dev->ibd_nets);
	INIT_LIST_HEAD(&dev->ibd_list); /* not yet in kib_devs */
	INIT_LIST_HEAD(&dev->ibd_fail_list);
	dev->ibd_ifip = ip;
	strcpy(&dev->ibd_ifname[0], ifname);

	/* initialize the device */
	rc = kiblnd_dev_failover(dev);
<<<<<<< HEAD
	if (rc != 0) {
=======
	if (rc) {
>>>>>>> v4.9.227
		CERROR("Can't initialize device: %d\n", rc);
		LIBCFS_FREE(dev, sizeof(*dev));
		return NULL;
	}

<<<<<<< HEAD
	list_add_tail(&dev->ibd_list,
			  &kiblnd_data.kib_devs);
	return dev;
}

static void
kiblnd_base_shutdown(void)
{
	struct kib_sched_info	*sched;
	int			i;

	LASSERT (list_empty(&kiblnd_data.kib_devs));

	CDEBUG(D_MALLOC, "before LND base cleanup: kmem %d\n",
	       atomic_read(&libcfs_kmemory));
=======
	list_add_tail(&dev->ibd_list, &kiblnd_data.kib_devs);
	return dev;
}

static void kiblnd_base_shutdown(void)
{
	struct kib_sched_info *sched;
	int i;

	LASSERT(list_empty(&kiblnd_data.kib_devs));
>>>>>>> v4.9.227

	switch (kiblnd_data.kib_init) {
	default:
		LBUG();

	case IBLND_INIT_ALL:
	case IBLND_INIT_DATA:
<<<<<<< HEAD
		LASSERT (kiblnd_data.kib_peers != NULL);
		for (i = 0; i < kiblnd_data.kib_peer_hash_size; i++) {
			LASSERT (list_empty(&kiblnd_data.kib_peers[i]));
		}
		LASSERT (list_empty(&kiblnd_data.kib_connd_zombies));
		LASSERT (list_empty(&kiblnd_data.kib_connd_conns));
=======
		LASSERT(kiblnd_data.kib_peers);
		for (i = 0; i < kiblnd_data.kib_peer_hash_size; i++)
			LASSERT(list_empty(&kiblnd_data.kib_peers[i]));
		LASSERT(list_empty(&kiblnd_data.kib_connd_zombies));
		LASSERT(list_empty(&kiblnd_data.kib_connd_conns));
		LASSERT(list_empty(&kiblnd_data.kib_reconn_list));
		LASSERT(list_empty(&kiblnd_data.kib_reconn_wait));
>>>>>>> v4.9.227

		/* flag threads to terminate; wake and wait for them to die */
		kiblnd_data.kib_shutdown = 1;

<<<<<<< HEAD
		/* NB: we really want to stop scheduler threads net by net
		 * instead of the whole module, this should be improved
		 * with dynamic configuration LNet */
=======
		/*
		 * NB: we really want to stop scheduler threads net by net
		 * instead of the whole module, this should be improved
		 * with dynamic configuration LNet
		 */
>>>>>>> v4.9.227
		cfs_percpt_for_each(sched, i, kiblnd_data.kib_scheds)
			wake_up_all(&sched->ibs_waitq);

		wake_up_all(&kiblnd_data.kib_connd_waitq);
		wake_up_all(&kiblnd_data.kib_failover_waitq);

		i = 2;
<<<<<<< HEAD
		while (atomic_read(&kiblnd_data.kib_nthreads) != 0) {
			i++;
			CDEBUG(((i & (-i)) == i) ? D_WARNING : D_NET, /* power of 2? */
=======
		while (atomic_read(&kiblnd_data.kib_nthreads)) {
			i++;
			/* power of 2 ? */
			CDEBUG(((i & (-i)) == i) ? D_WARNING : D_NET,
>>>>>>> v4.9.227
			       "Waiting for %d threads to terminate\n",
			       atomic_read(&kiblnd_data.kib_nthreads));
			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule_timeout(cfs_time_seconds(1));
		}

		/* fall through */

	case IBLND_INIT_NOTHING:
		break;
	}

<<<<<<< HEAD
	if (kiblnd_data.kib_peers != NULL) {
=======
	if (kiblnd_data.kib_peers) {
>>>>>>> v4.9.227
		LIBCFS_FREE(kiblnd_data.kib_peers,
			    sizeof(struct list_head) *
			    kiblnd_data.kib_peer_hash_size);
	}

<<<<<<< HEAD
	if (kiblnd_data.kib_scheds != NULL)
		cfs_percpt_free(kiblnd_data.kib_scheds);

	CDEBUG(D_MALLOC, "after LND base cleanup: kmem %d\n",
	       atomic_read(&libcfs_kmemory));

=======
	if (kiblnd_data.kib_scheds)
		cfs_percpt_free(kiblnd_data.kib_scheds);

>>>>>>> v4.9.227
	kiblnd_data.kib_init = IBLND_INIT_NOTHING;
	module_put(THIS_MODULE);
}

<<<<<<< HEAD
void
kiblnd_shutdown (lnet_ni_t *ni)
{
	kib_net_t	*net = ni->ni_data;
	rwlock_t     *g_lock = &kiblnd_data.kib_global_lock;
	int	       i;
	unsigned long     flags;

	LASSERT(kiblnd_data.kib_init == IBLND_INIT_ALL);

	if (net == NULL)
		goto out;

	CDEBUG(D_MALLOC, "before LND net cleanup: kmem %d\n",
	       atomic_read(&libcfs_kmemory));

=======
static void kiblnd_shutdown(lnet_ni_t *ni)
{
	struct kib_net *net = ni->ni_data;
	rwlock_t *g_lock = &kiblnd_data.kib_global_lock;
	int i;
	unsigned long flags;

	LASSERT(kiblnd_data.kib_init == IBLND_INIT_ALL);

	if (!net)
		goto out;

>>>>>>> v4.9.227
	write_lock_irqsave(g_lock, flags);
	net->ibn_shutdown = 1;
	write_unlock_irqrestore(g_lock, flags);

	switch (net->ibn_init) {
	default:
		LBUG();

	case IBLND_INIT_ALL:
		/* nuke all existing peers within this net */
		kiblnd_del_peer(ni, LNET_NID_ANY);

		/* Wait for all peer state to clean up */
		i = 2;
<<<<<<< HEAD
		while (atomic_read(&net->ibn_npeers) != 0) {
=======
		while (atomic_read(&net->ibn_npeers)) {
>>>>>>> v4.9.227
			i++;
			CDEBUG(((i & (-i)) == i) ? D_WARNING : D_NET, /* 2**n? */
			       "%s: waiting for %d peers to disconnect\n",
			       libcfs_nid2str(ni->ni_nid),
			       atomic_read(&net->ibn_npeers));
			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule_timeout(cfs_time_seconds(1));
		}

		kiblnd_net_fini_pools(net);

		write_lock_irqsave(g_lock, flags);
		LASSERT(net->ibn_dev->ibd_nnets > 0);
		net->ibn_dev->ibd_nnets--;
		list_del(&net->ibn_list);
		write_unlock_irqrestore(g_lock, flags);

		/* fall through */

	case IBLND_INIT_NOTHING:
<<<<<<< HEAD
		LASSERT (atomic_read(&net->ibn_nconns) == 0);

		if (net->ibn_dev != NULL &&
		    net->ibn_dev->ibd_nnets == 0)
=======
		LASSERT(!atomic_read(&net->ibn_nconns));

		if (net->ibn_dev && !net->ibn_dev->ibd_nnets)
>>>>>>> v4.9.227
			kiblnd_destroy_dev(net->ibn_dev);

		break;
	}

<<<<<<< HEAD
	CDEBUG(D_MALLOC, "after LND net cleanup: kmem %d\n",
	       atomic_read(&libcfs_kmemory));

=======
>>>>>>> v4.9.227
	net->ibn_init = IBLND_INIT_NOTHING;
	ni->ni_data = NULL;

	LIBCFS_FREE(net, sizeof(*net));

out:
	if (list_empty(&kiblnd_data.kib_devs))
		kiblnd_base_shutdown();
<<<<<<< HEAD
	return;
}

static int
kiblnd_base_startup(void)
{
	struct kib_sched_info	*sched;
	int			rc;
	int			i;

	LASSERT (kiblnd_data.kib_init == IBLND_INIT_NOTHING);

	try_module_get(THIS_MODULE);
	memset(&kiblnd_data, 0, sizeof(kiblnd_data)); /* zero pointers, flags etc */
=======
}

static int kiblnd_base_startup(void)
{
	struct kib_sched_info *sched;
	int rc;
	int i;

	LASSERT(kiblnd_data.kib_init == IBLND_INIT_NOTHING);

	try_module_get(THIS_MODULE);
	/* zero pointers, flags etc */
	memset(&kiblnd_data, 0, sizeof(kiblnd_data));
>>>>>>> v4.9.227

	rwlock_init(&kiblnd_data.kib_global_lock);

	INIT_LIST_HEAD(&kiblnd_data.kib_devs);
	INIT_LIST_HEAD(&kiblnd_data.kib_failed_devs);

	kiblnd_data.kib_peer_hash_size = IBLND_PEER_HASH_SIZE;
	LIBCFS_ALLOC(kiblnd_data.kib_peers,
<<<<<<< HEAD
		     sizeof(struct list_head) *
			    kiblnd_data.kib_peer_hash_size);
	if (kiblnd_data.kib_peers == NULL) {
		goto failed;
	}
=======
		     sizeof(struct list_head) * kiblnd_data.kib_peer_hash_size);
	if (!kiblnd_data.kib_peers)
		goto failed;
>>>>>>> v4.9.227
	for (i = 0; i < kiblnd_data.kib_peer_hash_size; i++)
		INIT_LIST_HEAD(&kiblnd_data.kib_peers[i]);

	spin_lock_init(&kiblnd_data.kib_connd_lock);
	INIT_LIST_HEAD(&kiblnd_data.kib_connd_conns);
	INIT_LIST_HEAD(&kiblnd_data.kib_connd_zombies);
<<<<<<< HEAD
=======
	INIT_LIST_HEAD(&kiblnd_data.kib_reconn_list);
	INIT_LIST_HEAD(&kiblnd_data.kib_reconn_wait);

>>>>>>> v4.9.227
	init_waitqueue_head(&kiblnd_data.kib_connd_waitq);
	init_waitqueue_head(&kiblnd_data.kib_failover_waitq);

	kiblnd_data.kib_scheds = cfs_percpt_alloc(lnet_cpt_table(),
						  sizeof(*sched));
<<<<<<< HEAD
	if (kiblnd_data.kib_scheds == NULL)
		goto failed;

	cfs_percpt_for_each(sched, i, kiblnd_data.kib_scheds) {
		int	nthrs;
=======
	if (!kiblnd_data.kib_scheds)
		goto failed;

	cfs_percpt_for_each(sched, i, kiblnd_data.kib_scheds) {
		int nthrs;
>>>>>>> v4.9.227

		spin_lock_init(&sched->ibs_lock);
		INIT_LIST_HEAD(&sched->ibs_conns);
		init_waitqueue_head(&sched->ibs_waitq);

		nthrs = cfs_cpt_weight(lnet_cpt_table(), i);
		if (*kiblnd_tunables.kib_nscheds > 0) {
			nthrs = min(nthrs, *kiblnd_tunables.kib_nscheds);
		} else {
<<<<<<< HEAD
			/* max to half of CPUs, another half is reserved for
			 * upper layer modules */
=======
			/*
			 * max to half of CPUs, another half is reserved for
			 * upper layer modules
			 */
>>>>>>> v4.9.227
			nthrs = min(max(IBLND_N_SCHED, nthrs >> 1), nthrs);
		}

		sched->ibs_nthreads_max = nthrs;
		sched->ibs_cpt = i;
	}

	kiblnd_data.kib_error_qpa.qp_state = IB_QPS_ERR;

	/* lists/ptrs/locks initialised */
	kiblnd_data.kib_init = IBLND_INIT_DATA;
	/*****************************************************/

	rc = kiblnd_thread_start(kiblnd_connd, NULL, "kiblnd_connd");
<<<<<<< HEAD
	if (rc != 0) {
=======
	if (rc) {
>>>>>>> v4.9.227
		CERROR("Can't spawn o2iblnd connd: %d\n", rc);
		goto failed;
	}

<<<<<<< HEAD
	if (*kiblnd_tunables.kib_dev_failover != 0)
		rc = kiblnd_thread_start(kiblnd_failover_thread, NULL,
					 "kiblnd_failover");

	if (rc != 0) {
=======
	if (*kiblnd_tunables.kib_dev_failover)
		rc = kiblnd_thread_start(kiblnd_failover_thread, NULL,
					 "kiblnd_failover");

	if (rc) {
>>>>>>> v4.9.227
		CERROR("Can't spawn o2iblnd failover thread: %d\n", rc);
		goto failed;
	}

	/* flag everything initialised */
	kiblnd_data.kib_init = IBLND_INIT_ALL;
	/*****************************************************/

	return 0;

 failed:
	kiblnd_base_shutdown();
	return -ENETDOWN;
}

<<<<<<< HEAD
static int
kiblnd_start_schedulers(struct kib_sched_info *sched)
{
	int	rc = 0;
	int	nthrs;
	int	i;

	if (sched->ibs_nthreads == 0) {
=======
static int kiblnd_start_schedulers(struct kib_sched_info *sched)
{
	int rc = 0;
	int nthrs;
	int i;

	if (!sched->ibs_nthreads) {
>>>>>>> v4.9.227
		if (*kiblnd_tunables.kib_nscheds > 0) {
			nthrs = sched->ibs_nthreads_max;
		} else {
			nthrs = cfs_cpt_weight(lnet_cpt_table(),
					       sched->ibs_cpt);
			nthrs = min(max(IBLND_N_SCHED, nthrs >> 1), nthrs);
			nthrs = min(IBLND_N_SCHED_HIGH, nthrs);
		}
	} else {
		LASSERT(sched->ibs_nthreads <= sched->ibs_nthreads_max);
		/* increase one thread if there is new interface */
<<<<<<< HEAD
		nthrs = (sched->ibs_nthreads < sched->ibs_nthreads_max);
	}

	for (i = 0; i < nthrs; i++) {
		long	id;
		char	name[20];
=======
		nthrs = sched->ibs_nthreads < sched->ibs_nthreads_max;
	}

	for (i = 0; i < nthrs; i++) {
		long id;
		char name[20];

>>>>>>> v4.9.227
		id = KIB_THREAD_ID(sched->ibs_cpt, sched->ibs_nthreads + i);
		snprintf(name, sizeof(name), "kiblnd_sd_%02ld_%02ld",
			 KIB_THREAD_CPT(id), KIB_THREAD_TID(id));
		rc = kiblnd_thread_start(kiblnd_scheduler, (void *)id, name);
<<<<<<< HEAD
		if (rc == 0)
=======
		if (!rc)
>>>>>>> v4.9.227
			continue;

		CERROR("Can't spawn thread %d for scheduler[%d]: %d\n",
		       sched->ibs_cpt, sched->ibs_nthreads + i, rc);
		break;
	}

	sched->ibs_nthreads += i;
	return rc;
}

<<<<<<< HEAD
static int
kiblnd_dev_start_threads(kib_dev_t *dev, int newdev, __u32 *cpts, int ncpts)
{
	int	cpt;
	int	rc;
	int	i;
=======
static int kiblnd_dev_start_threads(struct kib_dev *dev, int newdev, __u32 *cpts,
				    int ncpts)
{
	int cpt;
	int rc;
	int i;
>>>>>>> v4.9.227

	for (i = 0; i < ncpts; i++) {
		struct kib_sched_info *sched;

<<<<<<< HEAD
		cpt = (cpts == NULL) ? i : cpts[i];
=======
		cpt = !cpts ? i : cpts[i];
>>>>>>> v4.9.227
		sched = kiblnd_data.kib_scheds[cpt];

		if (!newdev && sched->ibs_nthreads > 0)
			continue;

		rc = kiblnd_start_schedulers(kiblnd_data.kib_scheds[cpt]);
<<<<<<< HEAD
		if (rc != 0) {
=======
		if (rc) {
>>>>>>> v4.9.227
			CERROR("Failed to start scheduler threads for %s\n",
			       dev->ibd_ifname);
			return rc;
		}
	}
	return 0;
}

<<<<<<< HEAD
static kib_dev_t *
kiblnd_dev_search(char *ifname)
{
	kib_dev_t	*alias = NULL;
	kib_dev_t	*dev;
	char		*colon;
	char		*colon2;

	colon = strchr(ifname, ':');
	list_for_each_entry(dev, &kiblnd_data.kib_devs, ibd_list) {
		if (strcmp(&dev->ibd_ifname[0], ifname) == 0)
			return dev;

		if (alias != NULL)
			continue;

		colon2 = strchr(dev->ibd_ifname, ':');
		if (colon != NULL)
			*colon = 0;
		if (colon2 != NULL)
			*colon2 = 0;

		if (strcmp(&dev->ibd_ifname[0], ifname) == 0)
			alias = dev;

		if (colon != NULL)
			*colon = ':';
		if (colon2 != NULL)
=======
static struct kib_dev *kiblnd_dev_search(char *ifname)
{
	struct kib_dev *alias = NULL;
	struct kib_dev *dev;
	char *colon;
	char *colon2;

	colon = strchr(ifname, ':');
	list_for_each_entry(dev, &kiblnd_data.kib_devs, ibd_list) {
		if (!strcmp(&dev->ibd_ifname[0], ifname))
			return dev;

		if (alias)
			continue;

		colon2 = strchr(dev->ibd_ifname, ':');
		if (colon)
			*colon = 0;
		if (colon2)
			*colon2 = 0;

		if (!strcmp(&dev->ibd_ifname[0], ifname))
			alias = dev;

		if (colon)
			*colon = ':';
		if (colon2)
>>>>>>> v4.9.227
			*colon2 = ':';
	}
	return alias;
}

<<<<<<< HEAD
int
kiblnd_startup (lnet_ni_t *ni)
{
	char		     *ifname;
	kib_dev_t		*ibdev = NULL;
	kib_net_t		*net;
	struct timeval	    tv;
	unsigned long	     flags;
	int		       rc;
	int			  newdev;

	LASSERT (ni->ni_lnd == &the_o2iblnd);

	if (kiblnd_data.kib_init == IBLND_INIT_NOTHING) {
		rc = kiblnd_base_startup();
		if (rc != 0)
=======
static int kiblnd_startup(lnet_ni_t *ni)
{
	char *ifname;
	struct kib_dev *ibdev = NULL;
	struct kib_net *net;
	struct timespec64 tv;
	unsigned long flags;
	int rc;
	int newdev;

	LASSERT(ni->ni_lnd == &the_o2iblnd);

	if (kiblnd_data.kib_init == IBLND_INIT_NOTHING) {
		rc = kiblnd_base_startup();
		if (rc)
>>>>>>> v4.9.227
			return rc;
	}

	LIBCFS_ALLOC(net, sizeof(*net));
	ni->ni_data = net;
<<<<<<< HEAD
	if (net == NULL)
		goto failed;

	do_gettimeofday(&tv);
	net->ibn_incarnation = (((__u64)tv.tv_sec) * 1000000) + tv.tv_usec;

	ni->ni_peertimeout    = *kiblnd_tunables.kib_peertimeout;
	ni->ni_maxtxcredits   = *kiblnd_tunables.kib_credits;
	ni->ni_peertxcredits  = *kiblnd_tunables.kib_peertxcredits;
	ni->ni_peerrtrcredits = *kiblnd_tunables.kib_peerrtrcredits;

	if (ni->ni_interfaces[0] != NULL) {
		/* Use the IPoIB interface specified in 'networks=' */

		CLASSERT (LNET_MAX_INTERFACES > 1);
		if (ni->ni_interfaces[1] != NULL) {
=======
	if (!net)
		goto net_failed;

	ktime_get_real_ts64(&tv);
	net->ibn_incarnation = tv.tv_sec * USEC_PER_SEC +
			       tv.tv_nsec / NSEC_PER_USEC;

	rc = kiblnd_tunables_setup(ni);
	if (rc)
		goto net_failed;

	if (ni->ni_interfaces[0]) {
		/* Use the IPoIB interface specified in 'networks=' */

		CLASSERT(LNET_MAX_INTERFACES > 1);
		if (ni->ni_interfaces[1]) {
>>>>>>> v4.9.227
			CERROR("Multiple interfaces not supported\n");
			goto failed;
		}

		ifname = ni->ni_interfaces[0];
	} else {
		ifname = *kiblnd_tunables.kib_default_ipif;
	}

	if (strlen(ifname) >= sizeof(ibdev->ibd_ifname)) {
		CERROR("IPoIB interface name too long: %s\n", ifname);
		goto failed;
	}

	ibdev = kiblnd_dev_search(ifname);

<<<<<<< HEAD
	newdev = ibdev == NULL;
	/* hmm...create kib_dev even for alias */
	if (ibdev == NULL || strcmp(&ibdev->ibd_ifname[0], ifname) != 0)
		ibdev = kiblnd_create_dev(ifname);

	if (ibdev == NULL)
=======
	newdev = !ibdev;
	/* hmm...create kib_dev even for alias */
	if (!ibdev || strcmp(&ibdev->ibd_ifname[0], ifname))
		ibdev = kiblnd_create_dev(ifname);

	if (!ibdev)
>>>>>>> v4.9.227
		goto failed;

	net->ibn_dev = ibdev;
	ni->ni_nid = LNET_MKNID(LNET_NIDNET(ni->ni_nid), ibdev->ibd_ifip);

	rc = kiblnd_dev_start_threads(ibdev, newdev,
				      ni->ni_cpts, ni->ni_ncpts);
<<<<<<< HEAD
	if (rc != 0)
		goto failed;

	rc = kiblnd_net_init_pools(net, ni->ni_cpts, ni->ni_ncpts);
	if (rc != 0) {
=======
	if (rc)
		goto failed;

	rc = kiblnd_net_init_pools(net, ni, ni->ni_cpts, ni->ni_ncpts);
	if (rc) {
>>>>>>> v4.9.227
		CERROR("Failed to initialize NI pools: %d\n", rc);
		goto failed;
	}

	write_lock_irqsave(&kiblnd_data.kib_global_lock, flags);
	ibdev->ibd_nnets++;
	list_add_tail(&net->ibn_list, &ibdev->ibd_nets);
	write_unlock_irqrestore(&kiblnd_data.kib_global_lock, flags);

	net->ibn_init = IBLND_INIT_ALL;

	return 0;

failed:
<<<<<<< HEAD
	if (net->ibn_dev == NULL && ibdev != NULL)
		kiblnd_destroy_dev(ibdev);

=======
	if (!net->ibn_dev && ibdev)
		kiblnd_destroy_dev(ibdev);

net_failed:
>>>>>>> v4.9.227
	kiblnd_shutdown(ni);

	CDEBUG(D_NET, "kiblnd_startup failed\n");
	return -ENETDOWN;
}

<<<<<<< HEAD
static void __exit
kiblnd_module_fini (void)
=======
static lnd_t the_o2iblnd = {
	.lnd_type	= O2IBLND,
	.lnd_startup	= kiblnd_startup,
	.lnd_shutdown	= kiblnd_shutdown,
	.lnd_ctl	= kiblnd_ctl,
	.lnd_query	= kiblnd_query,
	.lnd_send	= kiblnd_send,
	.lnd_recv	= kiblnd_recv,
};

static void __exit ko2iblnd_exit(void)
>>>>>>> v4.9.227
{
	lnet_unregister_lnd(&the_o2iblnd);
}

<<<<<<< HEAD
static int __init
kiblnd_module_init (void)
{
	int    rc;

	CLASSERT (sizeof(kib_msg_t) <= IBLND_MSG_SIZE);
	CLASSERT (offsetof(kib_msg_t, ibm_u.get.ibgm_rd.rd_frags[IBLND_MAX_RDMA_FRAGS])
		  <= IBLND_MSG_SIZE);
	CLASSERT (offsetof(kib_msg_t, ibm_u.putack.ibpam_rd.rd_frags[IBLND_MAX_RDMA_FRAGS])
		  <= IBLND_MSG_SIZE);

	rc = kiblnd_tunables_init();
	if (rc != 0)
		return rc;
=======
static int __init ko2iblnd_init(void)
{
	CLASSERT(sizeof(struct kib_msg) <= IBLND_MSG_SIZE);
	CLASSERT(offsetof(struct kib_msg,
			  ibm_u.get.ibgm_rd.rd_frags[IBLND_MAX_RDMA_FRAGS])
			  <= IBLND_MSG_SIZE);
	CLASSERT(offsetof(struct kib_msg,
			  ibm_u.putack.ibpam_rd.rd_frags[IBLND_MAX_RDMA_FRAGS])
			  <= IBLND_MSG_SIZE);

	kiblnd_tunables_init();
>>>>>>> v4.9.227

	lnet_register_lnd(&the_o2iblnd);

	return 0;
}

<<<<<<< HEAD
MODULE_AUTHOR("Sun Microsystems, Inc. <http://www.lustre.org/>");
MODULE_DESCRIPTION("Kernel OpenIB gen2 LND v2.00");
MODULE_LICENSE("GPL");

module_init(kiblnd_module_init);
module_exit(kiblnd_module_fini);
=======
MODULE_AUTHOR("OpenSFS, Inc. <http://www.lustre.org/>");
MODULE_DESCRIPTION("OpenIB gen2 LNet Network Driver");
MODULE_VERSION("2.7.0");
MODULE_LICENSE("GPL");

module_init(ko2iblnd_init);
module_exit(ko2iblnd_exit);
>>>>>>> v4.9.227
