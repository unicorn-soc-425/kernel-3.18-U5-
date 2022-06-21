<<<<<<< HEAD
/* Copyright (C) 2014 B.A.T.M.A.N. contributors:
=======
/* Copyright (C) 2014-2016  B.A.T.M.A.N. contributors:
>>>>>>> v4.9.227
 *
 * Linus Lüssing
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

#ifndef _NET_BATMAN_ADV_MULTICAST_H_
#define _NET_BATMAN_ADV_MULTICAST_H_

<<<<<<< HEAD
/**
 * batadv_forw_mode - the way a packet should be forwarded as
=======
#include "main.h"

struct seq_file;
struct sk_buff;

/**
 * enum batadv_forw_mode - the way a packet should be forwarded as
>>>>>>> v4.9.227
 * @BATADV_FORW_ALL: forward the packet to all nodes (currently via classic
 *  flooding)
 * @BATADV_FORW_SINGLE: forward the packet to a single node (currently via the
 *  BATMAN unicast routing protocol)
 * @BATADV_FORW_NONE: don't forward, drop it
 */
enum batadv_forw_mode {
	BATADV_FORW_ALL,
	BATADV_FORW_SINGLE,
	BATADV_FORW_NONE,
};

#ifdef CONFIG_BATMAN_ADV_MCAST

void batadv_mcast_mla_update(struct batadv_priv *bat_priv);

enum batadv_forw_mode
batadv_mcast_forw_mode(struct batadv_priv *bat_priv, struct sk_buff *skb,
		       struct batadv_orig_node **mcast_single_orig);

void batadv_mcast_init(struct batadv_priv *bat_priv);

<<<<<<< HEAD
=======
int batadv_mcast_flags_seq_print_text(struct seq_file *seq, void *offset);

>>>>>>> v4.9.227
void batadv_mcast_free(struct batadv_priv *bat_priv);

void batadv_mcast_purge_orig(struct batadv_orig_node *orig_node);

#else

static inline void batadv_mcast_mla_update(struct batadv_priv *bat_priv)
{
<<<<<<< HEAD
	return;
=======
>>>>>>> v4.9.227
}

static inline enum batadv_forw_mode
batadv_mcast_forw_mode(struct batadv_priv *bat_priv, struct sk_buff *skb,
		       struct batadv_orig_node **mcast_single_orig)
{
	return BATADV_FORW_ALL;
}

static inline int batadv_mcast_init(struct batadv_priv *bat_priv)
{
	return 0;
}

static inline void batadv_mcast_free(struct batadv_priv *bat_priv)
{
<<<<<<< HEAD
	return;
=======
>>>>>>> v4.9.227
}

static inline void batadv_mcast_purge_orig(struct batadv_orig_node *orig_node)
{
<<<<<<< HEAD
	return;
=======
>>>>>>> v4.9.227
}

#endif /* CONFIG_BATMAN_ADV_MCAST */

#endif /* _NET_BATMAN_ADV_MULTICAST_H_ */
