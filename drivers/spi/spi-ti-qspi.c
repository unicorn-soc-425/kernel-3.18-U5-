/*
 * TI QSPI driver
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com
 * Author: Sourav Poddar <sourav.poddar@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GPLv2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/omap-dma.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pinctrl/consumer.h>
<<<<<<< HEAD
=======
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
>>>>>>> v4.9.227

#include <linux/spi/spi.h>

struct ti_qspi_regs {
	u32 clkctrl;
};

struct ti_qspi {
<<<<<<< HEAD
	struct completion       transfer_complete;
=======
	struct completion	transfer_complete;
>>>>>>> v4.9.227

	/* list synchronization */
	struct mutex            list_lock;

	struct spi_master	*master;
	void __iomem            *base;
<<<<<<< HEAD
	void __iomem            *ctrl_base;
	void __iomem            *mmap_base;
=======
	void __iomem            *mmap_base;
	struct regmap		*ctrl_base;
	unsigned int		ctrl_reg;
>>>>>>> v4.9.227
	struct clk		*fclk;
	struct device           *dev;

	struct ti_qspi_regs     ctx_reg;

<<<<<<< HEAD
=======
	dma_addr_t		mmap_phys_base;
	struct dma_chan		*rx_chan;

>>>>>>> v4.9.227
	u32 spi_max_frequency;
	u32 cmd;
	u32 dc;

<<<<<<< HEAD
	bool ctrl_mod;
=======
	bool mmap_enabled;
>>>>>>> v4.9.227
};

#define QSPI_PID			(0x0)
#define QSPI_SYSCONFIG			(0x10)
<<<<<<< HEAD
#define QSPI_INTR_STATUS_RAW_SET	(0x20)
#define QSPI_INTR_STATUS_ENABLED_CLEAR	(0x24)
#define QSPI_INTR_ENABLE_SET_REG	(0x28)
#define QSPI_INTR_ENABLE_CLEAR_REG	(0x2c)
=======
>>>>>>> v4.9.227
#define QSPI_SPI_CLOCK_CNTRL_REG	(0x40)
#define QSPI_SPI_DC_REG			(0x44)
#define QSPI_SPI_CMD_REG		(0x48)
#define QSPI_SPI_STATUS_REG		(0x4c)
#define QSPI_SPI_DATA_REG		(0x50)
<<<<<<< HEAD
#define QSPI_SPI_SETUP0_REG		(0x54)
#define QSPI_SPI_SWITCH_REG		(0x64)
#define QSPI_SPI_SETUP1_REG		(0x58)
#define QSPI_SPI_SETUP2_REG		(0x5c)
#define QSPI_SPI_SETUP3_REG		(0x60)
=======
#define QSPI_SPI_SETUP_REG(n)		((0x54 + 4 * n))
#define QSPI_SPI_SWITCH_REG		(0x64)
>>>>>>> v4.9.227
#define QSPI_SPI_DATA_REG_1		(0x68)
#define QSPI_SPI_DATA_REG_2		(0x6c)
#define QSPI_SPI_DATA_REG_3		(0x70)

#define QSPI_COMPLETION_TIMEOUT		msecs_to_jiffies(2000)

#define QSPI_FCLK			192000000

/* Clock Control */
#define QSPI_CLK_EN			(1 << 31)
#define QSPI_CLK_DIV_MAX		0xffff

/* Command */
#define QSPI_EN_CS(n)			(n << 28)
#define QSPI_WLEN(n)			((n - 1) << 19)
#define QSPI_3_PIN			(1 << 18)
#define QSPI_RD_SNGL			(1 << 16)
#define QSPI_WR_SNGL			(2 << 16)
#define QSPI_RD_DUAL			(3 << 16)
#define QSPI_RD_QUAD			(7 << 16)
#define QSPI_INVAL			(4 << 16)
<<<<<<< HEAD
#define QSPI_WC_CMD_INT_EN			(1 << 14)
#define QSPI_FLEN(n)			((n - 1) << 0)

/* STATUS REGISTER */
#define WC				0x02

/* INTERRUPT REGISTER */
#define QSPI_WC_INT_EN				(1 << 1)
#define QSPI_WC_INT_DISABLE			(1 << 1)

=======
#define QSPI_FLEN(n)			((n - 1) << 0)
#define QSPI_WLEN_MAX_BITS		128
#define QSPI_WLEN_MAX_BYTES		16
#define QSPI_WLEN_MASK			QSPI_WLEN(QSPI_WLEN_MAX_BITS)

