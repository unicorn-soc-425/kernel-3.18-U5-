/*
 * 8250_dma.c - DMA Engine API support for 8250.c
 *
 * Copyright (C) 2013 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/serial_reg.h>
#include <linux/dma-mapping.h>

#include "8250.h"

static void __dma_tx_complete(void *param)
{
	struct uart_8250_port	*p = param;
	struct uart_8250_dma	*dma = p->dma;
	struct circ_buf		*xmit = &p->port.state->xmit;
	unsigned long	flags;
<<<<<<< HEAD
=======
	int		ret;
>>>>>>> v4.9.227

	dma_sync_single_for_cpu(dma->txchan->device->dev, dma->tx_addr,
				UART_XMIT_SIZE, DMA_TO_DEVICE);

	spin_lock_irqsave(&p->port.lock, flags);

	dma->tx_running = 0;

	xmit->tail += dma->tx_size;
	xmit->tail &= UART_XMIT_SIZE - 1;
	p->port.icount.tx += dma->tx_size;

	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
		uart_write_wakeup(&p->port);

<<<<<<< HEAD
	if (!uart_circ_empty(xmit) && !uart_tx_stopped(&p->port))
		serial8250_tx_dma(p);
=======
	ret = serial8250_tx_dma(p);
	if (ret) {
		p->ier |= UART_IER_THRI;
		serial_port_out(&p->port, UART_IER, p->ier);
	}
>>>>>>> v4.9.227

	spin_unlock_irqrestore(&p->port.lock, flags);
}

static void __dma_rx_complete(void *param)
{
	struct uart_8250_port	*p = param;
	struct uart_8250_dma	*dma = p->dma;
	struct tty_port		*tty_port = &p->port.state->port;
	struct dma_tx_state	state;
	int			count;

<<<<<<< HEAD
	dma_sync_single_for_cpu(dma->rxchan->device->dev, dma->rx_addr,
				dma->rx_size, DMA_FROM_DEVICE);

	dmaengine_tx_status(dma->rxchan, dma->rx_cookie, &state);
	dmaengine_terminate_all(dma->rxchan);
=======
	dma->rx_running = 0;
	dmaengine_tx_status(dma->rxchan, dma->rx_cookie, &state);
>>>>>>> v4.9.227

	count = dma->rx_size - state.residue;

	tty_insert_flip_string(tty_port, dma->rx_buf, count);
	p->port.icount.rx += count;

	tty_flip_buffer_push(tty_port);
}

int serial8250_tx_dma(struct uart_8250_port *p)
{
	struct uart_8250_dma		*dma = p->dma;
	struct circ_buf			*xmit = &p->port.state->xmit;
	struct dma_async_tx_descriptor	*desc;
<<<<<<< HEAD
=======
	int ret;
>>>>>>> v4.9.227

	if (uart_tx_stopped(&p->port) || dma->tx_running ||
	    uart_circ_empty(xmit))
		return 0;

	dma->tx_size = CIRC_CNT_TO_END(xmit->head, xmit->tail, UART_XMIT_SIZE);

	desc = dmaengine_prep_slave_single(dma->txchan,
					   dma->tx_addr + xmit->tail,
					   dma->tx_size, DMA_MEM_TO_DEV,
					   DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
<<<<<<< HEAD
	if (!desc)
		return -EBUSY;

	dma->tx_running = 1;

=======
	if (!desc) {
		ret = -EBUSY;
		goto err;
	}

	dma->tx_running = 1;
>>>>>>> v4.9.227
	desc->callback = __dma_tx_complete;
	desc->callback_param = p;

	dma->tx_cookie = dmaengine_submit(desc);

	dma_sync_single_for_device(dma->txchan->device->dev, dma->tx_addr,
				   UART_XMIT_SIZE, DMA_TO_DEVICE);

	dma_async_issue_pending(dma->txchan);
<<<<<<< HEAD

	return 0;
}
EXPORT_SYMBOL_GPL(serial8250_tx_dma);

int serial8250_rx_dma(struct uart_8250_port *p, unsigned int iir)
{
	struct uart_8250_dma		*dma = p->dma;
	struct dma_async_tx_descriptor	*desc;
	struct dma_tx_state		state;
	int				dma_status;

	dma_status = dmaengine_tx_status(dma->rxchan, dma->rx_cookie, &state);

	switch (iir & 0x3f) {
	case UART_IIR_RLSI:
		/* 8250_core handles errors and break interrupts */
		return -EIO;
	case UART_IIR_RX_TIMEOUT:
		/*
		 * If RCVR FIFO trigger level was not reached, complete the
		 * transfer and let 8250_core copy the remaining data.
		 */
		if (dma_status == DMA_IN_PROGRESS) {
			dmaengine_pause(dma->rxchan);
			__dma_rx_complete(p);
		}
		return -ETIMEDOUT;
	default:
		break;
	}

	if (dma_status)
