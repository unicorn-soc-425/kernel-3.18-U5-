/*
 * Copyright 2002-2005, Devicescape Software, Inc.
 * Copyright 2013-2014  Intel Mobile Communications GmbH
<<<<<<< HEAD
=======
 * Copyright(c) 2015-2016 Intel Deutschland GmbH
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef STA_INFO_H
#define STA_INFO_H

#include <linux/list.h>
#include <linux/types.h>
#include <linux/if_ether.h>
#include <linux/workqueue.h>
#include <linux/average.h>
#include <linux/etherdevice.h>
<<<<<<< HEAD
=======
#include <linux/rhashtable.h>
#include <linux/u64_stats_sync.h>
>>>>>>> v4.9.227
#include "key.h"

/**
 * enum ieee80211_sta_info_flags - Stations flags
 *
 * These flags are used with &struct sta_info's @flags member, but
 * only indirectly with set_sta_flag() and friends.
 *
 * @WLAN_STA_AUTH: Station is authenticated.
 * @WLAN_STA_ASSOC: Station is associated.
 * @WLAN_STA_PS_STA: Station is in power-save mode
 * @WLAN_STA_AUTHORIZED: Station is authorized to send/receive traffic.
 *	This bit is always checked so needs to be enabled for all stations
 *	when virtual port control is not in use.
 * @WLAN_STA_SHORT_PREAMBLE: Station is capable of receiving short-preamble
 *	frames.
 * @WLAN_STA_WDS: Station is one of our WDS peers.
 * @WLAN_STA_CLEAR_PS_FILT: Clear PS filter in hardware (using the
 *	IEEE80211_TX_CTL_CLEAR_PS_FILT control flag) when the next
 *	frame to this station is transmitted.
 * @WLAN_STA_MFP: Management frame protection is used with this STA.
 * @WLAN_STA_BLOCK_BA: Used to deny ADDBA requests (both TX and RX)
 *	during suspend/resume and station removal.
 * @WLAN_STA_PS_DRIVER: driver requires keeping this station in
 *	power-save mode logically to flush frames that might still
 *	be in the queues
 * @WLAN_STA_PSPOLL: Station sent PS-poll while driver was keeping
 *	station in power-save mode, reply when the driver unblocks.
 * @WLAN_STA_TDLS_PEER: Station is a TDLS peer.
 * @WLAN_STA_TDLS_PEER_AUTH: This TDLS peer is authorized to send direct
 *	packets. This means the link is enabled.
 * @WLAN_STA_TDLS_INITIATOR: We are the initiator of the TDLS link with this
 *	station.
<<<<<<< HEAD
=======
 * @WLAN_STA_TDLS_CHAN_SWITCH: This TDLS peer supports TDLS channel-switching
 * @WLAN_STA_TDLS_OFF_CHANNEL: The local STA is currently off-channel with this
 *	TDLS peer
 * @WLAN_STA_TDLS_WIDER_BW: This TDLS peer supports working on a wider bw on
 *	the BSS base channel.
>>>>>>> v4.9.227
 * @WLAN_STA_UAPSD: Station requested unscheduled SP while driver was
 *	keeping station in power-save mode, reply when the driver
 *	unblocks the station.
 * @WLAN_STA_SP: Station is in a service period, so don't try to
 *	reply to other uAPSD trigger frames or PS-Poll.
 * @WLAN_STA_4ADDR_EVENT: 4-addr event was already sent for this frame.
 * @WLAN_STA_INSERTED: This station is inserted into the hash table.
 * @WLAN_STA_RATE_CONTROL: rate control was initialized for this station.
 * @WLAN_STA_TOFFSET_KNOWN: toffset calculated for this station is valid.
 * @WLAN_STA_MPSP_OWNER: local STA is owner of a mesh Peer Service Period.
 * @WLAN_STA_MPSP_RECIPIENT: local STA is recipient of a MPSP.
 * @WLAN_STA_PS_DELIVER: station woke up, but we're still blocking TX
 *	until pending frames are delivered
<<<<<<< HEAD
=======
 *
 * @NUM_WLAN_STA_FLAGS: number of defined flags
>>>>>>> v4.9.227
 */
enum ieee80211_sta_info_flags {
	WLAN_STA_AUTH,
	WLAN_STA_ASSOC,
	WLAN_STA_PS_STA,
	WLAN_STA_AUTHORIZED,
	WLAN_STA_SHORT_PREAMBLE,
	WLAN_STA_WDS,
	WLAN_STA_CLEAR_PS_FILT,
	WLAN_STA_MFP,
	WLAN_STA_BLOCK_BA,
	WLAN_STA_PS_DRIVER,
	WLAN_STA_PSPOLL,
	WLAN_STA_TDLS_PEER,
	WLAN_STA_TDLS_PEER_AUTH,
	WLAN_STA_TDLS_INITIATOR,
<<<<<<< HEAD
=======
	WLAN_STA_TDLS_CHAN_SWITCH,
	WLAN_STA_TDLS_OFF_CHANNEL,
	WLAN_STA_TDLS_WIDER_BW,
>>>>>>> v4.9.227
	WLAN_STA_UAPSD,
	WLAN_STA_SP,
	WLAN_STA_4ADDR_EVENT,
	WLAN_STA_INSERTED,
	WLAN_STA_RATE_CONTROL,
	WLAN_STA_TOFFSET_KNOWN,
	WLAN_STA_MPSP_OWNER,
	WLAN_STA_MPSP_RECIPIENT,
	WLAN_STA_PS_DELIVER,
<<<<<<< HEAD
=======

