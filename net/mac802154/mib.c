/*
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
 * Dmitry Eremin-Solenikov <dbaryshkov@gmail.com>
 * Sergey Lapin <slapin@ossfans.org>
 * Maxim Gorbachyov <maxim.gorbachev@siemens.com>
 * Alexander Smirnov <alex.bluesman.smirnov@gmail.com>
 */

#include <linux/if_arp.h>

#include <net/mac802154.h>
#include <net/ieee802154_netdev.h>
<<<<<<< HEAD
#include <net/wpan-phy.h>

#include "mac802154.h"

struct phy_chan_notify_work {
	struct work_struct work;
	struct net_device *dev;
};

struct hw_addr_filt_notify_work {
	struct work_struct work;
	struct net_device *dev;
	unsigned long changed;
};

static struct mac802154_priv *mac802154_slave_get_priv(struct net_device *dev)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	return priv->hw;
}

static void hw_addr_notify(struct work_struct *work)
{
	struct hw_addr_filt_notify_work *nw = container_of(work,
			struct hw_addr_filt_notify_work, work);
	struct mac802154_priv *hw = mac802154_slave_get_priv(nw->dev);
	int res;

	res = hw->ops->set_hw_addr_filt(&hw->hw,
					&hw->hw.hw_filt,
					nw->changed);
	if (res)
		pr_debug("failed changed mask %lx\n", nw->changed);

	kfree(nw);
}

static void set_hw_addr_filt(struct net_device *dev, unsigned long changed)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
	struct hw_addr_filt_notify_work *work;

	work = kzalloc(sizeof(*work), GFP_ATOMIC);
	if (!work)
		return;

	INIT_WORK(&work->work, hw_addr_notify);
	work->dev = dev;
	work->changed = changed;
	queue_work(priv->hw->dev_workqueue, &work->work);
}

void mac802154_dev_set_short_addr(struct net_device *dev, __le16 val)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	spin_lock_bh(&priv->mib_lock);
	priv->short_addr = val;
	spin_unlock_bh(&priv->mib_lock);

	if ((priv->hw->ops->set_hw_addr_filt) &&
	    (priv->hw->hw.hw_filt.short_addr != priv->short_addr)) {
		priv->hw->hw.hw_filt.short_addr = priv->short_addr;
		set_hw_addr_filt(dev, IEEE802154_AFILT_SADDR_CHANGED);
	}
}

__le16 mac802154_dev_get_short_addr(const struct net_device *dev)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
	__le16 ret;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	spin_lock_bh(&priv->mib_lock);
	ret = priv->short_addr;
	spin_unlock_bh(&priv->mib_lock);

	return ret;
}

void mac802154_dev_set_ieee_addr(struct net_device *dev)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
	struct mac802154_priv *mac = priv->hw;

	priv->extended_addr = ieee802154_devaddr_from_raw(dev->dev_addr);

	if (mac->ops->set_hw_addr_filt &&
	    mac->hw.hw_filt.ieee_addr != priv->extended_addr) {
		mac->hw.hw_filt.ieee_addr = priv->extended_addr;
		set_hw_addr_filt(dev, IEEE802154_AFILT_IEEEADDR_CHANGED);
	}
}

__le16 mac802154_dev_get_pan_id(const struct net_device *dev)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
	__le16 ret;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	spin_lock_bh(&priv->mib_lock);
	ret = priv->pan_id;
	spin_unlock_bh(&priv->mib_lock);

	return ret;
}

void mac802154_dev_set_pan_id(struct net_device *dev, __le16 val)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	spin_lock_bh(&priv->mib_lock);
	priv->pan_id = val;
	spin_unlock_bh(&priv->mib_lock);

	if ((priv->hw->ops->set_hw_addr_filt) &&
	    (priv->hw->hw.hw_filt.pan_id != priv->pan_id)) {
		priv->hw->hw.hw_filt.pan_id = priv->pan_id;
		set_hw_addr_filt(dev, IEEE802154_AFILT_PANID_CHANGED);
	}
}

u8 mac802154_dev_get_dsn(const struct net_device *dev)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	return priv->dsn++;
}

static void phy_chan_notify(struct work_struct *work)
{
	struct phy_chan_notify_work *nw = container_of(work,
					  struct phy_chan_notify_work, work);
	struct mac802154_priv *hw = mac802154_slave_get_priv(nw->dev);
	struct mac802154_sub_if_data *priv = netdev_priv(nw->dev);
	int res;

	mutex_lock(&priv->hw->phy->pib_lock);
	res = hw->ops->set_channel(&hw->hw, priv->page, priv->chan);
	if (res) {
		pr_debug("set_channel failed\n");
	} else {
		priv->hw->phy->current_channel = priv->chan;
		priv->hw->phy->current_page = priv->page;
	}
	mutex_unlock(&priv->hw->phy->pib_lock);

	kfree(nw);
}

