/*
 * RNG driver for AMD RNGs
 *
 * Copyright 2005 (c) MontaVista Software, Inc.
 *
 * with the majority of the code coming from:
 *
 * Hardware driver for the Intel/AMD/VIA Random Number Generators (RNG)
 * (c) Copyright 2003 Red Hat Inc <jgarzik@redhat.com>
 *
 * derived from
 *
 * Hardware driver for the AMD 768 Random Number Generator (RNG)
 * (c) Copyright 2001 Red Hat Inc
 *
 * derived from
 *
 * Hardware driver for Intel i810 Random Number Generator (RNG)
 * Copyright 2000,2001 Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2000,2001 Philipp Rumpf <prumpf@mandrakesoft.com>
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

<<<<<<< HEAD
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/hw_random.h>
#include <linux/delay.h>
#include <asm/io.h>


#define PFX	KBUILD_MODNAME ": "

=======
#include <linux/delay.h>
#include <linux/hw_random.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>

#define DRV_NAME "AMD768-HWRNG"

#define RNGDATA		0x00
#define RNGDONE		0x04
#define PMBASE_OFFSET	0xF0
#define PMBASE_SIZE	8
>>>>>>> v4.9.227

/*
 * Data for PCI driver interface
 *
 * This data only exists for exporting the supported
 * PCI ids via MODULE_DEVICE_TABLE.  We do not actually
 * register a pci_driver, because someone else might one day
 * want to register another driver on the same PCI id.
 */
static const struct pci_device_id pci_tbl[] = {
	{ PCI_VDEVICE(AMD, 0x7443), 0, },
	{ PCI_VDEVICE(AMD, 0x746b), 0, },
	{ 0, },	/* terminate list */
};
MODULE_DEVICE_TABLE(pci, pci_tbl);

<<<<<<< HEAD
static struct pci_dev *amd_pdev;


static int amd_rng_data_present(struct hwrng *rng, int wait)
{
	u32 pmbase = (u32)rng->priv;
	int data, i;

	for (i = 0; i < 20; i++) {
		data = !!(inl(pmbase + 0xF4) & 1);
		if (data || !wait)
			break;
		udelay(10);
	}
	return data;
}

static int amd_rng_data_read(struct hwrng *rng, u32 *data)
{
	u32 pmbase = (u32)rng->priv;

	*data = inl(pmbase + 0xF0);

	return 4;
=======
struct amd768_priv {
	void __iomem *iobase;
	struct pci_dev *pcidev;
	u32 pmbase;
};

static int amd_rng_read(struct hwrng *rng, void *buf, size_t max, bool wait)
{
	u32 *data = buf;
	struct amd768_priv *priv = (struct amd768_priv *)rng->priv;
	size_t read = 0;
	/* We will wait at maximum one time per read */
	int timeout = max / 4 + 1;

	/*
	 * RNG data is available when RNGDONE is set to 1
	 * New random numbers are generated approximately 128 microseconds
	 * after RNGDATA is read
	 */
	while (read < max) {
		if (ioread32(priv->iobase + RNGDONE) == 0) {
			if (wait) {
				/* Delay given by datasheet */
				usleep_range(128, 196);
				if (timeout-- == 0)
					return read;
			} else {
				return 0;
			}
		} else {
			*data = ioread32(priv->iobase + RNGDATA);
			data++;
			read += 4;
		}
	}

	return read;
>>>>>>> v4.9.227
}

static int amd_rng_init(struct hwrng *rng)
{
<<<<<<< HEAD
	u8 rnen;

	pci_read_config_byte(amd_pdev, 0x40, &rnen);
	rnen |= (1 << 7);	/* RNG on */
	pci_write_config_byte(amd_pdev, 0x40, rnen);

	pci_read_config_byte(amd_pdev, 0x41, &rnen);
	rnen |= (1 << 7);	/* PMIO enable */
	pci_write_config_byte(amd_pdev, 0x41, rnen);
=======
	struct amd768_priv *priv = (struct amd768_priv *)rng->priv;
	u8 rnen;

	pci_read_config_byte(priv->pcidev, 0x40, &rnen);
	rnen |= BIT(7);	/* RNG on */
	pci_write_config_byte(priv->pcidev, 0x40, rnen);

	pci_read_config_byte(priv->pcidev, 0x41, &rnen);
	rnen |= BIT(7);	/* PMIO enable */
	pci_write_config_byte(priv->pcidev, 0x41, rnen);
>>>>>>> v4.9.227

	return 0;
}

static void amd_rng_cleanup(struct hwrng *rng)
{
<<<<<<< HEAD
	u8 rnen;

	pci_read_config_byte(amd_pdev, 0x40, &rnen);
	rnen &= ~(1 << 7);	/* RNG off */
	pci_write_config_byte(amd_pdev, 0x40, rnen);
}


=======
	struct amd768_priv *priv = (struct amd768_priv *)rng->priv;
	u8 rnen;

	pci_read_config_byte(priv->pcidev, 0x40, &rnen);
	rnen &= ~BIT(7);	/* RNG off */
	pci_write_config_byte(priv->pcidev, 0x40, rnen);
}

