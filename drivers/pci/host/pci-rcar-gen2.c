/*
 *  pci-rcar-gen2: internal PCI bus support
 *
 * Copyright (C) 2013 Renesas Solutions Corp.
 * Copyright (C) 2013 Cogent Embedded, Inc.
 *
<<<<<<< HEAD
=======
 * Author: Valentine Barshak <valentine.barshak@cogentembedded.com>
 *
>>>>>>> v4.9.227
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
<<<<<<< HEAD
#include <linux/module.h>
=======
#include <linux/of_address.h>
>>>>>>> v4.9.227
#include <linux/of_pci.h>
#include <linux/pci.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/sizes.h>
#include <linux/slab.h>

/* AHB-PCI Bridge PCI communication registers */
#define RCAR_AHBPCI_PCICOM_OFFSET	0x800

#define RCAR_PCIAHB_WIN1_CTR_REG	(RCAR_AHBPCI_PCICOM_OFFSET + 0x00)
#define RCAR_PCIAHB_WIN2_CTR_REG	(RCAR_AHBPCI_PCICOM_OFFSET + 0x04)
#define RCAR_PCIAHB_PREFETCH0		0x0
#define RCAR_PCIAHB_PREFETCH4		0x1
#define RCAR_PCIAHB_PREFETCH8		0x2
#define RCAR_PCIAHB_PREFETCH16		0x3

#define RCAR_AHBPCI_WIN1_CTR_REG	(RCAR_AHBPCI_PCICOM_OFFSET + 0x10)
#define RCAR_AHBPCI_WIN2_CTR_REG	(RCAR_AHBPCI_PCICOM_OFFSET + 0x14)
#define RCAR_AHBPCI_WIN_CTR_MEM		(3 << 1)
#define RCAR_AHBPCI_WIN_CTR_CFG		(5 << 1)
#define RCAR_AHBPCI_WIN1_HOST		(1 << 30)
#define RCAR_AHBPCI_WIN1_DEVICE		(1 << 31)

#define RCAR_PCI_INT_ENABLE_REG		(RCAR_AHBPCI_PCICOM_OFFSET + 0x20)
#define RCAR_PCI_INT_STATUS_REG		(RCAR_AHBPCI_PCICOM_OFFSET + 0x24)
#define RCAR_PCI_INT_SIGTABORT		(1 << 0)
#define RCAR_PCI_INT_SIGRETABORT	(1 << 1)
#define RCAR_PCI_INT_REMABORT		(1 << 2)
#define RCAR_PCI_INT_PERR		(1 << 3)
#define RCAR_PCI_INT_SIGSERR		(1 << 4)
#define RCAR_PCI_INT_RESERR		(1 << 5)
#define RCAR_PCI_INT_WIN1ERR		(1 << 12)
#define RCAR_PCI_INT_WIN2ERR		(1 << 13)
#define RCAR_PCI_INT_A			(1 << 16)
#define RCAR_PCI_INT_B			(1 << 17)
#define RCAR_PCI_INT_PME		(1 << 19)
#define RCAR_PCI_INT_ALLERRORS (RCAR_PCI_INT_SIGTABORT		| \
				RCAR_PCI_INT_SIGRETABORT	| \
				RCAR_PCI_INT_SIGRETABORT	| \
				RCAR_PCI_INT_REMABORT		| \
				RCAR_PCI_INT_PERR		| \
				RCAR_PCI_INT_SIGSERR		| \
				RCAR_PCI_INT_RESERR		| \
				RCAR_PCI_INT_WIN1ERR		| \
				RCAR_PCI_INT_WIN2ERR)

