/* AF_RXRPC internal definitions
 *
 * Copyright (C) 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

<<<<<<< HEAD
=======
#include <linux/atomic.h>
#include <linux/seqlock.h>
#include <net/sock.h>
#include <net/af_rxrpc.h>
>>>>>>> v4.9.227
#include <rxrpc/packet.h>

#if 0
#define CHECK_SLAB_OKAY(X)				     \
	BUG_ON(atomic_read((X)) >> (sizeof(atomic_t) - 2) == \
	       (POISON_FREE << 8 | POISON_FREE))
#else
<<<<<<< HEAD
#define CHECK_SLAB_OKAY(X) do {} while(0)
=======
#define CHECK_SLAB_OKAY(X) do {} while (0)
>>>>>>> v4.9.227
#endif

#define FCRYPT_BSIZE 8
struct rxrpc_crypt {
	union {
		u8	x[FCRYPT_BSIZE];
		__be32	n[2];
	};
} __attribute__((aligned(8)));

#define rxrpc_queue_work(WS)	queue_work(rxrpc_workqueue, (WS))
#define rxrpc_queue_delayed_work(WS,D)	\
	queue_delayed_work(rxrpc_workqueue, (WS), (D))

<<<<<<< HEAD
#define rxrpc_queue_call(CALL)	rxrpc_queue_work(&(CALL)->processor)
#define rxrpc_queue_conn(CONN)	rxrpc_queue_work(&(CONN)->processor)
=======
struct rxrpc_connection;

/*
 * Mark applied to socket buffers.
 */
enum rxrpc_skb_mark {
	RXRPC_SKB_MARK_DATA,		/* data message */
	RXRPC_SKB_MARK_FINAL_ACK,	/* final ACK received message */
	RXRPC_SKB_MARK_BUSY,		/* server busy message */
	RXRPC_SKB_MARK_REMOTE_ABORT,	/* remote abort message */
	RXRPC_SKB_MARK_LOCAL_ABORT,	/* local abort message */
	RXRPC_SKB_MARK_NET_ERROR,	/* network error message */
	RXRPC_SKB_MARK_LOCAL_ERROR,	/* local error message */
	RXRPC_SKB_MARK_NEW_CALL,	/* local error message */
};
>>>>>>> v4.9.227

/*
 * sk_state for RxRPC sockets
 */
enum {
<<<<<<< HEAD
	RXRPC_UNCONNECTED = 0,
	RXRPC_CLIENT_BOUND,		/* client local address bound */
	RXRPC_CLIENT_CONNECTED,		/* client is connected */
=======
	RXRPC_UNBOUND = 0,
	RXRPC_CLIENT_UNBOUND,		/* Unbound socket used as client */
	RXRPC_CLIENT_BOUND,		/* client local address bound */
>>>>>>> v4.9.227
	RXRPC_SERVER_BOUND,		/* server local address bound */
	RXRPC_SERVER_LISTENING,		/* server listening for connections */
	RXRPC_CLOSE,			/* socket is being closed */
};

/*
<<<<<<< HEAD
=======
 * Service backlog preallocation.
 *
 * This contains circular buffers of preallocated peers, connections and calls
 * for incoming service calls and their head and tail pointers.  This allows
 * calls to be set up in the data_ready handler, thereby avoiding the need to
 * shuffle packets around so much.
 */
struct rxrpc_backlog {
	unsigned short		peer_backlog_head;
	unsigned short		peer_backlog_tail;
	unsigned short		conn_backlog_head;
	unsigned short		conn_backlog_tail;
	unsigned short		call_backlog_head;
	unsigned short		call_backlog_tail;
#define RXRPC_BACKLOG_MAX	32
	struct rxrpc_peer	*peer_backlog[RXRPC_BACKLOG_MAX];
	struct rxrpc_connection	*conn_backlog[RXRPC_BACKLOG_MAX];
	struct rxrpc_call	*call_backlog[RXRPC_BACKLOG_MAX];
};

/*
>>>>>>> v4.9.227
 * RxRPC socket definition
 */
struct rxrpc_sock {
	/* WARNING: sk has to be the first member */
	struct sock		sk;
<<<<<<< HEAD
	rxrpc_interceptor_t	interceptor;	/* kernel service Rx interceptor function */
	struct rxrpc_local	*local;		/* local endpoint */
	struct rxrpc_transport	*trans;		/* transport handler */
	struct rxrpc_conn_bundle *bundle;	/* virtual connection bundle */
	struct rxrpc_connection	*conn;		/* exclusive virtual connection */
	struct list_head	listen_link;	/* link in the local endpoint's listen list */
	struct list_head	secureq;	/* calls awaiting connection security clearance */
	struct list_head	acceptq;	/* calls awaiting acceptance */
	struct key		*key;		/* security for this socket */
	struct key		*securities;	/* list of server security descriptors */
	struct rb_root		calls;		/* outstanding calls on this socket */
	unsigned long		flags;
#define RXRPC_SOCK_EXCLUSIVE_CONN	1	/* exclusive connection for a client socket */
	rwlock_t		call_lock;	/* lock for calls */
	u32			min_sec_level;	/* minimum security level */
#define RXRPC_SECURITY_MAX	RXRPC_SECURITY_ENCRYPT
	struct sockaddr_rxrpc	srx;		/* local address */
	sa_family_t		proto;		/* protocol created with */
	__be16			service_id;	/* service ID of local/remote service */
=======
	rxrpc_notify_new_call_t	notify_new_call; /* Func to notify of new call */
	rxrpc_discard_new_call_t discard_new_call; /* Func to discard a new call */
	struct rxrpc_local	*local;		/* local endpoint */
	struct rxrpc_backlog	*backlog;	/* Preallocation for services */
	spinlock_t		incoming_lock;	/* Incoming call vs service shutdown lock */
	struct list_head	sock_calls;	/* List of calls owned by this socket */
	struct list_head	to_be_accepted;	/* calls awaiting acceptance */
	struct list_head	recvmsg_q;	/* Calls awaiting recvmsg's attention  */
	rwlock_t		recvmsg_lock;	/* Lock for recvmsg_q */
	struct key		*key;		/* security for this socket */
	struct key		*securities;	/* list of server security descriptors */
	struct rb_root		calls;		/* User ID -> call mapping */
	unsigned long		flags;
#define RXRPC_SOCK_CONNECTED		0	/* connect_srx is set */
	rwlock_t		call_lock;	/* lock for calls */
	u32			min_sec_level;	/* minimum security level */
#define RXRPC_SECURITY_MAX	RXRPC_SECURITY_ENCRYPT
	bool			exclusive;	/* Exclusive connection for a client socket */
	sa_family_t		family;		/* Protocol family created with */
	struct sockaddr_rxrpc	srx;		/* local address */
	struct sockaddr_rxrpc	connect_srx;	/* Default client address from connect() */
>>>>>>> v4.9.227
};

#define rxrpc_sk(__sk) container_of((__sk), struct rxrpc_sock, sk)

/*
<<<<<<< HEAD
=======
 * CPU-byteorder normalised Rx packet header.
 */
struct rxrpc_host_header {
	u32		epoch;		/* client boot timestamp */
	u32		cid;		/* connection and channel ID */
	u32		callNumber;	/* call ID (0 for connection-level packets) */
	u32		seq;		/* sequence number of pkt in call stream */
	u32		serial;		/* serial number of pkt sent to network */
	u8		type;		/* packet type */
	u8		flags;		/* packet flags */
	u8		userStatus;	/* app-layer defined status */
	u8		securityIndex;	/* security protocol ID */
	union {
		u16	_rsvd;		/* reserved */
		u16	cksum;		/* kerberos security checksum */
	};
	u16		serviceId;	/* service ID */
} __packed;

/*
>>>>>>> v4.9.227
 * RxRPC socket buffer private variables
 * - max 48 bytes (struct sk_buff::cb)
 */
struct rxrpc_skb_priv {
<<<<<<< HEAD
	struct rxrpc_call	*call;		/* call with which associated */
	unsigned long		resend_at;	/* time in jiffies at which to resend */
	union {
		unsigned int	offset;		/* offset into buffer of next read */
		int		remain;		/* amount of space remaining for next write */
		u32		error;		/* network error code */
		bool		need_resend;	/* T if needs resending */
	};

	struct rxrpc_header	hdr;		/* RxRPC packet header from this packet */
=======
	union {
		u8		nr_jumbo;	/* Number of jumbo subpackets */
	};
	union {
		int		remain;		/* amount of space remaining for next write */
	};

	struct rxrpc_host_header hdr;		/* RxRPC packet header from this packet */
>>>>>>> v4.9.227
};

#define rxrpc_skb(__skb) ((struct rxrpc_skb_priv *) &(__skb)->cb)

<<<<<<< HEAD
enum rxrpc_command {
	RXRPC_CMD_SEND_DATA,		/* send data message */
	RXRPC_CMD_SEND_ABORT,		/* request abort generation */
	RXRPC_CMD_ACCEPT,		/* [server] accept incoming call */
	RXRPC_CMD_REJECT_BUSY,		/* [server] reject a call as busy */
};

=======
>>>>>>> v4.9.227
/*
 * RxRPC security module interface
 */
struct rxrpc_security {
<<<<<<< HEAD
	struct module		*owner;		/* providing module */
	struct list_head	link;		/* link in master list */
	const char		*name;		/* name of this service */
	u8			security_index;	/* security type provided */

=======
	const char		*name;		/* name of this service */
	u8			security_index;	/* security type provided */

	/* Initialise a security service */
	int (*init)(void);

	/* Clean up a security service */
	void (*exit)(void);

>>>>>>> v4.9.227
	/* initialise a connection's security */
	int (*init_connection_security)(struct rxrpc_connection *);

	/* prime a connection's packet security */
<<<<<<< HEAD
	void (*prime_packet_security)(struct rxrpc_connection *);

