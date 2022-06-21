/*
 * drivers/clk/clkdev.c
 *
 *  Copyright (C) 2008 Russell King.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Helper for the clk API to assist looking up a struct clk.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
<<<<<<< HEAD
=======
#include <linux/clk-provider.h>
>>>>>>> v4.9.227
#include <linux/of.h>

#include "clk.h"

static LIST_HEAD(clocks);
static DEFINE_MUTEX(clocks_mutex);

<<<<<<< HEAD
#if defined(CONFIG_OF)

/**
 * of_clk_get_by_clkspec() - Lookup a clock form a clock provider
 * @clkspec: pointer to a clock specifier data structure
 *
 * This function looks up a struct clk from the registered list of clock
 * providers, an input is a clock specifier data structure as returned
 * from the of_parse_phandle_with_args() function call.
 */
struct clk *of_clk_get_by_clkspec(struct of_phandle_args *clkspec)
{
	struct clk *clk;

	if (!clkspec)
		return ERR_PTR(-EINVAL);

	of_clk_lock();
	clk = __of_clk_get_from_provider(clkspec);

	if (!IS_ERR(clk) && !__clk_get(clk))
		clk = ERR_PTR(-ENOENT);

	of_clk_unlock();
	return clk;
}

struct clk *of_clk_get(struct device_node *np, int index)
=======
#if defined(CONFIG_OF) && defined(CONFIG_COMMON_CLK)
static struct clk *__of_clk_get(struct device_node *np, int index,
			       const char *dev_id, const char *con_id)
>>>>>>> v4.9.227
{
	struct of_phandle_args clkspec;
	struct clk *clk;
	int rc;

	if (index < 0)
		return ERR_PTR(-EINVAL);

	rc = of_parse_phandle_with_args(np, "clocks", "#clock-cells", index,
					&clkspec);
	if (rc)
		return ERR_PTR(rc);

<<<<<<< HEAD
	clk = of_clk_get_by_clkspec(&clkspec);
	of_node_put(clkspec.np);
	return clk;
}
EXPORT_SYMBOL(of_clk_get);

/**
 * of_clk_get_by_name() - Parse and lookup a clock referenced by a device node
 * @np: pointer to clock consumer node
 * @name: name of consumer's clock input, or NULL for the first clock reference
 *
 * This function parses the clocks and clock-names properties,
 * and uses them to look up the struct clk from the registered list of clock
 * providers.
 */
struct clk *of_clk_get_by_name(struct device_node *np, const char *name)
=======
	clk = __of_clk_get_from_provider(&clkspec, dev_id, con_id);
	of_node_put(clkspec.np);

	return clk;
}

struct clk *of_clk_get(struct device_node *np, int index)
{
	return __of_clk_get(np, index, np->full_name, NULL);
}
EXPORT_SYMBOL(of_clk_get);

static struct clk *__of_clk_get_by_name(struct device_node *np,
					const char *dev_id,
					const char *name)
>>>>>>> v4.9.227
{
	struct clk *clk = ERR_PTR(-ENOENT);

	/* Walk up the tree of devices looking for a clock that matches */
	while (np) {
		int index = 0;

		/*
		 * For named clocks, first look up the name in the
		 * "clock-names" property.  If it cannot be found, then
		 * index will be an error code, and of_clk_get() will fail.
		 */
		if (name)
			index = of_property_match_string(np, "clock-names", name);
<<<<<<< HEAD
		clk = of_clk_get(np, index);
		if (!IS_ERR(clk))
			break;
		else if (name && index >= 0)
			return clk;
=======
		clk = __of_clk_get(np, index, dev_id, name);
		if (!IS_ERR(clk)) {
			break;
		} else if (name && index >= 0) {
			if (PTR_ERR(clk) != -EPROBE_DEFER)
				pr_err("ERROR: could not get clock %s:%s(%i)\n",
					np->full_name, name ? name : "", index);
			return clk;
		}
>>>>>>> v4.9.227

		/*
		 * No matching clock found on this node.  If the parent node
		 * has a "clock-ranges" property, then we can try one of its
		 * clocks.
		 */
		np = np->parent;
		if (np && !of_get_property(np, "clock-ranges", NULL))
			break;
	}

	return clk;
}
<<<<<<< HEAD
EXPORT_SYMBOL(of_clk_get_by_name);
=======

/**
 * of_clk_get_by_name() - Parse and lookup a clock referenced by a device node
 * @np: pointer to clock consumer node
 * @name: name of consumer's clock input, or NULL for the first clock reference
 *
 * This function parses the clocks and clock-names properties,
 * and uses them to look up the struct clk from the registered list of clock
 * providers.
 */
struct clk *of_clk_get_by_name(struct device_node *np, const char *name)
{
	if (!np)
		return ERR_PTR(-ENOENT);

