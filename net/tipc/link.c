/*
 * net/tipc/link.c: TIPC link code
 *
<<<<<<< HEAD
 * Copyright (c) 1996-2007, 2012-2014, Ericsson AB
=======
 * Copyright (c) 1996-2007, 2012-2016, Ericsson AB
>>>>>>> v4.9.227
 * Copyright (c) 2004-2007, 2010-2013, Wind River Systems
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

#include "core.h"
<<<<<<< HEAD
#include "link.h"
#include "socket.h"
#include "name_distr.h"
#include "discover.h"
#include "config.h"

#include <linux/pkt_sched.h>

/*
 * Error message prefixes
 */
static const char *link_co_err = "Link changeover error, ";
static const char *link_rst_msg = "Resetting link ";
static const char *link_unk_evt = "Unknown link event ";

/*
 * Out-of-range value for link session numbers
 */
#define INVALID_SESSION 0x10000

/*
 * Link state events:
 */
#define  STARTING_EVT    856384768	/* link processing trigger */
#define  TRAFFIC_MSG_EVT 560815u	/* rx'd ??? */
#define  TIMEOUT_EVT     560817u	/* link timer expired */

/*
 * The following two 'message types' is really just implementation
 * data conveniently stored in the message header.
 * They must not be considered part of the protocol
 */
#define OPEN_MSG   0
#define CLOSED_MSG 1

/*
 * State value stored in 'exp_msg_count'
 */
#define START_CHANGEOVER 100000u

static void link_handle_out_of_seq_msg(struct tipc_link *l_ptr,
				       struct sk_buff *buf);
static void tipc_link_proto_rcv(struct tipc_link *l_ptr, struct sk_buff *buf);
static int  tipc_link_tunnel_rcv(struct tipc_node *n_ptr,
				 struct sk_buff **buf);
static void link_set_supervision_props(struct tipc_link *l_ptr, u32 tolerance);
static void link_state_event(struct tipc_link *l_ptr, u32 event);
static void link_reset_statistics(struct tipc_link *l_ptr);
static void link_print(struct tipc_link *l_ptr, const char *str);
static void tipc_link_sync_xmit(struct tipc_link *l);
static void tipc_link_sync_rcv(struct tipc_node *n, struct sk_buff *buf);
static int tipc_link_input(struct tipc_link *l, struct sk_buff *buf);
static int tipc_link_prepare_input(struct tipc_link *l, struct sk_buff **buf);

/*
 *  Simple link routines
 */
static unsigned int align(unsigned int i)
{
	return (i + 3) & ~3u;
}

static void link_init_max_pkt(struct tipc_link *l_ptr)
{
	struct tipc_bearer *b_ptr;
	u32 max_pkt;

	rcu_read_lock();
	b_ptr = rcu_dereference_rtnl(bearer_list[l_ptr->bearer_id]);
	if (!b_ptr) {
		rcu_read_unlock();
		return;
	}
	max_pkt = (b_ptr->mtu & ~3);
	rcu_read_unlock();

	if (max_pkt > MAX_MSG_SIZE)
		max_pkt = MAX_MSG_SIZE;

	l_ptr->max_pkt_target = max_pkt;
	if (l_ptr->max_pkt_target < MAX_PKT_DEFAULT)
		l_ptr->max_pkt = l_ptr->max_pkt_target;
	else
		l_ptr->max_pkt = MAX_PKT_DEFAULT;

	l_ptr->max_pkt_probes = 0;
}

static u32 link_next_sent(struct tipc_link *l_ptr)
{
	if (l_ptr->next_out)
		return buf_seqno(l_ptr->next_out);
	return mod(l_ptr->next_out_no);
}

static u32 link_last_sent(struct tipc_link *l_ptr)
{
	return mod(link_next_sent(l_ptr) - 1);
}
=======
#include "subscr.h"
#include "link.h"
#include "bcast.h"
#include "socket.h"
#include "name_distr.h"
#include "discover.h"
#include "netlink.h"
#include "monitor.h"

#include <linux/pkt_sched.h>

struct tipc_stats {
	u32 sent_pkts;
	u32 recv_pkts;
	u32 sent_states;
	u32 recv_states;
	u32 sent_probes;
	u32 recv_probes;
	u32 sent_nacks;
	u32 recv_nacks;
	u32 sent_acks;
	u32 sent_bundled;
	u32 sent_bundles;
	u32 recv_bundled;
	u32 recv_bundles;
	u32 retransmitted;
	u32 sent_fragmented;
	u32 sent_fragments;
	u32 recv_fragmented;
	u32 recv_fragments;
	u32 link_congs;		/* # port sends blocked by congestion */
	u32 deferred_recv;
	u32 duplicates;
	u32 max_queue_sz;	/* send queue size high water mark */
	u32 accu_queue_sz;	/* used for send queue size profiling */
	u32 queue_sz_counts;	/* used for send queue size profiling */
	u32 msg_length_counts;	/* used for message length profiling */
	u32 msg_lengths_total;	/* used for message length profiling */
	u32 msg_length_profile[7]; /* used for msg. length profiling */
};

/**
 * struct tipc_link - TIPC link data structure
 * @addr: network address of link's peer node
 * @name: link name character string
 * @media_addr: media address to use when sending messages over link
 * @timer: link timer
 * @net: pointer to namespace struct
 * @refcnt: reference counter for permanent references (owner node & timer)
 * @peer_session: link session # being used by peer end of link
 * @peer_bearer_id: bearer id used by link's peer endpoint
 * @bearer_id: local bearer id used by link
 * @tolerance: minimum link continuity loss needed to reset link [in ms]
 * @abort_limit: # of unacknowledged continuity probes needed to reset link
 * @state: current state of link FSM
 * @peer_caps: bitmap describing capabilities of peer node
 * @silent_intv_cnt: # of timer intervals without any reception from peer
 * @proto_msg: template for control messages generated by link
 * @pmsg: convenience pointer to "proto_msg" field
 * @priority: current link priority
 * @net_plane: current link network plane ('A' through 'H')
 * @mon_state: cookie with information needed by link monitor
 * @backlog_limit: backlog queue congestion thresholds (indexed by importance)
 * @exp_msg_count: # of tunnelled messages expected during link changeover
 * @reset_rcv_checkpt: seq # of last acknowledged message at time of link reset
 * @mtu: current maximum packet size for this link
 * @advertised_mtu: advertised own mtu when link is being established
 * @transmitq: queue for sent, non-acked messages
 * @backlogq: queue for messages waiting to be sent
 * @snt_nxt: next sequence number to use for outbound messages
 * @last_retransmitted: sequence number of most recently retransmitted message
 * @stale_count: # of identical retransmit requests made by peer
 * @ackers: # of peers that needs to ack each packet before it can be released
 * @acked: # last packet acked by a certain peer. Used for broadcast.
 * @rcv_nxt: next sequence number to expect for inbound messages
 * @deferred_queue: deferred queue saved OOS b'cast message received from node
 * @unacked_window: # of inbound messages rx'd without ack'ing back to peer
 * @inputq: buffer queue for messages to be delivered upwards
 * @namedq: buffer queue for name table messages to be delivered upwards
 * @next_out: ptr to first unsent outbound message in queue
 * @wakeupq: linked list of wakeup msgs waiting for link congestion to abate
 * @long_msg_seq_no: next identifier to use for outbound fragmented messages
 * @reasm_buf: head of partially reassembled inbound message fragments
 * @bc_rcvr: marks that this is a broadcast receiver link
 * @stats: collects statistics regarding link activity
 */
struct tipc_link {
	u32 addr;
	char name[TIPC_MAX_LINK_NAME];
	struct net *net;

	/* Management and link supervision data */
	u32 peer_session;
	u32 session;
	u32 peer_bearer_id;
	u32 bearer_id;
	u32 tolerance;
	u32 abort_limit;
	u32 state;
	u16 peer_caps;
	bool active;
	u32 silent_intv_cnt;
	char if_name[TIPC_MAX_IF_NAME];
	u32 priority;
	char net_plane;
	struct tipc_mon_state mon_state;
	u16 rst_cnt;

	/* Failover/synch */
	u16 drop_point;
	struct sk_buff *failover_reasm_skb;

	/* Max packet negotiation */
	u16 mtu;
	u16 advertised_mtu;

	/* Sending */
	struct sk_buff_head transmq;
	struct sk_buff_head backlogq;
	struct {
		u16 len;
		u16 limit;
	} backlog[5];
	u16 snd_nxt;
	u16 last_retransm;
	u16 window;
	u32 stale_count;

	/* Reception */
	u16 rcv_nxt;
	u32 rcv_unacked;
	struct sk_buff_head deferdq;
	struct sk_buff_head *inputq;
	struct sk_buff_head *namedq;

	/* Congestion handling */
	struct sk_buff_head wakeupq;

	/* Fragmentation/reassembly */
	struct sk_buff *reasm_buf;

	/* Broadcast */
	u16 ackers;
	u16 acked;
	struct tipc_link *bc_rcvlink;
	struct tipc_link *bc_sndlink;
	unsigned long prev_retr;
	u16 prev_from;
	u16 prev_to;
	u8 nack_state;
	bool bc_peer_is_up;

	/* Statistics */
	struct tipc_stats stats;
};

/*
 * Error message prefixes
 */
static const char *link_co_err = "Link tunneling error, ";
static const char *link_rst_msg = "Resetting link ";

/* Send states for broadcast NACKs
 */
enum {
	BC_NACK_SND_CONDITIONAL,
	BC_NACK_SND_UNCONDITIONAL,
	BC_NACK_SND_SUPPRESS,
};

#define TIPC_BC_RETR_LIMIT 10   /* [ms] */

/*
 * Interval between NACKs when packets arrive out of order
 */
#define TIPC_NACK_INTV (TIPC_MIN_LINK_WIN * 2)

/* Wildcard value for link session numbers. When it is known that
 * peer endpoint is down, any session number must be accepted.
 */
#define ANY_SESSION 0x10000

/* Link FSM states:
 */
enum {
	LINK_ESTABLISHED     = 0xe,
	LINK_ESTABLISHING    = 0xe  << 4,
	LINK_RESET           = 0x1  << 8,
	LINK_RESETTING       = 0x2  << 12,
	LINK_PEER_RESET      = 0xd  << 16,
	LINK_FAILINGOVER     = 0xf  << 20,
	LINK_SYNCHING        = 0xc  << 24
};

/* Link FSM state checking routines
 */
static int link_is_up(struct tipc_link *l)
{
	return l->state & (LINK_ESTABLISHED | LINK_SYNCHING);
}

static int tipc_link_proto_rcv(struct tipc_link *l, struct sk_buff *skb,
			       struct sk_buff_head *xmitq);
static void tipc_link_build_proto_msg(struct tipc_link *l, int mtyp, bool probe,
				      u16 rcvgap, int tolerance, int priority,
				      struct sk_buff_head *xmitq);
static void link_print(struct tipc_link *l, const char *str);
static int tipc_link_build_nack_msg(struct tipc_link *l,
				    struct sk_buff_head *xmitq);
static void tipc_link_build_bc_init_msg(struct tipc_link *l,
					struct sk_buff_head *xmitq);
static bool tipc_link_release_pkts(struct tipc_link *l, u16 to);
>>>>>>> v4.9.227

/*
 *  Simple non-static link routines (i.e. referenced outside this file)
 */
<<<<<<< HEAD
int tipc_link_is_up(struct tipc_link *l_ptr)
{
	if (!l_ptr)
		return 0;
	return link_working_working(l_ptr) || link_working_unknown(l_ptr);
}

int tipc_link_is_active(struct tipc_link *l_ptr)
{
	return	(l_ptr->owner->active_links[0] == l_ptr) ||
		(l_ptr->owner->active_links[1] == l_ptr);
}

/**
 * link_timeout - handle expiration of link timer
 * @l_ptr: pointer to link
 */
static void link_timeout(struct tipc_link *l_ptr)
{
	tipc_node_lock(l_ptr->owner);

	/* update counters used in statistical profiling of send traffic */
	l_ptr->stats.accu_queue_sz += l_ptr->out_queue_size;
	l_ptr->stats.queue_sz_counts++;

	if (l_ptr->first_out) {
		struct tipc_msg *msg = buf_msg(l_ptr->first_out);
		u32 length = msg_size(msg);

		if ((msg_user(msg) == MSG_FRAGMENTER) &&
		    (msg_type(msg) == FIRST_FRAGMENT)) {
			length = msg_size(msg_get_wrapped(msg));
		}
		if (length) {
			l_ptr->stats.msg_lengths_total += length;
			l_ptr->stats.msg_length_counts++;
			if (length <= 64)
				l_ptr->stats.msg_length_profile[0]++;
			else if (length <= 256)
				l_ptr->stats.msg_length_profile[1]++;
			else if (length <= 1024)
				l_ptr->stats.msg_length_profile[2]++;
			else if (length <= 4096)
				l_ptr->stats.msg_length_profile[3]++;
			else if (length <= 16384)
				l_ptr->stats.msg_length_profile[4]++;
			else if (length <= 32768)
				l_ptr->stats.msg_length_profile[5]++;
			else
				l_ptr->stats.msg_length_profile[6]++;
		}
	}

	/* do all other link processing performed on a periodic basis */

	link_state_event(l_ptr, TIMEOUT_EVT);

	if (l_ptr->next_out)
		tipc_link_push_queue(l_ptr);

	tipc_node_unlock(l_ptr->owner);
}

