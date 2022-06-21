/*
 * Designware SPI core controller driver (refer pxa2xx_spi.c)
 *
 * Copyright (c) 2009, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/highmem.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/spi/spi.h>
#include <linux/gpio.h>

#include "spi-dw.h"

#ifdef CONFIG_DEBUG_FS
#include <linux/debugfs.h>
#endif

<<<<<<< HEAD
#define START_STATE	((void *)0)
#define RUNNING_STATE	((void *)1)
#define DONE_STATE	((void *)2)
#define ERROR_STATE	((void *)-1)

/* Slave spi_dev related */
struct chip_data {
	u16 cr0;
	u8 cs;			/* chip select pin */
	u8 n_bytes;		/* current is a 1/2/4 byte op */
=======
/* Slave spi_dev related */
struct chip_data {
	u8 cs;			/* chip select pin */
>>>>>>> v4.9.227
	u8 tmode;		/* TR/TO/RO/EEPROM */
	u8 type;		/* SPI/SSP/MicroWire */

	u8 poll_mode;		/* 1 means use poll mode */

<<<<<<< HEAD
	u32 dma_width;
	u32 rx_threshold;
	u32 tx_threshold;
	u8 enable_dma;
	u8 bits_per_word;
=======
	u8 enable_dma;
>>>>>>> v4.9.227
	u16 clk_div;		/* baud rate divider */
	u32 speed_hz;		/* baud rate */
	void (*cs_control)(u32 command);
};

#ifdef CONFIG_DEBUG_FS
#define SPI_REGS_BUFSIZE	1024
static ssize_t dw_spi_show_regs(struct file *file, char __user *user_buf,
		size_t count, loff_t *ppos)
{
	struct dw_spi *dws = file->private_data;
	char *buf;
	u32 len = 0;
	ssize_t ret;

	buf = kzalloc(SPI_REGS_BUFSIZE, GFP_KERNEL);
	if (!buf)
		return 0;

	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"%s registers:\n", dev_name(&dws->master->dev));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"=================================\n");
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"CTRL0: \t\t0x%08x\n", dw_readl(dws, DW_SPI_CTRL0));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"CTRL1: \t\t0x%08x\n", dw_readl(dws, DW_SPI_CTRL1));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"SSIENR: \t0x%08x\n", dw_readl(dws, DW_SPI_SSIENR));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"SER: \t\t0x%08x\n", dw_readl(dws, DW_SPI_SER));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"BAUDR: \t\t0x%08x\n", dw_readl(dws, DW_SPI_BAUDR));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"TXFTLR: \t0x%08x\n", dw_readl(dws, DW_SPI_TXFLTR));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"RXFTLR: \t0x%08x\n", dw_readl(dws, DW_SPI_RXFLTR));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"TXFLR: \t\t0x%08x\n", dw_readl(dws, DW_SPI_TXFLR));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"RXFLR: \t\t0x%08x\n", dw_readl(dws, DW_SPI_RXFLR));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"SR: \t\t0x%08x\n", dw_readl(dws, DW_SPI_SR));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"IMR: \t\t0x%08x\n", dw_readl(dws, DW_SPI_IMR));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"ISR: \t\t0x%08x\n", dw_readl(dws, DW_SPI_ISR));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"DMACR: \t\t0x%08x\n", dw_readl(dws, DW_SPI_DMACR));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"DMATDLR: \t0x%08x\n", dw_readl(dws, DW_SPI_DMATDLR));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"DMARDLR: \t0x%08x\n", dw_readl(dws, DW_SPI_DMARDLR));
	len += snprintf(buf + len, SPI_REGS_BUFSIZE - len,
			"=================================\n");

	ret = simple_read_from_buffer(user_buf, count, ppos, buf, len);
	kfree(buf);
	return ret;
}

static const struct file_operations dw_spi_regs_ops = {
	.owner		= THIS_MODULE,
	.open		= simple_open,
	.read		= dw_spi_show_regs,
	.llseek		= default_llseek,
};

