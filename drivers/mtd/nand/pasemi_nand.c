/*
 * Copyright (C) 2006-2007 PA Semi, Inc
 *
 * Author: Egor Martovetsky <egor@pasemi.com>
 * Maintained by: Olof Johansson <olof@lixom.net>
 *
 * Driver for the PWRficient onchip NAND flash interface
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#undef DEBUG

#include <linux/slab.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/pci.h>

#include <asm/io.h>

#define LBICTRL_LPCCTL_NR		0x00004000
#define CLE_PIN_CTL			15
#define ALE_PIN_CTL			14

static unsigned int lpcctl;
static struct mtd_info *pasemi_nand_mtd;
static const char driver_name[] = "pasemi-nand";

static void pasemi_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
<<<<<<< HEAD
	struct nand_chip *chip = mtd->priv;
=======
	struct nand_chip *chip = mtd_to_nand(mtd);
>>>>>>> v4.9.227

	while (len > 0x800) {
		memcpy_fromio(buf, chip->IO_ADDR_R, 0x800);
		buf += 0x800;
		len -= 0x800;
	}
	memcpy_fromio(buf, chip->IO_ADDR_R, len);
}

static void pasemi_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
<<<<<<< HEAD
	struct nand_chip *chip = mtd->priv;
=======
	struct nand_chip *chip = mtd_to_nand(mtd);
>>>>>>> v4.9.227

	while (len > 0x800) {
		memcpy_toio(chip->IO_ADDR_R, buf, 0x800);
		buf += 0x800;
		len -= 0x800;
	}
	memcpy_toio(chip->IO_ADDR_R, buf, len);
}

static void pasemi_hwcontrol(struct mtd_info *mtd, int cmd,
			     unsigned int ctrl)
{
<<<<<<< HEAD
	struct nand_chip *chip = mtd->priv;
=======
	struct nand_chip *chip = mtd_to_nand(mtd);
>>>>>>> v4.9.227

	if (cmd == NAND_CMD_NONE)
		return;

	if (ctrl & NAND_CLE)
		out_8(chip->IO_ADDR_W + (1 << CLE_PIN_CTL), cmd);
	else
		out_8(chip->IO_ADDR_W + (1 << ALE_PIN_CTL), cmd);

	/* Push out posted writes */
	eieio();
	inl(lpcctl);
}

int pasemi_device_ready(struct mtd_info *mtd)
{
	return !!(inl(lpcctl) & LBICTRL_LPCCTL_NR);
}

