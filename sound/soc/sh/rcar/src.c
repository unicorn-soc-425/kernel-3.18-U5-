/*
 * Renesas R-Car SRC support
 *
 * Copyright (C) 2013 Renesas Solutions Corp.
 * Kuninori Morimoto <kuninori.morimoto.gx@renesas.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "rsnd.h"

#define SRC_NAME "src"

<<<<<<< HEAD
struct rsnd_src {
	struct rsnd_src_platform_info *info; /* rcar_snd.h */
	struct rsnd_mod mod;
	struct clk *clk;
=======
/* SRCx_STATUS */
#define OUF_SRCO	((1 << 12) | (1 << 13))
#define OUF_SRCI	((1 <<  9) | (1 <<  8))

/* SCU_SYSTEM_STATUS0/1 */
#define OUF_SRC(id)	((1 << (id + 16)) | (1 << id))

struct rsnd_src {
	struct rsnd_mod mod;
	struct rsnd_mod *dma;
	struct rsnd_kctrl_cfg_s sen;  /* sync convert enable */
	struct rsnd_kctrl_cfg_s sync; /* sync convert */
	u32 convert_rate; /* sampling rate convert */
	int irq;
>>>>>>> v4.9.227
};

#define RSND_SRC_NAME_SIZE 16

<<<<<<< HEAD
#define rsnd_src_convert_rate(p) ((p)->info->convert_rate)
#define rsnd_mod_to_src(_mod)				\
	container_of((_mod), struct rsnd_src, mod)
#define rsnd_src_dma_available(src) \
	rsnd_dma_available(rsnd_mod_to_dma(&(src)->mod))
=======
#define rsnd_src_get(priv, id) ((struct rsnd_src *)(priv->src) + id)
#define rsnd_src_to_dma(src) ((src)->dma)
#define rsnd_src_nr(priv) ((priv)->src_nr)
#define rsnd_src_sync_is_enabled(mod) (rsnd_mod_to_src(mod)->sen.val)

#define rsnd_mod_to_src(_mod)				\
	container_of((_mod), struct rsnd_src, mod)
>>>>>>> v4.9.227

#define for_each_rsnd_src(pos, priv, i)				\
	for ((i) = 0;						\
	     ((i) < rsnd_src_nr(priv)) &&			\
	     ((pos) = (struct rsnd_src *)(priv)->src + i);	\
	     i++)


/*
 *		image of SRC (Sampling Rate Converter)
 *
 * 96kHz   <-> +-----+	48kHz	+-----+	 48kHz	+-------+
 * 48kHz   <-> | SRC | <------>	| SSI |	<----->	| codec |
 * 44.1kHz <-> +-----+		+-----+		+-------+
 * ...
 *
 */

/*
 * src.c is caring...
 *
 * Gen1
 *
 * [mem] -> [SRU] -> [SSI]
 *        |--------|
 *
 * Gen2
 *
 * [mem] -> [SRC] -> [SSIU] -> [SSI]
 *        |-----------------|
 */

<<<<<<< HEAD
/*
 *	How to use SRC bypass mode for debugging
 *
 * SRC has bypass mode, and it is useful for debugging.
 * In Gen2 case,
 * SRCm_MODE controls whether SRC is used or not
 * SSI_MODE0 controls whether SSIU which receives SRC data
 * is used or not.
 * Both SRCm_MODE/SSI_MODE0 settings are needed if you use SRC,
 * but SRC bypass mode needs SSI_MODE0 only.
 *
 * This driver request
 * struct rsnd_src_platform_info {
 *	u32 convert_rate;
 *	int dma_id;
 * }
 *
 * rsnd_src_convert_rate() indicates
 * above convert_rate, and it controls
 * whether SRC is used or not.
 *
 * ex) doesn't use SRC
 * static struct rsnd_dai_platform_info rsnd_dai = {
 *	.playback = { .ssi = &rsnd_ssi[0], },
 * };
 *
 * ex) uses SRC
 * static struct rsnd_src_platform_info rsnd_src[] = {
 *	RSND_SCU(48000, 0),
 *	...
 * };
 * static struct rsnd_dai_platform_info rsnd_dai = {
 *	.playback = { .ssi = &rsnd_ssi[0], .src = &rsnd_src[0] },
 * };
 *
 * ex) uses SRC bypass mode
 * static struct rsnd_src_platform_info rsnd_src[] = {
 *	RSND_SCU(0, 0),
 *	...
 * };
 * static struct rsnd_dai_platform_info rsnd_dai = {
 *	.playback = { .ssi = &rsnd_ssi[0], .src = &rsnd_src[0] },
 * };
 *
 */

/*
 *		Gen1/Gen2 common functions
 */
