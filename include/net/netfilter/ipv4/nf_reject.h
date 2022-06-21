#ifndef _IPV4_NF_REJECT_H
#define _IPV4_NF_REJECT_H

#include <linux/skbuff.h>
#include <net/ip.h>
#include <net/icmp.h>

<<<<<<< HEAD
static inline void nf_send_unreach(struct sk_buff *skb_in, int code)
{
	icmp_send(skb_in, ICMP_DEST_UNREACH, code, 0);
}

void nf_send_reset(struct sk_buff *oldskb, int hook);
=======
void nf_send_unreach(struct sk_buff *skb_in, int code, int hook);
void nf_send_reset(struct net *net, struct sk_buff *oldskb, int hook);
>>>>>>> v4.9.227

const struct tcphdr *nf_reject_ip_tcphdr_get(struct sk_buff *oldskb,
					     struct tcphdr *_oth, int hook);
struct iphdr *nf_reject_iphdr_put(struct sk_buff *nskb,
				  const struct sk_buff *oldskb,
<<<<<<< HEAD
				  __be16 protocol, int ttl);
=======
				  __u8 protocol, int ttl);
>>>>>>> v4.9.227
void nf_reject_ip_tcphdr_put(struct sk_buff *nskb, const struct sk_buff *oldskb,
			     const struct tcphdr *oth);

#endif /* _IPV4_NF_REJECT_H */
