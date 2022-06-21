/*
 * GPL HEADER START
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 only,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License version 2 for more details (a copy is included
 * in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; If not, see
<<<<<<< HEAD
 * http://www.sun.com/software/products/lustre/docs/GPLv2.pdf
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
=======
 * http://www.gnu.org/licenses/gpl-2.0.html
>>>>>>> v4.9.227
 *
 * GPL HEADER END
 */
/*
 * Copyright (c) 2002, 2010, Oracle and/or its affiliates. All rights reserved.
 * Use is subject to license terms.
 *
<<<<<<< HEAD
 * Copyright (c) 2011, 2012, Intel Corporation.
=======
 * Copyright (c) 2011, 2015, Intel Corporation.
>>>>>>> v4.9.227
 */
/*
 * This file is part of Lustre, http://www.lustre.org/
 * Lustre is a trademark of Sun Microsystems, Inc.
 *
 * lustre/obdclass/lprocfs_status.c
 *
 * Author: Hariharan Thantry <thantry@users.sourceforge.net>
 */

#define DEBUG_SUBSYSTEM S_CLASS

<<<<<<< HEAD

=======
>>>>>>> v4.9.227
#include "../include/obd_class.h"
#include "../include/lprocfs_status.h"
#include "../include/lustre/lustre_idl.h"
#include <linux/seq_file.h>
<<<<<<< HEAD
=======
#include <linux/ctype.h>
>>>>>>> v4.9.227

static const char * const obd_connect_names[] = {
	"read_only",
	"lov_index",
<<<<<<< HEAD
	"unused",
=======
	"connect_from_mds",
>>>>>>> v4.9.227
	"write_grant",
	"server_lock",
	"version",
	"request_portal",
	"acl",
	"xattr",
	"create_on_write",
	"truncate_lock",
	"initial_transno",
	"inode_bit_locks",
	"join_file(obsolete)",
	"getattr_by_fid",
	"no_oh_for_devices",
	"remote_client",
	"remote_client_by_force",
	"max_byte_per_rpc",
	"64bit_qdata",
	"mds_capability",
	"oss_capability",
	"early_lock_cancel",
	"som",
	"adaptive_timeouts",
	"lru_resize",
	"mds_mds_connection",
	"real_conn",
	"change_qunit_size",
	"alt_checksum_algorithm",
	"fid_is_enabled",
	"version_recovery",
	"pools",
	"grant_shrink",
	"skip_orphan",
	"large_ea",
	"full20",
	"layout_lock",
	"64bithash",
	"object_max_bytes",
	"imp_recov",
	"jobstats",
	"umask",
	"einprogress",
	"grant_param",
	"flock_owner",
	"lvb_type",
	"nanoseconds_times",
	"lightweight_conn",
	"short_io",
	"pingless",
	"flock_deadlock",
	"disp_stripe",
<<<<<<< HEAD
=======
	"open_by_fid",
	"lfsck",
	"unknown",
	"unlink_close",
	"unknown",
	"dir_stripe",
>>>>>>> v4.9.227
	"unknown",
	NULL
};

