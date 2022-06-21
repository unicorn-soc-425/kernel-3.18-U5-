/*
 * Copyright (C) 2011 Alexander Stein <alexander.stein@systec-electronic.com>
 *
<<<<<<< HEAD
 * The LM95245 is a sensor chip made by National Semiconductors.
 * It reports up to two temperatures (its own plus an external one).
 * Complete datasheet can be obtained from National's website at:
 *   http://www.national.com/ds.cgi/LM/LM95245.pdf
=======
 * The LM95245 is a sensor chip made by TI / National Semiconductor.
 * It reports up to two temperatures (its own plus an external one).
>>>>>>> v4.9.227
 *
 * This driver is based on lm95241.c
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
#include <linux/mutex.h>
#include <linux/sysfs.h>

#define DEVNAME "lm95245"
=======
 */

#include <linux/err.h>
#include <linux/init.h>
#include <linux/hwmon.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/regmap.h>
#include <linux/slab.h>
>>>>>>> v4.9.227

static const unsigned short normal_i2c[] = {
	0x18, 0x19, 0x29, 0x4c, 0x4d, I2C_CLIENT_END };

/* LM95245 registers */
/* general registers */
#define LM95245_REG_RW_CONFIG1		0x03
#define LM95245_REG_RW_CONVERS_RATE	0x04
#define LM95245_REG_W_ONE_SHOT		0x0F

/* diode configuration */
#define LM95245_REG_RW_CONFIG2		0xBF
#define LM95245_REG_RW_REMOTE_OFFH	0x11
#define LM95245_REG_RW_REMOTE_OFFL	0x12

/* status registers */
#define LM95245_REG_R_STATUS1		0x02
#define LM95245_REG_R_STATUS2		0x33

/* limit registers */
#define LM95245_REG_RW_REMOTE_OS_LIMIT		0x07
#define LM95245_REG_RW_LOCAL_OS_TCRIT_LIMIT	0x20
#define LM95245_REG_RW_REMOTE_TCRIT_LIMIT	0x19
#define LM95245_REG_RW_COMMON_HYSTERESIS	0x21

/* temperature signed */
#define LM95245_REG_R_LOCAL_TEMPH_S	0x00
#define LM95245_REG_R_LOCAL_TEMPL_S	0x30
#define LM95245_REG_R_REMOTE_TEMPH_S	0x01
#define LM95245_REG_R_REMOTE_TEMPL_S	0x10
/* temperature unsigned */
#define LM95245_REG_R_REMOTE_TEMPH_U	0x31
#define LM95245_REG_R_REMOTE_TEMPL_U	0x32

/* id registers */
#define LM95245_REG_R_MAN_ID		0xFE
#define LM95245_REG_R_CHIP_ID		0xFF

/* LM95245 specific bitfields */
#define CFG_STOP		0x40
#define CFG_REMOTE_TCRIT_MASK	0x10
#define CFG_REMOTE_OS_MASK	0x08
#define CFG_LOCAL_TCRIT_MASK	0x04
#define CFG_LOCAL_OS_MASK	0x02

#define CFG2_OS_A0		0x40
#define CFG2_DIODE_FAULT_OS	0x20
#define CFG2_DIODE_FAULT_TCRIT	0x10
#define CFG2_REMOTE_TT		0x08
#define CFG2_REMOTE_FILTER_DIS	0x00
#define CFG2_REMOTE_FILTER_EN	0x06

/* conversation rate in ms */
#define RATE_CR0063	0x00
#define RATE_CR0364	0x01
#define RATE_CR1000	0x02
#define RATE_CR2500	0x03

<<<<<<< HEAD
=======
#define STATUS1_ROS		0x10
>>>>>>> v4.9.227
#define STATUS1_DIODE_FAULT	0x04
#define STATUS1_RTCRIT		0x02
#define STATUS1_LOC		0x01

#define MANUFACTURER_ID		0x01
<<<<<<< HEAD
#define DEFAULT_REVISION	0xB3
=======
#define LM95235_REVISION	0xB1
#define LM95245_REVISION	0xB3
>>>>>>> v4.9.227

