#ifndef __NET_TC_CSUM_H
#define __NET_TC_CSUM_H

#include <linux/types.h>
#include <net/act_api.h>

struct tcf_csum {
<<<<<<< HEAD
	struct tcf_common common;

	u32 update_flags;
};
#define to_tcf_csum(a) \
	container_of(a->priv,struct tcf_csum,common)
=======
	struct tc_action common;

	u32 update_flags;
};
#define to_tcf_csum(a) ((struct tcf_csum *)a)
>>>>>>> v4.9.227

#endif /* __NET_TC_CSUM_H */
