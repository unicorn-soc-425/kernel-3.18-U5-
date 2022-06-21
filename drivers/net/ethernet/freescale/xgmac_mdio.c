/*
 * QorIQ 10G MDIO Controller
 *
 * Copyright 2012 Freescale Semiconductor, Inc.
 *
 * Authors: Andy Fleming <afleming@freescale.com>
 *          Timur Tabi <timur@freescale.com>
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/mdio.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/of_mdio.h>

/* Number of microseconds to wait for a register to respond */
#define TIMEOUT	1000

struct tgec_mdio_controller {
	__be32	reserved[12];
	__be32	mdio_stat;	/* MDIO configuration and status */
	__be32	mdio_ctl;	/* MDIO control */
	__be32	mdio_data;	/* MDIO data */
	__be32	mdio_addr;	/* MDIO address */
} __packed;

<<<<<<< HEAD
#define MDIO_STAT_CLKDIV(x)	(((x>>1) & 0xff) << 8)
#define MDIO_STAT_BSY		(1 << 0)
#define MDIO_STAT_RD_ER		(1 << 1)
#define MDIO_CTL_DEV_ADDR(x) 	(x & 0x1f)
#define MDIO_CTL_PORT_ADDR(x)	((x & 0x1f) << 5)
#define MDIO_CTL_PRE_DIS	(1 << 10)
#define MDIO_CTL_SCAN_EN	(1 << 11)
#define MDIO_CTL_POST_INC	(1 << 14)
#define MDIO_CTL_READ		(1 << 15)

#define MDIO_DATA(x)		(x & 0xffff)
#define MDIO_DATA_BSY		(1 << 31)
=======
#define MDIO_STAT_ENC		BIT(6)
#define MDIO_STAT_CLKDIV(x)	(((x>>1) & 0xff) << 8)
#define MDIO_STAT_BSY		BIT(0)
#define MDIO_STAT_RD_ER		BIT(1)
#define MDIO_CTL_DEV_ADDR(x) 	(x & 0x1f)
#define MDIO_CTL_PORT_ADDR(x)	((x & 0x1f) << 5)
#define MDIO_CTL_PRE_DIS	BIT(10)
#define MDIO_CTL_SCAN_EN	BIT(11)
#define MDIO_CTL_POST_INC	BIT(14)
#define MDIO_CTL_READ		BIT(15)

#define MDIO_DATA(x)		(x & 0xffff)
#define MDIO_DATA_BSY		BIT(31)

struct mdio_fsl_priv {
	struct	tgec_mdio_controller __iomem *mdio_base;
	bool	is_little_endian;
	bool	has_a011043;
};

static u32 xgmac_read32(void __iomem *regs,
			bool is_little_endian)
{
	if (is_little_endian)
		return ioread32(regs);
	else
		return ioread32be(regs);
}

static void xgmac_write32(u32 value,
			  void __iomem *regs,
			  bool is_little_endian)
{
	if (is_little_endian)
		iowrite32(value, regs);
	else
		iowrite32be(value, regs);
}
>>>>>>> v4.9.227

/*
 * Wait until the MDIO bus is free
 */
static int xgmac_wait_until_free(struct device *dev,
<<<<<<< HEAD
				 struct tgec_mdio_controller __iomem *regs)
{
	uint32_t status;

	/* Wait till the bus is free */
	status = spin_event_timeout(
		!((in_be32(&regs->mdio_stat)) & MDIO_STAT_BSY), TIMEOUT, 0);
	if (!status) {
=======
				 struct tgec_mdio_controller __iomem *regs,
				 bool is_little_endian)
{
	unsigned int timeout;

	/* Wait till the bus is free */
	timeout = TIMEOUT;
	while ((xgmac_read32(&regs->mdio_stat, is_little_endian) &
		MDIO_STAT_BSY) && timeout) {
		cpu_relax();
		timeout--;
	}

	if (!timeout) {
>>>>>>> v4.9.227
		dev_err(dev, "timeout waiting for bus to be free\n");
		return -ETIMEDOUT;
	}

	return 0;
}

/*
 * Wait till the MDIO read or write operation is complete
 */
static int xgmac_wait_until_done(struct device *dev,
<<<<<<< HEAD
				 struct tgec_mdio_controller __iomem *regs)
{
	uint32_t status;

