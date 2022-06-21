/*
 * IPv6 raw table, a port of the IPv4 raw table to IPv6
 *
 * Copyright (C) 2003 Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 */
#include <linux/module.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <linux/slab.h>

#define RAW_VALID_HOOKS ((1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_LOCAL_OUT))

<<<<<<< HEAD
=======
static int __net_init ip6table_raw_table_init(struct net *net);

>>>>>>> v4.9.227
static const struct xt_table packet_raw = {
	.name = "raw",
	.valid_hooks = RAW_VALID_HOOKS,
	.me = THIS_MODULE,
	.af = NFPROTO_IPV6,
	.priority = NF_IP6_PRI_RAW,
<<<<<<< HEAD
=======
	.table_init = ip6table_raw_table_init,
>>>>>>> v4.9.227
};

/* The work comes in here from netfilter.c. */
static unsigned int
<<<<<<< HEAD
ip6table_raw_hook(const struct nf_hook_ops *ops, struct sk_buff *skb,
		  const struct net_device *in, const struct net_device *out,
		  int (*okfn)(struct sk_buff *))
{
	const struct net *net = dev_net((in != NULL) ? in : out);

	return ip6t_do_table(skb, ops->hooknum, in, out,
			     net->ipv6.ip6table_raw);
=======
ip6table_raw_hook(void *priv, struct sk_buff *skb,
		  const struct nf_hook_state *state)
{
	return ip6t_do_table(skb, state, state->net->ipv6.ip6table_raw);
>>>>>>> v4.9.227
}

static struct nf_hook_ops *rawtable_ops __read_mostly;

<<<<<<< HEAD
static int __net_init ip6table_raw_net_init(struct net *net)
{
	struct ip6t_replace *repl;
=======
static int __net_init ip6table_raw_table_init(struct net *net)
{
	struct ip6t_replace *repl;
	int ret;

	if (net->ipv6.ip6table_raw)
		return 0;
>>>>>>> v4.9.227

	repl = ip6t_alloc_initial_table(&packet_raw);
	if (repl == NULL)
		return -ENOMEM;
<<<<<<< HEAD
	net->ipv6.ip6table_raw =
		ip6t_register_table(net, &packet_raw, repl);
	kfree(repl);
	return PTR_ERR_OR_ZERO(net->ipv6.ip6table_raw);
=======
	ret = ip6t_register_table(net, &packet_raw, repl, rawtable_ops,
				  &net->ipv6.ip6table_raw);
	kfree(repl);
	return ret;
>>>>>>> v4.9.227
}

static void __net_exit ip6table_raw_net_exit(struct net *net)
{
<<<<<<< HEAD
	ip6t_unregister_table(net, net->ipv6.ip6table_raw);
}

static struct pernet_operations ip6table_raw_net_ops = {
	.init = ip6table_raw_net_init,
=======
	if (!net->ipv6.ip6table_raw)
		return;
	ip6t_unregister_table(net, net->ipv6.ip6table_raw, rawtable_ops);
	net->ipv6.ip6table_raw = NULL;
}

static struct pernet_operations ip6table_raw_net_ops = {
>>>>>>> v4.9.227
	.exit = ip6table_raw_net_exit,
};

static int __init ip6table_raw_init(void)
{
	int ret;

<<<<<<< HEAD
	ret = register_pernet_subsys(&ip6table_raw_net_ops);
	if (ret < 0)
		return ret;

	/* Register hooks */
	rawtable_ops = xt_hook_link(&packet_raw, ip6table_raw_hook);
	if (IS_ERR(rawtable_ops)) {
		ret = PTR_ERR(rawtable_ops);
		goto cleanup_table;
	}

	return ret;

 cleanup_table:
	unregister_pernet_subsys(&ip6table_raw_net_ops);
=======
	/* Register hooks */
	rawtable_ops = xt_hook_ops_alloc(&packet_raw, ip6table_raw_hook);
	if (IS_ERR(rawtable_ops))
		return PTR_ERR(rawtable_ops);

	ret = register_pernet_subsys(&ip6table_raw_net_ops);
	if (ret < 0) {
		kfree(rawtable_ops);
		return ret;
	}

	ret = ip6table_raw_table_init(&init_net);
	if (ret) {
		unregister_pernet_subsys(&ip6table_raw_net_ops);
		kfree(rawtable_ops);
	}
>>>>>>> v4.9.227
	return ret;
}

static void __exit ip6table_raw_fini(void)
{
<<<<<<< HEAD
	xt_hook_unlink(&packet_raw, rawtable_ops);
	unregister_pernet_subsys(&ip6table_raw_net_ops);
=======
	unregister_pernet_subsys(&ip6table_raw_net_ops);
	kfree(rawtable_ops);
>>>>>>> v4.9.227
}

module_init(ip6table_raw_init);
module_exit(ip6table_raw_fini);
MODULE_LICENSE("GPL");
