/*
 * Intel Low Power Subsystem PWM controller driver
 *
 * Copyright (C) 2014, Intel Corporation
 * Author: Mika Westerberg <mika.westerberg@linux.intel.com>
 * Author: Chew Kean Ho <kean.ho.chew@intel.com>
 * Author: Chang Rebecca Swee Fun <rebecca.swee.fun.chang@intel.com>
 * Author: Chew Chiau Ee <chiau.ee.chew@intel.com>
 * Author: Alan Cox <alan@linux.intel.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

<<<<<<< HEAD
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
=======
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pm_runtime.h>
#include <linux/time.h>
>>>>>>> v4.9.227

#include "pwm-lpss.h"

#define PWM				0x00000000
#define PWM_ENABLE			BIT(31)
#define PWM_SW_UPDATE			BIT(30)
#define PWM_BASE_UNIT_SHIFT		8
<<<<<<< HEAD
#define PWM_BASE_UNIT_MASK		0x00ffff00
#define PWM_ON_TIME_DIV_MASK		0x000000ff
#define PWM_DIVISION_CORRECTION		0x2
#define PWM_LIMIT			(0x8000 + PWM_DIVISION_CORRECTION)
#define NSECS_PER_SEC			1000000000UL
=======
#define PWM_ON_TIME_DIV_MASK		0x000000ff

/* Size of each PWM register space if multiple */
#define PWM_SIZE			0x400

#define MAX_PWMS			4
>>>>>>> v4.9.227

struct pwm_lpss_chip {
	struct pwm_chip chip;
	void __iomem *regs;
<<<<<<< HEAD
	unsigned long clk_rate;
=======
	const struct pwm_lpss_boardinfo *info;
	u32 saved_ctrl[MAX_PWMS];
>>>>>>> v4.9.227
};

/* BayTrail */
const struct pwm_lpss_boardinfo pwm_lpss_byt_info = {
<<<<<<< HEAD
	.clk_rate = 25000000
=======
	.clk_rate = 25000000,
	.npwm = 1,
	.base_unit_bits = 16,
>>>>>>> v4.9.227
};
EXPORT_SYMBOL_GPL(pwm_lpss_byt_info);

/* Braswell */
const struct pwm_lpss_boardinfo pwm_lpss_bsw_info = {
<<<<<<< HEAD
	.clk_rate = 19200000
};
EXPORT_SYMBOL_GPL(pwm_lpss_bsw_info);

=======
	.clk_rate = 19200000,
	.npwm = 1,
	.base_unit_bits = 16,
};
EXPORT_SYMBOL_GPL(pwm_lpss_bsw_info);

/* Broxton */
const struct pwm_lpss_boardinfo pwm_lpss_bxt_info = {
	.clk_rate = 19200000,
	.npwm = 4,
	.base_unit_bits = 22,
};
EXPORT_SYMBOL_GPL(pwm_lpss_bxt_info);

>>>>>>> v4.9.227
static inline struct pwm_lpss_chip *to_lpwm(struct pwm_chip *chip)
{
	return container_of(chip, struct pwm_lpss_chip, chip);
}

<<<<<<< HEAD
=======
static inline u32 pwm_lpss_read(const struct pwm_device *pwm)
{
	struct pwm_lpss_chip *lpwm = to_lpwm(pwm->chip);

	return readl(lpwm->regs + pwm->hwpwm * PWM_SIZE + PWM);
}

static inline void pwm_lpss_write(const struct pwm_device *pwm, u32 value)
{
	struct pwm_lpss_chip *lpwm = to_lpwm(pwm->chip);

	writel(value, lpwm->regs + pwm->hwpwm * PWM_SIZE + PWM);
}

static void pwm_lpss_update(struct pwm_device *pwm)
{
	pwm_lpss_write(pwm, pwm_lpss_read(pwm) | PWM_SW_UPDATE);
	/* Give it some time to propagate */
	usleep_range(10, 50);
}

