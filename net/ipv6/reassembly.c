/*
 *	IPv6 fragment reassembly
 *	Linux INET6 implementation
 *
 *	Authors:
 *	Pedro Roque		<roque@di.fc.ul.pt>
 *
 *	Based on: net/ipv4/ip_fragment.c
 *
 *	This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

/*
 *	Fixes:
 *	Andi Kleen	Make it work with multiple hosts.
 *			More RFC compliance.
 *
 *      Horst von Brand Add missing #include <linux/string.h>
 *	Alexey Kuznetsov	SMP races, threading, cleanup.
 *	Patrick McHardy		LRU queue of frag heads for evictor.
 *	Mitsuru KANDA @USAGI	Register inet6_protocol{}.
 *	David Stevens and
 *	YOSHIFUJI,H. @USAGI	Always remove fragment header to
 *				calculate ICV correctly.
 */

#define pr_fmt(fmt) "IPv6: " fmt

#include <linux/errno.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/jiffies.h>
#include <linux/net.h>
#include <linux/list.h>
#include <linux/netdevice.h>
#include <linux/in6.h>
#include <linux/ipv6.h>
#include <linux/icmpv6.h>
#include <linux/random.h>
#include <linux/jhash.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/export.h>

#include <net/sock.h>
#include <net/snmp.h>

#include <net/ipv6.h>
#include <net/ip6_route.h>
#include <net/protocol.h>
#include <net/transp_v6.h>
#include <net/rawv6.h>
#include <net/ndisc.h>
#include <net/addrconf.h>
<<<<<<< HEAD
#include <net/inet_frag.h>
=======
#include <net/ipv6_frag.h>
>>>>>>> v4.9.227
#include <net/inet_ecn.h>

static const char ip6_frag_cache_name[] = "ip6-frags";

<<<<<<< HEAD
struct ip6frag_skb_cb {
	struct inet6_skb_parm	h;
	int			offset;
};

#define FRAG6_CB(skb)	((struct ip6frag_skb_cb *)((skb)->cb))

static inline u8 ip6_frag_ecn(const struct ipv6hdr *ipv6h)
=======
static u8 ip6_frag_ecn(const struct ipv6hdr *ipv6h)
>>>>>>> v4.9.227
{
	return 1 << (ipv6_get_dsfield(ipv6h) & INET_ECN_MASK);
}

static struct inet_frags ip6_frags;

<<<<<<< HEAD
static int ip6_frag_reasm(struct frag_queue *fq, struct sk_buff *prev,
			  struct net_device *dev);

/*
 * callers should be careful not to use the hash value outside the ipfrag_lock
 * as doing so could race with ipfrag_hash_rnd being recalculated.
 */
static unsigned int inet6_hash_frag(__be32 id, const struct in6_addr *saddr,
				    const struct in6_addr *daddr)
{
	net_get_random_once(&ip6_frags.rnd, sizeof(ip6_frags.rnd));
	return jhash_3words(ipv6_addr_hash(saddr), ipv6_addr_hash(daddr),
			    (__force u32)id, ip6_frags.rnd);
}

static unsigned int ip6_hashfn(const struct inet_frag_queue *q)
{
	const struct frag_queue *fq;

	fq = container_of(q, struct frag_queue, q);
	return inet6_hash_frag(fq->id, &fq->saddr, &fq->daddr);
}

bool ip6_frag_match(const struct inet_frag_queue *q, const void *a)
{
	const struct frag_queue *fq;
	const struct ip6_create_arg *arg = a;

	fq = container_of(q, struct frag_queue, q);
	return	fq->id == arg->id &&
		fq->user == arg->user &&
		ipv6_addr_equal(&fq->saddr, arg->src) &&
		ipv6_addr_equal(&fq->daddr, arg->dst) &&
		(arg->iif == fq->iif ||
		 !(ipv6_addr_type(arg->dst) & (IPV6_ADDR_MULTICAST |
					       IPV6_ADDR_LINKLOCAL)));
}
EXPORT_SYMBOL(ip6_frag_match);

void ip6_frag_init(struct inet_frag_queue *q, const void *a)
{
	struct frag_queue *fq = container_of(q, struct frag_queue, q);
	const struct ip6_create_arg *arg = a;

	fq->id = arg->id;
	fq->user = arg->user;
	fq->saddr = *arg->src;
	fq->daddr = *arg->dst;
	fq->ecn = arg->ecn;
}
EXPORT_SYMBOL(ip6_frag_init);

void ip6_expire_frag_queue(struct net *net, struct frag_queue *fq,
			   struct inet_frags *frags)
{
	struct net_device *dev = NULL;

	spin_lock(&fq->q.lock);

	if (fq->q.flags & INET_FRAG_COMPLETE)
		goto out;

	inet_frag_kill(&fq->q, frags);

	rcu_read_lock();
	dev = dev_get_by_index_rcu(net, fq->iif);
	if (!dev)
		goto out_rcu_unlock;

