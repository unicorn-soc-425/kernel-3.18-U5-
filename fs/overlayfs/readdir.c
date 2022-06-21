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
#include <linux/namei.h>
#include <linux/file.h>
#include <linux/xattr.h>
#include <linux/rbtree.h>
#include <linux/security.h>
#include <linux/cred.h>
#include "overlayfs.h"

struct ovl_cache_entry {
	unsigned int len;
	unsigned int type;
	u64 ino;
	struct list_head l_node;
	struct rb_node node;
<<<<<<< HEAD
	bool is_whiteout;
	bool is_cursor;
=======
	struct ovl_cache_entry *next_maybe_whiteout;
	bool is_whiteout;
>>>>>>> v4.9.227
	char name[];
};

struct ovl_dir_cache {
	long refcount;
	u64 version;
	struct list_head entries;
};

struct ovl_readdir_data {
	struct dir_context ctx;
<<<<<<< HEAD
	bool is_merge;
	struct rb_root root;
	struct list_head *list;
	struct list_head middle;
	int count;
	int err;
=======
	struct dentry *dentry;
	bool is_lowest;
	struct rb_root root;
	struct list_head *list;
	struct list_head middle;
	struct ovl_cache_entry *first_maybe_whiteout;
	int count;
	int err;
	bool d_type_supported;
>>>>>>> v4.9.227
};

struct ovl_dir_file {
	bool is_real;
	bool is_upper;
	struct ovl_dir_cache *cache;
<<<<<<< HEAD
	struct ovl_cache_entry cursor;
=======
	struct list_head *cursor;
>>>>>>> v4.9.227
	struct file *realfile;
	struct file *upperfile;
};

static struct ovl_cache_entry *ovl_cache_entry_from_node(struct rb_node *n)
{
	return container_of(n, struct ovl_cache_entry, node);
}

static struct ovl_cache_entry *ovl_cache_entry_find(struct rb_root *root,
						    const char *name, int len)
{
	struct rb_node *node = root->rb_node;
	int cmp;

	while (node) {
		struct ovl_cache_entry *p = ovl_cache_entry_from_node(node);

		cmp = strncmp(name, p->name, len);
		if (cmp > 0)
			node = p->node.rb_right;
		else if (cmp < 0 || len < p->len)
			node = p->node.rb_left;
		else
			return p;
	}

	return NULL;
}

