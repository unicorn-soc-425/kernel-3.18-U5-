/*
 * I2C multiplexer
 *
 * Copyright (c) 2008-2009 Rodolfo Giometti <giometti@linux.it>
 * Copyright (c) 2008-2009 Eurotech S.p.A. <info@eurotech.it>
 *
 * This module supports the PCA954x series of I2C multiplexer/switch chips
 * made by Philips Semiconductors.
 * This includes the:
 *	 PCA9540, PCA9542, PCA9543, PCA9544, PCA9545, PCA9546, PCA9547
 *	 and PCA9548.
 *
 * These chips are all controlled via the I2C bus itself, and all have a
 * single 8-bit register. The upstream "parent" bus fans out to two,
 * four, or eight downstream busses or channels; which of these
 * are selected is determined by the chip type and register contents. A
 * mux can select only one sub-bus at a time; a switch can select any
 * combination simultaneously.
 *
 * Based on:
 *	pca954x.c from Kumar Gala <galak@kernel.crashing.org>
 * Copyright (C) 2006
 *
 * Based on:
 *	pca954x.c from Ken Harrenstien
 * Copyright (C) 2004 Google, Inc. (Ken Harrenstien)
 *
 * Based on:
 *	i2c-virtual_cb.c from Brian Kuschak <bkuschak@yahoo.com>
 * and
 *	pca9540.c from Jean Delvare <jdelvare@suse.de>.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/device.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/i2c-mux.h>
#include <linux/i2c/pca954x.h>
#include <linux/module.h>
<<<<<<< HEAD
=======
#include <linux/of.h>
#include <linux/of_device.h>
>>>>>>> v4.9.227
#include <linux/pm.h>
#include <linux/slab.h>

#define PCA954X_MAX_NCHANS 8

enum pca_type {
	pca_9540,
	pca_9542,
	pca_9543,
	pca_9544,
	pca_9545,
	pca_9546,
	pca_9547,
	pca_9548,
};

<<<<<<< HEAD
struct pca954x {
	enum pca_type type;
	struct i2c_adapter *virt_adaps[PCA954X_MAX_NCHANS];

	u8 last_chan;		/* last register value */
};

=======
>>>>>>> v4.9.227
struct chip_desc {
	u8 nchans;
	u8 enable;	/* used for muxes only */
	enum muxtype {
		pca954x_ismux = 0,
		pca954x_isswi
	} muxtype;
};

<<<<<<< HEAD
=======
struct pca954x {
	const struct chip_desc *chip;

	u8 last_chan;		/* last register value */
	u8 deselect;
	struct i2c_client *client;
};

>>>>>>> v4.9.227
/* Provide specs for the PCA954x types we know about */
static const struct chip_desc chips[] = {
	[pca_9540] = {
		.nchans = 2,
		.enable = 0x4,
		.muxtype = pca954x_ismux,
	},
	[pca_9543] = {
		.nchans = 2,
		.muxtype = pca954x_isswi,
	},
	[pca_9544] = {
		.nchans = 4,
		.enable = 0x4,
		.muxtype = pca954x_ismux,
	},
	[pca_9545] = {
		.nchans = 4,
		.muxtype = pca954x_isswi,
	},
	[pca_9547] = {
		.nchans = 8,
		.enable = 0x8,
		.muxtype = pca954x_ismux,
	},
	[pca_9548] = {
		.nchans = 8,
		.muxtype = pca954x_isswi,
	},
};

