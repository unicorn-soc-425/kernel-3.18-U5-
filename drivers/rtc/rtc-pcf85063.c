/*
 * An I2C driver for the PCF85063 RTC
 * Copyright 2014 Rose Technology
 *
 * Author: Søren Andersen <san@rosetechnology.dk>
 * Maintainers: http://www.nslu2-linux.org/
 *
 * based on the other drivers in this same directory.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/i2c.h>
#include <linux/bcd.h>
#include <linux/rtc.h>
#include <linux/module.h>

<<<<<<< HEAD
#define DRV_VERSION "0.0.1"

#define PCF85063_REG_CTRL1		0x00 /* status */
#define PCF85063_REG_CTRL2		0x01

#define PCF85063_REG_SC			0x04 /* datetime */
=======
/*
 * Information for this driver was pulled from the following datasheets.
 *
 *  http://www.nxp.com/documents/data_sheet/PCF85063A.pdf
 *  http://www.nxp.com/documents/data_sheet/PCF85063TP.pdf
 *
 *  PCF85063A -- Rev. 6 — 18 November 2015
 *  PCF85063TP -- Rev. 4 — 6 May 2015
*/

#define PCF85063_REG_CTRL1		0x00 /* status */
#define PCF85063_REG_CTRL1_STOP		BIT(5)
#define PCF85063_REG_CTRL2		0x01

#define PCF85063_REG_SC			0x04 /* datetime */
#define PCF85063_REG_SC_OS		0x80
>>>>>>> v4.9.227
#define PCF85063_REG_MN			0x05
#define PCF85063_REG_HR			0x06
#define PCF85063_REG_DM			0x07
#define PCF85063_REG_DW			0x08
#define PCF85063_REG_MO			0x09
#define PCF85063_REG_YR			0x0A

<<<<<<< HEAD
#define PCF85063_MO_C			0x80 /* century */

static struct i2c_driver pcf85063_driver;

struct pcf85063 {
	struct rtc_device *rtc;
	int c_polarity;	/* 0: MO_C=1 means 19xx, otherwise MO_C=1 means 20xx */
	int voltage_low; /* indicates if a low_voltage was detected */
};

/*
 * In the routines that deal directly with the pcf85063 hardware, we use
 * rtc_time -- month 0-11, hour 0-23, yr = calendar year-epoch.
 */
