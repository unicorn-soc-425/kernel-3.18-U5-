/* Texas Instruments TMP102 SMBus temperature sensor driver
 *
 * Copyright (C) 2010 Steven King <sfking@fdwdc.com>
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
<<<<<<< HEAD
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA
 */

=======
 */

#include <linux/delay.h>
>>>>>>> v4.9.227
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/hwmon.h>
#include <linux/hwmon-sysfs.h>
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/jiffies.h>
<<<<<<< HEAD
#include <linux/thermal.h>
=======
#include <linux/regmap.h>
>>>>>>> v4.9.227
#include <linux/of.h>

#define	DRIVER_NAME "tmp102"

#define	TMP102_TEMP_REG			0x00
#define	TMP102_CONF_REG			0x01
/* note: these bit definitions are byte swapped */
#define		TMP102_CONF_SD		0x0100
#define		TMP102_CONF_TM		0x0200
#define		TMP102_CONF_POL		0x0400
#define		TMP102_CONF_F0		0x0800
#define		TMP102_CONF_F1		0x1000
#define		TMP102_CONF_R0		0x2000
#define		TMP102_CONF_R1		0x4000
#define		TMP102_CONF_OS		0x8000
#define		TMP102_CONF_EM		0x0010
#define		TMP102_CONF_AL		0x0020
#define		TMP102_CONF_CR0		0x0040
#define		TMP102_CONF_CR1		0x0080
#define	TMP102_TLOW_REG			0x02
#define	TMP102_THIGH_REG		0x03

<<<<<<< HEAD
struct tmp102 {
	struct i2c_client *client;
	struct device *hwmon_dev;
	struct thermal_zone_device *tz;
	struct mutex lock;
	u16 config_orig;
	unsigned long last_update;
	int temp[3];
=======
#define TMP102_CONFREG_MASK	(TMP102_CONF_SD | TMP102_CONF_TM | \
				 TMP102_CONF_POL | TMP102_CONF_F0 | \
				 TMP102_CONF_F1 | TMP102_CONF_OS | \
				 TMP102_CONF_EM | TMP102_CONF_AL | \
				 TMP102_CONF_CR0 | TMP102_CONF_CR1)

#define TMP102_CONFIG_CLEAR	(TMP102_CONF_SD | TMP102_CONF_OS | \
				 TMP102_CONF_CR0)
#define TMP102_CONFIG_SET	(TMP102_CONF_TM | TMP102_CONF_EM | \
				 TMP102_CONF_CR1)

#define CONVERSION_TIME_MS		35	/* in milli-seconds */

struct tmp102 {
	struct regmap *regmap;
	u16 config_orig;
	unsigned long ready_time;
>>>>>>> v4.9.227
};

/* convert left adjusted 13-bit TMP102 register value to milliCelsius */
static inline int tmp102_reg_to_mC(s16 val)
{
	return ((val & ~0x01) * 1000) / 128;
}

/* convert milliCelsius to left adjusted 13-bit TMP102 register value */
static inline u16 tmp102_mC_to_reg(int val)
{
	return (val * 128) / 1000;
}

<<<<<<< HEAD
static const u8 tmp102_reg[] = {
	TMP102_TEMP_REG,
	TMP102_TLOW_REG,
	TMP102_THIGH_REG,
};

static struct tmp102 *tmp102_update_device(struct device *dev)
{
	struct tmp102 *tmp102 = dev_get_drvdata(dev);
	struct i2c_client *client = tmp102->client;

	mutex_lock(&tmp102->lock);
	if (time_after(jiffies, tmp102->last_update + HZ / 3)) {
		int i;
		for (i = 0; i < ARRAY_SIZE(tmp102->temp); ++i) {
			int status = i2c_smbus_read_word_swapped(client,
								 tmp102_reg[i]);
			if (status > -1)
				tmp102->temp[i] = tmp102_reg_to_mC(status);
		}
		tmp102->last_update = jiffies;
	}
	mutex_unlock(&tmp102->lock);
	return tmp102;
}

