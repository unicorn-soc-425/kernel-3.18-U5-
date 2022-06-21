/*
 *   fs/cifs/xattr.c
 *
 *   Copyright (c) International Business Machines  Corp., 2003, 2007
 *   Author(s): Steve French (sfrench@us.ibm.com)
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published
 *   by the Free Software Foundation; either version 2.1 of the License, or
 *   (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this library; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <linux/fs.h>
#include <linux/posix_acl_xattr.h>
#include <linux/slab.h>
#include <linux/xattr.h>
#include "cifsfs.h"
#include "cifspdu.h"
#include "cifsglob.h"
#include "cifsproto.h"
#include "cifs_debug.h"
#include "cifs_fs_sb.h"
#include "cifs_unicode.h"

<<<<<<< HEAD
#define MAX_EA_VALUE_SIZE 65535
#define CIFS_XATTR_DOS_ATTRIB "user.DosAttrib"
#define CIFS_XATTR_CIFS_ACL "system.cifs_acl"

/* BB need to add server (Samba e.g) support for security and trusted prefix */

int cifs_removexattr(struct dentry *direntry, const char *ea_name)
{
	int rc = -EOPNOTSUPP;
#ifdef CONFIG_CIFS_XATTR
	unsigned int xid;
	struct cifs_sb_info *cifs_sb;
	struct tcon_link *tlink;
	struct cifs_tcon *pTcon;
	struct super_block *sb;
	char *full_path = NULL;

	if (direntry == NULL)
		return -EIO;
	if (direntry->d_inode == NULL)
		return -EIO;
	sb = direntry->d_inode->i_sb;
	if (sb == NULL)
		return -EIO;

	cifs_sb = CIFS_SB(sb);
	tlink = cifs_sb_tlink(cifs_sb);
	if (IS_ERR(tlink))
		return PTR_ERR(tlink);
	pTcon = tlink_tcon(tlink);

	xid = get_xid();

	full_path = build_path_from_dentry(direntry);
	if (full_path == NULL) {
		rc = -ENOMEM;
		goto remove_ea_exit;
	}
	if (ea_name == NULL) {
		cifs_dbg(FYI, "Null xattr names not supported\n");
	} else if (strncmp(ea_name, XATTR_USER_PREFIX, XATTR_USER_PREFIX_LEN)
		&& (strncmp(ea_name, XATTR_OS2_PREFIX, XATTR_OS2_PREFIX_LEN))) {
		cifs_dbg(FYI,
			 "illegal xattr request %s (only user namespace supported)\n",
			 ea_name);
		/* BB what if no namespace prefix? */
		/* Should we just pass them to server, except for
		system and perhaps security prefixes? */
	} else {
		if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_NO_XATTR)
			goto remove_ea_exit;

		ea_name += XATTR_USER_PREFIX_LEN; /* skip past user. prefix */
		if (pTcon->ses->server->ops->set_EA)
			rc = pTcon->ses->server->ops->set_EA(xid, pTcon,
				full_path, ea_name, NULL, (__u16)0,
				cifs_sb->local_nls, cifs_remap(cifs_sb));
	}
remove_ea_exit:
	kfree(full_path);
	free_xid(xid);
	cifs_put_tlink(tlink);
#endif
	return rc;
}