	/* impose security on a packet */
	int (*secure_packet)(const struct rxrpc_call *,
=======
	int (*prime_packet_security)(struct rxrpc_connection *);

	/* impose security on a packet */
	int (*secure_packet)(struct rxrpc_call *,
>>>>>>> v4.9.227
			     struct sk_buff *,
			     size_t,
			     void *);

	/* verify the security on a received packet */
<<<<<<< HEAD
	int (*verify_packet)(const struct rxrpc_call *, struct sk_buff *,
			     u32 *);
=======
	int (*verify_packet)(struct rxrpc_call *, struct sk_buff *,
			     unsigned int, unsigned int, rxrpc_seq_t, u16);

	/* Locate the data in a received packet that has been verified. */
	void (*locate_data)(struct rxrpc_call *, struct sk_buff *,
			    unsigned int *, unsigned int *);
>>>>>>> v4.9.227

	/* issue a challenge */
	int (*issue_challenge)(struct rxrpc_connection *);

	/* respond to a challenge */
	int (*respond_to_challenge)(struct rxrpc_connection *,
				    struct sk_buff *,
				    u32 *);

	/* verify a response */
	int (*verify_response)(struct rxrpc_connection *,
			       struct sk_buff *,
			       u32 *);

	/* clear connection security */
	void (*clear)(struct rxrpc_connection *);
};

/*
<<<<<<< HEAD
 * RxRPC local transport endpoint definition
 * - matched by local port, address and protocol type
 */
struct rxrpc_local {
	struct socket		*socket;	/* my UDP socket */
	struct work_struct	destroyer;	/* endpoint destroyer */
	struct work_struct	acceptor;	/* incoming call processor */
	struct work_struct	rejecter;	/* packet reject writer */
	struct list_head	services;	/* services listening on this endpoint */
	struct list_head	link;		/* link in endpoint list */
	struct rw_semaphore	defrag_sem;	/* control re-enablement of IP DF bit */
	struct sk_buff_head	accept_queue;	/* incoming calls awaiting acceptance */
	struct sk_buff_head	reject_queue;	/* packets awaiting rejection */
	spinlock_t		lock;		/* access lock */
	rwlock_t		services_lock;	/* lock for services list */
	atomic_t		usage;
	int			debug_id;	/* debug ID for printks */
	volatile char		error_rcvd;	/* T if received ICMP error outstanding */
=======
 * RxRPC local transport endpoint description
 * - owned by a single AF_RXRPC socket
 * - pointed to by transport socket struct sk_user_data
 */
struct rxrpc_local {
	struct rcu_head		rcu;
	atomic_t		usage;
	struct list_head	link;
	struct socket		*socket;	/* my UDP socket */
	struct work_struct	processor;
	struct rxrpc_sock __rcu	*service;	/* Service(s) listening on this endpoint */
	struct rw_semaphore	defrag_sem;	/* control re-enablement of IP DF bit */
	struct sk_buff_head	reject_queue;	/* packets awaiting rejection */
	struct sk_buff_head	event_queue;	/* endpoint event packets awaiting processing */
	struct rb_root		client_conns;	/* Client connections by socket params */
	spinlock_t		client_conns_lock; /* Lock for client_conns */
	spinlock_t		lock;		/* access lock */
	rwlock_t		services_lock;	/* lock for services list */
	int			debug_id;	/* debug ID for printks */
	bool			dead;
>>>>>>> v4.9.227
	struct sockaddr_rxrpc	srx;		/* local address */
};

/*
 * RxRPC remote transport endpoint definition
<<<<<<< HEAD
 * - matched by remote port, address and protocol type
 * - holds the connection ID counter for connections between the two endpoints
 */
struct rxrpc_peer {
	struct work_struct	destroyer;	/* peer destroyer */
	struct list_head	link;		/* link in master peer list */
	struct list_head	error_targets;	/* targets for net error distribution */
	spinlock_t		lock;		/* access lock */
	atomic_t		usage;
=======
 * - matched by local endpoint, remote port, address and protocol type
 */
struct rxrpc_peer {
	struct rcu_head		rcu;		/* This must be first */
	atomic_t		usage;
	unsigned long		hash_key;
	struct hlist_node	hash_link;
	struct rxrpc_local	*local;
	struct hlist_head	error_targets;	/* targets for net error distribution */
	struct work_struct	error_distributor;
	struct rb_root		service_conns;	/* Service connections */
	seqlock_t		service_conn_lock;
	spinlock_t		lock;		/* access lock */
>>>>>>> v4.9.227
	unsigned int		if_mtu;		/* interface MTU for this peer */
	unsigned int		mtu;		/* network MTU for this peer */
	unsigned int		maxdata;	/* data size (MTU - hdrsize) */
	unsigned short		hdrsize;	/* header size (IP + UDP + RxRPC) */
	int			debug_id;	/* debug ID for printks */
<<<<<<< HEAD
	int			net_error;	/* network error distributed */
=======
	int			error_report;	/* Net (+0) or local (+1000000) to distribute */
#define RXRPC_LOCAL_ERROR_OFFSET 1000000
>>>>>>> v4.9.227
	struct sockaddr_rxrpc	srx;		/* remote address */

	/* calculated RTT cache */
#define RXRPC_RTT_CACHE_SIZE 32
<<<<<<< HEAD
	suseconds_t		rtt;		/* current RTT estimate (in uS) */
	unsigned int		rtt_point;	/* next entry at which to insert */
	unsigned int		rtt_usage;	/* amount of cache actually used */
	suseconds_t		rtt_cache[RXRPC_RTT_CACHE_SIZE]; /* calculated RTT cache */
};

/*
 * RxRPC point-to-point transport / connection manager definition
 * - handles a bundle of connections between two endpoints
 * - matched by { local, peer }
 */
struct rxrpc_transport {
	struct rxrpc_local	*local;		/* local transport endpoint */
	struct rxrpc_peer	*peer;		/* remote transport endpoint */
	struct work_struct	error_handler;	/* network error distributor */
	struct rb_root		bundles;	/* client connection bundles on this transport */
	struct rb_root		client_conns;	/* client connections on this transport */
	struct rb_root		server_conns;	/* server connections on this transport */
	struct list_head	link;		/* link in master session list */
	struct sk_buff_head	error_queue;	/* error packets awaiting processing */
	time_t			put_time;	/* time at which to reap */
	spinlock_t		client_lock;	/* client connection allocation lock */
	rwlock_t		conn_lock;	/* lock for active/dead connections */
	atomic_t		usage;
	int			debug_id;	/* debug ID for printks */
	unsigned int		conn_idcounter;	/* connection ID counter (client) */
};

/*
 * RxRPC client connection bundle
 * - matched by { transport, service_id, key }
 */
struct rxrpc_conn_bundle {
	struct rb_node		node;		/* node in transport's lookup tree */
	struct list_head	unused_conns;	/* unused connections in this bundle */
	struct list_head	avail_conns;	/* available connections in this bundle */
	struct list_head	busy_conns;	/* busy connections in this bundle */
	struct key		*key;		/* security for this bundle */
	wait_queue_head_t	chanwait;	/* wait for channel to become available */
	atomic_t		usage;
	int			debug_id;	/* debug ID for printks */
	unsigned short		num_conns;	/* number of connections in this bundle */
	__be16			service_id;	/* service ID */
	u8			security_ix;	/* security type */
=======
	ktime_t			rtt_last_req;	/* Time of last RTT request */
	u64			rtt;		/* Current RTT estimate (in nS) */
	u64			rtt_sum;	/* Sum of cache contents */
	u64			rtt_cache[RXRPC_RTT_CACHE_SIZE]; /* Determined RTT cache */
	u8			rtt_cursor;	/* next entry at which to insert */
	u8			rtt_usage;	/* amount of cache actually used */
};

/*
 * Keys for matching a connection.
 */
struct rxrpc_conn_proto {
	union {
		struct {
			u32	epoch;		/* epoch of this connection */
			u32	cid;		/* connection ID */
		};
		u64		index_key;
	};
};

struct rxrpc_conn_parameters {
	struct rxrpc_local	*local;		/* Representation of local endpoint */
	struct rxrpc_peer	*peer;		/* Remote endpoint */
	struct key		*key;		/* Security details */
	bool			exclusive;	/* T if conn is exclusive */
	u16			service_id;	/* Service ID for this connection */
	u32			security_level;	/* Security level selected */
};

/*
 * Bits in the connection flags.
 */
enum rxrpc_conn_flag {
	RXRPC_CONN_HAS_IDR,		/* Has a client conn ID assigned */
	RXRPC_CONN_IN_SERVICE_CONNS,	/* Conn is in peer->service_conns */
	RXRPC_CONN_IN_CLIENT_CONNS,	/* Conn is in local->client_conns */
	RXRPC_CONN_EXPOSED,		/* Conn has extra ref for exposure */
	RXRPC_CONN_DONT_REUSE,		/* Don't reuse this connection */
	RXRPC_CONN_COUNTED,		/* Counted by rxrpc_nr_client_conns */
};

/*
 * Events that can be raised upon a connection.
 */
enum rxrpc_conn_event {
	RXRPC_CONN_EV_CHALLENGE,	/* Send challenge packet */
};

/*
 * The connection cache state.
 */
enum rxrpc_conn_cache_state {
	RXRPC_CONN_CLIENT_INACTIVE,	/* Conn is not yet listed */
	RXRPC_CONN_CLIENT_WAITING,	/* Conn is on wait list, waiting for capacity */
	RXRPC_CONN_CLIENT_ACTIVE,	/* Conn is on active list, doing calls */
	RXRPC_CONN_CLIENT_CULLED,	/* Conn is culled and delisted, doing calls */
	RXRPC_CONN_CLIENT_IDLE,		/* Conn is on idle list, doing mostly nothing */
	RXRPC_CONN__NR_CACHE_STATES
};

/*
 * The connection protocol state.
 */
enum rxrpc_conn_proto_state {
	RXRPC_CONN_UNUSED,		/* Connection not yet attempted */
	RXRPC_CONN_CLIENT,		/* Client connection */
	RXRPC_CONN_SERVICE_PREALLOC,	/* Service connection preallocation */
	RXRPC_CONN_SERVICE_UNSECURED,	/* Service unsecured connection */
	RXRPC_CONN_SERVICE_CHALLENGING,	/* Service challenging for security */
	RXRPC_CONN_SERVICE,		/* Service secured connection */
	RXRPC_CONN_REMOTELY_ABORTED,	/* Conn aborted by peer */
	RXRPC_CONN_LOCALLY_ABORTED,	/* Conn aborted locally */
	RXRPC_CONN__NR_STATES
>>>>>>> v4.9.227
};

/*
 * RxRPC connection definition
<<<<<<< HEAD
 * - matched by { transport, service_id, conn_id, direction, key }
 * - each connection can only handle four simultaneous calls
 */
struct rxrpc_connection {
	struct rxrpc_transport	*trans;		/* transport session */
	struct rxrpc_conn_bundle *bundle;	/* connection bundle (client) */
	struct work_struct	processor;	/* connection event processor */
	struct rb_node		node;		/* node in transport's lookup tree */
	struct list_head	link;		/* link in master connection list */
	struct list_head	bundle_link;	/* link in bundle */
	struct rb_root		calls;		/* calls on this connection */
	struct sk_buff_head	rx_queue;	/* received conn-level packets */
	struct rxrpc_call	*channels[RXRPC_MAXCALLS]; /* channels (active calls) */
	struct rxrpc_security	*security;	/* applied security module */
	struct key		*key;		/* security for this connection (client) */
	struct key		*server_key;	/* security for this service */
	struct crypto_blkcipher	*cipher;	/* encryption handle */
	struct rxrpc_crypt	csum_iv;	/* packet checksum base */
	unsigned long		events;
#define RXRPC_CONN_CHALLENGE	0		/* send challenge packet */
	time_t			put_time;	/* time at which to reap */
	rwlock_t		lock;		/* access lock */
	spinlock_t		state_lock;	/* state-change lock */
	atomic_t		usage;
	u32			real_conn_id;	/* connection ID (host-endian) */
	enum {					/* current state of connection */
		RXRPC_CONN_UNUSED,		/* - connection not yet attempted */
		RXRPC_CONN_CLIENT,		/* - client connection */
		RXRPC_CONN_SERVER_UNSECURED,	/* - server unsecured connection */
		RXRPC_CONN_SERVER_CHALLENGING,	/* - server challenging for security */
		RXRPC_CONN_SERVER,		/* - server secured connection */
		RXRPC_CONN_REMOTELY_ABORTED,	/* - conn aborted by peer */
		RXRPC_CONN_LOCALLY_ABORTED,	/* - conn aborted locally */
		RXRPC_CONN_NETWORK_ERROR,	/* - conn terminated by network error */
	} state;
	int			error;		/* error code for local abort */
	int			debug_id;	/* debug ID for printks */
	unsigned int		call_counter;	/* call ID counter */
	atomic_t		serial;		/* packet serial number counter */
	atomic_t		hi_serial;	/* highest serial number received */
	u8			avail_calls;	/* number of calls available */
	u8			size_align;	/* data size alignment (for security) */
	u8			header_size;	/* rxrpc + security header size */
	u8			security_size;	/* security header size */
	u32			security_level;	/* security level negotiated */
	u32			security_nonce;	/* response re-use preventer */

