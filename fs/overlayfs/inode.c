/*
 *
 * Copyright (C) 2011 Novell Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 */

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/xattr.h>
<<<<<<< HEAD
#include "overlayfs.h"

static int ovl_copy_up_last(struct dentry *dentry, struct iattr *attr,
			    bool no_data)
=======
#include <linux/posix_acl.h>
#include "overlayfs.h"

static int ovl_copy_up_truncate(struct dentry *dentry)
>>>>>>> v4.9.227
{
	int err;
	struct dentry *parent;
	struct kstat stat;
	struct path lowerpath;
<<<<<<< HEAD
=======
	const struct cred *old_cred;
>>>>>>> v4.9.227

	parent = dget_parent(dentry);
	err = ovl_copy_up(parent);
	if (err)
		goto out_dput_parent;

	ovl_path_lower(dentry, &lowerpath);
<<<<<<< HEAD
	err = vfs_getattr(&lowerpath, &stat);
	if (err)
		goto out_dput_parent;

	if (no_data)
		stat.size = 0;

	err = ovl_copy_up_one(parent, dentry, &lowerpath, &stat, attr);
=======

	old_cred = ovl_override_creds(dentry->d_sb);
	err = vfs_getattr(&lowerpath, &stat);
	if (!err) {
		stat.size = 0;
		err = ovl_copy_up_one(parent, dentry, &lowerpath, &stat);
	}
	revert_creds(old_cred);
>>>>>>> v4.9.227

out_dput_parent:
	dput(parent);
	return err;
}

int ovl_setattr(struct dentry *dentry, struct iattr *attr)
{
	int err;
	struct dentry *upperdentry;
<<<<<<< HEAD
=======
	const struct cred *old_cred;

	/*
	 * Check for permissions before trying to copy-up.  This is redundant
	 * since it will be rechecked later by ->setattr() on upper dentry.  But
	 * without this, copy-up can be triggered by just about anybody.
	 *
	 * We don't initialize inode->size, which just means that
	 * inode_newsize_ok() will always check against MAX_LFS_FILESIZE and not
	 * check for a swapfile (which this won't be anyway).
	 */
	err = setattr_prepare(dentry, attr);
	if (err)
		return err;
>>>>>>> v4.9.227

	err = ovl_want_write(dentry);
	if (err)
		goto out;

<<<<<<< HEAD
	err = ovl_copy_up(dentry);
	if (!err) {
		upperdentry = ovl_dentry_upper(dentry);

		mutex_lock(&upperdentry->d_inode->i_mutex);
=======
	if (attr->ia_valid & ATTR_SIZE) {
		struct inode *realinode = d_inode(ovl_dentry_real(dentry));

		err = -ETXTBSY;
		if (atomic_read(&realinode->i_writecount) < 0)
			goto out_drop_write;
	}

	err = ovl_copy_up(dentry);
	if (!err) {
		struct inode *winode = NULL;

		upperdentry = ovl_dentry_upper(dentry);

		if (attr->ia_valid & ATTR_SIZE) {
			winode = d_inode(upperdentry);
			err = get_write_access(winode);
			if (err)
				goto out_drop_write;
		}
>>>>>>> v4.9.227

		if (attr->ia_valid & (ATTR_KILL_SUID|ATTR_KILL_SGID))
			attr->ia_valid &= ~ATTR_MODE;

<<<<<<< HEAD
		err = notify_change(upperdentry, attr, NULL);
		if (!err)
			ovl_copyattr(upperdentry->d_inode, dentry->d_inode);
		mutex_unlock(&upperdentry->d_inode->i_mutex);
	}
=======
		inode_lock(upperdentry->d_inode);
		old_cred = ovl_override_creds(dentry->d_sb);
		err = notify_change(upperdentry, attr, NULL);
		revert_creds(old_cred);
		if (!err)
			ovl_copyattr(upperdentry->d_inode, dentry->d_inode);
		inode_unlock(upperdentry->d_inode);

		if (winode)
			put_write_access(winode);
	}
out_drop_write:
>>>>>>> v4.9.227
	ovl_drop_write(dentry);
out:
	return err;
}

static int ovl_getattr(struct vfsmount *mnt, struct dentry *dentry,
			 struct kstat *stat)
{
	struct path realpath;
<<<<<<< HEAD

	ovl_path_real(dentry, &realpath);
	return vfs_getattr(&realpath, stat);
=======
	const struct cred *old_cred;
	int err;

