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
#include <linux/irq.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
=======
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
>>>>>>> v4.9.227

#include "pmc.h"

#define SYSTEM_MAX_ID		31

#define SYSTEM_MAX_NAME_SZ	32

#define to_clk_system(hw) container_of(hw, struct clk_system, hw)
struct clk_system {
	struct clk_hw hw;
<<<<<<< HEAD
	struct at91_pmc *pmc;
	unsigned int irq;
	wait_queue_head_t wait;
=======
	struct regmap *regmap;
>>>>>>> v4.9.227
	u8 id;
};

static inline int is_pck(int id)
{
	return (id >= 8) && (id <= 15);
}
<<<<<<< HEAD
static irqreturn_t clk_system_irq_handler(int irq, void *dev_id)
{
	struct clk_system *sys = (struct clk_system *)dev_id;

	wake_up(&sys->wait);
	disable_irq_nosync(sys->irq);

	return IRQ_HANDLED;
=======

static inline bool clk_system_ready(struct regmap *regmap, int id)
{
	unsigned int status;

	regmap_read(regmap, AT91_PMC_SR, &status);

	return status & (1 << id) ? 1 : 0;
>>>>>>> v4.9.227
}

static int clk_system_prepare(struct clk_hw *hw)
{
	struct clk_system *sys = to_clk_system(hw);
<<<<<<< HEAD
	struct at91_pmc *pmc = sys->pmc;
	u32 mask = 1 << sys->id;

	pmc_write(pmc, AT91_PMC_SCER, mask);
=======

	regmap_write(sys->regmap, AT91_PMC_SCER, 1 << sys->id);
>>>>>>> v4.9.227

	if (!is_pck(sys->id))
		return 0;

<<<<<<< HEAD
	while (!(pmc_read(pmc, AT91_PMC_SR) & mask)) {
		if (sys->irq) {
			enable_irq(sys->irq);
			wait_event(sys->wait,
				   pmc_read(pmc, AT91_PMC_SR) & mask);
		} else
			cpu_relax();
	}
=======
	while (!clk_system_ready(sys->regmap, sys->id))
		cpu_relax();

>>>>>>> v4.9.227
	return 0;
}

static void clk_system_unprepare(struct clk_hw *hw)
{
	struct clk_system *sys = to_clk_system(hw);
<<<<<<< HEAD
	struct at91_pmc *pmc = sys->pmc;

	pmc_write(pmc, AT91_PMC_SCDR, 1 << sys->id);
=======

	regmap_write(sys->regmap, AT91_PMC_SCDR, 1 << sys->id);
>>>>>>> v4.9.227
}

static int clk_system_is_prepared(struct clk_hw *hw)
{
	struct clk_system *sys = to_clk_system(hw);
<<<<<<< HEAD
	struct at91_pmc *pmc = sys->pmc;

	if (!(pmc_read(pmc, AT91_PMC_SCSR) & (1 << sys->id)))
=======
	unsigned int status;

	regmap_read(sys->regmap, AT91_PMC_SCSR, &status);

	if (!(status & (1 << sys->id)))
>>>>>>> v4.9.227
		return 0;

	if (!is_pck(sys->id))
		return 1;

<<<<<<< HEAD
	return !!(pmc_read(pmc, AT91_PMC_SR) & (1 << sys->id));
=======
	regmap_read(sys->regmap, AT91_PMC_SR, &status);

	return status & (1 << sys->id) ? 1 : 0;
>>>>>>> v4.9.227
}

static const struct clk_ops system_ops = {
	.prepare = clk_system_prepare,
	.unprepare = clk_system_unprepare,
	.is_prepared = clk_system_is_prepared,
};

<<<<<<< HEAD
static struct clk * __init
at91_clk_register_system(struct at91_pmc *pmc, const char *name,
			 const char *parent_name, u8 id, int irq)
{
	struct clk_system *sys;
	struct clk *clk = NULL;
=======
static struct clk_hw * __init
at91_clk_register_system(struct regmap *regmap, const char *name,
			 const char *parent_name, u8 id)
{
	struct clk_system *sys;
	struct clk_hw *hw;
>>>>>>> v4.9.227
	struct clk_init_data init;
	int ret;

	if (!parent_name || id > SYSTEM_MAX_ID)
		return ERR_PTR(-EINVAL);

	sys = kzalloc(sizeof(*sys), GFP_KERNEL);
	if (!sys)
		return ERR_PTR(-ENOMEM);

	init.name = name;
	init.ops = &system_ops;
	init.parent_names = &parent_name;
	init.num_parents = 1;
	init.flags = CLK_SET_RATE_PARENT;

	sys->id = id;
	sys->hw.init = &init;
<<<<<<< HEAD
	sys->pmc = pmc;
	sys->irq = irq;
	if (irq) {
		init_waitqueue_head(&sys->wait);
		irq_set_status_flags(sys->irq, IRQ_NOAUTOEN);
		ret = request_irq(sys->irq, clk_system_irq_handler,
				IRQF_TRIGGER_HIGH, name, sys);
		if (ret)
			return ERR_PTR(ret);
	}

	clk = clk_register(NULL, &sys->hw);
	if (IS_ERR(clk))
		kfree(sys);

	return clk;
}

static void __init
of_at91_clk_sys_setup(struct device_node *np, struct at91_pmc *pmc)
{
	int num;
	int irq = 0;
	u32 id;
	struct clk *clk;
	const char *name;
	struct device_node *sysclknp;
	const char *parent_name;
=======
	sys->regmap = regmap;

	hw = &sys->hw;
	ret = clk_hw_register(NULL, &sys->hw);
	if (ret) {
		kfree(sys);
		hw = ERR_PTR(ret);
	}

	return hw;
}

static void __init of_at91rm9200_clk_sys_setup(struct device_node *np)
{
	int num;
	u32 id;
	struct clk_hw *hw;
	const char *name;
	struct device_node *sysclknp;
	const char *parent_name;
	struct regmap *regmap;
>>>>>>> v4.9.227

	num = of_get_child_count(np);
	if (num > (SYSTEM_MAX_ID + 1))
		return;

<<<<<<< HEAD
=======
	regmap = syscon_node_to_regmap(of_get_parent(np));
	if (IS_ERR(regmap))
		return;

>>>>>>> v4.9.227
	for_each_child_of_node(np, sysclknp) {
		if (of_property_read_u32(sysclknp, "reg", &id))
			continue;

		if (of_property_read_string(np, "clock-output-names", &name))
			name = sysclknp->name;

<<<<<<< HEAD
		if (is_pck(id))
			irq = irq_of_parse_and_map(sysclknp, 0);

		parent_name = of_clk_get_parent_name(sysclknp, 0);

		clk = at91_clk_register_system(pmc, name, parent_name, id, irq);
		if (IS_ERR(clk))
			continue;

		of_clk_add_provider(sysclknp, of_clk_src_simple_get, clk);
	}
}

void __init of_at91rm9200_clk_sys_setup(struct device_node *np,
					struct at91_pmc *pmc)
{
	of_at91_clk_sys_setup(np, pmc);
}
=======
		parent_name = of_clk_get_parent_name(sysclknp, 0);

		hw = at91_clk_register_system(regmap, name, parent_name, id);
		if (IS_ERR(hw))
			continue;

		of_clk_add_hw_provider(sysclknp, of_clk_hw_simple_get, hw);
	}
}
CLK_OF_DECLARE(at91rm9200_clk_sys, "atmel,at91rm9200-clk-system",
	       of_at91rm9200_clk_sys_setup);
>>>>>>> v4.9.227
