/*******************************************************************************
 * Filename:  target_core_stat.c
 *
 * Modern ConfigFS group context specific statistics based on original
 * target_core_mib.c code
 *
 * (c) Copyright 2006-2013 Datera, Inc.
 *
 * Nicholas A. Bellinger <nab@linux-iscsi.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 ******************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/string.h>
#include <linux/utsname.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/configfs.h>
<<<<<<< HEAD
#include <scsi/scsi.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_host.h>
=======
>>>>>>> v4.9.227

#include <target/target_core_base.h>
#include <target/target_core_backend.h>
#include <target/target_core_fabric.h>
<<<<<<< HEAD
#include <target/target_core_configfs.h>
#include <target/configfs_macros.h>
=======
>>>>>>> v4.9.227

#include "target_core_internal.h"

#ifndef INITIAL_JIFFIES
#define INITIAL_JIFFIES ((unsigned long)(unsigned int) (-300*HZ))
#endif

#define NONE		"None"
#define ISPRINT(a)   ((a >= ' ') && (a <= '~'))

#define SCSI_LU_INDEX			1
#define LU_COUNT			1

/*
 * SCSI Device Table
 */

<<<<<<< HEAD
CONFIGFS_EATTR_STRUCT(target_stat_scsi_dev, se_dev_stat_grps);
#define DEV_STAT_SCSI_DEV_ATTR(_name, _mode)				\
static struct target_stat_scsi_dev_attribute				\
			target_stat_scsi_dev_##_name =			\
	__CONFIGFS_EATTR(_name, _mode,					\
	target_stat_scsi_dev_show_attr_##_name,				\
	target_stat_scsi_dev_store_attr_##_name);

#define DEV_STAT_SCSI_DEV_ATTR_RO(_name)				\
static struct target_stat_scsi_dev_attribute				\
			target_stat_scsi_dev_##_name =			\
	__CONFIGFS_EATTR_RO(_name,					\
	target_stat_scsi_dev_show_attr_##_name);

static ssize_t target_stat_scsi_dev_show_attr_inst(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
	struct se_hba *hba = dev->se_hba;

	return snprintf(page, PAGE_SIZE, "%u\n", hba->hba_index);
}
DEV_STAT_SCSI_DEV_ATTR_RO(inst);

static ssize_t target_stat_scsi_dev_show_attr_indx(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);

	return snprintf(page, PAGE_SIZE, "%u\n", dev->dev_index);
}
DEV_STAT_SCSI_DEV_ATTR_RO(indx);

static ssize_t target_stat_scsi_dev_show_attr_role(
	struct se_dev_stat_grps *sgrps, char *page)
{
	return snprintf(page, PAGE_SIZE, "Target\n");
}
DEV_STAT_SCSI_DEV_ATTR_RO(role);

static ssize_t target_stat_scsi_dev_show_attr_ports(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);

	return snprintf(page, PAGE_SIZE, "%u\n", dev->dev_port_count);
}
DEV_STAT_SCSI_DEV_ATTR_RO(ports);

CONFIGFS_EATTR_OPS(target_stat_scsi_dev, se_dev_stat_grps, scsi_dev_group);

static struct configfs_attribute *target_stat_scsi_dev_attrs[] = {
	&target_stat_scsi_dev_inst.attr,
	&target_stat_scsi_dev_indx.attr,
	&target_stat_scsi_dev_role.attr,
	&target_stat_scsi_dev_ports.attr,
	NULL,
};

static struct configfs_item_operations target_stat_scsi_dev_attrib_ops = {
	.show_attribute		= target_stat_scsi_dev_attr_show,
	.store_attribute	= target_stat_scsi_dev_attr_store,
};

