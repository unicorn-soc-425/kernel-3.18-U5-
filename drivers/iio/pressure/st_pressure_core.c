/*
 * STMicroelectronics pressures driver
 *
 * Copyright 2013 STMicroelectronics Inc.
 *
 * Denis Ciocca <denis.ciocca@st.com>
 *
 * Licensed under the GPL-2.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/trigger.h>
#include <linux/iio/buffer.h>
#include <asm/unaligned.h>

#include <linux/iio/common/st_sensors.h>
#include "st_pressure.h"

<<<<<<< HEAD
#define ST_PRESS_LSB_PER_MBAR			4096UL
#define ST_PRESS_KPASCAL_NANO_SCALE		(100000000UL / \
						 ST_PRESS_LSB_PER_MBAR)
#define ST_PRESS_LSB_PER_CELSIUS		480UL
#define ST_PRESS_CELSIUS_NANO_SCALE		(1000000000UL / \
						 ST_PRESS_LSB_PER_CELSIUS)
#define ST_PRESS_NUMBER_DATA_CHANNELS		1

/* FULLSCALE */
=======
/*
 * About determining pressure scaling factors
 * ------------------------------------------
 *
 * Datasheets specify typical pressure sensitivity so that pressure is computed
 * according to the following equation :
 *     pressure[mBar] = raw / sensitivity
 * where :
 *     raw          the 24 bits long raw sampled pressure
 *     sensitivity  a scaling factor specified by the datasheet in LSB/mBar
 *
 * IIO ABI expects pressure to be expressed as kPascal, hence pressure should be
 * computed according to :
 *     pressure[kPascal] = pressure[mBar] / 10
 *                       = raw / (sensitivity * 10)                          (1)
 *
 * Finally, st_press_read_raw() returns pressure scaling factor as an
 * IIO_VAL_INT_PLUS_NANO with a zero integral part and "gain" as decimal part.
 * Therefore, from (1), "gain" becomes :
 *     gain = 10^9 / (sensitivity * 10)
 *          = 10^8 / sensitivity
 *
 * About determining temperature scaling factors and offsets
 * ---------------------------------------------------------
 *
 * Datasheets specify typical temperature sensitivity and offset so that
 * temperature is computed according to the following equation :
 *     temp[Celsius] = offset[Celsius] + (raw / sensitivity)
 * where :
 *     raw          the 16 bits long raw sampled temperature
 *     offset       a constant specified by the datasheet in degree Celsius
 *                  (sometimes zero)
 *     sensitivity  a scaling factor specified by the datasheet in LSB/Celsius
 *
 * IIO ABI expects temperature to be expressed as milli degree Celsius such as
 * user space should compute temperature according to :
 *     temp[mCelsius] = temp[Celsius] * 10^3
 *                    = (offset[Celsius] + (raw / sensitivity)) * 10^3
 *                    = ((offset[Celsius] * sensitivity) + raw) *
 *                      (10^3 / sensitivity)                                 (2)
 *
 * IIO ABI expects user space to apply offset and scaling factors to raw samples
 * according to :
 *     temp[mCelsius] = (OFFSET + raw) * SCALE
 * where :
 *     OFFSET an arbitrary constant exposed by device
 *     SCALE  an arbitrary scaling factor exposed by device
 *
 * Matching OFFSET and SCALE with members of (2) gives :
 *     OFFSET = offset[Celsius] * sensitivity                                (3)
 *     SCALE  = 10^3 / sensitivity                                           (4)
 *
 * st_press_read_raw() returns temperature scaling factor as an
 * IIO_VAL_FRACTIONAL with a 10^3 numerator and "gain2" as denominator.
 * Therefore, from (3), "gain2" becomes :
 *     gain2 = sensitivity
 *
 * When declared within channel, i.e. for a non zero specified offset,
 * st_press_read_raw() will return the latter as an IIO_VAL_FRACTIONAL such as :
 *     numerator = OFFSET * 10^3
 *     denominator = 10^3
 * giving from (4):
 *     numerator = offset[Celsius] * 10^3 * sensitivity
 *               = offset[mCelsius] * gain2
 */

#define MCELSIUS_PER_CELSIUS			1000

/* Default pressure sensitivity */
#define ST_PRESS_LSB_PER_MBAR			4096UL
#define ST_PRESS_KPASCAL_NANO_SCALE		(100000000UL / \
						 ST_PRESS_LSB_PER_MBAR)

/* Default temperature sensitivity */
#define ST_PRESS_LSB_PER_CELSIUS		480UL
#define ST_PRESS_MILLI_CELSIUS_OFFSET		42500UL

/* FULLSCALE */
#define ST_PRESS_FS_AVL_1100MB			1100
>>>>>>> v4.9.227
#define ST_PRESS_FS_AVL_1260MB			1260

#define ST_PRESS_1_OUT_XL_ADDR			0x28
#define ST_TEMP_1_OUT_L_ADDR			0x2b

<<<<<<< HEAD
/* CUSTOM VALUES FOR LPS331AP SENSOR */
=======
/*
 * CUSTOM VALUES FOR LPS331AP SENSOR
 * See LPS331AP datasheet:
 * http://www2.st.com/resource/en/datasheet/lps331ap.pdf
 */