int cifs_setxattr(struct dentry *direntry, const char *ea_name,
		  const void *ea_value, size_t value_size, int flags)
{
	int rc = -EOPNOTSUPP;
#ifdef CONFIG_CIFS_XATTR
	unsigned int xid;
	struct cifs_sb_info *cifs_sb;
	struct tcon_link *tlink;
	struct cifs_tcon *pTcon;
	struct super_block *sb;
	char *full_path;

	if (direntry == NULL)
		return -EIO;
	if (direntry->d_inode == NULL)
		return -EIO;
	sb = direntry->d_inode->i_sb;
	if (sb == NULL)
		return -EIO;

	cifs_sb = CIFS_SB(sb);
=======
#define MAX_EA_VALUE_SIZE CIFSMaxBufSize
#define CIFS_XATTR_CIFS_ACL "system.cifs_acl"
#define CIFS_XATTR_ATTRIB "cifs.dosattrib"  /* full name: user.cifs.dosattrib */
#define CIFS_XATTR_CREATETIME "cifs.creationtime"  /* user.cifs.creationtime */
/* BB need to add server (Samba e.g) support for security and trusted prefix */

enum { XATTR_USER, XATTR_CIFS_ACL, XATTR_ACL_ACCESS, XATTR_ACL_DEFAULT };

static int cifs_xattr_set(const struct xattr_handler *handler,
			  struct dentry *dentry, struct inode *inode,
			  const char *name, const void *value,
			  size_t size, int flags)
{
	int rc = -EOPNOTSUPP;
	unsigned int xid;
	struct super_block *sb = dentry->d_sb;
	struct cifs_sb_info *cifs_sb = CIFS_SB(sb);
	struct tcon_link *tlink;
	struct cifs_tcon *pTcon;
	char *full_path;

>>>>>>> v4.9.227
	tlink = cifs_sb_tlink(cifs_sb);
	if (IS_ERR(tlink))
		return PTR_ERR(tlink);
	pTcon = tlink_tcon(tlink);

	xid = get_xid();

<<<<<<< HEAD
	full_path = build_path_from_dentry(direntry);
	if (full_path == NULL) {
		rc = -ENOMEM;
		goto set_ea_exit;
=======
	full_path = build_path_from_dentry(dentry);
	if (full_path == NULL) {
		rc = -ENOMEM;
		goto out;
>>>>>>> v4.9.227
	}
	/* return dos attributes as pseudo xattr */
	/* return alt name if available as pseudo attr */

	/* if proc/fs/cifs/streamstoxattr is set then
		search server for EAs or streams to
		returns as xattrs */
<<<<<<< HEAD
	if (value_size > MAX_EA_VALUE_SIZE) {
		cifs_dbg(FYI, "size of EA value too large\n");
		rc = -EOPNOTSUPP;
		goto set_ea_exit;
	}

	if (ea_name == NULL) {
		cifs_dbg(FYI, "Null xattr names not supported\n");
	} else if (strncmp(ea_name, XATTR_USER_PREFIX, XATTR_USER_PREFIX_LEN)
		   == 0) {
		if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_NO_XATTR)
			goto set_ea_exit;
		if (strncmp(ea_name, CIFS_XATTR_DOS_ATTRIB, 14) == 0)
			cifs_dbg(FYI, "attempt to set cifs inode metadata\n");

		ea_name += XATTR_USER_PREFIX_LEN; /* skip past user. prefix */
		if (pTcon->ses->server->ops->set_EA)
			rc = pTcon->ses->server->ops->set_EA(xid, pTcon,
				full_path, ea_name, ea_value, (__u16)value_size,
				cifs_sb->local_nls, cifs_remap(cifs_sb));
	} else if (strncmp(ea_name, XATTR_OS2_PREFIX, XATTR_OS2_PREFIX_LEN)
		   == 0) {
		if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_NO_XATTR)
			goto set_ea_exit;

		ea_name += XATTR_OS2_PREFIX_LEN; /* skip past os2. prefix */
		if (pTcon->ses->server->ops->set_EA)
			rc = pTcon->ses->server->ops->set_EA(xid, pTcon,
				full_path, ea_name, ea_value, (__u16)value_size,
				cifs_sb->local_nls, cifs_remap(cifs_sb));
	} else if (strncmp(ea_name, CIFS_XATTR_CIFS_ACL,
			strlen(CIFS_XATTR_CIFS_ACL)) == 0) {
#ifdef CONFIG_CIFS_ACL
		struct cifs_ntsd *pacl;
		pacl = kmalloc(value_size, GFP_KERNEL);
		if (!pacl) {
			rc = -ENOMEM;
		} else {
			memcpy(pacl, ea_value, value_size);
			if (pTcon->ses->server->ops->set_acl)
				rc = pTcon->ses->server->ops->set_acl(pacl,
						value_size, direntry->d_inode,
=======
	if (size > MAX_EA_VALUE_SIZE) {
		cifs_dbg(FYI, "size of EA value too large\n");
		rc = -EOPNOTSUPP;
		goto out;
	}

	switch (handler->flags) {
	case XATTR_USER:
		if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_NO_XATTR)
			goto out;

		if (pTcon->ses->server->ops->set_EA)
			rc = pTcon->ses->server->ops->set_EA(xid, pTcon,
				full_path, name, value, (__u16)size,
				cifs_sb->local_nls, cifs_remap(cifs_sb));
		break;

	case XATTR_CIFS_ACL: {
#ifdef CONFIG_CIFS_ACL
		struct cifs_ntsd *pacl;

		if (!value)
			goto out;
		pacl = kmalloc(size, GFP_KERNEL);
		if (!pacl) {
			rc = -ENOMEM;
		} else {
			memcpy(pacl, value, size);
			if (value &&
			    pTcon->ses->server->ops->set_acl)
				rc = pTcon->ses->server->ops->set_acl(pacl,
						size, inode,
>>>>>>> v4.9.227
						full_path, CIFS_ACL_DACL);
			else
				rc = -EOPNOTSUPP;
			if (rc == 0) /* force revalidate of the inode */
<<<<<<< HEAD
				CIFS_I(direntry->d_inode)->time = 0;
			kfree(pacl);
		}
#else
		cifs_dbg(FYI, "Set CIFS ACL not supported yet\n");
#endif /* CONFIG_CIFS_ACL */
	} else {
		int temp;
		temp = strncmp(ea_name, POSIX_ACL_XATTR_ACCESS,
			strlen(POSIX_ACL_XATTR_ACCESS));
		if (temp == 0) {
#ifdef CONFIG_CIFS_POSIX
			if (sb->s_flags & MS_POSIXACL)
				rc = CIFSSMBSetPosixACL(xid, pTcon, full_path,
					ea_value, (const int)value_size,
					ACL_TYPE_ACCESS, cifs_sb->local_nls,
					cifs_remap(cifs_sb));
			cifs_dbg(FYI, "set POSIX ACL rc %d\n", rc);
#else
			cifs_dbg(FYI, "set POSIX ACL not supported\n");
#endif
		} else if (strncmp(ea_name, POSIX_ACL_XATTR_DEFAULT,
				   strlen(POSIX_ACL_XATTR_DEFAULT)) == 0) {
#ifdef CONFIG_CIFS_POSIX
			if (sb->s_flags & MS_POSIXACL)
				rc = CIFSSMBSetPosixACL(xid, pTcon, full_path,
					ea_value, (const int)value_size,
					ACL_TYPE_DEFAULT, cifs_sb->local_nls,
					cifs_remap(cifs_sb));
			cifs_dbg(FYI, "set POSIX default ACL rc %d\n", rc);
#else
			cifs_dbg(FYI, "set default POSIX ACL not supported\n");
#endif
		} else {
			cifs_dbg(FYI, "illegal xattr request %s (only user namespace supported)\n",
				 ea_name);
		  /* BB what if no namespace prefix? */
		  /* Should we just pass them to server, except for
		  system and perhaps security prefixes? */
		}
	}

set_ea_exit:
	kfree(full_path);
	free_xid(xid);
	cifs_put_tlink(tlink);
#endif
	return rc;
}

ssize_t cifs_getxattr(struct dentry *direntry, const char *ea_name,
	void *ea_value, size_t buf_size)
{
	ssize_t rc = -EOPNOTSUPP;
#ifdef CONFIG_CIFS_XATTR
	unsigned int xid;
	struct cifs_sb_info *cifs_sb;
	struct tcon_link *tlink;
	struct cifs_tcon *pTcon;
	struct super_block *sb;
	char *full_path;

	if (direntry == NULL)
		return -EIO;
	if (direntry->d_inode == NULL)
		return -EIO;
	sb = direntry->d_inode->i_sb;
	if (sb == NULL)
		return -EIO;

	cifs_sb = CIFS_SB(sb);
=======
				CIFS_I(inode)->time = 0;
			kfree(pacl);
		}
#endif /* CONFIG_CIFS_ACL */
		break;
	}

	case XATTR_ACL_ACCESS:
#ifdef CONFIG_CIFS_POSIX
		if (!value)
			goto out;
		if (sb->s_flags & MS_POSIXACL)
			rc = CIFSSMBSetPosixACL(xid, pTcon, full_path,
				value, (const int)size,
				ACL_TYPE_ACCESS, cifs_sb->local_nls,
				cifs_remap(cifs_sb));
#endif  /* CONFIG_CIFS_POSIX */
		break;

	case XATTR_ACL_DEFAULT:
#ifdef CONFIG_CIFS_POSIX
		if (!value)
			goto out;
		if (sb->s_flags & MS_POSIXACL)
			rc = CIFSSMBSetPosixACL(xid, pTcon, full_path,
				value, (const int)size,
				ACL_TYPE_DEFAULT, cifs_sb->local_nls,
				cifs_remap(cifs_sb));
#endif  /* CONFIG_CIFS_POSIX */
		break;
	}

out:
	kfree(full_path);
	free_xid(xid);
	cifs_put_tlink(tlink);
	return rc;
}

