/*
 * OHCI HCD (Host Controller Driver) for USB.
 *
 *  Copyright (C) 2004 SAN People (Pty) Ltd.
 *  Copyright (C) 2005 Thibaut VARENE <varenet@parisc-linux.org>
 *
 * AT91 Bus Glue
 *
 * Based on fragments of 2.4 driver by Rick Bronson.
 * Based on ohci-omap.c
 *
 * This file is licenced under the GPL.
 */

#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/platform_data/atmel.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
<<<<<<< HEAD
#include <linux/usb.h>
#include <linux/usb/hcd.h>

#include <mach/hardware.h>
#include <asm/gpio.h>

#include <mach/cpu.h>

=======
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include <soc/at91/atmel-sfr.h>
>>>>>>> v4.9.227

#include "ohci.h"

#define valid_port(index)	((index) >= 0 && (index) < AT91_MAX_USBH_PORTS)
#define at91_for_each_port(index)	\
		for ((index) = 0; (index) < AT91_MAX_USBH_PORTS; (index)++)

/* interface, function and usb clocks; sometimes also an AHB clock */
<<<<<<< HEAD
static struct clk *iclk, *fclk, *uclk, *hclk;
=======
#define hcd_to_ohci_at91_priv(h) \
	((struct ohci_at91_priv *)hcd_to_ohci(h)->priv)

#define AT91_MAX_USBH_PORTS	3
struct at91_usbh_data {
	int vbus_pin[AT91_MAX_USBH_PORTS];	/* port power-control pin */
	int overcurrent_pin[AT91_MAX_USBH_PORTS];
	u8 ports;				/* number of ports on root hub */
	u8 overcurrent_supported;
	u8 vbus_pin_active_low[AT91_MAX_USBH_PORTS];
	u8 overcurrent_status[AT91_MAX_USBH_PORTS];
	u8 overcurrent_changed[AT91_MAX_USBH_PORTS];
};

struct ohci_at91_priv {
	struct clk *iclk;
	struct clk *fclk;
	struct clk *hclk;
	bool clocked;
	bool wakeup;		/* Saved wake-up state for resume */
	struct regmap *sfr_regmap;
};
>>>>>>> v4.9.227
/* interface and function clocks; sometimes also an AHB clock */

#define DRIVER_DESC "OHCI Atmel driver"

static const char hcd_name[] = "ohci-atmel";

static struct hc_driver __read_mostly ohci_at91_hc_driver;
<<<<<<< HEAD
static int clocked;
=======

static const struct ohci_driver_overrides ohci_at91_drv_overrides __initconst = {
	.extra_priv_size = sizeof(struct ohci_at91_priv),
};
>>>>>>> v4.9.227

extern int usb_disabled(void);

/*-------------------------------------------------------------------------*/

<<<<<<< HEAD
static void at91_start_clock(void)
{
	if (IS_ENABLED(CONFIG_COMMON_CLK)) {
		clk_set_rate(uclk, 48000000);
		clk_prepare_enable(uclk);
	}
	clk_prepare_enable(hclk);
	clk_prepare_enable(iclk);
	clk_prepare_enable(fclk);
	clocked = 1;
}

