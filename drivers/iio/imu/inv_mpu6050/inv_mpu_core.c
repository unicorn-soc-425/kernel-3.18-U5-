/*
* Copyright (C) 2012 Invensense, Inc.
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/sysfs.h>
#include <linux/jiffies.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/kfifo.h>
#include <linux/spinlock.h>
#include <linux/iio/iio.h>
<<<<<<< HEAD
=======
#include <linux/acpi.h>
>>>>>>> v4.9.227
#include "inv_mpu_iio.h"

/*
 * this is the gyro scale translated from dynamic range plus/minus
 * {250, 500, 1000, 2000} to rad/s
 */
static const int gyro_scale_6050[] = {133090, 266181, 532362, 1064724};

/*
 * this is the accel scale translated from dynamic range plus/minus
 * {2, 4, 8, 16} to m/s^2
 */
static const int accel_scale[] = {598, 1196, 2392, 4785};

<<<<<<< HEAD
=======
static const struct inv_mpu6050_reg_map reg_set_6500 = {
	.sample_rate_div	= INV_MPU6050_REG_SAMPLE_RATE_DIV,
	.lpf                    = INV_MPU6050_REG_CONFIG,
	.accel_lpf              = INV_MPU6500_REG_ACCEL_CONFIG_2,
	.user_ctrl              = INV_MPU6050_REG_USER_CTRL,
	.fifo_en                = INV_MPU6050_REG_FIFO_EN,
	.gyro_config            = INV_MPU6050_REG_GYRO_CONFIG,
	.accl_config            = INV_MPU6050_REG_ACCEL_CONFIG,
	.fifo_count_h           = INV_MPU6050_REG_FIFO_COUNT_H,
	.fifo_r_w               = INV_MPU6050_REG_FIFO_R_W,
	.raw_gyro               = INV_MPU6050_REG_RAW_GYRO,
	.raw_accl               = INV_MPU6050_REG_RAW_ACCEL,
	.temperature            = INV_MPU6050_REG_TEMPERATURE,
	.int_enable             = INV_MPU6050_REG_INT_ENABLE,
	.pwr_mgmt_1             = INV_MPU6050_REG_PWR_MGMT_1,
	.pwr_mgmt_2             = INV_MPU6050_REG_PWR_MGMT_2,
	.int_pin_cfg		= INV_MPU6050_REG_INT_PIN_CFG,
	.accl_offset		= INV_MPU6500_REG_ACCEL_OFFSET,
	.gyro_offset		= INV_MPU6050_REG_GYRO_OFFSET,
};

>>>>>>> v4.9.227
static const struct inv_mpu6050_reg_map reg_set_6050 = {
	.sample_rate_div	= INV_MPU6050_REG_SAMPLE_RATE_DIV,
	.lpf                    = INV_MPU6050_REG_CONFIG,
	.user_ctrl              = INV_MPU6050_REG_USER_CTRL,
	.fifo_en                = INV_MPU6050_REG_FIFO_EN,
	.gyro_config            = INV_MPU6050_REG_GYRO_CONFIG,
	.accl_config            = INV_MPU6050_REG_ACCEL_CONFIG,
	.fifo_count_h           = INV_MPU6050_REG_FIFO_COUNT_H,
	.fifo_r_w               = INV_MPU6050_REG_FIFO_R_W,
	.raw_gyro               = INV_MPU6050_REG_RAW_GYRO,
	.raw_accl               = INV_MPU6050_REG_RAW_ACCEL,
	.temperature            = INV_MPU6050_REG_TEMPERATURE,
	.int_enable             = INV_MPU6050_REG_INT_ENABLE,
	.pwr_mgmt_1             = INV_MPU6050_REG_PWR_MGMT_1,
	.pwr_mgmt_2             = INV_MPU6050_REG_PWR_MGMT_2,
<<<<<<< HEAD
=======
	.int_pin_cfg		= INV_MPU6050_REG_INT_PIN_CFG,
	.accl_offset		= INV_MPU6050_REG_ACCEL_OFFSET,
	.gyro_offset		= INV_MPU6050_REG_GYRO_OFFSET,
>>>>>>> v4.9.227
};

static const struct inv_mpu6050_chip_config chip_config_6050 = {
	.fsr = INV_MPU6050_FSR_2000DPS,
	.lpf = INV_MPU6050_FILTER_20HZ,
	.fifo_rate = INV_MPU6050_INIT_FIFO_RATE,
	.gyro_fifo_enable = false,
	.accl_fifo_enable = false,
	.accl_fs = INV_MPU6050_FS_02G,
};

