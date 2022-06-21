/* Frontend part of the Linux driver for the WideView/ Yakumo/ Hama/
 * Typhoon/ Yuan DVB-T USB2.0 receiver.
 *
<<<<<<< HEAD
 * Copyright (C) 2005 Patrick Boettcher <patrick.boettcher@desy.de>
=======
 * Copyright (C) 2005 Patrick Boettcher <patrick.boettcher@posteo.de>
>>>>>>> v4.9.227
 *
 *	This program is free software; you can redistribute it and/or modify it
 *	under the terms of the GNU General Public License as published by the Free
 *	Software Foundation, version 2.
 *
 * see Documentation/dvb/README.dvb-usb for more information
 */
#include "dtt200u.h"

struct dtt200u_fe_state {
	struct dvb_usb_device *d;

<<<<<<< HEAD
	fe_status_t stat;

	struct dtv_frontend_properties fep;
	struct dvb_frontend frontend;
};

static int dtt200u_fe_read_status(struct dvb_frontend* fe, fe_status_t *stat)
{
	struct dtt200u_fe_state *state = fe->demodulator_priv;
	u8 st = GET_TUNE_STATUS, b[3];

	dvb_usb_generic_rw(state->d,&st,1,b,3,0);

	switch (b[0]) {
=======
	enum fe_status stat;

	struct dtv_frontend_properties fep;
	struct dvb_frontend frontend;

	unsigned char data[80];
	struct mutex data_mutex;
};

static int dtt200u_fe_read_status(struct dvb_frontend *fe,
				  enum fe_status *stat)
{
	struct dtt200u_fe_state *state = fe->demodulator_priv;
	int ret;

	mutex_lock(&state->data_mutex);
	state->data[0] = GET_TUNE_STATUS;

	ret = dvb_usb_generic_rw(state->d, state->data, 1, state->data, 3, 0);
	if (ret < 0) {
		*stat = 0;
		mutex_unlock(&state->data_mutex);
		return ret;
	}

	switch (state->data[0]) {
>>>>>>> v4.9.227
		case 0x01:
			*stat = FE_HAS_SIGNAL | FE_HAS_CARRIER |
				FE_HAS_VITERBI | FE_HAS_SYNC | FE_HAS_LOCK;
			break;
		case 0x00: /* pending */
			*stat = FE_TIMEDOUT; /* during set_frontend */
			break;
		default:
		case 0x02: /* failed */
			*stat = 0;
			break;
	}
<<<<<<< HEAD
=======
	mutex_unlock(&state->data_mutex);
>>>>>>> v4.9.227
	return 0;
}

static int dtt200u_fe_read_ber(struct dvb_frontend* fe, u32 *ber)
{
	struct dtt200u_fe_state *state = fe->demodulator_priv;
<<<<<<< HEAD
	u8 bw = GET_VIT_ERR_CNT,b[3];
	dvb_usb_generic_rw(state->d,&bw,1,b,3,0);
	*ber = (b[0] << 16) | (b[1] << 8) | b[2];
	return 0;
=======
	int ret;

	mutex_lock(&state->data_mutex);
	state->data[0] = GET_VIT_ERR_CNT;

	ret = dvb_usb_generic_rw(state->d, state->data, 1, state->data, 3, 0);
	if (ret >= 0)
		*ber = (state->data[0] << 16) | (state->data[1] << 8) | state->data[2];

	mutex_unlock(&state->data_mutex);
	return ret;
>>>>>>> v4.9.227
}

static int dtt200u_fe_read_unc_blocks(struct dvb_frontend* fe, u32 *unc)
{
	struct dtt200u_fe_state *state = fe->demodulator_priv;
<<<<<<< HEAD
	u8 bw = GET_RS_UNCOR_BLK_CNT,b[2];

	dvb_usb_generic_rw(state->d,&bw,1,b,2,0);
	*unc = (b[0] << 8) | b[1];
	return 0;
=======
	int ret;

	mutex_lock(&state->data_mutex);
	state->data[0] = GET_RS_UNCOR_BLK_CNT;

	ret = dvb_usb_generic_rw(state->d, state->data, 1, state->data, 2, 0);
	if (ret >= 0)
		*unc = (state->data[0] << 8) | state->data[1];

	mutex_unlock(&state->data_mutex);
	return ret;
>>>>>>> v4.9.227
}

static int dtt200u_fe_read_signal_strength(struct dvb_frontend* fe, u16 *strength)
{
	struct dtt200u_fe_state *state = fe->demodulator_priv;
<<<<<<< HEAD
	u8 bw = GET_AGC, b;
	dvb_usb_generic_rw(state->d,&bw,1,&b,1,0);
	*strength = (b << 8) | b;
	return 0;
=======
	int ret;

	mutex_lock(&state->data_mutex);
	state->data[0] = GET_AGC;

	ret = dvb_usb_generic_rw(state->d, state->data, 1, state->data, 1, 0);
	if (ret >= 0)
		*strength = (state->data[0] << 8) | state->data[0];

	mutex_unlock(&state->data_mutex);
	return ret;
>>>>>>> v4.9.227
}

static int dtt200u_fe_read_snr(struct dvb_frontend* fe, u16 *snr)
{
	struct dtt200u_fe_state *state = fe->demodulator_priv;
<<<<<<< HEAD
	u8 bw = GET_SNR,br;
	dvb_usb_generic_rw(state->d,&bw,1,&br,1,0);
	*snr = ~((br << 8) | br);
	return 0;
=======
	int ret;

	mutex_lock(&state->data_mutex);
	state->data[0] = GET_SNR;

	ret = dvb_usb_generic_rw(state->d, state->data, 1, state->data, 1, 0);
	if (ret >= 0)
		*snr = ~((state->data[0] << 8) | state->data[0]);

	mutex_unlock(&state->data_mutex);
	return ret;
>>>>>>> v4.9.227
}

static int dtt200u_fe_init(struct dvb_frontend* fe)
{
	struct dtt200u_fe_state *state = fe->demodulator_priv;
<<<<<<< HEAD
	u8 b = SET_INIT;
	return dvb_usb_generic_write(state->d,&b,1);
=======
	int ret;

	mutex_lock(&state->data_mutex);
	state->data[0] = SET_INIT;

	ret = dvb_usb_generic_write(state->d, state->data, 1);
	mutex_unlock(&state->data_mutex);

	return ret;
>>>>>>> v4.9.227
}

static int dtt200u_fe_sleep(struct dvb_frontend* fe)
{
	return dtt200u_fe_init(fe);
}

static int dtt200u_fe_get_tune_settings(struct dvb_frontend* fe, struct dvb_frontend_tune_settings *tune)
{
	tune->min_delay_ms = 1500;
	tune->step_size = 0;
	tune->max_drift = 0;
	return 0;
}

static int dtt200u_fe_set_frontend(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *fep = &fe->dtv_property_cache;
	struct dtt200u_fe_state *state = fe->demodulator_priv;
<<<<<<< HEAD
	int i;
	fe_status_t st;
	u16 freq = fep->frequency / 250000;
	u8 bwbuf[2] = { SET_BANDWIDTH, 0 },freqbuf[3] = { SET_RF_FREQ, 0, 0 };

	switch (fep->bandwidth_hz) {
	case 8000000:
		bwbuf[1] = 8;
		break;
	case 7000000:
		bwbuf[1] = 7;
		break;
	case 6000000:
		bwbuf[1] = 6;
		break;
	default:
		return -EINVAL;
	}

	dvb_usb_generic_write(state->d,bwbuf,2);

	freqbuf[1] = freq & 0xff;
	freqbuf[2] = (freq >> 8) & 0xff;
	dvb_usb_generic_write(state->d,freqbuf,3);

	for (i = 0; i < 30; i++) {
		msleep(20);
		dtt200u_fe_read_status(fe, &st);
		if (st & FE_TIMEDOUT)
			continue;
	}

	return 0;
}

static int dtt200u_fe_get_frontend(struct dvb_frontend* fe)
{
	struct dtv_frontend_properties *fep = &fe->dtv_property_cache;
	struct dtt200u_fe_state *state = fe->demodulator_priv;
=======
	int ret;
	u16 freq = fep->frequency / 250000;

	mutex_lock(&state->data_mutex);
	state->data[0] = SET_BANDWIDTH;
	switch (fep->bandwidth_hz) {
	case 8000000:
		state->data[1] = 8;
		break;
	case 7000000:
		state->data[1] = 7;
		break;
	case 6000000:
		state->data[1] = 6;
		break;
	default:
		ret = -EINVAL;
		goto ret;
	}

	ret = dvb_usb_generic_write(state->d, state->data, 2);
	if (ret < 0)
		goto ret;

	state->data[0] = SET_RF_FREQ;
	state->data[1] = freq & 0xff;
	state->data[2] = (freq >> 8) & 0xff;
	ret = dvb_usb_generic_write(state->d, state->data, 3);
	if (ret < 0)
		goto ret;

ret:
	mutex_unlock(&state->data_mutex);
	return ret;
}

static int dtt200u_fe_get_frontend(struct dvb_frontend* fe,
				   struct dtv_frontend_properties *fep)
{
	struct dtt200u_fe_state *state = fe->demodulator_priv;

>>>>>>> v4.9.227
	memcpy(fep, &state->fep, sizeof(struct dtv_frontend_properties));
	return 0;
}

static void dtt200u_fe_release(struct dvb_frontend* fe)
{
	struct dtt200u_fe_state *state = (struct dtt200u_fe_state*) fe->demodulator_priv;
	kfree(state);
}

static struct dvb_frontend_ops dtt200u_fe_ops;

struct dvb_frontend* dtt200u_fe_attach(struct dvb_usb_device *d)
{
	struct dtt200u_fe_state* state = NULL;

	/* allocate memory for the internal state */
	state = kzalloc(sizeof(struct dtt200u_fe_state), GFP_KERNEL);
	if (state == NULL)
		goto error;

	deb_info("attaching frontend dtt200u\n");

	state->d = d;
<<<<<<< HEAD
=======
	mutex_init(&state->data_mutex);
>>>>>>> v4.9.227

	memcpy(&state->frontend.ops,&dtt200u_fe_ops,sizeof(struct dvb_frontend_ops));
	state->frontend.demodulator_priv = state;

	return &state->frontend;
error:
	return NULL;
}

static struct dvb_frontend_ops dtt200u_fe_ops = {
	.delsys = { SYS_DVBT },
	.info = {
		.name			= "WideView USB DVB-T",
		.frequency_min		= 44250000,
		.frequency_max		= 867250000,
		.frequency_stepsize	= 250000,
		.caps = FE_CAN_INVERSION_AUTO |
				FE_CAN_FEC_1_2 | FE_CAN_FEC_2_3 | FE_CAN_FEC_3_4 |
				FE_CAN_FEC_5_6 | FE_CAN_FEC_7_8 | FE_CAN_FEC_AUTO |
				FE_CAN_QPSK | FE_CAN_QAM_16 | FE_CAN_QAM_64 | FE_CAN_QAM_AUTO |
				FE_CAN_TRANSMISSION_MODE_AUTO |
				FE_CAN_GUARD_INTERVAL_AUTO |
				FE_CAN_RECOVER |
				FE_CAN_HIERARCHY_AUTO,
	},

	.release = dtt200u_fe_release,

	.init = dtt200u_fe_init,
	.sleep = dtt200u_fe_sleep,

	.set_frontend = dtt200u_fe_set_frontend,
	.get_frontend = dtt200u_fe_get_frontend,
	.get_tune_settings = dtt200u_fe_get_tune_settings,

	.read_status = dtt200u_fe_read_status,
	.read_ber = dtt200u_fe_read_ber,
	.read_signal_strength = dtt200u_fe_read_signal_strength,
	.read_snr = dtt200u_fe_read_snr,
	.read_ucblocks = dtt200u_fe_read_unc_blocks,
};
