/*
<<<<<<< HEAD
 * Copyright 2011 Texas Instruments Inc.
 *
 * Author: Margarita Olaya <magi@slimlogic.co.uk>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 * This driver is based on wm8350 implementation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/mfd/core.h>
#include <linux/platform_device.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/mfd/tps65912.h>

struct tps65912_gpio_data {
	struct tps65912 *tps65912;
	struct gpio_chip gpio_chip;
};

#define to_tgd(gc) container_of(gc, struct tps65912_gpio_data, gpio_chip)

static int tps65912_gpio_get(struct gpio_chip *gc, unsigned offset)
{
	struct tps65912_gpio_data *tps65912_gpio = to_tgd(gc);
	struct tps65912 *tps65912 = tps65912_gpio->tps65912;
	int val;

	val = tps65912_reg_read(tps65912, TPS65912_GPIO1 + offset);
=======
 * GPIO driver for TI TPS65912x PMICs
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 *	Andrew F. Davis <afd@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether expressed or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * Based on the Arizona GPIO driver and the previous TPS65912 driver by
 * Margarita Olaya Cabrera <magi@slimlogic.co.uk>
 */

#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <linux/mfd/tps65912.h>

struct tps65912_gpio {
	struct gpio_chip gpio_chip;
	struct tps65912 *tps;
};

static int tps65912_gpio_get_direction(struct gpio_chip *gc,
				       unsigned offset)
{
	struct tps65912_gpio *gpio = gpiochip_get_data(gc);

	int ret, val;

	ret = regmap_read(gpio->tps->regmap, TPS65912_GPIO1 + offset, &val);
	if (ret)
		return ret;

	if (val & GPIO_CFG_MASK)
		return GPIOF_DIR_OUT;
	else
		return GPIOF_DIR_IN;
}

static int tps65912_gpio_direction_input(struct gpio_chip *gc, unsigned offset)
{
	struct tps65912_gpio *gpio = gpiochip_get_data(gc);

	return regmap_update_bits(gpio->tps->regmap, TPS65912_GPIO1 + offset,
				  GPIO_CFG_MASK, 0);
}

static int tps65912_gpio_direction_output(struct gpio_chip *gc,
					  unsigned offset, int value)
{
	struct tps65912_gpio *gpio = gpiochip_get_data(gc);

	/* Set the initial value */
	regmap_update_bits(gpio->tps->regmap, TPS65912_GPIO1 + offset,
			   GPIO_SET_MASK, value ? GPIO_SET_MASK : 0);

	return regmap_update_bits(gpio->tps->regmap, TPS65912_GPIO1 + offset,
				  GPIO_CFG_MASK, GPIO_CFG_MASK);
}

static int tps65912_gpio_get(struct gpio_chip *gc, unsigned offset)
{
	struct tps65912_gpio *gpio = gpiochip_get_data(gc);
	int ret, val;

	ret = regmap_read(gpio->tps->regmap, TPS65912_GPIO1 + offset, &val);
	if (ret)
		return ret;
>>>>>>> v4.9.227

	if (val & GPIO_STS_MASK)
		return 1;

	return 0;
}

static void tps65912_gpio_set(struct gpio_chip *gc, unsigned offset,
			      int value)
{
<<<<<<< HEAD
	struct tps65912_gpio_data *tps65912_gpio = to_tgd(gc);
	struct tps65912 *tps65912 = tps65912_gpio->tps65912;

	if (value)
		tps65912_set_bits(tps65912, TPS65912_GPIO1 + offset,
							GPIO_SET_MASK);
	else
		tps65912_clear_bits(tps65912, TPS65912_GPIO1 + offset,
								GPIO_SET_MASK);
}

static int tps65912_gpio_output(struct gpio_chip *gc, unsigned offset,
				int value)
{
	struct tps65912_gpio_data *tps65912_gpio = to_tgd(gc);
	struct tps65912 *tps65912 = tps65912_gpio->tps65912;

	/* Set the initial value */
	tps65912_gpio_set(gc, offset, value);

	return tps65912_set_bits(tps65912, TPS65912_GPIO1 + offset,
								GPIO_CFG_MASK);
}

static int tps65912_gpio_input(struct gpio_chip *gc, unsigned offset)
{
	struct tps65912_gpio_data *tps65912_gpio = to_tgd(gc);
	struct tps65912 *tps65912 = tps65912_gpio->tps65912;

	return tps65912_clear_bits(tps65912, TPS65912_GPIO1 + offset,
								GPIO_CFG_MASK);
}