static void link_set_timer(struct tipc_link *l_ptr, u32 time)
{
	k_start_timer(&l_ptr->timer, time);
=======
bool tipc_link_is_up(struct tipc_link *l)
{
	return link_is_up(l);
}

bool tipc_link_peer_is_down(struct tipc_link *l)
{
	return l->state == LINK_PEER_RESET;
}

bool tipc_link_is_reset(struct tipc_link *l)
{
	return l->state & (LINK_RESET | LINK_FAILINGOVER | LINK_ESTABLISHING);
}

bool tipc_link_is_establishing(struct tipc_link *l)
{
	return l->state == LINK_ESTABLISHING;
}

bool tipc_link_is_synching(struct tipc_link *l)
{
	return l->state == LINK_SYNCHING;
}

bool tipc_link_is_failingover(struct tipc_link *l)
{
	return l->state == LINK_FAILINGOVER;
}

bool tipc_link_is_blocked(struct tipc_link *l)
{
	return l->state & (LINK_RESETTING | LINK_PEER_RESET | LINK_FAILINGOVER);
}

static bool link_is_bc_sndlink(struct tipc_link *l)
{
	return !l->bc_sndlink;
}

static bool link_is_bc_rcvlink(struct tipc_link *l)
{
	return ((l->bc_rcvlink == l) && !link_is_bc_sndlink(l));
}

int tipc_link_is_active(struct tipc_link *l)
{
	return l->active;
}

void tipc_link_set_active(struct tipc_link *l, bool active)
{
	l->active = active;
}

u32 tipc_link_id(struct tipc_link *l)
{
	return l->peer_bearer_id << 16 | l->bearer_id;
}

int tipc_link_window(struct tipc_link *l)
{
	return l->window;
}

int tipc_link_prio(struct tipc_link *l)
{
	return l->priority;
}

unsigned long tipc_link_tolerance(struct tipc_link *l)
{
	return l->tolerance;
}

struct sk_buff_head *tipc_link_inputq(struct tipc_link *l)
{
	return l->inputq;
}

char tipc_link_plane(struct tipc_link *l)
{
	return l->net_plane;
}

void tipc_link_add_bc_peer(struct tipc_link *snd_l,
			   struct tipc_link *uc_l,
			   struct sk_buff_head *xmitq)
{
	struct tipc_link *rcv_l = uc_l->bc_rcvlink;

	snd_l->ackers++;
	rcv_l->acked = snd_l->snd_nxt - 1;
	snd_l->state = LINK_ESTABLISHED;
	tipc_link_build_bc_init_msg(uc_l, xmitq);
}

void tipc_link_remove_bc_peer(struct tipc_link *snd_l,
			      struct tipc_link *rcv_l,
			      struct sk_buff_head *xmitq)
{
	u16 ack = snd_l->snd_nxt - 1;

	snd_l->ackers--;
	rcv_l->bc_peer_is_up = true;
	rcv_l->state = LINK_ESTABLISHED;
	tipc_link_bc_ack_rcv(rcv_l, ack, xmitq);
	tipc_link_reset(rcv_l);
	rcv_l->state = LINK_RESET;
	if (!snd_l->ackers) {
		tipc_link_reset(snd_l);
		snd_l->state = LINK_RESET;
		__skb_queue_purge(xmitq);
	}
}

int tipc_link_bc_peers(struct tipc_link *l)
{
	return l->ackers;
}

u16 link_bc_rcv_gap(struct tipc_link *l)
{
	struct sk_buff *skb = skb_peek(&l->deferdq);
	u16 gap = 0;

	if (more(l->snd_nxt, l->rcv_nxt))
		gap = l->snd_nxt - l->rcv_nxt;
	if (skb)
		gap = buf_seqno(skb) - l->rcv_nxt;
	return gap;
}

void tipc_link_set_mtu(struct tipc_link *l, int mtu)
{
	l->mtu = mtu;
}

int tipc_link_mtu(struct tipc_link *l)
{
	return l->mtu;
}

u16 tipc_link_rcv_nxt(struct tipc_link *l)
{
	return l->rcv_nxt;
}

u16 tipc_link_acked(struct tipc_link *l)
{
	return l->acked;
}

char *tipc_link_name(struct tipc_link *l)
{
	return l->name;
>>>>>>> v4.9.227
}

/**
 * tipc_link_create - create a new link
<<<<<<< HEAD
 * @n_ptr: pointer to associated node
 * @b_ptr: pointer to associated bearer
 * @media_addr: media address to use when sending messages over link
 *
 * Returns pointer to link.
 */
struct tipc_link *tipc_link_create(struct tipc_node *n_ptr,
				   struct tipc_bearer *b_ptr,
				   const struct tipc_media_addr *media_addr)
{
	struct tipc_link *l_ptr;
	struct tipc_msg *msg;
	char *if_name;
	char addr_string[16];
	u32 peer = n_ptr->addr;

	if (n_ptr->link_cnt >= 2) {
		tipc_addr_string_fill(addr_string, n_ptr->addr);
		pr_err("Attempt to establish third link to %s\n", addr_string);
		return NULL;
	}

	if (n_ptr->links[b_ptr->identity]) {
		tipc_addr_string_fill(addr_string, n_ptr->addr);
		pr_err("Attempt to establish second link on <%s> to %s\n",
		       b_ptr->name, addr_string);
		return NULL;
	}

	l_ptr = kzalloc(sizeof(*l_ptr), GFP_ATOMIC);
	if (!l_ptr) {
		pr_warn("Link creation failed, no memory\n");
		return NULL;
	}

	l_ptr->addr = peer;
	if_name = strchr(b_ptr->name, ':') + 1;
	sprintf(l_ptr->name, "%u.%u.%u:%s-%u.%u.%u:unknown",
		tipc_zone(tipc_own_addr), tipc_cluster(tipc_own_addr),
		tipc_node(tipc_own_addr),
		if_name,
		tipc_zone(peer), tipc_cluster(peer), tipc_node(peer));
		/* note: peer i/f name is updated by reset/activate message */
	memcpy(&l_ptr->media_addr, media_addr, sizeof(*media_addr));
	l_ptr->owner = n_ptr;
	l_ptr->checkpoint = 1;
	l_ptr->peer_session = INVALID_SESSION;
	l_ptr->bearer_id = b_ptr->identity;
	link_set_supervision_props(l_ptr, b_ptr->tolerance);
	l_ptr->state = RESET_UNKNOWN;

	l_ptr->pmsg = (struct tipc_msg *)&l_ptr->proto_msg;
	msg = l_ptr->pmsg;
	tipc_msg_init(msg, LINK_PROTOCOL, RESET_MSG, INT_H_SIZE, l_ptr->addr);
	msg_set_size(msg, sizeof(l_ptr->proto_msg));
	msg_set_session(msg, (tipc_random & 0xffff));
	msg_set_bearer_id(msg, b_ptr->identity);
	strcpy((char *)msg_data(msg), if_name);

	l_ptr->priority = b_ptr->priority;
	tipc_link_set_queue_limits(l_ptr, b_ptr->window);

	l_ptr->net_plane = b_ptr->net_plane;
	link_init_max_pkt(l_ptr);

	l_ptr->next_out_no = 1;
	__skb_queue_head_init(&l_ptr->waiting_sks);

	link_reset_statistics(l_ptr);

	tipc_node_attach_link(n_ptr, l_ptr);

	k_init_timer(&l_ptr->timer, (Handler)link_timeout,
		     (unsigned long)l_ptr);

	link_state_event(l_ptr, STARTING_EVT);

	return l_ptr;
}

void tipc_link_delete_list(unsigned int bearer_id, bool shutting_down)
{
	struct tipc_link *l_ptr;
	struct tipc_node *n_ptr;

	rcu_read_lock();
	list_for_each_entry_rcu(n_ptr, &tipc_node_list, list) {
		tipc_node_lock(n_ptr);
		l_ptr = n_ptr->links[bearer_id];
		if (l_ptr) {
			tipc_link_reset(l_ptr);
			if (shutting_down || !tipc_node_is_up(n_ptr)) {
				tipc_node_detach_link(l_ptr->owner, l_ptr);
				tipc_link_reset_fragments(l_ptr);
				tipc_node_unlock(n_ptr);

				/* Nobody else can access this link now: */
				del_timer_sync(&l_ptr->timer);
				kfree(l_ptr);
			} else {
				/* Detach/delete when failover is finished: */
				l_ptr->flags |= LINK_STOPPED;
				tipc_node_unlock(n_ptr);
				del_timer_sync(&l_ptr->timer);
			}
			continue;
		}
		tipc_node_unlock(n_ptr);
	}
	rcu_read_unlock();
}

/**
 * link_schedule_user - schedule user for wakeup after congestion
 * @link: congested link
 * @oport: sending port
 * @chain_sz: size of buffer chain that was attempted sent
 * @imp: importance of message attempted sent
 * Create pseudo msg to send back to user when congestion abates
 */
static bool link_schedule_user(struct tipc_link *link, u32 oport,
			       uint chain_sz, uint imp)
{
	struct sk_buff *buf;

	buf = tipc_msg_create(SOCK_WAKEUP, 0, INT_H_SIZE, 0, tipc_own_addr,
			      tipc_own_addr, oport, 0, 0);
	if (!buf)
		return false;
	TIPC_SKB_CB(buf)->chain_sz = chain_sz;
	TIPC_SKB_CB(buf)->chain_imp = imp;
	__skb_queue_tail(&link->waiting_sks, buf);
	link->stats.link_congs++;
=======
 * @n: pointer to associated node
 * @if_name: associated interface name
 * @bearer_id: id (index) of associated bearer
 * @tolerance: link tolerance to be used by link
 * @net_plane: network plane (A,B,c..) this link belongs to
 * @mtu: mtu to be advertised by link
 * @priority: priority to be used by link
 * @window: send window to be used by link
 * @session: session to be used by link
 * @ownnode: identity of own node
 * @peer: node id of peer node
 * @peer_caps: bitmap describing peer node capabilities
 * @bc_sndlink: the namespace global link used for broadcast sending
 * @bc_rcvlink: the peer specific link used for broadcast reception
 * @inputq: queue to put messages ready for delivery
 * @namedq: queue to put binding table update messages ready for delivery
 * @link: return value, pointer to put the created link
 *
 * Returns true if link was created, otherwise false
 */
bool tipc_link_create(struct net *net, char *if_name, int bearer_id,
		      int tolerance, char net_plane, u32 mtu, int priority,
		      int window, u32 session, u32 ownnode, u32 peer,
		      u16 peer_caps,
		      struct tipc_link *bc_sndlink,
		      struct tipc_link *bc_rcvlink,
		      struct sk_buff_head *inputq,
		      struct sk_buff_head *namedq,
		      struct tipc_link **link)
{
	struct tipc_link *l;

	l = kzalloc(sizeof(*l), GFP_ATOMIC);
	if (!l)
		return false;
	*link = l;
	l->session = session;

	/* Note: peer i/f name is completed by reset/activate message */
	sprintf(l->name, "%u.%u.%u:%s-%u.%u.%u:unknown",
		tipc_zone(ownnode), tipc_cluster(ownnode), tipc_node(ownnode),
		if_name, tipc_zone(peer), tipc_cluster(peer), tipc_node(peer));
	strcpy(l->if_name, if_name);
	l->addr = peer;
	l->peer_caps = peer_caps;
	l->net = net;
	l->peer_session = ANY_SESSION;
	l->bearer_id = bearer_id;
	l->tolerance = tolerance;
	l->net_plane = net_plane;
	l->advertised_mtu = mtu;
	l->mtu = mtu;
	l->priority = priority;
	tipc_link_set_queue_limits(l, window);
	l->ackers = 1;
	l->bc_sndlink = bc_sndlink;
	l->bc_rcvlink = bc_rcvlink;
	l->inputq = inputq;
	l->namedq = namedq;
	l->state = LINK_RESETTING;
	__skb_queue_head_init(&l->transmq);
	__skb_queue_head_init(&l->backlogq);
	__skb_queue_head_init(&l->deferdq);
	skb_queue_head_init(&l->wakeupq);
	skb_queue_head_init(l->inputq);
	return true;
}

/**
 * tipc_link_bc_create - create new link to be used for broadcast
 * @n: pointer to associated node
 * @mtu: mtu to be used
 * @window: send window to be used
 * @inputq: queue to put messages ready for delivery
 * @namedq: queue to put binding table update messages ready for delivery
 * @link: return value, pointer to put the created link
 *
 * Returns true if link was created, otherwise false
 */
bool tipc_link_bc_create(struct net *net, u32 ownnode, u32 peer,
			 int mtu, int window, u16 peer_caps,
			 struct sk_buff_head *inputq,
			 struct sk_buff_head *namedq,
			 struct tipc_link *bc_sndlink,
			 struct tipc_link **link)
{
	struct tipc_link *l;

	if (!tipc_link_create(net, "", MAX_BEARERS, 0, 'Z', mtu, 0, window,
			      0, ownnode, peer, peer_caps, bc_sndlink,
			      NULL, inputq, namedq, link))
		return false;

	l = *link;
	strcpy(l->name, tipc_bclink_name);
	tipc_link_reset(l);
	l->state = LINK_RESET;
	l->ackers = 0;
	l->bc_rcvlink = l;

	/* Broadcast send link is always up */
	if (link_is_bc_sndlink(l))
		l->state = LINK_ESTABLISHED;

>>>>>>> v4.9.227
	return true;
}

/**
<<<<<<< HEAD
=======
 * tipc_link_fsm_evt - link finite state machine
 * @l: pointer to link
 * @evt: state machine event to be processed
 */
int tipc_link_fsm_evt(struct tipc_link *l, int evt)
{
	int rc = 0;

	switch (l->state) {
	case LINK_RESETTING:
		switch (evt) {
		case LINK_PEER_RESET_EVT:
			l->state = LINK_PEER_RESET;
			break;
		case LINK_RESET_EVT:
			l->state = LINK_RESET;
			break;
		case LINK_FAILURE_EVT:
		case LINK_FAILOVER_BEGIN_EVT:
		case LINK_ESTABLISH_EVT:
		case LINK_FAILOVER_END_EVT:
		case LINK_SYNCH_BEGIN_EVT:
		case LINK_SYNCH_END_EVT:
		default:
			goto illegal_evt;
		}
		break;
	case LINK_RESET:
		switch (evt) {
		case LINK_PEER_RESET_EVT:
			l->state = LINK_ESTABLISHING;
			break;
		case LINK_FAILOVER_BEGIN_EVT:
			l->state = LINK_FAILINGOVER;
		case LINK_FAILURE_EVT:
		case LINK_RESET_EVT:
		case LINK_ESTABLISH_EVT:
		case LINK_FAILOVER_END_EVT:
			break;
		case LINK_SYNCH_BEGIN_EVT:
		case LINK_SYNCH_END_EVT:
		default:
			goto illegal_evt;
		}
		break;
	case LINK_PEER_RESET:
		switch (evt) {
		case LINK_RESET_EVT:
			l->state = LINK_ESTABLISHING;
			break;
		case LINK_PEER_RESET_EVT:
		case LINK_ESTABLISH_EVT:
		case LINK_FAILURE_EVT:
			break;
		case LINK_SYNCH_BEGIN_EVT:
		case LINK_SYNCH_END_EVT:
		case LINK_FAILOVER_BEGIN_EVT:
		case LINK_FAILOVER_END_EVT:
		default:
			goto illegal_evt;
		}
		break;
	case LINK_FAILINGOVER:
		switch (evt) {
		case LINK_FAILOVER_END_EVT:
			l->state = LINK_RESET;
			break;
		case LINK_PEER_RESET_EVT:
		case LINK_RESET_EVT:
		case LINK_ESTABLISH_EVT:
		case LINK_FAILURE_EVT:
			break;
		case LINK_FAILOVER_BEGIN_EVT:
		case LINK_SYNCH_BEGIN_EVT:
		case LINK_SYNCH_END_EVT:
		default:
			goto illegal_evt;
		}
		break;
	case LINK_ESTABLISHING:
		switch (evt) {
		case LINK_ESTABLISH_EVT:
			l->state = LINK_ESTABLISHED;
			break;
		case LINK_FAILOVER_BEGIN_EVT:
			l->state = LINK_FAILINGOVER;
			break;
		case LINK_RESET_EVT:
			l->state = LINK_RESET;
			break;
		case LINK_FAILURE_EVT:
		case LINK_PEER_RESET_EVT:
		case LINK_SYNCH_BEGIN_EVT:
		case LINK_FAILOVER_END_EVT:
			break;
		case LINK_SYNCH_END_EVT:
		default:
			goto illegal_evt;
		}
		break;
	case LINK_ESTABLISHED:
		switch (evt) {
		case LINK_PEER_RESET_EVT:
			l->state = LINK_PEER_RESET;
			rc |= TIPC_LINK_DOWN_EVT;
			break;
		case LINK_FAILURE_EVT:
			l->state = LINK_RESETTING;
			rc |= TIPC_LINK_DOWN_EVT;
			break;
		case LINK_RESET_EVT:
			l->state = LINK_RESET;
			break;
		case LINK_ESTABLISH_EVT:
		case LINK_SYNCH_END_EVT:
			break;
		case LINK_SYNCH_BEGIN_EVT:
			l->state = LINK_SYNCHING;
			break;
		case LINK_FAILOVER_BEGIN_EVT:
		case LINK_FAILOVER_END_EVT:
		default:
			goto illegal_evt;
		}
		break;
	case LINK_SYNCHING:
		switch (evt) {
		case LINK_PEER_RESET_EVT:
			l->state = LINK_PEER_RESET;
			rc |= TIPC_LINK_DOWN_EVT;
			break;
		case LINK_FAILURE_EVT:
			l->state = LINK_RESETTING;
			rc |= TIPC_LINK_DOWN_EVT;
			break;
		case LINK_RESET_EVT:
			l->state = LINK_RESET;
			break;
		case LINK_ESTABLISH_EVT:
		case LINK_SYNCH_BEGIN_EVT:
			break;
		case LINK_SYNCH_END_EVT:
			l->state = LINK_ESTABLISHED;
			break;
		case LINK_FAILOVER_BEGIN_EVT:
		case LINK_FAILOVER_END_EVT:
		default:
			goto illegal_evt;
		}
		break;
	default:
		pr_err("Unknown FSM state %x in %s\n", l->state, l->name);
	}
	return rc;
illegal_evt:
	pr_err("Illegal FSM event %x in state %x on link %s\n",
	       evt, l->state, l->name);
	return rc;
}

/* link_profile_stats - update statistical profiling of traffic
 */
static void link_profile_stats(struct tipc_link *l)
{
	struct sk_buff *skb;
	struct tipc_msg *msg;
	int length;

	/* Update counters used in statistical profiling of send traffic */
	l->stats.accu_queue_sz += skb_queue_len(&l->transmq);
	l->stats.queue_sz_counts++;

	skb = skb_peek(&l->transmq);
	if (!skb)
		return;
	msg = buf_msg(skb);
	length = msg_size(msg);

	if (msg_user(msg) == MSG_FRAGMENTER) {
		if (msg_type(msg) != FIRST_FRAGMENT)
			return;
		length = msg_size(msg_get_wrapped(msg));
	}
	l->stats.msg_lengths_total += length;
	l->stats.msg_length_counts++;
	if (length <= 64)
		l->stats.msg_length_profile[0]++;
	else if (length <= 256)
		l->stats.msg_length_profile[1]++;
	else if (length <= 1024)
		l->stats.msg_length_profile[2]++;
	else if (length <= 4096)
		l->stats.msg_length_profile[3]++;
	else if (length <= 16384)
		l->stats.msg_length_profile[4]++;
	else if (length <= 32768)
		l->stats.msg_length_profile[5]++;
	else
		l->stats.msg_length_profile[6]++;
}

/* tipc_link_timeout - perform periodic task as instructed from node timeout
 */
int tipc_link_timeout(struct tipc_link *l, struct sk_buff_head *xmitq)
{
	int mtyp = 0;
	int rc = 0;
	bool state = false;
	bool probe = false;
	bool setup = false;
	u16 bc_snt = l->bc_sndlink->snd_nxt - 1;
	u16 bc_acked = l->bc_rcvlink->acked;
	struct tipc_mon_state *mstate = &l->mon_state;

	switch (l->state) {
	case LINK_ESTABLISHED:
	case LINK_SYNCHING:
		mtyp = STATE_MSG;
		link_profile_stats(l);
		tipc_mon_get_state(l->net, l->addr, mstate, l->bearer_id);
		if (mstate->reset || (l->silent_intv_cnt > l->abort_limit))
			return tipc_link_fsm_evt(l, LINK_FAILURE_EVT);
		state = bc_acked != bc_snt;
		state |= l->bc_rcvlink->rcv_unacked;
		state |= l->rcv_unacked;
		state |= !skb_queue_empty(&l->transmq);
		state |= !skb_queue_empty(&l->deferdq);
		probe = mstate->probing;
		probe |= l->silent_intv_cnt;
		if (probe || mstate->monitoring)
			l->silent_intv_cnt++;
		break;
	case LINK_RESET:
		setup = l->rst_cnt++ <= 4;
		setup |= !(l->rst_cnt % 16);
		mtyp = RESET_MSG;
		break;
	case LINK_ESTABLISHING:
		setup = true;
		mtyp = ACTIVATE_MSG;
		break;
	case LINK_PEER_RESET:
	case LINK_RESETTING:
	case LINK_FAILINGOVER:
		break;
	default:
		break;
	}

	if (state || probe || setup)
		tipc_link_build_proto_msg(l, mtyp, probe, 0, 0, 0, xmitq);

	return rc;
}

/**
 * link_schedule_user - schedule a message sender for wakeup after congestion
 * @link: congested link
 * @list: message that was attempted sent
 * Create pseudo msg to send back to user when congestion abates
 * Does not consume buffer list
 */
static int link_schedule_user(struct tipc_link *link, struct sk_buff_head *list)
{
	struct tipc_msg *msg = buf_msg(skb_peek(list));
	int imp = msg_importance(msg);
	u32 oport = msg_origport(msg);
	u32 addr = tipc_own_addr(link->net);
	struct sk_buff *skb;

	/* This really cannot happen...  */
	if (unlikely(imp > TIPC_CRITICAL_IMPORTANCE)) {
		pr_warn("%s<%s>, send queue full", link_rst_msg, link->name);
		return -ENOBUFS;
	}
	/* Non-blocking sender: */
	if (TIPC_SKB_CB(skb_peek(list))->wakeup_pending)
		return -ELINKCONG;

	/* Create and schedule wakeup pseudo message */
	skb = tipc_msg_create(SOCK_WAKEUP, 0, INT_H_SIZE, 0,
			      addr, addr, oport, 0, 0);
	if (!skb)
		return -ENOBUFS;
	TIPC_SKB_CB(skb)->chain_sz = skb_queue_len(list);
	TIPC_SKB_CB(skb)->chain_imp = imp;
	skb_queue_tail(&link->wakeupq, skb);
	link->stats.link_congs++;
	return -ELINKCONG;
}

/**
>>>>>>> v4.9.227
 * link_prepare_wakeup - prepare users for wakeup after congestion
 * @link: congested link
 * Move a number of waiting users, as permitted by available space in
 * the send queue, from link wait queue to node wait queue for wakeup
 */
<<<<<<< HEAD
static void link_prepare_wakeup(struct tipc_link *link)
{
	struct sk_buff_head *wq = &link->waiting_sks;
	struct sk_buff *buf;
	uint pend_qsz = link->out_queue_size;

	for (buf = skb_peek(wq); buf; buf = skb_peek(wq)) {
		if (pend_qsz >= link->queue_limit[TIPC_SKB_CB(buf)->chain_imp])
			break;
		pend_qsz += TIPC_SKB_CB(buf)->chain_sz;
		__skb_queue_tail(&link->owner->waiting_sks, __skb_dequeue(wq));
	}
}

/**
 * link_release_outqueue - purge link's outbound message queue
 * @l_ptr: pointer to link
 */
static void link_release_outqueue(struct tipc_link *l_ptr)
{
	kfree_skb_list(l_ptr->first_out);
	l_ptr->first_out = NULL;
	l_ptr->out_queue_size = 0;
}

/**
 * tipc_link_reset_fragments - purge link's inbound message fragments queue
 * @l_ptr: pointer to link
 */
void tipc_link_reset_fragments(struct tipc_link *l_ptr)
{
	kfree_skb(l_ptr->reasm_buf);
	l_ptr->reasm_buf = NULL;
}

/**
 * tipc_link_purge_queues - purge all pkt queues associated with link
 * @l_ptr: pointer to link
 */
void tipc_link_purge_queues(struct tipc_link *l_ptr)
{
	kfree_skb_list(l_ptr->oldest_deferred_in);
	kfree_skb_list(l_ptr->first_out);
	tipc_link_reset_fragments(l_ptr);
	kfree_skb(l_ptr->proto_msg_queue);
	l_ptr->proto_msg_queue = NULL;
}

void tipc_link_reset(struct tipc_link *l_ptr)
{
	u32 prev_state = l_ptr->state;
	u32 checkpoint = l_ptr->next_in_no;
	int was_active_link = tipc_link_is_active(l_ptr);
	struct tipc_node *owner = l_ptr->owner;

	msg_set_session(l_ptr->pmsg, ((msg_session(l_ptr->pmsg) + 1) & 0xffff));

	/* Link is down, accept any session */
	l_ptr->peer_session = INVALID_SESSION;

	/* Prepare for max packet size negotiation */
	link_init_max_pkt(l_ptr);

	l_ptr->state = RESET_UNKNOWN;

	if ((prev_state == RESET_UNKNOWN) || (prev_state == RESET_RESET))
		return;

	tipc_node_link_down(l_ptr->owner, l_ptr);
	tipc_bearer_remove_dest(l_ptr->bearer_id, l_ptr->addr);

	if (was_active_link && tipc_node_active_links(l_ptr->owner)) {
		l_ptr->reset_checkpoint = checkpoint;
		l_ptr->exp_msg_count = START_CHANGEOVER;
	}

	/* Clean up all queues: */
	link_release_outqueue(l_ptr);
	kfree_skb(l_ptr->proto_msg_queue);
	l_ptr->proto_msg_queue = NULL;
	kfree_skb_list(l_ptr->oldest_deferred_in);
	if (!skb_queue_empty(&l_ptr->waiting_sks)) {
		skb_queue_splice_init(&l_ptr->waiting_sks, &owner->waiting_sks);
		owner->action_flags |= TIPC_WAKEUP_USERS;
	}
	l_ptr->retransm_queue_head = 0;
	l_ptr->retransm_queue_size = 0;
	l_ptr->last_out = NULL;
	l_ptr->first_out = NULL;
	l_ptr->next_out = NULL;
	l_ptr->unacked_window = 0;
	l_ptr->checkpoint = 1;
	l_ptr->next_out_no = 1;
	l_ptr->deferred_inqueue_sz = 0;
	l_ptr->oldest_deferred_in = NULL;
	l_ptr->newest_deferred_in = NULL;
	l_ptr->fsm_msg_cnt = 0;
	l_ptr->stale_count = 0;
	link_reset_statistics(l_ptr);
}

void tipc_link_reset_list(unsigned int bearer_id)
{
	struct tipc_link *l_ptr;
	struct tipc_node *n_ptr;

	rcu_read_lock();
	list_for_each_entry_rcu(n_ptr, &tipc_node_list, list) {
		tipc_node_lock(n_ptr);
		l_ptr = n_ptr->links[bearer_id];
		if (l_ptr)
			tipc_link_reset(l_ptr);
		tipc_node_unlock(n_ptr);
	}
	rcu_read_unlock();
}

static void link_activate(struct tipc_link *l_ptr)
{
	l_ptr->next_in_no = l_ptr->stats.recv_info = 1;
	tipc_node_link_up(l_ptr->owner, l_ptr);
	tipc_bearer_add_dest(l_ptr->bearer_id, l_ptr->addr);
}

/**
 * link_state_event - link finite state machine
 * @l_ptr: pointer to link
 * @event: state machine event to process
 */
static void link_state_event(struct tipc_link *l_ptr, unsigned int event)
{
	struct tipc_link *other;
	u32 cont_intv = l_ptr->continuity_interval;

	if (l_ptr->flags & LINK_STOPPED)
		return;

	if (!(l_ptr->flags & LINK_STARTED) && (event != STARTING_EVT))
		return;		/* Not yet. */

	/* Check whether changeover is going on */
	if (l_ptr->exp_msg_count) {
		if (event == TIMEOUT_EVT)
			link_set_timer(l_ptr, cont_intv);
		return;
	}

	switch (l_ptr->state) {
	case WORKING_WORKING:
		switch (event) {
		case TRAFFIC_MSG_EVT:
		case ACTIVATE_MSG:
			break;
		case TIMEOUT_EVT:
			if (l_ptr->next_in_no != l_ptr->checkpoint) {
				l_ptr->checkpoint = l_ptr->next_in_no;
				if (tipc_bclink_acks_missing(l_ptr->owner)) {
					tipc_link_proto_xmit(l_ptr, STATE_MSG,
							     0, 0, 0, 0, 0);
					l_ptr->fsm_msg_cnt++;
				} else if (l_ptr->max_pkt < l_ptr->max_pkt_target) {
					tipc_link_proto_xmit(l_ptr, STATE_MSG,
							     1, 0, 0, 0, 0);
					l_ptr->fsm_msg_cnt++;
				}
				link_set_timer(l_ptr, cont_intv);
				break;
			}
			l_ptr->state = WORKING_UNKNOWN;
			l_ptr->fsm_msg_cnt = 0;
			tipc_link_proto_xmit(l_ptr, STATE_MSG, 1, 0, 0, 0, 0);
			l_ptr->fsm_msg_cnt++;
			link_set_timer(l_ptr, cont_intv / 4);
			break;
		case RESET_MSG:
			pr_info("%s<%s>, requested by peer\n", link_rst_msg,
				l_ptr->name);
			tipc_link_reset(l_ptr);
			l_ptr->state = RESET_RESET;
			l_ptr->fsm_msg_cnt = 0;
			tipc_link_proto_xmit(l_ptr, ACTIVATE_MSG,
					     0, 0, 0, 0, 0);
			l_ptr->fsm_msg_cnt++;
			link_set_timer(l_ptr, cont_intv);
			break;
		default:
			pr_err("%s%u in WW state\n", link_unk_evt, event);
		}
		break;
	case WORKING_UNKNOWN:
		switch (event) {
		case TRAFFIC_MSG_EVT:
		case ACTIVATE_MSG:
			l_ptr->state = WORKING_WORKING;
			l_ptr->fsm_msg_cnt = 0;
			link_set_timer(l_ptr, cont_intv);
			break;
		case RESET_MSG:
			pr_info("%s<%s>, requested by peer while probing\n",
				link_rst_msg, l_ptr->name);
			tipc_link_reset(l_ptr);
			l_ptr->state = RESET_RESET;
			l_ptr->fsm_msg_cnt = 0;
			tipc_link_proto_xmit(l_ptr, ACTIVATE_MSG,
					     0, 0, 0, 0, 0);
			l_ptr->fsm_msg_cnt++;
			link_set_timer(l_ptr, cont_intv);
			break;
		case TIMEOUT_EVT:
			if (l_ptr->next_in_no != l_ptr->checkpoint) {
				l_ptr->state = WORKING_WORKING;
				l_ptr->fsm_msg_cnt = 0;
				l_ptr->checkpoint = l_ptr->next_in_no;
				if (tipc_bclink_acks_missing(l_ptr->owner)) {
					tipc_link_proto_xmit(l_ptr, STATE_MSG,
							     0, 0, 0, 0, 0);
					l_ptr->fsm_msg_cnt++;
				}
				link_set_timer(l_ptr, cont_intv);
			} else if (l_ptr->fsm_msg_cnt < l_ptr->abort_limit) {
				tipc_link_proto_xmit(l_ptr, STATE_MSG,
						     1, 0, 0, 0, 0);
				l_ptr->fsm_msg_cnt++;
				link_set_timer(l_ptr, cont_intv / 4);
			} else {	/* Link has failed */
				pr_warn("%s<%s>, peer not responding\n",
					link_rst_msg, l_ptr->name);
				tipc_link_reset(l_ptr);
				l_ptr->state = RESET_UNKNOWN;
				l_ptr->fsm_msg_cnt = 0;
				tipc_link_proto_xmit(l_ptr, RESET_MSG,
						     0, 0, 0, 0, 0);
				l_ptr->fsm_msg_cnt++;
				link_set_timer(l_ptr, cont_intv);
			}
			break;
		default:
			pr_err("%s%u in WU state\n", link_unk_evt, event);
		}
		break;
	case RESET_UNKNOWN:
		switch (event) {
		case TRAFFIC_MSG_EVT:
			break;
		case ACTIVATE_MSG:
			other = l_ptr->owner->active_links[0];
			if (other && link_working_unknown(other))
				break;
			l_ptr->state = WORKING_WORKING;
			l_ptr->fsm_msg_cnt = 0;
			link_activate(l_ptr);
			tipc_link_proto_xmit(l_ptr, STATE_MSG, 1, 0, 0, 0, 0);
			l_ptr->fsm_msg_cnt++;
			if (l_ptr->owner->working_links == 1)
				tipc_link_sync_xmit(l_ptr);
			link_set_timer(l_ptr, cont_intv);
			break;
		case RESET_MSG:
			l_ptr->state = RESET_RESET;
			l_ptr->fsm_msg_cnt = 0;
			tipc_link_proto_xmit(l_ptr, ACTIVATE_MSG,
					     1, 0, 0, 0, 0);
			l_ptr->fsm_msg_cnt++;
			link_set_timer(l_ptr, cont_intv);
			break;
		case STARTING_EVT:
			l_ptr->flags |= LINK_STARTED;
			/* fall through */
		case TIMEOUT_EVT:
			tipc_link_proto_xmit(l_ptr, RESET_MSG, 0, 0, 0, 0, 0);
			l_ptr->fsm_msg_cnt++;
			link_set_timer(l_ptr, cont_intv);
			break;
		default:
			pr_err("%s%u in RU state\n", link_unk_evt, event);
		}
		break;
	case RESET_RESET:
		switch (event) {
		case TRAFFIC_MSG_EVT:
		case ACTIVATE_MSG:
			other = l_ptr->owner->active_links[0];
			if (other && link_working_unknown(other))
				break;
			l_ptr->state = WORKING_WORKING;
			l_ptr->fsm_msg_cnt = 0;
			link_activate(l_ptr);
			tipc_link_proto_xmit(l_ptr, STATE_MSG, 1, 0, 0, 0, 0);
			l_ptr->fsm_msg_cnt++;
			if (l_ptr->owner->working_links == 1)
				tipc_link_sync_xmit(l_ptr);
			link_set_timer(l_ptr, cont_intv);
			break;
		case RESET_MSG:
			break;
		case TIMEOUT_EVT:
			tipc_link_proto_xmit(l_ptr, ACTIVATE_MSG,
					     0, 0, 0, 0, 0);
			l_ptr->fsm_msg_cnt++;
			link_set_timer(l_ptr, cont_intv);
			break;
		default:
			pr_err("%s%u in RR state\n", link_unk_evt, event);
		}
		break;
	default:
		pr_err("Unknown link state %u/%u\n", l_ptr->state, event);
	}
}

/* tipc_link_cong: determine return value and how to treat the
 * sent buffer during link congestion.
 * - For plain, errorless user data messages we keep the buffer and
 *   return -ELINKONG.
 * - For all other messages we discard the buffer and return -EHOSTUNREACH
 * - For TIPC internal messages we also reset the link
 */
static int tipc_link_cong(struct tipc_link *link, struct sk_buff *buf)
{
	struct tipc_msg *msg = buf_msg(buf);
	uint imp = tipc_msg_tot_importance(msg);
	u32 oport = msg_tot_origport(msg);

	if (unlikely(imp > TIPC_CRITICAL_IMPORTANCE)) {
		pr_warn("%s<%s>, send queue full", link_rst_msg, link->name);
		tipc_link_reset(link);
		goto drop;
	}
	if (unlikely(msg_errcode(msg)))
		goto drop;
	if (unlikely(msg_reroute_cnt(msg)))
		goto drop;
	if (TIPC_SKB_CB(buf)->wakeup_pending)
		return -ELINKCONG;
	if (link_schedule_user(link, oport, TIPC_SKB_CB(buf)->chain_sz, imp))
		return -ELINKCONG;
drop:
	kfree_skb_list(buf);
	return -EHOSTUNREACH;
}

/**
 * __tipc_link_xmit(): same as tipc_link_xmit, but destlink is known & locked
 * @link: link to use
 * @buf: chain of buffers containing message
 * Consumes the buffer chain, except when returning -ELINKCONG
 * Returns 0 if success, otherwise errno: -ELINKCONG, -EMSGSIZE (plain socket
 * user data messages) or -EHOSTUNREACH (all other messages/senders)
 * Only the socket functions tipc_send_stream() and tipc_send_packet() need
 * to act on the return value, since they may need to do more send attempts.
 */
int __tipc_link_xmit(struct tipc_link *link, struct sk_buff *buf)
{
	struct tipc_msg *msg = buf_msg(buf);
	uint psz = msg_size(msg);
	uint qsz = link->out_queue_size;
	uint sndlim = link->queue_limit[0];
	uint imp = tipc_msg_tot_importance(msg);
	uint mtu = link->max_pkt;
	uint ack = mod(link->next_in_no - 1);
	uint seqno = link->next_out_no;
	uint bc_last_in = link->owner->bclink.last_in;
	struct tipc_media_addr *addr = &link->media_addr;
	struct sk_buff *next = buf->next;

	/* Match queue limits against msg importance: */
	if (unlikely(qsz >= link->queue_limit[imp]))
		return tipc_link_cong(link, buf);

	/* Has valid packet limit been used ? */
	if (unlikely(psz > mtu)) {
		kfree_skb_list(buf);
		return -EMSGSIZE;
	}

	/* Prepare each packet for sending, and add to outqueue: */
	while (buf) {
		next = buf->next;
		msg = buf_msg(buf);
		msg_set_word(msg, 2, ((ack << 16) | mod(seqno)));
		msg_set_bcast_ack(msg, bc_last_in);

		if (!link->first_out) {
			link->first_out = buf;
		} else if (qsz < sndlim) {
			link->last_out->next = buf;
		} else if (tipc_msg_bundle(link->last_out, buf, mtu)) {
			link->stats.sent_bundled++;
			buf = next;
			next = buf->next;
			continue;
		} else if (tipc_msg_make_bundle(&buf, mtu, link->addr)) {
			link->stats.sent_bundled++;
			link->stats.sent_bundles++;
			link->last_out->next = buf;
			if (!link->next_out)
				link->next_out = buf;
		} else {
			link->last_out->next = buf;
			if (!link->next_out)
				link->next_out = buf;
		}

		/* Send packet if possible: */
		if (likely(++qsz <= sndlim)) {
			tipc_bearer_send(link->bearer_id, buf, addr);
			link->next_out = next;
			link->unacked_window = 0;
		}
		seqno++;
		link->last_out = buf;
		buf = next;
	}
	link->next_out_no = seqno;
	link->out_queue_size = qsz;
	return 0;
}

/**
 * tipc_link_xmit() is the general link level function for message sending
 * @buf: chain of buffers containing message
 * @dsz: amount of user data to be sent
 * @dnode: address of destination node
 * @selector: a number used for deterministic link selection
 * Consumes the buffer chain, except when returning -ELINKCONG
 * Returns 0 if success, otherwise errno: -ELINKCONG,-EHOSTUNREACH,-EMSGSIZE
 */
int tipc_link_xmit(struct sk_buff *buf, u32 dnode, u32 selector)
{
	struct tipc_link *link = NULL;
	struct tipc_node *node;
	int rc = -EHOSTUNREACH;

	node = tipc_node_find(dnode);
	if (node) {
		tipc_node_lock(node);
		link = node->active_links[selector & 1];
		if (link)
			rc = __tipc_link_xmit(link, buf);
		tipc_node_unlock(node);
	}

	if (link)
		return rc;

	if (likely(in_own_node(dnode)))
		return tipc_sk_rcv(buf);

	kfree_skb_list(buf);
	return rc;
}

/*
 * tipc_link_sync_xmit - synchronize broadcast link endpoints.
 *
 * Give a newly added peer node the sequence number where it should
 * start receiving and acking broadcast packets.
 *
 * Called with node locked
 */
static void tipc_link_sync_xmit(struct tipc_link *link)
{
	struct sk_buff *buf;
	struct tipc_msg *msg;

	buf = tipc_buf_acquire(INT_H_SIZE);
	if (!buf)
		return;

	msg = buf_msg(buf);
	tipc_msg_init(msg, BCAST_PROTOCOL, STATE_MSG, INT_H_SIZE, link->addr);
	msg_set_last_bcast(msg, link->owner->bclink.acked);
	__tipc_link_xmit(link, buf);
}

/*
 * tipc_link_sync_rcv - synchronize broadcast link endpoints.
 * Receive the sequence number where we should start receiving and
 * acking broadcast packets from a newly added peer node, and open
 * up for reception of such packets.
 *
 * Called with node locked
 */
static void tipc_link_sync_rcv(struct tipc_node *n, struct sk_buff *buf)
{
	struct tipc_msg *msg = buf_msg(buf);

	n->bclink.last_sent = n->bclink.last_in = msg_last_bcast(msg);
	n->bclink.recv_permitted = true;
	kfree_skb(buf);
}

/*
 * tipc_link_push_packet: Push one unsent packet to the media
 */
static u32 tipc_link_push_packet(struct tipc_link *l_ptr)
{
	struct sk_buff *buf = l_ptr->first_out;
	u32 r_q_size = l_ptr->retransm_queue_size;
	u32 r_q_head = l_ptr->retransm_queue_head;

	/* Step to position where retransmission failed, if any,    */
	/* consider that buffers may have been released in meantime */
	if (r_q_size && buf) {
		u32 last = lesser(mod(r_q_head + r_q_size),
				  link_last_sent(l_ptr));
		u32 first = buf_seqno(buf);

		while (buf && less(first, r_q_head)) {
			first = mod(first + 1);
			buf = buf->next;
		}
		l_ptr->retransm_queue_head = r_q_head = first;
		l_ptr->retransm_queue_size = r_q_size = mod(last - first);
	}

	/* Continue retransmission now, if there is anything: */
	if (r_q_size && buf) {
		msg_set_ack(buf_msg(buf), mod(l_ptr->next_in_no - 1));
		msg_set_bcast_ack(buf_msg(buf), l_ptr->owner->bclink.last_in);
		tipc_bearer_send(l_ptr->bearer_id, buf, &l_ptr->media_addr);
		l_ptr->retransm_queue_head = mod(++r_q_head);
		l_ptr->retransm_queue_size = --r_q_size;
		l_ptr->stats.retransmitted++;
		return 0;
	}

	/* Send deferred protocol message, if any: */
	buf = l_ptr->proto_msg_queue;
	if (buf) {
		msg_set_ack(buf_msg(buf), mod(l_ptr->next_in_no - 1));
		msg_set_bcast_ack(buf_msg(buf), l_ptr->owner->bclink.last_in);
		tipc_bearer_send(l_ptr->bearer_id, buf, &l_ptr->media_addr);
		l_ptr->unacked_window = 0;
		kfree_skb(buf);
		l_ptr->proto_msg_queue = NULL;
		return 0;
	}

	/* Send one deferred data message, if send window not full: */
	buf = l_ptr->next_out;
	if (buf) {
		struct tipc_msg *msg = buf_msg(buf);
		u32 next = msg_seqno(msg);
		u32 first = buf_seqno(l_ptr->first_out);

		if (mod(next - first) < l_ptr->queue_limit[0]) {
			msg_set_ack(msg, mod(l_ptr->next_in_no - 1));
			msg_set_bcast_ack(msg, l_ptr->owner->bclink.last_in);
			tipc_bearer_send(l_ptr->bearer_id, buf,
					 &l_ptr->media_addr);
			if (msg_user(msg) == MSG_BUNDLER)
				msg_set_type(msg, BUNDLE_CLOSED);
			l_ptr->next_out = buf->next;
			return 0;
		}
	}
	return 1;
}

/*
 * push_queue(): push out the unsent messages of a link where
 *               congestion has abated. Node is locked
 */
void tipc_link_push_queue(struct tipc_link *l_ptr)
{
	u32 res;

	do {
		res = tipc_link_push_packet(l_ptr);
	} while (!res);
}

void tipc_link_reset_all(struct tipc_node *node)
{
	char addr_string[16];
	u32 i;

	tipc_node_lock(node);

	pr_warn("Resetting all links to %s\n",
		tipc_addr_string_fill(addr_string, node->addr));

	for (i = 0; i < MAX_BEARERS; i++) {
		if (node->links[i]) {
			link_print(node->links[i], "Resetting link\n");
			tipc_link_reset(node->links[i]);
		}
	}

	tipc_node_unlock(node);
}

static void link_retransmit_failure(struct tipc_link *l_ptr,
				    struct sk_buff *buf)
{
	struct tipc_msg *msg = buf_msg(buf);

	pr_warn("Retransmission failure on link <%s>\n", l_ptr->name);

	if (l_ptr->addr) {
		/* Handle failure on standard link */
		link_print(l_ptr, "Resetting link\n");
		tipc_link_reset(l_ptr);

	} else {
		/* Handle failure on broadcast link */
		struct tipc_node *n_ptr;
		char addr_string[16];

		pr_info("Msg seq number: %u,  ", msg_seqno(msg));
		pr_cont("Outstanding acks: %lu\n",
			(unsigned long) TIPC_SKB_CB(buf)->handle);

		n_ptr = tipc_bclink_retransmit_to();
		tipc_node_lock(n_ptr);

		tipc_addr_string_fill(addr_string, n_ptr->addr);
		pr_info("Broadcast link info for %s\n", addr_string);
		pr_info("Reception permitted: %d,  Acked: %u\n",
			n_ptr->bclink.recv_permitted,
			n_ptr->bclink.acked);
		pr_info("Last in: %u,  Oos state: %u,  Last sent: %u\n",
			n_ptr->bclink.last_in,
			n_ptr->bclink.oos_state,
			n_ptr->bclink.last_sent);

		tipc_node_unlock(n_ptr);

		tipc_bclink_set_flags(TIPC_BCLINK_RESET);
		l_ptr->stale_count = 0;
	}
}

void tipc_link_retransmit(struct tipc_link *l_ptr, struct sk_buff *buf,
			  u32 retransmits)
{
	struct tipc_msg *msg;

	if (!buf)
		return;

	msg = buf_msg(buf);

	/* Detect repeated retransmit failures */
	if (l_ptr->last_retransmitted == msg_seqno(msg)) {
		if (++l_ptr->stale_count > 100) {
			link_retransmit_failure(l_ptr, buf);
			return;
		}
	} else {
		l_ptr->last_retransmitted = msg_seqno(msg);
		l_ptr->stale_count = 1;
	}

	while (retransmits && (buf != l_ptr->next_out) && buf) {
		msg = buf_msg(buf);
		msg_set_ack(msg, mod(l_ptr->next_in_no - 1));
		msg_set_bcast_ack(msg, l_ptr->owner->bclink.last_in);
		tipc_bearer_send(l_ptr->bearer_id, buf, &l_ptr->media_addr);
		buf = buf->next;
		retransmits--;
		l_ptr->stats.retransmitted++;
	}

	l_ptr->retransm_queue_head = l_ptr->retransm_queue_size = 0;
}

/**
 * link_insert_deferred_queue - insert deferred messages back into receive chain
 */
static struct sk_buff *link_insert_deferred_queue(struct tipc_link *l_ptr,
						  struct sk_buff *buf)
{
	u32 seq_no;

	if (l_ptr->oldest_deferred_in == NULL)
		return buf;

	seq_no = buf_seqno(l_ptr->oldest_deferred_in);
	if (seq_no == mod(l_ptr->next_in_no)) {
		l_ptr->newest_deferred_in->next = buf;
		buf = l_ptr->oldest_deferred_in;
		l_ptr->oldest_deferred_in = NULL;
		l_ptr->deferred_inqueue_sz = 0;
	}
	return buf;
}

/**
 * link_recv_buf_validate - validate basic format of received message
 *
 * This routine ensures a TIPC message has an acceptable header, and at least
 * as much data as the header indicates it should.  The routine also ensures
 * that the entire message header is stored in the main fragment of the message
 * buffer, to simplify future access to message header fields.
 *
 * Note: Having extra info present in the message header or data areas is OK.
 * TIPC will ignore the excess, under the assumption that it is optional info
 * introduced by a later release of the protocol.
 */
static int link_recv_buf_validate(struct sk_buff *buf)
{
	static u32 min_data_hdr_size[8] = {
		SHORT_H_SIZE, MCAST_H_SIZE, NAMED_H_SIZE, BASIC_H_SIZE,
		MAX_H_SIZE, MAX_H_SIZE, MAX_H_SIZE, MAX_H_SIZE
		};

	struct tipc_msg *msg;
	u32 tipc_hdr[2];
	u32 size;
	u32 hdr_size;
	u32 min_hdr_size;

	/* If this packet comes from the defer queue, the skb has already
	 * been validated
	 */
	if (unlikely(TIPC_SKB_CB(buf)->deferred))
		return 1;

	if (unlikely(buf->len < MIN_H_SIZE))
		return 0;

	msg = skb_header_pointer(buf, 0, sizeof(tipc_hdr), tipc_hdr);
	if (msg == NULL)
		return 0;

	if (unlikely(msg_version(msg) != TIPC_VERSION))
		return 0;

	size = msg_size(msg);
	hdr_size = msg_hdr_sz(msg);
	min_hdr_size = msg_isdata(msg) ?
		min_data_hdr_size[msg_type(msg)] : INT_H_SIZE;

	if (unlikely((hdr_size < min_hdr_size) ||
		     (size < hdr_size) ||
		     (buf->len < size) ||
		     (size - hdr_size > TIPC_MAX_USER_MSG_SIZE)))
		return 0;

	return pskb_may_pull(buf, hdr_size);
}

/**
 * tipc_rcv - process TIPC packets/messages arriving from off-node
 * @head: pointer to message buffer chain
 * @b_ptr: pointer to bearer message arrived on
 *
 * Invoked with no locks held.  Bearer pointer must point to a valid bearer
 * structure (i.e. cannot be NULL), but bearer can be inactive.
 */
void tipc_rcv(struct sk_buff *head, struct tipc_bearer *b_ptr)
{
	while (head) {
		struct tipc_node *n_ptr;
		struct tipc_link *l_ptr;
		struct sk_buff *crs;
		struct sk_buff *buf = head;
		struct tipc_msg *msg;
		u32 seq_no;
		u32 ackd;
		u32 released = 0;

		head = head->next;
		buf->next = NULL;

		/* Ensure message is well-formed */
		if (unlikely(!link_recv_buf_validate(buf)))
			goto discard;

		/* Ensure message data is a single contiguous unit */
		if (unlikely(skb_linearize(buf)))
			goto discard;

		/* Handle arrival of a non-unicast link message */
		msg = buf_msg(buf);

		if (unlikely(msg_non_seq(msg))) {
			if (msg_user(msg) ==  LINK_CONFIG)
				tipc_disc_rcv(buf, b_ptr);
			else
				tipc_bclink_rcv(buf);
			continue;
		}

		/* Discard unicast link messages destined for another node */
		if (unlikely(!msg_short(msg) &&
			     (msg_destnode(msg) != tipc_own_addr)))
			goto discard;

		/* Locate neighboring node that sent message */
		n_ptr = tipc_node_find(msg_prevnode(msg));
		if (unlikely(!n_ptr))
			goto discard;
		tipc_node_lock(n_ptr);

		/* Locate unicast link endpoint that should handle message */
		l_ptr = n_ptr->links[b_ptr->identity];
		if (unlikely(!l_ptr))
			goto unlock_discard;

		/* Verify that communication with node is currently allowed */
		if ((n_ptr->action_flags & TIPC_WAIT_PEER_LINKS_DOWN) &&
		    msg_user(msg) == LINK_PROTOCOL &&
		    (msg_type(msg) == RESET_MSG ||
		    msg_type(msg) == ACTIVATE_MSG) &&
		    !msg_redundant_link(msg))
			n_ptr->action_flags &= ~TIPC_WAIT_PEER_LINKS_DOWN;

		if (tipc_node_blocked(n_ptr))
			goto unlock_discard;

		/* Validate message sequence number info */
		seq_no = msg_seqno(msg);
		ackd = msg_ack(msg);

		/* Release acked messages */
		if (n_ptr->bclink.recv_permitted)
			tipc_bclink_acknowledge(n_ptr, msg_bcast_ack(msg));

		crs = l_ptr->first_out;
		while ((crs != l_ptr->next_out) &&
		       less_eq(buf_seqno(crs), ackd)) {
			struct sk_buff *next = crs->next;
			kfree_skb(crs);
			crs = next;
			released++;
		}
		if (released) {
			l_ptr->first_out = crs;
			l_ptr->out_queue_size -= released;
		}

		/* Try sending any messages link endpoint has pending */
		if (unlikely(l_ptr->next_out))
			tipc_link_push_queue(l_ptr);

		if (released && !skb_queue_empty(&l_ptr->waiting_sks)) {
			link_prepare_wakeup(l_ptr);
			l_ptr->owner->action_flags |= TIPC_WAKEUP_USERS;
		}

		/* Process the incoming packet */
		if (unlikely(!link_working_working(l_ptr))) {
			if (msg_user(msg) == LINK_PROTOCOL) {
				tipc_link_proto_rcv(l_ptr, buf);
				head = link_insert_deferred_queue(l_ptr, head);
				tipc_node_unlock(n_ptr);
				continue;
			}

			/* Traffic message. Conditionally activate link */
			link_state_event(l_ptr, TRAFFIC_MSG_EVT);

			if (link_working_working(l_ptr)) {
				/* Re-insert buffer in front of queue */
				buf->next = head;
				head = buf;
				tipc_node_unlock(n_ptr);
				continue;
			}
			goto unlock_discard;
		}

		/* Link is now in state WORKING_WORKING */
		if (unlikely(seq_no != mod(l_ptr->next_in_no))) {
			link_handle_out_of_seq_msg(l_ptr, buf);
			head = link_insert_deferred_queue(l_ptr, head);
			tipc_node_unlock(n_ptr);
			continue;
		}
		l_ptr->next_in_no++;
		if (unlikely(l_ptr->oldest_deferred_in))
			head = link_insert_deferred_queue(l_ptr, head);

		if (unlikely(++l_ptr->unacked_window >= TIPC_MIN_LINK_WIN)) {
			l_ptr->stats.sent_acks++;
			tipc_link_proto_xmit(l_ptr, STATE_MSG, 0, 0, 0, 0, 0);
		}

		if (tipc_link_prepare_input(l_ptr, &buf)) {
			tipc_node_unlock(n_ptr);
			continue;
		}
		tipc_node_unlock(n_ptr);
		msg = buf_msg(buf);
		if (tipc_link_input(l_ptr, buf) != 0)
			goto discard;
		continue;
unlock_discard:
		tipc_node_unlock(n_ptr);
discard:
		kfree_skb(buf);
	}
}

/**
 * tipc_link_prepare_input - process TIPC link messages
 *
 * returns nonzero if the message was consumed
 *
 * Node lock must be held
 */
static int tipc_link_prepare_input(struct tipc_link *l, struct sk_buff **buf)
{
	struct tipc_node *n;
	struct tipc_msg *msg;
	int res = -EINVAL;

	n = l->owner;
	msg = buf_msg(*buf);
	switch (msg_user(msg)) {
	case CHANGEOVER_PROTOCOL:
		if (tipc_link_tunnel_rcv(n, buf))
			res = 0;
		break;
	case MSG_FRAGMENTER:
		l->stats.recv_fragments++;
		if (tipc_buf_append(&l->reasm_buf, buf)) {
			l->stats.recv_fragmented++;
			res = 0;
		} else if (!l->reasm_buf) {
			tipc_link_reset(l);
		}
		break;
	case MSG_BUNDLER:
		l->stats.recv_bundles++;
		l->stats.recv_bundled += msg_msgcnt(msg);
		res = 0;
		break;
	case NAME_DISTRIBUTOR:
		n->bclink.recv_permitted = true;
		res = 0;
		break;
	case BCAST_PROTOCOL:
		tipc_link_sync_rcv(n, *buf);
		break;
	default:
		res = 0;
	}
	return res;
}
/**
 * tipc_link_input - Deliver message too higher layers
 */
static int tipc_link_input(struct tipc_link *l, struct sk_buff *buf)
{
	struct tipc_msg *msg = buf_msg(buf);
	int res = 0;

	switch (msg_user(msg)) {
=======
void link_prepare_wakeup(struct tipc_link *l)
{
	int pnd[TIPC_SYSTEM_IMPORTANCE + 1] = {0,};
	int imp, lim;
	struct sk_buff *skb, *tmp;

	skb_queue_walk_safe(&l->wakeupq, skb, tmp) {
		imp = TIPC_SKB_CB(skb)->chain_imp;
		lim = l->backlog[imp].limit;
		pnd[imp] += TIPC_SKB_CB(skb)->chain_sz;
		if ((pnd[imp] + l->backlog[imp].len) >= lim)
			break;
		skb_unlink(skb, &l->wakeupq);
		skb_queue_tail(l->inputq, skb);
	}
}

void tipc_link_reset(struct tipc_link *l)
{
	l->peer_session = ANY_SESSION;
	l->session++;
	l->mtu = l->advertised_mtu;
	__skb_queue_purge(&l->transmq);
	__skb_queue_purge(&l->deferdq);
	skb_queue_splice_init(&l->wakeupq, l->inputq);
	__skb_queue_purge(&l->backlogq);
	l->backlog[TIPC_LOW_IMPORTANCE].len = 0;
	l->backlog[TIPC_MEDIUM_IMPORTANCE].len = 0;
	l->backlog[TIPC_HIGH_IMPORTANCE].len = 0;
	l->backlog[TIPC_CRITICAL_IMPORTANCE].len = 0;
	l->backlog[TIPC_SYSTEM_IMPORTANCE].len = 0;
	kfree_skb(l->reasm_buf);
	kfree_skb(l->failover_reasm_skb);
	l->reasm_buf = NULL;
	l->failover_reasm_skb = NULL;
	l->rcv_unacked = 0;
	l->snd_nxt = 1;
	l->rcv_nxt = 1;
	l->acked = 0;
	l->silent_intv_cnt = 0;
	l->rst_cnt = 0;
	l->stale_count = 0;
	l->bc_peer_is_up = false;
	memset(&l->mon_state, 0, sizeof(l->mon_state));
	tipc_link_reset_stats(l);
}

/**
 * tipc_link_xmit(): enqueue buffer list according to queue situation
 * @link: link to use
 * @list: chain of buffers containing message
 * @xmitq: returned list of packets to be sent by caller
 *
 * Consumes the buffer chain, except when returning -ELINKCONG,
 * since the caller then may want to make more send attempts.
 * Returns 0 if success, or errno: -ELINKCONG, -EMSGSIZE or -ENOBUFS
 * Messages at TIPC_SYSTEM_IMPORTANCE are always accepted
 */
int tipc_link_xmit(struct tipc_link *l, struct sk_buff_head *list,
		   struct sk_buff_head *xmitq)
{
	struct tipc_msg *hdr = buf_msg(skb_peek(list));
	unsigned int maxwin = l->window;
	unsigned int i, imp = msg_importance(hdr);
	unsigned int mtu = l->mtu;
	u16 ack = l->rcv_nxt - 1;
	u16 seqno = l->snd_nxt;
	u16 bc_ack = l->bc_rcvlink->rcv_nxt - 1;
	struct sk_buff_head *transmq = &l->transmq;
	struct sk_buff_head *backlogq = &l->backlogq;
	struct sk_buff *skb, *_skb, *bskb;
	int pkt_cnt = skb_queue_len(list);

	/* Match msg importance against this and all higher backlog limits: */
	if (!skb_queue_empty(backlogq)) {
		for (i = imp; i <= TIPC_SYSTEM_IMPORTANCE; i++) {
			if (unlikely(l->backlog[i].len >= l->backlog[i].limit))
				return link_schedule_user(l, list);
		}
	}
	if (unlikely(msg_size(hdr) > mtu)) {
		skb_queue_purge(list);
		return -EMSGSIZE;
	}

	if (pkt_cnt > 1) {
		l->stats.sent_fragmented++;
		l->stats.sent_fragments += pkt_cnt;
	}

	/* Prepare each packet for sending, and add to relevant queue: */
	while (skb_queue_len(list)) {
		skb = skb_peek(list);
		hdr = buf_msg(skb);
		msg_set_seqno(hdr, seqno);
		msg_set_ack(hdr, ack);
		msg_set_bcast_ack(hdr, bc_ack);

		if (likely(skb_queue_len(transmq) < maxwin)) {
			_skb = skb_clone(skb, GFP_ATOMIC);
			if (!_skb) {
				skb_queue_purge(list);
				return -ENOBUFS;
			}
			__skb_dequeue(list);
			__skb_queue_tail(transmq, skb);
			__skb_queue_tail(xmitq, _skb);
			TIPC_SKB_CB(skb)->ackers = l->ackers;
			l->rcv_unacked = 0;
			l->stats.sent_pkts++;
			seqno++;
			continue;
		}
		if (tipc_msg_bundle(skb_peek_tail(backlogq), hdr, mtu)) {
			kfree_skb(__skb_dequeue(list));
			l->stats.sent_bundled++;
			continue;
		}
		if (tipc_msg_make_bundle(&bskb, hdr, mtu, l->addr)) {
			kfree_skb(__skb_dequeue(list));
			__skb_queue_tail(backlogq, bskb);
			l->backlog[msg_importance(buf_msg(bskb))].len++;
			l->stats.sent_bundled++;
			l->stats.sent_bundles++;
			continue;
		}
		l->backlog[imp].len += skb_queue_len(list);
		skb_queue_splice_tail_init(list, backlogq);
	}
	l->snd_nxt = seqno;
	return 0;
}

void tipc_link_advance_backlog(struct tipc_link *l, struct sk_buff_head *xmitq)
{
	struct sk_buff *skb, *_skb;
	struct tipc_msg *hdr;
	u16 seqno = l->snd_nxt;
	u16 ack = l->rcv_nxt - 1;
	u16 bc_ack = l->bc_rcvlink->rcv_nxt - 1;

	while (skb_queue_len(&l->transmq) < l->window) {
		skb = skb_peek(&l->backlogq);
		if (!skb)
			break;
		_skb = skb_clone(skb, GFP_ATOMIC);
		if (!_skb)
			break;
		__skb_dequeue(&l->backlogq);
		hdr = buf_msg(skb);
		l->backlog[msg_importance(hdr)].len--;
		__skb_queue_tail(&l->transmq, skb);
		__skb_queue_tail(xmitq, _skb);
		TIPC_SKB_CB(skb)->ackers = l->ackers;
		msg_set_seqno(hdr, seqno);
		msg_set_ack(hdr, ack);
		msg_set_bcast_ack(hdr, bc_ack);
		l->rcv_unacked = 0;
		l->stats.sent_pkts++;
		seqno++;
	}
	l->snd_nxt = seqno;
}

static void link_retransmit_failure(struct tipc_link *l, struct sk_buff *skb)
{
	struct tipc_msg *hdr = buf_msg(skb);

	pr_warn("Retransmission failure on link <%s>\n", l->name);
	link_print(l, "Resetting link ");
	pr_info("Failed msg: usr %u, typ %u, len %u, err %u\n",
		msg_user(hdr), msg_type(hdr), msg_size(hdr), msg_errcode(hdr));
	pr_info("sqno %u, prev: %x, src: %x\n",
		msg_seqno(hdr), msg_prevnode(hdr), msg_orignode(hdr));
}

int tipc_link_retrans(struct tipc_link *l, u16 from, u16 to,
		      struct sk_buff_head *xmitq)
{
	struct sk_buff *_skb, *skb = skb_peek(&l->transmq);
	struct tipc_msg *hdr;
	u16 ack = l->rcv_nxt - 1;
	u16 bc_ack = l->bc_rcvlink->rcv_nxt - 1;

	if (!skb)
		return 0;

	/* Detect repeated retransmit failures on same packet */
	if (likely(l->last_retransm != buf_seqno(skb))) {
		l->last_retransm = buf_seqno(skb);
		l->stale_count = 1;
	} else if (++l->stale_count > 100) {
		link_retransmit_failure(l, skb);
		return tipc_link_fsm_evt(l, LINK_FAILURE_EVT);
	}

	/* Move forward to where retransmission should start */
	skb_queue_walk(&l->transmq, skb) {
		if (!less(buf_seqno(skb), from))
			break;
	}

	skb_queue_walk_from(&l->transmq, skb) {
		if (more(buf_seqno(skb), to))
			break;
		hdr = buf_msg(skb);
		_skb = __pskb_copy(skb, MIN_H_SIZE, GFP_ATOMIC);
		if (!_skb)
			return 0;
		hdr = buf_msg(_skb);
		msg_set_ack(hdr, ack);
		msg_set_bcast_ack(hdr, bc_ack);
		_skb->priority = TC_PRIO_CONTROL;
		__skb_queue_tail(xmitq, _skb);
		l->stats.retransmitted++;
	}
	return 0;
}

/* tipc_data_input - deliver data and name distr msgs to upper layer
 *
 * Consumes buffer if message is of right type
 * Node lock must be held
 */
static bool tipc_data_input(struct tipc_link *l, struct sk_buff *skb,
			    struct sk_buff_head *inputq)
{
	switch (msg_user(buf_msg(skb))) {
>>>>>>> v4.9.227
	case TIPC_LOW_IMPORTANCE:
	case TIPC_MEDIUM_IMPORTANCE:
	case TIPC_HIGH_IMPORTANCE:
	case TIPC_CRITICAL_IMPORTANCE:
	case CONN_MANAGER:
<<<<<<< HEAD
		tipc_sk_rcv(buf);
		break;
	case NAME_DISTRIBUTOR:
		tipc_named_rcv(buf);
		break;
	case MSG_BUNDLER:
		tipc_link_bundle_rcv(buf);
		break;
	default:
		res = -EINVAL;
	}
	return res;
}

/**
 * tipc_link_defer_pkt - Add out-of-sequence message to deferred reception queue
 *
 * Returns increase in queue length (i.e. 0 or 1)
 */
u32 tipc_link_defer_pkt(struct sk_buff **head, struct sk_buff **tail,
			struct sk_buff *buf)
{
	struct sk_buff *queue_buf;
	struct sk_buff **prev;
	u32 seq_no = buf_seqno(buf);

	buf->next = NULL;

	/* Empty queue ? */
	if (*head == NULL) {
		*head = *tail = buf;
		return 1;
	}

	/* Last ? */
	if (less(buf_seqno(*tail), seq_no)) {
		(*tail)->next = buf;
		*tail = buf;
		return 1;
	}

	/* Locate insertion point in queue, then insert; discard if duplicate */
	prev = head;
	queue_buf = *head;
	for (;;) {
		u32 curr_seqno = buf_seqno(queue_buf);

		if (seq_no == curr_seqno) {
			kfree_skb(buf);
			return 0;
		}

		if (less(seq_no, curr_seqno))
			break;

		prev = &queue_buf->next;
		queue_buf = queue_buf->next;
	}

	buf->next = queue_buf;
	*prev = buf;
	return 1;
}

/*
 * link_handle_out_of_seq_msg - handle arrival of out-of-sequence packet
 */
static void link_handle_out_of_seq_msg(struct tipc_link *l_ptr,
				       struct sk_buff *buf)
{
	u32 seq_no = buf_seqno(buf);

	if (likely(msg_user(buf_msg(buf)) == LINK_PROTOCOL)) {
		tipc_link_proto_rcv(l_ptr, buf);
		return;
	}

	/* Record OOS packet arrival (force mismatch on next timeout) */
	l_ptr->checkpoint--;

	/*
	 * Discard packet if a duplicate; otherwise add it to deferred queue
	 * and notify peer of gap as per protocol specification
	 */
	if (less(seq_no, mod(l_ptr->next_in_no))) {
		l_ptr->stats.duplicates++;
		kfree_skb(buf);
		return;
	}

	if (tipc_link_defer_pkt(&l_ptr->oldest_deferred_in,
				&l_ptr->newest_deferred_in, buf)) {
		l_ptr->deferred_inqueue_sz++;
		l_ptr->stats.deferred_recv++;
		TIPC_SKB_CB(buf)->deferred = true;
		if ((l_ptr->deferred_inqueue_sz % 16) == 1)
			tipc_link_proto_xmit(l_ptr, STATE_MSG, 0, 0, 0, 0, 0);
	} else
		l_ptr->stats.duplicates++;
}

/*
 * Send protocol message to the other endpoint.
 */
void tipc_link_proto_xmit(struct tipc_link *l_ptr, u32 msg_typ, int probe_msg,
			  u32 gap, u32 tolerance, u32 priority, u32 ack_mtu)
{
	struct sk_buff *buf = NULL;
	struct tipc_msg *msg = l_ptr->pmsg;
	u32 msg_size = sizeof(l_ptr->proto_msg);
	int r_flag;

	/* Discard any previous message that was deferred due to congestion */
	if (l_ptr->proto_msg_queue) {
		kfree_skb(l_ptr->proto_msg_queue);
		l_ptr->proto_msg_queue = NULL;
	}

	/* Don't send protocol message during link changeover */
	if (l_ptr->exp_msg_count)
		return;

	/* Abort non-RESET send if communication with node is prohibited */
	if ((tipc_node_blocked(l_ptr->owner)) && (msg_typ != RESET_MSG))
		return;

	/* Create protocol message with "out-of-sequence" sequence number */
	msg_set_type(msg, msg_typ);
	msg_set_net_plane(msg, l_ptr->net_plane);
	msg_set_bcast_ack(msg, l_ptr->owner->bclink.last_in);
	msg_set_last_bcast(msg, tipc_bclink_get_last_sent());

	if (msg_typ == STATE_MSG) {
		u32 next_sent = mod(l_ptr->next_out_no);

		if (!tipc_link_is_up(l_ptr))
			return;
		if (l_ptr->next_out)
			next_sent = buf_seqno(l_ptr->next_out);
		msg_set_next_sent(msg, next_sent);
		if (l_ptr->oldest_deferred_in) {
			u32 rec = buf_seqno(l_ptr->oldest_deferred_in);
			gap = mod(rec - mod(l_ptr->next_in_no));
		}
		msg_set_seq_gap(msg, gap);
		if (gap)
			l_ptr->stats.sent_nacks++;
		msg_set_link_tolerance(msg, tolerance);
		msg_set_linkprio(msg, priority);
		msg_set_max_pkt(msg, ack_mtu);
		msg_set_ack(msg, mod(l_ptr->next_in_no - 1));
		msg_set_probe(msg, probe_msg != 0);
		if (probe_msg) {
			u32 mtu = l_ptr->max_pkt;

			if ((mtu < l_ptr->max_pkt_target) &&
			    link_working_working(l_ptr) &&
			    l_ptr->fsm_msg_cnt) {
				msg_size = (mtu + (l_ptr->max_pkt_target - mtu)/2 + 2) & ~3;
				if (l_ptr->max_pkt_probes == 10) {
					l_ptr->max_pkt_target = (msg_size - 4);
					l_ptr->max_pkt_probes = 0;
					msg_size = (mtu + (l_ptr->max_pkt_target - mtu)/2 + 2) & ~3;
				}
				l_ptr->max_pkt_probes++;
			}

			l_ptr->stats.sent_probes++;
		}
		l_ptr->stats.sent_states++;
	} else {		/* RESET_MSG or ACTIVATE_MSG */
		msg_set_ack(msg, mod(l_ptr->reset_checkpoint - 1));
		msg_set_seq_gap(msg, 0);
		msg_set_next_sent(msg, 1);
		msg_set_probe(msg, 0);
		msg_set_link_tolerance(msg, l_ptr->tolerance);
		msg_set_linkprio(msg, l_ptr->priority);
		msg_set_max_pkt(msg, l_ptr->max_pkt_target);
	}

	r_flag = (l_ptr->owner->working_links > tipc_link_is_up(l_ptr));
	msg_set_redundant_link(msg, r_flag);
	msg_set_linkprio(msg, l_ptr->priority);
	msg_set_size(msg, msg_size);

	msg_set_seqno(msg, mod(l_ptr->next_out_no + (0xffff/2)));

	buf = tipc_buf_acquire(msg_size);
	if (!buf)
		return;

	skb_copy_to_linear_data(buf, msg, sizeof(l_ptr->proto_msg));
	buf->priority = TC_PRIO_CONTROL;

	tipc_bearer_send(l_ptr->bearer_id, buf, &l_ptr->media_addr);
	l_ptr->unacked_window = 0;
	kfree_skb(buf);
}

/*
 * Receive protocol message :
 * Note that network plane id propagates through the network, and may
 * change at any time. The node with lowest address rules
 */
static void tipc_link_proto_rcv(struct tipc_link *l_ptr, struct sk_buff *buf)
{
	u32 rec_gap = 0;
	u32 max_pkt_info;
	u32 max_pkt_ack;
	u32 msg_tol;
	struct tipc_msg *msg = buf_msg(buf);

	/* Discard protocol message during link changeover */
	if (l_ptr->exp_msg_count)
		goto exit;

	if (l_ptr->net_plane != msg_net_plane(msg))
		if (tipc_own_addr > msg_prevnode(msg))
			l_ptr->net_plane = msg_net_plane(msg);

	switch (msg_type(msg)) {

	case RESET_MSG:
		if (!link_working_unknown(l_ptr) &&
		    (l_ptr->peer_session != INVALID_SESSION)) {
			if (less_eq(msg_session(msg), l_ptr->peer_session))
				break; /* duplicate or old reset: ignore */
		}

		if (!msg_redundant_link(msg) && (link_working_working(l_ptr) ||
				link_working_unknown(l_ptr))) {
			/*
			 * peer has lost contact -- don't allow peer's links
			 * to reactivate before we recognize loss & clean up
			 */
			l_ptr->owner->action_flags |= TIPC_WAIT_OWN_LINKS_DOWN;
		}

		link_state_event(l_ptr, RESET_MSG);

		/* fall thru' */
	case ACTIVATE_MSG:
		/* Update link settings according other endpoint's values */
		strcpy((strrchr(l_ptr->name, ':') + 1), (char *)msg_data(msg));

		msg_tol = msg_link_tolerance(msg);
		if (msg_tol > l_ptr->tolerance)
			link_set_supervision_props(l_ptr, msg_tol);

		if (msg_linkprio(msg) > l_ptr->priority)
			l_ptr->priority = msg_linkprio(msg);

		max_pkt_info = msg_max_pkt(msg);
		if (max_pkt_info) {
			if (max_pkt_info < l_ptr->max_pkt_target)
				l_ptr->max_pkt_target = max_pkt_info;
			if (l_ptr->max_pkt > l_ptr->max_pkt_target)
				l_ptr->max_pkt = l_ptr->max_pkt_target;
		} else {
			l_ptr->max_pkt = l_ptr->max_pkt_target;
		}

		/* Synchronize broadcast link info, if not done previously */
		if (!tipc_node_is_up(l_ptr->owner)) {
			l_ptr->owner->bclink.last_sent =
				l_ptr->owner->bclink.last_in =
				msg_last_bcast(msg);
			l_ptr->owner->bclink.oos_state = 0;
		}

		l_ptr->peer_session = msg_session(msg);
		l_ptr->peer_bearer_id = msg_bearer_id(msg);

		if (msg_type(msg) == ACTIVATE_MSG)
			link_state_event(l_ptr, ACTIVATE_MSG);
		break;
	case STATE_MSG:

		msg_tol = msg_link_tolerance(msg);
		if (msg_tol)
			link_set_supervision_props(l_ptr, msg_tol);

		if (msg_linkprio(msg) &&
		    (msg_linkprio(msg) != l_ptr->priority)) {
			pr_warn("%s<%s>, priority change %u->%u\n",
				link_rst_msg, l_ptr->name, l_ptr->priority,
				msg_linkprio(msg));
			l_ptr->priority = msg_linkprio(msg);
			tipc_link_reset(l_ptr); /* Enforce change to take effect */
			break;
		}

		/* Record reception; force mismatch at next timeout: */
		l_ptr->checkpoint--;

		link_state_event(l_ptr, TRAFFIC_MSG_EVT);
		l_ptr->stats.recv_states++;
		if (link_reset_unknown(l_ptr))
			break;

		if (less_eq(mod(l_ptr->next_in_no), msg_next_sent(msg))) {
			rec_gap = mod(msg_next_sent(msg) -
				      mod(l_ptr->next_in_no));
		}

		max_pkt_ack = msg_max_pkt(msg);
		if (max_pkt_ack > l_ptr->max_pkt) {
			l_ptr->max_pkt = max_pkt_ack;
			l_ptr->max_pkt_probes = 0;
		}

		max_pkt_ack = 0;
		if (msg_probe(msg)) {
			l_ptr->stats.recv_probes++;
			if (msg_size(msg) > sizeof(l_ptr->proto_msg))
				max_pkt_ack = msg_size(msg);
		}

		/* Protocol message before retransmits, reduce loss risk */
		if (l_ptr->owner->bclink.recv_permitted)
			tipc_bclink_update_link_state(l_ptr->owner,
						      msg_last_bcast(msg));

		if (rec_gap || (msg_probe(msg))) {
			tipc_link_proto_xmit(l_ptr, STATE_MSG, 0, rec_gap, 0,
					     0, max_pkt_ack);
		}
		if (msg_seq_gap(msg)) {
			l_ptr->stats.recv_nacks++;
			tipc_link_retransmit(l_ptr, l_ptr->first_out,
					     msg_seq_gap(msg));
		}
		break;
	}
exit:
	kfree_skb(buf);
}


/* tipc_link_tunnel_xmit(): Tunnel one packet via a link belonging to
 * a different bearer. Owner node is locked.
 */
static void tipc_link_tunnel_xmit(struct tipc_link *l_ptr,
				  struct tipc_msg *tunnel_hdr,
				  struct tipc_msg *msg,
				  u32 selector)
{
	struct tipc_link *tunnel;
	struct sk_buff *buf;
	u32 length = msg_size(msg);

	tunnel = l_ptr->owner->active_links[selector & 1];
	if (!tipc_link_is_up(tunnel)) {
		pr_warn("%stunnel link no longer available\n", link_co_err);
		return;
	}
	msg_set_size(tunnel_hdr, length + INT_H_SIZE);
	buf = tipc_buf_acquire(length + INT_H_SIZE);
	if (!buf) {
		pr_warn("%sunable to send tunnel msg\n", link_co_err);
		return;
	}
	skb_copy_to_linear_data(buf, tunnel_hdr, INT_H_SIZE);
	skb_copy_to_linear_data_offset(buf, INT_H_SIZE, msg, length);
	__tipc_link_xmit(tunnel, buf);
}


/* tipc_link_failover_send_queue(): A link has gone down, but a second
 * link is still active. We can do failover. Tunnel the failing link's
 * whole send queue via the remaining link. This way, we don't lose
 * any packets, and sequence order is preserved for subsequent traffic
 * sent over the remaining link. Owner node is locked.
 */
void tipc_link_failover_send_queue(struct tipc_link *l_ptr)
{
	u32 msgcount = l_ptr->out_queue_size;
	struct sk_buff *crs = l_ptr->first_out;
	struct tipc_link *tunnel = l_ptr->owner->active_links[0];
	struct tipc_msg tunnel_hdr;
	int split_bundles;

	if (!tunnel)
		return;

	tipc_msg_init(&tunnel_hdr, CHANGEOVER_PROTOCOL,
		 ORIGINAL_MSG, INT_H_SIZE, l_ptr->addr);
	msg_set_bearer_id(&tunnel_hdr, l_ptr->peer_bearer_id);
	msg_set_msgcnt(&tunnel_hdr, msgcount);

	if (!l_ptr->first_out) {
		struct sk_buff *buf;

		buf = tipc_buf_acquire(INT_H_SIZE);
		if (buf) {
			skb_copy_to_linear_data(buf, &tunnel_hdr, INT_H_SIZE);
			msg_set_size(&tunnel_hdr, INT_H_SIZE);
			__tipc_link_xmit(tunnel, buf);
		} else {
			pr_warn("%sunable to send changeover msg\n",
				link_co_err);
		}
		return;
	}

	split_bundles = (l_ptr->owner->active_links[0] !=
			 l_ptr->owner->active_links[1]);

	while (crs) {
		struct tipc_msg *msg = buf_msg(crs);

		if ((msg_user(msg) == MSG_BUNDLER) && split_bundles) {
			struct tipc_msg *m = msg_get_wrapped(msg);
			unchar *pos = (unchar *)m;

			msgcount = msg_msgcnt(msg);
			while (msgcount--) {
				msg_set_seqno(m, msg_seqno(msg));
				tipc_link_tunnel_xmit(l_ptr, &tunnel_hdr, m,
						      msg_link_selector(m));
				pos += align(msg_size(m));
				m = (struct tipc_msg *)pos;
			}
		} else {
			tipc_link_tunnel_xmit(l_ptr, &tunnel_hdr, msg,
					      msg_link_selector(msg));
		}
		crs = crs->next;
	}
}

/* tipc_link_dup_queue_xmit(): A second link has become active. Tunnel a
 * duplicate of the first link's send queue via the new link. This way, we
 * are guaranteed that currently queued packets from a socket are delivered
 * before future traffic from the same socket, even if this is using the
 * new link. The last arriving copy of each duplicate packet is dropped at
 * the receiving end by the regular protocol check, so packet cardinality
 * and sequence order is preserved per sender/receiver socket pair.
 * Owner node is locked.
 */
void tipc_link_dup_queue_xmit(struct tipc_link *l_ptr,
			      struct tipc_link *tunnel)
{
	struct sk_buff *iter;
	struct tipc_msg tunnel_hdr;

	tipc_msg_init(&tunnel_hdr, CHANGEOVER_PROTOCOL,
		 DUPLICATE_MSG, INT_H_SIZE, l_ptr->addr);
	msg_set_msgcnt(&tunnel_hdr, l_ptr->out_queue_size);
	msg_set_bearer_id(&tunnel_hdr, l_ptr->peer_bearer_id);
	iter = l_ptr->first_out;
	while (iter) {
		struct sk_buff *outbuf;
		struct tipc_msg *msg = buf_msg(iter);
		u32 length = msg_size(msg);

		if (msg_user(msg) == MSG_BUNDLER)
			msg_set_type(msg, CLOSED_MSG);
		msg_set_ack(msg, mod(l_ptr->next_in_no - 1));	/* Update */
		msg_set_bcast_ack(msg, l_ptr->owner->bclink.last_in);
		msg_set_size(&tunnel_hdr, length + INT_H_SIZE);
		outbuf = tipc_buf_acquire(length + INT_H_SIZE);
		if (outbuf == NULL) {
			pr_warn("%sunable to send duplicate msg\n",
				link_co_err);
			return;
		}
		skb_copy_to_linear_data(outbuf, &tunnel_hdr, INT_H_SIZE);
		skb_copy_to_linear_data_offset(outbuf, INT_H_SIZE, iter->data,
					       length);
		__tipc_link_xmit(tunnel, outbuf);
		if (!tipc_link_is_up(l_ptr))
			return;
		iter = iter->next;
	}
}

/**
 * buf_extract - extracts embedded TIPC message from another message
 * @skb: encapsulating message buffer
 * @from_pos: offset to extract from
 *
 * Returns a new message buffer containing an embedded message.  The
 * encapsulating message itself is left unchanged.
 */
static struct sk_buff *buf_extract(struct sk_buff *skb, u32 from_pos)
{
	struct tipc_msg *msg = (struct tipc_msg *)(skb->data + from_pos);
	u32 size = msg_size(msg);
	struct sk_buff *eb;

	eb = tipc_buf_acquire(size);
	if (eb)
		skb_copy_to_linear_data(eb, msg, size);
	return eb;
}



/* tipc_link_dup_rcv(): Receive a tunnelled DUPLICATE_MSG packet.
 * Owner node is locked.
 */
static void tipc_link_dup_rcv(struct tipc_link *l_ptr,
			      struct sk_buff *t_buf)
{
	struct sk_buff *buf;

	if (!tipc_link_is_up(l_ptr))
		return;

	buf = buf_extract(t_buf, INT_H_SIZE);
	if (buf == NULL) {
		pr_warn("%sfailed to extract inner dup pkt\n", link_co_err);
		return;
	}

	/* Add buffer to deferred queue, if applicable: */
	link_handle_out_of_seq_msg(l_ptr, buf);
}

/*  tipc_link_failover_rcv(): Receive a tunnelled ORIGINAL_MSG packet
 *  Owner node is locked.
 */
static struct sk_buff *tipc_link_failover_rcv(struct tipc_link *l_ptr,
					      struct sk_buff *t_buf)
{
	struct tipc_msg *t_msg = buf_msg(t_buf);
	struct sk_buff *buf = NULL;
	struct tipc_msg *msg;

	if (tipc_link_is_up(l_ptr))
		tipc_link_reset(l_ptr);

	/* First failover packet? */
	if (l_ptr->exp_msg_count == START_CHANGEOVER)
		l_ptr->exp_msg_count = msg_msgcnt(t_msg);

	/* Should there be an inner packet? */
	if (l_ptr->exp_msg_count) {
		l_ptr->exp_msg_count--;
		buf = buf_extract(t_buf, INT_H_SIZE);
		if (buf == NULL) {
			pr_warn("%sno inner failover pkt\n", link_co_err);
			goto exit;
		}
		msg = buf_msg(buf);

		if (less(msg_seqno(msg), l_ptr->reset_checkpoint)) {
			kfree_skb(buf);
			buf = NULL;
			goto exit;
		}
		if (msg_user(msg) == MSG_FRAGMENTER) {
			l_ptr->stats.recv_fragments++;
			tipc_buf_append(&l_ptr->reasm_buf, &buf);
		}
	}
exit:
	if ((l_ptr->exp_msg_count == 0) && (l_ptr->flags & LINK_STOPPED)) {
		tipc_node_detach_link(l_ptr->owner, l_ptr);
		kfree(l_ptr);
	}
	return buf;
}

/*  tipc_link_tunnel_rcv(): Receive a tunnelled packet, sent
 *  via other link as result of a failover (ORIGINAL_MSG) or
 *  a new active link (DUPLICATE_MSG). Failover packets are
 *  returned to the active link for delivery upwards.
 *  Owner node is locked.
 */
static int tipc_link_tunnel_rcv(struct tipc_node *n_ptr,
				struct sk_buff **buf)
{
	struct sk_buff *t_buf = *buf;
	struct tipc_link *l_ptr;
	struct tipc_msg *t_msg = buf_msg(t_buf);
	u32 bearer_id = msg_bearer_id(t_msg);

	*buf = NULL;

	if (bearer_id >= MAX_BEARERS)
		goto exit;

	l_ptr = n_ptr->links[bearer_id];
	if (!l_ptr)
		goto exit;

	if (msg_type(t_msg) == DUPLICATE_MSG)
		tipc_link_dup_rcv(l_ptr, t_buf);
	else if (msg_type(t_msg) == ORIGINAL_MSG)
		*buf = tipc_link_failover_rcv(l_ptr, t_buf);
	else
		pr_warn("%sunknown tunnel pkt received\n", link_co_err);
exit:
	kfree_skb(t_buf);
	return *buf != NULL;
}

/*
 *  Bundler functionality:
 */
void tipc_link_bundle_rcv(struct sk_buff *buf)
{
	u32 msgcount = msg_msgcnt(buf_msg(buf));
	u32 pos = INT_H_SIZE;
	struct sk_buff *obuf;
	struct tipc_msg *omsg;

	while (msgcount--) {
		obuf = buf_extract(buf, pos);
		if (obuf == NULL) {
			pr_warn("Link unable to unbundle message(s)\n");
			break;
		}
		omsg = buf_msg(obuf);
		pos += align(msg_size(omsg));
		if (msg_isdata(omsg)) {
			if (unlikely(msg_type(omsg) == TIPC_MCAST_MSG))
				tipc_sk_mcast_rcv(obuf);
			else
				tipc_sk_rcv(obuf);
		} else if (msg_user(omsg) == CONN_MANAGER) {
			tipc_sk_rcv(obuf);
		} else if (msg_user(omsg) == NAME_DISTRIBUTOR) {
			tipc_named_rcv(obuf);
		} else {
			pr_warn("Illegal bundled msg: %u\n", msg_user(omsg));
			kfree_skb(obuf);
		}
	}
	kfree_skb(buf);
}

static void link_set_supervision_props(struct tipc_link *l_ptr, u32 tolerance)
{
	if ((tolerance < TIPC_MIN_LINK_TOL) || (tolerance > TIPC_MAX_LINK_TOL))
		return;

	l_ptr->tolerance = tolerance;
	l_ptr->continuity_interval =
		((tolerance / 4) > 500) ? 500 : tolerance / 4;
	l_ptr->abort_limit = tolerance / (l_ptr->continuity_interval / 4);
}

void tipc_link_set_queue_limits(struct tipc_link *l_ptr, u32 window)
{
	/* Data messages from this node, inclusive FIRST_FRAGM */
	l_ptr->queue_limit[TIPC_LOW_IMPORTANCE] = window;
	l_ptr->queue_limit[TIPC_MEDIUM_IMPORTANCE] = (window / 3) * 4;
	l_ptr->queue_limit[TIPC_HIGH_IMPORTANCE] = (window / 3) * 5;
	l_ptr->queue_limit[TIPC_CRITICAL_IMPORTANCE] = (window / 3) * 6;
	/* Transiting data messages,inclusive FIRST_FRAGM */
	l_ptr->queue_limit[TIPC_LOW_IMPORTANCE + 4] = 300;
	l_ptr->queue_limit[TIPC_MEDIUM_IMPORTANCE + 4] = 600;
	l_ptr->queue_limit[TIPC_HIGH_IMPORTANCE + 4] = 900;
	l_ptr->queue_limit[TIPC_CRITICAL_IMPORTANCE + 4] = 1200;
	l_ptr->queue_limit[CONN_MANAGER] = 1200;
	l_ptr->queue_limit[CHANGEOVER_PROTOCOL] = 2500;
	l_ptr->queue_limit[NAME_DISTRIBUTOR] = 3000;
	/* FRAGMENT and LAST_FRAGMENT packets */
	l_ptr->queue_limit[MSG_FRAGMENTER] = 4000;
}

/* tipc_link_find_owner - locate owner node of link by link's name
 * @name: pointer to link name string
 * @bearer_id: pointer to index in 'node->links' array where the link was found.
 *
 * Returns pointer to node owning the link, or 0 if no matching link is found.
 */
static struct tipc_node *tipc_link_find_owner(const char *link_name,
					      unsigned int *bearer_id)
{
	struct tipc_link *l_ptr;
	struct tipc_node *n_ptr;
	struct tipc_node *found_node = 0;
	int i;

	*bearer_id = 0;
	rcu_read_lock();
	list_for_each_entry_rcu(n_ptr, &tipc_node_list, list) {
		tipc_node_lock(n_ptr);
		for (i = 0; i < MAX_BEARERS; i++) {
			l_ptr = n_ptr->links[i];
			if (l_ptr && !strcmp(l_ptr->name, link_name)) {
				*bearer_id = i;
				found_node = n_ptr;
				break;
			}
		}
		tipc_node_unlock(n_ptr);
		if (found_node)
			break;
	}
	rcu_read_unlock();

	return found_node;
}

/**
 * link_value_is_valid -- validate proposed link tolerance/priority/window
 *
 * @cmd: value type (TIPC_CMD_SET_LINK_*)
 * @new_value: the new value
 *
 * Returns 1 if value is within range, 0 if not.
 */
static int link_value_is_valid(u16 cmd, u32 new_value)
{
	switch (cmd) {
	case TIPC_CMD_SET_LINK_TOL:
		return (new_value >= TIPC_MIN_LINK_TOL) &&
			(new_value <= TIPC_MAX_LINK_TOL);
	case TIPC_CMD_SET_LINK_PRI:
		return (new_value <= TIPC_MAX_LINK_PRI);
	case TIPC_CMD_SET_LINK_WINDOW:
		return (new_value >= TIPC_MIN_LINK_WIN) &&
			(new_value <= TIPC_MAX_LINK_WIN);
	}
	return 0;
}

/**
 * link_cmd_set_value - change priority/tolerance/window for link/bearer/media
 * @name: ptr to link, bearer, or media name
 * @new_value: new value of link, bearer, or media setting
 * @cmd: which link, bearer, or media attribute to set (TIPC_CMD_SET_LINK_*)
 *
 * Caller must hold RTNL lock to ensure link/bearer/media is not deleted.
 *
 * Returns 0 if value updated and negative value on error.
 */
static int link_cmd_set_value(const char *name, u32 new_value, u16 cmd)
{
	struct tipc_node *node;
	struct tipc_link *l_ptr;
	struct tipc_bearer *b_ptr;
	struct tipc_media *m_ptr;
	int bearer_id;
	int res = 0;

	node = tipc_link_find_owner(name, &bearer_id);
	if (node) {
		tipc_node_lock(node);
		l_ptr = node->links[bearer_id];

		if (l_ptr) {
			switch (cmd) {
			case TIPC_CMD_SET_LINK_TOL:
				link_set_supervision_props(l_ptr, new_value);
				tipc_link_proto_xmit(l_ptr, STATE_MSG, 0, 0,
						     new_value, 0, 0);
				break;
			case TIPC_CMD_SET_LINK_PRI:
				l_ptr->priority = new_value;
				tipc_link_proto_xmit(l_ptr, STATE_MSG, 0, 0,
						     0, new_value, 0);
				break;
			case TIPC_CMD_SET_LINK_WINDOW:
				tipc_link_set_queue_limits(l_ptr, new_value);
				break;
			default:
				res = -EINVAL;
				break;
			}
		}
		tipc_node_unlock(node);
		return res;
	}

	b_ptr = tipc_bearer_find(name);
	if (b_ptr) {
		switch (cmd) {
		case TIPC_CMD_SET_LINK_TOL:
			b_ptr->tolerance = new_value;
			break;
		case TIPC_CMD_SET_LINK_PRI:
			b_ptr->priority = new_value;
			break;
		case TIPC_CMD_SET_LINK_WINDOW:
			b_ptr->window = new_value;
			break;
		default:
			res = -EINVAL;
			break;
		}
		return res;
	}

	m_ptr = tipc_media_find(name);
	if (!m_ptr)
		return -ENODEV;
	switch (cmd) {
	case TIPC_CMD_SET_LINK_TOL:
		m_ptr->tolerance = new_value;
		break;
	case TIPC_CMD_SET_LINK_PRI:
		m_ptr->priority = new_value;
		break;
	case TIPC_CMD_SET_LINK_WINDOW:
		m_ptr->window = new_value;
		break;
	default:
		res = -EINVAL;
		break;
	}
	return res;
}

struct sk_buff *tipc_link_cmd_config(const void *req_tlv_area, int req_tlv_space,
				     u16 cmd)
{
	struct tipc_link_config *args;
	u32 new_value;
	int res;

	if (!TLV_CHECK(req_tlv_area, req_tlv_space, TIPC_TLV_LINK_CONFIG))
		return tipc_cfg_reply_error_string(TIPC_CFG_TLV_ERROR);

	args = (struct tipc_link_config *)TLV_DATA(req_tlv_area);
	new_value = ntohl(args->value);

	if (!link_value_is_valid(cmd, new_value))
		return tipc_cfg_reply_error_string(
			"cannot change, value invalid");

	if (!strcmp(args->name, tipc_bclink_name)) {
		if ((cmd == TIPC_CMD_SET_LINK_WINDOW) &&
		    (tipc_bclink_set_queue_limits(new_value) == 0))
			return tipc_cfg_reply_none();
		return tipc_cfg_reply_error_string(TIPC_CFG_NOT_SUPPORTED
						   " (cannot change setting on broadcast link)");
	}

	res = link_cmd_set_value(args->name, new_value, cmd);
	if (res)
		return tipc_cfg_reply_error_string("cannot change link setting");

	return tipc_cfg_reply_none();
}

/**
 * link_reset_statistics - reset link statistics
 * @l_ptr: pointer to link
 */
static void link_reset_statistics(struct tipc_link *l_ptr)
{
	memset(&l_ptr->stats, 0, sizeof(l_ptr->stats));
	l_ptr->stats.sent_info = l_ptr->next_out_no;
	l_ptr->stats.recv_info = l_ptr->next_in_no;
}

struct sk_buff *tipc_link_cmd_reset_stats(const void *req_tlv_area, int req_tlv_space)
{
	char *link_name;
	struct tipc_link *l_ptr;
	struct tipc_node *node;
	unsigned int bearer_id;

	if (!TLV_CHECK(req_tlv_area, req_tlv_space, TIPC_TLV_LINK_NAME))
		return tipc_cfg_reply_error_string(TIPC_CFG_TLV_ERROR);

	link_name = (char *)TLV_DATA(req_tlv_area);
	if (!strcmp(link_name, tipc_bclink_name)) {
		if (tipc_bclink_reset_stats())
			return tipc_cfg_reply_error_string("link not found");
		return tipc_cfg_reply_none();
	}
	node = tipc_link_find_owner(link_name, &bearer_id);
	if (!node)
		return tipc_cfg_reply_error_string("link not found");

	tipc_node_lock(node);
	l_ptr = node->links[bearer_id];
	if (!l_ptr) {
		tipc_node_unlock(node);
		return tipc_cfg_reply_error_string("link not found");
	}
	link_reset_statistics(l_ptr);
	tipc_node_unlock(node);
	return tipc_cfg_reply_none();
}

/**
 * percent - convert count to a percentage of total (rounding up or down)
 */
static u32 percent(u32 count, u32 total)
{
	return (count * 100 + (total / 2)) / total;
}

/**
 * tipc_link_stats - print link statistics
 * @name: link name
 * @buf: print buffer area
 * @buf_size: size of print buffer area
 *
 * Returns length of print buffer data string (or 0 if error)
 */
static int tipc_link_stats(const char *name, char *buf, const u32 buf_size)
{
	struct tipc_link *l;
	struct tipc_stats *s;
	struct tipc_node *node;
	char *status;
	u32 profile_total = 0;
	unsigned int bearer_id;
	int ret;

	if (!strcmp(name, tipc_bclink_name))
		return tipc_bclink_stats(buf, buf_size);

	node = tipc_link_find_owner(name, &bearer_id);
	if (!node)
		return 0;

	tipc_node_lock(node);

	l = node->links[bearer_id];
	if (!l) {
		tipc_node_unlock(node);
		return 0;
	}

	s = &l->stats;

	if (tipc_link_is_active(l))
		status = "ACTIVE";
	else if (tipc_link_is_up(l))
		status = "STANDBY";
	else
		status = "DEFUNCT";

	ret = tipc_snprintf(buf, buf_size, "Link <%s>\n"
			    "  %s  MTU:%u  Priority:%u  Tolerance:%u ms"
			    "  Window:%u packets\n",
			    l->name, status, l->max_pkt, l->priority,
			    l->tolerance, l->queue_limit[0]);

	ret += tipc_snprintf(buf + ret, buf_size - ret,
			     "  RX packets:%u fragments:%u/%u bundles:%u/%u\n",
			     l->next_in_no - s->recv_info, s->recv_fragments,
			     s->recv_fragmented, s->recv_bundles,
			     s->recv_bundled);

	ret += tipc_snprintf(buf + ret, buf_size - ret,
			     "  TX packets:%u fragments:%u/%u bundles:%u/%u\n",
			     l->next_out_no - s->sent_info, s->sent_fragments,
			     s->sent_fragmented, s->sent_bundles,
			     s->sent_bundled);

	profile_total = s->msg_length_counts;
	if (!profile_total)
		profile_total = 1;

	ret += tipc_snprintf(buf + ret, buf_size - ret,
			     "  TX profile sample:%u packets  average:%u octets\n"
			     "  0-64:%u%% -256:%u%% -1024:%u%% -4096:%u%% "
			     "-16384:%u%% -32768:%u%% -66000:%u%%\n",
			     s->msg_length_counts,
			     s->msg_lengths_total / profile_total,
			     percent(s->msg_length_profile[0], profile_total),
			     percent(s->msg_length_profile[1], profile_total),
			     percent(s->msg_length_profile[2], profile_total),
			     percent(s->msg_length_profile[3], profile_total),
			     percent(s->msg_length_profile[4], profile_total),
			     percent(s->msg_length_profile[5], profile_total),
			     percent(s->msg_length_profile[6], profile_total));

	ret += tipc_snprintf(buf + ret, buf_size - ret,
			     "  RX states:%u probes:%u naks:%u defs:%u"
			     " dups:%u\n", s->recv_states, s->recv_probes,
			     s->recv_nacks, s->deferred_recv, s->duplicates);

	ret += tipc_snprintf(buf + ret, buf_size - ret,
			     "  TX states:%u probes:%u naks:%u acks:%u"
			     " dups:%u\n", s->sent_states, s->sent_probes,
			     s->sent_nacks, s->sent_acks, s->retransmitted);

	ret += tipc_snprintf(buf + ret, buf_size - ret,
			     "  Congestion link:%u  Send queue"
			     " max:%u avg:%u\n", s->link_congs,
			     s->max_queue_sz, s->queue_sz_counts ?
			     (s->accu_queue_sz / s->queue_sz_counts) : 0);

	tipc_node_unlock(node);
	return ret;
}

struct sk_buff *tipc_link_cmd_show_stats(const void *req_tlv_area, int req_tlv_space)
{
	struct sk_buff *buf;
	struct tlv_desc *rep_tlv;
	int str_len;
	int pb_len;
	char *pb;

	if (!TLV_CHECK(req_tlv_area, req_tlv_space, TIPC_TLV_LINK_NAME))
		return tipc_cfg_reply_error_string(TIPC_CFG_TLV_ERROR);

	buf = tipc_cfg_reply_alloc(TLV_SPACE(ULTRA_STRING_MAX_LEN));
	if (!buf)
		return NULL;

	rep_tlv = (struct tlv_desc *)buf->data;
	pb = TLV_DATA(rep_tlv);
	pb_len = ULTRA_STRING_MAX_LEN;
	str_len = tipc_link_stats((char *)TLV_DATA(req_tlv_area),
				  pb, pb_len);
	if (!str_len) {
		kfree_skb(buf);
		return tipc_cfg_reply_error_string("link not found");
	}
	str_len += 1;	/* for "\0" */
	skb_put(buf, TLV_SPACE(str_len));
	TLV_SET(rep_tlv, TIPC_TLV_ULTRA_STRING, NULL, str_len);

	return buf;
}

/**
 * tipc_link_get_max_pkt - get maximum packet size to use when sending to destination
 * @dest: network address of destination node
 * @selector: used to select from set of active links
 *
 * If no active link can be found, uses default maximum packet size.
 */
u32 tipc_link_get_max_pkt(u32 dest, u32 selector)
{
	struct tipc_node *n_ptr;
	struct tipc_link *l_ptr;
	u32 res = MAX_PKT_DEFAULT;

	if (dest == tipc_own_addr)
		return MAX_MSG_SIZE;

	n_ptr = tipc_node_find(dest);
	if (n_ptr) {
		tipc_node_lock(n_ptr);
		l_ptr = n_ptr->active_links[selector & 1];
		if (l_ptr)
			res = l_ptr->max_pkt;
		tipc_node_unlock(n_ptr);
	}
	return res;
}

static void link_print(struct tipc_link *l_ptr, const char *str)
{
	struct tipc_bearer *b_ptr;

	rcu_read_lock();
	b_ptr = rcu_dereference_rtnl(bearer_list[l_ptr->bearer_id]);
	if (b_ptr)
		pr_info("%s Link %x<%s>:", str, l_ptr->addr, b_ptr->name);
	rcu_read_unlock();

	if (link_working_unknown(l_ptr))
		pr_cont(":WU\n");
	else if (link_reset_reset(l_ptr))
		pr_cont(":RR\n");
	else if (link_reset_unknown(l_ptr))
		pr_cont(":RU\n");
	else if (link_working_working(l_ptr))
		pr_cont(":WW\n");
	else
		pr_cont("\n");
=======
		skb_queue_tail(inputq, skb);
		return true;
	case NAME_DISTRIBUTOR:
		l->bc_rcvlink->state = LINK_ESTABLISHED;
		skb_queue_tail(l->namedq, skb);
		return true;
	case MSG_BUNDLER:
	case TUNNEL_PROTOCOL:
	case MSG_FRAGMENTER:
	case BCAST_PROTOCOL:
		return false;
	default:
		pr_warn("Dropping received illegal msg type\n");
		kfree_skb(skb);
		return true;
	};
}

/* tipc_link_input - process packet that has passed link protocol check
 *
 * Consumes buffer
 */
static int tipc_link_input(struct tipc_link *l, struct sk_buff *skb,
			   struct sk_buff_head *inputq)
{
	struct tipc_msg *hdr = buf_msg(skb);
	struct sk_buff **reasm_skb = &l->reasm_buf;
	struct sk_buff *iskb;
	struct sk_buff_head tmpq;
	int usr = msg_user(hdr);
	int rc = 0;
	int pos = 0;
	int ipos = 0;

	if (unlikely(usr == TUNNEL_PROTOCOL)) {
		if (msg_type(hdr) == SYNCH_MSG) {
			__skb_queue_purge(&l->deferdq);
			goto drop;
		}
		if (!tipc_msg_extract(skb, &iskb, &ipos))
			return rc;
		kfree_skb(skb);
		skb = iskb;
		hdr = buf_msg(skb);
		if (less(msg_seqno(hdr), l->drop_point))
			goto drop;
		if (tipc_data_input(l, skb, inputq))
			return rc;
		usr = msg_user(hdr);
		reasm_skb = &l->failover_reasm_skb;
	}

	if (usr == MSG_BUNDLER) {
		skb_queue_head_init(&tmpq);
		l->stats.recv_bundles++;
		l->stats.recv_bundled += msg_msgcnt(hdr);
		while (tipc_msg_extract(skb, &iskb, &pos))
			tipc_data_input(l, iskb, &tmpq);
		tipc_skb_queue_splice_tail(&tmpq, inputq);
		return 0;
	} else if (usr == MSG_FRAGMENTER) {
		l->stats.recv_fragments++;
		if (tipc_buf_append(reasm_skb, &skb)) {
			l->stats.recv_fragmented++;
			tipc_data_input(l, skb, inputq);
		} else if (!*reasm_skb && !link_is_bc_rcvlink(l)) {
			pr_warn_ratelimited("Unable to build fragment list\n");
			return tipc_link_fsm_evt(l, LINK_FAILURE_EVT);
		}
		return 0;
	} else if (usr == BCAST_PROTOCOL) {
		tipc_bcast_lock(l->net);
		tipc_link_bc_init_rcv(l->bc_rcvlink, hdr);
		tipc_bcast_unlock(l->net);
	}
drop:
	kfree_skb(skb);
	return 0;
}

static bool tipc_link_release_pkts(struct tipc_link *l, u16 acked)
{
	bool released = false;
	struct sk_buff *skb, *tmp;

	skb_queue_walk_safe(&l->transmq, skb, tmp) {
		if (more(buf_seqno(skb), acked))
			break;
		__skb_unlink(skb, &l->transmq);
		kfree_skb(skb);
		released = true;
	}
	return released;
}

/* tipc_link_build_state_msg: prepare link state message for transmission
 *
 * Note that sending of broadcast ack is coordinated among nodes, to reduce
 * risk of ack storms towards the sender
 */
int tipc_link_build_state_msg(struct tipc_link *l, struct sk_buff_head *xmitq)
{
	if (!l)
		return 0;

	/* Broadcast ACK must be sent via a unicast link => defer to caller */
	if (link_is_bc_rcvlink(l)) {
		if (((l->rcv_nxt ^ tipc_own_addr(l->net)) & 0xf) != 0xf)
			return 0;
		l->rcv_unacked = 0;

		/* Use snd_nxt to store peer's snd_nxt in broadcast rcv link */
		l->snd_nxt = l->rcv_nxt;
		return TIPC_LINK_SND_STATE;
	}

	/* Unicast ACK */
	l->rcv_unacked = 0;
	l->stats.sent_acks++;
	tipc_link_build_proto_msg(l, STATE_MSG, 0, 0, 0, 0, xmitq);
	return 0;
}

/* tipc_link_build_reset_msg: prepare link RESET or ACTIVATE message
 */
void tipc_link_build_reset_msg(struct tipc_link *l, struct sk_buff_head *xmitq)
{
	int mtyp = RESET_MSG;
	struct sk_buff *skb;

	if (l->state == LINK_ESTABLISHING)
		mtyp = ACTIVATE_MSG;

	tipc_link_build_proto_msg(l, mtyp, 0, 0, 0, 0, xmitq);

	/* Inform peer that this endpoint is going down if applicable */
	skb = skb_peek_tail(xmitq);
	if (skb && (l->state == LINK_RESET))
		msg_set_peer_stopping(buf_msg(skb), 1);
}

/* tipc_link_build_nack_msg: prepare link nack message for transmission
 * Note that sending of broadcast NACK is coordinated among nodes, to
 * reduce the risk of NACK storms towards the sender
 */
static int tipc_link_build_nack_msg(struct tipc_link *l,
				    struct sk_buff_head *xmitq)
{
	u32 def_cnt = ++l->stats.deferred_recv;
	int match1, match2;

	if (link_is_bc_rcvlink(l)) {
		match1 = def_cnt & 0xf;
		match2 = tipc_own_addr(l->net) & 0xf;
		if (match1 == match2)
			return TIPC_LINK_SND_STATE;
		return 0;
	}

	if ((skb_queue_len(&l->deferdq) == 1) || !(def_cnt % TIPC_NACK_INTV))
		tipc_link_build_proto_msg(l, STATE_MSG, 0, 0, 0, 0, xmitq);
	return 0;
}

/* tipc_link_rcv - process TIPC packets/messages arriving from off-node
 * @l: the link that should handle the message
 * @skb: TIPC packet
 * @xmitq: queue to place packets to be sent after this call
 */
int tipc_link_rcv(struct tipc_link *l, struct sk_buff *skb,
		  struct sk_buff_head *xmitq)
{
	struct sk_buff_head *defq = &l->deferdq;
	struct tipc_msg *hdr;
	u16 seqno, rcv_nxt, win_lim;
	int rc = 0;

	do {
		hdr = buf_msg(skb);
		seqno = msg_seqno(hdr);
		rcv_nxt = l->rcv_nxt;
		win_lim = rcv_nxt + TIPC_MAX_LINK_WIN;

		/* Verify and update link state */
		if (unlikely(msg_user(hdr) == LINK_PROTOCOL))
			return tipc_link_proto_rcv(l, skb, xmitq);

		if (unlikely(!link_is_up(l))) {
			if (l->state == LINK_ESTABLISHING)
				rc = TIPC_LINK_UP_EVT;
			goto drop;
		}

		/* Don't send probe at next timeout expiration */
		l->silent_intv_cnt = 0;

		/* Drop if outside receive window */
		if (unlikely(less(seqno, rcv_nxt) || more(seqno, win_lim))) {
			l->stats.duplicates++;
			goto drop;
		}

		/* Forward queues and wake up waiting users */
		if (likely(tipc_link_release_pkts(l, msg_ack(hdr)))) {
			tipc_link_advance_backlog(l, xmitq);
			if (unlikely(!skb_queue_empty(&l->wakeupq)))
				link_prepare_wakeup(l);
		}

		/* Defer delivery if sequence gap */
		if (unlikely(seqno != rcv_nxt)) {
			__tipc_skb_queue_sorted(defq, seqno, skb);
			rc |= tipc_link_build_nack_msg(l, xmitq);
			break;
		}

		/* Deliver packet */
		l->rcv_nxt++;
		l->stats.recv_pkts++;
		if (!tipc_data_input(l, skb, l->inputq))
			rc |= tipc_link_input(l, skb, l->inputq);
		if (unlikely(++l->rcv_unacked >= TIPC_MIN_LINK_WIN))
			rc |= tipc_link_build_state_msg(l, xmitq);
		if (unlikely(rc & ~TIPC_LINK_SND_STATE))
			break;
	} while ((skb = __skb_dequeue(defq)));

	return rc;
drop:
	kfree_skb(skb);
	return rc;
}

static void tipc_link_build_proto_msg(struct tipc_link *l, int mtyp, bool probe,
				      u16 rcvgap, int tolerance, int priority,
				      struct sk_buff_head *xmitq)
{
	struct tipc_link *bcl = l->bc_rcvlink;
	struct sk_buff *skb;
	struct tipc_msg *hdr;
	struct sk_buff_head *dfq = &l->deferdq;
	bool node_up = link_is_up(bcl);
	struct tipc_mon_state *mstate = &l->mon_state;
	int dlen = 0;
	void *data;

	/* Don't send protocol message during reset or link failover */
	if (tipc_link_is_blocked(l))
		return;

	if (!tipc_link_is_up(l) && (mtyp == STATE_MSG))
		return;

	if (!skb_queue_empty(dfq))
		rcvgap = buf_seqno(skb_peek(dfq)) - l->rcv_nxt;

	skb = tipc_msg_create(LINK_PROTOCOL, mtyp, INT_H_SIZE,
			      tipc_max_domain_size, l->addr,
			      tipc_own_addr(l->net), 0, 0, 0);
	if (!skb)
		return;

	hdr = buf_msg(skb);
	data = msg_data(hdr);
	msg_set_session(hdr, l->session);
	msg_set_bearer_id(hdr, l->bearer_id);
	msg_set_net_plane(hdr, l->net_plane);
	msg_set_next_sent(hdr, l->snd_nxt);
	msg_set_ack(hdr, l->rcv_nxt - 1);
	msg_set_bcast_ack(hdr, bcl->rcv_nxt - 1);
	msg_set_bc_ack_invalid(hdr, !node_up);
	msg_set_last_bcast(hdr, l->bc_sndlink->snd_nxt - 1);
	msg_set_link_tolerance(hdr, tolerance);
	msg_set_linkprio(hdr, priority);
	msg_set_redundant_link(hdr, node_up);
	msg_set_seq_gap(hdr, 0);
	msg_set_seqno(hdr, l->snd_nxt + U16_MAX / 2);

	if (mtyp == STATE_MSG) {
		msg_set_seq_gap(hdr, rcvgap);
		msg_set_bc_gap(hdr, link_bc_rcv_gap(bcl));
		msg_set_probe(hdr, probe);
		tipc_mon_prep(l->net, data, &dlen, mstate, l->bearer_id);
		msg_set_size(hdr, INT_H_SIZE + dlen);
		skb_trim(skb, INT_H_SIZE + dlen);
		l->stats.sent_states++;
		l->rcv_unacked = 0;
	} else {
		/* RESET_MSG or ACTIVATE_MSG */
		msg_set_max_pkt(hdr, l->advertised_mtu);
		strcpy(data, l->if_name);
		msg_set_size(hdr, INT_H_SIZE + TIPC_MAX_IF_NAME);
		skb_trim(skb, INT_H_SIZE + TIPC_MAX_IF_NAME);
	}
	if (probe)
		l->stats.sent_probes++;
	if (rcvgap)
		l->stats.sent_nacks++;
	skb->priority = TC_PRIO_CONTROL;
	__skb_queue_tail(xmitq, skb);
}

/* tipc_link_tnl_prepare(): prepare and return a list of tunnel packets
 * with contents of the link's transmit and backlog queues.
 */
void tipc_link_tnl_prepare(struct tipc_link *l, struct tipc_link *tnl,
			   int mtyp, struct sk_buff_head *xmitq)
{
	struct sk_buff *skb, *tnlskb;
	struct tipc_msg *hdr, tnlhdr;
	struct sk_buff_head *queue = &l->transmq;
	struct sk_buff_head tmpxq, tnlq;
	u16 pktlen, pktcnt, seqno = l->snd_nxt;

	if (!tnl)
		return;

	skb_queue_head_init(&tnlq);
	skb_queue_head_init(&tmpxq);

	/* At least one packet required for safe algorithm => add dummy */
	skb = tipc_msg_create(TIPC_LOW_IMPORTANCE, TIPC_DIRECT_MSG,
			      BASIC_H_SIZE, 0, l->addr, tipc_own_addr(l->net),
			      0, 0, TIPC_ERR_NO_PORT);
	if (!skb) {
		pr_warn("%sunable to create tunnel packet\n", link_co_err);
		return;
	}
	skb_queue_tail(&tnlq, skb);
	tipc_link_xmit(l, &tnlq, &tmpxq);
	__skb_queue_purge(&tmpxq);

	/* Initialize reusable tunnel packet header */
	tipc_msg_init(tipc_own_addr(l->net), &tnlhdr, TUNNEL_PROTOCOL,
		      mtyp, INT_H_SIZE, l->addr);
	pktcnt = skb_queue_len(&l->transmq) + skb_queue_len(&l->backlogq);
	msg_set_msgcnt(&tnlhdr, pktcnt);
	msg_set_bearer_id(&tnlhdr, l->peer_bearer_id);
tnl:
	/* Wrap each packet into a tunnel packet */
	skb_queue_walk(queue, skb) {
		hdr = buf_msg(skb);
		if (queue == &l->backlogq)
			msg_set_seqno(hdr, seqno++);
		pktlen = msg_size(hdr);
		msg_set_size(&tnlhdr, pktlen + INT_H_SIZE);
		tnlskb = tipc_buf_acquire(pktlen + INT_H_SIZE, GFP_ATOMIC);
		if (!tnlskb) {
			pr_warn("%sunable to send packet\n", link_co_err);
			return;
		}
		skb_copy_to_linear_data(tnlskb, &tnlhdr, INT_H_SIZE);
		skb_copy_to_linear_data_offset(tnlskb, INT_H_SIZE, hdr, pktlen);
		__skb_queue_tail(&tnlq, tnlskb);
	}
	if (queue != &l->backlogq) {
		queue = &l->backlogq;
		goto tnl;
	}

	tipc_link_xmit(tnl, &tnlq, xmitq);

	if (mtyp == FAILOVER_MSG) {
		tnl->drop_point = l->rcv_nxt;
		tnl->failover_reasm_skb = l->reasm_buf;
		l->reasm_buf = NULL;
	}
}

/* tipc_link_proto_rcv(): receive link level protocol message :
 * Note that network plane id propagates through the network, and may
 * change at any time. The node with lowest numerical id determines
 * network plane
 */
static int tipc_link_proto_rcv(struct tipc_link *l, struct sk_buff *skb,
			       struct sk_buff_head *xmitq)
{
	struct tipc_msg *hdr = buf_msg(skb);
	u16 rcvgap = 0;
	u16 ack = msg_ack(hdr);
	u16 gap = msg_seq_gap(hdr);
	u16 peers_snd_nxt =  msg_next_sent(hdr);
	u16 peers_tol = msg_link_tolerance(hdr);
	u16 peers_prio = msg_linkprio(hdr);
	u16 rcv_nxt = l->rcv_nxt;
	u16 dlen = msg_data_sz(hdr);
	int mtyp = msg_type(hdr);
	void *data;
	char *if_name;
	int rc = 0;

	if (tipc_link_is_blocked(l) || !xmitq)
		goto exit;

	if (tipc_own_addr(l->net) > msg_prevnode(hdr))
		l->net_plane = msg_net_plane(hdr);

	skb_linearize(skb);
	hdr = buf_msg(skb);
	data = msg_data(hdr);

	switch (mtyp) {
	case RESET_MSG:

		/* Ignore duplicate RESET with old session number */
		if ((less_eq(msg_session(hdr), l->peer_session)) &&
		    (l->peer_session != ANY_SESSION))
			break;
		/* fall thru' */

	case ACTIVATE_MSG:

		/* Complete own link name with peer's interface name */
		if_name =  strrchr(l->name, ':') + 1;
		if (sizeof(l->name) - (if_name - l->name) <= TIPC_MAX_IF_NAME)
			break;
		if (msg_data_sz(hdr) < TIPC_MAX_IF_NAME)
			break;
		strncpy(if_name, data, TIPC_MAX_IF_NAME);

		/* Update own tolerance if peer indicates a non-zero value */
		if (in_range(peers_tol, TIPC_MIN_LINK_TOL, TIPC_MAX_LINK_TOL))
			l->tolerance = peers_tol;

		/* Update own priority if peer's priority is higher */
		if (in_range(peers_prio, l->priority + 1, TIPC_MAX_LINK_PRI))
			l->priority = peers_prio;

		/* ACTIVATE_MSG serves as PEER_RESET if link is already down */
		if (msg_peer_stopping(hdr))
			rc = tipc_link_fsm_evt(l, LINK_FAILURE_EVT);
		else if ((mtyp == RESET_MSG) || !link_is_up(l))
			rc = tipc_link_fsm_evt(l, LINK_PEER_RESET_EVT);

		/* ACTIVATE_MSG takes up link if it was already locally reset */
		if ((mtyp == ACTIVATE_MSG) && (l->state == LINK_ESTABLISHING))
			rc = TIPC_LINK_UP_EVT;

		l->peer_session = msg_session(hdr);
		l->peer_bearer_id = msg_bearer_id(hdr);
		if (l->mtu > msg_max_pkt(hdr))
			l->mtu = msg_max_pkt(hdr);
		break;

	case STATE_MSG:

		/* Update own tolerance if peer indicates a non-zero value */
		if (in_range(peers_tol, TIPC_MIN_LINK_TOL, TIPC_MAX_LINK_TOL))
			l->tolerance = peers_tol;

		/* Update own prio if peer indicates a different value */
		if ((peers_prio != l->priority) &&
		    in_range(peers_prio, 1, TIPC_MAX_LINK_PRI)) {
			l->priority = peers_prio;
			rc = tipc_link_fsm_evt(l, LINK_FAILURE_EVT);
		}

		l->silent_intv_cnt = 0;
		l->stats.recv_states++;
		if (msg_probe(hdr))
			l->stats.recv_probes++;

		if (!link_is_up(l)) {
			if (l->state == LINK_ESTABLISHING)
				rc = TIPC_LINK_UP_EVT;
			break;
		}
		tipc_mon_rcv(l->net, data, dlen, l->addr,
			     &l->mon_state, l->bearer_id);

		/* Send NACK if peer has sent pkts we haven't received yet */
		if (more(peers_snd_nxt, rcv_nxt) && !tipc_link_is_synching(l))
			rcvgap = peers_snd_nxt - l->rcv_nxt;
		if (rcvgap || (msg_probe(hdr)))
			tipc_link_build_proto_msg(l, STATE_MSG, 0, rcvgap,
						  0, 0, xmitq);
		tipc_link_release_pkts(l, ack);

		/* If NACK, retransmit will now start at right position */
		if (gap) {
			rc = tipc_link_retrans(l, ack + 1, ack + gap, xmitq);
			l->stats.recv_nacks++;
		}

		tipc_link_advance_backlog(l, xmitq);
		if (unlikely(!skb_queue_empty(&l->wakeupq)))
			link_prepare_wakeup(l);
	}
exit:
	kfree_skb(skb);
	return rc;
}

/* tipc_link_build_bc_proto_msg() - create broadcast protocol message
 */
static bool tipc_link_build_bc_proto_msg(struct tipc_link *l, bool bcast,
					 u16 peers_snd_nxt,
					 struct sk_buff_head *xmitq)
{
	struct sk_buff *skb;
	struct tipc_msg *hdr;
	struct sk_buff *dfrd_skb = skb_peek(&l->deferdq);
	u16 ack = l->rcv_nxt - 1;
	u16 gap_to = peers_snd_nxt - 1;

	skb = tipc_msg_create(BCAST_PROTOCOL, STATE_MSG, INT_H_SIZE,
			      0, l->addr, tipc_own_addr(l->net), 0, 0, 0);
	if (!skb)
		return false;
	hdr = buf_msg(skb);
	msg_set_last_bcast(hdr, l->bc_sndlink->snd_nxt - 1);
	msg_set_bcast_ack(hdr, ack);
	msg_set_bcgap_after(hdr, ack);
	if (dfrd_skb)
		gap_to = buf_seqno(dfrd_skb) - 1;
	msg_set_bcgap_to(hdr, gap_to);
	msg_set_non_seq(hdr, bcast);
	__skb_queue_tail(xmitq, skb);
	return true;
}

/* tipc_link_build_bc_init_msg() - synchronize broadcast link endpoints.
 *
 * Give a newly added peer node the sequence number where it should
 * start receiving and acking broadcast packets.
 */
static void tipc_link_build_bc_init_msg(struct tipc_link *l,
					struct sk_buff_head *xmitq)
{
	struct sk_buff_head list;

	__skb_queue_head_init(&list);
	if (!tipc_link_build_bc_proto_msg(l->bc_rcvlink, false, 0, &list))
		return;
	msg_set_bc_ack_invalid(buf_msg(skb_peek(&list)), true);
	tipc_link_xmit(l, &list, xmitq);
}

/* tipc_link_bc_init_rcv - receive initial broadcast synch data from peer
 */
void tipc_link_bc_init_rcv(struct tipc_link *l, struct tipc_msg *hdr)
{
	int mtyp = msg_type(hdr);
	u16 peers_snd_nxt = msg_bc_snd_nxt(hdr);

	if (link_is_up(l))
		return;

	if (msg_user(hdr) == BCAST_PROTOCOL) {
		l->rcv_nxt = peers_snd_nxt;
		l->state = LINK_ESTABLISHED;
		return;
	}

	if (l->peer_caps & TIPC_BCAST_SYNCH)
		return;

	if (msg_peer_node_is_up(hdr))
		return;

	/* Compatibility: accept older, less safe initial synch data */
	if ((mtyp == RESET_MSG) || (mtyp == ACTIVATE_MSG))
		l->rcv_nxt = peers_snd_nxt;
}

/* link_bc_retr eval()- check if the indicated range can be retransmitted now
 * - Adjust permitted range if there is overlap with previous retransmission
 */
static bool link_bc_retr_eval(struct tipc_link *l, u16 *from, u16 *to)
{
	unsigned long elapsed = jiffies_to_msecs(jiffies - l->prev_retr);

	if (less(*to, *from))
		return false;

	/* New retransmission request */
	if ((elapsed > TIPC_BC_RETR_LIMIT) ||
	    less(*to, l->prev_from) || more(*from, l->prev_to)) {
		l->prev_from = *from;
		l->prev_to = *to;
		l->prev_retr = jiffies;
		return true;
	}

	/* Inside range of previous retransmit */
	if (!less(*from, l->prev_from) && !more(*to, l->prev_to))
		return false;

	/* Fully or partially outside previous range => exclude overlap */
	if (less(*from, l->prev_from)) {
		*to = l->prev_from - 1;
		l->prev_from = *from;
	}
	if (more(*to, l->prev_to)) {
		*from = l->prev_to + 1;
		l->prev_to = *to;
	}
	l->prev_retr = jiffies;
	return true;
}

/* tipc_link_bc_sync_rcv - update rcv link according to peer's send state
 */
int tipc_link_bc_sync_rcv(struct tipc_link *l, struct tipc_msg *hdr,
			  struct sk_buff_head *xmitq)
{
	struct tipc_link *snd_l = l->bc_sndlink;
	u16 peers_snd_nxt = msg_bc_snd_nxt(hdr);
	u16 from = msg_bcast_ack(hdr) + 1;
	u16 to = from + msg_bc_gap(hdr) - 1;
	int rc = 0;

	if (!link_is_up(l))
		return rc;

	if (!msg_peer_node_is_up(hdr))
		return rc;

	/* Open when peer ackowledges our bcast init msg (pkt #1) */
	if (msg_ack(hdr))
		l->bc_peer_is_up = true;

	if (!l->bc_peer_is_up)
		return rc;

	l->stats.recv_nacks++;

	/* Ignore if peers_snd_nxt goes beyond receive window */
	if (more(peers_snd_nxt, l->rcv_nxt + l->window))
		return rc;

	if (link_bc_retr_eval(snd_l, &from, &to))
		rc = tipc_link_retrans(snd_l, from, to, xmitq);

	l->snd_nxt = peers_snd_nxt;
	if (link_bc_rcv_gap(l))
		rc |= TIPC_LINK_SND_STATE;

	/* Return now if sender supports nack via STATE messages */
	if (l->peer_caps & TIPC_BCAST_STATE_NACK)
		return rc;

	/* Otherwise, be backwards compatible */

	if (!more(peers_snd_nxt, l->rcv_nxt)) {
		l->nack_state = BC_NACK_SND_CONDITIONAL;
		return 0;
	}

	/* Don't NACK if one was recently sent or peeked */
	if (l->nack_state == BC_NACK_SND_SUPPRESS) {
		l->nack_state = BC_NACK_SND_UNCONDITIONAL;
		return 0;
	}

	/* Conditionally delay NACK sending until next synch rcv */
	if (l->nack_state == BC_NACK_SND_CONDITIONAL) {
		l->nack_state = BC_NACK_SND_UNCONDITIONAL;
		if ((peers_snd_nxt - l->rcv_nxt) < TIPC_MIN_LINK_WIN)
			return 0;
	}

	/* Send NACK now but suppress next one */
	tipc_link_build_bc_proto_msg(l, true, peers_snd_nxt, xmitq);
	l->nack_state = BC_NACK_SND_SUPPRESS;
	return 0;
}

void tipc_link_bc_ack_rcv(struct tipc_link *l, u16 acked,
			  struct sk_buff_head *xmitq)
{
	struct sk_buff *skb, *tmp;
	struct tipc_link *snd_l = l->bc_sndlink;

	if (!link_is_up(l) || !l->bc_peer_is_up)
		return;

	if (!more(acked, l->acked))
		return;

	/* Skip over packets peer has already acked */
	skb_queue_walk(&snd_l->transmq, skb) {
		if (more(buf_seqno(skb), l->acked))
			break;
	}

	/* Update/release the packets peer is acking now */
	skb_queue_walk_from_safe(&snd_l->transmq, skb, tmp) {
		if (more(buf_seqno(skb), acked))
			break;
		if (!--TIPC_SKB_CB(skb)->ackers) {
			__skb_unlink(skb, &snd_l->transmq);
			kfree_skb(skb);
		}
	}
	l->acked = acked;
	tipc_link_advance_backlog(snd_l, xmitq);
	if (unlikely(!skb_queue_empty(&snd_l->wakeupq)))
		link_prepare_wakeup(snd_l);
}

/* tipc_link_bc_nack_rcv(): receive broadcast nack message
 * This function is here for backwards compatibility, since
 * no BCAST_PROTOCOL/STATE messages occur from TIPC v2.5.
 */
int tipc_link_bc_nack_rcv(struct tipc_link *l, struct sk_buff *skb,
			  struct sk_buff_head *xmitq)
{
	struct tipc_msg *hdr = buf_msg(skb);
	u32 dnode = msg_destnode(hdr);
	int mtyp = msg_type(hdr);
	u16 acked = msg_bcast_ack(hdr);
	u16 from = acked + 1;
	u16 to = msg_bcgap_to(hdr);
	u16 peers_snd_nxt = to + 1;
	int rc = 0;

	kfree_skb(skb);

	if (!tipc_link_is_up(l) || !l->bc_peer_is_up)
		return 0;

	if (mtyp != STATE_MSG)
		return 0;

	if (dnode == tipc_own_addr(l->net)) {
		tipc_link_bc_ack_rcv(l, acked, xmitq);
		rc = tipc_link_retrans(l->bc_sndlink, from, to, xmitq);
		l->stats.recv_nacks++;
		return rc;
	}

	/* Msg for other node => suppress own NACK at next sync if applicable */
	if (more(peers_snd_nxt, l->rcv_nxt) && !less(l->rcv_nxt, from))
		l->nack_state = BC_NACK_SND_SUPPRESS;

	return 0;
}

void tipc_link_set_queue_limits(struct tipc_link *l, u32 win)
{
	int max_bulk = TIPC_MAX_PUBLICATIONS / (l->mtu / ITEM_SIZE);

	l->window = win;
	l->backlog[TIPC_LOW_IMPORTANCE].limit      = max_t(u16, 50, win);
	l->backlog[TIPC_MEDIUM_IMPORTANCE].limit   = max_t(u16, 100, win * 2);
	l->backlog[TIPC_HIGH_IMPORTANCE].limit     = max_t(u16, 150, win * 3);
	l->backlog[TIPC_CRITICAL_IMPORTANCE].limit = max_t(u16, 200, win * 4);
	l->backlog[TIPC_SYSTEM_IMPORTANCE].limit   = max_bulk;
}

/**
 * link_reset_stats - reset link statistics
 * @l: pointer to link
 */
void tipc_link_reset_stats(struct tipc_link *l)
{
	memset(&l->stats, 0, sizeof(l->stats));
}

static void link_print(struct tipc_link *l, const char *str)
{
	struct sk_buff *hskb = skb_peek(&l->transmq);
	u16 head = hskb ? msg_seqno(buf_msg(hskb)) : l->snd_nxt - 1;
	u16 tail = l->snd_nxt - 1;

	pr_info("%s Link <%s> state %x\n", str, l->name, l->state);
	pr_info("XMTQ: %u [%u-%u], BKLGQ: %u, SNDNX: %u, RCVNX: %u\n",
		skb_queue_len(&l->transmq), head, tail,
		skb_queue_len(&l->backlogq), l->snd_nxt, l->rcv_nxt);
}

/* Parse and validate nested (link) properties valid for media, bearer and link
 */
int tipc_nl_parse_link_prop(struct nlattr *prop, struct nlattr *props[])
{
	int err;

	err = nla_parse_nested(props, TIPC_NLA_PROP_MAX, prop,
			       tipc_nl_prop_policy);
	if (err)
		return err;

	if (props[TIPC_NLA_PROP_PRIO]) {
		u32 prio;

		prio = nla_get_u32(props[TIPC_NLA_PROP_PRIO]);
		if (prio > TIPC_MAX_LINK_PRI)
			return -EINVAL;
	}

	if (props[TIPC_NLA_PROP_TOL]) {
		u32 tol;

		tol = nla_get_u32(props[TIPC_NLA_PROP_TOL]);
		if ((tol < TIPC_MIN_LINK_TOL) || (tol > TIPC_MAX_LINK_TOL))
			return -EINVAL;
	}

	if (props[TIPC_NLA_PROP_WIN]) {
		u32 win;

		win = nla_get_u32(props[TIPC_NLA_PROP_WIN]);
		if ((win < TIPC_MIN_LINK_WIN) || (win > TIPC_MAX_LINK_WIN))
			return -EINVAL;
	}

	return 0;
}

static int __tipc_nl_add_stats(struct sk_buff *skb, struct tipc_stats *s)
{
	int i;
	struct nlattr *stats;

	struct nla_map {
		u32 key;
		u32 val;
	};

	struct nla_map map[] = {
		{TIPC_NLA_STATS_RX_INFO, 0},
		{TIPC_NLA_STATS_RX_FRAGMENTS, s->recv_fragments},
		{TIPC_NLA_STATS_RX_FRAGMENTED, s->recv_fragmented},
		{TIPC_NLA_STATS_RX_BUNDLES, s->recv_bundles},
		{TIPC_NLA_STATS_RX_BUNDLED, s->recv_bundled},
		{TIPC_NLA_STATS_TX_INFO, 0},
		{TIPC_NLA_STATS_TX_FRAGMENTS, s->sent_fragments},
		{TIPC_NLA_STATS_TX_FRAGMENTED, s->sent_fragmented},
		{TIPC_NLA_STATS_TX_BUNDLES, s->sent_bundles},
		{TIPC_NLA_STATS_TX_BUNDLED, s->sent_bundled},
		{TIPC_NLA_STATS_MSG_PROF_TOT, (s->msg_length_counts) ?
			s->msg_length_counts : 1},
		{TIPC_NLA_STATS_MSG_LEN_CNT, s->msg_length_counts},
		{TIPC_NLA_STATS_MSG_LEN_TOT, s->msg_lengths_total},
		{TIPC_NLA_STATS_MSG_LEN_P0, s->msg_length_profile[0]},
		{TIPC_NLA_STATS_MSG_LEN_P1, s->msg_length_profile[1]},
		{TIPC_NLA_STATS_MSG_LEN_P2, s->msg_length_profile[2]},
		{TIPC_NLA_STATS_MSG_LEN_P3, s->msg_length_profile[3]},
		{TIPC_NLA_STATS_MSG_LEN_P4, s->msg_length_profile[4]},
		{TIPC_NLA_STATS_MSG_LEN_P5, s->msg_length_profile[5]},
		{TIPC_NLA_STATS_MSG_LEN_P6, s->msg_length_profile[6]},
		{TIPC_NLA_STATS_RX_STATES, s->recv_states},
		{TIPC_NLA_STATS_RX_PROBES, s->recv_probes},
		{TIPC_NLA_STATS_RX_NACKS, s->recv_nacks},
		{TIPC_NLA_STATS_RX_DEFERRED, s->deferred_recv},
		{TIPC_NLA_STATS_TX_STATES, s->sent_states},
		{TIPC_NLA_STATS_TX_PROBES, s->sent_probes},
		{TIPC_NLA_STATS_TX_NACKS, s->sent_nacks},
		{TIPC_NLA_STATS_TX_ACKS, s->sent_acks},
		{TIPC_NLA_STATS_RETRANSMITTED, s->retransmitted},
		{TIPC_NLA_STATS_DUPLICATES, s->duplicates},
		{TIPC_NLA_STATS_LINK_CONGS, s->link_congs},
		{TIPC_NLA_STATS_MAX_QUEUE, s->max_queue_sz},
		{TIPC_NLA_STATS_AVG_QUEUE, s->queue_sz_counts ?
			(s->accu_queue_sz / s->queue_sz_counts) : 0}
	};

	stats = nla_nest_start(skb, TIPC_NLA_LINK_STATS);
	if (!stats)
		return -EMSGSIZE;

	for (i = 0; i <  ARRAY_SIZE(map); i++)
		if (nla_put_u32(skb, map[i].key, map[i].val))
			goto msg_full;

	nla_nest_end(skb, stats);

	return 0;
msg_full:
	nla_nest_cancel(skb, stats);

	return -EMSGSIZE;
}

/* Caller should hold appropriate locks to protect the link */
int __tipc_nl_add_link(struct net *net, struct tipc_nl_msg *msg,
		       struct tipc_link *link, int nlflags)
{
	int err;
	void *hdr;
	struct nlattr *attrs;
	struct nlattr *prop;
	struct tipc_net *tn = net_generic(net, tipc_net_id);

	hdr = genlmsg_put(msg->skb, msg->portid, msg->seq, &tipc_genl_family,
			  nlflags, TIPC_NL_LINK_GET);
	if (!hdr)
		return -EMSGSIZE;

	attrs = nla_nest_start(msg->skb, TIPC_NLA_LINK);
	if (!attrs)
		goto msg_full;

	if (nla_put_string(msg->skb, TIPC_NLA_LINK_NAME, link->name))
		goto attr_msg_full;
	if (nla_put_u32(msg->skb, TIPC_NLA_LINK_DEST,
			tipc_cluster_mask(tn->own_addr)))
		goto attr_msg_full;
	if (nla_put_u32(msg->skb, TIPC_NLA_LINK_MTU, link->mtu))
		goto attr_msg_full;
	if (nla_put_u32(msg->skb, TIPC_NLA_LINK_RX, link->stats.recv_pkts))
		goto attr_msg_full;
	if (nla_put_u32(msg->skb, TIPC_NLA_LINK_TX, link->stats.sent_pkts))
		goto attr_msg_full;

	if (tipc_link_is_up(link))
		if (nla_put_flag(msg->skb, TIPC_NLA_LINK_UP))
			goto attr_msg_full;
	if (link->active)
		if (nla_put_flag(msg->skb, TIPC_NLA_LINK_ACTIVE))
			goto attr_msg_full;

	prop = nla_nest_start(msg->skb, TIPC_NLA_LINK_PROP);
	if (!prop)
		goto attr_msg_full;
	if (nla_put_u32(msg->skb, TIPC_NLA_PROP_PRIO, link->priority))
		goto prop_msg_full;
	if (nla_put_u32(msg->skb, TIPC_NLA_PROP_TOL, link->tolerance))
		goto prop_msg_full;
	if (nla_put_u32(msg->skb, TIPC_NLA_PROP_WIN,
			link->window))
		goto prop_msg_full;
	if (nla_put_u32(msg->skb, TIPC_NLA_PROP_PRIO, link->priority))
		goto prop_msg_full;
	nla_nest_end(msg->skb, prop);

	err = __tipc_nl_add_stats(msg->skb, &link->stats);
	if (err)
		goto attr_msg_full;

	nla_nest_end(msg->skb, attrs);
	genlmsg_end(msg->skb, hdr);

	return 0;

prop_msg_full:
	nla_nest_cancel(msg->skb, prop);
attr_msg_full:
	nla_nest_cancel(msg->skb, attrs);
msg_full:
	genlmsg_cancel(msg->skb, hdr);

	return -EMSGSIZE;
}

static int __tipc_nl_add_bc_link_stat(struct sk_buff *skb,
				      struct tipc_stats *stats)
{
	int i;
	struct nlattr *nest;

	struct nla_map {
		__u32 key;
		__u32 val;
	};

	struct nla_map map[] = {
		{TIPC_NLA_STATS_RX_INFO, stats->recv_pkts},
		{TIPC_NLA_STATS_RX_FRAGMENTS, stats->recv_fragments},
		{TIPC_NLA_STATS_RX_FRAGMENTED, stats->recv_fragmented},
		{TIPC_NLA_STATS_RX_BUNDLES, stats->recv_bundles},
		{TIPC_NLA_STATS_RX_BUNDLED, stats->recv_bundled},
		{TIPC_NLA_STATS_TX_INFO, stats->sent_pkts},
		{TIPC_NLA_STATS_TX_FRAGMENTS, stats->sent_fragments},
		{TIPC_NLA_STATS_TX_FRAGMENTED, stats->sent_fragmented},
		{TIPC_NLA_STATS_TX_BUNDLES, stats->sent_bundles},
		{TIPC_NLA_STATS_TX_BUNDLED, stats->sent_bundled},
		{TIPC_NLA_STATS_RX_NACKS, stats->recv_nacks},
		{TIPC_NLA_STATS_RX_DEFERRED, stats->deferred_recv},
		{TIPC_NLA_STATS_TX_NACKS, stats->sent_nacks},
		{TIPC_NLA_STATS_TX_ACKS, stats->sent_acks},
		{TIPC_NLA_STATS_RETRANSMITTED, stats->retransmitted},
		{TIPC_NLA_STATS_DUPLICATES, stats->duplicates},
		{TIPC_NLA_STATS_LINK_CONGS, stats->link_congs},
		{TIPC_NLA_STATS_MAX_QUEUE, stats->max_queue_sz},
		{TIPC_NLA_STATS_AVG_QUEUE, stats->queue_sz_counts ?
			(stats->accu_queue_sz / stats->queue_sz_counts) : 0}
	};

	nest = nla_nest_start(skb, TIPC_NLA_LINK_STATS);
	if (!nest)
		return -EMSGSIZE;

	for (i = 0; i <  ARRAY_SIZE(map); i++)
		if (nla_put_u32(skb, map[i].key, map[i].val))
			goto msg_full;

	nla_nest_end(skb, nest);

	return 0;
msg_full:
	nla_nest_cancel(skb, nest);

	return -EMSGSIZE;
}

int tipc_nl_add_bc_link(struct net *net, struct tipc_nl_msg *msg)
{
	int err;
	void *hdr;
	struct nlattr *attrs;
	struct nlattr *prop;
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	struct tipc_link *bcl = tn->bcl;

	if (!bcl)
		return 0;

	tipc_bcast_lock(net);

	hdr = genlmsg_put(msg->skb, msg->portid, msg->seq, &tipc_genl_family,
			  NLM_F_MULTI, TIPC_NL_LINK_GET);
	if (!hdr) {
		tipc_bcast_unlock(net);
		return -EMSGSIZE;
	}

	attrs = nla_nest_start(msg->skb, TIPC_NLA_LINK);
	if (!attrs)
		goto msg_full;

	/* The broadcast link is always up */
	if (nla_put_flag(msg->skb, TIPC_NLA_LINK_UP))
		goto attr_msg_full;

	if (nla_put_flag(msg->skb, TIPC_NLA_LINK_BROADCAST))
		goto attr_msg_full;
	if (nla_put_string(msg->skb, TIPC_NLA_LINK_NAME, bcl->name))
		goto attr_msg_full;
	if (nla_put_u32(msg->skb, TIPC_NLA_LINK_RX, 0))
		goto attr_msg_full;
	if (nla_put_u32(msg->skb, TIPC_NLA_LINK_TX, 0))
		goto attr_msg_full;

	prop = nla_nest_start(msg->skb, TIPC_NLA_LINK_PROP);
	if (!prop)
		goto attr_msg_full;
	if (nla_put_u32(msg->skb, TIPC_NLA_PROP_WIN, bcl->window))
		goto prop_msg_full;
	nla_nest_end(msg->skb, prop);

	err = __tipc_nl_add_bc_link_stat(msg->skb, &bcl->stats);
	if (err)
		goto attr_msg_full;

	tipc_bcast_unlock(net);
	nla_nest_end(msg->skb, attrs);
	genlmsg_end(msg->skb, hdr);

	return 0;

prop_msg_full:
	nla_nest_cancel(msg->skb, prop);
attr_msg_full:
	nla_nest_cancel(msg->skb, attrs);
msg_full:
	tipc_bcast_unlock(net);
	genlmsg_cancel(msg->skb, hdr);

	return -EMSGSIZE;
}

void tipc_link_set_tolerance(struct tipc_link *l, u32 tol,
			     struct sk_buff_head *xmitq)
{
	l->tolerance = tol;
	tipc_link_build_proto_msg(l, STATE_MSG, 0, 0, tol, 0, xmitq);
}

void tipc_link_set_prio(struct tipc_link *l, u32 prio,
			struct sk_buff_head *xmitq)
{
	l->priority = prio;
	tipc_link_build_proto_msg(l, STATE_MSG, 0, 0, 0, prio, xmitq);
}

void tipc_link_set_abort_limit(struct tipc_link *l, u32 limit)
{
	l->abort_limit = limit;
>>>>>>> v4.9.227
}