static int dw_spi_debugfs_init(struct dw_spi *dws)
{
	char name[128];

	snprintf(name, 128, "dw_spi-%s", dev_name(&dws->master->dev));
	dws->debugfs = debugfs_create_dir(name, NULL);
	if (!dws->debugfs)
		return -ENOMEM;

	debugfs_create_file("registers", S_IFREG | S_IRUGO,
		dws->debugfs, (void *)dws, &dw_spi_regs_ops);
	return 0;
}

static void dw_spi_debugfs_remove(struct dw_spi *dws)
{
	debugfs_remove_recursive(dws->debugfs);
}

#else
static inline int dw_spi_debugfs_init(struct dw_spi *dws)
{
	return 0;
}

static inline void dw_spi_debugfs_remove(struct dw_spi *dws)
{
}
#endif /* CONFIG_DEBUG_FS */

<<<<<<< HEAD
=======
static void dw_spi_set_cs(struct spi_device *spi, bool enable)
{
	struct dw_spi *dws = spi_master_get_devdata(spi->master);
	struct chip_data *chip = spi_get_ctldata(spi);

	/* Chip select logic is inverted from spi_set_cs() */
	if (chip && chip->cs_control)
		chip->cs_control(!enable);

	if (!enable)
		dw_writel(dws, DW_SPI_SER, BIT(spi->chip_select));
}

>>>>>>> v4.9.227
/* Return the max entries we can fill into tx fifo */
static inline u32 tx_max(struct dw_spi *dws)
{
	u32 tx_left, tx_room, rxtx_gap;

	tx_left = (dws->tx_end - dws->tx) / dws->n_bytes;
<<<<<<< HEAD
	tx_room = dws->fifo_len - dw_readw(dws, DW_SPI_TXFLR);
=======
	tx_room = dws->fifo_len - dw_readl(dws, DW_SPI_TXFLR);
>>>>>>> v4.9.227

	/*
	 * Another concern is about the tx/rx mismatch, we
	 * though to use (dws->fifo_len - rxflr - txflr) as
	 * one maximum value for tx, but it doesn't cover the
	 * data which is out of tx/rx fifo and inside the
	 * shift registers. So a control from sw point of
	 * view is taken.
	 */
	rxtx_gap =  ((dws->rx_end - dws->rx) - (dws->tx_end - dws->tx))
			/ dws->n_bytes;

	return min3(tx_left, tx_room, (u32) (dws->fifo_len - rxtx_gap));
}

/* Return the max entries we should read out of rx fifo */
static inline u32 rx_max(struct dw_spi *dws)
{
	u32 rx_left = (dws->rx_end - dws->rx) / dws->n_bytes;

<<<<<<< HEAD
	return min_t(u32, rx_left, dw_readw(dws, DW_SPI_RXFLR));
=======
	return min_t(u32, rx_left, dw_readl(dws, DW_SPI_RXFLR));
>>>>>>> v4.9.227
}

static void dw_writer(struct dw_spi *dws)
{
<<<<<<< HEAD
	u32 max = tx_max(dws);
	u16 txw = 0;

=======
	u32 max;
	u16 txw = 0;

	spin_lock(&dws->buf_lock);
	max = tx_max(dws);
>>>>>>> v4.9.227
	while (max--) {
		/* Set the tx word if the transfer's original "tx" is not null */
		if (dws->tx_end - dws->len) {
			if (dws->n_bytes == 1)
				txw = *(u8 *)(dws->tx);
			else
				txw = *(u16 *)(dws->tx);
		}
<<<<<<< HEAD
		dw_writew(dws, DW_SPI_DR, txw);
		dws->tx += dws->n_bytes;
	}
=======
		dw_write_io_reg(dws, DW_SPI_DR, txw);
		dws->tx += dws->n_bytes;
	}
	spin_unlock(&dws->buf_lock);
>>>>>>> v4.9.227
}

