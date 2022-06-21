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
 */
#define DEBUG_SUBSYSTEM S_CLASS

<<<<<<< HEAD
#include <asm/statfs.h>
=======
#include <linux/statfs.h>
>>>>>>> v4.9.227
#include "../include/obd_cksum.h"
#include "../include/obd_class.h"
#include "../include/lprocfs_status.h"
#include <linux/seq_file.h>
#include "osc_internal.h"

<<<<<<< HEAD
static int osc_active_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *dev = m->private;
	int rc;

	LPROCFS_CLIMP_CHECK(dev);
	rc = seq_printf(m, "%d\n", !dev->u.cli.cl_import->imp_deactive);
	LPROCFS_CLIMP_EXIT(dev);
	return rc;
}

static ssize_t osc_active_seq_write(struct file *file, const char *buffer,
				    size_t count, loff_t *off)
{
	struct obd_device *dev = ((struct seq_file *)file->private_data)->private;
	int val, rc;

	rc = lprocfs_write_helper(buffer, count, &val);
	if (rc)
		return rc;
	if (val < 0 || val > 1)
=======
static ssize_t active_show(struct kobject *kobj, struct attribute *attr,
			   char *buf)
{
	struct obd_device *dev = container_of(kobj, struct obd_device,
					      obd_kobj);

	return sprintf(buf, "%d\n", !dev->u.cli.cl_import->imp_deactive);
}

static ssize_t active_store(struct kobject *kobj, struct attribute *attr,
			    const char *buffer,
			    size_t count)
{
	struct obd_device *dev = container_of(kobj, struct obd_device,
					      obd_kobj);
	int rc;
	unsigned long val;

	rc = kstrtoul(buffer, 10, &val);
	if (rc)
		return rc;
	if (val > 1)
>>>>>>> v4.9.227
		return -ERANGE;

	/* opposite senses */
	if (dev->u.cli.cl_import->imp_deactive == val)
		rc = ptlrpc_set_import_active(dev->u.cli.cl_import, val);
	else
<<<<<<< HEAD
		CDEBUG(D_CONFIG, "activate %d: ignoring repeat request\n", val);

	return count;
}
LPROC_SEQ_FOPS(osc_active);

static int osc_max_rpcs_in_flight_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *dev = m->private;
	struct client_obd *cli = &dev->u.cli;
	int rc;

	client_obd_list_lock(&cli->cl_loi_list_lock);
	rc = seq_printf(m, "%u\n", cli->cl_max_rpcs_in_flight);
	client_obd_list_unlock(&cli->cl_loi_list_lock);
	return rc;
}

static ssize_t osc_max_rpcs_in_flight_seq_write(struct file *file,
			const char *buffer, size_t count, loff_t *off)
{
	struct obd_device *dev = ((struct seq_file *)file->private_data)->private;
	struct client_obd *cli = &dev->u.cli;
	struct ptlrpc_request_pool *pool = cli->cl_import->imp_rq_pool;
	int val, rc;

	rc = lprocfs_write_helper(buffer, count, &val);
=======
		CDEBUG(D_CONFIG, "activate %ld: ignoring repeat request\n",
		       val);

	return count;
}
LUSTRE_RW_ATTR(active);

static ssize_t max_rpcs_in_flight_show(struct kobject *kobj,
				       struct attribute *attr,
				       char *buf)
{
	struct obd_device *dev = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct client_obd *cli = &dev->u.cli;

	return sprintf(buf, "%u\n", cli->cl_max_rpcs_in_flight);
}

static ssize_t max_rpcs_in_flight_store(struct kobject *kobj,
					struct attribute *attr,
					const char *buffer,
					size_t count)
{
	struct obd_device *dev = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct client_obd *cli = &dev->u.cli;
	int rc;
	unsigned long val;
	int adding, added, req_count;

	rc = kstrtoul(buffer, 10, &val);
>>>>>>> v4.9.227
	if (rc)
		return rc;

	if (val < 1 || val > OSC_MAX_RIF_MAX)
		return -ERANGE;

<<<<<<< HEAD
	LPROCFS_CLIMP_CHECK(dev);
	if (pool && val > cli->cl_max_rpcs_in_flight)
		pool->prp_populate(pool, val-cli->cl_max_rpcs_in_flight);

	client_obd_list_lock(&cli->cl_loi_list_lock);
	cli->cl_max_rpcs_in_flight = val;
	client_obd_list_unlock(&cli->cl_loi_list_lock);

	LPROCFS_CLIMP_EXIT(dev);
	return count;
}
LPROC_SEQ_FOPS(osc_max_rpcs_in_flight);

static int osc_max_dirty_mb_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *dev = m->private;
=======
	adding = val - cli->cl_max_rpcs_in_flight;
	req_count = atomic_read(&osc_pool_req_count);
	if (adding > 0 && req_count < osc_reqpool_maxreqcount) {
		/*
		 * There might be some race which will cause over-limit
		 * allocation, but it is fine.
		 */
		if (req_count + adding > osc_reqpool_maxreqcount)
			adding = osc_reqpool_maxreqcount - req_count;

		added = osc_rq_pool->prp_populate(osc_rq_pool, adding);
		atomic_add(added, &osc_pool_req_count);
	}

	spin_lock(&cli->cl_loi_list_lock);
	cli->cl_max_rpcs_in_flight = val;
	client_adjust_max_dirty(cli);
	spin_unlock(&cli->cl_loi_list_lock);

	return count;
}
LUSTRE_RW_ATTR(max_rpcs_in_flight);

static ssize_t max_dirty_mb_show(struct kobject *kobj,
				 struct attribute *attr,
				 char *buf)
{
	struct obd_device *dev = container_of(kobj, struct obd_device,
					      obd_kobj);
>>>>>>> v4.9.227
	struct client_obd *cli = &dev->u.cli;
	long val;
	int mult;

<<<<<<< HEAD
	client_obd_list_lock(&cli->cl_loi_list_lock);
	val = cli->cl_dirty_max;
	client_obd_list_unlock(&cli->cl_loi_list_lock);

	mult = 1 << 20;
	return lprocfs_seq_read_frac_helper(m, val, mult);
}

