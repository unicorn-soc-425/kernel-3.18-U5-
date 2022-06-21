/*
 * GPL HEADER START
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 only,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License version 2 for more details (a copy is included
 * in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; If not, see
<<<<<<< HEAD
 * http://www.sun.com/software/products/lustre/docs/GPLv2.pdf
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
=======
 * http://www.gnu.org/licenses/gpl-2.0.html
>>>>>>> v4.9.227
 *
 * GPL HEADER END
 */
/*
 * Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.
 * Use is subject to license terms.
 */
/*
 * This file is part of Lustre, http://www.lustre.org/
 * Lustre is a trademark of Sun Microsystems, Inc.
 *
 * libcfs/include/libcfs/libcfs_ioctl.h
 *
 * Low-level ioctl data structures. Kernel ioctl functions declared here,
<<<<<<< HEAD
 * and user space functions are in libcfsutil_ioctl.h.
=======
 * and user space functions are in libcfs/util/ioctl.h.
>>>>>>> v4.9.227
 *
 */

#ifndef __LIBCFS_IOCTL_H__
#define __LIBCFS_IOCTL_H__

<<<<<<< HEAD
#define LIBCFS_IOCTL_VERSION 0x0001000a

struct libcfs_ioctl_data {
	__u32 ioc_len;
	__u32 ioc_version;
=======
#include <linux/types.h>
#include <linux/ioctl.h>

#define LIBCFS_IOCTL_VERSION	0x0001000a
#define LIBCFS_IOCTL_VERSION2	0x0001000b

struct libcfs_ioctl_hdr {
	__u32 ioc_len;
	__u32 ioc_version;
};

/** max size to copy from userspace */
#define LIBCFS_IOC_DATA_MAX	(128 * 1024)

struct libcfs_ioctl_data {
	struct libcfs_ioctl_hdr ioc_hdr;
>>>>>>> v4.9.227

	__u64 ioc_nid;
	__u64 ioc_u64[1];

	__u32 ioc_flags;
	__u32 ioc_count;
	__u32 ioc_net;
	__u32 ioc_u32[7];

	__u32 ioc_inllen1;
	char *ioc_inlbuf1;
	__u32 ioc_inllen2;
	char *ioc_inlbuf2;

	__u32 ioc_plen1; /* buffers in userspace */
<<<<<<< HEAD
	char *ioc_pbuf1;
	__u32 ioc_plen2; /* buffers in userspace */
	char *ioc_pbuf2;
=======
	void __user *ioc_pbuf1;
	__u32 ioc_plen2; /* buffers in userspace */
	void __user *ioc_pbuf2;
>>>>>>> v4.9.227

