/*
 * SiRFSoC Real Time Clock interface for Linux
 *
 * Copyright (c) 2013 Cambridge Silicon Radio Limited, a CSR plc group company.
 *
 * Licensed under GPLv2 or later.
 */

#include <linux/module.h>
#include <linux/err.h>
#include <linux/rtc.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of.h>
<<<<<<< HEAD
=======
#include <linux/regmap.h>
>>>>>>> v4.9.227
#include <linux/rtc/sirfsoc_rtciobrg.h>


#define RTC_CN			0x00
#define RTC_ALARM0		0x04
#define RTC_ALARM1		0x18
#define RTC_STATUS		0x08
#define RTC_SW_VALUE            0x40
#define SIRFSOC_RTC_AL1E	(1<<6)
#define SIRFSOC_RTC_AL1		(1<<4)
#define SIRFSOC_RTC_HZE		(1<<3)
#define SIRFSOC_RTC_AL0E	(1<<2)
#define SIRFSOC_RTC_HZ		(1<<1)
#define SIRFSOC_RTC_AL0		(1<<0)
#define RTC_DIV			0x0c
#define RTC_DEEP_CTRL		0x14
#define RTC_CLOCK_SWITCH	0x1c
#define SIRFSOC_RTC_CLK		0x03	/* others are reserved */

/* Refer to RTC DIV switch */
#define RTC_HZ			16

/* This macro is also defined in arch/arm/plat-sirfsoc/cpu.c */
#define RTC_SHIFT		4

#define INTR_SYSRTC_CN		0x48

struct sirfsoc_rtc_drv {
	struct rtc_device	*rtc;
	u32			rtc_base;
	u32			irq;
	unsigned		irq_wake;
	/* Overflow for every 8 years extra time */
	u32			overflow_rtc;
<<<<<<< HEAD
=======
	spinlock_t		lock;
	struct regmap *regmap;
>>>>>>> v4.9.227
#ifdef CONFIG_PM
	u32		saved_counter;
	u32		saved_overflow_rtc;
#endif
};

<<<<<<< HEAD
=======
static u32 sirfsoc_rtc_readl(struct sirfsoc_rtc_drv *rtcdrv, u32 offset)
{
	u32 val;

	regmap_read(rtcdrv->regmap, rtcdrv->rtc_base + offset, &val);
	return val;
}

static void sirfsoc_rtc_writel(struct sirfsoc_rtc_drv *rtcdrv,
			       u32 offset, u32 val)
{
	regmap_write(rtcdrv->regmap, rtcdrv->rtc_base + offset, val);
}

>>>>>>> v4.9.227
static int sirfsoc_rtc_read_alarm(struct device *dev,
		struct rtc_wkalrm *alrm)
{
	unsigned long rtc_alarm, rtc_count;
	struct sirfsoc_rtc_drv *rtcdrv;

	rtcdrv = dev_get_drvdata(dev);

<<<<<<< HEAD
	local_irq_disable();

	rtc_count = sirfsoc_rtc_iobrg_readl(rtcdrv->rtc_base + RTC_CN);

	rtc_alarm = sirfsoc_rtc_iobrg_readl(rtcdrv->rtc_base + RTC_ALARM0);
=======
	spin_lock_irq(&rtcdrv->lock);

	rtc_count = sirfsoc_rtc_readl(rtcdrv, RTC_CN);

	rtc_alarm = sirfsoc_rtc_readl(rtcdrv, RTC_ALARM0);
>>>>>>> v4.9.227
	memset(alrm, 0, sizeof(struct rtc_wkalrm));

	/*
	 * assume alarm interval not beyond one round counter overflow_rtc:
	 * 0->0xffffffff
	 */
	/* if alarm is in next overflow cycle */
	if (rtc_count > rtc_alarm)
		rtc_time_to_tm((rtcdrv->overflow_rtc + 1)
				<< (BITS_PER_LONG - RTC_SHIFT)
				| rtc_alarm >> RTC_SHIFT, &(alrm->time));
	else
		rtc_time_to_tm(rtcdrv->overflow_rtc
				<< (BITS_PER_LONG - RTC_SHIFT)
				| rtc_alarm >> RTC_SHIFT, &(alrm->time));
<<<<<<< HEAD
	if (sirfsoc_rtc_iobrg_readl(
			rtcdrv->rtc_base + RTC_STATUS) & SIRFSOC_RTC_AL0E)
		alrm->enabled = 1;
	local_irq_enable();
=======
	if (sirfsoc_rtc_readl(rtcdrv, RTC_STATUS) & SIRFSOC_RTC_AL0E)
		alrm->enabled = 1;

