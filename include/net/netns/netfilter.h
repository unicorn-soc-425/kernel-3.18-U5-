#ifndef __NETNS_NETFILTER_H
#define __NETNS_NETFILTER_H

<<<<<<< HEAD
#include <linux/proc_fs.h>
#include <linux/netfilter.h>

struct nf_logger;
=======
#include <linux/netfilter_defs.h>

struct proc_dir_entry;
struct nf_logger;
struct nf_queue_handler;
>>>>>>> v4.9.227

struct netns_nf {
#if defined CONFIG_PROC_FS
	struct proc_dir_entry *proc_netfilter;
#endif
<<<<<<< HEAD
=======
	const struct nf_queue_handler __rcu *queue_handler;
>>>>>>> v4.9.227
	const struct nf_logger __rcu *nf_loggers[NFPROTO_NUMPROTO];
#ifdef CONFIG_SYSCTL
	struct ctl_table_header *nf_log_dir_header;
#endif
<<<<<<< HEAD
=======
	struct nf_hook_entry __rcu *hooks[NFPROTO_NUMPROTO][NF_MAX_HOOKS];
>>>>>>> v4.9.227
};
#endif