static const u8 lm95245_reg_address[] = {
	LM95245_REG_R_LOCAL_TEMPH_S,
	LM95245_REG_R_LOCAL_TEMPL_S,
	LM95245_REG_R_REMOTE_TEMPH_S,
	LM95245_REG_R_REMOTE_TEMPL_S,
	LM95245_REG_R_REMOTE_TEMPH_U,
	LM95245_REG_R_REMOTE_TEMPL_U,
	LM95245_REG_RW_LOCAL_OS_TCRIT_LIMIT,
	LM95245_REG_RW_REMOTE_TCRIT_LIMIT,
	LM95245_REG_RW_COMMON_HYSTERESIS,
	LM95245_REG_R_STATUS1,
};

/* Client data (each client gets its own) */
struct lm95245_data {
<<<<<<< HEAD
	struct i2c_client *client;
	struct mutex update_lock;
	unsigned long last_updated;	/* in jiffies */
	unsigned long interval;	/* in msecs */
	bool valid;		/* zero until following fields are valid */
	/* registers values */
	u8 regs[ARRAY_SIZE(lm95245_reg_address)];
	u8 config1, config2;
=======
	struct regmap *regmap;
	struct mutex update_lock;
	int interval;	/* in msecs */
>>>>>>> v4.9.227
};

/* Conversions */
static int temp_from_reg_unsigned(u8 val_h, u8 val_l)
{
	return val_h * 1000 + val_l * 1000 / 256;
}

static int temp_from_reg_signed(u8 val_h, u8 val_l)
{
	if (val_h & 0x80)
		return (val_h - 0x100) * 1000;
	return temp_from_reg_unsigned(val_h, val_l);
}

<<<<<<< HEAD
static struct lm95245_data *lm95245_update_device(struct device *dev)
{
	struct lm95245_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;

	mutex_lock(&data->update_lock);

	if (time_after(jiffies, data->last_updated
		+ msecs_to_jiffies(data->interval)) || !data->valid) {
		int i;

		for (i = 0; i < ARRAY_SIZE(lm95245_reg_address); i++)
			data->regs[i]
			  = i2c_smbus_read_byte_data(client,
						     lm95245_reg_address[i]);
		data->last_updated = jiffies;
		data->valid = 1;
	}

	mutex_unlock(&data->update_lock);

	return data;
}

static unsigned long lm95245_read_conversion_rate(struct i2c_client *client)
{
	int rate;
	unsigned long interval;

	rate = i2c_smbus_read_byte_data(client, LM95245_REG_RW_CONVERS_RATE);

	switch (rate) {
	case RATE_CR0063:
		interval = 63;
		break;
	case RATE_CR0364:
		interval = 364;
		break;
	case RATE_CR1000:
		interval = 1000;
		break;
	case RATE_CR2500:
	default:
		interval = 2500;
		break;
	}

	return interval;
}

