/*
 * Copyright (c) 2008 Patrick McHardy <kaber@trash.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Development of this code funded by Astaro AG (http://www.astaro.com/)
 */

<<<<<<< HEAD
=======
#include <linux/kernel.h>
>>>>>>> v4.9.227
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/rculist.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/netfilter.h>
<<<<<<< HEAD
=======
#include <linux/static_key.h>
>>>>>>> v4.9.227
#include <linux/netfilter/nfnetlink.h>
#include <linux/netfilter/nf_tables.h>
#include <net/netfilter/nf_tables_core.h>
#include <net/netfilter/nf_tables.h>
#include <net/netfilter/nf_log.h>

<<<<<<< HEAD
static void nft_cmp_fast_eval(const struct nft_expr *expr,
			      struct nft_data data[NFT_REG_MAX + 1])
=======
static const char *const comments[__NFT_TRACETYPE_MAX] = {
	[NFT_TRACETYPE_POLICY]	= "policy",
	[NFT_TRACETYPE_RETURN]	= "return",
	[NFT_TRACETYPE_RULE]	= "rule",
};

static struct nf_loginfo trace_loginfo = {
	.type = NF_LOG_TYPE_LOG,
	.u = {
		.log = {
			.level = LOGLEVEL_WARNING,
			.logflags = NF_LOG_DEFAULT_MASK,
	        },
	},
};

static noinline void __nft_trace_packet(struct nft_traceinfo *info,
					const struct nft_chain *chain,
					int rulenum, enum nft_trace_types type)
{
	const struct nft_pktinfo *pkt = info->pkt;

	if (!info->trace || !pkt->skb->nf_trace)
		return;

	info->chain = chain;
	info->type = type;

	nft_trace_notify(info);

	nf_log_trace(pkt->net, pkt->pf, pkt->hook, pkt->skb, pkt->in,
		     pkt->out, &trace_loginfo, "TRACE: %s:%s:%s:%u ",
		     chain->table->name, chain->name, comments[type],
		     rulenum);
}

static inline void nft_trace_packet(struct nft_traceinfo *info,
				    const struct nft_chain *chain,
				    const struct nft_rule *rule,
				    int rulenum,
				    enum nft_trace_types type)
{
	if (static_branch_unlikely(&nft_trace_enabled)) {
		info->rule = rule;
		__nft_trace_packet(info, chain, rulenum, type);
	}
}

static void nft_cmp_fast_eval(const struct nft_expr *expr,
			      struct nft_regs *regs)
>>>>>>> v4.9.227
{
	const struct nft_cmp_fast_expr *priv = nft_expr_priv(expr);
	u32 mask = nft_cmp_fast_mask(priv->len);

<<<<<<< HEAD
	if ((data[priv->sreg].data[0] & mask) == priv->data)
		return;
	data[NFT_REG_VERDICT].verdict = NFT_BREAK;
}

static bool nft_payload_fast_eval(const struct nft_expr *expr,
				  struct nft_data data[NFT_REG_MAX + 1],
=======
	if ((regs->data[priv->sreg] & mask) == priv->data)
		return;
	regs->verdict.code = NFT_BREAK;
}

static bool nft_payload_fast_eval(const struct nft_expr *expr,
				  struct nft_regs *regs,
>>>>>>> v4.9.227
				  const struct nft_pktinfo *pkt)
{
	const struct nft_payload *priv = nft_expr_priv(expr);
	const struct sk_buff *skb = pkt->skb;
<<<<<<< HEAD
	struct nft_data *dest = &data[priv->dreg];
=======
	u32 *dest = &regs->data[priv->dreg];
>>>>>>> v4.9.227
	unsigned char *ptr;

	if (priv->base == NFT_PAYLOAD_NETWORK_HEADER)
		ptr = skb_network_header(skb);
<<<<<<< HEAD
	else
		ptr = skb_network_header(skb) + pkt->xt.thoff;

	ptr += priv->offset;

	if (unlikely(ptr + priv->len >= skb_tail_pointer(skb)))
		return false;

	if (priv->len == 2)
		*(u16 *)dest->data = *(u16 *)ptr;
	else if (priv->len == 4)
		*(u32 *)dest->data = *(u32 *)ptr;
	else
		*(u8 *)dest->data = *(u8 *)ptr;
=======
	else {
		if (!pkt->tprot_set)
			return false;
		ptr = skb_network_header(skb) + pkt->xt.thoff;
	}

	ptr += priv->offset;

	if (unlikely(ptr + priv->len > skb_tail_pointer(skb)))
		return false;

	*dest = 0;
	if (priv->len == 2)
		*(u16 *)dest = *(u16 *)ptr;
	else if (priv->len == 4)
		*(u32 *)dest = *(u32 *)ptr;
	else
		*(u8 *)dest = *(u8 *)ptr;
>>>>>>> v4.9.227
	return true;
}

struct nft_jumpstack {
	const struct nft_chain	*chain;
	const struct nft_rule	*rule;
	int			rulenum;
};

