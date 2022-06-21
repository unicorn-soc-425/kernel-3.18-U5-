/* sound/soc/samsung/i2s.c
 *
 * ALSA SoC Audio Layer - Samsung I2S Controller driver
 *
 * Copyright (c) 2010 Samsung Electronics Co. Ltd.
 *	Jaswinder Singh <jassisinghbrar@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

<<<<<<< HEAD
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
=======
#include <dt-bindings/sound/samsung-i2s.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
>>>>>>> v4.9.227
#include <linux/of_gpio.h>
#include <linux/pm_runtime.h>

#include <sound/soc.h>
#include <sound/pcm_params.h>

#include <linux/platform_data/asoc-s3c.h>

#include "dma.h"
#include "idma.h"
#include "i2s.h"
#include "i2s-regs.h"

#define msecs_to_loops(t) (loops_per_jiffy / 1000 * HZ * t)

enum samsung_dai_type {
	TYPE_PRI,
	TYPE_SEC,
};

<<<<<<< HEAD
struct samsung_i2s_dai_data {
	int dai_type;
	u32 quirks;
=======
struct samsung_i2s_variant_regs {
	unsigned int	bfs_off;
	unsigned int	rfs_off;
	unsigned int	sdf_off;
	unsigned int	txr_off;
	unsigned int	rclksrc_off;
	unsigned int	mss_off;
	unsigned int	cdclkcon_off;
	unsigned int	lrp_off;
	unsigned int	bfs_mask;
	unsigned int	rfs_mask;
	unsigned int	ftx0cnt_off;
};

struct samsung_i2s_dai_data {
	int dai_type;
	u32 quirks;
	const struct samsung_i2s_variant_regs *i2s_variant_regs;
>>>>>>> v4.9.227
};

struct i2s_dai {
	/* Platform device for this DAI */
	struct platform_device *pdev;
<<<<<<< HEAD
	/* IOREMAP'd SFRs */
	void __iomem	*addr;
	/* Physical base address of SFRs */
	u32	base;
=======
	/* Memory mapped SFR region */
	void __iomem	*addr;
>>>>>>> v4.9.227
	/* Rate of RCLK source clock */
	unsigned long rclk_srcrate;
	/* Frame Clock */
	unsigned frmclk;
	/*
	 * Specifically requested RCLK,BCLK by MACHINE Driver.
	 * 0 indicates CPU driver is free to choose any value.
	 */
	unsigned rfs, bfs;
	/* I2S Controller's core clock */
	struct clk *clk;
	/* Clock for generating I2S signals */
	struct clk *op_clk;
	/* Pointer to the Primary_Fifo if this is Sec_Fifo, NULL otherwise */
	struct i2s_dai *pri_dai;
	/* Pointer to the Secondary_Fifo if it has one, NULL otherwise */
	struct i2s_dai *sec_dai;
#define DAI_OPENED	(1 << 0) /* Dai is opened */
#define DAI_MANAGER	(1 << 1) /* Dai is the manager */
	unsigned mode;
<<<<<<< HEAD
	/* CDCLK pin direction: 0  - input, 1 - output */
	unsigned int cdclk_out:1;
	/* Driver for this DAI */
	struct snd_soc_dai_driver i2s_dai_drv;
	/* DMA parameters */
	struct s3c_dma_params dma_playback;
	struct s3c_dma_params dma_capture;
	struct s3c_dma_params idma_playback;
=======
	/* Driver for this DAI */
	struct snd_soc_dai_driver i2s_dai_drv;
	/* DMA parameters */
	struct snd_dmaengine_dai_dma_data dma_playback;
	struct snd_dmaengine_dai_dma_data dma_capture;
	struct snd_dmaengine_dai_dma_data idma_playback;
	dma_filter_fn filter;
>>>>>>> v4.9.227
	u32	quirks;
	u32	suspend_i2smod;
	u32	suspend_i2scon;
	u32	suspend_i2spsr;
<<<<<<< HEAD
	unsigned long gpios[7];	/* i2s gpio line numbers */
=======
	const struct samsung_i2s_variant_regs *variant_regs;

	/* Spinlock protecting access to the device's registers */
	spinlock_t spinlock;
	spinlock_t *lock;

	/* Below fields are only valid if this is the primary FIFO */
	struct clk *clk_table[3];
	struct clk_onecell_data clk_data;
>>>>>>> v4.9.227
};

/* Lock for cross i/f checks */
static DEFINE_SPINLOCK(lock);

/* If this is the 'overlay' stereo DAI */
static inline bool is_secondary(struct i2s_dai *i2s)
{
	return i2s->pri_dai ? true : false;
}

/* If operating in SoC-Slave mode */
static inline bool is_slave(struct i2s_dai *i2s)
{
<<<<<<< HEAD
	return (readl(i2s->addr + I2SMOD) & MOD_SLAVE) ? true : false;
=======
	u32 mod = readl(i2s->addr + I2SMOD);
	return (mod & (1 << i2s->variant_regs->mss_off)) ? true : false;
>>>>>>> v4.9.227
}

/* If this interface of the controller is transmitting data */
static inline bool tx_active(struct i2s_dai *i2s)
{
	u32 active;

	if (!i2s)
		return false;

	active = readl(i2s->addr + I2SCON);

	if (is_secondary(i2s))
		active &= CON_TXSDMA_ACTIVE;
	else
		active &= CON_TXDMA_ACTIVE;

	return active ? true : false;
}

