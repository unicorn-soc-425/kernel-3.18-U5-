/*
 * Renesas R-Car SRU/SCU/SSIU/SSI support
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

/*
 * Renesas R-Car sound device structure
 *
 * Gen1
 *
 * SRU		: Sound Routing Unit
 *  - SRC	: Sampling Rate Converter
 *  - CMD
 *    - CTU	: Channel Count Conversion Unit
 *    - MIX	: Mixer
 *    - DVC	: Digital Volume and Mute Function
 *  - SSI	: Serial Sound Interface
 *
 * Gen2
 *
 * SCU		: Sampling Rate Converter Unit
 *  - SRC	: Sampling Rate Converter
 *  - CMD
 *   - CTU	: Channel Count Conversion Unit
 *   - MIX	: Mixer
 *   - DVC	: Digital Volume and Mute Function
 * SSIU		: Serial Sound Interface Unit
 *  - SSI	: Serial Sound Interface
 */

/*
 *	driver data Image
 *
 * rsnd_priv
 *   |
 *   | ** this depends on Gen1/Gen2
 *   |
 *   +- gen
 *   |
 *   | ** these depend on data path
 *   | ** gen and platform data control it
 *   |
 *   +- rdai[0]
 *   |   |		 sru     ssiu      ssi
 *   |   +- playback -> [mod] -> [mod] -> [mod] -> ...
 *   |   |
 *   |   |		 sru     ssiu      ssi
 *   |   +- capture  -> [mod] -> [mod] -> [mod] -> ...
 *   |
 *   +- rdai[1]
 *   |   |		 sru     ssiu      ssi
 *   |   +- playback -> [mod] -> [mod] -> [mod] -> ...
 *   |   |
 *   |   |		 sru     ssiu      ssi
 *   |   +- capture  -> [mod] -> [mod] -> [mod] -> ...
 *   ...
 *   |
 *   | ** these control ssi
 *   |
 *   +- ssi
 *   |  |
 *   |  +- ssi[0]
 *   |  +- ssi[1]
 *   |  +- ssi[2]
 *   |  ...
 *   |
 *   | ** these control src
 *   |
 *   +- src
 *      |
 *      +- src[0]
 *      +- src[1]
 *      +- src[2]
 *      ...
 *
 *
 * for_each_rsnd_dai(xx, priv, xx)
 *  rdai[0] => rdai[1] => rdai[2] => ...
 *
 * for_each_rsnd_mod(xx, rdai, xx)
 *  [mod] => [mod] => [mod] => ...
 *
 * rsnd_dai_call(xxx, fn )
 *  [mod]->fn() -> [mod]->fn() -> [mod]->fn()...
 *
 */
#include <linux/pm_runtime.h>
<<<<<<< HEAD
#include <linux/shdma-base.h>
=======
>>>>>>> v4.9.227
#include "rsnd.h"

#define RSND_RATES SNDRV_PCM_RATE_8000_96000
#define RSND_FMTS (SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S16_LE)

<<<<<<< HEAD
static struct rsnd_of_data rsnd_of_data_gen1 = {
	.flags = RSND_GEN1,
};

static struct rsnd_of_data rsnd_of_data_gen2 = {
	.flags = RSND_GEN2,
};

static struct of_device_id rsnd_of_match[] = {
	{ .compatible = "renesas,rcar_sound-gen1", .data = &rsnd_of_data_gen1 },
	{ .compatible = "renesas,rcar_sound-gen2", .data = &rsnd_of_data_gen2 },
=======
static const struct of_device_id rsnd_of_match[] = {
	{ .compatible = "renesas,rcar_sound-gen1", .data = (void *)RSND_GEN1 },
	{ .compatible = "renesas,rcar_sound-gen2", .data = (void *)RSND_GEN2 },
	{ .compatible = "renesas,rcar_sound-gen3", .data = (void *)RSND_GEN2 }, /* gen2 compatible */
>>>>>>> v4.9.227
	{},
};
MODULE_DEVICE_TABLE(of, rsnd_of_match);

/*
<<<<<<< HEAD
 *	rsnd_platform functions
 */
#define rsnd_platform_call(priv, dai, func, param...)	\
	(!(priv->info->func) ? 0 :		\
	 priv->info->func(param))

#define rsnd_is_enable_path(io, name) \
	((io)->info ? (io)->info->name : NULL)
#define rsnd_info_id(priv, io, name) \
	((io)->info->name - priv->info->name##_info)

/*
 *	rsnd_mod functions
 */
=======
 *	rsnd_mod functions
 */
#ifdef DEBUG
void rsnd_mod_make_sure(struct rsnd_mod *mod, enum rsnd_mod_type type)
{
	if (mod->type != type) {
		struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
		struct device *dev = rsnd_priv_to_dev(priv);

		dev_warn(dev, "%s[%d] is not your expected module\n",
			 rsnd_mod_name(mod), rsnd_mod_id(mod));
	}
}
#endif

>>>>>>> v4.9.227
char *rsnd_mod_name(struct rsnd_mod *mod)
{
	if (!mod || !mod->ops)
		return "unknown";

	return mod->ops->name;
}

<<<<<<< HEAD
char *rsnd_mod_dma_name(struct rsnd_mod *mod)
{
	if (!mod || !mod->ops)
		return "unknown";

	if (!mod->ops->dma_name)
		return mod->ops->name;

	return mod->ops->dma_name(mod);
}

void rsnd_mod_init(struct rsnd_priv *priv,
		   struct rsnd_mod *mod,
		   struct rsnd_mod_ops *ops,
		   enum rsnd_mod_type type,
		   int id)
{
	mod->priv	= priv;
	mod->id		= id;
	mod->ops	= ops;
	mod->type	= type;
}

/*
 *	rsnd_dma functions
 */
void rsnd_dma_stop(struct rsnd_dma *dma)
{
	dmaengine_terminate_all(dma->chan);
}

static void rsnd_dma_complete(void *data)
{
	struct rsnd_dma *dma = (struct rsnd_dma *)data;
	struct rsnd_mod *mod = rsnd_dma_to_mod(dma);
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);

	/*
	 * Renesas sound Gen1 needs 1 DMAC,
	 * Gen2 needs 2 DMAC.
	 * In Gen2 case, it are Audio-DMAC, and Audio-DMAC-peri-peri.
	 * But, Audio-DMAC-peri-peri doesn't have interrupt,
	 * and this driver is assuming that here.
	 *
	 * If Audio-DMAC-peri-peri has interrpt,
	 * rsnd_dai_pointer_update() will be called twice,
	 * ant it will breaks io->byte_pos
	 */

	rsnd_dai_pointer_update(io, io->byte_per_period);
}

void rsnd_dma_start(struct rsnd_dma *dma)
{
	struct rsnd_mod *mod = rsnd_dma_to_mod(dma);
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);
	struct snd_pcm_substream *substream = io->substream;
	struct device *dev = rsnd_priv_to_dev(priv);
	struct dma_async_tx_descriptor *desc;

	desc = dmaengine_prep_dma_cyclic(dma->chan,
					 (dma->addr) ? dma->addr :
					 substream->runtime->dma_addr,
					 snd_pcm_lib_buffer_bytes(substream),
					 snd_pcm_lib_period_bytes(substream),
					 dma->dir,
					 DMA_PREP_INTERRUPT | DMA_CTRL_ACK);

	if (!desc) {
		dev_err(dev, "dmaengine_prep_slave_sg() fail\n");
		return;
	}

	desc->callback		= rsnd_dma_complete;
	desc->callback_param	= dma;

	if (dmaengine_submit(desc) < 0) {
		dev_err(dev, "dmaengine_submit() fail\n");
		return;
	}

	dma_async_issue_pending(dma->chan);
}