/* STATUS REGISTER */
#define BUSY				0x01
#define WC				0x02

>>>>>>> v4.9.227
/* Device Control */
#define QSPI_DD(m, n)			(m << (3 + n * 8))
#define QSPI_CKPHA(n)			(1 << (2 + n * 8))
#define QSPI_CSPOL(n)			(1 << (1 + n * 8))
#define QSPI_CKPOL(n)			(1 << (n * 8))

#define	QSPI_FRAME			4096

#define QSPI_AUTOSUSPEND_TIMEOUT         2000

<<<<<<< HEAD
=======
#define MEM_CS_EN(n)			((n + 1) << 8)
#define MEM_CS_MASK			(7 << 8)

#define MM_SWITCH			0x1

#define QSPI_SETUP_RD_NORMAL		(0x0 << 12)
#define QSPI_SETUP_RD_DUAL		(0x1 << 12)
#define QSPI_SETUP_RD_QUAD		(0x3 << 12)
#define QSPI_SETUP_ADDR_SHIFT		8
#define QSPI_SETUP_DUMMY_SHIFT		10

>>>>>>> v4.9.227
static inline unsigned long ti_qspi_read(struct ti_qspi *qspi,
		unsigned long reg)
{
	return readl(qspi->base + reg);
}

static inline void ti_qspi_write(struct ti_qspi *qspi,
		unsigned long val, unsigned long reg)
{
	writel(val, qspi->base + reg);
}

static int ti_qspi_setup(struct spi_device *spi)
{
	struct ti_qspi	*qspi = spi_master_get_devdata(spi->master);
	struct ti_qspi_regs *ctx_reg = &qspi->ctx_reg;
	int clk_div = 0, ret;
	u32 clk_ctrl_reg, clk_rate, clk_mask;

	if (spi->master->busy) {
<<<<<<< HEAD
		dev_dbg(qspi->dev, "master busy doing other trasnfers\n");
=======
		dev_dbg(qspi->dev, "master busy doing other transfers\n");
>>>>>>> v4.9.227
		return -EBUSY;
	}

	if (!qspi->spi_max_frequency) {
		dev_err(qspi->dev, "spi max frequency not defined\n");
		return -EINVAL;
	}

	clk_rate = clk_get_rate(qspi->fclk);

	clk_div = DIV_ROUND_UP(clk_rate, qspi->spi_max_frequency) - 1;

	if (clk_div < 0) {
		dev_dbg(qspi->dev, "clock divider < 0, using /1 divider\n");
		return -EINVAL;
	}

	if (clk_div > QSPI_CLK_DIV_MAX) {
		dev_dbg(qspi->dev, "clock divider >%d , using /%d divider\n",
				QSPI_CLK_DIV_MAX, QSPI_CLK_DIV_MAX + 1);
		return -EINVAL;
	}

	dev_dbg(qspi->dev, "hz: %d, clock divider %d\n",
			qspi->spi_max_frequency, clk_div);

	ret = pm_runtime_get_sync(qspi->dev);
	if (ret < 0) {
		dev_err(qspi->dev, "pm_runtime_get_sync() failed\n");
		return ret;
	}

	clk_ctrl_reg = ti_qspi_read(qspi, QSPI_SPI_CLOCK_CNTRL_REG);

	clk_ctrl_reg &= ~QSPI_CLK_EN;

	/* disable SCLK */
	ti_qspi_write(qspi, clk_ctrl_reg, QSPI_SPI_CLOCK_CNTRL_REG);

	/* enable SCLK */
	clk_mask = QSPI_CLK_EN | clk_div;
	ti_qspi_write(qspi, clk_mask, QSPI_SPI_CLOCK_CNTRL_REG);
	ctx_reg->clkctrl = clk_mask;

	pm_runtime_mark_last_busy(qspi->dev);
	ret = pm_runtime_put_autosuspend(qspi->dev);
	if (ret < 0) {
		dev_err(qspi->dev, "pm_runtime_put_autosuspend() failed\n");
		return ret;
	}

	return 0;
}

static void ti_qspi_restore_ctx(struct ti_qspi *qspi)
{
	struct ti_qspi_regs *ctx_reg = &qspi->ctx_reg;

	ti_qspi_write(qspi, ctx_reg->clkctrl, QSPI_SPI_CLOCK_CNTRL_REG);
}