static int cifs_attrib_get(struct dentry *dentry,
			   struct inode *inode, void *value,
			   size_t size)
{
	ssize_t rc;
	__u32 *pattribute;

	rc = cifs_revalidate_dentry_attr(dentry);

	if (rc)
		return rc;

	if ((value == NULL) || (size == 0))
		return sizeof(__u32);
	else if (size < sizeof(__u32))
		return -ERANGE;

	/* return dos attributes as pseudo xattr */
	pattribute = (__u32 *)value;
	*pattribute = CIFS_I(inode)->cifsAttrs;

	return sizeof(__u32);
}

static int cifs_creation_time_get(struct dentry *dentry, struct inode *inode,
				  void *value, size_t size)
{
	ssize_t rc;
	__u64 * pcreatetime;

	rc = cifs_revalidate_dentry_attr(dentry);
	if (rc)
		return rc;

	if ((value == NULL) || (size == 0))
		return sizeof(__u64);
	else if (size < sizeof(__u64))
		return -ERANGE;

	/* return dos attributes as pseudo xattr */
	pcreatetime = (__u64 *)value;
	*pcreatetime = CIFS_I(inode)->createtime;
	return sizeof(__u64);

	return rc;
}


static int cifs_xattr_get(const struct xattr_handler *handler,
			  struct dentry *dentry, struct inode *inode,
			  const char *name, void *value, size_t size)
{
	ssize_t rc = -EOPNOTSUPP;
	unsigned int xid;
	struct super_block *sb = dentry->d_sb;
	struct cifs_sb_info *cifs_sb = CIFS_SB(sb);
	struct tcon_link *tlink;
	struct cifs_tcon *pTcon;
	char *full_path;

>>>>>>> v4.9.227
	tlink = cifs_sb_tlink(cifs_sb);
	if (IS_ERR(tlink))
		return PTR_ERR(tlink);
	pTcon = tlink_tcon(tlink);

	xid = get_xid();

<<<<<<< HEAD
	full_path = build_path_from_dentry(direntry);
	if (full_path == NULL) {
		rc = -ENOMEM;
		goto get_ea_exit;
	}
	/* return dos attributes as pseudo xattr */
	/* return alt name if available as pseudo attr */
	if (ea_name == NULL) {
		cifs_dbg(FYI, "Null xattr names not supported\n");
	} else if (strncmp(ea_name, XATTR_USER_PREFIX, XATTR_USER_PREFIX_LEN)
		   == 0) {
		if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_NO_XATTR)
			goto get_ea_exit;

		if (strncmp(ea_name, CIFS_XATTR_DOS_ATTRIB, 14) == 0) {
			cifs_dbg(FYI, "attempt to query cifs inode metadata\n");
			/* revalidate/getattr then populate from inode */
		} /* BB add else when above is implemented */
		ea_name += XATTR_USER_PREFIX_LEN; /* skip past user. prefix */
		if (pTcon->ses->server->ops->query_all_EAs)
			rc = pTcon->ses->server->ops->query_all_EAs(xid, pTcon,
				full_path, ea_name, ea_value, buf_size,
				cifs_sb->local_nls, cifs_remap(cifs_sb));
	} else if (strncmp(ea_name, XATTR_OS2_PREFIX, XATTR_OS2_PREFIX_LEN) == 0) {
		if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_NO_XATTR)
			goto get_ea_exit;

		ea_name += XATTR_OS2_PREFIX_LEN; /* skip past os2. prefix */
		if (pTcon->ses->server->ops->query_all_EAs)
			rc = pTcon->ses->server->ops->query_all_EAs(xid, pTcon,
				full_path, ea_name, ea_value, buf_size,
				cifs_sb->local_nls, cifs_remap(cifs_sb));
	} else if (strncmp(ea_name, POSIX_ACL_XATTR_ACCESS,
			  strlen(POSIX_ACL_XATTR_ACCESS)) == 0) {
#ifdef CONFIG_CIFS_POSIX
		if (sb->s_flags & MS_POSIXACL)
			rc = CIFSSMBGetPosixACL(xid, pTcon, full_path,
				ea_value, buf_size, ACL_TYPE_ACCESS,
				cifs_sb->local_nls,
				cifs_remap(cifs_sb));
#else
		cifs_dbg(FYI, "Query POSIX ACL not supported yet\n");
#endif /* CONFIG_CIFS_POSIX */
	} else if (strncmp(ea_name, POSIX_ACL_XATTR_DEFAULT,
			  strlen(POSIX_ACL_XATTR_DEFAULT)) == 0) {
#ifdef CONFIG_CIFS_POSIX
		if (sb->s_flags & MS_POSIXACL)
			rc = CIFSSMBGetPosixACL(xid, pTcon, full_path,
				ea_value, buf_size, ACL_TYPE_DEFAULT,
				cifs_sb->local_nls,
				cifs_remap(cifs_sb));
#else
		cifs_dbg(FYI, "Query POSIX default ACL not supported yet\n");
#endif /* CONFIG_CIFS_POSIX */
	} else if (strncmp(ea_name, CIFS_XATTR_CIFS_ACL,
				strlen(CIFS_XATTR_CIFS_ACL)) == 0) {
#ifdef CONFIG_CIFS_ACL
			u32 acllen;
			struct cifs_ntsd *pacl;

			if (pTcon->ses->server->ops->get_acl == NULL)
				goto get_ea_exit; /* rc already EOPNOTSUPP */

			pacl = pTcon->ses->server->ops->get_acl(cifs_sb,
					direntry->d_inode, full_path, &acllen);
			if (IS_ERR(pacl)) {
				rc = PTR_ERR(pacl);
				cifs_dbg(VFS, "%s: error %zd getting sec desc\n",
					 __func__, rc);
			} else {
				if (ea_value) {
					if (acllen > buf_size)
						acllen = -ERANGE;
					else
						memcpy(ea_value, pacl, acllen);
				}
				rc = acllen;
				kfree(pacl);
			}
#else
			cifs_dbg(FYI, "Query CIFS ACL not supported yet\n");
#endif /* CONFIG_CIFS_ACL */
	} else if (strncmp(ea_name,
		  XATTR_TRUSTED_PREFIX, XATTR_TRUSTED_PREFIX_LEN) == 0) {
		cifs_dbg(FYI, "Trusted xattr namespace not supported yet\n");
	} else if (strncmp(ea_name,
		  XATTR_SECURITY_PREFIX, XATTR_SECURITY_PREFIX_LEN) == 0) {
		cifs_dbg(FYI, "Security xattr namespace not supported yet\n");
	} else
		cifs_dbg(FYI,
			 "illegal xattr request %s (only user namespace supported)\n",
			 ea_name);
