/*
 * linux/fs/ext2/namei.c
 *
 * Rewrite to pagecache. Almost all code had been changed, so blame me
 * if the things go wrong. Please, send bug reports to
 * viro@parcelfarce.linux.theplanet.co.uk
 *
 * Stuff here is basically a glue between the VFS and generic UNIXish
 * filesystem that keeps everything in pagecache. All knowledge of the
 * directory layout is in fs/ext2/dir.c - it turned out to be easily separatable
 * and it's easier to debug that way. In principle we might want to
 * generalize that a bit and turn it into a library. Or not.
 *
 * The only non-static object here is ext2_dir_inode_operations.
 *
 * TODO: get rid of kmap() use, add readahead.
 *
 * Copyright (C) 1992, 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 *
 *  from
 *
 *  linux/fs/minix/namei.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  Big-endian to little-endian byte-swapping/bitmaps by
 *        David S. Miller (davem@caip.rutgers.edu), 1995
 */

#include <linux/pagemap.h>
#include <linux/quotaops.h>
#include "ext2.h"
#include "xattr.h"
#include "acl.h"
<<<<<<< HEAD
#include "xip.h"
=======
>>>>>>> v4.9.227

static inline int ext2_add_nondir(struct dentry *dentry, struct inode *inode)
{
	int err = ext2_add_link(dentry, inode);
	if (!err) {
		d_instantiate_new(dentry, inode);
		return 0;
	}
	inode_dec_link_count(inode);
	unlock_new_inode(inode);
	iput(inode);
	return err;
}

/*
 * Methods themselves.
 */

static struct dentry *ext2_lookup(struct inode * dir, struct dentry *dentry, unsigned int flags)
{
	struct inode * inode;
	ino_t ino;
	
	if (dentry->d_name.len > EXT2_NAME_LEN)
		return ERR_PTR(-ENAMETOOLONG);

	ino = ext2_inode_by_name(dir, &dentry->d_name);
	inode = NULL;
	if (ino) {
		inode = ext2_iget(dir->i_sb, ino);
		if (inode == ERR_PTR(-ESTALE)) {
			ext2_error(dir->i_sb, __func__,
					"deleted inode referenced: %lu",
					(unsigned long) ino);
			return ERR_PTR(-EIO);
		}
	}
	return d_splice_alias(inode, dentry);
}

struct dentry *ext2_get_parent(struct dentry *child)
{
	struct qstr dotdot = QSTR_INIT("..", 2);
<<<<<<< HEAD
	unsigned long ino = ext2_inode_by_name(child->d_inode, &dotdot);
	if (!ino)
		return ERR_PTR(-ENOENT);
	return d_obtain_alias(ext2_iget(child->d_inode->i_sb, ino));
=======
	unsigned long ino = ext2_inode_by_name(d_inode(child), &dotdot);
	if (!ino)
		return ERR_PTR(-ENOENT);
	return d_obtain_alias(ext2_iget(child->d_sb, ino));
>>>>>>> v4.9.227
} 

/*
 * By the time this is called, we already have created
 * the directory cache entry for the new file, but it
 * is so far negative - it has no inode.
 *
 * If the create succeeds, we fill in the inode information
 * with d_instantiate(). 
 */
