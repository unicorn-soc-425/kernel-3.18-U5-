/*
 * Copyright 2014 Chen-Yu Tsai
 *
 * Chen-Yu Tsai <wens@csie.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

<<<<<<< HEAD
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/of_address.h>

#include "clk-factors.h"

/**
 * sun8i_a23_get_mbus_factors() - calculates m factor for MBUS clocks
 * MBUS rate is calculated as follows
 * rate = parent_rate / (m + 1);
 */

static void sun8i_a23_get_mbus_factors(u32 *freq, u32 parent_rate,
				       u8 *n, u8 *k, u8 *m, u8 *p)
{
	u8 div;

	/*
	 * These clocks can only divide, so we will never be able to
	 * achieve frequencies higher than the parent frequency
	 */
	if (*freq > parent_rate)
		*freq = parent_rate;

	div = DIV_ROUND_UP(parent_rate, *freq);

	if (div > 8)
		div = 8;

	*freq = parent_rate / div;

	/* we were called to round the frequency, we can now return */
	if (m == NULL)
		return;

	*m = div - 1;
}

static struct clk_factors_config sun8i_a23_mbus_config = {
	.mshift = 0,
	.mwidth = 3,
};

static const struct factors_data sun8i_a23_mbus_data __initconst = {
	.enable = 31,
	.mux = 24,
	.table = &sun8i_a23_mbus_config,
	.getter = sun8i_a23_get_mbus_factors,
};
=======
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/clk-provider.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/of_address.h>

#define SUN8I_MBUS_ENABLE	31
#define SUN8I_MBUS_MUX_SHIFT	24
#define SUN8I_MBUS_MUX_MASK	0x3
#define SUN8I_MBUS_DIV_SHIFT	0
#define SUN8I_MBUS_DIV_WIDTH	3
#define SUN8I_MBUS_MAX_PARENTS	4
>>>>>>> v4.9.227

static DEFINE_SPINLOCK(sun8i_a23_mbus_lock);

static void __init sun8i_a23_mbus_setup(struct device_node *node)
{
<<<<<<< HEAD
	struct clk *mbus = sunxi_factors_register(node, &sun8i_a23_mbus_data,
						  &sun8i_a23_mbus_lock);

	/* The MBUS clocks needs to be always enabled */
	__clk_get(mbus);
	clk_prepare_enable(mbus);
=======
	int num_parents = of_clk_get_parent_count(node);
	const char **parents;
	const char *clk_name = node->name;
	struct resource res;
	struct clk_divider *div;
	struct clk_gate *gate;
	struct clk_mux *mux;
	struct clk *clk;
	void __iomem *reg;
	int err;

	parents = kcalloc(num_parents, sizeof(*parents), GFP_KERNEL);
	if (!parents)
		return;

	reg = of_io_request_and_map(node, 0, of_node_full_name(node));
	if (IS_ERR(reg)) {
		pr_err("Could not get registers for sun8i-mbus-clk\n");
		goto err_free_parents;
	}

	div = kzalloc(sizeof(*div), GFP_KERNEL);
	if (!div)
		goto err_unmap;

	mux = kzalloc(sizeof(*mux), GFP_KERNEL);
	if (!mux)
		goto err_free_div;

	gate = kzalloc(sizeof(*gate), GFP_KERNEL);
	if (!gate)
		goto err_free_mux;

	of_property_read_string(node, "clock-output-names", &clk_name);
	of_clk_parent_fill(node, parents, num_parents);

	gate->reg = reg;
	gate->bit_idx = SUN8I_MBUS_ENABLE;
	gate->lock = &sun8i_a23_mbus_lock;

	div->reg = reg;
	div->shift = SUN8I_MBUS_DIV_SHIFT;
	div->width = SUN8I_MBUS_DIV_WIDTH;
	div->lock = &sun8i_a23_mbus_lock;

	mux->reg = reg;
	mux->shift = SUN8I_MBUS_MUX_SHIFT;
	mux->mask = SUN8I_MBUS_MUX_MASK;
	mux->lock = &sun8i_a23_mbus_lock;

	clk = clk_register_composite(NULL, clk_name, parents, num_parents,
				     &mux->hw, &clk_mux_ops,
				     &div->hw, &clk_divider_ops,
				     &gate->hw, &clk_gate_ops,
				     0);
	if (IS_ERR(clk))
		goto err_free_gate;

	err = of_clk_add_provider(node, of_clk_src_simple_get, clk);
	if (err)
		goto err_unregister_clk;

	kfree(parents); /* parents is deep copied */
	/* The MBUS clocks needs to be always enabled */
	__clk_get(clk);
	clk_prepare_enable(clk);

	return;

err_unregister_clk:
	/* TODO: The composite clock stuff will leak a bit here. */
	clk_unregister(clk);
err_free_gate:
	kfree(gate);
err_free_mux:
	kfree(mux);
err_free_div:
	kfree(div);
err_unmap:
	iounmap(reg);
	of_address_to_resource(node, 0, &res);
	release_mem_region(res.start, resource_size(&res));
err_free_parents:
	kfree(parents);
>>>>>>> v4.9.227
}
CLK_OF_DECLARE(sun8i_a23_mbus, "allwinner,sun8i-a23-mbus-clk", sun8i_a23_mbus_setup);
