/*
 * Copyright (c) 2014 Nicira, Inc.
 * Copyright (c) 2013 Cisco Systems, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

<<<<<<< HEAD
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/in.h>
#include <linux/ip.h>
#include <linux/net.h>
#include <linux/rculist.h>
#include <linux/udp.h>

#include <net/icmp.h>
#include <net/ip.h>
#include <net/udp.h>
#include <net/ip_tunnels.h>
#include <net/rtnetlink.h>
#include <net/route.h>
#include <net/dsfield.h>
#include <net/inet_ecn.h>
#include <net/net_namespace.h>
#include <net/netns/generic.h>
=======
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/openvswitch.h>
#include <linux/module.h>
#include <net/udp.h>
#include <net/ip_tunnels.h>
#include <net/rtnetlink.h>
>>>>>>> v4.9.227
#include <net/vxlan.h>

#include "datapath.h"
#include "vport.h"
<<<<<<< HEAD

/**
 * struct vxlan_port - Keeps track of open UDP ports
 * @vs: vxlan_sock created for the port.
 * @name: vport name.
 */
struct vxlan_port {
	struct vxlan_sock *vs;
	char name[IFNAMSIZ];
};

static inline struct vxlan_port *vxlan_vport(const struct vport *vport)
{
	return vport_priv(vport);
}

/* Called with rcu_read_lock and BH disabled. */
static void vxlan_rcv(struct vxlan_sock *vs, struct sk_buff *skb, __be32 vx_vni)
{
	struct ovs_tunnel_info tun_info;
	struct vport *vport = vs->data;
	struct iphdr *iph;
	__be64 key;

	/* Save outer tunnel values */
	iph = ip_hdr(skb);
	key = cpu_to_be64(ntohl(vx_vni) >> 8);
	ovs_flow_tun_info_init(&tun_info, iph, key, TUNNEL_KEY, NULL, 0);

	ovs_vport_receive(vport, skb, &tun_info);
}

static int vxlan_get_options(const struct vport *vport, struct sk_buff *skb)
{
	struct vxlan_port *vxlan_port = vxlan_vport(vport);
	__be16 dst_port = inet_sk(vxlan_port->vs->sock->sk)->inet_sport;

	if (nla_put_u16(skb, OVS_TUNNEL_ATTR_DST_PORT, ntohs(dst_port)))
		return -EMSGSIZE;
	return 0;
}