#define RCAR_AHB_BUS_CTR_REG		(RCAR_AHBPCI_PCICOM_OFFSET + 0x30)
#define RCAR_AHB_BUS_MMODE_HTRANS	(1 << 0)
#define RCAR_AHB_BUS_MMODE_BYTE_BURST	(1 << 1)
#define RCAR_AHB_BUS_MMODE_WR_INCR	(1 << 2)
#define RCAR_AHB_BUS_MMODE_HBUS_REQ	(1 << 7)
#define RCAR_AHB_BUS_SMODE_READYCTR	(1 << 17)
#define RCAR_AHB_BUS_MODE		(RCAR_AHB_BUS_MMODE_HTRANS |	\
					RCAR_AHB_BUS_MMODE_BYTE_BURST |	\
					RCAR_AHB_BUS_MMODE_WR_INCR |	\
					RCAR_AHB_BUS_MMODE_HBUS_REQ |	\
					RCAR_AHB_BUS_SMODE_READYCTR)

#define RCAR_USBCTR_REG			(RCAR_AHBPCI_PCICOM_OFFSET + 0x34)
#define RCAR_USBCTR_USBH_RST		(1 << 0)
#define RCAR_USBCTR_PCICLK_MASK		(1 << 1)
#define RCAR_USBCTR_PLL_RST		(1 << 2)
#define RCAR_USBCTR_DIRPD		(1 << 8)
#define RCAR_USBCTR_PCIAHB_WIN2_EN	(1 << 9)
#define RCAR_USBCTR_PCIAHB_WIN1_256M	(0 << 10)
#define RCAR_USBCTR_PCIAHB_WIN1_512M	(1 << 10)
#define RCAR_USBCTR_PCIAHB_WIN1_1G	(2 << 10)
#define RCAR_USBCTR_PCIAHB_WIN1_2G	(3 << 10)
#define RCAR_USBCTR_PCIAHB_WIN1_MASK	(3 << 10)

#define RCAR_PCI_ARBITER_CTR_REG	(RCAR_AHBPCI_PCICOM_OFFSET + 0x40)
#define RCAR_PCI_ARBITER_PCIREQ0	(1 << 0)
#define RCAR_PCI_ARBITER_PCIREQ1	(1 << 1)
#define RCAR_PCI_ARBITER_PCIBP_MODE	(1 << 12)

#define RCAR_PCI_UNIT_REV_REG		(RCAR_AHBPCI_PCICOM_OFFSET + 0x48)

struct rcar_pci_priv {
	struct device *dev;
	void __iomem *reg;
<<<<<<< HEAD
	struct resource io_res;
=======
>>>>>>> v4.9.227
	struct resource mem_res;
	struct resource *cfg_res;
	unsigned busnr;
	int irq;
	unsigned long window_size;
<<<<<<< HEAD
=======
	unsigned long window_addr;
	unsigned long window_pci;
>>>>>>> v4.9.227
};

/* PCI configuration space operations */
static void __iomem *rcar_pci_cfg_base(struct pci_bus *bus, unsigned int devfn,
				       int where)
{
	struct pci_sys_data *sys = bus->sysdata;
	struct rcar_pci_priv *priv = sys->private_data;
	int slot, val;

	if (sys->busnr != bus->number || PCI_FUNC(devfn))
		return NULL;

	/* Only one EHCI/OHCI device built-in */
	slot = PCI_SLOT(devfn);
	if (slot > 2)
		return NULL;

	/* bridge logic only has registers to 0x40 */
	if (slot == 0x0 && where >= 0x40)
		return NULL;

	val = slot ? RCAR_AHBPCI_WIN1_DEVICE | RCAR_AHBPCI_WIN_CTR_CFG :
		     RCAR_AHBPCI_WIN1_HOST | RCAR_AHBPCI_WIN_CTR_CFG;

	iowrite32(val, priv->reg + RCAR_AHBPCI_WIN1_CTR_REG);
	return priv->reg + (slot >> 1) * 0x100 + where;
}

<<<<<<< HEAD
static int rcar_pci_read_config(struct pci_bus *bus, unsigned int devfn,
				int where, int size, u32 *val)
{
	void __iomem *reg = rcar_pci_cfg_base(bus, devfn, where);

	if (!reg)
		return PCIBIOS_DEVICE_NOT_FOUND;

	switch (size) {
	case 1:
		*val = ioread8(reg);
		break;
	case 2:
		*val = ioread16(reg);
		break;
	default:
		*val = ioread32(reg);
		break;
	}

	return PCIBIOS_SUCCESSFUL;
}

