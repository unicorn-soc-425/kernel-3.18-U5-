/*
 *   Copyright (C) Christoph Hellwig, 2001-2002
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <linux/fs.h>
<<<<<<< HEAD
#include <linux/namei.h>
=======
>>>>>>> v4.9.227
#include "jfs_incore.h"
#include "jfs_inode.h"
#include "jfs_xattr.h"

<<<<<<< HEAD
static void *jfs_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	char *s = JFS_IP(dentry->d_inode)->i_inline;
	nd_set_link(nd, s);
	return NULL;
}

const struct inode_operations jfs_fast_symlink_inode_operations = {
	.readlink	= generic_readlink,
	.follow_link	= jfs_follow_link,
	.setattr	= jfs_setattr,
	.setxattr	= jfs_setxattr,
	.getxattr	= jfs_getxattr,
	.listxattr	= jfs_listxattr,
	.removexattr	= jfs_removexattr,
=======
const struct inode_operations jfs_fast_symlink_inode_operations = {
	.readlink	= generic_readlink,
	.get_link	= simple_get_link,
	.setattr	= jfs_setattr,
	.listxattr	= jfs_listxattr,
>>>>>>> v4.9.227
};

const struct inode_operations jfs_symlink_inode_operations = {
	.readlink	= generic_readlink,
<<<<<<< HEAD
	.follow_link	= page_follow_link_light,
	.put_link	= page_put_link,
	.setattr	= jfs_setattr,
	.setxattr	= jfs_setxattr,
	.getxattr	= jfs_getxattr,
	.listxattr	= jfs_listxattr,
	.removexattr	= jfs_removexattr,
=======
	.get_link	= page_get_link,
	.setattr	= jfs_setattr,
	.listxattr	= jfs_listxattr,
>>>>>>> v4.9.227
};