	/* the following are all in net order */
	__be32			epoch;		/* epoch of this connection */
	__be32			cid;		/* connection ID */
	__be16			service_id;	/* service ID */
	u8			security_ix;	/* security type */
	u8			in_clientflag;	/* RXRPC_CLIENT_INITIATED if we are server */
=======
 * - matched by { local, peer, epoch, conn_id, direction }
 * - each connection can only handle four simultaneous calls
 */
struct rxrpc_connection {
	struct rxrpc_conn_proto	proto;
	struct rxrpc_conn_parameters params;

	atomic_t		usage;
	struct rcu_head		rcu;
	struct list_head	cache_link;

	spinlock_t		channel_lock;
	unsigned char		active_chans;	/* Mask of active channels */
#define RXRPC_ACTIVE_CHANS_MASK	((1 << RXRPC_MAXCALLS) - 1)
	struct list_head	waiting_calls;	/* Calls waiting for channels */
	struct rxrpc_channel {
		struct rxrpc_call __rcu	*call;		/* Active call */
		u32			call_id;	/* ID of current call */
		u32			call_counter;	/* Call ID counter */
		u32			last_call;	/* ID of last call */
		u8			last_type;	/* Type of last packet */
		u16			last_service_id;
		union {
			u32		last_seq;
			u32		last_abort;
		};
	} channels[RXRPC_MAXCALLS];

	struct work_struct	processor;	/* connection event processor */
	union {
		struct rb_node	client_node;	/* Node in local->client_conns */
		struct rb_node	service_node;	/* Node in peer->service_conns */
	};
	struct list_head	proc_link;	/* link in procfs list */
	struct list_head	link;		/* link in master connection list */
	struct sk_buff_head	rx_queue;	/* received conn-level packets */
	const struct rxrpc_security *security;	/* applied security module */
	struct key		*server_key;	/* security for this service */
	struct crypto_skcipher	*cipher;	/* encryption handle */
	struct rxrpc_crypt	csum_iv;	/* packet checksum base */
	unsigned long		flags;
	unsigned long		events;
	unsigned long		idle_timestamp;	/* Time at which last became idle */
	spinlock_t		state_lock;	/* state-change lock */
	enum rxrpc_conn_cache_state cache_state;
	enum rxrpc_conn_proto_state state;	/* current state of connection */
	u32			local_abort;	/* local abort code */
	u32			remote_abort;	/* remote abort code */
	int			debug_id;	/* debug ID for printks */
	atomic_t		serial;		/* packet serial number counter */
	unsigned int		hi_serial;	/* highest serial number received */
	u32			security_nonce;	/* response re-use preventer */
	u8			size_align;	/* data size alignment (for security) */
	u8			security_size;	/* security header size */
	u8			security_ix;	/* security type */
>>>>>>> v4.9.227
	u8			out_clientflag;	/* RXRPC_CLIENT_INITIATED if we are client */
};

/*
<<<<<<< HEAD
=======
 * Flags in call->flags.
 */
enum rxrpc_call_flag {
	RXRPC_CALL_RELEASED,		/* call has been released - no more message to userspace */
	RXRPC_CALL_HAS_USERID,		/* has a user ID attached */
	RXRPC_CALL_IS_SERVICE,		/* Call is service call */
	RXRPC_CALL_EXPOSED,		/* The call was exposed to the world */
	RXRPC_CALL_RX_LAST,		/* Received the last packet (at rxtx_top) */
	RXRPC_CALL_TX_LAST,		/* Last packet in Tx buffer (at rxtx_top) */
	RXRPC_CALL_SEND_PING,		/* A ping will need to be sent */
	RXRPC_CALL_PINGING,		/* Ping in process */
	RXRPC_CALL_RETRANS_TIMEOUT,	/* Retransmission due to timeout occurred */
	RXRPC_CALL_DISCONNECTED,	/* The call has been disconnected */
};

/*
 * Events that can be raised on a call.
 */
enum rxrpc_call_event {
	RXRPC_CALL_EV_ACK,		/* need to generate ACK */
	RXRPC_CALL_EV_ABORT,		/* need to generate abort */
	RXRPC_CALL_EV_TIMER,		/* Timer expired */
	RXRPC_CALL_EV_RESEND,		/* Tx resend required */
	RXRPC_CALL_EV_PING,		/* Ping send required */
};

/*
 * The states that a call can be in.
 */
enum rxrpc_call_state {
	RXRPC_CALL_UNINITIALISED,
	RXRPC_CALL_CLIENT_AWAIT_CONN,	/* - client waiting for connection to become available */
	RXRPC_CALL_CLIENT_SEND_REQUEST,	/* - client sending request phase */
	RXRPC_CALL_CLIENT_AWAIT_REPLY,	/* - client awaiting reply */
	RXRPC_CALL_CLIENT_RECV_REPLY,	/* - client receiving reply phase */
	RXRPC_CALL_SERVER_PREALLOC,	/* - service preallocation */
	RXRPC_CALL_SERVER_SECURING,	/* - server securing request connection */
	RXRPC_CALL_SERVER_ACCEPTING,	/* - server accepting request */
	RXRPC_CALL_SERVER_RECV_REQUEST,	/* - server receiving request */
	RXRPC_CALL_SERVER_ACK_REQUEST,	/* - server pending ACK of request */
	RXRPC_CALL_SERVER_SEND_REPLY,	/* - server sending reply */
	RXRPC_CALL_SERVER_AWAIT_ACK,	/* - server awaiting final ACK */
	RXRPC_CALL_COMPLETE,		/* - call complete */
	NR__RXRPC_CALL_STATES
};

/*
 * Call completion condition (state == RXRPC_CALL_COMPLETE).
 */
enum rxrpc_call_completion {
	RXRPC_CALL_SUCCEEDED,		/* - Normal termination */
	RXRPC_CALL_REMOTELY_ABORTED,	/* - call aborted by peer */
	RXRPC_CALL_LOCALLY_ABORTED,	/* - call aborted locally on error or close */
	RXRPC_CALL_LOCAL_ERROR,		/* - call failed due to local error */
	RXRPC_CALL_NETWORK_ERROR,	/* - call terminated by network error */
	NR__RXRPC_CALL_COMPLETIONS
};

/*
 * Call Tx congestion management modes.
 */
enum rxrpc_congest_mode {
	RXRPC_CALL_SLOW_START,
	RXRPC_CALL_CONGEST_AVOIDANCE,
	RXRPC_CALL_PACKET_LOSS,
	RXRPC_CALL_FAST_RETRANSMIT,
	NR__RXRPC_CONGEST_MODES
};

/*
>>>>>>> v4.9.227
 * RxRPC call definition
 * - matched by { connection, call_id }
 */
struct rxrpc_call {
<<<<<<< HEAD
	struct rxrpc_connection	*conn;		/* connection carrying call */
	struct rxrpc_sock	*socket;	/* socket responsible */
	struct timer_list	lifetimer;	/* lifetime remaining on call */
	struct timer_list	deadspan;	/* reap timer for re-ACK'ing, etc  */
	struct timer_list	ack_timer;	/* ACK generation timer */
	struct timer_list	resend_timer;	/* Tx resend timer */
	struct work_struct	destroyer;	/* call destroyer */
	struct work_struct	processor;	/* packet processor and ACK generator */
	struct list_head	link;		/* link in master call list */
	struct list_head	error_link;	/* link in error distribution list */
	struct list_head	accept_link;	/* calls awaiting acceptance */
	struct rb_node		sock_node;	/* node in socket call tree */
	struct rb_node		conn_node;	/* node in connection call tree */
	struct sk_buff_head	rx_queue;	/* received packets */
	struct sk_buff_head	rx_oos_queue;	/* packets received out of sequence */
	struct sk_buff		*tx_pending;	/* Tx socket buffer being filled */
	wait_queue_head_t	tx_waitq;	/* wait for Tx window space to become available */
	unsigned long		user_call_ID;	/* user-defined call ID */
	unsigned long		creation_jif;	/* time of call creation */
	unsigned long		flags;
#define RXRPC_CALL_RELEASED	0	/* call has been released - no more message to userspace */
#define RXRPC_CALL_TERMINAL_MSG	1	/* call has given the socket its final message */
#define RXRPC_CALL_RCVD_LAST	2	/* all packets received */
#define RXRPC_CALL_RUN_RTIMER	3	/* Tx resend timer started */
#define RXRPC_CALL_TX_SOFT_ACK	4	/* sent some soft ACKs */
#define RXRPC_CALL_PROC_BUSY	5	/* the processor is busy */
#define RXRPC_CALL_INIT_ACCEPT	6	/* acceptance was initiated */
#define RXRPC_CALL_HAS_USERID	7	/* has a user ID attached */
#define RXRPC_CALL_EXPECT_OOS	8	/* expect out of sequence packets */
	unsigned long		events;
#define RXRPC_CALL_RCVD_ACKALL	0	/* ACKALL or reply received */
#define RXRPC_CALL_RCVD_BUSY	1	/* busy packet received */
#define RXRPC_CALL_RCVD_ABORT	2	/* abort packet received */
#define RXRPC_CALL_RCVD_ERROR	3	/* network error received */
#define RXRPC_CALL_ACK_FINAL	4	/* need to generate final ACK (and release call) */
#define RXRPC_CALL_ACK		5	/* need to generate ACK */
#define RXRPC_CALL_REJECT_BUSY	6	/* need to generate busy message */
#define RXRPC_CALL_ABORT	7	/* need to generate abort */
#define RXRPC_CALL_CONN_ABORT	8	/* local connection abort generated */
#define RXRPC_CALL_RESEND_TIMER	9	/* Tx resend timer expired */
#define RXRPC_CALL_RESEND	10	/* Tx resend required */
#define RXRPC_CALL_DRAIN_RX_OOS	11	/* drain the Rx out of sequence queue */
#define RXRPC_CALL_LIFE_TIMER	12	/* call's lifetimer ran out */
#define RXRPC_CALL_ACCEPTED	13	/* incoming call accepted by userspace app */
#define RXRPC_CALL_SECURED	14	/* incoming call's connection is now secure */
#define RXRPC_CALL_POST_ACCEPT	15	/* need to post an "accept?" message to the app */
#define RXRPC_CALL_RELEASE	16	/* need to release the call's resources */

