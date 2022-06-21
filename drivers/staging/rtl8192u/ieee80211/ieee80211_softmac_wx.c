/* IEEE 802.11 SoftMAC layer
 * Copyright (c) 2005 Andrea Merello <andrea.merello@gmail.com>
 *
 * Mostly extracted from the rtl8180-sa2400 driver for the
 * in-kernel generic ieee802.11 stack.
 *
 * Some pieces of code might be stolen from ipw2100 driver
 * copyright of who own it's copyright ;-)
 *
 * PS wx handler mostly stolen from hostap, copyright who
 * own it's copyright ;-)
 *
 * released under the GPL
 */


#include <linux/etherdevice.h>

#include "ieee80211.h"
#include "dot11d.h"
/* FIXME: add A freqs */

const long ieee80211_wlan_frequencies[] = {
	2412, 2417, 2422, 2427,
	2432, 2437, 2442, 2447,
	2452, 2457, 2462, 2467,
	2472, 2484
};
EXPORT_SYMBOL(ieee80211_wlan_frequencies);

int ieee80211_wx_set_freq(struct ieee80211_device *ieee, struct iw_request_info *a,
			     union iwreq_data *wrqu, char *b)
{
	int ret;
<<<<<<< HEAD
	struct iw_freq *fwrq = & wrqu->freq;

	down(&ieee->wx_sem);