	NUM_WLAN_STA_FLAGS,
>>>>>>> v4.9.227
};

#define ADDBA_RESP_INTERVAL HZ
#define HT_AGG_MAX_RETRIES		15
#define HT_AGG_BURST_RETRIES		3
#define HT_AGG_RETRIES_PERIOD		(15 * HZ)

#define HT_AGG_STATE_DRV_READY		0
#define HT_AGG_STATE_RESPONSE_RECEIVED	1
#define HT_AGG_STATE_OPERATIONAL	2
#define HT_AGG_STATE_STOPPING		3
#define HT_AGG_STATE_WANT_START		4
#define HT_AGG_STATE_WANT_STOP		5
<<<<<<< HEAD
=======
#define HT_AGG_STATE_START_CB		6
#define HT_AGG_STATE_STOP_CB		7
>>>>>>> v4.9.227

enum ieee80211_agg_stop_reason {
	AGG_STOP_DECLINED,
	AGG_STOP_LOCAL_REQUEST,
	AGG_STOP_PEER_REQUEST,
	AGG_STOP_DESTROY_STA,
};

/**
 * struct tid_ampdu_tx - TID aggregation information (Tx).
 *
 * @rcu_head: rcu head for freeing structure
 * @session_timer: check if we keep Tx-ing on the TID (by timeout value)
 * @addba_resp_timer: timer for peer's response to addba request
 * @pending: pending frames queue -- use sta's spinlock to protect
 * @dialog_token: dialog token for aggregation session
 * @timeout: session timeout value to be filled in ADDBA requests
 * @state: session state (see above)
 * @last_tx: jiffies of last tx activity
 * @stop_initiator: initiator of a session stop
 * @tx_stop: TX DelBA frame when stopping
 * @buf_size: reorder buffer size at receiver
 * @failed_bar_ssn: ssn of the last failed BAR tx attempt
 * @bar_pending: BAR needs to be re-sent
<<<<<<< HEAD
=======
 * @amsdu: support A-MSDU withing A-MDPU
>>>>>>> v4.9.227
 *
 * This structure's lifetime is managed by RCU, assignments to
 * the array holding it must hold the aggregation mutex.
 *
 * The TX path can access it under RCU lock-free if, and
 * only if, the state has the flag %HT_AGG_STATE_OPERATIONAL
 * set. Otherwise, the TX path must also acquire the spinlock
 * and re-check the state, see comments in the tx code
 * touching it.
 */
struct tid_ampdu_tx {
	struct rcu_head rcu_head;
	struct timer_list session_timer;
	struct timer_list addba_resp_timer;
	struct sk_buff_head pending;
	unsigned long state;
	unsigned long last_tx;
	u16 timeout;
	u8 dialog_token;
	u8 stop_initiator;
	bool tx_stop;
	u8 buf_size;

	u16 failed_bar_ssn;
	bool bar_pending;
<<<<<<< HEAD
=======
	bool amsdu;
>>>>>>> v4.9.227
};

/**
 * struct tid_ampdu_rx - TID aggregation information (Rx).
 *
 * @reorder_buf: buffer to reorder incoming aggregated MPDUs. An MPDU may be an
 *	A-MSDU with individually reported subframes.
<<<<<<< HEAD
=======
 * @reorder_buf_filtered: bitmap indicating where there are filtered frames in
 *	the reorder buffer that should be ignored when releasing frames
>>>>>>> v4.9.227
 * @reorder_time: jiffies when skb was added
 * @session_timer: check if peer keeps Tx-ing on the TID (by timeout value)
 * @reorder_timer: releases expired frames from the reorder buffer.
 * @last_rx: jiffies of last rx activity
 * @head_seq_num: head sequence number in reordering buffer.
 * @stored_mpdu_num: number of MPDUs in reordering buffer
 * @ssn: Starting Sequence Number expected to be aggregated.
 * @buf_size: buffer size for incoming A-MPDUs
 * @timeout: reset timer value (in TUs).
<<<<<<< HEAD
 * @dialog_token: dialog token for aggregation session
=======
>>>>>>> v4.9.227
 * @rcu_head: RCU head used for freeing this struct
 * @reorder_lock: serializes access to reorder buffer, see below.
 * @auto_seq: used for offloaded BA sessions to automatically pick head_seq_and
 *	and ssn.
 * @removed: this session is removed (but might have been found due to RCU)
<<<<<<< HEAD
=======
 * @started: this session has started (head ssn or higher was received)
>>>>>>> v4.9.227
 *
 * This structure's lifetime is managed by RCU, assignments to
 * the array holding it must hold the aggregation mutex.
 *
 * The @reorder_lock is used to protect the members of this
 * struct, except for @timeout, @buf_size and @dialog_token,
 * which are constant across the lifetime of the struct (the
 * dialog token being used only for debugging).
 */
struct tid_ampdu_rx {
	struct rcu_head rcu_head;
	spinlock_t reorder_lock;
<<<<<<< HEAD
=======
	u64 reorder_buf_filtered;
>>>>>>> v4.9.227
	struct sk_buff_head *reorder_buf;
	unsigned long *reorder_time;
	struct timer_list session_timer;
	struct timer_list reorder_timer;
	unsigned long last_rx;
	u16 head_seq_num;
	u16 stored_mpdu_num;
	u16 ssn;
	u16 buf_size;
	u16 timeout;
<<<<<<< HEAD
	u8 dialog_token;
	bool auto_seq;
	bool removed;
=======
	u8 auto_seq:1,
	   removed:1,
	   started:1;
>>>>>>> v4.9.227
};

