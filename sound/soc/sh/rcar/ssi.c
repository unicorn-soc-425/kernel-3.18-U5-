/*
 * Renesas R-Car SSIU/SSI support
 *
 * Copyright (C) 2013 Renesas Solutions Corp.
 * Kuninori Morimoto <kuninori.morimoto.gx@renesas.com>
 *
 * Based on fsi.c
 * Kuninori Morimoto <morimoto.kuninori@renesas.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/delay.h>
#include "rsnd.h"
#define RSND_SSI_NAME_SIZE 16

/*
 * SSICR
 */
#define	FORCE		(1 << 31)	/* Fixed */
#define	DMEN		(1 << 28)	/* DMA Enable */
#define	UIEN		(1 << 27)	/* Underflow Interrupt Enable */
#define	OIEN		(1 << 26)	/* Overflow Interrupt Enable */
#define	IIEN		(1 << 25)	/* Idle Mode Interrupt Enable */
#define	DIEN		(1 << 24)	/* Data Interrupt Enable */
<<<<<<< HEAD

=======
#define	CHNL_4		(1 << 22)	/* Channels */
#define	CHNL_6		(2 << 22)	/* Channels */
#define	CHNL_8		(3 << 22)	/* Channels */
>>>>>>> v4.9.227
#define	DWL_8		(0 << 19)	/* Data Word Length */
#define	DWL_16		(1 << 19)	/* Data Word Length */
#define	DWL_18		(2 << 19)	/* Data Word Length */
#define	DWL_20		(3 << 19)	/* Data Word Length */
#define	DWL_22		(4 << 19)	/* Data Word Length */
#define	DWL_24		(5 << 19)	/* Data Word Length */
#define	DWL_32		(6 << 19)	/* Data Word Length */

#define	SWL_32		(3 << 16)	/* R/W System Word Length */
#define	SCKD		(1 << 15)	/* Serial Bit Clock Direction */
#define	SWSD		(1 << 14)	/* Serial WS Direction */
#define	SCKP		(1 << 13)	/* Serial Bit Clock Polarity */
#define	SWSP		(1 << 12)	/* Serial WS Polarity */
#define	SDTA		(1 << 10)	/* Serial Data Alignment */
<<<<<<< HEAD
=======
#define	PDTA		(1 <<  9)	/* Parallel Data Alignment */
>>>>>>> v4.9.227
#define	DEL		(1 <<  8)	/* Serial Data Delay */
#define	CKDV(v)		(v <<  4)	/* Serial Clock Division Ratio */
#define	TRMD		(1 <<  1)	/* Transmit/Receive Mode Select */
#define	EN		(1 <<  0)	/* SSI Module Enable */

/*
 * SSISR
 */
#define	UIRQ		(1 << 27)	/* Underflow Error Interrupt Status */
#define	OIRQ		(1 << 26)	/* Overflow Error Interrupt Status */
#define	IIRQ		(1 << 25)	/* Idle Mode Interrupt Status */
#define	DIRQ		(1 << 24)	/* Data Interrupt Status Flag */

/*
 * SSIWSR
 */
#define CONT		(1 << 8)	/* WS Continue Function */
<<<<<<< HEAD
=======
#define WS_MODE		(1 << 0)	/* WS Mode */
>>>>>>> v4.9.227

#define SSI_NAME "ssi"

struct rsnd_ssi {
<<<<<<< HEAD
	struct clk *clk;
	struct rsnd_ssi_platform_info *info; /* rcar_snd.h */
	struct rsnd_ssi *parent;
	struct rsnd_mod mod;

	struct rsnd_dai *rdai;
	u32 cr_own;
	u32 cr_clk;
	u32 cr_etc;
	int err;
	unsigned int usrcnt;
	unsigned int rate;
};

=======
	struct rsnd_mod mod;
	struct rsnd_mod *dma;

	u32 flags;
	u32 cr_own;
	u32 cr_clk;
	u32 cr_mode;
	u32 wsr;
	int chan;
	int rate;
	int irq;
	unsigned int usrcnt;
};

/* flags */
#define RSND_SSI_CLK_PIN_SHARE		(1 << 0)
#define RSND_SSI_NO_BUSIF		(1 << 1) /* SSI+DMA without BUSIF */

>>>>>>> v4.9.227
#define for_each_rsnd_ssi(pos, priv, i)					\
	for (i = 0;							\
	     (i < rsnd_ssi_nr(priv)) &&					\
		((pos) = ((struct rsnd_ssi *)(priv)->ssi + i));		\
	     i++)

<<<<<<< HEAD
#define rsnd_ssi_nr(priv) ((priv)->ssi_nr)
#define rsnd_mod_to_ssi(_mod) container_of((_mod), struct rsnd_ssi, mod)
#define rsnd_dma_to_ssi(dma)  rsnd_mod_to_ssi(rsnd_dma_to_mod(dma))
#define rsnd_ssi_pio_available(ssi) ((ssi)->info->pio_irq > 0)
#define rsnd_ssi_dma_available(ssi) \
	rsnd_dma_available(rsnd_mod_to_dma(&(ssi)->mod))
#define rsnd_ssi_clk_from_parent(ssi) ((ssi)->parent)
#define rsnd_ssi_mode_flags(p) ((p)->info->flags)
#define rsnd_ssi_dai_id(ssi) ((ssi)->info->dai_id)

static int rsnd_ssi_use_busif(struct rsnd_mod *mod)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);
	int use_busif = 0;

=======
#define rsnd_ssi_get(priv, id) ((struct rsnd_ssi *)(priv->ssi) + id)
#define rsnd_ssi_to_dma(mod) ((ssi)->dma)
#define rsnd_ssi_nr(priv) ((priv)->ssi_nr)
#define rsnd_mod_to_ssi(_mod) container_of((_mod), struct rsnd_ssi, mod)
#define rsnd_ssi_mode_flags(p) ((p)->flags)
#define rsnd_ssi_is_parent(ssi, io) ((ssi) == rsnd_io_to_mod_ssip(io))
#define rsnd_ssi_is_multi_slave(mod, io) \
	(rsnd_ssi_multi_slaves(io) & (1 << rsnd_mod_id(mod)))
