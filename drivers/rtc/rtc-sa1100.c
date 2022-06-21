/*
 * Real Time Clock interface for StrongARM SA1x00 and XScale PXA2xx
 *
 * Copyright (c) 2000 Nils Faerber
 *
 * Based on rtc.c by Paul Gortmaker
 *
 * Original Driver by Nils Faerber <nils@kernelconcepts.de>
 *
 * Modifications from:
 *   CIH <cih@coventive.com>
 *   Nicolas Pitre <nico@fluxnic.net>
 *   Andrew Christian <andrew.christian@hp.com>
 *
 * Converted to the RTC subsystem and Driver Model
 *   by Richard Purdie <rpurdie@rpsys.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/rtc.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/of.h>
#include <linux/pm.h>
#include <linux/bitops.h>
#include <linux/io.h>

<<<<<<< HEAD
#include <mach/hardware.h>
#include <mach/irqs.h>

#if defined(CONFIG_ARCH_PXA) || defined(CONFIG_ARCH_MMP)
#include <mach/regs-rtc.h>
#endif
=======
#define RTSR_HZE		BIT(3)	/* HZ interrupt enable */
#define RTSR_ALE		BIT(2)	/* RTC alarm interrupt enable */
#define RTSR_HZ			BIT(1)	/* HZ rising-edge detected */
#define RTSR_AL			BIT(0)	/* RTC alarm detected */

#include "rtc-sa1100.h"
>>>>>>> v4.9.227

#define RTC_DEF_DIVIDER		(32768 - 1)
#define RTC_DEF_TRIM		0
#define RTC_FREQ		1024

<<<<<<< HEAD
struct sa1100_rtc {
	spinlock_t		lock;
	int			irq_1hz;
	int			irq_alarm;
	struct rtc_device	*rtc;
	struct clk		*clk;
};
=======
>>>>>>> v4.9.227

static irqreturn_t sa1100_rtc_interrupt(int irq, void *dev_id)
{
	struct sa1100_rtc *info = dev_get_drvdata(dev_id);
	struct rtc_device *rtc = info->rtc;
	unsigned int rtsr;
	unsigned long events = 0;

	spin_lock(&info->lock);

<<<<<<< HEAD
	rtsr = RTSR;
	/* clear interrupt sources */
	RTSR = 0;
=======
	rtsr = readl_relaxed(info->rtsr);
	/* clear interrupt sources */
	writel_relaxed(0, info->rtsr);
>>>>>>> v4.9.227
	/* Fix for a nasty initialization problem the in SA11xx RTSR register.
	 * See also the comments in sa1100_rtc_probe(). */
	if (rtsr & (RTSR_ALE | RTSR_HZE)) {
		/* This is the original code, before there was the if test
		 * above. This code does not clear interrupts that were not
		 * enabled. */
<<<<<<< HEAD
		RTSR = (RTSR_AL | RTSR_HZ) & (rtsr >> 2);
=======
		writel_relaxed((RTSR_AL | RTSR_HZ) & (rtsr >> 2), info->rtsr);
>>>>>>> v4.9.227
	} else {
		/* For some reason, it is possible to enter this routine
		 * without interruptions enabled, it has been tested with
		 * several units (Bug in SA11xx chip?).
		 *
		 * This situation leads to an infinite "loop" of interrupt
		 * routine calling and as a result the processor seems to
		 * lock on its first call to open(). */
<<<<<<< HEAD
		RTSR = RTSR_AL | RTSR_HZ;
=======
		writel_relaxed(RTSR_AL | RTSR_HZ, info->rtsr);
>>>>>>> v4.9.227
	}

	/* clear alarm interrupt if it has occurred */
	if (rtsr & RTSR_AL)
		rtsr &= ~RTSR_ALE;
<<<<<<< HEAD
	RTSR = rtsr & (RTSR_ALE | RTSR_HZE);
=======
	writel_relaxed(rtsr & (RTSR_ALE | RTSR_HZE), info->rtsr);
>>>>>>> v4.9.227

	/* update irq data & counter */
	if (rtsr & RTSR_AL)
		events |= RTC_AF | RTC_IRQF;
	if (rtsr & RTSR_HZ)
		events |= RTC_UF | RTC_IRQF;

	rtc_update_irq(rtc, 1, events);

	spin_unlock(&info->lock);

	return IRQ_HANDLED;
}

