/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2011, 2012 Cavium, Inc.
 */

#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/of_mdio.h>
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/mdio-mux.h>
<<<<<<< HEAD
#include <linux/of_gpio.h>

#define DRV_VERSION "1.0"
#define DRV_DESCRIPTION "GPIO controlled MDIO bus multiplexer driver"

#define MDIO_MUX_GPIO_MAX_BITS 8

struct mdio_mux_gpio_state {
	int gpio[MDIO_MUX_GPIO_MAX_BITS];
	unsigned int num_gpios;
=======
#include <linux/gpio/consumer.h>

#define DRV_VERSION "1.1"
#define DRV_DESCRIPTION "GPIO controlled MDIO bus multiplexer driver"

struct mdio_mux_gpio_state {
	struct gpio_descs *gpios;
>>>>>>> v4.9.227
	void *mux_handle;
};

static int mdio_mux_gpio_switch_fn(int current_child, int desired_child,
				   void *data)
{
<<<<<<< HEAD
	int change;
	unsigned int n;
	struct mdio_mux_gpio_state *s = data;
=======
	struct mdio_mux_gpio_state *s = data;
	int values[s->gpios->ndescs];
	unsigned int n;
>>>>>>> v4.9.227

	if (current_child == desired_child)
		return 0;

<<<<<<< HEAD
	change = current_child == -1 ? -1 : current_child ^ desired_child;

	for (n = 0; n < s->num_gpios; n++) {
		if (change & 1)
			gpio_set_value_cansleep(s->gpio[n],
						(desired_child & 1) != 0);
		change >>= 1;
		desired_child >>= 1;
	}
=======
	for (n = 0; n < s->gpios->ndescs; n++)
		values[n] = (desired_child >> n) & 1;

	gpiod_set_array_value_cansleep(s->gpios->ndescs, s->gpios->desc,
				       values);
>>>>>>> v4.9.227

	return 0;
}

static int mdio_mux_gpio_probe(struct platform_device *pdev)
{
<<<<<<< HEAD
	enum of_gpio_flags f;
	struct mdio_mux_gpio_state *s;
	int num_gpios;
	unsigned int n;
	int r;

	if (!pdev->dev.of_node)
		return -ENODEV;

	num_gpios = of_gpio_count(pdev->dev.of_node);
	if (num_gpios <= 0 || num_gpios > MDIO_MUX_GPIO_MAX_BITS)
		return -ENODEV;

=======
	struct mdio_mux_gpio_state *s;
	int r;

>>>>>>> v4.9.227
	s = devm_kzalloc(&pdev->dev, sizeof(*s), GFP_KERNEL);
	if (!s)
		return -ENOMEM;

<<<<<<< HEAD
	s->num_gpios = num_gpios;

	for (n = 0; n < num_gpios; ) {
		int gpio = of_get_gpio_flags(pdev->dev.of_node, n, &f);
		if (gpio < 0) {
			r = (gpio == -ENODEV) ? -EPROBE_DEFER : gpio;
			goto err;
		}
		s->gpio[n] = gpio;

		n++;

		r = gpio_request(gpio, "mdio_mux_gpio");
		if (r)
			goto err;

		r = gpio_direction_output(gpio, 0);
		if (r)
			goto err;
	}

	r = mdio_mux_init(&pdev->dev,
			  mdio_mux_gpio_switch_fn, &s->mux_handle, s);

	if (r == 0) {
		pdev->dev.platform_data = s;
		return 0;
	}
err:
	while (n) {
		n--;
		gpio_free(s->gpio[n]);
	}
	return r;
=======
	s->gpios = gpiod_get_array(&pdev->dev, NULL, GPIOD_OUT_LOW);
	if (IS_ERR(s->gpios))
		return PTR_ERR(s->gpios);

	r = mdio_mux_init(&pdev->dev,
			  mdio_mux_gpio_switch_fn, &s->mux_handle, s, NULL);

	if (r != 0) {
		gpiod_put_array(s->gpios);
		return r;
	}

	pdev->dev.platform_data = s;
	return 0;
>>>>>>> v4.9.227
}

static int mdio_mux_gpio_remove(struct platform_device *pdev)
{
	struct mdio_mux_gpio_state *s = dev_get_platdata(&pdev->dev);
	mdio_mux_uninit(s->mux_handle);
<<<<<<< HEAD
	return 0;
}

static struct of_device_id mdio_mux_gpio_match[] = {
=======
	gpiod_put_array(s->gpios);
	return 0;
}

static const struct of_device_id mdio_mux_gpio_match[] = {
>>>>>>> v4.9.227
	{
		.compatible = "mdio-mux-gpio",
	},
	{
		/* Legacy compatible property. */
		.compatible = "cavium,mdio-mux-sn74cbtlv3253",
	},
	{},
};
MODULE_DEVICE_TABLE(of, mdio_mux_gpio_match);

static struct platform_driver mdio_mux_gpio_driver = {
	.driver = {
		.name		= "mdio-mux-gpio",
<<<<<<< HEAD
		.owner		= THIS_MODULE,
=======
>>>>>>> v4.9.227
		.of_match_table = mdio_mux_gpio_match,
	},
	.probe		= mdio_mux_gpio_probe,
	.remove		= mdio_mux_gpio_remove,
};

module_platform_driver(mdio_mux_gpio_driver);

MODULE_DESCRIPTION(DRV_DESCRIPTION);
MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("David Daney");
MODULE_LICENSE("GPL");