#define rsnd_ssi_is_run_mods(mod, io) \
	(rsnd_ssi_run_mods(io) & (1 << rsnd_mod_id(mod)))

int rsnd_ssi_use_busif(struct rsnd_dai_stream *io)
{
	struct rsnd_mod *mod = rsnd_io_to_mod_ssi(io);
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	int use_busif = 0;

	if (!rsnd_ssi_is_dma_mode(mod))
		return 0;

>>>>>>> v4.9.227
	if (!(rsnd_ssi_mode_flags(ssi) & RSND_SSI_NO_BUSIF))
		use_busif = 1;
	if (rsnd_io_to_mod_src(io))
		use_busif = 1;

	return use_busif;
}

<<<<<<< HEAD
=======
static void rsnd_ssi_status_clear(struct rsnd_mod *mod)
{
	rsnd_mod_write(mod, SSISR, 0);
}

static u32 rsnd_ssi_status_get(struct rsnd_mod *mod)
{
	return rsnd_mod_read(mod, SSISR);
}

>>>>>>> v4.9.227
static void rsnd_ssi_status_check(struct rsnd_mod *mod,
				  u32 bit)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct device *dev = rsnd_priv_to_dev(priv);
	u32 status;
	int i;

	for (i = 0; i < 1024; i++) {
<<<<<<< HEAD
		status = rsnd_mod_read(mod, SSISR);
=======
		status = rsnd_ssi_status_get(mod);
>>>>>>> v4.9.227
		if (status & bit)
			return;

		udelay(50);
	}

<<<<<<< HEAD
	dev_warn(dev, "status check failed\n");
}

static int rsnd_ssi_master_clk_start(struct rsnd_ssi *ssi,
				     struct rsnd_dai_stream *io)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(&ssi->mod);
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	struct device *dev = rsnd_priv_to_dev(priv);
	int i, j, ret;
	int adg_clk_div_table[] = {
		1, 6, /* see adg.c */
	};
=======
	dev_warn(dev, "%s[%d] status check failed\n",
		 rsnd_mod_name(mod), rsnd_mod_id(mod));
}

static u32 rsnd_ssi_multi_slaves(struct rsnd_dai_stream *io)
{
	struct rsnd_mod *mod;
	enum rsnd_mod_type types[] = {
		RSND_MOD_SSIM1,
		RSND_MOD_SSIM2,
		RSND_MOD_SSIM3,
	};
	int i, mask;

	mask = 0;
	for (i = 0; i < ARRAY_SIZE(types); i++) {
		mod = rsnd_io_to_mod(io, types[i]);
		if (!mod)
			continue;

		mask |= 1 << rsnd_mod_id(mod);
	}

	return mask;
}

static u32 rsnd_ssi_run_mods(struct rsnd_dai_stream *io)
{
	struct rsnd_mod *ssi_mod = rsnd_io_to_mod_ssi(io);
	struct rsnd_mod *ssi_parent_mod = rsnd_io_to_mod_ssip(io);
	u32 mods;

	mods = rsnd_ssi_multi_slaves_runtime(io) |
		1 << rsnd_mod_id(ssi_mod);

	if (ssi_parent_mod)
		mods |= 1 << rsnd_mod_id(ssi_parent_mod);

	return mods;
}

u32 rsnd_ssi_multi_slaves_runtime(struct rsnd_dai_stream *io)
{
	if (rsnd_runtime_is_ssi_multi(io))
		return rsnd_ssi_multi_slaves(io);

	return 0;
}

static int rsnd_ssi_master_clk_start(struct rsnd_mod *mod,
				     struct rsnd_dai_stream *io)
{
	struct rsnd_priv *priv = rsnd_io_to_priv(io);
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_dai *rdai = rsnd_io_to_rdai(io);
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	struct rsnd_mod *ssi_parent_mod = rsnd_io_to_mod_ssip(io);
	int chan = rsnd_runtime_channel_for_ssi(io);
	int j, ret;
>>>>>>> v4.9.227
	int ssi_clk_mul_table[] = {
		1, 2, 4, 8, 16, 6, 12,
	};
	unsigned int main_rate;
<<<<<<< HEAD
	unsigned int rate = rsnd_src_get_ssi_rate(priv, io, runtime);
=======
	unsigned int rate = rsnd_io_is_play(io) ?
		rsnd_src_get_out_rate(priv, io) :
		rsnd_src_get_in_rate(priv, io);

	if (!rsnd_rdai_is_clk_master(rdai))
		return 0;

	if (ssi_parent_mod && !rsnd_ssi_is_parent(mod, io))
		return 0;

	if (rsnd_ssi_is_multi_slave(mod, io))
		return 0;

	if (ssi->usrcnt > 1) {
		if (ssi->rate != rate) {
			dev_err(dev, "SSI parent/child should use same rate\n");
			return -EINVAL;
		}

		return 0;
	}
>>>>>>> v4.9.227