	spinlock_t		lock;
	rwlock_t		state_lock;	/* lock for state transition */
	atomic_t		usage;
	atomic_t		sequence;	/* Tx data packet sequence counter */
	u32			abort_code;	/* local/remote abort code */
	enum {					/* current state of call */
		RXRPC_CALL_CLIENT_SEND_REQUEST,	/* - client sending request phase */
		RXRPC_CALL_CLIENT_AWAIT_REPLY,	/* - client awaiting reply */
		RXRPC_CALL_CLIENT_RECV_REPLY,	/* - client receiving reply phase */
		RXRPC_CALL_CLIENT_FINAL_ACK,	/* - client sending final ACK phase */
		RXRPC_CALL_SERVER_SECURING,	/* - server securing request connection */
		RXRPC_CALL_SERVER_ACCEPTING,	/* - server accepting request */
		RXRPC_CALL_SERVER_RECV_REQUEST,	/* - server receiving request */
		RXRPC_CALL_SERVER_ACK_REQUEST,	/* - server pending ACK of request */
		RXRPC_CALL_SERVER_SEND_REPLY,	/* - server sending reply */
		RXRPC_CALL_SERVER_AWAIT_ACK,	/* - server awaiting final ACK */
		RXRPC_CALL_COMPLETE,		/* - call completed */
		RXRPC_CALL_SERVER_BUSY,		/* - call rejected by busy server */
		RXRPC_CALL_REMOTELY_ABORTED,	/* - call aborted by peer */
		RXRPC_CALL_LOCALLY_ABORTED,	/* - call aborted locally on error or close */
		RXRPC_CALL_NETWORK_ERROR,	/* - call terminated by network error */
		RXRPC_CALL_DEAD,		/* - call is dead */
	} state;
	int			debug_id;	/* debug ID for printks */
	u8			channel;	/* connection channel occupied by this call */

	/* transmission-phase ACK management */
	u8			acks_head;	/* offset into window of first entry */
	u8			acks_tail;	/* offset into window of last entry */
	u8			acks_winsz;	/* size of un-ACK'd window */
	u8			acks_unacked;	/* lowest unacked packet in last ACK received */
	int			acks_latest;	/* serial number of latest ACK received */
	rxrpc_seq_t		acks_hard;	/* highest definitively ACK'd msg seq */
	unsigned long		*acks_window;	/* sent packet window
						 * - elements are pointers with LSB set if ACK'd
						 */

	/* receive-phase ACK management */
	rxrpc_seq_t		rx_data_expect;	/* next data seq ID expected to be received */
	rxrpc_seq_t		rx_data_post;	/* next data seq ID expected to be posted */
	rxrpc_seq_t		rx_data_recv;	/* last data seq ID encountered by recvmsg */
	rxrpc_seq_t		rx_data_eaten;	/* last data seq ID consumed by recvmsg */
	rxrpc_seq_t		rx_first_oos;	/* first packet in rx_oos_queue (or 0) */
	rxrpc_seq_t		ackr_win_top;	/* top of ACK window (rx_data_eaten is bottom) */
	rxrpc_seq_net_t		ackr_prev_seq;	/* previous sequence number received */
	u8			ackr_reason;	/* reason to ACK */
	__be32			ackr_serial;	/* serial of packet being ACK'd */
	atomic_t		ackr_not_idle;	/* number of packets in Rx queue */

	/* received packet records, 1 bit per record */
#define RXRPC_ACKR_WINDOW_ASZ DIV_ROUND_UP(RXRPC_MAXACKS, BITS_PER_LONG)
	unsigned long		ackr_window[RXRPC_ACKR_WINDOW_ASZ + 1];

	struct hlist_node	hash_node;
	unsigned long		hash_key;	/* Full hash key */
	u8			in_clientflag;	/* Copy of conn->in_clientflag for hashing */
	struct rxrpc_local	*local;		/* Local endpoint. Used for hashing. */
	sa_family_t		proto;		/* Frame protocol */
	/* the following should all be in net order */
	__be32			cid;		/* connection ID + channel index  */
	__be32			call_id;	/* call ID on connection  */
	__be32			epoch;		/* epoch of this connection */
	__be16			service_id;	/* service ID */
	union {					/* Peer IP address for hashing */
		__be32	ipv4_addr;
		__u8	ipv6_addr[16];		/* Anticipates eventual IPv6 support */
	} peer_ip;
};

/*
 * locally abort an RxRPC call
 */
static inline void rxrpc_abort_call(struct rxrpc_call *call, u32 abort_code)
{
	write_lock_bh(&call->state_lock);
	if (call->state < RXRPC_CALL_COMPLETE) {
		call->abort_code = abort_code;
		call->state = RXRPC_CALL_LOCALLY_ABORTED;
		set_bit(RXRPC_CALL_ABORT, &call->events);
	}
	write_unlock_bh(&call->state_lock);
}
=======
	struct rcu_head		rcu;
	struct rxrpc_connection	*conn;		/* connection carrying call */
	struct rxrpc_peer	*peer;		/* Peer record for remote address */
	struct rxrpc_sock __rcu	*socket;	/* socket responsible */
	ktime_t			ack_at;		/* When deferred ACK needs to happen */
	ktime_t			resend_at;	/* When next resend needs to happen */
	ktime_t			ping_at;	/* When next to send a ping */
	ktime_t			expire_at;	/* When the call times out */
	struct timer_list	timer;		/* Combined event timer */
	struct work_struct	processor;	/* Event processor */
	rxrpc_notify_rx_t	notify_rx;	/* kernel service Rx notification function */
	struct list_head	link;		/* link in master call list */
	struct list_head	chan_wait_link;	/* Link in conn->waiting_calls */
	struct hlist_node	error_link;	/* link in error distribution list */
	struct list_head	accept_link;	/* Link in rx->acceptq */
	struct list_head	recvmsg_link;	/* Link in rx->recvmsg_q */
	struct list_head	sock_link;	/* Link in rx->sock_calls */
	struct rb_node		sock_node;	/* Node in rx->calls */
	struct sk_buff		*tx_pending;	/* Tx socket buffer being filled */
	wait_queue_head_t	waitq;		/* Wait queue for channel or Tx */
	__be32			crypto_buf[2];	/* Temporary packet crypto buffer */
	unsigned long		user_call_ID;	/* user-defined call ID */
	unsigned long		flags;
	unsigned long		events;
	spinlock_t		lock;
	rwlock_t		state_lock;	/* lock for state transition */
	u32			abort_code;	/* Local/remote abort code */
	int			error;		/* Local error incurred */
	enum rxrpc_call_state	state;		/* current state of call */
	enum rxrpc_call_completion completion;	/* Call completion condition */
	atomic_t		usage;
	u16			service_id;	/* service ID */
	u8			security_ix;	/* Security type */
	u32			call_id;	/* call ID on connection  */
	u32			cid;		/* connection ID plus channel index */
	int			debug_id;	/* debug ID for printks */
	unsigned short		rx_pkt_offset;	/* Current recvmsg packet offset */
	unsigned short		rx_pkt_len;	/* Current recvmsg packet len */

	/* Rx/Tx circular buffer, depending on phase.
	 *
	 * In the Rx phase, packets are annotated with 0 or the number of the
	 * segment of a jumbo packet each buffer refers to.  There can be up to
	 * 47 segments in a maximum-size UDP packet.
	 *
	 * In the Tx phase, packets are annotated with which buffers have been
	 * acked.
	 */
#define RXRPC_RXTX_BUFF_SIZE	64
#define RXRPC_RXTX_BUFF_MASK	(RXRPC_RXTX_BUFF_SIZE - 1)
#define RXRPC_INIT_RX_WINDOW_SIZE 32
	struct sk_buff		**rxtx_buffer;
	u8			*rxtx_annotations;
#define RXRPC_TX_ANNO_ACK	0
#define RXRPC_TX_ANNO_UNACK	1
#define RXRPC_TX_ANNO_NAK	2
#define RXRPC_TX_ANNO_RETRANS	3
#define RXRPC_TX_ANNO_MASK	0x03
#define RXRPC_TX_ANNO_LAST	0x04
#define RXRPC_TX_ANNO_RESENT	0x08

#define RXRPC_RX_ANNO_JUMBO	0x3f		/* Jumbo subpacket number + 1 if not zero */
#define RXRPC_RX_ANNO_JLAST	0x40		/* Set if last element of a jumbo packet */
#define RXRPC_RX_ANNO_VERIFIED	0x80		/* Set if verified and decrypted */
	rxrpc_seq_t		tx_hard_ack;	/* Dead slot in buffer; the first transmitted but
						 * not hard-ACK'd packet follows this.
						 */
	rxrpc_seq_t		tx_top;		/* Highest Tx slot allocated. */

