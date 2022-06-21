/*
 * arch/arm/mach-tegra/gpio.c
 *
 * Copyright (c) 2010 Google, Inc
 *
 * Author:
 *	Erik Gilling <konkers@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/err.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/irqdomain.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pm.h>

#define GPIO_BANK(x)		((x) >> 5)
#define GPIO_PORT(x)		(((x) >> 3) & 0x3)
#define GPIO_BIT(x)		((x) & 0x7)

<<<<<<< HEAD
#define GPIO_REG(x)		(GPIO_BANK(x) * tegra_gpio_bank_stride + \
					GPIO_PORT(x) * 4)

#define GPIO_CNF(x)		(GPIO_REG(x) + 0x00)
#define GPIO_OE(x)		(GPIO_REG(x) + 0x10)
#define GPIO_OUT(x)		(GPIO_REG(x) + 0X20)
#define GPIO_IN(x)		(GPIO_REG(x) + 0x30)
#define GPIO_INT_STA(x)		(GPIO_REG(x) + 0x40)
#define GPIO_INT_ENB(x)		(GPIO_REG(x) + 0x50)
#define GPIO_INT_LVL(x)		(GPIO_REG(x) + 0x60)
#define GPIO_INT_CLR(x)		(GPIO_REG(x) + 0x70)

#define GPIO_MSK_CNF(x)		(GPIO_REG(x) + tegra_gpio_upper_offset + 0x00)
#define GPIO_MSK_OE(x)		(GPIO_REG(x) + tegra_gpio_upper_offset + 0x10)
#define GPIO_MSK_OUT(x)		(GPIO_REG(x) + tegra_gpio_upper_offset + 0X20)
#define GPIO_MSK_INT_STA(x)	(GPIO_REG(x) + tegra_gpio_upper_offset + 0x40)
#define GPIO_MSK_INT_ENB(x)	(GPIO_REG(x) + tegra_gpio_upper_offset + 0x50)
#define GPIO_MSK_INT_LVL(x)	(GPIO_REG(x) + tegra_gpio_upper_offset + 0x60)
=======
#define GPIO_REG(tgi, x)	(GPIO_BANK(x) * tgi->soc->bank_stride + \
					GPIO_PORT(x) * 4)

#define GPIO_CNF(t, x)		(GPIO_REG(t, x) + 0x00)
#define GPIO_OE(t, x)		(GPIO_REG(t, x) + 0x10)
#define GPIO_OUT(t, x)		(GPIO_REG(t, x) + 0X20)
#define GPIO_IN(t, x)		(GPIO_REG(t, x) + 0x30)
#define GPIO_INT_STA(t, x)	(GPIO_REG(t, x) + 0x40)
#define GPIO_INT_ENB(t, x)	(GPIO_REG(t, x) + 0x50)
#define GPIO_INT_LVL(t, x)	(GPIO_REG(t, x) + 0x60)
#define GPIO_INT_CLR(t, x)	(GPIO_REG(t, x) + 0x70)
#define GPIO_DBC_CNT(t, x)	(GPIO_REG(t, x) + 0xF0)


#define GPIO_MSK_CNF(t, x)	(GPIO_REG(t, x) + t->soc->upper_offset + 0x00)
#define GPIO_MSK_OE(t, x)	(GPIO_REG(t, x) + t->soc->upper_offset + 0x10)
#define GPIO_MSK_OUT(t, x)	(GPIO_REG(t, x) + t->soc->upper_offset + 0X20)
#define GPIO_MSK_DBC_EN(t, x)	(GPIO_REG(t, x) + t->soc->upper_offset + 0x30)
#define GPIO_MSK_INT_STA(t, x)	(GPIO_REG(t, x) + t->soc->upper_offset + 0x40)
#define GPIO_MSK_INT_ENB(t, x)	(GPIO_REG(t, x) + t->soc->upper_offset + 0x50)
#define GPIO_MSK_INT_LVL(t, x)	(GPIO_REG(t, x) + t->soc->upper_offset + 0x60)
>>>>>>> v4.9.227

#define GPIO_INT_LVL_MASK		0x010101
#define GPIO_INT_LVL_EDGE_RISING	0x000101
#define GPIO_INT_LVL_EDGE_FALLING	0x000100
#define GPIO_INT_LVL_EDGE_BOTH		0x010100
#define GPIO_INT_LVL_LEVEL_HIGH		0x000001
#define GPIO_INT_LVL_LEVEL_LOW		0x000000

<<<<<<< HEAD
=======
struct tegra_gpio_info;

>>>>>>> v4.9.227
struct tegra_gpio_bank {
	int bank;
	int irq;
	spinlock_t lvl_lock[4];
<<<<<<< HEAD
=======
	spinlock_t dbc_lock[4];	/* Lock for updating debounce count register */
>>>>>>> v4.9.227
#ifdef CONFIG_PM_SLEEP
	u32 cnf[4];
	u32 out[4];
	u32 oe[4];
	u32 int_enb[4];
	u32 int_lvl[4];
	u32 wake_enb[4];
<<<<<<< HEAD
#endif
};

static struct device *dev;
static struct irq_domain *irq_domain;
static void __iomem *regs;
static u32 tegra_gpio_bank_count;
static u32 tegra_gpio_bank_stride;
static u32 tegra_gpio_upper_offset;
static struct tegra_gpio_bank *tegra_gpio_banks;

