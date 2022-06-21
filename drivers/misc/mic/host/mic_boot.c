/*
 * Intel MIC Platform Software Stack (MPSS)
 *
 * Copyright(c) 2013 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 * Intel MIC Host driver.
 *
 */
#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/pci.h>
<<<<<<< HEAD

#include <linux/mic_common.h>
#include <linux/mic_bus.h>
#include "../common/mic_dev.h"
#include "mic_device.h"
#include "mic_smpt.h"
#include "mic_virtio.h"
=======
#include <linux/kmod.h>
#include <linux/mic_common.h>
#include <linux/mic_bus.h>
#include "../bus/scif_bus.h"
#include "../bus/vop_bus.h"
#include "../common/mic_dev.h"
#include "mic_device.h"
#include "mic_smpt.h"

static inline struct mic_device *vpdev_to_mdev(struct device *dev)
{
	return dev_get_drvdata(dev->parent);
}

static dma_addr_t
_mic_dma_map_page(struct device *dev, struct page *page,
		  unsigned long offset, size_t size,
		  enum dma_data_direction dir, unsigned long attrs)
{
	void *va = phys_to_virt(page_to_phys(page)) + offset;
	struct mic_device *mdev = vpdev_to_mdev(dev);

	return mic_map_single(mdev, va, size);
}

static void _mic_dma_unmap_page(struct device *dev, dma_addr_t dma_addr,
				size_t size, enum dma_data_direction dir,
				unsigned long attrs)
{
	struct mic_device *mdev = vpdev_to_mdev(dev);

	mic_unmap_single(mdev, dma_addr, size);
}

static const struct dma_map_ops _mic_dma_ops = {
	.map_page = _mic_dma_map_page,
	.unmap_page = _mic_dma_unmap_page,
};

static struct mic_irq *
__mic_request_irq(struct vop_device *vpdev,
		  irqreturn_t (*func)(int irq, void *data),
		  const char *name, void *data, int intr_src)
{
	struct mic_device *mdev = vpdev_to_mdev(&vpdev->dev);

	return mic_request_threaded_irq(mdev, func, NULL, name, data,
					intr_src, MIC_INTR_DB);
}

static void __mic_free_irq(struct vop_device *vpdev,
			   struct mic_irq *cookie, void *data)
{
	struct mic_device *mdev = vpdev_to_mdev(&vpdev->dev);

	mic_free_irq(mdev, cookie, data);
}

static void __mic_ack_interrupt(struct vop_device *vpdev, int num)
{
	struct mic_device *mdev = vpdev_to_mdev(&vpdev->dev);

	mdev->ops->intr_workarounds(mdev);
}

static int __mic_next_db(struct vop_device *vpdev)
{
	struct mic_device *mdev = vpdev_to_mdev(&vpdev->dev);

	return mic_next_db(mdev);
}

static void *__mic_get_dp(struct vop_device *vpdev)
{
	struct mic_device *mdev = vpdev_to_mdev(&vpdev->dev);

	return mdev->dp;
}

static void __iomem *__mic_get_remote_dp(struct vop_device *vpdev)
{
	return NULL;
}

static void __mic_send_intr(struct vop_device *vpdev, int db)
{
	struct mic_device *mdev = vpdev_to_mdev(&vpdev->dev);

	mdev->ops->send_intr(mdev, db);
}

static void __iomem *__mic_ioremap(struct vop_device *vpdev,
				   dma_addr_t pa, size_t len)
{
	struct mic_device *mdev = vpdev_to_mdev(&vpdev->dev);

	return mdev->aper.va + pa;
}

static void __mic_iounmap(struct vop_device *vpdev, void __iomem *va)
{
	/* nothing to do */
}

static struct vop_hw_ops vop_hw_ops = {
	.request_irq = __mic_request_irq,
	.free_irq = __mic_free_irq,
	.ack_interrupt = __mic_ack_interrupt,
	.next_db = __mic_next_db,
	.get_dp = __mic_get_dp,
	.get_remote_dp = __mic_get_remote_dp,
	.send_intr = __mic_send_intr,
	.ioremap = __mic_ioremap,
	.iounmap = __mic_iounmap,
};

static inline struct mic_device *scdev_to_mdev(struct scif_hw_dev *scdev)
{
	return dev_get_drvdata(scdev->dev.parent);
}