static unsigned long lm95245_set_conversion_rate(struct i2c_client *client,
			unsigned long interval)
{
	int rate;
=======
static int lm95245_read_conversion_rate(struct lm95245_data *data)
{
	unsigned int rate;
	int ret;

	ret = regmap_read(data->regmap, LM95245_REG_RW_CONVERS_RATE, &rate);
	if (ret < 0)
		return ret;

	switch (rate) {
	case RATE_CR0063:
		data->interval = 63;
		break;
	case RATE_CR0364:
		data->interval = 364;
		break;
	case RATE_CR1000:
		data->interval = 1000;
		break;
	case RATE_CR2500:
	default:
		data->interval = 2500;
		break;
	}
	return 0;
}

static int lm95245_set_conversion_rate(struct lm95245_data *data, long interval)
{
	int ret, rate;
>>>>>>> v4.9.227

	if (interval <= 63) {
		interval = 63;
		rate = RATE_CR0063;
	} else if (interval <= 364) {
		interval = 364;
		rate = RATE_CR0364;
	} else if (interval <= 1000) {
		interval = 1000;
		rate = RATE_CR1000;
	} else {
		interval = 2500;
		rate = RATE_CR2500;
	}

<<<<<<< HEAD
	i2c_smbus_write_byte_data(client, LM95245_REG_RW_CONVERS_RATE, rate);

	return interval;
}

/* Sysfs stuff */
static ssize_t show_input(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct lm95245_data *data = lm95245_update_device(dev);
	int temp;
	int index = to_sensor_dev_attr(attr)->index;

	/*
	 * Index 0 (Local temp) is always signed
	 * Index 2 (Remote temp) has both signed and unsigned data
	 * use signed calculation for remote if signed bit is set
	 */
	if (index == 0 || data->regs[index] & 0x80)
		temp = temp_from_reg_signed(data->regs[index],
			    data->regs[index + 1]);
	else
		temp = temp_from_reg_unsigned(data->regs[index + 2],
			    data->regs[index + 3]);

	return snprintf(buf, PAGE_SIZE - 1, "%d\n", temp);
}

static ssize_t show_limit(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct lm95245_data *data = lm95245_update_device(dev);
	int index = to_sensor_dev_attr(attr)->index;

	return snprintf(buf, PAGE_SIZE - 1, "%d\n",
			data->regs[index] * 1000);
}

static ssize_t set_limit(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count)
{
	struct lm95245_data *data = dev_get_drvdata(dev);
	int index = to_sensor_dev_attr(attr)->index;
	struct i2c_client *client = data->client;
	unsigned long val;

	if (kstrtoul(buf, 10, &val) < 0)
		return -EINVAL;

	val /= 1000;

	val = clamp_val(val, 0, (index == 6 ? 127 : 255));

	mutex_lock(&data->update_lock);

	data->valid = 0;

	i2c_smbus_write_byte_data(client, lm95245_reg_address[index], val);

	mutex_unlock(&data->update_lock);

	return count;
}

static ssize_t show_crit_hyst(struct device *dev, struct device_attribute *attr,
			char *buf)
{
	struct lm95245_data *data = lm95245_update_device(dev);
	int index = to_sensor_dev_attr(attr)->index;
	int hyst = data->regs[index] - data->regs[8];

	return snprintf(buf, PAGE_SIZE - 1, "%d\n", hyst * 1000);
}

static ssize_t set_crit_hyst(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count)
{
	struct lm95245_data *data = dev_get_drvdata(dev);
	int index = to_sensor_dev_attr(attr)->index;
	struct i2c_client *client = data->client;
	unsigned long val;
	int hyst, limit;

	if (kstrtoul(buf, 10, &val) < 0)
		return -EINVAL;

	mutex_lock(&data->update_lock);

	limit = i2c_smbus_read_byte_data(client, lm95245_reg_address[index]);
	hyst = limit - val / 1000;
	hyst = clamp_val(hyst, 0, 31);
	data->regs[8] = hyst;

	/* shared crit hysteresis */
	i2c_smbus_write_byte_data(client, LM95245_REG_RW_COMMON_HYSTERESIS,
		hyst);

	mutex_unlock(&data->update_lock);

	return count;
}

static ssize_t show_type(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct lm95245_data *data = dev_get_drvdata(dev);

	return snprintf(buf, PAGE_SIZE - 1,
		data->config2 & CFG2_REMOTE_TT ? "1\n" : "2\n");
}

static ssize_t set_type(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count)
{
	struct lm95245_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	unsigned long val;

	if (kstrtoul(buf, 10, &val) < 0)
		return -EINVAL;
	if (val != 1 && val != 2)
		return -EINVAL;

	mutex_lock(&data->update_lock);

	if (val == 1)
		data->config2 |= CFG2_REMOTE_TT;
	else
		data->config2 &= ~CFG2_REMOTE_TT;

	data->valid = 0;

	i2c_smbus_write_byte_data(client, LM95245_REG_RW_CONFIG2,
				  data->config2);

	mutex_unlock(&data->update_lock);

	return count;
}

static ssize_t show_alarm(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct lm95245_data *data = lm95245_update_device(dev);
	int index = to_sensor_dev_attr(attr)->index;

	return snprintf(buf, PAGE_SIZE - 1, "%d\n",
			!!(data->regs[9] & index));
}

static ssize_t show_interval(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	struct lm95245_data *data = lm95245_update_device(dev);

	return snprintf(buf, PAGE_SIZE - 1, "%lu\n", data->interval);
}

static ssize_t set_interval(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct lm95245_data *data = dev_get_drvdata(dev);
	struct i2c_client *client = data->client;
	unsigned long val;

	if (kstrtoul(buf, 10, &val) < 0)
		return -EINVAL;

	mutex_lock(&data->update_lock);

	data->interval = lm95245_set_conversion_rate(client, val);

	mutex_unlock(&data->update_lock);

	return count;
}

static SENSOR_DEVICE_ATTR(temp1_input, S_IRUGO, show_input, NULL, 0);
static SENSOR_DEVICE_ATTR(temp1_crit, S_IWUSR | S_IRUGO, show_limit,
		set_limit, 6);
static SENSOR_DEVICE_ATTR(temp1_crit_hyst, S_IWUSR | S_IRUGO, show_crit_hyst,
		set_crit_hyst, 6);
static SENSOR_DEVICE_ATTR(temp1_crit_alarm, S_IRUGO, show_alarm, NULL,
		STATUS1_LOC);

static SENSOR_DEVICE_ATTR(temp2_input, S_IRUGO, show_input, NULL, 2);
static SENSOR_DEVICE_ATTR(temp2_crit, S_IWUSR | S_IRUGO, show_limit,
		set_limit, 7);
static SENSOR_DEVICE_ATTR(temp2_crit_hyst, S_IRUGO, show_crit_hyst, NULL, 7);
static SENSOR_DEVICE_ATTR(temp2_crit_alarm, S_IRUGO, show_alarm, NULL,
		STATUS1_RTCRIT);
static SENSOR_DEVICE_ATTR(temp2_type, S_IWUSR | S_IRUGO, show_type,
		set_type, 0);
static SENSOR_DEVICE_ATTR(temp2_fault, S_IRUGO, show_alarm, NULL,
		STATUS1_DIODE_FAULT);

static DEVICE_ATTR(update_interval, S_IWUSR | S_IRUGO, show_interval,
		set_interval);

static struct attribute *lm95245_attrs[] = {
	&sensor_dev_attr_temp1_input.dev_attr.attr,
	&sensor_dev_attr_temp1_crit.dev_attr.attr,
	&sensor_dev_attr_temp1_crit_hyst.dev_attr.attr,
	&sensor_dev_attr_temp1_crit_alarm.dev_attr.attr,
	&sensor_dev_attr_temp2_input.dev_attr.attr,
	&sensor_dev_attr_temp2_crit.dev_attr.attr,
	&sensor_dev_attr_temp2_crit_hyst.dev_attr.attr,
	&sensor_dev_attr_temp2_crit_alarm.dev_attr.attr,
	&sensor_dev_attr_temp2_type.dev_attr.attr,
	&sensor_dev_attr_temp2_fault.dev_attr.attr,
	&dev_attr_update_interval.attr,
	NULL
};
ATTRIBUTE_GROUPS(lm95245);
=======
	ret = regmap_write(data->regmap, LM95245_REG_RW_CONVERS_RATE, rate);
	if (ret < 0)
		return ret;

	data->interval = interval;
	return 0;
}