static inline void tegra_gpio_writel(u32 val, u32 reg)
{
	__raw_writel(val, regs + reg);
}

static inline u32 tegra_gpio_readl(u32 reg)
{
	return __raw_readl(regs + reg);
=======
	u32 dbc_enb[4];
#endif
	u32 dbc_cnt[4];
	struct tegra_gpio_info *tgi;
};

struct tegra_gpio_soc_config {
	bool debounce_supported;
	u32 bank_stride;
	u32 upper_offset;
};

struct tegra_gpio_info {
	struct device				*dev;
	void __iomem				*regs;
	struct irq_domain			*irq_domain;
	struct tegra_gpio_bank			*bank_info;
	const struct tegra_gpio_soc_config	*soc;
	struct gpio_chip			gc;
	struct irq_chip				ic;
	u32					bank_count;
};

static inline void tegra_gpio_writel(struct tegra_gpio_info *tgi,
				     u32 val, u32 reg)
{
	__raw_writel(val, tgi->regs + reg);
}

static inline u32 tegra_gpio_readl(struct tegra_gpio_info *tgi, u32 reg)
{
	return __raw_readl(tgi->regs + reg);
>>>>>>> v4.9.227
}

static int tegra_gpio_compose(int bank, int port, int bit)
{
	return (bank << 5) | ((port & 0x3) << 3) | (bit & 0x7);
}

<<<<<<< HEAD
static void tegra_gpio_mask_write(u32 reg, int gpio, int value)
=======
static void tegra_gpio_mask_write(struct tegra_gpio_info *tgi, u32 reg,
				  int gpio, int value)
>>>>>>> v4.9.227
{
	u32 val;

	val = 0x100 << GPIO_BIT(gpio);
	if (value)
		val |= 1 << GPIO_BIT(gpio);
<<<<<<< HEAD
	tegra_gpio_writel(val, reg);
}

static void tegra_gpio_enable(int gpio)
{
	tegra_gpio_mask_write(GPIO_MSK_CNF(gpio), gpio, 1);
}

static void tegra_gpio_disable(int gpio)
{
	tegra_gpio_mask_write(GPIO_MSK_CNF(gpio), gpio, 0);
=======
	tegra_gpio_writel(tgi, val, reg);
}

static void tegra_gpio_enable(struct tegra_gpio_info *tgi, int gpio)
{
	tegra_gpio_mask_write(tgi, GPIO_MSK_CNF(tgi, gpio), gpio, 1);
}

static void tegra_gpio_disable(struct tegra_gpio_info *tgi, int gpio)
{
	tegra_gpio_mask_write(tgi, GPIO_MSK_CNF(tgi, gpio), gpio, 0);
>>>>>>> v4.9.227
}

static int tegra_gpio_request(struct gpio_chip *chip, unsigned offset)
{
	return pinctrl_request_gpio(offset);
}

static void tegra_gpio_free(struct gpio_chip *chip, unsigned offset)
{
<<<<<<< HEAD
	pinctrl_free_gpio(offset);
	tegra_gpio_disable(offset);
=======
	struct tegra_gpio_info *tgi = gpiochip_get_data(chip);

	pinctrl_free_gpio(offset);
	tegra_gpio_disable(tgi, offset);
>>>>>>> v4.9.227
}

static void tegra_gpio_set(struct gpio_chip *chip, unsigned offset, int value)
{
<<<<<<< HEAD
	tegra_gpio_mask_write(GPIO_MSK_OUT(offset), offset, value);
=======
	struct tegra_gpio_info *tgi = gpiochip_get_data(chip);

	tegra_gpio_mask_write(tgi, GPIO_MSK_OUT(tgi, offset), offset, value);
>>>>>>> v4.9.227
}

static int tegra_gpio_get(struct gpio_chip *chip, unsigned offset)
{
<<<<<<< HEAD
	/* If gpio is in output mode then read from the out value */
	if ((tegra_gpio_readl(GPIO_OE(offset)) >> GPIO_BIT(offset)) & 1)
		return (tegra_gpio_readl(GPIO_OUT(offset)) >>
				GPIO_BIT(offset)) & 0x1;

	return (tegra_gpio_readl(GPIO_IN(offset)) >> GPIO_BIT(offset)) & 0x1;
=======
	struct tegra_gpio_info *tgi = gpiochip_get_data(chip);
	int bval = BIT(GPIO_BIT(offset));

	/* If gpio is in output mode then read from the out value */
	if (tegra_gpio_readl(tgi, GPIO_OE(tgi, offset)) & bval)
		return !!(tegra_gpio_readl(tgi, GPIO_OUT(tgi, offset)) & bval);

	return !!(tegra_gpio_readl(tgi, GPIO_IN(tgi, offset)) & bval);
>>>>>>> v4.9.227
}

static int tegra_gpio_direction_input(struct gpio_chip *chip, unsigned offset)
{
<<<<<<< HEAD
	tegra_gpio_mask_write(GPIO_MSK_OE(offset), offset, 0);
	tegra_gpio_enable(offset);
=======
	struct tegra_gpio_info *tgi = gpiochip_get_data(chip);

	tegra_gpio_mask_write(tgi, GPIO_MSK_OE(tgi, offset), offset, 0);
	tegra_gpio_enable(tgi, offset);
>>>>>>> v4.9.227
	return 0;
}