<<<<<<< HEAD
static const struct inv_mpu6050_hw hw_info[INV_NUM_PARTS] = {
	{
		.num_reg = 117,
=======
/* Indexed by enum inv_devices */
static const struct inv_mpu6050_hw hw_info[] = {
	{
		.whoami = INV_MPU6050_WHOAMI_VALUE,
>>>>>>> v4.9.227
		.name = "MPU6050",
		.reg = &reg_set_6050,
		.config = &chip_config_6050,
	},
<<<<<<< HEAD
};

int inv_mpu6050_write_reg(struct inv_mpu6050_state *st, int reg, u8 d)
{
	return i2c_smbus_write_i2c_block_data(st->client, reg, 1, &d);
}

int inv_mpu6050_switch_engine(struct inv_mpu6050_state *st, bool en, u32 mask)
{
	u8 d, mgmt_1;
	int result;

	/* switch clock needs to be careful. Only when gyro is on, can
	   clock source be switched to gyro. Otherwise, it must be set to
	   internal clock */
	if (INV_MPU6050_BIT_PWR_GYRO_STBY == mask) {
		result = i2c_smbus_read_i2c_block_data(st->client,
				       st->reg->pwr_mgmt_1, 1, &mgmt_1);
		if (result != 1)
=======
	{
		.whoami = INV_MPU6500_WHOAMI_VALUE,
		.name = "MPU6500",
		.reg = &reg_set_6500,
		.config = &chip_config_6050,
	},
	{
		.whoami = INV_MPU6000_WHOAMI_VALUE,
		.name = "MPU6000",
		.reg = &reg_set_6050,
		.config = &chip_config_6050,
	},
	{
		.whoami = INV_MPU9150_WHOAMI_VALUE,
		.name = "MPU9150",
		.reg = &reg_set_6050,
		.config = &chip_config_6050,
	},
	{
		.whoami = INV_ICM20608_WHOAMI_VALUE,
		.name = "ICM20608",
		.reg = &reg_set_6500,
		.config = &chip_config_6050,
	},
};

int inv_mpu6050_switch_engine(struct inv_mpu6050_state *st, bool en, u32 mask)
{
	unsigned int d, mgmt_1;
	int result;
	/*
	 * switch clock needs to be careful. Only when gyro is on, can
	 * clock source be switched to gyro. Otherwise, it must be set to
	 * internal clock
	 */
	if (mask == INV_MPU6050_BIT_PWR_GYRO_STBY) {
		result = regmap_read(st->map, st->reg->pwr_mgmt_1, &mgmt_1);
		if (result)
>>>>>>> v4.9.227
			return result;

		mgmt_1 &= ~INV_MPU6050_BIT_CLK_MASK;
	}

<<<<<<< HEAD
	if ((INV_MPU6050_BIT_PWR_GYRO_STBY == mask) && (!en)) {
		/* turning off gyro requires switch to internal clock first.
		   Then turn off gyro engine */
		mgmt_1 |= INV_CLK_INTERNAL;
		result = inv_mpu6050_write_reg(st, st->reg->pwr_mgmt_1, mgmt_1);
=======
	if ((mask == INV_MPU6050_BIT_PWR_GYRO_STBY) && (!en)) {
		/*
		 * turning off gyro requires switch to internal clock first.
		 * Then turn off gyro engine
		 */
		mgmt_1 |= INV_CLK_INTERNAL;
		result = regmap_write(st->map, st->reg->pwr_mgmt_1, mgmt_1);
>>>>>>> v4.9.227
		if (result)
			return result;
	}

<<<<<<< HEAD
	result = i2c_smbus_read_i2c_block_data(st->client,
				       st->reg->pwr_mgmt_2, 1, &d);
	if (result != 1)
=======
	result = regmap_read(st->map, st->reg->pwr_mgmt_2, &d);
	if (result)
>>>>>>> v4.9.227
		return result;
	if (en)
		d &= ~mask;
	else
		d |= mask;
<<<<<<< HEAD
	result = inv_mpu6050_write_reg(st, st->reg->pwr_mgmt_2, d);
=======
	result = regmap_write(st->map, st->reg->pwr_mgmt_2, d);
>>>>>>> v4.9.227
	if (result)
		return result;

	if (en) {
		/* Wait for output stabilize */
		msleep(INV_MPU6050_TEMP_UP_TIME);
<<<<<<< HEAD
		if (INV_MPU6050_BIT_PWR_GYRO_STBY == mask) {
			/* switch internal clock to PLL */
			mgmt_1 |= INV_CLK_PLL;
			result = inv_mpu6050_write_reg(st,
					st->reg->pwr_mgmt_1, mgmt_1);
=======
		if (mask == INV_MPU6050_BIT_PWR_GYRO_STBY) {
			/* switch internal clock to PLL */
			mgmt_1 |= INV_CLK_PLL;
			result = regmap_write(st->map,
					      st->reg->pwr_mgmt_1, mgmt_1);
>>>>>>> v4.9.227
			if (result)
				return result;
		}
	}

	return 0;
}

int inv_mpu6050_set_power_itg(struct inv_mpu6050_state *st, bool power_on)
{
<<<<<<< HEAD
	int result;

	if (power_on)
		result = inv_mpu6050_write_reg(st, st->reg->pwr_mgmt_1, 0);
	else
		result = inv_mpu6050_write_reg(st, st->reg->pwr_mgmt_1,
						INV_MPU6050_BIT_SLEEP);
=======
	int result = 0;

	if (power_on) {
		/* Already under indio-dev->mlock mutex */
		if (!st->powerup_count)
			result = regmap_write(st->map, st->reg->pwr_mgmt_1, 0);
		if (!result)
			st->powerup_count++;
	} else {
		st->powerup_count--;
		if (!st->powerup_count)
			result = regmap_write(st->map, st->reg->pwr_mgmt_1,
					      INV_MPU6050_BIT_SLEEP);
	}

>>>>>>> v4.9.227
	if (result)
		return result;

	if (power_on)
<<<<<<< HEAD
		msleep(INV_MPU6050_REG_UP_TIME);

	return 0;
}
=======
		usleep_range(INV_MPU6050_REG_UP_TIME_MIN,
			     INV_MPU6050_REG_UP_TIME_MAX);

	return 0;
}
EXPORT_SYMBOL_GPL(inv_mpu6050_set_power_itg);

/**
 *  inv_mpu6050_set_lpf_regs() - set low pass filter registers, chip dependent
 *
 *  MPU60xx/MPU9150 use only 1 register for accelerometer + gyroscope
 *  MPU6500 and above have a dedicated register for accelerometer
 */
static int inv_mpu6050_set_lpf_regs(struct inv_mpu6050_state *st,
				    enum inv_mpu6050_filter_e val)
{
	int result;

	result = regmap_write(st->map, st->reg->lpf, val);
	if (result)
		return result;

	switch (st->chip_type) {
	case INV_MPU6050:
	case INV_MPU6000:
	case INV_MPU9150:
		/* old chips, nothing to do */
		result = 0;
		break;
	default:
		/* set accel lpf */
		result = regmap_write(st->map, st->reg->accel_lpf, val);
		break;
	}

	return result;
}
>>>>>>> v4.9.227

/**
 *  inv_mpu6050_init_config() - Initialize hardware, disable FIFO.
 *
 *  Initial configuration:
 *  FSR: ± 2000DPS
 *  DLPF: 20Hz
 *  FIFO rate: 50Hz
 *  Clock source: Gyro PLL
 */
static int inv_mpu6050_init_config(struct iio_dev *indio_dev)
{
	int result;
	u8 d;
	struct inv_mpu6050_state *st = iio_priv(indio_dev);

	result = inv_mpu6050_set_power_itg(st, true);
	if (result)
		return result;
	d = (INV_MPU6050_FSR_2000DPS << INV_MPU6050_GYRO_CONFIG_FSR_SHIFT);
<<<<<<< HEAD
	result = inv_mpu6050_write_reg(st, st->reg->gyro_config, d);
	if (result)
		return result;

	d = INV_MPU6050_FILTER_20HZ;
	result = inv_mpu6050_write_reg(st, st->reg->lpf, d);
=======
	result = regmap_write(st->map, st->reg->gyro_config, d);
	if (result)
		return result;

	result = inv_mpu6050_set_lpf_regs(st, INV_MPU6050_FILTER_20HZ);
>>>>>>> v4.9.227
	if (result)
		return result;

	d = INV_MPU6050_ONE_K_HZ / INV_MPU6050_INIT_FIFO_RATE - 1;
<<<<<<< HEAD
	result = inv_mpu6050_write_reg(st, st->reg->sample_rate_div, d);
=======
	result = regmap_write(st->map, st->reg->sample_rate_div, d);
>>>>>>> v4.9.227
	if (result)
		return result;

	d = (INV_MPU6050_FS_02G << INV_MPU6050_ACCL_CONFIG_FSR_SHIFT);
<<<<<<< HEAD
	result = inv_mpu6050_write_reg(st, st->reg->accl_config, d);
=======
	result = regmap_write(st->map, st->reg->accl_config, d);
>>>>>>> v4.9.227
	if (result)
		return result;

	memcpy(&st->chip_config, hw_info[st->chip_type].config,
<<<<<<< HEAD
		sizeof(struct inv_mpu6050_chip_config));
=======
	       sizeof(struct inv_mpu6050_chip_config));
>>>>>>> v4.9.227
	result = inv_mpu6050_set_power_itg(st, false);

	return result;
}

<<<<<<< HEAD
static int inv_mpu6050_sensor_show(struct inv_mpu6050_state  *st, int reg,
				int axis, int *val)
=======
static int inv_mpu6050_sensor_set(struct inv_mpu6050_state  *st, int reg,
				int axis, int val)
{
	int ind, result;
	__be16 d = cpu_to_be16(val);

	ind = (axis - IIO_MOD_X) * 2;
	result = regmap_bulk_write(st->map, reg + ind, (u8 *)&d, 2);
	if (result)
		return -EINVAL;

	return 0;
}

static int inv_mpu6050_sensor_show(struct inv_mpu6050_state  *st, int reg,
				   int axis, int *val)
>>>>>>> v4.9.227
{
	int ind, result;
	__be16 d;

	ind = (axis - IIO_MOD_X) * 2;
<<<<<<< HEAD
	result = i2c_smbus_read_i2c_block_data(st->client, reg + ind,  2,
						(u8 *)&d);
	if (result != 2)
=======
	result = regmap_bulk_read(st->map, reg + ind, (u8 *)&d, 2);
	if (result)
>>>>>>> v4.9.227
		return -EINVAL;
	*val = (short)be16_to_cpup(&d);

	return IIO_VAL_INT;
}

<<<<<<< HEAD
static int inv_mpu6050_read_raw(struct iio_dev *indio_dev,
			      struct iio_chan_spec const *chan,
			      int *val,
			      int *val2,
			      long mask) {
	struct inv_mpu6050_state  *st = iio_priv(indio_dev);
=======
static int
inv_mpu6050_read_raw(struct iio_dev *indio_dev,
		     struct iio_chan_spec const *chan,
		     int *val, int *val2, long mask)
{
	struct inv_mpu6050_state  *st = iio_priv(indio_dev);
	int ret = 0;
>>>>>>> v4.9.227

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
	{
<<<<<<< HEAD
		int ret, result;
=======
		int result;
>>>>>>> v4.9.227

		ret = IIO_VAL_INT;
		result = 0;
		mutex_lock(&indio_dev->mlock);
		if (!st->chip_config.enable) {
			result = inv_mpu6050_set_power_itg(st, true);
			if (result)
				goto error_read_raw;
		}
		/* when enable is on, power is already on */
		switch (chan->type) {
		case IIO_ANGL_VEL:
			if (!st->chip_config.gyro_fifo_enable ||
<<<<<<< HEAD
					!st->chip_config.enable) {
=======
			    !st->chip_config.enable) {
>>>>>>> v4.9.227
				result = inv_mpu6050_switch_engine(st, true,
						INV_MPU6050_BIT_PWR_GYRO_STBY);
				if (result)
					goto error_read_raw;
			}
<<<<<<< HEAD
			ret =  inv_mpu6050_sensor_show(st, st->reg->raw_gyro,
						chan->channel2, val);
			if (!st->chip_config.gyro_fifo_enable ||
					!st->chip_config.enable) {
=======
			ret = inv_mpu6050_sensor_show(st, st->reg->raw_gyro,
						      chan->channel2, val);
			if (!st->chip_config.gyro_fifo_enable ||
			    !st->chip_config.enable) {
>>>>>>> v4.9.227
				result = inv_mpu6050_switch_engine(st, false,
						INV_MPU6050_BIT_PWR_GYRO_STBY);
				if (result)
					goto error_read_raw;
			}
			break;
		case IIO_ACCEL:
			if (!st->chip_config.accl_fifo_enable ||
<<<<<<< HEAD
					!st->chip_config.enable) {
=======
			    !st->chip_config.enable) {
>>>>>>> v4.9.227
				result = inv_mpu6050_switch_engine(st, true,
						INV_MPU6050_BIT_PWR_ACCL_STBY);
				if (result)
					goto error_read_raw;
			}
			ret = inv_mpu6050_sensor_show(st, st->reg->raw_accl,
<<<<<<< HEAD
						chan->channel2, val);
			if (!st->chip_config.accl_fifo_enable ||
					!st->chip_config.enable) {
=======
						      chan->channel2, val);
			if (!st->chip_config.accl_fifo_enable ||
			    !st->chip_config.enable) {
>>>>>>> v4.9.227
				result = inv_mpu6050_switch_engine(st, false,
						INV_MPU6050_BIT_PWR_ACCL_STBY);
				if (result)
					goto error_read_raw;
			}
			break;
		case IIO_TEMP:
			/* wait for stablization */
			msleep(INV_MPU6050_SENSOR_UP_TIME);
<<<<<<< HEAD
			inv_mpu6050_sensor_show(st, st->reg->temperature,
							IIO_MOD_X, val);
=======
			ret = inv_mpu6050_sensor_show(st, st->reg->temperature,
						IIO_MOD_X, val);
>>>>>>> v4.9.227
			break;
		default:
			ret = -EINVAL;
			break;
		}
error_read_raw:
		if (!st->chip_config.enable)
			result |= inv_mpu6050_set_power_itg(st, false);
		mutex_unlock(&indio_dev->mlock);
		if (result)
			return result;

		return ret;
	}
	case IIO_CHAN_INFO_SCALE:
		switch (chan->type) {
		case IIO_ANGL_VEL:
			*val  = 0;
			*val2 = gyro_scale_6050[st->chip_config.fsr];

			return IIO_VAL_INT_PLUS_NANO;
		case IIO_ACCEL:
			*val = 0;
			*val2 = accel_scale[st->chip_config.accl_fs];

			return IIO_VAL_INT_PLUS_MICRO;
		case IIO_TEMP:
			*val = 0;
			*val2 = INV_MPU6050_TEMP_SCALE;

			return IIO_VAL_INT_PLUS_MICRO;
		default:
			return -EINVAL;
		}
	case IIO_CHAN_INFO_OFFSET:
		switch (chan->type) {
		case IIO_TEMP:
			*val = INV_MPU6050_TEMP_OFFSET;

			return IIO_VAL_INT;
		default:
			return -EINVAL;
		}
<<<<<<< HEAD
=======
	case IIO_CHAN_INFO_CALIBBIAS:
		switch (chan->type) {
		case IIO_ANGL_VEL:
			ret = inv_mpu6050_sensor_show(st, st->reg->gyro_offset,
						chan->channel2, val);
			return IIO_VAL_INT;
		case IIO_ACCEL:
			ret = inv_mpu6050_sensor_show(st, st->reg->accl_offset,
						chan->channel2, val);
			return IIO_VAL_INT;

		default:
			return -EINVAL;
		}
>>>>>>> v4.9.227
	default:
		return -EINVAL;
	}
}

<<<<<<< HEAD
static int inv_mpu6050_write_fsr(struct inv_mpu6050_state *st, int fsr)
{
	int result;
	u8 d;

	if (fsr < 0 || fsr > INV_MPU6050_MAX_GYRO_FS_PARAM)
		return -EINVAL;
	if (fsr == st->chip_config.fsr)
		return 0;

	d = (fsr << INV_MPU6050_GYRO_CONFIG_FSR_SHIFT);
	result = inv_mpu6050_write_reg(st, st->reg->gyro_config, d);
	if (result)
		return result;
	st->chip_config.fsr = fsr;

	return 0;
}

static int inv_mpu6050_write_accel_fs(struct inv_mpu6050_state *st, int fs)
{
	int result;
	u8 d;

	if (fs < 0 || fs > INV_MPU6050_MAX_ACCL_FS_PARAM)
		return -EINVAL;
	if (fs == st->chip_config.accl_fs)
		return 0;

	d = (fs << INV_MPU6050_ACCL_CONFIG_FSR_SHIFT);
	result = inv_mpu6050_write_reg(st, st->reg->accl_config, d);
	if (result)
		return result;
	st->chip_config.accl_fs = fs;

	return 0;
}

static int inv_mpu6050_write_raw(struct iio_dev *indio_dev,
			       struct iio_chan_spec const *chan,
			       int val,
			       int val2,
			       long mask) {
=======
static int inv_mpu6050_write_gyro_scale(struct inv_mpu6050_state *st, int val)
{
	int result, i;
	u8 d;

	for (i = 0; i < ARRAY_SIZE(gyro_scale_6050); ++i) {
		if (gyro_scale_6050[i] == val) {
			d = (i << INV_MPU6050_GYRO_CONFIG_FSR_SHIFT);
			result = regmap_write(st->map, st->reg->gyro_config, d);
			if (result)
				return result;

			st->chip_config.fsr = i;
			return 0;
		}
	}

	return -EINVAL;
}

static int inv_write_raw_get_fmt(struct iio_dev *indio_dev,
				 struct iio_chan_spec const *chan, long mask)
{
	switch (mask) {
	case IIO_CHAN_INFO_SCALE:
		switch (chan->type) {
		case IIO_ANGL_VEL:
			return IIO_VAL_INT_PLUS_NANO;
		default:
			return IIO_VAL_INT_PLUS_MICRO;
		}
	default:
		return IIO_VAL_INT_PLUS_MICRO;
	}

	return -EINVAL;
}

static int inv_mpu6050_write_accel_scale(struct inv_mpu6050_state *st, int val)
{
	int result, i;
	u8 d;

	for (i = 0; i < ARRAY_SIZE(accel_scale); ++i) {
		if (accel_scale[i] == val) {
			d = (i << INV_MPU6050_ACCL_CONFIG_FSR_SHIFT);
			result = regmap_write(st->map, st->reg->accl_config, d);
			if (result)
				return result;

			st->chip_config.accl_fs = i;
			return 0;
		}
	}

	return -EINVAL;
}

static int inv_mpu6050_write_raw(struct iio_dev *indio_dev,
				 struct iio_chan_spec const *chan,
				 int val, int val2, long mask)
{
>>>>>>> v4.9.227
	struct inv_mpu6050_state  *st = iio_priv(indio_dev);
	int result;

	mutex_lock(&indio_dev->mlock);
<<<<<<< HEAD
	/* we should only update scale when the chip is disabled, i.e.,
		not running */
=======
	/*
	 * we should only update scale when the chip is disabled, i.e.
	 * not running
	 */
>>>>>>> v4.9.227
	if (st->chip_config.enable) {
		result = -EBUSY;
		goto error_write_raw;
	}
	result = inv_mpu6050_set_power_itg(st, true);
	if (result)
		goto error_write_raw;

	switch (mask) {
	case IIO_CHAN_INFO_SCALE:
		switch (chan->type) {
		case IIO_ANGL_VEL:
<<<<<<< HEAD
			result = inv_mpu6050_write_fsr(st, val);
			break;
		case IIO_ACCEL:
			result = inv_mpu6050_write_accel_fs(st, val);
=======
			result = inv_mpu6050_write_gyro_scale(st, val2);
			break;
		case IIO_ACCEL:
			result = inv_mpu6050_write_accel_scale(st, val2);
>>>>>>> v4.9.227
			break;
		default:
			result = -EINVAL;
			break;
		}
		break;
<<<<<<< HEAD
=======
	case IIO_CHAN_INFO_CALIBBIAS:
		switch (chan->type) {
		case IIO_ANGL_VEL:
			result = inv_mpu6050_sensor_set(st,
							st->reg->gyro_offset,
							chan->channel2, val);
			break;
		case IIO_ACCEL:
			result = inv_mpu6050_sensor_set(st,
							st->reg->accl_offset,
							chan->channel2, val);
			break;
		default:
			result = -EINVAL;
		}
>>>>>>> v4.9.227
	default:
		result = -EINVAL;
		break;
	}

error_write_raw:
	result |= inv_mpu6050_set_power_itg(st, false);
	mutex_unlock(&indio_dev->mlock);

	return result;
}

/**
 *  inv_mpu6050_set_lpf() - set low pass filer based on fifo rate.
 *
 *                  Based on the Nyquist principle, the sampling rate must
 *                  exceed twice of the bandwidth of the signal, or there
 *                  would be alising. This function basically search for the
 *                  correct low pass parameters based on the fifo rate, e.g,
 *                  sampling frequency.
<<<<<<< HEAD
=======
 *
 *  lpf is set automatically when setting sampling rate to avoid any aliases.
>>>>>>> v4.9.227
 */
static int inv_mpu6050_set_lpf(struct inv_mpu6050_state *st, int rate)
{
	const int hz[] = {188, 98, 42, 20, 10, 5};
	const int d[] = {INV_MPU6050_FILTER_188HZ, INV_MPU6050_FILTER_98HZ,
			INV_MPU6050_FILTER_42HZ, INV_MPU6050_FILTER_20HZ,
			INV_MPU6050_FILTER_10HZ, INV_MPU6050_FILTER_5HZ};
	int i, h, result;
	u8 data;

	h = (rate >> 1);
	i = 0;
	while ((h < hz[i]) && (i < ARRAY_SIZE(d) - 1))
		i++;
	data = d[i];
<<<<<<< HEAD
	result = inv_mpu6050_write_reg(st, st->reg->lpf, data);
=======
	result = inv_mpu6050_set_lpf_regs(st, data);
>>>>>>> v4.9.227
	if (result)
		return result;
	st->chip_config.lpf = data;

	return 0;
}

/**
 * inv_mpu6050_fifo_rate_store() - Set fifo rate.
 */
<<<<<<< HEAD
static ssize_t inv_mpu6050_fifo_rate_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
=======
static ssize_t
inv_mpu6050_fifo_rate_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
>>>>>>> v4.9.227
{
	s32 fifo_rate;
	u8 d;
	int result;
	struct iio_dev *indio_dev = dev_to_iio_dev(dev);
	struct inv_mpu6050_state *st = iio_priv(indio_dev);

	if (kstrtoint(buf, 10, &fifo_rate))
		return -EINVAL;
	if (fifo_rate < INV_MPU6050_MIN_FIFO_RATE ||
<<<<<<< HEAD
				fifo_rate > INV_MPU6050_MAX_FIFO_RATE)
=======
	    fifo_rate > INV_MPU6050_MAX_FIFO_RATE)
>>>>>>> v4.9.227
		return -EINVAL;
	if (fifo_rate == st->chip_config.fifo_rate)
		return count;

	mutex_lock(&indio_dev->mlock);
	if (st->chip_config.enable) {
		result = -EBUSY;
		goto fifo_rate_fail;
	}
	result = inv_mpu6050_set_power_itg(st, true);
	if (result)
		goto fifo_rate_fail;

	d = INV_MPU6050_ONE_K_HZ / fifo_rate - 1;
<<<<<<< HEAD
	result = inv_mpu6050_write_reg(st, st->reg->sample_rate_div, d);
=======
	result = regmap_write(st->map, st->reg->sample_rate_div, d);
>>>>>>> v4.9.227
	if (result)
		goto fifo_rate_fail;
	st->chip_config.fifo_rate = fifo_rate;

	result = inv_mpu6050_set_lpf(st, fifo_rate);
	if (result)
		goto fifo_rate_fail;

fifo_rate_fail:
	result |= inv_mpu6050_set_power_itg(st, false);
	mutex_unlock(&indio_dev->mlock);
	if (result)
		return result;

	return count;
}

/**
 * inv_fifo_rate_show() - Get the current sampling rate.
 */
<<<<<<< HEAD
static ssize_t inv_fifo_rate_show(struct device *dev,
	struct device_attribute *attr, char *buf)
=======
static ssize_t
inv_fifo_rate_show(struct device *dev, struct device_attribute *attr,
		   char *buf)
>>>>>>> v4.9.227
{
	struct inv_mpu6050_state *st = iio_priv(dev_to_iio_dev(dev));

	return sprintf(buf, "%d\n", st->chip_config.fifo_rate);
}

/**
 * inv_attr_show() - calling this function will show current
 *                    parameters.
<<<<<<< HEAD
 */
static ssize_t inv_attr_show(struct device *dev,
	struct device_attribute *attr, char *buf)
=======
 *
 * Deprecated in favor of IIO mounting matrix API.
 *
 * See inv_get_mount_matrix()
 */
static ssize_t inv_attr_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
>>>>>>> v4.9.227
{
	struct inv_mpu6050_state *st = iio_priv(dev_to_iio_dev(dev));
	struct iio_dev_attr *this_attr = to_iio_dev_attr(attr);
	s8 *m;

	switch (this_attr->address) {
<<<<<<< HEAD
	/* In MPU6050, the two matrix are the same because gyro and accel
	   are integrated in one chip */
=======
	/*
	 * In MPU6050, the two matrix are the same because gyro and accel
	 * are integrated in one chip
	 */
>>>>>>> v4.9.227
	case ATTR_GYRO_MATRIX:
	case ATTR_ACCL_MATRIX:
		m = st->plat_data.orientation;

		return sprintf(buf, "%d, %d, %d; %d, %d, %d; %d, %d, %d\n",
			m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8]);
	default:
		return -EINVAL;
	}
}

/**
 * inv_mpu6050_validate_trigger() - validate_trigger callback for invensense
 *                                  MPU6050 device.
 * @indio_dev: The IIO device
 * @trig: The new trigger
 *
 * Returns: 0 if the 'trig' matches the trigger registered by the MPU6050
 * device, -EINVAL otherwise.
 */
static int inv_mpu6050_validate_trigger(struct iio_dev *indio_dev,
					struct iio_trigger *trig)
{
	struct inv_mpu6050_state *st = iio_priv(indio_dev);

	if (st->trig != trig)
		return -EINVAL;

	return 0;
}

<<<<<<< HEAD
=======
static const struct iio_mount_matrix *
inv_get_mount_matrix(const struct iio_dev *indio_dev,
		     const struct iio_chan_spec *chan)
{
	return &((struct inv_mpu6050_state *)iio_priv(indio_dev))->orientation;
}

static const struct iio_chan_spec_ext_info inv_ext_info[] = {
	IIO_MOUNT_MATRIX(IIO_SHARED_BY_TYPE, inv_get_mount_matrix),
	{ },
};

>>>>>>> v4.9.227
#define INV_MPU6050_CHAN(_type, _channel2, _index)                    \
	{                                                             \
		.type = _type,                                        \
		.modified = 1,                                        \
		.channel2 = _channel2,                                \
<<<<<<< HEAD
		.info_mask_shared_by_type =  BIT(IIO_CHAN_INFO_SCALE), \
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),         \
=======
		.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE), \
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |	      \
				      BIT(IIO_CHAN_INFO_CALIBBIAS),   \
>>>>>>> v4.9.227
		.scan_index = _index,                                 \
		.scan_type = {                                        \
				.sign = 's',                          \
				.realbits = 16,                       \
				.storagebits = 16,                    \
<<<<<<< HEAD
				.shift = 0 ,                          \
				.endianness = IIO_BE,                 \
			     },                                       \
=======
				.shift = 0,                           \
				.endianness = IIO_BE,                 \
			     },                                       \
		.ext_info = inv_ext_info,                             \
>>>>>>> v4.9.227
	}