static void *__mic_dma_alloc(struct device *dev, size_t size,
			     dma_addr_t *dma_handle, gfp_t gfp,
			     unsigned long attrs)
{
	struct scif_hw_dev *scdev = dev_get_drvdata(dev);
	struct mic_device *mdev = scdev_to_mdev(scdev);
	dma_addr_t tmp;
	void *va = kmalloc(size, gfp);

	if (va) {
		tmp = mic_map_single(mdev, va, size);
		if (dma_mapping_error(dev, tmp)) {
			kfree(va);
			va = NULL;
		} else {
			*dma_handle = tmp;
		}
	}
	return va;
}

static void __mic_dma_free(struct device *dev, size_t size, void *vaddr,
			   dma_addr_t dma_handle, unsigned long attrs)
{
	struct scif_hw_dev *scdev = dev_get_drvdata(dev);
	struct mic_device *mdev = scdev_to_mdev(scdev);

	mic_unmap_single(mdev, dma_handle, size);
	kfree(vaddr);
}

static dma_addr_t
__mic_dma_map_page(struct device *dev, struct page *page, unsigned long offset,
		   size_t size, enum dma_data_direction dir,
		   unsigned long attrs)
{
	void *va = phys_to_virt(page_to_phys(page)) + offset;
	struct scif_hw_dev *scdev = dev_get_drvdata(dev);
	struct mic_device *mdev = scdev_to_mdev(scdev);

	return mic_map_single(mdev, va, size);
}

static void
__mic_dma_unmap_page(struct device *dev, dma_addr_t dma_addr,
		     size_t size, enum dma_data_direction dir,
		     unsigned long attrs)
{
	struct scif_hw_dev *scdev = dev_get_drvdata(dev);
	struct mic_device *mdev = scdev_to_mdev(scdev);

	mic_unmap_single(mdev, dma_addr, size);
}

static int __mic_dma_map_sg(struct device *dev, struct scatterlist *sg,
			    int nents, enum dma_data_direction dir,
			    unsigned long attrs)
{
	struct scif_hw_dev *scdev = dev_get_drvdata(dev);
	struct mic_device *mdev = scdev_to_mdev(scdev);
	struct scatterlist *s;
	int i, j, ret;
	dma_addr_t da;

	ret = dma_map_sg(&mdev->pdev->dev, sg, nents, dir);
	if (ret <= 0)
		return 0;

	for_each_sg(sg, s, nents, i) {
		da = mic_map(mdev, sg_dma_address(s) + s->offset, s->length);
		if (!da)
			goto err;
		sg_dma_address(s) = da;
	}
	return nents;
err:
	for_each_sg(sg, s, i, j) {
		mic_unmap(mdev, sg_dma_address(s), s->length);
		sg_dma_address(s) = mic_to_dma_addr(mdev, sg_dma_address(s));
	}
	dma_unmap_sg(&mdev->pdev->dev, sg, nents, dir);
	return 0;
}

static void __mic_dma_unmap_sg(struct device *dev,
			       struct scatterlist *sg, int nents,
			       enum dma_data_direction dir,
			       unsigned long attrs)
{
	struct scif_hw_dev *scdev = dev_get_drvdata(dev);
	struct mic_device *mdev = scdev_to_mdev(scdev);
	struct scatterlist *s;
	dma_addr_t da;
	int i;

	for_each_sg(sg, s, nents, i) {
		da = mic_to_dma_addr(mdev, sg_dma_address(s));
		mic_unmap(mdev, sg_dma_address(s), s->length);
		sg_dma_address(s) = da;
	}
	dma_unmap_sg(&mdev->pdev->dev, sg, nents, dir);
}

static struct dma_map_ops __mic_dma_ops = {
	.alloc = __mic_dma_alloc,
	.free = __mic_dma_free,
	.map_page = __mic_dma_map_page,
	.unmap_page = __mic_dma_unmap_page,
	.map_sg = __mic_dma_map_sg,
	.unmap_sg = __mic_dma_unmap_sg,
};

static struct mic_irq *
___mic_request_irq(struct scif_hw_dev *scdev,
		   irqreturn_t (*func)(int irq, void *data),
				       const char *name,
				       void *data, int db)
{
	struct mic_device *mdev = scdev_to_mdev(scdev);

	return mic_request_threaded_irq(mdev, func, NULL, name, data,
					db, MIC_INTR_DB);
}