	/*
	 * Find best clock, and try to start ADG
	 */
<<<<<<< HEAD
	for (i = 0; i < ARRAY_SIZE(adg_clk_div_table); i++) {
		for (j = 0; j < ARRAY_SIZE(ssi_clk_mul_table); j++) {

			/*
			 * this driver is assuming that
			 * system word is 64fs (= 2 x 32bit)
			 * see rsnd_ssi_init()
			 */
			main_rate = rate / adg_clk_div_table[i]
				* 32 * 2 * ssi_clk_mul_table[j];

			ret = rsnd_adg_ssi_clk_try_start(&ssi->mod, main_rate);
			if (0 == ret) {
				ssi->rate	= rate;
				ssi->cr_clk	= FORCE | SWL_32 |
						  SCKD | SWSD | CKDV(j);

				dev_dbg(dev, "ssi%d outputs %u Hz\n",
					rsnd_mod_id(&ssi->mod), rate);

				return 0;
			}
=======
	for (j = 0; j < ARRAY_SIZE(ssi_clk_mul_table); j++) {

		/*
		 * It will set SSIWSR.CONT here, but SSICR.CKDV = 000
		 * with it is not allowed. (SSIWSR.WS_MODE with
		 * SSICR.CKDV = 000 is not allowed either).
		 * Skip it. See SSICR.CKDV
		 */
		if (j == 0)
			continue;

		/*
		 * this driver is assuming that
		 * system word is 32bit x chan
		 * see rsnd_ssi_init()
		 */
		main_rate = rate * 32 * chan * ssi_clk_mul_table[j];

		ret = rsnd_adg_ssi_clk_try_start(mod, main_rate);
		if (0 == ret) {
			ssi->cr_clk	= FORCE | SWL_32 |
				SCKD | SWSD | CKDV(j);
			ssi->wsr = CONT;

			ssi->rate = rate;

			dev_dbg(dev, "%s[%d] outputs %u Hz\n",
				rsnd_mod_name(mod),
				rsnd_mod_id(mod), rate);

			return 0;
>>>>>>> v4.9.227
		}
	}

	dev_err(dev, "unsupported clock rate\n");
	return -EIO;
}

<<<<<<< HEAD
static void rsnd_ssi_master_clk_stop(struct rsnd_ssi *ssi)
{
	ssi->rate = 0;
	ssi->cr_clk = 0;
	rsnd_adg_ssi_clk_stop(&ssi->mod);
}

static void rsnd_ssi_hw_start(struct rsnd_ssi *ssi,
			      struct rsnd_dai *rdai,
			      struct rsnd_dai_stream *io)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(&ssi->mod);
	struct device *dev = rsnd_priv_to_dev(priv);
	u32 cr;

	if (0 == ssi->usrcnt) {
		clk_prepare_enable(ssi->clk);

		if (rsnd_dai_is_clk_master(rdai)) {
			if (rsnd_ssi_clk_from_parent(ssi))
				rsnd_ssi_hw_start(ssi->parent, rdai, io);
			else
				rsnd_ssi_master_clk_start(ssi, io);
		}
	}

	cr  =	ssi->cr_own	|
		ssi->cr_clk	|
		ssi->cr_etc	|
		EN;

	rsnd_mod_write(&ssi->mod, SSICR, cr);

	ssi->usrcnt++;

	dev_dbg(dev, "ssi%d hw started\n", rsnd_mod_id(&ssi->mod));
}

static void rsnd_ssi_hw_stop(struct rsnd_ssi *ssi,
			     struct rsnd_dai *rdai)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(&ssi->mod);
	struct device *dev = rsnd_priv_to_dev(priv);
	u32 cr;

	if (0 == ssi->usrcnt) /* stop might be called without start */
		return;

	ssi->usrcnt--;

	if (0 == ssi->usrcnt) {
		/*
		 * disable all IRQ,
		 * and, wait all data was sent
		 */
		cr  =	ssi->cr_own	|
			ssi->cr_clk;

		rsnd_mod_write(&ssi->mod, SSICR, cr | EN);
		rsnd_ssi_status_check(&ssi->mod, DIRQ);

		/*
		 * disable SSI,
		 * and, wait idle state
		 */
		rsnd_mod_write(&ssi->mod, SSICR, cr);	/* disabled all */
		rsnd_ssi_status_check(&ssi->mod, IIRQ);

		if (rsnd_dai_is_clk_master(rdai)) {
			if (rsnd_ssi_clk_from_parent(ssi))
				rsnd_ssi_hw_stop(ssi->parent, rdai);
			else
				rsnd_ssi_master_clk_stop(ssi);
		}

		clk_disable_unprepare(ssi->clk);
	}

	dev_dbg(dev, "ssi%d hw stopped\n", rsnd_mod_id(&ssi->mod));
=======
static void rsnd_ssi_master_clk_stop(struct rsnd_mod *mod,
				     struct rsnd_dai_stream *io)
{
	struct rsnd_dai *rdai = rsnd_io_to_rdai(io);
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	struct rsnd_mod *ssi_parent_mod = rsnd_io_to_mod_ssip(io);

	if (!rsnd_rdai_is_clk_master(rdai))
		return;

	if (ssi_parent_mod && !rsnd_ssi_is_parent(mod, io))
		return;

	if (ssi->usrcnt > 1)
		return;

	ssi->cr_clk	= 0;
	ssi->rate	= 0;

	rsnd_adg_ssi_clk_stop(mod);
}

static void rsnd_ssi_config_init(struct rsnd_mod *mod,
				struct rsnd_dai_stream *io)
{
	struct rsnd_dai *rdai = rsnd_io_to_rdai(io);
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	u32 cr_own;
	u32 cr_mode;
	u32 wsr;
	int is_tdm;

	is_tdm = rsnd_runtime_is_ssi_tdm(io);

	/*
	 * always use 32bit system word.
	 * see also rsnd_ssi_master_clk_enable()
	 */
	cr_own = FORCE | SWL_32 | PDTA;

	if (rdai->bit_clk_inv)
		cr_own |= SCKP;
	if (rdai->frm_clk_inv ^ is_tdm)
		cr_own |= SWSP;
	if (rdai->data_alignment)
		cr_own |= SDTA;
	if (rdai->sys_delay)
		cr_own |= DEL;
	if (rsnd_io_is_play(io))
		cr_own |= TRMD;

	switch (runtime->sample_bits) {
	case 16:
		cr_own |= DWL_16;
		break;
	case 32:
		cr_own |= DWL_24;
		break;
	}

	if (rsnd_ssi_is_dma_mode(mod)) {
		cr_mode = UIEN | OIEN |	/* over/under run */
			  DMEN;		/* DMA : enable DMA */
	} else {
		cr_mode = DIEN;		/* PIO : enable Data interrupt */
	}

	/*
	 * TDM Extend Mode
	 * see
	 *	rsnd_ssiu_init_gen2()
	 */
	wsr = ssi->wsr;
	if (is_tdm) {
		wsr	|= WS_MODE;
		cr_own	|= CHNL_8;
	}

