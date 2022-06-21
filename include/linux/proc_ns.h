/*
 * procfs namespace bits
 */
#ifndef _LINUX_PROC_NS_H
#define _LINUX_PROC_NS_H

<<<<<<< HEAD
struct pid_namespace;
struct nsproxy;
=======
#include <linux/ns_common.h>

struct pid_namespace;
struct nsproxy;
struct path;
struct task_struct;
struct inode;
>>>>>>> v4.9.227

struct proc_ns_operations {
	const char *name;
	int type;
<<<<<<< HEAD
	void *(*get)(struct task_struct *task);
	void (*put)(void *ns);
	int (*install)(struct nsproxy *nsproxy, void *ns);
	unsigned int (*inum)(void *ns);
};

struct proc_ns {
	void *ns;
	const struct proc_ns_operations *ns_ops;
=======
	struct ns_common *(*get)(struct task_struct *task);
	void (*put)(struct ns_common *ns);
	int (*install)(struct nsproxy *nsproxy, struct ns_common *ns);
	struct user_namespace *(*owner)(struct ns_common *ns);
	struct ns_common *(*get_parent)(struct ns_common *ns);
>>>>>>> v4.9.227
};

extern const struct proc_ns_operations netns_operations;
extern const struct proc_ns_operations utsns_operations;
extern const struct proc_ns_operations ipcns_operations;
extern const struct proc_ns_operations pidns_operations;
extern const struct proc_ns_operations userns_operations;
extern const struct proc_ns_operations mntns_operations;
<<<<<<< HEAD
=======
extern const struct proc_ns_operations cgroupns_operations;
>>>>>>> v4.9.227

/*
 * We always define these enumerators
 */
enum {
	PROC_ROOT_INO		= 1,
	PROC_IPC_INIT_INO	= 0xEFFFFFFFU,
	PROC_UTS_INIT_INO	= 0xEFFFFFFEU,
	PROC_USER_INIT_INO	= 0xEFFFFFFDU,
	PROC_PID_INIT_INO	= 0xEFFFFFFCU,
<<<<<<< HEAD
=======
	PROC_CGROUP_INIT_INO	= 0xEFFFFFFBU,
>>>>>>> v4.9.227
};

#ifdef CONFIG_PROC_FS

extern int pid_ns_prepare_proc(struct pid_namespace *ns);
extern void pid_ns_release_proc(struct pid_namespace *ns);
<<<<<<< HEAD
extern struct file *proc_ns_fget(int fd);
extern struct proc_ns *get_proc_ns(struct inode *);
extern int proc_alloc_inum(unsigned int *pino);
extern void proc_free_inum(unsigned int inum);
extern bool proc_ns_inode(struct inode *inode);
=======
extern int proc_alloc_inum(unsigned int *pino);
extern void proc_free_inum(unsigned int inum);
>>>>>>> v4.9.227

#else /* CONFIG_PROC_FS */

static inline int pid_ns_prepare_proc(struct pid_namespace *ns) { return 0; }
static inline void pid_ns_release_proc(struct pid_namespace *ns) {}

<<<<<<< HEAD
static inline struct file *proc_ns_fget(int fd)
{
	return ERR_PTR(-EINVAL);
}

static inline struct proc_ns *get_proc_ns(struct inode *inode) { return NULL; }

=======
>>>>>>> v4.9.227
static inline int proc_alloc_inum(unsigned int *inum)
{
	*inum = 1;
	return 0;
}
static inline void proc_free_inum(unsigned int inum) {}
<<<<<<< HEAD
static inline bool proc_ns_inode(struct inode *inode) { return false; }

#endif /* CONFIG_PROC_FS */

=======

#endif /* CONFIG_PROC_FS */

static inline int ns_alloc_inum(struct ns_common *ns)
{
	atomic_long_set(&ns->stashed, 0);
	return proc_alloc_inum(&ns->inum);
}

#define ns_free_inum(ns) proc_free_inum((ns)->inum)

extern struct file *proc_ns_fget(int fd);
#define get_proc_ns(inode) ((struct ns_common *)(inode)->i_private)
extern void *ns_get_path(struct path *path, struct task_struct *task,
			const struct proc_ns_operations *ns_ops);

extern int ns_get_name(char *buf, size_t size, struct task_struct *task,
			const struct proc_ns_operations *ns_ops);
extern void nsfs_init(void);

>>>>>>> v4.9.227
#endif /* _LINUX_PROC_NS_H */