/**
 * struct sta_ampdu_mlme - STA aggregation information.
 *
<<<<<<< HEAD
 * @tid_rx: aggregation info for Rx per TID -- RCU protected
 * @tid_tx: aggregation info for Tx per TID
 * @tid_start_tx: sessions where start was requested
 * @addba_req_num: number of times addBA request has been sent.
 * @last_addba_req_time: timestamp of the last addBA request.
 * @dialog_token_allocator: dialog token enumerator for each new session;
 * @work: work struct for starting/stopping aggregation
=======
 * @mtx: mutex to protect all TX data (except non-NULL assignments
 *	to tid_tx[idx], which are protected by the sta spinlock)
 *	tid_start_tx is also protected by sta->lock.
 * @tid_rx: aggregation info for Rx per TID -- RCU protected
 * @tid_rx_token: dialog tokens for valid aggregation sessions
>>>>>>> v4.9.227
 * @tid_rx_timer_expired: bitmap indicating on which TIDs the
 *	RX timer expired until the work for it runs
 * @tid_rx_stop_requested:  bitmap indicating which BA sessions per TID the
 *	driver requested to close until the work for it runs
<<<<<<< HEAD
 * @mtx: mutex to protect all TX data (except non-NULL assignments
 *	to tid_tx[idx], which are protected by the sta spinlock)
 *	tid_start_tx is also protected by sta->lock.
=======
 * @agg_session_valid: bitmap indicating which TID has a rx BA session open on
 * @unexpected_agg: bitmap indicating which TID already sent a delBA due to
 *	unexpected aggregation related frames outside a session
 * @work: work struct for starting/stopping aggregation
 * @tid_tx: aggregation info for Tx per TID
 * @tid_start_tx: sessions where start was requested
 * @last_addba_req_time: timestamp of the last addBA request.
 * @addba_req_num: number of times addBA request has been sent.
 * @dialog_token_allocator: dialog token enumerator for each new session;
>>>>>>> v4.9.227
 */
struct sta_ampdu_mlme {
	struct mutex mtx;
	/* rx */
	struct tid_ampdu_rx __rcu *tid_rx[IEEE80211_NUM_TIDS];
<<<<<<< HEAD
	unsigned long tid_rx_timer_expired[BITS_TO_LONGS(IEEE80211_NUM_TIDS)];
	unsigned long tid_rx_stop_requested[BITS_TO_LONGS(IEEE80211_NUM_TIDS)];
=======
	u8 tid_rx_token[IEEE80211_NUM_TIDS];
	unsigned long tid_rx_timer_expired[BITS_TO_LONGS(IEEE80211_NUM_TIDS)];
	unsigned long tid_rx_stop_requested[BITS_TO_LONGS(IEEE80211_NUM_TIDS)];
	unsigned long agg_session_valid[BITS_TO_LONGS(IEEE80211_NUM_TIDS)];
	unsigned long unexpected_agg[BITS_TO_LONGS(IEEE80211_NUM_TIDS)];
>>>>>>> v4.9.227
	/* tx */
	struct work_struct work;
	struct tid_ampdu_tx __rcu *tid_tx[IEEE80211_NUM_TIDS];
	struct tid_ampdu_tx *tid_start_tx[IEEE80211_NUM_TIDS];
	unsigned long last_addba_req_time[IEEE80211_NUM_TIDS];
	u8 addba_req_num[IEEE80211_NUM_TIDS];
	u8 dialog_token_allocator;
};

<<<<<<< HEAD
/*
 * struct ieee80211_tx_latency_stat - Tx latency statistics
 *
 * Measures TX latency and jitter for a station per TID.
 *
 * @max: worst case latency
 * @sum: sum of all latencies
 * @counter: amount of Tx frames sent from interface
 * @bins: each bin counts how many frames transmitted within a certain
 * latency range. when disabled it is NULL.
 * @bin_count: amount of bins.
 */
struct ieee80211_tx_latency_stat {
	u32 max;
	u32 sum;
	u32 counter;
	u32 *bins;
	u32 bin_count;
=======

/* Value to indicate no TID reservation */
#define IEEE80211_TID_UNRESERVED	0xff

#define IEEE80211_FAST_XMIT_MAX_IV	18

/**
 * struct ieee80211_fast_tx - TX fastpath information
 * @key: key to use for hw crypto
 * @hdr: the 802.11 header to put with the frame
 * @hdr_len: actual 802.11 header length
 * @sa_offs: offset of the SA
 * @da_offs: offset of the DA
 * @pn_offs: offset where to put PN for crypto (or 0 if not needed)
 * @band: band this will be transmitted on, for tx_info
 * @rcu_head: RCU head to free this struct
 *
 * This struct is small enough so that the common case (maximum crypto
 * header length of 8 like for CCMP/GCMP) fits into a single 64-byte
 * cache line.
 */
struct ieee80211_fast_tx {
	struct ieee80211_key *key;
	u8 hdr_len;
	u8 sa_offs, da_offs, pn_offs;
	u8 band;
	u8 hdr[30 + 2 + IEEE80211_FAST_XMIT_MAX_IV +
	       sizeof(rfc1042_header)] __aligned(2);

