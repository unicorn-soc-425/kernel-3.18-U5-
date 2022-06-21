/*
 * CAN driver for PEAK System PCAN-USB Pro adapter
 * Derived from the PCAN project file driver/src/pcan_usbpro_fw.h
 *
 * Copyright (C) 2003-2011 PEAK System-Technik GmbH
 * Copyright (C) 2011-2012 Stephane Grosjean <s.grosjean@peak-system.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */
#ifndef PCAN_USB_PRO_H
#define PCAN_USB_PRO_H

/*
 * USB Vendor request data types
 */
#define PCAN_USBPRO_REQ_INFO		0
#define PCAN_USBPRO_REQ_FCT		2

/* Vendor Request value for XXX_INFO */
#define PCAN_USBPRO_INFO_BL		0
#define PCAN_USBPRO_INFO_FW		1

<<<<<<< HEAD
=======
/* PCAN-USB Pro (FD) Endpoints */
#define PCAN_USBPRO_EP_CMDOUT		1
#define PCAN_USBPRO_EP_CMDIN		(PCAN_USBPRO_EP_CMDOUT | USB_DIR_IN)
#define PCAN_USBPRO_EP_MSGOUT_0		2
#define PCAN_USBPRO_EP_MSGIN		(PCAN_USBPRO_EP_MSGOUT_0 | USB_DIR_IN)
#define PCAN_USBPRO_EP_MSGOUT_1		3
#define PCAN_USBPRO_EP_UNUSED		(PCAN_USBPRO_EP_MSGOUT_1 | USB_DIR_IN)

>>>>>>> v4.9.227
/* Vendor Request value for XXX_FCT */
#define PCAN_USBPRO_FCT_DRVLD		5 /* tell device driver is loaded */
#define PCAN_USBPRO_FCT_DRVLD_REQ_LEN	16

/* PCAN_USBPRO_INFO_BL vendor request record type */
struct __packed pcan_usb_pro_blinfo {
<<<<<<< HEAD
	u32 ctrl_type;
=======
	__le32 ctrl_type;
>>>>>>> v4.9.227
	u8  version[4];
	u8  day;
	u8  month;
	u8  year;
	u8  dummy;
<<<<<<< HEAD
	u32 serial_num_hi;
	u32 serial_num_lo;
	u32 hw_type;
	u32 hw_rev;
=======
	__le32 serial_num_hi;
	__le32 serial_num_lo;
	__le32 hw_type;
	__le32 hw_rev;
>>>>>>> v4.9.227
};

/* PCAN_USBPRO_INFO_FW vendor request record type */
struct __packed pcan_usb_pro_fwinfo {
<<<<<<< HEAD
	u32 ctrl_type;
=======
	__le32 ctrl_type;
>>>>>>> v4.9.227
	u8  version[4];
	u8  day;
	u8  month;
	u8  year;
	u8  dummy;
<<<<<<< HEAD
	u32 fw_type;
=======
	__le32 fw_type;
>>>>>>> v4.9.227
};

/*
 * USB Command record types
 */
#define PCAN_USBPRO_SETBTR	0x02
#define PCAN_USBPRO_SETBUSACT	0x04
#define PCAN_USBPRO_SETSILENT	0x05
#define PCAN_USBPRO_SETFILTR	0x0a
#define PCAN_USBPRO_SETTS	0x10
#define PCAN_USBPRO_GETDEVID	0x12
#define PCAN_USBPRO_SETLED	0x1C
#define PCAN_USBPRO_RXMSG8	0x80
#define PCAN_USBPRO_RXMSG4	0x81
#define PCAN_USBPRO_RXMSG0	0x82
#define PCAN_USBPRO_RXRTR	0x83
#define PCAN_USBPRO_RXSTATUS	0x84
#define PCAN_USBPRO_RXTS	0x85
#define PCAN_USBPRO_TXMSG8	0x41
#define PCAN_USBPRO_TXMSG4	0x42
#define PCAN_USBPRO_TXMSG0	0x43

