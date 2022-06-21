/* exynos_drm_iommu.c
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 * Author: Inki Dae <inki.dae@samsung.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

<<<<<<< HEAD
#include <drmP.h>
=======
#include <drm/drmP.h>
>>>>>>> v4.9.227
#include <drm/exynos_drm.h>

#include <linux/dma-mapping.h>
#include <linux/iommu.h>
<<<<<<< HEAD
#include <linux/kref.h>

#include <asm/dma-iommu.h>
=======
>>>>>>> v4.9.227

#include "exynos_drm_drv.h"
#include "exynos_drm_iommu.h"

<<<<<<< HEAD
=======
static inline int configure_dma_max_seg_size(struct device *dev)
{
	if (!dev->dma_parms)
		dev->dma_parms = kzalloc(sizeof(*dev->dma_parms), GFP_KERNEL);
	if (!dev->dma_parms)
		return -ENOMEM;

	dma_set_max_seg_size(dev, DMA_BIT_MASK(32));
	return 0;
}

static inline void clear_dma_max_seg_size(struct device *dev)
{
	kfree(dev->dma_parms);
	dev->dma_parms = NULL;
}

>>>>>>> v4.9.227
/*
 * drm_create_iommu_mapping - create a mapping structure
 *
 * @drm_dev: DRM device
 */
int drm_create_iommu_mapping(struct drm_device *drm_dev)
{
<<<<<<< HEAD
	struct dma_iommu_mapping *mapping = NULL;
	struct exynos_drm_private *priv = drm_dev->dev_private;
	struct device *dev = drm_dev->dev;

	if (!priv->da_start)
		priv->da_start = EXYNOS_DEV_ADDR_START;
	if (!priv->da_space_size)
		priv->da_space_size = EXYNOS_DEV_ADDR_SIZE;

	mapping = arm_iommu_create_mapping(&platform_bus_type, priv->da_start,
						priv->da_space_size);

	if (IS_ERR(mapping))
		return PTR_ERR(mapping);

	dev->dma_parms = devm_kzalloc(dev, sizeof(*dev->dma_parms),
					GFP_KERNEL);
	if (!dev->dma_parms)
		goto error;

	dma_set_max_seg_size(dev, 0xffffffffu);
	dev->archdata.mapping = mapping;

	return 0;
error:
	arm_iommu_release_mapping(mapping);
	return -ENOMEM;
=======
	struct exynos_drm_private *priv = drm_dev->dev_private;

	return __exynos_iommu_create_mapping(priv, EXYNOS_DEV_ADDR_START,
					     EXYNOS_DEV_ADDR_SIZE);
>>>>>>> v4.9.227
}

/*
 * drm_release_iommu_mapping - release iommu mapping structure
 *
 * @drm_dev: DRM device
<<<<<<< HEAD
 *
 * if mapping->kref becomes 0 then all things related to iommu mapping
 * will be released
 */
void drm_release_iommu_mapping(struct drm_device *drm_dev)
{
	struct device *dev = drm_dev->dev;

	arm_iommu_release_mapping(dev->archdata.mapping);
=======
 */
void drm_release_iommu_mapping(struct drm_device *drm_dev)
{
	struct exynos_drm_private *priv = drm_dev->dev_private;

	__exynos_iommu_release_mapping(priv);
>>>>>>> v4.9.227
}

/*
 * drm_iommu_attach_device- attach device to iommu mapping
 *
 * @drm_dev: DRM device
 * @subdrv_dev: device to be attach
 *
 * This function should be called by sub drivers to attach it to iommu
 * mapping.
 */
int drm_iommu_attach_device(struct drm_device *drm_dev,
				struct device *subdrv_dev)
{
<<<<<<< HEAD
	struct device *dev = drm_dev->dev;
	int ret;

	if (!dev->archdata.mapping)
		return 0;

	subdrv_dev->dma_parms = devm_kzalloc(subdrv_dev,
					sizeof(*subdrv_dev->dma_parms),
					GFP_KERNEL);
	if (!subdrv_dev->dma_parms)
		return -ENOMEM;

	dma_set_max_seg_size(subdrv_dev, 0xffffffffu);

	if (subdrv_dev->archdata.mapping)
		arm_iommu_detach_device(subdrv_dev);

	ret = arm_iommu_attach_device(subdrv_dev, dev->archdata.mapping);
	if (ret < 0) {
		DRM_DEBUG_KMS("failed iommu attach.\n");
		return ret;
	}

	/*
	 * Set dma_ops to drm_device just one time.
	 *
	 * The dma mapping api needs device object and the api is used
	 * to allocate physial memory and map it with iommu table.
	 * If iommu attach succeeded, the sub driver would have dma_ops
	 * for iommu and also all sub drivers have same dma_ops.
	 */
	if (get_dma_ops(dev) == get_dma_ops(NULL))
		set_dma_ops(dev, get_dma_ops(subdrv_dev));
=======
	struct exynos_drm_private *priv = drm_dev->dev_private;
	int ret;

	if (get_dma_ops(priv->dma_dev) != get_dma_ops(subdrv_dev)) {
		DRM_ERROR("Device %s lacks support for IOMMU\n",
			  dev_name(subdrv_dev));
		return -EINVAL;
	}

	ret = configure_dma_max_seg_size(subdrv_dev);
	if (ret)
		return ret;

	ret = __exynos_iommu_attach(priv, subdrv_dev);
	if (ret)
		clear_dma_max_seg_size(subdrv_dev);
>>>>>>> v4.9.227

	return 0;
}

/*
 * drm_iommu_detach_device -detach device address space mapping from device
 *
 * @drm_dev: DRM device
 * @subdrv_dev: device to be detached
 *
 * This function should be called by sub drivers to detach it from iommu
 * mapping
 */
void drm_iommu_detach_device(struct drm_device *drm_dev,
				struct device *subdrv_dev)
{
<<<<<<< HEAD
	struct device *dev = drm_dev->dev;
	struct dma_iommu_mapping *mapping = dev->archdata.mapping;

	if (!mapping || !mapping->domain)
		return;

	arm_iommu_detach_device(subdrv_dev);
=======
	struct exynos_drm_private *priv = drm_dev->dev_private;

	__exynos_iommu_detach(priv, subdrv_dev);
	clear_dma_max_seg_size(subdrv_dev);
>>>>>>> v4.9.227
}
