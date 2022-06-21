/*
 * Copyright 2003-2005	Devicescape Software, Inc.
 * Copyright (c) 2006	Jiri Benc <jbenc@suse.cz>
 * Copyright 2007	Johannes Berg <johannes@sipsolutions.net>
 * Copyright 2013-2014  Intel Mobile Communications GmbH
<<<<<<< HEAD
=======
 * Copyright(c) 2016 Intel Deutschland GmbH
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/debugfs.h>
#include <linux/ieee80211.h>
#include "ieee80211_i.h"
#include "debugfs.h"
#include "debugfs_sta.h"
#include "sta_info.h"
#include "driver-ops.h"

/* sta attributtes */

#define STA_READ(name, field, format_string)				\
static ssize_t sta_ ##name## _read(struct file *file,			\
				   char __user *userbuf,		\
				   size_t count, loff_t *ppos)		\
{									\
	struct sta_info *sta = file->private_data;			\
	return mac80211_format_buffer(userbuf, count, ppos, 		\
				      format_string, sta->field);	\
}
#define STA_READ_D(name, field) STA_READ(name, field, "%d\n")
<<<<<<< HEAD
#define STA_READ_U(name, field) STA_READ(name, field, "%u\n")
#define STA_READ_S(name, field) STA_READ(name, field, "%s\n")
=======
>>>>>>> v4.9.227

#define STA_OPS(name)							\
static const struct file_operations sta_ ##name## _ops = {		\
	.read = sta_##name##_read,					\
	.open = simple_open,						\
	.llseek = generic_file_llseek,					\
}

<<<<<<< HEAD
#define STA_OPS_W(name)							\
static const struct file_operations sta_ ##name## _ops = {		\
	.write = sta_##name##_write,					\
	.open = simple_open,						\
	.llseek = generic_file_llseek,					\
}

=======
>>>>>>> v4.9.227
#define STA_OPS_RW(name)						\
static const struct file_operations sta_ ##name## _ops = {		\
	.read = sta_##name##_read,					\
	.write = sta_##name##_write,					\
	.open = simple_open,						\
	.llseek = generic_file_llseek,					\
}

#define STA_FILE(name, field, format)					\
		STA_READ_##format(name, field)				\
		STA_OPS(name)

STA_FILE(aid, sta.aid, D);
<<<<<<< HEAD
STA_FILE(dev, sdata->name, S);
STA_FILE(last_signal, last_signal, D);
STA_FILE(last_ack_signal, last_ack_signal, D);
STA_FILE(beacon_loss_count, beacon_loss_count, D);
=======

static const char * const sta_flag_names[] = {
#define FLAG(F) [WLAN_STA_##F] = #F
	FLAG(AUTH),
	FLAG(ASSOC),
	FLAG(PS_STA),
	FLAG(AUTHORIZED),
	FLAG(SHORT_PREAMBLE),
	FLAG(WDS),
	FLAG(CLEAR_PS_FILT),
	FLAG(MFP),
	FLAG(BLOCK_BA),
	FLAG(PS_DRIVER),
	FLAG(PSPOLL),
	FLAG(TDLS_PEER),
	FLAG(TDLS_PEER_AUTH),
	FLAG(TDLS_INITIATOR),
	FLAG(TDLS_CHAN_SWITCH),
	FLAG(TDLS_OFF_CHANNEL),
	FLAG(TDLS_WIDER_BW),
	FLAG(UAPSD),
	FLAG(SP),
	FLAG(4ADDR_EVENT),
	FLAG(INSERTED),
	FLAG(RATE_CONTROL),
	FLAG(TOFFSET_KNOWN),
	FLAG(MPSP_OWNER),
	FLAG(MPSP_RECIPIENT),
	FLAG(PS_DELIVER),
#undef FLAG
};
>>>>>>> v4.9.227