static void at91_stop_clock(void)
{
	clk_disable_unprepare(fclk);
	clk_disable_unprepare(iclk);
	clk_disable_unprepare(hclk);
	if (IS_ENABLED(CONFIG_COMMON_CLK))
		clk_disable_unprepare(uclk);
	clocked = 0;
=======
static void at91_start_clock(struct ohci_at91_priv *ohci_at91)
{
	if (ohci_at91->clocked)
		return;

	clk_set_rate(ohci_at91->fclk, 48000000);
	clk_prepare_enable(ohci_at91->hclk);
	clk_prepare_enable(ohci_at91->iclk);
	clk_prepare_enable(ohci_at91->fclk);
	ohci_at91->clocked = true;
}

static void at91_stop_clock(struct ohci_at91_priv *ohci_at91)
{
	if (!ohci_at91->clocked)
		return;

	clk_disable_unprepare(ohci_at91->fclk);
	clk_disable_unprepare(ohci_at91->iclk);
	clk_disable_unprepare(ohci_at91->hclk);
	ohci_at91->clocked = false;
>>>>>>> v4.9.227
}

static void at91_start_hc(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct ohci_regs __iomem *regs = hcd->regs;
<<<<<<< HEAD
=======
	struct ohci_at91_priv *ohci_at91 = hcd_to_ohci_at91_priv(hcd);
>>>>>>> v4.9.227

	dev_dbg(&pdev->dev, "start\n");

	/*
	 * Start the USB clocks.
	 */
<<<<<<< HEAD
	at91_start_clock();
=======
	at91_start_clock(ohci_at91);
>>>>>>> v4.9.227

	/*
	 * The USB host controller must remain in reset.
	 */
	writel(0, &regs->control);
}

static void at91_stop_hc(struct platform_device *pdev)
{
	struct usb_hcd *hcd = platform_get_drvdata(pdev);
	struct ohci_regs __iomem *regs = hcd->regs;
<<<<<<< HEAD
=======
	struct ohci_at91_priv *ohci_at91 = hcd_to_ohci_at91_priv(hcd);
>>>>>>> v4.9.227

	dev_dbg(&pdev->dev, "stop\n");

	/*
	 * Put the USB host controller into reset.
	 */
	writel(0, &regs->control);

	/*
	 * Stop the USB clocks.
	 */
<<<<<<< HEAD
	at91_stop_clock();
=======
	at91_stop_clock(ohci_at91);
>>>>>>> v4.9.227
}


/*-------------------------------------------------------------------------*/

static void usb_hcd_at91_remove (struct usb_hcd *, struct platform_device *);

<<<<<<< HEAD
=======
static struct regmap *at91_dt_syscon_sfr(void)
{
	struct regmap *regmap;

	regmap = syscon_regmap_lookup_by_compatible("atmel,sama5d2-sfr");
	if (IS_ERR(regmap))
		regmap = NULL;

	return regmap;
}

>>>>>>> v4.9.227
/* configure so an HC device and id are always provided */
/* always called with process context; sleeping is OK */


/**
 * usb_hcd_at91_probe - initialize AT91-based HCDs
 * Context: !in_interrupt()
 *
 * Allocates basic resources for this USB host controller, and
 * then invokes the start() method for the HCD associated with it
 * through the hotplug entry's driver_data.
 */
static int usb_hcd_at91_probe(const struct hc_driver *driver,
			struct platform_device *pdev)
{
	struct at91_usbh_data *board;
	struct ohci_hcd *ohci;
	int retval;
<<<<<<< HEAD
	struct usb_hcd *hcd = NULL;
=======
	struct usb_hcd *hcd;
	struct ohci_at91_priv *ohci_at91;
>>>>>>> v4.9.227
	struct device *dev = &pdev->dev;
	struct resource *res;
	int irq;

<<<<<<< HEAD
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_dbg(dev, "hcd probe: missing memory resource\n");
		return -ENXIO;
	}

=======
>>>>>>> v4.9.227
	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_dbg(dev, "hcd probe: missing irq resource\n");
		return irq;
	}

	hcd = usb_create_hcd(driver, dev, "at91");
	if (!hcd)
		return -ENOMEM;
<<<<<<< HEAD
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);

=======
	ohci_at91 = hcd_to_ohci_at91_priv(hcd);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
>>>>>>> v4.9.227
	hcd->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(hcd->regs)) {
		retval = PTR_ERR(hcd->regs);
		goto err;
	}
<<<<<<< HEAD

	iclk = devm_clk_get(dev, "ohci_clk");
	if (IS_ERR(iclk)) {
		dev_err(dev, "failed to get ohci_clk\n");
		retval = PTR_ERR(iclk);
		goto err;
	}
	fclk = devm_clk_get(dev, "uhpck");
	if (IS_ERR(fclk)) {
		dev_err(dev, "failed to get uhpck\n");
		retval = PTR_ERR(fclk);
		goto err;
	}
	hclk = devm_clk_get(dev, "hclk");
	if (IS_ERR(hclk)) {
		dev_err(dev, "failed to get hclk\n");
		retval = PTR_ERR(hclk);
		goto err;
	}
	if (IS_ENABLED(CONFIG_COMMON_CLK)) {
		uclk = devm_clk_get(dev, "usb_clk");
		if (IS_ERR(uclk)) {
			dev_err(dev, "failed to get uclk\n");
			retval = PTR_ERR(uclk);
			goto err;
		}
	}