<<<<<<< HEAD
/* If the other interface of the controller is transmitting data */
static inline bool other_tx_active(struct i2s_dai *i2s)
{
	struct i2s_dai *other = i2s->pri_dai ? : i2s->sec_dai;
=======
/* Return pointer to the other DAI */
static inline struct i2s_dai *get_other_dai(struct i2s_dai *i2s)
{
	return i2s->pri_dai ? : i2s->sec_dai;
}

/* If the other interface of the controller is transmitting data */
static inline bool other_tx_active(struct i2s_dai *i2s)
{
	struct i2s_dai *other = get_other_dai(i2s);
>>>>>>> v4.9.227

	return tx_active(other);
}

/* If any interface of the controller is transmitting data */
static inline bool any_tx_active(struct i2s_dai *i2s)
{
	return tx_active(i2s) || other_tx_active(i2s);
}

/* If this interface of the controller is receiving data */
static inline bool rx_active(struct i2s_dai *i2s)
{
	u32 active;

	if (!i2s)
		return false;

	active = readl(i2s->addr + I2SCON) & CON_RXDMA_ACTIVE;

	return active ? true : false;
}

/* If the other interface of the controller is receiving data */
static inline bool other_rx_active(struct i2s_dai *i2s)
{
<<<<<<< HEAD
	struct i2s_dai *other = i2s->pri_dai ? : i2s->sec_dai;
=======
	struct i2s_dai *other = get_other_dai(i2s);
>>>>>>> v4.9.227

	return rx_active(other);
}

/* If any interface of the controller is receiving data */
static inline bool any_rx_active(struct i2s_dai *i2s)
{
	return rx_active(i2s) || other_rx_active(i2s);
}

/* If the other DAI is transmitting or receiving data */
static inline bool other_active(struct i2s_dai *i2s)
{
	return other_rx_active(i2s) || other_tx_active(i2s);
}

/* If this DAI is transmitting or receiving data */
static inline bool this_active(struct i2s_dai *i2s)
{
	return tx_active(i2s) || rx_active(i2s);
}

/* If the controller is active anyway */
static inline bool any_active(struct i2s_dai *i2s)
{
	return this_active(i2s) || other_active(i2s);
}

static inline struct i2s_dai *to_info(struct snd_soc_dai *dai)
{
	return snd_soc_dai_get_drvdata(dai);
}

static inline bool is_opened(struct i2s_dai *i2s)
{
	if (i2s && (i2s->mode & DAI_OPENED))
		return true;
	else
		return false;
}

static inline bool is_manager(struct i2s_dai *i2s)
{
	if (is_opened(i2s) && (i2s->mode & DAI_MANAGER))
		return true;
	else
		return false;
}

/* Read RCLK of I2S (in multiples of LRCLK) */
static inline unsigned get_rfs(struct i2s_dai *i2s)
{
	u32 rfs;
<<<<<<< HEAD

	if (i2s->quirks & QUIRK_SUPPORTS_TDM)
		rfs = readl(i2s->addr + I2SMOD) >> EXYNOS5420_MOD_RCLK_SHIFT;
	else
		rfs = (readl(i2s->addr + I2SMOD) >> MOD_RCLK_SHIFT);
	rfs &= MOD_RCLK_MASK;

	switch (rfs) {
=======
	rfs = readl(i2s->addr + I2SMOD) >> i2s->variant_regs->rfs_off;
	rfs &= i2s->variant_regs->rfs_mask;

	switch (rfs) {
	case 7: return 192;
	case 6: return 96;
	case 5: return 128;
	case 4: return 64;
>>>>>>> v4.9.227
	case 3:	return 768;
	case 2: return 384;
	case 1:	return 512;
	default: return 256;
	}
}

/* Write RCLK of I2S (in multiples of LRCLK) */
static inline void set_rfs(struct i2s_dai *i2s, unsigned rfs)
{
	u32 mod = readl(i2s->addr + I2SMOD);
<<<<<<< HEAD
	int rfs_shift;

	if (i2s->quirks & QUIRK_SUPPORTS_TDM)
		rfs_shift = EXYNOS5420_MOD_RCLK_SHIFT;
	else
		rfs_shift = MOD_RCLK_SHIFT;
	mod &= ~(MOD_RCLK_MASK << rfs_shift);

	switch (rfs) {
=======
	int rfs_shift = i2s->variant_regs->rfs_off;

	mod &= ~(i2s->variant_regs->rfs_mask << rfs_shift);

	switch (rfs) {
	case 192:
		mod |= (EXYNOS7_MOD_RCLK_192FS << rfs_shift);
		break;
	case 96:
		mod |= (EXYNOS7_MOD_RCLK_96FS << rfs_shift);
		break;
	case 128:
		mod |= (EXYNOS7_MOD_RCLK_128FS << rfs_shift);
		break;
	case 64:
		mod |= (EXYNOS7_MOD_RCLK_64FS << rfs_shift);
		break;
>>>>>>> v4.9.227
	case 768:
		mod |= (MOD_RCLK_768FS << rfs_shift);
		break;
	case 512:
		mod |= (MOD_RCLK_512FS << rfs_shift);
		break;
	case 384:
		mod |= (MOD_RCLK_384FS << rfs_shift);
		break;
	default:
		mod |= (MOD_RCLK_256FS << rfs_shift);
		break;
	}

	writel(mod, i2s->addr + I2SMOD);
}

/* Read Bit-Clock of I2S (in multiples of LRCLK) */
static inline unsigned get_bfs(struct i2s_dai *i2s)
{
	u32 bfs;
<<<<<<< HEAD

	if (i2s->quirks & QUIRK_SUPPORTS_TDM) {
		bfs = readl(i2s->addr + I2SMOD) >> EXYNOS5420_MOD_BCLK_SHIFT;
		bfs &= EXYNOS5420_MOD_BCLK_MASK;
	} else {
		bfs =  readl(i2s->addr + I2SMOD) >> MOD_BCLK_SHIFT;
		bfs &= MOD_BCLK_MASK;
	}
=======
	bfs = readl(i2s->addr + I2SMOD) >> i2s->variant_regs->bfs_off;
	bfs &= i2s->variant_regs->bfs_mask;
>>>>>>> v4.9.227

	switch (bfs) {
	case 8: return 256;
	case 7: return 192;
	case 6: return 128;
	case 5: return 96;
	case 4: return 64;
	case 3: return 24;
	case 2: return 16;
	case 1:	return 48;
	default: return 32;
	}
}

/* Write Bit-Clock of I2S (in multiples of LRCLK) */
static inline void set_bfs(struct i2s_dai *i2s, unsigned bfs)
{
	u32 mod = readl(i2s->addr + I2SMOD);
<<<<<<< HEAD
	int bfs_shift;
	int tdm = i2s->quirks & QUIRK_SUPPORTS_TDM;

	if (i2s->quirks & QUIRK_SUPPORTS_TDM) {
		bfs_shift = EXYNOS5420_MOD_BCLK_SHIFT;
		mod &= ~(EXYNOS5420_MOD_BCLK_MASK << bfs_shift);
	} else {
		bfs_shift = MOD_BCLK_SHIFT;
		mod &= ~(MOD_BCLK_MASK << bfs_shift);
	}
=======
	int tdm = i2s->quirks & QUIRK_SUPPORTS_TDM;
	int bfs_shift = i2s->variant_regs->bfs_off;
>>>>>>> v4.9.227

	/* Non-TDM I2S controllers do not support BCLK > 48 * FS */
	if (!tdm && bfs > 48) {
		dev_err(&i2s->pdev->dev, "Unsupported BCLK divider\n");
		return;
	}

<<<<<<< HEAD
=======
	mod &= ~(i2s->variant_regs->bfs_mask << bfs_shift);

>>>>>>> v4.9.227
	switch (bfs) {
	case 48:
		mod |= (MOD_BCLK_48FS << bfs_shift);
		break;
	case 32:
		mod |= (MOD_BCLK_32FS << bfs_shift);
		break;
	case 24:
		mod |= (MOD_BCLK_24FS << bfs_shift);
		break;
	case 16:
		mod |= (MOD_BCLK_16FS << bfs_shift);
		break;
	case 64:
		mod |= (EXYNOS5420_MOD_BCLK_64FS << bfs_shift);
		break;
	case 96:
		mod |= (EXYNOS5420_MOD_BCLK_96FS << bfs_shift);
		break;
	case 128:
		mod |= (EXYNOS5420_MOD_BCLK_128FS << bfs_shift);
		break;
	case 192:
		mod |= (EXYNOS5420_MOD_BCLK_192FS << bfs_shift);
		break;
	case 256:
		mod |= (EXYNOS5420_MOD_BCLK_256FS << bfs_shift);
		break;
	default:
		dev_err(&i2s->pdev->dev, "Wrong BCLK Divider!\n");
		return;
	}

	writel(mod, i2s->addr + I2SMOD);
}

/* Sample-Size */
static inline int get_blc(struct i2s_dai *i2s)
{
	int blc = readl(i2s->addr + I2SMOD);

	blc = (blc >> 13) & 0x3;

	switch (blc) {
	case 2: return 24;
	case 1:	return 8;
	default: return 16;
	}
}

/* TX Channel Control */
static void i2s_txctrl(struct i2s_dai *i2s, int on)
{
	void __iomem *addr = i2s->addr;
<<<<<<< HEAD
	u32 con = readl(addr + I2SCON);
	u32 mod = readl(addr + I2SMOD) & ~MOD_MASK;
=======
	int txr_off = i2s->variant_regs->txr_off;
	u32 con = readl(addr + I2SCON);
	u32 mod = readl(addr + I2SMOD) & ~(3 << txr_off);
>>>>>>> v4.9.227

	if (on) {
		con |= CON_ACTIVE;
		con &= ~CON_TXCH_PAUSE;

		if (is_secondary(i2s)) {
			con |= CON_TXSDMA_ACTIVE;
			con &= ~CON_TXSDMA_PAUSE;
		} else {
			con |= CON_TXDMA_ACTIVE;
			con &= ~CON_TXDMA_PAUSE;
		}

		if (any_rx_active(i2s))
<<<<<<< HEAD
			mod |= MOD_TXRX;
		else
			mod |= MOD_TXONLY;
=======
			mod |= 2 << txr_off;
		else
			mod |= 0 << txr_off;
>>>>>>> v4.9.227
	} else {
		if (is_secondary(i2s)) {
			con |=  CON_TXSDMA_PAUSE;
			con &= ~CON_TXSDMA_ACTIVE;
		} else {
			con |=  CON_TXDMA_PAUSE;
			con &= ~CON_TXDMA_ACTIVE;
		}

		if (other_tx_active(i2s)) {
			writel(con, addr + I2SCON);
			return;
		}

		con |=  CON_TXCH_PAUSE;

		if (any_rx_active(i2s))
<<<<<<< HEAD
			mod |= MOD_RXONLY;
=======
			mod |= 1 << txr_off;
>>>>>>> v4.9.227
		else
			con &= ~CON_ACTIVE;
	}

	writel(mod, addr + I2SMOD);
	writel(con, addr + I2SCON);
}

/* RX Channel Control */
static void i2s_rxctrl(struct i2s_dai *i2s, int on)
{
	void __iomem *addr = i2s->addr;
<<<<<<< HEAD
	u32 con = readl(addr + I2SCON);
	u32 mod = readl(addr + I2SMOD) & ~MOD_MASK;
=======
	int txr_off = i2s->variant_regs->txr_off;
	u32 con = readl(addr + I2SCON);
	u32 mod = readl(addr + I2SMOD) & ~(3 << txr_off);
>>>>>>> v4.9.227

	if (on) {
		con |= CON_RXDMA_ACTIVE | CON_ACTIVE;
		con &= ~(CON_RXDMA_PAUSE | CON_RXCH_PAUSE);

		if (any_tx_active(i2s))
<<<<<<< HEAD
			mod |= MOD_TXRX;
		else
			mod |= MOD_RXONLY;
=======
			mod |= 2 << txr_off;
		else
			mod |= 1 << txr_off;
>>>>>>> v4.9.227
	} else {
		con |=  CON_RXDMA_PAUSE | CON_RXCH_PAUSE;
		con &= ~CON_RXDMA_ACTIVE;

		if (any_tx_active(i2s))
<<<<<<< HEAD
			mod |= MOD_TXONLY;
=======
			mod |= 0 << txr_off;
>>>>>>> v4.9.227
		else
			con &= ~CON_ACTIVE;
	}

	writel(mod, addr + I2SMOD);
	writel(con, addr + I2SCON);
}

/* Flush FIFO of an interface */
static inline void i2s_fifo(struct i2s_dai *i2s, u32 flush)
{
	void __iomem *fic;
	u32 val;

	if (!i2s)
		return;

	if (is_secondary(i2s))
		fic = i2s->addr + I2SFICS;
	else
		fic = i2s->addr + I2SFIC;

	/* Flush the FIFO */
	writel(readl(fic) | flush, fic);

	/* Be patient */
	val = msecs_to_loops(1) / 1000; /* 1 usec */
	while (--val)
		cpu_relax();

	writel(readl(fic) & ~flush, fic);
}

static int i2s_set_sysclk(struct snd_soc_dai *dai,
	  int clk_id, unsigned int rfs, int dir)
{
	struct i2s_dai *i2s = to_info(dai);
<<<<<<< HEAD
	struct i2s_dai *other = i2s->pri_dai ? : i2s->sec_dai;
	u32 mod = readl(i2s->addr + I2SMOD);

	switch (clk_id) {
	case SAMSUNG_I2S_OPCLK:
		mod &= ~MOD_OPCLK_MASK;
		mod |= dir;
		break;
	case SAMSUNG_I2S_CDCLK:
=======
	struct i2s_dai *other = get_other_dai(i2s);
	const struct samsung_i2s_variant_regs *i2s_regs = i2s->variant_regs;
	unsigned int cdcon_mask = 1 << i2s_regs->cdclkcon_off;
	unsigned int rsrc_mask = 1 << i2s_regs->rclksrc_off;
	u32 mod, mask, val = 0;
	unsigned long flags;

	spin_lock_irqsave(i2s->lock, flags);
	mod = readl(i2s->addr + I2SMOD);
	spin_unlock_irqrestore(i2s->lock, flags);

	switch (clk_id) {
	case SAMSUNG_I2S_OPCLK:
		mask = MOD_OPCLK_MASK;
		val = dir;
		break;
	case SAMSUNG_I2S_CDCLK:
		mask = 1 << i2s_regs->cdclkcon_off;
>>>>>>> v4.9.227
		/* Shouldn't matter in GATING(CLOCK_IN) mode */
		if (dir == SND_SOC_CLOCK_IN)
			rfs = 0;

		if ((rfs && other && other->rfs && (other->rfs != rfs)) ||
				(any_active(i2s) &&
				(((dir == SND_SOC_CLOCK_IN)
<<<<<<< HEAD
					&& !(mod & MOD_CDCLKCON)) ||
				((dir == SND_SOC_CLOCK_OUT)
					&& (mod & MOD_CDCLKCON))))) {
=======
					&& !(mod & cdcon_mask)) ||
				((dir == SND_SOC_CLOCK_OUT)
					&& (mod & cdcon_mask))))) {
>>>>>>> v4.9.227
			dev_err(&i2s->pdev->dev,
				"%s:%d Other DAI busy\n", __func__, __LINE__);
			return -EAGAIN;
		}

		if (dir == SND_SOC_CLOCK_IN)
<<<<<<< HEAD
			mod |= MOD_CDCLKCON;
		else
			mod &= ~MOD_CDCLKCON;
=======
			val = 1 << i2s_regs->cdclkcon_off;
>>>>>>> v4.9.227

		i2s->rfs = rfs;
		break;

	case SAMSUNG_I2S_RCLKSRC_0: /* clock corrsponding to IISMOD[10] := 0 */
	case SAMSUNG_I2S_RCLKSRC_1: /* clock corrsponding to IISMOD[10] := 1 */
<<<<<<< HEAD
=======
		mask = 1 << i2s_regs->rclksrc_off;

>>>>>>> v4.9.227
		if ((i2s->quirks & QUIRK_NO_MUXPSR)
				|| (clk_id == SAMSUNG_I2S_RCLKSRC_0))
			clk_id = 0;
		else
			clk_id = 1;

		if (!any_active(i2s)) {
			if (i2s->op_clk && !IS_ERR(i2s->op_clk)) {
<<<<<<< HEAD
				if ((clk_id && !(mod & MOD_IMS_SYSMUX)) ||
					(!clk_id && (mod & MOD_IMS_SYSMUX))) {
=======
				if ((clk_id && !(mod & rsrc_mask)) ||
					(!clk_id && (mod & rsrc_mask))) {
>>>>>>> v4.9.227
					clk_disable_unprepare(i2s->op_clk);
					clk_put(i2s->op_clk);
				} else {
					i2s->rclk_srcrate =
						clk_get_rate(i2s->op_clk);
					return 0;
				}
			}

			if (clk_id)
				i2s->op_clk = clk_get(&i2s->pdev->dev,
						"i2s_opclk1");
			else
				i2s->op_clk = clk_get(&i2s->pdev->dev,
						"i2s_opclk0");

			if (WARN_ON(IS_ERR(i2s->op_clk)))
				return PTR_ERR(i2s->op_clk);

			clk_prepare_enable(i2s->op_clk);
			i2s->rclk_srcrate = clk_get_rate(i2s->op_clk);

			/* Over-ride the other's */
			if (other) {
				other->op_clk = i2s->op_clk;
				other->rclk_srcrate = i2s->rclk_srcrate;
			}
<<<<<<< HEAD
		} else if ((!clk_id && (mod & MOD_IMS_SYSMUX))
				|| (clk_id && !(mod & MOD_IMS_SYSMUX))) {
=======
		} else if ((!clk_id && (mod & rsrc_mask))
				|| (clk_id && !(mod & rsrc_mask))) {
>>>>>>> v4.9.227
			dev_err(&i2s->pdev->dev,
				"%s:%d Other DAI busy\n", __func__, __LINE__);
			return -EAGAIN;
		} else {
			/* Call can't be on the active DAI */
			i2s->op_clk = other->op_clk;
			i2s->rclk_srcrate = other->rclk_srcrate;
			return 0;
		}

<<<<<<< HEAD
		if (clk_id == 0)
			mod &= ~MOD_IMS_SYSMUX;
		else
			mod |= MOD_IMS_SYSMUX;
		break;

=======
		if (clk_id == 1)
			val = 1 << i2s_regs->rclksrc_off;
		break;
>>>>>>> v4.9.227
	default:
		dev_err(&i2s->pdev->dev, "We don't serve that!\n");
		return -EINVAL;
	}

<<<<<<< HEAD
	writel(mod, i2s->addr + I2SMOD);
=======
	spin_lock_irqsave(i2s->lock, flags);
	mod = readl(i2s->addr + I2SMOD);
	mod = (mod & ~mask) | val;
	writel(mod, i2s->addr + I2SMOD);
	spin_unlock_irqrestore(i2s->lock, flags);
>>>>>>> v4.9.227

	return 0;
}

static int i2s_set_fmt(struct snd_soc_dai *dai,
	unsigned int fmt)
{
	struct i2s_dai *i2s = to_info(dai);
<<<<<<< HEAD
	u32 mod = readl(i2s->addr + I2SMOD);
	int lrp_shift, sdf_shift, sdf_mask, lrp_rlow;
	u32 tmp = 0;

	if (i2s->quirks & QUIRK_SUPPORTS_TDM) {
		lrp_shift = EXYNOS5420_MOD_LRP_SHIFT;
		sdf_shift = EXYNOS5420_MOD_SDF_SHIFT;
	} else {
		lrp_shift = MOD_LRP_SHIFT;
		sdf_shift = MOD_SDF_SHIFT;
	}
=======
	int lrp_shift, sdf_shift, sdf_mask, lrp_rlow, mod_slave;
	u32 mod, tmp = 0;
	unsigned long flags;

	lrp_shift = i2s->variant_regs->lrp_off;
	sdf_shift = i2s->variant_regs->sdf_off;
	mod_slave = 1 << i2s->variant_regs->mss_off;
>>>>>>> v4.9.227

	sdf_mask = MOD_SDF_MASK << sdf_shift;
	lrp_rlow = MOD_LR_RLOW << lrp_shift;

	/* Format is priority */
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_RIGHT_J:
		tmp |= lrp_rlow;
		tmp |= (MOD_SDF_MSB << sdf_shift);
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		tmp |= lrp_rlow;
		tmp |= (MOD_SDF_LSB << sdf_shift);
		break;
	case SND_SOC_DAIFMT_I2S:
		tmp |= (MOD_SDF_IIS << sdf_shift);
		break;
	default:
		dev_err(&i2s->pdev->dev, "Format not supported\n");
		return -EINVAL;
	}

	/*
	 * INV flag is relative to the FORMAT flag - if set it simply
	 * flips the polarity specified by the Standard
	 */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_NB_IF:
		if (tmp & lrp_rlow)
			tmp &= ~lrp_rlow;
		else
			tmp |= lrp_rlow;
		break;
	default:
		dev_err(&i2s->pdev->dev, "Polarity not supported\n");
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
<<<<<<< HEAD
		tmp |= MOD_SLAVE;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		/* Set default source clock in Master mode */
		if (i2s->rclk_srcrate == 0)
=======
		tmp |= mod_slave;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		/*
		 * Set default source clock in Master mode, only when the
		 * CLK_I2S_RCLK_SRC clock is not exposed so we ensure any
		 * clock configuration assigned in DT is not overwritten.
		 */
		if (i2s->rclk_srcrate == 0 && i2s->clk_data.clks == NULL)
>>>>>>> v4.9.227
			i2s_set_sysclk(dai, SAMSUNG_I2S_RCLKSRC_0,
							0, SND_SOC_CLOCK_IN);
		break;
	default:
		dev_err(&i2s->pdev->dev, "master/slave format not supported\n");
		return -EINVAL;
	}

<<<<<<< HEAD
=======
	spin_lock_irqsave(i2s->lock, flags);
	mod = readl(i2s->addr + I2SMOD);
>>>>>>> v4.9.227
	/*
	 * Don't change the I2S mode if any controller is active on this
	 * channel.
	 */
	if (any_active(i2s) &&
<<<<<<< HEAD
		((mod & (sdf_mask | lrp_rlow | MOD_SLAVE)) != tmp)) {
=======
		((mod & (sdf_mask | lrp_rlow | mod_slave)) != tmp)) {
		spin_unlock_irqrestore(i2s->lock, flags);
>>>>>>> v4.9.227
		dev_err(&i2s->pdev->dev,
				"%s:%d Other DAI busy\n", __func__, __LINE__);
		return -EAGAIN;
	}

<<<<<<< HEAD
	mod &= ~(sdf_mask | lrp_rlow | MOD_SLAVE);
	mod |= tmp;
	writel(mod, i2s->addr + I2SMOD);
=======
	mod &= ~(sdf_mask | lrp_rlow | mod_slave);
	mod |= tmp;
	writel(mod, i2s->addr + I2SMOD);
	spin_unlock_irqrestore(i2s->lock, flags);
>>>>>>> v4.9.227

	return 0;
}