static const struct iio_chan_spec inv_mpu_channels[] = {
	IIO_CHAN_SOFT_TIMESTAMP(INV_MPU6050_SCAN_TIMESTAMP),
	/*
	 * Note that temperature should only be via polled reading only,
	 * not the final scan elements output.
	 */
	{
		.type = IIO_TEMP,
<<<<<<< HEAD
		.info_mask_separate =  BIT(IIO_CHAN_INFO_RAW)
=======
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW)
>>>>>>> v4.9.227
				| BIT(IIO_CHAN_INFO_OFFSET)
				| BIT(IIO_CHAN_INFO_SCALE),
		.scan_index = -1,
	},
	INV_MPU6050_CHAN(IIO_ANGL_VEL, IIO_MOD_X, INV_MPU6050_SCAN_GYRO_X),
	INV_MPU6050_CHAN(IIO_ANGL_VEL, IIO_MOD_Y, INV_MPU6050_SCAN_GYRO_Y),
	INV_MPU6050_CHAN(IIO_ANGL_VEL, IIO_MOD_Z, INV_MPU6050_SCAN_GYRO_Z),

	INV_MPU6050_CHAN(IIO_ACCEL, IIO_MOD_X, INV_MPU6050_SCAN_ACCL_X),
	INV_MPU6050_CHAN(IIO_ACCEL, IIO_MOD_Y, INV_MPU6050_SCAN_ACCL_Y),
	INV_MPU6050_CHAN(IIO_ACCEL, IIO_MOD_Z, INV_MPU6050_SCAN_ACCL_Z),
};