int obd_connect_flags2str(char *page, int count, __u64 flags, char *sep)
{
	__u64 mask = 1;
	int i, ret = 0;

<<<<<<< HEAD
	for (i = 0; obd_connect_names[i] != NULL; i++, mask <<= 1) {
=======
	for (i = 0; obd_connect_names[i]; i++, mask <<= 1) {
>>>>>>> v4.9.227
		if (flags & mask)
			ret += snprintf(page + ret, count - ret, "%s%s",
					ret ? sep : "", obd_connect_names[i]);
	}
	if (flags & ~(mask - 1))
		ret += snprintf(page + ret, count - ret,
				"%sunknown flags %#llx",
				ret ? sep : "", flags & ~(mask - 1));
	return ret;
}
EXPORT_SYMBOL(obd_connect_flags2str);

<<<<<<< HEAD
=======
static void obd_connect_data_seqprint(struct seq_file *m,
				      struct obd_connect_data *ocd)
{
	int flags;

	LASSERT(ocd);
	flags = ocd->ocd_connect_flags;

	seq_printf(m, "    connect_data:\n"
		   "       flags: %llx\n"
		   "       instance: %u\n",
		   ocd->ocd_connect_flags,
		   ocd->ocd_instance);
	if (flags & OBD_CONNECT_VERSION)
		seq_printf(m, "       target_version: %u.%u.%u.%u\n",
			   OBD_OCD_VERSION_MAJOR(ocd->ocd_version),
			   OBD_OCD_VERSION_MINOR(ocd->ocd_version),
			   OBD_OCD_VERSION_PATCH(ocd->ocd_version),
			   OBD_OCD_VERSION_FIX(ocd->ocd_version));
	if (flags & OBD_CONNECT_MDS)
		seq_printf(m, "       mdt_index: %d\n", ocd->ocd_group);
	if (flags & OBD_CONNECT_GRANT)
		seq_printf(m, "       initial_grant: %d\n", ocd->ocd_grant);
	if (flags & OBD_CONNECT_INDEX)
		seq_printf(m, "       target_index: %u\n", ocd->ocd_index);
	if (flags & OBD_CONNECT_BRW_SIZE)
		seq_printf(m, "       max_brw_size: %d\n", ocd->ocd_brw_size);
	if (flags & OBD_CONNECT_IBITS)
		seq_printf(m, "       ibits_known: %llx\n",
			   ocd->ocd_ibits_known);
	if (flags & OBD_CONNECT_GRANT_PARAM)
		seq_printf(m, "       grant_block_size: %d\n"
			   "       grant_inode_size: %d\n"
			   "       grant_extent_overhead: %d\n",
			   ocd->ocd_blocksize,
			   ocd->ocd_inodespace,
			   ocd->ocd_grant_extent);
	if (flags & OBD_CONNECT_TRANSNO)
		seq_printf(m, "       first_transno: %llx\n",
			   ocd->ocd_transno);
	if (flags & OBD_CONNECT_CKSUM)
		seq_printf(m, "       cksum_types: %#x\n",
			   ocd->ocd_cksum_types);
	if (flags & OBD_CONNECT_MAX_EASIZE)
		seq_printf(m, "       max_easize: %d\n", ocd->ocd_max_easize);
	if (flags & OBD_CONNECT_MAXBYTES)
		seq_printf(m, "       max_object_bytes: %llx\n",
			   ocd->ocd_maxbytes);
}

>>>>>>> v4.9.227
int lprocfs_read_frac_helper(char *buffer, unsigned long count, long val,
			     int mult)
{
	long decimal_val, frac_val;
	int prtn;

	if (count < 10)
		return -EINVAL;

	decimal_val = val / mult;
	prtn = snprintf(buffer, count, "%ld", decimal_val);
	frac_val = val % mult;

	if (prtn < (count - 4) && frac_val > 0) {
		long temp_frac;
		int i, temp_mult = 1, frac_bits = 0;

		temp_frac = frac_val * 10;
		buffer[prtn++] = '.';
		while (frac_bits < 2 && (temp_frac / mult) < 1) {
			/* only reserved 2 bits fraction */
			buffer[prtn++] = '0';
			temp_frac *= 10;
			frac_bits++;
		}
		/*
		 * Need to think these cases :
<<<<<<< HEAD
		 *      1. #echo x.00 > /proc/xxx       output result : x
		 *      2. #echo x.0x > /proc/xxx       output result : x.0x
		 *      3. #echo x.x0 > /proc/xxx       output result : x.x
		 *      4. #echo x.xx > /proc/xxx       output result : x.xx
=======
		 *      1. #echo x.00 > /sys/xxx       output result : x
		 *      2. #echo x.0x > /sys/xxx       output result : x.0x
		 *      3. #echo x.x0 > /sys/xxx       output result : x.x
		 *      4. #echo x.xx > /sys/xxx       output result : x.xx
>>>>>>> v4.9.227
		 *      Only reserved 2 bits fraction.
		 */
		for (i = 0; i < (5 - prtn); i++)
			temp_mult *= 10;

		frac_bits = min((int)count - prtn, 3 - frac_bits);
		prtn += snprintf(buffer + prtn, frac_bits, "%ld",
				 frac_val * temp_mult / mult);

		prtn--;
		while (buffer[prtn] < '1' || buffer[prtn] > '9') {
			prtn--;
			if (buffer[prtn] == '.') {
				prtn--;
				break;
			}
		}
		prtn++;
	}
	buffer[prtn++] = '\n';
	return prtn;
}
EXPORT_SYMBOL(lprocfs_read_frac_helper);

<<<<<<< HEAD
int lprocfs_write_frac_helper(const char *buffer, unsigned long count,
=======
int lprocfs_write_frac_helper(const char __user *buffer, unsigned long count,
>>>>>>> v4.9.227
			      int *val, int mult)
{
	char kernbuf[20], *end, *pbuf;

	if (count > (sizeof(kernbuf) - 1))
		return -EINVAL;

	if (copy_from_user(kernbuf, buffer, count))
		return -EFAULT;

	kernbuf[count] = '\0';
	pbuf = kernbuf;
	if (*pbuf == '-') {
		mult = -mult;
		pbuf++;
	}

	*val = (int)simple_strtoul(pbuf, &end, 10) * mult;
	if (pbuf == end)
		return -EINVAL;

<<<<<<< HEAD
	if (end != NULL && *end == '.') {
=======
	if (end && *end == '.') {
>>>>>>> v4.9.227
		int temp_val, pow = 1;
		int i;

		pbuf = end + 1;
		if (strlen(pbuf) > 5)
			pbuf[5] = '\0'; /*only allow 5bits fractional*/

		temp_val = (int)simple_strtoul(pbuf, &end, 10) * mult;

		if (pbuf < end) {
			for (i = 0; i < (end - pbuf); i++)
				pow *= 10;

			*val += temp_val / pow;
		}
	}
	return 0;
}
EXPORT_SYMBOL(lprocfs_write_frac_helper);

<<<<<<< HEAD
#if defined (CONFIG_PROC_FS)

static int lprocfs_no_percpu_stats = 0;
=======
static int lprocfs_no_percpu_stats;
>>>>>>> v4.9.227
module_param(lprocfs_no_percpu_stats, int, 0644);
MODULE_PARM_DESC(lprocfs_no_percpu_stats, "Do not alloc percpu data for lprocfs stats");

#define MAX_STRING_SIZE 128

int lprocfs_single_release(struct inode *inode, struct file *file)
{
	return single_release(inode, file);
}
EXPORT_SYMBOL(lprocfs_single_release);

int lprocfs_seq_release(struct inode *inode, struct file *file)
{
	return seq_release(inode, file);
}
EXPORT_SYMBOL(lprocfs_seq_release);

/* lprocfs API calls */

<<<<<<< HEAD
struct proc_dir_entry *lprocfs_add_simple(struct proc_dir_entry *root,
				     char *name, void *data,
				     struct file_operations *fops)
{
	struct proc_dir_entry *proc;
	umode_t mode = 0;

	if (root == NULL || name == NULL || fops == NULL)
=======
struct dentry *ldebugfs_add_simple(struct dentry *root,
				   char *name, void *data,
				   struct file_operations *fops)
{
	struct dentry *entry;
	umode_t mode = 0;

	if (!root || !name || !fops)
>>>>>>> v4.9.227
		return ERR_PTR(-EINVAL);

	if (fops->read)
		mode = 0444;
	if (fops->write)
		mode |= 0200;
<<<<<<< HEAD
	proc = proc_create_data(name, mode, root, fops, data);
	if (!proc) {
		CERROR("LprocFS: No memory to create /proc entry %s", name);
		return ERR_PTR(-ENOMEM);
	}
	return proc;
}
EXPORT_SYMBOL(lprocfs_add_simple);

struct proc_dir_entry *lprocfs_add_symlink(const char *name,
			struct proc_dir_entry *parent, const char *format, ...)
{
	struct proc_dir_entry *entry;
	char *dest;
	va_list ap;

	if (parent == NULL || format == NULL)
		return NULL;

	OBD_ALLOC_WAIT(dest, MAX_STRING_SIZE + 1);
	if (dest == NULL)
		return NULL;

	va_start(ap, format);
	vsnprintf(dest, MAX_STRING_SIZE, format, ap);
	va_end(ap);

	entry = proc_symlink(name, parent, dest);
	if (entry == NULL)
		CERROR("LprocFS: Could not create symbolic link from %s to %s",
			name, dest);

	OBD_FREE(dest, MAX_STRING_SIZE + 1);
	return entry;
}
EXPORT_SYMBOL(lprocfs_add_symlink);

static struct file_operations lprocfs_generic_fops = { };

/**
 * Add /proc entries.
 *
 * \param root [in]  The parent proc entry on which new entry will be added.
 * \param list [in]  Array of proc entries to be added.
 * \param data [in]  The argument to be passed when entries read/write routines
 *		   are called through /proc file.
 *
 * \retval 0   on success
 *	 < 0 on error
 */
int lprocfs_add_vars(struct proc_dir_entry *root, struct lprocfs_vars *list,
		     void *data)
{
	if (root == NULL || list == NULL)
		return -EINVAL;

	while (list->name != NULL) {
		struct proc_dir_entry *proc;
=======
	entry = debugfs_create_file(name, mode, root, data, fops);
	if (IS_ERR_OR_NULL(entry)) {
		CERROR("LprocFS: No memory to create <debugfs> entry %s\n", name);
		return entry ?: ERR_PTR(-ENOMEM);
	}
	return entry;
}
EXPORT_SYMBOL_GPL(ldebugfs_add_simple);

static const struct file_operations lprocfs_generic_fops = { };

int ldebugfs_add_vars(struct dentry *parent,
		      struct lprocfs_vars *list,
		      void *data)
{
	if (IS_ERR_OR_NULL(parent) || IS_ERR_OR_NULL(list))
		return -EINVAL;

	while (list->name) {
		struct dentry *entry;
>>>>>>> v4.9.227
		umode_t mode = 0;

		if (list->proc_mode != 0000) {
			mode = list->proc_mode;
		} else if (list->fops) {
			if (list->fops->read)
				mode = 0444;
			if (list->fops->write)
				mode |= 0200;
		}
<<<<<<< HEAD
		proc = proc_create_data(list->name, mode, root,
					list->fops ?: &lprocfs_generic_fops,
					list->data ?: data);
		if (proc == NULL)
			return -ENOMEM;
=======
		entry = debugfs_create_file(list->name, mode, parent,
					    list->data ?: data,
					    list->fops ?: &lprocfs_generic_fops
					   );
		if (IS_ERR_OR_NULL(entry))
			return entry ? PTR_ERR(entry) : -ENOMEM;
>>>>>>> v4.9.227
		list++;
	}
	return 0;
}
<<<<<<< HEAD
EXPORT_SYMBOL(lprocfs_add_vars);

void lprocfs_remove(struct proc_dir_entry **rooth)
{
	proc_remove(*rooth);
	*rooth = NULL;
}
EXPORT_SYMBOL(lprocfs_remove);

void lprocfs_remove_proc_entry(const char *name, struct proc_dir_entry *parent)
{
	LASSERT(parent != NULL);
	remove_proc_entry(name, parent);
}
EXPORT_SYMBOL(lprocfs_remove_proc_entry);

struct proc_dir_entry *lprocfs_register(const char *name,
					struct proc_dir_entry *parent,
					struct lprocfs_vars *list, void *data)
{
	struct proc_dir_entry *entry;

	entry = proc_mkdir(name, parent);
	if (entry == NULL) {
		entry = ERR_PTR(-ENOMEM);
		goto out;
	}

	if (list != NULL) {
		int rc = lprocfs_add_vars(entry, list, data);
		if (rc != 0) {
			lprocfs_remove(&entry);
=======
EXPORT_SYMBOL_GPL(ldebugfs_add_vars);

void ldebugfs_remove(struct dentry **entryp)
{
	debugfs_remove_recursive(*entryp);
	*entryp = NULL;
}
EXPORT_SYMBOL_GPL(ldebugfs_remove);

struct dentry *ldebugfs_register(const char *name,
				 struct dentry *parent,
				 struct lprocfs_vars *list, void *data)
{
	struct dentry *entry;

	entry = debugfs_create_dir(name, parent);
	if (IS_ERR_OR_NULL(entry)) {
		entry = entry ?: ERR_PTR(-ENOMEM);
		goto out;
	}

	if (!IS_ERR_OR_NULL(list)) {
		int rc;

		rc = ldebugfs_add_vars(entry, list, data);
		if (rc != 0) {
			debugfs_remove(entry);
>>>>>>> v4.9.227
			entry = ERR_PTR(rc);
		}
	}
out:
	return entry;
}
<<<<<<< HEAD
EXPORT_SYMBOL(lprocfs_register);
=======
EXPORT_SYMBOL_GPL(ldebugfs_register);
>>>>>>> v4.9.227

/* Generic callbacks */
int lprocfs_rd_uint(struct seq_file *m, void *data)
{
<<<<<<< HEAD
	return seq_printf(m, "%u\n", *(unsigned int *)data);
=======
	seq_printf(m, "%u\n", *(unsigned int *)data);
	return 0;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(lprocfs_rd_uint);

int lprocfs_wr_uint(struct file *file, const char __user *buffer,
		    unsigned long count, void *data)
{
	unsigned *p = data;
	char dummy[MAX_STRING_SIZE + 1], *end;
	unsigned long tmp;

	dummy[MAX_STRING_SIZE] = '\0';
	if (copy_from_user(dummy, buffer, MAX_STRING_SIZE))
		return -EFAULT;

	tmp = simple_strtoul(dummy, &end, 0);
	if (dummy == end)
		return -EINVAL;

	*p = (unsigned int)tmp;
	return count;
}
EXPORT_SYMBOL(lprocfs_wr_uint);

<<<<<<< HEAD
int lprocfs_rd_u64(struct seq_file *m, void *data)
{
	return seq_printf(m, "%llu\n", *(__u64 *)data);
}
EXPORT_SYMBOL(lprocfs_rd_u64);

int lprocfs_rd_atomic(struct seq_file *m, void *data)
{
	atomic_t *atom = data;
	LASSERT(atom != NULL);
	return seq_printf(m, "%d\n", atomic_read(atom));
}
EXPORT_SYMBOL(lprocfs_rd_atomic);

int lprocfs_wr_atomic(struct file *file, const char __user *buffer,
		      unsigned long count, void *data)
{
	atomic_t *atm = data;
	int val = 0;
	int rc;

	rc = lprocfs_write_helper(buffer, count, &val);
	if (rc < 0)
		return rc;

	if (val <= 0)
		return -ERANGE;

	atomic_set(atm, val);
	return count;
}
EXPORT_SYMBOL(lprocfs_wr_atomic);

int lprocfs_rd_uuid(struct seq_file *m, void *data)
{
	struct obd_device *obd = data;

	LASSERT(obd != NULL);
	return seq_printf(m, "%s\n", obd->obd_uuid.uuid);
}
EXPORT_SYMBOL(lprocfs_rd_uuid);

int lprocfs_rd_name(struct seq_file *m, void *data)
{
	struct obd_device *dev = data;

	LASSERT(dev != NULL);
	return seq_printf(m, "%s\n", dev->obd_name);
}
EXPORT_SYMBOL(lprocfs_rd_name);

int lprocfs_rd_blksize(struct seq_file *m, void *data)
{
	struct obd_device *obd = data;
=======
static ssize_t uuid_show(struct kobject *kobj, struct attribute *attr,
			 char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);

	return sprintf(buf, "%s\n", obd->obd_uuid.uuid);
}
LUSTRE_RO_ATTR(uuid);

static ssize_t blocksize_show(struct kobject *kobj, struct attribute *attr,
			      char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
>>>>>>> v4.9.227
	struct obd_statfs  osfs;
	int rc = obd_statfs(NULL, obd->obd_self_export, &osfs,
			    cfs_time_shift_64(-OBD_STATFS_CACHE_SECONDS),
			    OBD_STATFS_NODELAY);
	if (!rc)
<<<<<<< HEAD
		rc = seq_printf(m, "%u\n", osfs.os_bsize);
	return rc;
}
EXPORT_SYMBOL(lprocfs_rd_blksize);

int lprocfs_rd_kbytestotal(struct seq_file *m, void *data)
{
	struct obd_device *obd = data;
=======
		return sprintf(buf, "%u\n", osfs.os_bsize);

	return rc;
}
LUSTRE_RO_ATTR(blocksize);

static ssize_t kbytestotal_show(struct kobject *kobj, struct attribute *attr,
				char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
>>>>>>> v4.9.227
	struct obd_statfs  osfs;
	int rc = obd_statfs(NULL, obd->obd_self_export, &osfs,
			    cfs_time_shift_64(-OBD_STATFS_CACHE_SECONDS),
			    OBD_STATFS_NODELAY);
	if (!rc) {
		__u32 blk_size = osfs.os_bsize >> 10;
		__u64 result = osfs.os_blocks;

		while (blk_size >>= 1)
			result <<= 1;

<<<<<<< HEAD
		rc = seq_printf(m, "%llu\n", result);
	}
	return rc;
}
EXPORT_SYMBOL(lprocfs_rd_kbytestotal);

int lprocfs_rd_kbytesfree(struct seq_file *m, void *data)
{
	struct obd_device *obd = data;
=======
		return sprintf(buf, "%llu\n", result);
	}

	return rc;
}
LUSTRE_RO_ATTR(kbytestotal);

static ssize_t kbytesfree_show(struct kobject *kobj, struct attribute *attr,
			       char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
>>>>>>> v4.9.227
	struct obd_statfs  osfs;
	int rc = obd_statfs(NULL, obd->obd_self_export, &osfs,
			    cfs_time_shift_64(-OBD_STATFS_CACHE_SECONDS),
			    OBD_STATFS_NODELAY);
	if (!rc) {
		__u32 blk_size = osfs.os_bsize >> 10;
		__u64 result = osfs.os_bfree;

		while (blk_size >>= 1)
			result <<= 1;

<<<<<<< HEAD
		rc = seq_printf(m, "%llu\n", result);
	}
	return rc;
}
EXPORT_SYMBOL(lprocfs_rd_kbytesfree);

int lprocfs_rd_kbytesavail(struct seq_file *m, void *data)
{
	struct obd_device *obd = data;
=======
		return sprintf(buf, "%llu\n", result);
	}

	return rc;
}
LUSTRE_RO_ATTR(kbytesfree);

static ssize_t kbytesavail_show(struct kobject *kobj, struct attribute *attr,
				char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
>>>>>>> v4.9.227
	struct obd_statfs  osfs;
	int rc = obd_statfs(NULL, obd->obd_self_export, &osfs,
			    cfs_time_shift_64(-OBD_STATFS_CACHE_SECONDS),
			    OBD_STATFS_NODELAY);
	if (!rc) {
		__u32 blk_size = osfs.os_bsize >> 10;
		__u64 result = osfs.os_bavail;

		while (blk_size >>= 1)
			result <<= 1;

<<<<<<< HEAD
		rc = seq_printf(m, "%llu\n", result);
	}
	return rc;
}
EXPORT_SYMBOL(lprocfs_rd_kbytesavail);

int lprocfs_rd_filestotal(struct seq_file *m, void *data)
{
	struct obd_device *obd = data;
=======
		return sprintf(buf, "%llu\n", result);
	}

	return rc;
}
LUSTRE_RO_ATTR(kbytesavail);

static ssize_t filestotal_show(struct kobject *kobj, struct attribute *attr,
			       char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
>>>>>>> v4.9.227
	struct obd_statfs  osfs;
	int rc = obd_statfs(NULL, obd->obd_self_export, &osfs,
			    cfs_time_shift_64(-OBD_STATFS_CACHE_SECONDS),
			    OBD_STATFS_NODELAY);
	if (!rc)
<<<<<<< HEAD
		rc = seq_printf(m, "%llu\n", osfs.os_files);

	return rc;
}
EXPORT_SYMBOL(lprocfs_rd_filestotal);

int lprocfs_rd_filesfree(struct seq_file *m, void *data)
{
	struct obd_device *obd = data;
=======
		return sprintf(buf, "%llu\n", osfs.os_files);

	return rc;
}
LUSTRE_RO_ATTR(filestotal);

static ssize_t filesfree_show(struct kobject *kobj, struct attribute *attr,
			      char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
>>>>>>> v4.9.227
	struct obd_statfs  osfs;
	int rc = obd_statfs(NULL, obd->obd_self_export, &osfs,
			    cfs_time_shift_64(-OBD_STATFS_CACHE_SECONDS),
			    OBD_STATFS_NODELAY);
	if (!rc)
<<<<<<< HEAD
		rc = seq_printf(m, "%llu\n", osfs.os_ffree);
	return rc;
}
EXPORT_SYMBOL(lprocfs_rd_filesfree);
=======
		return sprintf(buf, "%llu\n", osfs.os_ffree);

	return rc;
}
LUSTRE_RO_ATTR(filesfree);
>>>>>>> v4.9.227

int lprocfs_rd_server_uuid(struct seq_file *m, void *data)
{
	struct obd_device *obd = data;
	struct obd_import *imp;
	char *imp_state_name = NULL;
<<<<<<< HEAD
	int rc = 0;

	LASSERT(obd != NULL);
	LPROCFS_CLIMP_CHECK(obd);
	imp = obd->u.cli.cl_import;
	imp_state_name = ptlrpc_import_state_name(imp->imp_state);
	rc = seq_printf(m, "%s\t%s%s\n", obd2cli_tgt(obd), imp_state_name,
			imp->imp_deactive ? "\tDEACTIVATED" : "");

	LPROCFS_CLIMP_EXIT(obd);
	return rc;
=======
	int rc;

	LASSERT(obd);
	rc = lprocfs_climp_check(obd);
	if (rc)
		return rc;

	imp = obd->u.cli.cl_import;
	imp_state_name = ptlrpc_import_state_name(imp->imp_state);
	seq_printf(m, "%s\t%s%s\n",
		   obd2cli_tgt(obd), imp_state_name,
		   imp->imp_deactive ? "\tDEACTIVATED" : "");

	up_read(&obd->u.cli.cl_sem);

	return 0;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(lprocfs_rd_server_uuid);

int lprocfs_rd_conn_uuid(struct seq_file *m, void *data)
{
	struct obd_device *obd = data;
	struct ptlrpc_connection *conn;
<<<<<<< HEAD
	int rc = 0;

	LASSERT(obd != NULL);

	LPROCFS_CLIMP_CHECK(obd);
	conn = obd->u.cli.cl_import->imp_connection;
	if (conn && obd->u.cli.cl_import)
		rc = seq_printf(m, "%s\n", conn->c_remote_uuid.uuid);
	else
		rc = seq_printf(m, "%s\n", "<none>");

	LPROCFS_CLIMP_EXIT(obd);
	return rc;
=======
	int rc;

	LASSERT(obd);

	rc = lprocfs_climp_check(obd);
	if (rc)
		return rc;

	conn = obd->u.cli.cl_import->imp_connection;
	if (conn && obd->u.cli.cl_import)
		seq_printf(m, "%s\n", conn->c_remote_uuid.uuid);
	else
		seq_puts(m, "<none>\n");

	up_read(&obd->u.cli.cl_sem);

	return 0;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(lprocfs_rd_conn_uuid);

/** add up per-cpu counters */
void lprocfs_stats_collect(struct lprocfs_stats *stats, int idx,
			   struct lprocfs_counter *cnt)
{
	unsigned int			num_entry;
	struct lprocfs_counter		*percpu_cntr;
	int				i;
	unsigned long			flags = 0;

	memset(cnt, 0, sizeof(*cnt));

<<<<<<< HEAD
	if (stats == NULL) {
=======
	if (!stats) {
>>>>>>> v4.9.227
		/* set count to 1 to avoid divide-by-zero errs in callers */
		cnt->lc_count = 1;
		return;
	}

	cnt->lc_min = LC_MIN_INIT;

	num_entry = lprocfs_stats_lock(stats, LPROCFS_GET_NUM_CPU, &flags);

	for (i = 0; i < num_entry; i++) {
<<<<<<< HEAD
		if (stats->ls_percpu[i] == NULL)
=======
		if (!stats->ls_percpu[i])
>>>>>>> v4.9.227
			continue;
		percpu_cntr = lprocfs_stats_counter_get(stats, i, idx);

		cnt->lc_count += percpu_cntr->lc_count;
		cnt->lc_sum += percpu_cntr->lc_sum;
		if (percpu_cntr->lc_min < cnt->lc_min)
			cnt->lc_min = percpu_cntr->lc_min;
		if (percpu_cntr->lc_max > cnt->lc_max)
			cnt->lc_max = percpu_cntr->lc_max;
		cnt->lc_sumsquare += percpu_cntr->lc_sumsquare;
	}

	lprocfs_stats_unlock(stats, LPROCFS_GET_NUM_CPU, &flags);
}
<<<<<<< HEAD
EXPORT_SYMBOL(lprocfs_stats_collect);
=======
>>>>>>> v4.9.227

/**
 * Append a space separated list of current set flags to str.
 */
#define flag2str(flag, first)						\
	do {								\
		if (imp->imp_##flag)					\
<<<<<<< HEAD
		     seq_printf(m, "%s" #flag, first ? "" : ", ");	\
=======
			seq_printf(m, "%s" #flag, first ? "" : ", ");	\
>>>>>>> v4.9.227
	} while (0)
static int obd_import_flags2str(struct obd_import *imp, struct seq_file *m)
{
	bool first = true;

	if (imp->imp_obd->obd_no_recov) {
		seq_printf(m, "no_recov");
		first = false;
	}

	flag2str(invalid, first);
	first = false;
	flag2str(deactive, first);
	flag2str(replayable, first);
	flag2str(pingable, first);
	return 0;
}
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
#undef flags2str

static void obd_connect_seq_flags2str(struct seq_file *m, __u64 flags, char *sep)
{
	__u64 mask = 1;
	int i;
	bool first = true;

<<<<<<< HEAD
	for (i = 0; obd_connect_names[i] != NULL; i++, mask <<= 1) {
		if (flags & mask) {
			seq_printf(m, "%s%s",
					first ? sep : "", obd_connect_names[i]);
=======
	for (i = 0; obd_connect_names[i]; i++, mask <<= 1) {
		if (flags & mask) {
			seq_printf(m, "%s%s",
				   first ? sep : "", obd_connect_names[i]);
>>>>>>> v4.9.227
			first = false;
		}
	}
	if (flags & ~(mask - 1))
		seq_printf(m, "%sunknown flags %#llx",
<<<<<<< HEAD
				first ? sep : "", flags & ~(mask - 1));
=======
			   first ? sep : "", flags & ~(mask - 1));
>>>>>>> v4.9.227
}

int lprocfs_rd_import(struct seq_file *m, void *data)
{
<<<<<<< HEAD
	struct lprocfs_counter		ret;
	struct lprocfs_counter_header	*header;
	struct obd_device		*obd	= (struct obd_device *)data;
	struct obd_import		*imp;
	struct obd_import_conn		*conn;
	int				j;
	int				k;
	int				rw	= 0;

	LASSERT(obd != NULL);
	LPROCFS_CLIMP_CHECK(obd);
	imp = obd->u.cli.cl_import;

	seq_printf(m,
		     "import:\n"
		     "    name: %s\n"
		     "    target: %s\n"
		     "    state: %s\n"
		     "    instance: %u\n"
		     "    connect_flags: [",
		     obd->obd_name,
		     obd2cli_tgt(obd),
		     ptlrpc_import_state_name(imp->imp_state),
		     imp->imp_connect_data.ocd_instance);
	obd_connect_seq_flags2str(m, imp->imp_connect_data.ocd_connect_flags, ", ");
	seq_printf(m,
		      "]\n"
		      "    import_flags: [");
	obd_import_flags2str(imp, m);

	seq_printf(m,
		      "]\n"
		      "    connection:\n"
		      "       failover_nids: [");
	spin_lock(&imp->imp_lock);
	j = 0;
	list_for_each_entry(conn, &imp->imp_conn_list, oic_item) {
		seq_printf(m, "%s%s", j ? ", " : "",
			   libcfs_nid2str(conn->oic_conn->c_peer.nid));
		j++;
	}
	seq_printf(m,
		      "]\n"
		      "       current_connection: %s\n"
		      "       connection_attempts: %u\n"
		      "       generation: %u\n"
		      "       in-progress_invalidations: %u\n",
		      imp->imp_connection == NULL ? "<none>" :
			      libcfs_nid2str(imp->imp_connection->c_peer.nid),
		      imp->imp_conn_cnt,
		      imp->imp_generation,
		      atomic_read(&imp->imp_inval_count));
	spin_unlock(&imp->imp_lock);

	if (obd->obd_svc_stats == NULL)
=======
	char				nidstr[LNET_NIDSTR_SIZE];
	struct lprocfs_counter		ret;
	struct lprocfs_counter_header	*header;
	struct obd_device		*obd	= data;
	struct obd_import		*imp;
	struct obd_import_conn		*conn;
	struct obd_connect_data *ocd;
	int				j;
	int				k;
	int				rw	= 0;
	int				rc;

	LASSERT(obd);
	rc = lprocfs_climp_check(obd);
	if (rc)
		return rc;

	imp = obd->u.cli.cl_import;
	ocd = &imp->imp_connect_data;

	seq_printf(m, "import:\n"
		   "    name: %s\n"
		   "    target: %s\n"
		   "    state: %s\n"
		   "    instance: %u\n"
		   "    connect_flags: [ ",
		   obd->obd_name,
		   obd2cli_tgt(obd),
		   ptlrpc_import_state_name(imp->imp_state),
		   imp->imp_connect_data.ocd_instance);
	obd_connect_seq_flags2str(m, imp->imp_connect_data.ocd_connect_flags,
				  ", ");
	seq_printf(m, " ]\n");
	obd_connect_data_seqprint(m, ocd);
	seq_printf(m, "    import_flags: [ ");
	obd_import_flags2str(imp, m);

	seq_printf(m,
		   " ]\n"
		   "    connection:\n"
		   "       failover_nids: [ ");
	spin_lock(&imp->imp_lock);
	j = 0;
	list_for_each_entry(conn, &imp->imp_conn_list, oic_item) {
		libcfs_nid2str_r(conn->oic_conn->c_peer.nid,
				 nidstr, sizeof(nidstr));
		seq_printf(m, "%s%s", j ? ", " : "", nidstr);
		j++;
	}
	if (imp->imp_connection)
		libcfs_nid2str_r(imp->imp_connection->c_peer.nid,
				 nidstr, sizeof(nidstr));
	else
		strncpy(nidstr, "<none>", sizeof(nidstr));
	seq_printf(m,
		   " ]\n"
		   "       current_connection: %s\n"
		   "       connection_attempts: %u\n"
		   "       generation: %u\n"
		   "       in-progress_invalidations: %u\n",
		   nidstr,
		   imp->imp_conn_cnt,
		   imp->imp_generation,
		   atomic_read(&imp->imp_inval_count));
	spin_unlock(&imp->imp_lock);

	if (!obd->obd_svc_stats)
>>>>>>> v4.9.227
		goto out_climp;

	header = &obd->obd_svc_stats->ls_cnt_header[PTLRPC_REQWAIT_CNTR];
	lprocfs_stats_collect(obd->obd_svc_stats, PTLRPC_REQWAIT_CNTR, &ret);
	if (ret.lc_count != 0) {
		/* first argument to do_div MUST be __u64 */
		__u64 sum = ret.lc_sum;
<<<<<<< HEAD
		do_div(sum, ret.lc_count);
		ret.lc_sum = sum;
	} else
		ret.lc_sum = 0;
	seq_printf(m,
		      "    rpcs:\n"
		      "       inflight: %u\n"
		      "       unregistering: %u\n"
		      "       timeouts: %u\n"
		      "       avg_waittime: %llu %s\n",
		      atomic_read(&imp->imp_inflight),
		      atomic_read(&imp->imp_unregistering),
		      atomic_read(&imp->imp_timeouts),
		      ret.lc_sum, header->lc_units);
=======

		do_div(sum, ret.lc_count);
		ret.lc_sum = sum;
	} else {
		ret.lc_sum = 0;
	}
	seq_printf(m,
		   "    rpcs:\n"
		   "       inflight: %u\n"
		   "       unregistering: %u\n"
		   "       timeouts: %u\n"
		   "       avg_waittime: %llu %s\n",
		   atomic_read(&imp->imp_inflight),
		   atomic_read(&imp->imp_unregistering),
		   atomic_read(&imp->imp_timeouts),
		   ret.lc_sum, header->lc_units);
>>>>>>> v4.9.227

	k = 0;
	for (j = 0; j < IMP_AT_MAX_PORTALS; j++) {
		if (imp->imp_at.iat_portal[j] == 0)
			break;
		k = max_t(unsigned int, k,
			  at_get(&imp->imp_at.iat_service_estimate[j]));
	}
	seq_printf(m,
<<<<<<< HEAD
		      "    service_estimates:\n"
		      "       services: %u sec\n"
		      "       network: %u sec\n",
		      k,
		      at_get(&imp->imp_at.iat_net_latency));

	seq_printf(m,
		      "    transactions:\n"
		      "       last_replay: %llu\n"
		      "       peer_committed: %llu\n"
		      "       last_checked: %llu\n",
		      imp->imp_last_replay_transno,
		      imp->imp_peer_committed_transno,
		      imp->imp_last_transno_checked);
=======
		   "    service_estimates:\n"
		   "       services: %u sec\n"
		   "       network: %u sec\n",
		   k,
		   at_get(&imp->imp_at.iat_net_latency));

	seq_printf(m,
		   "    transactions:\n"
		   "       last_replay: %llu\n"
		   "       peer_committed: %llu\n"
		   "       last_checked: %llu\n",
		   imp->imp_last_replay_transno,
		   imp->imp_peer_committed_transno,
		   imp->imp_last_transno_checked);
>>>>>>> v4.9.227

	/* avg data rates */
	for (rw = 0; rw <= 1; rw++) {
		lprocfs_stats_collect(obd->obd_svc_stats,
				      PTLRPC_LAST_CNTR + BRW_READ_BYTES + rw,
				      &ret);
		if (ret.lc_sum > 0 && ret.lc_count > 0) {
			/* first argument to do_div MUST be __u64 */
			__u64 sum = ret.lc_sum;
<<<<<<< HEAD
			do_div(sum, ret.lc_count);
			ret.lc_sum = sum;
			seq_printf(m,
				      "    %s_data_averages:\n"
				      "       bytes_per_rpc: %llu\n",
				      rw ? "write" : "read",
				      ret.lc_sum);
=======

			do_div(sum, ret.lc_count);
			ret.lc_sum = sum;
			seq_printf(m,
				   "    %s_data_averages:\n"
				   "       bytes_per_rpc: %llu\n",
				   rw ? "write" : "read",
				   ret.lc_sum);
>>>>>>> v4.9.227
		}
		k = (int)ret.lc_sum;
		j = opcode_offset(OST_READ + rw) + EXTRA_MAX_OPCODES;
		header = &obd->obd_svc_stats->ls_cnt_header[j];
		lprocfs_stats_collect(obd->obd_svc_stats, j, &ret);
		if (ret.lc_sum > 0 && ret.lc_count != 0) {
			/* first argument to do_div MUST be __u64 */
			__u64 sum = ret.lc_sum;
<<<<<<< HEAD
			do_div(sum, ret.lc_count);
			ret.lc_sum = sum;
			seq_printf(m,
				      "       %s_per_rpc: %llu\n",
				      header->lc_units, ret.lc_sum);
			j = (int)ret.lc_sum;
			if (j > 0)
				seq_printf(m,
					      "       MB_per_sec: %u.%.02u\n",
					      k / j, (100 * k / j) % 100);
=======

			do_div(sum, ret.lc_count);
			ret.lc_sum = sum;
			seq_printf(m,
				   "       %s_per_rpc: %llu\n",
				   header->lc_units, ret.lc_sum);
			j = (int)ret.lc_sum;
			if (j > 0)
				seq_printf(m,
					   "       MB_per_sec: %u.%.02u\n",
					   k / j, (100 * k / j) % 100);
>>>>>>> v4.9.227
		}
	}

out_climp:
<<<<<<< HEAD
	LPROCFS_CLIMP_EXIT(obd);
=======
	up_read(&obd->u.cli.cl_sem);
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL(lprocfs_rd_import);

int lprocfs_rd_state(struct seq_file *m, void *data)
{
<<<<<<< HEAD
	struct obd_device *obd = (struct obd_device *)data;
	struct obd_import *imp;
	int j, k;

	LASSERT(obd != NULL);
	LPROCFS_CLIMP_CHECK(obd);
	imp = obd->u.cli.cl_import;

	seq_printf(m, "current_state: %s\n",
		     ptlrpc_import_state_name(imp->imp_state));
=======
	struct obd_device *obd = data;
	struct obd_import *imp;
	int j, k, rc;

	LASSERT(obd);
	rc = lprocfs_climp_check(obd);
	if (rc)
		return rc;

	imp = obd->u.cli.cl_import;

	seq_printf(m, "current_state: %s\n",
		   ptlrpc_import_state_name(imp->imp_state));
>>>>>>> v4.9.227
	seq_printf(m, "state_history:\n");
	k = imp->imp_state_hist_idx;
	for (j = 0; j < IMP_STATE_HIST_LEN; j++) {
		struct import_state_hist *ish =
			&imp->imp_state_hist[(k + j) % IMP_STATE_HIST_LEN];
		if (ish->ish_state == 0)
			continue;
<<<<<<< HEAD
		seq_printf(m, " - ["CFS_TIME_T", %s]\n",
			      ish->ish_time,
			      ptlrpc_import_state_name(ish->ish_state));
	}

	LPROCFS_CLIMP_EXIT(obd);
=======
		seq_printf(m, " - [ %lld, %s ]\n", (s64)ish->ish_time,
			   ptlrpc_import_state_name(ish->ish_state));
	}

	up_read(&obd->u.cli.cl_sem);
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL(lprocfs_rd_state);

int lprocfs_at_hist_helper(struct seq_file *m, struct adaptive_timeout *at)
{
	int i;
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
	for (i = 0; i < AT_BINS; i++)
		seq_printf(m, "%3u ", at->at_hist[i]);
	seq_printf(m, "\n");
	return 0;
}
EXPORT_SYMBOL(lprocfs_at_hist_helper);

/* See also ptlrpc_lprocfs_rd_timeouts */
int lprocfs_rd_timeouts(struct seq_file *m, void *data)
{
<<<<<<< HEAD
	struct obd_device *obd = (struct obd_device *)data;
	struct obd_import *imp;
	unsigned int cur, worst;
	time_t now, worstt;
	struct dhms ts;
	int i;

	LASSERT(obd != NULL);
	LPROCFS_CLIMP_CHECK(obd);
	imp = obd->u.cli.cl_import;

	now = get_seconds();

	/* Some network health info for kicks */
	s2dhms(&ts, now - imp->imp_last_reply_time);
	seq_printf(m, "%-10s : %ld, "DHMS_FMT" ago\n",
		       "last reply", imp->imp_last_reply_time, DHMS_VARS(&ts));
=======
	struct obd_device *obd = data;
	struct obd_import *imp;
	unsigned int cur, worst;
	time64_t now, worstt;
	struct dhms ts;
	int i, rc;

	LASSERT(obd);
	rc = lprocfs_climp_check(obd);
	if (rc)
		return rc;

	imp = obd->u.cli.cl_import;

	now = ktime_get_real_seconds();

	/* Some network health info for kicks */
	s2dhms(&ts, now - imp->imp_last_reply_time);
	seq_printf(m, "%-10s : %lld, " DHMS_FMT " ago\n",
		   "last reply", (s64)imp->imp_last_reply_time, DHMS_VARS(&ts));
>>>>>>> v4.9.227

	cur = at_get(&imp->imp_at.iat_net_latency);
	worst = imp->imp_at.iat_net_latency.at_worst_ever;
	worstt = imp->imp_at.iat_net_latency.at_worst_time;
	s2dhms(&ts, now - worstt);
<<<<<<< HEAD
	seq_printf(m, "%-10s : cur %3u  worst %3u (at %ld, "DHMS_FMT" ago) ",
		       "network", cur, worst, worstt, DHMS_VARS(&ts));
=======
	seq_printf(m, "%-10s : cur %3u  worst %3u (at %lld, " DHMS_FMT " ago) ",
		   "network", cur, worst, (s64)worstt, DHMS_VARS(&ts));
>>>>>>> v4.9.227
	lprocfs_at_hist_helper(m, &imp->imp_at.iat_net_latency);

	for (i = 0; i < IMP_AT_MAX_PORTALS; i++) {
		if (imp->imp_at.iat_portal[i] == 0)
			break;
		cur = at_get(&imp->imp_at.iat_service_estimate[i]);
		worst = imp->imp_at.iat_service_estimate[i].at_worst_ever;
		worstt = imp->imp_at.iat_service_estimate[i].at_worst_time;
		s2dhms(&ts, now - worstt);
<<<<<<< HEAD
		seq_printf(m, "portal %-2d  : cur %3u  worst %3u (at %ld, "
			       DHMS_FMT" ago) ", imp->imp_at.iat_portal[i],
			       cur, worst, worstt, DHMS_VARS(&ts));
		lprocfs_at_hist_helper(m, &imp->imp_at.iat_service_estimate[i]);
	}

	LPROCFS_CLIMP_EXIT(obd);
=======
		seq_printf(m, "portal %-2d  : cur %3u  worst %3u (at %lld, "
			   DHMS_FMT " ago) ", imp->imp_at.iat_portal[i],
			   cur, worst, (s64)worstt, DHMS_VARS(&ts));
		lprocfs_at_hist_helper(m, &imp->imp_at.iat_service_estimate[i]);
	}

	up_read(&obd->u.cli.cl_sem);
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL(lprocfs_rd_timeouts);

int lprocfs_rd_connect_flags(struct seq_file *m, void *data)
{
	struct obd_device *obd = data;
	__u64 flags;
<<<<<<< HEAD

	LPROCFS_CLIMP_CHECK(obd);
=======
	int rc;

	rc = lprocfs_climp_check(obd);
	if (rc)
		return rc;

>>>>>>> v4.9.227
	flags = obd->u.cli.cl_import->imp_connect_data.ocd_connect_flags;
	seq_printf(m, "flags=%#llx\n", flags);
	obd_connect_seq_flags2str(m, flags, "\n");
	seq_printf(m, "\n");
<<<<<<< HEAD
	LPROCFS_CLIMP_EXIT(obd);
=======
	up_read(&obd->u.cli.cl_sem);
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL(lprocfs_rd_connect_flags);

<<<<<<< HEAD
int lprocfs_rd_num_exports(struct seq_file *m, void *data)
{
	struct obd_device *obd = data;

	LASSERT(obd != NULL);
	return seq_printf(m, "%u\n", obd->obd_num_exports);
}
EXPORT_SYMBOL(lprocfs_rd_num_exports);

int lprocfs_rd_numrefs(struct seq_file *m, void *data)
{
	struct obd_type *class = (struct obd_type *) data;

	LASSERT(class != NULL);
	return seq_printf(m, "%d\n", class->typ_refcnt);
}
EXPORT_SYMBOL(lprocfs_rd_numrefs);

int lprocfs_obd_setup(struct obd_device *obd, struct lprocfs_vars *list)
{
	int rc = 0;

	LASSERT(obd != NULL);
	LASSERT(obd->obd_magic == OBD_DEVICE_MAGIC);
	LASSERT(obd->obd_type->typ_procroot != NULL);

	obd->obd_proc_entry = lprocfs_register(obd->obd_name,
					       obd->obd_type->typ_procroot,
					       list, obd);
	if (IS_ERR(obd->obd_proc_entry)) {
		rc = PTR_ERR(obd->obd_proc_entry);
		CERROR("error %d setting up lprocfs for %s\n",
		       rc, obd->obd_name);
		obd->obd_proc_entry = NULL;
	}
	return rc;
}
EXPORT_SYMBOL(lprocfs_obd_setup);
=======
static struct attribute *obd_def_attrs[] = {
	&lustre_attr_blocksize.attr,
	&lustre_attr_kbytestotal.attr,
	&lustre_attr_kbytesfree.attr,
	&lustre_attr_kbytesavail.attr,
	&lustre_attr_filestotal.attr,
	&lustre_attr_filesfree.attr,
	&lustre_attr_uuid.attr,
	NULL,
};

static void obd_sysfs_release(struct kobject *kobj)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);

	complete(&obd->obd_kobj_unregister);
}

static struct kobj_type obd_ktype = {
	.default_attrs	= obd_def_attrs,
	.sysfs_ops	= &lustre_sysfs_ops,
	.release	= obd_sysfs_release,
};

int lprocfs_obd_setup(struct obd_device *obd, struct lprocfs_vars *list,
		      struct attribute_group *attrs)
{
	int rc = 0;

	init_completion(&obd->obd_kobj_unregister);
	rc = kobject_init_and_add(&obd->obd_kobj, &obd_ktype,
				  obd->obd_type->typ_kobj,
				  "%s", obd->obd_name);
	if (rc)
		return rc;

	if (attrs) {
		rc = sysfs_create_group(&obd->obd_kobj, attrs);
		if (rc) {
			kobject_put(&obd->obd_kobj);
			return rc;
		}
	}

	obd->obd_debugfs_entry = ldebugfs_register(obd->obd_name,
						   obd->obd_type->typ_debugfs_entry,
						   list, obd);
	if (IS_ERR_OR_NULL(obd->obd_debugfs_entry)) {
		rc = obd->obd_debugfs_entry ? PTR_ERR(obd->obd_debugfs_entry)
					    : -ENOMEM;
		CERROR("error %d setting up lprocfs for %s\n",
		       rc, obd->obd_name);
		obd->obd_debugfs_entry = NULL;
	}

	return rc;
}
EXPORT_SYMBOL_GPL(lprocfs_obd_setup);
>>>>>>> v4.9.227

int lprocfs_obd_cleanup(struct obd_device *obd)
{
	if (!obd)
		return -EINVAL;
<<<<<<< HEAD
	if (obd->obd_proc_exports_entry) {
		/* Should be no exports left */
		lprocfs_remove(&obd->obd_proc_exports_entry);
		obd->obd_proc_exports_entry = NULL;
	}
	if (obd->obd_proc_entry) {
		lprocfs_remove(&obd->obd_proc_entry);
		obd->obd_proc_entry = NULL;
	}
	return 0;
}
EXPORT_SYMBOL(lprocfs_obd_cleanup);

static void lprocfs_free_client_stats(struct nid_stat *client_stat)
{
	CDEBUG(D_CONFIG, "stat %p - data %p/%p\n", client_stat,
	       client_stat->nid_proc, client_stat->nid_stats);

	LASSERTF(atomic_read(&client_stat->nid_exp_ref_count) == 0,
		 "nid %s:count %d\n", libcfs_nid2str(client_stat->nid),
		 atomic_read(&client_stat->nid_exp_ref_count));

	if (client_stat->nid_proc)
		lprocfs_remove(&client_stat->nid_proc);

	if (client_stat->nid_stats)
		lprocfs_free_stats(&client_stat->nid_stats);

	if (client_stat->nid_ldlm_stats)
		lprocfs_free_stats(&client_stat->nid_ldlm_stats);

	OBD_FREE_PTR(client_stat);
	return;

}

void lprocfs_free_per_client_stats(struct obd_device *obd)
{
	struct cfs_hash *hash = obd->obd_nid_stats_hash;
	struct nid_stat *stat;

	/* we need extra list - because hash_exit called to early */
	/* not need locking because all clients is died */
	while (!list_empty(&obd->obd_nid_stats)) {
		stat = list_entry(obd->obd_nid_stats.next,
				      struct nid_stat, nid_list);
		list_del_init(&stat->nid_list);
		cfs_hash_del(hash, &stat->nid, &stat->nid_hash);
		lprocfs_free_client_stats(stat);
	}
}
EXPORT_SYMBOL(lprocfs_free_per_client_stats);
=======

	if (!IS_ERR_OR_NULL(obd->obd_debugfs_entry))
		ldebugfs_remove(&obd->obd_debugfs_entry);

	kobject_put(&obd->obd_kobj);
	wait_for_completion(&obd->obd_kobj_unregister);

	return 0;
}
EXPORT_SYMBOL_GPL(lprocfs_obd_cleanup);
>>>>>>> v4.9.227

int lprocfs_stats_alloc_one(struct lprocfs_stats *stats, unsigned int cpuid)
{
	struct lprocfs_counter  *cntr;
	unsigned int            percpusize;
	int                     rc = -ENOMEM;
	unsigned long           flags = 0;
	int                     i;

<<<<<<< HEAD
	LASSERT(stats->ls_percpu[cpuid] == NULL);
=======
	LASSERT(!stats->ls_percpu[cpuid]);
>>>>>>> v4.9.227
	LASSERT((stats->ls_flags & LPROCFS_STATS_FLAG_NOPERCPU) == 0);

	percpusize = lprocfs_stats_counter_size(stats);
	LIBCFS_ALLOC_ATOMIC(stats->ls_percpu[cpuid], percpusize);
<<<<<<< HEAD
	if (stats->ls_percpu[cpuid] != NULL) {
=======
	if (stats->ls_percpu[cpuid]) {
>>>>>>> v4.9.227
		rc = 0;
		if (unlikely(stats->ls_biggest_alloc_num <= cpuid)) {
			if (stats->ls_flags & LPROCFS_STATS_FLAG_IRQ_SAFE)
				spin_lock_irqsave(&stats->ls_lock, flags);
			else
				spin_lock(&stats->ls_lock);
			if (stats->ls_biggest_alloc_num <= cpuid)
				stats->ls_biggest_alloc_num = cpuid + 1;
			if (stats->ls_flags & LPROCFS_STATS_FLAG_IRQ_SAFE)
				spin_unlock_irqrestore(&stats->ls_lock, flags);
			else
				spin_unlock(&stats->ls_lock);
		}
		/* initialize the ls_percpu[cpuid] non-zero counter */
		for (i = 0; i < stats->ls_num; ++i) {
			cntr = lprocfs_stats_counter_get(stats, cpuid, i);
			cntr->lc_min = LC_MIN_INIT;
		}
	}
	return rc;
}
<<<<<<< HEAD
EXPORT_SYMBOL(lprocfs_stats_alloc_one);
=======
>>>>>>> v4.9.227

struct lprocfs_stats *lprocfs_alloc_stats(unsigned int num,
					  enum lprocfs_stats_flags flags)
{
	struct lprocfs_stats	*stats;
	unsigned int		num_entry;
	unsigned int		percpusize = 0;
	int			i;

	if (num == 0)
		return NULL;

	if (lprocfs_no_percpu_stats != 0)
		flags |= LPROCFS_STATS_FLAG_NOPERCPU;

	if (flags & LPROCFS_STATS_FLAG_NOPERCPU)
		num_entry = 1;
	else
		num_entry = num_possible_cpus();

	/* alloc percpu pointers for all possible cpu slots */
	LIBCFS_ALLOC(stats, offsetof(typeof(*stats), ls_percpu[num_entry]));
<<<<<<< HEAD
	if (stats == NULL)
=======
	if (!stats)
>>>>>>> v4.9.227
		return NULL;

	stats->ls_num = num;
	stats->ls_flags = flags;
	spin_lock_init(&stats->ls_lock);

	/* alloc num of counter headers */
	LIBCFS_ALLOC(stats->ls_cnt_header,
		     stats->ls_num * sizeof(struct lprocfs_counter_header));
<<<<<<< HEAD
	if (stats->ls_cnt_header == NULL)
=======
	if (!stats->ls_cnt_header)
>>>>>>> v4.9.227
		goto fail;

	if ((flags & LPROCFS_STATS_FLAG_NOPERCPU) != 0) {
		/* contains only one set counters */
		percpusize = lprocfs_stats_counter_size(stats);
		LIBCFS_ALLOC_ATOMIC(stats->ls_percpu[0], percpusize);
<<<<<<< HEAD
		if (stats->ls_percpu[0] == NULL)
			goto fail;
		stats->ls_biggest_alloc_num = 1;
	} else if ((flags & LPROCFS_STATS_FLAG_IRQ_SAFE) != 0) {
		/* alloc all percpu data, currently only obd_memory use this */
=======
		if (!stats->ls_percpu[0])
			goto fail;
		stats->ls_biggest_alloc_num = 1;
	} else if ((flags & LPROCFS_STATS_FLAG_IRQ_SAFE) != 0) {
		/* alloc all percpu data */
>>>>>>> v4.9.227
		for (i = 0; i < num_entry; ++i)
			if (lprocfs_stats_alloc_one(stats, i) < 0)
				goto fail;
	}

	return stats;

fail:
	lprocfs_free_stats(&stats);
	return NULL;
}
EXPORT_SYMBOL(lprocfs_alloc_stats);

void lprocfs_free_stats(struct lprocfs_stats **statsh)
{
	struct lprocfs_stats *stats = *statsh;
	unsigned int num_entry;
	unsigned int percpusize;
	unsigned int i;

<<<<<<< HEAD
	if (stats == NULL || stats->ls_num == 0)
=======
	if (!stats || stats->ls_num == 0)
>>>>>>> v4.9.227
		return;
	*statsh = NULL;

	if (stats->ls_flags & LPROCFS_STATS_FLAG_NOPERCPU)
		num_entry = 1;
	else
		num_entry = num_possible_cpus();

	percpusize = lprocfs_stats_counter_size(stats);
	for (i = 0; i < num_entry; i++)
<<<<<<< HEAD
		if (stats->ls_percpu[i] != NULL)
			LIBCFS_FREE(stats->ls_percpu[i], percpusize);
	if (stats->ls_cnt_header != NULL)
=======
		if (stats->ls_percpu[i])
			LIBCFS_FREE(stats->ls_percpu[i], percpusize);
	if (stats->ls_cnt_header)
>>>>>>> v4.9.227
		LIBCFS_FREE(stats->ls_cnt_header, stats->ls_num *
					sizeof(struct lprocfs_counter_header));
	LIBCFS_FREE(stats, offsetof(typeof(*stats), ls_percpu[num_entry]));
}
EXPORT_SYMBOL(lprocfs_free_stats);

void lprocfs_clear_stats(struct lprocfs_stats *stats)
{
	struct lprocfs_counter		*percpu_cntr;
	int				i;
	int				j;
	unsigned int			num_entry;
	unsigned long			flags = 0;

	num_entry = lprocfs_stats_lock(stats, LPROCFS_GET_NUM_CPU, &flags);

	for (i = 0; i < num_entry; i++) {
<<<<<<< HEAD
		if (stats->ls_percpu[i] == NULL)
=======
		if (!stats->ls_percpu[i])
>>>>>>> v4.9.227
			continue;
		for (j = 0; j < stats->ls_num; j++) {
			percpu_cntr = lprocfs_stats_counter_get(stats, i, j);
			percpu_cntr->lc_count		= 0;
			percpu_cntr->lc_min		= LC_MIN_INIT;
			percpu_cntr->lc_max		= 0;
			percpu_cntr->lc_sumsquare	= 0;
			percpu_cntr->lc_sum		= 0;
			if (stats->ls_flags & LPROCFS_STATS_FLAG_IRQ_SAFE)
				percpu_cntr->lc_sum_irq	= 0;
		}
	}

	lprocfs_stats_unlock(stats, LPROCFS_GET_NUM_CPU, &flags);
}
EXPORT_SYMBOL(lprocfs_clear_stats);

static ssize_t lprocfs_stats_seq_write(struct file *file,
				       const char __user *buf,
				       size_t len, loff_t *off)
{
	struct seq_file *seq = file->private_data;
	struct lprocfs_stats *stats = seq->private;

	lprocfs_clear_stats(stats);

	return len;
}

static void *lprocfs_stats_seq_start(struct seq_file *p, loff_t *pos)
{
	struct lprocfs_stats *stats = p->private;

	return (*pos < stats->ls_num) ? pos : NULL;
}

static void lprocfs_stats_seq_stop(struct seq_file *p, void *v)
{
}

static void *lprocfs_stats_seq_next(struct seq_file *p, void *v, loff_t *pos)
{
	(*pos)++;
	return lprocfs_stats_seq_start(p, pos);
}

/* seq file export of one lprocfs counter */
static int lprocfs_stats_seq_show(struct seq_file *p, void *v)
{
	struct lprocfs_stats		*stats	= p->private;
	struct lprocfs_counter_header   *hdr;
	struct lprocfs_counter           ctr;
	int                              idx    = *(loff_t *)v;
<<<<<<< HEAD
	int                              rc     = 0;

	if (idx == 0) {
		struct timeval now;
		do_gettimeofday(&now);
		rc = seq_printf(p, "%-25s %lu.%lu secs.usecs\n",
				"snapshot_time", now.tv_sec, (unsigned long)now.tv_usec);
		if (rc < 0)
			return rc;
	}
	hdr = &stats->ls_cnt_header[idx];
	lprocfs_stats_collect(stats, idx, &ctr);

	if (ctr.lc_count == 0)
		goto out;

	rc = seq_printf(p, "%-25s %lld samples [%s]", hdr->lc_name,
			ctr.lc_count, hdr->lc_units);

	if (rc < 0)
		goto out;

	if ((hdr->lc_config & LPROCFS_CNTR_AVGMINMAX) && (ctr.lc_count > 0)) {
		rc = seq_printf(p, " %lld %lld %lld",
				ctr.lc_min, ctr.lc_max, ctr.lc_sum);
		if (rc < 0)
			goto out;
		if (hdr->lc_config & LPROCFS_CNTR_STDDEV)
			rc = seq_printf(p, " %lld", ctr.lc_sumsquare);
		if (rc < 0)
			goto out;
	}
	rc = seq_printf(p, "\n");
out:
	return (rc < 0) ? rc : 0;
=======

	if (idx == 0) {
		struct timespec64 now;

		ktime_get_real_ts64(&now);
		seq_printf(p, "%-25s %llu.%9lu secs.usecs\n",
			   "snapshot_time",
			   (s64)now.tv_sec, (unsigned long)now.tv_nsec);
	}

	hdr = &stats->ls_cnt_header[idx];
	lprocfs_stats_collect(stats, idx, &ctr);

	if (ctr.lc_count != 0) {
		seq_printf(p, "%-25s %lld samples [%s]",
			   hdr->lc_name, ctr.lc_count, hdr->lc_units);

		if ((hdr->lc_config & LPROCFS_CNTR_AVGMINMAX) &&
		    (ctr.lc_count > 0)) {
			seq_printf(p, " %lld %lld %lld",
				   ctr.lc_min, ctr.lc_max, ctr.lc_sum);
			if (hdr->lc_config & LPROCFS_CNTR_STDDEV)
				seq_printf(p, " %lld", ctr.lc_sumsquare);
		}
		seq_putc(p, '\n');
	}

	return 0;
>>>>>>> v4.9.227
}

static const struct seq_operations lprocfs_stats_seq_sops = {
	.start	= lprocfs_stats_seq_start,
	.stop	= lprocfs_stats_seq_stop,
	.next	= lprocfs_stats_seq_next,
	.show	= lprocfs_stats_seq_show,
};

static int lprocfs_stats_seq_open(struct inode *inode, struct file *file)
{
	struct seq_file *seq;
	int rc;

	rc = seq_open(file, &lprocfs_stats_seq_sops);
	if (rc)
		return rc;
<<<<<<< HEAD
	seq = file->private_data;
	seq->private = PDE_DATA(inode);
	return 0;
}

struct file_operations lprocfs_stats_seq_fops = {
=======

	seq = file->private_data;
	seq->private = inode->i_private;

	return 0;
}

static const struct file_operations lprocfs_stats_seq_fops = {
>>>>>>> v4.9.227
	.owner   = THIS_MODULE,
	.open    = lprocfs_stats_seq_open,
	.read    = seq_read,
	.write   = lprocfs_stats_seq_write,
	.llseek  = seq_lseek,
	.release = lprocfs_seq_release,
};

<<<<<<< HEAD
int lprocfs_register_stats(struct proc_dir_entry *root, const char *name,
			   struct lprocfs_stats *stats)
{
	struct proc_dir_entry *entry;
	LASSERT(root != NULL);

	entry = proc_create_data(name, 0644, root,
				 &lprocfs_stats_seq_fops, stats);
	if (entry == NULL)
		return -ENOMEM;

	return 0;
}
EXPORT_SYMBOL(lprocfs_register_stats);
=======
int ldebugfs_register_stats(struct dentry *parent, const char *name,
			    struct lprocfs_stats *stats)
{
	struct dentry *entry;

	LASSERT(!IS_ERR_OR_NULL(parent));

	entry = debugfs_create_file(name, 0644, parent, stats,
				    &lprocfs_stats_seq_fops);
	if (IS_ERR_OR_NULL(entry))
		return entry ? PTR_ERR(entry) : -ENOMEM;

	return 0;
}
EXPORT_SYMBOL_GPL(ldebugfs_register_stats);
>>>>>>> v4.9.227

void lprocfs_counter_init(struct lprocfs_stats *stats, int index,
			  unsigned conf, const char *name, const char *units)
{
	struct lprocfs_counter_header	*header;
	struct lprocfs_counter		*percpu_cntr;
	unsigned long			flags = 0;
	unsigned int			i;
	unsigned int			num_cpu;

<<<<<<< HEAD
	LASSERT(stats != NULL);

	header = &stats->ls_cnt_header[index];
	LASSERTF(header != NULL, "Failed to allocate stats header:[%d]%s/%s\n",
=======
	header = &stats->ls_cnt_header[index];
	LASSERTF(header, "Failed to allocate stats header:[%d]%s/%s\n",
>>>>>>> v4.9.227
		 index, name, units);

	header->lc_config = conf;
	header->lc_name   = name;
	header->lc_units  = units;

	num_cpu = lprocfs_stats_lock(stats, LPROCFS_GET_NUM_CPU, &flags);
	for (i = 0; i < num_cpu; ++i) {
<<<<<<< HEAD
		if (stats->ls_percpu[i] == NULL)
=======
		if (!stats->ls_percpu[i])
>>>>>>> v4.9.227
			continue;
		percpu_cntr = lprocfs_stats_counter_get(stats, i, index);
		percpu_cntr->lc_count		= 0;
		percpu_cntr->lc_min		= LC_MIN_INIT;
		percpu_cntr->lc_max		= 0;
		percpu_cntr->lc_sumsquare	= 0;
		percpu_cntr->lc_sum		= 0;
		if ((stats->ls_flags & LPROCFS_STATS_FLAG_IRQ_SAFE) != 0)
			percpu_cntr->lc_sum_irq	= 0;
	}
	lprocfs_stats_unlock(stats, LPROCFS_GET_NUM_CPU, &flags);
}
EXPORT_SYMBOL(lprocfs_counter_init);

<<<<<<< HEAD
#define LPROCFS_OBD_OP_INIT(base, stats, op)			       \
do {								       \
	unsigned int coffset = base + OBD_COUNTER_OFFSET(op);	      \
	LASSERT(coffset < stats->ls_num);				  \
	lprocfs_counter_init(stats, coffset, 0, #op, "reqs");	      \
} while (0)

void lprocfs_init_ops_stats(int num_private_stats, struct lprocfs_stats *stats)
{
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, iocontrol);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, get_info);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, set_info_async);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, attach);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, detach);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, setup);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, precleanup);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, cleanup);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, process_config);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, postrecov);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, add_conn);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, del_conn);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, connect);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, reconnect);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, disconnect);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, fid_init);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, fid_fini);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, fid_alloc);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, statfs);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, statfs_async);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, packmd);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, unpackmd);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, preallocate);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, create);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, destroy);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, setattr);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, setattr_async);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, getattr);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, getattr_async);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, adjust_kms);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, preprw);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, commitrw);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, find_cbdata);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, init_export);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, destroy_export);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, import_event);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, notify);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, health_check);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, get_uuid);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, quotacheck);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, quotactl);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, pool_new);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, pool_rem);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, pool_add);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, pool_del);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, getref);
	LPROCFS_OBD_OP_INIT(num_private_stats, stats, putref);
}
EXPORT_SYMBOL(lprocfs_init_ops_stats);