static ssize_t osc_max_dirty_mb_seq_write(struct file *file, const char *buffer,
				      size_t count, loff_t *off)
{
	struct obd_device *dev = ((struct seq_file *)file->private_data)->private;
	struct client_obd *cli = &dev->u.cli;
	int pages_number, mult, rc;

	mult = 1 << (20 - PAGE_CACHE_SHIFT);
	rc = lprocfs_write_frac_helper(buffer, count, &pages_number, mult);
	if (rc)
		return rc;

	if (pages_number <= 0 ||
	    pages_number > OSC_MAX_DIRTY_MB_MAX << (20 - PAGE_CACHE_SHIFT) ||
	    pages_number > totalram_pages / 4) /* 1/4 of RAM */
		return -ERANGE;

	client_obd_list_lock(&cli->cl_loi_list_lock);
	cli->cl_dirty_max = (u32)(pages_number << PAGE_CACHE_SHIFT);
	osc_wake_cache_waiters(cli);
	client_obd_list_unlock(&cli->cl_loi_list_lock);

	return count;
}
LPROC_SEQ_FOPS(osc_max_dirty_mb);
=======
	spin_lock(&cli->cl_loi_list_lock);
	val = cli->cl_dirty_max_pages;
	spin_unlock(&cli->cl_loi_list_lock);

	mult = 1 << (20 - PAGE_SHIFT);
	return lprocfs_read_frac_helper(buf, PAGE_SIZE, val, mult);
}

static ssize_t max_dirty_mb_store(struct kobject *kobj,
				  struct attribute *attr,
				  const char *buffer,
				  size_t count)
{
	struct obd_device *dev = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct client_obd *cli = &dev->u.cli;
	int rc;
	unsigned long pages_number;

	rc = kstrtoul(buffer, 10, &pages_number);
	if (rc)
		return rc;

	pages_number *= 1 << (20 - PAGE_SHIFT); /* MB -> pages */

	if (pages_number <= 0 ||
	    pages_number > OSC_MAX_DIRTY_MB_MAX << (20 - PAGE_SHIFT) ||
	    pages_number > totalram_pages / 4) /* 1/4 of RAM */
		return -ERANGE;

	spin_lock(&cli->cl_loi_list_lock);
	cli->cl_dirty_max_pages = pages_number;
	osc_wake_cache_waiters(cli);
	spin_unlock(&cli->cl_loi_list_lock);

	return count;
}
LUSTRE_RW_ATTR(max_dirty_mb);
>>>>>>> v4.9.227

static int osc_cached_mb_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *dev = m->private;
	struct client_obd *cli = &dev->u.cli;
<<<<<<< HEAD
	int shift = 20 - PAGE_CACHE_SHIFT;
	int rc;

	rc = seq_printf(m,
		      "used_mb: %d\n"
		      "busy_cnt: %d\n",
		      (atomic_read(&cli->cl_lru_in_list) +
			atomic_read(&cli->cl_lru_busy)) >> shift,
		      atomic_read(&cli->cl_lru_busy));

	return rc;
=======
	int shift = 20 - PAGE_SHIFT;

	seq_printf(m,
		   "used_mb: %ld\n"
		   "busy_cnt: %ld\n",
		   (atomic_long_read(&cli->cl_lru_in_list) +
		    atomic_long_read(&cli->cl_lru_busy)) >> shift,
		   atomic_long_read(&cli->cl_lru_busy));

	return 0;
>>>>>>> v4.9.227
}

/* shrink the number of caching pages to a specific number */
static ssize_t osc_cached_mb_seq_write(struct file *file,
				       const char __user *buffer,
				       size_t count, loff_t *off)
{
	struct obd_device *dev = ((struct seq_file *)file->private_data)->private;
	struct client_obd *cli = &dev->u.cli;
<<<<<<< HEAD
	int pages_number, mult, rc;
	char kernbuf[128];
=======
	long pages_number, rc;
	char kernbuf[128];
	int mult;
	u64 val;
>>>>>>> v4.9.227

	if (count >= sizeof(kernbuf))
		return -EINVAL;

	if (copy_from_user(kernbuf, buffer, count))
		return -EFAULT;
	kernbuf[count] = 0;

<<<<<<< HEAD
	mult = 1 << (20 - PAGE_CACHE_SHIFT);
	buffer += lprocfs_find_named_value(kernbuf, "used_mb:", &count) -
		  kernbuf;
	rc = lprocfs_write_frac_helper(buffer, count, &pages_number, mult);
	if (rc)
		return rc;

	if (pages_number < 0)
		return -ERANGE;

	rc = atomic_read(&cli->cl_lru_in_list) - pages_number;
	if (rc > 0)
		(void)osc_lru_shrink(cli, rc);

	return count;
}
LPROC_SEQ_FOPS(osc_cached_mb);

static int osc_cur_dirty_bytes_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *dev = m->private;
	struct client_obd *cli = &dev->u.cli;
	int rc;

	client_obd_list_lock(&cli->cl_loi_list_lock);
	rc = seq_printf(m, "%lu\n", cli->cl_dirty);
	client_obd_list_unlock(&cli->cl_loi_list_lock);
	return rc;
}
LPROC_SEQ_FOPS_RO(osc_cur_dirty_bytes);

static int osc_cur_grant_bytes_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *dev = m->private;
	struct client_obd *cli = &dev->u.cli;
	int rc;

	client_obd_list_lock(&cli->cl_loi_list_lock);
	rc = seq_printf(m, "%lu\n", cli->cl_avail_grant);
	client_obd_list_unlock(&cli->cl_loi_list_lock);
	return rc;
}