	if(ieee->iw_mode == IW_MODE_INFRA){
=======
	struct iw_freq *fwrq = &wrqu->freq;

	mutex_lock(&ieee->wx_mutex);

	if (ieee->iw_mode == IW_MODE_INFRA) {
>>>>>>> v4.9.227
		ret = -EOPNOTSUPP;
		goto out;
	}

	/* if setting by freq convert to channel */
	if (fwrq->e == 1) {
		if ((fwrq->m >= (int) 2.412e8 &&
		     fwrq->m <= (int) 2.487e8)) {
			int f = fwrq->m / 100000;
			int c = 0;

			while ((c < 14) && (f != ieee80211_wlan_frequencies[c]))
				c++;

			/* hack to fall through */
			fwrq->e = 0;
			fwrq->m = c + 1;
		}
	}

<<<<<<< HEAD
	if (fwrq->e > 0 || fwrq->m > 14 || fwrq->m < 1 ){
		ret = -EOPNOTSUPP;
		goto out;

	}else { /* Set the channel */
=======
	if (fwrq->e > 0 || fwrq->m > 14 || fwrq->m < 1) {
		ret = -EOPNOTSUPP;
		goto out;

	} else { /* Set the channel */
>>>>>>> v4.9.227

		if (!(GET_DOT11D_INFO(ieee)->channel_map)[fwrq->m]) {
			ret = -EINVAL;
			goto out;
		}
		ieee->current_network.channel = fwrq->m;
		ieee->set_chan(ieee->dev, ieee->current_network.channel);

<<<<<<< HEAD
		if(ieee->iw_mode == IW_MODE_ADHOC || ieee->iw_mode == IW_MODE_MASTER)
			if(ieee->state == IEEE80211_LINKED){

			ieee80211_stop_send_beacons(ieee);
			ieee80211_start_send_beacons(ieee);
=======
		if (ieee->iw_mode == IW_MODE_ADHOC || ieee->iw_mode == IW_MODE_MASTER)
			if (ieee->state == IEEE80211_LINKED) {
				ieee80211_stop_send_beacons(ieee);
				ieee80211_start_send_beacons(ieee);
>>>>>>> v4.9.227
			}
	}

	ret = 0;
out:
<<<<<<< HEAD
	up(&ieee->wx_sem);
=======
	mutex_unlock(&ieee->wx_mutex);
>>>>>>> v4.9.227
	return ret;
}
EXPORT_SYMBOL(ieee80211_wx_set_freq);

int ieee80211_wx_get_freq(struct ieee80211_device *ieee,
			     struct iw_request_info *a,
			     union iwreq_data *wrqu, char *b)
{
<<<<<<< HEAD
	struct iw_freq *fwrq = & wrqu->freq;

	if (ieee->current_network.channel == 0)
		return -1;
	//NM 0.7.0 will not accept channel any more.
	fwrq->m = ieee80211_wlan_frequencies[ieee->current_network.channel-1] * 100000;
	fwrq->e = 1;
//	fwrq->m = ieee->current_network.channel;
//	fwrq->e = 0;
=======
	struct iw_freq *fwrq = &wrqu->freq;

	if (ieee->current_network.channel == 0)
		return -1;
	/* NM 0.7.0 will not accept channel any more. */
	fwrq->m = ieee80211_wlan_frequencies[ieee->current_network.channel-1] * 100000;
	fwrq->e = 1;
	/* fwrq->m = ieee->current_network.channel; */
	/* fwrq->e = 0; */
>>>>>>> v4.9.227

	return 0;
}
EXPORT_SYMBOL(ieee80211_wx_get_freq);

int ieee80211_wx_get_wap(struct ieee80211_device *ieee,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *extra)
{
	unsigned long flags;

	wrqu->ap_addr.sa_family = ARPHRD_ETHER;

	if (ieee->iw_mode == IW_MODE_MONITOR)
		return -1;

	/* We want avoid to give to the user inconsistent infos*/
	spin_lock_irqsave(&ieee->lock, flags);

	if (ieee->state != IEEE80211_LINKED &&
		ieee->state != IEEE80211_LINKED_SCANNING &&
		ieee->wap_set == 0)

<<<<<<< HEAD
		memset(wrqu->ap_addr.sa_data, 0, ETH_ALEN);
=======
		eth_zero_addr(wrqu->ap_addr.sa_data);
>>>>>>> v4.9.227
	else
		memcpy(wrqu->ap_addr.sa_data,
		       ieee->current_network.bssid, ETH_ALEN);

	spin_unlock_irqrestore(&ieee->lock, flags);

	return 0;
}
EXPORT_SYMBOL(ieee80211_wx_get_wap);

int ieee80211_wx_set_wap(struct ieee80211_device *ieee,
			 struct iw_request_info *info,
			 union iwreq_data *awrq,
			 char *extra)
{

	int ret = 0;
	unsigned long flags;

<<<<<<< HEAD
	short ifup = ieee->proto_started;//dev->flags & IFF_UP;
=======
	short ifup = ieee->proto_started; /* dev->flags & IFF_UP; */
>>>>>>> v4.9.227
	struct sockaddr *temp = (struct sockaddr *)awrq;

	ieee->sync_scan_hurryup = 1;

<<<<<<< HEAD
	down(&ieee->wx_sem);
	/* use ifconfig hw ether */
	if (ieee->iw_mode == IW_MODE_MASTER){
=======
	mutex_lock(&ieee->wx_mutex);
	/* use ifconfig hw ether */
	if (ieee->iw_mode == IW_MODE_MASTER) {
>>>>>>> v4.9.227
		ret = -1;
		goto out;
	}

<<<<<<< HEAD
	if (temp->sa_family != ARPHRD_ETHER){
=======
	if (temp->sa_family != ARPHRD_ETHER) {
>>>>>>> v4.9.227
		ret = -EINVAL;
		goto out;
	}

	if (ifup)
		ieee80211_stop_protocol(ieee);

	/* just to avoid to give inconsistent infos in the
	 * get wx method. not really needed otherwise
	 */
	spin_lock_irqsave(&ieee->lock, flags);

	memcpy(ieee->current_network.bssid, temp->sa_data, ETH_ALEN);
	ieee->wap_set = !is_zero_ether_addr(temp->sa_data);

	spin_unlock_irqrestore(&ieee->lock, flags);

	if (ifup)
		ieee80211_start_protocol(ieee);
out:
<<<<<<< HEAD
	up(&ieee->wx_sem);
=======
	mutex_unlock(&ieee->wx_mutex);
>>>>>>> v4.9.227
	return ret;
}
EXPORT_SYMBOL(ieee80211_wx_set_wap);

<<<<<<< HEAD
 int ieee80211_wx_get_essid(struct ieee80211_device *ieee, struct iw_request_info *a,union iwreq_data *wrqu,char *b)
=======
int ieee80211_wx_get_essid(struct ieee80211_device *ieee, struct iw_request_info *a, union iwreq_data *wrqu, char *b)
>>>>>>> v4.9.227
{
	int len, ret = 0;
	unsigned long flags;

	if (ieee->iw_mode == IW_MODE_MONITOR)
		return -1;

	/* We want avoid to give to the user inconsistent infos*/
	spin_lock_irqsave(&ieee->lock, flags);

	if (ieee->current_network.ssid[0] == '\0' ||
<<<<<<< HEAD
		ieee->current_network.ssid_len == 0){
=======
		ieee->current_network.ssid_len == 0) {
>>>>>>> v4.9.227
		ret = -1;
		goto out;
	}

	if (ieee->state != IEEE80211_LINKED &&
		ieee->state != IEEE80211_LINKED_SCANNING &&
<<<<<<< HEAD
		ieee->ssid_set == 0){
=======
		ieee->ssid_set == 0) {
>>>>>>> v4.9.227
		ret = -1;
		goto out;
	}
	len = ieee->current_network.ssid_len;
	wrqu->essid.length = len;
	strncpy(b, ieee->current_network.ssid, len);
	wrqu->essid.flags = 1;

out:
	spin_unlock_irqrestore(&ieee->lock, flags);

	return ret;

}
EXPORT_SYMBOL(ieee80211_wx_get_essid);

int ieee80211_wx_set_rate(struct ieee80211_device *ieee,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{

	u32 target_rate = wrqu->bitrate.value;

	ieee->rate = target_rate/100000;
<<<<<<< HEAD
	//FIXME: we might want to limit rate also in management protocols.
=======
	/* FIXME: we might want to limit rate also in management protocols. */
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL(ieee80211_wx_set_rate);

int ieee80211_wx_get_rate(struct ieee80211_device *ieee,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	u32 tmp_rate;
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
	tmp_rate = TxCountToDataRate(ieee, ieee->softmac_stats.CurrentShowTxate);

	wrqu->bitrate.value = tmp_rate * 500000;

	return 0;
}
EXPORT_SYMBOL(ieee80211_wx_get_rate);

int ieee80211_wx_set_rts(struct ieee80211_device *ieee,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	if (wrqu->rts.disabled || !wrqu->rts.fixed)
		ieee->rts = DEFAULT_RTS_THRESHOLD;
<<<<<<< HEAD
	else
	{
=======
	else {
>>>>>>> v4.9.227
		if (wrqu->rts.value < MIN_RTS_THRESHOLD ||
				wrqu->rts.value > MAX_RTS_THRESHOLD)
			return -EINVAL;
		ieee->rts = wrqu->rts.value;
	}
	return 0;
}
EXPORT_SYMBOL(ieee80211_wx_set_rts);

int ieee80211_wx_get_rts(struct ieee80211_device *ieee,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	wrqu->rts.value = ieee->rts;
	wrqu->rts.fixed = 0;	/* no auto select */
	wrqu->rts.disabled = (wrqu->rts.value == DEFAULT_RTS_THRESHOLD);
	return 0;
}
EXPORT_SYMBOL(ieee80211_wx_get_rts);

int ieee80211_wx_set_mode(struct ieee80211_device *ieee, struct iw_request_info *a,
			     union iwreq_data *wrqu, char *b)
{

	ieee->sync_scan_hurryup = 1;

<<<<<<< HEAD
	down(&ieee->wx_sem);
=======
	mutex_lock(&ieee->wx_mutex);
>>>>>>> v4.9.227

	if (wrqu->mode == ieee->iw_mode)
		goto out;

<<<<<<< HEAD
	if (wrqu->mode == IW_MODE_MONITOR){

		ieee->dev->type = ARPHRD_IEEE80211;
	}else{
		ieee->dev->type = ARPHRD_ETHER;
	}

	if (!ieee->proto_started){
		ieee->iw_mode = wrqu->mode;
	}else{
=======
	if (wrqu->mode == IW_MODE_MONITOR)
		ieee->dev->type = ARPHRD_IEEE80211;
	else
		ieee->dev->type = ARPHRD_ETHER;

	if (!ieee->proto_started) {
		ieee->iw_mode = wrqu->mode;
	} else {
>>>>>>> v4.9.227
		ieee80211_stop_protocol(ieee);
		ieee->iw_mode = wrqu->mode;
		ieee80211_start_protocol(ieee);
	}

out:
<<<<<<< HEAD
	up(&ieee->wx_sem);
=======
	mutex_unlock(&ieee->wx_mutex);
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL(ieee80211_wx_set_mode);

void ieee80211_wx_sync_scan_wq(struct work_struct *work)
{
	struct ieee80211_device *ieee = container_of(work, struct ieee80211_device, wx_sync_scan_wq);
	short chan;
<<<<<<< HEAD
	HT_EXTCHNL_OFFSET chan_offset=0;
	HT_CHANNEL_WIDTH bandwidth=0;
	int b40M = 0;
	static int count;
=======
	HT_EXTCHNL_OFFSET chan_offset = 0;
	HT_CHANNEL_WIDTH bandwidth = 0;
	int b40M = 0;

>>>>>>> v4.9.227
	chan = ieee->current_network.channel;
	netif_carrier_off(ieee->dev);

	if (ieee->data_hard_stop)
		ieee->data_hard_stop(ieee->dev);

	ieee80211_stop_send_beacons(ieee);

	ieee->state = IEEE80211_LINKED_SCANNING;
	ieee->link_change(ieee->dev);
	ieee->InitialGainHandler(ieee->dev, IG_Backup);
	if (ieee->pHTInfo->bCurrentHTSupport && ieee->pHTInfo->bEnableHT && ieee->pHTInfo->bCurBW40MHz) {
		b40M = 1;
		chan_offset = ieee->pHTInfo->CurSTAExtChnlOffset;
		bandwidth = (HT_CHANNEL_WIDTH)ieee->pHTInfo->bCurBW40MHz;
		printk("Scan in 40M, force to 20M first:%d, %d\n", chan_offset, bandwidth);
		ieee->SetBWModeHandler(ieee->dev, HT_CHANNEL_WIDTH_20, HT_EXTCHNL_OFFSET_NO_EXT);
		}
	ieee80211_start_scan_syncro(ieee);
	if (b40M) {
		printk("Scan in 20M, back to 40M\n");
		if (chan_offset == HT_EXTCHNL_OFFSET_UPPER)
			ieee->set_chan(ieee->dev, chan + 2);
		else if (chan_offset == HT_EXTCHNL_OFFSET_LOWER)
			ieee->set_chan(ieee->dev, chan - 2);
		else
			ieee->set_chan(ieee->dev, chan);
		ieee->SetBWModeHandler(ieee->dev, bandwidth, chan_offset);
	} else {
		ieee->set_chan(ieee->dev, chan);
	}

	ieee->InitialGainHandler(ieee->dev, IG_Restore);
	ieee->state = IEEE80211_LINKED;
	ieee->link_change(ieee->dev);
<<<<<<< HEAD
	// To prevent the immediately calling watch_dog after scan.
	if(ieee->LinkDetectInfo.NumRecvBcnInPeriod==0||ieee->LinkDetectInfo.NumRecvDataInPeriod==0 )
	{
		ieee->LinkDetectInfo.NumRecvBcnInPeriod = 1;
		ieee->LinkDetectInfo.NumRecvDataInPeriod= 1;
=======
	/* To prevent the immediately calling watch_dog after scan. */
	if (ieee->LinkDetectInfo.NumRecvBcnInPeriod == 0 || ieee->LinkDetectInfo.NumRecvDataInPeriod == 0) {
		ieee->LinkDetectInfo.NumRecvBcnInPeriod = 1;
		ieee->LinkDetectInfo.NumRecvDataInPeriod = 1;
>>>>>>> v4.9.227
	}
	if (ieee->data_hard_resume)
		ieee->data_hard_resume(ieee->dev);

<<<<<<< HEAD
	if(ieee->iw_mode == IW_MODE_ADHOC || ieee->iw_mode == IW_MODE_MASTER)
		ieee80211_start_send_beacons(ieee);

	netif_carrier_on(ieee->dev);
	count = 0;
	up(&ieee->wx_sem);
=======
	if (ieee->iw_mode == IW_MODE_ADHOC || ieee->iw_mode == IW_MODE_MASTER)
		ieee80211_start_send_beacons(ieee);

	netif_carrier_on(ieee->dev);
	mutex_unlock(&ieee->wx_mutex);
>>>>>>> v4.9.227

}

int ieee80211_wx_set_scan(struct ieee80211_device *ieee, struct iw_request_info *a,
			     union iwreq_data *wrqu, char *b)
{
	int ret = 0;

<<<<<<< HEAD
	down(&ieee->wx_sem);

	if (ieee->iw_mode == IW_MODE_MONITOR || !(ieee->proto_started)){
=======
	mutex_lock(&ieee->wx_mutex);

	if (ieee->iw_mode == IW_MODE_MONITOR || !(ieee->proto_started)) {
>>>>>>> v4.9.227
		ret = -1;
		goto out;
	}

<<<<<<< HEAD
	if ( ieee->state == IEEE80211_LINKED){
=======
	if (ieee->state == IEEE80211_LINKED) {
>>>>>>> v4.9.227
		queue_work(ieee->wq, &ieee->wx_sync_scan_wq);
		/* intentionally forget to up sem */
		return 0;
	}

out:
<<<<<<< HEAD
	up(&ieee->wx_sem);
=======
	mutex_unlock(&ieee->wx_mutex);
>>>>>>> v4.9.227
	return ret;
}
EXPORT_SYMBOL(ieee80211_wx_set_scan);

int ieee80211_wx_set_essid(struct ieee80211_device *ieee,
			      struct iw_request_info *a,
			      union iwreq_data *wrqu, char *extra)
{

<<<<<<< HEAD
	int ret=0,len;
=======
	int ret = 0, len;
>>>>>>> v4.9.227
	short proto_started;
	unsigned long flags;

	ieee->sync_scan_hurryup = 1;
<<<<<<< HEAD
	down(&ieee->wx_sem);

	proto_started = ieee->proto_started;

	if (wrqu->essid.length > IW_ESSID_MAX_SIZE){
		ret= -E2BIG;
		goto out;
	}

	if (ieee->iw_mode == IW_MODE_MONITOR){
		ret= -1;
		goto out;
	}

	if(proto_started)
=======
	mutex_lock(&ieee->wx_mutex);

	proto_started = ieee->proto_started;

	if (wrqu->essid.length > IW_ESSID_MAX_SIZE) {
		ret = -E2BIG;
		goto out;
	}

	if (ieee->iw_mode == IW_MODE_MONITOR) {
		ret = -1;
		goto out;
	}

	if (proto_started)
>>>>>>> v4.9.227
		ieee80211_stop_protocol(ieee);


	/* this is just to be sure that the GET wx callback
	 * has consisten infos. not needed otherwise
	 */
	spin_lock_irqsave(&ieee->lock, flags);

	if (wrqu->essid.flags && wrqu->essid.length) {
<<<<<<< HEAD
		//first flush current network.ssid
=======
		/* first flush current network.ssid */
>>>>>>> v4.9.227
		len = ((wrqu->essid.length-1) < IW_ESSID_MAX_SIZE) ? (wrqu->essid.length-1) : IW_ESSID_MAX_SIZE;
		strncpy(ieee->current_network.ssid, extra, len+1);
		ieee->current_network.ssid_len = len+1;
		ieee->ssid_set = 1;
<<<<<<< HEAD
	}
	else{
=======
	} else {
>>>>>>> v4.9.227
		ieee->ssid_set = 0;
		ieee->current_network.ssid[0] = '\0';
		ieee->current_network.ssid_len = 0;
	}
	spin_unlock_irqrestore(&ieee->lock, flags);

	if (proto_started)
		ieee80211_start_protocol(ieee);
out:
<<<<<<< HEAD
	up(&ieee->wx_sem);
=======
	mutex_unlock(&ieee->wx_mutex);
>>>>>>> v4.9.227
	return ret;
}
EXPORT_SYMBOL(ieee80211_wx_set_essid);

<<<<<<< HEAD
 int ieee80211_wx_get_mode(struct ieee80211_device *ieee, struct iw_request_info *a,
=======
int ieee80211_wx_get_mode(struct ieee80211_device *ieee, struct iw_request_info *a,
>>>>>>> v4.9.227
			     union iwreq_data *wrqu, char *b)
{

	wrqu->mode = ieee->iw_mode;
	return 0;
}
EXPORT_SYMBOL(ieee80211_wx_get_mode);

<<<<<<< HEAD
 int ieee80211_wx_set_rawtx(struct ieee80211_device *ieee,
=======
int ieee80211_wx_set_rawtx(struct ieee80211_device *ieee,
>>>>>>> v4.9.227
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{

	int *parms = (int *)extra;
	int enable = (parms[0] > 0);
	short prev = ieee->raw_tx;

<<<<<<< HEAD
	down(&ieee->wx_sem);

	if(enable)
=======
	mutex_lock(&ieee->wx_mutex);

	if (enable)
>>>>>>> v4.9.227
		ieee->raw_tx = 1;
	else
		ieee->raw_tx = 0;

	printk(KERN_INFO"raw TX is %s\n",
	      ieee->raw_tx ? "enabled" : "disabled");

<<<<<<< HEAD
	if(ieee->iw_mode == IW_MODE_MONITOR)
	{
		if(prev == 0 && ieee->raw_tx){
=======
	if (ieee->iw_mode == IW_MODE_MONITOR) {
		if (prev == 0 && ieee->raw_tx) {
>>>>>>> v4.9.227
			if (ieee->data_hard_resume)
				ieee->data_hard_resume(ieee->dev);

			netif_carrier_on(ieee->dev);
		}

<<<<<<< HEAD
		if(prev && ieee->raw_tx == 1)
			netif_carrier_off(ieee->dev);
	}

	up(&ieee->wx_sem);
=======
		if (prev && ieee->raw_tx == 1)
			netif_carrier_off(ieee->dev);
	}

	mutex_unlock(&ieee->wx_mutex);
>>>>>>> v4.9.227

	return 0;
}
EXPORT_SYMBOL(ieee80211_wx_set_rawtx);

int ieee80211_wx_get_name(struct ieee80211_device *ieee,
			     struct iw_request_info *info,
			     union iwreq_data *wrqu, char *extra)
{
	strlcpy(wrqu->name, "802.11", IFNAMSIZ);
	if (ieee->modulation & IEEE80211_CCK_MODULATION) {
		strlcat(wrqu->name, "b", IFNAMSIZ);
		if (ieee->modulation & IEEE80211_OFDM_MODULATION)
			strlcat(wrqu->name, "/g", IFNAMSIZ);
	} else if (ieee->modulation & IEEE80211_OFDM_MODULATION) {
		strlcat(wrqu->name, "g", IFNAMSIZ);
	}

	if (ieee->mode & (IEEE_N_24G | IEEE_N_5G))
		strlcat(wrqu->name, "/n", IFNAMSIZ);

	if ((ieee->state == IEEE80211_LINKED) ||
	    (ieee->state == IEEE80211_LINKED_SCANNING))
		strlcat(wrqu->name, " linked", IFNAMSIZ);
	else if (ieee->state != IEEE80211_NOLINK)
		strlcat(wrqu->name, " link..", IFNAMSIZ);

	return 0;
}
EXPORT_SYMBOL(ieee80211_wx_get_name);

/* this is mostly stolen from hostap */
int ieee80211_wx_set_power(struct ieee80211_device *ieee,
				 struct iw_request_info *info,
				 union iwreq_data *wrqu, char *extra)
{
	int ret = 0;
<<<<<<< HEAD
	down(&ieee->wx_sem);

	if (wrqu->power.disabled){
=======

	mutex_lock(&ieee->wx_mutex);

	if (wrqu->power.disabled) {
>>>>>>> v4.9.227
		ieee->ps = IEEE80211_PS_DISABLED;
		goto exit;
	}
	if (wrqu->power.flags & IW_POWER_TIMEOUT) {
<<<<<<< HEAD
		//ieee->ps_period = wrqu->power.value / 1000;
=======
		/* ieee->ps_period = wrqu->power.value / 1000; */
>>>>>>> v4.9.227
		ieee->ps_timeout = wrqu->power.value / 1000;
	}

	if (wrqu->power.flags & IW_POWER_PERIOD) {

<<<<<<< HEAD
		//ieee->ps_timeout = wrqu->power.value / 1000;
		ieee->ps_period = wrqu->power.value / 1000;
		//wrq->value / 1024;
=======
		/* ieee->ps_timeout = wrqu->power.value / 1000; */
		ieee->ps_period = wrqu->power.value / 1000;
		/* wrq->value / 1024; */
>>>>>>> v4.9.227

	}
	switch (wrqu->power.flags & IW_POWER_MODE) {
	case IW_POWER_UNICAST_R:
		ieee->ps = IEEE80211_PS_UNICAST;
		break;
	case IW_POWER_MULTICAST_R:
		ieee->ps = IEEE80211_PS_MBCAST;
		break;
	case IW_POWER_ALL_R:
		ieee->ps = IEEE80211_PS_UNICAST | IEEE80211_PS_MBCAST;
		break;

	case IW_POWER_ON:
<<<<<<< HEAD
	//	ieee->ps = IEEE80211_PS_DISABLED;
=======
		/* ieee->ps = IEEE80211_PS_DISABLED; */
>>>>>>> v4.9.227
		break;

	default:
		ret = -EINVAL;
		goto exit;

	}
exit:
<<<<<<< HEAD
	up(&ieee->wx_sem);
=======
	mutex_unlock(&ieee->wx_mutex);
>>>>>>> v4.9.227
	return ret;

}
EXPORT_SYMBOL(ieee80211_wx_set_power);

/* this is stolen from hostap */
int ieee80211_wx_get_power(struct ieee80211_device *ieee,
				 struct iw_request_info *info,
				 union iwreq_data *wrqu, char *extra)
{
<<<<<<< HEAD
	down(&ieee->wx_sem);

	if(ieee->ps == IEEE80211_PS_DISABLED){
=======
	mutex_lock(&ieee->wx_mutex);

	if (ieee->ps == IEEE80211_PS_DISABLED) {
>>>>>>> v4.9.227
		wrqu->power.disabled = 1;
		goto exit;
	}

	wrqu->power.disabled = 0;

	if ((wrqu->power.flags & IW_POWER_TYPE) == IW_POWER_TIMEOUT) {
		wrqu->power.flags = IW_POWER_TIMEOUT;
		wrqu->power.value = ieee->ps_timeout * 1000;
	} else {
<<<<<<< HEAD
//		ret = -EOPNOTSUPP;
//		goto exit;
		wrqu->power.flags = IW_POWER_PERIOD;
		wrqu->power.value = ieee->ps_period * 1000;
//ieee->current_network.dtim_period * ieee->current_network.beacon_interval * 1024;
	}

       if ((ieee->ps & (IEEE80211_PS_MBCAST | IEEE80211_PS_UNICAST)) == (IEEE80211_PS_MBCAST | IEEE80211_PS_UNICAST))
=======
		/* ret = -EOPNOTSUPP; */
		/* goto exit; */
		wrqu->power.flags = IW_POWER_PERIOD;
		wrqu->power.value = ieee->ps_period * 1000;
		/* ieee->current_network.dtim_period * ieee->current_network.beacon_interval * 1024; */
	}

	if ((ieee->ps & (IEEE80211_PS_MBCAST | IEEE80211_PS_UNICAST)) == (IEEE80211_PS_MBCAST | IEEE80211_PS_UNICAST))
>>>>>>> v4.9.227
		wrqu->power.flags |= IW_POWER_ALL_R;
	else if (ieee->ps & IEEE80211_PS_MBCAST)
		wrqu->power.flags |= IW_POWER_MULTICAST_R;
	else
		wrqu->power.flags |= IW_POWER_UNICAST_R;

exit:
<<<<<<< HEAD
	up(&ieee->wx_sem);
=======
	mutex_unlock(&ieee->wx_mutex);
>>>>>>> v4.9.227
	return 0;

}
EXPORT_SYMBOL(ieee80211_wx_get_power);