/* constant IIO attribute */
static IIO_CONST_ATTR_SAMP_FREQ_AVAIL("10 20 50 100 200 500");
<<<<<<< HEAD
static IIO_DEV_ATTR_SAMP_FREQ(S_IRUGO | S_IWUSR, inv_fifo_rate_show,
	inv_mpu6050_fifo_rate_store);
=======
static IIO_CONST_ATTR(in_anglvel_scale_available,
					  "0.000133090 0.000266181 0.000532362 0.001064724");
static IIO_CONST_ATTR(in_accel_scale_available,
					  "0.000598 0.001196 0.002392 0.004785");
static IIO_DEV_ATTR_SAMP_FREQ(S_IRUGO | S_IWUSR, inv_fifo_rate_show,
	inv_mpu6050_fifo_rate_store);

/* Deprecated: kept for userspace backward compatibility. */
>>>>>>> v4.9.227
static IIO_DEVICE_ATTR(in_gyro_matrix, S_IRUGO, inv_attr_show, NULL,
	ATTR_GYRO_MATRIX);
static IIO_DEVICE_ATTR(in_accel_matrix, S_IRUGO, inv_attr_show, NULL,
	ATTR_ACCL_MATRIX);

static struct attribute *inv_attributes[] = {
<<<<<<< HEAD
	&iio_dev_attr_in_gyro_matrix.dev_attr.attr,
	&iio_dev_attr_in_accel_matrix.dev_attr.attr,
	&iio_dev_attr_sampling_frequency.dev_attr.attr,
	&iio_const_attr_sampling_frequency_available.dev_attr.attr,
=======
	&iio_dev_attr_in_gyro_matrix.dev_attr.attr,  /* deprecated */
	&iio_dev_attr_in_accel_matrix.dev_attr.attr, /* deprecated */
	&iio_dev_attr_sampling_frequency.dev_attr.attr,
	&iio_const_attr_sampling_frequency_available.dev_attr.attr,
	&iio_const_attr_in_accel_scale_available.dev_attr.attr,
	&iio_const_attr_in_anglvel_scale_available.dev_attr.attr,
>>>>>>> v4.9.227
	NULL,
};