static ssize_t osc_cur_grant_bytes_seq_write(struct file *file, const char *buffer,
				  size_t count, loff_t *off)
{
	struct obd_device *obd = ((struct seq_file *)file->private_data)->private;
	struct client_obd *cli = &obd->u.cli;
	int		rc;
	__u64	      val;

	if (obd == NULL)
		return 0;

	rc = lprocfs_write_u64_helper(buffer, count, &val);
=======
	mult = 1 << (20 - PAGE_SHIFT);
	buffer += lprocfs_find_named_value(kernbuf, "used_mb:", &count) -
		  kernbuf;
	rc = lprocfs_write_frac_u64_helper(buffer, count, &val, mult);
	if (rc)
		return rc;

	if (val > LONG_MAX)
		return -ERANGE;
	pages_number = (long)val;

	if (pages_number < 0)
		return -ERANGE;

	rc = atomic_long_read(&cli->cl_lru_in_list) - pages_number;
	if (rc > 0) {
		struct lu_env *env;
		int refcheck;

		env = cl_env_get(&refcheck);
		if (!IS_ERR(env)) {
			(void)osc_lru_shrink(env, cli, rc, true);
			cl_env_put(env, &refcheck);
		}
	}

	return count;
}

LPROC_SEQ_FOPS(osc_cached_mb);

static ssize_t cur_dirty_bytes_show(struct kobject *kobj,
				    struct attribute *attr,
				    char *buf)
{
	struct obd_device *dev = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct client_obd *cli = &dev->u.cli;
	int len;

	spin_lock(&cli->cl_loi_list_lock);
	len = sprintf(buf, "%lu\n", cli->cl_dirty_pages << PAGE_SHIFT);
	spin_unlock(&cli->cl_loi_list_lock);

	return len;
}
LUSTRE_RO_ATTR(cur_dirty_bytes);

static ssize_t cur_grant_bytes_show(struct kobject *kobj,
				    struct attribute *attr,
				    char *buf)
{
	struct obd_device *dev = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct client_obd *cli = &dev->u.cli;
	int len;

	spin_lock(&cli->cl_loi_list_lock);
	len = sprintf(buf, "%lu\n", cli->cl_avail_grant);
	spin_unlock(&cli->cl_loi_list_lock);

	return len;
}

static ssize_t cur_grant_bytes_store(struct kobject *kobj,
				     struct attribute *attr,
				     const char *buffer,
				     size_t count)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct client_obd *cli = &obd->u.cli;
	int rc;
	unsigned long long val;

	rc = kstrtoull(buffer, 10, &val);
>>>>>>> v4.9.227
	if (rc)
		return rc;

	/* this is only for shrinking grant */
<<<<<<< HEAD
	client_obd_list_lock(&cli->cl_loi_list_lock);
	if (val >= cli->cl_avail_grant) {
		client_obd_list_unlock(&cli->cl_loi_list_lock);
		return 0;
	}
	client_obd_list_unlock(&cli->cl_loi_list_lock);

	LPROCFS_CLIMP_CHECK(obd);
	if (cli->cl_import->imp_state == LUSTRE_IMP_FULL)
		rc = osc_shrink_grant_to_target(cli, val);
	LPROCFS_CLIMP_EXIT(obd);
=======
	spin_lock(&cli->cl_loi_list_lock);
	if (val >= cli->cl_avail_grant) {
		spin_unlock(&cli->cl_loi_list_lock);
		return -EINVAL;
	}
	spin_unlock(&cli->cl_loi_list_lock);

	if (cli->cl_import->imp_state == LUSTRE_IMP_FULL)
		rc = osc_shrink_grant_to_target(cli, val);
>>>>>>> v4.9.227
	if (rc)
		return rc;
	return count;
}
<<<<<<< HEAD
LPROC_SEQ_FOPS(osc_cur_grant_bytes);

static int osc_cur_lost_grant_bytes_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *dev = m->private;
	struct client_obd *cli = &dev->u.cli;
	int rc;

	client_obd_list_lock(&cli->cl_loi_list_lock);
	rc = seq_printf(m, "%lu\n", cli->cl_lost_grant);
	client_obd_list_unlock(&cli->cl_loi_list_lock);
	return rc;
}
LPROC_SEQ_FOPS_RO(osc_cur_lost_grant_bytes);

static int osc_grant_shrink_interval_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *obd = m->private;

	if (obd == NULL)
		return 0;
	return seq_printf(m, "%d\n",
			obd->u.cli.cl_grant_shrink_interval);
}

static ssize_t osc_grant_shrink_interval_seq_write(struct file *file,
				const char *buffer, size_t count, loff_t *off)
{
	struct obd_device *obd = ((struct seq_file *)file->private_data)->private;
	int val, rc;

	if (obd == NULL)
		return 0;

	rc = lprocfs_write_helper(buffer, count, &val);
=======
LUSTRE_RW_ATTR(cur_grant_bytes);

static ssize_t cur_lost_grant_bytes_show(struct kobject *kobj,
					 struct attribute *attr,
					 char *buf)
{
	struct obd_device *dev = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct client_obd *cli = &dev->u.cli;
	int len;

	spin_lock(&cli->cl_loi_list_lock);
	len = sprintf(buf, "%lu\n", cli->cl_lost_grant);
	spin_unlock(&cli->cl_loi_list_lock);

	return len;
}
LUSTRE_RO_ATTR(cur_lost_grant_bytes);

static ssize_t grant_shrink_interval_show(struct kobject *kobj,
					  struct attribute *attr,
					  char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);

	return sprintf(buf, "%d\n", obd->u.cli.cl_grant_shrink_interval);
}

static ssize_t grant_shrink_interval_store(struct kobject *kobj,
					   struct attribute *attr,
					   const char *buffer,
					   size_t count)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
	int rc;
	unsigned long val;

	rc = kstrtoul(buffer, 10, &val);