static int rcar_pci_write_config(struct pci_bus *bus, unsigned int devfn,
				 int where, int size, u32 val)
{
	void __iomem *reg = rcar_pci_cfg_base(bus, devfn, where);

	if (!reg)
		return PCIBIOS_DEVICE_NOT_FOUND;

	switch (size) {
	case 1:
		iowrite8(val, reg);
		break;
	case 2:
		iowrite16(val, reg);
		break;
	default:
		iowrite32(val, reg);
		break;
	}

	return PCIBIOS_SUCCESSFUL;
}

=======
>>>>>>> v4.9.227
/* PCI interrupt mapping */
static int rcar_pci_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	struct pci_sys_data *sys = dev->bus->sysdata;
	struct rcar_pci_priv *priv = sys->private_data;
	int irq;

	irq = of_irq_parse_and_map_pci(dev, slot, pin);
	if (!irq)
		irq = priv->irq;

	return irq;
}

#ifdef CONFIG_PCI_DEBUG
/* if debug enabled, then attach an error handler irq to the bridge */

static irqreturn_t rcar_pci_err_irq(int irq, void *pw)
{
	struct rcar_pci_priv *priv = pw;
<<<<<<< HEAD
	u32 status = ioread32(priv->reg + RCAR_PCI_INT_STATUS_REG);

	if (status & RCAR_PCI_INT_ALLERRORS) {
		dev_err(priv->dev, "error irq: status %08x\n", status);
=======
	struct device *dev = priv->dev;
	u32 status = ioread32(priv->reg + RCAR_PCI_INT_STATUS_REG);

	if (status & RCAR_PCI_INT_ALLERRORS) {
		dev_err(dev, "error irq: status %08x\n", status);
>>>>>>> v4.9.227

		/* clear the error(s) */
		iowrite32(status & RCAR_PCI_INT_ALLERRORS,
			  priv->reg + RCAR_PCI_INT_STATUS_REG);
		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

static void rcar_pci_setup_errirq(struct rcar_pci_priv *priv)
{
<<<<<<< HEAD
	int ret;
	u32 val;

	ret = devm_request_irq(priv->dev, priv->irq, rcar_pci_err_irq,
			       IRQF_SHARED, "error irq", priv);
	if (ret) {
		dev_err(priv->dev, "cannot claim IRQ for error handling\n");
=======
	struct device *dev = priv->dev;
	int ret;
	u32 val;

	ret = devm_request_irq(dev, priv->irq, rcar_pci_err_irq,
			       IRQF_SHARED, "error irq", priv);
	if (ret) {
		dev_err(dev, "cannot claim IRQ for error handling\n");
>>>>>>> v4.9.227
		return;
	}

	val = ioread32(priv->reg + RCAR_PCI_INT_ENABLE_REG);
	val |= RCAR_PCI_INT_ALLERRORS;
	iowrite32(val, priv->reg + RCAR_PCI_INT_ENABLE_REG);
}
#else
static inline void rcar_pci_setup_errirq(struct rcar_pci_priv *priv) { }
#endif

/* PCI host controller setup */
static int rcar_pci_setup(int nr, struct pci_sys_data *sys)
{
	struct rcar_pci_priv *priv = sys->private_data;
<<<<<<< HEAD
	void __iomem *reg = priv->reg;
	u32 val;

	pm_runtime_enable(priv->dev);
	pm_runtime_get_sync(priv->dev);

	val = ioread32(reg + RCAR_PCI_UNIT_REV_REG);
	dev_info(priv->dev, "PCI: bus%u revision %x\n", sys->busnr, val);
=======
	struct device *dev = priv->dev;
	void __iomem *reg = priv->reg;
	u32 val;
	int ret;

	pm_runtime_enable(dev);
	pm_runtime_get_sync(dev);

	val = ioread32(reg + RCAR_PCI_UNIT_REV_REG);
	dev_info(dev, "PCI: bus%u revision %x\n", sys->busnr, val);
>>>>>>> v4.9.227

	/* Disable Direct Power Down State and assert reset */
	val = ioread32(reg + RCAR_USBCTR_REG) & ~RCAR_USBCTR_DIRPD;
	val |= RCAR_USBCTR_USBH_RST | RCAR_USBCTR_PLL_RST;
	iowrite32(val, reg + RCAR_USBCTR_REG);
	udelay(4);

	/* De-assert reset and reset PCIAHB window1 size */
	val &= ~(RCAR_USBCTR_PCIAHB_WIN1_MASK | RCAR_USBCTR_PCICLK_MASK |
		 RCAR_USBCTR_USBH_RST | RCAR_USBCTR_PLL_RST);

	/* Setup PCIAHB window1 size */
	switch (priv->window_size) {
	case SZ_2G:
		val |= RCAR_USBCTR_PCIAHB_WIN1_2G;
		break;
	case SZ_1G:
		val |= RCAR_USBCTR_PCIAHB_WIN1_1G;
		break;
	case SZ_512M:
		val |= RCAR_USBCTR_PCIAHB_WIN1_512M;
		break;
	default:
		pr_warn("unknown window size %ld - defaulting to 256M\n",
			priv->window_size);
		priv->window_size = SZ_256M;
		/* fall-through */
	case SZ_256M:
		val |= RCAR_USBCTR_PCIAHB_WIN1_256M;
		break;
	}
	iowrite32(val, reg + RCAR_USBCTR_REG);

	/* Configure AHB master and slave modes */
	iowrite32(RCAR_AHB_BUS_MODE, reg + RCAR_AHB_BUS_CTR_REG);

	/* Configure PCI arbiter */
	val = ioread32(reg + RCAR_PCI_ARBITER_CTR_REG);
	val |= RCAR_PCI_ARBITER_PCIREQ0 | RCAR_PCI_ARBITER_PCIREQ1 |
	       RCAR_PCI_ARBITER_PCIBP_MODE;
	iowrite32(val, reg + RCAR_PCI_ARBITER_CTR_REG);

<<<<<<< HEAD
	/* PCI-AHB mapping: 0x40000000 base */
	iowrite32(0x40000000 | RCAR_PCIAHB_PREFETCH16,
=======
	/* PCI-AHB mapping */
	iowrite32(priv->window_addr | RCAR_PCIAHB_PREFETCH16,
>>>>>>> v4.9.227
		  reg + RCAR_PCIAHB_WIN1_CTR_REG);

	/* AHB-PCI mapping: OHCI/EHCI registers */
	val = priv->mem_res.start | RCAR_AHBPCI_WIN_CTR_MEM;
	iowrite32(val, reg + RCAR_AHBPCI_WIN2_CTR_REG);

	/* Enable AHB-PCI bridge PCI configuration access */
	iowrite32(RCAR_AHBPCI_WIN1_HOST | RCAR_AHBPCI_WIN_CTR_CFG,
		  reg + RCAR_AHBPCI_WIN1_CTR_REG);
	/* Set PCI-AHB Window1 address */
<<<<<<< HEAD
	iowrite32(0x40000000 | PCI_BASE_ADDRESS_MEM_PREFETCH,
=======
	iowrite32(priv->window_pci | PCI_BASE_ADDRESS_MEM_PREFETCH,
>>>>>>> v4.9.227
		  reg + PCI_BASE_ADDRESS_1);
	/* Set AHB-PCI bridge PCI communication area address */
	val = priv->cfg_res->start + RCAR_AHBPCI_PCICOM_OFFSET;
	iowrite32(val, reg + PCI_BASE_ADDRESS_0);

	val = ioread32(reg + PCI_COMMAND);
	val |= PCI_COMMAND_SERR | PCI_COMMAND_PARITY |
	       PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER;
	iowrite32(val, reg + PCI_COMMAND);

	/* Enable PCI interrupts */
	iowrite32(RCAR_PCI_INT_A | RCAR_PCI_INT_B | RCAR_PCI_INT_PME,
		  reg + RCAR_PCI_INT_ENABLE_REG);

	if (priv->irq > 0)
		rcar_pci_setup_errirq(priv);

	/* Add PCI resources */
<<<<<<< HEAD
	pci_add_resource(&sys->resources, &priv->io_res);
	pci_add_resource(&sys->resources, &priv->mem_res);
=======
	pci_add_resource(&sys->resources, &priv->mem_res);
	ret = devm_request_pci_bus_resources(dev, &sys->resources);
	if (ret < 0)
		return ret;
>>>>>>> v4.9.227

	/* Setup bus number based on platform device id / of bus-range */
	sys->busnr = priv->busnr;
	return 1;
}

static struct pci_ops rcar_pci_ops = {
<<<<<<< HEAD
	.read	= rcar_pci_read_config,
	.write	= rcar_pci_write_config,
};

static int rcar_pci_probe(struct platform_device *pdev)
{
=======
	.map_bus = rcar_pci_cfg_base,
	.read	= pci_generic_config_read,
	.write	= pci_generic_config_write,
};

static int pci_dma_range_parser_init(struct of_pci_range_parser *parser,
				     struct device_node *node)
{
	const int na = 3, ns = 2;
	int rlen;

	parser->node = node;
	parser->pna = of_n_addr_cells(node);
	parser->np = parser->pna + na + ns;

	parser->range = of_get_property(node, "dma-ranges", &rlen);
	if (!parser->range)
		return -ENOENT;

	parser->end = parser->range + rlen / sizeof(__be32);
	return 0;
}

static int rcar_pci_parse_map_dma_ranges(struct rcar_pci_priv *pci,
					 struct device_node *np)
{
	struct device *dev = pci->dev;
	struct of_pci_range range;
	struct of_pci_range_parser parser;
	int index = 0;

	/* Failure to parse is ok as we fall back to defaults */
	if (pci_dma_range_parser_init(&parser, np))
		return 0;

	/* Get the dma-ranges from DT */
	for_each_of_pci_range(&parser, &range) {
		/* Hardware only allows one inbound 32-bit range */
		if (index)
			return -EINVAL;

		pci->window_addr = (unsigned long)range.cpu_addr;
		pci->window_pci = (unsigned long)range.pci_addr;
		pci->window_size = (unsigned long)range.size;

		/* Catch HW limitations */
		if (!(range.flags & IORESOURCE_PREFETCH)) {
			dev_err(dev, "window must be prefetchable\n");
			return -EINVAL;
		}
		if (pci->window_addr) {
			u32 lowaddr = 1 << (ffs(pci->window_addr) - 1);

			if (lowaddr < pci->window_size) {
				dev_err(dev, "invalid window size/addr\n");
				return -EINVAL;
			}
		}
		index++;
	}

	return 0;
}

static int rcar_pci_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
>>>>>>> v4.9.227
	struct resource *cfg_res, *mem_res;
	struct rcar_pci_priv *priv;
	void __iomem *reg;
	struct hw_pci hw;
	void *hw_private[1];

	cfg_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
<<<<<<< HEAD
	reg = devm_ioremap_resource(&pdev->dev, cfg_res);
=======
	reg = devm_ioremap_resource(dev, cfg_res);
>>>>>>> v4.9.227
	if (IS_ERR(reg))
		return PTR_ERR(reg);

	mem_res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!mem_res || !mem_res->start)
		return -ENODEV;

<<<<<<< HEAD
	priv = devm_kzalloc(&pdev->dev,
			    sizeof(struct rcar_pci_priv), GFP_KERNEL);
=======
	if (mem_res->start & 0xFFFF)
		return -EINVAL;

	priv = devm_kzalloc(dev, sizeof(struct rcar_pci_priv), GFP_KERNEL);
>>>>>>> v4.9.227
	if (!priv)
		return -ENOMEM;

	priv->mem_res = *mem_res;
<<<<<<< HEAD
	/*
	 * The controller does not support/use port I/O,
	 * so setup a dummy port I/O region here.
	 */
	priv->io_res.start = priv->mem_res.start;
	priv->io_res.end = priv->mem_res.end;
	priv->io_res.flags = IORESOURCE_IO;

=======
>>>>>>> v4.9.227
	priv->cfg_res = cfg_res;

	priv->irq = platform_get_irq(pdev, 0);
	priv->reg = reg;
<<<<<<< HEAD
	priv->dev = &pdev->dev;

	if (priv->irq < 0) {
		dev_err(&pdev->dev, "no valid irq found\n");
		return priv->irq;
	}

	priv->window_size = SZ_1G;

	if (pdev->dev.of_node) {
		struct resource busnr;
		int ret;

		ret = of_pci_parse_bus_range(pdev->dev.of_node, &busnr);
		if (ret < 0) {
			dev_err(&pdev->dev, "failed to parse bus-range\n");
=======
	priv->dev = dev;

	if (priv->irq < 0) {
		dev_err(dev, "no valid irq found\n");
		return priv->irq;
	}

	/* default window addr and size if not specified in DT */
	priv->window_addr = 0x40000000;
	priv->window_pci = 0x40000000;
	priv->window_size = SZ_1G;

	if (dev->of_node) {
		struct resource busnr;
		int ret;

		ret = of_pci_parse_bus_range(dev->of_node, &busnr);
		if (ret < 0) {
			dev_err(dev, "failed to parse bus-range\n");
>>>>>>> v4.9.227
			return ret;
		}

		priv->busnr = busnr.start;
		if (busnr.end != busnr.start)
<<<<<<< HEAD
			dev_warn(&pdev->dev, "only one bus number supported\n");
=======
			dev_warn(dev, "only one bus number supported\n");

		ret = rcar_pci_parse_map_dma_ranges(priv, dev->of_node);
		if (ret < 0) {
			dev_err(dev, "failed to parse dma-range\n");
			return ret;
		}
>>>>>>> v4.9.227
	} else {
		priv->busnr = pdev->id;
	}

	hw_private[0] = priv;
	memset(&hw, 0, sizeof(hw));
	hw.nr_controllers = ARRAY_SIZE(hw_private);
<<<<<<< HEAD
=======
	hw.io_optional = 1;
>>>>>>> v4.9.227
	hw.private_data = hw_private;
	hw.map_irq = rcar_pci_map_irq;
	hw.ops = &rcar_pci_ops;
	hw.setup = rcar_pci_setup;
<<<<<<< HEAD
	pci_common_init_dev(&pdev->dev, &hw);
=======
	pci_common_init_dev(dev, &hw);
>>>>>>> v4.9.227
	return 0;
}

static struct of_device_id rcar_pci_of_match[] = {
<<<<<<< HEAD
	{ .compatible = "renesas,pci-r8a7790", },
	{ .compatible = "renesas,pci-r8a7791", },
	{ },
};

MODULE_DEVICE_TABLE(of, rcar_pci_of_match);

=======
	{ .compatible = "renesas,pci-rcar-gen2", },
	{ .compatible = "renesas,pci-r8a7790", },
	{ .compatible = "renesas,pci-r8a7791", },
	{ .compatible = "renesas,pci-r8a7794", },
	{ },
};

>>>>>>> v4.9.227
static struct platform_driver rcar_pci_driver = {
	.driver = {
		.name = "pci-rcar-gen2",
		.suppress_bind_attrs = true,
		.of_match_table = rcar_pci_of_match,
	},
	.probe = rcar_pci_probe,
};
<<<<<<< HEAD

module_platform_driver(rcar_pci_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Renesas R-Car Gen2 internal PCI");
MODULE_AUTHOR("Valentine Barshak <valentine.barshak@cogentembedded.com>");
=======
builtin_platform_driver(rcar_pci_driver);
>>>>>>> v4.9.227