	IP6_INC_STATS_BH(net, __in6_dev_get(dev), IPSTATS_MIB_REASMFAILS);

	if (fq->q.flags & INET_FRAG_EVICTED)
		goto out_rcu_unlock;

	IP6_INC_STATS_BH(net, __in6_dev_get(dev), IPSTATS_MIB_REASMTIMEOUT);

	/* Don't send error if the first segment did not arrive. */
	if (!(fq->q.flags & INET_FRAG_FIRST_IN) || !fq->q.fragments)
		goto out_rcu_unlock;

	/* But use as source device on which LAST ARRIVED
	 * segment was received. And do not use fq->dev
	 * pointer directly, device might already disappeared.
	 */
	fq->q.fragments->dev = dev;
	icmpv6_send(fq->q.fragments, ICMPV6_TIME_EXCEED, ICMPV6_EXC_FRAGTIME, 0);
out_rcu_unlock:
	rcu_read_unlock();
out:
	spin_unlock(&fq->q.lock);
	inet_frag_put(&fq->q, frags);
}
EXPORT_SYMBOL(ip6_expire_frag_queue);
=======
static int ip6_frag_reasm(struct frag_queue *fq, struct sk_buff *skb,
			  struct sk_buff *prev_tail, struct net_device *dev);
>>>>>>> v4.9.227

static void ip6_frag_expire(unsigned long data)
{
	struct frag_queue *fq;
	struct net *net;

	fq = container_of((struct inet_frag_queue *)data, struct frag_queue, q);
	net = container_of(fq->q.net, struct net, ipv6.frags);

<<<<<<< HEAD
	ip6_expire_frag_queue(net, fq, &ip6_frags);
}

static __inline__ struct frag_queue *
fq_find(struct net *net, __be32 id, const struct in6_addr *src,
	const struct in6_addr *dst, int iif, u8 ecn)
{
	struct inet_frag_queue *q;
	struct ip6_create_arg arg;
	unsigned int hash;

	arg.id = id;
	arg.user = IP6_DEFRAG_LOCAL_DELIVER;
	arg.src = src;
	arg.dst = dst;
	arg.iif = iif;
	arg.ecn = ecn;

	hash = inet6_hash_frag(id, src, dst);

	q = inet_frag_find(&net->ipv6.frags, &ip6_frags, &arg, hash);
	if (IS_ERR_OR_NULL(q)) {
		inet_frag_maybe_warn_overflow(q, pr_fmt());
		return NULL;
	}
=======
	ip6frag_expire_frag_queue(net, fq);
}

static struct frag_queue *
fq_find(struct net *net, __be32 id, const struct ipv6hdr *hdr, int iif)
{
	struct frag_v6_compare_key key = {
		.id = id,
		.saddr = hdr->saddr,
		.daddr = hdr->daddr,
		.user = IP6_DEFRAG_LOCAL_DELIVER,
		.iif = iif,
	};
	struct inet_frag_queue *q;

	if (!(ipv6_addr_type(&hdr->daddr) & (IPV6_ADDR_MULTICAST |
					    IPV6_ADDR_LINKLOCAL)))
		key.iif = 0;

	q = inet_frag_find(&net->ipv6.frags, &key);
	if (!q)
		return NULL;

>>>>>>> v4.9.227
	return container_of(q, struct frag_queue, q);
}