	ssi->cr_own	= cr_own;
	ssi->cr_mode	= cr_mode;
	ssi->wsr	= wsr;
}

static void rsnd_ssi_register_setup(struct rsnd_mod *mod)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);

	rsnd_mod_write(mod, SSIWSR,	ssi->wsr);
	rsnd_mod_write(mod, SSICR,	ssi->cr_own	|
					ssi->cr_clk	|
					ssi->cr_mode); /* without EN */
>>>>>>> v4.9.227
}

/*
 *	SSI mod common functions
 */
static int rsnd_ssi_init(struct rsnd_mod *mod,
<<<<<<< HEAD
			 struct rsnd_dai *rdai)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	u32 cr;

	cr = FORCE;

	/*
	 * always use 32bit system word for easy clock calculation.
	 * see also rsnd_ssi_master_clk_enable()
	 */
	cr |= SWL_32;

	/*
	 * init clock settings for SSICR
	 */
	switch (runtime->sample_bits) {
	case 16:
		cr |= DWL_16;
		break;
	case 32:
		cr |= DWL_24;
		break;
	default:
		return -EIO;
	}

	if (rdai->bit_clk_inv)
		cr |= SCKP;
	if (rdai->frm_clk_inv)
		cr |= SWSP;
	if (rdai->data_alignment)
		cr |= SDTA;
	if (rdai->sys_delay)
		cr |= DEL;
	if (rsnd_dai_is_play(rdai, io))
		cr |= TRMD;

	/*
	 * set ssi parameter
	 */
	ssi->rdai	= rdai;
	ssi->cr_own	= cr;
	ssi->err	= -1; /* ignore 1st error */
=======
			 struct rsnd_dai_stream *io,
			 struct rsnd_priv *priv)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	int ret;

	if (!rsnd_ssi_is_run_mods(mod, io))
		return 0;

	ssi->usrcnt++;

	rsnd_mod_power_on(mod);

	ret = rsnd_ssi_master_clk_start(mod, io);
	if (ret < 0)
		return ret;

	if (!rsnd_ssi_is_parent(mod, io))
		rsnd_ssi_config_init(mod, io);

	rsnd_ssi_register_setup(mod);

	/* clear error status */
	rsnd_ssi_status_clear(mod);
>>>>>>> v4.9.227

	return 0;
}

static int rsnd_ssi_quit(struct rsnd_mod *mod,
<<<<<<< HEAD
			 struct rsnd_dai *rdai)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct device *dev = rsnd_priv_to_dev(priv);

	if (ssi->err > 0)
		dev_warn(dev, "ssi under/over flow err = %d\n", ssi->err);

	ssi->rdai	= NULL;
	ssi->cr_own	= 0;
	ssi->err	= 0;
=======
			 struct rsnd_dai_stream *io,
			 struct rsnd_priv *priv)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	struct device *dev = rsnd_priv_to_dev(priv);

	if (!rsnd_ssi_is_run_mods(mod, io))
		return 0;

	if (!ssi->usrcnt) {
		dev_err(dev, "%s[%d] usrcnt error\n",
			rsnd_mod_name(mod), rsnd_mod_id(mod));
		return -EIO;
	}

	if (!rsnd_ssi_is_parent(mod, io))
		ssi->cr_own	= 0;

	rsnd_ssi_master_clk_stop(mod, io);

	rsnd_mod_power_off(mod);

	ssi->usrcnt--;
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static void rsnd_ssi_record_error(struct rsnd_ssi *ssi, u32 status)
{
	/* under/over flow error */
	if (status & (UIRQ | OIRQ)) {
		ssi->err++;

		/* clear error status */
		rsnd_mod_write(&ssi->mod, SSISR, 0);
	}
}

/*
 *		SSI PIO
 */
static irqreturn_t rsnd_ssi_pio_interrupt(int irq, void *data)
{
	struct rsnd_ssi *ssi = data;
	struct rsnd_mod *mod = &ssi->mod;
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);
	u32 status = rsnd_mod_read(mod, SSISR);
	irqreturn_t ret = IRQ_NONE;

	if (io && (status & DIRQ)) {
		struct rsnd_dai *rdai = ssi->rdai;
		struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
		u32 *buf = (u32 *)(runtime->dma_area +
				   rsnd_dai_pointer_offset(io, 0));

		rsnd_ssi_record_error(ssi, status);
=======
static int rsnd_ssi_hw_params(struct rsnd_mod *mod,
			      struct rsnd_dai_stream *io,
			      struct snd_pcm_substream *substream,
			      struct snd_pcm_hw_params *params)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	int chan = params_channels(params);

	/*
	 * Already working.
	 * It will happen if SSI has parent/child connection.
	 */
	if (ssi->usrcnt > 1) {
		/*
		 * it is error if child <-> parent SSI uses
		 * different channels.
		 */
		if (ssi->chan != chan)
			return -EIO;
	}

	ssi->chan = chan;

	return 0;
}

static int rsnd_ssi_start(struct rsnd_mod *mod,
			  struct rsnd_dai_stream *io,
			  struct rsnd_priv *priv)
{
	if (!rsnd_ssi_is_run_mods(mod, io))
		return 0;

	/*
	 * EN will be set via SSIU :: SSI_CONTROL
	 * if Multi channel mode
	 */
	if (rsnd_ssi_multi_slaves_runtime(io))
		return 0;

	rsnd_mod_bset(mod, SSICR, EN, EN);

	return 0;
}

static int rsnd_ssi_stop(struct rsnd_mod *mod,
			 struct rsnd_dai_stream *io,
			 struct rsnd_priv *priv)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	u32 cr;

	if (!rsnd_ssi_is_run_mods(mod, io))
		return 0;

	/*
	 * don't stop if not last user
	 * see also
	 *	rsnd_ssi_start
	 *	rsnd_ssi_interrupt
	 */
	if (ssi->usrcnt > 1)
		return 0;

	/*
	 * disable all IRQ,
	 * and, wait all data was sent
	 */
	cr  =	ssi->cr_own	|
		ssi->cr_clk;

	rsnd_mod_write(mod, SSICR, cr | EN);
	rsnd_ssi_status_check(mod, DIRQ);

	/*
	 * disable SSI,
	 * and, wait idle state
	 */
	rsnd_mod_write(mod, SSICR, cr);	/* disabled all */
	rsnd_ssi_status_check(mod, IIRQ);

	return 0;
}

