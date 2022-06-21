<<<<<<< HEAD
/**********************************************************************
 * Author: Cavium Networks
 *
 * Contact: support@caviumnetworks.com
 * This file is part of the OCTEON SDK
=======
/*
 * This file is based on code from OCTEON SDK by Cavium Networks.
>>>>>>> v4.9.227
 *
 * Copyright (c) 2003-2007 Cavium Networks
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
<<<<<<< HEAD
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * or visit http://www.gnu.org/licenses/.
 *
 * This file may also be available under a different license from Cavium.
 * Contact Cavium Networks for more information
**********************************************************************/
=======
 */

>>>>>>> v4.9.227
#include <linux/kernel.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/ratelimit.h>
#include <linux/of_mdio.h>
<<<<<<< HEAD

=======
#include <generated/utsrelease.h>
>>>>>>> v4.9.227
#include <net/dst.h>

#include <asm/octeon/octeon.h>

#include "ethernet-defines.h"
#include "octeon-ethernet.h"
#include "ethernet-mdio.h"
#include "ethernet-util.h"

<<<<<<< HEAD
#include <asm/octeon/cvmx-helper-board.h>

=======
#include <asm/octeon/cvmx-gmxx-defs.h>
>>>>>>> v4.9.227
#include <asm/octeon/cvmx-smix-defs.h>

static void cvm_oct_get_drvinfo(struct net_device *dev,
				struct ethtool_drvinfo *info)
{
<<<<<<< HEAD
	strlcpy(info->driver, "cavium-ethernet", sizeof(info->driver));
	strlcpy(info->version, OCTEON_ETHERNET_VERSION, sizeof(info->version));
	strlcpy(info->bus_info, "Builtin", sizeof(info->bus_info));
}

static int cvm_oct_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (priv->phydev)
		return phy_ethtool_gset(priv->phydev, cmd);

	return -EINVAL;
}

static int cvm_oct_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (priv->phydev)
		return phy_ethtool_sset(priv->phydev, cmd);

	return -EINVAL;
}

static int cvm_oct_nway_reset(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (priv->phydev)
		return phy_start_aneg(priv->phydev);
=======
	strlcpy(info->driver, KBUILD_MODNAME, sizeof(info->driver));
	strlcpy(info->version, UTS_RELEASE, sizeof(info->version));
	strlcpy(info->bus_info, "Builtin", sizeof(info->bus_info));
}

static int cvm_oct_nway_reset(struct net_device *dev)
{
	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (dev->phydev)
		return phy_start_aneg(dev->phydev);
>>>>>>> v4.9.227

	return -EINVAL;
}

const struct ethtool_ops cvm_oct_ethtool_ops = {
	.get_drvinfo = cvm_oct_get_drvinfo,
<<<<<<< HEAD
	.get_settings = cvm_oct_get_settings,
	.set_settings = cvm_oct_set_settings,
	.nway_reset = cvm_oct_nway_reset,
	.get_link = ethtool_op_get_link,
=======
	.nway_reset = cvm_oct_nway_reset,
	.get_link = ethtool_op_get_link,
	.get_link_ksettings = phy_ethtool_get_link_ksettings,
	.set_link_ksettings = phy_ethtool_set_link_ksettings,
>>>>>>> v4.9.227
};

/**
 * cvm_oct_ioctl - IOCTL support for PHY control
 * @dev:    Device to change
 * @rq:     the request
 * @cmd:    the command
 *
 * Returns Zero on success
 */
int cvm_oct_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
<<<<<<< HEAD
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (!netif_running(dev))
		return -EINVAL;

	if (!priv->phydev)
		return -EINVAL;

	return phy_mii_ioctl(priv->phydev, rq, cmd);
}

