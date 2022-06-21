/*
 * Copyright (c) 2005-2011 Atheros Communications Inc.
 * Copyright (c) 2011-2013 Qualcomm Atheros, Inc.
<<<<<<< HEAD
=======
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
>>>>>>> v4.9.227
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/module.h>
#include <linux/debugfs.h>
<<<<<<< HEAD
#include <linux/version.h>
#include <linux/vermagic.h>
#include <linux/vmalloc.h>

#include "core.h"
#include "debug.h"
=======
#include <linux/vmalloc.h>
#include <linux/utsname.h>
#include <linux/crc32.h>
#include <linux/firmware.h>

#include "core.h"
#include "debug.h"
#include "hif.h"
#include "wmi-ops.h"
>>>>>>> v4.9.227

/* ms */
#define ATH10K_DEBUG_HTT_STATS_INTERVAL 1000

<<<<<<< HEAD
=======
#define ATH10K_DEBUG_CAL_DATA_LEN 12064

>>>>>>> v4.9.227
#define ATH10K_FW_CRASH_DUMP_VERSION 1

/**
 * enum ath10k_fw_crash_dump_type - types of data in the dump file
 * @ATH10K_FW_CRASH_DUMP_REGDUMP: Register crash dump in binary format
 */
enum ath10k_fw_crash_dump_type {
	ATH10K_FW_CRASH_DUMP_REGISTERS = 0,

	ATH10K_FW_CRASH_DUMP_MAX,
};

struct ath10k_tlv_dump_data {
	/* see ath10k_fw_crash_dump_type above */
	__le32 type;

	/* in bytes */
	__le32 tlv_len;

	/* pad to 32-bit boundaries as needed */
	u8 tlv_data[];
} __packed;

struct ath10k_dump_file_data {
	/* dump file information */

	/* "ATH10K-FW-DUMP" */
	char df_magic[16];

	__le32 len;

	/* file dump version */
	__le32 version;

	/* some info we can get from ath10k struct that might help */

	u8 uuid[16];

	__le32 chip_id;

	/* 0 for now, in place for later hardware */
	__le32 bus_type;

	__le32 target_version;
	__le32 fw_version_major;
	__le32 fw_version_minor;
	__le32 fw_version_release;
	__le32 fw_version_build;
	__le32 phy_capability;
	__le32 hw_min_tx_power;
	__le32 hw_max_tx_power;
	__le32 ht_cap_info;
	__le32 vht_cap_info;
	__le32 num_rf_chains;

	/* firmware version string */
	char fw_ver[ETHTOOL_FWVERS_LEN];

	/* Kernel related information */

	/* time-of-day stamp */
	__le64 tv_sec;

	/* time-of-day stamp, nano-seconds */
	__le64 tv_nsec;

	/* LINUX_VERSION_CODE */
	__le32 kernel_ver_code;

	/* VERMAGIC_STRING */
	char kernel_ver[64];

	/* room for growth w/out changing binary format */
	u8 unused[128];

	/* struct ath10k_tlv_dump_data + more */
	u8 data[0];
} __packed;

<<<<<<< HEAD
int ath10k_info(struct ath10k *ar, const char *fmt, ...)
=======
void ath10k_info(struct ath10k *ar, const char *fmt, ...)
>>>>>>> v4.9.227
{
	struct va_format vaf = {
		.fmt = fmt,
	};
	va_list args;
<<<<<<< HEAD
	int ret;

	va_start(args, fmt);
	vaf.va = &args;
	ret = dev_info(ar->dev, "%pV", &vaf);
	trace_ath10k_log_info(ar, &vaf);
	va_end(args);

	return ret;
}
EXPORT_SYMBOL(ath10k_info);

void ath10k_print_driver_info(struct ath10k *ar)
{
	ath10k_info(ar, "%s (0x%08x, 0x%08x) fw %s api %d htt %d.%d\n",
		    ar->hw_params.name,
		    ar->target_version,
		    ar->chip_id,
		    ar->hw->wiphy->fw_version,
		    ar->fw_api,
		    ar->htt.target_version_major,
		    ar->htt.target_version_minor);
	ath10k_info(ar, "debug %d debugfs %d tracing %d dfs %d testmode %d\n",
		    config_enabled(CONFIG_ATH10K_DEBUG),
		    config_enabled(CONFIG_ATH10K_DEBUGFS),
		    config_enabled(CONFIG_ATH10K_TRACING),
		    config_enabled(CONFIG_ATH10K_DFS_CERTIFIED),
		    config_enabled(CONFIG_NL80211_TESTMODE));
}
EXPORT_SYMBOL(ath10k_print_driver_info);

int ath10k_err(struct ath10k *ar, const char *fmt, ...)
=======

	va_start(args, fmt);
	vaf.va = &args;
	dev_info(ar->dev, "%pV", &vaf);
	trace_ath10k_log_info(ar, &vaf);
	va_end(args);
}
EXPORT_SYMBOL(ath10k_info);

void ath10k_debug_print_hwfw_info(struct ath10k *ar)
{
	const struct firmware *firmware;
	char fw_features[128] = {};
	u32 crc = 0;

	ath10k_core_get_fw_features_str(ar, fw_features, sizeof(fw_features));

	ath10k_info(ar, "%s target 0x%08x chip_id 0x%08x sub %04x:%04x",
		    ar->hw_params.name,
		    ar->target_version,
		    ar->chip_id,
		    ar->id.subsystem_vendor, ar->id.subsystem_device);

	ath10k_info(ar, "kconfig debug %d debugfs %d tracing %d dfs %d testmode %d\n",
		    IS_ENABLED(CONFIG_ATH10K_DEBUG),
		    IS_ENABLED(CONFIG_ATH10K_DEBUGFS),
		    IS_ENABLED(CONFIG_ATH10K_TRACING),
		    IS_ENABLED(CONFIG_ATH10K_DFS_CERTIFIED),
		    IS_ENABLED(CONFIG_NL80211_TESTMODE));

	firmware = ar->normal_mode_fw.fw_file.firmware;
	if (firmware)
		crc = crc32_le(0, firmware->data, firmware->size);

	ath10k_info(ar, "firmware ver %s api %d features %s crc32 %08x\n",
		    ar->hw->wiphy->fw_version,
		    ar->fw_api,
		    fw_features,
		    crc);
}

void ath10k_debug_print_board_info(struct ath10k *ar)
{
	char boardinfo[100];
	const struct firmware *board;
	u32 crc;

	if (ar->id.bmi_ids_valid)
		scnprintf(boardinfo, sizeof(boardinfo), "%d:%d",
			  ar->id.bmi_chip_id, ar->id.bmi_board_id);
	else
		scnprintf(boardinfo, sizeof(boardinfo), "N/A");

	board = ar->normal_mode_fw.board;
	if (!IS_ERR_OR_NULL(board))
		crc = crc32_le(0, board->data, board->size);
	else
		crc = 0;

	ath10k_info(ar, "board_file api %d bmi_id %s crc32 %08x",
		    ar->bd_api,
		    boardinfo,
		    crc);
}

void ath10k_debug_print_boot_info(struct ath10k *ar)
{
	ath10k_info(ar, "htt-ver %d.%d wmi-op %d htt-op %d cal %s max-sta %d raw %d hwcrypto %d\n",
		    ar->htt.target_version_major,
		    ar->htt.target_version_minor,
		    ar->normal_mode_fw.fw_file.wmi_op_version,
		    ar->normal_mode_fw.fw_file.htt_op_version,
		    ath10k_cal_mode_str(ar->cal_mode),
		    ar->max_num_stations,
		    test_bit(ATH10K_FLAG_RAW_MODE, &ar->dev_flags),
		    !test_bit(ATH10K_FLAG_HW_CRYPTO_DISABLED, &ar->dev_flags));
}

void ath10k_print_driver_info(struct ath10k *ar)
{
	ath10k_debug_print_hwfw_info(ar);
	ath10k_debug_print_board_info(ar);
	ath10k_debug_print_boot_info(ar);
}
EXPORT_SYMBOL(ath10k_print_driver_info);

void ath10k_err(struct ath10k *ar, const char *fmt, ...)
>>>>>>> v4.9.227
{
	struct va_format vaf = {
		.fmt = fmt,
	};
	va_list args;
<<<<<<< HEAD
	int ret;

	va_start(args, fmt);
	vaf.va = &args;
	ret = dev_err(ar->dev, "%pV", &vaf);
	trace_ath10k_log_err(ar, &vaf);
	va_end(args);

	return ret;
}
EXPORT_SYMBOL(ath10k_err);

int ath10k_warn(struct ath10k *ar, const char *fmt, ...)
=======

	va_start(args, fmt);
	vaf.va = &args;
	dev_err(ar->dev, "%pV", &vaf);
	trace_ath10k_log_err(ar, &vaf);
	va_end(args);
}
EXPORT_SYMBOL(ath10k_err);

