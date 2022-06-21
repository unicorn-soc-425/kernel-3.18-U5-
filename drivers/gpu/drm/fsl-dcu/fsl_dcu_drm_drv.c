/*
 * Copyright 2015 Freescale Semiconductor, Inc.
 *
 * Freescale DCU drm device driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/clk.h>
#include <linux/clk-provider.h>
<<<<<<< HEAD
=======
#include <linux/console.h>
>>>>>>> v4.9.227
#include <linux/io.h>
#include <linux/mfd/syscon.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/pm_runtime.h>
#include <linux/regmap.h>

#include <drm/drmP.h>
<<<<<<< HEAD
#include <drm/drm_crtc_helper.h>
=======
#include <drm/drm_atomic_helper.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_fb_cma_helper.h>
>>>>>>> v4.9.227
#include <drm/drm_gem_cma_helper.h>

#include "fsl_dcu_drm_crtc.h"
#include "fsl_dcu_drm_drv.h"
<<<<<<< HEAD
=======
#include "fsl_tcon.h"

static bool fsl_dcu_drm_is_volatile_reg(struct device *dev, unsigned int reg)
{
	if (reg == DCU_INT_STATUS || reg == DCU_UPDATE_MODE)
		return true;

	return false;
}
>>>>>>> v4.9.227

static const struct regmap_config fsl_dcu_regmap_config = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
<<<<<<< HEAD
	.cache_type = REGCACHE_RBTREE,
=======

	.volatile_reg = fsl_dcu_drm_is_volatile_reg,
>>>>>>> v4.9.227
};

static int fsl_dcu_drm_irq_init(struct drm_device *dev)
{
	struct fsl_dcu_drm_device *fsl_dev = dev->dev_private;
<<<<<<< HEAD
	unsigned int value;
=======
>>>>>>> v4.9.227
	int ret;

	ret = drm_irq_install(dev, fsl_dev->irq);
	if (ret < 0)
		dev_err(dev->dev, "failed to install IRQ handler\n");

<<<<<<< HEAD
	ret = regmap_write(fsl_dev->regmap, DCU_INT_STATUS, 0);
	if (ret)
		dev_err(dev->dev, "set DCU_INT_STATUS failed\n");
	ret = regmap_read(fsl_dev->regmap, DCU_INT_MASK, &value);
	if (ret)
		dev_err(dev->dev, "read DCU_INT_MASK failed\n");
	value &= DCU_INT_MASK_VBLANK;
	ret = regmap_write(fsl_dev->regmap, DCU_INT_MASK, value);
	if (ret)
		dev_err(dev->dev, "set DCU_INT_MASK failed\n");
	ret = regmap_write(fsl_dev->regmap, DCU_UPDATE_MODE,
			   DCU_UPDATE_MODE_READREG);
	if (ret)
		dev_err(dev->dev, "set DCU_UPDATE_MODE failed\n");
=======
	regmap_write(fsl_dev->regmap, DCU_INT_STATUS, 0);
	regmap_write(fsl_dev->regmap, DCU_INT_MASK, ~0);
>>>>>>> v4.9.227

	return ret;
}

<<<<<<< HEAD
static int fsl_dcu_load(struct drm_device *drm, unsigned long flags)
{
	struct device *dev = drm->dev;
	struct fsl_dcu_drm_device *fsl_dev = drm->dev_private;
=======
static int fsl_dcu_load(struct drm_device *dev, unsigned long flags)
{
	struct fsl_dcu_drm_device *fsl_dev = dev->dev_private;
>>>>>>> v4.9.227
	int ret;

	ret = fsl_dcu_drm_modeset_init(fsl_dev);
	if (ret < 0) {
<<<<<<< HEAD
		dev_err(dev, "failed to initialize mode setting\n");
		return ret;
	}

	ret = drm_vblank_init(drm, drm->mode_config.num_crtc);
	if (ret < 0) {
		dev_err(dev, "failed to initialize vblank\n");
		goto done;
	}
	drm->vblank_disable_allowed = true;

	ret = fsl_dcu_drm_irq_init(drm);
	if (ret < 0)
		goto done;
	drm->irq_enabled = true;

	fsl_dcu_fbdev_init(drm);

	return 0;
done:
	if (ret) {
		drm_mode_config_cleanup(drm);
		drm_vblank_cleanup(drm);
		drm_irq_uninstall(drm);
		drm->dev_private = NULL;
	}
=======
		dev_err(dev->dev, "failed to initialize mode setting\n");
		return ret;
	}

	ret = drm_vblank_init(dev, dev->mode_config.num_crtc);
	if (ret < 0) {
		dev_err(dev->dev, "failed to initialize vblank\n");
		goto done;
	}

	ret = fsl_dcu_drm_irq_init(dev);
	if (ret < 0)
		goto done;
	dev->irq_enabled = true;

	fsl_dcu_fbdev_init(dev);

	return 0;
done:
	drm_kms_helper_poll_fini(dev);

	if (fsl_dev->fbdev)
		drm_fbdev_cma_fini(fsl_dev->fbdev);

	drm_mode_config_cleanup(dev);
	drm_vblank_cleanup(dev);
	drm_irq_uninstall(dev);
	dev->dev_private = NULL;
>>>>>>> v4.9.227

	return ret;
}

static int fsl_dcu_unload(struct drm_device *dev)
{
<<<<<<< HEAD
=======
	struct fsl_dcu_drm_device *fsl_dev = dev->dev_private;

	drm_kms_helper_poll_fini(dev);

	if (fsl_dev->fbdev)
		drm_fbdev_cma_fini(fsl_dev->fbdev);

>>>>>>> v4.9.227
	drm_mode_config_cleanup(dev);
	drm_vblank_cleanup(dev);
	drm_irq_uninstall(dev);

	dev->dev_private = NULL;

	return 0;
}

<<<<<<< HEAD
static void fsl_dcu_drm_preclose(struct drm_device *dev, struct drm_file *file)
{
}

=======
>>>>>>> v4.9.227
static irqreturn_t fsl_dcu_drm_irq(int irq, void *arg)
{
	struct drm_device *dev = arg;
	struct fsl_dcu_drm_device *fsl_dev = dev->dev_private;
	unsigned int int_status;
	int ret;

	ret = regmap_read(fsl_dev->regmap, DCU_INT_STATUS, &int_status);
<<<<<<< HEAD
	if (ret)
		dev_err(dev->dev, "set DCU_INT_STATUS failed\n");
	if (int_status & DCU_INT_STATUS_VBLANK)
		drm_handle_vblank(dev, 0);

	ret = regmap_write(fsl_dev->regmap, DCU_INT_STATUS, 0xffffffff);
	if (ret)
		dev_err(dev->dev, "set DCU_INT_STATUS failed\n");
	ret = regmap_write(fsl_dev->regmap, DCU_UPDATE_MODE,
			   DCU_UPDATE_MODE_READREG);
	if (ret)
		dev_err(dev->dev, "set DCU_UPDATE_MODE failed\n");
=======
	if (ret) {
		dev_err(dev->dev, "read DCU_INT_STATUS failed\n");
		return IRQ_NONE;
	}

	if (int_status & DCU_INT_STATUS_VBLANK)
		drm_handle_vblank(dev, 0);

	regmap_write(fsl_dev->regmap, DCU_INT_STATUS, int_status);
>>>>>>> v4.9.227

	return IRQ_HANDLED;
}

static int fsl_dcu_drm_enable_vblank(struct drm_device *dev, unsigned int pipe)
{
	struct fsl_dcu_drm_device *fsl_dev = dev->dev_private;
	unsigned int value;
<<<<<<< HEAD
	int ret;

	ret = regmap_read(fsl_dev->regmap, DCU_INT_MASK, &value);
	if (ret)
		dev_err(dev->dev, "read DCU_INT_MASK failed\n");
	value &= ~DCU_INT_MASK_VBLANK;
	ret = regmap_write(fsl_dev->regmap, DCU_INT_MASK, value);
	if (ret)
		dev_err(dev->dev, "set DCU_INT_MASK failed\n");
=======

	regmap_read(fsl_dev->regmap, DCU_INT_MASK, &value);
	value &= ~DCU_INT_MASK_VBLANK;
	regmap_write(fsl_dev->regmap, DCU_INT_MASK, value);

>>>>>>> v4.9.227
	return 0;
}

static void fsl_dcu_drm_disable_vblank(struct drm_device *dev,
				       unsigned int pipe)
{
	struct fsl_dcu_drm_device *fsl_dev = dev->dev_private;
	unsigned int value;
<<<<<<< HEAD
	int ret;

	ret = regmap_read(fsl_dev->regmap, DCU_INT_MASK, &value);
	if (ret)
		dev_err(dev->dev, "read DCU_INT_MASK failed\n");
	value |= DCU_INT_MASK_VBLANK;
	ret = regmap_write(fsl_dev->regmap, DCU_INT_MASK, value);
	if (ret)
		dev_err(dev->dev, "set DCU_INT_MASK failed\n");
=======

	regmap_read(fsl_dev->regmap, DCU_INT_MASK, &value);
	value |= DCU_INT_MASK_VBLANK;
	regmap_write(fsl_dev->regmap, DCU_INT_MASK, value);
}

static void fsl_dcu_drm_lastclose(struct drm_device *dev)
{
	struct fsl_dcu_drm_device *fsl_dev = dev->dev_private;

	drm_fbdev_cma_restore_mode(fsl_dev->fbdev);
>>>>>>> v4.9.227
}

static const struct file_operations fsl_dcu_drm_fops = {
	.owner		= THIS_MODULE,
	.open		= drm_open,
	.release	= drm_release,
	.unlocked_ioctl	= drm_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= drm_compat_ioctl,
#endif
	.poll		= drm_poll,
	.read		= drm_read,
	.llseek		= no_llseek,
	.mmap		= drm_gem_cma_mmap,
};

static struct drm_driver fsl_dcu_drm_driver = {
	.driver_features	= DRIVER_HAVE_IRQ | DRIVER_GEM | DRIVER_MODESET
				| DRIVER_PRIME | DRIVER_ATOMIC,
<<<<<<< HEAD
	.load			= fsl_dcu_load,
	.unload			= fsl_dcu_unload,
	.preclose		= fsl_dcu_drm_preclose,
=======
	.lastclose		= fsl_dcu_drm_lastclose,
	.load			= fsl_dcu_load,
	.unload			= fsl_dcu_unload,
>>>>>>> v4.9.227
	.irq_handler		= fsl_dcu_drm_irq,
	.get_vblank_counter	= drm_vblank_no_hw_counter,
	.enable_vblank		= fsl_dcu_drm_enable_vblank,
	.disable_vblank		= fsl_dcu_drm_disable_vblank,
<<<<<<< HEAD
	.gem_free_object	= drm_gem_cma_free_object,
=======
	.gem_free_object_unlocked = drm_gem_cma_free_object,
>>>>>>> v4.9.227
	.gem_vm_ops		= &drm_gem_cma_vm_ops,
	.prime_handle_to_fd	= drm_gem_prime_handle_to_fd,
	.prime_fd_to_handle	= drm_gem_prime_fd_to_handle,
	.gem_prime_import	= drm_gem_prime_import,
	.gem_prime_export	= drm_gem_prime_export,
	.gem_prime_get_sg_table	= drm_gem_cma_prime_get_sg_table,
	.gem_prime_import_sg_table = drm_gem_cma_prime_import_sg_table,
	.gem_prime_vmap		= drm_gem_cma_prime_vmap,
	.gem_prime_vunmap	= drm_gem_cma_prime_vunmap,
	.gem_prime_mmap		= drm_gem_cma_prime_mmap,
	.dumb_create		= drm_gem_cma_dumb_create,
	.dumb_map_offset	= drm_gem_cma_dumb_map_offset,
	.dumb_destroy		= drm_gem_dumb_destroy,
	.fops			= &fsl_dcu_drm_fops,
	.name			= "fsl-dcu-drm",
	.desc			= "Freescale DCU DRM",
<<<<<<< HEAD
	.date			= "20150213",
	.major			= 1,
	.minor			= 0,
=======
	.date			= "20160425",
	.major			= 1,
	.minor			= 1,
>>>>>>> v4.9.227
};

#ifdef CONFIG_PM_SLEEP
static int fsl_dcu_drm_pm_suspend(struct device *dev)
{
	struct fsl_dcu_drm_device *fsl_dev = dev_get_drvdata(dev);

	if (!fsl_dev)
		return 0;

<<<<<<< HEAD
	drm_kms_helper_poll_disable(fsl_dev->drm);
	regcache_cache_only(fsl_dev->regmap, true);
	regcache_mark_dirty(fsl_dev->regmap);
	clk_disable(fsl_dev->clk);
	clk_unprepare(fsl_dev->clk);
=======
	disable_irq(fsl_dev->irq);
	drm_kms_helper_poll_disable(fsl_dev->drm);

	console_lock();
	drm_fbdev_cma_set_suspend(fsl_dev->fbdev, 1);
	console_unlock();

	fsl_dev->state = drm_atomic_helper_suspend(fsl_dev->drm);
	if (IS_ERR(fsl_dev->state)) {
		console_lock();
		drm_fbdev_cma_set_suspend(fsl_dev->fbdev, 0);
		console_unlock();

		drm_kms_helper_poll_enable(fsl_dev->drm);
		enable_irq(fsl_dev->irq);
		return PTR_ERR(fsl_dev->state);
	}

	clk_disable_unprepare(fsl_dev->clk);
>>>>>>> v4.9.227

	return 0;
}

static int fsl_dcu_drm_pm_resume(struct device *dev)
{
	struct fsl_dcu_drm_device *fsl_dev = dev_get_drvdata(dev);
	int ret;

	if (!fsl_dev)
		return 0;

<<<<<<< HEAD
	ret = clk_enable(fsl_dev->clk);
	if (ret < 0) {
		dev_err(dev, "failed to enable dcu clk\n");
		clk_unprepare(fsl_dev->clk);
		return ret;
	}
	ret = clk_prepare(fsl_dev->clk);
	if (ret < 0) {
		dev_err(dev, "failed to prepare dcu clk\n");
		return ret;
	}

	drm_kms_helper_poll_enable(fsl_dev->drm);
	regcache_cache_only(fsl_dev->regmap, false);
	regcache_sync(fsl_dev->regmap);
=======
	ret = clk_prepare_enable(fsl_dev->clk);
	if (ret < 0) {
		dev_err(dev, "failed to enable dcu clk\n");
		return ret;
	}

	if (fsl_dev->tcon)
		fsl_tcon_bypass_enable(fsl_dev->tcon);
	fsl_dcu_drm_init_planes(fsl_dev->drm);
	enable_irq(fsl_dev->irq);
	drm_atomic_helper_resume(fsl_dev->drm, fsl_dev->state);

	console_lock();
	drm_fbdev_cma_set_suspend(fsl_dev->fbdev, 0);
	console_unlock();

	drm_kms_helper_poll_enable(fsl_dev->drm);
>>>>>>> v4.9.227

	return 0;
}
#endif

static const struct dev_pm_ops fsl_dcu_drm_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(fsl_dcu_drm_pm_suspend, fsl_dcu_drm_pm_resume)
};

static const struct fsl_dcu_soc_data fsl_dcu_ls1021a_data = {
	.name = "ls1021a",
	.total_layer = 16,
	.max_layer = 4,
<<<<<<< HEAD
=======
	.layer_regs = LS1021A_LAYER_REG_NUM,
>>>>>>> v4.9.227
};

static const struct fsl_dcu_soc_data fsl_dcu_vf610_data = {
	.name = "vf610",
	.total_layer = 64,
	.max_layer = 6,
<<<<<<< HEAD
=======
	.layer_regs = VF610_LAYER_REG_NUM,
>>>>>>> v4.9.227
};

static const struct of_device_id fsl_dcu_of_match[] = {
	{
		.compatible = "fsl,ls1021a-dcu",
		.data = &fsl_dcu_ls1021a_data,
	}, {
		.compatible = "fsl,vf610-dcu",
		.data = &fsl_dcu_vf610_data,
	}, {
	},
};
MODULE_DEVICE_TABLE(of, fsl_dcu_of_match);

static int fsl_dcu_drm_probe(struct platform_device *pdev)
{
	struct fsl_dcu_drm_device *fsl_dev;
	struct drm_device *drm;
	struct device *dev = &pdev->dev;
	struct resource *res;
	void __iomem *base;
	struct drm_driver *driver = &fsl_dcu_drm_driver;
<<<<<<< HEAD
	const struct of_device_id *id;
	int ret;
=======
	struct clk *pix_clk_in;
	char pix_clk_name[32];
	const char *pix_clk_in_name;
	const struct of_device_id *id;
	int ret;
	u8 div_ratio_shift = 0;
>>>>>>> v4.9.227

	fsl_dev = devm_kzalloc(dev, sizeof(*fsl_dev), GFP_KERNEL);
	if (!fsl_dev)
		return -ENOMEM;

<<<<<<< HEAD
=======
	id = of_match_node(fsl_dcu_of_match, pdev->dev.of_node);
	if (!id)
		return -ENODEV;
	fsl_dev->soc = id->data;

>>>>>>> v4.9.227
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "could not get memory IO resource\n");
		return -ENODEV;
	}

	base = devm_ioremap_resource(dev, res);
	if (IS_ERR(base)) {
		ret = PTR_ERR(base);
		return ret;
	}

	fsl_dev->irq = platform_get_irq(pdev, 0);
	if (fsl_dev->irq < 0) {
		dev_err(dev, "failed to get irq\n");
		return -ENXIO;
	}

<<<<<<< HEAD
	fsl_dev->clk = devm_clk_get(dev, "dcu");
	if (IS_ERR(fsl_dev->clk)) {
		ret = PTR_ERR(fsl_dev->clk);
		dev_err(dev, "failed to get dcu clock\n");
		return ret;
	}
	ret = clk_prepare(fsl_dev->clk);
	if (ret < 0) {
		dev_err(dev, "failed to prepare dcu clk\n");
		return ret;
	}
	ret = clk_enable(fsl_dev->clk);
	if (ret < 0) {
		dev_err(dev, "failed to enable dcu clk\n");
		clk_unprepare(fsl_dev->clk);
		return ret;
	}

=======
>>>>>>> v4.9.227
	fsl_dev->regmap = devm_regmap_init_mmio(dev, base,
			&fsl_dcu_regmap_config);
	if (IS_ERR(fsl_dev->regmap)) {
		dev_err(dev, "regmap init failed\n");
		return PTR_ERR(fsl_dev->regmap);
	}

<<<<<<< HEAD
	id = of_match_node(fsl_dcu_of_match, pdev->dev.of_node);
	if (!id)
		return -ENODEV;
	fsl_dev->soc = id->data;

	drm = drm_dev_alloc(driver, dev);
	if (!drm)
		return -ENOMEM;
=======
	fsl_dev->clk = devm_clk_get(dev, "dcu");
	if (IS_ERR(fsl_dev->clk)) {
		dev_err(dev, "failed to get dcu clock\n");
		return PTR_ERR(fsl_dev->clk);
	}
	ret = clk_prepare_enable(fsl_dev->clk);
	if (ret < 0) {
		dev_err(dev, "failed to enable dcu clk\n");
		return ret;
	}

	pix_clk_in = devm_clk_get(dev, "pix");
	if (IS_ERR(pix_clk_in)) {
		/* legancy binding, use dcu clock as pixel clock input */
		pix_clk_in = fsl_dev->clk;
	}

	if (of_property_read_bool(dev->of_node, "big-endian"))
		div_ratio_shift = 24;

	pix_clk_in_name = __clk_get_name(pix_clk_in);
	snprintf(pix_clk_name, sizeof(pix_clk_name), "%s_pix", pix_clk_in_name);
	fsl_dev->pix_clk = clk_register_divider(dev, pix_clk_name,
			pix_clk_in_name, 0, base + DCU_DIV_RATIO,
			div_ratio_shift, 8, CLK_DIVIDER_ROUND_CLOSEST, NULL);
	if (IS_ERR(fsl_dev->pix_clk)) {
		dev_err(dev, "failed to register pix clk\n");
		ret = PTR_ERR(fsl_dev->pix_clk);
		goto disable_clk;
	}

	fsl_dev->tcon = fsl_tcon_init(dev);

	drm = drm_dev_alloc(driver, dev);
	if (IS_ERR(drm)) {
		ret = PTR_ERR(drm);
		goto unregister_pix_clk;
	}