static void
___mic_free_irq(struct scif_hw_dev *scdev,
		struct mic_irq *cookie, void *data)
{
	struct mic_device *mdev = scdev_to_mdev(scdev);

	mic_free_irq(mdev, cookie, data);
}

static void ___mic_ack_interrupt(struct scif_hw_dev *scdev, int num)
{
	struct mic_device *mdev = scdev_to_mdev(scdev);

	mdev->ops->intr_workarounds(mdev);
}

static int ___mic_next_db(struct scif_hw_dev *scdev)
{
	struct mic_device *mdev = scdev_to_mdev(scdev);

	return mic_next_db(mdev);
}

static void ___mic_send_intr(struct scif_hw_dev *scdev, int db)
{
	struct mic_device *mdev = scdev_to_mdev(scdev);

	mdev->ops->send_intr(mdev, db);
}

static void __iomem *___mic_ioremap(struct scif_hw_dev *scdev,
				    phys_addr_t pa, size_t len)
{
	struct mic_device *mdev = scdev_to_mdev(scdev);

	return mdev->aper.va + pa;
}

static void ___mic_iounmap(struct scif_hw_dev *scdev, void __iomem *va)
{
	/* nothing to do */
}

static struct scif_hw_ops scif_hw_ops = {
	.request_irq = ___mic_request_irq,
	.free_irq = ___mic_free_irq,
	.ack_interrupt = ___mic_ack_interrupt,
	.next_db = ___mic_next_db,
	.send_intr = ___mic_send_intr,
	.ioremap = ___mic_ioremap,
	.iounmap = ___mic_iounmap,
};
>>>>>>> v4.9.227

static inline struct mic_device *mbdev_to_mdev(struct mbus_device *mbdev)
{
	return dev_get_drvdata(mbdev->dev.parent);
}

static dma_addr_t
mic_dma_map_page(struct device *dev, struct page *page,
		 unsigned long offset, size_t size, enum dma_data_direction dir,
<<<<<<< HEAD
		 struct dma_attrs *attrs)
=======
		 unsigned long attrs)
>>>>>>> v4.9.227
{
	void *va = phys_to_virt(page_to_phys(page)) + offset;
	struct mic_device *mdev = dev_get_drvdata(dev->parent);

	return mic_map_single(mdev, va, size);
}

static void
mic_dma_unmap_page(struct device *dev, dma_addr_t dma_addr,
		   size_t size, enum dma_data_direction dir,
<<<<<<< HEAD
		   struct dma_attrs *attrs)
=======
		   unsigned long attrs)
>>>>>>> v4.9.227
{
	struct mic_device *mdev = dev_get_drvdata(dev->parent);
	mic_unmap_single(mdev, dma_addr, size);
}

static struct dma_map_ops mic_dma_ops = {
	.map_page = mic_dma_map_page,
	.unmap_page = mic_dma_unmap_page,
};

static struct mic_irq *
_mic_request_threaded_irq(struct mbus_device *mbdev,
			  irq_handler_t handler, irq_handler_t thread_fn,
			  const char *name, void *data, int intr_src)
{
	return mic_request_threaded_irq(mbdev_to_mdev(mbdev), handler,
					thread_fn, name, data,
					intr_src, MIC_INTR_DMA);
}

static void _mic_free_irq(struct mbus_device *mbdev,
			  struct mic_irq *cookie, void *data)
{
<<<<<<< HEAD
	return mic_free_irq(mbdev_to_mdev(mbdev), cookie, data);
=======
	mic_free_irq(mbdev_to_mdev(mbdev), cookie, data);
>>>>>>> v4.9.227
}

static void _mic_ack_interrupt(struct mbus_device *mbdev, int num)
{
	struct mic_device *mdev = mbdev_to_mdev(mbdev);
	mdev->ops->intr_workarounds(mdev);
}

static struct mbus_hw_ops mbus_hw_ops = {
	.request_threaded_irq = _mic_request_threaded_irq,
	.free_irq = _mic_free_irq,
	.ack_interrupt = _mic_ack_interrupt,
};

<<<<<<< HEAD
/**
 * mic_reset - Reset the MIC device.
 * @mdev: pointer to mic_device instance
 */
