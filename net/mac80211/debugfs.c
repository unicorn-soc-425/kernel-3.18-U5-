<<<<<<< HEAD

=======
>>>>>>> v4.9.227
/*
 * mac80211 debugfs for wireless PHYs
 *
 * Copyright 2007	Johannes Berg <johannes@sipsolutions.net>
 * Copyright 2013-2014  Intel Mobile Communications GmbH
 *
 * GPLv2
 *
 */

#include <linux/debugfs.h>
#include <linux/rtnetlink.h>
<<<<<<< HEAD
=======
#include <linux/vmalloc.h>
>>>>>>> v4.9.227
#include "ieee80211_i.h"
#include "driver-ops.h"
#include "rate.h"
#include "debugfs.h"

#define DEBUGFS_FORMAT_BUFFER_SIZE 100

<<<<<<< HEAD
#define TX_LATENCY_BIN_DELIMTER_C ','
#define TX_LATENCY_BIN_DELIMTER_S ","
#define TX_LATENCY_BINS_DISABLED "enable(bins disabled)\n"
#define TX_LATENCY_DISABLED "disable\n"


/*
 * Display if Tx latency statistics & bins are enabled/disabled
 */
static ssize_t sta_tx_latency_stat_read(struct file *file,
					char __user *userbuf,
					size_t count, loff_t *ppos)
{
	struct ieee80211_local *local = file->private_data;
	struct ieee80211_tx_latency_bin_ranges  *tx_latency;
	char *buf;
	int bufsz, i, ret;
	int pos = 0;

	rcu_read_lock();

	tx_latency = rcu_dereference(local->tx_latency);

	if (tx_latency && tx_latency->n_ranges) {
		bufsz = tx_latency->n_ranges * 15;
		buf = kzalloc(bufsz, GFP_ATOMIC);
		if (!buf)
			goto err;

		for (i = 0; i < tx_latency->n_ranges; i++)
			pos += scnprintf(buf + pos, bufsz - pos, "%d,",
					 tx_latency->ranges[i]);
		pos += scnprintf(buf + pos, bufsz - pos, "\n");
	} else if (tx_latency) {
		bufsz = sizeof(TX_LATENCY_BINS_DISABLED) + 1;
		buf = kzalloc(bufsz, GFP_ATOMIC);
		if (!buf)
			goto err;

		pos += scnprintf(buf + pos, bufsz - pos, "%s\n",
				 TX_LATENCY_BINS_DISABLED);
	} else {
		bufsz = sizeof(TX_LATENCY_DISABLED) + 1;
		buf = kzalloc(bufsz, GFP_ATOMIC);
		if (!buf)
			goto err;

		pos += scnprintf(buf + pos, bufsz - pos, "%s\n",
				 TX_LATENCY_DISABLED);
	}

	rcu_read_unlock();

	ret = simple_read_from_buffer(userbuf, count, ppos, buf, pos);
	kfree(buf);

	return ret;
err:
	rcu_read_unlock();
	return -ENOMEM;
}

/*
 * Receive input from user regarding Tx latency statistics
 * The input should indicate if Tx latency statistics and bins are
 * enabled/disabled.
 * If bins are enabled input should indicate the amount of different bins and
 * their ranges. Each bin will count how many Tx frames transmitted within the
 * appropriate latency.
 * Legal input is:
 * a) "enable(bins disabled)" - to enable only general statistics
 * b) "a,b,c,d,...z" - to enable general statistics and bins, where all are
 * numbers and a < b < c < d.. < z
 * c) "disable" - disable all statistics
 * NOTE: must configure Tx latency statistics bins before stations connected.
 */

