/*
 * Copyright 2011, Siemens AG
 * written by Alexander Smirnov <alex.bluesman.smirnov@gmail.com>
 */

/* Based on patches from Jon Smirl <jonsmirl@gmail.com>
 * Copyright (c) 2011 Jon Smirl <jonsmirl@gmail.com>
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
=======
>>>>>>> v4.9.227
 */

/* Jon's code is based on 6lowpan implementation for Contiki which is:
 * Copyright (c) 2008, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <linux/bitops.h>
#include <linux/if_arp.h>
<<<<<<< HEAD
#include <linux/module.h>
#include <linux/netdevice.h>
#include <net/6lowpan.h>
#include <net/ipv6.h>
#include <net/af_ieee802154.h>
=======
#include <linux/netdevice.h>

#include <net/6lowpan.h>
#include <net/ipv6.h>

#include "6lowpan_i.h"
#include "nhc.h"

/* Values of fields within the IPHC encoding first byte */
#define LOWPAN_IPHC_TF_MASK	0x18
#define LOWPAN_IPHC_TF_00	0x00
#define LOWPAN_IPHC_TF_01	0x08
#define LOWPAN_IPHC_TF_10	0x10
#define LOWPAN_IPHC_TF_11	0x18

#define LOWPAN_IPHC_NH		0x04

#define LOWPAN_IPHC_HLIM_MASK	0x03
#define LOWPAN_IPHC_HLIM_00	0x00
#define LOWPAN_IPHC_HLIM_01	0x01
#define LOWPAN_IPHC_HLIM_10	0x02
#define LOWPAN_IPHC_HLIM_11	0x03

/* Values of fields within the IPHC encoding second byte */
#define LOWPAN_IPHC_CID		0x80

#define LOWPAN_IPHC_SAC		0x40

#define LOWPAN_IPHC_SAM_MASK	0x30
#define LOWPAN_IPHC_SAM_00	0x00
#define LOWPAN_IPHC_SAM_01	0x10
#define LOWPAN_IPHC_SAM_10	0x20
#define LOWPAN_IPHC_SAM_11	0x30

#define LOWPAN_IPHC_M		0x08

#define LOWPAN_IPHC_DAC		0x04

#define LOWPAN_IPHC_DAM_MASK	0x03
#define LOWPAN_IPHC_DAM_00	0x00
#define LOWPAN_IPHC_DAM_01	0x01
#define LOWPAN_IPHC_DAM_10	0x02
#define LOWPAN_IPHC_DAM_11	0x03

/* ipv6 address based on mac
 * second bit-flip (Universe/Local) is done according RFC2464
 */
#define is_addr_mac_addr_based(a, m) \
	((((a)->s6_addr[8])  == (((m)[0]) ^ 0x02)) &&	\
	 (((a)->s6_addr[9])  == (m)[1]) &&		\
	 (((a)->s6_addr[10]) == (m)[2]) &&		\
	 (((a)->s6_addr[11]) == (m)[3]) &&		\
	 (((a)->s6_addr[12]) == (m)[4]) &&		\
	 (((a)->s6_addr[13]) == (m)[5]) &&		\
	 (((a)->s6_addr[14]) == (m)[6]) &&		\
	 (((a)->s6_addr[15]) == (m)[7]))

/* check whether we can compress the IID to 16 bits,
 * it's possible for unicast addresses with first 49 bits are zero only.
 */