int lprocfs_alloc_obd_stats(struct obd_device *obd, unsigned num_private_stats)
{
	struct lprocfs_stats *stats;
	unsigned int num_stats;
	int rc, i;

	LASSERT(obd->obd_stats == NULL);
	LASSERT(obd->obd_proc_entry != NULL);
	LASSERT(obd->obd_cntr_base == 0);

	num_stats = ((int)sizeof(*obd->obd_type->typ_dt_ops) / sizeof(void *)) +
		num_private_stats - 1 /* o_owner */;
	stats = lprocfs_alloc_stats(num_stats, 0);
	if (stats == NULL)
		return -ENOMEM;

	lprocfs_init_ops_stats(num_private_stats, stats);

	for (i = num_private_stats; i < num_stats; i++) {
		/* If this LBUGs, it is likely that an obd
		 * operation was added to struct obd_ops in
		 * <obd.h>, and that the corresponding line item
		 * LPROCFS_OBD_OP_INIT(.., .., opname)
		 * is missing from the list above. */
		LASSERTF(stats->ls_cnt_header[i].lc_name != NULL,
			 "Missing obd_stat initializer obd_op "
			 "operation at offset %d.\n", i - num_private_stats);
	}
	rc = lprocfs_register_stats(obd->obd_proc_entry, "stats", stats);
	if (rc < 0) {
		lprocfs_free_stats(&stats);
	} else {
		obd->obd_stats  = stats;
		obd->obd_cntr_base = num_private_stats;
	}
	return rc;
}
EXPORT_SYMBOL(lprocfs_alloc_obd_stats);

