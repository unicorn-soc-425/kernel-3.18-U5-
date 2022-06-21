/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
<<<<<<< HEAD
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
=======
>>>>>>> v4.9.227
 ******************************************************************************/
/*-----------------------------------------------------------------------------

	For type defines and data structure defines

------------------------------------------------------------------------------*/


#ifndef __DRV_TYPES_H__
#define __DRV_TYPES_H__

#define DRV_NAME "r8188eu"

#include <osdep_service.h>
#include <wlan_bssdef.h>
#include <rtw_ht.h>
#include <rtw_cmd.h>
#include <rtw_xmit.h>
#include <rtw_recv.h>
#include <hal_intf.h>
#include <hal_com.h>
#include <rtw_qos.h>
#include <rtw_security.h>
#include <rtw_pwrctrl.h>
#include <rtw_eeprom.h>
#include <sta_info.h>
#include <rtw_mlme.h>
#include <rtw_debug.h>
#include <rtw_rf.h>
#include <rtw_event.h>
#include <rtw_led.h>
#include <rtw_mlme_ext.h>
#include <rtw_ap.h>

#define SPEC_DEV_ID_NONE		BIT(0)
#define SPEC_DEV_ID_DISABLE_HT		BIT(1)
#define SPEC_DEV_ID_ENABLE_PS		BIT(2)
#define SPEC_DEV_ID_RF_CONFIG_1T1R	BIT(3)
#define SPEC_DEV_ID_RF_CONFIG_2T2R	BIT(4)
#define SPEC_DEV_ID_ASSIGN_IFNAME	BIT(5)

struct registry_priv {
<<<<<<< HEAD
	u8	chip_version;
	u8	rfintfs;
	u8	lbkmode;
	u8	hci;
	struct ndis_802_11_ssid	ssid;
	u8	network_mode;	/* infra, ad-hoc, auto */
	u8	channel;/* ad-hoc support requirement */
	u8	wireless_mode;/* A, B, G, auto */
	u8	scan_mode;/* active, passive */
	u8	radio_enable;
=======
	struct ndis_802_11_ssid	ssid;
	u8	channel;/* ad-hoc support requirement */
	u8	wireless_mode;/* A, B, G, auto */
>>>>>>> v4.9.227
	u8	preamble;/* long, short, auto */
	u8	vrtl_carrier_sense;/* Enable, Disable, Auto */
	u8	vcs_type;/* RTS/CTS, CTS-to-self */
	u16	rts_thresh;
	u16	frag_thresh;
<<<<<<< HEAD
	u8	adhoc_tx_pwr;
	u8	soft_ap;
	u8	power_mgnt;
	u8	ips_mode;
	u8	smart_ps;
	u8	long_retry_lmt;
	u8	short_retry_lmt;
	u16	busy_thresh;
	u8	ack_policy;
=======
	u8	power_mgnt;
	u8	ips_mode;
	u8	smart_ps;
>>>>>>> v4.9.227
	u8	mp_mode;
	u8	software_encrypt;
	u8	software_decrypt;
	u8	acm_method;
	  /* UAPSD */
	u8	wmm_enable;
	u8	uapsd_enable;
<<<<<<< HEAD
	u8	uapsd_max_sp;
	u8	uapsd_acbk_en;
	u8	uapsd_acbe_en;
	u8	uapsd_acvi_en;
	u8	uapsd_acvo_en;
=======
>>>>>>> v4.9.227

	struct wlan_bssid_ex    dev_network;

	u8	ht_enable;
	u8	cbw40_enable;
	u8	ampdu_enable;/* for tx */
	u8	rx_stbc;
	u8	ampdu_amsdu;/* A-MPDU Supports A-MSDU is permitted */
<<<<<<< HEAD
	u8	lowrate_two_xmit;

	u8	rf_config;
	u8	low_power;
=======
>>>>>>> v4.9.227

	u8	wifi_spec;/*  !turbo_mode */

	u8	channel_plan;
<<<<<<< HEAD
	bool	bAcceptAddbaReq;
=======
	bool	accept_addba_req; /* true = accept AP's Add BA req */
>>>>>>> v4.9.227

	u8	antdiv_cfg;
	u8	antdiv_type;

	u8	usbss_enable;/* 0:disable,1:enable */
	u8	hwpdn_mode;/* 0:disable,1:enable,2:decide by EFUSE config */
<<<<<<< HEAD
	u8	hwpwrp_detect;/* 0:disable,1:enable */

	u8	hw_wps_pbc;/* 0:disable,1:enable */
=======
>>>>>>> v4.9.227

	u8	max_roaming_times; /*  the max number driver will try */

	u8	fw_iol; /* enable iol without other concern */

	u8	enable80211d;

	u8	ifname[16];
	u8	if2name[16];

	u8	notch_filter;
<<<<<<< HEAD
};

