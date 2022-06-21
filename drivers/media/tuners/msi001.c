/*
 * Mirics MSi001 silicon tuner driver
 *
 * Copyright (C) 2013 Antti Palosaari <crope@iki.fi>
 * Copyright (C) 2014 Antti Palosaari <crope@iki.fi>
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
 */

#include <linux/module.h>
#include <linux/gcd.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ctrls.h>

static const struct v4l2_frequency_band bands[] = {
	{
		.type = V4L2_TUNER_RF,
		.index = 0,
		.capability = V4L2_TUNER_CAP_1HZ | V4L2_TUNER_CAP_FREQ_BANDS,
		.rangelow   =   49000000,
		.rangehigh  =  263000000,
	}, {
		.type = V4L2_TUNER_RF,
		.index = 1,
		.capability = V4L2_TUNER_CAP_1HZ | V4L2_TUNER_CAP_FREQ_BANDS,
		.rangelow   =  390000000,
		.rangehigh  =  960000000,
	},
};

<<<<<<< HEAD
struct msi001 {
=======
struct msi001_dev {
>>>>>>> v4.9.227
	struct spi_device *spi;
	struct v4l2_subdev sd;

	/* Controls */
	struct v4l2_ctrl_handler hdl;
	struct v4l2_ctrl *bandwidth_auto;
	struct v4l2_ctrl *bandwidth;
	struct v4l2_ctrl *lna_gain;
	struct v4l2_ctrl *mixer_gain;
	struct v4l2_ctrl *if_gain;