static int i2s_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	struct i2s_dai *i2s = to_info(dai);
<<<<<<< HEAD
	u32 mod = readl(i2s->addr + I2SMOD);

	if (!is_secondary(i2s))
		mod &= ~(MOD_DC2_EN | MOD_DC1_EN);

	switch (params_channels(params)) {
	case 6:
		mod |= MOD_DC2_EN;
	case 4:
		mod |= MOD_DC1_EN;
		break;
	case 2:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			i2s->dma_playback.dma_size = 4;
		else
			i2s->dma_capture.dma_size = 4;
		break;
	case 1:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			i2s->dma_playback.dma_size = 2;
		else
			i2s->dma_capture.dma_size = 2;
=======
	u32 mod, mask = 0, val = 0;
	unsigned long flags;

	if (!is_secondary(i2s))
		mask |= (MOD_DC2_EN | MOD_DC1_EN);

	switch (params_channels(params)) {
	case 6:
		val |= MOD_DC2_EN;
	case 4:
		val |= MOD_DC1_EN;
		break;
	case 2:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			i2s->dma_playback.addr_width = 4;
		else
			i2s->dma_capture.addr_width = 4;
		break;
	case 1:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			i2s->dma_playback.addr_width = 2;
		else
			i2s->dma_capture.addr_width = 2;
>>>>>>> v4.9.227

		break;
	default:
		dev_err(&i2s->pdev->dev, "%d channels not supported\n",
				params_channels(params));
		return -EINVAL;
	}

	if (is_secondary(i2s))
<<<<<<< HEAD
		mod &= ~MOD_BLCS_MASK;
	else
		mod &= ~MOD_BLCP_MASK;

	if (is_manager(i2s))
		mod &= ~MOD_BLC_MASK;
=======
		mask |= MOD_BLCS_MASK;
	else
		mask |= MOD_BLCP_MASK;

	if (is_manager(i2s))
		mask |= MOD_BLC_MASK;
>>>>>>> v4.9.227

	switch (params_width(params)) {
	case 8:
		if (is_secondary(i2s))
<<<<<<< HEAD
			mod |= MOD_BLCS_8BIT;
		else
			mod |= MOD_BLCP_8BIT;
		if (is_manager(i2s))
			mod |= MOD_BLC_8BIT;
		break;
	case 16:
		if (is_secondary(i2s))
			mod |= MOD_BLCS_16BIT;
		else
			mod |= MOD_BLCP_16BIT;
		if (is_manager(i2s))
			mod |= MOD_BLC_16BIT;
		break;
	case 24:
		if (is_secondary(i2s))
			mod |= MOD_BLCS_24BIT;
		else
			mod |= MOD_BLCP_24BIT;
		if (is_manager(i2s))
			mod |= MOD_BLC_24BIT;
=======
			val |= MOD_BLCS_8BIT;
		else
			val |= MOD_BLCP_8BIT;
		if (is_manager(i2s))
			val |= MOD_BLC_8BIT;
		break;
	case 16:
		if (is_secondary(i2s))
			val |= MOD_BLCS_16BIT;
		else
			val |= MOD_BLCP_16BIT;
		if (is_manager(i2s))
			val |= MOD_BLC_16BIT;
		break;
	case 24:
		if (is_secondary(i2s))
			val |= MOD_BLCS_24BIT;
		else
			val |= MOD_BLCP_24BIT;
		if (is_manager(i2s))
			val |= MOD_BLC_24BIT;
>>>>>>> v4.9.227
		break;
	default:
		dev_err(&i2s->pdev->dev, "Format(%d) not supported\n",
				params_format(params));
		return -EINVAL;
	}
<<<<<<< HEAD
	writel(mod, i2s->addr + I2SMOD);

	samsung_asoc_init_dma_data(dai, &i2s->dma_playback, &i2s->dma_capture);
=======

	spin_lock_irqsave(i2s->lock, flags);
	mod = readl(i2s->addr + I2SMOD);
	mod = (mod & ~mask) | val;
	writel(mod, i2s->addr + I2SMOD);
	spin_unlock_irqrestore(i2s->lock, flags);

	snd_soc_dai_init_dma_data(dai, &i2s->dma_playback, &i2s->dma_capture);
>>>>>>> v4.9.227

	i2s->frmclk = params_rate(params);

	return 0;
}