>>>>>>> v4.9.227
#define ST_PRESS_LPS331AP_WAI_EXP		0xbb
#define ST_PRESS_LPS331AP_ODR_ADDR		0x20
#define ST_PRESS_LPS331AP_ODR_MASK		0x70
#define ST_PRESS_LPS331AP_ODR_AVL_1HZ_VAL	0x01
#define ST_PRESS_LPS331AP_ODR_AVL_7HZ_VAL	0x05
#define ST_PRESS_LPS331AP_ODR_AVL_13HZ_VAL	0x06
#define ST_PRESS_LPS331AP_ODR_AVL_25HZ_VAL	0x07
#define ST_PRESS_LPS331AP_PW_ADDR		0x20
#define ST_PRESS_LPS331AP_PW_MASK		0x80
#define ST_PRESS_LPS331AP_FS_ADDR		0x23
#define ST_PRESS_LPS331AP_FS_MASK		0x30
<<<<<<< HEAD
#define ST_PRESS_LPS331AP_FS_AVL_1260_VAL	0x00
#define ST_PRESS_LPS331AP_FS_AVL_1260_GAIN	ST_PRESS_KPASCAL_NANO_SCALE
#define ST_PRESS_LPS331AP_FS_AVL_TEMP_GAIN	ST_PRESS_CELSIUS_NANO_SCALE
=======
>>>>>>> v4.9.227
#define ST_PRESS_LPS331AP_BDU_ADDR		0x20
#define ST_PRESS_LPS331AP_BDU_MASK		0x04
#define ST_PRESS_LPS331AP_DRDY_IRQ_ADDR		0x22
#define ST_PRESS_LPS331AP_DRDY_IRQ_INT1_MASK	0x04
#define ST_PRESS_LPS331AP_DRDY_IRQ_INT2_MASK	0x20
<<<<<<< HEAD
#define ST_PRESS_LPS331AP_MULTIREAD_BIT		true
#define ST_PRESS_LPS331AP_TEMP_OFFSET		42500

/* CUSTOM VALUES FOR LPS001WP SENSOR */
=======
#define ST_PRESS_LPS331AP_IHL_IRQ_ADDR		0x22
#define ST_PRESS_LPS331AP_IHL_IRQ_MASK		0x80
#define ST_PRESS_LPS331AP_OD_IRQ_ADDR		0x22
#define ST_PRESS_LPS331AP_OD_IRQ_MASK		0x40
#define ST_PRESS_LPS331AP_MULTIREAD_BIT		true

/*
 * CUSTOM VALUES FOR THE OBSOLETE LPS001WP SENSOR
 */

/* LPS001WP pressure resolution */
#define ST_PRESS_LPS001WP_LSB_PER_MBAR		16UL
/* LPS001WP temperature resolution */
#define ST_PRESS_LPS001WP_LSB_PER_CELSIUS	64UL

>>>>>>> v4.9.227
#define ST_PRESS_LPS001WP_WAI_EXP		0xba
#define ST_PRESS_LPS001WP_ODR_ADDR		0x20
#define ST_PRESS_LPS001WP_ODR_MASK		0x30
#define ST_PRESS_LPS001WP_ODR_AVL_1HZ_VAL	0x01
#define ST_PRESS_LPS001WP_ODR_AVL_7HZ_VAL	0x02
#define ST_PRESS_LPS001WP_ODR_AVL_13HZ_VAL	0x03
#define ST_PRESS_LPS001WP_PW_ADDR		0x20
#define ST_PRESS_LPS001WP_PW_MASK		0x40
<<<<<<< HEAD
=======
#define ST_PRESS_LPS001WP_FS_AVL_PRESS_GAIN \
	(100000000UL / ST_PRESS_LPS001WP_LSB_PER_MBAR)
>>>>>>> v4.9.227
#define ST_PRESS_LPS001WP_BDU_ADDR		0x20
#define ST_PRESS_LPS001WP_BDU_MASK		0x04
#define ST_PRESS_LPS001WP_MULTIREAD_BIT		true
#define ST_PRESS_LPS001WP_OUT_L_ADDR		0x28
#define ST_TEMP_LPS001WP_OUT_L_ADDR		0x2a

<<<<<<< HEAD
/* CUSTOM VALUES FOR LPS25H SENSOR */
=======
/*
 * CUSTOM VALUES FOR LPS25H SENSOR
 * See LPS25H datasheet:
 * http://www2.st.com/resource/en/datasheet/lps25h.pdf
 */