=======
	if (dma->tx_err) {
		dma->tx_err = 0;
		if (p->ier & UART_IER_THRI) {
			p->ier &= ~UART_IER_THRI;
			serial_out(p, UART_IER, p->ier);
		}
	}
	return 0;
err:
	dma->tx_err = 1;
	return ret;
}

int serial8250_rx_dma(struct uart_8250_port *p)
{
	struct uart_8250_dma		*dma = p->dma;
	struct dma_async_tx_descriptor	*desc;

	if (dma->rx_running)
>>>>>>> v4.9.227
		return 0;

	desc = dmaengine_prep_slave_single(dma->rxchan, dma->rx_addr,
					   dma->rx_size, DMA_DEV_TO_MEM,
					   DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!desc)
		return -EBUSY;

<<<<<<< HEAD
=======
	dma->rx_running = 1;
>>>>>>> v4.9.227
	desc->callback = __dma_rx_complete;
	desc->callback_param = p;

	dma->rx_cookie = dmaengine_submit(desc);

<<<<<<< HEAD
	dma_sync_single_for_device(dma->rxchan->device->dev, dma->rx_addr,
				   dma->rx_size, DMA_FROM_DEVICE);

=======
>>>>>>> v4.9.227
	dma_async_issue_pending(dma->rxchan);

	return 0;
}
<<<<<<< HEAD
EXPORT_SYMBOL_GPL(serial8250_rx_dma);
=======

void serial8250_rx_dma_flush(struct uart_8250_port *p)
{
	struct uart_8250_dma *dma = p->dma;

	if (dma->rx_running) {
		dmaengine_pause(dma->rxchan);
		__dma_rx_complete(p);
		dmaengine_terminate_async(dma->rxchan);
	}
}
EXPORT_SYMBOL_GPL(serial8250_rx_dma_flush);
>>>>>>> v4.9.227

int serial8250_request_dma(struct uart_8250_port *p)
{
	struct uart_8250_dma	*dma = p->dma;
<<<<<<< HEAD
	dma_cap_mask_t		mask;
=======
	phys_addr_t rx_dma_addr = dma->rx_dma_addr ?
				  dma->rx_dma_addr : p->port.mapbase;
	phys_addr_t tx_dma_addr = dma->tx_dma_addr ?
				  dma->tx_dma_addr : p->port.mapbase;
	dma_cap_mask_t		mask;
	struct dma_slave_caps	caps;
	int			ret;
>>>>>>> v4.9.227

	/* Default slave configuration parameters */
	dma->rxconf.direction		= DMA_DEV_TO_MEM;
	dma->rxconf.src_addr_width	= DMA_SLAVE_BUSWIDTH_1_BYTE;
<<<<<<< HEAD
	dma->rxconf.src_addr		= p->port.mapbase + UART_RX;

	dma->txconf.direction		= DMA_MEM_TO_DEV;
	dma->txconf.dst_addr_width	= DMA_SLAVE_BUSWIDTH_1_BYTE;
	dma->txconf.dst_addr		= p->port.mapbase + UART_TX;
=======
	dma->rxconf.src_addr		= rx_dma_addr + UART_RX;

	dma->txconf.direction		= DMA_MEM_TO_DEV;
	dma->txconf.dst_addr_width	= DMA_SLAVE_BUSWIDTH_1_BYTE;
	dma->txconf.dst_addr		= tx_dma_addr + UART_TX;
>>>>>>> v4.9.227

	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);

	/* Get a channel for RX */
	dma->rxchan = dma_request_slave_channel_compat(mask,
						       dma->fn, dma->rx_param,
						       p->port.dev, "rx");
	if (!dma->rxchan)
		return -ENODEV;

