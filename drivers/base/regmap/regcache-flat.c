/*
 * Register cache access API - flat caching support
 *
 * Copyright 2012 Wolfson Microelectronics plc
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

<<<<<<< HEAD
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/seq_file.h>

#include "internal.h"

=======
#include <linux/device.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

#include "internal.h"

static inline unsigned int regcache_flat_get_index(const struct regmap *map,
						   unsigned int reg)
{
	return regcache_get_index_by_order(map, reg);
}

>>>>>>> v4.9.227
static int regcache_flat_init(struct regmap *map)
{
	int i;
	unsigned int *cache;

<<<<<<< HEAD
	map->cache = kzalloc(sizeof(unsigned int) * (map->max_register + 1),
			     GFP_KERNEL);
=======
	if (!map || map->reg_stride_order < 0 || !map->max_register)
		return -EINVAL;

	map->cache = kcalloc(regcache_flat_get_index(map, map->max_register)
			     + 1, sizeof(unsigned int), GFP_KERNEL);
>>>>>>> v4.9.227
	if (!map->cache)
		return -ENOMEM;

	cache = map->cache;

	for (i = 0; i < map->num_reg_defaults; i++)
<<<<<<< HEAD
		cache[map->reg_defaults[i].reg] = map->reg_defaults[i].def;
=======
		cache[regcache_flat_get_index(map, map->reg_defaults[i].reg)] =
				map->reg_defaults[i].def;
>>>>>>> v4.9.227

	return 0;
}

static int regcache_flat_exit(struct regmap *map)
{
	kfree(map->cache);
	map->cache = NULL;

	return 0;
}

static int regcache_flat_read(struct regmap *map,
			      unsigned int reg, unsigned int *value)
{
	unsigned int *cache = map->cache;

<<<<<<< HEAD
	*value = cache[reg];
=======
	*value = cache[regcache_flat_get_index(map, reg)];
>>>>>>> v4.9.227

	return 0;
}

static int regcache_flat_write(struct regmap *map, unsigned int reg,
			       unsigned int value)
{
	unsigned int *cache = map->cache;

<<<<<<< HEAD
	cache[reg] = value;
=======
	cache[regcache_flat_get_index(map, reg)] = value;
>>>>>>> v4.9.227

	return 0;
}

struct regcache_ops regcache_flat_ops = {
	.type = REGCACHE_FLAT,
	.name = "flat",
	.init = regcache_flat_init,
	.exit = regcache_flat_exit,
	.read = regcache_flat_read,
	.write = regcache_flat_write,
};