static ssize_t sta_flags_read(struct file *file, char __user *userbuf,
			      size_t count, loff_t *ppos)
{
<<<<<<< HEAD
	char buf[121];
	struct sta_info *sta = file->private_data;

#define TEST(flg) \
	test_sta_flag(sta, WLAN_STA_##flg) ? #flg "\n" : ""

	int res = scnprintf(buf, sizeof(buf),
			    "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
			    TEST(AUTH), TEST(ASSOC), TEST(PS_STA),
			    TEST(PS_DRIVER), TEST(AUTHORIZED),
			    TEST(SHORT_PREAMBLE),
			    sta->sta.wme ? "WME\n" : "",
			    TEST(WDS), TEST(CLEAR_PS_FILT),
			    TEST(MFP), TEST(BLOCK_BA), TEST(PSPOLL),
			    TEST(UAPSD), TEST(SP), TEST(TDLS_PEER),
			    TEST(TDLS_PEER_AUTH), TEST(4ADDR_EVENT),
			    TEST(INSERTED), TEST(RATE_CONTROL),
			    TEST(TOFFSET_KNOWN), TEST(MPSP_OWNER),
			    TEST(MPSP_RECIPIENT));
#undef TEST
	return simple_read_from_buffer(userbuf, count, ppos, buf, res);
=======
	char buf[16 * NUM_WLAN_STA_FLAGS], *pos = buf;
	char *end = buf + sizeof(buf) - 1;
	struct sta_info *sta = file->private_data;
	unsigned int flg;

	BUILD_BUG_ON(ARRAY_SIZE(sta_flag_names) != NUM_WLAN_STA_FLAGS);

	for (flg = 0; flg < NUM_WLAN_STA_FLAGS; flg++) {
		if (test_sta_flag(sta, flg))
			pos += scnprintf(pos, end - pos, "%s\n",
					 sta_flag_names[flg]);
	}

	return simple_read_from_buffer(userbuf, count, ppos, buf, strlen(buf));
>>>>>>> v4.9.227
}
STA_OPS(flags);

static ssize_t sta_num_ps_buf_frames_read(struct file *file,
					  char __user *userbuf,
					  size_t count, loff_t *ppos)
{
	struct sta_info *sta = file->private_data;
	char buf[17*IEEE80211_NUM_ACS], *p = buf;
	int ac;

	for (ac = 0; ac < IEEE80211_NUM_ACS; ac++)
		p += scnprintf(p, sizeof(buf)+buf-p, "AC%d: %d\n", ac,
			       skb_queue_len(&sta->ps_tx_buf[ac]) +
			       skb_queue_len(&sta->tx_filtered[ac]));
	return simple_read_from_buffer(userbuf, count, ppos, buf, p - buf);
}
STA_OPS(num_ps_buf_frames);

<<<<<<< HEAD
static ssize_t sta_inactive_ms_read(struct file *file, char __user *userbuf,
				    size_t count, loff_t *ppos)
{
	struct sta_info *sta = file->private_data;
	return mac80211_format_buffer(userbuf, count, ppos, "%d\n",
				      jiffies_to_msecs(jiffies - sta->last_rx));
}
STA_OPS(inactive_ms);


static ssize_t sta_connected_time_read(struct file *file, char __user *userbuf,
					size_t count, loff_t *ppos)
{
	struct sta_info *sta = file->private_data;
	struct timespec uptime;
	struct tm result;
	long connected_time_secs;
	char buf[100];
	int res;
	ktime_get_ts(&uptime);
	connected_time_secs = uptime.tv_sec - sta->last_connected;
	time_to_tm(connected_time_secs, 0, &result);
	result.tm_year -= 70;
	result.tm_mday -= 1;
	res = scnprintf(buf, sizeof(buf),
		"years  - %ld\nmonths - %d\ndays   - %d\nclock  - %d:%d:%d\n\n",
			result.tm_year, result.tm_mon, result.tm_mday,
			result.tm_hour, result.tm_min, result.tm_sec);
	return simple_read_from_buffer(userbuf, count, ppos, buf, res);
}
STA_OPS(connected_time);



=======
>>>>>>> v4.9.227
static ssize_t sta_last_seq_ctrl_read(struct file *file, char __user *userbuf,
				      size_t count, loff_t *ppos)
{
	char buf[15*IEEE80211_NUM_TIDS], *p = buf;
	int i;
	struct sta_info *sta = file->private_data;
	for (i = 0; i < IEEE80211_NUM_TIDS; i++)
		p += scnprintf(p, sizeof(buf)+buf-p, "%x ",
			       le16_to_cpu(sta->last_seq_ctrl[i]));
	p += scnprintf(p, sizeof(buf)+buf-p, "\n");
	return simple_read_from_buffer(userbuf, count, ppos, buf, p - buf);
}
STA_OPS(last_seq_ctrl);

<<<<<<< HEAD
=======
#define AQM_TXQ_ENTRY_LEN 130

static ssize_t sta_aqm_read(struct file *file, char __user *userbuf,
			size_t count, loff_t *ppos)
{
	struct sta_info *sta = file->private_data;
	struct ieee80211_local *local = sta->local;
	size_t bufsz = AQM_TXQ_ENTRY_LEN*(IEEE80211_NUM_TIDS+1);
	char *buf = kzalloc(bufsz, GFP_KERNEL), *p = buf;
	struct txq_info *txqi;
	ssize_t rv;
	int i;

	if (!buf)
		return -ENOMEM;

	spin_lock_bh(&local->fq.lock);
	rcu_read_lock();

	p += scnprintf(p,
		       bufsz+buf-p,
		       "tid ac backlog-bytes backlog-packets new-flows drops marks overlimit collisions tx-bytes tx-packets\n");

	for (i = 0; i < IEEE80211_NUM_TIDS; i++) {
		txqi = to_txq_info(sta->sta.txq[i]);
		p += scnprintf(p, bufsz+buf-p,
			       "%d %d %u %u %u %u %u %u %u %u %u\n",
			       txqi->txq.tid,
			       txqi->txq.ac,
			       txqi->tin.backlog_bytes,
			       txqi->tin.backlog_packets,
			       txqi->tin.flows,
			       txqi->cstats.drop_count,
			       txqi->cstats.ecn_mark,
			       txqi->tin.overlimit,
			       txqi->tin.collisions,
			       txqi->tin.tx_bytes,
			       txqi->tin.tx_packets);
	}

	rcu_read_unlock();
	spin_unlock_bh(&local->fq.lock);

	rv = simple_read_from_buffer(userbuf, count, ppos, buf, p - buf);
	kfree(buf);
	return rv;
}
STA_OPS(aqm);

>>>>>>> v4.9.227
static ssize_t sta_agg_status_read(struct file *file, char __user *userbuf,
					size_t count, loff_t *ppos)
{
	char buf[71 + IEEE80211_NUM_TIDS * 40], *p = buf;
	int i;
	struct sta_info *sta = file->private_data;
	struct tid_ampdu_rx *tid_rx;
	struct tid_ampdu_tx *tid_tx;

	rcu_read_lock();

	p += scnprintf(p, sizeof(buf) + buf - p, "next dialog_token: %#02x\n",
			sta->ampdu_mlme.dialog_token_allocator + 1);
	p += scnprintf(p, sizeof(buf) + buf - p,
		       "TID\t\tRX\tDTKN\tSSN\t\tTX\tDTKN\tpending\n");

	for (i = 0; i < IEEE80211_NUM_TIDS; i++) {
		tid_rx = rcu_dereference(sta->ampdu_mlme.tid_rx[i]);
		tid_tx = rcu_dereference(sta->ampdu_mlme.tid_tx[i]);

		p += scnprintf(p, sizeof(buf) + buf - p, "%02d", i);
		p += scnprintf(p, sizeof(buf) + buf - p, "\t\t%x", !!tid_rx);
		p += scnprintf(p, sizeof(buf) + buf - p, "\t%#.2x",
<<<<<<< HEAD
				tid_rx ? tid_rx->dialog_token : 0);
=======
				tid_rx ? sta->ampdu_mlme.tid_rx_token[i] : 0);
>>>>>>> v4.9.227
		p += scnprintf(p, sizeof(buf) + buf - p, "\t%#.3x",
				tid_rx ? tid_rx->ssn : 0);

		p += scnprintf(p, sizeof(buf) + buf - p, "\t\t%x", !!tid_tx);
		p += scnprintf(p, sizeof(buf) + buf - p, "\t%#.2x",
				tid_tx ? tid_tx->dialog_token : 0);
		p += scnprintf(p, sizeof(buf) + buf - p, "\t%03d",
				tid_tx ? skb_queue_len(&tid_tx->pending) : 0);
		p += scnprintf(p, sizeof(buf) + buf - p, "\n");
	}
	rcu_read_unlock();

	return simple_read_from_buffer(userbuf, count, ppos, buf, p - buf);
}