static void vxlan_tnl_destroy(struct vport *vport)
{
	struct vxlan_port *vxlan_port = vxlan_vport(vport);

	vxlan_sock_release(vxlan_port->vs);

	ovs_vport_deferred_free(vport);
=======
#include "vport-netdev.h"

static struct vport_ops ovs_vxlan_netdev_vport_ops;

static int vxlan_get_options(const struct vport *vport, struct sk_buff *skb)
{
	struct vxlan_dev *vxlan = netdev_priv(vport->dev);
	__be16 dst_port = vxlan->cfg.dst_port;

	if (nla_put_u16(skb, OVS_TUNNEL_ATTR_DST_PORT, ntohs(dst_port)))
		return -EMSGSIZE;

	if (vxlan->flags & VXLAN_F_GBP) {
		struct nlattr *exts;

		exts = nla_nest_start(skb, OVS_TUNNEL_ATTR_EXTENSION);
		if (!exts)
			return -EMSGSIZE;

		if (vxlan->flags & VXLAN_F_GBP &&
		    nla_put_flag(skb, OVS_VXLAN_EXT_GBP))
			return -EMSGSIZE;

		nla_nest_end(skb, exts);
	}

	return 0;
}

static const struct nla_policy exts_policy[OVS_VXLAN_EXT_MAX + 1] = {
	[OVS_VXLAN_EXT_GBP]	= { .type = NLA_FLAG, },
};

static int vxlan_configure_exts(struct vport *vport, struct nlattr *attr,
				struct vxlan_config *conf)
{
	struct nlattr *exts[OVS_VXLAN_EXT_MAX + 1];
	int err;

	if (nla_len(attr) < sizeof(struct nlattr))
		return -EINVAL;

	err = nla_parse_nested(exts, OVS_VXLAN_EXT_MAX, attr, exts_policy);
	if (err < 0)
		return err;

	if (exts[OVS_VXLAN_EXT_GBP])
		conf->flags |= VXLAN_F_GBP;

	return 0;
>>>>>>> v4.9.227
}

static struct vport *vxlan_tnl_create(const struct vport_parms *parms)
{
	struct net *net = ovs_dp_get_net(parms->dp);
	struct nlattr *options = parms->options;
<<<<<<< HEAD
	struct vxlan_port *vxlan_port;
	struct vxlan_sock *vs;
	struct vport *vport;
	struct nlattr *a;
	u16 dst_port;
	int err;
=======
	struct net_device *dev;
	struct vport *vport;
	struct nlattr *a;
	int err;
	struct vxlan_config conf = {
		.no_share = true,
		.flags = VXLAN_F_COLLECT_METADATA | VXLAN_F_UDP_ZERO_CSUM6_RX,
		/* Don't restrict the packets that can be sent by MTU */
		.mtu = IP_MAX_MTU,
	};
>>>>>>> v4.9.227

	if (!options) {
		err = -EINVAL;
		goto error;
	}
<<<<<<< HEAD
	a = nla_find_nested(options, OVS_TUNNEL_ATTR_DST_PORT);
	if (a && nla_len(a) == sizeof(u16)) {
		dst_port = nla_get_u16(a);
=======

	a = nla_find_nested(options, OVS_TUNNEL_ATTR_DST_PORT);
	if (a && nla_len(a) == sizeof(u16)) {
		conf.dst_port = htons(nla_get_u16(a));
>>>>>>> v4.9.227
	} else {
		/* Require destination port from userspace. */
		err = -EINVAL;
		goto error;
	}

<<<<<<< HEAD
	vport = ovs_vport_alloc(sizeof(struct vxlan_port),
				&ovs_vxlan_vport_ops, parms);
	if (IS_ERR(vport))
		return vport;

	vxlan_port = vxlan_vport(vport);
	strncpy(vxlan_port->name, parms->name, IFNAMSIZ);

	vs = vxlan_sock_add(net, htons(dst_port), vxlan_rcv, vport, true, 0);
	if (IS_ERR(vs)) {
		ovs_vport_free(vport);
		return (void *)vs;
	}
	vxlan_port->vs = vs;

	return vport;

=======
	vport = ovs_vport_alloc(0, &ovs_vxlan_netdev_vport_ops, parms);
	if (IS_ERR(vport))
		return vport;

	a = nla_find_nested(options, OVS_TUNNEL_ATTR_EXTENSION);
	if (a) {
		err = vxlan_configure_exts(vport, a, &conf);
		if (err) {
			ovs_vport_free(vport);
			goto error;
		}
	}

	rtnl_lock();
	dev = vxlan_dev_create(net, parms->name, NET_NAME_USER, &conf);
	if (IS_ERR(dev)) {
		rtnl_unlock();
		ovs_vport_free(vport);
		return ERR_CAST(dev);
	}

	err = dev_change_flags(dev, dev->flags | IFF_UP);
	if (err < 0) {
		rtnl_delete_link(dev);
		rtnl_unlock();
		ovs_vport_free(vport);
		goto error;
	}

	rtnl_unlock();
	return vport;
>>>>>>> v4.9.227
error:
	return ERR_PTR(err);
}

<<<<<<< HEAD
static int vxlan_tnl_send(struct vport *vport, struct sk_buff *skb)
{
	struct net *net = ovs_dp_get_net(vport->dp);
	struct vxlan_port *vxlan_port = vxlan_vport(vport);
	__be16 dst_port = inet_sk(vxlan_port->vs->sock->sk)->inet_sport;
	struct ovs_key_ipv4_tunnel *tun_key;
	struct rtable *rt;
	struct flowi4 fl;
	__be16 src_port;
	__be16 df;
	int err;

	if (unlikely(!OVS_CB(skb)->egress_tun_info)) {
		err = -EINVAL;
		goto error;
	}

	tun_key = &OVS_CB(skb)->egress_tun_info->tunnel;
	/* Route lookup */
	memset(&fl, 0, sizeof(fl));
	fl.daddr = tun_key->ipv4_dst;
	fl.saddr = tun_key->ipv4_src;
	fl.flowi4_tos = RT_TOS(tun_key->ipv4_tos);
	fl.flowi4_mark = skb->mark;
	fl.flowi4_proto = IPPROTO_UDP;

	rt = ip_route_output_key(net, &fl);
	if (IS_ERR(rt)) {
		err = PTR_ERR(rt);
		goto error;
	}

	df = tun_key->tun_flags & TUNNEL_DONT_FRAGMENT ?
		htons(IP_DF) : 0;

	skb->ignore_df = 1;

	src_port = udp_flow_src_port(net, skb, 0, 0, true);

	err = vxlan_xmit_skb(vxlan_port->vs, rt, skb,
			     fl.saddr, tun_key->ipv4_dst,
			     tun_key->ipv4_tos, tun_key->ipv4_ttl, df,
			     src_port, dst_port,
			     htonl(be64_to_cpu(tun_key->tun_id) << 8),
			     false);
	if (err < 0)
		ip_rt_put(rt);
error:
	return err;
}

static const char *vxlan_get_name(const struct vport *vport)
{
	struct vxlan_port *vxlan_port = vxlan_vport(vport);
	return vxlan_port->name;
}

const struct vport_ops ovs_vxlan_vport_ops = {
	.type		= OVS_VPORT_TYPE_VXLAN,
	.create		= vxlan_tnl_create,
	.destroy	= vxlan_tnl_destroy,
	.get_name	= vxlan_get_name,
	.get_options	= vxlan_get_options,
	.send		= vxlan_tnl_send,
};
=======
static struct vport *vxlan_create(const struct vport_parms *parms)
{
	struct vport *vport;

	vport = vxlan_tnl_create(parms);
	if (IS_ERR(vport))
		return vport;

	return ovs_netdev_link(vport, parms->name);
}

static struct vport_ops ovs_vxlan_netdev_vport_ops = {
	.type			= OVS_VPORT_TYPE_VXLAN,
	.create			= vxlan_create,
	.destroy		= ovs_netdev_tunnel_destroy,
	.get_options		= vxlan_get_options,
	.send			= dev_queue_xmit,
};

static int __init ovs_vxlan_tnl_init(void)
{
	return ovs_vport_ops_register(&ovs_vxlan_netdev_vport_ops);
}

static void __exit ovs_vxlan_tnl_exit(void)
{
	ovs_vport_ops_unregister(&ovs_vxlan_netdev_vport_ops);
}

module_init(ovs_vxlan_tnl_init);
module_exit(ovs_vxlan_tnl_exit);

MODULE_DESCRIPTION("OVS: VXLAN switching port");
MODULE_LICENSE("GPL");
MODULE_ALIAS("vport-type-4");
>>>>>>> v4.9.227