	/* Wait till the MDIO write is complete */
	status = spin_event_timeout(
		!((in_be32(&regs->mdio_data)) & MDIO_DATA_BSY), TIMEOUT, 0);
	if (!status) {
=======
				 struct tgec_mdio_controller __iomem *regs,
				 bool is_little_endian)
{
	unsigned int timeout;

	/* Wait till the MDIO write is complete */
	timeout = TIMEOUT;
	while ((xgmac_read32(&regs->mdio_stat, is_little_endian) &
		MDIO_STAT_BSY) && timeout) {
		cpu_relax();
		timeout--;
	}

	if (!timeout) {
>>>>>>> v4.9.227
		dev_err(dev, "timeout waiting for operation to complete\n");
		return -ETIMEDOUT;
	}

	return 0;
}

/*
 * Write value to the PHY for this device to the register at regnum,waiting
 * until the write is done before it returns.  All PHY configuration has to be
 * done through the TSEC1 MIIM regs.
 */
static int xgmac_mdio_write(struct mii_bus *bus, int phy_id, int regnum, u16 value)
{
<<<<<<< HEAD
	struct tgec_mdio_controller __iomem *regs = bus->priv;
	uint16_t dev_addr = regnum >> 16;
	int ret;

	/* Setup the MII Mgmt clock speed */
	out_be32(&regs->mdio_stat, MDIO_STAT_CLKDIV(100));

	ret = xgmac_wait_until_free(&bus->dev, regs);
=======
	struct mdio_fsl_priv *priv = (struct mdio_fsl_priv *)bus->priv;
	struct tgec_mdio_controller __iomem *regs = priv->mdio_base;
	uint16_t dev_addr;
	u32 mdio_ctl, mdio_stat;
	int ret;
	bool endian = priv->is_little_endian;

	mdio_stat = xgmac_read32(&regs->mdio_stat, endian);
	if (regnum & MII_ADDR_C45) {
		/* Clause 45 (ie 10G) */
		dev_addr = (regnum >> 16) & 0x1f;
		mdio_stat |= MDIO_STAT_ENC;
	} else {
		/* Clause 22 (ie 1G) */
		dev_addr = regnum & 0x1f;
		mdio_stat &= ~MDIO_STAT_ENC;
	}

	xgmac_write32(mdio_stat, &regs->mdio_stat, endian);

	ret = xgmac_wait_until_free(&bus->dev, regs, endian);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* Set the port and dev addr */
<<<<<<< HEAD
	out_be32(&regs->mdio_ctl,
		 MDIO_CTL_PORT_ADDR(phy_id) | MDIO_CTL_DEV_ADDR(dev_addr));

	/* Set the register address */
	out_be32(&regs->mdio_addr, regnum & 0xffff);

	ret = xgmac_wait_until_free(&bus->dev, regs);
	if (ret)
		return ret;

	/* Write the value to the register */
	out_be32(&regs->mdio_data, MDIO_DATA(value));

	ret = xgmac_wait_until_done(&bus->dev, regs);
=======
	mdio_ctl = MDIO_CTL_PORT_ADDR(phy_id) | MDIO_CTL_DEV_ADDR(dev_addr);
	xgmac_write32(mdio_ctl, &regs->mdio_ctl, endian);

	/* Set the register address */
	if (regnum & MII_ADDR_C45) {
		xgmac_write32(regnum & 0xffff, &regs->mdio_addr, endian);

		ret = xgmac_wait_until_free(&bus->dev, regs, endian);
		if (ret)
			return ret;
	}

	/* Write the value to the register */
	xgmac_write32(MDIO_DATA(value), &regs->mdio_data, endian);

	ret = xgmac_wait_until_done(&bus->dev, regs, endian);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	return 0;
}

/*
 * Reads from register regnum in the PHY for device dev, returning the value.
 * Clears miimcom first.  All PHY configuration has to be done through the
 * TSEC1 MIIM regs.
 */
static int xgmac_mdio_read(struct mii_bus *bus, int phy_id, int regnum)
{
<<<<<<< HEAD
	struct tgec_mdio_controller __iomem *regs = bus->priv;
	uint16_t dev_addr = regnum >> 16;
	uint32_t mdio_ctl;
	uint16_t value;
	int ret;

	/* Setup the MII Mgmt clock speed */
	out_be32(&regs->mdio_stat, MDIO_STAT_CLKDIV(100));

	ret = xgmac_wait_until_free(&bus->dev, regs);
=======
	struct mdio_fsl_priv *priv = (struct mdio_fsl_priv *)bus->priv;
	struct tgec_mdio_controller __iomem *regs = priv->mdio_base;
	uint16_t dev_addr;
	uint32_t mdio_stat;
	uint32_t mdio_ctl;
	uint16_t value;
	int ret;
	bool endian = priv->is_little_endian;

	mdio_stat = xgmac_read32(&regs->mdio_stat, endian);
	if (regnum & MII_ADDR_C45) {
		dev_addr = (regnum >> 16) & 0x1f;
		mdio_stat |= MDIO_STAT_ENC;
	} else {
		dev_addr = regnum & 0x1f;
		mdio_stat &= ~MDIO_STAT_ENC;
	}

	xgmac_write32(mdio_stat, &regs->mdio_stat, endian);

	ret = xgmac_wait_until_free(&bus->dev, regs, endian);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* Set the Port and Device Addrs */
	mdio_ctl = MDIO_CTL_PORT_ADDR(phy_id) | MDIO_CTL_DEV_ADDR(dev_addr);
<<<<<<< HEAD
	out_be32(&regs->mdio_ctl, mdio_ctl);

	/* Set the register address */
	out_be32(&regs->mdio_addr, regnum & 0xffff);

	ret = xgmac_wait_until_free(&bus->dev, regs);
	if (ret)
		return ret;

	/* Initiate the read */
	out_be32(&regs->mdio_ctl, mdio_ctl | MDIO_CTL_READ);

	ret = xgmac_wait_until_done(&bus->dev, regs);
=======
	xgmac_write32(mdio_ctl, &regs->mdio_ctl, endian);

	/* Set the register address */
	if (regnum & MII_ADDR_C45) {
		xgmac_write32(regnum & 0xffff, &regs->mdio_addr, endian);

		ret = xgmac_wait_until_free(&bus->dev, regs, endian);
		if (ret)
			return ret;
	}

	/* Initiate the read */
	xgmac_write32(mdio_ctl | MDIO_CTL_READ, &regs->mdio_ctl, endian);

	ret = xgmac_wait_until_done(&bus->dev, regs, endian);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* Return all Fs if nothing was there */
<<<<<<< HEAD
	if (in_be32(&regs->mdio_stat) & MDIO_STAT_RD_ER) {
=======
	if ((xgmac_read32(&regs->mdio_stat, endian) & MDIO_STAT_RD_ER) &&
	    !priv->has_a011043) {
>>>>>>> v4.9.227
		dev_err(&bus->dev,
			"Error while reading PHY%d reg at %d.%hhu\n",
			phy_id, dev_addr, regnum);
		return 0xffff;
	}

<<<<<<< HEAD
	value = in_be32(&regs->mdio_data) & 0xffff;
=======
	value = xgmac_read32(&regs->mdio_data, endian) & 0xffff;
>>>>>>> v4.9.227
	dev_dbg(&bus->dev, "read %04x\n", value);

	return value;
}

<<<<<<< HEAD
/* Reset the MIIM registers, and wait for the bus to free */
static int xgmac_mdio_reset(struct mii_bus *bus)
{
	struct tgec_mdio_controller __iomem *regs = bus->priv;
	int ret;

	mutex_lock(&bus->mdio_lock);

	/* Setup the MII Mgmt clock speed */
	out_be32(&regs->mdio_stat, MDIO_STAT_CLKDIV(100));

	ret = xgmac_wait_until_free(&bus->dev, regs);

	mutex_unlock(&bus->mdio_lock);

	return ret;
}

=======
>>>>>>> v4.9.227
static int xgmac_mdio_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct mii_bus *bus;
	struct resource res;
<<<<<<< HEAD
=======
	struct mdio_fsl_priv *priv;
>>>>>>> v4.9.227
	int ret;

	ret = of_address_to_resource(np, 0, &res);
	if (ret) {
		dev_err(&pdev->dev, "could not obtain address\n");
		return ret;
	}

<<<<<<< HEAD
	bus = mdiobus_alloc_size(PHY_MAX_ADDR * sizeof(int));
=======
	bus = mdiobus_alloc_size(sizeof(struct mdio_fsl_priv));
>>>>>>> v4.9.227
	if (!bus)
		return -ENOMEM;

	bus->name = "Freescale XGMAC MDIO Bus";
	bus->read = xgmac_mdio_read;
	bus->write = xgmac_mdio_write;
<<<<<<< HEAD
	bus->reset = xgmac_mdio_reset;
	bus->irq = bus->priv;
=======
>>>>>>> v4.9.227
	bus->parent = &pdev->dev;
	snprintf(bus->id, MII_BUS_ID_SIZE, "%llx", (unsigned long long)res.start);

	/* Set the PHY base address */
<<<<<<< HEAD
	bus->priv = of_iomap(np, 0);
	if (!bus->priv) {
=======
	priv = bus->priv;
	priv->mdio_base = of_iomap(np, 0);
	if (!priv->mdio_base) {
>>>>>>> v4.9.227
		ret = -ENOMEM;
		goto err_ioremap;
	}

<<<<<<< HEAD
=======
	priv->is_little_endian = of_property_read_bool(pdev->dev.of_node,
						       "little-endian");

	priv->has_a011043 = of_property_read_bool(pdev->dev.of_node,
						  "fsl,erratum-a011043");

>>>>>>> v4.9.227
	ret = of_mdiobus_register(bus, np);
	if (ret) {
		dev_err(&pdev->dev, "cannot register MDIO bus\n");
		goto err_registration;
	}

	platform_set_drvdata(pdev, bus);

	return 0;

err_registration:
<<<<<<< HEAD
	iounmap(bus->priv);
=======
	iounmap(priv->mdio_base);
>>>>>>> v4.9.227

err_ioremap:
	mdiobus_free(bus);

	return ret;
}

static int xgmac_mdio_remove(struct platform_device *pdev)
{
	struct mii_bus *bus = platform_get_drvdata(pdev);

	mdiobus_unregister(bus);
	iounmap(bus->priv);
	mdiobus_free(bus);

	return 0;
}

<<<<<<< HEAD
static struct of_device_id xgmac_mdio_match[] = {
	{
		.compatible = "fsl,fman-xmdio",
	},
=======
static const struct of_device_id xgmac_mdio_match[] = {
	{
		.compatible = "fsl,fman-xmdio",
	},
	{
		.compatible = "fsl,fman-memac-mdio",
	},
>>>>>>> v4.9.227
	{},
};
MODULE_DEVICE_TABLE(of, xgmac_mdio_match);

static struct platform_driver xgmac_mdio_driver = {
	.driver = {
		.name = "fsl-fman_xmdio",
		.of_match_table = xgmac_mdio_match,
	},
	.probe = xgmac_mdio_probe,
	.remove = xgmac_mdio_remove,
};

module_platform_driver(xgmac_mdio_driver);

MODULE_DESCRIPTION("Freescale QorIQ 10G MDIO Controller");
MODULE_LICENSE("GPL v2");
