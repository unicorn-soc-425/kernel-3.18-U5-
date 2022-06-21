/*
 *  linux/fs/affs/namei.c
 *
 *  (c) 1996  Hans-Joachim Widmaier - Rewritten
 *
 *  (C) 1993  Ray Burr - Modified for Amiga FFS filesystem.
 *
 *  (C) 1991  Linus Torvalds - minix filesystem
 */

#include "affs.h"

typedef int (*toupper_t)(int);

static int	 affs_toupper(int ch);
static int	 affs_hash_dentry(const struct dentry *, struct qstr *);
<<<<<<< HEAD
static int       affs_compare_dentry(const struct dentry *parent, const struct dentry *dentry,
		unsigned int len, const char *str, const struct qstr *name);
static int	 affs_intl_toupper(int ch);
static int	 affs_intl_hash_dentry(const struct dentry *, struct qstr *);
static int       affs_intl_compare_dentry(const struct dentry *parent, const struct dentry *dentry,
=======
static int       affs_compare_dentry(const struct dentry *dentry,
		unsigned int len, const char *str, const struct qstr *name);
static int	 affs_intl_toupper(int ch);
static int	 affs_intl_hash_dentry(const struct dentry *, struct qstr *);
static int       affs_intl_compare_dentry(const struct dentry *dentry,
>>>>>>> v4.9.227
		unsigned int len, const char *str, const struct qstr *name);

const struct dentry_operations affs_dentry_operations = {
	.d_hash		= affs_hash_dentry,
	.d_compare	= affs_compare_dentry,
};

const struct dentry_operations affs_intl_dentry_operations = {
	.d_hash		= affs_intl_hash_dentry,
	.d_compare	= affs_intl_compare_dentry,
};


/* Simple toupper() for DOS\1 */

static int
affs_toupper(int ch)
{
	return ch >= 'a' && ch <= 'z' ? ch -= ('a' - 'A') : ch;
}

/* International toupper() for DOS\3 ("international") */

static int
affs_intl_toupper(int ch)
{
	return (ch >= 'a' && ch <= 'z') || (ch >= 0xE0
		&& ch <= 0xFE && ch != 0xF7) ?
		ch - ('a' - 'A') : ch;
}

static inline toupper_t
affs_get_toupper(struct super_block *sb)
{
<<<<<<< HEAD
	return AFFS_SB(sb)->s_flags & SF_INTL ? affs_intl_toupper : affs_toupper;
=======
	return affs_test_opt(AFFS_SB(sb)->s_flags, SF_INTL) ?
	       affs_intl_toupper : affs_toupper;
>>>>>>> v4.9.227
}

/*
 * Note: the dentry argument is the parent dentry.
 */
static inline int
<<<<<<< HEAD
__affs_hash_dentry(struct qstr *qstr, toupper_t toupper, bool notruncate)
{
	const u8 *name = qstr->name;
	unsigned long hash;
	int i;

	i = affs_check_name(qstr->name, qstr->len, notruncate);
	if (i)
		return i;

	hash = init_name_hash();
	i = min(qstr->len, 30u);
	for (; i > 0; name++, i--)
=======
__affs_hash_dentry(const struct dentry *dentry, struct qstr *qstr, toupper_t toupper, bool notruncate)
{
	const u8 *name = qstr->name;
	unsigned long hash;
	int retval;
	u32 len;

	retval = affs_check_name(qstr->name, qstr->len, notruncate);
	if (retval)
		return retval;

	hash = init_name_hash(dentry);
	len = min(qstr->len, AFFSNAMEMAX);
	for (; len > 0; name++, len--)
>>>>>>> v4.9.227
		hash = partial_name_hash(toupper(*name), hash);
	qstr->hash = end_name_hash(hash);

	return 0;
}

static int
affs_hash_dentry(const struct dentry *dentry, struct qstr *qstr)
{
<<<<<<< HEAD
	return __affs_hash_dentry(qstr, affs_toupper,
=======
	return __affs_hash_dentry(dentry, qstr, affs_toupper,
>>>>>>> v4.9.227
				  affs_nofilenametruncate(dentry));

}

static int
affs_intl_hash_dentry(const struct dentry *dentry, struct qstr *qstr)
{
<<<<<<< HEAD
	return __affs_hash_dentry(qstr, affs_intl_toupper,
=======
	return __affs_hash_dentry(dentry, qstr, affs_intl_toupper,
>>>>>>> v4.9.227
				  affs_nofilenametruncate(dentry));

}

static inline int __affs_compare_dentry(unsigned int len,
		const char *str, const struct qstr *name, toupper_t toupper,
		bool notruncate)
{
	const u8 *aname = str;
	const u8 *bname = name->name;

	/*
	 * 'str' is the name of an already existing dentry, so the name
	 * must be valid. 'name' must be validated first.
	 */

	if (affs_check_name(name->name, name->len, notruncate))
		return 1;

	/*
	 * If the names are longer than the allowed 30 chars,
	 * the excess is ignored, so their length may differ.
	 */
<<<<<<< HEAD
	if (len >= 30) {
		if (name->len < 30)
			return 1;
		len = 30;
=======
	if (len >= AFFSNAMEMAX) {
		if (name->len < AFFSNAMEMAX)
			return 1;
		len = AFFSNAMEMAX;
>>>>>>> v4.9.227
	} else if (len != name->len)
		return 1;

	for (; len > 0; len--)
		if (toupper(*aname++) != toupper(*bname++))
			return 1;

	return 0;
}

static int
<<<<<<< HEAD
affs_compare_dentry(const struct dentry *parent, const struct dentry *dentry,
=======
affs_compare_dentry(const struct dentry *dentry,
>>>>>>> v4.9.227
		unsigned int len, const char *str, const struct qstr *name)
{

	return __affs_compare_dentry(len, str, name, affs_toupper,
<<<<<<< HEAD
				     affs_nofilenametruncate(parent));
}

static int
affs_intl_compare_dentry(const struct dentry *parent, const struct dentry *dentry,
		unsigned int len, const char *str, const struct qstr *name)
{
	return __affs_compare_dentry(len, str, name, affs_intl_toupper,
				     affs_nofilenametruncate(parent));
=======
				     affs_nofilenametruncate(dentry));
}

static int
affs_intl_compare_dentry(const struct dentry *dentry,
		unsigned int len, const char *str, const struct qstr *name)
{
	return __affs_compare_dentry(len, str, name, affs_intl_toupper,
				     affs_nofilenametruncate(dentry));
>>>>>>> v4.9.227

}

/*
 * NOTE! unlike strncmp, affs_match returns 1 for success, 0 for failure.
 */

static inline int
affs_match(struct dentry *dentry, const u8 *name2, toupper_t toupper)
{
	const u8 *name = dentry->d_name.name;
	int len = dentry->d_name.len;

<<<<<<< HEAD
	if (len >= 30) {
		if (*name2 < 30)
			return 0;
		len = 30;
=======
	if (len >= AFFSNAMEMAX) {
		if (*name2 < AFFSNAMEMAX)
			return 0;
		len = AFFSNAMEMAX;
>>>>>>> v4.9.227
	} else if (len != *name2)
		return 0;

	for (name2++; len > 0; len--)
		if (toupper(*name++) != toupper(*name2++))
			return 0;
	return 1;
}

int
affs_hash_name(struct super_block *sb, const u8 *name, unsigned int len)
{
	toupper_t toupper = affs_get_toupper(sb);
<<<<<<< HEAD
	int hash;

	hash = len = min(len, 30u);
=======
	u32 hash;

	hash = len = min(len, AFFSNAMEMAX);
>>>>>>> v4.9.227
	for (; len > 0; len--)
		hash = (hash * 13 + toupper(*name++)) & 0x7ff;

	return hash % AFFS_SB(sb)->s_hashsize;
}

static struct buffer_head *
affs_find_entry(struct inode *dir, struct dentry *dentry)
{
	struct super_block *sb = dir->i_sb;
	struct buffer_head *bh;
	toupper_t toupper = affs_get_toupper(sb);
	u32 key;

<<<<<<< HEAD
	pr_debug("%s(\"%.*s\")\n",
		 __func__, (int)dentry->d_name.len, dentry->d_name.name);
=======
	pr_debug("%s(\"%pd\")\n", __func__, dentry);
>>>>>>> v4.9.227

	bh = affs_bread(sb, dir->i_ino);
	if (!bh)
		return ERR_PTR(-EIO);

	key = be32_to_cpu(AFFS_HEAD(bh)->table[affs_hash_name(sb, dentry->d_name.name, dentry->d_name.len)]);

	for (;;) {
		affs_brelse(bh);
		if (key == 0)
			return NULL;
		bh = affs_bread(sb, key);
		if (!bh)
			return ERR_PTR(-EIO);
		if (affs_match(dentry, AFFS_TAIL(sb, bh)->name, toupper))
			return bh;
		key = be32_to_cpu(AFFS_TAIL(sb, bh)->hash_chain);
	}
}

struct dentry *
affs_lookup(struct inode *dir, struct dentry *dentry, unsigned int flags)
{
	struct super_block *sb = dir->i_sb;
	struct buffer_head *bh;
	struct inode *inode = NULL;

<<<<<<< HEAD
	pr_debug("%s(\"%.*s\")\n",
		 __func__, (int)dentry->d_name.len, dentry->d_name.name);
=======
	pr_debug("%s(\"%pd\")\n", __func__, dentry);
>>>>>>> v4.9.227

	affs_lock_dir(dir);
	bh = affs_find_entry(dir, dentry);
	if (IS_ERR(bh)) {
		affs_unlock_dir(dir);
		return ERR_CAST(bh);
	}
	if (bh) {
		u32 ino = bh->b_blocknr;

		/* store the real header ino in d_fsdata for faster lookups */
		dentry->d_fsdata = (void *)(long)ino;
		switch (be32_to_cpu(AFFS_TAIL(sb, bh)->stype)) {
		//link to dirs disabled
		//case ST_LINKDIR:
		case ST_LINKFILE:
			ino = be32_to_cpu(AFFS_TAIL(sb, bh)->original);
		}
		affs_brelse(bh);
		inode = affs_iget(sb, ino);
		if (IS_ERR(inode)) {
			affs_unlock_dir(dir);
			return ERR_CAST(inode);
		}
	}
	d_add(dentry, inode);
	affs_unlock_dir(dir);
	return NULL;
}

int
affs_unlink(struct inode *dir, struct dentry *dentry)
{
<<<<<<< HEAD
	pr_debug("%s(dir=%d, %lu \"%.*s\")\n",
		 __func__, (u32)dir->i_ino, dentry->d_inode->i_ino,
		(int)dentry->d_name.len, dentry->d_name.name);
=======
	pr_debug("%s(dir=%lu, %lu \"%pd\")\n", __func__, dir->i_ino,
		 d_inode(dentry)->i_ino, dentry);
>>>>>>> v4.9.227

	return affs_remove_header(dentry);
}

int
affs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl)
{
	struct super_block *sb = dir->i_sb;
	struct inode	*inode;
	int		 error;

<<<<<<< HEAD
	pr_debug("%s(%lu,\"%.*s\",0%ho)\n",
		 __func__, dir->i_ino, (int)dentry->d_name.len,
		 dentry->d_name.name,mode);
=======
	pr_debug("%s(%lu,\"%pd\",0%ho)\n",
		 __func__, dir->i_ino, dentry, mode);
>>>>>>> v4.9.227

	inode = affs_new_inode(dir);
	if (!inode)
		return -ENOSPC;

	inode->i_mode = mode;
	mode_to_prot(inode);
	mark_inode_dirty(inode);

	inode->i_op = &affs_file_inode_operations;
	inode->i_fop = &affs_file_operations;
<<<<<<< HEAD
	inode->i_mapping->a_ops = (AFFS_SB(sb)->s_flags & SF_OFS) ? &affs_aops_ofs : &affs_aops;
=======
	inode->i_mapping->a_ops = affs_test_opt(AFFS_SB(sb)->s_flags, SF_OFS) ?
				  &affs_aops_ofs : &affs_aops;
>>>>>>> v4.9.227
	error = affs_add_entry(dir, inode, dentry, ST_FILE);
	if (error) {
		clear_nlink(inode);
		iput(inode);
		return error;
	}
	return 0;
}

int
affs_mkdir(struct inode *dir, struct dentry *dentry, umode_t mode)
{
	struct inode		*inode;
	int			 error;

<<<<<<< HEAD
	pr_debug("%s(%lu,\"%.*s\",0%ho)\n",
		 __func__, dir->i_ino, (int)dentry->d_name.len,
		 dentry->d_name.name, mode);
=======
	pr_debug("%s(%lu,\"%pd\",0%ho)\n",
		 __func__, dir->i_ino, dentry, mode);
>>>>>>> v4.9.227

	inode = affs_new_inode(dir);
	if (!inode)
		return -ENOSPC;

	inode->i_mode = S_IFDIR | mode;
	mode_to_prot(inode);

	inode->i_op = &affs_dir_inode_operations;
	inode->i_fop = &affs_dir_operations;

	error = affs_add_entry(dir, inode, dentry, ST_USERDIR);
	if (error) {
		clear_nlink(inode);
		mark_inode_dirty(inode);
		iput(inode);
		return error;
	}
	return 0;
}

int
affs_rmdir(struct inode *dir, struct dentry *dentry)
{
<<<<<<< HEAD
	pr_debug("%s(dir=%u, %lu \"%.*s\")\n",
		__func__, (u32)dir->i_ino, dentry->d_inode->i_ino,
		 (int)dentry->d_name.len, dentry->d_name.name);
=======
	pr_debug("%s(dir=%lu, %lu \"%pd\")\n", __func__, dir->i_ino,
		 d_inode(dentry)->i_ino, dentry);
>>>>>>> v4.9.227

	return affs_remove_header(dentry);
}

int
affs_symlink(struct inode *dir, struct dentry *dentry, const char *symname)
{
	struct super_block	*sb = dir->i_sb;
	struct buffer_head	*bh;
	struct inode		*inode;
	char			*p;
	int			 i, maxlen, error;
	char			 c, lc;

<<<<<<< HEAD
	pr_debug("%s(%lu,\"%.*s\" -> \"%s\")\n",
		 __func__, dir->i_ino, (int)dentry->d_name.len,
		 dentry->d_name.name, symname);
=======
	pr_debug("%s(%lu,\"%pd\" -> \"%s\")\n",
		 __func__, dir->i_ino, dentry, symname);
>>>>>>> v4.9.227

	maxlen = AFFS_SB(sb)->s_hashsize * sizeof(u32) - 1;
	inode  = affs_new_inode(dir);
	if (!inode)
		return -ENOSPC;

	inode->i_op = &affs_symlink_inode_operations;
<<<<<<< HEAD
=======
	inode_nohighmem(inode);
>>>>>>> v4.9.227
	inode->i_data.a_ops = &affs_symlink_aops;
	inode->i_mode = S_IFLNK | 0777;
	mode_to_prot(inode);

	error = -EIO;
	bh = affs_bread(sb, inode->i_ino);
	if (!bh)
		goto err;
	i  = 0;
	p  = (char *)AFFS_HEAD(bh)->table;
	lc = '/';
	if (*symname == '/') {
		struct affs_sb_info *sbi = AFFS_SB(sb);
		while (*symname == '/')
			symname++;
		spin_lock(&sbi->symlink_lock);
		while (sbi->s_volume[i])	/* Cannot overflow */
			*p++ = sbi->s_volume[i++];
		spin_unlock(&sbi->symlink_lock);
	}
	while (i < maxlen && (c = *symname++)) {
		if (c == '.' && lc == '/' && *symname == '.' && symname[1] == '/') {
			*p++ = '/';
			i++;
			symname += 2;
			lc = '/';
		} else if (c == '.' && lc == '/' && *symname == '/') {
			symname++;
			lc = '/';
		} else {
			*p++ = c;
			lc   = c;
			i++;
		}
		if (lc == '/')
			while (*symname == '/')
				symname++;
	}
	*p = 0;
	mark_buffer_dirty_inode(bh, inode);
	affs_brelse(bh);
	mark_inode_dirty(inode);

	error = affs_add_entry(dir, inode, dentry, ST_SOFTLINK);
	if (error)
		goto err;

	return 0;

err:
	clear_nlink(inode);
	mark_inode_dirty(inode);
	iput(inode);
	return error;
}

int
affs_link(struct dentry *old_dentry, struct inode *dir, struct dentry *dentry)
{
<<<<<<< HEAD
	struct inode *inode = old_dentry->d_inode;

	pr_debug("%s(%u, %u, \"%.*s\")\n",
		 __func__, (u32)inode->i_ino, (u32)dir->i_ino,
		 (int)dentry->d_name.len,dentry->d_name.name);
=======
	struct inode *inode = d_inode(old_dentry);

	pr_debug("%s(%lu, %lu, \"%pd\")\n", __func__, inode->i_ino, dir->i_ino,
		 dentry);
>>>>>>> v4.9.227

	return affs_add_entry(dir, inode, dentry, ST_LINKFILE);
}

int
affs_rename(struct inode *old_dir, struct dentry *old_dentry,
<<<<<<< HEAD
	    struct inode *new_dir, struct dentry *new_dentry)
=======
	    struct inode *new_dir, struct dentry *new_dentry,
	    unsigned int flags)
>>>>>>> v4.9.227
{
	struct super_block *sb = old_dir->i_sb;
	struct buffer_head *bh = NULL;
	int retval;

<<<<<<< HEAD
	pr_debug("%s(old=%u,\"%*s\" to new=%u,\"%*s\")\n",
		 __func__, (u32)old_dir->i_ino, (int)old_dentry->d_name.len,
		 old_dentry->d_name.name, (u32)new_dir->i_ino,
		(int)new_dentry->d_name.len, new_dentry->d_name.name);
=======
	if (flags & ~RENAME_NOREPLACE)
		return -EINVAL;

	pr_debug("%s(old=%lu,\"%pd\" to new=%lu,\"%pd\")\n", __func__,
		 old_dir->i_ino, old_dentry, new_dir->i_ino, new_dentry);
>>>>>>> v4.9.227

	retval = affs_check_name(new_dentry->d_name.name,
				 new_dentry->d_name.len,
				 affs_nofilenametruncate(old_dentry));

	if (retval)
		return retval;

	/* Unlink destination if it already exists */
<<<<<<< HEAD
	if (new_dentry->d_inode) {
=======
	if (d_really_is_positive(new_dentry)) {
>>>>>>> v4.9.227
		retval = affs_remove_header(new_dentry);
		if (retval)
			return retval;
	}

<<<<<<< HEAD
	bh = affs_bread(sb, old_dentry->d_inode->i_ino);
=======
	bh = affs_bread(sb, d_inode(old_dentry)->i_ino);
>>>>>>> v4.9.227
	if (!bh)
		return -EIO;

	/* Remove header from its parent directory. */
	affs_lock_dir(old_dir);
	retval = affs_remove_hash(old_dir, bh);
	affs_unlock_dir(old_dir);
	if (retval)
		goto done;

	/* And insert it into the new directory with the new name. */
	affs_copy_name(AFFS_TAIL(sb, bh)->name, new_dentry);
	affs_fix_checksum(sb, bh);
	affs_lock_dir(new_dir);
	retval = affs_insert_hash(new_dir, bh);
	affs_unlock_dir(new_dir);
	/* TODO: move it back to old_dir, if error? */

done:
	mark_buffer_dirty_inode(bh, retval ? old_dir : new_dir);
	affs_brelse(bh);
	return retval;
}