static void mic_reset(struct mic_device *mdev)
{
	int i;

#define MIC_RESET_TO (45)

	reinit_completion(&mdev->reset_wait);
	mdev->ops->reset_fw_ready(mdev);
	mdev->ops->reset(mdev);

	for (i = 0; i < MIC_RESET_TO; i++) {
		if (mdev->ops->is_fw_ready(mdev))
			goto done;
		/*
		 * Resets typically take 10s of seconds to complete.
		 * Since an MMIO read is required to check if the
		 * firmware is ready or not, a 1 second delay works nicely.
		 */
		msleep(1000);
	}
	mic_set_state(mdev, MIC_RESET_FAILED);
done:
	complete_all(&mdev->reset_wait);
}

=======
>>>>>>> v4.9.227
/* Initialize the MIC bootparams */
void mic_bootparam_init(struct mic_device *mdev)
{
	struct mic_bootparam *bootparam = mdev->dp;

	bootparam->magic = cpu_to_le32(MIC_MAGIC);
<<<<<<< HEAD
	bootparam->c2h_shutdown_db = mdev->shutdown_db;
	bootparam->h2c_shutdown_db = -1;
	bootparam->h2c_config_db = -1;
	bootparam->shutdown_status = 0;
	bootparam->shutdown_card = 0;
}

/**
 * mic_start - Start the MIC.
 * @mdev: pointer to mic_device instance
 * @buf: buffer containing boot string including firmware/ramdisk path.
 *
 * This function prepares an MIC for boot and initiates boot.
 * RETURNS: An appropriate -ERRNO error value on error, or zero for success.
 */
int mic_start(struct mic_device *mdev, const char *buf)
{
	int rc;
	mutex_lock(&mdev->mic_mutex);
retry:
	if (MIC_OFFLINE != mdev->state) {
		rc = -EINVAL;
		goto unlock_ret;
	}
	if (!mdev->ops->is_fw_ready(mdev)) {
		mic_reset(mdev);
		/*
		 * The state will either be MIC_OFFLINE if the reset succeeded
		 * or MIC_RESET_FAILED if the firmware reset failed.
		 */
		goto retry;
	}
	mdev->dma_mbdev = mbus_register_device(mdev->sdev->parent,
					       MBUS_DEV_DMA_HOST, &mic_dma_ops,
					       &mbus_hw_ops, mdev->mmio.va);
=======
	bootparam->h2c_config_db = -1;
	bootparam->node_id = mdev->id + 1;
	bootparam->scif_host_dma_addr = 0x0;
	bootparam->scif_card_dma_addr = 0x0;
	bootparam->c2h_scif_db = -1;
	bootparam->h2c_scif_db = -1;
}

static inline struct mic_device *cosmdev_to_mdev(struct cosm_device *cdev)
{
	return dev_get_drvdata(cdev->dev.parent);
}

static void _mic_reset(struct cosm_device *cdev)
{
	struct mic_device *mdev = cosmdev_to_mdev(cdev);

	mdev->ops->reset_fw_ready(mdev);
	mdev->ops->reset(mdev);
}

static bool _mic_ready(struct cosm_device *cdev)
{
	struct mic_device *mdev = cosmdev_to_mdev(cdev);

	return mdev->ops->is_fw_ready(mdev);
}

/**
 * mic_request_dma_chans - Request DMA channels
 * @mdev: pointer to mic_device instance
 *
 * returns number of DMA channels acquired
 */
static int mic_request_dma_chans(struct mic_device *mdev)
{
	dma_cap_mask_t mask;
	struct dma_chan *chan;

	dma_cap_zero(mask);
	dma_cap_set(DMA_MEMCPY, mask);

	do {
		chan = dma_request_channel(mask, mdev->ops->dma_filter,
					   &mdev->pdev->dev);
		if (chan) {
			mdev->dma_ch[mdev->num_dma_ch++] = chan;
			if (mdev->num_dma_ch >= MIC_MAX_DMA_CHAN)
				break;
		}
	} while (chan);
	dev_info(&mdev->pdev->dev, "DMA channels # %d\n", mdev->num_dma_ch);
	return mdev->num_dma_ch;
}

/**
 * mic_free_dma_chans - release DMA channels
 * @mdev: pointer to mic_device instance
 *
 * returns none
 */
