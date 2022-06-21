/*
 * A simple sysfs interface for the generic PWM framework
 *
 * Copyright (C) 2013 H Hartley Sweeten <hsweeten@visionengravers.com>
 *
 * Based on previous work by Lars Poeschel <poeschel@lemonage.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/pwm.h>

struct pwm_export {
	struct device child;
	struct pwm_device *pwm;
<<<<<<< HEAD
=======
	struct mutex lock;
>>>>>>> v4.9.227
};

static struct pwm_export *child_to_pwm_export(struct device *child)
{
	return container_of(child, struct pwm_export, child);
}

static struct pwm_device *child_to_pwm_device(struct device *child)
{
	struct pwm_export *export = child_to_pwm_export(child);

	return export->pwm;
}

<<<<<<< HEAD
static ssize_t pwm_period_show(struct device *child,
			       struct device_attribute *attr,
			       char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);

	return sprintf(buf, "%u\n", pwm->period);
}

static ssize_t pwm_period_store(struct device *child,
				struct device_attribute *attr,
				const char *buf, size_t size)
{
	struct pwm_device *pwm = child_to_pwm_device(child);
=======
static ssize_t period_show(struct device *child,
			   struct device_attribute *attr,
			   char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return sprintf(buf, "%u\n", state.period);
}

static ssize_t period_store(struct device *child,
			    struct device_attribute *attr,
			    const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	struct pwm_state state;
>>>>>>> v4.9.227
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

<<<<<<< HEAD
	ret = pwm_config(pwm, pwm->duty_cycle, val);
=======
	mutex_lock(&export->lock);
	pwm_get_state(pwm, &state);
	state.period = val;
	ret = pwm_apply_state(pwm, &state);
	mutex_unlock(&export->lock);
>>>>>>> v4.9.227

	return ret ? : size;
}

<<<<<<< HEAD
static ssize_t pwm_duty_cycle_show(struct device *child,
				   struct device_attribute *attr,
				   char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);

	return sprintf(buf, "%u\n", pwm->duty_cycle);
}

static ssize_t pwm_duty_cycle_store(struct device *child,
				    struct device_attribute *attr,
				    const char *buf, size_t size)
{
	struct pwm_device *pwm = child_to_pwm_device(child);
=======
static ssize_t duty_cycle_show(struct device *child,
			       struct device_attribute *attr,
			       char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return sprintf(buf, "%u\n", state.duty_cycle);
}

static ssize_t duty_cycle_store(struct device *child,
				struct device_attribute *attr,
				const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	struct pwm_state state;
>>>>>>> v4.9.227
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

<<<<<<< HEAD
	ret = pwm_config(pwm, val, pwm->period);
=======
	mutex_lock(&export->lock);
	pwm_get_state(pwm, &state);
	state.duty_cycle = val;
	ret = pwm_apply_state(pwm, &state);
	mutex_unlock(&export->lock);
>>>>>>> v4.9.227

	return ret ? : size;
}

<<<<<<< HEAD
static ssize_t pwm_enable_show(struct device *child,
			       struct device_attribute *attr,
			       char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	int enabled = test_bit(PWMF_ENABLED, &pwm->flags);

	return sprintf(buf, "%d\n", enabled);
}

static ssize_t pwm_enable_store(struct device *child,
				struct device_attribute *attr,
				const char *buf, size_t size)
{
	struct pwm_device *pwm = child_to_pwm_device(child);
=======
static ssize_t enable_show(struct device *child,
			   struct device_attribute *attr,
			   char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return sprintf(buf, "%d\n", state.enabled);
}

static ssize_t enable_store(struct device *child,
			    struct device_attribute *attr,
			    const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	struct pwm_state state;
>>>>>>> v4.9.227
	int val, ret;

	ret = kstrtoint(buf, 0, &val);
	if (ret)
		return ret;

<<<<<<< HEAD
	switch (val) {
	case 0:
		pwm_disable(pwm);
		break;
	case 1:
		ret = pwm_enable(pwm);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret ? : size;
}

static ssize_t pwm_polarity_show(struct device *child,
				 struct device_attribute *attr,
				 char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);

	return sprintf(buf, "%s\n", pwm->polarity ? "inversed" : "normal");
}

static ssize_t pwm_polarity_store(struct device *child,
				  struct device_attribute *attr,
				  const char *buf, size_t size)
{
	struct pwm_device *pwm = child_to_pwm_device(child);
	enum pwm_polarity polarity;
=======
	mutex_lock(&export->lock);

	pwm_get_state(pwm, &state);

	switch (val) {
	case 0:
		state.enabled = false;
		break;
	case 1:
		state.enabled = true;
		break;
	default:
		ret = -EINVAL;
		goto unlock;
	}

	ret = pwm_apply_state(pwm, &state);

unlock:
	mutex_unlock(&export->lock);
	return ret ? : size;
}

static ssize_t polarity_show(struct device *child,
			     struct device_attribute *attr,
			     char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	const char *polarity = "unknown";
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	switch (state.polarity) {
	case PWM_POLARITY_NORMAL:
		polarity = "normal";
		break;

	case PWM_POLARITY_INVERSED:
		polarity = "inversed";
		break;
	}

	return sprintf(buf, "%s\n", polarity);
}

static ssize_t polarity_store(struct device *child,
			      struct device_attribute *attr,
			      const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	enum pwm_polarity polarity;
	struct pwm_state state;
>>>>>>> v4.9.227
	int ret;

	if (sysfs_streq(buf, "normal"))
		polarity = PWM_POLARITY_NORMAL;
	else if (sysfs_streq(buf, "inversed"))
		polarity = PWM_POLARITY_INVERSED;
	else
		return -EINVAL;

<<<<<<< HEAD
	ret = pwm_set_polarity(pwm, polarity);
=======
	mutex_lock(&export->lock);
	pwm_get_state(pwm, &state);
	state.polarity = polarity;
	ret = pwm_apply_state(pwm, &state);
	mutex_unlock(&export->lock);
>>>>>>> v4.9.227

	return ret ? : size;
}

<<<<<<< HEAD
static DEVICE_ATTR(period, 0644, pwm_period_show, pwm_period_store);
static DEVICE_ATTR(duty_cycle, 0644, pwm_duty_cycle_show, pwm_duty_cycle_store);
static DEVICE_ATTR(enable, 0644, pwm_enable_show, pwm_enable_store);
static DEVICE_ATTR(polarity, 0644, pwm_polarity_show, pwm_polarity_store);
=======
static ssize_t capture_show(struct device *child,
			    struct device_attribute *attr,
			    char *buf)
{
	struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_capture result;
	int ret;

	ret = pwm_capture(pwm, &result, jiffies_to_msecs(HZ));
	if (ret)
		return ret;

	return sprintf(buf, "%u %u\n", result.period, result.duty_cycle);
}

static DEVICE_ATTR_RW(period);
static DEVICE_ATTR_RW(duty_cycle);
static DEVICE_ATTR_RW(enable);
static DEVICE_ATTR_RW(polarity);
static DEVICE_ATTR_RO(capture);
>>>>>>> v4.9.227

static struct attribute *pwm_attrs[] = {
	&dev_attr_period.attr,
	&dev_attr_duty_cycle.attr,
	&dev_attr_enable.attr,
	&dev_attr_polarity.attr,
<<<<<<< HEAD
=======
	&dev_attr_capture.attr,
>>>>>>> v4.9.227
	NULL
};
ATTRIBUTE_GROUPS(pwm);

static void pwm_export_release(struct device *child)
{
	struct pwm_export *export = child_to_pwm_export(child);

	kfree(export);
}

static int pwm_export_child(struct device *parent, struct pwm_device *pwm)
{
	struct pwm_export *export;
	int ret;

	if (test_and_set_bit(PWMF_EXPORTED, &pwm->flags))
		return -EBUSY;

	export = kzalloc(sizeof(*export), GFP_KERNEL);
	if (!export) {
		clear_bit(PWMF_EXPORTED, &pwm->flags);
		return -ENOMEM;
	}

	export->pwm = pwm;
<<<<<<< HEAD
=======
	mutex_init(&export->lock);
>>>>>>> v4.9.227

	export->child.release = pwm_export_release;
	export->child.parent = parent;
	export->child.devt = MKDEV(0, 0);
	export->child.groups = pwm_groups;
	dev_set_name(&export->child, "pwm%u", pwm->hwpwm);

	ret = device_register(&export->child);
	if (ret) {
		clear_bit(PWMF_EXPORTED, &pwm->flags);
		kfree(export);
		return ret;
	}

	return 0;
}

static int pwm_unexport_match(struct device *child, void *data)
{
	return child_to_pwm_device(child) == data;
}

static int pwm_unexport_child(struct device *parent, struct pwm_device *pwm)
{
	struct device *child;

	if (!test_and_clear_bit(PWMF_EXPORTED, &pwm->flags))
		return -ENODEV;

	child = device_find_child(parent, pwm, pwm_unexport_match);
	if (!child)
		return -ENODEV;

	/* for device_find_child() */
	put_device(child);
	device_unregister(child);
	pwm_put(pwm);

	return 0;
}