	unsigned int f_tuner;
};

<<<<<<< HEAD
static inline struct msi001 *sd_to_msi001(struct v4l2_subdev *sd)
{
	return container_of(sd, struct msi001, sd);
}

static int msi001_wreg(struct msi001 *s, u32 data)
{
	/* Register format: 4 bits addr + 20 bits value */
	return spi_write(s->spi, &data, 3);
};

static int msi001_set_gain(struct msi001 *s, int lna_gain, int mixer_gain,
		int if_gain)
{
	int ret;
	u32 reg;

	dev_dbg(&s->spi->dev, "lna=%d mixer=%d if=%d\n",
			lna_gain, mixer_gain, if_gain);
=======
static inline struct msi001_dev *sd_to_msi001_dev(struct v4l2_subdev *sd)
{
	return container_of(sd, struct msi001_dev, sd);
}

static int msi001_wreg(struct msi001_dev *dev, u32 data)
{
	/* Register format: 4 bits addr + 20 bits value */
	return spi_write(dev->spi, &data, 3);
};

static int msi001_set_gain(struct msi001_dev *dev, int lna_gain, int mixer_gain,
			   int if_gain)
{
	struct spi_device *spi = dev->spi;
	int ret;
	u32 reg;

	dev_dbg(&spi->dev, "lna=%d mixer=%d if=%d\n",
		lna_gain, mixer_gain, if_gain);
>>>>>>> v4.9.227

	reg = 1 << 0;
	reg |= (59 - if_gain) << 4;
	reg |= 0 << 10;
	reg |= (1 - mixer_gain) << 12;
	reg |= (1 - lna_gain) << 13;
	reg |= 4 << 14;
	reg |= 0 << 17;
<<<<<<< HEAD
	ret = msi001_wreg(s, reg);
=======
	ret = msi001_wreg(dev, reg);
>>>>>>> v4.9.227
	if (ret)
		goto err;

	return 0;
err:
<<<<<<< HEAD
	dev_dbg(&s->spi->dev, "failed %d\n", ret);
	return ret;
};

static int msi001_set_tuner(struct msi001 *s)
{
	int ret, i;
	unsigned int n, m, thresh, frac, vco_step, tmp, f_if1;
	u32 reg;
	u64 f_vco, tmp64;
	u8 mode, filter_mode, lo_div;
=======
	dev_dbg(&spi->dev, "failed %d\n", ret);
	return ret;
};

static int msi001_set_tuner(struct msi001_dev *dev)
{
	struct spi_device *spi = dev->spi;
	int ret, i;
	unsigned int uitmp, div_n, k, k_thresh, k_frac, div_lo, f_if1;
	u32 reg;
	u64 f_vco;
	u8 mode, filter_mode;
>>>>>>> v4.9.227

	static const struct {
		u32 rf;
		u8 mode;
<<<<<<< HEAD
		u8 lo_div;
=======
		u8 div_lo;
>>>>>>> v4.9.227
	} band_lut[] = {
		{ 50000000, 0xe1, 16}, /* AM_MODE2, antenna 2 */
		{108000000, 0x42, 32}, /* VHF_MODE */
		{330000000, 0x44, 16}, /* B3_MODE */
		{960000000, 0x48,  4}, /* B45_MODE */
		{      ~0U, 0x50,  2}, /* BL_MODE */
	};
	static const struct {
		u32 freq;
		u8 filter_mode;
	} if_freq_lut[] = {
		{      0, 0x03}, /* Zero IF */
		{ 450000, 0x02}, /* 450 kHz IF */
		{1620000, 0x01}, /* 1.62 MHz IF */
		{2048000, 0x00}, /* 2.048 MHz IF */
	};
	static const struct {
		u32 freq;
		u8 val;
	} bandwidth_lut[] = {
		{ 200000, 0x00}, /* 200 kHz */
		{ 300000, 0x01}, /* 300 kHz */
		{ 600000, 0x02}, /* 600 kHz */
		{1536000, 0x03}, /* 1.536 MHz */
		{5000000, 0x04}, /* 5 MHz */
		{6000000, 0x05}, /* 6 MHz */
		{7000000, 0x06}, /* 7 MHz */
		{8000000, 0x07}, /* 8 MHz */
	};

<<<<<<< HEAD
	unsigned int f_rf = s->f_tuner;
=======
	unsigned int f_rf = dev->f_tuner;
>>>>>>> v4.9.227

	/*
	 * bandwidth (Hz)
	 * 200000, 300000, 600000, 1536000, 5000000, 6000000, 7000000, 8000000
	 */
	unsigned int bandwidth;

	/*
	 * intermediate frequency (Hz)
	 * 0, 450000, 1620000, 2048000
	 */
	unsigned int f_if = 0;
	#define F_REF 24000000
<<<<<<< HEAD
	#define R_REF 4
	#define F_OUT_STEP 1

	dev_dbg(&s->spi->dev, "f_rf=%d f_if=%d\n", f_rf, f_if);
=======
	#define DIV_PRE_N 4
	#define	F_VCO_STEP div_lo

	dev_dbg(&spi->dev, "f_rf=%d f_if=%d\n", f_rf, f_if);
>>>>>>> v4.9.227

	for (i = 0; i < ARRAY_SIZE(band_lut); i++) {
		if (f_rf <= band_lut[i].rf) {
			mode = band_lut[i].mode;
<<<<<<< HEAD
			lo_div = band_lut[i].lo_div;
			break;
		}
	}

=======
			div_lo = band_lut[i].div_lo;
			break;
		}
	}
>>>>>>> v4.9.227
	if (i == ARRAY_SIZE(band_lut)) {
		ret = -EINVAL;
		goto err;
	}

	/* AM_MODE is upconverted */
	if ((mode >> 0) & 0x1)
		f_if1 =  5 * F_REF;
	else
		f_if1 =  0;

	for (i = 0; i < ARRAY_SIZE(if_freq_lut); i++) {
		if (f_if == if_freq_lut[i].freq) {
			filter_mode = if_freq_lut[i].filter_mode;
			break;
		}
	}
<<<<<<< HEAD

=======
>>>>>>> v4.9.227
	if (i == ARRAY_SIZE(if_freq_lut)) {
		ret = -EINVAL;
		goto err;
	}

	/* filters */
<<<<<<< HEAD
	bandwidth = s->bandwidth->val;
=======
	bandwidth = dev->bandwidth->val;
>>>>>>> v4.9.227
	bandwidth = clamp(bandwidth, 200000U, 8000000U);

	for (i = 0; i < ARRAY_SIZE(bandwidth_lut); i++) {
		if (bandwidth <= bandwidth_lut[i].freq) {
			bandwidth = bandwidth_lut[i].val;
			break;
		}
	}
<<<<<<< HEAD

=======
>>>>>>> v4.9.227
	if (i == ARRAY_SIZE(bandwidth_lut)) {
		ret = -EINVAL;
		goto err;
	}

<<<<<<< HEAD
	s->bandwidth->val = bandwidth_lut[i].freq;

