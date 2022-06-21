/*
 * omap iommu: debugfs interface
 *
 * Copyright (C) 2008-2009 Nokia Corporation
 *
 * Written by Hiroshi DOYU <Hiroshi.DOYU@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

<<<<<<< HEAD
#include <linux/module.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include <linux/omap-iommu.h>
=======
#include <linux/err.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/pm_runtime.h>
#include <linux/debugfs.h>
>>>>>>> v4.9.227
#include <linux/platform_data/iommu-omap.h>

#include "omap-iopgtable.h"
#include "omap-iommu.h"

<<<<<<< HEAD
#define MAXCOLUMN 100 /* for short messages */

=======
>>>>>>> v4.9.227
static DEFINE_MUTEX(iommu_debug_lock);

static struct dentry *iommu_debug_root;

<<<<<<< HEAD
static ssize_t debug_read_ver(struct file *file, char __user *userbuf,
			      size_t count, loff_t *ppos)
{
	u32 ver = omap_iommu_arch_version();
	char buf[MAXCOLUMN], *p = buf;

	p += sprintf(p, "H/W version: %d.%d\n", (ver >> 4) & 0xf , ver & 0xf);

	return simple_read_from_buffer(userbuf, count, ppos, buf, p - buf);
=======
static inline bool is_omap_iommu_detached(struct omap_iommu *obj)
{
	return !obj->domain;
}

#define pr_reg(name)							\
	do {								\
		ssize_t bytes;						\
		const char *str = "%20s: %08x\n";			\
		const int maxcol = 32;					\
		bytes = snprintf(p, maxcol, str, __stringify(name),	\
				 iommu_read_reg(obj, MMU_##name));	\
		p += bytes;						\
		len -= bytes;						\
		if (len < maxcol)					\
			goto out;					\
	} while (0)

static ssize_t
omap2_iommu_dump_ctx(struct omap_iommu *obj, char *buf, ssize_t len)
{
	char *p = buf;

	pr_reg(REVISION);
	pr_reg(IRQSTATUS);
	pr_reg(IRQENABLE);
	pr_reg(WALKING_ST);
	pr_reg(CNTL);
	pr_reg(FAULT_AD);
	pr_reg(TTB);
	pr_reg(LOCK);
	pr_reg(LD_TLB);
	pr_reg(CAM);
	pr_reg(RAM);
	pr_reg(GFLUSH);
	pr_reg(FLUSH_ENTRY);
	pr_reg(READ_CAM);
	pr_reg(READ_RAM);
	pr_reg(EMU_FAULT_AD);
out:
	return p - buf;
}

static ssize_t omap_iommu_dump_ctx(struct omap_iommu *obj, char *buf,
				   ssize_t bytes)
{
	if (!obj || !buf)
		return -EINVAL;

	pm_runtime_get_sync(obj->dev);

	bytes = omap2_iommu_dump_ctx(obj, buf, bytes);

	pm_runtime_put_sync(obj->dev);

	return bytes;
>>>>>>> v4.9.227
}

static ssize_t debug_read_regs(struct file *file, char __user *userbuf,
			       size_t count, loff_t *ppos)
{
<<<<<<< HEAD
	struct device *dev = file->private_data;
	struct omap_iommu *obj = dev_to_omap_iommu(dev);
	char *p, *buf;
	ssize_t bytes;

=======
	struct omap_iommu *obj = file->private_data;
	char *p, *buf;
	ssize_t bytes;

	if (is_omap_iommu_detached(obj))
		return -EPERM;

>>>>>>> v4.9.227
	buf = kmalloc(count, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	p = buf;

	mutex_lock(&iommu_debug_lock);

	bytes = omap_iommu_dump_ctx(obj, p, count);
	bytes = simple_read_from_buffer(userbuf, count, ppos, buf, bytes);

	mutex_unlock(&iommu_debug_lock);
	kfree(buf);

	return bytes;
}

<<<<<<< HEAD
static ssize_t debug_read_tlb(struct file *file, char __user *userbuf,
			      size_t count, loff_t *ppos)
{
	struct device *dev = file->private_data;
	struct omap_iommu *obj = dev_to_omap_iommu(dev);
	char *p, *buf;
	ssize_t bytes, rest;

	buf = kmalloc(count, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	p = buf;

	mutex_lock(&iommu_debug_lock);

	p += sprintf(p, "%8s %8s\n", "cam:", "ram:");
	p += sprintf(p, "-----------------------------------------\n");
	rest = count - (p - buf);
	p += omap_dump_tlb_entries(obj, p, rest);

	bytes = simple_read_from_buffer(userbuf, count, ppos, buf, p - buf);

	mutex_unlock(&iommu_debug_lock);
	kfree(buf);

	return bytes;
}

static ssize_t debug_write_pagetable(struct file *file,
		     const char __user *userbuf, size_t count, loff_t *ppos)
{
	struct iotlb_entry e;
	struct cr_regs cr;
	int err;
	struct device *dev = file->private_data;
	struct omap_iommu *obj = dev_to_omap_iommu(dev);
	char buf[MAXCOLUMN], *p = buf;

	count = min(count, sizeof(buf));

	mutex_lock(&iommu_debug_lock);
	if (copy_from_user(p, userbuf, count)) {
		mutex_unlock(&iommu_debug_lock);
		return -EFAULT;
	}

	sscanf(p, "%x %x", &cr.cam, &cr.ram);
	if (!cr.cam || !cr.ram) {
		mutex_unlock(&iommu_debug_lock);
		return -EINVAL;
	}

	omap_iotlb_cr_to_e(&cr, &e);
	err = omap_iopgtable_store_entry(obj, &e);
	if (err)
		dev_err(obj->dev, "%s: fail to store cr\n", __func__);

	mutex_unlock(&iommu_debug_lock);
	return count;
}

#define dump_ioptable_entry_one(lv, da, val)			\
	({							\
		int __err = 0;					\
		ssize_t bytes;					\
		const int maxcol = 22;				\
		const char *str = "%d: %08x %08x\n";		\
		bytes = snprintf(p, maxcol, str, lv, da, val);	\
		p += bytes;					\
		len -= bytes;					\
		if (len < maxcol)				\
			__err = -ENOMEM;			\
		__err;						\
	})

static ssize_t dump_ioptable(struct omap_iommu *obj, char *buf, ssize_t len)
{
	int i;
	u32 *iopgd;
	char *p = buf;
=======
static int
__dump_tlb_entries(struct omap_iommu *obj, struct cr_regs *crs, int num)
{
	int i;
	struct iotlb_lock saved;
	struct cr_regs tmp;
	struct cr_regs *p = crs;

	pm_runtime_get_sync(obj->dev);
	iotlb_lock_get(obj, &saved);

	for_each_iotlb_cr(obj, num, i, tmp) {
		if (!iotlb_cr_valid(&tmp))
			continue;
		*p++ = tmp;
	}

	iotlb_lock_set(obj, &saved);
	pm_runtime_put_sync(obj->dev);

	return  p - crs;
}

static ssize_t iotlb_dump_cr(struct omap_iommu *obj, struct cr_regs *cr,
			     struct seq_file *s)
{
	seq_printf(s, "%08x %08x %01x\n", cr->cam, cr->ram,
		   (cr->cam & MMU_CAM_P) ? 1 : 0);
	return 0;
}

static size_t omap_dump_tlb_entries(struct omap_iommu *obj, struct seq_file *s)
{
	int i, num;
	struct cr_regs *cr;

	num = obj->nr_tlb_entries;

	cr = kcalloc(num, sizeof(*cr), GFP_KERNEL);
	if (!cr)
		return 0;

	num = __dump_tlb_entries(obj, cr, num);
	for (i = 0; i < num; i++)
		iotlb_dump_cr(obj, cr + i, s);
	kfree(cr);

	return 0;
}

static int debug_read_tlb(struct seq_file *s, void *data)
{
	struct omap_iommu *obj = s->private;

	if (is_omap_iommu_detached(obj))
		return -EPERM;

	mutex_lock(&iommu_debug_lock);

	seq_printf(s, "%8s %8s\n", "cam:", "ram:");
	seq_puts(s, "-----------------------------------------\n");
	omap_dump_tlb_entries(obj, s);

	mutex_unlock(&iommu_debug_lock);

	return 0;
}

static void dump_ioptable(struct seq_file *s)
{
	int i, j;
	u32 da;
	u32 *iopgd, *iopte;
	struct omap_iommu *obj = s->private;
>>>>>>> v4.9.227

	spin_lock(&obj->page_table_lock);

	iopgd = iopgd_offset(obj, 0);
	for (i = 0; i < PTRS_PER_IOPGD; i++, iopgd++) {
<<<<<<< HEAD
		int j, err;
		u32 *iopte;
		u32 da;

=======
>>>>>>> v4.9.227
		if (!*iopgd)
			continue;

		if (!(*iopgd & IOPGD_TABLE)) {
			da = i << IOPGD_SHIFT;
<<<<<<< HEAD

			err = dump_ioptable_entry_one(1, da, *iopgd);
			if (err)
				goto out;
=======
			seq_printf(s, "1: 0x%08x 0x%08x\n", da, *iopgd);
>>>>>>> v4.9.227
			continue;
		}

		iopte = iopte_offset(iopgd, 0);
<<<<<<< HEAD

=======
>>>>>>> v4.9.227
		for (j = 0; j < PTRS_PER_IOPTE; j++, iopte++) {
			if (!*iopte)
				continue;

			da = (i << IOPGD_SHIFT) + (j << IOPTE_SHIFT);
<<<<<<< HEAD
			err = dump_ioptable_entry_one(2, da, *iopgd);
			if (err)
				goto out;
		}
	}
out:
	spin_unlock(&obj->page_table_lock);

	return p - buf;
}

static ssize_t debug_read_pagetable(struct file *file, char __user *userbuf,
				    size_t count, loff_t *ppos)
{
	struct device *dev = file->private_data;
	struct omap_iommu *obj = dev_to_omap_iommu(dev);
	char *p, *buf;
	size_t bytes;

	buf = (char *)__get_free_page(GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	p = buf;

	p += sprintf(p, "L: %8s %8s\n", "da:", "pa:");
	p += sprintf(p, "-----------------------------------------\n");

	mutex_lock(&iommu_debug_lock);

	bytes = PAGE_SIZE - (p - buf);
	p += dump_ioptable(obj, p, bytes);

	bytes = simple_read_from_buffer(userbuf, count, ppos, buf, p - buf);

	mutex_unlock(&iommu_debug_lock);
	free_page((unsigned long)buf);

	return bytes;
}

#define DEBUG_FOPS(name)						\
	static const struct file_operations debug_##name##_fops = {	\
		.open = simple_open,					\
		.read = debug_read_##name,				\
		.write = debug_write_##name,				\
		.llseek = generic_file_llseek,				\
	};
=======
			seq_printf(s, "2: 0x%08x 0x%08x\n", da, *iopte);
		}
	}

	spin_unlock(&obj->page_table_lock);
}

static int debug_read_pagetable(struct seq_file *s, void *data)
{
	struct omap_iommu *obj = s->private;

	if (is_omap_iommu_detached(obj))
		return -EPERM;

	mutex_lock(&iommu_debug_lock);

	seq_printf(s, "L: %8s %8s\n", "da:", "pte:");
	seq_puts(s, "--------------------------\n");
	dump_ioptable(s);

	mutex_unlock(&iommu_debug_lock);

	return 0;
}

#define DEBUG_SEQ_FOPS_RO(name)						       \
	static int debug_open_##name(struct inode *inode, struct file *file)   \
	{								       \
		return single_open(file, debug_read_##name, inode->i_private); \
	}								       \
									       \
	static const struct file_operations debug_##name##_fops = {	       \
		.open		= debug_open_##name,			       \
		.read		= seq_read,				       \
		.llseek		= seq_lseek,				       \
		.release	= single_release,			       \
	}
>>>>>>> v4.9.227

#define DEBUG_FOPS_RO(name)						\
	static const struct file_operations debug_##name##_fops = {	\
		.open = simple_open,					\
		.read = debug_read_##name,				\
		.llseek = generic_file_llseek,				\
<<<<<<< HEAD
	};

DEBUG_FOPS_RO(ver);
DEBUG_FOPS_RO(regs);
DEBUG_FOPS_RO(tlb);
DEBUG_FOPS(pagetable);
=======
	}

DEBUG_FOPS_RO(regs);
DEBUG_SEQ_FOPS_RO(tlb);
DEBUG_SEQ_FOPS_RO(pagetable);
>>>>>>> v4.9.227

#define __DEBUG_ADD_FILE(attr, mode)					\
	{								\
		struct dentry *dent;					\
<<<<<<< HEAD
		dent = debugfs_create_file(#attr, mode, parent,		\
					   dev, &debug_##attr##_fops);	\
		if (!dent)						\
			return -ENOMEM;					\
	}

#define DEBUG_ADD_FILE(name) __DEBUG_ADD_FILE(name, 0600)
#define DEBUG_ADD_FILE_RO(name) __DEBUG_ADD_FILE(name, 0400)

static int iommu_debug_register(struct device *dev, void *data)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct omap_iommu *obj = platform_get_drvdata(pdev);
	struct omap_iommu_arch_data *arch_data;
	struct dentry *d, *parent;

	if (!obj || !obj->dev)
		return -EINVAL;

	arch_data = kzalloc(sizeof(*arch_data), GFP_KERNEL);
	if (!arch_data)
		return -ENOMEM;

	arch_data->iommu_dev = obj;

	dev->archdata.iommu = arch_data;

	d = debugfs_create_dir(obj->name, iommu_debug_root);
	if (!d)
		goto nomem;
	parent = d;

	d = debugfs_create_u8("nr_tlb_entries", 400, parent,
			      (u8 *)&obj->nr_tlb_entries);
	if (!d)
		goto nomem;

	DEBUG_ADD_FILE_RO(ver);
	DEBUG_ADD_FILE_RO(regs);
	DEBUG_ADD_FILE_RO(tlb);
	DEBUG_ADD_FILE(pagetable);

	return 0;

nomem:
	kfree(arch_data);
	return -ENOMEM;
}

static int iommu_debug_unregister(struct device *dev, void *data)
{
	if (!dev->archdata.iommu)
		return 0;

	kfree(dev->archdata.iommu);

	dev->archdata.iommu = NULL;

	return 0;
}

static int __init iommu_debug_init(void)
{
	struct dentry *d;
	int err;

	d = debugfs_create_dir("iommu", NULL);
	if (!d)
		return -ENOMEM;
	iommu_debug_root = d;

	err = omap_foreach_iommu_device(d, iommu_debug_register);
	if (err)
		goto err_out;
	return 0;

err_out:
	debugfs_remove_recursive(iommu_debug_root);
	return err;
}
module_init(iommu_debug_init)

static void __exit iommu_debugfs_exit(void)
{
	debugfs_remove_recursive(iommu_debug_root);
	omap_foreach_iommu_device(NULL, iommu_debug_unregister);
}
module_exit(iommu_debugfs_exit)

MODULE_DESCRIPTION("omap iommu: debugfs interface");
MODULE_AUTHOR("Hiroshi DOYU <Hiroshi.DOYU@nokia.com>");
MODULE_LICENSE("GPL v2");
=======
		dent = debugfs_create_file(#attr, mode, obj->debug_dir,	\
					   obj, &debug_##attr##_fops);	\
		if (!dent)						\
			goto err;					\
	}

#define DEBUG_ADD_FILE_RO(name) __DEBUG_ADD_FILE(name, 0400)

void omap_iommu_debugfs_add(struct omap_iommu *obj)
{
	struct dentry *d;

	if (!iommu_debug_root)
		return;

	obj->debug_dir = debugfs_create_dir(obj->name, iommu_debug_root);
	if (!obj->debug_dir)
		return;

	d = debugfs_create_u8("nr_tlb_entries", 0400, obj->debug_dir,
			      (u8 *)&obj->nr_tlb_entries);
	if (!d)
		return;

	DEBUG_ADD_FILE_RO(regs);
	DEBUG_ADD_FILE_RO(tlb);
	DEBUG_ADD_FILE_RO(pagetable);

	return;

err:
	debugfs_remove_recursive(obj->debug_dir);
}

void omap_iommu_debugfs_remove(struct omap_iommu *obj)
{
	if (!obj->debug_dir)
		return;

	debugfs_remove_recursive(obj->debug_dir);
}

void __init omap_iommu_debugfs_init(void)
{
	iommu_debug_root = debugfs_create_dir("omap_iommu", NULL);
	if (!iommu_debug_root)
		pr_err("can't create debugfs dir\n");
}

void __exit omap_iommu_debugfs_exit(void)
{
	debugfs_remove(iommu_debug_root);
}
>>>>>>> v4.9.227
