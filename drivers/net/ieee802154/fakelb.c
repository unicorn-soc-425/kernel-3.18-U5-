/*
 * Loopback IEEE 802.15.4 interface
 *
 * Copyright 2007-2012 Siemens AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
<<<<<<< HEAD
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
=======
>>>>>>> v4.9.227
 * Written by:
 * Sergey Lapin <slapin@ossfans.org>
 * Dmitry Eremin-Solenikov <dbaryshkov@gmail.com>
 * Alexander Smirnov <alex.bluesman.smirnov@gmail.com>
 */

#include <linux/module.h>
#include <linux/timer.h>
#include <linux/platform_device.h>
#include <linux/netdevice.h>
#include <linux/device.h>
#include <linux/spinlock.h>
#include <net/mac802154.h>
<<<<<<< HEAD
#include <net/wpan-phy.h>

static int numlbs = 1;

struct fakelb_dev_priv {
	struct ieee802154_dev *dev;

	struct list_head list;
	struct fakelb_priv *fake;

	spinlock_t lock;
	bool working;
};

struct fakelb_priv {
	struct list_head list;
	rwlock_t lock;
};

static int
fakelb_hw_ed(struct ieee802154_dev *dev, u8 *level)
{
	might_sleep();
	BUG_ON(!level);
=======
#include <net/cfg802154.h>

static int numlbs = 2;

static LIST_HEAD(fakelb_phys);
static DEFINE_MUTEX(fakelb_phys_lock);

static LIST_HEAD(fakelb_ifup_phys);
static DEFINE_RWLOCK(fakelb_ifup_phys_lock);

struct fakelb_phy {
	struct ieee802154_hw *hw;

	u8 page;
	u8 channel;

	bool suspended;

	struct list_head list;
	struct list_head list_ifup;
};

static int fakelb_hw_ed(struct ieee802154_hw *hw, u8 *level)
{
	WARN_ON(!level);
>>>>>>> v4.9.227
	*level = 0xbe;

	return 0;
}

<<<<<<< HEAD
static int
fakelb_hw_channel(struct ieee802154_dev *dev, int page, int channel)
{
	pr_debug("set channel to %d\n", channel);

	might_sleep();
	dev->phy->current_page = page;
	dev->phy->current_channel = channel;

	return 0;
}

static void
fakelb_hw_deliver(struct fakelb_dev_priv *priv, struct sk_buff *skb)
{
	struct sk_buff *newskb;

	spin_lock(&priv->lock);
	if (priv->working) {
		newskb = pskb_copy(skb, GFP_ATOMIC);
		ieee802154_rx_irqsafe(priv->dev, newskb, 0xcc);
	}
	spin_unlock(&priv->lock);
}

static int
fakelb_hw_xmit(struct ieee802154_dev *dev, struct sk_buff *skb)
{
	struct fakelb_dev_priv *priv = dev->priv;
	struct fakelb_priv *fake = priv->fake;

	might_sleep();

	read_lock_bh(&fake->lock);
	if (priv->list.next == priv->list.prev) {
		/* we are the only one device */
		fakelb_hw_deliver(priv, skb);
	} else {
		struct fakelb_dev_priv *dp;
		list_for_each_entry(dp, &priv->fake->list, list) {
			if (dp != priv &&
			    (dp->dev->phy->current_channel ==
			     priv->dev->phy->current_channel))
				fakelb_hw_deliver(dp, skb);
		}
	}
	read_unlock_bh(&fake->lock);
=======
static int fakelb_hw_channel(struct ieee802154_hw *hw, u8 page, u8 channel)
{
	struct fakelb_phy *phy = hw->priv;

	write_lock_bh(&fakelb_ifup_phys_lock);
	phy->page = page;
	phy->channel = channel;
	write_unlock_bh(&fakelb_ifup_phys_lock);
	return 0;
}

static int fakelb_hw_xmit(struct ieee802154_hw *hw, struct sk_buff *skb)
{
	struct fakelb_phy *current_phy = hw->priv, *phy;

	read_lock_bh(&fakelb_ifup_phys_lock);
	WARN_ON(current_phy->suspended);
	list_for_each_entry(phy, &fakelb_ifup_phys, list_ifup) {
		if (current_phy == phy)
			continue;

		if (current_phy->page == phy->page &&
		    current_phy->channel == phy->channel) {
			struct sk_buff *newskb = pskb_copy(skb, GFP_ATOMIC);

			if (newskb)
				ieee802154_rx_irqsafe(phy->hw, newskb, 0xcc);
		}
	}
	read_unlock_bh(&fakelb_ifup_phys_lock);

	ieee802154_xmit_complete(hw, skb, false);
	return 0;
}

static int fakelb_hw_start(struct ieee802154_hw *hw)
{
	struct fakelb_phy *phy = hw->priv;

	write_lock_bh(&fakelb_ifup_phys_lock);
	phy->suspended = false;
	list_add(&phy->list_ifup, &fakelb_ifup_phys);
	write_unlock_bh(&fakelb_ifup_phys_lock);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int
fakelb_hw_start(struct ieee802154_dev *dev) {
	struct fakelb_dev_priv *priv = dev->priv;
	int ret = 0;

	spin_lock(&priv->lock);
	if (priv->working)
		ret = -EBUSY;
	else
		priv->working = 1;
	spin_unlock(&priv->lock);

	return ret;
}

static void
fakelb_hw_stop(struct ieee802154_dev *dev) {
	struct fakelb_dev_priv *priv = dev->priv;

	spin_lock(&priv->lock);
	priv->working = 0;
	spin_unlock(&priv->lock);
}

static struct ieee802154_ops fakelb_ops = {
	.owner = THIS_MODULE,
	.xmit = fakelb_hw_xmit,
=======
static void fakelb_hw_stop(struct ieee802154_hw *hw)
{
	struct fakelb_phy *phy = hw->priv;

	write_lock_bh(&fakelb_ifup_phys_lock);
	phy->suspended = true;
	list_del(&phy->list_ifup);
	write_unlock_bh(&fakelb_ifup_phys_lock);
}

static int
fakelb_set_promiscuous_mode(struct ieee802154_hw *hw, const bool on)
{
	return 0;
}

static const struct ieee802154_ops fakelb_ops = {
	.owner = THIS_MODULE,
	.xmit_async = fakelb_hw_xmit,
>>>>>>> v4.9.227
	.ed = fakelb_hw_ed,
	.set_channel = fakelb_hw_channel,
	.start = fakelb_hw_start,
	.stop = fakelb_hw_stop,
<<<<<<< HEAD
=======
	.set_promiscuous_mode = fakelb_set_promiscuous_mode,
>>>>>>> v4.9.227
};

/* Number of dummy devices to be set up by this module. */
module_param(numlbs, int, 0);
MODULE_PARM_DESC(numlbs, " number of pseudo devices");

<<<<<<< HEAD
static int fakelb_add_one(struct device *dev, struct fakelb_priv *fake)
{
	struct fakelb_dev_priv *priv;
	int err;
	struct ieee802154_dev *ieee;

	ieee = ieee802154_alloc_device(sizeof(*priv), &fakelb_ops);
	if (!ieee)
		return -ENOMEM;

	priv = ieee->priv;
	priv->dev = ieee;

	/* 868 MHz BPSK	802.15.4-2003 */
	ieee->phy->channels_supported[0] |= 1;
	/* 915 MHz BPSK	802.15.4-2003 */
	ieee->phy->channels_supported[0] |= 0x7fe;
	/* 2.4 GHz O-QPSK 802.15.4-2003 */
	ieee->phy->channels_supported[0] |= 0x7FFF800;
	/* 868 MHz ASK 802.15.4-2006 */
	ieee->phy->channels_supported[1] |= 1;
	/* 915 MHz ASK 802.15.4-2006 */
	ieee->phy->channels_supported[1] |= 0x7fe;
	/* 868 MHz O-QPSK 802.15.4-2006 */
	ieee->phy->channels_supported[2] |= 1;
	/* 915 MHz O-QPSK 802.15.4-2006 */
	ieee->phy->channels_supported[2] |= 0x7fe;
	/* 2.4 GHz CSS 802.15.4a-2007 */
	ieee->phy->channels_supported[3] |= 0x3fff;
	/* UWB Sub-gigahertz 802.15.4a-2007 */
	ieee->phy->channels_supported[4] |= 1;
	/* UWB Low band 802.15.4a-2007 */
	ieee->phy->channels_supported[4] |= 0x1e;
	/* UWB High band 802.15.4a-2007 */
	ieee->phy->channels_supported[4] |= 0xffe0;
	/* 750 MHz O-QPSK 802.15.4c-2009 */
	ieee->phy->channels_supported[5] |= 0xf;
	/* 750 MHz MPSK 802.15.4c-2009 */
	ieee->phy->channels_supported[5] |= 0xf0;
	/* 950 MHz BPSK 802.15.4d-2009 */
	ieee->phy->channels_supported[6] |= 0x3ff;
	/* 950 MHz GFSK 802.15.4d-2009 */
	ieee->phy->channels_supported[6] |= 0x3ffc00;

	INIT_LIST_HEAD(&priv->list);
	priv->fake = fake;

	spin_lock_init(&priv->lock);

	ieee->parent = dev;

	err = ieee802154_register_device(ieee);
	if (err)
		goto err_reg;

	write_lock_bh(&fake->lock);
	list_add_tail(&priv->list, &fake->list);
	write_unlock_bh(&fake->lock);
=======
static int fakelb_add_one(struct device *dev)
{
	struct ieee802154_hw *hw;
	struct fakelb_phy *phy;
	int err;

	hw = ieee802154_alloc_hw(sizeof(*phy), &fakelb_ops);
	if (!hw)
		return -ENOMEM;

	phy = hw->priv;
	phy->hw = hw;

	/* 868 MHz BPSK	802.15.4-2003 */
	hw->phy->supported.channels[0] |= 1;
	/* 915 MHz BPSK	802.15.4-2003 */
	hw->phy->supported.channels[0] |= 0x7fe;
	/* 2.4 GHz O-QPSK 802.15.4-2003 */
	hw->phy->supported.channels[0] |= 0x7FFF800;
	/* 868 MHz ASK 802.15.4-2006 */
	hw->phy->supported.channels[1] |= 1;
	/* 915 MHz ASK 802.15.4-2006 */
	hw->phy->supported.channels[1] |= 0x7fe;
	/* 868 MHz O-QPSK 802.15.4-2006 */
	hw->phy->supported.channels[2] |= 1;
	/* 915 MHz O-QPSK 802.15.4-2006 */
	hw->phy->supported.channels[2] |= 0x7fe;
	/* 2.4 GHz CSS 802.15.4a-2007 */
	hw->phy->supported.channels[3] |= 0x3fff;
	/* UWB Sub-gigahertz 802.15.4a-2007 */
	hw->phy->supported.channels[4] |= 1;
	/* UWB Low band 802.15.4a-2007 */
	hw->phy->supported.channels[4] |= 0x1e;
	/* UWB High band 802.15.4a-2007 */
	hw->phy->supported.channels[4] |= 0xffe0;
	/* 750 MHz O-QPSK 802.15.4c-2009 */
	hw->phy->supported.channels[5] |= 0xf;
	/* 750 MHz MPSK 802.15.4c-2009 */
	hw->phy->supported.channels[5] |= 0xf0;
	/* 950 MHz BPSK 802.15.4d-2009 */
	hw->phy->supported.channels[6] |= 0x3ff;
	/* 950 MHz GFSK 802.15.4d-2009 */
	hw->phy->supported.channels[6] |= 0x3ffc00;

	ieee802154_random_extended_addr(&hw->phy->perm_extended_addr);
	/* fake phy channel 13 as default */
	hw->phy->current_channel = 13;
	phy->channel = hw->phy->current_channel;

	hw->flags = IEEE802154_HW_PROMISCUOUS;
	hw->parent = dev;

	err = ieee802154_register_hw(hw);
	if (err)
		goto err_reg;

	mutex_lock(&fakelb_phys_lock);
	list_add_tail(&phy->list, &fakelb_phys);
	mutex_unlock(&fakelb_phys_lock);
>>>>>>> v4.9.227

	return 0;

err_reg:
<<<<<<< HEAD
	ieee802154_free_device(priv->dev);
	return err;
}

static void fakelb_del(struct fakelb_dev_priv *priv)
{
	write_lock_bh(&priv->fake->lock);
	list_del(&priv->list);
	write_unlock_bh(&priv->fake->lock);

	ieee802154_unregister_device(priv->dev);
	ieee802154_free_device(priv->dev);
=======
	ieee802154_free_hw(phy->hw);
	return err;
}

static void fakelb_del(struct fakelb_phy *phy)
{
	list_del(&phy->list);

	ieee802154_unregister_hw(phy->hw);
	ieee802154_free_hw(phy->hw);
>>>>>>> v4.9.227
}

static int fakelb_probe(struct platform_device *pdev)
{
<<<<<<< HEAD
	struct fakelb_priv *priv;
	struct fakelb_dev_priv *dp;
	int err = -ENOMEM;
	int i;

	priv = devm_kzalloc(&pdev->dev, sizeof(struct fakelb_priv),
			    GFP_KERNEL);
	if (!priv)
		goto err_alloc;

	INIT_LIST_HEAD(&priv->list);
	rwlock_init(&priv->lock);

	for (i = 0; i < numlbs; i++) {
		err = fakelb_add_one(&pdev->dev, priv);
=======
	struct fakelb_phy *phy, *tmp;
	int err, i;

	for (i = 0; i < numlbs; i++) {
		err = fakelb_add_one(&pdev->dev);
>>>>>>> v4.9.227
		if (err < 0)
			goto err_slave;
	}

<<<<<<< HEAD
	platform_set_drvdata(pdev, priv);
=======
>>>>>>> v4.9.227
	dev_info(&pdev->dev, "added ieee802154 hardware\n");
	return 0;

err_slave:
<<<<<<< HEAD
	list_for_each_entry(dp, &priv->list, list)
		fakelb_del(dp);
err_alloc:
=======
	mutex_lock(&fakelb_phys_lock);
	list_for_each_entry_safe(phy, tmp, &fakelb_phys, list)
		fakelb_del(phy);
	mutex_unlock(&fakelb_phys_lock);
>>>>>>> v4.9.227
	return err;
}

static int fakelb_remove(struct platform_device *pdev)
{
<<<<<<< HEAD
	struct fakelb_priv *priv = platform_get_drvdata(pdev);
	struct fakelb_dev_priv *dp, *temp;

	list_for_each_entry_safe(dp, temp, &priv->list, list)
		fakelb_del(dp);

=======
	struct fakelb_phy *phy, *tmp;

	mutex_lock(&fakelb_phys_lock);
	list_for_each_entry_safe(phy, tmp, &fakelb_phys, list)
		fakelb_del(phy);
	mutex_unlock(&fakelb_phys_lock);
>>>>>>> v4.9.227
	return 0;
}

static struct platform_device *ieee802154fake_dev;

static struct platform_driver ieee802154fake_driver = {
	.probe = fakelb_probe,
	.remove = fakelb_remove,
	.driver = {
			.name = "ieee802154fakelb",
<<<<<<< HEAD
			.owner = THIS_MODULE,
=======
>>>>>>> v4.9.227
	},
};

static __init int fakelb_init_module(void)
{
	ieee802154fake_dev = platform_device_register_simple(
			     "ieee802154fakelb", -1, NULL, 0);
	return platform_driver_register(&ieee802154fake_driver);
}

static __exit void fake_remove_module(void)
{
	platform_driver_unregister(&ieee802154fake_driver);
	platform_device_unregister(ieee802154fake_dev);
}

module_init(fakelb_init_module);
module_exit(fake_remove_module);
MODULE_LICENSE("GPL");