/* We set constraints on the substream acc to the version of I2S */
static int i2s_startup(struct snd_pcm_substream *substream,
	  struct snd_soc_dai *dai)
{
	struct i2s_dai *i2s = to_info(dai);
<<<<<<< HEAD
	struct i2s_dai *other = i2s->pri_dai ? : i2s->sec_dai;
=======
	struct i2s_dai *other = get_other_dai(i2s);
>>>>>>> v4.9.227
	unsigned long flags;

	spin_lock_irqsave(&lock, flags);

	i2s->mode |= DAI_OPENED;

	if (is_manager(other))
		i2s->mode &= ~DAI_MANAGER;
	else
		i2s->mode |= DAI_MANAGER;

	if (!any_active(i2s) && (i2s->quirks & QUIRK_NEED_RSTCLR))
		writel(CON_RSTCLR, i2s->addr + I2SCON);

	spin_unlock_irqrestore(&lock, flags);

<<<<<<< HEAD
	if (!is_opened(other) && i2s->cdclk_out)
		i2s_set_sysclk(dai, SAMSUNG_I2S_CDCLK,
				0, SND_SOC_CLOCK_OUT);
=======
>>>>>>> v4.9.227
	return 0;
}

static void i2s_shutdown(struct snd_pcm_substream *substream,
	struct snd_soc_dai *dai)
{
	struct i2s_dai *i2s = to_info(dai);
<<<<<<< HEAD
	struct i2s_dai *other = i2s->pri_dai ? : i2s->sec_dai;
=======
	struct i2s_dai *other = get_other_dai(i2s);
>>>>>>> v4.9.227
	unsigned long flags;

	spin_lock_irqsave(&lock, flags);

	i2s->mode &= ~DAI_OPENED;
	i2s->mode &= ~DAI_MANAGER;

<<<<<<< HEAD
	if (is_opened(other)) {
		other->mode |= DAI_MANAGER;
	} else {
		u32 mod = readl(i2s->addr + I2SMOD);
		i2s->cdclk_out = !(mod & MOD_CDCLKCON);
		if (other)
			other->cdclk_out = i2s->cdclk_out;
	}
=======
	if (is_opened(other))
		other->mode |= DAI_MANAGER;

>>>>>>> v4.9.227
	/* Reset any constraint on RFS and BFS */
	i2s->rfs = 0;
	i2s->bfs = 0;