<<<<<<< HEAD
static struct ovl_cache_entry *ovl_cache_entry_new(const char *name, int len,
=======
static struct ovl_cache_entry *ovl_cache_entry_new(struct ovl_readdir_data *rdd,
						   const char *name, int len,
>>>>>>> v4.9.227
						   u64 ino, unsigned int d_type)
{
	struct ovl_cache_entry *p;
	size_t size = offsetof(struct ovl_cache_entry, name[len + 1]);

	p = kmalloc(size, GFP_KERNEL);
<<<<<<< HEAD
	if (p) {
		memcpy(p->name, name, len);
		p->name[len] = '\0';
		p->len = len;
		p->type = d_type;
		p->ino = ino;
		p->is_whiteout = false;
		p->is_cursor = false;
	}

=======
	if (!p)
		return NULL;

	memcpy(p->name, name, len);
	p->name[len] = '\0';
	p->len = len;
	p->type = d_type;
	p->ino = ino;
	p->is_whiteout = false;

	if (d_type == DT_CHR) {
		p->next_maybe_whiteout = rdd->first_maybe_whiteout;
		rdd->first_maybe_whiteout = p;
	}
>>>>>>> v4.9.227
	return p;
}

static int ovl_cache_entry_add_rb(struct ovl_readdir_data *rdd,
				  const char *name, int len, u64 ino,
				  unsigned int d_type)
{
	struct rb_node **newp = &rdd->root.rb_node;
	struct rb_node *parent = NULL;
	struct ovl_cache_entry *p;

	while (*newp) {
		int cmp;
		struct ovl_cache_entry *tmp;

		parent = *newp;
		tmp = ovl_cache_entry_from_node(*newp);
		cmp = strncmp(name, tmp->name, len);
		if (cmp > 0)
			newp = &tmp->node.rb_right;
		else if (cmp < 0 || len < tmp->len)
			newp = &tmp->node.rb_left;
		else
			return 0;
	}

<<<<<<< HEAD
	p = ovl_cache_entry_new(name, len, ino, d_type);
=======
	p = ovl_cache_entry_new(rdd, name, len, ino, d_type);
>>>>>>> v4.9.227
	if (p == NULL)
		return -ENOMEM;

	list_add_tail(&p->l_node, rdd->list);
	rb_link_node(&p->node, parent, newp);
	rb_insert_color(&p->node, &rdd->root);

	return 0;
}

<<<<<<< HEAD
static int ovl_fill_lower(struct ovl_readdir_data *rdd,
			  const char *name, int namelen,
			  loff_t offset, u64 ino, unsigned int d_type)
=======
static int ovl_fill_lowest(struct ovl_readdir_data *rdd,
			   const char *name, int namelen,
			   loff_t offset, u64 ino, unsigned int d_type)
>>>>>>> v4.9.227
{
	struct ovl_cache_entry *p;

	p = ovl_cache_entry_find(&rdd->root, name, namelen);
	if (p) {
		list_move_tail(&p->l_node, &rdd->middle);
	} else {
<<<<<<< HEAD
		p = ovl_cache_entry_new(name, namelen, ino, d_type);
=======
		p = ovl_cache_entry_new(rdd, name, namelen, ino, d_type);
>>>>>>> v4.9.227
		if (p == NULL)
			rdd->err = -ENOMEM;
		else
			list_add_tail(&p->l_node, &rdd->middle);
	}

	return rdd->err;
}

void ovl_cache_free(struct list_head *list)
{
	struct ovl_cache_entry *p;
	struct ovl_cache_entry *n;

	list_for_each_entry_safe(p, n, list, l_node)
		kfree(p);

	INIT_LIST_HEAD(list);
}

static void ovl_cache_put(struct ovl_dir_file *od, struct dentry *dentry)
{
	struct ovl_dir_cache *cache = od->cache;

<<<<<<< HEAD
	list_del_init(&od->cursor.l_node);
=======
>>>>>>> v4.9.227
	WARN_ON(cache->refcount <= 0);
	cache->refcount--;
	if (!cache->refcount) {
		if (ovl_dir_cache(dentry) == cache)
			ovl_set_dir_cache(dentry, NULL);

		ovl_cache_free(&cache->entries);
		kfree(cache);
	}
}

<<<<<<< HEAD
static int ovl_fill_merge(void *buf, const char *name, int namelen,
			  loff_t offset, u64 ino, unsigned int d_type)
{
	struct ovl_readdir_data *rdd = buf;

	rdd->count++;
	if (!rdd->is_merge)
		return ovl_cache_entry_add_rb(rdd, name, namelen, ino, d_type);
	else
		return ovl_fill_lower(rdd, name, namelen, offset, ino, d_type);
=======
static int ovl_fill_merge(struct dir_context *ctx, const char *name,
			  int namelen, loff_t offset, u64 ino,
			  unsigned int d_type)
{
	struct ovl_readdir_data *rdd =
		container_of(ctx, struct ovl_readdir_data, ctx);

	rdd->count++;
	if (!rdd->is_lowest)
		return ovl_cache_entry_add_rb(rdd, name, namelen, ino, d_type);
	else
		return ovl_fill_lowest(rdd, name, namelen, offset, ino, d_type);
}

static int ovl_check_whiteouts(struct dentry *dir, struct ovl_readdir_data *rdd)
{
	int err;
	struct ovl_cache_entry *p;
	struct dentry *dentry;
	const struct cred *old_cred;

	old_cred = ovl_override_creds(rdd->dentry->d_sb);

	err = down_write_killable(&dir->d_inode->i_rwsem);
	if (!err) {
		while (rdd->first_maybe_whiteout) {
			p = rdd->first_maybe_whiteout;
			rdd->first_maybe_whiteout = p->next_maybe_whiteout;
			dentry = lookup_one_len(p->name, dir, p->len);
			if (!IS_ERR(dentry)) {
				p->is_whiteout = ovl_is_whiteout(dentry);
				dput(dentry);
			}
		}
		inode_unlock(dir->d_inode);
	}
	revert_creds(old_cred);

	return err;
>>>>>>> v4.9.227
}

static inline int ovl_dir_read(struct path *realpath,
			       struct ovl_readdir_data *rdd)
{
	struct file *realfile;
	int err;

	realfile = ovl_path_open(realpath, O_RDONLY | O_DIRECTORY);
	if (IS_ERR(realfile))
		return PTR_ERR(realfile);

<<<<<<< HEAD
=======
	rdd->first_maybe_whiteout = NULL;
>>>>>>> v4.9.227
	rdd->ctx.pos = 0;
	do {
		rdd->count = 0;
		rdd->err = 0;
		err = iterate_dir(realfile, &rdd->ctx);
		if (err >= 0)
			err = rdd->err;
	} while (!err && rdd->count);
<<<<<<< HEAD
=======

	if (!err && rdd->first_maybe_whiteout && rdd->dentry)
		err = ovl_check_whiteouts(realpath->dentry, rdd);

>>>>>>> v4.9.227
	fput(realfile);

	return err;
}

static void ovl_dir_reset(struct file *file)
{
	struct ovl_dir_file *od = file->private_data;
	struct ovl_dir_cache *cache = od->cache;
	struct dentry *dentry = file->f_path.dentry;
	enum ovl_path_type type = ovl_path_type(dentry);

	if (cache && ovl_dentry_version_get(dentry) != cache->version) {
		ovl_cache_put(od, dentry);
		od->cache = NULL;
<<<<<<< HEAD
	}
	WARN_ON(!od->is_real && type != OVL_PATH_MERGE);
	if (od->is_real && type == OVL_PATH_MERGE)
		od->is_real = false;
}

static int ovl_dir_mark_whiteouts(struct dentry *dir,
				  struct ovl_readdir_data *rdd)
{
	struct ovl_cache_entry *p;
	struct dentry *dentry;
	const struct cred *old_cred;
	struct cred *override_cred;

	override_cred = prepare_creds();
	if (!override_cred) {
		ovl_cache_free(rdd->list);
		return -ENOMEM;
	}

	/*
	 * CAP_DAC_OVERRIDE for lookup
	 */
	cap_raise(override_cred->cap_effective, CAP_DAC_OVERRIDE);
	old_cred = override_creds(override_cred);

	mutex_lock(&dir->d_inode->i_mutex);
	list_for_each_entry(p, rdd->list, l_node) {
		if (p->is_cursor)
			continue;

		if (p->type != DT_CHR)
			continue;

		dentry = lookup_one_len(p->name, dir, p->len);
		if (IS_ERR(dentry))
			continue;

		p->is_whiteout = ovl_is_whiteout(dentry);
		dput(dentry);
	}
	mutex_unlock(&dir->d_inode->i_mutex);

	revert_creds(old_cred);
	put_cred(override_cred);

	return 0;
}

static int ovl_dir_read_merged(struct dentry *dentry, struct list_head *list)
{
	int err;
	struct path lowerpath;
	struct path upperpath;
	struct ovl_readdir_data rdd = {
		.ctx.actor = ovl_fill_merge,
		.list = list,
		.root = RB_ROOT,
		.is_merge = false,
	};

	ovl_path_lower(dentry, &lowerpath);
	ovl_path_upper(dentry, &upperpath);

	if (upperpath.dentry) {
		err = ovl_dir_read(&upperpath, &rdd);
		if (err)
			goto out;

		if (lowerpath.dentry) {
			err = ovl_dir_mark_whiteouts(upperpath.dentry, &rdd);
			if (err)
				goto out;
		}
	}
	if (lowerpath.dentry) {
		/*
		 * Insert lowerpath entries before upperpath ones, this allows
		 * offsets to be reasonably constant
		 */
		list_add(&rdd.middle, rdd.list);
		rdd.is_merge = true;
		err = ovl_dir_read(&lowerpath, &rdd);
		list_del(&rdd.middle);
	}
out:
=======
		od->cursor = NULL;
	}
	WARN_ON(!od->is_real && !OVL_TYPE_MERGE(type));
	if (od->is_real && OVL_TYPE_MERGE(type))
		od->is_real = false;
}

static int ovl_dir_read_merged(struct dentry *dentry, struct list_head *list)
{
	int err;
	struct path realpath;
	struct ovl_readdir_data rdd = {
		.ctx.actor = ovl_fill_merge,
		.dentry = dentry,
		.list = list,
		.root = RB_ROOT,
		.is_lowest = false,
	};
	int idx, next;

	for (idx = 0; idx != -1; idx = next) {
		next = ovl_path_next(idx, dentry, &realpath);

		if (next != -1) {
			err = ovl_dir_read(&realpath, &rdd);
			if (err)
				break;
		} else {
			/*
			 * Insert lowest layer entries before upper ones, this
			 * allows offsets to be reasonably constant
			 */
			list_add(&rdd.middle, rdd.list);
			rdd.is_lowest = true;
			err = ovl_dir_read(&realpath, &rdd);
			list_del(&rdd.middle);
		}
	}
>>>>>>> v4.9.227
	return err;
}

static void ovl_seek_cursor(struct ovl_dir_file *od, loff_t pos)
{
<<<<<<< HEAD
	struct ovl_cache_entry *p;
	loff_t off = 0;

	list_for_each_entry(p, &od->cache->entries, l_node) {
		if (p->is_cursor)
			continue;
=======
	struct list_head *p;
	loff_t off = 0;

	list_for_each(p, &od->cache->entries) {
>>>>>>> v4.9.227
		if (off >= pos)
			break;
		off++;
	}
<<<<<<< HEAD
	list_move_tail(&od->cursor.l_node, &p->l_node);
=======
	/* Cursor is safe since the cache is stable */
	od->cursor = p;
>>>>>>> v4.9.227
}

static struct ovl_dir_cache *ovl_cache_get(struct dentry *dentry)
{
	int res;
	struct ovl_dir_cache *cache;

	cache = ovl_dir_cache(dentry);
	if (cache && ovl_dentry_version_get(dentry) == cache->version) {
		cache->refcount++;
		return cache;
	}
	ovl_set_dir_cache(dentry, NULL);

	cache = kzalloc(sizeof(struct ovl_dir_cache), GFP_KERNEL);
	if (!cache)
		return ERR_PTR(-ENOMEM);

	cache->refcount = 1;
	INIT_LIST_HEAD(&cache->entries);

	res = ovl_dir_read_merged(dentry, &cache->entries);
	if (res) {
		ovl_cache_free(&cache->entries);
		kfree(cache);
		return ERR_PTR(res);
	}

	cache->version = ovl_dentry_version_get(dentry);
	ovl_set_dir_cache(dentry, cache);

	return cache;
}

static int ovl_iterate(struct file *file, struct dir_context *ctx)
{
	struct ovl_dir_file *od = file->private_data;
	struct dentry *dentry = file->f_path.dentry;
<<<<<<< HEAD
=======
	struct ovl_cache_entry *p;
>>>>>>> v4.9.227

	if (!ctx->pos)
		ovl_dir_reset(file);

	if (od->is_real)
		return iterate_dir(od->realfile, ctx);

	if (!od->cache) {
		struct ovl_dir_cache *cache;

		cache = ovl_cache_get(dentry);
		if (IS_ERR(cache))
			return PTR_ERR(cache);

		od->cache = cache;
		ovl_seek_cursor(od, ctx->pos);
	}

<<<<<<< HEAD
	while (od->cursor.l_node.next != &od->cache->entries) {
		struct ovl_cache_entry *p;

		p = list_entry(od->cursor.l_node.next, struct ovl_cache_entry, l_node);
		/* Skip cursors */
		if (!p->is_cursor) {
			if (!p->is_whiteout) {
				if (!dir_emit(ctx, p->name, p->len, p->ino, p->type))
					break;
			}
			ctx->pos++;
		}
		list_move(&od->cursor.l_node, &p->l_node);
=======
	while (od->cursor != &od->cache->entries) {
		p = list_entry(od->cursor, struct ovl_cache_entry, l_node);
		if (!p->is_whiteout)
			if (!dir_emit(ctx, p->name, p->len, p->ino, p->type))
				break;
		od->cursor = p->l_node.next;
		ctx->pos++;
>>>>>>> v4.9.227
	}
	return 0;
}

static loff_t ovl_dir_llseek(struct file *file, loff_t offset, int origin)
{
	loff_t res;
	struct ovl_dir_file *od = file->private_data;

<<<<<<< HEAD
	mutex_lock(&file_inode(file)->i_mutex);
=======
	inode_lock(file_inode(file));
>>>>>>> v4.9.227
	if (!file->f_pos)
		ovl_dir_reset(file);

	if (od->is_real) {
		res = vfs_llseek(od->realfile, offset, origin);
		file->f_pos = od->realfile->f_pos;
	} else {
		res = -EINVAL;

		switch (origin) {
		case SEEK_CUR:
			offset += file->f_pos;
			break;
		case SEEK_SET:
			break;
		default:
			goto out_unlock;
		}
		if (offset < 0)
			goto out_unlock;

		if (offset != file->f_pos) {
			file->f_pos = offset;
			if (od->cache)
				ovl_seek_cursor(od, offset);
		}
		res = offset;
	}
out_unlock:
<<<<<<< HEAD
	mutex_unlock(&file_inode(file)->i_mutex);
=======
	inode_unlock(file_inode(file));
>>>>>>> v4.9.227

	return res;
}

static int ovl_dir_fsync(struct file *file, loff_t start, loff_t end,
			 int datasync)
{
	struct ovl_dir_file *od = file->private_data;
	struct dentry *dentry = file->f_path.dentry;
	struct file *realfile = od->realfile;

<<<<<<< HEAD
	/*
	 * Need to check if we started out being a lower dir, but got copied up
	 */
	if (!od->is_upper && ovl_path_type(dentry) != OVL_PATH_LOWER) {
=======
	/* Nothing to sync for lower */
	if (!OVL_TYPE_UPPER(ovl_path_type(dentry)))
		return 0;

	/*
	 * Need to check if we started out being a lower dir, but got copied up
	 */
	if (!od->is_upper) {
>>>>>>> v4.9.227
		struct inode *inode = file_inode(file);

		realfile = lockless_dereference(od->upperfile);
		if (!realfile) {
			struct path upperpath;

			ovl_path_upper(dentry, &upperpath);
			realfile = ovl_path_open(&upperpath, O_RDONLY);
			smp_mb__before_spinlock();
<<<<<<< HEAD
			mutex_lock(&inode->i_mutex);
			if (!od->upperfile) {
				if (IS_ERR(realfile)) {
					mutex_unlock(&inode->i_mutex);
=======
			inode_lock(inode);
			if (!od->upperfile) {
				if (IS_ERR(realfile)) {
					inode_unlock(inode);
>>>>>>> v4.9.227
					return PTR_ERR(realfile);
				}
				od->upperfile = realfile;
			} else {
				/* somebody has beaten us to it */
				if (!IS_ERR(realfile))
					fput(realfile);
				realfile = od->upperfile;
			}
<<<<<<< HEAD
			mutex_unlock(&inode->i_mutex);
=======
			inode_unlock(inode);
>>>>>>> v4.9.227
		}
	}

	return vfs_fsync_range(realfile, start, end, datasync);
}

static int ovl_dir_release(struct inode *inode, struct file *file)
{
	struct ovl_dir_file *od = file->private_data;

	if (od->cache) {
<<<<<<< HEAD
		mutex_lock(&inode->i_mutex);
		ovl_cache_put(od, file->f_path.dentry);
		mutex_unlock(&inode->i_mutex);
=======
		inode_lock(inode);
		ovl_cache_put(od, file->f_path.dentry);
		inode_unlock(inode);
>>>>>>> v4.9.227
	}
	fput(od->realfile);
	if (od->upperfile)
		fput(od->upperfile);
	kfree(od);

	return 0;
}

static int ovl_dir_open(struct inode *inode, struct file *file)
{
	struct path realpath;
	struct file *realfile;
	struct ovl_dir_file *od;
	enum ovl_path_type type;

	od = kzalloc(sizeof(struct ovl_dir_file), GFP_KERNEL);
	if (!od)
		return -ENOMEM;

	type = ovl_path_real(file->f_path.dentry, &realpath);
	realfile = ovl_path_open(&realpath, file->f_flags);
	if (IS_ERR(realfile)) {
		kfree(od);
		return PTR_ERR(realfile);
	}
<<<<<<< HEAD
	INIT_LIST_HEAD(&od->cursor.l_node);
	od->realfile = realfile;
	od->is_real = (type != OVL_PATH_MERGE);
	od->is_upper = (type != OVL_PATH_LOWER);
	od->cursor.is_cursor = true;
=======
	od->realfile = realfile;
	od->is_real = !OVL_TYPE_MERGE(type);
	od->is_upper = OVL_TYPE_UPPER(type);
>>>>>>> v4.9.227
	file->private_data = od;

	return 0;
}

const struct file_operations ovl_dir_operations = {
	.read		= generic_read_dir,
	.open		= ovl_dir_open,
	.iterate	= ovl_iterate,
	.llseek		= ovl_dir_llseek,
	.fsync		= ovl_dir_fsync,
	.release	= ovl_dir_release,
};

int ovl_check_empty_dir(struct dentry *dentry, struct list_head *list)
{
	int err;
	struct ovl_cache_entry *p;

	err = ovl_dir_read_merged(dentry, list);
	if (err)
		return err;

	err = 0;

	list_for_each_entry(p, list, l_node) {
		if (p->is_whiteout)
			continue;

		if (p->name[0] == '.') {
			if (p->len == 1)
				continue;
			if (p->len == 2 && p->name[1] == '.')
				continue;
		}
		err = -ENOTEMPTY;
		break;
	}

	return err;
}

void ovl_cleanup_whiteouts(struct dentry *upper, struct list_head *list)
{
	struct ovl_cache_entry *p;

<<<<<<< HEAD
	mutex_lock_nested(&upper->d_inode->i_mutex, I_MUTEX_CHILD);
=======
	inode_lock_nested(upper->d_inode, I_MUTEX_CHILD);
>>>>>>> v4.9.227
	list_for_each_entry(p, list, l_node) {
		struct dentry *dentry;

		if (!p->is_whiteout)
			continue;

		dentry = lookup_one_len(p->name, upper, p->len);
		if (IS_ERR(dentry)) {
			pr_err("overlayfs: lookup '%s/%.*s' failed (%i)\n",
			       upper->d_name.name, p->len, p->name,
			       (int) PTR_ERR(dentry));
			continue;
		}
		if (dentry->d_inode)
			ovl_cleanup(upper->d_inode, dentry);
		dput(dentry);
	}
<<<<<<< HEAD
	mutex_unlock(&upper->d_inode->i_mutex);
=======
	inode_unlock(upper->d_inode);
}

static int ovl_check_d_type(struct dir_context *ctx, const char *name,
			  int namelen, loff_t offset, u64 ino,
			  unsigned int d_type)
{
	struct ovl_readdir_data *rdd =
		container_of(ctx, struct ovl_readdir_data, ctx);

	/* Even if d_type is not supported, DT_DIR is returned for . and .. */
	if (!strncmp(name, ".", namelen) || !strncmp(name, "..", namelen))
		return 0;

	if (d_type != DT_UNKNOWN)
		rdd->d_type_supported = true;

	return 0;
}

/*
 * Returns 1 if d_type is supported, 0 not supported/unknown. Negative values
 * if error is encountered.
 */
int ovl_check_d_type_supported(struct path *realpath)
{
	int err;
	struct ovl_readdir_data rdd = {
		.ctx.actor = ovl_check_d_type,
		.d_type_supported = false,
	};

	err = ovl_dir_read(realpath, &rdd);
	if (err)
		return err;

	return rdd.d_type_supported;
}

static void ovl_workdir_cleanup_recurse(struct path *path, int level)
{
	int err;
	struct inode *dir = path->dentry->d_inode;
	LIST_HEAD(list);
	struct ovl_cache_entry *p;
	struct ovl_readdir_data rdd = {
		.ctx.actor = ovl_fill_merge,
		.dentry = NULL,
		.list = &list,
		.root = RB_ROOT,
		.is_lowest = false,
	};

	err = ovl_dir_read(path, &rdd);
	if (err)
		goto out;

	inode_lock_nested(dir, I_MUTEX_PARENT);
	list_for_each_entry(p, &list, l_node) {
		struct dentry *dentry;

		if (p->name[0] == '.') {
			if (p->len == 1)
				continue;
			if (p->len == 2 && p->name[1] == '.')
				continue;
		}
		dentry = lookup_one_len(p->name, path->dentry, p->len);
		if (IS_ERR(dentry))
			continue;
		if (dentry->d_inode)
			ovl_workdir_cleanup(dir, path->mnt, dentry, level);
		dput(dentry);
	}
	inode_unlock(dir);
out:
	ovl_cache_free(&list);
}

void ovl_workdir_cleanup(struct inode *dir, struct vfsmount *mnt,
			 struct dentry *dentry, int level)
{
	int err;

	if (!d_is_dir(dentry) || level > 1) {
		ovl_cleanup(dir, dentry);
		return;
	}

	err = ovl_do_rmdir(dir, dentry);
	if (err) {
		struct path path = { .mnt = mnt, .dentry = dentry };

		inode_unlock(dir);
		ovl_workdir_cleanup_recurse(&path, level + 1);
		inode_lock_nested(dir, I_MUTEX_PARENT);
		ovl_cleanup(dir, dentry);
	}
>>>>>>> v4.9.227
}