	struct rcu_head rcu_head;
};

/**
 * struct ieee80211_fast_rx - RX fastpath information
 * @dev: netdevice for reporting the SKB
 * @vif_type: (P2P-less) interface type of the original sdata (sdata->vif.type)
 * @vif_addr: interface address
 * @rfc1042_hdr: copy of the RFC 1042 SNAP header (to have in cache)
 * @control_port_protocol: control port protocol copied from sdata
 * @expected_ds_bits: from/to DS bits expected
 * @icv_len: length of the MIC if present
 * @key: bool indicating encryption is expected (key is set)
 * @sta_notify: notify the MLME code (once)
 * @internal_forward: forward froms internally on AP/VLAN type interfaces
 * @uses_rss: copy of USES_RSS hw flag
 * @da_offs: offset of the DA in the header (for header conversion)
 * @sa_offs: offset of the SA in the header (for header conversion)
 * @rcu_head: RCU head for freeing this structure
 */
struct ieee80211_fast_rx {
	struct net_device *dev;
	enum nl80211_iftype vif_type;
	u8 vif_addr[ETH_ALEN] __aligned(2);
	u8 rfc1042_hdr[6] __aligned(2);
	__be16 control_port_protocol;
	__le16 expected_ds_bits;
	u8 icv_len;
	u8 key:1,
	   sta_notify:1,
	   internal_forward:1,
	   uses_rss:1;
	u8 da_offs, sa_offs;

	struct rcu_head rcu_head;
};

/**
 * struct mesh_sta - mesh STA information
 * @plink_lock: serialize access to plink fields
 * @llid: Local link ID
 * @plid: Peer link ID
 * @aid: local aid supplied by peer
 * @reason: Cancel reason on PLINK_HOLDING state
 * @plink_retries: Retries in establishment
 * @plink_state: peer link state
 * @plink_timeout: timeout of peer link
 * @plink_timer: peer link watch timer
 * @t_offset: timing offset relative to this host
 * @t_offset_setpoint: reference timing offset of this sta to be used when
 * 	calculating clockdrift
 * @local_pm: local link-specific power save mode
 * @peer_pm: peer-specific power save mode towards local STA
 * @nonpeer_pm: STA power save mode towards non-peer neighbors
 * @processed_beacon: set to true after peer rates and capabilities are
 *	processed
 * @fail_avg: moving percentage of failed MSDUs
 */
struct mesh_sta {
	struct timer_list plink_timer;

	s64 t_offset;
	s64 t_offset_setpoint;

	spinlock_t plink_lock;
	u16 llid;
	u16 plid;
	u16 aid;
	u16 reason;
	u8 plink_retries;

	bool processed_beacon;

	enum nl80211_plink_state plink_state;
	u32 plink_timeout;

	/* mesh power save */
	enum nl80211_mesh_power_mode local_pm;
	enum nl80211_mesh_power_mode peer_pm;
	enum nl80211_mesh_power_mode nonpeer_pm;

