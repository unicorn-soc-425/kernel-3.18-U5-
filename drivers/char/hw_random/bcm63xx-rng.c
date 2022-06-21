/*
 * Broadcom BCM63xx Random Number Generator support
 *
 * Copyright (C) 2011, Florian Fainelli <florian@openwrt.org>
 * Copyright (C) 2009, Broadcom Corporation
 *
 */
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/hw_random.h>
<<<<<<< HEAD

#include <bcm63xx_io.h>
#include <bcm63xx_regs.h>

struct bcm63xx_rng_priv {
=======
#include <linux/of.h>

#define RNG_CTRL			0x00
#define RNG_EN				(1 << 0)

#define RNG_STAT			0x04
#define RNG_AVAIL_MASK			(0xff000000)

#define RNG_DATA			0x08
#define RNG_THRES			0x0c
#define RNG_MASK			0x10

struct bcm63xx_rng_priv {
	struct hwrng rng;
>>>>>>> v4.9.227
	struct clk *clk;
	void __iomem *regs;
};

<<<<<<< HEAD
#define to_rng_priv(rng)	((struct bcm63xx_rng_priv *)rng->priv)
=======
#define to_rng_priv(rng)	container_of(rng, struct bcm63xx_rng_priv, rng)
>>>>>>> v4.9.227

static int bcm63xx_rng_init(struct hwrng *rng)
{
	struct bcm63xx_rng_priv *priv = to_rng_priv(rng);
	u32 val;
<<<<<<< HEAD

	val = bcm_readl(priv->regs + RNG_CTRL);
	val |= RNG_EN;
	bcm_writel(val, priv->regs + RNG_CTRL);
=======
	int error;

	error = clk_prepare_enable(priv->clk);
	if (error)
		return error;

	val = __raw_readl(priv->regs + RNG_CTRL);
	val |= RNG_EN;
	__raw_writel(val, priv->regs + RNG_CTRL);
>>>>>>> v4.9.227

	return 0;
}

static void bcm63xx_rng_cleanup(struct hwrng *rng)
{
	struct bcm63xx_rng_priv *priv = to_rng_priv(rng);
	u32 val;

<<<<<<< HEAD
	val = bcm_readl(priv->regs + RNG_CTRL);
	val &= ~RNG_EN;
	bcm_writel(val, priv->regs + RNG_CTRL);
=======
	val = __raw_readl(priv->regs + RNG_CTRL);
	val &= ~RNG_EN;
	__raw_writel(val, priv->regs + RNG_CTRL);

	clk_disable_unprepare(priv->clk);
>>>>>>> v4.9.227
}

static int bcm63xx_rng_data_present(struct hwrng *rng, int wait)
{
	struct bcm63xx_rng_priv *priv = to_rng_priv(rng);

<<<<<<< HEAD
	return bcm_readl(priv->regs + RNG_STAT) & RNG_AVAIL_MASK;
=======
	return __raw_readl(priv->regs + RNG_STAT) & RNG_AVAIL_MASK;
>>>>>>> v4.9.227
}

static int bcm63xx_rng_data_read(struct hwrng *rng, u32 *data)
{
	struct bcm63xx_rng_priv *priv = to_rng_priv(rng);

<<<<<<< HEAD
	*data = bcm_readl(priv->regs + RNG_DATA);
=======
	*data = __raw_readl(priv->regs + RNG_DATA);
>>>>>>> v4.9.227

	return 4;
}

static int bcm63xx_rng_probe(struct platform_device *pdev)
{
	struct resource *r;
<<<<<<< HEAD
	struct clk *clk;
	int ret;
	struct bcm63xx_rng_priv *priv;
	struct hwrng *rng;
=======
	int ret;
	struct bcm63xx_rng_priv *priv;
>>>>>>> v4.9.227

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r) {
		dev_err(&pdev->dev, "no iomem resource\n");
<<<<<<< HEAD
		ret = -ENXIO;
		goto out;
	}

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&pdev->dev, "no memory for private structure\n");
		ret = -ENOMEM;
		goto out;
	}

	rng = kzalloc(sizeof(*rng), GFP_KERNEL);
	if (!rng) {
		dev_err(&pdev->dev, "no memory for rng structure\n");
		ret = -ENOMEM;
		goto out_free_priv;
	}

	platform_set_drvdata(pdev, rng);
	rng->priv = (unsigned long)priv;
	rng->name = pdev->name;
	rng->init = bcm63xx_rng_init;
	rng->cleanup = bcm63xx_rng_cleanup;
	rng->data_present = bcm63xx_rng_data_present;
	rng->data_read = bcm63xx_rng_data_read;

	clk = clk_get(&pdev->dev, "ipsec");
	if (IS_ERR(clk)) {
		dev_err(&pdev->dev, "no clock for device\n");
		ret = PTR_ERR(clk);
		goto out_free_rng;
	}

	priv->clk = clk;

	if (!devm_request_mem_region(&pdev->dev, r->start,
					resource_size(r), pdev->name)) {
		dev_err(&pdev->dev, "request mem failed");
		ret = -ENOMEM;
		goto out_free_rng;
=======
		return -ENXIO;
	}

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->rng.name = pdev->name;
	priv->rng.init = bcm63xx_rng_init;
	priv->rng.cleanup = bcm63xx_rng_cleanup;
	priv->rng.data_present = bcm63xx_rng_data_present;
	priv->rng.data_read = bcm63xx_rng_data_read;

	priv->clk = devm_clk_get(&pdev->dev, "ipsec");
	if (IS_ERR(priv->clk)) {
		ret = PTR_ERR(priv->clk);
		dev_err(&pdev->dev, "no clock for device: %d\n", ret);
		return ret;
	}

	if (!devm_request_mem_region(&pdev->dev, r->start,
					resource_size(r), pdev->name)) {
		dev_err(&pdev->dev, "request mem failed");
		return -EBUSY;
>>>>>>> v4.9.227
	}

	priv->regs = devm_ioremap_nocache(&pdev->dev, r->start,
					resource_size(r));
	if (!priv->regs) {
		dev_err(&pdev->dev, "ioremap failed");
<<<<<<< HEAD
		ret = -ENOMEM;
		goto out_free_rng;
	}

	clk_enable(clk);

	ret = hwrng_register(rng);
	if (ret) {
		dev_err(&pdev->dev, "failed to register rng device\n");
		goto out_clk_disable;
=======
		return -ENOMEM;
	}

	ret = devm_hwrng_register(&pdev->dev, &priv->rng);
	if (ret) {
		dev_err(&pdev->dev, "failed to register rng device: %d\n",
			ret);
		return ret;
>>>>>>> v4.9.227
	}

	dev_info(&pdev->dev, "registered RNG driver\n");

	return 0;