void lprocfs_free_obd_stats(struct obd_device *obd)
{
	if (obd->obd_stats)
		lprocfs_free_stats(&obd->obd_stats);
}
EXPORT_SYMBOL(lprocfs_free_obd_stats);

#define LPROCFS_MD_OP_INIT(base, stats, op)			     \
do {								    \
	unsigned int coffset = base + MD_COUNTER_OFFSET(op);	    \
	LASSERT(coffset < stats->ls_num);			       \
	lprocfs_counter_init(stats, coffset, 0, #op, "reqs");	   \
} while (0)

void lprocfs_init_mps_stats(int num_private_stats, struct lprocfs_stats *stats)
{
	LPROCFS_MD_OP_INIT(num_private_stats, stats, getstatus);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, null_inode);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, find_cbdata);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, close);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, create);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, done_writing);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, enqueue);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, getattr);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, getattr_name);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, intent_lock);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, link);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, rename);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, is_subdir);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, setattr);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, sync);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, readpage);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, unlink);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, setxattr);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, getxattr);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, init_ea_size);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, get_lustre_md);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, free_lustre_md);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, set_open_replay_data);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, clear_open_replay_data);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, set_lock_data);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, lock_match);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, cancel_unused);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, renew_capa);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, unpack_capa);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, get_remote_perm);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, intent_getattr_async);
	LPROCFS_MD_OP_INIT(num_private_stats, stats, revalidate_lock);
}
EXPORT_SYMBOL(lprocfs_init_mps_stats);