>>>>>>> v4.9.227
static int pwm_lpss_config(struct pwm_chip *chip, struct pwm_device *pwm,
			   int duty_ns, int period_ns)
{
	struct pwm_lpss_chip *lpwm = to_lpwm(chip);
<<<<<<< HEAD
	u8 on_time_div;
	unsigned long c;
	unsigned long long base_unit, freq = NSECS_PER_SEC;
=======
	unsigned long long on_time_div;
	unsigned long c = lpwm->info->clk_rate, base_unit_range;
	unsigned long long base_unit, freq = NSEC_PER_SEC;
>>>>>>> v4.9.227
	u32 ctrl;

	do_div(freq, period_ns);

<<<<<<< HEAD
	/* The equation is: base_unit = ((freq / c) * 65536) + correction */
	base_unit = freq * 65536;

	c = lpwm->clk_rate;
	if (!c)
		return -EINVAL;

	do_div(base_unit, c);
	base_unit += PWM_DIVISION_CORRECTION;
	if (base_unit > PWM_LIMIT)
		return -EINVAL;

	if (duty_ns <= 0)
		duty_ns = 1;
	on_time_div = 255 - (255 * duty_ns / period_ns);

	ctrl = readl(lpwm->regs + PWM);
	ctrl &= ~(PWM_BASE_UNIT_MASK | PWM_ON_TIME_DIV_MASK);
	ctrl |= (u16) base_unit << PWM_BASE_UNIT_SHIFT;
	ctrl |= on_time_div;
	/* request PWM to update on next cycle */
	ctrl |= PWM_SW_UPDATE;
	writel(ctrl, lpwm->regs + PWM);
=======
	/*
	 * The equation is:
	 * base_unit = round(base_unit_range * freq / c)
	 */
	base_unit_range = BIT(lpwm->info->base_unit_bits);
	freq *= base_unit_range;

	base_unit = DIV_ROUND_CLOSEST_ULL(freq, c);

	if (duty_ns <= 0)
		duty_ns = 1;
	on_time_div = 255ULL * duty_ns;
	do_div(on_time_div, period_ns);
	on_time_div = 255ULL - on_time_div;

	pm_runtime_get_sync(chip->dev);

	ctrl = pwm_lpss_read(pwm);
	ctrl &= ~PWM_ON_TIME_DIV_MASK;
	ctrl &= ~((base_unit_range - 1) << PWM_BASE_UNIT_SHIFT);
	base_unit &= (base_unit_range - 1);
	ctrl |= (u32) base_unit << PWM_BASE_UNIT_SHIFT;
	ctrl |= on_time_div;
	pwm_lpss_write(pwm, ctrl);

	/*
	 * If the PWM is already enabled we need to notify the hardware
	 * about the change by setting PWM_SW_UPDATE.
	 */
	if (pwm_is_enabled(pwm))
		pwm_lpss_update(pwm);

	pm_runtime_put(chip->dev);
>>>>>>> v4.9.227

	return 0;
}

static int pwm_lpss_enable(struct pwm_chip *chip, struct pwm_device *pwm)
{
<<<<<<< HEAD
	struct pwm_lpss_chip *lpwm = to_lpwm(chip);
	u32 ctrl;

	ctrl = readl(lpwm->regs + PWM);
	writel(ctrl | PWM_ENABLE, lpwm->regs + PWM);

=======
	pm_runtime_get_sync(chip->dev);

	/*
	 * Hardware must first see PWM_SW_UPDATE before the PWM can be
	 * enabled.
	 */
	pwm_lpss_update(pwm);
	pwm_lpss_write(pwm, pwm_lpss_read(pwm) | PWM_ENABLE);
>>>>>>> v4.9.227
	return 0;
}

static void pwm_lpss_disable(struct pwm_chip *chip, struct pwm_device *pwm)
{
<<<<<<< HEAD
	struct pwm_lpss_chip *lpwm = to_lpwm(chip);
	u32 ctrl;

	ctrl = readl(lpwm->regs + PWM);
	writel(ctrl & ~PWM_ENABLE, lpwm->regs + PWM);
=======
	pwm_lpss_write(pwm, pwm_lpss_read(pwm) & ~PWM_ENABLE);
	pm_runtime_put(chip->dev);
>>>>>>> v4.9.227
}

