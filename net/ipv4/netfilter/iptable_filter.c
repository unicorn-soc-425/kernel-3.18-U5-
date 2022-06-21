/*
 * This is the 1999 rewrite of IP Firewalling, aiming for kernel 2.3.x.
 *
 * Copyright (C) 1999 Paul `Rusty' Russell & Michael J. Neuling
 * Copyright (C) 2000-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/slab.h>
#include <net/ip.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("iptables filter table");

#define FILTER_VALID_HOOKS ((1 << NF_INET_LOCAL_IN) | \
			    (1 << NF_INET_FORWARD) | \
			    (1 << NF_INET_LOCAL_OUT))
<<<<<<< HEAD
=======
static int __net_init iptable_filter_table_init(struct net *net);
>>>>>>> v4.9.227

static const struct xt_table packet_filter = {
	.name		= "filter",
	.valid_hooks	= FILTER_VALID_HOOKS,
	.me		= THIS_MODULE,
	.af		= NFPROTO_IPV4,
	.priority	= NF_IP_PRI_FILTER,
<<<<<<< HEAD
};

static unsigned int
iptable_filter_hook(const struct nf_hook_ops *ops, struct sk_buff *skb,
		    const struct net_device *in, const struct net_device *out,
		    int (*okfn)(struct sk_buff *))
{
	const struct net *net;

	if (ops->hooknum == NF_INET_LOCAL_OUT &&
=======
	.table_init	= iptable_filter_table_init,
};

static unsigned int
iptable_filter_hook(void *priv, struct sk_buff *skb,
		    const struct nf_hook_state *state)
{
	if (state->hook == NF_INET_LOCAL_OUT &&
>>>>>>> v4.9.227
	    (skb->len < sizeof(struct iphdr) ||
	     ip_hdrlen(skb) < sizeof(struct iphdr)))
		/* root is playing with raw sockets. */
		return NF_ACCEPT;

<<<<<<< HEAD
	net = dev_net((in != NULL) ? in : out);
	return ipt_do_table(skb, ops->hooknum, in, out,
			    net->ipv4.iptable_filter);
=======
	return ipt_do_table(skb, state, state->net->ipv4.iptable_filter);
>>>>>>> v4.9.227
}

static struct nf_hook_ops *filter_ops __read_mostly;

/* Default to forward because I got too much mail already. */
<<<<<<< HEAD
static bool forward = true;
module_param(forward, bool, 0000);

static int __net_init iptable_filter_net_init(struct net *net)
{
	struct ipt_replace *repl;
=======
static bool forward __read_mostly = true;
module_param(forward, bool, 0000);

static int __net_init iptable_filter_table_init(struct net *net)
{
	struct ipt_replace *repl;
	int err;

	if (net->ipv4.iptable_filter)
		return 0;
>>>>>>> v4.9.227

	repl = ipt_alloc_initial_table(&packet_filter);
	if (repl == NULL)
		return -ENOMEM;
	/* Entry 1 is the FORWARD hook */
	((struct ipt_standard *)repl->entries)[1].target.verdict =
		forward ? -NF_ACCEPT - 1 : -NF_DROP - 1;

<<<<<<< HEAD
	net->ipv4.iptable_filter =
		ipt_register_table(net, &packet_filter, repl);
	kfree(repl);
	return PTR_ERR_OR_ZERO(net->ipv4.iptable_filter);
=======
	err = ipt_register_table(net, &packet_filter, repl, filter_ops,
				 &net->ipv4.iptable_filter);
	kfree(repl);
	return err;
}

static int __net_init iptable_filter_net_init(struct net *net)
{
	if (net == &init_net || !forward)
		return iptable_filter_table_init(net);

	return 0;
>>>>>>> v4.9.227
}

static void __net_exit iptable_filter_net_exit(struct net *net)
{
<<<<<<< HEAD
	ipt_unregister_table(net, net->ipv4.iptable_filter);
=======
	if (!net->ipv4.iptable_filter)
		return;
	ipt_unregister_table(net, net->ipv4.iptable_filter, filter_ops);
	net->ipv4.iptable_filter = NULL;
>>>>>>> v4.9.227
}

static struct pernet_operations iptable_filter_net_ops = {
	.init = iptable_filter_net_init,
	.exit = iptable_filter_net_exit,
};

static int __init iptable_filter_init(void)
{
	int ret;

<<<<<<< HEAD
	ret = register_pernet_subsys(&iptable_filter_net_ops);
	if (ret < 0)
		return ret;

	/* Register hooks */
	filter_ops = xt_hook_link(&packet_filter, iptable_filter_hook);
	if (IS_ERR(filter_ops)) {
		ret = PTR_ERR(filter_ops);
		unregister_pernet_subsys(&iptable_filter_net_ops);
	}
=======
	filter_ops = xt_hook_ops_alloc(&packet_filter, iptable_filter_hook);
	if (IS_ERR(filter_ops))
		return PTR_ERR(filter_ops);

	ret = register_pernet_subsys(&iptable_filter_net_ops);
	if (ret < 0)
		kfree(filter_ops);
>>>>>>> v4.9.227

	return ret;
}

static void __exit iptable_filter_fini(void)
{
<<<<<<< HEAD
	xt_hook_unlink(&packet_filter, filter_ops);
	unregister_pernet_subsys(&iptable_filter_net_ops);
=======
	unregister_pernet_subsys(&iptable_filter_net_ops);
	kfree(filter_ops);
>>>>>>> v4.9.227
}

module_init(iptable_filter_init);
module_exit(iptable_filter_fini);