static int pcf85063_get_datetime(struct i2c_client *client, struct rtc_time *tm)
{
	struct pcf85063 *pcf85063 = i2c_get_clientdata(client);
	unsigned char buf[13] = { PCF85063_REG_CTRL1 };
	struct i2c_msg msgs[] = {
		{/* setup read ptr */
			.addr = client->addr,
			.len = 1,
			.buf = buf
		},
		{/* read status + date */
			.addr = client->addr,
			.flags = I2C_M_RD,
			.len = 13,
			.buf = buf
		},
	};

	/* read registers */
	if ((i2c_transfer(client->adapter, msgs, 2)) != 2) {
		dev_err(&client->dev, "%s: read error\n", __func__);
		return -EIO;
	}

	tm->tm_sec = bcd2bin(buf[PCF85063_REG_SC] & 0x7F);
	tm->tm_min = bcd2bin(buf[PCF85063_REG_MN] & 0x7F);
	tm->tm_hour = bcd2bin(buf[PCF85063_REG_HR] & 0x3F); /* rtc hr 0-23 */
	tm->tm_mday = bcd2bin(buf[PCF85063_REG_DM] & 0x3F);
	tm->tm_wday = buf[PCF85063_REG_DW] & 0x07;
	tm->tm_mon = bcd2bin(buf[PCF85063_REG_MO] & 0x1F) - 1; /* rtc mn 1-12 */
	tm->tm_year = bcd2bin(buf[PCF85063_REG_YR]);
	if (tm->tm_year < 70)
		tm->tm_year += 100;	/* assume we are in 1970...2069 */
	/* detect the polarity heuristically. see note above. */
	pcf85063->c_polarity = (buf[PCF85063_REG_MO] & PCF85063_MO_C) ?
		(tm->tm_year >= 100) : (tm->tm_year < 100);

	/* the clock can give out invalid datetime, but we cannot return
	 * -EINVAL otherwise hwclock will refuse to set the time on bootup.
	 */
	if (rtc_valid_tm(tm) < 0)
		dev_err(&client->dev, "retrieved date/time is not valid.\n");
=======
static struct i2c_driver pcf85063_driver;

static int pcf85063_stop_clock(struct i2c_client *client, u8 *ctrl1)
{
	s32 ret;

	ret = i2c_smbus_read_byte_data(client, PCF85063_REG_CTRL1);
	if (ret < 0) {
		dev_err(&client->dev, "Failing to stop the clock\n");
		return -EIO;
	}

	/* stop the clock */
	ret |= PCF85063_REG_CTRL1_STOP;

	ret = i2c_smbus_write_byte_data(client, PCF85063_REG_CTRL1, ret);
	if (ret < 0) {
		dev_err(&client->dev, "Failing to stop the clock\n");
		return -EIO;
	}

	*ctrl1 = ret;
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int pcf85063_set_datetime(struct i2c_client *client, struct rtc_time *tm)
{
	int i = 0, err = 0;
	unsigned char buf[11];

	/* Control & status */
	buf[PCF85063_REG_CTRL1] = 0;
	buf[PCF85063_REG_CTRL2] = 5;

	/* hours, minutes and seconds */
	buf[PCF85063_REG_SC] = bin2bcd(tm->tm_sec) & 0x7F;

	buf[PCF85063_REG_MN] = bin2bcd(tm->tm_min);
	buf[PCF85063_REG_HR] = bin2bcd(tm->tm_hour);

	/* Day of month, 1 - 31 */
	buf[PCF85063_REG_DM] = bin2bcd(tm->tm_mday);

	/* Day, 0 - 6 */
	buf[PCF85063_REG_DW] = tm->tm_wday & 0x07;

	/* month, 1 - 12 */
	buf[PCF85063_REG_MO] = bin2bcd(tm->tm_mon + 1);

	/* year and century */
	buf[PCF85063_REG_YR] = bin2bcd(tm->tm_year % 100);

	/* write register's data */
	for (i = 0; i < sizeof(buf); i++) {
		unsigned char data[2] = { i, buf[i] };

		err = i2c_master_send(client, data, sizeof(data));
		if (err != sizeof(data)) {
			dev_err(&client->dev, "%s: err=%d addr=%02x, data=%02x\n",
					__func__, err, data[0], data[1]);
			return -EIO;
		}
	}

=======
static int pcf85063_start_clock(struct i2c_client *client, u8 ctrl1)
{
	s32 ret;

	/* start the clock */
	ctrl1 &= PCF85063_REG_CTRL1_STOP;

	ret = i2c_smbus_write_byte_data(client, PCF85063_REG_CTRL1, ctrl1);
	if (ret < 0) {
		dev_err(&client->dev, "Failing to start the clock\n");
		return -EIO;
	}

	return 0;
}

static int pcf85063_get_datetime(struct i2c_client *client, struct rtc_time *tm)
{
	int rc;
	u8 regs[7];

	/*
	 * while reading, the time/date registers are blocked and not updated
	 * anymore until the access is finished. To not lose a second
	 * event, the access must be finished within one second. So, read all
	 * time/date registers in one turn.
	 */
	rc = i2c_smbus_read_i2c_block_data(client, PCF85063_REG_SC,
					   sizeof(regs), regs);
	if (rc != sizeof(regs)) {
		dev_err(&client->dev, "date/time register read error\n");
		return -EIO;
	}

	/* if the clock has lost its power it makes no sense to use its time */
	if (regs[0] & PCF85063_REG_SC_OS) {
		dev_warn(&client->dev, "Power loss detected, invalid time\n");
		return -EINVAL;
	}

	tm->tm_sec = bcd2bin(regs[0] & 0x7F);
	tm->tm_min = bcd2bin(regs[1] & 0x7F);
	tm->tm_hour = bcd2bin(regs[2] & 0x3F); /* rtc hr 0-23 */
	tm->tm_mday = bcd2bin(regs[3] & 0x3F);
	tm->tm_wday = regs[4] & 0x07;
	tm->tm_mon = bcd2bin(regs[5] & 0x1F) - 1; /* rtc mn 1-12 */
	tm->tm_year = bcd2bin(regs[6]);
	tm->tm_year += 100;

	return rtc_valid_tm(tm);
}

static int pcf85063_set_datetime(struct i2c_client *client, struct rtc_time *tm)
{
	int rc;
	u8 regs[7];
	u8 ctrl1;

	if ((tm->tm_year < 100) || (tm->tm_year > 199))
		return -EINVAL;

	/*
	 * to accurately set the time, reset the divider chain and keep it in
	 * reset state until all time/date registers are written
	 */
	rc = pcf85063_stop_clock(client, &ctrl1);
	if (rc != 0)
		return rc;

	/* hours, minutes and seconds */
	regs[0] = bin2bcd(tm->tm_sec) & 0x7F; /* clear OS flag */

	regs[1] = bin2bcd(tm->tm_min);
	regs[2] = bin2bcd(tm->tm_hour);

	/* Day of month, 1 - 31 */
	regs[3] = bin2bcd(tm->tm_mday);

	/* Day, 0 - 6 */
	regs[4] = tm->tm_wday & 0x07;

	/* month, 1 - 12 */
	regs[5] = bin2bcd(tm->tm_mon + 1);

	/* year and century */
	regs[6] = bin2bcd(tm->tm_year - 100);

	/* write all registers at once */
	rc = i2c_smbus_write_i2c_block_data(client, PCF85063_REG_SC,
					    sizeof(regs), regs);
	if (rc < 0) {
		dev_err(&client->dev, "date/time register write error\n");
		return rc;
	}

	/*
	 * Write the control register as a separate action since the size of
	 * the register space is different between the PCF85063TP and
	 * PCF85063A devices.  The rollover point can not be used.
	 */
	rc = pcf85063_start_clock(client, ctrl1);
	if (rc != 0)
		return rc;

>>>>>>> v4.9.227
	return 0;
}

static int pcf85063_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	return pcf85063_get_datetime(to_i2c_client(dev), tm);
}

static int pcf85063_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	return pcf85063_set_datetime(to_i2c_client(dev), tm);
}

static const struct rtc_class_ops pcf85063_rtc_ops = {
	.read_time	= pcf85063_rtc_read_time,
	.set_time	= pcf85063_rtc_set_time
};

static int pcf85063_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
<<<<<<< HEAD
	struct pcf85063 *pcf85063;
=======
	struct rtc_device *rtc;
>>>>>>> v4.9.227

	dev_dbg(&client->dev, "%s\n", __func__);

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENODEV;

<<<<<<< HEAD
	pcf85063 = devm_kzalloc(&client->dev, sizeof(struct pcf85063),
				GFP_KERNEL);
	if (!pcf85063)
		return -ENOMEM;

	dev_info(&client->dev, "chip found, driver version " DRV_VERSION "\n");

	i2c_set_clientdata(client, pcf85063);

	pcf85063->rtc = devm_rtc_device_register(&client->dev,
				pcf85063_driver.driver.name,
				&pcf85063_rtc_ops, THIS_MODULE);

	return PTR_ERR_OR_ZERO(pcf85063->rtc);
=======
	rtc = devm_rtc_device_register(&client->dev,
				       pcf85063_driver.driver.name,
				       &pcf85063_rtc_ops, THIS_MODULE);

	return PTR_ERR_OR_ZERO(rtc);
>>>>>>> v4.9.227
}

static const struct i2c_device_id pcf85063_id[] = {
	{ "pcf85063", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, pcf85063_id);

#ifdef CONFIG_OF
static const struct of_device_id pcf85063_of_match[] = {
	{ .compatible = "nxp,pcf85063" },
	{}
};
MODULE_DEVICE_TABLE(of, pcf85063_of_match);
#endif

static struct i2c_driver pcf85063_driver = {
	.driver		= {
		.name	= "rtc-pcf85063",
<<<<<<< HEAD
		.owner	= THIS_MODULE,
=======
>>>>>>> v4.9.227
		.of_match_table = of_match_ptr(pcf85063_of_match),
	},
	.probe		= pcf85063_probe,
	.id_table	= pcf85063_id,
};

module_i2c_driver(pcf85063_driver);

MODULE_AUTHOR("Søren Andersen <san@rosetechnology.dk>");
MODULE_DESCRIPTION("PCF85063 RTC driver");
MODULE_LICENSE("GPL");
<<<<<<< HEAD
MODULE_VERSION(DRV_VERSION);
=======
>>>>>>> v4.9.227