static int rsnd_ssi_irq(struct rsnd_mod *mod,
			struct rsnd_dai_stream *io,
			struct rsnd_priv *priv,
			int enable)
{
	u32 val = 0;

	if (rsnd_is_gen1(priv))
		return 0;

	if (rsnd_ssi_is_parent(mod, io))
		return 0;

	if (!rsnd_ssi_is_run_mods(mod, io))
		return 0;

	if (enable)
		val = rsnd_ssi_is_dma_mode(mod) ? 0x0e000000 : 0x0f000000;

	rsnd_mod_write(mod, SSI_INT_ENABLE, val);

	return 0;
}

static void __rsnd_ssi_interrupt(struct rsnd_mod *mod,
				 struct rsnd_dai_stream *io)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	int is_dma = rsnd_ssi_is_dma_mode(mod);
	u32 status;
	bool elapsed = false;
	bool stop = false;

	spin_lock(&priv->lock);

	/* ignore all cases if not working */
	if (!rsnd_io_is_working(io))
		goto rsnd_ssi_interrupt_out;

	status = rsnd_ssi_status_get(mod);

	/* PIO only */
	if (!is_dma && (status & DIRQ)) {
		struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
		u32 *buf = (u32 *)(runtime->dma_area +
				   rsnd_dai_pointer_offset(io, 0));
		int shift = 0;

		switch (runtime->sample_bits) {
		case 32:
			shift = 8;
			break;
		}
>>>>>>> v4.9.227

		/*
		 * 8/16/32 data can be assesse to TDR/RDR register
		 * directly as 32bit data
		 * see rsnd_ssi_init()
		 */
<<<<<<< HEAD
		if (rsnd_dai_is_play(rdai, io))
			rsnd_mod_write(mod, SSITDR, *buf);
		else
			*buf = rsnd_mod_read(mod, SSIRDR);

		rsnd_dai_pointer_update(io, sizeof(*buf));

		ret = IRQ_HANDLED;
	}

	return ret;
}

static int rsnd_ssi_pio_probe(struct rsnd_mod *mod,
			      struct rsnd_dai *rdai)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	int irq = ssi->info->pio_irq;
	int ret;

	ret = devm_request_irq(dev, irq,
			       rsnd_ssi_pio_interrupt,
			       IRQF_SHARED,
			       dev_name(dev), ssi);
	if (ret)
		dev_err(dev, "SSI request interrupt failed\n");

	dev_dbg(dev, "%s (PIO) is probed\n", rsnd_mod_name(mod));
=======
		if (rsnd_io_is_play(io))
			rsnd_mod_write(mod, SSITDR, (*buf) << shift);
		else
			*buf = (rsnd_mod_read(mod, SSIRDR) >> shift);

		elapsed = rsnd_dai_pointer_update(io, sizeof(*buf));
	}

	/* DMA only */
	if (is_dma && (status & (UIRQ | OIRQ)))
		stop = true;

	rsnd_ssi_status_clear(mod);
rsnd_ssi_interrupt_out:
	spin_unlock(&priv->lock);

	if (elapsed)
		rsnd_dai_period_elapsed(io);

	if (stop)
		snd_pcm_stop_xrun(io->substream);

}

static irqreturn_t rsnd_ssi_interrupt(int irq, void *data)
{
	struct rsnd_mod *mod = data;

	rsnd_mod_interrupt(mod, __rsnd_ssi_interrupt);

	return IRQ_HANDLED;
}

/*
 *		SSI PIO
 */
static void rsnd_ssi_parent_attach(struct rsnd_mod *mod,
				   struct rsnd_dai_stream *io)
{
	struct rsnd_dai *rdai = rsnd_io_to_rdai(io);
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);

	if (!__rsnd_ssi_is_pin_sharing(mod))
		return;

	if (!rsnd_rdai_is_clk_master(rdai))
		return;

	switch (rsnd_mod_id(mod)) {
	case 1:
	case 2:
		rsnd_dai_connect(rsnd_ssi_mod_get(priv, 0), io, RSND_MOD_SSIP);
		break;
	case 4:
		rsnd_dai_connect(rsnd_ssi_mod_get(priv, 3), io, RSND_MOD_SSIP);
		break;
	case 8:
		rsnd_dai_connect(rsnd_ssi_mod_get(priv, 7), io, RSND_MOD_SSIP);
		break;
	}
}

static int rsnd_ssi_pcm_new(struct rsnd_mod *mod,
			    struct rsnd_dai_stream *io,
			    struct snd_soc_pcm_runtime *rtd)
{
	/*
	 * rsnd_rdai_is_clk_master() will be enabled after set_fmt,
	 * and, pcm_new will be called after it.
	 * This function reuse pcm_new at this point.
	 */
	rsnd_ssi_parent_attach(mod, io);

	return 0;
}

static int rsnd_ssi_common_probe(struct rsnd_mod *mod,
				 struct rsnd_dai_stream *io,
				 struct rsnd_priv *priv)
{
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	int ret;

	/*
	 * SSIP/SSIU/IRQ are not needed on
	 * SSI Multi slaves
	 */
	if (rsnd_ssi_is_multi_slave(mod, io))
		return 0;

	/*
	 * It can't judge ssi parent at this point
	 * see rsnd_ssi_pcm_new()
	 */

	ret = rsnd_ssiu_attach(io, mod);
	if (ret < 0)
		return ret;

	ret = devm_request_irq(dev, ssi->irq,
			       rsnd_ssi_interrupt,
			       IRQF_SHARED,
			       dev_name(dev), mod);
>>>>>>> v4.9.227

	return ret;
}

