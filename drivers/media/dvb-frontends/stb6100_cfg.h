/*
	STB6100 Silicon Tuner
	Copyright (C) Manu Abraham (abraham.manu@gmail.com)

	Copyright (C) ST Microelectronics

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

<<<<<<< HEAD
=======
#include <linux/dvb/frontend.h>
#include "dvb_frontend.h"

>>>>>>> v4.9.227
static int stb6100_get_frequency(struct dvb_frontend *fe, u32 *frequency)
{
	struct dvb_frontend_ops	*frontend_ops = &fe->ops;
	struct dvb_tuner_ops	*tuner_ops = &frontend_ops->tuner_ops;
<<<<<<< HEAD
	struct tuner_state	t_state;
	int err = 0;

	if (tuner_ops->get_state) {
		err = tuner_ops->get_state(fe, DVBFE_TUNER_FREQUENCY, &t_state);
=======
	int err = 0;

	if (tuner_ops->get_frequency) {
		err = tuner_ops->get_frequency(fe, frequency);
>>>>>>> v4.9.227
		if (err < 0) {
			printk("%s: Invalid parameter\n", __func__);
			return err;
		}
<<<<<<< HEAD
		*frequency = t_state.frequency;
=======
>>>>>>> v4.9.227
	}
	return 0;
}

static int stb6100_set_frequency(struct dvb_frontend *fe, u32 frequency)
{
	struct dvb_frontend_ops	*frontend_ops = &fe->ops;
	struct dvb_tuner_ops	*tuner_ops = &frontend_ops->tuner_ops;
<<<<<<< HEAD
	struct tuner_state	t_state;
	int err = 0;

	t_state.frequency = frequency;

	if (tuner_ops->set_state) {
		err = tuner_ops->set_state(fe, DVBFE_TUNER_FREQUENCY, &t_state);
=======
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	u32 bw = c->bandwidth_hz;
	int err = 0;

	c->frequency = frequency;
	c->bandwidth_hz = 0;		/* Don't adjust the bandwidth */

	if (tuner_ops->set_params) {
		err = tuner_ops->set_params(fe);
		c->bandwidth_hz = bw;
>>>>>>> v4.9.227
		if (err < 0) {
			printk("%s: Invalid parameter\n", __func__);
			return err;
		}
	}
	return 0;
}

static int stb6100_get_bandwidth(struct dvb_frontend *fe, u32 *bandwidth)
{
	struct dvb_frontend_ops	*frontend_ops = &fe->ops;
	struct dvb_tuner_ops	*tuner_ops = &frontend_ops->tuner_ops;
<<<<<<< HEAD
	struct tuner_state	t_state;
	int err = 0;

	if (tuner_ops->get_state) {
		err = tuner_ops->get_state(fe, DVBFE_TUNER_BANDWIDTH, &t_state);
=======
	int err = 0;

	if (tuner_ops->get_bandwidth) {
		err = tuner_ops->get_bandwidth(fe, bandwidth);
>>>>>>> v4.9.227
		if (err < 0) {
			printk("%s: Invalid parameter\n", __func__);
			return err;
		}
<<<<<<< HEAD
		*bandwidth = t_state.bandwidth;
=======
>>>>>>> v4.9.227
	}
	return 0;
}

static int stb6100_set_bandwidth(struct dvb_frontend *fe, u32 bandwidth)
{
	struct dvb_frontend_ops	*frontend_ops = &fe->ops;
	struct dvb_tuner_ops	*tuner_ops = &frontend_ops->tuner_ops;
<<<<<<< HEAD
	struct tuner_state	t_state;
	int err = 0;

	t_state.bandwidth = bandwidth;

	if (tuner_ops->set_state) {
		err = tuner_ops->set_state(fe, DVBFE_TUNER_BANDWIDTH, &t_state);
=======
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	u32 freq = c->frequency;
	int err = 0;

	c->bandwidth_hz = bandwidth;
	c->frequency = 0;		/* Don't adjust the frequency */

	if (tuner_ops->set_params) {
		err = tuner_ops->set_params(fe);
		c->frequency = freq;
>>>>>>> v4.9.227
		if (err < 0) {
			printk("%s: Invalid parameter\n", __func__);
			return err;
		}
	}
	return 0;
}