int lprocfs_alloc_md_stats(struct obd_device *obd,
			   unsigned num_private_stats)
{
	struct lprocfs_stats *stats;
	unsigned int num_stats;
	int rc, i;

	LASSERT(obd->md_stats == NULL);
	LASSERT(obd->obd_proc_entry != NULL);
	LASSERT(obd->md_cntr_base == 0);

	num_stats = 1 + MD_COUNTER_OFFSET(revalidate_lock) +
		    num_private_stats;
	stats = lprocfs_alloc_stats(num_stats, 0);
	if (stats == NULL)
		return -ENOMEM;

	lprocfs_init_mps_stats(num_private_stats, stats);

	for (i = num_private_stats; i < num_stats; i++) {
		if (stats->ls_cnt_header[i].lc_name == NULL) {
			CERROR("Missing md_stat initializer md_op "
			       "operation at offset %d. Aborting.\n",
			       i - num_private_stats);
			LBUG();
		}
	}
	rc = lprocfs_register_stats(obd->obd_proc_entry, "md_stats", stats);
	if (rc < 0) {
		lprocfs_free_stats(&stats);
	} else {
		obd->md_stats  = stats;
		obd->md_cntr_base = num_private_stats;
	}
	return rc;
}
EXPORT_SYMBOL(lprocfs_alloc_md_stats);