<<<<<<< HEAD
static int rsnd_ssi_pio_start(struct rsnd_mod *mod,
			      struct rsnd_dai *rdai)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);

	/* enable PIO IRQ */
	ssi->cr_etc = UIEN | OIEN | DIEN;

	rsnd_src_ssiu_start(mod, rdai, 0);

	rsnd_src_enable_ssi_irq(mod, rdai);

	rsnd_ssi_hw_start(ssi, rdai, io);

	return 0;
}

static int rsnd_ssi_pio_stop(struct rsnd_mod *mod,
			     struct rsnd_dai *rdai)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);

	ssi->cr_etc = 0;

	rsnd_ssi_hw_stop(ssi, rdai);

	rsnd_src_ssiu_stop(mod, rdai, 0);

	return 0;
}

static struct rsnd_mod_ops rsnd_ssi_pio_ops = {
	.name	= SSI_NAME,
	.probe	= rsnd_ssi_pio_probe,
	.init	= rsnd_ssi_init,
	.quit	= rsnd_ssi_quit,
	.start	= rsnd_ssi_pio_start,
	.stop	= rsnd_ssi_pio_stop,
};

static int rsnd_ssi_dma_probe(struct rsnd_mod *mod,
			  struct rsnd_dai *rdai)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	struct device *dev = rsnd_priv_to_dev(priv);
	int dma_id = ssi->info->dma_id;
	int ret;

	ret = rsnd_dma_init(
		priv, rsnd_mod_to_dma(mod),
		rsnd_info_is_playback(priv, ssi),
		dma_id);

	if (ret < 0)
		dev_err(dev, "SSI DMA failed\n");

	dev_dbg(dev, "%s (DMA) is probed\n", rsnd_mod_name(mod));
=======
static struct rsnd_mod_ops rsnd_ssi_pio_ops = {
	.name	= SSI_NAME,
	.probe	= rsnd_ssi_common_probe,
	.init	= rsnd_ssi_init,
	.quit	= rsnd_ssi_quit,
	.start	= rsnd_ssi_start,
	.stop	= rsnd_ssi_stop,
	.irq	= rsnd_ssi_irq,
	.pcm_new = rsnd_ssi_pcm_new,
	.hw_params = rsnd_ssi_hw_params,
};

static int rsnd_ssi_dma_probe(struct rsnd_mod *mod,
			      struct rsnd_dai_stream *io,
			      struct rsnd_priv *priv)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	int dma_id = 0; /* not needed */
	int ret;

	/*
	 * SSIP/SSIU/IRQ/DMA are not needed on
	 * SSI Multi slaves
	 */
	if (rsnd_ssi_is_multi_slave(mod, io))
		return 0;

	ret = rsnd_ssi_common_probe(mod, io, priv);
	if (ret)
		return ret;

	/* SSI probe might be called many times in MUX multi path */
	ret = rsnd_dma_attach(io, mod, &ssi->dma, dma_id);
>>>>>>> v4.9.227

	return ret;
}

static int rsnd_ssi_dma_remove(struct rsnd_mod *mod,
<<<<<<< HEAD
			       struct rsnd_dai *rdai)
{
	rsnd_dma_quit(rsnd_mod_to_priv(mod), rsnd_mod_to_dma(mod));

	return 0;
}

static int rsnd_ssi_dma_start(struct rsnd_mod *mod,
			      struct rsnd_dai *rdai)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	struct rsnd_dma *dma = rsnd_mod_to_dma(&ssi->mod);
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);

	/* enable DMA transfer */
	ssi->cr_etc = DMEN;

	rsnd_src_ssiu_start(mod, rdai, rsnd_ssi_use_busif(mod));

	rsnd_dma_start(dma);

	rsnd_ssi_hw_start(ssi, ssi->rdai, io);

	/* enable WS continue */
	if (rsnd_dai_is_clk_master(rdai))
		rsnd_mod_write(&ssi->mod, SSIWSR, CONT);
=======
			       struct rsnd_dai_stream *io,
			       struct rsnd_priv *priv)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	struct rsnd_mod *pure_ssi_mod = rsnd_io_to_mod_ssi(io);
	struct device *dev = rsnd_priv_to_dev(priv);
	int irq = ssi->irq;

	/* Do nothing if non SSI (= SSI parent, multi SSI) mod */
	if (pure_ssi_mod != mod)
		return 0;

	/* PIO will request IRQ again */
	devm_free_irq(dev, irq, mod);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int rsnd_ssi_dma_stop(struct rsnd_mod *mod,
			     struct rsnd_dai *rdai)
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);
	struct rsnd_dma *dma = rsnd_mod_to_dma(&ssi->mod);

	ssi->cr_etc = 0;

	rsnd_ssi_record_error(ssi, rsnd_mod_read(mod, SSISR));

	rsnd_ssi_hw_stop(ssi, rdai);

	rsnd_dma_stop(dma);

	rsnd_src_ssiu_stop(mod, rdai, 1);
=======
static int rsnd_ssi_fallback(struct rsnd_mod *mod,
			     struct rsnd_dai_stream *io,
			     struct rsnd_priv *priv)
{
	struct device *dev = rsnd_priv_to_dev(priv);

	/*
	 * fallback to PIO
	 *
	 * SSI .probe might be called again.
	 * see
	 *	rsnd_rdai_continuance_probe()
	 */
	mod->ops = &rsnd_ssi_pio_ops;