	dev_dbg(&s->spi->dev, "bandwidth selected=%d\n", bandwidth_lut[i].freq);

	f_vco = (u64) (f_rf + f_if + f_if1) * lo_div;
	tmp64 = f_vco;
	m = do_div(tmp64, F_REF * R_REF);
	n = (unsigned int) tmp64;

	vco_step = F_OUT_STEP * lo_div;
	thresh = (F_REF * R_REF) / vco_step;
	frac = 1ul * thresh * m / (F_REF * R_REF);

	/* Find out greatest common divisor and divide to smaller. */
	tmp = gcd(thresh, frac);
	thresh /= tmp;
	frac /= tmp;

	/* Force divide to reg max. Resolution will be reduced. */
	tmp = DIV_ROUND_UP(thresh, 4095);
	thresh = DIV_ROUND_CLOSEST(thresh, tmp);
	frac = DIV_ROUND_CLOSEST(frac, tmp);

	/* calc real RF set */
	tmp = 1ul * F_REF * R_REF * n;
	tmp += 1ul * F_REF * R_REF * frac / thresh;
	tmp /= lo_div;

	dev_dbg(&s->spi->dev, "rf=%u:%u n=%d thresh=%d frac=%d\n",
				f_rf, tmp, n, thresh, frac);

	ret = msi001_wreg(s, 0x00000e);
	if (ret)
		goto err;

	ret = msi001_wreg(s, 0x000003);
=======
	dev->bandwidth->val = bandwidth_lut[i].freq;

	dev_dbg(&spi->dev, "bandwidth selected=%d\n", bandwidth_lut[i].freq);

	/*
	 * Fractional-N synthesizer
	 *
	 *           +---------------------------------------+
	 *           v                                       |
	 *  Fref   +----+     +-------+         +----+     +------+     +---+
	 * ------> | PD | --> |  VCO  | ------> | /4 | --> | /N.F | <-- | K |
	 *         +----+     +-------+         +----+     +------+     +---+
	 *                      |
	 *                      |
	 *                      v
	 *                    +-------+  Fout
	 *                    | /Rout | ------>
	 *                    +-------+
	 */

	/* Calculate PLL integer and fractional control word. */
	f_vco = (u64) (f_rf + f_if + f_if1) * div_lo;
	div_n = div_u64_rem(f_vco, DIV_PRE_N * F_REF, &k);
	k_thresh = (DIV_PRE_N * F_REF) / F_VCO_STEP;
	k_frac = div_u64((u64) k * k_thresh, (DIV_PRE_N * F_REF));

	/* Find out greatest common divisor and divide to smaller. */
	uitmp = gcd(k_thresh, k_frac);
	k_thresh /= uitmp;
	k_frac /= uitmp;

	/* Force divide to reg max. Resolution will be reduced. */
	uitmp = DIV_ROUND_UP(k_thresh, 4095);
	k_thresh = DIV_ROUND_CLOSEST(k_thresh, uitmp);
	k_frac = DIV_ROUND_CLOSEST(k_frac, uitmp);

	/* Calculate real RF set. */
	uitmp = (unsigned int) F_REF * DIV_PRE_N * div_n;
	uitmp += (unsigned int) F_REF * DIV_PRE_N * k_frac / k_thresh;
	uitmp /= div_lo;

	dev_dbg(&spi->dev,
		"f_rf=%u:%u f_vco=%llu div_n=%u k_thresh=%u k_frac=%u div_lo=%u\n",
		f_rf, uitmp, f_vco, div_n, k_thresh, k_frac, div_lo);

	ret = msi001_wreg(dev, 0x00000e);
	if (ret)
		goto err;

	ret = msi001_wreg(dev, 0x000003);
>>>>>>> v4.9.227
	if (ret)
		goto err;

	reg = 0 << 0;
	reg |= mode << 4;
	reg |= filter_mode << 12;
	reg |= bandwidth << 14;
	reg |= 0x02 << 17;
	reg |= 0x00 << 20;
<<<<<<< HEAD
	ret = msi001_wreg(s, reg);
=======
	ret = msi001_wreg(dev, reg);
>>>>>>> v4.9.227
	if (ret)
		goto err;