	spin_unlock_irq(&rtcdrv->lock);
>>>>>>> v4.9.227

	return 0;
}

static int sirfsoc_rtc_set_alarm(struct device *dev,
		struct rtc_wkalrm *alrm)
{
	unsigned long rtc_status_reg, rtc_alarm;
	struct sirfsoc_rtc_drv *rtcdrv;
	rtcdrv = dev_get_drvdata(dev);

	if (alrm->enabled) {
		rtc_tm_to_time(&(alrm->time), &rtc_alarm);

<<<<<<< HEAD
		local_irq_disable();

		rtc_status_reg = sirfsoc_rtc_iobrg_readl(
				rtcdrv->rtc_base + RTC_STATUS);
=======
		spin_lock_irq(&rtcdrv->lock);

		rtc_status_reg = sirfsoc_rtc_readl(rtcdrv, RTC_STATUS);
>>>>>>> v4.9.227
		if (rtc_status_reg & SIRFSOC_RTC_AL0E) {
			/*
			 * An ongoing alarm in progress - ingore it and not
			 * to return EBUSY
			 */
			dev_info(dev, "An old alarm was set, will be replaced by a new one\n");
		}

<<<<<<< HEAD
		sirfsoc_rtc_iobrg_writel(
			rtc_alarm << RTC_SHIFT, rtcdrv->rtc_base + RTC_ALARM0);
=======
		sirfsoc_rtc_writel(rtcdrv, RTC_ALARM0, rtc_alarm << RTC_SHIFT);
>>>>>>> v4.9.227
		rtc_status_reg &= ~0x07; /* mask out the lower status bits */
		/*
		 * This bit RTC_AL sets it as a wake-up source for Sleep Mode
		 * Writing 1 into this bit will clear it
		 */
		rtc_status_reg |= SIRFSOC_RTC_AL0;
		/* enable the RTC alarm interrupt */
		rtc_status_reg |= SIRFSOC_RTC_AL0E;
<<<<<<< HEAD
		sirfsoc_rtc_iobrg_writel(
			rtc_status_reg, rtcdrv->rtc_base + RTC_STATUS);
		local_irq_enable();
=======
		sirfsoc_rtc_writel(rtcdrv, RTC_STATUS, rtc_status_reg);

		spin_unlock_irq(&rtcdrv->lock);
>>>>>>> v4.9.227
	} else {
		/*
		 * if this function was called with enabled=0
		 * then it could mean that the application is
		 * trying to cancel an ongoing alarm
		 */
<<<<<<< HEAD
		local_irq_disable();

		rtc_status_reg = sirfsoc_rtc_iobrg_readl(
				rtcdrv->rtc_base + RTC_STATUS);
=======
		spin_lock_irq(&rtcdrv->lock);

		rtc_status_reg = sirfsoc_rtc_readl(rtcdrv, RTC_STATUS);
>>>>>>> v4.9.227
		if (rtc_status_reg & SIRFSOC_RTC_AL0E) {
			/* clear the RTC status register's alarm bit */
			rtc_status_reg &= ~0x07;
			/* write 1 into SIRFSOC_RTC_AL0 to force a clear */
			rtc_status_reg |= (SIRFSOC_RTC_AL0);
			/* Clear the Alarm enable bit */
			rtc_status_reg &= ~(SIRFSOC_RTC_AL0E);

<<<<<<< HEAD
			sirfsoc_rtc_iobrg_writel(rtc_status_reg,
					rtcdrv->rtc_base + RTC_STATUS);
		}

		local_irq_enable();
=======
			sirfsoc_rtc_writel(rtcdrv, RTC_STATUS,
					   rtc_status_reg);
		}

