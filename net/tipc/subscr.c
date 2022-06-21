/*
 * net/tipc/subscr.c: TIPC network topology service
 *
 * Copyright (c) 2000-2006, Ericsson AB
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

#include "core.h"
#include "name_table.h"
#include "subscr.h"

/**
 * struct tipc_subscriber - TIPC network topology subscriber
<<<<<<< HEAD
 * @conid: connection identifier to server connecting to subscriber
 * @lock: control access to subscriber
 * @subscription_list: list of subscription objects for this subscriber
 */
struct tipc_subscriber {
	int conid;
	spinlock_t lock;
	struct list_head subscription_list;
};

static void subscr_conn_msg_event(int conid, struct sockaddr_tipc *addr,
				  void *usr_data, void *buf, size_t len);
static void *subscr_named_msg_event(int conid);
static void subscr_conn_shutdown_event(int conid, void *usr_data);

static atomic_t subscription_count = ATOMIC_INIT(0);

static struct sockaddr_tipc topsrv_addr __read_mostly = {
	.family			= AF_TIPC,
	.addrtype		= TIPC_ADDR_NAMESEQ,
	.addr.nameseq.type	= TIPC_TOP_SRV,
	.addr.nameseq.lower	= TIPC_TOP_SRV,
	.addr.nameseq.upper	= TIPC_TOP_SRV,
	.scope			= TIPC_NODE_SCOPE
};

static struct tipc_server topsrv __read_mostly = {
	.saddr			= &topsrv_addr,
	.imp			= TIPC_CRITICAL_IMPORTANCE,
	.type			= SOCK_SEQPACKET,
	.max_rcvbuf_size	= sizeof(struct tipc_subscr),
	.name			= "topology_server",
	.tipc_conn_recvmsg	= subscr_conn_msg_event,
	.tipc_conn_new		= subscr_named_msg_event,
	.tipc_conn_shutdown	= subscr_conn_shutdown_event,
};
=======
 * @kref: reference counter to tipc_subscription object
 * @conid: connection identifier to server connecting to subscriber
 * @lock: control access to subscriber
 * @subscrp_list: list of subscription objects for this subscriber
 */
struct tipc_subscriber {
	struct kref kref;
	int conid;
	spinlock_t lock;
	struct list_head subscrp_list;
};

static void tipc_subscrp_delete(struct tipc_subscription *sub);
static void tipc_subscrb_put(struct tipc_subscriber *subscriber);
static void tipc_subscrp_put(struct tipc_subscription *subscription);
static void tipc_subscrp_get(struct tipc_subscription *subscription);
>>>>>>> v4.9.227

/**
 * htohl - convert value to endianness used by destination
 * @in: value to convert
 * @swap: non-zero if endianness must be reversed
 *
 * Returns converted value
 */
static u32 htohl(u32 in, int swap)
{
	return swap ? swab32(in) : in;
}