	dev_info(dev, "%s[%d] fallback to PIO mode\n",
		 rsnd_mod_name(mod), rsnd_mod_id(mod));
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static char *rsnd_ssi_dma_name(struct rsnd_mod *mod)
{
	return rsnd_ssi_use_busif(mod) ? "ssiu" : SSI_NAME;
=======
static struct dma_chan *rsnd_ssi_dma_req(struct rsnd_dai_stream *io,
					 struct rsnd_mod *mod)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	int is_play = rsnd_io_is_play(io);
	char *name;

	if (rsnd_ssi_use_busif(io))
		name = is_play ? "rxu" : "txu";
	else
		name = is_play ? "rx" : "tx";

	return rsnd_dma_request_channel(rsnd_ssi_of_node(priv),
					mod, name);
>>>>>>> v4.9.227
}

static struct rsnd_mod_ops rsnd_ssi_dma_ops = {
	.name	= SSI_NAME,
<<<<<<< HEAD
	.dma_name = rsnd_ssi_dma_name,
=======
	.dma_req = rsnd_ssi_dma_req,
>>>>>>> v4.9.227
	.probe	= rsnd_ssi_dma_probe,
	.remove	= rsnd_ssi_dma_remove,
	.init	= rsnd_ssi_init,
	.quit	= rsnd_ssi_quit,
<<<<<<< HEAD
	.start	= rsnd_ssi_dma_start,
	.stop	= rsnd_ssi_dma_stop,
};

=======
	.start	= rsnd_ssi_start,
	.stop	= rsnd_ssi_stop,
	.irq	= rsnd_ssi_irq,
	.pcm_new = rsnd_ssi_pcm_new,
	.fallback = rsnd_ssi_fallback,
	.hw_params = rsnd_ssi_hw_params,
};

int rsnd_ssi_is_dma_mode(struct rsnd_mod *mod)
{
	return mod->ops == &rsnd_ssi_dma_ops;
}


>>>>>>> v4.9.227
/*
 *		Non SSI
 */
static struct rsnd_mod_ops rsnd_ssi_non_ops = {
	.name	= SSI_NAME,
};

/*
 *		ssi mod function
 */
<<<<<<< HEAD
=======
static void rsnd_ssi_connect(struct rsnd_mod *mod,
			     struct rsnd_dai_stream *io)
{
	struct rsnd_dai *rdai = rsnd_io_to_rdai(io);
	enum rsnd_mod_type types[] = {
		RSND_MOD_SSI,
		RSND_MOD_SSIM1,
		RSND_MOD_SSIM2,
		RSND_MOD_SSIM3,
	};
	enum rsnd_mod_type type;
	int i;

	/* try SSI -> SSIM1 -> SSIM2 -> SSIM3 */
	for (i = 0; i < ARRAY_SIZE(types); i++) {
		type = types[i];
		if (!rsnd_io_to_mod(io, type)) {
			rsnd_dai_connect(mod, io, type);
			rsnd_set_slot(rdai, 2 * (i + 1), (i + 1));
			return;
		}
	}
}

void rsnd_parse_connect_ssi(struct rsnd_dai *rdai,
			    struct device_node *playback,
			    struct device_node *capture)
{
	struct rsnd_priv *priv = rsnd_rdai_to_priv(rdai);
	struct device_node *node;
	struct device_node *np;
	struct rsnd_mod *mod;
	int i;

	node = rsnd_ssi_of_node(priv);
	if (!node)
		return;

	i = 0;
	for_each_child_of_node(node, np) {
		mod = rsnd_ssi_mod_get(priv, i);
		if (np == playback)
			rsnd_ssi_connect(mod, &rdai->playback);
		if (np == capture)
			rsnd_ssi_connect(mod, &rdai->capture);
		i++;
	}

	of_node_put(node);
}

>>>>>>> v4.9.227
struct rsnd_mod *rsnd_ssi_mod_get(struct rsnd_priv *priv, int id)
{
	if (WARN_ON(id < 0 || id >= rsnd_ssi_nr(priv)))
		id = 0;

<<<<<<< HEAD
	return &((struct rsnd_ssi *)(priv->ssi) + id)->mod;
}

int rsnd_ssi_is_pin_sharing(struct rsnd_mod *mod)
=======
	return rsnd_mod_get(rsnd_ssi_get(priv, id));
}

int __rsnd_ssi_is_pin_sharing(struct rsnd_mod *mod)
>>>>>>> v4.9.227
{
	struct rsnd_ssi *ssi = rsnd_mod_to_ssi(mod);

	return !!(rsnd_ssi_mode_flags(ssi) & RSND_SSI_CLK_PIN_SHARE);
}

<<<<<<< HEAD
static void rsnd_ssi_parent_clk_setup(struct rsnd_priv *priv, struct rsnd_ssi *ssi)
{
	if (!rsnd_ssi_is_pin_sharing(&ssi->mod))
		return;

	switch (rsnd_mod_id(&ssi->mod)) {
	case 1:
	case 2:
		ssi->parent = rsnd_mod_to_ssi(rsnd_ssi_mod_get(priv, 0));
		break;
	case 4:
		ssi->parent = rsnd_mod_to_ssi(rsnd_ssi_mod_get(priv, 3));
		break;
	case 8:
		ssi->parent = rsnd_mod_to_ssi(rsnd_ssi_mod_get(priv, 7));
		break;
	}
}


static void rsnd_of_parse_ssi(struct platform_device *pdev,
			      const struct rsnd_of_data *of_data,
			      struct rsnd_priv *priv)
{
	struct device_node *node;
	struct device_node *np;
	struct rsnd_ssi_platform_info *ssi_info;
	struct rcar_snd_info *info = rsnd_priv_to_info(priv);
	struct device *dev = &pdev->dev;
	int nr, i;

	if (!of_data)
		return;

	node = of_get_child_by_name(dev->of_node, "rcar_sound,ssi");
	if (!node)
		return;

	nr = of_get_child_count(node);
	if (!nr)
		goto rsnd_of_parse_ssi_end;

	ssi_info = devm_kzalloc(dev,
				sizeof(struct rsnd_ssi_platform_info) * nr,
				GFP_KERNEL);
	if (!ssi_info) {
		dev_err(dev, "ssi info allocation error\n");
		goto rsnd_of_parse_ssi_end;
	}

	info->ssi_info		= ssi_info;
	info->ssi_info_nr	= nr;