>>>>>>> v4.9.227
#define ST_PRESS_LPS25H_WAI_EXP			0xbd
#define ST_PRESS_LPS25H_ODR_ADDR		0x20
#define ST_PRESS_LPS25H_ODR_MASK		0x70
#define ST_PRESS_LPS25H_ODR_AVL_1HZ_VAL		0x01
#define ST_PRESS_LPS25H_ODR_AVL_7HZ_VAL		0x02
#define ST_PRESS_LPS25H_ODR_AVL_13HZ_VAL	0x03
#define ST_PRESS_LPS25H_ODR_AVL_25HZ_VAL	0x04
#define ST_PRESS_LPS25H_PW_ADDR			0x20
#define ST_PRESS_LPS25H_PW_MASK			0x80
<<<<<<< HEAD
#define ST_PRESS_LPS25H_FS_ADDR			0x00
#define ST_PRESS_LPS25H_FS_MASK			0x00
#define ST_PRESS_LPS25H_FS_AVL_1260_VAL		0x00
#define ST_PRESS_LPS25H_FS_AVL_1260_GAIN	ST_PRESS_KPASCAL_NANO_SCALE
#define ST_PRESS_LPS25H_FS_AVL_TEMP_GAIN	ST_PRESS_CELSIUS_NANO_SCALE
=======
>>>>>>> v4.9.227
#define ST_PRESS_LPS25H_BDU_ADDR		0x20
#define ST_PRESS_LPS25H_BDU_MASK		0x04
#define ST_PRESS_LPS25H_DRDY_IRQ_ADDR		0x23
#define ST_PRESS_LPS25H_DRDY_IRQ_INT1_MASK	0x01
#define ST_PRESS_LPS25H_DRDY_IRQ_INT2_MASK	0x10
<<<<<<< HEAD
#define ST_PRESS_LPS25H_MULTIREAD_BIT		true
#define ST_PRESS_LPS25H_TEMP_OFFSET		42500
#define ST_PRESS_LPS25H_OUT_XL_ADDR		0x28
#define ST_TEMP_LPS25H_OUT_L_ADDR		0x2b