	spin_unlock_irqrestore(&lock, flags);
<<<<<<< HEAD

	/* Gate CDCLK by default */
	if (!is_opened(other))
		i2s_set_sysclk(dai, SAMSUNG_I2S_CDCLK,
				0, SND_SOC_CLOCK_IN);
=======
>>>>>>> v4.9.227
}

static int config_setup(struct i2s_dai *i2s)
{
<<<<<<< HEAD
	struct i2s_dai *other = i2s->pri_dai ? : i2s->sec_dai;
=======
	struct i2s_dai *other = get_other_dai(i2s);
>>>>>>> v4.9.227
	unsigned rfs, bfs, blc;
	u32 psr;

	blc = get_blc(i2s);

	bfs = i2s->bfs;

	if (!bfs && other)
		bfs = other->bfs;

	/* Select least possible multiple(2) if no constraint set */
	if (!bfs)
		bfs = blc * 2;

	rfs = i2s->rfs;

	if (!rfs && other)
		rfs = other->rfs;

	if ((rfs == 256 || rfs == 512) && (blc == 24)) {
		dev_err(&i2s->pdev->dev,
			"%d-RFS not supported for 24-blc\n", rfs);
		return -EINVAL;
	}

	if (!rfs) {
		if (bfs == 16 || bfs == 32)
			rfs = 256;
		else
			rfs = 384;
	}

	/* If already setup and running */
	if (any_active(i2s) && (get_rfs(i2s) != rfs || get_bfs(i2s) != bfs)) {
		dev_err(&i2s->pdev->dev,
				"%s:%d Other DAI busy\n", __func__, __LINE__);
		return -EAGAIN;
	}

	set_bfs(i2s, bfs);
	set_rfs(i2s, rfs);

	/* Don't bother with PSR in Slave mode */
	if (is_slave(i2s))
		return 0;

	if (!(i2s->quirks & QUIRK_NO_MUXPSR)) {
<<<<<<< HEAD
=======
		struct clk *rclksrc = i2s->clk_table[CLK_I2S_RCLK_SRC];

		if (i2s->rclk_srcrate == 0 && rclksrc && !IS_ERR(rclksrc))
			i2s->rclk_srcrate = clk_get_rate(rclksrc);

>>>>>>> v4.9.227
		psr = i2s->rclk_srcrate / i2s->frmclk / rfs;
		writel(((psr - 1) << 8) | PSR_PSREN, i2s->addr + I2SPSR);
		dev_dbg(&i2s->pdev->dev,
			"RCLK_SRC=%luHz PSR=%u, RCLK=%dfs, BCLK=%dfs\n",
				i2s->rclk_srcrate, psr, rfs, bfs);
	}

	return 0;
}

static int i2s_trigger(struct snd_pcm_substream *substream,
	int cmd, struct snd_soc_dai *dai)
{
	int capture = (substream->stream == SNDRV_PCM_STREAM_CAPTURE);
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct i2s_dai *i2s = to_info(rtd->cpu_dai);
	unsigned long flags;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
<<<<<<< HEAD
		local_irq_save(flags);

		if (config_setup(i2s)) {
			local_irq_restore(flags);
=======
		spin_lock_irqsave(i2s->lock, flags);

		if (config_setup(i2s)) {
			spin_unlock_irqrestore(i2s->lock, flags);
>>>>>>> v4.9.227
			return -EINVAL;
		}

		if (capture)
			i2s_rxctrl(i2s, 1);
		else
			i2s_txctrl(i2s, 1);

<<<<<<< HEAD
		local_irq_restore(flags);
=======
		spin_unlock_irqrestore(i2s->lock, flags);
>>>>>>> v4.9.227
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
<<<<<<< HEAD
		local_irq_save(flags);
=======
		spin_lock_irqsave(i2s->lock, flags);
>>>>>>> v4.9.227

		if (capture) {
			i2s_rxctrl(i2s, 0);
			i2s_fifo(i2s, FIC_RXFLUSH);
		} else {
			i2s_txctrl(i2s, 0);
			i2s_fifo(i2s, FIC_TXFLUSH);
		}

<<<<<<< HEAD
		local_irq_restore(flags);
=======
		spin_unlock_irqrestore(i2s->lock, flags);
>>>>>>> v4.9.227
		break;
	}

	return 0;
}

static int i2s_set_clkdiv(struct snd_soc_dai *dai,
	int div_id, int div)
{
	struct i2s_dai *i2s = to_info(dai);
<<<<<<< HEAD
	struct i2s_dai *other = i2s->pri_dai ? : i2s->sec_dai;
=======
	struct i2s_dai *other = get_other_dai(i2s);
>>>>>>> v4.9.227

	switch (div_id) {
	case SAMSUNG_I2S_DIV_BCLK:
		if ((any_active(i2s) && div && (get_bfs(i2s) != div))
			|| (other && other->bfs && (other->bfs != div))) {
			dev_err(&i2s->pdev->dev,
				"%s:%d Other DAI busy\n", __func__, __LINE__);
			return -EAGAIN;
		}
		i2s->bfs = div;
		break;
	default:
		dev_err(&i2s->pdev->dev,
			"Invalid clock divider(%d)\n", div_id);
		return -EINVAL;
	}

	return 0;
}

static snd_pcm_sframes_t
i2s_delay(struct snd_pcm_substream *substream, struct snd_soc_dai *dai)
{
	struct i2s_dai *i2s = to_info(dai);
	u32 reg = readl(i2s->addr + I2SFIC);
	snd_pcm_sframes_t delay;
<<<<<<< HEAD
=======
	const struct samsung_i2s_variant_regs *i2s_regs = i2s->variant_regs;
>>>>>>> v4.9.227

	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
		delay = FIC_RXCOUNT(reg);
	else if (is_secondary(i2s))
		delay = FICS_TXCOUNT(readl(i2s->addr + I2SFICS));
	else
<<<<<<< HEAD
		delay = FIC_TXCOUNT(reg);
=======
		delay = (reg >> i2s_regs->ftx0cnt_off) & 0x7f;
>>>>>>> v4.9.227

	return delay;
}

#ifdef CONFIG_PM
static int i2s_suspend(struct snd_soc_dai *dai)
{
	struct i2s_dai *i2s = to_info(dai);

	i2s->suspend_i2smod = readl(i2s->addr + I2SMOD);
	i2s->suspend_i2scon = readl(i2s->addr + I2SCON);
	i2s->suspend_i2spsr = readl(i2s->addr + I2SPSR);

	return 0;
}

static int i2s_resume(struct snd_soc_dai *dai)
{
	struct i2s_dai *i2s = to_info(dai);

	writel(i2s->suspend_i2scon, i2s->addr + I2SCON);
	writel(i2s->suspend_i2smod, i2s->addr + I2SMOD);
	writel(i2s->suspend_i2spsr, i2s->addr + I2SPSR);

	return 0;
}
#else
#define i2s_suspend NULL
#define i2s_resume  NULL
#endif

static int samsung_i2s_dai_probe(struct snd_soc_dai *dai)
{
	struct i2s_dai *i2s = to_info(dai);
<<<<<<< HEAD
	struct i2s_dai *other = i2s->pri_dai ? : i2s->sec_dai;

	if (other && other->clk) { /* If this is probe on secondary */
		samsung_asoc_init_dma_data(dai, &other->sec_dai->dma_playback,
					   NULL);
		goto probe_exit;
	}

	i2s->addr = ioremap(i2s->base, 0x100);
	if (i2s->addr == NULL) {
		dev_err(&i2s->pdev->dev, "cannot ioremap registers\n");
		return -ENXIO;
	}

	i2s->clk = clk_get(&i2s->pdev->dev, "iis");
	if (IS_ERR(i2s->clk)) {
		dev_err(&i2s->pdev->dev, "failed to get i2s_clock\n");
		iounmap(i2s->addr);
		return -ENOENT;
	}
	clk_prepare_enable(i2s->clk);

	samsung_asoc_init_dma_data(dai, &i2s->dma_playback, &i2s->dma_capture);

	if (other) {
		other->addr = i2s->addr;
		other->clk = i2s->clk;
	}

	if (i2s->quirks & QUIRK_NEED_RSTCLR)
		writel(CON_RSTCLR, i2s->addr + I2SCON);

	if (i2s->quirks & QUIRK_SEC_DAI)
		idma_reg_addr_init(i2s->addr,
					i2s->sec_dai->idma_playback.dma_addr);

probe_exit:
=======
	struct i2s_dai *other = get_other_dai(i2s);
	unsigned long flags;

	if (is_secondary(i2s)) { /* If this is probe on the secondary DAI */
		snd_soc_dai_init_dma_data(dai, &other->sec_dai->dma_playback,
					   NULL);
	} else {
		snd_soc_dai_init_dma_data(dai, &i2s->dma_playback,
					   &i2s->dma_capture);

		if (i2s->quirks & QUIRK_NEED_RSTCLR)
			writel(CON_RSTCLR, i2s->addr + I2SCON);

		if (i2s->quirks & QUIRK_SUPPORTS_IDMA)
			idma_reg_addr_init(i2s->addr,
					i2s->sec_dai->idma_playback.addr);
	}

>>>>>>> v4.9.227
	/* Reset any constraint on RFS and BFS */
	i2s->rfs = 0;
	i2s->bfs = 0;
	i2s->rclk_srcrate = 0;
<<<<<<< HEAD
=======

	spin_lock_irqsave(i2s->lock, flags);
>>>>>>> v4.9.227
	i2s_txctrl(i2s, 0);
	i2s_rxctrl(i2s, 0);
	i2s_fifo(i2s, FIC_TXFLUSH);
	i2s_fifo(other, FIC_TXFLUSH);
	i2s_fifo(i2s, FIC_RXFLUSH);
<<<<<<< HEAD
=======
	spin_unlock_irqrestore(i2s->lock, flags);
>>>>>>> v4.9.227

	/* Gate CDCLK by default */
	if (!is_opened(other))
		i2s_set_sysclk(dai, SAMSUNG_I2S_CDCLK,
				0, SND_SOC_CLOCK_IN);

	return 0;
}