>>>>>>> v4.9.227
	if (rc)
		return rc;

	if (val <= 0)
		return -ERANGE;

	obd->u.cli.cl_grant_shrink_interval = val;

	return count;
}
<<<<<<< HEAD
LPROC_SEQ_FOPS(osc_grant_shrink_interval);

static int osc_checksum_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *obd = m->private;

	if (obd == NULL)
		return 0;

	return seq_printf(m, "%d\n",
			obd->u.cli.cl_checksum ? 1 : 0);
}

static ssize_t osc_checksum_seq_write(struct file *file, const char *buffer,
			   size_t count, loff_t *off)
{
	struct obd_device *obd = ((struct seq_file *)file->private_data)->private;
	int val, rc;

	if (obd == NULL)
		return 0;

	rc = lprocfs_write_helper(buffer, count, &val);
=======
LUSTRE_RW_ATTR(grant_shrink_interval);

static ssize_t checksums_show(struct kobject *kobj,
			      struct attribute *attr,
			      char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);

	return sprintf(buf, "%d\n", obd->u.cli.cl_checksum ? 1 : 0);
}

static ssize_t checksums_store(struct kobject *kobj,
			       struct attribute *attr,
			       const char *buffer,
			       size_t count)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
	int rc;
	unsigned long val;

	rc = kstrtoul(buffer, 10, &val);
>>>>>>> v4.9.227
	if (rc)
		return rc;

	obd->u.cli.cl_checksum = (val ? 1 : 0);

	return count;
}
<<<<<<< HEAD
LPROC_SEQ_FOPS(osc_checksum);
=======
LUSTRE_RW_ATTR(checksums);
>>>>>>> v4.9.227

static int osc_checksum_type_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *obd = m->private;
	int i;
<<<<<<< HEAD
	DECLARE_CKSUM_NAME;

	if (obd == NULL)
=======

	DECLARE_CKSUM_NAME;

	if (!obd)
>>>>>>> v4.9.227
		return 0;

	for (i = 0; i < ARRAY_SIZE(cksum_name); i++) {
		if (((1 << i) & obd->u.cli.cl_supp_cksum_types) == 0)
			continue;
		if (obd->u.cli.cl_cksum_type == (1 << i))
			seq_printf(m, "[%s] ", cksum_name[i]);
		else
			seq_printf(m, "%s ", cksum_name[i]);
	}
<<<<<<< HEAD
	seq_printf(m, "\n");
	return 0;
}

static ssize_t osc_checksum_type_seq_write(struct file *file, const char *buffer,
				size_t count, loff_t *off)
{
	struct obd_device *obd = ((struct seq_file *)file->private_data)->private;
	int i;
	DECLARE_CKSUM_NAME;
	char kernbuf[10];

	if (obd == NULL)
=======
	seq_putc(m, '\n');
	return 0;
}

static ssize_t osc_checksum_type_seq_write(struct file *file,
					   const char __user *buffer,
					   size_t count, loff_t *off)
{
	struct obd_device *obd = ((struct seq_file *)file->private_data)->private;
	int i;

	DECLARE_CKSUM_NAME;
	char kernbuf[10];

	if (!obd)
>>>>>>> v4.9.227
		return 0;

	if (count > sizeof(kernbuf) - 1)
		return -EINVAL;
	if (copy_from_user(kernbuf, buffer, count))
		return -EFAULT;
	if (count > 0 && kernbuf[count - 1] == '\n')
		kernbuf[count - 1] = '\0';
	else
		kernbuf[count] = '\0';

	for (i = 0; i < ARRAY_SIZE(cksum_name); i++) {
		if (((1 << i) & obd->u.cli.cl_supp_cksum_types) == 0)
			continue;
		if (!strcmp(kernbuf, cksum_name[i])) {
<<<<<<< HEAD
		       obd->u.cli.cl_cksum_type = 1 << i;
		       return count;
=======
			obd->u.cli.cl_cksum_type = 1 << i;
			return count;
>>>>>>> v4.9.227
		}
	}
	return -EINVAL;
}
<<<<<<< HEAD
LPROC_SEQ_FOPS(osc_checksum_type);

static int osc_resend_count_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *obd = m->private;

	return seq_printf(m, "%u\n", atomic_read(&obd->u.cli.cl_resends));
}

static ssize_t osc_resend_count_seq_write(struct file *file, const char *buffer,
			       size_t count, loff_t *off)
{
	struct obd_device *obd = ((struct seq_file *)file->private_data)->private;
	int val, rc;

	rc = lprocfs_write_helper(buffer, count, &val);
	if (rc)
		return rc;

	if (val < 0)
	       return -EINVAL;

=======

LPROC_SEQ_FOPS(osc_checksum_type);

static ssize_t resend_count_show(struct kobject *kobj,
				 struct attribute *attr,
				 char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);

	return sprintf(buf, "%u\n", atomic_read(&obd->u.cli.cl_resends));
}

static ssize_t resend_count_store(struct kobject *kobj,
				  struct attribute *attr,
				  const char *buffer,
				  size_t count)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
	int rc;
	unsigned long val;

	rc = kstrtoul(buffer, 10, &val);
	if (rc)
		return rc;

>>>>>>> v4.9.227
	atomic_set(&obd->u.cli.cl_resends, val);

	return count;
}
<<<<<<< HEAD
LPROC_SEQ_FOPS(osc_resend_count);

static int osc_contention_seconds_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *obd = m->private;
	struct osc_device *od  = obd2osc_dev(obd);

	return seq_printf(m, "%u\n", od->od_contention_time);
}

static ssize_t osc_contention_seconds_seq_write(struct file *file, const char *buffer,
				     size_t count, loff_t *off)
{
	struct obd_device *obd = ((struct seq_file *)file->private_data)->private;
	struct osc_device *od  = obd2osc_dev(obd);

	return lprocfs_write_helper(buffer, count, &od->od_contention_time) ?:
		count;
}
LPROC_SEQ_FOPS(osc_contention_seconds);