<<<<<<< HEAD
static void subscr_send_event(struct tipc_subscription *sub, u32 found_lower,
			      u32 found_upper, u32 event, u32 port_ref,
			      u32 node)
{
=======
static void tipc_subscrp_send_event(struct tipc_subscription *sub,
				    u32 found_lower, u32 found_upper,
				    u32 event, u32 port_ref, u32 node)
{
	struct tipc_net *tn = net_generic(sub->net, tipc_net_id);
>>>>>>> v4.9.227
	struct tipc_subscriber *subscriber = sub->subscriber;
	struct kvec msg_sect;

	msg_sect.iov_base = (void *)&sub->evt;
	msg_sect.iov_len = sizeof(struct tipc_event);
	sub->evt.event = htohl(event, sub->swap);
	sub->evt.found_lower = htohl(found_lower, sub->swap);
	sub->evt.found_upper = htohl(found_upper, sub->swap);
	sub->evt.port.ref = htohl(port_ref, sub->swap);
	sub->evt.port.node = htohl(node, sub->swap);
<<<<<<< HEAD
	tipc_conn_sendmsg(&topsrv, subscriber->conid, NULL, msg_sect.iov_base,
			  msg_sect.iov_len);
}

/**
 * tipc_subscr_overlap - test for subscription overlap with the given values
 *
 * Returns 1 if there is overlap, otherwise 0.
 */
int tipc_subscr_overlap(struct tipc_subscription *sub, u32 found_lower,
			u32 found_upper)
{
	if (found_lower < sub->seq.lower)
		found_lower = sub->seq.lower;
	if (found_upper > sub->seq.upper)
		found_upper = sub->seq.upper;
=======
	tipc_conn_sendmsg(tn->topsrv, subscriber->conid, NULL,
			  msg_sect.iov_base, msg_sect.iov_len);
}

/**
 * tipc_subscrp_check_overlap - test for subscription overlap with the
 * given values
 *
 * Returns 1 if there is overlap, otherwise 0.
 */
int tipc_subscrp_check_overlap(struct tipc_name_seq *seq, u32 found_lower,
			       u32 found_upper)
{
	if (found_lower < seq->lower)
		found_lower = seq->lower;
	if (found_upper > seq->upper)
		found_upper = seq->upper;
>>>>>>> v4.9.227
	if (found_lower > found_upper)
		return 0;
	return 1;
}

<<<<<<< HEAD
/**
 * tipc_subscr_report_overlap - issue event if there is subscription overlap
 *
 * Protected by nameseq.lock in name_table.c
 */
void tipc_subscr_report_overlap(struct tipc_subscription *sub, u32 found_lower,
				u32 found_upper, u32 event, u32 port_ref,
				u32 node, int must)
{
	if (!tipc_subscr_overlap(sub, found_lower, found_upper))
		return;
	if (!must && !(sub->filter & TIPC_SUB_PORTS))
		return;

	subscr_send_event(sub, found_lower, found_upper, event, port_ref, node);
}

static void subscr_timeout(struct tipc_subscription *sub)
{
	struct tipc_subscriber *subscriber = sub->subscriber;

	/* The spin lock per subscriber is used to protect its members */
	spin_lock_bh(&subscriber->lock);

	/* Validate timeout (in case subscription is being cancelled) */
	if (sub->timeout == TIPC_WAIT_FOREVER) {
		spin_unlock_bh(&subscriber->lock);
		return;
	}

	/* Unlink subscription from name table */
	tipc_nametbl_unsubscribe(sub);

	/* Unlink subscription from subscriber */
	list_del(&sub->subscription_list);

	spin_unlock_bh(&subscriber->lock);

	/* Notify subscriber of timeout */
	subscr_send_event(sub, sub->evt.s.seq.lower, sub->evt.s.seq.upper,
			  TIPC_SUBSCR_TIMEOUT, 0, 0);

	/* Now destroy subscription */
	k_term_timer(&sub->timer);
	kfree(sub);
	atomic_dec(&subscription_count);
}

/**
 * subscr_del - delete a subscription within a subscription list
 *
 * Called with subscriber lock held.
 */
static void subscr_del(struct tipc_subscription *sub)
{
	tipc_nametbl_unsubscribe(sub);
	list_del(&sub->subscription_list);
	kfree(sub);
	atomic_dec(&subscription_count);
}

/**
 * subscr_terminate - terminate communication with a subscriber
 *
 * Note: Must call it in process context since it might sleep.
 */
static void subscr_terminate(struct tipc_subscriber *subscriber)
{
	tipc_conn_terminate(&topsrv, subscriber->conid);
}

static void subscr_release(struct tipc_subscriber *subscriber)
{
	struct tipc_subscription *sub;
	struct tipc_subscription *sub_temp;

	spin_lock_bh(&subscriber->lock);

	/* Destroy any existing subscriptions for subscriber */
	list_for_each_entry_safe(sub, sub_temp, &subscriber->subscription_list,
				 subscription_list) {
		if (sub->timeout != TIPC_WAIT_FOREVER) {
			spin_unlock_bh(&subscriber->lock);
			k_cancel_timer(&sub->timer);
			k_term_timer(&sub->timer);
			spin_lock_bh(&subscriber->lock);
		}
		subscr_del(sub);
	}
	spin_unlock_bh(&subscriber->lock);

	/* Now destroy subscriber */
	kfree(subscriber);
}

/**
 * subscr_cancel - handle subscription cancellation request
 *
 * Called with subscriber lock held. Routine must temporarily release lock
 * to enable the subscription timeout routine to finish without deadlocking;
 * the lock is then reclaimed to allow caller to release it upon return.
 *
 * Note that fields of 's' use subscriber's endianness!
 */
static void subscr_cancel(struct tipc_subscr *s,
			  struct tipc_subscriber *subscriber)
{
	struct tipc_subscription *sub;
	struct tipc_subscription *sub_temp;
	int found = 0;

	/* Find first matching subscription, exit if not found */
	list_for_each_entry_safe(sub, sub_temp, &subscriber->subscription_list,
				 subscription_list) {
		if (!memcmp(s, &sub->evt.s, sizeof(struct tipc_subscr))) {
			found = 1;
			break;
		}
	}
	if (!found)
		return;

	/* Cancel subscription timer (if used), then delete subscription */
	if (sub->timeout != TIPC_WAIT_FOREVER) {
		sub->timeout = TIPC_WAIT_FOREVER;
		spin_unlock_bh(&subscriber->lock);
		k_cancel_timer(&sub->timer);
		k_term_timer(&sub->timer);
		spin_lock_bh(&subscriber->lock);
	}
	subscr_del(sub);
}

/**
 * subscr_subscribe - create subscription for subscriber
 *
 * Called with subscriber lock held.
 */
static int subscr_subscribe(struct tipc_subscr *s,
			    struct tipc_subscriber *subscriber,
			    struct tipc_subscription **sub_p) {
	struct tipc_subscription *sub;
	int swap;

	/* Determine subscriber's endianness */
	swap = !(s->filter & (TIPC_SUB_PORTS | TIPC_SUB_SERVICE));

	/* Detect & process a subscription cancellation request */
	if (s->filter & htohl(TIPC_SUB_CANCEL, swap)) {
		s->filter &= ~htohl(TIPC_SUB_CANCEL, swap);
		subscr_cancel(s, subscriber);
		return 0;
	}

	/* Refuse subscription if global limit exceeded */
	if (atomic_read(&subscription_count) >= TIPC_MAX_SUBSCRIPTIONS) {
		pr_warn("Subscription rejected, limit reached (%u)\n",
			TIPC_MAX_SUBSCRIPTIONS);
		return -EINVAL;
=======
u32 tipc_subscrp_convert_seq_type(u32 type, int swap)
{
	return htohl(type, swap);
}

void tipc_subscrp_convert_seq(struct tipc_name_seq *in, int swap,
			      struct tipc_name_seq *out)
{
	out->type = htohl(in->type, swap);
	out->lower = htohl(in->lower, swap);
	out->upper = htohl(in->upper, swap);
}

void tipc_subscrp_report_overlap(struct tipc_subscription *sub, u32 found_lower,
				 u32 found_upper, u32 event, u32 port_ref,
				 u32 node, int must)
{
	struct tipc_name_seq seq;

	tipc_subscrp_get(sub);
	tipc_subscrp_convert_seq(&sub->evt.s.seq, sub->swap, &seq);
	if (!tipc_subscrp_check_overlap(&seq, found_lower, found_upper))
		return;
	if (!must &&
	    !(htohl(sub->evt.s.filter, sub->swap) & TIPC_SUB_PORTS))
		return;

	tipc_subscrp_send_event(sub, found_lower, found_upper, event, port_ref,
				node);
	tipc_subscrp_put(sub);
}

static void tipc_subscrp_timeout(unsigned long data)
{
	struct tipc_subscription *sub = (struct tipc_subscription *)data;
	struct tipc_subscriber *subscriber = sub->subscriber;

	spin_lock_bh(&subscriber->lock);
	tipc_nametbl_unsubscribe(sub);
	spin_unlock_bh(&subscriber->lock);

	/* Notify subscriber of timeout */
	tipc_subscrp_send_event(sub, sub->evt.s.seq.lower, sub->evt.s.seq.upper,
				TIPC_SUBSCR_TIMEOUT, 0, 0);

	tipc_subscrp_put(sub);
}

static void tipc_subscrb_kref_release(struct kref *kref)
{
	kfree(container_of(kref,struct tipc_subscriber, kref));
}

static void tipc_subscrb_put(struct tipc_subscriber *subscriber)
{
	kref_put(&subscriber->kref, tipc_subscrb_kref_release);
}

static void tipc_subscrb_get(struct tipc_subscriber *subscriber)
{
	kref_get(&subscriber->kref);
}

static void tipc_subscrp_kref_release(struct kref *kref)
{
	struct tipc_subscription *sub = container_of(kref,
						     struct tipc_subscription,
						     kref);
	struct tipc_net *tn = net_generic(sub->net, tipc_net_id);
	struct tipc_subscriber *subscriber = sub->subscriber;

	spin_lock_bh(&subscriber->lock);
	list_del(&sub->subscrp_list);
	atomic_dec(&tn->subscription_count);
	spin_unlock_bh(&subscriber->lock);
	kfree(sub);
	tipc_subscrb_put(subscriber);
}

static void tipc_subscrp_put(struct tipc_subscription *subscription)
{
	kref_put(&subscription->kref, tipc_subscrp_kref_release);
}

static void tipc_subscrp_get(struct tipc_subscription *subscription)
{
	kref_get(&subscription->kref);
}

/* tipc_subscrb_subscrp_delete - delete a specific subscription or all
 * subscriptions for a given subscriber.
 */
static void tipc_subscrb_subscrp_delete(struct tipc_subscriber *subscriber,
					struct tipc_subscr *s)
{
	struct list_head *subscription_list = &subscriber->subscrp_list;
	struct tipc_subscription *sub, *temp;

	spin_lock_bh(&subscriber->lock);
	list_for_each_entry_safe(sub, temp, subscription_list,  subscrp_list) {
		if (s && memcmp(s, &sub->evt.s, sizeof(struct tipc_subscr)))
			continue;

		tipc_nametbl_unsubscribe(sub);
		tipc_subscrp_get(sub);
		spin_unlock_bh(&subscriber->lock);
		tipc_subscrp_delete(sub);
		tipc_subscrp_put(sub);
		spin_lock_bh(&subscriber->lock);

		if (s)
			break;
	}
	spin_unlock_bh(&subscriber->lock);
}

static struct tipc_subscriber *tipc_subscrb_create(int conid)
{
	struct tipc_subscriber *subscriber;

	subscriber = kzalloc(sizeof(*subscriber), GFP_ATOMIC);
	if (!subscriber) {
		pr_warn("Subscriber rejected, no memory\n");
		return NULL;
	}
	INIT_LIST_HEAD(&subscriber->subscrp_list);
	kref_init(&subscriber->kref);
	subscriber->conid = conid;
	spin_lock_init(&subscriber->lock);

	return subscriber;
}

static void tipc_subscrb_delete(struct tipc_subscriber *subscriber)
{
	tipc_subscrb_subscrp_delete(subscriber, NULL);
	tipc_subscrb_put(subscriber);
}

static void tipc_subscrp_delete(struct tipc_subscription *sub)
{
	u32 timeout = htohl(sub->evt.s.timeout, sub->swap);

	if (timeout == TIPC_WAIT_FOREVER || del_timer(&sub->timer))
		tipc_subscrp_put(sub);
}

static void tipc_subscrp_cancel(struct tipc_subscr *s,
				struct tipc_subscriber *subscriber)
{
	tipc_subscrb_get(subscriber);
	tipc_subscrb_subscrp_delete(subscriber, s);
	tipc_subscrb_put(subscriber);
}

static struct tipc_subscription *tipc_subscrp_create(struct net *net,
						     struct tipc_subscr *s,
						     int swap)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	struct tipc_subscription *sub;
	u32 filter = htohl(s->filter, swap);

	/* Refuse subscription if global limit exceeded */
	if (atomic_read(&tn->subscription_count) >= TIPC_MAX_SUBSCRIPTIONS) {
		pr_warn("Subscription rejected, limit reached (%u)\n",
			TIPC_MAX_SUBSCRIPTIONS);
		return NULL;
>>>>>>> v4.9.227
	}

	/* Allocate subscription object */
	sub = kmalloc(sizeof(*sub), GFP_ATOMIC);
	if (!sub) {
		pr_warn("Subscription rejected, no memory\n");
<<<<<<< HEAD
		return -ENOMEM;
	}

	/* Initialize subscription object */
	sub->seq.type = htohl(s->seq.type, swap);
	sub->seq.lower = htohl(s->seq.lower, swap);
	sub->seq.upper = htohl(s->seq.upper, swap);
	sub->timeout = htohl(s->timeout, swap);
	sub->filter = htohl(s->filter, swap);
	if ((!(sub->filter & TIPC_SUB_PORTS) ==
	     !(sub->filter & TIPC_SUB_SERVICE)) ||
	    (sub->seq.lower > sub->seq.upper)) {
		pr_warn("Subscription rejected, illegal request\n");
		kfree(sub);
		return -EINVAL;
	}
	INIT_LIST_HEAD(&sub->nameseq_list);
	list_add(&sub->subscription_list, &subscriber->subscription_list);
	sub->subscriber = subscriber;
	sub->swap = swap;
	memcpy(&sub->evt.s, s, sizeof(struct tipc_subscr));
	atomic_inc(&subscription_count);
	if (sub->timeout != TIPC_WAIT_FOREVER) {
		k_init_timer(&sub->timer,
			     (Handler)subscr_timeout, (unsigned long)sub);
		k_start_timer(&sub->timer, sub->timeout);
	}
	*sub_p = sub;
	return 0;
}

/* Handle one termination request for the subscriber */
static void subscr_conn_shutdown_event(int conid, void *usr_data)
{
	subscr_release((struct tipc_subscriber *)usr_data);
}

/* Handle one request to create a new subscription for the subscriber */
static void subscr_conn_msg_event(int conid, struct sockaddr_tipc *addr,
				  void *usr_data, void *buf, size_t len)
{
	struct tipc_subscriber *subscriber = usr_data;
	struct tipc_subscription *sub = NULL;

	spin_lock_bh(&subscriber->lock);
	if (subscr_subscribe((struct tipc_subscr *)buf, subscriber, &sub) < 0) {
		spin_unlock_bh(&subscriber->lock);
		subscr_terminate(subscriber);
		return;
	}
	if (sub)
		tipc_nametbl_subscribe(sub);
	spin_unlock_bh(&subscriber->lock);
}


/* Handle one request to establish a new subscriber */
static void *subscr_named_msg_event(int conid)
{
	struct tipc_subscriber *subscriber;

	/* Create subscriber object */
	subscriber = kzalloc(sizeof(struct tipc_subscriber), GFP_ATOMIC);
	if (subscriber == NULL) {
		pr_warn("Subscriber rejected, no memory\n");
		return NULL;
	}
	INIT_LIST_HEAD(&subscriber->subscription_list);
	subscriber->conid = conid;
	spin_lock_init(&subscriber->lock);

	return (void *)subscriber;
}

int tipc_subscr_start(void)
{
	return tipc_server_start(&topsrv);
}

void tipc_subscr_stop(void)
{
	tipc_server_stop(&topsrv);
=======
		return NULL;
	}

	/* Initialize subscription object */
	sub->net = net;
	if (((filter & TIPC_SUB_PORTS) && (filter & TIPC_SUB_SERVICE)) ||
	    (htohl(s->seq.lower, swap) > htohl(s->seq.upper, swap))) {
		pr_warn("Subscription rejected, illegal request\n");
		kfree(sub);
		return NULL;
	}

	sub->swap = swap;
	memcpy(&sub->evt.s, s, sizeof(*s));
	atomic_inc(&tn->subscription_count);
	kref_init(&sub->kref);
	return sub;
}

static void tipc_subscrp_subscribe(struct net *net, struct tipc_subscr *s,
				   struct tipc_subscriber *subscriber, int swap)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	struct tipc_subscription *sub = NULL;
	u32 timeout;

	sub = tipc_subscrp_create(net, s, swap);
	if (!sub)
		return tipc_conn_terminate(tn->topsrv, subscriber->conid);

	spin_lock_bh(&subscriber->lock);
	list_add(&sub->subscrp_list, &subscriber->subscrp_list);
	sub->subscriber = subscriber;
	tipc_nametbl_subscribe(sub);
	tipc_subscrb_get(subscriber);
	spin_unlock_bh(&subscriber->lock);

	setup_timer(&sub->timer, tipc_subscrp_timeout, (unsigned long)sub);
	timeout = htohl(sub->evt.s.timeout, swap);

	if (timeout != TIPC_WAIT_FOREVER)
		mod_timer(&sub->timer, jiffies + msecs_to_jiffies(timeout));
}