		spin_unlock_irq(&rtcdrv->lock);
>>>>>>> v4.9.227
	}

	return 0;
}

static int sirfsoc_rtc_read_time(struct device *dev,
		struct rtc_time *tm)
{
	unsigned long tmp_rtc = 0;
	struct sirfsoc_rtc_drv *rtcdrv;
	rtcdrv = dev_get_drvdata(dev);
	/*
	 * This patch is taken from WinCE - Need to validate this for
	 * correctness. To work around sirfsoc RTC counter double sync logic
	 * fail, read several times to make sure get stable value.
	 */
	do {
<<<<<<< HEAD
		tmp_rtc = sirfsoc_rtc_iobrg_readl(rtcdrv->rtc_base + RTC_CN);
		cpu_relax();
	} while (tmp_rtc != sirfsoc_rtc_iobrg_readl(rtcdrv->rtc_base + RTC_CN));
=======
		tmp_rtc = sirfsoc_rtc_readl(rtcdrv, RTC_CN);
		cpu_relax();
	} while (tmp_rtc != sirfsoc_rtc_readl(rtcdrv, RTC_CN));
>>>>>>> v4.9.227

	rtc_time_to_tm(rtcdrv->overflow_rtc << (BITS_PER_LONG - RTC_SHIFT) |
					tmp_rtc >> RTC_SHIFT, tm);
	return 0;
}

static int sirfsoc_rtc_set_time(struct device *dev,
		struct rtc_time *tm)
{
	unsigned long rtc_time;
	struct sirfsoc_rtc_drv *rtcdrv;
	rtcdrv = dev_get_drvdata(dev);

	rtc_tm_to_time(tm, &rtc_time);

	rtcdrv->overflow_rtc = rtc_time >> (BITS_PER_LONG - RTC_SHIFT);

<<<<<<< HEAD
	sirfsoc_rtc_iobrg_writel(rtcdrv->overflow_rtc,
			rtcdrv->rtc_base + RTC_SW_VALUE);
	sirfsoc_rtc_iobrg_writel(
			rtc_time << RTC_SHIFT, rtcdrv->rtc_base + RTC_CN);
=======
	sirfsoc_rtc_writel(rtcdrv, RTC_SW_VALUE, rtcdrv->overflow_rtc);
	sirfsoc_rtc_writel(rtcdrv, RTC_CN, rtc_time << RTC_SHIFT);
>>>>>>> v4.9.227

	return 0;
}

static int sirfsoc_rtc_ioctl(struct device *dev, unsigned int cmd,
		unsigned long arg)
{
	switch (cmd) {
	case RTC_PIE_ON:
	case RTC_PIE_OFF:
	case RTC_UIE_ON:
	case RTC_UIE_OFF:
	case RTC_AIE_ON:
	case RTC_AIE_OFF:
		return 0;

	default:
		return -ENOIOCTLCMD;
	}
}

<<<<<<< HEAD
=======
static int sirfsoc_rtc_alarm_irq_enable(struct device *dev,
		unsigned int enabled)
{
	unsigned long rtc_status_reg = 0x0;
	struct sirfsoc_rtc_drv *rtcdrv;

	rtcdrv = dev_get_drvdata(dev);

	spin_lock_irq(&rtcdrv->lock);

	rtc_status_reg = sirfsoc_rtc_readl(rtcdrv, RTC_STATUS);
	if (enabled)
		rtc_status_reg |= SIRFSOC_RTC_AL0E;
	else
		rtc_status_reg &= ~SIRFSOC_RTC_AL0E;

	sirfsoc_rtc_writel(rtcdrv, RTC_STATUS, rtc_status_reg);

	spin_unlock_irq(&rtcdrv->lock);

	return 0;

}

>>>>>>> v4.9.227
static const struct rtc_class_ops sirfsoc_rtc_ops = {
	.read_time = sirfsoc_rtc_read_time,
	.set_time = sirfsoc_rtc_set_time,
	.read_alarm = sirfsoc_rtc_read_alarm,
	.set_alarm = sirfsoc_rtc_set_alarm,
<<<<<<< HEAD
	.ioctl = sirfsoc_rtc_ioctl
=======
	.ioctl = sirfsoc_rtc_ioctl,
	.alarm_irq_enable = sirfsoc_rtc_alarm_irq_enable
>>>>>>> v4.9.227
};