static int ext2_create (struct inode * dir, struct dentry * dentry, umode_t mode, bool excl)
{
	struct inode *inode;
<<<<<<< HEAD

	dquot_initialize(dir);
=======
	int err;

	err = dquot_initialize(dir);
	if (err)
		return err;
>>>>>>> v4.9.227

	inode = ext2_new_inode(dir, mode, &dentry->d_name);
	if (IS_ERR(inode))
		return PTR_ERR(inode);

	inode->i_op = &ext2_file_inode_operations;
<<<<<<< HEAD
	if (ext2_use_xip(inode->i_sb)) {
		inode->i_mapping->a_ops = &ext2_aops_xip;
		inode->i_fop = &ext2_xip_file_operations;
	} else if (test_opt(inode->i_sb, NOBH)) {
=======
	if (test_opt(inode->i_sb, NOBH)) {
>>>>>>> v4.9.227
		inode->i_mapping->a_ops = &ext2_nobh_aops;
		inode->i_fop = &ext2_file_operations;
	} else {
		inode->i_mapping->a_ops = &ext2_aops;
		inode->i_fop = &ext2_file_operations;
	}
	mark_inode_dirty(inode);
	return ext2_add_nondir(dentry, inode);
}

static int ext2_tmpfile(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	struct inode *inode = ext2_new_inode(dir, mode, NULL);
	if (IS_ERR(inode))
		return PTR_ERR(inode);

	inode->i_op = &ext2_file_inode_operations;
<<<<<<< HEAD
	if (ext2_use_xip(inode->i_sb)) {
		inode->i_mapping->a_ops = &ext2_aops_xip;
		inode->i_fop = &ext2_xip_file_operations;
	} else if (test_opt(inode->i_sb, NOBH)) {
=======
	if (test_opt(inode->i_sb, NOBH)) {
>>>>>>> v4.9.227
		inode->i_mapping->a_ops = &ext2_nobh_aops;
		inode->i_fop = &ext2_file_operations;
	} else {
		inode->i_mapping->a_ops = &ext2_aops;
		inode->i_fop = &ext2_file_operations;
	}
	mark_inode_dirty(inode);
	d_tmpfile(dentry, inode);
	unlock_new_inode(inode);
	return 0;
}

static int ext2_mknod (struct inode * dir, struct dentry *dentry, umode_t mode, dev_t rdev)
{
	struct inode * inode;
	int err;

<<<<<<< HEAD
	if (!new_valid_dev(rdev))
		return -EINVAL;

	dquot_initialize(dir);
=======
	err = dquot_initialize(dir);
	if (err)
		return err;
>>>>>>> v4.9.227

	inode = ext2_new_inode (dir, mode, &dentry->d_name);
	err = PTR_ERR(inode);
	if (!IS_ERR(inode)) {
		init_special_inode(inode, inode->i_mode, rdev);
#ifdef CONFIG_EXT2_FS_XATTR
		inode->i_op = &ext2_special_inode_operations;
#endif
		mark_inode_dirty(inode);
		err = ext2_add_nondir(dentry, inode);
	}
	return err;
}

static int ext2_symlink (struct inode * dir, struct dentry * dentry,
	const char * symname)
{
	struct super_block * sb = dir->i_sb;
	int err = -ENAMETOOLONG;
	unsigned l = strlen(symname)+1;
	struct inode * inode;

	if (l > sb->s_blocksize)
		goto out;

<<<<<<< HEAD
	dquot_initialize(dir);
=======
	err = dquot_initialize(dir);
	if (err)
		goto out;
>>>>>>> v4.9.227

	inode = ext2_new_inode (dir, S_IFLNK | S_IRWXUGO, &dentry->d_name);
	err = PTR_ERR(inode);
	if (IS_ERR(inode))
		goto out;

	if (l > sizeof (EXT2_I(inode)->i_data)) {
		/* slow symlink */
		inode->i_op = &ext2_symlink_inode_operations;
<<<<<<< HEAD
=======
		inode_nohighmem(inode);
>>>>>>> v4.9.227
		if (test_opt(inode->i_sb, NOBH))
			inode->i_mapping->a_ops = &ext2_nobh_aops;
		else
			inode->i_mapping->a_ops = &ext2_aops;
		err = page_symlink(inode, symname, l);
		if (err)
			goto out_fail;
	} else {
		/* fast symlink */
		inode->i_op = &ext2_fast_symlink_inode_operations;
<<<<<<< HEAD
		memcpy((char*)(EXT2_I(inode)->i_data),symname,l);
=======
		inode->i_link = (char*)EXT2_I(inode)->i_data;
		memcpy(inode->i_link, symname, l);
>>>>>>> v4.9.227
		inode->i_size = l-1;
	}
	mark_inode_dirty(inode);

	err = ext2_add_nondir(dentry, inode);
out:
	return err;

out_fail:
	inode_dec_link_count(inode);
	unlock_new_inode(inode);
	iput (inode);
	goto out;
}

static int ext2_link (struct dentry * old_dentry, struct inode * dir,
	struct dentry *dentry)
{
<<<<<<< HEAD
	struct inode *inode = old_dentry->d_inode;
	int err;

	dquot_initialize(dir);

	inode->i_ctime = CURRENT_TIME_SEC;
=======
	struct inode *inode = d_inode(old_dentry);
	int err;

	err = dquot_initialize(dir);
	if (err)
		return err;

	inode->i_ctime = current_time(inode);
>>>>>>> v4.9.227
	inode_inc_link_count(inode);
	ihold(inode);

	err = ext2_add_link(dentry, inode);
	if (!err) {
		d_instantiate(dentry, inode);
		return 0;
	}
	inode_dec_link_count(inode);
	iput(inode);
	return err;
}

static int ext2_mkdir(struct inode * dir, struct dentry * dentry, umode_t mode)
{
	struct inode * inode;
	int err;

<<<<<<< HEAD
	dquot_initialize(dir);
=======
	err = dquot_initialize(dir);
	if (err)
		return err;
>>>>>>> v4.9.227

	inode_inc_link_count(dir);

	inode = ext2_new_inode(dir, S_IFDIR | mode, &dentry->d_name);
	err = PTR_ERR(inode);
	if (IS_ERR(inode))
		goto out_dir;

	inode->i_op = &ext2_dir_inode_operations;
	inode->i_fop = &ext2_dir_operations;
	if (test_opt(inode->i_sb, NOBH))
		inode->i_mapping->a_ops = &ext2_nobh_aops;
	else
		inode->i_mapping->a_ops = &ext2_aops;

	inode_inc_link_count(inode);

	err = ext2_make_empty(inode, dir);
	if (err)
		goto out_fail;

	err = ext2_add_link(dentry, inode);
	if (err)
		goto out_fail;

	d_instantiate_new(dentry, inode);
out:
	return err;

out_fail:
	inode_dec_link_count(inode);
	inode_dec_link_count(inode);
	unlock_new_inode(inode);
	iput(inode);
out_dir:
	inode_dec_link_count(dir);
	goto out;
}

static int ext2_unlink(struct inode * dir, struct dentry *dentry)
{
<<<<<<< HEAD
	struct inode * inode = dentry->d_inode;
	struct ext2_dir_entry_2 * de;
	struct page * page;
	int err = -ENOENT;

	dquot_initialize(dir);

	de = ext2_find_entry (dir, &dentry->d_name, &page);
	if (!de)
		goto out;
=======
	struct inode * inode = d_inode(dentry);
	struct ext2_dir_entry_2 * de;
	struct page * page;
	int err;

	err = dquot_initialize(dir);
	if (err)
		goto out;

	de = ext2_find_entry (dir, &dentry->d_name, &page);
	if (!de) {
		err = -ENOENT;
		goto out;
	}
>>>>>>> v4.9.227

	err = ext2_delete_entry (de, page);
	if (err)
		goto out;

	inode->i_ctime = dir->i_ctime;
	inode_dec_link_count(inode);
	err = 0;
out:
	return err;
}

static int ext2_rmdir (struct inode * dir, struct dentry *dentry)
{
<<<<<<< HEAD
	struct inode * inode = dentry->d_inode;
=======
	struct inode * inode = d_inode(dentry);
>>>>>>> v4.9.227
	int err = -ENOTEMPTY;

	if (ext2_empty_dir(inode)) {
		err = ext2_unlink(dir, dentry);
		if (!err) {
			inode->i_size = 0;
			inode_dec_link_count(inode);
			inode_dec_link_count(dir);
		}
	}
	return err;
}

static int ext2_rename (struct inode * old_dir, struct dentry * old_dentry,
<<<<<<< HEAD
	struct inode * new_dir,	struct dentry * new_dentry )
{
	struct inode * old_inode = old_dentry->d_inode;
	struct inode * new_inode = new_dentry->d_inode;
=======
			struct inode * new_dir,	struct dentry * new_dentry,
			unsigned int flags)
{
	struct inode * old_inode = d_inode(old_dentry);
	struct inode * new_inode = d_inode(new_dentry);
>>>>>>> v4.9.227
	struct page * dir_page = NULL;
	struct ext2_dir_entry_2 * dir_de = NULL;
	struct page * old_page;
	struct ext2_dir_entry_2 * old_de;
<<<<<<< HEAD
	int err = -ENOENT;

	dquot_initialize(old_dir);
	dquot_initialize(new_dir);

	old_de = ext2_find_entry (old_dir, &old_dentry->d_name, &old_page);
	if (!old_de)
		goto out;
=======
	int err;

	if (flags & ~RENAME_NOREPLACE)
		return -EINVAL;

	err = dquot_initialize(old_dir);
	if (err)
		goto out;

	err = dquot_initialize(new_dir);
	if (err)
		goto out;

	old_de = ext2_find_entry (old_dir, &old_dentry->d_name, &old_page);
	if (!old_de) {
		err = -ENOENT;
		goto out;
	}
>>>>>>> v4.9.227

	if (S_ISDIR(old_inode->i_mode)) {
		err = -EIO;
		dir_de = ext2_dotdot(old_inode, &dir_page);
		if (!dir_de)
			goto out_old;
	}

	if (new_inode) {
		struct page *new_page;
		struct ext2_dir_entry_2 *new_de;

		err = -ENOTEMPTY;
		if (dir_de && !ext2_empty_dir (new_inode))
			goto out_dir;

		err = -ENOENT;
		new_de = ext2_find_entry (new_dir, &new_dentry->d_name, &new_page);
		if (!new_de)
			goto out_dir;
		ext2_set_link(new_dir, new_de, new_page, old_inode, 1);
<<<<<<< HEAD
		new_inode->i_ctime = CURRENT_TIME_SEC;
=======
		new_inode->i_ctime = current_time(new_inode);
>>>>>>> v4.9.227
		if (dir_de)
			drop_nlink(new_inode);
		inode_dec_link_count(new_inode);
	} else {
		err = ext2_add_link(new_dentry, old_inode);
		if (err)
			goto out_dir;
		if (dir_de)
			inode_inc_link_count(new_dir);
	}

	/*
	 * Like most other Unix systems, set the ctime for inodes on a
 	 * rename.
	 */
<<<<<<< HEAD
	old_inode->i_ctime = CURRENT_TIME_SEC;
=======
	old_inode->i_ctime = current_time(old_inode);
>>>>>>> v4.9.227
	mark_inode_dirty(old_inode);

	ext2_delete_entry (old_de, old_page);

	if (dir_de) {
		if (old_dir != new_dir)
			ext2_set_link(old_inode, dir_de, dir_page, new_dir, 0);
		else {
			kunmap(dir_page);
<<<<<<< HEAD
			page_cache_release(dir_page);
=======
			put_page(dir_page);
>>>>>>> v4.9.227
		}
		inode_dec_link_count(old_dir);
	}
	return 0;


out_dir:
	if (dir_de) {
		kunmap(dir_page);
<<<<<<< HEAD
		page_cache_release(dir_page);
	}
out_old:
	kunmap(old_page);
	page_cache_release(old_page);
=======
		put_page(dir_page);
	}
out_old:
	kunmap(old_page);
	put_page(old_page);
>>>>>>> v4.9.227
out:
	return err;
}

const struct inode_operations ext2_dir_inode_operations = {
	.create		= ext2_create,
	.lookup		= ext2_lookup,
	.link		= ext2_link,
	.unlink		= ext2_unlink,
	.symlink	= ext2_symlink,
	.mkdir		= ext2_mkdir,
	.rmdir		= ext2_rmdir,
	.mknod		= ext2_mknod,
	.rename		= ext2_rename,
#ifdef CONFIG_EXT2_FS_XATTR
<<<<<<< HEAD
	.setxattr	= generic_setxattr,
	.getxattr	= generic_getxattr,
	.listxattr	= ext2_listxattr,
	.removexattr	= generic_removexattr,
=======
	.listxattr	= ext2_listxattr,
>>>>>>> v4.9.227
#endif
	.setattr	= ext2_setattr,
	.get_acl	= ext2_get_acl,
	.set_acl	= ext2_set_acl,
	.tmpfile	= ext2_tmpfile,
};

const struct inode_operations ext2_special_inode_operations = {
#ifdef CONFIG_EXT2_FS_XATTR
<<<<<<< HEAD
	.setxattr	= generic_setxattr,
	.getxattr	= generic_getxattr,
	.listxattr	= ext2_listxattr,
	.removexattr	= generic_removexattr,
=======
	.listxattr	= ext2_listxattr,
>>>>>>> v4.9.227
#endif
	.setattr	= ext2_setattr,
	.get_acl	= ext2_get_acl,
	.set_acl	= ext2_set_acl,
};
