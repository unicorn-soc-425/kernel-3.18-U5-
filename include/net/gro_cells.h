#ifndef _NET_GRO_CELLS_H
#define _NET_GRO_CELLS_H

#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/netdevice.h>

struct gro_cell {
	struct sk_buff_head	napi_skbs;
	struct napi_struct	napi;
<<<<<<< HEAD
} ____cacheline_aligned_in_smp;

struct gro_cells {
	unsigned int		gro_cells_mask;
	struct gro_cell		*cells;
};

static inline void gro_cells_receive(struct gro_cells *gcells, struct sk_buff *skb)
{
	struct gro_cell *cell = gcells->cells;
	struct net_device *dev = skb->dev;

	if (!cell || skb_cloned(skb) || !(dev->features & NETIF_F_GRO)) {
		netif_rx(skb);
		return;
	}

	if (skb_rx_queue_recorded(skb))
		cell += skb_get_rx_queue(skb) & gcells->gro_cells_mask;

	if (skb_queue_len(&cell->napi_skbs) > netdev_max_backlog) {
		atomic_long_inc(&dev->rx_dropped);
		kfree_skb(skb);
		return;
	}

	/* We run in BH context */
	spin_lock(&cell->napi_skbs.lock);

=======
};

struct gro_cells {
	struct gro_cell __percpu	*cells;
};

static inline int gro_cells_receive(struct gro_cells *gcells, struct sk_buff *skb)
{
	struct gro_cell *cell;
	struct net_device *dev = skb->dev;
	int res;

	rcu_read_lock();
	if (unlikely(!(dev->flags & IFF_UP)))
		goto drop;

	if (!gcells->cells || skb_cloned(skb) || !(dev->features & NETIF_F_GRO)) {
		res = netif_rx(skb);
		goto unlock;
	}

	cell = this_cpu_ptr(gcells->cells);

	if (skb_queue_len(&cell->napi_skbs) > netdev_max_backlog) {
drop:
		atomic_long_inc(&dev->rx_dropped);
		kfree_skb(skb);
		res = NET_RX_DROP;
		goto unlock;
	}

>>>>>>> v4.9.227
	__skb_queue_tail(&cell->napi_skbs, skb);
	if (skb_queue_len(&cell->napi_skbs) == 1)
		napi_schedule(&cell->napi);

<<<<<<< HEAD
	spin_unlock(&cell->napi_skbs.lock);
}

/* called unser BH context */
=======
	res = NET_RX_SUCCESS;

unlock:
	rcu_read_unlock();
	return res;
}

/* called under BH context */
>>>>>>> v4.9.227
static inline int gro_cell_poll(struct napi_struct *napi, int budget)
{
	struct gro_cell *cell = container_of(napi, struct gro_cell, napi);
	struct sk_buff *skb;
	int work_done = 0;

<<<<<<< HEAD
	spin_lock(&cell->napi_skbs.lock);
=======
>>>>>>> v4.9.227
	while (work_done < budget) {
		skb = __skb_dequeue(&cell->napi_skbs);
		if (!skb)
			break;
<<<<<<< HEAD
		spin_unlock(&cell->napi_skbs.lock);
		napi_gro_receive(napi, skb);
		work_done++;
		spin_lock(&cell->napi_skbs.lock);
	}

	if (work_done < budget)
		napi_complete(napi);
	spin_unlock(&cell->napi_skbs.lock);
=======
		napi_gro_receive(napi, skb);
		work_done++;
	}

	if (work_done < budget)
		napi_complete_done(napi, work_done);
>>>>>>> v4.9.227
	return work_done;
}

static inline int gro_cells_init(struct gro_cells *gcells, struct net_device *dev)
{
	int i;

<<<<<<< HEAD
	gcells->gro_cells_mask = roundup_pow_of_two(netif_get_num_default_rss_queues()) - 1;
	gcells->cells = kcalloc(gcells->gro_cells_mask + 1,
				sizeof(struct gro_cell),
				GFP_KERNEL);
	if (!gcells->cells)
		return -ENOMEM;

	for (i = 0; i <= gcells->gro_cells_mask; i++) {
		struct gro_cell *cell = gcells->cells + i;

		skb_queue_head_init(&cell->napi_skbs);
=======
	gcells->cells = alloc_percpu(struct gro_cell);
	if (!gcells->cells)
		return -ENOMEM;

	for_each_possible_cpu(i) {
		struct gro_cell *cell = per_cpu_ptr(gcells->cells, i);

		__skb_queue_head_init(&cell->napi_skbs);

		set_bit(NAPI_STATE_NO_BUSY_POLL, &cell->napi.state);

>>>>>>> v4.9.227
		netif_napi_add(dev, &cell->napi, gro_cell_poll, 64);
		napi_enable(&cell->napi);
	}
	return 0;
}

static inline void gro_cells_destroy(struct gro_cells *gcells)
{
<<<<<<< HEAD
	struct gro_cell *cell = gcells->cells;
	int i;

	if (!cell)
		return;
	for (i = 0; i <= gcells->gro_cells_mask; i++,cell++) {
		netif_napi_del(&cell->napi);
		skb_queue_purge(&cell->napi_skbs);
	}
	kfree(gcells->cells);
=======
	int i;

	if (!gcells->cells)
		return;
	for_each_possible_cpu(i) {
		struct gro_cell *cell = per_cpu_ptr(gcells->cells, i);

		napi_disable(&cell->napi);
		netif_napi_del(&cell->napi);
		__skb_queue_purge(&cell->napi_skbs);
	}
	free_percpu(gcells->cells);
>>>>>>> v4.9.227
	gcells->cells = NULL;
}

#endif