int rsnd_src_ssiu_start(struct rsnd_mod *ssi_mod,
			struct rsnd_dai *rdai,
			int use_busif)
{
	struct rsnd_dai_stream *io = rsnd_mod_to_io(ssi_mod);
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	int ssi_id = rsnd_mod_id(ssi_mod);

	/*
	 * SSI_MODE0
	 */
	rsnd_mod_bset(ssi_mod, SSI_MODE0, (1 << ssi_id),
		      !use_busif << ssi_id);

	/*
	 * SSI_MODE1
	 */
	if (rsnd_ssi_is_pin_sharing(ssi_mod)) {
		int shift = -1;
		switch (ssi_id) {
		case 1:
			shift = 0;
			break;
		case 2:
			shift = 2;
			break;
		case 4:
			shift = 16;
			break;
		}

		if (shift >= 0)
			rsnd_mod_bset(ssi_mod, SSI_MODE1,
				      0x3 << shift,
				      rsnd_dai_is_clk_master(rdai) ?
				      0x2 << shift : 0x1 << shift);
	}

	/*
	 * DMA settings for SSIU
	 */
	if (use_busif) {
		u32 val = 0x76543210;
		u32 mask = ~0;

		rsnd_mod_write(ssi_mod, SSI_BUSIF_ADINR,
			       rsnd_get_adinr(ssi_mod));
		rsnd_mod_write(ssi_mod, SSI_BUSIF_MODE,  1);
		rsnd_mod_write(ssi_mod, SSI_CTRL, 0x1);

		mask <<= runtime->channels * 4;
		val = val & mask;

		switch (runtime->sample_bits) {
		case 16:
			val |= 0x67452301 & ~mask;
			break;
		case 32:
			val |= 0x76543210 & ~mask;
			break;
		}
		rsnd_mod_write(ssi_mod, BUSIF_DALIGN, val);

	}

	return 0;
}

int rsnd_src_ssiu_stop(struct rsnd_mod *ssi_mod,
			struct rsnd_dai *rdai,
			int use_busif)
{
	/*
	 * DMA settings for SSIU
	 */
	if (use_busif)
		rsnd_mod_write(ssi_mod, SSI_CTRL, 0);

	return 0;
}

int rsnd_src_enable_ssi_irq(struct rsnd_mod *ssi_mod,
			    struct rsnd_dai *rdai)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(ssi_mod);

	/* enable PIO interrupt if Gen2 */
	if (rsnd_is_gen2(priv))
		rsnd_mod_write(ssi_mod, INT_ENABLE, 0x0f000000);

	return 0;
}

unsigned int rsnd_src_get_ssi_rate(struct rsnd_priv *priv,
				   struct rsnd_dai_stream *io,
				   struct snd_pcm_runtime *runtime)
{
	struct rsnd_mod *src_mod = rsnd_io_to_mod_src(io);
	struct rsnd_src *src;
	unsigned int rate = 0;

	if (src_mod) {
		src = rsnd_mod_to_src(src_mod);

		/*
		 * return convert rate if SRC is used,
		 * otherwise, return runtime->rate as usual
		 */
		rate = rsnd_src_convert_rate(src);
	}
=======
static void rsnd_src_activation(struct rsnd_mod *mod)
{
	rsnd_mod_write(mod, SRC_SWRSR, 0);
	rsnd_mod_write(mod, SRC_SWRSR, 1);
}

static void rsnd_src_halt(struct rsnd_mod *mod)
{
	rsnd_mod_write(mod, SRC_SRCIR, 1);
	rsnd_mod_write(mod, SRC_SWRSR, 0);
}

static struct dma_chan *rsnd_src_dma_req(struct rsnd_dai_stream *io,
					 struct rsnd_mod *mod)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	int is_play = rsnd_io_is_play(io);

	return rsnd_dma_request_channel(rsnd_src_of_node(priv),
					mod,
					is_play ? "rx" : "tx");
}

static u32 rsnd_src_convert_rate(struct rsnd_dai_stream *io,
				 struct rsnd_mod *mod)
{
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	struct rsnd_src *src = rsnd_mod_to_src(mod);
	u32 convert_rate;

	if (!runtime)
		return 0;

	if (!rsnd_src_sync_is_enabled(mod))
		return src->convert_rate;

	convert_rate = src->sync.val;

	if (!convert_rate)
		convert_rate = src->convert_rate;

	if (!convert_rate)
		convert_rate = runtime->rate;

	return convert_rate;
}

unsigned int rsnd_src_get_rate(struct rsnd_priv *priv,
			       struct rsnd_dai_stream *io,
			       int is_in)
{
	struct rsnd_mod *src_mod = rsnd_io_to_mod_src(io);
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	unsigned int rate = 0;
	int is_play = rsnd_io_is_play(io);

	/*
	 *
	 * Playback
	 * runtime_rate -> [SRC] -> convert_rate
	 *
	 * Capture
	 * convert_rate -> [SRC] -> runtime_rate
	 */

	if (is_play == is_in)
		return runtime->rate;

	/*
	 * return convert rate if SRC is used,
	 * otherwise, return runtime->rate as usual
	 */
	if (src_mod)
		rate = rsnd_src_convert_rate(io, src_mod);
>>>>>>> v4.9.227

	if (!rate)
		rate = runtime->rate;

	return rate;
}