static void mic_free_dma_chans(struct mic_device *mdev)
{
	int i = 0;

	for (i = 0; i < mdev->num_dma_ch; i++) {
		dma_release_channel(mdev->dma_ch[i]);
		mdev->dma_ch[i] = NULL;
	}
	mdev->num_dma_ch = 0;
}

/**
 * _mic_start - Start the MIC.
 * @cdev: pointer to cosm_device instance
 * @id: MIC device id/index provided by COSM used in other drivers like SCIF
 *
 * This function prepares an MIC for boot and initiates boot.
 * RETURNS: An appropriate -ERRNO error value on error, or zero for success.
 *
 * For all cosm_hw_ops the caller holds a mutex to ensure serialization.
 */
static int _mic_start(struct cosm_device *cdev, int id)
{
	struct mic_device *mdev = cosmdev_to_mdev(cdev);
	int rc;

	mic_bootparam_init(mdev);
	mdev->dma_mbdev = mbus_register_device(&mdev->pdev->dev,
					       MBUS_DEV_DMA_HOST, &mic_dma_ops,
					       &mbus_hw_ops, id, mdev->mmio.va);
>>>>>>> v4.9.227
	if (IS_ERR(mdev->dma_mbdev)) {
		rc = PTR_ERR(mdev->dma_mbdev);
		goto unlock_ret;
	}
<<<<<<< HEAD
	mdev->dma_ch = mic_request_dma_chan(mdev);
	if (!mdev->dma_ch) {
		rc = -ENXIO;
		goto dma_remove;
	}
	rc = mdev->ops->load_mic_fw(mdev, buf);
	if (rc)
		goto dma_release;
=======
	if (!mic_request_dma_chans(mdev)) {
		rc = -ENODEV;
		goto dma_remove;
	}
	mdev->scdev = scif_register_device(&mdev->pdev->dev, MIC_SCIF_DEV,
					   &__mic_dma_ops, &scif_hw_ops,
					   id + 1, 0, &mdev->mmio,
					   &mdev->aper, mdev->dp, NULL,
					   mdev->dma_ch, mdev->num_dma_ch,
					   true);
	if (IS_ERR(mdev->scdev)) {
		rc = PTR_ERR(mdev->scdev);
		goto dma_free;
	}

	mdev->vpdev = vop_register_device(&mdev->pdev->dev,
					  VOP_DEV_TRNSP, &_mic_dma_ops,
					  &vop_hw_ops, id + 1, &mdev->aper,
					  mdev->dma_ch[0]);
	if (IS_ERR(mdev->vpdev)) {
		rc = PTR_ERR(mdev->vpdev);
		goto scif_remove;
	}

	rc = mdev->ops->load_mic_fw(mdev, NULL);
	if (rc)
		goto vop_remove;
>>>>>>> v4.9.227
	mic_smpt_restore(mdev);
	mic_intr_restore(mdev);
	mdev->intr_ops->enable_interrupts(mdev);
	mdev->ops->write_spad(mdev, MIC_DPLO_SPAD, mdev->dp_dma_addr);
	mdev->ops->write_spad(mdev, MIC_DPHI_SPAD, mdev->dp_dma_addr >> 32);
	mdev->ops->send_firmware_intr(mdev);
<<<<<<< HEAD
	mic_set_state(mdev, MIC_ONLINE);
	goto unlock_ret;
dma_release:
	dma_release_channel(mdev->dma_ch);
dma_remove:
	mbus_unregister_device(mdev->dma_mbdev);
unlock_ret:
	mutex_unlock(&mdev->mic_mutex);
=======
	goto unlock_ret;
vop_remove:
	vop_unregister_device(mdev->vpdev);
scif_remove:
	scif_unregister_device(mdev->scdev);
dma_free:
	mic_free_dma_chans(mdev);
dma_remove:
	mbus_unregister_device(mdev->dma_mbdev);
unlock_ret:
>>>>>>> v4.9.227
	return rc;
}

/**
<<<<<<< HEAD
 * mic_stop - Prepare the MIC for reset and trigger reset.
 * @mdev: pointer to mic_device instance
=======
 * _mic_stop - Prepare the MIC for reset and trigger reset.
 * @cdev: pointer to cosm_device instance
>>>>>>> v4.9.227
 * @force: force a MIC to reset even if it is already offline.
 *
 * RETURNS: None.
 */