>>>>>>> v4.9.227
static struct hwrng amd_rng = {
	.name		= "amd",
	.init		= amd_rng_init,
	.cleanup	= amd_rng_cleanup,
<<<<<<< HEAD
	.data_present	= amd_rng_data_present,
	.data_read	= amd_rng_data_read,
};


=======
	.read		= amd_rng_read,
};

>>>>>>> v4.9.227
static int __init mod_init(void)
{
	int err = -ENODEV;
	struct pci_dev *pdev = NULL;
	const struct pci_device_id *ent;
	u32 pmbase;
<<<<<<< HEAD
=======
	struct amd768_priv *priv;
>>>>>>> v4.9.227

	for_each_pci_dev(pdev) {
		ent = pci_match_id(pci_tbl, pdev);
		if (ent)
			goto found;
	}
	/* Device not found. */
<<<<<<< HEAD
	goto out;
=======
	return -ENODEV;
>>>>>>> v4.9.227

found:
	err = pci_read_config_dword(pdev, 0x58, &pmbase);
	if (err)
<<<<<<< HEAD
		goto out;
	err = -EIO;
	pmbase &= 0x0000FF00;
	if (pmbase == 0)
		goto out;
	if (!request_region(pmbase + 0xF0, 8, "AMD HWRNG")) {
		dev_err(&pdev->dev, "AMD HWRNG region 0x%x already in use!\n",
=======
		return err;

	pmbase &= 0x0000FF00;
	if (pmbase == 0)
		return -EIO;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	if (!request_region(pmbase + PMBASE_OFFSET, PMBASE_SIZE, DRV_NAME)) {
		dev_err(&pdev->dev, DRV_NAME " region 0x%x already in use!\n",
>>>>>>> v4.9.227
			pmbase + 0xF0);
		err = -EBUSY;
		goto out;
	}
<<<<<<< HEAD
	amd_rng.priv = (unsigned long)pmbase;
	amd_pdev = pdev;

	pr_info("AMD768 RNG detected\n");
	err = hwrng_register(&amd_rng);
	if (err) {
		pr_err(PFX "RNG registering failed (%d)\n",
		       err);
		release_region(pmbase + 0xF0, 8);
		goto out;
	}
out:
=======

	priv->iobase = ioport_map(pmbase + PMBASE_OFFSET, PMBASE_SIZE);
	if (!priv->iobase) {
		pr_err(DRV_NAME "Cannot map ioport\n");
		err = -EINVAL;
		goto err_iomap;
	}

	amd_rng.priv = (unsigned long)priv;
	priv->pmbase = pmbase;
	priv->pcidev = pdev;

	pr_info(DRV_NAME " detected\n");
	err = hwrng_register(&amd_rng);
	if (err) {
		pr_err(DRV_NAME " registering failed (%d)\n", err);
		goto err_hwrng;
	}
	return 0;

err_hwrng:
	ioport_unmap(priv->iobase);
err_iomap:
	release_region(pmbase + PMBASE_OFFSET, PMBASE_SIZE);
out:
	kfree(priv);
>>>>>>> v4.9.227
	return err;
}

static void __exit mod_exit(void)
{
<<<<<<< HEAD
	u32 pmbase = (unsigned long)amd_rng.priv;
	release_region(pmbase + 0xF0, 8);
	hwrng_unregister(&amd_rng);
=======
	struct amd768_priv *priv;

	priv = (struct amd768_priv *)amd_rng.priv;

	hwrng_unregister(&amd_rng);

	ioport_unmap(priv->iobase);

	release_region(priv->pmbase + PMBASE_OFFSET, PMBASE_SIZE);

	kfree(priv);
>>>>>>> v4.9.227
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_AUTHOR("The Linux Kernel team");
MODULE_DESCRIPTION("H/W RNG driver for AMD chipsets");
MODULE_LICENSE("GPL");
