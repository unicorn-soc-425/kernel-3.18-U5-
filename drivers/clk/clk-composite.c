/*
 * Copyright (c) 2013 NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/err.h>
#include <linux/slab.h>

<<<<<<< HEAD
#define to_clk_composite(_hw) container_of(_hw, struct clk_composite, hw)

=======
>>>>>>> v4.9.227
static u8 clk_composite_get_parent(struct clk_hw *hw)
{
	struct clk_composite *composite = to_clk_composite(hw);
	const struct clk_ops *mux_ops = composite->mux_ops;
	struct clk_hw *mux_hw = composite->mux_hw;

<<<<<<< HEAD
	mux_hw->clk = hw->clk;
=======
	__clk_hw_set_clk(mux_hw, hw);
>>>>>>> v4.9.227

	return mux_ops->get_parent(mux_hw);
}

static int clk_composite_set_parent(struct clk_hw *hw, u8 index)
{
	struct clk_composite *composite = to_clk_composite(hw);
	const struct clk_ops *mux_ops = composite->mux_ops;
	struct clk_hw *mux_hw = composite->mux_hw;

<<<<<<< HEAD
	mux_hw->clk = hw->clk;
=======
	__clk_hw_set_clk(mux_hw, hw);
>>>>>>> v4.9.227

	return mux_ops->set_parent(mux_hw, index);
}

static unsigned long clk_composite_recalc_rate(struct clk_hw *hw,
					    unsigned long parent_rate)
{
	struct clk_composite *composite = to_clk_composite(hw);
	const struct clk_ops *rate_ops = composite->rate_ops;
	struct clk_hw *rate_hw = composite->rate_hw;

<<<<<<< HEAD
	rate_hw->clk = hw->clk;
=======
	__clk_hw_set_clk(rate_hw, hw);
>>>>>>> v4.9.227

	return rate_ops->recalc_rate(rate_hw, parent_rate);
}

<<<<<<< HEAD
static long clk_composite_determine_rate(struct clk_hw *hw, unsigned long rate,
					unsigned long *best_parent_rate,
					struct clk **best_parent_p)
=======
static int clk_composite_determine_rate(struct clk_hw *hw,
					struct clk_rate_request *req)
>>>>>>> v4.9.227
{
	struct clk_composite *composite = to_clk_composite(hw);
	const struct clk_ops *rate_ops = composite->rate_ops;
	const struct clk_ops *mux_ops = composite->mux_ops;
	struct clk_hw *rate_hw = composite->rate_hw;
	struct clk_hw *mux_hw = composite->mux_hw;
<<<<<<< HEAD
	struct clk *parent;
=======
	struct clk_hw *parent;
>>>>>>> v4.9.227
	unsigned long parent_rate;
	long tmp_rate, best_rate = 0;
	unsigned long rate_diff;
	unsigned long best_rate_diff = ULONG_MAX;
<<<<<<< HEAD
	int i;

	if (rate_hw && rate_ops && rate_ops->determine_rate) {
		rate_hw->clk = hw->clk;
		return rate_ops->determine_rate(rate_hw, rate, best_parent_rate,
						best_parent_p);
	} else if (rate_hw && rate_ops && rate_ops->round_rate &&
		   mux_hw && mux_ops && mux_ops->set_parent) {
		*best_parent_p = NULL;

		if (__clk_get_flags(hw->clk) & CLK_SET_RATE_NO_REPARENT) {
			*best_parent_p = clk_get_parent(mux_hw->clk);
			*best_parent_rate = __clk_get_rate(*best_parent_p);

			return rate_ops->round_rate(rate_hw, rate,
						    best_parent_rate);
		}

		for (i = 0; i < __clk_get_num_parents(mux_hw->clk); i++) {
			parent = clk_get_parent_by_index(mux_hw->clk, i);
			if (!parent)
				continue;

			parent_rate = __clk_get_rate(parent);

			tmp_rate = rate_ops->round_rate(rate_hw, rate,
=======
	long rate;
	int i;

	if (rate_hw && rate_ops && rate_ops->determine_rate) {
		__clk_hw_set_clk(rate_hw, hw);
		return rate_ops->determine_rate(rate_hw, req);
	} else if (rate_hw && rate_ops && rate_ops->round_rate &&
		   mux_hw && mux_ops && mux_ops->set_parent) {
		req->best_parent_hw = NULL;

		if (clk_hw_get_flags(hw) & CLK_SET_RATE_NO_REPARENT) {
			parent = clk_hw_get_parent(mux_hw);
			req->best_parent_hw = parent;
			req->best_parent_rate = clk_hw_get_rate(parent);

			rate = rate_ops->round_rate(rate_hw, req->rate,
						    &req->best_parent_rate);
			if (rate < 0)
				return rate;

			req->rate = rate;
			return 0;
		}

		for (i = 0; i < clk_hw_get_num_parents(mux_hw); i++) {
			parent = clk_hw_get_parent_by_index(mux_hw, i);
			if (!parent)
				continue;

			parent_rate = clk_hw_get_rate(parent);

			tmp_rate = rate_ops->round_rate(rate_hw, req->rate,
>>>>>>> v4.9.227
							&parent_rate);
			if (tmp_rate < 0)
				continue;

<<<<<<< HEAD
			rate_diff = abs(rate - tmp_rate);

			if (!rate_diff || !*best_parent_p
				       || best_rate_diff > rate_diff) {
				*best_parent_p = parent;
				*best_parent_rate = parent_rate;
=======
			rate_diff = abs(req->rate - tmp_rate);

			if (!rate_diff || !req->best_parent_hw
				       || best_rate_diff > rate_diff) {
				req->best_parent_hw = parent;
				req->best_parent_rate = parent_rate;
>>>>>>> v4.9.227
				best_rate_diff = rate_diff;
				best_rate = tmp_rate;
			}

			if (!rate_diff)
<<<<<<< HEAD
				return rate;
		}

		return best_rate;
	} else if (mux_hw && mux_ops && mux_ops->determine_rate) {
		mux_hw->clk = hw->clk;
		return mux_ops->determine_rate(mux_hw, rate, best_parent_rate,
					       best_parent_p);
	} else {
		pr_err("clk: clk_composite_determine_rate function called, but no mux or rate callback set!\n");
		return 0;
=======
				return 0;
		}

		req->rate = best_rate;
		return 0;
	} else if (mux_hw && mux_ops && mux_ops->determine_rate) {
		__clk_hw_set_clk(mux_hw, hw);
		return mux_ops->determine_rate(mux_hw, req);
	} else {
		pr_err("clk: clk_composite_determine_rate function called, but no mux or rate callback set!\n");
		return -EINVAL;
>>>>>>> v4.9.227
	}
}

static long clk_composite_round_rate(struct clk_hw *hw, unsigned long rate,
				  unsigned long *prate)
{
	struct clk_composite *composite = to_clk_composite(hw);
	const struct clk_ops *rate_ops = composite->rate_ops;
	struct clk_hw *rate_hw = composite->rate_hw;

<<<<<<< HEAD
	rate_hw->clk = hw->clk;
=======
	__clk_hw_set_clk(rate_hw, hw);
>>>>>>> v4.9.227

	return rate_ops->round_rate(rate_hw, rate, prate);
}

static int clk_composite_set_rate(struct clk_hw *hw, unsigned long rate,
			       unsigned long parent_rate)
{
	struct clk_composite *composite = to_clk_composite(hw);
	const struct clk_ops *rate_ops = composite->rate_ops;
	struct clk_hw *rate_hw = composite->rate_hw;

<<<<<<< HEAD
	rate_hw->clk = hw->clk;
=======
	__clk_hw_set_clk(rate_hw, hw);
>>>>>>> v4.9.227

	return rate_ops->set_rate(rate_hw, rate, parent_rate);
}

<<<<<<< HEAD
=======
static int clk_composite_set_rate_and_parent(struct clk_hw *hw,
					     unsigned long rate,
					     unsigned long parent_rate,
					     u8 index)
{
	struct clk_composite *composite = to_clk_composite(hw);
	const struct clk_ops *rate_ops = composite->rate_ops;
	const struct clk_ops *mux_ops = composite->mux_ops;
	struct clk_hw *rate_hw = composite->rate_hw;
	struct clk_hw *mux_hw = composite->mux_hw;
	unsigned long temp_rate;

	__clk_hw_set_clk(rate_hw, hw);
	__clk_hw_set_clk(mux_hw, hw);

	temp_rate = rate_ops->recalc_rate(rate_hw, parent_rate);
	if (temp_rate > rate) {
		rate_ops->set_rate(rate_hw, rate, parent_rate);
		mux_ops->set_parent(mux_hw, index);
	} else {
		mux_ops->set_parent(mux_hw, index);
		rate_ops->set_rate(rate_hw, rate, parent_rate);
	}

	return 0;
}

>>>>>>> v4.9.227
static int clk_composite_is_enabled(struct clk_hw *hw)
{
	struct clk_composite *composite = to_clk_composite(hw);
	const struct clk_ops *gate_ops = composite->gate_ops;
	struct clk_hw *gate_hw = composite->gate_hw;

<<<<<<< HEAD
	gate_hw->clk = hw->clk;
=======
	__clk_hw_set_clk(gate_hw, hw);
>>>>>>> v4.9.227

	return gate_ops->is_enabled(gate_hw);
}

static int clk_composite_enable(struct clk_hw *hw)
{
	struct clk_composite *composite = to_clk_composite(hw);
	const struct clk_ops *gate_ops = composite->gate_ops;
	struct clk_hw *gate_hw = composite->gate_hw;

<<<<<<< HEAD
	gate_hw->clk = hw->clk;
=======
	__clk_hw_set_clk(gate_hw, hw);
>>>>>>> v4.9.227

	return gate_ops->enable(gate_hw);
}

static void clk_composite_disable(struct clk_hw *hw)
{
	struct clk_composite *composite = to_clk_composite(hw);
	const struct clk_ops *gate_ops = composite->gate_ops;
	struct clk_hw *gate_hw = composite->gate_hw;

<<<<<<< HEAD
	gate_hw->clk = hw->clk;
=======
	__clk_hw_set_clk(gate_hw, hw);
>>>>>>> v4.9.227

	gate_ops->disable(gate_hw);
}

<<<<<<< HEAD
struct clk *clk_register_composite(struct device *dev, const char *name,
			const char **parent_names, int num_parents,
=======
struct clk_hw *clk_hw_register_composite(struct device *dev, const char *name,
			const char * const *parent_names, int num_parents,
>>>>>>> v4.9.227
			struct clk_hw *mux_hw, const struct clk_ops *mux_ops,
			struct clk_hw *rate_hw, const struct clk_ops *rate_ops,
			struct clk_hw *gate_hw, const struct clk_ops *gate_ops,
			unsigned long flags)
{
<<<<<<< HEAD
	struct clk *clk;
	struct clk_init_data init;
	struct clk_composite *composite;
	struct clk_ops *clk_composite_ops;

	composite = kzalloc(sizeof(*composite), GFP_KERNEL);
	if (!composite) {
		pr_err("%s: could not allocate composite clk\n", __func__);
		return ERR_PTR(-ENOMEM);
	}
=======
	struct clk_hw *hw;
	struct clk_init_data init;
	struct clk_composite *composite;
	struct clk_ops *clk_composite_ops;
	int ret;

	composite = kzalloc(sizeof(*composite), GFP_KERNEL);
	if (!composite)
		return ERR_PTR(-ENOMEM);
>>>>>>> v4.9.227

	init.name = name;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = parent_names;
	init.num_parents = num_parents;
<<<<<<< HEAD
=======
	hw = &composite->hw;
>>>>>>> v4.9.227

	clk_composite_ops = &composite->ops;

	if (mux_hw && mux_ops) {
		if (!mux_ops->get_parent) {
<<<<<<< HEAD
			clk = ERR_PTR(-EINVAL);
=======
			hw = ERR_PTR(-EINVAL);
>>>>>>> v4.9.227
			goto err;
		}

		composite->mux_hw = mux_hw;
		composite->mux_ops = mux_ops;
		clk_composite_ops->get_parent = clk_composite_get_parent;
		if (mux_ops->set_parent)
			clk_composite_ops->set_parent = clk_composite_set_parent;
		if (mux_ops->determine_rate)
			clk_composite_ops->determine_rate = clk_composite_determine_rate;
	}

	if (rate_hw && rate_ops) {
		if (!rate_ops->recalc_rate) {
<<<<<<< HEAD
			clk = ERR_PTR(-EINVAL);
=======
			hw = ERR_PTR(-EINVAL);
>>>>>>> v4.9.227
			goto err;
		}
		clk_composite_ops->recalc_rate = clk_composite_recalc_rate;

		if (rate_ops->determine_rate)
			clk_composite_ops->determine_rate =
				clk_composite_determine_rate;
		else if (rate_ops->round_rate)
			clk_composite_ops->round_rate =
				clk_composite_round_rate;

		/* .set_rate requires either .round_rate or .determine_rate */
		if (rate_ops->set_rate) {
			if (rate_ops->determine_rate || rate_ops->round_rate)
				clk_composite_ops->set_rate =
						clk_composite_set_rate;
			else
				WARN(1, "%s: missing round_rate op is required\n",
						__func__);
		}

		composite->rate_hw = rate_hw;
		composite->rate_ops = rate_ops;
	}