static int tegra_gpio_direction_output(struct gpio_chip *chip, unsigned offset,
					int value)
{
<<<<<<< HEAD
	tegra_gpio_set(chip, offset, value);
	tegra_gpio_mask_write(GPIO_MSK_OE(offset), offset, 1);
	tegra_gpio_enable(offset);
=======
	struct tegra_gpio_info *tgi = gpiochip_get_data(chip);

	tegra_gpio_set(chip, offset, value);
	tegra_gpio_mask_write(tgi, GPIO_MSK_OE(tgi, offset), offset, 1);
	tegra_gpio_enable(tgi, offset);
	return 0;
}

static int tegra_gpio_get_direction(struct gpio_chip *chip, unsigned offset)
{
	struct tegra_gpio_info *tgi = gpiochip_get_data(chip);
	u32 pin_mask = BIT(GPIO_BIT(offset));
	u32 cnf, oe;

	cnf = tegra_gpio_readl(tgi, GPIO_CNF(tgi, offset));
	if (!(cnf & pin_mask))
		return -EINVAL;

	oe = tegra_gpio_readl(tgi, GPIO_OE(tgi, offset));

	return (oe & pin_mask) ? GPIOF_DIR_OUT : GPIOF_DIR_IN;
}

static int tegra_gpio_set_debounce(struct gpio_chip *chip, unsigned int offset,
				   unsigned int debounce)
{
	struct tegra_gpio_info *tgi = gpiochip_get_data(chip);
	struct tegra_gpio_bank *bank = &tgi->bank_info[GPIO_BANK(offset)];
	unsigned int debounce_ms = DIV_ROUND_UP(debounce, 1000);
	unsigned long flags;
	int port;

	if (!debounce_ms) {
		tegra_gpio_mask_write(tgi, GPIO_MSK_DBC_EN(tgi, offset),
				      offset, 0);
		return 0;
	}

	debounce_ms = min(debounce_ms, 255U);
	port = GPIO_PORT(offset);

	/* There is only one debounce count register per port and hence
	 * set the maximum of current and requested debounce time.
	 */
	spin_lock_irqsave(&bank->dbc_lock[port], flags);
	if (bank->dbc_cnt[port] < debounce_ms) {
		tegra_gpio_writel(tgi, debounce_ms, GPIO_DBC_CNT(tgi, offset));
		bank->dbc_cnt[port] = debounce_ms;
	}
	spin_unlock_irqrestore(&bank->dbc_lock[port], flags);

	tegra_gpio_mask_write(tgi, GPIO_MSK_DBC_EN(tgi, offset), offset, 1);

>>>>>>> v4.9.227
	return 0;
}

static int tegra_gpio_to_irq(struct gpio_chip *chip, unsigned offset)
{
<<<<<<< HEAD
	return irq_find_mapping(irq_domain, offset);
}

static struct gpio_chip tegra_gpio_chip = {
	.label			= "tegra-gpio",
	.request		= tegra_gpio_request,
	.free			= tegra_gpio_free,
	.direction_input	= tegra_gpio_direction_input,
	.get			= tegra_gpio_get,
	.direction_output	= tegra_gpio_direction_output,
	.set			= tegra_gpio_set,
	.to_irq			= tegra_gpio_to_irq,
	.base			= 0,
};

static void tegra_gpio_irq_ack(struct irq_data *d)
{
	int gpio = d->hwirq;

	tegra_gpio_writel(1 << GPIO_BIT(gpio), GPIO_INT_CLR(gpio));
=======
	struct tegra_gpio_info *tgi = gpiochip_get_data(chip);

	return irq_find_mapping(tgi->irq_domain, offset);
}

static void tegra_gpio_irq_ack(struct irq_data *d)
{
	struct tegra_gpio_bank *bank = irq_data_get_irq_chip_data(d);
	struct tegra_gpio_info *tgi = bank->tgi;
	int gpio = d->hwirq;

	tegra_gpio_writel(tgi, 1 << GPIO_BIT(gpio), GPIO_INT_CLR(tgi, gpio));
>>>>>>> v4.9.227
}

static void tegra_gpio_irq_mask(struct irq_data *d)
{
<<<<<<< HEAD
	int gpio = d->hwirq;

	tegra_gpio_mask_write(GPIO_MSK_INT_ENB(gpio), gpio, 0);
=======
	struct tegra_gpio_bank *bank = irq_data_get_irq_chip_data(d);
	struct tegra_gpio_info *tgi = bank->tgi;
	int gpio = d->hwirq;

	tegra_gpio_mask_write(tgi, GPIO_MSK_INT_ENB(tgi, gpio), gpio, 0);
>>>>>>> v4.9.227
}

static void tegra_gpio_irq_unmask(struct irq_data *d)
{
<<<<<<< HEAD
	int gpio = d->hwirq;

	tegra_gpio_mask_write(GPIO_MSK_INT_ENB(gpio), gpio, 1);
=======
	struct tegra_gpio_bank *bank = irq_data_get_irq_chip_data(d);
	struct tegra_gpio_info *tgi = bank->tgi;
	int gpio = d->hwirq;

	tegra_gpio_mask_write(tgi, GPIO_MSK_INT_ENB(tgi, gpio), gpio, 1);
>>>>>>> v4.9.227
}