static const struct attribute_group inv_attribute_group = {
	.attrs = inv_attributes
};

static const struct iio_info mpu_info = {
	.driver_module = THIS_MODULE,
	.read_raw = &inv_mpu6050_read_raw,
	.write_raw = &inv_mpu6050_write_raw,
<<<<<<< HEAD
=======
	.write_raw_get_fmt = &inv_write_raw_get_fmt,
>>>>>>> v4.9.227
	.attrs = &inv_attribute_group,
	.validate_trigger = inv_mpu6050_validate_trigger,
};

/**
 *  inv_check_and_setup_chip() - check and setup chip.
 */
<<<<<<< HEAD
static int inv_check_and_setup_chip(struct inv_mpu6050_state *st,
		const struct i2c_device_id *id)
{
	int result;

	st->chip_type = INV_MPU6050;
=======
static int inv_check_and_setup_chip(struct inv_mpu6050_state *st)
{
	int result;
	unsigned int regval;

>>>>>>> v4.9.227
	st->hw  = &hw_info[st->chip_type];
	st->reg = hw_info[st->chip_type].reg;

	/* reset to make sure previous state are not there */
<<<<<<< HEAD
	result = inv_mpu6050_write_reg(st, st->reg->pwr_mgmt_1,
					INV_MPU6050_BIT_H_RESET);
	if (result)
		return result;
	msleep(INV_MPU6050_POWER_UP_TIME);
	/* toggle power state. After reset, the sleep bit could be on
		or off depending on the OTP settings. Toggling power would
		make it in a definite state as well as making the hardware
		state align with the software state */
=======
	result = regmap_write(st->map, st->reg->pwr_mgmt_1,
			      INV_MPU6050_BIT_H_RESET);
	if (result)
		return result;
	msleep(INV_MPU6050_POWER_UP_TIME);

	/* check chip self-identification */
	result = regmap_read(st->map, INV_MPU6050_REG_WHOAMI, &regval);
	if (result)
		return result;
	if (regval != st->hw->whoami) {
		dev_warn(regmap_get_device(st->map),
				"whoami mismatch got %#02x expected %#02hhx for %s\n",
				regval, st->hw->whoami, st->hw->name);
	}

	/*
	 * toggle power state. After reset, the sleep bit could be on
	 * or off depending on the OTP settings. Toggling power would
	 * make it in a definite state as well as making the hardware
	 * state align with the software state
	 */
>>>>>>> v4.9.227
	result = inv_mpu6050_set_power_itg(st, false);
	if (result)
		return result;
	result = inv_mpu6050_set_power_itg(st, true);
	if (result)
		return result;

	result = inv_mpu6050_switch_engine(st, false,
<<<<<<< HEAD
					INV_MPU6050_BIT_PWR_ACCL_STBY);
	if (result)
		return result;
	result = inv_mpu6050_switch_engine(st, false,
					INV_MPU6050_BIT_PWR_GYRO_STBY);
=======
					   INV_MPU6050_BIT_PWR_ACCL_STBY);
	if (result)
		return result;
	result = inv_mpu6050_switch_engine(st, false,
					   INV_MPU6050_BIT_PWR_GYRO_STBY);
>>>>>>> v4.9.227
	if (result)
		return result;

	return 0;
}

