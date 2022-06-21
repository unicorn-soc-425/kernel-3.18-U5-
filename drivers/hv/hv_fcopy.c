/*
 * An implementation of file copy service.
 *
 * Copyright (C) 2014, Microsoft, Inc.
 *
 * Author : K. Y. Srinivasan <ksrinivasan@novell.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 * NON INFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

<<<<<<< HEAD
#include <linux/semaphore.h>
#include <linux/fs.h>
#include <linux/nls.h>
#include <linux/workqueue.h>
#include <linux/cdev.h>
#include <linux/hyperv.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

#include "hyperv_vmbus.h"
=======
#include <linux/nls.h>
#include <linux/workqueue.h>
#include <linux/hyperv.h>
#include <linux/sched.h>

#include "hyperv_vmbus.h"
#include "hv_utils_transport.h"
>>>>>>> v4.9.227

#define WIN8_SRV_MAJOR		1
#define WIN8_SRV_MINOR		1
#define WIN8_SRV_VERSION	(WIN8_SRV_MAJOR << 16 | WIN8_SRV_MINOR)

/*
 * Global state maintained for transaction that is being processed.
 * For a class of integration services, including the "file copy service",
 * the specified protocol is a "request/response" protocol which means that
 * there can only be single outstanding transaction from the host at any
 * given point in time. We use this to simplify memory management in this
 * driver - we cache and process only one message at a time.
 *
 * While the request/response protocol is guaranteed by the host, we further
 * ensure this by serializing packet processing in this driver - we do not
 * read additional packets from the VMBUs until the current packet is fully
 * handled.
<<<<<<< HEAD
 *
 * The transaction "active" state is set when we receive a request from the
 * host and we cleanup this state when the transaction is completed - when we
 * respond to the host with our response. When the transaction active state is
 * set, we defer handling incoming packets.
 */

static struct {
	bool active; /* transaction status - active or not */
	int recv_len; /* number of bytes received. */
	struct hv_fcopy_hdr  *fcopy_msg; /* current message */
	struct hv_start_fcopy  message; /*  sent to daemon */
	struct vmbus_channel *recv_channel; /* chn we got the request */
	u64 recv_req_id; /* request ID. */
	void *fcopy_context; /* for the channel callback */
	struct semaphore read_sema;
} fcopy_transaction;

static bool opened; /* currently device opened */

/*
 * Before we can accept copy messages from the host, we need
 * to handshake with the user level daemon. This state tracks
 * if we are in the handshake phase.
 */
static bool in_hand_shake = true;
static void fcopy_send_data(void);
static void fcopy_respond_to_host(int error);
static void fcopy_work_func(struct work_struct *dummy);
static DECLARE_DELAYED_WORK(fcopy_work, fcopy_work_func);
static u8 *recv_buffer;

static void fcopy_work_func(struct work_struct *dummy)
=======
 */

static struct {
	int state;   /* hvutil_device_state */
	int recv_len; /* number of bytes received. */
	struct hv_fcopy_hdr  *fcopy_msg; /* current message */
	struct vmbus_channel *recv_channel; /* chn we got the request */
	u64 recv_req_id; /* request ID. */
} fcopy_transaction;

static void fcopy_respond_to_host(int error);
static void fcopy_send_data(struct work_struct *dummy);
static void fcopy_timeout_func(struct work_struct *dummy);
static DECLARE_DELAYED_WORK(fcopy_timeout_work, fcopy_timeout_func);
static DECLARE_WORK(fcopy_send_work, fcopy_send_data);
static const char fcopy_devname[] = "vmbus/hv_fcopy";
static u8 *recv_buffer;
static struct hvutil_transport *hvt;
/*
 * This state maintains the version number registered by the daemon.
 */
static int dm_reg_value;

static void fcopy_poll_wrapper(void *channel)
{
	/* Transaction is finished, reset the state here to avoid races. */
	fcopy_transaction.state = HVUTIL_READY;
	hv_fcopy_onchannelcallback(channel);
}