static int lm95245_read_temp(struct device *dev, u32 attr, int channel,
			     long *val)
{
	struct lm95245_data *data = dev_get_drvdata(dev);
	struct regmap *regmap = data->regmap;
	int ret, regl, regh, regvall, regvalh;

	switch (attr) {
	case hwmon_temp_input:
		regl = channel ? LM95245_REG_R_REMOTE_TEMPL_S :
				 LM95245_REG_R_LOCAL_TEMPL_S;
		regh = channel ? LM95245_REG_R_REMOTE_TEMPH_S :
				 LM95245_REG_R_LOCAL_TEMPH_S;
		ret = regmap_read(regmap, regl, &regvall);
		if (ret < 0)
			return ret;
		ret = regmap_read(regmap, regh, &regvalh);
		if (ret < 0)
			return ret;
		/*
		 * Local temp is always signed.
		 * Remote temp has both signed and unsigned data.
		 * Use signed calculation for remote if signed bit is set
		 * or if reported temperature is below signed limit.
		 */
		if (!channel || (regvalh & 0x80) || regvalh < 0x7f) {
			*val = temp_from_reg_signed(regvalh, regvall);
			return 0;
		}
		ret = regmap_read(regmap, LM95245_REG_R_REMOTE_TEMPL_U,
				  &regvall);
		if (ret < 0)
			return ret;
		ret = regmap_read(regmap, LM95245_REG_R_REMOTE_TEMPH_U,
				  &regvalh);
		if (ret < 0)
			return ret;
		*val = temp_from_reg_unsigned(regvalh, regvall);
		return 0;
	case hwmon_temp_max:
		ret = regmap_read(regmap, LM95245_REG_RW_REMOTE_OS_LIMIT,
				  &regvalh);
		if (ret < 0)
			return ret;
		*val = regvalh * 1000;
		return 0;
	case hwmon_temp_crit:
		regh = channel ? LM95245_REG_RW_REMOTE_TCRIT_LIMIT :
				 LM95245_REG_RW_LOCAL_OS_TCRIT_LIMIT;
		ret = regmap_read(regmap, regh, &regvalh);
		if (ret < 0)
			return ret;
		*val = regvalh * 1000;
		return 0;
	case hwmon_temp_max_hyst:
		ret = regmap_read(regmap, LM95245_REG_RW_REMOTE_OS_LIMIT,
				  &regvalh);
		if (ret < 0)
			return ret;
		ret = regmap_read(regmap, LM95245_REG_RW_COMMON_HYSTERESIS,
				  &regvall);
		if (ret < 0)
			return ret;
		*val = (regvalh - regvall) * 1000;
		return 0;
	case hwmon_temp_crit_hyst:
		regh = channel ? LM95245_REG_RW_REMOTE_TCRIT_LIMIT :
				 LM95245_REG_RW_LOCAL_OS_TCRIT_LIMIT;
		ret = regmap_read(regmap, regh, &regvalh);
		if (ret < 0)
			return ret;
		ret = regmap_read(regmap, LM95245_REG_RW_COMMON_HYSTERESIS,
				  &regvall);
		if (ret < 0)
			return ret;
		*val = (regvalh - regvall) * 1000;
		return 0;
	case hwmon_temp_type:
		ret = regmap_read(regmap, LM95245_REG_RW_CONFIG2, &regvalh);
		if (ret < 0)
			return ret;
		*val = (regvalh & CFG2_REMOTE_TT) ? 1 : 2;
		return 0;
	case hwmon_temp_offset:
		ret = regmap_read(regmap, LM95245_REG_RW_REMOTE_OFFL,
				  &regvall);
		if (ret < 0)
			return ret;
		ret = regmap_read(regmap, LM95245_REG_RW_REMOTE_OFFH,
				  &regvalh);
		if (ret < 0)
			return ret;
		*val = temp_from_reg_signed(regvalh, regvall);
		return 0;
	case hwmon_temp_max_alarm:
		ret = regmap_read(regmap, LM95245_REG_R_STATUS1, &regvalh);
		if (ret < 0)
			return ret;
		*val = !!(regvalh & STATUS1_ROS);
		return 0;
	case hwmon_temp_crit_alarm:
		ret = regmap_read(regmap, LM95245_REG_R_STATUS1, &regvalh);
		if (ret < 0)
			return ret;
		*val = !!(regvalh & (channel ? STATUS1_RTCRIT : STATUS1_LOC));
		return 0;
	case hwmon_temp_fault:
		ret = regmap_read(regmap, LM95245_REG_R_STATUS1, &regvalh);
		if (ret < 0)
			return ret;
		*val = !!(regvalh & STATUS1_DIODE_FAULT);
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

static int lm95245_write_temp(struct device *dev, u32 attr, int channel,
			      long val)
{
	struct lm95245_data *data = dev_get_drvdata(dev);
	struct regmap *regmap = data->regmap;
	unsigned int regval;
	int ret, reg;

	switch (attr) {
	case hwmon_temp_max:
		val = clamp_val(val / 1000, 0, 255);
		ret = regmap_write(regmap, LM95245_REG_RW_REMOTE_OS_LIMIT, val);
		return ret;
	case hwmon_temp_crit:
		reg = channel ? LM95245_REG_RW_REMOTE_TCRIT_LIMIT :
				LM95245_REG_RW_LOCAL_OS_TCRIT_LIMIT;
		val = clamp_val(val / 1000, 0, channel ? 255 : 127);
		ret = regmap_write(regmap, reg, val);
		return ret;
	case hwmon_temp_crit_hyst:
		mutex_lock(&data->update_lock);
		ret = regmap_read(regmap, LM95245_REG_RW_LOCAL_OS_TCRIT_LIMIT,
				  &regval);
		if (ret < 0) {
			mutex_unlock(&data->update_lock);
			return ret;
		}
		/* Clamp to reasonable range to prevent overflow */
		val = clamp_val(val, -1000000, 1000000);
		val = regval - val / 1000;
		val = clamp_val(val, 0, 31);
		ret = regmap_write(regmap, LM95245_REG_RW_COMMON_HYSTERESIS,
				   val);
		mutex_unlock(&data->update_lock);
		return ret;
	case hwmon_temp_offset:
		val = clamp_val(val, -128000, 127875);
		val = val * 256 / 1000;
		mutex_lock(&data->update_lock);
		ret = regmap_write(regmap, LM95245_REG_RW_REMOTE_OFFL,
				   val & 0xe0);
		if (ret < 0) {
			mutex_unlock(&data->update_lock);
			return ret;
		}
		ret = regmap_write(regmap, LM95245_REG_RW_REMOTE_OFFH,
				   (val >> 8) & 0xff);
		mutex_unlock(&data->update_lock);
		return ret;
	case hwmon_temp_type:
		if (val != 1 && val != 2)
			return -EINVAL;
		ret = regmap_update_bits(regmap, LM95245_REG_RW_CONFIG2,
					 CFG2_REMOTE_TT,
					 val == 1 ? CFG2_REMOTE_TT : 0);
		return ret;
	default:
		return -EOPNOTSUPP;
	}
}

static int lm95245_read_chip(struct device *dev, u32 attr, int channel,
			     long *val)
{
	struct lm95245_data *data = dev_get_drvdata(dev);

	switch (attr) {
	case hwmon_chip_update_interval:
		*val = data->interval;
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

static int lm95245_write_chip(struct device *dev, u32 attr, int channel,
			      long val)
{
	struct lm95245_data *data = dev_get_drvdata(dev);
	int ret;

	switch (attr) {
	case hwmon_chip_update_interval:
		mutex_lock(&data->update_lock);
		ret = lm95245_set_conversion_rate(data, val);
		mutex_unlock(&data->update_lock);
		return ret;
	default:
		return -EOPNOTSUPP;
	}
}

static int lm95245_read(struct device *dev, enum hwmon_sensor_types type,
			u32 attr, int channel, long *val)
{
	switch (type) {
	case hwmon_chip:
		return lm95245_read_chip(dev, attr, channel, val);
	case hwmon_temp:
		return lm95245_read_temp(dev, attr, channel, val);
	default:
		return -EOPNOTSUPP;
	}
}

static int lm95245_write(struct device *dev, enum hwmon_sensor_types type,
			 u32 attr, int channel, long val)
{
	switch (type) {
	case hwmon_chip:
		return lm95245_write_chip(dev, attr, channel, val);
	case hwmon_temp:
		return lm95245_write_temp(dev, attr, channel, val);
	default:
		return -EOPNOTSUPP;
	}
}

static umode_t lm95245_temp_is_visible(const void *data, u32 attr, int channel)
{
	switch (attr) {
	case hwmon_temp_input:
	case hwmon_temp_max_alarm:
	case hwmon_temp_max_hyst:
	case hwmon_temp_crit_alarm:
	case hwmon_temp_fault:
		return S_IRUGO;
	case hwmon_temp_type:
	case hwmon_temp_max:
	case hwmon_temp_crit:
	case hwmon_temp_offset:
		return S_IRUGO | S_IWUSR;
	case hwmon_temp_crit_hyst:
		return (channel == 0) ? S_IRUGO | S_IWUSR : S_IRUGO;
	default:
		return 0;
	}
}

static umode_t lm95245_is_visible(const void *data,
				  enum hwmon_sensor_types type,
				  u32 attr, int channel)
{
	switch (type) {
	case hwmon_chip:
		switch (attr) {
		case hwmon_chip_update_interval:
			return S_IRUGO | S_IWUSR;
		default:
			return 0;
		}
	case hwmon_temp:
		return lm95245_temp_is_visible(data, attr, channel);
	default:
		return 0;
	}
}
>>>>>>> v4.9.227

/* Return 0 if detection is successful, -ENODEV otherwise */
static int lm95245_detect(struct i2c_client *new_client,
			  struct i2c_board_info *info)
{
	struct i2c_adapter *adapter = new_client->adapter;
<<<<<<< HEAD
=======
	int address = new_client->addr;
	const char *name;
	int rev, id;
>>>>>>> v4.9.227

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return -ENODEV;

<<<<<<< HEAD
	if (i2c_smbus_read_byte_data(new_client, LM95245_REG_R_MAN_ID)
			!= MANUFACTURER_ID
		|| i2c_smbus_read_byte_data(new_client, LM95245_REG_R_CHIP_ID)
			!= DEFAULT_REVISION)
		return -ENODEV;

	strlcpy(info->type, DEVNAME, I2C_NAME_SIZE);
	return 0;
}

static void lm95245_init_client(struct i2c_client *client,
				struct lm95245_data *data)
{
	data->interval = lm95245_read_conversion_rate(client);

	data->config1 = i2c_smbus_read_byte_data(client,
		LM95245_REG_RW_CONFIG1);
	data->config2 = i2c_smbus_read_byte_data(client,
		LM95245_REG_RW_CONFIG2);

	if (data->config1 & CFG_STOP) {
		/* Clear the standby bit */
		data->config1 &= ~CFG_STOP;
		i2c_smbus_write_byte_data(client, LM95245_REG_RW_CONFIG1,
			data->config1);
	}
}

=======
	id = i2c_smbus_read_byte_data(new_client, LM95245_REG_R_MAN_ID);
	if (id != MANUFACTURER_ID)
		return -ENODEV;

	rev = i2c_smbus_read_byte_data(new_client, LM95245_REG_R_CHIP_ID);
	switch (rev) {
	case LM95235_REVISION:
		if (address != 0x18 && address != 0x29 && address != 0x4c)
			return -ENODEV;
		name = "lm95235";
		break;
	case LM95245_REVISION:
		name = "lm95245";
		break;
	default:
		return -ENODEV;
	}

	strlcpy(info->type, name, I2C_NAME_SIZE);
	return 0;
}

static int lm95245_init_client(struct lm95245_data *data)
{
	int ret;

	ret = lm95245_read_conversion_rate(data);
	if (ret < 0)
		return ret;

	return regmap_update_bits(data->regmap, LM95245_REG_RW_CONFIG1,
				  CFG_STOP, 0);
}

static bool lm95245_is_writeable_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case LM95245_REG_RW_CONFIG1:
	case LM95245_REG_RW_CONVERS_RATE:
	case LM95245_REG_W_ONE_SHOT:
	case LM95245_REG_RW_CONFIG2:
	case LM95245_REG_RW_REMOTE_OFFH:
	case LM95245_REG_RW_REMOTE_OFFL:
	case LM95245_REG_RW_REMOTE_OS_LIMIT:
	case LM95245_REG_RW_LOCAL_OS_TCRIT_LIMIT:
	case LM95245_REG_RW_REMOTE_TCRIT_LIMIT:
	case LM95245_REG_RW_COMMON_HYSTERESIS:
		return true;
	default:
		return false;
	}
}

static bool lm95245_is_volatile_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case LM95245_REG_R_STATUS1:
	case LM95245_REG_R_STATUS2:
	case LM95245_REG_R_LOCAL_TEMPH_S:
	case LM95245_REG_R_LOCAL_TEMPL_S:
	case LM95245_REG_R_REMOTE_TEMPH_S:
	case LM95245_REG_R_REMOTE_TEMPL_S:
	case LM95245_REG_R_REMOTE_TEMPH_U:
	case LM95245_REG_R_REMOTE_TEMPL_U:
		return true;
	default:
		return false;
	}
}