<<<<<<< HEAD
/**
 *  inv_mpu_probe() - probe function.
 *  @client:          i2c client.
 *  @id:              i2c device id.
 *
 *  Returns 0 on success, a negative error code otherwise.
 */
static int inv_mpu_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
=======
int inv_mpu_core_probe(struct regmap *regmap, int irq, const char *name,
		int (*inv_mpu_bus_setup)(struct iio_dev *), int chip_type)
>>>>>>> v4.9.227
{
	struct inv_mpu6050_state *st;
	struct iio_dev *indio_dev;
	struct inv_mpu6050_platform_data *pdata;
<<<<<<< HEAD
	int result;

	if (!i2c_check_functionality(client->adapter,
		I2C_FUNC_SMBUS_I2C_BLOCK))
		return -ENOSYS;

	indio_dev = devm_iio_device_alloc(&client->dev, sizeof(*st));
	if (!indio_dev)
		return -ENOMEM;

	st = iio_priv(indio_dev);
	st->client = client;
	pdata = dev_get_platdata(&client->dev);
	if (pdata)
		st->plat_data = *pdata;
	/* power is turned on inside check chip type*/
	result = inv_check_and_setup_chip(st, id);
	if (result)
		return result;

	result = inv_mpu6050_init_config(indio_dev);
	if (result) {
		dev_err(&client->dev,
			"Could not initialize device.\n");
		return result;
	}

	i2c_set_clientdata(client, indio_dev);
	indio_dev->dev.parent = &client->dev;
	indio_dev->name = id->name;
=======
	struct device *dev = regmap_get_device(regmap);
	int result;

	indio_dev = devm_iio_device_alloc(dev, sizeof(*st));
	if (!indio_dev)
		return -ENOMEM;

	BUILD_BUG_ON(ARRAY_SIZE(hw_info) != INV_NUM_PARTS);
	if (chip_type < 0 || chip_type >= INV_NUM_PARTS) {
		dev_err(dev, "Bad invensense chip_type=%d name=%s\n",
				chip_type, name);
		return -ENODEV;
	}
	st = iio_priv(indio_dev);
	st->chip_type = chip_type;
	st->powerup_count = 0;
	st->irq = irq;
	st->map = regmap;

	pdata = dev_get_platdata(dev);
	if (!pdata) {
		result = of_iio_read_mount_matrix(dev, "mount-matrix",
						  &st->orientation);
		if (result) {
			dev_err(dev, "Failed to retrieve mounting matrix %d\n",
				result);
			return result;
		}
	} else {
		st->plat_data = *pdata;
	}

	/* power is turned on inside check chip type*/
	result = inv_check_and_setup_chip(st);
	if (result)
		return result;

	if (inv_mpu_bus_setup)
		inv_mpu_bus_setup(indio_dev);

	result = inv_mpu6050_init_config(indio_dev);
	if (result) {
		dev_err(dev, "Could not initialize device.\n");
		return result;
	}

	dev_set_drvdata(dev, indio_dev);
	indio_dev->dev.parent = dev;
	/* name will be NULL when enumerated via ACPI */
	if (name)
		indio_dev->name = name;
	else
		indio_dev->name = dev_name(dev);
>>>>>>> v4.9.227
	indio_dev->channels = inv_mpu_channels;
	indio_dev->num_channels = ARRAY_SIZE(inv_mpu_channels);

	indio_dev->info = &mpu_info;
	indio_dev->modes = INDIO_BUFFER_TRIGGERED;

	result = iio_triggered_buffer_setup(indio_dev,
					    inv_mpu6050_irq_handler,
					    inv_mpu6050_read_fifo,
					    NULL);
	if (result) {
<<<<<<< HEAD
		dev_err(&st->client->dev, "configure buffer fail %d\n",
				result);
=======
		dev_err(dev, "configure buffer fail %d\n", result);
>>>>>>> v4.9.227
		return result;
	}
	result = inv_mpu6050_probe_trigger(indio_dev);
	if (result) {
<<<<<<< HEAD
		dev_err(&st->client->dev, "trigger probe fail %d\n", result);
=======
		dev_err(dev, "trigger probe fail %d\n", result);
>>>>>>> v4.9.227
		goto out_unreg_ring;
	}

	INIT_KFIFO(st->timestamps);
	spin_lock_init(&st->time_stamp_lock);
	result = iio_device_register(indio_dev);
	if (result) {
<<<<<<< HEAD
		dev_err(&st->client->dev, "IIO register fail %d\n", result);
=======
		dev_err(dev, "IIO register fail %d\n", result);
>>>>>>> v4.9.227
		goto out_remove_trigger;
	}

	return 0;

out_remove_trigger:
	inv_mpu6050_remove_trigger(st);
out_unreg_ring:
	iio_triggered_buffer_cleanup(indio_dev);
	return result;
}
<<<<<<< HEAD