static ssize_t sta_tx_latency_stat_write(struct file *file,
					 const char __user *userbuf,
					 size_t count, loff_t *ppos)
{
	struct ieee80211_local *local = file->private_data;
	char buf[128] = {};
	char *bins = buf;
	char *token;
	int buf_size, i, alloc_size;
	int prev_bin = 0;
	int n_ranges = 0;
	int ret = count;
	struct ieee80211_tx_latency_bin_ranges  *tx_latency;

	if (sizeof(buf) <= count)
		return -EINVAL;
	buf_size = count;
	if (copy_from_user(buf, userbuf, buf_size))
		return -EFAULT;

	mutex_lock(&local->sta_mtx);

	/* cannot change config once we have stations */
	if (local->num_sta)
		goto unlock;

	tx_latency =
		rcu_dereference_protected(local->tx_latency,
					  lockdep_is_held(&local->sta_mtx));

	/* disable Tx statistics */
	if (!strcmp(buf, TX_LATENCY_DISABLED)) {
		if (!tx_latency)
			goto unlock;
		RCU_INIT_POINTER(local->tx_latency, NULL);
		synchronize_rcu();
		kfree(tx_latency);
		goto unlock;
	}

	/* Tx latency already enabled */
	if (tx_latency)
		goto unlock;

	if (strcmp(TX_LATENCY_BINS_DISABLED, buf)) {
		/* check how many bins and between what ranges user requested */
		token = buf;
		while (*token != '\0') {
			if (*token == TX_LATENCY_BIN_DELIMTER_C)
				n_ranges++;
			token++;
		}
		n_ranges++;
	}

	alloc_size = sizeof(struct ieee80211_tx_latency_bin_ranges) +
		     n_ranges * sizeof(u32);
	tx_latency = kzalloc(alloc_size, GFP_ATOMIC);
	if (!tx_latency) {
		ret = -ENOMEM;
		goto unlock;
	}
	tx_latency->n_ranges = n_ranges;
	for (i = 0; i < n_ranges; i++) { /* setting bin ranges */
		token = strsep(&bins, TX_LATENCY_BIN_DELIMTER_S);
		sscanf(token, "%d", &tx_latency->ranges[i]);
		/* bins values should be in ascending order */
		if (prev_bin >= tx_latency->ranges[i]) {
			ret = -EINVAL;
			kfree(tx_latency);
			goto unlock;
		}
		prev_bin = tx_latency->ranges[i];
	}
	rcu_assign_pointer(local->tx_latency, tx_latency);

unlock:
	mutex_unlock(&local->sta_mtx);

	return ret;
}

static const struct file_operations stats_tx_latency_ops = {
	.write = sta_tx_latency_stat_write,
	.read = sta_tx_latency_stat_read,
	.open = simple_open,
	.llseek = generic_file_llseek,
};

=======
>>>>>>> v4.9.227
int mac80211_format_buffer(char __user *userbuf, size_t count,
				  loff_t *ppos, char *fmt, ...)
{
	va_list args;
	char buf[DEBUGFS_FORMAT_BUFFER_SIZE];
	int res;

	va_start(args, fmt);
	res = vscnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);

	return simple_read_from_buffer(userbuf, count, ppos, buf, res);
}

#define DEBUGFS_READONLY_FILE_FN(name, fmt, value...)			\
static ssize_t name## _read(struct file *file, char __user *userbuf,	\
			    size_t count, loff_t *ppos)			\
{									\
	struct ieee80211_local *local = file->private_data;		\
									\
	return mac80211_format_buffer(userbuf, count, ppos, 		\
				      fmt "\n", ##value);		\
}

#define DEBUGFS_READONLY_FILE_OPS(name)			\
static const struct file_operations name## _ops = {			\
	.read = name## _read,						\
	.open = simple_open,						\
	.llseek = generic_file_llseek,					\
};

#define DEBUGFS_READONLY_FILE(name, fmt, value...)		\
	DEBUGFS_READONLY_FILE_FN(name, fmt, value)		\
	DEBUGFS_READONLY_FILE_OPS(name)

