/*
 * Freescale eSDHC controller driver.
 *
 * Copyright (c) 2007, 2010, 2012 Freescale Semiconductor, Inc.
 * Copyright (c) 2009 MontaVista Software, Inc.
 *
 * Authors: Xiaobo Xie <X.Xie@freescale.com>
 *	    Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */

#include <linux/err.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/mmc/host.h>
#include "sdhci-pltfm.h"
#include "sdhci-esdhc.h"

#define VENDOR_V_22	0x12
#define VENDOR_V_23	0x13
<<<<<<< HEAD
static u32 esdhc_readl(struct sdhci_host *host, int reg)
{
	u32 ret;

	ret = in_be32(host->ioaddr + reg);
=======

struct sdhci_esdhc {
	u8 vendor_ver;
	u8 spec_ver;
};

/**
 * esdhc_read*_fixup - Fixup the value read from incompatible eSDHC register
 *		       to make it compatible with SD spec.
 *
 * @host: pointer to sdhci_host
 * @spec_reg: SD spec register address
 * @value: 32bit eSDHC register value on spec_reg address
 *
 * In SD spec, there are 8/16/32/64 bits registers, while all of eSDHC
 * registers are 32 bits. There are differences in register size, register
 * address, register function, bit position and function between eSDHC spec
 * and SD spec.
 *
 * Return a fixed up register value
 */
static u32 esdhc_readl_fixup(struct sdhci_host *host,
				     int spec_reg, u32 value)
{
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_esdhc *esdhc = sdhci_pltfm_priv(pltfm_host);
	u32 ret;

>>>>>>> v4.9.227
	/*
	 * The bit of ADMA flag in eSDHC is not compatible with standard
	 * SDHC register, so set fake flag SDHCI_CAN_DO_ADMA2 when ADMA is
	 * supported by eSDHC.
	 * And for many FSL eSDHC controller, the reset value of field
<<<<<<< HEAD
	 * SDHCI_CAN_DO_ADMA1 is one, but some of them can't support ADMA,
	 * only these vendor version is greater than 2.2/0x12 support ADMA.
	 * For FSL eSDHC, must aligned 4-byte, so use 0xFC to read the
	 * the verdor version number, oxFE is SDHCI_HOST_VERSION.
	 */
	if ((reg == SDHCI_CAPABILITIES) && (ret & SDHCI_CAN_DO_ADMA1)) {
		u32 tmp = in_be32(host->ioaddr + SDHCI_SLOT_INT_STATUS);
		tmp = (tmp & SDHCI_VENDOR_VER_MASK) >> SDHCI_VENDOR_VER_SHIFT;
		if (tmp > VENDOR_V_22)
			ret |= SDHCI_CAN_DO_ADMA2;
	}

	return ret;
}

static u16 esdhc_readw(struct sdhci_host *host, int reg)
{
	u16 ret;
	int base = reg & ~0x3;
	int shift = (reg & 0x2) * 8;

	if (unlikely(reg == SDHCI_HOST_VERSION))
		ret = in_be32(host->ioaddr + base) & 0xffff;
	else
		ret = (in_be32(host->ioaddr + base) >> shift) & 0xffff;
	return ret;
}

static u8 esdhc_readb(struct sdhci_host *host, int reg)
{
	int base = reg & ~0x3;
	int shift = (reg & 0x3) * 8;
	u8 ret = (in_be32(host->ioaddr + base) >> shift) & 0xff;
=======
	 * SDHCI_CAN_DO_ADMA1 is 1, but some of them can't support ADMA,
	 * only these vendor version is greater than 2.2/0x12 support ADMA.
	 */
	if ((spec_reg == SDHCI_CAPABILITIES) && (value & SDHCI_CAN_DO_ADMA1)) {
		if (esdhc->vendor_ver > VENDOR_V_22) {
			ret = value | SDHCI_CAN_DO_ADMA2;
			return ret;
		}
	}
	/*
	 * The DAT[3:0] line signal levels and the CMD line signal level are
	 * not compatible with standard SDHC register. The line signal levels
	 * DAT[7:0] are at bits 31:24 and the command line signal level is at
	 * bit 23. All other bits are the same as in the standard SDHC
	 * register.
	 */
	if (spec_reg == SDHCI_PRESENT_STATE) {
		ret = value & 0x000fffff;
		ret |= (value >> 4) & SDHCI_DATA_LVL_MASK;
		ret |= (value << 1) & SDHCI_CMD_LVL;
		return ret;
	}

	ret = value;
	return ret;
}

static u16 esdhc_readw_fixup(struct sdhci_host *host,
				     int spec_reg, u32 value)
{
	u16 ret;
	int shift = (spec_reg & 0x2) * 8;

	if (spec_reg == SDHCI_HOST_VERSION)
		ret = value & 0xffff;
	else
		ret = (value >> shift) & 0xffff;
	return ret;
}

static u8 esdhc_readb_fixup(struct sdhci_host *host,
				     int spec_reg, u32 value)
{
	u8 ret;
	u8 dma_bits;
	int shift = (spec_reg & 0x3) * 8;

	ret = (value >> shift) & 0xff;
>>>>>>> v4.9.227

	/*
	 * "DMA select" locates at offset 0x28 in SD specification, but on
	 * P5020 or P3041, it locates at 0x29.
	 */
<<<<<<< HEAD
	if (reg == SDHCI_HOST_CONTROL) {
		u32 dma_bits;

		dma_bits = in_be32(host->ioaddr + reg);
		/* DMA select is 22,23 bits in Protocol Control Register */
		dma_bits = (dma_bits >> 5) & SDHCI_CTRL_DMA_MASK;

=======
	if (spec_reg == SDHCI_HOST_CONTROL) {
		/* DMA select is 22,23 bits in Protocol Control Register */
		dma_bits = (value >> 5) & SDHCI_CTRL_DMA_MASK;
>>>>>>> v4.9.227
		/* fixup the result */
		ret &= ~SDHCI_CTRL_DMA_MASK;
		ret |= dma_bits;
	}
<<<<<<< HEAD
=======
	return ret;
}

/**
 * esdhc_write*_fixup - Fixup the SD spec register value so that it could be
 *			written into eSDHC register.
 *
 * @host: pointer to sdhci_host
 * @spec_reg: SD spec register address
 * @value: 8/16/32bit SD spec register value that would be written
 * @old_value: 32bit eSDHC register value on spec_reg address
 *
 * In SD spec, there are 8/16/32/64 bits registers, while all of eSDHC
 * registers are 32 bits. There are differences in register size, register
 * address, register function, bit position and function between eSDHC spec
 * and SD spec.
 *
 * Return a fixed up register value
 */
static u32 esdhc_writel_fixup(struct sdhci_host *host,
				     int spec_reg, u32 value, u32 old_value)
{
	u32 ret;

	/*
	 * Enabling IRQSTATEN[BGESEN] is just to set IRQSTAT[BGE]
	 * when SYSCTL[RSTD] is set for some special operations.
	 * No any impact on other operation.
	 */
	if (spec_reg == SDHCI_INT_ENABLE)
		ret = value | SDHCI_INT_BLK_GAP;
	else
		ret = value;
>>>>>>> v4.9.227

	return ret;
}

<<<<<<< HEAD
static void esdhc_writel(struct sdhci_host *host, u32 val, int reg)
{
	/*
	 * Enable IRQSTATEN[BGESEN] is just to set IRQSTAT[BGE]
	 * when SYSCTL[RSTD]) is set for some special operations.
	 * No any impact other operation.
	 */
	if (reg == SDHCI_INT_ENABLE)
		val |= SDHCI_INT_BLK_GAP;
	sdhci_be32bs_writel(host, val, reg);
}

static void esdhc_writew(struct sdhci_host *host, u16 val, int reg)
{
	if (reg == SDHCI_BLOCK_SIZE) {
=======
static u32 esdhc_writew_fixup(struct sdhci_host *host,
				     int spec_reg, u16 value, u32 old_value)
{
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	int shift = (spec_reg & 0x2) * 8;
	u32 ret;

	switch (spec_reg) {
	case SDHCI_TRANSFER_MODE:
		/*
		 * Postpone this write, we must do it together with a
		 * command write that is down below. Return old value.
		 */
		pltfm_host->xfer_mode_shadow = value;
		return old_value;
	case SDHCI_COMMAND:
		ret = (value << 16) | pltfm_host->xfer_mode_shadow;
		return ret;
	}

	ret = old_value & (~(0xffff << shift));
	ret |= (value << shift);

	if (spec_reg == SDHCI_BLOCK_SIZE) {
>>>>>>> v4.9.227
		/*
		 * Two last DMA bits are reserved, and first one is used for
		 * non-standard blksz of 4096 bytes that we don't support
		 * yet. So clear the DMA boundary bits.
		 */
<<<<<<< HEAD
		val &= ~SDHCI_MAKE_BLKSZ(0x7, 0);
	}
	sdhci_be32bs_writew(host, val, reg);
}

static void esdhc_writeb(struct sdhci_host *host, u8 val, int reg)
{
=======
		ret &= (~SDHCI_MAKE_BLKSZ(0x7, 0));
	}
	return ret;
}

static u32 esdhc_writeb_fixup(struct sdhci_host *host,
				     int spec_reg, u8 value, u32 old_value)
{
	u32 ret;
	u32 dma_bits;
	u8 tmp;
	int shift = (spec_reg & 0x3) * 8;

	/*
	 * eSDHC doesn't have a standard power control register, so we do
	 * nothing here to avoid incorrect operation.
	 */
	if (spec_reg == SDHCI_POWER_CONTROL)
		return old_value;
>>>>>>> v4.9.227
	/*
	 * "DMA select" location is offset 0x28 in SD specification, but on
	 * P5020 or P3041, it's located at 0x29.
	 */
<<<<<<< HEAD
	if (reg == SDHCI_HOST_CONTROL) {
		u32 dma_bits;

=======
	if (spec_reg == SDHCI_HOST_CONTROL) {
>>>>>>> v4.9.227
		/*
		 * If host control register is not standard, exit
		 * this function
		 */
		if (host->quirks2 & SDHCI_QUIRK2_BROKEN_HOST_CONTROL)
<<<<<<< HEAD
			return;

		/* DMA select is 22,23 bits in Protocol Control Register */
		dma_bits = (val & SDHCI_CTRL_DMA_MASK) << 5;
		clrsetbits_be32(host->ioaddr + reg , SDHCI_CTRL_DMA_MASK << 5,
			dma_bits);
		val &= ~SDHCI_CTRL_DMA_MASK;
		val |= in_be32(host->ioaddr + reg) & SDHCI_CTRL_DMA_MASK;
	}

	/* Prevent SDHCI core from writing reserved bits (e.g. HISPD). */
	if (reg == SDHCI_HOST_CONTROL)
		val &= ~ESDHC_HOST_CONTROL_RES;
	sdhci_be32bs_writeb(host, val, reg);
=======
			return old_value;

		/* DMA select is 22,23 bits in Protocol Control Register */
		dma_bits = (value & SDHCI_CTRL_DMA_MASK) << 5;
		ret = (old_value & (~(SDHCI_CTRL_DMA_MASK << 5))) | dma_bits;
		tmp = (value & (~SDHCI_CTRL_DMA_MASK)) |
		      (old_value & SDHCI_CTRL_DMA_MASK);
		ret = (ret & (~0xff)) | tmp;

		/* Prevent SDHCI core from writing reserved bits (e.g. HISPD) */
		ret &= ~ESDHC_HOST_CONTROL_RES;
		return ret;
	}

	ret = (old_value & (~(0xff << shift))) | (value << shift);
	return ret;
}

static u32 esdhc_be_readl(struct sdhci_host *host, int reg)
{
	u32 ret;
	u32 value;

	value = ioread32be(host->ioaddr + reg);
	ret = esdhc_readl_fixup(host, reg, value);

	return ret;
}

static u32 esdhc_le_readl(struct sdhci_host *host, int reg)
{
	u32 ret;
	u32 value;

	value = ioread32(host->ioaddr + reg);
	ret = esdhc_readl_fixup(host, reg, value);

	return ret;
}

static u16 esdhc_be_readw(struct sdhci_host *host, int reg)
{
	u16 ret;
	u32 value;
	int base = reg & ~0x3;

	value = ioread32be(host->ioaddr + base);
	ret = esdhc_readw_fixup(host, reg, value);
	return ret;
}

static u16 esdhc_le_readw(struct sdhci_host *host, int reg)
{
	u16 ret;
	u32 value;
	int base = reg & ~0x3;

	value = ioread32(host->ioaddr + base);
	ret = esdhc_readw_fixup(host, reg, value);
	return ret;
}

static u8 esdhc_be_readb(struct sdhci_host *host, int reg)
{
	u8 ret;
	u32 value;
	int base = reg & ~0x3;

	value = ioread32be(host->ioaddr + base);
	ret = esdhc_readb_fixup(host, reg, value);
	return ret;
}

static u8 esdhc_le_readb(struct sdhci_host *host, int reg)
{
	u8 ret;
	u32 value;
	int base = reg & ~0x3;

	value = ioread32(host->ioaddr + base);
	ret = esdhc_readb_fixup(host, reg, value);
	return ret;
}

static void esdhc_be_writel(struct sdhci_host *host, u32 val, int reg)
{
	u32 value;

	value = esdhc_writel_fixup(host, reg, val, 0);
	iowrite32be(value, host->ioaddr + reg);
}

static void esdhc_le_writel(struct sdhci_host *host, u32 val, int reg)
{
	u32 value;

	value = esdhc_writel_fixup(host, reg, val, 0);
	iowrite32(value, host->ioaddr + reg);
}

static void esdhc_be_writew(struct sdhci_host *host, u16 val, int reg)
{
	int base = reg & ~0x3;
	u32 value;
	u32 ret;

	value = ioread32be(host->ioaddr + base);
	ret = esdhc_writew_fixup(host, reg, val, value);
	if (reg != SDHCI_TRANSFER_MODE)
		iowrite32be(ret, host->ioaddr + base);
}

static void esdhc_le_writew(struct sdhci_host *host, u16 val, int reg)
{
	int base = reg & ~0x3;
	u32 value;
	u32 ret;

	value = ioread32(host->ioaddr + base);
	ret = esdhc_writew_fixup(host, reg, val, value);
	if (reg != SDHCI_TRANSFER_MODE)
		iowrite32(ret, host->ioaddr + base);
}

static void esdhc_be_writeb(struct sdhci_host *host, u8 val, int reg)
{
	int base = reg & ~0x3;
	u32 value;
	u32 ret;

	value = ioread32be(host->ioaddr + base);
	ret = esdhc_writeb_fixup(host, reg, val, value);
	iowrite32be(ret, host->ioaddr + base);
}

static void esdhc_le_writeb(struct sdhci_host *host, u8 val, int reg)
{
	int base = reg & ~0x3;
	u32 value;
	u32 ret;

	value = ioread32(host->ioaddr + base);
	ret = esdhc_writeb_fixup(host, reg, val, value);
	iowrite32(ret, host->ioaddr + base);
>>>>>>> v4.9.227
}

/*
 * For Abort or Suspend after Stop at Block Gap, ignore the ADMA
 * error(IRQSTAT[ADMAE]) if both Transfer Complete(IRQSTAT[TC])
 * and Block Gap Event(IRQSTAT[BGE]) are also set.
 * For Continue, apply soft reset for data(SYSCTL[RSTD]);
 * and re-issue the entire read transaction from beginning.
 */
<<<<<<< HEAD
static void esdhci_of_adma_workaround(struct sdhci_host *host, u32 intmask)
{
	u32 tmp;
=======
static void esdhc_of_adma_workaround(struct sdhci_host *host, u32 intmask)
{
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_esdhc *esdhc = sdhci_pltfm_priv(pltfm_host);
>>>>>>> v4.9.227
	bool applicable;
	dma_addr_t dmastart;
	dma_addr_t dmanow;

<<<<<<< HEAD
	tmp = in_be32(host->ioaddr + SDHCI_SLOT_INT_STATUS);
	tmp = (tmp & SDHCI_VENDOR_VER_MASK) >> SDHCI_VENDOR_VER_SHIFT;

	applicable = (intmask & SDHCI_INT_DATA_END) &&
		(intmask & SDHCI_INT_BLK_GAP) &&
		(tmp == VENDOR_V_23);
=======
	applicable = (intmask & SDHCI_INT_DATA_END) &&
		     (intmask & SDHCI_INT_BLK_GAP) &&
		     (esdhc->vendor_ver == VENDOR_V_23);
>>>>>>> v4.9.227
	if (!applicable)
		return;

	host->data->error = 0;
	dmastart = sg_dma_address(host->data->sg);
	dmanow = dmastart + host->data->bytes_xfered;
	/*
	 * Force update to the next DMA block boundary.
	 */
	dmanow = (dmanow & ~(SDHCI_DEFAULT_BOUNDARY_SIZE - 1)) +
		SDHCI_DEFAULT_BOUNDARY_SIZE;
	host->data->bytes_xfered = dmanow - dmastart;
	sdhci_writel(host, dmanow, SDHCI_DMA_ADDRESS);
}

static int esdhc_of_enable_dma(struct sdhci_host *host)
{
<<<<<<< HEAD
	setbits32(host->ioaddr + ESDHC_DMA_SYSCTL, ESDHC_DMA_SNOOP);
=======
	u32 value;

	value = sdhci_readl(host, ESDHC_DMA_SYSCTL);
	value |= ESDHC_DMA_SNOOP;
	sdhci_writel(host, value, ESDHC_DMA_SYSCTL);
>>>>>>> v4.9.227
	return 0;
}

static unsigned int esdhc_of_get_max_clock(struct sdhci_host *host)
{
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);

	return pltfm_host->clock;
}

static unsigned int esdhc_of_get_min_clock(struct sdhci_host *host)
{
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);

	return pltfm_host->clock / 256 / 16;
}

static void esdhc_of_set_clock(struct sdhci_host *host, unsigned int clock)
{
<<<<<<< HEAD
	int pre_div = 2;
=======
	struct sdhci_pltfm_host *pltfm_host = sdhci_priv(host);
	struct sdhci_esdhc *esdhc = sdhci_pltfm_priv(pltfm_host);
	int pre_div = 1;
>>>>>>> v4.9.227
	int div = 1;
	u32 temp;

	host->mmc->actual_clock = 0;

	if (clock == 0)
		return;

<<<<<<< HEAD
=======
	/* Workaround to start pre_div at 2 for VNN < VENDOR_V_23 */
	if (esdhc->vendor_ver < VENDOR_V_23)
		pre_div = 2;

	/*
	 * Limit SD clock to 167MHz for ls1046a according to its datasheet
	 */
	if (clock > 167000000 &&
	    of_find_compatible_node(NULL, NULL, "fsl,ls1046a-esdhc"))
		clock = 167000000;

	/*
	 * Limit SD clock to 125MHz for ls1012a according to its datasheet
	 */
	if (clock > 125000000 &&
	    of_find_compatible_node(NULL, NULL, "fsl,ls1012a-esdhc"))
		clock = 125000000;

>>>>>>> v4.9.227
	/* Workaround to reduce the clock frequency for p1010 esdhc */
	if (of_find_compatible_node(NULL, NULL, "fsl,p1010-esdhc")) {
		if (clock > 20000000)
			clock -= 5000000;
		if (clock > 40000000)
			clock -= 5000000;
	}

	temp = sdhci_readl(host, ESDHC_SYSTEM_CONTROL);
	temp &= ~(ESDHC_CLOCK_IPGEN | ESDHC_CLOCK_HCKEN | ESDHC_CLOCK_PEREN
		| ESDHC_CLOCK_MASK);
	sdhci_writel(host, temp, ESDHC_SYSTEM_CONTROL);

	while (host->max_clk / pre_div / 16 > clock && pre_div < 256)
		pre_div *= 2;

	while (host->max_clk / pre_div / div > clock && div < 16)
		div++;

	dev_dbg(mmc_dev(host->mmc), "desired SD clock: %d, actual: %d\n",
		clock, host->max_clk / pre_div / div);
<<<<<<< HEAD

=======
	host->mmc->actual_clock = host->max_clk / pre_div / div;
>>>>>>> v4.9.227
	pre_div >>= 1;
	div--;

	temp = sdhci_readl(host, ESDHC_SYSTEM_CONTROL);
	temp |= (ESDHC_CLOCK_IPGEN | ESDHC_CLOCK_HCKEN | ESDHC_CLOCK_PEREN
		| (div << ESDHC_DIVIDER_SHIFT)
		| (pre_div << ESDHC_PREDIV_SHIFT));
	sdhci_writel(host, temp, ESDHC_SYSTEM_CONTROL);
	mdelay(1);
}

<<<<<<< HEAD
static void esdhc_of_platform_init(struct sdhci_host *host)
{
	u32 vvn;

	vvn = in_be32(host->ioaddr + SDHCI_SLOT_INT_STATUS);
	vvn = (vvn & SDHCI_VENDOR_VER_MASK) >> SDHCI_VENDOR_VER_SHIFT;
	if (vvn == VENDOR_V_22)
		host->quirks2 |= SDHCI_QUIRK2_HOST_NO_CMD23;

	if (vvn > VENDOR_V_22)
		host->quirks &= ~SDHCI_QUIRK_NO_BUSY_IRQ;
}

=======
>>>>>>> v4.9.227
static void esdhc_pltfm_set_bus_width(struct sdhci_host *host, int width)
{
	u32 ctrl;

<<<<<<< HEAD
	switch (width) {
	case MMC_BUS_WIDTH_8:
		ctrl = ESDHC_CTRL_8BITBUS;
		break;

	case MMC_BUS_WIDTH_4:
		ctrl = ESDHC_CTRL_4BITBUS;
		break;

	default:
		ctrl = 0;
		break;
	}

	clrsetbits_be32(host->ioaddr + SDHCI_HOST_CONTROL,
			ESDHC_CTRL_BUSWIDTH_MASK, ctrl);
}

static const struct sdhci_ops sdhci_esdhc_ops = {
	.read_l = esdhc_readl,
	.read_w = esdhc_readw,
	.read_b = esdhc_readb,
	.write_l = esdhc_writel,
	.write_w = esdhc_writew,
	.write_b = esdhc_writeb,
	.set_clock = esdhc_of_set_clock,
	.enable_dma = esdhc_of_enable_dma,
	.get_max_clock = esdhc_of_get_max_clock,
	.get_min_clock = esdhc_of_get_min_clock,
	.platform_init = esdhc_of_platform_init,
	.adma_workaround = esdhci_of_adma_workaround,
	.set_bus_width = esdhc_pltfm_set_bus_width,
	.reset = sdhci_reset,
	.set_uhs_signaling = sdhci_set_uhs_signaling,
};

#ifdef CONFIG_PM

=======
	ctrl = sdhci_readl(host, ESDHC_PROCTL);
	ctrl &= (~ESDHC_CTRL_BUSWIDTH_MASK);
	switch (width) {
	case MMC_BUS_WIDTH_8:
		ctrl |= ESDHC_CTRL_8BITBUS;
		break;

	case MMC_BUS_WIDTH_4:
		ctrl |= ESDHC_CTRL_4BITBUS;
		break;

	default:
		break;
	}

	sdhci_writel(host, ctrl, ESDHC_PROCTL);
}

static void esdhc_reset(struct sdhci_host *host, u8 mask)
{
	sdhci_reset(host, mask);

	sdhci_writel(host, host->ier, SDHCI_INT_ENABLE);
	sdhci_writel(host, host->ier, SDHCI_SIGNAL_ENABLE);
}

#ifdef CONFIG_PM_SLEEP
>>>>>>> v4.9.227
static u32 esdhc_proctl;
static int esdhc_of_suspend(struct device *dev)
{
	struct sdhci_host *host = dev_get_drvdata(dev);

<<<<<<< HEAD
	esdhc_proctl = sdhci_be32bs_readl(host, SDHCI_HOST_CONTROL);
=======
	esdhc_proctl = sdhci_readl(host, SDHCI_HOST_CONTROL);
>>>>>>> v4.9.227

	return sdhci_suspend_host(host);
}

static int esdhc_of_resume(struct device *dev)
{
	struct sdhci_host *host = dev_get_drvdata(dev);
	int ret = sdhci_resume_host(host);

	if (ret == 0) {
		/* Isn't this already done by sdhci_resume_host() ? --rmk */
		esdhc_of_enable_dma(host);
<<<<<<< HEAD
		sdhci_be32bs_writel(host, esdhc_proctl, SDHCI_HOST_CONTROL);
	}

	return ret;
}

static const struct dev_pm_ops esdhc_pmops = {
	.suspend	= esdhc_of_suspend,
	.resume		= esdhc_of_resume,
};
#define ESDHC_PMOPS (&esdhc_pmops)
#else
#define ESDHC_PMOPS NULL
#endif

static const struct sdhci_pltfm_data sdhci_esdhc_pdata = {
	/*
	 * card detection could be handled via GPIO
	 * eSDHC cannot support End Attribute in NOP ADMA descriptor
	 */
	.quirks = ESDHC_DEFAULT_QUIRKS | SDHCI_QUIRK_BROKEN_CARD_DETECTION
		| SDHCI_QUIRK_NO_CARD_NO_RESET
		| SDHCI_QUIRK_NO_ENDATTR_IN_NOPDESC,
	.ops = &sdhci_esdhc_ops,
};

=======
		sdhci_writel(host, esdhc_proctl, SDHCI_HOST_CONTROL);
	}
	return ret;
}
#endif