static const struct i2c_device_id pca954x_id[] = {
	{ "pca9540", pca_9540 },
	{ "pca9542", pca_9540 },
	{ "pca9543", pca_9543 },
	{ "pca9544", pca_9544 },
	{ "pca9545", pca_9545 },
	{ "pca9546", pca_9545 },
	{ "pca9547", pca_9547 },
	{ "pca9548", pca_9548 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, pca954x_id);

<<<<<<< HEAD
=======
#ifdef CONFIG_OF
static const struct of_device_id pca954x_of_match[] = {
	{ .compatible = "nxp,pca9540", .data = &chips[pca_9540] },
	{ .compatible = "nxp,pca9542", .data = &chips[pca_9542] },
	{ .compatible = "nxp,pca9543", .data = &chips[pca_9543] },
	{ .compatible = "nxp,pca9544", .data = &chips[pca_9544] },
	{ .compatible = "nxp,pca9545", .data = &chips[pca_9545] },
	{ .compatible = "nxp,pca9546", .data = &chips[pca_9546] },
	{ .compatible = "nxp,pca9547", .data = &chips[pca_9547] },
	{ .compatible = "nxp,pca9548", .data = &chips[pca_9548] },
	{}
};
#endif

>>>>>>> v4.9.227
/* Write to mux register. Don't use i2c_transfer()/i2c_smbus_xfer()
   for this as they will try to lock adapter a second time */
static int pca954x_reg_write(struct i2c_adapter *adap,
			     struct i2c_client *client, u8 val)
{
	int ret = -ENODEV;

	if (adap->algo->master_xfer) {
		struct i2c_msg msg;
		char buf[1];

		msg.addr = client->addr;
		msg.flags = 0;
		msg.len = 1;
		buf[0] = val;
		msg.buf = buf;
<<<<<<< HEAD
		ret = adap->algo->master_xfer(adap, &msg, 1);
=======
		ret = __i2c_transfer(adap, &msg, 1);

		if (ret >= 0 && ret != 1)
			ret = -EREMOTEIO;
>>>>>>> v4.9.227
	} else {
		union i2c_smbus_data data;
		ret = adap->algo->smbus_xfer(adap, client->addr,
					     client->flags,
					     I2C_SMBUS_WRITE,
					     val, I2C_SMBUS_BYTE, &data);
	}

	return ret;
}

<<<<<<< HEAD
static int pca954x_select_chan(struct i2c_adapter *adap,
			       void *client, u32 chan)
{
	struct pca954x *data = i2c_get_clientdata(client);
	const struct chip_desc *chip = &chips[data->type];
=======
static int pca954x_select_chan(struct i2c_mux_core *muxc, u32 chan)
{
	struct pca954x *data = i2c_mux_priv(muxc);
	struct i2c_client *client = data->client;
	const struct chip_desc *chip = data->chip;
>>>>>>> v4.9.227
	u8 regval;
	int ret = 0;

	/* we make switches look like muxes, not sure how to be smarter */
	if (chip->muxtype == pca954x_ismux)
		regval = chan | chip->enable;
	else
		regval = 1 << chan;

	/* Only select the channel if its different from the last channel */
	if (data->last_chan != regval) {
<<<<<<< HEAD
		ret = pca954x_reg_write(adap, client, regval);
		data->last_chan = regval;
=======
		ret = pca954x_reg_write(muxc->parent, client, regval);
		data->last_chan = ret < 0 ? 0 : regval;
>>>>>>> v4.9.227
	}

	return ret;
}

<<<<<<< HEAD
static int pca954x_deselect_mux(struct i2c_adapter *adap,
				void *client, u32 chan)
{
	struct pca954x *data = i2c_get_clientdata(client);

	/* Deselect active channel */
	data->last_chan = 0;
	return pca954x_reg_write(adap, client, data->last_chan);
=======
static int pca954x_deselect_mux(struct i2c_mux_core *muxc, u32 chan)
{
	struct pca954x *data = i2c_mux_priv(muxc);
	struct i2c_client *client = data->client;

	if (!(data->deselect & (1 << chan)))
		return 0;

	/* Deselect active channel */
	data->last_chan = 0;
	return pca954x_reg_write(muxc->parent, client, data->last_chan);
>>>>>>> v4.9.227
}

/*
 * I2C init/probing/exit functions
 */
static int pca954x_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct i2c_adapter *adap = to_i2c_adapter(client->dev.parent);
	struct pca954x_platform_data *pdata = dev_get_platdata(&client->dev);
<<<<<<< HEAD
	struct gpio_desc *gpio;
	int num, force, class;
	struct pca954x *data;
=======
	struct device_node *of_node = client->dev.of_node;
	bool idle_disconnect_dt;
	struct gpio_desc *gpio;
	int num, force, class;
	struct i2c_mux_core *muxc;
	struct pca954x *data;
	const struct of_device_id *match;
>>>>>>> v4.9.227
	int ret;

	if (!i2c_check_functionality(adap, I2C_FUNC_SMBUS_BYTE))
		return -ENODEV;

<<<<<<< HEAD
	data = devm_kzalloc(&client->dev, sizeof(struct pca954x), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	i2c_set_clientdata(client, data);

	/* Get the mux out of reset if a reset GPIO is specified. */
	gpio = devm_gpiod_get(&client->dev, "reset");
	if (!IS_ERR(gpio))
		gpiod_direction_output(gpio, 0);
=======
	muxc = i2c_mux_alloc(adap, &client->dev,
			     PCA954X_MAX_NCHANS, sizeof(*data), 0,
			     pca954x_select_chan, pca954x_deselect_mux);
	if (!muxc)
		return -ENOMEM;
	data = i2c_mux_priv(muxc);

	i2c_set_clientdata(client, muxc);
	data->client = client;

	/* Get the mux out of reset if a reset GPIO is specified. */
	gpio = devm_gpiod_get_optional(&client->dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(gpio))
		return PTR_ERR(gpio);
>>>>>>> v4.9.227

	/* Write the mux register at addr to verify
	 * that the mux is in fact present. This also
	 * initializes the mux to disconnected state.
	 */
	if (i2c_smbus_write_byte(client, 0) < 0) {
		dev_warn(&client->dev, "probe failed\n");
		return -ENODEV;
	}

<<<<<<< HEAD
	data->type = id->driver_data;
	data->last_chan = 0;		   /* force the first selection */

	/* Now create an adapter for each channel */
	for (num = 0; num < chips[data->type].nchans; num++) {
=======
	match = of_match_device(of_match_ptr(pca954x_of_match), &client->dev);
	if (match)
		data->chip = of_device_get_match_data(&client->dev);
	else
		data->chip = &chips[id->driver_data];

	data->last_chan = 0;		   /* force the first selection */

	idle_disconnect_dt = of_node &&
		of_property_read_bool(of_node, "i2c-mux-idle-disconnect");

	/* Now create an adapter for each channel */
	for (num = 0; num < data->chip->nchans; num++) {
		bool idle_disconnect_pd = false;

>>>>>>> v4.9.227
		force = 0;			  /* dynamic adap number */
		class = 0;			  /* no class by default */
		if (pdata) {
			if (num < pdata->num_modes) {
				/* force static number */
				force = pdata->modes[num].adap_id;
				class = pdata->modes[num].class;
			} else
				/* discard unconfigured channels */
				break;
<<<<<<< HEAD
		}

		data->virt_adaps[num] =
			i2c_add_mux_adapter(adap, &client->dev, client,
				force, num, class, pca954x_select_chan,
				(pdata && pdata->modes[num].deselect_on_exit)
					? pca954x_deselect_mux : NULL);

		if (data->virt_adaps[num] == NULL) {
			ret = -ENODEV;
=======
			idle_disconnect_pd = pdata->modes[num].deselect_on_exit;
		}
		data->deselect |= (idle_disconnect_pd ||
				   idle_disconnect_dt) << num;

		ret = i2c_mux_add_adapter(muxc, force, num, class);

		if (ret) {
>>>>>>> v4.9.227
			dev_err(&client->dev,
				"failed to register multiplexed adapter"
				" %d as bus %d\n", num, force);
			goto virt_reg_failed;
		}
	}

	dev_info(&client->dev,
		 "registered %d multiplexed busses for I2C %s %s\n",
<<<<<<< HEAD
		 num, chips[data->type].muxtype == pca954x_ismux
=======
		 num, data->chip->muxtype == pca954x_ismux
>>>>>>> v4.9.227
				? "mux" : "switch", client->name);

	return 0;

virt_reg_failed:
<<<<<<< HEAD
	for (num--; num >= 0; num--)
		i2c_del_mux_adapter(data->virt_adaps[num]);
=======
	i2c_mux_del_adapters(muxc);
>>>>>>> v4.9.227
	return ret;
}

static int pca954x_remove(struct i2c_client *client)
{
<<<<<<< HEAD
	struct pca954x *data = i2c_get_clientdata(client);
	const struct chip_desc *chip = &chips[data->type];
	int i;

	for (i = 0; i < chip->nchans; ++i)
		if (data->virt_adaps[i]) {
			i2c_del_mux_adapter(data->virt_adaps[i]);
			data->virt_adaps[i] = NULL;
		}

=======
	struct i2c_mux_core *muxc = i2c_get_clientdata(client);

	i2c_mux_del_adapters(muxc);
>>>>>>> v4.9.227
	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int pca954x_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
<<<<<<< HEAD
	struct pca954x *data = i2c_get_clientdata(client);
=======
	struct i2c_mux_core *muxc = i2c_get_clientdata(client);
	struct pca954x *data = i2c_mux_priv(muxc);
>>>>>>> v4.9.227

	data->last_chan = 0;
	return i2c_smbus_write_byte(client, 0);
}
#endif

static SIMPLE_DEV_PM_OPS(pca954x_pm, NULL, pca954x_resume);

static struct i2c_driver pca954x_driver = {
	.driver		= {
		.name	= "pca954x",
		.pm	= &pca954x_pm,
<<<<<<< HEAD
		.owner	= THIS_MODULE,
=======
		.of_match_table = of_match_ptr(pca954x_of_match),
>>>>>>> v4.9.227
	},
	.probe		= pca954x_probe,
	.remove		= pca954x_remove,
	.id_table	= pca954x_id,
};

module_i2c_driver(pca954x_driver);

MODULE_AUTHOR("Rodolfo Giometti <giometti@linux.it>");
MODULE_DESCRIPTION("PCA954x I2C mux/switch driver");
MODULE_LICENSE("GPL v2");
