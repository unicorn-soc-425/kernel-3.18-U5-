/**
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *      Ben Dooks <ben@simtec.co.uk>
 *      http://armlinux.simtec.co.uk/
 *
 * S3C USB2.0 High-speed / OtG driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
<<<<<<< HEAD
#include <linux/debugfs.h>
=======
#include <linux/mutex.h>
>>>>>>> v4.9.227
#include <linux/seq_file.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/slab.h>
<<<<<<< HEAD
#include <linux/clk.h>
#include <linux/regulator/consumer.h>
#include <linux/of_platform.h>
#include <linux/phy/phy.h>
=======
#include <linux/of_platform.h>
>>>>>>> v4.9.227

#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb/phy.h>
<<<<<<< HEAD
#include <linux/platform_data/s3c-hsotg.h>

#include "core.h"

/* conversion functions */
static inline struct s3c_hsotg_req *our_req(struct usb_request *req)
{
	return container_of(req, struct s3c_hsotg_req, req);
}

static inline struct s3c_hsotg_ep *our_ep(struct usb_ep *ep)
{
	return container_of(ep, struct s3c_hsotg_ep, ep);
}

static inline struct s3c_hsotg *to_hsotg(struct usb_gadget *gadget)
{
	return container_of(gadget, struct s3c_hsotg, gadget);
=======

#include "core.h"
#include "hw.h"

/* conversion functions */
static inline struct dwc2_hsotg_req *our_req(struct usb_request *req)
{
	return container_of(req, struct dwc2_hsotg_req, req);
}

static inline struct dwc2_hsotg_ep *our_ep(struct usb_ep *ep)
{
	return container_of(ep, struct dwc2_hsotg_ep, ep);
}

static inline struct dwc2_hsotg *to_hsotg(struct usb_gadget *gadget)
{
	return container_of(gadget, struct dwc2_hsotg, gadget);
>>>>>>> v4.9.227
}

static inline void __orr32(void __iomem *ptr, u32 val)
{
<<<<<<< HEAD
	writel(readl(ptr) | val, ptr);
=======
	dwc2_writel(dwc2_readl(ptr) | val, ptr);
>>>>>>> v4.9.227
}

static inline void __bic32(void __iomem *ptr, u32 val)
{
<<<<<<< HEAD
	writel(readl(ptr) & ~val, ptr);
}

/* forward decleration of functions */
static void s3c_hsotg_dump(struct s3c_hsotg *hsotg);
=======
	dwc2_writel(dwc2_readl(ptr) & ~val, ptr);
}

static inline struct dwc2_hsotg_ep *index_to_ep(struct dwc2_hsotg *hsotg,
						u32 ep_index, u32 dir_in)
{
	if (dir_in)
		return hsotg->eps_in[ep_index];
	else
		return hsotg->eps_out[ep_index];
}

/* forward declaration of functions */
static void dwc2_hsotg_dump(struct dwc2_hsotg *hsotg);
>>>>>>> v4.9.227

/**
 * using_dma - return the DMA status of the driver.
 * @hsotg: The driver state.
 *
 * Return true if we're using DMA.
 *
 * Currently, we have the DMA support code worked into everywhere
 * that needs it, but the AMBA DMA implementation in the hardware can
 * only DMA from 32bit aligned addresses. This means that gadgets such
 * as the CDC Ethernet cannot work as they often pass packets which are
 * not 32bit aligned.
 *
 * Unfortunately the choice to use DMA or not is global to the controller
 * and seems to be only settable when the controller is being put through
 * a core reset. This means we either need to fix the gadgets to take
 * account of DMA alignment, or add bounce buffers (yuerk).
 *
<<<<<<< HEAD
 * Until this issue is sorted out, we always return 'false'.
 */
static inline bool using_dma(struct s3c_hsotg *hsotg)
{
	return false;	/* support is not complete */
}

/**
 * s3c_hsotg_en_gsint - enable one or more of the general interrupt
 * @hsotg: The device state
 * @ints: A bitmask of the interrupts to enable
 */