static int inv_mpu_remove(struct i2c_client *client)
{
	struct iio_dev *indio_dev = i2c_get_clientdata(client);
	struct inv_mpu6050_state *st = iio_priv(indio_dev);

	iio_device_unregister(indio_dev);
	inv_mpu6050_remove_trigger(st);
=======
EXPORT_SYMBOL_GPL(inv_mpu_core_probe);

int inv_mpu_core_remove(struct device  *dev)
{
	struct iio_dev *indio_dev = dev_get_drvdata(dev);

	iio_device_unregister(indio_dev);
	inv_mpu6050_remove_trigger(iio_priv(indio_dev));
>>>>>>> v4.9.227
	iio_triggered_buffer_cleanup(indio_dev);

	return 0;
}
<<<<<<< HEAD
=======
EXPORT_SYMBOL_GPL(inv_mpu_core_remove);

>>>>>>> v4.9.227
#ifdef CONFIG_PM_SLEEP

static int inv_mpu_resume(struct device *dev)
{
<<<<<<< HEAD
	return inv_mpu6050_set_power_itg(
		iio_priv(i2c_get_clientdata(to_i2c_client(dev))), true);
=======
	return inv_mpu6050_set_power_itg(iio_priv(dev_get_drvdata(dev)), true);
>>>>>>> v4.9.227
}

static int inv_mpu_suspend(struct device *dev)
{
<<<<<<< HEAD
	return inv_mpu6050_set_power_itg(
		iio_priv(i2c_get_clientdata(to_i2c_client(dev))), false);
}
static SIMPLE_DEV_PM_OPS(inv_mpu_pmops, inv_mpu_suspend, inv_mpu_resume);

#define INV_MPU6050_PMOPS (&inv_mpu_pmops)
#else
#define INV_MPU6050_PMOPS NULL
#endif /* CONFIG_PM_SLEEP */

/*
 * device id table is used to identify what device can be
 * supported by this driver
 */
static const struct i2c_device_id inv_mpu_id[] = {
	{"mpu6050", INV_MPU6050},
	{"mpu6500", INV_MPU6500},
	{}
};

MODULE_DEVICE_TABLE(i2c, inv_mpu_id);

static struct i2c_driver inv_mpu_driver = {
	.probe		=	inv_mpu_probe,
	.remove		=	inv_mpu_remove,
	.id_table	=	inv_mpu_id,
	.driver = {
		.owner	=	THIS_MODULE,
		.name	=	"inv-mpu6050",
		.pm     =       INV_MPU6050_PMOPS,
	},
};

module_i2c_driver(inv_mpu_driver);
=======
	return inv_mpu6050_set_power_itg(iio_priv(dev_get_drvdata(dev)), false);
}
#endif /* CONFIG_PM_SLEEP */

SIMPLE_DEV_PM_OPS(inv_mpu_pmops, inv_mpu_suspend, inv_mpu_resume);
EXPORT_SYMBOL_GPL(inv_mpu_pmops);
>>>>>>> v4.9.227

MODULE_AUTHOR("Invensense Corporation");
MODULE_DESCRIPTION("Invensense device MPU6050 driver");
MODULE_LICENSE("GPL");