<<<<<<< HEAD
	if (gate_hw && gate_ops) {
		if (!gate_ops->is_enabled || !gate_ops->enable ||
		    !gate_ops->disable) {
			clk = ERR_PTR(-EINVAL);
=======
	if (mux_hw && mux_ops && rate_hw && rate_ops) {
		if (mux_ops->set_parent && rate_ops->set_rate)
			clk_composite_ops->set_rate_and_parent =
			clk_composite_set_rate_and_parent;
	}

	if (gate_hw && gate_ops) {
		if (!gate_ops->is_enabled || !gate_ops->enable ||
		    !gate_ops->disable) {
			hw = ERR_PTR(-EINVAL);
>>>>>>> v4.9.227
			goto err;
		}

		composite->gate_hw = gate_hw;
		composite->gate_ops = gate_ops;
		clk_composite_ops->is_enabled = clk_composite_is_enabled;
		clk_composite_ops->enable = clk_composite_enable;
		clk_composite_ops->disable = clk_composite_disable;
	}

	init.ops = clk_composite_ops;
	composite->hw.init = &init;

<<<<<<< HEAD
	clk = clk_register(dev, &composite->hw);
	if (IS_ERR(clk))
		goto err;

	if (composite->mux_hw)
		composite->mux_hw->clk = clk;

	if (composite->rate_hw)
		composite->rate_hw->clk = clk;

	if (composite->gate_hw)
		composite->gate_hw->clk = clk;

	return clk;

err:
	kfree(composite);
	return clk;
=======
	ret = clk_hw_register(dev, hw);
	if (ret) {
		hw = ERR_PTR(ret);
		goto err;
	}

	if (composite->mux_hw)
		composite->mux_hw->clk = hw->clk;

	if (composite->rate_hw)
		composite->rate_hw->clk = hw->clk;

	if (composite->gate_hw)
		composite->gate_hw->clk = hw->clk;

	return hw;

err:
	kfree(composite);
	return hw;
}

struct clk *clk_register_composite(struct device *dev, const char *name,
			const char * const *parent_names, int num_parents,
			struct clk_hw *mux_hw, const struct clk_ops *mux_ops,
			struct clk_hw *rate_hw, const struct clk_ops *rate_ops,
			struct clk_hw *gate_hw, const struct clk_ops *gate_ops,
			unsigned long flags)
{
	struct clk_hw *hw;

	hw = clk_hw_register_composite(dev, name, parent_names, num_parents,
			mux_hw, mux_ops, rate_hw, rate_ops, gate_hw, gate_ops,
			flags);
	if (IS_ERR(hw))
		return ERR_CAST(hw);
	return hw->clk;
}

void clk_unregister_composite(struct clk *clk)
{
	struct clk_composite *composite;
	struct clk_hw *hw;

	hw = __clk_get_hw(clk);
	if (!hw)
		return;

	composite = to_clk_composite(hw);

	clk_unregister(clk);
	kfree(composite);
>>>>>>> v4.9.227
}
