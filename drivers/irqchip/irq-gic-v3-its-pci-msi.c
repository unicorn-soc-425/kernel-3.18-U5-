/*
 * Copyright (C) 2013-2015 ARM Limited, All Rights Reserved.
 * Author: Marc Zyngier <marc.zyngier@arm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

<<<<<<< HEAD
=======
#include <linux/acpi_iort.h>
>>>>>>> v4.9.227
#include <linux/msi.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_pci.h>

static void its_mask_msi_irq(struct irq_data *d)
{
	pci_msi_mask_irq(d);
	irq_chip_mask_parent(d);
}

static void its_unmask_msi_irq(struct irq_data *d)
{
	pci_msi_unmask_irq(d);
	irq_chip_unmask_parent(d);
}

static struct irq_chip its_msi_irq_chip = {
	.name			= "ITS-MSI",
	.irq_unmask		= its_unmask_msi_irq,
	.irq_mask		= its_mask_msi_irq,
	.irq_eoi		= irq_chip_eoi_parent,
	.irq_write_msi_msg	= pci_msi_domain_write_msg,
};

struct its_pci_alias {
	struct pci_dev	*pdev;
<<<<<<< HEAD
	u32		dev_id;
=======
>>>>>>> v4.9.227
	u32		count;
};

static int its_pci_msi_vec_count(struct pci_dev *pdev)
{
	int msi, msix;

	msi = max(pci_msi_vec_count(pdev), 0);
	msix = max(pci_msix_vec_count(pdev), 0);

	return max(msi, msix);
}

static int its_get_pci_alias(struct pci_dev *pdev, u16 alias, void *data)
{
	struct its_pci_alias *dev_alias = data;

<<<<<<< HEAD
	dev_alias->dev_id = alias;
	if (pdev != dev_alias->pdev)
		dev_alias->count += its_pci_msi_vec_count(dev_alias->pdev);
=======
	if (pdev != dev_alias->pdev)
		dev_alias->count += its_pci_msi_vec_count(pdev);
>>>>>>> v4.9.227

	return 0;
}

static int its_pci_msi_prepare(struct irq_domain *domain, struct device *dev,
			       int nvec, msi_alloc_info_t *info)
{
	struct pci_dev *pdev;
	struct its_pci_alias dev_alias;
	struct msi_domain_info *msi_info;

	if (!dev_is_pci(dev))
		return -EINVAL;

	msi_info = msi_get_domain_info(domain->parent);

	pdev = to_pci_dev(dev);
	dev_alias.pdev = pdev;
	dev_alias.count = nvec;

	pci_for_each_dma_alias(pdev, its_get_pci_alias, &dev_alias);

	/* ITS specific DeviceID, as the core ITS ignores dev. */
<<<<<<< HEAD
	info->scratchpad[0].ul = dev_alias.dev_id;
=======
	info->scratchpad[0].ul = pci_msi_domain_get_msi_rid(domain, pdev);
>>>>>>> v4.9.227

	return msi_info->ops->msi_prepare(domain->parent,
					  dev, dev_alias.count, info);
}

static struct msi_domain_ops its_pci_msi_ops = {
	.msi_prepare	= its_pci_msi_prepare,
};

static struct msi_domain_info its_pci_msi_domain_info = {
	.flags	= (MSI_FLAG_USE_DEF_DOM_OPS | MSI_FLAG_USE_DEF_CHIP_OPS |
		   MSI_FLAG_MULTI_PCI_MSI | MSI_FLAG_PCI_MSIX),
	.ops	= &its_pci_msi_ops,
	.chip	= &its_msi_irq_chip,
};

static struct of_device_id its_device_id[] = {
	{	.compatible	= "arm,gic-v3-its",	},
	{},
};

<<<<<<< HEAD
static int __init its_pci_msi_init(void)
{
	struct device_node *np;
	struct irq_domain *parent;

	for (np = of_find_matching_node(NULL, its_device_id); np;
	     np = of_find_matching_node(np, its_device_id)) {
		if (!of_property_read_bool(np, "msi-controller"))
			continue;

		parent = irq_find_matching_host(np, DOMAIN_BUS_NEXUS);
		if (!parent || !msi_get_domain_info(parent)) {
			pr_err("%s: unable to locate ITS domain\n",
			       np->full_name);
			continue;
		}

		if (!pci_msi_create_irq_domain(np, &its_pci_msi_domain_info,
					       parent)) {
			pr_err("%s: unable to create PCI domain\n",
			       np->full_name);
			continue;
		}
=======
static int __init its_pci_msi_init_one(struct fwnode_handle *handle,
				       const char *name)
{
	struct irq_domain *parent;

	parent = irq_find_matching_fwnode(handle, DOMAIN_BUS_NEXUS);
	if (!parent || !msi_get_domain_info(parent)) {
		pr_err("%s: Unable to locate ITS domain\n", name);
		return -ENXIO;
	}

	if (!pci_msi_create_irq_domain(handle, &its_pci_msi_domain_info,
				       parent)) {
		pr_err("%s: Unable to create PCI domain\n", name);
		return -ENOMEM;
	}

	return 0;
}

static int __init its_pci_of_msi_init(void)
{
	struct device_node *np;

	for (np = of_find_matching_node(NULL, its_device_id); np;
	     np = of_find_matching_node(np, its_device_id)) {
		if (!of_device_is_available(np))
			continue;
		if (!of_property_read_bool(np, "msi-controller"))
			continue;

		if (its_pci_msi_init_one(of_node_to_fwnode(np), np->full_name))
			continue;
>>>>>>> v4.9.227

		pr_info("PCI/MSI: %s domain created\n", np->full_name);
	}

	return 0;
}
<<<<<<< HEAD
=======

#ifdef CONFIG_ACPI

static int __init
its_pci_msi_parse_madt(struct acpi_subtable_header *header,
		       const unsigned long end)
{
	struct acpi_madt_generic_translator *its_entry;
	struct fwnode_handle *dom_handle;
	const char *node_name;
	int err = -ENXIO;

	its_entry = (struct acpi_madt_generic_translator *)header;
	node_name = kasprintf(GFP_KERNEL, "ITS@0x%lx",
			      (long)its_entry->base_address);
	dom_handle = iort_find_domain_token(its_entry->translation_id);
	if (!dom_handle) {
		pr_err("%s: Unable to locate ITS domain handle\n", node_name);
		goto out;
	}

	err = its_pci_msi_init_one(dom_handle, node_name);
	if (!err)
		pr_info("PCI/MSI: %s domain created\n", node_name);

out:
	kfree(node_name);
	return err;
}

static int __init its_pci_acpi_msi_init(void)
{
	acpi_table_parse_madt(ACPI_MADT_TYPE_GENERIC_TRANSLATOR,
			      its_pci_msi_parse_madt, 0);
	return 0;
}
#else
static int __init its_pci_acpi_msi_init(void)
{
	return 0;
}
#endif

static int __init its_pci_msi_init(void)
{
	its_pci_of_msi_init();
	its_pci_acpi_msi_init();

	return 0;
}
>>>>>>> v4.9.227
early_initcall(its_pci_msi_init);
