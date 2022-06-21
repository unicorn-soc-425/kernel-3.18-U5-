/*
 * net/dsa/mv88e6060.c - Driver for Marvell 88e6060 switch chips
 * Copyright (c) 2008-2009 Marvell Semiconductor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/phy.h>
#include <net/dsa.h>
<<<<<<< HEAD

#define REG_PORT(p)		(8 + (p))
#define REG_GLOBAL		0x0f

static int reg_read(struct dsa_switch *ds, int addr, int reg)
{
	struct mii_bus *bus = dsa_host_dev_to_mii_bus(ds->master_dev);

	if (bus == NULL)
		return -EINVAL;

	return mdiobus_read(bus, ds->pd->sw_addr + addr, reg);
=======
#include "mv88e6060.h"

static int reg_read(struct dsa_switch *ds, int addr, int reg)
{
	struct mv88e6060_priv *priv = ds->priv;

	return mdiobus_read_nested(priv->bus, priv->sw_addr + addr, reg);
>>>>>>> v4.9.227
}

#define REG_READ(addr, reg)					\
	({							\
		int __ret;					\
								\
		__ret = reg_read(ds, addr, reg);		\
		if (__ret < 0)					\
			return __ret;				\
		__ret;						\
	})


static int reg_write(struct dsa_switch *ds, int addr, int reg, u16 val)
{
<<<<<<< HEAD
	struct mii_bus *bus = dsa_host_dev_to_mii_bus(ds->master_dev);

	if (bus == NULL)
		return -EINVAL;

	return mdiobus_write(bus, ds->pd->sw_addr + addr, reg, val);
=======
	struct mv88e6060_priv *priv = ds->priv;

	return mdiobus_write_nested(priv->bus, priv->sw_addr + addr, reg, val);
>>>>>>> v4.9.227
}

#define REG_WRITE(addr, reg, val)				\
	({							\
		int __ret;					\
								\
		__ret = reg_write(ds, addr, reg, val);		\
		if (__ret < 0)					\
			return __ret;				\
	})

<<<<<<< HEAD
static char *mv88e6060_probe(struct device *host_dev, int sw_addr)
{
	struct mii_bus *bus = dsa_host_dev_to_mii_bus(host_dev);
	int ret;

	if (bus == NULL)
		return NULL;

	ret = mdiobus_read(bus, sw_addr + REG_PORT(0), 0x03);
	if (ret >= 0) {
		ret &= 0xfff0;
		if (ret == 0x0600)
=======
static const char *mv88e6060_get_name(struct mii_bus *bus, int sw_addr)
{
	int ret;

	ret = mdiobus_read(bus, sw_addr + REG_PORT(0), PORT_SWITCH_ID);
	if (ret >= 0) {
		if (ret == PORT_SWITCH_ID_6060)
			return "Marvell 88E6060 (A0)";
		if (ret == PORT_SWITCH_ID_6060_R1 ||
		    ret == PORT_SWITCH_ID_6060_R2)
			return "Marvell 88E6060 (B0)";
		if ((ret & PORT_SWITCH_ID_6060_MASK) == PORT_SWITCH_ID_6060)
>>>>>>> v4.9.227
			return "Marvell 88E6060";
	}

	return NULL;
}

<<<<<<< HEAD
=======
static enum dsa_tag_protocol mv88e6060_get_tag_protocol(struct dsa_switch *ds)
{
	return DSA_TAG_PROTO_TRAILER;
}

static const char *mv88e6060_drv_probe(struct device *dsa_dev,
				       struct device *host_dev, int sw_addr,
				       void **_priv)
{
	struct mii_bus *bus = dsa_host_dev_to_mii_bus(host_dev);
	struct mv88e6060_priv *priv;
	const char *name;

	name = mv88e6060_get_name(bus, sw_addr);
	if (name) {
		priv = devm_kzalloc(dsa_dev, sizeof(*priv), GFP_KERNEL);
		if (!priv)
			return NULL;
		*_priv = priv;
		priv->bus = bus;
		priv->sw_addr = sw_addr;
	}

	return name;
}

>>>>>>> v4.9.227
static int mv88e6060_switch_reset(struct dsa_switch *ds)
{
	int i;
	int ret;
	unsigned long timeout;

	/* Set all ports to the disabled state. */