void lprocfs_free_md_stats(struct obd_device *obd)
{
	struct lprocfs_stats *stats = obd->md_stats;

	if (stats != NULL) {
		obd->md_stats = NULL;
		obd->md_cntr_base = 0;
		lprocfs_free_stats(&stats);
	}
}
EXPORT_SYMBOL(lprocfs_free_md_stats);

void lprocfs_init_ldlm_stats(struct lprocfs_stats *ldlm_stats)
{
	lprocfs_counter_init(ldlm_stats,
			     LDLM_ENQUEUE - LDLM_FIRST_OPC,
			     0, "ldlm_enqueue", "reqs");
	lprocfs_counter_init(ldlm_stats,
			     LDLM_CONVERT - LDLM_FIRST_OPC,
			     0, "ldlm_convert", "reqs");
	lprocfs_counter_init(ldlm_stats,
			     LDLM_CANCEL - LDLM_FIRST_OPC,
			     0, "ldlm_cancel", "reqs");
	lprocfs_counter_init(ldlm_stats,
			     LDLM_BL_CALLBACK - LDLM_FIRST_OPC,
			     0, "ldlm_bl_callback", "reqs");
	lprocfs_counter_init(ldlm_stats,
			     LDLM_CP_CALLBACK - LDLM_FIRST_OPC,
			     0, "ldlm_cp_callback", "reqs");
	lprocfs_counter_init(ldlm_stats,
			     LDLM_GL_CALLBACK - LDLM_FIRST_OPC,
			     0, "ldlm_gl_callback", "reqs");
}
EXPORT_SYMBOL(lprocfs_init_ldlm_stats);

