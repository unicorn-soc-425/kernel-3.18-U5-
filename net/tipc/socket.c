/*
 * net/tipc/socket.c: TIPC socket API
 *
<<<<<<< HEAD
 * Copyright (c) 2001-2007, 2012-2014, Ericsson AB
=======
 * Copyright (c) 2001-2007, 2012-2016, Ericsson AB
>>>>>>> v4.9.227
 * Copyright (c) 2004-2008, 2010-2013, Wind River Systems
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

<<<<<<< HEAD
=======
#include <linux/rhashtable.h>
>>>>>>> v4.9.227
#include "core.h"
#include "name_table.h"
#include "node.h"
#include "link.h"
<<<<<<< HEAD
#include <linux/export.h>
#include "config.h"
#include "socket.h"

#define SS_LISTENING	-1	/* socket is listening */
#define SS_READY	-2	/* socket is connectionless */

#define CONN_TIMEOUT_DEFAULT  8000	/* default connect timeout = 8s */
#define CONN_PROBING_INTERVAL 3600000	/* [ms] => 1 h */
#define TIPC_FWD_MSG	      1
#define TIPC_CONN_OK          0
#define TIPC_CONN_PROBING     1
=======
#include "name_distr.h"
#include "socket.h"
#include "bcast.h"
#include "netlink.h"

#define SS_LISTENING		-1	/* socket is listening */
#define SS_READY		-2	/* socket is connectionless */

#define CONN_TIMEOUT_DEFAULT	8000	/* default connect timeout = 8s */
#define CONN_PROBING_INTERVAL	msecs_to_jiffies(3600000)  /* [ms] => 1 h */
#define TIPC_FWD_MSG		1
#define TIPC_CONN_OK		0
#define TIPC_CONN_PROBING	1
#define TIPC_MAX_PORT		0xffffffff
#define TIPC_MIN_PORT		1
>>>>>>> v4.9.227

/**
 * struct tipc_sock - TIPC socket structure
 * @sk: socket - interacts with 'port' and with user via the socket API
 * @connected: non-zero if port is currently connected to a peer port
 * @conn_type: TIPC type used when connection was established
 * @conn_instance: TIPC instance used when connection was established
 * @published: non-zero if port has one or more associated names
 * @max_pkt: maximum packet size "hint" used when building messages sent by port
<<<<<<< HEAD
 * @ref: unique reference to port in TIPC object registry
=======
 * @portid: unique port identity in TIPC socket hash table
>>>>>>> v4.9.227
 * @phdr: preformatted message header used when sending messages
 * @port_list: adjacent ports in TIPC's global list of ports
 * @publications: list of publications for port
 * @pub_count: total # of publications port has made during its lifetime
 * @probing_state:
<<<<<<< HEAD
 * @probing_interval:
 * @timer:
 * @port: port - interacts with 'sk' and with the rest of the TIPC stack
 * @peer_name: the peer of the connection, if any
=======
 * @probing_intv:
>>>>>>> v4.9.227
 * @conn_timeout: the time we can wait for an unresponded setup request
 * @dupl_rcvcnt: number of bytes counted twice, in both backlog and rcv queue
 * @link_cong: non-zero if owner must sleep because of link congestion
 * @sent_unacked: # messages sent by socket, and not yet acked by peer
 * @rcv_unacked: # messages read by user, but not yet acked back to peer
<<<<<<< HEAD
=======
 * @remote: 'connected' peer for dgram/rdm
 * @node: hash table node
 * @rcu: rcu struct for tipc_sock
>>>>>>> v4.9.227
 */
struct tipc_sock {
	struct sock sk;
	int connected;
	u32 conn_type;
	u32 conn_instance;
	int published;
	u32 max_pkt;
<<<<<<< HEAD
	u32 ref;
=======
	u32 portid;
>>>>>>> v4.9.227
	struct tipc_msg phdr;
	struct list_head sock_list;
	struct list_head publications;
	u32 pub_count;
	u32 probing_state;
<<<<<<< HEAD
	u32 probing_interval;
	struct timer_list timer;
	uint conn_timeout;
	atomic_t dupl_rcvcnt;
	bool link_cong;
	uint sent_unacked;
	uint rcv_unacked;
=======
	unsigned long probing_intv;
	uint conn_timeout;
	atomic_t dupl_rcvcnt;
	bool link_cong;
	u16 snt_unacked;
	u16 snd_win;
	u16 peer_caps;
	u16 rcv_unacked;
	u16 rcv_win;
	struct sockaddr_tipc remote;
	struct rhash_head node;
	struct rcu_head rcu;
>>>>>>> v4.9.227
};

static int tipc_backlog_rcv(struct sock *sk, struct sk_buff *skb);
static void tipc_data_ready(struct sock *sk);
static void tipc_write_space(struct sock *sk);
<<<<<<< HEAD
static int tipc_release(struct socket *sock);
static int tipc_accept(struct socket *sock, struct socket *new_sock, int flags);
static int tipc_wait_for_sndmsg(struct socket *sock, long *timeo_p);
static void tipc_sk_timeout(unsigned long ref);
=======
static void tipc_sock_destruct(struct sock *sk);
static int tipc_release(struct socket *sock);
static int tipc_accept(struct socket *sock, struct socket *new_sock, int flags);
static int tipc_wait_for_sndmsg(struct socket *sock, long *timeo_p);
static void tipc_sk_timeout(unsigned long data);
>>>>>>> v4.9.227
static int tipc_sk_publish(struct tipc_sock *tsk, uint scope,
			   struct tipc_name_seq const *seq);
static int tipc_sk_withdraw(struct tipc_sock *tsk, uint scope,
			    struct tipc_name_seq const *seq);
<<<<<<< HEAD
static u32 tipc_sk_ref_acquire(struct tipc_sock *tsk);
static void tipc_sk_ref_discard(u32 ref);
static struct tipc_sock *tipc_sk_get(u32 ref);
static struct tipc_sock *tipc_sk_get_next(u32 *ref);
static void tipc_sk_put(struct tipc_sock *tsk);
=======
static struct tipc_sock *tipc_sk_lookup(struct net *net, u32 portid);
static int tipc_sk_insert(struct tipc_sock *tsk);
static void tipc_sk_remove(struct tipc_sock *tsk);
static int __tipc_send_stream(struct socket *sock, struct msghdr *m,
			      size_t dsz);
static int __tipc_sendmsg(struct socket *sock, struct msghdr *m, size_t dsz);
>>>>>>> v4.9.227

static const struct proto_ops packet_ops;
static const struct proto_ops stream_ops;
static const struct proto_ops msg_ops;
<<<<<<< HEAD

static struct proto tipc_proto;
static struct proto tipc_proto_kern;

/*
 * Revised TIPC socket locking policy:
 *
 * Most socket operations take the standard socket lock when they start
 * and hold it until they finish (or until they need to sleep).  Acquiring
 * this lock grants the owner exclusive access to the fields of the socket
 * data structures, with the exception of the backlog queue.  A few socket
 * operations can be done without taking the socket lock because they only
 * read socket information that never changes during the life of the socket.
 *
 * Socket operations may acquire the lock for the associated TIPC port if they
 * need to perform an operation on the port.  If any routine needs to acquire
 * both the socket lock and the port lock it must take the socket lock first
 * to avoid the risk of deadlock.
 *
 * The dispatcher handling incoming messages cannot grab the socket lock in
 * the standard fashion, since invoked it runs at the BH level and cannot block.
 * Instead, it checks to see if the socket lock is currently owned by someone,
 * and either handles the message itself or adds it to the socket's backlog
 * queue; in the latter case the queued message is processed once the process
 * owning the socket lock releases it.
 *
 * NOTE: Releasing the socket lock while an operation is sleeping overcomes
 * the problem of a blocked socket operation preventing any other operations
 * from occurring.  However, applications must be careful if they have
 * multiple threads trying to send (or receive) on the same socket, as these
 * operations might interfere with each other.  For example, doing a connect
 * and a receive at the same time might allow the receive to consume the
 * ACK message meant for the connect.  While additional work could be done
 * to try and overcome this, it doesn't seem to be worthwhile at the present.
 *
 * NOTE: Releasing the socket lock while an operation is sleeping also ensures
 * that another operation that must be performed in a non-blocking manner is
 * not delayed for very long because the lock has already been taken.
 *
 * NOTE: This code assumes that certain fields of a port/socket pair are
 * constant over its lifetime; such fields can be examined without taking
 * the socket lock and/or port lock, and do not need to be re-read even
 * after resuming processing after waiting.  These fields include:
 *   - socket type
 *   - pointer to socket sk structure (aka tipc_sock structure)
 *   - pointer to port structure
 *   - port reference
 */
=======
static struct proto tipc_proto;
static const struct rhashtable_params tsk_rht_params;

static u32 tsk_own_node(struct tipc_sock *tsk)
{
	return msg_prevnode(&tsk->phdr);
}
>>>>>>> v4.9.227

static u32 tsk_peer_node(struct tipc_sock *tsk)
{
	return msg_destnode(&tsk->phdr);
}

static u32 tsk_peer_port(struct tipc_sock *tsk)
{
	return msg_destport(&tsk->phdr);
}

static  bool tsk_unreliable(struct tipc_sock *tsk)
{
	return msg_src_droppable(&tsk->phdr) != 0;
}

static void tsk_set_unreliable(struct tipc_sock *tsk, bool unreliable)
{
	msg_set_src_droppable(&tsk->phdr, unreliable ? 1 : 0);
}

static bool tsk_unreturnable(struct tipc_sock *tsk)
{
	return msg_dest_droppable(&tsk->phdr) != 0;
}

static void tsk_set_unreturnable(struct tipc_sock *tsk, bool unreturnable)
{
	msg_set_dest_droppable(&tsk->phdr, unreturnable ? 1 : 0);
}

static int tsk_importance(struct tipc_sock *tsk)
{
	return msg_importance(&tsk->phdr);
}

static int tsk_set_importance(struct tipc_sock *tsk, int imp)
{
	if (imp > TIPC_CRITICAL_IMPORTANCE)
		return -EINVAL;
	msg_set_importance(&tsk->phdr, (u32)imp);
	return 0;
}

static struct tipc_sock *tipc_sk(const struct sock *sk)
{
	return container_of(sk, struct tipc_sock, sk);
}

