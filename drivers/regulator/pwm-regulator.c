/*
 * Regulator driver for PWM Regulators
 *
 * Copyright (C) 2014 - STMicroelectronics Inc.
 *
 * Author: Lee Jones <lee.jones@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pwm.h>
<<<<<<< HEAD

struct pwm_regulator_data {
	struct regulator_desc desc;
	struct pwm_voltages *duty_cycle_table;
	struct pwm_device *pwm;
	bool enabled;
	int state;
=======
#include <linux/gpio/consumer.h>

struct pwm_continuous_reg_data {
	unsigned int min_uV_dutycycle;
	unsigned int max_uV_dutycycle;
	unsigned int dutycycle_unit;
};

struct pwm_regulator_data {
	/*  Shared */
	struct pwm_device *pwm;

	/* Voltage table */
	struct pwm_voltages *duty_cycle_table;

	/* Continuous mode info */
	struct pwm_continuous_reg_data continuous;

	/* regulator descriptor */
	struct regulator_desc desc;

	/* Regulator ops */
	struct regulator_ops ops;

	int state;

	/* Enable GPIO */
	struct gpio_desc *enb_gpio;
>>>>>>> v4.9.227
};

struct pwm_voltages {
	unsigned int uV;
	unsigned int dutycycle;
};

<<<<<<< HEAD
static int pwm_regulator_get_voltage_sel(struct regulator_dev *dev)
{
	struct pwm_regulator_data *drvdata = rdev_get_drvdata(dev);
=======
/**
 * Voltage table call-backs
 */
static void pwm_regulator_init_state(struct regulator_dev *rdev)
{
	struct pwm_regulator_data *drvdata = rdev_get_drvdata(rdev);
	struct pwm_state pwm_state;
	unsigned int dutycycle;
	int i;

	pwm_get_state(drvdata->pwm, &pwm_state);
	dutycycle = pwm_get_relative_duty_cycle(&pwm_state, 100);

	for (i = 0; i < rdev->desc->n_voltages; i++) {
		if (dutycycle == drvdata->duty_cycle_table[i].dutycycle) {
			drvdata->state = i;
			return;
		}
	}
}

static int pwm_regulator_get_voltage_sel(struct regulator_dev *rdev)
{
	struct pwm_regulator_data *drvdata = rdev_get_drvdata(rdev);

	if (drvdata->state < 0)
		pwm_regulator_init_state(rdev);
>>>>>>> v4.9.227

	return drvdata->state;
}

<<<<<<< HEAD
static int pwm_regulator_set_voltage_sel(struct regulator_dev *dev,
					 unsigned selector)
{
	struct pwm_regulator_data *drvdata = rdev_get_drvdata(dev);
	unsigned int pwm_reg_period;
	int dutycycle;
	int ret;

	pwm_reg_period = pwm_get_period(drvdata->pwm);

	dutycycle = (pwm_reg_period *
		    drvdata->duty_cycle_table[selector].dutycycle) / 100;

	ret = pwm_config(drvdata->pwm, dutycycle, pwm_reg_period);
	if (ret) {
		dev_err(&dev->dev, "Failed to configure PWM\n");
=======
static int pwm_regulator_set_voltage_sel(struct regulator_dev *rdev,
					 unsigned selector)
{
	struct pwm_regulator_data *drvdata = rdev_get_drvdata(rdev);
	struct pwm_state pstate;
	int ret;

	pwm_init_state(drvdata->pwm, &pstate);
	pwm_set_relative_duty_cycle(&pstate,
			drvdata->duty_cycle_table[selector].dutycycle, 100);

	ret = pwm_apply_state(drvdata->pwm, &pstate);
	if (ret) {
		dev_err(&rdev->dev, "Failed to configure PWM: %d\n", ret);
>>>>>>> v4.9.227
		return ret;
	}

	drvdata->state = selector;

<<<<<<< HEAD
	if (!drvdata->enabled) {
		ret = pwm_enable(drvdata->pwm);
		if (ret) {
			dev_err(&dev->dev, "Failed to enable PWM\n");
			return ret;
		}
		drvdata->enabled = true;
	}

	return 0;
}

static int pwm_regulator_list_voltage(struct regulator_dev *dev,
				      unsigned selector)
{
	struct pwm_regulator_data *drvdata = rdev_get_drvdata(dev);

	if (selector >= drvdata->desc.n_voltages)
=======
	return 0;
}

static int pwm_regulator_list_voltage(struct regulator_dev *rdev,
				      unsigned selector)
{
	struct pwm_regulator_data *drvdata = rdev_get_drvdata(rdev);

	if (selector >= rdev->desc->n_voltages)
>>>>>>> v4.9.227
		return -EINVAL;

	return drvdata->duty_cycle_table[selector].uV;
}

<<<<<<< HEAD
static struct regulator_ops pwm_regulator_voltage_ops = {
=======
static int pwm_regulator_enable(struct regulator_dev *dev)
{
	struct pwm_regulator_data *drvdata = rdev_get_drvdata(dev);

	if (drvdata->enb_gpio)
		gpiod_set_value_cansleep(drvdata->enb_gpio, 1);

	return pwm_enable(drvdata->pwm);
}

static int pwm_regulator_disable(struct regulator_dev *dev)
{
	struct pwm_regulator_data *drvdata = rdev_get_drvdata(dev);

	pwm_disable(drvdata->pwm);

	if (drvdata->enb_gpio)
		gpiod_set_value_cansleep(drvdata->enb_gpio, 0);

	return 0;
}

static int pwm_regulator_is_enabled(struct regulator_dev *dev)
{
	struct pwm_regulator_data *drvdata = rdev_get_drvdata(dev);

	if (drvdata->enb_gpio && !gpiod_get_value_cansleep(drvdata->enb_gpio))
		return false;

	return pwm_is_enabled(drvdata->pwm);
}

static int pwm_regulator_get_voltage(struct regulator_dev *rdev)
{
	struct pwm_regulator_data *drvdata = rdev_get_drvdata(rdev);
	unsigned int min_uV_duty = drvdata->continuous.min_uV_dutycycle;
	unsigned int max_uV_duty = drvdata->continuous.max_uV_dutycycle;
	unsigned int duty_unit = drvdata->continuous.dutycycle_unit;
	int min_uV = rdev->constraints->min_uV;
	int max_uV = rdev->constraints->max_uV;
	int diff_uV = max_uV - min_uV;
	struct pwm_state pstate;
	unsigned int diff_duty;
	unsigned int voltage;

	pwm_get_state(drvdata->pwm, &pstate);

	voltage = pwm_get_relative_duty_cycle(&pstate, duty_unit);

	/*
	 * The dutycycle for min_uV might be greater than the one for max_uV.
	 * This is happening when the user needs an inversed polarity, but the
	 * PWM device does not support inversing it in hardware.
	 */
	if (max_uV_duty < min_uV_duty) {
		voltage = min_uV_duty - voltage;
		diff_duty = min_uV_duty - max_uV_duty;
	} else {
		voltage = voltage - min_uV_duty;
		diff_duty = max_uV_duty - min_uV_duty;
	}

	voltage = DIV_ROUND_CLOSEST_ULL((u64)voltage * diff_uV, diff_duty);

	return voltage + min_uV;
}

static int pwm_regulator_set_voltage(struct regulator_dev *rdev,
				     int req_min_uV, int req_max_uV,
				     unsigned int *selector)
{
	struct pwm_regulator_data *drvdata = rdev_get_drvdata(rdev);
	unsigned int min_uV_duty = drvdata->continuous.min_uV_dutycycle;
	unsigned int max_uV_duty = drvdata->continuous.max_uV_dutycycle;
	unsigned int duty_unit = drvdata->continuous.dutycycle_unit;
	int min_uV = rdev->constraints->min_uV;
	int max_uV = rdev->constraints->max_uV;
	int diff_uV = max_uV - min_uV;
	struct pwm_state pstate;
	unsigned int diff_duty;
	unsigned int dutycycle;
	int ret;

	pwm_init_state(drvdata->pwm, &pstate);

	/*
	 * The dutycycle for min_uV might be greater than the one for max_uV.
	 * This is happening when the user needs an inversed polarity, but the
	 * PWM device does not support inversing it in hardware.
	 */
	if (max_uV_duty < min_uV_duty)
		diff_duty = min_uV_duty - max_uV_duty;
	else
		diff_duty = max_uV_duty - min_uV_duty;

	dutycycle = DIV_ROUND_CLOSEST_ULL((u64)(req_min_uV - min_uV) *
					  diff_duty,
					  diff_uV);

	if (max_uV_duty < min_uV_duty)
		dutycycle = min_uV_duty - dutycycle;
	else
		dutycycle = min_uV_duty + dutycycle;

	pwm_set_relative_duty_cycle(&pstate, dutycycle, duty_unit);

	ret = pwm_apply_state(drvdata->pwm, &pstate);
	if (ret) {
		dev_err(&rdev->dev, "Failed to configure PWM: %d\n", ret);
		return ret;
	}

	return 0;
}

static struct regulator_ops pwm_regulator_voltage_table_ops = {
>>>>>>> v4.9.227
	.set_voltage_sel = pwm_regulator_set_voltage_sel,
	.get_voltage_sel = pwm_regulator_get_voltage_sel,
	.list_voltage    = pwm_regulator_list_voltage,
	.map_voltage     = regulator_map_voltage_iterate,
<<<<<<< HEAD
};

static const struct regulator_desc pwm_regulator_desc = {
	.name		= "pwm-regulator",
	.ops		= &pwm_regulator_voltage_ops,
=======
	.enable          = pwm_regulator_enable,
	.disable         = pwm_regulator_disable,
	.is_enabled      = pwm_regulator_is_enabled,
};

static struct regulator_ops pwm_regulator_voltage_continuous_ops = {
	.get_voltage = pwm_regulator_get_voltage,
	.set_voltage = pwm_regulator_set_voltage,
	.enable          = pwm_regulator_enable,
	.disable         = pwm_regulator_disable,
	.is_enabled      = pwm_regulator_is_enabled,
};

static struct regulator_desc pwm_regulator_desc = {
	.name		= "pwm-regulator",
>>>>>>> v4.9.227
	.type		= REGULATOR_VOLTAGE,
	.owner		= THIS_MODULE,
	.supply_name    = "pwm",
};

<<<<<<< HEAD
static int pwm_regulator_probe(struct platform_device *pdev)
{
	struct pwm_regulator_data *drvdata;
	struct property *prop;
	struct regulator_dev *regulator;
	struct regulator_config config = { };
	struct device_node *np = pdev->dev.of_node;
	int length, ret;
=======
static int pwm_regulator_init_table(struct platform_device *pdev,
				    struct pwm_regulator_data *drvdata)
{
	struct device_node *np = pdev->dev.of_node;
	struct pwm_voltages *duty_cycle_table;
	unsigned int length = 0;
	int ret;

	of_find_property(np, "voltage-table", &length);

	if ((length < sizeof(*duty_cycle_table)) ||
	    (length % sizeof(*duty_cycle_table))) {
		dev_err(&pdev->dev, "voltage-table length(%d) is invalid\n",
			length);
		return -EINVAL;
	}

	duty_cycle_table = devm_kzalloc(&pdev->dev, length, GFP_KERNEL);
	if (!duty_cycle_table)
		return -ENOMEM;

	ret = of_property_read_u32_array(np, "voltage-table",
					 (u32 *)duty_cycle_table,
					 length / sizeof(u32));
	if (ret) {
		dev_err(&pdev->dev, "Failed to read voltage-table: %d\n", ret);
		return ret;
	}

	drvdata->state			= -EINVAL;
	drvdata->duty_cycle_table	= duty_cycle_table;
	memcpy(&drvdata->ops, &pwm_regulator_voltage_table_ops,
	       sizeof(drvdata->ops));
	drvdata->desc.ops = &drvdata->ops;
	drvdata->desc.n_voltages	= length / sizeof(*duty_cycle_table);

	return 0;
}

static int pwm_regulator_init_continuous(struct platform_device *pdev,
					 struct pwm_regulator_data *drvdata)
{
	u32 dutycycle_range[2] = { 0, 100 };
	u32 dutycycle_unit = 100;

	memcpy(&drvdata->ops, &pwm_regulator_voltage_continuous_ops,
	       sizeof(drvdata->ops));
	drvdata->desc.ops = &drvdata->ops;
	drvdata->desc.continuous_voltage_range = true;

	of_property_read_u32_array(pdev->dev.of_node,
				   "pwm-dutycycle-range",
				   dutycycle_range, 2);
	of_property_read_u32(pdev->dev.of_node, "pwm-dutycycle-unit",
			     &dutycycle_unit);

	if (dutycycle_range[0] > dutycycle_unit ||
	    dutycycle_range[1] > dutycycle_unit)
		return -EINVAL;

	drvdata->continuous.dutycycle_unit = dutycycle_unit;
	drvdata->continuous.min_uV_dutycycle = dutycycle_range[0];
	drvdata->continuous.max_uV_dutycycle = dutycycle_range[1];

	return 0;
}

static int pwm_regulator_probe(struct platform_device *pdev)
{
	const struct regulator_init_data *init_data;
	struct pwm_regulator_data *drvdata;
	struct regulator_dev *regulator;
	struct regulator_config config = { };
	struct device_node *np = pdev->dev.of_node;
	enum gpiod_flags gpio_flags;
	int ret;
>>>>>>> v4.9.227

	if (!np) {
		dev_err(&pdev->dev, "Device Tree node missing\n");
		return -EINVAL;
	}

	drvdata = devm_kzalloc(&pdev->dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

<<<<<<< HEAD
	memcpy(&drvdata->desc, &pwm_regulator_desc, sizeof(pwm_regulator_desc));

	/* determine the number of voltage-table */
	prop = of_find_property(np, "voltage-table", &length);
	if (!prop) {
		dev_err(&pdev->dev, "No voltage-table\n");
		return -EINVAL;
	}

	if ((length < sizeof(*drvdata->duty_cycle_table)) ||
	    (length % sizeof(*drvdata->duty_cycle_table))) {
		dev_err(&pdev->dev, "voltage-table length(%d) is invalid\n",
			length);
		return -EINVAL;
	}

	drvdata->desc.n_voltages = length / sizeof(*drvdata->duty_cycle_table);

	drvdata->duty_cycle_table = devm_kzalloc(&pdev->dev,
						 length, GFP_KERNEL);
	if (!drvdata->duty_cycle_table)
		return -ENOMEM;

	/* read voltage table from DT property */
	ret = of_property_read_u32_array(np, "voltage-table",
					 (u32 *)drvdata->duty_cycle_table,
					 length / sizeof(u32));
	if (ret < 0) {
		dev_err(&pdev->dev, "read voltage-table failed\n");
		return ret;
	}

	config.init_data = of_get_regulator_init_data(&pdev->dev, np);
	if (!config.init_data)
=======
	memcpy(&drvdata->desc, &pwm_regulator_desc, sizeof(drvdata->desc));

	if (of_find_property(np, "voltage-table", NULL))
		ret = pwm_regulator_init_table(pdev, drvdata);
	else
		ret = pwm_regulator_init_continuous(pdev, drvdata);
	if (ret)
		return ret;

	init_data = of_get_regulator_init_data(&pdev->dev, np,
					       &drvdata->desc);
	if (!init_data)
>>>>>>> v4.9.227
		return -ENOMEM;

	config.of_node = np;
	config.dev = &pdev->dev;
	config.driver_data = drvdata;
<<<<<<< HEAD

	drvdata->pwm = devm_pwm_get(&pdev->dev, NULL);
	if (IS_ERR(drvdata->pwm)) {
		dev_err(&pdev->dev, "Failed to get PWM\n");
		return PTR_ERR(drvdata->pwm);
	}

	regulator = devm_regulator_register(&pdev->dev,
					    &drvdata->desc, &config);
	if (IS_ERR(regulator)) {
		dev_err(&pdev->dev, "Failed to register regulator %s\n",
			drvdata->desc.name);
		return PTR_ERR(regulator);
=======
	config.init_data = init_data;

	drvdata->pwm = devm_pwm_get(&pdev->dev, NULL);
	if (IS_ERR(drvdata->pwm)) {
		ret = PTR_ERR(drvdata->pwm);
		dev_err(&pdev->dev, "Failed to get PWM: %d\n", ret);
		return ret;
	}

	if (init_data->constraints.boot_on || init_data->constraints.always_on)
		gpio_flags = GPIOD_OUT_HIGH;
	else
		gpio_flags = GPIOD_OUT_LOW;
	drvdata->enb_gpio = devm_gpiod_get_optional(&pdev->dev, "enable",
						    gpio_flags);
	if (IS_ERR(drvdata->enb_gpio)) {
		ret = PTR_ERR(drvdata->enb_gpio);
		dev_err(&pdev->dev, "Failed to get enable GPIO: %d\n", ret);
		return ret;
	}

	ret = pwm_adjust_config(drvdata->pwm);
	if (ret)
		return ret;

	regulator = devm_regulator_register(&pdev->dev,
					    &drvdata->desc, &config);
	if (IS_ERR(regulator)) {
		ret = PTR_ERR(regulator);
		dev_err(&pdev->dev, "Failed to register regulator %s: %d\n",
			drvdata->desc.name, ret);
		return ret;
>>>>>>> v4.9.227
	}

	return 0;
}

static const struct of_device_id pwm_of_match[] = {
	{ .compatible = "pwm-regulator" },
	{ },
};
MODULE_DEVICE_TABLE(of, pwm_of_match);

static struct platform_driver pwm_regulator_driver = {
	.driver = {
		.name		= "pwm-regulator",
<<<<<<< HEAD
		.owner		= THIS_MODULE,
=======
>>>>>>> v4.9.227
		.of_match_table = of_match_ptr(pwm_of_match),
	},
	.probe = pwm_regulator_probe,
};

module_platform_driver(pwm_regulator_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lee Jones <lee.jones@linaro.org>");
MODULE_DESCRIPTION("PWM Regulator Driver");
MODULE_ALIAS("platform:pwm-regulator");