<<<<<<< HEAD
static int rsnd_src_set_convert_rate(struct rsnd_mod *mod,
				     struct rsnd_dai *rdai)
{
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	struct rsnd_src *src = rsnd_mod_to_src(mod);
	u32 convert_rate = rsnd_src_convert_rate(src);
	u32 fsrate = 0;

	if (convert_rate)
		fsrate = 0x0400000 / convert_rate * runtime->rate;

	/* set/clear soft reset */
	rsnd_mod_write(mod, SRC_SWRSR, 0);
	rsnd_mod_write(mod, SRC_SWRSR, 1);

	/*
	 * Initialize the operation of the SRC internal circuits
	 * see rsnd_src_start()
	 */
	rsnd_mod_write(mod, SRC_SRCIR, 1);

	/* Set channel number and output bit length */
	rsnd_mod_write(mod, SRC_ADINR, rsnd_get_adinr(mod));

	/* Enable the initial value of IFS */
	if (fsrate) {
		rsnd_mod_write(mod, SRC_IFSCR, 1);

		/* Set initial value of IFS */
		rsnd_mod_write(mod, SRC_IFSVR, fsrate);
	}

	/* use DMA transfer */
	rsnd_mod_write(mod, SRC_BUSIF_MODE, 1);

	return 0;
}

static int rsnd_src_init(struct rsnd_mod *mod,
			 struct rsnd_dai *rdai)
{
	struct rsnd_src *src = rsnd_mod_to_src(mod);

	clk_prepare_enable(src->clk);

	return 0;
}

static int rsnd_src_quit(struct rsnd_mod *mod,
			 struct rsnd_dai *rdai)
{
	struct rsnd_src *src = rsnd_mod_to_src(mod);

	clk_disable_unprepare(src->clk);

	return 0;
}

static int rsnd_src_start(struct rsnd_mod *mod,
			  struct rsnd_dai *rdai)
{
	struct rsnd_src *src = rsnd_mod_to_src(mod);

	/*
	 * Cancel the initialization and operate the SRC function
	 * see rsnd_src_set_convert_rate()
	 */
	rsnd_mod_write(mod, SRC_SRCIR, 0);

	if (rsnd_src_convert_rate(src))
		rsnd_mod_write(mod, SRC_ROUTE_MODE0, 1);

	return 0;
}


static int rsnd_src_stop(struct rsnd_mod *mod,
			 struct rsnd_dai *rdai)
{
	struct rsnd_src *src = rsnd_mod_to_src(mod);

	if (rsnd_src_convert_rate(src))
		rsnd_mod_write(mod, SRC_ROUTE_MODE0, 0);

	return 0;
}

/*
 *		Gen1 functions
 */
static int rsnd_src_set_route_gen1(struct rsnd_mod *mod,
				   struct rsnd_dai *rdai)
{
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);
	struct src_route_config {
		u32 mask;
		int shift;
	} routes[] = {
		{ 0xF,  0, }, /* 0 */
		{ 0xF,  4, }, /* 1 */
		{ 0xF,  8, }, /* 2 */
		{ 0x7, 12, }, /* 3 */
		{ 0x7, 16, }, /* 4 */
		{ 0x7, 20, }, /* 5 */
		{ 0x7, 24, }, /* 6 */
		{ 0x3, 28, }, /* 7 */
		{ 0x3, 30, }, /* 8 */
	};
	u32 mask;
	u32 val;
	int id;

	id = rsnd_mod_id(mod);
	if (id < 0 || id >= ARRAY_SIZE(routes))
		return -EIO;

	/*
	 * SRC_ROUTE_SELECT
	 */
	val = rsnd_dai_is_play(rdai, io) ? 0x1 : 0x2;
	val = val		<< routes[id].shift;
	mask = routes[id].mask	<< routes[id].shift;

	rsnd_mod_bset(mod, SRC_ROUTE_SEL, mask, val);

	return 0;
}

static int rsnd_src_set_convert_timing_gen1(struct rsnd_mod *mod,
					    struct rsnd_dai *rdai)
{
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct rsnd_src *src = rsnd_mod_to_src(mod);
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	u32 convert_rate = rsnd_src_convert_rate(src);
	u32 mask;
	u32 val;
	int shift;
	int id = rsnd_mod_id(mod);
	int ret;

	/*
	 * SRC_TIMING_SELECT
	 */
	shift	= (id % 4) * 8;
	mask	= 0x1F << shift;

	/*
	 * ADG is used as source clock if SRC was used,
	 * then, SSI WS is used as destination clock.
	 * SSI WS is used as source clock if SRC is not used
	 * (when playback, source/destination become reverse when capture)
	 */
	ret = 0;
	if (convert_rate) {
		/* use ADG */
		val = 0;
		ret = rsnd_adg_set_convert_clk_gen1(priv, mod,
						    runtime->rate,
						    convert_rate);
	} else if (8 == id) {
		/* use SSI WS, but SRU8 is special */
		val = id << shift;
	} else {
		/* use SSI WS */
		val = (id + 1) << shift;
	}