	ovl_path_real(dentry, &realpath);
	old_cred = ovl_override_creds(dentry->d_sb);
	err = vfs_getattr(&realpath, stat);
	revert_creds(old_cred);
	return err;
>>>>>>> v4.9.227
}

int ovl_permission(struct inode *inode, int mask)
{
<<<<<<< HEAD
	struct ovl_entry *oe;
	struct dentry *alias = NULL;
	struct inode *realinode;
	struct dentry *realdentry;
	bool is_upper;
	int err;

	if (S_ISDIR(inode->i_mode)) {
		oe = inode->i_private;
	} else if (mask & MAY_NOT_BLOCK) {
		return -ECHILD;
	} else {
		/*
		 * For non-directories find an alias and get the info
		 * from there.
		 */
		alias = d_find_any_alias(inode);
		if (WARN_ON(!alias))
			return -ENOENT;

		oe = alias->d_fsdata;
	}

	realdentry = ovl_entry_real(oe, &is_upper);

	/* Careful in RCU walk mode */
	realinode = ACCESS_ONCE(realdentry->d_inode);
	if (!realinode) {
		WARN_ON(!(mask & MAY_NOT_BLOCK));
		err = -ENOENT;
		goto out_dput;
	}

	if (mask & MAY_WRITE) {
		umode_t mode = realinode->i_mode;

		/*
		 * Writes will always be redirected to upper layer, so
		 * ignore lower layer being read-only.
		 *
		 * If the overlay itself is read-only then proceed
		 * with the permission check, don't return EROFS.
		 * This will only happen if this is the lower layer of
		 * another overlayfs.
		 *
		 * If upper fs becomes read-only after the overlay was
		 * constructed return EROFS to prevent modification of
		 * upper layer.
		 */
		err = -EROFS;
		if (is_upper && !IS_RDONLY(inode) && IS_RDONLY(realinode) &&
		    (S_ISREG(mode) || S_ISDIR(mode) || S_ISLNK(mode)))
			goto out_dput;
	}

	err = __inode_permission(realinode, mask);
out_dput:
	dput(alias);
	return err;
}


struct ovl_link_data {
	struct dentry *realdentry;
	void *cookie;
};

static void *ovl_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	void *ret;
	struct dentry *realdentry;
	struct inode *realinode;

	realdentry = ovl_dentry_real(dentry);
	realinode = realdentry->d_inode;

	if (WARN_ON(!realinode->i_op->follow_link))
		return ERR_PTR(-EPERM);

	ret = realinode->i_op->follow_link(realdentry, nd);
	if (IS_ERR(ret))
		return ret;

	if (realinode->i_op->put_link) {
		struct ovl_link_data *data;

		data = kmalloc(sizeof(struct ovl_link_data), GFP_KERNEL);
		if (!data) {
			realinode->i_op->put_link(realdentry, nd, ret);
			return ERR_PTR(-ENOMEM);
		}
		data->realdentry = realdentry;
		data->cookie = ret;

		return data;
	} else {
		return NULL;
	}
}

static void ovl_put_link(struct dentry *dentry, struct nameidata *nd, void *c)
{
	struct inode *realinode;
	struct ovl_link_data *data = c;

	if (!data)
		return;

	realinode = data->realdentry->d_inode;
	realinode->i_op->put_link(data->realdentry, nd, data->cookie);
	kfree(data);
}

static int ovl_readlink(struct dentry *dentry, char __user *buf, int bufsiz)
{
	struct path realpath;
	struct inode *realinode;

	ovl_path_real(dentry, &realpath);
	realinode = realpath.dentry->d_inode;

	if (!realinode->i_op->readlink)
		return -EINVAL;

	touch_atime(&realpath);

	return realinode->i_op->readlink(realpath.dentry, buf, bufsiz);
=======
	bool is_upper;
	struct inode *realinode = ovl_inode_real(inode, &is_upper);
	const struct cred *old_cred;
	int err;

	/* Careful in RCU walk mode */
	if (!realinode) {
		WARN_ON(!(mask & MAY_NOT_BLOCK));
		return -ECHILD;
	}

	/*
	 * Check overlay inode with the creds of task and underlying inode
	 * with creds of mounter
	 */
	err = generic_permission(inode, mask);
	if (err)
		return err;

	old_cred = ovl_override_creds(inode->i_sb);
	if (!is_upper && !special_file(realinode->i_mode) && mask & MAY_WRITE) {
		mask &= ~(MAY_WRITE | MAY_APPEND);
		/* Make sure mounter can read file for copy up later */
		mask |= MAY_READ;
	}
	err = inode_permission(realinode, mask);
	revert_creds(old_cred);

	return err;
}