static int ip6_frag_queue(struct frag_queue *fq, struct sk_buff *skb,
<<<<<<< HEAD
			   struct frag_hdr *fhdr, int nhoff)
{
	struct sk_buff *prev, *next;
	struct net_device *dev;
	int offset, end;
	struct net *net = dev_net(skb_dst(skb)->dev);
=======
			  struct frag_hdr *fhdr, int nhoff,
			  u32 *prob_offset)
{
	struct net *net = dev_net(skb_dst(skb)->dev);
	int offset, end, fragsize;
	struct sk_buff *prev_tail;
	struct net_device *dev;
	int err = -ENOENT;
>>>>>>> v4.9.227
	u8 ecn;

	if (fq->q.flags & INET_FRAG_COMPLETE)
		goto err;

<<<<<<< HEAD
=======
	err = -EINVAL;
>>>>>>> v4.9.227
	offset = ntohs(fhdr->frag_off) & ~0x7;
	end = offset + (ntohs(ipv6_hdr(skb)->payload_len) -
			((u8 *)(fhdr + 1) - (u8 *)(ipv6_hdr(skb) + 1)));

	if ((unsigned int)end > IPV6_MAXPLEN) {
<<<<<<< HEAD
		IP6_INC_STATS_BH(net, ip6_dst_idev(skb_dst(skb)),
				 IPSTATS_MIB_INHDRERRORS);
		icmpv6_param_prob(skb, ICMPV6_HDR_FIELD,
				  ((u8 *)&fhdr->frag_off -
				   skb_network_header(skb)));
=======
		*prob_offset = (u8 *)&fhdr->frag_off - skb_network_header(skb);
		/* note that if prob_offset is set, the skb is freed elsewhere,
		 * we do not free it here.
		 */
>>>>>>> v4.9.227
		return -1;
	}

	ecn = ip6_frag_ecn(ipv6_hdr(skb));

	if (skb->ip_summed == CHECKSUM_COMPLETE) {
		const unsigned char *nh = skb_network_header(skb);
		skb->csum = csum_sub(skb->csum,
				     csum_partial(nh, (u8 *)(fhdr + 1) - nh,
						  0));
	}

	/* Is this the final fragment? */
	if (!(fhdr->frag_off & htons(IP6_MF))) {
		/* If we already have some bits beyond end
		 * or have different end, the segment is corrupted.
		 */
		if (end < fq->q.len ||
		    ((fq->q.flags & INET_FRAG_LAST_IN) && end != fq->q.len))
<<<<<<< HEAD
			goto err;
=======
			goto discard_fq;
>>>>>>> v4.9.227
		fq->q.flags |= INET_FRAG_LAST_IN;
		fq->q.len = end;
	} else {
		/* Check if the fragment is rounded to 8 bytes.
		 * Required by the RFC.
		 */
		if (end & 0x7) {
			/* RFC2460 says always send parameter problem in
			 * this case. -DaveM
			 */
<<<<<<< HEAD
			IP6_INC_STATS_BH(net, ip6_dst_idev(skb_dst(skb)),
					 IPSTATS_MIB_INHDRERRORS);
			icmpv6_param_prob(skb, ICMPV6_HDR_FIELD,
					  offsetof(struct ipv6hdr, payload_len));
=======
			*prob_offset = offsetof(struct ipv6hdr, payload_len);
>>>>>>> v4.9.227
			return -1;
		}
		if (end > fq->q.len) {
			/* Some bits beyond end -> corruption. */
			if (fq->q.flags & INET_FRAG_LAST_IN)
<<<<<<< HEAD
				goto err;
=======
				goto discard_fq;
>>>>>>> v4.9.227
			fq->q.len = end;
		}
	}

	if (end == offset)
<<<<<<< HEAD
		goto err;

	/* Point into the IP datagram 'data' part. */
	if (!pskb_pull(skb, (u8 *) (fhdr + 1) - skb->data))
		goto err;

	if (pskb_trim_rcsum(skb, end - offset))
		goto err;

	/* Find out which fragments are in front and at the back of us
	 * in the chain of fragments so far.  We must know where to put
	 * this fragment, right?
	 */
	prev = fq->q.fragments_tail;
	if (!prev || FRAG6_CB(prev)->offset < offset) {
		next = NULL;
		goto found;
	}
	prev = NULL;
	for (next = fq->q.fragments; next != NULL; next = next->next) {
		if (FRAG6_CB(next)->offset >= offset)
			break;	/* bingo! */
		prev = next;
	}

found:
	/* RFC5722, Section 4, amended by Errata ID : 3089
	 *                          When reassembling an IPv6 datagram, if
	 *   one or more its constituent fragments is determined to be an
	 *   overlapping fragment, the entire datagram (and any constituent
	 *   fragments) MUST be silently discarded.
	 */

	/* Check for overlap with preceding fragment. */
	if (prev &&
	    (FRAG6_CB(prev)->offset + prev->len) > offset)
		goto discard_fq;

	/* Look for overlap with succeeding segment. */
	if (next && FRAG6_CB(next)->offset < end)
		goto discard_fq;

	FRAG6_CB(skb)->offset = offset;

	/* Insert this fragment in the chain of fragments. */
	skb->next = next;
	if (!next)
		fq->q.fragments_tail = skb;
	if (prev)
		prev->next = skb;
	else
		fq->q.fragments = skb;

	dev = skb->dev;
	if (dev) {
		fq->iif = dev->ifindex;
		skb->dev = NULL;
	}
	fq->q.stamp = skb->tstamp;
	fq->q.meat += skb->len;
	fq->ecn |= ecn;
	add_frag_mem_limit(&fq->q, skb->truesize);
=======
		goto discard_fq;

	err = -ENOMEM;
	/* Point into the IP datagram 'data' part. */
	if (!pskb_pull(skb, (u8 *) (fhdr + 1) - skb->data))
		goto discard_fq;

	err = pskb_trim_rcsum(skb, end - offset);
	if (err)
		goto discard_fq;

	/* Note : skb->rbnode and skb->dev share the same location. */
	dev = skb->dev;
	/* Makes sure compiler wont do silly aliasing games */
	barrier();

	prev_tail = fq->q.fragments_tail;
	err = inet_frag_queue_insert(&fq->q, skb, offset, end);
	if (err)
		goto insert_error;

	if (dev)
		fq->iif = dev->ifindex;

	fq->q.stamp = skb->tstamp;
	fq->q.meat += skb->len;
	fq->ecn |= ecn;
	add_frag_mem_limit(fq->q.net, skb->truesize);

	fragsize = -skb_network_offset(skb) + skb->len;
	if (fragsize > fq->q.max_size)
		fq->q.max_size = fragsize;
>>>>>>> v4.9.227

	/* The first fragment.
	 * nhoffset is obtained from the first fragment, of course.
	 */
	if (offset == 0) {
		fq->nhoffset = nhoff;
		fq->q.flags |= INET_FRAG_FIRST_IN;
	}

	if (fq->q.flags == (INET_FRAG_FIRST_IN | INET_FRAG_LAST_IN) &&
	    fq->q.meat == fq->q.len) {
<<<<<<< HEAD
		int res;
		unsigned long orefdst = skb->_skb_refdst;

		skb->_skb_refdst = 0UL;
		res = ip6_frag_reasm(fq, prev, dev);
		skb->_skb_refdst = orefdst;
		return res;
	}

	skb_dst_drop(skb);
	return -1;

discard_fq:
	inet_frag_kill(&fq->q, &ip6_frags);
err:
	IP6_INC_STATS_BH(net, ip6_dst_idev(skb_dst(skb)),
			 IPSTATS_MIB_REASMFAILS);
	kfree_skb(skb);
	return -1;
=======
		unsigned long orefdst = skb->_skb_refdst;

		skb->_skb_refdst = 0UL;
		err = ip6_frag_reasm(fq, skb, prev_tail, dev);
		skb->_skb_refdst = orefdst;
		return err;
	}

	skb_dst_drop(skb);
	return -EINPROGRESS;

