/*
 * Rotary counter driver for Analog Devices Blackfin Processors
 *
 * Copyright 2008-2009 Analog Devices Inc.
 * Licensed under the GPL-2 or later.
 */

#include <linux/module.h>
#include <linux/interrupt.h>
<<<<<<< HEAD
=======
#include <linux/io.h>
>>>>>>> v4.9.227
#include <linux/irq.h>
#include <linux/pm.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/slab.h>
<<<<<<< HEAD

#include <asm/portmux.h>
#include <asm/bfin_rotary.h>

static const u16 per_cnt[] = {
	P_CNT_CUD,
	P_CNT_CDG,
	P_CNT_CZM,
	0
};

struct bfin_rot {
	struct input_dev *input;
=======
#include <linux/platform_data/bfin_rotary.h>

#include <asm/portmux.h>

#define CNT_CONFIG_OFF		0	/* CNT Config Offset */
#define CNT_IMASK_OFF		4	/* CNT Interrupt Mask Offset */
#define CNT_STATUS_OFF		8	/* CNT Status Offset */
#define CNT_COMMAND_OFF		12	/* CNT Command Offset */
#define CNT_DEBOUNCE_OFF	16	/* CNT Debounce Offset */
#define CNT_COUNTER_OFF		20	/* CNT Counter Offset */
#define CNT_MAX_OFF		24	/* CNT Maximum Count Offset */
#define CNT_MIN_OFF		28	/* CNT Minimum Count Offset */

struct bfin_rot {
	struct input_dev *input;
	void __iomem *base;
>>>>>>> v4.9.227
	int irq;
	unsigned int up_key;
	unsigned int down_key;
	unsigned int button_key;
	unsigned int rel_code;
<<<<<<< HEAD
=======

