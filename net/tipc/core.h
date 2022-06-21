/*
 * net/tipc/core.h: Include file for TIPC global declarations
 *
 * Copyright (c) 2005-2006, 2013 Ericsson AB
 * Copyright (c) 2005-2007, 2010-2013, Wind River Systems
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

#ifndef _TIPC_CORE_H
#define _TIPC_CORE_H

<<<<<<< HEAD
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/tipc.h>
#include <linux/tipc_config.h>
=======
#include <linux/tipc.h>
#include <linux/tipc_config.h>
#include <linux/tipc_netlink.h>
>>>>>>> v4.9.227
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/atomic.h>
#include <asm/hardirq.h>
#include <linux/netdevice.h>
#include <linux/in.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/rtnetlink.h>
#include <linux/etherdevice.h>
<<<<<<< HEAD

#define TIPC_MOD_VER "2.0.0"

#define ULTRA_STRING_MAX_LEN	32768
#define TIPC_MAX_SUBSCRIPTIONS	65535
#define TIPC_MAX_PUBLICATIONS	65535

struct tipc_msg;	/* msg.h */

int tipc_snprintf(char *buf, int len, const char *fmt, ...);

/*
 * TIPC-specific error codes
 */
#define ELINKCONG EAGAIN	/* link congestion <=> resource unavailable */

/*
 * Global configuration variables
 */
extern u32 tipc_own_addr __read_mostly;
extern int tipc_max_ports __read_mostly;
=======
#include <net/netns/generic.h>
#include <linux/rhashtable.h>

struct tipc_node;
struct tipc_bearer;
struct tipc_bc_base;
struct tipc_link;
struct tipc_name_table;
struct tipc_server;
struct tipc_monitor;

#define TIPC_MOD_VER "2.0.0"

#define NODE_HTABLE_SIZE       512
#define MAX_BEARERS	         3
#define TIPC_DEF_MON_THRESHOLD  32

>>>>>>> v4.9.227
extern int tipc_net_id __read_mostly;
extern int sysctl_tipc_rmem[3] __read_mostly;
extern int sysctl_tipc_named_timeout __read_mostly;

<<<<<<< HEAD
/*
 * Other global variables
 */
extern int tipc_random __read_mostly;

/*
 * Routines available to privileged subsystems
 */
int tipc_netlink_start(void);
void tipc_netlink_stop(void);
int tipc_socket_init(void);
void tipc_socket_stop(void);
int tipc_sock_create_local(int type, struct socket **res);
void tipc_sock_release_local(struct socket *sock);
int tipc_sock_accept_local(struct socket *sock, struct socket **newsock,
			   int flags);
=======
struct tipc_net {
	u32 own_addr;
	int net_id;
	int random;

	/* Node table and node list */
	spinlock_t node_list_lock;
	struct hlist_head node_htable[NODE_HTABLE_SIZE];
	struct list_head node_list;
	u32 num_nodes;
	u32 num_links;

	/* Neighbor monitoring list */
	struct tipc_monitor *monitors[MAX_BEARERS];
	int mon_threshold;

	/* Bearer list */
	struct tipc_bearer __rcu *bearer_list[MAX_BEARERS + 1];

	/* Broadcast link */
	spinlock_t bclock;
	struct tipc_bc_base *bcbase;
	struct tipc_link *bcl;

	/* Socket hash table */
	struct rhashtable sk_rht;

	/* Name table */
	spinlock_t nametbl_lock;
	struct name_table *nametbl;

	/* Name dist queue */
	struct list_head dist_queue;

	/* Topology subscription server */
	struct tipc_server *topsrv;
	atomic_t subscription_count;
};

static inline struct tipc_net *tipc_net(struct net *net)
{
	return net_generic(net, tipc_net_id);
}

static inline int tipc_netid(struct net *net)
{
	return tipc_net(net)->net_id;
}

static inline struct list_head *tipc_nodes(struct net *net)
{
	return &tipc_net(net)->node_list;
}

