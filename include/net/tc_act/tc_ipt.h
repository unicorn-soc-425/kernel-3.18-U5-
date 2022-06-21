#ifndef __NET_TC_IPT_H
#define __NET_TC_IPT_H

#include <net/act_api.h>

struct xt_entry_target;

struct tcf_ipt {
<<<<<<< HEAD
	struct tcf_common	common;
=======
	struct tc_action	common;
>>>>>>> v4.9.227
	u32			tcfi_hook;
	char			*tcfi_tname;
	struct xt_entry_target	*tcfi_t;
};
<<<<<<< HEAD
#define to_ipt(a) \
	container_of(a->priv, struct tcf_ipt, common)
=======
#define to_ipt(a) ((struct tcf_ipt *)a)
>>>>>>> v4.9.227

#endif /* __NET_TC_IPT_H */
