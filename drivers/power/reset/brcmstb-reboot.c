/*
 * Copyright (C) 2013 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

<<<<<<< HEAD
=======
#include <linux/bitops.h>
>>>>>>> v4.9.227
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/jiffies.h>
<<<<<<< HEAD
=======
#include <linux/notifier.h>
>>>>>>> v4.9.227
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/printk.h>
#include <linux/reboot.h>
#include <linux/regmap.h>
#include <linux/smp.h>
#include <linux/mfd/syscon.h>

<<<<<<< HEAD
#include <asm/system_misc.h>

=======
>>>>>>> v4.9.227
#define RESET_SOURCE_ENABLE_REG 1
#define SW_MASTER_RESET_REG 2

static struct regmap *regmap;
static u32 rst_src_en;
static u32 sw_mstr_rst;

<<<<<<< HEAD
static void brcmstb_reboot(enum reboot_mode mode, const char *cmd)
=======
struct reset_reg_mask {
	u32 rst_src_en_mask;
	u32 sw_mstr_rst_mask;
};

static const struct reset_reg_mask *reset_masks;

static int brcmstb_restart_handler(struct notifier_block *this,
				   unsigned long mode, void *cmd)
>>>>>>> v4.9.227
{
	int rc;
	u32 tmp;

<<<<<<< HEAD
	rc = regmap_write(regmap, rst_src_en, 1);
	if (rc) {
		pr_err("failed to write rst_src_en (%d)\n", rc);
		return;
=======
	rc = regmap_write(regmap, rst_src_en, reset_masks->rst_src_en_mask);
	if (rc) {
		pr_err("failed to write rst_src_en (%d)\n", rc);
		return NOTIFY_DONE;
>>>>>>> v4.9.227
	}

	rc = regmap_read(regmap, rst_src_en, &tmp);
	if (rc) {
		pr_err("failed to read rst_src_en (%d)\n", rc);
<<<<<<< HEAD
		return;
	}

	rc = regmap_write(regmap, sw_mstr_rst, 1);
	if (rc) {
		pr_err("failed to write sw_mstr_rst (%d)\n", rc);
		return;
=======
		return NOTIFY_DONE;
	}

	rc = regmap_write(regmap, sw_mstr_rst, reset_masks->sw_mstr_rst_mask);
	if (rc) {
		pr_err("failed to write sw_mstr_rst (%d)\n", rc);
		return NOTIFY_DONE;
>>>>>>> v4.9.227
	}

	rc = regmap_read(regmap, sw_mstr_rst, &tmp);
	if (rc) {
		pr_err("failed to read sw_mstr_rst (%d)\n", rc);
<<<<<<< HEAD
		return;
=======
		return NOTIFY_DONE;
>>>>>>> v4.9.227
	}

	while (1)
		;
<<<<<<< HEAD
}

=======

	return NOTIFY_DONE;
}

static struct notifier_block brcmstb_restart_nb = {
	.notifier_call = brcmstb_restart_handler,
	.priority = 128,
};

static const struct reset_reg_mask reset_bits_40nm = {
	.rst_src_en_mask = BIT(0),
	.sw_mstr_rst_mask = BIT(0),
};

static const struct reset_reg_mask reset_bits_65nm = {
	.rst_src_en_mask = BIT(3),
	.sw_mstr_rst_mask = BIT(31),
};

static const struct of_device_id of_match[] = {
	{ .compatible = "brcm,brcmstb-reboot", .data = &reset_bits_40nm },
	{ .compatible = "brcm,bcm7038-reboot", .data = &reset_bits_65nm },
	{},
};

>>>>>>> v4.9.227
static int brcmstb_reboot_probe(struct platform_device *pdev)
{
	int rc;
	struct device_node *np = pdev->dev.of_node;
<<<<<<< HEAD
=======
	const struct of_device_id *of_id;

	of_id = of_match_node(of_match, np);
	if (!of_id) {
		pr_err("failed to look up compatible string\n");
		return -EINVAL;
	}
	reset_masks = of_id->data;
>>>>>>> v4.9.227

	regmap = syscon_regmap_lookup_by_phandle(np, "syscon");
	if (IS_ERR(regmap)) {
		pr_err("failed to get syscon phandle\n");
		return -EINVAL;
	}

	rc = of_property_read_u32_index(np, "syscon", RESET_SOURCE_ENABLE_REG,
					&rst_src_en);
	if (rc) {
		pr_err("can't get rst_src_en offset (%d)\n", rc);
		return -EINVAL;
	}

	rc = of_property_read_u32_index(np, "syscon", SW_MASTER_RESET_REG,
					&sw_mstr_rst);
	if (rc) {
		pr_err("can't get sw_mstr_rst offset (%d)\n", rc);
		return -EINVAL;
	}

<<<<<<< HEAD
	arm_pm_restart = brcmstb_reboot;

	return 0;
}

static const struct of_device_id of_match[] = {
	{ .compatible = "brcm,brcmstb-reboot", },
	{},
};

=======
	rc = register_restart_handler(&brcmstb_restart_nb);
	if (rc)
		dev_err(&pdev->dev,
			"cannot register restart handler (err=%d)\n", rc);

	return rc;
}

>>>>>>> v4.9.227
static struct platform_driver brcmstb_reboot_driver = {
	.probe = brcmstb_reboot_probe,
	.driver = {
		.name = "brcmstb-reboot",
<<<<<<< HEAD
		.owner = THIS_MODULE,
=======
>>>>>>> v4.9.227
		.of_match_table = of_match,
	},
};

static int __init brcmstb_reboot_init(void)
{
	return platform_driver_probe(&brcmstb_reboot_driver,
					brcmstb_reboot_probe);
}
subsys_initcall(brcmstb_reboot_init);