static ssize_t sta_agg_status_write(struct file *file, const char __user *userbuf,
				    size_t count, loff_t *ppos)
{
<<<<<<< HEAD
	char _buf[12] = {}, *buf = _buf;
	struct sta_info *sta = file->private_data;
	bool start, tx;
	unsigned long tid;
	int ret;
=======
	char _buf[25] = {}, *buf = _buf;
	struct sta_info *sta = file->private_data;
	bool start, tx;
	unsigned long tid;
	char *pos;
	int ret, timeout = 5000;
>>>>>>> v4.9.227

	if (count > sizeof(_buf))
		return -EINVAL;

	if (copy_from_user(buf, userbuf, count))
		return -EFAULT;

	buf[sizeof(_buf) - 1] = '\0';
<<<<<<< HEAD

	if (strncmp(buf, "tx ", 3) == 0) {
		buf += 3;
		tx = true;
	} else if (strncmp(buf, "rx ", 3) == 0) {
		buf += 3;
		tx = false;
	} else
		return -EINVAL;

	if (strncmp(buf, "start ", 6) == 0) {
		buf += 6;
		start = true;
		if (!tx)
			return -EINVAL;
	} else if (strncmp(buf, "stop ", 5) == 0) {
		buf += 5;
		start = false;
	} else
		return -EINVAL;

	ret = kstrtoul(buf, 0, &tid);
	if (ret)
		return ret;

	if (tid >= IEEE80211_NUM_TIDS)
=======
	pos = buf;
	buf = strsep(&pos, " ");
	if (!buf)
		return -EINVAL;

	if (!strcmp(buf, "tx"))
		tx = true;
	else if (!strcmp(buf, "rx"))
		tx = false;
	else
		return -EINVAL;

	buf = strsep(&pos, " ");
	if (!buf)
		return -EINVAL;
	if (!strcmp(buf, "start")) {
		start = true;
		if (!tx)
			return -EINVAL;
	} else if (!strcmp(buf, "stop")) {
		start = false;
	} else {
		return -EINVAL;
	}

	buf = strsep(&pos, " ");
	if (!buf)
		return -EINVAL;
	if (sscanf(buf, "timeout=%d", &timeout) == 1) {
		buf = strsep(&pos, " ");
		if (!buf || !tx || !start)
			return -EINVAL;
	}

	ret = kstrtoul(buf, 0, &tid);
	if (ret || tid >= IEEE80211_NUM_TIDS)
>>>>>>> v4.9.227
		return -EINVAL;

	if (tx) {
		if (start)
<<<<<<< HEAD
			ret = ieee80211_start_tx_ba_session(&sta->sta, tid, 5000);
=======
			ret = ieee80211_start_tx_ba_session(&sta->sta, tid,
							    timeout);
>>>>>>> v4.9.227
		else
			ret = ieee80211_stop_tx_ba_session(&sta->sta, tid);
	} else {
		__ieee80211_stop_rx_ba_session(sta, tid, WLAN_BACK_RECIPIENT,
					       3, true);
		ret = 0;
	}

	return ret ?: count;
}
STA_OPS_RW(agg_status);