/* Handle one termination request for the subscriber */
static void tipc_subscrb_release_cb(int conid, void *usr_data)
{
	tipc_subscrb_delete((struct tipc_subscriber *)usr_data);
}

/* Handle one request to create a new subscription for the subscriber */
static void tipc_subscrb_rcv_cb(struct net *net, int conid,
				struct sockaddr_tipc *addr, void *usr_data,
				void *buf, size_t len)
{
	struct tipc_subscriber *subscriber = usr_data;
	struct tipc_subscr *s = (struct tipc_subscr *)buf;
	int swap;

	/* Determine subscriber's endianness */
	swap = !(s->filter & (TIPC_SUB_PORTS | TIPC_SUB_SERVICE |
			      TIPC_SUB_CANCEL));

	/* Detect & process a subscription cancellation request */
	if (s->filter & htohl(TIPC_SUB_CANCEL, swap)) {
		s->filter &= ~htohl(TIPC_SUB_CANCEL, swap);
		return tipc_subscrp_cancel(s, subscriber);
	}

	tipc_subscrp_subscribe(net, s, subscriber, swap);
}

/* Handle one request to establish a new subscriber */
static void *tipc_subscrb_connect_cb(int conid)
{
	return (void *)tipc_subscrb_create(conid);
}

static int tipc_topsrv_start(struct net *net)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	const char name[] = "topology_server";
	struct tipc_server *topsrv;
	struct sockaddr_tipc *saddr;

	saddr = kzalloc(sizeof(*saddr), GFP_ATOMIC);
	if (!saddr)
		return -ENOMEM;
	saddr->family			= AF_TIPC;
	saddr->addrtype			= TIPC_ADDR_NAMESEQ;
	saddr->addr.nameseq.type	= TIPC_TOP_SRV;
	saddr->addr.nameseq.lower	= TIPC_TOP_SRV;
	saddr->addr.nameseq.upper	= TIPC_TOP_SRV;
	saddr->scope			= TIPC_NODE_SCOPE;

	topsrv = kzalloc(sizeof(*topsrv), GFP_ATOMIC);
	if (!topsrv) {
		kfree(saddr);
		return -ENOMEM;
	}
	topsrv->net			= net;
	topsrv->saddr			= saddr;
	topsrv->imp			= TIPC_CRITICAL_IMPORTANCE;
	topsrv->type			= SOCK_SEQPACKET;
	topsrv->max_rcvbuf_size		= sizeof(struct tipc_subscr);
	topsrv->tipc_conn_recvmsg	= tipc_subscrb_rcv_cb;
	topsrv->tipc_conn_new		= tipc_subscrb_connect_cb;
	topsrv->tipc_conn_release	= tipc_subscrb_release_cb;

	strscpy(topsrv->name, name, sizeof(topsrv->name));
	tn->topsrv = topsrv;
	atomic_set(&tn->subscription_count, 0);

	return tipc_server_start(topsrv);
}

static void tipc_topsrv_stop(struct net *net)
{
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	struct tipc_server *topsrv = tn->topsrv;

	tipc_server_stop(topsrv);
	kfree(topsrv->saddr);
	kfree(topsrv);
}

int __net_init tipc_topsrv_init_net(struct net *net)
{
	return tipc_topsrv_start(net);
}

void __net_exit tipc_topsrv_exit_net(struct net *net)
{
	tipc_topsrv_stop(net);
>>>>>>> v4.9.227
}