	/* moving percentage of failed MSDUs */
	unsigned int fail_avg;
};

DECLARE_EWMA(signal, 1024, 8)

struct ieee80211_sta_rx_stats {
	unsigned long packets;
	unsigned long last_rx;
	unsigned long num_duplicates;
	unsigned long fragments;
	unsigned long dropped;
	int last_signal;
	u8 chains;
	s8 chain_signal_last[IEEE80211_MAX_CHAINS];
	u16 last_rate;
	struct u64_stats_sync syncp;
	u64 bytes;
	u64 msdu[IEEE80211_NUM_TIDS + 1];
>>>>>>> v4.9.227
};

/**
 * struct sta_info - STA information
 *
 * This structure collects information about a station that
 * mac80211 is communicating with.
 *
 * @list: global linked list entry
 * @free_list: list entry for keeping track of stations to free
<<<<<<< HEAD
 * @hnext: hash table linked list pointer
=======
 * @hash_node: hash node for rhashtable
 * @addr: station's MAC address - duplicated from public part to
 *	let the hash table work with just a single cacheline
>>>>>>> v4.9.227
 * @local: pointer to the global information
 * @sdata: virtual interface this station belongs to
 * @ptk: peer keys negotiated with this station, if any
 * @ptk_idx: last installed peer key index
 * @gtk: group keys negotiated with this station, if any
<<<<<<< HEAD
 * @gtk_idx: last installed group key index
 * @rate_ctrl: rate control algorithm reference
 * @rate_ctrl_priv: rate control private per-STA pointer
 * @last_tx_rate: rate used for last transmit, to report to userspace as
 *	"the" transmit rate
 * @last_rx_rate_idx: rx status rate index of the last data packet
 * @last_rx_rate_flag: rx status flag of the last data packet
 * @last_rx_rate_vht_flag: rx status vht flag of the last data packet
 * @last_rx_rate_vht_nss: rx status nss of last data packet
=======
 * @rate_ctrl: rate control algorithm reference
 * @rate_ctrl_lock: spinlock used to protect rate control data
 *	(data inside the algorithm, so serializes calls there)
 * @rate_ctrl_priv: rate control private per-STA pointer
>>>>>>> v4.9.227
 * @lock: used for locking all fields that require locking, see comments
 *	in the header file.
 * @drv_deliver_wk: used for delivering frames after driver PS unblocking
 * @listen_interval: listen interval of this station, when we're acting as AP
 * @_flags: STA flags, see &enum ieee80211_sta_info_flags, do not use directly
 * @ps_lock: used for powersave (when mac80211 is the AP) related locking
 * @ps_tx_buf: buffers (per AC) of frames to transmit to this station
 *	when it leaves power saving state or polls
 * @tx_filtered: buffers (per AC) of frames we already tried to
 *	transmit but were filtered by hardware due to STA having
 *	entered power saving state, these are also delivered to
 *	the station when it leaves powersave or polls for frames
 * @driver_buffered_tids: bitmap of TIDs the driver has data buffered on
<<<<<<< HEAD
 * @rx_packets: Number of MSDUs received from this STA
 * @rx_bytes: Number of bytes received from this STA
 * @last_rx: time (in jiffies) when last frame was received from this STA
 * @last_connected: time (in seconds) when a station got connected
 * @num_duplicates: number of duplicate frames received from this STA
 * @rx_fragments: number of received MPDUs
 * @rx_dropped: number of dropped MPDUs from this STA
 * @last_signal: signal of last received frame from this STA
 * @avg_signal: moving average of signal of received frames from this STA
 * @last_ack_signal: signal of last received Ack frame from this STA
 * @last_seq_ctrl: last received seq/frag number from this STA (per RX queue)
 * @tx_filtered_count: number of frames the hardware filtered for this STA
 * @tx_retry_failed: number of frames that failed retry
 * @tx_retry_count: total number of retries for frames to this STA
 * @fail_avg: moving percentage of failed MSDUs
 * @tx_packets: number of RX/TX MSDUs
 * @tx_bytes: number of bytes transmitted to this STA
 * @tx_fragments: number of transmitted MPDUs
 * @tid_seq: per-TID sequence numbers for sending to this STA
 * @ampdu_mlme: A-MPDU state machine state
 * @timer_to_tid: identity mapping to ID timers
 * @tx_lat: Tx latency statistics
 * @llid: Local link ID
 * @plid: Peer link ID
 * @reason: Cancel reason on PLINK_HOLDING state
 * @plink_retries: Retries in establishment
 * @plink_state: peer link state
 * @plink_timeout: timeout of peer link
 * @plink_timer: peer link watch timer
 * @t_offset: timing offset relative to this host
 * @t_offset_setpoint: reference timing offset of this sta to be used when
 * 	calculating clockdrift
 * @local_pm: local link-specific power save mode
 * @peer_pm: peer-specific power save mode towards local STA
 * @nonpeer_pm: STA power save mode towards non-peer neighbors
 * @debugfs: debug filesystem info
 * @dead: set to true when sta is unlinked
 * @uploaded: set to true when sta is uploaded to the driver
 * @lost_packets: number of consecutive lost packets
 * @sta: station information we share with the driver
 * @sta_state: duplicates information about station state (for debug)
 * @beacon_loss_count: number of times beacon loss has triggered
 * @rcu_head: RCU head used for freeing this station struct
 * @cur_max_bandwidth: maximum bandwidth to use for TX to the station,
 *	taken from HT/VHT capabilities or VHT operating mode notification
 * @chains: chains ever used for RX from this station
 * @chain_signal_last: last signal (per chain)
 * @chain_signal_avg: signal average (per chain)
 * @known_smps_mode: the smps_mode the client thinks we are in. Relevant for
 *	AP only.
 * @cipher_scheme: optional cipher scheme for this station
 * @last_tdls_pkt_time: holds the time in jiffies of last TDLS pkt ACKed
=======
 * @txq_buffered_tids: bitmap of TIDs that mac80211 has txq data buffered on
 * @last_connected: time (in seconds) when a station got connected
 * @last_seq_ctrl: last received seq/frag number from this STA (per TID
 *	plus one for non-QoS frames)
 * @tid_seq: per-TID sequence numbers for sending to this STA
 * @ampdu_mlme: A-MPDU state machine state
 * @timer_to_tid: identity mapping to ID timers
 * @mesh: mesh STA information
 * @debugfs_dir: debug filesystem directory dentry
 * @dead: set to true when sta is unlinked
 * @removed: set to true when sta is being removed from sta_list
 * @uploaded: set to true when sta is uploaded to the driver
 * @sta: station information we share with the driver
 * @sta_state: duplicates information about station state (for debug)
 * @rcu_head: RCU head used for freeing this station struct
 * @cur_max_bandwidth: maximum bandwidth to use for TX to the station,
 *	taken from HT/VHT capabilities or VHT operating mode notification
 * @known_smps_mode: the smps_mode the client thinks we are in. Relevant for
 *	AP only.
 * @cipher_scheme: optional cipher scheme for this station
 * @reserved_tid: reserved TID (if any, otherwise IEEE80211_TID_UNRESERVED)
 * @fast_tx: TX fastpath information
 * @fast_rx: RX fastpath information
 * @tdls_chandef: a TDLS peer can have a wider chandef that is compatible to
 *	the BSS one.
 * @tx_stats: TX statistics
 * @rx_stats: RX statistics
 * @pcpu_rx_stats: per-CPU RX statistics, assigned only if the driver needs
 *	this (by advertising the USES_RSS hw flag)
 * @status_stats: TX status statistics
>>>>>>> v4.9.227
 */
struct sta_info {
	/* General information, mostly static */
	struct list_head list, free_list;
	struct rcu_head rcu_head;
<<<<<<< HEAD
	struct sta_info __rcu *hnext;
=======
	struct rhlist_head hash_node;
	u8 addr[ETH_ALEN];
>>>>>>> v4.9.227
	struct ieee80211_local *local;
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_key __rcu *gtk[NUM_DEFAULT_KEYS + NUM_DEFAULT_MGMT_KEYS];
	struct ieee80211_key __rcu *ptk[NUM_DEFAULT_KEYS];
<<<<<<< HEAD
	u8 gtk_idx;
	u8 ptk_idx;
	struct rate_control_ref *rate_ctrl;
	void *rate_ctrl_priv;
	spinlock_t lock;

=======
	u8 ptk_idx;
	struct rate_control_ref *rate_ctrl;
	void *rate_ctrl_priv;
	spinlock_t rate_ctrl_lock;
	spinlock_t lock;