static const char *ovl_get_link(struct dentry *dentry,
				struct inode *inode,
				struct delayed_call *done)
{
	const struct cred *old_cred;
	const char *p;

	if (!dentry)
		return ERR_PTR(-ECHILD);

	old_cred = ovl_override_creds(dentry->d_sb);
	p = vfs_get_link(ovl_dentry_real(dentry), done);
	revert_creds(old_cred);
	return p;
>>>>>>> v4.9.227
}

bool ovl_is_private_xattr(const char *name)
{
<<<<<<< HEAD
	return strncmp(name, "trusted.overlay.", 14) == 0;
}

int ovl_setxattr(struct dentry *dentry, const char *name,
		 const void *value, size_t size, int flags)
{
	int err;
	struct dentry *upperdentry;
=======
	return strncmp(name, OVL_XATTR_PREFIX,
		       sizeof(OVL_XATTR_PREFIX) - 1) == 0;
}

int ovl_xattr_set(struct dentry *dentry, const char *name, const void *value,
		  size_t size, int flags)
{
	int err;
	struct path realpath;
	enum ovl_path_type type = ovl_path_real(dentry, &realpath);
	const struct cred *old_cred;
>>>>>>> v4.9.227

	err = ovl_want_write(dentry);
	if (err)
		goto out;

<<<<<<< HEAD
	err = -EPERM;
	if (ovl_is_private_xattr(name))
		goto out_drop_write;
=======
	if (!value && !OVL_TYPE_UPPER(type)) {
		err = vfs_getxattr(realpath.dentry, name, NULL, 0);
		if (err < 0)
			goto out_drop_write;
	}
>>>>>>> v4.9.227

	err = ovl_copy_up(dentry);
	if (err)
		goto out_drop_write;

<<<<<<< HEAD
	upperdentry = ovl_dentry_upper(dentry);
	err = vfs_setxattr(upperdentry, name, value, size, flags);
=======
	if (!OVL_TYPE_UPPER(type))
		ovl_path_upper(dentry, &realpath);

	old_cred = ovl_override_creds(dentry->d_sb);
	if (value)
		err = vfs_setxattr(realpath.dentry, name, value, size, flags);
	else {
		WARN_ON(flags != XATTR_REPLACE);
		err = vfs_removexattr(realpath.dentry, name);
	}
	revert_creds(old_cred);
>>>>>>> v4.9.227

out_drop_write:
	ovl_drop_write(dentry);
out:
	return err;
}

<<<<<<< HEAD
static bool ovl_need_xattr_filter(struct dentry *dentry,
				  enum ovl_path_type type)
{
	return type == OVL_PATH_UPPER && S_ISDIR(dentry->d_inode->i_mode);
}