/* For registry parameters */
#define RGTRY_OFT(field) ((u32)FIELD_OFFSET(struct registry_priv, field))
#define RGTRY_SZ(field)   sizeof(((struct registry_priv *)0)->field)
#define BSSID_OFT(field) ((u32)FIELD_OFFSET(struct wlan_bssid_ex, field))
#define BSSID_SZ(field)   sizeof(((struct wlan_bssid_ex *)0)->field)

=======
	bool	monitor_enable;
};

>>>>>>> v4.9.227
#define MAX_CONTINUAL_URB_ERR		4

struct dvobj_priv {
	struct adapter *if1;
	/* For 92D, DMDP have 2 interface. */
	u8	InterfaceNumber;
	u8	NumInterfaces;

	/* In /Out Pipe information */
	int	RtInPipe[2];
	int	RtOutPipe[3];
	u8	Queue2Pipe[HW_QUEUE_ENTRY];/* for out pipe mapping */

/*-------- below is for USB INTERFACE --------*/
<<<<<<< HEAD

	u8	nr_endpoint;
	u8	ishighspeed;
	u8	RtNumInPipes;
	u8	RtNumOutPipes;
	int	ep_num[5]; /* endpoint number */
	struct mutex  usb_vendor_req_mutex;

	u8 *usb_vendor_req_buf;

=======
	u8	ishighspeed;
	u8	RtNumInPipes;
	u8	RtNumOutPipes;
	struct mutex  usb_vendor_req_mutex;

>>>>>>> v4.9.227
	struct usb_interface *pusbintf;
	struct usb_device *pusbdev;
};

static inline struct device *dvobj_to_dev(struct dvobj_priv *dvobj)
{
	/* todo: get interface type from dvobj and the return
	 * the dev accordingly */
	return &dvobj->pusbintf->dev;
};

struct adapter {
<<<<<<< HEAD
	int	pid[3];/* process id from UI, 0:wps, 1:hostapd, 2:dhcpcd */
	u16	chip_type;

=======
>>>>>>> v4.9.227
	struct dvobj_priv *dvobj;
	struct	mlme_priv mlmepriv;
	struct	mlme_ext_priv mlmeextpriv;
	struct	cmd_priv	cmdpriv;
	struct	xmit_priv	xmitpriv;
	struct	recv_priv	recvpriv;
	struct	sta_priv	stapriv;
	struct	security_priv	securitypriv;
	struct	registry_priv	registrypriv;
	struct	pwrctrl_priv	pwrctrlpriv;
	struct	eeprom_priv eeprompriv;
	struct	led_priv	ledpriv;

<<<<<<< HEAD
#ifdef CONFIG_88EU_AP_MODE
	struct	hostapd_priv	*phostapdpriv;
#endif

	struct wifidirect_info	wdinfo;

	void *HalData;
	struct hal_ops	HalFunc;
=======
	struct hal_data_8188e *HalData;
>>>>>>> v4.9.227

	s32	bDriverStopped;
	s32	bSurpriseRemoved;

	u8	hw_init_completed;

	void *cmdThread;
<<<<<<< HEAD
	void *evtThread;
	void (*intf_start)(struct adapter *adapter);
	void (*intf_stop)(struct adapter *adapter);
	struct  net_device *pnetdev;

	/*  used by rtw_rereg_nd_name related function */
	struct rereg_nd_name_data {
		struct  net_device *old_pnetdev;
		char old_ifname[IFNAMSIZ];
		u8 old_ips_mode;
		u8 old_bRegUseLed;
	} rereg_nd_name_priv;
=======
	void (*intf_start)(struct adapter *adapter);
	void (*intf_stop)(struct adapter *adapter);
	struct  net_device *pnetdev;
	struct  net_device *pmondev;
>>>>>>> v4.9.227

	int bup;
	struct net_device_stats stats;
	struct iw_statistics iwstats;
	struct proc_dir_entry *dir_dev;/*  for proc directory */

	int net_closed;
	u8 bFWReady;
	u8 bReadPortCancel;
	u8 bWritePortCancel;
	u8 bRxRSSIDisplay;
<<<<<<< HEAD
	/* The driver will show up the desired channel number
	 * when this flag is 1. */
	u8 bNotifyChannelChange;

	struct mutex hw_init_mutex;

	spinlock_t br_ext_lock;

	u8	fix_rate;

	unsigned char     in_cta_test;
=======

	struct mutex hw_init_mutex;
>>>>>>> v4.9.227
};

#define adapter_to_dvobj(adapter) (adapter->dvobj)

<<<<<<< HEAD
int rtw_handle_dualmac(struct adapter *adapter, bool init);

=======
>>>>>>> v4.9.227
static inline u8 *myid(struct eeprom_priv *peepriv)
{
	return peepriv->mac_addr;
}

#endif /* __DRV_TYPES_H__ */