	struct ieee80211_fast_tx __rcu *fast_tx;
	struct ieee80211_fast_rx __rcu *fast_rx;
	struct ieee80211_sta_rx_stats __percpu *pcpu_rx_stats;

#ifdef CONFIG_MAC80211_MESH
	struct mesh_sta *mesh;
#endif

>>>>>>> v4.9.227
	struct work_struct drv_deliver_wk;

	u16 listen_interval;

	bool dead;
<<<<<<< HEAD
=======
	bool removed;
>>>>>>> v4.9.227

	bool uploaded;

	enum ieee80211_sta_state sta_state;

	/* use the accessors defined below */
	unsigned long _flags;

	/* STA powersave lock and frame queues */
	spinlock_t ps_lock;
	struct sk_buff_head ps_tx_buf[IEEE80211_NUM_ACS];
	struct sk_buff_head tx_filtered[IEEE80211_NUM_ACS];
	unsigned long driver_buffered_tids;
<<<<<<< HEAD

	/* Updated from RX path only, no locking requirements */
	unsigned long rx_packets;
	u64 rx_bytes;
	unsigned long last_rx;
	long last_connected;
	unsigned long num_duplicates;
	unsigned long rx_fragments;
	unsigned long rx_dropped;
	int last_signal;
	struct ewma avg_signal;
	int last_ack_signal;

	u8 chains;
	s8 chain_signal_last[IEEE80211_MAX_CHAINS];
	struct ewma chain_signal_avg[IEEE80211_MAX_CHAINS];
=======
	unsigned long txq_buffered_tids;

	long last_connected;

	/* Updated from RX path only, no locking requirements */
	struct ieee80211_sta_rx_stats rx_stats;
	struct {
		struct ewma_signal signal;
		struct ewma_signal chain_signal[IEEE80211_MAX_CHAINS];
	} rx_stats_avg;
>>>>>>> v4.9.227

	/* Plus 1 for non-QoS frames */
	__le16 last_seq_ctrl[IEEE80211_NUM_TIDS + 1];

	/* Updated from TX status path only, no locking requirements */
<<<<<<< HEAD
	unsigned long tx_filtered_count;
	unsigned long tx_retry_failed, tx_retry_count;
	/* moving percentage of failed MSDUs */
	unsigned int fail_avg;

	/* Updated from TX path only, no locking requirements */
	u32 tx_fragments;
	u64 tx_packets[IEEE80211_NUM_ACS];
	u64 tx_bytes[IEEE80211_NUM_ACS];
	struct ieee80211_tx_rate last_tx_rate;
	int last_rx_rate_idx;
	u32 last_rx_rate_flag;
	u32 last_rx_rate_vht_flag;
	u8 last_rx_rate_vht_nss;
=======
	struct {
		unsigned long filtered;
		unsigned long retry_failed, retry_count;
		unsigned int lost_packets;
		unsigned long last_tdls_pkt_time;
		u64 msdu_retries[IEEE80211_NUM_TIDS + 1];
		u64 msdu_failed[IEEE80211_NUM_TIDS + 1];
		unsigned long last_ack;
	} status_stats;

	/* Updated from TX path only, no locking requirements */
	struct {
		u64 packets[IEEE80211_NUM_ACS];
		u64 bytes[IEEE80211_NUM_ACS];
		struct ieee80211_tx_rate last_rate;
		u64 msdu[IEEE80211_NUM_TIDS + 1];
	} tx_stats;
>>>>>>> v4.9.227
	u16 tid_seq[IEEE80211_QOS_CTL_TID_MASK + 1];

	/*
	 * Aggregation information, locked with lock.
	 */
	struct sta_ampdu_mlme ampdu_mlme;
	u8 timer_to_tid[IEEE80211_NUM_TIDS];

<<<<<<< HEAD
	struct ieee80211_tx_latency_stat *tx_lat;

#ifdef CONFIG_MAC80211_MESH
	/*
	 * Mesh peer link attributes
	 * TODO: move to a sub-structure that is referenced with pointer?
	 */
	u16 llid;
	u16 plid;
	u16 reason;
	u8 plink_retries;
	enum nl80211_plink_state plink_state;
	u32 plink_timeout;
	struct timer_list plink_timer;
	s64 t_offset;
	s64 t_offset_setpoint;
	/* mesh power save */
	enum nl80211_mesh_power_mode local_pm;
	enum nl80211_mesh_power_mode peer_pm;
	enum nl80211_mesh_power_mode nonpeer_pm;
#endif

#ifdef CONFIG_MAC80211_DEBUGFS
	struct sta_info_debugfsdentries {
		struct dentry *dir;
		bool add_has_run;
	} debugfs;
=======
#ifdef CONFIG_MAC80211_DEBUGFS
	struct dentry *debugfs_dir;
>>>>>>> v4.9.227
#endif