static int tegra_gpio_irq_set_type(struct irq_data *d, unsigned int type)
{
	int gpio = d->hwirq;
	struct tegra_gpio_bank *bank = irq_data_get_irq_chip_data(d);
<<<<<<< HEAD
=======
	struct tegra_gpio_info *tgi = bank->tgi;
>>>>>>> v4.9.227
	int port = GPIO_PORT(gpio);
	int lvl_type;
	int val;
	unsigned long flags;
	int ret;

	switch (type & IRQ_TYPE_SENSE_MASK) {
	case IRQ_TYPE_EDGE_RISING:
		lvl_type = GPIO_INT_LVL_EDGE_RISING;
		break;

	case IRQ_TYPE_EDGE_FALLING:
		lvl_type = GPIO_INT_LVL_EDGE_FALLING;
		break;

	case IRQ_TYPE_EDGE_BOTH:
		lvl_type = GPIO_INT_LVL_EDGE_BOTH;
		break;

	case IRQ_TYPE_LEVEL_HIGH:
		lvl_type = GPIO_INT_LVL_LEVEL_HIGH;
		break;

	case IRQ_TYPE_LEVEL_LOW:
		lvl_type = GPIO_INT_LVL_LEVEL_LOW;
		break;

	default:
		return -EINVAL;
	}

<<<<<<< HEAD
	ret = gpio_lock_as_irq(&tegra_gpio_chip, gpio);
	if (ret) {
		dev_err(dev, "unable to lock Tegra GPIO %d as IRQ\n", gpio);
=======
	ret = gpiochip_lock_as_irq(&tgi->gc, gpio);
	if (ret) {
		dev_err(tgi->dev,
			"unable to lock Tegra GPIO %d as IRQ\n", gpio);
>>>>>>> v4.9.227
		return ret;
	}

	spin_lock_irqsave(&bank->lvl_lock[port], flags);

<<<<<<< HEAD
	val = tegra_gpio_readl(GPIO_INT_LVL(gpio));
	val &= ~(GPIO_INT_LVL_MASK << GPIO_BIT(gpio));
	val |= lvl_type << GPIO_BIT(gpio);
	tegra_gpio_writel(val, GPIO_INT_LVL(gpio));

	spin_unlock_irqrestore(&bank->lvl_lock[port], flags);

	tegra_gpio_mask_write(GPIO_MSK_OE(gpio), gpio, 0);
	tegra_gpio_enable(gpio);

	if (type & (IRQ_TYPE_LEVEL_LOW | IRQ_TYPE_LEVEL_HIGH))
		__irq_set_handler_locked(d->irq, handle_level_irq);
	else if (type & (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING))
		__irq_set_handler_locked(d->irq, handle_edge_irq);
=======
	val = tegra_gpio_readl(tgi, GPIO_INT_LVL(tgi, gpio));
	val &= ~(GPIO_INT_LVL_MASK << GPIO_BIT(gpio));
	val |= lvl_type << GPIO_BIT(gpio);
	tegra_gpio_writel(tgi, val, GPIO_INT_LVL(tgi, gpio));

	spin_unlock_irqrestore(&bank->lvl_lock[port], flags);

	tegra_gpio_mask_write(tgi, GPIO_MSK_OE(tgi, gpio), gpio, 0);
	tegra_gpio_enable(tgi, gpio);

	if (type & (IRQ_TYPE_LEVEL_LOW | IRQ_TYPE_LEVEL_HIGH))
		irq_set_handler_locked(d, handle_level_irq);
	else if (type & (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING))
		irq_set_handler_locked(d, handle_edge_irq);
>>>>>>> v4.9.227

	return 0;
}

static void tegra_gpio_irq_shutdown(struct irq_data *d)
{
<<<<<<< HEAD
	int gpio = d->hwirq;

	gpio_unlock_as_irq(&tegra_gpio_chip, gpio);
}

static void tegra_gpio_irq_handler(unsigned int irq, struct irq_desc *desc)
{
	struct tegra_gpio_bank *bank;
	int port;
	int pin;
	int unmasked = 0;
	struct irq_chip *chip = irq_desc_get_chip(desc);

	chained_irq_enter(chip, desc);

	bank = irq_get_handler_data(irq);

	for (port = 0; port < 4; port++) {
		int gpio = tegra_gpio_compose(bank->bank, port, 0);
		unsigned long sta = tegra_gpio_readl(GPIO_INT_STA(gpio)) &
			tegra_gpio_readl(GPIO_INT_ENB(gpio));
		u32 lvl = tegra_gpio_readl(GPIO_INT_LVL(gpio));

		for_each_set_bit(pin, &sta, 8) {
			tegra_gpio_writel(1 << pin, GPIO_INT_CLR(gpio));

			/* if gpio is edge triggered, clear condition
			 * before executing the hander so that we don't
=======
	struct tegra_gpio_bank *bank = irq_data_get_irq_chip_data(d);
	struct tegra_gpio_info *tgi = bank->tgi;
	int gpio = d->hwirq;

	tegra_gpio_irq_mask(d);
	gpiochip_unlock_as_irq(&tgi->gc, gpio);
}

static void tegra_gpio_irq_handler(struct irq_desc *desc)
{
	int port;
	int pin;
	int unmasked = 0;
	int gpio;
	u32 lvl;
	unsigned long sta;
	struct irq_chip *chip = irq_desc_get_chip(desc);
	struct tegra_gpio_bank *bank = irq_desc_get_handler_data(desc);
	struct tegra_gpio_info *tgi = bank->tgi;

	chained_irq_enter(chip, desc);

	for (port = 0; port < 4; port++) {
		gpio = tegra_gpio_compose(bank->bank, port, 0);
		sta = tegra_gpio_readl(tgi, GPIO_INT_STA(tgi, gpio)) &
			tegra_gpio_readl(tgi, GPIO_INT_ENB(tgi, gpio));
		lvl = tegra_gpio_readl(tgi, GPIO_INT_LVL(tgi, gpio));

		for_each_set_bit(pin, &sta, 8) {
			tegra_gpio_writel(tgi, 1 << pin,
					  GPIO_INT_CLR(tgi, gpio));

			/* if gpio is edge triggered, clear condition
			 * before executing the handler so that we don't
>>>>>>> v4.9.227
			 * miss edges
			 */
			if (lvl & (0x100 << pin)) {
				unmasked = 1;
				chained_irq_exit(chip, desc);
			}

			generic_handle_irq(gpio_to_irq(gpio + pin));
		}
	}

	if (!unmasked)
		chained_irq_exit(chip, desc);

}

#ifdef CONFIG_PM_SLEEP
static int tegra_gpio_resume(struct device *dev)
{
<<<<<<< HEAD
=======
	struct platform_device *pdev = to_platform_device(dev);
	struct tegra_gpio_info *tgi = platform_get_drvdata(pdev);
>>>>>>> v4.9.227
	unsigned long flags;
	int b;
	int p;

	local_irq_save(flags);

<<<<<<< HEAD
	for (b = 0; b < tegra_gpio_bank_count; b++) {
		struct tegra_gpio_bank *bank = &tegra_gpio_banks[b];

		for (p = 0; p < ARRAY_SIZE(bank->oe); p++) {
			unsigned int gpio = (b<<5) | (p<<3);
			tegra_gpio_writel(bank->cnf[p], GPIO_CNF(gpio));
			tegra_gpio_writel(bank->out[p], GPIO_OUT(gpio));
			tegra_gpio_writel(bank->oe[p], GPIO_OE(gpio));
			tegra_gpio_writel(bank->int_lvl[p], GPIO_INT_LVL(gpio));
			tegra_gpio_writel(bank->int_enb[p], GPIO_INT_ENB(gpio));
=======
	for (b = 0; b < tgi->bank_count; b++) {
		struct tegra_gpio_bank *bank = &tgi->bank_info[b];

		for (p = 0; p < ARRAY_SIZE(bank->oe); p++) {
			unsigned int gpio = (b<<5) | (p<<3);
			tegra_gpio_writel(tgi, bank->cnf[p],
					  GPIO_CNF(tgi, gpio));

			if (tgi->soc->debounce_supported) {
				tegra_gpio_writel(tgi, bank->dbc_cnt[p],
						  GPIO_DBC_CNT(tgi, gpio));
				tegra_gpio_writel(tgi, bank->dbc_enb[p],
						  GPIO_MSK_DBC_EN(tgi, gpio));
			}

			tegra_gpio_writel(tgi, bank->out[p],
					  GPIO_OUT(tgi, gpio));
			tegra_gpio_writel(tgi, bank->oe[p],
					  GPIO_OE(tgi, gpio));
			tegra_gpio_writel(tgi, bank->int_lvl[p],
					  GPIO_INT_LVL(tgi, gpio));
			tegra_gpio_writel(tgi, bank->int_enb[p],
					  GPIO_INT_ENB(tgi, gpio));
>>>>>>> v4.9.227
		}
	}

	local_irq_restore(flags);
	return 0;
}

static int tegra_gpio_suspend(struct device *dev)
{
<<<<<<< HEAD
=======
	struct platform_device *pdev = to_platform_device(dev);
	struct tegra_gpio_info *tgi = platform_get_drvdata(pdev);
>>>>>>> v4.9.227
	unsigned long flags;
	int b;
	int p;

	local_irq_save(flags);
<<<<<<< HEAD
	for (b = 0; b < tegra_gpio_bank_count; b++) {
		struct tegra_gpio_bank *bank = &tegra_gpio_banks[b];

		for (p = 0; p < ARRAY_SIZE(bank->oe); p++) {
			unsigned int gpio = (b<<5) | (p<<3);
			bank->cnf[p] = tegra_gpio_readl(GPIO_CNF(gpio));
			bank->out[p] = tegra_gpio_readl(GPIO_OUT(gpio));
			bank->oe[p] = tegra_gpio_readl(GPIO_OE(gpio));
			bank->int_enb[p] = tegra_gpio_readl(GPIO_INT_ENB(gpio));
			bank->int_lvl[p] = tegra_gpio_readl(GPIO_INT_LVL(gpio));

			/* Enable gpio irq for wake up source */
			tegra_gpio_writel(bank->wake_enb[p],
					  GPIO_INT_ENB(gpio));
=======
	for (b = 0; b < tgi->bank_count; b++) {
		struct tegra_gpio_bank *bank = &tgi->bank_info[b];

		for (p = 0; p < ARRAY_SIZE(bank->oe); p++) {
			unsigned int gpio = (b<<5) | (p<<3);
			bank->cnf[p] = tegra_gpio_readl(tgi,
							GPIO_CNF(tgi, gpio));
			bank->out[p] = tegra_gpio_readl(tgi,
							GPIO_OUT(tgi, gpio));
			bank->oe[p] = tegra_gpio_readl(tgi,
						       GPIO_OE(tgi, gpio));
			if (tgi->soc->debounce_supported) {
				bank->dbc_enb[p] = tegra_gpio_readl(tgi,
						GPIO_MSK_DBC_EN(tgi, gpio));
				bank->dbc_enb[p] = (bank->dbc_enb[p] << 8) |
							bank->dbc_enb[p];
			}

			bank->int_enb[p] = tegra_gpio_readl(tgi,
						GPIO_INT_ENB(tgi, gpio));
			bank->int_lvl[p] = tegra_gpio_readl(tgi,
						GPIO_INT_LVL(tgi, gpio));

			/* Enable gpio irq for wake up source */
			tegra_gpio_writel(tgi, bank->wake_enb[p],
					  GPIO_INT_ENB(tgi, gpio));
>>>>>>> v4.9.227
		}
	}
	local_irq_restore(flags);
	return 0;
}

static int tegra_gpio_irq_set_wake(struct irq_data *d, unsigned int enable)
{
	struct tegra_gpio_bank *bank = irq_data_get_irq_chip_data(d);
	int gpio = d->hwirq;
	u32 port, bit, mask;

	port = GPIO_PORT(gpio);
	bit = GPIO_BIT(gpio);
	mask = BIT(bit);

	if (enable)
		bank->wake_enb[port] |= mask;
	else
		bank->wake_enb[port] &= ~mask;

	return irq_set_irq_wake(bank->irq, enable);
}
#endif

<<<<<<< HEAD
static struct irq_chip tegra_gpio_irq_chip = {
	.name		= "GPIO",
	.irq_ack	= tegra_gpio_irq_ack,
	.irq_mask	= tegra_gpio_irq_mask,
	.irq_unmask	= tegra_gpio_irq_unmask,
	.irq_set_type	= tegra_gpio_irq_set_type,
	.irq_shutdown	= tegra_gpio_irq_shutdown,
#ifdef CONFIG_PM_SLEEP
	.irq_set_wake	= tegra_gpio_irq_set_wake,
#endif
};

=======
#ifdef	CONFIG_DEBUG_FS

#include <linux/debugfs.h>
#include <linux/seq_file.h>

static int dbg_gpio_show(struct seq_file *s, void *unused)
{
	struct tegra_gpio_info *tgi = s->private;
	int i;
	int j;

	for (i = 0; i < tgi->bank_count; i++) {
		for (j = 0; j < 4; j++) {
			int gpio = tegra_gpio_compose(i, j, 0);
			seq_printf(s,
				"%d:%d %02x %02x %02x %02x %02x %02x %06x\n",
				i, j,
				tegra_gpio_readl(tgi, GPIO_CNF(tgi, gpio)),
				tegra_gpio_readl(tgi, GPIO_OE(tgi, gpio)),
				tegra_gpio_readl(tgi, GPIO_OUT(tgi, gpio)),
				tegra_gpio_readl(tgi, GPIO_IN(tgi, gpio)),
				tegra_gpio_readl(tgi, GPIO_INT_STA(tgi, gpio)),
				tegra_gpio_readl(tgi, GPIO_INT_ENB(tgi, gpio)),
				tegra_gpio_readl(tgi, GPIO_INT_LVL(tgi, gpio)));
		}
	}
	return 0;
}

static int dbg_gpio_open(struct inode *inode, struct file *file)
{
	return single_open(file, dbg_gpio_show, inode->i_private);
}

static const struct file_operations debug_fops = {
	.open		= dbg_gpio_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static void tegra_gpio_debuginit(struct tegra_gpio_info *tgi)
{
	(void) debugfs_create_file("tegra_gpio", S_IRUGO,
					NULL, tgi, &debug_fops);
}

#else

static inline void tegra_gpio_debuginit(struct tegra_gpio_info *tgi)
{
}

#endif

>>>>>>> v4.9.227
static const struct dev_pm_ops tegra_gpio_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(tegra_gpio_suspend, tegra_gpio_resume)
};

<<<<<<< HEAD
struct tegra_gpio_soc_config {
	u32 bank_stride;
	u32 upper_offset;
};

static struct tegra_gpio_soc_config tegra20_gpio_config = {
	.bank_stride = 0x80,
	.upper_offset = 0x800,
};

static struct tegra_gpio_soc_config tegra30_gpio_config = {
	.bank_stride = 0x100,
	.upper_offset = 0x80,
};

static const struct of_device_id tegra_gpio_of_match[] = {
	{ .compatible = "nvidia,tegra30-gpio", .data = &tegra30_gpio_config },
	{ .compatible = "nvidia,tegra20-gpio", .data = &tegra20_gpio_config },
	{ },
};

/* This lock class tells lockdep that GPIO irqs are in a different
 * category than their parents, so it won't report false recursion.
=======
/*
 * This lock class tells lockdep that GPIO irqs are in a different category
 * than their parents, so it won't report false recursion.
>>>>>>> v4.9.227
 */
static struct lock_class_key gpio_lock_class;

static int tegra_gpio_probe(struct platform_device *pdev)
{
<<<<<<< HEAD
	const struct of_device_id *match;
	struct tegra_gpio_soc_config *config;
=======
	const struct tegra_gpio_soc_config *config;
	struct tegra_gpio_info *tgi;
>>>>>>> v4.9.227
	struct resource *res;
	struct tegra_gpio_bank *bank;
	int ret;
	int gpio;
	int i;
	int j;

<<<<<<< HEAD
	dev = &pdev->dev;

	match = of_match_device(tegra_gpio_of_match, &pdev->dev);
	if (!match) {
		dev_err(&pdev->dev, "Error: No device match found\n");
		return -ENODEV;
	}
	config = (struct tegra_gpio_soc_config *)match->data;

	tegra_gpio_bank_stride = config->bank_stride;
	tegra_gpio_upper_offset = config->upper_offset;

	for (;;) {
		res = platform_get_resource(pdev, IORESOURCE_IRQ, tegra_gpio_bank_count);
		if (!res)
			break;
		tegra_gpio_bank_count++;
	}
	if (!tegra_gpio_bank_count) {
=======
	config = of_device_get_match_data(&pdev->dev);
	if (!config) {
		dev_err(&pdev->dev, "Error: No device match found\n");
		return -ENODEV;
	}

	tgi = devm_kzalloc(&pdev->dev, sizeof(*tgi), GFP_KERNEL);
	if (!tgi)
		return -ENODEV;

	tgi->soc = config;
	tgi->dev = &pdev->dev;

	for (;;) {
		res = platform_get_resource(pdev, IORESOURCE_IRQ,
					    tgi->bank_count);
		if (!res)
			break;
		tgi->bank_count++;
	}
	if (!tgi->bank_count) {
>>>>>>> v4.9.227
		dev_err(&pdev->dev, "Missing IRQ resource\n");
		return -ENODEV;
	}

<<<<<<< HEAD
	tegra_gpio_chip.ngpio = tegra_gpio_bank_count * 32;

	tegra_gpio_banks = devm_kzalloc(&pdev->dev,
			tegra_gpio_bank_count * sizeof(*tegra_gpio_banks),
			GFP_KERNEL);
	if (!tegra_gpio_banks)
		return -ENODEV;

	irq_domain = irq_domain_add_linear(pdev->dev.of_node,
					   tegra_gpio_chip.ngpio,
					   &irq_domain_simple_ops, NULL);
	if (!irq_domain)
		return -ENODEV;

	for (i = 0; i < tegra_gpio_bank_count; i++) {
=======
	tgi->gc.label			= "tegra-gpio";
	tgi->gc.request			= tegra_gpio_request;
	tgi->gc.free			= tegra_gpio_free;
	tgi->gc.direction_input		= tegra_gpio_direction_input;
	tgi->gc.get			= tegra_gpio_get;
	tgi->gc.direction_output	= tegra_gpio_direction_output;
	tgi->gc.set			= tegra_gpio_set;
	tgi->gc.get_direction		= tegra_gpio_get_direction;
	tgi->gc.to_irq			= tegra_gpio_to_irq;
	tgi->gc.base			= 0;
	tgi->gc.ngpio			= tgi->bank_count * 32;
	tgi->gc.parent			= &pdev->dev;
	tgi->gc.of_node			= pdev->dev.of_node;

	tgi->ic.name			= "GPIO";
	tgi->ic.irq_ack			= tegra_gpio_irq_ack;
	tgi->ic.irq_mask		= tegra_gpio_irq_mask;
	tgi->ic.irq_unmask		= tegra_gpio_irq_unmask;
	tgi->ic.irq_set_type		= tegra_gpio_irq_set_type;
	tgi->ic.irq_shutdown		= tegra_gpio_irq_shutdown;
#ifdef CONFIG_PM_SLEEP
	tgi->ic.irq_set_wake		= tegra_gpio_irq_set_wake;
#endif

	platform_set_drvdata(pdev, tgi);

	if (config->debounce_supported)
		tgi->gc.set_debounce = tegra_gpio_set_debounce;

	tgi->bank_info = devm_kzalloc(&pdev->dev, tgi->bank_count *
				      sizeof(*tgi->bank_info), GFP_KERNEL);
	if (!tgi->bank_info)
		return -ENODEV;

	tgi->irq_domain = irq_domain_add_linear(pdev->dev.of_node,
						tgi->gc.ngpio,
						&irq_domain_simple_ops, NULL);
	if (!tgi->irq_domain)
		return -ENODEV;

	for (i = 0; i < tgi->bank_count; i++) {
>>>>>>> v4.9.227
		res = platform_get_resource(pdev, IORESOURCE_IRQ, i);
		if (!res) {
			dev_err(&pdev->dev, "Missing IRQ resource\n");
			return -ENODEV;
		}

<<<<<<< HEAD
		bank = &tegra_gpio_banks[i];
		bank->bank = i;
		bank->irq = res->start;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(regs))
		return PTR_ERR(regs);

	for (i = 0; i < tegra_gpio_bank_count; i++) {
		for (j = 0; j < 4; j++) {
			int gpio = tegra_gpio_compose(i, j, 0);
			tegra_gpio_writel(0x00, GPIO_INT_ENB(gpio));
		}
	}

	tegra_gpio_chip.of_node = pdev->dev.of_node;

	ret = gpiochip_add(&tegra_gpio_chip);
	if (ret < 0) {
		irq_domain_remove(irq_domain);
		return ret;
	}

	for (gpio = 0; gpio < tegra_gpio_chip.ngpio; gpio++) {
		int irq = irq_create_mapping(irq_domain, gpio);
		/* No validity check; all Tegra GPIOs are valid IRQs */

		bank = &tegra_gpio_banks[GPIO_BANK(gpio)];

		irq_set_lockdep_class(irq, &gpio_lock_class);
		irq_set_chip_data(irq, bank);
		irq_set_chip_and_handler(irq, &tegra_gpio_irq_chip,
					 handle_simple_irq);
		set_irq_flags(irq, IRQF_VALID);
	}

	for (i = 0; i < tegra_gpio_bank_count; i++) {
		bank = &tegra_gpio_banks[i];

		irq_set_chained_handler(bank->irq, tegra_gpio_irq_handler);
		irq_set_handler_data(bank->irq, bank);

		for (j = 0; j < 4; j++)
			spin_lock_init(&bank->lvl_lock[j]);
	}

	return 0;
}

static struct platform_driver tegra_gpio_driver = {
	.driver		= {
		.name	= "tegra-gpio",
		.owner	= THIS_MODULE,
=======
		bank = &tgi->bank_info[i];
		bank->bank = i;
		bank->irq = res->start;
		bank->tgi = tgi;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	tgi->regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(tgi->regs))
		return PTR_ERR(tgi->regs);

	for (i = 0; i < tgi->bank_count; i++) {
		for (j = 0; j < 4; j++) {
			int gpio = tegra_gpio_compose(i, j, 0);
			tegra_gpio_writel(tgi, 0x00, GPIO_INT_ENB(tgi, gpio));
		}
	}

	ret = devm_gpiochip_add_data(&pdev->dev, &tgi->gc, tgi);
	if (ret < 0) {
		irq_domain_remove(tgi->irq_domain);
		return ret;
	}

	for (gpio = 0; gpio < tgi->gc.ngpio; gpio++) {
		int irq = irq_create_mapping(tgi->irq_domain, gpio);
		/* No validity check; all Tegra GPIOs are valid IRQs */

		bank = &tgi->bank_info[GPIO_BANK(gpio)];

		irq_set_lockdep_class(irq, &gpio_lock_class);
		irq_set_chip_data(irq, bank);
		irq_set_chip_and_handler(irq, &tgi->ic, handle_simple_irq);
	}

	for (i = 0; i < tgi->bank_count; i++) {
		bank = &tgi->bank_info[i];

		irq_set_chained_handler_and_data(bank->irq,
						 tegra_gpio_irq_handler, bank);

		for (j = 0; j < 4; j++) {
			spin_lock_init(&bank->lvl_lock[j]);
			spin_lock_init(&bank->dbc_lock[j]);
		}
	}

	tegra_gpio_debuginit(tgi);

	return 0;
}

static const struct tegra_gpio_soc_config tegra20_gpio_config = {
	.bank_stride = 0x80,
	.upper_offset = 0x800,
};

static const struct tegra_gpio_soc_config tegra30_gpio_config = {
	.bank_stride = 0x100,
	.upper_offset = 0x80,
};

static const struct tegra_gpio_soc_config tegra210_gpio_config = {
	.debounce_supported = true,
	.bank_stride = 0x100,
	.upper_offset = 0x80,
};

static const struct of_device_id tegra_gpio_of_match[] = {
	{ .compatible = "nvidia,tegra210-gpio", .data = &tegra210_gpio_config },
	{ .compatible = "nvidia,tegra30-gpio", .data = &tegra30_gpio_config },
	{ .compatible = "nvidia,tegra20-gpio", .data = &tegra20_gpio_config },
	{ },
};

static struct platform_driver tegra_gpio_driver = {
	.driver		= {
		.name	= "tegra-gpio",
>>>>>>> v4.9.227
		.pm	= &tegra_gpio_pm_ops,
		.of_match_table = tegra_gpio_of_match,
	},
	.probe		= tegra_gpio_probe,
};

static int __init tegra_gpio_init(void)
{
	return platform_driver_register(&tegra_gpio_driver);
}
<<<<<<< HEAD
postcore_initcall(tegra_gpio_init);

#ifdef	CONFIG_DEBUG_FS

#include <linux/debugfs.h>
#include <linux/seq_file.h>

static int dbg_gpio_show(struct seq_file *s, void *unused)
{
	int i;
	int j;

	for (i = 0; i < tegra_gpio_bank_count; i++) {
		for (j = 0; j < 4; j++) {
			int gpio = tegra_gpio_compose(i, j, 0);
			seq_printf(s,
				"%d:%d %02x %02x %02x %02x %02x %02x %06x\n",
				i, j,
				tegra_gpio_readl(GPIO_CNF(gpio)),
				tegra_gpio_readl(GPIO_OE(gpio)),
				tegra_gpio_readl(GPIO_OUT(gpio)),
				tegra_gpio_readl(GPIO_IN(gpio)),
				tegra_gpio_readl(GPIO_INT_STA(gpio)),
				tegra_gpio_readl(GPIO_INT_ENB(gpio)),
				tegra_gpio_readl(GPIO_INT_LVL(gpio)));
		}
	}
	return 0;
}

static int dbg_gpio_open(struct inode *inode, struct file *file)
{
	return single_open(file, dbg_gpio_show, &inode->i_private);
}

static const struct file_operations debug_fops = {
	.open		= dbg_gpio_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init tegra_gpio_debuginit(void)
{
	(void) debugfs_create_file("tegra_gpio", S_IRUGO,
					NULL, NULL, &debug_fops);
	return 0;
}
late_initcall(tegra_gpio_debuginit);
#endif
=======
subsys_initcall(tegra_gpio_init);
>>>>>>> v4.9.227