static void fcopy_timeout_func(struct work_struct *dummy)
>>>>>>> v4.9.227
{
	/*
	 * If the timer fires, the user-mode component has not responded;
	 * process the pending transaction.
	 */
	fcopy_respond_to_host(HV_E_FAIL);
<<<<<<< HEAD
=======
	hv_poll_channel(fcopy_transaction.recv_channel, fcopy_poll_wrapper);
}

static void fcopy_register_done(void)
{
	pr_debug("FCP: userspace daemon registered\n");
	hv_poll_channel(fcopy_transaction.recv_channel, fcopy_poll_wrapper);
>>>>>>> v4.9.227
}

static int fcopy_handle_handshake(u32 version)
{
<<<<<<< HEAD
	switch (version) {
	case FCOPY_CURRENT_VERSION:
=======
	u32 our_ver = FCOPY_CURRENT_VERSION;

	switch (version) {
	case FCOPY_VERSION_0:
		/* Daemon doesn't expect us to reply */
		dm_reg_value = version;
		break;
	case FCOPY_VERSION_1:
		/* Daemon expects us to reply with our own version */
		if (hvutil_transport_send(hvt, &our_ver, sizeof(our_ver),
		    fcopy_register_done))
			return -EFAULT;
		dm_reg_value = version;
>>>>>>> v4.9.227
		break;
	default:
		/*
		 * For now we will fail the registration.
		 * If and when we have multiple versions to
		 * deal with, we will be backward compatible.
		 * We will add this code when needed.
		 */
		return -EINVAL;
	}
<<<<<<< HEAD
	pr_info("FCP: user-mode registering done. Daemon version: %d\n",
		version);
	fcopy_transaction.active = false;
	if (fcopy_transaction.fcopy_context)
		hv_fcopy_onchannelcallback(fcopy_transaction.fcopy_context);
	in_hand_shake = false;
	return 0;
}

static void fcopy_send_data(void)
{
	struct hv_start_fcopy *smsg_out = &fcopy_transaction.message;
	int operation = fcopy_transaction.fcopy_msg->operation;
	struct hv_start_fcopy *smsg_in;
=======
	pr_debug("FCP: userspace daemon ver. %d connected\n", version);
	return 0;
}

static void fcopy_send_data(struct work_struct *dummy)
{
	struct hv_start_fcopy *smsg_out = NULL;
	int operation = fcopy_transaction.fcopy_msg->operation;
	struct hv_start_fcopy *smsg_in;
	void *out_src;
	int rc, out_len;
>>>>>>> v4.9.227

	/*
	 * The  strings sent from the host are encoded in
	 * in utf16; convert it to utf8 strings.
	 * The host assures us that the utf16 strings will not exceed
	 * the max lengths specified. We will however, reserve room
	 * for the string terminating character - in the utf16s_utf8s()
	 * function we limit the size of the buffer where the converted
	 * string is placed to W_MAX_PATH -1 to guarantee
	 * that the strings can be properly terminated!
	 */

	switch (operation) {
	case START_FILE_COPY:
<<<<<<< HEAD
		memset(smsg_out, 0, sizeof(struct hv_start_fcopy));
=======
		out_len = sizeof(struct hv_start_fcopy);
		smsg_out = kzalloc(sizeof(*smsg_out), GFP_KERNEL);
		if (!smsg_out)
			return;

>>>>>>> v4.9.227
		smsg_out->hdr.operation = operation;
		smsg_in = (struct hv_start_fcopy *)fcopy_transaction.fcopy_msg;

		utf16s_to_utf8s((wchar_t *)smsg_in->file_name, W_MAX_PATH,
				UTF16_LITTLE_ENDIAN,
<<<<<<< HEAD
				(__u8 *)smsg_out->file_name, W_MAX_PATH - 1);

		utf16s_to_utf8s((wchar_t *)smsg_in->path_name, W_MAX_PATH,
				UTF16_LITTLE_ENDIAN,
				(__u8 *)smsg_out->path_name, W_MAX_PATH - 1);

		smsg_out->copy_flags = smsg_in->copy_flags;
		smsg_out->file_size = smsg_in->file_size;
		break;

	default:
		break;
	}
	up(&fcopy_transaction.read_sema);
=======
				(__u8 *)&smsg_out->file_name, W_MAX_PATH - 1);

		utf16s_to_utf8s((wchar_t *)smsg_in->path_name, W_MAX_PATH,
				UTF16_LITTLE_ENDIAN,
				(__u8 *)&smsg_out->path_name, W_MAX_PATH - 1);

		smsg_out->copy_flags = smsg_in->copy_flags;
		smsg_out->file_size = smsg_in->file_size;
		out_src = smsg_out;
		break;

	case WRITE_TO_FILE:
		out_src = fcopy_transaction.fcopy_msg;
		out_len = sizeof(struct hv_do_fcopy);
		break;
	default:
		out_src = fcopy_transaction.fcopy_msg;
		out_len = fcopy_transaction.recv_len;
		break;
	}

	fcopy_transaction.state = HVUTIL_USERSPACE_REQ;
	rc = hvutil_transport_send(hvt, out_src, out_len, NULL);
	if (rc) {
		pr_debug("FCP: failed to communicate to the daemon: %d\n", rc);
		if (cancel_delayed_work_sync(&fcopy_timeout_work)) {
			fcopy_respond_to_host(HV_E_FAIL);
			fcopy_transaction.state = HVUTIL_READY;
		}
	}
	kfree(smsg_out);