	enum ieee80211_sta_rx_bandwidth cur_max_bandwidth;

<<<<<<< HEAD
	unsigned int lost_packets;
	unsigned int beacon_loss_count;

	enum ieee80211_smps_mode known_smps_mode;
	const struct ieee80211_cipher_scheme *cipher_scheme;

	/* TDLS timeout data */
	unsigned long last_tdls_pkt_time;
=======
	enum ieee80211_smps_mode known_smps_mode;
	const struct ieee80211_cipher_scheme *cipher_scheme;

	u8 reserved_tid;

	struct cfg80211_chan_def tdls_chandef;
>>>>>>> v4.9.227

	/* keep last! */
	struct ieee80211_sta sta;
};

static inline enum nl80211_plink_state sta_plink_state(struct sta_info *sta)
{
#ifdef CONFIG_MAC80211_MESH
<<<<<<< HEAD
	return sta->plink_state;
=======
	return sta->mesh->plink_state;
>>>>>>> v4.9.227
#endif
	return NL80211_PLINK_LISTEN;
}

static inline void set_sta_flag(struct sta_info *sta,
				enum ieee80211_sta_info_flags flag)
{
	WARN_ON(flag == WLAN_STA_AUTH ||
		flag == WLAN_STA_ASSOC ||
		flag == WLAN_STA_AUTHORIZED);
	set_bit(flag, &sta->_flags);
}

static inline void clear_sta_flag(struct sta_info *sta,
				  enum ieee80211_sta_info_flags flag)
{
	WARN_ON(flag == WLAN_STA_AUTH ||
		flag == WLAN_STA_ASSOC ||
		flag == WLAN_STA_AUTHORIZED);
	clear_bit(flag, &sta->_flags);
}

static inline int test_sta_flag(struct sta_info *sta,
				enum ieee80211_sta_info_flags flag)
{
	return test_bit(flag, &sta->_flags);
}

static inline int test_and_clear_sta_flag(struct sta_info *sta,
					  enum ieee80211_sta_info_flags flag)
{
	WARN_ON(flag == WLAN_STA_AUTH ||
		flag == WLAN_STA_ASSOC ||
		flag == WLAN_STA_AUTHORIZED);
	return test_and_clear_bit(flag, &sta->_flags);
}

static inline int test_and_set_sta_flag(struct sta_info *sta,
					enum ieee80211_sta_info_flags flag)
{
	WARN_ON(flag == WLAN_STA_AUTH ||
		flag == WLAN_STA_ASSOC ||
		flag == WLAN_STA_AUTHORIZED);
	return test_and_set_bit(flag, &sta->_flags);
}

int sta_info_move_state(struct sta_info *sta,
			enum ieee80211_sta_state new_state);

static inline void sta_info_pre_move_state(struct sta_info *sta,
					   enum ieee80211_sta_state new_state)
{
	int ret;

	WARN_ON_ONCE(test_sta_flag(sta, WLAN_STA_INSERTED));

