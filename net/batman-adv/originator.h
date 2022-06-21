<<<<<<< HEAD
/* Copyright (C) 2007-2014 B.A.T.M.A.N. contributors:
=======
/* Copyright (C) 2007-2016  B.A.T.M.A.N. contributors:
>>>>>>> v4.9.227
 *
 * Marek Lindner, Simon Wunderlich
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
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _NET_BATMAN_ADV_ORIGINATOR_H_
#define _NET_BATMAN_ADV_ORIGINATOR_H_

<<<<<<< HEAD
#include "hash.h"

int batadv_compare_orig(const struct hlist_node *node, const void *data2);
int batadv_originator_init(struct batadv_priv *bat_priv);
void batadv_originator_free(struct batadv_priv *bat_priv);
void batadv_purge_orig_ref(struct batadv_priv *bat_priv);
void batadv_orig_node_free_ref(struct batadv_orig_node *orig_node);
void batadv_orig_node_free_ref_now(struct batadv_orig_node *orig_node);
struct batadv_orig_node *batadv_orig_node_new(struct batadv_priv *bat_priv,
					      const uint8_t *addr);
struct batadv_neigh_node *
batadv_neigh_node_get(const struct batadv_orig_node *orig_node,
		      const struct batadv_hard_iface *hard_iface,
		      const uint8_t *addr);
struct batadv_neigh_node *
batadv_neigh_node_new(struct batadv_hard_iface *hard_iface,
		      const uint8_t *neigh_addr,
		      struct batadv_orig_node *orig_node);
void batadv_neigh_node_free_ref(struct batadv_neigh_node *neigh_node);
=======
#include "main.h"

#include <linux/compiler.h>
#include <linux/if_ether.h>
#include <linux/jhash.h>
#include <linux/kref.h>
#include <linux/rculist.h>
#include <linux/rcupdate.h>
#include <linux/stddef.h>
#include <linux/types.h>

#include "hash.h"

struct netlink_callback;
struct seq_file;
struct sk_buff;

bool batadv_compare_orig(const struct hlist_node *node, const void *data2);
int batadv_originator_init(struct batadv_priv *bat_priv);
void batadv_originator_free(struct batadv_priv *bat_priv);
void batadv_purge_orig_ref(struct batadv_priv *bat_priv);
void batadv_orig_node_put(struct batadv_orig_node *orig_node);
struct batadv_orig_node *batadv_orig_node_new(struct batadv_priv *bat_priv,
					      const u8 *addr);
struct batadv_hardif_neigh_node *
batadv_hardif_neigh_get(const struct batadv_hard_iface *hard_iface,
			const u8 *neigh_addr);
void
batadv_hardif_neigh_put(struct batadv_hardif_neigh_node *hardif_neigh);
struct batadv_neigh_node *
batadv_neigh_node_get_or_create(struct batadv_orig_node *orig_node,
				struct batadv_hard_iface *hard_iface,
				const u8 *neigh_addr);
void batadv_neigh_node_put(struct batadv_neigh_node *neigh_node);
>>>>>>> v4.9.227
struct batadv_neigh_node *
batadv_orig_router_get(struct batadv_orig_node *orig_node,
		       const struct batadv_hard_iface *if_outgoing);
struct batadv_neigh_ifinfo *
batadv_neigh_ifinfo_new(struct batadv_neigh_node *neigh,
			struct batadv_hard_iface *if_outgoing);
struct batadv_neigh_ifinfo *
batadv_neigh_ifinfo_get(struct batadv_neigh_node *neigh,
			struct batadv_hard_iface *if_outgoing);
<<<<<<< HEAD
void batadv_neigh_ifinfo_free_ref(struct batadv_neigh_ifinfo *neigh_ifinfo);
=======
void batadv_neigh_ifinfo_put(struct batadv_neigh_ifinfo *neigh_ifinfo);

int batadv_hardif_neigh_dump(struct sk_buff *msg, struct netlink_callback *cb);
int batadv_hardif_neigh_seq_print_text(struct seq_file *seq, void *offset);
>>>>>>> v4.9.227

struct batadv_orig_ifinfo *
batadv_orig_ifinfo_get(struct batadv_orig_node *orig_node,
		       struct batadv_hard_iface *if_outgoing);
struct batadv_orig_ifinfo *
batadv_orig_ifinfo_new(struct batadv_orig_node *orig_node,
		       struct batadv_hard_iface *if_outgoing);
<<<<<<< HEAD
void batadv_orig_ifinfo_free_ref(struct batadv_orig_ifinfo *orig_ifinfo);

int batadv_orig_seq_print_text(struct seq_file *seq, void *offset);
int batadv_orig_hardif_seq_print_text(struct seq_file *seq, void *offset);
int batadv_orig_hash_add_if(struct batadv_hard_iface *hard_iface,
			    int max_if_num);
int batadv_orig_hash_del_if(struct batadv_hard_iface *hard_iface,
			    int max_if_num);
=======
void batadv_orig_ifinfo_put(struct batadv_orig_ifinfo *orig_ifinfo);

int batadv_orig_seq_print_text(struct seq_file *seq, void *offset);
int batadv_orig_dump(struct sk_buff *msg, struct netlink_callback *cb);
int batadv_orig_hardif_seq_print_text(struct seq_file *seq, void *offset);
int batadv_orig_hash_add_if(struct batadv_hard_iface *hard_iface,
			    unsigned int max_if_num);
int batadv_orig_hash_del_if(struct batadv_hard_iface *hard_iface,
			    unsigned int max_if_num);
>>>>>>> v4.9.227
struct batadv_orig_node_vlan *
batadv_orig_node_vlan_new(struct batadv_orig_node *orig_node,
			  unsigned short vid);
struct batadv_orig_node_vlan *
batadv_orig_node_vlan_get(struct batadv_orig_node *orig_node,
			  unsigned short vid);
<<<<<<< HEAD
void batadv_orig_node_vlan_free_ref(struct batadv_orig_node_vlan *orig_vlan);

=======
void batadv_orig_node_vlan_put(struct batadv_orig_node_vlan *orig_vlan);
>>>>>>> v4.9.227

/* hashfunction to choose an entry in a hash table of given size
 * hash algorithm from http://en.wikipedia.org/wiki/Hash_table
 */