>>>>>>> v4.9.227
	return;
}

/*
 * Send a response back to the host.
 */

static void
fcopy_respond_to_host(int error)
{
	struct icmsg_hdr *icmsghdr;
	u32 buf_len;
	struct vmbus_channel *channel;
	u64 req_id;

	/*
	 * Copy the global state for completing the transaction. Note that
	 * only one transaction can be active at a time. This is guaranteed
	 * by the file copy protocol implemented by the host. Furthermore,
	 * the "transaction active" state we maintain ensures that there can
	 * only be one active transaction at a time.
	 */

	buf_len = fcopy_transaction.recv_len;
	channel = fcopy_transaction.recv_channel;
	req_id = fcopy_transaction.recv_req_id;

<<<<<<< HEAD
	fcopy_transaction.active = false;

=======
>>>>>>> v4.9.227
	icmsghdr = (struct icmsg_hdr *)
			&recv_buffer[sizeof(struct vmbuspipe_hdr)];

	if (channel->onchannel_callback == NULL)
		/*
		 * We have raced with util driver being unloaded;
		 * silently return.
		 */
		return;

	icmsghdr->status = error;
	icmsghdr->icflags = ICMSGHDRFLAG_TRANSACTION | ICMSGHDRFLAG_RESPONSE;
	vmbus_sendpacket(channel, recv_buffer, buf_len, req_id,
				VM_PKT_DATA_INBAND, 0);
}