	ret = sta_info_move_state(sta, new_state);
	WARN_ON_ONCE(ret);
}


void ieee80211_assign_tid_tx(struct sta_info *sta, int tid,
			     struct tid_ampdu_tx *tid_tx);

static inline struct tid_ampdu_tx *
rcu_dereference_protected_tid_tx(struct sta_info *sta, int tid)
{
	return rcu_dereference_protected(sta->ampdu_mlme.tid_tx[tid],
					 lockdep_is_held(&sta->lock) ||
					 lockdep_is_held(&sta->ampdu_mlme.mtx));
}

<<<<<<< HEAD
#define STA_HASH_SIZE 256
#define STA_HASH(sta) (sta[5])


=======
>>>>>>> v4.9.227
/* Maximum number of frames to buffer per power saving station per AC */
#define STA_MAX_TX_BUFFER	64

/* Minimum buffered frame expiry time. If STA uses listen interval that is
 * smaller than this value, the minimum value here is used instead. */
#define STA_TX_BUFFER_EXPIRE (10 * HZ)

/* How often station data is cleaned up (e.g., expiration of buffered frames)
 */
#define STA_INFO_CLEANUP_INTERVAL (10 * HZ)

<<<<<<< HEAD
=======
struct rhlist_head *sta_info_hash_lookup(struct ieee80211_local *local,
					 const u8 *addr);

>>>>>>> v4.9.227
/*
 * Get a STA info, must be under RCU read lock.
 */
struct sta_info *sta_info_get(struct ieee80211_sub_if_data *sdata,
			      const u8 *addr);

struct sta_info *sta_info_get_bss(struct ieee80211_sub_if_data *sdata,
				  const u8 *addr);

<<<<<<< HEAD
static inline
void for_each_sta_info_type_check(struct ieee80211_local *local,
				  const u8 *addr,
				  struct sta_info *sta,
				  struct sta_info *nxt)
{
}

#define for_each_sta_info(local, _addr, _sta, nxt)			\
	for (	/* initialise loop */					\
		_sta = rcu_dereference(local->sta_hash[STA_HASH(_addr)]),\
		nxt = _sta ? rcu_dereference(_sta->hnext) : NULL;	\
		/* typecheck */						\
		for_each_sta_info_type_check(local, (_addr), _sta, nxt),\
		/* continue condition */				\
		_sta;							\
		/* advance loop */					\
		_sta = nxt,						\
		nxt = _sta ? rcu_dereference(_sta->hnext) : NULL	\
	     )								\
	/* compare address and run code only if it matches */		\
	if (ether_addr_equal(_sta->sta.addr, (_addr)))
=======
#define for_each_sta_info(local, _addr, _sta, _tmp)			\
	rhl_for_each_entry_rcu(_sta, _tmp,				\
			       sta_info_hash_lookup(local, _addr), hash_node)
>>>>>>> v4.9.227

/*
 * Get STA info by index, BROKEN!
 */
struct sta_info *sta_info_get_by_idx(struct ieee80211_sub_if_data *sdata,
				     int idx);
/*
 * Create a new STA info, caller owns returned structure
 * until sta_info_insert().
 */
struct sta_info *sta_info_alloc(struct ieee80211_sub_if_data *sdata,
				const u8 *addr, gfp_t gfp);

void sta_info_free(struct ieee80211_local *local, struct sta_info *sta);

/*
 * Insert STA info into hash table/list, returns zero or a
 * -EEXIST if (if the same MAC address is already present).
 *
 * Calling the non-rcu version makes the caller relinquish,
 * the _rcu version calls read_lock_rcu() and must be called
 * without it held.
 */
int sta_info_insert(struct sta_info *sta);
int sta_info_insert_rcu(struct sta_info *sta) __acquires(RCU);

int __must_check __sta_info_destroy(struct sta_info *sta);
int sta_info_destroy_addr(struct ieee80211_sub_if_data *sdata,
			  const u8 *addr);
int sta_info_destroy_addr_bss(struct ieee80211_sub_if_data *sdata,
			      const u8 *addr);

void sta_info_recalc_tim(struct sta_info *sta);

<<<<<<< HEAD
void sta_info_init(struct ieee80211_local *local);
=======
int sta_info_init(struct ieee80211_local *local);
>>>>>>> v4.9.227
void sta_info_stop(struct ieee80211_local *local);

/**
 * sta_info_flush - flush matching STA entries from the STA table
 *
 * Returns the number of removed STA entries.
 *
 * @sdata: sdata to remove all stations from
 * @vlans: if the given interface is an AP interface, also flush VLANs
 */
int __sta_info_flush(struct ieee80211_sub_if_data *sdata, bool vlans);

static inline int sta_info_flush(struct ieee80211_sub_if_data *sdata)
{
	return __sta_info_flush(sdata, false);
}

void sta_set_rate_info_tx(struct sta_info *sta,
			  const struct ieee80211_tx_rate *rate,
			  struct rate_info *rinfo);
<<<<<<< HEAD
void sta_set_rate_info_rx(struct sta_info *sta,
			  struct rate_info *rinfo);
void sta_set_sinfo(struct sta_info *sta, struct station_info *sinfo);

=======
void sta_set_sinfo(struct sta_info *sta, struct station_info *sinfo);

u32 sta_get_expected_throughput(struct sta_info *sta);

>>>>>>> v4.9.227
void ieee80211_sta_expire(struct ieee80211_sub_if_data *sdata,
			  unsigned long exp_time);
u8 sta_info_tx_streams(struct sta_info *sta);

void ieee80211_sta_ps_deliver_wakeup(struct sta_info *sta);
void ieee80211_sta_ps_deliver_poll_response(struct sta_info *sta);
void ieee80211_sta_ps_deliver_uapsd(struct sta_info *sta);

<<<<<<< HEAD
=======
unsigned long ieee80211_sta_last_active(struct sta_info *sta);

#define STA_STATS_RATE_INVALID		0
#define STA_STATS_RATE_VHT		0x8000
#define STA_STATS_RATE_HT		0x4000
#define STA_STATS_RATE_LEGACY		0x2000
#define STA_STATS_RATE_SGI		0x1000
#define STA_STATS_RATE_BW_SHIFT		9
#define STA_STATS_RATE_BW_MASK		(0x7 << STA_STATS_RATE_BW_SHIFT)

static inline u16 sta_stats_encode_rate(struct ieee80211_rx_status *s)
{
	u16 r = s->rate_idx;

	if (s->vht_flag & RX_VHT_FLAG_80MHZ)
		r |= RATE_INFO_BW_80 << STA_STATS_RATE_BW_SHIFT;
	else if (s->vht_flag & RX_VHT_FLAG_160MHZ)
		r |= RATE_INFO_BW_160 << STA_STATS_RATE_BW_SHIFT;
	else if (s->flag & RX_FLAG_40MHZ)
		r |= RATE_INFO_BW_40 << STA_STATS_RATE_BW_SHIFT;
	else if (s->flag & RX_FLAG_10MHZ)
		r |= RATE_INFO_BW_10 << STA_STATS_RATE_BW_SHIFT;
	else if (s->flag & RX_FLAG_5MHZ)
		r |= RATE_INFO_BW_5 << STA_STATS_RATE_BW_SHIFT;
	else
		r |= RATE_INFO_BW_20 << STA_STATS_RATE_BW_SHIFT;

	if (s->flag & RX_FLAG_SHORT_GI)
		r |= STA_STATS_RATE_SGI;

	if (s->flag & RX_FLAG_VHT)
		r |= STA_STATS_RATE_VHT | (s->vht_nss << 4);
	else if (s->flag & RX_FLAG_HT)
		r |= STA_STATS_RATE_HT;
	else
		r |= STA_STATS_RATE_LEGACY | (s->band << 4);

	return r;
}

>>>>>>> v4.9.227
#endif /* STA_INFO_H */