#define DEBUGFS_ADD(name)						\
	debugfs_create_file(#name, 0400, phyd, local, &name## _ops);

#define DEBUGFS_ADD_MODE(name, mode)					\
	debugfs_create_file(#name, mode, phyd, local, &name## _ops);


DEBUGFS_READONLY_FILE(user_power, "%d",
		      local->user_power_level);
DEBUGFS_READONLY_FILE(power, "%d",
		      local->hw.conf.power_level);
DEBUGFS_READONLY_FILE(total_ps_buffered, "%d",
		      local->total_ps_buffered);
DEBUGFS_READONLY_FILE(wep_iv, "%#08x",
		      local->wep_iv & 0xffffff);
DEBUGFS_READONLY_FILE(rate_ctrl_alg, "%s",
	local->rate_ctrl ? local->rate_ctrl->ops->name : "hw/driver");

<<<<<<< HEAD
=======
static ssize_t aqm_read(struct file *file,
			char __user *user_buf,
			size_t count,
			loff_t *ppos)
{
	struct ieee80211_local *local = file->private_data;
	struct fq *fq = &local->fq;
	char buf[200];
	int len = 0;

	spin_lock_bh(&local->fq.lock);
	rcu_read_lock();

	len = scnprintf(buf, sizeof(buf),
			"access name value\n"
			"R fq_flows_cnt %u\n"
			"R fq_backlog %u\n"
			"R fq_overlimit %u\n"
			"R fq_overmemory %u\n"
			"R fq_collisions %u\n"
			"R fq_memory_usage %u\n"
			"RW fq_memory_limit %u\n"
			"RW fq_limit %u\n"
			"RW fq_quantum %u\n",
			fq->flows_cnt,
			fq->backlog,
			fq->overmemory,
			fq->overlimit,
			fq->collisions,
			fq->memory_usage,
			fq->memory_limit,
			fq->limit,
			fq->quantum);

	rcu_read_unlock();
	spin_unlock_bh(&local->fq.lock);

	return simple_read_from_buffer(user_buf, count, ppos,
				       buf, len);
}

static ssize_t aqm_write(struct file *file,
			 const char __user *user_buf,
			 size_t count,
			 loff_t *ppos)
{
	struct ieee80211_local *local = file->private_data;
	char buf[100];
	size_t len;

	if (count > sizeof(buf))
		return -EINVAL;

	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	buf[sizeof(buf) - 1] = '\0';
	len = strlen(buf);
	if (len > 0 && buf[len-1] == '\n')
		buf[len-1] = 0;

	if (sscanf(buf, "fq_limit %u", &local->fq.limit) == 1)
		return count;
	else if (sscanf(buf, "fq_memory_limit %u", &local->fq.memory_limit) == 1)
		return count;
	else if (sscanf(buf, "fq_quantum %u", &local->fq.quantum) == 1)
		return count;

	return -EINVAL;
}

static const struct file_operations aqm_ops = {
	.write = aqm_write,
	.read = aqm_read,
	.open = simple_open,
	.llseek = default_llseek,
};

>>>>>>> v4.9.227
#ifdef CONFIG_PM
static ssize_t reset_write(struct file *file, const char __user *user_buf,
			   size_t count, loff_t *ppos)
{
	struct ieee80211_local *local = file->private_data;

	rtnl_lock();
	__ieee80211_suspend(&local->hw, NULL);
	__ieee80211_resume(&local->hw);
	rtnl_unlock();

	return count;
}

static const struct file_operations reset_ops = {
	.write = reset_write,
	.open = simple_open,
	.llseek = noop_llseek,
};
#endif

<<<<<<< HEAD
=======
static const char *hw_flag_names[] = {
#define FLAG(F)	[IEEE80211_HW_##F] = #F
	FLAG(HAS_RATE_CONTROL),
	FLAG(RX_INCLUDES_FCS),
	FLAG(HOST_BROADCAST_PS_BUFFERING),
	FLAG(SIGNAL_UNSPEC),
	FLAG(SIGNAL_DBM),
	FLAG(NEED_DTIM_BEFORE_ASSOC),
	FLAG(SPECTRUM_MGMT),
	FLAG(AMPDU_AGGREGATION),
	FLAG(SUPPORTS_PS),
	FLAG(PS_NULLFUNC_STACK),
	FLAG(SUPPORTS_DYNAMIC_PS),
	FLAG(MFP_CAPABLE),
	FLAG(WANT_MONITOR_VIF),
	FLAG(NO_AUTO_VIF),
	FLAG(SW_CRYPTO_CONTROL),
	FLAG(SUPPORT_FAST_XMIT),
	FLAG(REPORTS_TX_ACK_STATUS),
	FLAG(CONNECTION_MONITOR),
	FLAG(QUEUE_CONTROL),
	FLAG(SUPPORTS_PER_STA_GTK),
	FLAG(AP_LINK_PS),
	FLAG(TX_AMPDU_SETUP_IN_HW),
	FLAG(SUPPORTS_RC_TABLE),
	FLAG(P2P_DEV_ADDR_FOR_INTF),
	FLAG(TIMING_BEACON_ONLY),
	FLAG(SUPPORTS_HT_CCK_RATES),
	FLAG(CHANCTX_STA_CSA),
	FLAG(SUPPORTS_CLONED_SKBS),
	FLAG(SINGLE_SCAN_ON_ALL_BANDS),
	FLAG(TDLS_WIDER_BW),
	FLAG(SUPPORTS_AMSDU_IN_AMPDU),
	FLAG(BEACON_TX_STATUS),
	FLAG(NEEDS_UNIQUE_STA_ADDR),
	FLAG(SUPPORTS_REORDERING_BUFFER),
	FLAG(USES_RSS),
	FLAG(TX_AMSDU),
	FLAG(TX_FRAG_LIST),
	FLAG(REPORTS_LOW_ACK),
#undef FLAG
};

>>>>>>> v4.9.227
static ssize_t hwflags_read(struct file *file, char __user *user_buf,
			    size_t count, loff_t *ppos)
{
	struct ieee80211_local *local = file->private_data;
<<<<<<< HEAD
	int mxln = 500;
	ssize_t rv;
	char *buf = kzalloc(mxln, GFP_KERNEL);
	int sf = 0; /* how many written so far */

	if (!buf)
		return 0;

	sf += scnprintf(buf, mxln - sf, "0x%x\n", local->hw.flags);
	if (local->hw.flags & IEEE80211_HW_HAS_RATE_CONTROL)
		sf += scnprintf(buf + sf, mxln - sf, "HAS_RATE_CONTROL\n");
	if (local->hw.flags & IEEE80211_HW_RX_INCLUDES_FCS)
		sf += scnprintf(buf + sf, mxln - sf, "RX_INCLUDES_FCS\n");
	if (local->hw.flags & IEEE80211_HW_HOST_BROADCAST_PS_BUFFERING)
		sf += scnprintf(buf + sf, mxln - sf,
				"HOST_BCAST_PS_BUFFERING\n");
	if (local->hw.flags & IEEE80211_HW_2GHZ_SHORT_SLOT_INCAPABLE)
		sf += scnprintf(buf + sf, mxln - sf,
				"2GHZ_SHORT_SLOT_INCAPABLE\n");
	if (local->hw.flags & IEEE80211_HW_2GHZ_SHORT_PREAMBLE_INCAPABLE)
		sf += scnprintf(buf + sf, mxln - sf,
				"2GHZ_SHORT_PREAMBLE_INCAPABLE\n");
	if (local->hw.flags & IEEE80211_HW_SIGNAL_UNSPEC)
		sf += scnprintf(buf + sf, mxln - sf, "SIGNAL_UNSPEC\n");
	if (local->hw.flags & IEEE80211_HW_SIGNAL_DBM)
		sf += scnprintf(buf + sf, mxln - sf, "SIGNAL_DBM\n");
	if (local->hw.flags & IEEE80211_HW_NEED_DTIM_BEFORE_ASSOC)
		sf += scnprintf(buf + sf, mxln - sf,
				"NEED_DTIM_BEFORE_ASSOC\n");
	if (local->hw.flags & IEEE80211_HW_SPECTRUM_MGMT)
		sf += scnprintf(buf + sf, mxln - sf, "SPECTRUM_MGMT\n");
	if (local->hw.flags & IEEE80211_HW_AMPDU_AGGREGATION)
		sf += scnprintf(buf + sf, mxln - sf, "AMPDU_AGGREGATION\n");
	if (local->hw.flags & IEEE80211_HW_SUPPORTS_PS)
		sf += scnprintf(buf + sf, mxln - sf, "SUPPORTS_PS\n");
	if (local->hw.flags & IEEE80211_HW_PS_NULLFUNC_STACK)
		sf += scnprintf(buf + sf, mxln - sf, "PS_NULLFUNC_STACK\n");
	if (local->hw.flags & IEEE80211_HW_SUPPORTS_DYNAMIC_PS)
		sf += scnprintf(buf + sf, mxln - sf, "SUPPORTS_DYNAMIC_PS\n");
	if (local->hw.flags & IEEE80211_HW_MFP_CAPABLE)
		sf += scnprintf(buf + sf, mxln - sf, "MFP_CAPABLE\n");
	if (local->hw.flags & IEEE80211_HW_SUPPORTS_UAPSD)
		sf += scnprintf(buf + sf, mxln - sf, "SUPPORTS_UAPSD\n");
	if (local->hw.flags & IEEE80211_HW_REPORTS_TX_ACK_STATUS)
		sf += scnprintf(buf + sf, mxln - sf,
				"REPORTS_TX_ACK_STATUS\n");
	if (local->hw.flags & IEEE80211_HW_CONNECTION_MONITOR)
		sf += scnprintf(buf + sf, mxln - sf, "CONNECTION_MONITOR\n");
	if (local->hw.flags & IEEE80211_HW_SUPPORTS_PER_STA_GTK)
		sf += scnprintf(buf + sf, mxln - sf, "SUPPORTS_PER_STA_GTK\n");
	if (local->hw.flags & IEEE80211_HW_AP_LINK_PS)
		sf += scnprintf(buf + sf, mxln - sf, "AP_LINK_PS\n");
	if (local->hw.flags & IEEE80211_HW_TX_AMPDU_SETUP_IN_HW)
		sf += scnprintf(buf + sf, mxln - sf, "TX_AMPDU_SETUP_IN_HW\n");
=======
	size_t bufsz = 30 * NUM_IEEE80211_HW_FLAGS;
	char *buf = kzalloc(bufsz, GFP_KERNEL);
	char *pos = buf, *end = buf + bufsz - 1;
	ssize_t rv;
	int i;

	if (!buf)
		return -ENOMEM;

	/* fail compilation if somebody adds or removes
	 * a flag without updating the name array above
	 */
	BUILD_BUG_ON(ARRAY_SIZE(hw_flag_names) != NUM_IEEE80211_HW_FLAGS);

	for (i = 0; i < NUM_IEEE80211_HW_FLAGS; i++) {
		if (test_bit(i, local->hw.flags))
			pos += scnprintf(pos, end - pos, "%s\n",
					 hw_flag_names[i]);
	}
>>>>>>> v4.9.227

	rv = simple_read_from_buffer(user_buf, count, ppos, buf, strlen(buf));
	kfree(buf);
	return rv;
}

static ssize_t queues_read(struct file *file, char __user *user_buf,
			   size_t count, loff_t *ppos)
{
	struct ieee80211_local *local = file->private_data;
	unsigned long flags;
	char buf[IEEE80211_MAX_QUEUES * 20];
	int q, res = 0;

	spin_lock_irqsave(&local->queue_stop_reason_lock, flags);
	for (q = 0; q < local->hw.queues; q++)
		res += sprintf(buf + res, "%02d: %#.8lx/%d\n", q,
				local->queue_stop_reasons[q],
				skb_queue_len(&local->pending[q]));
	spin_unlock_irqrestore(&local->queue_stop_reason_lock, flags);

	return simple_read_from_buffer(user_buf, count, ppos, buf, res);
}

DEBUGFS_READONLY_FILE_OPS(hwflags);
DEBUGFS_READONLY_FILE_OPS(queues);

/* statistics stuff */

static ssize_t format_devstat_counter(struct ieee80211_local *local,
	char __user *userbuf,
	size_t count, loff_t *ppos,
	int (*printvalue)(struct ieee80211_low_level_stats *stats, char *buf,
			  int buflen))
{
	struct ieee80211_low_level_stats stats;
	char buf[20];
	int res;

	rtnl_lock();
	res = drv_get_stats(local, &stats);
	rtnl_unlock();
	if (res)
		return res;
	res = printvalue(&stats, buf, sizeof(buf));
	return simple_read_from_buffer(userbuf, count, ppos, buf, res);
}

#define DEBUGFS_DEVSTATS_FILE(name)					\
static int print_devstats_##name(struct ieee80211_low_level_stats *stats,\
				 char *buf, int buflen)			\
{									\
	return scnprintf(buf, buflen, "%u\n", stats->name);		\
}									\
static ssize_t stats_ ##name## _read(struct file *file,			\
				     char __user *userbuf,		\
				     size_t count, loff_t *ppos)	\
{									\
	return format_devstat_counter(file->private_data,		\
				      userbuf,				\
				      count,				\
				      ppos,				\
				      print_devstats_##name);		\
}									\
									\
static const struct file_operations stats_ ##name## _ops = {		\
	.read = stats_ ##name## _read,					\
	.open = simple_open,						\
	.llseek = generic_file_llseek,					\
};