<<<<<<< HEAD
void mic_stop(struct mic_device *mdev, bool force)
{
	mutex_lock(&mdev->mic_mutex);
	if (MIC_OFFLINE != mdev->state || force) {
		mic_virtio_reset_devices(mdev);
		if (mdev->dma_ch) {
			dma_release_channel(mdev->dma_ch);
			mdev->dma_ch = NULL;
		}
		mbus_unregister_device(mdev->dma_mbdev);
		mic_bootparam_init(mdev);
		mic_reset(mdev);
		if (MIC_RESET_FAILED == mdev->state)
			goto unlock;
		mic_set_shutdown_status(mdev, MIC_NOP);
		if (MIC_SUSPENDED != mdev->state)
			mic_set_state(mdev, MIC_OFFLINE);
	}
unlock:
	mutex_unlock(&mdev->mic_mutex);
}

/**
 * mic_shutdown - Initiate MIC shutdown.
 * @mdev: pointer to mic_device instance
 *
 * RETURNS: None.
 */
void mic_shutdown(struct mic_device *mdev)
{
	struct mic_bootparam *bootparam = mdev->dp;
	s8 db = bootparam->h2c_shutdown_db;

	mutex_lock(&mdev->mic_mutex);
	if (MIC_ONLINE == mdev->state && db != -1) {
		bootparam->shutdown_card = 1;
		mdev->ops->send_intr(mdev, db);
		mic_set_state(mdev, MIC_SHUTTING_DOWN);
	}
	mutex_unlock(&mdev->mic_mutex);
}

/**
 * mic_shutdown_work - Handle shutdown interrupt from MIC.
 * @work: The work structure.
 *
 * This work is scheduled whenever the host has received a shutdown
 * interrupt from the MIC.
 */
void mic_shutdown_work(struct work_struct *work)
{
	struct mic_device *mdev = container_of(work, struct mic_device,
			shutdown_work);
	struct mic_bootparam *bootparam = mdev->dp;

	mutex_lock(&mdev->mic_mutex);
	mic_set_shutdown_status(mdev, bootparam->shutdown_status);
	bootparam->shutdown_status = 0;

	/*
	 * if state is MIC_SUSPENDED, OSPM suspend is in progress. We do not
	 * change the state here so as to prevent users from booting the card
	 * during and after the suspend operation.
	 */
	if (MIC_SHUTTING_DOWN != mdev->state &&
	    MIC_SUSPENDED != mdev->state)
		mic_set_state(mdev, MIC_SHUTTING_DOWN);
	mutex_unlock(&mdev->mic_mutex);
}

/**
 * mic_reset_trigger_work - Trigger MIC reset.
 * @work: The work structure.
 *
 * This work is scheduled whenever the host wants to reset the MIC.
 */
void mic_reset_trigger_work(struct work_struct *work)
{
	struct mic_device *mdev = container_of(work, struct mic_device,
			reset_trigger_work);

	mic_stop(mdev, false);
}

/**
 * mic_complete_resume - Complete MIC Resume after an OSPM suspend/hibernate
 * event.
 * @mdev: pointer to mic_device instance
 *
 * RETURNS: None.
 */
void mic_complete_resume(struct mic_device *mdev)
{
	if (mdev->state != MIC_SUSPENDED) {
		dev_warn(mdev->sdev->parent, "state %d should be %d\n",
			 mdev->state, MIC_SUSPENDED);
		return;
	}

	/* Make sure firmware is ready */
	if (!mdev->ops->is_fw_ready(mdev))
		mic_stop(mdev, true);

	mutex_lock(&mdev->mic_mutex);
	mic_set_state(mdev, MIC_OFFLINE);
	mutex_unlock(&mdev->mic_mutex);
}

/**
 * mic_prepare_suspend - Handle suspend notification for the MIC device.
 * @mdev: pointer to mic_device instance
 *
 * RETURNS: None.
 */
