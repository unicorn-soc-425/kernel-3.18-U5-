/*
 * lm75.c - Part of lm_sensors, Linux kernel modules for hardware
 *	 monitoring
 * Copyright (c) 1998, 1999  Frodo Looijaard <frodol@dds.nl>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
<<<<<<< HEAD
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/thermal.h>
=======
#include <linux/of.h>
#include <linux/regmap.h>
>>>>>>> v4.9.227
#include "lm75.h"


/*
 * This driver handles the LM75 and compatible digital temperature sensors.
 */

enum lm75_type {		/* keep sorted in alphabetical order */
	adt75,
	ds1775,
	ds75,
	ds7505,
	g751,
	lm75,
	lm75a,
<<<<<<< HEAD
=======
	lm75b,
>>>>>>> v4.9.227
	max6625,
	max6626,
	mcp980x,
	stds75,
	tcn75,
	tmp100,
	tmp101,
	tmp105,
	tmp112,
	tmp175,
	tmp275,
	tmp75,
<<<<<<< HEAD
=======
	tmp75c,
>>>>>>> v4.9.227
};

/* Addresses scanned */
static const unsigned short normal_i2c[] = { 0x48, 0x49, 0x4a, 0x4b, 0x4c,
					0x4d, 0x4e, 0x4f, I2C_CLIENT_END };


/* The LM75 registers */
<<<<<<< HEAD
#define LM75_REG_CONF		0x01
static const u8 LM75_REG_TEMP[3] = {
	0x00,		/* input */
	0x03,		/* max */
	0x02,		/* hyst */
};
=======
#define LM75_REG_TEMP		0x00
#define LM75_REG_CONF		0x01
#define LM75_REG_HYST		0x02
#define LM75_REG_MAX		0x03
>>>>>>> v4.9.227

/* Each client has this additional data */
struct lm75_data {
	struct i2c_client	*client;
<<<<<<< HEAD
	struct device		*hwmon_dev;
	struct thermal_zone_device	*tz;
	struct mutex		update_lock;
	u8			orig_conf;
	u8			resolution;	/* In bits, between 9 and 12 */
	u8			resolution_limits;
	char			valid;		/* !=0 if registers are valid */
	unsigned long		last_updated;	/* In jiffies */
	unsigned long		sample_time;	/* In jiffies */
	s16			temp[3];	/* Register values,
						   0 = input
						   1 = max
						   2 = hyst */
};

static int lm75_read_value(struct i2c_client *client, u8 reg);
static int lm75_write_value(struct i2c_client *client, u8 reg, u16 value);
static struct lm75_data *lm75_update_device(struct device *dev);


=======
	struct regmap		*regmap;
	u8			orig_conf;
	u8			resolution;	/* In bits, between 9 and 12 */
	u8			resolution_limits;
	unsigned int		sample_time;	/* In ms */
};

>>>>>>> v4.9.227
/*-----------------------------------------------------------------------*/

static inline long lm75_reg_to_mc(s16 temp, u8 resolution)
{
	return ((temp >> (16 - resolution)) * 1000) >> (resolution - 8);
}

<<<<<<< HEAD
/* sysfs attributes for hwmon */

static int lm75_read_temp(void *dev, long *temp)
{
	struct lm75_data *data = lm75_update_device(dev);

	if (IS_ERR(data))
		return PTR_ERR(data);

	*temp = lm75_reg_to_mc(data->temp[0], data->resolution);

	return 0;
}

static ssize_t show_temp(struct device *dev, struct device_attribute *da,
			 char *buf)
{
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	struct lm75_data *data = lm75_update_device(dev);

	if (IS_ERR(data))
		return PTR_ERR(data);

	return sprintf(buf, "%ld\n", lm75_reg_to_mc(data->temp[attr->index],
						    data->resolution));
}