<<<<<<< HEAD
#define DEBUGFS_STATS_ADD(name, field)					\
	debugfs_create_u32(#name, 0400, statsd, (u32 *) &field);
=======
#define DEBUGFS_STATS_ADD(name)					\
	debugfs_create_u32(#name, 0400, statsd, &local->name);
>>>>>>> v4.9.227
#define DEBUGFS_DEVSTATS_ADD(name)					\
	debugfs_create_file(#name, 0400, statsd, local, &stats_ ##name## _ops);

DEBUGFS_DEVSTATS_FILE(dot11ACKFailureCount);
DEBUGFS_DEVSTATS_FILE(dot11RTSFailureCount);
DEBUGFS_DEVSTATS_FILE(dot11FCSErrorCount);
DEBUGFS_DEVSTATS_FILE(dot11RTSSuccessCount);

void debugfs_hw_add(struct ieee80211_local *local)
{
	struct dentry *phyd = local->hw.wiphy->debugfsdir;
	struct dentry *statsd;

	if (!phyd)
		return;

	local->debugfs.keys = debugfs_create_dir("keys", phyd);

	DEBUGFS_ADD(total_ps_buffered);
	DEBUGFS_ADD(wep_iv);
	DEBUGFS_ADD(queues);
#ifdef CONFIG_PM
	DEBUGFS_ADD_MODE(reset, 0200);
#endif
	DEBUGFS_ADD(hwflags);
	DEBUGFS_ADD(user_power);
	DEBUGFS_ADD(power);

<<<<<<< HEAD
=======
	if (local->ops->wake_tx_queue)
		DEBUGFS_ADD_MODE(aqm, 0600);

>>>>>>> v4.9.227
	statsd = debugfs_create_dir("statistics", phyd);

	/* if the dir failed, don't put all the other things into the root! */
	if (!statsd)
		return;

<<<<<<< HEAD
	DEBUGFS_STATS_ADD(transmitted_fragment_count,
		local->dot11TransmittedFragmentCount);
	DEBUGFS_STATS_ADD(multicast_transmitted_frame_count,
		local->dot11MulticastTransmittedFrameCount);
	DEBUGFS_STATS_ADD(failed_count, local->dot11FailedCount);
	DEBUGFS_STATS_ADD(retry_count, local->dot11RetryCount);
	DEBUGFS_STATS_ADD(multiple_retry_count,
		local->dot11MultipleRetryCount);
	DEBUGFS_STATS_ADD(frame_duplicate_count,
		local->dot11FrameDuplicateCount);
	DEBUGFS_STATS_ADD(received_fragment_count,
		local->dot11ReceivedFragmentCount);
	DEBUGFS_STATS_ADD(multicast_received_frame_count,
		local->dot11MulticastReceivedFrameCount);
	DEBUGFS_STATS_ADD(transmitted_frame_count,
		local->dot11TransmittedFrameCount);
#ifdef CONFIG_MAC80211_DEBUG_COUNTERS
	DEBUGFS_STATS_ADD(tx_handlers_drop, local->tx_handlers_drop);
	DEBUGFS_STATS_ADD(tx_handlers_queued, local->tx_handlers_queued);
	DEBUGFS_STATS_ADD(tx_handlers_drop_unencrypted,
		local->tx_handlers_drop_unencrypted);
	DEBUGFS_STATS_ADD(tx_handlers_drop_fragment,
		local->tx_handlers_drop_fragment);
	DEBUGFS_STATS_ADD(tx_handlers_drop_wep,
		local->tx_handlers_drop_wep);
	DEBUGFS_STATS_ADD(tx_handlers_drop_not_assoc,
		local->tx_handlers_drop_not_assoc);
	DEBUGFS_STATS_ADD(tx_handlers_drop_unauth_port,
		local->tx_handlers_drop_unauth_port);
	DEBUGFS_STATS_ADD(rx_handlers_drop, local->rx_handlers_drop);
	DEBUGFS_STATS_ADD(rx_handlers_queued, local->rx_handlers_queued);
	DEBUGFS_STATS_ADD(rx_handlers_drop_nullfunc,
		local->rx_handlers_drop_nullfunc);
	DEBUGFS_STATS_ADD(rx_handlers_drop_defrag,
		local->rx_handlers_drop_defrag);
	DEBUGFS_STATS_ADD(rx_handlers_drop_short,
		local->rx_handlers_drop_short);
	DEBUGFS_STATS_ADD(tx_expand_skb_head,
		local->tx_expand_skb_head);
	DEBUGFS_STATS_ADD(tx_expand_skb_head_cloned,
		local->tx_expand_skb_head_cloned);
	DEBUGFS_STATS_ADD(rx_expand_skb_head,
		local->rx_expand_skb_head);
	DEBUGFS_STATS_ADD(rx_expand_skb_head2,
		local->rx_expand_skb_head2);
	DEBUGFS_STATS_ADD(rx_handlers_fragments,
		local->rx_handlers_fragments);
	DEBUGFS_STATS_ADD(tx_status_drop,
		local->tx_status_drop);
=======
#ifdef CONFIG_MAC80211_DEBUG_COUNTERS
	DEBUGFS_STATS_ADD(dot11TransmittedFragmentCount);
	DEBUGFS_STATS_ADD(dot11MulticastTransmittedFrameCount);
	DEBUGFS_STATS_ADD(dot11FailedCount);
	DEBUGFS_STATS_ADD(dot11RetryCount);
	DEBUGFS_STATS_ADD(dot11MultipleRetryCount);
	DEBUGFS_STATS_ADD(dot11FrameDuplicateCount);
	DEBUGFS_STATS_ADD(dot11ReceivedFragmentCount);
	DEBUGFS_STATS_ADD(dot11MulticastReceivedFrameCount);
	DEBUGFS_STATS_ADD(dot11TransmittedFrameCount);
	DEBUGFS_STATS_ADD(tx_handlers_drop);
	DEBUGFS_STATS_ADD(tx_handlers_queued);
	DEBUGFS_STATS_ADD(tx_handlers_drop_wep);
	DEBUGFS_STATS_ADD(tx_handlers_drop_not_assoc);
	DEBUGFS_STATS_ADD(tx_handlers_drop_unauth_port);
	DEBUGFS_STATS_ADD(rx_handlers_drop);
	DEBUGFS_STATS_ADD(rx_handlers_queued);
	DEBUGFS_STATS_ADD(rx_handlers_drop_nullfunc);
	DEBUGFS_STATS_ADD(rx_handlers_drop_defrag);
	DEBUGFS_STATS_ADD(tx_expand_skb_head);
	DEBUGFS_STATS_ADD(tx_expand_skb_head_cloned);
	DEBUGFS_STATS_ADD(rx_expand_skb_head_defrag);
	DEBUGFS_STATS_ADD(rx_handlers_fragments);
	DEBUGFS_STATS_ADD(tx_status_drop);
>>>>>>> v4.9.227
#endif
	DEBUGFS_DEVSTATS_ADD(dot11ACKFailureCount);
	DEBUGFS_DEVSTATS_ADD(dot11RTSFailureCount);
	DEBUGFS_DEVSTATS_ADD(dot11FCSErrorCount);
	DEBUGFS_DEVSTATS_ADD(dot11RTSSuccessCount);
<<<<<<< HEAD

	DEBUGFS_DEVSTATS_ADD(tx_latency);
=======
>>>>>>> v4.9.227
}