	return __of_clk_get_by_name(np, np->full_name, name);
}
EXPORT_SYMBOL(of_clk_get_by_name);

#else /* defined(CONFIG_OF) && defined(CONFIG_COMMON_CLK) */

static struct clk *__of_clk_get_by_name(struct device_node *np,
					const char *dev_id,
					const char *name)
{
	return ERR_PTR(-ENOENT);
}
>>>>>>> v4.9.227
#endif

/*
 * Find the correct struct clk for the device and connection ID.
 * We do slightly fuzzy matching here:
 *  An entry with a NULL ID is assumed to be a wildcard.
 *  If an entry has a device ID, it must match
 *  If an entry has a connection ID, it must match
 * Then we take the most specific entry - with the following
 * order of precedence: dev+con > dev only > con only.
 */
static struct clk_lookup *clk_find(const char *dev_id, const char *con_id)
{
	struct clk_lookup *p, *cl = NULL;
	int match, best_found = 0, best_possible = 0;

	if (dev_id)
		best_possible += 2;
	if (con_id)
		best_possible += 1;

	list_for_each_entry(p, &clocks, node) {
		match = 0;
		if (p->dev_id) {
			if (!dev_id || strcmp(p->dev_id, dev_id))
				continue;
			match += 2;
		}
		if (p->con_id) {
			if (!con_id || strcmp(p->con_id, con_id))
				continue;
			match += 1;
		}

		if (match > best_found) {
			cl = p;
			if (match != best_possible)
				best_found = match;
			else
				break;
		}
	}
	return cl;
}

struct clk *clk_get_sys(const char *dev_id, const char *con_id)
{
	struct clk_lookup *cl;
<<<<<<< HEAD

	mutex_lock(&clocks_mutex);
	cl = clk_find(dev_id, con_id);
	if (cl && !__clk_get(cl->clk))
		cl = NULL;
	mutex_unlock(&clocks_mutex);

	return cl ? cl->clk : ERR_PTR(-ENOENT);
=======
	struct clk *clk = NULL;

	mutex_lock(&clocks_mutex);

	cl = clk_find(dev_id, con_id);
	if (!cl)
		goto out;

	clk = __clk_create_clk(cl->clk_hw, dev_id, con_id);
	if (IS_ERR(clk))
		goto out;

	if (!__clk_get(clk)) {
		__clk_free_clk(clk);
		cl = NULL;
		goto out;
	}

out:
	mutex_unlock(&clocks_mutex);

	return cl ? clk : ERR_PTR(-ENOENT);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(clk_get_sys);

struct clk *clk_get(struct device *dev, const char *con_id)
{
	const char *dev_id = dev ? dev_name(dev) : NULL;
	struct clk *clk;

	if (dev) {
<<<<<<< HEAD
		clk = of_clk_get_by_name(dev->of_node, con_id);
		if (!IS_ERR(clk))
			return clk;
		if (PTR_ERR(clk) == -EPROBE_DEFER)
=======
		clk = __of_clk_get_by_name(dev->of_node, dev_id, con_id);
		if (!IS_ERR(clk) || PTR_ERR(clk) == -EPROBE_DEFER)
>>>>>>> v4.9.227
			return clk;
	}

