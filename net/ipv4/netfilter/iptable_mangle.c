/*
 * This is the 1999 rewrite of IP Firewalling, aiming for kernel 2.3.x.
 *
 * Copyright (C) 1999 Paul `Rusty' Russell & Michael J. Neuling
 * Copyright (C) 2000-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/module.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <net/sock.h>
#include <net/route.h>
#include <linux/ip.h>
#include <net/ip.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("iptables mangle table");

#define MANGLE_VALID_HOOKS ((1 << NF_INET_PRE_ROUTING) | \
			    (1 << NF_INET_LOCAL_IN) | \
			    (1 << NF_INET_FORWARD) | \
			    (1 << NF_INET_LOCAL_OUT) | \
			    (1 << NF_INET_POST_ROUTING))

<<<<<<< HEAD
=======
static int __net_init iptable_mangle_table_init(struct net *net);

>>>>>>> v4.9.227
static const struct xt_table packet_mangler = {
	.name		= "mangle",
	.valid_hooks	= MANGLE_VALID_HOOKS,
	.me		= THIS_MODULE,
	.af		= NFPROTO_IPV4,
	.priority	= NF_IP_PRI_MANGLE,
<<<<<<< HEAD
};

static unsigned int
ipt_mangle_out(struct sk_buff *skb, const struct net_device *out)
=======
	.table_init	= iptable_mangle_table_init,
};

static unsigned int
ipt_mangle_out(struct sk_buff *skb, const struct nf_hook_state *state)
>>>>>>> v4.9.227
{
	unsigned int ret;
	const struct iphdr *iph;
	u_int8_t tos;
	__be32 saddr, daddr;
	u_int32_t mark;
	int err;

	/* root is playing with raw sockets. */
	if (skb->len < sizeof(struct iphdr) ||
	    ip_hdrlen(skb) < sizeof(struct iphdr))
		return NF_ACCEPT;

	/* Save things which could affect route */
	mark = skb->mark;
	iph = ip_hdr(skb);
	saddr = iph->saddr;
	daddr = iph->daddr;
	tos = iph->tos;

<<<<<<< HEAD
	ret = ipt_do_table(skb, NF_INET_LOCAL_OUT, NULL, out,
			   dev_net(out)->ipv4.iptable_mangle);
=======
	ret = ipt_do_table(skb, state, state->net->ipv4.iptable_mangle);
>>>>>>> v4.9.227
	/* Reroute for ANY change. */
	if (ret != NF_DROP && ret != NF_STOLEN) {
		iph = ip_hdr(skb);

		if (iph->saddr != saddr ||
		    iph->daddr != daddr ||
		    skb->mark != mark ||
		    iph->tos != tos) {
<<<<<<< HEAD
			err = ip_route_me_harder(skb, RTN_UNSPEC);
=======
			err = ip_route_me_harder(state->net, skb, RTN_UNSPEC);
>>>>>>> v4.9.227
			if (err < 0)
				ret = NF_DROP_ERR(err);
		}
	}

	return ret;
}

/* The work comes in here from netfilter.c. */
static unsigned int
<<<<<<< HEAD
iptable_mangle_hook(const struct nf_hook_ops *ops,
		     struct sk_buff *skb,
		     const struct net_device *in,
		     const struct net_device *out,
		     int (*okfn)(struct sk_buff *))
{
	if (ops->hooknum == NF_INET_LOCAL_OUT)
		return ipt_mangle_out(skb, out);
	if (ops->hooknum == NF_INET_POST_ROUTING)
		return ipt_do_table(skb, ops->hooknum, in, out,
				    dev_net(out)->ipv4.iptable_mangle);
	/* PREROUTING/INPUT/FORWARD: */
	return ipt_do_table(skb, ops->hooknum, in, out,
			    dev_net(in)->ipv4.iptable_mangle);
}

static struct nf_hook_ops *mangle_ops __read_mostly;

static int __net_init iptable_mangle_net_init(struct net *net)
{
	struct ipt_replace *repl;
=======
iptable_mangle_hook(void *priv,
		     struct sk_buff *skb,
		     const struct nf_hook_state *state)
{
	if (state->hook == NF_INET_LOCAL_OUT)
		return ipt_mangle_out(skb, state);
	return ipt_do_table(skb, state, state->net->ipv4.iptable_mangle);
}

static struct nf_hook_ops *mangle_ops __read_mostly;
static int __net_init iptable_mangle_table_init(struct net *net)
{
	struct ipt_replace *repl;
	int ret;

	if (net->ipv4.iptable_mangle)
		return 0;
>>>>>>> v4.9.227

	repl = ipt_alloc_initial_table(&packet_mangler);
	if (repl == NULL)
		return -ENOMEM;
<<<<<<< HEAD
	net->ipv4.iptable_mangle =
		ipt_register_table(net, &packet_mangler, repl);
	kfree(repl);
	return PTR_ERR_OR_ZERO(net->ipv4.iptable_mangle);
=======
	ret = ipt_register_table(net, &packet_mangler, repl, mangle_ops,
				 &net->ipv4.iptable_mangle);
	kfree(repl);
	return ret;
>>>>>>> v4.9.227
}

static void __net_exit iptable_mangle_net_exit(struct net *net)
{
<<<<<<< HEAD
	ipt_unregister_table(net, net->ipv4.iptable_mangle);
}

static struct pernet_operations iptable_mangle_net_ops = {
	.init = iptable_mangle_net_init,
=======
	if (!net->ipv4.iptable_mangle)
		return;
	ipt_unregister_table(net, net->ipv4.iptable_mangle, mangle_ops);
	net->ipv4.iptable_mangle = NULL;
}

static struct pernet_operations iptable_mangle_net_ops = {
>>>>>>> v4.9.227
	.exit = iptable_mangle_net_exit,
};

static int __init iptable_mangle_init(void)
{
	int ret;

<<<<<<< HEAD
	ret = register_pernet_subsys(&iptable_mangle_net_ops);
	if (ret < 0)
		return ret;

	/* Register hooks */
	mangle_ops = xt_hook_link(&packet_mangler, iptable_mangle_hook);
	if (IS_ERR(mangle_ops)) {
		ret = PTR_ERR(mangle_ops);
		unregister_pernet_subsys(&iptable_mangle_net_ops);
=======
	mangle_ops = xt_hook_ops_alloc(&packet_mangler, iptable_mangle_hook);
	if (IS_ERR(mangle_ops)) {
		ret = PTR_ERR(mangle_ops);
		return ret;
	}

	ret = register_pernet_subsys(&iptable_mangle_net_ops);
	if (ret < 0) {
		kfree(mangle_ops);
		return ret;
	}

	ret = iptable_mangle_table_init(&init_net);
	if (ret) {
		unregister_pernet_subsys(&iptable_mangle_net_ops);
		kfree(mangle_ops);
>>>>>>> v4.9.227
	}

	return ret;
}

static void __exit iptable_mangle_fini(void)
{
<<<<<<< HEAD
	xt_hook_unlink(&packet_mangler, mangle_ops);
	unregister_pernet_subsys(&iptable_mangle_net_ops);
=======
	unregister_pernet_subsys(&iptable_mangle_net_ops);
	kfree(mangle_ops);
>>>>>>> v4.9.227
}

module_init(iptable_mangle_init);
module_exit(iptable_mangle_fini);