static irqreturn_t sirfsoc_rtc_irq_handler(int irq, void *pdata)
{
	struct sirfsoc_rtc_drv *rtcdrv = pdata;
	unsigned long rtc_status_reg = 0x0;
	unsigned long events = 0x0;

<<<<<<< HEAD
	rtc_status_reg = sirfsoc_rtc_iobrg_readl(rtcdrv->rtc_base + RTC_STATUS);
=======
	spin_lock(&rtcdrv->lock);

	rtc_status_reg = sirfsoc_rtc_readl(rtcdrv, RTC_STATUS);
>>>>>>> v4.9.227
	/* this bit will be set ONLY if an alarm was active
	 * and it expired NOW
	 * So this is being used as an ASSERT
	 */
	if (rtc_status_reg & SIRFSOC_RTC_AL0) {
		/*
		 * clear the RTC status register's alarm bit
		 * mask out the lower status bits
		 */
		rtc_status_reg &= ~0x07;
		/* write 1 into SIRFSOC_RTC_AL0 to ACK the alarm interrupt */
		rtc_status_reg |= (SIRFSOC_RTC_AL0);
		/* Clear the Alarm enable bit */
		rtc_status_reg &= ~(SIRFSOC_RTC_AL0E);
	}
<<<<<<< HEAD
	sirfsoc_rtc_iobrg_writel(rtc_status_reg, rtcdrv->rtc_base + RTC_STATUS);
=======

	sirfsoc_rtc_writel(rtcdrv, RTC_STATUS, rtc_status_reg);

	spin_unlock(&rtcdrv->lock);

>>>>>>> v4.9.227
	/* this should wake up any apps polling/waiting on the read
	 * after setting the alarm
	 */
	events |= RTC_IRQF | RTC_AF;
	rtc_update_irq(rtcdrv->rtc, 1, events);

	return IRQ_HANDLED;
}

static const struct of_device_id sirfsoc_rtc_of_match[] = {
	{ .compatible = "sirf,prima2-sysrtc"},
	{},
};
<<<<<<< HEAD
=======

const struct regmap_config sysrtc_regmap_config = {
	.reg_bits = 32,
	.val_bits = 32,
	.fast_io = true,
};

>>>>>>> v4.9.227
MODULE_DEVICE_TABLE(of, sirfsoc_rtc_of_match);