static void cvm_oct_note_carrier(struct octeon_ethernet *priv,
				 cvmx_helper_link_info_t li)
{
	if (li.s.link_up) {
		pr_notice_ratelimited("%s: %u Mbps %s duplex, port %d\n",
				      netdev_name(priv->netdev), li.s.speed,
				      (li.s.full_duplex) ? "Full" : "Half",
				      priv->port);
=======
	if (!netif_running(dev))
		return -EINVAL;

	if (!dev->phydev)
		return -EINVAL;

	return phy_mii_ioctl(dev->phydev, rq, cmd);
}

void cvm_oct_note_carrier(struct octeon_ethernet *priv,
			  cvmx_helper_link_info_t li)
{
	if (li.s.link_up) {
		pr_notice_ratelimited("%s: %u Mbps %s duplex, port %d, queue %d\n",
				      netdev_name(priv->netdev), li.s.speed,
				      (li.s.full_duplex) ? "Full" : "Half",
				      priv->port, priv->queue);
>>>>>>> v4.9.227
	} else {
		pr_notice_ratelimited("%s: Link down\n",
				      netdev_name(priv->netdev));
	}
}

<<<<<<< HEAD
void cvm_oct_set_carrier(struct octeon_ethernet *priv,
			 cvmx_helper_link_info_t link_info)
{
	cvm_oct_note_carrier(priv, link_info);
	if (link_info.s.link_up) {
		if (!netif_carrier_ok(priv->netdev))
			netif_carrier_on(priv->netdev);
	} else {
		if (netif_carrier_ok(priv->netdev))
			netif_carrier_off(priv->netdev);
	}
}

=======
>>>>>>> v4.9.227
void cvm_oct_adjust_link(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	cvmx_helper_link_info_t link_info;

<<<<<<< HEAD
	if (priv->last_link != priv->phydev->link) {
		priv->last_link = priv->phydev->link;
		link_info.u64 = 0;
		link_info.s.link_up = priv->last_link ? 1 : 0;
		link_info.s.full_duplex = priv->phydev->duplex ? 1 : 0;
		link_info.s.speed = priv->phydev->speed;

=======
	link_info.u64		= 0;
	link_info.s.link_up	= dev->phydev->link ? 1 : 0;
	link_info.s.full_duplex = dev->phydev->duplex ? 1 : 0;
	link_info.s.speed	= dev->phydev->speed;
	priv->link_info		= link_info.u64;

	/*
	 * The polling task need to know about link status changes.
	 */
	if (priv->poll)
		priv->poll(dev);

	if (priv->last_link != dev->phydev->link) {
		priv->last_link = dev->phydev->link;
>>>>>>> v4.9.227
		cvmx_helper_link_set(priv->port, link_info);
		cvm_oct_note_carrier(priv, link_info);
	}
}

int cvm_oct_common_stop(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
<<<<<<< HEAD
	cvmx_helper_link_info_t link_info;

	priv->poll = NULL;

	if (priv->phydev)
		phy_disconnect(priv->phydev);
	priv->phydev = NULL;
=======
	int interface = INTERFACE(priv->port);
	cvmx_helper_link_info_t link_info;
	union cvmx_gmxx_prtx_cfg gmx_cfg;
	int index = INDEX(priv->port);

	gmx_cfg.u64 = cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));
	gmx_cfg.s.en = 0;
	cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64);

	priv->poll = NULL;

	if (dev->phydev)
		phy_disconnect(dev->phydev);
>>>>>>> v4.9.227

	if (priv->last_link) {
		link_info.u64 = 0;
		priv->last_link = 0;

		cvmx_helper_link_set(priv->port, link_info);
		cvm_oct_note_carrier(priv, link_info);
	}
	return 0;
}

/**
 * cvm_oct_phy_setup_device - setup the PHY
 *
 * @dev:    Device to setup
 *
 * Returns Zero on success, negative on failure
 */
int cvm_oct_phy_setup_device(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	struct device_node *phy_node;
<<<<<<< HEAD
=======
	struct phy_device *phydev = NULL;
>>>>>>> v4.9.227

	if (!priv->of_node)
		goto no_phy;

	phy_node = of_parse_phandle(priv->of_node, "phy-handle", 0);
<<<<<<< HEAD
	if (!phy_node)
		goto no_phy;

	priv->phydev = of_phy_connect(dev, phy_node, cvm_oct_adjust_link, 0,
				      PHY_INTERFACE_MODE_GMII);

	if (priv->phydev == NULL)
		return -ENODEV;

	priv->last_link = 0;
	phy_start_aneg(priv->phydev);
=======
	if (!phy_node && of_phy_is_fixed_link(priv->of_node)) {
		int rc;

		rc = of_phy_register_fixed_link(priv->of_node);
		if (rc)
			return rc;

		phy_node = of_node_get(priv->of_node);
	}
	if (!phy_node)
		goto no_phy;

	phydev = of_phy_connect(dev, phy_node, cvm_oct_adjust_link, 0,
				PHY_INTERFACE_MODE_GMII);
	of_node_put(phy_node);

	if (!phydev)
		return -ENODEV;

	priv->last_link = 0;
	phy_start_aneg(phydev);
>>>>>>> v4.9.227

	return 0;
no_phy:
	/* If there is no phy, assume a direct MAC connection and that
	 * the link is up.
	 */
	netif_carrier_on(dev);
	return 0;
}