	reg = 5 << 0;
<<<<<<< HEAD
	reg |= thresh << 4;
	reg |= 1 << 19;
	reg |= 1 << 21;
	ret = msi001_wreg(s, reg);
=======
	reg |= k_thresh << 4;
	reg |= 1 << 19;
	reg |= 1 << 21;
	ret = msi001_wreg(dev, reg);
>>>>>>> v4.9.227
	if (ret)
		goto err;

	reg = 2 << 0;
<<<<<<< HEAD
	reg |= frac << 4;
	reg |= n << 16;
	ret = msi001_wreg(s, reg);
	if (ret)
		goto err;

	ret = msi001_set_gain(s, s->lna_gain->cur.val, s->mixer_gain->cur.val,
			s->if_gain->cur.val);
=======
	reg |= k_frac << 4;
	reg |= div_n << 16;
	ret = msi001_wreg(dev, reg);
	if (ret)
		goto err;

	ret = msi001_set_gain(dev, dev->lna_gain->cur.val,
			      dev->mixer_gain->cur.val, dev->if_gain->cur.val);
>>>>>>> v4.9.227
	if (ret)
		goto err;

	reg = 6 << 0;
	reg |= 63 << 4;
	reg |= 4095 << 10;
<<<<<<< HEAD
	ret = msi001_wreg(s, reg);
=======
	ret = msi001_wreg(dev, reg);
>>>>>>> v4.9.227
	if (ret)
		goto err;

	return 0;
err:
<<<<<<< HEAD
	dev_dbg(&s->spi->dev, "failed %d\n", ret);
	return ret;
};

static int msi001_s_power(struct v4l2_subdev *sd, int on)
{
	struct msi001 *s = sd_to_msi001(sd);
	int ret;

	dev_dbg(&s->spi->dev, "on=%d\n", on);
=======
	dev_dbg(&spi->dev, "failed %d\n", ret);
	return ret;
}

static int msi001_s_power(struct v4l2_subdev *sd, int on)
{
	struct msi001_dev *dev = sd_to_msi001_dev(sd);
	struct spi_device *spi = dev->spi;
	int ret;

	dev_dbg(&spi->dev, "on=%d\n", on);
>>>>>>> v4.9.227

	if (on)
		ret = 0;
	else
<<<<<<< HEAD
		ret = msi001_wreg(s, 0x000000);
=======
		ret = msi001_wreg(dev, 0x000000);
>>>>>>> v4.9.227

	return ret;
}

static const struct v4l2_subdev_core_ops msi001_core_ops = {
	.s_power                  = msi001_s_power,
};

static int msi001_g_tuner(struct v4l2_subdev *sd, struct v4l2_tuner *v)
{
<<<<<<< HEAD
	struct msi001 *s = sd_to_msi001(sd);

	dev_dbg(&s->spi->dev, "index=%d\n", v->index);
=======
	struct msi001_dev *dev = sd_to_msi001_dev(sd);
	struct spi_device *spi = dev->spi;

	dev_dbg(&spi->dev, "index=%d\n", v->index);
>>>>>>> v4.9.227

	strlcpy(v->name, "Mirics MSi001", sizeof(v->name));
	v->type = V4L2_TUNER_RF;
	v->capability = V4L2_TUNER_CAP_1HZ | V4L2_TUNER_CAP_FREQ_BANDS;
	v->rangelow =    49000000;
	v->rangehigh =  960000000;

	return 0;
}

static int msi001_s_tuner(struct v4l2_subdev *sd, const struct v4l2_tuner *v)
{
<<<<<<< HEAD
	struct msi001 *s = sd_to_msi001(sd);

	dev_dbg(&s->spi->dev, "index=%d\n", v->index);
=======
	struct msi001_dev *dev = sd_to_msi001_dev(sd);
	struct spi_device *spi = dev->spi;

	dev_dbg(&spi->dev, "index=%d\n", v->index);
>>>>>>> v4.9.227
	return 0;
}