void mac802154_dev_set_page_channel(struct net_device *dev, u8 page, u8 chan)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
	struct phy_chan_notify_work *work;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	spin_lock_bh(&priv->mib_lock);
	priv->page = page;
	priv->chan = chan;
	spin_unlock_bh(&priv->mib_lock);

	mutex_lock(&priv->hw->phy->pib_lock);
	if (priv->hw->phy->current_channel != priv->chan ||
	    priv->hw->phy->current_page != priv->page) {
		mutex_unlock(&priv->hw->phy->pib_lock);

		work = kzalloc(sizeof(*work), GFP_ATOMIC);
		if (!work)
			return;

		INIT_WORK(&work->work, phy_chan_notify);
		work->dev = dev;
		queue_work(priv->hw->dev_workqueue, &work->work);
	} else {
		mutex_unlock(&priv->hw->phy->pib_lock);
	}
}


int mac802154_get_params(struct net_device *dev,
			 struct ieee802154_llsec_params *params)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
=======
#include <net/cfg802154.h>

#include "ieee802154_i.h"
#include "driver-ops.h"

void mac802154_dev_set_page_channel(struct net_device *dev, u8 page, u8 chan)
{
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);
	struct ieee802154_local *local = sdata->local;
	int res;

	ASSERT_RTNL();

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	res = drv_set_channel(local, page, chan);
	if (res) {
		pr_debug("set_channel failed\n");
	} else {
		local->phy->current_channel = chan;
		local->phy->current_page = page;
	}
}

int mac802154_get_params(struct net_device *dev,
			 struct ieee802154_llsec_params *params)
{
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);
>>>>>>> v4.9.227
	int res;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

<<<<<<< HEAD
	mutex_lock(&priv->sec_mtx);
	res = mac802154_llsec_get_params(&priv->sec, params);
	mutex_unlock(&priv->sec_mtx);
=======
	mutex_lock(&sdata->sec_mtx);
	res = mac802154_llsec_get_params(&sdata->sec, params);
	mutex_unlock(&sdata->sec_mtx);
>>>>>>> v4.9.227

	return res;
}

int mac802154_set_params(struct net_device *dev,
			 const struct ieee802154_llsec_params *params,
			 int changed)
{
<<<<<<< HEAD
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
=======
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);
>>>>>>> v4.9.227
	int res;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

<<<<<<< HEAD
	mutex_lock(&priv->sec_mtx);
	res = mac802154_llsec_set_params(&priv->sec, params, changed);
	mutex_unlock(&priv->sec_mtx);
=======
	mutex_lock(&sdata->sec_mtx);
	res = mac802154_llsec_set_params(&sdata->sec, params, changed);
	mutex_unlock(&sdata->sec_mtx);
>>>>>>> v4.9.227

	return res;
}

<<<<<<< HEAD

=======
>>>>>>> v4.9.227
int mac802154_add_key(struct net_device *dev,
		      const struct ieee802154_llsec_key_id *id,
		      const struct ieee802154_llsec_key *key)
{
<<<<<<< HEAD
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
=======
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);
>>>>>>> v4.9.227
	int res;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

<<<<<<< HEAD
	mutex_lock(&priv->sec_mtx);
	res = mac802154_llsec_key_add(&priv->sec, id, key);
	mutex_unlock(&priv->sec_mtx);
=======
	mutex_lock(&sdata->sec_mtx);
	res = mac802154_llsec_key_add(&sdata->sec, id, key);
	mutex_unlock(&sdata->sec_mtx);
>>>>>>> v4.9.227

	return res;
}

int mac802154_del_key(struct net_device *dev,
		      const struct ieee802154_llsec_key_id *id)
{
<<<<<<< HEAD
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
=======
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);
>>>>>>> v4.9.227
	int res;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

<<<<<<< HEAD
	mutex_lock(&priv->sec_mtx);
	res = mac802154_llsec_key_del(&priv->sec, id);
	mutex_unlock(&priv->sec_mtx);
=======
	mutex_lock(&sdata->sec_mtx);
	res = mac802154_llsec_key_del(&sdata->sec, id);
	mutex_unlock(&sdata->sec_mtx);
>>>>>>> v4.9.227

	return res;
}

<<<<<<< HEAD

int mac802154_add_dev(struct net_device *dev,
		      const struct ieee802154_llsec_device *llsec_dev)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
=======
int mac802154_add_dev(struct net_device *dev,
		      const struct ieee802154_llsec_device *llsec_dev)
{
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);
>>>>>>> v4.9.227
	int res;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

<<<<<<< HEAD
	mutex_lock(&priv->sec_mtx);
	res = mac802154_llsec_dev_add(&priv->sec, llsec_dev);
	mutex_unlock(&priv->sec_mtx);
=======
	mutex_lock(&sdata->sec_mtx);
	res = mac802154_llsec_dev_add(&sdata->sec, llsec_dev);
	mutex_unlock(&sdata->sec_mtx);