=======
	full_path = build_path_from_dentry(dentry);
	if (full_path == NULL) {
		rc = -ENOMEM;
		goto out;
	}

	/* return alt name if available as pseudo attr */
	switch (handler->flags) {
	case XATTR_USER:
		cifs_dbg(FYI, "%s:querying user xattr %s\n", __func__, name);
		if (strcmp(name, CIFS_XATTR_ATTRIB) == 0) {
			rc = cifs_attrib_get(dentry, inode, value, size);
			break;
		} else if (strcmp(name, CIFS_XATTR_CREATETIME) == 0) {
			rc = cifs_creation_time_get(dentry, inode, value, size);
			break;
		}

		if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_NO_XATTR)
			goto out;

		if (pTcon->ses->server->ops->query_all_EAs)
			rc = pTcon->ses->server->ops->query_all_EAs(xid, pTcon,
				full_path, name, value, size,
				cifs_sb->local_nls, cifs_remap(cifs_sb));
		break;

	case XATTR_CIFS_ACL: {
#ifdef CONFIG_CIFS_ACL
		u32 acllen;
		struct cifs_ntsd *pacl;

		if (pTcon->ses->server->ops->get_acl == NULL)
			goto out; /* rc already EOPNOTSUPP */

		pacl = pTcon->ses->server->ops->get_acl(cifs_sb,
				inode, full_path, &acllen);
		if (IS_ERR(pacl)) {
			rc = PTR_ERR(pacl);
			cifs_dbg(VFS, "%s: error %zd getting sec desc\n",
				 __func__, rc);
		} else {
			if (value) {
				if (acllen > size)
					acllen = -ERANGE;
				else
					memcpy(value, pacl, acllen);
			}
			rc = acllen;
			kfree(pacl);
		}
#endif  /* CONFIG_CIFS_ACL */
		break;
	}

	case XATTR_ACL_ACCESS:
