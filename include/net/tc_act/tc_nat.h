#ifndef __NET_TC_NAT_H
#define __NET_TC_NAT_H

#include <linux/types.h>
#include <net/act_api.h>

struct tcf_nat {
<<<<<<< HEAD
	struct tcf_common common;
=======
	struct tc_action common;
>>>>>>> v4.9.227

	__be32 old_addr;
	__be32 new_addr;
	__be32 mask;
	u32 flags;
};

<<<<<<< HEAD
static inline struct tcf_nat *to_tcf_nat(struct tc_action *a)
{
	return container_of(a->priv, struct tcf_nat, common);
}
=======
#define to_tcf_nat(a) ((struct tcf_nat *)a)
>>>>>>> v4.9.227

#endif /* __NET_TC_NAT_H */