static const struct pwm_ops pwm_lpss_ops = {
	.config = pwm_lpss_config,
	.enable = pwm_lpss_enable,
	.disable = pwm_lpss_disable,
	.owner = THIS_MODULE,
};

struct pwm_lpss_chip *pwm_lpss_probe(struct device *dev, struct resource *r,
				     const struct pwm_lpss_boardinfo *info)
{
	struct pwm_lpss_chip *lpwm;
<<<<<<< HEAD
	int ret;

=======
	unsigned long c;
	int ret;

	if (WARN_ON(info->npwm > MAX_PWMS))
		return ERR_PTR(-ENODEV);

>>>>>>> v4.9.227
	lpwm = devm_kzalloc(dev, sizeof(*lpwm), GFP_KERNEL);
	if (!lpwm)
		return ERR_PTR(-ENOMEM);

	lpwm->regs = devm_ioremap_resource(dev, r);
	if (IS_ERR(lpwm->regs))
		return ERR_CAST(lpwm->regs);

<<<<<<< HEAD
	lpwm->clk_rate = info->clk_rate;
	lpwm->chip.dev = dev;
	lpwm->chip.ops = &pwm_lpss_ops;
	lpwm->chip.base = -1;
	lpwm->chip.npwm = 1;
=======
	lpwm->info = info;

	c = lpwm->info->clk_rate;
	if (!c)
		return ERR_PTR(-EINVAL);

	lpwm->chip.dev = dev;
	lpwm->chip.ops = &pwm_lpss_ops;
	lpwm->chip.base = -1;
	lpwm->chip.npwm = info->npwm;
>>>>>>> v4.9.227

	ret = pwmchip_add(&lpwm->chip);
	if (ret) {
		dev_err(dev, "failed to add PWM chip: %d\n", ret);
		return ERR_PTR(ret);
	}

	return lpwm;
}
EXPORT_SYMBOL_GPL(pwm_lpss_probe);

int pwm_lpss_remove(struct pwm_lpss_chip *lpwm)
{
<<<<<<< HEAD
	u32 ctrl;

	ctrl = readl(lpwm->regs + PWM);
	writel(ctrl & ~PWM_ENABLE, lpwm->regs + PWM);

=======
	int i;

	for (i = 0; i < lpwm->info->npwm; i++) {
		if (pwm_is_enabled(&lpwm->chip.pwms[i]))
			pm_runtime_put(lpwm->chip.dev);
	}
>>>>>>> v4.9.227
	return pwmchip_remove(&lpwm->chip);
}
EXPORT_SYMBOL_GPL(pwm_lpss_remove);

<<<<<<< HEAD
=======
int pwm_lpss_suspend(struct device *dev)
{
	struct pwm_lpss_chip *lpwm = dev_get_drvdata(dev);
	int i;

	for (i = 0; i < lpwm->info->npwm; i++)
		lpwm->saved_ctrl[i] = readl(lpwm->regs + i * PWM_SIZE + PWM);

	return 0;
}
EXPORT_SYMBOL_GPL(pwm_lpss_suspend);

int pwm_lpss_resume(struct device *dev)
{
	struct pwm_lpss_chip *lpwm = dev_get_drvdata(dev);
	int i;

	for (i = 0; i < lpwm->info->npwm; i++)
		writel(lpwm->saved_ctrl[i], lpwm->regs + i * PWM_SIZE + PWM);

	return 0;
}
EXPORT_SYMBOL_GPL(pwm_lpss_resume);

>>>>>>> v4.9.227
MODULE_DESCRIPTION("PWM driver for Intel LPSS");
MODULE_AUTHOR("Mika Westerberg <mika.westerberg@linux.intel.com>");
MODULE_LICENSE("GPL v2");