static int samsung_i2s_dai_remove(struct snd_soc_dai *dai)
{
	struct i2s_dai *i2s = snd_soc_dai_get_drvdata(dai);
<<<<<<< HEAD
	struct i2s_dai *other = i2s->pri_dai ? : i2s->sec_dai;

	if (!other || !other->clk) {

		if (i2s->quirks & QUIRK_NEED_RSTCLR)
			writel(0, i2s->addr + I2SCON);

		clk_disable_unprepare(i2s->clk);
		clk_put(i2s->clk);

		iounmap(i2s->addr);
	}

	i2s->clk = NULL;

=======
	unsigned long flags;

	if (!is_secondary(i2s)) {
		if (i2s->quirks & QUIRK_NEED_RSTCLR) {
			spin_lock_irqsave(i2s->lock, flags);
			writel(0, i2s->addr + I2SCON);
			spin_unlock_irqrestore(i2s->lock, flags);
		}
	}

>>>>>>> v4.9.227
	return 0;
}

static const struct snd_soc_dai_ops samsung_i2s_dai_ops = {
	.trigger = i2s_trigger,
	.hw_params = i2s_hw_params,
	.set_fmt = i2s_set_fmt,
	.set_clkdiv = i2s_set_clkdiv,
	.set_sysclk = i2s_set_sysclk,
	.startup = i2s_startup,
	.shutdown = i2s_shutdown,
	.delay = i2s_delay,
};

static const struct snd_soc_component_driver samsung_i2s_component = {
	.name		= "samsung-i2s",
};

#define SAMSUNG_I2S_RATES	SNDRV_PCM_RATE_8000_96000

#define SAMSUNG_I2S_FMTS	(SNDRV_PCM_FMTBIT_S8 | \
					SNDRV_PCM_FMTBIT_S16_LE | \
					SNDRV_PCM_FMTBIT_S24_LE)

static struct i2s_dai *i2s_alloc_dai(struct platform_device *pdev, bool sec)
{
	struct i2s_dai *i2s;
	int ret;

	i2s = devm_kzalloc(&pdev->dev, sizeof(struct i2s_dai), GFP_KERNEL);
	if (i2s == NULL)
		return NULL;

	i2s->pdev = pdev;
	i2s->pri_dai = NULL;
	i2s->sec_dai = NULL;
	i2s->i2s_dai_drv.symmetric_rates = 1;
	i2s->i2s_dai_drv.probe = samsung_i2s_dai_probe;
	i2s->i2s_dai_drv.remove = samsung_i2s_dai_remove;
	i2s->i2s_dai_drv.ops = &samsung_i2s_dai_ops;
	i2s->i2s_dai_drv.suspend = i2s_suspend;
	i2s->i2s_dai_drv.resume = i2s_resume;
	i2s->i2s_dai_drv.playback.channels_min = 1;
	i2s->i2s_dai_drv.playback.channels_max = 2;
	i2s->i2s_dai_drv.playback.rates = SAMSUNG_I2S_RATES;
	i2s->i2s_dai_drv.playback.formats = SAMSUNG_I2S_FMTS;

	if (!sec) {
		i2s->i2s_dai_drv.capture.channels_min = 1;
		i2s->i2s_dai_drv.capture.channels_max = 2;
		i2s->i2s_dai_drv.capture.rates = SAMSUNG_I2S_RATES;
		i2s->i2s_dai_drv.capture.formats = SAMSUNG_I2S_FMTS;
		dev_set_drvdata(&i2s->pdev->dev, i2s);
	} else {	/* Create a new platform_device for Secondary */
		i2s->pdev = platform_device_alloc("samsung-i2s-sec", -1);
		if (!i2s->pdev)
			return NULL;

		i2s->pdev->dev.parent = &pdev->dev;

		platform_set_drvdata(i2s->pdev, i2s);
		ret = platform_device_add(i2s->pdev);
		if (ret < 0)
			return NULL;
	}

	return i2s;
}

<<<<<<< HEAD
static const struct of_device_id exynos_i2s_match[];

static inline const struct samsung_i2s_dai_data *samsung_i2s_get_driver_data(
						struct platform_device *pdev)
{
#ifdef CONFIG_OF
	if (pdev->dev.of_node) {
		const struct of_device_id *match;
		match = of_match_node(exynos_i2s_match, pdev->dev.of_node);
		return match->data;
	} else
#endif
		return (struct samsung_i2s_dai_data *)
				platform_get_device_id(pdev)->driver_data;
}

#ifdef CONFIG_PM_RUNTIME
=======
static void i2s_free_sec_dai(struct i2s_dai *i2s)
{
	platform_device_del(i2s->pdev);
}

#ifdef CONFIG_PM
>>>>>>> v4.9.227
static int i2s_runtime_suspend(struct device *dev)
{
	struct i2s_dai *i2s = dev_get_drvdata(dev);

	clk_disable_unprepare(i2s->clk);

	return 0;
}

static int i2s_runtime_resume(struct device *dev)
{
	struct i2s_dai *i2s = dev_get_drvdata(dev);

	clk_prepare_enable(i2s->clk);

	return 0;
}
<<<<<<< HEAD
#endif /* CONFIG_PM_RUNTIME */
=======
#endif /* CONFIG_PM */

static void i2s_unregister_clocks(struct i2s_dai *i2s)
{
	int i;

	for (i = 0; i < i2s->clk_data.clk_num; i++) {
		if (!IS_ERR(i2s->clk_table[i]))
			clk_unregister(i2s->clk_table[i]);
	}
}

static void i2s_unregister_clock_provider(struct platform_device *pdev)
{
	struct i2s_dai *i2s = dev_get_drvdata(&pdev->dev);

	of_clk_del_provider(pdev->dev.of_node);
	i2s_unregister_clocks(i2s);
}

static int i2s_register_clock_provider(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct i2s_dai *i2s = dev_get_drvdata(dev);
	const char *clk_name[2] = { "i2s_opclk0", "i2s_opclk1" };
	const char *p_names[2] = { NULL };
	const struct samsung_i2s_variant_regs *reg_info = i2s->variant_regs;
	struct clk *rclksrc;
	int ret, i;

	/* Register the clock provider only if it's expected in the DTB */
	if (!of_find_property(dev->of_node, "#clock-cells", NULL))
		return 0;

	/* Get the RCLKSRC mux clock parent clock names */
	for (i = 0; i < ARRAY_SIZE(p_names); i++) {
		rclksrc = clk_get(dev, clk_name[i]);
		if (IS_ERR(rclksrc))
			continue;
		p_names[i] = __clk_get_name(rclksrc);
		clk_put(rclksrc);
	}

	if (!(i2s->quirks & QUIRK_NO_MUXPSR)) {
		/* Activate the prescaler */
		u32 val = readl(i2s->addr + I2SPSR);
		writel(val | PSR_PSREN, i2s->addr + I2SPSR);

		i2s->clk_table[CLK_I2S_RCLK_SRC] = clk_register_mux(NULL,
				"i2s_rclksrc", p_names, ARRAY_SIZE(p_names),
				CLK_SET_RATE_NO_REPARENT | CLK_SET_RATE_PARENT,
				i2s->addr + I2SMOD, reg_info->rclksrc_off,
				1, 0, i2s->lock);

		i2s->clk_table[CLK_I2S_RCLK_PSR] = clk_register_divider(NULL,
				"i2s_presc", "i2s_rclksrc",
				CLK_SET_RATE_PARENT,
				i2s->addr + I2SPSR, 8, 6, 0, i2s->lock);

		p_names[0] = "i2s_presc";
		i2s->clk_data.clk_num = 2;
	}
	of_property_read_string_index(dev->of_node,
				"clock-output-names", 0, &clk_name[0]);

	i2s->clk_table[CLK_I2S_CDCLK] = clk_register_gate(NULL, clk_name[0],
				p_names[0], CLK_SET_RATE_PARENT,
				i2s->addr + I2SMOD, reg_info->cdclkcon_off,
				CLK_GATE_SET_TO_DISABLE, i2s->lock);

	i2s->clk_data.clk_num += 1;
	i2s->clk_data.clks = i2s->clk_table;

	ret = of_clk_add_provider(dev->of_node, of_clk_src_onecell_get,
				  &i2s->clk_data);
	if (ret < 0) {
		dev_err(dev, "failed to add clock provider: %d\n", ret);
		i2s_unregister_clocks(i2s);
	}

	return ret;
}
>>>>>>> v4.9.227

