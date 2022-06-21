/*
 * Intel I/OAT DMA Linux driver
<<<<<<< HEAD
 * Copyright(c) 2004 - 2009 Intel Corporation.
=======
 * Copyright(c) 2004 - 2015 Intel Corporation.
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
<<<<<<< HEAD
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
=======
>>>>>>> v4.9.227
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 */

/*
 * This driver supports an Intel I/OAT DMA engine, which does asynchronous
 * copy operations.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/dmaengine.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/workqueue.h>
#include <linux/prefetch.h>
<<<<<<< HEAD
#include <linux/i7300_idle.h>
=======
#include <linux/sizes.h>
>>>>>>> v4.9.227
#include "dma.h"
#include "registers.h"
#include "hw.h"

#include "../dmaengine.h"

<<<<<<< HEAD
int ioat_pending_level = 4;
module_param(ioat_pending_level, int, 0644);
MODULE_PARM_DESC(ioat_pending_level,
		 "high-water mark for pushing ioat descriptors (default: 4)");

/* internal functions */
static void ioat1_cleanup(struct ioat_dma_chan *ioat);
static void ioat1_dma_start_null_desc(struct ioat_dma_chan *ioat);
=======
static char *chanerr_str[] = {
	"DMA Transfer Destination Address Error",
	"Next Descriptor Address Error",
	"Descriptor Error",
	"Chan Address Value Error",
	"CHANCMD Error",
	"Chipset Uncorrectable Data Integrity Error",
	"DMA Uncorrectable Data Integrity Error",
	"Read Data Error",
	"Write Data Error",
	"Descriptor Control Error",
	"Descriptor Transfer Size Error",
	"Completion Address Error",
	"Interrupt Configuration Error",
	"Super extended descriptor Address Error",
	"Unaffiliated Error",
	"CRC or XOR P Error",
	"XOR Q Error",
	"Descriptor Count Error",
	"DIF All F detect Error",
	"Guard Tag verification Error",
	"Application Tag verification Error",
	"Reference Tag verification Error",
	"Bundle Bit Error",
	"Result DIF All F detect Error",
	"Result Guard Tag verification Error",
	"Result Application Tag verification Error",
	"Result Reference Tag verification Error",
	NULL
};

static void ioat_eh(struct ioatdma_chan *ioat_chan);

static void ioat_print_chanerrs(struct ioatdma_chan *ioat_chan, u32 chanerr)
{
	int i;

	for (i = 0; i < 32; i++) {
		if ((chanerr >> i) & 1) {
			if (chanerr_str[i]) {
				dev_err(to_dev(ioat_chan), "Err(%d): %s\n",
					i, chanerr_str[i]);
			} else
				break;
		}
	}
}
>>>>>>> v4.9.227

/**
 * ioat_dma_do_interrupt - handler used for single vector interrupt mode
 * @irq: interrupt id
 * @data: interrupt data
 */
