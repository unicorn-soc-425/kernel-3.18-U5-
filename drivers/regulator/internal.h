/*
 * internal.h  --  Voltage/Current Regulator framework internal code
 *
 * Copyright 2007, 2008 Wolfson Microelectronics PLC.
 * Copyright 2008 SlimLogic Ltd.
 *
 * Author: Liam Girdwood <lrg@slimlogic.co.uk>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#ifndef __REGULATOR_INTERNAL_H
#define __REGULATOR_INTERNAL_H

/*
 * struct regulator
 *
 * One for each consumer device.
 */
struct regulator {
	struct device *dev;
	struct list_head list;
	unsigned int always_on:1;
	unsigned int bypass:1;
	int uA_load;
	int min_uV;
	int max_uV;
<<<<<<< HEAD
	int enabled;
=======
>>>>>>> v4.9.227
	char *supply_name;
	struct device_attribute dev_attr;
	struct regulator_dev *rdev;
	struct dentry *debugfs;
};

#ifdef CONFIG_OF
struct regulator_init_data *regulator_of_get_init_data(struct device *dev,
			         const struct regulator_desc *desc,
<<<<<<< HEAD
=======
				 struct regulator_config *config,
>>>>>>> v4.9.227
				 struct device_node **node);
#else
static inline struct regulator_init_data *
regulator_of_get_init_data(struct device *dev,
			   const struct regulator_desc *desc,
<<<<<<< HEAD
=======
			   struct regulator_config *config,
>>>>>>> v4.9.227
			   struct device_node **node)
{
	return NULL;
}
#endif

#endif