insert_error:
	if (err == IPFRAG_DUP) {
		kfree_skb(skb);
		return -EINVAL;
	}
	err = -EINVAL;
	__IP6_INC_STATS(net, ip6_dst_idev(skb_dst(skb)),
			IPSTATS_MIB_REASM_OVERLAPS);
discard_fq:
	inet_frag_kill(&fq->q);
	__IP6_INC_STATS(net, ip6_dst_idev(skb_dst(skb)),
			IPSTATS_MIB_REASMFAILS);
err:
	kfree_skb(skb);
	return err;
>>>>>>> v4.9.227
}

/*
 *	Check if this packet is complete.
<<<<<<< HEAD
 *	Returns NULL on failure by any reason, and pointer
 *	to current nexthdr field in reassembled frame.
=======
>>>>>>> v4.9.227
 *
 *	It is called with locked fq, and caller must check that
 *	queue is eligible for reassembly i.e. it is not COMPLETE,
 *	the last and the first frames arrived and all the bits are here.
 */
<<<<<<< HEAD
static int ip6_frag_reasm(struct frag_queue *fq, struct sk_buff *prev,
			  struct net_device *dev)
{
	struct net *net = container_of(fq->q.net, struct net, ipv6.frags);
	struct sk_buff *fp, *head = fq->q.fragments;
	int    payload_len;
	unsigned int nhoff;
	int sum_truesize;
	u8 ecn;

	inet_frag_kill(&fq->q, &ip6_frags);
=======
static int ip6_frag_reasm(struct frag_queue *fq, struct sk_buff *skb,
			  struct sk_buff *prev_tail, struct net_device *dev)
{
	struct net *net = container_of(fq->q.net, struct net, ipv6.frags);
	unsigned int nhoff;
	void *reasm_data;
	int payload_len;
	u8 ecn;

	inet_frag_kill(&fq->q);
>>>>>>> v4.9.227

	ecn = ip_frag_ecn_table[fq->ecn];
	if (unlikely(ecn == 0xff))
		goto out_fail;

<<<<<<< HEAD
	/* Make the one we just received the head. */
	if (prev) {
		head = prev->next;
		fp = skb_clone(head, GFP_ATOMIC);

		if (!fp)
			goto out_oom;

		fp->next = head->next;
		if (!fp->next)
			fq->q.fragments_tail = fp;
		prev->next = fp;

		skb_morph(head, fq->q.fragments);
		head->next = fq->q.fragments->next;

		consume_skb(fq->q.fragments);
		fq->q.fragments = head;
	}

	WARN_ON(head == NULL);
	WARN_ON(FRAG6_CB(head)->offset != 0);