static int osc_lockless_truncate_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *obd = m->private;
	struct osc_device *od  = obd2osc_dev(obd);

	return seq_printf(m, "%u\n", od->od_lockless_truncate);
}

static ssize_t osc_lockless_truncate_seq_write(struct file *file, const char *buffer,
				    size_t count, loff_t *off)
{
	struct obd_device *obd = ((struct seq_file *)file->private_data)->private;
	struct osc_device *od  = obd2osc_dev(obd);

	return lprocfs_write_helper(buffer, count, &od->od_lockless_truncate) ?:
		count;
}
LPROC_SEQ_FOPS(osc_lockless_truncate);

static int osc_destroys_in_flight_seq_show(struct seq_file *m, void *v)
{
	struct obd_device *obd = m->private;
	return seq_printf(m, "%u\n",
			atomic_read(&obd->u.cli.cl_destroy_in_flight));
}
LPROC_SEQ_FOPS_RO(osc_destroys_in_flight);

static int osc_obd_max_pages_per_rpc_seq_show(struct seq_file *m, void *v)
{
	return lprocfs_obd_rd_max_pages_per_rpc(m, m->private);
}

static ssize_t osc_obd_max_pages_per_rpc_seq_write(struct file *file,
				const char *buffer, size_t count, loff_t *off)
{
	struct obd_device *dev = ((struct seq_file *)file->private_data)->private;
	struct client_obd *cli = &dev->u.cli;
	struct obd_connect_data *ocd = &cli->cl_import->imp_connect_data;
	int chunk_mask, rc;
	__u64 val;

	rc = lprocfs_write_u64_helper(buffer, count, &val);
=======
LUSTRE_RW_ATTR(resend_count);

static ssize_t contention_seconds_show(struct kobject *kobj,
				       struct attribute *attr,
				       char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct osc_device *od  = obd2osc_dev(obd);

	return sprintf(buf, "%u\n", od->od_contention_time);
}

static ssize_t contention_seconds_store(struct kobject *kobj,
					struct attribute *attr,
					const char *buffer,
					size_t count)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct osc_device *od  = obd2osc_dev(obd);
	int rc;
	int val;

	rc = kstrtoint(buffer, 10, &val);
	if (rc)
		return rc;

	if (val < 0)
		return -EINVAL;

	od->od_contention_time = val;

	return count;
}
LUSTRE_RW_ATTR(contention_seconds);

static ssize_t lockless_truncate_show(struct kobject *kobj,
				      struct attribute *attr,
				      char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct osc_device *od  = obd2osc_dev(obd);

	return sprintf(buf, "%u\n", od->od_lockless_truncate);
}

static ssize_t lockless_truncate_store(struct kobject *kobj,
				       struct attribute *attr,
				       const char *buffer,
				       size_t count)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct osc_device *od  = obd2osc_dev(obd);
	int rc;
	unsigned int val;

	rc = kstrtouint(buffer, 10, &val);
	if (rc)
		return rc;

	od->od_lockless_truncate = val;

	return count;
}
LUSTRE_RW_ATTR(lockless_truncate);

static ssize_t destroys_in_flight_show(struct kobject *kobj,
				       struct attribute *attr,
				       char *buf)
{
	struct obd_device *obd = container_of(kobj, struct obd_device,
					      obd_kobj);

	return sprintf(buf, "%u\n",
		       atomic_read(&obd->u.cli.cl_destroy_in_flight));
}
LUSTRE_RO_ATTR(destroys_in_flight);

static ssize_t max_pages_per_rpc_show(struct kobject *kobj,
				      struct attribute *attr,
				      char *buf)
{
	struct obd_device *dev = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct client_obd *cli = &dev->u.cli;

	return sprintf(buf, "%d\n", cli->cl_max_pages_per_rpc);
}

static ssize_t max_pages_per_rpc_store(struct kobject *kobj,
				       struct attribute *attr,
				       const char *buffer,
				       size_t count)
{
	struct obd_device *dev = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct client_obd *cli = &dev->u.cli;
	struct obd_connect_data *ocd = &cli->cl_import->imp_connect_data;
	int chunk_mask, rc;
	unsigned long long val;

	rc = kstrtoull(buffer, 10, &val);
>>>>>>> v4.9.227
	if (rc)
		return rc;

	/* if the max_pages is specified in bytes, convert to pages */
	if (val >= ONE_MB_BRW_SIZE)
<<<<<<< HEAD
		val >>= PAGE_CACHE_SHIFT;

	LPROCFS_CLIMP_CHECK(dev);

	chunk_mask = ~((1 << (cli->cl_chunkbits - PAGE_CACHE_SHIFT)) - 1);
	/* max_pages_per_rpc must be chunk aligned */
	val = (val + ~chunk_mask) & chunk_mask;
	if (val == 0 || val > ocd->ocd_brw_size >> PAGE_CACHE_SHIFT) {
		LPROCFS_CLIMP_EXIT(dev);
		return -ERANGE;
	}
	client_obd_list_lock(&cli->cl_loi_list_lock);
	cli->cl_max_pages_per_rpc = val;
	client_obd_list_unlock(&cli->cl_loi_list_lock);

	LPROCFS_CLIMP_EXIT(dev);
	return count;
}
LPROC_SEQ_FOPS(osc_obd_max_pages_per_rpc);

