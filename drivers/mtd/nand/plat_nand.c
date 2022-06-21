/*
 * Generic NAND driver
 *
 * Author: Vitaly Wool <vitalywool@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

struct plat_nand_data {
	struct nand_chip	chip;
<<<<<<< HEAD
	struct mtd_info		mtd;
	void __iomem		*io_base;
};

static const char *part_probe_types[] = { "cmdlinepart", NULL };

=======
	void __iomem		*io_base;
};

>>>>>>> v4.9.227
/*
 * Probe for the NAND device.
 */
static int plat_nand_probe(struct platform_device *pdev)
{
	struct platform_nand_data *pdata = dev_get_platdata(&pdev->dev);
<<<<<<< HEAD
	struct mtd_part_parser_data ppdata;
	struct plat_nand_data *data;
=======
	struct plat_nand_data *data;
	struct mtd_info *mtd;
>>>>>>> v4.9.227
	struct resource *res;
	const char **part_types;
	int err = 0;

	if (!pdata) {
		dev_err(&pdev->dev, "platform_nand_data is missing\n");
		return -EINVAL;
	}

	if (pdata->chip.nr_chips < 1) {
		dev_err(&pdev->dev, "invalid number of chips specified\n");
		return -EINVAL;
	}

	/* Allocate memory for the device structure (and zero it) */
	data = devm_kzalloc(&pdev->dev, sizeof(struct plat_nand_data),
			    GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	data->io_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(data->io_base))
		return PTR_ERR(data->io_base);

<<<<<<< HEAD
	data->chip.priv = &data;
	data->mtd.priv = &data->chip;
	data->mtd.owner = THIS_MODULE;
	data->mtd.name = dev_name(&pdev->dev);
=======
	nand_set_flash_node(&data->chip, pdev->dev.of_node);
	mtd = nand_to_mtd(&data->chip);
	mtd->dev.parent = &pdev->dev;
>>>>>>> v4.9.227

	data->chip.IO_ADDR_R = data->io_base;
	data->chip.IO_ADDR_W = data->io_base;
	data->chip.cmd_ctrl = pdata->ctrl.cmd_ctrl;
	data->chip.dev_ready = pdata->ctrl.dev_ready;
	data->chip.select_chip = pdata->ctrl.select_chip;
	data->chip.write_buf = pdata->ctrl.write_buf;
	data->chip.read_buf = pdata->ctrl.read_buf;
	data->chip.read_byte = pdata->ctrl.read_byte;
	data->chip.chip_delay = pdata->chip.chip_delay;
	data->chip.options |= pdata->chip.options;
	data->chip.bbt_options |= pdata->chip.bbt_options;

	data->chip.ecc.hwctl = pdata->ctrl.hwcontrol;
<<<<<<< HEAD
	data->chip.ecc.layout = pdata->chip.ecclayout;
	data->chip.ecc.mode = NAND_ECC_SOFT;
=======
	data->chip.ecc.mode = NAND_ECC_SOFT;
	data->chip.ecc.algo = NAND_ECC_HAMMING;
>>>>>>> v4.9.227

	platform_set_drvdata(pdev, data);

	/* Handle any platform specific setup */
	if (pdata->ctrl.probe) {
		err = pdata->ctrl.probe(pdev);
		if (err)
			goto out;
	}

	/* Scan to find existence of the device */
<<<<<<< HEAD
	if (nand_scan(&data->mtd, pdata->chip.nr_chips)) {
=======
	if (nand_scan(mtd, pdata->chip.nr_chips)) {
>>>>>>> v4.9.227
		err = -ENXIO;
		goto out;
	}

<<<<<<< HEAD
	part_types = pdata->chip.part_probe_types ? : part_probe_types;

	ppdata.of_node = pdev->dev.of_node;
	err = mtd_device_parse_register(&data->mtd, part_types, &ppdata,
=======
	part_types = pdata->chip.part_probe_types;

	err = mtd_device_parse_register(mtd, part_types, NULL,
>>>>>>> v4.9.227
					pdata->chip.partitions,
					pdata->chip.nr_partitions);

	if (!err)
		return err;

<<<<<<< HEAD
	nand_release(&data->mtd);
=======
	nand_release(mtd);
>>>>>>> v4.9.227
out:
	if (pdata->ctrl.remove)
		pdata->ctrl.remove(pdev);
	return err;
}

/*
 * Remove a NAND device.
 */
static int plat_nand_remove(struct platform_device *pdev)
{
	struct plat_nand_data *data = platform_get_drvdata(pdev);
	struct platform_nand_data *pdata = dev_get_platdata(&pdev->dev);

<<<<<<< HEAD
	nand_release(&data->mtd);
=======
	nand_release(nand_to_mtd(&data->chip));
>>>>>>> v4.9.227
	if (pdata->ctrl.remove)
		pdata->ctrl.remove(pdev);

	return 0;
}

static const struct of_device_id plat_nand_match[] = {
	{ .compatible = "gen_nand" },
	{},
};
MODULE_DEVICE_TABLE(of, plat_nand_match);

static struct platform_driver plat_nand_driver = {
	.probe	= plat_nand_probe,
	.remove	= plat_nand_remove,
	.driver	= {
		.name		= "gen_nand",
<<<<<<< HEAD
		.owner		= THIS_MODULE,
=======
>>>>>>> v4.9.227
		.of_match_table = plat_nand_match,
	},
};

module_platform_driver(plat_nand_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vitaly Wool");
MODULE_DESCRIPTION("Simple generic NAND driver");
MODULE_ALIAS("platform:gen_nand");
