/*
 * Security plug functions
 *
 * Copyright (C) 2001 WireX Communications, Inc <chris@wirex.com>
 * Copyright (C) 2001-2002 Greg Kroah-Hartman <greg@kroah.com>
 * Copyright (C) 2001 Networks Associates Technology, Inc <ssmalley@nai.com>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 */

#include <linux/capability.h>
#include <linux/dcache.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
<<<<<<< HEAD
#include <linux/security.h>
#include <linux/integrity.h>
#include <linux/ima.h>
#include <linux/evm.h>
#include <linux/proca.h>
=======
#include <linux/lsm_hooks.h>
#include <linux/integrity.h>
#include <linux/ima.h>
#include <linux/evm.h>
>>>>>>> v4.9.227
#include <linux/fsnotify.h>
#include <linux/mman.h>
#include <linux/mount.h>
#include <linux/personality.h>
#include <linux/backing-dev.h>
<<<<<<< HEAD
#include <linux/pfk.h>
#include <net/flow.h>
#include <linux/task_integrity.h>

#define MAX_LSM_EVM_XATTR	2

=======
#include <net/flow.h>

#define MAX_LSM_EVM_XATTR	2

/* Maximum number of letters for an LSM name string */
#define SECURITY_NAME_MAX	10

>>>>>>> v4.9.227
/* Boot-time LSM user choice */
static __initdata char chosen_lsm[SECURITY_NAME_MAX + 1] =
	CONFIG_DEFAULT_SECURITY;

<<<<<<< HEAD
static struct security_operations *security_ops;
static struct security_operations default_security_ops = {
	.name	= "default",
};

static inline int __init verify(struct security_operations *ops)
{
	/* verify the security_operations structure exists */
	if (!ops)
		return -EINVAL;
	security_fixup_ops(ops);
	return 0;
}

=======
>>>>>>> v4.9.227
static void __init do_security_initcalls(void)
{
	initcall_t *call;
	call = __security_initcall_start;
	while (call < __security_initcall_end) {
		(*call) ();
		call++;
	}
}

/**
 * security_init - initializes the security framework
 *
 * This should be called early in the kernel initialization sequence.
 */
int __init security_init(void)
{
<<<<<<< HEAD
	printk(KERN_INFO "Security Framework initialized\n");

	security_fixup_ops(&default_security_ops);
	security_ops = &default_security_ops;
=======
	pr_info("Security Framework initialized\n");

	/*
	 * Load minor LSMs, with the capability module always first.
	 */
	capability_add_hooks();
	yama_add_hooks();
	loadpin_add_hooks();

	/*
	 * Load all the remaining security modules.
	 */
>>>>>>> v4.9.227
	do_security_initcalls();

	return 0;
}

<<<<<<< HEAD
void reset_security_ops(void)
{
	security_ops = &default_security_ops;
}

=======
>>>>>>> v4.9.227
/* Save user chosen LSM */
static int __init choose_lsm(char *str)
{
	strncpy(chosen_lsm, str, SECURITY_NAME_MAX);
	return 1;
}
__setup("security=", choose_lsm);

/**
 * security_module_enable - Load given security module on boot ?
<<<<<<< HEAD
 * @ops: a pointer to the struct security_operations that is to be checked.
=======
 * @module: the name of the module
>>>>>>> v4.9.227
 *
 * Each LSM must pass this method before registering its own operations
 * to avoid security registration races. This method may also be used
 * to check if your LSM is currently loaded during kernel initialization.
 *
 * Return true if:
 *	-The passed LSM is the one chosen by user at boot time,
 *	-or the passed LSM is configured as the default and the user did not
 *	 choose an alternate LSM at boot time.
 * Otherwise, return false.
 */
<<<<<<< HEAD
int __init security_module_enable(struct security_operations *ops)
{
	return !strcmp(ops->name, chosen_lsm);
}

/**
 * register_security - registers a security framework with the kernel
 * @ops: a pointer to the struct security_options that is to be registered
 *
 * This function allows a security module to register itself with the
 * kernel security subsystem.  Some rudimentary checking is done on the @ops
 * value passed to this function. You'll need to check first if your LSM
 * is allowed to register its @ops by calling security_module_enable(@ops).
 *
 * If there is already a security module registered with the kernel,
 * an error will be returned.  Otherwise %0 is returned on success.
 */
int __init register_security(struct security_operations *ops)
{
	if (verify(ops)) {
		printk(KERN_DEBUG "%s could not verify "
		       "security_operations structure.\n", __func__);
		return -EINVAL;
	}

	if (security_ops != &default_security_ops)
		return -EAGAIN;

	security_ops = ops;

	return 0;
}
=======
int __init security_module_enable(const char *module)
{
	return !strcmp(module, chosen_lsm);
}

/*
 * Hook list operation macros.
 *
 * call_void_hook:
 *	This is a hook that does not return a value.
 *
 * call_int_hook:
 *	This is a hook that returns a value.
 */

#define call_void_hook(FUNC, ...)				\
	do {							\
		struct security_hook_list *P;			\
								\
		list_for_each_entry(P, &security_hook_heads.FUNC, list)	\
			P->hook.FUNC(__VA_ARGS__);		\
	} while (0)

#define call_int_hook(FUNC, IRC, ...) ({			\
	int RC = IRC;						\
	do {							\
		struct security_hook_list *P;			\
								\
		list_for_each_entry(P, &security_hook_heads.FUNC, list) { \
			RC = P->hook.FUNC(__VA_ARGS__);		\
			if (RC != 0)				\
				break;				\
		}						\
	} while (0);						\
	RC;							\
})
>>>>>>> v4.9.227

/* Security operations */

int security_binder_set_context_mgr(struct task_struct *mgr)
{
<<<<<<< HEAD
	return security_ops->binder_set_context_mgr(mgr);
}

int security_binder_transaction(struct task_struct *from, struct task_struct *to)
{
	return security_ops->binder_transaction(from, to);
}

int security_binder_transfer_binder(struct task_struct *from, struct task_struct *to)
{
	return security_ops->binder_transfer_binder(from, to);
}

int security_binder_transfer_file(struct task_struct *from, struct task_struct *to, struct file *file)
{
	return security_ops->binder_transfer_file(from, to, file);
=======
	return call_int_hook(binder_set_context_mgr, 0, mgr);
}

int security_binder_transaction(struct task_struct *from,
				struct task_struct *to)
{
	return call_int_hook(binder_transaction, 0, from, to);
}

int security_binder_transfer_binder(struct task_struct *from,
				    struct task_struct *to)
{
	return call_int_hook(binder_transfer_binder, 0, from, to);
}

int security_binder_transfer_file(struct task_struct *from,
				  struct task_struct *to, struct file *file)
{
	return call_int_hook(binder_transfer_file, 0, from, to, file);
>>>>>>> v4.9.227
}

int security_ptrace_access_check(struct task_struct *child, unsigned int mode)
{
<<<<<<< HEAD
#ifdef CONFIG_SECURITY_YAMA_STACKED
	int rc;
	rc = yama_ptrace_access_check(child, mode);
	if (rc)
		return rc;
#endif
	return security_ops->ptrace_access_check(child, mode);
=======
	return call_int_hook(ptrace_access_check, 0, child, mode);
>>>>>>> v4.9.227
}

int security_ptrace_traceme(struct task_struct *parent)
{
<<<<<<< HEAD
#ifdef CONFIG_SECURITY_YAMA_STACKED
	int rc;
	rc = yama_ptrace_traceme(parent);
	if (rc)
		return rc;
#endif
	return security_ops->ptrace_traceme(parent);
=======
	return call_int_hook(ptrace_traceme, 0, parent);
>>>>>>> v4.9.227
}

int security_capget(struct task_struct *target,
		     kernel_cap_t *effective,
		     kernel_cap_t *inheritable,
		     kernel_cap_t *permitted)
{
<<<<<<< HEAD
	return security_ops->capget(target, effective, inheritable, permitted);
=======
	return call_int_hook(capget, 0, target,
				effective, inheritable, permitted);
>>>>>>> v4.9.227
}

int security_capset(struct cred *new, const struct cred *old,
		    const kernel_cap_t *effective,
		    const kernel_cap_t *inheritable,
		    const kernel_cap_t *permitted)
{
<<<<<<< HEAD
	return security_ops->capset(new, old,
				    effective, inheritable, permitted);
=======
	return call_int_hook(capset, 0, new, old,
				effective, inheritable, permitted);
>>>>>>> v4.9.227
}

int security_capable(const struct cred *cred, struct user_namespace *ns,
		     int cap)
{
<<<<<<< HEAD
	return security_ops->capable(cred, ns, cap, SECURITY_CAP_AUDIT);
=======
	return call_int_hook(capable, 0, cred, ns, cap, SECURITY_CAP_AUDIT);
>>>>>>> v4.9.227
}

int security_capable_noaudit(const struct cred *cred, struct user_namespace *ns,
			     int cap)
{
<<<<<<< HEAD
	return security_ops->capable(cred, ns, cap, SECURITY_CAP_NOAUDIT);
=======
	return call_int_hook(capable, 0, cred, ns, cap, SECURITY_CAP_NOAUDIT);
>>>>>>> v4.9.227
}

int security_quotactl(int cmds, int type, int id, struct super_block *sb)
{
<<<<<<< HEAD
	return security_ops->quotactl(cmds, type, id, sb);
=======
	return call_int_hook(quotactl, 0, cmds, type, id, sb);
>>>>>>> v4.9.227
}

int security_quota_on(struct dentry *dentry)
{
<<<<<<< HEAD
	return security_ops->quota_on(dentry);
=======
	return call_int_hook(quota_on, 0, dentry);
>>>>>>> v4.9.227
}

int security_syslog(int type)
{
<<<<<<< HEAD
	return security_ops->syslog(type);
}

int security_settime(const struct timespec *ts, const struct timezone *tz)
{
	return security_ops->settime(ts, tz);
=======
	return call_int_hook(syslog, 0, type);
}

int security_settime64(const struct timespec64 *ts, const struct timezone *tz)
{
	return call_int_hook(settime, 0, ts, tz);
>>>>>>> v4.9.227
}

int security_vm_enough_memory_mm(struct mm_struct *mm, long pages)
{
<<<<<<< HEAD
	return security_ops->vm_enough_memory(mm, pages);
=======
	struct security_hook_list *hp;
	int cap_sys_admin = 1;
	int rc;

	/*
	 * The module will respond with a positive value if
	 * it thinks the __vm_enough_memory() call should be
	 * made with the cap_sys_admin set. If all of the modules
	 * agree that it should be set it will. If any module
	 * thinks it should not be set it won't.
	 */
	list_for_each_entry(hp, &security_hook_heads.vm_enough_memory, list) {
		rc = hp->hook.vm_enough_memory(mm, pages);
		if (rc <= 0) {
			cap_sys_admin = 0;
			break;
		}
	}
	return __vm_enough_memory(mm, pages, cap_sys_admin);
>>>>>>> v4.9.227
}

int security_bprm_set_creds(struct linux_binprm *bprm)
{
<<<<<<< HEAD
	return security_ops->bprm_set_creds(bprm);
=======
	return call_int_hook(bprm_set_creds, 0, bprm);
>>>>>>> v4.9.227
}

int security_bprm_check(struct linux_binprm *bprm)
{
	int ret;

<<<<<<< HEAD
	ret = security_ops->bprm_check_security(bprm);
	if (ret)
		return ret;
	ret = five_bprm_check(bprm);
=======
	ret = call_int_hook(bprm_check_security, 0, bprm);
>>>>>>> v4.9.227
	if (ret)
		return ret;
	return ima_bprm_check(bprm);
}

void security_bprm_committing_creds(struct linux_binprm *bprm)
{
<<<<<<< HEAD
	security_ops->bprm_committing_creds(bprm);
=======
	call_void_hook(bprm_committing_creds, bprm);
>>>>>>> v4.9.227
}

void security_bprm_committed_creds(struct linux_binprm *bprm)
{
<<<<<<< HEAD
	security_ops->bprm_committed_creds(bprm);
=======
	call_void_hook(bprm_committed_creds, bprm);
>>>>>>> v4.9.227
}

int security_bprm_secureexec(struct linux_binprm *bprm)
{
<<<<<<< HEAD
	return security_ops->bprm_secureexec(bprm);
=======
	return call_int_hook(bprm_secureexec, 0, bprm);
>>>>>>> v4.9.227
}

int security_sb_alloc(struct super_block *sb)
{
<<<<<<< HEAD
	return security_ops->sb_alloc_security(sb);
=======
	return call_int_hook(sb_alloc_security, 0, sb);
>>>>>>> v4.9.227
}

void security_sb_free(struct super_block *sb)
{
<<<<<<< HEAD
	security_ops->sb_free_security(sb);
=======
	call_void_hook(sb_free_security, sb);
>>>>>>> v4.9.227
}