int rsnd_dma_available(struct rsnd_dma *dma)
{
	return !!dma->chan;
}

#define DMA_NAME_SIZE 16
#define MOD_MAX 4 /* MEM/SSI/SRC/DVC */
static int _rsnd_dma_of_name(char *dma_name, struct rsnd_mod *mod)
{
	if (mod)
		return snprintf(dma_name, DMA_NAME_SIZE / 2, "%s%d",
			 rsnd_mod_dma_name(mod), rsnd_mod_id(mod));
	else
		return snprintf(dma_name, DMA_NAME_SIZE / 2, "mem");

}

static void rsnd_dma_of_name(struct rsnd_mod *mod_from,
			     struct rsnd_mod *mod_to,
			     char *dma_name)
{
	int index = 0;

	index = _rsnd_dma_of_name(dma_name + index, mod_from);
	*(dma_name + index++) = '_';
	index = _rsnd_dma_of_name(dma_name + index, mod_to);
}

static void rsnd_dma_of_path(struct rsnd_dma *dma,
			     int is_play,
			     struct rsnd_mod **mod_from,
			     struct rsnd_mod **mod_to)
{
	struct rsnd_mod *this = rsnd_dma_to_mod(dma);
	struct rsnd_dai_stream *io = rsnd_mod_to_io(this);
	struct rsnd_mod *ssi = rsnd_io_to_mod_ssi(io);
	struct rsnd_mod *src = rsnd_io_to_mod_src(io);
	struct rsnd_mod *dvc = rsnd_io_to_mod_dvc(io);
	struct rsnd_mod *mod[MOD_MAX];
	int i, index;


	for (i = 0; i < MOD_MAX; i++)
		mod[i] = NULL;

	/*
	 * in play case...
	 *
	 * src -> dst
	 *
	 * mem -> SSI
	 * mem -> SRC -> SSI
	 * mem -> SRC -> DVC -> SSI
	 */
	mod[0] = NULL; /* for "mem" */
	index = 1;
	for (i = 1; i < MOD_MAX; i++) {
		if (!src) {
			mod[i] = ssi;
		} else if (!dvc) {
			mod[i] = src;
			src = NULL;
		} else {
			if ((!is_play) && (this == src))
				this = dvc;

			mod[i] = (is_play) ? src : dvc;
			i++;
			mod[i] = (is_play) ? dvc : src;
			src = NULL;
			dvc = NULL;
		}

		if (mod[i] == this)
			index = i;

		if (mod[i] == ssi)
			break;
	}

	if (is_play) {
		*mod_from = mod[index - 1];
		*mod_to   = mod[index];
	} else {
		*mod_from = mod[index];
		*mod_to   = mod[index - 1];
	}
}

int rsnd_dma_init(struct rsnd_priv *priv, struct rsnd_dma *dma,
		  int is_play, int id)
{
	struct device *dev = rsnd_priv_to_dev(priv);
	struct dma_slave_config cfg;
	struct rsnd_mod *mod_from;
	struct rsnd_mod *mod_to;
	char dma_name[DMA_NAME_SIZE];
	dma_cap_mask_t mask;
	int ret;

	if (dma->chan) {
		dev_err(dev, "it already has dma channel\n");
		return -EIO;
	}

	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);

	rsnd_dma_of_path(dma, is_play, &mod_from, &mod_to);
	rsnd_dma_of_name(mod_from, mod_to, dma_name);

	cfg.slave_id	= id;
	cfg.direction	= is_play ? DMA_MEM_TO_DEV : DMA_DEV_TO_MEM;
	cfg.src_addr	= rsnd_gen_dma_addr(priv, mod_from, is_play, 1);
	cfg.dst_addr	= rsnd_gen_dma_addr(priv, mod_to,   is_play, 0);
	cfg.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
	cfg.dst_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;

	dev_dbg(dev, "dma : %s %pad -> %pad\n",
		dma_name, &cfg.src_addr, &cfg.dst_addr);

	dma->chan = dma_request_slave_channel_compat(mask, shdma_chan_filter,
						     (void *)id, dev,
						     dma_name);
	if (!dma->chan) {
		dev_err(dev, "can't get dma channel\n");
		return -EIO;
	}

	ret = dmaengine_slave_config(dma->chan, &cfg);
	if (ret < 0)
		goto rsnd_dma_init_err;

	dma->addr = is_play ? cfg.src_addr : cfg.dst_addr;
	dma->dir = is_play ? DMA_MEM_TO_DEV : DMA_DEV_TO_MEM;

	return 0;

rsnd_dma_init_err:
	rsnd_dma_quit(priv, dma);
=======
struct dma_chan *rsnd_mod_dma_req(struct rsnd_dai_stream *io,
				  struct rsnd_mod *mod)
{
	if (!mod || !mod->ops || !mod->ops->dma_req)
		return NULL;

	return mod->ops->dma_req(io, mod);
}

u32 *rsnd_mod_get_status(struct rsnd_dai_stream *io,
			 struct rsnd_mod *mod,
			 enum rsnd_mod_type type)
{
	return &mod->status;
}

int rsnd_mod_init(struct rsnd_priv *priv,
		  struct rsnd_mod *mod,
		  struct rsnd_mod_ops *ops,
		  struct clk *clk,
		  u32* (*get_status)(struct rsnd_dai_stream *io,
				     struct rsnd_mod *mod,
				     enum rsnd_mod_type type),
		  enum rsnd_mod_type type,
		  int id)
{
	int ret = clk_prepare(clk);

	if (ret)
		return ret;

	mod->id		= id;
	mod->ops	= ops;
	mod->type	= type;
	mod->clk	= clk;
	mod->priv	= priv;
	mod->get_status	= get_status;
>>>>>>> v4.9.227

	return ret;
}

<<<<<<< HEAD
void  rsnd_dma_quit(struct rsnd_priv *priv,
		    struct rsnd_dma *dma)
{
	if (dma->chan)
		dma_release_channel(dma->chan);

	dma->chan = NULL;
}

/*
 *	settting function
 */
u32 rsnd_get_adinr(struct rsnd_mod *mod)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct rsnd_dai_stream *io = rsnd_mod_to_io(mod);
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	struct device *dev = rsnd_priv_to_dev(priv);
	u32 adinr = runtime->channels;

	switch (runtime->sample_bits) {
	case 16:
		adinr |= (8 << 16);
		break;
	case 32:
		adinr |= (0 << 16);
		break;
	default:
		dev_warn(dev, "not supported sample bits\n");
		return 0;
	}

	return adinr;
=======
void rsnd_mod_quit(struct rsnd_mod *mod)
{
	if (mod->clk)
		clk_unprepare(mod->clk);
	mod->clk = NULL;
}

void rsnd_mod_interrupt(struct rsnd_mod *mod,
			void (*callback)(struct rsnd_mod *mod,
					 struct rsnd_dai_stream *io))
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct rsnd_dai_stream *io;
	struct rsnd_dai *rdai;
	int i;

	for_each_rsnd_dai(rdai, priv, i) {
		io = &rdai->playback;
		if (mod == io->mod[mod->type])
			callback(mod, io);

		io = &rdai->capture;
		if (mod == io->mod[mod->type])
			callback(mod, io);
	}
}

int rsnd_io_is_working(struct rsnd_dai_stream *io)
{
	/* see rsnd_dai_stream_init/quit() */
	return !!io->substream;
}

void rsnd_set_slot(struct rsnd_dai *rdai,
		   int slots, int num)
{
	rdai->slots	= slots;
	rdai->slots_num	= num;
}

int rsnd_get_slot(struct rsnd_dai_stream *io)
{
	struct rsnd_dai *rdai = rsnd_io_to_rdai(io);

	return rdai->slots;
}