	if (ret < 0)
		return ret;

	switch (id / 4) {
	case 0:
		rsnd_mod_bset(mod, SRC_TMG_SEL0, mask, val);
		break;
	case 1:
		rsnd_mod_bset(mod, SRC_TMG_SEL1, mask, val);
		break;
	case 2:
		rsnd_mod_bset(mod, SRC_TMG_SEL2, mask, val);
		break;
=======
static int rsnd_src_hw_params(struct rsnd_mod *mod,
			      struct rsnd_dai_stream *io,
			      struct snd_pcm_substream *substream,
			      struct snd_pcm_hw_params *fe_params)
{
	struct rsnd_src *src = rsnd_mod_to_src(mod);
	struct snd_soc_pcm_runtime *fe = substream->private_data;

	/*
	 * SRC assumes that it is used under DPCM if user want to use
	 * sampling rate convert. Then, SRC should be FE.
	 * And then, this function will be called *after* BE settings.
	 * this means, each BE already has fixuped hw_params.
	 * see
	 *	dpcm_fe_dai_hw_params()
	 *	dpcm_be_dai_hw_params()
	 */
	if (fe->dai_link->dynamic) {
		int stream = substream->stream;
		struct snd_soc_dpcm *dpcm;
		struct snd_pcm_hw_params *be_params;

		list_for_each_entry(dpcm, &fe->dpcm[stream].be_clients, list_be) {
			be_params = &dpcm->hw_params;

			if (params_rate(fe_params) != params_rate(be_params))
				src->convert_rate = params_rate(be_params);
		}
>>>>>>> v4.9.227
	}

	return 0;
}

<<<<<<< HEAD
static int rsnd_src_set_convert_rate_gen1(struct rsnd_mod *mod,
					  struct rsnd_dai *rdai)
{
	int ret;

	ret = rsnd_src_set_convert_rate(mod, rdai);
	if (ret < 0)
		return ret;

	/* Select SRC mode (fixed value) */
	rsnd_mod_write(mod, SRC_SRCCR, 0x00010110);

	/* Set the restriction value of the FS ratio (98%) */
	rsnd_mod_write(mod, SRC_MNFSR,
		       rsnd_mod_read(mod, SRC_IFSVR) / 100 * 98);

	/* no SRC_BFSSR settings, since SRC_SRCCR::BUFMD is 0 */

	return 0;
}

static int rsnd_src_probe_gen1(struct rsnd_mod *mod,
			      struct rsnd_dai *rdai)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct device *dev = rsnd_priv_to_dev(priv);

	dev_dbg(dev, "%s (Gen1) is probed\n", rsnd_mod_name(mod));

	return 0;
}

static int rsnd_src_init_gen1(struct rsnd_mod *mod,
			      struct rsnd_dai *rdai)
{
	int ret;

	ret = rsnd_src_init(mod, rdai);
	if (ret < 0)
		return ret;

	ret = rsnd_src_set_route_gen1(mod, rdai);
	if (ret < 0)
		return ret;

	ret = rsnd_src_set_convert_rate_gen1(mod, rdai);
	if (ret < 0)
		return ret;

	ret = rsnd_src_set_convert_timing_gen1(mod, rdai);
	if (ret < 0)
		return ret;

	return 0;
}

static int rsnd_src_start_gen1(struct rsnd_mod *mod,
			       struct rsnd_dai *rdai)
{
	int id = rsnd_mod_id(mod);

	rsnd_mod_bset(mod, SRC_ROUTE_CTRL, (1 << id), (1 << id));

	return rsnd_src_start(mod, rdai);
}

static int rsnd_src_stop_gen1(struct rsnd_mod *mod,
			      struct rsnd_dai *rdai)
{
	int id = rsnd_mod_id(mod);

	rsnd_mod_bset(mod, SRC_ROUTE_CTRL, (1 << id), 0);

	return rsnd_src_stop(mod, rdai);
}

static struct rsnd_mod_ops rsnd_src_gen1_ops = {
	.name	= SRC_NAME,
	.probe	= rsnd_src_probe_gen1,
	.init	= rsnd_src_init_gen1,
	.quit	= rsnd_src_quit,
	.start	= rsnd_src_start_gen1,
	.stop	= rsnd_src_stop_gen1,
};

/*
 *		Gen2 functions
 */
static int rsnd_src_set_convert_rate_gen2(struct rsnd_mod *mod,
					  struct rsnd_dai *rdai)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	struct rsnd_src *src = rsnd_mod_to_src(mod);
	uint ratio;
	int ret;

	/* 6 - 1/6 are very enough ratio for SRC_BSDSR */
	if (!rsnd_src_convert_rate(src))
		ratio = 0;
	else if (rsnd_src_convert_rate(src) > runtime->rate)
		ratio = 100 * rsnd_src_convert_rate(src) / runtime->rate;
	else
		ratio = 100 * runtime->rate / rsnd_src_convert_rate(src);