static inline unsigned int tipc_hashfn(u32 addr)
{
	return addr & (NODE_HTABLE_SIZE - 1);
}

static inline u16 mod(u16 x)
{
	return x & 0xffffu;
}

static inline int less_eq(u16 left, u16 right)
{
	return mod(right - left) < 32768u;
}

static inline int more(u16 left, u16 right)
{
	return !less_eq(left, right);
}

static inline int less(u16 left, u16 right)
{
	return less_eq(left, right) && (mod(right) != mod(left));
}

static inline int in_range(u16 val, u16 min, u16 max)
{
	return !less(val, min) && !more(val, max);
}
>>>>>>> v4.9.227

#ifdef CONFIG_SYSCTL
int tipc_register_sysctl(void);
void tipc_unregister_sysctl(void);
#else
#define tipc_register_sysctl() 0
#define tipc_unregister_sysctl()
#endif
<<<<<<< HEAD

/*
 * TIPC timer code
 */
typedef void (*Handler) (unsigned long);

/**
 * k_init_timer - initialize a timer
 * @timer: pointer to timer structure
 * @routine: pointer to routine to invoke when timer expires
 * @argument: value to pass to routine when timer expires
 *
 * Timer must be initialized before use (and terminated when no longer needed).
 */
static inline void k_init_timer(struct timer_list *timer, Handler routine,
				unsigned long argument)
{
	setup_timer(timer, routine, argument);
}

/**
 * k_start_timer - start a timer
 * @timer: pointer to timer structure
 * @msec: time to delay (in ms)
 *
 * Schedules a previously initialized timer for later execution.
 * If timer is already running, the new timeout overrides the previous request.
 *
 * To ensure the timer doesn't expire before the specified delay elapses,
 * the amount of delay is rounded up when converting to the jiffies
 * then an additional jiffy is added to account for the fact that
 * the starting time may be in the middle of the current jiffy.
 */
static inline void k_start_timer(struct timer_list *timer, unsigned long msec)
{
	mod_timer(timer, jiffies + msecs_to_jiffies(msec) + 1);
}

/**
 * k_cancel_timer - cancel a timer
 * @timer: pointer to timer structure
 *
 * Cancels a previously initialized timer.
 * Can be called safely even if the timer is already inactive.
 *
 * WARNING: Must not be called when holding locks required by the timer's
 *          timeout routine, otherwise deadlock can occur on SMP systems!
 */
static inline void k_cancel_timer(struct timer_list *timer)
{
	del_timer_sync(timer);
}

/**
 * k_term_timer - terminate a timer
 * @timer: pointer to timer structure
 *
 * Prevents further use of a previously initialized timer.
 *
 * WARNING: Caller must ensure timer isn't currently running.
 *
 * (Do not "enhance" this routine to automatically cancel an active timer,
 * otherwise deadlock can arise when a timeout routine calls k_term_timer.)
 */
static inline void k_term_timer(struct timer_list *timer)
{
}

/*
 * TIPC message buffer code
 *
 * TIPC message buffer headroom reserves space for the worst-case
 * link-level device header (in case the message is sent off-node).
 *
 * Note: Headroom should be a multiple of 4 to ensure the TIPC header fields
 *       are word aligned for quicker access
 */
#define BUF_HEADROOM LL_MAX_HEADER

struct tipc_skb_cb {
	void *handle;
	struct sk_buff *tail;
	bool deferred;
	bool wakeup_pending;
	u16 chain_sz;
	u16 chain_imp;
};

#define TIPC_SKB_CB(__skb) ((struct tipc_skb_cb *)&((__skb)->cb[0]))

static inline struct tipc_msg *buf_msg(struct sk_buff *skb)
{
	return (struct tipc_msg *)skb->data;
}

struct sk_buff *tipc_buf_acquire(u32 size);

=======
>>>>>>> v4.9.227
#endif
