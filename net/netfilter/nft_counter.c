/*
 * Copyright (c) 2008-2009 Patrick McHardy <kaber@trash.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Development of this code funded by Astaro AG (http://www.astaro.com/)
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/seqlock.h>
#include <linux/netlink.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>
#include <net/netfilter/nf_tables.h>

struct nft_counter {
<<<<<<< HEAD
	seqlock_t	lock;
=======
>>>>>>> v4.9.227
	u64		bytes;
	u64		packets;
};

<<<<<<< HEAD
static void nft_counter_eval(const struct nft_expr *expr,
			     struct nft_data data[NFT_REG_MAX + 1],
			     const struct nft_pktinfo *pkt)
{
	struct nft_counter *priv = nft_expr_priv(expr);

	write_seqlock_bh(&priv->lock);
	priv->bytes += pkt->skb->len;
	priv->packets++;
	write_sequnlock_bh(&priv->lock);
=======
struct nft_counter_percpu {
	struct nft_counter	counter;
	struct u64_stats_sync	syncp;
};

struct nft_counter_percpu_priv {
	struct nft_counter_percpu __percpu *counter;
};

static void nft_counter_eval(const struct nft_expr *expr,
			     struct nft_regs *regs,
			     const struct nft_pktinfo *pkt)
{
	struct nft_counter_percpu_priv *priv = nft_expr_priv(expr);
	struct nft_counter_percpu *this_cpu;

	local_bh_disable();
	this_cpu = this_cpu_ptr(priv->counter);
	u64_stats_update_begin(&this_cpu->syncp);
	this_cpu->counter.bytes += pkt->skb->len;
	this_cpu->counter.packets++;
	u64_stats_update_end(&this_cpu->syncp);
	local_bh_enable();
}

static void nft_counter_fetch(const struct nft_counter_percpu __percpu *counter,
			      struct nft_counter *total)
{
	const struct nft_counter_percpu *cpu_stats;
	u64 bytes, packets;
	unsigned int seq;
	int cpu;

	memset(total, 0, sizeof(*total));
	for_each_possible_cpu(cpu) {
		cpu_stats = per_cpu_ptr(counter, cpu);
		do {
			seq	= u64_stats_fetch_begin_irq(&cpu_stats->syncp);
			bytes	= cpu_stats->counter.bytes;
			packets	= cpu_stats->counter.packets;
		} while (u64_stats_fetch_retry_irq(&cpu_stats->syncp, seq));

		total->packets += packets;
		total->bytes += bytes;
	}
>>>>>>> v4.9.227
}

static int nft_counter_dump(struct sk_buff *skb, const struct nft_expr *expr)
{
<<<<<<< HEAD
	struct nft_counter *priv = nft_expr_priv(expr);
	unsigned int seq;
	u64 bytes;
	u64 packets;

	do {
		seq = read_seqbegin(&priv->lock);
		bytes	= priv->bytes;
		packets	= priv->packets;
	} while (read_seqretry(&priv->lock, seq));

	if (nla_put_be64(skb, NFTA_COUNTER_BYTES, cpu_to_be64(bytes)))
		goto nla_put_failure;
	if (nla_put_be64(skb, NFTA_COUNTER_PACKETS, cpu_to_be64(packets)))
=======
	struct nft_counter_percpu_priv *priv = nft_expr_priv(expr);
	struct nft_counter total;

	nft_counter_fetch(priv->counter, &total);

	if (nla_put_be64(skb, NFTA_COUNTER_BYTES, cpu_to_be64(total.bytes),
			 NFTA_COUNTER_PAD) ||
	    nla_put_be64(skb, NFTA_COUNTER_PACKETS, cpu_to_be64(total.packets),
			 NFTA_COUNTER_PAD))
>>>>>>> v4.9.227
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -1;
}

static const struct nla_policy nft_counter_policy[NFTA_COUNTER_MAX + 1] = {
	[NFTA_COUNTER_PACKETS]	= { .type = NLA_U64 },
	[NFTA_COUNTER_BYTES]	= { .type = NLA_U64 },
};

static int nft_counter_init(const struct nft_ctx *ctx,
			    const struct nft_expr *expr,
			    const struct nlattr * const tb[])
{
<<<<<<< HEAD
	struct nft_counter *priv = nft_expr_priv(expr);

	if (tb[NFTA_COUNTER_PACKETS])
	        priv->packets = be64_to_cpu(nla_get_be64(tb[NFTA_COUNTER_PACKETS]));
	if (tb[NFTA_COUNTER_BYTES])
		priv->bytes = be64_to_cpu(nla_get_be64(tb[NFTA_COUNTER_BYTES]));

	seqlock_init(&priv->lock);
=======
	struct nft_counter_percpu_priv *priv = nft_expr_priv(expr);
	struct nft_counter_percpu __percpu *cpu_stats;
	struct nft_counter_percpu *this_cpu;

	cpu_stats = netdev_alloc_pcpu_stats(struct nft_counter_percpu);
	if (cpu_stats == NULL)
		return -ENOMEM;

	preempt_disable();
	this_cpu = this_cpu_ptr(cpu_stats);
	if (tb[NFTA_COUNTER_PACKETS]) {
	        this_cpu->counter.packets =
			be64_to_cpu(nla_get_be64(tb[NFTA_COUNTER_PACKETS]));
	}
	if (tb[NFTA_COUNTER_BYTES]) {
		this_cpu->counter.bytes =
			be64_to_cpu(nla_get_be64(tb[NFTA_COUNTER_BYTES]));
	}
	preempt_enable();
	priv->counter = cpu_stats;
	return 0;
}

static void nft_counter_destroy(const struct nft_ctx *ctx,
				const struct nft_expr *expr)
{
	struct nft_counter_percpu_priv *priv = nft_expr_priv(expr);

	free_percpu(priv->counter);
}

static int nft_counter_clone(struct nft_expr *dst, const struct nft_expr *src)
{
	struct nft_counter_percpu_priv *priv = nft_expr_priv(src);
	struct nft_counter_percpu_priv *priv_clone = nft_expr_priv(dst);
	struct nft_counter_percpu __percpu *cpu_stats;
	struct nft_counter_percpu *this_cpu;
	struct nft_counter total;

	nft_counter_fetch(priv->counter, &total);

	cpu_stats = __netdev_alloc_pcpu_stats(struct nft_counter_percpu,
					      GFP_ATOMIC);
	if (cpu_stats == NULL)
		return -ENOMEM;

	preempt_disable();
	this_cpu = this_cpu_ptr(cpu_stats);
	this_cpu->counter.packets = total.packets;
	this_cpu->counter.bytes = total.bytes;
	preempt_enable();

	priv_clone->counter = cpu_stats;
>>>>>>> v4.9.227
	return 0;
}

static struct nft_expr_type nft_counter_type;
static const struct nft_expr_ops nft_counter_ops = {
	.type		= &nft_counter_type,
<<<<<<< HEAD
	.size		= NFT_EXPR_SIZE(sizeof(struct nft_counter)),
	.eval		= nft_counter_eval,
	.init		= nft_counter_init,
	.dump		= nft_counter_dump,
=======
	.size		= NFT_EXPR_SIZE(sizeof(struct nft_counter_percpu_priv)),
	.eval		= nft_counter_eval,
	.init		= nft_counter_init,
	.destroy	= nft_counter_destroy,
	.dump		= nft_counter_dump,
	.clone		= nft_counter_clone,
>>>>>>> v4.9.227
};

static struct nft_expr_type nft_counter_type __read_mostly = {
	.name		= "counter",
	.ops		= &nft_counter_ops,
	.policy		= nft_counter_policy,
	.maxattr	= NFTA_COUNTER_MAX,
<<<<<<< HEAD
=======
	.flags		= NFT_EXPR_STATEFUL,
>>>>>>> v4.9.227
	.owner		= THIS_MODULE,
};

static int __init nft_counter_module_init(void)
{
	return nft_register_expr(&nft_counter_type);
}

static void __exit nft_counter_module_exit(void)
{
	nft_unregister_expr(&nft_counter_type);
}

module_init(nft_counter_module_init);
module_exit(nft_counter_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Patrick McHardy <kaber@trash.net>");
MODULE_ALIAS_NFT_EXPR("counter");