<<<<<<< HEAD
static ssize_t pwm_export_store(struct device *parent,
				struct device_attribute *attr,
				const char *buf, size_t len)
=======
static ssize_t export_store(struct device *parent,
			    struct device_attribute *attr,
			    const char *buf, size_t len)
>>>>>>> v4.9.227
{
	struct pwm_chip *chip = dev_get_drvdata(parent);
	struct pwm_device *pwm;
	unsigned int hwpwm;
	int ret;

	ret = kstrtouint(buf, 0, &hwpwm);
	if (ret < 0)
		return ret;

	if (hwpwm >= chip->npwm)
		return -ENODEV;

	pwm = pwm_request_from_chip(chip, hwpwm, "sysfs");
	if (IS_ERR(pwm))
		return PTR_ERR(pwm);

	ret = pwm_export_child(parent, pwm);
	if (ret < 0)
		pwm_put(pwm);

	return ret ? : len;
}
<<<<<<< HEAD
static DEVICE_ATTR(export, 0200, NULL, pwm_export_store);

static ssize_t pwm_unexport_store(struct device *parent,
				  struct device_attribute *attr,
				  const char *buf, size_t len)
=======
static DEVICE_ATTR_WO(export);

static ssize_t unexport_store(struct device *parent,
			      struct device_attribute *attr,
			      const char *buf, size_t len)
