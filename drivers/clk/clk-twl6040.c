/*
* TWL6040 clock module driver for OMAP4 McPDM functional clock
*
* Copyright (C) 2012 Texas Instruments Inc.
* Peter Ujfalusi <peter.ujfalusi@ti.com>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* version 2 as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301 USA
*
*/

<<<<<<< HEAD
#include <linux/clk.h>
=======
>>>>>>> v4.9.227
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/mfd/twl6040.h>
#include <linux/clk-provider.h>

<<<<<<< HEAD
struct twl6040_clk {
	struct twl6040 *twl6040;
	struct device *dev;
	struct clk_hw mcpdm_fclk;
	struct clk *clk;
	int enabled;
};

static int twl6040_bitclk_is_enabled(struct clk_hw *hw)
{
	struct twl6040_clk *twl6040_clk = container_of(hw, struct twl6040_clk,
						       mcpdm_fclk);
	return twl6040_clk->enabled;
}

static int twl6040_bitclk_prepare(struct clk_hw *hw)
{
	struct twl6040_clk *twl6040_clk = container_of(hw, struct twl6040_clk,
						       mcpdm_fclk);
	int ret;

	ret = twl6040_power(twl6040_clk->twl6040, 1);
	if (!ret)
		twl6040_clk->enabled = 1;
=======
struct twl6040_pdmclk {
	struct twl6040 *twl6040;
	struct device *dev;
	struct clk_hw pdmclk_hw;
	int enabled;
};

static int twl6040_pdmclk_is_prepared(struct clk_hw *hw)
{
	struct twl6040_pdmclk *pdmclk = container_of(hw, struct twl6040_pdmclk,
						     pdmclk_hw);

	return pdmclk->enabled;
}

static int twl6040_pdmclk_reset_one_clock(struct twl6040_pdmclk *pdmclk,
					  unsigned int reg)
{
	const u8 reset_mask = TWL6040_HPLLRST;	/* Same for HPPLL and LPPLL */
	int ret;

	ret = twl6040_set_bits(pdmclk->twl6040, reg, reset_mask);
	if (ret < 0)
		return ret;

	ret = twl6040_clear_bits(pdmclk->twl6040, reg, reset_mask);
	if (ret < 0)
		return ret;

	return 0;
}

/*
 * TWL6040A2 Phoenix Audio IC erratum #6: "PDM Clock Generation Issue At
 * Cold Temperature". This affects cold boot and deeper idle states it
 * seems. The workaround consists of resetting HPPLL and LPPLL.
 */
static int twl6040_pdmclk_quirk_reset_clocks(struct twl6040_pdmclk *pdmclk)
{
	int ret;

	ret = twl6040_pdmclk_reset_one_clock(pdmclk, TWL6040_REG_HPPLLCTL);
	if (ret)
		return ret;

	ret = twl6040_pdmclk_reset_one_clock(pdmclk, TWL6040_REG_LPPLLCTL);
	if (ret)
		return ret;

	return 0;
}

static int twl6040_pdmclk_prepare(struct clk_hw *hw)
{
	struct twl6040_pdmclk *pdmclk = container_of(hw, struct twl6040_pdmclk,
						     pdmclk_hw);
	int ret;

	ret = twl6040_power(pdmclk->twl6040, 1);
	if (ret)
		return ret;

	ret = twl6040_pdmclk_quirk_reset_clocks(pdmclk);
	if (ret)
		goto out_err;

	pdmclk->enabled = 1;

	return 0;

out_err:
	dev_err(pdmclk->dev, "%s: error %i\n", __func__, ret);
	twl6040_power(pdmclk->twl6040, 0);
>>>>>>> v4.9.227

	return ret;
}

<<<<<<< HEAD
static void twl6040_bitclk_unprepare(struct clk_hw *hw)
{
	struct twl6040_clk *twl6040_clk = container_of(hw, struct twl6040_clk,
						       mcpdm_fclk);
	int ret;

	ret = twl6040_power(twl6040_clk->twl6040, 0);
	if (!ret)
		twl6040_clk->enabled = 0;
}

static const struct clk_ops twl6040_mcpdm_ops = {
	.is_enabled = twl6040_bitclk_is_enabled,
	.prepare = twl6040_bitclk_prepare,
	.unprepare = twl6040_bitclk_unprepare,
};

static struct clk_init_data wm831x_clkout_init = {
	.name = "mcpdm_fclk",
	.ops = &twl6040_mcpdm_ops,
	.flags = CLK_IS_ROOT,
};

static int twl6040_clk_probe(struct platform_device *pdev)
{
	struct twl6040 *twl6040 = dev_get_drvdata(pdev->dev.parent);
	struct twl6040_clk *clkdata;
=======
static void twl6040_pdmclk_unprepare(struct clk_hw *hw)
{
	struct twl6040_pdmclk *pdmclk = container_of(hw, struct twl6040_pdmclk,
						     pdmclk_hw);
	int ret;

	ret = twl6040_power(pdmclk->twl6040, 0);
	if (!ret)
		pdmclk->enabled = 0;

}

static unsigned long twl6040_pdmclk_recalc_rate(struct clk_hw *hw,
						unsigned long parent_rate)
{
	struct twl6040_pdmclk *pdmclk = container_of(hw, struct twl6040_pdmclk,
						     pdmclk_hw);

	return twl6040_get_sysclk(pdmclk->twl6040);
}

static const struct clk_ops twl6040_pdmclk_ops = {
	.is_prepared = twl6040_pdmclk_is_prepared,
	.prepare = twl6040_pdmclk_prepare,
	.unprepare = twl6040_pdmclk_unprepare,
	.recalc_rate = twl6040_pdmclk_recalc_rate,
};

static struct clk_init_data twl6040_pdmclk_init = {
	.name = "pdmclk",
	.ops = &twl6040_pdmclk_ops,
	.flags = CLK_GET_RATE_NOCACHE,
};

static int twl6040_pdmclk_probe(struct platform_device *pdev)
{
	struct twl6040 *twl6040 = dev_get_drvdata(pdev->dev.parent);
	struct twl6040_pdmclk *clkdata;
	int ret;
>>>>>>> v4.9.227

	clkdata = devm_kzalloc(&pdev->dev, sizeof(*clkdata), GFP_KERNEL);
	if (!clkdata)
		return -ENOMEM;

	clkdata->dev = &pdev->dev;
	clkdata->twl6040 = twl6040;

<<<<<<< HEAD
	clkdata->mcpdm_fclk.init = &wm831x_clkout_init;
	clkdata->clk = clk_register(&pdev->dev, &clkdata->mcpdm_fclk);
	if (IS_ERR(clkdata->clk))
		return PTR_ERR(clkdata->clk);

	platform_set_drvdata(pdev, clkdata);

	return 0;
}

static int twl6040_clk_remove(struct platform_device *pdev)
{
	struct twl6040_clk *clkdata = platform_get_drvdata(pdev);

	clk_unregister(clkdata->clk);

	return 0;
}

static struct platform_driver twl6040_clk_driver = {
	.driver = {
		.name = "twl6040-clk",
	},
	.probe = twl6040_clk_probe,
	.remove = twl6040_clk_remove,
};

module_platform_driver(twl6040_clk_driver);

MODULE_DESCRIPTION("TWL6040 clock driver for McPDM functional clock");
MODULE_AUTHOR("Peter Ujfalusi <peter.ujfalusi@ti.com>");
MODULE_ALIAS("platform:twl6040-clk");
=======
	clkdata->pdmclk_hw.init = &twl6040_pdmclk_init;
	ret = devm_clk_hw_register(&pdev->dev, &clkdata->pdmclk_hw);
	if (ret)
		return ret;

	platform_set_drvdata(pdev, clkdata);

	return of_clk_add_hw_provider(pdev->dev.parent->of_node,
				      of_clk_hw_simple_get,
				      &clkdata->pdmclk_hw);
}

static struct platform_driver twl6040_pdmclk_driver = {
	.driver = {
		.name = "twl6040-pdmclk",
	},
	.probe = twl6040_pdmclk_probe,
};

module_platform_driver(twl6040_pdmclk_driver);

MODULE_DESCRIPTION("TWL6040 clock driver for McPDM functional clock");
MODULE_AUTHOR("Peter Ujfalusi <peter.ujfalusi@ti.com>");
MODULE_ALIAS("platform:twl6040-pdmclk");
>>>>>>> v4.9.227
MODULE_LICENSE("GPL");