void ath10k_warn(struct ath10k *ar, const char *fmt, ...)
>>>>>>> v4.9.227
{
	struct va_format vaf = {
		.fmt = fmt,
	};
	va_list args;

	va_start(args, fmt);
	vaf.va = &args;
	dev_warn_ratelimited(ar->dev, "%pV", &vaf);
	trace_ath10k_log_warn(ar, &vaf);

	va_end(args);
<<<<<<< HEAD

	return 0;
=======
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(ath10k_warn);

#ifdef CONFIG_ATH10K_DEBUGFS

<<<<<<< HEAD
void ath10k_debug_read_service_map(struct ath10k *ar,
				   void *service_map,
				   size_t map_size)
{
	memcpy(ar->debug.wmi_service_bitmap, service_map, map_size);
}

=======
>>>>>>> v4.9.227
static ssize_t ath10k_read_wmi_services(struct file *file,
					char __user *user_buf,
					size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	char *buf;
	unsigned int len = 0, buf_len = 4096;
	const char *name;
	ssize_t ret_cnt;
	bool enabled;
	int i;

	buf = kzalloc(buf_len, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	mutex_lock(&ar->conf_mutex);

	if (len > buf_len)
		len = buf_len;

<<<<<<< HEAD
	for (i = 0; i < WMI_SERVICE_MAX; i++) {
		enabled = test_bit(i, ar->debug.wmi_service_bitmap);
=======
	spin_lock_bh(&ar->data_lock);
	for (i = 0; i < WMI_SERVICE_MAX; i++) {
		enabled = test_bit(i, ar->wmi.svc_map);
>>>>>>> v4.9.227
		name = wmi_service_name(i);

		if (!name) {
			if (enabled)
				len += scnprintf(buf + len, buf_len - len,
						 "%-40s %s (bit %d)\n",
						 "unknown", "enabled", i);

			continue;
		}

		len += scnprintf(buf + len, buf_len - len,
				 "%-40s %s\n",
				 name, enabled ? "enabled" : "-");
	}
<<<<<<< HEAD
=======
	spin_unlock_bh(&ar->data_lock);
>>>>>>> v4.9.227

	ret_cnt = simple_read_from_buffer(user_buf, count, ppos, buf, len);

	mutex_unlock(&ar->conf_mutex);

	kfree(buf);
	return ret_cnt;
}

static const struct file_operations fops_wmi_services = {
	.read = ath10k_read_wmi_services,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

<<<<<<< HEAD
void ath10k_debug_read_target_stats(struct ath10k *ar,
				    struct wmi_stats_event *ev)
{
	u8 *tmp = ev->data;
	struct ath10k_target_stats *stats;
	int num_pdev_stats, num_vdev_stats, num_peer_stats;
	struct wmi_pdev_stats_10x *ps;
	int i;

	spin_lock_bh(&ar->data_lock);

	stats = &ar->debug.target_stats;

	num_pdev_stats = __le32_to_cpu(ev->num_pdev_stats); /* 0 or 1 */
	num_vdev_stats = __le32_to_cpu(ev->num_vdev_stats); /* 0 or max vdevs */
	num_peer_stats = __le32_to_cpu(ev->num_peer_stats); /* 0 or max peers */

	if (num_pdev_stats) {
		ps = (struct wmi_pdev_stats_10x *)tmp;

		stats->ch_noise_floor = __le32_to_cpu(ps->chan_nf);
		stats->tx_frame_count = __le32_to_cpu(ps->tx_frame_count);
		stats->rx_frame_count = __le32_to_cpu(ps->rx_frame_count);
		stats->rx_clear_count = __le32_to_cpu(ps->rx_clear_count);
		stats->cycle_count = __le32_to_cpu(ps->cycle_count);
		stats->phy_err_count = __le32_to_cpu(ps->phy_err_count);
		stats->chan_tx_power = __le32_to_cpu(ps->chan_tx_pwr);

		stats->comp_queued = __le32_to_cpu(ps->wal.tx.comp_queued);
		stats->comp_delivered =
			__le32_to_cpu(ps->wal.tx.comp_delivered);
		stats->msdu_enqued = __le32_to_cpu(ps->wal.tx.msdu_enqued);
		stats->mpdu_enqued = __le32_to_cpu(ps->wal.tx.mpdu_enqued);
		stats->wmm_drop = __le32_to_cpu(ps->wal.tx.wmm_drop);
		stats->local_enqued = __le32_to_cpu(ps->wal.tx.local_enqued);
		stats->local_freed = __le32_to_cpu(ps->wal.tx.local_freed);
		stats->hw_queued = __le32_to_cpu(ps->wal.tx.hw_queued);
		stats->hw_reaped = __le32_to_cpu(ps->wal.tx.hw_reaped);
		stats->underrun = __le32_to_cpu(ps->wal.tx.underrun);
		stats->tx_abort = __le32_to_cpu(ps->wal.tx.tx_abort);
		stats->mpdus_requed = __le32_to_cpu(ps->wal.tx.mpdus_requed);
		stats->tx_ko = __le32_to_cpu(ps->wal.tx.tx_ko);
		stats->data_rc = __le32_to_cpu(ps->wal.tx.data_rc);
		stats->self_triggers = __le32_to_cpu(ps->wal.tx.self_triggers);
		stats->sw_retry_failure =
			__le32_to_cpu(ps->wal.tx.sw_retry_failure);
		stats->illgl_rate_phy_err =
			__le32_to_cpu(ps->wal.tx.illgl_rate_phy_err);
		stats->pdev_cont_xretry =
			__le32_to_cpu(ps->wal.tx.pdev_cont_xretry);
		stats->pdev_tx_timeout =
			__le32_to_cpu(ps->wal.tx.pdev_tx_timeout);
		stats->pdev_resets = __le32_to_cpu(ps->wal.tx.pdev_resets);
		stats->phy_underrun = __le32_to_cpu(ps->wal.tx.phy_underrun);
		stats->txop_ovf = __le32_to_cpu(ps->wal.tx.txop_ovf);

		stats->mid_ppdu_route_change =
			__le32_to_cpu(ps->wal.rx.mid_ppdu_route_change);
		stats->status_rcvd = __le32_to_cpu(ps->wal.rx.status_rcvd);
		stats->r0_frags = __le32_to_cpu(ps->wal.rx.r0_frags);
		stats->r1_frags = __le32_to_cpu(ps->wal.rx.r1_frags);
		stats->r2_frags = __le32_to_cpu(ps->wal.rx.r2_frags);
		stats->r3_frags = __le32_to_cpu(ps->wal.rx.r3_frags);
		stats->htt_msdus = __le32_to_cpu(ps->wal.rx.htt_msdus);
		stats->htt_mpdus = __le32_to_cpu(ps->wal.rx.htt_mpdus);
		stats->loc_msdus = __le32_to_cpu(ps->wal.rx.loc_msdus);
		stats->loc_mpdus = __le32_to_cpu(ps->wal.rx.loc_mpdus);
		stats->oversize_amsdu =
			__le32_to_cpu(ps->wal.rx.oversize_amsdu);
		stats->phy_errs = __le32_to_cpu(ps->wal.rx.phy_errs);
		stats->phy_err_drop = __le32_to_cpu(ps->wal.rx.phy_err_drop);
		stats->mpdu_errs = __le32_to_cpu(ps->wal.rx.mpdu_errs);

		if (test_bit(ATH10K_FW_FEATURE_WMI_10X,
			     ar->fw_features)) {
			stats->ack_rx_bad = __le32_to_cpu(ps->ack_rx_bad);
			stats->rts_bad = __le32_to_cpu(ps->rts_bad);
			stats->rts_good = __le32_to_cpu(ps->rts_good);
			stats->fcs_bad = __le32_to_cpu(ps->fcs_bad);
			stats->no_beacons = __le32_to_cpu(ps->no_beacons);
			stats->mib_int_count = __le32_to_cpu(ps->mib_int_count);
			tmp += sizeof(struct wmi_pdev_stats_10x);
		} else {
			tmp += sizeof(struct wmi_pdev_stats_old);
		}
	}

	/* 0 or max vdevs */
	/* Currently firmware does not support VDEV stats */
	if (num_vdev_stats) {
		struct wmi_vdev_stats *vdev_stats;

		for (i = 0; i < num_vdev_stats; i++) {
			vdev_stats = (struct wmi_vdev_stats *)tmp;
			tmp += sizeof(struct wmi_vdev_stats);
		}
	}

	if (num_peer_stats) {
		struct wmi_peer_stats_10x *peer_stats;
		struct ath10k_peer_stat *s;

		stats->peers = num_peer_stats;

		for (i = 0; i < num_peer_stats; i++) {
			peer_stats = (struct wmi_peer_stats_10x *)tmp;
			s = &stats->peer_stat[i];

			memcpy(s->peer_macaddr, &peer_stats->peer_macaddr.addr,
			       ETH_ALEN);
			s->peer_rssi = __le32_to_cpu(peer_stats->peer_rssi);
			s->peer_tx_rate =
				__le32_to_cpu(peer_stats->peer_tx_rate);
			if (test_bit(ATH10K_FW_FEATURE_WMI_10X,
				     ar->fw_features)) {
				s->peer_rx_rate =
					__le32_to_cpu(peer_stats->peer_rx_rate);
				tmp += sizeof(struct wmi_peer_stats_10x);

			} else {
				tmp += sizeof(struct wmi_peer_stats_old);
			}
		}
	}

	spin_unlock_bh(&ar->data_lock);
	complete(&ar->debug.event_stats_compl);
}

static ssize_t ath10k_read_fw_stats(struct file *file, char __user *user_buf,
				    size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	struct ath10k_target_stats *fw_stats;
	char *buf = NULL;
	unsigned int len = 0, buf_len = 8000;
	ssize_t ret_cnt = 0;
	long left;
	int i;
	int ret;

	fw_stats = &ar->debug.target_stats;

	mutex_lock(&ar->conf_mutex);

	if (ar->state != ATH10K_STATE_ON)
		goto exit;

	buf = kzalloc(buf_len, GFP_KERNEL);
	if (!buf)
		goto exit;

	ret = ath10k_wmi_request_stats(ar, WMI_REQUEST_PEER_STAT);
	if (ret) {
		ath10k_warn(ar, "could not request stats (%d)\n", ret);
		goto exit;
	}

	left = wait_for_completion_timeout(&ar->debug.event_stats_compl, 1*HZ);
	if (left <= 0)
		goto exit;

	spin_lock_bh(&ar->data_lock);
	len += scnprintf(buf + len, buf_len - len, "\n");
	len += scnprintf(buf + len, buf_len - len, "%30s\n",
			 "ath10k PDEV stats");
	len += scnprintf(buf + len, buf_len - len, "%30s\n\n",
				 "=================");

	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Channel noise floor", fw_stats->ch_noise_floor);
	len += scnprintf(buf + len, buf_len - len, "%30s %10u\n",
			 "Channel TX power", fw_stats->chan_tx_power);
	len += scnprintf(buf + len, buf_len - len, "%30s %10u\n",
			 "TX frame count", fw_stats->tx_frame_count);
	len += scnprintf(buf + len, buf_len - len, "%30s %10u\n",
			 "RX frame count", fw_stats->rx_frame_count);
	len += scnprintf(buf + len, buf_len - len, "%30s %10u\n",
			 "RX clear count", fw_stats->rx_clear_count);
	len += scnprintf(buf + len, buf_len - len, "%30s %10u\n",
			 "Cycle count", fw_stats->cycle_count);
	len += scnprintf(buf + len, buf_len - len, "%30s %10u\n",
			 "PHY error count", fw_stats->phy_err_count);
	len += scnprintf(buf + len, buf_len - len, "%30s %10u\n",
			 "RTS bad count", fw_stats->rts_bad);
	len += scnprintf(buf + len, buf_len - len, "%30s %10u\n",
			 "RTS good count", fw_stats->rts_good);
	len += scnprintf(buf + len, buf_len - len, "%30s %10u\n",
			 "FCS bad count", fw_stats->fcs_bad);
	len += scnprintf(buf + len, buf_len - len, "%30s %10u\n",
			 "No beacon count", fw_stats->no_beacons);
	len += scnprintf(buf + len, buf_len - len, "%30s %10u\n",
			 "MIB int count", fw_stats->mib_int_count);

	len += scnprintf(buf + len, buf_len - len, "\n");
	len += scnprintf(buf + len, buf_len - len, "%30s\n",
			 "ath10k PDEV TX stats");
	len += scnprintf(buf + len, buf_len - len, "%30s\n\n",
				 "=================");

	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "HTT cookies queued", fw_stats->comp_queued);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "HTT cookies disp.", fw_stats->comp_delivered);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "MSDU queued", fw_stats->msdu_enqued);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "MPDU queued", fw_stats->mpdu_enqued);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "MSDUs dropped", fw_stats->wmm_drop);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Local enqued", fw_stats->local_enqued);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Local freed", fw_stats->local_freed);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "HW queued", fw_stats->hw_queued);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "PPDUs reaped", fw_stats->hw_reaped);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Num underruns", fw_stats->underrun);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "PPDUs cleaned", fw_stats->tx_abort);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "MPDUs requed", fw_stats->mpdus_requed);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Excessive retries", fw_stats->tx_ko);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "HW rate", fw_stats->data_rc);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Sched self tiggers", fw_stats->self_triggers);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Dropped due to SW retries",
			 fw_stats->sw_retry_failure);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Illegal rate phy errors",
			 fw_stats->illgl_rate_phy_err);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Pdev continous xretry", fw_stats->pdev_cont_xretry);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "TX timeout", fw_stats->pdev_tx_timeout);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "PDEV resets", fw_stats->pdev_resets);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "PHY underrun", fw_stats->phy_underrun);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "MPDU is more than txop limit", fw_stats->txop_ovf);

	len += scnprintf(buf + len, buf_len - len, "\n");
	len += scnprintf(buf + len, buf_len - len, "%30s\n",
			 "ath10k PDEV RX stats");
	len += scnprintf(buf + len, buf_len - len, "%30s\n\n",
				 "=================");

	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Mid PPDU route change",
			 fw_stats->mid_ppdu_route_change);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Tot. number of statuses", fw_stats->status_rcvd);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Extra frags on rings 0", fw_stats->r0_frags);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Extra frags on rings 1", fw_stats->r1_frags);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Extra frags on rings 2", fw_stats->r2_frags);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Extra frags on rings 3", fw_stats->r3_frags);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "MSDUs delivered to HTT", fw_stats->htt_msdus);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "MPDUs delivered to HTT", fw_stats->htt_mpdus);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "MSDUs delivered to stack", fw_stats->loc_msdus);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "MPDUs delivered to stack", fw_stats->loc_mpdus);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "Oversized AMSUs", fw_stats->oversize_amsdu);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "PHY errors", fw_stats->phy_errs);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "PHY errors drops", fw_stats->phy_err_drop);
	len += scnprintf(buf + len, buf_len - len, "%30s %10d\n",
			 "MPDU errors (FCS, MIC, ENC)", fw_stats->mpdu_errs);

	len += scnprintf(buf + len, buf_len - len, "\n");
	len += scnprintf(buf + len, buf_len - len, "%30s (%d)\n",
			 "ath10k PEER stats", fw_stats->peers);
	len += scnprintf(buf + len, buf_len - len, "%30s\n\n",
				 "=================");

	for (i = 0; i < fw_stats->peers; i++) {
		len += scnprintf(buf + len, buf_len - len, "%30s %pM\n",
				 "Peer MAC address",
				 fw_stats->peer_stat[i].peer_macaddr);
		len += scnprintf(buf + len, buf_len - len, "%30s %u\n",
				 "Peer RSSI", fw_stats->peer_stat[i].peer_rssi);
		len += scnprintf(buf + len, buf_len - len, "%30s %u\n",
				 "Peer TX rate",
				 fw_stats->peer_stat[i].peer_tx_rate);
		len += scnprintf(buf + len, buf_len - len, "%30s %u\n",
				 "Peer RX rate",
				 fw_stats->peer_stat[i].peer_rx_rate);
		len += scnprintf(buf + len, buf_len - len, "\n");
	}
	spin_unlock_bh(&ar->data_lock);

	if (len > buf_len)
		len = buf_len;

	ret_cnt = simple_read_from_buffer(user_buf, count, ppos, buf, len);

