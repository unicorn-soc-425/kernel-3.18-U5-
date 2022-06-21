/*
 *  Copyright (C) 2013 Boris BREZILLON <b.brezillon@overkiz.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/clk/at91_pmc.h>
#include <linux/of.h>
<<<<<<< HEAD
#include <linux/of_address.h>
#include <linux/io.h>

#include "pmc.h"

=======
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

#include "pmc.h"

DEFINE_SPINLOCK(pmc_pcr_lock);

>>>>>>> v4.9.227
#define PERIPHERAL_MAX		64

#define PERIPHERAL_AT91RM9200	0
#define PERIPHERAL_AT91SAM9X5	1

#define PERIPHERAL_ID_MIN	2
#define PERIPHERAL_ID_MAX	31
#define PERIPHERAL_MASK(id)	(1 << ((id) & PERIPHERAL_ID_MAX))

#define PERIPHERAL_RSHIFT_MASK	0x3
#define PERIPHERAL_RSHIFT(val)	(((val) >> 16) & PERIPHERAL_RSHIFT_MASK)

<<<<<<< HEAD
#define PERIPHERAL_MAX_SHIFT	4

struct clk_peripheral {
	struct clk_hw hw;
	struct at91_pmc *pmc;
=======
#define PERIPHERAL_MAX_SHIFT	3

struct clk_peripheral {
	struct clk_hw hw;
	struct regmap *regmap;
>>>>>>> v4.9.227
	u32 id;
};

#define to_clk_peripheral(hw) container_of(hw, struct clk_peripheral, hw)

struct clk_sam9x5_peripheral {
	struct clk_hw hw;
<<<<<<< HEAD
	struct at91_pmc *pmc;
	struct clk_range range;
=======
	struct regmap *regmap;
	struct clk_range range;
	spinlock_t *lock;
>>>>>>> v4.9.227
	u32 id;
	u32 div;
	bool auto_div;
};

#define to_clk_sam9x5_peripheral(hw) \
	container_of(hw, struct clk_sam9x5_peripheral, hw)

static int clk_peripheral_enable(struct clk_hw *hw)
{
	struct clk_peripheral *periph = to_clk_peripheral(hw);
<<<<<<< HEAD
	struct at91_pmc *pmc = periph->pmc;
=======
>>>>>>> v4.9.227
	int offset = AT91_PMC_PCER;
	u32 id = periph->id;

	if (id < PERIPHERAL_ID_MIN)
		return 0;
	if (id > PERIPHERAL_ID_MAX)
		offset = AT91_PMC_PCER1;
<<<<<<< HEAD
	pmc_write(pmc, offset, PERIPHERAL_MASK(id));
=======
	regmap_write(periph->regmap, offset, PERIPHERAL_MASK(id));

>>>>>>> v4.9.227
	return 0;
}

static void clk_peripheral_disable(struct clk_hw *hw)
{
	struct clk_peripheral *periph = to_clk_peripheral(hw);
<<<<<<< HEAD
	struct at91_pmc *pmc = periph->pmc;
=======
>>>>>>> v4.9.227
	int offset = AT91_PMC_PCDR;
	u32 id = periph->id;

	if (id < PERIPHERAL_ID_MIN)
		return;
	if (id > PERIPHERAL_ID_MAX)
		offset = AT91_PMC_PCDR1;
<<<<<<< HEAD
	pmc_write(pmc, offset, PERIPHERAL_MASK(id));
=======
	regmap_write(periph->regmap, offset, PERIPHERAL_MASK(id));
>>>>>>> v4.9.227
}

static int clk_peripheral_is_enabled(struct clk_hw *hw)
{
	struct clk_peripheral *periph = to_clk_peripheral(hw);
<<<<<<< HEAD
	struct at91_pmc *pmc = periph->pmc;
	int offset = AT91_PMC_PCSR;
=======
	int offset = AT91_PMC_PCSR;
	unsigned int status;
>>>>>>> v4.9.227
	u32 id = periph->id;

	if (id < PERIPHERAL_ID_MIN)
		return 1;
	if (id > PERIPHERAL_ID_MAX)
		offset = AT91_PMC_PCSR1;
<<<<<<< HEAD
	return !!(pmc_read(pmc, offset) & PERIPHERAL_MASK(id));
=======
	regmap_read(periph->regmap, offset, &status);

	return status & PERIPHERAL_MASK(id) ? 1 : 0;
>>>>>>> v4.9.227
}

static const struct clk_ops peripheral_ops = {
	.enable = clk_peripheral_enable,
	.disable = clk_peripheral_disable,
	.is_enabled = clk_peripheral_is_enabled,
};

<<<<<<< HEAD
static struct clk * __init
at91_clk_register_peripheral(struct at91_pmc *pmc, const char *name,
			     const char *parent_name, u32 id)
{
	struct clk_peripheral *periph;
	struct clk *clk = NULL;
	struct clk_init_data init;

	if (!pmc || !name || !parent_name || id > PERIPHERAL_ID_MAX)
=======
static struct clk_hw * __init
at91_clk_register_peripheral(struct regmap *regmap, const char *name,
			     const char *parent_name, u32 id)
{
	struct clk_peripheral *periph;
	struct clk_init_data init;
	struct clk_hw *hw;
	int ret;

	if (!name || !parent_name || id > PERIPHERAL_ID_MAX)
>>>>>>> v4.9.227
		return ERR_PTR(-EINVAL);

	periph = kzalloc(sizeof(*periph), GFP_KERNEL);
	if (!periph)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &peripheral_ops;
	init.parent_names = (parent_name ? &parent_name : NULL);
	init.num_parents = (parent_name ? 1 : 0);
	init.flags = 0;

	periph->id = id;
	periph->hw.init = &init;
<<<<<<< HEAD
	periph->pmc = pmc;

	clk = clk_register(NULL, &periph->hw);
	if (IS_ERR(clk))
		kfree(periph);

	return clk;
=======
	periph->regmap = regmap;

	hw = &periph->hw;
	ret = clk_hw_register(NULL, &periph->hw);
	if (ret) {
		kfree(periph);
		hw = ERR_PTR(ret);
	}

	return hw;
>>>>>>> v4.9.227
}

static void clk_sam9x5_peripheral_autodiv(struct clk_sam9x5_peripheral *periph)
{
<<<<<<< HEAD
	struct clk *parent;
=======
	struct clk_hw *parent;
>>>>>>> v4.9.227
	unsigned long parent_rate;
	int shift = 0;

	if (!periph->auto_div)
		return;

	if (periph->range.max) {
<<<<<<< HEAD
		parent = clk_get_parent_by_index(periph->hw.clk, 0);
		parent_rate = __clk_get_rate(parent);
=======
		parent = clk_hw_get_parent_by_index(&periph->hw, 0);
		parent_rate = clk_hw_get_rate(parent);
>>>>>>> v4.9.227
		if (!parent_rate)
			return;

		for (; shift < PERIPHERAL_MAX_SHIFT; shift++) {
			if (parent_rate >> shift <= periph->range.max)
				break;
		}
	}

	periph->auto_div = false;
	periph->div = shift;
}

static int clk_sam9x5_peripheral_enable(struct clk_hw *hw)
{
	struct clk_sam9x5_peripheral *periph = to_clk_sam9x5_peripheral(hw);
<<<<<<< HEAD
	struct at91_pmc *pmc = periph->pmc;
=======
	unsigned long flags;
>>>>>>> v4.9.227

	if (periph->id < PERIPHERAL_ID_MIN)
		return 0;

<<<<<<< HEAD
	pmc_write(pmc, AT91_PMC_PCR, (periph->id & AT91_PMC_PCR_PID) |
				     AT91_PMC_PCR_CMD |
				     AT91_PMC_PCR_DIV(periph->div) |
				     AT91_PMC_PCR_EN);
=======
	spin_lock_irqsave(periph->lock, flags);
	regmap_write(periph->regmap, AT91_PMC_PCR,
		     (periph->id & AT91_PMC_PCR_PID_MASK));
	regmap_update_bits(periph->regmap, AT91_PMC_PCR,
			   AT91_PMC_PCR_DIV_MASK | AT91_PMC_PCR_CMD |
			   AT91_PMC_PCR_EN,
			   AT91_PMC_PCR_DIV(periph->div) |
			   AT91_PMC_PCR_CMD |
			   AT91_PMC_PCR_EN);
	spin_unlock_irqrestore(periph->lock, flags);

>>>>>>> v4.9.227
	return 0;
}

static void clk_sam9x5_peripheral_disable(struct clk_hw *hw)
{
	struct clk_sam9x5_peripheral *periph = to_clk_sam9x5_peripheral(hw);
<<<<<<< HEAD
	struct at91_pmc *pmc = periph->pmc;
=======
	unsigned long flags;
>>>>>>> v4.9.227

	if (periph->id < PERIPHERAL_ID_MIN)
		return;

<<<<<<< HEAD
	pmc_write(pmc, AT91_PMC_PCR, (periph->id & AT91_PMC_PCR_PID) |
				     AT91_PMC_PCR_CMD);
=======
	spin_lock_irqsave(periph->lock, flags);
	regmap_write(periph->regmap, AT91_PMC_PCR,
		     (periph->id & AT91_PMC_PCR_PID_MASK));
	regmap_update_bits(periph->regmap, AT91_PMC_PCR,
			   AT91_PMC_PCR_EN | AT91_PMC_PCR_CMD,
			   AT91_PMC_PCR_CMD);
	spin_unlock_irqrestore(periph->lock, flags);
>>>>>>> v4.9.227
}

static int clk_sam9x5_peripheral_is_enabled(struct clk_hw *hw)
{
	struct clk_sam9x5_peripheral *periph = to_clk_sam9x5_peripheral(hw);
<<<<<<< HEAD
	struct at91_pmc *pmc = periph->pmc;
	int ret;
=======
	unsigned long flags;
	unsigned int status;
>>>>>>> v4.9.227

	if (periph->id < PERIPHERAL_ID_MIN)
		return 1;

<<<<<<< HEAD
	pmc_lock(pmc);
	pmc_write(pmc, AT91_PMC_PCR, (periph->id & AT91_PMC_PCR_PID));
	ret = !!(pmc_read(pmc, AT91_PMC_PCR) & AT91_PMC_PCR_EN);
	pmc_unlock(pmc);

	return ret;
=======
	spin_lock_irqsave(periph->lock, flags);
	regmap_write(periph->regmap, AT91_PMC_PCR,
		     (periph->id & AT91_PMC_PCR_PID_MASK));
	regmap_read(periph->regmap, AT91_PMC_PCR, &status);
	spin_unlock_irqrestore(periph->lock, flags);

	return status & AT91_PMC_PCR_EN ? 1 : 0;
>>>>>>> v4.9.227
}

static unsigned long
clk_sam9x5_peripheral_recalc_rate(struct clk_hw *hw,
				  unsigned long parent_rate)
{
	struct clk_sam9x5_peripheral *periph = to_clk_sam9x5_peripheral(hw);
<<<<<<< HEAD
	struct at91_pmc *pmc = periph->pmc;
	u32 tmp;
=======
	unsigned long flags;
	unsigned int status;
>>>>>>> v4.9.227

	if (periph->id < PERIPHERAL_ID_MIN)
		return parent_rate;

<<<<<<< HEAD
	pmc_lock(pmc);
	pmc_write(pmc, AT91_PMC_PCR, (periph->id & AT91_PMC_PCR_PID));
	tmp = pmc_read(pmc, AT91_PMC_PCR);
	pmc_unlock(pmc);

	if (tmp & AT91_PMC_PCR_EN) {
		periph->div = PERIPHERAL_RSHIFT(tmp);
=======
	spin_lock_irqsave(periph->lock, flags);
	regmap_write(periph->regmap, AT91_PMC_PCR,
		     (periph->id & AT91_PMC_PCR_PID_MASK));
	regmap_read(periph->regmap, AT91_PMC_PCR, &status);
	spin_unlock_irqrestore(periph->lock, flags);

	if (status & AT91_PMC_PCR_EN) {
		periph->div = PERIPHERAL_RSHIFT(status);
>>>>>>> v4.9.227
		periph->auto_div = false;
	} else {
		clk_sam9x5_peripheral_autodiv(periph);
	}

	return parent_rate >> periph->div;
}

static long clk_sam9x5_peripheral_round_rate(struct clk_hw *hw,
					     unsigned long rate,
					     unsigned long *parent_rate)
{
	int shift = 0;
	unsigned long best_rate;
	unsigned long best_diff;
	unsigned long cur_rate = *parent_rate;
	unsigned long cur_diff;
	struct clk_sam9x5_peripheral *periph = to_clk_sam9x5_peripheral(hw);

	if (periph->id < PERIPHERAL_ID_MIN || !periph->range.max)
		return *parent_rate;

	if (periph->range.max) {
<<<<<<< HEAD
		for (; shift < PERIPHERAL_MAX_SHIFT; shift++) {
=======
		for (; shift <= PERIPHERAL_MAX_SHIFT; shift++) {
>>>>>>> v4.9.227
			cur_rate = *parent_rate >> shift;
			if (cur_rate <= periph->range.max)
				break;
		}
	}

	if (rate >= cur_rate)
		return cur_rate;

	best_diff = cur_rate - rate;
	best_rate = cur_rate;
<<<<<<< HEAD
	for (; shift < PERIPHERAL_MAX_SHIFT; shift++) {
=======
	for (; shift <= PERIPHERAL_MAX_SHIFT; shift++) {
>>>>>>> v4.9.227
		cur_rate = *parent_rate >> shift;
		if (cur_rate < rate)
			cur_diff = rate - cur_rate;
		else
			cur_diff = cur_rate - rate;

		if (cur_diff < best_diff) {
			best_diff = cur_diff;
			best_rate = cur_rate;
		}

		if (!best_diff || cur_rate < rate)
			break;
	}

	return best_rate;
}

static int clk_sam9x5_peripheral_set_rate(struct clk_hw *hw,
					  unsigned long rate,
					  unsigned long parent_rate)
{
	int shift;
	struct clk_sam9x5_peripheral *periph = to_clk_sam9x5_peripheral(hw);
	if (periph->id < PERIPHERAL_ID_MIN || !periph->range.max) {
		if (parent_rate == rate)
			return 0;
		else
			return -EINVAL;
	}

	if (periph->range.max && rate > periph->range.max)
		return -EINVAL;

<<<<<<< HEAD
	for (shift = 0; shift < PERIPHERAL_MAX_SHIFT; shift++) {
=======
	for (shift = 0; shift <= PERIPHERAL_MAX_SHIFT; shift++) {
>>>>>>> v4.9.227
		if (parent_rate >> shift == rate) {
			periph->auto_div = false;
			periph->div = shift;
			return 0;
		}
	}

	return -EINVAL;
}

static const struct clk_ops sam9x5_peripheral_ops = {
	.enable = clk_sam9x5_peripheral_enable,
	.disable = clk_sam9x5_peripheral_disable,
	.is_enabled = clk_sam9x5_peripheral_is_enabled,
	.recalc_rate = clk_sam9x5_peripheral_recalc_rate,
	.round_rate = clk_sam9x5_peripheral_round_rate,
	.set_rate = clk_sam9x5_peripheral_set_rate,
};

<<<<<<< HEAD
static struct clk * __init
at91_clk_register_sam9x5_peripheral(struct at91_pmc *pmc, const char *name,
				    const char *parent_name, u32 id,
				    const struct clk_range *range)
{
	struct clk_sam9x5_peripheral *periph;
	struct clk *clk = NULL;
	struct clk_init_data init;

	if (!pmc || !name || !parent_name)
=======
static struct clk_hw * __init
at91_clk_register_sam9x5_peripheral(struct regmap *regmap, spinlock_t *lock,
				    const char *name, const char *parent_name,
				    u32 id, const struct clk_range *range)
{
	struct clk_sam9x5_peripheral *periph;
	struct clk_init_data init;
	struct clk_hw *hw;
	int ret;

	if (!name || !parent_name)
>>>>>>> v4.9.227
		return ERR_PTR(-EINVAL);

	periph = kzalloc(sizeof(*periph), GFP_KERNEL);
	if (!periph)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &sam9x5_peripheral_ops;
	init.parent_names = (parent_name ? &parent_name : NULL);
	init.num_parents = (parent_name ? 1 : 0);
	init.flags = 0;

	periph->id = id;
	periph->hw.init = &init;
	periph->div = 0;
<<<<<<< HEAD
	periph->pmc = pmc;
	periph->auto_div = true;
	periph->range = *range;

	clk = clk_register(NULL, &periph->hw);
	if (IS_ERR(clk))
		kfree(periph);
	else
		clk_sam9x5_peripheral_autodiv(periph);

	return clk;
}

static void __init
of_at91_clk_periph_setup(struct device_node *np, struct at91_pmc *pmc, u8 type)
{
	int num;
	u32 id;
	struct clk *clk;
	const char *parent_name;
	const char *name;
	struct device_node *periphclknp;
=======
	periph->regmap = regmap;
	periph->lock = lock;
	periph->auto_div = true;
	periph->range = *range;

	hw = &periph->hw;
	ret = clk_hw_register(NULL, &periph->hw);
	if (ret) {
		kfree(periph);
		hw = ERR_PTR(ret);
	} else
		clk_sam9x5_peripheral_autodiv(periph);

	return hw;
}

static void __init
of_at91_clk_periph_setup(struct device_node *np, u8 type)
{
	int num;
	u32 id;
	struct clk_hw *hw;
	const char *parent_name;
	const char *name;
	struct device_node *periphclknp;
	struct regmap *regmap;
>>>>>>> v4.9.227

	parent_name = of_clk_get_parent_name(np, 0);
	if (!parent_name)
		return;

	num = of_get_child_count(np);
	if (!num || num > PERIPHERAL_MAX)
		return;

<<<<<<< HEAD
=======
	regmap = syscon_node_to_regmap(of_get_parent(np));
	if (IS_ERR(regmap))
		return;

>>>>>>> v4.9.227
	for_each_child_of_node(np, periphclknp) {
		if (of_property_read_u32(periphclknp, "reg", &id))
			continue;

		if (id >= PERIPHERAL_MAX)
			continue;

		if (of_property_read_string(np, "clock-output-names", &name))
			name = periphclknp->name;

		if (type == PERIPHERAL_AT91RM9200) {
<<<<<<< HEAD
			clk = at91_clk_register_peripheral(pmc, name,
=======
			hw = at91_clk_register_peripheral(regmap, name,
>>>>>>> v4.9.227
							   parent_name, id);
		} else {
			struct clk_range range = CLK_RANGE(0, 0);

			of_at91_get_clk_range(periphclknp,
					      "atmel,clk-output-range",
					      &range);

<<<<<<< HEAD
			clk = at91_clk_register_sam9x5_peripheral(pmc, name,
=======
			hw = at91_clk_register_sam9x5_peripheral(regmap,
								  &pmc_pcr_lock,
								  name,
>>>>>>> v4.9.227
								  parent_name,
								  id, &range);
		}

<<<<<<< HEAD
		if (IS_ERR(clk))
			continue;

		of_clk_add_provider(periphclknp, of_clk_src_simple_get, clk);
	}
}

void __init of_at91rm9200_clk_periph_setup(struct device_node *np,
					   struct at91_pmc *pmc)
{
	of_at91_clk_periph_setup(np, pmc, PERIPHERAL_AT91RM9200);
}

void __init of_at91sam9x5_clk_periph_setup(struct device_node *np,
					   struct at91_pmc *pmc)
{
	of_at91_clk_periph_setup(np, pmc, PERIPHERAL_AT91SAM9X5);
}
=======
		if (IS_ERR(hw))
			continue;

		of_clk_add_hw_provider(periphclknp, of_clk_hw_simple_get, hw);
	}
}

static void __init of_at91rm9200_clk_periph_setup(struct device_node *np)
{
	of_at91_clk_periph_setup(np, PERIPHERAL_AT91RM9200);
}
CLK_OF_DECLARE(at91rm9200_clk_periph, "atmel,at91rm9200-clk-peripheral",
	       of_at91rm9200_clk_periph_setup);

static void __init of_at91sam9x5_clk_periph_setup(struct device_node *np)
{
	of_at91_clk_periph_setup(np, PERIPHERAL_AT91SAM9X5);
}
CLK_OF_DECLARE(at91sam9x5_clk_periph, "atmel,at91sam9x5-clk-peripheral",
	       of_at91sam9x5_clk_periph_setup);

>>>>>>> v4.9.227