<<<<<<< HEAD
static int tsk_conn_cong(struct tipc_sock *tsk)
{
	return tsk->sent_unacked >= TIPC_FLOWCTRL_WIN;
=======
static bool tsk_conn_cong(struct tipc_sock *tsk)
{
	return tsk->snt_unacked > tsk->snd_win;
}

/* tsk_blocks(): translate a buffer size in bytes to number of
 * advertisable blocks, taking into account the ratio truesize(len)/len
 * We can trust that this ratio is always < 4 for len >= FLOWCTL_BLK_SZ
 */
static u16 tsk_adv_blocks(int len)
{
	return len / FLOWCTL_BLK_SZ / 4;
}

/* tsk_inc(): increment counter for sent or received data
 * - If block based flow control is not supported by peer we
 *   fall back to message based ditto, incrementing the counter
 */
static u16 tsk_inc(struct tipc_sock *tsk, int msglen)
{
	if (likely(tsk->peer_caps & TIPC_BLOCK_FLOWCTL))
		return ((msglen / FLOWCTL_BLK_SZ) + 1);
	return 1;
>>>>>>> v4.9.227
}

/**
 * tsk_advance_rx_queue - discard first buffer in socket receive queue
 *
 * Caller must hold socket lock
 */
static void tsk_advance_rx_queue(struct sock *sk)
{
	kfree_skb(__skb_dequeue(&sk->sk_receive_queue));
}

<<<<<<< HEAD
=======
/* tipc_sk_respond() : send response message back to sender
 */
static void tipc_sk_respond(struct sock *sk, struct sk_buff *skb, int err)
{
	u32 selector;
	u32 dnode;
	u32 onode = tipc_own_addr(sock_net(sk));

	if (!tipc_msg_reverse(onode, &skb, err))
		return;

	dnode = msg_destnode(buf_msg(skb));
	selector = msg_origport(buf_msg(skb));
	tipc_node_xmit_skb(sock_net(sk), skb, dnode, selector);
}

>>>>>>> v4.9.227
/**
 * tsk_rej_rx_queue - reject all buffers in socket receive queue
 *
 * Caller must hold socket lock
 */
static void tsk_rej_rx_queue(struct sock *sk)
{
<<<<<<< HEAD
	struct sk_buff *buf;
	u32 dnode;

	while ((buf = __skb_dequeue(&sk->sk_receive_queue))) {
		if (tipc_msg_reverse(buf, &dnode, TIPC_ERR_NO_PORT))
			tipc_link_xmit(buf, dnode, 0);
	}
=======
	struct sk_buff *skb;

	while ((skb = __skb_dequeue(&sk->sk_receive_queue)))
		tipc_sk_respond(sk, skb, TIPC_ERR_NO_PORT);
>>>>>>> v4.9.227
}

/* tsk_peer_msg - verify if message was sent by connected port's peer
 *
 * Handles cases where the node's network address has changed from
 * the default of <0.0.0> to its configured setting.
 */
static bool tsk_peer_msg(struct tipc_sock *tsk, struct tipc_msg *msg)
{
<<<<<<< HEAD
=======
	struct tipc_net *tn = net_generic(sock_net(&tsk->sk), tipc_net_id);
>>>>>>> v4.9.227
	u32 peer_port = tsk_peer_port(tsk);
	u32 orig_node;
	u32 peer_node;

	if (unlikely(!tsk->connected))
		return false;

	if (unlikely(msg_origport(msg) != peer_port))
		return false;

	orig_node = msg_orignode(msg);
	peer_node = tsk_peer_node(tsk);

	if (likely(orig_node == peer_node))
		return true;

<<<<<<< HEAD
	if (!orig_node && (peer_node == tipc_own_addr))
		return true;

	if (!peer_node && (orig_node == tipc_own_addr))
=======
	if (!orig_node && (peer_node == tn->own_addr))
		return true;

	if (!peer_node && (orig_node == tn->own_addr))
>>>>>>> v4.9.227
		return true;

	return false;
}

/**
 * tipc_sk_create - create a TIPC socket
 * @net: network namespace (must be default network)
 * @sock: pre-allocated socket structure
 * @protocol: protocol indicator (must be 0)
 * @kern: caused by kernel or by userspace?
 *
 * This routine creates additional data structures used by the TIPC socket,
 * initializes them, and links them together.
 *
 * Returns 0 on success, errno otherwise
 */
static int tipc_sk_create(struct net *net, struct socket *sock,
			  int protocol, int kern)
{
<<<<<<< HEAD
=======
	struct tipc_net *tn;
>>>>>>> v4.9.227
	const struct proto_ops *ops;
	socket_state state;
	struct sock *sk;
	struct tipc_sock *tsk;
	struct tipc_msg *msg;
<<<<<<< HEAD
	u32 ref;
=======
>>>>>>> v4.9.227

	/* Validate arguments */
	if (unlikely(protocol != 0))
		return -EPROTONOSUPPORT;

	switch (sock->type) {
	case SOCK_STREAM:
		ops = &stream_ops;
		state = SS_UNCONNECTED;
		break;
	case SOCK_SEQPACKET:
		ops = &packet_ops;
		state = SS_UNCONNECTED;
		break;
	case SOCK_DGRAM:
	case SOCK_RDM:
		ops = &msg_ops;
		state = SS_READY;
		break;
	default:
		return -EPROTOTYPE;
	}

	/* Allocate socket's protocol area */
<<<<<<< HEAD
	if (!kern)
		sk = sk_alloc(net, AF_TIPC, GFP_KERNEL, &tipc_proto);
	else
		sk = sk_alloc(net, AF_TIPC, GFP_KERNEL, &tipc_proto_kern);

=======
	sk = sk_alloc(net, AF_TIPC, GFP_KERNEL, &tipc_proto, kern);
>>>>>>> v4.9.227
	if (sk == NULL)
		return -ENOMEM;

	tsk = tipc_sk(sk);
<<<<<<< HEAD
	ref = tipc_sk_ref_acquire(tsk);
	if (!ref) {
		pr_warn("Socket create failed; reference table exhausted\n");
		return -ENOMEM;
	}
	tsk->max_pkt = MAX_PKT_DEFAULT;
	tsk->ref = ref;
	INIT_LIST_HEAD(&tsk->publications);
	msg = &tsk->phdr;
	tipc_msg_init(msg, TIPC_LOW_IMPORTANCE, TIPC_NAMED_MSG,
		      NAMED_H_SIZE, 0);
	msg_set_origport(msg, ref);
=======
	tsk->max_pkt = MAX_PKT_DEFAULT;
	INIT_LIST_HEAD(&tsk->publications);
	msg = &tsk->phdr;
	tn = net_generic(sock_net(sk), tipc_net_id);
>>>>>>> v4.9.227

	/* Finish initializing socket data structures */
	sock->ops = ops;
	sock->state = state;
	sock_init_data(sock, sk);
<<<<<<< HEAD
	k_init_timer(&tsk->timer, (Handler)tipc_sk_timeout, ref);
=======
	if (tipc_sk_insert(tsk)) {
		pr_warn("Socket create failed; port number exhausted\n");
		return -EINVAL;
	}

	/* Ensure tsk is visible before we read own_addr. */
	smp_mb();

	tipc_msg_init(tn->own_addr, msg, TIPC_LOW_IMPORTANCE, TIPC_NAMED_MSG,
		      NAMED_H_SIZE, 0);

	msg_set_origport(msg, tsk->portid);
	setup_timer(&sk->sk_timer, tipc_sk_timeout, (unsigned long)tsk);
>>>>>>> v4.9.227
	sk->sk_backlog_rcv = tipc_backlog_rcv;
	sk->sk_rcvbuf = sysctl_tipc_rmem[1];
	sk->sk_data_ready = tipc_data_ready;
	sk->sk_write_space = tipc_write_space;
<<<<<<< HEAD
	tsk->conn_timeout = CONN_TIMEOUT_DEFAULT;
	tsk->sent_unacked = 0;
	atomic_set(&tsk->dupl_rcvcnt, 0);

=======
	sk->sk_destruct = tipc_sock_destruct;
	tsk->conn_timeout = CONN_TIMEOUT_DEFAULT;
	atomic_set(&tsk->dupl_rcvcnt, 0);

	/* Start out with safe limits until we receive an advertised window */
	tsk->snd_win = tsk_adv_blocks(RCVBUF_MIN);
	tsk->rcv_win = tsk->snd_win;

>>>>>>> v4.9.227
	if (sock->state == SS_READY) {
		tsk_set_unreturnable(tsk, true);
		if (sock->type == SOCK_DGRAM)
			tsk_set_unreliable(tsk, true);
	}
	return 0;
}

<<<<<<< HEAD
/**
 * tipc_sock_create_local - create TIPC socket from inside TIPC module
 * @type: socket type - SOCK_RDM or SOCK_SEQPACKET
 *
 * We cannot use sock_creat_kern here because it bumps module user count.
 * Since socket owner and creator is the same module we must make sure
 * that module count remains zero for module local sockets, otherwise
 * we cannot do rmmod.
 *
 * Returns 0 on success, errno otherwise
 */
int tipc_sock_create_local(int type, struct socket **res)
{
	int rc;

	rc = sock_create_lite(AF_TIPC, type, 0, res);
	if (rc < 0) {
		pr_err("Failed to create kernel socket\n");
		return rc;
	}
	tipc_sk_create(&init_net, *res, 0, 1);

	return 0;
}

/**
 * tipc_sock_release_local - release socket created by tipc_sock_create_local
 * @sock: the socket to be released.
 *
 * Module reference count is not incremented when such sockets are created,
 * so we must keep it from being decremented when they are released.
 */
void tipc_sock_release_local(struct socket *sock)
{
	tipc_release(sock);
	sock->ops = NULL;
	sock_release(sock);
}

/**
 * tipc_sock_accept_local - accept a connection on a socket created
 * with tipc_sock_create_local. Use this function to avoid that
 * module reference count is inadvertently incremented.
 *
 * @sock:    the accepting socket
 * @newsock: reference to the new socket to be created
 * @flags:   socket flags
 */

int tipc_sock_accept_local(struct socket *sock, struct socket **newsock,
			   int flags)
{
	struct sock *sk = sock->sk;
	int ret;

	ret = sock_create_lite(sk->sk_family, sk->sk_type,
			       sk->sk_protocol, newsock);
	if (ret < 0)
		return ret;

	ret = tipc_accept(sock, *newsock, flags);
	if (ret < 0) {
		sock_release(*newsock);
		return ret;
	}
	(*newsock)->ops = sock->ops;
	return ret;
=======
static void tipc_sk_callback(struct rcu_head *head)
{
	struct tipc_sock *tsk = container_of(head, struct tipc_sock, rcu);

	sock_put(&tsk->sk);
>>>>>>> v4.9.227
}

/**
 * tipc_release - destroy a TIPC socket
 * @sock: socket to destroy
 *
 * This routine cleans up any messages that are still queued on the socket.
 * For DGRAM and RDM socket types, all queued messages are rejected.
 * For SEQPACKET and STREAM socket types, the first message is rejected
 * and any others are discarded.  (If the first message on a STREAM socket
 * is partially-read, it is discarded and the next one is rejected instead.)
 *
 * NOTE: Rejected messages are not necessarily returned to the sender!  They
 * are returned or discarded according to the "destination droppable" setting
 * specified for the message by the sender.
 *
 * Returns 0 on success, errno otherwise
 */
static int tipc_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
<<<<<<< HEAD
	struct tipc_sock *tsk;
	struct sk_buff *buf;
=======
	struct net *net;
	struct tipc_sock *tsk;
	struct sk_buff *skb;
>>>>>>> v4.9.227
	u32 dnode;

	/*
	 * Exit if socket isn't fully initialized (occurs when a failed accept()
	 * releases a pre-allocated child socket that was never used)
	 */
	if (sk == NULL)
		return 0;

<<<<<<< HEAD
=======
	net = sock_net(sk);
>>>>>>> v4.9.227
	tsk = tipc_sk(sk);
	lock_sock(sk);

	/*
	 * Reject all unreceived messages, except on an active connection
	 * (which disconnects locally & sends a 'FIN+' to peer)
	 */
	dnode = tsk_peer_node(tsk);
	while (sock->state != SS_DISCONNECTING) {
<<<<<<< HEAD
		buf = __skb_dequeue(&sk->sk_receive_queue);
		if (buf == NULL)
			break;
		if (TIPC_SKB_CB(buf)->handle != NULL)
			kfree_skb(buf);
=======
		skb = __skb_dequeue(&sk->sk_receive_queue);
		if (skb == NULL)
			break;
		if (TIPC_SKB_CB(skb)->handle != NULL)
			kfree_skb(skb);
>>>>>>> v4.9.227
		else {
			if ((sock->state == SS_CONNECTING) ||
			    (sock->state == SS_CONNECTED)) {
				sock->state = SS_DISCONNECTING;
				tsk->connected = 0;
<<<<<<< HEAD
				tipc_node_remove_conn(dnode, tsk->ref);
			}
			if (tipc_msg_reverse(buf, &dnode, TIPC_ERR_NO_PORT))
				tipc_link_xmit(buf, dnode, 0);
=======
				tipc_node_remove_conn(net, dnode, tsk->portid);
			}
			tipc_sk_respond(sk, skb, TIPC_ERR_NO_PORT);
>>>>>>> v4.9.227
		}
	}

	tipc_sk_withdraw(tsk, 0, NULL);
<<<<<<< HEAD
	tipc_sk_ref_discard(tsk->ref);
	k_cancel_timer(&tsk->timer);
	if (tsk->connected) {
		buf = tipc_msg_create(TIPC_CRITICAL_IMPORTANCE, TIPC_CONN_MSG,
				      SHORT_H_SIZE, 0, dnode, tipc_own_addr,
				      tsk_peer_port(tsk),
				      tsk->ref, TIPC_ERR_NO_PORT);
		if (buf)
			tipc_link_xmit(buf, dnode, tsk->ref);
		tipc_node_remove_conn(dnode, tsk->ref);
	}
	k_term_timer(&tsk->timer);

	/* Discard any remaining (connection-based) messages in receive queue */
	__skb_queue_purge(&sk->sk_receive_queue);
=======
	sk_stop_timer(sk, &sk->sk_timer);
	tipc_sk_remove(tsk);
	if (tsk->connected) {
		skb = tipc_msg_create(TIPC_CRITICAL_IMPORTANCE,
				      TIPC_CONN_MSG, SHORT_H_SIZE, 0, dnode,
				      tsk_own_node(tsk), tsk_peer_port(tsk),
				      tsk->portid, TIPC_ERR_NO_PORT);
		if (skb)
			tipc_node_xmit_skb(net, skb, dnode, tsk->portid);
		tipc_node_remove_conn(net, dnode, tsk->portid);
	}
>>>>>>> v4.9.227

	/* Reject any messages that accumulated in backlog queue */
	sock->state = SS_DISCONNECTING;
	release_sock(sk);
<<<<<<< HEAD
	sock_put(sk);
=======

	call_rcu(&tsk->rcu, tipc_sk_callback);
>>>>>>> v4.9.227
	sock->sk = NULL;

	return 0;
}

/**
 * tipc_bind - associate or disassocate TIPC name(s) with a socket
 * @sock: socket structure
 * @uaddr: socket address describing name(s) and desired operation
 * @uaddr_len: size of socket address data structure
 *
 * Name and name sequence binding is indicated using a positive scope value;
 * a negative scope value unbinds the specified name.  Specifying no name
 * (i.e. a socket address length of 0) unbinds all names from the socket.
 *
 * Returns 0 on success, errno otherwise
 *
 * NOTE: This routine doesn't need to take the socket lock since it doesn't
 *       access any non-constant socket information.
 */
static int tipc_bind(struct socket *sock, struct sockaddr *uaddr,
		     int uaddr_len)
{
	struct sock *sk = sock->sk;
	struct sockaddr_tipc *addr = (struct sockaddr_tipc *)uaddr;
	struct tipc_sock *tsk = tipc_sk(sk);
	int res = -EINVAL;

	lock_sock(sk);
	if (unlikely(!uaddr_len)) {
		res = tipc_sk_withdraw(tsk, 0, NULL);
		goto exit;
	}

	if (uaddr_len < sizeof(struct sockaddr_tipc)) {
		res = -EINVAL;
		goto exit;
	}
	if (addr->family != AF_TIPC) {
		res = -EAFNOSUPPORT;
		goto exit;
	}

	if (addr->addrtype == TIPC_ADDR_NAME)
		addr->addr.nameseq.upper = addr->addr.nameseq.lower;
	else if (addr->addrtype != TIPC_ADDR_NAMESEQ) {
		res = -EAFNOSUPPORT;
		goto exit;
	}

	if ((addr->addr.nameseq.type < TIPC_RESERVED_TYPES) &&
	    (addr->addr.nameseq.type != TIPC_TOP_SRV) &&
	    (addr->addr.nameseq.type != TIPC_CFG_SRV)) {
		res = -EACCES;
		goto exit;
	}

	res = (addr->scope > 0) ?
		tipc_sk_publish(tsk, addr->scope, &addr->addr.nameseq) :
		tipc_sk_withdraw(tsk, -addr->scope, &addr->addr.nameseq);
exit:
	release_sock(sk);
	return res;
}

/**
 * tipc_getname - get port ID of socket or peer socket
 * @sock: socket structure
 * @uaddr: area for returned socket address
 * @uaddr_len: area for returned length of socket address
 * @peer: 0 = own ID, 1 = current peer ID, 2 = current/former peer ID
 *
 * Returns 0 on success, errno otherwise
 *
 * NOTE: This routine doesn't need to take the socket lock since it only
 *       accesses socket information that is unchanging (or which changes in
 *       a completely predictable manner).
 */
static int tipc_getname(struct socket *sock, struct sockaddr *uaddr,
			int *uaddr_len, int peer)
{
	struct sockaddr_tipc *addr = (struct sockaddr_tipc *)uaddr;
	struct tipc_sock *tsk = tipc_sk(sock->sk);
<<<<<<< HEAD
=======
	struct tipc_net *tn = net_generic(sock_net(sock->sk), tipc_net_id);
>>>>>>> v4.9.227

	memset(addr, 0, sizeof(*addr));
	if (peer) {
		if ((sock->state != SS_CONNECTED) &&
			((peer != 2) || (sock->state != SS_DISCONNECTING)))
			return -ENOTCONN;
		addr->addr.id.ref = tsk_peer_port(tsk);
		addr->addr.id.node = tsk_peer_node(tsk);
	} else {
<<<<<<< HEAD
		addr->addr.id.ref = tsk->ref;
		addr->addr.id.node = tipc_own_addr;
=======
		addr->addr.id.ref = tsk->portid;
		addr->addr.id.node = tn->own_addr;
>>>>>>> v4.9.227
	}

	*uaddr_len = sizeof(*addr);
	addr->addrtype = TIPC_ADDR_ID;
	addr->family = AF_TIPC;
	addr->scope = 0;
	addr->addr.name.domain = 0;

	return 0;
}

/**
 * tipc_poll - read and possibly block on pollmask
 * @file: file structure associated with the socket
 * @sock: socket for which to calculate the poll bits
 * @wait: ???
 *
 * Returns pollmask value
 *
 * COMMENTARY:
 * It appears that the usual socket locking mechanisms are not useful here
 * since the pollmask info is potentially out-of-date the moment this routine
 * exits.  TCP and other protocols seem to rely on higher level poll routines
 * to handle any preventable race conditions, so TIPC will do the same ...
 *
 * TIPC sets the returned events as follows:
 *
 * socket state		flags set
 * ------------		---------
 * unconnected		no read flags
 *			POLLOUT if port is not congested
 *
 * connecting		POLLIN/POLLRDNORM if ACK/NACK in rx queue
 *			no write flags
 *
 * connected		POLLIN/POLLRDNORM if data in rx queue
 *			POLLOUT if port is not congested
 *
 * disconnecting	POLLIN/POLLRDNORM/POLLHUP
 *			no write flags
 *
 * listening		POLLIN if SYN in rx queue
 *			no write flags
 *
 * ready		POLLIN/POLLRDNORM if data in rx queue
 * [connectionless]	POLLOUT (since port cannot be congested)
 *
 * IMPORTANT: The fact that a read or write operation is indicated does NOT
 * imply that the operation will succeed, merely that it should be performed
 * and will not block.
 */
static unsigned int tipc_poll(struct file *file, struct socket *sock,
			      poll_table *wait)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	u32 mask = 0;

	sock_poll_wait(file, sk_sleep(sk), wait);

	switch ((int)sock->state) {
	case SS_UNCONNECTED:
		if (!tsk->link_cong)
			mask |= POLLOUT;
		break;
	case SS_READY:
	case SS_CONNECTED:
		if (!tsk->link_cong && !tsk_conn_cong(tsk))
			mask |= POLLOUT;
		/* fall thru' */
	case SS_CONNECTING:
	case SS_LISTENING:
		if (!skb_queue_empty(&sk->sk_receive_queue))
			mask |= (POLLIN | POLLRDNORM);
		break;
	case SS_DISCONNECTING:
		mask = (POLLIN | POLLRDNORM | POLLHUP);
		break;
	}

	return mask;
}

/**
 * tipc_sendmcast - send multicast message
 * @sock: socket structure
 * @seq: destination address
<<<<<<< HEAD
 * @iov: message data to send
=======
 * @msg: message to send
>>>>>>> v4.9.227
 * @dsz: total length of message data
 * @timeo: timeout to wait for wakeup
 *
 * Called from function tipc_sendmsg(), which has done all sanity checks
 * Returns the number of bytes sent on success, or errno
 */
static int tipc_sendmcast(struct  socket *sock, struct tipc_name_seq *seq,
<<<<<<< HEAD
			  struct iovec *iov, size_t dsz, long timeo)
{
	struct sock *sk = sock->sk;
	struct tipc_msg *mhdr = &tipc_sk(sk)->phdr;
	struct sk_buff *buf;
=======
			  struct msghdr *msg, size_t dsz, long timeo)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct net *net = sock_net(sk);
	struct tipc_msg *mhdr = &tsk->phdr;
	struct sk_buff_head pktchain;
	struct iov_iter save = msg->msg_iter;
>>>>>>> v4.9.227
	uint mtu;
	int rc;

	msg_set_type(mhdr, TIPC_MCAST_MSG);
	msg_set_lookup_scope(mhdr, TIPC_CLUSTER_SCOPE);
	msg_set_destport(mhdr, 0);
	msg_set_destnode(mhdr, 0);
	msg_set_nametype(mhdr, seq->type);
	msg_set_namelower(mhdr, seq->lower);
	msg_set_nameupper(mhdr, seq->upper);
	msg_set_hdr_sz(mhdr, MCAST_H_SIZE);

<<<<<<< HEAD
new_mtu:
	mtu = tipc_bclink_get_mtu();
	rc = tipc_msg_build(mhdr, iov, 0, dsz, mtu, &buf);
=======
	skb_queue_head_init(&pktchain);

new_mtu:
	mtu = tipc_bcast_get_mtu(net);
	rc = tipc_msg_build(mhdr, msg, 0, dsz, mtu, &pktchain);
>>>>>>> v4.9.227
	if (unlikely(rc < 0))
		return rc;

	do {
<<<<<<< HEAD
		rc = tipc_bclink_xmit(buf);
		if (likely(rc >= 0)) {
			rc = dsz;
			break;
		}
		if (rc == -EMSGSIZE)
			goto new_mtu;
		if (rc != -ELINKCONG)
			break;
		tipc_sk(sk)->link_cong = 1;
		rc = tipc_wait_for_sndmsg(sock, &timeo);
		if (rc)
			kfree_skb_list(buf);
	} while (!rc);
	return rc;
}

/* tipc_sk_mcast_rcv - Deliver multicast message to all destination sockets
 */
void tipc_sk_mcast_rcv(struct sk_buff *buf)
{
	struct tipc_msg *msg = buf_msg(buf);
	struct tipc_port_list dports = {0, NULL, };
	struct tipc_port_list *item;
	struct sk_buff *b;
	uint i, last, dst = 0;
	u32 scope = TIPC_CLUSTER_SCOPE;

	if (in_own_node(msg_orignode(msg)))
		scope = TIPC_NODE_SCOPE;

	/* Create destination port list: */
	tipc_nametbl_mc_translate(msg_nametype(msg),
				  msg_namelower(msg),
				  msg_nameupper(msg),
				  scope,
				  &dports);
	last = dports.count;
	if (!last) {
		kfree_skb(buf);
		return;
	}

	for (item = &dports; item; item = item->next) {
		for (i = 0; i < PLSIZE && ++dst <= last; i++) {
			b = (dst != last) ? skb_clone(buf, GFP_ATOMIC) : buf;
			if (!b) {
				pr_warn("Failed do clone mcast rcv buffer\n");
				continue;
			}
			msg_set_destport(msg, item->ports[i]);
			tipc_sk_rcv(b);
		}
	}
	tipc_port_list_free(&dports);
=======
		rc = tipc_bcast_xmit(net, &pktchain);
		if (likely(!rc))
			return dsz;

		if (rc == -ELINKCONG) {
			tsk->link_cong = 1;
			rc = tipc_wait_for_sndmsg(sock, &timeo);
			if (!rc)
				continue;
		}
		__skb_queue_purge(&pktchain);
		if (rc == -EMSGSIZE) {
			msg->msg_iter = save;
			goto new_mtu;
		}
		break;
	} while (1);
	return rc;
}