static int sirfsoc_rtc_probe(struct platform_device *pdev)
{
	int err;
	unsigned long rtc_div;
	struct sirfsoc_rtc_drv *rtcdrv;
	struct device_node *np = pdev->dev.of_node;

	rtcdrv = devm_kzalloc(&pdev->dev,
		sizeof(struct sirfsoc_rtc_drv), GFP_KERNEL);
	if (rtcdrv == NULL)
		return -ENOMEM;

<<<<<<< HEAD
=======
	spin_lock_init(&rtcdrv->lock);

>>>>>>> v4.9.227
	err = of_property_read_u32(np, "reg", &rtcdrv->rtc_base);
	if (err) {
		dev_err(&pdev->dev, "unable to find base address of rtc node in dtb\n");
		return err;
	}

	platform_set_drvdata(pdev, rtcdrv);

	/* Register rtc alarm as a wakeup source */
	device_init_wakeup(&pdev->dev, 1);

<<<<<<< HEAD
=======
	rtcdrv->regmap = devm_regmap_init_iobg(&pdev->dev,
			&sysrtc_regmap_config);
	if (IS_ERR(rtcdrv->regmap)) {
		err = PTR_ERR(rtcdrv->regmap);
		dev_err(&pdev->dev, "Failed to allocate register map: %d\n",
			err);
		return err;
	}

>>>>>>> v4.9.227
	/*
	 * Set SYS_RTC counter in RTC_HZ HZ Units
	 * We are using 32K RTC crystal (32768 / RTC_HZ / 2) -1
	 * If 16HZ, therefore RTC_DIV = 1023;
	 */
	rtc_div = ((32768 / RTC_HZ) / 2) - 1;
<<<<<<< HEAD
	sirfsoc_rtc_iobrg_writel(rtc_div, rtcdrv->rtc_base + RTC_DIV);

	/* 0x3 -> RTC_CLK */
	sirfsoc_rtc_iobrg_writel(SIRFSOC_RTC_CLK,
			rtcdrv->rtc_base + RTC_CLOCK_SWITCH);

	/* reset SYS RTC ALARM0 */
	sirfsoc_rtc_iobrg_writel(0x0, rtcdrv->rtc_base + RTC_ALARM0);

	/* reset SYS RTC ALARM1 */
	sirfsoc_rtc_iobrg_writel(0x0, rtcdrv->rtc_base + RTC_ALARM1);

	/* Restore RTC Overflow From Register After Command Reboot */
	rtcdrv->overflow_rtc =
		sirfsoc_rtc_iobrg_readl(rtcdrv->rtc_base + RTC_SW_VALUE);
=======
	sirfsoc_rtc_writel(rtcdrv, RTC_DIV, rtc_div);

	/* 0x3 -> RTC_CLK */
	sirfsoc_rtc_writel(rtcdrv, RTC_CLOCK_SWITCH, SIRFSOC_RTC_CLK);

	/* reset SYS RTC ALARM0 */
	sirfsoc_rtc_writel(rtcdrv, RTC_ALARM0, 0x0);

	/* reset SYS RTC ALARM1 */
	sirfsoc_rtc_writel(rtcdrv, RTC_ALARM1, 0x0);

	/* Restore RTC Overflow From Register After Command Reboot */
	rtcdrv->overflow_rtc =
		sirfsoc_rtc_readl(rtcdrv, RTC_SW_VALUE);
>>>>>>> v4.9.227

	rtcdrv->rtc = devm_rtc_device_register(&pdev->dev, pdev->name,
			&sirfsoc_rtc_ops, THIS_MODULE);
	if (IS_ERR(rtcdrv->rtc)) {
		err = PTR_ERR(rtcdrv->rtc);
		dev_err(&pdev->dev, "can't register RTC device\n");
		return err;
	}

	rtcdrv->irq = platform_get_irq(pdev, 0);
	err = devm_request_irq(
			&pdev->dev,
			rtcdrv->irq,
			sirfsoc_rtc_irq_handler,
			IRQF_SHARED,
			pdev->name,
			rtcdrv);
	if (err) {
		dev_err(&pdev->dev, "Unable to register for the SiRF SOC RTC IRQ\n");
		return err;
	}

	return 0;
}

