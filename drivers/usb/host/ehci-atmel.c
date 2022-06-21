/*
 * Driver for EHCI UHP on Atmel chips
 *
 *  Copyright (C) 2009 Atmel Corporation,
 *                     Nicolas Ferre <nicolas.ferre@atmel.com>
 *
 *  Based on various ehci-*.c drivers
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive for
 * more details.
 */

#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>

#include "ehci.h"

#define DRIVER_DESC "EHCI Atmel driver"

static const char hcd_name[] = "ehci-atmel";
<<<<<<< HEAD
static struct hc_driver __read_mostly ehci_atmel_hc_driver;

/* interface and function clocks */
static struct clk *iclk, *fclk, *uclk;
static int clocked;

/*-------------------------------------------------------------------------*/

static void atmel_start_clock(void)
{
	if (IS_ENABLED(CONFIG_COMMON_CLK)) {
		clk_set_rate(uclk, 48000000);
		clk_prepare_enable(uclk);
	}
	clk_prepare_enable(iclk);
	clk_prepare_enable(fclk);
	clocked = 1;
}

static void atmel_stop_clock(void)
{
	clk_disable_unprepare(fclk);
	clk_disable_unprepare(iclk);
	if (IS_ENABLED(CONFIG_COMMON_CLK))
		clk_disable_unprepare(uclk);
	clocked = 0;
=======

/* interface and function clocks */
#define hcd_to_atmel_ehci_priv(h) \
	((struct atmel_ehci_priv *)hcd_to_ehci(h)->priv)

struct atmel_ehci_priv {
	struct clk *iclk;
	struct clk *uclk;
	bool clocked;
};

static struct hc_driver __read_mostly ehci_atmel_hc_driver;

static const struct ehci_driver_overrides ehci_atmel_drv_overrides __initconst = {
	.extra_priv_size = sizeof(struct atmel_ehci_priv),
};

/*-------------------------------------------------------------------------*/

static void atmel_start_clock(struct atmel_ehci_priv *atmel_ehci)
{
	if (atmel_ehci->clocked)
		return;

	clk_prepare_enable(atmel_ehci->uclk);
	clk_prepare_enable(atmel_ehci->iclk);
	atmel_ehci->clocked = true;
}

static void atmel_stop_clock(struct atmel_ehci_priv *atmel_ehci)
{
	if (!atmel_ehci->clocked)
		return;

	clk_disable_unprepare(atmel_ehci->iclk);
	clk_disable_unprepare(atmel_ehci->uclk);
	atmel_ehci->clocked = false;
>>>>>>> v4.9.227
}

static void atmel_start_ehci(struct platform_device *pdev)
{
<<<<<<< HEAD
	dev_dbg(&pdev->dev, "start\n");
	atmel_start_clock();
=======
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct atmel_ehci_priv *atmel_ehci = hcd_to_atmel_ehci_priv(hcd);

	dev_dbg(&pdev->dev, "start\n");
	atmel_start_clock(atmel_ehci);
>>>>>>> v4.9.227
}

static void atmel_stop_ehci(struct platform_device *pdev)
{
<<<<<<< HEAD
	dev_dbg(&pdev->dev, "stop\n");
	atmel_stop_clock();
=======
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct atmel_ehci_priv *atmel_ehci = hcd_to_atmel_ehci_priv(hcd);

	dev_dbg(&pdev->dev, "stop\n");
	atmel_stop_clock(atmel_ehci);
>>>>>>> v4.9.227
}

/*-------------------------------------------------------------------------*/

static int ehci_atmel_drv_probe(struct platform_device *pdev)
{
	struct usb_hcd *hcd;
	const struct hc_driver *driver = &ehci_atmel_hc_driver;
	struct resource *res;
	struct ehci_hcd *ehci;
<<<<<<< HEAD
=======
	struct atmel_ehci_priv *atmel_ehci;
>>>>>>> v4.9.227
	int irq;
	int retval;

	if (usb_disabled())
		return -ENODEV;

	pr_debug("Initializing Atmel-SoC USB Host Controller\n");

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0) {
		dev_err(&pdev->dev,
			"Found HC with no IRQ. Check %s setup!\n",
			dev_name(&pdev->dev));
		retval = -ENODEV;
		goto fail_create_hcd;
	}

	/* Right now device-tree probed devices don't get dma_mask set.
	 * Since shared usb code relies on it, set it here for now.
	 * Once we have dma capability bindings this can go away.
	 */
	retval = dma_coerce_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));
	if (retval)
		goto fail_create_hcd;

	hcd = usb_create_hcd(driver, &pdev->dev, dev_name(&pdev->dev));
	if (!hcd) {
		retval = -ENOMEM;
		goto fail_create_hcd;
	}
<<<<<<< HEAD

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev,
			"Found HC with no register addr. Check %s setup!\n",
			dev_name(&pdev->dev));
		retval = -ENODEV;
		goto fail_request_resource;
	}
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);

=======
	atmel_ehci = hcd_to_atmel_ehci_priv(hcd);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
