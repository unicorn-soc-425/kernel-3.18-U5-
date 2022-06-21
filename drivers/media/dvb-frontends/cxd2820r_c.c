/*
 * Sony CXD2820R demodulator driver
 *
 * Copyright (C) 2010 Antti Palosaari <crope@iki.fi>
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
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include "cxd2820r_priv.h"

int cxd2820r_set_frontend_c(struct dvb_frontend *fe)
{
	struct cxd2820r_priv *priv = fe->demodulator_priv;
<<<<<<< HEAD
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int ret, i;
	u8 buf[2];
	u32 if_freq;
	u16 if_ctl;
	u64 num;
=======
	struct i2c_client *client = priv->client[0];
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int ret;
	unsigned int utmp;
	u8 buf[2];
	u32 if_frequency;
>>>>>>> v4.9.227
	struct reg_val_mask tab[] = {
		{ 0x00080, 0x01, 0xff },
		{ 0x00081, 0x05, 0xff },
		{ 0x00085, 0x07, 0xff },
		{ 0x00088, 0x01, 0xff },

		{ 0x00082, 0x20, 0x60 },
		{ 0x1016a, 0x48, 0xff },
		{ 0x100a5, 0x00, 0x01 },
		{ 0x10020, 0x06, 0x07 },
		{ 0x10059, 0x50, 0xff },
		{ 0x10087, 0x0c, 0x3c },
		{ 0x1008b, 0x07, 0xff },
<<<<<<< HEAD
		{ 0x1001f, priv->cfg.if_agc_polarity << 7, 0x80 },
		{ 0x10070, priv->cfg.ts_mode, 0xff },
		{ 0x10071, !priv->cfg.ts_clock_inv << 4, 0x10 },
	};

	dev_dbg(&priv->i2c->dev, "%s: frequency=%d symbol_rate=%d\n", __func__,
			c->frequency, c->symbol_rate);
=======
		{ 0x1001f, priv->if_agc_polarity << 7, 0x80 },
		{ 0x10070, priv->ts_mode, 0xff },
		{ 0x10071, !priv->ts_clk_inv << 4, 0x10 },
	};

	dev_dbg(&client->dev,
		"delivery_system=%d modulation=%d frequency=%u symbol_rate=%u inversion=%d\n",
		c->delivery_system, c->modulation, c->frequency,
		c->symbol_rate, c->inversion);
>>>>>>> v4.9.227

	/* program tuner */
	if (fe->ops.tuner_ops.set_params)
		fe->ops.tuner_ops.set_params(fe);

	if (priv->delivery_system !=  SYS_DVBC_ANNEX_A) {
<<<<<<< HEAD
		for (i = 0; i < ARRAY_SIZE(tab); i++) {
			ret = cxd2820r_wr_reg_mask(priv, tab[i].reg,
				tab[i].val, tab[i].mask);
			if (ret)
				goto error;
		}
=======
		ret = cxd2820r_wr_reg_val_mask_tab(priv, tab, ARRAY_SIZE(tab));
		if (ret)
			goto error;
>>>>>>> v4.9.227
	}

	priv->delivery_system = SYS_DVBC_ANNEX_A;
	priv->ber_running = false; /* tune stops BER counter */

	/* program IF frequency */
	if (fe->ops.tuner_ops.get_if_frequency) {
<<<<<<< HEAD
		ret = fe->ops.tuner_ops.get_if_frequency(fe, &if_freq);
		if (ret)
			goto error;
	} else
		if_freq = 0;

	dev_dbg(&priv->i2c->dev, "%s: if_freq=%d\n", __func__, if_freq);

	num = if_freq / 1000; /* Hz => kHz */
	num *= 0x4000;
	if_ctl = 0x4000 - cxd2820r_div_u64_round_closest(num, 41000);
	buf[0] = (if_ctl >> 8) & 0x3f;
	buf[1] = (if_ctl >> 0) & 0xff;

	ret = cxd2820r_wr_regs(priv, 0x10042, buf, 2);
	if (ret)
		goto error;

	ret = cxd2820r_wr_reg(priv, 0x000ff, 0x08);
	if (ret)
		goto error;

	ret = cxd2820r_wr_reg(priv, 0x000fe, 0x01);