static int tmp102_read_temp(void *dev, long *temp)
{
	struct tmp102 *tmp102 = tmp102_update_device(dev);

	*temp = tmp102->temp[0];
=======
static int tmp102_read(struct device *dev, enum hwmon_sensor_types type,
		       u32 attr, int channel, long *temp)
{
	struct tmp102 *tmp102 = dev_get_drvdata(dev);
	unsigned int regval;
	int err, reg;

	switch (attr) {
	case hwmon_temp_input:
		/* Is it too early to return a conversion ? */
		if (time_before(jiffies, tmp102->ready_time)) {
			dev_dbg(dev, "%s: Conversion not ready yet..\n", __func__);
			return -EAGAIN;
		}
		reg = TMP102_TEMP_REG;
		break;
	case hwmon_temp_max_hyst:
		reg = TMP102_TLOW_REG;
		break;
	case hwmon_temp_max:
		reg = TMP102_THIGH_REG;
		break;
	default:
		return -EOPNOTSUPP;
	}

	err = regmap_read(tmp102->regmap, reg, &regval);
	if (err < 0)
		return err;
	*temp = tmp102_reg_to_mC(regval);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static ssize_t tmp102_show_temp(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	struct sensor_device_attribute *sda = to_sensor_dev_attr(attr);
	struct tmp102 *tmp102 = tmp102_update_device(dev);

	return sprintf(buf, "%d\n", tmp102->temp[sda->index]);
}

static ssize_t tmp102_set_temp(struct device *dev,
			       struct device_attribute *attr,
			       const char *buf, size_t count)
{
	struct sensor_device_attribute *sda = to_sensor_dev_attr(attr);
	struct tmp102 *tmp102 = dev_get_drvdata(dev);
	struct i2c_client *client = tmp102->client;
	long val;
	int status;

	if (kstrtol(buf, 10, &val) < 0)
		return -EINVAL;
	val = clamp_val(val, -256000, 255000);

	mutex_lock(&tmp102->lock);
	tmp102->temp[sda->index] = val;
	status = i2c_smbus_write_word_swapped(client, tmp102_reg[sda->index],
					      tmp102_mC_to_reg(val));
	mutex_unlock(&tmp102->lock);
	return status ? : count;
}

static SENSOR_DEVICE_ATTR(temp1_input, S_IRUGO, tmp102_show_temp, NULL , 0);

static SENSOR_DEVICE_ATTR(temp1_max_hyst, S_IWUSR | S_IRUGO, tmp102_show_temp,
			  tmp102_set_temp, 1);

static SENSOR_DEVICE_ATTR(temp1_max, S_IWUSR | S_IRUGO, tmp102_show_temp,
			  tmp102_set_temp, 2);

static struct attribute *tmp102_attrs[] = {
	&sensor_dev_attr_temp1_input.dev_attr.attr,
	&sensor_dev_attr_temp1_max_hyst.dev_attr.attr,
	&sensor_dev_attr_temp1_max.dev_attr.attr,
	NULL
};
ATTRIBUTE_GROUPS(tmp102);

#define TMP102_CONFIG  (TMP102_CONF_TM | TMP102_CONF_EM | TMP102_CONF_CR1)
#define TMP102_CONFIG_RD_ONLY (TMP102_CONF_R0 | TMP102_CONF_R1 | TMP102_CONF_AL)

static const struct thermal_zone_of_device_ops tmp102_of_thermal_ops = {
	.get_temp = tmp102_read_temp,
};

static int tmp102_probe(struct i2c_client *client,
				  const struct i2c_device_id *id)
=======
static int tmp102_write(struct device *dev, enum hwmon_sensor_types type,
			u32 attr, int channel, long temp)
{
	struct tmp102 *tmp102 = dev_get_drvdata(dev);
	int reg;

	switch (attr) {
	case hwmon_temp_max_hyst:
		reg = TMP102_TLOW_REG;
		break;
	case hwmon_temp_max:
		reg = TMP102_THIGH_REG;
		break;
	default:
		return -EOPNOTSUPP;
	}

	temp = clamp_val(temp, -256000, 255000);
	return regmap_write(tmp102->regmap, reg, tmp102_mC_to_reg(temp));
}

static umode_t tmp102_is_visible(const void *data, enum hwmon_sensor_types type,
				 u32 attr, int channel)
{
	if (type != hwmon_temp)
		return 0;

	switch (attr) {
	case hwmon_temp_input:
		return S_IRUGO;
	case hwmon_temp_max_hyst:
	case hwmon_temp_max:
		return S_IRUGO | S_IWUSR;
	default:
		return 0;
	}
}

static u32 tmp102_chip_config[] = {
	HWMON_C_REGISTER_TZ,
	0
};

static const struct hwmon_channel_info tmp102_chip = {
	.type = hwmon_chip,
	.config = tmp102_chip_config,
};

static u32 tmp102_temp_config[] = {
	HWMON_T_INPUT | HWMON_T_MAX | HWMON_T_MAX_HYST,
	0
};

static const struct hwmon_channel_info tmp102_temp = {
	.type = hwmon_temp,
	.config = tmp102_temp_config,
};

static const struct hwmon_channel_info *tmp102_info[] = {
	&tmp102_chip,
	&tmp102_temp,
	NULL
};

static const struct hwmon_ops tmp102_hwmon_ops = {
	.is_visible = tmp102_is_visible,
	.read = tmp102_read,
	.write = tmp102_write,
};

static const struct hwmon_chip_info tmp102_chip_info = {
	.ops = &tmp102_hwmon_ops,
	.info = tmp102_info,
};

static void tmp102_restore_config(void *data)
{
	struct tmp102 *tmp102 = data;

	regmap_write(tmp102->regmap, TMP102_CONF_REG, tmp102->config_orig);
}

static bool tmp102_is_writeable_reg(struct device *dev, unsigned int reg)
{
	return reg != TMP102_TEMP_REG;
}

static bool tmp102_is_volatile_reg(struct device *dev, unsigned int reg)
{
	return reg == TMP102_TEMP_REG;
}

static const struct regmap_config tmp102_regmap_config = {
	.reg_bits = 8,
	.val_bits = 16,
	.max_register = TMP102_THIGH_REG,
	.writeable_reg = tmp102_is_writeable_reg,
	.volatile_reg = tmp102_is_volatile_reg,
	.val_format_endian = REGMAP_ENDIAN_BIG,
	.cache_type = REGCACHE_RBTREE,
	.use_single_rw = true,
};

static int tmp102_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
>>>>>>> v4.9.227
{
	struct device *dev = &client->dev;
	struct device *hwmon_dev;
	struct tmp102 *tmp102;
<<<<<<< HEAD
	int status;
=======
	unsigned int regval;
	int err;
>>>>>>> v4.9.227

	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_WORD_DATA)) {
		dev_err(dev,
			"adapter doesn't support SMBus word transactions\n");
		return -ENODEV;
	}

	tmp102 = devm_kzalloc(dev, sizeof(*tmp102), GFP_KERNEL);
	if (!tmp102)
		return -ENOMEM;

	i2c_set_clientdata(client, tmp102);
<<<<<<< HEAD
	tmp102->client = client;

	status = i2c_smbus_read_word_swapped(client, TMP102_CONF_REG);
	if (status < 0) {
		dev_err(dev, "error reading config register\n");
		return status;
	}
	tmp102->config_orig = status;
	status = i2c_smbus_write_word_swapped(client, TMP102_CONF_REG,
					      TMP102_CONFIG);
	if (status < 0) {
		dev_err(dev, "error writing config register\n");
		goto fail_restore_config;
	}
	status = i2c_smbus_read_word_swapped(client, TMP102_CONF_REG);
	if (status < 0) {
		dev_err(dev, "error reading config register\n");
		goto fail_restore_config;
	}
	status &= ~TMP102_CONFIG_RD_ONLY;
	if (status != TMP102_CONFIG) {
		dev_err(dev, "config settings did not stick\n");
		status = -ENODEV;
		goto fail_restore_config;
	}
	tmp102->last_update = jiffies - HZ;
	mutex_init(&tmp102->lock);

	hwmon_dev = hwmon_device_register_with_groups(dev, client->name,
						      tmp102, tmp102_groups);
	if (IS_ERR(hwmon_dev)) {
		dev_dbg(dev, "unable to register hwmon device\n");
		status = PTR_ERR(hwmon_dev);
		goto fail_restore_config;
	}
	tmp102->hwmon_dev = hwmon_dev;
	tmp102->tz = thermal_zone_of_sensor_register(hwmon_dev, 0, hwmon_dev,
						     &tmp102_of_thermal_ops);
	if (IS_ERR(tmp102->tz))
		tmp102->tz = NULL;

	dev_info(dev, "initialized\n");

	return 0;

fail_restore_config:
	i2c_smbus_write_word_swapped(client, TMP102_CONF_REG,
				     tmp102->config_orig);
	return status;
}