int security_sb_copy_data(char *orig, char *copy)
{
<<<<<<< HEAD
	return security_ops->sb_copy_data(orig, copy);
=======
	return call_int_hook(sb_copy_data, 0, orig, copy);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_sb_copy_data);

int security_sb_remount(struct super_block *sb, void *data)
{
<<<<<<< HEAD
	return security_ops->sb_remount(sb, data);
=======
	return call_int_hook(sb_remount, 0, sb, data);
>>>>>>> v4.9.227
}

int security_sb_kern_mount(struct super_block *sb, int flags, void *data)
{
<<<<<<< HEAD
	return security_ops->sb_kern_mount(sb, flags, data);
=======
	return call_int_hook(sb_kern_mount, 0, sb, flags, data);
>>>>>>> v4.9.227
}

int security_sb_show_options(struct seq_file *m, struct super_block *sb)
{
<<<<<<< HEAD
	return security_ops->sb_show_options(m, sb);
=======
	return call_int_hook(sb_show_options, 0, m, sb);
>>>>>>> v4.9.227
}

int security_sb_statfs(struct dentry *dentry)
{
<<<<<<< HEAD
	return security_ops->sb_statfs(dentry);
}

int security_sb_mount(const char *dev_name, struct path *path,
                       const char *type, unsigned long flags, void *data)
{
	return security_ops->sb_mount(dev_name, path, type, flags, data);
=======
	return call_int_hook(sb_statfs, 0, dentry);
}

int security_sb_mount(const char *dev_name, const struct path *path,
                       const char *type, unsigned long flags, void *data)
{
	return call_int_hook(sb_mount, 0, dev_name, path, type, flags, data);
>>>>>>> v4.9.227
}

int security_sb_umount(struct vfsmount *mnt, int flags)
{
<<<<<<< HEAD
	return security_ops->sb_umount(mnt, flags);
}

int security_sb_pivotroot(struct path *old_path, struct path *new_path)
{
	return security_ops->sb_pivotroot(old_path, new_path);
=======
	return call_int_hook(sb_umount, 0, mnt, flags);
}

int security_sb_pivotroot(const struct path *old_path, const struct path *new_path)
{
	return call_int_hook(sb_pivotroot, 0, old_path, new_path);
>>>>>>> v4.9.227
}

int security_sb_set_mnt_opts(struct super_block *sb,
				struct security_mnt_opts *opts,
				unsigned long kern_flags,
				unsigned long *set_kern_flags)
{
<<<<<<< HEAD
	return security_ops->sb_set_mnt_opts(sb, opts, kern_flags,
						set_kern_flags);
=======
	return call_int_hook(sb_set_mnt_opts,
				opts->num_mnt_opts ? -EOPNOTSUPP : 0, sb,
				opts, kern_flags, set_kern_flags);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_sb_set_mnt_opts);

int security_sb_clone_mnt_opts(const struct super_block *oldsb,
				struct super_block *newsb)
{
<<<<<<< HEAD
	return security_ops->sb_clone_mnt_opts(oldsb, newsb);
=======
	return call_int_hook(sb_clone_mnt_opts, 0, oldsb, newsb);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_sb_clone_mnt_opts);

int security_sb_parse_opts_str(char *options, struct security_mnt_opts *opts)
{
<<<<<<< HEAD
	return security_ops->sb_parse_opts_str(options, opts);
=======
	return call_int_hook(sb_parse_opts_str, 0, options, opts);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_sb_parse_opts_str);

int security_inode_alloc(struct inode *inode)
{
	inode->i_security = NULL;
<<<<<<< HEAD
	return security_ops->inode_alloc_security(inode);
=======
	return call_int_hook(inode_alloc_security, 0, inode);
>>>>>>> v4.9.227
}

void security_inode_free(struct inode *inode)
{
	integrity_inode_free(inode);
<<<<<<< HEAD
	security_ops->inode_free_security(inode);
}

int security_dentry_init_security(struct dentry *dentry, int mode,
					struct qstr *name, void **ctx,
					u32 *ctxlen)
{
	return security_ops->dentry_init_security(dentry, mode, name,
							ctx, ctxlen);
}
EXPORT_SYMBOL(security_dentry_init_security);

=======
	call_void_hook(inode_free_security, inode);
}

int security_dentry_init_security(struct dentry *dentry, int mode,
					const struct qstr *name, void **ctx,
					u32 *ctxlen)
{
	return call_int_hook(dentry_init_security, -EOPNOTSUPP, dentry, mode,
				name, ctx, ctxlen);
}
EXPORT_SYMBOL(security_dentry_init_security);

int security_dentry_create_files_as(struct dentry *dentry, int mode,
				    struct qstr *name,
				    const struct cred *old, struct cred *new)
{
	return call_int_hook(dentry_create_files_as, 0, dentry, mode,
				name, old, new);
}
EXPORT_SYMBOL(security_dentry_create_files_as);

