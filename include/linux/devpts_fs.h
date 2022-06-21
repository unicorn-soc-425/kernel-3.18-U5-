/* -*- linux-c -*- --------------------------------------------------------- *
 *
 * linux/include/linux/devpts_fs.h
 *
 *  Copyright 1998-2004 H. Peter Anvin -- All Rights Reserved
 *
 * This file is part of the Linux kernel and is made available under
 * the terms of the GNU General Public License, version 2, or at your
 * option, any later version, incorporated herein by reference.
 *
 * ------------------------------------------------------------------------- */

#ifndef _LINUX_DEVPTS_FS_H
#define _LINUX_DEVPTS_FS_H

#include <linux/errno.h>

#ifdef CONFIG_UNIX98_PTYS

<<<<<<< HEAD
int devpts_new_index(struct inode *ptmx_inode);
void devpts_kill_index(struct inode *ptmx_inode, int idx);
void devpts_add_ref(struct inode *ptmx_inode);
void devpts_del_ref(struct inode *ptmx_inode);
/* mknod in devpts */
struct inode *devpts_pty_new(struct inode *ptmx_inode, dev_t device, int index,
		void *priv);
/* get private structure */
void *devpts_get_priv(struct inode *pts_inode);
/* unlink */
void devpts_pty_kill(struct inode *inode);

#else

/* Dummy stubs in the no-pty case */
static inline int devpts_new_index(struct inode *ptmx_inode) { return -EINVAL; }
static inline void devpts_kill_index(struct inode *ptmx_inode, int idx) { }
static inline void devpts_add_ref(struct inode *ptmx_inode) { }
static inline void devpts_del_ref(struct inode *ptmx_inode) { }
static inline struct inode *devpts_pty_new(struct inode *ptmx_inode,
		dev_t device, int index, void *priv)
{
	return ERR_PTR(-EINVAL);
}
static inline void *devpts_get_priv(struct inode *pts_inode)
{
	return NULL;
}
static inline void devpts_pty_kill(struct inode *inode) { }
=======
struct pts_fs_info;

struct pts_fs_info *devpts_acquire(struct file *);
void devpts_release(struct pts_fs_info *);

int devpts_new_index(struct pts_fs_info *);
void devpts_kill_index(struct pts_fs_info *, int);

/* mknod in devpts */
struct dentry *devpts_pty_new(struct pts_fs_info *, int, void *);
/* get private structure */
void *devpts_get_priv(struct dentry *);
/* unlink */
void devpts_pty_kill(struct dentry *);
>>>>>>> v4.9.227

#endif


#endif /* _LINUX_DEVPTS_FS_H */