static ssize_t set_temp(struct device *dev, struct device_attribute *da,
			const char *buf, size_t count)
{
	struct sensor_device_attribute *attr = to_sensor_dev_attr(da);
	struct lm75_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	int nr = attr->index;
	long temp;
	int error;
	u8 resolution;

	error = kstrtol(buf, 10, &temp);
	if (error)
		return error;
=======
static int lm75_read(struct device *dev, enum hwmon_sensor_types type,
		     u32 attr, int channel, long *val)
{
	struct lm75_data *data = dev_get_drvdata(dev);
	unsigned int regval;
	int err, reg;

	switch (type) {
	case hwmon_chip:
		switch (attr) {
		case hwmon_chip_update_interval:
			*val = data->sample_time;
			break;;
		default:
			return -EINVAL;
		}
		break;
	case hwmon_temp:
		switch (attr) {
		case hwmon_temp_input:
			reg = LM75_REG_TEMP;
			break;
		case hwmon_temp_max:
			reg = LM75_REG_MAX;
			break;
		case hwmon_temp_max_hyst:
			reg = LM75_REG_HYST;
			break;
		default:
			return -EINVAL;
		}
		err = regmap_read(data->regmap, reg, &regval);
		if (err < 0)
			return err;

		*val = lm75_reg_to_mc(regval, data->resolution);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int lm75_write(struct device *dev, enum hwmon_sensor_types type,
		      u32 attr, int channel, long temp)
{
	struct lm75_data *data = dev_get_drvdata(dev);
	u8 resolution;
	int reg;

	if (type != hwmon_temp)
		return -EINVAL;

	switch (attr) {
	case hwmon_temp_max:
		reg = LM75_REG_MAX;
		break;
	case hwmon_temp_max_hyst:
		reg = LM75_REG_HYST;
		break;
	default:
		return -EINVAL;
	}
>>>>>>> v4.9.227

	/*
	 * Resolution of limit registers is assumed to be the same as the
	 * temperature input register resolution unless given explicitly.
	 */
<<<<<<< HEAD
	if (attr->index && data->resolution_limits)
=======
	if (data->resolution_limits)
>>>>>>> v4.9.227
		resolution = data->resolution_limits;
	else
		resolution = data->resolution;

<<<<<<< HEAD
	mutex_lock(&data->update_lock);
	temp = clamp_val(temp, LM75_TEMP_MIN, LM75_TEMP_MAX);
	data->temp[nr] = DIV_ROUND_CLOSEST(temp  << (resolution - 8),
					   1000) << (16 - resolution);
	lm75_write_value(client, LM75_REG_TEMP[nr], data->temp[nr]);
	mutex_unlock(&data->update_lock);
	return count;
}

static SENSOR_DEVICE_ATTR(temp1_max, S_IWUSR | S_IRUGO,
			show_temp, set_temp, 1);
static SENSOR_DEVICE_ATTR(temp1_max_hyst, S_IWUSR | S_IRUGO,
			show_temp, set_temp, 2);
static SENSOR_DEVICE_ATTR(temp1_input, S_IRUGO, show_temp, NULL, 0);

static struct attribute *lm75_attrs[] = {
	&sensor_dev_attr_temp1_input.dev_attr.attr,
	&sensor_dev_attr_temp1_max.dev_attr.attr,
	&sensor_dev_attr_temp1_max_hyst.dev_attr.attr,

	NULL
};
ATTRIBUTE_GROUPS(lm75);

static const struct thermal_zone_of_device_ops lm75_of_thermal_ops = {
	.get_temp = lm75_read_temp,
};
=======
	temp = clamp_val(temp, LM75_TEMP_MIN, LM75_TEMP_MAX);
	temp = DIV_ROUND_CLOSEST(temp  << (resolution - 8),
				 1000) << (16 - resolution);

	return regmap_write(data->regmap, reg, (u16)temp);
}

static umode_t lm75_is_visible(const void *data, enum hwmon_sensor_types type,
			       u32 attr, int channel)
{
	switch (type) {
	case hwmon_chip:
		switch (attr) {
		case hwmon_chip_update_interval:
			return S_IRUGO;
		}
		break;
	case hwmon_temp:
		switch (attr) {
		case hwmon_temp_input:
			return S_IRUGO;
		case hwmon_temp_max:
		case hwmon_temp_max_hyst:
			return S_IRUGO | S_IWUSR;
		}
		break;
	default:
		break;
	}
	return 0;
}
>>>>>>> v4.9.227

/*-----------------------------------------------------------------------*/

/* device probe and removal */

<<<<<<< HEAD
=======
/* chip configuration */

static const u32 lm75_chip_config[] = {
	HWMON_C_REGISTER_TZ | HWMON_C_UPDATE_INTERVAL,
	0
};

static const struct hwmon_channel_info lm75_chip = {
	.type = hwmon_chip,
	.config = lm75_chip_config,
};

static const u32 lm75_temp_config[] = {
	HWMON_T_INPUT | HWMON_T_MAX | HWMON_T_MAX_HYST,
	0
};

static const struct hwmon_channel_info lm75_temp = {
	.type = hwmon_temp,
	.config = lm75_temp_config,
};

static const struct hwmon_channel_info *lm75_info[] = {
	&lm75_chip,
	&lm75_temp,
	NULL
};

static const struct hwmon_ops lm75_hwmon_ops = {
	.is_visible = lm75_is_visible,
	.read = lm75_read,
	.write = lm75_write,
};

static const struct hwmon_chip_info lm75_chip_info = {
	.ops = &lm75_hwmon_ops,
	.info = lm75_info,
};

static bool lm75_is_writeable_reg(struct device *dev, unsigned int reg)
{
	return reg != LM75_REG_TEMP;
}

static bool lm75_is_volatile_reg(struct device *dev, unsigned int reg)
{
	return reg == LM75_REG_TEMP;
}

static const struct regmap_config lm75_regmap_config = {
	.reg_bits = 8,
	.val_bits = 16,
	.max_register = LM75_REG_MAX,
	.writeable_reg = lm75_is_writeable_reg,
	.volatile_reg = lm75_is_volatile_reg,
	.val_format_endian = REGMAP_ENDIAN_BIG,
	.cache_type = REGCACHE_RBTREE,
	.use_single_rw = true,
};

static void lm75_remove(void *data)
{
	struct lm75_data *lm75 = data;
	struct i2c_client *client = lm75->client;

	i2c_smbus_write_byte_data(client, LM75_REG_CONF, lm75->orig_conf);
}

>>>>>>> v4.9.227
static int
lm75_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
<<<<<<< HEAD
	struct lm75_data *data;
	int status;
=======
	struct device *hwmon_dev;
	struct lm75_data *data;
	int status, err;
>>>>>>> v4.9.227
	u8 set_mask, clr_mask;
	int new;
	enum lm75_type kind = id->driver_data;

	if (!i2c_check_functionality(client->adapter,
			I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA))
		return -EIO;

	data = devm_kzalloc(dev, sizeof(struct lm75_data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	data->client = client;
<<<<<<< HEAD
	i2c_set_clientdata(client, data);
	mutex_init(&data->update_lock);
=======

	data->regmap = devm_regmap_init_i2c(client, &lm75_regmap_config);
	if (IS_ERR(data->regmap))
		return PTR_ERR(data->regmap);
>>>>>>> v4.9.227

	/* Set to LM75 resolution (9 bits, 1/2 degree C) and range.
	 * Then tweak to be more precise when appropriate.
	 */
	set_mask = 0;
	clr_mask = LM75_SHUTDOWN;		/* continuous conversions */

	switch (kind) {
	case adt75:
		clr_mask |= 1 << 5;		/* not one-shot mode */
		data->resolution = 12;
<<<<<<< HEAD
		data->sample_time = HZ / 8;
=======
		data->sample_time = MSEC_PER_SEC / 8;
>>>>>>> v4.9.227
		break;
	case ds1775:
	case ds75:
	case stds75:
		clr_mask |= 3 << 5;
		set_mask |= 2 << 5;		/* 11-bit mode */
		data->resolution = 11;
<<<<<<< HEAD
		data->sample_time = HZ;
=======
		data->sample_time = MSEC_PER_SEC;
>>>>>>> v4.9.227
		break;
	case ds7505:
		set_mask |= 3 << 5;		/* 12-bit mode */
		data->resolution = 12;
<<<<<<< HEAD
		data->sample_time = HZ / 4;
=======
		data->sample_time = MSEC_PER_SEC / 4;
>>>>>>> v4.9.227
		break;
	case g751:
	case lm75:
	case lm75a:
		data->resolution = 9;
<<<<<<< HEAD
		data->sample_time = HZ / 2;
		break;
	case max6625:
		data->resolution = 9;
		data->sample_time = HZ / 4;
=======
		data->sample_time = MSEC_PER_SEC / 2;
		break;
	case lm75b:
		data->resolution = 11;
		data->sample_time = MSEC_PER_SEC / 4;
		break;
	case max6625:
		data->resolution = 9;
		data->sample_time = MSEC_PER_SEC / 4;
>>>>>>> v4.9.227
		break;
	case max6626:
		data->resolution = 12;
		data->resolution_limits = 9;
<<<<<<< HEAD
		data->sample_time = HZ / 4;
		break;
	case tcn75:
		data->resolution = 9;
		data->sample_time = HZ / 8;
=======
		data->sample_time = MSEC_PER_SEC / 4;
		break;
	case tcn75:
		data->resolution = 9;
		data->sample_time = MSEC_PER_SEC / 8;
>>>>>>> v4.9.227
		break;
	case mcp980x:
		data->resolution_limits = 9;
		/* fall through */
	case tmp100:
	case tmp101:
		set_mask |= 3 << 5;		/* 12-bit mode */
		data->resolution = 12;
<<<<<<< HEAD
		data->sample_time = HZ;
=======
		data->sample_time = MSEC_PER_SEC;
>>>>>>> v4.9.227
		clr_mask |= 1 << 7;		/* not one-shot mode */
		break;
	case tmp112:
		set_mask |= 3 << 5;		/* 12-bit mode */
		clr_mask |= 1 << 7;		/* not one-shot mode */
		data->resolution = 12;
<<<<<<< HEAD
		data->sample_time = HZ / 4;
=======
		data->sample_time = MSEC_PER_SEC / 4;
>>>>>>> v4.9.227
		break;
	case tmp105:
	case tmp175:
	case tmp275:
	case tmp75:
		set_mask |= 3 << 5;		/* 12-bit mode */
		clr_mask |= 1 << 7;		/* not one-shot mode */
		data->resolution = 12;
<<<<<<< HEAD
		data->sample_time = HZ / 2;
=======
		data->sample_time = MSEC_PER_SEC / 2;
		break;
	case tmp75c:
		clr_mask |= 1 << 5;		/* not one-shot mode */
		data->resolution = 12;
		data->sample_time = MSEC_PER_SEC / 4;
>>>>>>> v4.9.227
		break;
	}

	/* configure as specified */
<<<<<<< HEAD
	status = lm75_read_value(client, LM75_REG_CONF);
=======
	status = i2c_smbus_read_byte_data(client, LM75_REG_CONF);
>>>>>>> v4.9.227
	if (status < 0) {
		dev_dbg(dev, "Can't read config? %d\n", status);
		return status;
	}
	data->orig_conf = status;
	new = status & ~clr_mask;
	new |= set_mask;
	if (status != new)
<<<<<<< HEAD
		lm75_write_value(client, LM75_REG_CONF, new);
	dev_dbg(dev, "Config %02x\n", new);

	data->hwmon_dev = hwmon_device_register_with_groups(dev, client->name,
							    data, lm75_groups);
	if (IS_ERR(data->hwmon_dev))
		return PTR_ERR(data->hwmon_dev);

	data->tz = thermal_zone_of_sensor_register(data->hwmon_dev, 0,
						   data->hwmon_dev,
						   &lm75_of_thermal_ops);
	if (IS_ERR(data->tz))
		data->tz = NULL;

	dev_info(dev, "%s: sensor '%s'\n",
		 dev_name(data->hwmon_dev), client->name);

	return 0;
}

static int lm75_remove(struct i2c_client *client)
{
	struct lm75_data *data = i2c_get_clientdata(client);

	thermal_zone_of_sensor_unregister(data->hwmon_dev, data->tz);
	hwmon_device_unregister(data->hwmon_dev);
	lm75_write_value(client, LM75_REG_CONF, data->orig_conf);
=======
		i2c_smbus_write_byte_data(client, LM75_REG_CONF, new);

	err = devm_add_action_or_reset(dev, lm75_remove, data);
	if (err)
		return err;

	dev_dbg(dev, "Config %02x\n", new);

	hwmon_dev = devm_hwmon_device_register_with_info(dev, client->name,
							 data, &lm75_chip_info,
							 NULL);
	if (IS_ERR(hwmon_dev))
		return PTR_ERR(hwmon_dev);

	dev_info(dev, "%s: sensor '%s'\n", dev_name(hwmon_dev), client->name);

>>>>>>> v4.9.227
	return 0;
}

static const struct i2c_device_id lm75_ids[] = {
	{ "adt75", adt75, },
	{ "ds1775", ds1775, },
	{ "ds75", ds75, },
	{ "ds7505", ds7505, },
	{ "g751", g751, },
	{ "lm75", lm75, },
	{ "lm75a", lm75a, },
<<<<<<< HEAD
=======
	{ "lm75b", lm75b, },
>>>>>>> v4.9.227
	{ "max6625", max6625, },
	{ "max6626", max6626, },
	{ "mcp980x", mcp980x, },
	{ "stds75", stds75, },
	{ "tcn75", tcn75, },
	{ "tmp100", tmp100, },
	{ "tmp101", tmp101, },
	{ "tmp105", tmp105, },
	{ "tmp112", tmp112, },
	{ "tmp175", tmp175, },
	{ "tmp275", tmp275, },
	{ "tmp75", tmp75, },
<<<<<<< HEAD
=======
	{ "tmp75c", tmp75c, },
>>>>>>> v4.9.227
	{ /* LIST END */ }
};
MODULE_DEVICE_TABLE(i2c, lm75_ids);

#define LM75A_ID 0xA1

/* Return 0 if detection is successful, -ENODEV otherwise */
static int lm75_detect(struct i2c_client *new_client,
		       struct i2c_board_info *info)
{
	struct i2c_adapter *adapter = new_client->adapter;
	int i;
	int conf, hyst, os;
	bool is_lm75a = 0;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA |
				     I2C_FUNC_SMBUS_WORD_DATA))
		return -ENODEV;

	/*
	 * Now, we do the remaining detection. There is no identification-
	 * dedicated register so we have to rely on several tricks:
	 * unused bits, registers cycling over 8-address boundaries,
	 * addresses 0x04-0x07 returning the last read value.
	 * The cycling+unused addresses combination is not tested,
	 * since it would significantly slow the detection down and would
	 * hardly add any value.
	 *
	 * The National Semiconductor LM75A is different than earlier
	 * LM75s.  It has an ID byte of 0xaX (where X is the chip
	 * revision, with 1 being the only revision in existence) in
	 * register 7, and unused registers return 0xff rather than the
	 * last read value.
	 *
	 * Note that this function only detects the original National
	 * Semiconductor LM75 and the LM75A. Clones from other vendors
	 * aren't detected, on purpose, because they are typically never
	 * found on PC hardware. They are found on embedded designs where
	 * they can be instantiated explicitly so detection is not needed.
	 * The absence of identification registers on all these clones
	 * would make their exhaustive detection very difficult and weak,
	 * and odds are that the driver would bind to unsupported devices.
	 */

	/* Unused bits */
	conf = i2c_smbus_read_byte_data(new_client, 1);
	if (conf & 0xe0)
		return -ENODEV;

	/* First check for LM75A */
	if (i2c_smbus_read_byte_data(new_client, 7) == LM75A_ID) {
		/* LM75A returns 0xff on unused registers so
		   just to be sure we check for that too. */
		if (i2c_smbus_read_byte_data(new_client, 4) != 0xff
		 || i2c_smbus_read_byte_data(new_client, 5) != 0xff
		 || i2c_smbus_read_byte_data(new_client, 6) != 0xff)
			return -ENODEV;
		is_lm75a = 1;
		hyst = i2c_smbus_read_byte_data(new_client, 2);
		os = i2c_smbus_read_byte_data(new_client, 3);
	} else { /* Traditional style LM75 detection */
		/* Unused addresses */
		hyst = i2c_smbus_read_byte_data(new_client, 2);
		if (i2c_smbus_read_byte_data(new_client, 4) != hyst
		 || i2c_smbus_read_byte_data(new_client, 5) != hyst
		 || i2c_smbus_read_byte_data(new_client, 6) != hyst
		 || i2c_smbus_read_byte_data(new_client, 7) != hyst)
			return -ENODEV;
		os = i2c_smbus_read_byte_data(new_client, 3);
		if (i2c_smbus_read_byte_data(new_client, 4) != os
		 || i2c_smbus_read_byte_data(new_client, 5) != os
		 || i2c_smbus_read_byte_data(new_client, 6) != os
		 || i2c_smbus_read_byte_data(new_client, 7) != os)
			return -ENODEV;
	}
<<<<<<< HEAD
=======
	/*
	 * It is very unlikely that this is a LM75 if both
	 * hysteresis and temperature limit registers are 0.
	 */
	if (hyst == 0 && os == 0)
		return -ENODEV;
>>>>>>> v4.9.227

	/* Addresses cycling */
	for (i = 8; i <= 248; i += 40) {
		if (i2c_smbus_read_byte_data(new_client, i + 1) != conf
		 || i2c_smbus_read_byte_data(new_client, i + 2) != hyst
		 || i2c_smbus_read_byte_data(new_client, i + 3) != os)
			return -ENODEV;
		if (is_lm75a && i2c_smbus_read_byte_data(new_client, i + 7)
				!= LM75A_ID)
			return -ENODEV;
	}

	strlcpy(info->type, is_lm75a ? "lm75a" : "lm75", I2C_NAME_SIZE);

	return 0;
}

#ifdef CONFIG_PM
static int lm75_suspend(struct device *dev)
{
	int status;
	struct i2c_client *client = to_i2c_client(dev);
<<<<<<< HEAD
	status = lm75_read_value(client, LM75_REG_CONF);
=======
	status = i2c_smbus_read_byte_data(client, LM75_REG_CONF);
>>>>>>> v4.9.227
	if (status < 0) {
		dev_dbg(&client->dev, "Can't read config? %d\n", status);
		return status;
	}
	status = status | LM75_SHUTDOWN;
<<<<<<< HEAD
	lm75_write_value(client, LM75_REG_CONF, status);
=======
	i2c_smbus_write_byte_data(client, LM75_REG_CONF, status);
>>>>>>> v4.9.227
	return 0;
}

static int lm75_resume(struct device *dev)
{
	int status;
	struct i2c_client *client = to_i2c_client(dev);
<<<<<<< HEAD
	status = lm75_read_value(client, LM75_REG_CONF);
=======
	status = i2c_smbus_read_byte_data(client, LM75_REG_CONF);
>>>>>>> v4.9.227
	if (status < 0) {
		dev_dbg(&client->dev, "Can't read config? %d\n", status);
		return status;
	}
	status = status & ~LM75_SHUTDOWN;
<<<<<<< HEAD
	lm75_write_value(client, LM75_REG_CONF, status);
=======
	i2c_smbus_write_byte_data(client, LM75_REG_CONF, status);
>>>>>>> v4.9.227
	return 0;
}

static const struct dev_pm_ops lm75_dev_pm_ops = {
	.suspend	= lm75_suspend,
	.resume		= lm75_resume,
};
#define LM75_DEV_PM_OPS (&lm75_dev_pm_ops)
#else
#define LM75_DEV_PM_OPS NULL
#endif /* CONFIG_PM */

static struct i2c_driver lm75_driver = {
	.class		= I2C_CLASS_HWMON,
	.driver = {
		.name	= "lm75",
		.pm	= LM75_DEV_PM_OPS,
	},
	.probe		= lm75_probe,
<<<<<<< HEAD
	.remove		= lm75_remove,
=======
>>>>>>> v4.9.227
	.id_table	= lm75_ids,
	.detect		= lm75_detect,
	.address_list	= normal_i2c,
};

<<<<<<< HEAD
/*-----------------------------------------------------------------------*/

/* register access */

/*
 * All registers are word-sized, except for the configuration register.
 * LM75 uses a high-byte first convention, which is exactly opposite to
 * the SMBus standard.
 */
static int lm75_read_value(struct i2c_client *client, u8 reg)
{
	if (reg == LM75_REG_CONF)
		return i2c_smbus_read_byte_data(client, reg);
	else
		return i2c_smbus_read_word_swapped(client, reg);
}

static int lm75_write_value(struct i2c_client *client, u8 reg, u16 value)
{
	if (reg == LM75_REG_CONF)
		return i2c_smbus_write_byte_data(client, reg, value);
	else
		return i2c_smbus_write_word_swapped(client, reg, value);
}

static struct lm75_data *lm75_update_device(struct device *dev)
{
	struct lm75_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	struct lm75_data *ret = data;

	mutex_lock(&data->update_lock);

	if (time_after(jiffies, data->last_updated + data->sample_time)
	    || !data->valid) {
		int i;
		dev_dbg(&client->dev, "Starting lm75 update\n");

		for (i = 0; i < ARRAY_SIZE(data->temp); i++) {
			int status;

			status = lm75_read_value(client, LM75_REG_TEMP[i]);
			if (unlikely(status < 0)) {
				dev_dbg(dev,
					"LM75: Failed to read value: reg %d, error %d\n",
					LM75_REG_TEMP[i], status);
				ret = ERR_PTR(status);
				data->valid = 0;
				goto abort;
			}
			data->temp[i] = status;
		}
		data->last_updated = jiffies;
		data->valid = 1;
	}

abort:
	mutex_unlock(&data->update_lock);
	return ret;
}

=======
>>>>>>> v4.9.227
module_i2c_driver(lm75_driver);

MODULE_AUTHOR("Frodo Looijaard <frodol@dds.nl>");
MODULE_DESCRIPTION("LM75 driver");
MODULE_LICENSE("GPL");