static int msi001_g_frequency(struct v4l2_subdev *sd, struct v4l2_frequency *f)
{
<<<<<<< HEAD
	struct msi001 *s = sd_to_msi001(sd);

	dev_dbg(&s->spi->dev, "tuner=%d\n", f->tuner);
	f->frequency = s->f_tuner;
=======
	struct msi001_dev *dev = sd_to_msi001_dev(sd);
	struct spi_device *spi = dev->spi;

	dev_dbg(&spi->dev, "tuner=%d\n", f->tuner);
	f->frequency = dev->f_tuner;
>>>>>>> v4.9.227
	return 0;
}

static int msi001_s_frequency(struct v4l2_subdev *sd,
<<<<<<< HEAD
		const struct v4l2_frequency *f)
{
	struct msi001 *s = sd_to_msi001(sd);
	unsigned int band;

	dev_dbg(&s->spi->dev, "tuner=%d type=%d frequency=%u\n",
			f->tuner, f->type, f->frequency);
=======
			      const struct v4l2_frequency *f)
{
	struct msi001_dev *dev = sd_to_msi001_dev(sd);
	struct spi_device *spi = dev->spi;
	unsigned int band;

	dev_dbg(&spi->dev, "tuner=%d type=%d frequency=%u\n",
		f->tuner, f->type, f->frequency);
>>>>>>> v4.9.227

	if (f->frequency < ((bands[0].rangehigh + bands[1].rangelow) / 2))
		band = 0;
	else
		band = 1;
<<<<<<< HEAD
	s->f_tuner = clamp_t(unsigned int, f->frequency,
			bands[band].rangelow, bands[band].rangehigh);

	return msi001_set_tuner(s);
}

static int msi001_enum_freq_bands(struct v4l2_subdev *sd,
		struct v4l2_frequency_band *band)
{
	struct msi001 *s = sd_to_msi001(sd);

	dev_dbg(&s->spi->dev, "tuner=%d type=%d index=%d\n",
			band->tuner, band->type, band->index);
=======
	dev->f_tuner = clamp_t(unsigned int, f->frequency,
			       bands[band].rangelow, bands[band].rangehigh);

	return msi001_set_tuner(dev);
}

static int msi001_enum_freq_bands(struct v4l2_subdev *sd,
				  struct v4l2_frequency_band *band)
{
	struct msi001_dev *dev = sd_to_msi001_dev(sd);
	struct spi_device *spi = dev->spi;

	dev_dbg(&spi->dev, "tuner=%d type=%d index=%d\n",
		band->tuner, band->type, band->index);
>>>>>>> v4.9.227

	if (band->index >= ARRAY_SIZE(bands))
		return -EINVAL;

	band->capability = bands[band->index].capability;
	band->rangelow = bands[band->index].rangelow;
	band->rangehigh = bands[band->index].rangehigh;

	return 0;
}

static const struct v4l2_subdev_tuner_ops msi001_tuner_ops = {
	.g_tuner                  = msi001_g_tuner,
	.s_tuner                  = msi001_s_tuner,
	.g_frequency              = msi001_g_frequency,
	.s_frequency              = msi001_s_frequency,
	.enum_freq_bands          = msi001_enum_freq_bands,
};

static const struct v4l2_subdev_ops msi001_ops = {
	.core                     = &msi001_core_ops,
	.tuner                    = &msi001_tuner_ops,
};

