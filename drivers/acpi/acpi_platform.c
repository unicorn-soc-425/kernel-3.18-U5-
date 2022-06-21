/*
 * ACPI support for platform bus type.
 *
 * Copyright (C) 2012, Intel Corporation
 * Authors: Mika Westerberg <mika.westerberg@linux.intel.com>
 *          Mathias Nyman <mathias.nyman@linux.intel.com>
 *          Rafael J. Wysocki <rafael.j.wysocki@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/acpi.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/dma-mapping.h>
<<<<<<< HEAD
=======
#include <linux/pci.h>
>>>>>>> v4.9.227
#include <linux/platform_device.h>

#include "internal.h"

ACPI_MODULE_NAME("platform");

static const struct acpi_device_id forbidden_id_list[] = {
	{"PNP0000",  0},	/* PIC */
	{"PNP0100",  0},	/* Timer */
	{"PNP0200",  0},	/* AT DMA Controller */
	{"ACPI0009", 0},	/* IOxAPIC */
	{"ACPI000A", 0},	/* IOAPIC */
<<<<<<< HEAD
	{"", 0},
};

/**
 * acpi_create_platform_device - Create platform device for ACPI device node
 * @adev: ACPI device node to create a platform device for.
=======
	{"SMB0001",  0},	/* ACPI SMBUS virtual device */
	{"", 0},
};

static void acpi_platform_fill_resource(struct acpi_device *adev,
	const struct resource *src, struct resource *dest)
{
	struct device *parent;

	*dest = *src;

	/*
	 * If the device has parent we need to take its resources into
	 * account as well because this device might consume part of those.
	 */
	parent = acpi_get_first_physical_node(adev->parent);
	if (parent && dev_is_pci(parent))
		dest->parent = pci_find_resource(to_pci_dev(parent), dest);
}

/**
 * acpi_create_platform_device - Create platform device for ACPI device node
 * @adev: ACPI device node to create a platform device for.
 * @properties: Optional collection of build-in properties.
>>>>>>> v4.9.227
 *
 * Check if the given @adev can be represented as a platform device and, if
 * that's the case, create and register a platform device, populate its common
 * resources and returns a pointer to it.  Otherwise, return %NULL.
 *
 * Name of the platform device will be the same as @adev's.
 */
<<<<<<< HEAD
struct platform_device *acpi_create_platform_device(struct acpi_device *adev)
{
	struct platform_device *pdev = NULL;
	struct acpi_device *acpi_parent;
	struct platform_device_info pdevinfo;
	struct resource_list_entry *rentry;
=======
struct platform_device *acpi_create_platform_device(struct acpi_device *adev,
					struct property_entry *properties)
{
	struct platform_device *pdev = NULL;
	struct platform_device_info pdevinfo;
	struct resource_entry *rentry;
>>>>>>> v4.9.227
	struct list_head resource_list;
	struct resource *resources = NULL;
	int count;

	/* If the ACPI node already has a physical device attached, skip it. */
	if (adev->physical_node_count)
		return NULL;

	if (!acpi_match_device_ids(adev, forbidden_id_list))
		return ERR_PTR(-EINVAL);

	INIT_LIST_HEAD(&resource_list);
	count = acpi_dev_get_resources(adev, &resource_list, NULL, NULL);
	if (count < 0) {
		return NULL;
	} else if (count > 0) {
<<<<<<< HEAD
		resources = kmalloc(count * sizeof(struct resource),
=======
		resources = kzalloc(count * sizeof(struct resource),
>>>>>>> v4.9.227
				    GFP_KERNEL);
		if (!resources) {
			dev_err(&adev->dev, "No memory for resources\n");
			acpi_dev_free_resource_list(&resource_list);
			return ERR_PTR(-ENOMEM);
		}
		count = 0;
		list_for_each_entry(rentry, &resource_list, node)
<<<<<<< HEAD
			resources[count++] = rentry->res;
=======
			acpi_platform_fill_resource(adev, rentry->res,
						    &resources[count++]);
>>>>>>> v4.9.227

		acpi_dev_free_resource_list(&resource_list);
	}

	memset(&pdevinfo, 0, sizeof(pdevinfo));
	/*
	 * If the ACPI node has a parent and that parent has a physical device
	 * attached to it, that physical device should be the parent of the
	 * platform device we are about to create.
	 */
<<<<<<< HEAD
	pdevinfo.parent = NULL;
	acpi_parent = adev->parent;
	if (acpi_parent) {
		struct acpi_device_physical_node *entry;
		struct list_head *list;

		mutex_lock(&acpi_parent->physical_node_lock);
		list = &acpi_parent->physical_node_list;
		if (!list_empty(list)) {
			entry = list_first_entry(list,
					struct acpi_device_physical_node,
					node);
			pdevinfo.parent = entry->dev;
		}
		mutex_unlock(&acpi_parent->physical_node_lock);
	}
=======
	pdevinfo.parent = adev->parent ?
		acpi_get_first_physical_node(adev->parent) : NULL;
>>>>>>> v4.9.227
	pdevinfo.name = dev_name(&adev->dev);
	pdevinfo.id = -1;
	pdevinfo.res = resources;
	pdevinfo.num_res = count;
<<<<<<< HEAD
	pdevinfo.acpi_node.companion = adev;
	pdevinfo.dma_mask = DMA_BIT_MASK(32);
=======
	pdevinfo.fwnode = acpi_fwnode_handle(adev);
	pdevinfo.properties = properties;

	if (acpi_dma_supported(adev))
		pdevinfo.dma_mask = DMA_BIT_MASK(32);
	else
		pdevinfo.dma_mask = 0;

>>>>>>> v4.9.227
	pdev = platform_device_register_full(&pdevinfo);
	if (IS_ERR(pdev))
		dev_err(&adev->dev, "platform device creation failed: %ld\n",
			PTR_ERR(pdev));
	else
		dev_dbg(&adev->dev, "created platform device %s\n",
			dev_name(&pdev->dev));

	kfree(resources);
	return pdev;
}
EXPORT_SYMBOL_GPL(acpi_create_platform_device);