static const struct regmap_config lm95245_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.writeable_reg = lm95245_is_writeable_reg,
	.volatile_reg = lm95245_is_volatile_reg,
	.cache_type = REGCACHE_RBTREE,
	.use_single_rw = true,
};

static const u32 lm95245_chip_config[] = {
	HWMON_C_UPDATE_INTERVAL,
	0
};

static const struct hwmon_channel_info lm95245_chip = {
	.type = hwmon_chip,
	.config = lm95245_chip_config,
};

static const u32 lm95245_temp_config[] = {
	HWMON_T_INPUT | HWMON_T_CRIT | HWMON_T_CRIT_HYST | HWMON_T_CRIT_ALARM,
	HWMON_T_INPUT | HWMON_T_MAX | HWMON_T_MAX_HYST | HWMON_T_CRIT |
		HWMON_T_CRIT_HYST | HWMON_T_FAULT | HWMON_T_MAX_ALARM |
		HWMON_T_CRIT_ALARM | HWMON_T_TYPE | HWMON_T_OFFSET,
	0
};

static const struct hwmon_channel_info lm95245_temp = {
	.type = hwmon_temp,
	.config = lm95245_temp_config,
};

static const struct hwmon_channel_info *lm95245_info[] = {
	&lm95245_chip,
	&lm95245_temp,
	NULL
};