	return clk_get_sys(dev_id, con_id);
}
EXPORT_SYMBOL(clk_get);

void clk_put(struct clk *clk)
{
	__clk_put(clk);
}
EXPORT_SYMBOL(clk_put);

<<<<<<< HEAD
void clkdev_add(struct clk_lookup *cl)
=======
static void __clkdev_add(struct clk_lookup *cl)
>>>>>>> v4.9.227
{
	mutex_lock(&clocks_mutex);
	list_add_tail(&cl->node, &clocks);
	mutex_unlock(&clocks_mutex);
}
<<<<<<< HEAD
=======

void clkdev_add(struct clk_lookup *cl)
{
	if (!cl->clk_hw)
		cl->clk_hw = __clk_get_hw(cl->clk);
	__clkdev_add(cl);
}
>>>>>>> v4.9.227
EXPORT_SYMBOL(clkdev_add);

void clkdev_add_table(struct clk_lookup *cl, size_t num)
{
	mutex_lock(&clocks_mutex);
	while (num--) {
<<<<<<< HEAD
=======
		cl->clk_hw = __clk_get_hw(cl->clk);
>>>>>>> v4.9.227
		list_add_tail(&cl->node, &clocks);
		cl++;
	}
	mutex_unlock(&clocks_mutex);
}

#define MAX_DEV_ID	20
#define MAX_CON_ID	16

struct clk_lookup_alloc {
	struct clk_lookup cl;
	char	dev_id[MAX_DEV_ID];
	char	con_id[MAX_CON_ID];
};

<<<<<<< HEAD
static struct clk_lookup * __init_refok
vclkdev_alloc(struct clk *clk, const char *con_id, const char *dev_fmt,
=======
static struct clk_lookup * __ref
vclkdev_alloc(struct clk_hw *hw, const char *con_id, const char *dev_fmt,
>>>>>>> v4.9.227
	va_list ap)
{
	struct clk_lookup_alloc *cla;

	cla = __clkdev_alloc(sizeof(*cla));
	if (!cla)
		return NULL;

<<<<<<< HEAD
	cla->cl.clk = clk;
=======
	cla->cl.clk_hw = hw;
>>>>>>> v4.9.227
	if (con_id) {
		strlcpy(cla->con_id, con_id, sizeof(cla->con_id));
		cla->cl.con_id = cla->con_id;
	}

	if (dev_fmt) {
		vscnprintf(cla->dev_id, sizeof(cla->dev_id), dev_fmt, ap);
		cla->cl.dev_id = cla->dev_id;
	}

	return &cla->cl;
}

<<<<<<< HEAD
struct clk_lookup * __init_refok
=======
static struct clk_lookup *
vclkdev_create(struct clk_hw *hw, const char *con_id, const char *dev_fmt,
	va_list ap)
{
	struct clk_lookup *cl;

	cl = vclkdev_alloc(hw, con_id, dev_fmt, ap);
	if (cl)
		__clkdev_add(cl);

	return cl;
}

struct clk_lookup * __ref
>>>>>>> v4.9.227
clkdev_alloc(struct clk *clk, const char *con_id, const char *dev_fmt, ...)
{
	struct clk_lookup *cl;
	va_list ap;

	va_start(ap, dev_fmt);
<<<<<<< HEAD
	cl = vclkdev_alloc(clk, con_id, dev_fmt, ap);
=======
	cl = vclkdev_alloc(__clk_get_hw(clk), con_id, dev_fmt, ap);
>>>>>>> v4.9.227
	va_end(ap);

	return cl;
}
EXPORT_SYMBOL(clkdev_alloc);

<<<<<<< HEAD
int clk_add_alias(const char *alias, const char *alias_dev_name, char *id,
	struct device *dev)
{
	struct clk *r = clk_get(dev, id);
=======
struct clk_lookup *
clkdev_hw_alloc(struct clk_hw *hw, const char *con_id, const char *dev_fmt, ...)
{
	struct clk_lookup *cl;
	va_list ap;

	va_start(ap, dev_fmt);
	cl = vclkdev_alloc(hw, con_id, dev_fmt, ap);
	va_end(ap);

	return cl;
}
EXPORT_SYMBOL(clkdev_hw_alloc);

/**
 * clkdev_create - allocate and add a clkdev lookup structure
 * @clk: struct clk to associate with all clk_lookups
 * @con_id: connection ID string on device
 * @dev_fmt: format string describing device name
 *
 * Returns a clk_lookup structure, which can be later unregistered and
 * freed.
 */
struct clk_lookup *clkdev_create(struct clk *clk, const char *con_id,
	const char *dev_fmt, ...)
{
	struct clk_lookup *cl;
	va_list ap;

	va_start(ap, dev_fmt);
	cl = vclkdev_create(__clk_get_hw(clk), con_id, dev_fmt, ap);
	va_end(ap);

	return cl;
}
EXPORT_SYMBOL_GPL(clkdev_create);

/**
 * clkdev_hw_create - allocate and add a clkdev lookup structure
 * @hw: struct clk_hw to associate with all clk_lookups
 * @con_id: connection ID string on device
 * @dev_fmt: format string describing device name
 *
 * Returns a clk_lookup structure, which can be later unregistered and
 * freed.
 */
struct clk_lookup *clkdev_hw_create(struct clk_hw *hw, const char *con_id,
	const char *dev_fmt, ...)
{
	struct clk_lookup *cl;
	va_list ap;

	va_start(ap, dev_fmt);
	cl = vclkdev_create(hw, con_id, dev_fmt, ap);
	va_end(ap);

	return cl;
}
EXPORT_SYMBOL_GPL(clkdev_hw_create);

int clk_add_alias(const char *alias, const char *alias_dev_name,
	const char *con_id, struct device *dev)
{
	struct clk *r = clk_get(dev, con_id);
>>>>>>> v4.9.227
	struct clk_lookup *l;

	if (IS_ERR(r))
		return PTR_ERR(r);

<<<<<<< HEAD
	l = clkdev_alloc(r, alias, alias_dev_name);
	clk_put(r);
	if (!l)
		return -ENODEV;
	clkdev_add(l);
	return 0;
=======
	l = clkdev_create(r, alias, alias_dev_name ? "%s" : NULL,
			  alias_dev_name);
	clk_put(r);

	return l ? 0 : -ENODEV;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(clk_add_alias);

/*
 * clkdev_drop - remove a clock dynamically allocated
 */
void clkdev_drop(struct clk_lookup *cl)
{
	mutex_lock(&clocks_mutex);
	list_del(&cl->node);
	mutex_unlock(&clocks_mutex);
	kfree(cl);
}
EXPORT_SYMBOL(clkdev_drop);

<<<<<<< HEAD
=======
static struct clk_lookup *__clk_register_clkdev(struct clk_hw *hw,
						const char *con_id,
						const char *dev_id, ...)
{
	struct clk_lookup *cl;
	va_list ap;

	va_start(ap, dev_id);
	cl = vclkdev_create(hw, con_id, dev_id, ap);
	va_end(ap);

	return cl;
}

>>>>>>> v4.9.227
/**
 * clk_register_clkdev - register one clock lookup for a struct clk
 * @clk: struct clk to associate with all clk_lookups
 * @con_id: connection ID string on device
<<<<<<< HEAD
 * @dev_id: format string describing device name
=======
 * @dev_id: string describing device name
>>>>>>> v4.9.227
 *
 * con_id or dev_id may be NULL as a wildcard, just as in the rest of
 * clkdev.
 *
 * To make things easier for mass registration, we detect error clks
 * from a previous clk_register() call, and return the error code for
 * those.  This is to permit this function to be called immediately
 * after clk_register().
 */
int clk_register_clkdev(struct clk *clk, const char *con_id,
<<<<<<< HEAD
	const char *dev_fmt, ...)
{
	struct clk_lookup *cl;
	va_list ap;
=======
	const char *dev_id)
{
	struct clk_lookup *cl;
>>>>>>> v4.9.227

	if (IS_ERR(clk))
		return PTR_ERR(clk);

<<<<<<< HEAD
	va_start(ap, dev_fmt);
	cl = vclkdev_alloc(clk, con_id, dev_fmt, ap);
	va_end(ap);

	if (!cl)
		return -ENOMEM;

	clkdev_add(cl);

	return 0;
}

/**
 * clk_register_clkdevs - register a set of clk_lookup for a struct clk
 * @clk: struct clk to associate with all clk_lookups
 * @cl: array of clk_lookup structures with con_id and dev_id pre-initialized
 * @num: number of clk_lookup structures to register
 *
 * To make things easier for mass registration, we detect error clks
 * from a previous clk_register() call, and return the error code for
 * those.  This is to permit this function to be called immediately
 * after clk_register().
 */
int clk_register_clkdevs(struct clk *clk, struct clk_lookup *cl, size_t num)
{
	unsigned i;

	if (IS_ERR(clk))
		return PTR_ERR(clk);

	for (i = 0; i < num; i++, cl++) {
		cl->clk = clk;
		clkdev_add(cl);
	}

	return 0;
}
EXPORT_SYMBOL(clk_register_clkdevs);
=======
	/*
	 * Since dev_id can be NULL, and NULL is handled specially, we must
	 * pass it as either a NULL format string, or with "%s".
	 */
	if (dev_id)
		cl = __clk_register_clkdev(__clk_get_hw(clk), con_id, "%s",
					   dev_id);
	else
		cl = __clk_register_clkdev(__clk_get_hw(clk), con_id, NULL);

	return cl ? 0 : -ENOMEM;
}
EXPORT_SYMBOL(clk_register_clkdev);

/**
 * clk_hw_register_clkdev - register one clock lookup for a struct clk_hw
 * @hw: struct clk_hw to associate with all clk_lookups
 * @con_id: connection ID string on device
 * @dev_id: format string describing device name
 *
 * con_id or dev_id may be NULL as a wildcard, just as in the rest of
 * clkdev.
 *
 * To make things easier for mass registration, we detect error clk_hws
 * from a previous clk_hw_register_*() call, and return the error code for
 * those.  This is to permit this function to be called immediately
 * after clk_hw_register_*().
 */
int clk_hw_register_clkdev(struct clk_hw *hw, const char *con_id,
	const char *dev_id)
{
	struct clk_lookup *cl;

	if (IS_ERR(hw))
		return PTR_ERR(hw);

	/*
	 * Since dev_id can be NULL, and NULL is handled specially, we must
	 * pass it as either a NULL format string, or with "%s".
	 */
	if (dev_id)
		cl = __clk_register_clkdev(hw, con_id, "%s", dev_id);
	else
		cl = __clk_register_clkdev(hw, con_id, NULL);

	return cl ? 0 : -ENOMEM;
}
EXPORT_SYMBOL(clk_hw_register_clkdev);
>>>>>>> v4.9.227