	if (ratio > 600) {
		dev_err(dev, "FSO/FSI ratio error\n");
		return -EINVAL;
	}

	ret = rsnd_src_set_convert_rate(mod, rdai);
	if (ret < 0)
		return ret;

	rsnd_mod_write(mod, SRC_SRCCR, 0x00011110);

=======
static void rsnd_src_set_convert_rate(struct rsnd_dai_stream *io,
				      struct rsnd_mod *mod)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct device *dev = rsnd_priv_to_dev(priv);
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	u32 fin, fout;
	u32 ifscr, fsrate, adinr;
	u32 cr, route;
	u32 bsdsr, bsisr;
	uint ratio;

	if (!runtime)
		return;

	fin  = rsnd_src_get_in_rate(priv, io);
	fout = rsnd_src_get_out_rate(priv, io);

	/* 6 - 1/6 are very enough ratio for SRC_BSDSR */
	if (fin == fout)
		ratio = 0;
	else if (fin > fout)
		ratio = 100 * fin / fout;
	else
		ratio = 100 * fout / fin;

	if (ratio > 600) {
		dev_err(dev, "FSO/FSI ratio error\n");
		return;
	}

	/*
	 *	SRC_ADINR
	 */
	adinr = rsnd_get_adinr_bit(mod, io) |
		rsnd_runtime_channel_original(io);

	/*
	 *	SRC_IFSCR / SRC_IFSVR
	 */
	ifscr = 0;
	fsrate = 0;
	if (fin != fout) {
		u64 n;

		ifscr = 1;
		n = (u64)0x0400000 * fin;
		do_div(n, fout);
		fsrate = n;
	}

	/*
	 *	SRC_SRCCR / SRC_ROUTE_MODE0
	 */
	cr	= 0x00011110;
	route	= 0x0;
	if (fin != fout) {
		route	= 0x1;

		if (rsnd_src_sync_is_enabled(mod)) {
			cr |= 0x1;
			route |= rsnd_io_is_play(io) ?
				(0x1 << 24) : (0x1 << 25);
		}
	}

	/*
	 * SRC_BSDSR / SRC_BSISR
	 */
>>>>>>> v4.9.227
	switch (rsnd_mod_id(mod)) {
	case 5:
	case 6:
	case 7:
	case 8:
<<<<<<< HEAD
		rsnd_mod_write(mod, SRC_BSDSR, 0x02400000);
		break;
	default:
		rsnd_mod_write(mod, SRC_BSDSR, 0x01800000);
		break;
	}

	rsnd_mod_write(mod, SRC_BSISR, 0x00100060);
=======
		bsdsr = 0x02400000; /* 6 - 1/6 */
		bsisr = 0x00100060; /* 6 - 1/6 */
		break;
	default:
		bsdsr = 0x01800000; /* 6 - 1/6 */
		bsisr = 0x00100060 ;/* 6 - 1/6 */
		break;
	}

	rsnd_mod_write(mod, SRC_ROUTE_MODE0, route);

	rsnd_mod_write(mod, SRC_SRCIR, 1);	/* initialize */
	rsnd_mod_write(mod, SRC_ADINR, adinr);
	rsnd_mod_write(mod, SRC_IFSCR, ifscr);
	rsnd_mod_write(mod, SRC_IFSVR, fsrate);
	rsnd_mod_write(mod, SRC_SRCCR, cr);
	rsnd_mod_write(mod, SRC_BSDSR, bsdsr);
	rsnd_mod_write(mod, SRC_BSISR, bsisr);
	rsnd_mod_write(mod, SRC_SRCIR, 0);	/* cancel initialize */

	rsnd_mod_write(mod, SRC_I_BUSIF_MODE, 1);
	rsnd_mod_write(mod, SRC_O_BUSIF_MODE, 1);
	rsnd_mod_write(mod, SRC_BUSIF_DALIGN, rsnd_get_dalign(mod, io));

	rsnd_adg_set_src_timesel_gen2(mod, io, fin, fout);
}