static const struct iio_chan_spec st_press_1_channels[] = {
	{
		.type = IIO_PRESSURE,
		.channel2 = IIO_NO_MOD,
		.address = ST_PRESS_1_OUT_XL_ADDR,
		.scan_index = ST_SENSORS_SCAN_X,
		.scan_type = {
			.sign = 'u',
			.realbits = 24,
			.storagebits = 24,
=======
#define ST_PRESS_LPS25H_IHL_IRQ_ADDR		0x22
#define ST_PRESS_LPS25H_IHL_IRQ_MASK		0x80
#define ST_PRESS_LPS25H_OD_IRQ_ADDR		0x22
#define ST_PRESS_LPS25H_OD_IRQ_MASK		0x40
#define ST_PRESS_LPS25H_MULTIREAD_BIT		true
#define ST_PRESS_LPS25H_OUT_XL_ADDR		0x28
#define ST_TEMP_LPS25H_OUT_L_ADDR		0x2b

/*
 * CUSTOM VALUES FOR LPS22HB SENSOR
 * See LPS22HB datasheet:
 * http://www2.st.com/resource/en/datasheet/lps22hb.pdf
 */

/* LPS22HB temperature sensitivity */
#define ST_PRESS_LPS22HB_LSB_PER_CELSIUS	100UL

#define ST_PRESS_LPS22HB_WAI_EXP		0xb1
#define ST_PRESS_LPS22HB_ODR_ADDR		0x10
#define ST_PRESS_LPS22HB_ODR_MASK		0x70
#define ST_PRESS_LPS22HB_ODR_AVL_1HZ_VAL	0x01
#define ST_PRESS_LPS22HB_ODR_AVL_10HZ_VAL	0x02
#define ST_PRESS_LPS22HB_ODR_AVL_25HZ_VAL	0x03
#define ST_PRESS_LPS22HB_ODR_AVL_50HZ_VAL	0x04
#define ST_PRESS_LPS22HB_ODR_AVL_75HZ_VAL	0x05
#define ST_PRESS_LPS22HB_PW_ADDR		0x10
#define ST_PRESS_LPS22HB_PW_MASK		0x70
#define ST_PRESS_LPS22HB_BDU_ADDR		0x10
#define ST_PRESS_LPS22HB_BDU_MASK		0x02
#define ST_PRESS_LPS22HB_DRDY_IRQ_ADDR		0x12
#define ST_PRESS_LPS22HB_DRDY_IRQ_INT1_MASK	0x04
#define ST_PRESS_LPS22HB_DRDY_IRQ_INT2_MASK	0x08
#define ST_PRESS_LPS22HB_IHL_IRQ_ADDR		0x12
#define ST_PRESS_LPS22HB_IHL_IRQ_MASK		0x80
#define ST_PRESS_LPS22HB_OD_IRQ_ADDR		0x12
#define ST_PRESS_LPS22HB_OD_IRQ_MASK		0x40
#define ST_PRESS_LPS22HB_MULTIREAD_BIT		true

static const struct iio_chan_spec st_press_1_channels[] = {
	{
		.type = IIO_PRESSURE,
		.address = ST_PRESS_1_OUT_XL_ADDR,
		.scan_index = 0,
		.scan_type = {
			.sign = 's',
			.realbits = 24,
			.storagebits = 32,
>>>>>>> v4.9.227
			.endianness = IIO_LE,
		},
		.info_mask_separate =
			BIT(IIO_CHAN_INFO_RAW) | BIT(IIO_CHAN_INFO_SCALE),
<<<<<<< HEAD
		.modified = 0,
	},
	{
		.type = IIO_TEMP,
		.channel2 = IIO_NO_MOD,
		.address = ST_TEMP_1_OUT_L_ADDR,
		.scan_index = -1,
		.scan_type = {
			.sign = 'u',
=======
	},
	{
		.type = IIO_TEMP,
		.address = ST_TEMP_1_OUT_L_ADDR,
		.scan_index = 1,
		.scan_type = {
			.sign = 's',
>>>>>>> v4.9.227
			.realbits = 16,
			.storagebits = 16,
			.endianness = IIO_LE,
		},
		.info_mask_separate =
			BIT(IIO_CHAN_INFO_RAW) |
			BIT(IIO_CHAN_INFO_SCALE) |
			BIT(IIO_CHAN_INFO_OFFSET),
<<<<<<< HEAD
		.modified = 0,
	},
	IIO_CHAN_SOFT_TIMESTAMP(1)
=======
	},
	IIO_CHAN_SOFT_TIMESTAMP(2)
>>>>>>> v4.9.227
};

static const struct iio_chan_spec st_press_lps001wp_channels[] = {
	{
		.type = IIO_PRESSURE,
<<<<<<< HEAD
		.channel2 = IIO_NO_MOD,
		.address = ST_PRESS_LPS001WP_OUT_L_ADDR,
		.scan_index = ST_SENSORS_SCAN_X,
		.scan_type = {
			.sign = 'u',
			.realbits = 16,
			.storagebits = 16,
			.endianness = IIO_LE,
		},
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
		.modified = 0,
	},
	{
		.type = IIO_TEMP,
		.channel2 = IIO_NO_MOD,
		.address = ST_TEMP_LPS001WP_OUT_L_ADDR,
		.scan_index = -1,
		.scan_type = {
			.sign = 'u',
=======
		.address = ST_PRESS_LPS001WP_OUT_L_ADDR,
		.scan_index = 0,
		.scan_type = {
			.sign = 's',
>>>>>>> v4.9.227
			.realbits = 16,
			.storagebits = 16,
			.endianness = IIO_LE,
		},
		.info_mask_separate =
			BIT(IIO_CHAN_INFO_RAW) |
<<<<<<< HEAD
			BIT(IIO_CHAN_INFO_OFFSET),
		.modified = 0,
	},
	IIO_CHAN_SOFT_TIMESTAMP(1)
};

static const struct st_sensors st_press_sensors[] = {
	{
		.wai = ST_PRESS_LPS331AP_WAI_EXP,
=======
			BIT(IIO_CHAN_INFO_SCALE),
	},
	{
		.type = IIO_TEMP,
		.address = ST_TEMP_LPS001WP_OUT_L_ADDR,
		.scan_index = 1,
		.scan_type = {
			.sign = 's',
			.realbits = 16,
			.storagebits = 16,
			.endianness = IIO_LE,
		},
		.info_mask_separate =
			BIT(IIO_CHAN_INFO_RAW) |
			BIT(IIO_CHAN_INFO_SCALE),
	},
	IIO_CHAN_SOFT_TIMESTAMP(2)
};

static const struct iio_chan_spec st_press_lps22hb_channels[] = {
	{
		.type = IIO_PRESSURE,
		.address = ST_PRESS_1_OUT_XL_ADDR,
		.scan_index = 0,
		.scan_type = {
			.sign = 's',
			.realbits = 24,
			.storagebits = 32,
			.endianness = IIO_LE,
		},
		.info_mask_separate =
			BIT(IIO_CHAN_INFO_RAW) |
			BIT(IIO_CHAN_INFO_SCALE),
		.info_mask_shared_by_all = BIT(IIO_CHAN_INFO_SAMP_FREQ),
	},
	{
		.type = IIO_TEMP,
		.address = ST_TEMP_1_OUT_L_ADDR,
		.scan_index = 1,
		.scan_type = {
			.sign = 's',
			.realbits = 16,
			.storagebits = 16,
			.endianness = IIO_LE,
		},
		.info_mask_separate =
			BIT(IIO_CHAN_INFO_RAW) |
			BIT(IIO_CHAN_INFO_SCALE),
		.info_mask_shared_by_all = BIT(IIO_CHAN_INFO_SAMP_FREQ),
	},
	IIO_CHAN_SOFT_TIMESTAMP(2)
};

static const struct st_sensor_settings st_press_sensors_settings[] = {
	{
		.wai = ST_PRESS_LPS331AP_WAI_EXP,
		.wai_addr = ST_SENSORS_DEFAULT_WAI_ADDRESS,
>>>>>>> v4.9.227
		.sensors_supported = {
			[0] = LPS331AP_PRESS_DEV_NAME,
		},
		.ch = (struct iio_chan_spec *)st_press_1_channels,
		.num_ch = ARRAY_SIZE(st_press_1_channels),
		.odr = {
			.addr = ST_PRESS_LPS331AP_ODR_ADDR,
			.mask = ST_PRESS_LPS331AP_ODR_MASK,
			.odr_avl = {
				{ 1, ST_PRESS_LPS331AP_ODR_AVL_1HZ_VAL, },
				{ 7, ST_PRESS_LPS331AP_ODR_AVL_7HZ_VAL, },
				{ 13, ST_PRESS_LPS331AP_ODR_AVL_13HZ_VAL, },
				{ 25, ST_PRESS_LPS331AP_ODR_AVL_25HZ_VAL, },
			},
		},
		.pw = {
			.addr = ST_PRESS_LPS331AP_PW_ADDR,
			.mask = ST_PRESS_LPS331AP_PW_MASK,
			.value_on = ST_SENSORS_DEFAULT_POWER_ON_VALUE,
			.value_off = ST_SENSORS_DEFAULT_POWER_OFF_VALUE,
		},
		.fs = {
			.addr = ST_PRESS_LPS331AP_FS_ADDR,
			.mask = ST_PRESS_LPS331AP_FS_MASK,
			.fs_avl = {
<<<<<<< HEAD
				[0] = {
					.num = ST_PRESS_FS_AVL_1260MB,
					.value = ST_PRESS_LPS331AP_FS_AVL_1260_VAL,
					.gain = ST_PRESS_LPS331AP_FS_AVL_1260_GAIN,
					.gain2 = ST_PRESS_LPS331AP_FS_AVL_TEMP_GAIN,
=======
				/*
				 * Pressure and temperature sensitivity values
				 * as defined in table 3 of LPS331AP datasheet.
				 */
				[0] = {
					.num = ST_PRESS_FS_AVL_1260MB,
					.gain = ST_PRESS_KPASCAL_NANO_SCALE,
					.gain2 = ST_PRESS_LSB_PER_CELSIUS,
>>>>>>> v4.9.227
				},
			},
		},
		.bdu = {
			.addr = ST_PRESS_LPS331AP_BDU_ADDR,
			.mask = ST_PRESS_LPS331AP_BDU_MASK,
		},
		.drdy_irq = {
			.addr = ST_PRESS_LPS331AP_DRDY_IRQ_ADDR,
			.mask_int1 = ST_PRESS_LPS331AP_DRDY_IRQ_INT1_MASK,
			.mask_int2 = ST_PRESS_LPS331AP_DRDY_IRQ_INT2_MASK,
<<<<<<< HEAD
=======
			.addr_ihl = ST_PRESS_LPS331AP_IHL_IRQ_ADDR,
			.mask_ihl = ST_PRESS_LPS331AP_IHL_IRQ_MASK,
			.addr_od = ST_PRESS_LPS331AP_OD_IRQ_ADDR,
			.mask_od = ST_PRESS_LPS331AP_OD_IRQ_MASK,
			.addr_stat_drdy = ST_SENSORS_DEFAULT_STAT_ADDR,
>>>>>>> v4.9.227
		},
		.multi_read_bit = ST_PRESS_LPS331AP_MULTIREAD_BIT,
		.bootime = 2,
	},
	{
		.wai = ST_PRESS_LPS001WP_WAI_EXP,
<<<<<<< HEAD
=======
		.wai_addr = ST_SENSORS_DEFAULT_WAI_ADDRESS,
>>>>>>> v4.9.227
		.sensors_supported = {
			[0] = LPS001WP_PRESS_DEV_NAME,
		},
		.ch = (struct iio_chan_spec *)st_press_lps001wp_channels,
		.num_ch = ARRAY_SIZE(st_press_lps001wp_channels),
		.odr = {
			.addr = ST_PRESS_LPS001WP_ODR_ADDR,
			.mask = ST_PRESS_LPS001WP_ODR_MASK,
			.odr_avl = {
				{ 1, ST_PRESS_LPS001WP_ODR_AVL_1HZ_VAL, },
				{ 7, ST_PRESS_LPS001WP_ODR_AVL_7HZ_VAL, },
				{ 13, ST_PRESS_LPS001WP_ODR_AVL_13HZ_VAL, },
			},
		},
		.pw = {
			.addr = ST_PRESS_LPS001WP_PW_ADDR,
			.mask = ST_PRESS_LPS001WP_PW_MASK,
			.value_on = ST_SENSORS_DEFAULT_POWER_ON_VALUE,
			.value_off = ST_SENSORS_DEFAULT_POWER_OFF_VALUE,
		},
		.fs = {
<<<<<<< HEAD
			.addr = 0,
=======
			.fs_avl = {
				/*
				 * Pressure and temperature resolution values
				 * as defined in table 3 of LPS001WP datasheet.
				 */
				[0] = {
					.num = ST_PRESS_FS_AVL_1100MB,
					.gain = ST_PRESS_LPS001WP_FS_AVL_PRESS_GAIN,
					.gain2 = ST_PRESS_LPS001WP_LSB_PER_CELSIUS,
				},
			},
>>>>>>> v4.9.227
		},
		.bdu = {
			.addr = ST_PRESS_LPS001WP_BDU_ADDR,
			.mask = ST_PRESS_LPS001WP_BDU_MASK,
		},
		.drdy_irq = {
			.addr = 0,
		},
		.multi_read_bit = ST_PRESS_LPS001WP_MULTIREAD_BIT,
		.bootime = 2,
	},
	{
		.wai = ST_PRESS_LPS25H_WAI_EXP,
<<<<<<< HEAD
=======
		.wai_addr = ST_SENSORS_DEFAULT_WAI_ADDRESS,
>>>>>>> v4.9.227
		.sensors_supported = {
			[0] = LPS25H_PRESS_DEV_NAME,
		},
		.ch = (struct iio_chan_spec *)st_press_1_channels,
		.num_ch = ARRAY_SIZE(st_press_1_channels),
		.odr = {
			.addr = ST_PRESS_LPS25H_ODR_ADDR,
			.mask = ST_PRESS_LPS25H_ODR_MASK,
			.odr_avl = {
				{ 1, ST_PRESS_LPS25H_ODR_AVL_1HZ_VAL, },
				{ 7, ST_PRESS_LPS25H_ODR_AVL_7HZ_VAL, },
				{ 13, ST_PRESS_LPS25H_ODR_AVL_13HZ_VAL, },
				{ 25, ST_PRESS_LPS25H_ODR_AVL_25HZ_VAL, },
			},
		},
		.pw = {
			.addr = ST_PRESS_LPS25H_PW_ADDR,
			.mask = ST_PRESS_LPS25H_PW_MASK,
			.value_on = ST_SENSORS_DEFAULT_POWER_ON_VALUE,
			.value_off = ST_SENSORS_DEFAULT_POWER_OFF_VALUE,
		},
		.fs = {
<<<<<<< HEAD
			.addr = ST_PRESS_LPS25H_FS_ADDR,
			.mask = ST_PRESS_LPS25H_FS_MASK,
			.fs_avl = {
				[0] = {
					.num = ST_PRESS_FS_AVL_1260MB,
					.value = ST_PRESS_LPS25H_FS_AVL_1260_VAL,
					.gain = ST_PRESS_LPS25H_FS_AVL_1260_GAIN,
					.gain2 = ST_PRESS_LPS25H_FS_AVL_TEMP_GAIN,
=======
			.fs_avl = {
				/*
				 * Pressure and temperature sensitivity values
				 * as defined in table 3 of LPS25H datasheet.
				 */
				[0] = {
					.num = ST_PRESS_FS_AVL_1260MB,
					.gain = ST_PRESS_KPASCAL_NANO_SCALE,
					.gain2 = ST_PRESS_LSB_PER_CELSIUS,
>>>>>>> v4.9.227
				},
			},
		},
		.bdu = {
			.addr = ST_PRESS_LPS25H_BDU_ADDR,
			.mask = ST_PRESS_LPS25H_BDU_MASK,
		},
		.drdy_irq = {
			.addr = ST_PRESS_LPS25H_DRDY_IRQ_ADDR,
			.mask_int1 = ST_PRESS_LPS25H_DRDY_IRQ_INT1_MASK,
			.mask_int2 = ST_PRESS_LPS25H_DRDY_IRQ_INT2_MASK,
<<<<<<< HEAD
=======
			.addr_ihl = ST_PRESS_LPS25H_IHL_IRQ_ADDR,
			.mask_ihl = ST_PRESS_LPS25H_IHL_IRQ_MASK,
			.addr_od = ST_PRESS_LPS25H_OD_IRQ_ADDR,
			.mask_od = ST_PRESS_LPS25H_OD_IRQ_MASK,
			.addr_stat_drdy = ST_SENSORS_DEFAULT_STAT_ADDR,
>>>>>>> v4.9.227
		},
		.multi_read_bit = ST_PRESS_LPS25H_MULTIREAD_BIT,
		.bootime = 2,
	},
<<<<<<< HEAD
=======
	{
		.wai = ST_PRESS_LPS22HB_WAI_EXP,
		.wai_addr = ST_SENSORS_DEFAULT_WAI_ADDRESS,
		.sensors_supported = {
			[0] = LPS22HB_PRESS_DEV_NAME,
		},
		.ch = (struct iio_chan_spec *)st_press_lps22hb_channels,
		.num_ch = ARRAY_SIZE(st_press_lps22hb_channels),
		.odr = {
			.addr = ST_PRESS_LPS22HB_ODR_ADDR,
			.mask = ST_PRESS_LPS22HB_ODR_MASK,
			.odr_avl = {
				{ 1, ST_PRESS_LPS22HB_ODR_AVL_1HZ_VAL, },
				{ 10, ST_PRESS_LPS22HB_ODR_AVL_10HZ_VAL, },
				{ 25, ST_PRESS_LPS22HB_ODR_AVL_25HZ_VAL, },
				{ 50, ST_PRESS_LPS22HB_ODR_AVL_50HZ_VAL, },
				{ 75, ST_PRESS_LPS22HB_ODR_AVL_75HZ_VAL, },
			},
		},
		.pw = {
			.addr = ST_PRESS_LPS22HB_PW_ADDR,
			.mask = ST_PRESS_LPS22HB_PW_MASK,
			.value_off = ST_SENSORS_DEFAULT_POWER_OFF_VALUE,
		},
		.fs = {
			.fs_avl = {
				/*
				 * Pressure and temperature sensitivity values
				 * as defined in table 3 of LPS22HB datasheet.
				 */
				[0] = {
					.num = ST_PRESS_FS_AVL_1260MB,
					.gain = ST_PRESS_KPASCAL_NANO_SCALE,
					.gain2 = ST_PRESS_LPS22HB_LSB_PER_CELSIUS,
				},
			},
		},
		.bdu = {
			.addr = ST_PRESS_LPS22HB_BDU_ADDR,
			.mask = ST_PRESS_LPS22HB_BDU_MASK,
		},
		.drdy_irq = {
			.addr = ST_PRESS_LPS22HB_DRDY_IRQ_ADDR,
			.mask_int1 = ST_PRESS_LPS22HB_DRDY_IRQ_INT1_MASK,
			.mask_int2 = ST_PRESS_LPS22HB_DRDY_IRQ_INT2_MASK,
			.addr_ihl = ST_PRESS_LPS22HB_IHL_IRQ_ADDR,
			.mask_ihl = ST_PRESS_LPS22HB_IHL_IRQ_MASK,
			.addr_od = ST_PRESS_LPS22HB_OD_IRQ_ADDR,
			.mask_od = ST_PRESS_LPS22HB_OD_IRQ_MASK,
			.addr_stat_drdy = ST_SENSORS_DEFAULT_STAT_ADDR,
		},
		.multi_read_bit = ST_PRESS_LPS22HB_MULTIREAD_BIT,
	},
>>>>>>> v4.9.227
};

static int st_press_write_raw(struct iio_dev *indio_dev,
			      struct iio_chan_spec const *ch,
			      int val,
			      int val2,
			      long mask)
{
	int err;

	switch (mask) {
	case IIO_CHAN_INFO_SAMP_FREQ:
		if (val2)
			return -EINVAL;
		mutex_lock(&indio_dev->mlock);
		err = st_sensors_set_odr(indio_dev, val);
		mutex_unlock(&indio_dev->mlock);
		return err;
	default:
		return -EINVAL;
	}
}

static int st_press_read_raw(struct iio_dev *indio_dev,
			struct iio_chan_spec const *ch, int *val,
							int *val2, long mask)
{
	int err;
<<<<<<< HEAD
	struct st_sensor_data *pdata = iio_priv(indio_dev);
=======
	struct st_sensor_data *press_data = iio_priv(indio_dev);
>>>>>>> v4.9.227

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		err = st_sensors_read_info_raw(indio_dev, ch, val);
		if (err < 0)
			goto read_error;

		return IIO_VAL_INT;
	case IIO_CHAN_INFO_SCALE:
<<<<<<< HEAD
		*val = 0;

		switch (ch->type) {
		case IIO_PRESSURE:
			*val2 = pdata->current_fullscale->gain;
			break;
		case IIO_TEMP:
			*val2 = pdata->current_fullscale->gain2;
			break;
=======
		switch (ch->type) {
		case IIO_PRESSURE:
			*val = 0;
			*val2 = press_data->current_fullscale->gain;
			return IIO_VAL_INT_PLUS_NANO;
		case IIO_TEMP:
			*val = MCELSIUS_PER_CELSIUS;
			*val2 = press_data->current_fullscale->gain2;
			return IIO_VAL_FRACTIONAL;
>>>>>>> v4.9.227
		default:
			err = -EINVAL;
			goto read_error;
		}

<<<<<<< HEAD
		return IIO_VAL_INT_PLUS_NANO;
	case IIO_CHAN_INFO_OFFSET:
		switch (ch->type) {
		case IIO_TEMP:
			*val = 425;
			*val2 = 10;
=======
	case IIO_CHAN_INFO_OFFSET:
		switch (ch->type) {
		case IIO_TEMP:
			*val = ST_PRESS_MILLI_CELSIUS_OFFSET *
			       press_data->current_fullscale->gain2;
			*val2 = MCELSIUS_PER_CELSIUS;
>>>>>>> v4.9.227
			break;
		default:
			err = -EINVAL;
			goto read_error;
		}

		return IIO_VAL_FRACTIONAL;
	case IIO_CHAN_INFO_SAMP_FREQ:
<<<<<<< HEAD
		*val = pdata->odr;
=======
		*val = press_data->odr;
>>>>>>> v4.9.227
		return IIO_VAL_INT;
	default:
		return -EINVAL;
	}

read_error:
	return err;
}

static ST_SENSORS_DEV_ATTR_SAMP_FREQ_AVAIL();

static struct attribute *st_press_attributes[] = {
	&iio_dev_attr_sampling_frequency_available.dev_attr.attr,
	NULL,
};

static const struct attribute_group st_press_attribute_group = {
	.attrs = st_press_attributes,
};

static const struct iio_info press_info = {
	.driver_module = THIS_MODULE,
	.attrs = &st_press_attribute_group,
	.read_raw = &st_press_read_raw,
	.write_raw = &st_press_write_raw,
<<<<<<< HEAD
=======
	.debugfs_reg_access = &st_sensors_debugfs_reg_access,
>>>>>>> v4.9.227
};

#ifdef CONFIG_IIO_TRIGGER
static const struct iio_trigger_ops st_press_trigger_ops = {
	.owner = THIS_MODULE,
	.set_trigger_state = ST_PRESS_TRIGGER_SET_STATE,
<<<<<<< HEAD
=======
	.validate_device = st_sensors_validate_device,
>>>>>>> v4.9.227
};
#define ST_PRESS_TRIGGER_OPS (&st_press_trigger_ops)
#else
#define ST_PRESS_TRIGGER_OPS NULL
#endif

<<<<<<< HEAD
int st_press_common_probe(struct iio_dev *indio_dev,
				struct st_sensors_platform_data *plat_data)
{
	struct st_sensor_data *pdata = iio_priv(indio_dev);
	int irq = pdata->get_irq_data_ready(indio_dev);
=======
int st_press_common_probe(struct iio_dev *indio_dev)
{
	struct st_sensor_data *press_data = iio_priv(indio_dev);
	struct st_sensors_platform_data *pdata =
		(struct st_sensors_platform_data *)press_data->dev->platform_data;
	int irq = press_data->get_irq_data_ready(indio_dev);
>>>>>>> v4.9.227
	int err;

	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->info = &press_info;
<<<<<<< HEAD

	st_sensors_power_enable(indio_dev);

	err = st_sensors_check_device_support(indio_dev,
					      ARRAY_SIZE(st_press_sensors),
					      st_press_sensors);
	if (err < 0)
		return err;

	pdata->num_data_channels = ST_PRESS_NUMBER_DATA_CHANNELS;
	pdata->multiread_bit     = pdata->sensor->multi_read_bit;
	indio_dev->channels      = pdata->sensor->ch;
	indio_dev->num_channels  = pdata->sensor->num_ch;

	if (pdata->sensor->fs.addr != 0)
		pdata->current_fullscale = (struct st_sensor_fullscale_avl *)
			&pdata->sensor->fs.fs_avl[0];

	pdata->odr = pdata->sensor->odr.odr_avl[0].hz;

	/* Some devices don't support a data ready pin. */
	if (!plat_data && pdata->sensor->drdy_irq.addr)
		plat_data =
			(struct st_sensors_platform_data *)&default_press_pdata;

	err = st_sensors_init_sensor(indio_dev, plat_data);
	if (err < 0)
		return err;

	err = st_press_allocate_ring(indio_dev);
	if (err < 0)
		return err;
=======
	mutex_init(&press_data->tb.buf_lock);

	err = st_sensors_power_enable(indio_dev);
	if (err)
		return err;

	err = st_sensors_check_device_support(indio_dev,
					ARRAY_SIZE(st_press_sensors_settings),
					st_press_sensors_settings);
	if (err < 0)
		goto st_press_power_off;

	/*
	 * Skip timestamping channel while declaring available channels to
	 * common st_sensor layer. Look at st_sensors_get_buffer_element() to
	 * see how timestamps are explicitly pushed as last samples block
	 * element.
	 */
	press_data->num_data_channels = press_data->sensor_settings->num_ch - 1;
	press_data->multiread_bit = press_data->sensor_settings->multi_read_bit;
	indio_dev->channels = press_data->sensor_settings->ch;
	indio_dev->num_channels = press_data->sensor_settings->num_ch;

	press_data->current_fullscale =
		(struct st_sensor_fullscale_avl *)
			&press_data->sensor_settings->fs.fs_avl[0];

	press_data->odr = press_data->sensor_settings->odr.odr_avl[0].hz;

	/* Some devices don't support a data ready pin. */
	if (!pdata && press_data->sensor_settings->drdy_irq.addr)
		pdata =	(struct st_sensors_platform_data *)&default_press_pdata;

	err = st_sensors_init_sensor(indio_dev, pdata);
	if (err < 0)
		goto st_press_power_off;

	err = st_press_allocate_ring(indio_dev);
	if (err < 0)
		goto st_press_power_off;
>>>>>>> v4.9.227

	if (irq > 0) {
		err = st_sensors_allocate_trigger(indio_dev,
						  ST_PRESS_TRIGGER_OPS);
		if (err < 0)
			goto st_press_probe_trigger_error;
	}

	err = iio_device_register(indio_dev);
	if (err)
		goto st_press_device_register_error;

	dev_info(&indio_dev->dev, "registered pressure sensor %s\n",
		 indio_dev->name);

	return err;

st_press_device_register_error:
	if (irq > 0)
		st_sensors_deallocate_trigger(indio_dev);
st_press_probe_trigger_error:
	st_press_deallocate_ring(indio_dev);
<<<<<<< HEAD
=======
st_press_power_off:
	st_sensors_power_disable(indio_dev);
>>>>>>> v4.9.227

	return err;
}
EXPORT_SYMBOL(st_press_common_probe);

void st_press_common_remove(struct iio_dev *indio_dev)
{
<<<<<<< HEAD
	struct st_sensor_data *pdata = iio_priv(indio_dev);
=======
	struct st_sensor_data *press_data = iio_priv(indio_dev);
>>>>>>> v4.9.227

	st_sensors_power_disable(indio_dev);

	iio_device_unregister(indio_dev);
<<<<<<< HEAD
	if (pdata->get_irq_data_ready(indio_dev) > 0)
=======
	if (press_data->get_irq_data_ready(indio_dev) > 0)
>>>>>>> v4.9.227
		st_sensors_deallocate_trigger(indio_dev);

	st_press_deallocate_ring(indio_dev);
}
EXPORT_SYMBOL(st_press_common_remove);

MODULE_AUTHOR("Denis Ciocca <denis.ciocca@st.com>");
MODULE_DESCRIPTION("STMicroelectronics pressures driver");
MODULE_LICENSE("GPL v2");