static struct config_item_type target_stat_scsi_dev_cit = {
	.ct_item_ops		= &target_stat_scsi_dev_attrib_ops,
=======
static struct se_device *to_stat_dev(struct config_item *item)
{
	struct se_dev_stat_grps *sgrps = container_of(to_config_group(item),
			struct se_dev_stat_grps, scsi_dev_group);
	return container_of(sgrps, struct se_device, dev_stat_grps);
}

static ssize_t target_stat_inst_show(struct config_item *item, char *page)
{
	struct se_hba *hba = to_stat_dev(item)->se_hba;

	return snprintf(page, PAGE_SIZE, "%u\n", hba->hba_index);
}

static ssize_t target_stat_indx_show(struct config_item *item, char *page)
{
	return snprintf(page, PAGE_SIZE, "%u\n", to_stat_dev(item)->dev_index);
}

static ssize_t target_stat_role_show(struct config_item *item, char *page)
{
	return snprintf(page, PAGE_SIZE, "Target\n");
}

static ssize_t target_stat_ports_show(struct config_item *item, char *page)
{
	return snprintf(page, PAGE_SIZE, "%u\n", to_stat_dev(item)->export_count);
}

CONFIGFS_ATTR_RO(target_stat_, inst);
CONFIGFS_ATTR_RO(target_stat_, indx);
CONFIGFS_ATTR_RO(target_stat_, role);
CONFIGFS_ATTR_RO(target_stat_, ports);

static struct configfs_attribute *target_stat_scsi_dev_attrs[] = {
	&target_stat_attr_inst,
	&target_stat_attr_indx,
	&target_stat_attr_role,
	&target_stat_attr_ports,
	NULL,
};

static struct config_item_type target_stat_scsi_dev_cit = {
>>>>>>> v4.9.227
	.ct_attrs		= target_stat_scsi_dev_attrs,
	.ct_owner		= THIS_MODULE,
};

/*
 * SCSI Target Device Table
 */
<<<<<<< HEAD

CONFIGFS_EATTR_STRUCT(target_stat_scsi_tgt_dev, se_dev_stat_grps);
#define DEV_STAT_SCSI_TGT_DEV_ATTR(_name, _mode)			\
static struct target_stat_scsi_tgt_dev_attribute			\
			target_stat_scsi_tgt_dev_##_name =		\
	__CONFIGFS_EATTR(_name, _mode,					\
	target_stat_scsi_tgt_dev_show_attr_##_name,			\
	target_stat_scsi_tgt_dev_store_attr_##_name);

#define DEV_STAT_SCSI_TGT_DEV_ATTR_RO(_name)				\
static struct target_stat_scsi_tgt_dev_attribute			\
			target_stat_scsi_tgt_dev_##_name =		\
	__CONFIGFS_EATTR_RO(_name,					\
	target_stat_scsi_tgt_dev_show_attr_##_name);

static ssize_t target_stat_scsi_tgt_dev_show_attr_inst(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
	struct se_hba *hba = dev->se_hba;

	return snprintf(page, PAGE_SIZE, "%u\n", hba->hba_index);
}
DEV_STAT_SCSI_TGT_DEV_ATTR_RO(inst);

static ssize_t target_stat_scsi_tgt_dev_show_attr_indx(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);

	return snprintf(page, PAGE_SIZE, "%u\n", dev->dev_index);
}
DEV_STAT_SCSI_TGT_DEV_ATTR_RO(indx);

static ssize_t target_stat_scsi_tgt_dev_show_attr_num_lus(
	struct se_dev_stat_grps *sgrps, char *page)
{
	return snprintf(page, PAGE_SIZE, "%u\n", LU_COUNT);
}
DEV_STAT_SCSI_TGT_DEV_ATTR_RO(num_lus);

static ssize_t target_stat_scsi_tgt_dev_show_attr_status(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);

	if (dev->export_count)
=======
static struct se_device *to_stat_tgt_dev(struct config_item *item)
{
	struct se_dev_stat_grps *sgrps = container_of(to_config_group(item),
			struct se_dev_stat_grps, scsi_tgt_dev_group);
	return container_of(sgrps, struct se_device, dev_stat_grps);
}

static ssize_t target_stat_tgt_inst_show(struct config_item *item, char *page)
{
	struct se_hba *hba = to_stat_tgt_dev(item)->se_hba;

	return snprintf(page, PAGE_SIZE, "%u\n", hba->hba_index);
}

static ssize_t target_stat_tgt_indx_show(struct config_item *item, char *page)
{
	return snprintf(page, PAGE_SIZE, "%u\n", to_stat_tgt_dev(item)->dev_index);
}

static ssize_t target_stat_tgt_num_lus_show(struct config_item *item,
		char *page)
{
	return snprintf(page, PAGE_SIZE, "%u\n", LU_COUNT);
}

static ssize_t target_stat_tgt_status_show(struct config_item *item,
		char *page)
{
	if (to_stat_tgt_dev(item)->export_count)
>>>>>>> v4.9.227
		return snprintf(page, PAGE_SIZE, "activated");
	else
		return snprintf(page, PAGE_SIZE, "deactivated");
}
<<<<<<< HEAD
DEV_STAT_SCSI_TGT_DEV_ATTR_RO(status);

static ssize_t target_stat_scsi_tgt_dev_show_attr_non_access_lus(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
	int non_accessible_lus;

	if (dev->export_count)
=======

static ssize_t target_stat_tgt_non_access_lus_show(struct config_item *item,
		char *page)
{
	int non_accessible_lus;

	if (to_stat_tgt_dev(item)->export_count)
>>>>>>> v4.9.227
		non_accessible_lus = 0;
	else
		non_accessible_lus = 1;

	return snprintf(page, PAGE_SIZE, "%u\n", non_accessible_lus);
}
<<<<<<< HEAD
DEV_STAT_SCSI_TGT_DEV_ATTR_RO(non_access_lus);

static ssize_t target_stat_scsi_tgt_dev_show_attr_resets(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);

	return snprintf(page, PAGE_SIZE, "%lu\n",
			atomic_long_read(&dev->num_resets));
}
DEV_STAT_SCSI_TGT_DEV_ATTR_RO(resets);


CONFIGFS_EATTR_OPS(target_stat_scsi_tgt_dev, se_dev_stat_grps, scsi_tgt_dev_group);

static struct configfs_attribute *target_stat_scsi_tgt_dev_attrs[] = {
	&target_stat_scsi_tgt_dev_inst.attr,
	&target_stat_scsi_tgt_dev_indx.attr,
	&target_stat_scsi_tgt_dev_num_lus.attr,
	&target_stat_scsi_tgt_dev_status.attr,
	&target_stat_scsi_tgt_dev_non_access_lus.attr,
	&target_stat_scsi_tgt_dev_resets.attr,
	NULL,
};

static struct configfs_item_operations target_stat_scsi_tgt_dev_attrib_ops = {
	.show_attribute		= target_stat_scsi_tgt_dev_attr_show,
	.store_attribute	= target_stat_scsi_tgt_dev_attr_store,
};

static struct config_item_type target_stat_scsi_tgt_dev_cit = {
	.ct_item_ops		= &target_stat_scsi_tgt_dev_attrib_ops,
=======

static ssize_t target_stat_tgt_resets_show(struct config_item *item,
		char *page)
{
	return snprintf(page, PAGE_SIZE, "%lu\n",
			atomic_long_read(&to_stat_tgt_dev(item)->num_resets));
}

CONFIGFS_ATTR_RO(target_stat_tgt_, inst);
CONFIGFS_ATTR_RO(target_stat_tgt_, indx);
CONFIGFS_ATTR_RO(target_stat_tgt_, num_lus);
CONFIGFS_ATTR_RO(target_stat_tgt_, status);
CONFIGFS_ATTR_RO(target_stat_tgt_, non_access_lus);
CONFIGFS_ATTR_RO(target_stat_tgt_, resets);

static struct configfs_attribute *target_stat_scsi_tgt_dev_attrs[] = {
	&target_stat_tgt_attr_inst,
	&target_stat_tgt_attr_indx,
	&target_stat_tgt_attr_num_lus,
	&target_stat_tgt_attr_status,
	&target_stat_tgt_attr_non_access_lus,
	&target_stat_tgt_attr_resets,
	NULL,
};

static struct config_item_type target_stat_scsi_tgt_dev_cit = {
>>>>>>> v4.9.227
	.ct_attrs		= target_stat_scsi_tgt_dev_attrs,
	.ct_owner		= THIS_MODULE,
};

/*
 * SCSI Logical Unit Table
 */

<<<<<<< HEAD
CONFIGFS_EATTR_STRUCT(target_stat_scsi_lu, se_dev_stat_grps);
#define DEV_STAT_SCSI_LU_ATTR(_name, _mode)				\
static struct target_stat_scsi_lu_attribute target_stat_scsi_lu_##_name = \
	__CONFIGFS_EATTR(_name, _mode,					\
	target_stat_scsi_lu_show_attr_##_name,				\
	target_stat_scsi_lu_store_attr_##_name);

#define DEV_STAT_SCSI_LU_ATTR_RO(_name)					\
static struct target_stat_scsi_lu_attribute target_stat_scsi_lu_##_name = \
	__CONFIGFS_EATTR_RO(_name,					\
	target_stat_scsi_lu_show_attr_##_name);

static ssize_t target_stat_scsi_lu_show_attr_inst(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
	struct se_hba *hba = dev->se_hba;

	return snprintf(page, PAGE_SIZE, "%u\n", hba->hba_index);
}
DEV_STAT_SCSI_LU_ATTR_RO(inst);

static ssize_t target_stat_scsi_lu_show_attr_dev(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);

	return snprintf(page, PAGE_SIZE, "%u\n", dev->dev_index);
}
DEV_STAT_SCSI_LU_ATTR_RO(dev);

static ssize_t target_stat_scsi_lu_show_attr_indx(
	struct se_dev_stat_grps *sgrps, char *page)
{
	return snprintf(page, PAGE_SIZE, "%u\n", SCSI_LU_INDEX);
}
DEV_STAT_SCSI_LU_ATTR_RO(indx);

static ssize_t target_stat_scsi_lu_show_attr_lun(
	struct se_dev_stat_grps *sgrps, char *page)
=======
static struct se_device *to_stat_lu_dev(struct config_item *item)
{
	struct se_dev_stat_grps *sgrps = container_of(to_config_group(item),
			struct se_dev_stat_grps, scsi_lu_group);
	return container_of(sgrps, struct se_device, dev_stat_grps);
}

static ssize_t target_stat_lu_inst_show(struct config_item *item, char *page)
{
	struct se_hba *hba = to_stat_lu_dev(item)->se_hba;

	return snprintf(page, PAGE_SIZE, "%u\n", hba->hba_index);
}

static ssize_t target_stat_lu_dev_show(struct config_item *item, char *page)
{
	return snprintf(page, PAGE_SIZE, "%u\n",
			to_stat_lu_dev(item)->dev_index);
}

static ssize_t target_stat_lu_indx_show(struct config_item *item, char *page)
{
	return snprintf(page, PAGE_SIZE, "%u\n", SCSI_LU_INDEX);
}

static ssize_t target_stat_lu_lun_show(struct config_item *item, char *page)
>>>>>>> v4.9.227
{
	/* FIXME: scsiLuDefaultLun */
	return snprintf(page, PAGE_SIZE, "%llu\n", (unsigned long long)0);
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(lun);

static ssize_t target_stat_scsi_lu_show_attr_lu_name(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
=======

static ssize_t target_stat_lu_lu_name_show(struct config_item *item, char *page)
{
	struct se_device *dev = to_stat_lu_dev(item);
>>>>>>> v4.9.227

	/* scsiLuWwnName */
	return snprintf(page, PAGE_SIZE, "%s\n",
			(strlen(dev->t10_wwn.unit_serial)) ?
			dev->t10_wwn.unit_serial : "None");
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(lu_name);

static ssize_t target_stat_scsi_lu_show_attr_vend(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
=======

static ssize_t target_stat_lu_vend_show(struct config_item *item, char *page)
{
	struct se_device *dev = to_stat_lu_dev(item);
>>>>>>> v4.9.227
	int i;
	char str[sizeof(dev->t10_wwn.vendor)+1];

	/* scsiLuVendorId */
	for (i = 0; i < sizeof(dev->t10_wwn.vendor); i++)
		str[i] = ISPRINT(dev->t10_wwn.vendor[i]) ?
			dev->t10_wwn.vendor[i] : ' ';
	str[i] = '\0';
	return snprintf(page, PAGE_SIZE, "%s\n", str);
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(vend);

static ssize_t target_stat_scsi_lu_show_attr_prod(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
=======

static ssize_t target_stat_lu_prod_show(struct config_item *item, char *page)
{
	struct se_device *dev = to_stat_lu_dev(item);
>>>>>>> v4.9.227
	int i;
	char str[sizeof(dev->t10_wwn.model)+1];

	/* scsiLuProductId */
<<<<<<< HEAD
	for (i = 0; i < sizeof(dev->t10_wwn.vendor); i++)
=======
	for (i = 0; i < sizeof(dev->t10_wwn.model); i++)
>>>>>>> v4.9.227
		str[i] = ISPRINT(dev->t10_wwn.model[i]) ?
			dev->t10_wwn.model[i] : ' ';
	str[i] = '\0';
	return snprintf(page, PAGE_SIZE, "%s\n", str);
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(prod);

static ssize_t target_stat_scsi_lu_show_attr_rev(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
=======

static ssize_t target_stat_lu_rev_show(struct config_item *item, char *page)
{
	struct se_device *dev = to_stat_lu_dev(item);
>>>>>>> v4.9.227
	int i;
	char str[sizeof(dev->t10_wwn.revision)+1];

	/* scsiLuRevisionId */
	for (i = 0; i < sizeof(dev->t10_wwn.revision); i++)
		str[i] = ISPRINT(dev->t10_wwn.revision[i]) ?
			dev->t10_wwn.revision[i] : ' ';
	str[i] = '\0';
	return snprintf(page, PAGE_SIZE, "%s\n", str);
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(rev);

static ssize_t target_stat_scsi_lu_show_attr_dev_type(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
=======

static ssize_t target_stat_lu_dev_type_show(struct config_item *item, char *page)
{
	struct se_device *dev = to_stat_lu_dev(item);
>>>>>>> v4.9.227

	/* scsiLuPeripheralType */
	return snprintf(page, PAGE_SIZE, "%u\n",
			dev->transport->get_device_type(dev));
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(dev_type);

static ssize_t target_stat_scsi_lu_show_attr_status(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
=======

static ssize_t target_stat_lu_status_show(struct config_item *item, char *page)
{
	struct se_device *dev = to_stat_lu_dev(item);
>>>>>>> v4.9.227

	/* scsiLuStatus */
	return snprintf(page, PAGE_SIZE, "%s\n",
		(dev->export_count) ? "available" : "notavailable");
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(status);

static ssize_t target_stat_scsi_lu_show_attr_state_bit(
	struct se_dev_stat_grps *sgrps, char *page)
=======

static ssize_t target_stat_lu_state_bit_show(struct config_item *item,
		char *page)
>>>>>>> v4.9.227
{
	/* scsiLuState */
	return snprintf(page, PAGE_SIZE, "exposed\n");
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(state_bit);

static ssize_t target_stat_scsi_lu_show_attr_num_cmds(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
=======

static ssize_t target_stat_lu_num_cmds_show(struct config_item *item,
		char *page)
{
	struct se_device *dev = to_stat_lu_dev(item);
>>>>>>> v4.9.227

	/* scsiLuNumCommands */
	return snprintf(page, PAGE_SIZE, "%lu\n",
			atomic_long_read(&dev->num_cmds));
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(num_cmds);

static ssize_t target_stat_scsi_lu_show_attr_read_mbytes(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
=======

static ssize_t target_stat_lu_read_mbytes_show(struct config_item *item,
		char *page)
{
	struct se_device *dev = to_stat_lu_dev(item);
>>>>>>> v4.9.227

	/* scsiLuReadMegaBytes */
	return snprintf(page, PAGE_SIZE, "%lu\n",
			atomic_long_read(&dev->read_bytes) >> 20);
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(read_mbytes);

static ssize_t target_stat_scsi_lu_show_attr_write_mbytes(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
=======

static ssize_t target_stat_lu_write_mbytes_show(struct config_item *item,
		char *page)
{
	struct se_device *dev = to_stat_lu_dev(item);
>>>>>>> v4.9.227

	/* scsiLuWrittenMegaBytes */
	return snprintf(page, PAGE_SIZE, "%lu\n",
			atomic_long_read(&dev->write_bytes) >> 20);
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(write_mbytes);

static ssize_t target_stat_scsi_lu_show_attr_resets(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);

	/* scsiLuInResets */
	return snprintf(page, PAGE_SIZE, "%lu\n", atomic_long_read(&dev->num_resets));
}
DEV_STAT_SCSI_LU_ATTR_RO(resets);

static ssize_t target_stat_scsi_lu_show_attr_full_stat(
	struct se_dev_stat_grps *sgrps, char *page)
=======

static ssize_t target_stat_lu_resets_show(struct config_item *item, char *page)
{
	struct se_device *dev = to_stat_lu_dev(item);

	/* scsiLuInResets */
	return snprintf(page, PAGE_SIZE, "%lu\n",
		atomic_long_read(&dev->num_resets));
}

static ssize_t target_stat_lu_full_stat_show(struct config_item *item,
		char *page)
>>>>>>> v4.9.227
{
	/* FIXME: scsiLuOutTaskSetFullStatus */
	return snprintf(page, PAGE_SIZE, "%u\n", 0);
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(full_stat);

static ssize_t target_stat_scsi_lu_show_attr_hs_num_cmds(
	struct se_dev_stat_grps *sgrps, char *page)
=======

static ssize_t target_stat_lu_hs_num_cmds_show(struct config_item *item,
		char *page)
>>>>>>> v4.9.227
{
	/* FIXME: scsiLuHSInCommands */
	return snprintf(page, PAGE_SIZE, "%u\n", 0);
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(hs_num_cmds);

static ssize_t target_stat_scsi_lu_show_attr_creation_time(
	struct se_dev_stat_grps *sgrps, char *page)
{
	struct se_device *dev =
		container_of(sgrps, struct se_device, dev_stat_grps);
=======

static ssize_t target_stat_lu_creation_time_show(struct config_item *item,
		char *page)
{
	struct se_device *dev = to_stat_lu_dev(item);
>>>>>>> v4.9.227

	/* scsiLuCreationTime */
	return snprintf(page, PAGE_SIZE, "%u\n", (u32)(((u32)dev->creation_time -
				INITIAL_JIFFIES) * 100 / HZ));
}
<<<<<<< HEAD
DEV_STAT_SCSI_LU_ATTR_RO(creation_time);

CONFIGFS_EATTR_OPS(target_stat_scsi_lu, se_dev_stat_grps, scsi_lu_group);

static struct configfs_attribute *target_stat_scsi_lu_attrs[] = {
	&target_stat_scsi_lu_inst.attr,
	&target_stat_scsi_lu_dev.attr,
	&target_stat_scsi_lu_indx.attr,
	&target_stat_scsi_lu_lun.attr,
	&target_stat_scsi_lu_lu_name.attr,
	&target_stat_scsi_lu_vend.attr,
	&target_stat_scsi_lu_prod.attr,
	&target_stat_scsi_lu_rev.attr,
	&target_stat_scsi_lu_dev_type.attr,
	&target_stat_scsi_lu_status.attr,
	&target_stat_scsi_lu_state_bit.attr,
	&target_stat_scsi_lu_num_cmds.attr,
	&target_stat_scsi_lu_read_mbytes.attr,
	&target_stat_scsi_lu_write_mbytes.attr,
	&target_stat_scsi_lu_resets.attr,
	&target_stat_scsi_lu_full_stat.attr,
	&target_stat_scsi_lu_hs_num_cmds.attr,
	&target_stat_scsi_lu_creation_time.attr,
	NULL,
};

static struct configfs_item_operations target_stat_scsi_lu_attrib_ops = {
	.show_attribute		= target_stat_scsi_lu_attr_show,
	.store_attribute	= target_stat_scsi_lu_attr_store,
};

static struct config_item_type target_stat_scsi_lu_cit = {
	.ct_item_ops		= &target_stat_scsi_lu_attrib_ops,
=======

CONFIGFS_ATTR_RO(target_stat_lu_, inst);
CONFIGFS_ATTR_RO(target_stat_lu_, dev);
CONFIGFS_ATTR_RO(target_stat_lu_, indx);
CONFIGFS_ATTR_RO(target_stat_lu_, lun);
CONFIGFS_ATTR_RO(target_stat_lu_, lu_name);
CONFIGFS_ATTR_RO(target_stat_lu_, vend);
CONFIGFS_ATTR_RO(target_stat_lu_, prod);
CONFIGFS_ATTR_RO(target_stat_lu_, rev);
CONFIGFS_ATTR_RO(target_stat_lu_, dev_type);
CONFIGFS_ATTR_RO(target_stat_lu_, status);
CONFIGFS_ATTR_RO(target_stat_lu_, state_bit);
CONFIGFS_ATTR_RO(target_stat_lu_, num_cmds);
CONFIGFS_ATTR_RO(target_stat_lu_, read_mbytes);
CONFIGFS_ATTR_RO(target_stat_lu_, write_mbytes);
CONFIGFS_ATTR_RO(target_stat_lu_, resets);
CONFIGFS_ATTR_RO(target_stat_lu_, full_stat);
CONFIGFS_ATTR_RO(target_stat_lu_, hs_num_cmds);
CONFIGFS_ATTR_RO(target_stat_lu_, creation_time);

static struct configfs_attribute *target_stat_scsi_lu_attrs[] = {
	&target_stat_lu_attr_inst,
	&target_stat_lu_attr_dev,
	&target_stat_lu_attr_indx,
	&target_stat_lu_attr_lun,
	&target_stat_lu_attr_lu_name,
	&target_stat_lu_attr_vend,
	&target_stat_lu_attr_prod,
	&target_stat_lu_attr_rev,
	&target_stat_lu_attr_dev_type,
	&target_stat_lu_attr_status,
	&target_stat_lu_attr_state_bit,
	&target_stat_lu_attr_num_cmds,
	&target_stat_lu_attr_read_mbytes,
	&target_stat_lu_attr_write_mbytes,
	&target_stat_lu_attr_resets,
	&target_stat_lu_attr_full_stat,
	&target_stat_lu_attr_hs_num_cmds,
	&target_stat_lu_attr_creation_time,
	NULL,
};

static struct config_item_type target_stat_scsi_lu_cit = {
>>>>>>> v4.9.227
	.ct_attrs		= target_stat_scsi_lu_attrs,
	.ct_owner		= THIS_MODULE,
};

/*
 * Called from target_core_configfs.c:target_core_make_subdev() to setup
 * the target statistics groups + configfs CITs located in target_core_stat.c
 */
void target_stat_setup_dev_default_groups(struct se_device *dev)
{
<<<<<<< HEAD
	struct config_group *dev_stat_grp = &dev->dev_stat_grps.stat_group;

	config_group_init_type_name(&dev->dev_stat_grps.scsi_dev_group,
			"scsi_dev", &target_stat_scsi_dev_cit);
	config_group_init_type_name(&dev->dev_stat_grps.scsi_tgt_dev_group,
			"scsi_tgt_dev", &target_stat_scsi_tgt_dev_cit);
	config_group_init_type_name(&dev->dev_stat_grps.scsi_lu_group,
			"scsi_lu", &target_stat_scsi_lu_cit);

	dev_stat_grp->default_groups[0] = &dev->dev_stat_grps.scsi_dev_group;
	dev_stat_grp->default_groups[1] = &dev->dev_stat_grps.scsi_tgt_dev_group;
	dev_stat_grp->default_groups[2] = &dev->dev_stat_grps.scsi_lu_group;
	dev_stat_grp->default_groups[3] = NULL;
=======
	config_group_init_type_name(&dev->dev_stat_grps.scsi_dev_group,
			"scsi_dev", &target_stat_scsi_dev_cit);
	configfs_add_default_group(&dev->dev_stat_grps.scsi_dev_group,
			&dev->dev_stat_grps.stat_group);

	config_group_init_type_name(&dev->dev_stat_grps.scsi_tgt_dev_group,
			"scsi_tgt_dev", &target_stat_scsi_tgt_dev_cit);
	configfs_add_default_group(&dev->dev_stat_grps.scsi_tgt_dev_group,
			&dev->dev_stat_grps.stat_group);

	config_group_init_type_name(&dev->dev_stat_grps.scsi_lu_group,
			"scsi_lu", &target_stat_scsi_lu_cit);
	configfs_add_default_group(&dev->dev_stat_grps.scsi_lu_group,
			&dev->dev_stat_grps.stat_group);
>>>>>>> v4.9.227
}

/*
 * SCSI Port Table
 */

<<<<<<< HEAD
CONFIGFS_EATTR_STRUCT(target_stat_scsi_port, se_port_stat_grps);
#define DEV_STAT_SCSI_PORT_ATTR(_name, _mode)				\
static struct target_stat_scsi_port_attribute				\
			target_stat_scsi_port_##_name =			\
	__CONFIGFS_EATTR(_name, _mode,					\
	target_stat_scsi_port_show_attr_##_name,			\
	target_stat_scsi_port_store_attr_##_name);

#define DEV_STAT_SCSI_PORT_ATTR_RO(_name)				\
static struct target_stat_scsi_port_attribute				\
			target_stat_scsi_port_##_name =			\
	__CONFIGFS_EATTR_RO(_name,					\
	target_stat_scsi_port_show_attr_##_name);

static ssize_t target_stat_scsi_port_show_attr_inst(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	struct se_device *dev = lun->lun_se_dev;
	struct se_hba *hba;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	hba = dev->se_hba;
	ret = snprintf(page, PAGE_SIZE, "%u\n", hba->hba_index);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_PORT_ATTR_RO(inst);

static ssize_t target_stat_scsi_port_show_attr_dev(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	struct se_device *dev = lun->lun_se_dev;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	ret = snprintf(page, PAGE_SIZE, "%u\n", dev->dev_index);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_PORT_ATTR_RO(dev);

static ssize_t target_stat_scsi_port_show_attr_indx(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	ret = snprintf(page, PAGE_SIZE, "%u\n", sep->sep_index);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_PORT_ATTR_RO(indx);

static ssize_t target_stat_scsi_port_show_attr_role(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_device *dev = lun->lun_se_dev;
	struct se_port *sep;
	ssize_t ret;

	if (!dev)
		return -ENODEV;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	ret = snprintf(page, PAGE_SIZE, "%s%u\n", "Device", dev->dev_index);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_PORT_ATTR_RO(role);

static ssize_t target_stat_scsi_port_show_attr_busy_count(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	/* FIXME: scsiPortBusyStatuses  */
	ret = snprintf(page, PAGE_SIZE, "%u\n", 0);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_PORT_ATTR_RO(busy_count);

CONFIGFS_EATTR_OPS(target_stat_scsi_port, se_port_stat_grps, scsi_port_group);

static struct configfs_attribute *target_stat_scsi_port_attrs[] = {
	&target_stat_scsi_port_inst.attr,
	&target_stat_scsi_port_dev.attr,
	&target_stat_scsi_port_indx.attr,
	&target_stat_scsi_port_role.attr,
	&target_stat_scsi_port_busy_count.attr,
	NULL,
};

static struct configfs_item_operations target_stat_scsi_port_attrib_ops = {
	.show_attribute		= target_stat_scsi_port_attr_show,
	.store_attribute	= target_stat_scsi_port_attr_store,
};

static struct config_item_type target_stat_scsi_port_cit = {
	.ct_item_ops		= &target_stat_scsi_port_attrib_ops,
=======
static struct se_lun *to_stat_port(struct config_item *item)
{
	struct se_port_stat_grps *pgrps = container_of(to_config_group(item),
			struct se_port_stat_grps, scsi_port_group);
	return container_of(pgrps, struct se_lun, port_stat_grps);
}

static ssize_t target_stat_port_inst_show(struct config_item *item, char *page)
{
	struct se_lun *lun = to_stat_port(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%u\n", dev->hba_index);
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_port_dev_show(struct config_item *item, char *page)
{
	struct se_lun *lun = to_stat_port(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%u\n", dev->dev_index);
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_port_indx_show(struct config_item *item, char *page)
{
	struct se_lun *lun = to_stat_port(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%u\n", lun->lun_rtpi);
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_port_role_show(struct config_item *item, char *page)
{
	struct se_lun *lun = to_stat_port(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%s%u\n", "Device", dev->dev_index);
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_port_busy_count_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_stat_port(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev) {
		/* FIXME: scsiPortBusyStatuses  */
		ret = snprintf(page, PAGE_SIZE, "%u\n", 0);
	}
	rcu_read_unlock();
	return ret;
}

CONFIGFS_ATTR_RO(target_stat_port_, inst);
CONFIGFS_ATTR_RO(target_stat_port_, dev);
CONFIGFS_ATTR_RO(target_stat_port_, indx);
CONFIGFS_ATTR_RO(target_stat_port_, role);
CONFIGFS_ATTR_RO(target_stat_port_, busy_count);

static struct configfs_attribute *target_stat_scsi_port_attrs[] = {
	&target_stat_port_attr_inst,
	&target_stat_port_attr_dev,
	&target_stat_port_attr_indx,
	&target_stat_port_attr_role,
	&target_stat_port_attr_busy_count,
	NULL,
};

static struct config_item_type target_stat_scsi_port_cit = {
>>>>>>> v4.9.227
	.ct_attrs		= target_stat_scsi_port_attrs,
	.ct_owner		= THIS_MODULE,
};

/*
 * SCSI Target Port Table
 */
<<<<<<< HEAD
CONFIGFS_EATTR_STRUCT(target_stat_scsi_tgt_port, se_port_stat_grps);
#define DEV_STAT_SCSI_TGT_PORT_ATTR(_name, _mode)			\
static struct target_stat_scsi_tgt_port_attribute			\
			target_stat_scsi_tgt_port_##_name =		\
	__CONFIGFS_EATTR(_name, _mode,					\
	target_stat_scsi_tgt_port_show_attr_##_name,			\
	target_stat_scsi_tgt_port_store_attr_##_name);

#define DEV_STAT_SCSI_TGT_PORT_ATTR_RO(_name)				\
static struct target_stat_scsi_tgt_port_attribute			\
			target_stat_scsi_tgt_port_##_name =		\
	__CONFIGFS_EATTR_RO(_name,					\
	target_stat_scsi_tgt_port_show_attr_##_name);

static ssize_t target_stat_scsi_tgt_port_show_attr_inst(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_device *dev = lun->lun_se_dev;
	struct se_port *sep;
	struct se_hba *hba;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	hba = dev->se_hba;
	ret = snprintf(page, PAGE_SIZE, "%u\n", hba->hba_index);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TGT_PORT_ATTR_RO(inst);

static ssize_t target_stat_scsi_tgt_port_show_attr_dev(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_device *dev = lun->lun_se_dev;
	struct se_port *sep;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	ret = snprintf(page, PAGE_SIZE, "%u\n", dev->dev_index);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TGT_PORT_ATTR_RO(dev);

static ssize_t target_stat_scsi_tgt_port_show_attr_indx(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	ret = snprintf(page, PAGE_SIZE, "%u\n", sep->sep_index);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TGT_PORT_ATTR_RO(indx);

static ssize_t target_stat_scsi_tgt_port_show_attr_name(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	struct se_portal_group *tpg;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	tpg = sep->sep_tpg;

	ret = snprintf(page, PAGE_SIZE, "%sPort#%u\n",
		tpg->se_tpg_tfo->get_fabric_name(), sep->sep_index);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TGT_PORT_ATTR_RO(name);

static ssize_t target_stat_scsi_tgt_port_show_attr_port_index(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	struct se_portal_group *tpg;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	tpg = sep->sep_tpg;

	ret = snprintf(page, PAGE_SIZE, "%s%s%d\n",
		tpg->se_tpg_tfo->tpg_get_wwn(tpg), "+t+",
		tpg->se_tpg_tfo->tpg_get_tag(tpg));
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TGT_PORT_ATTR_RO(port_index);

static ssize_t target_stat_scsi_tgt_port_show_attr_in_cmds(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}

	ret = snprintf(page, PAGE_SIZE, "%llu\n", sep->sep_stats.cmd_pdus);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TGT_PORT_ATTR_RO(in_cmds);

static ssize_t target_stat_scsi_tgt_port_show_attr_write_mbytes(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}

	ret = snprintf(page, PAGE_SIZE, "%u\n",
			(u32)(sep->sep_stats.rx_data_octets >> 20));
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TGT_PORT_ATTR_RO(write_mbytes);

static ssize_t target_stat_scsi_tgt_port_show_attr_read_mbytes(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}

	ret = snprintf(page, PAGE_SIZE, "%u\n",
			(u32)(sep->sep_stats.tx_data_octets >> 20));
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TGT_PORT_ATTR_RO(read_mbytes);

static ssize_t target_stat_scsi_tgt_port_show_attr_hs_in_cmds(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}

	/* FIXME: scsiTgtPortHsInCommands */
	ret = snprintf(page, PAGE_SIZE, "%u\n", 0);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TGT_PORT_ATTR_RO(hs_in_cmds);

CONFIGFS_EATTR_OPS(target_stat_scsi_tgt_port, se_port_stat_grps,
		scsi_tgt_port_group);

static struct configfs_attribute *target_stat_scsi_tgt_port_attrs[] = {
	&target_stat_scsi_tgt_port_inst.attr,
	&target_stat_scsi_tgt_port_dev.attr,
	&target_stat_scsi_tgt_port_indx.attr,
	&target_stat_scsi_tgt_port_name.attr,
	&target_stat_scsi_tgt_port_port_index.attr,
	&target_stat_scsi_tgt_port_in_cmds.attr,
	&target_stat_scsi_tgt_port_write_mbytes.attr,
	&target_stat_scsi_tgt_port_read_mbytes.attr,
	&target_stat_scsi_tgt_port_hs_in_cmds.attr,
	NULL,
};

static struct configfs_item_operations target_stat_scsi_tgt_port_attrib_ops = {
	.show_attribute		= target_stat_scsi_tgt_port_attr_show,
	.store_attribute	= target_stat_scsi_tgt_port_attr_store,
};

static struct config_item_type target_stat_scsi_tgt_port_cit = {
	.ct_item_ops		= &target_stat_scsi_tgt_port_attrib_ops,
=======
static struct se_lun *to_stat_tgt_port(struct config_item *item)
{
	struct se_port_stat_grps *pgrps = container_of(to_config_group(item),
			struct se_port_stat_grps, scsi_tgt_port_group);
	return container_of(pgrps, struct se_lun, port_stat_grps);
}

static ssize_t target_stat_tgt_port_inst_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_stat_tgt_port(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%u\n", dev->hba_index);
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_tgt_port_dev_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_stat_tgt_port(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%u\n", dev->dev_index);
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_tgt_port_indx_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_stat_tgt_port(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%u\n", lun->lun_rtpi);
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_tgt_port_name_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_stat_tgt_port(item);
	struct se_portal_group *tpg = lun->lun_tpg;
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%sPort#%u\n",
			tpg->se_tpg_tfo->get_fabric_name(),
			lun->lun_rtpi);
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_tgt_port_port_index_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_stat_tgt_port(item);
	struct se_portal_group *tpg = lun->lun_tpg;
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%s%s%d\n",
			tpg->se_tpg_tfo->tpg_get_wwn(tpg), "+t+",
			tpg->se_tpg_tfo->tpg_get_tag(tpg));
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_tgt_port_in_cmds_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_stat_tgt_port(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%lu\n",
			       atomic_long_read(&lun->lun_stats.cmd_pdus));
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_tgt_port_write_mbytes_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_stat_tgt_port(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%u\n",
			(u32)(atomic_long_read(&lun->lun_stats.rx_data_octets) >> 20));
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_tgt_port_read_mbytes_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_stat_tgt_port(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%u\n",
				(u32)(atomic_long_read(&lun->lun_stats.tx_data_octets) >> 20));
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_tgt_port_hs_in_cmds_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_stat_tgt_port(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev) {
		/* FIXME: scsiTgtPortHsInCommands */
		ret = snprintf(page, PAGE_SIZE, "%u\n", 0);
	}
	rcu_read_unlock();
	return ret;
}

CONFIGFS_ATTR_RO(target_stat_tgt_port_, inst);
CONFIGFS_ATTR_RO(target_stat_tgt_port_, dev);
CONFIGFS_ATTR_RO(target_stat_tgt_port_, indx);
CONFIGFS_ATTR_RO(target_stat_tgt_port_, name);
CONFIGFS_ATTR_RO(target_stat_tgt_port_, port_index);
CONFIGFS_ATTR_RO(target_stat_tgt_port_, in_cmds);
CONFIGFS_ATTR_RO(target_stat_tgt_port_, write_mbytes);
CONFIGFS_ATTR_RO(target_stat_tgt_port_, read_mbytes);
CONFIGFS_ATTR_RO(target_stat_tgt_port_, hs_in_cmds);

static struct configfs_attribute *target_stat_scsi_tgt_port_attrs[] = {
	&target_stat_tgt_port_attr_inst,
	&target_stat_tgt_port_attr_dev,
	&target_stat_tgt_port_attr_indx,
	&target_stat_tgt_port_attr_name,
	&target_stat_tgt_port_attr_port_index,
	&target_stat_tgt_port_attr_in_cmds,
	&target_stat_tgt_port_attr_write_mbytes,
	&target_stat_tgt_port_attr_read_mbytes,
	&target_stat_tgt_port_attr_hs_in_cmds,
	NULL,
};

static struct config_item_type target_stat_scsi_tgt_port_cit = {
>>>>>>> v4.9.227
	.ct_attrs		= target_stat_scsi_tgt_port_attrs,
	.ct_owner		= THIS_MODULE,
};

/*
 * SCSI Transport Table
<<<<<<< HEAD
o */

CONFIGFS_EATTR_STRUCT(target_stat_scsi_transport, se_port_stat_grps);
#define DEV_STAT_SCSI_TRANSPORT_ATTR(_name, _mode)			\
static struct target_stat_scsi_transport_attribute			\
			target_stat_scsi_transport_##_name =		\
	__CONFIGFS_EATTR(_name, _mode,					\
	target_stat_scsi_transport_show_attr_##_name,			\
	target_stat_scsi_transport_store_attr_##_name);

#define DEV_STAT_SCSI_TRANSPORT_ATTR_RO(_name)				\
static struct target_stat_scsi_transport_attribute			\
			target_stat_scsi_transport_##_name =		\
	__CONFIGFS_EATTR_RO(_name,					\
	target_stat_scsi_transport_show_attr_##_name);

static ssize_t target_stat_scsi_transport_show_attr_inst(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_device *dev = lun->lun_se_dev;
	struct se_port *sep;
	struct se_hba *hba;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}

	hba = dev->se_hba;
	ret = snprintf(page, PAGE_SIZE, "%u\n", hba->hba_index);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TRANSPORT_ATTR_RO(inst);

static ssize_t target_stat_scsi_transport_show_attr_device(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	struct se_portal_group *tpg;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	tpg = sep->sep_tpg;
	/* scsiTransportType */
	ret = snprintf(page, PAGE_SIZE, "scsiTransport%s\n",
			tpg->se_tpg_tfo->get_fabric_name());
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TRANSPORT_ATTR_RO(device);

static ssize_t target_stat_scsi_transport_show_attr_indx(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_port *sep;
	struct se_portal_group *tpg;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	tpg = sep->sep_tpg;
	ret = snprintf(page, PAGE_SIZE, "%u\n",
			tpg->se_tpg_tfo->tpg_get_inst_index(tpg));
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TRANSPORT_ATTR_RO(indx);

static ssize_t target_stat_scsi_transport_show_attr_dev_name(
	struct se_port_stat_grps *pgrps, char *page)
{
	struct se_lun *lun = container_of(pgrps, struct se_lun, port_stat_grps);
	struct se_device *dev = lun->lun_se_dev;
	struct se_port *sep;
	struct se_portal_group *tpg;
	struct t10_wwn *wwn;
	ssize_t ret;

	spin_lock(&lun->lun_sep_lock);
	sep = lun->lun_sep;
	if (!sep) {
		spin_unlock(&lun->lun_sep_lock);
		return -ENODEV;
	}
	tpg = sep->sep_tpg;
	wwn = &dev->t10_wwn;
	/* scsiTransportDevName */
	ret = snprintf(page, PAGE_SIZE, "%s+%s\n",
			tpg->se_tpg_tfo->tpg_get_wwn(tpg),
			(strlen(wwn->unit_serial)) ? wwn->unit_serial :
			wwn->vendor);
	spin_unlock(&lun->lun_sep_lock);
	return ret;
}
DEV_STAT_SCSI_TRANSPORT_ATTR_RO(dev_name);

CONFIGFS_EATTR_OPS(target_stat_scsi_transport, se_port_stat_grps,
		scsi_transport_group);

static struct configfs_attribute *target_stat_scsi_transport_attrs[] = {
	&target_stat_scsi_transport_inst.attr,
	&target_stat_scsi_transport_device.attr,
	&target_stat_scsi_transport_indx.attr,
	&target_stat_scsi_transport_dev_name.attr,
	NULL,
};

static struct configfs_item_operations target_stat_scsi_transport_attrib_ops = {
	.show_attribute		= target_stat_scsi_transport_attr_show,
	.store_attribute	= target_stat_scsi_transport_attr_store,
};

static struct config_item_type target_stat_scsi_transport_cit = {
	.ct_item_ops		= &target_stat_scsi_transport_attrib_ops,
=======
 */
static struct se_lun *to_transport_stat(struct config_item *item)
{
	struct se_port_stat_grps *pgrps = container_of(to_config_group(item),
			struct se_port_stat_grps, scsi_transport_group);
	return container_of(pgrps, struct se_lun, port_stat_grps);
}

static ssize_t target_stat_transport_inst_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_transport_stat(item);
	struct se_device *dev;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%u\n", dev->hba_index);
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_transport_device_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_transport_stat(item);
	struct se_device *dev;
	struct se_portal_group *tpg = lun->lun_tpg;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev) {
		/* scsiTransportType */
		ret = snprintf(page, PAGE_SIZE, "scsiTransport%s\n",
			       tpg->se_tpg_tfo->get_fabric_name());
	}
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_transport_indx_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_transport_stat(item);
	struct se_device *dev;
	struct se_portal_group *tpg = lun->lun_tpg;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev)
		ret = snprintf(page, PAGE_SIZE, "%u\n",
			       tpg->se_tpg_tfo->tpg_get_inst_index(tpg));
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_transport_dev_name_show(struct config_item *item,
		char *page)
{
	struct se_lun *lun = to_transport_stat(item);
	struct se_device *dev;
	struct se_portal_group *tpg = lun->lun_tpg;
	struct t10_wwn *wwn;
	ssize_t ret = -ENODEV;

	rcu_read_lock();
	dev = rcu_dereference(lun->lun_se_dev);
	if (dev) {
		wwn = &dev->t10_wwn;
		/* scsiTransportDevName */
		ret = snprintf(page, PAGE_SIZE, "%s+%s\n",
				tpg->se_tpg_tfo->tpg_get_wwn(tpg),
				(strlen(wwn->unit_serial)) ? wwn->unit_serial :
				wwn->vendor);
	}
	rcu_read_unlock();
	return ret;
}

CONFIGFS_ATTR_RO(target_stat_transport_, inst);
CONFIGFS_ATTR_RO(target_stat_transport_, device);
CONFIGFS_ATTR_RO(target_stat_transport_, indx);
CONFIGFS_ATTR_RO(target_stat_transport_, dev_name);

static struct configfs_attribute *target_stat_scsi_transport_attrs[] = {
	&target_stat_transport_attr_inst,
	&target_stat_transport_attr_device,
	&target_stat_transport_attr_indx,
	&target_stat_transport_attr_dev_name,
	NULL,
};

static struct config_item_type target_stat_scsi_transport_cit = {
>>>>>>> v4.9.227
	.ct_attrs		= target_stat_scsi_transport_attrs,
	.ct_owner		= THIS_MODULE,
};

/*
 * Called from target_core_fabric_configfs.c:target_fabric_make_lun() to setup
 * the target port statistics groups + configfs CITs located in target_core_stat.c
 */
void target_stat_setup_port_default_groups(struct se_lun *lun)
{
<<<<<<< HEAD
	struct config_group *port_stat_grp = &lun->port_stat_grps.stat_group;

	config_group_init_type_name(&lun->port_stat_grps.scsi_port_group,
			"scsi_port", &target_stat_scsi_port_cit);
	config_group_init_type_name(&lun->port_stat_grps.scsi_tgt_port_group,
			"scsi_tgt_port", &target_stat_scsi_tgt_port_cit);
	config_group_init_type_name(&lun->port_stat_grps.scsi_transport_group,
			"scsi_transport", &target_stat_scsi_transport_cit);

	port_stat_grp->default_groups[0] = &lun->port_stat_grps.scsi_port_group;
	port_stat_grp->default_groups[1] = &lun->port_stat_grps.scsi_tgt_port_group;
	port_stat_grp->default_groups[2] = &lun->port_stat_grps.scsi_transport_group;
	port_stat_grp->default_groups[3] = NULL;
=======
	config_group_init_type_name(&lun->port_stat_grps.scsi_port_group,
			"scsi_port", &target_stat_scsi_port_cit);
	configfs_add_default_group(&lun->port_stat_grps.scsi_port_group,
			&lun->port_stat_grps.stat_group);

	config_group_init_type_name(&lun->port_stat_grps.scsi_tgt_port_group,
			"scsi_tgt_port", &target_stat_scsi_tgt_port_cit);
	configfs_add_default_group(&lun->port_stat_grps.scsi_tgt_port_group,
			&lun->port_stat_grps.stat_group);

	config_group_init_type_name(&lun->port_stat_grps.scsi_transport_group,
			"scsi_transport", &target_stat_scsi_transport_cit);
	configfs_add_default_group(&lun->port_stat_grps.scsi_transport_group,
			&lun->port_stat_grps.stat_group);
>>>>>>> v4.9.227
}

/*
 * SCSI Authorized Initiator Table
 */

<<<<<<< HEAD
CONFIGFS_EATTR_STRUCT(target_stat_scsi_auth_intr, se_ml_stat_grps);
#define DEV_STAT_SCSI_AUTH_INTR_ATTR(_name, _mode)			\
static struct target_stat_scsi_auth_intr_attribute			\
			target_stat_scsi_auth_intr_##_name =		\
	__CONFIGFS_EATTR(_name, _mode,					\
	target_stat_scsi_auth_intr_show_attr_##_name,			\
	target_stat_scsi_auth_intr_store_attr_##_name);

#define DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(_name)				\
static struct target_stat_scsi_auth_intr_attribute			\
			target_stat_scsi_auth_intr_##_name =		\
	__CONFIGFS_EATTR_RO(_name,					\
	target_stat_scsi_auth_intr_show_attr_##_name);

static ssize_t target_stat_scsi_auth_intr_show_attr_inst(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
static struct se_lun_acl *auth_to_lacl(struct config_item *item)
{
	struct se_ml_stat_grps *lgrps = container_of(to_config_group(item),
			struct se_ml_stat_grps, scsi_auth_intr_group);
	return container_of(lgrps, struct se_lun_acl, ml_stat_grps);
}

static ssize_t target_stat_auth_inst_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	struct se_portal_group *tpg;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	tpg = nacl->se_tpg;
	/* scsiInstIndex */
	ret = snprintf(page, PAGE_SIZE, "%u\n",
			tpg->se_tpg_tfo->tpg_get_inst_index(tpg));
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(inst);

static ssize_t target_stat_scsi_auth_intr_show_attr_dev(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_dev_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	struct se_lun *lun;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
		return -ENODEV;
	}
	lun = deve->se_lun;
	/* scsiDeviceIndex */
	ret = snprintf(page, PAGE_SIZE, "%u\n", lun->lun_se_dev->dev_index);
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(dev);

static ssize_t target_stat_scsi_auth_intr_show_attr_port(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
		return -ENODEV;
	}
	lun = rcu_dereference(deve->se_lun);
	/* scsiDeviceIndex */
	ret = snprintf(page, PAGE_SIZE, "%u\n", lun->lun_index);
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_port_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	struct se_portal_group *tpg;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	tpg = nacl->se_tpg;
	/* scsiAuthIntrTgtPortIndex */
	ret = snprintf(page, PAGE_SIZE, "%u\n", tpg->se_tpg_tfo->tpg_get_tag(tpg));
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(port);

static ssize_t target_stat_scsi_auth_intr_show_attr_indx(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_indx_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	/* scsiAuthIntrIndex */
	ret = snprintf(page, PAGE_SIZE, "%u\n", nacl->acl_index);
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(indx);

static ssize_t target_stat_scsi_auth_intr_show_attr_dev_or_port(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_dev_or_port_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	/* scsiAuthIntrDevOrPort */
	ret = snprintf(page, PAGE_SIZE, "%u\n", 1);
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(dev_or_port);

static ssize_t target_stat_scsi_auth_intr_show_attr_intr_name(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_intr_name_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	/* scsiAuthIntrName */
	ret = snprintf(page, PAGE_SIZE, "%s\n", nacl->initiatorname);
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(intr_name);

static ssize_t target_stat_scsi_auth_intr_show_attr_map_indx(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_map_indx_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	/* FIXME: scsiAuthIntrLunMapIndex */
	ret = snprintf(page, PAGE_SIZE, "%u\n", 0);
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(map_indx);

static ssize_t target_stat_scsi_auth_intr_show_attr_att_count(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_att_count_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	/* scsiAuthIntrAttachedTimes */
	ret = snprintf(page, PAGE_SIZE, "%u\n", deve->attach_count);
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(att_count);

static ssize_t target_stat_scsi_auth_intr_show_attr_num_cmds(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_num_cmds_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
		return -ENODEV;
	}
	/* scsiAuthIntrOutCommands */
	ret = snprintf(page, PAGE_SIZE, "%u\n", deve->total_cmds);
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(num_cmds);

static ssize_t target_stat_scsi_auth_intr_show_attr_read_mbytes(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
		return -ENODEV;
	}
	/* scsiAuthIntrOutCommands */
	ret = snprintf(page, PAGE_SIZE, "%lu\n",
		       atomic_long_read(&deve->total_cmds));
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_read_mbytes_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
		return -ENODEV;
	}
	/* scsiAuthIntrReadMegaBytes */
	ret = snprintf(page, PAGE_SIZE, "%u\n", (u32)(deve->read_bytes >> 20));
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(read_mbytes);

static ssize_t target_stat_scsi_auth_intr_show_attr_write_mbytes(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
		return -ENODEV;
	}
	/* scsiAuthIntrReadMegaBytes */
	ret = snprintf(page, PAGE_SIZE, "%u\n",
		      (u32)(atomic_long_read(&deve->read_bytes) >> 20));
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_write_mbytes_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
		return -ENODEV;
	}
	/* scsiAuthIntrWrittenMegaBytes */
	ret = snprintf(page, PAGE_SIZE, "%u\n", (u32)(deve->write_bytes >> 20));
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(write_mbytes);

static ssize_t target_stat_scsi_auth_intr_show_attr_hs_num_cmds(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
		return -ENODEV;
	}
	/* scsiAuthIntrWrittenMegaBytes */
	ret = snprintf(page, PAGE_SIZE, "%u\n",
		      (u32)(atomic_long_read(&deve->write_bytes) >> 20));
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_hs_num_cmds_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	/* FIXME: scsiAuthIntrHSOutCommands */
	ret = snprintf(page, PAGE_SIZE, "%u\n", 0);
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(hs_num_cmds);

static ssize_t target_stat_scsi_auth_intr_show_attr_creation_time(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_creation_time_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	/* scsiAuthIntrLastCreation */
	ret = snprintf(page, PAGE_SIZE, "%u\n", (u32)(((u32)deve->creation_time -
				INITIAL_JIFFIES) * 100 / HZ));
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(creation_time);

static ssize_t target_stat_scsi_auth_intr_show_attr_row_status(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_auth_row_status_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = auth_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	/* FIXME: scsiAuthIntrRowStatus */
	ret = snprintf(page, PAGE_SIZE, "Ready\n");
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_AUTH_INTR_ATTR_RO(row_status);

CONFIGFS_EATTR_OPS(target_stat_scsi_auth_intr, se_ml_stat_grps,
		scsi_auth_intr_group);

static struct configfs_attribute *target_stat_scsi_auth_intr_attrs[] = {
	&target_stat_scsi_auth_intr_inst.attr,
	&target_stat_scsi_auth_intr_dev.attr,
	&target_stat_scsi_auth_intr_port.attr,
	&target_stat_scsi_auth_intr_indx.attr,
	&target_stat_scsi_auth_intr_dev_or_port.attr,
	&target_stat_scsi_auth_intr_intr_name.attr,
	&target_stat_scsi_auth_intr_map_indx.attr,
	&target_stat_scsi_auth_intr_att_count.attr,
	&target_stat_scsi_auth_intr_num_cmds.attr,
	&target_stat_scsi_auth_intr_read_mbytes.attr,
	&target_stat_scsi_auth_intr_write_mbytes.attr,
	&target_stat_scsi_auth_intr_hs_num_cmds.attr,
	&target_stat_scsi_auth_intr_creation_time.attr,
	&target_stat_scsi_auth_intr_row_status.attr,
	NULL,
};

static struct configfs_item_operations target_stat_scsi_auth_intr_attrib_ops = {
	.show_attribute		= target_stat_scsi_auth_intr_attr_show,
	.store_attribute	= target_stat_scsi_auth_intr_attr_store,
};

static struct config_item_type target_stat_scsi_auth_intr_cit = {
	.ct_item_ops		= &target_stat_scsi_auth_intr_attrib_ops,
=======
	rcu_read_unlock();
	return ret;
}

CONFIGFS_ATTR_RO(target_stat_auth_, inst);
CONFIGFS_ATTR_RO(target_stat_auth_, dev);
CONFIGFS_ATTR_RO(target_stat_auth_, port);
CONFIGFS_ATTR_RO(target_stat_auth_, indx);
CONFIGFS_ATTR_RO(target_stat_auth_, dev_or_port);
CONFIGFS_ATTR_RO(target_stat_auth_, intr_name);
CONFIGFS_ATTR_RO(target_stat_auth_, map_indx);
CONFIGFS_ATTR_RO(target_stat_auth_, att_count);
CONFIGFS_ATTR_RO(target_stat_auth_, num_cmds);
CONFIGFS_ATTR_RO(target_stat_auth_, read_mbytes);
CONFIGFS_ATTR_RO(target_stat_auth_, write_mbytes);
CONFIGFS_ATTR_RO(target_stat_auth_, hs_num_cmds);
CONFIGFS_ATTR_RO(target_stat_auth_, creation_time);
CONFIGFS_ATTR_RO(target_stat_auth_, row_status);

static struct configfs_attribute *target_stat_scsi_auth_intr_attrs[] = {
	&target_stat_auth_attr_inst,
	&target_stat_auth_attr_dev,
	&target_stat_auth_attr_port,
	&target_stat_auth_attr_indx,
	&target_stat_auth_attr_dev_or_port,
	&target_stat_auth_attr_intr_name,
	&target_stat_auth_attr_map_indx,
	&target_stat_auth_attr_att_count,
	&target_stat_auth_attr_num_cmds,
	&target_stat_auth_attr_read_mbytes,
	&target_stat_auth_attr_write_mbytes,
	&target_stat_auth_attr_hs_num_cmds,
	&target_stat_auth_attr_creation_time,
	&target_stat_auth_attr_row_status,
	NULL,
};

static struct config_item_type target_stat_scsi_auth_intr_cit = {
>>>>>>> v4.9.227
	.ct_attrs		= target_stat_scsi_auth_intr_attrs,
	.ct_owner		= THIS_MODULE,
};

/*
 * SCSI Attached Initiator Port Table
 */

<<<<<<< HEAD
CONFIGFS_EATTR_STRUCT(target_stat_scsi_att_intr_port, se_ml_stat_grps);
#define DEV_STAT_SCSI_ATTR_INTR_PORT_ATTR(_name, _mode)			\
static struct target_stat_scsi_att_intr_port_attribute			\
		target_stat_scsi_att_intr_port_##_name =		\
	__CONFIGFS_EATTR(_name, _mode,					\
	target_stat_scsi_att_intr_port_show_attr_##_name,		\
	target_stat_scsi_att_intr_port_store_attr_##_name);

#define DEV_STAT_SCSI_ATTR_INTR_PORT_ATTR_RO(_name)			\
static struct target_stat_scsi_att_intr_port_attribute			\
		target_stat_scsi_att_intr_port_##_name =		\
	__CONFIGFS_EATTR_RO(_name,					\
	target_stat_scsi_att_intr_port_show_attr_##_name);

static ssize_t target_stat_scsi_att_intr_port_show_attr_inst(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
static struct se_lun_acl *iport_to_lacl(struct config_item *item)
{
	struct se_ml_stat_grps *lgrps = container_of(to_config_group(item),
			struct se_ml_stat_grps, scsi_att_intr_port_group);
	return container_of(lgrps, struct se_lun_acl, ml_stat_grps);
}

static ssize_t target_stat_iport_inst_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = iport_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	struct se_portal_group *tpg;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	tpg = nacl->se_tpg;
	/* scsiInstIndex */
	ret = snprintf(page, PAGE_SIZE, "%u\n",
			tpg->se_tpg_tfo->tpg_get_inst_index(tpg));
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_ATTR_INTR_PORT_ATTR_RO(inst);

static ssize_t target_stat_scsi_att_intr_port_show_attr_dev(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_iport_dev_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = iport_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	struct se_lun *lun;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
		return -ENODEV;
	}
	lun = deve->se_lun;
	/* scsiDeviceIndex */
	ret = snprintf(page, PAGE_SIZE, "%u\n", lun->lun_se_dev->dev_index);
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_ATTR_INTR_PORT_ATTR_RO(dev);

static ssize_t target_stat_scsi_att_intr_port_show_attr_port(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
		return -ENODEV;
	}
	lun = rcu_dereference(deve->se_lun);
	/* scsiDeviceIndex */
	ret = snprintf(page, PAGE_SIZE, "%u\n", lun->lun_index);
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_iport_port_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = iport_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	struct se_portal_group *tpg;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	tpg = nacl->se_tpg;
	/* scsiPortIndex */
	ret = snprintf(page, PAGE_SIZE, "%u\n", tpg->se_tpg_tfo->tpg_get_tag(tpg));
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_ATTR_INTR_PORT_ATTR_RO(port);

static ssize_t target_stat_scsi_att_intr_port_show_attr_indx(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_iport_indx_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = iport_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_session *se_sess;
	struct se_portal_group *tpg;
	ssize_t ret;

	spin_lock_irq(&nacl->nacl_sess_lock);
	se_sess = nacl->nacl_sess;
	if (!se_sess) {
		spin_unlock_irq(&nacl->nacl_sess_lock);
		return -ENODEV;
	}

	tpg = nacl->se_tpg;
	/* scsiAttIntrPortIndex */
	ret = snprintf(page, PAGE_SIZE, "%u\n",
			tpg->se_tpg_tfo->sess_get_index(se_sess));
	spin_unlock_irq(&nacl->nacl_sess_lock);
	return ret;
}
<<<<<<< HEAD
DEV_STAT_SCSI_ATTR_INTR_PORT_ATTR_RO(indx);

static ssize_t target_stat_scsi_att_intr_port_show_attr_port_auth_indx(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======

static ssize_t target_stat_iport_port_auth_indx_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = iport_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_dev_entry *deve;
	ssize_t ret;

<<<<<<< HEAD
	spin_lock_irq(&nacl->device_list_lock);
	deve = nacl->device_list[lacl->mapped_lun];
	if (!deve->se_lun || !deve->se_lun_acl) {
		spin_unlock_irq(&nacl->device_list_lock);
=======
	rcu_read_lock();
	deve = target_nacl_find_deve(nacl, lacl->mapped_lun);
	if (!deve) {
		rcu_read_unlock();
>>>>>>> v4.9.227
		return -ENODEV;
	}
	/* scsiAttIntrPortAuthIntrIdx */
	ret = snprintf(page, PAGE_SIZE, "%u\n", nacl->acl_index);
<<<<<<< HEAD
	spin_unlock_irq(&nacl->device_list_lock);
	return ret;
}
DEV_STAT_SCSI_ATTR_INTR_PORT_ATTR_RO(port_auth_indx);

static ssize_t target_stat_scsi_att_intr_port_show_attr_port_ident(
	struct se_ml_stat_grps *lgrps, char *page)
{
	struct se_lun_acl *lacl = container_of(lgrps,
			struct se_lun_acl, ml_stat_grps);
=======
	rcu_read_unlock();
	return ret;
}

static ssize_t target_stat_iport_port_ident_show(struct config_item *item,
		char *page)
{
	struct se_lun_acl *lacl = iport_to_lacl(item);
>>>>>>> v4.9.227
	struct se_node_acl *nacl = lacl->se_lun_nacl;
	struct se_session *se_sess;
	struct se_portal_group *tpg;
	ssize_t ret;
	unsigned char buf[64];

	spin_lock_irq(&nacl->nacl_sess_lock);
	se_sess = nacl->nacl_sess;
	if (!se_sess) {
		spin_unlock_irq(&nacl->nacl_sess_lock);
		return -ENODEV;
	}

	tpg = nacl->se_tpg;
	/* scsiAttIntrPortName+scsiAttIntrPortIdentifier */
	memset(buf, 0, 64);
	if (tpg->se_tpg_tfo->sess_get_initiator_sid != NULL)
		tpg->se_tpg_tfo->sess_get_initiator_sid(se_sess, buf, 64);

	ret = snprintf(page, PAGE_SIZE, "%s+i+%s\n", nacl->initiatorname, buf);
	spin_unlock_irq(&nacl->nacl_sess_lock);
	return ret;
}
<<<<<<< HEAD
DEV_STAT_SCSI_ATTR_INTR_PORT_ATTR_RO(port_ident);

CONFIGFS_EATTR_OPS(target_stat_scsi_att_intr_port, se_ml_stat_grps,
		scsi_att_intr_port_group);

static struct configfs_attribute *target_stat_scsi_ath_intr_port_attrs[] = {
	&target_stat_scsi_att_intr_port_inst.attr,
	&target_stat_scsi_att_intr_port_dev.attr,
	&target_stat_scsi_att_intr_port_port.attr,
	&target_stat_scsi_att_intr_port_indx.attr,
	&target_stat_scsi_att_intr_port_port_auth_indx.attr,
	&target_stat_scsi_att_intr_port_port_ident.attr,
	NULL,
};

static struct configfs_item_operations target_stat_scsi_att_intr_port_attrib_ops = {
	.show_attribute		= target_stat_scsi_att_intr_port_attr_show,
	.store_attribute	= target_stat_scsi_att_intr_port_attr_store,
};

static struct config_item_type target_stat_scsi_att_intr_port_cit = {
	.ct_item_ops		= &target_stat_scsi_att_intr_port_attrib_ops,
=======

CONFIGFS_ATTR_RO(target_stat_iport_, inst);
CONFIGFS_ATTR_RO(target_stat_iport_, dev);
CONFIGFS_ATTR_RO(target_stat_iport_, port);
CONFIGFS_ATTR_RO(target_stat_iport_, indx);
CONFIGFS_ATTR_RO(target_stat_iport_, port_auth_indx);
CONFIGFS_ATTR_RO(target_stat_iport_, port_ident);

static struct configfs_attribute *target_stat_scsi_ath_intr_port_attrs[] = {
	&target_stat_iport_attr_inst,
	&target_stat_iport_attr_dev,
	&target_stat_iport_attr_port,
	&target_stat_iport_attr_indx,
	&target_stat_iport_attr_port_auth_indx,
	&target_stat_iport_attr_port_ident,
	NULL,
};

static struct config_item_type target_stat_scsi_att_intr_port_cit = {
>>>>>>> v4.9.227
	.ct_attrs		= target_stat_scsi_ath_intr_port_attrs,
	.ct_owner		= THIS_MODULE,
};

/*
 * Called from target_core_fabric_configfs.c:target_fabric_make_mappedlun() to setup
 * the target MappedLUN statistics groups + configfs CITs located in target_core_stat.c
 */
void target_stat_setup_mappedlun_default_groups(struct se_lun_acl *lacl)
{
<<<<<<< HEAD
	struct config_group *ml_stat_grp = &lacl->ml_stat_grps.stat_group;

	config_group_init_type_name(&lacl->ml_stat_grps.scsi_auth_intr_group,
			"scsi_auth_intr", &target_stat_scsi_auth_intr_cit);
	config_group_init_type_name(&lacl->ml_stat_grps.scsi_att_intr_port_group,
			"scsi_att_intr_port", &target_stat_scsi_att_intr_port_cit);

	ml_stat_grp->default_groups[0] = &lacl->ml_stat_grps.scsi_auth_intr_group;
	ml_stat_grp->default_groups[1] = &lacl->ml_stat_grps.scsi_att_intr_port_group;
	ml_stat_grp->default_groups[2] = NULL;
=======
	config_group_init_type_name(&lacl->ml_stat_grps.scsi_auth_intr_group,
			"scsi_auth_intr", &target_stat_scsi_auth_intr_cit);
	configfs_add_default_group(&lacl->ml_stat_grps.scsi_auth_intr_group,
			&lacl->ml_stat_grps.stat_group);

	config_group_init_type_name(&lacl->ml_stat_grps.scsi_att_intr_port_group,
			"scsi_att_intr_port", &target_stat_scsi_att_intr_port_cit);
	configfs_add_default_group(&lacl->ml_stat_grps.scsi_att_intr_port_group,
			&lacl->ml_stat_grps.stat_group);
>>>>>>> v4.9.227
}