	unsigned short mode;
	unsigned short debounce;

>>>>>>> v4.9.227
	unsigned short cnt_config;
	unsigned short cnt_imask;
	unsigned short cnt_debounce;
};

static void report_key_event(struct input_dev *input, int keycode)
{
	/* simulate a press-n-release */
	input_report_key(input, keycode, 1);
	input_sync(input);
	input_report_key(input, keycode, 0);
	input_sync(input);
}

static void report_rotary_event(struct bfin_rot *rotary, int delta)
{
	struct input_dev *input = rotary->input;

	if (rotary->up_key) {
		report_key_event(input,
				 delta > 0 ? rotary->up_key : rotary->down_key);
	} else {
		input_report_rel(input, rotary->rel_code, delta);
		input_sync(input);
	}
}

static irqreturn_t bfin_rotary_isr(int irq, void *dev_id)
{
<<<<<<< HEAD
	struct platform_device *pdev = dev_id;
	struct bfin_rot *rotary = platform_get_drvdata(pdev);
	int delta;

	switch (bfin_read_CNT_STATUS()) {
=======
	struct bfin_rot *rotary = dev_id;
	int delta;

	switch (readw(rotary->base + CNT_STATUS_OFF)) {
>>>>>>> v4.9.227

	case ICII:
		break;

	case UCII:
	case DCII:
<<<<<<< HEAD
		delta = bfin_read_CNT_COUNTER();
=======
		delta = readl(rotary->base + CNT_COUNTER_OFF);
>>>>>>> v4.9.227
		if (delta)
			report_rotary_event(rotary, delta);
		break;

	case CZMII:
		report_key_event(rotary->input, rotary->button_key);
		break;

	default:
		break;
	}

<<<<<<< HEAD
	bfin_write_CNT_COMMAND(W1LCNT_ZERO);	/* Clear COUNTER */
	bfin_write_CNT_STATUS(-1);	/* Clear STATUS */
=======
	writew(W1LCNT_ZERO, rotary->base + CNT_COMMAND_OFF); /* Clear COUNTER */
	writew(-1, rotary->base + CNT_STATUS_OFF); /* Clear STATUS */
>>>>>>> v4.9.227

	return IRQ_HANDLED;
}

<<<<<<< HEAD
static int bfin_rotary_probe(struct platform_device *pdev)
{
	struct bfin_rotary_platform_data *pdata = dev_get_platdata(&pdev->dev);
	struct bfin_rot *rotary;
=======
static int bfin_rotary_open(struct input_dev *input)
{
	struct bfin_rot *rotary = input_get_drvdata(input);
	unsigned short val;

	if (rotary->mode & ROT_DEBE)
		writew(rotary->debounce & DPRESCALE,
			rotary->base + CNT_DEBOUNCE_OFF);

	writew(rotary->mode & ~CNTE, rotary->base + CNT_CONFIG_OFF);

	val = UCIE | DCIE;
	if (rotary->button_key)
		val |= CZMIE;
	writew(val, rotary->base + CNT_IMASK_OFF);

	writew(rotary->mode | CNTE, rotary->base + CNT_CONFIG_OFF);

	return 0;
}

static void bfin_rotary_close(struct input_dev *input)
{
	struct bfin_rot *rotary = input_get_drvdata(input);

	writew(0, rotary->base + CNT_CONFIG_OFF);
	writew(0, rotary->base + CNT_IMASK_OFF);
}

static void bfin_rotary_free_action(void *data)
{
	peripheral_free_list(data);
}

static int bfin_rotary_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct bfin_rotary_platform_data *pdata = dev_get_platdata(dev);
	struct bfin_rot *rotary;
	struct resource *res;
>>>>>>> v4.9.227
	struct input_dev *input;
	int error;

	/* Basic validation */
	if ((pdata->rotary_up_key && !pdata->rotary_down_key) ||
	    (!pdata->rotary_up_key && pdata->rotary_down_key)) {
		return -EINVAL;
	}

<<<<<<< HEAD
	error = peripheral_request_list(per_cnt, dev_name(&pdev->dev));
	if (error) {
		dev_err(&pdev->dev, "requesting peripherals failed\n");
		return error;
	}

	rotary = kzalloc(sizeof(struct bfin_rot), GFP_KERNEL);
	input = input_allocate_device();
	if (!rotary || !input) {
		error = -ENOMEM;
		goto out1;
	}
=======
	if (pdata->pin_list) {
		error = peripheral_request_list(pdata->pin_list,
						dev_name(&pdev->dev));
		if (error) {
			dev_err(dev, "requesting peripherals failed: %d\n",
				error);
			return error;
		}

		error = devm_add_action(dev, bfin_rotary_free_action,
					pdata->pin_list);
		if (error) {
			dev_err(dev, "setting cleanup action failed: %d\n",
				error);
			peripheral_free_list(pdata->pin_list);
			return error;
		}
	}

	rotary = devm_kzalloc(dev, sizeof(struct bfin_rot), GFP_KERNEL);
	if (!rotary)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	rotary->base = devm_ioremap_resource(dev, res);
	if (IS_ERR(rotary->base))
		return PTR_ERR(rotary->base);

	input = devm_input_allocate_device(dev);
	if (!input)
		return -ENOMEM;
>>>>>>> v4.9.227

	rotary->input = input;

	rotary->up_key = pdata->rotary_up_key;
	rotary->down_key = pdata->rotary_down_key;
	rotary->button_key = pdata->rotary_button_key;
	rotary->rel_code = pdata->rotary_rel_code;

<<<<<<< HEAD
	error = rotary->irq = platform_get_irq(pdev, 0);
	if (error < 0)
		goto out1;
=======
	rotary->mode = pdata->mode;
	rotary->debounce = pdata->debounce;
>>>>>>> v4.9.227

	input->name = pdev->name;
	input->phys = "bfin-rotary/input0";
	input->dev.parent = &pdev->dev;

	input_set_drvdata(input, rotary);

	input->id.bustype = BUS_HOST;
	input->id.vendor = 0x0001;
	input->id.product = 0x0001;
	input->id.version = 0x0100;

<<<<<<< HEAD
=======
	input->open = bfin_rotary_open;
	input->close = bfin_rotary_close;

>>>>>>> v4.9.227
	if (rotary->up_key) {
		__set_bit(EV_KEY, input->evbit);
		__set_bit(rotary->up_key, input->keybit);
		__set_bit(rotary->down_key, input->keybit);
	} else {
		__set_bit(EV_REL, input->evbit);
		__set_bit(rotary->rel_code, input->relbit);
	}

	if (rotary->button_key) {
		__set_bit(EV_KEY, input->evbit);
		__set_bit(rotary->button_key, input->keybit);
	}

<<<<<<< HEAD
	error = request_irq(rotary->irq, bfin_rotary_isr,
			    0, dev_name(&pdev->dev), pdev);
	if (error) {
		dev_err(&pdev->dev,
			"unable to claim irq %d; error %d\n",
			rotary->irq, error);
		goto out1;
=======
	/* Quiesce the device before requesting irq */
	bfin_rotary_close(input);

	rotary->irq = platform_get_irq(pdev, 0);
	if (rotary->irq < 0) {
		dev_err(dev, "No rotary IRQ specified\n");
		return -ENOENT;
	}

	error = devm_request_irq(dev, rotary->irq, bfin_rotary_isr,
				 0, dev_name(dev), rotary);
	if (error) {
		dev_err(dev, "unable to claim irq %d; error %d\n",
			rotary->irq, error);
		return error;
>>>>>>> v4.9.227
	}

	error = input_register_device(input);
	if (error) {
<<<<<<< HEAD
		dev_err(&pdev->dev,
			"unable to register input device (%d)\n", error);
		goto out2;
	}

	if (pdata->rotary_button_key)
		bfin_write_CNT_IMASK(CZMIE);

	if (pdata->mode & ROT_DEBE)
		bfin_write_CNT_DEBOUNCE(pdata->debounce & DPRESCALE);

	if (pdata->mode)
		bfin_write_CNT_CONFIG(bfin_read_CNT_CONFIG() |
					(pdata->mode & ~CNTE));

	bfin_write_CNT_IMASK(bfin_read_CNT_IMASK() | UCIE | DCIE);
	bfin_write_CNT_CONFIG(bfin_read_CNT_CONFIG() | CNTE);

=======
		dev_err(dev, "unable to register input device (%d)\n", error);
		return error;
	}

>>>>>>> v4.9.227
	platform_set_drvdata(pdev, rotary);
	device_init_wakeup(&pdev->dev, 1);

	return 0;
<<<<<<< HEAD

out2:
	free_irq(rotary->irq, pdev);
out1:
	input_free_device(input);
	kfree(rotary);
	peripheral_free_list(per_cnt);

	return error;
}

static int bfin_rotary_remove(struct platform_device *pdev)
{
	struct bfin_rot *rotary = platform_get_drvdata(pdev);

	bfin_write_CNT_CONFIG(0);
	bfin_write_CNT_IMASK(0);

	free_irq(rotary->irq, pdev);
	input_unregister_device(rotary->input);
	peripheral_free_list(per_cnt);

	kfree(rotary);

	return 0;
}

#ifdef CONFIG_PM
static int bfin_rotary_suspend(struct device *dev)
=======
}

static int __maybe_unused bfin_rotary_suspend(struct device *dev)
>>>>>>> v4.9.227
{
	struct platform_device *pdev = to_platform_device(dev);
	struct bfin_rot *rotary = platform_get_drvdata(pdev);

<<<<<<< HEAD
	rotary->cnt_config = bfin_read_CNT_CONFIG();
	rotary->cnt_imask = bfin_read_CNT_IMASK();
	rotary->cnt_debounce = bfin_read_CNT_DEBOUNCE();
=======
	rotary->cnt_config = readw(rotary->base + CNT_CONFIG_OFF);
	rotary->cnt_imask = readw(rotary->base + CNT_IMASK_OFF);
	rotary->cnt_debounce = readw(rotary->base + CNT_DEBOUNCE_OFF);
>>>>>>> v4.9.227

	if (device_may_wakeup(&pdev->dev))
		enable_irq_wake(rotary->irq);

	return 0;
}

<<<<<<< HEAD
static int bfin_rotary_resume(struct device *dev)
=======
static int __maybe_unused bfin_rotary_resume(struct device *dev)
>>>>>>> v4.9.227
{
	struct platform_device *pdev = to_platform_device(dev);
	struct bfin_rot *rotary = platform_get_drvdata(pdev);

<<<<<<< HEAD
	bfin_write_CNT_DEBOUNCE(rotary->cnt_debounce);
	bfin_write_CNT_IMASK(rotary->cnt_imask);
	bfin_write_CNT_CONFIG(rotary->cnt_config & ~CNTE);
=======
	writew(rotary->cnt_debounce, rotary->base + CNT_DEBOUNCE_OFF);
	writew(rotary->cnt_imask, rotary->base + CNT_IMASK_OFF);
	writew(rotary->cnt_config & ~CNTE, rotary->base + CNT_CONFIG_OFF);
>>>>>>> v4.9.227

	if (device_may_wakeup(&pdev->dev))
		disable_irq_wake(rotary->irq);

	if (rotary->cnt_config & CNTE)
<<<<<<< HEAD
		bfin_write_CNT_CONFIG(rotary->cnt_config);
=======
		writew(rotary->cnt_config, rotary->base + CNT_CONFIG_OFF);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static const struct dev_pm_ops bfin_rotary_pm_ops = {
	.suspend	= bfin_rotary_suspend,
	.resume		= bfin_rotary_resume,
};
#endif

static struct platform_driver bfin_rotary_device_driver = {
	.probe		= bfin_rotary_probe,
	.remove		= bfin_rotary_remove,
	.driver		= {
		.name	= "bfin-rotary",
		.owner	= THIS_MODULE,
#ifdef CONFIG_PM
		.pm	= &bfin_rotary_pm_ops,
#endif
=======
static SIMPLE_DEV_PM_OPS(bfin_rotary_pm_ops,
			 bfin_rotary_suspend, bfin_rotary_resume);

static struct platform_driver bfin_rotary_device_driver = {
	.probe		= bfin_rotary_probe,
	.driver		= {
		.name	= "bfin-rotary",
		.pm	= &bfin_rotary_pm_ops,
>>>>>>> v4.9.227
	},
};
module_platform_driver(bfin_rotary_device_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michael Hennerich <hennerich@blackfin.uclinux.org>");
MODULE_DESCRIPTION("Rotary Counter driver for Blackfin Processors");
MODULE_ALIAS("platform:bfin-rotary");
