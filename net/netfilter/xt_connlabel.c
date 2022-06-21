/*
 * (C) 2013 Astaro GmbH & Co KG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <net/netfilter/nf_conntrack.h>
<<<<<<< HEAD
=======
#include <net/netfilter/nf_conntrack_ecache.h>
>>>>>>> v4.9.227
#include <net/netfilter/nf_conntrack_labels.h>
#include <linux/netfilter/x_tables.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Florian Westphal <fw@strlen.de>");
MODULE_DESCRIPTION("Xtables: add/match connection trackling labels");
MODULE_ALIAS("ipt_connlabel");
MODULE_ALIAS("ip6t_connlabel");

static bool
connlabel_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_connlabel_mtinfo *info = par->matchinfo;
	enum ip_conntrack_info ctinfo;
<<<<<<< HEAD
=======
	struct nf_conn_labels *labels;
>>>>>>> v4.9.227
	struct nf_conn *ct;
	bool invert = info->options & XT_CONNLABEL_OP_INVERT;

	ct = nf_ct_get(skb, &ctinfo);
	if (ct == NULL || nf_ct_is_untracked(ct))
		return invert;

<<<<<<< HEAD
	if (info->options & XT_CONNLABEL_OP_SET)
		return (nf_connlabel_set(ct, info->bit) == 0) ^ invert;

	return nf_connlabel_match(ct, info->bit) ^ invert;
=======
	labels = nf_ct_labels_find(ct);
	if (!labels)
		return invert;

	if (test_bit(info->bit, labels->bits))
		return !invert;

	if (info->options & XT_CONNLABEL_OP_SET) {
		if (!test_and_set_bit(info->bit, labels->bits))
			nf_conntrack_event_cache(IPCT_LABEL, ct);

		return !invert;
	}

	return invert;
>>>>>>> v4.9.227
}

static int connlabel_mt_check(const struct xt_mtchk_param *par)
{
	const int options = XT_CONNLABEL_OP_INVERT |
			    XT_CONNLABEL_OP_SET;
	struct xt_connlabel_mtinfo *info = par->matchinfo;
	int ret;
<<<<<<< HEAD
	size_t words;

	if (info->bit > XT_CONNLABEL_MAXBIT)
		return -ERANGE;
=======
>>>>>>> v4.9.227

	if (info->options & ~options) {
		pr_err("Unknown options in mask %x\n", info->options);
		return -EINVAL;
	}

	ret = nf_ct_l3proto_try_module_get(par->family);
	if (ret < 0) {
		pr_info("cannot load conntrack support for proto=%u\n",
							par->family);
		return ret;
	}

<<<<<<< HEAD
	par->net->ct.labels_used++;
	words = BITS_TO_LONGS(info->bit+1);
	if (words > par->net->ct.label_words)
		par->net->ct.label_words = words;

=======
	ret = nf_connlabels_get(par->net, info->bit);
	if (ret < 0)
		nf_ct_l3proto_module_put(par->family);
>>>>>>> v4.9.227
	return ret;
}

static void connlabel_mt_destroy(const struct xt_mtdtor_param *par)
{
<<<<<<< HEAD
	par->net->ct.labels_used--;
	if (par->net->ct.labels_used == 0)
		par->net->ct.label_words = 0;
=======
	nf_connlabels_put(par->net);
>>>>>>> v4.9.227
	nf_ct_l3proto_module_put(par->family);
}

static struct xt_match connlabels_mt_reg __read_mostly = {
	.name           = "connlabel",
	.family         = NFPROTO_UNSPEC,
	.checkentry     = connlabel_mt_check,
	.match          = connlabel_mt,
	.matchsize      = sizeof(struct xt_connlabel_mtinfo),
	.destroy        = connlabel_mt_destroy,
	.me             = THIS_MODULE,
};

static int __init connlabel_mt_init(void)
{
	return xt_register_match(&connlabels_mt_reg);
}

static void __exit connlabel_mt_exit(void)
{
	xt_unregister_match(&connlabels_mt_reg);
}

module_init(connlabel_mt_init);
module_exit(connlabel_mt_exit);