<<<<<<< HEAD
enum nft_trace {
	NFT_TRACE_RULE,
	NFT_TRACE_RETURN,
	NFT_TRACE_POLICY,
};

static const char *const comments[] = {
	[NFT_TRACE_RULE]	= "rule",
	[NFT_TRACE_RETURN]	= "return",
	[NFT_TRACE_POLICY]	= "policy",
};

static struct nf_loginfo trace_loginfo = {
	.type = NF_LOG_TYPE_LOG,
	.u = {
		.log = {
			.level = 4,
			.logflags = NF_LOG_MASK,
	        },
	},
};

static void nft_trace_packet(const struct nft_pktinfo *pkt,
			     const struct nft_chain *chain,
			     int rulenum, enum nft_trace type)
{
	struct net *net = dev_net(pkt->in ? pkt->in : pkt->out);

	nf_log_packet(net, pkt->xt.family, pkt->ops->hooknum, pkt->skb, pkt->in,
		      pkt->out, &trace_loginfo, "TRACE: %s:%s:%s:%u ",
		      chain->table->name, chain->name, comments[type],
		      rulenum);
}

unsigned int
nft_do_chain(struct nft_pktinfo *pkt, const struct nf_hook_ops *ops)
{
	const struct nft_chain *chain = ops->priv, *basechain = chain;
	const struct nft_rule *rule;
	const struct nft_expr *expr, *last;
	struct nft_data data[NFT_REG_MAX + 1];
=======
unsigned int
nft_do_chain(struct nft_pktinfo *pkt, void *priv)
{
	const struct nft_chain *chain = priv, *basechain = chain;
	const struct net *net = pkt->net;
	const struct nft_rule *rule;
	const struct nft_expr *expr, *last;
	struct nft_regs regs;
>>>>>>> v4.9.227
	unsigned int stackptr = 0;
	struct nft_jumpstack jumpstack[NFT_JUMP_STACK_SIZE];
	struct nft_stats *stats;
	int rulenum;
<<<<<<< HEAD
	/*
	 * Cache cursor to avoid problems in case that the cursor is updated
	 * while traversing the ruleset.
	 */
	unsigned int gencursor = ACCESS_ONCE(chain->net->nft.gencursor);

=======
	unsigned int gencursor = nft_genmask_cur(net);
	struct nft_traceinfo info;

	info.trace = false;
	if (static_branch_unlikely(&nft_trace_enabled))
		nft_trace_init(&info, pkt, &regs.verdict, basechain);
>>>>>>> v4.9.227
do_chain:
	rulenum = 0;
	rule = list_entry(&chain->rules, struct nft_rule, list);
next_rule:
<<<<<<< HEAD
	data[NFT_REG_VERDICT].verdict = NFT_CONTINUE;
	list_for_each_entry_continue_rcu(rule, &chain->rules, list) {

		/* This rule is not active, skip. */
		if (unlikely(rule->genmask & (1 << gencursor)))
=======
	regs.verdict.code = NFT_CONTINUE;
	list_for_each_entry_continue_rcu(rule, &chain->rules, list) {

		/* This rule is not active, skip. */
		if (unlikely(rule->genmask & gencursor))
>>>>>>> v4.9.227
			continue;

		rulenum++;

		nft_rule_for_each_expr(expr, last, rule) {
			if (expr->ops == &nft_cmp_fast_ops)
<<<<<<< HEAD
				nft_cmp_fast_eval(expr, data);
			else if (expr->ops != &nft_payload_fast_ops ||
				 !nft_payload_fast_eval(expr, data, pkt))
				expr->ops->eval(expr, data, pkt);

			if (data[NFT_REG_VERDICT].verdict != NFT_CONTINUE)
				break;
		}

		switch (data[NFT_REG_VERDICT].verdict) {
		case NFT_BREAK:
			data[NFT_REG_VERDICT].verdict = NFT_CONTINUE;
			continue;
		case NFT_CONTINUE:
			if (unlikely(pkt->skb->nf_trace))
				nft_trace_packet(pkt, chain, rulenum, NFT_TRACE_RULE);
=======
				nft_cmp_fast_eval(expr, &regs);
			else if (expr->ops != &nft_payload_fast_ops ||
				 !nft_payload_fast_eval(expr, &regs, pkt))
				expr->ops->eval(expr, &regs, pkt);

			if (regs.verdict.code != NFT_CONTINUE)
				break;
		}

		switch (regs.verdict.code) {
		case NFT_BREAK:
			regs.verdict.code = NFT_CONTINUE;
			continue;
		case NFT_CONTINUE:
			nft_trace_packet(&info, chain, rule,
					 rulenum, NFT_TRACETYPE_RULE);
>>>>>>> v4.9.227
			continue;
		}
		break;
	}

<<<<<<< HEAD
	switch (data[NFT_REG_VERDICT].verdict & NF_VERDICT_MASK) {
	case NF_ACCEPT:
	case NF_DROP:
	case NF_QUEUE:
		if (unlikely(pkt->skb->nf_trace))
			nft_trace_packet(pkt, chain, rulenum, NFT_TRACE_RULE);

		return data[NFT_REG_VERDICT].verdict;
	}

	switch (data[NFT_REG_VERDICT].verdict) {
	case NFT_JUMP:
		if (unlikely(pkt->skb->nf_trace))
			nft_trace_packet(pkt, chain, rulenum, NFT_TRACE_RULE);

		BUG_ON(stackptr >= NFT_JUMP_STACK_SIZE);
=======
	switch (regs.verdict.code & NF_VERDICT_MASK) {
	case NF_ACCEPT:
	case NF_DROP:
	case NF_QUEUE:
		nft_trace_packet(&info, chain, rule,
				 rulenum, NFT_TRACETYPE_RULE);
		return regs.verdict.code;
	}

	switch (regs.verdict.code) {
	case NFT_JUMP:
		if (WARN_ON_ONCE(stackptr >= NFT_JUMP_STACK_SIZE))
			return NF_DROP;
>>>>>>> v4.9.227
		jumpstack[stackptr].chain = chain;
		jumpstack[stackptr].rule  = rule;
		jumpstack[stackptr].rulenum = rulenum;
		stackptr++;
<<<<<<< HEAD
		chain = data[NFT_REG_VERDICT].chain;
		goto do_chain;
	case NFT_GOTO:
		if (unlikely(pkt->skb->nf_trace))
			nft_trace_packet(pkt, chain, rulenum, NFT_TRACE_RULE);

		chain = data[NFT_REG_VERDICT].chain;
		goto do_chain;
	case NFT_RETURN:
		if (unlikely(pkt->skb->nf_trace))
			nft_trace_packet(pkt, chain, rulenum, NFT_TRACE_RETURN);
		break;
	case NFT_CONTINUE:
		if (unlikely(pkt->skb->nf_trace && !(chain->flags & NFT_BASE_CHAIN)))
			nft_trace_packet(pkt, chain, ++rulenum, NFT_TRACE_RETURN);
=======
		/* fall through */
	case NFT_GOTO:
		nft_trace_packet(&info, chain, rule,
				 rulenum, NFT_TRACETYPE_RULE);

		chain = regs.verdict.chain;
		goto do_chain;
	case NFT_CONTINUE:
		rulenum++;
		/* fall through */
	case NFT_RETURN:
		nft_trace_packet(&info, chain, rule,
				 rulenum, NFT_TRACETYPE_RETURN);
>>>>>>> v4.9.227
		break;
	default:
		WARN_ON(1);
	}

	if (stackptr > 0) {
		stackptr--;
		chain = jumpstack[stackptr].chain;
		rule  = jumpstack[stackptr].rule;
		rulenum = jumpstack[stackptr].rulenum;
		goto next_rule;
	}

<<<<<<< HEAD
	if (unlikely(pkt->skb->nf_trace))
		nft_trace_packet(pkt, basechain, -1, NFT_TRACE_POLICY);
=======
	nft_trace_packet(&info, basechain, NULL, -1,
			 NFT_TRACETYPE_POLICY);
>>>>>>> v4.9.227

	rcu_read_lock_bh();
	stats = this_cpu_ptr(rcu_dereference(nft_base_chain(basechain)->stats));
	u64_stats_update_begin(&stats->syncp);
	stats->pkts++;
	stats->bytes += pkt->skb->len;
	u64_stats_update_end(&stats->syncp);
	rcu_read_unlock_bh();

	return nft_base_chain(basechain)->policy;
}
EXPORT_SYMBOL_GPL(nft_do_chain);

int __init nf_tables_core_module_init(void)
{
	int err;

	err = nft_immediate_module_init();
	if (err < 0)
		goto err1;

	err = nft_cmp_module_init();
	if (err < 0)
		goto err2;

	err = nft_lookup_module_init();
	if (err < 0)
		goto err3;

	err = nft_bitwise_module_init();
	if (err < 0)
		goto err4;

	err = nft_byteorder_module_init();
	if (err < 0)
		goto err5;

	err = nft_payload_module_init();
	if (err < 0)
		goto err6;

<<<<<<< HEAD
	return 0;

=======
	err = nft_dynset_module_init();
	if (err < 0)
		goto err7;

	err = nft_range_module_init();
	if (err < 0)
		goto err8;

	return 0;
err8:
	nft_dynset_module_exit();
err7:
	nft_payload_module_exit();
>>>>>>> v4.9.227
err6:
	nft_byteorder_module_exit();
err5:
	nft_bitwise_module_exit();
err4:
	nft_lookup_module_exit();
err3:
	nft_cmp_module_exit();
err2:
	nft_immediate_module_exit();
err1:
	return err;
}

void nf_tables_core_module_exit(void)
{
<<<<<<< HEAD
=======
	nft_dynset_module_exit();
>>>>>>> v4.9.227
	nft_payload_module_exit();
	nft_byteorder_module_exit();
	nft_bitwise_module_exit();
	nft_lookup_module_exit();
	nft_cmp_module_exit();
	nft_immediate_module_exit();
}