/* record structures */
struct __packed pcan_usb_pro_btr {
	u8  data_type;
	u8  channel;
<<<<<<< HEAD
	u16 dummy;
	u32 CCBT;
=======
	__le16 dummy;
	__le32 CCBT;
>>>>>>> v4.9.227
};

struct __packed pcan_usb_pro_busact {
	u8  data_type;
	u8  channel;
<<<<<<< HEAD
	u16 onoff;
=======
	__le16 onoff;
>>>>>>> v4.9.227
};

struct __packed pcan_usb_pro_silent {
	u8  data_type;
	u8  channel;
<<<<<<< HEAD
	u16 onoff;
=======
	__le16 onoff;
>>>>>>> v4.9.227
};

struct __packed pcan_usb_pro_filter {
	u8  data_type;
	u8  dummy;
<<<<<<< HEAD
	u16 filter_mode;
=======
	__le16 filter_mode;
>>>>>>> v4.9.227
};

struct __packed pcan_usb_pro_setts {
	u8  data_type;
	u8  dummy;
<<<<<<< HEAD
	u16 mode;
=======
	__le16 mode;
>>>>>>> v4.9.227
};

struct __packed pcan_usb_pro_devid {
	u8  data_type;
	u8  channel;
<<<<<<< HEAD
	u16 dummy;
	u32 serial_num;
=======
	__le16 dummy;
	__le32 serial_num;
>>>>>>> v4.9.227
};

struct __packed pcan_usb_pro_setled {
	u8  data_type;
	u8  channel;
<<<<<<< HEAD
	u16 mode;
	u32 timeout;
=======
	__le16 mode;
	__le32 timeout;
>>>>>>> v4.9.227
};

struct __packed pcan_usb_pro_rxmsg {
	u8  data_type;
	u8  client;
	u8  flags;
	u8  len;
<<<<<<< HEAD
	u32 ts32;
	u32 id;
=======
	__le32 ts32;
	__le32 id;
>>>>>>> v4.9.227

	u8  data[8];
};

#define PCAN_USBPRO_STATUS_ERROR	0x0001
#define PCAN_USBPRO_STATUS_BUS		0x0002
#define PCAN_USBPRO_STATUS_OVERRUN	0x0004
#define PCAN_USBPRO_STATUS_QOVERRUN	0x0008

struct __packed pcan_usb_pro_rxstatus {
	u8  data_type;
	u8  channel;
<<<<<<< HEAD
	u16 status;
	u32 ts32;
	u32 err_frm;
=======
	__le16 status;
	__le32 ts32;
	__le32 err_frm;
>>>>>>> v4.9.227
};

struct __packed pcan_usb_pro_rxts {
	u8  data_type;
	u8  dummy[3];
<<<<<<< HEAD
	u32 ts64[2];
=======
	__le32 ts64[2];
>>>>>>> v4.9.227
};

struct __packed pcan_usb_pro_txmsg {
	u8  data_type;
	u8  client;
	u8  flags;
	u8  len;
<<<<<<< HEAD
	u32 id;
=======
	__le32 id;
>>>>>>> v4.9.227
	u8  data[8];
};

union pcan_usb_pro_rec {
	u8				data_type;
	struct pcan_usb_pro_btr		btr;
	struct pcan_usb_pro_busact	bus_act;
	struct pcan_usb_pro_silent	silent_mode;
	struct pcan_usb_pro_filter	filter_mode;
	struct pcan_usb_pro_setts	ts;
	struct pcan_usb_pro_devid	dev_id;
	struct pcan_usb_pro_setled	set_led;
	struct pcan_usb_pro_rxmsg	rx_msg;
	struct pcan_usb_pro_rxstatus	rx_status;
	struct pcan_usb_pro_rxts	rx_ts;
	struct pcan_usb_pro_txmsg	tx_msg;
};

<<<<<<< HEAD
=======
int pcan_usb_pro_probe(struct usb_interface *intf);
int pcan_usb_pro_send_req(struct peak_usb_device *dev, int req_id,
			  int req_value, void *req_addr, int req_size);
void pcan_usb_pro_restart_complete(struct urb *urb);

>>>>>>> v4.9.227
#endif