#ifdef CONFIG_CIFS_POSIX
		if (sb->s_flags & MS_POSIXACL)
			rc = CIFSSMBGetPosixACL(xid, pTcon, full_path,
				value, size, ACL_TYPE_ACCESS,
				cifs_sb->local_nls,
				cifs_remap(cifs_sb));
#endif  /* CONFIG_CIFS_POSIX */
		break;

	case XATTR_ACL_DEFAULT:
#ifdef CONFIG_CIFS_POSIX
		if (sb->s_flags & MS_POSIXACL)
			rc = CIFSSMBGetPosixACL(xid, pTcon, full_path,
				value, size, ACL_TYPE_DEFAULT,
				cifs_sb->local_nls,
				cifs_remap(cifs_sb));
#endif  /* CONFIG_CIFS_POSIX */
		break;
	}
>>>>>>> v4.9.227

	/* We could add an additional check for streams ie
	    if proc/fs/cifs/streamstoxattr is set then
		search server for EAs or streams to
		returns as xattrs */

	if (rc == -EINVAL)
		rc = -EOPNOTSUPP;

<<<<<<< HEAD
get_ea_exit:
	kfree(full_path);
	free_xid(xid);
	cifs_put_tlink(tlink);
#endif
=======
out:
	kfree(full_path);
	free_xid(xid);
	cifs_put_tlink(tlink);
