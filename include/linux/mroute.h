#ifndef __LINUX_MROUTE_H
#define __LINUX_MROUTE_H

#include <linux/in.h>
#include <linux/pim.h>
#include <net/sock.h>
#include <uapi/linux/mroute.h>

#ifdef CONFIG_IP_MROUTE
static inline int ip_mroute_opt(int opt)
{
<<<<<<< HEAD
	return (opt >= MRT_BASE) && (opt <= MRT_MAX);
}
#else
static inline int ip_mroute_opt(int opt)
{
	return 0;
}
#endif

#ifdef CONFIG_IP_MROUTE
extern int ip_mroute_setsockopt(struct sock *, int, char __user *, unsigned int);
extern int ip_mroute_getsockopt(struct sock *, int, char __user *, int __user *);
extern int ipmr_ioctl(struct sock *sk, int cmd, void __user *arg);
extern int ipmr_compat_ioctl(struct sock *sk, unsigned int cmd, void __user *arg);
extern int ip_mr_init(void);
#else
static inline
int ip_mroute_setsockopt(struct sock *sock,
			 int optname, char __user *optval, unsigned int optlen)
=======
	return opt >= MRT_BASE && opt <= MRT_MAX;
}

int ip_mroute_setsockopt(struct sock *, int, char __user *, unsigned int);
int ip_mroute_getsockopt(struct sock *, int, char __user *, int __user *);
int ipmr_ioctl(struct sock *sk, int cmd, void __user *arg);
int ipmr_compat_ioctl(struct sock *sk, unsigned int cmd, void __user *arg);
int ip_mr_init(void);
#else
static inline int ip_mroute_setsockopt(struct sock *sock, int optname,
				       char __user *optval, unsigned int optlen)
>>>>>>> v4.9.227
{
	return -ENOPROTOOPT;
}

<<<<<<< HEAD
static inline
int ip_mroute_getsockopt(struct sock *sock,
			 int optname, char __user *optval, int __user *optlen)
=======
static inline int ip_mroute_getsockopt(struct sock *sock, int optname,
				       char __user *optval, int __user *optlen)
>>>>>>> v4.9.227
{
	return -ENOPROTOOPT;
}

<<<<<<< HEAD
static inline
int ipmr_ioctl(struct sock *sk, int cmd, void __user *arg)
=======
static inline int ipmr_ioctl(struct sock *sk, int cmd, void __user *arg)
>>>>>>> v4.9.227
{
	return -ENOIOCTLCMD;
}

static inline int ip_mr_init(void)
{
	return 0;
}
<<<<<<< HEAD
=======

static inline int ip_mroute_opt(int opt)
{
	return 0;
}
>>>>>>> v4.9.227
#endif

struct vif_device {
	struct net_device 	*dev;			/* Device we are using */
	unsigned long	bytes_in,bytes_out;
	unsigned long	pkt_in,pkt_out;		/* Statistics 			*/
	unsigned long	rate_limit;		/* Traffic shaping (NI) 	*/
	unsigned char	threshold;		/* TTL threshold 		*/
	unsigned short	flags;			/* Control flags 		*/
	__be32		local,remote;		/* Addresses(remote for tunnels)*/
	int		link;			/* Physical interface index	*/
};

#define VIFF_STATIC 0x8000

<<<<<<< HEAD
=======
#define VIF_EXISTS(_mrt, _idx) ((_mrt)->vif_table[_idx].dev != NULL)
#define MFC_LINES 64

struct mr_table {
	struct list_head	list;
	possible_net_t		net;
	u32			id;
	struct sock __rcu	*mroute_sk;
	struct timer_list	ipmr_expire_timer;
	struct list_head	mfc_unres_queue;
	struct list_head	mfc_cache_array[MFC_LINES];
	struct vif_device	vif_table[MAXVIFS];
	int			maxvif;
	atomic_t		cache_resolve_queue_len;
	bool			mroute_do_assert;
	bool			mroute_do_pim;
	int			mroute_reg_vif_num;
};

/* mfc_flags:
 * MFC_STATIC - the entry was added statically (not by a routing daemon)
 */
enum {
	MFC_STATIC = BIT(0),
};

>>>>>>> v4.9.227
struct mfc_cache {
	struct list_head list;
	__be32 mfc_mcastgrp;			/* Group the entry belongs to 	*/
	__be32 mfc_origin;			/* Source of packet 		*/
	vifi_t mfc_parent;			/* Source interface		*/
	int mfc_flags;				/* Flags on line		*/

	union {
		struct {
			unsigned long expires;
			struct sk_buff_head unresolved;	/* Unresolved buffers		*/
		} unres;
		struct {
			unsigned long last_assert;
			int minvif;
			int maxvif;
			unsigned long bytes;
			unsigned long pkt;
			unsigned long wrong_if;
<<<<<<< HEAD
=======
			unsigned long lastuse;
>>>>>>> v4.9.227
			unsigned char ttls[MAXVIFS];	/* TTL thresholds		*/
		} res;
	} mfc_un;
	struct rcu_head	rcu;
};

<<<<<<< HEAD
#define MFC_STATIC		1
#define MFC_NOTIFY		2

#define MFC_LINES		64

=======
>>>>>>> v4.9.227
#ifdef __BIG_ENDIAN
#define MFC_HASH(a,b)	(((((__force u32)(__be32)a)>>24)^(((__force u32)(__be32)b)>>26))&(MFC_LINES-1))
#else
#define MFC_HASH(a,b)	((((__force u32)(__be32)a)^(((__force u32)(__be32)b)>>2))&(MFC_LINES-1))
<<<<<<< HEAD
#endif		

struct rtmsg;
extern int ipmr_get_route(struct net *net, struct sk_buff *skb,
			  __be32 saddr, __be32 daddr,
			  struct rtmsg *rtm, int nowait, u32 portid);
=======
#endif

struct rtmsg;
int ipmr_get_route(struct net *net, struct sk_buff *skb,
		   __be32 saddr, __be32 daddr,
		   struct rtmsg *rtm, int nowait, u32 portid);
>>>>>>> v4.9.227
#endif