static void dw_reader(struct dw_spi *dws)
{
<<<<<<< HEAD
	u32 max = rx_max(dws);
	u16 rxw;

	while (max--) {
		rxw = dw_readw(dws, DW_SPI_DR);
=======
	u32 max;
	u16 rxw;

	spin_lock(&dws->buf_lock);
	max = rx_max(dws);
	while (max--) {
		rxw = dw_read_io_reg(dws, DW_SPI_DR);
>>>>>>> v4.9.227
		/* Care rx only if the transfer's original "rx" is not null */
		if (dws->rx_end - dws->len) {
			if (dws->n_bytes == 1)
				*(u8 *)(dws->rx) = rxw;
			else
				*(u16 *)(dws->rx) = rxw;
		}
		dws->rx += dws->n_bytes;
	}
<<<<<<< HEAD
}

static void *next_transfer(struct dw_spi *dws)
{
	struct spi_message *msg = dws->cur_msg;
	struct spi_transfer *trans = dws->cur_transfer;

	/* Move to next transfer */
	if (trans->transfer_list.next != &msg->transfers) {
		dws->cur_transfer =
			list_entry(trans->transfer_list.next,
					struct spi_transfer,
					transfer_list);
		return RUNNING_STATE;
	}

	return DONE_STATE;
}

/*
 * Note: first step is the protocol driver prepares
 * a dma-capable memory, and this func just need translate
 * the virt addr to physical
 */
static int map_dma_buffers(struct dw_spi *dws)
{
	if (!dws->cur_msg->is_dma_mapped
		|| !dws->dma_inited
		|| !dws->cur_chip->enable_dma
		|| !dws->dma_ops)
		return 0;

	if (dws->cur_transfer->tx_dma)
		dws->tx_dma = dws->cur_transfer->tx_dma;

	if (dws->cur_transfer->rx_dma)
		dws->rx_dma = dws->cur_transfer->rx_dma;

	return 1;
}

/* Caller already set message->status; dma and pio irqs are blocked */
static void giveback(struct dw_spi *dws)
{
	struct spi_transfer *last_transfer;
	struct spi_message *msg;

	msg = dws->cur_msg;
	dws->cur_msg = NULL;
	dws->cur_transfer = NULL;
	dws->prev_chip = dws->cur_chip;
	dws->cur_chip = NULL;
	dws->dma_mapped = 0;

	last_transfer = list_last_entry(&msg->transfers, struct spi_transfer,
					transfer_list);

	if (!last_transfer->cs_change)
		spi_chip_sel(dws, msg->spi, 0);

	spi_finalize_current_message(dws->master);
=======
	spin_unlock(&dws->buf_lock);
>>>>>>> v4.9.227
}

static void int_error_stop(struct dw_spi *dws, const char *msg)
{
<<<<<<< HEAD
	/* Stop the hw */
	spi_enable_chip(dws, 0);

	dev_err(&dws->master->dev, "%s\n", msg);
	dws->cur_msg->state = ERROR_STATE;
	tasklet_schedule(&dws->pump_transfers);
}

void dw_spi_xfer_done(struct dw_spi *dws)
{
	/* Update total byte transferred return count actual bytes read */
	dws->cur_msg->actual_length += dws->len;

	/* Move to next transfer */
	dws->cur_msg->state = next_transfer(dws);

	/* Handle end of message */
	if (dws->cur_msg->state == DONE_STATE) {
		dws->cur_msg->status = 0;
		giveback(dws);
	} else
		tasklet_schedule(&dws->pump_transfers);
}
EXPORT_SYMBOL_GPL(dw_spi_xfer_done);

static irqreturn_t interrupt_transfer(struct dw_spi *dws)
{
	u16 irq_status = dw_readw(dws, DW_SPI_ISR);

	/* Error handling */
	if (irq_status & (SPI_INT_TXOI | SPI_INT_RXOI | SPI_INT_RXUI)) {
		dw_readw(dws, DW_SPI_TXOICR);
		dw_readw(dws, DW_SPI_RXOICR);
		dw_readw(dws, DW_SPI_RXUICR);
=======
	spi_reset_chip(dws);

	dev_err(&dws->master->dev, "%s\n", msg);
	dws->master->cur_msg->status = -EIO;
	spi_finalize_current_transfer(dws->master);
}

static irqreturn_t interrupt_transfer(struct dw_spi *dws)
{
	u16 irq_status = dw_readl(dws, DW_SPI_ISR);

	/* Error handling */
	if (irq_status & (SPI_INT_TXOI | SPI_INT_RXOI | SPI_INT_RXUI)) {
		dw_readl(dws, DW_SPI_ICR);
>>>>>>> v4.9.227
		int_error_stop(dws, "interrupt_transfer: fifo overrun/underrun");
		return IRQ_HANDLED;
	}

	dw_reader(dws);
	if (dws->rx_end == dws->rx) {
		spi_mask_intr(dws, SPI_INT_TXEI);
<<<<<<< HEAD
		dw_spi_xfer_done(dws);
=======
		spi_finalize_current_transfer(dws->master);
>>>>>>> v4.9.227
		return IRQ_HANDLED;
	}
	if (irq_status & SPI_INT_TXEI) {
		spi_mask_intr(dws, SPI_INT_TXEI);
		dw_writer(dws);
		/* Enable TX irq always, it will be disabled when RX finished */
		spi_umask_intr(dws, SPI_INT_TXEI);
	}

	return IRQ_HANDLED;
}

static irqreturn_t dw_spi_irq(int irq, void *dev_id)
{
<<<<<<< HEAD
	struct dw_spi *dws = dev_id;
	u16 irq_status = dw_readw(dws, DW_SPI_ISR) & 0x3f;
=======
	struct spi_master *master = dev_id;
	struct dw_spi *dws = spi_master_get_devdata(master);
	u16 irq_status = dw_readl(dws, DW_SPI_ISR) & 0x3f;
>>>>>>> v4.9.227

	if (!irq_status)
		return IRQ_NONE;

<<<<<<< HEAD
	if (!dws->cur_msg) {
=======
	if (!master->cur_msg) {
>>>>>>> v4.9.227
		spi_mask_intr(dws, SPI_INT_TXEI);
		return IRQ_HANDLED;
	}

	return dws->transfer_handler(dws);
}

/* Must be called inside pump_transfers() */
<<<<<<< HEAD
static void poll_transfer(struct dw_spi *dws)
=======
static int poll_transfer(struct dw_spi *dws)
>>>>>>> v4.9.227
{
	do {
		dw_writer(dws);
		dw_reader(dws);
		cpu_relax();
	} while (dws->rx_end > dws->rx);

<<<<<<< HEAD
	dw_spi_xfer_done(dws);
}

static void pump_transfers(unsigned long data)
{
	struct dw_spi *dws = (struct dw_spi *)data;
	struct spi_message *message = NULL;
	struct spi_transfer *transfer = NULL;
	struct spi_transfer *previous = NULL;
	struct spi_device *spi = NULL;
	struct chip_data *chip = NULL;
	u8 bits = 0;
	u8 imask = 0;
	u8 cs_change = 0;
	u16 txint_level = 0;
	u16 clk_div = 0;
	u32 speed = 0;
	u32 cr0 = 0;

	/* Get current state information */
	message = dws->cur_msg;
	transfer = dws->cur_transfer;
	chip = dws->cur_chip;
	spi = message->spi;

	if (message->state == ERROR_STATE) {
		message->status = -EIO;
		goto early_exit;
	}

	/* Handle end of message */
	if (message->state == DONE_STATE) {
		message->status = 0;
		goto early_exit;
	}

	/* Delay if requested at end of transfer */
	if (message->state == RUNNING_STATE) {
		previous = list_entry(transfer->transfer_list.prev,
					struct spi_transfer,
					transfer_list);
		if (previous->delay_usecs)
			udelay(previous->delay_usecs);
	}

	dws->n_bytes = chip->n_bytes;
	dws->dma_width = chip->dma_width;
	dws->cs_control = chip->cs_control;

	dws->rx_dma = transfer->rx_dma;
	dws->tx_dma = transfer->tx_dma;
=======
	return 0;
}

static int dw_spi_transfer_one(struct spi_master *master,
		struct spi_device *spi, struct spi_transfer *transfer)
{
	struct dw_spi *dws = spi_master_get_devdata(master);
	struct chip_data *chip = spi_get_ctldata(spi);
	unsigned long flags;
	u8 imask = 0;
	u16 txlevel = 0;
	u32 cr0;
	int ret;

	dws->dma_mapped = 0;
	spin_lock_irqsave(&dws->buf_lock, flags);
>>>>>>> v4.9.227
	dws->tx = (void *)transfer->tx_buf;
	dws->tx_end = dws->tx + transfer->len;
	dws->rx = transfer->rx_buf;
	dws->rx_end = dws->rx + transfer->len;
<<<<<<< HEAD
	dws->len = dws->cur_transfer->len;
	if (chip != dws->prev_chip)
		cs_change = 1;

	cr0 = chip->cr0;

	/* Handle per transfer options for bpw and speed */
	if (transfer->speed_hz) {
		speed = chip->speed_hz;

		if ((transfer->speed_hz != speed) || (!chip->clk_div)) {
			speed = transfer->speed_hz;

			/* clk_div doesn't support odd number */
			clk_div = dws->max_freq / speed;
			clk_div = (clk_div + 1) & 0xfffe;

			chip->speed_hz = speed;
			chip->clk_div = clk_div;
		}
	}
	if (transfer->bits_per_word) {
		bits = transfer->bits_per_word;
		dws->n_bytes = dws->dma_width = bits >> 3;
		cr0 = (bits - 1)
			| (chip->type << SPI_FRF_OFFSET)
			| (spi->mode << SPI_MODE_OFFSET)
			| (chip->tmode << SPI_TMOD_OFFSET);
	}
	message->state = RUNNING_STATE;
=======
	dws->len = transfer->len;
	spin_unlock_irqrestore(&dws->buf_lock, flags);

	/* Ensure dw->rx and dw->rx_end are visible */
	smp_mb();

	spi_enable_chip(dws, 0);

	/* Handle per transfer options for bpw and speed */
	if (transfer->speed_hz != dws->current_freq) {
		if (transfer->speed_hz != chip->speed_hz) {
			/* clk_div doesn't support odd number */
			chip->clk_div = (DIV_ROUND_UP(dws->max_freq, transfer->speed_hz) + 1) & 0xfffe;
			chip->speed_hz = transfer->speed_hz;
		}
		dws->current_freq = transfer->speed_hz;
		spi_set_clk(dws, chip->clk_div);
	}
	if (transfer->bits_per_word == 8) {
		dws->n_bytes = 1;
		dws->dma_width = 1;
	} else if (transfer->bits_per_word == 16) {
		dws->n_bytes = 2;
		dws->dma_width = 2;
	} else {
		return -EINVAL;
	}
	/* Default SPI mode is SCPOL = 0, SCPH = 0 */
	cr0 = (transfer->bits_per_word - 1)
		| (chip->type << SPI_FRF_OFFSET)
		| (spi->mode << SPI_MODE_OFFSET)
		| (chip->tmode << SPI_TMOD_OFFSET);
>>>>>>> v4.9.227

	/*
	 * Adjust transfer mode if necessary. Requires platform dependent
	 * chipselect mechanism.
	 */
<<<<<<< HEAD
	if (dws->cs_control) {
=======
	if (chip->cs_control) {
>>>>>>> v4.9.227
		if (dws->rx && dws->tx)
			chip->tmode = SPI_TMOD_TR;
		else if (dws->rx)
			chip->tmode = SPI_TMOD_RO;
		else
			chip->tmode = SPI_TMOD_TO;

		cr0 &= ~SPI_TMOD_MASK;
		cr0 |= (chip->tmode << SPI_TMOD_OFFSET);
	}

<<<<<<< HEAD
	/* Check if current transfer is a DMA transaction */
	dws->dma_mapped = map_dma_buffers(dws);
=======
	dw_writel(dws, DW_SPI_CTRL0, cr0);

	/* Check if current transfer is a DMA transaction */
	if (master->can_dma && master->can_dma(master, spi, transfer))
		dws->dma_mapped = master->cur_msg_mapped;

	/* For poll mode just disable all interrupts */
	spi_mask_intr(dws, 0xff);
>>>>>>> v4.9.227

	/*
	 * Interrupt mode
	 * we only need set the TXEI IRQ, as TX/RX always happen syncronizely
	 */
<<<<<<< HEAD
	if (!dws->dma_mapped && !chip->poll_mode) {
		int templen = dws->len / dws->n_bytes;

		txint_level = dws->fifo_len / 2;
		txint_level = (templen > txint_level) ? txint_level : templen;

		imask |= SPI_INT_TXEI | SPI_INT_TXOI |
			 SPI_INT_RXUI | SPI_INT_RXOI;
		dws->transfer_handler = interrupt_transfer;
	}

	/*
	 * Reprogram registers only if
	 *	1. chip select changes
	 *	2. clk_div is changed
	 *	3. control value changes
	 */
	if (dw_readw(dws, DW_SPI_CTRL0) != cr0 || cs_change || clk_div || imask) {
		spi_enable_chip(dws, 0);

		if (dw_readw(dws, DW_SPI_CTRL0) != cr0)
			dw_writew(dws, DW_SPI_CTRL0, cr0);

		spi_set_clk(dws, clk_div ? clk_div : chip->clk_div);
		spi_chip_sel(dws, spi, 1);

		/* Set the interrupt mask, for poll mode just disable all int */
		spi_mask_intr(dws, 0xff);
		if (imask)
			spi_umask_intr(dws, imask);
		if (txint_level)
			dw_writew(dws, DW_SPI_TXFLTR, txint_level);

		spi_enable_chip(dws, 1);
		if (cs_change)
			dws->prev_chip = chip;
	}

	if (dws->dma_mapped)
		dws->dma_ops->dma_transfer(dws, cs_change);

	if (chip->poll_mode)
		poll_transfer(dws);

	return;

early_exit:
	giveback(dws);
}

static int dw_spi_transfer_one_message(struct spi_master *master,
=======
	if (dws->dma_mapped) {
		ret = dws->dma_ops->dma_setup(dws, transfer);
		if (ret < 0) {
			spi_enable_chip(dws, 1);
			return ret;
		}
	} else if (!chip->poll_mode) {
		txlevel = min_t(u16, dws->fifo_len / 2, dws->len / dws->n_bytes);
		dw_writel(dws, DW_SPI_TXFLTR, txlevel);

		/* Set the interrupt mask */
		imask |= SPI_INT_TXEI | SPI_INT_TXOI |
			 SPI_INT_RXUI | SPI_INT_RXOI;
		spi_umask_intr(dws, imask);

		dws->transfer_handler = interrupt_transfer;
	}

	spi_enable_chip(dws, 1);

	if (dws->dma_mapped) {
		ret = dws->dma_ops->dma_transfer(dws, transfer);
		if (ret < 0)
			return ret;
	}

	if (chip->poll_mode)
		return poll_transfer(dws);

	return 1;
}

static void dw_spi_handle_err(struct spi_master *master,
>>>>>>> v4.9.227
		struct spi_message *msg)
{
	struct dw_spi *dws = spi_master_get_devdata(master);

<<<<<<< HEAD
	dws->cur_msg = msg;
	/* Initial message state */
	dws->cur_msg->state = START_STATE;
	dws->cur_transfer = list_entry(dws->cur_msg->transfers.next,
						struct spi_transfer,
						transfer_list);
	dws->cur_chip = spi_get_ctldata(dws->cur_msg->spi);

	/* Launch transfers */
	tasklet_schedule(&dws->pump_transfers);

	return 0;
=======
	if (dws->dma_mapped)
		dws->dma_ops->dma_stop(dws);

	spi_reset_chip(dws);
>>>>>>> v4.9.227
}

/* This may be called twice for each spi dev */
static int dw_spi_setup(struct spi_device *spi)
{
	struct dw_spi_chip *chip_info = NULL;
	struct chip_data *chip;
	int ret;

	/* Only alloc on first setup */
	chip = spi_get_ctldata(spi);
	if (!chip) {
		chip = kzalloc(sizeof(struct chip_data), GFP_KERNEL);
		if (!chip)
			return -ENOMEM;
		spi_set_ctldata(spi, chip);
	}

	/*
	 * Protocol drivers may change the chip settings, so...
	 * if chip_info exists, use it
	 */
	chip_info = spi->controller_data;

	/* chip_info doesn't always exist */
	if (chip_info) {
		if (chip_info->cs_control)
			chip->cs_control = chip_info->cs_control;

		chip->poll_mode = chip_info->poll_mode;
		chip->type = chip_info->type;
<<<<<<< HEAD

		chip->rx_threshold = 0;
		chip->tx_threshold = 0;

		chip->enable_dma = chip_info->enable_dma;
	}

	if (spi->bits_per_word == 8) {
		chip->n_bytes = 1;
		chip->dma_width = 1;
	} else if (spi->bits_per_word == 16) {
		chip->n_bytes = 2;
		chip->dma_width = 2;
	}
	chip->bits_per_word = spi->bits_per_word;

	if (!spi->max_speed_hz) {
		dev_err(&spi->dev, "No max speed HZ parameter\n");
		return -EINVAL;
	}

	chip->tmode = 0; /* Tx & Rx */
	/* Default SPI mode is SCPOL = 0, SCPH = 0 */
	chip->cr0 = (chip->bits_per_word - 1)
			| (chip->type << SPI_FRF_OFFSET)
			| (spi->mode  << SPI_MODE_OFFSET)
			| (chip->tmode << SPI_TMOD_OFFSET);

	if (spi->mode & SPI_LOOP)
		chip->cr0 |= 1 << SPI_SRL_OFFSET;
=======
	}

	chip->tmode = SPI_TMOD_TR;
>>>>>>> v4.9.227

	if (gpio_is_valid(spi->cs_gpio)) {
		ret = gpio_direction_output(spi->cs_gpio,
				!(spi->mode & SPI_CS_HIGH));
		if (ret)
			return ret;
	}

	return 0;
}

static void dw_spi_cleanup(struct spi_device *spi)
{
	struct chip_data *chip = spi_get_ctldata(spi);

	kfree(chip);
	spi_set_ctldata(spi, NULL);
}

/* Restart the controller, disable all interrupts, clean rx fifo */
<<<<<<< HEAD
static void spi_hw_init(struct dw_spi *dws)
{
	spi_enable_chip(dws, 0);
	spi_mask_intr(dws, 0xff);
	spi_enable_chip(dws, 1);
=======
static void spi_hw_init(struct device *dev, struct dw_spi *dws)
{
	spi_reset_chip(dws);
>>>>>>> v4.9.227

	/*
	 * Try to detect the FIFO depth if not set by interface driver,
	 * the depth could be from 2 to 256 from HW spec
	 */
	if (!dws->fifo_len) {
		u32 fifo;

<<<<<<< HEAD
		for (fifo = 2; fifo <= 256; fifo++) {
			dw_writew(dws, DW_SPI_TXFLTR, fifo);
			if (fifo != dw_readw(dws, DW_SPI_TXFLTR))
				break;
		}

		dws->fifo_len = (fifo == 2) ? 0 : fifo - 1;
		dw_writew(dws, DW_SPI_TXFLTR, 0);
=======
		for (fifo = 1; fifo < 256; fifo++) {
			dw_writel(dws, DW_SPI_TXFLTR, fifo);
			if (fifo != dw_readl(dws, DW_SPI_TXFLTR))
				break;
		}
		dw_writel(dws, DW_SPI_TXFLTR, 0);

		dws->fifo_len = (fifo == 1) ? 0 : fifo;
		dev_dbg(dev, "Detected FIFO size: %u bytes\n", dws->fifo_len);
>>>>>>> v4.9.227
	}
}

int dw_spi_add_host(struct device *dev, struct dw_spi *dws)
{
	struct spi_master *master;
	int ret;

	BUG_ON(dws == NULL);

	master = spi_alloc_master(dev, 0);
	if (!master)
		return -ENOMEM;

	dws->master = master;
	dws->type = SSI_MOTO_SPI;
<<<<<<< HEAD
	dws->prev_chip = NULL;
	dws->dma_inited = 0;
	dws->dma_addr = (dma_addr_t)(dws->paddr + 0x60);
	snprintf(dws->name, sizeof(dws->name), "dw_spi%d", dws->bus_num);

	ret = devm_request_irq(dev, dws->irq, dw_spi_irq, IRQF_SHARED,
			dws->name, dws);
	if (ret < 0) {
		dev_err(&master->dev, "can not get IRQ\n");
=======
	dws->dma_inited = 0;
	dws->dma_addr = (dma_addr_t)(dws->paddr + DW_SPI_DR);
	snprintf(dws->name, sizeof(dws->name), "dw_spi%d", dws->bus_num);
	spin_lock_init(&dws->buf_lock);

	ret = request_irq(dws->irq, dw_spi_irq, IRQF_SHARED, dws->name, master);
	if (ret < 0) {
		dev_err(dev, "can not get IRQ\n");
>>>>>>> v4.9.227
		goto err_free_master;
	}

	master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_LOOP;
	master->bits_per_word_mask = SPI_BPW_MASK(8) | SPI_BPW_MASK(16);
	master->bus_num = dws->bus_num;
	master->num_chipselect = dws->num_cs;
	master->setup = dw_spi_setup;
	master->cleanup = dw_spi_cleanup;
<<<<<<< HEAD
	master->transfer_one_message = dw_spi_transfer_one_message;
=======
	master->set_cs = dw_spi_set_cs;
	master->transfer_one = dw_spi_transfer_one;
	master->handle_err = dw_spi_handle_err;
>>>>>>> v4.9.227
	master->max_speed_hz = dws->max_freq;
	master->dev.of_node = dev->of_node;

	/* Basic HW init */
<<<<<<< HEAD
	spi_hw_init(dws);
=======
	spi_hw_init(dev, dws);
>>>>>>> v4.9.227

	if (dws->dma_ops && dws->dma_ops->dma_init) {
		ret = dws->dma_ops->dma_init(dws);
		if (ret) {
<<<<<<< HEAD
			dev_warn(&master->dev, "DMA init failed\n");
			dws->dma_inited = 0;
		}
	}

	tasklet_init(&dws->pump_transfers, pump_transfers, (unsigned long)dws);

=======
			dev_warn(dev, "DMA init failed\n");
			dws->dma_inited = 0;
		} else {
			master->can_dma = dws->dma_ops->can_dma;
		}
	}

>>>>>>> v4.9.227
	spi_master_set_devdata(master, dws);
	ret = devm_spi_register_master(dev, master);
	if (ret) {
		dev_err(&master->dev, "problem registering spi master\n");
		goto err_dma_exit;
	}

	dw_spi_debugfs_init(dws);
	return 0;

err_dma_exit:
	if (dws->dma_ops && dws->dma_ops->dma_exit)
		dws->dma_ops->dma_exit(dws);
	spi_enable_chip(dws, 0);
<<<<<<< HEAD
=======
	free_irq(dws->irq, master);
>>>>>>> v4.9.227
err_free_master:
	spi_master_put(master);
	return ret;
}
EXPORT_SYMBOL_GPL(dw_spi_add_host);

void dw_spi_remove_host(struct dw_spi *dws)
{
<<<<<<< HEAD
	if (!dws)
		return;
=======
>>>>>>> v4.9.227
	dw_spi_debugfs_remove(dws);

	if (dws->dma_ops && dws->dma_ops->dma_exit)
		dws->dma_ops->dma_exit(dws);
<<<<<<< HEAD
	spi_enable_chip(dws, 0);
	/* Disable clk */
	spi_set_clk(dws, 0);
=======

	spi_shutdown_chip(dws);

	free_irq(dws->irq, dws->master);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(dw_spi_remove_host);

int dw_spi_suspend_host(struct dw_spi *dws)
{
<<<<<<< HEAD
	int ret = 0;
=======
	int ret;
>>>>>>> v4.9.227

	ret = spi_master_suspend(dws->master);
	if (ret)
		return ret;
<<<<<<< HEAD
	spi_enable_chip(dws, 0);
	spi_set_clk(dws, 0);
	return ret;
=======

	spi_shutdown_chip(dws);
	return 0;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(dw_spi_suspend_host);

int dw_spi_resume_host(struct dw_spi *dws)
{
	int ret;

<<<<<<< HEAD
	spi_hw_init(dws);
=======
	spi_hw_init(&dws->master->dev, dws);
>>>>>>> v4.9.227
	ret = spi_master_resume(dws->master);
	if (ret)
		dev_err(&dws->master->dev, "fail to start queue (%d)\n", ret);
	return ret;
}
EXPORT_SYMBOL_GPL(dw_spi_resume_host);

MODULE_AUTHOR("Feng Tang <feng.tang@intel.com>");
MODULE_DESCRIPTION("Driver for DesignWare SPI controller core");
MODULE_LICENSE("GPL v2");
