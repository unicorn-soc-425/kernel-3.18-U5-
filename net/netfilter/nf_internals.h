#ifndef _NF_INTERNALS_H
#define _NF_INTERNALS_H

#include <linux/list.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>

#ifdef CONFIG_NETFILTER_DEBUG
#define NFDEBUG(format, args...)  printk(KERN_DEBUG format , ## args)
#else
#define NFDEBUG(format, args...)
#endif


/* core.c */
<<<<<<< HEAD
unsigned int nf_iterate(struct list_head *head, struct sk_buff *skb,
			unsigned int hook, const struct net_device *indev,
			const struct net_device *outdev,
			struct nf_hook_ops **elemp,
			int (*okfn)(struct sk_buff *), int hook_thresh);

/* nf_queue.c */
int nf_queue(struct sk_buff *skb, struct nf_hook_ops *elem, u_int8_t pf,
	     unsigned int hook, struct net_device *indev,
	     struct net_device *outdev, int (*okfn)(struct sk_buff *),
	     unsigned int queuenum);
void nf_queue_nf_hook_drop(struct nf_hook_ops *ops);
=======
unsigned int nf_iterate(struct sk_buff *skb, struct nf_hook_state *state,
			struct nf_hook_entry **entryp);

/* nf_queue.c */
int nf_queue(struct sk_buff *skb, struct nf_hook_state *state,
	     struct nf_hook_entry **entryp, unsigned int verdict);
void nf_queue_nf_hook_drop(struct net *net, const struct nf_hook_entry *entry);
>>>>>>> v4.9.227
int __init netfilter_queue_init(void);

/* nf_log.c */
int __init netfilter_log_init(void);

#endif