>>>>>>> v4.9.227
	return rc;
}

ssize_t cifs_listxattr(struct dentry *direntry, char *data, size_t buf_size)
{
	ssize_t rc = -EOPNOTSUPP;
<<<<<<< HEAD
#ifdef CONFIG_CIFS_XATTR
	unsigned int xid;
	struct cifs_sb_info *cifs_sb;
	struct tcon_link *tlink;
	struct cifs_tcon *pTcon;
	struct super_block *sb;
	char *full_path;

	if (direntry == NULL)
		return -EIO;
	if (direntry->d_inode == NULL)
		return -EIO;
	sb = direntry->d_inode->i_sb;
	if (sb == NULL)
		return -EIO;

	cifs_sb = CIFS_SB(sb);
=======
	unsigned int xid;
	struct cifs_sb_info *cifs_sb = CIFS_SB(direntry->d_sb);
	struct tcon_link *tlink;
	struct cifs_tcon *pTcon;
	char *full_path;

>>>>>>> v4.9.227
	if (cifs_sb->mnt_cifs_flags & CIFS_MOUNT_NO_XATTR)
		return -EOPNOTSUPP;

	tlink = cifs_sb_tlink(cifs_sb);
	if (IS_ERR(tlink))
		return PTR_ERR(tlink);
	pTcon = tlink_tcon(tlink);

	xid = get_xid();

	full_path = build_path_from_dentry(direntry);
	if (full_path == NULL) {
		rc = -ENOMEM;
		goto list_ea_exit;
	}
	/* return dos attributes as pseudo xattr */
	/* return alt name if available as pseudo attr */

	/* if proc/fs/cifs/streamstoxattr is set then
		search server for EAs or streams to
		returns as xattrs */

	if (pTcon->ses->server->ops->query_all_EAs)
		rc = pTcon->ses->server->ops->query_all_EAs(xid, pTcon,
				full_path, NULL, data, buf_size,
				cifs_sb->local_nls, cifs_remap(cifs_sb));
list_ea_exit:
	kfree(full_path);
	free_xid(xid);
	cifs_put_tlink(tlink);
<<<<<<< HEAD
#endif
	return rc;
}
=======
	return rc;
}