int rsnd_get_slot_num(struct rsnd_dai_stream *io)
{
	struct rsnd_dai *rdai = rsnd_io_to_rdai(io);

	return rdai->slots_num;
}

int rsnd_runtime_channel_original(struct rsnd_dai_stream *io)
{
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);

	return runtime->channels;
}

int rsnd_runtime_channel_after_ctu(struct rsnd_dai_stream *io)
{
	int chan = rsnd_runtime_channel_original(io);
	struct rsnd_mod *ctu_mod = rsnd_io_to_mod_ctu(io);

	if (ctu_mod) {
		u32 converted_chan = rsnd_ctu_converted_channel(ctu_mod);

		if (converted_chan)
			return converted_chan;
	}

	return chan;
}

int rsnd_runtime_channel_for_ssi(struct rsnd_dai_stream *io)
{
	int chan = rsnd_io_is_play(io) ?
		rsnd_runtime_channel_after_ctu(io) :
		rsnd_runtime_channel_original(io);

	/* Use Multi SSI */
	if (rsnd_runtime_is_ssi_multi(io))
		chan /= rsnd_get_slot_num(io);

	/* TDM Extend Mode needs 8ch */
	if (chan == 6)
		chan = 8;

	return chan;
}

int rsnd_runtime_is_ssi_multi(struct rsnd_dai_stream *io)
{
	int slots = rsnd_get_slot_num(io);
	int chan = rsnd_io_is_play(io) ?
		rsnd_runtime_channel_after_ctu(io) :
		rsnd_runtime_channel_original(io);

	return (chan >= 6) && (slots > 1);
}

int rsnd_runtime_is_ssi_tdm(struct rsnd_dai_stream *io)
{
	return rsnd_runtime_channel_for_ssi(io) >= 6;
}

/*
 *	ADINR function
 */
u32 rsnd_get_adinr_bit(struct rsnd_mod *mod, struct rsnd_dai_stream *io)
{
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	struct device *dev = rsnd_priv_to_dev(priv);

	switch (runtime->sample_bits) {
	case 16:
		return 8 << 16;
	case 32:
		return 0 << 16;
	}

	dev_warn(dev, "not supported sample bits\n");

	return 0;
}

/*
 *	DALIGN function
 */
u32 rsnd_get_dalign(struct rsnd_mod *mod, struct rsnd_dai_stream *io)
{
	struct rsnd_mod *ssi = rsnd_io_to_mod_ssi(io);
	struct rsnd_mod *target;
	struct snd_pcm_runtime *runtime = rsnd_io_to_runtime(io);
	u32 val = 0x76543210;
	u32 mask = ~0;

	if (rsnd_io_is_play(io)) {
		struct rsnd_mod *src = rsnd_io_to_mod_src(io);

		target = src ? src : ssi;
	} else {
		struct rsnd_mod *cmd = rsnd_io_to_mod_cmd(io);

		target = cmd ? cmd : ssi;
	}

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

	/*
	 * exchange channeles on SRC if possible,
	 * otherwise, R/L volume settings on DVC
	 * changes inverted channels
	 */
	if (mod == target)
		return val;
	else
		return 0x76543210;
>>>>>>> v4.9.227
}

/*
 *	rsnd_dai functions
 */
<<<<<<< HEAD
#define __rsnd_mod_call(mod, func, rdai...)			\
({								\
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);		\
	struct device *dev = rsnd_priv_to_dev(priv);		\
	dev_dbg(dev, "%s [%d] %s\n",				\
		rsnd_mod_name(mod), rsnd_mod_id(mod), #func);	\
	(mod)->ops->func(mod, rdai);				\
})

#define rsnd_mod_call(mod, func, rdai...)	\
	(!(mod) ? -ENODEV :			\
	 !((mod)->ops->func) ? 0 :		\
	 __rsnd_mod_call(mod, func, rdai))