LPROC_SEQ_FOPS_RO_TYPE(osc, uuid);
LPROC_SEQ_FOPS_RO_TYPE(osc, connect_flags);
LPROC_SEQ_FOPS_RO_TYPE(osc, blksize);
LPROC_SEQ_FOPS_RO_TYPE(osc, kbytestotal);
LPROC_SEQ_FOPS_RO_TYPE(osc, kbytesfree);
LPROC_SEQ_FOPS_RO_TYPE(osc, kbytesavail);
LPROC_SEQ_FOPS_RO_TYPE(osc, filestotal);
LPROC_SEQ_FOPS_RO_TYPE(osc, filesfree);
=======
		val >>= PAGE_SHIFT;

	chunk_mask = ~((1 << (cli->cl_chunkbits - PAGE_SHIFT)) - 1);
	/* max_pages_per_rpc must be chunk aligned */
	val = (val + ~chunk_mask) & chunk_mask;
	if (val == 0 || val > ocd->ocd_brw_size >> PAGE_SHIFT) {
		return -ERANGE;
	}
	spin_lock(&cli->cl_loi_list_lock);
	cli->cl_max_pages_per_rpc = val;
	client_adjust_max_dirty(cli);
	spin_unlock(&cli->cl_loi_list_lock);

	return count;
}
LUSTRE_RW_ATTR(max_pages_per_rpc);

static ssize_t unstable_stats_show(struct kobject *kobj,
				   struct attribute *attr,
				   char *buf)
{
	struct obd_device *dev = container_of(kobj, struct obd_device,
					      obd_kobj);
	struct client_obd *cli = &dev->u.cli;
	long pages;
	int mb;

	pages = atomic_long_read(&cli->cl_unstable_count);
	mb = (pages * PAGE_SIZE) >> 20;

	return sprintf(buf, "unstable_pages: %20ld\n"
		       "unstable_mb:              %10d\n", pages, mb);
}
LUSTRE_RO_ATTR(unstable_stats);

LPROC_SEQ_FOPS_RO_TYPE(osc, connect_flags);
>>>>>>> v4.9.227
LPROC_SEQ_FOPS_RO_TYPE(osc, server_uuid);
LPROC_SEQ_FOPS_RO_TYPE(osc, conn_uuid);
LPROC_SEQ_FOPS_RO_TYPE(osc, timeouts);
LPROC_SEQ_FOPS_RO_TYPE(osc, state);

LPROC_SEQ_FOPS_WR_ONLY(osc, ping);

LPROC_SEQ_FOPS_RW_TYPE(osc, import);
LPROC_SEQ_FOPS_RW_TYPE(osc, pinger_recov);

static struct lprocfs_vars lprocfs_osc_obd_vars[] = {
<<<<<<< HEAD
	{ "uuid",	     &osc_uuid_fops,	NULL, 0 },
	{ "ping",	     &osc_ping_fops,    NULL, 0222 },
	{ "connect_flags",   &osc_connect_flags_fops, NULL, 0 },
	{ "blocksize",       &osc_blksize_fops,     NULL, 0 },
	{ "kbytestotal",     &osc_kbytestotal_fops, NULL, 0 },
	{ "kbytesfree",      &osc_kbytesfree_fops,  NULL, 0 },
	{ "kbytesavail",     &osc_kbytesavail_fops, NULL, 0 },
	{ "filestotal",      &osc_filestotal_fops,  NULL, 0 },
	{ "filesfree",       &osc_filesfree_fops,   NULL, 0 },
	/*{ "filegroups",      lprocfs_rd_filegroups,  NULL, 0 },*/
	{ "ost_server_uuid", &osc_server_uuid_fops, NULL, 0 },
	{ "ost_conn_uuid",   &osc_conn_uuid_fops, NULL, 0 },
	{ "active",	     &osc_active_fops, NULL },
	{ "max_pages_per_rpc", &osc_obd_max_pages_per_rpc_fops, NULL },
	{ "max_rpcs_in_flight", &osc_max_rpcs_in_flight_fops, NULL },
	{ "destroys_in_flight", &osc_destroys_in_flight_fops, NULL, 0 },
	{ "max_dirty_mb",    &osc_max_dirty_mb_fops, NULL },
	{ "osc_cached_mb",   &osc_cached_mb_fops, NULL },
	{ "cur_dirty_bytes", &osc_cur_dirty_bytes_fops, NULL, 0 },
	{ "cur_grant_bytes", &osc_cur_grant_bytes_fops, NULL },
	{ "cur_lost_grant_bytes", &osc_cur_lost_grant_bytes_fops, NULL, 0},
	{ "grant_shrink_interval", &osc_grant_shrink_interval_fops, NULL },
	{ "checksums",       &osc_checksum_fops, NULL },
	{ "checksum_type",   &osc_checksum_type_fops, NULL },
	{ "resend_count",    &osc_resend_count_fops, NULL},
	{ "timeouts",	     &osc_timeouts_fops, NULL, 0 },
	{ "contention_seconds", &osc_contention_seconds_fops, NULL },
	{ "lockless_truncate",  &osc_lockless_truncate_fops, NULL },
=======
	{ "ping",	     &osc_ping_fops,    NULL, 0222 },
	{ "connect_flags",   &osc_connect_flags_fops, NULL, 0 },
	/*{ "filegroups",      lprocfs_rd_filegroups,  NULL, 0 },*/
	{ "ost_server_uuid", &osc_server_uuid_fops, NULL, 0 },
	{ "ost_conn_uuid",   &osc_conn_uuid_fops, NULL, 0 },
	{ "osc_cached_mb",   &osc_cached_mb_fops, NULL },
	{ "checksum_type",   &osc_checksum_type_fops, NULL },
	{ "timeouts",	     &osc_timeouts_fops, NULL, 0 },
>>>>>>> v4.9.227
	{ "import",		&osc_import_fops, NULL },
	{ "state",		&osc_state_fops, NULL, 0 },
	{ "pinger_recov",	&osc_pinger_recov_fops, NULL },
	{ NULL }
};

<<<<<<< HEAD
LPROC_SEQ_FOPS_RO_TYPE(osc, numrefs);
static struct lprocfs_vars lprocfs_osc_module_vars[] = {
	{ "num_refs",	&osc_numrefs_fops,     NULL, 0 },
	{ NULL }
};