	i = -1;
	for_each_child_of_node(node, np) {
		i++;

		ssi_info = info->ssi_info + i;

		/*
		 * pin settings
		 */
		if (of_get_property(np, "shared-pin", NULL))
			ssi_info->flags |= RSND_SSI_CLK_PIN_SHARE;

		/*
		 * irq
		 */
		ssi_info->pio_irq = irq_of_parse_and_map(np, 0);

		/*
		 * DMA
		 */
		ssi_info->dma_id = of_get_property(np, "pio-transfer", NULL) ?
			0 : 1;

		if (of_get_property(np, "no-busif", NULL))
			ssi_info->flags |= RSND_SSI_NO_BUSIF;
	}

rsnd_of_parse_ssi_end:
	of_node_put(node);
}

int rsnd_ssi_probe(struct platform_device *pdev,
		   const struct rsnd_of_data *of_data,
		   struct rsnd_priv *priv)
{
	struct rcar_snd_info *info = rsnd_priv_to_info(priv);
	struct rsnd_ssi_platform_info *pinfo;
=======
static u32 *rsnd_ssi_get_status(struct rsnd_dai_stream *io,
				struct rsnd_mod *mod,
				enum rsnd_mod_type type)
{
	/*
	 * SSIP (= SSI parent) needs to be special, otherwise,
	 * 2nd SSI might doesn't start. see also rsnd_mod_call()
	 *
	 * We can't include parent SSI status on SSI, because we don't know
	 * how many SSI requests parent SSI. Thus, it is localed on "io" now.
	 * ex) trouble case
	 *	Playback: SSI0
	 *	Capture : SSI1 (needs SSI0)
	 *
	 * 1) start Capture  ->	SSI0/SSI1 are started.
	 * 2) start Playback ->	SSI0 doesn't work, because it is already
	 *			marked as "started" on 1)
	 *
	 * OTOH, using each mod's status is good for MUX case.
	 * It doesn't need to start in 2nd start
	 * ex)
	 *	IO-0: SRC0 -> CTU1 -+-> MUX -> DVC -> SSIU -> SSI0
	 *			    |
	 *	IO-1: SRC1 -> CTU2 -+
	 *
	 * 1) start IO-0 ->	start SSI0
	 * 2) start IO-1 ->	SSI0 doesn't need to start, because it is
	 *			already started on 1)
	 */
	if (type == RSND_MOD_SSIP)
		return &io->parent_ssi_status;

	return rsnd_mod_get_status(io, mod, type);
}

int rsnd_ssi_probe(struct rsnd_priv *priv)
{
	struct device_node *node;
	struct device_node *np;
>>>>>>> v4.9.227
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_mod_ops *ops;
	struct clk *clk;
	struct rsnd_ssi *ssi;
	char name[RSND_SSI_NAME_SIZE];
<<<<<<< HEAD
	int i, nr;

	rsnd_of_parse_ssi(pdev, of_data, priv);

	/*
	 *	init SSI
	 */
	nr	= info->ssi_info_nr;
	ssi	= devm_kzalloc(dev, sizeof(*ssi) * nr, GFP_KERNEL);
	if (!ssi) {
		dev_err(dev, "SSI allocate failed\n");
		return -ENOMEM;
=======
	int i, nr, ret;

	node = rsnd_ssi_of_node(priv);
	if (!node)
		return -EINVAL;

	nr = of_get_child_count(node);
	if (!nr) {
		ret = -EINVAL;
		goto rsnd_ssi_probe_done;
	}

	ssi	= devm_kzalloc(dev, sizeof(*ssi) * nr, GFP_KERNEL);
	if (!ssi) {
		ret = -ENOMEM;
		goto rsnd_ssi_probe_done;
>>>>>>> v4.9.227
	}

	priv->ssi	= ssi;
	priv->ssi_nr	= nr;

<<<<<<< HEAD
	for_each_rsnd_ssi(ssi, priv, i) {
		pinfo = &info->ssi_info[i];
=======
	i = 0;
	for_each_child_of_node(node, np) {
		ssi = rsnd_ssi_get(priv, i);
>>>>>>> v4.9.227

		snprintf(name, RSND_SSI_NAME_SIZE, "%s.%d",
			 SSI_NAME, i);

		clk = devm_clk_get(dev, name);
<<<<<<< HEAD
		if (IS_ERR(clk))
			return PTR_ERR(clk);

		ssi->info	= pinfo;
		ssi->clk	= clk;

		ops = &rsnd_ssi_non_ops;
		if (pinfo->dma_id > 0)
			ops = &rsnd_ssi_dma_ops;
		else if (rsnd_ssi_pio_available(ssi))
			ops = &rsnd_ssi_pio_ops;

		rsnd_mod_init(priv, &ssi->mod, ops, RSND_MOD_SSI, i);

		rsnd_ssi_parent_clk_setup(priv, ssi);
	}

	return 0;
=======
		if (IS_ERR(clk)) {
			ret = PTR_ERR(clk);
			goto rsnd_ssi_probe_done;
		}

		if (of_get_property(np, "shared-pin", NULL))
			ssi->flags |= RSND_SSI_CLK_PIN_SHARE;

		if (of_get_property(np, "no-busif", NULL))
			ssi->flags |= RSND_SSI_NO_BUSIF;

		ssi->irq = irq_of_parse_and_map(np, 0);
		if (!ssi->irq) {
			ret = -EINVAL;
			goto rsnd_ssi_probe_done;
		}

		ops = &rsnd_ssi_non_ops;
		if (of_property_read_bool(np, "pio-transfer"))
			ops = &rsnd_ssi_pio_ops;
		else
			ops = &rsnd_ssi_dma_ops;

		ret = rsnd_mod_init(priv, rsnd_mod_get(ssi), ops, clk,
				    rsnd_ssi_get_status, RSND_MOD_SSI, i);
		if (ret)
			goto rsnd_ssi_probe_done;

		i++;
	}

	ret = 0;

rsnd_ssi_probe_done:
	of_node_put(node);

	return ret;
}

void rsnd_ssi_remove(struct rsnd_priv *priv)
{
	struct rsnd_ssi *ssi;
	int i;

	for_each_rsnd_ssi(ssi, priv, i) {
		rsnd_mod_quit(rsnd_mod_get(ssi));
	}
>>>>>>> v4.9.227
}