static const struct hwmon_ops lm95245_hwmon_ops = {
	.is_visible = lm95245_is_visible,
	.read = lm95245_read,
	.write = lm95245_write,
};

static const struct hwmon_chip_info lm95245_chip_info = {
	.ops = &lm95245_hwmon_ops,
	.info = lm95245_info,
};

>>>>>>> v4.9.227
static int lm95245_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct lm95245_data *data;
	struct device *hwmon_dev;
<<<<<<< HEAD
=======
	int ret;
>>>>>>> v4.9.227

	data = devm_kzalloc(dev, sizeof(struct lm95245_data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

<<<<<<< HEAD
	data->client = client;
	mutex_init(&data->update_lock);

	/* Initialize the LM95245 chip */
	lm95245_init_client(client, data);

	hwmon_dev = devm_hwmon_device_register_with_groups(dev, client->name,
							   data,
							   lm95245_groups);
=======
	data->regmap = devm_regmap_init_i2c(client, &lm95245_regmap_config);
	if (IS_ERR(data->regmap))
		return PTR_ERR(data->regmap);

	mutex_init(&data->update_lock);

	/* Initialize the LM95245 chip */
	ret = lm95245_init_client(data);
	if (ret < 0)
		return ret;

	hwmon_dev = devm_hwmon_device_register_with_info(dev, client->name,
							 data,
							 &lm95245_chip_info,
							 NULL);
>>>>>>> v4.9.227
	return PTR_ERR_OR_ZERO(hwmon_dev);
}

/* Driver data (common to all clients) */
static const struct i2c_device_id lm95245_id[] = {
<<<<<<< HEAD
	{ DEVNAME, 0 },
=======
	{ "lm95235", 0 },
	{ "lm95245", 0 },
>>>>>>> v4.9.227
	{ }
};
MODULE_DEVICE_TABLE(i2c, lm95245_id);

static struct i2c_driver lm95245_driver = {
	.class		= I2C_CLASS_HWMON,
	.driver = {
<<<<<<< HEAD
		.name	= DEVNAME,
=======
		.name	= "lm95245",
>>>>>>> v4.9.227
	},
	.probe		= lm95245_probe,
	.id_table	= lm95245_id,
	.detect		= lm95245_detect,
	.address_list	= normal_i2c,
};

module_i2c_driver(lm95245_driver);

MODULE_AUTHOR("Alexander Stein <alexander.stein@systec-electronic.com>");
<<<<<<< HEAD
MODULE_DESCRIPTION("LM95245 sensor driver");
=======
MODULE_DESCRIPTION("LM95235/LM95245 sensor driver");
>>>>>>> v4.9.227
MODULE_LICENSE("GPL");