static struct gpio_chip template_chip = {
	.label			= "tps65912",
	.owner			= THIS_MODULE,
	.direction_input	= tps65912_gpio_input,
	.direction_output	= tps65912_gpio_output,
	.get			= tps65912_gpio_get,
	.set			= tps65912_gpio_set,
	.can_sleep		= true,
	.ngpio			= 5,
	.base			= -1,
=======
	struct tps65912_gpio *gpio = gpiochip_get_data(gc);

	regmap_update_bits(gpio->tps->regmap, TPS65912_GPIO1 + offset,
			   GPIO_SET_MASK, value ? GPIO_SET_MASK : 0);
}

static const struct gpio_chip template_chip = {
	.label			= "tps65912-gpio",
	.owner			= THIS_MODULE,
	.get_direction		= tps65912_gpio_get_direction,
	.direction_input	= tps65912_gpio_direction_input,
	.direction_output	= tps65912_gpio_direction_output,
	.get			= tps65912_gpio_get,
	.set			= tps65912_gpio_set,
	.base			= -1,
	.ngpio			= 5,
	.can_sleep		= true,
>>>>>>> v4.9.227
};

static int tps65912_gpio_probe(struct platform_device *pdev)
{
<<<<<<< HEAD
	struct tps65912 *tps65912 = dev_get_drvdata(pdev->dev.parent);
	struct tps65912_board *pdata = dev_get_platdata(tps65912->dev);
	struct tps65912_gpio_data *tps65912_gpio;
	int ret;

	tps65912_gpio = devm_kzalloc(&pdev->dev, sizeof(*tps65912_gpio),
				     GFP_KERNEL);
	if (tps65912_gpio == NULL)
		return -ENOMEM;

	tps65912_gpio->tps65912 = tps65912;
	tps65912_gpio->gpio_chip = template_chip;
	tps65912_gpio->gpio_chip.dev = &pdev->dev;
	if (pdata && pdata->gpio_base)
		tps65912_gpio->gpio_chip.base = pdata->gpio_base;

	ret = gpiochip_add(&tps65912_gpio->gpio_chip);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to register gpiochip, %d\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, tps65912_gpio);

	return ret;
}

static int tps65912_gpio_remove(struct platform_device *pdev)
{
	struct tps65912_gpio_data  *tps65912_gpio = platform_get_drvdata(pdev);

	gpiochip_remove(&tps65912_gpio->gpio_chip);
	return 0;
}

static struct platform_driver tps65912_gpio_driver = {
	.driver = {
		.name = "tps65912-gpio",
		.owner = THIS_MODULE,
	},
	.probe = tps65912_gpio_probe,
	.remove = tps65912_gpio_remove,
};

static int __init tps65912_gpio_init(void)
{
	return platform_driver_register(&tps65912_gpio_driver);
}
subsys_initcall(tps65912_gpio_init);

static void __exit tps65912_gpio_exit(void)
{
	platform_driver_unregister(&tps65912_gpio_driver);
}
module_exit(tps65912_gpio_exit);

MODULE_AUTHOR("Margarita Olaya Cabrera <magi@slimlogic.co.uk>");
MODULE_DESCRIPTION("GPIO interface for TPS65912 PMICs");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:tps65912-gpio");
=======
	struct tps65912 *tps = dev_get_drvdata(pdev->dev.parent);
	struct tps65912_gpio *gpio;
	int ret;

	gpio = devm_kzalloc(&pdev->dev, sizeof(*gpio), GFP_KERNEL);
	if (!gpio)
		return -ENOMEM;

	gpio->tps = dev_get_drvdata(pdev->dev.parent);
	gpio->gpio_chip = template_chip;
	gpio->gpio_chip.parent = tps->dev;

	ret = devm_gpiochip_add_data(&pdev->dev, &gpio->gpio_chip,
				     gpio);
	if (ret < 0) {
		dev_err(&pdev->dev, "Could not register gpiochip, %d\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, gpio);

	return 0;
}

static const struct platform_device_id tps65912_gpio_id_table[] = {
	{ "tps65912-gpio", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(platform, tps65912_gpio_id_table);

static struct platform_driver tps65912_gpio_driver = {
	.driver = {
		.name = "tps65912-gpio",
	},
	.probe = tps65912_gpio_probe,
	.id_table = tps65912_gpio_id_table,
};
module_platform_driver(tps65912_gpio_driver);

MODULE_AUTHOR("Andrew F. Davis <afd@ti.com>");
MODULE_DESCRIPTION("TPS65912 GPIO driver");
MODULE_LICENSE("GPL v2");
>>>>>>> v4.9.227
