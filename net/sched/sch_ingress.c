<<<<<<< HEAD
/* net/sched/sch_ingress.c - Ingress qdisc
=======
/* net/sched/sch_ingress.c - Ingress and clsact qdisc
 *
>>>>>>> v4.9.227
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 * Authors:     Jamal Hadi Salim 1999
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/skbuff.h>
#include <linux/rtnetlink.h>
<<<<<<< HEAD
#include <net/netlink.h>
#include <net/pkt_sched.h>


struct ingress_qdisc_data {
	struct tcf_proto __rcu	*filter_list;
};

/* ------------------------- Class/flow operations ------------------------- */

=======

#include <net/netlink.h>
#include <net/pkt_sched.h>

>>>>>>> v4.9.227
static struct Qdisc *ingress_leaf(struct Qdisc *sch, unsigned long arg)
{
	return NULL;
}

static unsigned long ingress_get(struct Qdisc *sch, u32 classid)
{
	return TC_H_MIN(classid) + 1;
}

<<<<<<< HEAD
=======
static bool ingress_cl_offload(u32 classid)
{
	return true;
}

>>>>>>> v4.9.227
static unsigned long ingress_bind_filter(struct Qdisc *sch,
					 unsigned long parent, u32 classid)
{
	return ingress_get(sch, classid);
}

static void ingress_put(struct Qdisc *sch, unsigned long cl)
{
}

static void ingress_walk(struct Qdisc *sch, struct qdisc_walker *walker)
{
}

static struct tcf_proto __rcu **ingress_find_tcf(struct Qdisc *sch,
						 unsigned long cl)
{
<<<<<<< HEAD
	struct ingress_qdisc_data *p = qdisc_priv(sch);

	return &p->filter_list;
}

/* --------------------------- Qdisc operations ---------------------------- */

static int ingress_enqueue(struct sk_buff *skb, struct Qdisc *sch)
{
	struct ingress_qdisc_data *p = qdisc_priv(sch);
	struct tcf_result res;
	struct tcf_proto *fl = rcu_dereference_bh(p->filter_list);
	int result;

	result = tc_classify(skb, fl, &res);

	qdisc_bstats_update(sch, skb);
	switch (result) {
	case TC_ACT_SHOT:
		result = TC_ACT_SHOT;
		qdisc_qstats_drop(sch);
		break;
	case TC_ACT_STOLEN:
	case TC_ACT_QUEUED:
		result = TC_ACT_STOLEN;
		break;
	case TC_ACT_RECLASSIFY:
	case TC_ACT_OK:
		skb->tc_index = TC_H_MIN(res.classid);
	default:
		result = TC_ACT_OK;
		break;
	}

	return result;
}

/* ------------------------------------------------------------- */

static void ingress_destroy(struct Qdisc *sch)
{
	struct ingress_qdisc_data *p = qdisc_priv(sch);

	tcf_destroy_chain(&p->filter_list);
=======
	struct net_device *dev = qdisc_dev(sch);

	return &dev->ingress_cl_list;
}

static int ingress_init(struct Qdisc *sch, struct nlattr *opt)
{
	net_inc_ingress_queue();
	sch->flags |= TCQ_F_CPUSTATS;

	return 0;
}

static void ingress_destroy(struct Qdisc *sch)
{
	struct net_device *dev = qdisc_dev(sch);

	tcf_destroy_chain(&dev->ingress_cl_list);
	net_dec_ingress_queue();
>>>>>>> v4.9.227
}

static int ingress_dump(struct Qdisc *sch, struct sk_buff *skb)
{
	struct nlattr *nest;

	nest = nla_nest_start(skb, TCA_OPTIONS);
	if (nest == NULL)
		goto nla_put_failure;
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
	return nla_nest_end(skb, nest);

nla_put_failure:
	nla_nest_cancel(skb, nest);
	return -1;
}

static const struct Qdisc_class_ops ingress_class_ops = {
	.leaf		=	ingress_leaf,
	.get		=	ingress_get,
	.put		=	ingress_put,
	.walk		=	ingress_walk,
	.tcf_chain	=	ingress_find_tcf,
<<<<<<< HEAD
=======
	.tcf_cl_offload	=	ingress_cl_offload,
>>>>>>> v4.9.227
	.bind_tcf	=	ingress_bind_filter,
	.unbind_tcf	=	ingress_put,
};