static int msi001_s_ctrl(struct v4l2_ctrl *ctrl)
{
<<<<<<< HEAD
	struct msi001 *s = container_of(ctrl->handler, struct msi001, hdl);

	int ret;

	dev_dbg(&s->spi->dev,
			"id=%d name=%s val=%d min=%lld max=%lld step=%lld\n",
			ctrl->id, ctrl->name, ctrl->val,
			ctrl->minimum, ctrl->maximum, ctrl->step);
=======
	struct msi001_dev *dev = container_of(ctrl->handler, struct msi001_dev, hdl);
	struct spi_device *spi = dev->spi;

	int ret;

	dev_dbg(&spi->dev, "id=%d name=%s val=%d min=%lld max=%lld step=%lld\n",
		ctrl->id, ctrl->name, ctrl->val, ctrl->minimum, ctrl->maximum,
		ctrl->step);
>>>>>>> v4.9.227

	switch (ctrl->id) {
	case V4L2_CID_RF_TUNER_BANDWIDTH_AUTO:
	case V4L2_CID_RF_TUNER_BANDWIDTH:
<<<<<<< HEAD
		ret = msi001_set_tuner(s);
		break;
	case  V4L2_CID_RF_TUNER_LNA_GAIN:
		ret = msi001_set_gain(s, s->lna_gain->val,
				s->mixer_gain->cur.val, s->if_gain->cur.val);
		break;
	case  V4L2_CID_RF_TUNER_MIXER_GAIN:
		ret = msi001_set_gain(s, s->lna_gain->cur.val,
				s->mixer_gain->val, s->if_gain->cur.val);
		break;
	case  V4L2_CID_RF_TUNER_IF_GAIN:
		ret = msi001_set_gain(s, s->lna_gain->cur.val,
				s->mixer_gain->cur.val, s->if_gain->val);
		break;
	default:
		dev_dbg(&s->spi->dev, "unkown control %d\n", ctrl->id);
=======
		ret = msi001_set_tuner(dev);
		break;
	case  V4L2_CID_RF_TUNER_LNA_GAIN:
		ret = msi001_set_gain(dev, dev->lna_gain->val,
				      dev->mixer_gain->cur.val,
				      dev->if_gain->cur.val);
		break;
	case  V4L2_CID_RF_TUNER_MIXER_GAIN:
		ret = msi001_set_gain(dev, dev->lna_gain->cur.val,
				      dev->mixer_gain->val,
				      dev->if_gain->cur.val);
		break;
	case  V4L2_CID_RF_TUNER_IF_GAIN:
		ret = msi001_set_gain(dev, dev->lna_gain->cur.val,
				      dev->mixer_gain->cur.val,
				      dev->if_gain->val);
		break;
	default:
		dev_dbg(&spi->dev, "unknown control %d\n", ctrl->id);
>>>>>>> v4.9.227
		ret = -EINVAL;
	}

	return ret;
}

static const struct v4l2_ctrl_ops msi001_ctrl_ops = {
	.s_ctrl                   = msi001_s_ctrl,
};