static SIMPLE_DEV_PM_OPS(esdhc_of_dev_pm_ops,
			esdhc_of_suspend,
			esdhc_of_resume);

static const struct sdhci_ops sdhci_esdhc_be_ops = {
	.read_l = esdhc_be_readl,
	.read_w = esdhc_be_readw,
	.read_b = esdhc_be_readb,
	.write_l = esdhc_be_writel,
	.write_w = esdhc_be_writew,
	.write_b = esdhc_be_writeb,
	.set_clock = esdhc_of_set_clock,
	.enable_dma = esdhc_of_enable_dma,
	.get_max_clock = esdhc_of_get_max_clock,
	.get_min_clock = esdhc_of_get_min_clock,
	.adma_workaround = esdhc_of_adma_workaround,
	.set_bus_width = esdhc_pltfm_set_bus_width,
	.reset = esdhc_reset,
	.set_uhs_signaling = sdhci_set_uhs_signaling,
};

static const struct sdhci_ops sdhci_esdhc_le_ops = {
	.read_l = esdhc_le_readl,
	.read_w = esdhc_le_readw,
	.read_b = esdhc_le_readb,
	.write_l = esdhc_le_writel,
	.write_w = esdhc_le_writew,
	.write_b = esdhc_le_writeb,
	.set_clock = esdhc_of_set_clock,
	.enable_dma = esdhc_of_enable_dma,
	.get_max_clock = esdhc_of_get_max_clock,
	.get_min_clock = esdhc_of_get_min_clock,
	.adma_workaround = esdhc_of_adma_workaround,
	.set_bus_width = esdhc_pltfm_set_bus_width,
	.reset = esdhc_reset,
	.set_uhs_signaling = sdhci_set_uhs_signaling,
};

