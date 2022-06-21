/*
 * net/dsa/dsa_priv.h - Hardware switch handling
 * Copyright (c) 2008-2009 Marvell Semiconductor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __DSA_PRIV_H
#define __DSA_PRIV_H

#include <linux/phy.h>
#include <linux/netdevice.h>
<<<<<<< HEAD

struct dsa_device_ops {
	netdev_tx_t (*xmit)(struct sk_buff *skb, struct net_device *dev);
=======
#include <linux/netpoll.h>

struct dsa_device_ops {
	struct sk_buff *(*xmit)(struct sk_buff *skb, struct net_device *dev);
>>>>>>> v4.9.227
	int (*rcv)(struct sk_buff *skb, struct net_device *dev,
		   struct packet_type *pt, struct net_device *orig_dev);
};

struct dsa_slave_priv {
<<<<<<< HEAD
	/*
	 * The linux network interface corresponding to this
	 * switch port.
	 */
	struct net_device	*dev;
	netdev_tx_t		(*xmit)(struct sk_buff *skb,
=======
	struct sk_buff *	(*xmit)(struct sk_buff *skb,
>>>>>>> v4.9.227
					struct net_device *dev);

	/*
	 * Which switch this port is a part of, and the port index
	 * for this port.
	 */
	struct dsa_switch	*parent;
	u8			port;

	/*
	 * The phylib phy_device pointer for the PHY connected
	 * to this port.
	 */
	struct phy_device	*phy;
	phy_interface_t		phy_interface;
	int			old_link;
	int			old_pause;
	int			old_duplex;
<<<<<<< HEAD
=======

	struct net_device	*bridge_dev;
#ifdef CONFIG_NET_POLL_CONTROLLER
	struct netpoll		*netpoll;
#endif
>>>>>>> v4.9.227
};

/* dsa.c */
extern char dsa_driver_version[];
<<<<<<< HEAD
=======
int dsa_cpu_dsa_setup(struct dsa_switch *ds, struct device *dev,
		      struct device_node *port_dn, int port);
void dsa_cpu_dsa_destroy(struct device_node *port_dn);
const struct dsa_device_ops *dsa_resolve_tag_protocol(int tag_protocol);
int dsa_cpu_port_ethtool_setup(struct dsa_switch *ds);
void dsa_cpu_port_ethtool_restore(struct dsa_switch *ds);
>>>>>>> v4.9.227

/* slave.c */
extern const struct dsa_device_ops notag_netdev_ops;
void dsa_slave_mii_bus_init(struct dsa_switch *ds);
<<<<<<< HEAD
struct net_device *dsa_slave_create(struct dsa_switch *ds,
				    struct device *parent,
				    int port, char *name);
int dsa_slave_suspend(struct net_device *slave_dev);
int dsa_slave_resume(struct net_device *slave_dev);
=======
void dsa_cpu_port_ethtool_init(struct ethtool_ops *ops);
int dsa_slave_create(struct dsa_switch *ds, struct device *parent,
		     int port, const char *name);
void dsa_slave_destroy(struct net_device *slave_dev);
int dsa_slave_suspend(struct net_device *slave_dev);
int dsa_slave_resume(struct net_device *slave_dev);
int dsa_slave_netdevice_event(struct notifier_block *unused,
			      unsigned long event, void *ptr);
>>>>>>> v4.9.227

/* tag_dsa.c */
extern const struct dsa_device_ops dsa_netdev_ops;

/* tag_edsa.c */
extern const struct dsa_device_ops edsa_netdev_ops;

/* tag_trailer.c */
extern const struct dsa_device_ops trailer_netdev_ops;

/* tag_brcm.c */
extern const struct dsa_device_ops brcm_netdev_ops;

<<<<<<< HEAD
=======
/* tag_qca.c */
extern const struct dsa_device_ops qca_netdev_ops;
>>>>>>> v4.9.227

#endif
