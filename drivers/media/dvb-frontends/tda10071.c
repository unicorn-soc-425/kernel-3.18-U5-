/*
 * NXP TDA10071 + Conexant CX24118A DVB-S/S2 demodulator + tuner driver
 *
 * Copyright (C) 2011 Antti Palosaari <crope@iki.fi>
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

#include "tda10071_priv.h"

<<<<<<< HEAD
/* Max transfer size done by I2C transfer functions */
#define MAX_XFER_SIZE  64

static struct dvb_frontend_ops tda10071_ops;

/* write multiple registers */
static int tda10071_wr_regs(struct tda10071_priv *priv, u8 reg, u8 *val,
	int len)
{
	int ret;
	u8 buf[MAX_XFER_SIZE];
	struct i2c_msg msg[1] = {
		{
			.addr = priv->cfg.demod_i2c_addr,
			.flags = 0,
			.len = 1 + len,
			.buf = buf,
		}
	};

	if (1 + len > sizeof(buf)) {
		dev_warn(&priv->i2c->dev,
				"%s: i2c wr reg=%04x: len=%d is too big!\n",
				KBUILD_MODNAME, reg, len);
		return -EINVAL;
	}

	buf[0] = reg;
	memcpy(&buf[1], val, len);

	ret = i2c_transfer(priv->i2c, msg, 1);
	if (ret == 1) {
		ret = 0;
	} else {
		dev_warn(&priv->i2c->dev,
				"%s: i2c wr failed=%d reg=%02x len=%d\n",
				KBUILD_MODNAME, ret, reg, len);
		ret = -EREMOTEIO;
	}
	return ret;
}

/* read multiple registers */
static int tda10071_rd_regs(struct tda10071_priv *priv, u8 reg, u8 *val,
	int len)
{
	int ret;
	u8 buf[MAX_XFER_SIZE];
	struct i2c_msg msg[2] = {
		{
			.addr = priv->cfg.demod_i2c_addr,
			.flags = 0,
			.len = 1,
			.buf = &reg,
		}, {
			.addr = priv->cfg.demod_i2c_addr,
			.flags = I2C_M_RD,
			.len = len,
			.buf = buf,
		}
	};

	if (len > sizeof(buf)) {
		dev_warn(&priv->i2c->dev,
				"%s: i2c wr reg=%04x: len=%d is too big!\n",
				KBUILD_MODNAME, reg, len);
		return -EINVAL;
	}

	ret = i2c_transfer(priv->i2c, msg, 2);
	if (ret == 2) {
		memcpy(val, buf, len);
		ret = 0;
	} else {
		dev_warn(&priv->i2c->dev,
				"%s: i2c rd failed=%d reg=%02x len=%d\n",
				KBUILD_MODNAME, ret, reg, len);
		ret = -EREMOTEIO;
	}
	return ret;
}

/* write single register */
static int tda10071_wr_reg(struct tda10071_priv *priv, u8 reg, u8 val)
{
	return tda10071_wr_regs(priv, reg, &val, 1);
}

/* read single register */
static int tda10071_rd_reg(struct tda10071_priv *priv, u8 reg, u8 *val)
{
	return tda10071_rd_regs(priv, reg, val, 1);
}