<<<<<<< HEAD
static inline uint32_t batadv_choose_orig(const void *data, uint32_t size)
{
	const unsigned char *key = data;
	uint32_t hash = 0;
	size_t i;

	for (i = 0; i < 6; i++) {
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

=======
static inline u32 batadv_choose_orig(const void *data, u32 size)
{
	u32 hash = 0;

	hash = jhash(data, ETH_ALEN, hash);
>>>>>>> v4.9.227
	return hash % size;
}

static inline struct batadv_orig_node *
batadv_orig_hash_find(struct batadv_priv *bat_priv, const void *data)
{
	struct batadv_hashtable *hash = bat_priv->orig_hash;
	struct hlist_head *head;
	struct batadv_orig_node *orig_node, *orig_node_tmp = NULL;
	int index;

	if (!hash)
		return NULL;

	index = batadv_choose_orig(data, hash->size);
	head = &hash->table[index];

	rcu_read_lock();
	hlist_for_each_entry_rcu(orig_node, head, hash_entry) {
		if (!batadv_compare_eth(orig_node, data))
			continue;

<<<<<<< HEAD
		if (!atomic_inc_not_zero(&orig_node->refcount))
=======
		if (!kref_get_unless_zero(&orig_node->refcount))
>>>>>>> v4.9.227
			continue;

		orig_node_tmp = orig_node;
		break;
	}
	rcu_read_unlock();

	return orig_node_tmp;
}

#endif /* _NET_BATMAN_ADV_ORIGINATOR_H_ */
