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

#include <linux/netdevice.h>
#include <linux/if_arp.h>
#include <linux/crc-ccitt.h>
<<<<<<< HEAD

#include <net/ieee802154_netdev.h>
#include <net/mac802154.h>
#include <net/wpan-phy.h>

#include "mac802154.h"

/* IEEE 802.15.4 transceivers can sleep during the xmit session, so process
 * packets through the workqueue.
 */
struct xmit_work {
	struct sk_buff *skb;
	struct work_struct work;
	struct mac802154_priv *priv;
	u8 chan;
	u8 page;
};

static void mac802154_xmit_worker(struct work_struct *work)
{
	struct xmit_work *xw = container_of(work, struct xmit_work, work);
	struct mac802154_sub_if_data *sdata;
	int res;

	mutex_lock(&xw->priv->phy->pib_lock);
	if (xw->priv->phy->current_channel != xw->chan ||
	    xw->priv->phy->current_page != xw->page) {
		res = xw->priv->ops->set_channel(&xw->priv->hw,
						  xw->page,
						  xw->chan);
		if (res) {
			pr_debug("set_channel failed\n");
			goto out;
		}

		xw->priv->phy->current_channel = xw->chan;
		xw->priv->phy->current_page = xw->page;
	}

	res = xw->priv->ops->xmit(&xw->priv->hw, xw->skb);
	if (res)
		pr_debug("transmission failed\n");

out:
	mutex_unlock(&xw->priv->phy->pib_lock);

	/* Restart the netif queue on each sub_if_data object. */
	rcu_read_lock();
	list_for_each_entry_rcu(sdata, &xw->priv->slaves, list)
		netif_wake_queue(sdata->dev);
	rcu_read_unlock();

	dev_kfree_skb(xw->skb);

	kfree(xw);
}

netdev_tx_t mac802154_tx(struct mac802154_priv *priv, struct sk_buff *skb,
			 u8 page, u8 chan)
{
	struct xmit_work *work;
	struct mac802154_sub_if_data *sdata;

	if (!(priv->phy->channels_supported[page] & (1 << chan))) {
		WARN_ON(1);
		goto err_tx;
	}

	mac802154_monitors_rx(mac802154_to_priv(&priv->hw), skb);

	if (!(priv->hw.flags & IEEE802154_HW_OMIT_CKSUM)) {
		u16 crc = crc_ccitt(0, skb->data, skb->len);
		u8 *data = skb_put(skb, 2);

		data[0] = crc & 0xff;
		data[1] = crc >> 8;
	}

	if (skb_cow_head(skb, priv->hw.extra_tx_headroom))
		goto err_tx;

	work = kzalloc(sizeof(*work), GFP_ATOMIC);
	if (!work) {
		kfree_skb(skb);
		return NETDEV_TX_BUSY;
	}

	/* Stop the netif queue on each sub_if_data object. */
	rcu_read_lock();
	list_for_each_entry_rcu(sdata, &priv->slaves, list)
		netif_stop_queue(sdata->dev);
	rcu_read_unlock();

	INIT_WORK(&work->work, mac802154_xmit_worker);
	work->skb = skb;
	work->priv = priv;
	work->page = page;
	work->chan = chan;

	queue_work(priv->dev_workqueue, &work->work);
=======
#include <asm/unaligned.h>

#include <net/rtnetlink.h>
#include <net/ieee802154_netdev.h>
#include <net/mac802154.h>
#include <net/cfg802154.h>

#include "ieee802154_i.h"
#include "driver-ops.h"

void ieee802154_xmit_worker(struct work_struct *work)
{
	struct ieee802154_local *local =
		container_of(work, struct ieee802154_local, tx_work);
	struct sk_buff *skb = local->tx_skb;
	struct net_device *dev = skb->dev;
	int res;

	res = drv_xmit_sync(local, skb);
	if (res)
		goto err_tx;

	ieee802154_xmit_complete(&local->hw, skb, false);

	dev->stats.tx_packets++;
	dev->stats.tx_bytes += skb->len;

	return;

err_tx:
	/* Restart the netif queue on each sub_if_data object. */
	ieee802154_wake_queue(&local->hw);
	kfree_skb(skb);
	netdev_dbg(dev, "transmission failed\n");
}

static netdev_tx_t
ieee802154_tx(struct ieee802154_local *local, struct sk_buff *skb)
{
	struct net_device *dev = skb->dev;
	int ret;

	if (!(local->hw.flags & IEEE802154_HW_TX_OMIT_CKSUM)) {
		struct sk_buff *nskb;
		u16 crc;

		if (unlikely(skb_tailroom(skb) < IEEE802154_FCS_LEN)) {
			nskb = skb_copy_expand(skb, 0, IEEE802154_FCS_LEN,
					       GFP_ATOMIC);
			if (likely(nskb)) {
				consume_skb(skb);
				skb = nskb;
			} else {
				goto err_tx;
			}
		}

		crc = crc_ccitt(0, skb->data, skb->len);
		put_unaligned_le16(crc, skb_put(skb, 2));
	}

	/* Stop the netif queue on each sub_if_data object. */
	ieee802154_stop_queue(&local->hw);

	/* async is priority, otherwise sync is fallback */
	if (local->ops->xmit_async) {
		ret = drv_xmit_async(local, skb);
		if (ret) {
			ieee802154_wake_queue(&local->hw);
			goto err_tx;
		}

		dev->stats.tx_packets++;
		dev->stats.tx_bytes += skb->len;
	} else {
		local->tx_skb = skb;
		queue_work(local->workqueue, &local->tx_work);
	}
>>>>>>> v4.9.227

	return NETDEV_TX_OK;

err_tx:
	kfree_skb(skb);
	return NETDEV_TX_OK;
}
<<<<<<< HEAD
=======

netdev_tx_t
ieee802154_monitor_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);

	skb->skb_iif = dev->ifindex;

	return ieee802154_tx(sdata->local, skb);
}

netdev_tx_t
ieee802154_subif_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct ieee802154_sub_if_data *sdata = IEEE802154_DEV_TO_SUB_IF(dev);
	int rc;

	/* TODO we should move it to wpan_dev_hard_header and dev_hard_header
	 * functions. The reason is wireshark will show a mac header which is
	 * with security fields but the payload is not encrypted.
	 */
	rc = mac802154_llsec_encrypt(&sdata->sec, skb);
	if (rc) {
		netdev_warn(dev, "encryption failed: %i\n", rc);
		kfree_skb(skb);
		return NETDEV_TX_OK;
	}

	skb->skb_iif = dev->ifindex;

	return ieee802154_tx(sdata->local, skb);
}
>>>>>>> v4.9.227
