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
 * Copyright (c) 2003, 2010, Oracle and/or its affiliates. All rights reserved.
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
 * lnet/klnds/socklnd/socklnd.c
 *
 * Author: Zach Brown <zab@zabbo.net>
 * Author: Peter J. Braam <braam@clusterfs.com>
 * Author: Phil Schwan <phil@clusterfs.com>
 * Author: Eric Barton <eric@bartonsoftware.com>
 */

#include "socklnd.h"

static lnd_t the_ksocklnd;
<<<<<<< HEAD
ksock_nal_data_t ksocknal_data;

static ksock_interface_t *
ksocknal_ip2iface(lnet_ni_t *ni, __u32 ip)
{
	ksock_net_t       *net = ni->ni_data;
	int		i;
	ksock_interface_t *iface;
=======
struct ksock_nal_data ksocknal_data;

static struct ksock_interface *
ksocknal_ip2iface(lnet_ni_t *ni, __u32 ip)
{
	struct ksock_net *net = ni->ni_data;
	int i;
	struct ksock_interface *iface;
>>>>>>> v4.9.227

	for (i = 0; i < net->ksnn_ninterfaces; i++) {
		LASSERT(i < LNET_MAX_INTERFACES);
		iface = &net->ksnn_interfaces[i];

		if (iface->ksni_ipaddr == ip)
			return iface;
	}

	return NULL;
}

<<<<<<< HEAD
static ksock_route_t *
ksocknal_create_route (__u32 ipaddr, int port)
{
	ksock_route_t *route;

	LIBCFS_ALLOC (route, sizeof (*route));
	if (route == NULL)
		return NULL;

	atomic_set (&route->ksnr_refcount, 1);
=======
static struct ksock_route *
ksocknal_create_route(__u32 ipaddr, int port)
{
	struct ksock_route *route;

	LIBCFS_ALLOC(route, sizeof(*route));
	if (!route)
		return NULL;

	atomic_set(&route->ksnr_refcount, 1);
>>>>>>> v4.9.227
	route->ksnr_peer = NULL;
	route->ksnr_retry_interval = 0;	 /* OK to connect at any time */
	route->ksnr_ipaddr = ipaddr;
	route->ksnr_port = port;
	route->ksnr_scheduled = 0;
	route->ksnr_connecting = 0;
	route->ksnr_connected = 0;
	route->ksnr_deleted = 0;
	route->ksnr_conn_count = 0;
	route->ksnr_share_count = 0;

	return route;
}

void
<<<<<<< HEAD
ksocknal_destroy_route (ksock_route_t *route)
{
	LASSERT (atomic_read(&route->ksnr_refcount) == 0);

	if (route->ksnr_peer != NULL)
		ksocknal_peer_decref(route->ksnr_peer);

	LIBCFS_FREE (route, sizeof (*route));
}

static int
ksocknal_create_peer (ksock_peer_t **peerp, lnet_ni_t *ni, lnet_process_id_t id)
{
	ksock_net_t   *net = ni->ni_data;
	ksock_peer_t  *peer;

	LASSERT (id.nid != LNET_NID_ANY);
	LASSERT (id.pid != LNET_PID_ANY);
	LASSERT (!in_interrupt());

	LIBCFS_ALLOC (peer, sizeof (*peer));
	if (peer == NULL)
=======
ksocknal_destroy_route(struct ksock_route *route)
{
	LASSERT(!atomic_read(&route->ksnr_refcount));

	if (route->ksnr_peer)
		ksocknal_peer_decref(route->ksnr_peer);

	LIBCFS_FREE(route, sizeof(*route));
}

static int
ksocknal_create_peer(struct ksock_peer **peerp, lnet_ni_t *ni, lnet_process_id_t id)
{
	int cpt = lnet_cpt_of_nid(id.nid);
	struct ksock_net *net = ni->ni_data;
	struct ksock_peer *peer;

	LASSERT(id.nid != LNET_NID_ANY);
	LASSERT(id.pid != LNET_PID_ANY);
	LASSERT(!in_interrupt());

	LIBCFS_CPT_ALLOC(peer, lnet_cpt_table(), cpt, sizeof(*peer));
	if (!peer)
>>>>>>> v4.9.227
		return -ENOMEM;

	peer->ksnp_ni = ni;
	peer->ksnp_id = id;
<<<<<<< HEAD
	atomic_set (&peer->ksnp_refcount, 1);   /* 1 ref for caller */
=======
	atomic_set(&peer->ksnp_refcount, 1);   /* 1 ref for caller */
>>>>>>> v4.9.227
	peer->ksnp_closing = 0;
	peer->ksnp_accepting = 0;
	peer->ksnp_proto = NULL;
	peer->ksnp_last_alive = 0;
	peer->ksnp_zc_next_cookie = SOCKNAL_KEEPALIVE_PING + 1;

<<<<<<< HEAD
	INIT_LIST_HEAD (&peer->ksnp_conns);
	INIT_LIST_HEAD (&peer->ksnp_routes);
	INIT_LIST_HEAD (&peer->ksnp_tx_queue);
	INIT_LIST_HEAD (&peer->ksnp_zc_req_list);
=======
	INIT_LIST_HEAD(&peer->ksnp_conns);
	INIT_LIST_HEAD(&peer->ksnp_routes);
	INIT_LIST_HEAD(&peer->ksnp_tx_queue);
	INIT_LIST_HEAD(&peer->ksnp_zc_req_list);
>>>>>>> v4.9.227
	spin_lock_init(&peer->ksnp_lock);

	spin_lock_bh(&net->ksnn_lock);

	if (net->ksnn_shutdown) {
		spin_unlock_bh(&net->ksnn_lock);

		LIBCFS_FREE(peer, sizeof(*peer));
		CERROR("Can't create peer: network shutdown\n");
		return -ESHUTDOWN;
	}

	net->ksnn_npeers++;

	spin_unlock_bh(&net->ksnn_lock);

	*peerp = peer;
	return 0;
}

void
<<<<<<< HEAD
ksocknal_destroy_peer (ksock_peer_t *peer)
{
	ksock_net_t    *net = peer->ksnp_ni->ni_data;

	CDEBUG (D_NET, "peer %s %p deleted\n",
		libcfs_id2str(peer->ksnp_id), peer);

	LASSERT (atomic_read (&peer->ksnp_refcount) == 0);
	LASSERT (peer->ksnp_accepting == 0);
	LASSERT (list_empty (&peer->ksnp_conns));
	LASSERT (list_empty (&peer->ksnp_routes));
	LASSERT (list_empty (&peer->ksnp_tx_queue));
	LASSERT (list_empty (&peer->ksnp_zc_req_list));

	LIBCFS_FREE (peer, sizeof (*peer));

	/* NB a peer's connections and routes keep a reference on their peer
	 * until they are destroyed, so we can be assured that _all_ state to
	 * do with this peer has been cleaned up when its refcount drops to
	 * zero. */
=======
ksocknal_destroy_peer(struct ksock_peer *peer)
{
	struct ksock_net *net = peer->ksnp_ni->ni_data;

	CDEBUG(D_NET, "peer %s %p deleted\n",
	       libcfs_id2str(peer->ksnp_id), peer);

	LASSERT(!atomic_read(&peer->ksnp_refcount));
	LASSERT(!peer->ksnp_accepting);
	LASSERT(list_empty(&peer->ksnp_conns));
	LASSERT(list_empty(&peer->ksnp_routes));
	LASSERT(list_empty(&peer->ksnp_tx_queue));
	LASSERT(list_empty(&peer->ksnp_zc_req_list));

	LIBCFS_FREE(peer, sizeof(*peer));

	/*
	 * NB a peer's connections and routes keep a reference on their peer
	 * until they are destroyed, so we can be assured that _all_ state to
	 * do with this peer has been cleaned up when its refcount drops to
	 * zero.
	 */
>>>>>>> v4.9.227
	spin_lock_bh(&net->ksnn_lock);
	net->ksnn_npeers--;
	spin_unlock_bh(&net->ksnn_lock);
}

<<<<<<< HEAD
ksock_peer_t *
ksocknal_find_peer_locked (lnet_ni_t *ni, lnet_process_id_t id)
{
	struct list_head       *peer_list = ksocknal_nid2peerlist(id.nid);
	struct list_head       *tmp;
	ksock_peer_t     *peer;

	list_for_each (tmp, peer_list) {

		peer = list_entry (tmp, ksock_peer_t, ksnp_list);

		LASSERT (!peer->ksnp_closing);
=======
struct ksock_peer *
ksocknal_find_peer_locked(lnet_ni_t *ni, lnet_process_id_t id)
{
	struct list_head *peer_list = ksocknal_nid2peerlist(id.nid);
	struct list_head *tmp;
	struct ksock_peer *peer;

	list_for_each(tmp, peer_list) {
		peer = list_entry(tmp, struct ksock_peer, ksnp_list);

		LASSERT(!peer->ksnp_closing);
>>>>>>> v4.9.227

		if (peer->ksnp_ni != ni)
			continue;

		if (peer->ksnp_id.nid != id.nid ||
		    peer->ksnp_id.pid != id.pid)
			continue;

		CDEBUG(D_NET, "got peer [%p] -> %s (%d)\n",
		       peer, libcfs_id2str(id),
		       atomic_read(&peer->ksnp_refcount));
		return peer;
	}
	return NULL;
}

<<<<<<< HEAD
ksock_peer_t *
ksocknal_find_peer (lnet_ni_t *ni, lnet_process_id_t id)
{
	ksock_peer_t     *peer;

	read_lock(&ksocknal_data.ksnd_global_lock);
	peer = ksocknal_find_peer_locked(ni, id);
	if (peer != NULL)			/* +1 ref for caller? */
=======
struct ksock_peer *
ksocknal_find_peer(lnet_ni_t *ni, lnet_process_id_t id)
{
	struct ksock_peer *peer;

	read_lock(&ksocknal_data.ksnd_global_lock);
	peer = ksocknal_find_peer_locked(ni, id);
	if (peer)			/* +1 ref for caller? */
>>>>>>> v4.9.227
		ksocknal_peer_addref(peer);
	read_unlock(&ksocknal_data.ksnd_global_lock);

	return peer;
}

static void
<<<<<<< HEAD
ksocknal_unlink_peer_locked (ksock_peer_t *peer)
{
	int		i;
	__u32	      ip;
	ksock_interface_t *iface;

	for (i = 0; i < peer->ksnp_n_passive_ips; i++) {
		LASSERT (i < LNET_MAX_INTERFACES);
		ip = peer->ksnp_passive_ips[i];

		iface = ksocknal_ip2iface(peer->ksnp_ni, ip);
		/* All IPs in peer->ksnp_passive_ips[] come from the
		 * interface list, therefore the call must succeed. */
		LASSERT (iface != NULL);
=======
ksocknal_unlink_peer_locked(struct ksock_peer *peer)
{
	int i;
	__u32 ip;
	struct ksock_interface *iface;

	for (i = 0; i < peer->ksnp_n_passive_ips; i++) {
		LASSERT(i < LNET_MAX_INTERFACES);
		ip = peer->ksnp_passive_ips[i];

		iface = ksocknal_ip2iface(peer->ksnp_ni, ip);
		/*
		 * All IPs in peer->ksnp_passive_ips[] come from the
		 * interface list, therefore the call must succeed.
		 */
		LASSERT(iface);
>>>>>>> v4.9.227

		CDEBUG(D_NET, "peer=%p iface=%p ksni_nroutes=%d\n",
		       peer, iface, iface->ksni_nroutes);
		iface->ksni_npeers--;
	}

<<<<<<< HEAD
	LASSERT (list_empty(&peer->ksnp_conns));
	LASSERT (list_empty(&peer->ksnp_routes));
	LASSERT (!peer->ksnp_closing);
	peer->ksnp_closing = 1;
	list_del (&peer->ksnp_list);
=======
	LASSERT(list_empty(&peer->ksnp_conns));
	LASSERT(list_empty(&peer->ksnp_routes));
	LASSERT(!peer->ksnp_closing);
	peer->ksnp_closing = 1;
	list_del(&peer->ksnp_list);
>>>>>>> v4.9.227
	/* lose peerlist's ref */
	ksocknal_peer_decref(peer);
}

static int
<<<<<<< HEAD
ksocknal_get_peer_info (lnet_ni_t *ni, int index,
			lnet_process_id_t *id, __u32 *myip, __u32 *peer_ip,
			int *port, int *conn_count, int *share_count)
{
	ksock_peer_t      *peer;
	struct list_head	*ptmp;
	ksock_route_t     *route;
	struct list_head	*rtmp;
	int		i;
	int		j;
	int		rc = -ENOENT;
=======
ksocknal_get_peer_info(lnet_ni_t *ni, int index,
		       lnet_process_id_t *id, __u32 *myip, __u32 *peer_ip,
		       int *port, int *conn_count, int *share_count)
{
	struct ksock_peer *peer;
	struct list_head *ptmp;
	struct ksock_route *route;
	struct list_head *rtmp;
	int i;
	int j;
	int rc = -ENOENT;
>>>>>>> v4.9.227

	read_lock(&ksocknal_data.ksnd_global_lock);

	for (i = 0; i < ksocknal_data.ksnd_peer_hash_size; i++) {
<<<<<<< HEAD

		list_for_each (ptmp, &ksocknal_data.ksnd_peers[i]) {
			peer = list_entry (ptmp, ksock_peer_t, ksnp_list);
=======
		list_for_each(ptmp, &ksocknal_data.ksnd_peers[i]) {
			peer = list_entry(ptmp, struct ksock_peer, ksnp_list);
>>>>>>> v4.9.227

			if (peer->ksnp_ni != ni)
				continue;

<<<<<<< HEAD
			if (peer->ksnp_n_passive_ips == 0 &&
=======
			if (!peer->ksnp_n_passive_ips &&
>>>>>>> v4.9.227
			    list_empty(&peer->ksnp_routes)) {
				if (index-- > 0)
					continue;

				*id = peer->ksnp_id;
				*myip = 0;
				*peer_ip = 0;
				*port = 0;
				*conn_count = 0;
				*share_count = 0;
				rc = 0;
				goto out;
			}

			for (j = 0; j < peer->ksnp_n_passive_ips; j++) {
				if (index-- > 0)
					continue;

				*id = peer->ksnp_id;
				*myip = peer->ksnp_passive_ips[j];
				*peer_ip = 0;
				*port = 0;
				*conn_count = 0;
				*share_count = 0;
				rc = 0;
				goto out;
			}

<<<<<<< HEAD
			list_for_each (rtmp, &peer->ksnp_routes) {
				if (index-- > 0)
					continue;

				route = list_entry(rtmp, ksock_route_t,
						       ksnr_list);
=======
			list_for_each(rtmp, &peer->ksnp_routes) {
				if (index-- > 0)
					continue;

				route = list_entry(rtmp, struct ksock_route,
						   ksnr_list);
>>>>>>> v4.9.227

				*id = peer->ksnp_id;
				*myip = route->ksnr_myipaddr;
				*peer_ip = route->ksnr_ipaddr;
				*port = route->ksnr_port;
				*conn_count = route->ksnr_conn_count;
				*share_count = route->ksnr_share_count;
				rc = 0;
				goto out;
			}
		}
	}
 out:
	read_unlock(&ksocknal_data.ksnd_global_lock);
	return rc;
}

static void
<<<<<<< HEAD
ksocknal_associate_route_conn_locked(ksock_route_t *route, ksock_conn_t *conn)
{
	ksock_peer_t      *peer = route->ksnr_peer;
	int		type = conn->ksnc_type;
	ksock_interface_t *iface;
=======
ksocknal_associate_route_conn_locked(struct ksock_route *route, struct ksock_conn *conn)
{
	struct ksock_peer *peer = route->ksnr_peer;
	int type = conn->ksnc_type;
	struct ksock_interface *iface;
>>>>>>> v4.9.227

	conn->ksnc_route = route;
	ksocknal_route_addref(route);

	if (route->ksnr_myipaddr != conn->ksnc_myipaddr) {
<<<<<<< HEAD
		if (route->ksnr_myipaddr == 0) {
=======
		if (!route->ksnr_myipaddr) {
>>>>>>> v4.9.227
			/* route wasn't bound locally yet (the initial route) */
			CDEBUG(D_NET, "Binding %s %pI4h to %pI4h\n",
			       libcfs_id2str(peer->ksnp_id),
			       &route->ksnr_ipaddr,
			       &conn->ksnc_myipaddr);
		} else {
<<<<<<< HEAD
			CDEBUG(D_NET, "Rebinding %s %pI4h from "
			       "%pI4h to %pI4h\n",
=======
			CDEBUG(D_NET, "Rebinding %s %pI4h from %pI4h to %pI4h\n",
>>>>>>> v4.9.227
			       libcfs_id2str(peer->ksnp_id),
			       &route->ksnr_ipaddr,
			       &route->ksnr_myipaddr,
			       &conn->ksnc_myipaddr);

			iface = ksocknal_ip2iface(route->ksnr_peer->ksnp_ni,
						  route->ksnr_myipaddr);
<<<<<<< HEAD
			if (iface != NULL)
=======
			if (iface)
>>>>>>> v4.9.227
				iface->ksni_nroutes--;
		}
		route->ksnr_myipaddr = conn->ksnc_myipaddr;
		iface = ksocknal_ip2iface(route->ksnr_peer->ksnp_ni,
					  route->ksnr_myipaddr);
<<<<<<< HEAD
		if (iface != NULL)
			iface->ksni_nroutes++;
	}

	route->ksnr_connected |= (1<<type);
	route->ksnr_conn_count++;

	/* Successful connection => further attempts can
	 * proceed immediately */
=======
		if (iface)
			iface->ksni_nroutes++;
	}

	route->ksnr_connected |= (1 << type);
	route->ksnr_conn_count++;

	/*
	 * Successful connection => further attempts can
	 * proceed immediately
	 */
>>>>>>> v4.9.227
	route->ksnr_retry_interval = 0;
}

static void
<<<<<<< HEAD
ksocknal_add_route_locked (ksock_peer_t *peer, ksock_route_t *route)
{
	struct list_head	*tmp;
	ksock_conn_t      *conn;
	ksock_route_t     *route2;

	LASSERT (!peer->ksnp_closing);
	LASSERT (route->ksnr_peer == NULL);
	LASSERT (!route->ksnr_scheduled);
	LASSERT (!route->ksnr_connecting);
	LASSERT (route->ksnr_connected == 0);

	/* LASSERT(unique) */
	list_for_each(tmp, &peer->ksnp_routes) {
		route2 = list_entry(tmp, ksock_route_t, ksnr_list);

		if (route2->ksnr_ipaddr == route->ksnr_ipaddr) {
			CERROR("Duplicate route %s %pI4h\n",
				libcfs_id2str(peer->ksnp_id),
				&route->ksnr_ipaddr);
=======
ksocknal_add_route_locked(struct ksock_peer *peer, struct ksock_route *route)
{
	struct list_head *tmp;
	struct ksock_conn *conn;
	struct ksock_route *route2;

	LASSERT(!peer->ksnp_closing);
	LASSERT(!route->ksnr_peer);
	LASSERT(!route->ksnr_scheduled);
	LASSERT(!route->ksnr_connecting);
	LASSERT(!route->ksnr_connected);

	/* LASSERT(unique) */
	list_for_each(tmp, &peer->ksnp_routes) {
		route2 = list_entry(tmp, struct ksock_route, ksnr_list);

		if (route2->ksnr_ipaddr == route->ksnr_ipaddr) {
			CERROR("Duplicate route %s %pI4h\n",
			       libcfs_id2str(peer->ksnp_id),
			       &route->ksnr_ipaddr);
>>>>>>> v4.9.227
			LBUG();
		}
	}

	route->ksnr_peer = peer;
	ksocknal_peer_addref(peer);
	/* peer's routelist takes over my ref on 'route' */
	list_add_tail(&route->ksnr_list, &peer->ksnp_routes);

	list_for_each(tmp, &peer->ksnp_conns) {
<<<<<<< HEAD
		conn = list_entry(tmp, ksock_conn_t, ksnc_list);
=======
		conn = list_entry(tmp, struct ksock_conn, ksnc_list);
>>>>>>> v4.9.227

		if (conn->ksnc_ipaddr != route->ksnr_ipaddr)
			continue;

		ksocknal_associate_route_conn_locked(route, conn);
		/* keep going (typed routes) */
	}
}

static void
<<<<<<< HEAD
ksocknal_del_route_locked (ksock_route_t *route)
{
	ksock_peer_t      *peer = route->ksnr_peer;
	ksock_interface_t *iface;
	ksock_conn_t      *conn;
	struct list_head	*ctmp;
	struct list_head	*cnxt;

	LASSERT (!route->ksnr_deleted);

	/* Close associated conns */
	list_for_each_safe (ctmp, cnxt, &peer->ksnp_conns) {
		conn = list_entry(ctmp, ksock_conn_t, ksnc_list);
=======
ksocknal_del_route_locked(struct ksock_route *route)
{
	struct ksock_peer *peer = route->ksnr_peer;
	struct ksock_interface *iface;
	struct ksock_conn *conn;
	struct list_head *ctmp;
	struct list_head *cnxt;

	LASSERT(!route->ksnr_deleted);

	/* Close associated conns */
	list_for_each_safe(ctmp, cnxt, &peer->ksnp_conns) {
		conn = list_entry(ctmp, struct ksock_conn, ksnc_list);
>>>>>>> v4.9.227

		if (conn->ksnc_route != route)
			continue;

<<<<<<< HEAD
		ksocknal_close_conn_locked (conn, 0);
	}

	if (route->ksnr_myipaddr != 0) {
		iface = ksocknal_ip2iface(route->ksnr_peer->ksnp_ni,
					  route->ksnr_myipaddr);
		if (iface != NULL)
=======
		ksocknal_close_conn_locked(conn, 0);
	}

	if (route->ksnr_myipaddr) {
		iface = ksocknal_ip2iface(route->ksnr_peer->ksnp_ni,
					  route->ksnr_myipaddr);
		if (iface)
>>>>>>> v4.9.227
			iface->ksni_nroutes--;
	}

	route->ksnr_deleted = 1;
<<<<<<< HEAD
	list_del (&route->ksnr_list);
	ksocknal_route_decref(route);	     /* drop peer's ref */

	if (list_empty (&peer->ksnp_routes) &&
	    list_empty (&peer->ksnp_conns)) {
		/* I've just removed the last route to a peer with no active
		 * connections */
		ksocknal_unlink_peer_locked (peer);
=======
	list_del(&route->ksnr_list);
	ksocknal_route_decref(route);	     /* drop peer's ref */

	if (list_empty(&peer->ksnp_routes) &&
	    list_empty(&peer->ksnp_conns)) {
		/*
		 * I've just removed the last route to a peer with no active
		 * connections
		 */
		ksocknal_unlink_peer_locked(peer);
>>>>>>> v4.9.227
	}
}

int
<<<<<<< HEAD
ksocknal_add_peer (lnet_ni_t *ni, lnet_process_id_t id, __u32 ipaddr, int port)
{
	struct list_head	*tmp;
	ksock_peer_t      *peer;
	ksock_peer_t      *peer2;
	ksock_route_t     *route;
	ksock_route_t     *route2;
	int		rc;
=======
ksocknal_add_peer(lnet_ni_t *ni, lnet_process_id_t id, __u32 ipaddr, int port)
{
	struct list_head *tmp;
	struct ksock_peer *peer;
	struct ksock_peer *peer2;
	struct ksock_route *route;
	struct ksock_route *route2;
	int rc;
>>>>>>> v4.9.227

	if (id.nid == LNET_NID_ANY ||
	    id.pid == LNET_PID_ANY)
		return -EINVAL;

	/* Have a brand new peer ready... */
	rc = ksocknal_create_peer(&peer, ni, id);
<<<<<<< HEAD
	if (rc != 0)
		return rc;

	route = ksocknal_create_route (ipaddr, port);
	if (route == NULL) {
=======
	if (rc)
		return rc;

	route = ksocknal_create_route(ipaddr, port);
	if (!route) {
>>>>>>> v4.9.227
		ksocknal_peer_decref(peer);
		return -ENOMEM;
	}

	write_lock_bh(&ksocknal_data.ksnd_global_lock);

	/* always called with a ref on ni, so shutdown can't have started */
<<<<<<< HEAD
	LASSERT (((ksock_net_t *) ni->ni_data)->ksnn_shutdown == 0);

	peer2 = ksocknal_find_peer_locked (ni, id);
	if (peer2 != NULL) {
=======
	LASSERT(!((struct ksock_net *)ni->ni_data)->ksnn_shutdown);

	peer2 = ksocknal_find_peer_locked(ni, id);
	if (peer2) {
>>>>>>> v4.9.227
		ksocknal_peer_decref(peer);
		peer = peer2;
	} else {
		/* peer table takes my ref on peer */
<<<<<<< HEAD
		list_add_tail (&peer->ksnp_list,
				   ksocknal_nid2peerlist (id.nid));
	}

	route2 = NULL;
	list_for_each (tmp, &peer->ksnp_routes) {
		route2 = list_entry(tmp, ksock_route_t, ksnr_list);
=======
		list_add_tail(&peer->ksnp_list,
			      ksocknal_nid2peerlist(id.nid));
	}

	route2 = NULL;
	list_for_each(tmp, &peer->ksnp_routes) {
		route2 = list_entry(tmp, struct ksock_route, ksnr_list);
>>>>>>> v4.9.227

		if (route2->ksnr_ipaddr == ipaddr)
			break;

		route2 = NULL;
	}
<<<<<<< HEAD
	if (route2 == NULL) {
=======
	if (!route2) {
>>>>>>> v4.9.227
		ksocknal_add_route_locked(peer, route);
		route->ksnr_share_count++;
	} else {
		ksocknal_route_decref(route);
		route2->ksnr_share_count++;
	}

	write_unlock_bh(&ksocknal_data.ksnd_global_lock);

	return 0;
}

static void
<<<<<<< HEAD
ksocknal_del_peer_locked (ksock_peer_t *peer, __u32 ip)
{
	ksock_conn_t     *conn;
	ksock_route_t    *route;
	struct list_head       *tmp;
	struct list_head       *nxt;
	int	       nshared;

	LASSERT (!peer->ksnp_closing);
=======
ksocknal_del_peer_locked(struct ksock_peer *peer, __u32 ip)
{
	struct ksock_conn *conn;
	struct ksock_route *route;
	struct list_head *tmp;
	struct list_head *nxt;
	int nshared;

	LASSERT(!peer->ksnp_closing);
>>>>>>> v4.9.227

	/* Extra ref prevents peer disappearing until I'm done with it */
	ksocknal_peer_addref(peer);

<<<<<<< HEAD
	list_for_each_safe (tmp, nxt, &peer->ksnp_routes) {
		route = list_entry(tmp, ksock_route_t, ksnr_list);

		/* no match */
		if (!(ip == 0 || route->ksnr_ipaddr == ip))
=======
	list_for_each_safe(tmp, nxt, &peer->ksnp_routes) {
		route = list_entry(tmp, struct ksock_route, ksnr_list);

		/* no match */
		if (!(!ip || route->ksnr_ipaddr == ip))
>>>>>>> v4.9.227
			continue;

		route->ksnr_share_count = 0;
		/* This deletes associated conns too */
<<<<<<< HEAD
		ksocknal_del_route_locked (route);
	}

	nshared = 0;
	list_for_each_safe (tmp, nxt, &peer->ksnp_routes) {
		route = list_entry(tmp, ksock_route_t, ksnr_list);
		nshared += route->ksnr_share_count;
	}

	if (nshared == 0) {
		/* remove everything else if there are no explicit entries
		 * left */

		list_for_each_safe (tmp, nxt, &peer->ksnp_routes) {
			route = list_entry(tmp, ksock_route_t, ksnr_list);

			/* we should only be removing auto-entries */
			LASSERT(route->ksnr_share_count == 0);
			ksocknal_del_route_locked (route);
		}

		list_for_each_safe (tmp, nxt, &peer->ksnp_conns) {
			conn = list_entry(tmp, ksock_conn_t, ksnc_list);
=======
		ksocknal_del_route_locked(route);
	}

	nshared = 0;
	list_for_each_safe(tmp, nxt, &peer->ksnp_routes) {
		route = list_entry(tmp, struct ksock_route, ksnr_list);
		nshared += route->ksnr_share_count;
	}

	if (!nshared) {
		/*
		 * remove everything else if there are no explicit entries
		 * left
		 */
		list_for_each_safe(tmp, nxt, &peer->ksnp_routes) {
			route = list_entry(tmp, struct ksock_route, ksnr_list);

			/* we should only be removing auto-entries */
			LASSERT(!route->ksnr_share_count);
			ksocknal_del_route_locked(route);
		}

		list_for_each_safe(tmp, nxt, &peer->ksnp_conns) {
			conn = list_entry(tmp, struct ksock_conn, ksnc_list);
>>>>>>> v4.9.227

			ksocknal_close_conn_locked(conn, 0);
		}
	}

	ksocknal_peer_decref(peer);
	/* NB peer unlinks itself when last conn/route is removed */
}

static int
<<<<<<< HEAD
ksocknal_del_peer (lnet_ni_t *ni, lnet_process_id_t id, __u32 ip)
{
	LIST_HEAD     (zombies);
	struct list_head	*ptmp;
	struct list_head	*pnxt;
	ksock_peer_t      *peer;
	int		lo;
	int		hi;
	int		i;
	int		rc = -ENOENT;

	write_lock_bh(&ksocknal_data.ksnd_global_lock);

	if (id.nid != LNET_NID_ANY)
		lo = hi = (int)(ksocknal_nid2peerlist(id.nid) - ksocknal_data.ksnd_peers);
	else {
=======
ksocknal_del_peer(lnet_ni_t *ni, lnet_process_id_t id, __u32 ip)
{
	LIST_HEAD(zombies);
	struct list_head *ptmp;
	struct list_head *pnxt;
	struct ksock_peer *peer;
	int lo;
	int hi;
	int i;
	int rc = -ENOENT;

	write_lock_bh(&ksocknal_data.ksnd_global_lock);

	if (id.nid != LNET_NID_ANY) {
		lo = (int)(ksocknal_nid2peerlist(id.nid) - ksocknal_data.ksnd_peers);
		hi = (int)(ksocknal_nid2peerlist(id.nid) - ksocknal_data.ksnd_peers);
	} else {
>>>>>>> v4.9.227
		lo = 0;
		hi = ksocknal_data.ksnd_peer_hash_size - 1;
	}

	for (i = lo; i <= hi; i++) {
<<<<<<< HEAD
		list_for_each_safe (ptmp, pnxt,
					&ksocknal_data.ksnd_peers[i]) {
			peer = list_entry (ptmp, ksock_peer_t, ksnp_list);
=======
		list_for_each_safe(ptmp, pnxt, &ksocknal_data.ksnd_peers[i]) {
			peer = list_entry(ptmp, struct ksock_peer, ksnp_list);
>>>>>>> v4.9.227

			if (peer->ksnp_ni != ni)
				continue;

			if (!((id.nid == LNET_NID_ANY || peer->ksnp_id.nid == id.nid) &&
			      (id.pid == LNET_PID_ANY || peer->ksnp_id.pid == id.pid)))
				continue;

			ksocknal_peer_addref(peer);     /* a ref for me... */

<<<<<<< HEAD
			ksocknal_del_peer_locked (peer, ip);

			if (peer->ksnp_closing &&
			    !list_empty(&peer->ksnp_tx_queue)) {
				LASSERT (list_empty(&peer->ksnp_conns));
				LASSERT (list_empty(&peer->ksnp_routes));

				list_splice_init(&peer->ksnp_tx_queue,
						     &zombies);
=======
			ksocknal_del_peer_locked(peer, ip);

			if (peer->ksnp_closing &&
			    !list_empty(&peer->ksnp_tx_queue)) {
				LASSERT(list_empty(&peer->ksnp_conns));
				LASSERT(list_empty(&peer->ksnp_routes));

				list_splice_init(&peer->ksnp_tx_queue,
						 &zombies);
>>>>>>> v4.9.227
			}

			ksocknal_peer_decref(peer);     /* ...till here */

			rc = 0;		 /* matched! */
		}
	}

	write_unlock_bh(&ksocknal_data.ksnd_global_lock);

	ksocknal_txlist_done(ni, &zombies, 1);

	return rc;
}

<<<<<<< HEAD
static ksock_conn_t *
ksocknal_get_conn_by_idx (lnet_ni_t *ni, int index)
{
	ksock_peer_t      *peer;
	struct list_head	*ptmp;
	ksock_conn_t      *conn;
	struct list_head	*ctmp;
	int		i;
=======
static struct ksock_conn *
ksocknal_get_conn_by_idx(lnet_ni_t *ni, int index)
{
	struct ksock_peer *peer;
	struct list_head *ptmp;
	struct ksock_conn *conn;
	struct list_head *ctmp;
	int i;
>>>>>>> v4.9.227

	read_lock(&ksocknal_data.ksnd_global_lock);

	for (i = 0; i < ksocknal_data.ksnd_peer_hash_size; i++) {
<<<<<<< HEAD
		list_for_each (ptmp, &ksocknal_data.ksnd_peers[i]) {
			peer = list_entry (ptmp, ksock_peer_t, ksnp_list);

			LASSERT (!peer->ksnp_closing);
=======
		list_for_each(ptmp, &ksocknal_data.ksnd_peers[i]) {
			peer = list_entry(ptmp, struct ksock_peer, ksnp_list);

			LASSERT(!peer->ksnp_closing);
>>>>>>> v4.9.227

			if (peer->ksnp_ni != ni)
				continue;

<<<<<<< HEAD
			list_for_each (ctmp, &peer->ksnp_conns) {
				if (index-- > 0)
					continue;

				conn = list_entry (ctmp, ksock_conn_t,
						       ksnc_list);
=======
			list_for_each(ctmp, &peer->ksnp_conns) {
				if (index-- > 0)
					continue;

				conn = list_entry(ctmp, struct ksock_conn,
						  ksnc_list);
>>>>>>> v4.9.227
				ksocknal_conn_addref(conn);
				read_unlock(&ksocknal_data.ksnd_global_lock);
				return conn;
			}
		}
	}

	read_unlock(&ksocknal_data.ksnd_global_lock);
	return NULL;
}

<<<<<<< HEAD
static ksock_sched_t *
ksocknal_choose_scheduler_locked(unsigned int cpt)
{
	struct ksock_sched_info	*info = ksocknal_data.ksnd_sched_info[cpt];
	ksock_sched_t		*sched;
	int			i;
=======
static struct ksock_sched *
ksocknal_choose_scheduler_locked(unsigned int cpt)
{
	struct ksock_sched_info	*info = ksocknal_data.ksnd_sched_info[cpt];
	struct ksock_sched *sched;
	int i;
>>>>>>> v4.9.227

	LASSERT(info->ksi_nthreads > 0);

	sched = &info->ksi_scheds[0];
	/*
	 * NB: it's safe so far, but info->ksi_nthreads could be changed
	 * at runtime when we have dynamic LNet configuration, then we
	 * need to take care of this.
	 */
	for (i = 1; i < info->ksi_nthreads; i++) {
		if (sched->kss_nconns > info->ksi_scheds[i].kss_nconns)
			sched = &info->ksi_scheds[i];
	}

	return sched;
}

static int
<<<<<<< HEAD
ksocknal_local_ipvec (lnet_ni_t *ni, __u32 *ipaddrs)
{
	ksock_net_t       *net = ni->ni_data;
	int		i;
	int		nip;
=======
ksocknal_local_ipvec(lnet_ni_t *ni, __u32 *ipaddrs)
{
	struct ksock_net *net = ni->ni_data;
	int i;
	int nip;
>>>>>>> v4.9.227

	read_lock(&ksocknal_data.ksnd_global_lock);

	nip = net->ksnn_ninterfaces;
<<<<<<< HEAD
	LASSERT (nip <= LNET_MAX_INTERFACES);

	/* Only offer interfaces for additional connections if I have
	 * more than one. */
=======
	LASSERT(nip <= LNET_MAX_INTERFACES);

	/*
	 * Only offer interfaces for additional connections if I have
	 * more than one.
	 */
>>>>>>> v4.9.227
	if (nip < 2) {
		read_unlock(&ksocknal_data.ksnd_global_lock);
		return 0;
	}

	for (i = 0; i < nip; i++) {
		ipaddrs[i] = net->ksnn_interfaces[i].ksni_ipaddr;
<<<<<<< HEAD
		LASSERT (ipaddrs[i] != 0);
=======
		LASSERT(ipaddrs[i]);
>>>>>>> v4.9.227
	}

	read_unlock(&ksocknal_data.ksnd_global_lock);
	return nip;
}

static int
<<<<<<< HEAD
ksocknal_match_peerip (ksock_interface_t *iface, __u32 *ips, int nips)
{
	int   best_netmatch = 0;
	int   best_xor      = 0;
	int   best	  = -1;
	int   this_xor;
	int   this_netmatch;
	int   i;

	for (i = 0; i < nips; i++) {
		if (ips[i] == 0)
			continue;

		this_xor = (ips[i] ^ iface->ksni_ipaddr);
		this_netmatch = ((this_xor & iface->ksni_netmask) == 0) ? 1 : 0;
=======
ksocknal_match_peerip(struct ksock_interface *iface, __u32 *ips, int nips)
{
	int best_netmatch = 0;
	int best_xor      = 0;
	int best	  = -1;
	int this_xor;
	int this_netmatch;
	int i;

	for (i = 0; i < nips; i++) {
		if (!ips[i])
			continue;

		this_xor = ips[i] ^ iface->ksni_ipaddr;
		this_netmatch = !(this_xor & iface->ksni_netmask) ? 1 : 0;
>>>>>>> v4.9.227

		if (!(best < 0 ||
		      best_netmatch < this_netmatch ||
		      (best_netmatch == this_netmatch &&
		       best_xor > this_xor)))
			continue;

		best = i;
		best_netmatch = this_netmatch;
		best_xor = this_xor;
	}

<<<<<<< HEAD
	LASSERT (best >= 0);
=======
	LASSERT(best >= 0);
>>>>>>> v4.9.227
	return best;
}

static int
<<<<<<< HEAD
ksocknal_select_ips(ksock_peer_t *peer, __u32 *peerips, int n_peerips)
{
	rwlock_t		*global_lock = &ksocknal_data.ksnd_global_lock;
	ksock_net_t	*net = peer->ksnp_ni->ni_data;
	ksock_interface_t  *iface;
	ksock_interface_t  *best_iface;
	int		 n_ips;
	int		 i;
	int		 j;
	int		 k;
	__u32	       ip;
	__u32	       xor;
	int		 this_netmatch;
	int		 best_netmatch;
	int		 best_npeers;

	/* CAVEAT EMPTOR: We do all our interface matching with an
	 * exclusive hold of global lock at IRQ priority.  We're only
	 * expecting to be dealing with small numbers of interfaces, so the
	 * O(n**3)-ness shouldn't matter */

	/* Also note that I'm not going to return more than n_peerips
	 * interfaces, even if I have more myself */

	write_lock_bh(global_lock);

	LASSERT (n_peerips <= LNET_MAX_INTERFACES);
	LASSERT (net->ksnn_ninterfaces <= LNET_MAX_INTERFACES);

	/* Only match interfaces for additional connections
	 * if I have > 1 interface */
	n_ips = (net->ksnn_ninterfaces < 2) ? 0 :
		MIN(n_peerips, net->ksnn_ninterfaces);
=======
ksocknal_select_ips(struct ksock_peer *peer, __u32 *peerips, int n_peerips)
{
	rwlock_t *global_lock = &ksocknal_data.ksnd_global_lock;
	struct ksock_net *net = peer->ksnp_ni->ni_data;
	struct ksock_interface *iface;
	struct ksock_interface *best_iface;
	int n_ips;
	int i;
	int j;
	int k;
	__u32 ip;
	__u32 xor;
	int this_netmatch;
	int best_netmatch;
	int best_npeers;

	/*
	 * CAVEAT EMPTOR: We do all our interface matching with an
	 * exclusive hold of global lock at IRQ priority.  We're only
	 * expecting to be dealing with small numbers of interfaces, so the
	 * O(n**3)-ness shouldn't matter
	 */
	/*
	 * Also note that I'm not going to return more than n_peerips
	 * interfaces, even if I have more myself
	 */
	write_lock_bh(global_lock);

	LASSERT(n_peerips <= LNET_MAX_INTERFACES);
	LASSERT(net->ksnn_ninterfaces <= LNET_MAX_INTERFACES);

	/*
	 * Only match interfaces for additional connections
	 * if I have > 1 interface
	 */
	n_ips = (net->ksnn_ninterfaces < 2) ? 0 :
		min(n_peerips, net->ksnn_ninterfaces);
>>>>>>> v4.9.227

	for (i = 0; peer->ksnp_n_passive_ips < n_ips; i++) {
		/*	      ^ yes really... */

<<<<<<< HEAD
		/* If we have any new interfaces, first tick off all the
		 * peer IPs that match old interfaces, then choose new
		 * interfaces to match the remaining peer IPS.
		 * We don't forget interfaces we've stopped using; we might
		 * start using them again... */

=======
		/*
		 * If we have any new interfaces, first tick off all the
		 * peer IPs that match old interfaces, then choose new
		 * interfaces to match the remaining peer IPS.
		 * We don't forget interfaces we've stopped using; we might
		 * start using them again...
		 */
>>>>>>> v4.9.227
		if (i < peer->ksnp_n_passive_ips) {
			/* Old interface. */
			ip = peer->ksnp_passive_ips[i];
			best_iface = ksocknal_ip2iface(peer->ksnp_ni, ip);

<<<<<<< HEAD
		} else {
			/* choose a new interface */
			LASSERT (i == peer->ksnp_n_passive_ips);
=======
			/* peer passive ips are kept up to date */
			LASSERT(best_iface);
		} else {
			/* choose a new interface */
			LASSERT(i == peer->ksnp_n_passive_ips);
>>>>>>> v4.9.227

			best_iface = NULL;
			best_netmatch = 0;
			best_npeers = 0;

			for (j = 0; j < net->ksnn_ninterfaces; j++) {
				iface = &net->ksnn_interfaces[j];
				ip = iface->ksni_ipaddr;

				for (k = 0; k < peer->ksnp_n_passive_ips; k++)
					if (peer->ksnp_passive_ips[k] == ip)
						break;

				if (k < peer->ksnp_n_passive_ips) /* using it already */
					continue;

				k = ksocknal_match_peerip(iface, peerips, n_peerips);
<<<<<<< HEAD
				xor = (ip ^ peerips[k]);
				this_netmatch = ((xor & iface->ksni_netmask) == 0) ? 1 : 0;

				if (!(best_iface == NULL ||
=======
				xor = ip ^ peerips[k];
				this_netmatch = !(xor & iface->ksni_netmask) ? 1 : 0;

				if (!(!best_iface ||
>>>>>>> v4.9.227
				      best_netmatch < this_netmatch ||
				      (best_netmatch == this_netmatch &&
				       best_npeers > iface->ksni_npeers)))
					continue;

				best_iface = iface;
				best_netmatch = this_netmatch;
				best_npeers = iface->ksni_npeers;
			}

<<<<<<< HEAD
			best_iface->ksni_npeers++;
			ip = best_iface->ksni_ipaddr;
			peer->ksnp_passive_ips[i] = ip;
			peer->ksnp_n_passive_ips = i+1;
=======
			LASSERT(best_iface);

			best_iface->ksni_npeers++;
			ip = best_iface->ksni_ipaddr;
			peer->ksnp_passive_ips[i] = ip;
			peer->ksnp_n_passive_ips = i + 1;
>>>>>>> v4.9.227
		}

		/* mark the best matching peer IP used */
		j = ksocknal_match_peerip(best_iface, peerips, n_peerips);
		peerips[j] = 0;
	}

	/* Overwrite input peer IP addresses */
	memcpy(peerips, peer->ksnp_passive_ips, n_ips * sizeof(*peerips));

	write_unlock_bh(global_lock);

	return n_ips;
}

static void
<<<<<<< HEAD
ksocknal_create_routes(ksock_peer_t *peer, int port,
		       __u32 *peer_ipaddrs, int npeer_ipaddrs)
{
	ksock_route_t       *newroute = NULL;
	rwlock_t		*global_lock = &ksocknal_data.ksnd_global_lock;
	lnet_ni_t	   *ni = peer->ksnp_ni;
	ksock_net_t	 *net = ni->ni_data;
	struct list_head	  *rtmp;
	ksock_route_t       *route;
	ksock_interface_t   *iface;
	ksock_interface_t   *best_iface;
	int		  best_netmatch;
	int		  this_netmatch;
	int		  best_nroutes;
	int		  i;
	int		  j;

	/* CAVEAT EMPTOR: We do all our interface matching with an
	 * exclusive hold of global lock at IRQ priority.  We're only
	 * expecting to be dealing with small numbers of interfaces, so the
	 * O(n**3)-ness here shouldn't matter */

	write_lock_bh(global_lock);

	if (net->ksnn_ninterfaces < 2) {
		/* Only create additional connections
		 * if I have > 1 interface */
=======
ksocknal_create_routes(struct ksock_peer *peer, int port,
		       __u32 *peer_ipaddrs, int npeer_ipaddrs)
{
	struct ksock_route *newroute = NULL;
	rwlock_t *global_lock = &ksocknal_data.ksnd_global_lock;
	lnet_ni_t *ni = peer->ksnp_ni;
	struct ksock_net *net = ni->ni_data;
	struct list_head *rtmp;
	struct ksock_route *route;
	struct ksock_interface *iface;
	struct ksock_interface *best_iface;
	int best_netmatch;
	int this_netmatch;
	int best_nroutes;
	int i;
	int j;

	/*
	 * CAVEAT EMPTOR: We do all our interface matching with an
	 * exclusive hold of global lock at IRQ priority.  We're only
	 * expecting to be dealing with small numbers of interfaces, so the
	 * O(n**3)-ness here shouldn't matter
	 */
	write_lock_bh(global_lock);

	if (net->ksnn_ninterfaces < 2) {
		/*
		 * Only create additional connections
		 * if I have > 1 interface
		 */
>>>>>>> v4.9.227
		write_unlock_bh(global_lock);
		return;
	}

<<<<<<< HEAD
	LASSERT (npeer_ipaddrs <= LNET_MAX_INTERFACES);

	for (i = 0; i < npeer_ipaddrs; i++) {
		if (newroute != NULL) {
=======
	LASSERT(npeer_ipaddrs <= LNET_MAX_INTERFACES);

	for (i = 0; i < npeer_ipaddrs; i++) {
		if (newroute) {
>>>>>>> v4.9.227
			newroute->ksnr_ipaddr = peer_ipaddrs[i];
		} else {
			write_unlock_bh(global_lock);

			newroute = ksocknal_create_route(peer_ipaddrs[i], port);
<<<<<<< HEAD
			if (newroute == NULL)
=======
			if (!newroute)
>>>>>>> v4.9.227
				return;

			write_lock_bh(global_lock);
		}

		if (peer->ksnp_closing) {
			/* peer got closed under me */
			break;
		}

		/* Already got a route? */
		route = NULL;
		list_for_each(rtmp, &peer->ksnp_routes) {
<<<<<<< HEAD
			route = list_entry(rtmp, ksock_route_t, ksnr_list);
=======
			route = list_entry(rtmp, struct ksock_route, ksnr_list);
>>>>>>> v4.9.227

			if (route->ksnr_ipaddr == newroute->ksnr_ipaddr)
				break;

			route = NULL;
		}
<<<<<<< HEAD
		if (route != NULL)
=======
		if (route)
>>>>>>> v4.9.227
			continue;

		best_iface = NULL;
		best_nroutes = 0;
		best_netmatch = 0;

<<<<<<< HEAD
		LASSERT (net->ksnn_ninterfaces <= LNET_MAX_INTERFACES);
=======
		LASSERT(net->ksnn_ninterfaces <= LNET_MAX_INTERFACES);
>>>>>>> v4.9.227

		/* Select interface to connect from */
		for (j = 0; j < net->ksnn_ninterfaces; j++) {
			iface = &net->ksnn_interfaces[j];

			/* Using this interface already? */
			list_for_each(rtmp, &peer->ksnp_routes) {
<<<<<<< HEAD
				route = list_entry(rtmp, ksock_route_t,
						       ksnr_list);
=======
				route = list_entry(rtmp, struct ksock_route,
						   ksnr_list);
>>>>>>> v4.9.227

				if (route->ksnr_myipaddr == iface->ksni_ipaddr)
					break;

				route = NULL;
			}
<<<<<<< HEAD
			if (route != NULL)
				continue;

			this_netmatch = (((iface->ksni_ipaddr ^
					   newroute->ksnr_ipaddr) &
					   iface->ksni_netmask) == 0) ? 1 : 0;

			if (!(best_iface == NULL ||
=======
			if (route)
				continue;

			this_netmatch = (!((iface->ksni_ipaddr ^
					   newroute->ksnr_ipaddr) &
					   iface->ksni_netmask)) ? 1 : 0;

			if (!(!best_iface ||
>>>>>>> v4.9.227
			      best_netmatch < this_netmatch ||
			      (best_netmatch == this_netmatch &&
			       best_nroutes > iface->ksni_nroutes)))
				continue;

			best_iface = iface;
			best_netmatch = this_netmatch;
			best_nroutes = iface->ksni_nroutes;
		}

<<<<<<< HEAD
		if (best_iface == NULL)
=======
		if (!best_iface)
>>>>>>> v4.9.227
			continue;

		newroute->ksnr_myipaddr = best_iface->ksni_ipaddr;
		best_iface->ksni_nroutes++;

		ksocknal_add_route_locked(peer, newroute);
		newroute = NULL;
	}

	write_unlock_bh(global_lock);
<<<<<<< HEAD
	if (newroute != NULL)
=======
	if (newroute)
>>>>>>> v4.9.227
		ksocknal_route_decref(newroute);
}

int
<<<<<<< HEAD
ksocknal_accept (lnet_ni_t *ni, struct socket *sock)
{
	ksock_connreq_t    *cr;
	int		 rc;
	__u32	       peer_ip;
	int		 peer_port;

	rc = libcfs_sock_getaddr(sock, 1, &peer_ip, &peer_port);
	LASSERT (rc == 0);		      /* we succeeded before */

	LIBCFS_ALLOC(cr, sizeof(*cr));
	if (cr == NULL) {
		LCONSOLE_ERROR_MSG(0x12f, "Dropping connection request from "
				   "%pI4h: memory exhausted\n",
=======
ksocknal_accept(lnet_ni_t *ni, struct socket *sock)
{
	struct ksock_connreq *cr;
	int rc;
	__u32 peer_ip;
	int peer_port;

	rc = lnet_sock_getaddr(sock, 1, &peer_ip, &peer_port);
	LASSERT(!rc);		      /* we succeeded before */

	LIBCFS_ALLOC(cr, sizeof(*cr));
	if (!cr) {
		LCONSOLE_ERROR_MSG(0x12f, "Dropping connection request from %pI4h: memory exhausted\n",
>>>>>>> v4.9.227
				   &peer_ip);
		return -ENOMEM;
	}

	lnet_ni_addref(ni);
	cr->ksncr_ni   = ni;
	cr->ksncr_sock = sock;

	spin_lock_bh(&ksocknal_data.ksnd_connd_lock);

	list_add_tail(&cr->ksncr_list, &ksocknal_data.ksnd_connd_connreqs);
	wake_up(&ksocknal_data.ksnd_connd_waitq);

	spin_unlock_bh(&ksocknal_data.ksnd_connd_lock);
	return 0;
}

static int
<<<<<<< HEAD
ksocknal_connecting (ksock_peer_t *peer, __u32 ipaddr)
{
	ksock_route_t   *route;

	list_for_each_entry (route, &peer->ksnp_routes, ksnr_list) {

=======
ksocknal_connecting(struct ksock_peer *peer, __u32 ipaddr)
{
	struct ksock_route *route;

	list_for_each_entry(route, &peer->ksnp_routes, ksnr_list) {
>>>>>>> v4.9.227
		if (route->ksnr_ipaddr == ipaddr)
			return route->ksnr_connecting;
	}
	return 0;
}

int
<<<<<<< HEAD
ksocknal_create_conn (lnet_ni_t *ni, ksock_route_t *route,
		      struct socket *sock, int type)
{
	rwlock_t		*global_lock = &ksocknal_data.ksnd_global_lock;
	LIST_HEAD     (zombies);
	lnet_process_id_t  peerid;
	struct list_head	*tmp;
	__u64	      incarnation;
	ksock_conn_t      *conn;
	ksock_conn_t      *conn2;
	ksock_peer_t      *peer = NULL;
	ksock_peer_t      *peer2;
	ksock_sched_t     *sched;
	ksock_hello_msg_t *hello;
	int		   cpt;
	ksock_tx_t	*tx;
	ksock_tx_t	*txtmp;
	int		rc;
	int		active;
	char	      *warn = NULL;

	active = (route != NULL);

	LASSERT (active == (type != SOCKLND_CONN_NONE));

	LIBCFS_ALLOC(conn, sizeof(*conn));
	if (conn == NULL) {
=======
ksocknal_create_conn(lnet_ni_t *ni, struct ksock_route *route,
		     struct socket *sock, int type)
{
	rwlock_t *global_lock = &ksocknal_data.ksnd_global_lock;
	LIST_HEAD(zombies);
	lnet_process_id_t peerid;
	struct list_head *tmp;
	__u64 incarnation;
	struct ksock_conn *conn;
	struct ksock_conn *conn2;
	struct ksock_peer *peer = NULL;
	struct ksock_peer *peer2;
	struct ksock_sched *sched;
	ksock_hello_msg_t *hello;
	int cpt;
	struct ksock_tx *tx;
	struct ksock_tx *txtmp;
	int rc;
	int active;
	char *warn = NULL;

	active = !!route;

	LASSERT(active == (type != SOCKLND_CONN_NONE));

	LIBCFS_ALLOC(conn, sizeof(*conn));
	if (!conn) {
>>>>>>> v4.9.227
		rc = -ENOMEM;
		goto failed_0;
	}

	conn->ksnc_peer = NULL;
	conn->ksnc_route = NULL;
	conn->ksnc_sock = sock;
<<<<<<< HEAD
	/* 2 ref, 1 for conn, another extra ref prevents socket
	 * being closed before establishment of connection */
	atomic_set (&conn->ksnc_sock_refcount, 2);
	conn->ksnc_type = type;
	ksocknal_lib_save_callback(sock, conn);
	atomic_set (&conn->ksnc_conn_refcount, 1); /* 1 ref for me */
=======
	/*
	 * 2 ref, 1 for conn, another extra ref prevents socket
	 * being closed before establishment of connection
	 */
	atomic_set(&conn->ksnc_sock_refcount, 2);
	conn->ksnc_type = type;
	ksocknal_lib_save_callback(sock, conn);
	atomic_set(&conn->ksnc_conn_refcount, 1); /* 1 ref for me */
>>>>>>> v4.9.227

	conn->ksnc_rx_ready = 0;
	conn->ksnc_rx_scheduled = 0;

<<<<<<< HEAD
	INIT_LIST_HEAD (&conn->ksnc_tx_queue);
	conn->ksnc_tx_ready = 0;
	conn->ksnc_tx_scheduled = 0;
	conn->ksnc_tx_carrier = NULL;
	atomic_set (&conn->ksnc_tx_nob, 0);

	LIBCFS_ALLOC(hello, offsetof(ksock_hello_msg_t,
				     kshm_ips[LNET_MAX_INTERFACES]));
	if (hello == NULL) {
=======
	INIT_LIST_HEAD(&conn->ksnc_tx_queue);
	conn->ksnc_tx_ready = 0;
	conn->ksnc_tx_scheduled = 0;
	conn->ksnc_tx_carrier = NULL;
	atomic_set(&conn->ksnc_tx_nob, 0);

	LIBCFS_ALLOC(hello, offsetof(ksock_hello_msg_t,
				     kshm_ips[LNET_MAX_INTERFACES]));
	if (!hello) {
>>>>>>> v4.9.227
		rc = -ENOMEM;
		goto failed_1;
	}

	/* stash conn's local and remote addrs */
<<<<<<< HEAD
	rc = ksocknal_lib_get_conn_addrs (conn);
	if (rc != 0)
		goto failed_1;

	/* Find out/confirm peer's NID and connection type and get the
	 * vector of interfaces she's willing to let me connect to.
	 * Passive connections use the listener timeout since the peer sends
	 * eagerly */

=======
	rc = ksocknal_lib_get_conn_addrs(conn);
	if (rc)
		goto failed_1;

	/*
	 * Find out/confirm peer's NID and connection type and get the
	 * vector of interfaces she's willing to let me connect to.
	 * Passive connections use the listener timeout since the peer sends
	 * eagerly
	 */
>>>>>>> v4.9.227
	if (active) {
		peer = route->ksnr_peer;
		LASSERT(ni == peer->ksnp_ni);

		/* Active connection sends HELLO eagerly */
		hello->kshm_nips = ksocknal_local_ipvec(ni, hello->kshm_ips);
		peerid = peer->ksnp_id;

		write_lock_bh(global_lock);
		conn->ksnc_proto = peer->ksnp_proto;
		write_unlock_bh(global_lock);

<<<<<<< HEAD
		if (conn->ksnc_proto == NULL) {
=======
		if (!conn->ksnc_proto) {
>>>>>>> v4.9.227
			 conn->ksnc_proto = &ksocknal_protocol_v3x;
#if SOCKNAL_VERSION_DEBUG
			 if (*ksocknal_tunables.ksnd_protocol == 2)
				 conn->ksnc_proto = &ksocknal_protocol_v2x;
			 else if (*ksocknal_tunables.ksnd_protocol == 1)
				 conn->ksnc_proto = &ksocknal_protocol_v1x;
#endif
		}

<<<<<<< HEAD
		rc = ksocknal_send_hello (ni, conn, peerid.nid, hello);
		if (rc != 0)
=======
		rc = ksocknal_send_hello(ni, conn, peerid.nid, hello);
		if (rc)
>>>>>>> v4.9.227
			goto failed_1;
	} else {
		peerid.nid = LNET_NID_ANY;
		peerid.pid = LNET_PID_ANY;

		/* Passive, get protocol from peer */
		conn->ksnc_proto = NULL;
	}

<<<<<<< HEAD
	rc = ksocknal_recv_hello (ni, conn, hello, &peerid, &incarnation);
	if (rc < 0)
		goto failed_1;

	LASSERT (rc == 0 || active);
	LASSERT (conn->ksnc_proto != NULL);
	LASSERT (peerid.nid != LNET_NID_ANY);
=======
	rc = ksocknal_recv_hello(ni, conn, hello, &peerid, &incarnation);
	if (rc < 0)
		goto failed_1;

	LASSERT(!rc || active);
	LASSERT(conn->ksnc_proto);
	LASSERT(peerid.nid != LNET_NID_ANY);
>>>>>>> v4.9.227

	cpt = lnet_cpt_of_nid(peerid.nid);

	if (active) {
		ksocknal_peer_addref(peer);
		write_lock_bh(global_lock);
	} else {
		rc = ksocknal_create_peer(&peer, ni, peerid);
<<<<<<< HEAD
		if (rc != 0)
=======
		if (rc)
>>>>>>> v4.9.227
			goto failed_1;

		write_lock_bh(global_lock);

		/* called with a ref on ni, so shutdown can't have started */
<<<<<<< HEAD
		LASSERT (((ksock_net_t *) ni->ni_data)->ksnn_shutdown == 0);

		peer2 = ksocknal_find_peer_locked(ni, peerid);
		if (peer2 == NULL) {
			/* NB this puts an "empty" peer in the peer
			 * table (which takes my ref) */
			list_add_tail(&peer->ksnp_list,
					  ksocknal_nid2peerlist(peerid.nid));
=======
		LASSERT(!((struct ksock_net *)ni->ni_data)->ksnn_shutdown);

		peer2 = ksocknal_find_peer_locked(ni, peerid);
		if (!peer2) {
			/*
			 * NB this puts an "empty" peer in the peer
			 * table (which takes my ref)
			 */
			list_add_tail(&peer->ksnp_list,
				      ksocknal_nid2peerlist(peerid.nid));
>>>>>>> v4.9.227
		} else {
			ksocknal_peer_decref(peer);
			peer = peer2;
		}

		/* +1 ref for me */
		ksocknal_peer_addref(peer);
		peer->ksnp_accepting++;

<<<<<<< HEAD
		/* Am I already connecting to this guy?  Resolve in
		 * favour of higher NID... */
=======
		/*
		 * Am I already connecting to this guy?  Resolve in
		 * favour of higher NID...
		 */
>>>>>>> v4.9.227
		if (peerid.nid < ni->ni_nid &&
		    ksocknal_connecting(peer, conn->ksnc_ipaddr)) {
			rc = EALREADY;
			warn = "connection race resolution";
			goto failed_2;
		}
	}

	if (peer->ksnp_closing ||
	    (active && route->ksnr_deleted)) {
		/* peer/route got closed under me */
		rc = -ESTALE;
		warn = "peer/route removed";
		goto failed_2;
	}

<<<<<<< HEAD
	if (peer->ksnp_proto == NULL) {
		/* Never connected before.
		 * NB recv_hello may have returned EPROTO to signal my peer
		 * wants a different protocol than the one I asked for.
		 */
		LASSERT (list_empty(&peer->ksnp_conns));
=======
	if (!peer->ksnp_proto) {
		/*
		 * Never connected before.
		 * NB recv_hello may have returned EPROTO to signal my peer
		 * wants a different protocol than the one I asked for.
		 */
		LASSERT(list_empty(&peer->ksnp_conns));
>>>>>>> v4.9.227

		peer->ksnp_proto = conn->ksnc_proto;
		peer->ksnp_incarnation = incarnation;
	}

	if (peer->ksnp_proto != conn->ksnc_proto ||
	    peer->ksnp_incarnation != incarnation) {
		/* Peer rebooted or I've got the wrong protocol version */
		ksocknal_close_peer_conns_locked(peer, 0, 0);

		peer->ksnp_proto = NULL;
		rc = ESTALE;
		warn = peer->ksnp_incarnation != incarnation ?
		       "peer rebooted" :
		       "wrong proto version";
		goto failed_2;
	}

	switch (rc) {
	default:
		LBUG();
	case 0:
		break;
	case EALREADY:
		warn = "lost conn race";
		goto failed_2;
	case EPROTO:
		warn = "retry with different protocol version";
		goto failed_2;
	}

<<<<<<< HEAD
	/* Refuse to duplicate an existing connection, unless this is a
	 * loopback connection */
	if (conn->ksnc_ipaddr != conn->ksnc_myipaddr) {
		list_for_each(tmp, &peer->ksnp_conns) {
			conn2 = list_entry(tmp, ksock_conn_t, ksnc_list);
=======
	/*
	 * Refuse to duplicate an existing connection, unless this is a
	 * loopback connection
	 */
	if (conn->ksnc_ipaddr != conn->ksnc_myipaddr) {
		list_for_each(tmp, &peer->ksnp_conns) {
			conn2 = list_entry(tmp, struct ksock_conn, ksnc_list);
>>>>>>> v4.9.227

			if (conn2->ksnc_ipaddr != conn->ksnc_ipaddr ||
			    conn2->ksnc_myipaddr != conn->ksnc_myipaddr ||
			    conn2->ksnc_type != conn->ksnc_type)
				continue;

<<<<<<< HEAD
			/* Reply on a passive connection attempt so the peer
			 * realises we're connected. */
			LASSERT (rc == 0);
=======
			/*
			 * Reply on a passive connection attempt so the peer
			 * realises we're connected.
			 */
			LASSERT(!rc);
>>>>>>> v4.9.227
			if (!active)
				rc = EALREADY;

			warn = "duplicate";
			goto failed_2;
		}
	}

<<<<<<< HEAD
	/* If the connection created by this route didn't bind to the IP
	 * address the route connected to, the connection/route matching
	 * code below probably isn't going to work. */
=======
	/*
	 * If the connection created by this route didn't bind to the IP
	 * address the route connected to, the connection/route matching
	 * code below probably isn't going to work.
	 */
>>>>>>> v4.9.227
	if (active &&
	    route->ksnr_ipaddr != conn->ksnc_ipaddr) {
		CERROR("Route %s %pI4h connected to %pI4h\n",
		       libcfs_id2str(peer->ksnp_id),
		       &route->ksnr_ipaddr,
		       &conn->ksnc_ipaddr);
	}

<<<<<<< HEAD
	/* Search for a route corresponding to the new connection and
	 * create an association.  This allows incoming connections created
	 * by routes in my peer to match my own route entries so I don't
	 * continually create duplicate routes. */
	list_for_each (tmp, &peer->ksnp_routes) {
		route = list_entry(tmp, ksock_route_t, ksnr_list);
=======
	/*
	 * Search for a route corresponding to the new connection and
	 * create an association.  This allows incoming connections created
	 * by routes in my peer to match my own route entries so I don't
	 * continually create duplicate routes.
	 */
	list_for_each(tmp, &peer->ksnp_routes) {
		route = list_entry(tmp, struct ksock_route, ksnr_list);
>>>>>>> v4.9.227

		if (route->ksnr_ipaddr != conn->ksnc_ipaddr)
			continue;

		ksocknal_associate_route_conn_locked(route, conn);
		break;
	}

	conn->ksnc_peer = peer;		 /* conn takes my ref on peer */
	peer->ksnp_last_alive = cfs_time_current();
	peer->ksnp_send_keepalive = 0;
	peer->ksnp_error = 0;

	sched = ksocknal_choose_scheduler_locked(cpt);
	sched->kss_nconns++;
	conn->ksnc_scheduler = sched;

	conn->ksnc_tx_last_post = cfs_time_current();
	/* Set the deadline for the outgoing HELLO to drain */
	conn->ksnc_tx_bufnob = sock->sk->sk_wmem_queued;
	conn->ksnc_tx_deadline = cfs_time_shift(*ksocknal_tunables.ksnd_timeout);
	mb();   /* order with adding to peer's conn list */

<<<<<<< HEAD
	list_add (&conn->ksnc_list, &peer->ksnp_conns);
=======
	list_add(&conn->ksnc_list, &peer->ksnp_conns);
>>>>>>> v4.9.227
	ksocknal_conn_addref(conn);

	ksocknal_new_packet(conn, 0);

	conn->ksnc_zc_capable = ksocknal_lib_zc_capable(conn);

	/* Take packets blocking for this connection. */
	list_for_each_entry_safe(tx, txtmp, &peer->ksnp_tx_queue, tx_list) {
		if (conn->ksnc_proto->pro_match_tx(conn, tx, tx->tx_nonblk) == SOCKNAL_MATCH_NO)
				continue;

<<<<<<< HEAD
		list_del (&tx->tx_list);
		ksocknal_queue_tx_locked (tx, conn);
=======
		list_del(&tx->tx_list);
		ksocknal_queue_tx_locked(tx, conn);
>>>>>>> v4.9.227
	}

	write_unlock_bh(global_lock);

<<<<<<< HEAD
	/* We've now got a new connection.  Any errors from here on are just
=======
	/*
	 * We've now got a new connection.  Any errors from here on are just
>>>>>>> v4.9.227
	 * like "normal" comms errors and we close the connection normally.
	 * NB (a) we still have to send the reply HELLO for passive
	 *	connections,
	 *    (b) normal I/O on the conn is blocked until I setup and call the
	 *	socket callbacks.
	 */
<<<<<<< HEAD

	CDEBUG(D_NET, "New conn %s p %d.x %pI4h -> %pI4h/%d"
	       " incarnation:%lld sched[%d:%d]\n",
=======
	CDEBUG(D_NET, "New conn %s p %d.x %pI4h -> %pI4h/%d incarnation:%lld sched[%d:%d]\n",
>>>>>>> v4.9.227
	       libcfs_id2str(peerid), conn->ksnc_proto->pro_version,
	       &conn->ksnc_myipaddr, &conn->ksnc_ipaddr,
	       conn->ksnc_port, incarnation, cpt,
	       (int)(sched - &sched->kss_info->ksi_scheds[0]));

	if (active) {
		/* additional routes after interface exchange? */
		ksocknal_create_routes(peer, conn->ksnc_port,
				       hello->kshm_ips, hello->kshm_nips);
	} else {
		hello->kshm_nips = ksocknal_select_ips(peer, hello->kshm_ips,
						       hello->kshm_nips);
		rc = ksocknal_send_hello(ni, conn, peerid.nid, hello);
	}

	LIBCFS_FREE(hello, offsetof(ksock_hello_msg_t,
				    kshm_ips[LNET_MAX_INTERFACES]));

<<<<<<< HEAD
	/* setup the socket AFTER I've received hello (it disables
	 * SO_LINGER).  I might call back to the acceptor who may want
	 * to send a protocol version response and then close the
	 * socket; this ensures the socket only tears down after the
	 * response has been sent. */
	if (rc == 0)
=======
	/*
	 * setup the socket AFTER I've received hello (it disables
	 * SO_LINGER).  I might call back to the acceptor who may want
	 * to send a protocol version response and then close the
	 * socket; this ensures the socket only tears down after the
	 * response has been sent.
	 */
	if (!rc)
>>>>>>> v4.9.227
		rc = ksocknal_lib_setup_sock(sock);

	write_lock_bh(global_lock);

	/* NB my callbacks block while I hold ksnd_global_lock */
	ksocknal_lib_set_callback(sock, conn);

	if (!active)
		peer->ksnp_accepting--;

	write_unlock_bh(global_lock);

<<<<<<< HEAD
	if (rc != 0) {
=======
	if (rc) {
>>>>>>> v4.9.227
		write_lock_bh(global_lock);
		if (!conn->ksnc_closing) {
			/* could be closed by another thread */
			ksocknal_close_conn_locked(conn, rc);
		}
		write_unlock_bh(global_lock);
<<<<<<< HEAD
	} else if (ksocknal_connsock_addref(conn) == 0) {
=======
	} else if (!ksocknal_connsock_addref(conn)) {
>>>>>>> v4.9.227
		/* Allow I/O to proceed. */
		ksocknal_read_callback(conn);
		ksocknal_write_callback(conn);
		ksocknal_connsock_decref(conn);
	}

	ksocknal_connsock_decref(conn);
	ksocknal_conn_decref(conn);
	return rc;

 failed_2:
	if (!peer->ksnp_closing &&
<<<<<<< HEAD
	    list_empty (&peer->ksnp_conns) &&
	    list_empty (&peer->ksnp_routes)) {
=======
	    list_empty(&peer->ksnp_conns) &&
	    list_empty(&peer->ksnp_routes)) {
>>>>>>> v4.9.227
		list_add(&zombies, &peer->ksnp_tx_queue);
		list_del_init(&peer->ksnp_tx_queue);
		ksocknal_unlink_peer_locked(peer);
	}

	write_unlock_bh(global_lock);

<<<<<<< HEAD
	if (warn != NULL) {
=======
	if (warn) {
>>>>>>> v4.9.227
		if (rc < 0)
			CERROR("Not creating conn %s type %d: %s\n",
			       libcfs_id2str(peerid), conn->ksnc_type, warn);
		else
			CDEBUG(D_NET, "Not creating conn %s type %d: %s\n",
<<<<<<< HEAD
			      libcfs_id2str(peerid), conn->ksnc_type, warn);
=======
			       libcfs_id2str(peerid), conn->ksnc_type, warn);
>>>>>>> v4.9.227
	}

	if (!active) {
		if (rc > 0) {
<<<<<<< HEAD
			/* Request retry by replying with CONN_NONE
			 * ksnc_proto has been set already */
=======
			/*
			 * Request retry by replying with CONN_NONE
			 * ksnc_proto has been set already
			 */
>>>>>>> v4.9.227
			conn->ksnc_type = SOCKLND_CONN_NONE;
			hello->kshm_nips = 0;
			ksocknal_send_hello(ni, conn, peerid.nid, hello);
		}

		write_lock_bh(global_lock);
		peer->ksnp_accepting--;
		write_unlock_bh(global_lock);
	}

	ksocknal_txlist_done(ni, &zombies, 1);
	ksocknal_peer_decref(peer);

<<<<<<< HEAD
 failed_1:
	if (hello != NULL)
		LIBCFS_FREE(hello, offsetof(ksock_hello_msg_t,
					    kshm_ips[LNET_MAX_INTERFACES]));

	LIBCFS_FREE (conn, sizeof(*conn));

 failed_0:
	libcfs_sock_release(sock);
=======
failed_1:
	if (hello)
		LIBCFS_FREE(hello, offsetof(ksock_hello_msg_t,
					    kshm_ips[LNET_MAX_INTERFACES]));

	LIBCFS_FREE(conn, sizeof(*conn));

failed_0:
	sock_release(sock);
>>>>>>> v4.9.227
	return rc;
}

void
<<<<<<< HEAD
ksocknal_close_conn_locked (ksock_conn_t *conn, int error)
{
	/* This just does the immmediate housekeeping, and queues the
	 * connection for the reaper to terminate.
	 * Caller holds ksnd_global_lock exclusively in irq context */
	ksock_peer_t      *peer = conn->ksnc_peer;
	ksock_route_t     *route;
	ksock_conn_t      *conn2;
	struct list_head	*tmp;

	LASSERT (peer->ksnp_error == 0);
	LASSERT (!conn->ksnc_closing);
	conn->ksnc_closing = 1;

	/* ksnd_deathrow_conns takes over peer's ref */
	list_del (&conn->ksnc_list);

	route = conn->ksnc_route;
	if (route != NULL) {
		/* dissociate conn from route... */
		LASSERT (!route->ksnr_deleted);
		LASSERT ((route->ksnr_connected & (1 << conn->ksnc_type)) != 0);

		conn2 = NULL;
		list_for_each(tmp, &peer->ksnp_conns) {
			conn2 = list_entry(tmp, ksock_conn_t, ksnc_list);
=======
ksocknal_close_conn_locked(struct ksock_conn *conn, int error)
{
	/*
	 * This just does the immmediate housekeeping, and queues the
	 * connection for the reaper to terminate.
	 * Caller holds ksnd_global_lock exclusively in irq context
	 */
	struct ksock_peer *peer = conn->ksnc_peer;
	struct ksock_route *route;
	struct ksock_conn *conn2;
	struct list_head *tmp;

	LASSERT(!peer->ksnp_error);
	LASSERT(!conn->ksnc_closing);
	conn->ksnc_closing = 1;

	/* ksnd_deathrow_conns takes over peer's ref */
	list_del(&conn->ksnc_list);

	route = conn->ksnc_route;
	if (route) {
		/* dissociate conn from route... */
		LASSERT(!route->ksnr_deleted);
		LASSERT(route->ksnr_connected & (1 << conn->ksnc_type));

		conn2 = NULL;
		list_for_each(tmp, &peer->ksnp_conns) {
			conn2 = list_entry(tmp, struct ksock_conn, ksnc_list);
>>>>>>> v4.9.227

			if (conn2->ksnc_route == route &&
			    conn2->ksnc_type == conn->ksnc_type)
				break;

			conn2 = NULL;
		}
<<<<<<< HEAD
		if (conn2 == NULL)
=======
		if (!conn2)
>>>>>>> v4.9.227
			route->ksnr_connected &= ~(1 << conn->ksnc_type);

		conn->ksnc_route = NULL;

<<<<<<< HEAD
#if 0	   /* irrelevant with only eager routes */
		/* make route least favourite */
		list_del (&route->ksnr_list);
		list_add_tail (&route->ksnr_list, &peer->ksnp_routes);
#endif
		ksocknal_route_decref(route);     /* drop conn's ref on route */
	}

	if (list_empty (&peer->ksnp_conns)) {
		/* No more connections to this peer */

		if (!list_empty(&peer->ksnp_tx_queue)) {
			ksock_tx_t *tx;

			LASSERT (conn->ksnc_proto == &ksocknal_protocol_v3x);

			/* throw them to the last connection...,
			 * these TXs will be send to /dev/null by scheduler */
			list_for_each_entry(tx, &peer->ksnp_tx_queue,
						tx_list)
=======
		ksocknal_route_decref(route);     /* drop conn's ref on route */
	}

	if (list_empty(&peer->ksnp_conns)) {
		/* No more connections to this peer */

		if (!list_empty(&peer->ksnp_tx_queue)) {
			struct ksock_tx *tx;

			LASSERT(conn->ksnc_proto == &ksocknal_protocol_v3x);

			/*
			 * throw them to the last connection...,
			 * these TXs will be send to /dev/null by scheduler
			 */
			list_for_each_entry(tx, &peer->ksnp_tx_queue,
					    tx_list)
>>>>>>> v4.9.227
				ksocknal_tx_prep(conn, tx);

			spin_lock_bh(&conn->ksnc_scheduler->kss_lock);
			list_splice_init(&peer->ksnp_tx_queue,
<<<<<<< HEAD
					     &conn->ksnc_tx_queue);
=======
					 &conn->ksnc_tx_queue);
>>>>>>> v4.9.227
			spin_unlock_bh(&conn->ksnc_scheduler->kss_lock);
		}

		peer->ksnp_proto = NULL;	/* renegotiate protocol version */
		peer->ksnp_error = error;       /* stash last conn close reason */

<<<<<<< HEAD
		if (list_empty (&peer->ksnp_routes)) {
			/* I've just closed last conn belonging to a
			 * peer with no routes to it */
			ksocknal_unlink_peer_locked (peer);
=======
		if (list_empty(&peer->ksnp_routes)) {
			/*
			 * I've just closed last conn belonging to a
			 * peer with no routes to it
			 */
			ksocknal_unlink_peer_locked(peer);
>>>>>>> v4.9.227
		}
	}

	spin_lock_bh(&ksocknal_data.ksnd_reaper_lock);

	list_add_tail(&conn->ksnc_list,
<<<<<<< HEAD
			  &ksocknal_data.ksnd_deathrow_conns);
=======
		      &ksocknal_data.ksnd_deathrow_conns);
>>>>>>> v4.9.227
	wake_up(&ksocknal_data.ksnd_reaper_waitq);

	spin_unlock_bh(&ksocknal_data.ksnd_reaper_lock);
}

void
<<<<<<< HEAD
ksocknal_peer_failed (ksock_peer_t *peer)
{
	int	notify = 0;
	unsigned long last_alive = 0;

	/* There has been a connection failure or comms error; but I'll only
	 * tell LNET I think the peer is dead if it's to another kernel and
	 * there are no connections or connection attempts in existence. */

	read_lock(&ksocknal_data.ksnd_global_lock);

	if ((peer->ksnp_id.pid & LNET_PID_USERFLAG) == 0 &&
	    list_empty(&peer->ksnp_conns) &&
	    peer->ksnp_accepting == 0 &&
	    ksocknal_find_connecting_route_locked(peer) == NULL) {
=======
ksocknal_peer_failed(struct ksock_peer *peer)
{
	int notify = 0;
	unsigned long last_alive = 0;

	/*
	 * There has been a connection failure or comms error; but I'll only
	 * tell LNET I think the peer is dead if it's to another kernel and
	 * there are no connections or connection attempts in existence.
	 */
	read_lock(&ksocknal_data.ksnd_global_lock);

	if (!(peer->ksnp_id.pid & LNET_PID_USERFLAG) &&
	    list_empty(&peer->ksnp_conns) &&
	    !peer->ksnp_accepting &&
	    !ksocknal_find_connecting_route_locked(peer)) {
>>>>>>> v4.9.227
		notify = 1;
		last_alive = peer->ksnp_last_alive;
	}

	read_unlock(&ksocknal_data.ksnd_global_lock);

	if (notify)
<<<<<<< HEAD
		lnet_notify (peer->ksnp_ni, peer->ksnp_id.nid, 0,
			     last_alive);
}

void
ksocknal_finalize_zcreq(ksock_conn_t *conn)
{
	ksock_peer_t     *peer = conn->ksnc_peer;
	ksock_tx_t       *tx;
	ksock_tx_t       *tmp;
	LIST_HEAD    (zlist);

	/* NB safe to finalize TXs because closing of socket will
	 * abort all buffered data */
	LASSERT (conn->ksnc_sock == NULL);
=======
		lnet_notify(peer->ksnp_ni, peer->ksnp_id.nid, 0,
			    last_alive);
}

void
ksocknal_finalize_zcreq(struct ksock_conn *conn)
{
	struct ksock_peer *peer = conn->ksnc_peer;
	struct ksock_tx *tx;
	struct ksock_tx *temp;
	struct ksock_tx *tmp;
	LIST_HEAD(zlist);

	/*
	 * NB safe to finalize TXs because closing of socket will
	 * abort all buffered data
	 */
	LASSERT(!conn->ksnc_sock);
>>>>>>> v4.9.227

	spin_lock(&peer->ksnp_lock);

	list_for_each_entry_safe(tx, tmp, &peer->ksnp_zc_req_list, tx_zc_list) {
		if (tx->tx_conn != conn)
			continue;

<<<<<<< HEAD
		LASSERT (tx->tx_msg.ksm_zc_cookies[0] != 0);
=======
		LASSERT(tx->tx_msg.ksm_zc_cookies[0]);
>>>>>>> v4.9.227

		tx->tx_msg.ksm_zc_cookies[0] = 0;
		tx->tx_zc_aborted = 1; /* mark it as not-acked */
		list_del(&tx->tx_zc_list);
		list_add(&tx->tx_zc_list, &zlist);
	}

	spin_unlock(&peer->ksnp_lock);

<<<<<<< HEAD
	while (!list_empty(&zlist)) {
		tx = list_entry(zlist.next, ksock_tx_t, tx_zc_list);

=======
	list_for_each_entry_safe(tx, temp, &zlist, tx_zc_list) {
>>>>>>> v4.9.227
		list_del(&tx->tx_zc_list);
		ksocknal_tx_decref(tx);
	}
}

void
<<<<<<< HEAD
ksocknal_terminate_conn (ksock_conn_t *conn)
{
	/* This gets called by the reaper (guaranteed thread context) to
	 * disengage the socket from its callbacks and close it.
	 * ksnc_refcount will eventually hit zero, and then the reaper will
	 * destroy it. */
	ksock_peer_t     *peer = conn->ksnc_peer;
	ksock_sched_t    *sched = conn->ksnc_scheduler;
	int	       failed = 0;
=======
ksocknal_terminate_conn(struct ksock_conn *conn)
{
	/*
	 * This gets called by the reaper (guaranteed thread context) to
	 * disengage the socket from its callbacks and close it.
	 * ksnc_refcount will eventually hit zero, and then the reaper will
	 * destroy it.
	 */
	struct ksock_peer *peer = conn->ksnc_peer;
	struct ksock_sched *sched = conn->ksnc_scheduler;
	int failed = 0;
>>>>>>> v4.9.227

	LASSERT(conn->ksnc_closing);

	/* wake up the scheduler to "send" all remaining packets to /dev/null */
	spin_lock_bh(&sched->kss_lock);

	/* a closing conn is always ready to tx */
	conn->ksnc_tx_ready = 1;

	if (!conn->ksnc_tx_scheduled &&
	    !list_empty(&conn->ksnc_tx_queue)) {
<<<<<<< HEAD
		list_add_tail (&conn->ksnc_tx_list,
			       &sched->kss_tx_conns);
=======
		list_add_tail(&conn->ksnc_tx_list,
			      &sched->kss_tx_conns);
>>>>>>> v4.9.227
		conn->ksnc_tx_scheduled = 1;
		/* extra ref for scheduler */
		ksocknal_conn_addref(conn);

<<<<<<< HEAD
		wake_up (&sched->kss_waitq);
=======
		wake_up(&sched->kss_waitq);
>>>>>>> v4.9.227
	}

	spin_unlock_bh(&sched->kss_lock);

	/* serialise with callbacks */
	write_lock_bh(&ksocknal_data.ksnd_global_lock);

	ksocknal_lib_reset_callback(conn->ksnc_sock, conn);

<<<<<<< HEAD
	/* OK, so this conn may not be completely disengaged from its
	 * scheduler yet, but it _has_ committed to terminate... */
	conn->ksnc_scheduler->kss_nconns--;

	if (peer->ksnp_error != 0) {
		/* peer's last conn closed in error */
		LASSERT (list_empty (&peer->ksnp_conns));
=======
	/*
	 * OK, so this conn may not be completely disengaged from its
	 * scheduler yet, but it _has_ committed to terminate...
	 */
	conn->ksnc_scheduler->kss_nconns--;

	if (peer->ksnp_error) {
		/* peer's last conn closed in error */
		LASSERT(list_empty(&peer->ksnp_conns));
>>>>>>> v4.9.227
		failed = 1;
		peer->ksnp_error = 0;     /* avoid multiple notifications */
	}

	write_unlock_bh(&ksocknal_data.ksnd_global_lock);

	if (failed)
		ksocknal_peer_failed(peer);

<<<<<<< HEAD
	/* The socket is closed on the final put; either here, or in
	 * ksocknal_{send,recv}msg().  Since we set up the linger2 option
	 * when the connection was established, this will close the socket
	 * immediately, aborting anything buffered in it. Any hung
	 * zero-copy transmits will therefore complete in finite time. */
=======
	/*
	 * The socket is closed on the final put; either here, or in
	 * ksocknal_{send,recv}msg().  Since we set up the linger2 option
	 * when the connection was established, this will close the socket
	 * immediately, aborting anything buffered in it. Any hung
	 * zero-copy transmits will therefore complete in finite time.
	 */
>>>>>>> v4.9.227
	ksocknal_connsock_decref(conn);
}

void
<<<<<<< HEAD
ksocknal_queue_zombie_conn (ksock_conn_t *conn)
{
	/* Queue the conn for the reaper to destroy */

	LASSERT(atomic_read(&conn->ksnc_conn_refcount) == 0);
=======
ksocknal_queue_zombie_conn(struct ksock_conn *conn)
{
	/* Queue the conn for the reaper to destroy */

	LASSERT(!atomic_read(&conn->ksnc_conn_refcount));
>>>>>>> v4.9.227
	spin_lock_bh(&ksocknal_data.ksnd_reaper_lock);

	list_add_tail(&conn->ksnc_list, &ksocknal_data.ksnd_zombie_conns);
	wake_up(&ksocknal_data.ksnd_reaper_waitq);

	spin_unlock_bh(&ksocknal_data.ksnd_reaper_lock);
}

void
<<<<<<< HEAD
ksocknal_destroy_conn (ksock_conn_t *conn)
{
	unsigned long      last_rcv;

	/* Final coup-de-grace of the reaper */
	CDEBUG (D_NET, "connection %p\n", conn);

	LASSERT (atomic_read (&conn->ksnc_conn_refcount) == 0);
	LASSERT (atomic_read (&conn->ksnc_sock_refcount) == 0);
	LASSERT (conn->ksnc_sock == NULL);
	LASSERT (conn->ksnc_route == NULL);
	LASSERT (!conn->ksnc_tx_scheduled);
	LASSERT (!conn->ksnc_rx_scheduled);
	LASSERT (list_empty(&conn->ksnc_tx_queue));
=======
ksocknal_destroy_conn(struct ksock_conn *conn)
{
	unsigned long last_rcv;

	/* Final coup-de-grace of the reaper */
	CDEBUG(D_NET, "connection %p\n", conn);

	LASSERT(!atomic_read(&conn->ksnc_conn_refcount));
	LASSERT(!atomic_read(&conn->ksnc_sock_refcount));
	LASSERT(!conn->ksnc_sock);
	LASSERT(!conn->ksnc_route);
	LASSERT(!conn->ksnc_tx_scheduled);
	LASSERT(!conn->ksnc_rx_scheduled);
	LASSERT(list_empty(&conn->ksnc_tx_queue));
>>>>>>> v4.9.227

	/* complete current receive if any */
	switch (conn->ksnc_rx_state) {
	case SOCKNAL_RX_LNET_PAYLOAD:
		last_rcv = conn->ksnc_rx_deadline -
			   cfs_time_seconds(*ksocknal_tunables.ksnd_timeout);
<<<<<<< HEAD
		CERROR("Completing partial receive from %s[%d]"
		       ", ip %pI4h:%d, with error, wanted: %d, left: %d, "
		       "last alive is %ld secs ago\n",
=======
		CERROR("Completing partial receive from %s[%d], ip %pI4h:%d, with error, wanted: %d, left: %d, last alive is %ld secs ago\n",
>>>>>>> v4.9.227
		       libcfs_id2str(conn->ksnc_peer->ksnp_id), conn->ksnc_type,
		       &conn->ksnc_ipaddr, conn->ksnc_port,
		       conn->ksnc_rx_nob_wanted, conn->ksnc_rx_nob_left,
		       cfs_duration_sec(cfs_time_sub(cfs_time_current(),
<<<<<<< HEAD
					last_rcv)));
		lnet_finalize (conn->ksnc_peer->ksnp_ni,
			       conn->ksnc_cookie, -EIO);
		break;
	case SOCKNAL_RX_LNET_HEADER:
		if (conn->ksnc_rx_started)
			CERROR("Incomplete receive of lnet header from %s"
			       ", ip %pI4h:%d, with error, protocol: %d.x.\n",
=======
						     last_rcv)));
		lnet_finalize(conn->ksnc_peer->ksnp_ni,
			      conn->ksnc_cookie, -EIO);
		break;
	case SOCKNAL_RX_LNET_HEADER:
		if (conn->ksnc_rx_started)
			CERROR("Incomplete receive of lnet header from %s, ip %pI4h:%d, with error, protocol: %d.x.\n",
>>>>>>> v4.9.227
			       libcfs_id2str(conn->ksnc_peer->ksnp_id),
			       &conn->ksnc_ipaddr, conn->ksnc_port,
			       conn->ksnc_proto->pro_version);
		break;
	case SOCKNAL_RX_KSM_HEADER:
		if (conn->ksnc_rx_started)
<<<<<<< HEAD
			CERROR("Incomplete receive of ksock message from %s"
			       ", ip %pI4h:%d, with error, protocol: %d.x.\n",
=======
			CERROR("Incomplete receive of ksock message from %s, ip %pI4h:%d, with error, protocol: %d.x.\n",
>>>>>>> v4.9.227
			       libcfs_id2str(conn->ksnc_peer->ksnp_id),
			       &conn->ksnc_ipaddr, conn->ksnc_port,
			       conn->ksnc_proto->pro_version);
		break;
	case SOCKNAL_RX_SLOP:
		if (conn->ksnc_rx_started)
<<<<<<< HEAD
			CERROR("Incomplete receive of slops from %s"
			       ", ip %pI4h:%d, with error\n",
=======
			CERROR("Incomplete receive of slops from %s, ip %pI4h:%d, with error\n",
>>>>>>> v4.9.227
			       libcfs_id2str(conn->ksnc_peer->ksnp_id),
			       &conn->ksnc_ipaddr, conn->ksnc_port);
	       break;
	default:
<<<<<<< HEAD
		LBUG ();
=======
		LBUG();
>>>>>>> v4.9.227
		break;
	}

	ksocknal_peer_decref(conn->ksnc_peer);

<<<<<<< HEAD
	LIBCFS_FREE (conn, sizeof (*conn));
}

int
ksocknal_close_peer_conns_locked (ksock_peer_t *peer, __u32 ipaddr, int why)
{
	ksock_conn_t       *conn;
	struct list_head	 *ctmp;
	struct list_head	 *cnxt;
	int		 count = 0;

	list_for_each_safe (ctmp, cnxt, &peer->ksnp_conns) {
		conn = list_entry (ctmp, ksock_conn_t, ksnc_list);

		if (ipaddr == 0 ||
		    conn->ksnc_ipaddr == ipaddr) {
			count++;
			ksocknal_close_conn_locked (conn, why);
=======
	LIBCFS_FREE(conn, sizeof(*conn));
}

int
ksocknal_close_peer_conns_locked(struct ksock_peer *peer, __u32 ipaddr, int why)
{
	struct ksock_conn *conn;
	struct list_head *ctmp;
	struct list_head *cnxt;
	int count = 0;

	list_for_each_safe(ctmp, cnxt, &peer->ksnp_conns) {
		conn = list_entry(ctmp, struct ksock_conn, ksnc_list);

		if (!ipaddr || conn->ksnc_ipaddr == ipaddr) {
			count++;
			ksocknal_close_conn_locked(conn, why);
>>>>>>> v4.9.227
		}
	}

	return count;
}

int
<<<<<<< HEAD
ksocknal_close_conn_and_siblings (ksock_conn_t *conn, int why)
{
	ksock_peer_t     *peer = conn->ksnc_peer;
	__u32	     ipaddr = conn->ksnc_ipaddr;
	int	       count;

	write_lock_bh(&ksocknal_data.ksnd_global_lock);

	count = ksocknal_close_peer_conns_locked (peer, ipaddr, why);
=======
ksocknal_close_conn_and_siblings(struct ksock_conn *conn, int why)
{
	struct ksock_peer *peer = conn->ksnc_peer;
	__u32 ipaddr = conn->ksnc_ipaddr;
	int count;

	write_lock_bh(&ksocknal_data.ksnd_global_lock);

	count = ksocknal_close_peer_conns_locked(peer, ipaddr, why);
>>>>>>> v4.9.227

	write_unlock_bh(&ksocknal_data.ksnd_global_lock);

	return count;
}

int
<<<<<<< HEAD
ksocknal_close_matching_conns (lnet_process_id_t id, __u32 ipaddr)
{
	ksock_peer_t       *peer;
	struct list_head	 *ptmp;
	struct list_head	 *pnxt;
	int		 lo;
	int		 hi;
	int		 i;
	int		 count = 0;

	write_lock_bh(&ksocknal_data.ksnd_global_lock);

	if (id.nid != LNET_NID_ANY)
		lo = hi = (int)(ksocknal_nid2peerlist(id.nid) - ksocknal_data.ksnd_peers);
	else {
=======
ksocknal_close_matching_conns(lnet_process_id_t id, __u32 ipaddr)
{
	struct ksock_peer *peer;
	struct list_head *ptmp;
	struct list_head *pnxt;
	int lo;
	int hi;
	int i;
	int count = 0;

	write_lock_bh(&ksocknal_data.ksnd_global_lock);

	if (id.nid != LNET_NID_ANY) {
		lo = (int)(ksocknal_nid2peerlist(id.nid) - ksocknal_data.ksnd_peers);
		hi = (int)(ksocknal_nid2peerlist(id.nid) - ksocknal_data.ksnd_peers);
	} else {
>>>>>>> v4.9.227
		lo = 0;
		hi = ksocknal_data.ksnd_peer_hash_size - 1;
	}

	for (i = lo; i <= hi; i++) {
<<<<<<< HEAD
		list_for_each_safe (ptmp, pnxt,
					&ksocknal_data.ksnd_peers[i]) {

			peer = list_entry (ptmp, ksock_peer_t, ksnp_list);
=======
		list_for_each_safe(ptmp, pnxt,
				   &ksocknal_data.ksnd_peers[i]) {
			peer = list_entry(ptmp, struct ksock_peer, ksnp_list);
>>>>>>> v4.9.227

			if (!((id.nid == LNET_NID_ANY || id.nid == peer->ksnp_id.nid) &&
			      (id.pid == LNET_PID_ANY || id.pid == peer->ksnp_id.pid)))
				continue;

<<<<<<< HEAD
			count += ksocknal_close_peer_conns_locked (peer, ipaddr, 0);
=======
			count += ksocknal_close_peer_conns_locked(peer, ipaddr, 0);
>>>>>>> v4.9.227
		}
	}

	write_unlock_bh(&ksocknal_data.ksnd_global_lock);

	/* wildcards always succeed */
<<<<<<< HEAD
	if (id.nid == LNET_NID_ANY || id.pid == LNET_PID_ANY || ipaddr == 0)
		return 0;

	if (count == 0)
=======
	if (id.nid == LNET_NID_ANY || id.pid == LNET_PID_ANY || !ipaddr)
		return 0;

	if (!count)
>>>>>>> v4.9.227
		return -ENOENT;
	else
		return 0;
}

void
<<<<<<< HEAD
ksocknal_notify (lnet_ni_t *ni, lnet_nid_t gw_nid, int alive)
{
	/* The router is telling me she's been notified of a change in
	 * gateway state.... */
	lnet_process_id_t  id = {0};
=======
ksocknal_notify(lnet_ni_t *ni, lnet_nid_t gw_nid, int alive)
{
	/*
	 * The router is telling me she's been notified of a change in
	 * gateway state....
	 */
	lnet_process_id_t id = {0};
>>>>>>> v4.9.227

	id.nid = gw_nid;
	id.pid = LNET_PID_ANY;

<<<<<<< HEAD
	CDEBUG (D_NET, "gw %s %s\n", libcfs_nid2str(gw_nid),
		alive ? "up" : "down");

	if (!alive) {
		/* If the gateway crashed, close all open connections... */
		ksocknal_close_matching_conns (id, 0);
		return;
	}

	/* ...otherwise do nothing.  We can only establish new connections
	 * if we have autroutes, and these connect on demand. */
}

void
ksocknal_query (lnet_ni_t *ni, lnet_nid_t nid, unsigned long *when)
{
	int		connect = 1;
	unsigned long	 last_alive = 0;
	unsigned long	 now = cfs_time_current();
	ksock_peer_t      *peer = NULL;
	rwlock_t		*glock = &ksocknal_data.ksnd_global_lock;
	lnet_process_id_t  id = {.nid = nid, .pid = LUSTRE_SRV_LNET_PID};
=======
	CDEBUG(D_NET, "gw %s %s\n", libcfs_nid2str(gw_nid),
	       alive ? "up" : "down");

	if (!alive) {
		/* If the gateway crashed, close all open connections... */
		ksocknal_close_matching_conns(id, 0);
		return;
	}

	/*
	 * ...otherwise do nothing.  We can only establish new connections
	 * if we have autroutes, and these connect on demand.
	 */
}

void
ksocknal_query(lnet_ni_t *ni, lnet_nid_t nid, unsigned long *when)
{
	int connect = 1;
	unsigned long last_alive = 0;
	unsigned long now = cfs_time_current();
	struct ksock_peer *peer = NULL;
	rwlock_t *glock = &ksocknal_data.ksnd_global_lock;
	lnet_process_id_t id = {
		.nid = nid,
		.pid = LNET_PID_LUSTRE,
	};
>>>>>>> v4.9.227

	read_lock(glock);

	peer = ksocknal_find_peer_locked(ni, id);
<<<<<<< HEAD
	if (peer != NULL) {
		struct list_head       *tmp;
		ksock_conn_t     *conn;
		int	       bufnob;

		list_for_each (tmp, &peer->ksnp_conns) {
			conn = list_entry(tmp, ksock_conn_t, ksnc_list);
=======
	if (peer) {
		struct list_head *tmp;
		struct ksock_conn *conn;
		int bufnob;

		list_for_each(tmp, &peer->ksnp_conns) {
			conn = list_entry(tmp, struct ksock_conn, ksnc_list);
>>>>>>> v4.9.227
			bufnob = conn->ksnc_sock->sk->sk_wmem_queued;

			if (bufnob < conn->ksnc_tx_bufnob) {
				/* something got ACKed */
				conn->ksnc_tx_deadline =
					cfs_time_shift(*ksocknal_tunables.ksnd_timeout);
				peer->ksnp_last_alive = now;
				conn->ksnc_tx_bufnob = bufnob;
			}
		}

		last_alive = peer->ksnp_last_alive;
<<<<<<< HEAD
		if (ksocknal_find_connectable_route_locked(peer) == NULL)
=======
		if (!ksocknal_find_connectable_route_locked(peer))
>>>>>>> v4.9.227
			connect = 0;
	}

	read_unlock(glock);

<<<<<<< HEAD
	if (last_alive != 0)
=======
	if (last_alive)
>>>>>>> v4.9.227
		*when = last_alive;

	CDEBUG(D_NET, "Peer %s %p, alive %ld secs ago, connect %d\n",
	       libcfs_nid2str(nid), peer,
	       last_alive ? cfs_duration_sec(now - last_alive) : -1,
	       connect);

	if (!connect)
		return;

	ksocknal_add_peer(ni, id, LNET_NIDADDR(nid), lnet_acceptor_port());

	write_lock_bh(glock);

	peer = ksocknal_find_peer_locked(ni, id);
<<<<<<< HEAD
	if (peer != NULL)
		ksocknal_launch_all_connections_locked(peer);

	write_unlock_bh(glock);
	return;
}

static void
ksocknal_push_peer (ksock_peer_t *peer)
{
	int	       index;
	int	       i;
	struct list_head       *tmp;
	ksock_conn_t     *conn;
=======
	if (peer)
		ksocknal_launch_all_connections_locked(peer);

	write_unlock_bh(glock);
}

static void
ksocknal_push_peer(struct ksock_peer *peer)
{
	int index;
	int i;
	struct list_head *tmp;
	struct ksock_conn *conn;
>>>>>>> v4.9.227

	for (index = 0; ; index++) {
		read_lock(&ksocknal_data.ksnd_global_lock);

		i = 0;
		conn = NULL;

<<<<<<< HEAD
		list_for_each (tmp, &peer->ksnp_conns) {
			if (i++ == index) {
				conn = list_entry (tmp, ksock_conn_t,
						       ksnc_list);
=======
		list_for_each(tmp, &peer->ksnp_conns) {
			if (i++ == index) {
				conn = list_entry(tmp, struct ksock_conn,
						  ksnc_list);
>>>>>>> v4.9.227
				ksocknal_conn_addref(conn);
				break;
			}
		}

		read_unlock(&ksocknal_data.ksnd_global_lock);

<<<<<<< HEAD
		if (conn == NULL)
			break;

		ksocknal_lib_push_conn (conn);
=======
		if (!conn)
			break;

		ksocknal_lib_push_conn(conn);
>>>>>>> v4.9.227
		ksocknal_conn_decref(conn);
	}
}

<<<<<<< HEAD
static int
ksocknal_push (lnet_ni_t *ni, lnet_process_id_t id)
{
	ksock_peer_t      *peer;
	struct list_head	*tmp;
	int		index;
	int		i;
	int		j;
	int		rc = -ENOENT;

	for (i = 0; i < ksocknal_data.ksnd_peer_hash_size; i++) {
		for (j = 0; ; j++) {
			read_lock(&ksocknal_data.ksnd_global_lock);

			index = 0;
			peer = NULL;

			list_for_each (tmp, &ksocknal_data.ksnd_peers[i]) {
				peer = list_entry(tmp, ksock_peer_t,
						      ksnp_list);

				if (!((id.nid == LNET_NID_ANY ||
				       id.nid == peer->ksnp_id.nid) &&
				      (id.pid == LNET_PID_ANY ||
				       id.pid == peer->ksnp_id.pid))) {
					peer = NULL;
					continue;
				}

				if (index++ == j) {
=======
static int ksocknal_push(lnet_ni_t *ni, lnet_process_id_t id)
{
	struct list_head *start;
	struct list_head *end;
	struct list_head *tmp;
	int rc = -ENOENT;
	unsigned int hsize = ksocknal_data.ksnd_peer_hash_size;

	if (id.nid == LNET_NID_ANY) {
		start = &ksocknal_data.ksnd_peers[0];
		end = &ksocknal_data.ksnd_peers[hsize - 1];
	} else {
		start = ksocknal_nid2peerlist(id.nid);
		end = ksocknal_nid2peerlist(id.nid);
	}

	for (tmp = start; tmp <= end; tmp++) {
		int peer_off; /* searching offset in peer hash table */

		for (peer_off = 0; ; peer_off++) {
			struct ksock_peer *peer;
			int i = 0;

			read_lock(&ksocknal_data.ksnd_global_lock);
			list_for_each_entry(peer, tmp, ksnp_list) {
				if (!((id.nid == LNET_NID_ANY ||
				       id.nid == peer->ksnp_id.nid) &&
				      (id.pid == LNET_PID_ANY ||
				       id.pid == peer->ksnp_id.pid)))
					continue;

				if (i++ == peer_off) {
>>>>>>> v4.9.227
					ksocknal_peer_addref(peer);
					break;
				}
			}
<<<<<<< HEAD

			read_unlock(&ksocknal_data.ksnd_global_lock);

			if (peer != NULL) {
				rc = 0;
				ksocknal_push_peer (peer);
				ksocknal_peer_decref(peer);
			}
		}

	}

=======
			read_unlock(&ksocknal_data.ksnd_global_lock);

			if (!i) /* no match */
				break;

			rc = 0;
			ksocknal_push_peer(peer);
			ksocknal_peer_decref(peer);
		}
	}
>>>>>>> v4.9.227
	return rc;
}

static int
ksocknal_add_interface(lnet_ni_t *ni, __u32 ipaddress, __u32 netmask)
{
<<<<<<< HEAD
	ksock_net_t       *net = ni->ni_data;
	ksock_interface_t *iface;
	int		rc;
	int		i;
	int		j;
	struct list_head	*ptmp;
	ksock_peer_t      *peer;
	struct list_head	*rtmp;
	ksock_route_t     *route;

	if (ipaddress == 0 ||
	    netmask == 0)
=======
	struct ksock_net *net = ni->ni_data;
	struct ksock_interface *iface;
	int rc;
	int i;
	int j;
	struct list_head *ptmp;
	struct ksock_peer *peer;
	struct list_head *rtmp;
	struct ksock_route *route;

	if (!ipaddress || !netmask)
>>>>>>> v4.9.227
		return -EINVAL;

	write_lock_bh(&ksocknal_data.ksnd_global_lock);

	iface = ksocknal_ip2iface(ni, ipaddress);
<<<<<<< HEAD
	if (iface != NULL) {
=======
	if (iface) {
>>>>>>> v4.9.227
		/* silently ignore dups */
		rc = 0;
	} else if (net->ksnn_ninterfaces == LNET_MAX_INTERFACES) {
		rc = -ENOSPC;
	} else {
		iface = &net->ksnn_interfaces[net->ksnn_ninterfaces++];

		iface->ksni_ipaddr = ipaddress;
		iface->ksni_netmask = netmask;
		iface->ksni_nroutes = 0;
		iface->ksni_npeers = 0;

		for (i = 0; i < ksocknal_data.ksnd_peer_hash_size; i++) {
			list_for_each(ptmp, &ksocknal_data.ksnd_peers[i]) {
<<<<<<< HEAD
				peer = list_entry(ptmp, ksock_peer_t,
						      ksnp_list);
=======
				peer = list_entry(ptmp, struct ksock_peer,
						  ksnp_list);
>>>>>>> v4.9.227

				for (j = 0; j < peer->ksnp_n_passive_ips; j++)
					if (peer->ksnp_passive_ips[j] == ipaddress)
						iface->ksni_npeers++;

				list_for_each(rtmp, &peer->ksnp_routes) {
<<<<<<< HEAD
					route = list_entry(rtmp,
							       ksock_route_t,
							       ksnr_list);
=======
					route = list_entry(rtmp, struct ksock_route,
							   ksnr_list);
>>>>>>> v4.9.227

					if (route->ksnr_myipaddr == ipaddress)
						iface->ksni_nroutes++;
				}
			}
		}

		rc = 0;
		/* NB only new connections will pay attention to the new interface! */
	}

	write_unlock_bh(&ksocknal_data.ksnd_global_lock);

	return rc;
}

static void
<<<<<<< HEAD
ksocknal_peer_del_interface_locked(ksock_peer_t *peer, __u32 ipaddr)
{
	struct list_head	 *tmp;
	struct list_head	 *nxt;
	ksock_route_t      *route;
	ksock_conn_t       *conn;
	int		 i;
	int		 j;

	for (i = 0; i < peer->ksnp_n_passive_ips; i++)
		if (peer->ksnp_passive_ips[i] == ipaddr) {
			for (j = i+1; j < peer->ksnp_n_passive_ips; j++)
				peer->ksnp_passive_ips[j-1] =
=======
ksocknal_peer_del_interface_locked(struct ksock_peer *peer, __u32 ipaddr)
{
	struct list_head *tmp;
	struct list_head *nxt;
	struct ksock_route *route;
	struct ksock_conn *conn;
	int i;
	int j;

	for (i = 0; i < peer->ksnp_n_passive_ips; i++)
		if (peer->ksnp_passive_ips[i] == ipaddr) {
			for (j = i + 1; j < peer->ksnp_n_passive_ips; j++)
				peer->ksnp_passive_ips[j - 1] =
>>>>>>> v4.9.227
					peer->ksnp_passive_ips[j];
			peer->ksnp_n_passive_ips--;
			break;
		}

	list_for_each_safe(tmp, nxt, &peer->ksnp_routes) {
<<<<<<< HEAD
		route = list_entry (tmp, ksock_route_t, ksnr_list);
=======
		route = list_entry(tmp, struct ksock_route, ksnr_list);
>>>>>>> v4.9.227

		if (route->ksnr_myipaddr != ipaddr)
			continue;

<<<<<<< HEAD
		if (route->ksnr_share_count != 0) {
=======
		if (route->ksnr_share_count) {
>>>>>>> v4.9.227
			/* Manually created; keep, but unbind */
			route->ksnr_myipaddr = 0;
		} else {
			ksocknal_del_route_locked(route);
		}
	}

	list_for_each_safe(tmp, nxt, &peer->ksnp_conns) {
<<<<<<< HEAD
		conn = list_entry(tmp, ksock_conn_t, ksnc_list);

		if (conn->ksnc_myipaddr == ipaddr)
			ksocknal_close_conn_locked (conn, 0);
=======
		conn = list_entry(tmp, struct ksock_conn, ksnc_list);

		if (conn->ksnc_myipaddr == ipaddr)
			ksocknal_close_conn_locked(conn, 0);
>>>>>>> v4.9.227
	}
}

static int
ksocknal_del_interface(lnet_ni_t *ni, __u32 ipaddress)
{
<<<<<<< HEAD
	ksock_net_t       *net = ni->ni_data;
	int		rc = -ENOENT;
	struct list_head	*tmp;
	struct list_head	*nxt;
	ksock_peer_t      *peer;
	__u32	      this_ip;
	int		i;
	int		j;
=======
	struct ksock_net *net = ni->ni_data;
	int rc = -ENOENT;
	struct list_head *tmp;
	struct list_head *nxt;
	struct ksock_peer *peer;
	__u32 this_ip;
	int i;
	int j;
>>>>>>> v4.9.227

	write_lock_bh(&ksocknal_data.ksnd_global_lock);

	for (i = 0; i < net->ksnn_ninterfaces; i++) {
		this_ip = net->ksnn_interfaces[i].ksni_ipaddr;

<<<<<<< HEAD
		if (!(ipaddress == 0 ||
		      ipaddress == this_ip))
=======
		if (!(!ipaddress || ipaddress == this_ip))
>>>>>>> v4.9.227
			continue;

		rc = 0;

<<<<<<< HEAD
		for (j = i+1; j < net->ksnn_ninterfaces; j++)
			net->ksnn_interfaces[j-1] =
=======
		for (j = i + 1; j < net->ksnn_ninterfaces; j++)
			net->ksnn_interfaces[j - 1] =
>>>>>>> v4.9.227
				net->ksnn_interfaces[j];

		net->ksnn_ninterfaces--;

		for (j = 0; j < ksocknal_data.ksnd_peer_hash_size; j++) {
			list_for_each_safe(tmp, nxt,
<<<<<<< HEAD
					       &ksocknal_data.ksnd_peers[j]) {
				peer = list_entry(tmp, ksock_peer_t,
						      ksnp_list);
=======
					   &ksocknal_data.ksnd_peers[j]) {
				peer = list_entry(tmp, struct ksock_peer, ksnp_list);
>>>>>>> v4.9.227

				if (peer->ksnp_ni != ni)
					continue;

				ksocknal_peer_del_interface_locked(peer, this_ip);
			}
		}
	}

	write_unlock_bh(&ksocknal_data.ksnd_global_lock);

	return rc;
}

int
ksocknal_ctl(lnet_ni_t *ni, unsigned int cmd, void *arg)
{
	lnet_process_id_t id = {0};
	struct libcfs_ioctl_data *data = arg;
	int rc;

	switch (cmd) {
	case IOC_LIBCFS_GET_INTERFACE: {
<<<<<<< HEAD
		ksock_net_t       *net = ni->ni_data;
		ksock_interface_t *iface;
=======
		struct ksock_net       *net = ni->ni_data;
		struct ksock_interface *iface;
>>>>>>> v4.9.227

		read_lock(&ksocknal_data.ksnd_global_lock);

		if (data->ioc_count >= (__u32)net->ksnn_ninterfaces) {
			rc = -ENOENT;
		} else {
			rc = 0;
			iface = &net->ksnn_interfaces[data->ioc_count];

			data->ioc_u32[0] = iface->ksni_ipaddr;
			data->ioc_u32[1] = iface->ksni_netmask;
			data->ioc_u32[2] = iface->ksni_npeers;
			data->ioc_u32[3] = iface->ksni_nroutes;
		}

		read_unlock(&ksocknal_data.ksnd_global_lock);
		return rc;
	}

	case IOC_LIBCFS_ADD_INTERFACE:
		return ksocknal_add_interface(ni,
					      data->ioc_u32[0], /* IP address */
					      data->ioc_u32[1]); /* net mask */

	case IOC_LIBCFS_DEL_INTERFACE:
		return ksocknal_del_interface(ni,
					      data->ioc_u32[0]); /* IP address */

	case IOC_LIBCFS_GET_PEER: {
<<<<<<< HEAD
		__u32	    myip = 0;
		__u32	    ip = 0;
		int	      port = 0;
		int	      conn_count = 0;
		int	      share_count = 0;
=======
		__u32 myip = 0;
		__u32 ip = 0;
		int port = 0;
		int conn_count = 0;
		int share_count = 0;
>>>>>>> v4.9.227

		rc = ksocknal_get_peer_info(ni, data->ioc_count,
					    &id, &myip, &ip, &port,
					    &conn_count,  &share_count);
<<<<<<< HEAD
		if (rc != 0)
=======
		if (rc)
>>>>>>> v4.9.227
			return rc;

		data->ioc_nid    = id.nid;
		data->ioc_count  = share_count;
		data->ioc_u32[0] = ip;
		data->ioc_u32[1] = port;
		data->ioc_u32[2] = myip;
		data->ioc_u32[3] = conn_count;
		data->ioc_u32[4] = id.pid;
		return 0;
	}

	case IOC_LIBCFS_ADD_PEER:
		id.nid = data->ioc_nid;
<<<<<<< HEAD
		id.pid = LUSTRE_SRV_LNET_PID;
		return ksocknal_add_peer (ni, id,
=======
		id.pid = LNET_PID_LUSTRE;
		return ksocknal_add_peer(ni, id,
>>>>>>> v4.9.227
					  data->ioc_u32[0], /* IP */
					  data->ioc_u32[1]); /* port */

	case IOC_LIBCFS_DEL_PEER:
		id.nid = data->ioc_nid;
		id.pid = LNET_PID_ANY;
<<<<<<< HEAD
		return ksocknal_del_peer (ni, id,
					  data->ioc_u32[0]); /* IP */

	case IOC_LIBCFS_GET_CONN: {
		int	   txmem;
		int	   rxmem;
		int	   nagle;
		ksock_conn_t *conn = ksocknal_get_conn_by_idx (ni, data->ioc_count);

		if (conn == NULL)
=======
		return ksocknal_del_peer(ni, id,
					  data->ioc_u32[0]); /* IP */

	case IOC_LIBCFS_GET_CONN: {
		int txmem;
		int rxmem;
		int nagle;
		struct ksock_conn *conn = ksocknal_get_conn_by_idx(ni, data->ioc_count);

		if (!conn)
>>>>>>> v4.9.227
			return -ENOENT;

		ksocknal_lib_get_conn_tunables(conn, &txmem, &rxmem, &nagle);

		data->ioc_count  = txmem;
		data->ioc_nid    = conn->ksnc_peer->ksnp_id.nid;
		data->ioc_flags  = nagle;
		data->ioc_u32[0] = conn->ksnc_ipaddr;
		data->ioc_u32[1] = conn->ksnc_port;
		data->ioc_u32[2] = conn->ksnc_myipaddr;
		data->ioc_u32[3] = conn->ksnc_type;
		data->ioc_u32[4] = conn->ksnc_scheduler->kss_info->ksi_cpt;
		data->ioc_u32[5] = rxmem;
		data->ioc_u32[6] = conn->ksnc_peer->ksnp_id.pid;
		ksocknal_conn_decref(conn);
		return 0;
	}

	case IOC_LIBCFS_CLOSE_CONNECTION:
		id.nid = data->ioc_nid;
		id.pid = LNET_PID_ANY;
<<<<<<< HEAD
		return ksocknal_close_matching_conns (id,
=======
		return ksocknal_close_matching_conns(id,
>>>>>>> v4.9.227
						      data->ioc_u32[0]);

	case IOC_LIBCFS_REGISTER_MYNID:
		/* Ignore if this is a noop */
		if (data->ioc_nid == ni->ni_nid)
			return 0;

		CERROR("obsolete IOC_LIBCFS_REGISTER_MYNID: %s(%s)\n",
		       libcfs_nid2str(data->ioc_nid),
		       libcfs_nid2str(ni->ni_nid));
		return -EINVAL;

	case IOC_LIBCFS_PUSH_CONNECTION:
		id.nid = data->ioc_nid;
		id.pid = LNET_PID_ANY;
		return ksocknal_push(ni, id);

	default:
		return -EINVAL;
	}
	/* not reached */
}

static void
<<<<<<< HEAD
ksocknal_free_buffers (void)
{
	LASSERT (atomic_read(&ksocknal_data.ksnd_nactive_txs) == 0);

	if (ksocknal_data.ksnd_sched_info != NULL) {
		struct ksock_sched_info	*info;
		int			i;

		cfs_percpt_for_each(info, i, ksocknal_data.ksnd_sched_info) {
			if (info->ksi_scheds != NULL) {
=======
ksocknal_free_buffers(void)
{
	LASSERT(!atomic_read(&ksocknal_data.ksnd_nactive_txs));

	if (ksocknal_data.ksnd_sched_info) {
		struct ksock_sched_info *info;
		int i;

		cfs_percpt_for_each(info, i, ksocknal_data.ksnd_sched_info) {
			if (info->ksi_scheds) {
>>>>>>> v4.9.227
				LIBCFS_FREE(info->ksi_scheds,
					    info->ksi_nthreads_max *
					    sizeof(info->ksi_scheds[0]));
			}
		}
		cfs_percpt_free(ksocknal_data.ksnd_sched_info);
	}

<<<<<<< HEAD
	LIBCFS_FREE (ksocknal_data.ksnd_peers,
		     sizeof (struct list_head) *
		     ksocknal_data.ksnd_peer_hash_size);
=======
	LIBCFS_FREE(ksocknal_data.ksnd_peers,
		    sizeof(struct list_head) *
		    ksocknal_data.ksnd_peer_hash_size);
>>>>>>> v4.9.227

	spin_lock(&ksocknal_data.ksnd_tx_lock);

	if (!list_empty(&ksocknal_data.ksnd_idle_noop_txs)) {
<<<<<<< HEAD
		struct list_head	zlist;
		ksock_tx_t	*tx;
=======
		struct list_head zlist;
		struct ksock_tx *tx;
		struct ksock_tx *temp;
>>>>>>> v4.9.227

		list_add(&zlist, &ksocknal_data.ksnd_idle_noop_txs);
		list_del_init(&ksocknal_data.ksnd_idle_noop_txs);
		spin_unlock(&ksocknal_data.ksnd_tx_lock);

<<<<<<< HEAD
		while (!list_empty(&zlist)) {
			tx = list_entry(zlist.next, ksock_tx_t, tx_list);
=======
		list_for_each_entry_safe(tx, temp, &zlist, tx_list) {
>>>>>>> v4.9.227
			list_del(&tx->tx_list);
			LIBCFS_FREE(tx, tx->tx_desc_size);
		}
	} else {
		spin_unlock(&ksocknal_data.ksnd_tx_lock);
	}
}

static void
ksocknal_base_shutdown(void)
{
	struct ksock_sched_info *info;
<<<<<<< HEAD
	ksock_sched_t		*sched;
	int			i;
	int			j;

	CDEBUG(D_MALLOC, "before NAL cleanup: kmem %d\n",
	       atomic_read (&libcfs_kmemory));
	LASSERT (ksocknal_data.ksnd_nnets == 0);

	switch (ksocknal_data.ksnd_init) {
	default:
		LASSERT (0);

	case SOCKNAL_INIT_ALL:
	case SOCKNAL_INIT_DATA:
		LASSERT (ksocknal_data.ksnd_peers != NULL);
		for (i = 0; i < ksocknal_data.ksnd_peer_hash_size; i++) {
			LASSERT (list_empty (&ksocknal_data.ksnd_peers[i]));
		}

		LASSERT(list_empty(&ksocknal_data.ksnd_nets));
		LASSERT (list_empty (&ksocknal_data.ksnd_enomem_conns));
		LASSERT (list_empty (&ksocknal_data.ksnd_zombie_conns));
		LASSERT (list_empty (&ksocknal_data.ksnd_connd_connreqs));
		LASSERT (list_empty (&ksocknal_data.ksnd_connd_routes));

		if (ksocknal_data.ksnd_sched_info != NULL) {
			cfs_percpt_for_each(info, i,
					    ksocknal_data.ksnd_sched_info) {
				if (info->ksi_scheds == NULL)
					continue;

				for (j = 0; j < info->ksi_nthreads_max; j++) {

=======
	struct ksock_sched *sched;
	int i;
	int j;

	LASSERT(!ksocknal_data.ksnd_nnets);

	switch (ksocknal_data.ksnd_init) {
	default:
		LASSERT(0);

	case SOCKNAL_INIT_ALL:
	case SOCKNAL_INIT_DATA:
		LASSERT(ksocknal_data.ksnd_peers);
		for (i = 0; i < ksocknal_data.ksnd_peer_hash_size; i++)
			LASSERT(list_empty(&ksocknal_data.ksnd_peers[i]));

		LASSERT(list_empty(&ksocknal_data.ksnd_nets));
		LASSERT(list_empty(&ksocknal_data.ksnd_enomem_conns));
		LASSERT(list_empty(&ksocknal_data.ksnd_zombie_conns));
		LASSERT(list_empty(&ksocknal_data.ksnd_connd_connreqs));
		LASSERT(list_empty(&ksocknal_data.ksnd_connd_routes));

		if (ksocknal_data.ksnd_sched_info) {
			cfs_percpt_for_each(info, i,
					    ksocknal_data.ksnd_sched_info) {
				if (!info->ksi_scheds)
					continue;

				for (j = 0; j < info->ksi_nthreads_max; j++) {
>>>>>>> v4.9.227
					sched = &info->ksi_scheds[j];
					LASSERT(list_empty(
						&sched->kss_tx_conns));
					LASSERT(list_empty(
						&sched->kss_rx_conns));
					LASSERT(list_empty(
						&sched->kss_zombie_noop_txs));
<<<<<<< HEAD
					LASSERT(sched->kss_nconns == 0);
=======
					LASSERT(!sched->kss_nconns);
>>>>>>> v4.9.227
				}
			}
		}

		/* flag threads to terminate; wake and wait for them to die */
		ksocknal_data.ksnd_shuttingdown = 1;
		wake_up_all(&ksocknal_data.ksnd_connd_waitq);
		wake_up_all(&ksocknal_data.ksnd_reaper_waitq);

<<<<<<< HEAD
		if (ksocknal_data.ksnd_sched_info != NULL) {
			cfs_percpt_for_each(info, i,
					    ksocknal_data.ksnd_sched_info) {
				if (info->ksi_scheds == NULL)
=======
		if (ksocknal_data.ksnd_sched_info) {
			cfs_percpt_for_each(info, i,
					    ksocknal_data.ksnd_sched_info) {
				if (!info->ksi_scheds)
>>>>>>> v4.9.227
					continue;

				for (j = 0; j < info->ksi_nthreads_max; j++) {
					sched = &info->ksi_scheds[j];
					wake_up_all(&sched->kss_waitq);
				}
			}
		}

		i = 4;
		read_lock(&ksocknal_data.ksnd_global_lock);
<<<<<<< HEAD
		while (ksocknal_data.ksnd_nthreads != 0) {
=======
		while (ksocknal_data.ksnd_nthreads) {
>>>>>>> v4.9.227
			i++;
			CDEBUG(((i & (-i)) == i) ? D_WARNING : D_NET, /* power of 2? */
			       "waiting for %d threads to terminate\n",
				ksocknal_data.ksnd_nthreads);
			read_unlock(&ksocknal_data.ksnd_global_lock);
			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule_timeout(cfs_time_seconds(1));
			read_lock(&ksocknal_data.ksnd_global_lock);
		}
		read_unlock(&ksocknal_data.ksnd_global_lock);

		ksocknal_free_buffers();

		ksocknal_data.ksnd_init = SOCKNAL_INIT_NOTHING;
		break;
	}

<<<<<<< HEAD
	CDEBUG(D_MALLOC, "after NAL cleanup: kmem %d\n",
	       atomic_read (&libcfs_kmemory));

=======
>>>>>>> v4.9.227
	module_put(THIS_MODULE);
}

static __u64
<<<<<<< HEAD
ksocknal_new_incarnation (void)
{
	struct timeval tv;

	/* The incarnation number is the time this module loaded and it
	 * identifies this particular instance of the socknal.  Hopefully
	 * we won't be able to reboot more frequently than 1MHz for the
	 * foreseeable future :) */

	do_gettimeofday(&tv);

	return (((__u64)tv.tv_sec) * 1000000) + tv.tv_usec;
=======
ksocknal_new_incarnation(void)
{
	/* The incarnation number is the time this module loaded and it
	 * identifies this particular instance of the socknal.
	 */
	return ktime_get_ns();
>>>>>>> v4.9.227
}

static int
ksocknal_base_startup(void)
{
	struct ksock_sched_info	*info;
<<<<<<< HEAD
	int			rc;
	int			i;

	LASSERT (ksocknal_data.ksnd_init == SOCKNAL_INIT_NOTHING);
	LASSERT (ksocknal_data.ksnd_nnets == 0);

	memset (&ksocknal_data, 0, sizeof (ksocknal_data)); /* zero pointers */

	ksocknal_data.ksnd_peer_hash_size = SOCKNAL_PEER_HASH_SIZE;
	LIBCFS_ALLOC (ksocknal_data.ksnd_peers,
		      sizeof (struct list_head) *
		      ksocknal_data.ksnd_peer_hash_size);
	if (ksocknal_data.ksnd_peers == NULL)
=======
	int rc;
	int i;

	LASSERT(ksocknal_data.ksnd_init == SOCKNAL_INIT_NOTHING);
	LASSERT(!ksocknal_data.ksnd_nnets);

	memset(&ksocknal_data, 0, sizeof(ksocknal_data)); /* zero pointers */

	ksocknal_data.ksnd_peer_hash_size = SOCKNAL_PEER_HASH_SIZE;
	LIBCFS_ALLOC(ksocknal_data.ksnd_peers,
		     sizeof(struct list_head) *
		     ksocknal_data.ksnd_peer_hash_size);
	if (!ksocknal_data.ksnd_peers)
>>>>>>> v4.9.227
		return -ENOMEM;

	for (i = 0; i < ksocknal_data.ksnd_peer_hash_size; i++)
		INIT_LIST_HEAD(&ksocknal_data.ksnd_peers[i]);

	rwlock_init(&ksocknal_data.ksnd_global_lock);
	INIT_LIST_HEAD(&ksocknal_data.ksnd_nets);

	spin_lock_init(&ksocknal_data.ksnd_reaper_lock);
<<<<<<< HEAD
	INIT_LIST_HEAD (&ksocknal_data.ksnd_enomem_conns);
	INIT_LIST_HEAD (&ksocknal_data.ksnd_zombie_conns);
	INIT_LIST_HEAD (&ksocknal_data.ksnd_deathrow_conns);
	init_waitqueue_head(&ksocknal_data.ksnd_reaper_waitq);

	spin_lock_init(&ksocknal_data.ksnd_connd_lock);
	INIT_LIST_HEAD (&ksocknal_data.ksnd_connd_connreqs);
	INIT_LIST_HEAD (&ksocknal_data.ksnd_connd_routes);
	init_waitqueue_head(&ksocknal_data.ksnd_connd_waitq);

	spin_lock_init(&ksocknal_data.ksnd_tx_lock);
	INIT_LIST_HEAD (&ksocknal_data.ksnd_idle_noop_txs);
=======
	INIT_LIST_HEAD(&ksocknal_data.ksnd_enomem_conns);
	INIT_LIST_HEAD(&ksocknal_data.ksnd_zombie_conns);
	INIT_LIST_HEAD(&ksocknal_data.ksnd_deathrow_conns);
	init_waitqueue_head(&ksocknal_data.ksnd_reaper_waitq);

	spin_lock_init(&ksocknal_data.ksnd_connd_lock);
	INIT_LIST_HEAD(&ksocknal_data.ksnd_connd_connreqs);
	INIT_LIST_HEAD(&ksocknal_data.ksnd_connd_routes);
	init_waitqueue_head(&ksocknal_data.ksnd_connd_waitq);

	spin_lock_init(&ksocknal_data.ksnd_tx_lock);
	INIT_LIST_HEAD(&ksocknal_data.ksnd_idle_noop_txs);
>>>>>>> v4.9.227

	/* NB memset above zeros whole of ksocknal_data */

	/* flag lists/ptrs/locks initialised */
	ksocknal_data.ksnd_init = SOCKNAL_INIT_DATA;
	try_module_get(THIS_MODULE);

	ksocknal_data.ksnd_sched_info = cfs_percpt_alloc(lnet_cpt_table(),
							 sizeof(*info));
<<<<<<< HEAD
	if (ksocknal_data.ksnd_sched_info == NULL)
		goto failed;

	cfs_percpt_for_each(info, i, ksocknal_data.ksnd_sched_info) {
		ksock_sched_t	*sched;
		int		nthrs;
=======
	if (!ksocknal_data.ksnd_sched_info)
		goto failed;

	cfs_percpt_for_each(info, i, ksocknal_data.ksnd_sched_info) {
		struct ksock_sched *sched;
		int nthrs;
>>>>>>> v4.9.227

		nthrs = cfs_cpt_weight(lnet_cpt_table(), i);
		if (*ksocknal_tunables.ksnd_nscheds > 0) {
			nthrs = min(nthrs, *ksocknal_tunables.ksnd_nscheds);
		} else {
<<<<<<< HEAD
			/* max to half of CPUs, assume another half should be
			 * reserved for upper layer modules */
=======
			/*
			 * max to half of CPUs, assume another half should be
			 * reserved for upper layer modules
			 */
>>>>>>> v4.9.227
			nthrs = min(max(SOCKNAL_NSCHEDS, nthrs >> 1), nthrs);
		}

		info->ksi_nthreads_max = nthrs;
		info->ksi_cpt = i;

		LIBCFS_CPT_ALLOC(info->ksi_scheds, lnet_cpt_table(), i,
				 info->ksi_nthreads_max * sizeof(*sched));
<<<<<<< HEAD
		if (info->ksi_scheds == NULL)
=======
		if (!info->ksi_scheds)
>>>>>>> v4.9.227
			goto failed;

		for (; nthrs > 0; nthrs--) {
			sched = &info->ksi_scheds[nthrs - 1];

			sched->kss_info = info;
			spin_lock_init(&sched->kss_lock);
			INIT_LIST_HEAD(&sched->kss_rx_conns);
			INIT_LIST_HEAD(&sched->kss_tx_conns);
			INIT_LIST_HEAD(&sched->kss_zombie_noop_txs);
			init_waitqueue_head(&sched->kss_waitq);
		}
	}

<<<<<<< HEAD
	ksocknal_data.ksnd_connd_starting	 = 0;
	ksocknal_data.ksnd_connd_failed_stamp     = 0;
	ksocknal_data.ksnd_connd_starting_stamp   = get_seconds();
	/* must have at least 2 connds to remain responsive to accepts while
	 * connecting */
=======
	ksocknal_data.ksnd_connd_starting       = 0;
	ksocknal_data.ksnd_connd_failed_stamp   = 0;
	ksocknal_data.ksnd_connd_starting_stamp = ktime_get_real_seconds();
	/*
	 * must have at least 2 connds to remain responsive to accepts while
	 * connecting
	 */
>>>>>>> v4.9.227
	if (*ksocknal_tunables.ksnd_nconnds < SOCKNAL_CONND_RESV + 1)
		*ksocknal_tunables.ksnd_nconnds = SOCKNAL_CONND_RESV + 1;

	if (*ksocknal_tunables.ksnd_nconnds_max <
	    *ksocknal_tunables.ksnd_nconnds) {
		ksocknal_tunables.ksnd_nconnds_max =
			ksocknal_tunables.ksnd_nconnds;
	}

	for (i = 0; i < *ksocknal_tunables.ksnd_nconnds; i++) {
		char name[16];
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
		spin_lock_bh(&ksocknal_data.ksnd_connd_lock);
		ksocknal_data.ksnd_connd_starting++;
		spin_unlock_bh(&ksocknal_data.ksnd_connd_lock);

<<<<<<< HEAD

		snprintf(name, sizeof(name), "socknal_cd%02d", i);
		rc = ksocknal_thread_start(ksocknal_connd,
					   (void *)((ulong_ptr_t)i), name);
		if (rc != 0) {
=======
		snprintf(name, sizeof(name), "socknal_cd%02d", i);
		rc = ksocknal_thread_start(ksocknal_connd,
					   (void *)((ulong_ptr_t)i), name);
		if (rc) {
>>>>>>> v4.9.227
			spin_lock_bh(&ksocknal_data.ksnd_connd_lock);
			ksocknal_data.ksnd_connd_starting--;
			spin_unlock_bh(&ksocknal_data.ksnd_connd_lock);
			CERROR("Can't spawn socknal connd: %d\n", rc);
			goto failed;
		}
	}

	rc = ksocknal_thread_start(ksocknal_reaper, NULL, "socknal_reaper");
<<<<<<< HEAD
	if (rc != 0) {
		CERROR ("Can't spawn socknal reaper: %d\n", rc);
=======
	if (rc) {
		CERROR("Can't spawn socknal reaper: %d\n", rc);
>>>>>>> v4.9.227
		goto failed;
	}

	/* flag everything initialised */
	ksocknal_data.ksnd_init = SOCKNAL_INIT_ALL;

	return 0;

 failed:
	ksocknal_base_shutdown();
	return -ENETDOWN;
}

static void
<<<<<<< HEAD
ksocknal_debug_peerhash (lnet_ni_t *ni)
{
	ksock_peer_t	*peer = NULL;
	struct list_head	*tmp;
	int		i;
=======
ksocknal_debug_peerhash(lnet_ni_t *ni)
{
	struct ksock_peer *peer = NULL;
	struct list_head *tmp;
	int i;
>>>>>>> v4.9.227

	read_lock(&ksocknal_data.ksnd_global_lock);

	for (i = 0; i < ksocknal_data.ksnd_peer_hash_size; i++) {
<<<<<<< HEAD
		list_for_each (tmp, &ksocknal_data.ksnd_peers[i]) {
			peer = list_entry (tmp, ksock_peer_t, ksnp_list);
=======
		list_for_each(tmp, &ksocknal_data.ksnd_peers[i]) {
			peer = list_entry(tmp, struct ksock_peer, ksnp_list);
>>>>>>> v4.9.227

			if (peer->ksnp_ni == ni)
				break;

			peer = NULL;
		}
	}

<<<<<<< HEAD
	if (peer != NULL) {
		ksock_route_t *route;
		ksock_conn_t  *conn;

		CWARN ("Active peer on shutdown: %s, ref %d, scnt %d, "
		       "closing %d, accepting %d, err %d, zcookie %llu, "
		       "txq %d, zc_req %d\n", libcfs_id2str(peer->ksnp_id),
		       atomic_read(&peer->ksnp_refcount),
		       peer->ksnp_sharecount, peer->ksnp_closing,
		       peer->ksnp_accepting, peer->ksnp_error,
		       peer->ksnp_zc_next_cookie,
		       !list_empty(&peer->ksnp_tx_queue),
		       !list_empty(&peer->ksnp_zc_req_list));

		list_for_each (tmp, &peer->ksnp_routes) {
			route = list_entry(tmp, ksock_route_t, ksnr_list);
			CWARN ("Route: ref %d, schd %d, conn %d, cnted %d, "
			       "del %d\n", atomic_read(&route->ksnr_refcount),
			       route->ksnr_scheduled, route->ksnr_connecting,
			       route->ksnr_connected, route->ksnr_deleted);
		}

		list_for_each (tmp, &peer->ksnp_conns) {
			conn = list_entry(tmp, ksock_conn_t, ksnc_list);
			CWARN ("Conn: ref %d, sref %d, t %d, c %d\n",
			       atomic_read(&conn->ksnc_conn_refcount),
			       atomic_read(&conn->ksnc_sock_refcount),
			       conn->ksnc_type, conn->ksnc_closing);
=======
	if (peer) {
		struct ksock_route *route;
		struct ksock_conn  *conn;

		CWARN("Active peer on shutdown: %s, ref %d, scnt %d, closing %d, accepting %d, err %d, zcookie %llu, txq %d, zc_req %d\n",
		      libcfs_id2str(peer->ksnp_id),
		      atomic_read(&peer->ksnp_refcount),
		      peer->ksnp_sharecount, peer->ksnp_closing,
		      peer->ksnp_accepting, peer->ksnp_error,
		      peer->ksnp_zc_next_cookie,
		      !list_empty(&peer->ksnp_tx_queue),
		      !list_empty(&peer->ksnp_zc_req_list));

		list_for_each(tmp, &peer->ksnp_routes) {
			route = list_entry(tmp, struct ksock_route, ksnr_list);
			CWARN("Route: ref %d, schd %d, conn %d, cnted %d, del %d\n",
			      atomic_read(&route->ksnr_refcount),
			      route->ksnr_scheduled, route->ksnr_connecting,
			      route->ksnr_connected, route->ksnr_deleted);
		}

		list_for_each(tmp, &peer->ksnp_conns) {
			conn = list_entry(tmp, struct ksock_conn, ksnc_list);
			CWARN("Conn: ref %d, sref %d, t %d, c %d\n",
			      atomic_read(&conn->ksnc_conn_refcount),
			      atomic_read(&conn->ksnc_sock_refcount),
			      conn->ksnc_type, conn->ksnc_closing);
>>>>>>> v4.9.227
		}
	}

	read_unlock(&ksocknal_data.ksnd_global_lock);
<<<<<<< HEAD
	return;
}

void
ksocknal_shutdown (lnet_ni_t *ni)
{
	ksock_net_t      *net = ni->ni_data;
	int	       i;
	lnet_process_id_t anyid = {0};

	anyid.nid =  LNET_NID_ANY;
	anyid.pid =  LNET_PID_ANY;
=======
}

void
ksocknal_shutdown(lnet_ni_t *ni)
{
	struct ksock_net *net = ni->ni_data;
	int i;
	lnet_process_id_t anyid = {0};

	anyid.nid = LNET_NID_ANY;
	anyid.pid = LNET_PID_ANY;
>>>>>>> v4.9.227

	LASSERT(ksocknal_data.ksnd_init == SOCKNAL_INIT_ALL);
	LASSERT(ksocknal_data.ksnd_nnets > 0);

	spin_lock_bh(&net->ksnn_lock);
	net->ksnn_shutdown = 1;		 /* prevent new peers */
	spin_unlock_bh(&net->ksnn_lock);

	/* Delete all peers */
	ksocknal_del_peer(ni, anyid, 0);

	/* Wait for all peer state to clean up */
	i = 2;
	spin_lock_bh(&net->ksnn_lock);
<<<<<<< HEAD
	while (net->ksnn_npeers != 0) {
=======
	while (net->ksnn_npeers) {
>>>>>>> v4.9.227
		spin_unlock_bh(&net->ksnn_lock);

		i++;
		CDEBUG(((i & (-i)) == i) ? D_WARNING : D_NET, /* power of 2? */
		       "waiting for %d peers to disconnect\n",
		       net->ksnn_npeers);
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(cfs_time_seconds(1));

		ksocknal_debug_peerhash(ni);

		spin_lock_bh(&net->ksnn_lock);
	}
	spin_unlock_bh(&net->ksnn_lock);

	for (i = 0; i < net->ksnn_ninterfaces; i++) {
<<<<<<< HEAD
		LASSERT (net->ksnn_interfaces[i].ksni_npeers == 0);
		LASSERT (net->ksnn_interfaces[i].ksni_nroutes == 0);
=======
		LASSERT(!net->ksnn_interfaces[i].ksni_npeers);
		LASSERT(!net->ksnn_interfaces[i].ksni_nroutes);
>>>>>>> v4.9.227
	}

	list_del(&net->ksnn_list);
	LIBCFS_FREE(net, sizeof(*net));

	ksocknal_data.ksnd_nnets--;
<<<<<<< HEAD
	if (ksocknal_data.ksnd_nnets == 0)
=======
	if (!ksocknal_data.ksnd_nnets)
>>>>>>> v4.9.227
		ksocknal_base_shutdown();
}

static int
<<<<<<< HEAD
ksocknal_enumerate_interfaces(ksock_net_t *net)
{
	char      **names;
	int	 i;
	int	 j;
	int	 rc;
	int	 n;

	n = libcfs_ipif_enumerate(&names);
=======
ksocknal_enumerate_interfaces(struct ksock_net *net)
{
	char **names;
	int i;
	int j;
	int rc;
	int n;

	n = lnet_ipif_enumerate(&names);
>>>>>>> v4.9.227
	if (n <= 0) {
		CERROR("Can't enumerate interfaces: %d\n", n);
		return n;
	}

	for (i = j = 0; i < n; i++) {
<<<<<<< HEAD
		int	up;
		__u32      ip;
		__u32      mask;
=======
		int up;
		__u32 ip;
		__u32 mask;
>>>>>>> v4.9.227

		if (!strcmp(names[i], "lo")) /* skip the loopback IF */
			continue;

<<<<<<< HEAD
		rc = libcfs_ipif_query(names[i], &up, &ip, &mask);
		if (rc != 0) {
=======
		rc = lnet_ipif_query(names[i], &up, &ip, &mask);
		if (rc) {
>>>>>>> v4.9.227
			CWARN("Can't get interface %s info: %d\n",
			      names[i], rc);
			continue;
		}

		if (!up) {
			CWARN("Ignoring interface %s (down)\n",
			      names[i]);
			continue;
		}

		if (j == LNET_MAX_INTERFACES) {
			CWARN("Ignoring interface %s (too many interfaces)\n",
			      names[i]);
			continue;
		}

		net->ksnn_interfaces[j].ksni_ipaddr = ip;
		net->ksnn_interfaces[j].ksni_netmask = mask;
<<<<<<< HEAD
		strncpy(&net->ksnn_interfaces[j].ksni_name[0],
			names[i], IFNAMSIZ);
		j++;
	}

	libcfs_ipif_free_enumeration(names, n);

	if (j == 0)
=======
		strlcpy(net->ksnn_interfaces[j].ksni_name,
			names[i], sizeof(net->ksnn_interfaces[j].ksni_name));
		j++;
	}

	lnet_ipif_free_enumeration(names, n);

	if (!j)
>>>>>>> v4.9.227
		CERROR("Can't find any usable interfaces\n");

	return j;
}

static int
<<<<<<< HEAD
ksocknal_search_new_ipif(ksock_net_t *net)
{
	int	new_ipif = 0;
	int	i;

	for (i = 0; i < net->ksnn_ninterfaces; i++) {
		char		*ifnam = &net->ksnn_interfaces[i].ksni_name[0];
		char		*colon = strchr(ifnam, ':');
		int		found  = 0;
		ksock_net_t	*tmp;
		int		j;

		if (colon != NULL) /* ignore alias device */
			*colon = 0;

		list_for_each_entry(tmp, &ksocknal_data.ksnd_nets,
					ksnn_list) {
=======
ksocknal_search_new_ipif(struct ksock_net *net)
{
	int new_ipif = 0;
	int i;

	for (i = 0; i < net->ksnn_ninterfaces; i++) {
		char *ifnam = &net->ksnn_interfaces[i].ksni_name[0];
		char *colon = strchr(ifnam, ':');
		int found  = 0;
		struct ksock_net *tmp;
		int j;

		if (colon) /* ignore alias device */
			*colon = 0;

		list_for_each_entry(tmp, &ksocknal_data.ksnd_nets, ksnn_list) {
>>>>>>> v4.9.227
			for (j = 0; !found && j < tmp->ksnn_ninterfaces; j++) {
				char *ifnam2 =
					&tmp->ksnn_interfaces[j].ksni_name[0];
				char *colon2 = strchr(ifnam2, ':');

<<<<<<< HEAD
				if (colon2 != NULL)
					*colon2 = 0;

				found = strcmp(ifnam, ifnam2) == 0;
				if (colon2 != NULL)
=======
				if (colon2)
					*colon2 = 0;

				found = !strcmp(ifnam, ifnam2);
				if (colon2)
>>>>>>> v4.9.227
					*colon2 = ':';
			}
			if (found)
				break;
		}

		new_ipif += !found;
<<<<<<< HEAD
		if (colon != NULL)
=======
		if (colon)
>>>>>>> v4.9.227
			*colon = ':';
	}

	return new_ipif;
}

static int
ksocknal_start_schedulers(struct ksock_sched_info *info)
{
<<<<<<< HEAD
	int	nthrs;
	int	rc = 0;
	int	i;

	if (info->ksi_nthreads == 0) {
=======
	int nthrs;
	int rc = 0;
	int i;

	if (!info->ksi_nthreads) {
>>>>>>> v4.9.227
		if (*ksocknal_tunables.ksnd_nscheds > 0) {
			nthrs = info->ksi_nthreads_max;
		} else {
			nthrs = cfs_cpt_weight(lnet_cpt_table(),
					       info->ksi_cpt);
			nthrs = min(max(SOCKNAL_NSCHEDS, nthrs >> 1), nthrs);
			nthrs = min(SOCKNAL_NSCHEDS_HIGH, nthrs);
		}
		nthrs = min(nthrs, info->ksi_nthreads_max);
	} else {
		LASSERT(info->ksi_nthreads <= info->ksi_nthreads_max);
		/* increase two threads if there is new interface */
		nthrs = min(2, info->ksi_nthreads_max - info->ksi_nthreads);
	}

	for (i = 0; i < nthrs; i++) {
<<<<<<< HEAD
		long		id;
		char		name[20];
		ksock_sched_t	*sched;
=======
		long id;
		char name[20];
		struct ksock_sched *sched;

>>>>>>> v4.9.227
		id = KSOCK_THREAD_ID(info->ksi_cpt, info->ksi_nthreads + i);
		sched = &info->ksi_scheds[KSOCK_THREAD_SID(id)];
		snprintf(name, sizeof(name), "socknal_sd%02d_%02d",
			 info->ksi_cpt, (int)(sched - &info->ksi_scheds[0]));

		rc = ksocknal_thread_start(ksocknal_scheduler,
					   (void *)id, name);
<<<<<<< HEAD
		if (rc == 0)
=======
		if (!rc)
>>>>>>> v4.9.227
			continue;

		CERROR("Can't spawn thread %d for scheduler[%d]: %d\n",
		       info->ksi_cpt, info->ksi_nthreads + i, rc);
		break;
	}

	info->ksi_nthreads += i;
	return rc;
}

static int
<<<<<<< HEAD
ksocknal_net_start_threads(ksock_net_t *net, __u32 *cpts, int ncpts)
{
	int	newif = ksocknal_search_new_ipif(net);
	int	rc;
	int	i;
=======
ksocknal_net_start_threads(struct ksock_net *net, __u32 *cpts, int ncpts)
{
	int newif = ksocknal_search_new_ipif(net);
	int rc;
	int i;
>>>>>>> v4.9.227

	LASSERT(ncpts > 0 && ncpts <= cfs_cpt_number(lnet_cpt_table()));

	for (i = 0; i < ncpts; i++) {
<<<<<<< HEAD
		struct ksock_sched_info	*info;
		int cpt = (cpts == NULL) ? i : cpts[i];
=======
		struct ksock_sched_info *info;
		int cpt = !cpts ? i : cpts[i];
>>>>>>> v4.9.227

		LASSERT(cpt < cfs_cpt_number(lnet_cpt_table()));
		info = ksocknal_data.ksnd_sched_info[cpt];

		if (!newif && info->ksi_nthreads > 0)
			continue;

		rc = ksocknal_start_schedulers(info);
<<<<<<< HEAD
		if (rc != 0)
=======
		if (rc)
>>>>>>> v4.9.227
			return rc;
	}
	return 0;
}

int
<<<<<<< HEAD
ksocknal_startup (lnet_ni_t *ni)
{
	ksock_net_t  *net;
	int	   rc;
	int	   i;

	LASSERT (ni->ni_lnd == &the_ksocklnd);

	if (ksocknal_data.ksnd_init == SOCKNAL_INIT_NOTHING) {
		rc = ksocknal_base_startup();
		if (rc != 0)
=======
ksocknal_startup(lnet_ni_t *ni)
{
	struct ksock_net *net;
	int rc;
	int i;

	LASSERT(ni->ni_lnd == &the_ksocklnd);

	if (ksocknal_data.ksnd_init == SOCKNAL_INIT_NOTHING) {
		rc = ksocknal_base_startup();
		if (rc)
>>>>>>> v4.9.227
			return rc;
	}

	LIBCFS_ALLOC(net, sizeof(*net));
<<<<<<< HEAD
	if (net == NULL)
=======
	if (!net)
>>>>>>> v4.9.227
		goto fail_0;

	spin_lock_init(&net->ksnn_lock);
	net->ksnn_incarnation = ksocknal_new_incarnation();
	ni->ni_data = net;
	ni->ni_peertimeout    = *ksocknal_tunables.ksnd_peertimeout;
	ni->ni_maxtxcredits   = *ksocknal_tunables.ksnd_credits;
	ni->ni_peertxcredits  = *ksocknal_tunables.ksnd_peertxcredits;
	ni->ni_peerrtrcredits = *ksocknal_tunables.ksnd_peerrtrcredits;

<<<<<<< HEAD
	if (ni->ni_interfaces[0] == NULL) {
=======
	if (!ni->ni_interfaces[0]) {
>>>>>>> v4.9.227
		rc = ksocknal_enumerate_interfaces(net);
		if (rc <= 0)
			goto fail_1;

		net->ksnn_ninterfaces = 1;
	} else {
		for (i = 0; i < LNET_MAX_INTERFACES; i++) {
<<<<<<< HEAD
			int    up;

			if (ni->ni_interfaces[i] == NULL)
				break;

			rc = libcfs_ipif_query(
				ni->ni_interfaces[i], &up,
				&net->ksnn_interfaces[i].ksni_ipaddr,
				&net->ksnn_interfaces[i].ksni_netmask);

			if (rc != 0) {
=======
			int up;

			if (!ni->ni_interfaces[i])
				break;

			rc = lnet_ipif_query(ni->ni_interfaces[i], &up,
					     &net->ksnn_interfaces[i].ksni_ipaddr,
					     &net->ksnn_interfaces[i].ksni_netmask);

			if (rc) {
>>>>>>> v4.9.227
				CERROR("Can't get interface %s info: %d\n",
				       ni->ni_interfaces[i], rc);
				goto fail_1;
			}

			if (!up) {
				CERROR("Interface %s is down\n",
				       ni->ni_interfaces[i]);
				goto fail_1;
			}

<<<<<<< HEAD
			strncpy(&net->ksnn_interfaces[i].ksni_name[0],
				ni->ni_interfaces[i], IFNAMSIZ);
=======
			strlcpy(net->ksnn_interfaces[i].ksni_name,
				ni->ni_interfaces[i],
				sizeof(net->ksnn_interfaces[i].ksni_name));
>>>>>>> v4.9.227
		}
		net->ksnn_ninterfaces = i;
	}

	/* call it before add it to ksocknal_data.ksnd_nets */
	rc = ksocknal_net_start_threads(net, ni->ni_cpts, ni->ni_ncpts);
<<<<<<< HEAD
	if (rc != 0)
=======
	if (rc)
>>>>>>> v4.9.227
		goto fail_1;

	ni->ni_nid = LNET_MKNID(LNET_NIDNET(ni->ni_nid),
				net->ksnn_interfaces[0].ksni_ipaddr);
	list_add(&net->ksnn_list, &ksocknal_data.ksnd_nets);

	ksocknal_data.ksnd_nnets++;

	return 0;

 fail_1:
	LIBCFS_FREE(net, sizeof(*net));
 fail_0:
<<<<<<< HEAD
	if (ksocknal_data.ksnd_nnets == 0)
=======
	if (!ksocknal_data.ksnd_nnets)
>>>>>>> v4.9.227
		ksocknal_base_shutdown();

	return -ENETDOWN;
}

<<<<<<< HEAD

static void __exit
ksocknal_module_fini (void)
=======
static void __exit ksocklnd_exit(void)
>>>>>>> v4.9.227
{
	lnet_unregister_lnd(&the_ksocklnd);
}

<<<<<<< HEAD
static int __init
ksocknal_module_init (void)
{
	int    rc;

	/* check ksnr_connected/connecting field large enough */
	CLASSERT (SOCKLND_CONN_NTYPES <= 4);
	CLASSERT (SOCKLND_CONN_ACK == SOCKLND_CONN_BULK_IN);
=======
static int __init ksocklnd_init(void)
{
	int rc;

	/* check ksnr_connected/connecting field large enough */
	CLASSERT(SOCKLND_CONN_NTYPES <= 4);
	CLASSERT(SOCKLND_CONN_ACK == SOCKLND_CONN_BULK_IN);
>>>>>>> v4.9.227

	/* initialize the_ksocklnd */
	the_ksocklnd.lnd_type     = SOCKLND;
	the_ksocklnd.lnd_startup  = ksocknal_startup;
	the_ksocklnd.lnd_shutdown = ksocknal_shutdown;
	the_ksocklnd.lnd_ctl      = ksocknal_ctl;
	the_ksocklnd.lnd_send     = ksocknal_send;
	the_ksocklnd.lnd_recv     = ksocknal_recv;
	the_ksocklnd.lnd_notify   = ksocknal_notify;
	the_ksocklnd.lnd_query    = ksocknal_query;
	the_ksocklnd.lnd_accept   = ksocknal_accept;

	rc = ksocknal_tunables_init();
<<<<<<< HEAD
	if (rc != 0)
=======
	if (rc)
>>>>>>> v4.9.227
		return rc;

	lnet_register_lnd(&the_ksocklnd);

	return 0;
}

<<<<<<< HEAD
MODULE_AUTHOR("Sun Microsystems, Inc. <http://www.lustre.org/>");
MODULE_DESCRIPTION("Kernel TCP Socket LND v3.0.0");
MODULE_LICENSE("GPL");
MODULE_VERSION("3.0.0");

module_init(ksocknal_module_init);
module_exit(ksocknal_module_fini);
=======
MODULE_AUTHOR("OpenSFS, Inc. <http://www.lustre.org/>");
MODULE_DESCRIPTION("TCP Socket LNet Network Driver");
MODULE_VERSION("2.7.0");
MODULE_LICENSE("GPL");

module_init(ksocklnd_init);
module_exit(ksocklnd_exit);
>>>>>>> v4.9.227