<<<<<<< HEAD

out_clk_disable:
	clk_disable(clk);
out_free_rng:
	kfree(rng);
out_free_priv:
	kfree(priv);
out:
	return ret;
}

static int bcm63xx_rng_remove(struct platform_device *pdev)
{
	struct hwrng *rng = platform_get_drvdata(pdev);
	struct bcm63xx_rng_priv *priv = to_rng_priv(rng);

	hwrng_unregister(rng);
	clk_disable(priv->clk);
	kfree(priv);
	kfree(rng);

	return 0;
}

static struct platform_driver bcm63xx_rng_driver = {
	.probe		= bcm63xx_rng_probe,
	.remove		= bcm63xx_rng_remove,
	.driver		= {
		.name	= "bcm63xx-rng",
		.owner	= THIS_MODULE,
=======
}

#ifdef CONFIG_OF
static const struct of_device_id bcm63xx_rng_of_match[] = {
	{ .compatible = "brcm,bcm6368-rng", },
	{},
};
MODULE_DEVICE_TABLE(of, bcm63xx_rng_of_match);
#endif

static struct platform_driver bcm63xx_rng_driver = {
	.probe		= bcm63xx_rng_probe,
	.driver		= {
		.name	= "bcm63xx-rng",
		.of_match_table = of_match_ptr(bcm63xx_rng_of_match),
>>>>>>> v4.9.227
	},
};

module_platform_driver(bcm63xx_rng_driver);

MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_DESCRIPTION("Broadcom BCM63xx RNG driver");
MODULE_LICENSE("GPL");
