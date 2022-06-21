/*
 * Realtek RTL2832 DVB-T demodulator driver
 *
 * Copyright (C) 2012 Thomas Mair <thomas.mair86@gmail.com>
<<<<<<< HEAD
=======
 * Copyright (C) 2012-2014 Antti Palosaari <crope@iki.fi>
>>>>>>> v4.9.227
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "rtl2832_priv.h"
<<<<<<< HEAD
#include "dvb_math.h"
#include <linux/bitops.h>

/* Max transfer size done by I2C transfer functions */
#define MAX_XFER_SIZE  64
#define REG_MASK(b) (BIT(b + 1) - 1)

static const struct rtl2832_reg_entry registers[] = {
	[DVBT_SOFT_RST]		= {0x1, 0x1,   2, 2},
	[DVBT_IIC_REPEAT]	= {0x1, 0x1,   3, 3},
	[DVBT_TR_WAIT_MIN_8K]	= {0x1, 0x88, 11, 2},
	[DVBT_RSD_BER_FAIL_VAL]	= {0x1, 0x8f, 15, 0},
	[DVBT_EN_BK_TRK]	= {0x1, 0xa6,  7, 7},
	[DVBT_AD_EN_REG]	= {0x0, 0x8,   7, 7},
	[DVBT_AD_EN_REG1]	= {0x0, 0x8,   6, 6},
	[DVBT_EN_BBIN]		= {0x1, 0xb1,  0, 0},
	[DVBT_MGD_THD0]		= {0x1, 0x95,  7, 0},
	[DVBT_MGD_THD1]		= {0x1, 0x96,  7, 0},
	[DVBT_MGD_THD2]		= {0x1, 0x97,  7, 0},
	[DVBT_MGD_THD3]		= {0x1, 0x98,  7, 0},
	[DVBT_MGD_THD4]		= {0x1, 0x99,  7, 0},
	[DVBT_MGD_THD5]		= {0x1, 0x9a,  7, 0},
	[DVBT_MGD_THD6]		= {0x1, 0x9b,  7, 0},
	[DVBT_MGD_THD7]		= {0x1, 0x9c,  7, 0},
	[DVBT_EN_CACQ_NOTCH]	= {0x1, 0x61,  4, 4},
	[DVBT_AD_AV_REF]	= {0x0, 0x9,   6, 0},
	[DVBT_REG_PI]		= {0x0, 0xa,   2, 0},
	[DVBT_PIP_ON]		= {0x0, 0x21,  3, 3},
	[DVBT_SCALE1_B92]	= {0x2, 0x92,  7, 0},
	[DVBT_SCALE1_B93]	= {0x2, 0x93,  7, 0},
	[DVBT_SCALE1_BA7]	= {0x2, 0xa7,  7, 0},
	[DVBT_SCALE1_BA9]	= {0x2, 0xa9,  7, 0},
	[DVBT_SCALE1_BAA]	= {0x2, 0xaa,  7, 0},
	[DVBT_SCALE1_BAB]	= {0x2, 0xab,  7, 0},
	[DVBT_SCALE1_BAC]	= {0x2, 0xac,  7, 0},
	[DVBT_SCALE1_BB0]	= {0x2, 0xb0,  7, 0},
	[DVBT_SCALE1_BB1]	= {0x2, 0xb1,  7, 0},
	[DVBT_KB_P1]		= {0x1, 0x64,  3, 1},
	[DVBT_KB_P2]		= {0x1, 0x64,  6, 4},
	[DVBT_KB_P3]		= {0x1, 0x65,  2, 0},
	[DVBT_OPT_ADC_IQ]	= {0x0, 0x6,   5, 4},
	[DVBT_AD_AVI]		= {0x0, 0x9,   1, 0},
	[DVBT_AD_AVQ]		= {0x0, 0x9,   3, 2},
	[DVBT_K1_CR_STEP12]	= {0x2, 0xad,  9, 4},
	[DVBT_TRK_KS_P2]	= {0x1, 0x6f,  2, 0},
	[DVBT_TRK_KS_I2]	= {0x1, 0x70,  5, 3},
	[DVBT_TR_THD_SET2]	= {0x1, 0x72,  3, 0},
	[DVBT_TRK_KC_P2]	= {0x1, 0x73,  5, 3},
	[DVBT_TRK_KC_I2]	= {0x1, 0x75,  2, 0},
	[DVBT_CR_THD_SET2]	= {0x1, 0x76,  7, 6},
	[DVBT_PSET_IFFREQ]	= {0x1, 0x19, 21, 0},
	[DVBT_SPEC_INV]		= {0x1, 0x15,  0, 0},
	[DVBT_RSAMP_RATIO]	= {0x1, 0x9f, 27, 2},
	[DVBT_CFREQ_OFF_RATIO]	= {0x1, 0x9d, 23, 4},
	[DVBT_FSM_STAGE]	= {0x3, 0x51,  6, 3},
	[DVBT_RX_CONSTEL]	= {0x3, 0x3c,  3, 2},
	[DVBT_RX_HIER]		= {0x3, 0x3c,  6, 4},
	[DVBT_RX_C_RATE_LP]	= {0x3, 0x3d,  2, 0},
	[DVBT_RX_C_RATE_HP]	= {0x3, 0x3d,  5, 3},
	[DVBT_GI_IDX]		= {0x3, 0x51,  1, 0},
	[DVBT_FFT_MODE_IDX]	= {0x3, 0x51,  2, 2},
	[DVBT_RSD_BER_EST]	= {0x3, 0x4e, 15, 0},
	[DVBT_CE_EST_EVM]	= {0x4, 0xc,  15, 0},
	[DVBT_RF_AGC_VAL]	= {0x3, 0x5b, 13, 0},
	[DVBT_IF_AGC_VAL]	= {0x3, 0x59, 13, 0},
	[DVBT_DAGC_VAL]		= {0x3, 0x5,   7, 0},
	[DVBT_SFREQ_OFF]	= {0x3, 0x18, 13, 0},
	[DVBT_CFREQ_OFF]	= {0x3, 0x5f, 17, 0},
	[DVBT_POLAR_RF_AGC]	= {0x0, 0xe,   1, 1},
	[DVBT_POLAR_IF_AGC]	= {0x0, 0xe,   0, 0},
	[DVBT_AAGC_HOLD]	= {0x1, 0x4,   5, 5},
	[DVBT_EN_RF_AGC]	= {0x1, 0x4,   6, 6},
	[DVBT_EN_IF_AGC]	= {0x1, 0x4,   7, 7},
	[DVBT_IF_AGC_MIN]	= {0x1, 0x8,   7, 0},
	[DVBT_IF_AGC_MAX]	= {0x1, 0x9,   7, 0},
	[DVBT_RF_AGC_MIN]	= {0x1, 0xa,   7, 0},
	[DVBT_RF_AGC_MAX]	= {0x1, 0xb,   7, 0},
	[DVBT_IF_AGC_MAN]	= {0x1, 0xc,   6, 6},
	[DVBT_IF_AGC_MAN_VAL]	= {0x1, 0xc,  13, 0},
	[DVBT_RF_AGC_MAN]	= {0x1, 0xe,   6, 6},
	[DVBT_RF_AGC_MAN_VAL]	= {0x1, 0xe,  13, 0},
	[DVBT_DAGC_TRG_VAL]	= {0x1, 0x12,  7, 0},
	[DVBT_AGC_TARG_VAL_0]	= {0x1, 0x2,   0, 0},
	[DVBT_AGC_TARG_VAL_8_1]	= {0x1, 0x3,   7, 0},
	[DVBT_AAGC_LOOP_GAIN]	= {0x1, 0xc7,  5, 1},
	[DVBT_LOOP_GAIN2_3_0]	= {0x1, 0x4,   4, 1},
	[DVBT_LOOP_GAIN2_4]	= {0x1, 0x5,   7, 7},
	[DVBT_LOOP_GAIN3]	= {0x1, 0xc8,  4, 0},
	[DVBT_VTOP1]		= {0x1, 0x6,   5, 0},
	[DVBT_VTOP2]		= {0x1, 0xc9,  5, 0},
	[DVBT_VTOP3]		= {0x1, 0xca,  5, 0},
	[DVBT_KRF1]		= {0x1, 0xcb,  7, 0},
	[DVBT_KRF2]		= {0x1, 0x7,   7, 0},
	[DVBT_KRF3]		= {0x1, 0xcd,  7, 0},
	[DVBT_KRF4]		= {0x1, 0xce,  7, 0},
	[DVBT_EN_GI_PGA]	= {0x1, 0xe5,  0, 0},
	[DVBT_THD_LOCK_UP]	= {0x1, 0xd9,  8, 0},
	[DVBT_THD_LOCK_DW]	= {0x1, 0xdb,  8, 0},
	[DVBT_THD_UP1]		= {0x1, 0xdd,  7, 0},
	[DVBT_THD_DW1]		= {0x1, 0xde,  7, 0},
	[DVBT_INTER_CNT_LEN]	= {0x1, 0xd8,  3, 0},
	[DVBT_GI_PGA_STATE]	= {0x1, 0xe6,  3, 3},
	[DVBT_EN_AGC_PGA]	= {0x1, 0xd7,  0, 0},
	[DVBT_CKOUTPAR]		= {0x1, 0x7b,  5, 5},
	[DVBT_CKOUT_PWR]	= {0x1, 0x7b,  6, 6},
	[DVBT_SYNC_DUR]		= {0x1, 0x7b,  7, 7},
	[DVBT_ERR_DUR]		= {0x1, 0x7c,  0, 0},
	[DVBT_SYNC_LVL]		= {0x1, 0x7c,  1, 1},
	[DVBT_ERR_LVL]		= {0x1, 0x7c,  2, 2},
	[DVBT_VAL_LVL]		= {0x1, 0x7c,  3, 3},
	[DVBT_SERIAL]		= {0x1, 0x7c,  4, 4},
	[DVBT_SER_LSB]		= {0x1, 0x7c,  5, 5},
	[DVBT_CDIV_PH0]		= {0x1, 0x7d,  3, 0},
	[DVBT_CDIV_PH1]		= {0x1, 0x7d,  7, 4},
	[DVBT_MPEG_IO_OPT_2_2]	= {0x0, 0x6,   7, 7},
	[DVBT_MPEG_IO_OPT_1_0]	= {0x0, 0x7,   7, 6},
	[DVBT_CKOUTPAR_PIP]	= {0x0, 0xb7,  4, 4},
	[DVBT_CKOUT_PWR_PIP]	= {0x0, 0xb7,  3, 3},
	[DVBT_SYNC_LVL_PIP]	= {0x0, 0xb7,  2, 2},
	[DVBT_ERR_LVL_PIP]	= {0x0, 0xb7,  1, 1},
	[DVBT_VAL_LVL_PIP]	= {0x0, 0xb7,  0, 0},
	[DVBT_CKOUTPAR_PID]	= {0x0, 0xb9,  4, 4},
	[DVBT_CKOUT_PWR_PID]	= {0x0, 0xb9,  3, 3},
	[DVBT_SYNC_LVL_PID]	= {0x0, 0xb9,  2, 2},
	[DVBT_ERR_LVL_PID]	= {0x0, 0xb9,  1, 1},
	[DVBT_VAL_LVL_PID]	= {0x0, 0xb9,  0, 0},
	[DVBT_SM_PASS]		= {0x1, 0x93, 11, 0},
	[DVBT_AD7_SETTING]	= {0x0, 0x11, 15, 0},
	[DVBT_RSSI_R]		= {0x3, 0x1,   6, 0},
	[DVBT_ACI_DET_IND]	= {0x3, 0x12,  0, 0},
	[DVBT_REG_MON]		= {0x0, 0xd,   1, 0},
	[DVBT_REG_MONSEL]	= {0x0, 0xd,   2, 2},
	[DVBT_REG_GPE]		= {0x0, 0xd,   7, 7},
	[DVBT_REG_GPO]		= {0x0, 0x10,  0, 0},
	[DVBT_REG_4MSEL]	= {0x0, 0x13,  0, 0},
};