<<<<<<< HEAD
=======
	/* 8250 rx dma requires dmaengine driver to support pause/terminate */
	ret = dma_get_slave_caps(dma->rxchan, &caps);
	if (ret)
		goto release_rx;
	if (!caps.cmd_pause || !caps.cmd_terminate ||
	    caps.residue_granularity == DMA_RESIDUE_GRANULARITY_DESCRIPTOR) {
		ret = -EINVAL;
		goto release_rx;
	}

>>>>>>> v4.9.227
	dmaengine_slave_config(dma->rxchan, &dma->rxconf);

	/* Get a channel for TX */
	dma->txchan = dma_request_slave_channel_compat(mask,
						       dma->fn, dma->tx_param,
						       p->port.dev, "tx");
	if (!dma->txchan) {
<<<<<<< HEAD
		dma_release_channel(dma->rxchan);
		return -ENODEV;
=======
		ret = -ENODEV;
		goto release_rx;
	}

	/* 8250 tx dma requires dmaengine driver to support terminate */
	ret = dma_get_slave_caps(dma->txchan, &caps);
	if (ret)
		goto err;
	if (!caps.cmd_terminate) {
		ret = -EINVAL;
		goto err;
>>>>>>> v4.9.227
	}

	dmaengine_slave_config(dma->txchan, &dma->txconf);

	/* RX buffer */
	if (!dma->rx_size)
		dma->rx_size = PAGE_SIZE;

	dma->rx_buf = dma_alloc_coherent(dma->rxchan->device->dev, dma->rx_size,
					&dma->rx_addr, GFP_KERNEL);
<<<<<<< HEAD
	if (!dma->rx_buf)
		goto err;
=======
	if (!dma->rx_buf) {
		ret = -ENOMEM;
		goto err;
	}
>>>>>>> v4.9.227

	/* TX buffer */
	dma->tx_addr = dma_map_single(dma->txchan->device->dev,
					p->port.state->xmit.buf,
					UART_XMIT_SIZE,
					DMA_TO_DEVICE);
	if (dma_mapping_error(dma->txchan->device->dev, dma->tx_addr)) {
		dma_free_coherent(dma->rxchan->device->dev, dma->rx_size,
				  dma->rx_buf, dma->rx_addr);
<<<<<<< HEAD
=======
		ret = -ENOMEM;
>>>>>>> v4.9.227
		goto err;
	}

	dev_dbg_ratelimited(p->port.dev, "got both dma channels\n");

	return 0;
err:
<<<<<<< HEAD
	dma_release_channel(dma->rxchan);
	dma_release_channel(dma->txchan);

	return -ENOMEM;
=======
	dma_release_channel(dma->txchan);
release_rx:
	dma_release_channel(dma->rxchan);
	return ret;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(serial8250_request_dma);

void serial8250_release_dma(struct uart_8250_port *p)
{
	struct uart_8250_dma *dma = p->dma;

	if (!dma)
		return;

	/* Release RX resources */
<<<<<<< HEAD
	dmaengine_terminate_all(dma->rxchan);
=======
	dmaengine_terminate_sync(dma->rxchan);
>>>>>>> v4.9.227
	dma_free_coherent(dma->rxchan->device->dev, dma->rx_size, dma->rx_buf,
			  dma->rx_addr);
	dma_release_channel(dma->rxchan);
	dma->rxchan = NULL;

	/* Release TX resources */
<<<<<<< HEAD
	dmaengine_terminate_all(dma->txchan);
=======
	dmaengine_terminate_sync(dma->txchan);
>>>>>>> v4.9.227
	dma_unmap_single(dma->txchan->device->dev, dma->tx_addr,
			 UART_XMIT_SIZE, DMA_TO_DEVICE);
	dma_release_channel(dma->txchan);
	dma->txchan = NULL;
	dma->tx_running = 0;

	dev_dbg_ratelimited(p->port.dev, "dma channels released\n");
}
EXPORT_SYMBOL_GPL(serial8250_release_dma);