static int sa1100_rtc_open(struct device *dev)
{
	struct sa1100_rtc *info = dev_get_drvdata(dev);
	struct rtc_device *rtc = info->rtc;
	int ret;

	ret = request_irq(info->irq_1hz, sa1100_rtc_interrupt, 0, "rtc 1Hz", dev);
	if (ret) {
		dev_err(dev, "IRQ %d already in use.\n", info->irq_1hz);
		goto fail_ui;
	}
	ret = request_irq(info->irq_alarm, sa1100_rtc_interrupt, 0, "rtc Alrm", dev);
	if (ret) {
		dev_err(dev, "IRQ %d already in use.\n", info->irq_alarm);
		goto fail_ai;
	}
	rtc->max_user_freq = RTC_FREQ;
	rtc_irq_set_freq(rtc, NULL, RTC_FREQ);

	return 0;

 fail_ai:
	free_irq(info->irq_1hz, dev);
 fail_ui:
	clk_disable_unprepare(info->clk);
	return ret;
}

static void sa1100_rtc_release(struct device *dev)
{
	struct sa1100_rtc *info = dev_get_drvdata(dev);

	spin_lock_irq(&info->lock);
<<<<<<< HEAD
	RTSR = 0;
=======
	writel_relaxed(0, info->rtsr);
>>>>>>> v4.9.227
	spin_unlock_irq(&info->lock);

	free_irq(info->irq_alarm, dev);
	free_irq(info->irq_1hz, dev);
}

static int sa1100_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
<<<<<<< HEAD
	struct sa1100_rtc *info = dev_get_drvdata(dev);

	spin_lock_irq(&info->lock);
	if (enabled)
		RTSR |= RTSR_ALE;
	else
		RTSR &= ~RTSR_ALE;
=======
	u32 rtsr;
	struct sa1100_rtc *info = dev_get_drvdata(dev);

	spin_lock_irq(&info->lock);
	rtsr = readl_relaxed(info->rtsr);
	if (enabled)
		rtsr |= RTSR_ALE;
	else
		rtsr &= ~RTSR_ALE;
	writel_relaxed(rtsr, info->rtsr);
>>>>>>> v4.9.227
	spin_unlock_irq(&info->lock);
	return 0;
}

static int sa1100_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
<<<<<<< HEAD
	rtc_time_to_tm(RCNR, tm);
=======
	struct sa1100_rtc *info = dev_get_drvdata(dev);

	rtc_time_to_tm(readl_relaxed(info->rcnr), tm);
>>>>>>> v4.9.227
	return 0;
}

static int sa1100_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
<<<<<<< HEAD
=======
	struct sa1100_rtc *info = dev_get_drvdata(dev);
>>>>>>> v4.9.227
	unsigned long time;
	int ret;

	ret = rtc_tm_to_time(tm, &time);
	if (ret == 0)
<<<<<<< HEAD
		RCNR = time;
=======
		writel_relaxed(time, info->rcnr);
>>>>>>> v4.9.227
	return ret;
}

static int sa1100_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	u32	rtsr;
<<<<<<< HEAD

	rtsr = RTSR;
=======
	struct sa1100_rtc *info = dev_get_drvdata(dev);

	rtsr = readl_relaxed(info->rtsr);
>>>>>>> v4.9.227
	alrm->enabled = (rtsr & RTSR_ALE) ? 1 : 0;
	alrm->pending = (rtsr & RTSR_AL) ? 1 : 0;
	return 0;
}

static int sa1100_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct sa1100_rtc *info = dev_get_drvdata(dev);
	unsigned long time;
	int ret;

	spin_lock_irq(&info->lock);
	ret = rtc_tm_to_time(&alrm->time, &time);
	if (ret != 0)
		goto out;
<<<<<<< HEAD
	RTSR = RTSR & (RTSR_HZE|RTSR_ALE|RTSR_AL);
	RTAR = time;
	if (alrm->enabled)
		RTSR |= RTSR_ALE;
	else
		RTSR &= ~RTSR_ALE;
=======
	writel_relaxed(readl_relaxed(info->rtsr) &
		(RTSR_HZE | RTSR_ALE | RTSR_AL), info->rtsr);
	writel_relaxed(time, info->rtar);
	if (alrm->enabled)
		writel_relaxed(readl_relaxed(info->rtsr) | RTSR_ALE, info->rtsr);
	else
		writel_relaxed(readl_relaxed(info->rtsr) & ~RTSR_ALE, info->rtsr);
>>>>>>> v4.9.227
out:
	spin_unlock_irq(&info->lock);

	return ret;
}

