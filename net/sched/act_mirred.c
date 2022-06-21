/*
<<<<<<< HEAD
 * net/sched/mirred.c	packet mirroring and redirect actions
=======
 * net/sched/act_mirred.c	packet mirroring and redirect actions
>>>>>>> v4.9.227
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Jamal Hadi Salim (2002-4)
 *
 * TODO: Add ingress support (and socket redirect support)
 *
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/skbuff.h>
#include <linux/rtnetlink.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gfp.h>
#include <net/net_namespace.h>
#include <net/netlink.h>
#include <net/pkt_sched.h>
#include <linux/tc_act/tc_mirred.h>
#include <net/tc_act/tc_mirred.h>

#include <linux/if_arp.h>

#define MIRRED_TAB_MASK     7
static LIST_HEAD(mirred_list);
static DEFINE_SPINLOCK(mirred_list_lock);

static void tcf_mirred_release(struct tc_action *a, int bind)
{
	struct tcf_mirred *m = to_mirred(a);
<<<<<<< HEAD
=======
	struct net_device *dev;
>>>>>>> v4.9.227

	/* We could be called either in a RCU callback or with RTNL lock held. */
	spin_lock_bh(&mirred_list_lock);
	list_del(&m->tcfm_list);
<<<<<<< HEAD
	spin_unlock_bh(&mirred_list_lock);
	if (m->tcfm_dev)
		dev_put(m->tcfm_dev);
=======
	dev = rcu_dereference_protected(m->tcfm_dev, 1);
	if (dev)
		dev_put(dev);
	spin_unlock_bh(&mirred_list_lock);
>>>>>>> v4.9.227
}

static const struct nla_policy mirred_policy[TCA_MIRRED_MAX + 1] = {
	[TCA_MIRRED_PARMS]	= { .len = sizeof(struct tc_mirred) },
};

