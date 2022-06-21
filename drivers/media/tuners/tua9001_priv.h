/*
<<<<<<< HEAD
 * Infineon TUA 9001 silicon tuner driver
=======
 * Infineon TUA9001 silicon tuner driver
>>>>>>> v4.9.227
 *
 * Copyright (C) 2009 Antti Palosaari <crope@iki.fi>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
<<<<<<< HEAD
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
=======
>>>>>>> v4.9.227
 */

#ifndef TUA9001_PRIV_H
#define TUA9001_PRIV_H

<<<<<<< HEAD
struct reg_val {
=======
#include "tua9001.h"
#include <linux/math64.h>
#include <linux/regmap.h>

struct tua9001_reg_val {
>>>>>>> v4.9.227
	u8 reg;
	u16 val;
};

<<<<<<< HEAD
struct tua9001_priv {
	struct tua9001_config *cfg;
	struct i2c_adapter *i2c;
=======
struct tua9001_dev {
	struct dvb_frontend *fe;
	struct i2c_client *client;
	struct regmap *regmap;
>>>>>>> v4.9.227
};

#endif