#define rsnd_dai_call(fn, io, rdai...)				\
({								\
	struct rsnd_mod *mod;					\
	int ret = 0, i;						\
	for (i = 0; i < RSND_MOD_MAX; i++) {			\
		mod = (io)->mod[i];				\
		if (!mod)					\
			continue;				\
		ret = rsnd_mod_call(mod, fn, rdai);		\
		if (ret < 0)					\
			break;					\
=======
#define rsnd_mod_call(idx, io, func, param...)			\
({								\
	struct rsnd_priv *priv = rsnd_mod_to_priv(mod);		\
	struct rsnd_mod *mod = (io)->mod[idx];			\
	struct device *dev = rsnd_priv_to_dev(priv);		\
	u32 *status = mod->get_status(io, mod, idx);			\
	u32 mask = 0xF << __rsnd_mod_shift_##func;			\
	u8 val  = (*status >> __rsnd_mod_shift_##func) & 0xF;		\
	u8 add  = ((val + __rsnd_mod_add_##func) & 0xF);		\
	int ret = 0;							\
	int call = (val == __rsnd_mod_call_##func) && (mod)->ops->func;	\
	if (add == 0xF)							\
		call = 0;						\
	else								\
		*status = (*status & ~mask) +				\
			(add << __rsnd_mod_shift_##func);		\
	dev_dbg(dev, "%s[%d]\t0x%08x %s\n",				\
		rsnd_mod_name(mod), rsnd_mod_id(mod),			\
		*status, call ? #func : "");				\
	if (call)							\
		ret = (mod)->ops->func(mod, io, param);			\
	if (ret)							\
		dev_dbg(dev, "%s[%d] : rsnd_mod_call error %d\n",	\
			rsnd_mod_name(mod), rsnd_mod_id(mod), ret);	\
	ret;								\
})

static enum rsnd_mod_type rsnd_mod_sequence[][RSND_MOD_MAX] = {
	{
		/* CAPTURE */
		RSND_MOD_AUDMAPP,
		RSND_MOD_AUDMA,
		RSND_MOD_DVC,
		RSND_MOD_MIX,
		RSND_MOD_CTU,
		RSND_MOD_CMD,
		RSND_MOD_SRC,
		RSND_MOD_SSIU,
		RSND_MOD_SSIM3,
		RSND_MOD_SSIM2,
		RSND_MOD_SSIM1,
		RSND_MOD_SSIP,
		RSND_MOD_SSI,
	}, {
		/* PLAYBACK */
		RSND_MOD_AUDMAPP,
		RSND_MOD_AUDMA,
		RSND_MOD_SSIM3,
		RSND_MOD_SSIM2,
		RSND_MOD_SSIM1,
		RSND_MOD_SSIP,
		RSND_MOD_SSI,
		RSND_MOD_SSIU,
		RSND_MOD_DVC,
		RSND_MOD_MIX,
		RSND_MOD_CTU,
		RSND_MOD_CMD,
		RSND_MOD_SRC,
	},
};

#define rsnd_dai_call(fn, io, param...)				\
({								\
	struct rsnd_mod *mod;					\
	int type, is_play = rsnd_io_is_play(io);		\
	int ret = 0, i;						\
	for (i = 0; i < RSND_MOD_MAX; i++) {			\
		type = rsnd_mod_sequence[is_play][i];		\
		mod = (io)->mod[type];				\
		if (!mod)					\
			continue;				\
		ret |= rsnd_mod_call(type, io, fn, param);	\
>>>>>>> v4.9.227
	}							\
	ret;							\
})

<<<<<<< HEAD
static int rsnd_dai_connect(struct rsnd_mod *mod,
			    struct rsnd_dai_stream *io)
{
	if (!mod)
		return -EIO;

	if (io->mod[mod->type]) {
		struct rsnd_priv *priv = rsnd_mod_to_priv(mod);
		struct device *dev = rsnd_priv_to_dev(priv);

		dev_err(dev, "%s%d is not empty\n",
			rsnd_mod_name(mod),
			rsnd_mod_id(mod));
		return -EIO;
	}

	io->mod[mod->type] = mod;
	mod->io = io;
=======
int rsnd_dai_connect(struct rsnd_mod *mod,
		     struct rsnd_dai_stream *io,
		     enum rsnd_mod_type type)
{
	struct rsnd_priv *priv;
	struct device *dev;

	if (!mod)
		return -EIO;

	if (io->mod[type] == mod)
		return 0;

	if (io->mod[type])
		return -EINVAL;

	priv = rsnd_mod_to_priv(mod);
	dev = rsnd_priv_to_dev(priv);

	io->mod[type] = mod;

	dev_dbg(dev, "%s[%d] is connected to io (%s)\n",
		rsnd_mod_name(mod), rsnd_mod_id(mod),
		rsnd_io_is_play(io) ? "Playback" : "Capture");
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
int rsnd_dai_id(struct rsnd_priv *priv, struct rsnd_dai *rdai)
{
	int id = rdai - priv->rdai;

	if ((id < 0) || (id >= rsnd_rdai_nr(priv)))
		return -EINVAL;

	return id;
}

struct rsnd_dai *rsnd_dai_get(struct rsnd_priv *priv, int id)
=======
static void rsnd_dai_disconnect(struct rsnd_mod *mod,
				struct rsnd_dai_stream *io,
				enum rsnd_mod_type type)
{
	io->mod[type] = NULL;
}

struct rsnd_dai *rsnd_rdai_get(struct rsnd_priv *priv, int id)
>>>>>>> v4.9.227
{
	if ((id < 0) || (id >= rsnd_rdai_nr(priv)))
		return NULL;

	return priv->rdai + id;
}

<<<<<<< HEAD
static struct rsnd_dai *rsnd_dai_to_rdai(struct snd_soc_dai *dai)
{
	struct rsnd_priv *priv = snd_soc_dai_get_drvdata(dai);

	return rsnd_dai_get(priv, dai->id);
}

int rsnd_dai_is_play(struct rsnd_dai *rdai, struct rsnd_dai_stream *io)
{
	return &rdai->playback == io;
=======
#define rsnd_dai_to_priv(dai) snd_soc_dai_get_drvdata(dai)
static struct rsnd_dai *rsnd_dai_to_rdai(struct snd_soc_dai *dai)
{
	struct rsnd_priv *priv = rsnd_dai_to_priv(dai);

	return rsnd_rdai_get(priv, dai->id);
>>>>>>> v4.9.227
}

/*
 *	rsnd_soc_dai functions
 */
int rsnd_dai_pointer_offset(struct rsnd_dai_stream *io, int additional)
{
	struct snd_pcm_substream *substream = io->substream;
	struct snd_pcm_runtime *runtime = substream->runtime;
	int pos = io->byte_pos + additional;

	pos %= (runtime->periods * io->byte_per_period);

	return pos;
}

<<<<<<< HEAD
void rsnd_dai_pointer_update(struct rsnd_dai_stream *io, int byte)
=======
bool rsnd_dai_pointer_update(struct rsnd_dai_stream *io, int byte)
>>>>>>> v4.9.227
{
	io->byte_pos += byte;

	if (io->byte_pos >= io->next_period_byte) {
		struct snd_pcm_substream *substream = io->substream;
		struct snd_pcm_runtime *runtime = substream->runtime;

		io->period_pos++;
		io->next_period_byte += io->byte_per_period;

		if (io->period_pos >= runtime->periods) {
			io->byte_pos = 0;
			io->period_pos = 0;
			io->next_period_byte = io->byte_per_period;
		}

<<<<<<< HEAD
		snd_pcm_period_elapsed(substream);
	}
}

static int rsnd_dai_stream_init(struct rsnd_dai_stream *io,
=======
		return true;
	}

	return false;
}

void rsnd_dai_period_elapsed(struct rsnd_dai_stream *io)
{
	struct snd_pcm_substream *substream = io->substream;

	/*
	 * this function should be called...
	 *
	 * - if rsnd_dai_pointer_update() returns true
	 * - without spin lock
	 */

	snd_pcm_period_elapsed(substream);
}

static void rsnd_dai_stream_init(struct rsnd_dai_stream *io,
>>>>>>> v4.9.227
				struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	io->substream		= substream;
	io->byte_pos		= 0;
	io->period_pos		= 0;
	io->byte_per_period	= runtime->period_size *
				  runtime->channels *
				  samples_to_bytes(runtime, 1);
	io->next_period_byte	= io->byte_per_period;
<<<<<<< HEAD

	return 0;
=======
}

static void rsnd_dai_stream_quit(struct rsnd_dai_stream *io)
{
	io->substream		= NULL;
>>>>>>> v4.9.227
}

static
struct snd_soc_dai *rsnd_substream_to_dai(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;

	return  rtd->cpu_dai;
}

static
struct rsnd_dai_stream *rsnd_rdai_to_io(struct rsnd_dai *rdai,
					struct snd_pcm_substream *substream)
{
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		return &rdai->playback;
	else
		return &rdai->capture;
}

static int rsnd_soc_dai_trigger(struct snd_pcm_substream *substream, int cmd,
			    struct snd_soc_dai *dai)
{
<<<<<<< HEAD
	struct rsnd_priv *priv = snd_soc_dai_get_drvdata(dai);
	struct rsnd_dai *rdai = rsnd_dai_to_rdai(dai);
	struct rsnd_dai_stream *io = rsnd_rdai_to_io(rdai, substream);
	int ssi_id = rsnd_mod_id(rsnd_io_to_mod_ssi(io));
	int ret;
	unsigned long flags;

	rsnd_lock(priv, flags);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		ret = rsnd_dai_stream_init(io, substream);
		if (ret < 0)
			goto dai_trigger_end;

		ret = rsnd_platform_call(priv, dai, start, ssi_id);
		if (ret < 0)
			goto dai_trigger_end;

		ret = rsnd_dai_call(init, io, rdai);
		if (ret < 0)
			goto dai_trigger_end;

		ret = rsnd_dai_call(start, io, rdai);
		if (ret < 0)
			goto dai_trigger_end;
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		ret = rsnd_dai_call(stop, io, rdai);
		if (ret < 0)
			goto dai_trigger_end;

		ret = rsnd_dai_call(quit, io, rdai);
		if (ret < 0)
			goto dai_trigger_end;

		ret = rsnd_platform_call(priv, dai, stop, ssi_id);
		if (ret < 0)
			goto dai_trigger_end;
=======
	struct rsnd_priv *priv = rsnd_dai_to_priv(dai);
	struct rsnd_dai *rdai = rsnd_dai_to_rdai(dai);
	struct rsnd_dai_stream *io = rsnd_rdai_to_io(rdai, substream);
	int ret;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
		rsnd_dai_stream_init(io, substream);

		ret = rsnd_dai_call(init, io, priv);
		if (ret < 0)
			goto dai_trigger_end;

		ret = rsnd_dai_call(start, io, priv);
		if (ret < 0)
			goto dai_trigger_end;

		ret = rsnd_dai_call(irq, io, priv, 1);
		if (ret < 0)
			goto dai_trigger_end;

		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
		ret = rsnd_dai_call(irq, io, priv, 0);

		ret |= rsnd_dai_call(stop, io, priv);

		ret |= rsnd_dai_call(quit, io, priv);

		rsnd_dai_stream_quit(io);
>>>>>>> v4.9.227
		break;
	default:
		ret = -EINVAL;
	}

dai_trigger_end:
<<<<<<< HEAD
	rsnd_unlock(priv, flags);
=======
	spin_unlock_irqrestore(&priv->lock, flags);
>>>>>>> v4.9.227

	return ret;
}

static int rsnd_soc_dai_set_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct rsnd_dai *rdai = rsnd_dai_to_rdai(dai);

	/* set master/slave audio interface */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		rdai->clk_master = 0;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		rdai->clk_master = 1; /* codec is slave, cpu is master */
		break;
	default:
		return -EINVAL;
	}

	/* set format */
<<<<<<< HEAD
=======
	rdai->bit_clk_inv = 0;
>>>>>>> v4.9.227
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		rdai->sys_delay = 0;
		rdai->data_alignment = 0;
		rdai->frm_clk_inv = 0;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		rdai->sys_delay = 1;
		rdai->data_alignment = 0;
		rdai->frm_clk_inv = 1;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		rdai->sys_delay = 1;
		rdai->data_alignment = 1;
		rdai->frm_clk_inv = 1;
		break;
	}

	/* set clock inversion */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_IF:
		rdai->bit_clk_inv =  rdai->bit_clk_inv;
		rdai->frm_clk_inv = !rdai->frm_clk_inv;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		rdai->bit_clk_inv = !rdai->bit_clk_inv;
		rdai->frm_clk_inv =  rdai->frm_clk_inv;
		break;
	case SND_SOC_DAIFMT_IB_IF:
		rdai->bit_clk_inv = !rdai->bit_clk_inv;
		rdai->frm_clk_inv = !rdai->frm_clk_inv;
		break;
	case SND_SOC_DAIFMT_NB_NF:
	default:
		break;
	}

	return 0;
}

<<<<<<< HEAD
static const struct snd_soc_dai_ops rsnd_soc_dai_ops = {
	.trigger	= rsnd_soc_dai_trigger,
	.set_fmt	= rsnd_soc_dai_set_fmt,
};

#define rsnd_path_parse(priv, io, type)				\
({								\
	struct rsnd_mod *mod;					\
	int ret = 0;						\
	int id = -1;						\
								\
	if (rsnd_is_enable_path(io, type)) {			\
		id = rsnd_info_id(priv, io, type);		\
		if (id >= 0) {					\
			mod = rsnd_##type##_mod_get(priv, id);	\
			ret = rsnd_dai_connect(mod, io);	\
		}						\
	}							\
	ret;							\
})

static int rsnd_path_init(struct rsnd_priv *priv,
			  struct rsnd_dai *rdai,
			  struct rsnd_dai_stream *io)
{
	int ret;

	/*
	 * Gen1 is created by SRU/SSI, and this SRU is base module of
	 * Gen2's SCU/SSIU/SSI. (Gen2 SCU/SSIU came from SRU)
	 *
	 * Easy image is..
	 *	Gen1 SRU = Gen2 SCU + SSIU + etc
	 *
	 * Gen2 SCU path is very flexible, but, Gen1 SRU (SCU parts) is
	 * using fixed path.
	 */

	/* SRC */
	ret = rsnd_path_parse(priv, io, src);
	if (ret < 0)
		return ret;

	/* SSI */
	ret = rsnd_path_parse(priv, io, ssi);
	if (ret < 0)
		return ret;

	/* DVC */
	ret = rsnd_path_parse(priv, io, dvc);
	if (ret < 0)
		return ret;

	return ret;
}

static void rsnd_of_parse_dai(struct platform_device *pdev,
			      const struct rsnd_of_data *of_data,
			      struct rsnd_priv *priv)
{
	struct device_node *dai_node,	*dai_np;
	struct device_node *ssi_node,	*ssi_np;
	struct device_node *src_node,	*src_np;
	struct device_node *dvc_node,	*dvc_np;
	struct device_node *playback, *capture;
	struct rsnd_dai_platform_info *dai_info;
	struct rcar_snd_info *info = rsnd_priv_to_info(priv);
	struct device *dev = &pdev->dev;
	int nr, i;
	int dai_i, ssi_i, src_i, dvc_i;

	if (!of_data)
		return;

	dai_node = of_get_child_by_name(dev->of_node, "rcar_sound,dai");
	if (!dai_node)
		return;

	nr = of_get_child_count(dai_node);
	if (!nr)
		return;

	dai_info = devm_kzalloc(dev,
				sizeof(struct rsnd_dai_platform_info) * nr,
				GFP_KERNEL);
	if (!dai_info) {
		dev_err(dev, "dai info allocation error\n");
		return;
	}

	info->dai_info_nr	= nr;
	info->dai_info		= dai_info;

	ssi_node = of_get_child_by_name(dev->of_node, "rcar_sound,ssi");
	src_node = of_get_child_by_name(dev->of_node, "rcar_sound,src");
	dvc_node = of_get_child_by_name(dev->of_node, "rcar_sound,dvc");

#define mod_parse(name)							\
if (name##_node) {							\
	struct rsnd_##name##_platform_info *name##_info;		\
									\
	name##_i = 0;							\
	for_each_child_of_node(name##_node, name##_np) {		\
		name##_info = info->name##_info + name##_i;		\
									\
		if (name##_np == playback)				\
			dai_info->playback.name = name##_info;		\
		if (name##_np == capture)				\
			dai_info->capture.name = name##_info;		\
									\
		name##_i++;						\
	}								\
}

=======
static int rsnd_soc_set_dai_tdm_slot(struct snd_soc_dai *dai,
				     u32 tx_mask, u32 rx_mask,
				     int slots, int slot_width)
{
	struct rsnd_priv *priv = rsnd_dai_to_priv(dai);
	struct rsnd_dai *rdai = rsnd_dai_to_rdai(dai);
	struct device *dev = rsnd_priv_to_dev(priv);

	switch (slots) {
	case 6:
		/* TDM Extend Mode */
		rsnd_set_slot(rdai, slots, 1);
		break;
	default:
		dev_err(dev, "unsupported TDM slots (%d)\n", slots);
		return -EINVAL;
	}

	return 0;
}

static const struct snd_soc_dai_ops rsnd_soc_dai_ops = {
	.trigger	= rsnd_soc_dai_trigger,
	.set_fmt	= rsnd_soc_dai_set_fmt,
	.set_tdm_slot	= rsnd_soc_set_dai_tdm_slot,
};

void rsnd_parse_connect_common(struct rsnd_dai *rdai,
		struct rsnd_mod* (*mod_get)(struct rsnd_priv *priv, int id),
		struct device_node *node,
		struct device_node *playback,
		struct device_node *capture)
{
	struct rsnd_priv *priv = rsnd_rdai_to_priv(rdai);
	struct device_node *np;
	struct rsnd_mod *mod;
	int i;

	if (!node)
		return;

	i = 0;
	for_each_child_of_node(node, np) {
		mod = mod_get(priv, i);
		if (np == playback)
			rsnd_dai_connect(mod, &rdai->playback, mod->type);
		if (np == capture)
			rsnd_dai_connect(mod, &rdai->capture, mod->type);
		i++;
	}

	of_node_put(node);
}

static int rsnd_dai_probe(struct rsnd_priv *priv)
{
	struct device_node *dai_node;
	struct device_node *dai_np;
	struct device_node *playback, *capture;
	struct rsnd_dai_stream *io_playback;
	struct rsnd_dai_stream *io_capture;
	struct snd_soc_dai_driver *rdrv, *drv;
	struct rsnd_dai *rdai;
	struct device *dev = rsnd_priv_to_dev(priv);
	int nr, dai_i, io_i;
	int ret;

	dai_node = rsnd_dai_of_node(priv);
	nr = of_get_child_count(dai_node);
	if (!nr) {
		ret = -EINVAL;
		goto rsnd_dai_probe_done;
	}

	rdrv = devm_kzalloc(dev, sizeof(*rdrv) * nr, GFP_KERNEL);
	rdai = devm_kzalloc(dev, sizeof(*rdai) * nr, GFP_KERNEL);
	if (!rdrv || !rdai) {
		ret = -ENOMEM;
		goto rsnd_dai_probe_done;
	}

	priv->rdai_nr	= nr;
	priv->daidrv	= rdrv;
	priv->rdai	= rdai;

>>>>>>> v4.9.227
	/*
	 * parse all dai
	 */
	dai_i = 0;
	for_each_child_of_node(dai_node, dai_np) {
<<<<<<< HEAD
		dai_info = info->dai_info + dai_i;

		for (i = 0;; i++) {

			playback = of_parse_phandle(dai_np, "playback", i);
			capture  = of_parse_phandle(dai_np, "capture", i);
=======
		rdai		= rsnd_rdai_get(priv, dai_i);
		drv		= rdrv + dai_i;
		io_playback	= &rdai->playback;
		io_capture	= &rdai->capture;

		snprintf(rdai->name, RSND_DAI_NAME_SIZE, "rsnd-dai.%d", dai_i);

		rdai->priv	= priv;
		drv->name	= rdai->name;
		drv->ops	= &rsnd_soc_dai_ops;

		snprintf(rdai->playback.name, RSND_DAI_NAME_SIZE,
			 "DAI%d Playback", dai_i);
		drv->playback.rates		= RSND_RATES;
		drv->playback.formats		= RSND_FMTS;
		drv->playback.channels_min	= 2;
		drv->playback.channels_max	= 6;
		drv->playback.stream_name	= rdai->playback.name;

		snprintf(rdai->capture.name, RSND_DAI_NAME_SIZE,
			 "DAI%d Capture", dai_i);
		drv->capture.rates		= RSND_RATES;
		drv->capture.formats		= RSND_FMTS;
		drv->capture.channels_min	= 2;
		drv->capture.channels_max	= 6;
		drv->capture.stream_name	= rdai->capture.name;

		rdai->playback.rdai		= rdai;
		rdai->capture.rdai		= rdai;
		rsnd_set_slot(rdai, 2, 1); /* default */

		for (io_i = 0;; io_i++) {
			playback = of_parse_phandle(dai_np, "playback", io_i);
			capture  = of_parse_phandle(dai_np, "capture", io_i);
>>>>>>> v4.9.227

			if (!playback && !capture)
				break;

<<<<<<< HEAD
			mod_parse(ssi);
			mod_parse(src);
			mod_parse(dvc);
=======
			rsnd_parse_connect_ssi(rdai, playback, capture);
			rsnd_parse_connect_src(rdai, playback, capture);
			rsnd_parse_connect_ctu(rdai, playback, capture);
			rsnd_parse_connect_mix(rdai, playback, capture);
			rsnd_parse_connect_dvc(rdai, playback, capture);
>>>>>>> v4.9.227

			of_node_put(playback);
			of_node_put(capture);
		}

		dai_i++;
<<<<<<< HEAD
	}
}

static int rsnd_dai_probe(struct platform_device *pdev,
			  const struct rsnd_of_data *of_data,
			  struct rsnd_priv *priv)
{
	struct snd_soc_dai_driver *drv;
	struct rcar_snd_info *info = rsnd_priv_to_info(priv);
	struct rsnd_dai *rdai;
	struct rsnd_ssi_platform_info *pmod, *cmod;
	struct device *dev = rsnd_priv_to_dev(priv);
	int dai_nr;
	int i;

	rsnd_of_parse_dai(pdev, of_data, priv);

	dai_nr = info->dai_info_nr;
	if (!dai_nr) {
		dev_err(dev, "no dai\n");
		return -EIO;
	}

	drv  = devm_kzalloc(dev, sizeof(*drv)  * dai_nr, GFP_KERNEL);
	rdai = devm_kzalloc(dev, sizeof(*rdai) * dai_nr, GFP_KERNEL);
	if (!drv || !rdai) {
		dev_err(dev, "dai allocate failed\n");
		return -ENOMEM;
	}

	priv->rdai_nr	= dai_nr;
	priv->daidrv	= drv;
	priv->rdai	= rdai;

	for (i = 0; i < dai_nr; i++) {
		rdai[i].info = &info->dai_info[i];

		pmod = rdai[i].info->playback.ssi;
		cmod = rdai[i].info->capture.ssi;

		/*
		 *	init rsnd_dai
		 */
		snprintf(rdai[i].name, RSND_DAI_NAME_SIZE, "rsnd-dai.%d", i);

		/*
		 *	init snd_soc_dai_driver
		 */
		drv[i].name	= rdai[i].name;
		drv[i].ops	= &rsnd_soc_dai_ops;
		if (pmod) {
			drv[i].playback.rates		= RSND_RATES;
			drv[i].playback.formats		= RSND_FMTS;
			drv[i].playback.channels_min	= 2;
			drv[i].playback.channels_max	= 2;

			rdai[i].playback.info = &info->dai_info[i].playback;
			rsnd_path_init(priv, &rdai[i], &rdai[i].playback);
		}
		if (cmod) {
			drv[i].capture.rates		= RSND_RATES;
			drv[i].capture.formats		= RSND_FMTS;
			drv[i].capture.channels_min	= 2;
			drv[i].capture.channels_max	= 2;

			rdai[i].capture.info = &info->dai_info[i].capture;
			rsnd_path_init(priv, &rdai[i], &rdai[i].capture);
		}

		dev_dbg(dev, "%s (%s/%s)\n", rdai[i].name,
			pmod ? "play"    : " -- ",
			cmod ? "capture" : "  --   ");
	}

	return 0;
=======

		dev_dbg(dev, "%s (%s/%s)\n", rdai->name,
			rsnd_io_to_mod_ssi(io_playback) ? "play"    : " -- ",
			rsnd_io_to_mod_ssi(io_capture) ? "capture" : "  --   ");
	}

	ret = 0;

rsnd_dai_probe_done:
	of_node_put(dai_node);

	return ret;
>>>>>>> v4.9.227
}

/*
 *		pcm ops
 */
static struct snd_pcm_hardware rsnd_pcm_hardware = {
	.info =		SNDRV_PCM_INFO_INTERLEAVED	|
			SNDRV_PCM_INFO_MMAP		|
			SNDRV_PCM_INFO_MMAP_VALID,
	.buffer_bytes_max	= 64 * 1024,
	.period_bytes_min	= 32,
	.period_bytes_max	= 8192,
	.periods_min		= 1,
	.periods_max		= 32,
	.fifo_size		= 256,
};

static int rsnd_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int ret = 0;

	snd_soc_set_runtime_hwparams(substream, &rsnd_pcm_hardware);

	ret = snd_pcm_hw_constraint_integer(runtime,
					    SNDRV_PCM_HW_PARAM_PERIODS);

	return ret;
}

static int rsnd_hw_params(struct snd_pcm_substream *substream,
			 struct snd_pcm_hw_params *hw_params)
{
<<<<<<< HEAD
=======
	struct snd_soc_dai *dai = rsnd_substream_to_dai(substream);
	struct rsnd_dai *rdai = rsnd_dai_to_rdai(dai);
	struct rsnd_dai_stream *io = rsnd_rdai_to_io(rdai, substream);
	int ret;

	ret = rsnd_dai_call(hw_params, io, substream, hw_params);
	if (ret)
		return ret;

>>>>>>> v4.9.227
	return snd_pcm_lib_malloc_pages(substream,
					params_buffer_bytes(hw_params));
}

static snd_pcm_uframes_t rsnd_pointer(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_soc_dai *dai = rsnd_substream_to_dai(substream);
	struct rsnd_dai *rdai = rsnd_dai_to_rdai(dai);
	struct rsnd_dai_stream *io = rsnd_rdai_to_io(rdai, substream);

	return bytes_to_frames(runtime, io->byte_pos);
}

static struct snd_pcm_ops rsnd_pcm_ops = {
	.open		= rsnd_pcm_open,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= rsnd_hw_params,
	.hw_free	= snd_pcm_lib_free_pages,
	.pointer	= rsnd_pointer,
};

/*
<<<<<<< HEAD
=======
 *		snd_kcontrol
 */
#define kcontrol_to_cfg(kctrl) ((struct rsnd_kctrl_cfg *)kctrl->private_value)
static int rsnd_kctrl_info(struct snd_kcontrol *kctrl,
			   struct snd_ctl_elem_info *uinfo)
{
	struct rsnd_kctrl_cfg *cfg = kcontrol_to_cfg(kctrl);

	if (cfg->texts) {
		uinfo->type = SNDRV_CTL_ELEM_TYPE_ENUMERATED;
		uinfo->count = cfg->size;
		uinfo->value.enumerated.items = cfg->max;
		if (uinfo->value.enumerated.item >= cfg->max)
			uinfo->value.enumerated.item = cfg->max - 1;
		strlcpy(uinfo->value.enumerated.name,
			cfg->texts[uinfo->value.enumerated.item],
			sizeof(uinfo->value.enumerated.name));
	} else {
		uinfo->count = cfg->size;
		uinfo->value.integer.min = 0;
		uinfo->value.integer.max = cfg->max;
		uinfo->type = (cfg->max == 1) ?
			SNDRV_CTL_ELEM_TYPE_BOOLEAN :
			SNDRV_CTL_ELEM_TYPE_INTEGER;
	}

	return 0;
}

static int rsnd_kctrl_get(struct snd_kcontrol *kctrl,
			  struct snd_ctl_elem_value *uc)
{
	struct rsnd_kctrl_cfg *cfg = kcontrol_to_cfg(kctrl);
	int i;

	for (i = 0; i < cfg->size; i++)
		if (cfg->texts)
			uc->value.enumerated.item[i] = cfg->val[i];
		else
			uc->value.integer.value[i] = cfg->val[i];

	return 0;
}

static int rsnd_kctrl_put(struct snd_kcontrol *kctrl,
			  struct snd_ctl_elem_value *uc)
{
	struct rsnd_mod *mod = snd_kcontrol_chip(kctrl);
	struct rsnd_kctrl_cfg *cfg = kcontrol_to_cfg(kctrl);
	int i, change = 0;

	for (i = 0; i < cfg->size; i++) {
		if (cfg->texts) {
			change |= (uc->value.enumerated.item[i] != cfg->val[i]);
			cfg->val[i] = uc->value.enumerated.item[i];
		} else {
			change |= (uc->value.integer.value[i] != cfg->val[i]);
			cfg->val[i] = uc->value.integer.value[i];
		}
	}

	if (change && cfg->update)
		cfg->update(cfg->io, mod);

	return change;
}

static int __rsnd_kctrl_new(struct rsnd_mod *mod,
			    struct rsnd_dai_stream *io,
			    struct snd_soc_pcm_runtime *rtd,
			    const unsigned char *name,
			    struct rsnd_kctrl_cfg *cfg,
			    void (*update)(struct rsnd_dai_stream *io,
					   struct rsnd_mod *mod))
{
	struct snd_card *card = rtd->card->snd_card;
	struct snd_kcontrol *kctrl;
	struct snd_kcontrol_new knew = {
		.iface		= SNDRV_CTL_ELEM_IFACE_MIXER,
		.name		= name,
		.info		= rsnd_kctrl_info,
		.index		= rtd->num,
		.get		= rsnd_kctrl_get,
		.put		= rsnd_kctrl_put,
		.private_value	= (unsigned long)cfg,
	};
	int ret;

	kctrl = snd_ctl_new1(&knew, mod);
	if (!kctrl)
		return -ENOMEM;

	ret = snd_ctl_add(card, kctrl);
	if (ret < 0)
		return ret;

	cfg->update = update;
	cfg->card = card;
	cfg->kctrl = kctrl;
	cfg->io = io;

	return 0;
}

void _rsnd_kctrl_remove(struct rsnd_kctrl_cfg *cfg)
{
	snd_ctl_remove(cfg->card, cfg->kctrl);
}

int rsnd_kctrl_new_m(struct rsnd_mod *mod,
		     struct rsnd_dai_stream *io,
		     struct snd_soc_pcm_runtime *rtd,
		     const unsigned char *name,
		     void (*update)(struct rsnd_dai_stream *io,
				    struct rsnd_mod *mod),
		     struct rsnd_kctrl_cfg_m *_cfg,
		     int ch_size,
		     u32 max)
{
	if (ch_size > RSND_MAX_CHANNELS)
		return -EINVAL;

	_cfg->cfg.max	= max;
	_cfg->cfg.size	= ch_size;
	_cfg->cfg.val	= _cfg->val;
	return __rsnd_kctrl_new(mod, io, rtd, name, &_cfg->cfg, update);
}

int rsnd_kctrl_new_s(struct rsnd_mod *mod,
		     struct rsnd_dai_stream *io,
		     struct snd_soc_pcm_runtime *rtd,
		     const unsigned char *name,
		     void (*update)(struct rsnd_dai_stream *io,
				    struct rsnd_mod *mod),
		     struct rsnd_kctrl_cfg_s *_cfg,
		     u32 max)
{
	_cfg->cfg.max	= max;
	_cfg->cfg.size	= 1;
	_cfg->cfg.val	= &_cfg->val;
	return __rsnd_kctrl_new(mod, io, rtd, name, &_cfg->cfg, update);
}

int rsnd_kctrl_new_e(struct rsnd_mod *mod,
		     struct rsnd_dai_stream *io,
		     struct snd_soc_pcm_runtime *rtd,
		     const unsigned char *name,
		     struct rsnd_kctrl_cfg_s *_cfg,
		     void (*update)(struct rsnd_dai_stream *io,
				    struct rsnd_mod *mod),
		     const char * const *texts,
		     u32 max)
{
	_cfg->cfg.max	= max;
	_cfg->cfg.size	= 1;
	_cfg->cfg.val	= &_cfg->val;
	_cfg->cfg.texts	= texts;
	return __rsnd_kctrl_new(mod, io, rtd, name, &_cfg->cfg, update);
}

/*
>>>>>>> v4.9.227
 *		snd_soc_platform
 */

#define PREALLOC_BUFFER		(32 * 1024)
#define PREALLOC_BUFFER_MAX	(32 * 1024)

static int rsnd_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_dai *dai = rtd->cpu_dai;
	struct rsnd_dai *rdai = rsnd_dai_to_rdai(dai);
	int ret;

<<<<<<< HEAD
	ret = rsnd_dai_call(pcm_new, &rdai->playback, rdai, rtd);
	if (ret)
		return ret;

	ret = rsnd_dai_call(pcm_new, &rdai->capture, rdai, rtd);
=======
	ret = rsnd_dai_call(pcm_new, &rdai->playback, rtd);
	if (ret)
		return ret;

	ret = rsnd_dai_call(pcm_new, &rdai->capture, rtd);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	return snd_pcm_lib_preallocate_pages_for_all(
		rtd->pcm,
		SNDRV_DMA_TYPE_DEV,
		rtd->card->snd_card->dev,
		PREALLOC_BUFFER, PREALLOC_BUFFER_MAX);
}

<<<<<<< HEAD
static void rsnd_pcm_free(struct snd_pcm *pcm)
{
	snd_pcm_lib_preallocate_free_for_all(pcm);
}

static struct snd_soc_platform_driver rsnd_soc_platform = {
	.ops		= &rsnd_pcm_ops,
	.pcm_new	= rsnd_pcm_new,
	.pcm_free	= rsnd_pcm_free,
=======
static struct snd_soc_platform_driver rsnd_soc_platform = {
	.ops		= &rsnd_pcm_ops,
	.pcm_new	= rsnd_pcm_new,
>>>>>>> v4.9.227
};

static const struct snd_soc_component_driver rsnd_soc_component = {
	.name		= "rsnd",
};

<<<<<<< HEAD
=======
static int rsnd_rdai_continuance_probe(struct rsnd_priv *priv,
				       struct rsnd_dai_stream *io)
{
	int ret;

	ret = rsnd_dai_call(probe, io, priv);
	if (ret == -EAGAIN) {
		struct rsnd_mod *ssi_mod = rsnd_io_to_mod_ssi(io);
		int i;

		/*
		 * Fallback to PIO mode
		 */

		/*
		 * call "remove" for SSI/SRC/DVC
		 * SSI will be switch to PIO mode if it was DMA mode
		 * see
		 *	rsnd_dma_init()
		 *	rsnd_ssi_fallback()
		 */
		rsnd_dai_call(remove, io, priv);

		/*
		 * remove all mod from io
		 * and, re connect ssi
		 */
		for (i = 0; i < RSND_MOD_MAX; i++)
			rsnd_dai_disconnect((io)->mod[i], io, i);
		rsnd_dai_connect(ssi_mod, io, RSND_MOD_SSI);

		/*
		 * fallback
		 */
		rsnd_dai_call(fallback, io, priv);

		/*
		 * retry to "probe".
		 * DAI has SSI which is PIO mode only now.
		 */
		ret = rsnd_dai_call(probe, io, priv);
	}

	return ret;
}

>>>>>>> v4.9.227
/*
 *	rsnd probe
 */
static int rsnd_probe(struct platform_device *pdev)
{
<<<<<<< HEAD
	struct rcar_snd_info *info;
	struct rsnd_priv *priv;
	struct device *dev = &pdev->dev;
	struct rsnd_dai *rdai;
	const struct of_device_id *of_id = of_match_device(rsnd_of_match, dev);
	const struct rsnd_of_data *of_data;
	int (*probe_func[])(struct platform_device *pdev,
			    const struct rsnd_of_data *of_data,
			    struct rsnd_priv *priv) = {
		rsnd_gen_probe,
		rsnd_ssi_probe,
		rsnd_src_probe,
		rsnd_dvc_probe,
=======
	struct rsnd_priv *priv;
	struct device *dev = &pdev->dev;
	struct rsnd_dai *rdai;
	int (*probe_func[])(struct rsnd_priv *priv) = {
		rsnd_gen_probe,
		rsnd_dma_probe,
		rsnd_ssi_probe,
		rsnd_ssiu_probe,
		rsnd_src_probe,
		rsnd_ctu_probe,
		rsnd_mix_probe,
		rsnd_dvc_probe,
		rsnd_cmd_probe,
>>>>>>> v4.9.227
		rsnd_adg_probe,
		rsnd_dai_probe,
	};
	int ret, i;

<<<<<<< HEAD
	info = NULL;
	of_data = NULL;
	if (of_id) {
		info = devm_kzalloc(&pdev->dev,
				    sizeof(struct rcar_snd_info), GFP_KERNEL);
		of_data = of_id->data;
	} else {
		info = pdev->dev.platform_data;
	}

	if (!info) {
		dev_err(dev, "driver needs R-Car sound information\n");
		return -ENODEV;
	}

=======
>>>>>>> v4.9.227
	/*
	 *	init priv data
	 */
	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		dev_err(dev, "priv allocate failed\n");
		return -ENODEV;
	}

	priv->pdev	= pdev;
<<<<<<< HEAD
	priv->info	= info;
=======
	priv->flags	= (unsigned long)of_device_get_match_data(dev);
>>>>>>> v4.9.227
	spin_lock_init(&priv->lock);

	/*
	 *	init each module
	 */
	for (i = 0; i < ARRAY_SIZE(probe_func); i++) {
<<<<<<< HEAD
		ret = probe_func[i](pdev, of_data, priv);
=======
		ret = probe_func[i](priv);
>>>>>>> v4.9.227
		if (ret)
			return ret;
	}

	for_each_rsnd_dai(rdai, priv, i) {
<<<<<<< HEAD
		ret = rsnd_dai_call(probe, &rdai->playback, rdai);
		if (ret)
			goto exit_snd_probe;

		ret = rsnd_dai_call(probe, &rdai->capture, rdai);
=======
		ret = rsnd_rdai_continuance_probe(priv, &rdai->playback);
		if (ret)
			goto exit_snd_probe;

		ret = rsnd_rdai_continuance_probe(priv, &rdai->capture);
>>>>>>> v4.9.227
		if (ret)
			goto exit_snd_probe;
	}

<<<<<<< HEAD
=======
	dev_set_drvdata(dev, priv);

>>>>>>> v4.9.227
	/*
	 *	asoc register
	 */
	ret = snd_soc_register_platform(dev, &rsnd_soc_platform);
	if (ret < 0) {
		dev_err(dev, "cannot snd soc register\n");
		return ret;
	}

	ret = snd_soc_register_component(dev, &rsnd_soc_component,
					 priv->daidrv, rsnd_rdai_nr(priv));
	if (ret < 0) {
		dev_err(dev, "cannot snd dai register\n");
		goto exit_snd_soc;
	}

<<<<<<< HEAD
	dev_set_drvdata(dev, priv);

=======
>>>>>>> v4.9.227
	pm_runtime_enable(dev);

	dev_info(dev, "probed\n");
	return ret;

exit_snd_soc:
	snd_soc_unregister_platform(dev);
exit_snd_probe:
	for_each_rsnd_dai(rdai, priv, i) {
<<<<<<< HEAD
		rsnd_dai_call(remove, &rdai->playback, rdai);
		rsnd_dai_call(remove, &rdai->capture, rdai);
=======
		rsnd_dai_call(remove, &rdai->playback, priv);
		rsnd_dai_call(remove, &rdai->capture, priv);
>>>>>>> v4.9.227
	}

	return ret;
}

static int rsnd_remove(struct platform_device *pdev)
{
	struct rsnd_priv *priv = dev_get_drvdata(&pdev->dev);
	struct rsnd_dai *rdai;
<<<<<<< HEAD
=======
	void (*remove_func[])(struct rsnd_priv *priv) = {
		rsnd_ssi_remove,
		rsnd_ssiu_remove,
		rsnd_src_remove,
		rsnd_ctu_remove,
		rsnd_mix_remove,
		rsnd_dvc_remove,
		rsnd_cmd_remove,
		rsnd_adg_remove,
	};
>>>>>>> v4.9.227
	int ret = 0, i;

	pm_runtime_disable(&pdev->dev);

	for_each_rsnd_dai(rdai, priv, i) {
<<<<<<< HEAD
		ret |= rsnd_dai_call(remove, &rdai->playback, rdai);
		ret |= rsnd_dai_call(remove, &rdai->capture, rdai);
	}

=======
		ret |= rsnd_dai_call(remove, &rdai->playback, priv);
		ret |= rsnd_dai_call(remove, &rdai->capture, priv);
	}

	for (i = 0; i < ARRAY_SIZE(remove_func); i++)
		remove_func[i](priv);

	snd_soc_unregister_component(&pdev->dev);
	snd_soc_unregister_platform(&pdev->dev);

>>>>>>> v4.9.227
	return ret;
}

static struct platform_driver rsnd_driver = {
	.driver	= {
		.name	= "rcar_sound",
		.of_match_table = rsnd_of_match,
	},
	.probe		= rsnd_probe,
	.remove		= rsnd_remove,
};
module_platform_driver(rsnd_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Renesas R-Car audio driver");
MODULE_AUTHOR("Kuninori Morimoto <kuninori.morimoto.gx@renesas.com>");
MODULE_ALIAS("platform:rcar-pcm-audio");