<<<<<<< HEAD
static irqreturn_t ioat_dma_do_interrupt(int irq, void *data)
{
	struct ioatdma_device *instance = data;
	struct ioat_chan_common *chan;
=======
irqreturn_t ioat_dma_do_interrupt(int irq, void *data)
{
	struct ioatdma_device *instance = data;
	struct ioatdma_chan *ioat_chan;
>>>>>>> v4.9.227
	unsigned long attnstatus;
	int bit;
	u8 intrctrl;

	intrctrl = readb(instance->reg_base + IOAT_INTRCTRL_OFFSET);

	if (!(intrctrl & IOAT_INTRCTRL_MASTER_INT_EN))
		return IRQ_NONE;

	if (!(intrctrl & IOAT_INTRCTRL_INT_STATUS)) {
		writeb(intrctrl, instance->reg_base + IOAT_INTRCTRL_OFFSET);
		return IRQ_NONE;
	}

	attnstatus = readl(instance->reg_base + IOAT_ATTNSTATUS_OFFSET);
	for_each_set_bit(bit, &attnstatus, BITS_PER_LONG) {
<<<<<<< HEAD
		chan = ioat_chan_by_index(instance, bit);
		if (test_bit(IOAT_RUN, &chan->state))
			tasklet_schedule(&chan->cleanup_task);
=======
		ioat_chan = ioat_chan_by_index(instance, bit);
		if (test_bit(IOAT_RUN, &ioat_chan->state))
			tasklet_schedule(&ioat_chan->cleanup_task);
>>>>>>> v4.9.227
	}

	writeb(intrctrl, instance->reg_base + IOAT_INTRCTRL_OFFSET);
	return IRQ_HANDLED;
}

/**
 * ioat_dma_do_interrupt_msix - handler used for vector-per-channel interrupt mode
 * @irq: interrupt id
 * @data: interrupt data
 */
<<<<<<< HEAD
static irqreturn_t ioat_dma_do_interrupt_msix(int irq, void *data)
{
	struct ioat_chan_common *chan = data;

	if (test_bit(IOAT_RUN, &chan->state))
		tasklet_schedule(&chan->cleanup_task);
=======
irqreturn_t ioat_dma_do_interrupt_msix(int irq, void *data)
{
	struct ioatdma_chan *ioat_chan = data;

	if (test_bit(IOAT_RUN, &ioat_chan->state))
		tasklet_schedule(&ioat_chan->cleanup_task);
>>>>>>> v4.9.227

	return IRQ_HANDLED;
}

<<<<<<< HEAD
/* common channel initialization */
void ioat_init_channel(struct ioatdma_device *device, struct ioat_chan_common *chan, int idx)
{
	struct dma_device *dma = &device->common;
	struct dma_chan *c = &chan->common;
	unsigned long data = (unsigned long) c;

	chan->device = device;
	chan->reg_base = device->reg_base + (0x80 * (idx + 1));
	spin_lock_init(&chan->cleanup_lock);
	chan->common.device = dma;
	dma_cookie_init(&chan->common);
	list_add_tail(&chan->common.device_node, &dma->channels);
	device->idx[idx] = chan;
	init_timer(&chan->timer);
	chan->timer.function = device->timer_fn;
	chan->timer.data = data;
	tasklet_init(&chan->cleanup_task, device->cleanup_fn, data);
}

/**
 * ioat1_dma_enumerate_channels - find and initialize the device's channels
 * @device: the device to be enumerated
 */
static int ioat1_enumerate_channels(struct ioatdma_device *device)
{
	u8 xfercap_scale;
	u32 xfercap;
	int i;
	struct ioat_dma_chan *ioat;
	struct device *dev = &device->pdev->dev;
	struct dma_device *dma = &device->common;

	INIT_LIST_HEAD(&dma->channels);
	dma->chancnt = readb(device->reg_base + IOAT_CHANCNT_OFFSET);
	dma->chancnt &= 0x1f; /* bits [4:0] valid */
	if (dma->chancnt > ARRAY_SIZE(device->idx)) {
		dev_warn(dev, "(%d) exceeds max supported channels (%zu)\n",
			 dma->chancnt, ARRAY_SIZE(device->idx));
		dma->chancnt = ARRAY_SIZE(device->idx);
	}
	xfercap_scale = readb(device->reg_base + IOAT_XFERCAP_OFFSET);
	xfercap_scale &= 0x1f; /* bits [4:0] valid */
	xfercap = (xfercap_scale == 0 ? -1 : (1UL << xfercap_scale));
	dev_dbg(dev, "%s: xfercap = %d\n", __func__, xfercap);

#ifdef  CONFIG_I7300_IDLE_IOAT_CHANNEL
	if (i7300_idle_platform_probe(NULL, NULL, 1) == 0)
		dma->chancnt--;
#endif
	for (i = 0; i < dma->chancnt; i++) {
		ioat = devm_kzalloc(dev, sizeof(*ioat), GFP_KERNEL);
		if (!ioat)
			break;

		ioat_init_channel(device, &ioat->base, i);
		ioat->xfercap = xfercap;
		spin_lock_init(&ioat->desc_lock);
		INIT_LIST_HEAD(&ioat->free_desc);
		INIT_LIST_HEAD(&ioat->used_desc);
	}
	dma->chancnt = i;
	return i;
}

/**
 * ioat_dma_memcpy_issue_pending - push potentially unrecognized appended
 *                                 descriptors to hw
 * @chan: DMA channel handle
 */
static inline void
__ioat1_dma_memcpy_issue_pending(struct ioat_dma_chan *ioat)
{
	void __iomem *reg_base = ioat->base.reg_base;

	dev_dbg(to_dev(&ioat->base), "%s: pending: %d\n",
		__func__, ioat->pending);
	ioat->pending = 0;
	writeb(IOAT_CHANCMD_APPEND, reg_base + IOAT1_CHANCMD_OFFSET);
}

static void ioat1_dma_memcpy_issue_pending(struct dma_chan *chan)
{
	struct ioat_dma_chan *ioat = to_ioat_chan(chan);

	if (ioat->pending > 0) {
		spin_lock_bh(&ioat->desc_lock);
		__ioat1_dma_memcpy_issue_pending(ioat);
		spin_unlock_bh(&ioat->desc_lock);
	}
}

/**
 * ioat1_reset_channel - restart a channel
 * @ioat: IOAT DMA channel handle
 */
static void ioat1_reset_channel(struct ioat_dma_chan *ioat)
{
	struct ioat_chan_common *chan = &ioat->base;
	void __iomem *reg_base = chan->reg_base;
	u32 chansts, chanerr;

	dev_warn(to_dev(chan), "reset\n");
	chanerr = readl(reg_base + IOAT_CHANERR_OFFSET);
	chansts = *chan->completion & IOAT_CHANSTS_STATUS;
	if (chanerr) {
		dev_err(to_dev(chan),
			"chan%d, CHANSTS = 0x%08x CHANERR = 0x%04x, clearing\n",
			chan_num(chan), chansts, chanerr);
		writel(chanerr, reg_base + IOAT_CHANERR_OFFSET);
	}

	/*
	 * whack it upside the head with a reset
	 * and wait for things to settle out.
	 * force the pending count to a really big negative
	 * to make sure no one forces an issue_pending
	 * while we're waiting.
	 */

	ioat->pending = INT_MIN;
	writeb(IOAT_CHANCMD_RESET,
	       reg_base + IOAT_CHANCMD_OFFSET(chan->device->version));
	set_bit(IOAT_RESET_PENDING, &chan->state);
	mod_timer(&chan->timer, jiffies + RESET_DELAY);
}

static dma_cookie_t ioat1_tx_submit(struct dma_async_tx_descriptor *tx)
{
	struct dma_chan *c = tx->chan;
	struct ioat_dma_chan *ioat = to_ioat_chan(c);
	struct ioat_desc_sw *desc = tx_to_ioat_desc(tx);
	struct ioat_chan_common *chan = &ioat->base;
	struct ioat_desc_sw *first;
	struct ioat_desc_sw *chain_tail;
	dma_cookie_t cookie;

	spin_lock_bh(&ioat->desc_lock);
	/* cookie incr and addition to used_list must be atomic */
	cookie = dma_cookie_assign(tx);
	dev_dbg(to_dev(&ioat->base), "%s: cookie: %d\n", __func__, cookie);

	/* write address into NextDescriptor field of last desc in chain */
	first = to_ioat_desc(desc->tx_list.next);
	chain_tail = to_ioat_desc(ioat->used_desc.prev);
	/* make descriptor updates globally visible before chaining */
	wmb();
	chain_tail->hw->next = first->txd.phys;
	list_splice_tail_init(&desc->tx_list, &ioat->used_desc);
	dump_desc_dbg(ioat, chain_tail);
	dump_desc_dbg(ioat, first);

	if (!test_and_set_bit(IOAT_COMPLETION_PENDING, &chan->state))
		mod_timer(&chan->timer, jiffies + COMPLETION_TIMEOUT);

	ioat->active += desc->hw->tx_cnt;
	ioat->pending += desc->hw->tx_cnt;
	if (ioat->pending >= ioat_pending_level)
		__ioat1_dma_memcpy_issue_pending(ioat);
	spin_unlock_bh(&ioat->desc_lock);

	return cookie;
}

/**
 * ioat_dma_alloc_descriptor - allocate and return a sw and hw descriptor pair
 * @ioat: the channel supplying the memory pool for the descriptors
 * @flags: allocation flags
 */
static struct ioat_desc_sw *
ioat_dma_alloc_descriptor(struct ioat_dma_chan *ioat, gfp_t flags)
{
	struct ioat_dma_descriptor *desc;
	struct ioat_desc_sw *desc_sw;
	struct ioatdma_device *ioatdma_device;
	dma_addr_t phys;

	ioatdma_device = ioat->base.device;
	desc = pci_pool_alloc(ioatdma_device->dma_pool, flags, &phys);
	if (unlikely(!desc))
		return NULL;

	desc_sw = kzalloc(sizeof(*desc_sw), flags);
	if (unlikely(!desc_sw)) {
		pci_pool_free(ioatdma_device->dma_pool, desc, phys);
		return NULL;
	}

	memset(desc, 0, sizeof(*desc));

	INIT_LIST_HEAD(&desc_sw->tx_list);
	dma_async_tx_descriptor_init(&desc_sw->txd, &ioat->base.common);
	desc_sw->txd.tx_submit = ioat1_tx_submit;
	desc_sw->hw = desc;
	desc_sw->txd.phys = phys;
	set_desc_id(desc_sw, -1);

	return desc_sw;
}

static int ioat_initial_desc_count = 256;
module_param(ioat_initial_desc_count, int, 0644);
MODULE_PARM_DESC(ioat_initial_desc_count,
		 "ioat1: initial descriptors per channel (default: 256)");
/**
 * ioat1_dma_alloc_chan_resources - returns the number of allocated descriptors
 * @chan: the channel to be filled out
 */
static int ioat1_dma_alloc_chan_resources(struct dma_chan *c)
{
	struct ioat_dma_chan *ioat = to_ioat_chan(c);
	struct ioat_chan_common *chan = &ioat->base;
	struct ioat_desc_sw *desc;
	u32 chanerr;
	int i;
	LIST_HEAD(tmp_list);

	/* have we already been set up? */
	if (!list_empty(&ioat->free_desc))
		return ioat->desccount;

	/* Setup register to interrupt and write completion status on error */
	writew(IOAT_CHANCTRL_RUN, chan->reg_base + IOAT_CHANCTRL_OFFSET);

	chanerr = readl(chan->reg_base + IOAT_CHANERR_OFFSET);
	if (chanerr) {
		dev_err(to_dev(chan), "CHANERR = %x, clearing\n", chanerr);
		writel(chanerr, chan->reg_base + IOAT_CHANERR_OFFSET);
	}

	/* Allocate descriptors */
	for (i = 0; i < ioat_initial_desc_count; i++) {
		desc = ioat_dma_alloc_descriptor(ioat, GFP_KERNEL);
		if (!desc) {
			dev_err(to_dev(chan), "Only %d initial descriptors\n", i);
			break;
		}
		set_desc_id(desc, i);
		list_add_tail(&desc->node, &tmp_list);
	}
	spin_lock_bh(&ioat->desc_lock);
	ioat->desccount = i;
	list_splice(&tmp_list, &ioat->free_desc);
	spin_unlock_bh(&ioat->desc_lock);

	/* allocate a completion writeback area */
	/* doing 2 32bit writes to mmio since 1 64b write doesn't work */
	chan->completion = pci_pool_alloc(chan->device->completion_pool,
					  GFP_KERNEL, &chan->completion_dma);
	memset(chan->completion, 0, sizeof(*chan->completion));
	writel(((u64) chan->completion_dma) & 0x00000000FFFFFFFF,
	       chan->reg_base + IOAT_CHANCMP_OFFSET_LOW);
	writel(((u64) chan->completion_dma) >> 32,
	       chan->reg_base + IOAT_CHANCMP_OFFSET_HIGH);

	set_bit(IOAT_RUN, &chan->state);
	ioat1_dma_start_null_desc(ioat);  /* give chain to dma device */
	dev_dbg(to_dev(chan), "%s: allocated %d descriptors\n",
		__func__, ioat->desccount);
	return ioat->desccount;
}

void ioat_stop(struct ioat_chan_common *chan)
{
	struct ioatdma_device *device = chan->device;
	struct pci_dev *pdev = device->pdev;
	int chan_id = chan_num(chan);
=======
void ioat_stop(struct ioatdma_chan *ioat_chan)
{
	struct ioatdma_device *ioat_dma = ioat_chan->ioat_dma;
	struct pci_dev *pdev = ioat_dma->pdev;
	int chan_id = chan_num(ioat_chan);
>>>>>>> v4.9.227
	struct msix_entry *msix;

	/* 1/ stop irq from firing tasklets
	 * 2/ stop the tasklet from re-arming irqs
	 */
<<<<<<< HEAD
	clear_bit(IOAT_RUN, &chan->state);

	/* flush inflight interrupts */
	switch (device->irq_mode) {
	case IOAT_MSIX:
		msix = &device->msix_entries[chan_id];
=======
	clear_bit(IOAT_RUN, &ioat_chan->state);

	/* flush inflight interrupts */
	switch (ioat_dma->irq_mode) {
	case IOAT_MSIX:
		msix = &ioat_dma->msix_entries[chan_id];
>>>>>>> v4.9.227
		synchronize_irq(msix->vector);
		break;
	case IOAT_MSI:
	case IOAT_INTX:
		synchronize_irq(pdev->irq);
		break;
	default:
		break;
	}

	/* flush inflight timers */
<<<<<<< HEAD
	del_timer_sync(&chan->timer);

	/* flush inflight tasklet runs */
	tasklet_kill(&chan->cleanup_task);

	/* final cleanup now that everything is quiesced and can't re-arm */
	device->cleanup_fn((unsigned long) &chan->common);
}

/**
 * ioat1_dma_free_chan_resources - release all the descriptors
 * @chan: the channel to be cleaned
 */
static void ioat1_dma_free_chan_resources(struct dma_chan *c)
{
	struct ioat_dma_chan *ioat = to_ioat_chan(c);
	struct ioat_chan_common *chan = &ioat->base;
	struct ioatdma_device *ioatdma_device = chan->device;
	struct ioat_desc_sw *desc, *_desc;
	int in_use_descs = 0;

	/* Before freeing channel resources first check
	 * if they have been previously allocated for this channel.
	 */
	if (ioat->desccount == 0)
		return;

	ioat_stop(chan);

	/* Delay 100ms after reset to allow internal DMA logic to quiesce
	 * before removing DMA descriptor resources.
	 */
	writeb(IOAT_CHANCMD_RESET,
	       chan->reg_base + IOAT_CHANCMD_OFFSET(chan->device->version));
	mdelay(100);

	spin_lock_bh(&ioat->desc_lock);
	list_for_each_entry_safe(desc, _desc, &ioat->used_desc, node) {
		dev_dbg(to_dev(chan), "%s: freeing %d from used list\n",
			__func__, desc_id(desc));
		dump_desc_dbg(ioat, desc);
		in_use_descs++;
		list_del(&desc->node);
		pci_pool_free(ioatdma_device->dma_pool, desc->hw,
			      desc->txd.phys);
		kfree(desc);
	}
	list_for_each_entry_safe(desc, _desc,
				 &ioat->free_desc, node) {
		list_del(&desc->node);
		pci_pool_free(ioatdma_device->dma_pool, desc->hw,
			      desc->txd.phys);
		kfree(desc);
	}
	spin_unlock_bh(&ioat->desc_lock);

	pci_pool_free(ioatdma_device->completion_pool,
		      chan->completion,
		      chan->completion_dma);

	/* one is ok since we left it on there on purpose */
	if (in_use_descs > 1)
		dev_err(to_dev(chan), "Freeing %d in use descriptors!\n",
			in_use_descs - 1);

	chan->last_completion = 0;
	chan->completion_dma = 0;
	ioat->pending = 0;
	ioat->desccount = 0;
}

/**
 * ioat1_dma_get_next_descriptor - return the next available descriptor
 * @ioat: IOAT DMA channel handle
 *
 * Gets the next descriptor from the chain, and must be called with the
 * channel's desc_lock held.  Allocates more descriptors if the channel
 * has run out.
 */
static struct ioat_desc_sw *
ioat1_dma_get_next_descriptor(struct ioat_dma_chan *ioat)
{
	struct ioat_desc_sw *new;

	if (!list_empty(&ioat->free_desc)) {
		new = to_ioat_desc(ioat->free_desc.next);
		list_del(&new->node);
	} else {
		/* try to get another desc */
		new = ioat_dma_alloc_descriptor(ioat, GFP_ATOMIC);
		if (!new) {
			dev_err(to_dev(&ioat->base), "alloc failed\n");
			return NULL;
		}
	}
	dev_dbg(to_dev(&ioat->base), "%s: allocated: %d\n",
		__func__, desc_id(new));
	prefetch(new->hw);
	return new;
}

static struct dma_async_tx_descriptor *
ioat1_dma_prep_memcpy(struct dma_chan *c, dma_addr_t dma_dest,
		      dma_addr_t dma_src, size_t len, unsigned long flags)
{
	struct ioat_dma_chan *ioat = to_ioat_chan(c);
	struct ioat_desc_sw *desc;
	size_t copy;
	LIST_HEAD(chain);
	dma_addr_t src = dma_src;
	dma_addr_t dest = dma_dest;
	size_t total_len = len;
	struct ioat_dma_descriptor *hw = NULL;
	int tx_cnt = 0;

	spin_lock_bh(&ioat->desc_lock);
	desc = ioat1_dma_get_next_descriptor(ioat);
	do {
		if (!desc)
			break;

		tx_cnt++;
		copy = min_t(size_t, len, ioat->xfercap);

		hw = desc->hw;
		hw->size = copy;
		hw->ctl = 0;
		hw->src_addr = src;
		hw->dst_addr = dest;

		list_add_tail(&desc->node, &chain);

		len -= copy;
		dest += copy;
		src += copy;
		if (len) {
			struct ioat_desc_sw *next;

			async_tx_ack(&desc->txd);
			next = ioat1_dma_get_next_descriptor(ioat);
			hw->next = next ? next->txd.phys : 0;
			dump_desc_dbg(ioat, desc);
			desc = next;
		} else
			hw->next = 0;
	} while (len);

	if (!desc) {
		struct ioat_chan_common *chan = &ioat->base;

		dev_err(to_dev(chan),
			"chan%d - get_next_desc failed\n", chan_num(chan));
		list_splice(&chain, &ioat->free_desc);
		spin_unlock_bh(&ioat->desc_lock);
		return NULL;
	}
	spin_unlock_bh(&ioat->desc_lock);

	desc->txd.flags = flags;
	desc->len = total_len;
	list_splice(&chain, &desc->tx_list);
	hw->ctl_f.int_en = !!(flags & DMA_PREP_INTERRUPT);
	hw->ctl_f.compl_write = 1;
	hw->tx_cnt = tx_cnt;
	dump_desc_dbg(ioat, desc);

	return &desc->txd;
}

static void ioat1_cleanup_event(unsigned long data)
{
	struct ioat_dma_chan *ioat = to_ioat_chan((void *) data);
	struct ioat_chan_common *chan = &ioat->base;

	ioat1_cleanup(ioat);
	if (!test_bit(IOAT_RUN, &chan->state))
		return;
	writew(IOAT_CHANCTRL_RUN, ioat->base.reg_base + IOAT_CHANCTRL_OFFSET);
}

dma_addr_t ioat_get_current_completion(struct ioat_chan_common *chan)
{
	dma_addr_t phys_complete;
	u64 completion;

	completion = *chan->completion;
	phys_complete = ioat_chansts_to_addr(completion);

	dev_dbg(to_dev(chan), "%s: phys_complete: %#llx\n", __func__,
		(unsigned long long) phys_complete);

	if (is_ioat_halted(completion)) {
		u32 chanerr = readl(chan->reg_base + IOAT_CHANERR_OFFSET);
		dev_err(to_dev(chan), "Channel halted, chanerr = %x\n",
			chanerr);

		/* TODO do something to salvage the situation */
	}

	return phys_complete;
}

bool ioat_cleanup_preamble(struct ioat_chan_common *chan,
			   dma_addr_t *phys_complete)
{
	*phys_complete = ioat_get_current_completion(chan);
	if (*phys_complete == chan->last_completion)
		return false;
	clear_bit(IOAT_COMPLETION_ACK, &chan->state);
	mod_timer(&chan->timer, jiffies + COMPLETION_TIMEOUT);

	return true;
}

static void __cleanup(struct ioat_dma_chan *ioat, dma_addr_t phys_complete)
{
	struct ioat_chan_common *chan = &ioat->base;
	struct list_head *_desc, *n;
	struct dma_async_tx_descriptor *tx;

	dev_dbg(to_dev(chan), "%s: phys_complete: %llx\n",
		 __func__, (unsigned long long) phys_complete);
	list_for_each_safe(_desc, n, &ioat->used_desc) {
		struct ioat_desc_sw *desc;

		prefetch(n);
		desc = list_entry(_desc, typeof(*desc), node);
		tx = &desc->txd;
		/*
		 * Incoming DMA requests may use multiple descriptors,
		 * due to exceeding xfercap, perhaps. If so, only the
		 * last one will have a cookie, and require unmapping.
		 */
		dump_desc_dbg(ioat, desc);
		if (tx->cookie) {
			dma_cookie_complete(tx);
			dma_descriptor_unmap(tx);
			ioat->active -= desc->hw->tx_cnt;
			if (tx->callback) {
				tx->callback(tx->callback_param);
				tx->callback = NULL;
			}
		}

		if (tx->phys != phys_complete) {
			/*
			 * a completed entry, but not the last, so clean
			 * up if the client is done with the descriptor
			 */
			if (async_tx_test_ack(tx))
				list_move_tail(&desc->node, &ioat->free_desc);
		} else {
			/*
			 * last used desc. Do not remove, so we can
			 * append from it.
			 */

			/* if nothing else is pending, cancel the
			 * completion timeout
			 */
			if (n == &ioat->used_desc) {
				dev_dbg(to_dev(chan),
					"%s cancel completion timeout\n",
					__func__);
				clear_bit(IOAT_COMPLETION_PENDING, &chan->state);
			}

			/* TODO check status bits? */
			break;
		}
	}

	chan->last_completion = phys_complete;
}

/**
 * ioat1_cleanup - cleanup up finished descriptors
 * @chan: ioat channel to be cleaned up
 *
 * To prevent lock contention we defer cleanup when the locks are
 * contended with a terminal timeout that forces cleanup and catches
 * completion notification errors.
 */
static void ioat1_cleanup(struct ioat_dma_chan *ioat)
{
	struct ioat_chan_common *chan = &ioat->base;
	dma_addr_t phys_complete;

	prefetch(chan->completion);

	if (!spin_trylock_bh(&chan->cleanup_lock))
		return;

	if (!ioat_cleanup_preamble(chan, &phys_complete)) {
		spin_unlock_bh(&chan->cleanup_lock);
		return;
	}

	if (!spin_trylock_bh(&ioat->desc_lock)) {
		spin_unlock_bh(&chan->cleanup_lock);
		return;
	}

	__cleanup(ioat, phys_complete);

	spin_unlock_bh(&ioat->desc_lock);
	spin_unlock_bh(&chan->cleanup_lock);
}

static void ioat1_timer_event(unsigned long data)
{
	struct ioat_dma_chan *ioat = to_ioat_chan((void *) data);
	struct ioat_chan_common *chan = &ioat->base;

	dev_dbg(to_dev(chan), "%s: state: %lx\n", __func__, chan->state);

	spin_lock_bh(&chan->cleanup_lock);
	if (test_and_clear_bit(IOAT_RESET_PENDING, &chan->state)) {
		struct ioat_desc_sw *desc;

		spin_lock_bh(&ioat->desc_lock);

		/* restart active descriptors */
		desc = to_ioat_desc(ioat->used_desc.prev);
		ioat_set_chainaddr(ioat, desc->txd.phys);
		ioat_start(chan);

		ioat->pending = 0;
		set_bit(IOAT_COMPLETION_PENDING, &chan->state);
		mod_timer(&chan->timer, jiffies + COMPLETION_TIMEOUT);
		spin_unlock_bh(&ioat->desc_lock);
	} else if (test_bit(IOAT_COMPLETION_PENDING, &chan->state)) {
		dma_addr_t phys_complete;

		spin_lock_bh(&ioat->desc_lock);
		/* if we haven't made progress and we have already
		 * acknowledged a pending completion once, then be more
		 * forceful with a restart
		 */
		if (ioat_cleanup_preamble(chan, &phys_complete))
			__cleanup(ioat, phys_complete);
		else if (test_bit(IOAT_COMPLETION_ACK, &chan->state))
			ioat1_reset_channel(ioat);
		else {
			u64 status = ioat_chansts(chan);

			/* manually update the last completion address */
			if (ioat_chansts_to_addr(status) != 0)
				*chan->completion = status;

			set_bit(IOAT_COMPLETION_ACK, &chan->state);
			mod_timer(&chan->timer, jiffies + COMPLETION_TIMEOUT);
		}
		spin_unlock_bh(&ioat->desc_lock);
	}
	spin_unlock_bh(&chan->cleanup_lock);
}

enum dma_status
ioat_dma_tx_status(struct dma_chan *c, dma_cookie_t cookie,
		   struct dma_tx_state *txstate)
{
	struct ioat_chan_common *chan = to_chan_common(c);
	struct ioatdma_device *device = chan->device;
	enum dma_status ret;

	ret = dma_cookie_status(c, cookie, txstate);
	if (ret == DMA_COMPLETE)
		return ret;

	device->cleanup_fn((unsigned long) c);

	return dma_cookie_status(c, cookie, txstate);
}

static void ioat1_dma_start_null_desc(struct ioat_dma_chan *ioat)
{
	struct ioat_chan_common *chan = &ioat->base;
	struct ioat_desc_sw *desc;
	struct ioat_dma_descriptor *hw;

	spin_lock_bh(&ioat->desc_lock);

	desc = ioat1_dma_get_next_descriptor(ioat);

	if (!desc) {
		dev_err(to_dev(chan),
			"Unable to start null desc - get next desc failed\n");
		spin_unlock_bh(&ioat->desc_lock);
		return;
	}

=======
	del_timer_sync(&ioat_chan->timer);

	/* flush inflight tasklet runs */
	tasklet_kill(&ioat_chan->cleanup_task);

	/* final cleanup now that everything is quiesced and can't re-arm */
	ioat_cleanup_event((unsigned long)&ioat_chan->dma_chan);
}

static void __ioat_issue_pending(struct ioatdma_chan *ioat_chan)
{
	ioat_chan->dmacount += ioat_ring_pending(ioat_chan);
	ioat_chan->issued = ioat_chan->head;
	writew(ioat_chan->dmacount,
	       ioat_chan->reg_base + IOAT_CHAN_DMACOUNT_OFFSET);
	dev_dbg(to_dev(ioat_chan),
		"%s: head: %#x tail: %#x issued: %#x count: %#x\n",
		__func__, ioat_chan->head, ioat_chan->tail,
		ioat_chan->issued, ioat_chan->dmacount);
}

void ioat_issue_pending(struct dma_chan *c)
{
	struct ioatdma_chan *ioat_chan = to_ioat_chan(c);

	if (ioat_ring_pending(ioat_chan)) {
		spin_lock_bh(&ioat_chan->prep_lock);
		__ioat_issue_pending(ioat_chan);
		spin_unlock_bh(&ioat_chan->prep_lock);
	}
}

/**
 * ioat_update_pending - log pending descriptors
 * @ioat: ioat+ channel
 *
 * Check if the number of unsubmitted descriptors has exceeded the
 * watermark.  Called with prep_lock held
 */
static void ioat_update_pending(struct ioatdma_chan *ioat_chan)
{
	if (ioat_ring_pending(ioat_chan) > ioat_pending_level)
		__ioat_issue_pending(ioat_chan);
}

static void __ioat_start_null_desc(struct ioatdma_chan *ioat_chan)
{
	struct ioat_ring_ent *desc;
	struct ioat_dma_descriptor *hw;

	if (ioat_ring_space(ioat_chan) < 1) {
		dev_err(to_dev(ioat_chan),
			"Unable to start null desc - ring full\n");
		return;
	}

	dev_dbg(to_dev(ioat_chan),
		"%s: head: %#x tail: %#x issued: %#x\n",
		__func__, ioat_chan->head, ioat_chan->tail, ioat_chan->issued);
	desc = ioat_get_ring_ent(ioat_chan, ioat_chan->head);

>>>>>>> v4.9.227
	hw = desc->hw;
	hw->ctl = 0;
	hw->ctl_f.null = 1;
	hw->ctl_f.int_en = 1;
	hw->ctl_f.compl_write = 1;
	/* set size to non-zero value (channel returns error when size is 0) */
	hw->size = NULL_DESC_BUFFER_SIZE;
	hw->src_addr = 0;
	hw->dst_addr = 0;
	async_tx_ack(&desc->txd);
<<<<<<< HEAD
	hw->next = 0;
	list_add_tail(&desc->node, &ioat->used_desc);
	dump_desc_dbg(ioat, desc);

	ioat_set_chainaddr(ioat, desc->txd.phys);
	ioat_start(chan);
	spin_unlock_bh(&ioat->desc_lock);
}

/*
 * Perform a IOAT transaction to verify the HW works.
 */
#define IOAT_TEST_SIZE 2000

static void ioat_dma_test_callback(void *dma_async_param)
{
	struct completion *cmp = dma_async_param;

	complete(cmp);
}

/**
 * ioat_dma_self_test - Perform a IOAT transaction to verify the HW works.
 * @device: device to be tested
 */
int ioat_dma_self_test(struct ioatdma_device *device)
{
	int i;
	u8 *src;
	u8 *dest;
	struct dma_device *dma = &device->common;
	struct device *dev = &device->pdev->dev;
	struct dma_chan *dma_chan;
	struct dma_async_tx_descriptor *tx;
	dma_addr_t dma_dest, dma_src;
	dma_cookie_t cookie;
	int err = 0;
	struct completion cmp;
	unsigned long tmo;
	unsigned long flags;

	src = kzalloc(sizeof(u8) * IOAT_TEST_SIZE, GFP_KERNEL);
	if (!src)
		return -ENOMEM;
	dest = kzalloc(sizeof(u8) * IOAT_TEST_SIZE, GFP_KERNEL);
	if (!dest) {
		kfree(src);
		return -ENOMEM;
	}

	/* Fill in src buffer */
	for (i = 0; i < IOAT_TEST_SIZE; i++)
		src[i] = (u8)i;

	/* Start copy, using first DMA channel */
	dma_chan = container_of(dma->channels.next, struct dma_chan,
				device_node);
	if (dma->device_alloc_chan_resources(dma_chan) < 1) {
		dev_err(dev, "selftest cannot allocate chan resource\n");
		err = -ENODEV;
		goto out;
	}

	dma_src = dma_map_single(dev, src, IOAT_TEST_SIZE, DMA_TO_DEVICE);
	if (dma_mapping_error(dev, dma_src)) {
		dev_err(dev, "mapping src buffer failed\n");
		goto free_resources;
	}
	dma_dest = dma_map_single(dev, dest, IOAT_TEST_SIZE, DMA_FROM_DEVICE);
	if (dma_mapping_error(dev, dma_dest)) {
		dev_err(dev, "mapping dest buffer failed\n");
		goto unmap_src;
	}
	flags = DMA_PREP_INTERRUPT;
	tx = device->common.device_prep_dma_memcpy(dma_chan, dma_dest, dma_src,
						   IOAT_TEST_SIZE, flags);
	if (!tx) {
		dev_err(dev, "Self-test prep failed, disabling\n");
		err = -ENODEV;
		goto unmap_dma;
	}

	async_tx_ack(tx);
	init_completion(&cmp);
	tx->callback = ioat_dma_test_callback;
	tx->callback_param = &cmp;
	cookie = tx->tx_submit(tx);
	if (cookie < 0) {
		dev_err(dev, "Self-test setup failed, disabling\n");
		err = -ENODEV;
		goto unmap_dma;
	}
	dma->device_issue_pending(dma_chan);

	tmo = wait_for_completion_timeout(&cmp, msecs_to_jiffies(3000));

	if (tmo == 0 ||
	    dma->device_tx_status(dma_chan, cookie, NULL)
					!= DMA_COMPLETE) {
		dev_err(dev, "Self-test copy timed out, disabling\n");
		err = -ENODEV;
		goto unmap_dma;
	}
	if (memcmp(src, dest, IOAT_TEST_SIZE)) {
		dev_err(dev, "Self-test copy failed compare, disabling\n");
		err = -ENODEV;
		goto free_resources;
	}

unmap_dma:
	dma_unmap_single(dev, dma_dest, IOAT_TEST_SIZE, DMA_FROM_DEVICE);
unmap_src:
	dma_unmap_single(dev, dma_src, IOAT_TEST_SIZE, DMA_TO_DEVICE);
free_resources:
	dma->device_free_chan_resources(dma_chan);
out:
	kfree(src);
	kfree(dest);
	return err;
}

static char ioat_interrupt_style[32] = "msix";
module_param_string(ioat_interrupt_style, ioat_interrupt_style,
		    sizeof(ioat_interrupt_style), 0644);
MODULE_PARM_DESC(ioat_interrupt_style,
		 "set ioat interrupt style: msix (default), msi, intx");

/**
 * ioat_dma_setup_interrupts - setup interrupt handler
 * @device: ioat device
 */
int ioat_dma_setup_interrupts(struct ioatdma_device *device)
{
	struct ioat_chan_common *chan;
	struct pci_dev *pdev = device->pdev;
	struct device *dev = &pdev->dev;
	struct msix_entry *msix;
	int i, j, msixcnt;
	int err = -EINVAL;
	u8 intrctrl = 0;

	if (!strcmp(ioat_interrupt_style, "msix"))
		goto msix;
	if (!strcmp(ioat_interrupt_style, "msi"))
		goto msi;
	if (!strcmp(ioat_interrupt_style, "intx"))
		goto intx;
	dev_err(dev, "invalid ioat_interrupt_style %s\n", ioat_interrupt_style);
	goto err_no_irq;

msix:
	/* The number of MSI-X vectors should equal the number of channels */
	msixcnt = device->common.chancnt;
	for (i = 0; i < msixcnt; i++)
		device->msix_entries[i].entry = i;

	err = pci_enable_msix_exact(pdev, device->msix_entries, msixcnt);
	if (err)
		goto msi;

	for (i = 0; i < msixcnt; i++) {
		msix = &device->msix_entries[i];
		chan = ioat_chan_by_index(device, i);
		err = devm_request_irq(dev, msix->vector,
				       ioat_dma_do_interrupt_msix, 0,
				       "ioat-msix", chan);
		if (err) {
			for (j = 0; j < i; j++) {
				msix = &device->msix_entries[j];
				chan = ioat_chan_by_index(device, j);
				devm_free_irq(dev, msix->vector, chan);
			}
			goto msi;
		}
	}
	intrctrl |= IOAT_INTRCTRL_MSIX_VECTOR_CONTROL;
	device->irq_mode = IOAT_MSIX;
	goto done;

msi:
	err = pci_enable_msi(pdev);
	if (err)
		goto intx;

	err = devm_request_irq(dev, pdev->irq, ioat_dma_do_interrupt, 0,
			       "ioat-msi", device);
	if (err) {
		pci_disable_msi(pdev);
		goto intx;
	}
	device->irq_mode = IOAT_MSI;
	goto done;

intx:
	err = devm_request_irq(dev, pdev->irq, ioat_dma_do_interrupt,
			       IRQF_SHARED, "ioat-intx", device);
	if (err)
		goto err_no_irq;

	device->irq_mode = IOAT_INTX;
done:
	if (device->intr_quirk)
		device->intr_quirk(device);
	intrctrl |= IOAT_INTRCTRL_MASTER_INT_EN;
	writeb(intrctrl, device->reg_base + IOAT_INTRCTRL_OFFSET);
	return 0;

err_no_irq:
	/* Disable all interrupt generation */
	writeb(0, device->reg_base + IOAT_INTRCTRL_OFFSET);
	device->irq_mode = IOAT_NOIRQ;
	dev_err(dev, "no usable interrupts\n");
	return err;
}
EXPORT_SYMBOL(ioat_dma_setup_interrupts);

static void ioat_disable_interrupts(struct ioatdma_device *device)
{
	/* Disable all interrupt generation */
	writeb(0, device->reg_base + IOAT_INTRCTRL_OFFSET);
}

int ioat_probe(struct ioatdma_device *device)
{
	int err = -ENODEV;
	struct dma_device *dma = &device->common;
	struct pci_dev *pdev = device->pdev;
	struct device *dev = &pdev->dev;

	/* DMA coherent memory pool for DMA descriptor allocations */
	device->dma_pool = pci_pool_create("dma_desc_pool", pdev,
					   sizeof(struct ioat_dma_descriptor),
					   64, 0);
	if (!device->dma_pool) {
		err = -ENOMEM;
		goto err_dma_pool;
	}

	device->completion_pool = pci_pool_create("completion_pool", pdev,
						  sizeof(u64), SMP_CACHE_BYTES,
						  SMP_CACHE_BYTES);

	if (!device->completion_pool) {
		err = -ENOMEM;
		goto err_completion_pool;
	}

	device->enumerate_channels(device);

	dma_cap_set(DMA_MEMCPY, dma->cap_mask);
	dma->dev = &pdev->dev;

	if (!dma->chancnt) {
		dev_err(dev, "channel enumeration error\n");
		goto err_setup_interrupts;
	}

	err = ioat_dma_setup_interrupts(device);
	if (err)
		goto err_setup_interrupts;

	err = device->self_test(device);
	if (err)
		goto err_self_test;

	return 0;

err_self_test:
	ioat_disable_interrupts(device);
err_setup_interrupts:
	pci_pool_destroy(device->completion_pool);
err_completion_pool:
	pci_pool_destroy(device->dma_pool);
err_dma_pool:
	return err;
}

int ioat_register(struct ioatdma_device *device)
{
	int err = dma_async_device_register(&device->common);

	if (err) {
		ioat_disable_interrupts(device);
		pci_pool_destroy(device->completion_pool);
		pci_pool_destroy(device->dma_pool);
	}

	return err;
}

/* ioat1_intr_quirk - fix up dma ctrl register to enable / disable msi */
static void ioat1_intr_quirk(struct ioatdma_device *device)
{
	struct pci_dev *pdev = device->pdev;
	u32 dmactrl;

	pci_read_config_dword(pdev, IOAT_PCI_DMACTRL_OFFSET, &dmactrl);
	if (pdev->msi_enabled)
		dmactrl |= IOAT_PCI_DMACTRL_MSI_EN;
	else
		dmactrl &= ~IOAT_PCI_DMACTRL_MSI_EN;
	pci_write_config_dword(pdev, IOAT_PCI_DMACTRL_OFFSET, dmactrl);
}

static ssize_t ring_size_show(struct dma_chan *c, char *page)
{
	struct ioat_dma_chan *ioat = to_ioat_chan(c);

	return sprintf(page, "%d\n", ioat->desccount);
}
static struct ioat_sysfs_entry ring_size_attr = __ATTR_RO(ring_size);

static ssize_t ring_active_show(struct dma_chan *c, char *page)
{
	struct ioat_dma_chan *ioat = to_ioat_chan(c);

	return sprintf(page, "%d\n", ioat->active);
}
static struct ioat_sysfs_entry ring_active_attr = __ATTR_RO(ring_active);

static ssize_t cap_show(struct dma_chan *c, char *page)
{
	struct dma_device *dma = c->device;

	return sprintf(page, "copy%s%s%s%s%s\n",
		       dma_has_cap(DMA_PQ, dma->cap_mask) ? " pq" : "",
		       dma_has_cap(DMA_PQ_VAL, dma->cap_mask) ? " pq_val" : "",
		       dma_has_cap(DMA_XOR, dma->cap_mask) ? " xor" : "",
		       dma_has_cap(DMA_XOR_VAL, dma->cap_mask) ? " xor_val" : "",
		       dma_has_cap(DMA_INTERRUPT, dma->cap_mask) ? " intr" : "");

}
struct ioat_sysfs_entry ioat_cap_attr = __ATTR_RO(cap);

static ssize_t version_show(struct dma_chan *c, char *page)
{
	struct dma_device *dma = c->device;
	struct ioatdma_device *device = to_ioatdma_device(dma);

	return sprintf(page, "%d.%d\n",
		       device->version >> 4, device->version & 0xf);
}
struct ioat_sysfs_entry ioat_version_attr = __ATTR_RO(version);

static struct attribute *ioat1_attrs[] = {
	&ring_size_attr.attr,
	&ring_active_attr.attr,
	&ioat_cap_attr.attr,
	&ioat_version_attr.attr,
	NULL,
};

static ssize_t
ioat_attr_show(struct kobject *kobj, struct attribute *attr, char *page)
{
	struct ioat_sysfs_entry *entry;
	struct ioat_chan_common *chan;

	entry = container_of(attr, struct ioat_sysfs_entry, attr);
	chan = container_of(kobj, struct ioat_chan_common, kobj);

	if (!entry->show)
		return -EIO;
	return entry->show(&chan->common, page);
}

const struct sysfs_ops ioat_sysfs_ops = {
	.show	= ioat_attr_show,
};

static struct kobj_type ioat1_ktype = {
	.sysfs_ops = &ioat_sysfs_ops,
	.default_attrs = ioat1_attrs,
};

void ioat_kobject_add(struct ioatdma_device *device, struct kobj_type *type)
{
	struct dma_device *dma = &device->common;
	struct dma_chan *c;

	list_for_each_entry(c, &dma->channels, device_node) {
		struct ioat_chan_common *chan = to_chan_common(c);
		struct kobject *parent = &c->dev->device.kobj;
		int err;

		err = kobject_init_and_add(&chan->kobj, type, parent, "quickdata");
		if (err) {
			dev_warn(to_dev(chan),
				 "sysfs init error (%d), continuing...\n", err);
			kobject_put(&chan->kobj);
			set_bit(IOAT_KOBJ_INIT_FAIL, &chan->state);
		}
	}
}

void ioat_kobject_del(struct ioatdma_device *device)
{
	struct dma_device *dma = &device->common;
	struct dma_chan *c;

	list_for_each_entry(c, &dma->channels, device_node) {
		struct ioat_chan_common *chan = to_chan_common(c);

		if (!test_bit(IOAT_KOBJ_INIT_FAIL, &chan->state)) {
			kobject_del(&chan->kobj);
			kobject_put(&chan->kobj);
		}
	}
}

int ioat1_dma_probe(struct ioatdma_device *device, int dca)
{
	struct pci_dev *pdev = device->pdev;
	struct dma_device *dma;
	int err;

	device->intr_quirk = ioat1_intr_quirk;
	device->enumerate_channels = ioat1_enumerate_channels;
	device->self_test = ioat_dma_self_test;
	device->timer_fn = ioat1_timer_event;
	device->cleanup_fn = ioat1_cleanup_event;
	dma = &device->common;
	dma->device_prep_dma_memcpy = ioat1_dma_prep_memcpy;
	dma->device_issue_pending = ioat1_dma_memcpy_issue_pending;
	dma->device_alloc_chan_resources = ioat1_dma_alloc_chan_resources;
	dma->device_free_chan_resources = ioat1_dma_free_chan_resources;
	dma->device_tx_status = ioat_dma_tx_status;

	err = ioat_probe(device);
	if (err)
		return err;
	err = ioat_register(device);
	if (err)
		return err;
	ioat_kobject_add(device, &ioat1_ktype);

	if (dca)
		device->dca = ioat_dca_init(pdev, device->reg_base);

	return err;
}

void ioat_dma_remove(struct ioatdma_device *device)
{
	struct dma_device *dma = &device->common;

	ioat_disable_interrupts(device);

	ioat_kobject_del(device);

	dma_async_device_unregister(dma);

	pci_pool_destroy(device->dma_pool);
	pci_pool_destroy(device->completion_pool);

	INIT_LIST_HEAD(&dma->channels);
}
=======
	ioat_set_chainaddr(ioat_chan, desc->txd.phys);
	dump_desc_dbg(ioat_chan, desc);
	/* make sure descriptors are written before we submit */
	wmb();
	ioat_chan->head += 1;
	__ioat_issue_pending(ioat_chan);
}

void ioat_start_null_desc(struct ioatdma_chan *ioat_chan)
{
	spin_lock_bh(&ioat_chan->prep_lock);
	if (!test_bit(IOAT_CHAN_DOWN, &ioat_chan->state))
		__ioat_start_null_desc(ioat_chan);
	spin_unlock_bh(&ioat_chan->prep_lock);
}

static void __ioat_restart_chan(struct ioatdma_chan *ioat_chan)
{
	/* set the tail to be re-issued */
	ioat_chan->issued = ioat_chan->tail;
	ioat_chan->dmacount = 0;
	mod_timer(&ioat_chan->timer, jiffies + COMPLETION_TIMEOUT);

	dev_dbg(to_dev(ioat_chan),
		"%s: head: %#x tail: %#x issued: %#x count: %#x\n",
		__func__, ioat_chan->head, ioat_chan->tail,
		ioat_chan->issued, ioat_chan->dmacount);

	if (ioat_ring_pending(ioat_chan)) {
		struct ioat_ring_ent *desc;

		desc = ioat_get_ring_ent(ioat_chan, ioat_chan->tail);
		ioat_set_chainaddr(ioat_chan, desc->txd.phys);
		__ioat_issue_pending(ioat_chan);
	} else
		__ioat_start_null_desc(ioat_chan);
}

static int ioat_quiesce(struct ioatdma_chan *ioat_chan, unsigned long tmo)
{
	unsigned long end = jiffies + tmo;
	int err = 0;
	u32 status;

	status = ioat_chansts(ioat_chan);
	if (is_ioat_active(status) || is_ioat_idle(status))
		ioat_suspend(ioat_chan);
	while (is_ioat_active(status) || is_ioat_idle(status)) {
		if (tmo && time_after(jiffies, end)) {
			err = -ETIMEDOUT;
			break;
		}
		status = ioat_chansts(ioat_chan);
		cpu_relax();
	}

	return err;
}

static int ioat_reset_sync(struct ioatdma_chan *ioat_chan, unsigned long tmo)
{
	unsigned long end = jiffies + tmo;
	int err = 0;

	ioat_reset(ioat_chan);
	while (ioat_reset_pending(ioat_chan)) {
		if (end && time_after(jiffies, end)) {
			err = -ETIMEDOUT;
			break;
		}
		cpu_relax();
	}

	return err;
}

static dma_cookie_t ioat_tx_submit_unlock(struct dma_async_tx_descriptor *tx)
	__releases(&ioat_chan->prep_lock)
{
	struct dma_chan *c = tx->chan;
	struct ioatdma_chan *ioat_chan = to_ioat_chan(c);
	dma_cookie_t cookie;

	cookie = dma_cookie_assign(tx);
	dev_dbg(to_dev(ioat_chan), "%s: cookie: %d\n", __func__, cookie);

	if (!test_and_set_bit(IOAT_CHAN_ACTIVE, &ioat_chan->state))
		mod_timer(&ioat_chan->timer, jiffies + COMPLETION_TIMEOUT);

	/* make descriptor updates visible before advancing ioat->head,
	 * this is purposefully not smp_wmb() since we are also
	 * publishing the descriptor updates to a dma device
	 */
	wmb();

	ioat_chan->head += ioat_chan->produce;

	ioat_update_pending(ioat_chan);
	spin_unlock_bh(&ioat_chan->prep_lock);

	return cookie;
}

static struct ioat_ring_ent *
ioat_alloc_ring_ent(struct dma_chan *chan, int idx, gfp_t flags)
{
	struct ioat_dma_descriptor *hw;
	struct ioat_ring_ent *desc;
	struct ioatdma_device *ioat_dma;
	struct ioatdma_chan *ioat_chan = to_ioat_chan(chan);
	int chunk;
	dma_addr_t phys;
	u8 *pos;
	off_t offs;

	ioat_dma = to_ioatdma_device(chan->device);

	chunk = idx / IOAT_DESCS_PER_2M;
	idx &= (IOAT_DESCS_PER_2M - 1);
	offs = idx * IOAT_DESC_SZ;
	pos = (u8 *)ioat_chan->descs[chunk].virt + offs;
	phys = ioat_chan->descs[chunk].hw + offs;
	hw = (struct ioat_dma_descriptor *)pos;
	memset(hw, 0, sizeof(*hw));

	desc = kmem_cache_zalloc(ioat_cache, flags);
	if (!desc)
		return NULL;

	dma_async_tx_descriptor_init(&desc->txd, chan);
	desc->txd.tx_submit = ioat_tx_submit_unlock;
	desc->hw = hw;
	desc->txd.phys = phys;
	return desc;
}

void ioat_free_ring_ent(struct ioat_ring_ent *desc, struct dma_chan *chan)
{
	kmem_cache_free(ioat_cache, desc);
}

struct ioat_ring_ent **
ioat_alloc_ring(struct dma_chan *c, int order, gfp_t flags)
{
	struct ioatdma_chan *ioat_chan = to_ioat_chan(c);
	struct ioat_ring_ent **ring;
	int total_descs = 1 << order;
	int i, chunks;

	/* allocate the array to hold the software ring */
	ring = kcalloc(total_descs, sizeof(*ring), flags);
	if (!ring)
		return NULL;

	ioat_chan->desc_chunks = chunks = (total_descs * IOAT_DESC_SZ) / SZ_2M;

	for (i = 0; i < chunks; i++) {
		struct ioat_descs *descs = &ioat_chan->descs[i];

		descs->virt = dma_alloc_coherent(to_dev(ioat_chan),
						 SZ_2M, &descs->hw, flags);
		if (!descs->virt) {
			int idx;

			for (idx = 0; idx < i; idx++) {
				descs = &ioat_chan->descs[idx];
				dma_free_coherent(to_dev(ioat_chan), SZ_2M,
						  descs->virt, descs->hw);
				descs->virt = NULL;
				descs->hw = 0;
			}

			ioat_chan->desc_chunks = 0;
			kfree(ring);
			return NULL;
		}
	}

	for (i = 0; i < total_descs; i++) {
		ring[i] = ioat_alloc_ring_ent(c, i, flags);
		if (!ring[i]) {
			int idx;

			while (i--)
				ioat_free_ring_ent(ring[i], c);

			for (idx = 0; idx < ioat_chan->desc_chunks; idx++) {
				dma_free_coherent(to_dev(ioat_chan),
						  SZ_2M,
						  ioat_chan->descs[idx].virt,
						  ioat_chan->descs[idx].hw);
				ioat_chan->descs[idx].virt = NULL;
				ioat_chan->descs[idx].hw = 0;
			}

			ioat_chan->desc_chunks = 0;
			kfree(ring);
			return NULL;
		}
		set_desc_id(ring[i], i);
	}

	/* link descs */
	for (i = 0; i < total_descs-1; i++) {
		struct ioat_ring_ent *next = ring[i+1];
		struct ioat_dma_descriptor *hw = ring[i]->hw;

		hw->next = next->txd.phys;
	}
	ring[i]->hw->next = ring[0]->txd.phys;

	return ring;
}

/**
 * ioat_check_space_lock - verify space and grab ring producer lock
 * @ioat: ioat,3 channel (ring) to operate on
 * @num_descs: allocation length
 */
int ioat_check_space_lock(struct ioatdma_chan *ioat_chan, int num_descs)
	__acquires(&ioat_chan->prep_lock)
{
	spin_lock_bh(&ioat_chan->prep_lock);
	/* never allow the last descriptor to be consumed, we need at
	 * least one free at all times to allow for on-the-fly ring
	 * resizing.
	 */
	if (likely(ioat_ring_space(ioat_chan) > num_descs)) {
		dev_dbg(to_dev(ioat_chan), "%s: num_descs: %d (%x:%x:%x)\n",
			__func__, num_descs, ioat_chan->head,
			ioat_chan->tail, ioat_chan->issued);
		ioat_chan->produce = num_descs;
		return 0;  /* with ioat->prep_lock held */
	}
	spin_unlock_bh(&ioat_chan->prep_lock);

	dev_dbg_ratelimited(to_dev(ioat_chan),
			    "%s: ring full! num_descs: %d (%x:%x:%x)\n",
			    __func__, num_descs, ioat_chan->head,
			    ioat_chan->tail, ioat_chan->issued);

	/* progress reclaim in the allocation failure case we may be
	 * called under bh_disabled so we need to trigger the timer
	 * event directly
	 */
	if (time_is_before_jiffies(ioat_chan->timer.expires)
	    && timer_pending(&ioat_chan->timer)) {
		mod_timer(&ioat_chan->timer, jiffies + COMPLETION_TIMEOUT);
		ioat_timer_event((unsigned long)ioat_chan);
	}

	return -ENOMEM;
}

static bool desc_has_ext(struct ioat_ring_ent *desc)
{
	struct ioat_dma_descriptor *hw = desc->hw;

	if (hw->ctl_f.op == IOAT_OP_XOR ||
	    hw->ctl_f.op == IOAT_OP_XOR_VAL) {
		struct ioat_xor_descriptor *xor = desc->xor;

		if (src_cnt_to_sw(xor->ctl_f.src_cnt) > 5)
			return true;
	} else if (hw->ctl_f.op == IOAT_OP_PQ ||
		   hw->ctl_f.op == IOAT_OP_PQ_VAL) {
		struct ioat_pq_descriptor *pq = desc->pq;

		if (src_cnt_to_sw(pq->ctl_f.src_cnt) > 3)
			return true;
	}

	return false;
}

static void
ioat_free_sed(struct ioatdma_device *ioat_dma, struct ioat_sed_ent *sed)
{
	if (!sed)
		return;

	dma_pool_free(ioat_dma->sed_hw_pool[sed->hw_pool], sed->hw, sed->dma);
	kmem_cache_free(ioat_sed_cache, sed);
}

static u64 ioat_get_current_completion(struct ioatdma_chan *ioat_chan)
{
	u64 phys_complete;
	u64 completion;

	completion = *ioat_chan->completion;
	phys_complete = ioat_chansts_to_addr(completion);

	dev_dbg(to_dev(ioat_chan), "%s: phys_complete: %#llx\n", __func__,
		(unsigned long long) phys_complete);

	return phys_complete;
}

static bool ioat_cleanup_preamble(struct ioatdma_chan *ioat_chan,
				   u64 *phys_complete)
{
	*phys_complete = ioat_get_current_completion(ioat_chan);
	if (*phys_complete == ioat_chan->last_completion)
		return false;

	clear_bit(IOAT_COMPLETION_ACK, &ioat_chan->state);
	mod_timer(&ioat_chan->timer, jiffies + COMPLETION_TIMEOUT);

	return true;
}

static void
desc_get_errstat(struct ioatdma_chan *ioat_chan, struct ioat_ring_ent *desc)
{
	struct ioat_dma_descriptor *hw = desc->hw;

	switch (hw->ctl_f.op) {
	case IOAT_OP_PQ_VAL:
	case IOAT_OP_PQ_VAL_16S:
	{
		struct ioat_pq_descriptor *pq = desc->pq;

		/* check if there's error written */
		if (!pq->dwbes_f.wbes)
			return;

		/* need to set a chanerr var for checking to clear later */

		if (pq->dwbes_f.p_val_err)
			*desc->result |= SUM_CHECK_P_RESULT;

		if (pq->dwbes_f.q_val_err)
			*desc->result |= SUM_CHECK_Q_RESULT;

		return;
	}
	default:
		return;
	}
}

/**
 * __cleanup - reclaim used descriptors
 * @ioat: channel (ring) to clean
 */
static void __cleanup(struct ioatdma_chan *ioat_chan, dma_addr_t phys_complete)
{
	struct ioatdma_device *ioat_dma = ioat_chan->ioat_dma;
	struct ioat_ring_ent *desc;
	bool seen_current = false;
	int idx = ioat_chan->tail, i;
	u16 active;

	dev_dbg(to_dev(ioat_chan), "%s: head: %#x tail: %#x issued: %#x\n",
		__func__, ioat_chan->head, ioat_chan->tail, ioat_chan->issued);

	/*
	 * At restart of the channel, the completion address and the
	 * channel status will be 0 due to starting a new chain. Since
	 * it's new chain and the first descriptor "fails", there is
	 * nothing to clean up. We do not want to reap the entire submitted
	 * chain due to this 0 address value and then BUG.
	 */
	if (!phys_complete)
		return;

	active = ioat_ring_active(ioat_chan);
	for (i = 0; i < active && !seen_current; i++) {
		struct dma_async_tx_descriptor *tx;

		smp_read_barrier_depends();
		prefetch(ioat_get_ring_ent(ioat_chan, idx + i + 1));
		desc = ioat_get_ring_ent(ioat_chan, idx + i);
		dump_desc_dbg(ioat_chan, desc);

		/* set err stat if we are using dwbes */
		if (ioat_dma->cap & IOAT_CAP_DWBES)
			desc_get_errstat(ioat_chan, desc);

		tx = &desc->txd;
		if (tx->cookie) {
			struct dmaengine_result res;

			dma_cookie_complete(tx);
			dma_descriptor_unmap(tx);
			res.result = DMA_TRANS_NOERROR;
			dmaengine_desc_get_callback_invoke(tx, NULL);
			tx->callback = NULL;
			tx->callback_result = NULL;
		}

		if (tx->phys == phys_complete)
			seen_current = true;

		/* skip extended descriptors */
		if (desc_has_ext(desc)) {
			BUG_ON(i + 1 >= active);
			i++;
		}

		/* cleanup super extended descriptors */
		if (desc->sed) {
			ioat_free_sed(ioat_dma, desc->sed);
			desc->sed = NULL;
		}
	}

	/* finish all descriptor reads before incrementing tail */
	smp_mb();
	ioat_chan->tail = idx + i;
	/* no active descs have written a completion? */
	BUG_ON(active && !seen_current);
	ioat_chan->last_completion = phys_complete;

	if (active - i == 0) {
		dev_dbg(to_dev(ioat_chan), "%s: cancel completion timeout\n",
			__func__);
		mod_timer(&ioat_chan->timer, jiffies + IDLE_TIMEOUT);
	}

	/* 5 microsecond delay per pending descriptor */
	writew(min((5 * (active - i)), IOAT_INTRDELAY_MASK),
	       ioat_chan->ioat_dma->reg_base + IOAT_INTRDELAY_OFFSET);
}

static void ioat_cleanup(struct ioatdma_chan *ioat_chan)
{
	u64 phys_complete;

	spin_lock_bh(&ioat_chan->cleanup_lock);

	if (ioat_cleanup_preamble(ioat_chan, &phys_complete))
		__cleanup(ioat_chan, phys_complete);

	if (is_ioat_halted(*ioat_chan->completion)) {
		u32 chanerr = readl(ioat_chan->reg_base + IOAT_CHANERR_OFFSET);

		if (chanerr &
		    (IOAT_CHANERR_HANDLE_MASK | IOAT_CHANERR_RECOVER_MASK)) {
			mod_timer(&ioat_chan->timer, jiffies + IDLE_TIMEOUT);
			ioat_eh(ioat_chan);
		}
	}

	spin_unlock_bh(&ioat_chan->cleanup_lock);
}

void ioat_cleanup_event(unsigned long data)
{
	struct ioatdma_chan *ioat_chan = to_ioat_chan((void *)data);

	ioat_cleanup(ioat_chan);
	if (!test_bit(IOAT_RUN, &ioat_chan->state))
		return;
	writew(IOAT_CHANCTRL_RUN, ioat_chan->reg_base + IOAT_CHANCTRL_OFFSET);
}

static void ioat_restart_channel(struct ioatdma_chan *ioat_chan)
{
	u64 phys_complete;

	ioat_quiesce(ioat_chan, 0);
	if (ioat_cleanup_preamble(ioat_chan, &phys_complete))
		__cleanup(ioat_chan, phys_complete);

	__ioat_restart_chan(ioat_chan);
}


static void ioat_abort_descs(struct ioatdma_chan *ioat_chan)
{
	struct ioatdma_device *ioat_dma = ioat_chan->ioat_dma;
	struct ioat_ring_ent *desc;
	u16 active;
	int idx = ioat_chan->tail, i;

	/*
	 * We assume that the failed descriptor has been processed.
	 * Now we are just returning all the remaining submitted
	 * descriptors to abort.
	 */
	active = ioat_ring_active(ioat_chan);

	/* we skip the failed descriptor that tail points to */
	for (i = 1; i < active; i++) {
		struct dma_async_tx_descriptor *tx;

		smp_read_barrier_depends();
		prefetch(ioat_get_ring_ent(ioat_chan, idx + i + 1));
		desc = ioat_get_ring_ent(ioat_chan, idx + i);

		tx = &desc->txd;
		if (tx->cookie) {
			struct dmaengine_result res;

			dma_cookie_complete(tx);
			dma_descriptor_unmap(tx);
			res.result = DMA_TRANS_ABORTED;
			dmaengine_desc_get_callback_invoke(tx, &res);
			tx->callback = NULL;
			tx->callback_result = NULL;
		}

		/* skip extended descriptors */
		if (desc_has_ext(desc)) {
			WARN_ON(i + 1 >= active);
			i++;
		}

		/* cleanup super extended descriptors */
		if (desc->sed) {
			ioat_free_sed(ioat_dma, desc->sed);
			desc->sed = NULL;
		}
	}

	smp_mb(); /* finish all descriptor reads before incrementing tail */
	ioat_chan->tail = idx + active;

	desc = ioat_get_ring_ent(ioat_chan, ioat_chan->tail);
	ioat_chan->last_completion = *ioat_chan->completion = desc->txd.phys;
}

static void ioat_eh(struct ioatdma_chan *ioat_chan)
{
	struct pci_dev *pdev = to_pdev(ioat_chan);
	struct ioat_dma_descriptor *hw;
	struct dma_async_tx_descriptor *tx;
	u64 phys_complete;
	struct ioat_ring_ent *desc;
	u32 err_handled = 0;
	u32 chanerr_int;
	u32 chanerr;
	bool abort = false;
	struct dmaengine_result res;

	/* cleanup so tail points to descriptor that caused the error */
	if (ioat_cleanup_preamble(ioat_chan, &phys_complete))
		__cleanup(ioat_chan, phys_complete);

	chanerr = readl(ioat_chan->reg_base + IOAT_CHANERR_OFFSET);
	pci_read_config_dword(pdev, IOAT_PCI_CHANERR_INT_OFFSET, &chanerr_int);

	dev_dbg(to_dev(ioat_chan), "%s: error = %x:%x\n",
		__func__, chanerr, chanerr_int);

	desc = ioat_get_ring_ent(ioat_chan, ioat_chan->tail);
	hw = desc->hw;
	dump_desc_dbg(ioat_chan, desc);

	switch (hw->ctl_f.op) {
	case IOAT_OP_XOR_VAL:
		if (chanerr & IOAT_CHANERR_XOR_P_OR_CRC_ERR) {
			*desc->result |= SUM_CHECK_P_RESULT;
			err_handled |= IOAT_CHANERR_XOR_P_OR_CRC_ERR;
		}
		break;
	case IOAT_OP_PQ_VAL:
	case IOAT_OP_PQ_VAL_16S:
		if (chanerr & IOAT_CHANERR_XOR_P_OR_CRC_ERR) {
			*desc->result |= SUM_CHECK_P_RESULT;
			err_handled |= IOAT_CHANERR_XOR_P_OR_CRC_ERR;
		}
		if (chanerr & IOAT_CHANERR_XOR_Q_ERR) {
			*desc->result |= SUM_CHECK_Q_RESULT;
			err_handled |= IOAT_CHANERR_XOR_Q_ERR;
		}
		break;
	}

	if (chanerr & IOAT_CHANERR_RECOVER_MASK) {
		if (chanerr & IOAT_CHANERR_READ_DATA_ERR) {
			res.result = DMA_TRANS_READ_FAILED;
			err_handled |= IOAT_CHANERR_READ_DATA_ERR;
		} else if (chanerr & IOAT_CHANERR_WRITE_DATA_ERR) {
			res.result = DMA_TRANS_WRITE_FAILED;
			err_handled |= IOAT_CHANERR_WRITE_DATA_ERR;
		}

		abort = true;
	} else
		res.result = DMA_TRANS_NOERROR;

	/* fault on unhandled error or spurious halt */
	if (chanerr ^ err_handled || chanerr == 0) {
		dev_err(to_dev(ioat_chan), "%s: fatal error (%x:%x)\n",
			__func__, chanerr, err_handled);
		dev_err(to_dev(ioat_chan), "Errors handled:\n");
		ioat_print_chanerrs(ioat_chan, err_handled);
		dev_err(to_dev(ioat_chan), "Errors not handled:\n");
		ioat_print_chanerrs(ioat_chan, (chanerr & ~err_handled));

		BUG();
	}

	/* cleanup the faulty descriptor since we are continuing */
	tx = &desc->txd;
	if (tx->cookie) {
		dma_cookie_complete(tx);
		dma_descriptor_unmap(tx);
		dmaengine_desc_get_callback_invoke(tx, &res);
		tx->callback = NULL;
		tx->callback_result = NULL;
	}

	/* mark faulting descriptor as complete */
	*ioat_chan->completion = desc->txd.phys;

	spin_lock_bh(&ioat_chan->prep_lock);
	/* we need abort all descriptors */
	if (abort) {
		ioat_abort_descs(ioat_chan);
		/* clean up the channel, we could be in weird state */
		ioat_reset_hw(ioat_chan);
	}

	writel(chanerr, ioat_chan->reg_base + IOAT_CHANERR_OFFSET);
	pci_write_config_dword(pdev, IOAT_PCI_CHANERR_INT_OFFSET, chanerr_int);

	ioat_restart_channel(ioat_chan);
	spin_unlock_bh(&ioat_chan->prep_lock);
}

static void check_active(struct ioatdma_chan *ioat_chan)
{
	if (ioat_ring_active(ioat_chan)) {
		mod_timer(&ioat_chan->timer, jiffies + COMPLETION_TIMEOUT);
		return;
	}

	if (test_and_clear_bit(IOAT_CHAN_ACTIVE, &ioat_chan->state))
		mod_timer(&ioat_chan->timer, jiffies + IDLE_TIMEOUT);
}

void ioat_timer_event(unsigned long data)
{
	struct ioatdma_chan *ioat_chan = to_ioat_chan((void *)data);
	dma_addr_t phys_complete;
	u64 status;

	status = ioat_chansts(ioat_chan);

	/* when halted due to errors check for channel
	 * programming errors before advancing the completion state
	 */
	if (is_ioat_halted(status)) {
		u32 chanerr;

		chanerr = readl(ioat_chan->reg_base + IOAT_CHANERR_OFFSET);
		dev_err(to_dev(ioat_chan), "%s: Channel halted (%x)\n",
			__func__, chanerr);
		dev_err(to_dev(ioat_chan), "Errors:\n");
		ioat_print_chanerrs(ioat_chan, chanerr);

		if (test_bit(IOAT_RUN, &ioat_chan->state)) {
			spin_lock_bh(&ioat_chan->cleanup_lock);
			spin_lock_bh(&ioat_chan->prep_lock);
			set_bit(IOAT_CHAN_DOWN, &ioat_chan->state);
			spin_unlock_bh(&ioat_chan->prep_lock);

			ioat_abort_descs(ioat_chan);
			dev_warn(to_dev(ioat_chan), "Reset channel...\n");
			ioat_reset_hw(ioat_chan);
			dev_warn(to_dev(ioat_chan), "Restart channel...\n");
			ioat_restart_channel(ioat_chan);

			spin_lock_bh(&ioat_chan->prep_lock);
			clear_bit(IOAT_CHAN_DOWN, &ioat_chan->state);
			spin_unlock_bh(&ioat_chan->prep_lock);
			spin_unlock_bh(&ioat_chan->cleanup_lock);
		}

		return;
	}

	spin_lock_bh(&ioat_chan->cleanup_lock);

	/* handle the no-actives case */
	if (!ioat_ring_active(ioat_chan)) {
		spin_lock_bh(&ioat_chan->prep_lock);
		check_active(ioat_chan);
		spin_unlock_bh(&ioat_chan->prep_lock);
		spin_unlock_bh(&ioat_chan->cleanup_lock);
		return;
	}

	/* if we haven't made progress and we have already
	 * acknowledged a pending completion once, then be more
	 * forceful with a restart
	 */
	if (ioat_cleanup_preamble(ioat_chan, &phys_complete))
		__cleanup(ioat_chan, phys_complete);
	else if (test_bit(IOAT_COMPLETION_ACK, &ioat_chan->state)) {
		u32 chanerr;

		chanerr = readl(ioat_chan->reg_base + IOAT_CHANERR_OFFSET);
		dev_err(to_dev(ioat_chan), "CHANSTS: %#Lx CHANERR: %#x\n",
			status, chanerr);
		dev_err(to_dev(ioat_chan), "Errors:\n");
		ioat_print_chanerrs(ioat_chan, chanerr);

		dev_dbg(to_dev(ioat_chan), "Active descriptors: %d\n",
			ioat_ring_active(ioat_chan));

		spin_lock_bh(&ioat_chan->prep_lock);
		set_bit(IOAT_CHAN_DOWN, &ioat_chan->state);
		spin_unlock_bh(&ioat_chan->prep_lock);

		ioat_abort_descs(ioat_chan);
		dev_warn(to_dev(ioat_chan), "Resetting channel...\n");
		ioat_reset_hw(ioat_chan);
		dev_warn(to_dev(ioat_chan), "Restarting channel...\n");
		ioat_restart_channel(ioat_chan);

		spin_lock_bh(&ioat_chan->prep_lock);
		clear_bit(IOAT_CHAN_DOWN, &ioat_chan->state);
		spin_unlock_bh(&ioat_chan->prep_lock);
		spin_unlock_bh(&ioat_chan->cleanup_lock);
		return;
	} else
		set_bit(IOAT_COMPLETION_ACK, &ioat_chan->state);

	mod_timer(&ioat_chan->timer, jiffies + COMPLETION_TIMEOUT);
	spin_unlock_bh(&ioat_chan->cleanup_lock);
}

enum dma_status
ioat_tx_status(struct dma_chan *c, dma_cookie_t cookie,
		struct dma_tx_state *txstate)
{
	struct ioatdma_chan *ioat_chan = to_ioat_chan(c);
	enum dma_status ret;

	ret = dma_cookie_status(c, cookie, txstate);
	if (ret == DMA_COMPLETE)
		return ret;

	ioat_cleanup(ioat_chan);

	return dma_cookie_status(c, cookie, txstate);
}

int ioat_reset_hw(struct ioatdma_chan *ioat_chan)
{
	/* throw away whatever the channel was doing and get it
	 * initialized, with ioat3 specific workarounds
	 */
	struct ioatdma_device *ioat_dma = ioat_chan->ioat_dma;
	struct pci_dev *pdev = ioat_dma->pdev;
	u32 chanerr;
	u16 dev_id;
	int err;

	ioat_quiesce(ioat_chan, msecs_to_jiffies(100));

	chanerr = readl(ioat_chan->reg_base + IOAT_CHANERR_OFFSET);
	writel(chanerr, ioat_chan->reg_base + IOAT_CHANERR_OFFSET);

	if (ioat_dma->version < IOAT_VER_3_3) {
		/* clear any pending errors */
		err = pci_read_config_dword(pdev,
				IOAT_PCI_CHANERR_INT_OFFSET, &chanerr);
		if (err) {
			dev_err(&pdev->dev,
				"channel error register unreachable\n");
			return err;
		}
		pci_write_config_dword(pdev,
				IOAT_PCI_CHANERR_INT_OFFSET, chanerr);

		/* Clear DMAUNCERRSTS Cfg-Reg Parity Error status bit
		 * (workaround for spurious config parity error after restart)
		 */
		pci_read_config_word(pdev, IOAT_PCI_DEVICE_ID_OFFSET, &dev_id);
		if (dev_id == PCI_DEVICE_ID_INTEL_IOAT_TBG0) {
			pci_write_config_dword(pdev,
					       IOAT_PCI_DMAUNCERRSTS_OFFSET,
					       0x10);
		}
	}

	if (is_bwd_ioat(pdev) && (ioat_dma->irq_mode == IOAT_MSIX)) {
		ioat_dma->msixtba0 = readq(ioat_dma->reg_base + 0x1000);
		ioat_dma->msixdata0 = readq(ioat_dma->reg_base + 0x1008);
		ioat_dma->msixpba = readq(ioat_dma->reg_base + 0x1800);
	}


	err = ioat_reset_sync(ioat_chan, msecs_to_jiffies(200));
	if (!err) {
		if (is_bwd_ioat(pdev) && (ioat_dma->irq_mode == IOAT_MSIX)) {
			writeq(ioat_dma->msixtba0, ioat_dma->reg_base + 0x1000);
			writeq(ioat_dma->msixdata0, ioat_dma->reg_base + 0x1008);
			writeq(ioat_dma->msixpba, ioat_dma->reg_base + 0x1800);
		}
	}

	if (err)
		dev_err(&pdev->dev, "Failed to reset: %d\n", err);

	return err;
}
>>>>>>> v4.9.227