#define lowpan_is_iid_16_bit_compressable(a)	\
	((((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr[10]) == 0) &&		\
	 (((a)->s6_addr[11]) == 0xff) &&	\
	 (((a)->s6_addr[12]) == 0xfe) &&	\
	 (((a)->s6_addr[13]) == 0))

/* check whether the 112-bit gid of the multicast address is mappable to: */

/* 48 bits, FFXX::00XX:XXXX:XXXX */
#define lowpan_is_mcast_addr_compressable48(a)	\
	((((a)->s6_addr16[1]) == 0) &&		\
	 (((a)->s6_addr16[2]) == 0) &&		\
	 (((a)->s6_addr16[3]) == 0) &&		\
	 (((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr[10]) == 0))

/* 32 bits, FFXX::00XX:XXXX */
#define lowpan_is_mcast_addr_compressable32(a)	\
	((((a)->s6_addr16[1]) == 0) &&		\
	 (((a)->s6_addr16[2]) == 0) &&		\
	 (((a)->s6_addr16[3]) == 0) &&		\
	 (((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr16[5]) == 0) &&		\
	 (((a)->s6_addr[12]) == 0))

/* 8 bits, FF02::00XX */
#define lowpan_is_mcast_addr_compressable8(a)	\
	((((a)->s6_addr[1])  == 2) &&		\
	 (((a)->s6_addr16[1]) == 0) &&		\
	 (((a)->s6_addr16[2]) == 0) &&		\
	 (((a)->s6_addr16[3]) == 0) &&		\
	 (((a)->s6_addr16[4]) == 0) &&		\
	 (((a)->s6_addr16[5]) == 0) &&		\
	 (((a)->s6_addr16[6]) == 0) &&		\
	 (((a)->s6_addr[14]) == 0))

#define lowpan_is_linklocal_zero_padded(a)	\
	(!(hdr->saddr.s6_addr[1] & 0x3f) &&	\
	 !hdr->saddr.s6_addr16[1] &&		\
	 !hdr->saddr.s6_addr32[1])

#define LOWPAN_IPHC_CID_DCI(cid)	(cid & 0x0f)
#define LOWPAN_IPHC_CID_SCI(cid)	((cid & 0xf0) >> 4)

static inline void
lowpan_iphc_uncompress_802154_lladdr(struct in6_addr *ipaddr,
				     const void *lladdr)
{
	const struct ieee802154_addr *addr = lladdr;
	u8 eui64[EUI64_ADDR_LEN];

	switch (addr->mode) {
	case IEEE802154_ADDR_LONG:
		ieee802154_le64_to_be64(eui64, &addr->extended_addr);
		lowpan_iphc_uncompress_eui64_lladdr(ipaddr, eui64);
		break;
	case IEEE802154_ADDR_SHORT:
		/* fe:80::ff:fe00:XXXX
		 *                \__/
		 *             short_addr
		 *
		 * Universe/Local bit is zero.
		 */
		ipaddr->s6_addr[0] = 0xFE;
		ipaddr->s6_addr[1] = 0x80;
		ipaddr->s6_addr[11] = 0xFF;
		ipaddr->s6_addr[12] = 0xFE;
		ieee802154_le16_to_be16(&ipaddr->s6_addr16[7],
					&addr->short_addr);
		break;
	default:
		/* should never handled and filtered by 802154 6lowpan */
		WARN_ON_ONCE(1);
		break;
	}
}

static struct lowpan_iphc_ctx *
lowpan_iphc_ctx_get_by_id(const struct net_device *dev, u8 id)
{
	struct lowpan_iphc_ctx *ret = &lowpan_dev(dev)->ctx.table[id];

	if (!lowpan_iphc_ctx_is_active(ret))
		return NULL;

	return ret;
}

static struct lowpan_iphc_ctx *
lowpan_iphc_ctx_get_by_addr(const struct net_device *dev,
			    const struct in6_addr *addr)
{
	struct lowpan_iphc_ctx *table = lowpan_dev(dev)->ctx.table;
	struct lowpan_iphc_ctx *ret = NULL;
	struct in6_addr addr_pfx;
	u8 addr_plen;
	int i;

	for (i = 0; i < LOWPAN_IPHC_CTX_TABLE_SIZE; i++) {
		/* Check if context is valid. A context that is not valid
		 * MUST NOT be used for compression.
		 */
		if (!lowpan_iphc_ctx_is_active(&table[i]) ||
		    !lowpan_iphc_ctx_is_compression(&table[i]))
			continue;

		ipv6_addr_prefix(&addr_pfx, addr, table[i].plen);

		/* if prefix len < 64, the remaining bits until 64th bit is
		 * zero. Otherwise we use table[i]->plen.
		 */
		if (table[i].plen < 64)
			addr_plen = 64;
		else
			addr_plen = table[i].plen;

		if (ipv6_prefix_equal(&addr_pfx, &table[i].pfx, addr_plen)) {
			/* remember first match */
			if (!ret) {
				ret = &table[i];
				continue;
			}

			/* get the context with longest prefix len */
			if (table[i].plen > ret->plen)
				ret = &table[i];
		}
	}

	return ret;
}

static struct lowpan_iphc_ctx *
lowpan_iphc_ctx_get_by_mcast_addr(const struct net_device *dev,
				  const struct in6_addr *addr)
{
	struct lowpan_iphc_ctx *table = lowpan_dev(dev)->ctx.table;
	struct lowpan_iphc_ctx *ret = NULL;
	struct in6_addr addr_mcast, network_pfx = {};
	int i;

	/* init mcast address with  */
	memcpy(&addr_mcast, addr, sizeof(*addr));

	for (i = 0; i < LOWPAN_IPHC_CTX_TABLE_SIZE; i++) {
		/* Check if context is valid. A context that is not valid
		 * MUST NOT be used for compression.
		 */
		if (!lowpan_iphc_ctx_is_active(&table[i]) ||
		    !lowpan_iphc_ctx_is_compression(&table[i]))
			continue;

		/* setting plen */
		addr_mcast.s6_addr[3] = table[i].plen;
		/* get network prefix to copy into multicast address */
		ipv6_addr_prefix(&network_pfx, &table[i].pfx,
				 table[i].plen);
		/* setting network prefix */
		memcpy(&addr_mcast.s6_addr[4], &network_pfx, 8);

		if (ipv6_addr_equal(addr, &addr_mcast)) {
			ret = &table[i];
			break;
		}
	}

	return ret;
}
>>>>>>> v4.9.227

/* Uncompress address function for source and
 * destination address(non-multicast).
 *
<<<<<<< HEAD
 * address_mode is sam value or dam value.
 */
static int uncompress_addr(struct sk_buff *skb,
			   struct in6_addr *ipaddr, const u8 address_mode,
			   const u8 *lladdr, const u8 addr_type,
			   const u8 addr_len)
=======
 * address_mode is the masked value for sam or dam value
 */
static int lowpan_iphc_uncompress_addr(struct sk_buff *skb,
				       const struct net_device *dev,
				       struct in6_addr *ipaddr,
				       u8 address_mode, const void *lladdr)
>>>>>>> v4.9.227
{
	bool fail;

	switch (address_mode) {
<<<<<<< HEAD
	case LOWPAN_IPHC_ADDR_00:
		/* for global link addresses */
		fail = lowpan_fetch_skb(skb, ipaddr->s6_addr, 16);
		break;
	case LOWPAN_IPHC_ADDR_01:
=======
	/* SAM and DAM are the same here */
	case LOWPAN_IPHC_DAM_00:
		/* for global link addresses */
		fail = lowpan_fetch_skb(skb, ipaddr->s6_addr, 16);
		break;
	case LOWPAN_IPHC_SAM_01:
	case LOWPAN_IPHC_DAM_01:
>>>>>>> v4.9.227
		/* fe:80::XXXX:XXXX:XXXX:XXXX */
		ipaddr->s6_addr[0] = 0xFE;
		ipaddr->s6_addr[1] = 0x80;
		fail = lowpan_fetch_skb(skb, &ipaddr->s6_addr[8], 8);
		break;
<<<<<<< HEAD
	case LOWPAN_IPHC_ADDR_02:
=======
	case LOWPAN_IPHC_SAM_10:
	case LOWPAN_IPHC_DAM_10:
>>>>>>> v4.9.227
		/* fe:80::ff:fe00:XXXX */
		ipaddr->s6_addr[0] = 0xFE;
		ipaddr->s6_addr[1] = 0x80;
		ipaddr->s6_addr[11] = 0xFF;
		ipaddr->s6_addr[12] = 0xFE;
		fail = lowpan_fetch_skb(skb, &ipaddr->s6_addr[14], 2);
		break;
<<<<<<< HEAD
	case LOWPAN_IPHC_ADDR_03:
		fail = false;
		switch (addr_type) {
		case IEEE802154_ADDR_LONG:
			/* fe:80::XXXX:XXXX:XXXX:XXXX
			 *        \_________________/
			 *              hwaddr
			 */
			ipaddr->s6_addr[0] = 0xFE;
			ipaddr->s6_addr[1] = 0x80;
			memcpy(&ipaddr->s6_addr[8], lladdr, addr_len);
			/* second bit-flip (Universe/Local)
			 * is done according RFC2464
			 */
			ipaddr->s6_addr[8] ^= 0x02;
			break;
		case IEEE802154_ADDR_SHORT:
			/* fe:80::ff:fe00:XXXX
			 *		  \__/
			 *	       short_addr
			 *
			 * Universe/Local bit is zero.
			 */
			ipaddr->s6_addr[0] = 0xFE;
			ipaddr->s6_addr[1] = 0x80;
			ipaddr->s6_addr[11] = 0xFF;
			ipaddr->s6_addr[12] = 0xFE;
			ipaddr->s6_addr16[7] = htons(*((u16 *)lladdr));
			break;
		default:
			pr_debug("Invalid addr_type set\n");
			return -EINVAL;
=======
	case LOWPAN_IPHC_SAM_11:
	case LOWPAN_IPHC_DAM_11:
		fail = false;
		switch (lowpan_dev(dev)->lltype) {
		case LOWPAN_LLTYPE_IEEE802154:
			lowpan_iphc_uncompress_802154_lladdr(ipaddr, lladdr);
			break;
		default:
			lowpan_iphc_uncompress_eui64_lladdr(ipaddr, lladdr);
			break;
>>>>>>> v4.9.227
		}
		break;
	default:
		pr_debug("Invalid address mode value: 0x%x\n", address_mode);
		return -EINVAL;
	}

	if (fail) {
		pr_debug("Failed to fetch skb data\n");
		return -EIO;
	}

	raw_dump_inline(NULL, "Reconstructed ipv6 addr is",
			ipaddr->s6_addr, 16);

	return 0;
}

/* Uncompress address function for source context
 * based address(non-multicast).
 */
<<<<<<< HEAD
static int uncompress_context_based_src_addr(struct sk_buff *skb,
					     struct in6_addr *ipaddr,
					     const u8 sam)
{
	switch (sam) {
	case LOWPAN_IPHC_ADDR_00:
		/* unspec address ::
		 * Do nothing, address is already ::
		 */
		break;
	case LOWPAN_IPHC_ADDR_01:
		/* TODO */
	case LOWPAN_IPHC_ADDR_02:
		/* TODO */
	case LOWPAN_IPHC_ADDR_03:
		/* TODO */
		netdev_warn(skb->dev, "SAM value 0x%x not supported\n", sam);
		return -EINVAL;
	default:
		pr_debug("Invalid sam value: 0x%x\n", sam);
		return -EINVAL;
	}

=======
static int lowpan_iphc_uncompress_ctx_addr(struct sk_buff *skb,
					   const struct net_device *dev,
					   const struct lowpan_iphc_ctx *ctx,
					   struct in6_addr *ipaddr,
					   u8 address_mode, const void *lladdr)
{
	bool fail;

	switch (address_mode) {
	/* SAM and DAM are the same here */
	case LOWPAN_IPHC_DAM_00:
		fail = false;
		/* SAM_00 -> unspec address ::
		 * Do nothing, address is already ::
		 *
		 * DAM 00 -> reserved should never occur.
		 */
		break;
	case LOWPAN_IPHC_SAM_01:
	case LOWPAN_IPHC_DAM_01:
		fail = lowpan_fetch_skb(skb, &ipaddr->s6_addr[8], 8);
		ipv6_addr_prefix_copy(ipaddr, &ctx->pfx, ctx->plen);
		break;
	case LOWPAN_IPHC_SAM_10:
	case LOWPAN_IPHC_DAM_10:
		ipaddr->s6_addr[11] = 0xFF;
		ipaddr->s6_addr[12] = 0xFE;
		fail = lowpan_fetch_skb(skb, &ipaddr->s6_addr[14], 2);
		ipv6_addr_prefix_copy(ipaddr, &ctx->pfx, ctx->plen);
		break;
	case LOWPAN_IPHC_SAM_11:
	case LOWPAN_IPHC_DAM_11:
		fail = false;
		switch (lowpan_dev(dev)->lltype) {
		case LOWPAN_LLTYPE_IEEE802154:
			lowpan_iphc_uncompress_802154_lladdr(ipaddr, lladdr);
			break;
		default:
			lowpan_iphc_uncompress_eui64_lladdr(ipaddr, lladdr);
			break;
		}
		ipv6_addr_prefix_copy(ipaddr, &ctx->pfx, ctx->plen);
		break;
	default:
		pr_debug("Invalid sam value: 0x%x\n", address_mode);
		return -EINVAL;
	}

	if (fail) {
		pr_debug("Failed to fetch skb data\n");
		return -EIO;
	}

>>>>>>> v4.9.227
	raw_dump_inline(NULL,
			"Reconstructed context based ipv6 src addr is",
			ipaddr->s6_addr, 16);

	return 0;
}

<<<<<<< HEAD
static int skb_deliver(struct sk_buff *skb, struct ipv6hdr *hdr,
		       struct net_device *dev, skb_delivery_cb deliver_skb)
{
	struct sk_buff *new;
	int stat;

	new = skb_copy_expand(skb, sizeof(struct ipv6hdr), skb_tailroom(skb),
			      GFP_ATOMIC);
	kfree_skb(skb);

	if (!new)
		return -ENOMEM;

	skb_push(new, sizeof(struct ipv6hdr));
	skb_reset_network_header(new);
	skb_copy_to_linear_data(new, hdr, sizeof(struct ipv6hdr));

	new->protocol = htons(ETH_P_IPV6);
	new->pkt_type = PACKET_HOST;
	new->dev = dev;

	raw_dump_table(__func__, "raw skb data dump before receiving",
		       new->data, new->len);

	stat = deliver_skb(new, dev);

	kfree_skb(new);

	return stat;
}

=======
>>>>>>> v4.9.227
/* Uncompress function for multicast destination address,
 * when M bit is set.
 */
static int lowpan_uncompress_multicast_daddr(struct sk_buff *skb,
					     struct in6_addr *ipaddr,
<<<<<<< HEAD
					     const u8 dam)
{
	bool fail;

	switch (dam) {
=======
					     u8 address_mode)
{
	bool fail;

	switch (address_mode) {
>>>>>>> v4.9.227
	case LOWPAN_IPHC_DAM_00:
		/* 00:  128 bits.  The full address
		 * is carried in-line.
		 */
		fail = lowpan_fetch_skb(skb, ipaddr->s6_addr, 16);
		break;
	case LOWPAN_IPHC_DAM_01:
		/* 01:  48 bits.  The address takes
		 * the form ffXX::00XX:XXXX:XXXX.
		 */
		ipaddr->s6_addr[0] = 0xFF;
		fail = lowpan_fetch_skb(skb, &ipaddr->s6_addr[1], 1);
		fail |= lowpan_fetch_skb(skb, &ipaddr->s6_addr[11], 5);
		break;
	case LOWPAN_IPHC_DAM_10:
		/* 10:  32 bits.  The address takes
		 * the form ffXX::00XX:XXXX.
		 */
		ipaddr->s6_addr[0] = 0xFF;
		fail = lowpan_fetch_skb(skb, &ipaddr->s6_addr[1], 1);
		fail |= lowpan_fetch_skb(skb, &ipaddr->s6_addr[13], 3);
		break;
	case LOWPAN_IPHC_DAM_11:
		/* 11:  8 bits.  The address takes
		 * the form ff02::00XX.
		 */
		ipaddr->s6_addr[0] = 0xFF;
		ipaddr->s6_addr[1] = 0x02;
		fail = lowpan_fetch_skb(skb, &ipaddr->s6_addr[15], 1);
		break;
	default:
<<<<<<< HEAD
		pr_debug("DAM value has a wrong value: 0x%x\n", dam);
=======
		pr_debug("DAM value has a wrong value: 0x%x\n", address_mode);
>>>>>>> v4.9.227
		return -EINVAL;
	}

	if (fail) {
		pr_debug("Failed to fetch skb data\n");
		return -EIO;
	}

	raw_dump_inline(NULL, "Reconstructed ipv6 multicast addr is",
			ipaddr->s6_addr, 16);

	return 0;
}

<<<<<<< HEAD
static int uncompress_udp_header(struct sk_buff *skb, struct udphdr *uh)
{
	bool fail;
	u8 tmp = 0, val = 0;

	fail = lowpan_fetch_skb(skb, &tmp, sizeof(tmp));

	if ((tmp & LOWPAN_NHC_UDP_MASK) == LOWPAN_NHC_UDP_ID) {
		pr_debug("UDP header uncompression\n");
		switch (tmp & LOWPAN_NHC_UDP_CS_P_11) {
		case LOWPAN_NHC_UDP_CS_P_00:
			fail |= lowpan_fetch_skb(skb, &uh->source,
						 sizeof(uh->source));
			fail |= lowpan_fetch_skb(skb, &uh->dest,
						 sizeof(uh->dest));
			break;
		case LOWPAN_NHC_UDP_CS_P_01:
			fail |= lowpan_fetch_skb(skb, &uh->source,
						 sizeof(uh->source));
			fail |= lowpan_fetch_skb(skb, &val, sizeof(val));
			uh->dest = htons(val + LOWPAN_NHC_UDP_8BIT_PORT);
			break;
		case LOWPAN_NHC_UDP_CS_P_10:
			fail |= lowpan_fetch_skb(skb, &val, sizeof(val));
			uh->source = htons(val + LOWPAN_NHC_UDP_8BIT_PORT);
			fail |= lowpan_fetch_skb(skb, &uh->dest,
						 sizeof(uh->dest));
			break;
		case LOWPAN_NHC_UDP_CS_P_11:
			fail |= lowpan_fetch_skb(skb, &val, sizeof(val));
			uh->source = htons(LOWPAN_NHC_UDP_4BIT_PORT +
					   (val >> 4));
			uh->dest = htons(LOWPAN_NHC_UDP_4BIT_PORT +
					 (val & 0x0f));
			break;
		default:
			pr_debug("ERROR: unknown UDP format\n");
			goto err;
		}

		pr_debug("uncompressed UDP ports: src = %d, dst = %d\n",
			 ntohs(uh->source), ntohs(uh->dest));

		/* checksum */
		if (tmp & LOWPAN_NHC_UDP_CS_C) {
			pr_debug_ratelimited("checksum elided currently not supported\n");
			goto err;
		} else {
			fail |= lowpan_fetch_skb(skb, &uh->check,
						 sizeof(uh->check));
		}

		/* UDP length needs to be infered from the lower layers
		 * here, we obtain the hint from the remaining size of the
		 * frame
		 */
		uh->len = htons(skb->len + sizeof(struct udphdr));
		pr_debug("uncompressed UDP length: src = %d", ntohs(uh->len));
	} else {
		pr_debug("ERROR: unsupported NH format\n");
		goto err;
	}

	if (fail)
		goto err;

	return 0;
err:
	return -EINVAL;
}

/* TTL uncompression values */
static const u8 lowpan_ttl_values[] = { 0, 1, 64, 255 };

int lowpan_process_data(struct sk_buff *skb, struct net_device *dev,
			const u8 *saddr, const u8 saddr_type, const u8 saddr_len,
			const u8 *daddr, const u8 daddr_type, const u8 daddr_len,
			u8 iphc0, u8 iphc1, skb_delivery_cb deliver_skb)
{
	struct ipv6hdr hdr = {};
	u8 tmp, num_context = 0;
=======
static int lowpan_uncompress_multicast_ctx_daddr(struct sk_buff *skb,
						 struct lowpan_iphc_ctx *ctx,
						 struct in6_addr *ipaddr,
						 u8 address_mode)
{
	struct in6_addr network_pfx = {};
	bool fail;

	ipaddr->s6_addr[0] = 0xFF;
	fail = lowpan_fetch_skb(skb, &ipaddr->s6_addr[1], 2);
	fail |= lowpan_fetch_skb(skb, &ipaddr->s6_addr[12], 4);
	if (fail)
		return -EIO;

	/* take prefix_len and network prefix from the context */
	ipaddr->s6_addr[3] = ctx->plen;
	/* get network prefix to copy into multicast address */
	ipv6_addr_prefix(&network_pfx, &ctx->pfx, ctx->plen);
	/* setting network prefix */
	memcpy(&ipaddr->s6_addr[4], &network_pfx, 8);

	return 0;
}

/* get the ecn values from iphc tf format and set it to ipv6hdr */
static inline void lowpan_iphc_tf_set_ecn(struct ipv6hdr *hdr, const u8 *tf)
{
	/* get the two higher bits which is ecn */
	u8 ecn = tf[0] & 0xc0;

	/* ECN takes 0x30 in hdr->flow_lbl[0] */
	hdr->flow_lbl[0] |= (ecn >> 2);
}

/* get the dscp values from iphc tf format and set it to ipv6hdr */
static inline void lowpan_iphc_tf_set_dscp(struct ipv6hdr *hdr, const u8 *tf)
{
	/* DSCP is at place after ECN */
	u8 dscp = tf[0] & 0x3f;

	/* The four highest bits need to be set at hdr->priority */
	hdr->priority |= ((dscp & 0x3c) >> 2);
	/* The two lower bits is part of hdr->flow_lbl[0] */
	hdr->flow_lbl[0] |= ((dscp & 0x03) << 6);
}

/* get the flow label values from iphc tf format and set it to ipv6hdr */
static inline void lowpan_iphc_tf_set_lbl(struct ipv6hdr *hdr, const u8 *lbl)
{
	/* flow label is always some array started with lower nibble of
	 * flow_lbl[0] and followed with two bytes afterwards. Inside inline
	 * data the flow_lbl position can be different, which will be handled
	 * by lbl pointer. E.g. case "01" vs "00" the traffic class is 8 bit
	 * shifted, the different lbl pointer will handle that.
	 *
	 * The flow label will started at lower nibble of flow_lbl[0], the
	 * higher nibbles are part of DSCP + ECN.
	 */
	hdr->flow_lbl[0] |= lbl[0] & 0x0f;
	memcpy(&hdr->flow_lbl[1], &lbl[1], 2);
}

/* lowpan_iphc_tf_decompress - decompress the traffic class.
 *	This function will return zero on success, a value lower than zero if
 *	failed.
 */
static int lowpan_iphc_tf_decompress(struct sk_buff *skb, struct ipv6hdr *hdr,
				     u8 val)
{
	u8 tf[4];

	/* Traffic Class and Flow Label */
	switch (val) {
	case LOWPAN_IPHC_TF_00:
		/* ECN + DSCP + 4-bit Pad + Flow Label (4 bytes) */
		if (lowpan_fetch_skb(skb, tf, 4))
			return -EINVAL;

		/*                      1                   2                   3
		 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
		 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 * |ECN|   DSCP    |  rsv  |             Flow Label                |
		 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 */
		lowpan_iphc_tf_set_ecn(hdr, tf);
		lowpan_iphc_tf_set_dscp(hdr, tf);
		lowpan_iphc_tf_set_lbl(hdr, &tf[1]);
		break;
	case LOWPAN_IPHC_TF_01:
		/* ECN + 2-bit Pad + Flow Label (3 bytes), DSCP is elided. */
		if (lowpan_fetch_skb(skb, tf, 3))
			return -EINVAL;

		/*                     1                   2
		 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
		 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 * |ECN|rsv|             Flow Label                |
		 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
		 */
		lowpan_iphc_tf_set_ecn(hdr, tf);
		lowpan_iphc_tf_set_lbl(hdr, &tf[0]);
		break;
	case LOWPAN_IPHC_TF_10:
		/* ECN + DSCP (1 byte), Flow Label is elided. */
		if (lowpan_fetch_skb(skb, tf, 1))
			return -EINVAL;

		/*  0 1 2 3 4 5 6 7
		 * +-+-+-+-+-+-+-+-+
		 * |ECN|   DSCP    |
		 * +-+-+-+-+-+-+-+-+
		 */
		lowpan_iphc_tf_set_ecn(hdr, tf);
		lowpan_iphc_tf_set_dscp(hdr, tf);
		break;
	case LOWPAN_IPHC_TF_11:
		/* Traffic Class and Flow Label are elided */
		break;
	default:
		WARN_ON_ONCE(1);
		return -EINVAL;
	}

	return 0;
}

/* TTL uncompression values */
static const u8 lowpan_ttl_values[] = {
	[LOWPAN_IPHC_HLIM_01] = 1,
	[LOWPAN_IPHC_HLIM_10] = 64,
	[LOWPAN_IPHC_HLIM_11] = 255,
};

int lowpan_header_decompress(struct sk_buff *skb, const struct net_device *dev,
			     const void *daddr, const void *saddr)
{
	struct ipv6hdr hdr = {};
	struct lowpan_iphc_ctx *ci;
	u8 iphc0, iphc1, cid = 0;
>>>>>>> v4.9.227
	int err;

	raw_dump_table(__func__, "raw skb data dump uncompressed",
		       skb->data, skb->len);

<<<<<<< HEAD
	/* another if the CID flag is set */
	if (iphc1 & LOWPAN_IPHC_CID) {
		pr_debug("CID flag is set, increase header with one\n");
		if (lowpan_fetch_skb(skb, &num_context, sizeof(num_context)))
			goto drop;
	}

	hdr.version = 6;

	/* Traffic Class and Flow Label */
	switch ((iphc0 & LOWPAN_IPHC_TF) >> 3) {
	/* Traffic Class and FLow Label carried in-line
	 * ECN + DSCP + 4-bit Pad + Flow Label (4 bytes)
	 */
	case 0: /* 00b */
		if (lowpan_fetch_skb(skb, &tmp, sizeof(tmp)))
			goto drop;

		memcpy(&hdr.flow_lbl, &skb->data[0], 3);
		skb_pull(skb, 3);
		hdr.priority = ((tmp >> 2) & 0x0f);
		hdr.flow_lbl[0] = ((tmp >> 2) & 0x30) | (tmp << 6) |
					(hdr.flow_lbl[0] & 0x0f);
		break;
	/* Traffic class carried in-line
	 * ECN + DSCP (1 byte), Flow Label is elided
	 */
	case 2: /* 10b */
		if (lowpan_fetch_skb(skb, &tmp, sizeof(tmp)))
			goto drop;

		hdr.priority = ((tmp >> 2) & 0x0f);
		hdr.flow_lbl[0] = ((tmp << 6) & 0xC0) | ((tmp >> 2) & 0x30);
		break;
	/* Flow Label carried in-line
	 * ECN + 2-bit Pad + Flow Label (3 bytes), DSCP is elided
	 */
	case 1: /* 01b */
		if (lowpan_fetch_skb(skb, &tmp, sizeof(tmp)))
			goto drop;

		hdr.flow_lbl[0] = (skb->data[0] & 0x0F) | ((tmp >> 2) & 0x30);
		memcpy(&hdr.flow_lbl[1], &skb->data[0], 2);
		skb_pull(skb, 2);
		break;
	/* Traffic Class and Flow Label are elided */
	case 3: /* 11b */
		break;
	default:
		break;
	}

	/* Next Header */
	if ((iphc0 & LOWPAN_IPHC_NH_C) == 0) {
		/* Next header is carried inline */
		if (lowpan_fetch_skb(skb, &hdr.nexthdr, sizeof(hdr.nexthdr)))
			goto drop;
=======
	if (lowpan_fetch_skb(skb, &iphc0, sizeof(iphc0)) ||
	    lowpan_fetch_skb(skb, &iphc1, sizeof(iphc1)))
		return -EINVAL;

	hdr.version = 6;

	/* default CID = 0, another if the CID flag is set */
	if (iphc1 & LOWPAN_IPHC_CID) {
		if (lowpan_fetch_skb(skb, &cid, sizeof(cid)))
			return -EINVAL;
	}

	err = lowpan_iphc_tf_decompress(skb, &hdr,
					iphc0 & LOWPAN_IPHC_TF_MASK);
	if (err < 0)
		return err;

	/* Next Header */
	if (!(iphc0 & LOWPAN_IPHC_NH)) {
		/* Next header is carried inline */
		if (lowpan_fetch_skb(skb, &hdr.nexthdr, sizeof(hdr.nexthdr)))
			return -EINVAL;
>>>>>>> v4.9.227

		pr_debug("NH flag is set, next header carried inline: %02x\n",
			 hdr.nexthdr);
	}

	/* Hop Limit */
<<<<<<< HEAD
	if ((iphc0 & 0x03) != LOWPAN_IPHC_TTL_I) {
		hdr.hop_limit = lowpan_ttl_values[iphc0 & 0x03];
	} else {
		if (lowpan_fetch_skb(skb, &hdr.hop_limit,
				     sizeof(hdr.hop_limit)))
			goto drop;
	}

	/* Extract SAM to the tmp variable */
	tmp = ((iphc1 & LOWPAN_IPHC_SAM) >> LOWPAN_IPHC_SAM_BIT) & 0x03;

	if (iphc1 & LOWPAN_IPHC_SAC) {
		/* Source address context based uncompression */
		pr_debug("SAC bit is set. Handle context based source address.\n");
		err = uncompress_context_based_src_addr(skb, &hdr.saddr, tmp);
	} else {
		/* Source address uncompression */
		pr_debug("source address stateless compression\n");
		err = uncompress_addr(skb, &hdr.saddr, tmp, saddr,
				      saddr_type, saddr_len);
=======
	if ((iphc0 & LOWPAN_IPHC_HLIM_MASK) != LOWPAN_IPHC_HLIM_00) {
		hdr.hop_limit = lowpan_ttl_values[iphc0 & LOWPAN_IPHC_HLIM_MASK];
	} else {
		if (lowpan_fetch_skb(skb, &hdr.hop_limit,
				     sizeof(hdr.hop_limit)))
			return -EINVAL;
	}

	if (iphc1 & LOWPAN_IPHC_SAC) {
		spin_lock_bh(&lowpan_dev(dev)->ctx.lock);
		ci = lowpan_iphc_ctx_get_by_id(dev, LOWPAN_IPHC_CID_SCI(cid));
		if (!ci) {
			spin_unlock_bh(&lowpan_dev(dev)->ctx.lock);
			return -EINVAL;
		}

		pr_debug("SAC bit is set. Handle context based source address.\n");
		err = lowpan_iphc_uncompress_ctx_addr(skb, dev, ci, &hdr.saddr,
						      iphc1 & LOWPAN_IPHC_SAM_MASK,
						      saddr);
		spin_unlock_bh(&lowpan_dev(dev)->ctx.lock);
	} else {
		/* Source address uncompression */
		pr_debug("source address stateless compression\n");
		err = lowpan_iphc_uncompress_addr(skb, dev, &hdr.saddr,
						  iphc1 & LOWPAN_IPHC_SAM_MASK,
						  saddr);
>>>>>>> v4.9.227
	}

	/* Check on error of previous branch */
	if (err)
<<<<<<< HEAD
		goto drop;

	/* Extract DAM to the tmp variable */
	tmp = ((iphc1 & LOWPAN_IPHC_DAM_11) >> LOWPAN_IPHC_DAM_BIT) & 0x03;

	/* check for Multicast Compression */
	if (iphc1 & LOWPAN_IPHC_M) {
		if (iphc1 & LOWPAN_IPHC_DAC) {
			pr_debug("dest: context-based mcast compression\n");
			/* TODO: implement this */
		} else {
			err = lowpan_uncompress_multicast_daddr(skb, &hdr.daddr,
								tmp);

			if (err)
				goto drop;
		}
	} else {
		err = uncompress_addr(skb, &hdr.daddr, tmp, daddr,
				      daddr_type, daddr_len);
		pr_debug("dest: stateless compression mode %d dest %pI6c\n",
			 tmp, &hdr.daddr);
		if (err)
			goto drop;
	}

	/* UDP data uncompression */
	if (iphc0 & LOWPAN_IPHC_NH_C) {
		struct udphdr uh;
		struct sk_buff *new;

		if (uncompress_udp_header(skb, &uh))
			goto drop;

		/* replace the compressed UDP head by the uncompressed UDP
		 * header
		 */
		new = skb_copy_expand(skb, sizeof(struct udphdr),
				      skb_tailroom(skb), GFP_ATOMIC);
		kfree_skb(skb);

		if (!new)
			return -ENOMEM;

		skb = new;

		skb_push(skb, sizeof(struct udphdr));
		skb_reset_transport_header(skb);
		skb_copy_to_linear_data(skb, &uh, sizeof(struct udphdr));

		raw_dump_table(__func__, "raw UDP header dump",
			       (u8 *)&uh, sizeof(uh));

		hdr.nexthdr = UIP_PROTO_UDP;
	}

	hdr.payload_len = htons(skb->len);
=======
		return -EINVAL;

	switch (iphc1 & (LOWPAN_IPHC_M | LOWPAN_IPHC_DAC)) {
	case LOWPAN_IPHC_M | LOWPAN_IPHC_DAC:
		spin_lock_bh(&lowpan_dev(dev)->ctx.lock);
		ci = lowpan_iphc_ctx_get_by_id(dev, LOWPAN_IPHC_CID_DCI(cid));
		if (!ci) {
			spin_unlock_bh(&lowpan_dev(dev)->ctx.lock);
			return -EINVAL;
		}

		/* multicast with context */
		pr_debug("dest: context-based mcast compression\n");
		err = lowpan_uncompress_multicast_ctx_daddr(skb, ci,
							    &hdr.daddr,
							    iphc1 & LOWPAN_IPHC_DAM_MASK);
		spin_unlock_bh(&lowpan_dev(dev)->ctx.lock);
		break;
	case LOWPAN_IPHC_M:
		/* multicast */
		err = lowpan_uncompress_multicast_daddr(skb, &hdr.daddr,
							iphc1 & LOWPAN_IPHC_DAM_MASK);
		break;
	case LOWPAN_IPHC_DAC:
		spin_lock_bh(&lowpan_dev(dev)->ctx.lock);
		ci = lowpan_iphc_ctx_get_by_id(dev, LOWPAN_IPHC_CID_DCI(cid));
		if (!ci) {
			spin_unlock_bh(&lowpan_dev(dev)->ctx.lock);
			return -EINVAL;
		}

		/* Destination address context based uncompression */
		pr_debug("DAC bit is set. Handle context based destination address.\n");
		err = lowpan_iphc_uncompress_ctx_addr(skb, dev, ci, &hdr.daddr,
						      iphc1 & LOWPAN_IPHC_DAM_MASK,
						      daddr);
		spin_unlock_bh(&lowpan_dev(dev)->ctx.lock);
		break;
	default:
		err = lowpan_iphc_uncompress_addr(skb, dev, &hdr.daddr,
						  iphc1 & LOWPAN_IPHC_DAM_MASK,
						  daddr);
		pr_debug("dest: stateless compression mode %d dest %pI6c\n",
			 iphc1 & LOWPAN_IPHC_DAM_MASK, &hdr.daddr);
		break;
	}

	if (err)
		return -EINVAL;

	/* Next header data uncompression */
	if (iphc0 & LOWPAN_IPHC_NH) {
		err = lowpan_nhc_do_uncompression(skb, dev, &hdr);
		if (err < 0)
			return err;
	} else {
		err = skb_cow(skb, sizeof(hdr));
		if (unlikely(err))
			return err;
	}

	switch (lowpan_dev(dev)->lltype) {
	case LOWPAN_LLTYPE_IEEE802154:
		if (lowpan_802154_cb(skb)->d_size)
			hdr.payload_len = htons(lowpan_802154_cb(skb)->d_size -
						sizeof(struct ipv6hdr));
		else
			hdr.payload_len = htons(skb->len);
		break;
	default:
		hdr.payload_len = htons(skb->len);
		break;
	}
>>>>>>> v4.9.227

	pr_debug("skb headroom size = %d, data length = %d\n",
		 skb_headroom(skb), skb->len);

	pr_debug("IPv6 header dump:\n\tversion = %d\n\tlength  = %d\n\t"
		 "nexthdr = 0x%02x\n\thop_lim = %d\n\tdest    = %pI6c\n",
		hdr.version, ntohs(hdr.payload_len), hdr.nexthdr,
		hdr.hop_limit, &hdr.daddr);

<<<<<<< HEAD
	raw_dump_table(__func__, "raw header dump", (u8 *)&hdr, sizeof(hdr));

	return skb_deliver(skb, &hdr, dev, deliver_skb);

drop:
	kfree_skb(skb);
	return -EINVAL;
}
EXPORT_SYMBOL_GPL(lowpan_process_data);

static u8 lowpan_compress_addr_64(u8 **hc_ptr, u8 shift,
				  const struct in6_addr *ipaddr,
				  const unsigned char *lladdr)
{
	u8 val = 0;

	if (is_addr_mac_addr_based(ipaddr, lladdr)) {
		val = 3; /* 0-bits */
		pr_debug("address compression 0 bits\n");
	} else if (lowpan_is_iid_16_bit_compressable(ipaddr)) {
		/* compress IID to 16 bits xxxx::XXXX */
		lowpan_push_hc_data(hc_ptr, &ipaddr->s6_addr16[7], 2);
		val = 2; /* 16-bits */
		raw_dump_inline(NULL, "Compressed ipv6 addr is (16 bits)",
				*hc_ptr - 2, 2);
	} else {
		/* do not compress IID => xxxx::IID */
		lowpan_push_hc_data(hc_ptr, &ipaddr->s6_addr16[4], 8);
		val = 1; /* 64-bits */
		raw_dump_inline(NULL, "Compressed ipv6 addr is (64 bits)",
				*hc_ptr - 8, 8);
	}

	return rol8(val, shift);
}

static void compress_udp_header(u8 **hc_ptr, struct sk_buff *skb)
{
	struct udphdr *uh = udp_hdr(skb);
	u8 tmp;

	if (((ntohs(uh->source) & LOWPAN_NHC_UDP_4BIT_MASK) ==
	     LOWPAN_NHC_UDP_4BIT_PORT) &&
	    ((ntohs(uh->dest) & LOWPAN_NHC_UDP_4BIT_MASK) ==
	     LOWPAN_NHC_UDP_4BIT_PORT)) {
		pr_debug("UDP header: both ports compression to 4 bits\n");
		/* compression value */
		tmp = LOWPAN_NHC_UDP_CS_P_11;
		lowpan_push_hc_data(hc_ptr, &tmp, sizeof(tmp));
		/* source and destination port */
		tmp = ntohs(uh->dest) - LOWPAN_NHC_UDP_4BIT_PORT +
		      ((ntohs(uh->source) - LOWPAN_NHC_UDP_4BIT_PORT) << 4);
		lowpan_push_hc_data(hc_ptr, &tmp, sizeof(tmp));
	} else if ((ntohs(uh->dest) & LOWPAN_NHC_UDP_8BIT_MASK) ==
			LOWPAN_NHC_UDP_8BIT_PORT) {
		pr_debug("UDP header: remove 8 bits of dest\n");
		/* compression value */
		tmp = LOWPAN_NHC_UDP_CS_P_01;
		lowpan_push_hc_data(hc_ptr, &tmp, sizeof(tmp));
		/* source port */
		lowpan_push_hc_data(hc_ptr, &uh->source, sizeof(uh->source));
		/* destination port */
		tmp = ntohs(uh->dest) - LOWPAN_NHC_UDP_8BIT_PORT;
		lowpan_push_hc_data(hc_ptr, &tmp, sizeof(tmp));
	} else if ((ntohs(uh->source) & LOWPAN_NHC_UDP_8BIT_MASK) ==
			LOWPAN_NHC_UDP_8BIT_PORT) {
		pr_debug("UDP header: remove 8 bits of source\n");
		/* compression value */
		tmp = LOWPAN_NHC_UDP_CS_P_10;
		lowpan_push_hc_data(hc_ptr, &tmp, sizeof(tmp));
		/* source port */
		tmp = ntohs(uh->source) - LOWPAN_NHC_UDP_8BIT_PORT;
		lowpan_push_hc_data(hc_ptr, &tmp, sizeof(tmp));
		/* destination port */
		lowpan_push_hc_data(hc_ptr, &uh->dest, sizeof(uh->dest));
	} else {
		pr_debug("UDP header: can't compress\n");
		/* compression value */
		tmp = LOWPAN_NHC_UDP_CS_P_00;
		lowpan_push_hc_data(hc_ptr, &tmp, sizeof(tmp));
		/* source port */
		lowpan_push_hc_data(hc_ptr, &uh->source, sizeof(uh->source));
		/* destination port */
		lowpan_push_hc_data(hc_ptr, &uh->dest, sizeof(uh->dest));
	}

	/* checksum is always inline */
	lowpan_push_hc_data(hc_ptr, &uh->check, sizeof(uh->check));

	/* skip the UDP header */
	skb_pull(skb, sizeof(struct udphdr));
}

int lowpan_header_compress(struct sk_buff *skb, struct net_device *dev,
			   unsigned short type, const void *_daddr,
			   const void *_saddr, unsigned int len)
{
	u8 tmp, iphc0, iphc1, *hc_ptr;
	struct ipv6hdr *hdr;
	u8 head[100] = {};
	int addr_type;

	if (type != ETH_P_IPV6)
=======
	skb_push(skb, sizeof(hdr));
	skb_reset_mac_header(skb);
	skb_reset_network_header(skb);
	skb_copy_to_linear_data(skb, &hdr, sizeof(hdr));

	raw_dump_table(__func__, "raw header dump", (u8 *)&hdr, sizeof(hdr));

	return 0;
}
EXPORT_SYMBOL_GPL(lowpan_header_decompress);

static const u8 lowpan_iphc_dam_to_sam_value[] = {
	[LOWPAN_IPHC_DAM_00] = LOWPAN_IPHC_SAM_00,
	[LOWPAN_IPHC_DAM_01] = LOWPAN_IPHC_SAM_01,
	[LOWPAN_IPHC_DAM_10] = LOWPAN_IPHC_SAM_10,
	[LOWPAN_IPHC_DAM_11] = LOWPAN_IPHC_SAM_11,
};

static inline bool
lowpan_iphc_compress_ctx_802154_lladdr(const struct in6_addr *ipaddr,
				       const struct lowpan_iphc_ctx *ctx,
				       const void *lladdr)
{
	const struct ieee802154_addr *addr = lladdr;
	unsigned char extended_addr[EUI64_ADDR_LEN];
	bool lladdr_compress = false;
	struct in6_addr tmp = {};

	switch (addr->mode) {
	case IEEE802154_ADDR_LONG:
		ieee802154_le64_to_be64(&extended_addr, &addr->extended_addr);
		/* check for SAM/DAM = 11 */
		memcpy(&tmp.s6_addr[8], &extended_addr, EUI64_ADDR_LEN);
		/* second bit-flip (Universe/Local) is done according RFC2464 */
		tmp.s6_addr[8] ^= 0x02;
		/* context information are always used */
		ipv6_addr_prefix_copy(&tmp, &ctx->pfx, ctx->plen);
		if (ipv6_addr_equal(&tmp, ipaddr))
			lladdr_compress = true;
		break;
	case IEEE802154_ADDR_SHORT:
		tmp.s6_addr[11] = 0xFF;
		tmp.s6_addr[12] = 0xFE;
		ieee802154_le16_to_be16(&tmp.s6_addr16[7],
					&addr->short_addr);
		/* context information are always used */
		ipv6_addr_prefix_copy(&tmp, &ctx->pfx, ctx->plen);
		if (ipv6_addr_equal(&tmp, ipaddr))
			lladdr_compress = true;
		break;
	default:
		/* should never handled and filtered by 802154 6lowpan */
		WARN_ON_ONCE(1);
		break;
	}

	return lladdr_compress;
}

static u8 lowpan_compress_ctx_addr(u8 **hc_ptr, const struct net_device *dev,
				   const struct in6_addr *ipaddr,
				   const struct lowpan_iphc_ctx *ctx,
				   const unsigned char *lladdr, bool sam)
{
	struct in6_addr tmp = {};
	u8 dam;

	switch (lowpan_dev(dev)->lltype) {
	case LOWPAN_LLTYPE_IEEE802154:
		if (lowpan_iphc_compress_ctx_802154_lladdr(ipaddr, ctx,
							   lladdr)) {
			dam = LOWPAN_IPHC_DAM_11;
			goto out;
		}
		break;
	default:
		/* check for SAM/DAM = 11 */
		memcpy(&tmp.s6_addr[8], lladdr, EUI64_ADDR_LEN);
		/* second bit-flip (Universe/Local) is done according RFC2464 */
		tmp.s6_addr[8] ^= 0x02;
		/* context information are always used */
		ipv6_addr_prefix_copy(&tmp, &ctx->pfx, ctx->plen);
		if (ipv6_addr_equal(&tmp, ipaddr)) {
			dam = LOWPAN_IPHC_DAM_11;
			goto out;
		}
		break;
	}

	memset(&tmp, 0, sizeof(tmp));
	/* check for SAM/DAM = 10 */
	tmp.s6_addr[11] = 0xFF;
	tmp.s6_addr[12] = 0xFE;
	memcpy(&tmp.s6_addr[14], &ipaddr->s6_addr[14], 2);
	/* context information are always used */
	ipv6_addr_prefix_copy(&tmp, &ctx->pfx, ctx->plen);
	if (ipv6_addr_equal(&tmp, ipaddr)) {
		lowpan_push_hc_data(hc_ptr, &ipaddr->s6_addr[14], 2);
		dam = LOWPAN_IPHC_DAM_10;
		goto out;
	}

	memset(&tmp, 0, sizeof(tmp));
	/* check for SAM/DAM = 01, should always match */
	memcpy(&tmp.s6_addr[8], &ipaddr->s6_addr[8], 8);
	/* context information are always used */
	ipv6_addr_prefix_copy(&tmp, &ctx->pfx, ctx->plen);
	if (ipv6_addr_equal(&tmp, ipaddr)) {
		lowpan_push_hc_data(hc_ptr, &ipaddr->s6_addr[8], 8);
		dam = LOWPAN_IPHC_DAM_01;
		goto out;
	}

	WARN_ONCE(1, "context found but no address mode matched\n");
	return LOWPAN_IPHC_DAM_00;
out:

	if (sam)
		return lowpan_iphc_dam_to_sam_value[dam];
	else
		return dam;
}

static inline bool
lowpan_iphc_compress_802154_lladdr(const struct in6_addr *ipaddr,
				   const void *lladdr)
{
	const struct ieee802154_addr *addr = lladdr;
	unsigned char extended_addr[EUI64_ADDR_LEN];
	bool lladdr_compress = false;
	struct in6_addr tmp = {};

	switch (addr->mode) {
	case IEEE802154_ADDR_LONG:
		ieee802154_le64_to_be64(&extended_addr, &addr->extended_addr);
		if (is_addr_mac_addr_based(ipaddr, extended_addr))
			lladdr_compress = true;
		break;
	case IEEE802154_ADDR_SHORT:
		/* fe:80::ff:fe00:XXXX
		 *                \__/
		 *             short_addr
		 *
		 * Universe/Local bit is zero.
		 */
		tmp.s6_addr[0] = 0xFE;
		tmp.s6_addr[1] = 0x80;
		tmp.s6_addr[11] = 0xFF;
		tmp.s6_addr[12] = 0xFE;
		ieee802154_le16_to_be16(&tmp.s6_addr16[7],
					&addr->short_addr);
		if (ipv6_addr_equal(&tmp, ipaddr))
			lladdr_compress = true;
		break;
	default:
		/* should never handled and filtered by 802154 6lowpan */
		WARN_ON_ONCE(1);
		break;
	}

	return lladdr_compress;
}

static u8 lowpan_compress_addr_64(u8 **hc_ptr, const struct net_device *dev,
				  const struct in6_addr *ipaddr,
				  const unsigned char *lladdr, bool sam)
{
	u8 dam = LOWPAN_IPHC_DAM_01;

	switch (lowpan_dev(dev)->lltype) {
	case LOWPAN_LLTYPE_IEEE802154:
		if (lowpan_iphc_compress_802154_lladdr(ipaddr, lladdr)) {
			dam = LOWPAN_IPHC_DAM_11; /* 0-bits */
			pr_debug("address compression 0 bits\n");
			goto out;
		}
		break;
	default:
		if (is_addr_mac_addr_based(ipaddr, lladdr)) {
			dam = LOWPAN_IPHC_DAM_11; /* 0-bits */
			pr_debug("address compression 0 bits\n");
			goto out;
		}
		break;
	}

	if (lowpan_is_iid_16_bit_compressable(ipaddr)) {
		/* compress IID to 16 bits xxxx::XXXX */
		lowpan_push_hc_data(hc_ptr, &ipaddr->s6_addr16[7], 2);
		dam = LOWPAN_IPHC_DAM_10; /* 16-bits */
		raw_dump_inline(NULL, "Compressed ipv6 addr is (16 bits)",
				*hc_ptr - 2, 2);
		goto out;
	}

	/* do not compress IID => xxxx::IID */
	lowpan_push_hc_data(hc_ptr, &ipaddr->s6_addr16[4], 8);
	raw_dump_inline(NULL, "Compressed ipv6 addr is (64 bits)",
			*hc_ptr - 8, 8);

out:

	if (sam)
		return lowpan_iphc_dam_to_sam_value[dam];
	else
		return dam;
}

/* lowpan_iphc_get_tc - get the ECN + DCSP fields in hc format */
static inline u8 lowpan_iphc_get_tc(const struct ipv6hdr *hdr)
{
	u8 dscp, ecn;

	/* hdr->priority contains the higher bits of dscp, lower are part of
	 * flow_lbl[0]. Note ECN, DCSP is swapped in ipv6 hdr.
	 */
	dscp = (hdr->priority << 2) | ((hdr->flow_lbl[0] & 0xc0) >> 6);
	/* ECN is at the two lower bits from first nibble of flow_lbl[0] */
	ecn = (hdr->flow_lbl[0] & 0x30);
	/* for pretty debug output, also shift ecn to get the ecn value */
	pr_debug("ecn 0x%02x dscp 0x%02x\n", ecn >> 4, dscp);
	/* ECN is at 0x30 now, shift it to have ECN + DCSP */
	return (ecn << 2) | dscp;
}

/* lowpan_iphc_is_flow_lbl_zero - check if flow label is zero */
static inline bool lowpan_iphc_is_flow_lbl_zero(const struct ipv6hdr *hdr)
{
	return ((!(hdr->flow_lbl[0] & 0x0f)) &&
		!hdr->flow_lbl[1] && !hdr->flow_lbl[2]);
}

/* lowpan_iphc_tf_compress - compress the traffic class which is set by
 *	ipv6hdr. Return the corresponding format identifier which is used.
 */
static u8 lowpan_iphc_tf_compress(u8 **hc_ptr, const struct ipv6hdr *hdr)
{
	/* get ecn dscp data in a byteformat as: ECN(hi) + DSCP(lo) */
	u8 tc = lowpan_iphc_get_tc(hdr), tf[4], val;

	/* printout the traffic class in hc format */
	pr_debug("tc 0x%02x\n", tc);

	if (lowpan_iphc_is_flow_lbl_zero(hdr)) {
		if (!tc) {
			/* 11:  Traffic Class and Flow Label are elided. */
			val = LOWPAN_IPHC_TF_11;
		} else {
			/* 10:  ECN + DSCP (1 byte), Flow Label is elided.
			 *
			 *  0 1 2 3 4 5 6 7
			 * +-+-+-+-+-+-+-+-+
			 * |ECN|   DSCP    |
			 * +-+-+-+-+-+-+-+-+
			 */
			lowpan_push_hc_data(hc_ptr, &tc, sizeof(tc));
			val = LOWPAN_IPHC_TF_10;
		}
	} else {
		/* check if dscp is zero, it's after the first two bit */
		if (!(tc & 0x3f)) {
			/* 01:  ECN + 2-bit Pad + Flow Label (3 bytes), DSCP is elided
			 *
			 *                     1                   2
			 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
			 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			 * |ECN|rsv|             Flow Label                |
			 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			 */
			memcpy(&tf[0], &hdr->flow_lbl[0], 3);
			/* zero the highest 4-bits, contains DCSP + ECN */
			tf[0] &= ~0xf0;
			/* set ECN */
			tf[0] |= (tc & 0xc0);

			lowpan_push_hc_data(hc_ptr, tf, 3);
			val = LOWPAN_IPHC_TF_01;
		} else {
			/* 00:  ECN + DSCP + 4-bit Pad + Flow Label (4 bytes)
			 *
			 *                      1                   2                   3
			 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
			 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			 * |ECN|   DSCP    |  rsv  |             Flow Label                |
			 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
			 */
			memcpy(&tf[0], &tc, sizeof(tc));
			/* highest nibble of flow_lbl[0] is part of DSCP + ECN
			 * which will be the 4-bit pad and will be filled with
			 * zeros afterwards.
			 */
			memcpy(&tf[1], &hdr->flow_lbl[0], 3);
			/* zero the 4-bit pad, which is reserved */
			tf[1] &= ~0xf0;

			lowpan_push_hc_data(hc_ptr, tf, 4);
			val = LOWPAN_IPHC_TF_00;
		}
	}

	return val;
}

static u8 lowpan_iphc_mcast_ctx_addr_compress(u8 **hc_ptr,
					      const struct lowpan_iphc_ctx *ctx,
					      const struct in6_addr *ipaddr)
{
	u8 data[6];

	/* flags/scope, reserved (RIID) */
	memcpy(data, &ipaddr->s6_addr[1], 2);
	/* group ID */
	memcpy(&data[1], &ipaddr->s6_addr[11], 4);
	lowpan_push_hc_data(hc_ptr, data, 6);

	return LOWPAN_IPHC_DAM_00;
}

static u8 lowpan_iphc_mcast_addr_compress(u8 **hc_ptr,
					  const struct in6_addr *ipaddr)
{
	u8 val;

	if (lowpan_is_mcast_addr_compressable8(ipaddr)) {
		pr_debug("compressed to 1 octet\n");
		/* use last byte */
		lowpan_push_hc_data(hc_ptr, &ipaddr->s6_addr[15], 1);
		val = LOWPAN_IPHC_DAM_11;
	} else if (lowpan_is_mcast_addr_compressable32(ipaddr)) {
		pr_debug("compressed to 4 octets\n");
		/* second byte + the last three */
		lowpan_push_hc_data(hc_ptr, &ipaddr->s6_addr[1], 1);
		lowpan_push_hc_data(hc_ptr, &ipaddr->s6_addr[13], 3);
		val = LOWPAN_IPHC_DAM_10;
	} else if (lowpan_is_mcast_addr_compressable48(ipaddr)) {
		pr_debug("compressed to 6 octets\n");
		/* second byte + the last five */
		lowpan_push_hc_data(hc_ptr, &ipaddr->s6_addr[1], 1);
		lowpan_push_hc_data(hc_ptr, &ipaddr->s6_addr[11], 5);
		val = LOWPAN_IPHC_DAM_01;
	} else {
		pr_debug("using full address\n");
		lowpan_push_hc_data(hc_ptr, ipaddr->s6_addr, 16);
		val = LOWPAN_IPHC_DAM_00;
	}

	return val;
}

int lowpan_header_compress(struct sk_buff *skb, const struct net_device *dev,
			   const void *daddr, const void *saddr)
{
	u8 iphc0, iphc1, *hc_ptr, cid = 0;
	struct ipv6hdr *hdr;
	u8 head[LOWPAN_IPHC_MAX_HC_BUF_LEN] = {};
	struct lowpan_iphc_ctx *dci, *sci, dci_entry, sci_entry;
	int ret, ipv6_daddr_type, ipv6_saddr_type;

	if (skb->protocol != htons(ETH_P_IPV6))
>>>>>>> v4.9.227
		return -EINVAL;

	hdr = ipv6_hdr(skb);
	hc_ptr = head + 2;

	pr_debug("IPv6 header dump:\n\tversion = %d\n\tlength  = %d\n"
		 "\tnexthdr = 0x%02x\n\thop_lim = %d\n\tdest    = %pI6c\n",
		 hdr->version, ntohs(hdr->payload_len), hdr->nexthdr,
		 hdr->hop_limit, &hdr->daddr);

	raw_dump_table(__func__, "raw skb network header dump",
		       skb_network_header(skb), sizeof(struct ipv6hdr));

	/* As we copy some bit-length fields, in the IPHC encoding bytes,
	 * we sometimes use |=
	 * If the field is 0, and the current bit value in memory is 1,
	 * this does not work. We therefore reset the IPHC encoding here
	 */
	iphc0 = LOWPAN_DISPATCH_IPHC;
	iphc1 = 0;

<<<<<<< HEAD
	/* TODO: context lookup */

	raw_dump_inline(__func__, "saddr",
			(unsigned char *)_saddr, IEEE802154_ADDR_LEN);
	raw_dump_inline(__func__, "daddr",
			(unsigned char *)_daddr, IEEE802154_ADDR_LEN);

	raw_dump_table(__func__, "sending raw skb network uncompressed packet",
		       skb->data, skb->len);

	/* Traffic class, flow label
	 * If flow label is 0, compress it. If traffic class is 0, compress it
	 * We have to process both in the same time as the offset of traffic
	 * class depends on the presence of version and flow label
	 */

	/* hc format of TC is ECN | DSCP , original one is DSCP | ECN */
	tmp = (hdr->priority << 4) | (hdr->flow_lbl[0] >> 4);
	tmp = ((tmp & 0x03) << 6) | (tmp >> 2);

	if (((hdr->flow_lbl[0] & 0x0F) == 0) &&
	    (hdr->flow_lbl[1] == 0) && (hdr->flow_lbl[2] == 0)) {
		/* flow label can be compressed */
		iphc0 |= LOWPAN_IPHC_FL_C;
		if ((hdr->priority == 0) &&
		    ((hdr->flow_lbl[0] & 0xF0) == 0)) {
			/* compress (elide) all */
			iphc0 |= LOWPAN_IPHC_TC_C;
		} else {
			/* compress only the flow label */
			*hc_ptr = tmp;
			hc_ptr += 1;
		}
	} else {
		/* Flow label cannot be compressed */
		if ((hdr->priority == 0) &&
		    ((hdr->flow_lbl[0] & 0xF0) == 0)) {
			/* compress only traffic class */
			iphc0 |= LOWPAN_IPHC_TC_C;
			*hc_ptr = (tmp & 0xc0) | (hdr->flow_lbl[0] & 0x0F);
			memcpy(hc_ptr + 1, &hdr->flow_lbl[1], 2);
			hc_ptr += 3;
		} else {
			/* compress nothing */
			memcpy(hc_ptr, hdr, 4);
			/* replace the top byte with new ECN | DSCP format */
			*hc_ptr = tmp;
			hc_ptr += 4;
		}
	}

	/* NOTE: payload length is always compressed */

	/* Next Header is compress if UDP */
	if (hdr->nexthdr == UIP_PROTO_UDP)
		iphc0 |= LOWPAN_IPHC_NH_C;

	if ((iphc0 & LOWPAN_IPHC_NH_C) == 0)
		lowpan_push_hc_data(&hc_ptr, &hdr->nexthdr,
				    sizeof(hdr->nexthdr));
=======
	raw_dump_table(__func__, "sending raw skb network uncompressed packet",
		       skb->data, skb->len);

	ipv6_daddr_type = ipv6_addr_type(&hdr->daddr);
	spin_lock_bh(&lowpan_dev(dev)->ctx.lock);
	if (ipv6_daddr_type & IPV6_ADDR_MULTICAST)
		dci = lowpan_iphc_ctx_get_by_mcast_addr(dev, &hdr->daddr);
	else
		dci = lowpan_iphc_ctx_get_by_addr(dev, &hdr->daddr);
	if (dci) {
		memcpy(&dci_entry, dci, sizeof(*dci));
		cid |= dci->id;
	}
	spin_unlock_bh(&lowpan_dev(dev)->ctx.lock);

	spin_lock_bh(&lowpan_dev(dev)->ctx.lock);
	sci = lowpan_iphc_ctx_get_by_addr(dev, &hdr->saddr);
	if (sci) {
		memcpy(&sci_entry, sci, sizeof(*sci));
		cid |= (sci->id << 4);
	}
	spin_unlock_bh(&lowpan_dev(dev)->ctx.lock);

	/* if cid is zero it will be compressed */
	if (cid) {
		iphc1 |= LOWPAN_IPHC_CID;
		lowpan_push_hc_data(&hc_ptr, &cid, sizeof(cid));
	}

	/* Traffic Class, Flow Label compression */
	iphc0 |= lowpan_iphc_tf_compress(&hc_ptr, hdr);

	/* NOTE: payload length is always compressed */

	/* Check if we provide the nhc format for nexthdr and compression
	 * functionality. If not nexthdr is handled inline and not compressed.
	 */
	ret = lowpan_nhc_check_compression(skb, hdr, &hc_ptr);
	if (ret == -ENOENT)
		lowpan_push_hc_data(&hc_ptr, &hdr->nexthdr,
				    sizeof(hdr->nexthdr));
	else
		iphc0 |= LOWPAN_IPHC_NH;
>>>>>>> v4.9.227

	/* Hop limit
	 * if 1:   compress, encoding is 01
	 * if 64:  compress, encoding is 10
	 * if 255: compress, encoding is 11
	 * else do not compress
	 */
	switch (hdr->hop_limit) {
	case 1:
<<<<<<< HEAD
		iphc0 |= LOWPAN_IPHC_TTL_1;
		break;
	case 64:
		iphc0 |= LOWPAN_IPHC_TTL_64;
		break;
	case 255:
		iphc0 |= LOWPAN_IPHC_TTL_255;
=======
		iphc0 |= LOWPAN_IPHC_HLIM_01;
		break;
	case 64:
		iphc0 |= LOWPAN_IPHC_HLIM_10;
		break;
	case 255:
		iphc0 |= LOWPAN_IPHC_HLIM_11;
>>>>>>> v4.9.227
		break;
	default:
		lowpan_push_hc_data(&hc_ptr, &hdr->hop_limit,
				    sizeof(hdr->hop_limit));
	}

<<<<<<< HEAD
	addr_type = ipv6_addr_type(&hdr->saddr);
	/* source address compression */
	if (addr_type == IPV6_ADDR_ANY) {
		pr_debug("source address is unspecified, setting SAC\n");
		iphc1 |= LOWPAN_IPHC_SAC;
	} else {
		if (addr_type & IPV6_ADDR_LINKLOCAL) {
			iphc1 |= lowpan_compress_addr_64(&hc_ptr,
							 LOWPAN_IPHC_SAM_BIT,
							 &hdr->saddr, _saddr);
			pr_debug("source address unicast link-local %pI6c iphc1 0x%02x\n",
				 &hdr->saddr, iphc1);
		} else {
			pr_debug("send the full source address\n");
			lowpan_push_hc_data(&hc_ptr, hdr->saddr.s6_addr, 16);
		}
	}

	addr_type = ipv6_addr_type(&hdr->daddr);
	/* destination address compression */
	if (addr_type & IPV6_ADDR_MULTICAST) {
		pr_debug("destination address is multicast: ");
		iphc1 |= LOWPAN_IPHC_M;
		if (lowpan_is_mcast_addr_compressable8(&hdr->daddr)) {
			pr_debug("compressed to 1 octet\n");
			iphc1 |= LOWPAN_IPHC_DAM_11;
			/* use last byte */
			lowpan_push_hc_data(&hc_ptr,
					    &hdr->daddr.s6_addr[15], 1);
		} else if (lowpan_is_mcast_addr_compressable32(&hdr->daddr)) {
			pr_debug("compressed to 4 octets\n");
			iphc1 |= LOWPAN_IPHC_DAM_10;
			/* second byte + the last three */
			lowpan_push_hc_data(&hc_ptr,
					    &hdr->daddr.s6_addr[1], 1);
			lowpan_push_hc_data(&hc_ptr,
					    &hdr->daddr.s6_addr[13], 3);
		} else if (lowpan_is_mcast_addr_compressable48(&hdr->daddr)) {
			pr_debug("compressed to 6 octets\n");
			iphc1 |= LOWPAN_IPHC_DAM_01;
			/* second byte + the last five */
			lowpan_push_hc_data(&hc_ptr,
					    &hdr->daddr.s6_addr[1], 1);
			lowpan_push_hc_data(&hc_ptr,
					    &hdr->daddr.s6_addr[11], 5);
		} else {
			pr_debug("using full address\n");
			iphc1 |= LOWPAN_IPHC_DAM_00;
			lowpan_push_hc_data(&hc_ptr, hdr->daddr.s6_addr, 16);
		}
	} else {
		if (addr_type & IPV6_ADDR_LINKLOCAL) {
			/* TODO: context lookup */
			iphc1 |= lowpan_compress_addr_64(&hc_ptr,
				LOWPAN_IPHC_DAM_BIT, &hdr->daddr, _daddr);
			pr_debug("dest address unicast link-local %pI6c "
				 "iphc1 0x%02x\n", &hdr->daddr, iphc1);
		} else {
			pr_debug("dest address unicast %pI6c\n", &hdr->daddr);
			lowpan_push_hc_data(&hc_ptr, hdr->daddr.s6_addr, 16);
		}
	}

	/* UDP header compression */
	if (hdr->nexthdr == UIP_PROTO_UDP)
		compress_udp_header(&hc_ptr, skb);
=======
	ipv6_saddr_type = ipv6_addr_type(&hdr->saddr);
	/* source address compression */
	if (ipv6_saddr_type == IPV6_ADDR_ANY) {
		pr_debug("source address is unspecified, setting SAC\n");
		iphc1 |= LOWPAN_IPHC_SAC;
	} else {
		if (sci) {
			iphc1 |= lowpan_compress_ctx_addr(&hc_ptr, dev,
							  &hdr->saddr,
							  &sci_entry, saddr,
							  true);
			iphc1 |= LOWPAN_IPHC_SAC;
		} else {
			if (ipv6_saddr_type & IPV6_ADDR_LINKLOCAL &&
			    lowpan_is_linklocal_zero_padded(hdr->saddr)) {
				iphc1 |= lowpan_compress_addr_64(&hc_ptr, dev,
								 &hdr->saddr,
								 saddr, true);
				pr_debug("source address unicast link-local %pI6c iphc1 0x%02x\n",
					 &hdr->saddr, iphc1);
			} else {
				pr_debug("send the full source address\n");
				lowpan_push_hc_data(&hc_ptr,
						    hdr->saddr.s6_addr, 16);
			}
		}
	}

	/* destination address compression */
	if (ipv6_daddr_type & IPV6_ADDR_MULTICAST) {
		pr_debug("destination address is multicast: ");
		iphc1 |= LOWPAN_IPHC_M;
		if (dci) {
			iphc1 |= lowpan_iphc_mcast_ctx_addr_compress(&hc_ptr,
								     &dci_entry,
								     &hdr->daddr);
			iphc1 |= LOWPAN_IPHC_DAC;
		} else {
			iphc1 |= lowpan_iphc_mcast_addr_compress(&hc_ptr,
								 &hdr->daddr);
		}
	} else {
		if (dci) {
			iphc1 |= lowpan_compress_ctx_addr(&hc_ptr, dev,
							  &hdr->daddr,
							  &dci_entry, daddr,
							  false);
			iphc1 |= LOWPAN_IPHC_DAC;
		} else {
			if (ipv6_daddr_type & IPV6_ADDR_LINKLOCAL &&
			    lowpan_is_linklocal_zero_padded(hdr->daddr)) {
				iphc1 |= lowpan_compress_addr_64(&hc_ptr, dev,
								 &hdr->daddr,
								 daddr, false);
				pr_debug("dest address unicast link-local %pI6c iphc1 0x%02x\n",
					 &hdr->daddr, iphc1);
			} else {
				pr_debug("dest address unicast %pI6c\n",
					 &hdr->daddr);
				lowpan_push_hc_data(&hc_ptr,
						    hdr->daddr.s6_addr, 16);
			}
		}
	}

	/* next header compression */
	if (iphc0 & LOWPAN_IPHC_NH) {
		ret = lowpan_nhc_do_compression(skb, hdr, &hc_ptr);
		if (ret < 0)
			return ret;
	}
>>>>>>> v4.9.227

	head[0] = iphc0;
	head[1] = iphc1;

	skb_pull(skb, sizeof(struct ipv6hdr));
	skb_reset_transport_header(skb);
	memcpy(skb_push(skb, hc_ptr - head), head, hc_ptr - head);
	skb_reset_network_header(skb);

	pr_debug("header len %d skb %u\n", (int)(hc_ptr - head), skb->len);

	raw_dump_table(__func__, "raw skb data dump compressed",
		       skb->data, skb->len);
	return 0;
}
EXPORT_SYMBOL_GPL(lowpan_header_compress);
<<<<<<< HEAD

MODULE_LICENSE("GPL");
=======
>>>>>>> v4.9.227