ssize_t ovl_getxattr(struct dentry *dentry, const char *name,
		     void *value, size_t size)
{
	struct path realpath;
	enum ovl_path_type type = ovl_path_real(dentry, &realpath);

	if (ovl_need_xattr_filter(dentry, type) && ovl_is_private_xattr(name))
		return -ENODATA;

	return vfs_getxattr(realpath.dentry, name, value, size);
=======
int ovl_xattr_get(struct dentry *dentry, const char *name,
		  void *value, size_t size)
{
	struct dentry *realdentry = ovl_dentry_real(dentry);
	ssize_t res;
	const struct cred *old_cred;

	old_cred = ovl_override_creds(dentry->d_sb);
	res = vfs_getxattr(realdentry, name, value, size);
	revert_creds(old_cred);
	return res;
>>>>>>> v4.9.227
}

static bool ovl_can_list(const char *s)
{
	/* List all non-trusted xatts */
	if (strncmp(s, XATTR_TRUSTED_PREFIX, XATTR_TRUSTED_PREFIX_LEN) != 0)
		return true;

	/* Never list trusted.overlay, list other trusted for superuser only */
<<<<<<< HEAD
	return !ovl_is_private_xattr(s) && capable(CAP_SYS_ADMIN);
=======
	return !ovl_is_private_xattr(s) &&
	       ns_capable_noaudit(&init_user_ns, CAP_SYS_ADMIN);
>>>>>>> v4.9.227
}

ssize_t ovl_listxattr(struct dentry *dentry, char *list, size_t size)
{
<<<<<<< HEAD
	struct path realpath;
	enum ovl_path_type type = ovl_path_real(dentry, &realpath);
	ssize_t res;
	size_t len;
	char *s;

	res = vfs_listxattr(realpath.dentry, list, size);
	if (res <= 0 || size == 0)
		return res;

	if (!ovl_need_xattr_filter(dentry, type))
		return res;

=======
	struct dentry *realdentry = ovl_dentry_real(dentry);
	ssize_t res;
	size_t len;
	char *s;
	const struct cred *old_cred;

	old_cred = ovl_override_creds(dentry->d_sb);
	res = vfs_listxattr(realdentry, list, size);
	revert_creds(old_cred);
	if (res <= 0 || size == 0)
		return res;

>>>>>>> v4.9.227
	/* filter out private xattrs */
	for (s = list, len = res; len;) {
		size_t slen = strnlen(s, len) + 1;

		/* underlying fs providing us with an broken xattr list? */
		if (WARN_ON(slen > len))
			return -EIO;

		len -= slen;
		if (!ovl_can_list(s)) {
			res -= slen;
			memmove(s, s + slen, len);
		} else {
			s += slen;
		}
	}

	return res;
}

<<<<<<< HEAD
int ovl_removexattr(struct dentry *dentry, const char *name)
{
	int err;
	struct path realpath;
	enum ovl_path_type type = ovl_path_real(dentry, &realpath);

	err = ovl_want_write(dentry);
	if (err)
		goto out;

	err = -ENODATA;
	if (ovl_need_xattr_filter(dentry, type) && ovl_is_private_xattr(name))
		goto out_drop_write;

	if (type == OVL_PATH_LOWER) {
		err = vfs_getxattr(realpath.dentry, name, NULL, 0);
		if (err < 0)
			goto out_drop_write;

		err = ovl_copy_up(dentry);
		if (err)
			goto out_drop_write;

		ovl_path_upper(dentry, &realpath);
	}

	err = vfs_removexattr(realpath.dentry, name);
out_drop_write:
	ovl_drop_write(dentry);
out:
	return err;
=======
struct posix_acl *ovl_get_acl(struct inode *inode, int type)
{
	struct inode *realinode = ovl_inode_real(inode, NULL);
	const struct cred *old_cred;
	struct posix_acl *acl;

	if (!IS_ENABLED(CONFIG_FS_POSIX_ACL) || !IS_POSIXACL(realinode))
		return NULL;

	old_cred = ovl_override_creds(inode->i_sb);
	acl = get_acl(realinode, type);
	revert_creds(old_cred);

	return acl;
>>>>>>> v4.9.227
}

static bool ovl_open_need_copy_up(int flags, enum ovl_path_type type,
				  struct dentry *realdentry)
{
<<<<<<< HEAD
	if (type != OVL_PATH_LOWER)
=======
	if (OVL_TYPE_UPPER(type))
>>>>>>> v4.9.227
		return false;

	if (special_file(realdentry->d_inode->i_mode))
		return false;

	if (!(OPEN_FMODE(flags) & FMODE_WRITE) && !(flags & O_TRUNC))
		return false;

	return true;
}

<<<<<<< HEAD
struct inode *ovl_d_select_inode(struct dentry *dentry, unsigned file_flags)
{
	int err;
	struct path realpath;
	enum ovl_path_type type;

	if (d_is_dir(dentry))
		return d_backing_inode(dentry);

	type = ovl_path_real(dentry, &realpath);
	if (ovl_open_need_copy_up(file_flags, type, realpath.dentry)) {
		err = ovl_want_write(dentry);
		if (err)
			return ERR_PTR(err);

		if (file_flags & O_TRUNC)
			err = ovl_copy_up_last(dentry, NULL, true);
		else
			err = ovl_copy_up(dentry);
		ovl_drop_write(dentry);
		if (err)
			return ERR_PTR(err);

		ovl_path_upper(dentry, &realpath);
	}

	return d_backing_inode(realpath.dentry);
=======
int ovl_open_maybe_copy_up(struct dentry *dentry, unsigned int file_flags)
{
	int err = 0;
	struct path realpath;
	enum ovl_path_type type;

	type = ovl_path_real(dentry, &realpath);
	if (ovl_open_need_copy_up(file_flags, type, realpath.dentry)) {
		err = ovl_want_write(dentry);
		if (!err) {
			if (file_flags & O_TRUNC)
				err = ovl_copy_up_truncate(dentry);
			else
				err = ovl_copy_up(dentry);
			ovl_drop_write(dentry);
		}
	}

	return err;
}

int ovl_update_time(struct inode *inode, struct timespec *ts, int flags)
{
	struct dentry *alias;
	struct path upperpath;

	if (!(flags & S_ATIME))
		return 0;

	alias = d_find_any_alias(inode);
	if (!alias)
		return 0;

	ovl_path_upper(alias, &upperpath);
	if (upperpath.dentry) {
		touch_atime(&upperpath);
		inode->i_atime = d_inode(upperpath.dentry)->i_atime;
	}

	dput(alias);

	return 0;
>>>>>>> v4.9.227
}

static const struct inode_operations ovl_file_inode_operations = {
	.setattr	= ovl_setattr,
	.permission	= ovl_permission,
	.getattr	= ovl_getattr,
<<<<<<< HEAD
	.setxattr	= ovl_setxattr,
	.getxattr	= ovl_getxattr,
	.listxattr	= ovl_listxattr,
	.removexattr	= ovl_removexattr,
=======
	.listxattr	= ovl_listxattr,
	.get_acl	= ovl_get_acl,
	.update_time	= ovl_update_time,
>>>>>>> v4.9.227
};

static const struct inode_operations ovl_symlink_inode_operations = {
	.setattr	= ovl_setattr,
<<<<<<< HEAD
	.follow_link	= ovl_follow_link,
	.put_link	= ovl_put_link,
	.readlink	= ovl_readlink,
	.getattr	= ovl_getattr,
	.setxattr	= ovl_setxattr,
	.getxattr	= ovl_getxattr,
	.listxattr	= ovl_listxattr,
	.removexattr	= ovl_removexattr,
};

struct inode *ovl_new_inode(struct super_block *sb, umode_t mode,
			    struct ovl_entry *oe)
{
	struct inode *inode;

	inode = new_inode(sb);
	if (!inode)
		return NULL;

	inode->i_ino = get_next_ino();
	inode->i_mode = mode;
	inode->i_flags |= S_NOATIME | S_NOCMTIME;
=======
	.get_link	= ovl_get_link,
	.readlink	= generic_readlink,
	.getattr	= ovl_getattr,
	.listxattr	= ovl_listxattr,
	.update_time	= ovl_update_time,
};

static void ovl_fill_inode(struct inode *inode, umode_t mode)
{
	inode->i_ino = get_next_ino();
	inode->i_mode = mode;
	inode->i_flags |= S_NOCMTIME;
#ifdef CONFIG_FS_POSIX_ACL
	inode->i_acl = inode->i_default_acl = ACL_DONT_CACHE;
#endif
>>>>>>> v4.9.227

	mode &= S_IFMT;
	switch (mode) {
	case S_IFDIR:
<<<<<<< HEAD
		inode->i_private = oe;
=======
>>>>>>> v4.9.227
		inode->i_op = &ovl_dir_inode_operations;
		inode->i_fop = &ovl_dir_operations;
		break;

	case S_IFLNK:
		inode->i_op = &ovl_symlink_inode_operations;
		break;

<<<<<<< HEAD
=======
	default:
		WARN(1, "illegal file type: %i\n", mode);
		/* Fall through */

>>>>>>> v4.9.227
	case S_IFREG:
	case S_IFSOCK:
	case S_IFBLK:
	case S_IFCHR:
	case S_IFIFO:
		inode->i_op = &ovl_file_inode_operations;
		break;
<<<<<<< HEAD

	default:
		WARN(1, "illegal file type: %i\n", mode);
		iput(inode);
		inode = NULL;
	}

	return inode;

=======
	}
}

struct inode *ovl_new_inode(struct super_block *sb, umode_t mode)
{
	struct inode *inode;

	inode = new_inode(sb);
	if (inode)
		ovl_fill_inode(inode, mode);

	return inode;
}

static int ovl_inode_test(struct inode *inode, void *data)
{
	return ovl_inode_real(inode, NULL) == data;
}

static int ovl_inode_set(struct inode *inode, void *data)
{
	inode->i_private = (void *) (((unsigned long) data) | OVL_ISUPPER_MASK);
	return 0;
}

struct inode *ovl_get_inode(struct super_block *sb, struct inode *realinode)

{
	struct inode *inode;

	inode = iget5_locked(sb, (unsigned long) realinode,
			     ovl_inode_test, ovl_inode_set, realinode);
	if (inode && inode->i_state & I_NEW) {
		ovl_fill_inode(inode, realinode->i_mode);
		set_nlink(inode, realinode->i_nlink);
		unlock_new_inode(inode);
	}

	return inode;
>>>>>>> v4.9.227
}