exit:
	mutex_unlock(&ar->conf_mutex);
	kfree(buf);
	return ret_cnt;
}

static const struct file_operations fops_fw_stats = {
	.read = ath10k_read_fw_stats,
	.open = simple_open,
=======
static void ath10k_fw_stats_pdevs_free(struct list_head *head)
{
	struct ath10k_fw_stats_pdev *i, *tmp;

	list_for_each_entry_safe(i, tmp, head, list) {
		list_del(&i->list);
		kfree(i);
	}
}

static void ath10k_fw_stats_vdevs_free(struct list_head *head)
{
	struct ath10k_fw_stats_vdev *i, *tmp;

	list_for_each_entry_safe(i, tmp, head, list) {
		list_del(&i->list);
		kfree(i);
	}
}

static void ath10k_fw_stats_peers_free(struct list_head *head)
{
	struct ath10k_fw_stats_peer *i, *tmp;

	list_for_each_entry_safe(i, tmp, head, list) {
		list_del(&i->list);
		kfree(i);
	}
}

static void ath10k_fw_extd_stats_peers_free(struct list_head *head)
{
	struct ath10k_fw_extd_stats_peer *i, *tmp;

	list_for_each_entry_safe(i, tmp, head, list) {
		list_del(&i->list);
		kfree(i);
	}
}

static void ath10k_debug_fw_stats_reset(struct ath10k *ar)
{
	spin_lock_bh(&ar->data_lock);
	ar->debug.fw_stats_done = false;
	ar->debug.fw_stats.extended = false;
	ath10k_fw_stats_pdevs_free(&ar->debug.fw_stats.pdevs);
	ath10k_fw_stats_vdevs_free(&ar->debug.fw_stats.vdevs);
	ath10k_fw_stats_peers_free(&ar->debug.fw_stats.peers);
	ath10k_fw_extd_stats_peers_free(&ar->debug.fw_stats.peers_extd);
	spin_unlock_bh(&ar->data_lock);
}

void ath10k_debug_fw_stats_process(struct ath10k *ar, struct sk_buff *skb)
{
	struct ath10k_fw_stats stats = {};
	bool is_start, is_started, is_end;
	size_t num_peers;
	size_t num_vdevs;
	int ret;

	INIT_LIST_HEAD(&stats.pdevs);
	INIT_LIST_HEAD(&stats.vdevs);
	INIT_LIST_HEAD(&stats.peers);
	INIT_LIST_HEAD(&stats.peers_extd);

	spin_lock_bh(&ar->data_lock);
	ret = ath10k_wmi_pull_fw_stats(ar, skb, &stats);
	if (ret) {
		ath10k_warn(ar, "failed to pull fw stats: %d\n", ret);
		goto free;
	}

	/* Stat data may exceed htc-wmi buffer limit. In such case firmware
	 * splits the stats data and delivers it in a ping-pong fashion of
	 * request cmd-update event.
	 *
	 * However there is no explicit end-of-data. Instead start-of-data is
	 * used as an implicit one. This works as follows:
	 *  a) discard stat update events until one with pdev stats is
	 *     delivered - this skips session started at end of (b)
	 *  b) consume stat update events until another one with pdev stats is
	 *     delivered which is treated as end-of-data and is itself discarded
	 */
	if (ath10k_peer_stats_enabled(ar))
		ath10k_sta_update_rx_duration(ar, &stats);

	if (ar->debug.fw_stats_done) {
		if (!ath10k_peer_stats_enabled(ar))
			ath10k_warn(ar, "received unsolicited stats update event\n");

		goto free;
	}

	num_peers = ath10k_wmi_fw_stats_num_peers(&ar->debug.fw_stats.peers);
	num_vdevs = ath10k_wmi_fw_stats_num_vdevs(&ar->debug.fw_stats.vdevs);
	is_start = (list_empty(&ar->debug.fw_stats.pdevs) &&
		    !list_empty(&stats.pdevs));
	is_end = (!list_empty(&ar->debug.fw_stats.pdevs) &&
		  !list_empty(&stats.pdevs));

	if (is_start)
		list_splice_tail_init(&stats.pdevs, &ar->debug.fw_stats.pdevs);

	if (is_end)
		ar->debug.fw_stats_done = true;

	is_started = !list_empty(&ar->debug.fw_stats.pdevs);

	if (is_started && !is_end) {
		if (num_peers >= ATH10K_MAX_NUM_PEER_IDS) {
			/* Although this is unlikely impose a sane limit to
			 * prevent firmware from DoS-ing the host.
			 */
			ath10k_fw_stats_peers_free(&ar->debug.fw_stats.peers);
			ath10k_warn(ar, "dropping fw peer stats\n");
			goto free;
		}

		if (num_vdevs >= BITS_PER_LONG) {
			ath10k_fw_stats_vdevs_free(&ar->debug.fw_stats.vdevs);
			ath10k_warn(ar, "dropping fw vdev stats\n");
			goto free;
		}

		list_splice_tail_init(&stats.peers, &ar->debug.fw_stats.peers);
		list_splice_tail_init(&stats.vdevs, &ar->debug.fw_stats.vdevs);
		list_splice_tail_init(&stats.peers_extd,
				      &ar->debug.fw_stats.peers_extd);
	}

	complete(&ar->debug.fw_stats_complete);

free:
	/* In some cases lists have been spliced and cleared. Free up
	 * resources if that is not the case.
	 */
	ath10k_fw_stats_pdevs_free(&stats.pdevs);
	ath10k_fw_stats_vdevs_free(&stats.vdevs);
	ath10k_fw_stats_peers_free(&stats.peers);
	ath10k_fw_extd_stats_peers_free(&stats.peers_extd);

	spin_unlock_bh(&ar->data_lock);
}

static int ath10k_debug_fw_stats_request(struct ath10k *ar)
{
	unsigned long timeout, time_left;
	int ret;

	lockdep_assert_held(&ar->conf_mutex);

	timeout = jiffies + msecs_to_jiffies(1 * HZ);

	ath10k_debug_fw_stats_reset(ar);

	for (;;) {
		if (time_after(jiffies, timeout))
			return -ETIMEDOUT;

		reinit_completion(&ar->debug.fw_stats_complete);

		ret = ath10k_wmi_request_stats(ar, ar->fw_stats_req_mask);
		if (ret) {
			ath10k_warn(ar, "could not request stats (%d)\n", ret);
			return ret;
		}

		time_left =
		wait_for_completion_timeout(&ar->debug.fw_stats_complete,
					    1 * HZ);
		if (!time_left)
			return -ETIMEDOUT;

		spin_lock_bh(&ar->data_lock);
		if (ar->debug.fw_stats_done) {
			spin_unlock_bh(&ar->data_lock);
			break;
		}
		spin_unlock_bh(&ar->data_lock);
	}

	return 0;
}

static int ath10k_fw_stats_open(struct inode *inode, struct file *file)
{
	struct ath10k *ar = inode->i_private;
	void *buf = NULL;
	int ret;

	mutex_lock(&ar->conf_mutex);

	if (ar->state != ATH10K_STATE_ON) {
		ret = -ENETDOWN;
		goto err_unlock;
	}

	buf = vmalloc(ATH10K_FW_STATS_BUF_SIZE);
	if (!buf) {
		ret = -ENOMEM;
		goto err_unlock;
	}

	ret = ath10k_debug_fw_stats_request(ar);
	if (ret) {
		ath10k_warn(ar, "failed to request fw stats: %d\n", ret);
		goto err_free;
	}

	ret = ath10k_wmi_fw_stats_fill(ar, &ar->debug.fw_stats, buf);
	if (ret) {
		ath10k_warn(ar, "failed to fill fw stats: %d\n", ret);
		goto err_free;
	}

	file->private_data = buf;

	mutex_unlock(&ar->conf_mutex);
	return 0;

err_free:
	vfree(buf);

err_unlock:
	mutex_unlock(&ar->conf_mutex);
	return ret;
}

static int ath10k_fw_stats_release(struct inode *inode, struct file *file)
{
	vfree(file->private_data);

	return 0;
}

static ssize_t ath10k_fw_stats_read(struct file *file, char __user *user_buf,
				    size_t count, loff_t *ppos)
{
	const char *buf = file->private_data;
	unsigned int len = strlen(buf);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static const struct file_operations fops_fw_stats = {
	.open = ath10k_fw_stats_open,
	.release = ath10k_fw_stats_release,
	.read = ath10k_fw_stats_read,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static ssize_t ath10k_debug_fw_reset_stats_read(struct file *file,
						char __user *user_buf,
						size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	int ret, len, buf_len;
	char *buf;

	buf_len = 500;
	buf = kmalloc(buf_len, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	spin_lock_bh(&ar->data_lock);

	len = 0;
	len += scnprintf(buf + len, buf_len - len,
			 "fw_crash_counter\t\t%d\n", ar->stats.fw_crash_counter);
	len += scnprintf(buf + len, buf_len - len,
			 "fw_warm_reset_counter\t\t%d\n",
			 ar->stats.fw_warm_reset_counter);
	len += scnprintf(buf + len, buf_len - len,
			 "fw_cold_reset_counter\t\t%d\n",
			 ar->stats.fw_cold_reset_counter);

	spin_unlock_bh(&ar->data_lock);

	ret = simple_read_from_buffer(user_buf, count, ppos, buf, len);

	kfree(buf);

	return ret;
}

static const struct file_operations fops_fw_reset_stats = {
	.open = simple_open,
	.read = ath10k_debug_fw_reset_stats_read,
>>>>>>> v4.9.227
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

/* This is a clean assert crash in firmware. */
static int ath10k_debug_fw_assert(struct ath10k *ar)
{
	struct wmi_vdev_install_key_cmd *cmd;
	struct sk_buff *skb;

	skb = ath10k_wmi_alloc_skb(ar, sizeof(*cmd) + 16);
	if (!skb)
		return -ENOMEM;

	cmd = (struct wmi_vdev_install_key_cmd *)skb->data;
	memset(cmd, 0, sizeof(*cmd));

	/* big enough number so that firmware asserts */
	cmd->vdev_id = __cpu_to_le32(0x7ffe);

	return ath10k_wmi_cmd_send(ar, skb,
				   ar->wmi.cmd->vdev_install_key_cmdid);
}

static ssize_t ath10k_read_simulate_fw_crash(struct file *file,
					     char __user *user_buf,
					     size_t count, loff_t *ppos)
{
	const char buf[] =
		"To simulate firmware crash write one of the keywords to this file:\n"
		"`soft` - this will send WMI_FORCE_FW_HANG_ASSERT to firmware if FW supports that command.\n"
		"`hard` - this will send to firmware command with illegal parameters causing firmware crash.\n"
<<<<<<< HEAD
		"`assert` - this will send special illegal parameter to firmware to cause assert failure and crash.\n";
=======
		"`assert` - this will send special illegal parameter to firmware to cause assert failure and crash.\n"
		"`hw-restart` - this will simply queue hw restart without fw/hw actually crashing.\n";
>>>>>>> v4.9.227

	return simple_read_from_buffer(user_buf, count, ppos, buf, strlen(buf));
}

/* Simulate firmware crash:
 * 'soft': Call wmi command causing firmware hang. This firmware hang is
 * recoverable by warm firmware reset.
 * 'hard': Force firmware crash by setting any vdev parameter for not allowed
 * vdev id. This is hard firmware crash because it is recoverable only by cold
 * firmware reset.
 */
static ssize_t ath10k_write_simulate_fw_crash(struct file *file,
					      const char __user *user_buf,
					      size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
<<<<<<< HEAD
	char buf[32];
	int ret;

	mutex_lock(&ar->conf_mutex);

	simple_write_to_buffer(buf, sizeof(buf) - 1, ppos, user_buf, count);

	/* make sure that buf is null terminated */
	buf[sizeof(buf) - 1] = 0;

=======
	char buf[32] = {0};
	ssize_t rc;
	int ret;

	/* filter partial writes and invalid commands */
	if (*ppos != 0 || count >= sizeof(buf) || count == 0)
		return -EINVAL;

	rc = simple_write_to_buffer(buf, sizeof(buf) - 1, ppos, user_buf, count);
	if (rc < 0)
		return rc;

	/* drop the possible '\n' from the end */
	if (buf[*ppos - 1] == '\n')
		buf[*ppos - 1] = '\0';

	mutex_lock(&ar->conf_mutex);

>>>>>>> v4.9.227
	if (ar->state != ATH10K_STATE_ON &&
	    ar->state != ATH10K_STATE_RESTARTED) {
		ret = -ENETDOWN;
		goto exit;
	}

<<<<<<< HEAD
	/* drop the possible '\n' from the end */
	if (buf[count - 1] == '\n') {
		buf[count - 1] = 0;
		count--;
	}

=======
>>>>>>> v4.9.227
	if (!strcmp(buf, "soft")) {
		ath10k_info(ar, "simulating soft firmware crash\n");
		ret = ath10k_wmi_force_fw_hang(ar, WMI_FORCE_FW_HANG_ASSERT, 0);
	} else if (!strcmp(buf, "hard")) {
		ath10k_info(ar, "simulating hard firmware crash\n");
		/* 0x7fff is vdev id, and it is always out of range for all
		 * firmware variants in order to force a firmware crash.
		 */
		ret = ath10k_wmi_vdev_set_param(ar, 0x7fff,
						ar->wmi.vdev_param->rts_threshold,
						0);
	} else if (!strcmp(buf, "assert")) {
		ath10k_info(ar, "simulating firmware assert crash\n");
		ret = ath10k_debug_fw_assert(ar);
<<<<<<< HEAD
=======
	} else if (!strcmp(buf, "hw-restart")) {
		ath10k_info(ar, "user requested hw restart\n");
		queue_work(ar->workqueue, &ar->restart_work);
		ret = 0;
>>>>>>> v4.9.227
	} else {
		ret = -EINVAL;
		goto exit;
	}

	if (ret) {
		ath10k_warn(ar, "failed to simulate firmware crash: %d\n", ret);
		goto exit;
	}

	ret = count;

exit:
	mutex_unlock(&ar->conf_mutex);
	return ret;
}

static const struct file_operations fops_simulate_fw_crash = {
	.read = ath10k_read_simulate_fw_crash,
	.write = ath10k_write_simulate_fw_crash,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static ssize_t ath10k_read_chip_id(struct file *file, char __user *user_buf,
				   size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	unsigned int len;
	char buf[50];

	len = scnprintf(buf, sizeof(buf), "0x%08x\n", ar->chip_id);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static const struct file_operations fops_chip_id = {
	.read = ath10k_read_chip_id,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

struct ath10k_fw_crash_data *
ath10k_debug_get_new_fw_crash_data(struct ath10k *ar)
{
	struct ath10k_fw_crash_data *crash_data = ar->debug.fw_crash_data;

	lockdep_assert_held(&ar->data_lock);

	crash_data->crashed_since_read = true;
	uuid_le_gen(&crash_data->uuid);
	getnstimeofday(&crash_data->timestamp);

	return crash_data;
}
EXPORT_SYMBOL(ath10k_debug_get_new_fw_crash_data);

static struct ath10k_dump_file_data *ath10k_build_dump_file(struct ath10k *ar)
{
	struct ath10k_fw_crash_data *crash_data = ar->debug.fw_crash_data;
	struct ath10k_dump_file_data *dump_data;
	struct ath10k_tlv_dump_data *dump_tlv;
	int hdr_len = sizeof(*dump_data);
	unsigned int len, sofar = 0;
	unsigned char *buf;

	len = hdr_len;
	len += sizeof(*dump_tlv) + sizeof(crash_data->registers);

	sofar += hdr_len;

	/* This is going to get big when we start dumping FW RAM and such,
	 * so go ahead and use vmalloc.
	 */
	buf = vzalloc(len);
	if (!buf)
		return NULL;

	spin_lock_bh(&ar->data_lock);

	if (!crash_data->crashed_since_read) {
		spin_unlock_bh(&ar->data_lock);
		vfree(buf);
		return NULL;
	}

	dump_data = (struct ath10k_dump_file_data *)(buf);
	strlcpy(dump_data->df_magic, "ATH10K-FW-DUMP",
		sizeof(dump_data->df_magic));
	dump_data->len = cpu_to_le32(len);

	dump_data->version = cpu_to_le32(ATH10K_FW_CRASH_DUMP_VERSION);

	memcpy(dump_data->uuid, &crash_data->uuid, sizeof(dump_data->uuid));
	dump_data->chip_id = cpu_to_le32(ar->chip_id);
	dump_data->bus_type = cpu_to_le32(0);
	dump_data->target_version = cpu_to_le32(ar->target_version);
	dump_data->fw_version_major = cpu_to_le32(ar->fw_version_major);
	dump_data->fw_version_minor = cpu_to_le32(ar->fw_version_minor);
	dump_data->fw_version_release = cpu_to_le32(ar->fw_version_release);
	dump_data->fw_version_build = cpu_to_le32(ar->fw_version_build);
	dump_data->phy_capability = cpu_to_le32(ar->phy_capability);
	dump_data->hw_min_tx_power = cpu_to_le32(ar->hw_min_tx_power);
	dump_data->hw_max_tx_power = cpu_to_le32(ar->hw_max_tx_power);
	dump_data->ht_cap_info = cpu_to_le32(ar->ht_cap_info);
	dump_data->vht_cap_info = cpu_to_le32(ar->vht_cap_info);
	dump_data->num_rf_chains = cpu_to_le32(ar->num_rf_chains);

	strlcpy(dump_data->fw_ver, ar->hw->wiphy->fw_version,
		sizeof(dump_data->fw_ver));

<<<<<<< HEAD
	dump_data->kernel_ver_code = cpu_to_le32(LINUX_VERSION_CODE);
	strlcpy(dump_data->kernel_ver, VERMAGIC_STRING,
=======
	dump_data->kernel_ver_code = 0;
	strlcpy(dump_data->kernel_ver, init_utsname()->release,
>>>>>>> v4.9.227
		sizeof(dump_data->kernel_ver));

	dump_data->tv_sec = cpu_to_le64(crash_data->timestamp.tv_sec);
	dump_data->tv_nsec = cpu_to_le64(crash_data->timestamp.tv_nsec);

	/* Gather crash-dump */
	dump_tlv = (struct ath10k_tlv_dump_data *)(buf + sofar);
	dump_tlv->type = cpu_to_le32(ATH10K_FW_CRASH_DUMP_REGISTERS);
	dump_tlv->tlv_len = cpu_to_le32(sizeof(crash_data->registers));
	memcpy(dump_tlv->tlv_data, &crash_data->registers,
	       sizeof(crash_data->registers));
	sofar += sizeof(*dump_tlv) + sizeof(crash_data->registers);

	ar->debug.fw_crash_data->crashed_since_read = false;

	spin_unlock_bh(&ar->data_lock);

	return dump_data;
}

static int ath10k_fw_crash_dump_open(struct inode *inode, struct file *file)
{
	struct ath10k *ar = inode->i_private;
	struct ath10k_dump_file_data *dump;

	dump = ath10k_build_dump_file(ar);
	if (!dump)
		return -ENODATA;

	file->private_data = dump;

	return 0;
}

static ssize_t ath10k_fw_crash_dump_read(struct file *file,
					 char __user *user_buf,
					 size_t count, loff_t *ppos)
{
	struct ath10k_dump_file_data *dump_file = file->private_data;

	return simple_read_from_buffer(user_buf, count, ppos,
				       dump_file,
				       le32_to_cpu(dump_file->len));
}

static int ath10k_fw_crash_dump_release(struct inode *inode,
					struct file *file)
{
	vfree(file->private_data);

	return 0;
}

static const struct file_operations fops_fw_crash_dump = {
	.open = ath10k_fw_crash_dump_open,
	.read = ath10k_fw_crash_dump_read,
	.release = ath10k_fw_crash_dump_release,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

<<<<<<< HEAD
=======
static ssize_t ath10k_reg_addr_read(struct file *file,
				    char __user *user_buf,
				    size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	u8 buf[32];
	unsigned int len = 0;
	u32 reg_addr;

	mutex_lock(&ar->conf_mutex);
	reg_addr = ar->debug.reg_addr;
	mutex_unlock(&ar->conf_mutex);

	len += scnprintf(buf + len, sizeof(buf) - len, "0x%x\n", reg_addr);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t ath10k_reg_addr_write(struct file *file,
				     const char __user *user_buf,
				     size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	u32 reg_addr;
	int ret;

	ret = kstrtou32_from_user(user_buf, count, 0, &reg_addr);
	if (ret)
		return ret;

	if (!IS_ALIGNED(reg_addr, 4))
		return -EFAULT;

	mutex_lock(&ar->conf_mutex);
	ar->debug.reg_addr = reg_addr;
	mutex_unlock(&ar->conf_mutex);

	return count;
}

static const struct file_operations fops_reg_addr = {
	.read = ath10k_reg_addr_read,
	.write = ath10k_reg_addr_write,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static ssize_t ath10k_reg_value_read(struct file *file,
				     char __user *user_buf,
				     size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	u8 buf[48];
	unsigned int len;
	u32 reg_addr, reg_val;
	int ret;

	mutex_lock(&ar->conf_mutex);

	if (ar->state != ATH10K_STATE_ON &&
	    ar->state != ATH10K_STATE_UTF) {
		ret = -ENETDOWN;
		goto exit;
	}

	reg_addr = ar->debug.reg_addr;

	reg_val = ath10k_hif_read32(ar, reg_addr);
	len = scnprintf(buf, sizeof(buf), "0x%08x:0x%08x\n", reg_addr, reg_val);

	ret = simple_read_from_buffer(user_buf, count, ppos, buf, len);

exit:
	mutex_unlock(&ar->conf_mutex);

	return ret;
}

static ssize_t ath10k_reg_value_write(struct file *file,
				      const char __user *user_buf,
				      size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	u32 reg_addr, reg_val;
	int ret;

	mutex_lock(&ar->conf_mutex);

	if (ar->state != ATH10K_STATE_ON &&
	    ar->state != ATH10K_STATE_UTF) {
		ret = -ENETDOWN;
		goto exit;
	}

	reg_addr = ar->debug.reg_addr;

	ret = kstrtou32_from_user(user_buf, count, 0, &reg_val);
	if (ret)
		goto exit;

	ath10k_hif_write32(ar, reg_addr, reg_val);

	ret = count;

exit:
	mutex_unlock(&ar->conf_mutex);

	return ret;
}

static const struct file_operations fops_reg_value = {
	.read = ath10k_reg_value_read,
	.write = ath10k_reg_value_write,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static ssize_t ath10k_mem_value_read(struct file *file,
				     char __user *user_buf,
				     size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	u8 *buf;
	int ret;

	if (*ppos < 0)
		return -EINVAL;

	if (!count)
		return 0;

	mutex_lock(&ar->conf_mutex);

	buf = vmalloc(count);
	if (!buf) {
		ret = -ENOMEM;
		goto exit;
	}

	if (ar->state != ATH10K_STATE_ON &&
	    ar->state != ATH10K_STATE_UTF) {
		ret = -ENETDOWN;
		goto exit;
	}

	ret = ath10k_hif_diag_read(ar, *ppos, buf, count);
	if (ret) {
		ath10k_warn(ar, "failed to read address 0x%08x via diagnose window fnrom debugfs: %d\n",
			    (u32)(*ppos), ret);
		goto exit;
	}

	ret = copy_to_user(user_buf, buf, count);
	if (ret) {
		ret = -EFAULT;
		goto exit;
	}

	count -= ret;
	*ppos += count;
	ret = count;

exit:
	vfree(buf);
	mutex_unlock(&ar->conf_mutex);

	return ret;
}

static ssize_t ath10k_mem_value_write(struct file *file,
				      const char __user *user_buf,
				      size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	u8 *buf;
	int ret;

	if (*ppos < 0)
		return -EINVAL;

	if (!count)
		return 0;

	mutex_lock(&ar->conf_mutex);

	buf = vmalloc(count);
	if (!buf) {
		ret = -ENOMEM;
		goto exit;
	}

	if (ar->state != ATH10K_STATE_ON &&
	    ar->state != ATH10K_STATE_UTF) {
		ret = -ENETDOWN;
		goto exit;
	}

	ret = copy_from_user(buf, user_buf, count);
	if (ret) {
		ret = -EFAULT;
		goto exit;
	}

	ret = ath10k_hif_diag_write(ar, *ppos, buf, count);
	if (ret) {
		ath10k_warn(ar, "failed to write address 0x%08x via diagnose window from debugfs: %d\n",
			    (u32)(*ppos), ret);
		goto exit;
	}

	*ppos += count;
	ret = count;

exit:
	vfree(buf);
	mutex_unlock(&ar->conf_mutex);

	return ret;
}

static const struct file_operations fops_mem_value = {
	.read = ath10k_mem_value_read,
	.write = ath10k_mem_value_write,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

>>>>>>> v4.9.227
static int ath10k_debug_htt_stats_req(struct ath10k *ar)
{
	u64 cookie;
	int ret;

	lockdep_assert_held(&ar->conf_mutex);

	if (ar->debug.htt_stats_mask == 0)
		/* htt stats are disabled */
		return 0;

	if (ar->state != ATH10K_STATE_ON)
		return 0;

	cookie = get_jiffies_64();

	ret = ath10k_htt_h2t_stats_req(&ar->htt, ar->debug.htt_stats_mask,
				       cookie);
	if (ret) {
		ath10k_warn(ar, "failed to send htt stats request: %d\n", ret);
		return ret;
	}

	queue_delayed_work(ar->workqueue, &ar->debug.htt_stats_dwork,
			   msecs_to_jiffies(ATH10K_DEBUG_HTT_STATS_INTERVAL));

	return 0;
}

static void ath10k_debug_htt_stats_dwork(struct work_struct *work)
{
	struct ath10k *ar = container_of(work, struct ath10k,
					 debug.htt_stats_dwork.work);

	mutex_lock(&ar->conf_mutex);

	ath10k_debug_htt_stats_req(ar);

	mutex_unlock(&ar->conf_mutex);
}

static ssize_t ath10k_read_htt_stats_mask(struct file *file,
					  char __user *user_buf,
					  size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	char buf[32];
	unsigned int len;

	len = scnprintf(buf, sizeof(buf), "%lu\n", ar->debug.htt_stats_mask);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t ath10k_write_htt_stats_mask(struct file *file,
					   const char __user *user_buf,
					   size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	unsigned long mask;
	int ret;

	ret = kstrtoul_from_user(user_buf, count, 0, &mask);
	if (ret)
		return ret;

	/* max 8 bit masks (for now) */
	if (mask > 0xff)
		return -E2BIG;

	mutex_lock(&ar->conf_mutex);

	ar->debug.htt_stats_mask = mask;

	ret = ath10k_debug_htt_stats_req(ar);
	if (ret)
		goto out;

	ret = count;

out:
	mutex_unlock(&ar->conf_mutex);

	return ret;
}

static const struct file_operations fops_htt_stats_mask = {
	.read = ath10k_read_htt_stats_mask,
	.write = ath10k_write_htt_stats_mask,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static ssize_t ath10k_read_htt_max_amsdu_ampdu(struct file *file,
					       char __user *user_buf,
					       size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	char buf[64];
<<<<<<< HEAD
	u8 amsdu = 3, ampdu = 64;
=======
	u8 amsdu, ampdu;
>>>>>>> v4.9.227
	unsigned int len;

	mutex_lock(&ar->conf_mutex);

<<<<<<< HEAD
	if (ar->debug.htt_max_amsdu)
		amsdu = ar->debug.htt_max_amsdu;

	if (ar->debug.htt_max_ampdu)
		ampdu = ar->debug.htt_max_ampdu;

=======
	amsdu = ar->htt.max_num_amsdu;
	ampdu = ar->htt.max_num_ampdu;
>>>>>>> v4.9.227
	mutex_unlock(&ar->conf_mutex);

	len = scnprintf(buf, sizeof(buf), "%u %u\n", amsdu, ampdu);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t ath10k_write_htt_max_amsdu_ampdu(struct file *file,
						const char __user *user_buf,
						size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	int res;
	char buf[64];
	unsigned int amsdu, ampdu;

	simple_write_to_buffer(buf, sizeof(buf) - 1, ppos, user_buf, count);

	/* make sure that buf is null terminated */
	buf[sizeof(buf) - 1] = 0;

	res = sscanf(buf, "%u %u", &amsdu, &ampdu);

	if (res != 2)
		return -EINVAL;

	mutex_lock(&ar->conf_mutex);

	res = ath10k_htt_h2t_aggr_cfg_msg(&ar->htt, ampdu, amsdu);
	if (res)
		goto out;

	res = count;
<<<<<<< HEAD
	ar->debug.htt_max_amsdu = amsdu;
	ar->debug.htt_max_ampdu = ampdu;
=======
	ar->htt.max_num_amsdu = amsdu;
	ar->htt.max_num_ampdu = ampdu;
>>>>>>> v4.9.227

out:
	mutex_unlock(&ar->conf_mutex);
	return res;
}

static const struct file_operations fops_htt_max_amsdu_ampdu = {
	.read = ath10k_read_htt_max_amsdu_ampdu,
	.write = ath10k_write_htt_max_amsdu_ampdu,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static ssize_t ath10k_read_fw_dbglog(struct file *file,
				     char __user *user_buf,
				     size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	unsigned int len;
<<<<<<< HEAD
	char buf[32];

	len = scnprintf(buf, sizeof(buf), "0x%08x\n",
			ar->debug.fw_dbglog_mask);
=======
	char buf[96];

	len = scnprintf(buf, sizeof(buf), "0x%16llx %u\n",
			ar->debug.fw_dbglog_mask, ar->debug.fw_dbglog_level);
>>>>>>> v4.9.227

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t ath10k_write_fw_dbglog(struct file *file,
				      const char __user *user_buf,
				      size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
<<<<<<< HEAD
	unsigned long mask;
	int ret;

	ret = kstrtoul_from_user(user_buf, count, 0, &mask);
	if (ret)
		return ret;
=======
	int ret;
	char buf[96];
	unsigned int log_level;
	u64 mask;

	simple_write_to_buffer(buf, sizeof(buf) - 1, ppos, user_buf, count);

	/* make sure that buf is null terminated */
	buf[sizeof(buf) - 1] = 0;

	ret = sscanf(buf, "%llx %u", &mask, &log_level);

	if (!ret)
		return -EINVAL;

	if (ret == 1)
		/* default if user did not specify */
		log_level = ATH10K_DBGLOG_LEVEL_WARN;
>>>>>>> v4.9.227

	mutex_lock(&ar->conf_mutex);

	ar->debug.fw_dbglog_mask = mask;
<<<<<<< HEAD

	if (ar->state == ATH10K_STATE_ON) {
		ret = ath10k_wmi_dbglog_cfg(ar, ar->debug.fw_dbglog_mask);
=======
	ar->debug.fw_dbglog_level = log_level;

	if (ar->state == ATH10K_STATE_ON) {
		ret = ath10k_wmi_dbglog_cfg(ar, ar->debug.fw_dbglog_mask,
					    ar->debug.fw_dbglog_level);
>>>>>>> v4.9.227
		if (ret) {
			ath10k_warn(ar, "dbglog cfg failed from debugfs: %d\n",
				    ret);
			goto exit;
		}
	}

	ret = count;

exit:
	mutex_unlock(&ar->conf_mutex);

	return ret;
}

<<<<<<< HEAD
=======
/* TODO:  Would be nice to always support ethtool stats, would need to
 * move the stats storage out of ath10k_debug, or always have ath10k_debug
 * struct available..
 */

/* This generally cooresponds to the debugfs fw_stats file */
static const char ath10k_gstrings_stats[][ETH_GSTRING_LEN] = {
	"tx_pkts_nic",
	"tx_bytes_nic",
	"rx_pkts_nic",
	"rx_bytes_nic",
	"d_noise_floor",
	"d_cycle_count",
	"d_phy_error",
	"d_rts_bad",
	"d_rts_good",
	"d_tx_power", /* in .5 dbM I think */
	"d_rx_crc_err", /* fcs_bad */
	"d_no_beacon",
	"d_tx_mpdus_queued",
	"d_tx_msdu_queued",
	"d_tx_msdu_dropped",
	"d_local_enqued",
	"d_local_freed",
	"d_tx_ppdu_hw_queued",
	"d_tx_ppdu_reaped",
	"d_tx_fifo_underrun",
	"d_tx_ppdu_abort",
	"d_tx_mpdu_requed",
	"d_tx_excessive_retries",
	"d_tx_hw_rate",
	"d_tx_dropped_sw_retries",
	"d_tx_illegal_rate",
	"d_tx_continuous_xretries",
	"d_tx_timeout",
	"d_tx_mpdu_txop_limit",
	"d_pdev_resets",
	"d_rx_mid_ppdu_route_change",
	"d_rx_status",
	"d_rx_extra_frags_ring0",
	"d_rx_extra_frags_ring1",
	"d_rx_extra_frags_ring2",
	"d_rx_extra_frags_ring3",
	"d_rx_msdu_htt",
	"d_rx_mpdu_htt",
	"d_rx_msdu_stack",
	"d_rx_mpdu_stack",
	"d_rx_phy_err",
	"d_rx_phy_err_drops",
	"d_rx_mpdu_errors", /* FCS, MIC, ENC */
	"d_fw_crash_count",
	"d_fw_warm_reset_count",
	"d_fw_cold_reset_count",
};

#define ATH10K_SSTATS_LEN ARRAY_SIZE(ath10k_gstrings_stats)

void ath10k_debug_get_et_strings(struct ieee80211_hw *hw,
				 struct ieee80211_vif *vif,
				 u32 sset, u8 *data)
{
	if (sset == ETH_SS_STATS)
		memcpy(data, *ath10k_gstrings_stats,
		       sizeof(ath10k_gstrings_stats));
}

int ath10k_debug_get_et_sset_count(struct ieee80211_hw *hw,
				   struct ieee80211_vif *vif, int sset)
{
	if (sset == ETH_SS_STATS)
		return ATH10K_SSTATS_LEN;

	return 0;
}

void ath10k_debug_get_et_stats(struct ieee80211_hw *hw,
			       struct ieee80211_vif *vif,
			       struct ethtool_stats *stats, u64 *data)
{
	struct ath10k *ar = hw->priv;
	static const struct ath10k_fw_stats_pdev zero_stats = {};
	const struct ath10k_fw_stats_pdev *pdev_stats;
	int i = 0, ret;

	mutex_lock(&ar->conf_mutex);

	if (ar->state == ATH10K_STATE_ON) {
		ret = ath10k_debug_fw_stats_request(ar);
		if (ret) {
			/* just print a warning and try to use older results */
			ath10k_warn(ar,
				    "failed to get fw stats for ethtool: %d\n",
				    ret);
		}
	}

	pdev_stats = list_first_entry_or_null(&ar->debug.fw_stats.pdevs,
					      struct ath10k_fw_stats_pdev,
					      list);
	if (!pdev_stats) {
		/* no results available so just return zeroes */
		pdev_stats = &zero_stats;
	}

	spin_lock_bh(&ar->data_lock);

	data[i++] = pdev_stats->hw_reaped; /* ppdu reaped */
	data[i++] = 0; /* tx bytes */
	data[i++] = pdev_stats->htt_mpdus;
	data[i++] = 0; /* rx bytes */
	data[i++] = pdev_stats->ch_noise_floor;
	data[i++] = pdev_stats->cycle_count;
	data[i++] = pdev_stats->phy_err_count;
	data[i++] = pdev_stats->rts_bad;
	data[i++] = pdev_stats->rts_good;
	data[i++] = pdev_stats->chan_tx_power;
	data[i++] = pdev_stats->fcs_bad;
	data[i++] = pdev_stats->no_beacons;
	data[i++] = pdev_stats->mpdu_enqued;
	data[i++] = pdev_stats->msdu_enqued;
	data[i++] = pdev_stats->wmm_drop;
	data[i++] = pdev_stats->local_enqued;
	data[i++] = pdev_stats->local_freed;
	data[i++] = pdev_stats->hw_queued;
	data[i++] = pdev_stats->hw_reaped;
	data[i++] = pdev_stats->underrun;
	data[i++] = pdev_stats->tx_abort;
	data[i++] = pdev_stats->mpdus_requed;
	data[i++] = pdev_stats->tx_ko;
	data[i++] = pdev_stats->data_rc;
	data[i++] = pdev_stats->sw_retry_failure;
	data[i++] = pdev_stats->illgl_rate_phy_err;
	data[i++] = pdev_stats->pdev_cont_xretry;
	data[i++] = pdev_stats->pdev_tx_timeout;
	data[i++] = pdev_stats->txop_ovf;
	data[i++] = pdev_stats->pdev_resets;
	data[i++] = pdev_stats->mid_ppdu_route_change;
	data[i++] = pdev_stats->status_rcvd;
	data[i++] = pdev_stats->r0_frags;
	data[i++] = pdev_stats->r1_frags;
	data[i++] = pdev_stats->r2_frags;
	data[i++] = pdev_stats->r3_frags;
	data[i++] = pdev_stats->htt_msdus;
	data[i++] = pdev_stats->htt_mpdus;
	data[i++] = pdev_stats->loc_msdus;
	data[i++] = pdev_stats->loc_mpdus;
	data[i++] = pdev_stats->phy_errs;
	data[i++] = pdev_stats->phy_err_drop;
	data[i++] = pdev_stats->mpdu_errs;
	data[i++] = ar->stats.fw_crash_counter;
	data[i++] = ar->stats.fw_warm_reset_counter;
	data[i++] = ar->stats.fw_cold_reset_counter;

	spin_unlock_bh(&ar->data_lock);

	mutex_unlock(&ar->conf_mutex);

	WARN_ON(i != ATH10K_SSTATS_LEN);
}

>>>>>>> v4.9.227
static const struct file_operations fops_fw_dbglog = {
	.read = ath10k_read_fw_dbglog,
	.write = ath10k_write_fw_dbglog,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

<<<<<<< HEAD
=======
static int ath10k_debug_cal_data_fetch(struct ath10k *ar)
{
	u32 hi_addr;
	__le32 addr;
	int ret;

	lockdep_assert_held(&ar->conf_mutex);

	if (WARN_ON(ar->hw_params.cal_data_len > ATH10K_DEBUG_CAL_DATA_LEN))
		return -EINVAL;

	hi_addr = host_interest_item_address(HI_ITEM(hi_board_data));

	ret = ath10k_hif_diag_read(ar, hi_addr, &addr, sizeof(addr));
	if (ret) {
		ath10k_warn(ar, "failed to read hi_board_data address: %d\n",
			    ret);
		return ret;
	}

	ret = ath10k_hif_diag_read(ar, le32_to_cpu(addr), ar->debug.cal_data,
				   ar->hw_params.cal_data_len);
	if (ret) {
		ath10k_warn(ar, "failed to read calibration data: %d\n", ret);
		return ret;
	}

	return 0;
}

static int ath10k_debug_cal_data_open(struct inode *inode, struct file *file)
{
	struct ath10k *ar = inode->i_private;

	mutex_lock(&ar->conf_mutex);

	if (ar->state == ATH10K_STATE_ON ||
	    ar->state == ATH10K_STATE_UTF) {
		ath10k_debug_cal_data_fetch(ar);
	}

	file->private_data = ar;
	mutex_unlock(&ar->conf_mutex);

	return 0;
}

static ssize_t ath10k_debug_cal_data_read(struct file *file,
					  char __user *user_buf,
					  size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;

	mutex_lock(&ar->conf_mutex);

	count = simple_read_from_buffer(user_buf, count, ppos,
					ar->debug.cal_data,
					ar->hw_params.cal_data_len);

	mutex_unlock(&ar->conf_mutex);

	return count;
}

static ssize_t ath10k_write_ani_enable(struct file *file,
				       const char __user *user_buf,
				       size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	int ret;
	u8 enable;

	if (kstrtou8_from_user(user_buf, count, 0, &enable))
		return -EINVAL;

	mutex_lock(&ar->conf_mutex);

	if (ar->ani_enabled == enable) {
		ret = count;
		goto exit;
	}

	ret = ath10k_wmi_pdev_set_param(ar, ar->wmi.pdev_param->ani_enable,
					enable);
	if (ret) {
		ath10k_warn(ar, "ani_enable failed from debugfs: %d\n", ret);
		goto exit;
	}
	ar->ani_enabled = enable;

	ret = count;

exit:
	mutex_unlock(&ar->conf_mutex);

	return ret;
}

static ssize_t ath10k_read_ani_enable(struct file *file, char __user *user_buf,
				      size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	int len = 0;
	char buf[32];

	len = scnprintf(buf, sizeof(buf) - len, "%d\n",
			ar->ani_enabled);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static const struct file_operations fops_ani_enable = {
	.read = ath10k_read_ani_enable,
	.write = ath10k_write_ani_enable,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static const struct file_operations fops_cal_data = {
	.open = ath10k_debug_cal_data_open,
	.read = ath10k_debug_cal_data_read,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

static ssize_t ath10k_read_nf_cal_period(struct file *file,
					 char __user *user_buf,
					 size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	unsigned int len;
	char buf[32];

	len = scnprintf(buf, sizeof(buf), "%d\n",
			ar->debug.nf_cal_period);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t ath10k_write_nf_cal_period(struct file *file,
					  const char __user *user_buf,
					  size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	unsigned long period;
	int ret;

	ret = kstrtoul_from_user(user_buf, count, 0, &period);
	if (ret)
		return ret;

	if (period > WMI_PDEV_PARAM_CAL_PERIOD_MAX)
		return -EINVAL;

	/* there's no way to switch back to the firmware default */
	if (period == 0)
		return -EINVAL;

	mutex_lock(&ar->conf_mutex);

	ar->debug.nf_cal_period = period;

	if (ar->state != ATH10K_STATE_ON) {
		/* firmware is not running, nothing else to do */
		ret = count;
		goto exit;
	}

	ret = ath10k_wmi_pdev_set_param(ar, ar->wmi.pdev_param->cal_period,
					ar->debug.nf_cal_period);
	if (ret) {
		ath10k_warn(ar, "cal period cfg failed from debugfs: %d\n",
			    ret);
		goto exit;
	}

	ret = count;

exit:
	mutex_unlock(&ar->conf_mutex);

	return ret;
}

static const struct file_operations fops_nf_cal_period = {
	.read = ath10k_read_nf_cal_period,
	.write = ath10k_write_nf_cal_period,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

#define ATH10K_TPC_CONFIG_BUF_SIZE	(1024 * 1024)

static int ath10k_debug_tpc_stats_request(struct ath10k *ar)
{
	int ret;
	unsigned long time_left;

	lockdep_assert_held(&ar->conf_mutex);

	reinit_completion(&ar->debug.tpc_complete);

	ret = ath10k_wmi_pdev_get_tpc_config(ar, WMI_TPC_CONFIG_PARAM);
	if (ret) {
		ath10k_warn(ar, "failed to request tpc config: %d\n", ret);
		return ret;
	}

	time_left = wait_for_completion_timeout(&ar->debug.tpc_complete,
						1 * HZ);
	if (time_left == 0)
		return -ETIMEDOUT;

	return 0;
}

void ath10k_debug_tpc_stats_process(struct ath10k *ar,
				    struct ath10k_tpc_stats *tpc_stats)
{
	spin_lock_bh(&ar->data_lock);

	kfree(ar->debug.tpc_stats);
	ar->debug.tpc_stats = tpc_stats;
	complete(&ar->debug.tpc_complete);

	spin_unlock_bh(&ar->data_lock);
}

static void ath10k_tpc_stats_print(struct ath10k_tpc_stats *tpc_stats,
				   unsigned int j, char *buf, unsigned int *len)
{
	unsigned int i, buf_len;
	static const char table_str[][5] = { "CDD",
					     "STBC",
					     "TXBF" };
	static const char pream_str[][6] = { "CCK",
					     "OFDM",
					     "HT20",
					     "HT40",
					     "VHT20",
					     "VHT40",
					     "VHT80",
					     "HTCUP" };

	buf_len = ATH10K_TPC_CONFIG_BUF_SIZE;
	*len += scnprintf(buf + *len, buf_len - *len,
			  "********************************\n");
	*len += scnprintf(buf + *len, buf_len - *len,
			  "******************* %s POWER TABLE ****************\n",
			  table_str[j]);
	*len += scnprintf(buf + *len, buf_len - *len,
			  "********************************\n");
	*len += scnprintf(buf + *len, buf_len - *len,
			  "No.  Preamble Rate_code tpc_value1 tpc_value2 tpc_value3\n");

	for (i = 0; i < tpc_stats->rate_max; i++) {
		*len += scnprintf(buf + *len, buf_len - *len,
				  "%8d %s 0x%2x %s\n", i,
				  pream_str[tpc_stats->tpc_table[j].pream_idx[i]],
				  tpc_stats->tpc_table[j].rate_code[i],
				  tpc_stats->tpc_table[j].tpc_value[i]);
	}

	*len += scnprintf(buf + *len, buf_len - *len,
			  "***********************************\n");
}

static void ath10k_tpc_stats_fill(struct ath10k *ar,
				  struct ath10k_tpc_stats *tpc_stats,
				  char *buf)
{
	unsigned int len, j, buf_len;

	len = 0;
	buf_len = ATH10K_TPC_CONFIG_BUF_SIZE;

	spin_lock_bh(&ar->data_lock);

	if (!tpc_stats) {
		ath10k_warn(ar, "failed to get tpc stats\n");
		goto unlock;
	}

	len += scnprintf(buf + len, buf_len - len, "\n");
	len += scnprintf(buf + len, buf_len - len,
			 "*************************************\n");
	len += scnprintf(buf + len, buf_len - len,
			 "TPC config for channel %4d mode %d\n",
			 tpc_stats->chan_freq,
			 tpc_stats->phy_mode);
	len += scnprintf(buf + len, buf_len - len,
			 "*************************************\n");
	len += scnprintf(buf + len, buf_len - len,
			 "CTL		=  0x%2x Reg. Domain		= %2d\n",
			 tpc_stats->ctl,
			 tpc_stats->reg_domain);
	len += scnprintf(buf + len, buf_len - len,
			 "Antenna Gain	= %2d Reg. Max Antenna Gain	=  %2d\n",
			 tpc_stats->twice_antenna_gain,
			 tpc_stats->twice_antenna_reduction);
	len += scnprintf(buf + len, buf_len - len,
			 "Power Limit	= %2d Reg. Max Power		= %2d\n",
			 tpc_stats->power_limit,
			 tpc_stats->twice_max_rd_power / 2);
	len += scnprintf(buf + len, buf_len - len,
			 "Num tx chains	= %2d Num supported rates	= %2d\n",
			 tpc_stats->num_tx_chain,
			 tpc_stats->rate_max);

	for (j = 0; j < tpc_stats->num_tx_chain ; j++) {
		switch (j) {
		case WMI_TPC_TABLE_TYPE_CDD:
			if (tpc_stats->flag[j] == ATH10K_TPC_TABLE_TYPE_FLAG) {
				len += scnprintf(buf + len, buf_len - len,
						 "CDD not supported\n");
				break;
			}

			ath10k_tpc_stats_print(tpc_stats, j, buf, &len);
			break;
		case WMI_TPC_TABLE_TYPE_STBC:
			if (tpc_stats->flag[j] == ATH10K_TPC_TABLE_TYPE_FLAG) {
				len += scnprintf(buf + len, buf_len - len,
						 "STBC not supported\n");
				break;
			}

			ath10k_tpc_stats_print(tpc_stats, j, buf, &len);
			break;
		case WMI_TPC_TABLE_TYPE_TXBF:
			if (tpc_stats->flag[j] == ATH10K_TPC_TABLE_TYPE_FLAG) {
				len += scnprintf(buf + len, buf_len - len,
						 "TXBF not supported\n***************************\n");
				break;
			}

			ath10k_tpc_stats_print(tpc_stats, j, buf, &len);
			break;
		default:
			len += scnprintf(buf + len, buf_len - len,
					 "Invalid Type\n");
			break;
		}
	}

unlock:
	spin_unlock_bh(&ar->data_lock);

	if (len >= buf_len)
		buf[len - 1] = 0;
	else
		buf[len] = 0;
}

static int ath10k_tpc_stats_open(struct inode *inode, struct file *file)
{
	struct ath10k *ar = inode->i_private;
	void *buf = NULL;
	int ret;

	mutex_lock(&ar->conf_mutex);

	if (ar->state != ATH10K_STATE_ON) {
		ret = -ENETDOWN;
		goto err_unlock;
	}

	buf = vmalloc(ATH10K_TPC_CONFIG_BUF_SIZE);
	if (!buf) {
		ret = -ENOMEM;
		goto err_unlock;
	}

	ret = ath10k_debug_tpc_stats_request(ar);
	if (ret) {
		ath10k_warn(ar, "failed to request tpc config stats: %d\n",
			    ret);
		goto err_free;
	}

	ath10k_tpc_stats_fill(ar, ar->debug.tpc_stats, buf);
	file->private_data = buf;

	mutex_unlock(&ar->conf_mutex);
	return 0;

err_free:
	vfree(buf);

err_unlock:
	mutex_unlock(&ar->conf_mutex);
	return ret;
}

static int ath10k_tpc_stats_release(struct inode *inode, struct file *file)
{
	vfree(file->private_data);

	return 0;
}

static ssize_t ath10k_tpc_stats_read(struct file *file, char __user *user_buf,
				     size_t count, loff_t *ppos)
{
	const char *buf = file->private_data;
	unsigned int len = strlen(buf);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static const struct file_operations fops_tpc_stats = {
	.open = ath10k_tpc_stats_open,
	.release = ath10k_tpc_stats_release,
	.read = ath10k_tpc_stats_read,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

>>>>>>> v4.9.227
int ath10k_debug_start(struct ath10k *ar)
{
	int ret;

	lockdep_assert_held(&ar->conf_mutex);

	ret = ath10k_debug_htt_stats_req(ar);
	if (ret)
		/* continue normally anyway, this isn't serious */
		ath10k_warn(ar, "failed to start htt stats workqueue: %d\n",
			    ret);

	if (ar->debug.fw_dbglog_mask) {
<<<<<<< HEAD
		ret = ath10k_wmi_dbglog_cfg(ar, ar->debug.fw_dbglog_mask);
=======
		ret = ath10k_wmi_dbglog_cfg(ar, ar->debug.fw_dbglog_mask,
					    ATH10K_DBGLOG_LEVEL_WARN);
>>>>>>> v4.9.227
		if (ret)
			/* not serious */
			ath10k_warn(ar, "failed to enable dbglog during start: %d",
				    ret);
	}

<<<<<<< HEAD
	return 0;
=======
	if (ar->debug.pktlog_filter) {
		ret = ath10k_wmi_pdev_pktlog_enable(ar,
						    ar->debug.pktlog_filter);
		if (ret)
			/* not serious */
			ath10k_warn(ar,
				    "failed to enable pktlog filter %x: %d\n",
				    ar->debug.pktlog_filter, ret);
	} else {
		ret = ath10k_wmi_pdev_pktlog_disable(ar);
		if (ret)
			/* not serious */
			ath10k_warn(ar, "failed to disable pktlog: %d\n", ret);
	}

	if (ar->debug.nf_cal_period) {
		ret = ath10k_wmi_pdev_set_param(ar,
						ar->wmi.pdev_param->cal_period,
						ar->debug.nf_cal_period);
		if (ret)
			/* not serious */
			ath10k_warn(ar, "cal period cfg failed from debug start: %d\n",
				    ret);
	}

	return ret;
>>>>>>> v4.9.227
}

void ath10k_debug_stop(struct ath10k *ar)
{
	lockdep_assert_held(&ar->conf_mutex);

<<<<<<< HEAD
=======
	ath10k_debug_cal_data_fetch(ar);

>>>>>>> v4.9.227
	/* Must not use _sync to avoid deadlock, we do that in
	 * ath10k_debug_destroy(). The check for htt_stats_mask is to avoid
	 * warning from del_timer(). */
	if (ar->debug.htt_stats_mask != 0)
		cancel_delayed_work(&ar->debug.htt_stats_dwork);

<<<<<<< HEAD
	ar->debug.htt_max_amsdu = 0;
	ar->debug.htt_max_ampdu = 0;
=======
	ath10k_wmi_pdev_pktlog_disable(ar);
>>>>>>> v4.9.227
}

static ssize_t ath10k_write_simulate_radar(struct file *file,
					   const char __user *user_buf,
					   size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	struct ath10k_vif *arvif;

	/* Just check for for the first vif alone, as all the vifs will be
	 * sharing the same channel and if the channel is disabled, all the
	 * vifs will share the same 'is_started' state.
	 */
	arvif = list_first_entry(&ar->arvifs, typeof(*arvif), list);
	if (!arvif->is_started)
		return -EINVAL;

	ieee80211_radar_detected(ar->hw);

	return count;
}

static const struct file_operations fops_simulate_radar = {
	.write = ath10k_write_simulate_radar,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

#define ATH10K_DFS_STAT(s, p) (\
	len += scnprintf(buf + len, size - len, "%-28s : %10u\n", s, \
			 ar->debug.dfs_stats.p))

#define ATH10K_DFS_POOL_STAT(s, p) (\
	len += scnprintf(buf + len, size - len, "%-28s : %10u\n", s, \
			 ar->debug.dfs_pool_stats.p))

static ssize_t ath10k_read_dfs_stats(struct file *file, char __user *user_buf,
				     size_t count, loff_t *ppos)
{
	int retval = 0, len = 0;
	const int size = 8000;
	struct ath10k *ar = file->private_data;
	char *buf;

	buf = kzalloc(size, GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;

	if (!ar->dfs_detector) {
		len += scnprintf(buf + len, size - len, "DFS not enabled\n");
		goto exit;
	}

	ar->debug.dfs_pool_stats =
			ar->dfs_detector->get_stats(ar->dfs_detector);

	len += scnprintf(buf + len, size - len, "Pulse detector statistics:\n");

	ATH10K_DFS_STAT("reported phy errors", phy_errors);
	ATH10K_DFS_STAT("pulse events reported", pulses_total);
	ATH10K_DFS_STAT("DFS pulses detected", pulses_detected);
	ATH10K_DFS_STAT("DFS pulses discarded", pulses_discarded);
	ATH10K_DFS_STAT("Radars detected", radar_detected);

	len += scnprintf(buf + len, size - len, "Global Pool statistics:\n");
	ATH10K_DFS_POOL_STAT("Pool references", pool_reference);
	ATH10K_DFS_POOL_STAT("Pulses allocated", pulse_allocated);
	ATH10K_DFS_POOL_STAT("Pulses alloc error", pulse_alloc_error);
	ATH10K_DFS_POOL_STAT("Pulses in use", pulse_used);
	ATH10K_DFS_POOL_STAT("Seqs. allocated", pseq_allocated);
	ATH10K_DFS_POOL_STAT("Seqs. alloc error", pseq_alloc_error);
	ATH10K_DFS_POOL_STAT("Seqs. in use", pseq_used);

exit:
	if (len > size)
		len = size;

	retval = simple_read_from_buffer(user_buf, count, ppos, buf, len);
	kfree(buf);

	return retval;
}

static const struct file_operations fops_dfs_stats = {
	.read = ath10k_read_dfs_stats,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

<<<<<<< HEAD
=======
static ssize_t ath10k_write_pktlog_filter(struct file *file,
					  const char __user *ubuf,
					  size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	u32 filter;
	int ret;

	if (kstrtouint_from_user(ubuf, count, 0, &filter))
		return -EINVAL;

	mutex_lock(&ar->conf_mutex);

	if (ar->state != ATH10K_STATE_ON) {
		ar->debug.pktlog_filter = filter;
		ret = count;
		goto out;
	}

	if (filter == ar->debug.pktlog_filter) {
		ret = count;
		goto out;
	}

	if (filter) {
		ret = ath10k_wmi_pdev_pktlog_enable(ar, filter);
		if (ret) {
			ath10k_warn(ar, "failed to enable pktlog filter %x: %d\n",
				    ar->debug.pktlog_filter, ret);
			goto out;
		}
	} else {
		ret = ath10k_wmi_pdev_pktlog_disable(ar);
		if (ret) {
			ath10k_warn(ar, "failed to disable pktlog: %d\n", ret);
			goto out;
		}
	}

	ar->debug.pktlog_filter = filter;
	ret = count;

out:
	mutex_unlock(&ar->conf_mutex);
	return ret;
}

static ssize_t ath10k_read_pktlog_filter(struct file *file, char __user *ubuf,
					 size_t count, loff_t *ppos)
{
	char buf[32];
	struct ath10k *ar = file->private_data;
	int len = 0;

	mutex_lock(&ar->conf_mutex);
	len = scnprintf(buf, sizeof(buf) - len, "%08x\n",
			ar->debug.pktlog_filter);
	mutex_unlock(&ar->conf_mutex);

	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static const struct file_operations fops_pktlog_filter = {
	.read = ath10k_read_pktlog_filter,
	.write = ath10k_write_pktlog_filter,
	.open = simple_open
};

static ssize_t ath10k_write_quiet_period(struct file *file,
					 const char __user *ubuf,
					 size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	u32 period;

	if (kstrtouint_from_user(ubuf, count, 0, &period))
		return -EINVAL;

	if (period < ATH10K_QUIET_PERIOD_MIN) {
		ath10k_warn(ar, "Quiet period %u can not be lesser than 25ms\n",
			    period);
		return -EINVAL;
	}
	mutex_lock(&ar->conf_mutex);
	ar->thermal.quiet_period = period;
	ath10k_thermal_set_throttling(ar);
	mutex_unlock(&ar->conf_mutex);

	return count;
}

static ssize_t ath10k_read_quiet_period(struct file *file, char __user *ubuf,
					size_t count, loff_t *ppos)
{
	char buf[32];
	struct ath10k *ar = file->private_data;
	int len = 0;

	mutex_lock(&ar->conf_mutex);
	len = scnprintf(buf, sizeof(buf) - len, "%d\n",
			ar->thermal.quiet_period);
	mutex_unlock(&ar->conf_mutex);

	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static const struct file_operations fops_quiet_period = {
	.read = ath10k_read_quiet_period,
	.write = ath10k_write_quiet_period,
	.open = simple_open
};

static ssize_t ath10k_write_btcoex(struct file *file,
				   const char __user *ubuf,
				   size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	char buf[32];
	size_t buf_size;
	int ret;
	bool val;
	u32 pdev_param;

	buf_size = min(count, (sizeof(buf) - 1));
	if (copy_from_user(buf, ubuf, buf_size))
		return -EFAULT;

	buf[buf_size] = '\0';

	if (strtobool(buf, &val) != 0)
		return -EINVAL;

	mutex_lock(&ar->conf_mutex);

	if (ar->state != ATH10K_STATE_ON &&
	    ar->state != ATH10K_STATE_RESTARTED) {
		ret = -ENETDOWN;
		goto exit;
	}

	if (!(test_bit(ATH10K_FLAG_BTCOEX, &ar->dev_flags) ^ val)) {
		ret = count;
		goto exit;
	}

	pdev_param = ar->wmi.pdev_param->enable_btcoex;
	if (test_bit(ATH10K_FW_FEATURE_BTCOEX_PARAM,
		     ar->running_fw->fw_file.fw_features)) {
		ret = ath10k_wmi_pdev_set_param(ar, pdev_param, val);
		if (ret) {
			ath10k_warn(ar, "failed to enable btcoex: %d\n", ret);
			ret = count;
			goto exit;
		}
	} else {
		ath10k_info(ar, "restarting firmware due to btcoex change");
		queue_work(ar->workqueue, &ar->restart_work);
	}

	if (val)
		set_bit(ATH10K_FLAG_BTCOEX, &ar->dev_flags);
	else
		clear_bit(ATH10K_FLAG_BTCOEX, &ar->dev_flags);

	ret = count;

exit:
	mutex_unlock(&ar->conf_mutex);

	return ret;
}

static ssize_t ath10k_read_btcoex(struct file *file, char __user *ubuf,
				  size_t count, loff_t *ppos)
{
	char buf[32];
	struct ath10k *ar = file->private_data;
	int len = 0;

	mutex_lock(&ar->conf_mutex);
	len = scnprintf(buf, sizeof(buf) - len, "%d\n",
			test_bit(ATH10K_FLAG_BTCOEX, &ar->dev_flags));
	mutex_unlock(&ar->conf_mutex);

	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static const struct file_operations fops_btcoex = {
	.read = ath10k_read_btcoex,
	.write = ath10k_write_btcoex,
	.open = simple_open
};

static ssize_t ath10k_write_peer_stats(struct file *file,
				       const char __user *ubuf,
				       size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	char buf[32];
	size_t buf_size;
	int ret;
	bool val;

	buf_size = min(count, (sizeof(buf) - 1));
	if (copy_from_user(buf, ubuf, buf_size))
		return -EFAULT;

	buf[buf_size] = '\0';

	if (strtobool(buf, &val) != 0)
		return -EINVAL;

	mutex_lock(&ar->conf_mutex);

	if (ar->state != ATH10K_STATE_ON &&
	    ar->state != ATH10K_STATE_RESTARTED) {
		ret = -ENETDOWN;
		goto exit;
	}

	if (!(test_bit(ATH10K_FLAG_PEER_STATS, &ar->dev_flags) ^ val)) {
		ret = count;
		goto exit;
	}

	if (val)
		set_bit(ATH10K_FLAG_PEER_STATS, &ar->dev_flags);
	else
		clear_bit(ATH10K_FLAG_PEER_STATS, &ar->dev_flags);

	ath10k_info(ar, "restarting firmware due to Peer stats change");

	queue_work(ar->workqueue, &ar->restart_work);
	ret = count;

exit:
	mutex_unlock(&ar->conf_mutex);
	return ret;
}

static ssize_t ath10k_read_peer_stats(struct file *file, char __user *ubuf,
				      size_t count, loff_t *ppos)

{
	char buf[32];
	struct ath10k *ar = file->private_data;
	int len = 0;

	mutex_lock(&ar->conf_mutex);
	len = scnprintf(buf, sizeof(buf) - len, "%d\n",
			test_bit(ATH10K_FLAG_PEER_STATS, &ar->dev_flags));
	mutex_unlock(&ar->conf_mutex);

	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static const struct file_operations fops_peer_stats = {
	.read = ath10k_read_peer_stats,
	.write = ath10k_write_peer_stats,
	.open = simple_open
};

static ssize_t ath10k_debug_fw_checksums_read(struct file *file,
					      char __user *user_buf,
					      size_t count, loff_t *ppos)
{
	struct ath10k *ar = file->private_data;
	unsigned int len = 0, buf_len = 4096;
	ssize_t ret_cnt;
	char *buf;

	buf = kzalloc(buf_len, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	mutex_lock(&ar->conf_mutex);

	len += scnprintf(buf + len, buf_len - len,
			 "firmware-N.bin\t\t%08x\n",
			 crc32_le(0, ar->normal_mode_fw.fw_file.firmware->data,
				  ar->normal_mode_fw.fw_file.firmware->size));
	len += scnprintf(buf + len, buf_len - len,
			 "athwlan\t\t\t%08x\n",
			 crc32_le(0, ar->normal_mode_fw.fw_file.firmware_data,
				  ar->normal_mode_fw.fw_file.firmware_len));
	len += scnprintf(buf + len, buf_len - len,
			 "otp\t\t\t%08x\n",
			 crc32_le(0, ar->normal_mode_fw.fw_file.otp_data,
				  ar->normal_mode_fw.fw_file.otp_len));
	len += scnprintf(buf + len, buf_len - len,
			 "codeswap\t\t%08x\n",
			 crc32_le(0, ar->normal_mode_fw.fw_file.codeswap_data,
				  ar->normal_mode_fw.fw_file.codeswap_len));
	len += scnprintf(buf + len, buf_len - len,
			 "board-N.bin\t\t%08x\n",
			 crc32_le(0, ar->normal_mode_fw.board->data,
				  ar->normal_mode_fw.board->size));
	len += scnprintf(buf + len, buf_len - len,
			 "board\t\t\t%08x\n",
			 crc32_le(0, ar->normal_mode_fw.board_data,
				  ar->normal_mode_fw.board_len));

	ret_cnt = simple_read_from_buffer(user_buf, count, ppos, buf, len);

	mutex_unlock(&ar->conf_mutex);

	kfree(buf);
	return ret_cnt;
}

static const struct file_operations fops_fw_checksums = {
	.read = ath10k_debug_fw_checksums_read,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

>>>>>>> v4.9.227
int ath10k_debug_create(struct ath10k *ar)
{
	ar->debug.fw_crash_data = vzalloc(sizeof(*ar->debug.fw_crash_data));
	if (!ar->debug.fw_crash_data)
		return -ENOMEM;

<<<<<<< HEAD
=======
	ar->debug.cal_data = vzalloc(ATH10K_DEBUG_CAL_DATA_LEN);
	if (!ar->debug.cal_data)
		return -ENOMEM;

	INIT_LIST_HEAD(&ar->debug.fw_stats.pdevs);
	INIT_LIST_HEAD(&ar->debug.fw_stats.vdevs);
	INIT_LIST_HEAD(&ar->debug.fw_stats.peers);
	INIT_LIST_HEAD(&ar->debug.fw_stats.peers_extd);

>>>>>>> v4.9.227
	return 0;
}

void ath10k_debug_destroy(struct ath10k *ar)
{
	vfree(ar->debug.fw_crash_data);
	ar->debug.fw_crash_data = NULL;
<<<<<<< HEAD
=======

	vfree(ar->debug.cal_data);
	ar->debug.cal_data = NULL;

	ath10k_debug_fw_stats_reset(ar);

	kfree(ar->debug.tpc_stats);
>>>>>>> v4.9.227
}

int ath10k_debug_register(struct ath10k *ar)
{
	ar->debug.debugfs_phy = debugfs_create_dir("ath10k",
						   ar->hw->wiphy->debugfsdir);
	if (IS_ERR_OR_NULL(ar->debug.debugfs_phy)) {
		if (IS_ERR(ar->debug.debugfs_phy))
			return PTR_ERR(ar->debug.debugfs_phy);

		return -ENOMEM;
	}

	INIT_DELAYED_WORK(&ar->debug.htt_stats_dwork,
			  ath10k_debug_htt_stats_dwork);

<<<<<<< HEAD
	init_completion(&ar->debug.event_stats_compl);
=======
	init_completion(&ar->debug.tpc_complete);
	init_completion(&ar->debug.fw_stats_complete);
>>>>>>> v4.9.227

	debugfs_create_file("fw_stats", S_IRUSR, ar->debug.debugfs_phy, ar,
			    &fops_fw_stats);

<<<<<<< HEAD
	debugfs_create_file("wmi_services", S_IRUSR, ar->debug.debugfs_phy, ar,
			    &fops_wmi_services);

	debugfs_create_file("simulate_fw_crash", S_IRUSR, ar->debug.debugfs_phy,
			    ar, &fops_simulate_fw_crash);
=======
	debugfs_create_file("fw_reset_stats", S_IRUSR, ar->debug.debugfs_phy,
			    ar, &fops_fw_reset_stats);

	debugfs_create_file("wmi_services", S_IRUSR, ar->debug.debugfs_phy, ar,
			    &fops_wmi_services);

	debugfs_create_file("simulate_fw_crash", S_IRUSR | S_IWUSR,
			    ar->debug.debugfs_phy, ar, &fops_simulate_fw_crash);
>>>>>>> v4.9.227

	debugfs_create_file("fw_crash_dump", S_IRUSR, ar->debug.debugfs_phy,
			    ar, &fops_fw_crash_dump);

<<<<<<< HEAD
	debugfs_create_file("chip_id", S_IRUSR, ar->debug.debugfs_phy,
			    ar, &fops_chip_id);

	debugfs_create_file("htt_stats_mask", S_IRUSR, ar->debug.debugfs_phy,
			    ar, &fops_htt_stats_mask);
=======
	debugfs_create_file("reg_addr", S_IRUSR | S_IWUSR,
			    ar->debug.debugfs_phy, ar, &fops_reg_addr);

	debugfs_create_file("reg_value", S_IRUSR | S_IWUSR,
			    ar->debug.debugfs_phy, ar, &fops_reg_value);

	debugfs_create_file("mem_value", S_IRUSR | S_IWUSR,
			    ar->debug.debugfs_phy, ar, &fops_mem_value);

	debugfs_create_file("chip_id", S_IRUSR, ar->debug.debugfs_phy,
			    ar, &fops_chip_id);

	debugfs_create_file("htt_stats_mask", S_IRUSR | S_IWUSR,
			    ar->debug.debugfs_phy, ar, &fops_htt_stats_mask);
>>>>>>> v4.9.227

	debugfs_create_file("htt_max_amsdu_ampdu", S_IRUSR | S_IWUSR,
			    ar->debug.debugfs_phy, ar,
			    &fops_htt_max_amsdu_ampdu);

<<<<<<< HEAD
	debugfs_create_file("fw_dbglog", S_IRUSR, ar->debug.debugfs_phy,
			    ar, &fops_fw_dbglog);

	if (config_enabled(CONFIG_ATH10K_DFS_CERTIFIED)) {
=======
	debugfs_create_file("fw_dbglog", S_IRUSR | S_IWUSR,
			    ar->debug.debugfs_phy, ar, &fops_fw_dbglog);

	debugfs_create_file("cal_data", S_IRUSR, ar->debug.debugfs_phy,
			    ar, &fops_cal_data);

	debugfs_create_file("ani_enable", S_IRUSR | S_IWUSR,
			    ar->debug.debugfs_phy, ar, &fops_ani_enable);

	debugfs_create_file("nf_cal_period", S_IRUSR | S_IWUSR,
			    ar->debug.debugfs_phy, ar, &fops_nf_cal_period);

	if (IS_ENABLED(CONFIG_ATH10K_DFS_CERTIFIED)) {
>>>>>>> v4.9.227
		debugfs_create_file("dfs_simulate_radar", S_IWUSR,
				    ar->debug.debugfs_phy, ar,
				    &fops_simulate_radar);

		debugfs_create_bool("dfs_block_radar_events", S_IWUSR,
				    ar->debug.debugfs_phy,
				    &ar->dfs_block_radar_events);

		debugfs_create_file("dfs_stats", S_IRUSR,
				    ar->debug.debugfs_phy, ar,
				    &fops_dfs_stats);
	}

<<<<<<< HEAD
=======
	debugfs_create_file("pktlog_filter", S_IRUGO | S_IWUSR,
			    ar->debug.debugfs_phy, ar, &fops_pktlog_filter);

	debugfs_create_file("quiet_period", S_IRUGO | S_IWUSR,
			    ar->debug.debugfs_phy, ar, &fops_quiet_period);

	debugfs_create_file("tpc_stats", S_IRUSR,
			    ar->debug.debugfs_phy, ar, &fops_tpc_stats);

	if (test_bit(WMI_SERVICE_COEX_GPIO, ar->wmi.svc_map))
		debugfs_create_file("btcoex", S_IRUGO | S_IWUSR,
				    ar->debug.debugfs_phy, ar, &fops_btcoex);

	if (test_bit(WMI_SERVICE_PEER_STATS, ar->wmi.svc_map))
		debugfs_create_file("peer_stats", S_IRUGO | S_IWUSR,
				    ar->debug.debugfs_phy, ar,
				    &fops_peer_stats);

	debugfs_create_file("fw_checksums", S_IRUSR,
			    ar->debug.debugfs_phy, ar, &fops_fw_checksums);

>>>>>>> v4.9.227
	return 0;
}

void ath10k_debug_unregister(struct ath10k *ar)
{
	cancel_delayed_work_sync(&ar->debug.htt_stats_dwork);
}

#endif /* CONFIG_ATH10K_DEBUGFS */

#ifdef CONFIG_ATH10K_DEBUG
void ath10k_dbg(struct ath10k *ar, enum ath10k_debug_mask mask,
		const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	va_start(args, fmt);

	vaf.fmt = fmt;
	vaf.va = &args;

	if (ath10k_debug_mask & mask)
		dev_printk(KERN_DEBUG, ar->dev, "%pV", &vaf);

	trace_ath10k_log_dbg(ar, mask, &vaf);

	va_end(args);
}
EXPORT_SYMBOL(ath10k_dbg);

void ath10k_dbg_dump(struct ath10k *ar,
		     enum ath10k_debug_mask mask,
		     const char *msg, const char *prefix,
		     const void *buf, size_t len)
{
<<<<<<< HEAD
=======
	char linebuf[256];
	unsigned int linebuflen;
	const void *ptr;

>>>>>>> v4.9.227
	if (ath10k_debug_mask & mask) {
		if (msg)
			ath10k_dbg(ar, mask, "%s\n", msg);

<<<<<<< HEAD
		print_hex_dump_bytes(prefix, DUMP_PREFIX_OFFSET, buf, len);
=======
		for (ptr = buf; (ptr - buf) < len; ptr += 16) {
			linebuflen = 0;
			linebuflen += scnprintf(linebuf + linebuflen,
						sizeof(linebuf) - linebuflen,
						"%s%08x: ",
						(prefix ? prefix : ""),
						(unsigned int)(ptr - buf));
			hex_dump_to_buffer(ptr, len - (ptr - buf), 16, 1,
					   linebuf + linebuflen,
					   sizeof(linebuf) - linebuflen, true);
			dev_printk(KERN_DEBUG, ar->dev, "%s\n", linebuf);
		}
>>>>>>> v4.9.227
	}

	/* tracing code doesn't like null strings :/ */
	trace_ath10k_log_dbg_dump(ar, msg ? msg : "", prefix ? prefix : "",
				  buf, len);
}
EXPORT_SYMBOL(ath10k_dbg_dump);

#endif /* CONFIG_ATH10K_DEBUG */