/* write single register with mask */
static int tda10071_wr_reg_mask(struct tda10071_priv *priv,
=======
static struct dvb_frontend_ops tda10071_ops;

/*
 * XXX: regmap_update_bits() does not fit our needs as it does not support
 * partially volatile registers. Also it performs register read even mask is as
 * wide as register value.
 */
/* write single register with mask */
static int tda10071_wr_reg_mask(struct tda10071_dev *dev,
>>>>>>> v4.9.227
				u8 reg, u8 val, u8 mask)
{
	int ret;
	u8 tmp;

	/* no need for read if whole reg is written */
	if (mask != 0xff) {
<<<<<<< HEAD
		ret = tda10071_rd_regs(priv, reg, &tmp, 1);
=======
		ret = regmap_bulk_read(dev->regmap, reg, &tmp, 1);
>>>>>>> v4.9.227
		if (ret)
			return ret;

		val &= mask;
		tmp &= ~mask;
		val |= tmp;
	}

<<<<<<< HEAD
	return tda10071_wr_regs(priv, reg, &val, 1);
}

/* read single register with mask */
static int tda10071_rd_reg_mask(struct tda10071_priv *priv,
				u8 reg, u8 *val, u8 mask)
{
	int ret, i;
	u8 tmp;

	ret = tda10071_rd_regs(priv, reg, &tmp, 1);
	if (ret)
		return ret;

	tmp &= mask;

	/* find position of the first bit */
	for (i = 0; i < 8; i++) {
		if ((mask >> i) & 0x01)
			break;
	}
	*val = tmp >> i;

	return 0;
}

/* execute firmware command */
static int tda10071_cmd_execute(struct tda10071_priv *priv,
	struct tda10071_cmd *cmd)
{
	int ret, i;
	u8 tmp;

	if (!priv->warm) {
=======
	return regmap_bulk_write(dev->regmap, reg, &val, 1);
}

/* execute firmware command */
static int tda10071_cmd_execute(struct tda10071_dev *dev,
	struct tda10071_cmd *cmd)
{
	struct i2c_client *client = dev->client;
	int ret, i;
	unsigned int uitmp;

	if (!dev->warm) {
>>>>>>> v4.9.227
		ret = -EFAULT;
		goto error;
	}

<<<<<<< HEAD
	/* write cmd and args for firmware */
	ret = tda10071_wr_regs(priv, 0x00, cmd->args, cmd->len);
	if (ret)
		goto error;

	/* start cmd execution */
	ret = tda10071_wr_reg(priv, 0x1f, 1);
	if (ret)
		goto error;

	/* wait cmd execution terminate */
	for (i = 1000, tmp = 1; i && tmp; i--) {
		ret = tda10071_rd_reg(priv, 0x1f, &tmp);
		if (ret)
			goto error;
=======
	mutex_lock(&dev->cmd_execute_mutex);

	/* write cmd and args for firmware */
	ret = regmap_bulk_write(dev->regmap, 0x00, cmd->args, cmd->len);
	if (ret)
		goto error_mutex_unlock;

	/* start cmd execution */
	ret = regmap_write(dev->regmap, 0x1f, 1);
	if (ret)
		goto error_mutex_unlock;

	/* wait cmd execution terminate */
	for (i = 1000, uitmp = 1; i && uitmp; i--) {
		ret = regmap_read(dev->regmap, 0x1f, &uitmp);
		if (ret)
			goto error_mutex_unlock;
>>>>>>> v4.9.227

		usleep_range(200, 5000);
	}

<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: loop=%d\n", __func__, i);
=======
	mutex_unlock(&dev->cmd_execute_mutex);
	dev_dbg(&client->dev, "loop=%d\n", i);
>>>>>>> v4.9.227

	if (i == 0) {
		ret = -ETIMEDOUT;
		goto error;
	}

	return ret;
<<<<<<< HEAD
error:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
=======
error_mutex_unlock:
	mutex_unlock(&dev->cmd_execute_mutex);
error:
	dev_dbg(&client->dev, "failed=%d\n", ret);
>>>>>>> v4.9.227
	return ret;
}

static int tda10071_set_tone(struct dvb_frontend *fe,
<<<<<<< HEAD
	fe_sec_tone_mode_t fe_sec_tone_mode)
{
	struct tda10071_priv *priv = fe->demodulator_priv;
=======
	enum fe_sec_tone_mode fe_sec_tone_mode)
{
	struct tda10071_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
>>>>>>> v4.9.227
	struct tda10071_cmd cmd;
	int ret;
	u8 tone;

<<<<<<< HEAD
	if (!priv->warm) {
=======
	if (!dev->warm) {
>>>>>>> v4.9.227
		ret = -EFAULT;
		goto error;
	}

<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: tone_mode=%d\n", __func__,
			fe_sec_tone_mode);
=======
	dev_dbg(&client->dev, "tone_mode=%d\n", fe_sec_tone_mode);
>>>>>>> v4.9.227

	switch (fe_sec_tone_mode) {
	case SEC_TONE_ON:
		tone = 1;
		break;
	case SEC_TONE_OFF:
		tone = 0;
		break;
	default:
<<<<<<< HEAD
		dev_dbg(&priv->i2c->dev, "%s: invalid fe_sec_tone_mode\n",
				__func__);
=======
		dev_dbg(&client->dev, "invalid fe_sec_tone_mode\n");
>>>>>>> v4.9.227
		ret = -EINVAL;
		goto error;
	}

	cmd.args[0] = CMD_LNB_PCB_CONFIG;
	cmd.args[1] = 0;
	cmd.args[2] = 0x00;
	cmd.args[3] = 0x00;
	cmd.args[4] = tone;
	cmd.len = 5;
<<<<<<< HEAD
	ret = tda10071_cmd_execute(priv, &cmd);
=======
	ret = tda10071_cmd_execute(dev, &cmd);
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

static int tda10071_set_voltage(struct dvb_frontend *fe,
<<<<<<< HEAD
	fe_sec_voltage_t fe_sec_voltage)
{
	struct tda10071_priv *priv = fe->demodulator_priv;
=======
	enum fe_sec_voltage fe_sec_voltage)
{
	struct tda10071_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
>>>>>>> v4.9.227
	struct tda10071_cmd cmd;
	int ret;
	u8 voltage;

<<<<<<< HEAD
	if (!priv->warm) {
=======
	if (!dev->warm) {
>>>>>>> v4.9.227
		ret = -EFAULT;
		goto error;
	}

<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: voltage=%d\n", __func__, fe_sec_voltage);
=======
	dev_dbg(&client->dev, "voltage=%d\n", fe_sec_voltage);
>>>>>>> v4.9.227

	switch (fe_sec_voltage) {
	case SEC_VOLTAGE_13:
		voltage = 0;
		break;
	case SEC_VOLTAGE_18:
		voltage = 1;
		break;
	case SEC_VOLTAGE_OFF:
		voltage = 0;
		break;
	default:
<<<<<<< HEAD
		dev_dbg(&priv->i2c->dev, "%s: invalid fe_sec_voltage\n",
				__func__);
=======
		dev_dbg(&client->dev, "invalid fe_sec_voltage\n");
>>>>>>> v4.9.227
		ret = -EINVAL;
		goto error;
	}

	cmd.args[0] = CMD_LNB_SET_DC_LEVEL;
	cmd.args[1] = 0;
	cmd.args[2] = voltage;
	cmd.len = 3;
<<<<<<< HEAD
	ret = tda10071_cmd_execute(priv, &cmd);
=======
	ret = tda10071_cmd_execute(dev, &cmd);
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

static int tda10071_diseqc_send_master_cmd(struct dvb_frontend *fe,
	struct dvb_diseqc_master_cmd *diseqc_cmd)
{
<<<<<<< HEAD
	struct tda10071_priv *priv = fe->demodulator_priv;
	struct tda10071_cmd cmd;
	int ret, i;
	u8 tmp;

	if (!priv->warm) {
=======
	struct tda10071_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
	struct tda10071_cmd cmd;
	int ret, i;
	unsigned int uitmp;

	if (!dev->warm) {
>>>>>>> v4.9.227
		ret = -EFAULT;
		goto error;
	}

<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: msg_len=%d\n", __func__,
			diseqc_cmd->msg_len);
=======
	dev_dbg(&client->dev, "msg_len=%d\n", diseqc_cmd->msg_len);
>>>>>>> v4.9.227

	if (diseqc_cmd->msg_len < 3 || diseqc_cmd->msg_len > 6) {
		ret = -EINVAL;
		goto error;
	}

	/* wait LNB TX */
<<<<<<< HEAD
	for (i = 500, tmp = 0; i && !tmp; i--) {
		ret = tda10071_rd_reg_mask(priv, 0x47, &tmp, 0x01);
		if (ret)
			goto error;

		usleep_range(10000, 20000);
	}

	dev_dbg(&priv->i2c->dev, "%s: loop=%d\n", __func__, i);
=======
	for (i = 500, uitmp = 0; i && !uitmp; i--) {
		ret = regmap_read(dev->regmap, 0x47, &uitmp);
		if (ret)
			goto error;
		uitmp = (uitmp >> 0) & 1;
		usleep_range(10000, 20000);
	}

	dev_dbg(&client->dev, "loop=%d\n", i);
>>>>>>> v4.9.227

	if (i == 0) {
		ret = -ETIMEDOUT;
		goto error;
	}

<<<<<<< HEAD
	ret = tda10071_wr_reg_mask(priv, 0x47, 0x00, 0x01);
=======
	ret = regmap_update_bits(dev->regmap, 0x47, 0x01, 0x00);
>>>>>>> v4.9.227
	if (ret)
		goto error;

	cmd.args[0] = CMD_LNB_SEND_DISEQC;
	cmd.args[1] = 0;
	cmd.args[2] = 0;
	cmd.args[3] = 0;
	cmd.args[4] = 2;
	cmd.args[5] = 0;
	cmd.args[6] = diseqc_cmd->msg_len;
	memcpy(&cmd.args[7], diseqc_cmd->msg, diseqc_cmd->msg_len);
	cmd.len = 7 + diseqc_cmd->msg_len;
<<<<<<< HEAD
	ret = tda10071_cmd_execute(priv, &cmd);
=======
	ret = tda10071_cmd_execute(dev, &cmd);
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

static int tda10071_diseqc_recv_slave_reply(struct dvb_frontend *fe,
	struct dvb_diseqc_slave_reply *reply)
{
<<<<<<< HEAD
	struct tda10071_priv *priv = fe->demodulator_priv;
	struct tda10071_cmd cmd;
	int ret, i;
	u8 tmp;

	if (!priv->warm) {
=======
	struct tda10071_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
	struct tda10071_cmd cmd;
	int ret, i;
	unsigned int uitmp;

	if (!dev->warm) {
>>>>>>> v4.9.227
		ret = -EFAULT;
		goto error;
	}

<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s:\n", __func__);

	/* wait LNB RX */
	for (i = 500, tmp = 0; i && !tmp; i--) {
		ret = tda10071_rd_reg_mask(priv, 0x47, &tmp, 0x02);
		if (ret)
			goto error;

		usleep_range(10000, 20000);
	}

	dev_dbg(&priv->i2c->dev, "%s: loop=%d\n", __func__, i);
=======
	dev_dbg(&client->dev, "\n");

	/* wait LNB RX */
	for (i = 500, uitmp = 0; i && !uitmp; i--) {
		ret = regmap_read(dev->regmap, 0x47, &uitmp);
		if (ret)
			goto error;
		uitmp = (uitmp >> 1) & 1;
		usleep_range(10000, 20000);
	}

	dev_dbg(&client->dev, "loop=%d\n", i);
>>>>>>> v4.9.227

	if (i == 0) {
		ret = -ETIMEDOUT;
		goto error;
	}

	/* reply len */
<<<<<<< HEAD
	ret = tda10071_rd_reg(priv, 0x46, &tmp);
	if (ret)
		goto error;

	reply->msg_len = tmp & 0x1f; /* [4:0] */
=======
	ret = regmap_read(dev->regmap, 0x46, &uitmp);
	if (ret)
		goto error;

	reply->msg_len = uitmp & 0x1f; /* [4:0] */
>>>>>>> v4.9.227
	if (reply->msg_len > sizeof(reply->msg))
		reply->msg_len = sizeof(reply->msg); /* truncate API max */

	/* read reply */
	cmd.args[0] = CMD_LNB_UPDATE_REPLY;
	cmd.args[1] = 0;
	cmd.len = 2;
<<<<<<< HEAD
	ret = tda10071_cmd_execute(priv, &cmd);
	if (ret)
		goto error;

	ret = tda10071_rd_regs(priv, cmd.len, reply->msg, reply->msg_len);
=======
	ret = tda10071_cmd_execute(dev, &cmd);
	if (ret)
		goto error;

	ret = regmap_bulk_read(dev->regmap, cmd.len, reply->msg,
			       reply->msg_len);
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

static int tda10071_diseqc_send_burst(struct dvb_frontend *fe,
<<<<<<< HEAD
	fe_sec_mini_cmd_t fe_sec_mini_cmd)
{
	struct tda10071_priv *priv = fe->demodulator_priv;
	struct tda10071_cmd cmd;
	int ret, i;
	u8 tmp, burst;

	if (!priv->warm) {
=======
	enum fe_sec_mini_cmd fe_sec_mini_cmd)
{
	struct tda10071_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
	struct tda10071_cmd cmd;
	int ret, i;
	unsigned int uitmp;
	u8 burst;

	if (!dev->warm) {
>>>>>>> v4.9.227
		ret = -EFAULT;
		goto error;
	}

<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: fe_sec_mini_cmd=%d\n", __func__,
			fe_sec_mini_cmd);
=======
	dev_dbg(&client->dev, "fe_sec_mini_cmd=%d\n", fe_sec_mini_cmd);
>>>>>>> v4.9.227

	switch (fe_sec_mini_cmd) {
	case SEC_MINI_A:
		burst = 0;
		break;
	case SEC_MINI_B:
		burst = 1;
		break;
	default:
<<<<<<< HEAD
		dev_dbg(&priv->i2c->dev, "%s: invalid fe_sec_mini_cmd\n",
				__func__);
=======
		dev_dbg(&client->dev, "invalid fe_sec_mini_cmd\n");
>>>>>>> v4.9.227
		ret = -EINVAL;
		goto error;
	}

	/* wait LNB TX */
<<<<<<< HEAD
	for (i = 500, tmp = 0; i && !tmp; i--) {
		ret = tda10071_rd_reg_mask(priv, 0x47, &tmp, 0x01);
		if (ret)
			goto error;

		usleep_range(10000, 20000);
	}

	dev_dbg(&priv->i2c->dev, "%s: loop=%d\n", __func__, i);
=======
	for (i = 500, uitmp = 0; i && !uitmp; i--) {
		ret = regmap_read(dev->regmap, 0x47, &uitmp);
		if (ret)
			goto error;
		uitmp = (uitmp >> 0) & 1;
		usleep_range(10000, 20000);
	}

	dev_dbg(&client->dev, "loop=%d\n", i);
>>>>>>> v4.9.227

	if (i == 0) {
		ret = -ETIMEDOUT;
		goto error;
	}

<<<<<<< HEAD
	ret = tda10071_wr_reg_mask(priv, 0x47, 0x00, 0x01);
=======
	ret = regmap_update_bits(dev->regmap, 0x47, 0x01, 0x00);
>>>>>>> v4.9.227
	if (ret)
		goto error;

	cmd.args[0] = CMD_LNB_SEND_TONEBURST;
	cmd.args[1] = 0;
	cmd.args[2] = burst;
	cmd.len = 3;
<<<<<<< HEAD
	ret = tda10071_cmd_execute(priv, &cmd);
=======
	ret = tda10071_cmd_execute(dev, &cmd);
>>>>>>> v4.9.227
	if (ret)
		goto error;

	return ret;
error:
<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}

static int tda10071_read_status(struct dvb_frontend *fe, fe_status_t *status)
{
	struct tda10071_priv *priv = fe->demodulator_priv;
	int ret;
	u8 tmp;

	*status = 0;

	if (!priv->warm) {
=======
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

static int tda10071_read_status(struct dvb_frontend *fe, enum fe_status *status)
{
	struct tda10071_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	struct tda10071_cmd cmd;
	int ret;
	unsigned int uitmp;
	u8 buf[8];

	*status = 0;

	if (!dev->warm) {
>>>>>>> v4.9.227
		ret = 0;
		goto error;
	}

<<<<<<< HEAD
	ret = tda10071_rd_reg(priv, 0x39, &tmp);
=======
	ret = regmap_read(dev->regmap, 0x39, &uitmp);
>>>>>>> v4.9.227
	if (ret)
		goto error;

	/* 0x39[0] tuner PLL */
<<<<<<< HEAD
	if (tmp & 0x02) /* demod PLL */
		*status |= FE_HAS_SIGNAL | FE_HAS_CARRIER;
	if (tmp & 0x04) /* viterbi or LDPC*/
		*status |= FE_HAS_VITERBI;
	if (tmp & 0x08) /* RS or BCH */
		*status |= FE_HAS_SYNC | FE_HAS_LOCK;

	priv->fe_status = *status;

	return ret;
error:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
=======
	if (uitmp & 0x02) /* demod PLL */
		*status |= FE_HAS_SIGNAL | FE_HAS_CARRIER;
	if (uitmp & 0x04) /* viterbi or LDPC*/
		*status |= FE_HAS_VITERBI;
	if (uitmp & 0x08) /* RS or BCH */
		*status |= FE_HAS_SYNC | FE_HAS_LOCK;

	dev->fe_status = *status;

	/* signal strength */
	if (dev->fe_status & FE_HAS_SIGNAL) {
		cmd.args[0] = CMD_GET_AGCACC;
		cmd.args[1] = 0;
		cmd.len = 2;
		ret = tda10071_cmd_execute(dev, &cmd);
		if (ret)
			goto error;

		/* input power estimate dBm */
		ret = regmap_read(dev->regmap, 0x50, &uitmp);
		if (ret)
			goto error;

		c->strength.stat[0].scale = FE_SCALE_DECIBEL;
		c->strength.stat[0].svalue = (int) (uitmp - 256) * 1000;
	} else {
		c->strength.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	}

	/* CNR */
	if (dev->fe_status & FE_HAS_VITERBI) {
		/* Es/No */
		ret = regmap_bulk_read(dev->regmap, 0x3a, buf, 2);
		if (ret)
			goto error;

		c->cnr.stat[0].scale = FE_SCALE_DECIBEL;
		c->cnr.stat[0].svalue = (buf[0] << 8 | buf[1] << 0) * 100;
	} else {
		c->cnr.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	}

	/* UCB/PER/BER */
	if (dev->fe_status & FE_HAS_LOCK) {
		/* TODO: report total bits/packets */
		u8 delivery_system, reg, len;

		switch (dev->delivery_system) {
		case SYS_DVBS:
			reg = 0x4c;
			len = 8;
			delivery_system = 1;
			break;
		case SYS_DVBS2:
			reg = 0x4d;
			len = 4;
			delivery_system = 0;
			break;
		default:
			ret = -EINVAL;
			goto error;
		}

		ret = regmap_read(dev->regmap, reg, &uitmp);
		if (ret)
			goto error;

		if (dev->meas_count == uitmp) {
			dev_dbg(&client->dev, "meas not ready=%02x\n", uitmp);
			ret = 0;
			goto error;
		} else {
			dev->meas_count = uitmp;
		}

		cmd.args[0] = CMD_BER_UPDATE_COUNTERS;
		cmd.args[1] = 0;
		cmd.args[2] = delivery_system;
		cmd.len = 3;
		ret = tda10071_cmd_execute(dev, &cmd);
		if (ret)
			goto error;

		ret = regmap_bulk_read(dev->regmap, cmd.len, buf, len);
		if (ret)
			goto error;

		if (dev->delivery_system == SYS_DVBS) {
			dev->dvbv3_ber = buf[0] << 24 | buf[1] << 16 |
					 buf[2] << 8 | buf[3] << 0;
			dev->post_bit_error += buf[0] << 24 | buf[1] << 16 |
					       buf[2] << 8 | buf[3] << 0;
			c->post_bit_error.stat[0].scale = FE_SCALE_COUNTER;
			c->post_bit_error.stat[0].uvalue = dev->post_bit_error;
			dev->block_error += buf[4] << 8 | buf[5] << 0;
			c->block_error.stat[0].scale = FE_SCALE_COUNTER;
			c->block_error.stat[0].uvalue = dev->block_error;
		} else {
			dev->dvbv3_ber = buf[0] << 8 | buf[1] << 0;
			dev->post_bit_error += buf[0] << 8 | buf[1] << 0;
			c->post_bit_error.stat[0].scale = FE_SCALE_COUNTER;
			c->post_bit_error.stat[0].uvalue = dev->post_bit_error;
			c->block_error.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
		}
	} else {
		c->post_bit_error.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
		c->block_error.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	}

	return ret;
error:
	dev_dbg(&client->dev, "failed=%d\n", ret);
>>>>>>> v4.9.227
	return ret;
}

static int tda10071_read_snr(struct dvb_frontend *fe, u16 *snr)
{
<<<<<<< HEAD
	struct tda10071_priv *priv = fe->demodulator_priv;
	int ret;
	u8 buf[2];

	if (!priv->warm || !(priv->fe_status & FE_HAS_LOCK)) {
		*snr = 0;
		ret = 0;
		goto error;
	}

	ret = tda10071_rd_regs(priv, 0x3a, buf, 2);
	if (ret)
		goto error;

	/* Es/No dBx10 */
	*snr = buf[0] << 8 | buf[1];

	return ret;
error:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
=======
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;

	if (c->cnr.stat[0].scale == FE_SCALE_DECIBEL)
		*snr = div_s64(c->cnr.stat[0].svalue, 100);
	else
		*snr = 0;
	return 0;
>>>>>>> v4.9.227
}

static int tda10071_read_signal_strength(struct dvb_frontend *fe, u16 *strength)
{
<<<<<<< HEAD
	struct tda10071_priv *priv = fe->demodulator_priv;
	struct tda10071_cmd cmd;
	int ret;
	u8 tmp;

	if (!priv->warm || !(priv->fe_status & FE_HAS_LOCK)) {
		*strength = 0;
		ret = 0;
		goto error;
	}

	cmd.args[0] = CMD_GET_AGCACC;
	cmd.args[1] = 0;
	cmd.len = 2;
	ret = tda10071_cmd_execute(priv, &cmd);
	if (ret)
		goto error;

	/* input power estimate dBm */
	ret = tda10071_rd_reg(priv, 0x50, &tmp);
	if (ret)
		goto error;

	if (tmp < 181)
		tmp = 181; /* -75 dBm */
	else if (tmp > 236)
		tmp = 236; /* -20 dBm */

	/* scale value to 0x0000-0xffff */
	*strength = (tmp-181) * 0xffff / (236-181);

	return ret;
error:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
=======
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	unsigned int uitmp;

	if (c->strength.stat[0].scale == FE_SCALE_DECIBEL) {
		uitmp = div_s64(c->strength.stat[0].svalue, 1000) + 256;
		uitmp = clamp(uitmp, 181U, 236U); /* -75dBm - -20dBm */
		/* scale value to 0x0000-0xffff */
		*strength = (uitmp-181) * 0xffff / (236-181);
	} else {
		*strength = 0;
	}
	return 0;
>>>>>>> v4.9.227
}

static int tda10071_read_ber(struct dvb_frontend *fe, u32 *ber)
{
<<<<<<< HEAD
	struct tda10071_priv *priv = fe->demodulator_priv;
	struct tda10071_cmd cmd;
	int ret, i, len;
	u8 tmp, reg, buf[8];

	if (!priv->warm || !(priv->fe_status & FE_HAS_LOCK)) {
		*ber = priv->ber = 0;
		ret = 0;
		goto error;
	}

	switch (priv->delivery_system) {
	case SYS_DVBS:
		reg = 0x4c;
		len = 8;
		i = 1;
		break;
	case SYS_DVBS2:
		reg = 0x4d;
		len = 4;
		i = 0;
		break;
	default:
		*ber = priv->ber = 0;
		return 0;
	}

	ret = tda10071_rd_reg(priv, reg, &tmp);
	if (ret)
		goto error;

	if (priv->meas_count[i] == tmp) {
		dev_dbg(&priv->i2c->dev, "%s: meas not ready=%02x\n", __func__,
				tmp);
		*ber = priv->ber;
		return 0;
	} else {
		priv->meas_count[i] = tmp;
	}

	cmd.args[0] = CMD_BER_UPDATE_COUNTERS;
	cmd.args[1] = 0;
	cmd.args[2] = i;
	cmd.len = 3;
	ret = tda10071_cmd_execute(priv, &cmd);
	if (ret)
		goto error;

	ret = tda10071_rd_regs(priv, cmd.len, buf, len);
	if (ret)
		goto error;

	if (priv->delivery_system == SYS_DVBS) {
		*ber = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
		priv->ucb += (buf[4] << 8) | buf[5];
	} else {
		*ber = (buf[0] << 8) | buf[1];
	}
	priv->ber = *ber;

	return ret;
error:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
=======
	struct tda10071_dev *dev = fe->demodulator_priv;

	*ber = dev->dvbv3_ber;
	return 0;
>>>>>>> v4.9.227
}

static int tda10071_read_ucblocks(struct dvb_frontend *fe, u32 *ucblocks)
{
<<<<<<< HEAD
	struct tda10071_priv *priv = fe->demodulator_priv;
	int ret = 0;

	if (!priv->warm || !(priv->fe_status & FE_HAS_LOCK)) {
		*ucblocks = 0;
		goto error;
	}

	/* UCB is updated when BER is read. Assume BER is read anyway. */

	*ucblocks = priv->ucb;

	return ret;
error:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
=======
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;

	if (c->block_error.stat[0].scale == FE_SCALE_COUNTER)
		*ucblocks = c->block_error.stat[0].uvalue;
	else
		*ucblocks = 0;
	return 0;
>>>>>>> v4.9.227
}

static int tda10071_set_frontend(struct dvb_frontend *fe)
{
<<<<<<< HEAD
	struct tda10071_priv *priv = fe->demodulator_priv;
=======
	struct tda10071_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
>>>>>>> v4.9.227
	struct tda10071_cmd cmd;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int ret, i;
	u8 mode, rolloff, pilot, inversion, div;
<<<<<<< HEAD
	fe_modulation_t modulation;

	dev_dbg(&priv->i2c->dev,
			"%s: delivery_system=%d modulation=%d frequency=%d symbol_rate=%d inversion=%d pilot=%d rolloff=%d\n",
			__func__, c->delivery_system, c->modulation,
			c->frequency, c->symbol_rate, c->inversion, c->pilot,
			c->rolloff);

	priv->delivery_system = SYS_UNDEFINED;

	if (!priv->warm) {
=======
	enum fe_modulation modulation;

	dev_dbg(&client->dev,
		"delivery_system=%d modulation=%d frequency=%u symbol_rate=%d inversion=%d pilot=%d rolloff=%d\n",
		c->delivery_system, c->modulation, c->frequency, c->symbol_rate,
		c->inversion, c->pilot, c->rolloff);

	dev->delivery_system = SYS_UNDEFINED;

	if (!dev->warm) {
>>>>>>> v4.9.227
		ret = -EFAULT;
		goto error;
	}

	switch (c->inversion) {
	case INVERSION_OFF:
		inversion = 1;
		break;
	case INVERSION_ON:
		inversion = 0;
		break;
	case INVERSION_AUTO:
		/* 2 = auto; try first on then off
		 * 3 = auto; try first off then on */
		inversion = 3;
		break;
	default:
<<<<<<< HEAD
		dev_dbg(&priv->i2c->dev, "%s: invalid inversion\n", __func__);
=======
		dev_dbg(&client->dev, "invalid inversion\n");
>>>>>>> v4.9.227
		ret = -EINVAL;
		goto error;
	}

	switch (c->delivery_system) {
	case SYS_DVBS:
		modulation = QPSK;
		rolloff = 0;
		pilot = 2;
		break;
	case SYS_DVBS2:
		modulation = c->modulation;

		switch (c->rolloff) {
		case ROLLOFF_20:
			rolloff = 2;
			break;
		case ROLLOFF_25:
			rolloff = 1;
			break;
		case ROLLOFF_35:
			rolloff = 0;
			break;
		case ROLLOFF_AUTO:
		default:
<<<<<<< HEAD
			dev_dbg(&priv->i2c->dev, "%s: invalid rolloff\n",
					__func__);
=======
			dev_dbg(&client->dev, "invalid rolloff\n");
>>>>>>> v4.9.227
			ret = -EINVAL;
			goto error;
		}

		switch (c->pilot) {
		case PILOT_OFF:
			pilot = 0;
			break;
		case PILOT_ON:
			pilot = 1;
			break;
		case PILOT_AUTO:
			pilot = 2;
			break;
		default:
<<<<<<< HEAD
			dev_dbg(&priv->i2c->dev, "%s: invalid pilot\n",
					__func__);
=======
			dev_dbg(&client->dev, "invalid pilot\n");
>>>>>>> v4.9.227
			ret = -EINVAL;
			goto error;
		}
		break;
	default:
<<<<<<< HEAD
		dev_dbg(&priv->i2c->dev, "%s: invalid delivery_system\n",
				__func__);
=======
		dev_dbg(&client->dev, "invalid delivery_system\n");
>>>>>>> v4.9.227
		ret = -EINVAL;
		goto error;
	}

	for (i = 0, mode = 0xff; i < ARRAY_SIZE(TDA10071_MODCOD); i++) {
		if (c->delivery_system == TDA10071_MODCOD[i].delivery_system &&
			modulation == TDA10071_MODCOD[i].modulation &&
			c->fec_inner == TDA10071_MODCOD[i].fec) {
			mode = TDA10071_MODCOD[i].val;
<<<<<<< HEAD
			dev_dbg(&priv->i2c->dev, "%s: mode found=%02x\n",
					__func__, mode);
=======
			dev_dbg(&client->dev, "mode found=%02x\n", mode);
>>>>>>> v4.9.227
			break;
		}
	}

	if (mode == 0xff) {
<<<<<<< HEAD
		dev_dbg(&priv->i2c->dev, "%s: invalid parameter combination\n",
				__func__);
=======
		dev_dbg(&client->dev, "invalid parameter combination\n");
>>>>>>> v4.9.227
		ret = -EINVAL;
		goto error;
	}

	if (c->symbol_rate <= 5000000)
		div = 14;
	else
		div = 4;

<<<<<<< HEAD
	ret = tda10071_wr_reg(priv, 0x81, div);
	if (ret)
		goto error;

	ret = tda10071_wr_reg(priv, 0xe3, div);
=======
	ret = regmap_write(dev->regmap, 0x81, div);
	if (ret)
		goto error;

	ret = regmap_write(dev->regmap, 0xe3, div);
>>>>>>> v4.9.227
	if (ret)
		goto error;

	cmd.args[0] = CMD_CHANGE_CHANNEL;
	cmd.args[1] = 0;
	cmd.args[2] = mode;
	cmd.args[3] = (c->frequency >> 16) & 0xff;
	cmd.args[4] = (c->frequency >>  8) & 0xff;
	cmd.args[5] = (c->frequency >>  0) & 0xff;
	cmd.args[6] = ((c->symbol_rate / 1000) >> 8) & 0xff;
	cmd.args[7] = ((c->symbol_rate / 1000) >> 0) & 0xff;
	cmd.args[8] = (tda10071_ops.info.frequency_tolerance >> 8) & 0xff;
	cmd.args[9] = (tda10071_ops.info.frequency_tolerance >> 0) & 0xff;
	cmd.args[10] = rolloff;
	cmd.args[11] = inversion;
	cmd.args[12] = pilot;
	cmd.args[13] = 0x00;
	cmd.args[14] = 0x00;
	cmd.len = 15;
<<<<<<< HEAD
	ret = tda10071_cmd_execute(priv, &cmd);
	if (ret)
		goto error;

	priv->delivery_system = c->delivery_system;

	return ret;
error:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}

static int tda10071_get_frontend(struct dvb_frontend *fe)
{
	struct tda10071_priv *priv = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int ret, i;
	u8 buf[5], tmp;

	if (!priv->warm || !(priv->fe_status & FE_HAS_LOCK)) {
		ret = -EFAULT;
		goto error;
	}

	ret = tda10071_rd_regs(priv, 0x30, buf, 5);
=======
	ret = tda10071_cmd_execute(dev, &cmd);
	if (ret)
		goto error;

	dev->delivery_system = c->delivery_system;

	return ret;
error:
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

static int tda10071_get_frontend(struct dvb_frontend *fe,
				 struct dtv_frontend_properties *c)
{
	struct tda10071_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
	int ret, i;
	u8 buf[5], tmp;

	if (!dev->warm || !(dev->fe_status & FE_HAS_LOCK)) {
		ret = 0;
		goto error;
	}

	ret = regmap_bulk_read(dev->regmap, 0x30, buf, 5);
>>>>>>> v4.9.227
	if (ret)
		goto error;

	tmp = buf[0] & 0x3f;
	for (i = 0; i < ARRAY_SIZE(TDA10071_MODCOD); i++) {
		if (tmp == TDA10071_MODCOD[i].val) {
			c->modulation = TDA10071_MODCOD[i].modulation;
			c->fec_inner = TDA10071_MODCOD[i].fec;
			c->delivery_system = TDA10071_MODCOD[i].delivery_system;
		}
	}

	switch ((buf[1] >> 0) & 0x01) {
	case 0:
		c->inversion = INVERSION_ON;
		break;
	case 1:
		c->inversion = INVERSION_OFF;
		break;
	}

	switch ((buf[1] >> 7) & 0x01) {
	case 0:
		c->pilot = PILOT_OFF;
		break;
	case 1:
		c->pilot = PILOT_ON;
		break;
	}

	c->frequency = (buf[2] << 16) | (buf[3] << 8) | (buf[4] << 0);

<<<<<<< HEAD
	ret = tda10071_rd_regs(priv, 0x52, buf, 3);
=======
	ret = regmap_bulk_read(dev->regmap, 0x52, buf, 3);
>>>>>>> v4.9.227
	if (ret)
		goto error;

	c->symbol_rate = ((buf[0] << 16) | (buf[1] << 8) | (buf[2] << 0)) * 1000;

	return ret;
error:
<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
=======
	dev_dbg(&client->dev, "failed=%d\n", ret);
>>>>>>> v4.9.227
	return ret;
}

static int tda10071_init(struct dvb_frontend *fe)
{
<<<<<<< HEAD
	struct tda10071_priv *priv = fe->demodulator_priv;
	struct tda10071_cmd cmd;
	int ret, i, len, remaining, fw_size;
=======
	struct tda10071_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	struct tda10071_cmd cmd;
	int ret, i, len, remaining, fw_size;
	unsigned int uitmp;
>>>>>>> v4.9.227
	const struct firmware *fw;
	u8 *fw_file = TDA10071_FIRMWARE;
	u8 tmp, buf[4];
	struct tda10071_reg_val_mask tab[] = {
		{ 0xcd, 0x00, 0x07 },
		{ 0x80, 0x00, 0x02 },
		{ 0xcd, 0x00, 0xc0 },
		{ 0xce, 0x00, 0x1b },
		{ 0x9d, 0x00, 0x01 },
		{ 0x9d, 0x00, 0x02 },
		{ 0x9e, 0x00, 0x01 },
		{ 0x87, 0x00, 0x80 },
		{ 0xce, 0x00, 0x08 },
		{ 0xce, 0x00, 0x10 },
	};
	struct tda10071_reg_val_mask tab2[] = {
		{ 0xf1, 0x70, 0xff },
<<<<<<< HEAD
		{ 0x88, priv->cfg.pll_multiplier, 0x3f },
=======
		{ 0x88, dev->pll_multiplier, 0x3f },
>>>>>>> v4.9.227
		{ 0x89, 0x00, 0x10 },
		{ 0x89, 0x10, 0x10 },
		{ 0xc0, 0x01, 0x01 },
		{ 0xc0, 0x00, 0x01 },
		{ 0xe0, 0xff, 0xff },
		{ 0xe0, 0x00, 0xff },
		{ 0x96, 0x1e, 0x7e },
		{ 0x8b, 0x08, 0x08 },
		{ 0x8b, 0x00, 0x08 },
		{ 0x8f, 0x1a, 0x7e },
		{ 0x8c, 0x68, 0xff },
		{ 0x8d, 0x08, 0xff },
		{ 0x8e, 0x4c, 0xff },
		{ 0x8f, 0x01, 0x01 },
		{ 0x8b, 0x04, 0x04 },
		{ 0x8b, 0x00, 0x04 },
		{ 0x87, 0x05, 0x07 },
		{ 0x80, 0x00, 0x20 },
		{ 0xc8, 0x01, 0xff },
		{ 0xb4, 0x47, 0xff },
		{ 0xb5, 0x9c, 0xff },
		{ 0xb6, 0x7d, 0xff },
		{ 0xba, 0x00, 0x03 },
		{ 0xb7, 0x47, 0xff },
		{ 0xb8, 0x9c, 0xff },
		{ 0xb9, 0x7d, 0xff },
		{ 0xba, 0x00, 0x0c },
		{ 0xc8, 0x00, 0xff },
		{ 0xcd, 0x00, 0x04 },
		{ 0xcd, 0x00, 0x20 },
		{ 0xe8, 0x02, 0xff },
		{ 0xcf, 0x20, 0xff },
		{ 0x9b, 0xd7, 0xff },
		{ 0x9a, 0x01, 0x03 },
		{ 0xa8, 0x05, 0x0f },
		{ 0xa8, 0x65, 0xf0 },
		{ 0xa6, 0xa0, 0xf0 },
		{ 0x9d, 0x50, 0xfc },
		{ 0x9e, 0x20, 0xe0 },
		{ 0xa3, 0x1c, 0x7c },
		{ 0xd5, 0x03, 0x03 },
	};

<<<<<<< HEAD
	if (priv->warm) {
		/* warm state - wake up device from sleep */

		for (i = 0; i < ARRAY_SIZE(tab); i++) {
			ret = tda10071_wr_reg_mask(priv, tab[i].reg,
=======
	if (dev->warm) {
		/* warm state - wake up device from sleep */

		for (i = 0; i < ARRAY_SIZE(tab); i++) {
			ret = tda10071_wr_reg_mask(dev, tab[i].reg,
>>>>>>> v4.9.227
				tab[i].val, tab[i].mask);
			if (ret)
				goto error;
		}

		cmd.args[0] = CMD_SET_SLEEP_MODE;
		cmd.args[1] = 0;
		cmd.args[2] = 0;
		cmd.len = 3;
<<<<<<< HEAD
		ret = tda10071_cmd_execute(priv, &cmd);
=======
		ret = tda10071_cmd_execute(dev, &cmd);
>>>>>>> v4.9.227
		if (ret)
			goto error;
	} else {
		/* cold state - try to download firmware */

		/* request the firmware, this will block and timeout */
<<<<<<< HEAD
		ret = request_firmware(&fw, fw_file, priv->i2c->dev.parent);
		if (ret) {
			dev_err(&priv->i2c->dev,
					"%s: did not find the firmware file. (%s) Please see linux/Documentation/dvb/ for more details on firmware-problems. (%d)\n",
					KBUILD_MODNAME, fw_file, ret);
=======
		ret = request_firmware(&fw, fw_file, &client->dev);
		if (ret) {
			dev_err(&client->dev,
				"did not find the firmware file. (%s) Please see linux/Documentation/dvb/ for more details on firmware-problems. (%d)\n",
				fw_file, ret);
>>>>>>> v4.9.227
			goto error;
		}

		/* init */
		for (i = 0; i < ARRAY_SIZE(tab2); i++) {
<<<<<<< HEAD
			ret = tda10071_wr_reg_mask(priv, tab2[i].reg,
=======
			ret = tda10071_wr_reg_mask(dev, tab2[i].reg,
>>>>>>> v4.9.227
				tab2[i].val, tab2[i].mask);
			if (ret)
				goto error_release_firmware;
		}

		/*  download firmware */
<<<<<<< HEAD
		ret = tda10071_wr_reg(priv, 0xe0, 0x7f);
		if (ret)
			goto error_release_firmware;

		ret = tda10071_wr_reg(priv, 0xf7, 0x81);
		if (ret)
			goto error_release_firmware;

		ret = tda10071_wr_reg(priv, 0xf8, 0x00);
		if (ret)
			goto error_release_firmware;

		ret = tda10071_wr_reg(priv, 0xf9, 0x00);
		if (ret)
			goto error_release_firmware;

		dev_info(&priv->i2c->dev,
				"%s: found a '%s' in cold state, will try to load a firmware\n",
				KBUILD_MODNAME, tda10071_ops.info.name);
		dev_info(&priv->i2c->dev,
				"%s: downloading firmware from file '%s'\n",
				KBUILD_MODNAME, fw_file);
=======
		ret = regmap_write(dev->regmap, 0xe0, 0x7f);
		if (ret)
			goto error_release_firmware;

		ret = regmap_write(dev->regmap, 0xf7, 0x81);
		if (ret)
			goto error_release_firmware;

		ret = regmap_write(dev->regmap, 0xf8, 0x00);
		if (ret)
			goto error_release_firmware;

		ret = regmap_write(dev->regmap, 0xf9, 0x00);
		if (ret)
			goto error_release_firmware;

		dev_info(&client->dev,
			 "found a '%s' in cold state, will try to load a firmware\n",
			 tda10071_ops.info.name);
		dev_info(&client->dev, "downloading firmware from file '%s'\n",
			 fw_file);
>>>>>>> v4.9.227

		/* do not download last byte */
		fw_size = fw->size - 1;

		for (remaining = fw_size; remaining > 0;
<<<<<<< HEAD
			remaining -= (priv->cfg.i2c_wr_max - 1)) {
			len = remaining;
			if (len > (priv->cfg.i2c_wr_max - 1))
				len = (priv->cfg.i2c_wr_max - 1);

			ret = tda10071_wr_regs(priv, 0xfa,
				(u8 *) &fw->data[fw_size - remaining], len);
			if (ret) {
				dev_err(&priv->i2c->dev,
						"%s: firmware download failed=%d\n",
						KBUILD_MODNAME, ret);
=======
			remaining -= (dev->i2c_wr_max - 1)) {
			len = remaining;
			if (len > (dev->i2c_wr_max - 1))
				len = (dev->i2c_wr_max - 1);

			ret = regmap_bulk_write(dev->regmap, 0xfa,
				(u8 *) &fw->data[fw_size - remaining], len);
			if (ret) {
				dev_err(&client->dev,
					"firmware download failed=%d\n", ret);
>>>>>>> v4.9.227
				goto error_release_firmware;
			}
		}
		release_firmware(fw);

<<<<<<< HEAD
		ret = tda10071_wr_reg(priv, 0xf7, 0x0c);
		if (ret)
			goto error;

		ret = tda10071_wr_reg(priv, 0xe0, 0x00);
=======
		ret = regmap_write(dev->regmap, 0xf7, 0x0c);
		if (ret)
			goto error;

		ret = regmap_write(dev->regmap, 0xe0, 0x00);
>>>>>>> v4.9.227
		if (ret)
			goto error;

		/* wait firmware start */
		msleep(250);

		/* firmware status */
<<<<<<< HEAD
		ret = tda10071_rd_reg(priv, 0x51, &tmp);
		if (ret)
			goto error;

		if (tmp) {
			dev_info(&priv->i2c->dev, "%s: firmware did not run\n",
					KBUILD_MODNAME);
			ret = -EFAULT;
			goto error;
		} else {
			priv->warm = true;
=======
		ret = regmap_read(dev->regmap, 0x51, &uitmp);
		if (ret)
			goto error;

		if (uitmp) {
			dev_info(&client->dev, "firmware did not run\n");
			ret = -EFAULT;
			goto error;
		} else {
			dev->warm = true;
>>>>>>> v4.9.227
		}

		cmd.args[0] = CMD_GET_FW_VERSION;
		cmd.len = 1;
<<<<<<< HEAD
		ret = tda10071_cmd_execute(priv, &cmd);
		if (ret)
			goto error;

		ret = tda10071_rd_regs(priv, cmd.len, buf, 4);
		if (ret)
			goto error;

		dev_info(&priv->i2c->dev, "%s: firmware version %d.%d.%d.%d\n",
				KBUILD_MODNAME, buf[0], buf[1], buf[2], buf[3]);
		dev_info(&priv->i2c->dev, "%s: found a '%s' in warm state\n",
				KBUILD_MODNAME, tda10071_ops.info.name);

		ret = tda10071_rd_regs(priv, 0x81, buf, 2);
=======
		ret = tda10071_cmd_execute(dev, &cmd);
		if (ret)
			goto error;

		ret = regmap_bulk_read(dev->regmap, cmd.len, buf, 4);
		if (ret)
			goto error;

		dev_info(&client->dev, "firmware version %d.%d.%d.%d\n",
			 buf[0], buf[1], buf[2], buf[3]);
		dev_info(&client->dev, "found a '%s' in warm state\n",
			 tda10071_ops.info.name);

		ret = regmap_bulk_read(dev->regmap, 0x81, buf, 2);
>>>>>>> v4.9.227
		if (ret)
			goto error;

		cmd.args[0] = CMD_DEMOD_INIT;
<<<<<<< HEAD
		cmd.args[1] = ((priv->cfg.xtal / 1000) >> 8) & 0xff;
		cmd.args[2] = ((priv->cfg.xtal / 1000) >> 0) & 0xff;
		cmd.args[3] = buf[0];
		cmd.args[4] = buf[1];
		cmd.args[5] = priv->cfg.pll_multiplier;
		cmd.args[6] = priv->cfg.spec_inv;
		cmd.args[7] = 0x00;
		cmd.len = 8;
		ret = tda10071_cmd_execute(priv, &cmd);
		if (ret)
			goto error;

		if (priv->cfg.tuner_i2c_addr)
			tmp = priv->cfg.tuner_i2c_addr;
=======
		cmd.args[1] = ((dev->clk / 1000) >> 8) & 0xff;
		cmd.args[2] = ((dev->clk / 1000) >> 0) & 0xff;
		cmd.args[3] = buf[0];
		cmd.args[4] = buf[1];
		cmd.args[5] = dev->pll_multiplier;
		cmd.args[6] = dev->spec_inv;
		cmd.args[7] = 0x00;
		cmd.len = 8;
		ret = tda10071_cmd_execute(dev, &cmd);
		if (ret)
			goto error;

		if (dev->tuner_i2c_addr)
			tmp = dev->tuner_i2c_addr;
>>>>>>> v4.9.227
		else
			tmp = 0x14;

		cmd.args[0] = CMD_TUNER_INIT;
		cmd.args[1] = 0x00;
		cmd.args[2] = 0x00;
		cmd.args[3] = 0x00;
		cmd.args[4] = 0x00;
		cmd.args[5] = tmp;
		cmd.args[6] = 0x00;
		cmd.args[7] = 0x03;
		cmd.args[8] = 0x02;
		cmd.args[9] = 0x02;
		cmd.args[10] = 0x00;
		cmd.args[11] = 0x00;
		cmd.args[12] = 0x00;
		cmd.args[13] = 0x00;
		cmd.args[14] = 0x00;
		cmd.len = 15;
<<<<<<< HEAD
		ret = tda10071_cmd_execute(priv, &cmd);
=======
		ret = tda10071_cmd_execute(dev, &cmd);
>>>>>>> v4.9.227
		if (ret)
			goto error;

		cmd.args[0] = CMD_MPEG_CONFIG;
		cmd.args[1] = 0;
<<<<<<< HEAD
		cmd.args[2] = priv->cfg.ts_mode;
=======
		cmd.args[2] = dev->ts_mode;
>>>>>>> v4.9.227
		cmd.args[3] = 0x00;
		cmd.args[4] = 0x04;
		cmd.args[5] = 0x00;
		cmd.len = 6;
<<<<<<< HEAD
		ret = tda10071_cmd_execute(priv, &cmd);
		if (ret)
			goto error;

		ret = tda10071_wr_reg_mask(priv, 0xf0, 0x01, 0x01);
=======
		ret = tda10071_cmd_execute(dev, &cmd);
		if (ret)
			goto error;

		ret = regmap_update_bits(dev->regmap, 0xf0, 0x01, 0x01);
>>>>>>> v4.9.227
		if (ret)
			goto error;

		cmd.args[0] = CMD_LNB_CONFIG;
		cmd.args[1] = 0;
		cmd.args[2] = 150;
		cmd.args[3] = 3;
		cmd.args[4] = 22;
		cmd.args[5] = 1;
		cmd.args[6] = 1;
		cmd.args[7] = 30;
		cmd.args[8] = 30;
		cmd.args[9] = 30;
		cmd.args[10] = 30;
		cmd.len = 11;
<<<<<<< HEAD
		ret = tda10071_cmd_execute(priv, &cmd);
=======
		ret = tda10071_cmd_execute(dev, &cmd);
>>>>>>> v4.9.227
		if (ret)
			goto error;

		cmd.args[0] = CMD_BER_CONTROL;
		cmd.args[1] = 0;
		cmd.args[2] = 14;
		cmd.args[3] = 14;
		cmd.len = 4;
<<<<<<< HEAD
		ret = tda10071_cmd_execute(priv, &cmd);
=======
		ret = tda10071_cmd_execute(dev, &cmd);
>>>>>>> v4.9.227
		if (ret)
			goto error;
	}

<<<<<<< HEAD
=======
	/* init stats here in order signal app which stats are supported */
	c->strength.len = 1;
	c->strength.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	c->cnr.len = 1;
	c->cnr.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	c->post_bit_error.len = 1;
	c->post_bit_error.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	c->block_error.len = 1;
	c->block_error.stat[0].scale = FE_SCALE_NOT_AVAILABLE;

>>>>>>> v4.9.227
	return ret;
error_release_firmware:
	release_firmware(fw);
error:
<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
=======
	dev_dbg(&client->dev, "failed=%d\n", ret);
>>>>>>> v4.9.227
	return ret;
}

static int tda10071_sleep(struct dvb_frontend *fe)
{
<<<<<<< HEAD
	struct tda10071_priv *priv = fe->demodulator_priv;
=======
	struct tda10071_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
>>>>>>> v4.9.227
	struct tda10071_cmd cmd;
	int ret, i;
	struct tda10071_reg_val_mask tab[] = {
		{ 0xcd, 0x07, 0x07 },
		{ 0x80, 0x02, 0x02 },
		{ 0xcd, 0xc0, 0xc0 },
		{ 0xce, 0x1b, 0x1b },
		{ 0x9d, 0x01, 0x01 },
		{ 0x9d, 0x02, 0x02 },
		{ 0x9e, 0x01, 0x01 },
		{ 0x87, 0x80, 0x80 },
		{ 0xce, 0x08, 0x08 },
		{ 0xce, 0x10, 0x10 },
	};

<<<<<<< HEAD
	if (!priv->warm) {
=======
	if (!dev->warm) {
>>>>>>> v4.9.227
		ret = -EFAULT;
		goto error;
	}

	cmd.args[0] = CMD_SET_SLEEP_MODE;
	cmd.args[1] = 0;
	cmd.args[2] = 1;
	cmd.len = 3;
<<<<<<< HEAD
	ret = tda10071_cmd_execute(priv, &cmd);
=======
	ret = tda10071_cmd_execute(dev, &cmd);
>>>>>>> v4.9.227
	if (ret)
		goto error;

	for (i = 0; i < ARRAY_SIZE(tab); i++) {
<<<<<<< HEAD
		ret = tda10071_wr_reg_mask(priv, tab[i].reg, tab[i].val,
=======
		ret = tda10071_wr_reg_mask(dev, tab[i].reg, tab[i].val,
>>>>>>> v4.9.227
			tab[i].mask);
		if (ret)
			goto error;
	}

	return ret;
error:
<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
=======
	dev_dbg(&client->dev, "failed=%d\n", ret);
>>>>>>> v4.9.227
	return ret;
}

static int tda10071_get_tune_settings(struct dvb_frontend *fe,
	struct dvb_frontend_tune_settings *s)
{
	s->min_delay_ms = 8000;
	s->step_size = 0;
	s->max_drift = 0;

	return 0;
}

<<<<<<< HEAD
static void tda10071_release(struct dvb_frontend *fe)
{
	struct tda10071_priv *priv = fe->demodulator_priv;
	kfree(priv);
}

struct dvb_frontend *tda10071_attach(const struct tda10071_config *config,
	struct i2c_adapter *i2c)
{
	int ret;
	struct tda10071_priv *priv = NULL;
	u8 tmp;

	/* allocate memory for the internal priv */
	priv = kzalloc(sizeof(struct tda10071_priv), GFP_KERNEL);
	if (priv == NULL) {
		ret = -ENOMEM;
		goto error;
	}

	/* make sure demod i2c address is specified */
	if (!config->demod_i2c_addr) {
		dev_dbg(&i2c->dev, "%s: invalid demod i2c address\n", __func__);
		ret = -EINVAL;
		goto error;
	}

	/* make sure tuner i2c address is specified */
	if (!config->tuner_i2c_addr) {
		dev_dbg(&i2c->dev, "%s: invalid tuner i2c address\n", __func__);
		ret = -EINVAL;
		goto error;
	}

	/* setup the priv */
	priv->i2c = i2c;
	memcpy(&priv->cfg, config, sizeof(struct tda10071_config));

	/* chip ID */
	ret = tda10071_rd_reg(priv, 0xff, &tmp);
	if (ret || tmp != 0x0f)
		goto error;

	/* chip type */
	ret = tda10071_rd_reg(priv, 0xdd, &tmp);
	if (ret || tmp != 0x00)
		goto error;

	/* chip version */
	ret = tda10071_rd_reg(priv, 0xfe, &tmp);
	if (ret || tmp != 0x01)
		goto error;

	/* create dvb_frontend */
	memcpy(&priv->fe.ops, &tda10071_ops, sizeof(struct dvb_frontend_ops));
	priv->fe.demodulator_priv = priv;

	return &priv->fe;
error:
	dev_dbg(&i2c->dev, "%s: failed=%d\n", __func__, ret);
	kfree(priv);
	return NULL;
}
EXPORT_SYMBOL(tda10071_attach);

=======
>>>>>>> v4.9.227
static struct dvb_frontend_ops tda10071_ops = {
	.delsys = { SYS_DVBS, SYS_DVBS2 },
	.info = {
		.name = "NXP TDA10071",
		.frequency_min = 950000,
		.frequency_max = 2150000,
		.frequency_tolerance = 5000,
		.symbol_rate_min = 1000000,
		.symbol_rate_max = 45000000,
		.caps = FE_CAN_INVERSION_AUTO |
			FE_CAN_FEC_1_2 |
			FE_CAN_FEC_2_3 |
			FE_CAN_FEC_3_4 |
			FE_CAN_FEC_4_5 |
			FE_CAN_FEC_5_6 |
			FE_CAN_FEC_6_7 |
			FE_CAN_FEC_7_8 |
			FE_CAN_FEC_8_9 |
			FE_CAN_FEC_AUTO |
			FE_CAN_QPSK |
			FE_CAN_RECOVER |
			FE_CAN_2G_MODULATION
	},

<<<<<<< HEAD
	.release = tda10071_release,

=======
>>>>>>> v4.9.227
	.get_tune_settings = tda10071_get_tune_settings,

	.init = tda10071_init,
	.sleep = tda10071_sleep,

	.set_frontend = tda10071_set_frontend,
	.get_frontend = tda10071_get_frontend,

	.read_status = tda10071_read_status,
	.read_snr = tda10071_read_snr,
	.read_signal_strength = tda10071_read_signal_strength,
	.read_ber = tda10071_read_ber,
	.read_ucblocks = tda10071_read_ucblocks,

	.diseqc_send_master_cmd = tda10071_diseqc_send_master_cmd,
	.diseqc_recv_slave_reply = tda10071_diseqc_recv_slave_reply,
	.diseqc_send_burst = tda10071_diseqc_send_burst,

	.set_tone = tda10071_set_tone,
	.set_voltage = tda10071_set_voltage,
};

<<<<<<< HEAD
=======
static struct dvb_frontend *tda10071_get_dvb_frontend(struct i2c_client *client)
{
	struct tda10071_dev *dev = i2c_get_clientdata(client);

	dev_dbg(&client->dev, "\n");

	return &dev->fe;
}

static int tda10071_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct tda10071_dev *dev;
	struct tda10071_platform_data *pdata = client->dev.platform_data;
	int ret;
	unsigned int uitmp;
	static const struct regmap_config regmap_config = {
		.reg_bits = 8,
		.val_bits = 8,
	};

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev) {
		ret = -ENOMEM;
		goto err;
	}

	dev->client = client;
	mutex_init(&dev->cmd_execute_mutex);
	dev->clk = pdata->clk;
	dev->i2c_wr_max = pdata->i2c_wr_max;
	dev->ts_mode = pdata->ts_mode;
	dev->spec_inv = pdata->spec_inv;
	dev->pll_multiplier = pdata->pll_multiplier;
	dev->tuner_i2c_addr = pdata->tuner_i2c_addr;
	dev->regmap = devm_regmap_init_i2c(client, &regmap_config);
	if (IS_ERR(dev->regmap)) {
		ret = PTR_ERR(dev->regmap);
		goto err_kfree;
	}

	/* chip ID */
	ret = regmap_read(dev->regmap, 0xff, &uitmp);
	if (ret)
		goto err_kfree;
	if (uitmp != 0x0f) {
		ret = -ENODEV;
		goto err_kfree;
	}

	/* chip type */
	ret = regmap_read(dev->regmap, 0xdd, &uitmp);
	if (ret)
		goto err_kfree;
	if (uitmp != 0x00) {
		ret = -ENODEV;
		goto err_kfree;
	}

	/* chip version */
	ret = regmap_read(dev->regmap, 0xfe, &uitmp);
	if (ret)
		goto err_kfree;
	if (uitmp != 0x01) {
		ret = -ENODEV;
		goto err_kfree;
	}

	/* create dvb_frontend */
	memcpy(&dev->fe.ops, &tda10071_ops, sizeof(struct dvb_frontend_ops));
	dev->fe.demodulator_priv = dev;
	i2c_set_clientdata(client, dev);

	/* setup callbacks */
	pdata->get_dvb_frontend = tda10071_get_dvb_frontend;

	dev_info(&client->dev, "NXP TDA10071 successfully identified\n");
	return 0;
err_kfree:
	kfree(dev);
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

static int tda10071_remove(struct i2c_client *client)
{
	struct tda10071_dev *dev = i2c_get_clientdata(client);

	dev_dbg(&client->dev, "\n");

	kfree(dev);
	return 0;
}

static const struct i2c_device_id tda10071_id_table[] = {
	{"tda10071_cx24118", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, tda10071_id_table);

static struct i2c_driver tda10071_driver = {
	.driver = {
		.name	= "tda10071",
		.suppress_bind_attrs = true,
	},
	.probe		= tda10071_probe,
	.remove		= tda10071_remove,
	.id_table	= tda10071_id_table,
};

module_i2c_driver(tda10071_driver);

>>>>>>> v4.9.227
MODULE_AUTHOR("Antti Palosaari <crope@iki.fi>");
MODULE_DESCRIPTION("NXP TDA10071 DVB-S/S2 demodulator driver");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE(TDA10071_FIRMWARE);