static int sa1100_rtc_proc(struct device *dev, struct seq_file *seq)
{
<<<<<<< HEAD
	seq_printf(seq, "trim/divider\t\t: 0x%08x\n", (u32) RTTR);
	seq_printf(seq, "RTSR\t\t\t: 0x%08x\n", (u32)RTSR);
=======
	struct sa1100_rtc *info = dev_get_drvdata(dev);

	seq_printf(seq, "trim/divider\t\t: 0x%08x\n", readl_relaxed(info->rttr));
	seq_printf(seq, "RTSR\t\t\t: 0x%08x\n", readl_relaxed(info->rtsr));
>>>>>>> v4.9.227

	return 0;
}

static const struct rtc_class_ops sa1100_rtc_ops = {
	.open = sa1100_rtc_open,
	.release = sa1100_rtc_release,
	.read_time = sa1100_rtc_read_time,
	.set_time = sa1100_rtc_set_time,
	.read_alarm = sa1100_rtc_read_alarm,
	.set_alarm = sa1100_rtc_set_alarm,
	.proc = sa1100_rtc_proc,
	.alarm_irq_enable = sa1100_rtc_alarm_irq_enable,
};

<<<<<<< HEAD
static int sa1100_rtc_probe(struct platform_device *pdev)
{
	struct rtc_device *rtc;
	struct sa1100_rtc *info;
	int irq_1hz, irq_alarm, ret = 0;

	irq_1hz = platform_get_irq_byname(pdev, "rtc 1Hz");
	irq_alarm = platform_get_irq_byname(pdev, "rtc alarm");
	if (irq_1hz < 0 || irq_alarm < 0)
		return -ENODEV;

	info = devm_kzalloc(&pdev->dev, sizeof(struct sa1100_rtc), GFP_KERNEL);
	if (!info)
		return -ENOMEM;
=======
int sa1100_rtc_init(struct platform_device *pdev, struct sa1100_rtc *info)
{
	struct rtc_device *rtc;
	int ret;

	spin_lock_init(&info->lock);

>>>>>>> v4.9.227
	info->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(info->clk)) {
		dev_err(&pdev->dev, "failed to find rtc clock source\n");
		return PTR_ERR(info->clk);
	}
<<<<<<< HEAD
	info->irq_1hz = irq_1hz;
	info->irq_alarm = irq_alarm;
	spin_lock_init(&info->lock);
	platform_set_drvdata(pdev, info);
=======
>>>>>>> v4.9.227

	ret = clk_prepare_enable(info->clk);
	if (ret)
		return ret;
	/*
	 * According to the manual we should be able to let RTTR be zero
	 * and then a default diviser for a 32.768KHz clock is used.
	 * Apparently this doesn't work, at least for my SA1110 rev 5.
	 * If the clock divider is uninitialized then reset it to the
	 * default value to get the 1Hz clock.
	 */
<<<<<<< HEAD
	if (RTTR == 0) {
		RTTR = RTC_DEF_DIVIDER + (RTC_DEF_TRIM << 16);
		dev_warn(&pdev->dev, "warning: "
			"initializing default clock divider/trim value\n");
		/* The current RTC value probably doesn't make sense either */
		RCNR = 0;
	}

	device_init_wakeup(&pdev->dev, 1);

	rtc = devm_rtc_device_register(&pdev->dev, pdev->name, &sa1100_rtc_ops,
					THIS_MODULE);

	if (IS_ERR(rtc)) {
		ret = PTR_ERR(rtc);
		goto err_dev;
=======
	if (readl_relaxed(info->rttr) == 0) {
		writel_relaxed(RTC_DEF_DIVIDER + (RTC_DEF_TRIM << 16), info->rttr);
		dev_warn(&pdev->dev, "warning: "
			"initializing default clock divider/trim value\n");
		/* The current RTC value probably doesn't make sense either */
		writel_relaxed(0, info->rcnr);
	}

	rtc = devm_rtc_device_register(&pdev->dev, pdev->name, &sa1100_rtc_ops,
					THIS_MODULE);
	if (IS_ERR(rtc)) {
		clk_disable_unprepare(info->clk);
		return PTR_ERR(rtc);
>>>>>>> v4.9.227
	}
	info->rtc = rtc;

	/* Fix for a nasty initialization problem the in SA11xx RTSR register.
	 * See also the comments in sa1100_rtc_interrupt().
	 *
	 * Sometimes bit 1 of the RTSR (RTSR_HZ) will wake up 1, which means an
	 * interrupt pending, even though interrupts were never enabled.
	 * In this case, this bit it must be reset before enabling
	 * interruptions to avoid a nonexistent interrupt to occur.
	 *
	 * In principle, the same problem would apply to bit 0, although it has
	 * never been observed to happen.
	 *
	 * This issue is addressed both here and in sa1100_rtc_interrupt().
	 * If the issue is not addressed here, in the times when the processor
	 * wakes up with the bit set there will be one spurious interrupt.
	 *
	 * The issue is also dealt with in sa1100_rtc_interrupt() to be on the
	 * safe side, once the condition that lead to this strange
	 * initialization is unknown and could in principle happen during
	 * normal processing.
	 *
	 * Notice that clearing bit 1 and 0 is accomplished by writting ONES to
	 * the corresponding bits in RTSR. */
<<<<<<< HEAD
	RTSR = RTSR_AL | RTSR_HZ;

	return 0;
err_dev:
	clk_disable_unprepare(info->clk);
	return ret;
=======
	writel_relaxed(RTSR_AL | RTSR_HZ, info->rtsr);

	return 0;
}
EXPORT_SYMBOL_GPL(sa1100_rtc_init);

static int sa1100_rtc_probe(struct platform_device *pdev)
{
	struct sa1100_rtc *info;
	struct resource *iores;
	void __iomem *base;
	int irq_1hz, irq_alarm;

	irq_1hz = platform_get_irq_byname(pdev, "rtc 1Hz");
	irq_alarm = platform_get_irq_byname(pdev, "rtc alarm");
	if (irq_1hz < 0 || irq_alarm < 0)
		return -ENODEV;

	info = devm_kzalloc(&pdev->dev, sizeof(struct sa1100_rtc), GFP_KERNEL);
	if (!info)
		return -ENOMEM;
	info->irq_1hz = irq_1hz;
	info->irq_alarm = irq_alarm;

	iores = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	base = devm_ioremap_resource(&pdev->dev, iores);
	if (IS_ERR(base))
		return PTR_ERR(base);

	if (IS_ENABLED(CONFIG_ARCH_SA1100) ||
	    of_device_is_compatible(pdev->dev.of_node, "mrvl,sa1100-rtc")) {
		info->rcnr = base + 0x04;
		info->rtsr = base + 0x10;
		info->rtar = base + 0x00;
		info->rttr = base + 0x08;
	} else {
		info->rcnr = base + 0x0;
		info->rtsr = base + 0x8;
		info->rtar = base + 0x4;
		info->rttr = base + 0xc;
	}

	platform_set_drvdata(pdev, info);
	device_init_wakeup(&pdev->dev, 1);

	return sa1100_rtc_init(pdev, info);
>>>>>>> v4.9.227
}

static int sa1100_rtc_remove(struct platform_device *pdev)
{
	struct sa1100_rtc *info = platform_get_drvdata(pdev);

	if (info)
		clk_disable_unprepare(info->clk);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int sa1100_rtc_suspend(struct device *dev)
{
	struct sa1100_rtc *info = dev_get_drvdata(dev);
	if (device_may_wakeup(dev))
		enable_irq_wake(info->irq_alarm);
	return 0;
}

static int sa1100_rtc_resume(struct device *dev)
{
	struct sa1100_rtc *info = dev_get_drvdata(dev);
	if (device_may_wakeup(dev))
		disable_irq_wake(info->irq_alarm);
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(sa1100_rtc_pm_ops, sa1100_rtc_suspend,
			sa1100_rtc_resume);

#ifdef CONFIG_OF
static const struct of_device_id sa1100_rtc_dt_ids[] = {
	{ .compatible = "mrvl,sa1100-rtc", },
	{ .compatible = "mrvl,mmp-rtc", },
	{}
};
MODULE_DEVICE_TABLE(of, sa1100_rtc_dt_ids);
#endif

static struct platform_driver sa1100_rtc_driver = {
	.probe		= sa1100_rtc_probe,
	.remove		= sa1100_rtc_remove,
	.driver		= {
		.name	= "sa1100-rtc",
		.pm	= &sa1100_rtc_pm_ops,
		.of_match_table = of_match_ptr(sa1100_rtc_dt_ids),
	},
};

module_platform_driver(sa1100_rtc_driver);

MODULE_AUTHOR("Richard Purdie <rpurdie@rpsys.net>");
MODULE_DESCRIPTION("SA11x0/PXA2xx Realtime Clock Driver (RTC)");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:sa1100-rtc");