static int rsnd_src_irq(struct rsnd_mod *mod,
			struct rsnd_dai_stream *io,
			struct rsnd_priv *priv,
			int enable)
{
	struct rsnd_src *src = rsnd_mod_to_src(mod);
	u32 sys_int_val, int_val, sys_int_mask;
	int irq = src->irq;
	int id = rsnd_mod_id(mod);

	sys_int_val =
	sys_int_mask = OUF_SRC(id);
	int_val = 0x3300;

	/*
	 * IRQ is not supported on non-DT
	 * see
	 *	rsnd_src_probe_()
	 */
	if ((irq <= 0) || !enable) {
		sys_int_val = 0;
		int_val = 0;
	}

	/*
	 * WORKAROUND
	 *
	 * ignore over flow error when rsnd_src_sync_is_enabled()
	 */
	if (rsnd_src_sync_is_enabled(mod))
		sys_int_val = sys_int_val & 0xffff;

	rsnd_mod_write(mod, SRC_INT_ENABLE0, int_val);
	rsnd_mod_bset(mod, SCU_SYS_INT_EN0, sys_int_mask, sys_int_val);
	rsnd_mod_bset(mod, SCU_SYS_INT_EN1, sys_int_mask, sys_int_val);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int rsnd_src_set_convert_timing_gen2(struct rsnd_mod *mod,
					    struct rsnd_dai *rdai)
{
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	struct rsnd_src *src = rsnd_mod_to_src(mod);
	u32 convert_rate = rsnd_src_convert_rate(src);
	int ret;

	if (convert_rate)
		ret = rsnd_adg_set_convert_clk_gen2(mod, rdai, io,
						    runtime->rate,
						    convert_rate);
	else
		ret = rsnd_adg_set_convert_timing_gen2(mod, rdai, io);
=======
static void rsnd_src_status_clear(struct rsnd_mod *mod)
{
	u32 val = OUF_SRC(rsnd_mod_id(mod));

	rsnd_mod_bset(mod, SCU_SYS_STATUS0, val, val);
	rsnd_mod_bset(mod, SCU_SYS_STATUS1, val, val);
}

static bool rsnd_src_error_occurred(struct rsnd_mod *mod)
{
	u32 val0, val1;
	bool ret = false;

	val0 = val1 = OUF_SRC(rsnd_mod_id(mod));

	/*
	 * WORKAROUND
	 *
	 * ignore over flow error when rsnd_src_sync_is_enabled()
	 */
	if (rsnd_src_sync_is_enabled(mod))
		val0 = val0 & 0xffff;

	if ((rsnd_mod_read(mod, SCU_SYS_STATUS0) & val0) ||
	    (rsnd_mod_read(mod, SCU_SYS_STATUS1) & val1))
		ret = true;
>>>>>>> v4.9.227

	return ret;
}

<<<<<<< HEAD
static int rsnd_src_probe_gen2(struct rsnd_mod *mod,
			       struct rsnd_dai *rdai)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct rsnd_src *src = rsnd_mod_to_src(mod);
	struct device *dev = rsnd_priv_to_dev(priv);
	int ret;

	ret = rsnd_dma_init(priv,
			    rsnd_mod_to_dma(mod),
			    rsnd_info_is_playback(priv, src),
			    src->info->dma_id);
	if (ret < 0)
		dev_err(dev, "SRC DMA failed\n");

	dev_dbg(dev, "%s (Gen2) is probed\n", rsnd_mod_name(mod));

	return ret;
}

static int rsnd_src_remove_gen2(struct rsnd_mod *mod,
				struct rsnd_dai *rdai)
{
	rsnd_dma_quit(rsnd_mod_to_priv(mod), rsnd_mod_to_dma(mod));

	return 0;
}

static int rsnd_src_init_gen2(struct rsnd_mod *mod,
			      struct rsnd_dai *rdai)
{
	int ret;

	ret = rsnd_src_init(mod, rdai);
	if (ret < 0)
		return ret;

	ret = rsnd_src_set_convert_rate_gen2(mod, rdai);
	if (ret < 0)
		return ret;

	ret = rsnd_src_set_convert_timing_gen2(mod, rdai);
	if (ret < 0)
		return ret;

	return 0;
}

static int rsnd_src_start_gen2(struct rsnd_mod *mod,
			       struct rsnd_dai *rdai)
{
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);
	struct rsnd_src *src = rsnd_mod_to_src(mod);
	u32 val = rsnd_io_to_mod_dvc(io) ? 0x01 : 0x11;

	rsnd_dma_start(rsnd_mod_to_dma(&src->mod));

	rsnd_mod_write(mod, SRC_CTRL, val);

	return rsnd_src_start(mod, rdai);
}

static int rsnd_src_stop_gen2(struct rsnd_mod *mod,
			      struct rsnd_dai *rdai)
{
	struct rsnd_src *src = rsnd_mod_to_src(mod);

	rsnd_mod_write(mod, SRC_CTRL, 0);

	rsnd_dma_stop(rsnd_mod_to_dma(&src->mod));

	return rsnd_src_stop(mod, rdai);
}

static struct rsnd_mod_ops rsnd_src_gen2_ops = {
	.name	= SRC_NAME,
	.probe	= rsnd_src_probe_gen2,
	.remove	= rsnd_src_remove_gen2,
	.init	= rsnd_src_init_gen2,
	.quit	= rsnd_src_quit,
	.start	= rsnd_src_start_gen2,
	.stop	= rsnd_src_stop_gen2,
=======
static int rsnd_src_start(struct rsnd_mod *mod,
			  struct rsnd_dai_stream *io,
			  struct rsnd_priv *priv)
{
	u32 val;

	/*
	 * WORKAROUND
	 *
	 * Enable SRC output if you want to use sync convert together with DVC
	 */
	val = (rsnd_io_to_mod_dvc(io) && !rsnd_src_sync_is_enabled(mod)) ?
		0x01 : 0x11;

