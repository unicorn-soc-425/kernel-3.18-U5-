#ifndef __NET_TC_PED_H
#define __NET_TC_PED_H

#include <net/act_api.h>

struct tcf_pedit {
<<<<<<< HEAD
	struct tcf_common	common;
=======
	struct tc_action	common;
>>>>>>> v4.9.227
	unsigned char		tcfp_nkeys;
	unsigned char		tcfp_flags;
	struct tc_pedit_key	*tcfp_keys;
};
<<<<<<< HEAD
#define to_pedit(a) \
	container_of(a->priv, struct tcf_pedit, common)
=======
#define to_pedit(a) ((struct tcf_pedit *)a)
>>>>>>> v4.9.227

#endif /* __NET_TC_PED_H */