>>>>>>> v4.9.227
	hcd->regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(hcd->regs)) {
		retval = PTR_ERR(hcd->regs);
		goto fail_request_resource;
	}

<<<<<<< HEAD
	iclk = devm_clk_get(&pdev->dev, "ehci_clk");
	if (IS_ERR(iclk)) {
=======
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);

	atmel_ehci->iclk = devm_clk_get(&pdev->dev, "ehci_clk");
	if (IS_ERR(atmel_ehci->iclk)) {
>>>>>>> v4.9.227
		dev_err(&pdev->dev, "Error getting interface clock\n");
		retval = -ENOENT;
		goto fail_request_resource;
	}
<<<<<<< HEAD
	fclk = devm_clk_get(&pdev->dev, "uhpck");
	if (IS_ERR(fclk)) {
		dev_err(&pdev->dev, "Error getting function clock\n");
		retval = -ENOENT;
		goto fail_request_resource;
	}
	if (IS_ENABLED(CONFIG_COMMON_CLK)) {
		uclk = devm_clk_get(&pdev->dev, "usb_clk");
		if (IS_ERR(uclk)) {
			dev_err(&pdev->dev, "failed to get uclk\n");
			retval = PTR_ERR(uclk);
			goto fail_request_resource;
		}
	}
=======

	atmel_ehci->uclk = devm_clk_get(&pdev->dev, "usb_clk");
	if (IS_ERR(atmel_ehci->uclk)) {
		dev_err(&pdev->dev, "failed to get uclk\n");
		retval = PTR_ERR(atmel_ehci->uclk);
		goto fail_request_resource;
	}
>>>>>>> v4.9.227

	ehci = hcd_to_ehci(hcd);
	/* registers start at offset 0x0 */
	ehci->caps = hcd->regs;

	atmel_start_ehci(pdev);

	retval = usb_add_hcd(hcd, irq, IRQF_SHARED);
	if (retval)
		goto fail_add_hcd;
	device_wakeup_enable(hcd->self.controller);

	return retval;

fail_add_hcd:
	atmel_stop_ehci(pdev);
fail_request_resource:
	usb_put_hcd(hcd);
fail_create_hcd:
	dev_err(&pdev->dev, "init %s fail, %d\n",
		dev_name(&pdev->dev), retval);

	return retval;
}

static int ehci_atmel_drv_remove(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);

	usb_remove_hcd(hcd);
	usb_put_hcd(hcd);

	atmel_stop_ehci(pdev);
<<<<<<< HEAD
	fclk = iclk = NULL;
=======
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
=======
static int __maybe_unused ehci_atmel_drv_suspend(struct device *dev)
{
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct atmel_ehci_priv *atmel_ehci = hcd_to_atmel_ehci_priv(hcd);
	int ret;

	ret = ehci_suspend(hcd, false);
	if (ret)
		return ret;

	atmel_stop_clock(atmel_ehci);
	return 0;
}

static int __maybe_unused ehci_atmel_drv_resume(struct device *dev)
{
	struct usb_hcd *hcd = dev_get_drvdata(dev);
	struct atmel_ehci_priv *atmel_ehci = hcd_to_atmel_ehci_priv(hcd);

	atmel_start_clock(atmel_ehci);
	return ehci_resume(hcd, false);
}

>>>>>>> v4.9.227
#ifdef CONFIG_OF
static const struct of_device_id atmel_ehci_dt_ids[] = {
	{ .compatible = "atmel,at91sam9g45-ehci" },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, atmel_ehci_dt_ids);
#endif

<<<<<<< HEAD
=======
static SIMPLE_DEV_PM_OPS(ehci_atmel_pm_ops, ehci_atmel_drv_suspend,
					ehci_atmel_drv_resume);

>>>>>>> v4.9.227
static struct platform_driver ehci_atmel_driver = {
	.probe		= ehci_atmel_drv_probe,
	.remove		= ehci_atmel_drv_remove,
	.shutdown	= usb_hcd_platform_shutdown,
	.driver		= {
		.name	= "atmel-ehci",
<<<<<<< HEAD
=======
		.pm	= &ehci_atmel_pm_ops,
>>>>>>> v4.9.227
		.of_match_table	= of_match_ptr(atmel_ehci_dt_ids),
	},
};

static int __init ehci_atmel_init(void)
{
	if (usb_disabled())
		return -ENODEV;

	pr_info("%s: " DRIVER_DESC "\n", hcd_name);
<<<<<<< HEAD
	ehci_init_driver(&ehci_atmel_hc_driver, NULL);
=======
	ehci_init_driver(&ehci_atmel_hc_driver, &ehci_atmel_drv_overrides);
>>>>>>> v4.9.227
	return platform_driver_register(&ehci_atmel_driver);
}
module_init(ehci_atmel_init);

static void __exit ehci_atmel_cleanup(void)
{
	platform_driver_unregister(&ehci_atmel_driver);
}
module_exit(ehci_atmel_cleanup);

MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_ALIAS("platform:atmel-ehci");
MODULE_AUTHOR("Nicolas Ferre");
MODULE_LICENSE("GPL");