static const struct xattr_handler cifs_user_xattr_handler = {
	.prefix = XATTR_USER_PREFIX,
	.flags = XATTR_USER,
	.get = cifs_xattr_get,
	.set = cifs_xattr_set,
};

/* os2.* attributes are treated like user.* attributes */
static const struct xattr_handler cifs_os2_xattr_handler = {
	.prefix = XATTR_OS2_PREFIX,
	.flags = XATTR_USER,
	.get = cifs_xattr_get,
	.set = cifs_xattr_set,
};

static const struct xattr_handler cifs_cifs_acl_xattr_handler = {
	.name = CIFS_XATTR_CIFS_ACL,
	.flags = XATTR_CIFS_ACL,
	.get = cifs_xattr_get,
	.set = cifs_xattr_set,
};

static const struct xattr_handler cifs_posix_acl_access_xattr_handler = {
	.name = XATTR_NAME_POSIX_ACL_ACCESS,
	.flags = XATTR_ACL_ACCESS,
	.get = cifs_xattr_get,
	.set = cifs_xattr_set,
};

static const struct xattr_handler cifs_posix_acl_default_xattr_handler = {
	.name = XATTR_NAME_POSIX_ACL_DEFAULT,
	.flags = XATTR_ACL_DEFAULT,
	.get = cifs_xattr_get,
	.set = cifs_xattr_set,
};

const struct xattr_handler *cifs_xattr_handlers[] = {
	&cifs_user_xattr_handler,
	&cifs_os2_xattr_handler,
	&cifs_cifs_acl_xattr_handler,
	&cifs_posix_acl_access_xattr_handler,
	&cifs_posix_acl_default_xattr_handler,
	NULL
};
>>>>>>> v4.9.227