>>>>>>> v4.9.227
int security_inode_init_security(struct inode *inode, struct inode *dir,
				 const struct qstr *qstr,
				 const initxattrs initxattrs, void *fs_data)
{
	struct xattr new_xattrs[MAX_LSM_EVM_XATTR + 1];
	struct xattr *lsm_xattr, *evm_xattr, *xattr;
	int ret;

	if (unlikely(IS_PRIVATE(inode)))
		return 0;

	if (!initxattrs)
<<<<<<< HEAD
		return security_ops->inode_init_security(inode, dir, qstr,
							 NULL, NULL, NULL);
	memset(new_xattrs, 0, sizeof(new_xattrs));
	lsm_xattr = new_xattrs;
	ret = security_ops->inode_init_security(inode, dir, qstr,
=======
		return call_int_hook(inode_init_security, -EOPNOTSUPP, inode,
				     dir, qstr, NULL, NULL, NULL);
	memset(new_xattrs, 0, sizeof(new_xattrs));
	lsm_xattr = new_xattrs;
	ret = call_int_hook(inode_init_security, -EOPNOTSUPP, inode, dir, qstr,
>>>>>>> v4.9.227
						&lsm_xattr->name,
						&lsm_xattr->value,
						&lsm_xattr->value_len);
	if (ret)
		goto out;

	evm_xattr = lsm_xattr + 1;
	ret = evm_inode_init_security(inode, lsm_xattr, evm_xattr);
	if (ret)
		goto out;
	ret = initxattrs(inode, new_xattrs, fs_data);
out:
	for (xattr = new_xattrs; xattr->value != NULL; xattr++)
		kfree(xattr->value);
	return (ret == -EOPNOTSUPP) ? 0 : ret;
}
EXPORT_SYMBOL(security_inode_init_security);

int security_old_inode_init_security(struct inode *inode, struct inode *dir,
				     const struct qstr *qstr, const char **name,
				     void **value, size_t *len)
{
	if (unlikely(IS_PRIVATE(inode)))
		return -EOPNOTSUPP;
<<<<<<< HEAD
	return security_ops->inode_init_security(inode, dir, qstr, name, value,
						 len);
=======
	return call_int_hook(inode_init_security, -EOPNOTSUPP, inode, dir,
			     qstr, name, value, len);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_old_inode_init_security);

#ifdef CONFIG_SECURITY_PATH
<<<<<<< HEAD
int security_path_mknod(struct path *dir, struct dentry *dentry, umode_t mode,
			unsigned int dev)
{
	if (unlikely(IS_PRIVATE(dir->dentry->d_inode)))
		return 0;
	return security_ops->path_mknod(dir, dentry, mode, dev);
}
EXPORT_SYMBOL(security_path_mknod);

int security_path_mkdir(struct path *dir, struct dentry *dentry, umode_t mode)
{
	if (unlikely(IS_PRIVATE(dir->dentry->d_inode)))
		return 0;
	return security_ops->path_mkdir(dir, dentry, mode);
}
EXPORT_SYMBOL(security_path_mkdir);

int security_path_rmdir(struct path *dir, struct dentry *dentry)
{
	if (unlikely(IS_PRIVATE(dir->dentry->d_inode)))
		return 0;
	return security_ops->path_rmdir(dir, dentry);
}

int security_path_unlink(struct path *dir, struct dentry *dentry)
{
	if (unlikely(IS_PRIVATE(dir->dentry->d_inode)))
		return 0;
	return security_ops->path_unlink(dir, dentry);
}
EXPORT_SYMBOL(security_path_unlink);

int security_path_symlink(struct path *dir, struct dentry *dentry,
			  const char *old_name)
{
	if (unlikely(IS_PRIVATE(dir->dentry->d_inode)))
		return 0;
	return security_ops->path_symlink(dir, dentry, old_name);
}

int security_path_link(struct dentry *old_dentry, struct path *new_dir,
		       struct dentry *new_dentry)
{
	if (unlikely(IS_PRIVATE(old_dentry->d_inode)))
		return 0;
	return security_ops->path_link(old_dentry, new_dir, new_dentry);
}

int security_path_rename(struct path *old_dir, struct dentry *old_dentry,
			 struct path *new_dir, struct dentry *new_dentry,
			 unsigned int flags)
{
	if (unlikely(IS_PRIVATE(old_dentry->d_inode) ||
		     (new_dentry->d_inode && IS_PRIVATE(new_dentry->d_inode))))
		return 0;

	if (flags & RENAME_EXCHANGE) {
		int err = security_ops->path_rename(new_dir, new_dentry,
						    old_dir, old_dentry);
=======
int security_path_mknod(const struct path *dir, struct dentry *dentry, umode_t mode,
			unsigned int dev)
{
	if (unlikely(IS_PRIVATE(d_backing_inode(dir->dentry))))
		return 0;
	return call_int_hook(path_mknod, 0, dir, dentry, mode, dev);
}
EXPORT_SYMBOL(security_path_mknod);

int security_path_mkdir(const struct path *dir, struct dentry *dentry, umode_t mode)
{
	if (unlikely(IS_PRIVATE(d_backing_inode(dir->dentry))))
		return 0;
	return call_int_hook(path_mkdir, 0, dir, dentry, mode);
}
EXPORT_SYMBOL(security_path_mkdir);

int security_path_rmdir(const struct path *dir, struct dentry *dentry)
{
	if (unlikely(IS_PRIVATE(d_backing_inode(dir->dentry))))
		return 0;
	return call_int_hook(path_rmdir, 0, dir, dentry);
}

int security_path_unlink(const struct path *dir, struct dentry *dentry)
{
	if (unlikely(IS_PRIVATE(d_backing_inode(dir->dentry))))
		return 0;
	return call_int_hook(path_unlink, 0, dir, dentry);
}
EXPORT_SYMBOL(security_path_unlink);

int security_path_symlink(const struct path *dir, struct dentry *dentry,
			  const char *old_name)
{
	if (unlikely(IS_PRIVATE(d_backing_inode(dir->dentry))))
		return 0;
	return call_int_hook(path_symlink, 0, dir, dentry, old_name);
}

int security_path_link(struct dentry *old_dentry, const struct path *new_dir,
		       struct dentry *new_dentry)
{
	if (unlikely(IS_PRIVATE(d_backing_inode(old_dentry))))
		return 0;
	return call_int_hook(path_link, 0, old_dentry, new_dir, new_dentry);
}

int security_path_rename(const struct path *old_dir, struct dentry *old_dentry,
			 const struct path *new_dir, struct dentry *new_dentry,
			 unsigned int flags)
{
	if (unlikely(IS_PRIVATE(d_backing_inode(old_dentry)) ||
		     (d_is_positive(new_dentry) && IS_PRIVATE(d_backing_inode(new_dentry)))))
		return 0;

	if (flags & RENAME_EXCHANGE) {
		int err = call_int_hook(path_rename, 0, new_dir, new_dentry,
					old_dir, old_dentry);
>>>>>>> v4.9.227
		if (err)
			return err;
	}

<<<<<<< HEAD
	return security_ops->path_rename(old_dir, old_dentry, new_dir,
					 new_dentry);
}
EXPORT_SYMBOL(security_path_rename);

int security_path_truncate(struct path *path)
{
	if (unlikely(IS_PRIVATE(path->dentry->d_inode)))
		return 0;
	return security_ops->path_truncate(path);
}

int security_path_chmod(struct path *path, umode_t mode)
{
	if (unlikely(IS_PRIVATE(path->dentry->d_inode)))
		return 0;
	return security_ops->path_chmod(path, mode);
}

int security_path_chown(struct path *path, kuid_t uid, kgid_t gid)
{
	if (unlikely(IS_PRIVATE(path->dentry->d_inode)))
		return 0;
	return security_ops->path_chown(path, uid, gid);
}
EXPORT_SYMBOL(security_path_chown);

int security_path_chroot(struct path *path)
{
	return security_ops->path_chroot(path);
=======
	return call_int_hook(path_rename, 0, old_dir, old_dentry, new_dir,
				new_dentry);
}
EXPORT_SYMBOL(security_path_rename);

int security_path_truncate(const struct path *path)
{
	if (unlikely(IS_PRIVATE(d_backing_inode(path->dentry))))
		return 0;
	return call_int_hook(path_truncate, 0, path);
}

int security_path_chmod(const struct path *path, umode_t mode)
{
	if (unlikely(IS_PRIVATE(d_backing_inode(path->dentry))))
		return 0;
	return call_int_hook(path_chmod, 0, path, mode);
}

int security_path_chown(const struct path *path, kuid_t uid, kgid_t gid)
{
	if (unlikely(IS_PRIVATE(d_backing_inode(path->dentry))))
		return 0;
	return call_int_hook(path_chown, 0, path, uid, gid);
}

int security_path_chroot(const struct path *path)
{
	return call_int_hook(path_chroot, 0, path);
>>>>>>> v4.9.227
}
#endif

int security_inode_create(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	if (unlikely(IS_PRIVATE(dir)))
		return 0;
<<<<<<< HEAD
	return security_ops->inode_create(dir, dentry, mode);
}
EXPORT_SYMBOL_GPL(security_inode_create);

int security_inode_post_create(struct inode *dir, struct dentry *dentry,
			       umode_t mode)
{
	if (unlikely(IS_PRIVATE(dir)))
		return 0;
	if (security_ops->inode_post_create == NULL)
		return 0;
	return security_ops->inode_post_create(dir, dentry, mode);
}

int security_inode_link(struct dentry *old_dentry, struct inode *dir,
			 struct dentry *new_dentry)
{
	if (unlikely(IS_PRIVATE(old_dentry->d_inode)))
		return 0;
	return security_ops->inode_link(old_dentry, dir, new_dentry);
=======
	return call_int_hook(inode_create, 0, dir, dentry, mode);
}
EXPORT_SYMBOL_GPL(security_inode_create);

int security_inode_link(struct dentry *old_dentry, struct inode *dir,
			 struct dentry *new_dentry)
{
	if (unlikely(IS_PRIVATE(d_backing_inode(old_dentry))))
		return 0;
	return call_int_hook(inode_link, 0, old_dentry, dir, new_dentry);
>>>>>>> v4.9.227
}

int security_inode_unlink(struct inode *dir, struct dentry *dentry)
{
<<<<<<< HEAD
	if (unlikely(IS_PRIVATE(dentry->d_inode)))
		return 0;
	return security_ops->inode_unlink(dir, dentry);
=======
	if (unlikely(IS_PRIVATE(d_backing_inode(dentry))))
		return 0;
	return call_int_hook(inode_unlink, 0, dir, dentry);
>>>>>>> v4.9.227
}

int security_inode_symlink(struct inode *dir, struct dentry *dentry,
			    const char *old_name)
{
	if (unlikely(IS_PRIVATE(dir)))
		return 0;
<<<<<<< HEAD
	return security_ops->inode_symlink(dir, dentry, old_name);
=======
	return call_int_hook(inode_symlink, 0, dir, dentry, old_name);
>>>>>>> v4.9.227
}

int security_inode_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	if (unlikely(IS_PRIVATE(dir)))
		return 0;
<<<<<<< HEAD
	return security_ops->inode_mkdir(dir, dentry, mode);
=======
	return call_int_hook(inode_mkdir, 0, dir, dentry, mode);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(security_inode_mkdir);

int security_inode_rmdir(struct inode *dir, struct dentry *dentry)
{
<<<<<<< HEAD
	if (unlikely(IS_PRIVATE(dentry->d_inode)))
		return 0;
	return security_ops->inode_rmdir(dir, dentry);
=======
	if (unlikely(IS_PRIVATE(d_backing_inode(dentry))))
		return 0;
	return call_int_hook(inode_rmdir, 0, dir, dentry);
>>>>>>> v4.9.227
}

int security_inode_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev)
{
	if (unlikely(IS_PRIVATE(dir)))
		return 0;
<<<<<<< HEAD
	return security_ops->inode_mknod(dir, dentry, mode, dev);
=======
	return call_int_hook(inode_mknod, 0, dir, dentry, mode, dev);
>>>>>>> v4.9.227
}

int security_inode_rename(struct inode *old_dir, struct dentry *old_dentry,
			   struct inode *new_dir, struct dentry *new_dentry,
			   unsigned int flags)
{
<<<<<<< HEAD
        if (unlikely(IS_PRIVATE(old_dentry->d_inode) ||
            (new_dentry->d_inode && IS_PRIVATE(new_dentry->d_inode))))
		return 0;

	if (flags & RENAME_EXCHANGE) {
		int err = security_ops->inode_rename(new_dir, new_dentry,
=======
        if (unlikely(IS_PRIVATE(d_backing_inode(old_dentry)) ||
            (d_is_positive(new_dentry) && IS_PRIVATE(d_backing_inode(new_dentry)))))
		return 0;

	if (flags & RENAME_EXCHANGE) {
		int err = call_int_hook(inode_rename, 0, new_dir, new_dentry,
>>>>>>> v4.9.227
						     old_dir, old_dentry);
		if (err)
			return err;
	}

<<<<<<< HEAD
	return security_ops->inode_rename(old_dir, old_dentry,
=======
	return call_int_hook(inode_rename, 0, old_dir, old_dentry,
>>>>>>> v4.9.227
					   new_dir, new_dentry);
}

int security_inode_readlink(struct dentry *dentry)
{
<<<<<<< HEAD
	if (unlikely(IS_PRIVATE(dentry->d_inode)))
		return 0;
	return security_ops->inode_readlink(dentry);
}

int security_inode_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	if (unlikely(IS_PRIVATE(dentry->d_inode)))
		return 0;
	return security_ops->inode_follow_link(dentry, nd);
=======
	if (unlikely(IS_PRIVATE(d_backing_inode(dentry))))
		return 0;
	return call_int_hook(inode_readlink, 0, dentry);
}

int security_inode_follow_link(struct dentry *dentry, struct inode *inode,
			       bool rcu)
{
	if (unlikely(IS_PRIVATE(inode)))
		return 0;
	return call_int_hook(inode_follow_link, 0, dentry, inode, rcu);
>>>>>>> v4.9.227
}

int security_inode_permission(struct inode *inode, int mask)
{
	if (unlikely(IS_PRIVATE(inode)))
		return 0;
<<<<<<< HEAD
	return security_ops->inode_permission(inode, mask);
=======
	return call_int_hook(inode_permission, 0, inode, mask);
>>>>>>> v4.9.227
}

int security_inode_setattr(struct dentry *dentry, struct iattr *attr)
{
	int ret;

<<<<<<< HEAD
	if (unlikely(IS_PRIVATE(dentry->d_inode)))
		return 0;
	ret = security_ops->inode_setattr(dentry, attr);
=======
	if (unlikely(IS_PRIVATE(d_backing_inode(dentry))))
		return 0;
	ret = call_int_hook(inode_setattr, 0, dentry, attr);
>>>>>>> v4.9.227
	if (ret)
		return ret;
	return evm_inode_setattr(dentry, attr);
}
EXPORT_SYMBOL_GPL(security_inode_setattr);

<<<<<<< HEAD
int security_inode_getattr(struct vfsmount *mnt, struct dentry *dentry)
{
	if (unlikely(IS_PRIVATE(dentry->d_inode)))
		return 0;
	return security_ops->inode_getattr(mnt, dentry);
=======
int security_inode_getattr(const struct path *path)
{
	if (unlikely(IS_PRIVATE(d_backing_inode(path->dentry))))
		return 0;
	return call_int_hook(inode_getattr, 0, path);
>>>>>>> v4.9.227
}

int security_inode_setxattr(struct dentry *dentry, const char *name,
			    const void *value, size_t size, int flags)
{
	int ret;

<<<<<<< HEAD
	if (unlikely(IS_PRIVATE(dentry->d_inode)))
		return 0;
	ret = security_ops->inode_setxattr(dentry, name, value, size, flags);
	if (ret)
		return ret;
	ret = five_inode_setxattr(dentry, name, value, size);
=======
	if (unlikely(IS_PRIVATE(d_backing_inode(dentry))))
		return 0;
	/*
	 * SELinux and Smack integrate the cap call,
	 * so assume that all LSMs supplying this call do so.
	 */
	ret = call_int_hook(inode_setxattr, 1, dentry, name, value, size,
				flags);

	if (ret == 1)
		ret = cap_inode_setxattr(dentry, name, value, size, flags);
>>>>>>> v4.9.227
	if (ret)
		return ret;
	ret = ima_inode_setxattr(dentry, name, value, size);
	if (ret)
		return ret;
	return evm_inode_setxattr(dentry, name, value, size);
}

void security_inode_post_setxattr(struct dentry *dentry, const char *name,
				  const void *value, size_t size, int flags)
{
<<<<<<< HEAD
	if (unlikely(IS_PRIVATE(dentry->d_inode)))
		return;
	security_ops->inode_post_setxattr(dentry, name, value, size, flags);
=======
	if (unlikely(IS_PRIVATE(d_backing_inode(dentry))))
		return;
	call_void_hook(inode_post_setxattr, dentry, name, value, size, flags);
>>>>>>> v4.9.227
	evm_inode_post_setxattr(dentry, name, value, size);
}

int security_inode_getxattr(struct dentry *dentry, const char *name)
{
<<<<<<< HEAD
	if (unlikely(IS_PRIVATE(dentry->d_inode)))
		return 0;
	return security_ops->inode_getxattr(dentry, name);
=======
	if (unlikely(IS_PRIVATE(d_backing_inode(dentry))))
		return 0;
	return call_int_hook(inode_getxattr, 0, dentry, name);
>>>>>>> v4.9.227
}

int security_inode_listxattr(struct dentry *dentry)
{
<<<<<<< HEAD
	if (unlikely(IS_PRIVATE(dentry->d_inode)))
		return 0;
	return security_ops->inode_listxattr(dentry);
=======
	if (unlikely(IS_PRIVATE(d_backing_inode(dentry))))
		return 0;
	return call_int_hook(inode_listxattr, 0, dentry);
>>>>>>> v4.9.227
}

int security_inode_removexattr(struct dentry *dentry, const char *name)
{
	int ret;

<<<<<<< HEAD
	if (unlikely(IS_PRIVATE(dentry->d_inode)))
		return 0;
	ret = security_ops->inode_removexattr(dentry, name);
	if (ret)
		return ret;
	ret = five_inode_removexattr(dentry, name);
=======
	if (unlikely(IS_PRIVATE(d_backing_inode(dentry))))
		return 0;
	/*
	 * SELinux and Smack integrate the cap call,
	 * so assume that all LSMs supplying this call do so.
	 */
	ret = call_int_hook(inode_removexattr, 1, dentry, name);
	if (ret == 1)
		ret = cap_inode_removexattr(dentry, name);
>>>>>>> v4.9.227
	if (ret)
		return ret;
	ret = ima_inode_removexattr(dentry, name);
	if (ret)
		return ret;
	return evm_inode_removexattr(dentry, name);
}

int security_inode_need_killpriv(struct dentry *dentry)
{
<<<<<<< HEAD
	return security_ops->inode_need_killpriv(dentry);
=======
	return call_int_hook(inode_need_killpriv, 0, dentry);
>>>>>>> v4.9.227
}

int security_inode_killpriv(struct dentry *dentry)
{
<<<<<<< HEAD
	return security_ops->inode_killpriv(dentry);
}

int security_inode_getsecurity(const struct inode *inode, const char *name, void **buffer, bool alloc)
{
	if (unlikely(IS_PRIVATE(inode)))
		return -EOPNOTSUPP;
	return security_ops->inode_getsecurity(inode, name, buffer, alloc);
=======
	return call_int_hook(inode_killpriv, 0, dentry);
}

int security_inode_getsecurity(struct inode *inode, const char *name, void **buffer, bool alloc)
{
	struct security_hook_list *hp;
	int rc;

	if (unlikely(IS_PRIVATE(inode)))
		return -EOPNOTSUPP;
	/*
	 * Only one module will provide an attribute with a given name.
	 */
	list_for_each_entry(hp, &security_hook_heads.inode_getsecurity, list) {
		rc = hp->hook.inode_getsecurity(inode, name, buffer, alloc);
		if (rc != -EOPNOTSUPP)
			return rc;
	}
	return -EOPNOTSUPP;
>>>>>>> v4.9.227
}

int security_inode_setsecurity(struct inode *inode, const char *name, const void *value, size_t size, int flags)
{
<<<<<<< HEAD
	if (unlikely(IS_PRIVATE(inode)))
		return -EOPNOTSUPP;
	return security_ops->inode_setsecurity(inode, name, value, size, flags);
=======
	struct security_hook_list *hp;
	int rc;

	if (unlikely(IS_PRIVATE(inode)))
		return -EOPNOTSUPP;
	/*
	 * Only one module will provide an attribute with a given name.
	 */
	list_for_each_entry(hp, &security_hook_heads.inode_setsecurity, list) {
		rc = hp->hook.inode_setsecurity(inode, name, value, size,
								flags);
		if (rc != -EOPNOTSUPP)
			return rc;
	}
	return -EOPNOTSUPP;
>>>>>>> v4.9.227
}

int security_inode_listsecurity(struct inode *inode, char *buffer, size_t buffer_size)
{
	if (unlikely(IS_PRIVATE(inode)))
		return 0;
<<<<<<< HEAD
	return security_ops->inode_listsecurity(inode, buffer, buffer_size);
}
EXPORT_SYMBOL(security_inode_listsecurity);

void security_inode_getsecid(const struct inode *inode, u32 *secid)
{
	security_ops->inode_getsecid(inode, secid);
}

=======
	return call_int_hook(inode_listsecurity, 0, inode, buffer, buffer_size);
}
EXPORT_SYMBOL(security_inode_listsecurity);

void security_inode_getsecid(struct inode *inode, u32 *secid)
{
	call_void_hook(inode_getsecid, inode, secid);
}

int security_inode_copy_up(struct dentry *src, struct cred **new)
{
	return call_int_hook(inode_copy_up, 0, src, new);
}
EXPORT_SYMBOL(security_inode_copy_up);

int security_inode_copy_up_xattr(const char *name)
{
	return call_int_hook(inode_copy_up_xattr, -EOPNOTSUPP, name);
}
EXPORT_SYMBOL(security_inode_copy_up_xattr);

>>>>>>> v4.9.227
int security_file_permission(struct file *file, int mask)
{
	int ret;

<<<<<<< HEAD
	ret = security_ops->file_permission(file, mask);
=======
	ret = call_int_hook(file_permission, 0, file, mask);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	return fsnotify_perm(file, mask);
}

int security_file_alloc(struct file *file)
{
<<<<<<< HEAD
	return security_ops->file_alloc_security(file);
=======
	return call_int_hook(file_alloc_security, 0, file);
>>>>>>> v4.9.227
}

void security_file_free(struct file *file)
{
<<<<<<< HEAD
	security_ops->file_free_security(file);
#ifdef CONFIG_PROCA
	proca_compat_file_free_security_hook(file);
#endif
=======
	call_void_hook(file_free_security, file);
>>>>>>> v4.9.227
}

int security_file_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
<<<<<<< HEAD
	return security_ops->file_ioctl(file, cmd, arg);
=======
	return call_int_hook(file_ioctl, 0, file, cmd, arg);
>>>>>>> v4.9.227
}

static inline unsigned long mmap_prot(struct file *file, unsigned long prot)
{
	/*
	 * Does we have PROT_READ and does the application expect
	 * it to imply PROT_EXEC?  If not, nothing to talk about...
	 */
	if ((prot & (PROT_READ | PROT_EXEC)) != PROT_READ)
		return prot;
	if (!(current->personality & READ_IMPLIES_EXEC))
		return prot;
	/*
	 * if that's an anonymous mapping, let it.
	 */
	if (!file)
		return prot | PROT_EXEC;
	/*
	 * ditto if it's not on noexec mount, except that on !MMU we need
<<<<<<< HEAD
	 * BDI_CAP_EXEC_MMAP (== VM_MAYEXEC) in this case
	 */
	if (!path_noexec(&file->f_path)) {
#ifndef CONFIG_MMU
		unsigned long caps = 0;
		struct address_space *mapping = file->f_mapping;
		if (mapping && mapping->backing_dev_info)
			caps = mapping->backing_dev_info->capabilities;
		if (!(caps & BDI_CAP_EXEC_MAP))
			return prot;
=======
	 * NOMMU_MAP_EXEC (== VM_MAYEXEC) in this case
	 */
	if (!path_noexec(&file->f_path)) {
#ifndef CONFIG_MMU
		if (file->f_op->mmap_capabilities) {
			unsigned caps = file->f_op->mmap_capabilities(file);
			if (!(caps & NOMMU_MAP_EXEC))
				return prot;
		}
>>>>>>> v4.9.227
#endif
		return prot | PROT_EXEC;
	}
	/* anything on noexec mount won't get PROT_EXEC */
	return prot;
}

int security_mmap_file(struct file *file, unsigned long prot,
			unsigned long flags)
{
	int ret;
<<<<<<< HEAD
	ret = security_ops->mmap_file(file, prot,
					mmap_prot(file, prot), flags);
	if (ret)
		return ret;
	ret = five_file_mmap(file, prot);
	if (ret)
		return ret;
=======
	ret = call_int_hook(mmap_file, 0, file, prot,
					mmap_prot(file, prot), flags);
	if (ret)
		return ret;
>>>>>>> v4.9.227
	return ima_file_mmap(file, prot);
}

int security_mmap_addr(unsigned long addr)
{
<<<<<<< HEAD
	return security_ops->mmap_addr(addr);
=======
	return call_int_hook(mmap_addr, 0, addr);
>>>>>>> v4.9.227
}

int security_file_mprotect(struct vm_area_struct *vma, unsigned long reqprot,
			    unsigned long prot)
{
<<<<<<< HEAD
	return security_ops->file_mprotect(vma, reqprot, prot);
=======
	return call_int_hook(file_mprotect, 0, vma, reqprot, prot);
>>>>>>> v4.9.227
}

int security_file_lock(struct file *file, unsigned int cmd)
{
<<<<<<< HEAD
	return security_ops->file_lock(file, cmd);
=======
	return call_int_hook(file_lock, 0, file, cmd);
>>>>>>> v4.9.227
}

int security_file_fcntl(struct file *file, unsigned int cmd, unsigned long arg)
{
<<<<<<< HEAD
	return security_ops->file_fcntl(file, cmd, arg);
=======
	return call_int_hook(file_fcntl, 0, file, cmd, arg);
>>>>>>> v4.9.227
}

void security_file_set_fowner(struct file *file)
{
<<<<<<< HEAD
	security_ops->file_set_fowner(file);
=======
	call_void_hook(file_set_fowner, file);
>>>>>>> v4.9.227
}

int security_file_send_sigiotask(struct task_struct *tsk,
				  struct fown_struct *fown, int sig)
{
<<<<<<< HEAD
	return security_ops->file_send_sigiotask(tsk, fown, sig);
=======
	return call_int_hook(file_send_sigiotask, 0, tsk, fown, sig);
>>>>>>> v4.9.227
}

int security_file_receive(struct file *file)
{
<<<<<<< HEAD
	return security_ops->file_receive(file);
=======
	return call_int_hook(file_receive, 0, file);
>>>>>>> v4.9.227
}

int security_file_open(struct file *file, const struct cred *cred)
{
	int ret;

<<<<<<< HEAD
	ret = security_ops->file_open(file, cred);
	if (ret)
		return ret;

	ret = fsnotify_perm(file, MAY_OPEN);
	if (ret)
		return ret;

	return five_file_open(file);
}

bool security_allow_merge_bio(struct bio *bio1, struct bio *bio2)
{
	bool ret = pfk_allow_merge_bio(bio1, bio2);

	if (security_ops->allow_merge_bio)
		ret = ret && security_ops->allow_merge_bio(bio1, bio2);

	return ret;
=======
	ret = call_int_hook(file_open, 0, file, cred);
	if (ret)
		return ret;

	return fsnotify_perm(file, MAY_OPEN);
>>>>>>> v4.9.227
}

int security_task_create(unsigned long clone_flags)
{
<<<<<<< HEAD
	return security_ops->task_create(clone_flags);
=======
	return call_int_hook(task_create, 0, clone_flags);
>>>>>>> v4.9.227
}

void security_task_free(struct task_struct *task)
{
<<<<<<< HEAD
#ifdef CONFIG_SECURITY_YAMA_STACKED
	yama_task_free(task);
#endif
	security_ops->task_free(task);
	five_task_free(task);
#ifdef CONFIG_PROCA
	proca_compat_task_free_hook(task);
#endif
=======
	call_void_hook(task_free, task);
>>>>>>> v4.9.227
}

int security_cred_alloc_blank(struct cred *cred, gfp_t gfp)
{
<<<<<<< HEAD
	return security_ops->cred_alloc_blank(cred, gfp);
=======
	return call_int_hook(cred_alloc_blank, 0, cred, gfp);
>>>>>>> v4.9.227
}

void security_cred_free(struct cred *cred)
{
<<<<<<< HEAD
	security_ops->cred_free(cred);
=======
	/*
	 * There is a failure case in prepare_creds() that
	 * may result in a call here with ->security being NULL.
	 */
	if (unlikely(cred->security == NULL))
		return;

	call_void_hook(cred_free, cred);
>>>>>>> v4.9.227
}

int security_prepare_creds(struct cred *new, const struct cred *old, gfp_t gfp)
{
<<<<<<< HEAD
	return security_ops->cred_prepare(new, old, gfp);
=======
	return call_int_hook(cred_prepare, 0, new, old, gfp);
>>>>>>> v4.9.227
}

void security_transfer_creds(struct cred *new, const struct cred *old)
{
<<<<<<< HEAD
	security_ops->cred_transfer(new, old);
=======
	call_void_hook(cred_transfer, new, old);
>>>>>>> v4.9.227
}

int security_kernel_act_as(struct cred *new, u32 secid)
{
<<<<<<< HEAD
	return security_ops->kernel_act_as(new, secid);
=======
	return call_int_hook(kernel_act_as, 0, new, secid);
>>>>>>> v4.9.227
}

int security_kernel_create_files_as(struct cred *new, struct inode *inode)
{
<<<<<<< HEAD
	return security_ops->kernel_create_files_as(new, inode);
}

int security_kernel_fw_from_file(struct file *file, char *buf, size_t size)
{
	int ret;

	ret = security_ops->kernel_fw_from_file(file, buf, size);
	if (ret)
		return ret;
	return ima_fw_from_file(file, buf, size);
}
EXPORT_SYMBOL_GPL(security_kernel_fw_from_file);

int security_kernel_module_request(char *kmod_name)
{
	return security_ops->kernel_module_request(kmod_name);
}

int security_kernel_module_from_file(struct file *file)
{
	int ret;

	ret = security_ops->kernel_module_from_file(file);
	if (ret)
		return ret;
	return ima_module_check(file);
}
=======
	return call_int_hook(kernel_create_files_as, 0, new, inode);
}

int security_kernel_module_request(char *kmod_name)
{
	return call_int_hook(kernel_module_request, 0, kmod_name);
}

int security_kernel_read_file(struct file *file, enum kernel_read_file_id id)
{
	int ret;

	ret = call_int_hook(kernel_read_file, 0, file, id);
	if (ret)
		return ret;
	return ima_read_file(file, id);
}
EXPORT_SYMBOL_GPL(security_kernel_read_file);

int security_kernel_post_read_file(struct file *file, char *buf, loff_t size,
				   enum kernel_read_file_id id)
{
	int ret;

	ret = call_int_hook(kernel_post_read_file, 0, file, buf, size, id);
	if (ret)
		return ret;
	return ima_post_read_file(file, buf, size, id);
}
EXPORT_SYMBOL_GPL(security_kernel_post_read_file);
>>>>>>> v4.9.227

int security_task_fix_setuid(struct cred *new, const struct cred *old,
			     int flags)
{
<<<<<<< HEAD
	return security_ops->task_fix_setuid(new, old, flags);
=======
	return call_int_hook(task_fix_setuid, 0, new, old, flags);
>>>>>>> v4.9.227
}

int security_task_setpgid(struct task_struct *p, pid_t pgid)
{
<<<<<<< HEAD
	return security_ops->task_setpgid(p, pgid);
=======
	return call_int_hook(task_setpgid, 0, p, pgid);
>>>>>>> v4.9.227
}

int security_task_getpgid(struct task_struct *p)
{
<<<<<<< HEAD
	return security_ops->task_getpgid(p);
=======
	return call_int_hook(task_getpgid, 0, p);
>>>>>>> v4.9.227
}

int security_task_getsid(struct task_struct *p)
{
<<<<<<< HEAD
	return security_ops->task_getsid(p);
=======
	return call_int_hook(task_getsid, 0, p);
>>>>>>> v4.9.227
}

void security_task_getsecid(struct task_struct *p, u32 *secid)
{
<<<<<<< HEAD
	security_ops->task_getsecid(p, secid);
=======
	*secid = 0;
	call_void_hook(task_getsecid, p, secid);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_task_getsecid);

int security_task_setnice(struct task_struct *p, int nice)
{
<<<<<<< HEAD
	return security_ops->task_setnice(p, nice);
=======
	return call_int_hook(task_setnice, 0, p, nice);
>>>>>>> v4.9.227
}

int security_task_setioprio(struct task_struct *p, int ioprio)
{
<<<<<<< HEAD
	return security_ops->task_setioprio(p, ioprio);
=======
	return call_int_hook(task_setioprio, 0, p, ioprio);
>>>>>>> v4.9.227
}

int security_task_getioprio(struct task_struct *p)
{
<<<<<<< HEAD
	return security_ops->task_getioprio(p);
=======
	return call_int_hook(task_getioprio, 0, p);
>>>>>>> v4.9.227
}

int security_task_setrlimit(struct task_struct *p, unsigned int resource,
		struct rlimit *new_rlim)
{
<<<<<<< HEAD
	return security_ops->task_setrlimit(p, resource, new_rlim);
=======
	return call_int_hook(task_setrlimit, 0, p, resource, new_rlim);
>>>>>>> v4.9.227
}

int security_task_setscheduler(struct task_struct *p)
{
<<<<<<< HEAD
	return security_ops->task_setscheduler(p);
=======
	return call_int_hook(task_setscheduler, 0, p);
>>>>>>> v4.9.227
}

int security_task_getscheduler(struct task_struct *p)
{
<<<<<<< HEAD
	return security_ops->task_getscheduler(p);
=======
	return call_int_hook(task_getscheduler, 0, p);
>>>>>>> v4.9.227
}

int security_task_movememory(struct task_struct *p)
{
<<<<<<< HEAD
	return security_ops->task_movememory(p);
=======
	return call_int_hook(task_movememory, 0, p);
>>>>>>> v4.9.227
}

int security_task_kill(struct task_struct *p, struct siginfo *info,
			int sig, u32 secid)
{
<<<<<<< HEAD
	return security_ops->task_kill(p, info, sig, secid);
=======
	return call_int_hook(task_kill, 0, p, info, sig, secid);
>>>>>>> v4.9.227
}

int security_task_wait(struct task_struct *p)
{
<<<<<<< HEAD
	return security_ops->task_wait(p);
=======
	return call_int_hook(task_wait, 0, p);
>>>>>>> v4.9.227
}

int security_task_prctl(int option, unsigned long arg2, unsigned long arg3,
			 unsigned long arg4, unsigned long arg5)
{
<<<<<<< HEAD
#ifdef CONFIG_SECURITY_YAMA_STACKED
	int rc;
	rc = yama_task_prctl(option, arg2, arg3, arg4, arg5);
	if (rc != -ENOSYS)
		return rc;
#endif
	return security_ops->task_prctl(option, arg2, arg3, arg4, arg5);
=======
	int thisrc;
	int rc = -ENOSYS;
	struct security_hook_list *hp;

	list_for_each_entry(hp, &security_hook_heads.task_prctl, list) {
		thisrc = hp->hook.task_prctl(option, arg2, arg3, arg4, arg5);
		if (thisrc != -ENOSYS) {
			rc = thisrc;
			if (thisrc != 0)
				break;
		}
	}
	return rc;
>>>>>>> v4.9.227
}

void security_task_to_inode(struct task_struct *p, struct inode *inode)
{
<<<<<<< HEAD
	security_ops->task_to_inode(p, inode);
=======
	call_void_hook(task_to_inode, p, inode);
>>>>>>> v4.9.227
}

int security_ipc_permission(struct kern_ipc_perm *ipcp, short flag)
{
<<<<<<< HEAD
	return security_ops->ipc_permission(ipcp, flag);
=======
	return call_int_hook(ipc_permission, 0, ipcp, flag);
>>>>>>> v4.9.227
}

void security_ipc_getsecid(struct kern_ipc_perm *ipcp, u32 *secid)
{
<<<<<<< HEAD
	security_ops->ipc_getsecid(ipcp, secid);
=======
	*secid = 0;
	call_void_hook(ipc_getsecid, ipcp, secid);
>>>>>>> v4.9.227
}

int security_msg_msg_alloc(struct msg_msg *msg)
{
<<<<<<< HEAD
	return security_ops->msg_msg_alloc_security(msg);
=======
	return call_int_hook(msg_msg_alloc_security, 0, msg);
>>>>>>> v4.9.227
}

void security_msg_msg_free(struct msg_msg *msg)
{
<<<<<<< HEAD
	security_ops->msg_msg_free_security(msg);
=======
	call_void_hook(msg_msg_free_security, msg);
>>>>>>> v4.9.227
}

int security_msg_queue_alloc(struct msg_queue *msq)
{
<<<<<<< HEAD
	return security_ops->msg_queue_alloc_security(msq);
=======
	return call_int_hook(msg_queue_alloc_security, 0, msq);
>>>>>>> v4.9.227
}

void security_msg_queue_free(struct msg_queue *msq)
{
<<<<<<< HEAD
	security_ops->msg_queue_free_security(msq);
=======
	call_void_hook(msg_queue_free_security, msq);
>>>>>>> v4.9.227
}

int security_msg_queue_associate(struct msg_queue *msq, int msqflg)
{
<<<<<<< HEAD
	return security_ops->msg_queue_associate(msq, msqflg);
=======
	return call_int_hook(msg_queue_associate, 0, msq, msqflg);
>>>>>>> v4.9.227
}

int security_msg_queue_msgctl(struct msg_queue *msq, int cmd)
{
<<<<<<< HEAD
	return security_ops->msg_queue_msgctl(msq, cmd);
=======
	return call_int_hook(msg_queue_msgctl, 0, msq, cmd);
>>>>>>> v4.9.227
}

int security_msg_queue_msgsnd(struct msg_queue *msq,
			       struct msg_msg *msg, int msqflg)
{
<<<<<<< HEAD
	return security_ops->msg_queue_msgsnd(msq, msg, msqflg);
=======
	return call_int_hook(msg_queue_msgsnd, 0, msq, msg, msqflg);
>>>>>>> v4.9.227
}

int security_msg_queue_msgrcv(struct msg_queue *msq, struct msg_msg *msg,
			       struct task_struct *target, long type, int mode)
{
<<<<<<< HEAD
	return security_ops->msg_queue_msgrcv(msq, msg, target, type, mode);
=======
	return call_int_hook(msg_queue_msgrcv, 0, msq, msg, target, type, mode);
>>>>>>> v4.9.227
}

int security_shm_alloc(struct shmid_kernel *shp)
{
<<<<<<< HEAD
	return security_ops->shm_alloc_security(shp);
=======
	return call_int_hook(shm_alloc_security, 0, shp);
>>>>>>> v4.9.227
}

void security_shm_free(struct shmid_kernel *shp)
{
<<<<<<< HEAD
	security_ops->shm_free_security(shp);
=======
	call_void_hook(shm_free_security, shp);
>>>>>>> v4.9.227
}

int security_shm_associate(struct shmid_kernel *shp, int shmflg)
{
<<<<<<< HEAD
	return security_ops->shm_associate(shp, shmflg);
=======
	return call_int_hook(shm_associate, 0, shp, shmflg);
>>>>>>> v4.9.227
}

int security_shm_shmctl(struct shmid_kernel *shp, int cmd)
{
<<<<<<< HEAD
	return security_ops->shm_shmctl(shp, cmd);
=======
	return call_int_hook(shm_shmctl, 0, shp, cmd);
>>>>>>> v4.9.227
}

int security_shm_shmat(struct shmid_kernel *shp, char __user *shmaddr, int shmflg)
{
<<<<<<< HEAD
	return security_ops->shm_shmat(shp, shmaddr, shmflg);
=======
	return call_int_hook(shm_shmat, 0, shp, shmaddr, shmflg);
>>>>>>> v4.9.227
}

int security_sem_alloc(struct sem_array *sma)
{
<<<<<<< HEAD
	return security_ops->sem_alloc_security(sma);
=======
	return call_int_hook(sem_alloc_security, 0, sma);
>>>>>>> v4.9.227
}

void security_sem_free(struct sem_array *sma)
{
<<<<<<< HEAD
	security_ops->sem_free_security(sma);
=======
	call_void_hook(sem_free_security, sma);
>>>>>>> v4.9.227
}

int security_sem_associate(struct sem_array *sma, int semflg)
{
<<<<<<< HEAD
	return security_ops->sem_associate(sma, semflg);
=======
	return call_int_hook(sem_associate, 0, sma, semflg);
>>>>>>> v4.9.227
}

int security_sem_semctl(struct sem_array *sma, int cmd)
{
<<<<<<< HEAD
	return security_ops->sem_semctl(sma, cmd);
=======
	return call_int_hook(sem_semctl, 0, sma, cmd);
>>>>>>> v4.9.227
}

int security_sem_semop(struct sem_array *sma, struct sembuf *sops,
			unsigned nsops, int alter)
{
<<<<<<< HEAD
	return security_ops->sem_semop(sma, sops, nsops, alter);
=======
	return call_int_hook(sem_semop, 0, sma, sops, nsops, alter);
>>>>>>> v4.9.227
}

void security_d_instantiate(struct dentry *dentry, struct inode *inode)
{
	if (unlikely(inode && IS_PRIVATE(inode)))
		return;
<<<<<<< HEAD
	security_ops->d_instantiate(dentry, inode);
=======
	call_void_hook(d_instantiate, dentry, inode);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_d_instantiate);

int security_getprocattr(struct task_struct *p, char *name, char **value)
{
<<<<<<< HEAD
	return security_ops->getprocattr(p, name, value);
=======
	return call_int_hook(getprocattr, -EINVAL, p, name, value);
>>>>>>> v4.9.227
}

int security_setprocattr(struct task_struct *p, char *name, void *value, size_t size)
{
<<<<<<< HEAD
	return security_ops->setprocattr(p, name, value, size);
=======
	return call_int_hook(setprocattr, -EINVAL, p, name, value, size);
>>>>>>> v4.9.227
}

int security_netlink_send(struct sock *sk, struct sk_buff *skb)
{
<<<<<<< HEAD
	return security_ops->netlink_send(sk, skb);
=======
	return call_int_hook(netlink_send, 0, sk, skb);
>>>>>>> v4.9.227
}

int security_ismaclabel(const char *name)
{
<<<<<<< HEAD
	return security_ops->ismaclabel(name);
=======
	return call_int_hook(ismaclabel, 0, name);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_ismaclabel);

int security_secid_to_secctx(u32 secid, char **secdata, u32 *seclen)
{
<<<<<<< HEAD
	return security_ops->secid_to_secctx(secid, secdata, seclen);
=======
	return call_int_hook(secid_to_secctx, -EOPNOTSUPP, secid, secdata,
				seclen);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_secid_to_secctx);

int security_secctx_to_secid(const char *secdata, u32 seclen, u32 *secid)
{
<<<<<<< HEAD
	return security_ops->secctx_to_secid(secdata, seclen, secid);
=======
	*secid = 0;
	return call_int_hook(secctx_to_secid, 0, secdata, seclen, secid);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_secctx_to_secid);

void security_release_secctx(char *secdata, u32 seclen)
{
<<<<<<< HEAD
	security_ops->release_secctx(secdata, seclen);
}
EXPORT_SYMBOL(security_release_secctx);

int security_inode_notifysecctx(struct inode *inode, void *ctx, u32 ctxlen)
{
	return security_ops->inode_notifysecctx(inode, ctx, ctxlen);
=======
	call_void_hook(release_secctx, secdata, seclen);
}
EXPORT_SYMBOL(security_release_secctx);

void security_inode_invalidate_secctx(struct inode *inode)
{
	call_void_hook(inode_invalidate_secctx, inode);
}
EXPORT_SYMBOL(security_inode_invalidate_secctx);

int security_inode_notifysecctx(struct inode *inode, void *ctx, u32 ctxlen)
{
	return call_int_hook(inode_notifysecctx, 0, inode, ctx, ctxlen);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_inode_notifysecctx);

int security_inode_setsecctx(struct dentry *dentry, void *ctx, u32 ctxlen)
{
<<<<<<< HEAD
	return security_ops->inode_setsecctx(dentry, ctx, ctxlen);
=======
	return call_int_hook(inode_setsecctx, 0, dentry, ctx, ctxlen);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_inode_setsecctx);

int security_inode_getsecctx(struct inode *inode, void **ctx, u32 *ctxlen)
{
<<<<<<< HEAD
	return security_ops->inode_getsecctx(inode, ctx, ctxlen);
=======
	return call_int_hook(inode_getsecctx, -EOPNOTSUPP, inode, ctx, ctxlen);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_inode_getsecctx);

#ifdef CONFIG_SECURITY_NETWORK

int security_unix_stream_connect(struct sock *sock, struct sock *other, struct sock *newsk)
{
<<<<<<< HEAD
	return security_ops->unix_stream_connect(sock, other, newsk);
=======
	return call_int_hook(unix_stream_connect, 0, sock, other, newsk);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_unix_stream_connect);

int security_unix_may_send(struct socket *sock,  struct socket *other)
{
<<<<<<< HEAD
	return security_ops->unix_may_send(sock, other);
=======
	return call_int_hook(unix_may_send, 0, sock, other);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_unix_may_send);

int security_socket_create(int family, int type, int protocol, int kern)
{
<<<<<<< HEAD
	return security_ops->socket_create(family, type, protocol, kern);
=======
	return call_int_hook(socket_create, 0, family, type, protocol, kern);
>>>>>>> v4.9.227
}

int security_socket_post_create(struct socket *sock, int family,
				int type, int protocol, int kern)
{
<<<<<<< HEAD
	return security_ops->socket_post_create(sock, family, type,
=======
	return call_int_hook(socket_post_create, 0, sock, family, type,
>>>>>>> v4.9.227
						protocol, kern);
}

int security_socket_bind(struct socket *sock, struct sockaddr *address, int addrlen)
{
<<<<<<< HEAD
	return security_ops->socket_bind(sock, address, addrlen);
=======
	return call_int_hook(socket_bind, 0, sock, address, addrlen);
>>>>>>> v4.9.227
}

int security_socket_connect(struct socket *sock, struct sockaddr *address, int addrlen)
{
<<<<<<< HEAD
	return security_ops->socket_connect(sock, address, addrlen);
=======
	return call_int_hook(socket_connect, 0, sock, address, addrlen);
>>>>>>> v4.9.227
}

int security_socket_listen(struct socket *sock, int backlog)
{
<<<<<<< HEAD
	return security_ops->socket_listen(sock, backlog);
=======
	return call_int_hook(socket_listen, 0, sock, backlog);
>>>>>>> v4.9.227
}

int security_socket_accept(struct socket *sock, struct socket *newsock)
{
<<<<<<< HEAD
	return security_ops->socket_accept(sock, newsock);
=======
	return call_int_hook(socket_accept, 0, sock, newsock);
>>>>>>> v4.9.227
}

int security_socket_sendmsg(struct socket *sock, struct msghdr *msg, int size)
{
<<<<<<< HEAD
	return security_ops->socket_sendmsg(sock, msg, size);
=======
	return call_int_hook(socket_sendmsg, 0, sock, msg, size);
>>>>>>> v4.9.227
}

int security_socket_recvmsg(struct socket *sock, struct msghdr *msg,
			    int size, int flags)
{
<<<<<<< HEAD
	return security_ops->socket_recvmsg(sock, msg, size, flags);
=======
	return call_int_hook(socket_recvmsg, 0, sock, msg, size, flags);
>>>>>>> v4.9.227
}

int security_socket_getsockname(struct socket *sock)
{
<<<<<<< HEAD
	return security_ops->socket_getsockname(sock);
=======
	return call_int_hook(socket_getsockname, 0, sock);
>>>>>>> v4.9.227
}

int security_socket_getpeername(struct socket *sock)
{
<<<<<<< HEAD
	return security_ops->socket_getpeername(sock);
=======
	return call_int_hook(socket_getpeername, 0, sock);
>>>>>>> v4.9.227
}

int security_socket_getsockopt(struct socket *sock, int level, int optname)
{
<<<<<<< HEAD
	return security_ops->socket_getsockopt(sock, level, optname);
=======
	return call_int_hook(socket_getsockopt, 0, sock, level, optname);
>>>>>>> v4.9.227
}

int security_socket_setsockopt(struct socket *sock, int level, int optname)
{
<<<<<<< HEAD
	return security_ops->socket_setsockopt(sock, level, optname);
=======
	return call_int_hook(socket_setsockopt, 0, sock, level, optname);
>>>>>>> v4.9.227
}

int security_socket_shutdown(struct socket *sock, int how)
{
<<<<<<< HEAD
	return security_ops->socket_shutdown(sock, how);
=======
	return call_int_hook(socket_shutdown, 0, sock, how);
>>>>>>> v4.9.227
}

int security_sock_rcv_skb(struct sock *sk, struct sk_buff *skb)
{
<<<<<<< HEAD
	return security_ops->socket_sock_rcv_skb(sk, skb);
=======
	return call_int_hook(socket_sock_rcv_skb, 0, sk, skb);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_sock_rcv_skb);

int security_socket_getpeersec_stream(struct socket *sock, char __user *optval,
				      int __user *optlen, unsigned len)
{
<<<<<<< HEAD
	return security_ops->socket_getpeersec_stream(sock, optval, optlen, len);
=======
	return call_int_hook(socket_getpeersec_stream, -ENOPROTOOPT, sock,
				optval, optlen, len);
>>>>>>> v4.9.227
}

int security_socket_getpeersec_dgram(struct socket *sock, struct sk_buff *skb, u32 *secid)
{
<<<<<<< HEAD
	return security_ops->socket_getpeersec_dgram(sock, skb, secid);
=======
	return call_int_hook(socket_getpeersec_dgram, -ENOPROTOOPT, sock,
			     skb, secid);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_socket_getpeersec_dgram);

int security_sk_alloc(struct sock *sk, int family, gfp_t priority)
{
<<<<<<< HEAD
	return security_ops->sk_alloc_security(sk, family, priority);
=======
	return call_int_hook(sk_alloc_security, 0, sk, family, priority);
>>>>>>> v4.9.227
}

void security_sk_free(struct sock *sk)
{
<<<<<<< HEAD
	security_ops->sk_free_security(sk);
=======
	call_void_hook(sk_free_security, sk);
>>>>>>> v4.9.227
}

void security_sk_clone(const struct sock *sk, struct sock *newsk)
{
<<<<<<< HEAD
	security_ops->sk_clone_security(sk, newsk);
=======
	call_void_hook(sk_clone_security, sk, newsk);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_sk_clone);

void security_sk_classify_flow(struct sock *sk, struct flowi *fl)
{
<<<<<<< HEAD
	security_ops->sk_getsecid(sk, &fl->flowi_secid);
=======
	call_void_hook(sk_getsecid, sk, &fl->flowi_secid);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_sk_classify_flow);

void security_req_classify_flow(const struct request_sock *req, struct flowi *fl)
{
<<<<<<< HEAD
	security_ops->req_classify_flow(req, fl);
=======
	call_void_hook(req_classify_flow, req, fl);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_req_classify_flow);

void security_sock_graft(struct sock *sk, struct socket *parent)
{
<<<<<<< HEAD
	security_ops->sock_graft(sk, parent);
=======
	call_void_hook(sock_graft, sk, parent);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_sock_graft);

int security_inet_conn_request(struct sock *sk,
			struct sk_buff *skb, struct request_sock *req)
{
<<<<<<< HEAD
	return security_ops->inet_conn_request(sk, skb, req);
=======
	return call_int_hook(inet_conn_request, 0, sk, skb, req);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_inet_conn_request);

void security_inet_csk_clone(struct sock *newsk,
			const struct request_sock *req)
{
<<<<<<< HEAD
	security_ops->inet_csk_clone(newsk, req);
=======
	call_void_hook(inet_csk_clone, newsk, req);
>>>>>>> v4.9.227
}

void security_inet_conn_established(struct sock *sk,
			struct sk_buff *skb)
{
<<<<<<< HEAD
	security_ops->inet_conn_established(sk, skb);
=======
	call_void_hook(inet_conn_established, sk, skb);
>>>>>>> v4.9.227
}

int security_secmark_relabel_packet(u32 secid)
{
<<<<<<< HEAD
	return security_ops->secmark_relabel_packet(secid);
=======
	return call_int_hook(secmark_relabel_packet, 0, secid);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_secmark_relabel_packet);

void security_secmark_refcount_inc(void)
{
<<<<<<< HEAD
	security_ops->secmark_refcount_inc();
=======
	call_void_hook(secmark_refcount_inc);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_secmark_refcount_inc);

void security_secmark_refcount_dec(void)
{
<<<<<<< HEAD
	security_ops->secmark_refcount_dec();
=======
	call_void_hook(secmark_refcount_dec);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_secmark_refcount_dec);

int security_tun_dev_alloc_security(void **security)
{
<<<<<<< HEAD
	return security_ops->tun_dev_alloc_security(security);
=======
	return call_int_hook(tun_dev_alloc_security, 0, security);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_tun_dev_alloc_security);

void security_tun_dev_free_security(void *security)
{
<<<<<<< HEAD
	security_ops->tun_dev_free_security(security);
=======
	call_void_hook(tun_dev_free_security, security);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_tun_dev_free_security);

int security_tun_dev_create(void)
{
<<<<<<< HEAD
	return security_ops->tun_dev_create();
=======
	return call_int_hook(tun_dev_create, 0);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_tun_dev_create);

int security_tun_dev_attach_queue(void *security)
{
<<<<<<< HEAD
	return security_ops->tun_dev_attach_queue(security);
=======
	return call_int_hook(tun_dev_attach_queue, 0, security);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_tun_dev_attach_queue);

int security_tun_dev_attach(struct sock *sk, void *security)
{
<<<<<<< HEAD
	return security_ops->tun_dev_attach(sk, security);
=======
	return call_int_hook(tun_dev_attach, 0, sk, security);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_tun_dev_attach);

int security_tun_dev_open(void *security)
{
<<<<<<< HEAD
	return security_ops->tun_dev_open(security);
}
EXPORT_SYMBOL(security_tun_dev_open);

void security_skb_owned_by(struct sk_buff *skb, struct sock *sk)
{
	security_ops->skb_owned_by(skb, sk);
}

=======
	return call_int_hook(tun_dev_open, 0, security);
}
EXPORT_SYMBOL(security_tun_dev_open);

>>>>>>> v4.9.227
#endif	/* CONFIG_SECURITY_NETWORK */

#ifdef CONFIG_SECURITY_NETWORK_XFRM

int security_xfrm_policy_alloc(struct xfrm_sec_ctx **ctxp,
			       struct xfrm_user_sec_ctx *sec_ctx,
			       gfp_t gfp)
{
<<<<<<< HEAD
	return security_ops->xfrm_policy_alloc_security(ctxp, sec_ctx, gfp);
=======
	return call_int_hook(xfrm_policy_alloc_security, 0, ctxp, sec_ctx, gfp);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_xfrm_policy_alloc);

int security_xfrm_policy_clone(struct xfrm_sec_ctx *old_ctx,
			      struct xfrm_sec_ctx **new_ctxp)
{
<<<<<<< HEAD
	return security_ops->xfrm_policy_clone_security(old_ctx, new_ctxp);
=======
	return call_int_hook(xfrm_policy_clone_security, 0, old_ctx, new_ctxp);
>>>>>>> v4.9.227
}

void security_xfrm_policy_free(struct xfrm_sec_ctx *ctx)
{
<<<<<<< HEAD
	security_ops->xfrm_policy_free_security(ctx);
=======
	call_void_hook(xfrm_policy_free_security, ctx);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_xfrm_policy_free);

int security_xfrm_policy_delete(struct xfrm_sec_ctx *ctx)
{
<<<<<<< HEAD
	return security_ops->xfrm_policy_delete_security(ctx);
=======
	return call_int_hook(xfrm_policy_delete_security, 0, ctx);
>>>>>>> v4.9.227
}

int security_xfrm_state_alloc(struct xfrm_state *x,
			      struct xfrm_user_sec_ctx *sec_ctx)
{
<<<<<<< HEAD
	return security_ops->xfrm_state_alloc(x, sec_ctx);
=======
	return call_int_hook(xfrm_state_alloc, 0, x, sec_ctx);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_xfrm_state_alloc);

int security_xfrm_state_alloc_acquire(struct xfrm_state *x,
				      struct xfrm_sec_ctx *polsec, u32 secid)
{
<<<<<<< HEAD
	return security_ops->xfrm_state_alloc_acquire(x, polsec, secid);
=======
	return call_int_hook(xfrm_state_alloc_acquire, 0, x, polsec, secid);
>>>>>>> v4.9.227
}

int security_xfrm_state_delete(struct xfrm_state *x)
{
<<<<<<< HEAD
	return security_ops->xfrm_state_delete_security(x);
=======
	return call_int_hook(xfrm_state_delete_security, 0, x);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(security_xfrm_state_delete);

void security_xfrm_state_free(struct xfrm_state *x)
{
<<<<<<< HEAD
	security_ops->xfrm_state_free_security(x);
=======
	call_void_hook(xfrm_state_free_security, x);
>>>>>>> v4.9.227
}

int security_xfrm_policy_lookup(struct xfrm_sec_ctx *ctx, u32 fl_secid, u8 dir)
{
<<<<<<< HEAD
	return security_ops->xfrm_policy_lookup(ctx, fl_secid, dir);
=======
	return call_int_hook(xfrm_policy_lookup, 0, ctx, fl_secid, dir);
>>>>>>> v4.9.227
}

int security_xfrm_state_pol_flow_match(struct xfrm_state *x,
				       struct xfrm_policy *xp,
				       const struct flowi *fl)
{
<<<<<<< HEAD
	return security_ops->xfrm_state_pol_flow_match(x, xp, fl);
=======
	struct security_hook_list *hp;
	int rc = 1;

	/*
	 * Since this function is expected to return 0 or 1, the judgment
	 * becomes difficult if multiple LSMs supply this call. Fortunately,
	 * we can use the first LSM's judgment because currently only SELinux
	 * supplies this call.
	 *
	 * For speed optimization, we explicitly break the loop rather than
	 * using the macro
	 */
	list_for_each_entry(hp, &security_hook_heads.xfrm_state_pol_flow_match,
				list) {
		rc = hp->hook.xfrm_state_pol_flow_match(x, xp, fl);
		break;
	}
	return rc;
>>>>>>> v4.9.227
}

int security_xfrm_decode_session(struct sk_buff *skb, u32 *secid)
{
<<<<<<< HEAD
	return security_ops->xfrm_decode_session(skb, secid, 1);
=======
	return call_int_hook(xfrm_decode_session, 0, skb, secid, 1);
>>>>>>> v4.9.227
}

void security_skb_classify_flow(struct sk_buff *skb, struct flowi *fl)
{
<<<<<<< HEAD
	int rc = security_ops->xfrm_decode_session(skb, &fl->flowi_secid, 0);
=======
	int rc = call_int_hook(xfrm_decode_session, 0, skb, &fl->flowi_secid,
				0);
>>>>>>> v4.9.227

	BUG_ON(rc);
}
EXPORT_SYMBOL(security_skb_classify_flow);

#endif	/* CONFIG_SECURITY_NETWORK_XFRM */

#ifdef CONFIG_KEYS

int security_key_alloc(struct key *key, const struct cred *cred,
		       unsigned long flags)
{
<<<<<<< HEAD
	return security_ops->key_alloc(key, cred, flags);
=======
	return call_int_hook(key_alloc, 0, key, cred, flags);
>>>>>>> v4.9.227
}

void security_key_free(struct key *key)
{
<<<<<<< HEAD
	security_ops->key_free(key);
=======
	call_void_hook(key_free, key);
>>>>>>> v4.9.227
}

int security_key_permission(key_ref_t key_ref,
			    const struct cred *cred, unsigned perm)
{
<<<<<<< HEAD
	return security_ops->key_permission(key_ref, cred, perm);
=======
	return call_int_hook(key_permission, 0, key_ref, cred, perm);
>>>>>>> v4.9.227
}

int security_key_getsecurity(struct key *key, char **_buffer)
{
<<<<<<< HEAD
	return security_ops->key_getsecurity(key, _buffer);
=======
	*_buffer = NULL;
	return call_int_hook(key_getsecurity, 0, key, _buffer);
>>>>>>> v4.9.227
}

#endif	/* CONFIG_KEYS */

#ifdef CONFIG_AUDIT

int security_audit_rule_init(u32 field, u32 op, char *rulestr, void **lsmrule)
{
<<<<<<< HEAD
	return security_ops->audit_rule_init(field, op, rulestr, lsmrule);
=======
	return call_int_hook(audit_rule_init, 0, field, op, rulestr, lsmrule);
>>>>>>> v4.9.227
}

int security_audit_rule_known(struct audit_krule *krule)
{
<<<<<<< HEAD
	return security_ops->audit_rule_known(krule);
=======
	return call_int_hook(audit_rule_known, 0, krule);
>>>>>>> v4.9.227
}

void security_audit_rule_free(void *lsmrule)
{
<<<<<<< HEAD
	security_ops->audit_rule_free(lsmrule);
=======
	call_void_hook(audit_rule_free, lsmrule);
>>>>>>> v4.9.227
}

int security_audit_rule_match(u32 secid, u32 field, u32 op, void *lsmrule,
			      struct audit_context *actx)
{
<<<<<<< HEAD
	return security_ops->audit_rule_match(secid, field, op, lsmrule, actx);
}

#endif /* CONFIG_AUDIT */
=======
	return call_int_hook(audit_rule_match, 0, secid, field, op, lsmrule,
				actx);
}
#endif /* CONFIG_AUDIT */

struct security_hook_heads security_hook_heads = {
	.binder_set_context_mgr =
		LIST_HEAD_INIT(security_hook_heads.binder_set_context_mgr),
	.binder_transaction =
		LIST_HEAD_INIT(security_hook_heads.binder_transaction),
	.binder_transfer_binder =
		LIST_HEAD_INIT(security_hook_heads.binder_transfer_binder),
	.binder_transfer_file =
		LIST_HEAD_INIT(security_hook_heads.binder_transfer_file),

	.ptrace_access_check =
		LIST_HEAD_INIT(security_hook_heads.ptrace_access_check),
	.ptrace_traceme =
		LIST_HEAD_INIT(security_hook_heads.ptrace_traceme),
	.capget =	LIST_HEAD_INIT(security_hook_heads.capget),
	.capset =	LIST_HEAD_INIT(security_hook_heads.capset),
	.capable =	LIST_HEAD_INIT(security_hook_heads.capable),
	.quotactl =	LIST_HEAD_INIT(security_hook_heads.quotactl),
	.quota_on =	LIST_HEAD_INIT(security_hook_heads.quota_on),
	.syslog =	LIST_HEAD_INIT(security_hook_heads.syslog),
	.settime =	LIST_HEAD_INIT(security_hook_heads.settime),
	.vm_enough_memory =
		LIST_HEAD_INIT(security_hook_heads.vm_enough_memory),
	.bprm_set_creds =
		LIST_HEAD_INIT(security_hook_heads.bprm_set_creds),
	.bprm_check_security =
		LIST_HEAD_INIT(security_hook_heads.bprm_check_security),
	.bprm_secureexec =
		LIST_HEAD_INIT(security_hook_heads.bprm_secureexec),
	.bprm_committing_creds =
		LIST_HEAD_INIT(security_hook_heads.bprm_committing_creds),
	.bprm_committed_creds =
		LIST_HEAD_INIT(security_hook_heads.bprm_committed_creds),
	.sb_alloc_security =
		LIST_HEAD_INIT(security_hook_heads.sb_alloc_security),
	.sb_free_security =
		LIST_HEAD_INIT(security_hook_heads.sb_free_security),
	.sb_copy_data =	LIST_HEAD_INIT(security_hook_heads.sb_copy_data),
	.sb_remount =	LIST_HEAD_INIT(security_hook_heads.sb_remount),
	.sb_kern_mount =
		LIST_HEAD_INIT(security_hook_heads.sb_kern_mount),
	.sb_show_options =
		LIST_HEAD_INIT(security_hook_heads.sb_show_options),
	.sb_statfs =	LIST_HEAD_INIT(security_hook_heads.sb_statfs),
	.sb_mount =	LIST_HEAD_INIT(security_hook_heads.sb_mount),
	.sb_umount =	LIST_HEAD_INIT(security_hook_heads.sb_umount),
	.sb_pivotroot =	LIST_HEAD_INIT(security_hook_heads.sb_pivotroot),
	.sb_set_mnt_opts =
		LIST_HEAD_INIT(security_hook_heads.sb_set_mnt_opts),
	.sb_clone_mnt_opts =
		LIST_HEAD_INIT(security_hook_heads.sb_clone_mnt_opts),
	.sb_parse_opts_str =
		LIST_HEAD_INIT(security_hook_heads.sb_parse_opts_str),
	.dentry_init_security =
		LIST_HEAD_INIT(security_hook_heads.dentry_init_security),
	.dentry_create_files_as =
		LIST_HEAD_INIT(security_hook_heads.dentry_create_files_as),
#ifdef CONFIG_SECURITY_PATH
	.path_unlink =	LIST_HEAD_INIT(security_hook_heads.path_unlink),
	.path_mkdir =	LIST_HEAD_INIT(security_hook_heads.path_mkdir),
	.path_rmdir =	LIST_HEAD_INIT(security_hook_heads.path_rmdir),
	.path_mknod =	LIST_HEAD_INIT(security_hook_heads.path_mknod),
	.path_truncate =
		LIST_HEAD_INIT(security_hook_heads.path_truncate),
	.path_symlink =	LIST_HEAD_INIT(security_hook_heads.path_symlink),
	.path_link =	LIST_HEAD_INIT(security_hook_heads.path_link),
	.path_rename =	LIST_HEAD_INIT(security_hook_heads.path_rename),
	.path_chmod =	LIST_HEAD_INIT(security_hook_heads.path_chmod),
	.path_chown =	LIST_HEAD_INIT(security_hook_heads.path_chown),
	.path_chroot =	LIST_HEAD_INIT(security_hook_heads.path_chroot),
#endif
	.inode_alloc_security =
		LIST_HEAD_INIT(security_hook_heads.inode_alloc_security),
	.inode_free_security =
		LIST_HEAD_INIT(security_hook_heads.inode_free_security),
	.inode_init_security =
		LIST_HEAD_INIT(security_hook_heads.inode_init_security),
	.inode_create =	LIST_HEAD_INIT(security_hook_heads.inode_create),
	.inode_link =	LIST_HEAD_INIT(security_hook_heads.inode_link),
	.inode_unlink =	LIST_HEAD_INIT(security_hook_heads.inode_unlink),
	.inode_symlink =
		LIST_HEAD_INIT(security_hook_heads.inode_symlink),
	.inode_mkdir =	LIST_HEAD_INIT(security_hook_heads.inode_mkdir),
	.inode_rmdir =	LIST_HEAD_INIT(security_hook_heads.inode_rmdir),
	.inode_mknod =	LIST_HEAD_INIT(security_hook_heads.inode_mknod),
	.inode_rename =	LIST_HEAD_INIT(security_hook_heads.inode_rename),
	.inode_readlink =
		LIST_HEAD_INIT(security_hook_heads.inode_readlink),
	.inode_follow_link =
		LIST_HEAD_INIT(security_hook_heads.inode_follow_link),
	.inode_permission =
		LIST_HEAD_INIT(security_hook_heads.inode_permission),
	.inode_setattr =
		LIST_HEAD_INIT(security_hook_heads.inode_setattr),
	.inode_getattr =
		LIST_HEAD_INIT(security_hook_heads.inode_getattr),
	.inode_setxattr =
		LIST_HEAD_INIT(security_hook_heads.inode_setxattr),
	.inode_post_setxattr =
		LIST_HEAD_INIT(security_hook_heads.inode_post_setxattr),
	.inode_getxattr =
		LIST_HEAD_INIT(security_hook_heads.inode_getxattr),
	.inode_listxattr =
		LIST_HEAD_INIT(security_hook_heads.inode_listxattr),
	.inode_removexattr =
		LIST_HEAD_INIT(security_hook_heads.inode_removexattr),
	.inode_need_killpriv =
		LIST_HEAD_INIT(security_hook_heads.inode_need_killpriv),
	.inode_killpriv =
		LIST_HEAD_INIT(security_hook_heads.inode_killpriv),
	.inode_getsecurity =
		LIST_HEAD_INIT(security_hook_heads.inode_getsecurity),
	.inode_setsecurity =
		LIST_HEAD_INIT(security_hook_heads.inode_setsecurity),
	.inode_listsecurity =
		LIST_HEAD_INIT(security_hook_heads.inode_listsecurity),
	.inode_getsecid =
		LIST_HEAD_INIT(security_hook_heads.inode_getsecid),
	.inode_copy_up =
		LIST_HEAD_INIT(security_hook_heads.inode_copy_up),
	.inode_copy_up_xattr =
		LIST_HEAD_INIT(security_hook_heads.inode_copy_up_xattr),
	.file_permission =
		LIST_HEAD_INIT(security_hook_heads.file_permission),
	.file_alloc_security =
		LIST_HEAD_INIT(security_hook_heads.file_alloc_security),
	.file_free_security =
		LIST_HEAD_INIT(security_hook_heads.file_free_security),
	.file_ioctl =	LIST_HEAD_INIT(security_hook_heads.file_ioctl),
	.mmap_addr =	LIST_HEAD_INIT(security_hook_heads.mmap_addr),
	.mmap_file =	LIST_HEAD_INIT(security_hook_heads.mmap_file),
	.file_mprotect =
		LIST_HEAD_INIT(security_hook_heads.file_mprotect),
	.file_lock =	LIST_HEAD_INIT(security_hook_heads.file_lock),
	.file_fcntl =	LIST_HEAD_INIT(security_hook_heads.file_fcntl),
	.file_set_fowner =
		LIST_HEAD_INIT(security_hook_heads.file_set_fowner),
	.file_send_sigiotask =
		LIST_HEAD_INIT(security_hook_heads.file_send_sigiotask),
	.file_receive =	LIST_HEAD_INIT(security_hook_heads.file_receive),
	.file_open =	LIST_HEAD_INIT(security_hook_heads.file_open),
	.task_create =	LIST_HEAD_INIT(security_hook_heads.task_create),
	.task_free =	LIST_HEAD_INIT(security_hook_heads.task_free),
	.cred_alloc_blank =
		LIST_HEAD_INIT(security_hook_heads.cred_alloc_blank),
	.cred_free =	LIST_HEAD_INIT(security_hook_heads.cred_free),
	.cred_prepare =	LIST_HEAD_INIT(security_hook_heads.cred_prepare),
	.cred_transfer =
		LIST_HEAD_INIT(security_hook_heads.cred_transfer),
	.kernel_act_as =
		LIST_HEAD_INIT(security_hook_heads.kernel_act_as),
	.kernel_create_files_as =
		LIST_HEAD_INIT(security_hook_heads.kernel_create_files_as),
	.kernel_module_request =
		LIST_HEAD_INIT(security_hook_heads.kernel_module_request),
	.kernel_read_file =
		LIST_HEAD_INIT(security_hook_heads.kernel_read_file),
	.kernel_post_read_file =
		LIST_HEAD_INIT(security_hook_heads.kernel_post_read_file),
	.task_fix_setuid =
		LIST_HEAD_INIT(security_hook_heads.task_fix_setuid),
	.task_setpgid =	LIST_HEAD_INIT(security_hook_heads.task_setpgid),
	.task_getpgid =	LIST_HEAD_INIT(security_hook_heads.task_getpgid),
	.task_getsid =	LIST_HEAD_INIT(security_hook_heads.task_getsid),
	.task_getsecid =
		LIST_HEAD_INIT(security_hook_heads.task_getsecid),
	.task_setnice =	LIST_HEAD_INIT(security_hook_heads.task_setnice),
	.task_setioprio =
		LIST_HEAD_INIT(security_hook_heads.task_setioprio),
	.task_getioprio =
		LIST_HEAD_INIT(security_hook_heads.task_getioprio),
	.task_setrlimit =
		LIST_HEAD_INIT(security_hook_heads.task_setrlimit),
	.task_setscheduler =
		LIST_HEAD_INIT(security_hook_heads.task_setscheduler),
	.task_getscheduler =
		LIST_HEAD_INIT(security_hook_heads.task_getscheduler),
	.task_movememory =
		LIST_HEAD_INIT(security_hook_heads.task_movememory),
	.task_kill =	LIST_HEAD_INIT(security_hook_heads.task_kill),
	.task_wait =	LIST_HEAD_INIT(security_hook_heads.task_wait),
	.task_prctl =	LIST_HEAD_INIT(security_hook_heads.task_prctl),
	.task_to_inode =
		LIST_HEAD_INIT(security_hook_heads.task_to_inode),
	.ipc_permission =
		LIST_HEAD_INIT(security_hook_heads.ipc_permission),
	.ipc_getsecid =	LIST_HEAD_INIT(security_hook_heads.ipc_getsecid),
	.msg_msg_alloc_security =
		LIST_HEAD_INIT(security_hook_heads.msg_msg_alloc_security),
	.msg_msg_free_security =
		LIST_HEAD_INIT(security_hook_heads.msg_msg_free_security),
	.msg_queue_alloc_security =
		LIST_HEAD_INIT(security_hook_heads.msg_queue_alloc_security),
	.msg_queue_free_security =
		LIST_HEAD_INIT(security_hook_heads.msg_queue_free_security),
	.msg_queue_associate =
		LIST_HEAD_INIT(security_hook_heads.msg_queue_associate),
	.msg_queue_msgctl =
		LIST_HEAD_INIT(security_hook_heads.msg_queue_msgctl),
	.msg_queue_msgsnd =
		LIST_HEAD_INIT(security_hook_heads.msg_queue_msgsnd),
	.msg_queue_msgrcv =
		LIST_HEAD_INIT(security_hook_heads.msg_queue_msgrcv),
	.shm_alloc_security =
		LIST_HEAD_INIT(security_hook_heads.shm_alloc_security),
	.shm_free_security =
		LIST_HEAD_INIT(security_hook_heads.shm_free_security),
	.shm_associate =
		LIST_HEAD_INIT(security_hook_heads.shm_associate),
	.shm_shmctl =	LIST_HEAD_INIT(security_hook_heads.shm_shmctl),
	.shm_shmat =	LIST_HEAD_INIT(security_hook_heads.shm_shmat),
	.sem_alloc_security =
		LIST_HEAD_INIT(security_hook_heads.sem_alloc_security),
	.sem_free_security =
		LIST_HEAD_INIT(security_hook_heads.sem_free_security),
	.sem_associate =
		LIST_HEAD_INIT(security_hook_heads.sem_associate),
	.sem_semctl =	LIST_HEAD_INIT(security_hook_heads.sem_semctl),
	.sem_semop =	LIST_HEAD_INIT(security_hook_heads.sem_semop),
	.netlink_send =	LIST_HEAD_INIT(security_hook_heads.netlink_send),
	.d_instantiate =
		LIST_HEAD_INIT(security_hook_heads.d_instantiate),
	.getprocattr =	LIST_HEAD_INIT(security_hook_heads.getprocattr),
	.setprocattr =	LIST_HEAD_INIT(security_hook_heads.setprocattr),
	.ismaclabel =	LIST_HEAD_INIT(security_hook_heads.ismaclabel),
	.secid_to_secctx =
		LIST_HEAD_INIT(security_hook_heads.secid_to_secctx),
	.secctx_to_secid =
		LIST_HEAD_INIT(security_hook_heads.secctx_to_secid),
	.release_secctx =
		LIST_HEAD_INIT(security_hook_heads.release_secctx),
	.inode_invalidate_secctx =
		LIST_HEAD_INIT(security_hook_heads.inode_invalidate_secctx),
	.inode_notifysecctx =
		LIST_HEAD_INIT(security_hook_heads.inode_notifysecctx),
	.inode_setsecctx =
		LIST_HEAD_INIT(security_hook_heads.inode_setsecctx),
	.inode_getsecctx =
		LIST_HEAD_INIT(security_hook_heads.inode_getsecctx),
#ifdef CONFIG_SECURITY_NETWORK
	.unix_stream_connect =
		LIST_HEAD_INIT(security_hook_heads.unix_stream_connect),
	.unix_may_send =
		LIST_HEAD_INIT(security_hook_heads.unix_may_send),
	.socket_create =
		LIST_HEAD_INIT(security_hook_heads.socket_create),
	.socket_post_create =
		LIST_HEAD_INIT(security_hook_heads.socket_post_create),
	.socket_bind =	LIST_HEAD_INIT(security_hook_heads.socket_bind),
	.socket_connect =
		LIST_HEAD_INIT(security_hook_heads.socket_connect),
	.socket_listen =
		LIST_HEAD_INIT(security_hook_heads.socket_listen),
	.socket_accept =
		LIST_HEAD_INIT(security_hook_heads.socket_accept),
	.socket_sendmsg =
		LIST_HEAD_INIT(security_hook_heads.socket_sendmsg),
	.socket_recvmsg =
		LIST_HEAD_INIT(security_hook_heads.socket_recvmsg),
	.socket_getsockname =
		LIST_HEAD_INIT(security_hook_heads.socket_getsockname),
	.socket_getpeername =
		LIST_HEAD_INIT(security_hook_heads.socket_getpeername),
	.socket_getsockopt =
		LIST_HEAD_INIT(security_hook_heads.socket_getsockopt),
	.socket_setsockopt =
		LIST_HEAD_INIT(security_hook_heads.socket_setsockopt),
	.socket_shutdown =
		LIST_HEAD_INIT(security_hook_heads.socket_shutdown),
	.socket_sock_rcv_skb =
		LIST_HEAD_INIT(security_hook_heads.socket_sock_rcv_skb),
	.socket_getpeersec_stream =
		LIST_HEAD_INIT(security_hook_heads.socket_getpeersec_stream),
	.socket_getpeersec_dgram =
		LIST_HEAD_INIT(security_hook_heads.socket_getpeersec_dgram),
	.sk_alloc_security =
		LIST_HEAD_INIT(security_hook_heads.sk_alloc_security),
	.sk_free_security =
		LIST_HEAD_INIT(security_hook_heads.sk_free_security),
	.sk_clone_security =
		LIST_HEAD_INIT(security_hook_heads.sk_clone_security),
	.sk_getsecid =	LIST_HEAD_INIT(security_hook_heads.sk_getsecid),
	.sock_graft =	LIST_HEAD_INIT(security_hook_heads.sock_graft),
	.inet_conn_request =
		LIST_HEAD_INIT(security_hook_heads.inet_conn_request),
	.inet_csk_clone =
		LIST_HEAD_INIT(security_hook_heads.inet_csk_clone),
	.inet_conn_established =
		LIST_HEAD_INIT(security_hook_heads.inet_conn_established),
	.secmark_relabel_packet =
		LIST_HEAD_INIT(security_hook_heads.secmark_relabel_packet),
	.secmark_refcount_inc =
		LIST_HEAD_INIT(security_hook_heads.secmark_refcount_inc),
	.secmark_refcount_dec =
		LIST_HEAD_INIT(security_hook_heads.secmark_refcount_dec),
	.req_classify_flow =
		LIST_HEAD_INIT(security_hook_heads.req_classify_flow),
	.tun_dev_alloc_security =
		LIST_HEAD_INIT(security_hook_heads.tun_dev_alloc_security),
	.tun_dev_free_security =
		LIST_HEAD_INIT(security_hook_heads.tun_dev_free_security),
	.tun_dev_create =
		LIST_HEAD_INIT(security_hook_heads.tun_dev_create),
	.tun_dev_attach_queue =
		LIST_HEAD_INIT(security_hook_heads.tun_dev_attach_queue),
	.tun_dev_attach =
		LIST_HEAD_INIT(security_hook_heads.tun_dev_attach),
	.tun_dev_open =	LIST_HEAD_INIT(security_hook_heads.tun_dev_open),
#endif	/* CONFIG_SECURITY_NETWORK */
#ifdef CONFIG_SECURITY_NETWORK_XFRM
	.xfrm_policy_alloc_security =
		LIST_HEAD_INIT(security_hook_heads.xfrm_policy_alloc_security),
	.xfrm_policy_clone_security =
		LIST_HEAD_INIT(security_hook_heads.xfrm_policy_clone_security),
	.xfrm_policy_free_security =
		LIST_HEAD_INIT(security_hook_heads.xfrm_policy_free_security),
	.xfrm_policy_delete_security =
		LIST_HEAD_INIT(security_hook_heads.xfrm_policy_delete_security),
	.xfrm_state_alloc =
		LIST_HEAD_INIT(security_hook_heads.xfrm_state_alloc),
	.xfrm_state_alloc_acquire =
		LIST_HEAD_INIT(security_hook_heads.xfrm_state_alloc_acquire),
	.xfrm_state_free_security =
		LIST_HEAD_INIT(security_hook_heads.xfrm_state_free_security),
	.xfrm_state_delete_security =
		LIST_HEAD_INIT(security_hook_heads.xfrm_state_delete_security),
	.xfrm_policy_lookup =
		LIST_HEAD_INIT(security_hook_heads.xfrm_policy_lookup),
	.xfrm_state_pol_flow_match =
		LIST_HEAD_INIT(security_hook_heads.xfrm_state_pol_flow_match),
	.xfrm_decode_session =
		LIST_HEAD_INIT(security_hook_heads.xfrm_decode_session),
#endif	/* CONFIG_SECURITY_NETWORK_XFRM */
#ifdef CONFIG_KEYS
	.key_alloc =	LIST_HEAD_INIT(security_hook_heads.key_alloc),
	.key_free =	LIST_HEAD_INIT(security_hook_heads.key_free),
	.key_permission =
		LIST_HEAD_INIT(security_hook_heads.key_permission),
	.key_getsecurity =
		LIST_HEAD_INIT(security_hook_heads.key_getsecurity),
#endif	/* CONFIG_KEYS */
#ifdef CONFIG_AUDIT
	.audit_rule_init =
		LIST_HEAD_INIT(security_hook_heads.audit_rule_init),
	.audit_rule_known =
		LIST_HEAD_INIT(security_hook_heads.audit_rule_known),
	.audit_rule_match =
		LIST_HEAD_INIT(security_hook_heads.audit_rule_match),
	.audit_rule_free =
		LIST_HEAD_INIT(security_hook_heads.audit_rule_free),
#endif /* CONFIG_AUDIT */
};
>>>>>>> v4.9.227