static int tmp102_remove(struct i2c_client *client)
{
	struct tmp102 *tmp102 = i2c_get_clientdata(client);

	thermal_zone_of_sensor_unregister(tmp102->hwmon_dev, tmp102->tz);
	hwmon_device_unregister(tmp102->hwmon_dev);

	/* Stop monitoring if device was stopped originally */
	if (tmp102->config_orig & TMP102_CONF_SD) {
		int config;

		config = i2c_smbus_read_word_swapped(client, TMP102_CONF_REG);
		if (config >= 0)
			i2c_smbus_write_word_swapped(client, TMP102_CONF_REG,
						     config | TMP102_CONF_SD);
	}

	return 0;
}

#ifdef CONFIG_PM
static int tmp102_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	int config;

	config = i2c_smbus_read_word_swapped(client, TMP102_CONF_REG);
	if (config < 0)
		return config;

	config |= TMP102_CONF_SD;
	return i2c_smbus_write_word_swapped(client, TMP102_CONF_REG, config);
=======

	tmp102->regmap = devm_regmap_init_i2c(client, &tmp102_regmap_config);
	if (IS_ERR(tmp102->regmap))
		return PTR_ERR(tmp102->regmap);

	err = regmap_read(tmp102->regmap, TMP102_CONF_REG, &regval);
	if (err < 0) {
		dev_err(dev, "error reading config register\n");
		return err;
	}

	if ((regval & ~TMP102_CONFREG_MASK) !=
	    (TMP102_CONF_R0 | TMP102_CONF_R1)) {
		dev_err(dev, "unexpected config register value\n");
		return -ENODEV;
	}

	tmp102->config_orig = regval;

	err = devm_add_action_or_reset(dev, tmp102_restore_config, tmp102);
	if (err)
		return err;

	regval &= ~TMP102_CONFIG_CLEAR;
	regval |= TMP102_CONFIG_SET;

	err = regmap_write(tmp102->regmap, TMP102_CONF_REG, regval);
	if (err < 0) {
		dev_err(dev, "error writing config register\n");
		return err;
	}

	tmp102->ready_time = jiffies;
	if (tmp102->config_orig & TMP102_CONF_SD) {
		/*
		 * Mark that we are not ready with data until the first
		 * conversion is complete
		 */
		tmp102->ready_time += msecs_to_jiffies(CONVERSION_TIME_MS);
	}

	hwmon_dev = devm_hwmon_device_register_with_info(dev, client->name,
							 tmp102,
							 &tmp102_chip_info,
							 NULL);
	if (IS_ERR(hwmon_dev)) {
		dev_dbg(dev, "unable to register hwmon device\n");
		return PTR_ERR(hwmon_dev);
	}
	dev_info(dev, "initialized\n");

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int tmp102_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct tmp102 *tmp102 = i2c_get_clientdata(client);

	return regmap_update_bits(tmp102->regmap, TMP102_CONF_REG,
				  TMP102_CONF_SD, TMP102_CONF_SD);