<<<<<<< HEAD
	for (i = 0; i < 6; i++) {
		ret = REG_READ(REG_PORT(i), 0x04);
		REG_WRITE(REG_PORT(i), 0x04, ret & 0xfffc);
=======
	for (i = 0; i < MV88E6060_PORTS; i++) {
		ret = REG_READ(REG_PORT(i), PORT_CONTROL);
		REG_WRITE(REG_PORT(i), PORT_CONTROL,
			  ret & ~PORT_CONTROL_STATE_MASK);
>>>>>>> v4.9.227
	}

	/* Wait for transmit queues to drain. */
	usleep_range(2000, 4000);

	/* Reset the switch. */
<<<<<<< HEAD
	REG_WRITE(REG_GLOBAL, 0x0a, 0xa130);
=======
	REG_WRITE(REG_GLOBAL, GLOBAL_ATU_CONTROL,
		  GLOBAL_ATU_CONTROL_SWRESET |
		  GLOBAL_ATU_CONTROL_LEARNDIS);
>>>>>>> v4.9.227

	/* Wait up to one second for reset to complete. */
	timeout = jiffies + 1 * HZ;
	while (time_before(jiffies, timeout)) {
<<<<<<< HEAD
		ret = REG_READ(REG_GLOBAL, 0x00);
		if ((ret & 0x8000) == 0x0000)
=======
		ret = REG_READ(REG_GLOBAL, GLOBAL_STATUS);
		if (ret & GLOBAL_STATUS_INIT_READY)
>>>>>>> v4.9.227
			break;

		usleep_range(1000, 2000);
	}
	if (time_after(jiffies, timeout))
		return -ETIMEDOUT;

	return 0;
}

static int mv88e6060_setup_global(struct dsa_switch *ds)
{
	/* Disable discarding of frames with excessive collisions,
	 * set the maximum frame size to 1536 bytes, and mask all
	 * interrupt sources.
	 */
<<<<<<< HEAD
	REG_WRITE(REG_GLOBAL, 0x04, 0x0800);

	/* Enable automatic address learning, set the address
	 * database size to 1024 entries, and set the default aging
	 * time to 5 minutes.
	 */
	REG_WRITE(REG_GLOBAL, 0x0a, 0x2130);
=======
	REG_WRITE(REG_GLOBAL, GLOBAL_CONTROL, GLOBAL_CONTROL_MAX_FRAME_1536);

	/* Disable automatic address learning.
	 */
	REG_WRITE(REG_GLOBAL, GLOBAL_ATU_CONTROL,
		  GLOBAL_ATU_CONTROL_LEARNDIS);
>>>>>>> v4.9.227

	return 0;
}

static int mv88e6060_setup_port(struct dsa_switch *ds, int p)
{
	int addr = REG_PORT(p);

	/* Do not force flow control, disable Ingress and Egress
	 * Header tagging, disable VLAN tunneling, and set the port
	 * state to Forwarding.  Additionally, if this is the CPU
	 * port, enable Ingress and Egress Trailer tagging mode.
	 */
<<<<<<< HEAD
	REG_WRITE(addr, 0x04, dsa_is_cpu_port(ds, p) ?  0x4103 : 0x0003);
=======
	REG_WRITE(addr, PORT_CONTROL,
		  dsa_is_cpu_port(ds, p) ?
			PORT_CONTROL_TRAILER |
			PORT_CONTROL_INGRESS_MODE |
			PORT_CONTROL_STATE_FORWARDING :
			PORT_CONTROL_STATE_FORWARDING);
>>>>>>> v4.9.227

	/* Port based VLAN map: give each port its own address
	 * database, allow the CPU port to talk to each of the 'real'
	 * ports, and allow each of the 'real' ports to only talk to
	 * the CPU port.
	 */
<<<<<<< HEAD
	REG_WRITE(addr, 0x06,
			((p & 0xf) << 12) |
			 (dsa_is_cpu_port(ds, p) ?
				ds->phys_port_mask :
				(1 << ds->dst->cpu_port)));
=======
	REG_WRITE(addr, PORT_VLAN_MAP,
		  ((p & 0xf) << PORT_VLAN_MAP_DBNUM_SHIFT) |
		   (dsa_is_cpu_port(ds, p) ?
			ds->enabled_port_mask :
			BIT(ds->dst->cpu_port)));
>>>>>>> v4.9.227

	/* Port Association Vector: when learning source addresses
	 * of packets, add the address to the address database using
	 * a port bitmap that has only the bit for this port set and
	 * the other bits clear.
	 */
<<<<<<< HEAD
	REG_WRITE(addr, 0x0b, 1 << p);
=======
	REG_WRITE(addr, PORT_ASSOC_VECTOR, BIT(p));
>>>>>>> v4.9.227

	return 0;
}

