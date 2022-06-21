/*
 * 'raw' table, which is the very first hooked in at PRE_ROUTING and LOCAL_OUT .
 *
 * Copyright (C) 2003 Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 */
#include <linux/module.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/slab.h>
#include <net/ip.h>

#define RAW_VALID_HOOKS ((1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_LOCAL_OUT))

<<<<<<< HEAD
=======
static int __net_init iptable_raw_table_init(struct net *net);

>>>>>>> v4.9.227
static const struct xt_table packet_raw = {
	.name = "raw",
	.valid_hooks =  RAW_VALID_HOOKS,
	.me = THIS_MODULE,
	.af = NFPROTO_IPV4,
	.priority = NF_IP_PRI_RAW,
<<<<<<< HEAD
=======
	.table_init = iptable_raw_table_init,
>>>>>>> v4.9.227
};

/* The work comes in here from netfilter.c. */
static unsigned int
<<<<<<< HEAD
iptable_raw_hook(const struct nf_hook_ops *ops, struct sk_buff *skb,
		 const struct net_device *in, const struct net_device *out,
		 int (*okfn)(struct sk_buff *))
{
	const struct net *net;

	if (ops->hooknum == NF_INET_LOCAL_OUT &&
=======
iptable_raw_hook(void *priv, struct sk_buff *skb,
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
	return ipt_do_table(skb, ops->hooknum, in, out, net->ipv4.iptable_raw);
=======
	return ipt_do_table(skb, state, state->net->ipv4.iptable_raw);
>>>>>>> v4.9.227
}

static struct nf_hook_ops *rawtable_ops __read_mostly;

<<<<<<< HEAD
static int __net_init iptable_raw_net_init(struct net *net)
{
	struct ipt_replace *repl;
=======
static int __net_init iptable_raw_table_init(struct net *net)
{
	struct ipt_replace *repl;
	int ret;

	if (net->ipv4.iptable_raw)
		return 0;
>>>>>>> v4.9.227

	repl = ipt_alloc_initial_table(&packet_raw);
	if (repl == NULL)
		return -ENOMEM;
<<<<<<< HEAD
	net->ipv4.iptable_raw =
		ipt_register_table(net, &packet_raw, repl);
	kfree(repl);
	return PTR_ERR_OR_ZERO(net->ipv4.iptable_raw);
=======
	ret = ipt_register_table(net, &packet_raw, repl, rawtable_ops,
				 &net->ipv4.iptable_raw);
	kfree(repl);
	return ret;
>>>>>>> v4.9.227
}

static void __net_exit iptable_raw_net_exit(struct net *net)
{
<<<<<<< HEAD
	ipt_unregister_table(net, net->ipv4.iptable_raw);
}

static struct pernet_operations iptable_raw_net_ops = {
	.init = iptable_raw_net_init,
=======
	if (!net->ipv4.iptable_raw)
		return;
	ipt_unregister_table(net, net->ipv4.iptable_raw, rawtable_ops);
	net->ipv4.iptable_raw = NULL;
}

static struct pernet_operations iptable_raw_net_ops = {
>>>>>>> v4.9.227
	.exit = iptable_raw_net_exit,
};

static int __init iptable_raw_init(void)
{
	int ret;

<<<<<<< HEAD
	ret = register_pernet_subsys(&iptable_raw_net_ops);
	if (ret < 0)
		return ret;

	/* Register hooks */
	rawtable_ops = xt_hook_link(&packet_raw, iptable_raw_hook);
	if (IS_ERR(rawtable_ops)) {
		ret = PTR_ERR(rawtable_ops);
		unregister_pernet_subsys(&iptable_raw_net_ops);
=======
	rawtable_ops = xt_hook_ops_alloc(&packet_raw, iptable_raw_hook);
	if (IS_ERR(rawtable_ops))
		return PTR_ERR(rawtable_ops);

	ret = register_pernet_subsys(&iptable_raw_net_ops);
	if (ret < 0) {
		kfree(rawtable_ops);
		return ret;
	}

	ret = iptable_raw_table_init(&init_net);
	if (ret) {
		unregister_pernet_subsys(&iptable_raw_net_ops);
		kfree(rawtable_ops);
>>>>>>> v4.9.227
	}

	return ret;
}

static void __exit iptable_raw_fini(void)
{
<<<<<<< HEAD
	xt_hook_unlink(&packet_raw, rawtable_ops);
	unregister_pernet_subsys(&iptable_raw_net_ops);
=======
	unregister_pernet_subsys(&iptable_raw_net_ops);
	kfree(rawtable_ops);
>>>>>>> v4.9.227
}

module_init(iptable_raw_init);
module_exit(iptable_raw_fini);
MODULE_LICENSE("GPL");
