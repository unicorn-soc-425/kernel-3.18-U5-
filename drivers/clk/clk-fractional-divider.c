/*
 * Copyright (C) 2014 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Adjustable fractional divider clock implementation.
 * Output rate = (m / n) * parent_rate.
<<<<<<< HEAD
=======
 * Uses rational best approximation algorithm.
>>>>>>> v4.9.227
 */

#include <linux/clk-provider.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
<<<<<<< HEAD
#include <linux/gcd.h>

#define to_clk_fd(_hw) container_of(_hw, struct clk_fractional_divider, hw)
=======
#include <linux/rational.h>
>>>>>>> v4.9.227

static unsigned long clk_fd_recalc_rate(struct clk_hw *hw,
					unsigned long parent_rate)
{
	struct clk_fractional_divider *fd = to_clk_fd(hw);
	unsigned long flags = 0;
<<<<<<< HEAD
	u32 val, m, n;
=======
	unsigned long m, n;
	u32 val;
>>>>>>> v4.9.227
	u64 ret;

	if (fd->lock)
		spin_lock_irqsave(fd->lock, flags);
<<<<<<< HEAD
=======
	else
		__acquire(fd->lock);
>>>>>>> v4.9.227

	val = clk_readl(fd->reg);

	if (fd->lock)
		spin_unlock_irqrestore(fd->lock, flags);
<<<<<<< HEAD
=======
	else
		__release(fd->lock);
>>>>>>> v4.9.227

	m = (val & fd->mmask) >> fd->mshift;
	n = (val & fd->nmask) >> fd->nshift;

<<<<<<< HEAD
=======
	if (!n || !m)
		return parent_rate;

>>>>>>> v4.9.227
	ret = (u64)parent_rate * m;
	do_div(ret, n);

	return ret;
}

static long clk_fd_round_rate(struct clk_hw *hw, unsigned long rate,
<<<<<<< HEAD
			      unsigned long *prate)
{
	struct clk_fractional_divider *fd = to_clk_fd(hw);
	unsigned maxn = (fd->nmask >> fd->nshift) + 1;
	unsigned div;

	if (!rate || rate >= *prate)
		return *prate;

	div = gcd(*prate, rate);

	while ((*prate / div) > maxn) {
		div <<= 1;
		rate <<= 1;
	}

	return rate;
=======
			      unsigned long *parent_rate)
{
	struct clk_fractional_divider *fd = to_clk_fd(hw);
	unsigned long scale;
	unsigned long m, n;
	u64 ret;

	if (!rate || rate >= *parent_rate)
		return *parent_rate;

	/*
	 * Get rate closer to *parent_rate to guarantee there is no overflow
	 * for m and n. In the result it will be the nearest rate left shifted
	 * by (scale - fd->nwidth) bits.
	 */
	scale = fls_long(*parent_rate / rate - 1);
	if (scale > fd->nwidth)
		rate <<= scale - fd->nwidth;

	rational_best_approximation(rate, *parent_rate,
			GENMASK(fd->mwidth - 1, 0), GENMASK(fd->nwidth - 1, 0),
			&m, &n);

	ret = (u64)*parent_rate * m;
	do_div(ret, n);

	return ret;
>>>>>>> v4.9.227
}

static int clk_fd_set_rate(struct clk_hw *hw, unsigned long rate,
			   unsigned long parent_rate)
{
	struct clk_fractional_divider *fd = to_clk_fd(hw);
	unsigned long flags = 0;
<<<<<<< HEAD
	unsigned long div;
	unsigned n, m;
	u32 val;

	div = gcd(parent_rate, rate);
	m = rate / div;
	n = parent_rate / div;

	if (fd->lock)
		spin_lock_irqsave(fd->lock, flags);
=======
	unsigned long m, n;
	u32 val;

	rational_best_approximation(rate, parent_rate,
			GENMASK(fd->mwidth - 1, 0), GENMASK(fd->nwidth - 1, 0),
			&m, &n);

	if (fd->lock)
		spin_lock_irqsave(fd->lock, flags);
	else
		__acquire(fd->lock);
>>>>>>> v4.9.227

	val = clk_readl(fd->reg);
	val &= ~(fd->mmask | fd->nmask);
	val |= (m << fd->mshift) | (n << fd->nshift);
	clk_writel(val, fd->reg);

	if (fd->lock)
		spin_unlock_irqrestore(fd->lock, flags);
<<<<<<< HEAD
=======
	else
		__release(fd->lock);
>>>>>>> v4.9.227

	return 0;
}