=======
	hcd->rsrc_start = res->start;
	hcd->rsrc_len = resource_size(res);

	ohci_at91->iclk = devm_clk_get(dev, "ohci_clk");
	if (IS_ERR(ohci_at91->iclk)) {
		dev_err(dev, "failed to get ohci_clk\n");
		retval = PTR_ERR(ohci_at91->iclk);
		goto err;
	}
	ohci_at91->fclk = devm_clk_get(dev, "uhpck");
	if (IS_ERR(ohci_at91->fclk)) {
		dev_err(dev, "failed to get uhpck\n");
		retval = PTR_ERR(ohci_at91->fclk);
		goto err;
	}
	ohci_at91->hclk = devm_clk_get(dev, "hclk");
	if (IS_ERR(ohci_at91->hclk)) {
		dev_err(dev, "failed to get hclk\n");
		retval = PTR_ERR(ohci_at91->hclk);
		goto err;
	}

	ohci_at91->sfr_regmap = at91_dt_syscon_sfr();
	if (!ohci_at91->sfr_regmap)
		dev_warn(dev, "failed to find sfr node\n");
>>>>>>> v4.9.227

	board = hcd->self.controller->platform_data;
	ohci = hcd_to_ohci(hcd);
	ohci->num_ports = board->ports;
	at91_start_hc(pdev);

<<<<<<< HEAD
=======
	/*
	 * The RemoteWakeupConnected bit has to be set explicitly
	 * before calling ohci_run. The reset value of this bit is 0.
	 */
	ohci->hc_control = OHCI_CTRL_RWC;

>>>>>>> v4.9.227
	retval = usb_add_hcd(hcd, irq, IRQF_SHARED);
	if (retval == 0) {
		device_wakeup_enable(hcd->self.controller);
		return retval;
	}

	/* Error handling */
	at91_stop_hc(pdev);

 err:
	usb_put_hcd(hcd);
	return retval;
}


/* may be called with controller, bus, and devices active */

/**
 * usb_hcd_at91_remove - shutdown processing for AT91-based HCDs
 * @dev: USB Host Controller being removed
 * Context: !in_interrupt()
 *
 * Reverses the effect of usb_hcd_at91_probe(), first invoking
 * the HCD's stop() method.  It is always called from a thread
 * context, "rmmod" or something similar.
 *
 */
static void usb_hcd_at91_remove(struct usb_hcd *hcd,
				struct platform_device *pdev)
{
	usb_remove_hcd(hcd);
	at91_stop_hc(pdev);
	usb_put_hcd(hcd);
}

/*-------------------------------------------------------------------------*/
static void ohci_at91_usb_set_power(struct at91_usbh_data *pdata, int port, int enable)
{
	if (!valid_port(port))
		return;

	if (!gpio_is_valid(pdata->vbus_pin[port]))
		return;

	gpio_set_value(pdata->vbus_pin[port],
		       pdata->vbus_pin_active_low[port] ^ enable);
}

static int ohci_at91_usb_get_power(struct at91_usbh_data *pdata, int port)
{
	if (!valid_port(port))
		return -EINVAL;

	if (!gpio_is_valid(pdata->vbus_pin[port]))
		return -EINVAL;

	return gpio_get_value(pdata->vbus_pin[port]) ^
		pdata->vbus_pin_active_low[port];
}

/*
 * Update the status data from the hub with the over-current indicator change.
 */
static int ohci_at91_hub_status_data(struct usb_hcd *hcd, char *buf)
{
	struct at91_usbh_data *pdata = hcd->self.controller->platform_data;
	int length = ohci_hub_status_data(hcd, buf);
	int port;

	at91_for_each_port(port) {
		if (pdata->overcurrent_changed[port]) {
			if (!length)
				length = 1;
			buf[0] |= 1 << (port + 1);
		}
	}

	return length;
}