static int sirfsoc_rtc_remove(struct platform_device *pdev)
{
	device_init_wakeup(&pdev->dev, 0);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int sirfsoc_rtc_suspend(struct device *dev)
{
	struct sirfsoc_rtc_drv *rtcdrv = dev_get_drvdata(dev);
	rtcdrv->overflow_rtc =
<<<<<<< HEAD
		sirfsoc_rtc_iobrg_readl(rtcdrv->rtc_base + RTC_SW_VALUE);

	rtcdrv->saved_counter =
		sirfsoc_rtc_iobrg_readl(rtcdrv->rtc_base + RTC_CN);
=======
		sirfsoc_rtc_readl(rtcdrv, RTC_SW_VALUE);

	rtcdrv->saved_counter =
		sirfsoc_rtc_readl(rtcdrv, RTC_CN);
>>>>>>> v4.9.227
	rtcdrv->saved_overflow_rtc = rtcdrv->overflow_rtc;
	if (device_may_wakeup(dev) && !enable_irq_wake(rtcdrv->irq))
		rtcdrv->irq_wake = 1;

	return 0;
}

static int sirfsoc_rtc_resume(struct device *dev)
{
	u32 tmp;
	struct sirfsoc_rtc_drv *rtcdrv = dev_get_drvdata(dev);

	/*
	 * if resume from snapshot and the rtc power is lost,
	 * restroe the rtc settings
	 */
<<<<<<< HEAD
	if (SIRFSOC_RTC_CLK != sirfsoc_rtc_iobrg_readl(
			rtcdrv->rtc_base + RTC_CLOCK_SWITCH)) {
		u32 rtc_div;
		/* 0x3 -> RTC_CLK */
		sirfsoc_rtc_iobrg_writel(SIRFSOC_RTC_CLK,
			rtcdrv->rtc_base + RTC_CLOCK_SWITCH);
=======
	if (SIRFSOC_RTC_CLK != sirfsoc_rtc_readl(rtcdrv, RTC_CLOCK_SWITCH)) {
		u32 rtc_div;
		/* 0x3 -> RTC_CLK */
		sirfsoc_rtc_writel(rtcdrv, RTC_CLOCK_SWITCH, SIRFSOC_RTC_CLK);
>>>>>>> v4.9.227
		/*
		 * Set SYS_RTC counter in RTC_HZ HZ Units
		 * We are using 32K RTC crystal (32768 / RTC_HZ / 2) -1
		 * If 16HZ, therefore RTC_DIV = 1023;
		 */
		rtc_div = ((32768 / RTC_HZ) / 2) - 1;

<<<<<<< HEAD
		sirfsoc_rtc_iobrg_writel(rtc_div, rtcdrv->rtc_base + RTC_DIV);

		/* reset SYS RTC ALARM0 */
		sirfsoc_rtc_iobrg_writel(0x0, rtcdrv->rtc_base + RTC_ALARM0);

		/* reset SYS RTC ALARM1 */
		sirfsoc_rtc_iobrg_writel(0x0, rtcdrv->rtc_base + RTC_ALARM1);
=======
		sirfsoc_rtc_writel(rtcdrv, RTC_DIV, rtc_div);

		/* reset SYS RTC ALARM0 */
		sirfsoc_rtc_writel(rtcdrv, RTC_ALARM0, 0x0);

		/* reset SYS RTC ALARM1 */
		sirfsoc_rtc_writel(rtcdrv, RTC_ALARM1, 0x0);
>>>>>>> v4.9.227
	}
	rtcdrv->overflow_rtc = rtcdrv->saved_overflow_rtc;

	/*
	 * if current counter is small than previous,
	 * it means overflow in sleep
	 */
<<<<<<< HEAD
	tmp = sirfsoc_rtc_iobrg_readl(rtcdrv->rtc_base + RTC_CN);
=======
	tmp = sirfsoc_rtc_readl(rtcdrv, RTC_CN);
>>>>>>> v4.9.227
	if (tmp <= rtcdrv->saved_counter)
		rtcdrv->overflow_rtc++;
	/*
	 *PWRC Value Be Changed When Suspend, Restore Overflow
	 * In Memory To Register
	 */
<<<<<<< HEAD
	sirfsoc_rtc_iobrg_writel(rtcdrv->overflow_rtc,
			rtcdrv->rtc_base + RTC_SW_VALUE);
=======
	sirfsoc_rtc_writel(rtcdrv, RTC_SW_VALUE, rtcdrv->overflow_rtc);
>>>>>>> v4.9.227

	if (device_may_wakeup(dev) && rtcdrv->irq_wake) {
		disable_irq_wake(rtcdrv->irq);
		rtcdrv->irq_wake = 0;
	}

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(sirfsoc_rtc_pm_ops,
		sirfsoc_rtc_suspend, sirfsoc_rtc_resume);

static struct platform_driver sirfsoc_rtc_driver = {
	.driver = {
		.name = "sirfsoc-rtc",
<<<<<<< HEAD
		.owner = THIS_MODULE,
=======
>>>>>>> v4.9.227
		.pm = &sirfsoc_rtc_pm_ops,
		.of_match_table = sirfsoc_rtc_of_match,
	},
	.probe = sirfsoc_rtc_probe,
	.remove = sirfsoc_rtc_remove,
};
module_platform_driver(sirfsoc_rtc_driver);

MODULE_DESCRIPTION("SiRF SoC rtc driver");
MODULE_AUTHOR("Xianglong Du <Xianglong.Du@csr.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:sirfsoc-rtc");