const struct clk_ops clk_fractional_divider_ops = {
	.recalc_rate = clk_fd_recalc_rate,
	.round_rate = clk_fd_round_rate,
	.set_rate = clk_fd_set_rate,
};
EXPORT_SYMBOL_GPL(clk_fractional_divider_ops);

<<<<<<< HEAD
struct clk *clk_register_fractional_divider(struct device *dev,
=======
struct clk_hw *clk_hw_register_fractional_divider(struct device *dev,
>>>>>>> v4.9.227
		const char *name, const char *parent_name, unsigned long flags,
		void __iomem *reg, u8 mshift, u8 mwidth, u8 nshift, u8 nwidth,
		u8 clk_divider_flags, spinlock_t *lock)
{
	struct clk_fractional_divider *fd;
	struct clk_init_data init;
<<<<<<< HEAD
	struct clk *clk;

	fd = kzalloc(sizeof(*fd), GFP_KERNEL);
	if (!fd) {
		dev_err(dev, "could not allocate fractional divider clk\n");
		return ERR_PTR(-ENOMEM);
	}
=======
	struct clk_hw *hw;
	int ret;

	fd = kzalloc(sizeof(*fd), GFP_KERNEL);
	if (!fd)
		return ERR_PTR(-ENOMEM);
>>>>>>> v4.9.227

	init.name = name;
	init.ops = &clk_fractional_divider_ops;
	init.flags = flags | CLK_IS_BASIC;
	init.parent_names = parent_name ? &parent_name : NULL;
	init.num_parents = parent_name ? 1 : 0;

	fd->reg = reg;
	fd->mshift = mshift;
<<<<<<< HEAD
	fd->mmask = (BIT(mwidth) - 1) << mshift;
	fd->nshift = nshift;
	fd->nmask = (BIT(nwidth) - 1) << nshift;
=======
	fd->mwidth = mwidth;
	fd->mmask = GENMASK(mwidth - 1, 0) << mshift;
	fd->nshift = nshift;
	fd->nwidth = nwidth;
	fd->nmask = GENMASK(nwidth - 1, 0) << nshift;
>>>>>>> v4.9.227
	fd->flags = clk_divider_flags;
	fd->lock = lock;
	fd->hw.init = &init;

<<<<<<< HEAD
	clk = clk_register(dev, &fd->hw);
	if (IS_ERR(clk))
		kfree(fd);

	return clk;
}
EXPORT_SYMBOL_GPL(clk_register_fractional_divider);
=======
	hw = &fd->hw;
	ret = clk_hw_register(dev, hw);
	if (ret) {
		kfree(fd);
		hw = ERR_PTR(ret);
	}

	return hw;
}
EXPORT_SYMBOL_GPL(clk_hw_register_fractional_divider);

struct clk *clk_register_fractional_divider(struct device *dev,
		const char *name, const char *parent_name, unsigned long flags,
		void __iomem *reg, u8 mshift, u8 mwidth, u8 nshift, u8 nwidth,
		u8 clk_divider_flags, spinlock_t *lock)
{
	struct clk_hw *hw;

	hw = clk_hw_register_fractional_divider(dev, name, parent_name, flags,
			reg, mshift, mwidth, nshift, nwidth, clk_divider_flags,
			lock);
	if (IS_ERR(hw))
		return ERR_CAST(hw);
	return hw->clk;
}
EXPORT_SYMBOL_GPL(clk_register_fractional_divider);

void clk_hw_unregister_fractional_divider(struct clk_hw *hw)
{
	struct clk_fractional_divider *fd;

	fd = to_clk_fd(hw);

	clk_hw_unregister(hw);
	kfree(fd);
}
>>>>>>> v4.9.227