<<<<<<< HEAD
=======
static int ohci_at91_port_suspend(struct regmap *regmap, u8 set)
{
	u32 regval;
	int ret;

	if (!regmap)
		return 0;

	ret = regmap_read(regmap, AT91_SFR_OHCIICR, &regval);
	if (ret)
		return ret;

	if (set)
		regval |= AT91_OHCIICR_USB_SUSPEND;
	else
		regval &= ~AT91_OHCIICR_USB_SUSPEND;

	regmap_write(regmap, AT91_SFR_OHCIICR, regval);

	return 0;
}

>>>>>>> v4.9.227
/*
 * Look at the control requests to the root hub and see if we need to override.
 */
static int ohci_at91_hub_control(struct usb_hcd *hcd, u16 typeReq, u16 wValue,
				 u16 wIndex, char *buf, u16 wLength)
{
	struct at91_usbh_data *pdata = dev_get_platdata(hcd->self.controller);
<<<<<<< HEAD
=======
	struct ohci_at91_priv *ohci_at91 = hcd_to_ohci_at91_priv(hcd);
>>>>>>> v4.9.227
	struct usb_hub_descriptor *desc;
	int ret = -EINVAL;
	u32 *data = (u32 *)buf;

	dev_dbg(hcd->self.controller,
		"ohci_at91_hub_control(%p,0x%04x,0x%04x,0x%04x,%p,%04x)\n",
		hcd, typeReq, wValue, wIndex, buf, wLength);

	wIndex--;

	switch (typeReq) {
	case SetPortFeature:
<<<<<<< HEAD
		if (wValue == USB_PORT_FEAT_POWER) {
=======
		switch (wValue) {
		case USB_PORT_FEAT_POWER:
>>>>>>> v4.9.227
			dev_dbg(hcd->self.controller, "SetPortFeat: POWER\n");
			if (valid_port(wIndex)) {
				ohci_at91_usb_set_power(pdata, wIndex, 1);
				ret = 0;
			}

			goto out;
<<<<<<< HEAD
=======

		case USB_PORT_FEAT_SUSPEND:
			dev_dbg(hcd->self.controller, "SetPortFeat: SUSPEND\n");
			if (valid_port(wIndex) && ohci_at91->sfr_regmap) {
				ohci_at91_port_suspend(ohci_at91->sfr_regmap,
						       1);
				return 0;
			}
			break;
>>>>>>> v4.9.227
		}
		break;

	case ClearPortFeature:
		switch (wValue) {
		case USB_PORT_FEAT_C_OVER_CURRENT:
			dev_dbg(hcd->self.controller,
				"ClearPortFeature: C_OVER_CURRENT\n");

			if (valid_port(wIndex)) {
				pdata->overcurrent_changed[wIndex] = 0;
				pdata->overcurrent_status[wIndex] = 0;
			}

			goto out;

		case USB_PORT_FEAT_OVER_CURRENT:
			dev_dbg(hcd->self.controller,
				"ClearPortFeature: OVER_CURRENT\n");

			if (valid_port(wIndex))
				pdata->overcurrent_status[wIndex] = 0;

			goto out;

		case USB_PORT_FEAT_POWER:
			dev_dbg(hcd->self.controller,
				"ClearPortFeature: POWER\n");

			if (valid_port(wIndex)) {
				ohci_at91_usb_set_power(pdata, wIndex, 0);
				return 0;
			}
<<<<<<< HEAD
=======
			break;

		case USB_PORT_FEAT_SUSPEND:
			dev_dbg(hcd->self.controller, "ClearPortFeature: SUSPEND\n");
			if (valid_port(wIndex) && ohci_at91->sfr_regmap) {
				ohci_at91_port_suspend(ohci_at91->sfr_regmap,
						       0);
				return 0;
			}
			break;
>>>>>>> v4.9.227
		}
		break;
	}

	ret = ohci_hub_control(hcd, typeReq, wValue, wIndex + 1, buf, wLength);
	if (ret)
		goto out;

	switch (typeReq) {
	case GetHubDescriptor:

		/* update the hub's descriptor */

		desc = (struct usb_hub_descriptor *)buf;

		dev_dbg(hcd->self.controller, "wHubCharacteristics 0x%04x\n",
			desc->wHubCharacteristics);

		/* remove the old configurations for power-switching, and
		 * over-current protection, and insert our new configuration
		 */

		desc->wHubCharacteristics &= ~cpu_to_le16(HUB_CHAR_LPSM);
<<<<<<< HEAD
		desc->wHubCharacteristics |= cpu_to_le16(0x0001);

		if (pdata->overcurrent_supported) {
			desc->wHubCharacteristics &= ~cpu_to_le16(HUB_CHAR_OCPM);
			desc->wHubCharacteristics |=  cpu_to_le16(0x0008|0x0001);
=======
		desc->wHubCharacteristics |=
			cpu_to_le16(HUB_CHAR_INDV_PORT_LPSM);

		if (pdata->overcurrent_supported) {
			desc->wHubCharacteristics &= ~cpu_to_le16(HUB_CHAR_OCPM);
			desc->wHubCharacteristics |=
				cpu_to_le16(HUB_CHAR_INDV_PORT_OCPM);
>>>>>>> v4.9.227
		}

		dev_dbg(hcd->self.controller, "wHubCharacteristics after 0x%04x\n",
			desc->wHubCharacteristics);

		return ret;

	case GetPortStatus:
		/* check port status */

		dev_dbg(hcd->self.controller, "GetPortStatus(%d)\n", wIndex);

		if (valid_port(wIndex)) {
			if (!ohci_at91_usb_get_power(pdata, wIndex))
				*data &= ~cpu_to_le32(RH_PS_PPS);

			if (pdata->overcurrent_changed[wIndex])
				*data |= cpu_to_le32(RH_PS_OCIC);

			if (pdata->overcurrent_status[wIndex])
				*data |= cpu_to_le32(RH_PS_POCI);
		}
	}

 out:
	return ret;
}

/*-------------------------------------------------------------------------*/

static irqreturn_t ohci_hcd_at91_overcurrent_irq(int irq, void *data)
{
	struct platform_device *pdev = data;
	struct at91_usbh_data *pdata = dev_get_platdata(&pdev->dev);
	int val, gpio, port;

	/* From the GPIO notifying the over-current situation, find
	 * out the corresponding port */
	at91_for_each_port(port) {
		if (gpio_is_valid(pdata->overcurrent_pin[port]) &&
				gpio_to_irq(pdata->overcurrent_pin[port]) == irq) {
			gpio = pdata->overcurrent_pin[port];
			break;
		}
	}

	if (port == AT91_MAX_USBH_PORTS) {
		dev_err(& pdev->dev, "overcurrent interrupt from unknown GPIO\n");
		return IRQ_HANDLED;
	}

	val = gpio_get_value(gpio);

	/* When notified of an over-current situation, disable power
	   on the corresponding port, and mark this port in
	   over-current. */
	if (!val) {
		ohci_at91_usb_set_power(pdata, port, 0);
		pdata->overcurrent_status[port]  = 1;
		pdata->overcurrent_changed[port] = 1;
	}

	dev_dbg(& pdev->dev, "overcurrent situation %s\n",
		val ? "exited" : "notified");

	return IRQ_HANDLED;
}

<<<<<<< HEAD
#ifdef CONFIG_OF
=======
>>>>>>> v4.9.227
static const struct of_device_id at91_ohci_dt_ids[] = {
	{ .compatible = "atmel,at91rm9200-ohci" },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, at91_ohci_dt_ids);

<<<<<<< HEAD
static int ohci_at91_of_init(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	int i, gpio, ret;
	enum of_gpio_flags flags;
	struct at91_usbh_data	*pdata;
	u32 ports;

	if (!np)
		return 0;
=======
/*-------------------------------------------------------------------------*/

static int ohci_hcd_at91_drv_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct at91_usbh_data	*pdata;
	int			i;
	int			gpio;
	int			ret;
	enum of_gpio_flags	flags;
	u32			ports;
>>>>>>> v4.9.227

	/* Right now device-tree probed devices don't get dma_mask set.
	 * Since shared usb code relies on it, set it here for now.
	 * Once we have dma capability bindings this can go away.
	 */
	ret = dma_coerce_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));
	if (ret)
		return ret;

	pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;

<<<<<<< HEAD
=======
	pdev->dev.platform_data = pdata;

>>>>>>> v4.9.227
	if (!of_property_read_u32(np, "num-ports", &ports))
		pdata->ports = ports;

	at91_for_each_port(i) {
<<<<<<< HEAD
		gpio = of_get_named_gpio_flags(np, "atmel,vbus-gpio", i, &flags);
=======
		/*
		 * do not configure PIO if not in relation with
		 * real USB port on board
		 */
		if (i >= pdata->ports) {
			pdata->vbus_pin[i] = -EINVAL;
			pdata->overcurrent_pin[i] = -EINVAL;
			continue;
		}

		gpio = of_get_named_gpio_flags(np, "atmel,vbus-gpio", i,
					       &flags);
>>>>>>> v4.9.227
		pdata->vbus_pin[i] = gpio;
		if (!gpio_is_valid(gpio))
			continue;
		pdata->vbus_pin_active_low[i] = flags & OF_GPIO_ACTIVE_LOW;
<<<<<<< HEAD
	}

	at91_for_each_port(i)
		pdata->overcurrent_pin[i] =
			of_get_named_gpio_flags(np, "atmel,oc-gpio", i, &flags);

	pdev->dev.platform_data = pdata;

	return 0;
}
#else
static int ohci_at91_of_init(struct platform_device *pdev)
{
	return 0;
}
#endif

