/*
 * OF helpers for network devices.
 *
 * This file is released under the GPLv2
 */

#ifndef __LINUX_OF_NET_H
#define __LINUX_OF_NET_H

#ifdef CONFIG_OF_NET
#include <linux/of.h>
<<<<<<< HEAD
extern int of_get_phy_mode(struct device_node *np);
extern const void *of_get_mac_address(struct device_node *np);
=======

struct net_device;
extern int of_get_phy_mode(struct device_node *np);
extern const void *of_get_mac_address(struct device_node *np);
extern struct net_device *of_find_net_device_by_node(struct device_node *np);
>>>>>>> v4.9.227
#else
static inline int of_get_phy_mode(struct device_node *np)
{
	return -ENODEV;
}

static inline const void *of_get_mac_address(struct device_node *np)
{
	return NULL;
}
<<<<<<< HEAD
=======

static inline struct net_device *of_find_net_device_by_node(struct device_node *np)
{
	return NULL;
}
>>>>>>> v4.9.227
#endif

#endif /* __LINUX_OF_NET_H */