<<<<<<< HEAD
static int qspi_write_msg(struct ti_qspi *qspi, struct spi_transfer *t)
{
	int wlen, count, ret;
	unsigned int cmd;
	const u8 *txbuf;

	txbuf = t->tx_buf;
	cmd = qspi->cmd | QSPI_WR_SNGL;
	count = t->len;
	wlen = t->bits_per_word >> 3;	/* in bytes */

	while (count) {
=======
static inline u32 qspi_is_busy(struct ti_qspi *qspi)
{
	u32 stat;
	unsigned long timeout = jiffies + QSPI_COMPLETION_TIMEOUT;

	stat = ti_qspi_read(qspi, QSPI_SPI_STATUS_REG);
	while ((stat & BUSY) && time_after(timeout, jiffies)) {
		cpu_relax();
		stat = ti_qspi_read(qspi, QSPI_SPI_STATUS_REG);
	}

	WARN(stat & BUSY, "qspi busy\n");
	return stat & BUSY;
}

static inline int ti_qspi_poll_wc(struct ti_qspi *qspi)
{
	u32 stat;
	unsigned long timeout = jiffies + QSPI_COMPLETION_TIMEOUT;

	do {
		stat = ti_qspi_read(qspi, QSPI_SPI_STATUS_REG);
		if (stat & WC)
			return 0;
		cpu_relax();
	} while (time_after(timeout, jiffies));

	stat = ti_qspi_read(qspi, QSPI_SPI_STATUS_REG);
	if (stat & WC)
		return 0;
	return  -ETIMEDOUT;
}

static int qspi_write_msg(struct ti_qspi *qspi, struct spi_transfer *t,
			  int count)
{
	int wlen, xfer_len;
	unsigned int cmd;
	const u8 *txbuf;
	u32 data;

	txbuf = t->tx_buf;
	cmd = qspi->cmd | QSPI_WR_SNGL;
	wlen = t->bits_per_word >> 3;	/* in bytes */
	xfer_len = wlen;

	while (count) {
		if (qspi_is_busy(qspi))
			return -EBUSY;

>>>>>>> v4.9.227
		switch (wlen) {
		case 1:
			dev_dbg(qspi->dev, "tx cmd %08x dc %08x data %02x\n",
					cmd, qspi->dc, *txbuf);
<<<<<<< HEAD
			writeb(*txbuf, qspi->base + QSPI_SPI_DATA_REG);
=======
			if (count >= QSPI_WLEN_MAX_BYTES) {
				u32 *txp = (u32 *)txbuf;

				data = cpu_to_be32(*txp++);
				writel(data, qspi->base +
				       QSPI_SPI_DATA_REG_3);
				data = cpu_to_be32(*txp++);
				writel(data, qspi->base +
				       QSPI_SPI_DATA_REG_2);
				data = cpu_to_be32(*txp++);
				writel(data, qspi->base +
				       QSPI_SPI_DATA_REG_1);
				data = cpu_to_be32(*txp++);
				writel(data, qspi->base +
				       QSPI_SPI_DATA_REG);
				xfer_len = QSPI_WLEN_MAX_BYTES;
				cmd |= QSPI_WLEN(QSPI_WLEN_MAX_BITS);
			} else {
				writeb(*txbuf, qspi->base + QSPI_SPI_DATA_REG);
				cmd = qspi->cmd | QSPI_WR_SNGL;
				xfer_len = wlen;
				cmd |= QSPI_WLEN(wlen);
			}
>>>>>>> v4.9.227
			break;
		case 2:
			dev_dbg(qspi->dev, "tx cmd %08x dc %08x data %04x\n",
					cmd, qspi->dc, *txbuf);
			writew(*((u16 *)txbuf), qspi->base + QSPI_SPI_DATA_REG);
			break;
		case 4:
			dev_dbg(qspi->dev, "tx cmd %08x dc %08x data %08x\n",
					cmd, qspi->dc, *txbuf);
			writel(*((u32 *)txbuf), qspi->base + QSPI_SPI_DATA_REG);
			break;
		}

		ti_qspi_write(qspi, cmd, QSPI_SPI_CMD_REG);
<<<<<<< HEAD
		ret = wait_for_completion_timeout(&qspi->transfer_complete,
						  QSPI_COMPLETION_TIMEOUT);
		if (ret == 0) {
			dev_err(qspi->dev, "write timed out\n");
			return -ETIMEDOUT;
		}
		txbuf += wlen;
		count -= wlen;
=======
		if (ti_qspi_poll_wc(qspi)) {
			dev_err(qspi->dev, "write timed out\n");
			return -ETIMEDOUT;
		}
		txbuf += xfer_len;
		count -= xfer_len;
>>>>>>> v4.9.227
	}

	return 0;
}

<<<<<<< HEAD
static int qspi_read_msg(struct ti_qspi *qspi, struct spi_transfer *t)
{
	int wlen, count, ret;
=======
static int qspi_read_msg(struct ti_qspi *qspi, struct spi_transfer *t,
			 int count)
{
	int wlen;
>>>>>>> v4.9.227
	unsigned int cmd;
	u8 *rxbuf;

	rxbuf = t->rx_buf;
	cmd = qspi->cmd;
	switch (t->rx_nbits) {
	case SPI_NBITS_DUAL:
		cmd |= QSPI_RD_DUAL;
		break;
	case SPI_NBITS_QUAD:
		cmd |= QSPI_RD_QUAD;
		break;
	default:
		cmd |= QSPI_RD_SNGL;
		break;
	}
<<<<<<< HEAD
	count = t->len;
=======
>>>>>>> v4.9.227
	wlen = t->bits_per_word >> 3;	/* in bytes */

	while (count) {
		dev_dbg(qspi->dev, "rx cmd %08x dc %08x\n", cmd, qspi->dc);
<<<<<<< HEAD
		ti_qspi_write(qspi, cmd, QSPI_SPI_CMD_REG);
		ret = wait_for_completion_timeout(&qspi->transfer_complete,
				QSPI_COMPLETION_TIMEOUT);
		if (ret == 0) {
=======
		if (qspi_is_busy(qspi))
			return -EBUSY;

		ti_qspi_write(qspi, cmd, QSPI_SPI_CMD_REG);
		if (ti_qspi_poll_wc(qspi)) {
>>>>>>> v4.9.227
			dev_err(qspi->dev, "read timed out\n");
			return -ETIMEDOUT;
		}
		switch (wlen) {
		case 1:
			*rxbuf = readb(qspi->base + QSPI_SPI_DATA_REG);
			break;
		case 2:
			*((u16 *)rxbuf) = readw(qspi->base + QSPI_SPI_DATA_REG);
			break;
		case 4:
			*((u32 *)rxbuf) = readl(qspi->base + QSPI_SPI_DATA_REG);
			break;
		}
		rxbuf += wlen;
		count -= wlen;
	}

	return 0;
}

<<<<<<< HEAD
static int qspi_transfer_msg(struct ti_qspi *qspi, struct spi_transfer *t)
=======
static int qspi_transfer_msg(struct ti_qspi *qspi, struct spi_transfer *t,
			     int count)
>>>>>>> v4.9.227
{
	int ret;

	if (t->tx_buf) {
<<<<<<< HEAD
		ret = qspi_write_msg(qspi, t);
=======
		ret = qspi_write_msg(qspi, t, count);
>>>>>>> v4.9.227
		if (ret) {
			dev_dbg(qspi->dev, "Error while writing\n");
			return ret;
		}
	}

	if (t->rx_buf) {
<<<<<<< HEAD
		ret = qspi_read_msg(qspi, t);
=======
		ret = qspi_read_msg(qspi, t, count);
>>>>>>> v4.9.227
		if (ret) {
			dev_dbg(qspi->dev, "Error while reading\n");
			return ret;
		}
	}

	return 0;
}

<<<<<<< HEAD
=======
static void ti_qspi_dma_callback(void *param)
{
	struct ti_qspi *qspi = param;

	complete(&qspi->transfer_complete);
}

static int ti_qspi_dma_xfer(struct ti_qspi *qspi, dma_addr_t dma_dst,
			    dma_addr_t dma_src, size_t len)
{
	struct dma_chan *chan = qspi->rx_chan;
	struct dma_device *dma_dev = chan->device;
	dma_cookie_t cookie;
	enum dma_ctrl_flags flags = DMA_CTRL_ACK | DMA_PREP_INTERRUPT;
	struct dma_async_tx_descriptor *tx;
	int ret;

	tx = dma_dev->device_prep_dma_memcpy(chan, dma_dst, dma_src,
					     len, flags);
	if (!tx) {
		dev_err(qspi->dev, "device_prep_dma_memcpy error\n");
		return -EIO;
	}

	tx->callback = ti_qspi_dma_callback;
	tx->callback_param = qspi;
	cookie = tx->tx_submit(tx);

	ret = dma_submit_error(cookie);
	if (ret) {
		dev_err(qspi->dev, "dma_submit_error %d\n", cookie);
		return -EIO;
	}

	dma_async_issue_pending(chan);
	ret = wait_for_completion_timeout(&qspi->transfer_complete,
					  msecs_to_jiffies(len));
	if (ret <= 0) {
		dmaengine_terminate_sync(chan);
		dev_err(qspi->dev, "DMA wait_for_completion_timeout\n");
		return -ETIMEDOUT;
	}

	return 0;
}

static int ti_qspi_dma_xfer_sg(struct ti_qspi *qspi, struct sg_table rx_sg,
			       loff_t from)
{
	struct scatterlist *sg;
	dma_addr_t dma_src = qspi->mmap_phys_base + from;
	dma_addr_t dma_dst;
	int i, len, ret;

	for_each_sg(rx_sg.sgl, sg, rx_sg.nents, i) {
		dma_dst = sg_dma_address(sg);
		len = sg_dma_len(sg);
		ret = ti_qspi_dma_xfer(qspi, dma_dst, dma_src, len);
		if (ret)
			return ret;
		dma_src += len;
	}

	return 0;
}

static void ti_qspi_enable_memory_map(struct spi_device *spi)
{
	struct ti_qspi  *qspi = spi_master_get_devdata(spi->master);

	ti_qspi_write(qspi, MM_SWITCH, QSPI_SPI_SWITCH_REG);
	if (qspi->ctrl_base) {
		regmap_update_bits(qspi->ctrl_base, qspi->ctrl_reg,
				   MEM_CS_MASK,
				   MEM_CS_EN(spi->chip_select));
	}
	qspi->mmap_enabled = true;
}

static void ti_qspi_disable_memory_map(struct spi_device *spi)
{
	struct ti_qspi  *qspi = spi_master_get_devdata(spi->master);

	ti_qspi_write(qspi, 0, QSPI_SPI_SWITCH_REG);
	if (qspi->ctrl_base)
		regmap_update_bits(qspi->ctrl_base, qspi->ctrl_reg,
				   MEM_CS_MASK, 0);
	qspi->mmap_enabled = false;
}

static void ti_qspi_setup_mmap_read(struct spi_device *spi,
				    struct spi_flash_read_message *msg)
{
	struct ti_qspi  *qspi = spi_master_get_devdata(spi->master);
	u32 memval = msg->read_opcode;

	switch (msg->data_nbits) {
	case SPI_NBITS_QUAD:
		memval |= QSPI_SETUP_RD_QUAD;
		break;
	case SPI_NBITS_DUAL:
		memval |= QSPI_SETUP_RD_DUAL;
		break;
	default:
		memval |= QSPI_SETUP_RD_NORMAL;
		break;
	}
	memval |= ((msg->addr_width - 1) << QSPI_SETUP_ADDR_SHIFT |
		   msg->dummy_bytes << QSPI_SETUP_DUMMY_SHIFT);
	ti_qspi_write(qspi, memval,
		      QSPI_SPI_SETUP_REG(spi->chip_select));
}

static int ti_qspi_spi_flash_read(struct spi_device *spi,
				  struct spi_flash_read_message *msg)
{
	struct ti_qspi *qspi = spi_master_get_devdata(spi->master);
	int ret = 0;

	mutex_lock(&qspi->list_lock);

	if (!qspi->mmap_enabled)
		ti_qspi_enable_memory_map(spi);
	ti_qspi_setup_mmap_read(spi, msg);

	if (qspi->rx_chan) {
		if (msg->cur_msg_mapped) {
			ret = ti_qspi_dma_xfer_sg(qspi, msg->rx_sg, msg->from);
			if (ret)
				goto err_unlock;
		} else {
			dev_err(qspi->dev, "Invalid address for DMA\n");
			ret = -EIO;
			goto err_unlock;
		}
	} else {
		memcpy_fromio(msg->buf, qspi->mmap_base + msg->from, msg->len);
	}
	msg->retlen = msg->len;

err_unlock:
	mutex_unlock(&qspi->list_lock);

	return ret;
}

>>>>>>> v4.9.227
static int ti_qspi_start_transfer_one(struct spi_master *master,
		struct spi_message *m)
{
	struct ti_qspi *qspi = spi_master_get_devdata(master);
	struct spi_device *spi = m->spi;
	struct spi_transfer *t;
	int status = 0, ret;
<<<<<<< HEAD
	int frame_length;
=======
	unsigned int frame_len_words, transfer_len_words;
	int wlen;
>>>>>>> v4.9.227

	/* setup device control reg */
	qspi->dc = 0;

	if (spi->mode & SPI_CPHA)
		qspi->dc |= QSPI_CKPHA(spi->chip_select);
	if (spi->mode & SPI_CPOL)
		qspi->dc |= QSPI_CKPOL(spi->chip_select);
	if (spi->mode & SPI_CS_HIGH)
		qspi->dc |= QSPI_CSPOL(spi->chip_select);

<<<<<<< HEAD
	frame_length = (m->frame_length << 3) / spi->bits_per_word;

	frame_length = clamp(frame_length, 0, QSPI_FRAME);
=======
	frame_len_words = 0;
	list_for_each_entry(t, &m->transfers, transfer_list)
		frame_len_words += t->len / (t->bits_per_word >> 3);
	frame_len_words = min_t(unsigned int, frame_len_words, QSPI_FRAME);
>>>>>>> v4.9.227

	/* setup command reg */
	qspi->cmd = 0;
	qspi->cmd |= QSPI_EN_CS(spi->chip_select);
<<<<<<< HEAD
	qspi->cmd |= QSPI_FLEN(frame_length);
	qspi->cmd |= QSPI_WC_CMD_INT_EN;

	ti_qspi_write(qspi, QSPI_WC_INT_EN, QSPI_INTR_ENABLE_SET_REG);
=======
	qspi->cmd |= QSPI_FLEN(frame_len_words);

>>>>>>> v4.9.227
	ti_qspi_write(qspi, qspi->dc, QSPI_SPI_DC_REG);

	mutex_lock(&qspi->list_lock);

<<<<<<< HEAD
	list_for_each_entry(t, &m->transfers, transfer_list) {
		qspi->cmd |= QSPI_WLEN(t->bits_per_word);

		ret = qspi_transfer_msg(qspi, t);
=======
	if (qspi->mmap_enabled)
		ti_qspi_disable_memory_map(spi);

	list_for_each_entry(t, &m->transfers, transfer_list) {
		qspi->cmd = ((qspi->cmd & ~QSPI_WLEN_MASK) |
			     QSPI_WLEN(t->bits_per_word));

		wlen = t->bits_per_word >> 3;
		transfer_len_words = min(t->len / wlen, frame_len_words);

		ret = qspi_transfer_msg(qspi, t, transfer_len_words * wlen);
>>>>>>> v4.9.227
		if (ret) {
			dev_dbg(qspi->dev, "transfer message failed\n");
			mutex_unlock(&qspi->list_lock);
			return -EINVAL;
		}

<<<<<<< HEAD
		m->actual_length += t->len;
=======
		m->actual_length += transfer_len_words * wlen;
		frame_len_words -= transfer_len_words;
		if (frame_len_words == 0)
			break;
>>>>>>> v4.9.227
	}

	mutex_unlock(&qspi->list_lock);

<<<<<<< HEAD
	m->status = status;
	spi_finalize_current_message(master);

	ti_qspi_write(qspi, qspi->cmd | QSPI_INVAL, QSPI_SPI_CMD_REG);

	return status;
}

static irqreturn_t ti_qspi_isr(int irq, void *dev_id)
{
	struct ti_qspi *qspi = dev_id;
	u16 int_stat;
	u32 stat;

	irqreturn_t ret = IRQ_HANDLED;

	int_stat = ti_qspi_read(qspi, QSPI_INTR_STATUS_ENABLED_CLEAR);
	stat = ti_qspi_read(qspi, QSPI_SPI_STATUS_REG);

	if (!int_stat) {
		dev_dbg(qspi->dev, "No IRQ triggered\n");
		ret = IRQ_NONE;
		goto out;
	}

	ti_qspi_write(qspi, QSPI_WC_INT_DISABLE,
				QSPI_INTR_STATUS_ENABLED_CLEAR);
	if (stat & WC)
		complete(&qspi->transfer_complete);
out:
	return ret;
}

=======
	ti_qspi_write(qspi, qspi->cmd | QSPI_INVAL, QSPI_SPI_CMD_REG);
	m->status = status;
	spi_finalize_current_message(master);

	return status;
}

>>>>>>> v4.9.227
static int ti_qspi_runtime_resume(struct device *dev)
{
	struct ti_qspi      *qspi;

	qspi = dev_get_drvdata(dev);
	ti_qspi_restore_ctx(qspi);

	return 0;
}

static const struct of_device_id ti_qspi_match[] = {
	{.compatible = "ti,dra7xxx-qspi" },
	{.compatible = "ti,am4372-qspi" },
	{},
};
MODULE_DEVICE_TABLE(of, ti_qspi_match);

static int ti_qspi_probe(struct platform_device *pdev)
{
	struct  ti_qspi *qspi;
	struct spi_master *master;
<<<<<<< HEAD
	struct resource         *r, *res_ctrl, *res_mmap;
	struct device_node *np = pdev->dev.of_node;
	u32 max_freq;
	int ret = 0, num_cs, irq;
=======
	struct resource         *r, *res_mmap;
	struct device_node *np = pdev->dev.of_node;
	u32 max_freq;
	int ret = 0, num_cs, irq;
	dma_cap_mask_t mask;
>>>>>>> v4.9.227

	master = spi_alloc_master(&pdev->dev, sizeof(*qspi));
	if (!master)
		return -ENOMEM;

	master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_RX_DUAL | SPI_RX_QUAD;

	master->flags = SPI_MASTER_HALF_DUPLEX;
	master->setup = ti_qspi_setup;
	master->auto_runtime_pm = true;
	master->transfer_one_message = ti_qspi_start_transfer_one;
	master->dev.of_node = pdev->dev.of_node;
	master->bits_per_word_mask = SPI_BPW_MASK(32) | SPI_BPW_MASK(16) |
				     SPI_BPW_MASK(8);
<<<<<<< HEAD
=======
	master->spi_flash_read = ti_qspi_spi_flash_read;
>>>>>>> v4.9.227

	if (!of_property_read_u32(np, "num-cs", &num_cs))
		master->num_chipselect = num_cs;

	qspi = spi_master_get_devdata(master);
	qspi->master = master;
	qspi->dev = &pdev->dev;
	platform_set_drvdata(pdev, qspi);

	r = platform_get_resource_byname(pdev, IORESOURCE_MEM, "qspi_base");
	if (r == NULL) {
		r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		if (r == NULL) {
			dev_err(&pdev->dev, "missing platform data\n");
			return -ENODEV;
		}
	}

	res_mmap = platform_get_resource_byname(pdev,
			IORESOURCE_MEM, "qspi_mmap");
	if (res_mmap == NULL) {
		res_mmap = platform_get_resource(pdev, IORESOURCE_MEM, 1);
		if (res_mmap == NULL) {
			dev_err(&pdev->dev,
				"memory mapped resource not required\n");
		}
	}

<<<<<<< HEAD
	res_ctrl = platform_get_resource_byname(pdev,
			IORESOURCE_MEM, "qspi_ctrlmod");
	if (res_ctrl == NULL) {
		res_ctrl = platform_get_resource(pdev, IORESOURCE_MEM, 2);
		if (res_ctrl == NULL) {
			dev_dbg(&pdev->dev,
				"control module resources not required\n");
		}
	}

=======
>>>>>>> v4.9.227
	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "no irq resource?\n");
		return irq;
	}

	mutex_init(&qspi->list_lock);

	qspi->base = devm_ioremap_resource(&pdev->dev, r);
	if (IS_ERR(qspi->base)) {
		ret = PTR_ERR(qspi->base);
		goto free_master;
	}

<<<<<<< HEAD
	if (res_ctrl) {
		qspi->ctrl_mod = true;
		qspi->ctrl_base = devm_ioremap_resource(&pdev->dev, res_ctrl);
		if (IS_ERR(qspi->ctrl_base)) {
			ret = PTR_ERR(qspi->ctrl_base);
			goto free_master;
		}
	}

	if (res_mmap) {
		qspi->mmap_base = devm_ioremap_resource(&pdev->dev, res_mmap);
		if (IS_ERR(qspi->mmap_base)) {
			ret = PTR_ERR(qspi->mmap_base);
			goto free_master;
		}
	}

	ret = devm_request_irq(&pdev->dev, irq, ti_qspi_isr, 0,
			dev_name(&pdev->dev), qspi);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to register ISR for IRQ %d\n",
				irq);
		goto free_master;
	}

=======

	if (of_property_read_bool(np, "syscon-chipselects")) {
		qspi->ctrl_base =
		syscon_regmap_lookup_by_phandle(np,
						"syscon-chipselects");
		if (IS_ERR(qspi->ctrl_base))
			return PTR_ERR(qspi->ctrl_base);
		ret = of_property_read_u32_index(np,
						 "syscon-chipselects",
						 1, &qspi->ctrl_reg);
		if (ret) {
			dev_err(&pdev->dev,
				"couldn't get ctrl_mod reg index\n");
			return ret;
		}
	}

>>>>>>> v4.9.227
	qspi->fclk = devm_clk_get(&pdev->dev, "fck");
	if (IS_ERR(qspi->fclk)) {
		ret = PTR_ERR(qspi->fclk);
		dev_err(&pdev->dev, "could not get clk: %d\n", ret);
	}

<<<<<<< HEAD
	init_completion(&qspi->transfer_complete);

=======
>>>>>>> v4.9.227
	pm_runtime_use_autosuspend(&pdev->dev);
	pm_runtime_set_autosuspend_delay(&pdev->dev, QSPI_AUTOSUSPEND_TIMEOUT);
	pm_runtime_enable(&pdev->dev);

	if (!of_property_read_u32(np, "spi-max-frequency", &max_freq))
		qspi->spi_max_frequency = max_freq;

<<<<<<< HEAD
	ret = devm_spi_register_master(&pdev->dev, master);
	if (ret)
		goto free_master;

	return 0;
=======
	dma_cap_zero(mask);
	dma_cap_set(DMA_MEMCPY, mask);

	qspi->rx_chan = dma_request_chan_by_mask(&mask);
	if (!qspi->rx_chan) {
		dev_err(qspi->dev,
			"No Rx DMA available, trying mmap mode\n");
		ret = 0;
		goto no_dma;
	}
	master->dma_rx = qspi->rx_chan;
	init_completion(&qspi->transfer_complete);
	if (res_mmap)
		qspi->mmap_phys_base = (dma_addr_t)res_mmap->start;

no_dma:
	if (!qspi->rx_chan && res_mmap) {
		qspi->mmap_base = devm_ioremap_resource(&pdev->dev, res_mmap);
		if (IS_ERR(qspi->mmap_base)) {
			dev_info(&pdev->dev,
				 "mmap failed with error %ld using PIO mode\n",
				 PTR_ERR(qspi->mmap_base));
			qspi->mmap_base = NULL;
			master->spi_flash_read = NULL;
		}
	}
	qspi->mmap_enabled = false;

	ret = devm_spi_register_master(&pdev->dev, master);
	if (!ret)
		return 0;
>>>>>>> v4.9.227

free_master:
	spi_master_put(master);
	return ret;
}

