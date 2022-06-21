#ifndef __NET_TC_DEF_H
#define __NET_TC_DEF_H

#include <net/act_api.h>

struct tcf_defact {
<<<<<<< HEAD
	struct tcf_common	common;
	u32     		tcfd_datalen;
	void    		*tcfd_defdata;
};
#define to_defact(a) \
	container_of(a->priv, struct tcf_defact, common)
=======
	struct tc_action	common;
	u32		tcfd_datalen;
	void		*tcfd_defdata;
};
#define to_defact(a) ((struct tcf_defact *)a)
>>>>>>> v4.9.227

#endif /* __NET_TC_DEF_H */
