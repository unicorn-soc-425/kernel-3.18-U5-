/*
 * pci.c - DesignWare HS OTG Controller PCI driver
 *
 * Copyright (C) 2004-2013 Synopsys, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the above-listed copyright holders may not be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Provides the initialization and cleanup entry points for the DWC_otg PCI
 * driver
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/usb.h>

#include <linux/usb/hcd.h>
#include <linux/usb/ch11.h>
<<<<<<< HEAD

#include "core.h"
#include "hcd.h"

#define PCI_VENDOR_ID_SYNOPSYS		0x16c3
#define PCI_PRODUCT_ID_HAPS_HSOTG	0xabc0

static const char dwc2_driver_name[] = "dwc2";

static const struct dwc2_core_params dwc2_module_params = {
	.otg_cap			= -1,
	.otg_ver			= -1,
	.dma_enable			= -1,
	.dma_desc_enable		= 0,
	.speed				= -1,
	.enable_dynamic_fifo		= -1,
	.en_multiple_tx_fifo		= -1,
	.host_rx_fifo_size		= 1024,
	.host_nperio_tx_fifo_size	= 256,
	.host_perio_tx_fifo_size	= 1024,
	.max_transfer_size		= 65535,
	.max_packet_count		= 511,
	.host_channels			= -1,
	.phy_type			= -1,
	.phy_utmi_width			= -1,
	.phy_ulpi_ddr			= -1,
	.phy_ulpi_ext_vbus		= -1,
	.i2c_enable			= -1,
	.ulpi_fs_ls			= -1,
	.host_support_fs_ls_low_power	= -1,
	.host_ls_low_power_phy_clk	= -1,
	.ts_dline			= -1,
	.reload_ctl			= -1,
	.ahbcfg				= -1,
	.uframe_sched			= -1,
};

/**
 * dwc2_driver_remove() - Called when the DWC_otg core is unregistered with the
 * DWC_otg driver
 *
 * @dev: Bus device
 *
 * This routine is called, for example, when the rmmod command is executed. The
 * device may or may not be electrically present. If it is present, the driver
 * stops device processing. Any resources used on behalf of this device are
 * freed.
 */
static void dwc2_driver_remove(struct pci_dev *dev)
{
	struct dwc2_hsotg *hsotg = pci_get_drvdata(dev);

	dwc2_hcd_remove(hsotg);
	pci_disable_device(dev);
}

/**
 * dwc2_driver_probe() - Called when the DWC_otg core is bound to the DWC_otg
 * driver
 *
 * @dev: Bus device
 *
 * This routine creates the driver components required to control the device
 * (core, HCD, and PCD) and initializes the device. The driver components are
 * stored in a dwc2_hsotg structure. A reference to the dwc2_hsotg is saved
 * in the device private data. This allows the driver to access the dwc2_hsotg
 * structure on subsequent calls to driver methods for this device.
 */