static int ti_qspi_remove(struct platform_device *pdev)
{
	struct ti_qspi *qspi = platform_get_drvdata(pdev);
<<<<<<< HEAD
	int ret;

	ret = pm_runtime_get_sync(qspi->dev);
	if (ret < 0) {
		dev_err(qspi->dev, "pm_runtime_get_sync() failed\n");
		return ret;
	}

	ti_qspi_write(qspi, QSPI_WC_INT_DISABLE, QSPI_INTR_ENABLE_CLEAR_REG);

	pm_runtime_put(qspi->dev);
	pm_runtime_disable(&pdev->dev);

=======
	int rc;

	rc = spi_master_suspend(qspi->master);
	if (rc)
		return rc;

	pm_runtime_put_sync(&pdev->dev);
	pm_runtime_disable(&pdev->dev);

	if (qspi->rx_chan)
		dma_release_channel(qspi->rx_chan);

>>>>>>> v4.9.227
	return 0;
}

static const struct dev_pm_ops ti_qspi_pm_ops = {
	.runtime_resume = ti_qspi_runtime_resume,
};

static struct platform_driver ti_qspi_driver = {
	.probe	= ti_qspi_probe,
	.remove = ti_qspi_remove,
	.driver = {
		.name	= "ti-qspi",
<<<<<<< HEAD
		.owner	= THIS_MODULE,
=======
>>>>>>> v4.9.227
		.pm =   &ti_qspi_pm_ops,
		.of_match_table = ti_qspi_match,
	}
};

module_platform_driver(ti_qspi_driver);

MODULE_AUTHOR("Sourav Poddar <sourav.poddar@ti.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("TI QSPI controller driver");
MODULE_ALIAS("platform:ti-qspi");