int lprocfs_exp_print_uuid(struct cfs_hash *hs, struct cfs_hash_bd *bd,
			   struct hlist_node *hnode, void *data)

{
	struct obd_export *exp = cfs_hash_object(hs, hnode);
	struct seq_file *m = (struct seq_file *)data;

	if (exp->exp_nid_stats)
		seq_printf(m, "%s\n", obd_uuid2str(&exp->exp_client_uuid));

	return 0;
}

static int
lproc_exp_uuid_seq_show(struct seq_file *m, void *unused)
{
	struct nid_stat *stats = (struct nid_stat *)m->private;
	struct obd_device *obd = stats->nid_obd;

	cfs_hash_for_each_key(obd->obd_nid_hash, &stats->nid,
			      lprocfs_exp_print_uuid, m);
	return 0;
}

LPROC_SEQ_FOPS_RO(lproc_exp_uuid);

struct exp_hash_cb_data {
	struct seq_file *m;
	bool		first;
};

int lprocfs_exp_print_hash(struct cfs_hash *hs, struct cfs_hash_bd *bd,
			   struct hlist_node *hnode, void *cb_data)

{
	struct exp_hash_cb_data *data = (struct exp_hash_cb_data *)cb_data;
	struct obd_export       *exp = cfs_hash_object(hs, hnode);

	if (exp->exp_lock_hash != NULL) {
		if (data->first) {
			cfs_hash_debug_header(data->m);
			data->first = false;
		}
		cfs_hash_debug_str(hs, data->m);
	}

	return 0;
}

static int
lproc_exp_hash_seq_show(struct seq_file *m, void *unused)
{
	struct nid_stat *stats = (struct nid_stat *)m->private;
	struct obd_device *obd = stats->nid_obd;
	struct exp_hash_cb_data cb_data = {
		.m = m,
		.first = true
	};

	cfs_hash_for_each_key(obd->obd_nid_hash, &stats->nid,
			      lprocfs_exp_print_hash, &cb_data);
	return 0;
}

LPROC_SEQ_FOPS_RO(lproc_exp_hash);

int lprocfs_nid_stats_clear_read(struct seq_file *m, void *data)
{
	return seq_printf(m, "%s\n",
			"Write into this file to clear all nid stats and "
			"stale nid entries");
}
EXPORT_SYMBOL(lprocfs_nid_stats_clear_read);

static int lprocfs_nid_stats_clear_write_cb(void *obj, void *data)
{
	struct nid_stat *stat = obj;

	CDEBUG(D_INFO, "refcnt %d\n", atomic_read(&stat->nid_exp_ref_count));
	if (atomic_read(&stat->nid_exp_ref_count) == 1) {
		/* object has only hash references. */
		spin_lock(&stat->nid_obd->obd_nid_lock);
		list_move(&stat->nid_list, data);
		spin_unlock(&stat->nid_obd->obd_nid_lock);
		return 1;
	}
	/* we has reference to object - only clear data*/
	if (stat->nid_stats)
		lprocfs_clear_stats(stat->nid_stats);

	return 0;
}

int lprocfs_nid_stats_clear_write(struct file *file, const char *buffer,
				  unsigned long count, void *data)
{
	struct obd_device *obd = (struct obd_device *)data;
	struct nid_stat *client_stat;
	LIST_HEAD(free_list);

	cfs_hash_cond_del(obd->obd_nid_stats_hash,
			  lprocfs_nid_stats_clear_write_cb, &free_list);

	while (!list_empty(&free_list)) {
		client_stat = list_entry(free_list.next, struct nid_stat,
					     nid_list);
		list_del_init(&client_stat->nid_list);
		lprocfs_free_client_stats(client_stat);
	}

	return count;
}
EXPORT_SYMBOL(lprocfs_nid_stats_clear_write);

int lprocfs_exp_setup(struct obd_export *exp, lnet_nid_t *nid, int *newnid)
{
	struct nid_stat *new_stat, *old_stat;
	struct obd_device *obd = NULL;
	struct proc_dir_entry *entry;
	char *buffer = NULL;
	int rc = 0;

	*newnid = 0;

	if (!exp || !exp->exp_obd || !exp->exp_obd->obd_proc_exports_entry ||
	    !exp->exp_obd->obd_nid_stats_hash)
		return -EINVAL;

	/* not test against zero because eric say:
	 * You may only test nid against another nid, or LNET_NID_ANY.
	 * Anything else is nonsense.*/
	if (!nid || *nid == LNET_NID_ANY)
		return 0;

	obd = exp->exp_obd;

	CDEBUG(D_CONFIG, "using hash %p\n", obd->obd_nid_stats_hash);

	OBD_ALLOC_PTR(new_stat);
	if (new_stat == NULL)
		return -ENOMEM;

	new_stat->nid	       = *nid;
	new_stat->nid_obd	   = exp->exp_obd;
	/* we need set default refcount to 1 to balance obd_disconnect */
	atomic_set(&new_stat->nid_exp_ref_count, 1);

	old_stat = cfs_hash_findadd_unique(obd->obd_nid_stats_hash,
					   nid, &new_stat->nid_hash);
	CDEBUG(D_INFO, "Found stats %p for nid %s - ref %d\n",
	       old_stat, libcfs_nid2str(*nid),
	       atomic_read(&new_stat->nid_exp_ref_count));

	/* We need to release old stats because lprocfs_exp_cleanup() hasn't
	 * been and will never be called. */
	if (exp->exp_nid_stats) {
		nidstat_putref(exp->exp_nid_stats);
		exp->exp_nid_stats = NULL;
	}

	/* Return -EALREADY here so that we know that the /proc
	 * entry already has been created */
	if (old_stat != new_stat) {
		exp->exp_nid_stats = old_stat;
		rc = -EALREADY;
		goto destroy_new;
	}
	/* not found - create */
	OBD_ALLOC(buffer, LNET_NIDSTR_SIZE);
	if (buffer == NULL) {
		rc = -ENOMEM;
		goto destroy_new;
	}

	memcpy(buffer, libcfs_nid2str(*nid), LNET_NIDSTR_SIZE);
	new_stat->nid_proc = lprocfs_register(buffer,
					      obd->obd_proc_exports_entry,
					      NULL, NULL);
	OBD_FREE(buffer, LNET_NIDSTR_SIZE);

	if (IS_ERR(new_stat->nid_proc)) {
		CERROR("Error making export directory for nid %s\n",
		       libcfs_nid2str(*nid));
		rc = PTR_ERR(new_stat->nid_proc);
		new_stat->nid_proc = NULL;
		goto destroy_new_ns;
	}

	entry = lprocfs_add_simple(new_stat->nid_proc, "uuid",
				   new_stat, &lproc_exp_uuid_fops);
	if (IS_ERR(entry)) {
		CWARN("Error adding the NID stats file\n");
		rc = PTR_ERR(entry);
		goto destroy_new_ns;
	}

	entry = lprocfs_add_simple(new_stat->nid_proc, "hash",
				   new_stat, &lproc_exp_hash_fops);
	if (IS_ERR(entry)) {
		CWARN("Error adding the hash file\n");
		rc = PTR_ERR(entry);
		goto destroy_new_ns;
	}

	exp->exp_nid_stats = new_stat;
	*newnid = 1;
	/* protect competitive add to list, not need locking on destroy */
	spin_lock(&obd->obd_nid_lock);
	list_add(&new_stat->nid_list, &obd->obd_nid_stats);
	spin_unlock(&obd->obd_nid_lock);

	return rc;

destroy_new_ns:
	if (new_stat->nid_proc != NULL)
		lprocfs_remove(&new_stat->nid_proc);
	cfs_hash_del(obd->obd_nid_stats_hash, nid, &new_stat->nid_hash);

destroy_new:
	nidstat_putref(new_stat);
	OBD_FREE_PTR(new_stat);
	return rc;
}
EXPORT_SYMBOL(lprocfs_exp_setup);