	char ioc_bulk[0];
};

<<<<<<< HEAD
#define ioc_priority ioc_u32[0]

struct libcfs_ioctl_hdr {
	__u32 ioc_len;
	__u32 ioc_version;
};

=======
>>>>>>> v4.9.227
struct libcfs_debug_ioctl_data {
	struct libcfs_ioctl_hdr hdr;
	unsigned int subs;
	unsigned int debug;
};

<<<<<<< HEAD
#define LIBCFS_IOC_INIT(data)			   \
do {						    \
	memset(&data, 0, sizeof(data));		 \
	data.ioc_version = LIBCFS_IOCTL_VERSION;	\
	data.ioc_len = sizeof(data);		    \
} while (0)

struct libcfs_ioctl_handler {
	struct list_head item;
	int (*handle_ioctl)(unsigned int cmd, struct libcfs_ioctl_data *data);
};

#define DECLARE_IOCTL_HANDLER(ident, func)		      \
	struct libcfs_ioctl_handler ident = {		   \
		/* .item = */ LIST_HEAD_INIT(ident.item),   \
		/* .handle_ioctl = */ func		      \
	}

/* FIXME check conflict with lustre_lib.h */
#define LIBCFS_IOC_DEBUG_MASK	     _IOWR('f', 250, long)

/* ioctls for manipulating snapshots 30- */
#define IOC_LIBCFS_TYPE		   'e'
#define IOC_LIBCFS_MIN_NR		 30
/* libcfs ioctls */
#define IOC_LIBCFS_PANIC		   _IOWR('e', 30, long)
#define IOC_LIBCFS_CLEAR_DEBUG	     _IOWR('e', 31, long)
#define IOC_LIBCFS_MARK_DEBUG	      _IOWR('e', 32, long)
#define IOC_LIBCFS_MEMHOG		  _IOWR('e', 36, long)
#define IOC_LIBCFS_PING_TEST	       _IOWR('e', 37, long)
/* lnet ioctls */
#define IOC_LIBCFS_GET_NI		  _IOWR('e', 50, long)
#define IOC_LIBCFS_FAIL_NID		_IOWR('e', 51, long)
#define IOC_LIBCFS_ADD_ROUTE	       _IOWR('e', 52, long)
#define IOC_LIBCFS_DEL_ROUTE	       _IOWR('e', 53, long)
#define IOC_LIBCFS_GET_ROUTE	       _IOWR('e', 54, long)
#define IOC_LIBCFS_NOTIFY_ROUTER	   _IOWR('e', 55, long)
#define IOC_LIBCFS_UNCONFIGURE	     _IOWR('e', 56, long)
#define IOC_LIBCFS_PORTALS_COMPATIBILITY   _IOWR('e', 57, long)
#define IOC_LIBCFS_LNET_DIST	       _IOWR('e', 58, long)
#define IOC_LIBCFS_CONFIGURE	       _IOWR('e', 59, long)
#define IOC_LIBCFS_TESTPROTOCOMPAT	 _IOWR('e', 60, long)
#define IOC_LIBCFS_PING		    _IOWR('e', 61, long)
#define IOC_LIBCFS_DEBUG_PEER	      _IOWR('e', 62, long)
#define IOC_LIBCFS_LNETST		  _IOWR('e', 63, long)
/* lnd ioctls */
#define IOC_LIBCFS_REGISTER_MYNID	  _IOWR('e', 70, long)
#define IOC_LIBCFS_CLOSE_CONNECTION	_IOWR('e', 71, long)
#define IOC_LIBCFS_PUSH_CONNECTION	 _IOWR('e', 72, long)
#define IOC_LIBCFS_GET_CONN		_IOWR('e', 73, long)
#define IOC_LIBCFS_DEL_PEER		_IOWR('e', 74, long)
#define IOC_LIBCFS_ADD_PEER		_IOWR('e', 75, long)
#define IOC_LIBCFS_GET_PEER		_IOWR('e', 76, long)
#define IOC_LIBCFS_GET_TXDESC	      _IOWR('e', 77, long)
#define IOC_LIBCFS_ADD_INTERFACE	   _IOWR('e', 78, long)
#define IOC_LIBCFS_DEL_INTERFACE	   _IOWR('e', 79, long)
#define IOC_LIBCFS_GET_INTERFACE	   _IOWR('e', 80, long)

#define IOC_LIBCFS_MAX_NR			     80

static inline int libcfs_ioctl_packlen(struct libcfs_ioctl_data *data)
{
	int len = sizeof(*data);

	len += cfs_size_round(data->ioc_inllen1);
	len += cfs_size_round(data->ioc_inllen2);
	return len;
}

static inline int libcfs_ioctl_is_invalid(struct libcfs_ioctl_data *data)
{
	if (data->ioc_len > (1<<30)) {
		CERROR("LIBCFS ioctl: ioc_len larger than 1<<30\n");
		return 1;
	}
	if (data->ioc_inllen1 > (1<<30)) {
		CERROR("LIBCFS ioctl: ioc_inllen1 larger than 1<<30\n");
		return 1;
	}
	if (data->ioc_inllen2 > (1<<30)) {
		CERROR("LIBCFS ioctl: ioc_inllen2 larger than 1<<30\n");
		return 1;
	}
	if (data->ioc_inlbuf1 && !data->ioc_inllen1) {
		CERROR("LIBCFS ioctl: inlbuf1 pointer but 0 length\n");
		return 1;
	}
	if (data->ioc_inlbuf2 && !data->ioc_inllen2) {
		CERROR("LIBCFS ioctl: inlbuf2 pointer but 0 length\n");
		return 1;
	}
	if (data->ioc_pbuf1 && !data->ioc_plen1) {
		CERROR("LIBCFS ioctl: pbuf1 pointer but 0 length\n");
		return 1;
	}
	if (data->ioc_pbuf2 && !data->ioc_plen2) {
		CERROR("LIBCFS ioctl: pbuf2 pointer but 0 length\n");
		return 1;
	}
	if (data->ioc_plen1 && !data->ioc_pbuf1) {
		CERROR("LIBCFS ioctl: plen1 nonzero but no pbuf1 pointer\n");
		return 1;
	}
	if (data->ioc_plen2 && !data->ioc_pbuf2) {
		CERROR("LIBCFS ioctl: plen2 nonzero but no pbuf2 pointer\n");
		return 1;
	}
	if ((__u32)libcfs_ioctl_packlen(data) != data->ioc_len) {
		CERROR("LIBCFS ioctl: packlen != ioc_len\n");
		return 1;
	}
	if (data->ioc_inllen1 &&
	    data->ioc_bulk[data->ioc_inllen1 - 1] != '\0') {
		CERROR("LIBCFS ioctl: inlbuf1 not 0 terminated\n");
		return 1;
	}
	if (data->ioc_inllen2 &&
	    data->ioc_bulk[cfs_size_round(data->ioc_inllen1) +
			   data->ioc_inllen2 - 1] != '\0') {
		CERROR("LIBCFS ioctl: inlbuf2 not 0 terminated\n");
		return 1;
	}
	return 0;
}

int libcfs_register_ioctl(struct libcfs_ioctl_handler *hand);
int libcfs_deregister_ioctl(struct libcfs_ioctl_handler *hand);
int libcfs_ioctl_getdata(char *buf, char *end, void *arg);
int libcfs_ioctl_popdata(void *arg, void *buf, int size);
=======
/* 'f' ioctls are defined in lustre_ioctl.h and lustre_user.h except for: */
#define LIBCFS_IOC_DEBUG_MASK		   _IOWR('f', 250, long)
#define IOCTL_LIBCFS_TYPE		   long

#define IOC_LIBCFS_TYPE			   ('e')
#define IOC_LIBCFS_MIN_NR		   30
/* libcfs ioctls */
/* IOC_LIBCFS_PANIC obsolete in 2.8.0, was _IOWR('e', 30, IOCTL_LIBCFS_TYPE) */
#define IOC_LIBCFS_CLEAR_DEBUG		   _IOWR('e', 31, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_MARK_DEBUG		   _IOWR('e', 32, IOCTL_LIBCFS_TYPE)
/* IOC_LIBCFS_MEMHOG obsolete in 2.8.0, was _IOWR('e', 36, IOCTL_LIBCFS_TYPE) */
/* lnet ioctls */
#define IOC_LIBCFS_GET_NI		   _IOWR('e', 50, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_FAIL_NID		   _IOWR('e', 51, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_NOTIFY_ROUTER	   _IOWR('e', 55, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_UNCONFIGURE		   _IOWR('e', 56, IOCTL_LIBCFS_TYPE)
/*	 IOC_LIBCFS_PORTALS_COMPATIBILITY  _IOWR('e', 57, IOCTL_LIBCFS_TYPE) */
#define IOC_LIBCFS_LNET_DIST		   _IOWR('e', 58, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_CONFIGURE		   _IOWR('e', 59, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_TESTPROTOCOMPAT	   _IOWR('e', 60, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_PING			   _IOWR('e', 61, IOCTL_LIBCFS_TYPE)
/*	IOC_LIBCFS_DEBUG_PEER		   _IOWR('e', 62, IOCTL_LIBCFS_TYPE) */
#define IOC_LIBCFS_LNETST		   _IOWR('e', 63, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_LNET_FAULT		   _IOWR('e', 64, IOCTL_LIBCFS_TYPE)
/* lnd ioctls */
#define IOC_LIBCFS_REGISTER_MYNID	   _IOWR('e', 70, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_CLOSE_CONNECTION	   _IOWR('e', 71, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_PUSH_CONNECTION	   _IOWR('e', 72, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_GET_CONN		   _IOWR('e', 73, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_DEL_PEER		   _IOWR('e', 74, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_ADD_PEER		   _IOWR('e', 75, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_GET_PEER		   _IOWR('e', 76, IOCTL_LIBCFS_TYPE)
/* ioctl 77 is free for use */
#define IOC_LIBCFS_ADD_INTERFACE	   _IOWR('e', 78, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_DEL_INTERFACE	   _IOWR('e', 79, IOCTL_LIBCFS_TYPE)
#define IOC_LIBCFS_GET_INTERFACE	   _IOWR('e', 80, IOCTL_LIBCFS_TYPE)

/*
 * DLC Specific IOCTL numbers.
 * In order to maintain backward compatibility with any possible external
 * tools which might be accessing the IOCTL numbers, a new group of IOCTL
 * number have been allocated.
 */
#define IOCTL_CONFIG_SIZE		struct lnet_ioctl_config_data
#define IOC_LIBCFS_ADD_ROUTE		_IOWR(IOC_LIBCFS_TYPE, 81, IOCTL_CONFIG_SIZE)
#define IOC_LIBCFS_DEL_ROUTE		_IOWR(IOC_LIBCFS_TYPE, 82, IOCTL_CONFIG_SIZE)
#define IOC_LIBCFS_GET_ROUTE		_IOWR(IOC_LIBCFS_TYPE, 83, IOCTL_CONFIG_SIZE)
#define IOC_LIBCFS_ADD_NET		_IOWR(IOC_LIBCFS_TYPE, 84, IOCTL_CONFIG_SIZE)
#define IOC_LIBCFS_DEL_NET		_IOWR(IOC_LIBCFS_TYPE, 85, IOCTL_CONFIG_SIZE)
#define IOC_LIBCFS_GET_NET		_IOWR(IOC_LIBCFS_TYPE, 86, IOCTL_CONFIG_SIZE)
#define IOC_LIBCFS_CONFIG_RTR		_IOWR(IOC_LIBCFS_TYPE, 87, IOCTL_CONFIG_SIZE)
#define IOC_LIBCFS_ADD_BUF		_IOWR(IOC_LIBCFS_TYPE, 88, IOCTL_CONFIG_SIZE)
#define IOC_LIBCFS_GET_BUF		_IOWR(IOC_LIBCFS_TYPE, 89, IOCTL_CONFIG_SIZE)
#define IOC_LIBCFS_GET_PEER_INFO	_IOWR(IOC_LIBCFS_TYPE, 90, IOCTL_CONFIG_SIZE)
#define IOC_LIBCFS_GET_LNET_STATS	_IOWR(IOC_LIBCFS_TYPE, 91, IOCTL_CONFIG_SIZE)
#define IOC_LIBCFS_MAX_NR		91
>>>>>>> v4.9.227

#endif /* __LIBCFS_IOCTL_H__ */