static int pasemi_nand_probe(struct platform_device *ofdev)
{
<<<<<<< HEAD
	struct pci_dev *pdev;
	struct device_node *np = ofdev->dev.of_node;
=======
	struct device *dev = &ofdev->dev;
	struct pci_dev *pdev;
	struct device_node *np = dev->of_node;
>>>>>>> v4.9.227
	struct resource res;
	struct nand_chip *chip;
	int err = 0;

	err = of_address_to_resource(np, 0, &res);

	if (err)
		return -EINVAL;

	/* We only support one device at the moment */
	if (pasemi_nand_mtd)
		return -ENODEV;

<<<<<<< HEAD
	pr_debug("pasemi_nand at %pR\n", &res);

	/* Allocate memory for MTD device structure and private data */
	pasemi_nand_mtd = kzalloc(sizeof(struct mtd_info) +
				  sizeof(struct nand_chip), GFP_KERNEL);
	if (!pasemi_nand_mtd) {
		printk(KERN_WARNING
		       "Unable to allocate PASEMI NAND MTD device structure\n");
=======
	dev_dbg(dev, "pasemi_nand at %pR\n", &res);

	/* Allocate memory for MTD device structure and private data */
	chip = kzalloc(sizeof(struct nand_chip), GFP_KERNEL);
	if (!chip) {
>>>>>>> v4.9.227
		err = -ENOMEM;
		goto out;
	}

<<<<<<< HEAD
	/* Get pointer to private data */
	chip = (struct nand_chip *)&pasemi_nand_mtd[1];

	/* Link the private data with the MTD structure */
	pasemi_nand_mtd->priv = chip;
	pasemi_nand_mtd->owner = THIS_MODULE;
=======
	pasemi_nand_mtd = nand_to_mtd(chip);

	/* Link the private data with the MTD structure */
	pasemi_nand_mtd->dev.parent = dev;
>>>>>>> v4.9.227

	chip->IO_ADDR_R = of_iomap(np, 0);
	chip->IO_ADDR_W = chip->IO_ADDR_R;

	if (!chip->IO_ADDR_R) {
		err = -EIO;
		goto out_mtd;
	}

	pdev = pci_get_device(PCI_VENDOR_ID_PASEMI, 0xa008, NULL);
	if (!pdev) {
		err = -ENODEV;
		goto out_ior;
	}

	lpcctl = pci_resource_start(pdev, 0);
	pci_dev_put(pdev);

	if (!request_region(lpcctl, 4, driver_name)) {
		err = -EBUSY;
		goto out_ior;
	}

	chip->cmd_ctrl = pasemi_hwcontrol;
	chip->dev_ready = pasemi_device_ready;
	chip->read_buf = pasemi_read_buf;
	chip->write_buf = pasemi_write_buf;
	chip->chip_delay = 0;
	chip->ecc.mode = NAND_ECC_SOFT;
<<<<<<< HEAD
=======
	chip->ecc.algo = NAND_ECC_HAMMING;
>>>>>>> v4.9.227

	/* Enable the following for a flash based bad block table */
	chip->bbt_options = NAND_BBT_USE_FLASH;

	/* Scan to find existence of the device */
	if (nand_scan(pasemi_nand_mtd, 1)) {
		err = -ENXIO;
		goto out_lpc;
	}

	if (mtd_device_register(pasemi_nand_mtd, NULL, 0)) {
<<<<<<< HEAD
		printk(KERN_ERR "pasemi_nand: Unable to register MTD device\n");
=======
		dev_err(dev, "Unable to register MTD device\n");
>>>>>>> v4.9.227
		err = -ENODEV;
		goto out_lpc;
	}

<<<<<<< HEAD
	printk(KERN_INFO "PA Semi NAND flash at %08llx, control at I/O %x\n",
	       res.start, lpcctl);
=======
	dev_info(dev, "PA Semi NAND flash at %pR, control at I/O %x\n", &res,
		 lpcctl);
>>>>>>> v4.9.227

	return 0;

 out_lpc:
	release_region(lpcctl, 4);
 out_ior:
	iounmap(chip->IO_ADDR_R);
 out_mtd:
<<<<<<< HEAD
	kfree(pasemi_nand_mtd);
=======
	kfree(chip);
>>>>>>> v4.9.227
 out:
	return err;
}

static int pasemi_nand_remove(struct platform_device *ofdev)
{
	struct nand_chip *chip;

	if (!pasemi_nand_mtd)
		return 0;

<<<<<<< HEAD
	chip = pasemi_nand_mtd->priv;
=======
	chip = mtd_to_nand(pasemi_nand_mtd);
>>>>>>> v4.9.227

	/* Release resources, unregister device */
	nand_release(pasemi_nand_mtd);

	release_region(lpcctl, 4);

	iounmap(chip->IO_ADDR_R);

	/* Free the MTD device structure */
<<<<<<< HEAD
	kfree(pasemi_nand_mtd);
=======
	kfree(chip);
>>>>>>> v4.9.227

	pasemi_nand_mtd = NULL;

	return 0;
}

static const struct of_device_id pasemi_nand_match[] =
{
	{
		.compatible   = "pasemi,localbus-nand",
	},
	{},
};

MODULE_DEVICE_TABLE(of, pasemi_nand_match);

static struct platform_driver pasemi_nand_driver =
{
	.driver = {
		.name = driver_name,
<<<<<<< HEAD
		.owner = THIS_MODULE,
=======
>>>>>>> v4.9.227
		.of_match_table = pasemi_nand_match,
	},
	.probe		= pasemi_nand_probe,
	.remove		= pasemi_nand_remove,
};

module_platform_driver(pasemi_nand_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Egor Martovetsky <egor@pasemi.com>");
MODULE_DESCRIPTION("NAND flash interface driver for PA Semi PWRficient");