>>>>>>> v4.9.227
{
	struct pwm_chip *chip = dev_get_drvdata(parent);
	unsigned int hwpwm;
	int ret;

	ret = kstrtouint(buf, 0, &hwpwm);
	if (ret < 0)
		return ret;

	if (hwpwm >= chip->npwm)
		return -ENODEV;

	ret = pwm_unexport_child(parent, &chip->pwms[hwpwm]);

	return ret ? : len;
}
<<<<<<< HEAD
static DEVICE_ATTR(unexport, 0200, NULL, pwm_unexport_store);
=======
static DEVICE_ATTR_WO(unexport);
>>>>>>> v4.9.227

static ssize_t npwm_show(struct device *parent, struct device_attribute *attr,
			 char *buf)
{
	const struct pwm_chip *chip = dev_get_drvdata(parent);

	return sprintf(buf, "%u\n", chip->npwm);
}
static DEVICE_ATTR_RO(npwm);

static struct attribute *pwm_chip_attrs[] = {
	&dev_attr_export.attr,
	&dev_attr_unexport.attr,
	&dev_attr_npwm.attr,
	NULL,
};
ATTRIBUTE_GROUPS(pwm_chip);

static struct class pwm_class = {
<<<<<<< HEAD
	.name		= "pwm",
	.owner		= THIS_MODULE,
	.dev_groups	= pwm_chip_groups,
=======
	.name = "pwm",
	.owner = THIS_MODULE,
	.dev_groups = pwm_chip_groups,
>>>>>>> v4.9.227
};

static int pwmchip_sysfs_match(struct device *parent, const void *data)
{
	return dev_get_drvdata(parent) == data;
}

void pwmchip_sysfs_export(struct pwm_chip *chip)
{
	struct device *parent;

	/*
	 * If device_create() fails the pwm_chip is still usable by
	 * the kernel its just not exported.
	 */
	parent = device_create(&pwm_class, chip->dev, MKDEV(0, 0), chip,
			       "pwmchip%d", chip->base);
	if (IS_ERR(parent)) {
		dev_warn(chip->dev,
			 "device_create failed for pwm_chip sysfs export\n");
	}
}

void pwmchip_sysfs_unexport(struct pwm_chip *chip)
{
	struct device *parent;
<<<<<<< HEAD

	parent = class_find_device(&pwm_class, NULL, chip,
				   pwmchip_sysfs_match);
	if (parent) {
		/* for class_find_device() */
		put_device(parent);
		device_unregister(parent);
	}
}

void pwmchip_sysfs_unexport_children(struct pwm_chip *chip)
{
	struct device *parent;
=======
>>>>>>> v4.9.227
	unsigned int i;

	parent = class_find_device(&pwm_class, NULL, chip,
				   pwmchip_sysfs_match);
	if (!parent)
		return;

	for (i = 0; i < chip->npwm; i++) {
		struct pwm_device *pwm = &chip->pwms[i];

		if (test_bit(PWMF_EXPORTED, &pwm->flags))
			pwm_unexport_child(parent, pwm);
	}
<<<<<<< HEAD
=======

	put_device(parent);
	device_unregister(parent);
>>>>>>> v4.9.227
}

static int __init pwm_sysfs_init(void)
{
	return class_register(&pwm_class);
}
subsys_initcall(pwm_sysfs_init);