=======
>>>>>>> v4.9.227
#define pct(a, b) (b ? a * 100 / b : 0)

static int osc_rpc_stats_seq_show(struct seq_file *seq, void *v)
{
<<<<<<< HEAD
	struct timeval now;
=======
	struct timespec64 now;
>>>>>>> v4.9.227
	struct obd_device *dev = seq->private;
	struct client_obd *cli = &dev->u.cli;
	unsigned long read_tot = 0, write_tot = 0, read_cum, write_cum;
	int i;

<<<<<<< HEAD
	do_gettimeofday(&now);

	client_obd_list_lock(&cli->cl_loi_list_lock);

	seq_printf(seq, "snapshot_time:	 %lu.%lu (secs.usecs)\n",
		   now.tv_sec, (unsigned long)now.tv_usec);
=======
	ktime_get_real_ts64(&now);

	spin_lock(&cli->cl_loi_list_lock);

	seq_printf(seq, "snapshot_time:	 %llu.%9lu (secs.usecs)\n",
		   (s64)now.tv_sec, (unsigned long)now.tv_nsec);
>>>>>>> v4.9.227
	seq_printf(seq, "read RPCs in flight:  %d\n",
		   cli->cl_r_in_flight);
	seq_printf(seq, "write RPCs in flight: %d\n",
		   cli->cl_w_in_flight);
	seq_printf(seq, "pending write pages:  %d\n",
		   atomic_read(&cli->cl_pending_w_pages));
	seq_printf(seq, "pending read pages:   %d\n",
		   atomic_read(&cli->cl_pending_r_pages));

<<<<<<< HEAD
	seq_printf(seq, "\n\t\t\tread\t\t\twrite\n");
	seq_printf(seq, "pages per rpc	 rpcs   %% cum %% |");
	seq_printf(seq, "       rpcs   %% cum %%\n");
=======
	seq_puts(seq, "\n\t\t\tread\t\t\twrite\n");
	seq_puts(seq, "pages per rpc	 rpcs   % cum % |");
	seq_puts(seq, "       rpcs   % cum %\n");
>>>>>>> v4.9.227

	read_tot = lprocfs_oh_sum(&cli->cl_read_page_hist);
	write_tot = lprocfs_oh_sum(&cli->cl_write_page_hist);

	read_cum = 0;
	write_cum = 0;
	for (i = 0; i < OBD_HIST_MAX; i++) {
		unsigned long r = cli->cl_read_page_hist.oh_buckets[i];
		unsigned long w = cli->cl_write_page_hist.oh_buckets[i];
<<<<<<< HEAD
		read_cum += r;
		write_cum += w;
		seq_printf(seq, "%d:\t\t%10lu %3lu %3lu   | %10lu %3lu %3lu\n",
				 1 << i, r, pct(r, read_tot),
				 pct(read_cum, read_tot), w,
				 pct(w, write_tot),
				 pct(write_cum, write_tot));
=======

		read_cum += r;
		write_cum += w;
		seq_printf(seq, "%d:\t\t%10lu %3lu %3lu   | %10lu %3lu %3lu\n",
			   1 << i, r, pct(r, read_tot),
			   pct(read_cum, read_tot), w,
			   pct(w, write_tot),
			   pct(write_cum, write_tot));
>>>>>>> v4.9.227
		if (read_cum == read_tot && write_cum == write_tot)
			break;
	}

<<<<<<< HEAD
	seq_printf(seq, "\n\t\t\tread\t\t\twrite\n");
	seq_printf(seq, "rpcs in flight	rpcs   %% cum %% |");
	seq_printf(seq, "       rpcs   %% cum %%\n");
=======
	seq_puts(seq, "\n\t\t\tread\t\t\twrite\n");
	seq_puts(seq, "rpcs in flight	rpcs   % cum % |");
	seq_puts(seq, "       rpcs   % cum %\n");
>>>>>>> v4.9.227

	read_tot = lprocfs_oh_sum(&cli->cl_read_rpc_hist);
	write_tot = lprocfs_oh_sum(&cli->cl_write_rpc_hist);

	read_cum = 0;
	write_cum = 0;
	for (i = 0; i < OBD_HIST_MAX; i++) {
		unsigned long r = cli->cl_read_rpc_hist.oh_buckets[i];
		unsigned long w = cli->cl_write_rpc_hist.oh_buckets[i];
<<<<<<< HEAD
		read_cum += r;
		write_cum += w;
		seq_printf(seq, "%d:\t\t%10lu %3lu %3lu   | %10lu %3lu %3lu\n",
				 i, r, pct(r, read_tot),
				 pct(read_cum, read_tot), w,
				 pct(w, write_tot),
				 pct(write_cum, write_tot));
=======

		read_cum += r;
		write_cum += w;
		seq_printf(seq, "%d:\t\t%10lu %3lu %3lu   | %10lu %3lu %3lu\n",
			   i, r, pct(r, read_tot),
			   pct(read_cum, read_tot), w,
			   pct(w, write_tot),
			   pct(write_cum, write_tot));
>>>>>>> v4.9.227
		if (read_cum == read_tot && write_cum == write_tot)
			break;
	}

<<<<<<< HEAD
	seq_printf(seq, "\n\t\t\tread\t\t\twrite\n");
	seq_printf(seq, "offset		rpcs   %% cum %% |");
	seq_printf(seq, "       rpcs   %% cum %%\n");
=======
	seq_puts(seq, "\n\t\t\tread\t\t\twrite\n");
	seq_puts(seq, "offset		rpcs   % cum % |");
	seq_puts(seq, "       rpcs   % cum %\n");
>>>>>>> v4.9.227

	read_tot = lprocfs_oh_sum(&cli->cl_read_offset_hist);
	write_tot = lprocfs_oh_sum(&cli->cl_write_offset_hist);

	read_cum = 0;
	write_cum = 0;
	for (i = 0; i < OBD_HIST_MAX; i++) {
		unsigned long r = cli->cl_read_offset_hist.oh_buckets[i];
		unsigned long w = cli->cl_write_offset_hist.oh_buckets[i];
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
		read_cum += r;
		write_cum += w;
		seq_printf(seq, "%d:\t\t%10lu %3lu %3lu   | %10lu %3lu %3lu\n",
			   (i == 0) ? 0 : 1 << (i - 1),
			   r, pct(r, read_tot), pct(read_cum, read_tot),
			   w, pct(w, write_tot), pct(write_cum, write_tot));
		if (read_cum == read_tot && write_cum == write_tot)
			break;
	}

<<<<<<< HEAD
	client_obd_list_unlock(&cli->cl_loi_list_lock);

	return 0;
}
#undef pct