static int samsung_i2s_probe(struct platform_device *pdev)
{
	struct i2s_dai *pri_dai, *sec_dai = NULL;
	struct s3c_audio_pdata *i2s_pdata = pdev->dev.platform_data;
	struct samsung_i2s *i2s_cfg = NULL;
	struct resource *res;
	u32 regs_base, quirks = 0, idma_addr = 0;
	struct device_node *np = pdev->dev.of_node;
	const struct samsung_i2s_dai_data *i2s_dai_data;
<<<<<<< HEAD
	int ret = 0;

	/* Call during Seconday interface registration */
	i2s_dai_data = samsung_i2s_get_driver_data(pdev);

=======
	int ret;

	if (IS_ENABLED(CONFIG_OF) && pdev->dev.of_node)
		i2s_dai_data = of_device_get_match_data(&pdev->dev);
	else
		i2s_dai_data = (struct samsung_i2s_dai_data *)
				platform_get_device_id(pdev)->driver_data;

	/* Call during the secondary interface registration */
>>>>>>> v4.9.227
	if (i2s_dai_data->dai_type == TYPE_SEC) {
		sec_dai = dev_get_drvdata(&pdev->dev);
		if (!sec_dai) {
			dev_err(&pdev->dev, "Unable to get drvdata\n");
			return -EFAULT;
		}
<<<<<<< HEAD
		devm_snd_soc_register_component(&sec_dai->pdev->dev,
						&samsung_i2s_component,
						&sec_dai->i2s_dai_drv, 1);
		samsung_asoc_dma_platform_register(&pdev->dev);
		return 0;
=======
		ret = samsung_asoc_dma_platform_register(&pdev->dev,
					sec_dai->filter, "tx-sec", NULL);
		if (ret != 0)
			return ret;

		return devm_snd_soc_register_component(&sec_dai->pdev->dev,
						&samsung_i2s_component,
						&sec_dai->i2s_dai_drv, 1);
>>>>>>> v4.9.227
	}

	pri_dai = i2s_alloc_dai(pdev, false);
	if (!pri_dai) {
		dev_err(&pdev->dev, "Unable to alloc I2S_pri\n");
		return -ENOMEM;
	}

<<<<<<< HEAD
=======
	spin_lock_init(&pri_dai->spinlock);
	pri_dai->lock = &pri_dai->spinlock;

>>>>>>> v4.9.227
	if (!np) {
		if (i2s_pdata == NULL) {
			dev_err(&pdev->dev, "Can't work without s3c_audio_pdata\n");
			return -EINVAL;
		}

<<<<<<< HEAD
		pri_dai->dma_playback.slave = i2s_pdata->dma_playback;
		pri_dai->dma_capture.slave = i2s_pdata->dma_capture;
=======
		pri_dai->dma_playback.filter_data = i2s_pdata->dma_playback;
		pri_dai->dma_capture.filter_data = i2s_pdata->dma_capture;
		pri_dai->filter = i2s_pdata->dma_filter;
>>>>>>> v4.9.227

		if (&i2s_pdata->type)
			i2s_cfg = &i2s_pdata->type.i2s;

		if (i2s_cfg) {
			quirks = i2s_cfg->quirks;
			idma_addr = i2s_cfg->idma_addr;
		}
	} else {
		quirks = i2s_dai_data->quirks;
		if (of_property_read_u32(np, "samsung,idma-addr",
					 &idma_addr)) {
<<<<<<< HEAD
			if (quirks & QUIRK_SEC_DAI) {
				dev_err(&pdev->dev, "idma address is not"\
						"specified");
				return -EINVAL;
=======
			if (quirks & QUIRK_SUPPORTS_IDMA) {
				dev_info(&pdev->dev, "idma address is not"\
						"specified");
>>>>>>> v4.9.227
			}
		}
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
<<<<<<< HEAD
	if (!res) {
		dev_err(&pdev->dev, "Unable to get I2S SFR address\n");
		return -ENXIO;
	}

	if (!request_mem_region(res->start, resource_size(res),
							"samsung-i2s")) {
		dev_err(&pdev->dev, "Unable to request SFR region\n");
		return -EBUSY;
	}
	regs_base = res->start;

	pri_dai->dma_playback.dma_addr = regs_base + I2STXD;
	pri_dai->dma_capture.dma_addr = regs_base + I2SRXD;
	pri_dai->dma_playback.ch_name = "tx";
	pri_dai->dma_capture.ch_name = "rx";
	pri_dai->dma_playback.dma_size = 4;
	pri_dai->dma_capture.dma_size = 4;
	pri_dai->base = regs_base;
	pri_dai->quirks = quirks;
=======
	pri_dai->addr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(pri_dai->addr))
		return PTR_ERR(pri_dai->addr);

	regs_base = res->start;

	pri_dai->clk = devm_clk_get(&pdev->dev, "iis");
	if (IS_ERR(pri_dai->clk)) {
		dev_err(&pdev->dev, "Failed to get iis clock\n");
		return PTR_ERR(pri_dai->clk);
	}

	ret = clk_prepare_enable(pri_dai->clk);
	if (ret != 0) {
		dev_err(&pdev->dev, "failed to enable clock: %d\n", ret);
		return ret;
	}
	pri_dai->dma_playback.addr = regs_base + I2STXD;
	pri_dai->dma_capture.addr = regs_base + I2SRXD;
	pri_dai->dma_playback.chan_name = "tx";
	pri_dai->dma_capture.chan_name = "rx";
	pri_dai->dma_playback.addr_width = 4;
	pri_dai->dma_capture.addr_width = 4;
	pri_dai->quirks = quirks;
	pri_dai->variant_regs = i2s_dai_data->i2s_variant_regs;
>>>>>>> v4.9.227

	if (quirks & QUIRK_PRI_6CHAN)
		pri_dai->i2s_dai_drv.playback.channels_max = 6;

<<<<<<< HEAD
=======
	ret = samsung_asoc_dma_platform_register(&pdev->dev, pri_dai->filter,
						 NULL, NULL);
	if (ret < 0)
		goto err_disable_clk;

>>>>>>> v4.9.227
	if (quirks & QUIRK_SEC_DAI) {
		sec_dai = i2s_alloc_dai(pdev, true);
		if (!sec_dai) {
			dev_err(&pdev->dev, "Unable to alloc I2S_sec\n");
			ret = -ENOMEM;
<<<<<<< HEAD
			goto err;
		}
		sec_dai->dma_playback.dma_addr = regs_base + I2STXDS;
		sec_dai->dma_playback.ch_name = "tx-sec";

		if (!np)
			sec_dai->dma_playback.slave = i2s_pdata->dma_play_sec;

		sec_dai->dma_playback.dma_size = 4;
		sec_dai->base = regs_base;
		sec_dai->quirks = quirks;
		sec_dai->idma_playback.dma_addr = idma_addr;
=======
			goto err_disable_clk;
		}

		sec_dai->lock = &pri_dai->spinlock;
		sec_dai->variant_regs = pri_dai->variant_regs;
		sec_dai->dma_playback.addr = regs_base + I2STXDS;
		sec_dai->dma_playback.chan_name = "tx-sec";

		if (!np) {
			sec_dai->dma_playback.filter_data = i2s_pdata->dma_play_sec;
			sec_dai->filter = i2s_pdata->dma_filter;
		}

		sec_dai->dma_playback.addr_width = 4;
		sec_dai->addr = pri_dai->addr;
		sec_dai->clk = pri_dai->clk;
		sec_dai->quirks = quirks;
		sec_dai->idma_playback.addr = idma_addr;
>>>>>>> v4.9.227
		sec_dai->pri_dai = pri_dai;
		pri_dai->sec_dai = sec_dai;
	}

	if (i2s_pdata && i2s_pdata->cfg_gpio && i2s_pdata->cfg_gpio(pdev)) {
		dev_err(&pdev->dev, "Unable to configure gpio\n");
		ret = -EINVAL;
<<<<<<< HEAD
		goto err;
	}

	devm_snd_soc_register_component(&pri_dai->pdev->dev,
					&samsung_i2s_component,
					&pri_dai->i2s_dai_drv, 1);

	pm_runtime_enable(&pdev->dev);

	samsung_asoc_dma_platform_register(&pdev->dev);

	return 0;
err:
	if (res)
		release_mem_region(regs_base, resource_size(res));

=======
		goto err_disable_clk;
	}

	ret = devm_snd_soc_register_component(&pri_dai->pdev->dev,
					&samsung_i2s_component,
					&pri_dai->i2s_dai_drv, 1);
	if (ret < 0)
		goto err_free_dai;


	pm_runtime_enable(&pdev->dev);

	ret = i2s_register_clock_provider(pdev);
	if (!ret)
		return 0;

	pm_runtime_disable(&pdev->dev);