<<<<<<< HEAD
static int tcf_mirred_init(struct net *net, struct nlattr *nla,
			   struct nlattr *est, struct tc_action *a, int ovr,
			   int bind)
{
=======
static int mirred_net_id;
static struct tc_action_ops act_mirred_ops;

static int tcf_mirred_init(struct net *net, struct nlattr *nla,
			   struct nlattr *est, struct tc_action **a, int ovr,
			   int bind)
{
	struct tc_action_net *tn = net_generic(net, mirred_net_id);
>>>>>>> v4.9.227
	struct nlattr *tb[TCA_MIRRED_MAX + 1];
	struct tc_mirred *parm;
	struct tcf_mirred *m;
	struct net_device *dev;
	int ret, ok_push = 0;
<<<<<<< HEAD
=======
	bool exists = false;
>>>>>>> v4.9.227

	if (nla == NULL)
		return -EINVAL;
	ret = nla_parse_nested(tb, TCA_MIRRED_MAX, nla, mirred_policy);
	if (ret < 0)
		return ret;
	if (tb[TCA_MIRRED_PARMS] == NULL)
		return -EINVAL;
	parm = nla_data(tb[TCA_MIRRED_PARMS]);
<<<<<<< HEAD
	switch (parm->eaction) {
	case TCA_EGRESS_MIRROR:
	case TCA_EGRESS_REDIR:
	case TCA_INGRESS_REDIR:
		break;
	default:
=======

	exists = tcf_hash_check(tn, parm->index, a, bind);
	if (exists && bind)
		return 0;

	switch (parm->eaction) {
	case TCA_EGRESS_MIRROR:
	case TCA_EGRESS_REDIR:
		break;
	default:
		if (exists)
			tcf_hash_release(*a, bind);
>>>>>>> v4.9.227
		return -EINVAL;
	}
	if (parm->ifindex) {
		dev = __dev_get_by_index(net, parm->ifindex);
<<<<<<< HEAD
		if (dev == NULL)
			return -ENODEV;
=======
		if (dev == NULL) {
			if (exists)
				tcf_hash_release(*a, bind);
			return -ENODEV;
		}
>>>>>>> v4.9.227
		switch (dev->type) {
		case ARPHRD_TUNNEL:
		case ARPHRD_TUNNEL6:
		case ARPHRD_SIT:
		case ARPHRD_IPGRE:
		case ARPHRD_VOID:
		case ARPHRD_NONE:
			ok_push = 0;
			break;
		default:
			ok_push = 1;
			break;
		}
	} else {
		dev = NULL;
	}

<<<<<<< HEAD
	if (!tcf_hash_check(parm->index, a, bind)) {
		if (dev == NULL)
			return -EINVAL;
		ret = tcf_hash_create(parm->index, est, a, sizeof(*m), bind);
=======
	if (!exists) {
		if (dev == NULL)
			return -EINVAL;
		ret = tcf_hash_create(tn, parm->index, est, a,
				      &act_mirred_ops, bind, true);
>>>>>>> v4.9.227
		if (ret)
			return ret;
		ret = ACT_P_CREATED;
	} else {
<<<<<<< HEAD
		if (!ovr) {
			tcf_hash_release(a, bind);
			return -EEXIST;
		}
	}
	m = to_mirred(a);

	spin_lock_bh(&m->tcf_lock);
=======
		tcf_hash_release(*a, bind);
		if (!ovr)
			return -EEXIST;
	}
	m = to_mirred(*a);

	ASSERT_RTNL();
>>>>>>> v4.9.227
	m->tcf_action = parm->action;
	m->tcfm_eaction = parm->eaction;
	if (dev != NULL) {
		m->tcfm_ifindex = parm->ifindex;
		if (ret != ACT_P_CREATED)
<<<<<<< HEAD
			dev_put(m->tcfm_dev);
		dev_hold(dev);
		m->tcfm_dev = dev;
		m->tcfm_ok_push = ok_push;
	}
	spin_unlock_bh(&m->tcf_lock);
=======
			dev_put(rcu_dereference_protected(m->tcfm_dev, 1));
		dev_hold(dev);
		rcu_assign_pointer(m->tcfm_dev, dev);
		m->tcfm_ok_push = ok_push;
	}

>>>>>>> v4.9.227
	if (ret == ACT_P_CREATED) {
		spin_lock_bh(&mirred_list_lock);
		list_add(&m->tcfm_list, &mirred_list);
		spin_unlock_bh(&mirred_list_lock);
<<<<<<< HEAD
		tcf_hash_insert(a);
=======
		tcf_hash_insert(tn, *a);
>>>>>>> v4.9.227
	}

	return ret;
}

static int tcf_mirred(struct sk_buff *skb, const struct tc_action *a,
		      struct tcf_result *res)
{
<<<<<<< HEAD
	struct tcf_mirred *m = a->priv;
	struct net_device *dev;
	struct sk_buff *skb2;
	u32 at;
	int retval, err = 1;

	spin_lock(&m->tcf_lock);
	m->tcf_tm.lastuse = jiffies;
	bstats_update(&m->tcf_bstats, skb);

	dev = m->tcfm_dev;
	if (!dev) {
		printk_once(KERN_NOTICE "tc mirred: target device is gone\n");
		goto out;
	}

	if (!(dev->flags & IFF_UP)) {
=======
	struct tcf_mirred *m = to_mirred(a);
	struct net_device *dev;
	struct sk_buff *skb2;
	int retval, err;
	u32 at;

	tcf_lastuse_update(&m->tcf_tm);
	bstats_cpu_update(this_cpu_ptr(m->common.cpu_bstats), skb);

	rcu_read_lock();
	retval = READ_ONCE(m->tcf_action);
	dev = rcu_dereference(m->tcfm_dev);
	if (unlikely(!dev)) {
		pr_notice_once("tc mirred: target device is gone\n");
		goto out;
	}

	if (unlikely(!(dev->flags & IFF_UP))) {
>>>>>>> v4.9.227
		net_notice_ratelimited("tc mirred to Houston: device %s is down\n",
				       dev->name);
		goto out;
	}

<<<<<<< HEAD
	skb2 = skb_act_clone(skb, GFP_ATOMIC, m->tcf_action);
	if (skb2 == NULL)
		goto out;

        if (m->tcfm_eaction == TCA_INGRESS_REDIR) {
                /* Let's _hope_ the devices are of similar type.
                 * This is rather dangerous; with changed skb_iif, we
                 * will not know the real input device, but perhaps
                 * that's the whole point of doing the ingress
                 * redirect/mirror in the first place?  (Note: This
                 * can lead to bad things if two devices ingress
                 * redirect at each other. Don't do that.)*/
                skb2->dev = dev;
                skb2->skb_iif = skb2->dev->ifindex;
                skb2->pkt_type = PACKET_HOST;
                netif_rx(skb2);
        } else {
                at = G_TC_AT(skb->tc_verd);
                if (!(at & AT_EGRESS)) {
                        if (m->tcfm_ok_push) {
			        skb_push(skb2, skb2->dev->hard_header_len);
                        }
                }

                /* mirror is always swallowed */
                if (m->tcfm_eaction != TCA_EGRESS_MIRROR)
		        skb2->tc_verd = SET_TC_FROM(skb2->tc_verd, at);

                skb2->skb_iif = skb->dev->ifindex;
                skb2->dev = dev;
                err = dev_queue_xmit(skb2);
        }
out:
	if (err) {
		m->tcf_qstats.overlimits++;
		if (m->tcfm_eaction != TCA_EGRESS_MIRROR)
			retval = TC_ACT_SHOT;
		else
			retval = m->tcf_action;
	} else
		retval = m->tcf_action;
	spin_unlock(&m->tcf_lock);
=======
	at = G_TC_AT(skb->tc_verd);
	skb2 = skb_clone(skb, GFP_ATOMIC);
	if (!skb2)
		goto out;

	if (!(at & AT_EGRESS)) {
		if (m->tcfm_ok_push)
			skb_push_rcsum(skb2, skb->mac_len);
	}

	/* mirror is always swallowed */
	if (m->tcfm_eaction != TCA_EGRESS_MIRROR)
		skb2->tc_verd = SET_TC_FROM(skb2->tc_verd, at);

	skb2->skb_iif = skb->dev->ifindex;
	skb2->dev = dev;
	err = dev_queue_xmit(skb2);

	if (err) {
out:
		qstats_overlimit_inc(this_cpu_ptr(m->common.cpu_qstats));
		if (m->tcfm_eaction != TCA_EGRESS_MIRROR)
			retval = TC_ACT_SHOT;
	}
	rcu_read_unlock();
>>>>>>> v4.9.227

	return retval;
}

<<<<<<< HEAD
static int tcf_mirred_dump(struct sk_buff *skb, struct tc_action *a, int bind, int ref)
{
	unsigned char *b = skb_tail_pointer(skb);
	struct tcf_mirred *m = a->priv;
=======
static void tcf_stats_update(struct tc_action *a, u64 bytes, u32 packets,
			     u64 lastuse)
{
	struct tcf_mirred *m = to_mirred(a);
	struct tcf_t *tm = &m->tcf_tm;

	_bstats_cpu_update(this_cpu_ptr(a->cpu_bstats), bytes, packets);
	tm->lastuse = max_t(u64, tm->lastuse, lastuse);
}

static int tcf_mirred_dump(struct sk_buff *skb, struct tc_action *a, int bind,
			   int ref)
{
	unsigned char *b = skb_tail_pointer(skb);
	struct tcf_mirred *m = to_mirred(a);
>>>>>>> v4.9.227
	struct tc_mirred opt = {
		.index   = m->tcf_index,
		.action  = m->tcf_action,
		.refcnt  = m->tcf_refcnt - ref,
		.bindcnt = m->tcf_bindcnt - bind,
		.eaction = m->tcfm_eaction,
		.ifindex = m->tcfm_ifindex,
	};
	struct tcf_t t;

	if (nla_put(skb, TCA_MIRRED_PARMS, sizeof(opt), &opt))
		goto nla_put_failure;
<<<<<<< HEAD
	t.install = jiffies_to_clock_t(jiffies - m->tcf_tm.install);
	t.lastuse = jiffies_to_clock_t(jiffies - m->tcf_tm.lastuse);
	t.expires = jiffies_to_clock_t(m->tcf_tm.expires);
	if (nla_put(skb, TCA_MIRRED_TM, sizeof(t), &t))
=======

	tcf_tm_dump(&t, &m->tcf_tm);
	if (nla_put_64bit(skb, TCA_MIRRED_TM, sizeof(t), &t, TCA_MIRRED_PAD))
>>>>>>> v4.9.227
		goto nla_put_failure;
	return skb->len;

nla_put_failure:
	nlmsg_trim(skb, b);
	return -1;
}

<<<<<<< HEAD
=======
static int tcf_mirred_walker(struct net *net, struct sk_buff *skb,
			     struct netlink_callback *cb, int type,
			     const struct tc_action_ops *ops)
{
	struct tc_action_net *tn = net_generic(net, mirred_net_id);

	return tcf_generic_walker(tn, skb, cb, type, ops);
}

static int tcf_mirred_search(struct net *net, struct tc_action **a, u32 index)
{
	struct tc_action_net *tn = net_generic(net, mirred_net_id);

	return tcf_hash_search(tn, a, index);
}

>>>>>>> v4.9.227
static int mirred_device_event(struct notifier_block *unused,
			       unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct tcf_mirred *m;

<<<<<<< HEAD
	if (event == NETDEV_UNREGISTER) {
		spin_lock_bh(&mirred_list_lock);
		list_for_each_entry(m, &mirred_list, tcfm_list) {
			spin_lock_bh(&m->tcf_lock);
			if (m->tcfm_dev == dev) {
				dev_put(dev);
				m->tcfm_dev = NULL;
			}
			spin_unlock_bh(&m->tcf_lock);
=======
	ASSERT_RTNL();
	if (event == NETDEV_UNREGISTER) {
		spin_lock_bh(&mirred_list_lock);
		list_for_each_entry(m, &mirred_list, tcfm_list) {
			if (rcu_access_pointer(m->tcfm_dev) == dev) {
				dev_put(dev);
				/* Note : no rcu grace period necessary, as
				 * net_device are already rcu protected.
				 */
				RCU_INIT_POINTER(m->tcfm_dev, NULL);
			}
>>>>>>> v4.9.227
		}
		spin_unlock_bh(&mirred_list_lock);
	}

	return NOTIFY_DONE;
}

static struct notifier_block mirred_device_notifier = {
	.notifier_call = mirred_device_event,
};

static struct tc_action_ops act_mirred_ops = {
	.kind		=	"mirred",
	.type		=	TCA_ACT_MIRRED,
	.owner		=	THIS_MODULE,
	.act		=	tcf_mirred,
<<<<<<< HEAD
	.dump		=	tcf_mirred_dump,
	.cleanup	=	tcf_mirred_release,
	.init		=	tcf_mirred_init,
=======
	.stats_update	=	tcf_stats_update,
	.dump		=	tcf_mirred_dump,
	.cleanup	=	tcf_mirred_release,
	.init		=	tcf_mirred_init,
	.walk		=	tcf_mirred_walker,
	.lookup		=	tcf_mirred_search,
	.size		=	sizeof(struct tcf_mirred),
};

static __net_init int mirred_init_net(struct net *net)
{
	struct tc_action_net *tn = net_generic(net, mirred_net_id);

	return tc_action_net_init(tn, &act_mirred_ops, MIRRED_TAB_MASK);
}

static void __net_exit mirred_exit_net(struct net *net)
{
	struct tc_action_net *tn = net_generic(net, mirred_net_id);

	tc_action_net_exit(tn);
}

static struct pernet_operations mirred_net_ops = {
	.init = mirred_init_net,
	.exit = mirred_exit_net,
	.id   = &mirred_net_id,
	.size = sizeof(struct tc_action_net),
>>>>>>> v4.9.227
};

MODULE_AUTHOR("Jamal Hadi Salim(2002)");
MODULE_DESCRIPTION("Device Mirror/redirect actions");
MODULE_LICENSE("GPL");

static int __init mirred_init_module(void)
{
	int err = register_netdevice_notifier(&mirred_device_notifier);
	if (err)
		return err;

	pr_info("Mirror/redirect action on\n");
<<<<<<< HEAD
	return tcf_register_action(&act_mirred_ops, MIRRED_TAB_MASK);
=======
	err = tcf_register_action(&act_mirred_ops, &mirred_net_ops);
	if (err)
		unregister_netdevice_notifier(&mirred_device_notifier);

	return err;
>>>>>>> v4.9.227
}

static void __exit mirred_cleanup_module(void)
{
<<<<<<< HEAD
	tcf_unregister_action(&act_mirred_ops);
=======
	tcf_unregister_action(&act_mirred_ops, &mirred_net_ops);
>>>>>>> v4.9.227
	unregister_netdevice_notifier(&mirred_device_notifier);
}

module_init(mirred_init_module);
module_exit(mirred_cleanup_module);