	rsnd_mod_write(mod, SRC_CTRL, val);

	return 0;
}

static int rsnd_src_stop(struct rsnd_mod *mod,
			 struct rsnd_dai_stream *io,
			 struct rsnd_priv *priv)
{
	rsnd_mod_write(mod, SRC_CTRL, 0);

	return 0;
}

static int rsnd_src_init(struct rsnd_mod *mod,
			 struct rsnd_dai_stream *io,
			 struct rsnd_priv *priv)
{
	struct rsnd_src *src = rsnd_mod_to_src(mod);

	rsnd_mod_power_on(mod);

	rsnd_src_activation(mod);

	rsnd_src_set_convert_rate(io, mod);

	rsnd_src_status_clear(mod);

	/* reset sync convert_rate */
	src->sync.val = 0;

	return 0;
}

static int rsnd_src_quit(struct rsnd_mod *mod,
			 struct rsnd_dai_stream *io,
			 struct rsnd_priv *priv)
{
	struct rsnd_src *src = rsnd_mod_to_src(mod);

	rsnd_src_halt(mod);

	rsnd_mod_power_off(mod);

	src->convert_rate = 0;

	/* reset sync convert_rate */
	src->sync.val = 0;

	return 0;
}

static void __rsnd_src_interrupt(struct rsnd_mod *mod,
				 struct rsnd_dai_stream *io)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	bool stop = false;

	spin_lock(&priv->lock);

	/* ignore all cases if not working */
	if (!rsnd_io_is_working(io))
		goto rsnd_src_interrupt_out;

	if (rsnd_src_error_occurred(mod))
		stop = true;

	rsnd_src_status_clear(mod);
rsnd_src_interrupt_out:

	spin_unlock(&priv->lock);

	if (stop)
		snd_pcm_stop_xrun(io->substream);
}

static irqreturn_t rsnd_src_interrupt(int irq, void *data)
{
	struct rsnd_mod *mod = data;

	rsnd_mod_interrupt(mod, __rsnd_src_interrupt);

	return IRQ_HANDLED;
}

static int rsnd_src_probe_(struct rsnd_mod *mod,
			   struct rsnd_dai_stream *io,
			   struct rsnd_priv *priv)
{
	struct rsnd_src *src = rsnd_mod_to_src(mod);
	struct device *dev = rsnd_priv_to_dev(priv);
	int irq = src->irq;
	int ret;

	if (irq > 0) {
		/*
		 * IRQ is not supported on non-DT
		 * see
		 *	rsnd_src_irq()
		 */
		ret = devm_request_irq(dev, irq,
				       rsnd_src_interrupt,
				       IRQF_SHARED,
				       dev_name(dev), mod);
		if (ret)
			return ret;
	}

	ret = rsnd_dma_attach(io, mod, &src->dma, 0);

	return ret;
}

static int rsnd_src_pcm_new(struct rsnd_mod *mod,
			    struct rsnd_dai_stream *io,
			    struct snd_soc_pcm_runtime *rtd)
{
	struct rsnd_src *src = rsnd_mod_to_src(mod);
	int ret;

	/*
	 * enable SRC sync convert if possible
	 */

	/*
	 * It can't use SRC Synchronous convert
	 * when Capture if it uses CMD
	 */
	if (rsnd_io_to_mod_cmd(io) && !rsnd_io_is_play(io))
		return 0;

	/*
	 * enable sync convert
	 */
	ret = rsnd_kctrl_new_s(mod, io, rtd,
			       rsnd_io_is_play(io) ?
			       "SRC Out Rate Switch" :
			       "SRC In Rate Switch",
			       rsnd_src_set_convert_rate,
			       &src->sen, 1);
	if (ret < 0)
		return ret;

	ret = rsnd_kctrl_new_s(mod, io, rtd,
			       rsnd_io_is_play(io) ?
			       "SRC Out Rate" :
			       "SRC In Rate",
			       rsnd_src_set_convert_rate,
			       &src->sync, 192000);

	return ret;
}

static struct rsnd_mod_ops rsnd_src_ops = {
	.name	= SRC_NAME,
	.dma_req = rsnd_src_dma_req,
	.probe	= rsnd_src_probe_,
	.init	= rsnd_src_init,
	.quit	= rsnd_src_quit,
	.start	= rsnd_src_start,
	.stop	= rsnd_src_stop,
	.irq	= rsnd_src_irq,
	.hw_params = rsnd_src_hw_params,
	.pcm_new = rsnd_src_pcm_new,
>>>>>>> v4.9.227
};

struct rsnd_mod *rsnd_src_mod_get(struct rsnd_priv *priv, int id)
{
	if (WARN_ON(id < 0 || id >= rsnd_src_nr(priv)))
		id = 0;

<<<<<<< HEAD
	return &((struct rsnd_src *)(priv->src) + id)->mod;
}