void hv_fcopy_onchannelcallback(void *context)
{
	struct vmbus_channel *channel = context;
	u32 recvlen;
	u64 requestid;
	struct hv_fcopy_hdr *fcopy_msg;
	struct icmsg_hdr *icmsghdr;
	struct icmsg_negotiate *negop = NULL;
	int util_fw_version;
	int fcopy_srv_version;

<<<<<<< HEAD
	if (fcopy_transaction.active) {
		/*
		 * We will defer processing this callback once
		 * the current transaction is complete.
		 */
		fcopy_transaction.fcopy_context = context;
		return;
	}
=======
	if (fcopy_transaction.state > HVUTIL_READY)
		return;
>>>>>>> v4.9.227

	vmbus_recvpacket(channel, recv_buffer, PAGE_SIZE * 2, &recvlen,
			 &requestid);
	if (recvlen <= 0)
		return;

	icmsghdr = (struct icmsg_hdr *)&recv_buffer[
			sizeof(struct vmbuspipe_hdr)];
	if (icmsghdr->icmsgtype == ICMSGTYPE_NEGOTIATE) {
		util_fw_version = UTIL_FW_VERSION;
		fcopy_srv_version = WIN8_SRV_VERSION;
		vmbus_prep_negotiate_resp(icmsghdr, negop, recv_buffer,
				util_fw_version, fcopy_srv_version);
	} else {
		fcopy_msg = (struct hv_fcopy_hdr *)&recv_buffer[
				sizeof(struct vmbuspipe_hdr) +
				sizeof(struct icmsg_hdr)];

		/*
		 * Stash away this global state for completing the
		 * transaction; note transactions are serialized.
		 */

<<<<<<< HEAD
		fcopy_transaction.active = true;
		fcopy_transaction.recv_len = recvlen;
		fcopy_transaction.recv_channel = channel;
		fcopy_transaction.recv_req_id = requestid;
		fcopy_transaction.fcopy_msg = fcopy_msg;

		/*
		 * Send the information to the user-level daemon.
		 */
		schedule_delayed_work(&fcopy_work, 5*HZ);
		fcopy_send_data();
=======
		fcopy_transaction.recv_len = recvlen;
		fcopy_transaction.recv_req_id = requestid;
		fcopy_transaction.fcopy_msg = fcopy_msg;

		if (fcopy_transaction.state < HVUTIL_READY) {
			/* Userspace is not registered yet */
			fcopy_respond_to_host(HV_E_FAIL);
			return;
		}
		fcopy_transaction.state = HVUTIL_HOSTMSG_RECEIVED;

		/*
		 * Send the information to the user-level daemon.
		 */
		schedule_work(&fcopy_send_work);
		schedule_delayed_work(&fcopy_timeout_work,
				      HV_UTIL_TIMEOUT * HZ);
>>>>>>> v4.9.227
		return;
	}
	icmsghdr->icflags = ICMSGHDRFLAG_TRANSACTION | ICMSGHDRFLAG_RESPONSE;
	vmbus_sendpacket(channel, recv_buffer, recvlen, requestid,
			VM_PKT_DATA_INBAND, 0);
}

<<<<<<< HEAD
/*
 * Create a char device that can support read/write for passing
 * the payload.
 */

static ssize_t fcopy_read(struct file *file, char __user *buf,
		size_t count, loff_t *ppos)
{
	void *src;
	size_t copy_size;
	int operation;

	/*
	 * Wait until there is something to be read.
	 */
	if (down_interruptible(&fcopy_transaction.read_sema))
		return -EINTR;

	/*
	 * The channel may be rescinded and in this case, we will wakeup the
	 * the thread blocked on the semaphore and we will use the opened
	 * state to correctly handle this case.
	 */
	if (!opened)
		return -ENODEV;

	operation = fcopy_transaction.fcopy_msg->operation;

	if (operation == START_FILE_COPY) {
		src = &fcopy_transaction.message;
		copy_size = sizeof(struct hv_start_fcopy);
		if (count < copy_size)
			return 0;
	} else {
		src = fcopy_transaction.fcopy_msg;
		copy_size = sizeof(struct hv_do_fcopy);
		if (count < copy_size)
			return 0;
	}
	if (copy_to_user(buf, src, copy_size))
		return -EFAULT;

	return copy_size;
}

