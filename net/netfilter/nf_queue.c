/*
 * Rusty Russell (C)2000 -- This code is GPL.
 * Patrick McHardy (c) 2006-2012
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
<<<<<<< HEAD
=======
#include <linux/netfilter_bridge.h>
>>>>>>> v4.9.227
#include <linux/seq_file.h>
#include <linux/rcupdate.h>
#include <net/protocol.h>
#include <net/netfilter/nf_queue.h>
#include <net/dst.h>

#include "nf_internals.h"

/*
 * Hook for nfnetlink_queue to register its queue handler.
 * We do this so that most of the NFQUEUE code can be modular.
 *
 * Once the queue is registered it must reinject all packets it
 * receives, no matter what.
 */
<<<<<<< HEAD
static const struct nf_queue_handler __rcu *queue_handler __read_mostly;

/* return EBUSY when somebody else is registered, return EEXIST if the
 * same handler is registered, return 0 in case of success. */
void nf_register_queue_handler(const struct nf_queue_handler *qh)
{
	/* should never happen, we only have one queueing backend in kernel */
	WARN_ON(rcu_access_pointer(queue_handler));
	rcu_assign_pointer(queue_handler, qh);
=======

/* return EBUSY when somebody else is registered, return EEXIST if the
 * same handler is registered, return 0 in case of success. */
void nf_register_queue_handler(struct net *net, const struct nf_queue_handler *qh)
{
	/* should never happen, we only have one queueing backend in kernel */
	WARN_ON(rcu_access_pointer(net->nf.queue_handler));
	rcu_assign_pointer(net->nf.queue_handler, qh);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(nf_register_queue_handler);

/* The caller must flush their queue before this */
<<<<<<< HEAD
void nf_unregister_queue_handler(void)
{
	RCU_INIT_POINTER(queue_handler, NULL);
	synchronize_rcu();
=======
void nf_unregister_queue_handler(struct net *net)
{
	RCU_INIT_POINTER(net->nf.queue_handler, NULL);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(nf_unregister_queue_handler);

void nf_queue_entry_release_refs(struct nf_queue_entry *entry)
{
<<<<<<< HEAD
	/* Release those devices we held, or Alexey will kill me. */
	if (entry->indev)
		dev_put(entry->indev);
	if (entry->outdev)
		dev_put(entry->outdev);
#if IS_ENABLED(CONFIG_BRIDGE_NETFILTER)
	if (entry->skb->nf_bridge) {
		struct nf_bridge_info *nf_bridge = entry->skb->nf_bridge;

		if (nf_bridge->physindev)
			dev_put(nf_bridge->physindev);
		if (nf_bridge->physoutdev)
			dev_put(nf_bridge->physoutdev);
	}
#endif
	/* Drop reference to owner of hook which queued us. */
	module_put(entry->elem->owner);
=======
	struct nf_hook_state *state = &entry->state;

	/* Release those devices we held, or Alexey will kill me. */
	if (state->in)
		dev_put(state->in);
	if (state->out)
		dev_put(state->out);
	if (state->sk)
		sock_put(state->sk);
#if IS_ENABLED(CONFIG_BRIDGE_NETFILTER)
	if (entry->skb->nf_bridge) {
		struct net_device *physdev;

		physdev = nf_bridge_get_physindev(entry->skb);
		if (physdev)
			dev_put(physdev);
		physdev = nf_bridge_get_physoutdev(entry->skb);
		if (physdev)
			dev_put(physdev);
	}
#endif
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(nf_queue_entry_release_refs);

/* Bump dev refs so they don't vanish while packet is out */
<<<<<<< HEAD
bool nf_queue_entry_get_refs(struct nf_queue_entry *entry)
{
	if (!try_module_get(entry->elem->owner))
		return false;

	if (entry->indev)
		dev_hold(entry->indev);
	if (entry->outdev)
		dev_hold(entry->outdev);
#if IS_ENABLED(CONFIG_BRIDGE_NETFILTER)
	if (entry->skb->nf_bridge) {
		struct nf_bridge_info *nf_bridge = entry->skb->nf_bridge;
		struct net_device *physdev;

		physdev = nf_bridge->physindev;
		if (physdev)
			dev_hold(physdev);
		physdev = nf_bridge->physoutdev;
=======
void nf_queue_entry_get_refs(struct nf_queue_entry *entry)
{
	struct nf_hook_state *state = &entry->state;

	if (state->in)
		dev_hold(state->in);
	if (state->out)
		dev_hold(state->out);
	if (state->sk)
		sock_hold(state->sk);
#if IS_ENABLED(CONFIG_BRIDGE_NETFILTER)
	if (entry->skb->nf_bridge) {
		struct net_device *physdev;

		physdev = nf_bridge_get_physindev(entry->skb);
		if (physdev)
			dev_hold(physdev);
		physdev = nf_bridge_get_physoutdev(entry->skb);
>>>>>>> v4.9.227
		if (physdev)
			dev_hold(physdev);
	}
#endif
<<<<<<< HEAD

	return true;
}
EXPORT_SYMBOL_GPL(nf_queue_entry_get_refs);

void nf_queue_nf_hook_drop(struct nf_hook_ops *ops)
{
	const struct nf_queue_handler *qh;
	struct net *net;

	rtnl_lock();
	rcu_read_lock();
	qh = rcu_dereference(queue_handler);
	if (qh) {
		for_each_net(net) {
			qh->nf_hook_drop(net, ops);
		}
	}
	rcu_read_unlock();
	rtnl_unlock();
}

/*
 * Any packet that leaves via this function must come back
 * through nf_reinject().
 */
int nf_queue(struct sk_buff *skb,
		      struct nf_hook_ops *elem,
		      u_int8_t pf, unsigned int hook,
		      struct net_device *indev,
		      struct net_device *outdev,
		      int (*okfn)(struct sk_buff *),
=======
}
EXPORT_SYMBOL_GPL(nf_queue_entry_get_refs);

void nf_queue_nf_hook_drop(struct net *net, const struct nf_hook_entry *entry)
{
	const struct nf_queue_handler *qh;

	rcu_read_lock();
	qh = rcu_dereference(net->nf.queue_handler);
	if (qh)
		qh->nf_hook_drop(net, entry);
	rcu_read_unlock();
}

static int __nf_queue(struct sk_buff *skb, const struct nf_hook_state *state,
>>>>>>> v4.9.227
		      unsigned int queuenum)
{
	int status = -ENOENT;
	struct nf_queue_entry *entry = NULL;
	const struct nf_afinfo *afinfo;
	const struct nf_queue_handler *qh;
<<<<<<< HEAD

	/* QUEUE == DROP if no one is waiting, to be safe. */
	rcu_read_lock();

	qh = rcu_dereference(queue_handler);
	if (!qh) {
		status = -ESRCH;
		goto err_unlock;
	}

	afinfo = nf_get_afinfo(pf);
	if (!afinfo)
		goto err_unlock;
=======
	struct net *net = state->net;

	/* QUEUE == DROP if no one is waiting, to be safe. */
	qh = rcu_dereference(net->nf.queue_handler);
	if (!qh) {
		status = -ESRCH;
		goto err;
	}

	afinfo = nf_get_afinfo(state->pf);
	if (!afinfo)
		goto err;
>>>>>>> v4.9.227

	entry = kmalloc(sizeof(*entry) + afinfo->route_key_size, GFP_ATOMIC);
	if (!entry) {
		status = -ENOMEM;
<<<<<<< HEAD
		goto err_unlock;
=======
		goto err;
>>>>>>> v4.9.227
	}

	*entry = (struct nf_queue_entry) {
		.skb	= skb,
<<<<<<< HEAD
		.elem	= elem,
		.pf	= pf,
		.hook	= hook,
		.indev	= indev,
		.outdev	= outdev,
		.okfn	= okfn,
		.size	= sizeof(*entry) + afinfo->route_key_size,
	};

	if (!nf_queue_entry_get_refs(entry)) {
		status = -ECANCELED;
		goto err_unlock;
	}
=======
		.state	= *state,
		.size	= sizeof(*entry) + afinfo->route_key_size,
	};

	nf_queue_entry_get_refs(entry);
>>>>>>> v4.9.227
	skb_dst_force(skb);
	afinfo->saveroute(skb, entry);
	status = qh->outfn(entry, queuenum);

<<<<<<< HEAD
	rcu_read_unlock();

=======
>>>>>>> v4.9.227
	if (status < 0) {
		nf_queue_entry_release_refs(entry);
		goto err;
	}

	return 0;

<<<<<<< HEAD
err_unlock:
	rcu_read_unlock();
=======
>>>>>>> v4.9.227
err:
	kfree(entry);
	return status;
}

<<<<<<< HEAD
void nf_reinject(struct nf_queue_entry *entry, unsigned int verdict)
{
	struct sk_buff *skb = entry->skb;
	struct nf_hook_ops *elem = entry->elem;
	const struct nf_afinfo *afinfo;
	int err;

	rcu_read_lock();
=======
/* Packets leaving via this function must come back through nf_reinject(). */
int nf_queue(struct sk_buff *skb, struct nf_hook_state *state,
	     struct nf_hook_entry **entryp, unsigned int verdict)
{
	struct nf_hook_entry *entry = *entryp;
	int ret;

	RCU_INIT_POINTER(state->hook_entries, entry);
	ret = __nf_queue(skb, state, verdict >> NF_VERDICT_QBITS);
	if (ret < 0) {
		if (ret == -ESRCH &&
		    (verdict & NF_VERDICT_FLAG_QUEUE_BYPASS)) {
			*entryp = rcu_dereference(entry->next);
			return 1;
		}
		kfree_skb(skb);
	}

	return 0;
}

void nf_reinject(struct nf_queue_entry *entry, unsigned int verdict)
{
	struct nf_hook_entry *hook_entry;
	struct sk_buff *skb = entry->skb;
	const struct nf_afinfo *afinfo;
	struct nf_hook_ops *elem;
	int err;

	hook_entry = rcu_dereference(entry->state.hook_entries);
	elem = &hook_entry->ops;
>>>>>>> v4.9.227

	nf_queue_entry_release_refs(entry);

	/* Continue traversal iff userspace said ok... */
<<<<<<< HEAD
	if (verdict == NF_REPEAT) {
		elem = list_entry(elem->list.prev, struct nf_hook_ops, list);
		verdict = NF_ACCEPT;
	}

	if (verdict == NF_ACCEPT) {
		afinfo = nf_get_afinfo(entry->pf);
		if (!afinfo || afinfo->reroute(skb, entry) < 0)
			verdict = NF_DROP;
	}

	if (verdict == NF_ACCEPT) {
	next_hook:
		verdict = nf_iterate(&nf_hooks[entry->pf][entry->hook],
				     skb, entry->hook,
				     entry->indev, entry->outdev, &elem,
				     entry->okfn, INT_MIN);
=======
	if (verdict == NF_REPEAT)
		verdict = elem->hook(elem->priv, skb, &entry->state);

	if (verdict == NF_ACCEPT) {
		afinfo = nf_get_afinfo(entry->state.pf);
		if (!afinfo || afinfo->reroute(entry->state.net, skb, entry) < 0)
			verdict = NF_DROP;
	}

	entry->state.thresh = INT_MIN;

	if (verdict == NF_ACCEPT) {
		hook_entry = rcu_dereference(hook_entry->next);
		if (hook_entry)
next_hook:
			verdict = nf_iterate(skb, &entry->state, &hook_entry);
>>>>>>> v4.9.227
	}

	switch (verdict & NF_VERDICT_MASK) {
	case NF_ACCEPT:
	case NF_STOP:
<<<<<<< HEAD
		local_bh_disable();
		entry->okfn(skb);
		local_bh_enable();
		break;
	case NF_QUEUE:
		err = nf_queue(skb, elem, entry->pf, entry->hook,
				entry->indev, entry->outdev, entry->okfn,
				verdict >> NF_VERDICT_QBITS);
		if (err < 0) {
			if (err == -ECANCELED)
				goto next_hook;
			if (err == -ESRCH &&
			   (verdict & NF_VERDICT_FLAG_QUEUE_BYPASS))
				goto next_hook;
			kfree_skb(skb);
=======
okfn:
		local_bh_disable();
		entry->state.okfn(entry->state.net, entry->state.sk, skb);
		local_bh_enable();
		break;
	case NF_QUEUE:
		err = nf_queue(skb, &entry->state, &hook_entry, verdict);
		if (err == 1) {
			if (hook_entry)
				goto next_hook;
			goto okfn;
>>>>>>> v4.9.227
		}
		break;
	case NF_STOLEN:
		break;
	default:
		kfree_skb(skb);
	}
<<<<<<< HEAD
	rcu_read_unlock();
=======

>>>>>>> v4.9.227
	kfree(entry);
}
EXPORT_SYMBOL(nf_reinject);