/*-------------------------------------------------------------------------*/

static int ohci_hcd_at91_drv_probe(struct platform_device *pdev)
{
	struct at91_usbh_data	*pdata;
	int			i;
	int			gpio;
	int			ret;

	ret = ohci_at91_of_init(pdev);
	if (ret)
		return ret;

	pdata = dev_get_platdata(&pdev->dev);

	if (pdata) {
		at91_for_each_port(i) {
			/*
			 * do not configure PIO if not in relation with
			 * real USB port on board
			 */
			if (i >= pdata->ports) {
				pdata->vbus_pin[i] = -EINVAL;
				pdata->overcurrent_pin[i] = -EINVAL;
				break;
			}

			if (!gpio_is_valid(pdata->vbus_pin[i]))
				continue;
			gpio = pdata->vbus_pin[i];

			ret = gpio_request(gpio, "ohci_vbus");
			if (ret) {
				dev_err(&pdev->dev,
					"can't request vbus gpio %d\n", gpio);
				continue;
			}
			ret = gpio_direction_output(gpio,
						!pdata->vbus_pin_active_low[i]);
			if (ret) {
				dev_err(&pdev->dev,
					"can't put vbus gpio %d as output %d\n",
					gpio, !pdata->vbus_pin_active_low[i]);
				gpio_free(gpio);
				continue;
			}

			ohci_at91_usb_set_power(pdata, i, 1);
		}

		at91_for_each_port(i) {
			if (!gpio_is_valid(pdata->overcurrent_pin[i]))
				continue;
			gpio = pdata->overcurrent_pin[i];

			ret = gpio_request(gpio, "ohci_overcurrent");
			if (ret) {
				dev_err(&pdev->dev,
					"can't request overcurrent gpio %d\n",
					gpio);
				continue;
			}

			ret = gpio_direction_input(gpio);
			if (ret) {
				dev_err(&pdev->dev,
					"can't configure overcurrent gpio %d as input\n",
					gpio);
				gpio_free(gpio);
				continue;
			}

			ret = request_irq(gpio_to_irq(gpio),
					  ohci_hcd_at91_overcurrent_irq,
					  IRQF_SHARED, "ohci_overcurrent", pdev);
			if (ret) {
				gpio_free(gpio);
				dev_err(&pdev->dev,
					"can't get gpio IRQ for overcurrent\n");
			}
=======

		ret = gpio_request(gpio, "ohci_vbus");
		if (ret) {
			dev_err(&pdev->dev,
				"can't request vbus gpio %d\n", gpio);
			continue;
		}
		ret = gpio_direction_output(gpio,
					!pdata->vbus_pin_active_low[i]);
		if (ret) {
			dev_err(&pdev->dev,
				"can't put vbus gpio %d as output %d\n",
				gpio, !pdata->vbus_pin_active_low[i]);
			gpio_free(gpio);
			continue;
		}

		ohci_at91_usb_set_power(pdata, i, 1);
	}

	at91_for_each_port(i) {
		if (i >= pdata->ports)
			break;

		pdata->overcurrent_pin[i] =
			of_get_named_gpio_flags(np, "atmel,oc-gpio", i, &flags);

		if (!gpio_is_valid(pdata->overcurrent_pin[i]))
			continue;
		gpio = pdata->overcurrent_pin[i];

		ret = gpio_request(gpio, "ohci_overcurrent");
		if (ret) {
			dev_err(&pdev->dev,
				"can't request overcurrent gpio %d\n",
				gpio);
			continue;
		}

		ret = gpio_direction_input(gpio);
		if (ret) {
			dev_err(&pdev->dev,
				"can't configure overcurrent gpio %d as input\n",
				gpio);
			gpio_free(gpio);
			continue;
		}

		ret = request_irq(gpio_to_irq(gpio),
				  ohci_hcd_at91_overcurrent_irq,
				  IRQF_SHARED, "ohci_overcurrent", pdev);
		if (ret) {
			gpio_free(gpio);
			dev_err(&pdev->dev,
				"can't get gpio IRQ for overcurrent\n");
>>>>>>> v4.9.227
		}
	}

	device_init_wakeup(&pdev->dev, 1);
	return usb_hcd_at91_probe(&ohci_at91_hc_driver, pdev);
}

static int ohci_hcd_at91_drv_remove(struct platform_device *pdev)
{
	struct at91_usbh_data	*pdata = dev_get_platdata(&pdev->dev);
	int			i;

	if (pdata) {
		at91_for_each_port(i) {
			if (!gpio_is_valid(pdata->vbus_pin[i]))
				continue;
			ohci_at91_usb_set_power(pdata, i, 0);
			gpio_free(pdata->vbus_pin[i]);
		}

		at91_for_each_port(i) {
			if (!gpio_is_valid(pdata->overcurrent_pin[i]))
				continue;
			free_irq(gpio_to_irq(pdata->overcurrent_pin[i]), pdev);
			gpio_free(pdata->overcurrent_pin[i]);
		}
	}

	device_init_wakeup(&pdev->dev, 0);
	usb_hcd_at91_remove(platform_get_drvdata(pdev), pdev);
	return 0;
}

<<<<<<< HEAD
#ifdef CONFIG_PM

static int
ohci_hcd_at91_drv_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	struct usb_hcd	*hcd = platform_get_drvdata(pdev);
	struct ohci_hcd	*ohci = hcd_to_ohci(hcd);
	bool		do_wakeup = device_may_wakeup(&pdev->dev);
	int		ret;

	if (do_wakeup)
		enable_irq_wake(hcd->irq);

	ret = ohci_suspend(hcd, do_wakeup);
	if (ret) {
		disable_irq_wake(hcd->irq);
=======
static int __maybe_unused
ohci_hcd_at91_drv_suspend(struct device *dev)
{
	struct usb_hcd	*hcd = dev_get_drvdata(dev);
	struct ohci_hcd	*ohci = hcd_to_ohci(hcd);
	struct ohci_at91_priv *ohci_at91 = hcd_to_ohci_at91_priv(hcd);
	int		ret;

	/*
	 * Disable wakeup if we are going to sleep with slow clock mode
	 * enabled.
	 */
	ohci_at91->wakeup = device_may_wakeup(dev)
			&& !at91_suspend_entering_slow_clock();

	if (ohci_at91->wakeup)
		enable_irq_wake(hcd->irq);

	ohci_at91_port_suspend(ohci_at91->sfr_regmap, 1);

	ret = ohci_suspend(hcd, ohci_at91->wakeup);
	if (ret) {
		if (ohci_at91->wakeup)
			disable_irq_wake(hcd->irq);
>>>>>>> v4.9.227
		return ret;
	}
	/*
	 * The integrated transceivers seem unable to notice disconnect,
	 * reconnect, or wakeup without the 48 MHz clock active.  so for
	 * correctness, always discard connection state (using reset).
	 *
	 * REVISIT: some boards will be able to turn VBUS off...
	 */
<<<<<<< HEAD
	if (at91_suspend_entering_slow_clock()) {
		ohci->hc_control = ohci_readl(ohci, &ohci->regs->control);
		ohci->hc_control &= OHCI_CTRL_RWC;
		ohci_writel(ohci, ohci->hc_control, &ohci->regs->control);
=======
	if (!ohci_at91->wakeup) {
>>>>>>> v4.9.227
		ohci->rh_state = OHCI_RH_HALTED;

		/* flush the writes */
		(void) ohci_readl (ohci, &ohci->regs->control);
<<<<<<< HEAD
		at91_stop_clock();
=======
		at91_stop_clock(ohci_at91);
>>>>>>> v4.9.227
	}

	return ret;
}

<<<<<<< HEAD
static int ohci_hcd_at91_drv_resume(struct platform_device *pdev)
{
	struct usb_hcd	*hcd = platform_get_drvdata(pdev);

	if (device_may_wakeup(&pdev->dev))
		disable_irq_wake(hcd->irq);

	if (!clocked)
		at91_start_clock();

	ohci_resume(hcd, false);
	return 0;
}
#else
#define ohci_hcd_at91_drv_suspend NULL
#define ohci_hcd_at91_drv_resume  NULL
#endif
=======
static int __maybe_unused
ohci_hcd_at91_drv_resume(struct device *dev)
{
	struct usb_hcd	*hcd = dev_get_drvdata(dev);
	struct ohci_at91_priv *ohci_at91 = hcd_to_ohci_at91_priv(hcd);

	if (ohci_at91->wakeup)
		disable_irq_wake(hcd->irq);

	at91_start_clock(ohci_at91);

	ohci_resume(hcd, false);

	ohci_at91_port_suspend(ohci_at91->sfr_regmap, 0);

	return 0;
}

static SIMPLE_DEV_PM_OPS(ohci_hcd_at91_pm_ops, ohci_hcd_at91_drv_suspend,
					ohci_hcd_at91_drv_resume);
>>>>>>> v4.9.227

static struct platform_driver ohci_hcd_at91_driver = {
	.probe		= ohci_hcd_at91_drv_probe,
	.remove		= ohci_hcd_at91_drv_remove,
	.shutdown	= usb_hcd_platform_shutdown,
<<<<<<< HEAD
	.suspend	= ohci_hcd_at91_drv_suspend,
	.resume		= ohci_hcd_at91_drv_resume,
	.driver		= {
		.name	= "at91_ohci",
		.owner	= THIS_MODULE,
		.of_match_table	= of_match_ptr(at91_ohci_dt_ids),
=======
	.driver		= {
		.name	= "at91_ohci",
		.pm	= &ohci_hcd_at91_pm_ops,
		.of_match_table	= at91_ohci_dt_ids,
>>>>>>> v4.9.227
	},
};

static int __init ohci_at91_init(void)
{
	if (usb_disabled())
		return -ENODEV;

	pr_info("%s: " DRIVER_DESC "\n", hcd_name);
<<<<<<< HEAD
	ohci_init_driver(&ohci_at91_hc_driver, NULL);
=======
	ohci_init_driver(&ohci_at91_hc_driver, &ohci_at91_drv_overrides);
>>>>>>> v4.9.227

	/*
	 * The Atmel HW has some unusual quirks, which require Atmel-specific
	 * workarounds. We override certain hc_driver functions here to
	 * achieve that. We explicitly do not enhance ohci_driver_overrides to
	 * allow this more easily, since this is an unusual case, and we don't
	 * want to encourage others to override these functions by making it
	 * too easy.
	 */

	ohci_at91_hc_driver.hub_status_data	= ohci_at91_hub_status_data;
	ohci_at91_hc_driver.hub_control		= ohci_at91_hub_control;

	return platform_driver_register(&ohci_hcd_at91_driver);
}
module_init(ohci_at91_init);

static void __exit ohci_at91_cleanup(void)
{
	platform_driver_unregister(&ohci_hcd_at91_driver);
}
module_exit(ohci_at91_cleanup);

MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:at91_ohci");