static ssize_t fcopy_write(struct file *file, const char __user *buf,
			size_t count, loff_t *ppos)
{
	int response = 0;

	if (count != sizeof(int))
		return -EINVAL;

	if (copy_from_user(&response, buf, sizeof(int)))
		return -EFAULT;

	if (in_hand_shake) {
		if (fcopy_handle_handshake(response))
			return -EINVAL;
		return sizeof(int);
	}
=======
/* Callback when data is received from userspace */
static int fcopy_on_msg(void *msg, int len)
{
	int *val = (int *)msg;

	if (len != sizeof(int))
		return -EINVAL;

	if (fcopy_transaction.state == HVUTIL_DEVICE_INIT)
		return fcopy_handle_handshake(*val);

	if (fcopy_transaction.state != HVUTIL_USERSPACE_REQ)
		return -EINVAL;
>>>>>>> v4.9.227

	/*
	 * Complete the transaction by forwarding the result
	 * to the host. But first, cancel the timeout.
	 */
<<<<<<< HEAD
	if (cancel_delayed_work_sync(&fcopy_work))
		fcopy_respond_to_host(response);

	return sizeof(int);
}

static int fcopy_open(struct inode *inode, struct file *f)
{
	/*
	 * The user level daemon that will open this device is
	 * really an extension of this driver. We can have only
	 * active open at a time.
	 */
	if (opened)
		return -EBUSY;

	/*
	 * The daemon is alive; setup the state.
	 */
	opened = true;
	return 0;
}

static int fcopy_release(struct inode *inode, struct file *f)
=======
	if (cancel_delayed_work_sync(&fcopy_timeout_work)) {
		fcopy_transaction.state = HVUTIL_USERSPACE_RECV;
		fcopy_respond_to_host(*val);
		hv_poll_channel(fcopy_transaction.recv_channel,
				fcopy_poll_wrapper);
	}

	return 0;
}

static void fcopy_on_reset(void)
>>>>>>> v4.9.227
{
	/*
	 * The daemon has exited; reset the state.
	 */
<<<<<<< HEAD
	in_hand_shake = true;
	opened = false;
	return 0;
}


static const struct file_operations fcopy_fops = {
	.read           = fcopy_read,
	.write          = fcopy_write,
	.release	= fcopy_release,
	.open		= fcopy_open,
};

static struct miscdevice fcopy_misc = {
	.minor          = MISC_DYNAMIC_MINOR,
	.name           = "vmbus/hv_fcopy",
	.fops           = &fcopy_fops,
};

static int fcopy_dev_init(void)
{
	return misc_register(&fcopy_misc);
}

static void fcopy_dev_deinit(void)
{

	/*
	 * The device is going away - perhaps because the
	 * host has rescinded the channel. Setup state so that
	 * user level daemon can gracefully exit if it is blocked
	 * on the read semaphore.
	 */
	opened = false;
	/*
	 * Signal the semaphore as the device is
	 * going away.
	 */
	up(&fcopy_transaction.read_sema);
	misc_deregister(&fcopy_misc);
=======
	fcopy_transaction.state = HVUTIL_DEVICE_INIT;

	if (cancel_delayed_work_sync(&fcopy_timeout_work))
		fcopy_respond_to_host(HV_E_FAIL);
>>>>>>> v4.9.227
}

int hv_fcopy_init(struct hv_util_service *srv)
{
	recv_buffer = srv->recv_buffer;
<<<<<<< HEAD
=======
	fcopy_transaction.recv_channel = srv->channel;
>>>>>>> v4.9.227

	/*
	 * When this driver loads, the user level daemon that
	 * processes the host requests may not yet be running.
	 * Defer processing channel callbacks until the daemon
	 * has registered.
	 */
<<<<<<< HEAD
	fcopy_transaction.active = true;
	sema_init(&fcopy_transaction.read_sema, 0);

	return fcopy_dev_init();
=======
	fcopy_transaction.state = HVUTIL_DEVICE_INIT;

	hvt = hvutil_transport_init(fcopy_devname, 0, 0,
				    fcopy_on_msg, fcopy_on_reset);
	if (!hvt)
		return -EFAULT;

	return 0;
>>>>>>> v4.9.227
}

void hv_fcopy_deinit(void)
{
<<<<<<< HEAD
	cancel_delayed_work_sync(&fcopy_work);
	fcopy_dev_deinit();
=======
	fcopy_transaction.state = HVUTIL_DEVICE_DYING;
	cancel_delayed_work_sync(&fcopy_timeout_work);
	hvutil_transport_destroy(hvt);
>>>>>>> v4.9.227
}
