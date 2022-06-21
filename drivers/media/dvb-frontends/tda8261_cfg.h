/*
	TDA8261 8PSK/QPSK tuner driver
	Copyright (C) Manu Abraham (abraham.manu@gmail.com)

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

static int tda8261_get_frequency(struct dvb_frontend *fe, u32 *frequency)
{
	struct dvb_frontend_ops	*frontend_ops = &fe->ops;
	struct dvb_tuner_ops	*tuner_ops = &frontend_ops->tuner_ops;
<<<<<<< HEAD
	struct tuner_state	t_state;
	int err = 0;

	if (tuner_ops->get_state) {
		err = tuner_ops->get_state(fe, DVBFE_TUNER_FREQUENCY, &t_state);
		if (err < 0) {
			printk("%s: Invalid parameter\n", __func__);
			return err;
		}
		*frequency = t_state.frequency;
		printk("%s: Frequency=%d\n", __func__, t_state.frequency);
=======
	int err = 0;

	if (tuner_ops->get_frequency) {
		err = tuner_ops->get_frequency(fe, frequency);
		if (err < 0) {
			pr_err("%s: Invalid parameter\n", __func__);
			return err;
		}
		pr_debug("%s: Frequency=%d\n", __func__, *frequency);
>>>>>>> v4.9.227
	}
	return 0;
}

static int tda8261_set_frequency(struct dvb_frontend *fe, u32 frequency)
{
	struct dvb_frontend_ops	*frontend_ops = &fe->ops;
	struct dvb_tuner_ops	*tuner_ops = &frontend_ops->tuner_ops;
<<<<<<< HEAD
	struct tuner_state	t_state;
	int err = 0;

	t_state.frequency = frequency;

	if (tuner_ops->set_state) {
		err = tuner_ops->set_state(fe, DVBFE_TUNER_FREQUENCY, &t_state);
		if (err < 0) {
			printk("%s: Invalid parameter\n", __func__);
			return err;
		}
	}
	printk("%s: Frequency=%d\n", __func__, t_state.frequency);
=======
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int err = 0;

	if (tuner_ops->set_params) {
		err = tuner_ops->set_params(fe);
		if (err < 0) {
			pr_err("%s: Invalid parameter\n", __func__);
			return err;
		}
	}
	pr_debug("%s: Frequency=%d\n", __func__, c->frequency);
>>>>>>> v4.9.227
	return 0;
}

static int tda8261_get_bandwidth(struct dvb_frontend *fe, u32 *bandwidth)
{
<<<<<<< HEAD
	struct dvb_frontend_ops	*frontend_ops = &fe->ops;
	struct dvb_tuner_ops	*tuner_ops = &frontend_ops->tuner_ops;
	struct tuner_state	t_state;
	int err = 0;

	if (tuner_ops->get_state) {
		err = tuner_ops->get_state(fe, DVBFE_TUNER_BANDWIDTH, &t_state);
		if (err < 0) {
			printk("%s: Invalid parameter\n", __func__);
			return err;
		}
		*bandwidth = t_state.bandwidth;
		printk("%s: Bandwidth=%d\n", __func__, t_state.bandwidth);
	}
=======
	/* FIXME! need to calculate Bandwidth */
	*bandwidth = 40000000;

>>>>>>> v4.9.227
	return 0;
}
