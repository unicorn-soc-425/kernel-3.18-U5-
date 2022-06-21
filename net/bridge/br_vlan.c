#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <linux/slab.h>
<<<<<<< HEAD

#include "br_private.h"

static void __vlan_add_pvid(struct net_port_vlans *v, u16 vid)
{
	if (v->pvid == vid)
		return;

	smp_wmb();
	v->pvid = vid;
}

static void __vlan_delete_pvid(struct net_port_vlans *v, u16 vid)
{
	if (v->pvid != vid)
		return;

	smp_wmb();
	v->pvid = 0;
}

static void __vlan_add_flags(struct net_port_vlans *v, u16 vid, u16 flags)
{
	if (flags & BRIDGE_VLAN_INFO_PVID)
		__vlan_add_pvid(v, vid);
	else
		__vlan_delete_pvid(v, vid);

	if (flags & BRIDGE_VLAN_INFO_UNTAGGED)
		set_bit(vid, v->untagged_bitmap);
	else
		clear_bit(vid, v->untagged_bitmap);
}

static int __vlan_add(struct net_port_vlans *v, u16 vid, u16 flags)
{
	struct net_bridge_port *p = NULL;
	struct net_bridge *br;
	struct net_device *dev;
	int err;

	if (test_bit(vid, v->vlan_bitmap)) {
		__vlan_add_flags(v, vid, flags);
		return 0;
	}

	if (v->port_idx) {
		p = v->parent.port;
		br = p->br;
		dev = p->dev;
	} else {
		br = v->parent.br;
		dev = br->dev;
=======
#include <net/switchdev.h>

#include "br_private.h"

static inline int br_vlan_cmp(struct rhashtable_compare_arg *arg,
			      const void *ptr)
{
	const struct net_bridge_vlan *vle = ptr;
	u16 vid = *(u16 *)arg->key;

	return vle->vid != vid;
}

static const struct rhashtable_params br_vlan_rht_params = {
	.head_offset = offsetof(struct net_bridge_vlan, vnode),
	.key_offset = offsetof(struct net_bridge_vlan, vid),
	.key_len = sizeof(u16),
	.nelem_hint = 3,
	.locks_mul = 1,
	.max_size = VLAN_N_VID,
	.obj_cmpfn = br_vlan_cmp,
	.automatic_shrinking = true,
};

static struct net_bridge_vlan *br_vlan_lookup(struct rhashtable *tbl, u16 vid)
{
	return rhashtable_lookup_fast(tbl, &vid, br_vlan_rht_params);
}

static void __vlan_add_pvid(struct net_bridge_vlan_group *vg, u16 vid)
{
	if (vg->pvid == vid)
		return;

	smp_wmb();
	vg->pvid = vid;
}

static void __vlan_delete_pvid(struct net_bridge_vlan_group *vg, u16 vid)
{
	if (vg->pvid != vid)
		return;

	smp_wmb();
	vg->pvid = 0;
}

static void __vlan_add_flags(struct net_bridge_vlan *v, u16 flags)
{
	struct net_bridge_vlan_group *vg;

	if (br_vlan_is_master(v))
		vg = br_vlan_group(v->br);
	else
		vg = nbp_vlan_group(v->port);

	if (flags & BRIDGE_VLAN_INFO_PVID)
		__vlan_add_pvid(vg, v->vid);
	else
		__vlan_delete_pvid(vg, v->vid);

	if (flags & BRIDGE_VLAN_INFO_UNTAGGED)
		v->flags |= BRIDGE_VLAN_INFO_UNTAGGED;
	else
		v->flags &= ~BRIDGE_VLAN_INFO_UNTAGGED;
}

static int __vlan_vid_add(struct net_device *dev, struct net_bridge *br,
			  u16 vid, u16 flags)
{
	struct switchdev_obj_port_vlan v = {
		.obj.orig_dev = dev,
		.obj.id = SWITCHDEV_OBJ_ID_PORT_VLAN,
		.flags = flags,
		.vid_begin = vid,
		.vid_end = vid,
	};
	int err;

	/* Try switchdev op first. In case it is not supported, fallback to
	 * 8021q add.
	 */
	err = switchdev_port_obj_add(dev, &v.obj);
	if (err == -EOPNOTSUPP)
		return vlan_vid_add(dev, br->vlan_proto, vid);
	return err;
}

static void __vlan_add_list(struct net_bridge_vlan *v)
{
	struct net_bridge_vlan_group *vg;
	struct list_head *headp, *hpos;
	struct net_bridge_vlan *vent;

	if (br_vlan_is_master(v))
		vg = br_vlan_group(v->br);
	else
		vg = nbp_vlan_group(v->port);

	headp = &vg->vlan_list;
	list_for_each_prev(hpos, headp) {
		vent = list_entry(hpos, struct net_bridge_vlan, vlist);
		if (v->vid < vent->vid)
			continue;
		else
			break;
	}
	list_add_rcu(&v->vlist, hpos);
}

static void __vlan_del_list(struct net_bridge_vlan *v)
{
	list_del_rcu(&v->vlist);
}

static int __vlan_vid_del(struct net_device *dev, struct net_bridge *br,
			  u16 vid)
{
	struct switchdev_obj_port_vlan v = {
		.obj.orig_dev = dev,
		.obj.id = SWITCHDEV_OBJ_ID_PORT_VLAN,
		.vid_begin = vid,
		.vid_end = vid,
	};
	int err;

	/* Try switchdev op first. In case it is not supported, fallback to
	 * 8021q del.
	 */
	err = switchdev_port_obj_del(dev, &v.obj);
	if (err == -EOPNOTSUPP) {
		vlan_vid_del(dev, br->vlan_proto, vid);
		return 0;
	}
	return err;
}

/* Returns a master vlan, if it didn't exist it gets created. In all cases a
 * a reference is taken to the master vlan before returning.
 */
static struct net_bridge_vlan *br_vlan_get_master(struct net_bridge *br, u16 vid)
{
	struct net_bridge_vlan_group *vg;
	struct net_bridge_vlan *masterv;

	vg = br_vlan_group(br);
	masterv = br_vlan_find(vg, vid);
	if (!masterv) {
		/* missing global ctx, create it now */
		if (br_vlan_add(br, vid, 0))
			return NULL;
		masterv = br_vlan_find(vg, vid);
		if (WARN_ON(!masterv))
			return NULL;
	}
	atomic_inc(&masterv->refcnt);

	return masterv;
}

static void br_master_vlan_rcu_free(struct rcu_head *rcu)
{
	struct net_bridge_vlan *v;

	v = container_of(rcu, struct net_bridge_vlan, rcu);
	WARN_ON(!br_vlan_is_master(v));
	free_percpu(v->stats);
	v->stats = NULL;
	kfree(v);
}

static void br_vlan_put_master(struct net_bridge_vlan *masterv)
{
	struct net_bridge_vlan_group *vg;

	if (!br_vlan_is_master(masterv))
		return;

	vg = br_vlan_group(masterv->br);
	if (atomic_dec_and_test(&masterv->refcnt)) {
		rhashtable_remove_fast(&vg->vlan_hash,
				       &masterv->vnode, br_vlan_rht_params);
		__vlan_del_list(masterv);
		call_rcu(&masterv->rcu, br_master_vlan_rcu_free);
	}
}

/* This is the shared VLAN add function which works for both ports and bridge
 * devices. There are four possible calls to this function in terms of the
 * vlan entry type:
 * 1. vlan is being added on a port (no master flags, global entry exists)
 * 2. vlan is being added on a bridge (both master and brentry flags)
 * 3. vlan is being added on a port, but a global entry didn't exist which
 *    is being created right now (master flag set, brentry flag unset), the
 *    global entry is used for global per-vlan features, but not for filtering
 * 4. same as 3 but with both master and brentry flags set so the entry
 *    will be used for filtering in both the port and the bridge
 */
static int __vlan_add(struct net_bridge_vlan *v, u16 flags)
{
	struct net_bridge_vlan *masterv = NULL;
	struct net_bridge_port *p = NULL;
	struct net_bridge_vlan_group *vg;
	struct net_device *dev;
	struct net_bridge *br;
	int err;

	if (br_vlan_is_master(v)) {
		br = v->br;
		dev = br->dev;
		vg = br_vlan_group(br);
	} else {
		p = v->port;
		br = p->br;
		dev = p->dev;
		vg = nbp_vlan_group(p);
>>>>>>> v4.9.227
	}

	if (p) {
		/* Add VLAN to the device filter if it is supported.
		 * This ensures tagged traffic enters the bridge when
		 * promiscuous mode is disabled by br_manage_promisc().
		 */
<<<<<<< HEAD
		err = vlan_vid_add(dev, br->vlan_proto, vid);
		if (err)
			return err;
	}

	err = br_fdb_insert(br, p, dev->dev_addr, vid);
	if (err) {
		br_err(br, "failed insert local address into bridge "
		       "forwarding table\n");
		goto out_filt;
	}

	set_bit(vid, v->vlan_bitmap);
	v->num_vlans++;
	__vlan_add_flags(v, vid, flags);

	return 0;

out_filt:
	if (p)
		vlan_vid_del(dev, br->vlan_proto, vid);
	return err;
}

static int __vlan_del(struct net_port_vlans *v, u16 vid)
{
	if (!test_bit(vid, v->vlan_bitmap))
		return -EINVAL;

	__vlan_delete_pvid(v, vid);
	clear_bit(vid, v->untagged_bitmap);

	if (v->port_idx) {
		struct net_bridge_port *p = v->parent.port;
		vlan_vid_del(p->dev, p->br->vlan_proto, vid);
	}

	clear_bit(vid, v->vlan_bitmap);
	v->num_vlans--;
	if (bitmap_empty(v->vlan_bitmap, VLAN_N_VID)) {
		if (v->port_idx)
			RCU_INIT_POINTER(v->parent.port->vlan_info, NULL);
		else
			RCU_INIT_POINTER(v->parent.br->vlan_info, NULL);
		kfree_rcu(v, rcu);
	}
	return 0;
}

static void __vlan_flush(struct net_port_vlans *v)
{
	smp_wmb();
	v->pvid = 0;
	bitmap_zero(v->vlan_bitmap, VLAN_N_VID);
	if (v->port_idx)
		RCU_INIT_POINTER(v->parent.port->vlan_info, NULL);
	else
		RCU_INIT_POINTER(v->parent.br->vlan_info, NULL);
	kfree_rcu(v, rcu);
}

struct sk_buff *br_handle_vlan(struct net_bridge *br,
			       const struct net_port_vlans *pv,
			       struct sk_buff *skb)
{
=======
		err = __vlan_vid_add(dev, br, v->vid, flags);
		if (err)
			goto out;

		/* need to work on the master vlan too */
		if (flags & BRIDGE_VLAN_INFO_MASTER) {
			err = br_vlan_add(br, v->vid, flags |
						      BRIDGE_VLAN_INFO_BRENTRY);
			if (err)
				goto out_filt;
		}

		masterv = br_vlan_get_master(br, v->vid);
		if (!masterv)
			goto out_filt;
		v->brvlan = masterv;
		v->stats = masterv->stats;
	}

	/* Add the dev mac and count the vlan only if it's usable */
	if (br_vlan_should_use(v)) {
		err = br_fdb_insert(br, p, dev->dev_addr, v->vid);
		if (err) {
			br_err(br, "failed insert local address into bridge forwarding table\n");
			goto out_filt;
		}
		vg->num_vlans++;
	}

	err = rhashtable_lookup_insert_fast(&vg->vlan_hash, &v->vnode,
					    br_vlan_rht_params);
	if (err)
		goto out_fdb_insert;

	__vlan_add_list(v);
	__vlan_add_flags(v, flags);
out:
	return err;

out_fdb_insert:
	if (br_vlan_should_use(v)) {
		br_fdb_find_delete_local(br, p, dev->dev_addr, v->vid);
		vg->num_vlans--;
	}

out_filt:
	if (p) {
		__vlan_vid_del(dev, br, v->vid);
		if (masterv) {
			br_vlan_put_master(masterv);
			v->brvlan = NULL;
		}
	}

	goto out;
}

static int __vlan_del(struct net_bridge_vlan *v)
{
	struct net_bridge_vlan *masterv = v;
	struct net_bridge_vlan_group *vg;
	struct net_bridge_port *p = NULL;
	int err = 0;

	if (br_vlan_is_master(v)) {
		vg = br_vlan_group(v->br);
	} else {
		p = v->port;
		vg = nbp_vlan_group(v->port);
		masterv = v->brvlan;
	}

	__vlan_delete_pvid(vg, v->vid);
	if (p) {
		err = __vlan_vid_del(p->dev, p->br, v->vid);
		if (err)
			goto out;
	}

	if (br_vlan_should_use(v)) {
		v->flags &= ~BRIDGE_VLAN_INFO_BRENTRY;
		vg->num_vlans--;
	}

	if (masterv != v) {
		rhashtable_remove_fast(&vg->vlan_hash, &v->vnode,
				       br_vlan_rht_params);
		__vlan_del_list(v);
		kfree_rcu(v, rcu);
	}

	br_vlan_put_master(masterv);
out:
	return err;
}

static void __vlan_group_free(struct net_bridge_vlan_group *vg)
{
	WARN_ON(!list_empty(&vg->vlan_list));
	rhashtable_destroy(&vg->vlan_hash);
	kfree(vg);
}

static void __vlan_flush(struct net_bridge_vlan_group *vg)
{
	struct net_bridge_vlan *vlan, *tmp;

	__vlan_delete_pvid(vg, vg->pvid);
	list_for_each_entry_safe(vlan, tmp, &vg->vlan_list, vlist)
		__vlan_del(vlan);
}

struct sk_buff *br_handle_vlan(struct net_bridge *br,
			       struct net_bridge_vlan_group *vg,
			       struct sk_buff *skb)
{
	struct br_vlan_stats *stats;
	struct net_bridge_vlan *v;
>>>>>>> v4.9.227
	u16 vid;

	/* If this packet was not filtered at input, let it pass */
	if (!BR_INPUT_SKB_CB(skb)->vlan_filtered)
		goto out;

<<<<<<< HEAD
	/* Vlan filter table must be configured at this point.  The
=======
	/* At this point, we know that the frame was filtered and contains
	 * a valid vlan id.  If the vlan id has untagged flag set,
	 * send untagged; otherwise, send tagged.
	 */
	br_vlan_get_tag(skb, &vid);
	v = br_vlan_find(vg, vid);
	/* Vlan entry must be configured at this point.  The
>>>>>>> v4.9.227
	 * only exception is the bridge is set in promisc mode and the
	 * packet is destined for the bridge device.  In this case
	 * pass the packet as is.
	 */
<<<<<<< HEAD
	if (!pv) {
=======
	if (!v || !br_vlan_should_use(v)) {
>>>>>>> v4.9.227
		if ((br->dev->flags & IFF_PROMISC) && skb->dev == br->dev) {
			goto out;
		} else {
			kfree_skb(skb);
			return NULL;
		}
	}
<<<<<<< HEAD

	/* At this point, we know that the frame was filtered and contains
	 * a valid vlan id.  If the vlan id is set in the untagged bitmap,
	 * send untagged; otherwise, send tagged.
	 */
	br_vlan_get_tag(skb, &vid);
	if (test_bit(vid, pv->untagged_bitmap))
		skb->vlan_tci = 0;

=======
	if (br->vlan_stats_enabled) {
		stats = this_cpu_ptr(v->stats);
		u64_stats_update_begin(&stats->syncp);
		stats->tx_bytes += skb->len;
		stats->tx_packets++;
		u64_stats_update_end(&stats->syncp);
	}

	if (v->flags & BRIDGE_VLAN_INFO_UNTAGGED)
		skb->vlan_tci = 0;
>>>>>>> v4.9.227
out:
	return skb;
}

/* Called under RCU */
<<<<<<< HEAD
bool br_allowed_ingress(struct net_bridge *br, struct net_port_vlans *v,
			struct sk_buff *skb, u16 *vid)
{
	bool tagged;
	__be16 proto;

	/* If VLAN filtering is disabled on the bridge, all packets are
	 * permitted.
	 */
	if (!br->vlan_enabled) {
		BR_INPUT_SKB_CB(skb)->vlan_filtered = false;
		return true;
	}

	/* If there are no vlan in the permitted list, all packets are
	 * rejected.
	 */
	if (!v)
		goto drop;

	BR_INPUT_SKB_CB(skb)->vlan_filtered = true;
	proto = br->vlan_proto;

=======
static bool __allowed_ingress(const struct net_bridge *br,
			      struct net_bridge_vlan_group *vg,
			      struct sk_buff *skb, u16 *vid)
{
	struct br_vlan_stats *stats;
	struct net_bridge_vlan *v;
	bool tagged;

	BR_INPUT_SKB_CB(skb)->vlan_filtered = true;
>>>>>>> v4.9.227
	/* If vlan tx offload is disabled on bridge device and frame was
	 * sent from vlan device on the bridge device, it does not have
	 * HW accelerated vlan tag.
	 */
<<<<<<< HEAD
	if (unlikely(!vlan_tx_tag_present(skb) &&
		     skb->protocol == proto)) {
=======
	if (unlikely(!skb_vlan_tag_present(skb) &&
		     skb->protocol == br->vlan_proto)) {
>>>>>>> v4.9.227
		skb = skb_vlan_untag(skb);
		if (unlikely(!skb))
			return false;
	}

	if (!br_vlan_get_tag(skb, vid)) {
		/* Tagged frame */
<<<<<<< HEAD
		if (skb->vlan_proto != proto) {
			/* Protocol-mismatch, empty out vlan_tci for new tag */
			skb_push(skb, ETH_HLEN);
			skb = vlan_insert_tag_set_proto(skb, skb->vlan_proto,
							vlan_tx_tag_get(skb));
=======
		if (skb->vlan_proto != br->vlan_proto) {
			/* Protocol-mismatch, empty out vlan_tci for new tag */
			skb_push(skb, ETH_HLEN);
			skb = vlan_insert_tag_set_proto(skb, skb->vlan_proto,
							skb_vlan_tag_get(skb));
>>>>>>> v4.9.227
			if (unlikely(!skb))
				return false;

			skb_pull(skb, ETH_HLEN);
			skb_reset_mac_len(skb);
			*vid = 0;
			tagged = false;
		} else {
			tagged = true;
		}
	} else {
		/* Untagged frame */
		tagged = false;
	}

	if (!*vid) {
<<<<<<< HEAD
		u16 pvid = br_get_pvid(v);
=======
		u16 pvid = br_get_pvid(vg);
>>>>>>> v4.9.227

		/* Frame had a tag with VID 0 or did not have a tag.
		 * See if pvid is set on this port.  That tells us which
		 * vlan untagged or priority-tagged traffic belongs to.
		 */
		if (!pvid)
			goto drop;

		/* PVID is set on this port.  Any untagged or priority-tagged
		 * ingress frame is considered to belong to this vlan.
		 */
		*vid = pvid;
		if (likely(!tagged))
			/* Untagged Frame. */
<<<<<<< HEAD
			__vlan_hwaccel_put_tag(skb, proto, pvid);
=======
			__vlan_hwaccel_put_tag(skb, br->vlan_proto, pvid);
>>>>>>> v4.9.227
		else
			/* Priority-tagged Frame.
			 * At this point, We know that skb->vlan_tci had
			 * VLAN_TAG_PRESENT bit and its VID field was 0x000.
			 * We update only VID field and preserve PCP field.
			 */
			skb->vlan_tci |= pvid;

<<<<<<< HEAD
		return true;
	}

	/* Frame had a valid vlan tag.  See if vlan is allowed */
	if (test_bit(*vid, v->vlan_bitmap))
		return true;
=======
		/* if stats are disabled we can avoid the lookup */
		if (!br->vlan_stats_enabled)
			return true;
	}
	v = br_vlan_find(vg, *vid);
	if (!v || !br_vlan_should_use(v))
		goto drop;

	if (br->vlan_stats_enabled) {
		stats = this_cpu_ptr(v->stats);
		u64_stats_update_begin(&stats->syncp);
		stats->rx_bytes += skb->len;
		stats->rx_packets++;
		u64_stats_update_end(&stats->syncp);
	}

	return true;

>>>>>>> v4.9.227
drop:
	kfree_skb(skb);
	return false;
}

<<<<<<< HEAD
/* Called under RCU. */
bool br_allowed_egress(struct net_bridge *br,
		       const struct net_port_vlans *v,
		       const struct sk_buff *skb)
{
=======
bool br_allowed_ingress(const struct net_bridge *br,
			struct net_bridge_vlan_group *vg, struct sk_buff *skb,
			u16 *vid)
{
	/* If VLAN filtering is disabled on the bridge, all packets are
	 * permitted.
	 */
	if (!br->vlan_enabled) {
		BR_INPUT_SKB_CB(skb)->vlan_filtered = false;
		return true;
	}

	return __allowed_ingress(br, vg, skb, vid);
}

/* Called under RCU. */
bool br_allowed_egress(struct net_bridge_vlan_group *vg,
		       const struct sk_buff *skb)
{
	const struct net_bridge_vlan *v;
>>>>>>> v4.9.227
	u16 vid;

	/* If this packet was not filtered at input, let it pass */
	if (!BR_INPUT_SKB_CB(skb)->vlan_filtered)
		return true;

<<<<<<< HEAD
	if (!v)
		return false;

	br_vlan_get_tag(skb, &vid);
	if (test_bit(vid, v->vlan_bitmap))
=======
	br_vlan_get_tag(skb, &vid);
	v = br_vlan_find(vg, vid);
	if (v && br_vlan_should_use(v))
>>>>>>> v4.9.227
		return true;

	return false;
}

/* Called under RCU */
bool br_should_learn(struct net_bridge_port *p, struct sk_buff *skb, u16 *vid)
{
<<<<<<< HEAD
	struct net_bridge *br = p->br;
	struct net_port_vlans *v;
=======
	struct net_bridge_vlan_group *vg;
	struct net_bridge *br = p->br;
>>>>>>> v4.9.227

	/* If filtering was disabled at input, let it pass. */
	if (!br->vlan_enabled)
		return true;

<<<<<<< HEAD
	v = rcu_dereference(p->vlan_info);
	if (!v)
=======
	vg = nbp_vlan_group_rcu(p);
	if (!vg || !vg->num_vlans)
>>>>>>> v4.9.227
		return false;

	if (!br_vlan_get_tag(skb, vid) && skb->vlan_proto != br->vlan_proto)
		*vid = 0;

	if (!*vid) {
<<<<<<< HEAD
		*vid = br_get_pvid(v);
=======
		*vid = br_get_pvid(vg);
>>>>>>> v4.9.227
		if (!*vid)
			return false;

		return true;
	}

<<<<<<< HEAD
	if (test_bit(*vid, v->vlan_bitmap))
=======
	if (br_vlan_find(vg, *vid))
>>>>>>> v4.9.227
		return true;

	return false;
}

/* Must be protected by RTNL.
 * Must be called with vid in range from 1 to 4094 inclusive.
 */
int br_vlan_add(struct net_bridge *br, u16 vid, u16 flags)
{
<<<<<<< HEAD
	struct net_port_vlans *pv = NULL;
	int err;

	ASSERT_RTNL();

	pv = rtnl_dereference(br->vlan_info);
	if (pv)
		return __vlan_add(pv, vid, flags);

	/* Create port vlan infomration
	 */
	pv = kzalloc(sizeof(*pv), GFP_KERNEL);
	if (!pv)
		return -ENOMEM;

	pv->parent.br = br;
	err = __vlan_add(pv, vid, flags);
	if (err)
		goto out;

	rcu_assign_pointer(br->vlan_info, pv);
	return 0;
out:
	kfree(pv);
	return err;
=======
	struct net_bridge_vlan_group *vg;
	struct net_bridge_vlan *vlan;
	int ret;

	ASSERT_RTNL();

	vg = br_vlan_group(br);
	vlan = br_vlan_find(vg, vid);
	if (vlan) {
		if (!br_vlan_is_brentry(vlan)) {
			/* Trying to change flags of non-existent bridge vlan */
			if (!(flags & BRIDGE_VLAN_INFO_BRENTRY))
				return -EINVAL;
			/* It was only kept for port vlans, now make it real */
			ret = br_fdb_insert(br, NULL, br->dev->dev_addr,
					    vlan->vid);
			if (ret) {
				br_err(br, "failed insert local address into bridge forwarding table\n");
				return ret;
			}
			atomic_inc(&vlan->refcnt);
			vlan->flags |= BRIDGE_VLAN_INFO_BRENTRY;
			vg->num_vlans++;
		}
		__vlan_add_flags(vlan, flags);
		return 0;
	}

	vlan = kzalloc(sizeof(*vlan), GFP_KERNEL);
	if (!vlan)
		return -ENOMEM;

	vlan->stats = netdev_alloc_pcpu_stats(struct br_vlan_stats);
	if (!vlan->stats) {
		kfree(vlan);
		return -ENOMEM;
	}
	vlan->vid = vid;
	vlan->flags = flags | BRIDGE_VLAN_INFO_MASTER;
	vlan->flags &= ~BRIDGE_VLAN_INFO_PVID;
	vlan->br = br;
	if (flags & BRIDGE_VLAN_INFO_BRENTRY)
		atomic_set(&vlan->refcnt, 1);
	ret = __vlan_add(vlan, flags);
	if (ret) {
		free_percpu(vlan->stats);
		kfree(vlan);
	}

	return ret;
>>>>>>> v4.9.227
}

/* Must be protected by RTNL.
 * Must be called with vid in range from 1 to 4094 inclusive.
 */
int br_vlan_delete(struct net_bridge *br, u16 vid)
{
<<<<<<< HEAD
	struct net_port_vlans *pv;

	ASSERT_RTNL();

	pv = rtnl_dereference(br->vlan_info);
	if (!pv)
		return -EINVAL;

	br_fdb_find_delete_local(br, NULL, br->dev->dev_addr, vid);

	__vlan_del(pv, vid);
	return 0;
=======
	struct net_bridge_vlan_group *vg;
	struct net_bridge_vlan *v;

	ASSERT_RTNL();

	vg = br_vlan_group(br);
	v = br_vlan_find(vg, vid);
	if (!v || !br_vlan_is_brentry(v))
		return -ENOENT;

	br_fdb_find_delete_local(br, NULL, br->dev->dev_addr, vid);
	br_fdb_delete_by_port(br, NULL, vid, 0);

	return __vlan_del(v);
>>>>>>> v4.9.227
}

void br_vlan_flush(struct net_bridge *br)
{
<<<<<<< HEAD
	struct net_port_vlans *pv;

	ASSERT_RTNL();
	pv = rtnl_dereference(br->vlan_info);
	if (!pv)
		return;

	__vlan_flush(pv);
}

bool br_vlan_find(struct net_bridge *br, u16 vid)
{
	struct net_port_vlans *pv;
	bool found = false;

	rcu_read_lock();
	pv = rcu_dereference(br->vlan_info);

	if (!pv)
		goto out;

	if (test_bit(vid, pv->vlan_bitmap))
		found = true;

out:
	rcu_read_unlock();
	return found;
=======
	struct net_bridge_vlan_group *vg;

	ASSERT_RTNL();

	/* delete auto-added default pvid local fdb before flushing vlans
	 * otherwise it will be leaked on bridge device init failure
	 */
	br_fdb_delete_by_port(br, NULL, 0, 1);

	vg = br_vlan_group(br);
	__vlan_flush(vg);
	RCU_INIT_POINTER(br->vlgrp, NULL);
	synchronize_rcu();
	__vlan_group_free(vg);
}

struct net_bridge_vlan *br_vlan_find(struct net_bridge_vlan_group *vg, u16 vid)
{
	if (!vg)
		return NULL;

	return br_vlan_lookup(&vg->vlan_hash, vid);
>>>>>>> v4.9.227
}

/* Must be protected by RTNL. */
static void recalculate_group_addr(struct net_bridge *br)
{
	if (br->group_addr_set)
		return;

	spin_lock_bh(&br->lock);
	if (!br->vlan_enabled || br->vlan_proto == htons(ETH_P_8021Q)) {
		/* Bridge Group Address */
		br->group_addr[5] = 0x00;
	} else { /* vlan_enabled && ETH_P_8021AD */
		/* Provider Bridge Group Address */
		br->group_addr[5] = 0x08;
	}
	spin_unlock_bh(&br->lock);
}

/* Must be protected by RTNL. */
void br_recalculate_fwd_mask(struct net_bridge *br)
{
	if (!br->vlan_enabled || br->vlan_proto == htons(ETH_P_8021Q))
		br->group_fwd_mask_required = BR_GROUPFWD_DEFAULT;
	else /* vlan_enabled && ETH_P_8021AD */
		br->group_fwd_mask_required = BR_GROUPFWD_8021AD &
					      ~(1u << br->group_addr[5]);
}

<<<<<<< HEAD
int br_vlan_filter_toggle(struct net_bridge *br, unsigned long val)
{
	if (!rtnl_trylock())
		return restart_syscall();

	if (br->vlan_enabled == val)
		goto unlock;
=======
int __br_vlan_filter_toggle(struct net_bridge *br, unsigned long val)
{
	struct switchdev_attr attr = {
		.orig_dev = br->dev,
		.id = SWITCHDEV_ATTR_ID_BRIDGE_VLAN_FILTERING,
		.flags = SWITCHDEV_F_SKIP_EOPNOTSUPP,
		.u.vlan_filtering = val,
	};
	int err;

	if (br->vlan_enabled == val)
		return 0;

	err = switchdev_port_attr_set(br->dev, &attr);
	if (err && err != -EOPNOTSUPP)
		return err;
>>>>>>> v4.9.227

	br->vlan_enabled = val;
	br_manage_promisc(br);
	recalculate_group_addr(br);
	br_recalculate_fwd_mask(br);

<<<<<<< HEAD
unlock:
	rtnl_unlock();
	return 0;
}

int br_vlan_set_proto(struct net_bridge *br, unsigned long val)
{
	int err = 0;
	struct net_bridge_port *p;
	struct net_port_vlans *pv;
	__be16 proto, oldproto;
	u16 vid, errvid;

	if (val != ETH_P_8021Q && val != ETH_P_8021AD)
		return -EPROTONOSUPPORT;

	if (!rtnl_trylock())
		return restart_syscall();

	proto = htons(val);
	if (br->vlan_proto == proto)
		goto unlock;

	/* Add VLANs for the new proto to the device filter. */
	list_for_each_entry(p, &br->port_list, list) {
		pv = rtnl_dereference(p->vlan_info);
		if (!pv)
			continue;

		for_each_set_bit(vid, pv->vlan_bitmap, VLAN_N_VID) {
			err = vlan_vid_add(p->dev, proto, vid);
=======
	return 0;
}

int br_vlan_filter_toggle(struct net_bridge *br, unsigned long val)
{
	return __br_vlan_filter_toggle(br, val);
}

int __br_vlan_set_proto(struct net_bridge *br, __be16 proto)
{
	int err = 0;
	struct net_bridge_port *p;
	struct net_bridge_vlan *vlan;
	struct net_bridge_vlan_group *vg;
	__be16 oldproto;

	if (br->vlan_proto == proto)
		return 0;

	/* Add VLANs for the new proto to the device filter. */
	list_for_each_entry(p, &br->port_list, list) {
		vg = nbp_vlan_group(p);
		list_for_each_entry(vlan, &vg->vlan_list, vlist) {
			err = vlan_vid_add(p->dev, proto, vlan->vid);
>>>>>>> v4.9.227
			if (err)
				goto err_filt;
		}
	}

	oldproto = br->vlan_proto;
	br->vlan_proto = proto;

	recalculate_group_addr(br);
	br_recalculate_fwd_mask(br);

	/* Delete VLANs for the old proto from the device filter. */
	list_for_each_entry(p, &br->port_list, list) {
<<<<<<< HEAD
		pv = rtnl_dereference(p->vlan_info);
		if (!pv)
			continue;

		for_each_set_bit(vid, pv->vlan_bitmap, VLAN_N_VID)
			vlan_vid_del(p->dev, oldproto, vid);
	}

unlock:
	rtnl_unlock();
	return err;

err_filt:
	errvid = vid;
	for_each_set_bit(vid, pv->vlan_bitmap, errvid)
		vlan_vid_del(p->dev, proto, vid);

	list_for_each_entry_continue_reverse(p, &br->port_list, list) {
		pv = rtnl_dereference(p->vlan_info);
		if (!pv)
			continue;

		for_each_set_bit(vid, pv->vlan_bitmap, VLAN_N_VID)
			vlan_vid_del(p->dev, proto, vid);
	}

	goto unlock;
}

static bool vlan_default_pvid(struct net_port_vlans *pv, u16 vid)
{
	return pv && vid == pv->pvid && test_bit(vid, pv->untagged_bitmap);
=======
		vg = nbp_vlan_group(p);
		list_for_each_entry(vlan, &vg->vlan_list, vlist)
			vlan_vid_del(p->dev, oldproto, vlan->vid);
	}

	return 0;

err_filt:
	list_for_each_entry_continue_reverse(vlan, &vg->vlan_list, vlist)
		vlan_vid_del(p->dev, proto, vlan->vid);

	list_for_each_entry_continue_reverse(p, &br->port_list, list) {
		vg = nbp_vlan_group(p);
		list_for_each_entry(vlan, &vg->vlan_list, vlist)
			vlan_vid_del(p->dev, proto, vlan->vid);
	}

	return err;
}

int br_vlan_set_proto(struct net_bridge *br, unsigned long val)
{
	if (val != ETH_P_8021Q && val != ETH_P_8021AD)
		return -EPROTONOSUPPORT;

	return __br_vlan_set_proto(br, htons(val));
}

int br_vlan_set_stats(struct net_bridge *br, unsigned long val)
{
	switch (val) {
	case 0:
	case 1:
		br->vlan_stats_enabled = val;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static bool vlan_default_pvid(struct net_bridge_vlan_group *vg, u16 vid)
{
	struct net_bridge_vlan *v;

	if (vid != vg->pvid)
		return false;

	v = br_vlan_lookup(&vg->vlan_hash, vid);
	if (v && br_vlan_should_use(v) &&
	    (v->flags & BRIDGE_VLAN_INFO_UNTAGGED))
		return true;

	return false;
>>>>>>> v4.9.227
}

static void br_vlan_disable_default_pvid(struct net_bridge *br)
{
	struct net_bridge_port *p;
	u16 pvid = br->default_pvid;

	/* Disable default_pvid on all ports where it is still
	 * configured.
	 */
<<<<<<< HEAD
	if (vlan_default_pvid(br_get_vlan_info(br), pvid))
		br_vlan_delete(br, pvid);

	list_for_each_entry(p, &br->port_list, list) {
		if (vlan_default_pvid(nbp_get_vlan_info(p), pvid))
=======
	if (vlan_default_pvid(br_vlan_group(br), pvid))
		br_vlan_delete(br, pvid);

	list_for_each_entry(p, &br->port_list, list) {
		if (vlan_default_pvid(nbp_vlan_group(p), pvid))
>>>>>>> v4.9.227
			nbp_vlan_delete(p, pvid);
	}

	br->default_pvid = 0;
}

<<<<<<< HEAD
static int __br_vlan_set_default_pvid(struct net_bridge *br, u16 pvid)
{
=======
int __br_vlan_set_default_pvid(struct net_bridge *br, u16 pvid)
{
	const struct net_bridge_vlan *pvent;
	struct net_bridge_vlan_group *vg;
>>>>>>> v4.9.227
	struct net_bridge_port *p;
	u16 old_pvid;
	int err = 0;
	unsigned long *changed;

<<<<<<< HEAD
=======
	if (!pvid) {
		br_vlan_disable_default_pvid(br);
		return 0;
	}

>>>>>>> v4.9.227
	changed = kcalloc(BITS_TO_LONGS(BR_MAX_PORTS), sizeof(unsigned long),
			  GFP_KERNEL);
	if (!changed)
		return -ENOMEM;

	old_pvid = br->default_pvid;

	/* Update default_pvid config only if we do not conflict with
	 * user configuration.
	 */
<<<<<<< HEAD
	if ((!old_pvid || vlan_default_pvid(br_get_vlan_info(br), old_pvid)) &&
	    !br_vlan_find(br, pvid)) {
		err = br_vlan_add(br, pvid,
				  BRIDGE_VLAN_INFO_PVID |
				  BRIDGE_VLAN_INFO_UNTAGGED);
=======
	vg = br_vlan_group(br);
	pvent = br_vlan_find(vg, pvid);
	if ((!old_pvid || vlan_default_pvid(vg, old_pvid)) &&
	    (!pvent || !br_vlan_should_use(pvent))) {
		err = br_vlan_add(br, pvid,
				  BRIDGE_VLAN_INFO_PVID |
				  BRIDGE_VLAN_INFO_UNTAGGED |
				  BRIDGE_VLAN_INFO_BRENTRY);
>>>>>>> v4.9.227
		if (err)
			goto out;
		br_vlan_delete(br, old_pvid);
		set_bit(0, changed);
	}

	list_for_each_entry(p, &br->port_list, list) {
		/* Update default_pvid config only if we do not conflict with
		 * user configuration.
		 */
<<<<<<< HEAD
		if ((old_pvid &&
		     !vlan_default_pvid(nbp_get_vlan_info(p), old_pvid)) ||
		    nbp_vlan_find(p, pvid))
=======
		vg = nbp_vlan_group(p);
		if ((old_pvid &&
		     !vlan_default_pvid(vg, old_pvid)) ||
		    br_vlan_find(vg, pvid))
>>>>>>> v4.9.227
			continue;

		err = nbp_vlan_add(p, pvid,
				   BRIDGE_VLAN_INFO_PVID |
				   BRIDGE_VLAN_INFO_UNTAGGED);
		if (err)
			goto err_port;
		nbp_vlan_delete(p, old_pvid);
		set_bit(p->port_no, changed);
	}

	br->default_pvid = pvid;

out:
	kfree(changed);
	return err;

err_port:
	list_for_each_entry_continue_reverse(p, &br->port_list, list) {
		if (!test_bit(p->port_no, changed))
			continue;

		if (old_pvid)
			nbp_vlan_add(p, old_pvid,
				     BRIDGE_VLAN_INFO_PVID |
				     BRIDGE_VLAN_INFO_UNTAGGED);
		nbp_vlan_delete(p, pvid);
	}

	if (test_bit(0, changed)) {
		if (old_pvid)
			br_vlan_add(br, old_pvid,
				    BRIDGE_VLAN_INFO_PVID |
<<<<<<< HEAD
				    BRIDGE_VLAN_INFO_UNTAGGED);
=======
				    BRIDGE_VLAN_INFO_UNTAGGED |
				    BRIDGE_VLAN_INFO_BRENTRY);
>>>>>>> v4.9.227
		br_vlan_delete(br, pvid);
	}
	goto out;
}

int br_vlan_set_default_pvid(struct net_bridge *br, unsigned long val)
{
	u16 pvid = val;
	int err = 0;

	if (val >= VLAN_VID_MASK)
		return -EINVAL;

<<<<<<< HEAD
	if (!rtnl_trylock())
		return restart_syscall();

	if (pvid == br->default_pvid)
		goto unlock;
=======
	if (pvid == br->default_pvid)
		goto out;
>>>>>>> v4.9.227

	/* Only allow default pvid change when filtering is disabled */
	if (br->vlan_enabled) {
		pr_info_once("Please disable vlan filtering to change default_pvid\n");
		err = -EPERM;
<<<<<<< HEAD
		goto unlock;
	}

	if (!pvid)
		br_vlan_disable_default_pvid(br);
	else
		err = __br_vlan_set_default_pvid(br, pvid);

unlock:
	rtnl_unlock();
=======
		goto out;
	}
	err = __br_vlan_set_default_pvid(br, pvid);
out:
>>>>>>> v4.9.227
	return err;
}

int br_vlan_init(struct net_bridge *br)
{
<<<<<<< HEAD
	br->vlan_proto = htons(ETH_P_8021Q);
	br->default_pvid = 1;
	return br_vlan_add(br, 1,
			   BRIDGE_VLAN_INFO_PVID | BRIDGE_VLAN_INFO_UNTAGGED);
=======
	struct net_bridge_vlan_group *vg;
	int ret = -ENOMEM;

	vg = kzalloc(sizeof(*vg), GFP_KERNEL);
	if (!vg)
		goto out;
	ret = rhashtable_init(&vg->vlan_hash, &br_vlan_rht_params);
	if (ret)
		goto err_rhtbl;
	INIT_LIST_HEAD(&vg->vlan_list);
	br->vlan_proto = htons(ETH_P_8021Q);
	br->default_pvid = 1;
	rcu_assign_pointer(br->vlgrp, vg);
	ret = br_vlan_add(br, 1,
			  BRIDGE_VLAN_INFO_PVID | BRIDGE_VLAN_INFO_UNTAGGED |
			  BRIDGE_VLAN_INFO_BRENTRY);
	if (ret)
		goto err_vlan_add;

out:
	return ret;

err_vlan_add:
	rhashtable_destroy(&vg->vlan_hash);
err_rhtbl:
	kfree(vg);

	goto out;
}

int nbp_vlan_init(struct net_bridge_port *p)
{
	struct switchdev_attr attr = {
		.orig_dev = p->br->dev,
		.id = SWITCHDEV_ATTR_ID_BRIDGE_VLAN_FILTERING,
		.flags = SWITCHDEV_F_SKIP_EOPNOTSUPP,
		.u.vlan_filtering = p->br->vlan_enabled,
	};
	struct net_bridge_vlan_group *vg;
	int ret = -ENOMEM;

	vg = kzalloc(sizeof(struct net_bridge_vlan_group), GFP_KERNEL);
	if (!vg)
		goto out;

	ret = switchdev_port_attr_set(p->dev, &attr);
	if (ret && ret != -EOPNOTSUPP)
		goto err_vlan_enabled;

	ret = rhashtable_init(&vg->vlan_hash, &br_vlan_rht_params);
	if (ret)
		goto err_rhtbl;
	INIT_LIST_HEAD(&vg->vlan_list);
	rcu_assign_pointer(p->vlgrp, vg);
	if (p->br->default_pvid) {
		ret = nbp_vlan_add(p, p->br->default_pvid,
				   BRIDGE_VLAN_INFO_PVID |
				   BRIDGE_VLAN_INFO_UNTAGGED);
		if (ret)
			goto err_vlan_add;
	}
out:
	return ret;

err_vlan_add:
	RCU_INIT_POINTER(p->vlgrp, NULL);
	synchronize_rcu();
	rhashtable_destroy(&vg->vlan_hash);
err_vlan_enabled:
err_rhtbl:
	kfree(vg);

	goto out;
>>>>>>> v4.9.227
}

/* Must be protected by RTNL.
 * Must be called with vid in range from 1 to 4094 inclusive.
 */
int nbp_vlan_add(struct net_bridge_port *port, u16 vid, u16 flags)
{
<<<<<<< HEAD
	struct net_port_vlans *pv = NULL;
	int err;

	ASSERT_RTNL();

	pv = rtnl_dereference(port->vlan_info);
	if (pv)
		return __vlan_add(pv, vid, flags);

	/* Create port vlan infomration
	 */
	pv = kzalloc(sizeof(*pv), GFP_KERNEL);
	if (!pv) {
		err = -ENOMEM;
		goto clean_up;
	}

	pv->port_idx = port->port_no;
	pv->parent.port = port;
	err = __vlan_add(pv, vid, flags);
	if (err)
		goto clean_up;

	rcu_assign_pointer(port->vlan_info, pv);
	return 0;

clean_up:
	kfree(pv);
	return err;
=======
	struct switchdev_obj_port_vlan v = {
		.obj.orig_dev = port->dev,
		.obj.id = SWITCHDEV_OBJ_ID_PORT_VLAN,
		.flags = flags,
		.vid_begin = vid,
		.vid_end = vid,
	};
	struct net_bridge_vlan *vlan;
	int ret;

	ASSERT_RTNL();

	vlan = br_vlan_find(nbp_vlan_group(port), vid);
	if (vlan) {
		/* Pass the flags to the hardware bridge */
		ret = switchdev_port_obj_add(port->dev, &v.obj);
		if (ret && ret != -EOPNOTSUPP)
			return ret;
		__vlan_add_flags(vlan, flags);
		return 0;
	}

	vlan = kzalloc(sizeof(*vlan), GFP_KERNEL);
	if (!vlan)
		return -ENOMEM;

	vlan->vid = vid;
	vlan->port = port;
	ret = __vlan_add(vlan, flags);
	if (ret)
		kfree(vlan);

	return ret;
>>>>>>> v4.9.227
}

/* Must be protected by RTNL.
 * Must be called with vid in range from 1 to 4094 inclusive.
 */
int nbp_vlan_delete(struct net_bridge_port *port, u16 vid)
{
<<<<<<< HEAD
	struct net_port_vlans *pv;

	ASSERT_RTNL();

	pv = rtnl_dereference(port->vlan_info);
	if (!pv)
		return -EINVAL;

	br_fdb_find_delete_local(port->br, port, port->dev->dev_addr, vid);

	return __vlan_del(pv, vid);
=======
	struct net_bridge_vlan *v;

	ASSERT_RTNL();

	v = br_vlan_find(nbp_vlan_group(port), vid);
	if (!v)
		return -ENOENT;
	br_fdb_find_delete_local(port->br, port, port->dev->dev_addr, vid);
	br_fdb_delete_by_port(port->br, port, vid, 0);

	return __vlan_del(v);
>>>>>>> v4.9.227
}

void nbp_vlan_flush(struct net_bridge_port *port)
{
<<<<<<< HEAD
	struct net_port_vlans *pv;
	u16 vid;

	ASSERT_RTNL();

	pv = rtnl_dereference(port->vlan_info);
	if (!pv)
		return;

	for_each_set_bit(vid, pv->vlan_bitmap, VLAN_N_VID)
		vlan_vid_del(port->dev, port->br->vlan_proto, vid);

	__vlan_flush(pv);
}

bool nbp_vlan_find(struct net_bridge_port *port, u16 vid)
{
	struct net_port_vlans *pv;
	bool found = false;

	rcu_read_lock();
	pv = rcu_dereference(port->vlan_info);

	if (!pv)
		goto out;

	if (test_bit(vid, pv->vlan_bitmap))
		found = true;

out:
	rcu_read_unlock();
	return found;
}

int nbp_vlan_init(struct net_bridge_port *p)
{
	return p->br->default_pvid ?
			nbp_vlan_add(p, p->br->default_pvid,
				     BRIDGE_VLAN_INFO_PVID |
				     BRIDGE_VLAN_INFO_UNTAGGED) :
			0;
=======
	struct net_bridge_vlan_group *vg;

	ASSERT_RTNL();

	vg = nbp_vlan_group(port);
	__vlan_flush(vg);
	RCU_INIT_POINTER(port->vlgrp, NULL);
	synchronize_rcu();
	__vlan_group_free(vg);
}

void br_vlan_get_stats(const struct net_bridge_vlan *v,
		       struct br_vlan_stats *stats)
{
	int i;

	memset(stats, 0, sizeof(*stats));
	for_each_possible_cpu(i) {
		u64 rxpackets, rxbytes, txpackets, txbytes;
		struct br_vlan_stats *cpu_stats;
		unsigned int start;

		cpu_stats = per_cpu_ptr(v->stats, i);
		do {
			start = u64_stats_fetch_begin_irq(&cpu_stats->syncp);
			rxpackets = cpu_stats->rx_packets;
			rxbytes = cpu_stats->rx_bytes;
			txbytes = cpu_stats->tx_bytes;
			txpackets = cpu_stats->tx_packets;
		} while (u64_stats_fetch_retry_irq(&cpu_stats->syncp, start));

		stats->rx_packets += rxpackets;
		stats->rx_bytes += rxbytes;
		stats->tx_bytes += txbytes;
		stats->tx_packets += txpackets;
	}
>>>>>>> v4.9.227
}