static const struct sdhci_pltfm_data sdhci_esdhc_be_pdata = {
	.quirks = ESDHC_DEFAULT_QUIRKS |
#ifdef CONFIG_PPC
		  SDHCI_QUIRK_BROKEN_CARD_DETECTION |
#endif
		  SDHCI_QUIRK_NO_CARD_NO_RESET |
		  SDHCI_QUIRK_NO_ENDATTR_IN_NOPDESC,
	.ops = &sdhci_esdhc_be_ops,
};

static const struct sdhci_pltfm_data sdhci_esdhc_le_pdata = {
	.quirks = ESDHC_DEFAULT_QUIRKS |
		  SDHCI_QUIRK_NO_CARD_NO_RESET |
		  SDHCI_QUIRK_NO_ENDATTR_IN_NOPDESC,
	.ops = &sdhci_esdhc_le_ops,
};

static void esdhc_init(struct platform_device *pdev, struct sdhci_host *host)
{
	struct sdhci_pltfm_host *pltfm_host;
	struct sdhci_esdhc *esdhc;
	u16 host_ver;

	pltfm_host = sdhci_priv(host);
	esdhc = sdhci_pltfm_priv(pltfm_host);

	host_ver = sdhci_readw(host, SDHCI_HOST_VERSION);
	esdhc->vendor_ver = (host_ver & SDHCI_VENDOR_VER_MASK) >>
			     SDHCI_VENDOR_VER_SHIFT;
	esdhc->spec_ver = host_ver & SDHCI_SPEC_VER_MASK;
}