static ssize_t sta_ht_capa_read(struct file *file, char __user *userbuf,
				size_t count, loff_t *ppos)
{
#define PRINT_HT_CAP(_cond, _str) \
	do { \
	if (_cond) \
			p += scnprintf(p, sizeof(buf)+buf-p, "\t" _str "\n"); \
	} while (0)
	char buf[512], *p = buf;
	int i;
	struct sta_info *sta = file->private_data;
	struct ieee80211_sta_ht_cap *htc = &sta->sta.ht_cap;

	p += scnprintf(p, sizeof(buf) + buf - p, "ht %ssupported\n",
			htc->ht_supported ? "" : "not ");
	if (htc->ht_supported) {
		p += scnprintf(p, sizeof(buf)+buf-p, "cap: %#.4x\n", htc->cap);

		PRINT_HT_CAP((htc->cap & BIT(0)), "RX LDPC");
		PRINT_HT_CAP((htc->cap & BIT(1)), "HT20/HT40");
		PRINT_HT_CAP(!(htc->cap & BIT(1)), "HT20");

		PRINT_HT_CAP(((htc->cap >> 2) & 0x3) == 0, "Static SM Power Save");
		PRINT_HT_CAP(((htc->cap >> 2) & 0x3) == 1, "Dynamic SM Power Save");
		PRINT_HT_CAP(((htc->cap >> 2) & 0x3) == 3, "SM Power Save disabled");

		PRINT_HT_CAP((htc->cap & BIT(4)), "RX Greenfield");
		PRINT_HT_CAP((htc->cap & BIT(5)), "RX HT20 SGI");
		PRINT_HT_CAP((htc->cap & BIT(6)), "RX HT40 SGI");
		PRINT_HT_CAP((htc->cap & BIT(7)), "TX STBC");

		PRINT_HT_CAP(((htc->cap >> 8) & 0x3) == 0, "No RX STBC");
		PRINT_HT_CAP(((htc->cap >> 8) & 0x3) == 1, "RX STBC 1-stream");
		PRINT_HT_CAP(((htc->cap >> 8) & 0x3) == 2, "RX STBC 2-streams");
		PRINT_HT_CAP(((htc->cap >> 8) & 0x3) == 3, "RX STBC 3-streams");

		PRINT_HT_CAP((htc->cap & BIT(10)), "HT Delayed Block Ack");

		PRINT_HT_CAP(!(htc->cap & BIT(11)), "Max AMSDU length: "
			     "3839 bytes");
		PRINT_HT_CAP((htc->cap & BIT(11)), "Max AMSDU length: "
			     "7935 bytes");

		/*
		 * For beacons and probe response this would mean the BSS
		 * does or does not allow the usage of DSSS/CCK HT40.
		 * Otherwise it means the STA does or does not use
		 * DSSS/CCK HT40.
		 */
		PRINT_HT_CAP((htc->cap & BIT(12)), "DSSS/CCK HT40");
		PRINT_HT_CAP(!(htc->cap & BIT(12)), "No DSSS/CCK HT40");

		/* BIT(13) is reserved */

		PRINT_HT_CAP((htc->cap & BIT(14)), "40 MHz Intolerant");

		PRINT_HT_CAP((htc->cap & BIT(15)), "L-SIG TXOP protection");

		p += scnprintf(p, sizeof(buf)+buf-p, "ampdu factor/density: %d/%d\n",
				htc->ampdu_factor, htc->ampdu_density);
		p += scnprintf(p, sizeof(buf)+buf-p, "MCS mask:");

		for (i = 0; i < IEEE80211_HT_MCS_MASK_LEN; i++)
			p += scnprintf(p, sizeof(buf)+buf-p, " %.2x",
					htc->mcs.rx_mask[i]);
		p += scnprintf(p, sizeof(buf)+buf-p, "\n");

		/* If not set this is meaningless */
		if (le16_to_cpu(htc->mcs.rx_highest)) {
			p += scnprintf(p, sizeof(buf)+buf-p,
				       "MCS rx highest: %d Mbps\n",
				       le16_to_cpu(htc->mcs.rx_highest));
		}

		p += scnprintf(p, sizeof(buf)+buf-p, "MCS tx params: %x\n",
				htc->mcs.tx_params);
	}

	return simple_read_from_buffer(userbuf, count, ppos, buf, p - buf);
}
STA_OPS(ht_capa);