static void s3c_hsotg_en_gsint(struct s3c_hsotg *hsotg, u32 ints)
{
	u32 gsintmsk = readl(hsotg->regs + GINTMSK);
=======
 * g_using_dma is set depending on dts flag.
 */
static inline bool using_dma(struct dwc2_hsotg *hsotg)
{
	return hsotg->g_using_dma;
}

/**
 * dwc2_gadget_incr_frame_num - Increments the targeted frame number.
 * @hs_ep: The endpoint
 * @increment: The value to increment by
 *
 * This function will also check if the frame number overruns DSTS_SOFFN_LIMIT.
 * If an overrun occurs it will wrap the value and set the frame_overrun flag.
 */
static inline void dwc2_gadget_incr_frame_num(struct dwc2_hsotg_ep *hs_ep)
{
	hs_ep->target_frame += hs_ep->interval;
	if (hs_ep->target_frame > DSTS_SOFFN_LIMIT) {
		hs_ep->frame_overrun = 1;
		hs_ep->target_frame &= DSTS_SOFFN_LIMIT;
	} else {
		hs_ep->frame_overrun = 0;
	}
}

/**
 * dwc2_hsotg_en_gsint - enable one or more of the general interrupt
 * @hsotg: The device state
 * @ints: A bitmask of the interrupts to enable
 */
static void dwc2_hsotg_en_gsint(struct dwc2_hsotg *hsotg, u32 ints)
{
	u32 gsintmsk = dwc2_readl(hsotg->regs + GINTMSK);
>>>>>>> v4.9.227
	u32 new_gsintmsk;

	new_gsintmsk = gsintmsk | ints;

	if (new_gsintmsk != gsintmsk) {
		dev_dbg(hsotg->dev, "gsintmsk now 0x%08x\n", new_gsintmsk);
<<<<<<< HEAD
		writel(new_gsintmsk, hsotg->regs + GINTMSK);
=======
		dwc2_writel(new_gsintmsk, hsotg->regs + GINTMSK);
>>>>>>> v4.9.227
	}
}

/**
<<<<<<< HEAD
 * s3c_hsotg_disable_gsint - disable one or more of the general interrupt
 * @hsotg: The device state
 * @ints: A bitmask of the interrupts to enable
 */
static void s3c_hsotg_disable_gsint(struct s3c_hsotg *hsotg, u32 ints)
{
	u32 gsintmsk = readl(hsotg->regs + GINTMSK);
=======
 * dwc2_hsotg_disable_gsint - disable one or more of the general interrupt
 * @hsotg: The device state
 * @ints: A bitmask of the interrupts to enable
 */
static void dwc2_hsotg_disable_gsint(struct dwc2_hsotg *hsotg, u32 ints)
{
	u32 gsintmsk = dwc2_readl(hsotg->regs + GINTMSK);
>>>>>>> v4.9.227
	u32 new_gsintmsk;

	new_gsintmsk = gsintmsk & ~ints;

	if (new_gsintmsk != gsintmsk)
<<<<<<< HEAD
		writel(new_gsintmsk, hsotg->regs + GINTMSK);
}

/**
 * s3c_hsotg_ctrl_epint - enable/disable an endpoint irq
=======
		dwc2_writel(new_gsintmsk, hsotg->regs + GINTMSK);
}

/**
 * dwc2_hsotg_ctrl_epint - enable/disable an endpoint irq
>>>>>>> v4.9.227
 * @hsotg: The device state
 * @ep: The endpoint index
 * @dir_in: True if direction is in.
 * @en: The enable value, true to enable
 *
 * Set or clear the mask for an individual endpoint's interrupt
 * request.
 */
<<<<<<< HEAD
static void s3c_hsotg_ctrl_epint(struct s3c_hsotg *hsotg,
=======
static void dwc2_hsotg_ctrl_epint(struct dwc2_hsotg *hsotg,
>>>>>>> v4.9.227
				 unsigned int ep, unsigned int dir_in,
				 unsigned int en)
{
	unsigned long flags;
	u32 bit = 1 << ep;
	u32 daint;

	if (!dir_in)
		bit <<= 16;

	local_irq_save(flags);
<<<<<<< HEAD
	daint = readl(hsotg->regs + DAINTMSK);
=======
	daint = dwc2_readl(hsotg->regs + DAINTMSK);
>>>>>>> v4.9.227
	if (en)
		daint |= bit;
	else
		daint &= ~bit;
<<<<<<< HEAD
	writel(daint, hsotg->regs + DAINTMSK);
=======
	dwc2_writel(daint, hsotg->regs + DAINTMSK);
>>>>>>> v4.9.227
	local_irq_restore(flags);
}

/**
<<<<<<< HEAD
 * s3c_hsotg_init_fifo - initialise non-periodic FIFOs
 * @hsotg: The device instance.
 */
static void s3c_hsotg_init_fifo(struct s3c_hsotg *hsotg)
{
	unsigned int ep;
	unsigned int addr;
	unsigned int size;
	int timeout;
	u32 val;

	/* set FIFO sizes to 2048/1024 */

	writel(2048, hsotg->regs + GRXFSIZ);
	writel((2048 << FIFOSIZE_STARTADDR_SHIFT) |
		(1024 << FIFOSIZE_DEPTH_SHIFT), hsotg->regs + GNPTXFSIZ);
=======
 * dwc2_hsotg_init_fifo - initialise non-periodic FIFOs
 * @hsotg: The device instance.
 */
static void dwc2_hsotg_init_fifo(struct dwc2_hsotg *hsotg)
{
	unsigned int ep;
	unsigned int addr;
	int timeout;
	u32 val;

	/* Reset fifo map if not correctly cleared during previous session */
	WARN_ON(hsotg->fifo_map);
	hsotg->fifo_map = 0;

	/* set RX/NPTX FIFO sizes */
	dwc2_writel(hsotg->g_rx_fifo_sz, hsotg->regs + GRXFSIZ);
	dwc2_writel((hsotg->g_rx_fifo_sz << FIFOSIZE_STARTADDR_SHIFT) |
		(hsotg->g_np_g_tx_fifo_sz << FIFOSIZE_DEPTH_SHIFT),
		hsotg->regs + GNPTXFSIZ);
>>>>>>> v4.9.227

	/*
	 * arange all the rest of the TX FIFOs, as some versions of this
	 * block have overlapping default addresses. This also ensures
	 * that if the settings have been changed, then they are set to
	 * known values.
	 */

	/* start at the end of the GNPTXFSIZ, rounded up */
<<<<<<< HEAD
	addr = 2048 + 1024;

	/*
	 * Because we have not enough memory to have each TX FIFO of size at
	 * least 3072 bytes (the maximum single packet size), we create four
	 * FIFOs of lenght 1024, and four of length 3072 bytes, and assing
	 * them to endpoints dynamically according to maxpacket size value of
	 * given endpoint.
	 */

	/* 256*4=1024 bytes FIFO length */
	size = 256;
	for (ep = 1; ep <= 4; ep++) {
		val = addr;
		val |= size << FIFOSIZE_DEPTH_SHIFT;
		WARN_ONCE(addr + size > hsotg->fifo_mem,
			  "insufficient fifo memory");
		addr += size;

		writel(val, hsotg->regs + DPTXFSIZN(ep));
	}
	/* 768*4=3072 bytes FIFO length */
	size = 768;
	for (ep = 5; ep <= 8; ep++) {
		val = addr;
		val |= size << FIFOSIZE_DEPTH_SHIFT;
		WARN_ONCE(addr + size > hsotg->fifo_mem,
			  "insufficient fifo memory");
		addr += size;

		writel(val, hsotg->regs + DPTXFSIZN(ep));
=======
	addr = hsotg->g_rx_fifo_sz + hsotg->g_np_g_tx_fifo_sz;

	/*
	 * Configure fifos sizes from provided configuration and assign
	 * them to endpoints dynamically according to maxpacket size value of
	 * given endpoint.
	 */
	for (ep = 1; ep < MAX_EPS_CHANNELS; ep++) {
		if (!hsotg->g_tx_fifo_sz[ep])
			continue;
		val = addr;
		val |= hsotg->g_tx_fifo_sz[ep] << FIFOSIZE_DEPTH_SHIFT;
		WARN_ONCE(addr + hsotg->g_tx_fifo_sz[ep] > hsotg->fifo_mem,
			  "insufficient fifo memory");
		addr += hsotg->g_tx_fifo_sz[ep];

		dwc2_writel(val, hsotg->regs + DPTXFSIZN(ep));
>>>>>>> v4.9.227
	}

	/*
	 * according to p428 of the design guide, we need to ensure that
	 * all fifos are flushed before continuing
	 */

<<<<<<< HEAD
	writel(GRSTCTL_TXFNUM(0x10) | GRSTCTL_TXFFLSH |
=======
	dwc2_writel(GRSTCTL_TXFNUM(0x10) | GRSTCTL_TXFFLSH |
>>>>>>> v4.9.227
	       GRSTCTL_RXFFLSH, hsotg->regs + GRSTCTL);

	/* wait until the fifos are both flushed */
	timeout = 100;
	while (1) {
<<<<<<< HEAD
		val = readl(hsotg->regs + GRSTCTL);
=======
		val = dwc2_readl(hsotg->regs + GRSTCTL);
>>>>>>> v4.9.227

		if ((val & (GRSTCTL_TXFFLSH | GRSTCTL_RXFFLSH)) == 0)
			break;

		if (--timeout == 0) {
			dev_err(hsotg->dev,
				"%s: timeout flushing fifos (GRSTCTL=%08x)\n",
				__func__, val);
<<<<<<< HEAD
=======
			break;
>>>>>>> v4.9.227
		}

		udelay(1);
	}

	dev_dbg(hsotg->dev, "FIFOs reset, timeout at %d\n", timeout);
}

/**
 * @ep: USB endpoint to allocate request for.
 * @flags: Allocation flags
 *
 * Allocate a new USB request structure appropriate for the specified endpoint
 */
<<<<<<< HEAD
static struct usb_request *s3c_hsotg_ep_alloc_request(struct usb_ep *ep,
						      gfp_t flags)
{
	struct s3c_hsotg_req *req;

	req = kzalloc(sizeof(struct s3c_hsotg_req), flags);
=======
static struct usb_request *dwc2_hsotg_ep_alloc_request(struct usb_ep *ep,
						      gfp_t flags)
{
	struct dwc2_hsotg_req *req;

	req = kzalloc(sizeof(struct dwc2_hsotg_req), flags);
>>>>>>> v4.9.227
	if (!req)
		return NULL;

	INIT_LIST_HEAD(&req->queue);

	return &req->req;
}

/**
 * is_ep_periodic - return true if the endpoint is in periodic mode.
 * @hs_ep: The endpoint to query.
 *
 * Returns true if the endpoint is in periodic mode, meaning it is being
 * used for an Interrupt or ISO transfer.
 */
<<<<<<< HEAD
static inline int is_ep_periodic(struct s3c_hsotg_ep *hs_ep)
=======
static inline int is_ep_periodic(struct dwc2_hsotg_ep *hs_ep)
>>>>>>> v4.9.227
{
	return hs_ep->periodic;
}

/**
<<<<<<< HEAD
 * s3c_hsotg_unmap_dma - unmap the DMA memory being used for the request
=======
 * dwc2_hsotg_unmap_dma - unmap the DMA memory being used for the request
>>>>>>> v4.9.227
 * @hsotg: The device state.
 * @hs_ep: The endpoint for the request
 * @hs_req: The request being processed.
 *
<<<<<<< HEAD
 * This is the reverse of s3c_hsotg_map_dma(), called for the completion
 * of a request to ensure the buffer is ready for access by the caller.
 */
static void s3c_hsotg_unmap_dma(struct s3c_hsotg *hsotg,
				struct s3c_hsotg_ep *hs_ep,
				struct s3c_hsotg_req *hs_req)
=======
 * This is the reverse of dwc2_hsotg_map_dma(), called for the completion
 * of a request to ensure the buffer is ready for access by the caller.
 */
static void dwc2_hsotg_unmap_dma(struct dwc2_hsotg *hsotg,
				struct dwc2_hsotg_ep *hs_ep,
				struct dwc2_hsotg_req *hs_req)
>>>>>>> v4.9.227
{
	struct usb_request *req = &hs_req->req;

	/* ignore this if we're not moving any data */
	if (hs_req->req.length == 0)
		return;

	usb_gadget_unmap_request(&hsotg->gadget, req, hs_ep->dir_in);
}

/**
<<<<<<< HEAD
 * s3c_hsotg_write_fifo - write packet Data to the TxFIFO
=======
 * dwc2_hsotg_write_fifo - write packet Data to the TxFIFO
>>>>>>> v4.9.227
 * @hsotg: The controller state.
 * @hs_ep: The endpoint we're going to write for.
 * @hs_req: The request to write data for.
 *
 * This is called when the TxFIFO has some space in it to hold a new
 * transmission and we have something to give it. The actual setup of
 * the data size is done elsewhere, so all we have to do is to actually
 * write the data.
 *
 * The return value is zero if there is more space (or nothing was done)
 * otherwise -ENOSPC is returned if the FIFO space was used up.
 *
 * This routine is only needed for PIO
 */
<<<<<<< HEAD
static int s3c_hsotg_write_fifo(struct s3c_hsotg *hsotg,
				struct s3c_hsotg_ep *hs_ep,
				struct s3c_hsotg_req *hs_req)
{
	bool periodic = is_ep_periodic(hs_ep);
	u32 gnptxsts = readl(hsotg->regs + GNPTXSTS);
=======
static int dwc2_hsotg_write_fifo(struct dwc2_hsotg *hsotg,
				struct dwc2_hsotg_ep *hs_ep,
				struct dwc2_hsotg_req *hs_req)
{
	bool periodic = is_ep_periodic(hs_ep);
	u32 gnptxsts = dwc2_readl(hsotg->regs + GNPTXSTS);
>>>>>>> v4.9.227
	int buf_pos = hs_req->req.actual;
	int to_write = hs_ep->size_loaded;
	void *data;
	int can_write;
	int pkt_round;
	int max_transfer;

	to_write -= (buf_pos - hs_ep->last_load);

	/* if there's nothing to write, get out early */
	if (to_write == 0)
		return 0;

	if (periodic && !hsotg->dedicated_fifos) {
<<<<<<< HEAD
		u32 epsize = readl(hsotg->regs + DIEPTSIZ(hs_ep->index));
=======
		u32 epsize = dwc2_readl(hsotg->regs + DIEPTSIZ(hs_ep->index));
>>>>>>> v4.9.227
		int size_left;
		int size_done;

		/*
		 * work out how much data was loaded so we can calculate
		 * how much data is left in the fifo.
		 */

		size_left = DXEPTSIZ_XFERSIZE_GET(epsize);

		/*
		 * if shared fifo, we cannot write anything until the
		 * previous data has been completely sent.
		 */
		if (hs_ep->fifo_load != 0) {
<<<<<<< HEAD
			s3c_hsotg_en_gsint(hsotg, GINTSTS_PTXFEMP);
=======
			dwc2_hsotg_en_gsint(hsotg, GINTSTS_PTXFEMP);
>>>>>>> v4.9.227
			return -ENOSPC;
		}

		dev_dbg(hsotg->dev, "%s: left=%d, load=%d, fifo=%d, size %d\n",
			__func__, size_left,
			hs_ep->size_loaded, hs_ep->fifo_load, hs_ep->fifo_size);

		/* how much of the data has moved */
		size_done = hs_ep->size_loaded - size_left;

		/* how much data is left in the fifo */
		can_write = hs_ep->fifo_load - size_done;
		dev_dbg(hsotg->dev, "%s: => can_write1=%d\n",
			__func__, can_write);

		can_write = hs_ep->fifo_size - can_write;
		dev_dbg(hsotg->dev, "%s: => can_write2=%d\n",
			__func__, can_write);

		if (can_write <= 0) {
<<<<<<< HEAD
			s3c_hsotg_en_gsint(hsotg, GINTSTS_PTXFEMP);
			return -ENOSPC;
		}
	} else if (hsotg->dedicated_fifos && hs_ep->index != 0) {
		can_write = readl(hsotg->regs + DTXFSTS(hs_ep->index));
=======
			dwc2_hsotg_en_gsint(hsotg, GINTSTS_PTXFEMP);
			return -ENOSPC;
		}
	} else if (hsotg->dedicated_fifos && hs_ep->index != 0) {
		can_write = dwc2_readl(hsotg->regs +
				DTXFSTS(hs_ep->fifo_index));
>>>>>>> v4.9.227

		can_write &= 0xffff;
		can_write *= 4;
	} else {
		if (GNPTXSTS_NP_TXQ_SPC_AVAIL_GET(gnptxsts) == 0) {
			dev_dbg(hsotg->dev,
				"%s: no queue slots available (0x%08x)\n",
				__func__, gnptxsts);

<<<<<<< HEAD
			s3c_hsotg_en_gsint(hsotg, GINTSTS_NPTXFEMP);
=======
			dwc2_hsotg_en_gsint(hsotg, GINTSTS_NPTXFEMP);
>>>>>>> v4.9.227
			return -ENOSPC;
		}

		can_write = GNPTXSTS_NP_TXF_SPC_AVAIL_GET(gnptxsts);
		can_write *= 4;	/* fifo size is in 32bit quantities. */
	}

	max_transfer = hs_ep->ep.maxpacket * hs_ep->mc;

	dev_dbg(hsotg->dev, "%s: GNPTXSTS=%08x, can=%d, to=%d, max_transfer %d\n",
		 __func__, gnptxsts, can_write, to_write, max_transfer);

	/*
	 * limit to 512 bytes of data, it seems at least on the non-periodic
	 * FIFO, requests of >512 cause the endpoint to get stuck with a
	 * fragment of the end of the transfer in it.
	 */
	if (can_write > 512 && !periodic)
		can_write = 512;

	/*
	 * limit the write to one max-packet size worth of data, but allow
	 * the transfer to return that it did not run out of fifo space
	 * doing it.
	 */
	if (to_write > max_transfer) {
		to_write = max_transfer;

		/* it's needed only when we do not use dedicated fifos */
		if (!hsotg->dedicated_fifos)
<<<<<<< HEAD
			s3c_hsotg_en_gsint(hsotg,
=======
			dwc2_hsotg_en_gsint(hsotg,
>>>>>>> v4.9.227
					   periodic ? GINTSTS_PTXFEMP :
					   GINTSTS_NPTXFEMP);
	}

	/* see if we can write data */

	if (to_write > can_write) {
		to_write = can_write;
		pkt_round = to_write % max_transfer;

		/*
		 * Round the write down to an
		 * exact number of packets.
		 *
		 * Note, we do not currently check to see if we can ever
		 * write a full packet or not to the FIFO.
		 */

		if (pkt_round)
			to_write -= pkt_round;

		/*
		 * enable correct FIFO interrupt to alert us when there
		 * is more room left.
		 */

		/* it's needed only when we do not use dedicated fifos */
		if (!hsotg->dedicated_fifos)
<<<<<<< HEAD
			s3c_hsotg_en_gsint(hsotg,
=======
			dwc2_hsotg_en_gsint(hsotg,
>>>>>>> v4.9.227
					   periodic ? GINTSTS_PTXFEMP :
					   GINTSTS_NPTXFEMP);
	}

	dev_dbg(hsotg->dev, "write %d/%d, can_write %d, done %d\n",
		 to_write, hs_req->req.length, can_write, buf_pos);

	if (to_write <= 0)
		return -ENOSPC;

	hs_req->req.actual = buf_pos + to_write;
	hs_ep->total_data += to_write;

	if (periodic)
		hs_ep->fifo_load += to_write;

	to_write = DIV_ROUND_UP(to_write, 4);
	data = hs_req->req.buf + buf_pos;

	iowrite32_rep(hsotg->regs + EPFIFO(hs_ep->index), data, to_write);

	return (to_write >= can_write) ? -ENOSPC : 0;
}

/**
 * get_ep_limit - get the maximum data legnth for this endpoint
 * @hs_ep: The endpoint
 *
 * Return the maximum data that can be queued in one go on a given endpoint
 * so that transfers that are too long can be split.
 */
<<<<<<< HEAD
static unsigned get_ep_limit(struct s3c_hsotg_ep *hs_ep)
=======
static unsigned get_ep_limit(struct dwc2_hsotg_ep *hs_ep)
>>>>>>> v4.9.227
{
	int index = hs_ep->index;
	unsigned maxsize;
	unsigned maxpkt;

	if (index != 0) {
		maxsize = DXEPTSIZ_XFERSIZE_LIMIT + 1;
		maxpkt = DXEPTSIZ_PKTCNT_LIMIT + 1;
	} else {
		maxsize = 64+64;
		if (hs_ep->dir_in)
			maxpkt = DIEPTSIZ0_PKTCNT_LIMIT + 1;
		else
			maxpkt = 2;
	}

	/* we made the constant loading easier above by using +1 */
	maxpkt--;
	maxsize--;

	/*
	 * constrain by packet count if maxpkts*pktsize is greater
	 * than the length register size.
	 */

	if ((maxpkt * hs_ep->ep.maxpacket) < maxsize)
		maxsize = maxpkt * hs_ep->ep.maxpacket;

	return maxsize;
}

/**
<<<<<<< HEAD
 * s3c_hsotg_start_req - start a USB request from an endpoint's queue
=======
* dwc2_hsotg_read_frameno - read current frame number
* @hsotg: The device instance
*
* Return the current frame number
*/
static u32 dwc2_hsotg_read_frameno(struct dwc2_hsotg *hsotg)
{
	u32 dsts;

	dsts = dwc2_readl(hsotg->regs + DSTS);
	dsts &= DSTS_SOFFN_MASK;
	dsts >>= DSTS_SOFFN_SHIFT;

	return dsts;
}

/**
 * dwc2_hsotg_start_req - start a USB request from an endpoint's queue
>>>>>>> v4.9.227
 * @hsotg: The controller state.
 * @hs_ep: The endpoint to process a request for
 * @hs_req: The request to start.
 * @continuing: True if we are doing more for the current request.
 *
 * Start the given request running by setting the endpoint registers
 * appropriately, and writing any data to the FIFOs.
 */
<<<<<<< HEAD
static void s3c_hsotg_start_req(struct s3c_hsotg *hsotg,
				struct s3c_hsotg_ep *hs_ep,
				struct s3c_hsotg_req *hs_req,
=======
static void dwc2_hsotg_start_req(struct dwc2_hsotg *hsotg,
				struct dwc2_hsotg_ep *hs_ep,
				struct dwc2_hsotg_req *hs_req,
>>>>>>> v4.9.227
				bool continuing)
{
	struct usb_request *ureq = &hs_req->req;
	int index = hs_ep->index;
	int dir_in = hs_ep->dir_in;
	u32 epctrl_reg;
	u32 epsize_reg;
	u32 epsize;
	u32 ctrl;
	unsigned length;
	unsigned packets;
	unsigned maxreq;

	if (index != 0) {
		if (hs_ep->req && !continuing) {
			dev_err(hsotg->dev, "%s: active request\n", __func__);
			WARN_ON(1);
			return;
		} else if (hs_ep->req != hs_req && continuing) {
			dev_err(hsotg->dev,
				"%s: continue different req\n", __func__);
			WARN_ON(1);
			return;
		}
	}

	epctrl_reg = dir_in ? DIEPCTL(index) : DOEPCTL(index);
	epsize_reg = dir_in ? DIEPTSIZ(index) : DOEPTSIZ(index);

	dev_dbg(hsotg->dev, "%s: DxEPCTL=0x%08x, ep %d, dir %s\n",
<<<<<<< HEAD
		__func__, readl(hsotg->regs + epctrl_reg), index,
		hs_ep->dir_in ? "in" : "out");

	/* If endpoint is stalled, we will restart request later */
	ctrl = readl(hsotg->regs + epctrl_reg);

	if (ctrl & DXEPCTL_STALL) {
=======
		__func__, dwc2_readl(hsotg->regs + epctrl_reg), index,
		hs_ep->dir_in ? "in" : "out");

	/* If endpoint is stalled, we will restart request later */
	ctrl = dwc2_readl(hsotg->regs + epctrl_reg);

	if (index && ctrl & DXEPCTL_STALL) {
>>>>>>> v4.9.227
		dev_warn(hsotg->dev, "%s: ep%d is stalled\n", __func__, index);
		return;
	}

	length = ureq->length - ureq->actual;
	dev_dbg(hsotg->dev, "ureq->length:%d ureq->actual:%d\n",
		ureq->length, ureq->actual);
<<<<<<< HEAD
	if (0)
		dev_dbg(hsotg->dev,
			"REQ buf %p len %d dma %pad noi=%d zp=%d snok=%d\n",
			ureq->buf, length, &ureq->dma,
			ureq->no_interrupt, ureq->zero, ureq->short_not_ok);
=======
>>>>>>> v4.9.227

	maxreq = get_ep_limit(hs_ep);
	if (length > maxreq) {
		int round = maxreq % hs_ep->ep.maxpacket;

		dev_dbg(hsotg->dev, "%s: length %d, max-req %d, r %d\n",
			__func__, length, maxreq, round);

		/* round down to multiple of packets */
		if (round)
			maxreq -= round;

		length = maxreq;
	}

	if (length)
		packets = DIV_ROUND_UP(length, hs_ep->ep.maxpacket);
	else
		packets = 1;	/* send one packet if length is zero. */

	if (hs_ep->isochronous && length > (hs_ep->mc * hs_ep->ep.maxpacket)) {
		dev_err(hsotg->dev, "req length > maxpacket*mc\n");
		return;
	}

	if (dir_in && index != 0)
		if (hs_ep->isochronous)
			epsize = DXEPTSIZ_MC(packets);
		else
			epsize = DXEPTSIZ_MC(1);
	else
		epsize = 0;

<<<<<<< HEAD
	if (index != 0 && ureq->zero) {
		/*
		 * test for the packets being exactly right for the
		 * transfer
		 */

		if (length == (packets * hs_ep->ep.maxpacket))
			packets++;
=======
	/*
	 * zero length packet should be programmed on its own and should not
	 * be counted in DIEPTSIZ.PktCnt with other packets.
	 */
	if (dir_in && ureq->zero && !continuing) {
		/* Test if zlp is actually required. */
		if ((ureq->length >= hs_ep->ep.maxpacket) &&
					!(ureq->length % hs_ep->ep.maxpacket))
			hs_ep->send_zlp = 1;
>>>>>>> v4.9.227
	}

	epsize |= DXEPTSIZ_PKTCNT(packets);
	epsize |= DXEPTSIZ_XFERSIZE(length);

	dev_dbg(hsotg->dev, "%s: %d@%d/%d, 0x%08x => 0x%08x\n",
		__func__, packets, length, ureq->length, epsize, epsize_reg);

	/* store the request as the current one we're doing */
	hs_ep->req = hs_req;

	/* write size / packets */
<<<<<<< HEAD
	writel(epsize, hsotg->regs + epsize_reg);
=======
	dwc2_writel(epsize, hsotg->regs + epsize_reg);
>>>>>>> v4.9.227

	if (using_dma(hsotg) && !continuing) {
		unsigned int dma_reg;

		/*
		 * write DMA address to control register, buffer already
<<<<<<< HEAD
		 * synced by s3c_hsotg_ep_queue().
		 */

		dma_reg = dir_in ? DIEPDMA(index) : DOEPDMA(index);
		writel(ureq->dma, hsotg->regs + dma_reg);
=======
		 * synced by dwc2_hsotg_ep_queue().
		 */

		dma_reg = dir_in ? DIEPDMA(index) : DOEPDMA(index);
		dwc2_writel(ureq->dma, hsotg->regs + dma_reg);
>>>>>>> v4.9.227

		dev_dbg(hsotg->dev, "%s: %pad => 0x%08x\n",
			__func__, &ureq->dma, dma_reg);
	}

<<<<<<< HEAD
	ctrl |= DXEPCTL_EPENA;	/* ensure ep enabled */
	ctrl |= DXEPCTL_USBACTEP;

	dev_dbg(hsotg->dev, "setup req:%d\n", hsotg->setup);

	/* For Setup request do not clear NAK */
	if (hsotg->setup && index == 0)
		hsotg->setup = 0;
	else
		ctrl |= DXEPCTL_CNAK;	/* clear NAK set by core */


	dev_dbg(hsotg->dev, "%s: DxEPCTL=0x%08x\n", __func__, ctrl);
	writel(ctrl, hsotg->regs + epctrl_reg);
=======
	if (hs_ep->isochronous && hs_ep->interval == 1) {
		hs_ep->target_frame = dwc2_hsotg_read_frameno(hsotg);
		dwc2_gadget_incr_frame_num(hs_ep);

		if (hs_ep->target_frame & 0x1)
			ctrl |= DXEPCTL_SETODDFR;
		else
			ctrl |= DXEPCTL_SETEVENFR;
	}

	ctrl |= DXEPCTL_EPENA;	/* ensure ep enabled */

	dev_dbg(hsotg->dev, "ep0 state:%d\n", hsotg->ep0_state);

	/* For Setup request do not clear NAK */
	if (!(index == 0 && hsotg->ep0_state == DWC2_EP0_SETUP))
		ctrl |= DXEPCTL_CNAK;	/* clear NAK set by core */

	dev_dbg(hsotg->dev, "%s: DxEPCTL=0x%08x\n", __func__, ctrl);
	dwc2_writel(ctrl, hsotg->regs + epctrl_reg);
>>>>>>> v4.9.227

	/*
	 * set these, it seems that DMA support increments past the end
	 * of the packet buffer so we need to calculate the length from
	 * this information.
	 */
	hs_ep->size_loaded = length;
	hs_ep->last_load = ureq->actual;

	if (dir_in && !using_dma(hsotg)) {
		/* set these anyway, we may need them for non-periodic in */
		hs_ep->fifo_load = 0;

<<<<<<< HEAD
		s3c_hsotg_write_fifo(hsotg, hs_ep, hs_req);
	}

	/*
	 * clear the INTknTXFEmpMsk when we start request, more as a aide
	 * to debugging to see what is going on.
	 */
	if (dir_in)
		writel(DIEPMSK_INTKNTXFEMPMSK,
		       hsotg->regs + DIEPINT(index));

	/*
=======
		dwc2_hsotg_write_fifo(hsotg, hs_ep, hs_req);
	}

	/*
>>>>>>> v4.9.227
	 * Note, trying to clear the NAK here causes problems with transmit
	 * on the S3C6400 ending up with the TXFIFO becoming full.
	 */

	/* check ep is enabled */
<<<<<<< HEAD
	if (!(readl(hsotg->regs + epctrl_reg) & DXEPCTL_EPENA))
		dev_warn(hsotg->dev,
			 "ep%d: failed to become enabled (DXEPCTL=0x%08x)?\n",
			 index, readl(hsotg->regs + epctrl_reg));

	dev_dbg(hsotg->dev, "%s: DXEPCTL=0x%08x\n",
		__func__, readl(hsotg->regs + epctrl_reg));

	/* enable ep interrupts */
	s3c_hsotg_ctrl_epint(hsotg, hs_ep->index, hs_ep->dir_in, 1);
}

/**
 * s3c_hsotg_map_dma - map the DMA memory being used for the request
=======
	if (!(dwc2_readl(hsotg->regs + epctrl_reg) & DXEPCTL_EPENA))
		dev_dbg(hsotg->dev,
			 "ep%d: failed to become enabled (DXEPCTL=0x%08x)?\n",
			 index, dwc2_readl(hsotg->regs + epctrl_reg));

	dev_dbg(hsotg->dev, "%s: DXEPCTL=0x%08x\n",
		__func__, dwc2_readl(hsotg->regs + epctrl_reg));

	/* enable ep interrupts */
	dwc2_hsotg_ctrl_epint(hsotg, hs_ep->index, hs_ep->dir_in, 1);
}

/**
 * dwc2_hsotg_map_dma - map the DMA memory being used for the request
>>>>>>> v4.9.227
 * @hsotg: The device state.
 * @hs_ep: The endpoint the request is on.
 * @req: The request being processed.
 *
 * We've been asked to queue a request, so ensure that the memory buffer
 * is correctly setup for DMA. If we've been passed an extant DMA address
 * then ensure the buffer has been synced to memory. If our buffer has no
 * DMA memory, then we map the memory and mark our request to allow us to
 * cleanup on completion.
 */
<<<<<<< HEAD
static int s3c_hsotg_map_dma(struct s3c_hsotg *hsotg,
			     struct s3c_hsotg_ep *hs_ep,
			     struct usb_request *req)
{
	struct s3c_hsotg_req *hs_req = our_req(req);
=======
static int dwc2_hsotg_map_dma(struct dwc2_hsotg *hsotg,
			     struct dwc2_hsotg_ep *hs_ep,
			     struct usb_request *req)
{
	struct dwc2_hsotg_req *hs_req = our_req(req);
>>>>>>> v4.9.227
	int ret;

	/* if the length is zero, ignore the DMA data */
	if (hs_req->req.length == 0)
		return 0;

	ret = usb_gadget_map_request(&hsotg->gadget, req, hs_ep->dir_in);
	if (ret)
		goto dma_error;

	return 0;

dma_error:
	dev_err(hsotg->dev, "%s: failed to map buffer %p, %d bytes\n",
		__func__, req->buf, req->length);

	return -EIO;
}

<<<<<<< HEAD
static int s3c_hsotg_ep_queue(struct usb_ep *ep, struct usb_request *req,
			      gfp_t gfp_flags)
{
	struct s3c_hsotg_req *hs_req = our_req(req);
	struct s3c_hsotg_ep *hs_ep = our_ep(ep);
	struct s3c_hsotg *hs = hs_ep->parent;
	bool first;
=======
static int dwc2_hsotg_handle_unaligned_buf_start(struct dwc2_hsotg *hsotg,
	struct dwc2_hsotg_ep *hs_ep, struct dwc2_hsotg_req *hs_req)
{
	void *req_buf = hs_req->req.buf;

	/* If dma is not being used or buffer is aligned */
	if (!using_dma(hsotg) || !((long)req_buf & 3))
		return 0;

	WARN_ON(hs_req->saved_req_buf);

	dev_dbg(hsotg->dev, "%s: %s: buf=%p length=%d\n", __func__,
			hs_ep->ep.name, req_buf, hs_req->req.length);

	hs_req->req.buf = kmalloc(hs_req->req.length, GFP_ATOMIC);
	if (!hs_req->req.buf) {
		hs_req->req.buf = req_buf;
		dev_err(hsotg->dev,
			"%s: unable to allocate memory for bounce buffer\n",
			__func__);
		return -ENOMEM;
	}

	/* Save actual buffer */
	hs_req->saved_req_buf = req_buf;

	if (hs_ep->dir_in)
		memcpy(hs_req->req.buf, req_buf, hs_req->req.length);
	return 0;
}

static void dwc2_hsotg_handle_unaligned_buf_complete(struct dwc2_hsotg *hsotg,
	struct dwc2_hsotg_ep *hs_ep, struct dwc2_hsotg_req *hs_req)
{
	/* If dma is not being used or buffer was aligned */
	if (!using_dma(hsotg) || !hs_req->saved_req_buf)
		return;

	dev_dbg(hsotg->dev, "%s: %s: status=%d actual-length=%d\n", __func__,
		hs_ep->ep.name, hs_req->req.status, hs_req->req.actual);

	/* Copy data from bounce buffer on successful out transfer */
	if (!hs_ep->dir_in && !hs_req->req.status)
		memcpy(hs_req->saved_req_buf, hs_req->req.buf,
							hs_req->req.actual);

	/* Free bounce buffer */
	kfree(hs_req->req.buf);

	hs_req->req.buf = hs_req->saved_req_buf;
	hs_req->saved_req_buf = NULL;
}

/**
 * dwc2_gadget_target_frame_elapsed - Checks target frame
 * @hs_ep: The driver endpoint to check
 *
 * Returns 1 if targeted frame elapsed. If returned 1 then we need to drop
 * corresponding transfer.
 */
static bool dwc2_gadget_target_frame_elapsed(struct dwc2_hsotg_ep *hs_ep)
{
	struct dwc2_hsotg *hsotg = hs_ep->parent;
	u32 target_frame = hs_ep->target_frame;
	u32 current_frame = dwc2_hsotg_read_frameno(hsotg);
	bool frame_overrun = hs_ep->frame_overrun;

	if (!frame_overrun && current_frame >= target_frame)
		return true;

	if (frame_overrun && current_frame >= target_frame &&
	    ((current_frame - target_frame) < DSTS_SOFFN_LIMIT / 2))
		return true;

	return false;
}

static int dwc2_hsotg_ep_queue(struct usb_ep *ep, struct usb_request *req,
			      gfp_t gfp_flags)
{
	struct dwc2_hsotg_req *hs_req = our_req(req);
	struct dwc2_hsotg_ep *hs_ep = our_ep(ep);
	struct dwc2_hsotg *hs = hs_ep->parent;
	bool first;
	int ret;
>>>>>>> v4.9.227

	dev_dbg(hs->dev, "%s: req %p: %d@%p, noi=%d, zero=%d, snok=%d\n",
		ep->name, req, req->length, req->buf, req->no_interrupt,
		req->zero, req->short_not_ok);

<<<<<<< HEAD
=======
	/* Prevent new request submission when controller is suspended */
	if (hs->lx_state == DWC2_L2) {
		dev_dbg(hs->dev, "%s: don't submit request while suspended\n",
				__func__);
		return -EAGAIN;
	}

>>>>>>> v4.9.227
	/* initialise status of the request */
	INIT_LIST_HEAD(&hs_req->queue);
	req->actual = 0;
	req->status = -EINPROGRESS;

<<<<<<< HEAD
	/* if we're using DMA, sync the buffers as necessary */
	if (using_dma(hs)) {
		int ret = s3c_hsotg_map_dma(hs, hs_ep, req);
=======
	ret = dwc2_hsotg_handle_unaligned_buf_start(hs, hs_ep, hs_req);
	if (ret)
		return ret;

	/* if we're using DMA, sync the buffers as necessary */
	if (using_dma(hs)) {
		ret = dwc2_hsotg_map_dma(hs, hs_ep, req);
>>>>>>> v4.9.227
		if (ret)
			return ret;
	}

	first = list_empty(&hs_ep->queue);
	list_add_tail(&hs_req->queue, &hs_ep->queue);

<<<<<<< HEAD
	if (first)
		s3c_hsotg_start_req(hs, hs_ep, hs_req, false);

	return 0;
}

static int s3c_hsotg_ep_queue_lock(struct usb_ep *ep, struct usb_request *req,
			      gfp_t gfp_flags)
{
	struct s3c_hsotg_ep *hs_ep = our_ep(ep);
	struct s3c_hsotg *hs = hs_ep->parent;
=======
	if (first) {
		if (!hs_ep->isochronous) {
			dwc2_hsotg_start_req(hs, hs_ep, hs_req, false);
			return 0;
		}

		while (dwc2_gadget_target_frame_elapsed(hs_ep))
			dwc2_gadget_incr_frame_num(hs_ep);

		if (hs_ep->target_frame != TARGET_FRAME_INITIAL)
			dwc2_hsotg_start_req(hs, hs_ep, hs_req, false);
	}
	return 0;
}

static int dwc2_hsotg_ep_queue_lock(struct usb_ep *ep, struct usb_request *req,
			      gfp_t gfp_flags)
{
	struct dwc2_hsotg_ep *hs_ep = our_ep(ep);
	struct dwc2_hsotg *hs = hs_ep->parent;
>>>>>>> v4.9.227
	unsigned long flags = 0;
	int ret = 0;

	spin_lock_irqsave(&hs->lock, flags);
<<<<<<< HEAD
	ret = s3c_hsotg_ep_queue(ep, req, gfp_flags);
=======
	ret = dwc2_hsotg_ep_queue(ep, req, gfp_flags);
>>>>>>> v4.9.227
	spin_unlock_irqrestore(&hs->lock, flags);

	return ret;
}

<<<<<<< HEAD
static void s3c_hsotg_ep_free_request(struct usb_ep *ep,
				      struct usb_request *req)
{
	struct s3c_hsotg_req *hs_req = our_req(req);
=======
static void dwc2_hsotg_ep_free_request(struct usb_ep *ep,
				      struct usb_request *req)
{
	struct dwc2_hsotg_req *hs_req = our_req(req);
>>>>>>> v4.9.227

	kfree(hs_req);
}

/**
<<<<<<< HEAD
 * s3c_hsotg_complete_oursetup - setup completion callback
=======
 * dwc2_hsotg_complete_oursetup - setup completion callback
>>>>>>> v4.9.227
 * @ep: The endpoint the request was on.
 * @req: The request completed.
 *
 * Called on completion of any requests the driver itself
 * submitted that need cleaning up.
 */
<<<<<<< HEAD
static void s3c_hsotg_complete_oursetup(struct usb_ep *ep,
					struct usb_request *req)
{
	struct s3c_hsotg_ep *hs_ep = our_ep(ep);
	struct s3c_hsotg *hsotg = hs_ep->parent;

	dev_dbg(hsotg->dev, "%s: ep %p, req %p\n", __func__, ep, req);

	s3c_hsotg_ep_free_request(ep, req);
=======
static void dwc2_hsotg_complete_oursetup(struct usb_ep *ep,
					struct usb_request *req)
{
	struct dwc2_hsotg_ep *hs_ep = our_ep(ep);
	struct dwc2_hsotg *hsotg = hs_ep->parent;

	dev_dbg(hsotg->dev, "%s: ep %p, req %p\n", __func__, ep, req);

	dwc2_hsotg_ep_free_request(ep, req);
>>>>>>> v4.9.227
}

/**
 * ep_from_windex - convert control wIndex value to endpoint
 * @hsotg: The driver state.
 * @windex: The control request wIndex field (in host order).
 *
 * Convert the given wIndex into a pointer to an driver endpoint
 * structure, or return NULL if it is not a valid endpoint.
 */
<<<<<<< HEAD
static struct s3c_hsotg_ep *ep_from_windex(struct s3c_hsotg *hsotg,
					   u32 windex)
{
	struct s3c_hsotg_ep *ep = &hsotg->eps[windex & 0x7F];
=======
static struct dwc2_hsotg_ep *ep_from_windex(struct dwc2_hsotg *hsotg,
					   u32 windex)
{
	struct dwc2_hsotg_ep *ep;
>>>>>>> v4.9.227
	int dir = (windex & USB_DIR_IN) ? 1 : 0;
	int idx = windex & 0x7F;

	if (windex >= 0x100)
		return NULL;

	if (idx > hsotg->num_of_eps)
		return NULL;

<<<<<<< HEAD
=======
	ep = index_to_ep(hsotg, idx, dir);

>>>>>>> v4.9.227
	if (idx && ep->dir_in != dir)
		return NULL;

	return ep;
}

/**
<<<<<<< HEAD
 * s3c_hsotg_send_reply - send reply to control request
=======
 * dwc2_hsotg_set_test_mode - Enable usb Test Modes
 * @hsotg: The driver state.
 * @testmode: requested usb test mode
 * Enable usb Test Mode requested by the Host.
 */
int dwc2_hsotg_set_test_mode(struct dwc2_hsotg *hsotg, int testmode)
{
	int dctl = dwc2_readl(hsotg->regs + DCTL);

	dctl &= ~DCTL_TSTCTL_MASK;
	switch (testmode) {
	case TEST_J:
	case TEST_K:
	case TEST_SE0_NAK:
	case TEST_PACKET:
	case TEST_FORCE_EN:
		dctl |= testmode << DCTL_TSTCTL_SHIFT;
		break;
	default:
		return -EINVAL;
	}
	dwc2_writel(dctl, hsotg->regs + DCTL);
	return 0;
}

/**
 * dwc2_hsotg_send_reply - send reply to control request
>>>>>>> v4.9.227
 * @hsotg: The device state
 * @ep: Endpoint 0
 * @buff: Buffer for request
 * @length: Length of reply.
 *
 * Create a request and queue it on the given endpoint. This is useful as
 * an internal method of sending replies to certain control requests, etc.
 */
<<<<<<< HEAD
static int s3c_hsotg_send_reply(struct s3c_hsotg *hsotg,
				struct s3c_hsotg_ep *ep,
=======
static int dwc2_hsotg_send_reply(struct dwc2_hsotg *hsotg,
				struct dwc2_hsotg_ep *ep,
>>>>>>> v4.9.227
				void *buff,
				int length)
{
	struct usb_request *req;
	int ret;

	dev_dbg(hsotg->dev, "%s: buff %p, len %d\n", __func__, buff, length);

<<<<<<< HEAD
	req = s3c_hsotg_ep_alloc_request(&ep->ep, GFP_ATOMIC);
=======
	req = dwc2_hsotg_ep_alloc_request(&ep->ep, GFP_ATOMIC);
>>>>>>> v4.9.227
	hsotg->ep0_reply = req;
	if (!req) {
		dev_warn(hsotg->dev, "%s: cannot alloc req\n", __func__);
		return -ENOMEM;
	}

	req->buf = hsotg->ep0_buff;
	req->length = length;
<<<<<<< HEAD
	req->zero = 1; /* always do zero-length final transfer */
	req->complete = s3c_hsotg_complete_oursetup;

	if (length)
		memcpy(req->buf, buff, length);
	else
		ep->sent_zlp = 1;

	ret = s3c_hsotg_ep_queue(&ep->ep, req, GFP_ATOMIC);
=======
	/*
	 * zero flag is for sending zlp in DATA IN stage. It has no impact on
	 * STATUS stage.
	 */
	req->zero = 0;
	req->complete = dwc2_hsotg_complete_oursetup;

	if (length)
		memcpy(req->buf, buff, length);

	ret = dwc2_hsotg_ep_queue(&ep->ep, req, GFP_ATOMIC);
>>>>>>> v4.9.227
	if (ret) {
		dev_warn(hsotg->dev, "%s: cannot queue req\n", __func__);
		return ret;
	}

	return 0;
}

/**
<<<<<<< HEAD
 * s3c_hsotg_process_req_status - process request GET_STATUS
 * @hsotg: The device state
 * @ctrl: USB control request
 */
static int s3c_hsotg_process_req_status(struct s3c_hsotg *hsotg,
					struct usb_ctrlrequest *ctrl)
{
	struct s3c_hsotg_ep *ep0 = &hsotg->eps[0];
	struct s3c_hsotg_ep *ep;
=======
 * dwc2_hsotg_process_req_status - process request GET_STATUS
 * @hsotg: The device state
 * @ctrl: USB control request
 */
static int dwc2_hsotg_process_req_status(struct dwc2_hsotg *hsotg,
					struct usb_ctrlrequest *ctrl)
{
	struct dwc2_hsotg_ep *ep0 = hsotg->eps_out[0];
	struct dwc2_hsotg_ep *ep;
>>>>>>> v4.9.227
	__le16 reply;
	int ret;

	dev_dbg(hsotg->dev, "%s: USB_REQ_GET_STATUS\n", __func__);

	if (!ep0->dir_in) {
		dev_warn(hsotg->dev, "%s: direction out?\n", __func__);
		return -EINVAL;
	}

	switch (ctrl->bRequestType & USB_RECIP_MASK) {
	case USB_RECIP_DEVICE:
		reply = cpu_to_le16(0); /* bit 0 => self powered,
					 * bit 1 => remote wakeup */
		break;

	case USB_RECIP_INTERFACE:
		/* currently, the data result should be zero */
		reply = cpu_to_le16(0);
		break;

	case USB_RECIP_ENDPOINT:
		ep = ep_from_windex(hsotg, le16_to_cpu(ctrl->wIndex));
		if (!ep)
			return -ENOENT;

		reply = cpu_to_le16(ep->halted ? 1 : 0);
		break;

	default:
		return 0;
	}

	if (le16_to_cpu(ctrl->wLength) != 2)
		return -EINVAL;

<<<<<<< HEAD
	ret = s3c_hsotg_send_reply(hsotg, ep0, &reply, 2);
=======
	ret = dwc2_hsotg_send_reply(hsotg, ep0, &reply, 2);
>>>>>>> v4.9.227
	if (ret) {
		dev_err(hsotg->dev, "%s: failed to send reply\n", __func__);
		return ret;
	}

	return 1;
}

<<<<<<< HEAD
static int s3c_hsotg_ep_sethalt(struct usb_ep *ep, int value);
=======
static int dwc2_hsotg_ep_sethalt(struct usb_ep *ep, int value, bool now);
>>>>>>> v4.9.227

/**
 * get_ep_head - return the first request on the endpoint
 * @hs_ep: The controller endpoint to get
 *
 * Get the first request on the endpoint.
 */
<<<<<<< HEAD
static struct s3c_hsotg_req *get_ep_head(struct s3c_hsotg_ep *hs_ep)
=======
static struct dwc2_hsotg_req *get_ep_head(struct dwc2_hsotg_ep *hs_ep)
>>>>>>> v4.9.227
{
	if (list_empty(&hs_ep->queue))
		return NULL;

<<<<<<< HEAD
	return list_first_entry(&hs_ep->queue, struct s3c_hsotg_req, queue);
}

/**
 * s3c_hsotg_process_req_featire - process request {SET,CLEAR}_FEATURE
 * @hsotg: The device state
 * @ctrl: USB control request
 */
static int s3c_hsotg_process_req_feature(struct s3c_hsotg *hsotg,
					 struct usb_ctrlrequest *ctrl)
{
	struct s3c_hsotg_ep *ep0 = &hsotg->eps[0];
	struct s3c_hsotg_req *hs_req;
	bool restart;
	bool set = (ctrl->bRequest == USB_REQ_SET_FEATURE);
	struct s3c_hsotg_ep *ep;
	int ret;
	bool halted;
=======
	return list_first_entry(&hs_ep->queue, struct dwc2_hsotg_req, queue);
}

/**
 * dwc2_gadget_start_next_request - Starts next request from ep queue
 * @hs_ep: Endpoint structure
 *
 * If queue is empty and EP is ISOC-OUT - unmasks OUTTKNEPDIS which is masked
 * in its handler. Hence we need to unmask it here to be able to do
 * resynchronization.
 */
static void dwc2_gadget_start_next_request(struct dwc2_hsotg_ep *hs_ep)
{
	u32 mask;
	struct dwc2_hsotg *hsotg = hs_ep->parent;
	int dir_in = hs_ep->dir_in;
	struct dwc2_hsotg_req *hs_req;
	u32 epmsk_reg = dir_in ? DIEPMSK : DOEPMSK;

	if (!list_empty(&hs_ep->queue)) {
		hs_req = get_ep_head(hs_ep);
		dwc2_hsotg_start_req(hsotg, hs_ep, hs_req, false);
		return;
	}
	if (!hs_ep->isochronous)
		return;

	if (dir_in) {
		dev_dbg(hsotg->dev, "%s: No more ISOC-IN requests\n",
			__func__);
	} else {
		dev_dbg(hsotg->dev, "%s: No more ISOC-OUT requests\n",
			__func__);
		mask = dwc2_readl(hsotg->regs + epmsk_reg);
		mask |= DOEPMSK_OUTTKNEPDISMSK;
		dwc2_writel(mask, hsotg->regs + epmsk_reg);
	}
}

/**
 * dwc2_hsotg_process_req_feature - process request {SET,CLEAR}_FEATURE
 * @hsotg: The device state
 * @ctrl: USB control request
 */
static int dwc2_hsotg_process_req_feature(struct dwc2_hsotg *hsotg,
					 struct usb_ctrlrequest *ctrl)
{
	struct dwc2_hsotg_ep *ep0 = hsotg->eps_out[0];
	struct dwc2_hsotg_req *hs_req;
	bool set = (ctrl->bRequest == USB_REQ_SET_FEATURE);
	struct dwc2_hsotg_ep *ep;
	int ret;
	bool halted;
	u32 recip;
	u32 wValue;
	u32 wIndex;
>>>>>>> v4.9.227

	dev_dbg(hsotg->dev, "%s: %s_FEATURE\n",
		__func__, set ? "SET" : "CLEAR");

<<<<<<< HEAD
	if (ctrl->bRequestType == USB_RECIP_ENDPOINT) {
		ep = ep_from_windex(hsotg, le16_to_cpu(ctrl->wIndex));
		if (!ep) {
			dev_dbg(hsotg->dev, "%s: no endpoint for 0x%04x\n",
				__func__, le16_to_cpu(ctrl->wIndex));
			return -ENOENT;
		}

		switch (le16_to_cpu(ctrl->wValue)) {
		case USB_ENDPOINT_HALT:
			halted = ep->halted;

			s3c_hsotg_ep_sethalt(&ep->ep, set);

			ret = s3c_hsotg_send_reply(hsotg, ep0, NULL, 0);
=======
	wValue = le16_to_cpu(ctrl->wValue);
	wIndex = le16_to_cpu(ctrl->wIndex);
	recip = ctrl->bRequestType & USB_RECIP_MASK;

	switch (recip) {
	case USB_RECIP_DEVICE:
		switch (wValue) {
		case USB_DEVICE_TEST_MODE:
			if ((wIndex & 0xff) != 0)
				return -EINVAL;
			if (!set)
				return -EINVAL;

			hsotg->test_mode = wIndex >> 8;
			ret = dwc2_hsotg_send_reply(hsotg, ep0, NULL, 0);
			if (ret) {
				dev_err(hsotg->dev,
					"%s: failed to send reply\n", __func__);
				return ret;
			}
			break;
		default:
			return -ENOENT;
		}
		break;

	case USB_RECIP_ENDPOINT:
		ep = ep_from_windex(hsotg, wIndex);
		if (!ep) {
			dev_dbg(hsotg->dev, "%s: no endpoint for 0x%04x\n",
				__func__, wIndex);
			return -ENOENT;
		}

		switch (wValue) {
		case USB_ENDPOINT_HALT:
			halted = ep->halted;

			dwc2_hsotg_ep_sethalt(&ep->ep, set, true);

			ret = dwc2_hsotg_send_reply(hsotg, ep0, NULL, 0);
>>>>>>> v4.9.227
			if (ret) {
				dev_err(hsotg->dev,
					"%s: failed to send reply\n", __func__);
				return ret;
			}

			/*
			 * we have to complete all requests for ep if it was
			 * halted, and the halt was cleared by CLEAR_FEATURE
			 */

			if (!set && halted) {
				/*
				 * If we have request in progress,
				 * then complete it
				 */
				if (ep->req) {
					hs_req = ep->req;
					ep->req = NULL;
					list_del_init(&hs_req->queue);
<<<<<<< HEAD
					usb_gadget_giveback_request(&ep->ep,
								    &hs_req->req);
				}

				/* If we have pending request, then start it */
				restart = !list_empty(&ep->queue);
				if (restart) {
					hs_req = get_ep_head(ep);
					s3c_hsotg_start_req(hsotg, ep,
							    hs_req, false);
=======
					if (hs_req->req.complete) {
						spin_unlock(&hsotg->lock);
						usb_gadget_giveback_request(
							&ep->ep, &hs_req->req);
						spin_lock(&hsotg->lock);
					}
				}

				/* If we have pending request, then start it */
				if (!ep->req) {
					dwc2_gadget_start_next_request(ep);
>>>>>>> v4.9.227
				}
			}

			break;

		default:
			return -ENOENT;
		}
<<<<<<< HEAD
	} else
		return -ENOENT;  /* currently only deal with endpoint */

	return 1;
}

static void s3c_hsotg_enqueue_setup(struct s3c_hsotg *hsotg);
static void s3c_hsotg_disconnect(struct s3c_hsotg *hsotg);

/**
 * s3c_hsotg_stall_ep0 - stall ep0
=======
		break;
	default:
		return -ENOENT;
	}
	return 1;
}

static void dwc2_hsotg_enqueue_setup(struct dwc2_hsotg *hsotg);

/**
 * dwc2_hsotg_stall_ep0 - stall ep0
>>>>>>> v4.9.227
 * @hsotg: The device state
 *
 * Set stall for ep0 as response for setup request.
 */
<<<<<<< HEAD
static void s3c_hsotg_stall_ep0(struct s3c_hsotg *hsotg)
{
	struct s3c_hsotg_ep *ep0 = &hsotg->eps[0];
=======
static void dwc2_hsotg_stall_ep0(struct dwc2_hsotg *hsotg)
{
	struct dwc2_hsotg_ep *ep0 = hsotg->eps_out[0];
>>>>>>> v4.9.227
	u32 reg;
	u32 ctrl;

	dev_dbg(hsotg->dev, "ep0 stall (dir=%d)\n", ep0->dir_in);
	reg = (ep0->dir_in) ? DIEPCTL0 : DOEPCTL0;

	/*
	 * DxEPCTL_Stall will be cleared by EP once it has
	 * taken effect, so no need to clear later.
	 */

<<<<<<< HEAD
	ctrl = readl(hsotg->regs + reg);
	ctrl |= DXEPCTL_STALL;
	ctrl |= DXEPCTL_CNAK;
	writel(ctrl, hsotg->regs + reg);

	dev_dbg(hsotg->dev,
		"written DXEPCTL=0x%08x to %08x (DXEPCTL=0x%08x)\n",
		ctrl, reg, readl(hsotg->regs + reg));
=======
	ctrl = dwc2_readl(hsotg->regs + reg);
	ctrl |= DXEPCTL_STALL;
	ctrl |= DXEPCTL_CNAK;
	dwc2_writel(ctrl, hsotg->regs + reg);

	dev_dbg(hsotg->dev,
		"written DXEPCTL=0x%08x to %08x (DXEPCTL=0x%08x)\n",
		ctrl, reg, dwc2_readl(hsotg->regs + reg));
>>>>>>> v4.9.227

	 /*
	  * complete won't be called, so we enqueue
	  * setup request here
	  */
<<<<<<< HEAD
	 s3c_hsotg_enqueue_setup(hsotg);
}

/**
 * s3c_hsotg_process_control - process a control request
=======
	 dwc2_hsotg_enqueue_setup(hsotg);
}

/**
 * dwc2_hsotg_process_control - process a control request
>>>>>>> v4.9.227
 * @hsotg: The device state
 * @ctrl: The control request received
 *
 * The controller has received the SETUP phase of a control request, and
 * needs to work out what to do next (and whether to pass it on to the
 * gadget driver).
 */
<<<<<<< HEAD
static void s3c_hsotg_process_control(struct s3c_hsotg *hsotg,
				      struct usb_ctrlrequest *ctrl)
{
	struct s3c_hsotg_ep *ep0 = &hsotg->eps[0];
	int ret = 0;
	u32 dcfg;

	ep0->sent_zlp = 0;

	dev_dbg(hsotg->dev, "ctrl Req=%02x, Type=%02x, V=%04x, L=%04x\n",
		 ctrl->bRequest, ctrl->bRequestType,
		 ctrl->wValue, ctrl->wLength);

	/*
	 * record the direction of the request, for later use when enquing
	 * packets onto EP0.
	 */

	ep0->dir_in = (ctrl->bRequestType & USB_DIR_IN) ? 1 : 0;
	dev_dbg(hsotg->dev, "ctrl: dir_in=%d\n", ep0->dir_in);

	/*
	 * if we've no data with this request, then the last part of the
	 * transaction is going to implicitly be IN.
	 */
	if (ctrl->wLength == 0)
		ep0->dir_in = 1;
=======
static void dwc2_hsotg_process_control(struct dwc2_hsotg *hsotg,
				      struct usb_ctrlrequest *ctrl)
{
	struct dwc2_hsotg_ep *ep0 = hsotg->eps_out[0];
	int ret = 0;
	u32 dcfg;

	dev_dbg(hsotg->dev,
		"ctrl Type=%02x, Req=%02x, V=%04x, I=%04x, L=%04x\n",
		ctrl->bRequestType, ctrl->bRequest, ctrl->wValue,
		ctrl->wIndex, ctrl->wLength);

	if (ctrl->wLength == 0) {
		ep0->dir_in = 1;
		hsotg->ep0_state = DWC2_EP0_STATUS_IN;
	} else if (ctrl->bRequestType & USB_DIR_IN) {
		ep0->dir_in = 1;
		hsotg->ep0_state = DWC2_EP0_DATA_IN;
	} else {
		ep0->dir_in = 0;
		hsotg->ep0_state = DWC2_EP0_DATA_OUT;
	}
>>>>>>> v4.9.227

	if ((ctrl->bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
		switch (ctrl->bRequest) {
		case USB_REQ_SET_ADDRESS:
<<<<<<< HEAD
			s3c_hsotg_disconnect(hsotg);
			dcfg = readl(hsotg->regs + DCFG);
			dcfg &= ~DCFG_DEVADDR_MASK;
			dcfg |= (le16_to_cpu(ctrl->wValue) <<
				 DCFG_DEVADDR_SHIFT) & DCFG_DEVADDR_MASK;
			writel(dcfg, hsotg->regs + DCFG);

			dev_info(hsotg->dev, "new address %d\n", ctrl->wValue);

			ret = s3c_hsotg_send_reply(hsotg, ep0, NULL, 0);
			return;

		case USB_REQ_GET_STATUS:
			ret = s3c_hsotg_process_req_status(hsotg, ctrl);
=======
			hsotg->connected = 1;
			dcfg = dwc2_readl(hsotg->regs + DCFG);
			dcfg &= ~DCFG_DEVADDR_MASK;
			dcfg |= (le16_to_cpu(ctrl->wValue) <<
				 DCFG_DEVADDR_SHIFT) & DCFG_DEVADDR_MASK;
			dwc2_writel(dcfg, hsotg->regs + DCFG);

			dev_info(hsotg->dev, "new address %d\n", ctrl->wValue);

			ret = dwc2_hsotg_send_reply(hsotg, ep0, NULL, 0);
			return;

		case USB_REQ_GET_STATUS:
			ret = dwc2_hsotg_process_req_status(hsotg, ctrl);
>>>>>>> v4.9.227
			break;

		case USB_REQ_CLEAR_FEATURE:
		case USB_REQ_SET_FEATURE:
<<<<<<< HEAD
			ret = s3c_hsotg_process_req_feature(hsotg, ctrl);
=======
			ret = dwc2_hsotg_process_req_feature(hsotg, ctrl);
>>>>>>> v4.9.227
			break;
		}
	}

	/* as a fallback, try delivering it to the driver to deal with */

	if (ret == 0 && hsotg->driver) {
		spin_unlock(&hsotg->lock);
		ret = hsotg->driver->setup(&hsotg->gadget, ctrl);
		spin_lock(&hsotg->lock);
		if (ret < 0)
			dev_dbg(hsotg->dev, "driver->setup() ret %d\n", ret);
	}

	/*
	 * the request is either unhandlable, or is not formatted correctly
	 * so respond with a STALL for the status stage to indicate failure.
	 */

	if (ret < 0)
<<<<<<< HEAD
		s3c_hsotg_stall_ep0(hsotg);
}

/**
 * s3c_hsotg_complete_setup - completion of a setup transfer
=======
		dwc2_hsotg_stall_ep0(hsotg);
}

/**
 * dwc2_hsotg_complete_setup - completion of a setup transfer
>>>>>>> v4.9.227
 * @ep: The endpoint the request was on.
 * @req: The request completed.
 *
 * Called on completion of any requests the driver itself submitted for
 * EP0 setup packets
 */
<<<<<<< HEAD
static void s3c_hsotg_complete_setup(struct usb_ep *ep,
				     struct usb_request *req)
{
	struct s3c_hsotg_ep *hs_ep = our_ep(ep);
	struct s3c_hsotg *hsotg = hs_ep->parent;
=======
static void dwc2_hsotg_complete_setup(struct usb_ep *ep,
				     struct usb_request *req)
{
	struct dwc2_hsotg_ep *hs_ep = our_ep(ep);
	struct dwc2_hsotg *hsotg = hs_ep->parent;
>>>>>>> v4.9.227

	if (req->status < 0) {
		dev_dbg(hsotg->dev, "%s: failed %d\n", __func__, req->status);
		return;
	}

	spin_lock(&hsotg->lock);
	if (req->actual == 0)
<<<<<<< HEAD
		s3c_hsotg_enqueue_setup(hsotg);
	else
		s3c_hsotg_process_control(hsotg, req->buf);
=======
		dwc2_hsotg_enqueue_setup(hsotg);
	else
		dwc2_hsotg_process_control(hsotg, req->buf);
>>>>>>> v4.9.227
	spin_unlock(&hsotg->lock);
}

/**
<<<<<<< HEAD
 * s3c_hsotg_enqueue_setup - start a request for EP0 packets
=======
 * dwc2_hsotg_enqueue_setup - start a request for EP0 packets
>>>>>>> v4.9.227
 * @hsotg: The device state.
 *
 * Enqueue a request on EP0 if necessary to received any SETUP packets
 * received from the host.
 */
<<<<<<< HEAD
static void s3c_hsotg_enqueue_setup(struct s3c_hsotg *hsotg)
{
	struct usb_request *req = hsotg->ctrl_req;
	struct s3c_hsotg_req *hs_req = our_req(req);
=======
static void dwc2_hsotg_enqueue_setup(struct dwc2_hsotg *hsotg)
{
	struct usb_request *req = hsotg->ctrl_req;
	struct dwc2_hsotg_req *hs_req = our_req(req);
>>>>>>> v4.9.227
	int ret;

	dev_dbg(hsotg->dev, "%s: queueing setup request\n", __func__);

	req->zero = 0;
	req->length = 8;
	req->buf = hsotg->ctrl_buff;
<<<<<<< HEAD
	req->complete = s3c_hsotg_complete_setup;
=======
	req->complete = dwc2_hsotg_complete_setup;
>>>>>>> v4.9.227

	if (!list_empty(&hs_req->queue)) {
		dev_dbg(hsotg->dev, "%s already queued???\n", __func__);
		return;
	}

<<<<<<< HEAD
	hsotg->eps[0].dir_in = 0;

	ret = s3c_hsotg_ep_queue(&hsotg->eps[0].ep, req, GFP_ATOMIC);
=======
	hsotg->eps_out[0]->dir_in = 0;
	hsotg->eps_out[0]->send_zlp = 0;
	hsotg->ep0_state = DWC2_EP0_SETUP;

	ret = dwc2_hsotg_ep_queue(&hsotg->eps_out[0]->ep, req, GFP_ATOMIC);
>>>>>>> v4.9.227
	if (ret < 0) {
		dev_err(hsotg->dev, "%s: failed queue (%d)\n", __func__, ret);
		/*
		 * Don't think there's much we can do other than watch the
		 * driver fail.
		 */
	}
}

<<<<<<< HEAD
/**
 * s3c_hsotg_complete_request - complete a request given to us
=======
static void dwc2_hsotg_program_zlp(struct dwc2_hsotg *hsotg,
					struct dwc2_hsotg_ep *hs_ep)
{
	u32 ctrl;
	u8 index = hs_ep->index;
	u32 epctl_reg = hs_ep->dir_in ? DIEPCTL(index) : DOEPCTL(index);
	u32 epsiz_reg = hs_ep->dir_in ? DIEPTSIZ(index) : DOEPTSIZ(index);

	if (hs_ep->dir_in)
		dev_dbg(hsotg->dev, "Sending zero-length packet on ep%d\n",
									index);
	else
		dev_dbg(hsotg->dev, "Receiving zero-length packet on ep%d\n",
									index);

	dwc2_writel(DXEPTSIZ_MC(1) | DXEPTSIZ_PKTCNT(1) |
		    DXEPTSIZ_XFERSIZE(0), hsotg->regs +
		    epsiz_reg);

	ctrl = dwc2_readl(hsotg->regs + epctl_reg);
	ctrl |= DXEPCTL_CNAK;  /* clear NAK set by core */
	ctrl |= DXEPCTL_EPENA; /* ensure ep enabled */
	ctrl |= DXEPCTL_USBACTEP;
	dwc2_writel(ctrl, hsotg->regs + epctl_reg);
}

/**
 * dwc2_hsotg_complete_request - complete a request given to us
>>>>>>> v4.9.227
 * @hsotg: The device state.
 * @hs_ep: The endpoint the request was on.
 * @hs_req: The request to complete.
 * @result: The result code (0 => Ok, otherwise errno)
 *
 * The given request has finished, so call the necessary completion
 * if it has one and then look to see if we can start a new request
 * on the endpoint.
 *
 * Note, expects the ep to already be locked as appropriate.
 */
<<<<<<< HEAD
static void s3c_hsotg_complete_request(struct s3c_hsotg *hsotg,
				       struct s3c_hsotg_ep *hs_ep,
				       struct s3c_hsotg_req *hs_req,
				       int result)
{
	bool restart;
=======
static void dwc2_hsotg_complete_request(struct dwc2_hsotg *hsotg,
				       struct dwc2_hsotg_ep *hs_ep,
				       struct dwc2_hsotg_req *hs_req,
				       int result)
{
>>>>>>> v4.9.227

	if (!hs_req) {
		dev_dbg(hsotg->dev, "%s: nothing to complete?\n", __func__);
		return;
	}

	dev_dbg(hsotg->dev, "complete: ep %p %s, req %p, %d => %p\n",
		hs_ep, hs_ep->ep.name, hs_req, result, hs_req->req.complete);

	/*
	 * only replace the status if we've not already set an error
	 * from a previous transaction
	 */

	if (hs_req->req.status == -EINPROGRESS)
		hs_req->req.status = result;

<<<<<<< HEAD
	hs_ep->req = NULL;
	list_del_init(&hs_req->queue);

	if (using_dma(hsotg))
		s3c_hsotg_unmap_dma(hsotg, hs_ep, hs_req);

=======
	if (using_dma(hsotg))
		dwc2_hsotg_unmap_dma(hsotg, hs_ep, hs_req);

	dwc2_hsotg_handle_unaligned_buf_complete(hsotg, hs_ep, hs_req);

	hs_ep->req = NULL;
	list_del_init(&hs_req->queue);

>>>>>>> v4.9.227
	/*
	 * call the complete request with the locks off, just in case the
	 * request tries to queue more work for this endpoint.
	 */

	if (hs_req->req.complete) {
		spin_unlock(&hsotg->lock);
		usb_gadget_giveback_request(&hs_ep->ep, &hs_req->req);
		spin_lock(&hsotg->lock);
	}

	/*
	 * Look to see if there is anything else to do. Note, the completion
	 * of the previous request may have caused a new request to be started
	 * so be careful when doing this.
	 */

	if (!hs_ep->req && result >= 0) {
<<<<<<< HEAD
		restart = !list_empty(&hs_ep->queue);
		if (restart) {
			hs_req = get_ep_head(hs_ep);
			s3c_hsotg_start_req(hsotg, hs_ep, hs_req, false);
		}
=======
		dwc2_gadget_start_next_request(hs_ep);
>>>>>>> v4.9.227
	}
}

/**
<<<<<<< HEAD
 * s3c_hsotg_rx_data - receive data from the FIFO for an endpoint
=======
 * dwc2_hsotg_rx_data - receive data from the FIFO for an endpoint
>>>>>>> v4.9.227
 * @hsotg: The device state.
 * @ep_idx: The endpoint index for the data
 * @size: The size of data in the fifo, in bytes
 *
 * The FIFO status shows there is data to read from the FIFO for a given
 * endpoint, so sort out whether we need to read the data into a request
 * that has been made for that endpoint.
 */
<<<<<<< HEAD
static void s3c_hsotg_rx_data(struct s3c_hsotg *hsotg, int ep_idx, int size)
{
	struct s3c_hsotg_ep *hs_ep = &hsotg->eps[ep_idx];
	struct s3c_hsotg_req *hs_req = hs_ep->req;
=======
static void dwc2_hsotg_rx_data(struct dwc2_hsotg *hsotg, int ep_idx, int size)
{
	struct dwc2_hsotg_ep *hs_ep = hsotg->eps_out[ep_idx];
	struct dwc2_hsotg_req *hs_req = hs_ep->req;
>>>>>>> v4.9.227
	void __iomem *fifo = hsotg->regs + EPFIFO(ep_idx);
	int to_read;
	int max_req;
	int read_ptr;


	if (!hs_req) {
<<<<<<< HEAD
		u32 epctl = readl(hsotg->regs + DOEPCTL(ep_idx));
		int ptr;

		dev_warn(hsotg->dev,
=======
		u32 epctl = dwc2_readl(hsotg->regs + DOEPCTL(ep_idx));
		int ptr;

		dev_dbg(hsotg->dev,
>>>>>>> v4.9.227
			 "%s: FIFO %d bytes on ep%d but no req (DXEPCTl=0x%08x)\n",
			 __func__, size, ep_idx, epctl);

		/* dump the data from the FIFO, we've nothing we can do */
		for (ptr = 0; ptr < size; ptr += 4)
<<<<<<< HEAD
			(void)readl(fifo);
=======
			(void)dwc2_readl(fifo);
>>>>>>> v4.9.227

		return;
	}

	to_read = size;
	read_ptr = hs_req->req.actual;
	max_req = hs_req->req.length - read_ptr;

	dev_dbg(hsotg->dev, "%s: read %d/%d, done %d/%d\n",
		__func__, to_read, max_req, read_ptr, hs_req->req.length);

	if (to_read > max_req) {
		/*
		 * more data appeared than we where willing
		 * to deal with in this request.
		 */

		/* currently we don't deal this */
		WARN_ON_ONCE(1);
	}

	hs_ep->total_data += to_read;
	hs_req->req.actual += to_read;
	to_read = DIV_ROUND_UP(to_read, 4);

	/*
	 * note, we might over-write the buffer end by 3 bytes depending on
	 * alignment of the data.
	 */
	ioread32_rep(fifo, hs_req->req.buf + read_ptr, to_read);
}

/**
<<<<<<< HEAD
 * s3c_hsotg_send_zlp - send zero-length packet on control endpoint
 * @hsotg: The device instance
 * @req: The request currently on this endpoint
=======
 * dwc2_hsotg_ep0_zlp - send/receive zero-length packet on control endpoint
 * @hsotg: The device instance
 * @dir_in: If IN zlp
>>>>>>> v4.9.227
 *
 * Generate a zero-length IN packet request for terminating a SETUP
 * transaction.
 *
 * Note, since we don't write any data to the TxFIFO, then it is
 * currently believed that we do not need to wait for any space in
 * the TxFIFO.
 */
<<<<<<< HEAD
static void s3c_hsotg_send_zlp(struct s3c_hsotg *hsotg,
			       struct s3c_hsotg_req *req)
{
	u32 ctrl;

	if (!req) {
		dev_warn(hsotg->dev, "%s: no request?\n", __func__);
		return;
	}

	if (req->req.length == 0) {
		hsotg->eps[0].sent_zlp = 1;
		s3c_hsotg_enqueue_setup(hsotg);
		return;
	}

	hsotg->eps[0].dir_in = 1;
	hsotg->eps[0].sent_zlp = 1;

	dev_dbg(hsotg->dev, "sending zero-length packet\n");

	/* issue a zero-sized packet to terminate this */
	writel(DXEPTSIZ_MC(1) | DXEPTSIZ_PKTCNT(1) |
	       DXEPTSIZ_XFERSIZE(0), hsotg->regs + DIEPTSIZ(0));

	ctrl = readl(hsotg->regs + DIEPCTL0);
	ctrl |= DXEPCTL_CNAK;  /* clear NAK set by core */
	ctrl |= DXEPCTL_EPENA; /* ensure ep enabled */
	ctrl |= DXEPCTL_USBACTEP;
	writel(ctrl, hsotg->regs + DIEPCTL0);
}

/**
 * s3c_hsotg_handle_outdone - handle receiving OutDone/SetupDone from RXFIFO
 * @hsotg: The device instance
 * @epnum: The endpoint received from
 * @was_setup: Set if processing a SetupDone event.
=======
static void dwc2_hsotg_ep0_zlp(struct dwc2_hsotg *hsotg, bool dir_in)
{
	/* eps_out[0] is used in both directions */
	hsotg->eps_out[0]->dir_in = dir_in;
	hsotg->ep0_state = dir_in ? DWC2_EP0_STATUS_IN : DWC2_EP0_STATUS_OUT;

	dwc2_hsotg_program_zlp(hsotg, hsotg->eps_out[0]);
}

static void dwc2_hsotg_change_ep_iso_parity(struct dwc2_hsotg *hsotg,
			u32 epctl_reg)
{
	u32 ctrl;

	ctrl = dwc2_readl(hsotg->regs + epctl_reg);
	if (ctrl & DXEPCTL_EOFRNUM)
		ctrl |= DXEPCTL_SETEVENFR;
	else
		ctrl |= DXEPCTL_SETODDFR;
	dwc2_writel(ctrl, hsotg->regs + epctl_reg);
}

/**
 * dwc2_hsotg_handle_outdone - handle receiving OutDone/SetupDone from RXFIFO
 * @hsotg: The device instance
 * @epnum: The endpoint received from
>>>>>>> v4.9.227
 *
 * The RXFIFO has delivered an OutDone event, which means that the data
 * transfer for an OUT endpoint has been completed, either by a short
 * packet or by the finish of a transfer.
 */
<<<<<<< HEAD
static void s3c_hsotg_handle_outdone(struct s3c_hsotg *hsotg,
				     int epnum, bool was_setup)
{
	u32 epsize = readl(hsotg->regs + DOEPTSIZ(epnum));
	struct s3c_hsotg_ep *hs_ep = &hsotg->eps[epnum];
	struct s3c_hsotg_req *hs_req = hs_ep->req;
=======
static void dwc2_hsotg_handle_outdone(struct dwc2_hsotg *hsotg, int epnum)
{
	u32 epsize = dwc2_readl(hsotg->regs + DOEPTSIZ(epnum));
	struct dwc2_hsotg_ep *hs_ep = hsotg->eps_out[epnum];
	struct dwc2_hsotg_req *hs_req = hs_ep->req;
>>>>>>> v4.9.227
	struct usb_request *req = &hs_req->req;
	unsigned size_left = DXEPTSIZ_XFERSIZE_GET(epsize);
	int result = 0;

	if (!hs_req) {
		dev_dbg(hsotg->dev, "%s: no request active\n", __func__);
		return;
	}

<<<<<<< HEAD
=======
	if (epnum == 0 && hsotg->ep0_state == DWC2_EP0_STATUS_OUT) {
		dev_dbg(hsotg->dev, "zlp packet received\n");
		dwc2_hsotg_complete_request(hsotg, hs_ep, hs_req, 0);
		dwc2_hsotg_enqueue_setup(hsotg);
		return;
	}

>>>>>>> v4.9.227
	if (using_dma(hsotg)) {
		unsigned size_done;

		/*
		 * Calculate the size of the transfer by checking how much
		 * is left in the endpoint size register and then working it
		 * out from the amount we loaded for the transfer.
		 *
		 * We need to do this as DMA pointers are always 32bit aligned
		 * so may overshoot/undershoot the transfer.
		 */

		size_done = hs_ep->size_loaded - size_left;
		size_done += hs_ep->last_load;

		req->actual = size_done;
	}

	/* if there is more request to do, schedule new transfer */
	if (req->actual < req->length && size_left == 0) {
<<<<<<< HEAD
		s3c_hsotg_start_req(hsotg, hs_ep, hs_req, true);
		return;
	} else if (epnum == 0) {
		/*
		 * After was_setup = 1 =>
		 * set CNAK for non Setup requests
		 */
		hsotg->setup = was_setup ? 0 : 1;
=======
		dwc2_hsotg_start_req(hsotg, hs_ep, hs_req, true);
		return;
>>>>>>> v4.9.227
	}

	if (req->actual < req->length && req->short_not_ok) {
		dev_dbg(hsotg->dev, "%s: got %d/%d (short not ok) => error\n",
			__func__, req->actual, req->length);

		/*
		 * todo - what should we return here? there's no one else
		 * even bothering to check the status.
		 */
	}

<<<<<<< HEAD
	if (epnum == 0) {
		/*
		 * Condition req->complete != s3c_hsotg_complete_setup says:
		 * send ZLP when we have an asynchronous request from gadget
		 */
		if (!was_setup && req->complete != s3c_hsotg_complete_setup)
			s3c_hsotg_send_zlp(hsotg, hs_req);
	}

	s3c_hsotg_complete_request(hsotg, hs_ep, hs_req, result);
}

/**
 * s3c_hsotg_read_frameno - read current frame number
 * @hsotg: The device instance
 *
 * Return the current frame number
 */
static u32 s3c_hsotg_read_frameno(struct s3c_hsotg *hsotg)
{
	u32 dsts;

	dsts = readl(hsotg->regs + DSTS);
	dsts &= DSTS_SOFFN_MASK;
	dsts >>= DSTS_SOFFN_SHIFT;

	return dsts;
}

/**
 * s3c_hsotg_handle_rx - RX FIFO has data
=======
	if (epnum == 0 && hsotg->ep0_state == DWC2_EP0_DATA_OUT) {
		/* Move to STATUS IN */
		dwc2_hsotg_ep0_zlp(hsotg, true);
		return;
	}

	/*
	 * Slave mode OUT transfers do not go through XferComplete so
	 * adjust the ISOC parity here.
	 */
	if (!using_dma(hsotg)) {
		if (hs_ep->isochronous && hs_ep->interval == 1)
			dwc2_hsotg_change_ep_iso_parity(hsotg, DOEPCTL(epnum));
		else if (hs_ep->isochronous && hs_ep->interval > 1)
			dwc2_gadget_incr_frame_num(hs_ep);
	}

	dwc2_hsotg_complete_request(hsotg, hs_ep, hs_req, result);
}

/**
 * dwc2_hsotg_handle_rx - RX FIFO has data
>>>>>>> v4.9.227
 * @hsotg: The device instance
 *
 * The IRQ handler has detected that the RX FIFO has some data in it
 * that requires processing, so find out what is in there and do the
 * appropriate read.
 *
 * The RXFIFO is a true FIFO, the packets coming out are still in packet
 * chunks, so if you have x packets received on an endpoint you'll get x
 * FIFO events delivered, each with a packet's worth of data in it.
 *
 * When using DMA, we should not be processing events from the RXFIFO
 * as the actual data should be sent to the memory directly and we turn
 * on the completion interrupts to get notifications of transfer completion.
 */
<<<<<<< HEAD
static void s3c_hsotg_handle_rx(struct s3c_hsotg *hsotg)
{
	u32 grxstsr = readl(hsotg->regs + GRXSTSP);
=======
static void dwc2_hsotg_handle_rx(struct dwc2_hsotg *hsotg)
{
	u32 grxstsr = dwc2_readl(hsotg->regs + GRXSTSP);
>>>>>>> v4.9.227
	u32 epnum, status, size;

	WARN_ON(using_dma(hsotg));

	epnum = grxstsr & GRXSTS_EPNUM_MASK;
	status = grxstsr & GRXSTS_PKTSTS_MASK;

	size = grxstsr & GRXSTS_BYTECNT_MASK;
	size >>= GRXSTS_BYTECNT_SHIFT;

<<<<<<< HEAD
	if (1)
		dev_dbg(hsotg->dev, "%s: GRXSTSP=0x%08x (%d@%d)\n",
=======
	dev_dbg(hsotg->dev, "%s: GRXSTSP=0x%08x (%d@%d)\n",
>>>>>>> v4.9.227
			__func__, grxstsr, size, epnum);

	switch ((status & GRXSTS_PKTSTS_MASK) >> GRXSTS_PKTSTS_SHIFT) {
	case GRXSTS_PKTSTS_GLOBALOUTNAK:
		dev_dbg(hsotg->dev, "GLOBALOUTNAK\n");
		break;

	case GRXSTS_PKTSTS_OUTDONE:
		dev_dbg(hsotg->dev, "OutDone (Frame=0x%08x)\n",
<<<<<<< HEAD
			s3c_hsotg_read_frameno(hsotg));

		if (!using_dma(hsotg))
			s3c_hsotg_handle_outdone(hsotg, epnum, false);
=======
			dwc2_hsotg_read_frameno(hsotg));

		if (!using_dma(hsotg))
			dwc2_hsotg_handle_outdone(hsotg, epnum);
>>>>>>> v4.9.227
		break;

	case GRXSTS_PKTSTS_SETUPDONE:
		dev_dbg(hsotg->dev,
			"SetupDone (Frame=0x%08x, DOPEPCTL=0x%08x)\n",
<<<<<<< HEAD
			s3c_hsotg_read_frameno(hsotg),
			readl(hsotg->regs + DOEPCTL(0)));

		s3c_hsotg_handle_outdone(hsotg, epnum, true);
		break;

	case GRXSTS_PKTSTS_OUTRX:
		s3c_hsotg_rx_data(hsotg, epnum, size);
=======
			dwc2_hsotg_read_frameno(hsotg),
			dwc2_readl(hsotg->regs + DOEPCTL(0)));
		/*
		 * Call dwc2_hsotg_handle_outdone here if it was not called from
		 * GRXSTS_PKTSTS_OUTDONE. That is, if the core didn't
		 * generate GRXSTS_PKTSTS_OUTDONE for setup packet.
		 */
		if (hsotg->ep0_state == DWC2_EP0_SETUP)
			dwc2_hsotg_handle_outdone(hsotg, epnum);
		break;

	case GRXSTS_PKTSTS_OUTRX:
		dwc2_hsotg_rx_data(hsotg, epnum, size);
>>>>>>> v4.9.227
		break;

	case GRXSTS_PKTSTS_SETUPRX:
		dev_dbg(hsotg->dev,
			"SetupRX (Frame=0x%08x, DOPEPCTL=0x%08x)\n",
<<<<<<< HEAD
			s3c_hsotg_read_frameno(hsotg),
			readl(hsotg->regs + DOEPCTL(0)));

		s3c_hsotg_rx_data(hsotg, epnum, size);
=======
			dwc2_hsotg_read_frameno(hsotg),
			dwc2_readl(hsotg->regs + DOEPCTL(0)));

		WARN_ON(hsotg->ep0_state != DWC2_EP0_SETUP);

		dwc2_hsotg_rx_data(hsotg, epnum, size);
>>>>>>> v4.9.227
		break;

	default:
		dev_warn(hsotg->dev, "%s: unknown status %08x\n",
			 __func__, grxstsr);

<<<<<<< HEAD
		s3c_hsotg_dump(hsotg);
=======
		dwc2_hsotg_dump(hsotg);
>>>>>>> v4.9.227
		break;
	}
}

/**
<<<<<<< HEAD
 * s3c_hsotg_ep0_mps - turn max packet size into register setting
 * @mps: The maximum packet size in bytes.
 */
static u32 s3c_hsotg_ep0_mps(unsigned int mps)
=======
 * dwc2_hsotg_ep0_mps - turn max packet size into register setting
 * @mps: The maximum packet size in bytes.
 */
static u32 dwc2_hsotg_ep0_mps(unsigned int mps)
>>>>>>> v4.9.227
{
	switch (mps) {
	case 64:
		return D0EPCTL_MPS_64;
	case 32:
		return D0EPCTL_MPS_32;
	case 16:
		return D0EPCTL_MPS_16;
	case 8:
		return D0EPCTL_MPS_8;
	}

	/* bad max packet size, warn and return invalid result */
	WARN_ON(1);
	return (u32)-1;
}

/**
<<<<<<< HEAD
 * s3c_hsotg_set_ep_maxpacket - set endpoint's max-packet field
=======
 * dwc2_hsotg_set_ep_maxpacket - set endpoint's max-packet field
>>>>>>> v4.9.227
 * @hsotg: The driver state.
 * @ep: The index number of the endpoint
 * @mps: The maximum packet size in bytes
 *
 * Configure the maximum packet size for the given endpoint, updating
 * the hardware control registers to reflect this.
 */
<<<<<<< HEAD
static void s3c_hsotg_set_ep_maxpacket(struct s3c_hsotg *hsotg,
				       unsigned int ep, unsigned int mps)
{
	struct s3c_hsotg_ep *hs_ep = &hsotg->eps[ep];
=======
static void dwc2_hsotg_set_ep_maxpacket(struct dwc2_hsotg *hsotg,
			unsigned int ep, unsigned int mps, unsigned int dir_in)
{
	struct dwc2_hsotg_ep *hs_ep;
>>>>>>> v4.9.227
	void __iomem *regs = hsotg->regs;
	u32 mpsval;
	u32 mcval;
	u32 reg;

<<<<<<< HEAD
	if (ep == 0) {
		/* EP0 is a special case */
		mpsval = s3c_hsotg_ep0_mps(mps);
=======
	hs_ep = index_to_ep(hsotg, ep, dir_in);
	if (!hs_ep)
		return;

	if (ep == 0) {
		/* EP0 is a special case */
		mpsval = dwc2_hsotg_ep0_mps(mps);
>>>>>>> v4.9.227
		if (mpsval > 3)
			goto bad_mps;
		hs_ep->ep.maxpacket = mps;
		hs_ep->mc = 1;
	} else {
		mpsval = mps & DXEPCTL_MPS_MASK;
		if (mpsval > 1024)
			goto bad_mps;
		mcval = ((mps >> 11) & 0x3) + 1;
		hs_ep->mc = mcval;
		if (mcval > 3)
			goto bad_mps;
		hs_ep->ep.maxpacket = mpsval;
	}

<<<<<<< HEAD
	/*
	 * update both the in and out endpoint controldir_ registers, even
	 * if one of the directions may not be in use.
	 */

	reg = readl(regs + DIEPCTL(ep));
	reg &= ~DXEPCTL_MPS_MASK;
	reg |= mpsval;
	writel(reg, regs + DIEPCTL(ep));

	if (ep) {
		reg = readl(regs + DOEPCTL(ep));
		reg &= ~DXEPCTL_MPS_MASK;
		reg |= mpsval;
		writel(reg, regs + DOEPCTL(ep));
=======
	if (dir_in) {
		reg = dwc2_readl(regs + DIEPCTL(ep));
		reg &= ~DXEPCTL_MPS_MASK;
		reg |= mpsval;
		dwc2_writel(reg, regs + DIEPCTL(ep));
	} else {
		reg = dwc2_readl(regs + DOEPCTL(ep));
		reg &= ~DXEPCTL_MPS_MASK;
		reg |= mpsval;
		dwc2_writel(reg, regs + DOEPCTL(ep));
>>>>>>> v4.9.227
	}

	return;

bad_mps:
	dev_err(hsotg->dev, "ep%d: bad mps of %d\n", ep, mps);
}

/**
<<<<<<< HEAD
 * s3c_hsotg_txfifo_flush - flush Tx FIFO
 * @hsotg: The driver state
 * @idx: The index for the endpoint (0..15)
 */
static void s3c_hsotg_txfifo_flush(struct s3c_hsotg *hsotg, unsigned int idx)
=======
 * dwc2_hsotg_txfifo_flush - flush Tx FIFO
 * @hsotg: The driver state
 * @idx: The index for the endpoint (0..15)
 */
static void dwc2_hsotg_txfifo_flush(struct dwc2_hsotg *hsotg, unsigned int idx)
>>>>>>> v4.9.227
{
	int timeout;
	int val;

<<<<<<< HEAD
	writel(GRSTCTL_TXFNUM(idx) | GRSTCTL_TXFFLSH,
		hsotg->regs + GRSTCTL);
=======
	dwc2_writel(GRSTCTL_TXFNUM(idx) | GRSTCTL_TXFFLSH,
		    hsotg->regs + GRSTCTL);
>>>>>>> v4.9.227

	/* wait until the fifo is flushed */
	timeout = 100;

	while (1) {
<<<<<<< HEAD
		val = readl(hsotg->regs + GRSTCTL);
=======
		val = dwc2_readl(hsotg->regs + GRSTCTL);
>>>>>>> v4.9.227

		if ((val & (GRSTCTL_TXFFLSH)) == 0)
			break;

		if (--timeout == 0) {
			dev_err(hsotg->dev,
				"%s: timeout flushing fifo (GRSTCTL=%08x)\n",
				__func__, val);
			break;
		}

		udelay(1);
	}
}

/**
<<<<<<< HEAD
 * s3c_hsotg_trytx - check to see if anything needs transmitting
=======
 * dwc2_hsotg_trytx - check to see if anything needs transmitting
>>>>>>> v4.9.227
 * @hsotg: The driver state
 * @hs_ep: The driver endpoint to check.
 *
 * Check to see if there is a request that has data to send, and if so
 * make an attempt to write data into the FIFO.
 */
<<<<<<< HEAD
static int s3c_hsotg_trytx(struct s3c_hsotg *hsotg,
			   struct s3c_hsotg_ep *hs_ep)
{
	struct s3c_hsotg_req *hs_req = hs_ep->req;
=======
static int dwc2_hsotg_trytx(struct dwc2_hsotg *hsotg,
			   struct dwc2_hsotg_ep *hs_ep)
{
	struct dwc2_hsotg_req *hs_req = hs_ep->req;
>>>>>>> v4.9.227

	if (!hs_ep->dir_in || !hs_req) {
		/**
		 * if request is not enqueued, we disable interrupts
		 * for endpoints, excepting ep0
		 */
		if (hs_ep->index != 0)
<<<<<<< HEAD
			s3c_hsotg_ctrl_epint(hsotg, hs_ep->index,
=======
			dwc2_hsotg_ctrl_epint(hsotg, hs_ep->index,
>>>>>>> v4.9.227
					     hs_ep->dir_in, 0);
		return 0;
	}

	if (hs_req->req.actual < hs_req->req.length) {
		dev_dbg(hsotg->dev, "trying to write more for ep%d\n",
			hs_ep->index);
<<<<<<< HEAD
		return s3c_hsotg_write_fifo(hsotg, hs_ep, hs_req);
=======
		return dwc2_hsotg_write_fifo(hsotg, hs_ep, hs_req);
>>>>>>> v4.9.227
	}

	return 0;
}

/**
<<<<<<< HEAD
 * s3c_hsotg_complete_in - complete IN transfer
=======
 * dwc2_hsotg_complete_in - complete IN transfer
>>>>>>> v4.9.227
 * @hsotg: The device state.
 * @hs_ep: The endpoint that has just completed.
 *
 * An IN transfer has been completed, update the transfer's state and then
 * call the relevant completion routines.
 */
<<<<<<< HEAD
static void s3c_hsotg_complete_in(struct s3c_hsotg *hsotg,
				  struct s3c_hsotg_ep *hs_ep)
{
	struct s3c_hsotg_req *hs_req = hs_ep->req;
	u32 epsize = readl(hsotg->regs + DIEPTSIZ(hs_ep->index));
=======
static void dwc2_hsotg_complete_in(struct dwc2_hsotg *hsotg,
				  struct dwc2_hsotg_ep *hs_ep)
{
	struct dwc2_hsotg_req *hs_req = hs_ep->req;
	u32 epsize = dwc2_readl(hsotg->regs + DIEPTSIZ(hs_ep->index));
>>>>>>> v4.9.227
	int size_left, size_done;

	if (!hs_req) {
		dev_dbg(hsotg->dev, "XferCompl but no req\n");
		return;
	}

	/* Finish ZLP handling for IN EP0 transactions */
<<<<<<< HEAD
	if (hsotg->eps[0].sent_zlp) {
		dev_dbg(hsotg->dev, "zlp packet received\n");
		s3c_hsotg_complete_request(hsotg, hs_ep, hs_req, 0);
=======
	if (hs_ep->index == 0 && hsotg->ep0_state == DWC2_EP0_STATUS_IN) {
		dev_dbg(hsotg->dev, "zlp packet sent\n");
		dwc2_hsotg_complete_request(hsotg, hs_ep, hs_req, 0);
		if (hsotg->test_mode) {
			int ret;

			ret = dwc2_hsotg_set_test_mode(hsotg, hsotg->test_mode);
			if (ret < 0) {
				dev_dbg(hsotg->dev, "Invalid Test #%d\n",
						hsotg->test_mode);
				dwc2_hsotg_stall_ep0(hsotg);
				return;
			}
		}
		dwc2_hsotg_enqueue_setup(hsotg);
>>>>>>> v4.9.227
		return;
	}

	/*
	 * Calculate the size of the transfer by checking how much is left
	 * in the endpoint size register and then working it out from
	 * the amount we loaded for the transfer.
	 *
	 * We do this even for DMA, as the transfer may have incremented
	 * past the end of the buffer (DMA transfers are always 32bit
	 * aligned).
	 */

	size_left = DXEPTSIZ_XFERSIZE_GET(epsize);

	size_done = hs_ep->size_loaded - size_left;
	size_done += hs_ep->last_load;

	if (hs_req->req.actual != size_done)
		dev_dbg(hsotg->dev, "%s: adjusting size done %d => %d\n",
			__func__, hs_req->req.actual, size_done);

	hs_req->req.actual = size_done;
	dev_dbg(hsotg->dev, "req->length:%d req->actual:%d req->zero:%d\n",
		hs_req->req.length, hs_req->req.actual, hs_req->req.zero);

<<<<<<< HEAD
	/*
	 * Check if dealing with Maximum Packet Size(MPS) IN transfer at EP0
	 * When sent data is a multiple MPS size (e.g. 64B ,128B ,192B
	 * ,256B ... ), after last MPS sized packet send IN ZLP packet to
	 * inform the host that no more data is available.
	 * The state of req.zero member is checked to be sure that the value to
	 * send is smaller than wValue expected from host.
	 * Check req.length to NOT send another ZLP when the current one is
	 * under completion (the one for which this completion has been called).
	 */
	if (hs_req->req.length && hs_ep->index == 0 && hs_req->req.zero &&
	    hs_req->req.length == hs_req->req.actual &&
	    !(hs_req->req.length % hs_ep->ep.maxpacket)) {

		dev_dbg(hsotg->dev, "ep0 zlp IN packet sent\n");
		s3c_hsotg_send_zlp(hsotg, hs_req);

		return;
	}

	if (!size_left && hs_req->req.actual < hs_req->req.length) {
		dev_dbg(hsotg->dev, "%s trying more for req...\n", __func__);
		s3c_hsotg_start_req(hsotg, hs_ep, hs_req, true);
	} else
		s3c_hsotg_complete_request(hsotg, hs_ep, hs_req, 0);
}

/**
 * s3c_hsotg_epint - handle an in/out endpoint interrupt
=======
	if (!size_left && hs_req->req.actual < hs_req->req.length) {
		dev_dbg(hsotg->dev, "%s trying more for req...\n", __func__);
		dwc2_hsotg_start_req(hsotg, hs_ep, hs_req, true);
		return;
	}

	/* Zlp for all endpoints, for ep0 only in DATA IN stage */
	if (hs_ep->send_zlp) {
		dwc2_hsotg_program_zlp(hsotg, hs_ep);
		hs_ep->send_zlp = 0;
		/* transfer will be completed on next complete interrupt */
		return;
	}

	if (hs_ep->index == 0 && hsotg->ep0_state == DWC2_EP0_DATA_IN) {
		/* Move to STATUS OUT */
		dwc2_hsotg_ep0_zlp(hsotg, false);
		return;
	}

	dwc2_hsotg_complete_request(hsotg, hs_ep, hs_req, 0);
}

/**
 * dwc2_gadget_read_ep_interrupts - reads interrupts for given ep
 * @hsotg: The device state.
 * @idx: Index of ep.
 * @dir_in: Endpoint direction 1-in 0-out.
 *
 * Reads for endpoint with given index and direction, by masking
 * epint_reg with coresponding mask.
 */
static u32 dwc2_gadget_read_ep_interrupts(struct dwc2_hsotg *hsotg,
					  unsigned int idx, int dir_in)
{
	u32 epmsk_reg = dir_in ? DIEPMSK : DOEPMSK;
	u32 epint_reg = dir_in ? DIEPINT(idx) : DOEPINT(idx);
	u32 ints;
	u32 mask;
	u32 diepempmsk;

	mask = dwc2_readl(hsotg->regs + epmsk_reg);
	diepempmsk = dwc2_readl(hsotg->regs + DIEPEMPMSK);
	mask |= ((diepempmsk >> idx) & 0x1) ? DIEPMSK_TXFIFOEMPTY : 0;
	mask |= DXEPINT_SETUP_RCVD;

	ints = dwc2_readl(hsotg->regs + epint_reg);
	ints &= mask;
	return ints;
}

/**
 * dwc2_gadget_handle_ep_disabled - handle DXEPINT_EPDISBLD
 * @hs_ep: The endpoint on which interrupt is asserted.
 *
 * This interrupt indicates that the endpoint has been disabled per the
 * application's request.
 *
 * For IN endpoints flushes txfifo, in case of BULK clears DCTL_CGNPINNAK,
 * in case of ISOC completes current request.
 *
 * For ISOC-OUT endpoints completes expired requests. If there is remaining
 * request starts it.
 */
static void dwc2_gadget_handle_ep_disabled(struct dwc2_hsotg_ep *hs_ep)
{
	struct dwc2_hsotg *hsotg = hs_ep->parent;
	struct dwc2_hsotg_req *hs_req;
	unsigned char idx = hs_ep->index;
	int dir_in = hs_ep->dir_in;
	u32 epctl_reg = dir_in ? DIEPCTL(idx) : DOEPCTL(idx);
	int dctl = dwc2_readl(hsotg->regs + DCTL);

	dev_dbg(hsotg->dev, "%s: EPDisbld\n", __func__);

	if (dir_in) {
		int epctl = dwc2_readl(hsotg->regs + epctl_reg);

		dwc2_hsotg_txfifo_flush(hsotg, hs_ep->fifo_index);

		if (hs_ep->isochronous) {
			dwc2_hsotg_complete_in(hsotg, hs_ep);
			return;
		}

		if ((epctl & DXEPCTL_STALL) && (epctl & DXEPCTL_EPTYPE_BULK)) {
			int dctl = dwc2_readl(hsotg->regs + DCTL);

			dctl |= DCTL_CGNPINNAK;
			dwc2_writel(dctl, hsotg->regs + DCTL);
		}
		return;
	}

	if (dctl & DCTL_GOUTNAKSTS) {
		dctl |= DCTL_CGOUTNAK;
		dwc2_writel(dctl, hsotg->regs + DCTL);
	}

	if (!hs_ep->isochronous)
		return;

	if (list_empty(&hs_ep->queue)) {
		dev_dbg(hsotg->dev, "%s: complete_ep 0x%p, ep->queue empty!\n",
			__func__, hs_ep);
		return;
	}

	do {
		hs_req = get_ep_head(hs_ep);
		if (hs_req)
			dwc2_hsotg_complete_request(hsotg, hs_ep, hs_req,
						    -ENODATA);
		dwc2_gadget_incr_frame_num(hs_ep);
	} while (dwc2_gadget_target_frame_elapsed(hs_ep));

	dwc2_gadget_start_next_request(hs_ep);
}

/**
 * dwc2_gadget_handle_out_token_ep_disabled - handle DXEPINT_OUTTKNEPDIS
 * @hs_ep: The endpoint on which interrupt is asserted.
 *
 * This is starting point for ISOC-OUT transfer, synchronization done with
 * first out token received from host while corresponding EP is disabled.
 *
 * Device does not know initial frame in which out token will come. For this
 * HW generates OUTTKNEPDIS - out token is received while EP is disabled. Upon
 * getting this interrupt SW starts calculation for next transfer frame.
 */
static void dwc2_gadget_handle_out_token_ep_disabled(struct dwc2_hsotg_ep *ep)
{
	struct dwc2_hsotg *hsotg = ep->parent;
	int dir_in = ep->dir_in;
	u32 doepmsk;

	if (dir_in || !ep->isochronous)
		return;

	dwc2_hsotg_complete_request(hsotg, ep, get_ep_head(ep), -ENODATA);

	if (ep->interval > 1 &&
	    ep->target_frame == TARGET_FRAME_INITIAL) {
		u32 dsts;
		u32 ctrl;

		dsts = dwc2_readl(hsotg->regs + DSTS);
		ep->target_frame = dwc2_hsotg_read_frameno(hsotg);
		dwc2_gadget_incr_frame_num(ep);

		ctrl = dwc2_readl(hsotg->regs + DOEPCTL(ep->index));
		if (ep->target_frame & 0x1)
			ctrl |= DXEPCTL_SETODDFR;
		else
			ctrl |= DXEPCTL_SETEVENFR;

		dwc2_writel(ctrl, hsotg->regs + DOEPCTL(ep->index));
	}

	dwc2_gadget_start_next_request(ep);
	doepmsk = dwc2_readl(hsotg->regs + DOEPMSK);
	doepmsk &= ~DOEPMSK_OUTTKNEPDISMSK;
	dwc2_writel(doepmsk, hsotg->regs + DOEPMSK);
}

/**
* dwc2_gadget_handle_nak - handle NAK interrupt
* @hs_ep: The endpoint on which interrupt is asserted.
*
* This is starting point for ISOC-IN transfer, synchronization done with
* first IN token received from host while corresponding EP is disabled.
*
* Device does not know when first one token will arrive from host. On first
* token arrival HW generates 2 interrupts: 'in token received while FIFO empty'
* and 'NAK'. NAK interrupt for ISOC-IN means that token has arrived and ZLP was
* sent in response to that as there was no data in FIFO. SW is basing on this
* interrupt to obtain frame in which token has come and then based on the
* interval calculates next frame for transfer.
*/
static void dwc2_gadget_handle_nak(struct dwc2_hsotg_ep *hs_ep)
{
	struct dwc2_hsotg *hsotg = hs_ep->parent;
	int dir_in = hs_ep->dir_in;

	if (!dir_in || !hs_ep->isochronous)
		return;

	if (hs_ep->target_frame == TARGET_FRAME_INITIAL) {
		hs_ep->target_frame = dwc2_hsotg_read_frameno(hsotg);
		if (hs_ep->interval > 1) {
			u32 ctrl = dwc2_readl(hsotg->regs +
					      DIEPCTL(hs_ep->index));
			if (hs_ep->target_frame & 0x1)
				ctrl |= DXEPCTL_SETODDFR;
			else
				ctrl |= DXEPCTL_SETEVENFR;

			dwc2_writel(ctrl, hsotg->regs + DIEPCTL(hs_ep->index));
		}

		dwc2_hsotg_complete_request(hsotg, hs_ep,
					    get_ep_head(hs_ep), 0);
	}

	dwc2_gadget_incr_frame_num(hs_ep);
}

/**
 * dwc2_hsotg_epint - handle an in/out endpoint interrupt
>>>>>>> v4.9.227
 * @hsotg: The driver state
 * @idx: The index for the endpoint (0..15)
 * @dir_in: Set if this is an IN endpoint
 *
 * Process and clear any interrupt pending for an individual endpoint
 */
<<<<<<< HEAD
static void s3c_hsotg_epint(struct s3c_hsotg *hsotg, unsigned int idx,
			    int dir_in)
{
	struct s3c_hsotg_ep *hs_ep = &hsotg->eps[idx];
=======
static void dwc2_hsotg_epint(struct dwc2_hsotg *hsotg, unsigned int idx,
			    int dir_in)
{
	struct dwc2_hsotg_ep *hs_ep = index_to_ep(hsotg, idx, dir_in);
>>>>>>> v4.9.227
	u32 epint_reg = dir_in ? DIEPINT(idx) : DOEPINT(idx);
	u32 epctl_reg = dir_in ? DIEPCTL(idx) : DOEPCTL(idx);
	u32 epsiz_reg = dir_in ? DIEPTSIZ(idx) : DOEPTSIZ(idx);
	u32 ints;
	u32 ctrl;

<<<<<<< HEAD
	ints = readl(hsotg->regs + epint_reg);
	ctrl = readl(hsotg->regs + epctl_reg);

	/* Clear endpoint interrupts */
	writel(ints, hsotg->regs + epint_reg);
=======
	ints = dwc2_gadget_read_ep_interrupts(hsotg, idx, dir_in);
	ctrl = dwc2_readl(hsotg->regs + epctl_reg);

	/* Clear endpoint interrupts */
	dwc2_writel(ints, hsotg->regs + epint_reg);

	if (!hs_ep) {
		dev_err(hsotg->dev, "%s:Interrupt for unconfigured ep%d(%s)\n",
					__func__, idx, dir_in ? "in" : "out");
		return;
	}
>>>>>>> v4.9.227

	dev_dbg(hsotg->dev, "%s: ep%d(%s) DxEPINT=0x%08x\n",
		__func__, idx, dir_in ? "in" : "out", ints);

<<<<<<< HEAD
	if (ints & DXEPINT_XFERCOMPL) {
		if (hs_ep->isochronous && hs_ep->interval == 1) {
			if (ctrl & DXEPCTL_EOFRNUM)
				ctrl |= DXEPCTL_SETEVENFR;
			else
				ctrl |= DXEPCTL_SETODDFR;
			writel(ctrl, hsotg->regs + epctl_reg);
		}

		dev_dbg(hsotg->dev,
			"%s: XferCompl: DxEPCTL=0x%08x, DXEPTSIZ=%08x\n",
			__func__, readl(hsotg->regs + epctl_reg),
			readl(hsotg->regs + epsiz_reg));
=======
	/* Don't process XferCompl interrupt if it is a setup packet */
	if (idx == 0 && (ints & (DXEPINT_SETUP | DXEPINT_SETUP_RCVD)))
		ints &= ~DXEPINT_XFERCOMPL;

	if (ints & DXEPINT_STSPHSERCVD)
		dev_dbg(hsotg->dev, "%s: StsPhseRcvd asserted\n", __func__);

	if (ints & DXEPINT_XFERCOMPL) {
		dev_dbg(hsotg->dev,
			"%s: XferCompl: DxEPCTL=0x%08x, DXEPTSIZ=%08x\n",
			__func__, dwc2_readl(hsotg->regs + epctl_reg),
			dwc2_readl(hsotg->regs + epsiz_reg));
>>>>>>> v4.9.227

		/*
		 * we get OutDone from the FIFO, so we only need to look
		 * at completing IN requests here
		 */
		if (dir_in) {
<<<<<<< HEAD
			s3c_hsotg_complete_in(hsotg, hs_ep);

			if (idx == 0 && !hs_ep->req)
				s3c_hsotg_enqueue_setup(hsotg);
=======
			if (hs_ep->isochronous && hs_ep->interval > 1)
				dwc2_gadget_incr_frame_num(hs_ep);

			dwc2_hsotg_complete_in(hsotg, hs_ep);
			if (ints & DXEPINT_NAKINTRPT)
				ints &= ~DXEPINT_NAKINTRPT;

			if (idx == 0 && !hs_ep->req)
				dwc2_hsotg_enqueue_setup(hsotg);
>>>>>>> v4.9.227
		} else if (using_dma(hsotg)) {
			/*
			 * We're using DMA, we need to fire an OutDone here
			 * as we ignore the RXFIFO.
			 */
<<<<<<< HEAD

			s3c_hsotg_handle_outdone(hsotg, idx, false);
		}
	}

	if (ints & DXEPINT_EPDISBLD) {
		dev_dbg(hsotg->dev, "%s: EPDisbld\n", __func__);

		if (dir_in) {
			int epctl = readl(hsotg->regs + epctl_reg);

			s3c_hsotg_txfifo_flush(hsotg, hs_ep->fifo_index);

			if ((epctl & DXEPCTL_STALL) &&
				(epctl & DXEPCTL_EPTYPE_BULK)) {
				int dctl = readl(hsotg->regs + DCTL);

				dctl |= DCTL_CGNPINNAK;
				writel(dctl, hsotg->regs + DCTL);
			}
		}
	}
=======
			if (hs_ep->isochronous && hs_ep->interval > 1)
				dwc2_gadget_incr_frame_num(hs_ep);

			dwc2_hsotg_handle_outdone(hsotg, idx);
		}
	}

	if (ints & DXEPINT_EPDISBLD)
		dwc2_gadget_handle_ep_disabled(hs_ep);

	if (ints & DXEPINT_OUTTKNEPDIS)
		dwc2_gadget_handle_out_token_ep_disabled(hs_ep);

	if (ints & DXEPINT_NAKINTRPT)
		dwc2_gadget_handle_nak(hs_ep);
>>>>>>> v4.9.227

	if (ints & DXEPINT_AHBERR)
		dev_dbg(hsotg->dev, "%s: AHBErr\n", __func__);

	if (ints & DXEPINT_SETUP) {  /* Setup or Timeout */
		dev_dbg(hsotg->dev, "%s: Setup/Timeout\n",  __func__);

		if (using_dma(hsotg) && idx == 0) {
			/*
			 * this is the notification we've received a
			 * setup packet. In non-DMA mode we'd get this
			 * from the RXFIFO, instead we need to process
			 * the setup here.
			 */

			if (dir_in)
				WARN_ON_ONCE(1);
			else
<<<<<<< HEAD
				s3c_hsotg_handle_outdone(hsotg, 0, true);
=======
				dwc2_hsotg_handle_outdone(hsotg, 0);
>>>>>>> v4.9.227
		}
	}

	if (ints & DXEPINT_BACK2BACKSETUP)
		dev_dbg(hsotg->dev, "%s: B2BSetup/INEPNakEff\n", __func__);

	if (dir_in && !hs_ep->isochronous) {
		/* not sure if this is important, but we'll clear it anyway */
<<<<<<< HEAD
		if (ints & DIEPMSK_INTKNTXFEMPMSK) {
=======
		if (ints & DXEPINT_INTKNTXFEMP) {
>>>>>>> v4.9.227
			dev_dbg(hsotg->dev, "%s: ep%d: INTknTXFEmpMsk\n",
				__func__, idx);
		}

		/* this probably means something bad is happening */
<<<<<<< HEAD
		if (ints & DIEPMSK_INTKNEPMISMSK) {
=======
		if (ints & DXEPINT_INTKNEPMIS) {
>>>>>>> v4.9.227
			dev_warn(hsotg->dev, "%s: ep%d: INTknEP\n",
				 __func__, idx);
		}

		/* FIFO has space or is empty (see GAHBCFG) */
		if (hsotg->dedicated_fifos &&
<<<<<<< HEAD
		    ints & DIEPMSK_TXFIFOEMPTY) {
			dev_dbg(hsotg->dev, "%s: ep%d: TxFIFOEmpty\n",
				__func__, idx);
			if (!using_dma(hsotg))
				s3c_hsotg_trytx(hsotg, hs_ep);
=======
		    ints & DXEPINT_TXFEMP) {
			dev_dbg(hsotg->dev, "%s: ep%d: TxFIFOEmpty\n",
				__func__, idx);
			if (!using_dma(hsotg))
				dwc2_hsotg_trytx(hsotg, hs_ep);
>>>>>>> v4.9.227
		}
	}
}

/**
<<<<<<< HEAD
 * s3c_hsotg_irq_enumdone - Handle EnumDone interrupt (enumeration done)
=======
 * dwc2_hsotg_irq_enumdone - Handle EnumDone interrupt (enumeration done)
>>>>>>> v4.9.227
 * @hsotg: The device state.
 *
 * Handle updating the device settings after the enumeration phase has
 * been completed.
 */
<<<<<<< HEAD
static void s3c_hsotg_irq_enumdone(struct s3c_hsotg *hsotg)
{
	u32 dsts = readl(hsotg->regs + DSTS);
=======
static void dwc2_hsotg_irq_enumdone(struct dwc2_hsotg *hsotg)
{
	u32 dsts = dwc2_readl(hsotg->regs + DSTS);
>>>>>>> v4.9.227
	int ep0_mps = 0, ep_mps = 8;

	/*
	 * This should signal the finish of the enumeration phase
	 * of the USB handshaking, so we should now know what rate
	 * we connected at.
	 */

	dev_dbg(hsotg->dev, "EnumDone (DSTS=0x%08x)\n", dsts);

	/*
	 * note, since we're limited by the size of transfer on EP0, and
	 * it seems IN transfers must be a even number of packets we do
	 * not advertise a 64byte MPS on EP0.
	 */

	/* catch both EnumSpd_FS and EnumSpd_FS48 */
<<<<<<< HEAD
	switch (dsts & DSTS_ENUMSPD_MASK) {
=======
	switch ((dsts & DSTS_ENUMSPD_MASK) >> DSTS_ENUMSPD_SHIFT) {
>>>>>>> v4.9.227
	case DSTS_ENUMSPD_FS:
	case DSTS_ENUMSPD_FS48:
		hsotg->gadget.speed = USB_SPEED_FULL;
		ep0_mps = EP0_MPS_LIMIT;
		ep_mps = 1023;
		break;

	case DSTS_ENUMSPD_HS:
		hsotg->gadget.speed = USB_SPEED_HIGH;
		ep0_mps = EP0_MPS_LIMIT;
		ep_mps = 1024;
		break;

	case DSTS_ENUMSPD_LS:
		hsotg->gadget.speed = USB_SPEED_LOW;
		/*
		 * note, we don't actually support LS in this driver at the
		 * moment, and the documentation seems to imply that it isn't
		 * supported by the PHYs on some of the devices.
		 */
		break;
	}
	dev_info(hsotg->dev, "new device is %s\n",
		 usb_speed_string(hsotg->gadget.speed));

	/*
	 * we should now know the maximum packet size for an
	 * endpoint, so set the endpoints to a default value.
	 */

	if (ep0_mps) {
		int i;
<<<<<<< HEAD
		s3c_hsotg_set_ep_maxpacket(hsotg, 0, ep0_mps);
		for (i = 1; i < hsotg->num_of_eps; i++)
			s3c_hsotg_set_ep_maxpacket(hsotg, i, ep_mps);
=======
		/* Initialize ep0 for both in and out directions */
		dwc2_hsotg_set_ep_maxpacket(hsotg, 0, ep0_mps, 1);
		dwc2_hsotg_set_ep_maxpacket(hsotg, 0, ep0_mps, 0);
		for (i = 1; i < hsotg->num_of_eps; i++) {
			if (hsotg->eps_in[i])
				dwc2_hsotg_set_ep_maxpacket(hsotg, i, ep_mps, 1);
			if (hsotg->eps_out[i])
				dwc2_hsotg_set_ep_maxpacket(hsotg, i, ep_mps, 0);
		}
>>>>>>> v4.9.227
	}

	/* ensure after enumeration our EP0 is active */

<<<<<<< HEAD
	s3c_hsotg_enqueue_setup(hsotg);

	dev_dbg(hsotg->dev, "EP0: DIEPCTL0=0x%08x, DOEPCTL0=0x%08x\n",
		readl(hsotg->regs + DIEPCTL0),
		readl(hsotg->regs + DOEPCTL0));
=======
	dwc2_hsotg_enqueue_setup(hsotg);

	dev_dbg(hsotg->dev, "EP0: DIEPCTL0=0x%08x, DOEPCTL0=0x%08x\n",
		dwc2_readl(hsotg->regs + DIEPCTL0),
		dwc2_readl(hsotg->regs + DOEPCTL0));
>>>>>>> v4.9.227
}

/**
 * kill_all_requests - remove all requests from the endpoint's queue
 * @hsotg: The device state.
 * @ep: The endpoint the requests may be on.
 * @result: The result code to use.
<<<<<<< HEAD
 * @force: Force removal of any current requests
=======
>>>>>>> v4.9.227
 *
 * Go through the requests on the given endpoint and mark them
 * completed with the given result code.
 */
<<<<<<< HEAD
static void kill_all_requests(struct s3c_hsotg *hsotg,
			      struct s3c_hsotg_ep *ep,
			      int result, bool force)
{
	struct s3c_hsotg_req *req, *treq;
	unsigned size;

	list_for_each_entry_safe(req, treq, &ep->queue, queue) {
		/*
		 * currently, we can't do much about an already
		 * running request on an in endpoint
		 */

		if (ep->req == req && ep->dir_in && !force)
			continue;

		s3c_hsotg_complete_request(hsotg, ep, req,
					   result);
	}
	if (!hsotg->dedicated_fifos)
		return;
	size = (readl(hsotg->regs + DTXFSTS(ep->index)) & 0xffff) * 4;
	if (size < ep->fifo_size)
		s3c_hsotg_txfifo_flush(hsotg, ep->fifo_index);
}

/**
 * s3c_hsotg_disconnect - disconnect service
=======
static void kill_all_requests(struct dwc2_hsotg *hsotg,
			      struct dwc2_hsotg_ep *ep,
			      int result)
{
	struct dwc2_hsotg_req *req, *treq;
	unsigned size;

	ep->req = NULL;

	list_for_each_entry_safe(req, treq, &ep->queue, queue)
		dwc2_hsotg_complete_request(hsotg, ep, req,
					   result);

	if (!hsotg->dedicated_fifos)
		return;
	size = (dwc2_readl(hsotg->regs + DTXFSTS(ep->fifo_index)) & 0xffff) * 4;
	if (size < ep->fifo_size)
		dwc2_hsotg_txfifo_flush(hsotg, ep->fifo_index);
}

/**
 * dwc2_hsotg_disconnect - disconnect service
>>>>>>> v4.9.227
 * @hsotg: The device state.
 *
 * The device has been disconnected. Remove all current
 * transactions and signal the gadget driver that this
 * has happened.
 */
<<<<<<< HEAD
static void s3c_hsotg_disconnect(struct s3c_hsotg *hsotg)
{
	unsigned ep;

	for (ep = 0; ep < hsotg->num_of_eps; ep++)
		kill_all_requests(hsotg, &hsotg->eps[ep], -ESHUTDOWN, true);

	call_gadget(hsotg, disconnect);
}

/**
 * s3c_hsotg_irq_fifoempty - TX FIFO empty interrupt handler
 * @hsotg: The device state:
 * @periodic: True if this is a periodic FIFO interrupt
 */
static void s3c_hsotg_irq_fifoempty(struct s3c_hsotg *hsotg, bool periodic)
{
	struct s3c_hsotg_ep *ep;
	int epno, ret;

	/* look through for any more data to transmit */

	for (epno = 0; epno < hsotg->num_of_eps; epno++) {
		ep = &hsotg->eps[epno];
=======
void dwc2_hsotg_disconnect(struct dwc2_hsotg *hsotg)
{
	unsigned ep;

	if (!hsotg->connected)
		return;

	hsotg->connected = 0;
	hsotg->test_mode = 0;

	for (ep = 0; ep < hsotg->num_of_eps; ep++) {
		if (hsotg->eps_in[ep])
			kill_all_requests(hsotg, hsotg->eps_in[ep],
								-ESHUTDOWN);
		if (hsotg->eps_out[ep])
			kill_all_requests(hsotg, hsotg->eps_out[ep],
								-ESHUTDOWN);
	}

	call_gadget(hsotg, disconnect);
	hsotg->lx_state = DWC2_L3;
}

/**
 * dwc2_hsotg_irq_fifoempty - TX FIFO empty interrupt handler
 * @hsotg: The device state:
 * @periodic: True if this is a periodic FIFO interrupt
 */
static void dwc2_hsotg_irq_fifoempty(struct dwc2_hsotg *hsotg, bool periodic)
{
	struct dwc2_hsotg_ep *ep;
	int epno, ret;

	/* look through for any more data to transmit */
	for (epno = 0; epno < hsotg->num_of_eps; epno++) {
		ep = index_to_ep(hsotg, epno, 1);

		if (!ep)
			continue;
>>>>>>> v4.9.227

		if (!ep->dir_in)
			continue;

		if ((periodic && !ep->periodic) ||
		    (!periodic && ep->periodic))
			continue;

<<<<<<< HEAD
		ret = s3c_hsotg_trytx(hsotg, ep);
=======
		ret = dwc2_hsotg_trytx(hsotg, ep);
>>>>>>> v4.9.227
		if (ret < 0)
			break;
	}
}

/* IRQ flags which will trigger a retry around the IRQ loop */
#define IRQ_RETRY_MASK (GINTSTS_NPTXFEMP | \
			GINTSTS_PTXFEMP |  \
			GINTSTS_RXFLVL)

/**
<<<<<<< HEAD
 * s3c_hsotg_corereset - issue softreset to the core
=======
 * dwc2_hsotg_core_init - issue softreset to the core
>>>>>>> v4.9.227
 * @hsotg: The device state
 *
 * Issue a soft reset to the core, and await the core finishing it.
 */
<<<<<<< HEAD
static int s3c_hsotg_corereset(struct s3c_hsotg *hsotg)
{
	int timeout;
	u32 grstctl;

	dev_dbg(hsotg->dev, "resetting core\n");

	/* issue soft reset */
	writel(GRSTCTL_CSFTRST, hsotg->regs + GRSTCTL);

	timeout = 10000;
	do {
		grstctl = readl(hsotg->regs + GRSTCTL);
	} while ((grstctl & GRSTCTL_CSFTRST) && timeout-- > 0);

	if (grstctl & GRSTCTL_CSFTRST) {
		dev_err(hsotg->dev, "Failed to get CSftRst asserted\n");
		return -EINVAL;
	}

	timeout = 10000;

	while (1) {
		u32 grstctl = readl(hsotg->regs + GRSTCTL);

		if (timeout-- < 0) {
			dev_info(hsotg->dev,
				 "%s: reset failed, GRSTCTL=%08x\n",
				 __func__, grstctl);
			return -ETIMEDOUT;
		}

		if (!(grstctl & GRSTCTL_AHBIDLE))
			continue;

		break;		/* reset done */
	}

	dev_dbg(hsotg->dev, "reset successful\n");
	return 0;
}

/**
 * s3c_hsotg_core_init - issue softreset to the core
 * @hsotg: The device state
 *
 * Issue a soft reset to the core, and await the core finishing it.
 */
static void s3c_hsotg_core_init(struct s3c_hsotg *hsotg)
{
	s3c_hsotg_corereset(hsotg);
=======
void dwc2_hsotg_core_init_disconnected(struct dwc2_hsotg *hsotg,
						bool is_usb_reset)
{
	u32 intmsk;
	u32 val;
	u32 usbcfg;

	/* Kill any ep0 requests as controller will be reinitialized */
	kill_all_requests(hsotg, hsotg->eps_out[0], -ECONNRESET);

	if (!is_usb_reset)
		if (dwc2_core_reset(hsotg))
			return;
>>>>>>> v4.9.227

	/*
	 * we must now enable ep0 ready for host detection and then
	 * set configuration.
	 */

<<<<<<< HEAD
	/* set the PLL on, remove the HNP/SRP and set the PHY */
	writel(hsotg->phyif | GUSBCFG_TOUTCAL(7) |
	       (0x5 << 10), hsotg->regs + GUSBCFG);

	s3c_hsotg_init_fifo(hsotg);

	__orr32(hsotg->regs + DCTL, DCTL_SFTDISCON);

	writel(1 << 18 | DCFG_DEVSPD_HS,  hsotg->regs + DCFG);

	/* Clear any pending OTG interrupts */
	writel(0xffffffff, hsotg->regs + GOTGINT);

	/* Clear any pending interrupts */
	writel(0xffffffff, hsotg->regs + GINTSTS);

	writel(GINTSTS_ERLYSUSP | GINTSTS_SESSREQINT |
		GINTSTS_GOUTNAKEFF | GINTSTS_GINNAKEFF |
		GINTSTS_CONIDSTSCHNG | GINTSTS_USBRST |
		GINTSTS_ENUMDONE | GINTSTS_OTGINT |
		GINTSTS_USBSUSP | GINTSTS_WKUPINT,
		hsotg->regs + GINTMSK);

	if (using_dma(hsotg))
		writel(GAHBCFG_GLBL_INTR_EN | GAHBCFG_DMA_EN |
		       GAHBCFG_HBSTLEN_INCR4,
		       hsotg->regs + GAHBCFG);
	else
		writel(((hsotg->dedicated_fifos) ? (GAHBCFG_NP_TXF_EMP_LVL |
						    GAHBCFG_P_TXF_EMP_LVL) : 0) |
		       GAHBCFG_GLBL_INTR_EN,
		       hsotg->regs + GAHBCFG);
=======
	/* keep other bits untouched (so e.g. forced modes are not lost) */
	usbcfg = dwc2_readl(hsotg->regs + GUSBCFG);
	usbcfg &= ~(GUSBCFG_TOUTCAL_MASK | GUSBCFG_PHYIF16 | GUSBCFG_SRPCAP |
		GUSBCFG_HNPCAP | GUSBCFG_USBTRDTIM_MASK);

	/* set the PLL on, remove the HNP/SRP and set the PHY */
	val = (hsotg->phyif == GUSBCFG_PHYIF8) ? 9 : 5;
	usbcfg |= hsotg->phyif | GUSBCFG_TOUTCAL(7) |
		(val << GUSBCFG_USBTRDTIM_SHIFT);
	dwc2_writel(usbcfg, hsotg->regs + GUSBCFG);

	dwc2_hsotg_init_fifo(hsotg);

	if (!is_usb_reset)
		__orr32(hsotg->regs + DCTL, DCTL_SFTDISCON);

	dwc2_writel(DCFG_EPMISCNT(1) | DCFG_DEVSPD_HS,  hsotg->regs + DCFG);

	/* Clear any pending OTG interrupts */
	dwc2_writel(0xffffffff, hsotg->regs + GOTGINT);

	/* Clear any pending interrupts */
	dwc2_writel(0xffffffff, hsotg->regs + GINTSTS);
	intmsk = GINTSTS_ERLYSUSP | GINTSTS_SESSREQINT |
		GINTSTS_GOUTNAKEFF | GINTSTS_GINNAKEFF |
		GINTSTS_USBRST | GINTSTS_RESETDET |
		GINTSTS_ENUMDONE | GINTSTS_OTGINT |
		GINTSTS_USBSUSP | GINTSTS_WKUPINT |
		GINTSTS_INCOMPL_SOIN | GINTSTS_INCOMPL_SOOUT;

	if (hsotg->core_params->external_id_pin_ctl <= 0)
		intmsk |= GINTSTS_CONIDSTSCHNG;

	dwc2_writel(intmsk, hsotg->regs + GINTMSK);

	if (using_dma(hsotg))
		dwc2_writel(GAHBCFG_GLBL_INTR_EN | GAHBCFG_DMA_EN |
			    (GAHBCFG_HBSTLEN_INCR4 << GAHBCFG_HBSTLEN_SHIFT),
			    hsotg->regs + GAHBCFG);
	else
		dwc2_writel(((hsotg->dedicated_fifos) ?
						(GAHBCFG_NP_TXF_EMP_LVL |
						 GAHBCFG_P_TXF_EMP_LVL) : 0) |
			    GAHBCFG_GLBL_INTR_EN, hsotg->regs + GAHBCFG);
>>>>>>> v4.9.227

	/*
	 * If INTknTXFEmpMsk is enabled, it's important to disable ep interrupts
	 * when we have no data to transfer. Otherwise we get being flooded by
	 * interrupts.
	 */

<<<<<<< HEAD
	writel(((hsotg->dedicated_fifos) ? DIEPMSK_TXFIFOEMPTY |
		DIEPMSK_INTKNTXFEMPMSK : 0) |
		DIEPMSK_EPDISBLDMSK | DIEPMSK_XFERCOMPLMSK |
		DIEPMSK_TIMEOUTMSK | DIEPMSK_AHBERRMSK |
		DIEPMSK_INTKNEPMISMSK,
=======
	dwc2_writel(((hsotg->dedicated_fifos && !using_dma(hsotg)) ?
		DIEPMSK_TXFIFOEMPTY | DIEPMSK_INTKNTXFEMPMSK : 0) |
		DIEPMSK_EPDISBLDMSK | DIEPMSK_XFERCOMPLMSK |
		DIEPMSK_TIMEOUTMSK | DIEPMSK_AHBERRMSK,
>>>>>>> v4.9.227
		hsotg->regs + DIEPMSK);

	/*
	 * don't need XferCompl, we get that from RXFIFO in slave mode. In
	 * DMA mode we may need this.
	 */
<<<<<<< HEAD
	writel((using_dma(hsotg) ? (DIEPMSK_XFERCOMPLMSK |
				    DIEPMSK_TIMEOUTMSK) : 0) |
		DOEPMSK_EPDISBLDMSK | DOEPMSK_AHBERRMSK |
		DOEPMSK_SETUPMSK,
		hsotg->regs + DOEPMSK);

	writel(0, hsotg->regs + DAINTMSK);

	dev_dbg(hsotg->dev, "EP0: DIEPCTL0=0x%08x, DOEPCTL0=0x%08x\n",
		readl(hsotg->regs + DIEPCTL0),
		readl(hsotg->regs + DOEPCTL0));

	/* enable in and out endpoint interrupts */
	s3c_hsotg_en_gsint(hsotg, GINTSTS_OEPINT | GINTSTS_IEPINT);
=======
	dwc2_writel((using_dma(hsotg) ? (DIEPMSK_XFERCOMPLMSK) : 0) |
		DOEPMSK_EPDISBLDMSK | DOEPMSK_AHBERRMSK |
		DOEPMSK_SETUPMSK | DOEPMSK_STSPHSERCVDMSK,
		hsotg->regs + DOEPMSK);

	dwc2_writel(0, hsotg->regs + DAINTMSK);

	dev_dbg(hsotg->dev, "EP0: DIEPCTL0=0x%08x, DOEPCTL0=0x%08x\n",
		dwc2_readl(hsotg->regs + DIEPCTL0),
		dwc2_readl(hsotg->regs + DOEPCTL0));

	/* enable in and out endpoint interrupts */
	dwc2_hsotg_en_gsint(hsotg, GINTSTS_OEPINT | GINTSTS_IEPINT);
>>>>>>> v4.9.227

	/*
	 * Enable the RXFIFO when in slave mode, as this is how we collect
	 * the data. In DMA mode, we get events from the FIFO but also
	 * things we cannot process, so do not use it.
	 */
	if (!using_dma(hsotg))
<<<<<<< HEAD
		s3c_hsotg_en_gsint(hsotg, GINTSTS_RXFLVL);

	/* Enable interrupts for EP0 in and out */
	s3c_hsotg_ctrl_epint(hsotg, 0, 0, 1);
	s3c_hsotg_ctrl_epint(hsotg, 0, 1, 1);

	__orr32(hsotg->regs + DCTL, DCTL_PWRONPRGDONE);
	udelay(10);  /* see openiboot */
	__bic32(hsotg->regs + DCTL, DCTL_PWRONPRGDONE);

	dev_dbg(hsotg->dev, "DCTL=0x%08x\n", readl(hsotg->regs + DCTL));
=======
		dwc2_hsotg_en_gsint(hsotg, GINTSTS_RXFLVL);

	/* Enable interrupts for EP0 in and out */
	dwc2_hsotg_ctrl_epint(hsotg, 0, 0, 1);
	dwc2_hsotg_ctrl_epint(hsotg, 0, 1, 1);

	if (!is_usb_reset) {
		__orr32(hsotg->regs + DCTL, DCTL_PWRONPRGDONE);
		udelay(10);  /* see openiboot */
		__bic32(hsotg->regs + DCTL, DCTL_PWRONPRGDONE);
	}

	dev_dbg(hsotg->dev, "DCTL=0x%08x\n", dwc2_readl(hsotg->regs + DCTL));
>>>>>>> v4.9.227

	/*
	 * DxEPCTL_USBActEp says RO in manual, but seems to be set by
	 * writing to the EPCTL register..
	 */

	/* set to read 1 8byte packet */
<<<<<<< HEAD
	writel(DXEPTSIZ_MC(1) | DXEPTSIZ_PKTCNT(1) |
	       DXEPTSIZ_XFERSIZE(8), hsotg->regs + DOEPTSIZ0);

	writel(s3c_hsotg_ep0_mps(hsotg->eps[0].ep.maxpacket) |
=======
	dwc2_writel(DXEPTSIZ_MC(1) | DXEPTSIZ_PKTCNT(1) |
	       DXEPTSIZ_XFERSIZE(8), hsotg->regs + DOEPTSIZ0);

	dwc2_writel(dwc2_hsotg_ep0_mps(hsotg->eps_out[0]->ep.maxpacket) |
>>>>>>> v4.9.227
	       DXEPCTL_CNAK | DXEPCTL_EPENA |
	       DXEPCTL_USBACTEP,
	       hsotg->regs + DOEPCTL0);

	/* enable, but don't activate EP0in */
<<<<<<< HEAD
	writel(s3c_hsotg_ep0_mps(hsotg->eps[0].ep.maxpacket) |
	       DXEPCTL_USBACTEP, hsotg->regs + DIEPCTL0);

	s3c_hsotg_enqueue_setup(hsotg);

	dev_dbg(hsotg->dev, "EP0: DIEPCTL0=0x%08x, DOEPCTL0=0x%08x\n",
		readl(hsotg->regs + DIEPCTL0),
		readl(hsotg->regs + DOEPCTL0));

	/* clear global NAKs */
	writel(DCTL_CGOUTNAK | DCTL_CGNPINNAK,
	       hsotg->regs + DCTL);
=======
	dwc2_writel(dwc2_hsotg_ep0_mps(hsotg->eps_out[0]->ep.maxpacket) |
	       DXEPCTL_USBACTEP, hsotg->regs + DIEPCTL0);

	/* clear global NAKs */
	val = DCTL_CGOUTNAK | DCTL_CGNPINNAK;
	if (!is_usb_reset)
		val |= DCTL_SFTDISCON;
	__orr32(hsotg->regs + DCTL, val);
>>>>>>> v4.9.227

	/* must be at-least 3ms to allow bus to see disconnect */
	mdelay(3);

<<<<<<< HEAD
=======
	hsotg->lx_state = DWC2_L0;

	dwc2_hsotg_enqueue_setup(hsotg);

	dev_dbg(hsotg->dev, "EP0: DIEPCTL0=0x%08x, DOEPCTL0=0x%08x\n",
		dwc2_readl(hsotg->regs + DIEPCTL0),
		dwc2_readl(hsotg->regs + DOEPCTL0));
}

static void dwc2_hsotg_core_disconnect(struct dwc2_hsotg *hsotg)
{
	/* set the soft-disconnect bit */
	__orr32(hsotg->regs + DCTL, DCTL_SFTDISCON);
}

void dwc2_hsotg_core_connect(struct dwc2_hsotg *hsotg)
{
>>>>>>> v4.9.227
	/* remove the soft-disconnect and let's go */
	__bic32(hsotg->regs + DCTL, DCTL_SFTDISCON);
}

/**
<<<<<<< HEAD
 * s3c_hsotg_irq - handle device interrupt
 * @irq: The IRQ number triggered
 * @pw: The pw value when registered the handler.
 */
static irqreturn_t s3c_hsotg_irq(int irq, void *pw)
{
	struct s3c_hsotg *hsotg = pw;
=======
 * dwc2_gadget_handle_incomplete_isoc_in - handle incomplete ISO IN Interrupt.
 * @hsotg: The device state:
 *
 * This interrupt indicates one of the following conditions occurred while
 * transmitting an ISOC transaction.
 * - Corrupted IN Token for ISOC EP.
 * - Packet not complete in FIFO.
 *
 * The following actions will be taken:
 * - Determine the EP
 * - Disable EP; when 'Endpoint Disabled' interrupt is received Flush FIFO
 */
static void dwc2_gadget_handle_incomplete_isoc_in(struct dwc2_hsotg *hsotg)
{
	struct dwc2_hsotg_ep *hs_ep;
	u32 epctrl;
	u32 idx;

	dev_dbg(hsotg->dev, "Incomplete isoc in interrupt received:\n");

	for (idx = 1; idx <= hsotg->num_of_eps; idx++) {
		hs_ep = hsotg->eps_in[idx];
		epctrl = dwc2_readl(hsotg->regs + DIEPCTL(idx));
		if ((epctrl & DXEPCTL_EPENA) && hs_ep->isochronous &&
		    dwc2_gadget_target_frame_elapsed(hs_ep)) {
			epctrl |= DXEPCTL_SNAK;
			epctrl |= DXEPCTL_EPDIS;
			dwc2_writel(epctrl, hsotg->regs + DIEPCTL(idx));
		}
	}

	/* Clear interrupt */
	dwc2_writel(GINTSTS_INCOMPL_SOIN, hsotg->regs + GINTSTS);
}

/**
 * dwc2_gadget_handle_incomplete_isoc_out - handle incomplete ISO OUT Interrupt
 * @hsotg: The device state:
 *
 * This interrupt indicates one of the following conditions occurred while
 * transmitting an ISOC transaction.
 * - Corrupted OUT Token for ISOC EP.
 * - Packet not complete in FIFO.
 *
 * The following actions will be taken:
 * - Determine the EP
 * - Set DCTL_SGOUTNAK and unmask GOUTNAKEFF if target frame elapsed.
 */
static void dwc2_gadget_handle_incomplete_isoc_out(struct dwc2_hsotg *hsotg)
{
	u32 gintsts;
	u32 gintmsk;
	u32 epctrl;
	struct dwc2_hsotg_ep *hs_ep;
	int idx;

	dev_dbg(hsotg->dev, "%s: GINTSTS_INCOMPL_SOOUT\n", __func__);

	for (idx = 1; idx <= hsotg->num_of_eps; idx++) {
		hs_ep = hsotg->eps_out[idx];
		epctrl = dwc2_readl(hsotg->regs + DOEPCTL(idx));
		if ((epctrl & DXEPCTL_EPENA) && hs_ep->isochronous &&
		    dwc2_gadget_target_frame_elapsed(hs_ep)) {
			/* Unmask GOUTNAKEFF interrupt */
			gintmsk = dwc2_readl(hsotg->regs + GINTMSK);
			gintmsk |= GINTSTS_GOUTNAKEFF;
			dwc2_writel(gintmsk, hsotg->regs + GINTMSK);

			gintsts = dwc2_readl(hsotg->regs + GINTSTS);
			if (!(gintsts & GINTSTS_GOUTNAKEFF))
				__orr32(hsotg->regs + DCTL, DCTL_SGOUTNAK);
		}
	}

	/* Clear interrupt */
	dwc2_writel(GINTSTS_INCOMPL_SOOUT, hsotg->regs + GINTSTS);
}

/**
 * dwc2_hsotg_irq - handle device interrupt
 * @irq: The IRQ number triggered
 * @pw: The pw value when registered the handler.
 */
static irqreturn_t dwc2_hsotg_irq(int irq, void *pw)
{
	struct dwc2_hsotg *hsotg = pw;
>>>>>>> v4.9.227
	int retry_count = 8;
	u32 gintsts;
	u32 gintmsk;

<<<<<<< HEAD
	spin_lock(&hsotg->lock);
irq_retry:
	gintsts = readl(hsotg->regs + GINTSTS);
	gintmsk = readl(hsotg->regs + GINTMSK);
=======
	if (!dwc2_is_device_mode(hsotg))
		return IRQ_NONE;

	spin_lock(&hsotg->lock);
irq_retry:
	gintsts = dwc2_readl(hsotg->regs + GINTSTS);
	gintmsk = dwc2_readl(hsotg->regs + GINTMSK);
>>>>>>> v4.9.227

	dev_dbg(hsotg->dev, "%s: %08x %08x (%08x) retry %d\n",
		__func__, gintsts, gintsts & gintmsk, gintmsk, retry_count);

	gintsts &= gintmsk;

<<<<<<< HEAD
	if (gintsts & GINTSTS_OTGINT) {
		u32 otgint = readl(hsotg->regs + GOTGINT);

		dev_info(hsotg->dev, "OTGInt: %08x\n", otgint);

		writel(otgint, hsotg->regs + GOTGINT);
	}

	if (gintsts & GINTSTS_SESSREQINT) {
		dev_dbg(hsotg->dev, "%s: SessReqInt\n", __func__);
		writel(GINTSTS_SESSREQINT, hsotg->regs + GINTSTS);
	}

	if (gintsts & GINTSTS_ENUMDONE) {
		writel(GINTSTS_ENUMDONE, hsotg->regs + GINTSTS);

		s3c_hsotg_irq_enumdone(hsotg);
	}

	if (gintsts & GINTSTS_CONIDSTSCHNG) {
		dev_dbg(hsotg->dev, "ConIDStsChg (DSTS=0x%08x, GOTCTL=%08x)\n",
			readl(hsotg->regs + DSTS),
			readl(hsotg->regs + GOTGCTL));

		writel(GINTSTS_CONIDSTSCHNG, hsotg->regs + GINTSTS);
	}

	if (gintsts & (GINTSTS_OEPINT | GINTSTS_IEPINT)) {
		u32 daint = readl(hsotg->regs + DAINT);
		u32 daintmsk = readl(hsotg->regs + DAINTMSK);
=======
	if (gintsts & GINTSTS_RESETDET) {
		dev_dbg(hsotg->dev, "%s: USBRstDet\n", __func__);

		dwc2_writel(GINTSTS_RESETDET, hsotg->regs + GINTSTS);

		/* This event must be used only if controller is suspended */
		if (hsotg->lx_state == DWC2_L2) {
			dwc2_exit_hibernation(hsotg, true);
			hsotg->lx_state = DWC2_L0;
		}
	}

	if (gintsts & (GINTSTS_USBRST | GINTSTS_RESETDET)) {

		u32 usb_status = dwc2_readl(hsotg->regs + GOTGCTL);
		u32 connected = hsotg->connected;

		dev_dbg(hsotg->dev, "%s: USBRst\n", __func__);
		dev_dbg(hsotg->dev, "GNPTXSTS=%08x\n",
			dwc2_readl(hsotg->regs + GNPTXSTS));

		dwc2_writel(GINTSTS_USBRST, hsotg->regs + GINTSTS);

		/* Report disconnection if it is not already done. */
		dwc2_hsotg_disconnect(hsotg);

		if (usb_status & GOTGCTL_BSESVLD && connected)
			dwc2_hsotg_core_init_disconnected(hsotg, true);
	}

	if (gintsts & GINTSTS_ENUMDONE) {
		dwc2_writel(GINTSTS_ENUMDONE, hsotg->regs + GINTSTS);

		dwc2_hsotg_irq_enumdone(hsotg);
	}

	if (gintsts & (GINTSTS_OEPINT | GINTSTS_IEPINT)) {
		u32 daint = dwc2_readl(hsotg->regs + DAINT);
		u32 daintmsk = dwc2_readl(hsotg->regs + DAINTMSK);
>>>>>>> v4.9.227
		u32 daint_out, daint_in;
		int ep;

		daint &= daintmsk;
		daint_out = daint >> DAINT_OUTEP_SHIFT;
		daint_in = daint & ~(daint_out << DAINT_OUTEP_SHIFT);

		dev_dbg(hsotg->dev, "%s: daint=%08x\n", __func__, daint);

<<<<<<< HEAD
		for (ep = 0; ep < 15 && daint_out; ep++, daint_out >>= 1) {
			if (daint_out & 1)
				s3c_hsotg_epint(hsotg, ep, 0);
		}

		for (ep = 0; ep < 15 && daint_in; ep++, daint_in >>= 1) {
			if (daint_in & 1)
				s3c_hsotg_epint(hsotg, ep, 1);
		}
	}

	if (gintsts & GINTSTS_USBRST) {

		u32 usb_status = readl(hsotg->regs + GOTGCTL);

		dev_dbg(hsotg->dev, "%s: USBRst\n", __func__);
		dev_dbg(hsotg->dev, "GNPTXSTS=%08x\n",
			readl(hsotg->regs + GNPTXSTS));

		writel(GINTSTS_USBRST, hsotg->regs + GINTSTS);

		if (usb_status & GOTGCTL_BSESVLD) {
			if (time_after(jiffies, hsotg->last_rst +
				       msecs_to_jiffies(200))) {

				kill_all_requests(hsotg, &hsotg->eps[0],
							  -ECONNRESET, true);

				s3c_hsotg_core_init(hsotg);
				hsotg->last_rst = jiffies;
			}
=======
		for (ep = 0; ep < hsotg->num_of_eps && daint_out;
						ep++, daint_out >>= 1) {
			if (daint_out & 1)
				dwc2_hsotg_epint(hsotg, ep, 0);
		}

		for (ep = 0; ep < hsotg->num_of_eps  && daint_in;
						ep++, daint_in >>= 1) {
			if (daint_in & 1)
				dwc2_hsotg_epint(hsotg, ep, 1);
>>>>>>> v4.9.227
		}
	}

	/* check both FIFOs */

	if (gintsts & GINTSTS_NPTXFEMP) {
		dev_dbg(hsotg->dev, "NPTxFEmp\n");

		/*
		 * Disable the interrupt to stop it happening again
		 * unless one of these endpoint routines decides that
		 * it needs re-enabling
		 */

<<<<<<< HEAD
		s3c_hsotg_disable_gsint(hsotg, GINTSTS_NPTXFEMP);
		s3c_hsotg_irq_fifoempty(hsotg, false);
=======
		dwc2_hsotg_disable_gsint(hsotg, GINTSTS_NPTXFEMP);
		dwc2_hsotg_irq_fifoempty(hsotg, false);
>>>>>>> v4.9.227
	}

	if (gintsts & GINTSTS_PTXFEMP) {
		dev_dbg(hsotg->dev, "PTxFEmp\n");

		/* See note in GINTSTS_NPTxFEmp */

<<<<<<< HEAD
		s3c_hsotg_disable_gsint(hsotg, GINTSTS_PTXFEMP);
		s3c_hsotg_irq_fifoempty(hsotg, true);
=======
		dwc2_hsotg_disable_gsint(hsotg, GINTSTS_PTXFEMP);
		dwc2_hsotg_irq_fifoempty(hsotg, true);
>>>>>>> v4.9.227
	}

	if (gintsts & GINTSTS_RXFLVL) {
		/*
		 * note, since GINTSTS_RxFLvl doubles as FIFO-not-empty,
<<<<<<< HEAD
		 * we need to retry s3c_hsotg_handle_rx if this is still
		 * set.
		 */

		s3c_hsotg_handle_rx(hsotg);
	}

	if (gintsts & GINTSTS_MODEMIS) {
		dev_warn(hsotg->dev, "warning, mode mismatch triggered\n");
		writel(GINTSTS_MODEMIS, hsotg->regs + GINTSTS);
	}

	if (gintsts & GINTSTS_USBSUSP) {
		dev_info(hsotg->dev, "GINTSTS_USBSusp\n");
		writel(GINTSTS_USBSUSP, hsotg->regs + GINTSTS);

		call_gadget(hsotg, suspend);
	}

	if (gintsts & GINTSTS_WKUPINT) {
		dev_info(hsotg->dev, "GINTSTS_WkUpIn\n");
		writel(GINTSTS_WKUPINT, hsotg->regs + GINTSTS);

		call_gadget(hsotg, resume);
=======
		 * we need to retry dwc2_hsotg_handle_rx if this is still
		 * set.
		 */

		dwc2_hsotg_handle_rx(hsotg);
>>>>>>> v4.9.227
	}

	if (gintsts & GINTSTS_ERLYSUSP) {
		dev_dbg(hsotg->dev, "GINTSTS_ErlySusp\n");
<<<<<<< HEAD
		writel(GINTSTS_ERLYSUSP, hsotg->regs + GINTSTS);
=======
		dwc2_writel(GINTSTS_ERLYSUSP, hsotg->regs + GINTSTS);
>>>>>>> v4.9.227
	}

	/*
	 * these next two seem to crop-up occasionally causing the core
	 * to shutdown the USB transfer, so try clearing them and logging
	 * the occurrence.
	 */

	if (gintsts & GINTSTS_GOUTNAKEFF) {
<<<<<<< HEAD
		dev_info(hsotg->dev, "GOUTNakEff triggered\n");

		writel(DCTL_CGOUTNAK, hsotg->regs + DCTL);

		s3c_hsotg_dump(hsotg);
=======
		u8 idx;
		u32 epctrl;
		u32 gintmsk;
		struct dwc2_hsotg_ep *hs_ep;

		/* Mask this interrupt */
		gintmsk = dwc2_readl(hsotg->regs + GINTMSK);
		gintmsk &= ~GINTSTS_GOUTNAKEFF;
		dwc2_writel(gintmsk, hsotg->regs + GINTMSK);

		dev_dbg(hsotg->dev, "GOUTNakEff triggered\n");
		for (idx = 1; idx <= hsotg->num_of_eps; idx++) {
			hs_ep = hsotg->eps_out[idx];
			epctrl = dwc2_readl(hsotg->regs + DOEPCTL(idx));

			if ((epctrl & DXEPCTL_EPENA) && hs_ep->isochronous) {
				epctrl |= DXEPCTL_SNAK;
				epctrl |= DXEPCTL_EPDIS;
				dwc2_writel(epctrl, hsotg->regs + DOEPCTL(idx));
			}
		}

		/* This interrupt bit is cleared in DXEPINT_EPDISBLD handler */
>>>>>>> v4.9.227
	}

	if (gintsts & GINTSTS_GINNAKEFF) {
		dev_info(hsotg->dev, "GINNakEff triggered\n");

<<<<<<< HEAD
		writel(DCTL_CGNPINNAK, hsotg->regs + DCTL);

		s3c_hsotg_dump(hsotg);
	}

=======
		__orr32(hsotg->regs + DCTL, DCTL_CGNPINNAK);

		dwc2_hsotg_dump(hsotg);
	}

	if (gintsts & GINTSTS_INCOMPL_SOIN)
		dwc2_gadget_handle_incomplete_isoc_in(hsotg);

	if (gintsts & GINTSTS_INCOMPL_SOOUT)
		dwc2_gadget_handle_incomplete_isoc_out(hsotg);

>>>>>>> v4.9.227
	/*
	 * if we've had fifo events, we should try and go around the
	 * loop again to see if there's any point in returning yet.
	 */

	if (gintsts & IRQ_RETRY_MASK && --retry_count > 0)
			goto irq_retry;

	spin_unlock(&hsotg->lock);

	return IRQ_HANDLED;
}

/**
<<<<<<< HEAD
 * s3c_hsotg_ep_enable - enable the given endpoint
=======
 * dwc2_hsotg_ep_enable - enable the given endpoint
>>>>>>> v4.9.227
 * @ep: The USB endpint to configure
 * @desc: The USB endpoint descriptor to configure with.
 *
 * This is called from the USB gadget code's usb_ep_enable().
 */
<<<<<<< HEAD
static int s3c_hsotg_ep_enable(struct usb_ep *ep,
			       const struct usb_endpoint_descriptor *desc)
{
	struct s3c_hsotg_ep *hs_ep = our_ep(ep);
	struct s3c_hsotg *hsotg = hs_ep->parent;
	unsigned long flags;
	int index = hs_ep->index;
	u32 epctrl_reg;
	u32 epctrl;
	u32 mps;
	int dir_in;
	int i, val, size;
=======
static int dwc2_hsotg_ep_enable(struct usb_ep *ep,
			       const struct usb_endpoint_descriptor *desc)
{
	struct dwc2_hsotg_ep *hs_ep = our_ep(ep);
	struct dwc2_hsotg *hsotg = hs_ep->parent;
	unsigned long flags;
	unsigned int index = hs_ep->index;
	u32 epctrl_reg;
	u32 epctrl;
	u32 mps;
	u32 mask;
	unsigned int dir_in;
	unsigned int i, val, size;
>>>>>>> v4.9.227
	int ret = 0;

	dev_dbg(hsotg->dev,
		"%s: ep %s: a 0x%02x, attr 0x%02x, mps 0x%04x, intr %d\n",
		__func__, ep->name, desc->bEndpointAddress, desc->bmAttributes,
		desc->wMaxPacketSize, desc->bInterval);

	/* not to be called for EP0 */
<<<<<<< HEAD
	WARN_ON(index == 0);
=======
	if (index == 0) {
		dev_err(hsotg->dev, "%s: called for EP 0\n", __func__);
		return -EINVAL;
	}
>>>>>>> v4.9.227

	dir_in = (desc->bEndpointAddress & USB_ENDPOINT_DIR_MASK) ? 1 : 0;
	if (dir_in != hs_ep->dir_in) {
		dev_err(hsotg->dev, "%s: direction mismatch!\n", __func__);
		return -EINVAL;
	}

	mps = usb_endpoint_maxp(desc);

<<<<<<< HEAD
	/* note, we handle this here instead of s3c_hsotg_set_ep_maxpacket */

	epctrl_reg = dir_in ? DIEPCTL(index) : DOEPCTL(index);
	epctrl = readl(hsotg->regs + epctrl_reg);
=======
	/* note, we handle this here instead of dwc2_hsotg_set_ep_maxpacket */

	epctrl_reg = dir_in ? DIEPCTL(index) : DOEPCTL(index);
	epctrl = dwc2_readl(hsotg->regs + epctrl_reg);
>>>>>>> v4.9.227

	dev_dbg(hsotg->dev, "%s: read DxEPCTL=0x%08x from 0x%08x\n",
		__func__, epctrl, epctrl_reg);

	spin_lock_irqsave(&hsotg->lock, flags);

	epctrl &= ~(DXEPCTL_EPTYPE_MASK | DXEPCTL_MPS_MASK);
	epctrl |= DXEPCTL_MPS(mps);

	/*
	 * mark the endpoint as active, otherwise the core may ignore
	 * transactions entirely for this endpoint
	 */
	epctrl |= DXEPCTL_USBACTEP;

<<<<<<< HEAD
	/*
	 * set the NAK status on the endpoint, otherwise we might try and
	 * do something with data that we've yet got a request to process
	 * since the RXFIFO will take data for an endpoint even if the
	 * size register hasn't been set.
	 */

	epctrl |= DXEPCTL_SNAK;

	/* update the endpoint state */
	s3c_hsotg_set_ep_maxpacket(hsotg, hs_ep->index, mps);
=======
	/* update the endpoint state */
	dwc2_hsotg_set_ep_maxpacket(hsotg, hs_ep->index, mps, dir_in);
>>>>>>> v4.9.227

	/* default, set to non-periodic */
	hs_ep->isochronous = 0;
	hs_ep->periodic = 0;
	hs_ep->halted = 0;
	hs_ep->interval = desc->bInterval;

<<<<<<< HEAD
	if (hs_ep->interval > 1 && hs_ep->mc > 1)
		dev_err(hsotg->dev, "MC > 1 when interval is not 1\n");

=======
>>>>>>> v4.9.227
	switch (desc->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) {
	case USB_ENDPOINT_XFER_ISOC:
		epctrl |= DXEPCTL_EPTYPE_ISO;
		epctrl |= DXEPCTL_SETEVENFR;
		hs_ep->isochronous = 1;
<<<<<<< HEAD
		if (dir_in)
			hs_ep->periodic = 1;
=======
		hs_ep->interval = 1 << (desc->bInterval - 1);
		hs_ep->target_frame = TARGET_FRAME_INITIAL;
		if (dir_in) {
			hs_ep->periodic = 1;
			mask = dwc2_readl(hsotg->regs + DIEPMSK);
			mask |= DIEPMSK_NAKMSK;
			dwc2_writel(mask, hsotg->regs + DIEPMSK);
		} else {
			mask = dwc2_readl(hsotg->regs + DOEPMSK);
			mask |= DOEPMSK_OUTTKNEPDISMSK;
			dwc2_writel(mask, hsotg->regs + DOEPMSK);
		}
>>>>>>> v4.9.227
		break;

	case USB_ENDPOINT_XFER_BULK:
		epctrl |= DXEPCTL_EPTYPE_BULK;
		break;

	case USB_ENDPOINT_XFER_INT:
		if (dir_in)
			hs_ep->periodic = 1;

<<<<<<< HEAD
=======
		if (hsotg->gadget.speed == USB_SPEED_HIGH)
			hs_ep->interval = 1 << (desc->bInterval - 1);

>>>>>>> v4.9.227
		epctrl |= DXEPCTL_EPTYPE_INTERRUPT;
		break;

	case USB_ENDPOINT_XFER_CONTROL:
		epctrl |= DXEPCTL_EPTYPE_CONTROL;
		break;
	}

	/*
	 * if the hardware has dedicated fifos, we must give each IN EP
	 * a unique tx-fifo even if it is non-periodic.
	 */
	if (dir_in && hsotg->dedicated_fifos) {
<<<<<<< HEAD
		size = hs_ep->ep.maxpacket*hs_ep->mc;
		for (i = 1; i <= 8; ++i) {
			if (hsotg->fifo_map & (1<<i))
				continue;
			val = readl(hsotg->regs + DPTXFSIZN(i));
			val = (val >> FIFOSIZE_DEPTH_SHIFT)*4;
			if (val < size)
				continue;
			hsotg->fifo_map |= 1<<i;

			epctrl |= DXEPCTL_TXFNUM(i);
			hs_ep->fifo_index = i;
			hs_ep->fifo_size = val;
			break;
		}
		if (i == 8) {
			ret = -ENOMEM;
			goto error;
		}
	}

	/* for non control endpoints, set PID to D0 */
	if (index)
=======
		u32 fifo_index = 0;
		u32 fifo_size = UINT_MAX;
		size = hs_ep->ep.maxpacket*hs_ep->mc;
		for (i = 1; i < hsotg->num_of_eps; ++i) {
			if (hsotg->fifo_map & (1<<i))
				continue;
			val = dwc2_readl(hsotg->regs + DPTXFSIZN(i));
			val = (val >> FIFOSIZE_DEPTH_SHIFT)*4;
			if (val < size)
				continue;
			/* Search for smallest acceptable fifo */
			if (val < fifo_size) {
				fifo_size = val;
				fifo_index = i;
			}
		}
		if (!fifo_index) {
			dev_err(hsotg->dev,
				"%s: No suitable fifo found\n", __func__);
			ret = -ENOMEM;
			goto error;
		}
		hsotg->fifo_map |= 1 << fifo_index;
		epctrl |= DXEPCTL_TXFNUM(fifo_index);
		hs_ep->fifo_index = fifo_index;
		hs_ep->fifo_size = fifo_size;
	}

	/* for non control endpoints, set PID to D0 */
	if (index && !hs_ep->isochronous)
>>>>>>> v4.9.227
		epctrl |= DXEPCTL_SETD0PID;

	dev_dbg(hsotg->dev, "%s: write DxEPCTL=0x%08x\n",
		__func__, epctrl);

<<<<<<< HEAD
	writel(epctrl, hsotg->regs + epctrl_reg);
	dev_dbg(hsotg->dev, "%s: read DxEPCTL=0x%08x\n",
		__func__, readl(hsotg->regs + epctrl_reg));

	/* enable the endpoint interrupt */
	s3c_hsotg_ctrl_epint(hsotg, index, dir_in, 1);
=======
	dwc2_writel(epctrl, hsotg->regs + epctrl_reg);
	dev_dbg(hsotg->dev, "%s: read DxEPCTL=0x%08x\n",
		__func__, dwc2_readl(hsotg->regs + epctrl_reg));

	/* enable the endpoint interrupt */
	dwc2_hsotg_ctrl_epint(hsotg, index, dir_in, 1);
>>>>>>> v4.9.227

error:
	spin_unlock_irqrestore(&hsotg->lock, flags);
	return ret;
}

/**
<<<<<<< HEAD
 * s3c_hsotg_ep_disable - disable given endpoint
 * @ep: The endpoint to disable.
 */
static int s3c_hsotg_ep_disable(struct usb_ep *ep)
{
	struct s3c_hsotg_ep *hs_ep = our_ep(ep);
	struct s3c_hsotg *hsotg = hs_ep->parent;
=======
 * dwc2_hsotg_ep_disable - disable given endpoint
 * @ep: The endpoint to disable.
 */
static int dwc2_hsotg_ep_disable(struct usb_ep *ep)
{
	struct dwc2_hsotg_ep *hs_ep = our_ep(ep);
	struct dwc2_hsotg *hsotg = hs_ep->parent;
>>>>>>> v4.9.227
	int dir_in = hs_ep->dir_in;
	int index = hs_ep->index;
	unsigned long flags;
	u32 epctrl_reg;
	u32 ctrl;

	dev_dbg(hsotg->dev, "%s(ep %p)\n", __func__, ep);

<<<<<<< HEAD
	if (ep == &hsotg->eps[0].ep) {
=======
	if (ep == &hsotg->eps_out[0]->ep) {
>>>>>>> v4.9.227
		dev_err(hsotg->dev, "%s: called for ep0\n", __func__);
		return -EINVAL;
	}

	epctrl_reg = dir_in ? DIEPCTL(index) : DOEPCTL(index);

	spin_lock_irqsave(&hsotg->lock, flags);
<<<<<<< HEAD
	/* terminate all requests with shutdown */
	kill_all_requests(hsotg, hs_ep, -ESHUTDOWN, false);

	hsotg->fifo_map &= ~(1<<hs_ep->fifo_index);
	hs_ep->fifo_index = 0;
	hs_ep->fifo_size = 0;

	ctrl = readl(hsotg->regs + epctrl_reg);
=======

	ctrl = dwc2_readl(hsotg->regs + epctrl_reg);
>>>>>>> v4.9.227
	ctrl &= ~DXEPCTL_EPENA;
	ctrl &= ~DXEPCTL_USBACTEP;
	ctrl |= DXEPCTL_SNAK;

	dev_dbg(hsotg->dev, "%s: DxEPCTL=0x%08x\n", __func__, ctrl);
<<<<<<< HEAD
	writel(ctrl, hsotg->regs + epctrl_reg);

	/* disable endpoint interrupts */
	s3c_hsotg_ctrl_epint(hsotg, hs_ep->index, hs_ep->dir_in, 0);
=======
	dwc2_writel(ctrl, hsotg->regs + epctrl_reg);

	/* disable endpoint interrupts */
	dwc2_hsotg_ctrl_epint(hsotg, hs_ep->index, hs_ep->dir_in, 0);

	/* terminate all requests with shutdown */
	kill_all_requests(hsotg, hs_ep, -ESHUTDOWN);

	hsotg->fifo_map &= ~(1 << hs_ep->fifo_index);
	hs_ep->fifo_index = 0;
	hs_ep->fifo_size = 0;
>>>>>>> v4.9.227

	spin_unlock_irqrestore(&hsotg->lock, flags);
	return 0;
}

/**
 * on_list - check request is on the given endpoint
 * @ep: The endpoint to check.
 * @test: The request to test if it is on the endpoint.
 */
<<<<<<< HEAD
static bool on_list(struct s3c_hsotg_ep *ep, struct s3c_hsotg_req *test)
{
	struct s3c_hsotg_req *req, *treq;
=======
static bool on_list(struct dwc2_hsotg_ep *ep, struct dwc2_hsotg_req *test)
{
	struct dwc2_hsotg_req *req, *treq;
>>>>>>> v4.9.227

	list_for_each_entry_safe(req, treq, &ep->queue, queue) {
		if (req == test)
			return true;
	}

	return false;
}

<<<<<<< HEAD
/**
 * s3c_hsotg_ep_dequeue - dequeue given endpoint
 * @ep: The endpoint to dequeue.
 * @req: The request to be removed from a queue.
 */
static int s3c_hsotg_ep_dequeue(struct usb_ep *ep, struct usb_request *req)
{
	struct s3c_hsotg_req *hs_req = our_req(req);
	struct s3c_hsotg_ep *hs_ep = our_ep(ep);
	struct s3c_hsotg *hs = hs_ep->parent;
=======
static int dwc2_hsotg_wait_bit_set(struct dwc2_hsotg *hs_otg, u32 reg,
							u32 bit, u32 timeout)
{
	u32 i;

	for (i = 0; i < timeout; i++) {
		if (dwc2_readl(hs_otg->regs + reg) & bit)
			return 0;
		udelay(1);
	}

	return -ETIMEDOUT;
}

static void dwc2_hsotg_ep_stop_xfr(struct dwc2_hsotg *hsotg,
						struct dwc2_hsotg_ep *hs_ep)
{
	u32 epctrl_reg;
	u32 epint_reg;

	epctrl_reg = hs_ep->dir_in ? DIEPCTL(hs_ep->index) :
		DOEPCTL(hs_ep->index);
	epint_reg = hs_ep->dir_in ? DIEPINT(hs_ep->index) :
		DOEPINT(hs_ep->index);

	dev_dbg(hsotg->dev, "%s: stopping transfer on %s\n", __func__,
			hs_ep->name);
	if (hs_ep->dir_in) {
		__orr32(hsotg->regs + epctrl_reg, DXEPCTL_SNAK);
		/* Wait for Nak effect */
		if (dwc2_hsotg_wait_bit_set(hsotg, epint_reg,
						DXEPINT_INEPNAKEFF, 100))
			dev_warn(hsotg->dev,
				"%s: timeout DIEPINT.NAKEFF\n", __func__);
	} else {
		if (!(dwc2_readl(hsotg->regs + GINTSTS) & GINTSTS_GOUTNAKEFF))
			__orr32(hsotg->regs + DCTL, DCTL_SGOUTNAK);

		/* Wait for global nak to take effect */
		if (dwc2_hsotg_wait_bit_set(hsotg, GINTSTS,
						GINTSTS_GOUTNAKEFF, 100))
			dev_warn(hsotg->dev,
				"%s: timeout GINTSTS.GOUTNAKEFF\n", __func__);
	}

	/* Disable ep */
	__orr32(hsotg->regs + epctrl_reg, DXEPCTL_EPDIS | DXEPCTL_SNAK);

	/* Wait for ep to be disabled */
	if (dwc2_hsotg_wait_bit_set(hsotg, epint_reg, DXEPINT_EPDISBLD, 100))
		dev_warn(hsotg->dev,
			"%s: timeout DOEPCTL.EPDisable\n", __func__);

	if (hs_ep->dir_in) {
		if (hsotg->dedicated_fifos) {
			dwc2_writel(GRSTCTL_TXFNUM(hs_ep->fifo_index) |
				GRSTCTL_TXFFLSH, hsotg->regs + GRSTCTL);
			/* Wait for fifo flush */
			if (dwc2_hsotg_wait_bit_set(hsotg, GRSTCTL,
							GRSTCTL_TXFFLSH, 100))
				dev_warn(hsotg->dev,
					"%s: timeout flushing fifos\n",
					__func__);
		}
		/* TODO: Flush shared tx fifo */
	} else {
		/* Remove global NAKs */
		__bic32(hsotg->regs + DCTL, DCTL_SGOUTNAK);
	}
}

/**
 * dwc2_hsotg_ep_dequeue - dequeue given endpoint
 * @ep: The endpoint to dequeue.
 * @req: The request to be removed from a queue.
 */
static int dwc2_hsotg_ep_dequeue(struct usb_ep *ep, struct usb_request *req)
{
	struct dwc2_hsotg_req *hs_req = our_req(req);
	struct dwc2_hsotg_ep *hs_ep = our_ep(ep);
	struct dwc2_hsotg *hs = hs_ep->parent;
>>>>>>> v4.9.227
	unsigned long flags;

	dev_dbg(hs->dev, "ep_dequeue(%p,%p)\n", ep, req);

	spin_lock_irqsave(&hs->lock, flags);

	if (!on_list(hs_ep, hs_req)) {
		spin_unlock_irqrestore(&hs->lock, flags);
		return -EINVAL;
	}

<<<<<<< HEAD
	s3c_hsotg_complete_request(hs, hs_ep, hs_req, -ECONNRESET);
=======
	/* Dequeue already started request */
	if (req == &hs_ep->req->req)
		dwc2_hsotg_ep_stop_xfr(hs, hs_ep);

	dwc2_hsotg_complete_request(hs, hs_ep, hs_req, -ECONNRESET);
>>>>>>> v4.9.227
	spin_unlock_irqrestore(&hs->lock, flags);

	return 0;
}

/**
<<<<<<< HEAD
 * s3c_hsotg_ep_sethalt - set halt on a given endpoint
 * @ep: The endpoint to set halt.
 * @value: Set or unset the halt.
 */
static int s3c_hsotg_ep_sethalt(struct usb_ep *ep, int value)
{
	struct s3c_hsotg_ep *hs_ep = our_ep(ep);
	struct s3c_hsotg *hs = hs_ep->parent;
=======
 * dwc2_hsotg_ep_sethalt - set halt on a given endpoint
 * @ep: The endpoint to set halt.
 * @value: Set or unset the halt.
 * @now: If true, stall the endpoint now. Otherwise return -EAGAIN if
 *       the endpoint is busy processing requests.
 *
 * We need to stall the endpoint immediately if request comes from set_feature
 * protocol command handler.
 */
static int dwc2_hsotg_ep_sethalt(struct usb_ep *ep, int value, bool now)
{
	struct dwc2_hsotg_ep *hs_ep = our_ep(ep);
	struct dwc2_hsotg *hs = hs_ep->parent;
>>>>>>> v4.9.227
	int index = hs_ep->index;
	u32 epreg;
	u32 epctl;
	u32 xfertype;

	dev_info(hs->dev, "%s(ep %p %s, %d)\n", __func__, ep, ep->name, value);

	if (index == 0) {
		if (value)
<<<<<<< HEAD
			s3c_hsotg_stall_ep0(hs);
=======
			dwc2_hsotg_stall_ep0(hs);
>>>>>>> v4.9.227
		else
			dev_warn(hs->dev,
				 "%s: can't clear halt on ep0\n", __func__);
		return 0;
	}

<<<<<<< HEAD
	/* write both IN and OUT control registers */

	epreg = DIEPCTL(index);
	epctl = readl(hs->regs + epreg);

	if (value) {
		epctl |= DXEPCTL_STALL + DXEPCTL_SNAK;
		if (epctl & DXEPCTL_EPENA)
			epctl |= DXEPCTL_EPDIS;
	} else {
		epctl &= ~DXEPCTL_STALL;
		xfertype = epctl & DXEPCTL_EPTYPE_MASK;
		if (xfertype == DXEPCTL_EPTYPE_BULK ||
			xfertype == DXEPCTL_EPTYPE_INTERRUPT)
				epctl |= DXEPCTL_SETD0PID;
	}

	writel(epctl, hs->regs + epreg);

	epreg = DOEPCTL(index);
	epctl = readl(hs->regs + epreg);

	if (value)
		epctl |= DXEPCTL_STALL;
	else {
		epctl &= ~DXEPCTL_STALL;
		xfertype = epctl & DXEPCTL_EPTYPE_MASK;
		if (xfertype == DXEPCTL_EPTYPE_BULK ||
			xfertype == DXEPCTL_EPTYPE_INTERRUPT)
				epctl |= DXEPCTL_SETD0PID;
	}

	writel(epctl, hs->regs + epreg);

=======
	if (hs_ep->isochronous) {
		dev_err(hs->dev, "%s is Isochronous Endpoint\n", ep->name);
		return -EINVAL;
	}

	if (!now && value && !list_empty(&hs_ep->queue)) {
		dev_dbg(hs->dev, "%s request is pending, cannot halt\n",
			ep->name);
		return -EAGAIN;
	}

	if (hs_ep->dir_in) {
		epreg = DIEPCTL(index);
		epctl = dwc2_readl(hs->regs + epreg);

		if (value) {
			epctl |= DXEPCTL_STALL | DXEPCTL_SNAK;
			if (epctl & DXEPCTL_EPENA)
				epctl |= DXEPCTL_EPDIS;
		} else {
			epctl &= ~DXEPCTL_STALL;
			xfertype = epctl & DXEPCTL_EPTYPE_MASK;
			if (xfertype == DXEPCTL_EPTYPE_BULK ||
				xfertype == DXEPCTL_EPTYPE_INTERRUPT)
					epctl |= DXEPCTL_SETD0PID;
		}
		dwc2_writel(epctl, hs->regs + epreg);
	} else {

		epreg = DOEPCTL(index);
		epctl = dwc2_readl(hs->regs + epreg);

		if (value)
			epctl |= DXEPCTL_STALL;
		else {
			epctl &= ~DXEPCTL_STALL;
			xfertype = epctl & DXEPCTL_EPTYPE_MASK;
			if (xfertype == DXEPCTL_EPTYPE_BULK ||
				xfertype == DXEPCTL_EPTYPE_INTERRUPT)
					epctl |= DXEPCTL_SETD0PID;
		}
		dwc2_writel(epctl, hs->regs + epreg);
	}

>>>>>>> v4.9.227
	hs_ep->halted = value;

	return 0;
}

/**
<<<<<<< HEAD
 * s3c_hsotg_ep_sethalt_lock - set halt on a given endpoint with lock held
 * @ep: The endpoint to set halt.
 * @value: Set or unset the halt.
 */
static int s3c_hsotg_ep_sethalt_lock(struct usb_ep *ep, int value)
{
	struct s3c_hsotg_ep *hs_ep = our_ep(ep);
	struct s3c_hsotg *hs = hs_ep->parent;
=======
 * dwc2_hsotg_ep_sethalt_lock - set halt on a given endpoint with lock held
 * @ep: The endpoint to set halt.
 * @value: Set or unset the halt.
 */
static int dwc2_hsotg_ep_sethalt_lock(struct usb_ep *ep, int value)
{
	struct dwc2_hsotg_ep *hs_ep = our_ep(ep);
	struct dwc2_hsotg *hs = hs_ep->parent;
>>>>>>> v4.9.227
	unsigned long flags = 0;
	int ret = 0;

	spin_lock_irqsave(&hs->lock, flags);
<<<<<<< HEAD
	ret = s3c_hsotg_ep_sethalt(ep, value);
=======
	ret = dwc2_hsotg_ep_sethalt(ep, value, false);
>>>>>>> v4.9.227
	spin_unlock_irqrestore(&hs->lock, flags);

	return ret;
}

<<<<<<< HEAD
static struct usb_ep_ops s3c_hsotg_ep_ops = {
	.enable		= s3c_hsotg_ep_enable,
	.disable	= s3c_hsotg_ep_disable,
	.alloc_request	= s3c_hsotg_ep_alloc_request,
	.free_request	= s3c_hsotg_ep_free_request,
	.queue		= s3c_hsotg_ep_queue_lock,
	.dequeue	= s3c_hsotg_ep_dequeue,
	.set_halt	= s3c_hsotg_ep_sethalt_lock,
=======
static struct usb_ep_ops dwc2_hsotg_ep_ops = {
	.enable		= dwc2_hsotg_ep_enable,
	.disable	= dwc2_hsotg_ep_disable,
	.alloc_request	= dwc2_hsotg_ep_alloc_request,
	.free_request	= dwc2_hsotg_ep_free_request,
	.queue		= dwc2_hsotg_ep_queue_lock,
	.dequeue	= dwc2_hsotg_ep_dequeue,
	.set_halt	= dwc2_hsotg_ep_sethalt_lock,
>>>>>>> v4.9.227
	/* note, don't believe we have any call for the fifo routines */
};

/**
<<<<<<< HEAD
 * s3c_hsotg_phy_enable - enable platform phy dev
 * @hsotg: The driver state
 *
 * A wrapper for platform code responsible for controlling
 * low-level USB code
 */
static void s3c_hsotg_phy_enable(struct s3c_hsotg *hsotg)
{
	struct platform_device *pdev = to_platform_device(hsotg->dev);

	dev_dbg(hsotg->dev, "pdev 0x%p\n", pdev);

	if (hsotg->uphy)
		usb_phy_init(hsotg->uphy);
	else if (hsotg->plat && hsotg->plat->phy_init)
		hsotg->plat->phy_init(pdev, hsotg->plat->phy_type);
	else {
		phy_init(hsotg->phy);
		phy_power_on(hsotg->phy);
	}
}

/**
 * s3c_hsotg_phy_disable - disable platform phy dev
 * @hsotg: The driver state
 *
 * A wrapper for platform code responsible for controlling
 * low-level USB code
 */
static void s3c_hsotg_phy_disable(struct s3c_hsotg *hsotg)
{
	struct platform_device *pdev = to_platform_device(hsotg->dev);

	if (hsotg->uphy)
		usb_phy_shutdown(hsotg->uphy);
	else if (hsotg->plat && hsotg->plat->phy_exit)
		hsotg->plat->phy_exit(pdev, hsotg->plat->phy_type);
	else {
		phy_power_off(hsotg->phy);
		phy_exit(hsotg->phy);
	}
}

/**
 * s3c_hsotg_init - initalize the usb core
 * @hsotg: The driver state
 */
static void s3c_hsotg_init(struct s3c_hsotg *hsotg)
{
	/* unmask subset of endpoint interrupts */

	writel(DIEPMSK_TIMEOUTMSK | DIEPMSK_AHBERRMSK |
		DIEPMSK_EPDISBLDMSK | DIEPMSK_XFERCOMPLMSK,
		hsotg->regs + DIEPMSK);

	writel(DOEPMSK_SETUPMSK | DOEPMSK_AHBERRMSK |
		DOEPMSK_EPDISBLDMSK | DOEPMSK_XFERCOMPLMSK,
		hsotg->regs + DOEPMSK);

	writel(0, hsotg->regs + DAINTMSK);
=======
 * dwc2_hsotg_init - initalize the usb core
 * @hsotg: The driver state
 */
static void dwc2_hsotg_init(struct dwc2_hsotg *hsotg)
{
	u32 trdtim;
	u32 usbcfg;
	/* unmask subset of endpoint interrupts */

	dwc2_writel(DIEPMSK_TIMEOUTMSK | DIEPMSK_AHBERRMSK |
		    DIEPMSK_EPDISBLDMSK | DIEPMSK_XFERCOMPLMSK,
		    hsotg->regs + DIEPMSK);

	dwc2_writel(DOEPMSK_SETUPMSK | DOEPMSK_AHBERRMSK |
		    DOEPMSK_EPDISBLDMSK | DOEPMSK_XFERCOMPLMSK,
		    hsotg->regs + DOEPMSK);

	dwc2_writel(0, hsotg->regs + DAINTMSK);
>>>>>>> v4.9.227

	/* Be in disconnected state until gadget is registered */
	__orr32(hsotg->regs + DCTL, DCTL_SFTDISCON);

<<<<<<< HEAD
	if (0) {
		/* post global nak until we're ready */
		writel(DCTL_SGNPINNAK | DCTL_SGOUTNAK,
		       hsotg->regs + DCTL);
	}

	/* setup fifos */

	dev_dbg(hsotg->dev, "GRXFSIZ=0x%08x, GNPTXFSIZ=0x%08x\n",
		readl(hsotg->regs + GRXFSIZ),
		readl(hsotg->regs + GNPTXFSIZ));

	s3c_hsotg_init_fifo(hsotg);

	/* set the PLL on, remove the HNP/SRP and set the PHY */
	writel(GUSBCFG_PHYIF16 | GUSBCFG_TOUTCAL(7) | (0x5 << 10),
	       hsotg->regs + GUSBCFG);

	writel(using_dma(hsotg) ? GAHBCFG_DMA_EN : 0x0,
	       hsotg->regs + GAHBCFG);
}

/**
 * s3c_hsotg_udc_start - prepare the udc for work
=======
	/* setup fifos */

	dev_dbg(hsotg->dev, "GRXFSIZ=0x%08x, GNPTXFSIZ=0x%08x\n",
		dwc2_readl(hsotg->regs + GRXFSIZ),
		dwc2_readl(hsotg->regs + GNPTXFSIZ));

	dwc2_hsotg_init_fifo(hsotg);

	/* keep other bits untouched (so e.g. forced modes are not lost) */
	usbcfg = dwc2_readl(hsotg->regs + GUSBCFG);
	usbcfg &= ~(GUSBCFG_TOUTCAL_MASK | GUSBCFG_PHYIF16 | GUSBCFG_SRPCAP |
		GUSBCFG_HNPCAP | GUSBCFG_USBTRDTIM_MASK);

	/* set the PLL on, remove the HNP/SRP and set the PHY */
	trdtim = (hsotg->phyif == GUSBCFG_PHYIF8) ? 9 : 5;
	usbcfg |= hsotg->phyif | GUSBCFG_TOUTCAL(7) |
		(trdtim << GUSBCFG_USBTRDTIM_SHIFT);
	dwc2_writel(usbcfg, hsotg->regs + GUSBCFG);

	if (using_dma(hsotg))
		__orr32(hsotg->regs + GAHBCFG, GAHBCFG_DMA_EN);
}

/**
 * dwc2_hsotg_udc_start - prepare the udc for work
>>>>>>> v4.9.227
 * @gadget: The usb gadget state
 * @driver: The usb gadget driver
 *
 * Perform initialization to prepare udc device and driver
 * to work.
 */
<<<<<<< HEAD
static int s3c_hsotg_udc_start(struct usb_gadget *gadget,
			   struct usb_gadget_driver *driver)
{
	struct s3c_hsotg *hsotg = to_hsotg(gadget);
=======
static int dwc2_hsotg_udc_start(struct usb_gadget *gadget,
			   struct usb_gadget_driver *driver)
{
	struct dwc2_hsotg *hsotg = to_hsotg(gadget);
	unsigned long flags;
>>>>>>> v4.9.227
	int ret;

	if (!hsotg) {
		pr_err("%s: called with no device\n", __func__);
		return -ENODEV;
	}

	if (!driver) {
		dev_err(hsotg->dev, "%s: no driver\n", __func__);
		return -EINVAL;
	}

	if (driver->max_speed < USB_SPEED_FULL)
		dev_err(hsotg->dev, "%s: bad speed\n", __func__);

	if (!driver->setup) {
		dev_err(hsotg->dev, "%s: missing entry points\n", __func__);
		return -EINVAL;
	}

	WARN_ON(hsotg->driver);

	driver->driver.bus = NULL;
	hsotg->driver = driver;
	hsotg->gadget.dev.of_node = hsotg->dev->of_node;
	hsotg->gadget.speed = USB_SPEED_UNKNOWN;

<<<<<<< HEAD
	clk_enable(hsotg->clk);

	ret = regulator_bulk_enable(ARRAY_SIZE(hsotg->supplies),
				    hsotg->supplies);
	if (ret) {
		dev_err(hsotg->dev, "failed to enable supplies: %d\n", ret);
		goto err;
	}

	hsotg->last_rst = jiffies;
	dev_info(hsotg->dev, "bound driver %s\n", driver->driver.name);
=======
	if (hsotg->dr_mode == USB_DR_MODE_PERIPHERAL) {
		ret = dwc2_lowlevel_hw_enable(hsotg);
		if (ret)
			goto err;
	}

	if (!IS_ERR_OR_NULL(hsotg->uphy))
		otg_set_peripheral(hsotg->uphy->otg, &hsotg->gadget);

	spin_lock_irqsave(&hsotg->lock, flags);
	if (dwc2_hw_is_device(hsotg)) {
		dwc2_hsotg_init(hsotg);
		dwc2_hsotg_core_init_disconnected(hsotg, false);
	}

	hsotg->enabled = 0;
	spin_unlock_irqrestore(&hsotg->lock, flags);

	dev_info(hsotg->dev, "bound driver %s\n", driver->driver.name);

>>>>>>> v4.9.227
	return 0;

err:
	hsotg->driver = NULL;
	return ret;
}

/**
<<<<<<< HEAD
 * s3c_hsotg_udc_stop - stop the udc
=======
 * dwc2_hsotg_udc_stop - stop the udc
>>>>>>> v4.9.227
 * @gadget: The usb gadget state
 * @driver: The usb gadget driver
 *
 * Stop udc hw block and stay tunned for future transmissions
 */
<<<<<<< HEAD
static int s3c_hsotg_udc_stop(struct usb_gadget *gadget,
			  struct usb_gadget_driver *driver)
{
	struct s3c_hsotg *hsotg = to_hsotg(gadget);
=======
static int dwc2_hsotg_udc_stop(struct usb_gadget *gadget)
{
	struct dwc2_hsotg *hsotg = to_hsotg(gadget);
>>>>>>> v4.9.227
	unsigned long flags = 0;
	int ep;

	if (!hsotg)
		return -ENODEV;

	/* all endpoints should be shutdown */
<<<<<<< HEAD
	for (ep = 1; ep < hsotg->num_of_eps; ep++)
		s3c_hsotg_ep_disable(&hsotg->eps[ep].ep);
=======
	for (ep = 1; ep < hsotg->num_of_eps; ep++) {
		if (hsotg->eps_in[ep])
			dwc2_hsotg_ep_disable(&hsotg->eps_in[ep]->ep);
		if (hsotg->eps_out[ep])
			dwc2_hsotg_ep_disable(&hsotg->eps_out[ep]->ep);
	}
>>>>>>> v4.9.227

	spin_lock_irqsave(&hsotg->lock, flags);

	hsotg->driver = NULL;
	hsotg->gadget.speed = USB_SPEED_UNKNOWN;
<<<<<<< HEAD

	spin_unlock_irqrestore(&hsotg->lock, flags);

	regulator_bulk_disable(ARRAY_SIZE(hsotg->supplies), hsotg->supplies);

	clk_disable(hsotg->clk);
=======
	hsotg->enabled = 0;

	spin_unlock_irqrestore(&hsotg->lock, flags);

	if (!IS_ERR_OR_NULL(hsotg->uphy))
		otg_set_peripheral(hsotg->uphy->otg, NULL);

	if (hsotg->dr_mode == USB_DR_MODE_PERIPHERAL)
		dwc2_lowlevel_hw_disable(hsotg);
>>>>>>> v4.9.227

	return 0;
}

/**
<<<<<<< HEAD
 * s3c_hsotg_gadget_getframe - read the frame number
=======
 * dwc2_hsotg_gadget_getframe - read the frame number
>>>>>>> v4.9.227
 * @gadget: The usb gadget state
 *
 * Read the {micro} frame number
 */
<<<<<<< HEAD
static int s3c_hsotg_gadget_getframe(struct usb_gadget *gadget)
{
	return s3c_hsotg_read_frameno(to_hsotg(gadget));
}

/**
 * s3c_hsotg_pullup - connect/disconnect the USB PHY
=======
static int dwc2_hsotg_gadget_getframe(struct usb_gadget *gadget)
{
	return dwc2_hsotg_read_frameno(to_hsotg(gadget));
}

/**
 * dwc2_hsotg_pullup - connect/disconnect the USB PHY
>>>>>>> v4.9.227
 * @gadget: The usb gadget state
 * @is_on: Current state of the USB PHY
 *
 * Connect/Disconnect the USB PHY pullup
 */
<<<<<<< HEAD
static int s3c_hsotg_pullup(struct usb_gadget *gadget, int is_on)
{
	struct s3c_hsotg *hsotg = to_hsotg(gadget);
	unsigned long flags = 0;

	dev_dbg(hsotg->dev, "%s: is_on: %d\n", __func__, is_on);

	spin_lock_irqsave(&hsotg->lock, flags);
	if (is_on) {
		s3c_hsotg_phy_enable(hsotg);
		clk_enable(hsotg->clk);
		s3c_hsotg_core_init(hsotg);
	} else {
		clk_disable(hsotg->clk);
		s3c_hsotg_phy_disable(hsotg);
=======
static int dwc2_hsotg_pullup(struct usb_gadget *gadget, int is_on)
{
	struct dwc2_hsotg *hsotg = to_hsotg(gadget);
	unsigned long flags = 0;

	dev_dbg(hsotg->dev, "%s: is_on: %d op_state: %d\n", __func__, is_on,
			hsotg->op_state);

	/* Don't modify pullup state while in host mode */
	if (hsotg->op_state != OTG_STATE_B_PERIPHERAL) {
		hsotg->enabled = is_on;
		return 0;
	}

	spin_lock_irqsave(&hsotg->lock, flags);
	if (is_on) {
		hsotg->enabled = 1;
		dwc2_hsotg_core_init_disconnected(hsotg, false);
		dwc2_hsotg_core_connect(hsotg);
	} else {
		dwc2_hsotg_core_disconnect(hsotg);
		dwc2_hsotg_disconnect(hsotg);
		hsotg->enabled = 0;
>>>>>>> v4.9.227
	}

	hsotg->gadget.speed = USB_SPEED_UNKNOWN;
	spin_unlock_irqrestore(&hsotg->lock, flags);

	return 0;
}

<<<<<<< HEAD
static const struct usb_gadget_ops s3c_hsotg_gadget_ops = {
	.get_frame	= s3c_hsotg_gadget_getframe,
	.udc_start		= s3c_hsotg_udc_start,
	.udc_stop		= s3c_hsotg_udc_stop,
	.pullup                 = s3c_hsotg_pullup,
};

/**
 * s3c_hsotg_initep - initialise a single endpoint
=======
static int dwc2_hsotg_vbus_session(struct usb_gadget *gadget, int is_active)
{
	struct dwc2_hsotg *hsotg = to_hsotg(gadget);
	unsigned long flags;

	dev_dbg(hsotg->dev, "%s: is_active: %d\n", __func__, is_active);
	spin_lock_irqsave(&hsotg->lock, flags);

	/*
	 * If controller is hibernated, it must exit from hibernation
	 * before being initialized / de-initialized
	 */
	if (hsotg->lx_state == DWC2_L2)
		dwc2_exit_hibernation(hsotg, false);

	if (is_active) {
		hsotg->op_state = OTG_STATE_B_PERIPHERAL;

		dwc2_hsotg_core_init_disconnected(hsotg, false);
		if (hsotg->enabled)
			dwc2_hsotg_core_connect(hsotg);
	} else {
		dwc2_hsotg_core_disconnect(hsotg);
		dwc2_hsotg_disconnect(hsotg);
	}

	spin_unlock_irqrestore(&hsotg->lock, flags);
	return 0;
}

/**
 * dwc2_hsotg_vbus_draw - report bMaxPower field
 * @gadget: The usb gadget state
 * @mA: Amount of current
 *
 * Report how much power the device may consume to the phy.
 */
static int dwc2_hsotg_vbus_draw(struct usb_gadget *gadget, unsigned mA)
{
	struct dwc2_hsotg *hsotg = to_hsotg(gadget);

	if (IS_ERR_OR_NULL(hsotg->uphy))
		return -ENOTSUPP;
	return usb_phy_set_power(hsotg->uphy, mA);
}

static const struct usb_gadget_ops dwc2_hsotg_gadget_ops = {
	.get_frame	= dwc2_hsotg_gadget_getframe,
	.udc_start		= dwc2_hsotg_udc_start,
	.udc_stop		= dwc2_hsotg_udc_stop,
	.pullup                 = dwc2_hsotg_pullup,
	.vbus_session		= dwc2_hsotg_vbus_session,
	.vbus_draw		= dwc2_hsotg_vbus_draw,
};

/**
 * dwc2_hsotg_initep - initialise a single endpoint
>>>>>>> v4.9.227
 * @hsotg: The device state.
 * @hs_ep: The endpoint to be initialised.
 * @epnum: The endpoint number
 *
 * Initialise the given endpoint (as part of the probe and device state
 * creation) to give to the gadget driver. Setup the endpoint name, any
 * direction information and other state that may be required.
 */
<<<<<<< HEAD
static void s3c_hsotg_initep(struct s3c_hsotg *hsotg,
				       struct s3c_hsotg_ep *hs_ep,
				       int epnum)
=======
static void dwc2_hsotg_initep(struct dwc2_hsotg *hsotg,
				       struct dwc2_hsotg_ep *hs_ep,
				       int epnum,
				       bool dir_in)
>>>>>>> v4.9.227
{
	char *dir;

	if (epnum == 0)
		dir = "";
<<<<<<< HEAD
	else if ((epnum % 2) == 0) {
		dir = "out";
	} else {
		dir = "in";
		hs_ep->dir_in = 1;
	}

=======
	else if (dir_in)
		dir = "in";
	else
		dir = "out";

	hs_ep->dir_in = dir_in;
>>>>>>> v4.9.227
	hs_ep->index = epnum;

	snprintf(hs_ep->name, sizeof(hs_ep->name), "ep%d%s", epnum, dir);

	INIT_LIST_HEAD(&hs_ep->queue);
	INIT_LIST_HEAD(&hs_ep->ep.ep_list);

	/* add to the list of endpoints known by the gadget driver */
	if (epnum)
		list_add_tail(&hs_ep->ep.ep_list, &hsotg->gadget.ep_list);

	hs_ep->parent = hsotg;
	hs_ep->ep.name = hs_ep->name;
	usb_ep_set_maxpacket_limit(&hs_ep->ep, epnum ? 1024 : EP0_MPS_LIMIT);
<<<<<<< HEAD
	hs_ep->ep.ops = &s3c_hsotg_ep_ops;
=======
	hs_ep->ep.ops = &dwc2_hsotg_ep_ops;

	if (epnum == 0) {
		hs_ep->ep.caps.type_control = true;
	} else {
		hs_ep->ep.caps.type_iso = true;
		hs_ep->ep.caps.type_bulk = true;
		hs_ep->ep.caps.type_int = true;
	}

	if (dir_in)
		hs_ep->ep.caps.dir_in = true;
	else
		hs_ep->ep.caps.dir_out = true;
>>>>>>> v4.9.227

	/*
	 * if we're using dma, we need to set the next-endpoint pointer
	 * to be something valid.
	 */

	if (using_dma(hsotg)) {
		u32 next = DXEPCTL_NEXTEP((epnum + 1) % 15);
<<<<<<< HEAD
		writel(next, hsotg->regs + DIEPCTL(epnum));
		writel(next, hsotg->regs + DOEPCTL(epnum));
=======
		if (dir_in)
			dwc2_writel(next, hsotg->regs + DIEPCTL(epnum));
		else
			dwc2_writel(next, hsotg->regs + DOEPCTL(epnum));
>>>>>>> v4.9.227
	}
}

/**
<<<<<<< HEAD
 * s3c_hsotg_hw_cfg - read HW configuration registers
=======
 * dwc2_hsotg_hw_cfg - read HW configuration registers
>>>>>>> v4.9.227
 * @param: The device state
 *
 * Read the USB core HW configuration registers
 */
<<<<<<< HEAD
static void s3c_hsotg_hw_cfg(struct s3c_hsotg *hsotg)
{
	u32 cfg2, cfg3, cfg4;
	/* check hardware configuration */

	cfg2 = readl(hsotg->regs + 0x48);
	hsotg->num_of_eps = (cfg2 >> 10) & 0xF;

	cfg3 = readl(hsotg->regs + 0x4C);
	hsotg->fifo_mem = (cfg3 >> 16);

	cfg4 = readl(hsotg->regs + 0x50);
	hsotg->dedicated_fifos = (cfg4 >> 25) & 1;
=======
static int dwc2_hsotg_hw_cfg(struct dwc2_hsotg *hsotg)
{
	u32 cfg;
	u32 ep_type;
	u32 i;

	/* check hardware configuration */

	hsotg->num_of_eps = hsotg->hw_params.num_dev_ep;

	/* Add ep0 */
	hsotg->num_of_eps++;

	hsotg->eps_in[0] = devm_kzalloc(hsotg->dev, sizeof(struct dwc2_hsotg_ep),
								GFP_KERNEL);
	if (!hsotg->eps_in[0])
		return -ENOMEM;
	/* Same dwc2_hsotg_ep is used in both directions for ep0 */
	hsotg->eps_out[0] = hsotg->eps_in[0];

	cfg = hsotg->hw_params.dev_ep_dirs;
	for (i = 1, cfg >>= 2; i < hsotg->num_of_eps; i++, cfg >>= 2) {
		ep_type = cfg & 3;
		/* Direction in or both */
		if (!(ep_type & 2)) {
			hsotg->eps_in[i] = devm_kzalloc(hsotg->dev,
				sizeof(struct dwc2_hsotg_ep), GFP_KERNEL);
			if (!hsotg->eps_in[i])
				return -ENOMEM;
		}
		/* Direction out or both */
		if (!(ep_type & 1)) {
			hsotg->eps_out[i] = devm_kzalloc(hsotg->dev,
				sizeof(struct dwc2_hsotg_ep), GFP_KERNEL);
			if (!hsotg->eps_out[i])
				return -ENOMEM;
		}
	}

	hsotg->fifo_mem = hsotg->hw_params.total_fifo_size;
	hsotg->dedicated_fifos = hsotg->hw_params.en_multiple_tx_fifo;
>>>>>>> v4.9.227

	dev_info(hsotg->dev, "EPs: %d, %s fifos, %d entries in SPRAM\n",
		 hsotg->num_of_eps,
		 hsotg->dedicated_fifos ? "dedicated" : "shared",
		 hsotg->fifo_mem);
<<<<<<< HEAD
}

/**
 * s3c_hsotg_dump - dump state of the udc
 * @param: The device state
 */
static void s3c_hsotg_dump(struct s3c_hsotg *hsotg)
=======
	return 0;
}

/**
 * dwc2_hsotg_dump - dump state of the udc
 * @param: The device state
 */
static void dwc2_hsotg_dump(struct dwc2_hsotg *hsotg)
>>>>>>> v4.9.227
{
#ifdef DEBUG
	struct device *dev = hsotg->dev;
	void __iomem *regs = hsotg->regs;
	u32 val;
	int idx;

	dev_info(dev, "DCFG=0x%08x, DCTL=0x%08x, DIEPMSK=%08x\n",
<<<<<<< HEAD
		 readl(regs + DCFG), readl(regs + DCTL),
		 readl(regs + DIEPMSK));

	dev_info(dev, "GAHBCFG=0x%08x, 0x44=0x%08x\n",
		 readl(regs + GAHBCFG), readl(regs + 0x44));

	dev_info(dev, "GRXFSIZ=0x%08x, GNPTXFSIZ=0x%08x\n",
		 readl(regs + GRXFSIZ), readl(regs + GNPTXFSIZ));

	/* show periodic fifo settings */

	for (idx = 1; idx <= 15; idx++) {
		val = readl(regs + DPTXFSIZN(idx));
=======
		 dwc2_readl(regs + DCFG), dwc2_readl(regs + DCTL),
		 dwc2_readl(regs + DIEPMSK));

	dev_info(dev, "GAHBCFG=0x%08x, GHWCFG1=0x%08x\n",
		 dwc2_readl(regs + GAHBCFG), dwc2_readl(regs + GHWCFG1));

	dev_info(dev, "GRXFSIZ=0x%08x, GNPTXFSIZ=0x%08x\n",
		 dwc2_readl(regs + GRXFSIZ), dwc2_readl(regs + GNPTXFSIZ));

	/* show periodic fifo settings */

	for (idx = 1; idx < hsotg->num_of_eps; idx++) {
		val = dwc2_readl(regs + DPTXFSIZN(idx));
>>>>>>> v4.9.227
		dev_info(dev, "DPTx[%d] FSize=%d, StAddr=0x%08x\n", idx,
			 val >> FIFOSIZE_DEPTH_SHIFT,
			 val & FIFOSIZE_STARTADDR_MASK);
	}

<<<<<<< HEAD
	for (idx = 0; idx < 15; idx++) {
		dev_info(dev,
			 "ep%d-in: EPCTL=0x%08x, SIZ=0x%08x, DMA=0x%08x\n", idx,
			 readl(regs + DIEPCTL(idx)),
			 readl(regs + DIEPTSIZ(idx)),
			 readl(regs + DIEPDMA(idx)));

		val = readl(regs + DOEPCTL(idx));
		dev_info(dev,
			 "ep%d-out: EPCTL=0x%08x, SIZ=0x%08x, DMA=0x%08x\n",
			 idx, readl(regs + DOEPCTL(idx)),
			 readl(regs + DOEPTSIZ(idx)),
			 readl(regs + DOEPDMA(idx)));
=======
	for (idx = 0; idx < hsotg->num_of_eps; idx++) {
		dev_info(dev,
			 "ep%d-in: EPCTL=0x%08x, SIZ=0x%08x, DMA=0x%08x\n", idx,
			 dwc2_readl(regs + DIEPCTL(idx)),
			 dwc2_readl(regs + DIEPTSIZ(idx)),
			 dwc2_readl(regs + DIEPDMA(idx)));

		val = dwc2_readl(regs + DOEPCTL(idx));
		dev_info(dev,
			 "ep%d-out: EPCTL=0x%08x, SIZ=0x%08x, DMA=0x%08x\n",
			 idx, dwc2_readl(regs + DOEPCTL(idx)),
			 dwc2_readl(regs + DOEPTSIZ(idx)),
			 dwc2_readl(regs + DOEPDMA(idx)));
>>>>>>> v4.9.227

	}

	dev_info(dev, "DVBUSDIS=0x%08x, DVBUSPULSE=%08x\n",
<<<<<<< HEAD
		 readl(regs + DVBUSDIS), readl(regs + DVBUSPULSE));
#endif
}

/**
 * state_show - debugfs: show overall driver and device state.
 * @seq: The seq file to write to.
 * @v: Unused parameter.
 *
 * This debugfs entry shows the overall state of the hardware and
 * some general information about each of the endpoints available
 * to the system.
 */
static int state_show(struct seq_file *seq, void *v)
{
	struct s3c_hsotg *hsotg = seq->private;
	void __iomem *regs = hsotg->regs;
	int idx;

	seq_printf(seq, "DCFG=0x%08x, DCTL=0x%08x, DSTS=0x%08x\n",
		 readl(regs + DCFG),
		 readl(regs + DCTL),
		 readl(regs + DSTS));

	seq_printf(seq, "DIEPMSK=0x%08x, DOEPMASK=0x%08x\n",
		   readl(regs + DIEPMSK), readl(regs + DOEPMSK));

	seq_printf(seq, "GINTMSK=0x%08x, GINTSTS=0x%08x\n",
		   readl(regs + GINTMSK),
		   readl(regs + GINTSTS));

	seq_printf(seq, "DAINTMSK=0x%08x, DAINT=0x%08x\n",
		   readl(regs + DAINTMSK),
		   readl(regs + DAINT));

	seq_printf(seq, "GNPTXSTS=0x%08x, GRXSTSR=%08x\n",
		   readl(regs + GNPTXSTS),
		   readl(regs + GRXSTSR));

	seq_puts(seq, "\nEndpoint status:\n");

	for (idx = 0; idx < 15; idx++) {
		u32 in, out;

		in = readl(regs + DIEPCTL(idx));
		out = readl(regs + DOEPCTL(idx));

		seq_printf(seq, "ep%d: DIEPCTL=0x%08x, DOEPCTL=0x%08x",
			   idx, in, out);

		in = readl(regs + DIEPTSIZ(idx));
		out = readl(regs + DOEPTSIZ(idx));

		seq_printf(seq, ", DIEPTSIZ=0x%08x, DOEPTSIZ=0x%08x",
			   in, out);

		seq_puts(seq, "\n");
	}

	return 0;
}

static int state_open(struct inode *inode, struct file *file)
{
	return single_open(file, state_show, inode->i_private);
}

static const struct file_operations state_fops = {
	.owner		= THIS_MODULE,
	.open		= state_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

/**
 * fifo_show - debugfs: show the fifo information
 * @seq: The seq_file to write data to.
 * @v: Unused parameter.
 *
 * Show the FIFO information for the overall fifo and all the
 * periodic transmission FIFOs.
 */
static int fifo_show(struct seq_file *seq, void *v)
{
	struct s3c_hsotg *hsotg = seq->private;
	void __iomem *regs = hsotg->regs;
	u32 val;
	int idx;

	seq_puts(seq, "Non-periodic FIFOs:\n");
	seq_printf(seq, "RXFIFO: Size %d\n", readl(regs + GRXFSIZ));

	val = readl(regs + GNPTXFSIZ);
	seq_printf(seq, "NPTXFIFO: Size %d, Start 0x%08x\n",
		   val >> FIFOSIZE_DEPTH_SHIFT,
		   val & FIFOSIZE_DEPTH_MASK);

	seq_puts(seq, "\nPeriodic TXFIFOs:\n");

	for (idx = 1; idx <= 15; idx++) {
		val = readl(regs + DPTXFSIZN(idx));

		seq_printf(seq, "\tDPTXFIFO%2d: Size %d, Start 0x%08x\n", idx,
			   val >> FIFOSIZE_DEPTH_SHIFT,
			   val & FIFOSIZE_STARTADDR_MASK);
	}

	return 0;
}

static int fifo_open(struct inode *inode, struct file *file)
{
	return single_open(file, fifo_show, inode->i_private);
}

static const struct file_operations fifo_fops = {
	.owner		= THIS_MODULE,
	.open		= fifo_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};


static const char *decode_direction(int is_in)
{
	return is_in ? "in" : "out";
}

/**
 * ep_show - debugfs: show the state of an endpoint.
 * @seq: The seq_file to write data to.
 * @v: Unused parameter.
 *
 * This debugfs entry shows the state of the given endpoint (one is
 * registered for each available).
 */
static int ep_show(struct seq_file *seq, void *v)
{
	struct s3c_hsotg_ep *ep = seq->private;
	struct s3c_hsotg *hsotg = ep->parent;
	struct s3c_hsotg_req *req;
	void __iomem *regs = hsotg->regs;
	int index = ep->index;
	int show_limit = 15;
	unsigned long flags;

	seq_printf(seq, "Endpoint index %d, named %s,  dir %s:\n",
		   ep->index, ep->ep.name, decode_direction(ep->dir_in));

	/* first show the register state */

	seq_printf(seq, "\tDIEPCTL=0x%08x, DOEPCTL=0x%08x\n",
		   readl(regs + DIEPCTL(index)),
		   readl(regs + DOEPCTL(index)));

	seq_printf(seq, "\tDIEPDMA=0x%08x, DOEPDMA=0x%08x\n",
		   readl(regs + DIEPDMA(index)),
		   readl(regs + DOEPDMA(index)));

	seq_printf(seq, "\tDIEPINT=0x%08x, DOEPINT=0x%08x\n",
		   readl(regs + DIEPINT(index)),
		   readl(regs + DOEPINT(index)));

	seq_printf(seq, "\tDIEPTSIZ=0x%08x, DOEPTSIZ=0x%08x\n",
		   readl(regs + DIEPTSIZ(index)),
		   readl(regs + DOEPTSIZ(index)));

	seq_puts(seq, "\n");
	seq_printf(seq, "mps %d\n", ep->ep.maxpacket);
	seq_printf(seq, "total_data=%ld\n", ep->total_data);

	seq_printf(seq, "request list (%p,%p):\n",
		   ep->queue.next, ep->queue.prev);

	spin_lock_irqsave(&hsotg->lock, flags);

	list_for_each_entry(req, &ep->queue, queue) {
		if (--show_limit < 0) {
			seq_puts(seq, "not showing more requests...\n");
			break;
		}

		seq_printf(seq, "%c req %p: %d bytes @%p, ",
			   req == ep->req ? '*' : ' ',
			   req, req->req.length, req->req.buf);
		seq_printf(seq, "%d done, res %d\n",
			   req->req.actual, req->req.status);
	}

	spin_unlock_irqrestore(&hsotg->lock, flags);

	return 0;
}

static int ep_open(struct inode *inode, struct file *file)
{
	return single_open(file, ep_show, inode->i_private);
}

static const struct file_operations ep_fops = {
	.owner		= THIS_MODULE,
	.open		= ep_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

/**
 * s3c_hsotg_create_debug - create debugfs directory and files
 * @hsotg: The driver state
 *
 * Create the debugfs files to allow the user to get information
 * about the state of the system. The directory name is created
 * with the same name as the device itself, in case we end up
 * with multiple blocks in future systems.
 */
static void s3c_hsotg_create_debug(struct s3c_hsotg *hsotg)
{
	struct dentry *root;
	unsigned epidx;

	root = debugfs_create_dir(dev_name(hsotg->dev), NULL);
	hsotg->debug_root = root;
	if (IS_ERR(root)) {
		dev_err(hsotg->dev, "cannot create debug root\n");
		return;
	}

	/* create general state file */

	hsotg->debug_file = debugfs_create_file("state", 0444, root,
						hsotg, &state_fops);

	if (IS_ERR(hsotg->debug_file))
		dev_err(hsotg->dev, "%s: failed to create state\n", __func__);

	hsotg->debug_fifo = debugfs_create_file("fifo", 0444, root,
						hsotg, &fifo_fops);

	if (IS_ERR(hsotg->debug_fifo))
		dev_err(hsotg->dev, "%s: failed to create fifo\n", __func__);

	/* create one file for each endpoint */

	for (epidx = 0; epidx < hsotg->num_of_eps; epidx++) {
		struct s3c_hsotg_ep *ep = &hsotg->eps[epidx];

		ep->debugfs = debugfs_create_file(ep->name, 0444,
						  root, ep, &ep_fops);

		if (IS_ERR(ep->debugfs))
			dev_err(hsotg->dev, "failed to create %s debug file\n",
				ep->name);
	}
}

/**
 * s3c_hsotg_delete_debug - cleanup debugfs entries
 * @hsotg: The driver state
 *
 * Cleanup (remove) the debugfs files for use on module exit.
 */
static void s3c_hsotg_delete_debug(struct s3c_hsotg *hsotg)
{
	unsigned epidx;

	for (epidx = 0; epidx < hsotg->num_of_eps; epidx++) {
		struct s3c_hsotg_ep *ep = &hsotg->eps[epidx];
		debugfs_remove(ep->debugfs);
	}

	debugfs_remove(hsotg->debug_file);
	debugfs_remove(hsotg->debug_fifo);
	debugfs_remove(hsotg->debug_root);
}

/**
 * s3c_hsotg_probe - probe function for hsotg driver
 * @pdev: The platform information for the driver
 */

static int s3c_hsotg_probe(struct platform_device *pdev)
{
	struct s3c_hsotg_plat *plat = dev_get_platdata(&pdev->dev);
	struct phy *phy;
	struct usb_phy *uphy;
	struct device *dev = &pdev->dev;
	struct s3c_hsotg_ep *eps;
	struct s3c_hsotg *hsotg;
	struct resource *res;
	int epnum;
	int ret;
	int i;

	hsotg = devm_kzalloc(&pdev->dev, sizeof(struct s3c_hsotg), GFP_KERNEL);
	if (!hsotg)
		return -ENOMEM;

	/* Set default UTMI width */
	hsotg->phyif = GUSBCFG_PHYIF16;

	/*
	 * Attempt to find a generic PHY, then look for an old style
	 * USB PHY, finally fall back to pdata
	 */
	phy = devm_phy_get(&pdev->dev, "usb2-phy");
	if (IS_ERR(phy)) {
		uphy = devm_usb_get_phy(dev, USB_PHY_TYPE_USB2);
		if (IS_ERR(uphy)) {
			/* Fallback for pdata */
			plat = dev_get_platdata(&pdev->dev);
			if (!plat) {
				dev_err(&pdev->dev,
				"no platform data or transceiver defined\n");
				return -EPROBE_DEFER;
			}
			hsotg->plat = plat;
		} else
			hsotg->uphy = uphy;
	} else {
		hsotg->phy = phy;
		/*
		 * If using the generic PHY framework, check if the PHY bus
		 * width is 8-bit and set the phyif appropriately.
		 */
		if (phy_get_bus_width(phy) == 8)
			hsotg->phyif = GUSBCFG_PHYIF8;
	}

	hsotg->dev = dev;

	hsotg->clk = devm_clk_get(&pdev->dev, "otg");
	if (IS_ERR(hsotg->clk)) {
		dev_err(dev, "cannot get otg clock\n");
		return PTR_ERR(hsotg->clk);
	}

	platform_set_drvdata(pdev, hsotg);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	hsotg->regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(hsotg->regs)) {
		ret = PTR_ERR(hsotg->regs);
		goto err_clk;
	}

	ret = platform_get_irq(pdev, 0);
	if (ret < 0) {
		dev_err(dev, "cannot find IRQ\n");
		goto err_clk;
	}

	spin_lock_init(&hsotg->lock);

	hsotg->irq = ret;

	dev_info(dev, "regs %p, irq %d\n", hsotg->regs, hsotg->irq);

	hsotg->gadget.max_speed = USB_SPEED_HIGH;
	hsotg->gadget.ops = &s3c_hsotg_gadget_ops;
	hsotg->gadget.name = dev_name(dev);

	/* reset the system */

	clk_prepare_enable(hsotg->clk);

	/* regulators */

	for (i = 0; i < ARRAY_SIZE(hsotg->supplies); i++)
		hsotg->supplies[i].supply = s3c_hsotg_supply_names[i];

	ret = devm_regulator_bulk_get(dev, ARRAY_SIZE(hsotg->supplies),
				 hsotg->supplies);
	if (ret) {
		dev_err(dev, "failed to request supplies: %d\n", ret);
		goto err_clk;
	}

	ret = regulator_bulk_enable(ARRAY_SIZE(hsotg->supplies),
				    hsotg->supplies);

	if (ret) {
		dev_err(hsotg->dev, "failed to enable supplies: %d\n", ret);
		goto err_supplies;
	}

	/* usb phy enable */
	s3c_hsotg_phy_enable(hsotg);

	s3c_hsotg_corereset(hsotg);
	s3c_hsotg_hw_cfg(hsotg);
	s3c_hsotg_init(hsotg);

	ret = devm_request_irq(&pdev->dev, hsotg->irq, s3c_hsotg_irq, 0,
				dev_name(dev), hsotg);
	if (ret < 0) {
		s3c_hsotg_phy_disable(hsotg);
		clk_disable_unprepare(hsotg->clk);
		regulator_bulk_disable(ARRAY_SIZE(hsotg->supplies),
				       hsotg->supplies);
		dev_err(dev, "cannot claim IRQ\n");
		goto err_clk;
=======
		 dwc2_readl(regs + DVBUSDIS), dwc2_readl(regs + DVBUSPULSE));
#endif
}

#ifdef CONFIG_OF
static void dwc2_hsotg_of_probe(struct dwc2_hsotg *hsotg)
{
	struct device_node *np = hsotg->dev->of_node;
	u32 len = 0;
	u32 i = 0;

	/* Enable dma if requested in device tree */
	hsotg->g_using_dma = of_property_read_bool(np, "g-use-dma");

	/*
	* Register TX periodic fifo size per endpoint.
	* EP0 is excluded since it has no fifo configuration.
	*/
	if (!of_find_property(np, "g-tx-fifo-size", &len))
		goto rx_fifo;

	len /= sizeof(u32);

	/* Read tx fifo sizes other than ep0 */
	if (of_property_read_u32_array(np, "g-tx-fifo-size",
						&hsotg->g_tx_fifo_sz[1], len))
		goto rx_fifo;

	/* Add ep0 */
	len++;

	/* Make remaining TX fifos unavailable */
	if (len < MAX_EPS_CHANNELS) {
		for (i = len; i < MAX_EPS_CHANNELS; i++)
			hsotg->g_tx_fifo_sz[i] = 0;
	}

rx_fifo:
	/* Register RX fifo size */
	of_property_read_u32(np, "g-rx-fifo-size", &hsotg->g_rx_fifo_sz);

	/* Register NPTX fifo size */
	of_property_read_u32(np, "g-np-tx-fifo-size",
						&hsotg->g_np_g_tx_fifo_sz);
}
#else
static inline void dwc2_hsotg_of_probe(struct dwc2_hsotg *hsotg) { }
#endif

/**
 * dwc2_gadget_init - init function for gadget
 * @dwc2: The data structure for the DWC2 driver.
 * @irq: The IRQ number for the controller.
 */
int dwc2_gadget_init(struct dwc2_hsotg *hsotg, int irq)
{
	struct device *dev = hsotg->dev;
	int epnum;
	int ret;
	int i;
	u32 p_tx_fifo[] = DWC2_G_P_LEGACY_TX_FIFO_SIZE;

	/* Initialize to legacy fifo configuration values */
	hsotg->g_rx_fifo_sz = 2048;
	hsotg->g_np_g_tx_fifo_sz = 1024;
	memcpy(&hsotg->g_tx_fifo_sz[1], p_tx_fifo, sizeof(p_tx_fifo));
	/* Device tree specific probe */
	dwc2_hsotg_of_probe(hsotg);

	/* Check against largest possible value. */
	if (hsotg->g_np_g_tx_fifo_sz >
	    hsotg->hw_params.dev_nperio_tx_fifo_size) {
		dev_warn(dev, "Specified GNPTXFDEP=%d > %d\n",
			 hsotg->g_np_g_tx_fifo_sz,
			 hsotg->hw_params.dev_nperio_tx_fifo_size);
		hsotg->g_np_g_tx_fifo_sz =
			hsotg->hw_params.dev_nperio_tx_fifo_size;
	}

	/* Dump fifo information */
	dev_dbg(dev, "NonPeriodic TXFIFO size: %d\n",
						hsotg->g_np_g_tx_fifo_sz);
	dev_dbg(dev, "RXFIFO size: %d\n", hsotg->g_rx_fifo_sz);
	for (i = 0; i < MAX_EPS_CHANNELS; i++)
		dev_dbg(dev, "Periodic TXFIFO%2d size: %d\n", i,
						hsotg->g_tx_fifo_sz[i]);

	hsotg->gadget.max_speed = USB_SPEED_HIGH;
	hsotg->gadget.ops = &dwc2_hsotg_gadget_ops;
	hsotg->gadget.name = dev_name(dev);
	if (hsotg->dr_mode == USB_DR_MODE_OTG)
		hsotg->gadget.is_otg = 1;
	else if (hsotg->dr_mode == USB_DR_MODE_PERIPHERAL)
		hsotg->op_state = OTG_STATE_B_PERIPHERAL;

	ret = dwc2_hsotg_hw_cfg(hsotg);
	if (ret) {
		dev_err(hsotg->dev, "Hardware configuration failed: %d\n", ret);
		return ret;
	}

	hsotg->ctrl_buff = devm_kzalloc(hsotg->dev,
			DWC2_CTRL_BUFF_SIZE, GFP_KERNEL);
	if (!hsotg->ctrl_buff)
		return -ENOMEM;

	hsotg->ep0_buff = devm_kzalloc(hsotg->dev,
			DWC2_CTRL_BUFF_SIZE, GFP_KERNEL);
	if (!hsotg->ep0_buff)
		return -ENOMEM;

	ret = devm_request_irq(hsotg->dev, irq, dwc2_hsotg_irq, IRQF_SHARED,
				dev_name(hsotg->dev), hsotg);
	if (ret < 0) {
		dev_err(dev, "cannot claim IRQ for gadget\n");
		return ret;
>>>>>>> v4.9.227
	}

	/* hsotg->num_of_eps holds number of EPs other than ep0 */

	if (hsotg->num_of_eps == 0) {
		dev_err(dev, "wrong number of EPs (zero)\n");
<<<<<<< HEAD
		ret = -EINVAL;
		goto err_supplies;
	}

	eps = kcalloc(hsotg->num_of_eps + 1, sizeof(struct s3c_hsotg_ep),
		      GFP_KERNEL);
	if (!eps) {
		ret = -ENOMEM;
		goto err_supplies;
	}

	hsotg->eps = eps;

	/* setup endpoint information */

	INIT_LIST_HEAD(&hsotg->gadget.ep_list);
	hsotg->gadget.ep0 = &hsotg->eps[0].ep;

	/* allocate EP0 request */

	hsotg->ctrl_req = s3c_hsotg_ep_alloc_request(&hsotg->eps[0].ep,
						     GFP_KERNEL);
	if (!hsotg->ctrl_req) {
		dev_err(dev, "failed to allocate ctrl req\n");
		ret = -ENOMEM;
		goto err_ep_mem;
	}

	/* initialise the endpoints now the core has been initialised */
	for (epnum = 0; epnum < hsotg->num_of_eps; epnum++)
		s3c_hsotg_initep(hsotg, &hsotg->eps[epnum], epnum);

	/* disable power and clock */
	s3c_hsotg_phy_disable(hsotg);

	ret = regulator_bulk_disable(ARRAY_SIZE(hsotg->supplies),
				    hsotg->supplies);
	if (ret) {
		dev_err(hsotg->dev, "failed to disable supplies: %d\n", ret);
		goto err_ep_mem;
	}

	ret = usb_add_gadget_udc(&pdev->dev, &hsotg->gadget);
	if (ret)
		goto err_ep_mem;

	s3c_hsotg_create_debug(hsotg);

	s3c_hsotg_dump(hsotg);

	return 0;

err_ep_mem:
	kfree(eps);
err_supplies:
	s3c_hsotg_phy_disable(hsotg);
err_clk:
	clk_disable_unprepare(hsotg->clk);

	return ret;
}

/**
 * s3c_hsotg_remove - remove function for hsotg driver
 * @pdev: The platform information for the driver
 */
static int s3c_hsotg_remove(struct platform_device *pdev)
{
	struct s3c_hsotg *hsotg = platform_get_drvdata(pdev);

	usb_del_gadget_udc(&hsotg->gadget);

	s3c_hsotg_delete_debug(hsotg);

	if (hsotg->driver) {
		/* should have been done already by driver model core */
		usb_gadget_unregister_driver(hsotg->driver);
	}

	clk_disable_unprepare(hsotg->clk);
=======
		return -EINVAL;
	}

	/* setup endpoint information */

	INIT_LIST_HEAD(&hsotg->gadget.ep_list);
	hsotg->gadget.ep0 = &hsotg->eps_out[0]->ep;

	/* allocate EP0 request */

	hsotg->ctrl_req = dwc2_hsotg_ep_alloc_request(&hsotg->eps_out[0]->ep,
						     GFP_KERNEL);
	if (!hsotg->ctrl_req) {
		dev_err(dev, "failed to allocate ctrl req\n");
		return -ENOMEM;
	}

	/* initialise the endpoints now the core has been initialised */
	for (epnum = 0; epnum < hsotg->num_of_eps; epnum++) {
		if (hsotg->eps_in[epnum])
			dwc2_hsotg_initep(hsotg, hsotg->eps_in[epnum],
								epnum, 1);
		if (hsotg->eps_out[epnum])
			dwc2_hsotg_initep(hsotg, hsotg->eps_out[epnum],
								epnum, 0);
	}

	ret = usb_add_gadget_udc(dev, &hsotg->gadget);
	if (ret) {
		dwc2_hsotg_ep_free_request(&hsotg->eps_out[0]->ep,
					   hsotg->ctrl_req);
		return ret;
	}
	dwc2_hsotg_dump(hsotg);

	return 0;
}

/**
 * dwc2_hsotg_remove - remove function for hsotg driver
 * @pdev: The platform information for the driver
 */
int dwc2_hsotg_remove(struct dwc2_hsotg *hsotg)
{
	usb_del_gadget_udc(&hsotg->gadget);
	dwc2_hsotg_ep_free_request(&hsotg->eps_out[0]->ep, hsotg->ctrl_req);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int s3c_hsotg_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct s3c_hsotg *hsotg = platform_get_drvdata(pdev);
	unsigned long flags;
	int ret = 0;

	if (hsotg->driver)
		dev_info(hsotg->dev, "suspending usb gadget %s\n",
			 hsotg->driver->driver.name);

	spin_lock_irqsave(&hsotg->lock, flags);
	s3c_hsotg_disconnect(hsotg);
	s3c_hsotg_phy_disable(hsotg);
	hsotg->gadget.speed = USB_SPEED_UNKNOWN;
	spin_unlock_irqrestore(&hsotg->lock, flags);

	if (hsotg->driver) {
		int ep;
		for (ep = 0; ep < hsotg->num_of_eps; ep++)
			s3c_hsotg_ep_disable(&hsotg->eps[ep].ep);

		ret = regulator_bulk_disable(ARRAY_SIZE(hsotg->supplies),
					     hsotg->supplies);
		clk_disable(hsotg->clk);
	}

	return ret;
}

static int s3c_hsotg_resume(struct platform_device *pdev)
{
	struct s3c_hsotg *hsotg = platform_get_drvdata(pdev);
	unsigned long flags;
	int ret = 0;
=======
int dwc2_hsotg_suspend(struct dwc2_hsotg *hsotg)
{
	unsigned long flags;

	if (hsotg->lx_state != DWC2_L0)
		return 0;

	if (hsotg->driver) {
		int ep;

		dev_info(hsotg->dev, "suspending usb gadget %s\n",
			 hsotg->driver->driver.name);

		spin_lock_irqsave(&hsotg->lock, flags);
		if (hsotg->enabled)
			dwc2_hsotg_core_disconnect(hsotg);
		dwc2_hsotg_disconnect(hsotg);
		hsotg->gadget.speed = USB_SPEED_UNKNOWN;
		spin_unlock_irqrestore(&hsotg->lock, flags);

		for (ep = 0; ep < hsotg->num_of_eps; ep++) {
			if (hsotg->eps_in[ep])
				dwc2_hsotg_ep_disable(&hsotg->eps_in[ep]->ep);
			if (hsotg->eps_out[ep])
				dwc2_hsotg_ep_disable(&hsotg->eps_out[ep]->ep);
		}
	}

	return 0;
}

int dwc2_hsotg_resume(struct dwc2_hsotg *hsotg)
{
	unsigned long flags;

	if (hsotg->lx_state == DWC2_L2)
		return 0;
>>>>>>> v4.9.227

	if (hsotg->driver) {
		dev_info(hsotg->dev, "resuming usb gadget %s\n",
			 hsotg->driver->driver.name);

<<<<<<< HEAD
		clk_enable(hsotg->clk);
		ret = regulator_bulk_enable(ARRAY_SIZE(hsotg->supplies),
				      hsotg->supplies);
	}

	spin_lock_irqsave(&hsotg->lock, flags);
	hsotg->last_rst = jiffies;
	s3c_hsotg_phy_enable(hsotg);
	s3c_hsotg_core_init(hsotg);
	spin_unlock_irqrestore(&hsotg->lock, flags);

	return ret;
}

#ifdef CONFIG_OF
static const struct of_device_id s3c_hsotg_of_ids[] = {
	{ .compatible = "samsung,s3c6400-hsotg", },
	{ .compatible = "snps,dwc2", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, s3c_hsotg_of_ids);
#endif

static struct platform_driver s3c_hsotg_driver = {
	.driver		= {
		.name	= "s3c-hsotg",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(s3c_hsotg_of_ids),
	},
	.probe		= s3c_hsotg_probe,
	.remove		= s3c_hsotg_remove,
	.suspend	= s3c_hsotg_suspend,
	.resume		= s3c_hsotg_resume,
};

module_platform_driver(s3c_hsotg_driver);

MODULE_DESCRIPTION("Samsung S3C USB High-speed/OtG device");
MODULE_AUTHOR("Ben Dooks <ben@simtec.co.uk>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:s3c-hsotg");
=======
		spin_lock_irqsave(&hsotg->lock, flags);
		dwc2_hsotg_core_init_disconnected(hsotg, false);
		if (hsotg->enabled)
			dwc2_hsotg_core_connect(hsotg);
		spin_unlock_irqrestore(&hsotg->lock, flags);
	}

	return 0;
}

/**
 * dwc2_backup_device_registers() - Backup controller device registers.
 * When suspending usb bus, registers needs to be backuped
 * if controller power is disabled once suspended.
 *
 * @hsotg: Programming view of the DWC_otg controller
 */
int dwc2_backup_device_registers(struct dwc2_hsotg *hsotg)
{
	struct dwc2_dregs_backup *dr;
	int i;

	dev_dbg(hsotg->dev, "%s\n", __func__);

	/* Backup dev regs */
	dr = &hsotg->dr_backup;

	dr->dcfg = dwc2_readl(hsotg->regs + DCFG);
	dr->dctl = dwc2_readl(hsotg->regs + DCTL);
	dr->daintmsk = dwc2_readl(hsotg->regs + DAINTMSK);
	dr->diepmsk = dwc2_readl(hsotg->regs + DIEPMSK);
	dr->doepmsk = dwc2_readl(hsotg->regs + DOEPMSK);

	for (i = 0; i < hsotg->num_of_eps; i++) {
		/* Backup IN EPs */
		dr->diepctl[i] = dwc2_readl(hsotg->regs + DIEPCTL(i));

		/* Ensure DATA PID is correctly configured */
		if (dr->diepctl[i] & DXEPCTL_DPID)
			dr->diepctl[i] |= DXEPCTL_SETD1PID;
		else
			dr->diepctl[i] |= DXEPCTL_SETD0PID;

		dr->dieptsiz[i] = dwc2_readl(hsotg->regs + DIEPTSIZ(i));
		dr->diepdma[i] = dwc2_readl(hsotg->regs + DIEPDMA(i));

		/* Backup OUT EPs */
		dr->doepctl[i] = dwc2_readl(hsotg->regs + DOEPCTL(i));

		/* Ensure DATA PID is correctly configured */
		if (dr->doepctl[i] & DXEPCTL_DPID)
			dr->doepctl[i] |= DXEPCTL_SETD1PID;
		else
			dr->doepctl[i] |= DXEPCTL_SETD0PID;

		dr->doeptsiz[i] = dwc2_readl(hsotg->regs + DOEPTSIZ(i));
		dr->doepdma[i] = dwc2_readl(hsotg->regs + DOEPDMA(i));
	}
	dr->valid = true;
	return 0;
}

/**
 * dwc2_restore_device_registers() - Restore controller device registers.
 * When resuming usb bus, device registers needs to be restored
 * if controller power were disabled.
 *
 * @hsotg: Programming view of the DWC_otg controller
 */
int dwc2_restore_device_registers(struct dwc2_hsotg *hsotg)
{
	struct dwc2_dregs_backup *dr;
	u32 dctl;
	int i;

	dev_dbg(hsotg->dev, "%s\n", __func__);

	/* Restore dev regs */
	dr = &hsotg->dr_backup;
	if (!dr->valid) {
		dev_err(hsotg->dev, "%s: no device registers to restore\n",
			__func__);
		return -EINVAL;
	}
	dr->valid = false;

	dwc2_writel(dr->dcfg, hsotg->regs + DCFG);
	dwc2_writel(dr->dctl, hsotg->regs + DCTL);
	dwc2_writel(dr->daintmsk, hsotg->regs + DAINTMSK);
	dwc2_writel(dr->diepmsk, hsotg->regs + DIEPMSK);
	dwc2_writel(dr->doepmsk, hsotg->regs + DOEPMSK);

	for (i = 0; i < hsotg->num_of_eps; i++) {
		/* Restore IN EPs */
		dwc2_writel(dr->diepctl[i], hsotg->regs + DIEPCTL(i));
		dwc2_writel(dr->dieptsiz[i], hsotg->regs + DIEPTSIZ(i));
		dwc2_writel(dr->diepdma[i], hsotg->regs + DIEPDMA(i));

		/* Restore OUT EPs */
		dwc2_writel(dr->doepctl[i], hsotg->regs + DOEPCTL(i));
		dwc2_writel(dr->doeptsiz[i], hsotg->regs + DOEPTSIZ(i));
		dwc2_writel(dr->doepdma[i], hsotg->regs + DOEPDMA(i));
	}

	/* Set the Power-On Programming done bit */
	dctl = dwc2_readl(hsotg->regs + DCTL);
	dctl |= DCTL_PWRONPRGDONE;
	dwc2_writel(dctl, hsotg->regs + DCTL);

	return 0;
}
>>>>>>> v4.9.227