>>>>>>> v4.9.227
static int sdhci_esdhc_probe(struct platform_device *pdev)
{
	struct sdhci_host *host;
	struct device_node *np;
<<<<<<< HEAD
	int ret;

	host = sdhci_pltfm_init(pdev, &sdhci_esdhc_pdata, 0);
	if (IS_ERR(host))
		return PTR_ERR(host);

	sdhci_get_of_property(pdev);

	np = pdev->dev.of_node;
=======
	struct sdhci_pltfm_host *pltfm_host;
	struct sdhci_esdhc *esdhc;
	int ret;

	np = pdev->dev.of_node;

	if (of_property_read_bool(np, "little-endian"))
		host = sdhci_pltfm_init(pdev, &sdhci_esdhc_le_pdata,
					sizeof(struct sdhci_esdhc));
	else
		host = sdhci_pltfm_init(pdev, &sdhci_esdhc_be_pdata,
					sizeof(struct sdhci_esdhc));

	if (IS_ERR(host))
		return PTR_ERR(host);

	esdhc_init(pdev, host);

	sdhci_get_of_property(pdev);

	pltfm_host = sdhci_priv(host);
	esdhc = sdhci_pltfm_priv(pltfm_host);
	if (esdhc->vendor_ver == VENDOR_V_22)
		host->quirks2 |= SDHCI_QUIRK2_HOST_NO_CMD23;

	if (esdhc->vendor_ver > VENDOR_V_22)
		host->quirks &= ~SDHCI_QUIRK_NO_BUSY_IRQ;

	if (of_find_compatible_node(NULL, NULL, "fsl,p2020-esdhc")) {
		host->quirks |= SDHCI_QUIRK_RESET_AFTER_REQUEST;
		host->quirks |= SDHCI_QUIRK_BROKEN_TIMEOUT_VAL;
	}

	if (of_device_is_compatible(np, "fsl,p5040-esdhc") ||
	    of_device_is_compatible(np, "fsl,p5020-esdhc") ||
	    of_device_is_compatible(np, "fsl,p4080-esdhc") ||
	    of_device_is_compatible(np, "fsl,p1020-esdhc") ||
	    of_device_is_compatible(np, "fsl,t1040-esdhc"))
		host->quirks &= ~SDHCI_QUIRK_BROKEN_CARD_DETECTION;

	if (of_device_is_compatible(np, "fsl,ls1021a-esdhc"))
		host->quirks |= SDHCI_QUIRK_BROKEN_TIMEOUT_VAL;

>>>>>>> v4.9.227
	if (of_device_is_compatible(np, "fsl,p2020-esdhc")) {
		/*
		 * Freescale messed up with P2020 as it has a non-standard
		 * host control register
		 */
		host->quirks2 |= SDHCI_QUIRK2_BROKEN_HOST_CONTROL;
	}

	/* call to generic mmc_of_parse to support additional capabilities */
<<<<<<< HEAD
	mmc_of_parse(host->mmc);
=======
	ret = mmc_of_parse(host->mmc);
	if (ret)
		goto err;

>>>>>>> v4.9.227
	mmc_of_parse_voltage(np, &host->ocr_mask);

	ret = sdhci_add_host(host);
	if (ret)
<<<<<<< HEAD
		sdhci_pltfm_free(pdev);

	return ret;
}