static int dwc2_driver_probe(struct pci_dev *dev,
			     const struct pci_device_id *id)
{
	struct dwc2_hsotg *hsotg;
	int retval;

	hsotg = devm_kzalloc(&dev->dev, sizeof(*hsotg), GFP_KERNEL);
	if (!hsotg)
		return -ENOMEM;

	hsotg->dev = &dev->dev;
	hsotg->regs = devm_ioremap_resource(&dev->dev, &dev->resource[0]);
	if (IS_ERR(hsotg->regs))
		return PTR_ERR(hsotg->regs);

	dev_dbg(&dev->dev, "mapped PA %08lx to VA %p\n",
		(unsigned long)pci_resource_start(dev, 0), hsotg->regs);

	if (pci_enable_device(dev) < 0)
		return -ENODEV;

	pci_set_master(dev);

	retval = dwc2_hcd_init(hsotg, dev->irq, &dwc2_module_params);
	if (retval) {
		pci_disable_device(dev);
		return retval;
	}

	pci_set_drvdata(dev, hsotg);

	return retval;
=======
#include <linux/platform_device.h>
#include <linux/usb/usb_phy_generic.h>

#define PCI_PRODUCT_ID_HAPS_HSOTG	0xabc0

static const char dwc2_driver_name[] = "dwc2-pci";

struct dwc2_pci_glue {
	struct platform_device *dwc2;
	struct platform_device *phy;
};

static void dwc2_pci_remove(struct pci_dev *pci)
{
	struct dwc2_pci_glue *glue = pci_get_drvdata(pci);

	platform_device_unregister(glue->dwc2);
	usb_phy_generic_unregister(glue->phy);
	kfree(glue);
	pci_set_drvdata(pci, NULL);
}

static int dwc2_pci_probe(struct pci_dev *pci,
		const struct pci_device_id *id)
{
	struct resource		res[2];
	struct platform_device	*dwc2;
	struct platform_device	*phy;
	int			ret;
	struct device		*dev = &pci->dev;
	struct dwc2_pci_glue	*glue;

	ret = pcim_enable_device(pci);
	if (ret) {
		dev_err(dev, "failed to enable pci device\n");
		return -ENODEV;
	}

	pci_set_master(pci);

	dwc2 = platform_device_alloc("dwc2", PLATFORM_DEVID_AUTO);
	if (!dwc2) {
		dev_err(dev, "couldn't allocate dwc2 device\n");
		return -ENOMEM;
	}

	memset(res, 0x00, sizeof(struct resource) * ARRAY_SIZE(res));

	res[0].start	= pci_resource_start(pci, 0);
	res[0].end	= pci_resource_end(pci, 0);
	res[0].name	= "dwc2";
	res[0].flags	= IORESOURCE_MEM;

	res[1].start	= pci->irq;
	res[1].name	= "dwc2";
	res[1].flags	= IORESOURCE_IRQ;

	ret = platform_device_add_resources(dwc2, res, ARRAY_SIZE(res));
	if (ret) {
		dev_err(dev, "couldn't add resources to dwc2 device\n");
		return ret;
	}

	dwc2->dev.parent = dev;

	phy = usb_phy_generic_register();
	if (IS_ERR(phy)) {
		dev_err(dev, "error registering generic PHY (%ld)\n",
			PTR_ERR(phy));
		return PTR_ERR(phy);
	}

	ret = platform_device_add(dwc2);
	if (ret) {
		dev_err(dev, "failed to register dwc2 device\n");
		goto err;
	}

	glue = kzalloc(sizeof(*glue), GFP_KERNEL);
	if (!glue)
		return -ENOMEM;

	glue->phy = phy;
	glue->dwc2 = dwc2;
	pci_set_drvdata(pci, glue);

	return 0;
err:
	usb_phy_generic_unregister(phy);
	platform_device_put(dwc2);
	return ret;
>>>>>>> v4.9.227
}

static const struct pci_device_id dwc2_pci_ids[] = {
	{
		PCI_DEVICE(PCI_VENDOR_ID_SYNOPSYS, PCI_PRODUCT_ID_HAPS_HSOTG),
	},
	{
		PCI_DEVICE(PCI_VENDOR_ID_STMICRO,
			   PCI_DEVICE_ID_STMICRO_USB_OTG),
	},
	{ /* end: all zeroes */ }
};
MODULE_DEVICE_TABLE(pci, dwc2_pci_ids);

static struct pci_driver dwc2_pci_driver = {
	.name = dwc2_driver_name,
	.id_table = dwc2_pci_ids,
<<<<<<< HEAD
	.probe = dwc2_driver_probe,
	.remove = dwc2_driver_remove,
=======
	.probe = dwc2_pci_probe,
	.remove = dwc2_pci_remove,
>>>>>>> v4.9.227
};

module_pci_driver(dwc2_pci_driver);

MODULE_DESCRIPTION("DESIGNWARE HS OTG PCI Bus Glue");
MODULE_AUTHOR("Synopsys, Inc.");
MODULE_LICENSE("Dual BSD/GPL");