err_free_dai:
	if (sec_dai)
		i2s_free_sec_dai(sec_dai);
err_disable_clk:
	clk_disable_unprepare(pri_dai->clk);
>>>>>>> v4.9.227
	return ret;
}

static int samsung_i2s_remove(struct platform_device *pdev)
{
	struct i2s_dai *i2s, *other;
<<<<<<< HEAD
	struct resource *res;

	i2s = dev_get_drvdata(&pdev->dev);
	other = i2s->pri_dai ? : i2s->sec_dai;
=======

	i2s = dev_get_drvdata(&pdev->dev);
	other = get_other_dai(i2s);
>>>>>>> v4.9.227

	if (other) {
		other->pri_dai = NULL;
		other->sec_dai = NULL;
	} else {
		pm_runtime_disable(&pdev->dev);
<<<<<<< HEAD
		res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		if (res)
			release_mem_region(res->start, resource_size(res));
=======
	}

	if (!is_secondary(i2s)) {
		i2s_unregister_clock_provider(pdev);
		clk_disable_unprepare(i2s->clk);
>>>>>>> v4.9.227
	}

	i2s->pri_dai = NULL;
	i2s->sec_dai = NULL;

	return 0;
}

<<<<<<< HEAD
static const struct samsung_i2s_dai_data i2sv3_dai_type = {
	.dai_type = TYPE_PRI,
	.quirks = QUIRK_NO_MUXPSR,
=======
static const struct samsung_i2s_variant_regs i2sv3_regs = {
	.bfs_off = 1,
	.rfs_off = 3,
	.sdf_off = 5,
	.txr_off = 8,
	.rclksrc_off = 10,
	.mss_off = 11,
	.cdclkcon_off = 12,
	.lrp_off = 7,
	.bfs_mask = 0x3,
	.rfs_mask = 0x3,
	.ftx0cnt_off = 8,
};

static const struct samsung_i2s_variant_regs i2sv6_regs = {
	.bfs_off = 0,
	.rfs_off = 4,
	.sdf_off = 6,
	.txr_off = 8,
	.rclksrc_off = 10,
	.mss_off = 11,
	.cdclkcon_off = 12,
	.lrp_off = 15,
	.bfs_mask = 0xf,
	.rfs_mask = 0x3,
	.ftx0cnt_off = 8,
};

static const struct samsung_i2s_variant_regs i2sv7_regs = {
	.bfs_off = 0,
	.rfs_off = 4,
	.sdf_off = 7,
	.txr_off = 9,
	.rclksrc_off = 11,
	.mss_off = 12,
	.cdclkcon_off = 22,
	.lrp_off = 15,
	.bfs_mask = 0xf,
	.rfs_mask = 0x7,
	.ftx0cnt_off = 0,
};

static const struct samsung_i2s_variant_regs i2sv5_i2s1_regs = {
	.bfs_off = 0,
	.rfs_off = 3,
	.sdf_off = 6,
	.txr_off = 8,
	.rclksrc_off = 10,
	.mss_off = 11,
	.cdclkcon_off = 12,
	.lrp_off = 15,
	.bfs_mask = 0x7,
	.rfs_mask = 0x7,
	.ftx0cnt_off = 8,
};

static const struct samsung_i2s_dai_data i2sv3_dai_type = {
	.dai_type = TYPE_PRI,
	.quirks = QUIRK_NO_MUXPSR,
	.i2s_variant_regs = &i2sv3_regs,
>>>>>>> v4.9.227
};

static const struct samsung_i2s_dai_data i2sv5_dai_type = {
	.dai_type = TYPE_PRI,
<<<<<<< HEAD
	.quirks = QUIRK_PRI_6CHAN | QUIRK_SEC_DAI | QUIRK_NEED_RSTCLR,
=======
	.quirks = QUIRK_PRI_6CHAN | QUIRK_SEC_DAI | QUIRK_NEED_RSTCLR |
			QUIRK_SUPPORTS_IDMA,
	.i2s_variant_regs = &i2sv3_regs,
>>>>>>> v4.9.227
};

static const struct samsung_i2s_dai_data i2sv6_dai_type = {
	.dai_type = TYPE_PRI,
	.quirks = QUIRK_PRI_6CHAN | QUIRK_SEC_DAI | QUIRK_NEED_RSTCLR |
<<<<<<< HEAD
			QUIRK_SUPPORTS_TDM,
};

static const struct samsung_i2s_dai_data samsung_dai_type_pri = {
	.dai_type = TYPE_PRI,
=======
			QUIRK_SUPPORTS_TDM | QUIRK_SUPPORTS_IDMA,
	.i2s_variant_regs = &i2sv6_regs,
};

static const struct samsung_i2s_dai_data i2sv7_dai_type = {
	.dai_type = TYPE_PRI,
	.quirks = QUIRK_PRI_6CHAN | QUIRK_SEC_DAI | QUIRK_NEED_RSTCLR |
			QUIRK_SUPPORTS_TDM,
	.i2s_variant_regs = &i2sv7_regs,
};

static const struct samsung_i2s_dai_data i2sv5_dai_type_i2s1 = {
	.dai_type = TYPE_PRI,
	.quirks = QUIRK_PRI_6CHAN | QUIRK_NEED_RSTCLR,
	.i2s_variant_regs = &i2sv5_i2s1_regs,
>>>>>>> v4.9.227
};

static const struct samsung_i2s_dai_data samsung_dai_type_sec = {
	.dai_type = TYPE_SEC,
};

<<<<<<< HEAD
static struct platform_device_id samsung_i2s_driver_ids[] = {
	{
		.name           = "samsung-i2s",
		.driver_data    = (kernel_ulong_t)&samsung_dai_type_pri,
=======
static const struct platform_device_id samsung_i2s_driver_ids[] = {
	{
		.name           = "samsung-i2s",
		.driver_data	= (kernel_ulong_t)&i2sv3_dai_type,
>>>>>>> v4.9.227
	}, {
		.name           = "samsung-i2s-sec",
		.driver_data    = (kernel_ulong_t)&samsung_dai_type_sec,
	},
	{},
};
MODULE_DEVICE_TABLE(platform, samsung_i2s_driver_ids);

#ifdef CONFIG_OF
static const struct of_device_id exynos_i2s_match[] = {
	{
		.compatible = "samsung,s3c6410-i2s",
		.data = &i2sv3_dai_type,
	}, {
		.compatible = "samsung,s5pv210-i2s",
		.data = &i2sv5_dai_type,
	}, {
		.compatible = "samsung,exynos5420-i2s",
		.data = &i2sv6_dai_type,
<<<<<<< HEAD
=======
	}, {
		.compatible = "samsung,exynos7-i2s",
		.data = &i2sv7_dai_type,
	}, {
		.compatible = "samsung,exynos7-i2s1",
		.data = &i2sv5_dai_type_i2s1,
>>>>>>> v4.9.227
	},
	{},
};
MODULE_DEVICE_TABLE(of, exynos_i2s_match);
#endif

static const struct dev_pm_ops samsung_i2s_pm = {
	SET_RUNTIME_PM_OPS(i2s_runtime_suspend,
				i2s_runtime_resume, NULL)
};

static struct platform_driver samsung_i2s_driver = {
	.probe  = samsung_i2s_probe,
	.remove = samsung_i2s_remove,
	.id_table = samsung_i2s_driver_ids,
	.driver = {
		.name = "samsung-i2s",
<<<<<<< HEAD
		.owner = THIS_MODULE,
=======
>>>>>>> v4.9.227
		.of_match_table = of_match_ptr(exynos_i2s_match),
		.pm = &samsung_i2s_pm,
	},
};

module_platform_driver(samsung_i2s_driver);

/* Module information */
MODULE_AUTHOR("Jaswinder Singh, <jassisinghbrar@gmail.com>");
MODULE_DESCRIPTION("Samsung I2S Interface");
MODULE_ALIAS("platform:samsung-i2s");
MODULE_LICENSE("GPL");