int lprocfs_exp_cleanup(struct obd_export *exp)
{
	struct nid_stat *stat = exp->exp_nid_stats;

	if (!stat || !exp->exp_obd)
		return 0;

	nidstat_putref(exp->exp_nid_stats);
	exp->exp_nid_stats = NULL;

=======
int lprocfs_exp_cleanup(struct obd_export *exp)
{
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL(lprocfs_exp_cleanup);

__s64 lprocfs_read_helper(struct lprocfs_counter *lc,
			  struct lprocfs_counter_header *header,
			  enum lprocfs_stats_flags flags,
			  enum lprocfs_fields_flags field)
{
	__s64 ret = 0;

<<<<<<< HEAD
	if (lc == NULL || header == NULL)
=======
	if (!lc || !header)
>>>>>>> v4.9.227
		return 0;

	switch (field) {
	case LPROCFS_FIELDS_FLAGS_CONFIG:
		ret = header->lc_config;
		break;
	case LPROCFS_FIELDS_FLAGS_SUM:
		ret = lc->lc_sum;
		if ((flags & LPROCFS_STATS_FLAG_IRQ_SAFE) != 0)
			ret += lc->lc_sum_irq;
		break;
	case LPROCFS_FIELDS_FLAGS_MIN:
		ret = lc->lc_min;
		break;
	case LPROCFS_FIELDS_FLAGS_MAX:
		ret = lc->lc_max;
		break;
	case LPROCFS_FIELDS_FLAGS_AVG:
		ret = (lc->lc_max - lc->lc_min) / 2;
		break;
	case LPROCFS_FIELDS_FLAGS_SUMSQUARE:
		ret = lc->lc_sumsquare;
		break;
	case LPROCFS_FIELDS_FLAGS_COUNT:
		ret = lc->lc_count;
		break;
	default:
		break;
	}

	return 0;
}
EXPORT_SYMBOL(lprocfs_read_helper);

<<<<<<< HEAD
int lprocfs_write_helper(const char *buffer, unsigned long count,
=======
int lprocfs_write_helper(const char __user *buffer, unsigned long count,
>>>>>>> v4.9.227
			 int *val)
{
	return lprocfs_write_frac_helper(buffer, count, val, 1);
}
EXPORT_SYMBOL(lprocfs_write_helper);

<<<<<<< HEAD
int lprocfs_seq_read_frac_helper(struct seq_file *m, long val, int mult)
{
	long decimal_val, frac_val;

	decimal_val = val / mult;
	seq_printf(m, "%ld", decimal_val);
	frac_val = val % mult;

	if (frac_val > 0) {
		frac_val *= 100;
		frac_val /= mult;
	}
	if (frac_val > 0) {
		/* Three cases: x0, xx, 0x */
		if ((frac_val % 10) != 0)
			seq_printf(m, ".%ld", frac_val);
		else
			seq_printf(m, ".%ld", frac_val / 10);
	}

	seq_printf(m, "\n");
	return 0;
}
EXPORT_SYMBOL(lprocfs_seq_read_frac_helper);

int lprocfs_write_u64_helper(const char *buffer, unsigned long count,
=======
int lprocfs_write_u64_helper(const char __user *buffer, unsigned long count,
>>>>>>> v4.9.227
			     __u64 *val)
{
	return lprocfs_write_frac_u64_helper(buffer, count, val, 1);
}
EXPORT_SYMBOL(lprocfs_write_u64_helper);

<<<<<<< HEAD
int lprocfs_write_frac_u64_helper(const char *buffer, unsigned long count,
			      __u64 *val, int mult)
=======
int lprocfs_write_frac_u64_helper(const char __user *buffer,
				  unsigned long count, __u64 *val, int mult)
>>>>>>> v4.9.227
{
	char kernbuf[22], *end, *pbuf;
	__u64 whole, frac = 0, units;
	unsigned frac_d = 1;
<<<<<<< HEAD
=======
	int sign = 1;
>>>>>>> v4.9.227

	if (count > (sizeof(kernbuf) - 1))
		return -EINVAL;

	if (copy_from_user(kernbuf, buffer, count))
		return -EFAULT;

	kernbuf[count] = '\0';
	pbuf = kernbuf;
	if (*pbuf == '-') {
<<<<<<< HEAD
		mult = -mult;
=======
		sign = -1;
>>>>>>> v4.9.227
		pbuf++;
	}

	whole = simple_strtoull(pbuf, &end, 10);
	if (pbuf == end)
		return -EINVAL;

<<<<<<< HEAD
	if (end != NULL && *end == '.') {
		int i;
=======
	if (*end == '.') {
		int i;

>>>>>>> v4.9.227
		pbuf = end + 1;

		/* need to limit frac_d to a __u32 */
		if (strlen(pbuf) > 10)
			pbuf[10] = '\0';

		frac = simple_strtoull(pbuf, &end, 10);
		/* count decimal places */
		for (i = 0; i < (end - pbuf); i++)
			frac_d *= 10;
	}

	units = 1;
<<<<<<< HEAD
	switch (*end) {
	case 'p': case 'P':
		units <<= 10;
	case 't': case 'T':
		units <<= 10;
	case 'g': case 'G':
		units <<= 10;
	case 'm': case 'M':
		units <<= 10;
	case 'k': case 'K':
		units <<= 10;
	}
	/* Specified units override the multiplier */
	if (units)
		mult = mult < 0 ? -units : units;

	frac *= mult;
	do_div(frac, frac_d);
	*val = whole * mult + frac;
=======
	if (end) {
		switch (tolower(*end)) {
		case 'p':
			units <<= 10;
		case 't':
			units <<= 10;
		case 'g':
			units <<= 10;
		case 'm':
			units <<= 10;
		case 'k':
			units <<= 10;
		}
	}
	/* Specified units override the multiplier */
	if (units > 1)
		mult = units;

	frac *= mult;
	do_div(frac, frac_d);
	*val = sign * (whole * mult + frac);
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL(lprocfs_write_frac_u64_helper);

static char *lprocfs_strnstr(const char *s1, const char *s2, size_t len)
{
	size_t l2;

	l2 = strlen(s2);
	if (!l2)
		return (char *)s1;
	while (len >= l2) {
		len--;
		if (!memcmp(s1, s2, l2))
			return (char *)s1;
		s1++;
	}
	return NULL;
}

/**
 * Find the string \a name in the input \a buffer, and return a pointer to the
 * value immediately following \a name, reducing \a count appropriately.
 * If \a name is not found the original \a buffer is returned.
 */
char *lprocfs_find_named_value(const char *buffer, const char *name,
			       size_t *count)
{
	char *val;
	size_t buflen = *count;

	/* there is no strnstr() in rhel5 and ubuntu kernels */
	val = lprocfs_strnstr(buffer, name, buflen);
<<<<<<< HEAD
	if (val == NULL)
=======
	if (!val)
>>>>>>> v4.9.227
		return (char *)buffer;

	val += strlen(name);			     /* skip prefix */
	while (val < buffer + buflen && isspace(*val)) /* skip separator */
		val++;

	*count = 0;
	while (val < buffer + buflen && isalnum(*val)) {
		++*count;
		++val;
	}

	return val - *count;
}
EXPORT_SYMBOL(lprocfs_find_named_value);

<<<<<<< HEAD
int lprocfs_seq_create(struct proc_dir_entry *parent,
		       const char *name,
		       umode_t mode,
		       const struct file_operations *seq_fops,
		       void *data)
{
	struct proc_dir_entry *entry;

	/* Disallow secretly (un)writable entries. */
	LASSERT((seq_fops->write == NULL) == ((mode & 0222) == 0));
	entry = proc_create_data(name, mode, parent, seq_fops, data);

	if (entry == NULL)
		return -ENOMEM;

	return 0;
}
EXPORT_SYMBOL(lprocfs_seq_create);

int lprocfs_obd_seq_create(struct obd_device *dev,
			   const char *name,
			   umode_t mode,
			   const struct file_operations *seq_fops,
			   void *data)
{
	return lprocfs_seq_create(dev->obd_proc_entry, name,
				  mode, seq_fops, data);
}
EXPORT_SYMBOL(lprocfs_obd_seq_create);
=======
int ldebugfs_seq_create(struct dentry *parent, const char *name,
			umode_t mode, const struct file_operations *seq_fops,
			void *data)
{
	struct dentry *entry;

	/* Disallow secretly (un)writable entries. */
	LASSERT((seq_fops->write == NULL) == ((mode & 0222) == 0));

	entry = debugfs_create_file(name, mode, parent, data, seq_fops);
	if (IS_ERR_OR_NULL(entry))
		return entry ? PTR_ERR(entry) : -ENOMEM;

	return 0;
}
EXPORT_SYMBOL_GPL(ldebugfs_seq_create);

int ldebugfs_obd_seq_create(struct obd_device *dev,
			    const char *name,
			    umode_t mode,
			    const struct file_operations *seq_fops,
			    void *data)
{
	return ldebugfs_seq_create(dev->obd_debugfs_entry, name,
				   mode, seq_fops, data);
}
EXPORT_SYMBOL_GPL(ldebugfs_obd_seq_create);
>>>>>>> v4.9.227

void lprocfs_oh_tally(struct obd_histogram *oh, unsigned int value)
{
	if (value >= OBD_HIST_MAX)
		value = OBD_HIST_MAX - 1;

	spin_lock(&oh->oh_lock);
	oh->oh_buckets[value]++;
	spin_unlock(&oh->oh_lock);
}
EXPORT_SYMBOL(lprocfs_oh_tally);

void lprocfs_oh_tally_log2(struct obd_histogram *oh, unsigned int value)
{
<<<<<<< HEAD
	unsigned int val;

	for (val = 0; ((1 << val) < value) && (val <= OBD_HIST_MAX); val++)
		;
=======
	unsigned int val = 0;

	if (likely(value != 0))
		val = min(fls(value - 1), OBD_HIST_MAX);
>>>>>>> v4.9.227

	lprocfs_oh_tally(oh, val);
}
EXPORT_SYMBOL(lprocfs_oh_tally_log2);

unsigned long lprocfs_oh_sum(struct obd_histogram *oh)
{
	unsigned long ret = 0;
	int i;

	for (i = 0; i < OBD_HIST_MAX; i++)
		ret +=  oh->oh_buckets[i];
	return ret;
}
EXPORT_SYMBOL(lprocfs_oh_sum);

void lprocfs_oh_clear(struct obd_histogram *oh)
{
	spin_lock(&oh->oh_lock);
	memset(oh->oh_buckets, 0, sizeof(oh->oh_buckets));
	spin_unlock(&oh->oh_lock);
}
EXPORT_SYMBOL(lprocfs_oh_clear);

<<<<<<< HEAD
int lprocfs_obd_rd_max_pages_per_rpc(struct seq_file *m, void *data)
{
	struct obd_device *dev = data;
	struct client_obd *cli = &dev->u.cli;
	int rc;

	client_obd_list_lock(&cli->cl_loi_list_lock);
	rc = seq_printf(m, "%d\n", cli->cl_max_pages_per_rpc);
	client_obd_list_unlock(&cli->cl_loi_list_lock);
	return rc;
}
EXPORT_SYMBOL(lprocfs_obd_rd_max_pages_per_rpc);

#endif
=======
int lprocfs_wr_root_squash(const char __user *buffer, unsigned long count,
			   struct root_squash_info *squash, char *name)
{
	char kernbuf[64], *tmp, *errmsg;
	unsigned long uid, gid;
	int rc;

	if (count >= sizeof(kernbuf)) {
		errmsg = "string too long";
		rc = -EINVAL;
		goto failed_noprint;
	}
	if (copy_from_user(kernbuf, buffer, count)) {
		errmsg = "bad address";
		rc = -EFAULT;
		goto failed_noprint;
	}
	kernbuf[count] = '\0';

	/* look for uid gid separator */
	tmp = strchr(kernbuf, ':');
	if (!tmp) {
		errmsg = "needs uid:gid format";
		rc = -EINVAL;
		goto failed;
	}
	*tmp = '\0';
	tmp++;

	/* parse uid */
	if (kstrtoul(kernbuf, 0, &uid) != 0) {
		errmsg = "bad uid";
		rc = -EINVAL;
		goto failed;
	}
	/* parse gid */
	if (kstrtoul(tmp, 0, &gid) != 0) {
		errmsg = "bad gid";
		rc = -EINVAL;
		goto failed;
	}

	squash->rsi_uid = uid;
	squash->rsi_gid = gid;

	LCONSOLE_INFO("%s: root_squash is set to %u:%u\n",
		      name, squash->rsi_uid, squash->rsi_gid);
	return count;

failed:
	if (tmp) {
		tmp--;
		*tmp = ':';
	}
	CWARN("%s: failed to set root_squash to \"%s\", %s, rc = %d\n",
	      name, kernbuf, errmsg, rc);
	return rc;
failed_noprint:
	CWARN("%s: failed to set root_squash due to %s, rc = %d\n",
	      name, errmsg, rc);
	return rc;
}
EXPORT_SYMBOL(lprocfs_wr_root_squash);

int lprocfs_wr_nosquash_nids(const char __user *buffer, unsigned long count,
			     struct root_squash_info *squash, char *name)
{
	char *kernbuf = NULL, *errmsg;
	struct list_head tmp;
	int len = count;
	int rc;

	if (count > 4096) {
		errmsg = "string too long";
		rc = -EINVAL;
		goto failed;
	}

	kernbuf = kzalloc(count + 1, GFP_NOFS);
	if (!kernbuf) {
		errmsg = "no memory";
		rc = -ENOMEM;
		goto failed;
	}

	if (copy_from_user(kernbuf, buffer, count)) {
		errmsg = "bad address";
		rc = -EFAULT;
		goto failed;
	}
	kernbuf[count] = '\0';

	if (count > 0 && kernbuf[count - 1] == '\n')
		len = count - 1;

	if ((len == 4 && !strncmp(kernbuf, "NONE", len)) ||
	    (len == 5 && !strncmp(kernbuf, "clear", len))) {
		/* empty string is special case */
		down_write(&squash->rsi_sem);
		if (!list_empty(&squash->rsi_nosquash_nids))
			cfs_free_nidlist(&squash->rsi_nosquash_nids);
		up_write(&squash->rsi_sem);
		LCONSOLE_INFO("%s: nosquash_nids is cleared\n", name);
		kfree(kernbuf);
		return count;
	}

	INIT_LIST_HEAD(&tmp);
	if (cfs_parse_nidlist(kernbuf, count, &tmp) <= 0) {
		errmsg = "can't parse";
		rc = -EINVAL;
		goto failed;
	}
	LCONSOLE_INFO("%s: nosquash_nids set to %s\n",
		      name, kernbuf);
	kfree(kernbuf);
	kernbuf = NULL;

	down_write(&squash->rsi_sem);
	if (!list_empty(&squash->rsi_nosquash_nids))
		cfs_free_nidlist(&squash->rsi_nosquash_nids);
	list_splice(&tmp, &squash->rsi_nosquash_nids);
	up_write(&squash->rsi_sem);

	return count;

failed:
	if (kernbuf) {
		CWARN("%s: failed to set nosquash_nids to \"%s\", %s rc = %d\n",
		      name, kernbuf, errmsg, rc);
		kfree(kernbuf);
		kernbuf = NULL;
	} else {
		CWARN("%s: failed to set nosquash_nids due to %s rc = %d\n",
		      name, errmsg, rc);
	}
	return rc;
}
EXPORT_SYMBOL(lprocfs_wr_nosquash_nids);

static ssize_t lustre_attr_show(struct kobject *kobj,
				struct attribute *attr, char *buf)
{
	struct lustre_attr *a = container_of(attr, struct lustre_attr, attr);

	return a->show ? a->show(kobj, attr, buf) : 0;
}

static ssize_t lustre_attr_store(struct kobject *kobj, struct attribute *attr,
				 const char *buf, size_t len)
{
	struct lustre_attr *a = container_of(attr, struct lustre_attr, attr);

	return a->store ? a->store(kobj, attr, buf, len) : len;
}

const struct sysfs_ops lustre_sysfs_ops = {
	.show  = lustre_attr_show,
	.store = lustre_attr_store,
};
EXPORT_SYMBOL_GPL(lustre_sysfs_ops);
>>>>>>> v4.9.227
