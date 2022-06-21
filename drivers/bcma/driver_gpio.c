/*
 * Broadcom specific AMBA
 * GPIO driver
 *
 * Copyright 2011, Broadcom Corporation
 * Copyright 2012, Hauke Mehrtens <hauke@hauke-m.de>
 *
 * Licensed under the GNU/GPL. See COPYING for details.
 */

<<<<<<< HEAD
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/irqdomain.h>
=======
#include <linux/gpio/driver.h>
#include <linux/interrupt.h>
>>>>>>> v4.9.227
#include <linux/export.h>
#include <linux/bcma/bcma.h>

#include "bcma_private.h"

<<<<<<< HEAD
static inline struct bcma_drv_cc *bcma_gpio_get_cc(struct gpio_chip *chip)
{
	return container_of(chip, struct bcma_drv_cc, gpio);
}

static int bcma_gpio_get_value(struct gpio_chip *chip, unsigned gpio)
{
	struct bcma_drv_cc *cc = bcma_gpio_get_cc(chip);
=======
#define BCMA_GPIO_MAX_PINS	32

static int bcma_gpio_get_value(struct gpio_chip *chip, unsigned gpio)
{
	struct bcma_drv_cc *cc = gpiochip_get_data(chip);
>>>>>>> v4.9.227

	return !!bcma_chipco_gpio_in(cc, 1 << gpio);
}

static void bcma_gpio_set_value(struct gpio_chip *chip, unsigned gpio,
				int value)
{
<<<<<<< HEAD
	struct bcma_drv_cc *cc = bcma_gpio_get_cc(chip);
=======
	struct bcma_drv_cc *cc = gpiochip_get_data(chip);
>>>>>>> v4.9.227

	bcma_chipco_gpio_out(cc, 1 << gpio, value ? 1 << gpio : 0);
}

static int bcma_gpio_direction_input(struct gpio_chip *chip, unsigned gpio)
{
<<<<<<< HEAD
	struct bcma_drv_cc *cc = bcma_gpio_get_cc(chip);
=======
	struct bcma_drv_cc *cc = gpiochip_get_data(chip);
>>>>>>> v4.9.227

	bcma_chipco_gpio_outen(cc, 1 << gpio, 0);
	return 0;
}

static int bcma_gpio_direction_output(struct gpio_chip *chip, unsigned gpio,
				      int value)
{
<<<<<<< HEAD
	struct bcma_drv_cc *cc = bcma_gpio_get_cc(chip);
=======
	struct bcma_drv_cc *cc = gpiochip_get_data(chip);
>>>>>>> v4.9.227

	bcma_chipco_gpio_outen(cc, 1 << gpio, 1 << gpio);
	bcma_chipco_gpio_out(cc, 1 << gpio, value ? 1 << gpio : 0);
	return 0;
}

static int bcma_gpio_request(struct gpio_chip *chip, unsigned gpio)
{
<<<<<<< HEAD
	struct bcma_drv_cc *cc = bcma_gpio_get_cc(chip);
=======
	struct bcma_drv_cc *cc = gpiochip_get_data(chip);
>>>>>>> v4.9.227

	bcma_chipco_gpio_control(cc, 1 << gpio, 0);
	/* clear pulldown */
	bcma_chipco_gpio_pulldown(cc, 1 << gpio, 0);
	/* Set pullup */
	bcma_chipco_gpio_pullup(cc, 1 << gpio, 1 << gpio);

	return 0;
}

static void bcma_gpio_free(struct gpio_chip *chip, unsigned gpio)
{
<<<<<<< HEAD
	struct bcma_drv_cc *cc = bcma_gpio_get_cc(chip);
=======
	struct bcma_drv_cc *cc = gpiochip_get_data(chip);
>>>>>>> v4.9.227

	/* clear pullup */
	bcma_chipco_gpio_pullup(cc, 1 << gpio, 0);
}

<<<<<<< HEAD
#if IS_BUILTIN(CONFIG_BCM47XX)
static int bcma_gpio_to_irq(struct gpio_chip *chip, unsigned gpio)
{
	struct bcma_drv_cc *cc = bcma_gpio_get_cc(chip);

	if (cc->core->bus->hosttype == BCMA_HOSTTYPE_SOC)
		return irq_find_mapping(cc->irq_domain, gpio);
	else
		return -EINVAL;
}

static void bcma_gpio_irq_unmask(struct irq_data *d)
{
	struct bcma_drv_cc *cc = irq_data_get_irq_chip_data(d);
=======
#if IS_BUILTIN(CONFIG_BCM47XX) || IS_BUILTIN(CONFIG_ARCH_BCM_5301X)

static void bcma_gpio_irq_unmask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct bcma_drv_cc *cc = gpiochip_get_data(gc);
>>>>>>> v4.9.227
	int gpio = irqd_to_hwirq(d);
	u32 val = bcma_chipco_gpio_in(cc, BIT(gpio));

	bcma_chipco_gpio_polarity(cc, BIT(gpio), val);
	bcma_chipco_gpio_intmask(cc, BIT(gpio), BIT(gpio));
}

static void bcma_gpio_irq_mask(struct irq_data *d)
{
<<<<<<< HEAD
	struct bcma_drv_cc *cc = irq_data_get_irq_chip_data(d);
=======
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct bcma_drv_cc *cc = gpiochip_get_data(gc);
>>>>>>> v4.9.227
	int gpio = irqd_to_hwirq(d);

	bcma_chipco_gpio_intmask(cc, BIT(gpio), 0);
}

static struct irq_chip bcma_gpio_irq_chip = {
	.name		= "BCMA-GPIO",
	.irq_mask	= bcma_gpio_irq_mask,
	.irq_unmask	= bcma_gpio_irq_unmask,
};

static irqreturn_t bcma_gpio_irq_handler(int irq, void *dev_id)
{
	struct bcma_drv_cc *cc = dev_id;
<<<<<<< HEAD
=======
	struct gpio_chip *gc = &cc->gpio;
>>>>>>> v4.9.227
	u32 val = bcma_cc_read32(cc, BCMA_CC_GPIOIN);
	u32 mask = bcma_cc_read32(cc, BCMA_CC_GPIOIRQ);
	u32 pol = bcma_cc_read32(cc, BCMA_CC_GPIOPOL);
	unsigned long irqs = (val ^ pol) & mask;
	int gpio;

	if (!irqs)
		return IRQ_NONE;

<<<<<<< HEAD
	for_each_set_bit(gpio, &irqs, cc->gpio.ngpio)
		generic_handle_irq(bcma_gpio_to_irq(&cc->gpio, gpio));
=======
	for_each_set_bit(gpio, &irqs, gc->ngpio)
		generic_handle_irq(irq_find_mapping(gc->irqdomain, gpio));
>>>>>>> v4.9.227
	bcma_chipco_gpio_polarity(cc, irqs, val & irqs);

	return IRQ_HANDLED;
}

<<<<<<< HEAD
static int bcma_gpio_irq_domain_init(struct bcma_drv_cc *cc)
{
	struct gpio_chip *chip = &cc->gpio;
	int gpio, hwirq, err;
=======
static int bcma_gpio_irq_init(struct bcma_drv_cc *cc)
{
	struct gpio_chip *chip = &cc->gpio;
	int hwirq, err;
>>>>>>> v4.9.227

	if (cc->core->bus->hosttype != BCMA_HOSTTYPE_SOC)
		return 0;

<<<<<<< HEAD
	cc->irq_domain = irq_domain_add_linear(NULL, chip->ngpio,
					       &irq_domain_simple_ops, cc);
	if (!cc->irq_domain) {
		err = -ENODEV;
		goto err_irq_domain;
	}
	for (gpio = 0; gpio < chip->ngpio; gpio++) {
		int irq = irq_create_mapping(cc->irq_domain, gpio);

		irq_set_chip_data(irq, cc);
		irq_set_chip_and_handler(irq, &bcma_gpio_irq_chip,
					 handle_simple_irq);
	}

	hwirq = bcma_core_irq(cc->core);
	err = request_irq(hwirq, bcma_gpio_irq_handler, IRQF_SHARED, "gpio",
			  cc);
	if (err)
		goto err_req_irq;
=======
	hwirq = bcma_core_irq(cc->core, 0);
	err = request_irq(hwirq, bcma_gpio_irq_handler, IRQF_SHARED, "gpio",
			  cc);
	if (err)
		return err;
>>>>>>> v4.9.227

	bcma_chipco_gpio_intmask(cc, ~0, 0);
	bcma_cc_set32(cc, BCMA_CC_IRQMASK, BCMA_CC_IRQ_GPIO);

<<<<<<< HEAD
	return 0;

err_req_irq:
	for (gpio = 0; gpio < chip->ngpio; gpio++) {
		int irq = irq_find_mapping(cc->irq_domain, gpio);

		irq_dispose_mapping(irq);
	}
	irq_domain_remove(cc->irq_domain);
err_irq_domain:
	return err;
}

static void bcma_gpio_irq_domain_exit(struct bcma_drv_cc *cc)
{
	struct gpio_chip *chip = &cc->gpio;
	int gpio;

=======
	err =  gpiochip_irqchip_add(chip,
				    &bcma_gpio_irq_chip,
				    0,
				    handle_simple_irq,
				    IRQ_TYPE_NONE);
	if (err) {
		free_irq(hwirq, cc);
		return err;
	}

	return 0;
}

static void bcma_gpio_irq_exit(struct bcma_drv_cc *cc)
{
>>>>>>> v4.9.227
	if (cc->core->bus->hosttype != BCMA_HOSTTYPE_SOC)
		return;

	bcma_cc_mask32(cc, BCMA_CC_IRQMASK, ~BCMA_CC_IRQ_GPIO);
<<<<<<< HEAD
	free_irq(bcma_core_irq(cc->core), cc);
	for (gpio = 0; gpio < chip->ngpio; gpio++) {
		int irq = irq_find_mapping(cc->irq_domain, gpio);

		irq_dispose_mapping(irq);
	}
	irq_domain_remove(cc->irq_domain);
}
#else
static int bcma_gpio_irq_domain_init(struct bcma_drv_cc *cc)
=======
	free_irq(bcma_core_irq(cc->core, 0), cc);
}
#else
static int bcma_gpio_irq_init(struct bcma_drv_cc *cc)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static void bcma_gpio_irq_domain_exit(struct bcma_drv_cc *cc)
=======
static void bcma_gpio_irq_exit(struct bcma_drv_cc *cc)
>>>>>>> v4.9.227
{
}
#endif

int bcma_gpio_init(struct bcma_drv_cc *cc)
{
<<<<<<< HEAD
=======
	struct bcma_bus *bus = cc->core->bus;
>>>>>>> v4.9.227
	struct gpio_chip *chip = &cc->gpio;
	int err;

	chip->label		= "bcma_gpio";
	chip->owner		= THIS_MODULE;
	chip->request		= bcma_gpio_request;
	chip->free		= bcma_gpio_free;
	chip->get		= bcma_gpio_get_value;
	chip->set		= bcma_gpio_set_value;
	chip->direction_input	= bcma_gpio_direction_input;
	chip->direction_output	= bcma_gpio_direction_output;
<<<<<<< HEAD
#if IS_BUILTIN(CONFIG_BCM47XX)
	chip->to_irq		= bcma_gpio_to_irq;
#endif
=======
	chip->owner		= THIS_MODULE;
	chip->parent		= bcma_bus_get_host_dev(bus);
>>>>>>> v4.9.227
#if IS_BUILTIN(CONFIG_OF)
	if (cc->core->bus->hosttype == BCMA_HOSTTYPE_SOC)
		chip->of_node	= cc->core->dev.of_node;
#endif
<<<<<<< HEAD
	switch (cc->core->bus->chipinfo.id) {
	case BCMA_CHIP_ID_BCM5357:
	case BCMA_CHIP_ID_BCM53572:
=======
	switch (bus->chipinfo.id) {
	case BCMA_CHIP_ID_BCM4707:
	case BCMA_CHIP_ID_BCM5357:
	case BCMA_CHIP_ID_BCM53572:
	case BCMA_CHIP_ID_BCM47094:
>>>>>>> v4.9.227
		chip->ngpio	= 32;
		break;
	default:
		chip->ngpio	= 16;
	}

<<<<<<< HEAD
	/* There is just one SoC in one device and its GPIO addresses should be
	 * deterministic to address them more easily. The other buses could get
	 * a random base number. */
	if (cc->core->bus->hosttype == BCMA_HOSTTYPE_SOC)
		chip->base		= 0;
	else
		chip->base		= -1;

	err = bcma_gpio_irq_domain_init(cc);
	if (err)
		return err;

	err = gpiochip_add(chip);
	if (err) {
		bcma_gpio_irq_domain_exit(cc);
=======
	/*
	 * Register SoC GPIO devices with absolute GPIO pin base.
	 * On MIPS, we don't have Device Tree and we can't use relative (per chip)
	 * GPIO numbers.
	 * On some ARM devices, user space may want to access some system GPIO
	 * pins directly, which is easier to do with a predictable GPIO base.
	 */
	if (IS_BUILTIN(CONFIG_BCM47XX) ||
	    cc->core->bus->hosttype == BCMA_HOSTTYPE_SOC)
		chip->base		= bus->num * BCMA_GPIO_MAX_PINS;
	else
		chip->base		= -1;

	err = gpiochip_add_data(chip, cc);
	if (err)
		return err;

	err = bcma_gpio_irq_init(cc);
	if (err) {
		gpiochip_remove(chip);
>>>>>>> v4.9.227
		return err;
	}

	return 0;
}

int bcma_gpio_unregister(struct bcma_drv_cc *cc)
{
<<<<<<< HEAD
	bcma_gpio_irq_domain_exit(cc);
=======
	bcma_gpio_irq_exit(cc);
>>>>>>> v4.9.227
	gpiochip_remove(&cc->gpio);
	return 0;
}