=======
		ret = fe->ops.tuner_ops.get_if_frequency(fe, &if_frequency);
		if (ret)
			goto error;
		dev_dbg(&client->dev, "if_frequency=%u\n", if_frequency);
	} else {
		ret = -EINVAL;
		goto error;
	}

	utmp = 0x4000 - DIV_ROUND_CLOSEST_ULL((u64)if_frequency * 0x4000, CXD2820R_CLK);
	buf[0] = (utmp >> 8) & 0xff;
	buf[1] = (utmp >> 0) & 0xff;
	ret = regmap_bulk_write(priv->regmap[1], 0x0042, buf, 2);
	if (ret)
		goto error;

	ret = regmap_write(priv->regmap[0], 0x00ff, 0x08);
	if (ret)
		goto error;

	ret = regmap_write(priv->regmap[0], 0x00fe, 0x01);
>>>>>>> v4.9.227
	if (ret)
		goto error;

	return ret;
error:
<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}

int cxd2820r_get_frontend_c(struct dvb_frontend *fe)
{
	struct cxd2820r_priv *priv = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int ret;
	u8 buf[2];

	ret = cxd2820r_rd_regs(priv, 0x1001a, buf, 2);
=======
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

int cxd2820r_get_frontend_c(struct dvb_frontend *fe,
			    struct dtv_frontend_properties *c)
{
	struct cxd2820r_priv *priv = fe->demodulator_priv;
	struct i2c_client *client = priv->client[0];
	int ret;
	unsigned int utmp;
	u8 buf[2];

	dev_dbg(&client->dev, "\n");

	ret = regmap_bulk_read(priv->regmap[1], 0x001a, buf, 2);
>>>>>>> v4.9.227
	if (ret)
		goto error;

	c->symbol_rate = 2500 * ((buf[0] & 0x0f) << 8 | buf[1]);

<<<<<<< HEAD
	ret = cxd2820r_rd_reg(priv, 0x10019, &buf[0]);
	if (ret)
		goto error;

	switch ((buf[0] >> 0) & 0x07) {
=======
	ret = regmap_read(priv->regmap[1], 0x0019, &utmp);
	if (ret)
		goto error;

	switch ((utmp >> 0) & 0x07) {
>>>>>>> v4.9.227
	case 0:
		c->modulation = QAM_16;
		break;
	case 1:
		c->modulation = QAM_32;
		break;
	case 2:
		c->modulation = QAM_64;
		break;
	case 3:
		c->modulation = QAM_128;
		break;
	case 4:
		c->modulation = QAM_256;
		break;
	}

<<<<<<< HEAD
	switch ((buf[0] >> 7) & 0x01) {
=======
	switch ((utmp >> 7) & 0x01) {
>>>>>>> v4.9.227
	case 0:
		c->inversion = INVERSION_OFF;
		break;
	case 1:
		c->inversion = INVERSION_ON;
		break;
	}

	return ret;
error:
<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}

int cxd2820r_read_ber_c(struct dvb_frontend *fe, u32 *ber)
{
	struct cxd2820r_priv *priv = fe->demodulator_priv;
	int ret;
	u8 buf[3], start_ber = 0;
	*ber = 0;

	if (priv->ber_running) {
		ret = cxd2820r_rd_regs(priv, 0x10076, buf, sizeof(buf));
		if (ret)
			goto error;

		if ((buf[2] >> 7) & 0x01 || (buf[2] >> 4) & 0x01) {
			*ber = (buf[2] & 0x0f) << 16 | buf[1] << 8 | buf[0];
			start_ber = 1;
		}
	} else {
		priv->ber_running = true;
		start_ber = 1;
	}

	if (start_ber) {
		/* (re)start BER */
		ret = cxd2820r_wr_reg(priv, 0x10079, 0x01);
		if (ret)
			goto error;
	}

	return ret;
error:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}

int cxd2820r_read_signal_strength_c(struct dvb_frontend *fe,
	u16 *strength)
{
	struct cxd2820r_priv *priv = fe->demodulator_priv;
	int ret;
	u8 buf[2];
	u16 tmp;

	ret = cxd2820r_rd_regs(priv, 0x10049, buf, sizeof(buf));
	if (ret)
		goto error;

	tmp = (buf[0] & 0x03) << 8 | buf[1];
	tmp = (~tmp & 0x03ff);

	if (tmp == 512)
		/* ~no signal */
		tmp = 0;
	else if (tmp > 350)
		tmp = 350;

	/* scale value to 0x0000-0xffff */
	*strength = tmp * 0xffff / (350-0);

	return ret;
error:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}

int cxd2820r_read_snr_c(struct dvb_frontend *fe, u16 *snr)
{
	struct cxd2820r_priv *priv = fe->demodulator_priv;
	int ret;
	u8 tmp;
	unsigned int A, B;
	/* report SNR in dB * 10 */

	ret = cxd2820r_rd_reg(priv, 0x10019, &tmp);
	if (ret)
		goto error;

	if (((tmp >> 0) & 0x03) % 2) {
		A = 875;
		B = 650;
	} else {
		A = 950;
		B = 760;
	}

	ret = cxd2820r_rd_reg(priv, 0x1004d, &tmp);
	if (ret)
		goto error;

	#define CXD2820R_LOG2_E_24 24204406 /* log2(e) << 24 */
	if (tmp)
		*snr = A * (intlog2(B / tmp) >> 5) / (CXD2820R_LOG2_E_24 >> 5)
			/ 10;
	else
		*snr = 0;

	return ret;
error:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}

int cxd2820r_read_ucblocks_c(struct dvb_frontend *fe, u32 *ucblocks)
{
	*ucblocks = 0;
	/* no way to read ? */
	return 0;
}

int cxd2820r_read_status_c(struct dvb_frontend *fe, fe_status_t *status)
{
	struct cxd2820r_priv *priv = fe->demodulator_priv;
	int ret;
	u8 buf[2];
	*status = 0;

	ret = cxd2820r_rd_regs(priv, 0x10088, buf, sizeof(buf));
	if (ret)
		goto error;

	if (((buf[0] >> 0) & 0x01) == 1) {
		*status |= FE_HAS_SIGNAL | FE_HAS_CARRIER |
			FE_HAS_VITERBI | FE_HAS_SYNC;

		if (((buf[1] >> 3) & 0x01) == 1) {
			*status |= FE_HAS_SIGNAL | FE_HAS_CARRIER |
				FE_HAS_VITERBI | FE_HAS_SYNC | FE_HAS_LOCK;
		}
	}

	dev_dbg(&priv->i2c->dev, "%s: lock=%02x %02x\n", __func__, buf[0],
			buf[1]);

	return ret;
error:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
=======
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

int cxd2820r_read_status_c(struct dvb_frontend *fe, enum fe_status *status)
{
	struct cxd2820r_priv *priv = fe->demodulator_priv;
	struct i2c_client *client = priv->client[0];
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int ret;
	unsigned int utmp, utmp1, utmp2;
	u8 buf[3];

	/* Lock detection */
	ret = regmap_bulk_read(priv->regmap[1], 0x0088, &buf[0], 1);
	if (ret)
		goto error;
	ret = regmap_bulk_read(priv->regmap[1], 0x0073, &buf[1], 1);
	if (ret)
		goto error;

	utmp1 = (buf[0] >> 0) & 0x01;
	utmp2 = (buf[1] >> 3) & 0x01;

	if (utmp1 == 1 && utmp2 == 1) {
		*status = FE_HAS_SIGNAL | FE_HAS_CARRIER |
			  FE_HAS_VITERBI | FE_HAS_SYNC | FE_HAS_LOCK;
	} else if (utmp1 == 1 || utmp2 == 1) {
		*status = FE_HAS_SIGNAL | FE_HAS_CARRIER |
			  FE_HAS_VITERBI | FE_HAS_SYNC;
	} else {
		*status = 0;
	}

	dev_dbg(&client->dev, "status=%02x raw=%*ph sync=%u ts=%u\n",
		*status, 2, buf, utmp1, utmp2);

	/* Signal strength */
	if (*status & FE_HAS_SIGNAL) {
		unsigned int strength;

		ret = regmap_bulk_read(priv->regmap[1], 0x0049, buf, 2);
		if (ret)
			goto error;

		utmp = buf[0] << 8 | buf[1] << 0;
		utmp = 511 - sign_extend32(utmp, 9);
		/* Scale value to 0x0000-0xffff */
		strength = utmp << 6 | utmp >> 4;

		c->strength.len = 1;
		c->strength.stat[0].scale = FE_SCALE_RELATIVE;
		c->strength.stat[0].uvalue = strength;
	} else {
		c->strength.len = 1;
		c->strength.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	}

	/* CNR */
	if (*status & FE_HAS_VITERBI) {
		unsigned int cnr, const_a, const_b;

		ret = regmap_read(priv->regmap[1], 0x0019, &utmp);
		if (ret)
			goto error;

		if (((utmp >> 0) & 0x03) % 2) {
			const_a = 8750;
			const_b = 650;
		} else {
			const_a = 9500;
			const_b = 760;
		}

		ret = regmap_read(priv->regmap[1], 0x004d, &utmp);
		if (ret)
			goto error;

		#define CXD2820R_LOG2_E_24 24204406 /* log2(e) << 24 */
		if (utmp)
			cnr = div_u64((u64)(intlog2(const_b) - intlog2(utmp))
				      * const_a, CXD2820R_LOG2_E_24);
		else
			cnr = 0;

		c->cnr.len = 1;
		c->cnr.stat[0].scale = FE_SCALE_DECIBEL;
		c->cnr.stat[0].svalue = cnr;
	} else {
		c->cnr.len = 1;
		c->cnr.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	}

	/* BER */
	if (*status & FE_HAS_SYNC) {
		unsigned int post_bit_error;
		bool start_ber;

		if (priv->ber_running) {
			ret = regmap_bulk_read(priv->regmap[1], 0x0076, buf, 3);
			if (ret)
				goto error;

			if ((buf[2] >> 7) & 0x01) {
				post_bit_error = buf[2] << 16 | buf[1] << 8 |
						 buf[0] << 0;
				post_bit_error &= 0x0fffff;
				start_ber = true;
			} else {
				post_bit_error = 0;
				start_ber = false;
			}
		} else {
			post_bit_error = 0;
			start_ber = true;
		}

		if (start_ber) {
			ret = regmap_write(priv->regmap[1], 0x0079, 0x01);
			if (ret)
				goto error;
			priv->ber_running = true;
		}

		priv->post_bit_error += post_bit_error;

		c->post_bit_error.len = 1;
		c->post_bit_error.stat[0].scale = FE_SCALE_COUNTER;
		c->post_bit_error.stat[0].uvalue = priv->post_bit_error;
	} else {
		c->post_bit_error.len = 1;
		c->post_bit_error.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	}

	return ret;
error:
	dev_dbg(&client->dev, "failed=%d\n", ret);
>>>>>>> v4.9.227
	return ret;
}

int cxd2820r_init_c(struct dvb_frontend *fe)
{
	struct cxd2820r_priv *priv = fe->demodulator_priv;
<<<<<<< HEAD
	int ret;

	ret = cxd2820r_wr_reg(priv, 0x00085, 0x07);
=======
	struct i2c_client *client = priv->client[0];
	int ret;

	dev_dbg(&client->dev, "\n");

	ret = regmap_write(priv->regmap[0], 0x0085, 0x07);
>>>>>>> v4.9.227
	if (ret)
		goto error;

	return ret;
error:
<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
=======
	dev_dbg(&client->dev, "failed=%d\n", ret);
>>>>>>> v4.9.227
	return ret;
}

int cxd2820r_sleep_c(struct dvb_frontend *fe)
{
	struct cxd2820r_priv *priv = fe->demodulator_priv;
<<<<<<< HEAD
	int ret, i;
=======
	struct i2c_client *client = priv->client[0];
	int ret;
>>>>>>> v4.9.227
	struct reg_val_mask tab[] = {
		{ 0x000ff, 0x1f, 0xff },
		{ 0x00085, 0x00, 0xff },
		{ 0x00088, 0x01, 0xff },
		{ 0x00081, 0x00, 0xff },
		{ 0x00080, 0x00, 0xff },
	};

<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s\n", __func__);

	priv->delivery_system = SYS_UNDEFINED;

	for (i = 0; i < ARRAY_SIZE(tab); i++) {
		ret = cxd2820r_wr_reg_mask(priv, tab[i].reg, tab[i].val,
			tab[i].mask);
		if (ret)
			goto error;
	}

	return ret;
error:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
=======
	dev_dbg(&client->dev, "\n");

	priv->delivery_system = SYS_UNDEFINED;

	ret = cxd2820r_wr_reg_val_mask_tab(priv, tab, ARRAY_SIZE(tab));
	if (ret)
		goto error;

	return ret;
error:
	dev_dbg(&client->dev, "failed=%d\n", ret);
>>>>>>> v4.9.227
	return ret;
}

int cxd2820r_get_tune_settings_c(struct dvb_frontend *fe,
	struct dvb_frontend_tune_settings *s)
{
	s->min_delay_ms = 500;
	s->step_size = 0; /* no zigzag */
	s->max_drift = 0;

	return 0;
}