/**
 * tipc_sk_mcast_rcv - Deliver multicast messages to all destination sockets
 * @arrvq: queue with arriving messages, to be cloned after destination lookup
 * @inputq: queue with cloned messages, delivered to socket after dest lookup
 *
 * Multi-threaded: parallel calls with reference to same queues may occur
 */
void tipc_sk_mcast_rcv(struct net *net, struct sk_buff_head *arrvq,
		       struct sk_buff_head *inputq)
{
	struct tipc_msg *msg;
	struct tipc_plist dports;
	u32 portid;
	u32 scope = TIPC_CLUSTER_SCOPE;
	struct sk_buff_head tmpq;
	uint hsz;
	struct sk_buff *skb, *_skb;

	__skb_queue_head_init(&tmpq);
	tipc_plist_init(&dports);

	skb = tipc_skb_peek(arrvq, &inputq->lock);
	for (; skb; skb = tipc_skb_peek(arrvq, &inputq->lock)) {
		msg = buf_msg(skb);
		hsz = skb_headroom(skb) + msg_hdr_sz(msg);

		if (in_own_node(net, msg_orignode(msg)))
			scope = TIPC_NODE_SCOPE;

		/* Create destination port list and message clones: */
		tipc_nametbl_mc_translate(net,
					  msg_nametype(msg), msg_namelower(msg),
					  msg_nameupper(msg), scope, &dports);
		portid = tipc_plist_pop(&dports);
		for (; portid; portid = tipc_plist_pop(&dports)) {
			_skb = __pskb_copy(skb, hsz, GFP_ATOMIC);
			if (_skb) {
				msg_set_destport(buf_msg(_skb), portid);
				__skb_queue_tail(&tmpq, _skb);
				continue;
			}
			pr_warn("Failed to clone mcast rcv buffer\n");
		}
		/* Append to inputq if not already done by other thread */
		spin_lock_bh(&inputq->lock);
		if (skb_peek(arrvq) == skb) {
			skb_queue_splice_tail_init(&tmpq, inputq);
			kfree_skb(__skb_dequeue(arrvq));
		}
		spin_unlock_bh(&inputq->lock);
		__skb_queue_purge(&tmpq);
		kfree_skb(skb);
	}
	tipc_sk_rcv(net, inputq);
>>>>>>> v4.9.227
}

/**
 * tipc_sk_proto_rcv - receive a connection mng protocol message
 * @tsk: receiving socket
<<<<<<< HEAD
 * @dnode: node to send response message to, if any
 * @buf: buffer containing protocol message
 * Returns 0 (TIPC_OK) if message was consumed, 1 (TIPC_FWD_MSG) if
 * (CONN_PROBE_REPLY) message should be forwarded.
 */
static int tipc_sk_proto_rcv(struct tipc_sock *tsk, u32 *dnode,
			     struct sk_buff *buf)
{
	struct tipc_msg *msg = buf_msg(buf);
	int conn_cong;

	/* Ignore if connection cannot be validated: */
	if (!tsk_peer_msg(tsk, msg))
=======
 * @skb: pointer to message buffer.
 */
static void tipc_sk_proto_rcv(struct tipc_sock *tsk, struct sk_buff *skb,
			      struct sk_buff_head *xmitq)
{
	struct sock *sk = &tsk->sk;
	u32 onode = tsk_own_node(tsk);
	struct tipc_msg *hdr = buf_msg(skb);
	int mtyp = msg_type(hdr);
	bool conn_cong;

	/* Ignore if connection cannot be validated: */
	if (!tsk_peer_msg(tsk, hdr))
>>>>>>> v4.9.227
		goto exit;

	tsk->probing_state = TIPC_CONN_OK;

<<<<<<< HEAD
	if (msg_type(msg) == CONN_ACK) {
		conn_cong = tsk_conn_cong(tsk);
		tsk->sent_unacked -= msg_msgcnt(msg);
		if (conn_cong)
			tsk->sk.sk_write_space(&tsk->sk);
	} else if (msg_type(msg) == CONN_PROBE) {
		if (!tipc_msg_reverse(buf, dnode, TIPC_OK))
			return TIPC_OK;
		msg_set_type(msg, CONN_PROBE_REPLY);
		return TIPC_FWD_MSG;
	}
	/* Do nothing if msg_type() == CONN_PROBE_REPLY */
exit:
	kfree_skb(buf);
	return TIPC_OK;
}

/**
 * dest_name_check - verify user is permitted to send to specified port name
 * @dest: destination address
 * @m: descriptor for message to be sent
 *
 * Prevents restricted configuration commands from being issued by
 * unauthorized users.
 *
 * Returns 0 if permission is granted, otherwise errno
 */
static int dest_name_check(struct sockaddr_tipc *dest, struct msghdr *m)
{
	struct tipc_cfg_msg_hdr hdr;

	if (unlikely(dest->addrtype == TIPC_ADDR_ID))
		return 0;
	if (likely(dest->addr.name.name.type >= TIPC_RESERVED_TYPES))
		return 0;
	if (likely(dest->addr.name.name.type == TIPC_TOP_SRV))
		return 0;
	if (likely(dest->addr.name.name.type != TIPC_CFG_SRV))
		return -EACCES;

	if (!m->msg_iovlen || (m->msg_iov[0].iov_len < sizeof(hdr)))
		return -EMSGSIZE;
	if (copy_from_user(&hdr, m->msg_iov[0].iov_base, sizeof(hdr)))
		return -EFAULT;
	if ((ntohs(hdr.tcm_type) & 0xC000) && (!capable(CAP_NET_ADMIN)))
		return -EACCES;

	return 0;
=======
	if (mtyp == CONN_PROBE) {
		msg_set_type(hdr, CONN_PROBE_REPLY);
		if (tipc_msg_reverse(onode, &skb, TIPC_OK))
			__skb_queue_tail(xmitq, skb);
		return;
	} else if (mtyp == CONN_ACK) {
		conn_cong = tsk_conn_cong(tsk);
		tsk->snt_unacked -= msg_conn_ack(hdr);
		if (tsk->peer_caps & TIPC_BLOCK_FLOWCTL)
			tsk->snd_win = msg_adv_win(hdr);
		if (conn_cong)
			sk->sk_write_space(sk);
	} else if (mtyp != CONN_PROBE_REPLY) {
		pr_warn("Received unknown CONN_PROTO msg\n");
	}
exit:
	kfree_skb(skb);
>>>>>>> v4.9.227
}

static int tipc_wait_for_sndmsg(struct socket *sock, long *timeo_p)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	DEFINE_WAIT(wait);
	int done;

	do {
		int err = sock_error(sk);
		if (err)
			return err;
		if (sock->state == SS_DISCONNECTING)
			return -EPIPE;
		if (!*timeo_p)
			return -EAGAIN;
		if (signal_pending(current))
			return sock_intr_errno(*timeo_p);

		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
		done = sk_wait_event(sk, timeo_p, !tsk->link_cong);
		finish_wait(sk_sleep(sk), &wait);
	} while (!done);
	return 0;
}

/**
 * tipc_sendmsg - send message in connectionless manner
<<<<<<< HEAD
 * @iocb: if NULL, indicates that socket lock is already held
=======
>>>>>>> v4.9.227
 * @sock: socket structure
 * @m: message to send
 * @dsz: amount of user data to be sent
 *
 * Message must have an destination specified explicitly.
 * Used for SOCK_RDM and SOCK_DGRAM messages,
 * and for 'SYN' messages on SOCK_SEQPACKET and SOCK_STREAM connections.
 * (Note: 'SYN+' is prohibited on SOCK_STREAM.)
 *
 * Returns the number of bytes sent on success, or errno otherwise
 */