	/* TCP-style slow-start congestion control [RFC5681].  Since the SMSS
	 * is fixed, we keep these numbers in terms of segments (ie. DATA
	 * packets) rather than bytes.
	 */
#define RXRPC_TX_SMSS		RXRPC_JUMBO_DATALEN
	u8			cong_cwnd;	/* Congestion window size */
	u8			cong_extra;	/* Extra to send for congestion management */
	u8			cong_ssthresh;	/* Slow-start threshold */
	enum rxrpc_congest_mode	cong_mode:8;	/* Congestion management mode */
	u8			cong_dup_acks;	/* Count of ACKs showing missing packets */
	u8			cong_cumul_acks; /* Cumulative ACK count */
	ktime_t			cong_tstamp;	/* Last time cwnd was changed */

	rxrpc_seq_t		rx_hard_ack;	/* Dead slot in buffer; the first received but not
						 * consumed packet follows this.
						 */
	rxrpc_seq_t		rx_top;		/* Highest Rx slot allocated. */
	rxrpc_seq_t		rx_expect_next;	/* Expected next packet sequence number */
	u8			rx_winsize;	/* Size of Rx window */
	u8			tx_winsize;	/* Maximum size of Tx window */
	bool			tx_phase;	/* T if transmission phase, F if receive phase */
	u8			nr_jumbo_bad;	/* Number of jumbo dups/exceeds-windows */

	/* receive-phase ACK management */
	u8			ackr_reason;	/* reason to ACK */
	u16			ackr_skew;	/* skew on packet being ACK'd */
	rxrpc_serial_t		ackr_serial;	/* serial of packet being ACK'd */
	rxrpc_seq_t		ackr_prev_seq;	/* previous sequence number received */
	rxrpc_seq_t		ackr_consumed;	/* Highest packet shown consumed */
	rxrpc_seq_t		ackr_seen;	/* Highest packet shown seen */

	/* ping management */
	rxrpc_serial_t		ping_serial;	/* Last ping sent */
	ktime_t			ping_time;	/* Time last ping sent */