>>>>>>> v4.9.227

	return res;
}

int mac802154_del_dev(struct net_device *dev, __le64 dev_addr)
{
<<<<<<< HEAD
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
=======
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);
>>>>>>> v4.9.227
	int res;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

<<<<<<< HEAD
	mutex_lock(&priv->sec_mtx);
	res = mac802154_llsec_dev_del(&priv->sec, dev_addr);
	mutex_unlock(&priv->sec_mtx);
=======
	mutex_lock(&sdata->sec_mtx);
	res = mac802154_llsec_dev_del(&sdata->sec, dev_addr);
	mutex_unlock(&sdata->sec_mtx);
>>>>>>> v4.9.227

	return res;
}

<<<<<<< HEAD

=======
>>>>>>> v4.9.227
int mac802154_add_devkey(struct net_device *dev,
			 __le64 device_addr,
			 const struct ieee802154_llsec_device_key *key)
{
<<<<<<< HEAD
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
=======
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);
>>>>>>> v4.9.227
	int res;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

<<<<<<< HEAD
	mutex_lock(&priv->sec_mtx);
	res = mac802154_llsec_devkey_add(&priv->sec, device_addr, key);
	mutex_unlock(&priv->sec_mtx);
=======
	mutex_lock(&sdata->sec_mtx);
	res = mac802154_llsec_devkey_add(&sdata->sec, device_addr, key);
	mutex_unlock(&sdata->sec_mtx);
>>>>>>> v4.9.227

	return res;
}

int mac802154_del_devkey(struct net_device *dev,
			 __le64 device_addr,
			 const struct ieee802154_llsec_device_key *key)
{
<<<<<<< HEAD
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
=======
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);
>>>>>>> v4.9.227
	int res;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

<<<<<<< HEAD
	mutex_lock(&priv->sec_mtx);
	res = mac802154_llsec_devkey_del(&priv->sec, device_addr, key);
	mutex_unlock(&priv->sec_mtx);
=======
	mutex_lock(&sdata->sec_mtx);
	res = mac802154_llsec_devkey_del(&sdata->sec, device_addr, key);
	mutex_unlock(&sdata->sec_mtx);
>>>>>>> v4.9.227

	return res;
}

<<<<<<< HEAD

int mac802154_add_seclevel(struct net_device *dev,
			   const struct ieee802154_llsec_seclevel *sl)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
=======
int mac802154_add_seclevel(struct net_device *dev,
			   const struct ieee802154_llsec_seclevel *sl)
{
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);
>>>>>>> v4.9.227
	int res;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

<<<<<<< HEAD
	mutex_lock(&priv->sec_mtx);
	res = mac802154_llsec_seclevel_add(&priv->sec, sl);
	mutex_unlock(&priv->sec_mtx);
=======
	mutex_lock(&sdata->sec_mtx);
	res = mac802154_llsec_seclevel_add(&sdata->sec, sl);
	mutex_unlock(&sdata->sec_mtx);
>>>>>>> v4.9.227

	return res;
}

int mac802154_del_seclevel(struct net_device *dev,
			   const struct ieee802154_llsec_seclevel *sl)
{
<<<<<<< HEAD
	struct mac802154_sub_if_data *priv = netdev_priv(dev);
=======
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);
>>>>>>> v4.9.227
	int res;

	BUG_ON(dev->type != ARPHRD_IEEE802154);

<<<<<<< HEAD
	mutex_lock(&priv->sec_mtx);
	res = mac802154_llsec_seclevel_del(&priv->sec, sl);
	mutex_unlock(&priv->sec_mtx);
=======
	mutex_lock(&sdata->sec_mtx);
	res = mac802154_llsec_seclevel_del(&sdata->sec, sl);
	mutex_unlock(&sdata->sec_mtx);
>>>>>>> v4.9.227

	return res;
}

<<<<<<< HEAD

void mac802154_lock_table(struct net_device *dev)
{
	struct mac802154_sub_if_data *priv = netdev_priv(dev);

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	mutex_lock(&priv->sec_mtx);
=======
void mac802154_lock_table(struct net_device *dev)
{
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	mutex_lock(&sdata->sec_mtx);
>>>>>>> v4.9.227
}

void mac802154_get_table(struct net_device *dev,
			 struct ieee802154_llsec_table **t)
{
<<<<<<< HEAD
	struct mac802154_sub_if_data *priv = netdev_priv(dev);

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	*t = &priv->sec.table;
=======
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	*t = &sdata->sec.table;
>>>>>>> v4.9.227
}

void mac802154_unlock_table(struct net_device *dev)
{
<<<<<<< HEAD
	struct mac802154_sub_if_data *priv = netdev_priv(dev);

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	mutex_unlock(&priv->sec_mtx);
=======
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);

	BUG_ON(dev->type != ARPHRD_IEEE802154);

	mutex_unlock(&sdata->sec_mtx);
>>>>>>> v4.9.227
}