static int sdhci_esdhc_remove(struct platform_device *pdev)
{
	return sdhci_pltfm_unregister(pdev);
}

=======
		goto err;

	return 0;
 err:
	sdhci_pltfm_free(pdev);
	return ret;
}

>>>>>>> v4.9.227
static const struct of_device_id sdhci_esdhc_of_match[] = {
	{ .compatible = "fsl,mpc8379-esdhc" },
	{ .compatible = "fsl,mpc8536-esdhc" },
	{ .compatible = "fsl,esdhc" },
	{ }
};
MODULE_DEVICE_TABLE(of, sdhci_esdhc_of_match);

static struct platform_driver sdhci_esdhc_driver = {
	.driver = {
		.name = "sdhci-esdhc",
		.of_match_table = sdhci_esdhc_of_match,
<<<<<<< HEAD
		.pm = ESDHC_PMOPS,
	},
	.probe = sdhci_esdhc_probe,
	.remove = sdhci_esdhc_remove,
=======
		.pm = &esdhc_of_dev_pm_ops,
	},
	.probe = sdhci_esdhc_probe,
	.remove = sdhci_pltfm_unregister,
>>>>>>> v4.9.227
};

module_platform_driver(sdhci_esdhc_driver);

MODULE_DESCRIPTION("SDHCI OF driver for Freescale MPC eSDHC");
MODULE_AUTHOR("Xiaobo Xie <X.Xie@freescale.com>, "
	      "Anton Vorontsov <avorontsov@ru.mvista.com>");
MODULE_LICENSE("GPL v2");