	/* Unfragmented part is taken from the first segment. */
	payload_len = ((head->data - skb_network_header(head)) -
=======
	reasm_data = inet_frag_reasm_prepare(&fq->q, skb, prev_tail);
	if (!reasm_data)
		goto out_oom;

	payload_len = ((skb->data - skb_network_header(skb)) -
>>>>>>> v4.9.227
		       sizeof(struct ipv6hdr) + fq->q.len -
		       sizeof(struct frag_hdr));
	if (payload_len > IPV6_MAXPLEN)
		goto out_oversize;

<<<<<<< HEAD
	/* Head of list must not be cloned. */
	if (skb_unclone(head, GFP_ATOMIC))
		goto out_oom;

	/* If the first fragment is fragmented itself, we split
	 * it to two chunks: the first with data and paged part
	 * and the second, holding only fragments. */
	if (skb_has_frag_list(head)) {
		struct sk_buff *clone;
		int i, plen = 0;

		if ((clone = alloc_skb(0, GFP_ATOMIC)) == NULL)
			goto out_oom;
		clone->next = head->next;
		head->next = clone;
		skb_shinfo(clone)->frag_list = skb_shinfo(head)->frag_list;
		skb_frag_list_init(head);
		for (i = 0; i < skb_shinfo(head)->nr_frags; i++)
			plen += skb_frag_size(&skb_shinfo(head)->frags[i]);
		clone->len = clone->data_len = head->data_len - plen;
		head->data_len -= clone->len;
		head->len -= clone->len;
		clone->csum = 0;
		clone->ip_summed = head->ip_summed;
		add_frag_mem_limit(&fq->q, clone->truesize);
	}

	/* We have to remove fragment header from datagram and to relocate
	 * header in order to calculate ICV correctly. */
	nhoff = fq->nhoffset;
	skb_network_header(head)[nhoff] = skb_transport_header(head)[0];
	memmove(head->head + sizeof(struct frag_hdr), head->head,
		(head->data - head->head) - sizeof(struct frag_hdr));
	head->mac_header += sizeof(struct frag_hdr);
	head->network_header += sizeof(struct frag_hdr);

	skb_reset_transport_header(head);
	skb_push(head, head->data - skb_network_header(head));

	sum_truesize = head->truesize;
	for (fp = head->next; fp;) {
		bool headstolen;
		int delta;
		struct sk_buff *next = fp->next;

		sum_truesize += fp->truesize;
		if (head->ip_summed != fp->ip_summed)
			head->ip_summed = CHECKSUM_NONE;
		else if (head->ip_summed == CHECKSUM_COMPLETE)
			head->csum = csum_add(head->csum, fp->csum);

		if (skb_try_coalesce(head, fp, &headstolen, &delta)) {
			kfree_skb_partial(fp, headstolen);
		} else {
			if (!skb_shinfo(head)->frag_list)
				skb_shinfo(head)->frag_list = fp;
			head->data_len += fp->len;
			head->len += fp->len;
			head->truesize += fp->truesize;
		}
		fp = next;
	}
	sub_frag_mem_limit(&fq->q, sum_truesize);

	head->next = NULL;
	head->dev = dev;
	head->tstamp = fq->q.stamp;
	ipv6_hdr(head)->payload_len = htons(payload_len);
	ipv6_change_dsfield(ipv6_hdr(head), 0xff, ecn);
	IP6CB(head)->nhoff = nhoff;
	IP6CB(head)->flags |= IP6SKB_FRAGMENTED;

	/* Yes, and fold redundant checksum back. 8) */
	if (head->ip_summed == CHECKSUM_COMPLETE)
		head->csum = csum_partial(skb_network_header(head),
					  skb_network_header_len(head),
					  head->csum);

	rcu_read_lock();
	IP6_INC_STATS_BH(net, __in6_dev_get(dev), IPSTATS_MIB_REASMOKS);
	rcu_read_unlock();
	fq->q.fragments = NULL;
	fq->q.fragments_tail = NULL;
=======
	/* We have to remove fragment header from datagram and to relocate
	 * header in order to calculate ICV correctly. */
	nhoff = fq->nhoffset;
	skb_network_header(skb)[nhoff] = skb_transport_header(skb)[0];
	memmove(skb->head + sizeof(struct frag_hdr), skb->head,
		(skb->data - skb->head) - sizeof(struct frag_hdr));
	if (skb_mac_header_was_set(skb))
		skb->mac_header += sizeof(struct frag_hdr);
	skb->network_header += sizeof(struct frag_hdr);

	skb_reset_transport_header(skb);

	inet_frag_reasm_finish(&fq->q, skb, reasm_data);

	skb->dev = dev;
	ipv6_hdr(skb)->payload_len = htons(payload_len);
	ipv6_change_dsfield(ipv6_hdr(skb), 0xff, ecn);
	IP6CB(skb)->nhoff = nhoff;
	IP6CB(skb)->flags |= IP6SKB_FRAGMENTED;
	IP6CB(skb)->frag_max_size = fq->q.max_size;

	/* Yes, and fold redundant checksum back. 8) */
	skb_postpush_rcsum(skb, skb_network_header(skb),
			   skb_network_header_len(skb));

	rcu_read_lock();
	__IP6_INC_STATS(net, __in6_dev_get(dev), IPSTATS_MIB_REASMOKS);
	rcu_read_unlock();
	fq->q.fragments = NULL;
	fq->q.rb_fragments = RB_ROOT;
	fq->q.fragments_tail = NULL;
	fq->q.last_run_head = NULL;
>>>>>>> v4.9.227
	return 1;

out_oversize:
	net_dbg_ratelimited("ip6_frag_reasm: payload len = %d\n", payload_len);
	goto out_fail;
out_oom:
	net_dbg_ratelimited("ip6_frag_reasm: no memory for reassembly\n");
out_fail:
	rcu_read_lock();
<<<<<<< HEAD
	IP6_INC_STATS_BH(net, __in6_dev_get(dev), IPSTATS_MIB_REASMFAILS);
	rcu_read_unlock();
=======
	__IP6_INC_STATS(net, __in6_dev_get(dev), IPSTATS_MIB_REASMFAILS);
	rcu_read_unlock();
	inet_frag_kill(&fq->q);
>>>>>>> v4.9.227
	return -1;
}

static int ipv6_frag_rcv(struct sk_buff *skb)
{
	struct frag_hdr *fhdr;
	struct frag_queue *fq;
	const struct ipv6hdr *hdr = ipv6_hdr(skb);
	struct net *net = dev_net(skb_dst(skb)->dev);
<<<<<<< HEAD
=======
	int iif;
>>>>>>> v4.9.227

	if (IP6CB(skb)->flags & IP6SKB_FRAGMENTED)
		goto fail_hdr;

<<<<<<< HEAD
	IP6_INC_STATS_BH(net, ip6_dst_idev(skb_dst(skb)), IPSTATS_MIB_REASMREQDS);
=======
	__IP6_INC_STATS(net, ip6_dst_idev(skb_dst(skb)), IPSTATS_MIB_REASMREQDS);
>>>>>>> v4.9.227

	/* Jumbo payload inhibits frag. header */
	if (hdr->payload_len == 0)
		goto fail_hdr;

	if (!pskb_may_pull(skb, (skb_transport_offset(skb) +
				 sizeof(struct frag_hdr))))
		goto fail_hdr;

	hdr = ipv6_hdr(skb);
	fhdr = (struct frag_hdr *)skb_transport_header(skb);

	if (!(fhdr->frag_off & htons(0xFFF9))) {
		/* It is not a fragmented frame */
		skb->transport_header += sizeof(struct frag_hdr);
<<<<<<< HEAD
		IP6_INC_STATS_BH(net,
				 ip6_dst_idev(skb_dst(skb)), IPSTATS_MIB_REASMOKS);
=======
		__IP6_INC_STATS(net,
				ip6_dst_idev(skb_dst(skb)), IPSTATS_MIB_REASMOKS);
>>>>>>> v4.9.227

		IP6CB(skb)->nhoff = (u8 *)fhdr - skb_network_header(skb);
		IP6CB(skb)->flags |= IP6SKB_FRAGMENTED;
		return 1;
	}

<<<<<<< HEAD
	fq = fq_find(net, fhdr->identification, &hdr->saddr, &hdr->daddr,
		     skb->dev ? skb->dev->ifindex : 0, ip6_frag_ecn(hdr));
	if (fq != NULL) {
=======
	iif = skb->dev ? skb->dev->ifindex : 0;
	fq = fq_find(net, fhdr->identification, hdr, iif);
	if (fq) {
		u32 prob_offset = 0;
>>>>>>> v4.9.227
		int ret;

		spin_lock(&fq->q.lock);

<<<<<<< HEAD
		ret = ip6_frag_queue(fq, skb, fhdr, IP6CB(skb)->nhoff);

		spin_unlock(&fq->q.lock);
		inet_frag_put(&fq->q, &ip6_frags);
		return ret;
	}

	IP6_INC_STATS_BH(net, ip6_dst_idev(skb_dst(skb)), IPSTATS_MIB_REASMFAILS);
=======
		fq->iif = iif;
		ret = ip6_frag_queue(fq, skb, fhdr, IP6CB(skb)->nhoff,
				     &prob_offset);

		spin_unlock(&fq->q.lock);
		inet_frag_put(&fq->q);
		if (prob_offset) {
			__IP6_INC_STATS(net, ip6_dst_idev(skb_dst(skb)),
					IPSTATS_MIB_INHDRERRORS);
			/* icmpv6_param_prob() calls kfree_skb(skb) */
			icmpv6_param_prob(skb, ICMPV6_HDR_FIELD, prob_offset);
		}
		return ret;
	}

	__IP6_INC_STATS(net, ip6_dst_idev(skb_dst(skb)), IPSTATS_MIB_REASMFAILS);
>>>>>>> v4.9.227
	kfree_skb(skb);
	return -1;

fail_hdr:
<<<<<<< HEAD
	IP6_INC_STATS_BH(net, ip6_dst_idev(skb_dst(skb)),
			 IPSTATS_MIB_INHDRERRORS);
=======
	__IP6_INC_STATS(net, ip6_dst_idev(skb_dst(skb)),
			IPSTATS_MIB_INHDRERRORS);
>>>>>>> v4.9.227
	icmpv6_param_prob(skb, ICMPV6_HDR_FIELD, skb_network_header_len(skb));
	return -1;
}

static const struct inet6_protocol frag_protocol = {
	.handler	=	ipv6_frag_rcv,
	.flags		=	INET6_PROTO_NOPOLICY,
};

#ifdef CONFIG_SYSCTL
<<<<<<< HEAD
static int zero;
=======
>>>>>>> v4.9.227

static struct ctl_table ip6_frags_ns_ctl_table[] = {
	{
		.procname	= "ip6frag_high_thresh",
		.data		= &init_net.ipv6.frags.high_thresh,
<<<<<<< HEAD
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
=======
		.maxlen		= sizeof(unsigned long),
		.mode		= 0644,
		.proc_handler	= proc_doulongvec_minmax,
>>>>>>> v4.9.227
		.extra1		= &init_net.ipv6.frags.low_thresh
	},
	{
		.procname	= "ip6frag_low_thresh",
		.data		= &init_net.ipv6.frags.low_thresh,
<<<<<<< HEAD
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_minmax,
		.extra1		= &zero,
=======
		.maxlen		= sizeof(unsigned long),
		.mode		= 0644,
		.proc_handler	= proc_doulongvec_minmax,
>>>>>>> v4.9.227
		.extra2		= &init_net.ipv6.frags.high_thresh
	},
	{
		.procname	= "ip6frag_time",
		.data		= &init_net.ipv6.frags.timeout,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_jiffies,
	},
	{ }
};

/* secret interval has been deprecated */
static int ip6_frags_secret_interval_unused;
static struct ctl_table ip6_frags_ctl_table[] = {
	{
		.procname	= "ip6frag_secret_interval",
		.data		= &ip6_frags_secret_interval_unused,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= proc_dointvec_jiffies,
	},
	{ }
};

static int __net_init ip6_frags_ns_sysctl_register(struct net *net)
{
	struct ctl_table *table;
	struct ctl_table_header *hdr;

	table = ip6_frags_ns_ctl_table;
	if (!net_eq(net, &init_net)) {
		table = kmemdup(table, sizeof(ip6_frags_ns_ctl_table), GFP_KERNEL);
<<<<<<< HEAD
		if (table == NULL)
=======
		if (!table)
>>>>>>> v4.9.227
			goto err_alloc;

		table[0].data = &net->ipv6.frags.high_thresh;
		table[0].extra1 = &net->ipv6.frags.low_thresh;
		table[0].extra2 = &init_net.ipv6.frags.high_thresh;
		table[1].data = &net->ipv6.frags.low_thresh;
		table[1].extra2 = &net->ipv6.frags.high_thresh;
		table[2].data = &net->ipv6.frags.timeout;
<<<<<<< HEAD

		/* Don't export sysctls to unprivileged users */
		if (net->user_ns != &init_user_ns)
			table[0].procname = NULL;
	}

	hdr = register_net_sysctl(net, "net/ipv6", table);
	if (hdr == NULL)
=======
	}