static ssize_t osc_rpc_stats_seq_write(struct file *file, const char *buf,
=======
	spin_unlock(&cli->cl_loi_list_lock);

	return 0;
}

#undef pct

static ssize_t osc_rpc_stats_seq_write(struct file *file,
				       const char __user *buf,
>>>>>>> v4.9.227
				       size_t len, loff_t *off)
{
	struct seq_file *seq = file->private_data;
	struct obd_device *dev = seq->private;
	struct client_obd *cli = &dev->u.cli;

	lprocfs_oh_clear(&cli->cl_read_rpc_hist);
	lprocfs_oh_clear(&cli->cl_write_rpc_hist);
	lprocfs_oh_clear(&cli->cl_read_page_hist);
	lprocfs_oh_clear(&cli->cl_write_page_hist);
	lprocfs_oh_clear(&cli->cl_read_offset_hist);
	lprocfs_oh_clear(&cli->cl_write_offset_hist);

	return len;
}

LPROC_SEQ_FOPS(osc_rpc_stats);

static int osc_stats_seq_show(struct seq_file *seq, void *v)
{
<<<<<<< HEAD
	struct timeval now;
	struct obd_device *dev = seq->private;
	struct osc_stats *stats = &obd2osc_dev(dev)->od_stats;

	do_gettimeofday(&now);

	seq_printf(seq, "snapshot_time:	 %lu.%lu (secs.usecs)\n",
		   now.tv_sec, (unsigned long)now.tv_usec);
=======
	struct timespec64 now;
	struct obd_device *dev = seq->private;
	struct osc_stats *stats = &obd2osc_dev(dev)->od_stats;

	ktime_get_real_ts64(&now);

	seq_printf(seq, "snapshot_time:	 %llu.%9lu (secs.usecs)\n",
		   (s64)now.tv_sec, (unsigned long)now.tv_nsec);
>>>>>>> v4.9.227
	seq_printf(seq, "lockless_write_bytes\t\t%llu\n",
		   stats->os_lockless_writes);
	seq_printf(seq, "lockless_read_bytes\t\t%llu\n",
		   stats->os_lockless_reads);
	seq_printf(seq, "lockless_truncate\t\t%llu\n",
		   stats->os_lockless_truncates);
	return 0;
}

<<<<<<< HEAD
static ssize_t osc_stats_seq_write(struct file *file, const char *buf,
=======
static ssize_t osc_stats_seq_write(struct file *file,
				   const char __user *buf,
>>>>>>> v4.9.227
				   size_t len, loff_t *off)
{
	struct seq_file *seq = file->private_data;
	struct obd_device *dev = seq->private;
	struct osc_stats *stats = &obd2osc_dev(dev)->od_stats;

	memset(stats, 0, sizeof(*stats));
	return len;
}

LPROC_SEQ_FOPS(osc_stats);

int lproc_osc_attach_seqstat(struct obd_device *dev)
{
	int rc;

<<<<<<< HEAD
	rc = lprocfs_seq_create(dev->obd_proc_entry, "osc_stats", 0644,
				&osc_stats_fops, dev);
	if (rc == 0)
		rc = lprocfs_obd_seq_create(dev, "rpc_stats", 0644,
					    &osc_rpc_stats_fops, dev);
=======
	rc = ldebugfs_seq_create(dev->obd_debugfs_entry, "osc_stats", 0644,
				 &osc_stats_fops, dev);
	if (rc == 0)
		rc = ldebugfs_obd_seq_create(dev, "rpc_stats", 0644,
					     &osc_rpc_stats_fops, dev);
>>>>>>> v4.9.227

	return rc;
}

<<<<<<< HEAD
void lprocfs_osc_init_vars(struct lprocfs_static_vars *lvars)
{
	lvars->module_vars = lprocfs_osc_module_vars;
=======
static struct attribute *osc_attrs[] = {
	&lustre_attr_active.attr,
	&lustre_attr_checksums.attr,
	&lustre_attr_contention_seconds.attr,
	&lustre_attr_cur_dirty_bytes.attr,
	&lustre_attr_cur_grant_bytes.attr,
	&lustre_attr_cur_lost_grant_bytes.attr,
	&lustre_attr_destroys_in_flight.attr,
	&lustre_attr_grant_shrink_interval.attr,
	&lustre_attr_lockless_truncate.attr,
	&lustre_attr_max_dirty_mb.attr,
	&lustre_attr_max_pages_per_rpc.attr,
	&lustre_attr_max_rpcs_in_flight.attr,
	&lustre_attr_resend_count.attr,
	&lustre_attr_unstable_stats.attr,
	NULL,
};

static struct attribute_group osc_attr_group = {
	.attrs = osc_attrs,
};

void lprocfs_osc_init_vars(struct lprocfs_static_vars *lvars)
{
	lvars->sysfs_vars  = &osc_attr_group;
>>>>>>> v4.9.227
	lvars->obd_vars    = lprocfs_osc_obd_vars;
}