static int msi001_probe(struct spi_device *spi)
{
<<<<<<< HEAD
	struct msi001 *s;
=======
	struct msi001_dev *dev;
>>>>>>> v4.9.227
	int ret;

	dev_dbg(&spi->dev, "\n");

<<<<<<< HEAD
	s = kzalloc(sizeof(struct msi001), GFP_KERNEL);
	if (s == NULL) {
		ret = -ENOMEM;
		dev_dbg(&spi->dev, "Could not allocate memory for msi001\n");
		goto err_kfree;
	}

	s->spi = spi;
	s->f_tuner = bands[0].rangelow;
	v4l2_spi_subdev_init(&s->sd, spi, &msi001_ops);

	/* Register controls */
	v4l2_ctrl_handler_init(&s->hdl, 5);
	s->bandwidth_auto = v4l2_ctrl_new_std(&s->hdl, &msi001_ctrl_ops,
			V4L2_CID_RF_TUNER_BANDWIDTH_AUTO, 0, 1, 1, 1);
	s->bandwidth = v4l2_ctrl_new_std(&s->hdl, &msi001_ctrl_ops,
			V4L2_CID_RF_TUNER_BANDWIDTH, 200000, 8000000, 1, 200000);
	v4l2_ctrl_auto_cluster(2, &s->bandwidth_auto, 0, false);
	s->lna_gain = v4l2_ctrl_new_std(&s->hdl, &msi001_ctrl_ops,
			V4L2_CID_RF_TUNER_LNA_GAIN, 0, 1, 1, 1);
	s->mixer_gain = v4l2_ctrl_new_std(&s->hdl, &msi001_ctrl_ops,
			V4L2_CID_RF_TUNER_MIXER_GAIN, 0, 1, 1, 1);
	s->if_gain = v4l2_ctrl_new_std(&s->hdl, &msi001_ctrl_ops,
			V4L2_CID_RF_TUNER_IF_GAIN, 0, 59, 1, 0);
	if (s->hdl.error) {
		ret = s->hdl.error;
		dev_err(&s->spi->dev, "Could not initialize controls\n");
=======
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev) {
		ret = -ENOMEM;
		goto err;
	}

	dev->spi = spi;
	dev->f_tuner = bands[0].rangelow;
	v4l2_spi_subdev_init(&dev->sd, spi, &msi001_ops);

	/* Register controls */
	v4l2_ctrl_handler_init(&dev->hdl, 5);
	dev->bandwidth_auto = v4l2_ctrl_new_std(&dev->hdl, &msi001_ctrl_ops,
			V4L2_CID_RF_TUNER_BANDWIDTH_AUTO, 0, 1, 1, 1);
	dev->bandwidth = v4l2_ctrl_new_std(&dev->hdl, &msi001_ctrl_ops,
			V4L2_CID_RF_TUNER_BANDWIDTH, 200000, 8000000, 1, 200000);
	v4l2_ctrl_auto_cluster(2, &dev->bandwidth_auto, 0, false);
	dev->lna_gain = v4l2_ctrl_new_std(&dev->hdl, &msi001_ctrl_ops,
			V4L2_CID_RF_TUNER_LNA_GAIN, 0, 1, 1, 1);
	dev->mixer_gain = v4l2_ctrl_new_std(&dev->hdl, &msi001_ctrl_ops,
			V4L2_CID_RF_TUNER_MIXER_GAIN, 0, 1, 1, 1);
	dev->if_gain = v4l2_ctrl_new_std(&dev->hdl, &msi001_ctrl_ops,
			V4L2_CID_RF_TUNER_IF_GAIN, 0, 59, 1, 0);
	if (dev->hdl.error) {
		ret = dev->hdl.error;
		dev_err(&spi->dev, "Could not initialize controls\n");
>>>>>>> v4.9.227
		/* control init failed, free handler */
		goto err_ctrl_handler_free;
	}

<<<<<<< HEAD
	s->sd.ctrl_handler = &s->hdl;
	return 0;

err_ctrl_handler_free:
	v4l2_ctrl_handler_free(&s->hdl);
err_kfree:
	kfree(s);
=======
	dev->sd.ctrl_handler = &dev->hdl;
	return 0;
err_ctrl_handler_free:
	v4l2_ctrl_handler_free(&dev->hdl);
	kfree(dev);
err:
>>>>>>> v4.9.227
	return ret;
}

static int msi001_remove(struct spi_device *spi)
{
	struct v4l2_subdev *sd = spi_get_drvdata(spi);
<<<<<<< HEAD
	struct msi001 *s = sd_to_msi001(sd);
=======
	struct msi001_dev *dev = sd_to_msi001_dev(sd);
>>>>>>> v4.9.227

	dev_dbg(&spi->dev, "\n");

	/*
	 * Registered by v4l2_spi_new_subdev() from master driver, but we must
	 * unregister it from here. Weird.
	 */
<<<<<<< HEAD
	v4l2_device_unregister_subdev(&s->sd);
	v4l2_ctrl_handler_free(&s->hdl);
	kfree(s);
	return 0;
}

static const struct spi_device_id msi001_id[] = {
	{"msi001", 0},
	{}
};
MODULE_DEVICE_TABLE(spi, msi001_id);
=======
	v4l2_device_unregister_subdev(&dev->sd);
	v4l2_ctrl_handler_free(&dev->hdl);
	kfree(dev);
	return 0;
}

static const struct spi_device_id msi001_id_table[] = {
	{"msi001", 0},
	{}
};
MODULE_DEVICE_TABLE(spi, msi001_id_table);
>>>>>>> v4.9.227

static struct spi_driver msi001_driver = {
	.driver = {
		.name	= "msi001",
<<<<<<< HEAD
		.owner	= THIS_MODULE,
	},
	.probe		= msi001_probe,
	.remove		= msi001_remove,
	.id_table	= msi001_id,
=======
		.suppress_bind_attrs = true,
	},
	.probe		= msi001_probe,
	.remove		= msi001_remove,
	.id_table	= msi001_id_table,
>>>>>>> v4.9.227
};
module_spi_driver(msi001_driver);

MODULE_AUTHOR("Antti Palosaari <crope@iki.fi>");
MODULE_DESCRIPTION("Mirics MSi001");
MODULE_LICENSE("GPL");