static struct Qdisc_ops ingress_qdisc_ops __read_mostly = {
	.cl_ops		=	&ingress_class_ops,
	.id		=	"ingress",
<<<<<<< HEAD
	.priv_size	=	sizeof(struct ingress_qdisc_data),
	.enqueue	=	ingress_enqueue,
=======
	.init		=	ingress_init,
>>>>>>> v4.9.227
	.destroy	=	ingress_destroy,
	.dump		=	ingress_dump,
	.owner		=	THIS_MODULE,
};

<<<<<<< HEAD
static int __init ingress_module_init(void)
{
	return register_qdisc(&ingress_qdisc_ops);
=======
static unsigned long clsact_get(struct Qdisc *sch, u32 classid)
{
	switch (TC_H_MIN(classid)) {
	case TC_H_MIN(TC_H_MIN_INGRESS):
	case TC_H_MIN(TC_H_MIN_EGRESS):
		return TC_H_MIN(classid);
	default:
		return 0;
	}
}

static bool clsact_cl_offload(u32 classid)
{
	return TC_H_MIN(classid) == TC_H_MIN(TC_H_MIN_INGRESS);
}

static unsigned long clsact_bind_filter(struct Qdisc *sch,
					unsigned long parent, u32 classid)
{
	return clsact_get(sch, classid);
}

static struct tcf_proto __rcu **clsact_find_tcf(struct Qdisc *sch,
						unsigned long cl)
{
	struct net_device *dev = qdisc_dev(sch);

	switch (cl) {
	case TC_H_MIN(TC_H_MIN_INGRESS):
		return &dev->ingress_cl_list;
	case TC_H_MIN(TC_H_MIN_EGRESS):
		return &dev->egress_cl_list;
	default:
		return NULL;
	}
}

static int clsact_init(struct Qdisc *sch, struct nlattr *opt)
{
	net_inc_ingress_queue();
	net_inc_egress_queue();

	sch->flags |= TCQ_F_CPUSTATS;

	return 0;
}

static void clsact_destroy(struct Qdisc *sch)
{
	struct net_device *dev = qdisc_dev(sch);

	tcf_destroy_chain(&dev->ingress_cl_list);
	tcf_destroy_chain(&dev->egress_cl_list);

	net_dec_ingress_queue();
	net_dec_egress_queue();
}

static const struct Qdisc_class_ops clsact_class_ops = {
	.leaf		=	ingress_leaf,
	.get		=	clsact_get,
	.put		=	ingress_put,
	.walk		=	ingress_walk,
	.tcf_chain	=	clsact_find_tcf,
	.tcf_cl_offload	=	clsact_cl_offload,
	.bind_tcf	=	clsact_bind_filter,
	.unbind_tcf	=	ingress_put,
};

static struct Qdisc_ops clsact_qdisc_ops __read_mostly = {
	.cl_ops		=	&clsact_class_ops,
	.id		=	"clsact",
	.init		=	clsact_init,
	.destroy	=	clsact_destroy,
	.dump		=	ingress_dump,
	.owner		=	THIS_MODULE,
};

static int __init ingress_module_init(void)
{
	int ret;

	ret = register_qdisc(&ingress_qdisc_ops);
	if (!ret) {
		ret = register_qdisc(&clsact_qdisc_ops);
		if (ret)
			unregister_qdisc(&ingress_qdisc_ops);
	}

	return ret;
>>>>>>> v4.9.227
}

static void __exit ingress_module_exit(void)
{
	unregister_qdisc(&ingress_qdisc_ops);
<<<<<<< HEAD
}

module_init(ingress_module_init)
module_exit(ingress_module_exit)
=======
	unregister_qdisc(&clsact_qdisc_ops);
}

module_init(ingress_module_init);
module_exit(ingress_module_exit);

MODULE_ALIAS("sch_clsact");
>>>>>>> v4.9.227
MODULE_LICENSE("GPL");