	hdr = register_net_sysctl(net, "net/ipv6", table);
	if (!hdr)
>>>>>>> v4.9.227
		goto err_reg;

	net->ipv6.sysctl.frags_hdr = hdr;
	return 0;

err_reg:
	if (!net_eq(net, &init_net))
		kfree(table);
err_alloc:
	return -ENOMEM;
}

static void __net_exit ip6_frags_ns_sysctl_unregister(struct net *net)
{
	struct ctl_table *table;

	table = net->ipv6.sysctl.frags_hdr->ctl_table_arg;
	unregister_net_sysctl_table(net->ipv6.sysctl.frags_hdr);
	if (!net_eq(net, &init_net))
		kfree(table);
}

static struct ctl_table_header *ip6_ctl_header;

static int ip6_frags_sysctl_register(void)
{
	ip6_ctl_header = register_net_sysctl(&init_net, "net/ipv6",
			ip6_frags_ctl_table);
	return ip6_ctl_header == NULL ? -ENOMEM : 0;
}

static void ip6_frags_sysctl_unregister(void)
{
	unregister_net_sysctl_table(ip6_ctl_header);
}
#else
<<<<<<< HEAD
static inline int ip6_frags_ns_sysctl_register(struct net *net)
=======
static int ip6_frags_ns_sysctl_register(struct net *net)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static inline void ip6_frags_ns_sysctl_unregister(struct net *net)
{
}

static inline int ip6_frags_sysctl_register(void)
=======
static void ip6_frags_ns_sysctl_unregister(struct net *net)
{
}

static int ip6_frags_sysctl_register(void)
>>>>>>> v4.9.227
{
	return 0;
}

<<<<<<< HEAD
static inline void ip6_frags_sysctl_unregister(void)
=======
static void ip6_frags_sysctl_unregister(void)
>>>>>>> v4.9.227
{
}
#endif

static int __net_init ipv6_frags_init_net(struct net *net)
{
<<<<<<< HEAD
	net->ipv6.frags.high_thresh = IPV6_FRAG_HIGH_THRESH;
	net->ipv6.frags.low_thresh = IPV6_FRAG_LOW_THRESH;
	net->ipv6.frags.timeout = IPV6_FRAG_TIMEOUT;

	inet_frags_init_net(&net->ipv6.frags);

	return ip6_frags_ns_sysctl_register(net);
=======
	int res;

	net->ipv6.frags.high_thresh = IPV6_FRAG_HIGH_THRESH;
	net->ipv6.frags.low_thresh = IPV6_FRAG_LOW_THRESH;
	net->ipv6.frags.timeout = IPV6_FRAG_TIMEOUT;
	net->ipv6.frags.f = &ip6_frags;

	res = inet_frags_init_net(&net->ipv6.frags);
	if (res < 0)
		return res;

	res = ip6_frags_ns_sysctl_register(net);
	if (res < 0)
		inet_frags_exit_net(&net->ipv6.frags);
	return res;
>>>>>>> v4.9.227
}

static void __net_exit ipv6_frags_exit_net(struct net *net)
{
	ip6_frags_ns_sysctl_unregister(net);
<<<<<<< HEAD
	inet_frags_exit_net(&net->ipv6.frags, &ip6_frags);
=======
	inet_frags_exit_net(&net->ipv6.frags);
>>>>>>> v4.9.227
}

static struct pernet_operations ip6_frags_ops = {
	.init = ipv6_frags_init_net,
	.exit = ipv6_frags_exit_net,
};

<<<<<<< HEAD
=======
static const struct rhashtable_params ip6_rhash_params = {
	.head_offset		= offsetof(struct inet_frag_queue, node),
	.hashfn			= ip6frag_key_hashfn,
	.obj_hashfn		= ip6frag_obj_hashfn,
	.obj_cmpfn		= ip6frag_obj_cmpfn,
	.automatic_shrinking	= true,
};

>>>>>>> v4.9.227
int __init ipv6_frag_init(void)
{
	int ret;

<<<<<<< HEAD
	ret = inet6_add_protocol(&frag_protocol, IPPROTO_FRAGMENT);
	if (ret)
		goto out;

=======
	ip6_frags.constructor = ip6frag_init;
	ip6_frags.destructor = NULL;
	ip6_frags.qsize = sizeof(struct frag_queue);
	ip6_frags.frag_expire = ip6_frag_expire;
	ip6_frags.frags_cache_name = ip6_frag_cache_name;
	ip6_frags.rhash_params = ip6_rhash_params;
	ret = inet_frags_init(&ip6_frags);
	if (ret)
		goto out;

	ret = inet6_add_protocol(&frag_protocol, IPPROTO_FRAGMENT);
	if (ret)
		goto err_protocol;

>>>>>>> v4.9.227
	ret = ip6_frags_sysctl_register();
	if (ret)
		goto err_sysctl;

	ret = register_pernet_subsys(&ip6_frags_ops);
	if (ret)
		goto err_pernet;

<<<<<<< HEAD
	ip6_frags.hashfn = ip6_hashfn;
	ip6_frags.constructor = ip6_frag_init;
	ip6_frags.destructor = NULL;
	ip6_frags.skb_free = NULL;
	ip6_frags.qsize = sizeof(struct frag_queue);
	ip6_frags.match = ip6_frag_match;
	ip6_frags.frag_expire = ip6_frag_expire;
	ip6_frags.frags_cache_name = ip6_frag_cache_name;
	ret = inet_frags_init(&ip6_frags);
	if (ret)
		goto err_pernet;
=======
>>>>>>> v4.9.227
out:
	return ret;

err_pernet:
	ip6_frags_sysctl_unregister();
err_sysctl:
	inet6_del_protocol(&frag_protocol, IPPROTO_FRAGMENT);
<<<<<<< HEAD
=======
err_protocol:
	inet_frags_fini(&ip6_frags);
>>>>>>> v4.9.227
	goto out;
}

void ipv6_frag_exit(void)
{
<<<<<<< HEAD
	inet_frags_fini(&ip6_frags);
	ip6_frags_sysctl_unregister();
	unregister_pernet_subsys(&ip6_frags_ops);
	inet6_del_protocol(&frag_protocol, IPPROTO_FRAGMENT);
=======
	ip6_frags_sysctl_unregister();
	unregister_pernet_subsys(&ip6_frags_ops);
	inet6_del_protocol(&frag_protocol, IPPROTO_FRAGMENT);
	inet_frags_fini(&ip6_frags);
>>>>>>> v4.9.227
}