static int mv88e6060_setup(struct dsa_switch *ds)
{
<<<<<<< HEAD
	int i;
	int ret;
=======
	int ret;
	int i;
>>>>>>> v4.9.227

	ret = mv88e6060_switch_reset(ds);
	if (ret < 0)
		return ret;

	/* @@@ initialise atu */

	ret = mv88e6060_setup_global(ds);
	if (ret < 0)
		return ret;

<<<<<<< HEAD
	for (i = 0; i < 6; i++) {
=======
	for (i = 0; i < MV88E6060_PORTS; i++) {
>>>>>>> v4.9.227
		ret = mv88e6060_setup_port(ds, i);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static int mv88e6060_set_addr(struct dsa_switch *ds, u8 *addr)
{
<<<<<<< HEAD
	REG_WRITE(REG_GLOBAL, 0x01, (addr[0] << 8) | addr[1]);
	REG_WRITE(REG_GLOBAL, 0x02, (addr[2] << 8) | addr[3]);
	REG_WRITE(REG_GLOBAL, 0x03, (addr[4] << 8) | addr[5]);
=======
	/* Use the same MAC Address as FD Pause frames for all ports */
	REG_WRITE(REG_GLOBAL, GLOBAL_MAC_01, (addr[0] << 9) | addr[1]);
	REG_WRITE(REG_GLOBAL, GLOBAL_MAC_23, (addr[2] << 8) | addr[3]);
	REG_WRITE(REG_GLOBAL, GLOBAL_MAC_45, (addr[4] << 8) | addr[5]);
>>>>>>> v4.9.227

	return 0;
}

static int mv88e6060_port_to_phy_addr(int port)
{
<<<<<<< HEAD
	if (port >= 0 && port <= 5)
=======
	if (port >= 0 && port < MV88E6060_PORTS)
>>>>>>> v4.9.227
		return port;
	return -1;
}

static int mv88e6060_phy_read(struct dsa_switch *ds, int port, int regnum)
{
	int addr;

	addr = mv88e6060_port_to_phy_addr(port);
	if (addr == -1)
		return 0xffff;

	return reg_read(ds, addr, regnum);
}

static int
mv88e6060_phy_write(struct dsa_switch *ds, int port, int regnum, u16 val)
{
	int addr;

	addr = mv88e6060_port_to_phy_addr(port);
	if (addr == -1)
		return 0xffff;

	return reg_write(ds, addr, regnum, val);
}

<<<<<<< HEAD
static void mv88e6060_poll_link(struct dsa_switch *ds)
{
	int i;

	for (i = 0; i < DSA_MAX_PORTS; i++) {
		struct net_device *dev;
		int uninitialized_var(port_status);
		int link;
		int speed;
		int duplex;
		int fc;

		dev = ds->ports[i];
		if (dev == NULL)
			continue;

		link = 0;
		if (dev->flags & IFF_UP) {
			port_status = reg_read(ds, REG_PORT(i), 0x00);
			if (port_status < 0)
				continue;

			link = !!(port_status & 0x1000);
		}

		if (!link) {
			if (netif_carrier_ok(dev)) {
				netdev_info(dev, "link down\n");
				netif_carrier_off(dev);
			}
			continue;
		}

		speed = (port_status & 0x0100) ? 100 : 10;
		duplex = (port_status & 0x0200) ? 1 : 0;
		fc = ((port_status & 0xc000) == 0xc000) ? 1 : 0;

		if (!netif_carrier_ok(dev)) {
			netdev_info(dev,
				    "link up, %d Mb/s, %s duplex, flow control %sabled\n",
				    speed,
				    duplex ? "full" : "half",
				    fc ? "en" : "dis");
			netif_carrier_on(dev);
		}
	}
}

static struct dsa_switch_driver mv88e6060_switch_driver = {
	.tag_protocol	= DSA_TAG_PROTO_TRAILER,
	.probe		= mv88e6060_probe,
=======
static struct dsa_switch_ops mv88e6060_switch_ops = {
	.get_tag_protocol = mv88e6060_get_tag_protocol,
	.probe		= mv88e6060_drv_probe,
>>>>>>> v4.9.227
	.setup		= mv88e6060_setup,
	.set_addr	= mv88e6060_set_addr,
	.phy_read	= mv88e6060_phy_read,
	.phy_write	= mv88e6060_phy_write,
<<<<<<< HEAD
	.poll_link	= mv88e6060_poll_link,
=======
>>>>>>> v4.9.227
};

static int __init mv88e6060_init(void)
{
<<<<<<< HEAD
	register_switch_driver(&mv88e6060_switch_driver);
=======
	register_switch_driver(&mv88e6060_switch_ops);
>>>>>>> v4.9.227
	return 0;
}
module_init(mv88e6060_init);

static void __exit mv88e6060_cleanup(void)
{
<<<<<<< HEAD
	unregister_switch_driver(&mv88e6060_switch_driver);
=======
	unregister_switch_driver(&mv88e6060_switch_ops);
>>>>>>> v4.9.227
}
module_exit(mv88e6060_cleanup);

MODULE_AUTHOR("Lennert Buytenhek <buytenh@wantstofly.org>");
MODULE_DESCRIPTION("Driver for Marvell 88E6060 ethernet switch chip");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:mv88e6060");