static void rsnd_of_parse_src(struct platform_device *pdev,
			      const struct rsnd_of_data *of_data,
			      struct rsnd_priv *priv)
{
	struct device_node *src_node;
	struct rcar_snd_info *info = rsnd_priv_to_info(priv);
	struct rsnd_src_platform_info *src_info;
	struct device *dev = &pdev->dev;
	int nr;

	if (!of_data)
		return;

	src_node = of_get_child_by_name(dev->of_node, "rcar_sound,src");
	if (!src_node)
		return;

	nr = of_get_child_count(src_node);
	if (!nr)
		goto rsnd_of_parse_src_end;

	src_info = devm_kzalloc(dev,
				sizeof(struct rsnd_src_platform_info) * nr,
				GFP_KERNEL);
	if (!src_info) {
		dev_err(dev, "src info allocation error\n");
		goto rsnd_of_parse_src_end;
	}

	info->src_info		= src_info;
	info->src_info_nr	= nr;

rsnd_of_parse_src_end:
	of_node_put(src_node);
}

int rsnd_src_probe(struct platform_device *pdev,
		   const struct rsnd_of_data *of_data,
		   struct rsnd_priv *priv)
{
	struct rcar_snd_info *info = rsnd_priv_to_info(priv);
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_src *src;
	struct rsnd_mod_ops *ops;
	struct clk *clk;
	char name[RSND_SRC_NAME_SIZE];
	int i, nr;

	ops = NULL;
	if (rsnd_is_gen1(priv))
		ops = &rsnd_src_gen1_ops;
	if (rsnd_is_gen2(priv))
		ops = &rsnd_src_gen2_ops;
	if (!ops) {
		dev_err(dev, "unknown Generation\n");
		return -EIO;
	}

	rsnd_of_parse_src(pdev, of_data, priv);

	/*
	 * init SRC
	 */
	nr	= info->src_info_nr;
	if (!nr)
		return 0;

	src	= devm_kzalloc(dev, sizeof(*src) * nr, GFP_KERNEL);
	if (!src) {
		dev_err(dev, "SRC allocate failed\n");
		return -ENOMEM;
=======
	return rsnd_mod_get(rsnd_src_get(priv, id));
}

int rsnd_src_probe(struct rsnd_priv *priv)
{
	struct device_node *node;
	struct device_node *np;
	struct device *dev = rsnd_priv_to_dev(priv);
	struct rsnd_src *src;
	struct clk *clk;
	char name[RSND_SRC_NAME_SIZE];
	int i, nr, ret;

	/* This driver doesn't support Gen1 at this point */
	if (rsnd_is_gen1(priv))
		return 0;

	node = rsnd_src_of_node(priv);
	if (!node)
		return 0; /* not used is not error */

	nr = of_get_child_count(node);
	if (!nr) {
		ret = -EINVAL;
		goto rsnd_src_probe_done;
	}

	src	= devm_kzalloc(dev, sizeof(*src) * nr, GFP_KERNEL);
	if (!src) {
		ret = -ENOMEM;
		goto rsnd_src_probe_done;
>>>>>>> v4.9.227
	}

	priv->src_nr	= nr;
	priv->src	= src;

<<<<<<< HEAD
	for_each_rsnd_src(src, priv, i) {
		snprintf(name, RSND_SRC_NAME_SIZE, "%s.%d",
			 SRC_NAME, i);

		clk = devm_clk_get(dev, name);
		if (IS_ERR(clk))
			return PTR_ERR(clk);

		src->info = &info->src_info[i];
		src->clk = clk;

		rsnd_mod_init(priv, &src->mod, ops, RSND_MOD_SRC, i);

		dev_dbg(dev, "SRC%d probed\n", i);
	}

	return 0;
=======
	i = 0;
	for_each_child_of_node(node, np) {
		if (!of_device_is_available(np))
			goto skip;

		src = rsnd_src_get(priv, i);

		snprintf(name, RSND_SRC_NAME_SIZE, "%s.%d",
			 SRC_NAME, i);

		src->irq = irq_of_parse_and_map(np, 0);
		if (!src->irq) {
			ret = -EINVAL;
			goto rsnd_src_probe_done;
		}

		clk = devm_clk_get(dev, name);
		if (IS_ERR(clk)) {
			ret = PTR_ERR(clk);
			goto rsnd_src_probe_done;
		}

		ret = rsnd_mod_init(priv, rsnd_mod_get(src),
				    &rsnd_src_ops, clk, rsnd_mod_get_status,
				    RSND_MOD_SRC, i);
		if (ret)
			goto rsnd_src_probe_done;

skip:
		i++;
	}

	ret = 0;

rsnd_src_probe_done:
	of_node_put(node);

	return ret;
}

void rsnd_src_remove(struct rsnd_priv *priv)
{
	struct rsnd_src *src;
	int i;

	for_each_rsnd_src(src, priv, i) {
		rsnd_mod_quit(rsnd_mod_get(src));
	}
>>>>>>> v4.9.227
}