>>>>>>> v4.9.227

	fsl_dev->dev = dev;
	fsl_dev->drm = drm;
	fsl_dev->np = dev->of_node;
	drm->dev_private = fsl_dev;
	dev_set_drvdata(dev, fsl_dev);
<<<<<<< HEAD
	drm_dev_set_unique(drm, dev_name(dev));
=======
>>>>>>> v4.9.227

	ret = drm_dev_register(drm, 0);
	if (ret < 0)
		goto unref;

	DRM_INFO("Initialized %s %d.%d.%d %s on minor %d\n", driver->name,
		 driver->major, driver->minor, driver->patchlevel,
		 driver->date, drm->primary->index);

	return 0;

unref:
	drm_dev_unref(drm);
<<<<<<< HEAD
=======
unregister_pix_clk:
	clk_unregister(fsl_dev->pix_clk);
disable_clk:
	clk_disable_unprepare(fsl_dev->clk);
>>>>>>> v4.9.227
	return ret;
}

static int fsl_dcu_drm_remove(struct platform_device *pdev)
{
	struct fsl_dcu_drm_device *fsl_dev = platform_get_drvdata(pdev);

<<<<<<< HEAD
=======
	clk_disable_unprepare(fsl_dev->clk);
	clk_unregister(fsl_dev->pix_clk);
>>>>>>> v4.9.227
	drm_put_dev(fsl_dev->drm);

	return 0;
}

static struct platform_driver fsl_dcu_drm_platform_driver = {
	.probe		= fsl_dcu_drm_probe,
	.remove		= fsl_dcu_drm_remove,
	.driver		= {
		.name	= "fsl-dcu",
		.pm	= &fsl_dcu_drm_pm_ops,
		.of_match_table = fsl_dcu_of_match,
	},
};

module_platform_driver(fsl_dcu_drm_platform_driver);

MODULE_DESCRIPTION("Freescale DCU DRM Driver");
MODULE_LICENSE("GPL");