<<<<<<< HEAD
static int tipc_sendmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *m, size_t dsz)
{
	DECLARE_SOCKADDR(struct sockaddr_tipc *, dest, m->msg_name);
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_msg *mhdr = &tsk->phdr;
	struct iovec *iov = m->msg_iov;
	u32 dnode, dport;
	struct sk_buff *buf;
	struct tipc_name_seq *seq = &dest->addr.nameseq;
	u32 mtu;
	long timeo;
	int rc = -EINVAL;

	if (unlikely(!dest))
		return -EDESTADDRREQ;

	if (unlikely((m->msg_namelen < sizeof(*dest)) ||
		     (dest->family != AF_TIPC)))
		return -EINVAL;

	if (dsz > TIPC_MAX_USER_MSG_SIZE)
		return -EMSGSIZE;

	if (iocb)
		lock_sock(sk);

	if (unlikely(sock->state != SS_READY)) {
		if (sock->state == SS_LISTENING) {
			rc = -EPIPE;
			goto exit;
		}
		if (sock->state != SS_UNCONNECTED) {
			rc = -EISCONN;
			goto exit;
		}
		if (tsk->published) {
			rc = -EOPNOTSUPP;
			goto exit;
		}
=======
static int tipc_sendmsg(struct socket *sock,
			struct msghdr *m, size_t dsz)
{
	struct sock *sk = sock->sk;
	int ret;

	lock_sock(sk);
	ret = __tipc_sendmsg(sock, m, dsz);
	release_sock(sk);

	return ret;
}

static int __tipc_sendmsg(struct socket *sock, struct msghdr *m, size_t dsz)
{
	DECLARE_SOCKADDR(struct sockaddr_tipc *, dest, m->msg_name);
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct net *net = sock_net(sk);
	struct tipc_msg *mhdr = &tsk->phdr;
	u32 dnode, dport;
	struct sk_buff_head pktchain;
	struct sk_buff *skb;
	struct tipc_name_seq *seq;
	struct iov_iter save;
	u32 mtu;
	long timeo;
	int rc;

	if (dsz > TIPC_MAX_USER_MSG_SIZE)
		return -EMSGSIZE;
	if (unlikely(!dest)) {
		if (tsk->connected && sock->state == SS_READY)
			dest = &tsk->remote;
		else
			return -EDESTADDRREQ;
	} else if (unlikely(m->msg_namelen < sizeof(*dest)) ||
		   dest->family != AF_TIPC) {
		return -EINVAL;
	}
	if (unlikely(sock->state != SS_READY)) {
		if (sock->state == SS_LISTENING)
			return -EPIPE;
		if (sock->state != SS_UNCONNECTED)
			return -EISCONN;
		if (tsk->published)
			return -EOPNOTSUPP;
>>>>>>> v4.9.227
		if (dest->addrtype == TIPC_ADDR_NAME) {
			tsk->conn_type = dest->addr.name.name.type;
			tsk->conn_instance = dest->addr.name.name.instance;
		}
	}
<<<<<<< HEAD
	rc = dest_name_check(dest, m);
	if (rc)
		goto exit;

	timeo = sock_sndtimeo(sk, m->msg_flags & MSG_DONTWAIT);

	if (dest->addrtype == TIPC_ADDR_MCAST) {
		rc = tipc_sendmcast(sock, seq, iov, dsz, timeo);
		goto exit;
=======
	seq = &dest->addr.nameseq;
	timeo = sock_sndtimeo(sk, m->msg_flags & MSG_DONTWAIT);

	if (dest->addrtype == TIPC_ADDR_MCAST) {
		return tipc_sendmcast(sock, seq, m, dsz, timeo);
>>>>>>> v4.9.227
	} else if (dest->addrtype == TIPC_ADDR_NAME) {
		u32 type = dest->addr.name.name.type;
		u32 inst = dest->addr.name.name.instance;
		u32 domain = dest->addr.name.domain;

		dnode = domain;
		msg_set_type(mhdr, TIPC_NAMED_MSG);
		msg_set_hdr_sz(mhdr, NAMED_H_SIZE);
		msg_set_nametype(mhdr, type);
		msg_set_nameinst(mhdr, inst);
		msg_set_lookup_scope(mhdr, tipc_addr_scope(domain));
<<<<<<< HEAD
		dport = tipc_nametbl_translate(type, inst, &dnode);
		msg_set_destnode(mhdr, dnode);
		msg_set_destport(mhdr, dport);
		if (unlikely(!dport && !dnode)) {
			rc = -EHOSTUNREACH;
			goto exit;
		}
=======
		dport = tipc_nametbl_translate(net, type, inst, &dnode);
		msg_set_destnode(mhdr, dnode);
		msg_set_destport(mhdr, dport);
		if (unlikely(!dport && !dnode))
			return -EHOSTUNREACH;
>>>>>>> v4.9.227
	} else if (dest->addrtype == TIPC_ADDR_ID) {
		dnode = dest->addr.id.node;
		msg_set_type(mhdr, TIPC_DIRECT_MSG);
		msg_set_lookup_scope(mhdr, 0);
		msg_set_destnode(mhdr, dnode);
		msg_set_destport(mhdr, dest->addr.id.ref);
		msg_set_hdr_sz(mhdr, BASIC_H_SIZE);
	}

<<<<<<< HEAD
new_mtu:
	mtu = tipc_node_get_mtu(dnode, tsk->ref);
	rc = tipc_msg_build(mhdr, iov, 0, dsz, mtu, &buf);
	if (rc < 0)
		goto exit;

	do {
		TIPC_SKB_CB(buf)->wakeup_pending = tsk->link_cong;
		rc = tipc_link_xmit(buf, dnode, tsk->ref);
		if (likely(rc >= 0)) {
			if (sock->state != SS_READY)
				sock->state = SS_CONNECTING;
			rc = dsz;
			break;
		}
		if (rc == -EMSGSIZE)
			goto new_mtu;
		if (rc != -ELINKCONG)
			break;
		tsk->link_cong = 1;
		rc = tipc_wait_for_sndmsg(sock, &timeo);
		if (rc)
			kfree_skb_list(buf);
	} while (!rc);
exit:
	if (iocb)
		release_sock(sk);
=======
	skb_queue_head_init(&pktchain);
	save = m->msg_iter;
new_mtu:
	mtu = tipc_node_get_mtu(net, dnode, tsk->portid);
	rc = tipc_msg_build(mhdr, m, 0, dsz, mtu, &pktchain);
	if (rc < 0)
		return rc;

	do {
		skb = skb_peek(&pktchain);
		TIPC_SKB_CB(skb)->wakeup_pending = tsk->link_cong;
		rc = tipc_node_xmit(net, &pktchain, dnode, tsk->portid);
		if (likely(!rc)) {
			if (sock->state != SS_READY)
				sock->state = SS_CONNECTING;
			return dsz;
		}
		if (rc == -ELINKCONG) {
			tsk->link_cong = 1;
			rc = tipc_wait_for_sndmsg(sock, &timeo);
			if (!rc)
				continue;
		}
		__skb_queue_purge(&pktchain);
		if (rc == -EMSGSIZE) {
			m->msg_iter = save;
			goto new_mtu;
		}
		break;
	} while (1);
>>>>>>> v4.9.227

	return rc;
}

static int tipc_wait_for_sndpkt(struct socket *sock, long *timeo_p)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	DEFINE_WAIT(wait);
	int done;

	do {
		int err = sock_error(sk);
		if (err)
			return err;
		if (sock->state == SS_DISCONNECTING)
			return -EPIPE;
		else if (sock->state != SS_CONNECTED)
			return -ENOTCONN;
		if (!*timeo_p)
			return -EAGAIN;
		if (signal_pending(current))
			return sock_intr_errno(*timeo_p);

		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
		done = sk_wait_event(sk, timeo_p,
				     (!tsk->link_cong &&
				      !tsk_conn_cong(tsk)) ||
				     !tsk->connected);
		finish_wait(sk_sleep(sk), &wait);
	} while (!done);
	return 0;
}

/**
 * tipc_send_stream - send stream-oriented data
<<<<<<< HEAD
 * @iocb: (unused)
=======
>>>>>>> v4.9.227
 * @sock: socket structure
 * @m: data to send
 * @dsz: total length of data to be transmitted
 *
 * Used for SOCK_STREAM data.
 *
 * Returns the number of bytes sent on success (or partial success),
 * or errno if no data sent
 */
<<<<<<< HEAD
static int tipc_send_stream(struct kiocb *iocb, struct socket *sock,
			    struct msghdr *m, size_t dsz)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_msg *mhdr = &tsk->phdr;
	struct sk_buff *buf;
	DECLARE_SOCKADDR(struct sockaddr_tipc *, dest, m->msg_name);
	u32 ref = tsk->ref;
=======
static int tipc_send_stream(struct socket *sock, struct msghdr *m, size_t dsz)
{
	struct sock *sk = sock->sk;
	int ret;

	lock_sock(sk);
	ret = __tipc_send_stream(sock, m, dsz);
	release_sock(sk);

	return ret;
}

static int __tipc_send_stream(struct socket *sock, struct msghdr *m, size_t dsz)
{
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_msg *mhdr = &tsk->phdr;
	struct sk_buff_head pktchain;
	DECLARE_SOCKADDR(struct sockaddr_tipc *, dest, m->msg_name);
	u32 portid = tsk->portid;
>>>>>>> v4.9.227
	int rc = -EINVAL;
	long timeo;
	u32 dnode;
	uint mtu, send, sent = 0;
<<<<<<< HEAD

	/* Handle implied connection establishment */
	if (unlikely(dest)) {
		rc = tipc_sendmsg(iocb, sock, m, dsz);
		if (dsz && (dsz == rc))
			tsk->sent_unacked = 1;
=======
	struct iov_iter save;
	int hlen = MIN_H_SIZE;

	/* Handle implied connection establishment */
	if (unlikely(dest)) {
		rc = __tipc_sendmsg(sock, m, dsz);
		hlen = msg_hdr_sz(mhdr);
		if (dsz && (dsz == rc))
			tsk->snt_unacked = tsk_inc(tsk, dsz + hlen);
>>>>>>> v4.9.227
		return rc;
	}
	if (dsz > (uint)INT_MAX)
		return -EMSGSIZE;

<<<<<<< HEAD
	if (iocb)
		lock_sock(sk);

	if (unlikely(sock->state != SS_CONNECTED)) {
		if (sock->state == SS_DISCONNECTING)
			rc = -EPIPE;
		else
			rc = -ENOTCONN;
		goto exit;
=======
	if (unlikely(sock->state != SS_CONNECTED)) {
		if (sock->state == SS_DISCONNECTING)
			return -EPIPE;
		else
			return -ENOTCONN;
>>>>>>> v4.9.227
	}

	timeo = sock_sndtimeo(sk, m->msg_flags & MSG_DONTWAIT);
	dnode = tsk_peer_node(tsk);
<<<<<<< HEAD

next:
	mtu = tsk->max_pkt;
	send = min_t(uint, dsz - sent, TIPC_MAX_USER_MSG_SIZE);
	rc = tipc_msg_build(mhdr, m->msg_iov, sent, send, mtu, &buf);
	if (unlikely(rc < 0))
		goto exit;
	do {
		if (likely(!tsk_conn_cong(tsk))) {
			rc = tipc_link_xmit(buf, dnode, ref);
			if (likely(!rc)) {
				tsk->sent_unacked++;
				sent += send;
				if (sent == dsz)
					break;
				goto next;
			}
			if (rc == -EMSGSIZE) {
				tsk->max_pkt = tipc_node_get_mtu(dnode, ref);
=======
	skb_queue_head_init(&pktchain);

next:
	save = m->msg_iter;
	mtu = tsk->max_pkt;
	send = min_t(uint, dsz - sent, TIPC_MAX_USER_MSG_SIZE);
	rc = tipc_msg_build(mhdr, m, sent, send, mtu, &pktchain);
	if (unlikely(rc < 0))
		return rc;

	do {
		if (likely(!tsk_conn_cong(tsk))) {
			rc = tipc_node_xmit(net, &pktchain, dnode, portid);
			if (likely(!rc)) {
				tsk->snt_unacked += tsk_inc(tsk, send + hlen);
				sent += send;
				if (sent == dsz)
					return dsz;
				goto next;
			}
			if (rc == -EMSGSIZE) {
				__skb_queue_purge(&pktchain);
				tsk->max_pkt = tipc_node_get_mtu(net, dnode,
								 portid);
				m->msg_iter = save;
>>>>>>> v4.9.227
				goto next;
			}
			if (rc != -ELINKCONG)
				break;
<<<<<<< HEAD
			tsk->link_cong = 1;
		}
		rc = tipc_wait_for_sndpkt(sock, &timeo);
		if (rc)
			kfree_skb_list(buf);
	} while (!rc);
exit:
	if (iocb)
		release_sock(sk);
=======

			tsk->link_cong = 1;
		}
		rc = tipc_wait_for_sndpkt(sock, &timeo);
	} while (!rc);

	__skb_queue_purge(&pktchain);
>>>>>>> v4.9.227
	return sent ? sent : rc;
}

/**
 * tipc_send_packet - send a connection-oriented message
<<<<<<< HEAD
 * @iocb: if NULL, indicates that socket lock is already held
=======
>>>>>>> v4.9.227
 * @sock: socket structure
 * @m: message to send
 * @dsz: length of data to be transmitted
 *
 * Used for SOCK_SEQPACKET messages.
 *
 * Returns the number of bytes sent on success, or errno otherwise
 */
<<<<<<< HEAD
static int tipc_send_packet(struct kiocb *iocb, struct socket *sock,
			    struct msghdr *m, size_t dsz)
=======
static int tipc_send_packet(struct socket *sock, struct msghdr *m, size_t dsz)
>>>>>>> v4.9.227
{
	if (dsz > TIPC_MAX_USER_MSG_SIZE)
		return -EMSGSIZE;

<<<<<<< HEAD
	return tipc_send_stream(iocb, sock, m, dsz);
=======
	return tipc_send_stream(sock, m, dsz);
>>>>>>> v4.9.227
}

/* tipc_sk_finish_conn - complete the setup of a connection
 */
static void tipc_sk_finish_conn(struct tipc_sock *tsk, u32 peer_port,
				u32 peer_node)
{
<<<<<<< HEAD
=======
	struct sock *sk = &tsk->sk;
	struct net *net = sock_net(sk);
>>>>>>> v4.9.227
	struct tipc_msg *msg = &tsk->phdr;

	msg_set_destnode(msg, peer_node);
	msg_set_destport(msg, peer_port);
	msg_set_type(msg, TIPC_CONN_MSG);
	msg_set_lookup_scope(msg, 0);
	msg_set_hdr_sz(msg, SHORT_H_SIZE);

<<<<<<< HEAD
	tsk->probing_interval = CONN_PROBING_INTERVAL;
	tsk->probing_state = TIPC_CONN_OK;
	tsk->connected = 1;
	k_start_timer(&tsk->timer, tsk->probing_interval);
	tipc_node_add_conn(peer_node, tsk->ref, peer_port);
	tsk->max_pkt = tipc_node_get_mtu(peer_node, tsk->ref);
=======
	tsk->probing_intv = CONN_PROBING_INTERVAL;
	tsk->probing_state = TIPC_CONN_OK;
	tsk->connected = 1;
	sk_reset_timer(sk, &sk->sk_timer, jiffies + tsk->probing_intv);
	tipc_node_add_conn(net, peer_node, tsk->portid, peer_port);
	tsk->max_pkt = tipc_node_get_mtu(net, peer_node, tsk->portid);
	tsk->peer_caps = tipc_node_get_capabilities(net, peer_node);
	if (tsk->peer_caps & TIPC_BLOCK_FLOWCTL)
		return;

	/* Fall back to message based flow control */
	tsk->rcv_win = FLOWCTL_MSG_WIN;
	tsk->snd_win = FLOWCTL_MSG_WIN;
>>>>>>> v4.9.227
}

/**
 * set_orig_addr - capture sender's address for received message
 * @m: descriptor for message info
 * @msg: received message header
 *
 * Note: Address is not captured if not requested by receiver.
 */
static void set_orig_addr(struct msghdr *m, struct tipc_msg *msg)
{
	DECLARE_SOCKADDR(struct sockaddr_tipc *, addr, m->msg_name);

	if (addr) {
		addr->family = AF_TIPC;
		addr->addrtype = TIPC_ADDR_ID;
		memset(&addr->addr, 0, sizeof(addr->addr));
		addr->addr.id.ref = msg_origport(msg);
		addr->addr.id.node = msg_orignode(msg);
		addr->addr.name.domain = 0;	/* could leave uninitialized */
		addr->scope = 0;		/* could leave uninitialized */
		m->msg_namelen = sizeof(struct sockaddr_tipc);
	}
}

/**
 * tipc_sk_anc_data_recv - optionally capture ancillary data for received message
 * @m: descriptor for message info
 * @msg: received message header
 * @tsk: TIPC port associated with message
 *
 * Note: Ancillary data is not captured if not requested by receiver.
 *
 * Returns 0 if successful, otherwise errno
 */
static int tipc_sk_anc_data_recv(struct msghdr *m, struct tipc_msg *msg,
				 struct tipc_sock *tsk)
{
	u32 anc_data[3];
	u32 err;
	u32 dest_type;
	int has_name;
	int res;

	if (likely(m->msg_controllen == 0))
		return 0;

	/* Optionally capture errored message object(s) */
	err = msg ? msg_errcode(msg) : 0;
	if (unlikely(err)) {
		anc_data[0] = err;
		anc_data[1] = msg_data_sz(msg);
		res = put_cmsg(m, SOL_TIPC, TIPC_ERRINFO, 8, anc_data);
		if (res)
			return res;
		if (anc_data[1]) {
			res = put_cmsg(m, SOL_TIPC, TIPC_RETDATA, anc_data[1],
				       msg_data(msg));
			if (res)
				return res;
		}
	}

	/* Optionally capture message destination object */
	dest_type = msg ? msg_type(msg) : TIPC_DIRECT_MSG;
	switch (dest_type) {
	case TIPC_NAMED_MSG:
		has_name = 1;
		anc_data[0] = msg_nametype(msg);
		anc_data[1] = msg_namelower(msg);
		anc_data[2] = msg_namelower(msg);
		break;
	case TIPC_MCAST_MSG:
		has_name = 1;
		anc_data[0] = msg_nametype(msg);
		anc_data[1] = msg_namelower(msg);
		anc_data[2] = msg_nameupper(msg);
		break;
	case TIPC_CONN_MSG:
		has_name = (tsk->conn_type != 0);
		anc_data[0] = tsk->conn_type;
		anc_data[1] = tsk->conn_instance;
		anc_data[2] = tsk->conn_instance;
		break;
	default:
		has_name = 0;
	}
	if (has_name) {
		res = put_cmsg(m, SOL_TIPC, TIPC_DESTNAME, 12, anc_data);
		if (res)
			return res;
	}

	return 0;
}

<<<<<<< HEAD
static void tipc_sk_send_ack(struct tipc_sock *tsk, uint ack)
{
	struct sk_buff *buf = NULL;
=======
static void tipc_sk_send_ack(struct tipc_sock *tsk)
{
	struct net *net = sock_net(&tsk->sk);
	struct sk_buff *skb = NULL;
>>>>>>> v4.9.227
	struct tipc_msg *msg;
	u32 peer_port = tsk_peer_port(tsk);
	u32 dnode = tsk_peer_node(tsk);

	if (!tsk->connected)
		return;
<<<<<<< HEAD
	buf = tipc_msg_create(CONN_MANAGER, CONN_ACK, INT_H_SIZE, 0, dnode,
			      tipc_own_addr, peer_port, tsk->ref, TIPC_OK);
	if (!buf)
		return;
	msg = buf_msg(buf);
	msg_set_msgcnt(msg, ack);
	tipc_link_xmit(buf, dnode, msg_link_selector(msg));
=======
	skb = tipc_msg_create(CONN_MANAGER, CONN_ACK, INT_H_SIZE, 0,
			      dnode, tsk_own_node(tsk), peer_port,
			      tsk->portid, TIPC_OK);
	if (!skb)
		return;
	msg = buf_msg(skb);
	msg_set_conn_ack(msg, tsk->rcv_unacked);
	tsk->rcv_unacked = 0;

	/* Adjust to and advertize the correct window limit */
	if (tsk->peer_caps & TIPC_BLOCK_FLOWCTL) {
		tsk->rcv_win = tsk_adv_blocks(tsk->sk.sk_rcvbuf);
		msg_set_adv_win(msg, tsk->rcv_win);
	}
	tipc_node_xmit_skb(net, skb, dnode, msg_link_selector(msg));
>>>>>>> v4.9.227
}

static int tipc_wait_for_rcvmsg(struct socket *sock, long *timeop)
{
	struct sock *sk = sock->sk;
	DEFINE_WAIT(wait);
	long timeo = *timeop;
	int err;

	for (;;) {
		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
		if (timeo && skb_queue_empty(&sk->sk_receive_queue)) {
			if (sock->state == SS_DISCONNECTING) {
				err = -ENOTCONN;
				break;
			}
			release_sock(sk);
			timeo = schedule_timeout(timeo);
			lock_sock(sk);
		}
		err = 0;
		if (!skb_queue_empty(&sk->sk_receive_queue))
			break;
<<<<<<< HEAD
		err = sock_intr_errno(timeo);
		if (signal_pending(current))
			break;
		err = -EAGAIN;
		if (!timeo)
			break;
=======
		err = -EAGAIN;
		if (!timeo)
			break;
		err = sock_intr_errno(timeo);
		if (signal_pending(current))
			break;
>>>>>>> v4.9.227
	}
	finish_wait(sk_sleep(sk), &wait);
	*timeop = timeo;
	return err;
}

/**
 * tipc_recvmsg - receive packet-oriented message
<<<<<<< HEAD
 * @iocb: (unused)
=======
>>>>>>> v4.9.227
 * @m: descriptor for message info
 * @buf_len: total size of user buffer area
 * @flags: receive flags
 *
 * Used for SOCK_DGRAM, SOCK_RDM, and SOCK_SEQPACKET messages.
 * If the complete message doesn't fit in user area, truncate it.
 *
 * Returns size of returned message data, errno otherwise
 */
<<<<<<< HEAD
static int tipc_recvmsg(struct kiocb *iocb, struct socket *sock,
			struct msghdr *m, size_t buf_len, int flags)
=======
static int tipc_recvmsg(struct socket *sock, struct msghdr *m, size_t buf_len,
			int flags)
>>>>>>> v4.9.227
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct sk_buff *buf;
	struct tipc_msg *msg;
	long timeo;
	unsigned int sz;
	u32 err;
<<<<<<< HEAD
	int res;
=======
	int res, hlen;
>>>>>>> v4.9.227

	/* Catch invalid receive requests */
	if (unlikely(!buf_len))
		return -EINVAL;

	lock_sock(sk);

	if (unlikely(sock->state == SS_UNCONNECTED)) {
		res = -ENOTCONN;
		goto exit;
	}

	timeo = sock_rcvtimeo(sk, flags & MSG_DONTWAIT);
restart:

	/* Look for a message in receive queue; wait if necessary */
	res = tipc_wait_for_rcvmsg(sock, &timeo);
	if (res)
		goto exit;

	/* Look at first message in receive queue */
	buf = skb_peek(&sk->sk_receive_queue);
	msg = buf_msg(buf);
	sz = msg_data_sz(msg);
<<<<<<< HEAD
=======
	hlen = msg_hdr_sz(msg);
>>>>>>> v4.9.227
	err = msg_errcode(msg);

	/* Discard an empty non-errored message & try again */
	if ((!sz) && (!err)) {
		tsk_advance_rx_queue(sk);
		goto restart;
	}

	/* Capture sender's address (optional) */
	set_orig_addr(m, msg);

	/* Capture ancillary data (optional) */
	res = tipc_sk_anc_data_recv(m, msg, tsk);
	if (res)
		goto exit;

	/* Capture message data (if valid) & compute return value (always) */
	if (!err) {
		if (unlikely(buf_len < sz)) {
			sz = buf_len;
			m->msg_flags |= MSG_TRUNC;
		}
<<<<<<< HEAD
		res = skb_copy_datagram_iovec(buf, msg_hdr_sz(msg),
					      m->msg_iov, sz);
=======
		res = skb_copy_datagram_msg(buf, hlen, m, sz);
>>>>>>> v4.9.227
		if (res)
			goto exit;
		res = sz;
	} else {
		if ((sock->state == SS_READY) ||
		    ((err == TIPC_CONN_SHUTDOWN) || m->msg_control))
			res = 0;
		else
			res = -ECONNRESET;
	}

<<<<<<< HEAD
	/* Consume received message (optional) */
	if (likely(!(flags & MSG_PEEK))) {
		if ((sock->state != SS_READY) &&
		    (++tsk->rcv_unacked >= TIPC_CONNACK_INTV)) {
			tipc_sk_send_ack(tsk, tsk->rcv_unacked);
			tsk->rcv_unacked = 0;
		}
		tsk_advance_rx_queue(sk);
	}
=======
	if (unlikely(flags & MSG_PEEK))
		goto exit;

	if (likely(sock->state != SS_READY)) {
		tsk->rcv_unacked += tsk_inc(tsk, hlen + sz);
		if (unlikely(tsk->rcv_unacked >= (tsk->rcv_win / 4)))
			tipc_sk_send_ack(tsk);
	}
	tsk_advance_rx_queue(sk);
>>>>>>> v4.9.227
exit:
	release_sock(sk);
	return res;
}

/**
 * tipc_recv_stream - receive stream-oriented data
<<<<<<< HEAD
 * @iocb: (unused)
=======
>>>>>>> v4.9.227
 * @m: descriptor for message info
 * @buf_len: total size of user buffer area
 * @flags: receive flags
 *
 * Used for SOCK_STREAM messages only.  If not enough data is available
 * will optionally wait for more; never truncates data.
 *
 * Returns size of returned message data, errno otherwise
 */
<<<<<<< HEAD
static int tipc_recv_stream(struct kiocb *iocb, struct socket *sock,
			    struct msghdr *m, size_t buf_len, int flags)
=======
static int tipc_recv_stream(struct socket *sock, struct msghdr *m,
			    size_t buf_len, int flags)
>>>>>>> v4.9.227
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct sk_buff *buf;
	struct tipc_msg *msg;
	long timeo;
	unsigned int sz;
	int sz_to_copy, target, needed;
	int sz_copied = 0;
	u32 err;
<<<<<<< HEAD
	int res = 0;
=======
	int res = 0, hlen;
>>>>>>> v4.9.227

	/* Catch invalid receive attempts */
	if (unlikely(!buf_len))
		return -EINVAL;

	lock_sock(sk);

	if (unlikely(sock->state == SS_UNCONNECTED)) {
		res = -ENOTCONN;
		goto exit;
	}

	target = sock_rcvlowat(sk, flags & MSG_WAITALL, buf_len);
	timeo = sock_rcvtimeo(sk, flags & MSG_DONTWAIT);

restart:
	/* Look for a message in receive queue; wait if necessary */
	res = tipc_wait_for_rcvmsg(sock, &timeo);
	if (res)
		goto exit;

	/* Look at first message in receive queue */
	buf = skb_peek(&sk->sk_receive_queue);
	msg = buf_msg(buf);
	sz = msg_data_sz(msg);
<<<<<<< HEAD
=======
	hlen = msg_hdr_sz(msg);
>>>>>>> v4.9.227
	err = msg_errcode(msg);

	/* Discard an empty non-errored message & try again */
	if ((!sz) && (!err)) {
		tsk_advance_rx_queue(sk);
		goto restart;
	}

	/* Optionally capture sender's address & ancillary data of first msg */
	if (sz_copied == 0) {
		set_orig_addr(m, msg);
		res = tipc_sk_anc_data_recv(m, msg, tsk);
		if (res)
			goto exit;
	}

	/* Capture message data (if valid) & compute return value (always) */
	if (!err) {
		u32 offset = (u32)(unsigned long)(TIPC_SKB_CB(buf)->handle);

		sz -= offset;
		needed = (buf_len - sz_copied);
		sz_to_copy = (sz <= needed) ? sz : needed;

<<<<<<< HEAD
		res = skb_copy_datagram_iovec(buf, msg_hdr_sz(msg) + offset,
					      m->msg_iov, sz_to_copy);
=======
		res = skb_copy_datagram_msg(buf, hlen + offset, m, sz_to_copy);
>>>>>>> v4.9.227
		if (res)
			goto exit;

		sz_copied += sz_to_copy;

		if (sz_to_copy < sz) {
			if (!(flags & MSG_PEEK))
				TIPC_SKB_CB(buf)->handle =
				(void *)(unsigned long)(offset + sz_to_copy);
			goto exit;
		}
	} else {
		if (sz_copied != 0)
			goto exit; /* can't add error msg to valid data */

		if ((err == TIPC_CONN_SHUTDOWN) || m->msg_control)
			res = 0;
		else
			res = -ECONNRESET;
	}

<<<<<<< HEAD
	/* Consume received message (optional) */
	if (likely(!(flags & MSG_PEEK))) {
		if (unlikely(++tsk->rcv_unacked >= TIPC_CONNACK_INTV)) {
			tipc_sk_send_ack(tsk, tsk->rcv_unacked);
			tsk->rcv_unacked = 0;
		}
		tsk_advance_rx_queue(sk);
	}
=======
	if (unlikely(flags & MSG_PEEK))
		goto exit;

	tsk->rcv_unacked += tsk_inc(tsk, hlen + sz);
	if (unlikely(tsk->rcv_unacked >= (tsk->rcv_win / 4)))
		tipc_sk_send_ack(tsk);
	tsk_advance_rx_queue(sk);
>>>>>>> v4.9.227

	/* Loop around if more data is required */
	if ((sz_copied < buf_len) &&	/* didn't get all requested data */
	    (!skb_queue_empty(&sk->sk_receive_queue) ||
	    (sz_copied < target)) &&	/* and more is ready or required */
<<<<<<< HEAD
	    (!(flags & MSG_PEEK)) &&	/* and aren't just peeking at data */
=======
>>>>>>> v4.9.227
	    (!err))			/* and haven't reached a FIN */
		goto restart;

exit:
	release_sock(sk);
	return sz_copied ? sz_copied : res;
}

/**
 * tipc_write_space - wake up thread if port congestion is released
 * @sk: socket
 */
static void tipc_write_space(struct sock *sk)
{
	struct socket_wq *wq;

	rcu_read_lock();
	wq = rcu_dereference(sk->sk_wq);
<<<<<<< HEAD
	if (wq_has_sleeper(wq))
=======
	if (skwq_has_sleeper(wq))
>>>>>>> v4.9.227
		wake_up_interruptible_sync_poll(&wq->wait, POLLOUT |
						POLLWRNORM | POLLWRBAND);
	rcu_read_unlock();
}

/**
 * tipc_data_ready - wake up threads to indicate messages have been received
 * @sk: socket
 * @len: the length of messages
 */
static void tipc_data_ready(struct sock *sk)
{
	struct socket_wq *wq;

	rcu_read_lock();
	wq = rcu_dereference(sk->sk_wq);
<<<<<<< HEAD
	if (wq_has_sleeper(wq))
=======
	if (skwq_has_sleeper(wq))
>>>>>>> v4.9.227
		wake_up_interruptible_sync_poll(&wq->wait, POLLIN |
						POLLRDNORM | POLLRDBAND);
	rcu_read_unlock();
}

<<<<<<< HEAD
/**
 * filter_connect - Handle all incoming messages for a connection-based socket
 * @tsk: TIPC socket
 * @msg: message
 *
 * Returns 0 (TIPC_OK) if everyting ok, -TIPC_ERR_NO_PORT otherwise
 */
static int filter_connect(struct tipc_sock *tsk, struct sk_buff **buf)
{
	struct sock *sk = &tsk->sk;
	struct socket *sock = sk->sk_socket;
	struct tipc_msg *msg = buf_msg(*buf);
	int retval = -TIPC_ERR_NO_PORT;

	if (msg_mcast(msg))
		return retval;

	switch ((int)sock->state) {
	case SS_CONNECTED:
		/* Accept only connection-based messages sent by peer */
		if (tsk_peer_msg(tsk, msg)) {
			if (unlikely(msg_errcode(msg))) {
				sock->state = SS_DISCONNECTING;
				tsk->connected = 0;
				/* let timer expire on it's own */
				tipc_node_remove_conn(tsk_peer_node(tsk),
						      tsk->ref);
			}
			retval = TIPC_OK;
		}
		break;
	case SS_CONNECTING:
		/* Accept only ACK or NACK message */

		if (unlikely(!msg_connected(msg)))
			break;

		if (unlikely(msg_errcode(msg))) {
			sock->state = SS_DISCONNECTING;
			sk->sk_err = ECONNREFUSED;
			retval = TIPC_OK;
			break;
		}

		if (unlikely(msg_importance(msg) > TIPC_CRITICAL_IMPORTANCE)) {
			sock->state = SS_DISCONNECTING;
			sk->sk_err = EINVAL;
			retval = TIPC_OK;
			break;
		}

		tipc_sk_finish_conn(tsk, msg_origport(msg), msg_orignode(msg));
		msg_set_importance(&tsk->phdr, msg_importance(msg));
		sock->state = SS_CONNECTED;

		/* If an incoming message is an 'ACK-', it should be
		 * discarded here because it doesn't contain useful
		 * data. In addition, we should try to wake up
		 * connect() routine if sleeping.
		 */
		if (msg_data_sz(msg) == 0) {
			kfree_skb(*buf);
			*buf = NULL;
			if (waitqueue_active(sk_sleep(sk)))
				wake_up_interruptible(sk_sleep(sk));
		}
		retval = TIPC_OK;
		break;
	case SS_LISTENING:
	case SS_UNCONNECTED:
		/* Accept only SYN message */
		if (!msg_connected(msg) && !(msg_errcode(msg)))
			retval = TIPC_OK;
=======
static void tipc_sock_destruct(struct sock *sk)
{
	__skb_queue_purge(&sk->sk_receive_queue);
}

/**
 * filter_connect - Handle all incoming messages for a connection-based socket
 * @tsk: TIPC socket
 * @skb: pointer to message buffer. Set to NULL if buffer is consumed
 *
 * Returns true if everything ok, false otherwise
 */
static bool filter_connect(struct tipc_sock *tsk, struct sk_buff *skb)
{
	struct sock *sk = &tsk->sk;
	struct net *net = sock_net(sk);
	struct socket *sock = sk->sk_socket;
	struct tipc_msg *hdr = buf_msg(skb);

	if (unlikely(msg_mcast(hdr)))
		return false;

	switch ((int)sock->state) {
	case SS_CONNECTED:

		/* Accept only connection-based messages sent by peer */
		if (unlikely(!tsk_peer_msg(tsk, hdr)))
			return false;

		if (unlikely(msg_errcode(hdr))) {
			sock->state = SS_DISCONNECTING;
			tsk->connected = 0;
			/* Let timer expire on it's own */
			tipc_node_remove_conn(net, tsk_peer_node(tsk),
					      tsk->portid);
		}
		return true;

	case SS_CONNECTING:

		/* Accept only ACK or NACK message */
		if (unlikely(!msg_connected(hdr)))
			return false;

		if (unlikely(msg_errcode(hdr))) {
			sock->state = SS_DISCONNECTING;
			sk->sk_err = ECONNREFUSED;
			return true;
		}

		if (unlikely(!msg_isdata(hdr))) {
			sock->state = SS_DISCONNECTING;
			sk->sk_err = EINVAL;
			return true;
		}

		tipc_sk_finish_conn(tsk, msg_origport(hdr), msg_orignode(hdr));
		msg_set_importance(&tsk->phdr, msg_importance(hdr));
		sock->state = SS_CONNECTED;

		/* If 'ACK+' message, add to socket receive queue */
		if (msg_data_sz(hdr))
			return true;

		/* If empty 'ACK-' message, wake up sleeping connect() */
		if (waitqueue_active(sk_sleep(sk)))
			wake_up_interruptible(sk_sleep(sk));

		/* 'ACK-' message is neither accepted nor rejected: */
		msg_set_dest_droppable(hdr, 1);
		return false;

	case SS_LISTENING:
	case SS_UNCONNECTED:

		/* Accept only SYN message */
		if (!msg_connected(hdr) && !(msg_errcode(hdr)))
			return true;
>>>>>>> v4.9.227
		break;
	case SS_DISCONNECTING:
		break;
	default:
		pr_err("Unknown socket state %u\n", sock->state);
	}
<<<<<<< HEAD
	return retval;
=======
	return false;
>>>>>>> v4.9.227
}

/**
 * rcvbuf_limit - get proper overload limit of socket receive queue
 * @sk: socket
<<<<<<< HEAD
 * @buf: message
 *
 * For all connection oriented messages, irrespective of importance,
 * the default overload value (i.e. 67MB) is set as limit.
 *
 * For all connectionless messages, by default new queue limits are
 * as belows:
 *
 * TIPC_LOW_IMPORTANCE       (4 MB)
 * TIPC_MEDIUM_IMPORTANCE    (8 MB)
 * TIPC_HIGH_IMPORTANCE      (16 MB)
 * TIPC_CRITICAL_IMPORTANCE  (32 MB)
 *
 * Returns overload limit according to corresponding message importance
 */
static unsigned int rcvbuf_limit(struct sock *sk, struct sk_buff *buf)
{
	struct tipc_msg *msg = buf_msg(buf);

	if (msg_connected(msg))
		return sysctl_tipc_rmem[2];

	return sk->sk_rcvbuf >> TIPC_CRITICAL_IMPORTANCE <<
		msg_importance(msg);
=======
 * @skb: message
 *
 * For connection oriented messages, irrespective of importance,
 * default queue limit is 2 MB.
 *
 * For connectionless messages, queue limits are based on message
 * importance as follows:
 *
 * TIPC_LOW_IMPORTANCE       (2 MB)
 * TIPC_MEDIUM_IMPORTANCE    (4 MB)
 * TIPC_HIGH_IMPORTANCE      (8 MB)
 * TIPC_CRITICAL_IMPORTANCE  (16 MB)
 *
 * Returns overload limit according to corresponding message importance
 */
static unsigned int rcvbuf_limit(struct sock *sk, struct sk_buff *skb)
{
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_msg *hdr = buf_msg(skb);

	if (unlikely(!msg_connected(hdr)))
		return sk->sk_rcvbuf << msg_importance(hdr);

	if (likely(tsk->peer_caps & TIPC_BLOCK_FLOWCTL))
		return sk->sk_rcvbuf;

	return FLOWCTL_MSG_LIM;
>>>>>>> v4.9.227
}

/**
 * filter_rcv - validate incoming message
 * @sk: socket
<<<<<<< HEAD
 * @buf: message
=======
 * @skb: pointer to message.
>>>>>>> v4.9.227
 *
 * Enqueues message on receive queue if acceptable; optionally handles
 * disconnect indication for a connected socket.
 *
<<<<<<< HEAD
 * Called with socket lock already taken; port lock may also be taken.
 *
 * Returns 0 (TIPC_OK) if message was consumed, -TIPC error code if message
 * to be rejected, 1 (TIPC_FWD_MSG) if (CONN_MANAGER) message to be forwarded
 */
static int filter_rcv(struct sock *sk, struct sk_buff *buf)
{
	struct socket *sock = sk->sk_socket;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_msg *msg = buf_msg(buf);
	unsigned int limit = rcvbuf_limit(sk, buf);
	u32 onode;
	int rc = TIPC_OK;

	if (unlikely(msg_user(msg) == CONN_MANAGER))
		return tipc_sk_proto_rcv(tsk, &onode, buf);

	if (unlikely(msg_user(msg) == SOCK_WAKEUP)) {
		kfree_skb(buf);
		tsk->link_cong = 0;
		sk->sk_write_space(sk);
		return TIPC_OK;
	}

	/* Reject message if it is wrong sort of message for socket */
	if (msg_type(msg) > TIPC_DIRECT_MSG)
		return -TIPC_ERR_NO_PORT;

	if (sock->state == SS_READY) {
		if (msg_connected(msg))
			return -TIPC_ERR_NO_PORT;
	} else {
		rc = filter_connect(tsk, &buf);
		if (rc != TIPC_OK || buf == NULL)
			return rc;
	}

	/* Reject message if there isn't room to queue it */
	if (sk_rmem_alloc_get(sk) + buf->truesize >= limit)
		return -TIPC_ERR_OVERLOAD;

	/* Enqueue message */
	TIPC_SKB_CB(buf)->handle = NULL;
	__skb_queue_tail(&sk->sk_receive_queue, buf);
	skb_set_owner_r(buf, sk);

	sk->sk_data_ready(sk);
	return TIPC_OK;
=======
 * Called with socket lock already taken
 *
 * Returns true if message was added to socket receive queue, otherwise false
 */
static bool filter_rcv(struct sock *sk, struct sk_buff *skb,
		       struct sk_buff_head *xmitq)
{
	struct socket *sock = sk->sk_socket;
	struct tipc_sock *tsk = tipc_sk(sk);
	struct tipc_msg *hdr = buf_msg(skb);
	unsigned int limit = rcvbuf_limit(sk, skb);
	int err = TIPC_OK;
	int usr = msg_user(hdr);

	if (unlikely(msg_user(hdr) == CONN_MANAGER)) {
		tipc_sk_proto_rcv(tsk, skb, xmitq);
		return false;
	}

	if (unlikely(usr == SOCK_WAKEUP)) {
		kfree_skb(skb);
		tsk->link_cong = 0;
		sk->sk_write_space(sk);
		return false;
	}

	/* Drop if illegal message type */
	if (unlikely(msg_type(hdr) > TIPC_DIRECT_MSG)) {
		kfree_skb(skb);
		return false;
	}

	/* Reject if wrong message type for current socket state */
	if (unlikely(sock->state == SS_READY)) {
		if (msg_connected(hdr)) {
			err = TIPC_ERR_NO_PORT;
			goto reject;
		}
	} else if (unlikely(!filter_connect(tsk, skb))) {
		err = TIPC_ERR_NO_PORT;
		goto reject;
	}

	/* Reject message if there isn't room to queue it */
	if (unlikely(sk_rmem_alloc_get(sk) + skb->truesize >= limit)) {
		err = TIPC_ERR_OVERLOAD;
		goto reject;
	}

	/* Enqueue message */
	TIPC_SKB_CB(skb)->handle = NULL;
	__skb_queue_tail(&sk->sk_receive_queue, skb);
	skb_set_owner_r(skb, sk);

	sk->sk_data_ready(sk);
	return true;

reject:
	if (tipc_msg_reverse(tsk_own_node(tsk), &skb, err))
		__skb_queue_tail(xmitq, skb);
	return false;
>>>>>>> v4.9.227
}

/**
 * tipc_backlog_rcv - handle incoming message from backlog queue
 * @sk: socket
<<<<<<< HEAD
 * @buf: message
 *
 * Caller must hold socket lock, but not port lock.
 *
 * Returns 0
 */
static int tipc_backlog_rcv(struct sock *sk, struct sk_buff *buf)
{
	int rc;
	u32 onode;
	struct tipc_sock *tsk = tipc_sk(sk);
	uint truesize = buf->truesize;

	rc = filter_rcv(sk, buf);

	if (likely(!rc)) {
		if (atomic_read(&tsk->dupl_rcvcnt) < TIPC_CONN_OVERLOAD_LIMIT)
			atomic_add(truesize, &tsk->dupl_rcvcnt);
		return 0;
	}

	if ((rc < 0) && !tipc_msg_reverse(buf, &onode, -rc))
		return 0;

	tipc_link_xmit(buf, onode, 0);

=======
 * @skb: message
 *
 * Caller must hold socket lock
 *
 * Returns 0
 */
static int tipc_backlog_rcv(struct sock *sk, struct sk_buff *skb)
{
	unsigned int truesize = skb->truesize;
	struct sk_buff_head xmitq;
	u32 dnode, selector;

	__skb_queue_head_init(&xmitq);

	if (likely(filter_rcv(sk, skb, &xmitq))) {
		atomic_add(truesize, &tipc_sk(sk)->dupl_rcvcnt);
		return 0;
	}

	if (skb_queue_empty(&xmitq))
		return 0;

	/* Send response/rejected message */
	skb = __skb_dequeue(&xmitq);
	dnode = msg_destnode(buf_msg(skb));
	selector = msg_origport(buf_msg(skb));
	tipc_node_xmit_skb(sock_net(sk), skb, dnode, selector);
>>>>>>> v4.9.227
	return 0;
}

/**
<<<<<<< HEAD
 * tipc_sk_rcv - handle incoming message
 * @buf: buffer containing arriving message
 * Consumes buffer
 * Returns 0 if success, or errno: -EHOSTUNREACH
 */
int tipc_sk_rcv(struct sk_buff *buf)
{
	struct tipc_sock *tsk;
	struct sock *sk;
	u32 dport = msg_destport(buf_msg(buf));
	int rc = TIPC_OK;
	uint limit;
	u32 dnode;

	/* Validate destination and message */
	tsk = tipc_sk_get(dport);
	if (unlikely(!tsk)) {
		rc = tipc_msg_eval(buf, &dnode);
		goto exit;
	}
	sk = &tsk->sk;

	/* Queue message */
	spin_lock_bh(&sk->sk_lock.slock);

	if (!sock_owned_by_user(sk)) {
		rc = filter_rcv(sk, buf);
	} else {
		if (sk->sk_backlog.len == 0)
			atomic_set(&tsk->dupl_rcvcnt, 0);
		limit = rcvbuf_limit(sk, buf) + atomic_read(&tsk->dupl_rcvcnt);
		if (sk_add_backlog(sk, buf, limit))
			rc = -TIPC_ERR_OVERLOAD;
	}
	spin_unlock_bh(&sk->sk_lock.slock);
	tipc_sk_put(tsk);
	if (likely(!rc))
		return 0;
exit:
	if ((rc < 0) && !tipc_msg_reverse(buf, &dnode, -rc))
		return -EHOSTUNREACH;

	tipc_link_xmit(buf, dnode, 0);
	return (rc < 0) ? -EHOSTUNREACH : 0;
=======
 * tipc_sk_enqueue - extract all buffers with destination 'dport' from
 *                   inputq and try adding them to socket or backlog queue
 * @inputq: list of incoming buffers with potentially different destinations
 * @sk: socket where the buffers should be enqueued
 * @dport: port number for the socket
 *
 * Caller must hold socket lock
 */
static void tipc_sk_enqueue(struct sk_buff_head *inputq, struct sock *sk,
			    u32 dport, struct sk_buff_head *xmitq)
{
	unsigned long time_limit = jiffies + 2;
	struct sk_buff *skb;
	unsigned int lim;
	atomic_t *dcnt;
	u32 onode;

	while (skb_queue_len(inputq)) {
		if (unlikely(time_after_eq(jiffies, time_limit)))
			return;

		skb = tipc_skb_dequeue(inputq, dport);
		if (unlikely(!skb))
			return;

		/* Add message directly to receive queue if possible */
		if (!sock_owned_by_user(sk)) {
			filter_rcv(sk, skb, xmitq);
			continue;
		}

		/* Try backlog, compensating for double-counted bytes */
		dcnt = &tipc_sk(sk)->dupl_rcvcnt;
		if (!sk->sk_backlog.len)
			atomic_set(dcnt, 0);
		lim = rcvbuf_limit(sk, skb) + atomic_read(dcnt);
		if (likely(!sk_add_backlog(sk, skb, lim)))
			continue;

		/* Overload => reject message back to sender */
		onode = tipc_own_addr(sock_net(sk));
		if (tipc_msg_reverse(onode, &skb, TIPC_ERR_OVERLOAD))
			__skb_queue_tail(xmitq, skb);
		break;
	}
}

/**
 * tipc_sk_rcv - handle a chain of incoming buffers
 * @inputq: buffer list containing the buffers
 * Consumes all buffers in list until inputq is empty
 * Note: may be called in multiple threads referring to the same queue
 */
void tipc_sk_rcv(struct net *net, struct sk_buff_head *inputq)
{
	struct sk_buff_head xmitq;
	u32 dnode, dport = 0;
	int err;
	struct tipc_sock *tsk;
	struct sock *sk;
	struct sk_buff *skb;

	__skb_queue_head_init(&xmitq);
	while (skb_queue_len(inputq)) {
		dport = tipc_skb_peek_port(inputq, dport);
		tsk = tipc_sk_lookup(net, dport);

		if (likely(tsk)) {
			sk = &tsk->sk;
			if (likely(spin_trylock_bh(&sk->sk_lock.slock))) {
				tipc_sk_enqueue(inputq, sk, dport, &xmitq);
				spin_unlock_bh(&sk->sk_lock.slock);
			}
			/* Send pending response/rejected messages, if any */
			while ((skb = __skb_dequeue(&xmitq))) {
				dnode = msg_destnode(buf_msg(skb));
				tipc_node_xmit_skb(net, skb, dnode, dport);
			}
			sock_put(sk);
			continue;
		}

		/* No destination socket => dequeue skb if still there */
		skb = tipc_skb_dequeue(inputq, dport);
		if (!skb)
			return;

		/* Try secondary lookup if unresolved named message */
		err = TIPC_ERR_NO_PORT;
		if (tipc_msg_lookup_dest(net, skb, &err))
			goto xmit;

		/* Prepare for message rejection */
		if (!tipc_msg_reverse(tipc_own_addr(net), &skb, err))
			continue;
xmit:
		dnode = msg_destnode(buf_msg(skb));
		tipc_node_xmit_skb(net, skb, dnode, dport);
	}
>>>>>>> v4.9.227
}

static int tipc_wait_for_connect(struct socket *sock, long *timeo_p)
{
	struct sock *sk = sock->sk;
	DEFINE_WAIT(wait);
	int done;

	do {
		int err = sock_error(sk);
		if (err)
			return err;
		if (!*timeo_p)
			return -ETIMEDOUT;
		if (signal_pending(current))
			return sock_intr_errno(*timeo_p);

		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
		done = sk_wait_event(sk, timeo_p, sock->state != SS_CONNECTING);
		finish_wait(sk_sleep(sk), &wait);
	} while (!done);
	return 0;
}

/**
 * tipc_connect - establish a connection to another TIPC port
 * @sock: socket structure
 * @dest: socket address for destination port
 * @destlen: size of socket address data structure
 * @flags: file-related flags associated with socket
 *
 * Returns 0 on success, errno otherwise
 */
static int tipc_connect(struct socket *sock, struct sockaddr *dest,
			int destlen, int flags)
{
	struct sock *sk = sock->sk;
<<<<<<< HEAD
	struct sockaddr_tipc *dst = (struct sockaddr_tipc *)dest;
	struct msghdr m = {NULL,};
	long timeout = (flags & O_NONBLOCK) ? 0 : tipc_sk(sk)->conn_timeout;
	socket_state previous;
	int res;

	lock_sock(sk);

	/* For now, TIPC does not allow use of connect() with DGRAM/RDM types */
	if (sock->state == SS_READY) {
		res = -EOPNOTSUPP;
=======
	struct tipc_sock *tsk = tipc_sk(sk);
	struct sockaddr_tipc *dst = (struct sockaddr_tipc *)dest;
	struct msghdr m = {NULL,};
	long timeout = (flags & O_NONBLOCK) ? 0 : tsk->conn_timeout;
	socket_state previous;
	int res = 0;

	lock_sock(sk);

	/* DGRAM/RDM connect(), just save the destaddr */
	if (sock->state == SS_READY) {
		if (dst->family == AF_UNSPEC) {
			memset(&tsk->remote, 0, sizeof(struct sockaddr_tipc));
			tsk->connected = 0;
		} else if (destlen != sizeof(struct sockaddr_tipc)) {
			res = -EINVAL;
		} else {
			memcpy(&tsk->remote, dest, destlen);
			tsk->connected = 1;
		}
>>>>>>> v4.9.227
		goto exit;
	}

	/*
	 * Reject connection attempt using multicast address
	 *
	 * Note: send_msg() validates the rest of the address fields,
	 *       so there's no need to do it here
	 */
	if (dst->addrtype == TIPC_ADDR_MCAST) {
		res = -EINVAL;
		goto exit;
	}

	previous = sock->state;
	switch (sock->state) {
	case SS_UNCONNECTED:
		/* Send a 'SYN-' to destination */
		m.msg_name = dest;
		m.msg_namelen = destlen;

		/* If connect is in non-blocking case, set MSG_DONTWAIT to
		 * indicate send_msg() is never blocked.
		 */
		if (!timeout)
			m.msg_flags = MSG_DONTWAIT;

<<<<<<< HEAD
		res = tipc_sendmsg(NULL, sock, &m, 0);
=======
		res = __tipc_sendmsg(sock, &m, 0);
>>>>>>> v4.9.227
		if ((res < 0) && (res != -EWOULDBLOCK))
			goto exit;

		/* Just entered SS_CONNECTING state; the only
		 * difference is that return value in non-blocking
		 * case is EINPROGRESS, rather than EALREADY.
		 */
		res = -EINPROGRESS;
	case SS_CONNECTING:
		if (previous == SS_CONNECTING)
			res = -EALREADY;
		if (!timeout)
			goto exit;
		timeout = msecs_to_jiffies(timeout);
		/* Wait until an 'ACK' or 'RST' arrives, or a timeout occurs */
		res = tipc_wait_for_connect(sock, &timeout);
		break;
	case SS_CONNECTED:
		res = -EISCONN;
		break;
	default:
		res = -EINVAL;
		break;
	}
exit:
	release_sock(sk);
	return res;
}

/**
 * tipc_listen - allow socket to listen for incoming connections
 * @sock: socket structure
 * @len: (unused)
 *
 * Returns 0 on success, errno otherwise
 */
static int tipc_listen(struct socket *sock, int len)
{
	struct sock *sk = sock->sk;
	int res;

	lock_sock(sk);

	if (sock->state != SS_UNCONNECTED)
		res = -EINVAL;
	else {
		sock->state = SS_LISTENING;
		res = 0;
	}

	release_sock(sk);
	return res;
}

static int tipc_wait_for_accept(struct socket *sock, long timeo)
{
	struct sock *sk = sock->sk;
	DEFINE_WAIT(wait);
	int err;

	/* True wake-one mechanism for incoming connections: only
	 * one process gets woken up, not the 'whole herd'.
	 * Since we do not 'race & poll' for established sockets
	 * anymore, the common case will execute the loop only once.
	*/
	for (;;) {
		prepare_to_wait_exclusive(sk_sleep(sk), &wait,
					  TASK_INTERRUPTIBLE);
		if (timeo && skb_queue_empty(&sk->sk_receive_queue)) {
			release_sock(sk);
			timeo = schedule_timeout(timeo);
			lock_sock(sk);
		}
		err = 0;
		if (!skb_queue_empty(&sk->sk_receive_queue))
			break;
		err = -EINVAL;
		if (sock->state != SS_LISTENING)
			break;
<<<<<<< HEAD
		err = sock_intr_errno(timeo);
		if (signal_pending(current))
			break;
		err = -EAGAIN;
		if (!timeo)
			break;
=======
		err = -EAGAIN;
		if (!timeo)
			break;
		err = sock_intr_errno(timeo);
		if (signal_pending(current))
			break;
>>>>>>> v4.9.227
	}
	finish_wait(sk_sleep(sk), &wait);
	return err;
}

/**
 * tipc_accept - wait for connection request
 * @sock: listening socket
 * @newsock: new socket that is to be connected
 * @flags: file-related flags associated with socket
 *
 * Returns 0 on success, errno otherwise
 */
static int tipc_accept(struct socket *sock, struct socket *new_sock, int flags)
{
	struct sock *new_sk, *sk = sock->sk;
	struct sk_buff *buf;
	struct tipc_sock *new_tsock;
	struct tipc_msg *msg;
	long timeo;
	int res;

	lock_sock(sk);

	if (sock->state != SS_LISTENING) {
		res = -EINVAL;
		goto exit;
	}
	timeo = sock_rcvtimeo(sk, flags & O_NONBLOCK);
	res = tipc_wait_for_accept(sock, timeo);
	if (res)
		goto exit;

	buf = skb_peek(&sk->sk_receive_queue);

	res = tipc_sk_create(sock_net(sock->sk), new_sock, 0, 1);
	if (res)
		goto exit;
	security_sk_clone(sock->sk, new_sock->sk);

	new_sk = new_sock->sk;
	new_tsock = tipc_sk(new_sk);
	msg = buf_msg(buf);

	/* we lock on new_sk; but lockdep sees the lock on sk */
	lock_sock_nested(new_sk, SINGLE_DEPTH_NESTING);

	/*
	 * Reject any stray messages received by new socket
	 * before the socket lock was taken (very, very unlikely)
	 */
	tsk_rej_rx_queue(new_sk);

	/* Connect new socket to it's peer */
	tipc_sk_finish_conn(new_tsock, msg_origport(msg), msg_orignode(msg));
	new_sock->state = SS_CONNECTED;

	tsk_set_importance(new_tsock, msg_importance(msg));
	if (msg_named(msg)) {
		new_tsock->conn_type = msg_nametype(msg);
		new_tsock->conn_instance = msg_nameinst(msg);
	}

	/*
	 * Respond to 'SYN-' by discarding it & returning 'ACK'-.
	 * Respond to 'SYN+' by queuing it on new socket.
	 */
	if (!msg_data_sz(msg)) {
		struct msghdr m = {NULL,};

		tsk_advance_rx_queue(sk);
<<<<<<< HEAD
		tipc_send_packet(NULL, new_sock, &m, 0);
=======
		__tipc_send_stream(new_sock, &m, 0);
>>>>>>> v4.9.227
	} else {
		__skb_dequeue(&sk->sk_receive_queue);
		__skb_queue_head(&new_sk->sk_receive_queue, buf);
		skb_set_owner_r(buf, new_sk);
	}
	release_sock(new_sk);
exit:
	release_sock(sk);
	return res;
}

/**
 * tipc_shutdown - shutdown socket connection
 * @sock: socket structure
 * @how: direction to close (must be SHUT_RDWR)
 *
 * Terminates connection (if necessary), then purges socket's receive queue.
 *
 * Returns 0 on success, errno otherwise
 */
static int tipc_shutdown(struct socket *sock, int how)
{
	struct sock *sk = sock->sk;
<<<<<<< HEAD
	struct tipc_sock *tsk = tipc_sk(sk);
	struct sk_buff *buf;
	u32 dnode;
=======
	struct net *net = sock_net(sk);
	struct tipc_sock *tsk = tipc_sk(sk);
	struct sk_buff *skb;
	u32 dnode = tsk_peer_node(tsk);
	u32 dport = tsk_peer_port(tsk);
	u32 onode = tipc_own_addr(net);
	u32 oport = tsk->portid;
>>>>>>> v4.9.227
	int res;

	if (how != SHUT_RDWR)
		return -EINVAL;

	lock_sock(sk);

	switch (sock->state) {
	case SS_CONNECTING:
	case SS_CONNECTED:

restart:
<<<<<<< HEAD
		/* Disconnect and send a 'FIN+' or 'FIN-' message to peer */
		buf = __skb_dequeue(&sk->sk_receive_queue);
		if (buf) {
			if (TIPC_SKB_CB(buf)->handle != NULL) {
				kfree_skb(buf);
				goto restart;
			}
			if (tipc_msg_reverse(buf, &dnode, TIPC_CONN_SHUTDOWN))
				tipc_link_xmit(buf, dnode, tsk->ref);
			tipc_node_remove_conn(dnode, tsk->ref);
		} else {
			dnode = tsk_peer_node(tsk);
			buf = tipc_msg_create(TIPC_CRITICAL_IMPORTANCE,
					      TIPC_CONN_MSG, SHORT_H_SIZE,
					      0, dnode, tipc_own_addr,
					      tsk_peer_port(tsk),
					      tsk->ref, TIPC_CONN_SHUTDOWN);
			tipc_link_xmit(buf, dnode, tsk->ref);
		}
		tsk->connected = 0;
		sock->state = SS_DISCONNECTING;
		tipc_node_remove_conn(dnode, tsk->ref);
=======
		dnode = tsk_peer_node(tsk);

		/* Disconnect and send a 'FIN+' or 'FIN-' message to peer */
		skb = __skb_dequeue(&sk->sk_receive_queue);
		if (skb) {
			if (TIPC_SKB_CB(skb)->handle != NULL) {
				kfree_skb(skb);
				goto restart;
			}
			tipc_sk_respond(sk, skb, TIPC_CONN_SHUTDOWN);
		} else {
			skb = tipc_msg_create(TIPC_CRITICAL_IMPORTANCE,
					      TIPC_CONN_MSG, SHORT_H_SIZE,
					      0, dnode, onode, dport, oport,
					      TIPC_CONN_SHUTDOWN);
			if (skb)
				tipc_node_xmit_skb(net, skb, dnode, tsk->portid);
		}
		tsk->connected = 0;
		sock->state = SS_DISCONNECTING;
		tipc_node_remove_conn(net, dnode, tsk->portid);
>>>>>>> v4.9.227
		/* fall through */

	case SS_DISCONNECTING:

		/* Discard any unreceived messages */
		__skb_queue_purge(&sk->sk_receive_queue);

		/* Wake up anyone sleeping in poll */
		sk->sk_state_change(sk);
		res = 0;
		break;

	default:
		res = -ENOTCONN;
	}

	release_sock(sk);
	return res;
}

<<<<<<< HEAD
static void tipc_sk_timeout(unsigned long ref)
{
	struct tipc_sock *tsk;
	struct sock *sk;
	struct sk_buff *buf = NULL;
	u32 peer_port, peer_node;

	tsk = tipc_sk_get(ref);
	if (!tsk)
		return;

	sk = &tsk->sk;
=======
static void tipc_sk_timeout(unsigned long data)
{
	struct tipc_sock *tsk = (struct tipc_sock *)data;
	struct sock *sk = &tsk->sk;
	struct sk_buff *skb = NULL;
	u32 peer_port, peer_node;
	u32 own_node = tsk_own_node(tsk);

>>>>>>> v4.9.227
	bh_lock_sock(sk);
	if (!tsk->connected) {
		bh_unlock_sock(sk);
		goto exit;
	}
	peer_port = tsk_peer_port(tsk);
	peer_node = tsk_peer_node(tsk);

	if (tsk->probing_state == TIPC_CONN_PROBING) {
<<<<<<< HEAD
		/* Previous probe not answered -> self abort */
		buf = tipc_msg_create(TIPC_CRITICAL_IMPORTANCE, TIPC_CONN_MSG,
				      SHORT_H_SIZE, 0, tipc_own_addr,
				      peer_node, ref, peer_port,
				      TIPC_ERR_NO_PORT);
	} else {
		buf = tipc_msg_create(CONN_MANAGER, CONN_PROBE, INT_H_SIZE,
				      0, peer_node, tipc_own_addr,
				      peer_port, ref, TIPC_OK);
		tsk->probing_state = TIPC_CONN_PROBING;
		k_start_timer(&tsk->timer, tsk->probing_interval);
	}
	bh_unlock_sock(sk);
	if (buf)
		tipc_link_xmit(buf, peer_node, ref);
exit:
	tipc_sk_put(tsk);
=======
		if (!sock_owned_by_user(sk)) {
			sk->sk_socket->state = SS_DISCONNECTING;
			tsk->connected = 0;
			tipc_node_remove_conn(sock_net(sk), tsk_peer_node(tsk),
					      tsk_peer_port(tsk));
			sk->sk_state_change(sk);
		} else {
			/* Try again later */
			sk_reset_timer(sk, &sk->sk_timer, (HZ / 20));
		}

	} else {
		skb = tipc_msg_create(CONN_MANAGER, CONN_PROBE,
				      INT_H_SIZE, 0, peer_node, own_node,
				      peer_port, tsk->portid, TIPC_OK);
		tsk->probing_state = TIPC_CONN_PROBING;
		sk_reset_timer(sk, &sk->sk_timer, jiffies + tsk->probing_intv);
	}
	bh_unlock_sock(sk);
	if (skb)
		tipc_node_xmit_skb(sock_net(sk), skb, peer_node, tsk->portid);
exit:
	sock_put(sk);
>>>>>>> v4.9.227
}

static int tipc_sk_publish(struct tipc_sock *tsk, uint scope,
			   struct tipc_name_seq const *seq)
{
<<<<<<< HEAD
=======
	struct net *net = sock_net(&tsk->sk);
>>>>>>> v4.9.227
	struct publication *publ;
	u32 key;

	if (tsk->connected)
		return -EINVAL;
<<<<<<< HEAD
	key = tsk->ref + tsk->pub_count + 1;
	if (key == tsk->ref)
		return -EADDRINUSE;

	publ = tipc_nametbl_publish(seq->type, seq->lower, seq->upper,
				    scope, tsk->ref, key);
=======
	key = tsk->portid + tsk->pub_count + 1;
	if (key == tsk->portid)
		return -EADDRINUSE;

	publ = tipc_nametbl_publish(net, seq->type, seq->lower, seq->upper,
				    scope, tsk->portid, key);
>>>>>>> v4.9.227
	if (unlikely(!publ))
		return -EINVAL;

	list_add(&publ->pport_list, &tsk->publications);
	tsk->pub_count++;
	tsk->published = 1;
	return 0;
}

static int tipc_sk_withdraw(struct tipc_sock *tsk, uint scope,
			    struct tipc_name_seq const *seq)
{
<<<<<<< HEAD
=======
	struct net *net = sock_net(&tsk->sk);
>>>>>>> v4.9.227
	struct publication *publ;
	struct publication *safe;
	int rc = -EINVAL;

	list_for_each_entry_safe(publ, safe, &tsk->publications, pport_list) {
		if (seq) {
			if (publ->scope != scope)
				continue;
			if (publ->type != seq->type)
				continue;
			if (publ->lower != seq->lower)
				continue;
			if (publ->upper != seq->upper)
				break;
<<<<<<< HEAD
			tipc_nametbl_withdraw(publ->type, publ->lower,
=======
			tipc_nametbl_withdraw(net, publ->type, publ->lower,
>>>>>>> v4.9.227
					      publ->ref, publ->key);
			rc = 0;
			break;
		}
<<<<<<< HEAD
		tipc_nametbl_withdraw(publ->type, publ->lower,
=======
		tipc_nametbl_withdraw(net, publ->type, publ->lower,
>>>>>>> v4.9.227
				      publ->ref, publ->key);
		rc = 0;
	}
	if (list_empty(&tsk->publications))
		tsk->published = 0;
	return rc;
}

<<<<<<< HEAD
static int tipc_sk_show(struct tipc_sock *tsk, char *buf,
			int len, int full_id)
{
	struct publication *publ;
	int ret;

	if (full_id)
		ret = tipc_snprintf(buf, len, "<%u.%u.%u:%u>:",
				    tipc_zone(tipc_own_addr),
				    tipc_cluster(tipc_own_addr),
				    tipc_node(tipc_own_addr), tsk->ref);
	else
		ret = tipc_snprintf(buf, len, "%-10u:", tsk->ref);

	if (tsk->connected) {
		u32 dport = tsk_peer_port(tsk);
		u32 destnode = tsk_peer_node(tsk);

		ret += tipc_snprintf(buf + ret, len - ret,
				     " connected to <%u.%u.%u:%u>",
				     tipc_zone(destnode),
				     tipc_cluster(destnode),
				     tipc_node(destnode), dport);
		if (tsk->conn_type != 0)
			ret += tipc_snprintf(buf + ret, len - ret,
					     " via {%u,%u}", tsk->conn_type,
					     tsk->conn_instance);
	} else if (tsk->published) {
		ret += tipc_snprintf(buf + ret, len - ret, " bound to");
		list_for_each_entry(publ, &tsk->publications, pport_list) {
			if (publ->lower == publ->upper)
				ret += tipc_snprintf(buf + ret, len - ret,
						     " {%u,%u}", publ->type,
						     publ->lower);
			else
				ret += tipc_snprintf(buf + ret, len - ret,
						     " {%u,%u,%u}", publ->type,
						     publ->lower, publ->upper);
		}
	}
	ret += tipc_snprintf(buf + ret, len - ret, "\n");
	return ret;
}

struct sk_buff *tipc_sk_socks_show(void)
{
	struct sk_buff *buf;
	struct tlv_desc *rep_tlv;
	char *pb;
	int pb_len;
	struct tipc_sock *tsk;
	int str_len = 0;
	u32 ref = 0;

	buf = tipc_cfg_reply_alloc(TLV_SPACE(ULTRA_STRING_MAX_LEN));
	if (!buf)
		return NULL;
	rep_tlv = (struct tlv_desc *)buf->data;
	pb = TLV_DATA(rep_tlv);
	pb_len = ULTRA_STRING_MAX_LEN;

	tsk = tipc_sk_get_next(&ref);
	for (; tsk; tsk = tipc_sk_get_next(&ref)) {
		lock_sock(&tsk->sk);
		str_len += tipc_sk_show(tsk, pb + str_len,
					pb_len - str_len, 0);
		release_sock(&tsk->sk);
		tipc_sk_put(tsk);
	}
	str_len += 1;	/* for "\0" */
	skb_put(buf, TLV_SPACE(str_len));
	TLV_SET(rep_tlv, TIPC_TLV_ULTRA_STRING, NULL, str_len);

	return buf;
}

/* tipc_sk_reinit: set non-zero address in all existing sockets
 *                 when we go from standalone to network mode.
 */
void tipc_sk_reinit(void)
{
	struct tipc_msg *msg;
	u32 ref = 0;
	struct tipc_sock *tsk = tipc_sk_get_next(&ref);

	for (; tsk; tsk = tipc_sk_get_next(&ref)) {
		lock_sock(&tsk->sk);
		msg = &tsk->phdr;
		msg_set_prevnode(msg, tipc_own_addr);
		msg_set_orignode(msg, tipc_own_addr);
		release_sock(&tsk->sk);
		tipc_sk_put(tsk);
	}
}

/**
 * struct reference - TIPC socket reference entry
 * @tsk: pointer to socket associated with reference entry
 * @ref: reference value for socket (combines instance & array index info)
 */
struct reference {
	struct tipc_sock *tsk;
	u32 ref;
};

/**
 * struct tipc_ref_table - table of TIPC socket reference entries
 * @entries: pointer to array of reference entries
 * @capacity: array index of first unusable entry
 * @init_point: array index of first uninitialized entry
 * @first_free: array index of first unused socket reference entry
 * @last_free: array index of last unused socket reference entry
 * @index_mask: bitmask for array index portion of reference values
 * @start_mask: initial value for instance value portion of reference values
 */
struct ref_table {
	struct reference *entries;
	u32 capacity;
	u32 init_point;
	u32 first_free;
	u32 last_free;
	u32 index_mask;
	u32 start_mask;
};

/* Socket reference table consists of 2**N entries.
 *
 * State	Socket ptr	Reference
 * -----        ----------      ---------
 * In use        non-NULL       XXXX|own index
 *				(XXXX changes each time entry is acquired)
 * Free            NULL         YYYY|next free index
 *				(YYYY is one more than last used XXXX)
 * Uninitialized   NULL         0
 *
 * Entry 0 is not used; this allows index 0 to denote the end of the free list.
 *
 * Note that a reference value of 0 does not necessarily indicate that an
 * entry is uninitialized, since the last entry in the free list could also
 * have a reference value of 0 (although this is unlikely).
 */

static struct ref_table tipc_ref_table;

static DEFINE_RWLOCK(ref_table_lock);

/**
 * tipc_ref_table_init - create reference table for sockets
 */
int tipc_sk_ref_table_init(u32 req_sz, u32 start)
{
	struct reference *table;
	u32 actual_sz;

	/* account for unused entry, then round up size to a power of 2 */

	req_sz++;
	for (actual_sz = 16; actual_sz < req_sz; actual_sz <<= 1) {
		/* do nothing */
	};

	/* allocate table & mark all entries as uninitialized */
	table = vzalloc(actual_sz * sizeof(struct reference));
	if (table == NULL)
		return -ENOMEM;

	tipc_ref_table.entries = table;
	tipc_ref_table.capacity = req_sz;
	tipc_ref_table.init_point = 1;
	tipc_ref_table.first_free = 0;
	tipc_ref_table.last_free = 0;
	tipc_ref_table.index_mask = actual_sz - 1;
	tipc_ref_table.start_mask = start & ~tipc_ref_table.index_mask;

	return 0;
}

/**
 * tipc_ref_table_stop - destroy reference table for sockets
 */
void tipc_sk_ref_table_stop(void)
{
	if (!tipc_ref_table.entries)
		return;
	vfree(tipc_ref_table.entries);
	tipc_ref_table.entries = NULL;
}

/* tipc_ref_acquire - create reference to a socket
 *
 * Register an socket pointer in the reference table.
 * Returns a unique reference value that is used from then on to retrieve the
 * socket pointer, or to determine if the socket has been deregistered.
 */
u32 tipc_sk_ref_acquire(struct tipc_sock *tsk)
{
	u32 index;
	u32 index_mask;
	u32 next_plus_upper;
	u32 ref = 0;
	struct reference *entry;

	if (unlikely(!tsk)) {
		pr_err("Attempt to acquire ref. to non-existent obj\n");
		return 0;
	}
	if (unlikely(!tipc_ref_table.entries)) {
		pr_err("Ref. table not found in acquisition attempt\n");
		return 0;
	}

	/* Take a free entry, if available; otherwise initialize a new one */
	write_lock_bh(&ref_table_lock);
	index = tipc_ref_table.first_free;
	entry = &tipc_ref_table.entries[index];

	if (likely(index)) {
		index = tipc_ref_table.first_free;
		entry = &tipc_ref_table.entries[index];
		index_mask = tipc_ref_table.index_mask;
		next_plus_upper = entry->ref;
		tipc_ref_table.first_free = next_plus_upper & index_mask;
		ref = (next_plus_upper & ~index_mask) + index;
		entry->tsk = tsk;
	} else if (tipc_ref_table.init_point < tipc_ref_table.capacity) {
		index = tipc_ref_table.init_point++;
		entry = &tipc_ref_table.entries[index];
		ref = tipc_ref_table.start_mask + index;
	}

	if (ref) {
		entry->ref = ref;
		entry->tsk = tsk;
	}
	write_unlock_bh(&ref_table_lock);
	return ref;
}

/* tipc_sk_ref_discard - invalidate reference to an socket
 *
 * Disallow future references to an socket and free up the entry for re-use.
 */
void tipc_sk_ref_discard(u32 ref)
{
	struct reference *entry;
	u32 index;
	u32 index_mask;

	if (unlikely(!tipc_ref_table.entries)) {
		pr_err("Ref. table not found during discard attempt\n");
		return;
	}

	index_mask = tipc_ref_table.index_mask;
	index = ref & index_mask;
	entry = &tipc_ref_table.entries[index];

	write_lock_bh(&ref_table_lock);

	if (unlikely(!entry->tsk)) {
		pr_err("Attempt to discard ref. to non-existent socket\n");
		goto exit;
	}
	if (unlikely(entry->ref != ref)) {
		pr_err("Attempt to discard non-existent reference\n");
		goto exit;
	}

	/* Mark entry as unused; increment instance part of entry's
	 *   reference to invalidate any subsequent references
	 */

	entry->tsk = NULL;
	entry->ref = (ref & ~index_mask) + (index_mask + 1);

	/* Append entry to free entry list */
	if (unlikely(tipc_ref_table.first_free == 0))
		tipc_ref_table.first_free = index;
	else
		tipc_ref_table.entries[tipc_ref_table.last_free].ref |= index;
	tipc_ref_table.last_free = index;
exit:
	write_unlock_bh(&ref_table_lock);
}

/* tipc_sk_get - find referenced socket and return pointer to it
 */
struct tipc_sock *tipc_sk_get(u32 ref)
{
	struct reference *entry;
	struct tipc_sock *tsk;

	if (unlikely(!tipc_ref_table.entries))
		return NULL;
	read_lock_bh(&ref_table_lock);
	entry = &tipc_ref_table.entries[ref & tipc_ref_table.index_mask];
	tsk = entry->tsk;
	if (likely(tsk && (entry->ref == ref)))
		sock_hold(&tsk->sk);
	else
		tsk = NULL;
	read_unlock_bh(&ref_table_lock);
	return tsk;
}

/* tipc_sk_get_next - lock & return next socket after referenced one
*/
struct tipc_sock *tipc_sk_get_next(u32 *ref)
{
	struct reference *entry;
	struct tipc_sock *tsk = NULL;
	uint index = *ref & tipc_ref_table.index_mask;

	read_lock_bh(&ref_table_lock);
	while (++index < tipc_ref_table.capacity) {
		entry = &tipc_ref_table.entries[index];
		if (!entry->tsk)
			continue;
		tsk = entry->tsk;
		sock_hold(&tsk->sk);
		*ref = entry->ref;
		break;
	}
	read_unlock_bh(&ref_table_lock);
	return tsk;
}

static void tipc_sk_put(struct tipc_sock *tsk)
{
	sock_put(&tsk->sk);
=======
/* tipc_sk_reinit: set non-zero address in all existing sockets
 *                 when we go from standalone to network mode.
 */
void tipc_sk_reinit(struct net *net)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	struct rhashtable_iter iter;
	struct tipc_sock *tsk;
	struct tipc_msg *msg;

	rhashtable_walk_enter(&tn->sk_rht, &iter);

	do {
		tsk = ERR_PTR(rhashtable_walk_start(&iter));
		if (IS_ERR(tsk))
			goto walk_stop;

		while ((tsk = rhashtable_walk_next(&iter)) && !IS_ERR(tsk)) {
			sock_hold(&tsk->sk);
			rhashtable_walk_stop(&iter);
			lock_sock(&tsk->sk);
			msg = &tsk->phdr;
			msg_set_prevnode(msg, tn->own_addr);
			msg_set_orignode(msg, tn->own_addr);
			release_sock(&tsk->sk);
			rhashtable_walk_start(&iter);
			sock_put(&tsk->sk);
		}
walk_stop:
		rhashtable_walk_stop(&iter);
	} while (tsk == ERR_PTR(-EAGAIN));
}

static struct tipc_sock *tipc_sk_lookup(struct net *net, u32 portid)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	struct tipc_sock *tsk;

	rcu_read_lock();
	tsk = rhashtable_lookup_fast(&tn->sk_rht, &portid, tsk_rht_params);
	if (tsk)
		sock_hold(&tsk->sk);
	rcu_read_unlock();

	return tsk;
}

static int tipc_sk_insert(struct tipc_sock *tsk)
{
	struct sock *sk = &tsk->sk;
	struct net *net = sock_net(sk);
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	u32 remaining = (TIPC_MAX_PORT - TIPC_MIN_PORT) + 1;
	u32 portid = prandom_u32() % remaining + TIPC_MIN_PORT;

	while (remaining--) {
		portid++;
		if ((portid < TIPC_MIN_PORT) || (portid > TIPC_MAX_PORT))
			portid = TIPC_MIN_PORT;
		tsk->portid = portid;
		sock_hold(&tsk->sk);
		if (!rhashtable_lookup_insert_fast(&tn->sk_rht, &tsk->node,
						   tsk_rht_params))
			return 0;
		sock_put(&tsk->sk);
	}

	return -1;
}

static void tipc_sk_remove(struct tipc_sock *tsk)
{
	struct sock *sk = &tsk->sk;
	struct tipc_net *tn = net_generic(sock_net(sk), tipc_net_id);

	if (!rhashtable_remove_fast(&tn->sk_rht, &tsk->node, tsk_rht_params)) {
		WARN_ON(atomic_read(&sk->sk_refcnt) == 1);
		__sock_put(sk);
	}
}

static const struct rhashtable_params tsk_rht_params = {
	.nelem_hint = 192,
	.head_offset = offsetof(struct tipc_sock, node),
	.key_offset = offsetof(struct tipc_sock, portid),
	.key_len = sizeof(u32), /* portid */
	.max_size = 1048576,
	.min_size = 256,
	.automatic_shrinking = true,
};

int tipc_sk_rht_init(struct net *net)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);

	return rhashtable_init(&tn->sk_rht, &tsk_rht_params);
}

void tipc_sk_rht_destroy(struct net *net)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);

	/* Wait for socket readers to complete */
	synchronize_net();

	rhashtable_destroy(&tn->sk_rht);
>>>>>>> v4.9.227
}

/**
 * tipc_setsockopt - set socket option
 * @sock: socket structure
 * @lvl: option level
 * @opt: option identifier
 * @ov: pointer to new option value
 * @ol: length of option value
 *
 * For stream sockets only, accepts and ignores all IPPROTO_TCP options
 * (to ease compatibility).
 *
 * Returns 0 on success, errno otherwise
 */
static int tipc_setsockopt(struct socket *sock, int lvl, int opt,
			   char __user *ov, unsigned int ol)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	u32 value;
	int res;

	if ((lvl == IPPROTO_TCP) && (sock->type == SOCK_STREAM))
		return 0;
	if (lvl != SOL_TIPC)
		return -ENOPROTOOPT;
	if (ol < sizeof(value))
		return -EINVAL;
	res = get_user(value, (u32 __user *)ov);
	if (res)
		return res;

	lock_sock(sk);

	switch (opt) {
	case TIPC_IMPORTANCE:
		res = tsk_set_importance(tsk, value);
		break;
	case TIPC_SRC_DROPPABLE:
		if (sock->type != SOCK_STREAM)
			tsk_set_unreliable(tsk, value);
		else
			res = -ENOPROTOOPT;
		break;
	case TIPC_DEST_DROPPABLE:
		tsk_set_unreturnable(tsk, value);
		break;
	case TIPC_CONN_TIMEOUT:
		tipc_sk(sk)->conn_timeout = value;
		/* no need to set "res", since already 0 at this point */
		break;
	default:
		res = -EINVAL;
	}

	release_sock(sk);

	return res;
}

/**
 * tipc_getsockopt - get socket option
 * @sock: socket structure
 * @lvl: option level
 * @opt: option identifier
 * @ov: receptacle for option value
 * @ol: receptacle for length of option value
 *
 * For stream sockets only, returns 0 length result for all IPPROTO_TCP options
 * (to ease compatibility).
 *
 * Returns 0 on success, errno otherwise
 */
static int tipc_getsockopt(struct socket *sock, int lvl, int opt,
			   char __user *ov, int __user *ol)
{
	struct sock *sk = sock->sk;
	struct tipc_sock *tsk = tipc_sk(sk);
	int len;
	u32 value;
	int res;

	if ((lvl == IPPROTO_TCP) && (sock->type == SOCK_STREAM))
		return put_user(0, ol);
	if (lvl != SOL_TIPC)
		return -ENOPROTOOPT;
	res = get_user(len, ol);
	if (res)
		return res;

	lock_sock(sk);

	switch (opt) {
	case TIPC_IMPORTANCE:
		value = tsk_importance(tsk);
		break;
	case TIPC_SRC_DROPPABLE:
		value = tsk_unreliable(tsk);
		break;
	case TIPC_DEST_DROPPABLE:
		value = tsk_unreturnable(tsk);
		break;
	case TIPC_CONN_TIMEOUT:
		value = tsk->conn_timeout;
		/* no need to set "res", since already 0 at this point */
		break;
	case TIPC_NODE_RECVQ_DEPTH:
		value = 0; /* was tipc_queue_size, now obsolete */
		break;
	case TIPC_SOCK_RECVQ_DEPTH:
		value = skb_queue_len(&sk->sk_receive_queue);
		break;
	default:
		res = -EINVAL;
	}

	release_sock(sk);

	if (res)
		return res;	/* "get" failed */

	if (len < sizeof(value))
		return -EINVAL;

	if (copy_to_user(ov, &value, sizeof(value)))
		return -EFAULT;

	return put_user(sizeof(value), ol);
}

<<<<<<< HEAD
static int tipc_ioctl(struct socket *sk, unsigned int cmd, unsigned long arg)
{
=======
static int tipc_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct sock *sk = sock->sk;
>>>>>>> v4.9.227
	struct tipc_sioc_ln_req lnr;
	void __user *argp = (void __user *)arg;

	switch (cmd) {
	case SIOCGETLINKNAME:
		if (copy_from_user(&lnr, argp, sizeof(lnr)))
			return -EFAULT;
<<<<<<< HEAD
		if (!tipc_node_get_linkname(lnr.bearer_id & 0xffff, lnr.peer,
=======
		if (!tipc_node_get_linkname(sock_net(sk),
					    lnr.bearer_id & 0xffff, lnr.peer,
>>>>>>> v4.9.227
					    lnr.linkname, TIPC_MAX_LINK_NAME)) {
			if (copy_to_user(argp, &lnr, sizeof(lnr)))
				return -EFAULT;
			return 0;
		}
		return -EADDRNOTAVAIL;
	default:
		return -ENOIOCTLCMD;
	}
}

/* Protocol switches for the various types of TIPC sockets */

static const struct proto_ops msg_ops = {
	.owner		= THIS_MODULE,
	.family		= AF_TIPC,
	.release	= tipc_release,
	.bind		= tipc_bind,
	.connect	= tipc_connect,
	.socketpair	= sock_no_socketpair,
	.accept		= sock_no_accept,
	.getname	= tipc_getname,
	.poll		= tipc_poll,
	.ioctl		= tipc_ioctl,
	.listen		= sock_no_listen,
	.shutdown	= tipc_shutdown,
	.setsockopt	= tipc_setsockopt,
	.getsockopt	= tipc_getsockopt,
	.sendmsg	= tipc_sendmsg,
	.recvmsg	= tipc_recvmsg,
	.mmap		= sock_no_mmap,
	.sendpage	= sock_no_sendpage
};

static const struct proto_ops packet_ops = {
	.owner		= THIS_MODULE,
	.family		= AF_TIPC,
	.release	= tipc_release,
	.bind		= tipc_bind,
	.connect	= tipc_connect,
	.socketpair	= sock_no_socketpair,
	.accept		= tipc_accept,
	.getname	= tipc_getname,
	.poll		= tipc_poll,
	.ioctl		= tipc_ioctl,
	.listen		= tipc_listen,
	.shutdown	= tipc_shutdown,
	.setsockopt	= tipc_setsockopt,
	.getsockopt	= tipc_getsockopt,
	.sendmsg	= tipc_send_packet,
	.recvmsg	= tipc_recvmsg,
	.mmap		= sock_no_mmap,
	.sendpage	= sock_no_sendpage
};

static const struct proto_ops stream_ops = {
	.owner		= THIS_MODULE,
	.family		= AF_TIPC,
	.release	= tipc_release,
	.bind		= tipc_bind,
	.connect	= tipc_connect,
	.socketpair	= sock_no_socketpair,
	.accept		= tipc_accept,
	.getname	= tipc_getname,
	.poll		= tipc_poll,
	.ioctl		= tipc_ioctl,
	.listen		= tipc_listen,
	.shutdown	= tipc_shutdown,
	.setsockopt	= tipc_setsockopt,
	.getsockopt	= tipc_getsockopt,
	.sendmsg	= tipc_send_stream,
	.recvmsg	= tipc_recv_stream,
	.mmap		= sock_no_mmap,
	.sendpage	= sock_no_sendpage
};

static const struct net_proto_family tipc_family_ops = {
	.owner		= THIS_MODULE,
	.family		= AF_TIPC,
	.create		= tipc_sk_create
};

static struct proto tipc_proto = {
	.name		= "TIPC",
	.owner		= THIS_MODULE,
	.obj_size	= sizeof(struct tipc_sock),
	.sysctl_rmem	= sysctl_tipc_rmem
};

<<<<<<< HEAD
static struct proto tipc_proto_kern = {
	.name		= "TIPC",
	.obj_size	= sizeof(struct tipc_sock),
	.sysctl_rmem	= sysctl_tipc_rmem
};

=======
>>>>>>> v4.9.227
/**
 * tipc_socket_init - initialize TIPC socket interface
 *
 * Returns 0 on success, errno otherwise
 */
int tipc_socket_init(void)
{
	int res;

	res = proto_register(&tipc_proto, 1);
	if (res) {
		pr_err("Failed to register TIPC protocol type\n");
		goto out;
	}

	res = sock_register(&tipc_family_ops);
	if (res) {
		pr_err("Failed to register TIPC socket type\n");
		proto_unregister(&tipc_proto);
		goto out;
	}
 out:
	return res;
}

/**
 * tipc_socket_stop - stop TIPC socket interface
 */
void tipc_socket_stop(void)
{
	sock_unregister(tipc_family_ops.family);
	proto_unregister(&tipc_proto);
}
<<<<<<< HEAD
=======

/* Caller should hold socket lock for the passed tipc socket. */
static int __tipc_nl_add_sk_con(struct sk_buff *skb, struct tipc_sock *tsk)
{
	u32 peer_node;
	u32 peer_port;
	struct nlattr *nest;

	peer_node = tsk_peer_node(tsk);
	peer_port = tsk_peer_port(tsk);

	nest = nla_nest_start(skb, TIPC_NLA_SOCK_CON);

	if (nla_put_u32(skb, TIPC_NLA_CON_NODE, peer_node))
		goto msg_full;
	if (nla_put_u32(skb, TIPC_NLA_CON_SOCK, peer_port))
		goto msg_full;

	if (tsk->conn_type != 0) {
		if (nla_put_flag(skb, TIPC_NLA_CON_FLAG))
			goto msg_full;
		if (nla_put_u32(skb, TIPC_NLA_CON_TYPE, tsk->conn_type))
			goto msg_full;
		if (nla_put_u32(skb, TIPC_NLA_CON_INST, tsk->conn_instance))
			goto msg_full;
	}
	nla_nest_end(skb, nest);

	return 0;

msg_full:
	nla_nest_cancel(skb, nest);

	return -EMSGSIZE;
}

/* Caller should hold socket lock for the passed tipc socket. */
static int __tipc_nl_add_sk(struct sk_buff *skb, struct netlink_callback *cb,
			    struct tipc_sock *tsk)
{
	int err;
	void *hdr;
	struct nlattr *attrs;
	struct net *net = sock_net(skb->sk);
	struct tipc_net *tn = net_generic(net, tipc_net_id);

	hdr = genlmsg_put(skb, NETLINK_CB(cb->skb).portid, cb->nlh->nlmsg_seq,
			  &tipc_genl_family, NLM_F_MULTI, TIPC_NL_SOCK_GET);
	if (!hdr)
		goto msg_cancel;

	attrs = nla_nest_start(skb, TIPC_NLA_SOCK);
	if (!attrs)
		goto genlmsg_cancel;
	if (nla_put_u32(skb, TIPC_NLA_SOCK_REF, tsk->portid))
		goto attr_msg_cancel;
	if (nla_put_u32(skb, TIPC_NLA_SOCK_ADDR, tn->own_addr))
		goto attr_msg_cancel;

	if (tsk->connected) {
		err = __tipc_nl_add_sk_con(skb, tsk);
		if (err)
			goto attr_msg_cancel;
	} else if (!list_empty(&tsk->publications)) {
		if (nla_put_flag(skb, TIPC_NLA_SOCK_HAS_PUBL))
			goto attr_msg_cancel;
	}
	nla_nest_end(skb, attrs);
	genlmsg_end(skb, hdr);

	return 0;

attr_msg_cancel:
	nla_nest_cancel(skb, attrs);
genlmsg_cancel:
	genlmsg_cancel(skb, hdr);
msg_cancel:
	return -EMSGSIZE;
}

int tipc_nl_sk_dump(struct sk_buff *skb, struct netlink_callback *cb)
{
	int err;
	struct tipc_sock *tsk;
	const struct bucket_table *tbl;
	struct rhash_head *pos;
	struct net *net = sock_net(skb->sk);
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	u32 tbl_id = cb->args[0];
	u32 prev_portid = cb->args[1];

	rcu_read_lock();
	tbl = rht_dereference_rcu((&tn->sk_rht)->tbl, &tn->sk_rht);
	for (; tbl_id < tbl->size; tbl_id++) {
		rht_for_each_entry_rcu(tsk, pos, tbl, tbl_id, node) {
			spin_lock_bh(&tsk->sk.sk_lock.slock);
			if (prev_portid && prev_portid != tsk->portid) {
				spin_unlock_bh(&tsk->sk.sk_lock.slock);
				continue;
			}

			err = __tipc_nl_add_sk(skb, cb, tsk);
			if (err) {
				prev_portid = tsk->portid;
				spin_unlock_bh(&tsk->sk.sk_lock.slock);
				goto out;
			}
			prev_portid = 0;
			spin_unlock_bh(&tsk->sk.sk_lock.slock);
		}
	}
out:
	rcu_read_unlock();
	cb->args[0] = tbl_id;
	cb->args[1] = prev_portid;

	return skb->len;
}

/* Caller should hold socket lock for the passed tipc socket. */
static int __tipc_nl_add_sk_publ(struct sk_buff *skb,
				 struct netlink_callback *cb,
				 struct publication *publ)
{
	void *hdr;
	struct nlattr *attrs;

	hdr = genlmsg_put(skb, NETLINK_CB(cb->skb).portid, cb->nlh->nlmsg_seq,
			  &tipc_genl_family, NLM_F_MULTI, TIPC_NL_PUBL_GET);
	if (!hdr)
		goto msg_cancel;

	attrs = nla_nest_start(skb, TIPC_NLA_PUBL);
	if (!attrs)
		goto genlmsg_cancel;

	if (nla_put_u32(skb, TIPC_NLA_PUBL_KEY, publ->key))
		goto attr_msg_cancel;
	if (nla_put_u32(skb, TIPC_NLA_PUBL_TYPE, publ->type))
		goto attr_msg_cancel;
	if (nla_put_u32(skb, TIPC_NLA_PUBL_LOWER, publ->lower))
		goto attr_msg_cancel;
	if (nla_put_u32(skb, TIPC_NLA_PUBL_UPPER, publ->upper))
		goto attr_msg_cancel;

	nla_nest_end(skb, attrs);
	genlmsg_end(skb, hdr);

	return 0;

attr_msg_cancel:
	nla_nest_cancel(skb, attrs);
genlmsg_cancel:
	genlmsg_cancel(skb, hdr);
msg_cancel:
	return -EMSGSIZE;
}

/* Caller should hold socket lock for the passed tipc socket. */
static int __tipc_nl_list_sk_publ(struct sk_buff *skb,
				  struct netlink_callback *cb,
				  struct tipc_sock *tsk, u32 *last_publ)
{
	int err;
	struct publication *p;

	if (*last_publ) {
		list_for_each_entry(p, &tsk->publications, pport_list) {
			if (p->key == *last_publ)
				break;
		}
		if (p->key != *last_publ) {
			/* We never set seq or call nl_dump_check_consistent()
			 * this means that setting prev_seq here will cause the
			 * consistence check to fail in the netlink callback
			 * handler. Resulting in the last NLMSG_DONE message
			 * having the NLM_F_DUMP_INTR flag set.
			 */
			cb->prev_seq = 1;
			*last_publ = 0;
			return -EPIPE;
		}
	} else {
		p = list_first_entry(&tsk->publications, struct publication,
				     pport_list);
	}

	list_for_each_entry_from(p, &tsk->publications, pport_list) {
		err = __tipc_nl_add_sk_publ(skb, cb, p);
		if (err) {
			*last_publ = p->key;
			return err;
		}
	}
	*last_publ = 0;

	return 0;
}

int tipc_nl_publ_dump(struct sk_buff *skb, struct netlink_callback *cb)
{
	int err;
	u32 tsk_portid = cb->args[0];
	u32 last_publ = cb->args[1];
	u32 done = cb->args[2];
	struct net *net = sock_net(skb->sk);
	struct tipc_sock *tsk;

	if (!tsk_portid) {
		struct nlattr **attrs;
		struct nlattr *sock[TIPC_NLA_SOCK_MAX + 1];

		err = tipc_nlmsg_parse(cb->nlh, &attrs);
		if (err)
			return err;

		if (!attrs[TIPC_NLA_SOCK])
			return -EINVAL;

		err = nla_parse_nested(sock, TIPC_NLA_SOCK_MAX,
				       attrs[TIPC_NLA_SOCK],
				       tipc_nl_sock_policy);
		if (err)
			return err;

		if (!sock[TIPC_NLA_SOCK_REF])
			return -EINVAL;

		tsk_portid = nla_get_u32(sock[TIPC_NLA_SOCK_REF]);
	}

	if (done)
		return 0;

	tsk = tipc_sk_lookup(net, tsk_portid);
	if (!tsk)
		return -EINVAL;

	lock_sock(&tsk->sk);
	err = __tipc_nl_list_sk_publ(skb, cb, tsk, &last_publ);
	if (!err)
		done = 1;
	release_sock(&tsk->sk);
	sock_put(&tsk->sk);

	cb->args[0] = tsk_portid;
	cb->args[1] = last_publ;
	cb->args[2] = done;

	return skb->len;
}
>>>>>>> v4.9.227