void mic_prepare_suspend(struct mic_device *mdev)
{
	int rc;

#define MIC_SUSPEND_TIMEOUT (60 * HZ)

	mutex_lock(&mdev->mic_mutex);
	switch (mdev->state) {
	case MIC_OFFLINE:
		/*
		 * Card is already offline. Set state to MIC_SUSPENDED
		 * to prevent users from booting the card.
		 */
		mic_set_state(mdev, MIC_SUSPENDED);
		mutex_unlock(&mdev->mic_mutex);
		break;
	case MIC_ONLINE:
		/*
		 * Card is online. Set state to MIC_SUSPENDING and notify
		 * MIC user space daemon which will issue card
		 * shutdown and reset.
		 */
		mic_set_state(mdev, MIC_SUSPENDING);
		mutex_unlock(&mdev->mic_mutex);
		rc = wait_for_completion_timeout(&mdev->reset_wait,
						MIC_SUSPEND_TIMEOUT);
		/* Force reset the card if the shutdown completion timed out */
		if (!rc) {
			mutex_lock(&mdev->mic_mutex);
			mic_set_state(mdev, MIC_SUSPENDED);
			mutex_unlock(&mdev->mic_mutex);
			mic_stop(mdev, true);
		}
		break;
	case MIC_SHUTTING_DOWN:
		/*
		 * Card is shutting down. Set state to MIC_SUSPENDED
		 * to prevent further boot of the card.
		 */
		mic_set_state(mdev, MIC_SUSPENDED);
		mutex_unlock(&mdev->mic_mutex);
		rc = wait_for_completion_timeout(&mdev->reset_wait,
						MIC_SUSPEND_TIMEOUT);
		/* Force reset the card if the shutdown completion timed out */
		if (!rc)
			mic_stop(mdev, true);
		break;
	default:
		mutex_unlock(&mdev->mic_mutex);
		break;
	}
}

/**
 * mic_suspend - Initiate MIC suspend. Suspend merely issues card shutdown.
 * @mdev: pointer to mic_device instance
 *
 * RETURNS: None.
 */
void mic_suspend(struct mic_device *mdev)
{
	struct mic_bootparam *bootparam = mdev->dp;
	s8 db = bootparam->h2c_shutdown_db;

	mutex_lock(&mdev->mic_mutex);
	if (MIC_SUSPENDING == mdev->state && db != -1) {
		bootparam->shutdown_card = 1;
		mdev->ops->send_intr(mdev, db);
		mic_set_state(mdev, MIC_SUSPENDED);
	}
	mutex_unlock(&mdev->mic_mutex);
}
=======
static void _mic_stop(struct cosm_device *cdev, bool force)
{
	struct mic_device *mdev = cosmdev_to_mdev(cdev);

	/*
	 * Since SCIF handles card shutdown and reset (using COSM), it will
	 * will be the first to be registered and the last to be
	 * unregistered.
	 */
	vop_unregister_device(mdev->vpdev);
	scif_unregister_device(mdev->scdev);
	mic_free_dma_chans(mdev);
	mbus_unregister_device(mdev->dma_mbdev);
	mic_bootparam_init(mdev);
}

static ssize_t _mic_family(struct cosm_device *cdev, char *buf)
{
	struct mic_device *mdev = cosmdev_to_mdev(cdev);
	static const char *family[MIC_FAMILY_LAST] = { "x100", "Unknown" };

	return scnprintf(buf, PAGE_SIZE, "%s\n", family[mdev->family]);
}

static ssize_t _mic_stepping(struct cosm_device *cdev, char *buf)
{
	struct mic_device *mdev = cosmdev_to_mdev(cdev);
	const char *string = "??";

	switch (mdev->stepping) {
	case MIC_A0_STEP:
		string = "A0";
		break;
	case MIC_B0_STEP:
		string = "B0";
		break;
	case MIC_B1_STEP:
		string = "B1";
		break;
	case MIC_C0_STEP:
		string = "C0";
		break;
	default:
		break;
	}
	return scnprintf(buf, PAGE_SIZE, "%s\n", string);
}

static struct mic_mw *_mic_aper(struct cosm_device *cdev)
{
	struct mic_device *mdev = cosmdev_to_mdev(cdev);

	return &mdev->aper;
}

struct cosm_hw_ops cosm_hw_ops = {
	.reset = _mic_reset,
	.force_reset = _mic_reset,
	.post_reset = NULL,
	.ready = _mic_ready,
	.start = _mic_start,
	.stop = _mic_stop,
	.family = _mic_family,
	.stepping = _mic_stepping,
	.aper = _mic_aper,
};
>>>>>>> v4.9.227
