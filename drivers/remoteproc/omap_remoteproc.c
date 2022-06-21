/*
 * OMAP Remote Processor driver
 *
 * Copyright (C) 2011 Texas Instruments, Inc.
 * Copyright (C) 2011 Google, Inc.
 *
 * Ohad Ben-Cohen <ohad@wizery.com>
 * Brian Swetland <swetland@google.com>
 * Fernando Guzman Lugo <fernando.lugo@ti.com>
 * Mark Grosen <mgrosen@ti.com>
 * Suman Anna <s-anna@ti.com>
 * Hari Kanigeri <h-kanigeri2@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/remoteproc.h>
<<<<<<< HEAD
=======
#include <linux/mailbox_client.h>
>>>>>>> v4.9.227
#include <linux/omap-mailbox.h>

#include <linux/platform_data/remoteproc-omap.h>

#include "omap_remoteproc.h"
#include "remoteproc_internal.h"

/**
 * struct omap_rproc - omap remote processor state
<<<<<<< HEAD
 * @mbox: omap mailbox handle
 * @nb: notifier block that will be invoked on inbound mailbox messages
 * @rproc: rproc handle
 */
struct omap_rproc {
	struct omap_mbox *mbox;
	struct notifier_block nb;
=======
 * @mbox: mailbox channel handle
 * @client: mailbox client to request the mailbox channel
 * @rproc: rproc handle
 */
struct omap_rproc {
	struct mbox_chan *mbox;
	struct mbox_client client;
>>>>>>> v4.9.227
	struct rproc *rproc;
};

/**
 * omap_rproc_mbox_callback() - inbound mailbox message handler
<<<<<<< HEAD
 * @this: notifier block
 * @index: unused
=======
 * @client: mailbox client pointer used for requesting the mailbox channel
>>>>>>> v4.9.227
 * @data: mailbox payload
 *
 * This handler is invoked by omap's mailbox driver whenever a mailbox
 * message is received. Usually, the mailbox payload simply contains
 * the index of the virtqueue that is kicked by the remote processor,
 * and we let remoteproc core handle it.
 *
 * In addition to virtqueue indices, we also have some out-of-band values
 * that indicates different events. Those values are deliberately very
 * big so they don't coincide with virtqueue indices.
 */
<<<<<<< HEAD
static int omap_rproc_mbox_callback(struct notifier_block *this,
					unsigned long index, void *data)
{
	mbox_msg_t msg = (mbox_msg_t) data;
	struct omap_rproc *oproc = container_of(this, struct omap_rproc, nb);
	struct device *dev = oproc->rproc->dev.parent;
	const char *name = oproc->rproc->name;
=======
static void omap_rproc_mbox_callback(struct mbox_client *client, void *data)
{
	struct omap_rproc *oproc = container_of(client, struct omap_rproc,
						client);
	struct device *dev = oproc->rproc->dev.parent;
	const char *name = oproc->rproc->name;
	u32 msg = (u32)data;
>>>>>>> v4.9.227

	dev_dbg(dev, "mbox msg: 0x%x\n", msg);

	switch (msg) {
	case RP_MBOX_CRASH:
		/* just log this for now. later, we'll also do recovery */
		dev_err(dev, "omap rproc %s crashed\n", name);
		break;
	case RP_MBOX_ECHO_REPLY:
		dev_info(dev, "received echo reply from %s\n", name);
		break;
	default:
		/* msg contains the index of the triggered vring */
		if (rproc_vq_interrupt(oproc->rproc, msg) == IRQ_NONE)
			dev_dbg(dev, "no message was found in vqid %d\n", msg);
	}
<<<<<<< HEAD

	return NOTIFY_DONE;
=======
>>>>>>> v4.9.227
}

/* kick a virtqueue */
static void omap_rproc_kick(struct rproc *rproc, int vqid)
{
	struct omap_rproc *oproc = rproc->priv;
	struct device *dev = rproc->dev.parent;
	int ret;

	/* send the index of the triggered virtqueue in the mailbox payload */
<<<<<<< HEAD
	ret = omap_mbox_msg_send(oproc->mbox, vqid);
	if (ret)
		dev_err(dev, "omap_mbox_msg_send failed: %d\n", ret);
=======
	ret = mbox_send_message(oproc->mbox, (void *)vqid);
	if (ret < 0)
		dev_err(dev, "failed to send mailbox message, status = %d\n",
			ret);
>>>>>>> v4.9.227
}

/*
 * Power up the remote processor.
 *
 * This function will be invoked only after the firmware for this rproc
 * was loaded, parsed successfully, and all of its resource requirements
 * were met.
 */
static int omap_rproc_start(struct rproc *rproc)
{
	struct omap_rproc *oproc = rproc->priv;
	struct device *dev = rproc->dev.parent;
	struct platform_device *pdev = to_platform_device(dev);
	struct omap_rproc_pdata *pdata = pdev->dev.platform_data;
	int ret;
<<<<<<< HEAD
=======
	struct mbox_client *client = &oproc->client;
>>>>>>> v4.9.227

	if (pdata->set_bootaddr)
		pdata->set_bootaddr(rproc->bootaddr);

<<<<<<< HEAD
	oproc->nb.notifier_call = omap_rproc_mbox_callback;

	/* every omap rproc is assigned a mailbox instance for messaging */
	oproc->mbox = omap_mbox_get(pdata->mbox_name, &oproc->nb);
	if (IS_ERR(oproc->mbox)) {
		ret = PTR_ERR(oproc->mbox);
		dev_err(dev, "omap_mbox_get failed: %d\n", ret);
=======
	client->dev = dev;
	client->tx_done = NULL;
	client->rx_callback = omap_rproc_mbox_callback;
	client->tx_block = false;
	client->knows_txdone = false;

	oproc->mbox = omap_mbox_request_channel(client, pdata->mbox_name);
	if (IS_ERR(oproc->mbox)) {
		ret = -EBUSY;
		dev_err(dev, "mbox_request_channel failed: %ld\n",
			PTR_ERR(oproc->mbox));
>>>>>>> v4.9.227
		return ret;
	}

	/*
	 * Ping the remote processor. this is only for sanity-sake;
	 * there is no functional effect whatsoever.
	 *
	 * Note that the reply will _not_ arrive immediately: this message
	 * will wait in the mailbox fifo until the remote processor is booted.
	 */
<<<<<<< HEAD
	ret = omap_mbox_msg_send(oproc->mbox, RP_MBOX_ECHO_REQUEST);
	if (ret) {
		dev_err(dev, "omap_mbox_get failed: %d\n", ret);
=======
	ret = mbox_send_message(oproc->mbox, (void *)RP_MBOX_ECHO_REQUEST);
	if (ret < 0) {
		dev_err(dev, "mbox_send_message failed: %d\n", ret);
>>>>>>> v4.9.227
		goto put_mbox;
	}

	ret = pdata->device_enable(pdev);
	if (ret) {
		dev_err(dev, "omap_device_enable failed: %d\n", ret);
		goto put_mbox;
	}

	return 0;

put_mbox:
<<<<<<< HEAD
	omap_mbox_put(oproc->mbox, &oproc->nb);
=======
	mbox_free_channel(oproc->mbox);
>>>>>>> v4.9.227
	return ret;
}

/* power off the remote processor */
static int omap_rproc_stop(struct rproc *rproc)
{
	struct device *dev = rproc->dev.parent;
	struct platform_device *pdev = to_platform_device(dev);
	struct omap_rproc_pdata *pdata = pdev->dev.platform_data;
	struct omap_rproc *oproc = rproc->priv;
	int ret;

	ret = pdata->device_shutdown(pdev);
	if (ret)
		return ret;

<<<<<<< HEAD
	omap_mbox_put(oproc->mbox, &oproc->nb);
=======
	mbox_free_channel(oproc->mbox);
>>>>>>> v4.9.227

	return 0;
}

static struct rproc_ops omap_rproc_ops = {
	.start		= omap_rproc_start,
	.stop		= omap_rproc_stop,
	.kick		= omap_rproc_kick,
};

static int omap_rproc_probe(struct platform_device *pdev)
{
	struct omap_rproc_pdata *pdata = pdev->dev.platform_data;
	struct omap_rproc *oproc;
	struct rproc *rproc;
	int ret;

	ret = dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(32));
	if (ret) {
		dev_err(&pdev->dev, "dma_set_coherent_mask: %d\n", ret);
		return ret;
	}

	rproc = rproc_alloc(&pdev->dev, pdata->name, &omap_rproc_ops,
<<<<<<< HEAD
				pdata->firmware, sizeof(*oproc));
=======
			    pdata->firmware, sizeof(*oproc));
>>>>>>> v4.9.227
	if (!rproc)
		return -ENOMEM;

	oproc = rproc->priv;
	oproc->rproc = rproc;
<<<<<<< HEAD
=======
	/* All existing OMAP IPU and DSP processors have an MMU */
	rproc->has_iommu = true;
>>>>>>> v4.9.227

	platform_set_drvdata(pdev, rproc);

	ret = rproc_add(rproc);
	if (ret)
		goto free_rproc;

	return 0;

free_rproc:
<<<<<<< HEAD
	rproc_put(rproc);
=======
	rproc_free(rproc);
>>>>>>> v4.9.227
	return ret;
}

static int omap_rproc_remove(struct platform_device *pdev)
{
	struct rproc *rproc = platform_get_drvdata(pdev);

	rproc_del(rproc);
<<<<<<< HEAD
	rproc_put(rproc);
=======
	rproc_free(rproc);
>>>>>>> v4.9.227

	return 0;
}

static struct platform_driver omap_rproc_driver = {
	.probe = omap_rproc_probe,
	.remove = omap_rproc_remove,
	.driver = {
		.name = "omap-rproc",
<<<<<<< HEAD
		.owner = THIS_MODULE,
=======
>>>>>>> v4.9.227
	},
};

module_platform_driver(omap_rproc_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("OMAP Remote Processor control driver");
