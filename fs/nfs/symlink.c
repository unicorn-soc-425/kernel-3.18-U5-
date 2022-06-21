/*
 *  linux/fs/nfs/symlink.c
 *
 *  Copyright (C) 1992  Rick Sladkey
 *
 *  Optimization changes Copyright (C) 1994 Florian La Roche
 *
 *  Jun 7 1999, cache symlink lookups in the page cache.  -DaveM
 *
 *  nfs symlink handling code
 */

#include <linux/time.h>
#include <linux/errno.h>
#include <linux/sunrpc/clnt.h>
#include <linux/nfs.h>
#include <linux/nfs2.h>
#include <linux/nfs_fs.h>
#include <linux/pagemap.h>
#include <linux/stat.h>
#include <linux/mm.h>
#include <linux/string.h>
<<<<<<< HEAD
#include <linux/namei.h>
=======
>>>>>>> v4.9.227

/* Symlink caching in the page cache is even more simplistic
 * and straight-forward than readdir caching.
 */

static int nfs_symlink_filler(struct inode *inode, struct page *page)
{
	int error;

	error = NFS_PROTO(inode)->readlink(inode, page, 0, PAGE_SIZE);
	if (error < 0)
		goto error;
	SetPageUptodate(page);
	unlock_page(page);
	return 0;

error:
	SetPageError(page);
	unlock_page(page);
	return -EIO;
}

<<<<<<< HEAD
static void *nfs_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	struct inode *inode = dentry->d_inode;
	struct page *page;
	void *err;

	err = ERR_PTR(nfs_revalidate_mapping(inode, inode->i_mapping));
	if (err)
		goto read_failed;
	page = read_cache_page(&inode->i_data, 0,
				(filler_t *)nfs_symlink_filler, inode);
	if (IS_ERR(page)) {
		err = page;
		goto read_failed;
	}
	nd_set_link(nd, kmap(page));
	return page;

read_failed:
	nd_set_link(nd, err);
	return NULL;
=======
static const char *nfs_get_link(struct dentry *dentry,
				struct inode *inode,
				struct delayed_call *done)
{
	struct page *page;
	void *err;

	if (!dentry) {
		err = ERR_PTR(nfs_revalidate_mapping_rcu(inode));
		if (err)
			return err;
		page = find_get_page(inode->i_mapping, 0);
		if (!page)
			return ERR_PTR(-ECHILD);
		if (!PageUptodate(page)) {
			put_page(page);
			return ERR_PTR(-ECHILD);
		}
	} else {
		err = ERR_PTR(nfs_revalidate_mapping(inode, inode->i_mapping));
		if (err)
			return err;
		page = read_cache_page(&inode->i_data, 0,
					(filler_t *)nfs_symlink_filler, inode);
		if (IS_ERR(page))
			return ERR_CAST(page);
	}
	set_delayed_call(done, page_put_link, page);
	return page_address(page);
>>>>>>> v4.9.227
}

/*
 * symlinks can't do much...
 */
const struct inode_operations nfs_symlink_inode_operations = {
	.readlink	= generic_readlink,
<<<<<<< HEAD
	.follow_link	= nfs_follow_link,
	.put_link	= page_put_link,
=======
	.get_link	= nfs_get_link,
>>>>>>> v4.9.227
	.getattr	= nfs_getattr,
	.setattr	= nfs_setattr,
};
