/*
 * Copyright (c) 2014 Anna Schumaker <Anna.Schumaker@Netapp.com>
 */

#ifndef __LINUX_FS_NFS_NFS4_2_H
#define __LINUX_FS_NFS_NFS4_2_H

<<<<<<< HEAD
/* nfs4.2proc.c */
loff_t nfs42_proc_llseek(struct file *, loff_t, int);

/* nfs4.2xdr.h */
extern struct rpc_procinfo nfs4_2_procedures[];
=======
/*
 * FIXME:  four LAYOUTSTATS calls per compound at most! Do we need to support
 * more? Need to consider not to pre-alloc too much for a compound.
 */
#define PNFS_LAYOUTSTATS_MAXDEV (4)

/* nfs4.2proc.c */
int nfs42_proc_allocate(struct file *, loff_t, loff_t);
ssize_t nfs42_proc_copy(struct file *, loff_t, struct file *, loff_t, size_t);
int nfs42_proc_deallocate(struct file *, loff_t, loff_t);
loff_t nfs42_proc_llseek(struct file *, loff_t, int);
int nfs42_proc_layoutstats_generic(struct nfs_server *,
				   struct nfs42_layoutstat_data *);
int nfs42_proc_clone(struct file *, struct file *, loff_t, loff_t, loff_t);
>>>>>>> v4.9.227

#endif /* __LINUX_FS_NFS_NFS4_2_H */