static ssize_t sta_vht_capa_read(struct file *file, char __user *userbuf,
				 size_t count, loff_t *ppos)
{
<<<<<<< HEAD
	char buf[128], *p = buf;
=======
	char buf[512], *p = buf;
>>>>>>> v4.9.227
	struct sta_info *sta = file->private_data;
	struct ieee80211_sta_vht_cap *vhtc = &sta->sta.vht_cap;

	p += scnprintf(p, sizeof(buf) + buf - p, "VHT %ssupported\n",
			vhtc->vht_supported ? "" : "not ");
	if (vhtc->vht_supported) {
<<<<<<< HEAD
		p += scnprintf(p, sizeof(buf)+buf-p, "cap: %#.8x\n", vhtc->cap);
=======
		p += scnprintf(p, sizeof(buf) + buf - p, "cap: %#.8x\n",
			       vhtc->cap);
#define PFLAG(a, b)							\
		do {							\
			if (vhtc->cap & IEEE80211_VHT_CAP_ ## a)	\
				p += scnprintf(p, sizeof(buf) + buf - p, \
					       "\t\t%s\n", b);		\
		} while (0)

		switch (vhtc->cap & 0x3) {
		case IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_3895:
			p += scnprintf(p, sizeof(buf) + buf - p,
				       "\t\tMAX-MPDU-3895\n");
			break;
		case IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_7991:
			p += scnprintf(p, sizeof(buf) + buf - p,
				       "\t\tMAX-MPDU-7991\n");
			break;
		case IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_11454:
			p += scnprintf(p, sizeof(buf) + buf - p,
				       "\t\tMAX-MPDU-11454\n");
			break;
		default:
			p += scnprintf(p, sizeof(buf) + buf - p,
				       "\t\tMAX-MPDU-UNKNOWN\n");
		};
		switch (vhtc->cap & IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_MASK) {
		case 0:
			p += scnprintf(p, sizeof(buf) + buf - p,
				       "\t\t80Mhz\n");
			break;
		case IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160MHZ:
			p += scnprintf(p, sizeof(buf) + buf - p,
				       "\t\t160Mhz\n");
			break;
		case IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160_80PLUS80MHZ:
			p += scnprintf(p, sizeof(buf) + buf - p,
				       "\t\t80+80Mhz\n");
			break;
		default:
			p += scnprintf(p, sizeof(buf) + buf - p,
				       "\t\tUNKNOWN-MHZ: 0x%x\n",
				       (vhtc->cap >> 2) & 0x3);
		};
		PFLAG(RXLDPC, "RXLDPC");
		PFLAG(SHORT_GI_80, "SHORT-GI-80");
		PFLAG(SHORT_GI_160, "SHORT-GI-160");
		PFLAG(TXSTBC, "TXSTBC");
		p += scnprintf(p, sizeof(buf) + buf - p,
			       "\t\tRXSTBC_%d\n", (vhtc->cap >> 8) & 0x7);
		PFLAG(SU_BEAMFORMER_CAPABLE, "SU-BEAMFORMER-CAPABLE");
		PFLAG(SU_BEAMFORMEE_CAPABLE, "SU-BEAMFORMEE-CAPABLE");
		p += scnprintf(p, sizeof(buf) + buf - p,
			"\t\tBEAMFORMEE-STS: 0x%x\n",
			(vhtc->cap & IEEE80211_VHT_CAP_BEAMFORMEE_STS_MASK) >>
			IEEE80211_VHT_CAP_BEAMFORMEE_STS_SHIFT);
		p += scnprintf(p, sizeof(buf) + buf - p,
			"\t\tSOUNDING-DIMENSIONS: 0x%x\n",
			(vhtc->cap & IEEE80211_VHT_CAP_SOUNDING_DIMENSIONS_MASK)
			>> IEEE80211_VHT_CAP_SOUNDING_DIMENSIONS_SHIFT);
		PFLAG(MU_BEAMFORMER_CAPABLE, "MU-BEAMFORMER-CAPABLE");
		PFLAG(MU_BEAMFORMEE_CAPABLE, "MU-BEAMFORMEE-CAPABLE");
		PFLAG(VHT_TXOP_PS, "TXOP-PS");
		PFLAG(HTC_VHT, "HTC-VHT");
		p += scnprintf(p, sizeof(buf) + buf - p,
			"\t\tMPDU-LENGTH-EXPONENT: 0x%x\n",
			(vhtc->cap & IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_MASK) >>
			IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_SHIFT);
		PFLAG(VHT_LINK_ADAPTATION_VHT_UNSOL_MFB,
		      "LINK-ADAPTATION-VHT-UNSOL-MFB");
		p += scnprintf(p, sizeof(buf) + buf - p,
			"\t\tLINK-ADAPTATION-VHT-MRQ-MFB: 0x%x\n",
			(vhtc->cap & IEEE80211_VHT_CAP_VHT_LINK_ADAPTATION_VHT_MRQ_MFB) >> 26);
		PFLAG(RX_ANTENNA_PATTERN, "RX-ANTENNA-PATTERN");
		PFLAG(TX_ANTENNA_PATTERN, "TX-ANTENNA-PATTERN");
>>>>>>> v4.9.227

		p += scnprintf(p, sizeof(buf)+buf-p, "RX MCS: %.4x\n",
			       le16_to_cpu(vhtc->vht_mcs.rx_mcs_map));
		if (vhtc->vht_mcs.rx_highest)
			p += scnprintf(p, sizeof(buf)+buf-p,
				       "MCS RX highest: %d Mbps\n",
				       le16_to_cpu(vhtc->vht_mcs.rx_highest));
		p += scnprintf(p, sizeof(buf)+buf-p, "TX MCS: %.4x\n",
			       le16_to_cpu(vhtc->vht_mcs.tx_mcs_map));
		if (vhtc->vht_mcs.tx_highest)
			p += scnprintf(p, sizeof(buf)+buf-p,
				       "MCS TX highest: %d Mbps\n",
				       le16_to_cpu(vhtc->vht_mcs.tx_highest));
	}

	return simple_read_from_buffer(userbuf, count, ppos, buf, p - buf);
}
STA_OPS(vht_capa);

<<<<<<< HEAD
static ssize_t sta_current_tx_rate_read(struct file *file, char __user *userbuf,
					size_t count, loff_t *ppos)
{
	struct sta_info *sta = file->private_data;
	struct rate_info rinfo;
	u16 rate;
	sta_set_rate_info_tx(sta, &sta->last_tx_rate, &rinfo);
	rate = cfg80211_calculate_bitrate(&rinfo);

	return mac80211_format_buffer(userbuf, count, ppos,
				      "%d.%d MBit/s\n",
				      rate/10, rate%10);
}
STA_OPS(current_tx_rate);

static ssize_t sta_last_rx_rate_read(struct file *file, char __user *userbuf,
				     size_t count, loff_t *ppos)
{
	struct sta_info *sta = file->private_data;
	struct rate_info rinfo;
	u16 rate;

	sta_set_rate_info_rx(sta, &rinfo);

	rate = cfg80211_calculate_bitrate(&rinfo);

	return mac80211_format_buffer(userbuf, count, ppos,
				      "%d.%d MBit/s\n",
				      rate/10, rate%10);
}
STA_OPS(last_rx_rate);

static int
sta_tx_latency_stat_header(struct ieee80211_tx_latency_bin_ranges *tx_latency,
			   char *buf, int pos, int bufsz)
{
	int i;
	int range_count = tx_latency->n_ranges;
	u32 *bin_ranges = tx_latency->ranges;

	pos += scnprintf(buf + pos, bufsz - pos,
			  "Station\t\t\tTID\tMax\tAvg");
	if (range_count) {
		pos += scnprintf(buf + pos, bufsz - pos,
				  "\t<=%d", bin_ranges[0]);
		for (i = 0; i < range_count - 1; i++)
			pos += scnprintf(buf + pos, bufsz - pos, "\t%d-%d",
					  bin_ranges[i], bin_ranges[i+1]);
		pos += scnprintf(buf + pos, bufsz - pos,
				  "\t%d<", bin_ranges[range_count - 1]);
	}

	pos += scnprintf(buf + pos, bufsz - pos, "\n");

	return pos;
}

static int
sta_tx_latency_stat_table(struct ieee80211_tx_latency_bin_ranges *tx_lat_range,
			  struct ieee80211_tx_latency_stat *tx_lat,
			  char *buf, int pos, int bufsz, int tid)
{
	u32 avg = 0;
	int j;
	int bin_count = tx_lat->bin_count;

	pos += scnprintf(buf + pos, bufsz - pos, "\t\t\t%d", tid);
	/* make sure you don't divide in 0 */
	if (tx_lat->counter)
		avg = tx_lat->sum / tx_lat->counter;

	pos += scnprintf(buf + pos, bufsz - pos, "\t%d\t%d",
			  tx_lat->max, avg);

	if (tx_lat_range->n_ranges && tx_lat->bins)
		for (j = 0; j < bin_count; j++)
			pos += scnprintf(buf + pos, bufsz - pos,
					  "\t%d", tx_lat->bins[j]);
	pos += scnprintf(buf + pos, bufsz - pos, "\n");

	return pos;
}

/*
 * Output Tx latency statistics station && restart all statistics information
 */
static ssize_t sta_tx_latency_stat_read(struct file *file,
					char __user *userbuf,
					size_t count, loff_t *ppos)
{
	struct sta_info *sta = file->private_data;
	struct ieee80211_local *local = sta->local;
	struct ieee80211_tx_latency_bin_ranges *tx_latency;
	char *buf;
	int bufsz, ret, i;
	int pos = 0;

	bufsz = 20 * IEEE80211_NUM_TIDS *
		sizeof(struct ieee80211_tx_latency_stat);
	buf = kzalloc(bufsz, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	rcu_read_lock();

	tx_latency = rcu_dereference(local->tx_latency);

	if (!sta->tx_lat) {
		pos += scnprintf(buf + pos, bufsz - pos,
				 "Tx latency statistics are not enabled\n");
		goto unlock;
	}

	pos = sta_tx_latency_stat_header(tx_latency, buf, pos, bufsz);

	pos += scnprintf(buf + pos, bufsz - pos, "%pM\n", sta->sta.addr);
	for (i = 0; i < IEEE80211_NUM_TIDS; i++)
		pos = sta_tx_latency_stat_table(tx_latency, &sta->tx_lat[i],
						buf, pos, bufsz, i);
unlock:
	rcu_read_unlock();

	ret = simple_read_from_buffer(userbuf, count, ppos, buf, pos);
	kfree(buf);

	return ret;
}
STA_OPS(tx_latency_stat);

static ssize_t sta_tx_latency_stat_reset_write(struct file *file,
					       const char __user *userbuf,
					       size_t count, loff_t *ppos)
{
	u32 *bins;
	int bin_count;
	struct sta_info *sta = file->private_data;
	int i;

	if (!sta->tx_lat)
		return -EINVAL;

	for (i = 0; i < IEEE80211_NUM_TIDS; i++) {
		bins = sta->tx_lat[i].bins;
		bin_count = sta->tx_lat[i].bin_count;

		sta->tx_lat[i].max = 0;
		sta->tx_lat[i].sum = 0;
		sta->tx_lat[i].counter = 0;

		if (bin_count)
			memset(bins, 0, bin_count * sizeof(u32));
	}

	return count;
}
STA_OPS_W(tx_latency_stat_reset);

#define DEBUGFS_ADD(name) \
	debugfs_create_file(#name, 0400, \
		sta->debugfs.dir, sta, &sta_ ##name## _ops);

#define DEBUGFS_ADD_COUNTER(name, field)				\
	if (sizeof(sta->field) == sizeof(u32))				\
		debugfs_create_u32(#name, 0400, sta->debugfs.dir,	\
			(u32 *) &sta->field);				\
	else								\
		debugfs_create_u64(#name, 0400, sta->debugfs.dir,	\
=======

#define DEBUGFS_ADD(name) \
	debugfs_create_file(#name, 0400, \
		sta->debugfs_dir, sta, &sta_ ##name## _ops);

#define DEBUGFS_ADD_COUNTER(name, field)				\
	if (sizeof(sta->field) == sizeof(u32))				\
		debugfs_create_u32(#name, 0400, sta->debugfs_dir,	\
			(u32 *) &sta->field);				\
	else								\
		debugfs_create_u64(#name, 0400, sta->debugfs_dir,	\
>>>>>>> v4.9.227
			(u64 *) &sta->field);

void ieee80211_sta_debugfs_add(struct sta_info *sta)
{
	struct ieee80211_local *local = sta->local;
	struct ieee80211_sub_if_data *sdata = sta->sdata;
	struct dentry *stations_dir = sta->sdata->debugfs.subdir_stations;
	u8 mac[3*ETH_ALEN];

<<<<<<< HEAD
	sta->debugfs.add_has_run = true;

=======
>>>>>>> v4.9.227
	if (!stations_dir)
		return;

	snprintf(mac, sizeof(mac), "%pM", sta->sta.addr);

	/*
	 * This might fail due to a race condition:
	 * When mac80211 unlinks a station, the debugfs entries
	 * remain, but it is already possible to link a new
	 * station with the same address which triggers adding
	 * it to debugfs; therefore, if the old station isn't
	 * destroyed quickly enough the old station's debugfs
	 * dir might still be around.
	 */
<<<<<<< HEAD
	sta->debugfs.dir = debugfs_create_dir(mac, stations_dir);
	if (!sta->debugfs.dir)
=======
	sta->debugfs_dir = debugfs_create_dir(mac, stations_dir);
	if (!sta->debugfs_dir)
>>>>>>> v4.9.227
		return;

	DEBUGFS_ADD(flags);
	DEBUGFS_ADD(num_ps_buf_frames);
<<<<<<< HEAD
	DEBUGFS_ADD(inactive_ms);
	DEBUGFS_ADD(connected_time);
	DEBUGFS_ADD(last_seq_ctrl);
	DEBUGFS_ADD(agg_status);
	DEBUGFS_ADD(dev);
	DEBUGFS_ADD(last_signal);
	DEBUGFS_ADD(beacon_loss_count);
	DEBUGFS_ADD(ht_capa);
	DEBUGFS_ADD(vht_capa);
	DEBUGFS_ADD(last_ack_signal);
	DEBUGFS_ADD(current_tx_rate);
	DEBUGFS_ADD(last_rx_rate);
	DEBUGFS_ADD(tx_latency_stat);
	DEBUGFS_ADD(tx_latency_stat_reset);

	DEBUGFS_ADD_COUNTER(rx_packets, rx_packets);
	DEBUGFS_ADD_COUNTER(tx_packets, tx_packets);
	DEBUGFS_ADD_COUNTER(rx_bytes, rx_bytes);
	DEBUGFS_ADD_COUNTER(tx_bytes, tx_bytes);
	DEBUGFS_ADD_COUNTER(rx_duplicates, num_duplicates);
	DEBUGFS_ADD_COUNTER(rx_fragments, rx_fragments);
	DEBUGFS_ADD_COUNTER(rx_dropped, rx_dropped);
	DEBUGFS_ADD_COUNTER(tx_fragments, tx_fragments);
	DEBUGFS_ADD_COUNTER(tx_filtered, tx_filtered_count);
	DEBUGFS_ADD_COUNTER(tx_retry_failed, tx_retry_failed);
	DEBUGFS_ADD_COUNTER(tx_retry_count, tx_retry_count);

	if (sizeof(sta->driver_buffered_tids) == sizeof(u32))
		debugfs_create_x32("driver_buffered_tids", 0400,
				   sta->debugfs.dir,
				   (u32 *)&sta->driver_buffered_tids);
	else
		debugfs_create_x64("driver_buffered_tids", 0400,
				   sta->debugfs.dir,
				   (u64 *)&sta->driver_buffered_tids);

	drv_sta_add_debugfs(local, sdata, &sta->sta, sta->debugfs.dir);
=======
	DEBUGFS_ADD(last_seq_ctrl);
	DEBUGFS_ADD(agg_status);
	DEBUGFS_ADD(ht_capa);
	DEBUGFS_ADD(vht_capa);

	DEBUGFS_ADD_COUNTER(rx_duplicates, rx_stats.num_duplicates);
	DEBUGFS_ADD_COUNTER(rx_fragments, rx_stats.fragments);
	DEBUGFS_ADD_COUNTER(tx_filtered, status_stats.filtered);

	if (local->ops->wake_tx_queue)
		DEBUGFS_ADD(aqm);

	if (sizeof(sta->driver_buffered_tids) == sizeof(u32))
		debugfs_create_x32("driver_buffered_tids", 0400,
				   sta->debugfs_dir,
				   (u32 *)&sta->driver_buffered_tids);
	else
		debugfs_create_x64("driver_buffered_tids", 0400,
				   sta->debugfs_dir,
				   (u64 *)&sta->driver_buffered_tids);

	drv_sta_add_debugfs(local, sdata, &sta->sta, sta->debugfs_dir);
>>>>>>> v4.9.227
}

void ieee80211_sta_debugfs_remove(struct sta_info *sta)
{
<<<<<<< HEAD
	struct ieee80211_local *local = sta->local;
	struct ieee80211_sub_if_data *sdata = sta->sdata;

	drv_sta_remove_debugfs(local, sdata, &sta->sta, sta->debugfs.dir);
	debugfs_remove_recursive(sta->debugfs.dir);
	sta->debugfs.dir = NULL;
=======
	debugfs_remove_recursive(sta->debugfs_dir);
	sta->debugfs_dir = NULL;
>>>>>>> v4.9.227
}