>>>>>>> v4.9.227
}

static int tmp102_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
<<<<<<< HEAD
	int config;

	config = i2c_smbus_read_word_swapped(client, TMP102_CONF_REG);
	if (config < 0)
		return config;

	config &= ~TMP102_CONF_SD;
	return i2c_smbus_write_word_swapped(client, TMP102_CONF_REG, config);
}

static const struct dev_pm_ops tmp102_dev_pm_ops = {
	.suspend	= tmp102_suspend,
	.resume		= tmp102_resume,
};

#define TMP102_DEV_PM_OPS (&tmp102_dev_pm_ops)
#else
#define	TMP102_DEV_PM_OPS NULL
#endif /* CONFIG_PM */

=======
	struct tmp102 *tmp102 = i2c_get_clientdata(client);
	int err;

	err = regmap_update_bits(tmp102->regmap, TMP102_CONF_REG,
				 TMP102_CONF_SD, 0);

	tmp102->ready_time = jiffies + msecs_to_jiffies(CONVERSION_TIME_MS);

	return err;
}
#endif /* CONFIG_PM */

static SIMPLE_DEV_PM_OPS(tmp102_dev_pm_ops, tmp102_suspend, tmp102_resume);

>>>>>>> v4.9.227
static const struct i2c_device_id tmp102_id[] = {
	{ "tmp102", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, tmp102_id);

static struct i2c_driver tmp102_driver = {
	.driver.name	= DRIVER_NAME,
<<<<<<< HEAD
	.driver.pm	= TMP102_DEV_PM_OPS,
	.probe		= tmp102_probe,
	.remove		= tmp102_remove,
=======
	.driver.pm	= &tmp102_dev_pm_ops,
	.probe		= tmp102_probe,
>>>>>>> v4.9.227
	.id_table	= tmp102_id,
};

module_i2c_driver(tmp102_driver);

MODULE_AUTHOR("Steven King <sfking@fdwdc.com>");
MODULE_DESCRIPTION("Texas Instruments TMP102 temperature sensor driver");
MODULE_LICENSE("GPL");