	/* transmission-phase ACK management */
	ktime_t			acks_latest_ts;	/* Timestamp of latest ACK received */
	rxrpc_serial_t		acks_latest;	/* serial number of latest ACK received */
	rxrpc_seq_t		acks_lowest_nak; /* Lowest NACK in the buffer (or ==tx_hard_ack) */
};

/*
 * Summary of a new ACK and the changes it made to the Tx buffer packet states.
 */
struct rxrpc_ack_summary {
	u8			ack_reason;
	u8			nr_acks;		/* Number of ACKs in packet */
	u8			nr_nacks;		/* Number of NACKs in packet */
	u8			nr_new_acks;		/* Number of new ACKs in packet */
	u8			nr_new_nacks;		/* Number of new NACKs in packet */
	u8			nr_rot_new_acks;	/* Number of rotated new ACKs */
	bool			new_low_nack;		/* T if new low NACK found */
	bool			retrans_timeo;		/* T if reTx due to timeout happened */
	u8			flight_size;		/* Number of unreceived transmissions */
	/* Place to stash values for tracing */
	enum rxrpc_congest_mode	mode:8;
	u8			cwnd;
	u8			ssthresh;
	u8			dup_acks;
	u8			cumulative_acks;
};

enum rxrpc_skb_trace {
	rxrpc_skb_rx_cleaned,
	rxrpc_skb_rx_freed,
	rxrpc_skb_rx_got,
	rxrpc_skb_rx_lost,
	rxrpc_skb_rx_received,
	rxrpc_skb_rx_rotated,
	rxrpc_skb_rx_purged,
	rxrpc_skb_rx_seen,
	rxrpc_skb_tx_cleaned,
	rxrpc_skb_tx_freed,
	rxrpc_skb_tx_got,
	rxrpc_skb_tx_new,
	rxrpc_skb_tx_rotated,
	rxrpc_skb_tx_seen,
	rxrpc_skb__nr_trace
};

extern const char rxrpc_skb_traces[rxrpc_skb__nr_trace][7];

enum rxrpc_conn_trace {
	rxrpc_conn_new_client,
	rxrpc_conn_new_service,
	rxrpc_conn_queued,
	rxrpc_conn_seen,
	rxrpc_conn_got,
	rxrpc_conn_put_client,
	rxrpc_conn_put_service,
	rxrpc_conn__nr_trace
};

extern const char rxrpc_conn_traces[rxrpc_conn__nr_trace][4];

enum rxrpc_client_trace {
	rxrpc_client_activate_chans,
	rxrpc_client_alloc,
	rxrpc_client_chan_activate,
	rxrpc_client_chan_disconnect,
	rxrpc_client_chan_pass,
	rxrpc_client_chan_unstarted,
	rxrpc_client_cleanup,
	rxrpc_client_count,
	rxrpc_client_discard,
	rxrpc_client_duplicate,
	rxrpc_client_exposed,
	rxrpc_client_replace,
	rxrpc_client_to_active,
	rxrpc_client_to_culled,
	rxrpc_client_to_idle,
	rxrpc_client_to_inactive,
	rxrpc_client_to_waiting,
	rxrpc_client_uncount,
	rxrpc_client__nr_trace
};

extern const char rxrpc_client_traces[rxrpc_client__nr_trace][7];
extern const char rxrpc_conn_cache_states[RXRPC_CONN__NR_CACHE_STATES][5];

enum rxrpc_call_trace {
	rxrpc_call_new_client,
	rxrpc_call_new_service,
	rxrpc_call_queued,
	rxrpc_call_queued_ref,
	rxrpc_call_seen,
	rxrpc_call_connected,
	rxrpc_call_release,
	rxrpc_call_got,
	rxrpc_call_got_userid,
	rxrpc_call_got_kernel,
	rxrpc_call_put,
	rxrpc_call_put_userid,
	rxrpc_call_put_kernel,
	rxrpc_call_put_noqueue,
	rxrpc_call_error,
	rxrpc_call__nr_trace
};

extern const char rxrpc_call_traces[rxrpc_call__nr_trace][4];

enum rxrpc_transmit_trace {
	rxrpc_transmit_wait,
	rxrpc_transmit_queue,
	rxrpc_transmit_queue_last,
	rxrpc_transmit_rotate,
	rxrpc_transmit_rotate_last,
	rxrpc_transmit_await_reply,
	rxrpc_transmit_end,
	rxrpc_transmit__nr_trace
};

extern const char rxrpc_transmit_traces[rxrpc_transmit__nr_trace][4];

enum rxrpc_receive_trace {
	rxrpc_receive_incoming,
	rxrpc_receive_queue,
	rxrpc_receive_queue_last,
	rxrpc_receive_front,
	rxrpc_receive_rotate,
	rxrpc_receive_end,
	rxrpc_receive__nr_trace
};

extern const char rxrpc_receive_traces[rxrpc_receive__nr_trace][4];

enum rxrpc_recvmsg_trace {
	rxrpc_recvmsg_enter,
	rxrpc_recvmsg_wait,
	rxrpc_recvmsg_dequeue,
	rxrpc_recvmsg_hole,
	rxrpc_recvmsg_next,
	rxrpc_recvmsg_cont,
	rxrpc_recvmsg_full,
	rxrpc_recvmsg_data_return,
	rxrpc_recvmsg_terminal,
	rxrpc_recvmsg_to_be_accepted,
	rxrpc_recvmsg_return,
	rxrpc_recvmsg__nr_trace
};

extern const char rxrpc_recvmsg_traces[rxrpc_recvmsg__nr_trace][5];

enum rxrpc_rtt_tx_trace {
	rxrpc_rtt_tx_ping,
	rxrpc_rtt_tx_data,
	rxrpc_rtt_tx__nr_trace
};

extern const char rxrpc_rtt_tx_traces[rxrpc_rtt_tx__nr_trace][5];

enum rxrpc_rtt_rx_trace {
	rxrpc_rtt_rx_ping_response,
	rxrpc_rtt_rx_requested_ack,
	rxrpc_rtt_rx__nr_trace
};

extern const char rxrpc_rtt_rx_traces[rxrpc_rtt_rx__nr_trace][5];

enum rxrpc_timer_trace {
	rxrpc_timer_begin,
	rxrpc_timer_init_for_reply,
	rxrpc_timer_init_for_send_reply,
	rxrpc_timer_expired,
	rxrpc_timer_set_for_ack,
	rxrpc_timer_set_for_ping,
	rxrpc_timer_set_for_resend,
	rxrpc_timer_set_for_send,
	rxrpc_timer__nr_trace
};

extern const char rxrpc_timer_traces[rxrpc_timer__nr_trace][8];

enum rxrpc_propose_ack_trace {
	rxrpc_propose_ack_client_tx_end,
	rxrpc_propose_ack_input_data,
	rxrpc_propose_ack_ping_for_lost_ack,
	rxrpc_propose_ack_ping_for_lost_reply,
	rxrpc_propose_ack_ping_for_params,
	rxrpc_propose_ack_processing_op,
	rxrpc_propose_ack_respond_to_ack,
	rxrpc_propose_ack_respond_to_ping,
	rxrpc_propose_ack_retry_tx,
	rxrpc_propose_ack_rotate_rx,
	rxrpc_propose_ack_terminal_ack,
	rxrpc_propose_ack__nr_trace
};

enum rxrpc_propose_ack_outcome {
	rxrpc_propose_ack_use,
	rxrpc_propose_ack_update,
	rxrpc_propose_ack_subsume,
	rxrpc_propose_ack__nr_outcomes
};

extern const char rxrpc_propose_ack_traces[rxrpc_propose_ack__nr_trace][8];
extern const char *const rxrpc_propose_ack_outcomes[rxrpc_propose_ack__nr_outcomes];

enum rxrpc_congest_change {
	rxrpc_cong_begin_retransmission,
	rxrpc_cong_cleared_nacks,
	rxrpc_cong_new_low_nack,
	rxrpc_cong_no_change,
	rxrpc_cong_progress,
	rxrpc_cong_retransmit_again,
	rxrpc_cong_rtt_window_end,
	rxrpc_cong_saw_nack,
	rxrpc_congest__nr_change
};

extern const char rxrpc_congest_modes[NR__RXRPC_CONGEST_MODES][10];
extern const char rxrpc_congest_changes[rxrpc_congest__nr_change][9];

extern const char *const rxrpc_pkts[];
extern const char rxrpc_ack_names[RXRPC_ACK__INVALID + 1][4];

#include <trace/events/rxrpc.h>
>>>>>>> v4.9.227

/*
 * af_rxrpc.c
 */
<<<<<<< HEAD
extern atomic_t rxrpc_n_skbs;
extern __be32 rxrpc_epoch;
=======
extern atomic_t rxrpc_n_tx_skbs, rxrpc_n_rx_skbs;
extern u32 rxrpc_epoch;
>>>>>>> v4.9.227
extern atomic_t rxrpc_debug_id;
extern struct workqueue_struct *rxrpc_workqueue;

/*
<<<<<<< HEAD
 * ar-accept.c
 */
void rxrpc_accept_incoming_calls(struct work_struct *);
struct rxrpc_call *rxrpc_accept_call(struct rxrpc_sock *, unsigned long);
int rxrpc_reject_call(struct rxrpc_sock *);

/*
 * ar-ack.c
 */
extern unsigned rxrpc_requested_ack_delay;
extern unsigned rxrpc_soft_ack_delay;
extern unsigned rxrpc_idle_ack_delay;
extern unsigned rxrpc_rx_window_size;
extern unsigned rxrpc_rx_mtu;
extern unsigned rxrpc_rx_jumbo_max;

void __rxrpc_propose_ACK(struct rxrpc_call *, u8, __be32, bool);
void rxrpc_propose_ACK(struct rxrpc_call *, u8, __be32, bool);
void rxrpc_process_call(struct work_struct *);

/*
 * ar-call.c
 */
extern unsigned rxrpc_max_call_lifetime;
extern unsigned rxrpc_dead_call_expiry;
=======
 * call_accept.c
 */
int rxrpc_service_prealloc(struct rxrpc_sock *, gfp_t);
void rxrpc_discard_prealloc(struct rxrpc_sock *);
struct rxrpc_call *rxrpc_new_incoming_call(struct rxrpc_local *,
					   struct rxrpc_connection *,
					   struct sk_buff *);
void rxrpc_accept_incoming_calls(struct rxrpc_local *);
struct rxrpc_call *rxrpc_accept_call(struct rxrpc_sock *, unsigned long,
				     rxrpc_notify_rx_t);
int rxrpc_reject_call(struct rxrpc_sock *);

/*
 * call_event.c
 */
void __rxrpc_set_timer(struct rxrpc_call *, enum rxrpc_timer_trace, ktime_t);
void rxrpc_set_timer(struct rxrpc_call *, enum rxrpc_timer_trace, ktime_t);
void rxrpc_propose_ACK(struct rxrpc_call *, u8, u16, u32, bool, bool,
		       enum rxrpc_propose_ack_trace);
void rxrpc_process_call(struct work_struct *);

/*
 * call_object.c
 */
extern const char *const rxrpc_call_states[];
extern const char *const rxrpc_call_completions[];
extern unsigned int rxrpc_max_call_lifetime;
>>>>>>> v4.9.227
extern struct kmem_cache *rxrpc_call_jar;
extern struct list_head rxrpc_calls;
extern rwlock_t rxrpc_call_lock;

<<<<<<< HEAD
struct rxrpc_call *rxrpc_find_call_hash(u8,  __be32, __be32, __be32,
					__be16, void *, sa_family_t, const u8 *);
struct rxrpc_call *rxrpc_get_client_call(struct rxrpc_sock *,
					 struct rxrpc_transport *,
					 struct rxrpc_conn_bundle *,
					 unsigned long, int, gfp_t);
struct rxrpc_call *rxrpc_incoming_call(struct rxrpc_sock *,
				       struct rxrpc_connection *,
				       struct rxrpc_header *, gfp_t);
struct rxrpc_call *rxrpc_find_server_call(struct rxrpc_sock *, unsigned long);
void rxrpc_release_call(struct rxrpc_call *);
void rxrpc_release_calls_on_socket(struct rxrpc_sock *);
void __rxrpc_put_call(struct rxrpc_call *);
void __exit rxrpc_destroy_all_calls(void);

/*
 * ar-connection.c
 */
extern unsigned rxrpc_connection_expiry;
extern struct list_head rxrpc_connections;
extern rwlock_t rxrpc_connection_lock;

struct rxrpc_conn_bundle *rxrpc_get_bundle(struct rxrpc_sock *,
					   struct rxrpc_transport *,
					   struct key *, __be16, gfp_t);
void rxrpc_put_bundle(struct rxrpc_transport *, struct rxrpc_conn_bundle *);
int rxrpc_connect_call(struct rxrpc_sock *, struct rxrpc_transport *,
		       struct rxrpc_conn_bundle *, struct rxrpc_call *, gfp_t);
void rxrpc_put_connection(struct rxrpc_connection *);
void __exit rxrpc_destroy_all_connections(void);
struct rxrpc_connection *rxrpc_find_connection(struct rxrpc_transport *,
					       struct rxrpc_header *);
extern struct rxrpc_connection *
rxrpc_incoming_connection(struct rxrpc_transport *, struct rxrpc_header *,
			  gfp_t);

/*
 * ar-connevent.c
 */
void rxrpc_process_connection(struct work_struct *);
void rxrpc_reject_packet(struct rxrpc_local *, struct sk_buff *);
void rxrpc_reject_packets(struct work_struct *);

/*
 * ar-error.c
 */
void rxrpc_UDP_error_report(struct sock *);
void rxrpc_UDP_error_handler(struct work_struct *);

/*
 * ar-input.c
 */
extern const char *rxrpc_pkts[];

void rxrpc_data_ready(struct sock *);
int rxrpc_queue_rcv_skb(struct rxrpc_call *, struct sk_buff *, bool, bool);
void rxrpc_fast_process_packet(struct rxrpc_call *, struct sk_buff *);

/*
 * ar-local.c
 */
extern rwlock_t rxrpc_local_lock;

struct rxrpc_local *rxrpc_lookup_local(struct sockaddr_rxrpc *);
void rxrpc_put_local(struct rxrpc_local *);
void __exit rxrpc_destroy_all_locals(void);

/*
 * ar-key.c
=======
struct rxrpc_call *rxrpc_find_call_by_user_ID(struct rxrpc_sock *, unsigned long);
struct rxrpc_call *rxrpc_alloc_call(gfp_t);
struct rxrpc_call *rxrpc_new_client_call(struct rxrpc_sock *,
					 struct rxrpc_conn_parameters *,
					 struct sockaddr_rxrpc *,
					 unsigned long, gfp_t);
void rxrpc_incoming_call(struct rxrpc_sock *, struct rxrpc_call *,
			 struct sk_buff *);
void rxrpc_release_call(struct rxrpc_sock *, struct rxrpc_call *);
void rxrpc_release_calls_on_socket(struct rxrpc_sock *);
bool __rxrpc_queue_call(struct rxrpc_call *);
bool rxrpc_queue_call(struct rxrpc_call *);
void rxrpc_see_call(struct rxrpc_call *);
void rxrpc_get_call(struct rxrpc_call *, enum rxrpc_call_trace);
void rxrpc_put_call(struct rxrpc_call *, enum rxrpc_call_trace);
void rxrpc_cleanup_call(struct rxrpc_call *);
void __exit rxrpc_destroy_all_calls(void);

static inline bool rxrpc_is_service_call(const struct rxrpc_call *call)
{
	return test_bit(RXRPC_CALL_IS_SERVICE, &call->flags);
}

static inline bool rxrpc_is_client_call(const struct rxrpc_call *call)
{
	return !rxrpc_is_service_call(call);
}

/*
 * Transition a call to the complete state.
 */
static inline bool __rxrpc_set_call_completion(struct rxrpc_call *call,
					       enum rxrpc_call_completion compl,
					       u32 abort_code,
					       int error)
{
	if (call->state < RXRPC_CALL_COMPLETE) {
		call->abort_code = abort_code;
		call->error = error;
		call->completion = compl,
		call->state = RXRPC_CALL_COMPLETE;
		wake_up(&call->waitq);
		return true;
	}
	return false;
}

static inline bool rxrpc_set_call_completion(struct rxrpc_call *call,
					     enum rxrpc_call_completion compl,
					     u32 abort_code,
					     int error)
{
	bool ret;

	write_lock_bh(&call->state_lock);
	ret = __rxrpc_set_call_completion(call, compl, abort_code, error);
	write_unlock_bh(&call->state_lock);
	return ret;
}

/*
 * Record that a call successfully completed.
 */
static inline bool __rxrpc_call_completed(struct rxrpc_call *call)
{
	return __rxrpc_set_call_completion(call, RXRPC_CALL_SUCCEEDED, 0, 0);
}

static inline bool rxrpc_call_completed(struct rxrpc_call *call)
{
	bool ret;

	write_lock_bh(&call->state_lock);
	ret = __rxrpc_call_completed(call);
	write_unlock_bh(&call->state_lock);
	return ret;
}

/*
 * Record that a call is locally aborted.
 */
static inline bool __rxrpc_abort_call(const char *why, struct rxrpc_call *call,
				      rxrpc_seq_t seq,
				      u32 abort_code, int error)
{
	trace_rxrpc_abort(why, call->cid, call->call_id, seq,
			  abort_code, error);
	return __rxrpc_set_call_completion(call, RXRPC_CALL_LOCALLY_ABORTED,
					   abort_code, error);
}

static inline bool rxrpc_abort_call(const char *why, struct rxrpc_call *call,
				    rxrpc_seq_t seq, u32 abort_code, int error)
{
	bool ret;

	write_lock_bh(&call->state_lock);
	ret = __rxrpc_abort_call(why, call, seq, abort_code, error);
	write_unlock_bh(&call->state_lock);
	return ret;
}

/*
 * conn_client.c
 */
extern unsigned int rxrpc_max_client_connections;
extern unsigned int rxrpc_reap_client_connections;
extern unsigned int rxrpc_conn_idle_client_expiry;
extern unsigned int rxrpc_conn_idle_client_fast_expiry;
extern struct idr rxrpc_client_conn_ids;

void rxrpc_destroy_client_conn_ids(void);
int rxrpc_connect_call(struct rxrpc_call *, struct rxrpc_conn_parameters *,
		       struct sockaddr_rxrpc *, gfp_t);
void rxrpc_expose_client_call(struct rxrpc_call *);
void rxrpc_disconnect_client_call(struct rxrpc_call *);
void rxrpc_put_client_conn(struct rxrpc_connection *);
void __exit rxrpc_destroy_all_client_connections(void);

/*
 * conn_event.c
 */
void rxrpc_process_connection(struct work_struct *);

/*
 * conn_object.c
 */
extern unsigned int rxrpc_connection_expiry;
extern struct list_head rxrpc_connections;
extern struct list_head rxrpc_connection_proc_list;
extern rwlock_t rxrpc_connection_lock;

int rxrpc_extract_addr_from_skb(struct sockaddr_rxrpc *, struct sk_buff *);
struct rxrpc_connection *rxrpc_alloc_connection(gfp_t);
struct rxrpc_connection *rxrpc_find_connection_rcu(struct rxrpc_local *,
						   struct sk_buff *);
void __rxrpc_disconnect_call(struct rxrpc_connection *, struct rxrpc_call *);
void rxrpc_disconnect_call(struct rxrpc_call *);
void rxrpc_kill_connection(struct rxrpc_connection *);
bool rxrpc_queue_conn(struct rxrpc_connection *);
void rxrpc_see_connection(struct rxrpc_connection *);
void rxrpc_get_connection(struct rxrpc_connection *);
struct rxrpc_connection *rxrpc_get_connection_maybe(struct rxrpc_connection *);
void rxrpc_put_service_conn(struct rxrpc_connection *);
void __exit rxrpc_destroy_all_connections(void);

static inline bool rxrpc_conn_is_client(const struct rxrpc_connection *conn)
{
	return conn->out_clientflag;
}

static inline bool rxrpc_conn_is_service(const struct rxrpc_connection *conn)
{
	return !rxrpc_conn_is_client(conn);
}

static inline void rxrpc_put_connection(struct rxrpc_connection *conn)
{
	if (!conn)
		return;

	if (rxrpc_conn_is_client(conn))
		rxrpc_put_client_conn(conn);
	else
		rxrpc_put_service_conn(conn);
}

/*
 * conn_service.c
 */
struct rxrpc_connection *rxrpc_find_service_conn_rcu(struct rxrpc_peer *,
						     struct sk_buff *);
struct rxrpc_connection *rxrpc_prealloc_service_connection(gfp_t);
void rxrpc_new_incoming_connection(struct rxrpc_connection *, struct sk_buff *);
void rxrpc_unpublish_service_conn(struct rxrpc_connection *);

/*
 * input.c
 */
void rxrpc_data_ready(struct sock *);

/*
 * insecure.c
 */
extern const struct rxrpc_security rxrpc_no_security;

/*
 * key.c
>>>>>>> v4.9.227
 */
extern struct key_type key_type_rxrpc;
extern struct key_type key_type_rxrpc_s;

int rxrpc_request_key(struct rxrpc_sock *, char __user *, int);
int rxrpc_server_keyring(struct rxrpc_sock *, char __user *, int);
int rxrpc_get_server_data_key(struct rxrpc_connection *, const void *, time_t,
			      u32);

/*
<<<<<<< HEAD
 * ar-output.c
 */
extern unsigned rxrpc_resend_timeout;

int rxrpc_send_packet(struct rxrpc_transport *, struct sk_buff *);
int rxrpc_client_sendmsg(struct kiocb *, struct rxrpc_sock *,
			 struct rxrpc_transport *, struct msghdr *, size_t);
int rxrpc_server_sendmsg(struct kiocb *, struct rxrpc_sock *, struct msghdr *,
			 size_t);

/*
 * ar-peer.c
 */
struct rxrpc_peer *rxrpc_get_peer(struct sockaddr_rxrpc *, gfp_t);
void rxrpc_put_peer(struct rxrpc_peer *);
struct rxrpc_peer *rxrpc_find_peer(struct rxrpc_local *, __be32, __be16);
void __exit rxrpc_destroy_all_peers(void);

/*
 * ar-proc.c
 */
extern const char *const rxrpc_call_states[];
=======
 * local_event.c
 */
extern void rxrpc_process_local_events(struct rxrpc_local *);

/*
 * local_object.c
 */
struct rxrpc_local *rxrpc_lookup_local(const struct sockaddr_rxrpc *);
void __rxrpc_put_local(struct rxrpc_local *);
void __exit rxrpc_destroy_all_locals(void);

static inline void rxrpc_get_local(struct rxrpc_local *local)
{
	atomic_inc(&local->usage);
}

static inline
struct rxrpc_local *rxrpc_get_local_maybe(struct rxrpc_local *local)
{
	return atomic_inc_not_zero(&local->usage) ? local : NULL;
}

static inline void rxrpc_put_local(struct rxrpc_local *local)
{
	if (local && atomic_dec_and_test(&local->usage))
		__rxrpc_put_local(local);
}

static inline void rxrpc_queue_local(struct rxrpc_local *local)
{
	rxrpc_queue_work(&local->processor);
}

/*
 * misc.c
 */
extern unsigned int rxrpc_max_backlog __read_mostly;
extern unsigned int rxrpc_requested_ack_delay;
extern unsigned int rxrpc_soft_ack_delay;
extern unsigned int rxrpc_idle_ack_delay;
extern unsigned int rxrpc_rx_window_size;
extern unsigned int rxrpc_rx_mtu;
extern unsigned int rxrpc_rx_jumbo_max;
extern unsigned int rxrpc_resend_timeout;

extern const s8 rxrpc_ack_priority[];

/*
 * output.c
 */
int rxrpc_send_ack_packet(struct rxrpc_call *, bool);
int rxrpc_send_abort_packet(struct rxrpc_call *);
int rxrpc_send_data_packet(struct rxrpc_call *, struct sk_buff *, bool);
void rxrpc_reject_packets(struct rxrpc_local *);

/*
 * peer_event.c
 */
void rxrpc_error_report(struct sock *);
void rxrpc_peer_error_distributor(struct work_struct *);
void rxrpc_peer_add_rtt(struct rxrpc_call *, enum rxrpc_rtt_rx_trace,
			rxrpc_serial_t, rxrpc_serial_t, ktime_t, ktime_t);

/*
 * peer_object.c
 */
struct rxrpc_peer *rxrpc_lookup_peer_rcu(struct rxrpc_local *,
					 const struct sockaddr_rxrpc *);
struct rxrpc_peer *rxrpc_lookup_peer(struct rxrpc_local *,
				     struct sockaddr_rxrpc *, gfp_t);
struct rxrpc_peer *rxrpc_alloc_peer(struct rxrpc_local *, gfp_t);
struct rxrpc_peer *rxrpc_lookup_incoming_peer(struct rxrpc_local *,
					      struct rxrpc_peer *);

static inline struct rxrpc_peer *rxrpc_get_peer(struct rxrpc_peer *peer)
{
	atomic_inc(&peer->usage);
	return peer;
}

static inline
struct rxrpc_peer *rxrpc_get_peer_maybe(struct rxrpc_peer *peer)
{
	return atomic_inc_not_zero(&peer->usage) ? peer : NULL;
}

extern void __rxrpc_put_peer(struct rxrpc_peer *peer);
static inline void rxrpc_put_peer(struct rxrpc_peer *peer)
{
	if (peer && atomic_dec_and_test(&peer->usage))
		__rxrpc_put_peer(peer);
}

/*
 * proc.c
 */
>>>>>>> v4.9.227
extern const struct file_operations rxrpc_call_seq_fops;
extern const struct file_operations rxrpc_connection_seq_fops;

/*
<<<<<<< HEAD
 * ar-recvmsg.c
 */
void rxrpc_remove_user_ID(struct rxrpc_sock *, struct rxrpc_call *);
int rxrpc_recvmsg(struct kiocb *, struct socket *, struct msghdr *, size_t,
		  int);

/*
 * ar-security.c
 */
int rxrpc_register_security(struct rxrpc_security *);
void rxrpc_unregister_security(struct rxrpc_security *);
int rxrpc_init_client_conn_security(struct rxrpc_connection *);
int rxrpc_init_server_conn_security(struct rxrpc_connection *);
int rxrpc_secure_packet(const struct rxrpc_call *, struct sk_buff *, size_t,
			void *);
int rxrpc_verify_packet(const struct rxrpc_call *, struct sk_buff *, u32 *);
void rxrpc_clear_conn_security(struct rxrpc_connection *);

/*
 * ar-skbuff.c
 */
void rxrpc_packet_destructor(struct sk_buff *);

/*
 * ar-transport.c
 */
extern unsigned rxrpc_transport_expiry;

struct rxrpc_transport *rxrpc_get_transport(struct rxrpc_local *,
					    struct rxrpc_peer *, gfp_t);
void rxrpc_put_transport(struct rxrpc_transport *);
void __exit rxrpc_destroy_all_transports(void);
struct rxrpc_transport *rxrpc_find_transport(struct rxrpc_local *,
					     struct rxrpc_peer *);
=======
 * recvmsg.c
 */
void rxrpc_notify_socket(struct rxrpc_call *);
int rxrpc_recvmsg(struct socket *, struct msghdr *, size_t, int);

/*
 * rxkad.c
 */
#ifdef CONFIG_RXKAD
extern const struct rxrpc_security rxkad;
#endif

/*
 * security.c
 */
int __init rxrpc_init_security(void);
void rxrpc_exit_security(void);
int rxrpc_init_client_conn_security(struct rxrpc_connection *);
int rxrpc_init_server_conn_security(struct rxrpc_connection *);

/*
 * sendmsg.c
 */
int rxrpc_do_sendmsg(struct rxrpc_sock *, struct msghdr *, size_t);

/*
 * skbuff.c
 */
void rxrpc_kernel_data_consumed(struct rxrpc_call *, struct sk_buff *);
void rxrpc_packet_destructor(struct sk_buff *);
void rxrpc_new_skb(struct sk_buff *, enum rxrpc_skb_trace);
void rxrpc_see_skb(struct sk_buff *, enum rxrpc_skb_trace);
void rxrpc_get_skb(struct sk_buff *, enum rxrpc_skb_trace);
void rxrpc_free_skb(struct sk_buff *, enum rxrpc_skb_trace);
void rxrpc_lose_skb(struct sk_buff *, enum rxrpc_skb_trace);
void rxrpc_purge_queue(struct sk_buff_head *);
>>>>>>> v4.9.227

/*
 * sysctl.c
 */
#ifdef CONFIG_SYSCTL
extern int __init rxrpc_sysctl_init(void);
extern void rxrpc_sysctl_exit(void);
#else
static inline int __init rxrpc_sysctl_init(void) { return 0; }
static inline void rxrpc_sysctl_exit(void) {}
#endif

/*
<<<<<<< HEAD
=======
 * utils.c
 */
int rxrpc_extract_addr_from_skb(struct sockaddr_rxrpc *, struct sk_buff *);

static inline bool before(u32 seq1, u32 seq2)
{
        return (s32)(seq1 - seq2) < 0;
}
static inline bool before_eq(u32 seq1, u32 seq2)
{
        return (s32)(seq1 - seq2) <= 0;
}
static inline bool after(u32 seq1, u32 seq2)
{
        return (s32)(seq1 - seq2) > 0;
}
static inline bool after_eq(u32 seq1, u32 seq2)
{
        return (s32)(seq1 - seq2) >= 0;
}

/*
>>>>>>> v4.9.227
 * debug tracing
 */
extern unsigned int rxrpc_debug;

#define dbgprintk(FMT,...) \
	printk("[%-6.6s] "FMT"\n", current->comm ,##__VA_ARGS__)

#define kenter(FMT,...)	dbgprintk("==> %s("FMT")",__func__ ,##__VA_ARGS__)
#define kleave(FMT,...)	dbgprintk("<== %s()"FMT"",__func__ ,##__VA_ARGS__)
#define kdebug(FMT,...)	dbgprintk("    "FMT ,##__VA_ARGS__)
#define kproto(FMT,...)	dbgprintk("### "FMT ,##__VA_ARGS__)
#define knet(FMT,...)	dbgprintk("@@@ "FMT ,##__VA_ARGS__)


#if defined(__KDEBUG)
#define _enter(FMT,...)	kenter(FMT,##__VA_ARGS__)
#define _leave(FMT,...)	kleave(FMT,##__VA_ARGS__)
#define _debug(FMT,...)	kdebug(FMT,##__VA_ARGS__)
#define _proto(FMT,...)	kproto(FMT,##__VA_ARGS__)
#define _net(FMT,...)	knet(FMT,##__VA_ARGS__)

#elif defined(CONFIG_AF_RXRPC_DEBUG)
#define RXRPC_DEBUG_KENTER	0x01
#define RXRPC_DEBUG_KLEAVE	0x02
#define RXRPC_DEBUG_KDEBUG	0x04
#define RXRPC_DEBUG_KPROTO	0x08
#define RXRPC_DEBUG_KNET	0x10

#define _enter(FMT,...)					\
do {							\
	if (unlikely(rxrpc_debug & RXRPC_DEBUG_KENTER))	\
		kenter(FMT,##__VA_ARGS__);		\
} while (0)

#define _leave(FMT,...)					\
do {							\
	if (unlikely(rxrpc_debug & RXRPC_DEBUG_KLEAVE))	\
		kleave(FMT,##__VA_ARGS__);		\
} while (0)

#define _debug(FMT,...)					\
do {							\
	if (unlikely(rxrpc_debug & RXRPC_DEBUG_KDEBUG))	\
		kdebug(FMT,##__VA_ARGS__);		\
} while (0)

#define _proto(FMT,...)					\
do {							\
	if (unlikely(rxrpc_debug & RXRPC_DEBUG_KPROTO))	\
		kproto(FMT,##__VA_ARGS__);		\
} while (0)

#define _net(FMT,...)					\
do {							\
	if (unlikely(rxrpc_debug & RXRPC_DEBUG_KNET))	\
		knet(FMT,##__VA_ARGS__);		\
} while (0)

#else
#define _enter(FMT,...)	no_printk("==> %s("FMT")",__func__ ,##__VA_ARGS__)
#define _leave(FMT,...)	no_printk("<== %s()"FMT"",__func__ ,##__VA_ARGS__)
#define _debug(FMT,...)	no_printk("    "FMT ,##__VA_ARGS__)
#define _proto(FMT,...)	no_printk("### "FMT ,##__VA_ARGS__)
#define _net(FMT,...)	no_printk("@@@ "FMT ,##__VA_ARGS__)
#endif

/*
 * debug assertion checking
 */
#if 1 // defined(__KDEBUGALL)

#define ASSERT(X)						\
do {								\
	if (unlikely(!(X))) {					\
<<<<<<< HEAD
		printk(KERN_ERR "\n");				\
		printk(KERN_ERR "RxRPC: Assertion failed\n");	\
		BUG();						\
	}							\
} while(0)

#define ASSERTCMP(X, OP, Y)						\
do {									\
	if (unlikely(!((X) OP (Y)))) {					\
		printk(KERN_ERR "\n");					\
		printk(KERN_ERR "RxRPC: Assertion failed\n");		\
		printk(KERN_ERR "%lu " #OP " %lu is false\n",		\
		       (unsigned long)(X), (unsigned long)(Y));		\
		printk(KERN_ERR "0x%lx " #OP " 0x%lx is false\n",	\
		       (unsigned long)(X), (unsigned long)(Y));		\
		BUG();							\
	}								\
} while(0)
=======
		pr_err("Assertion failed\n");			\
		BUG();						\
	}							\
} while (0)

#define ASSERTCMP(X, OP, Y)						\
do {									\
	__typeof__(X) _x = (X);						\
	__typeof__(Y) _y = (__typeof__(X))(Y);				\
	if (unlikely(!(_x OP _y))) {					\
		pr_err("Assertion failed - %lu(0x%lx) %s %lu(0x%lx) is false\n", \
		       (unsigned long)_x, (unsigned long)_x, #OP,	\
		       (unsigned long)_y, (unsigned long)_y);		\
		BUG();							\
	}								\
} while (0)
>>>>>>> v4.9.227

#define ASSERTIF(C, X)						\
do {								\
	if (unlikely((C) && !(X))) {				\
<<<<<<< HEAD
		printk(KERN_ERR "\n");				\
		printk(KERN_ERR "RxRPC: Assertion failed\n");	\
		BUG();						\
	}							\
} while(0)

#define ASSERTIFCMP(C, X, OP, Y)					\
do {									\
	if (unlikely((C) && !((X) OP (Y)))) {				\
		printk(KERN_ERR "\n");					\
		printk(KERN_ERR "RxRPC: Assertion failed\n");		\
		printk(KERN_ERR "%lu " #OP " %lu is false\n",		\
		       (unsigned long)(X), (unsigned long)(Y));		\
		printk(KERN_ERR "0x%lx " #OP " 0x%lx is false\n",	\
		       (unsigned long)(X), (unsigned long)(Y));		\
		BUG();							\
	}								\
} while(0)
=======
		pr_err("Assertion failed\n");			\
		BUG();						\
	}							\
} while (0)

#define ASSERTIFCMP(C, X, OP, Y)					\
do {									\
	__typeof__(X) _x = (X);						\
	__typeof__(Y) _y = (__typeof__(X))(Y);				\
	if (unlikely((C) && !(_x OP _y))) {				\
		pr_err("Assertion failed - %lu(0x%lx) %s %lu(0x%lx) is false\n", \
		       (unsigned long)_x, (unsigned long)_x, #OP,	\
		       (unsigned long)_y, (unsigned long)_y);		\
		BUG();							\
	}								\
} while (0)
>>>>>>> v4.9.227

#else

#define ASSERT(X)				\
do {						\
<<<<<<< HEAD
} while(0)

#define ASSERTCMP(X, OP, Y)			\
do {						\
} while(0)

#define ASSERTIF(C, X)				\
do {						\
} while(0)

#define ASSERTIFCMP(C, X, OP, Y)		\
do {						\
} while(0)

#endif /* __KDEBUGALL */

/*
 * socket buffer accounting / leak finding
 */
static inline void __rxrpc_new_skb(struct sk_buff *skb, const char *fn)
{
	//_net("new skb %p %s [%d]", skb, fn, atomic_read(&rxrpc_n_skbs));
	//atomic_inc(&rxrpc_n_skbs);
}

#define rxrpc_new_skb(skb) __rxrpc_new_skb((skb), __func__)

static inline void __rxrpc_kill_skb(struct sk_buff *skb, const char *fn)
{
	//_net("kill skb %p %s [%d]", skb, fn, atomic_read(&rxrpc_n_skbs));
	//atomic_dec(&rxrpc_n_skbs);
}

#define rxrpc_kill_skb(skb) __rxrpc_kill_skb((skb), __func__)

static inline void __rxrpc_free_skb(struct sk_buff *skb, const char *fn)
{
	if (skb) {
		CHECK_SLAB_OKAY(&skb->users);
		//_net("free skb %p %s [%d]",
		//     skb, fn, atomic_read(&rxrpc_n_skbs));
		//atomic_dec(&rxrpc_n_skbs);
		kfree_skb(skb);
	}
}

#define rxrpc_free_skb(skb) __rxrpc_free_skb((skb), __func__)

static inline void rxrpc_purge_queue(struct sk_buff_head *list)
{
	struct sk_buff *skb;
	while ((skb = skb_dequeue((list))) != NULL)
		rxrpc_free_skb(skb);
}

static inline void __rxrpc_get_local(struct rxrpc_local *local, const char *f)
{
	CHECK_SLAB_OKAY(&local->usage);
	if (atomic_inc_return(&local->usage) == 1)
		printk("resurrected (%s)\n", f);
}

#define rxrpc_get_local(LOCAL) __rxrpc_get_local((LOCAL), __func__)

#define rxrpc_get_call(CALL)				\
do {							\
	CHECK_SLAB_OKAY(&(CALL)->usage);		\
	if (atomic_inc_return(&(CALL)->usage) == 1)	\
		BUG();					\
} while(0)

#define rxrpc_put_call(CALL)				\
do {							\
	__rxrpc_put_call(CALL);				\
} while(0)
=======
} while (0)

#define ASSERTCMP(X, OP, Y)			\
do {						\
} while (0)

#define ASSERTIF(C, X)				\
do {						\
} while (0)

#define ASSERTIFCMP(C, X, OP, Y)		\
do {						\
} while (0)

#endif /* __KDEBUGALL */
>>>>>>> v4.9.227