/* write multiple hardware registers */
static int rtl2832_wr(struct rtl2832_priv *priv, u8 reg, u8 *val, int len)
{
	int ret;
	u8 buf[MAX_XFER_SIZE];
	struct i2c_msg msg[1] = {
		{
			.addr = priv->cfg.i2c_addr,
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

	ret = i2c_transfer(priv->i2c_adapter, msg, 1);
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

/* read multiple hardware registers */
static int rtl2832_rd(struct rtl2832_priv *priv, u8 reg, u8 *val, int len)
{
	int ret;
	struct i2c_msg msg[2] = {
		{
			.addr = priv->cfg.i2c_addr,
			.flags = 0,
			.len = 1,
			.buf = &reg,
		}, {
			.addr = priv->cfg.i2c_addr,
			.flags = I2C_M_RD,
			.len = len,
			.buf = val,
		}
	};

	ret = i2c_transfer(priv->i2c_adapter, msg, 2);
	if (ret == 2) {
		ret = 0;
	} else {
		dev_warn(&priv->i2c->dev,
				"%s: i2c rd failed=%d reg=%02x len=%d\n",
				KBUILD_MODNAME, ret, reg, len);
		ret = -EREMOTEIO;
	}
	return ret;
}

/* write multiple registers */
static int rtl2832_wr_regs(struct rtl2832_priv *priv, u8 reg, u8 page, u8 *val,
	int len)
{
	int ret;

	/* switch bank if needed */
	if (page != priv->page) {
		ret = rtl2832_wr(priv, 0x00, &page, 1);
		if (ret)
			return ret;

		priv->page = page;
}

return rtl2832_wr(priv, reg, val, len);
}

/* read multiple registers */
static int rtl2832_rd_regs(struct rtl2832_priv *priv, u8 reg, u8 page, u8 *val,
	int len)
{
	int ret;

	/* switch bank if needed */
	if (page != priv->page) {
		ret = rtl2832_wr(priv, 0x00, &page, 1);
		if (ret)
			return ret;

		priv->page = page;
	}

	return rtl2832_rd(priv, reg, val, len);
}

#if 0 /* currently not used */
/* write single register */
static int rtl2832_wr_reg(struct rtl2832_priv *priv, u8 reg, u8 page, u8 val)
{
	return rtl2832_wr_regs(priv, reg, page, &val, 1);
}
#endif

/* read single register */
static int rtl2832_rd_reg(struct rtl2832_priv *priv, u8 reg, u8 page, u8 *val)
{
	return rtl2832_rd_regs(priv, reg, page, val, 1);
}

static int rtl2832_rd_demod_reg(struct rtl2832_priv *priv, int reg, u32 *val)
{
	int ret;

	u8 reg_start_addr;
	u8 msb, lsb;
	u8 page;
	u8 reading[4];
	u32 reading_tmp;
	int i;

	u8 len;
	u32 mask;
=======

#define REG_MASK(b) (BIT(b + 1) - 1)

static const struct rtl2832_reg_entry registers[] = {
	[DVBT_SOFT_RST]		= {0x101,  2, 2},
	[DVBT_IIC_REPEAT]	= {0x101,  3, 3},
	[DVBT_TR_WAIT_MIN_8K]	= {0x188, 11, 2},
	[DVBT_RSD_BER_FAIL_VAL]	= {0x18f, 15, 0},
	[DVBT_EN_BK_TRK]	= {0x1a6,  7, 7},
	[DVBT_AD_EN_REG]	= {0x008,  7, 7},
	[DVBT_AD_EN_REG1]	= {0x008,  6, 6},
	[DVBT_EN_BBIN]		= {0x1b1,  0, 0},
	[DVBT_MGD_THD0]		= {0x195,  7, 0},
	[DVBT_MGD_THD1]		= {0x196,  7, 0},
	[DVBT_MGD_THD2]		= {0x197,  7, 0},
	[DVBT_MGD_THD3]		= {0x198,  7, 0},
	[DVBT_MGD_THD4]		= {0x199,  7, 0},
	[DVBT_MGD_THD5]		= {0x19a,  7, 0},
	[DVBT_MGD_THD6]		= {0x19b,  7, 0},
	[DVBT_MGD_THD7]		= {0x19c,  7, 0},
	[DVBT_EN_CACQ_NOTCH]	= {0x161,  4, 4},
	[DVBT_AD_AV_REF]	= {0x009,  6, 0},
	[DVBT_REG_PI]		= {0x00a,  2, 0},
	[DVBT_PIP_ON]		= {0x021,  3, 3},
	[DVBT_SCALE1_B92]	= {0x292,  7, 0},
	[DVBT_SCALE1_B93]	= {0x293,  7, 0},
	[DVBT_SCALE1_BA7]	= {0x2a7,  7, 0},
	[DVBT_SCALE1_BA9]	= {0x2a9,  7, 0},
	[DVBT_SCALE1_BAA]	= {0x2aa,  7, 0},
	[DVBT_SCALE1_BAB]	= {0x2ab,  7, 0},
	[DVBT_SCALE1_BAC]	= {0x2ac,  7, 0},
	[DVBT_SCALE1_BB0]	= {0x2b0,  7, 0},
	[DVBT_SCALE1_BB1]	= {0x2b1,  7, 0},
	[DVBT_KB_P1]		= {0x164,  3, 1},
	[DVBT_KB_P2]		= {0x164,  6, 4},
	[DVBT_KB_P3]		= {0x165,  2, 0},
	[DVBT_OPT_ADC_IQ]	= {0x006,  5, 4},
	[DVBT_AD_AVI]		= {0x009,  1, 0},
	[DVBT_AD_AVQ]		= {0x009,  3, 2},
	[DVBT_K1_CR_STEP12]	= {0x2ad,  9, 4},
	[DVBT_TRK_KS_P2]	= {0x16f,  2, 0},
	[DVBT_TRK_KS_I2]	= {0x170,  5, 3},
	[DVBT_TR_THD_SET2]	= {0x172,  3, 0},
	[DVBT_TRK_KC_P2]	= {0x173,  5, 3},
	[DVBT_TRK_KC_I2]	= {0x175,  2, 0},
	[DVBT_CR_THD_SET2]	= {0x176,  7, 6},
	[DVBT_PSET_IFFREQ]	= {0x119, 21, 0},
	[DVBT_SPEC_INV]		= {0x115,  0, 0},
	[DVBT_RSAMP_RATIO]	= {0x19f, 27, 2},
	[DVBT_CFREQ_OFF_RATIO]	= {0x19d, 23, 4},
	[DVBT_FSM_STAGE]	= {0x351,  6, 3},
	[DVBT_RX_CONSTEL]	= {0x33c,  3, 2},
	[DVBT_RX_HIER]		= {0x33c,  6, 4},
	[DVBT_RX_C_RATE_LP]	= {0x33d,  2, 0},
	[DVBT_RX_C_RATE_HP]	= {0x33d,  5, 3},
	[DVBT_GI_IDX]		= {0x351,  1, 0},
	[DVBT_FFT_MODE_IDX]	= {0x351,  2, 2},
	[DVBT_RSD_BER_EST]	= {0x34e, 15, 0},
	[DVBT_CE_EST_EVM]	= {0x40c, 15, 0},
	[DVBT_RF_AGC_VAL]	= {0x35b, 13, 0},
	[DVBT_IF_AGC_VAL]	= {0x359, 13, 0},
	[DVBT_DAGC_VAL]		= {0x305,  7, 0},
	[DVBT_SFREQ_OFF]	= {0x318, 13, 0},
	[DVBT_CFREQ_OFF]	= {0x35f, 17, 0},
	[DVBT_POLAR_RF_AGC]	= {0x00e,  1, 1},
	[DVBT_POLAR_IF_AGC]	= {0x00e,  0, 0},
	[DVBT_AAGC_HOLD]	= {0x104,  5, 5},
	[DVBT_EN_RF_AGC]	= {0x104,  6, 6},
	[DVBT_EN_IF_AGC]	= {0x104,  7, 7},
	[DVBT_IF_AGC_MIN]	= {0x108,  7, 0},
	[DVBT_IF_AGC_MAX]	= {0x109,  7, 0},
	[DVBT_RF_AGC_MIN]	= {0x10a,  7, 0},
	[DVBT_RF_AGC_MAX]	= {0x10b,  7, 0},
	[DVBT_IF_AGC_MAN]	= {0x10c,  6, 6},
	[DVBT_IF_AGC_MAN_VAL]	= {0x10c, 13, 0},
	[DVBT_RF_AGC_MAN]	= {0x10e,  6, 6},
	[DVBT_RF_AGC_MAN_VAL]	= {0x10e, 13, 0},
	[DVBT_DAGC_TRG_VAL]	= {0x112,  7, 0},
	[DVBT_AGC_TARG_VAL_0]	= {0x102,  0, 0},
	[DVBT_AGC_TARG_VAL_8_1]	= {0x103,  7, 0},
	[DVBT_AAGC_LOOP_GAIN]	= {0x1c7,  5, 1},
	[DVBT_LOOP_GAIN2_3_0]	= {0x104,  4, 1},
	[DVBT_LOOP_GAIN2_4]	= {0x105,  7, 7},
	[DVBT_LOOP_GAIN3]	= {0x1c8,  4, 0},
	[DVBT_VTOP1]		= {0x106,  5, 0},
	[DVBT_VTOP2]		= {0x1c9,  5, 0},
	[DVBT_VTOP3]		= {0x1ca,  5, 0},
	[DVBT_KRF1]		= {0x1cb,  7, 0},
	[DVBT_KRF2]		= {0x107,  7, 0},
	[DVBT_KRF3]		= {0x1cd,  7, 0},
	[DVBT_KRF4]		= {0x1ce,  7, 0},
	[DVBT_EN_GI_PGA]	= {0x1e5,  0, 0},
	[DVBT_THD_LOCK_UP]	= {0x1d9,  8, 0},
	[DVBT_THD_LOCK_DW]	= {0x1db,  8, 0},
	[DVBT_THD_UP1]		= {0x1dd,  7, 0},
	[DVBT_THD_DW1]		= {0x1de,  7, 0},
	[DVBT_INTER_CNT_LEN]	= {0x1d8,  3, 0},
	[DVBT_GI_PGA_STATE]	= {0x1e6,  3, 3},
	[DVBT_EN_AGC_PGA]	= {0x1d7,  0, 0},
	[DVBT_CKOUTPAR]		= {0x17b,  5, 5},
	[DVBT_CKOUT_PWR]	= {0x17b,  6, 6},
	[DVBT_SYNC_DUR]		= {0x17b,  7, 7},
	[DVBT_ERR_DUR]		= {0x17c,  0, 0},
	[DVBT_SYNC_LVL]		= {0x17c,  1, 1},
	[DVBT_ERR_LVL]		= {0x17c,  2, 2},
	[DVBT_VAL_LVL]		= {0x17c,  3, 3},
	[DVBT_SERIAL]		= {0x17c,  4, 4},
	[DVBT_SER_LSB]		= {0x17c,  5, 5},
	[DVBT_CDIV_PH0]		= {0x17d,  3, 0},
	[DVBT_CDIV_PH1]		= {0x17d,  7, 4},
	[DVBT_MPEG_IO_OPT_2_2]	= {0x006,  7, 7},
	[DVBT_MPEG_IO_OPT_1_0]	= {0x007,  7, 6},
	[DVBT_CKOUTPAR_PIP]	= {0x0b7,  4, 4},
	[DVBT_CKOUT_PWR_PIP]	= {0x0b7,  3, 3},
	[DVBT_SYNC_LVL_PIP]	= {0x0b7,  2, 2},
	[DVBT_ERR_LVL_PIP]	= {0x0b7,  1, 1},
	[DVBT_VAL_LVL_PIP]	= {0x0b7,  0, 0},
	[DVBT_CKOUTPAR_PID]	= {0x0b9,  4, 4},
	[DVBT_CKOUT_PWR_PID]	= {0x0b9,  3, 3},
	[DVBT_SYNC_LVL_PID]	= {0x0b9,  2, 2},
	[DVBT_ERR_LVL_PID]	= {0x0b9,  1, 1},
	[DVBT_VAL_LVL_PID]	= {0x0b9,  0, 0},
	[DVBT_SM_PASS]		= {0x193, 11, 0},
	[DVBT_AD7_SETTING]	= {0x011, 15, 0},
	[DVBT_RSSI_R]		= {0x301,  6, 0},
	[DVBT_ACI_DET_IND]	= {0x312,  0, 0},
	[DVBT_REG_MON]		= {0x00d,  1, 0},
	[DVBT_REG_MONSEL]	= {0x00d,  2, 2},
	[DVBT_REG_GPE]		= {0x00d,  7, 7},
	[DVBT_REG_GPO]		= {0x010,  0, 0},
	[DVBT_REG_4MSEL]	= {0x013,  0, 0},
};

static int rtl2832_rd_demod_reg(struct rtl2832_dev *dev, int reg, u32 *val)
{
	struct i2c_client *client = dev->client;
	int ret, i;
	u16 reg_start_addr;
	u8 msb, lsb, reading[4], len;
	u32 reading_tmp, mask;
>>>>>>> v4.9.227

	reg_start_addr = registers[reg].start_address;
	msb = registers[reg].msb;
	lsb = registers[reg].lsb;
<<<<<<< HEAD
	page = registers[reg].page;

	len = (msb >> 3) + 1;
	mask = REG_MASK(msb - lsb);

	ret = rtl2832_rd_regs(priv, reg_start_addr, page, &reading[0], len);
=======
	len = (msb >> 3) + 1;
	mask = REG_MASK(msb - lsb);

	ret = regmap_bulk_read(dev->regmap, reg_start_addr, reading, len);
>>>>>>> v4.9.227
	if (ret)
		goto err;

	reading_tmp = 0;
	for (i = 0; i < len; i++)
		reading_tmp |= reading[i] << ((len - 1 - i) * 8);

	*val = (reading_tmp >> lsb) & mask;

<<<<<<< HEAD
	return ret;

err:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;

}

static int rtl2832_wr_demod_reg(struct rtl2832_priv *priv, int reg, u32 val)
{
	int ret, i;
	u8 len;
	u8 reg_start_addr;
	u8 msb, lsb;
	u8 page;
	u32 mask;


	u8 reading[4];
	u8 writing[4];
	u32 reading_tmp;
	u32 writing_tmp;

=======
	return 0;
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

static int rtl2832_wr_demod_reg(struct rtl2832_dev *dev, int reg, u32 val)
{
	struct i2c_client *client = dev->client;
	int ret, i;
	u16 reg_start_addr;
	u8 msb, lsb, reading[4], writing[4], len;
	u32 reading_tmp, writing_tmp, mask;
>>>>>>> v4.9.227

	reg_start_addr = registers[reg].start_address;
	msb = registers[reg].msb;
	lsb = registers[reg].lsb;
<<<<<<< HEAD
	page = registers[reg].page;

	len = (msb >> 3) + 1;
	mask = REG_MASK(msb - lsb);


	ret = rtl2832_rd_regs(priv, reg_start_addr, page, &reading[0], len);
=======
	len = (msb >> 3) + 1;
	mask = REG_MASK(msb - lsb);

	ret = regmap_bulk_read(dev->regmap, reg_start_addr, reading, len);
>>>>>>> v4.9.227
	if (ret)
		goto err;

	reading_tmp = 0;
	for (i = 0; i < len; i++)
		reading_tmp |= reading[i] << ((len - 1 - i) * 8);

	writing_tmp = reading_tmp & ~(mask << lsb);
	writing_tmp |= ((val & mask) << lsb);

<<<<<<< HEAD

	for (i = 0; i < len; i++)
		writing[i] = (writing_tmp >> ((len - 1 - i) * 8)) & 0xff;

	ret = rtl2832_wr_regs(priv, reg_start_addr, page, &writing[0], len);
	if (ret)
		goto err;

	return ret;

err:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;

}

static int rtl2832_i2c_gate_ctrl(struct dvb_frontend *fe, int enable)
{
	int ret;
	struct rtl2832_priv *priv = fe->demodulator_priv;

	dev_dbg(&priv->i2c->dev, "%s: enable=%d\n", __func__, enable);

	/* gate already open or close */
	if (priv->i2c_gate_state == enable)
		return 0;

	ret = rtl2832_wr_demod_reg(priv, DVBT_IIC_REPEAT, (enable ? 0x1 : 0x0));
	if (ret)
		goto err;

	priv->i2c_gate_state = enable;

	return ret;
err:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}


static int rtl2832_set_if(struct dvb_frontend *fe, u32 if_freq)
{
	struct rtl2832_priv *priv = fe->demodulator_priv;
=======
	for (i = 0; i < len; i++)
		writing[i] = (writing_tmp >> ((len - 1 - i) * 8)) & 0xff;

	ret = regmap_bulk_write(dev->regmap, reg_start_addr, writing, len);
	if (ret)
		goto err;

	return 0;
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

static int rtl2832_set_if(struct dvb_frontend *fe, u32 if_freq)
{
	struct rtl2832_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
>>>>>>> v4.9.227
	int ret;
	u64 pset_iffreq;
	u8 en_bbin = (if_freq == 0 ? 0x1 : 0x0);

	/*
	* PSET_IFFREQ = - floor((IfFreqHz % CrystalFreqHz) * pow(2, 22)
	*		/ CrystalFreqHz)
	*/
<<<<<<< HEAD

	pset_iffreq = if_freq % priv->cfg.xtal;
	pset_iffreq *= 0x400000;
	pset_iffreq = div_u64(pset_iffreq, priv->cfg.xtal);
	pset_iffreq = -pset_iffreq;
	pset_iffreq = pset_iffreq & 0x3fffff;
	dev_dbg(&priv->i2c->dev, "%s: if_frequency=%d pset_iffreq=%08x\n",
			__func__, if_freq, (unsigned)pset_iffreq);

	ret = rtl2832_wr_demod_reg(priv, DVBT_EN_BBIN, en_bbin);
	if (ret)
		return ret;

	ret = rtl2832_wr_demod_reg(priv, DVBT_PSET_IFFREQ, pset_iffreq);

=======
	pset_iffreq = if_freq % dev->pdata->clk;
	pset_iffreq *= 0x400000;
	pset_iffreq = div_u64(pset_iffreq, dev->pdata->clk);
	pset_iffreq = -pset_iffreq;
	pset_iffreq = pset_iffreq & 0x3fffff;
	dev_dbg(&client->dev, "if_frequency=%d pset_iffreq=%08x\n",
		if_freq, (unsigned)pset_iffreq);

	ret = rtl2832_wr_demod_reg(dev, DVBT_EN_BBIN, en_bbin);
	if (ret)
		goto err;

	ret = rtl2832_wr_demod_reg(dev, DVBT_PSET_IFFREQ, pset_iffreq);
	if (ret)
		goto err;

	return 0;
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
>>>>>>> v4.9.227
	return ret;
}

static int rtl2832_init(struct dvb_frontend *fe)
{
<<<<<<< HEAD
	struct rtl2832_priv *priv = fe->demodulator_priv;
	const struct rtl2832_reg_value *init;
	int i, ret, len;

=======
	struct rtl2832_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
	struct dtv_frontend_properties *c = &dev->fe.dtv_property_cache;
	const struct rtl2832_reg_value *init;
	int i, ret, len;
>>>>>>> v4.9.227
	/* initialization values for the demodulator registers */
	struct rtl2832_reg_value rtl2832_initial_regs[] = {
		{DVBT_AD_EN_REG,		0x1},
		{DVBT_AD_EN_REG1,		0x1},
		{DVBT_RSD_BER_FAIL_VAL,		0x2800},
		{DVBT_MGD_THD0,			0x10},
		{DVBT_MGD_THD1,			0x20},
		{DVBT_MGD_THD2,			0x20},
		{DVBT_MGD_THD3,			0x40},
		{DVBT_MGD_THD4,			0x22},
		{DVBT_MGD_THD5,			0x32},
		{DVBT_MGD_THD6,			0x37},
		{DVBT_MGD_THD7,			0x39},
		{DVBT_EN_BK_TRK,		0x0},
		{DVBT_EN_CACQ_NOTCH,		0x0},
		{DVBT_AD_AV_REF,		0x2a},
		{DVBT_REG_PI,			0x6},
		{DVBT_PIP_ON,			0x0},
		{DVBT_CDIV_PH0,			0x8},
		{DVBT_CDIV_PH1,			0x8},
		{DVBT_SCALE1_B92,		0x4},
		{DVBT_SCALE1_B93,		0xb0},
		{DVBT_SCALE1_BA7,		0x78},
		{DVBT_SCALE1_BA9,		0x28},
		{DVBT_SCALE1_BAA,		0x59},
		{DVBT_SCALE1_BAB,		0x83},
		{DVBT_SCALE1_BAC,		0xd4},
		{DVBT_SCALE1_BB0,		0x65},
		{DVBT_SCALE1_BB1,		0x43},
		{DVBT_KB_P1,			0x1},
		{DVBT_KB_P2,			0x4},
		{DVBT_KB_P3,			0x7},
		{DVBT_K1_CR_STEP12,		0xa},
		{DVBT_REG_GPE,			0x1},
		{DVBT_SERIAL,			0x0},
		{DVBT_CDIV_PH0,			0x9},
		{DVBT_CDIV_PH1,			0x9},
		{DVBT_MPEG_IO_OPT_2_2,		0x0},
		{DVBT_MPEG_IO_OPT_1_0,		0x0},
		{DVBT_TRK_KS_P2,		0x4},
		{DVBT_TRK_KS_I2,		0x7},
		{DVBT_TR_THD_SET2,		0x6},
		{DVBT_TRK_KC_I2,		0x5},
		{DVBT_CR_THD_SET2,		0x1},
	};

<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s:\n", __func__);

	for (i = 0; i < ARRAY_SIZE(rtl2832_initial_regs); i++) {
		ret = rtl2832_wr_demod_reg(priv, rtl2832_initial_regs[i].reg,
=======
	dev_dbg(&client->dev, "\n");

	ret = rtl2832_wr_demod_reg(dev, DVBT_SOFT_RST, 0x0);
	if (ret)
		goto err;

	for (i = 0; i < ARRAY_SIZE(rtl2832_initial_regs); i++) {
		ret = rtl2832_wr_demod_reg(dev, rtl2832_initial_regs[i].reg,
>>>>>>> v4.9.227
			rtl2832_initial_regs[i].value);
		if (ret)
			goto err;
	}

	/* load tuner specific settings */
<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: load settings for tuner=%02x\n",
			__func__, priv->cfg.tuner);
	switch (priv->cfg.tuner) {
=======
	dev_dbg(&client->dev, "load settings for tuner=%02x\n",
		dev->pdata->tuner);
	switch (dev->pdata->tuner) {
	case RTL2832_TUNER_FC2580:
		len = ARRAY_SIZE(rtl2832_tuner_init_fc2580);
		init = rtl2832_tuner_init_fc2580;
		break;
>>>>>>> v4.9.227
	case RTL2832_TUNER_FC0012:
	case RTL2832_TUNER_FC0013:
		len = ARRAY_SIZE(rtl2832_tuner_init_fc0012);
		init = rtl2832_tuner_init_fc0012;
		break;
	case RTL2832_TUNER_TUA9001:
		len = ARRAY_SIZE(rtl2832_tuner_init_tua9001);
		init = rtl2832_tuner_init_tua9001;
		break;
	case RTL2832_TUNER_E4000:
		len = ARRAY_SIZE(rtl2832_tuner_init_e4000);
		init = rtl2832_tuner_init_e4000;
		break;
	case RTL2832_TUNER_R820T:
	case RTL2832_TUNER_R828D:
		len = ARRAY_SIZE(rtl2832_tuner_init_r820t);
		init = rtl2832_tuner_init_r820t;
		break;
<<<<<<< HEAD
=======
	case RTL2832_TUNER_SI2157:
		len = ARRAY_SIZE(rtl2832_tuner_init_si2157);
		init = rtl2832_tuner_init_si2157;
		break;
>>>>>>> v4.9.227
	default:
		ret = -EINVAL;
		goto err;
	}

	for (i = 0; i < len; i++) {
<<<<<<< HEAD
		ret = rtl2832_wr_demod_reg(priv, init[i].reg, init[i].value);
=======
		ret = rtl2832_wr_demod_reg(dev, init[i].reg, init[i].value);
>>>>>>> v4.9.227
		if (ret)
			goto err;
	}

<<<<<<< HEAD
	/*
	 * r820t NIM code does a software reset here at the demod -
	 * may not be needed, as there's already a software reset at
	 * set_params()
	 */
#if 1
	/* soft reset */
	ret = rtl2832_wr_demod_reg(priv, DVBT_SOFT_RST, 0x1);
	if (ret)
		goto err;

	ret = rtl2832_wr_demod_reg(priv, DVBT_SOFT_RST, 0x0);
	if (ret)
		goto err;
#endif

	priv->sleeping = false;

	return ret;

err:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
=======
	/* init stats here in order signal app which stats are supported */
	c->strength.len = 1;
	c->strength.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	c->cnr.len = 1;
	c->cnr.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	c->post_bit_error.len = 1;
	c->post_bit_error.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	c->post_bit_count.len = 1;
	c->post_bit_count.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	dev->sleeping = false;

	return 0;
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
>>>>>>> v4.9.227
	return ret;
}

static int rtl2832_sleep(struct dvb_frontend *fe)
{
<<<<<<< HEAD
	struct rtl2832_priv *priv = fe->demodulator_priv;

	dev_dbg(&priv->i2c->dev, "%s:\n", __func__);
	priv->sleeping = true;
	return 0;
=======
	struct rtl2832_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
	int ret;

	dev_dbg(&client->dev, "\n");

	dev->sleeping = true;
	dev->fe_status = 0;

	ret = rtl2832_wr_demod_reg(dev, DVBT_SOFT_RST, 0x1);
	if (ret)
		goto err;

	return 0;
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
>>>>>>> v4.9.227
}

static int rtl2832_get_tune_settings(struct dvb_frontend *fe,
	struct dvb_frontend_tune_settings *s)
{
<<<<<<< HEAD
	struct rtl2832_priv *priv = fe->demodulator_priv;

	dev_dbg(&priv->i2c->dev, "%s:\n", __func__);
=======
	struct rtl2832_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;

	dev_dbg(&client->dev, "\n");
>>>>>>> v4.9.227
	s->min_delay_ms = 1000;
	s->step_size = fe->ops.info.frequency_stepsize * 2;
	s->max_drift = (fe->ops.info.frequency_stepsize * 2) + 1;
	return 0;
}

static int rtl2832_set_frontend(struct dvb_frontend *fe)
{
<<<<<<< HEAD
	struct rtl2832_priv *priv = fe->demodulator_priv;
=======
	struct rtl2832_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
>>>>>>> v4.9.227
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int ret, i, j;
	u64 bw_mode, num, num2;
	u32 resamp_ratio, cfreq_off_ratio;
	static u8 bw_params[3][32] = {
	/* 6 MHz bandwidth */
		{
		0xf5, 0xff, 0x15, 0x38, 0x5d, 0x6d, 0x52, 0x07, 0xfa, 0x2f,
		0x53, 0xf5, 0x3f, 0xca, 0x0b, 0x91, 0xea, 0x30, 0x63, 0xb2,
		0x13, 0xda, 0x0b, 0xc4, 0x18, 0x7e, 0x16, 0x66, 0x08, 0x67,
		0x19, 0xe0,
		},

	/*  7 MHz bandwidth */
		{
		0xe7, 0xcc, 0xb5, 0xba, 0xe8, 0x2f, 0x67, 0x61, 0x00, 0xaf,
		0x86, 0xf2, 0xbf, 0x59, 0x04, 0x11, 0xb6, 0x33, 0xa4, 0x30,
		0x15, 0x10, 0x0a, 0x42, 0x18, 0xf8, 0x17, 0xd9, 0x07, 0x22,
		0x19, 0x10,
		},

	/*  8 MHz bandwidth */
		{
		0x09, 0xf6, 0xd2, 0xa7, 0x9a, 0xc9, 0x27, 0x77, 0x06, 0xbf,
		0xec, 0xf4, 0x4f, 0x0b, 0xfc, 0x01, 0x63, 0x35, 0x54, 0xa7,
		0x16, 0x66, 0x08, 0xb4, 0x19, 0x6e, 0x19, 0x65, 0x05, 0xc8,
		0x19, 0xe0,
		},
	};

<<<<<<< HEAD

	dev_dbg(&priv->i2c->dev,
			"%s: frequency=%d bandwidth_hz=%d inversion=%d\n",
			__func__, c->frequency, c->bandwidth_hz, c->inversion);
=======
	dev_dbg(&client->dev, "frequency=%u bandwidth_hz=%u inversion=%u\n",
		c->frequency, c->bandwidth_hz, c->inversion);
>>>>>>> v4.9.227

	/* program tuner */
	if (fe->ops.tuner_ops.set_params)
		fe->ops.tuner_ops.set_params(fe);

	/* If the frontend has get_if_frequency(), use it */
	if (fe->ops.tuner_ops.get_if_frequency) {
		u32 if_freq;

		ret = fe->ops.tuner_ops.get_if_frequency(fe, &if_freq);
		if (ret)
			goto err;

		ret = rtl2832_set_if(fe, if_freq);
		if (ret)
			goto err;
	}

	switch (c->bandwidth_hz) {
	case 6000000:
		i = 0;
		bw_mode = 48000000;
		break;
	case 7000000:
		i = 1;
		bw_mode = 56000000;
		break;
	case 8000000:
		i = 2;
		bw_mode = 64000000;
		break;
	default:
<<<<<<< HEAD
		dev_dbg(&priv->i2c->dev, "%s: invalid bandwidth\n", __func__);
		return -EINVAL;
	}

	for (j = 0; j < sizeof(bw_params[0]); j++) {
		ret = rtl2832_wr_regs(priv, 0x1c+j, 1, &bw_params[i][j], 1);
=======
		dev_err(&client->dev, "invalid bandwidth_hz %u\n",
			c->bandwidth_hz);
		ret = -EINVAL;
		goto err;
	}

	for (j = 0; j < sizeof(bw_params[0]); j++) {
		ret = regmap_bulk_write(dev->regmap,
					0x11c + j, &bw_params[i][j], 1);
>>>>>>> v4.9.227
		if (ret)
			goto err;
	}

	/* calculate and set resample ratio
	* RSAMP_RATIO = floor(CrystalFreqHz * 7 * pow(2, 22)
	*	/ ConstWithBandwidthMode)
	*/
<<<<<<< HEAD
	num = priv->cfg.xtal * 7;
	num *= 0x400000;
	num = div_u64(num, bw_mode);
	resamp_ratio =  num & 0x3ffffff;
	ret = rtl2832_wr_demod_reg(priv, DVBT_RSAMP_RATIO, resamp_ratio);
=======
	num = dev->pdata->clk * 7;
	num *= 0x400000;
	num = div_u64(num, bw_mode);
	resamp_ratio =  num & 0x3ffffff;
	ret = rtl2832_wr_demod_reg(dev, DVBT_RSAMP_RATIO, resamp_ratio);
>>>>>>> v4.9.227
	if (ret)
		goto err;

	/* calculate and set cfreq off ratio
	* CFREQ_OFF_RATIO = - floor(ConstWithBandwidthMode * pow(2, 20)
	*	/ (CrystalFreqHz * 7))
	*/
	num = bw_mode << 20;
<<<<<<< HEAD
	num2 = priv->cfg.xtal * 7;
	num = div_u64(num, num2);
	num = -num;
	cfreq_off_ratio = num & 0xfffff;
	ret = rtl2832_wr_demod_reg(priv, DVBT_CFREQ_OFF_RATIO, cfreq_off_ratio);
	if (ret)
		goto err;


	/* soft reset */
	ret = rtl2832_wr_demod_reg(priv, DVBT_SOFT_RST, 0x1);
	if (ret)
		goto err;

	ret = rtl2832_wr_demod_reg(priv, DVBT_SOFT_RST, 0x0);
	if (ret)
		goto err;

	return ret;
err:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}

static int rtl2832_get_frontend(struct dvb_frontend *fe)
{
	struct rtl2832_priv *priv = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int ret;
	u8 buf[3];

	if (priv->sleeping)
		return 0;

	ret = rtl2832_rd_regs(priv, 0x3c, 3, buf, 2);
	if (ret)
		goto err;

	ret = rtl2832_rd_reg(priv, 0x51, 3, &buf[2]);
	if (ret)
		goto err;

	dev_dbg(&priv->i2c->dev, "%s: TPS=%*ph\n", __func__, 3, buf);
=======
	num2 = dev->pdata->clk * 7;
	num = div_u64(num, num2);
	num = -num;
	cfreq_off_ratio = num & 0xfffff;
	ret = rtl2832_wr_demod_reg(dev, DVBT_CFREQ_OFF_RATIO, cfreq_off_ratio);
	if (ret)
		goto err;

	/* soft reset */
	ret = rtl2832_wr_demod_reg(dev, DVBT_SOFT_RST, 0x1);
	if (ret)
		goto err;

	ret = rtl2832_wr_demod_reg(dev, DVBT_SOFT_RST, 0x0);
	if (ret)
		goto err;

	return 0;
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

static int rtl2832_get_frontend(struct dvb_frontend *fe,
				struct dtv_frontend_properties *c)
{
	struct rtl2832_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
	int ret;
	u8 buf[3];

	if (dev->sleeping)
		return 0;

	ret = regmap_bulk_read(dev->regmap, 0x33c, buf, 2);
	if (ret)
		goto err;

	ret = regmap_bulk_read(dev->regmap, 0x351, &buf[2], 1);
	if (ret)
		goto err;

	dev_dbg(&client->dev, "TPS=%*ph\n", 3, buf);
>>>>>>> v4.9.227

	switch ((buf[0] >> 2) & 3) {
	case 0:
		c->modulation = QPSK;
		break;
	case 1:
		c->modulation = QAM_16;
		break;
	case 2:
		c->modulation = QAM_64;
		break;
	}

	switch ((buf[2] >> 2) & 1) {
	case 0:
		c->transmission_mode = TRANSMISSION_MODE_2K;
		break;
	case 1:
		c->transmission_mode = TRANSMISSION_MODE_8K;
	}

	switch ((buf[2] >> 0) & 3) {
	case 0:
		c->guard_interval = GUARD_INTERVAL_1_32;
		break;
	case 1:
		c->guard_interval = GUARD_INTERVAL_1_16;
		break;
	case 2:
		c->guard_interval = GUARD_INTERVAL_1_8;
		break;
	case 3:
		c->guard_interval = GUARD_INTERVAL_1_4;
		break;
	}

	switch ((buf[0] >> 4) & 7) {
	case 0:
		c->hierarchy = HIERARCHY_NONE;
		break;
	case 1:
		c->hierarchy = HIERARCHY_1;
		break;
	case 2:
		c->hierarchy = HIERARCHY_2;
		break;
	case 3:
		c->hierarchy = HIERARCHY_4;
		break;
	}

	switch ((buf[1] >> 3) & 7) {
	case 0:
		c->code_rate_HP = FEC_1_2;
		break;
	case 1:
		c->code_rate_HP = FEC_2_3;
		break;
	case 2:
		c->code_rate_HP = FEC_3_4;
		break;
	case 3:
		c->code_rate_HP = FEC_5_6;
		break;
	case 4:
		c->code_rate_HP = FEC_7_8;
		break;
	}

	switch ((buf[1] >> 0) & 7) {
	case 0:
		c->code_rate_LP = FEC_1_2;
		break;
	case 1:
		c->code_rate_LP = FEC_2_3;
		break;
	case 2:
		c->code_rate_LP = FEC_3_4;
		break;
	case 3:
		c->code_rate_LP = FEC_5_6;
		break;
	case 4:
		c->code_rate_LP = FEC_7_8;
		break;
	}

	return 0;
err:
<<<<<<< HEAD
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}

static int rtl2832_read_status(struct dvb_frontend *fe, fe_status_t *status)
{
	struct rtl2832_priv *priv = fe->demodulator_priv;
	int ret;
	u32 tmp;
	*status = 0;

	dev_dbg(&priv->i2c->dev, "%s:\n", __func__);
	if (priv->sleeping)
		return 0;

	ret = rtl2832_rd_demod_reg(priv, DVBT_FSM_STAGE, &tmp);
=======
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

static int rtl2832_read_status(struct dvb_frontend *fe, enum fe_status *status)
{
	struct rtl2832_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int ret;
	u32 uninitialized_var(tmp);
	u8 u8tmp, buf[2];
	u16 u16tmp;

	dev_dbg(&client->dev, "\n");

	*status = 0;
	if (dev->sleeping)
		return 0;

	ret = rtl2832_rd_demod_reg(dev, DVBT_FSM_STAGE, &tmp);
>>>>>>> v4.9.227
	if (ret)
		goto err;

	if (tmp == 11) {
		*status |= FE_HAS_SIGNAL | FE_HAS_CARRIER |
				FE_HAS_VITERBI | FE_HAS_SYNC | FE_HAS_LOCK;
<<<<<<< HEAD
	}
	/* TODO find out if this is also true for rtl2832? */
	/*else if (tmp == 10) {
		*status |= FE_HAS_SIGNAL | FE_HAS_CARRIER |
				FE_HAS_VITERBI;
	}*/

	return ret;
err:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
=======
	} else if (tmp == 10) {
		*status |= FE_HAS_SIGNAL | FE_HAS_CARRIER |
				FE_HAS_VITERBI;
	}

	dev->fe_status = *status;

	/* signal strength */
	if (dev->fe_status & FE_HAS_SIGNAL) {
		/* read digital AGC */
		ret = regmap_bulk_read(dev->regmap, 0x305, &u8tmp, 1);
		if (ret)
			goto err;

		dev_dbg(&client->dev, "digital agc=%02x", u8tmp);

		u8tmp = ~u8tmp;
		u16tmp = u8tmp << 8 | u8tmp << 0;

		c->strength.stat[0].scale = FE_SCALE_RELATIVE;
		c->strength.stat[0].uvalue = u16tmp;
	} else {
		c->strength.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	}

	/* CNR */
	if (dev->fe_status & FE_HAS_VITERBI) {
		unsigned hierarchy, constellation;
		#define CONSTELLATION_NUM 3
		#define HIERARCHY_NUM 4
		static const u32 constant[CONSTELLATION_NUM][HIERARCHY_NUM] = {
			{85387325, 85387325, 85387325, 85387325},
			{86676178, 86676178, 87167949, 87795660},
			{87659938, 87659938, 87885178, 88241743},
		};

		ret = regmap_bulk_read(dev->regmap, 0x33c, &u8tmp, 1);
		if (ret)
			goto err;

		constellation = (u8tmp >> 2) & 0x03; /* [3:2] */
		if (constellation > CONSTELLATION_NUM - 1)
			goto err;

		hierarchy = (u8tmp >> 4) & 0x07; /* [6:4] */
		if (hierarchy > HIERARCHY_NUM - 1)
			goto err;

		ret = regmap_bulk_read(dev->regmap, 0x40c, buf, 2);
		if (ret)
			goto err;

		u16tmp = buf[0] << 8 | buf[1] << 0;
		if (u16tmp)
			tmp = (constant[constellation][hierarchy] -
			       intlog10(u16tmp)) / ((1 << 24) / 10000);
		else
			tmp = 0;

		dev_dbg(&client->dev, "cnr raw=%u\n", u16tmp);

		c->cnr.stat[0].scale = FE_SCALE_DECIBEL;
		c->cnr.stat[0].svalue = tmp;
	} else {
		c->cnr.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	}

	/* BER */
	if (dev->fe_status & FE_HAS_LOCK) {
		ret = regmap_bulk_read(dev->regmap, 0x34e, buf, 2);
		if (ret)
			goto err;

		u16tmp = buf[0] << 8 | buf[1] << 0;
		dev->post_bit_error += u16tmp;
		dev->post_bit_count += 1000000;

		dev_dbg(&client->dev, "ber errors=%u total=1000000\n", u16tmp);

		c->post_bit_error.stat[0].scale = FE_SCALE_COUNTER;
		c->post_bit_error.stat[0].uvalue = dev->post_bit_error;
		c->post_bit_count.stat[0].scale = FE_SCALE_COUNTER;
		c->post_bit_count.stat[0].uvalue = dev->post_bit_count;
	} else {
		c->post_bit_error.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
		c->post_bit_count.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
	}

	return 0;
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
>>>>>>> v4.9.227
	return ret;
}

static int rtl2832_read_snr(struct dvb_frontend *fe, u16 *snr)
{
<<<<<<< HEAD
	struct rtl2832_priv *priv = fe->demodulator_priv;
	int ret, hierarchy, constellation;
	u8 buf[2], tmp;
	u16 tmp16;
#define CONSTELLATION_NUM 3
#define HIERARCHY_NUM 4
	static const u32 snr_constant[CONSTELLATION_NUM][HIERARCHY_NUM] = {
		{ 85387325, 85387325, 85387325, 85387325 },
		{ 86676178, 86676178, 87167949, 87795660 },
		{ 87659938, 87659938, 87885178, 88241743 },
	};

	/* reports SNR in resolution of 0.1 dB */

	ret = rtl2832_rd_reg(priv, 0x3c, 3, &tmp);
	if (ret)
		goto err;

	constellation = (tmp >> 2) & 0x03; /* [3:2] */
	if (constellation > CONSTELLATION_NUM - 1)
		goto err;

	hierarchy = (tmp >> 4) & 0x07; /* [6:4] */
	if (hierarchy > HIERARCHY_NUM - 1)
		goto err;

	ret = rtl2832_rd_regs(priv, 0x0c, 4, buf, 2);
	if (ret)
		goto err;

	tmp16 = buf[0] << 8 | buf[1];

	if (tmp16)
		*snr = (snr_constant[constellation][hierarchy] -
				intlog10(tmp16)) / ((1 << 24) / 100);
=======
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;

	/* report SNR in resolution of 0.1 dB */
	if (c->cnr.stat[0].scale == FE_SCALE_DECIBEL)
		*snr = div_s64(c->cnr.stat[0].svalue, 100);
>>>>>>> v4.9.227
	else
		*snr = 0;

	return 0;
<<<<<<< HEAD
err:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
=======
>>>>>>> v4.9.227
}

static int rtl2832_read_ber(struct dvb_frontend *fe, u32 *ber)
{
<<<<<<< HEAD
	struct rtl2832_priv *priv = fe->demodulator_priv;
	int ret;
	u8 buf[2];

	ret = rtl2832_rd_regs(priv, 0x4e, 3, buf, 2);
	if (ret)
		goto err;

	*ber = buf[0] << 8 | buf[1];

	return 0;
err:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);
	return ret;
}

static struct dvb_frontend_ops rtl2832_ops;

static void rtl2832_release(struct dvb_frontend *fe)
{
	struct rtl2832_priv *priv = fe->demodulator_priv;

	dev_dbg(&priv->i2c->dev, "%s:\n", __func__);
	cancel_delayed_work_sync(&priv->i2c_gate_work);
	i2c_del_mux_adapter(priv->i2c_adapter_tuner);
	i2c_del_mux_adapter(priv->i2c_adapter);
	kfree(priv);
}

/*
 * Delay mechanism to avoid unneeded I2C gate open / close. Gate close is
 * delayed here a little bit in order to see if there is sequence of I2C
 * messages sent to same I2C bus.
 * We must use unlocked version of __i2c_transfer() in order to avoid deadlock
 * as lock is already taken by calling muxed i2c_transfer().
 */
static void rtl2832_i2c_gate_work(struct work_struct *work)
{
	struct rtl2832_priv *priv = container_of(work,
			struct rtl2832_priv, i2c_gate_work.work);
	struct i2c_adapter *adap = priv->i2c;
	int ret;
	u8 buf[2];
	struct i2c_msg msg[1] = {
		{
			.addr = priv->cfg.i2c_addr,
			.flags = 0,
			.len = sizeof(buf),
			.buf = buf,
		}
	};

	/* select reg bank 1 */
	buf[0] = 0x00;
	buf[1] = 0x01;
	ret = __i2c_transfer(adap, msg, 1);
	if (ret != 1)
		goto err;

	priv->page = 1;

	/* close I2C repeater gate */
	buf[0] = 0x01;
	buf[1] = 0x10;
	ret = __i2c_transfer(adap, msg, 1);
	if (ret != 1)
		goto err;

	priv->i2c_gate_state = false;

	return;
err:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);

	return;
}

static int rtl2832_select(struct i2c_adapter *adap, void *mux_priv, u32 chan_id)
{
	struct rtl2832_priv *priv = mux_priv;
	int ret;
	u8 buf[2], val;
	struct i2c_msg msg[1] = {
		{
			.addr = priv->cfg.i2c_addr,
			.flags = 0,
			.len = sizeof(buf),
			.buf = buf,
		}
	};
	struct i2c_msg msg_rd[2] = {
		{
			.addr = priv->cfg.i2c_addr,
			.flags = 0,
			.len = 1,
			.buf = "\x01",
		}, {
			.addr = priv->cfg.i2c_addr,
			.flags = I2C_M_RD,
			.len = 1,
			.buf = &val,
		}
	};

	/* terminate possible gate closing */
	cancel_delayed_work_sync(&priv->i2c_gate_work);

	if (priv->i2c_gate_state == chan_id)
		return 0;

	/* select reg bank 1 */
	buf[0] = 0x00;
	buf[1] = 0x01;
	ret = __i2c_transfer(adap, msg, 1);
	if (ret != 1)
		goto err;

	priv->page = 1;

	/* we must read that register, otherwise there will be errors */
	ret = __i2c_transfer(adap, msg_rd, 2);
	if (ret != 2)
		goto err;

	/* open or close I2C repeater gate */
	buf[0] = 0x01;
	if (chan_id == 1)
		buf[1] = 0x18; /* open */
	else
		buf[1] = 0x10; /* close */

	ret = __i2c_transfer(adap, msg, 1);
	if (ret != 1)
		goto err;

	priv->i2c_gate_state = chan_id;

	return 0;
err:
	dev_dbg(&priv->i2c->dev, "%s: failed=%d\n", __func__, ret);

	return -EREMOTEIO;
}

static int rtl2832_deselect(struct i2c_adapter *adap, void *mux_priv,
		u32 chan_id)
{
	struct rtl2832_priv *priv = mux_priv;
	schedule_delayed_work(&priv->i2c_gate_work, usecs_to_jiffies(100));
	return 0;
}

struct i2c_adapter *rtl2832_get_i2c_adapter(struct dvb_frontend *fe)
{
	struct rtl2832_priv *priv = fe->demodulator_priv;
	return priv->i2c_adapter_tuner;
}
EXPORT_SYMBOL(rtl2832_get_i2c_adapter);

struct i2c_adapter *rtl2832_get_private_i2c_adapter(struct dvb_frontend *fe)
{
	struct rtl2832_priv *priv = fe->demodulator_priv;
	return priv->i2c_adapter;
}
EXPORT_SYMBOL(rtl2832_get_private_i2c_adapter);

struct dvb_frontend *rtl2832_attach(const struct rtl2832_config *cfg,
	struct i2c_adapter *i2c)
{
	struct rtl2832_priv *priv = NULL;
	int ret = 0;
	u8 tmp;

	dev_dbg(&i2c->dev, "%s:\n", __func__);

	/* allocate memory for the internal state */
	priv = kzalloc(sizeof(struct rtl2832_priv), GFP_KERNEL);
	if (priv == NULL)
		goto err;

	/* setup the priv */
	priv->i2c = i2c;
	priv->tuner = cfg->tuner;
	memcpy(&priv->cfg, cfg, sizeof(struct rtl2832_config));
	INIT_DELAYED_WORK(&priv->i2c_gate_work, rtl2832_i2c_gate_work);

	/* create muxed i2c adapter for demod itself */
	priv->i2c_adapter = i2c_add_mux_adapter(i2c, &i2c->dev, priv, 0, 0, 0,
			rtl2832_select, NULL);
	if (priv->i2c_adapter == NULL)
		goto err;

	/* check if the demod is there */
	ret = rtl2832_rd_reg(priv, 0x00, 0x0, &tmp);
	if (ret)
		goto err;

	/* create muxed i2c adapter for demod tuner bus */
	priv->i2c_adapter_tuner = i2c_add_mux_adapter(i2c, &i2c->dev, priv,
			0, 1, 0, rtl2832_select, rtl2832_deselect);
	if (priv->i2c_adapter_tuner == NULL)
		goto err;

	/* create dvb_frontend */
	memcpy(&priv->fe.ops, &rtl2832_ops, sizeof(struct dvb_frontend_ops));
	priv->fe.demodulator_priv = priv;

	/* TODO implement sleep mode */
	priv->sleeping = true;

	return &priv->fe;
err:
	dev_dbg(&i2c->dev, "%s: failed=%d\n", __func__, ret);
	if (priv && priv->i2c_adapter)
		i2c_del_mux_adapter(priv->i2c_adapter);
	kfree(priv);
	return NULL;
}
EXPORT_SYMBOL(rtl2832_attach);
=======
	struct rtl2832_dev *dev = fe->demodulator_priv;

	*ber = (dev->post_bit_error - dev->post_bit_error_prev);
	dev->post_bit_error_prev = dev->post_bit_error;

	return 0;
}

/*
 * I2C gate/mux/repeater logic
 * There is delay mechanism to avoid unneeded I2C gate open / close. Gate close
 * is delayed here a little bit in order to see if there is sequence of I2C
 * messages sent to same I2C bus.
 */
static void rtl2832_i2c_gate_work(struct work_struct *work)
{
	struct rtl2832_dev *dev = container_of(work, struct rtl2832_dev, i2c_gate_work.work);
	struct i2c_client *client = dev->client;
	int ret;

	/* close gate */
	ret = regmap_update_bits(dev->regmap, 0x101, 0x08, 0x00);
	if (ret)
		goto err;

	return;
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
}

static int rtl2832_select(struct i2c_mux_core *muxc, u32 chan_id)
{
	struct rtl2832_dev *dev = i2c_mux_priv(muxc);
	struct i2c_client *client = dev->client;
	int ret;

	/* terminate possible gate closing */
	cancel_delayed_work(&dev->i2c_gate_work);

	/* open gate */
	ret = regmap_update_bits(dev->regmap, 0x101, 0x08, 0x08);
	if (ret)
		goto err;

	return 0;
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

static int rtl2832_deselect(struct i2c_mux_core *muxc, u32 chan_id)
{
	struct rtl2832_dev *dev = i2c_mux_priv(muxc);

	schedule_delayed_work(&dev->i2c_gate_work, usecs_to_jiffies(100));
	return 0;
}
>>>>>>> v4.9.227

static struct dvb_frontend_ops rtl2832_ops = {
	.delsys = { SYS_DVBT },
	.info = {
		.name = "Realtek RTL2832 (DVB-T)",
		.frequency_min	  = 174000000,
		.frequency_max	  = 862000000,
		.frequency_stepsize = 166667,
		.caps = FE_CAN_FEC_1_2 |
			FE_CAN_FEC_2_3 |
			FE_CAN_FEC_3_4 |
			FE_CAN_FEC_5_6 |
			FE_CAN_FEC_7_8 |
			FE_CAN_FEC_AUTO |
			FE_CAN_QPSK |
			FE_CAN_QAM_16 |
			FE_CAN_QAM_64 |
			FE_CAN_QAM_AUTO |
			FE_CAN_TRANSMISSION_MODE_AUTO |
			FE_CAN_GUARD_INTERVAL_AUTO |
			FE_CAN_HIERARCHY_AUTO |
			FE_CAN_RECOVER |
			FE_CAN_MUTE_TS
	 },

<<<<<<< HEAD
	.release = rtl2832_release,

=======
>>>>>>> v4.9.227
	.init = rtl2832_init,
	.sleep = rtl2832_sleep,

	.get_tune_settings = rtl2832_get_tune_settings,

	.set_frontend = rtl2832_set_frontend,
	.get_frontend = rtl2832_get_frontend,

	.read_status = rtl2832_read_status,
	.read_snr = rtl2832_read_snr,
	.read_ber = rtl2832_read_ber,
<<<<<<< HEAD

	.i2c_gate_ctrl = rtl2832_i2c_gate_ctrl,
};

MODULE_AUTHOR("Thomas Mair <mair.thomas86@gmail.com>");
MODULE_DESCRIPTION("Realtek RTL2832 DVB-T demodulator driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.5");
=======
};

static bool rtl2832_volatile_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case 0x305:
	case 0x33c:
	case 0x34e:
	case 0x351:
	case 0x40c ... 0x40d:
		return true;
	default:
		break;
	}

	return false;
}

static struct dvb_frontend *rtl2832_get_dvb_frontend(struct i2c_client *client)
{
	struct rtl2832_dev *dev = i2c_get_clientdata(client);

	dev_dbg(&client->dev, "\n");
	return &dev->fe;
}

static struct i2c_adapter *rtl2832_get_i2c_adapter(struct i2c_client *client)
{
	struct rtl2832_dev *dev = i2c_get_clientdata(client);

	dev_dbg(&client->dev, "\n");
	return dev->muxc->adapter[0];
}

static int rtl2832_slave_ts_ctrl(struct i2c_client *client, bool enable)
{
	struct rtl2832_dev *dev = i2c_get_clientdata(client);
	int ret;

	dev_dbg(&client->dev, "enable=%d\n", enable);

	if (enable) {
		ret = rtl2832_wr_demod_reg(dev, DVBT_SOFT_RST, 0x0);
		if (ret)
			goto err;
		ret = regmap_bulk_write(dev->regmap, 0x10c, "\x5f\xff", 2);
		if (ret)
			goto err;
		ret = rtl2832_wr_demod_reg(dev, DVBT_PIP_ON, 0x1);
		if (ret)
			goto err;
		ret = regmap_bulk_write(dev->regmap, 0x0bc, "\x18", 1);
		if (ret)
			goto err;
		ret = regmap_bulk_write(dev->regmap, 0x192, "\x7f\xf7\xff", 3);
		if (ret)
			goto err;
	} else {
		ret = regmap_bulk_write(dev->regmap, 0x192, "\x00\x0f\xff", 3);
		if (ret)
			goto err;
		ret = regmap_bulk_write(dev->regmap, 0x0bc, "\x08", 1);
		if (ret)
			goto err;
		ret = rtl2832_wr_demod_reg(dev, DVBT_PIP_ON, 0x0);
		if (ret)
			goto err;
		ret = regmap_bulk_write(dev->regmap, 0x10c, "\x00\x00", 2);
		if (ret)
			goto err;
		ret = rtl2832_wr_demod_reg(dev, DVBT_SOFT_RST, 0x1);
		if (ret)
			goto err;
	}

	dev->slave_ts = enable;

	return 0;
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

static int rtl2832_pid_filter_ctrl(struct dvb_frontend *fe, int onoff)
{
	struct rtl2832_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
	int ret;
	u8 u8tmp;

	dev_dbg(&client->dev, "onoff=%d, slave_ts=%d\n", onoff, dev->slave_ts);

	/* enable / disable PID filter */
	if (onoff)
		u8tmp = 0x80;
	else
		u8tmp = 0x00;

	if (dev->slave_ts)
		ret = regmap_update_bits(dev->regmap, 0x021, 0xc0, u8tmp);
	else
		ret = regmap_update_bits(dev->regmap, 0x061, 0xc0, u8tmp);
	if (ret)
		goto err;

	return 0;
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

static int rtl2832_pid_filter(struct dvb_frontend *fe, u8 index, u16 pid,
			      int onoff)
{
	struct rtl2832_dev *dev = fe->demodulator_priv;
	struct i2c_client *client = dev->client;
	int ret;
	u8 buf[4];

	dev_dbg(&client->dev, "index=%d pid=%04x onoff=%d slave_ts=%d\n",
		index, pid, onoff, dev->slave_ts);

	/* skip invalid PIDs (0x2000) */
	if (pid > 0x1fff || index > 32)
		return 0;

	if (onoff)
		set_bit(index, &dev->filters);
	else
		clear_bit(index, &dev->filters);

	/* enable / disable PIDs */
	buf[0] = (dev->filters >>  0) & 0xff;
	buf[1] = (dev->filters >>  8) & 0xff;
	buf[2] = (dev->filters >> 16) & 0xff;
	buf[3] = (dev->filters >> 24) & 0xff;

	if (dev->slave_ts)
		ret = regmap_bulk_write(dev->regmap, 0x022, buf, 4);
	else
		ret = regmap_bulk_write(dev->regmap, 0x062, buf, 4);
	if (ret)
		goto err;

	/* add PID */
	buf[0] = (pid >> 8) & 0xff;
	buf[1] = (pid >> 0) & 0xff;

	if (dev->slave_ts)
		ret = regmap_bulk_write(dev->regmap, 0x026 + 2 * index, buf, 2);
	else
		ret = regmap_bulk_write(dev->regmap, 0x066 + 2 * index, buf, 2);
	if (ret)
		goto err;

	return 0;
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

static int rtl2832_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct rtl2832_platform_data *pdata = client->dev.platform_data;
	struct i2c_adapter *i2c = client->adapter;
	struct rtl2832_dev *dev;
	int ret;
	u8 tmp;
	static const struct regmap_range_cfg regmap_range_cfg[] = {
		{
			.selector_reg     = 0x00,
			.selector_mask    = 0xff,
			.selector_shift   = 0,
			.window_start     = 0,
			.window_len       = 0x100,
			.range_min        = 0 * 0x100,
			.range_max        = 5 * 0x100,
		},
	};

	dev_dbg(&client->dev, "\n");

	/* allocate memory for the internal state */
	dev = kzalloc(sizeof(struct rtl2832_dev), GFP_KERNEL);
	if (dev == NULL) {
		ret = -ENOMEM;
		goto err;
	}

	/* setup the state */
	i2c_set_clientdata(client, dev);
	dev->client = client;
	dev->pdata = client->dev.platform_data;
	dev->sleeping = true;
	INIT_DELAYED_WORK(&dev->i2c_gate_work, rtl2832_i2c_gate_work);
	/* create regmap */
	dev->regmap_config.reg_bits =  8,
	dev->regmap_config.val_bits =  8,
	dev->regmap_config.volatile_reg = rtl2832_volatile_reg,
	dev->regmap_config.max_register = 5 * 0x100,
	dev->regmap_config.ranges = regmap_range_cfg,
	dev->regmap_config.num_ranges = ARRAY_SIZE(regmap_range_cfg),
	dev->regmap_config.cache_type = REGCACHE_NONE,
	dev->regmap = regmap_init_i2c(client, &dev->regmap_config);
	if (IS_ERR(dev->regmap)) {
		ret = PTR_ERR(dev->regmap);
		goto err_kfree;
	}

	/* check if the demod is there */
	ret = regmap_bulk_read(dev->regmap, 0x000, &tmp, 1);
	if (ret)
		goto err_regmap_exit;

	/* create muxed i2c adapter for demod tuner bus */
	dev->muxc = i2c_mux_alloc(i2c, &i2c->dev, 1, 0, I2C_MUX_LOCKED,
				  rtl2832_select, rtl2832_deselect);
	if (!dev->muxc) {
		ret = -ENOMEM;
		goto err_regmap_exit;
	}
	dev->muxc->priv = dev;
	ret = i2c_mux_add_adapter(dev->muxc, 0, 0, 0);
	if (ret)
		goto err_regmap_exit;

	/* create dvb_frontend */
	memcpy(&dev->fe.ops, &rtl2832_ops, sizeof(struct dvb_frontend_ops));
	dev->fe.demodulator_priv = dev;

	/* setup callbacks */
	pdata->get_dvb_frontend = rtl2832_get_dvb_frontend;
	pdata->get_i2c_adapter = rtl2832_get_i2c_adapter;
	pdata->slave_ts_ctrl = rtl2832_slave_ts_ctrl;
	pdata->pid_filter = rtl2832_pid_filter;
	pdata->pid_filter_ctrl = rtl2832_pid_filter_ctrl;
	pdata->regmap = dev->regmap;

	dev_info(&client->dev, "Realtek RTL2832 successfully attached\n");
	return 0;
err_regmap_exit:
	regmap_exit(dev->regmap);
err_kfree:
	kfree(dev);
err:
	dev_dbg(&client->dev, "failed=%d\n", ret);
	return ret;
}

static int rtl2832_remove(struct i2c_client *client)
{
	struct rtl2832_dev *dev = i2c_get_clientdata(client);

	dev_dbg(&client->dev, "\n");

	cancel_delayed_work_sync(&dev->i2c_gate_work);

	i2c_mux_del_adapters(dev->muxc);

	regmap_exit(dev->regmap);

	kfree(dev);

	return 0;
}

static const struct i2c_device_id rtl2832_id_table[] = {
	{"rtl2832", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, rtl2832_id_table);

static struct i2c_driver rtl2832_driver = {
	.driver = {
		.name	= "rtl2832",
		.suppress_bind_attrs	= true,
	},
	.probe		= rtl2832_probe,
	.remove		= rtl2832_remove,
	.id_table	= rtl2832_id_table,
};

module_i2c_driver(rtl2832_driver);

MODULE_AUTHOR("Thomas Mair <mair.thomas86@gmail.com>");
MODULE_AUTHOR("Antti Palosaari <crope@iki.fi>");
MODULE_DESCRIPTION("Realtek RTL2832 DVB-T demodulator driver");
MODULE_LICENSE("GPL");
>>>>>>> v4.9.227
