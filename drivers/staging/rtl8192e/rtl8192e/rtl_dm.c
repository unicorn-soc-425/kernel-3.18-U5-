/******************************************************************************
 * Copyright(c) 2008 - 2010 Realtek Corporation. All rights reserved.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
<<<<<<< HEAD
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
=======
>>>>>>> v4.9.227
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
******************************************************************************/
#include "rtl_core.h"
#include "rtl_dm.h"
#include "r8192E_hw.h"
#include "r8192E_phy.h"
#include "r8192E_phyreg.h"
#include "r8190P_rtl8256.h"
#include "r8192E_cmdpkt.h"

/*---------------------------Define Local Constant---------------------------*/
static u32 edca_setting_DL[HT_IOT_PEER_MAX] = {
	0x5e4322,
	0x5e4322,
	0x5ea44f,
	0x5e4322,
	0x604322,
	0xa44f,
	0x5e4322,
	0x5e4332
};

static u32 edca_setting_DL_GMode[HT_IOT_PEER_MAX] = {
	0x5e4322,
	0x5e4322,
	0x5e4322,
	0x5e4322,
	0x604322,
	0xa44f,
	0x5e4322,
	0x5e4322
};

static u32 edca_setting_UL[HT_IOT_PEER_MAX] = {
	0x5e4322,
	0xa44f,
	0x5ea44f,
	0x5e4322,
	0x604322,
	0x5e4322,
	0x5e4322,
	0x5e4332
};

<<<<<<< HEAD
#define RTK_UL_EDCA 0xa44f
#define RTK_DL_EDCA 0x5e4322
=======
const u32 dm_tx_bb_gain[TxBBGainTableLength] = {
	0x7f8001fe, /* 12 dB */
	0x788001e2, /* 11 dB */
	0x71c001c7,
	0x6b8001ae,
	0x65400195,
	0x5fc0017f,
	0x5a400169,
	0x55400155,
	0x50800142,
	0x4c000130,
	0x47c0011f,
	0x43c0010f,
	0x40000100,
	0x3c8000f2,
	0x390000e4,
	0x35c000d7,
	0x32c000cb,
	0x300000c0,
	0x2d4000b5,
	0x2ac000ab,
	0x288000a2,
	0x26000098,
	0x24000090,
	0x22000088,
	0x20000080,
	0x1a00006c,
	0x1c800072,
	0x18000060,
	0x19800066,
	0x15800056,
	0x26c0005b,
	0x14400051,
	0x24400051,
	0x1300004c,
	0x12000048,
	0x11000044,
	0x10000040, /* -24 dB */
};

const u8 dm_cck_tx_bb_gain[CCKTxBBGainTableLength][8] = {
	{0x36, 0x35, 0x2e, 0x25, 0x1c, 0x12, 0x09, 0x04},
	{0x33, 0x32, 0x2b, 0x23, 0x1a, 0x11, 0x08, 0x04},
	{0x30, 0x2f, 0x29, 0x21, 0x19, 0x10, 0x08, 0x03},
	{0x2d, 0x2d, 0x27, 0x1f, 0x18, 0x0f, 0x08, 0x03},
	{0x2b, 0x2a, 0x25, 0x1e, 0x16, 0x0e, 0x07, 0x03},
	{0x28, 0x28, 0x22, 0x1c, 0x15, 0x0d, 0x07, 0x03},
	{0x26, 0x25, 0x21, 0x1b, 0x14, 0x0d, 0x06, 0x03},
	{0x24, 0x23, 0x1f, 0x19, 0x13, 0x0c, 0x06, 0x03},
	{0x22, 0x21, 0x1d, 0x18, 0x11, 0x0b, 0x06, 0x02},
	{0x20, 0x20, 0x1b, 0x16, 0x11, 0x08, 0x05, 0x02},
	{0x1f, 0x1e, 0x1a, 0x15, 0x10, 0x0a, 0x05, 0x02},
	{0x1d, 0x1c, 0x18, 0x14, 0x0f, 0x0a, 0x05, 0x02},
	{0x1b, 0x1a, 0x17, 0x13, 0x0e, 0x09, 0x04, 0x02},
	{0x1a, 0x19, 0x16, 0x12, 0x0d, 0x09, 0x04, 0x02},
	{0x18, 0x17, 0x15, 0x11, 0x0c, 0x08, 0x04, 0x02},
	{0x17, 0x16, 0x13, 0x10, 0x0c, 0x08, 0x04, 0x02},
	{0x16, 0x15, 0x12, 0x0f, 0x0b, 0x07, 0x04, 0x01},
	{0x14, 0x14, 0x11, 0x0e, 0x0b, 0x07, 0x03, 0x02},
	{0x13, 0x13, 0x10, 0x0d, 0x0a, 0x06, 0x03, 0x01},
	{0x12, 0x12, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},
	{0x11, 0x11, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},
	{0x10, 0x10, 0x0e, 0x0b, 0x08, 0x05, 0x03, 0x01},
	{0x0f, 0x0f, 0x0d, 0x0b, 0x08, 0x05, 0x03, 0x01}
};

const u8 dm_cck_tx_bb_gain_ch14[CCKTxBBGainTableLength][8] = {
	{0x36, 0x35, 0x2e, 0x1b, 0x00, 0x00, 0x00, 0x00},
	{0x33, 0x32, 0x2b, 0x19, 0x00, 0x00, 0x00, 0x00},
	{0x30, 0x2f, 0x29, 0x18, 0x00, 0x00, 0x00, 0x00},
	{0x2d, 0x2d, 0x27, 0x17, 0x00, 0x00, 0x00, 0x00},
	{0x2b, 0x2a, 0x25, 0x15, 0x00, 0x00, 0x00, 0x00},
	{0x28, 0x28, 0x22, 0x14, 0x00, 0x00, 0x00, 0x00},
	{0x26, 0x25, 0x21, 0x13, 0x00, 0x00, 0x00, 0x00},
	{0x24, 0x23, 0x1f, 0x12, 0x00, 0x00, 0x00, 0x00},
	{0x22, 0x21, 0x1d, 0x11, 0x00, 0x00, 0x00, 0x00},
	{0x20, 0x20, 0x1b, 0x10, 0x00, 0x00, 0x00, 0x00},
	{0x1f, 0x1e, 0x1a, 0x0f, 0x00, 0x00, 0x00, 0x00},
	{0x1d, 0x1c, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00},
	{0x1b, 0x1a, 0x17, 0x0e, 0x00, 0x00, 0x00, 0x00},
	{0x1a, 0x19, 0x16, 0x0d, 0x00, 0x00, 0x00, 0x00},
	{0x18, 0x17, 0x15, 0x0c, 0x00, 0x00, 0x00, 0x00},
	{0x17, 0x16, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00},
	{0x16, 0x15, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00},
	{0x14, 0x14, 0x11, 0x0a, 0x00, 0x00, 0x00, 0x00},
	{0x13, 0x13, 0x10, 0x0a, 0x00, 0x00, 0x00, 0x00},
	{0x12, 0x12, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},
	{0x11, 0x11, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},
	{0x10, 0x10, 0x0e, 0x08, 0x00, 0x00, 0x00, 0x00},
	{0x0f, 0x0f, 0x0d, 0x08, 0x00, 0x00, 0x00, 0x00}
};

>>>>>>> v4.9.227
/*---------------------------Define Local Constant---------------------------*/


/*------------------------Define global variable-----------------------------*/
struct dig_t dm_digtable;
<<<<<<< HEAD
u8 dm_shadow[16][256] = {
	{0}
};
=======
>>>>>>> v4.9.227

struct drx_path_sel DM_RxPathSelTable;
/*------------------------Define global variable-----------------------------*/


/*------------------------Define local variable------------------------------*/
/*------------------------Define local variable------------------------------*/



/*---------------------Define local function prototype-----------------------*/
<<<<<<< HEAD
static	void	dm_check_rate_adaptive(struct net_device *dev);

static	void	dm_init_bandwidth_autoswitch(struct net_device *dev);
static	void	dm_bandwidth_autoswitch(struct net_device *dev);


static	void	dm_check_txpower_tracking(struct net_device *dev);





static	void	dm_bb_initialgain_restore(struct net_device *dev);


static	void	dm_bb_initialgain_backup(struct net_device *dev);

static	void dm_dig_init(struct net_device *dev);
static	void dm_ctrl_initgain_byrssi(struct net_device *dev);
static	void dm_ctrl_initgain_byrssi_highpwr(struct net_device *dev);
static	void dm_ctrl_initgain_byrssi_by_driverrssi(struct net_device *dev);
static	void dm_ctrl_initgain_byrssi_by_fwfalse_alarm(struct net_device *dev);
static	void dm_initial_gain(struct net_device *dev);
static	void dm_pd_th(struct net_device *dev);
static	void dm_cs_ratio(struct net_device *dev);

static	void dm_init_ctstoself(struct net_device *dev);
static	void dm_Init_WA_Broadcom_IOT(struct net_device *dev);

static	void	dm_check_edca_turbo(struct net_device *dev);

static	void dm_check_pbc_gpio(struct net_device *dev);


static	void dm_check_rx_path_selection(struct net_device *dev);
static	void dm_init_rxpath_selection(struct net_device *dev);
static	void dm_rxpath_sel_byrssi(struct net_device *dev);


static void dm_init_fsync(struct net_device *dev);
static void dm_deInit_fsync(struct net_device *dev);

static	void dm_check_txrateandretrycount(struct net_device *dev);
static  void dm_check_ac_dc_power(struct net_device *dev);

/*---------------------Define local function prototype-----------------------*/

static	void	dm_init_dynamic_txpower(struct net_device *dev);
static	void	dm_dynamic_txpower(struct net_device *dev);


static	void dm_send_rssi_tofw(struct net_device *dev);
static	void	dm_ctstoself(struct net_device *dev);
/*---------------------------Define function prototype------------------------*/

void init_hal_dm(struct net_device *dev)
=======
static void _rtl92e_dm_check_rate_adaptive(struct net_device *dev);

static void _rtl92e_dm_init_bandwidth_autoswitch(struct net_device *dev);
static	void	_rtl92e_dm_bandwidth_autoswitch(struct net_device *dev);


static	void	_rtl92e_dm_check_tx_power_tracking(struct net_device *dev);

static void _rtl92e_dm_bb_initialgain_restore(struct net_device *dev);
static void _rtl92e_dm_dig_init(struct net_device *dev);
static void _rtl92e_dm_ctrl_initgain_byrssi(struct net_device *dev);
static void _rtl92e_dm_ctrl_initgain_byrssi_highpwr(struct net_device *dev);
static void _rtl92e_dm_ctrl_initgain_byrssi_driver(struct net_device *dev);
static void _rtl92e_dm_ctrl_initgain_byrssi_false_alarm(struct net_device *dev);
static void _rtl92e_dm_initial_gain(struct net_device *dev);
static void _rtl92e_dm_pd_th(struct net_device *dev);
static void _rtl92e_dm_cs_ratio(struct net_device *dev);

static	void _rtl92e_dm_init_cts_to_self(struct net_device *dev);
static void _rtl92e_dm_init_wa_broadcom_iot(struct net_device *dev);

static void _rtl92e_dm_check_edca_turbo(struct net_device *dev);
static void _rtl92e_dm_check_rx_path_selection(struct net_device *dev);
static void _rtl92e_dm_init_rx_path_selection(struct net_device *dev);
static void _rtl92e_dm_rx_path_sel_byrssi(struct net_device *dev);


static void _rtl92e_dm_init_fsync(struct net_device *dev);
static void _rtl92e_dm_deinit_fsync(struct net_device *dev);

static	void _rtl92e_dm_check_txrateandretrycount(struct net_device *dev);
static  void _rtl92e_dm_check_ac_dc_power(struct net_device *dev);
static void _rtl92e_dm_check_fsync(struct net_device *dev);
static void _rtl92e_dm_check_rf_ctrl_gpio(void *data);
static void _rtl92e_dm_fsync_timer_callback(unsigned long data);

/*---------------------Define local function prototype-----------------------*/

static	void	_rtl92e_dm_init_dynamic_tx_power(struct net_device *dev);
static void _rtl92e_dm_dynamic_tx_power(struct net_device *dev);

static void _rtl92e_dm_send_rssi_to_fw(struct net_device *dev);
static void _rtl92e_dm_cts_to_self(struct net_device *dev);
/*---------------------------Define function prototype------------------------*/

void rtl92e_dm_init(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	priv->DM_Type = DM_Type_ByDriver;

	priv->undecorated_smoothed_pwdb = -1;

<<<<<<< HEAD
	dm_init_dynamic_txpower(dev);

	init_rate_adaptive(dev);

	dm_dig_init(dev);
	dm_init_edca_turbo(dev);
	dm_init_bandwidth_autoswitch(dev);
	dm_init_fsync(dev);
	dm_init_rxpath_selection(dev);
	dm_init_ctstoself(dev);
	if (IS_HARDWARE_TYPE_8192SE(dev))
		dm_Init_WA_Broadcom_IOT(dev);

	INIT_DELAYED_WORK_RSL(&priv->gpio_change_rf_wq, (void *)dm_CheckRfCtrlGPIO, dev);
}

void deinit_hal_dm(struct net_device *dev)
{

	dm_deInit_fsync(dev);

}

void hal_dm_watchdog(struct net_device *dev)
=======
	_rtl92e_dm_init_dynamic_tx_power(dev);

	rtl92e_init_adaptive_rate(dev);

	_rtl92e_dm_dig_init(dev);
	rtl92e_dm_init_edca_turbo(dev);
	_rtl92e_dm_init_bandwidth_autoswitch(dev);
	_rtl92e_dm_init_fsync(dev);
	_rtl92e_dm_init_rx_path_selection(dev);
	_rtl92e_dm_init_cts_to_self(dev);
	if (IS_HARDWARE_TYPE_8192SE(dev))
		_rtl92e_dm_init_wa_broadcom_iot(dev);

	INIT_DELAYED_WORK_RSL(&priv->gpio_change_rf_wq,
			      (void *)_rtl92e_dm_check_rf_ctrl_gpio, dev);
}

void rtl92e_dm_deinit(struct net_device *dev)
{

	_rtl92e_dm_deinit_fsync(dev);

}

void rtl92e_dm_watchdog(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->being_init_adapter)
		return;

<<<<<<< HEAD
	dm_check_ac_dc_power(dev);

	dm_check_pbc_gpio(dev);
	dm_check_txrateandretrycount(dev);
	dm_check_edca_turbo(dev);

	dm_check_rate_adaptive(dev);
	dm_dynamic_txpower(dev);
	dm_check_txpower_tracking(dev);

	dm_ctrl_initgain_byrssi(dev);
	dm_bandwidth_autoswitch(dev);

	dm_check_rx_path_selection(dev);
	dm_check_fsync(dev);

	dm_send_rssi_tofw(dev);
	dm_ctstoself(dev);
}

static void dm_check_ac_dc_power(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static char *ac_dc_check_script_path = "/etc/acpi/wireless-rtl-ac-dc-power.sh";
	char *argv[] = {ac_dc_check_script_path, DRV_NAME, NULL};
=======
	_rtl92e_dm_check_ac_dc_power(dev);

	_rtl92e_dm_check_txrateandretrycount(dev);
	_rtl92e_dm_check_edca_turbo(dev);

	_rtl92e_dm_check_rate_adaptive(dev);
	_rtl92e_dm_dynamic_tx_power(dev);
	_rtl92e_dm_check_tx_power_tracking(dev);

	_rtl92e_dm_ctrl_initgain_byrssi(dev);
	_rtl92e_dm_bandwidth_autoswitch(dev);

	_rtl92e_dm_check_rx_path_selection(dev);
	_rtl92e_dm_check_fsync(dev);

	_rtl92e_dm_send_rssi_to_fw(dev);
	_rtl92e_dm_cts_to_self(dev);
}

static void _rtl92e_dm_check_ac_dc_power(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static char *ac_dc_script = "/etc/acpi/wireless-rtl-ac-dc-power.sh";
	char *argv[] = {ac_dc_script, DRV_NAME, NULL};
>>>>>>> v4.9.227
	static char *envp[] = {"HOME=/",
			"TERM=linux",
			"PATH=/usr/bin:/bin",
			 NULL};

	if (priv->ResetProgress == RESET_TYPE_SILENT) {
		RT_TRACE((COMP_INIT | COMP_POWER | COMP_RF),
			 "GPIOChangeRFWorkItemCallBack(): Silent Reset!!!!!!!\n");
		return;
	}

	if (priv->rtllib->state != RTLLIB_LINKED)
		return;
<<<<<<< HEAD
	call_usermodehelper(ac_dc_check_script_path, argv, envp, UMH_WAIT_PROC);
=======
	call_usermodehelper(ac_dc_script, argv, envp, UMH_WAIT_PROC);
>>>>>>> v4.9.227

	return;
};


<<<<<<< HEAD
void init_rate_adaptive(struct net_device *dev)
{

	struct r8192_priv *priv = rtllib_priv(dev);
	struct rate_adaptive *pra = (struct rate_adaptive *)&priv->rate_adaptive;
=======
void rtl92e_init_adaptive_rate(struct net_device *dev)
{

	struct r8192_priv *priv = rtllib_priv(dev);
	struct rate_adaptive *pra = &priv->rate_adaptive;
>>>>>>> v4.9.227

	pra->ratr_state = DM_RATR_STA_MAX;
	pra->high2low_rssi_thresh_for_ra = RateAdaptiveTH_High;
	pra->low2high_rssi_thresh_for_ra20M = RateAdaptiveTH_Low_20M+5;
	pra->low2high_rssi_thresh_for_ra40M = RateAdaptiveTH_Low_40M+5;

	pra->high_rssi_thresh_for_ra = RateAdaptiveTH_High+5;
	pra->low_rssi_thresh_for_ra20M = RateAdaptiveTH_Low_20M;
	pra->low_rssi_thresh_for_ra40M = RateAdaptiveTH_Low_40M;

	if (priv->CustomerID == RT_CID_819x_Netcore)
		pra->ping_rssi_enable = 1;
	else
		pra->ping_rssi_enable = 0;
	pra->ping_rssi_thresh_for_ra = 15;


	if (priv->rf_type == RF_2T4R) {
		pra->upper_rssi_threshold_ratr		=	0x8f0f0000;
		pra->middle_rssi_threshold_ratr		=	0x8f0ff000;
		pra->low_rssi_threshold_ratr		=	0x8f0ff001;
		pra->low_rssi_threshold_ratr_40M	=	0x8f0ff005;
		pra->low_rssi_threshold_ratr_20M	=	0x8f0ff001;
		pra->ping_rssi_ratr	=	0x0000000d;
	} else if (priv->rf_type == RF_1T2R) {
		pra->upper_rssi_threshold_ratr		=	0x000fc000;
		pra->middle_rssi_threshold_ratr		=	0x000ff000;
		pra->low_rssi_threshold_ratr		=	0x000ff001;
		pra->low_rssi_threshold_ratr_40M	=	0x000ff005;
		pra->low_rssi_threshold_ratr_20M	=	0x000ff001;
		pra->ping_rssi_ratr	=	0x0000000d;
	}

}


<<<<<<< HEAD
static void dm_check_rate_adaptive(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rt_hi_throughput *pHTInfo = priv->rtllib->pHTInfo;
	struct rate_adaptive *pra = (struct rate_adaptive *)&priv->rate_adaptive;
=======
static void _rtl92e_dm_check_rate_adaptive(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rt_hi_throughput *pHTInfo = priv->rtllib->pHTInfo;
	struct rate_adaptive *pra = &priv->rate_adaptive;
>>>>>>> v4.9.227
	u32 currentRATR, targetRATR = 0;
	u32 LowRSSIThreshForRA = 0, HighRSSIThreshForRA = 0;
	bool bshort_gi_enabled = false;
	static u8 ping_rssi_state;

<<<<<<< HEAD
	if (IS_NIC_DOWN(priv)) {
		RT_TRACE(COMP_RATE, "<---- dm_check_rate_adaptive(): driver is going to unload\n");
=======
	if (!priv->up) {
		RT_TRACE(COMP_RATE,
			 "<---- _rtl92e_dm_check_rate_adaptive(): driver is going to unload\n");
>>>>>>> v4.9.227
		return;
	}

	if (pra->rate_adaptive_disabled)
		return;

	if (!(priv->rtllib->mode == WIRELESS_MODE_N_24G ||
	    priv->rtllib->mode == WIRELESS_MODE_N_5G))
		return;

	if (priv->rtllib->state == RTLLIB_LINKED) {

<<<<<<< HEAD
		bshort_gi_enabled = (pHTInfo->bCurTxBW40MHz && pHTInfo->bCurShortGI40MHz) ||
			(!pHTInfo->bCurTxBW40MHz && pHTInfo->bCurShortGI20MHz);


		pra->upper_rssi_threshold_ratr =
				(pra->upper_rssi_threshold_ratr & (~BIT31)) | ((bshort_gi_enabled) ? BIT31 : 0);

		pra->middle_rssi_threshold_ratr =
				(pra->middle_rssi_threshold_ratr & (~BIT31)) | ((bshort_gi_enabled) ? BIT31 : 0);

		if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20) {
			pra->low_rssi_threshold_ratr =
				(pra->low_rssi_threshold_ratr_40M & (~BIT31)) | ((bshort_gi_enabled) ? BIT31 : 0);
		} else {
			pra->low_rssi_threshold_ratr =
			(pra->low_rssi_threshold_ratr_20M & (~BIT31)) | ((bshort_gi_enabled) ? BIT31 : 0);
		}
		pra->ping_rssi_ratr =
				(pra->ping_rssi_ratr & (~BIT31)) | ((bshort_gi_enabled) ? BIT31 : 0);

		if (pra->ratr_state == DM_RATR_STA_HIGH) {
			HighRSSIThreshForRA	= pra->high2low_rssi_thresh_for_ra;
			LowRSSIThreshForRA	= (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20) ?
					(pra->low_rssi_thresh_for_ra40M) : (pra->low_rssi_thresh_for_ra20M);
		} else if (pra->ratr_state == DM_RATR_STA_LOW) {
			HighRSSIThreshForRA	= pra->high_rssi_thresh_for_ra;
			LowRSSIThreshForRA	= (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20) ?
					(pra->low2high_rssi_thresh_for_ra40M) : (pra->low2high_rssi_thresh_for_ra20M);
		} else {
			HighRSSIThreshForRA	= pra->high_rssi_thresh_for_ra;
			LowRSSIThreshForRA	= (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20) ?
					(pra->low_rssi_thresh_for_ra40M) : (pra->low_rssi_thresh_for_ra20M);
		}

		if (priv->undecorated_smoothed_pwdb >= (long)HighRSSIThreshForRA) {
			pra->ratr_state = DM_RATR_STA_HIGH;
			targetRATR = pra->upper_rssi_threshold_ratr;
		} else if (priv->undecorated_smoothed_pwdb >= (long)LowRSSIThreshForRA) {
=======
		bshort_gi_enabled = (pHTInfo->bCurTxBW40MHz &&
				     pHTInfo->bCurShortGI40MHz) ||
				    (!pHTInfo->bCurTxBW40MHz &&
				     pHTInfo->bCurShortGI20MHz);

		pra->upper_rssi_threshold_ratr =
				(pra->upper_rssi_threshold_ratr & (~BIT31)) |
				((bshort_gi_enabled) ? BIT31 : 0);

		pra->middle_rssi_threshold_ratr =
				(pra->middle_rssi_threshold_ratr & (~BIT31)) |
				((bshort_gi_enabled) ? BIT31 : 0);

		if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20) {
			pra->low_rssi_threshold_ratr =
				(pra->low_rssi_threshold_ratr_40M & (~BIT31)) |
				((bshort_gi_enabled) ? BIT31 : 0);
		} else {
			pra->low_rssi_threshold_ratr =
				(pra->low_rssi_threshold_ratr_20M & (~BIT31)) |
				((bshort_gi_enabled) ? BIT31 : 0);
		}
		pra->ping_rssi_ratr =
				(pra->ping_rssi_ratr & (~BIT31)) |
				((bshort_gi_enabled) ? BIT31 : 0);

		if (pra->ratr_state == DM_RATR_STA_HIGH) {
			HighRSSIThreshForRA = pra->high2low_rssi_thresh_for_ra;
			LowRSSIThreshForRA = (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20) ?
					(pra->low_rssi_thresh_for_ra40M) : (pra->low_rssi_thresh_for_ra20M);
		} else if (pra->ratr_state == DM_RATR_STA_LOW) {
			HighRSSIThreshForRA = pra->high_rssi_thresh_for_ra;
			LowRSSIThreshForRA = (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20) ?
					(pra->low2high_rssi_thresh_for_ra40M) : (pra->low2high_rssi_thresh_for_ra20M);
		} else {
			HighRSSIThreshForRA = pra->high_rssi_thresh_for_ra;
			LowRSSIThreshForRA = (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20) ?
					(pra->low_rssi_thresh_for_ra40M) : (pra->low_rssi_thresh_for_ra20M);
		}

		if (priv->undecorated_smoothed_pwdb >=
		    (long)HighRSSIThreshForRA) {
			pra->ratr_state = DM_RATR_STA_HIGH;
			targetRATR = pra->upper_rssi_threshold_ratr;
		} else if (priv->undecorated_smoothed_pwdb >=
			   (long)LowRSSIThreshForRA) {
>>>>>>> v4.9.227
			pra->ratr_state = DM_RATR_STA_MIDDLE;
			targetRATR = pra->middle_rssi_threshold_ratr;
		} else {
			pra->ratr_state = DM_RATR_STA_LOW;
			targetRATR = pra->low_rssi_threshold_ratr;
		}

		if (pra->ping_rssi_enable) {
<<<<<<< HEAD
			if (priv->undecorated_smoothed_pwdb < (long)(pra->ping_rssi_thresh_for_ra+5)) {
				if ((priv->undecorated_smoothed_pwdb < (long)pra->ping_rssi_thresh_for_ra) ||
=======
			if (priv->undecorated_smoothed_pwdb <
			    (long)(pra->ping_rssi_thresh_for_ra+5)) {
				if ((priv->undecorated_smoothed_pwdb <
				     (long)pra->ping_rssi_thresh_for_ra) ||
>>>>>>> v4.9.227
				    ping_rssi_state) {
					pra->ratr_state = DM_RATR_STA_LOW;
					targetRATR = pra->ping_rssi_ratr;
					ping_rssi_state = 1;
				}
			} else {
				ping_rssi_state = 0;
			}
		}

		if (priv->rtllib->GetHalfNmodeSupportByAPsHandler(dev))
			targetRATR &=  0xf00fffff;

<<<<<<< HEAD
		currentRATR = read_nic_dword(dev, RATR0);
=======
		currentRATR = rtl92e_readl(dev, RATR0);
>>>>>>> v4.9.227
		if (targetRATR !=  currentRATR) {
			u32 ratr_value;

			ratr_value = targetRATR;
			RT_TRACE(COMP_RATE,
				 "currentRATR = %x, targetRATR = %x\n",
				 currentRATR, targetRATR);
			if (priv->rf_type == RF_1T2R)
				ratr_value &= ~(RATE_ALL_OFDM_2SS);
<<<<<<< HEAD
			write_nic_dword(dev, RATR0, ratr_value);
			write_nic_byte(dev, UFWP, 1);
=======
			rtl92e_writel(dev, RATR0, ratr_value);
			rtl92e_writeb(dev, UFWP, 1);
>>>>>>> v4.9.227

			pra->last_ratr = targetRATR;
		}

	} else {
		pra->ratr_state = DM_RATR_STA_MAX;
	}
}

<<<<<<< HEAD
static void dm_init_bandwidth_autoswitch(struct net_device *dev)
=======
static void _rtl92e_dm_init_bandwidth_autoswitch(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	priv->rtllib->bandwidth_auto_switch.threshold_20Mhzto40Mhz = BW_AUTO_SWITCH_LOW_HIGH;
	priv->rtllib->bandwidth_auto_switch.threshold_40Mhzto20Mhz = BW_AUTO_SWITCH_HIGH_LOW;
	priv->rtllib->bandwidth_auto_switch.bforced_tx20Mhz = false;
	priv->rtllib->bandwidth_auto_switch.bautoswitch_enable = false;
}

<<<<<<< HEAD
static void dm_bandwidth_autoswitch(struct net_device *dev)
=======
static void _rtl92e_dm_bandwidth_autoswitch(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->CurrentChannelBW == HT_CHANNEL_WIDTH_20 ||
<<<<<<< HEAD
	   !priv->rtllib->bandwidth_auto_switch.bautoswitch_enable) {
		return;
	} else {
		if (priv->rtllib->bandwidth_auto_switch.bforced_tx20Mhz == false) {
			if (priv->undecorated_smoothed_pwdb <=
			    priv->rtllib->bandwidth_auto_switch.threshold_40Mhzto20Mhz)
				priv->rtllib->bandwidth_auto_switch.bforced_tx20Mhz = true;
		} else {
			if (priv->undecorated_smoothed_pwdb >=
			    priv->rtllib->bandwidth_auto_switch.threshold_20Mhzto40Mhz)
				priv->rtllib->bandwidth_auto_switch.bforced_tx20Mhz = false;

		}
=======
	   !priv->rtllib->bandwidth_auto_switch.bautoswitch_enable)
		return;
	if (priv->rtllib->bandwidth_auto_switch.bforced_tx20Mhz == false) {
		if (priv->undecorated_smoothed_pwdb <=
		    priv->rtllib->bandwidth_auto_switch.threshold_40Mhzto20Mhz)
			priv->rtllib->bandwidth_auto_switch.bforced_tx20Mhz = true;
	} else {
		if (priv->undecorated_smoothed_pwdb >=
		    priv->rtllib->bandwidth_auto_switch.threshold_20Mhzto40Mhz)
			priv->rtllib->bandwidth_auto_switch.bforced_tx20Mhz = false;
>>>>>>> v4.9.227
	}
}

static u32 OFDMSwingTable[OFDM_Table_Length] = {
	0x7f8001fe,
	0x71c001c7,
	0x65400195,
	0x5a400169,
	0x50800142,
	0x47c0011f,
	0x40000100,
	0x390000e4,
	0x32c000cb,
	0x2d4000b5,
	0x288000a2,
	0x24000090,
	0x20000080,
	0x1c800072,
	0x19800066,
	0x26c0005b,
	0x24400051,
	0x12000048,
	0x10000040
};

static u8	CCKSwingTable_Ch1_Ch13[CCK_Table_length][8] = {
	{0x36, 0x35, 0x2e, 0x25, 0x1c, 0x12, 0x09, 0x04},
	{0x30, 0x2f, 0x29, 0x21, 0x19, 0x10, 0x08, 0x03},
	{0x2b, 0x2a, 0x25, 0x1e, 0x16, 0x0e, 0x07, 0x03},
	{0x26, 0x25, 0x21, 0x1b, 0x14, 0x0d, 0x06, 0x03},
	{0x22, 0x21, 0x1d, 0x18, 0x11, 0x0b, 0x06, 0x02},
	{0x1f, 0x1e, 0x1a, 0x15, 0x10, 0x0a, 0x05, 0x02},
	{0x1b, 0x1a, 0x17, 0x13, 0x0e, 0x09, 0x04, 0x02},
	{0x18, 0x17, 0x15, 0x11, 0x0c, 0x08, 0x04, 0x02},
	{0x16, 0x15, 0x12, 0x0f, 0x0b, 0x07, 0x04, 0x01},
	{0x13, 0x13, 0x10, 0x0d, 0x0a, 0x06, 0x03, 0x01},
	{0x11, 0x11, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},
	{0x0f, 0x0f, 0x0d, 0x0b, 0x08, 0x05, 0x03, 0x01}
};

static u8	CCKSwingTable_Ch14[CCK_Table_length][8] = {
	{0x36, 0x35, 0x2e, 0x1b, 0x00, 0x00, 0x00, 0x00},
	{0x30, 0x2f, 0x29, 0x18, 0x00, 0x00, 0x00, 0x00},
	{0x2b, 0x2a, 0x25, 0x15, 0x00, 0x00, 0x00, 0x00},
	{0x26, 0x25, 0x21, 0x13, 0x00, 0x00, 0x00, 0x00},
	{0x22, 0x21, 0x1d, 0x11, 0x00, 0x00, 0x00, 0x00},
	{0x1f, 0x1e, 0x1a, 0x0f, 0x00, 0x00, 0x00, 0x00},
	{0x1b, 0x1a, 0x17, 0x0e, 0x00, 0x00, 0x00, 0x00},
	{0x18, 0x17, 0x15, 0x0c, 0x00, 0x00, 0x00, 0x00},
	{0x16, 0x15, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00},
	{0x13, 0x13, 0x10, 0x0a, 0x00, 0x00, 0x00, 0x00},
	{0x11, 0x11, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},
	{0x0f, 0x0f, 0x0d, 0x08, 0x00, 0x00, 0x00, 0x00}
};

#define		Pw_Track_Flag				0x11d
#define		Tssi_Mea_Value				0x13c
#define		Tssi_Report_Value1			0x134
#define		Tssi_Report_Value2			0x13e
#define		FW_Busy_Flag				0x13f

<<<<<<< HEAD
static void dm_TXPowerTrackingCallback_TSSI(struct net_device *dev)
=======
static void _rtl92e_dm_tx_update_tssi_weak_signal(struct net_device *dev,
						  u8 RF_Type)
{
	struct r8192_priv *p = rtllib_priv(dev);

	if (RF_Type == RF_2T4R) {
		if ((p->rfa_txpowertrackingindex > 0) &&
		    (p->rfc_txpowertrackingindex > 0)) {
			p->rfa_txpowertrackingindex--;
			if (p->rfa_txpowertrackingindex_real > 4) {
				p->rfa_txpowertrackingindex_real--;
				rtl92e_set_bb_reg(dev, rOFDM0_XATxIQImbalance,
						  bMaskDWord,
						  dm_tx_bb_gain[p->rfa_txpowertrackingindex_real]);
			}

			p->rfc_txpowertrackingindex--;
			if (p->rfc_txpowertrackingindex_real > 4) {
				p->rfc_txpowertrackingindex_real--;
				rtl92e_set_bb_reg(dev,
						  rOFDM0_XCTxIQImbalance,
						  bMaskDWord,
						  dm_tx_bb_gain[p->rfc_txpowertrackingindex_real]);
			}
		} else {
			rtl92e_set_bb_reg(dev, rOFDM0_XATxIQImbalance,
					  bMaskDWord,
					  dm_tx_bb_gain[4]);
			rtl92e_set_bb_reg(dev,
					  rOFDM0_XCTxIQImbalance,
					  bMaskDWord, dm_tx_bb_gain[4]);
		}
	} else {
		if (p->rfa_txpowertrackingindex > 0) {
			p->rfa_txpowertrackingindex--;
			if (p->rfa_txpowertrackingindex_real > 4) {
				p->rfa_txpowertrackingindex_real--;
				rtl92e_set_bb_reg(dev,
						  rOFDM0_XATxIQImbalance,
						  bMaskDWord,
						  dm_tx_bb_gain[p->rfa_txpowertrackingindex_real]);
			}
		} else {
			rtl92e_set_bb_reg(dev, rOFDM0_XATxIQImbalance,
					  bMaskDWord, dm_tx_bb_gain[4]);
		}
	}
}

static void _rtl92e_dm_tx_update_tssi_strong_signal(struct net_device *dev,
						    u8 RF_Type)
{
	struct r8192_priv *p = rtllib_priv(dev);

	if (RF_Type == RF_2T4R) {
		if ((p->rfa_txpowertrackingindex < TxBBGainTableLength - 1) &&
		    (p->rfc_txpowertrackingindex < TxBBGainTableLength - 1)) {
			p->rfa_txpowertrackingindex++;
			p->rfa_txpowertrackingindex_real++;
			rtl92e_set_bb_reg(dev, rOFDM0_XATxIQImbalance,
					  bMaskDWord,
					  dm_tx_bb_gain[p->rfa_txpowertrackingindex_real]);
			p->rfc_txpowertrackingindex++;
			p->rfc_txpowertrackingindex_real++;
			rtl92e_set_bb_reg(dev, rOFDM0_XCTxIQImbalance,
					  bMaskDWord,
					  dm_tx_bb_gain[p->rfc_txpowertrackingindex_real]);
		} else {
			rtl92e_set_bb_reg(dev, rOFDM0_XATxIQImbalance,
					  bMaskDWord,
					  dm_tx_bb_gain[TxBBGainTableLength - 1]);
			rtl92e_set_bb_reg(dev, rOFDM0_XCTxIQImbalance,
					  bMaskDWord,
					  dm_tx_bb_gain[TxBBGainTableLength - 1]);
		}
	} else {
		if (p->rfa_txpowertrackingindex < (TxBBGainTableLength - 1)) {
			p->rfa_txpowertrackingindex++;
			p->rfa_txpowertrackingindex_real++;
			rtl92e_set_bb_reg(dev, rOFDM0_XATxIQImbalance,
					  bMaskDWord,
					  dm_tx_bb_gain[p->rfa_txpowertrackingindex_real]);
		} else {
			rtl92e_set_bb_reg(dev, rOFDM0_XATxIQImbalance,
					  bMaskDWord,
					  dm_tx_bb_gain[TxBBGainTableLength - 1]);
		}
	}
}

static void _rtl92e_dm_tx_power_tracking_callback_tssi(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	bool	bHighpowerstate, viviflag = false;
	struct dcmd_txcmd tx_cmd;
	u8	powerlevelOFDM24G;
	int	i = 0, j = 0, k = 0;
	u8	RF_Type, tmp_report[5] = {0, 0, 0, 0, 0};
	u32	Value;
	u8	Pwr_Flag;
	u16	Avg_TSSI_Meas, TSSI_13dBm, Avg_TSSI_Meas_from_driver = 0;
	u32	delta = 0;

	RT_TRACE(COMP_POWER_TRACKING, "%s()\n", __func__);
<<<<<<< HEAD
	write_nic_byte(dev, Pw_Track_Flag, 0);
	write_nic_byte(dev, FW_Busy_Flag, 0);
=======
	rtl92e_writeb(dev, Pw_Track_Flag, 0);
	rtl92e_writeb(dev, FW_Busy_Flag, 0);
>>>>>>> v4.9.227
	priv->rtllib->bdynamic_txpower_enable = false;
	bHighpowerstate = priv->bDynamicTxHighPower;

	powerlevelOFDM24G = (u8)(priv->Pwr_Track>>24);
	RF_Type = priv->rf_type;
	Value = (RF_Type<<8) | powerlevelOFDM24G;

	RT_TRACE(COMP_POWER_TRACKING, "powerlevelOFDM24G = %x\n",
		 powerlevelOFDM24G);


	for (j = 0; j <= 30; j++) {

		tx_cmd.Op		= TXCMD_SET_TX_PWR_TRACKING;
		tx_cmd.Length	= 4;
		tx_cmd.Value		= Value;
<<<<<<< HEAD
		cmpk_message_handle_tx(dev, (u8 *)&tx_cmd,
				       DESC_PACKET_TYPE_INIT,
				       sizeof(struct dcmd_txcmd));
		mdelay(1);
		for (i = 0; i <= 30; i++) {
			Pwr_Flag = read_nic_byte(dev, Pw_Track_Flag);
=======
		rtl92e_send_cmd_pkt(dev, DESC_PACKET_TYPE_NORMAL, (u8 *)&tx_cmd,
				    sizeof(struct dcmd_txcmd));
		mdelay(1);
		for (i = 0; i <= 30; i++) {
			Pwr_Flag = rtl92e_readb(dev, Pw_Track_Flag);
>>>>>>> v4.9.227

			if (Pwr_Flag == 0) {
				mdelay(1);

				if (priv->bResetInProgress) {
					RT_TRACE(COMP_POWER_TRACKING,
						 "we are in silent reset progress, so return\n");
<<<<<<< HEAD
					write_nic_byte(dev, Pw_Track_Flag, 0);
					write_nic_byte(dev, FW_Busy_Flag, 0);
					return;
				}
				if ((priv->rtllib->eRFPowerState != eRfOn)) {
					RT_TRACE(COMP_POWER_TRACKING,
						 "we are in power save, so return\n");
					write_nic_byte(dev, Pw_Track_Flag, 0);
					write_nic_byte(dev, FW_Busy_Flag, 0);
=======
					rtl92e_writeb(dev, Pw_Track_Flag, 0);
					rtl92e_writeb(dev, FW_Busy_Flag, 0);
					return;
				}
				if (priv->rtllib->eRFPowerState != eRfOn) {
					RT_TRACE(COMP_POWER_TRACKING,
						 "we are in power save, so return\n");
					rtl92e_writeb(dev, Pw_Track_Flag, 0);
					rtl92e_writeb(dev, FW_Busy_Flag, 0);
>>>>>>> v4.9.227
					return;
				}

				continue;
			}

<<<<<<< HEAD
			Avg_TSSI_Meas = read_nic_word(dev, Tssi_Mea_Value);

			if (Avg_TSSI_Meas == 0) {
				write_nic_byte(dev, Pw_Track_Flag, 0);
				write_nic_byte(dev, FW_Busy_Flag, 0);
=======
			Avg_TSSI_Meas = rtl92e_readw(dev, Tssi_Mea_Value);

			if (Avg_TSSI_Meas == 0) {
				rtl92e_writeb(dev, Pw_Track_Flag, 0);
				rtl92e_writeb(dev, FW_Busy_Flag, 0);
>>>>>>> v4.9.227
				return;
			}

			for (k = 0; k < 5; k++) {
				if (k != 4)
<<<<<<< HEAD
					tmp_report[k] = read_nic_byte(dev,
							 Tssi_Report_Value1+k);
				else
					tmp_report[k] = read_nic_byte(dev,
=======
					tmp_report[k] = rtl92e_readb(dev,
							 Tssi_Report_Value1+k);
				else
					tmp_report[k] = rtl92e_readb(dev,
>>>>>>> v4.9.227
							 Tssi_Report_Value2);

				RT_TRACE(COMP_POWER_TRACKING,
					 "TSSI_report_value = %d\n",
					 tmp_report[k]);

<<<<<<< HEAD
			       if (tmp_report[k] <= 20) {
=======
				if (tmp_report[k] <= 20) {
>>>>>>> v4.9.227
					viviflag = true;
					break;
				}
			}

			if (viviflag) {
<<<<<<< HEAD
				write_nic_byte(dev, Pw_Track_Flag, 0);
				viviflag = false;
				RT_TRACE(COMP_POWER_TRACKING, "we filted this data\n");
=======
				rtl92e_writeb(dev, Pw_Track_Flag, 0);
				viviflag = false;
				RT_TRACE(COMP_POWER_TRACKING,
					 "we filted this data\n");
>>>>>>> v4.9.227
				for (k = 0; k < 5; k++)
					tmp_report[k] = 0;
				break;
			}

			for (k = 0; k < 5; k++)
				Avg_TSSI_Meas_from_driver += tmp_report[k];

<<<<<<< HEAD
			Avg_TSSI_Meas_from_driver = Avg_TSSI_Meas_from_driver*100/5;
=======
			Avg_TSSI_Meas_from_driver *= 100 / 5;
>>>>>>> v4.9.227
			RT_TRACE(COMP_POWER_TRACKING,
				 "Avg_TSSI_Meas_from_driver = %d\n",
				 Avg_TSSI_Meas_from_driver);
			TSSI_13dBm = priv->TSSI_13dBm;
<<<<<<< HEAD
			RT_TRACE(COMP_POWER_TRACKING, "TSSI_13dBm = %d\n", TSSI_13dBm);
=======
			RT_TRACE(COMP_POWER_TRACKING, "TSSI_13dBm = %d\n",
				 TSSI_13dBm);
>>>>>>> v4.9.227

			if (Avg_TSSI_Meas_from_driver > TSSI_13dBm)
				delta = Avg_TSSI_Meas_from_driver - TSSI_13dBm;
			else
				delta = TSSI_13dBm - Avg_TSSI_Meas_from_driver;

			if (delta <= E_FOR_TX_POWER_TRACK) {
				priv->rtllib->bdynamic_txpower_enable = true;
<<<<<<< HEAD
				write_nic_byte(dev, Pw_Track_Flag, 0);
				write_nic_byte(dev, FW_Busy_Flag, 0);
=======
				rtl92e_writeb(dev, Pw_Track_Flag, 0);
				rtl92e_writeb(dev, FW_Busy_Flag, 0);
>>>>>>> v4.9.227
				RT_TRACE(COMP_POWER_TRACKING,
					 "tx power track is done\n");
				RT_TRACE(COMP_POWER_TRACKING,
					 "priv->rfa_txpowertrackingindex = %d\n",
					 priv->rfa_txpowertrackingindex);
				RT_TRACE(COMP_POWER_TRACKING,
					 "priv->rfa_txpowertrackingindex_real = %d\n",
					 priv->rfa_txpowertrackingindex_real);
				RT_TRACE(COMP_POWER_TRACKING,
					 "priv->CCKPresentAttentuation_difference = %d\n",
					 priv->CCKPresentAttentuation_difference);
				RT_TRACE(COMP_POWER_TRACKING,
					 "priv->CCKPresentAttentuation = %d\n",
					 priv->CCKPresentAttentuation);
				return;
<<<<<<< HEAD
			} else {
				if (Avg_TSSI_Meas_from_driver < TSSI_13dBm - E_FOR_TX_POWER_TRACK) {
					if (RF_Type == RF_2T4R) {

						if ((priv->rfa_txpowertrackingindex > 0) &&
						    (priv->rfc_txpowertrackingindex > 0)) {
							priv->rfa_txpowertrackingindex--;
							if (priv->rfa_txpowertrackingindex_real > 4) {
								priv->rfa_txpowertrackingindex_real--;
								rtl8192_setBBreg(dev,
									 rOFDM0_XATxIQImbalance,
									 bMaskDWord,
									 priv->txbbgain_table[priv->rfa_txpowertrackingindex_real].txbbgain_value);
							}

							priv->rfc_txpowertrackingindex--;
							if (priv->rfc_txpowertrackingindex_real > 4) {
								priv->rfc_txpowertrackingindex_real--;
								rtl8192_setBBreg(dev,
									 rOFDM0_XCTxIQImbalance,
									 bMaskDWord,
									 priv->txbbgain_table[priv->rfc_txpowertrackingindex_real].txbbgain_value);
							}
						} else {
							rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance,
									 bMaskDWord,
									 priv->txbbgain_table[4].txbbgain_value);
							rtl8192_setBBreg(dev,
									 rOFDM0_XCTxIQImbalance,
									 bMaskDWord, priv->txbbgain_table[4].txbbgain_value);
						}
					} else {
						if (priv->rfa_txpowertrackingindex > 0) {
							priv->rfa_txpowertrackingindex--;
							if (priv->rfa_txpowertrackingindex_real > 4) {
								priv->rfa_txpowertrackingindex_real--;
								rtl8192_setBBreg(dev,
										 rOFDM0_XATxIQImbalance,
										 bMaskDWord,
										 priv->txbbgain_table[priv->rfa_txpowertrackingindex_real].txbbgain_value);
							}
						} else
							rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance,
									 bMaskDWord, priv->txbbgain_table[4].txbbgain_value);

					}
				} else {
					if (RF_Type == RF_2T4R) {
						if ((priv->rfa_txpowertrackingindex <
						    TxBBGainTableLength - 1) &&
						    (priv->rfc_txpowertrackingindex <
						    TxBBGainTableLength - 1)) {
							priv->rfa_txpowertrackingindex++;
							priv->rfa_txpowertrackingindex_real++;
							rtl8192_setBBreg(dev,
								 rOFDM0_XATxIQImbalance,
								 bMaskDWord,
								 priv->txbbgain_table
								 [priv->rfa_txpowertrackingindex_real].txbbgain_value);
							priv->rfc_txpowertrackingindex++;
							priv->rfc_txpowertrackingindex_real++;
							rtl8192_setBBreg(dev,
								 rOFDM0_XCTxIQImbalance,
								 bMaskDWord,
								 priv->txbbgain_table[priv->rfc_txpowertrackingindex_real].txbbgain_value);
						} else {
							rtl8192_setBBreg(dev,
								 rOFDM0_XATxIQImbalance,
								 bMaskDWord,
								 priv->txbbgain_table[TxBBGainTableLength - 1].txbbgain_value);
							rtl8192_setBBreg(dev,
								 rOFDM0_XCTxIQImbalance,
								 bMaskDWord, priv->txbbgain_table[TxBBGainTableLength - 1].txbbgain_value);
						}
					} else {
						if (priv->rfa_txpowertrackingindex < (TxBBGainTableLength - 1)) {
							priv->rfa_txpowertrackingindex++;
							priv->rfa_txpowertrackingindex_real++;
							rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance,
									 bMaskDWord,
									 priv->txbbgain_table[priv->rfa_txpowertrackingindex_real].txbbgain_value);
						} else
							rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance,
									 bMaskDWord,
									 priv->txbbgain_table[TxBBGainTableLength - 1].txbbgain_value);
					}
				}
				if (RF_Type == RF_2T4R) {
					priv->CCKPresentAttentuation_difference
						= priv->rfa_txpowertrackingindex - priv->rfa_txpowertracking_default;
				} else {
					priv->CCKPresentAttentuation_difference
						= priv->rfa_txpowertrackingindex_real - priv->rfa_txpowertracking_default;
				}

				if (priv->CurrentChannelBW == HT_CHANNEL_WIDTH_20)
					priv->CCKPresentAttentuation =
						 priv->CCKPresentAttentuation_20Mdefault +
						 priv->CCKPresentAttentuation_difference;
				else
					priv->CCKPresentAttentuation =
						 priv->CCKPresentAttentuation_40Mdefault +
						 priv->CCKPresentAttentuation_difference;

				if (priv->CCKPresentAttentuation > (CCKTxBBGainTableLength-1))
					priv->CCKPresentAttentuation = CCKTxBBGainTableLength-1;
				if (priv->CCKPresentAttentuation < 0)
					priv->CCKPresentAttentuation = 0;

				if (priv->CCKPresentAttentuation > -1 &&
				    priv->CCKPresentAttentuation < CCKTxBBGainTableLength) {
					if (priv->rtllib->current_network.channel == 14 &&
					    !priv->bcck_in_ch14) {
						priv->bcck_in_ch14 = true;
						dm_cck_txpower_adjust(dev, priv->bcck_in_ch14);
					} else if (priv->rtllib->current_network.channel != 14 && priv->bcck_in_ch14) {
						priv->bcck_in_ch14 = false;
						dm_cck_txpower_adjust(dev, priv->bcck_in_ch14);
					} else
						dm_cck_txpower_adjust(dev, priv->bcck_in_ch14);
				}
				RT_TRACE(COMP_POWER_TRACKING,
					 "priv->rfa_txpowertrackingindex = %d\n",
					 priv->rfa_txpowertrackingindex);
				RT_TRACE(COMP_POWER_TRACKING,
					 "priv->rfa_txpowertrackingindex_real = %d\n",
					 priv->rfa_txpowertrackingindex_real);
				RT_TRACE(COMP_POWER_TRACKING,
					 "priv->CCKPresentAttentuation_difference = %d\n",
					 priv->CCKPresentAttentuation_difference);
				RT_TRACE(COMP_POWER_TRACKING,
					 "priv->CCKPresentAttentuation = %d\n",
					 priv->CCKPresentAttentuation);

				if (priv->CCKPresentAttentuation_difference <= -12 || priv->CCKPresentAttentuation_difference >= 24) {
					priv->rtllib->bdynamic_txpower_enable = true;
					write_nic_byte(dev, Pw_Track_Flag, 0);
					write_nic_byte(dev, FW_Busy_Flag, 0);
					RT_TRACE(COMP_POWER_TRACKING, "tx power track--->limited\n");
					return;
				}
			}
			write_nic_byte(dev, Pw_Track_Flag, 0);
=======
			}
			if (Avg_TSSI_Meas_from_driver < TSSI_13dBm - E_FOR_TX_POWER_TRACK)
				_rtl92e_dm_tx_update_tssi_weak_signal(dev,
								      RF_Type);
			else
				_rtl92e_dm_tx_update_tssi_strong_signal(dev, RF_Type);

			if (RF_Type == RF_2T4R) {
				priv->CCKPresentAttentuation_difference
					= priv->rfa_txpowertrackingindex - priv->rfa_txpowertracking_default;
			} else {
				priv->CCKPresentAttentuation_difference
					= priv->rfa_txpowertrackingindex_real - priv->rfa_txpowertracking_default;
			}

			if (priv->CurrentChannelBW == HT_CHANNEL_WIDTH_20)
				priv->CCKPresentAttentuation =
					 priv->CCKPresentAttentuation_20Mdefault +
					 priv->CCKPresentAttentuation_difference;
			else
				priv->CCKPresentAttentuation =
					 priv->CCKPresentAttentuation_40Mdefault +
					 priv->CCKPresentAttentuation_difference;

			if (priv->CCKPresentAttentuation > (CCKTxBBGainTableLength-1))
				priv->CCKPresentAttentuation = CCKTxBBGainTableLength-1;
			if (priv->CCKPresentAttentuation < 0)
				priv->CCKPresentAttentuation = 0;

			if (priv->CCKPresentAttentuation > -1 &&
			    priv->CCKPresentAttentuation < CCKTxBBGainTableLength) {
				if (priv->rtllib->current_network.channel == 14 &&
				    !priv->bcck_in_ch14) {
					priv->bcck_in_ch14 = true;
					rtl92e_dm_cck_txpower_adjust(dev, priv->bcck_in_ch14);
				} else if (priv->rtllib->current_network.channel != 14 && priv->bcck_in_ch14) {
					priv->bcck_in_ch14 = false;
					rtl92e_dm_cck_txpower_adjust(dev, priv->bcck_in_ch14);
				} else
					rtl92e_dm_cck_txpower_adjust(dev, priv->bcck_in_ch14);
			}
			RT_TRACE(COMP_POWER_TRACKING,
				 "priv->rfa_txpowertrackingindex = %d\n",
				 priv->rfa_txpowertrackingindex);
			RT_TRACE(COMP_POWER_TRACKING,
				 "priv->rfa_txpowertrackingindex_real = %d\n",
				 priv->rfa_txpowertrackingindex_real);
			RT_TRACE(COMP_POWER_TRACKING,
				 "priv->CCKPresentAttentuation_difference = %d\n",
				 priv->CCKPresentAttentuation_difference);
			RT_TRACE(COMP_POWER_TRACKING,
				 "priv->CCKPresentAttentuation = %d\n",
				 priv->CCKPresentAttentuation);

			if (priv->CCKPresentAttentuation_difference <= -12 ||
			    priv->CCKPresentAttentuation_difference >= 24) {
				priv->rtllib->bdynamic_txpower_enable = true;
				rtl92e_writeb(dev, Pw_Track_Flag, 0);
				rtl92e_writeb(dev, FW_Busy_Flag, 0);
				RT_TRACE(COMP_POWER_TRACKING,
					 "tx power track--->limited\n");
				return;
			}

			rtl92e_writeb(dev, Pw_Track_Flag, 0);
>>>>>>> v4.9.227
			Avg_TSSI_Meas_from_driver = 0;
			for (k = 0; k < 5; k++)
				tmp_report[k] = 0;
			break;
		}
<<<<<<< HEAD
		write_nic_byte(dev, FW_Busy_Flag, 0);
	}
	priv->rtllib->bdynamic_txpower_enable = true;
	write_nic_byte(dev, Pw_Track_Flag, 0);
}

static void dm_TXPowerTrackingCallback_ThermalMeter(struct net_device *dev)
=======
		rtl92e_writeb(dev, FW_Busy_Flag, 0);
	}
	priv->rtllib->bdynamic_txpower_enable = true;
	rtl92e_writeb(dev, Pw_Track_Flag, 0);
}

static void _rtl92e_dm_tx_power_tracking_cb_thermal(struct net_device *dev)
>>>>>>> v4.9.227
{
#define ThermalMeterVal	9
	struct r8192_priv *priv = rtllib_priv(dev);
	u32 tmpRegA, TempCCk;
	u8 tmpOFDMindex, tmpCCKindex, tmpCCK20Mindex, tmpCCK40Mindex, tmpval;
	int i = 0, CCKSwingNeedUpdate = 0;

	if (!priv->btxpower_trackingInit) {
<<<<<<< HEAD
		tmpRegA = rtl8192_QueryBBReg(dev, rOFDM0_XATxIQImbalance, bMaskDWord);
		for (i = 0; i < OFDM_Table_Length; i++) {
			if (tmpRegA == OFDMSwingTable[i]) {
				priv->OFDM_index[0] = (u8)i;
				RT_TRACE(COMP_POWER_TRACKING, "Initial reg0x%x = 0x%x, OFDM_index = 0x%x\n",
					rOFDM0_XATxIQImbalance, tmpRegA, priv->OFDM_index[0]);
			}
		}

		TempCCk = rtl8192_QueryBBReg(dev, rCCK0_TxFilter1, bMaskByte2);
		for (i = 0; i < CCK_Table_length; i++) {
			if (TempCCk == (u32)CCKSwingTable_Ch1_Ch13[i][0]) {
				priv->CCK_index = (u8) i;
				RT_TRACE(COMP_POWER_TRACKING, "Initial reg0x%x"
					 " = 0x%x, CCK_index = 0x%x\n",
=======
		tmpRegA = rtl92e_get_bb_reg(dev, rOFDM0_XATxIQImbalance,
					    bMaskDWord);
		for (i = 0; i < OFDM_Table_Length; i++) {
			if (tmpRegA == OFDMSwingTable[i]) {
				priv->OFDM_index[0] = (u8)i;
				RT_TRACE(COMP_POWER_TRACKING,
					 "Initial reg0x%x = 0x%x, OFDM_index = 0x%x\n",
					 rOFDM0_XATxIQImbalance, tmpRegA,
					 priv->OFDM_index[0]);
			}
		}

		TempCCk = rtl92e_get_bb_reg(dev, rCCK0_TxFilter1, bMaskByte2);
		for (i = 0; i < CCK_Table_length; i++) {
			if (TempCCk == (u32)CCKSwingTable_Ch1_Ch13[i][0]) {
				priv->CCK_index = (u8) i;
				RT_TRACE(COMP_POWER_TRACKING,
					 "Initial reg0x%x = 0x%x, CCK_index = 0x%x\n",
>>>>>>> v4.9.227
					 rCCK0_TxFilter1, TempCCk,
					 priv->CCK_index);
				break;
			}
		}
		priv->btxpower_trackingInit = true;
		return;
	}

<<<<<<< HEAD
	tmpRegA = rtl8192_phy_QueryRFReg(dev, RF90_PATH_A, 0x12, 0x078);
=======
	tmpRegA = rtl92e_get_rf_reg(dev, RF90_PATH_A, 0x12, 0x078);
>>>>>>> v4.9.227
	RT_TRACE(COMP_POWER_TRACKING, "Readback ThermalMeterA = %d\n", tmpRegA);
	if (tmpRegA < 3 || tmpRegA > 13)
		return;
	if (tmpRegA >= 12)
		tmpRegA = 12;
	RT_TRACE(COMP_POWER_TRACKING, "Valid ThermalMeterA = %d\n", tmpRegA);
	priv->ThermalMeter[0] = ThermalMeterVal;
	priv->ThermalMeter[1] = ThermalMeterVal;

	if (priv->ThermalMeter[0] >= (u8)tmpRegA) {
		tmpOFDMindex = tmpCCK20Mindex = 6+(priv->ThermalMeter[0] -
			      (u8)tmpRegA);
		tmpCCK40Mindex = tmpCCK20Mindex - 6;
		if (tmpOFDMindex >= OFDM_Table_Length)
			tmpOFDMindex = OFDM_Table_Length-1;
		if (tmpCCK20Mindex >= CCK_Table_length)
			tmpCCK20Mindex = CCK_Table_length-1;
		if (tmpCCK40Mindex >= CCK_Table_length)
			tmpCCK40Mindex = CCK_Table_length-1;
	} else {
		tmpval = ((u8)tmpRegA - priv->ThermalMeter[0]);
		if (tmpval >= 6)
			tmpOFDMindex = tmpCCK20Mindex = 0;
		else
			tmpOFDMindex = tmpCCK20Mindex = 6 - tmpval;
		tmpCCK40Mindex = 0;
	}
	if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
		tmpCCKindex = tmpCCK40Mindex;
	else
		tmpCCKindex = tmpCCK20Mindex;

	priv->Record_CCK_20Mindex = tmpCCK20Mindex;
	priv->Record_CCK_40Mindex = tmpCCK40Mindex;
<<<<<<< HEAD
	RT_TRACE(COMP_POWER_TRACKING, "Record_CCK_20Mindex / Record_CCK_40"
		 "Mindex = %d / %d.\n",
=======
	RT_TRACE(COMP_POWER_TRACKING,
		 "Record_CCK_20Mindex / Record_CCK_40Mindex = %d / %d.\n",
>>>>>>> v4.9.227
		 priv->Record_CCK_20Mindex, priv->Record_CCK_40Mindex);

	if (priv->rtllib->current_network.channel == 14 &&
	    !priv->bcck_in_ch14) {
		priv->bcck_in_ch14 = true;
		CCKSwingNeedUpdate = 1;
	} else if (priv->rtllib->current_network.channel != 14 &&
		   priv->bcck_in_ch14) {
		priv->bcck_in_ch14 = false;
		CCKSwingNeedUpdate = 1;
	}

	if (priv->CCK_index != tmpCCKindex) {
		priv->CCK_index = tmpCCKindex;
		CCKSwingNeedUpdate = 1;
	}

	if (CCKSwingNeedUpdate)
<<<<<<< HEAD
		dm_cck_txpower_adjust(dev, priv->bcck_in_ch14);
	if (priv->OFDM_index[0] != tmpOFDMindex) {
		priv->OFDM_index[0] = tmpOFDMindex;
		rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance, bMaskDWord,
				 OFDMSwingTable[priv->OFDM_index[0]]);
=======
		rtl92e_dm_cck_txpower_adjust(dev, priv->bcck_in_ch14);
	if (priv->OFDM_index[0] != tmpOFDMindex) {
		priv->OFDM_index[0] = tmpOFDMindex;
		rtl92e_set_bb_reg(dev, rOFDM0_XATxIQImbalance, bMaskDWord,
				  OFDMSwingTable[priv->OFDM_index[0]]);
>>>>>>> v4.9.227
		RT_TRACE(COMP_POWER_TRACKING, "Update OFDMSwing[%d] = 0x%x\n",
			 priv->OFDM_index[0],
			 OFDMSwingTable[priv->OFDM_index[0]]);
	}
	priv->txpower_count = 0;
}

<<<<<<< HEAD
void	dm_txpower_trackingcallback(void *data)
=======
void rtl92e_dm_txpower_tracking_wq(void *data)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = container_of_dwork_rsl(data,
				  struct r8192_priv, txpower_tracking_wq);
	struct net_device *dev = priv->rtllib->dev;

	if (priv->IC_Cut >= IC_VersionCut_D)
<<<<<<< HEAD
		dm_TXPowerTrackingCallback_TSSI(dev);
	else
		dm_TXPowerTrackingCallback_ThermalMeter(dev);
}

static void dm_InitializeTXPowerTracking_TSSI(struct net_device *dev)
=======
		_rtl92e_dm_tx_power_tracking_callback_tssi(dev);
	else
		_rtl92e_dm_tx_power_tracking_cb_thermal(dev);
}

static void _rtl92e_dm_initialize_tx_power_tracking_tssi(struct net_device *dev)
>>>>>>> v4.9.227
{

	struct r8192_priv *priv = rtllib_priv(dev);

<<<<<<< HEAD
	priv->txbbgain_table[0].txbb_iq_amplifygain = 12;
	priv->txbbgain_table[0].txbbgain_value = 0x7f8001fe;
	priv->txbbgain_table[1].txbb_iq_amplifygain = 11;
	priv->txbbgain_table[1].txbbgain_value = 0x788001e2;
	priv->txbbgain_table[2].txbb_iq_amplifygain = 10;
	priv->txbbgain_table[2].txbbgain_value = 0x71c001c7;
	priv->txbbgain_table[3].txbb_iq_amplifygain = 9;
	priv->txbbgain_table[3].txbbgain_value = 0x6b8001ae;
	priv->txbbgain_table[4].txbb_iq_amplifygain = 8;
	priv->txbbgain_table[4].txbbgain_value = 0x65400195;
	priv->txbbgain_table[5].txbb_iq_amplifygain = 7;
	priv->txbbgain_table[5].txbbgain_value = 0x5fc0017f;
	priv->txbbgain_table[6].txbb_iq_amplifygain = 6;
	priv->txbbgain_table[6].txbbgain_value = 0x5a400169;
	priv->txbbgain_table[7].txbb_iq_amplifygain = 5;
	priv->txbbgain_table[7].txbbgain_value = 0x55400155;
	priv->txbbgain_table[8].txbb_iq_amplifygain = 4;
	priv->txbbgain_table[8].txbbgain_value = 0x50800142;
	priv->txbbgain_table[9].txbb_iq_amplifygain = 3;
	priv->txbbgain_table[9].txbbgain_value = 0x4c000130;
	priv->txbbgain_table[10].txbb_iq_amplifygain = 2;
	priv->txbbgain_table[10].txbbgain_value = 0x47c0011f;
	priv->txbbgain_table[11].txbb_iq_amplifygain = 1;
	priv->txbbgain_table[11].txbbgain_value = 0x43c0010f;
	priv->txbbgain_table[12].txbb_iq_amplifygain = 0;
	priv->txbbgain_table[12].txbbgain_value = 0x40000100;
	priv->txbbgain_table[13].txbb_iq_amplifygain = -1;
	priv->txbbgain_table[13].txbbgain_value = 0x3c8000f2;
	priv->txbbgain_table[14].txbb_iq_amplifygain = -2;
	priv->txbbgain_table[14].txbbgain_value = 0x390000e4;
	priv->txbbgain_table[15].txbb_iq_amplifygain = -3;
	priv->txbbgain_table[15].txbbgain_value = 0x35c000d7;
	priv->txbbgain_table[16].txbb_iq_amplifygain = -4;
	priv->txbbgain_table[16].txbbgain_value = 0x32c000cb;
	priv->txbbgain_table[17].txbb_iq_amplifygain = -5;
	priv->txbbgain_table[17].txbbgain_value = 0x300000c0;
	priv->txbbgain_table[18].txbb_iq_amplifygain = -6;
	priv->txbbgain_table[18].txbbgain_value = 0x2d4000b5;
	priv->txbbgain_table[19].txbb_iq_amplifygain = -7;
	priv->txbbgain_table[19].txbbgain_value = 0x2ac000ab;
	priv->txbbgain_table[20].txbb_iq_amplifygain = -8;
	priv->txbbgain_table[20].txbbgain_value = 0x288000a2;
	priv->txbbgain_table[21].txbb_iq_amplifygain = -9;
	priv->txbbgain_table[21].txbbgain_value = 0x26000098;
	priv->txbbgain_table[22].txbb_iq_amplifygain = -10;
	priv->txbbgain_table[22].txbbgain_value = 0x24000090;
	priv->txbbgain_table[23].txbb_iq_amplifygain = -11;
	priv->txbbgain_table[23].txbbgain_value = 0x22000088;
	priv->txbbgain_table[24].txbb_iq_amplifygain = -12;
	priv->txbbgain_table[24].txbbgain_value = 0x20000080;
	priv->txbbgain_table[25].txbb_iq_amplifygain = -13;
	priv->txbbgain_table[25].txbbgain_value = 0x1a00006c;
	priv->txbbgain_table[26].txbb_iq_amplifygain = -14;
	priv->txbbgain_table[26].txbbgain_value = 0x1c800072;
	priv->txbbgain_table[27].txbb_iq_amplifygain = -15;
	priv->txbbgain_table[27].txbbgain_value = 0x18000060;
	priv->txbbgain_table[28].txbb_iq_amplifygain = -16;
	priv->txbbgain_table[28].txbbgain_value = 0x19800066;
	priv->txbbgain_table[29].txbb_iq_amplifygain = -17;
	priv->txbbgain_table[29].txbbgain_value = 0x15800056;
	priv->txbbgain_table[30].txbb_iq_amplifygain = -18;
	priv->txbbgain_table[30].txbbgain_value = 0x26c0005b;
	priv->txbbgain_table[31].txbb_iq_amplifygain = -19;
	priv->txbbgain_table[31].txbbgain_value = 0x14400051;
	priv->txbbgain_table[32].txbb_iq_amplifygain = -20;
	priv->txbbgain_table[32].txbbgain_value = 0x24400051;
	priv->txbbgain_table[33].txbb_iq_amplifygain = -21;
	priv->txbbgain_table[33].txbbgain_value = 0x1300004c;
	priv->txbbgain_table[34].txbb_iq_amplifygain = -22;
	priv->txbbgain_table[34].txbbgain_value = 0x12000048;
	priv->txbbgain_table[35].txbb_iq_amplifygain = -23;
	priv->txbbgain_table[35].txbbgain_value = 0x11000044;
	priv->txbbgain_table[36].txbb_iq_amplifygain = -24;
	priv->txbbgain_table[36].txbbgain_value = 0x10000040;

	priv->cck_txbbgain_table[0].ccktxbb_valuearray[0] = 0x36;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[1] = 0x35;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[2] = 0x2e;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[3] = 0x25;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[4] = 0x1c;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[5] = 0x12;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[6] = 0x09;
	priv->cck_txbbgain_table[0].ccktxbb_valuearray[7] = 0x04;

	priv->cck_txbbgain_table[1].ccktxbb_valuearray[0] = 0x33;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[1] = 0x32;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[2] = 0x2b;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[3] = 0x23;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[4] = 0x1a;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[5] = 0x11;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[6] = 0x08;
	priv->cck_txbbgain_table[1].ccktxbb_valuearray[7] = 0x04;

	priv->cck_txbbgain_table[2].ccktxbb_valuearray[0] = 0x30;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[1] = 0x2f;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[2] = 0x29;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[3] = 0x21;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[4] = 0x19;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[5] = 0x10;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[6] = 0x08;
	priv->cck_txbbgain_table[2].ccktxbb_valuearray[7] = 0x03;

	priv->cck_txbbgain_table[3].ccktxbb_valuearray[0] = 0x2d;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[1] = 0x2d;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[2] = 0x27;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[3] = 0x1f;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[4] = 0x18;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[5] = 0x0f;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[6] = 0x08;
	priv->cck_txbbgain_table[3].ccktxbb_valuearray[7] = 0x03;

	priv->cck_txbbgain_table[4].ccktxbb_valuearray[0] = 0x2b;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[1] = 0x2a;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[2] = 0x25;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[3] = 0x1e;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[4] = 0x16;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[5] = 0x0e;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[6] = 0x07;
	priv->cck_txbbgain_table[4].ccktxbb_valuearray[7] = 0x03;

	priv->cck_txbbgain_table[5].ccktxbb_valuearray[0] = 0x28;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[1] = 0x28;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[2] = 0x22;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[3] = 0x1c;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[4] = 0x15;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[5] = 0x0d;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[6] = 0x07;
	priv->cck_txbbgain_table[5].ccktxbb_valuearray[7] = 0x03;

	priv->cck_txbbgain_table[6].ccktxbb_valuearray[0] = 0x26;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[1] = 0x25;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[2] = 0x21;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[3] = 0x1b;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[4] = 0x14;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[5] = 0x0d;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[6] = 0x06;
	priv->cck_txbbgain_table[6].ccktxbb_valuearray[7] = 0x03;

	priv->cck_txbbgain_table[7].ccktxbb_valuearray[0] = 0x24;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[1] = 0x23;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[2] = 0x1f;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[3] = 0x19;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[4] = 0x13;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[5] = 0x0c;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[6] = 0x06;
	priv->cck_txbbgain_table[7].ccktxbb_valuearray[7] = 0x03;

	priv->cck_txbbgain_table[8].ccktxbb_valuearray[0] = 0x22;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[1] = 0x21;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[2] = 0x1d;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[3] = 0x18;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[4] = 0x11;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[5] = 0x0b;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[6] = 0x06;
	priv->cck_txbbgain_table[8].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[9].ccktxbb_valuearray[0] = 0x20;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[1] = 0x20;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[2] = 0x1b;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[3] = 0x16;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[4] = 0x11;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[5] = 0x08;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[6] = 0x05;
	priv->cck_txbbgain_table[9].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[10].ccktxbb_valuearray[0] = 0x1f;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[1] = 0x1e;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[2] = 0x1a;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[3] = 0x15;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[4] = 0x10;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[5] = 0x0a;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[6] = 0x05;
	priv->cck_txbbgain_table[10].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[11].ccktxbb_valuearray[0] = 0x1d;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[1] = 0x1c;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[2] = 0x18;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[3] = 0x14;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[4] = 0x0f;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[5] = 0x0a;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[6] = 0x05;
	priv->cck_txbbgain_table[11].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[12].ccktxbb_valuearray[0] = 0x1b;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[1] = 0x1a;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[2] = 0x17;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[3] = 0x13;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[4] = 0x0e;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[5] = 0x09;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[6] = 0x04;
	priv->cck_txbbgain_table[12].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[13].ccktxbb_valuearray[0] = 0x1a;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[1] = 0x19;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[2] = 0x16;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[3] = 0x12;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[4] = 0x0d;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[5] = 0x09;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[6] = 0x04;
	priv->cck_txbbgain_table[13].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[14].ccktxbb_valuearray[0] = 0x18;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[1] = 0x17;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[2] = 0x15;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[3] = 0x11;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[4] = 0x0c;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[5] = 0x08;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[6] = 0x04;
	priv->cck_txbbgain_table[14].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[15].ccktxbb_valuearray[0] = 0x17;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[1] = 0x16;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[2] = 0x13;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[3] = 0x10;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[4] = 0x0c;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[5] = 0x08;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[6] = 0x04;
	priv->cck_txbbgain_table[15].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[16].ccktxbb_valuearray[0] = 0x16;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[1] = 0x15;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[2] = 0x12;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[3] = 0x0f;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[4] = 0x0b;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[5] = 0x07;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[6] = 0x04;
	priv->cck_txbbgain_table[16].ccktxbb_valuearray[7] = 0x01;

	priv->cck_txbbgain_table[17].ccktxbb_valuearray[0] = 0x14;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[1] = 0x14;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[2] = 0x11;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[3] = 0x0e;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[4] = 0x0b;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[5] = 0x07;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[6] = 0x03;
	priv->cck_txbbgain_table[17].ccktxbb_valuearray[7] = 0x02;

	priv->cck_txbbgain_table[18].ccktxbb_valuearray[0] = 0x13;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[1] = 0x13;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[2] = 0x10;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[3] = 0x0d;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[4] = 0x0a;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[5] = 0x06;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[6] = 0x03;
	priv->cck_txbbgain_table[18].ccktxbb_valuearray[7] = 0x01;

	priv->cck_txbbgain_table[19].ccktxbb_valuearray[0] = 0x12;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[1] = 0x12;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[2] = 0x0f;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[3] = 0x0c;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[4] = 0x09;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[5] = 0x06;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[6] = 0x03;
	priv->cck_txbbgain_table[19].ccktxbb_valuearray[7] = 0x01;

	priv->cck_txbbgain_table[20].ccktxbb_valuearray[0] = 0x11;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[1] = 0x11;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[2] = 0x0f;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[3] = 0x0c;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[4] = 0x09;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[5] = 0x06;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[6] = 0x03;
	priv->cck_txbbgain_table[20].ccktxbb_valuearray[7] = 0x01;

	priv->cck_txbbgain_table[21].ccktxbb_valuearray[0] = 0x10;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[1] = 0x10;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[2] = 0x0e;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[3] = 0x0b;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[4] = 0x08;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[5] = 0x05;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[6] = 0x03;
	priv->cck_txbbgain_table[21].ccktxbb_valuearray[7] = 0x01;

	priv->cck_txbbgain_table[22].ccktxbb_valuearray[0] = 0x0f;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[1] = 0x0f;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[2] = 0x0d;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[3] = 0x0b;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[4] = 0x08;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[5] = 0x05;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[6] = 0x03;
	priv->cck_txbbgain_table[22].ccktxbb_valuearray[7] = 0x01;

	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[0] = 0x36;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[1] = 0x35;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[2] = 0x2e;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[3] = 0x1b;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[0].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[0] = 0x33;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[1] = 0x32;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[2] = 0x2b;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[3] = 0x19;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[1].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[0] = 0x30;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[1] = 0x2f;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[2] = 0x29;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[3] = 0x18;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[2].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[0] = 0x2d;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[1] = 0x2d;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[2] = 0x27;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[3] = 0x17;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[3].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[0] = 0x2b;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[1] = 0x2a;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[2] = 0x25;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[3] = 0x15;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[4].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[0] = 0x28;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[1] = 0x28;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[2] = 0x22;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[3] = 0x14;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[5].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[0] = 0x26;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[1] = 0x25;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[2] = 0x21;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[3] = 0x13;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[6].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[0] = 0x24;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[1] = 0x23;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[2] = 0x1f;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[3] = 0x12;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[7].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[0] = 0x22;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[1] = 0x21;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[2] = 0x1d;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[3] = 0x11;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[8].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[0] = 0x20;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[1] = 0x20;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[2] = 0x1b;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[3] = 0x10;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[9].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[0] = 0x1f;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[1] = 0x1e;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[2] = 0x1a;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[3] = 0x0f;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[10].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[0] = 0x1d;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[1] = 0x1c;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[2] = 0x18;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[3] = 0x0e;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[11].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[0] = 0x1b;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[1] = 0x1a;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[2] = 0x17;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[3] = 0x0e;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[12].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[0] = 0x1a;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[1] = 0x19;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[2] = 0x16;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[3] = 0x0d;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[13].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[0] = 0x18;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[1] = 0x17;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[2] = 0x15;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[3] = 0x0c;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[14].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[0] = 0x17;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[1] = 0x16;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[2] = 0x13;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[3] = 0x0b;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[15].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[0] = 0x16;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[1] = 0x15;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[2] = 0x12;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[3] = 0x0b;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[16].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[0] = 0x14;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[1] = 0x14;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[2] = 0x11;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[3] = 0x0a;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[17].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[0] = 0x13;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[1] = 0x13;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[2] = 0x10;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[3] = 0x0a;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[18].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[0] = 0x12;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[1] = 0x12;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[2] = 0x0f;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[3] = 0x09;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[19].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[0] = 0x11;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[1] = 0x11;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[2] = 0x0f;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[3] = 0x09;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[20].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[0] = 0x10;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[1] = 0x10;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[2] = 0x0e;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[3] = 0x08;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[21].ccktxbb_valuearray[7] = 0x00;

	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[0] = 0x0f;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[1] = 0x0f;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[2] = 0x0d;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[3] = 0x08;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[4] = 0x00;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[5] = 0x00;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[6] = 0x00;
	priv->cck_txbbgain_ch14_table[22].ccktxbb_valuearray[7] = 0x00;

=======
>>>>>>> v4.9.227
	priv->btxpower_tracking = true;
	priv->txpower_count       = 0;
	priv->btxpower_trackingInit = false;

}

<<<<<<< HEAD
static void dm_InitializeTXPowerTracking_ThermalMeter(struct net_device *dev)
=======
static void _rtl92e_dm_init_tx_power_tracking_thermal(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);


	if (priv->rtllib->FwRWRF)
		priv->btxpower_tracking = true;
	else
		priv->btxpower_tracking = false;
	priv->txpower_count       = 0;
	priv->btxpower_trackingInit = false;
	RT_TRACE(COMP_POWER_TRACKING, "pMgntInfo->bTXPowerTracking = %d\n",
		 priv->btxpower_tracking);
}

<<<<<<< HEAD
void dm_initialize_txpower_tracking(struct net_device *dev)
=======
void rtl92e_dm_init_txpower_tracking(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->IC_Cut >= IC_VersionCut_D)
<<<<<<< HEAD
		dm_InitializeTXPowerTracking_TSSI(dev);
	else
		dm_InitializeTXPowerTracking_ThermalMeter(dev);
}

static void dm_CheckTXPowerTracking_TSSI(struct net_device *dev)
=======
		_rtl92e_dm_initialize_tx_power_tracking_tssi(dev);
	else
		_rtl92e_dm_init_tx_power_tracking_thermal(dev);
}

static void _rtl92e_dm_check_tx_power_tracking_tssi(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static u32 tx_power_track_counter;

	RT_TRACE(COMP_POWER_TRACKING, "%s()\n", __func__);
<<<<<<< HEAD
	if (read_nic_byte(dev, 0x11e) == 1)
=======
	if (rtl92e_readb(dev, 0x11e) == 1)
>>>>>>> v4.9.227
		return;
	if (!priv->btxpower_tracking)
		return;
	tx_power_track_counter++;


	 if (tx_power_track_counter >= 180) {
<<<<<<< HEAD
		queue_delayed_work_rsl(priv->priv_wq, &priv->txpower_tracking_wq, 0);
=======
		schedule_delayed_work(&priv->txpower_tracking_wq, 0);
>>>>>>> v4.9.227
		tx_power_track_counter = 0;
	}

}
<<<<<<< HEAD
static void dm_CheckTXPowerTracking_ThermalMeter(struct net_device *dev)
=======

static void _rtl92e_dm_check_tx_power_tracking_thermal(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static u8	TM_Trigger;
	u8		TxPowerCheckCnt = 0;

	if (IS_HARDWARE_TYPE_8192SE(dev))
		TxPowerCheckCnt = 5;
	else
		TxPowerCheckCnt = 2;
<<<<<<< HEAD
	if (!priv->btxpower_tracking) {
		return;
	} else {
		if (priv->txpower_count  <= TxPowerCheckCnt) {
			priv->txpower_count++;
			return;
		}
	}

	if (!TM_Trigger) {
		{
		rtl8192_phy_SetRFReg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4d);
		rtl8192_phy_SetRFReg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4f);
		rtl8192_phy_SetRFReg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4d);
		rtl8192_phy_SetRFReg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4f);
		}
		TM_Trigger = 1;
		return;
	} else {
	    printk(KERN_INFO "===============>Schedule TxPowerTrackingWorkItem\n");

		queue_delayed_work_rsl(priv->priv_wq, &priv->txpower_tracking_wq, 0);
		TM_Trigger = 0;
		}

	}

static void dm_check_txpower_tracking(struct net_device *dev)
=======
	if (!priv->btxpower_tracking)
		return;

	if (priv->txpower_count  <= TxPowerCheckCnt) {
		priv->txpower_count++;
		return;
	}

	if (!TM_Trigger) {
		rtl92e_set_rf_reg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4d);
		rtl92e_set_rf_reg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4f);
		rtl92e_set_rf_reg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4d);
		rtl92e_set_rf_reg(dev, RF90_PATH_A, 0x02, bMask12Bits, 0x4f);
		TM_Trigger = 1;
		return;
	}
	netdev_info(dev, "===============>Schedule TxPowerTrackingWorkItem\n");
	schedule_delayed_work(&priv->txpower_tracking_wq, 0);
	TM_Trigger = 0;

}

static void _rtl92e_dm_check_tx_power_tracking(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->IC_Cut >= IC_VersionCut_D)
<<<<<<< HEAD
		dm_CheckTXPowerTracking_TSSI(dev);
	else
		dm_CheckTXPowerTracking_ThermalMeter(dev);
}

static void dm_CCKTxPowerAdjust_TSSI(struct net_device *dev, bool  bInCH14)
=======
		_rtl92e_dm_check_tx_power_tracking_tssi(dev);
	else
		_rtl92e_dm_check_tx_power_tracking_thermal(dev);
}

static void _rtl92e_dm_cck_tx_power_adjust_tssi(struct net_device *dev,
						bool bInCH14)
{
	u32 TempVal;
	struct r8192_priv *priv = rtllib_priv(dev);
	u8 attenuation = (u8)priv->CCKPresentAttentuation;

	TempVal = 0;
	if (!bInCH14) {
		TempVal = (u32)(dm_cck_tx_bb_gain[attenuation][0] +
			  (dm_cck_tx_bb_gain[attenuation][1] << 8));

		rtl92e_set_bb_reg(dev, rCCK0_TxFilter1, bMaskHWord, TempVal);
		TempVal = (u32)((dm_cck_tx_bb_gain[attenuation][2]) +
			  (dm_cck_tx_bb_gain[attenuation][3] << 8) +
			  (dm_cck_tx_bb_gain[attenuation][4] << 16)+
			  (dm_cck_tx_bb_gain[attenuation][5] << 24));
		rtl92e_set_bb_reg(dev, rCCK0_TxFilter2, bMaskDWord, TempVal);
		TempVal = (u32)(dm_cck_tx_bb_gain[attenuation][6] +
			  (dm_cck_tx_bb_gain[attenuation][7] << 8));

		rtl92e_set_bb_reg(dev, rCCK0_DebugPort, bMaskLWord, TempVal);
	} else {
		TempVal = (u32)((dm_cck_tx_bb_gain_ch14[attenuation][0]) +
			  (dm_cck_tx_bb_gain_ch14[attenuation][1] << 8));

		rtl92e_set_bb_reg(dev, rCCK0_TxFilter1, bMaskHWord, TempVal);
		TempVal = (u32)((dm_cck_tx_bb_gain_ch14[attenuation][2]) +
			  (dm_cck_tx_bb_gain_ch14[attenuation][3] << 8) +
			  (dm_cck_tx_bb_gain_ch14[attenuation][4] << 16)+
			  (dm_cck_tx_bb_gain_ch14[attenuation][5] << 24));
		rtl92e_set_bb_reg(dev, rCCK0_TxFilter2, bMaskDWord, TempVal);
		TempVal = (u32)((dm_cck_tx_bb_gain_ch14[attenuation][6]) +
			  (dm_cck_tx_bb_gain_ch14[attenuation][7] << 8));

		rtl92e_set_bb_reg(dev, rCCK0_DebugPort, bMaskLWord, TempVal);
	}
}

static void _rtl92e_dm_cck_tx_power_adjust_thermal_meter(struct net_device *dev,
							 bool bInCH14)
>>>>>>> v4.9.227
{
	u32 TempVal;
	struct r8192_priv *priv = rtllib_priv(dev);

	TempVal = 0;
	if (!bInCH14) {
<<<<<<< HEAD
		TempVal = (u32)(priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[0] +
			  (priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[1]<<8)) ;

		rtl8192_setBBreg(dev, rCCK0_TxFilter1, bMaskHWord, TempVal);
		TempVal = 0;
		TempVal = (u32)(priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[2] +
			  (priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[3]<<8) +
			  (priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[4]<<16)+
			  (priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[5]<<24));
		rtl8192_setBBreg(dev, rCCK0_TxFilter2, bMaskDWord, TempVal);
		TempVal = 0;
		TempVal = (u32)(priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[6] +
			  (priv->cck_txbbgain_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[7]<<8)) ;

		rtl8192_setBBreg(dev, rCCK0_DebugPort, bMaskLWord, TempVal);
	} else {
		TempVal = (u32)(priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[0] +
			  (priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[1]<<8)) ;

		rtl8192_setBBreg(dev, rCCK0_TxFilter1, bMaskHWord, TempVal);
		TempVal = 0;
		TempVal = (u32)(priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[2] +
			  (priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[3]<<8) +
			  (priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[4]<<16)+
			  (priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[5]<<24));
		rtl8192_setBBreg(dev, rCCK0_TxFilter2, bMaskDWord, TempVal);
		TempVal = 0;
		TempVal = (u32)(priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[6] +
			  (priv->cck_txbbgain_ch14_table[(u8)(priv->CCKPresentAttentuation)].ccktxbb_valuearray[7]<<8)) ;

		rtl8192_setBBreg(dev, rCCK0_DebugPort, bMaskLWord, TempVal);
	}


}

static void dm_CCKTxPowerAdjust_ThermalMeter(struct net_device *dev,	bool  bInCH14)
{
	u32 TempVal;
	struct r8192_priv *priv = rtllib_priv(dev);

	TempVal = 0;
	if (!bInCH14) {
		TempVal =	CCKSwingTable_Ch1_Ch13[priv->CCK_index][0] +
					(CCKSwingTable_Ch1_Ch13[priv->CCK_index][1]<<8) ;
		rtl8192_setBBreg(dev, rCCK0_TxFilter1, bMaskHWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK not chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_TxFilter1, TempVal);
		TempVal = 0;
		TempVal =	CCKSwingTable_Ch1_Ch13[priv->CCK_index][2] +
					(CCKSwingTable_Ch1_Ch13[priv->CCK_index][3]<<8) +
					(CCKSwingTable_Ch1_Ch13[priv->CCK_index][4]<<16)+
					(CCKSwingTable_Ch1_Ch13[priv->CCK_index][5]<<24);
		rtl8192_setBBreg(dev, rCCK0_TxFilter2, bMaskDWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK not chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_TxFilter2, TempVal);
		TempVal = 0;
		TempVal =	CCKSwingTable_Ch1_Ch13[priv->CCK_index][6] +
					(CCKSwingTable_Ch1_Ch13[priv->CCK_index][7]<<8) ;

		rtl8192_setBBreg(dev, rCCK0_DebugPort, bMaskLWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK not chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_DebugPort, TempVal);
	} else {
		TempVal =	CCKSwingTable_Ch14[priv->CCK_index][0] +
					(CCKSwingTable_Ch14[priv->CCK_index][1]<<8) ;

		rtl8192_setBBreg(dev, rCCK0_TxFilter1, bMaskHWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_TxFilter1, TempVal);
		TempVal = 0;
		TempVal =	CCKSwingTable_Ch14[priv->CCK_index][2] +
					(CCKSwingTable_Ch14[priv->CCK_index][3]<<8) +
					(CCKSwingTable_Ch14[priv->CCK_index][4]<<16)+
					(CCKSwingTable_Ch14[priv->CCK_index][5]<<24);
		rtl8192_setBBreg(dev, rCCK0_TxFilter2, bMaskDWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_TxFilter2, TempVal);
		TempVal = 0;
		TempVal =	CCKSwingTable_Ch14[priv->CCK_index][6] +
					(CCKSwingTable_Ch14[priv->CCK_index][7]<<8) ;

		rtl8192_setBBreg(dev, rCCK0_DebugPort, bMaskLWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_DebugPort, TempVal);
	}
	}

void dm_cck_txpower_adjust(struct net_device *dev, bool  binch14)
=======
		TempVal = CCKSwingTable_Ch1_Ch13[priv->CCK_index][0] +
			  (CCKSwingTable_Ch1_Ch13[priv->CCK_index][1] << 8);
		rtl92e_set_bb_reg(dev, rCCK0_TxFilter1, bMaskHWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING,
			 "CCK not chnl 14, reg 0x%x = 0x%x\n", rCCK0_TxFilter1,
			 TempVal);
		TempVal = CCKSwingTable_Ch1_Ch13[priv->CCK_index][2] +
			  (CCKSwingTable_Ch1_Ch13[priv->CCK_index][3] << 8) +
			  (CCKSwingTable_Ch1_Ch13[priv->CCK_index][4] << 16)+
			  (CCKSwingTable_Ch1_Ch13[priv->CCK_index][5] << 24);
		rtl92e_set_bb_reg(dev, rCCK0_TxFilter2, bMaskDWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING,
			 "CCK not chnl 14, reg 0x%x = 0x%x\n", rCCK0_TxFilter2,
			 TempVal);
		TempVal = CCKSwingTable_Ch1_Ch13[priv->CCK_index][6] +
			  (CCKSwingTable_Ch1_Ch13[priv->CCK_index][7] << 8);

		rtl92e_set_bb_reg(dev, rCCK0_DebugPort, bMaskLWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING,
			 "CCK not chnl 14, reg 0x%x = 0x%x\n", rCCK0_DebugPort,
			 TempVal);
	} else {
		TempVal = CCKSwingTable_Ch14[priv->CCK_index][0] +
			  (CCKSwingTable_Ch14[priv->CCK_index][1] << 8);

		rtl92e_set_bb_reg(dev, rCCK0_TxFilter1, bMaskHWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_TxFilter1, TempVal);
		TempVal = CCKSwingTable_Ch14[priv->CCK_index][2] +
			  (CCKSwingTable_Ch14[priv->CCK_index][3] << 8) +
			  (CCKSwingTable_Ch14[priv->CCK_index][4] << 16)+
			  (CCKSwingTable_Ch14[priv->CCK_index][5] << 24);
		rtl92e_set_bb_reg(dev, rCCK0_TxFilter2, bMaskDWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_TxFilter2, TempVal);
		TempVal = CCKSwingTable_Ch14[priv->CCK_index][6] +
			  (CCKSwingTable_Ch14[priv->CCK_index][7]<<8);

		rtl92e_set_bb_reg(dev, rCCK0_DebugPort, bMaskLWord, TempVal);
		RT_TRACE(COMP_POWER_TRACKING, "CCK chnl 14, reg 0x%x = 0x%x\n",
			rCCK0_DebugPort, TempVal);
	}
}

void rtl92e_dm_cck_txpower_adjust(struct net_device *dev, bool binch14)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	if (priv->IC_Cut >= IC_VersionCut_D)
<<<<<<< HEAD
		dm_CCKTxPowerAdjust_TSSI(dev, binch14);
	else
		dm_CCKTxPowerAdjust_ThermalMeter(dev, binch14);
}

static void dm_txpower_reset_recovery(struct net_device *dev)
=======
		_rtl92e_dm_cck_tx_power_adjust_tssi(dev, binch14);
	else
		_rtl92e_dm_cck_tx_power_adjust_thermal_meter(dev, binch14);
}

static void _rtl92e_dm_tx_power_reset_recovery(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	RT_TRACE(COMP_POWER_TRACKING, "Start Reset Recovery ==>\n");
<<<<<<< HEAD
	rtl8192_setBBreg(dev, rOFDM0_XATxIQImbalance, bMaskDWord,
			 priv->txbbgain_table[priv->rfa_txpowertrackingindex].txbbgain_value);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery: Fill in 0xc80 is %08x\n",
		 priv->txbbgain_table[priv->rfa_txpowertrackingindex].txbbgain_value);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery: Fill in RFA_txPowerTrackingIndex is %x\n",
		 priv->rfa_txpowertrackingindex);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery : RF A I/Q Amplify Gain is %ld\n",
		 priv->txbbgain_table[priv->rfa_txpowertrackingindex].txbb_iq_amplifygain);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery: CCK Attenuation is %d dB\n",
		 priv->CCKPresentAttentuation);
	dm_cck_txpower_adjust(dev, priv->bcck_in_ch14);

	rtl8192_setBBreg(dev, rOFDM0_XCTxIQImbalance, bMaskDWord,
			 priv->txbbgain_table[priv->rfc_txpowertrackingindex].txbbgain_value);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery: Fill in 0xc90 is %08x\n",
		 priv->txbbgain_table[priv->rfc_txpowertrackingindex].txbbgain_value);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery: Fill in RFC_txPowerTrackingIndex is %x\n",
		 priv->rfc_txpowertrackingindex);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery : RF C I/Q Amplify Gain is %ld\n",
		 priv->txbbgain_table[priv->rfc_txpowertrackingindex].txbb_iq_amplifygain);

}

void dm_restore_dynamic_mechanism_state(struct net_device *dev)
=======
	rtl92e_set_bb_reg(dev, rOFDM0_XATxIQImbalance, bMaskDWord,
			  dm_tx_bb_gain[priv->rfa_txpowertrackingindex]);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery: Fill in 0xc80 is %08x\n",
		 dm_tx_bb_gain[priv->rfa_txpowertrackingindex]);
	RT_TRACE(COMP_POWER_TRACKING,
		 "Reset Recovery: Fill in RFA_txPowerTrackingIndex is %x\n",
		 priv->rfa_txpowertrackingindex);
	RT_TRACE(COMP_POWER_TRACKING,
		 "Reset Recovery : RF A I/Q Amplify Gain is %d\n",
		 dm_tx_bb_gain_idx_to_amplify(priv->rfa_txpowertrackingindex));
	RT_TRACE(COMP_POWER_TRACKING,
		 "Reset Recovery: CCK Attenuation is %d dB\n",
		 priv->CCKPresentAttentuation);
	rtl92e_dm_cck_txpower_adjust(dev, priv->bcck_in_ch14);

	rtl92e_set_bb_reg(dev, rOFDM0_XCTxIQImbalance, bMaskDWord,
			  dm_tx_bb_gain[priv->rfc_txpowertrackingindex]);
	RT_TRACE(COMP_POWER_TRACKING, "Reset Recovery: Fill in 0xc90 is %08x\n",
		 dm_tx_bb_gain[priv->rfc_txpowertrackingindex]);
	RT_TRACE(COMP_POWER_TRACKING,
		 "Reset Recovery: Fill in RFC_txPowerTrackingIndex is %x\n",
		 priv->rfc_txpowertrackingindex);
	RT_TRACE(COMP_POWER_TRACKING,
		 "Reset Recovery : RF C I/Q Amplify Gain is %d\n",
		 dm_tx_bb_gain_idx_to_amplify(priv->rfc_txpowertrackingindex));
}

void rtl92e_dm_restore_state(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u32	reg_ratr = priv->rate_adaptive.last_ratr;
	u32 ratr_value;

<<<<<<< HEAD
	if (IS_NIC_DOWN(priv)) {
		RT_TRACE(COMP_RATE, "<---- dm_restore_dynamic_mechanism_state(): driver is going to unload\n");
=======
	if (!priv->up) {
		RT_TRACE(COMP_RATE,
			 "<---- rtl92e_dm_restore_state(): driver is going to unload\n");
>>>>>>> v4.9.227
		return;
	}

	if (priv->rate_adaptive.rate_adaptive_disabled)
		return;
	if (!(priv->rtllib->mode == WIRELESS_MODE_N_24G ||
	      priv->rtllib->mode == WIRELESS_MODE_N_5G))
		return;
	ratr_value = reg_ratr;
	if (priv->rf_type == RF_1T2R)
		ratr_value &= ~(RATE_ALL_OFDM_2SS);
<<<<<<< HEAD
	write_nic_dword(dev, RATR0, ratr_value);
	write_nic_byte(dev, UFWP, 1);
	if (priv->btxpower_trackingInit && priv->btxpower_tracking)
		dm_txpower_reset_recovery(dev);

	dm_bb_initialgain_restore(dev);

}

static void dm_bb_initialgain_restore(struct net_device *dev)
=======
	rtl92e_writel(dev, RATR0, ratr_value);
	rtl92e_writeb(dev, UFWP, 1);
	if (priv->btxpower_trackingInit && priv->btxpower_tracking)
		_rtl92e_dm_tx_power_reset_recovery(dev);

	_rtl92e_dm_bb_initialgain_restore(dev);

}

static void _rtl92e_dm_bb_initialgain_restore(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u32 bit_mask = 0x7f;

	if (dm_digtable.dig_algorithm == DIG_ALGO_BY_RSSI)
		return;

<<<<<<< HEAD
	rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x8);
	rtl8192_setBBreg(dev, rOFDM0_XAAGCCore1, bit_mask, (u32)priv->initgain_backup.xaagccore1);
	rtl8192_setBBreg(dev, rOFDM0_XBAGCCore1, bit_mask, (u32)priv->initgain_backup.xbagccore1);
	rtl8192_setBBreg(dev, rOFDM0_XCAGCCore1, bit_mask, (u32)priv->initgain_backup.xcagccore1);
	rtl8192_setBBreg(dev, rOFDM0_XDAGCCore1, bit_mask, (u32)priv->initgain_backup.xdagccore1);
	bit_mask  = bMaskByte2;
	rtl8192_setBBreg(dev, rCCK0_CCA, bit_mask, (u32)priv->initgain_backup.cca);

	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xc50 is %x\n", priv->initgain_backup.xaagccore1);
	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xc58 is %x\n", priv->initgain_backup.xbagccore1);
	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xc60 is %x\n", priv->initgain_backup.xcagccore1);
	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xc68 is %x\n", priv->initgain_backup.xdagccore1);
	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xa0a is %x\n", priv->initgain_backup.cca);
	rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x1);

}


void dm_backup_dynamic_mechanism_state(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	priv->bswitch_fsync  = false;
	priv->bfsync_processing = false;
	dm_bb_initialgain_backup(dev);

}


static void dm_bb_initialgain_backup(struct net_device *dev)
=======
	rtl92e_set_bb_reg(dev, UFWP, bMaskByte1, 0x8);
	rtl92e_set_bb_reg(dev, rOFDM0_XAAGCCore1, bit_mask,
			  (u32)priv->initgain_backup.xaagccore1);
	rtl92e_set_bb_reg(dev, rOFDM0_XBAGCCore1, bit_mask,
			  (u32)priv->initgain_backup.xbagccore1);
	rtl92e_set_bb_reg(dev, rOFDM0_XCAGCCore1, bit_mask,
			  (u32)priv->initgain_backup.xcagccore1);
	rtl92e_set_bb_reg(dev, rOFDM0_XDAGCCore1, bit_mask,
			  (u32)priv->initgain_backup.xdagccore1);
	bit_mask  = bMaskByte2;
	rtl92e_set_bb_reg(dev, rCCK0_CCA, bit_mask,
			  (u32)priv->initgain_backup.cca);

	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xc50 is %x\n",
		 priv->initgain_backup.xaagccore1);
	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xc58 is %x\n",
		 priv->initgain_backup.xbagccore1);
	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xc60 is %x\n",
		 priv->initgain_backup.xcagccore1);
	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xc68 is %x\n",
		 priv->initgain_backup.xdagccore1);
	RT_TRACE(COMP_DIG, "dm_BBInitialGainRestore 0xa0a is %x\n",
		 priv->initgain_backup.cca);
	rtl92e_set_bb_reg(dev, UFWP, bMaskByte1, 0x1);

}

void rtl92e_dm_backup_state(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u32 bit_mask = bMaskByte0;

<<<<<<< HEAD
	if (dm_digtable.dig_algorithm == DIG_ALGO_BY_RSSI)
		return;

	rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x8);
	priv->initgain_backup.xaagccore1 = (u8)rtl8192_QueryBBReg(dev, rOFDM0_XAAGCCore1, bit_mask);
	priv->initgain_backup.xbagccore1 = (u8)rtl8192_QueryBBReg(dev, rOFDM0_XBAGCCore1, bit_mask);
	priv->initgain_backup.xcagccore1 = (u8)rtl8192_QueryBBReg(dev, rOFDM0_XCAGCCore1, bit_mask);
	priv->initgain_backup.xdagccore1 = (u8)rtl8192_QueryBBReg(dev, rOFDM0_XDAGCCore1, bit_mask);
	bit_mask  = bMaskByte2;
	priv->initgain_backup.cca = (u8)rtl8192_QueryBBReg(dev, rCCK0_CCA, bit_mask);

	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xc50 is %x\n", priv->initgain_backup.xaagccore1);
	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xc58 is %x\n", priv->initgain_backup.xbagccore1);
	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xc60 is %x\n", priv->initgain_backup.xcagccore1);
	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xc68 is %x\n", priv->initgain_backup.xdagccore1);
	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xa0a is %x\n", priv->initgain_backup.cca);

}

void dm_change_dynamic_initgain_thresh(struct net_device *dev,
				       u32 dm_type, u32 dm_value)
{
	if (dm_type == DIG_TYPE_THRESH_HIGH) {
		dm_digtable.rssi_high_thresh = dm_value;
	} else if (dm_type == DIG_TYPE_THRESH_LOW) {
		dm_digtable.rssi_low_thresh = dm_value;
	} else if (dm_type == DIG_TYPE_THRESH_HIGHPWR_HIGH) {
		dm_digtable.rssi_high_power_highthresh = dm_value;
	} else if (dm_type == DIG_TYPE_THRESH_HIGHPWR_HIGH) {
		dm_digtable.rssi_high_power_highthresh = dm_value;
	} else if (dm_type == DIG_TYPE_ENABLE) {
		dm_digtable.dig_state		= DM_STA_DIG_MAX;
		dm_digtable.dig_enable_flag	= true;
	} else if (dm_type == DIG_TYPE_DISABLE) {
		dm_digtable.dig_state		= DM_STA_DIG_MAX;
		dm_digtable.dig_enable_flag	= false;
	} else if (dm_type == DIG_TYPE_DBG_MODE) {
		if (dm_value >= DM_DBG_MAX)
			dm_value = DM_DBG_OFF;
		dm_digtable.dbg_mode		= (u8)dm_value;
	} else if (dm_type == DIG_TYPE_RSSI) {
		if (dm_value > 100)
			dm_value = 30;
		dm_digtable.rssi_val			= (long)dm_value;
	} else if (dm_type == DIG_TYPE_ALGORITHM) {
		if (dm_value >= DIG_ALGO_MAX)
			dm_value = DIG_ALGO_BY_FALSE_ALARM;
		if (dm_digtable.dig_algorithm != (u8)dm_value)
			dm_digtable.dig_algorithm_switch = 1;
		dm_digtable.dig_algorithm	= (u8)dm_value;
	} else if (dm_type == DIG_TYPE_BACKOFF) {
		if (dm_value > 30)
			dm_value = 30;
		dm_digtable.backoff_val		= (u8)dm_value;
	} else if (dm_type == DIG_TYPE_RX_GAIN_MIN) {
		if (dm_value == 0)
			dm_value = 0x1;
		dm_digtable.rx_gain_range_min = (u8)dm_value;
	} else if (dm_type == DIG_TYPE_RX_GAIN_MAX) {
		if (dm_value > 0x50)
			dm_value = 0x50;
		dm_digtable.rx_gain_range_max = (u8)dm_value;
	}
}

static void dm_dig_init(struct net_device *dev)
=======
	priv->bswitch_fsync  = false;
	priv->bfsync_processing = false;

	if (dm_digtable.dig_algorithm == DIG_ALGO_BY_RSSI)
		return;

	rtl92e_set_bb_reg(dev, UFWP, bMaskByte1, 0x8);
	priv->initgain_backup.xaagccore1 = (u8)rtl92e_get_bb_reg(dev, rOFDM0_XAAGCCore1, bit_mask);
	priv->initgain_backup.xbagccore1 = (u8)rtl92e_get_bb_reg(dev, rOFDM0_XBAGCCore1, bit_mask);
	priv->initgain_backup.xcagccore1 = (u8)rtl92e_get_bb_reg(dev, rOFDM0_XCAGCCore1, bit_mask);
	priv->initgain_backup.xdagccore1 = (u8)rtl92e_get_bb_reg(dev, rOFDM0_XDAGCCore1, bit_mask);
	bit_mask  = bMaskByte2;
	priv->initgain_backup.cca = (u8)rtl92e_get_bb_reg(dev, rCCK0_CCA, bit_mask);

	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xc50 is %x\n",
		 priv->initgain_backup.xaagccore1);
	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xc58 is %x\n",
		 priv->initgain_backup.xbagccore1);
	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xc60 is %x\n",
		 priv->initgain_backup.xcagccore1);
	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xc68 is %x\n",
		 priv->initgain_backup.xdagccore1);
	RT_TRACE(COMP_DIG, "BBInitialGainBackup 0xa0a is %x\n",
		 priv->initgain_backup.cca);
}

static void _rtl92e_dm_dig_init(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	dm_digtable.dig_enable_flag	= true;
<<<<<<< HEAD
	dm_digtable.Backoff_Enable_Flag = true;

	dm_digtable.dig_algorithm = DIG_ALGO_BY_RSSI;

	dm_digtable.Dig_TwoPort_Algorithm = DIG_TWO_PORT_ALGO_RSSI;
	dm_digtable.Dig_Ext_Port_Stage = DIG_EXT_PORT_STAGE_MAX;
	dm_digtable.dbg_mode = DM_DBG_OFF;
=======

	dm_digtable.dig_algorithm = DIG_ALGO_BY_RSSI;

>>>>>>> v4.9.227
	dm_digtable.dig_algorithm_switch = 0;

	dm_digtable.dig_state		= DM_STA_DIG_MAX;
	dm_digtable.dig_highpwr_state	= DM_STA_DIG_MAX;
<<<<<<< HEAD
	dm_digtable.CurSTAConnectState = dm_digtable.PreSTAConnectState = DIG_STA_DISCONNECT;
	dm_digtable.CurAPConnectState = dm_digtable.PreAPConnectState = DIG_AP_DISCONNECT;
	dm_digtable.initialgain_lowerbound_state = false;
=======
	dm_digtable.CurSTAConnectState = DIG_STA_DISCONNECT;
	dm_digtable.PreSTAConnectState = DIG_STA_DISCONNECT;
>>>>>>> v4.9.227

	dm_digtable.rssi_low_thresh	= DM_DIG_THRESH_LOW;
	dm_digtable.rssi_high_thresh	= DM_DIG_THRESH_HIGH;

<<<<<<< HEAD
	dm_digtable.FALowThresh	= DM_FALSEALARM_THRESH_LOW;
	dm_digtable.FAHighThresh	= DM_FALSEALARM_THRESH_HIGH;

=======
>>>>>>> v4.9.227
	dm_digtable.rssi_high_power_lowthresh = DM_DIG_HIGH_PWR_THRESH_LOW;
	dm_digtable.rssi_high_power_highthresh = DM_DIG_HIGH_PWR_THRESH_HIGH;

	dm_digtable.rssi_val = 50;
	dm_digtable.backoff_val = DM_DIG_BACKOFF;
	dm_digtable.rx_gain_range_max = DM_DIG_MAX;
	if (priv->CustomerID == RT_CID_819x_Netcore)
		dm_digtable.rx_gain_range_min = DM_DIG_MIN_Netcore;
	else
		dm_digtable.rx_gain_range_min = DM_DIG_MIN;
<<<<<<< HEAD

	dm_digtable.BackoffVal_range_max = DM_DIG_BACKOFF_MAX;
	dm_digtable.BackoffVal_range_min = DM_DIG_BACKOFF_MIN;
}

static void dm_ctrl_initgain_byrssi(struct net_device *dev)
=======
}

static void _rtl92e_dm_ctrl_initgain_byrssi(struct net_device *dev)
>>>>>>> v4.9.227
{

	if (dm_digtable.dig_enable_flag == false)
		return;

	if (dm_digtable.dig_algorithm == DIG_ALGO_BY_FALSE_ALARM)
<<<<<<< HEAD
		dm_ctrl_initgain_byrssi_by_fwfalse_alarm(dev);
	else if (dm_digtable.dig_algorithm == DIG_ALGO_BY_RSSI)
		dm_ctrl_initgain_byrssi_by_driverrssi(dev);
=======
		_rtl92e_dm_ctrl_initgain_byrssi_false_alarm(dev);
	else if (dm_digtable.dig_algorithm == DIG_ALGO_BY_RSSI)
		_rtl92e_dm_ctrl_initgain_byrssi_driver(dev);
>>>>>>> v4.9.227
	else
		return;
}

/*-----------------------------------------------------------------------------
 * Function:	dm_CtrlInitGainBeforeConnectByRssiAndFalseAlarm()
 *
 * Overview:	Driver monitor RSSI and False Alarm to change initial gain.
			Only change initial gain during link in progress.
 *
 * Input:		IN	PADAPTER	pAdapter
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	03/04/2009	hpfan	Create Version 0.
 *
 *---------------------------------------------------------------------------*/

<<<<<<< HEAD
static void dm_ctrl_initgain_byrssi_by_driverrssi(
	struct net_device *dev)
=======
static void _rtl92e_dm_ctrl_initgain_byrssi_driver(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u8 i;
	static u8	fw_dig;

	if (dm_digtable.dig_enable_flag == false)
		return;

	if (dm_digtable.dig_algorithm_switch)
		fw_dig = 0;
	if (fw_dig <= 3) {
		for (i = 0; i < 3; i++)
<<<<<<< HEAD
			rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x8);
=======
			rtl92e_set_bb_reg(dev, UFWP, bMaskByte1, 0x8);
>>>>>>> v4.9.227
		fw_dig++;
		dm_digtable.dig_state = DM_STA_DIG_OFF;
	}

	if (priv->rtllib->state == RTLLIB_LINKED)
		dm_digtable.CurSTAConnectState = DIG_STA_CONNECT;
	else
		dm_digtable.CurSTAConnectState = DIG_STA_DISCONNECT;


<<<<<<< HEAD
	if (dm_digtable.dbg_mode == DM_DBG_OFF)
		dm_digtable.rssi_val = priv->undecorated_smoothed_pwdb;
	dm_initial_gain(dev);
	dm_pd_th(dev);
	dm_cs_ratio(dev);
=======
	dm_digtable.rssi_val = priv->undecorated_smoothed_pwdb;
	_rtl92e_dm_initial_gain(dev);
	_rtl92e_dm_pd_th(dev);
	_rtl92e_dm_cs_ratio(dev);
>>>>>>> v4.9.227
	if (dm_digtable.dig_algorithm_switch)
		dm_digtable.dig_algorithm_switch = 0;
	dm_digtable.PreSTAConnectState = dm_digtable.CurSTAConnectState;

}

<<<<<<< HEAD
static void dm_ctrl_initgain_byrssi_by_fwfalse_alarm(
	struct net_device *dev)
=======
static void _rtl92e_dm_ctrl_initgain_byrssi_false_alarm(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static u32 reset_cnt;
	u8 i;

	if (dm_digtable.dig_enable_flag == false)
		return;

	if (dm_digtable.dig_algorithm_switch) {
		dm_digtable.dig_state = DM_STA_DIG_MAX;
		for (i = 0; i < 3; i++)
<<<<<<< HEAD
			rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x1);
=======
			rtl92e_set_bb_reg(dev, UFWP, bMaskByte1, 0x1);
>>>>>>> v4.9.227
		dm_digtable.dig_algorithm_switch = 0;
	}

	if (priv->rtllib->state != RTLLIB_LINKED)
		return;

	if ((priv->undecorated_smoothed_pwdb > dm_digtable.rssi_low_thresh) &&
		(priv->undecorated_smoothed_pwdb < dm_digtable.rssi_high_thresh))
		return;
<<<<<<< HEAD
	if ((priv->undecorated_smoothed_pwdb <= dm_digtable.rssi_low_thresh)) {
		if (dm_digtable.dig_state == DM_STA_DIG_OFF &&
			(priv->reset_count == reset_cnt))
			return;
		else
			reset_cnt = priv->reset_count;
=======
	if (priv->undecorated_smoothed_pwdb <= dm_digtable.rssi_low_thresh) {
		if (dm_digtable.dig_state == DM_STA_DIG_OFF &&
			(priv->reset_count == reset_cnt))
			return;
		reset_cnt = priv->reset_count;
>>>>>>> v4.9.227

		dm_digtable.dig_highpwr_state = DM_STA_DIG_MAX;
		dm_digtable.dig_state = DM_STA_DIG_OFF;

<<<<<<< HEAD
		rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x8);

		write_nic_byte(dev, rOFDM0_XAAGCCore1, 0x17);
		write_nic_byte(dev, rOFDM0_XBAGCCore1, 0x17);
		write_nic_byte(dev, rOFDM0_XCAGCCore1, 0x17);
		write_nic_byte(dev, rOFDM0_XDAGCCore1, 0x17);

		if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
			write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x00);
		else
			write_nic_byte(dev, rOFDM0_RxDetector1, 0x42);

		write_nic_byte(dev, 0xa0a, 0x08);
=======
		rtl92e_set_bb_reg(dev, UFWP, bMaskByte1, 0x8);

		rtl92e_writeb(dev, rOFDM0_XAAGCCore1, 0x17);
		rtl92e_writeb(dev, rOFDM0_XBAGCCore1, 0x17);
		rtl92e_writeb(dev, rOFDM0_XCAGCCore1, 0x17);
		rtl92e_writeb(dev, rOFDM0_XDAGCCore1, 0x17);

		if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
			rtl92e_writeb(dev, (rOFDM0_XATxAFE+3), 0x00);
		else
			rtl92e_writeb(dev, rOFDM0_RxDetector1, 0x42);

		rtl92e_writeb(dev, 0xa0a, 0x08);
>>>>>>> v4.9.227

		return;
	}

<<<<<<< HEAD
	if ((priv->undecorated_smoothed_pwdb >= dm_digtable.rssi_high_thresh)) {
=======
	if (priv->undecorated_smoothed_pwdb >= dm_digtable.rssi_high_thresh) {
>>>>>>> v4.9.227
		u8 reset_flag = 0;

		if (dm_digtable.dig_state == DM_STA_DIG_ON &&
		    (priv->reset_count == reset_cnt)) {
<<<<<<< HEAD
			dm_ctrl_initgain_byrssi_highpwr(dev);
			return;
		} else {
			if (priv->reset_count != reset_cnt)
				reset_flag = 1;

			reset_cnt = priv->reset_count;
		}
=======
			_rtl92e_dm_ctrl_initgain_byrssi_highpwr(dev);
			return;
		}
		if (priv->reset_count != reset_cnt)
			reset_flag = 1;

		reset_cnt = priv->reset_count;
>>>>>>> v4.9.227

		dm_digtable.dig_state = DM_STA_DIG_ON;

		if (reset_flag == 1) {
<<<<<<< HEAD
			write_nic_byte(dev, rOFDM0_XAAGCCore1, 0x2c);
			write_nic_byte(dev, rOFDM0_XBAGCCore1, 0x2c);
			write_nic_byte(dev, rOFDM0_XCAGCCore1, 0x2c);
			write_nic_byte(dev, rOFDM0_XDAGCCore1, 0x2c);
		} else {
			write_nic_byte(dev, rOFDM0_XAAGCCore1, 0x20);
			write_nic_byte(dev, rOFDM0_XBAGCCore1, 0x20);
			write_nic_byte(dev, rOFDM0_XCAGCCore1, 0x20);
			write_nic_byte(dev, rOFDM0_XDAGCCore1, 0x20);
		}

		if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
			write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x20);
		else
			write_nic_byte(dev, rOFDM0_RxDetector1, 0x44);

		write_nic_byte(dev, 0xa0a, 0xcd);

		rtl8192_setBBreg(dev, UFWP, bMaskByte1, 0x1);
	}
	dm_ctrl_initgain_byrssi_highpwr(dev);
}


static void dm_ctrl_initgain_byrssi_highpwr(struct net_device *dev)
=======
			rtl92e_writeb(dev, rOFDM0_XAAGCCore1, 0x2c);
			rtl92e_writeb(dev, rOFDM0_XBAGCCore1, 0x2c);
			rtl92e_writeb(dev, rOFDM0_XCAGCCore1, 0x2c);
			rtl92e_writeb(dev, rOFDM0_XDAGCCore1, 0x2c);
		} else {
			rtl92e_writeb(dev, rOFDM0_XAAGCCore1, 0x20);
			rtl92e_writeb(dev, rOFDM0_XBAGCCore1, 0x20);
			rtl92e_writeb(dev, rOFDM0_XCAGCCore1, 0x20);
			rtl92e_writeb(dev, rOFDM0_XDAGCCore1, 0x20);
		}

		if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
			rtl92e_writeb(dev, (rOFDM0_XATxAFE+3), 0x20);
		else
			rtl92e_writeb(dev, rOFDM0_RxDetector1, 0x44);

		rtl92e_writeb(dev, 0xa0a, 0xcd);

		rtl92e_set_bb_reg(dev, UFWP, bMaskByte1, 0x1);
	}
	_rtl92e_dm_ctrl_initgain_byrssi_highpwr(dev);
}


static void _rtl92e_dm_ctrl_initgain_byrssi_highpwr(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static u32 reset_cnt_highpwr;

<<<<<<< HEAD
	if ((priv->undecorated_smoothed_pwdb > dm_digtable.rssi_high_power_lowthresh) &&
		(priv->undecorated_smoothed_pwdb < dm_digtable.rssi_high_power_highthresh))
		return;

	if (priv->undecorated_smoothed_pwdb >= dm_digtable.rssi_high_power_highthresh) {
		if (dm_digtable.dig_highpwr_state == DM_STA_DIG_ON &&
			(priv->reset_count == reset_cnt_highpwr))
			return;
		else
			dm_digtable.dig_highpwr_state = DM_STA_DIG_ON;

		if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
				write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x10);
		else
			write_nic_byte(dev, rOFDM0_RxDetector1, 0x43);
=======
	if ((priv->undecorated_smoothed_pwdb >
	     dm_digtable.rssi_high_power_lowthresh) &&
	    (priv->undecorated_smoothed_pwdb <
	     dm_digtable.rssi_high_power_highthresh))
		return;

	if (priv->undecorated_smoothed_pwdb >=
	    dm_digtable.rssi_high_power_highthresh) {
		if (dm_digtable.dig_highpwr_state == DM_STA_DIG_ON &&
			(priv->reset_count == reset_cnt_highpwr))
			return;
		dm_digtable.dig_highpwr_state = DM_STA_DIG_ON;

		if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
			rtl92e_writeb(dev, (rOFDM0_XATxAFE+3), 0x10);
		else
			rtl92e_writeb(dev, rOFDM0_RxDetector1, 0x43);
>>>>>>> v4.9.227
	} else {
		if (dm_digtable.dig_highpwr_state == DM_STA_DIG_OFF &&
			(priv->reset_count == reset_cnt_highpwr))
			return;
<<<<<<< HEAD
		else
			dm_digtable.dig_highpwr_state = DM_STA_DIG_OFF;

		if (priv->undecorated_smoothed_pwdb < dm_digtable.rssi_high_power_lowthresh &&
			 priv->undecorated_smoothed_pwdb >= dm_digtable.rssi_high_thresh) {
			if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
				write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x20);
			else
				write_nic_byte(dev, rOFDM0_RxDetector1, 0x44);
=======
		dm_digtable.dig_highpwr_state = DM_STA_DIG_OFF;

		if ((priv->undecorated_smoothed_pwdb <
		     dm_digtable.rssi_high_power_lowthresh) &&
		    (priv->undecorated_smoothed_pwdb >=
		    dm_digtable.rssi_high_thresh)) {
			if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
				rtl92e_writeb(dev, (rOFDM0_XATxAFE+3), 0x20);
			else
				rtl92e_writeb(dev, rOFDM0_RxDetector1, 0x44);
>>>>>>> v4.9.227
		}
	}
	reset_cnt_highpwr = priv->reset_count;
}

<<<<<<< HEAD
static void dm_initial_gain(struct net_device *dev)
=======
static void _rtl92e_dm_initial_gain(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u8 initial_gain = 0;
	static u8 initialized, force_write;
	static u32 reset_cnt;

	if (dm_digtable.dig_algorithm_switch) {
		initialized = 0;
		reset_cnt = 0;
	}

	if (rtllib_act_scanning(priv->rtllib, true) == true) {
		force_write = 1;
		return;
	}

	if (dm_digtable.PreSTAConnectState == dm_digtable.CurSTAConnectState) {
		if (dm_digtable.CurSTAConnectState == DIG_STA_CONNECT) {
<<<<<<< HEAD
			if ((dm_digtable.rssi_val+10-dm_digtable.backoff_val) > dm_digtable.rx_gain_range_max)
				dm_digtable.cur_ig_value = dm_digtable.rx_gain_range_max;
			else if ((dm_digtable.rssi_val+10-dm_digtable.backoff_val) < dm_digtable.rx_gain_range_min)
				dm_digtable.cur_ig_value = dm_digtable.rx_gain_range_min;
			else
				dm_digtable.cur_ig_value = dm_digtable.rssi_val+10-dm_digtable.backoff_val;
=======
			long gain_range = dm_digtable.rssi_val + 10 -
					  dm_digtable.backoff_val;
			gain_range = clamp_t(long, gain_range,
					     dm_digtable.rx_gain_range_min,
					     dm_digtable.rx_gain_range_max);
			dm_digtable.cur_ig_value = gain_range;
>>>>>>> v4.9.227
		} else {
			if (dm_digtable.cur_ig_value == 0)
				dm_digtable.cur_ig_value = priv->DefaultInitialGain[0];
			else
				dm_digtable.cur_ig_value = dm_digtable.pre_ig_value;
		}
	} else {
		dm_digtable.cur_ig_value = priv->DefaultInitialGain[0];
		dm_digtable.pre_ig_value = 0;
	}

	if (priv->reset_count != reset_cnt) {
		force_write = 1;
		reset_cnt = priv->reset_count;
	}

<<<<<<< HEAD
	if (dm_digtable.pre_ig_value != read_nic_byte(dev, rOFDM0_XAAGCCore1))
=======
	if (dm_digtable.pre_ig_value != rtl92e_readb(dev, rOFDM0_XAAGCCore1))
>>>>>>> v4.9.227
		force_write = 1;

	if ((dm_digtable.pre_ig_value != dm_digtable.cur_ig_value)
	    || !initialized || force_write) {
		initial_gain = (u8)dm_digtable.cur_ig_value;
<<<<<<< HEAD
		write_nic_byte(dev, rOFDM0_XAAGCCore1, initial_gain);
		write_nic_byte(dev, rOFDM0_XBAGCCore1, initial_gain);
		write_nic_byte(dev, rOFDM0_XCAGCCore1, initial_gain);
		write_nic_byte(dev, rOFDM0_XDAGCCore1, initial_gain);
=======
		rtl92e_writeb(dev, rOFDM0_XAAGCCore1, initial_gain);
		rtl92e_writeb(dev, rOFDM0_XBAGCCore1, initial_gain);
		rtl92e_writeb(dev, rOFDM0_XCAGCCore1, initial_gain);
		rtl92e_writeb(dev, rOFDM0_XDAGCCore1, initial_gain);
>>>>>>> v4.9.227
		dm_digtable.pre_ig_value = dm_digtable.cur_ig_value;
		initialized = 1;
		force_write = 0;
	}
}

<<<<<<< HEAD
static void dm_pd_th(struct net_device *dev)
=======
static void _rtl92e_dm_pd_th(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static u8 initialized, force_write;
	static u32 reset_cnt;

	if (dm_digtable.dig_algorithm_switch) {
		initialized = 0;
		reset_cnt = 0;
	}

	if (dm_digtable.PreSTAConnectState == dm_digtable.CurSTAConnectState) {
		if (dm_digtable.CurSTAConnectState == DIG_STA_CONNECT) {
<<<<<<< HEAD
			if (dm_digtable.rssi_val >= dm_digtable.rssi_high_power_highthresh)
				dm_digtable.curpd_thstate = DIG_PD_AT_HIGH_POWER;
			else if ((dm_digtable.rssi_val <= dm_digtable.rssi_low_thresh))
				dm_digtable.curpd_thstate = DIG_PD_AT_LOW_POWER;
			else if ((dm_digtable.rssi_val >= dm_digtable.rssi_high_thresh) &&
					(dm_digtable.rssi_val < dm_digtable.rssi_high_power_lowthresh))
				dm_digtable.curpd_thstate = DIG_PD_AT_NORMAL_POWER;
			else
				dm_digtable.curpd_thstate = dm_digtable.prepd_thstate;
=======
			if (dm_digtable.rssi_val >=
			    dm_digtable.rssi_high_power_highthresh)
				dm_digtable.curpd_thstate =
							DIG_PD_AT_HIGH_POWER;
			else if (dm_digtable.rssi_val <=
				 dm_digtable.rssi_low_thresh)
				dm_digtable.curpd_thstate =
							DIG_PD_AT_LOW_POWER;
			else if ((dm_digtable.rssi_val >=
				  dm_digtable.rssi_high_thresh) &&
				 (dm_digtable.rssi_val <
				  dm_digtable.rssi_high_power_lowthresh))
				dm_digtable.curpd_thstate =
							DIG_PD_AT_NORMAL_POWER;
			else
				dm_digtable.curpd_thstate =
						dm_digtable.prepd_thstate;
>>>>>>> v4.9.227
		} else {
			dm_digtable.curpd_thstate = DIG_PD_AT_LOW_POWER;
		}
	} else {
		dm_digtable.curpd_thstate = DIG_PD_AT_LOW_POWER;
	}

	if (priv->reset_count != reset_cnt) {
		force_write = 1;
		reset_cnt = priv->reset_count;
	}

	if ((dm_digtable.prepd_thstate != dm_digtable.curpd_thstate) ||
	    (initialized <= 3) || force_write) {
		if (dm_digtable.curpd_thstate == DIG_PD_AT_LOW_POWER) {
			if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
<<<<<<< HEAD
				write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x00);
			else
				write_nic_byte(dev, rOFDM0_RxDetector1, 0x42);
		} else if (dm_digtable.curpd_thstate == DIG_PD_AT_NORMAL_POWER) {
			if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
				write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x20);
			else
				write_nic_byte(dev, rOFDM0_RxDetector1, 0x44);
		} else if (dm_digtable.curpd_thstate == DIG_PD_AT_HIGH_POWER) {
			if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
				write_nic_byte(dev, (rOFDM0_XATxAFE+3), 0x10);
			else
				write_nic_byte(dev, rOFDM0_RxDetector1, 0x43);
=======
				rtl92e_writeb(dev, (rOFDM0_XATxAFE+3), 0x00);
			else
				rtl92e_writeb(dev, rOFDM0_RxDetector1, 0x42);
		} else if (dm_digtable.curpd_thstate ==
			   DIG_PD_AT_NORMAL_POWER) {
			if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
				rtl92e_writeb(dev, (rOFDM0_XATxAFE+3), 0x20);
			else
				rtl92e_writeb(dev, rOFDM0_RxDetector1, 0x44);
		} else if (dm_digtable.curpd_thstate == DIG_PD_AT_HIGH_POWER) {
			if (priv->CurrentChannelBW != HT_CHANNEL_WIDTH_20)
				rtl92e_writeb(dev, (rOFDM0_XATxAFE+3), 0x10);
			else
				rtl92e_writeb(dev, rOFDM0_RxDetector1, 0x43);
>>>>>>> v4.9.227
		}
		dm_digtable.prepd_thstate = dm_digtable.curpd_thstate;
		if (initialized <= 3)
			initialized++;
		force_write = 0;
	}
}

<<<<<<< HEAD
static	void dm_cs_ratio(struct net_device *dev)
=======
static void _rtl92e_dm_cs_ratio(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	static u8 initialized, force_write;
	static u32 reset_cnt;

	if (dm_digtable.dig_algorithm_switch) {
		initialized = 0;
		reset_cnt = 0;
	}

	if (dm_digtable.PreSTAConnectState == dm_digtable.CurSTAConnectState) {
		if (dm_digtable.CurSTAConnectState == DIG_STA_CONNECT) {
<<<<<<< HEAD
			if ((dm_digtable.rssi_val <= dm_digtable.rssi_low_thresh))
				dm_digtable.curcs_ratio_state = DIG_CS_RATIO_LOWER;
			else if ((dm_digtable.rssi_val >= dm_digtable.rssi_high_thresh))
=======
			if (dm_digtable.rssi_val <= dm_digtable.rssi_low_thresh)
				dm_digtable.curcs_ratio_state = DIG_CS_RATIO_LOWER;
			else if (dm_digtable.rssi_val >= dm_digtable.rssi_high_thresh)
>>>>>>> v4.9.227
				dm_digtable.curcs_ratio_state = DIG_CS_RATIO_HIGHER;
			else
				dm_digtable.curcs_ratio_state = dm_digtable.precs_ratio_state;
		} else {
			dm_digtable.curcs_ratio_state = DIG_CS_RATIO_LOWER;
		}
	} else {
		dm_digtable.curcs_ratio_state = DIG_CS_RATIO_LOWER;
	}

	if (priv->reset_count != reset_cnt) {
		force_write = 1;
		reset_cnt = priv->reset_count;
	}


	if ((dm_digtable.precs_ratio_state != dm_digtable.curcs_ratio_state) ||
	    !initialized || force_write) {
		if (dm_digtable.curcs_ratio_state == DIG_CS_RATIO_LOWER)
<<<<<<< HEAD
			write_nic_byte(dev, 0xa0a, 0x08);
		else if (dm_digtable.curcs_ratio_state == DIG_CS_RATIO_HIGHER)
			write_nic_byte(dev, 0xa0a, 0xcd);
=======
			rtl92e_writeb(dev, 0xa0a, 0x08);
		else if (dm_digtable.curcs_ratio_state == DIG_CS_RATIO_HIGHER)
			rtl92e_writeb(dev, 0xa0a, 0xcd);
>>>>>>> v4.9.227
		dm_digtable.precs_ratio_state = dm_digtable.curcs_ratio_state;
		initialized = 1;
		force_write = 0;
	}
}

<<<<<<< HEAD
void dm_init_edca_turbo(struct net_device *dev)
=======
void rtl92e_dm_init_edca_turbo(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	priv->bcurrent_turbo_EDCA = false;
	priv->rtllib->bis_any_nonbepkts = false;
	priv->bis_cur_rdlstate = false;
}

<<<<<<< HEAD
static void dm_check_edca_turbo(struct net_device *dev)
=======
static void _rtl92e_dm_check_edca_turbo(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rt_hi_throughput *pHTInfo = priv->rtllib->pHTInfo;

	static unsigned long lastTxOkCnt;
	static unsigned long lastRxOkCnt;
	unsigned long curTxOkCnt = 0;
	unsigned long curRxOkCnt = 0;

	if (priv->rtllib->iw_mode == IW_MODE_ADHOC)
		goto dm_CheckEdcaTurbo_EXIT;
	if (priv->rtllib->state != RTLLIB_LINKED)
		goto dm_CheckEdcaTurbo_EXIT;
	if (priv->rtllib->pHTInfo->IOTAction & HT_IOT_ACT_DISABLE_EDCA_TURBO)
		goto dm_CheckEdcaTurbo_EXIT;

	{
		u8 *peername[11] = {
			"unknown", "realtek_90", "realtek_92se", "broadcom",
			"ralink", "atheros", "cisco", "marvell", "92u_softap",
			"self_softap"
		};
		static int wb_tmp;

		if (wb_tmp == 0) {
<<<<<<< HEAD
			printk(KERN_INFO "%s():iot peer is %s, bssid:"
			       " %pM\n", __func__,
			       peername[pHTInfo->IOTPeer],
			       priv->rtllib->current_network.bssid);
=======
			netdev_info(dev,
				    "%s():iot peer is %s, bssid: %pM\n",
				    __func__, peername[pHTInfo->IOTPeer],
				    priv->rtllib->current_network.bssid);
>>>>>>> v4.9.227
			wb_tmp = 1;
		}
	}
	if (!priv->rtllib->bis_any_nonbepkts) {
		curTxOkCnt = priv->stats.txbytesunicast - lastTxOkCnt;
		curRxOkCnt = priv->stats.rxbytesunicast - lastRxOkCnt;
		if (pHTInfo->IOTAction & HT_IOT_ACT_EDCA_BIAS_ON_RX) {
			if (curTxOkCnt > 4*curRxOkCnt) {
				if (priv->bis_cur_rdlstate ||
				    !priv->bcurrent_turbo_EDCA) {
<<<<<<< HEAD
					write_nic_dword(dev, EDCAPARA_BE,
						 edca_setting_UL[pHTInfo->IOTPeer]);
=======
					rtl92e_writel(dev, EDCAPARA_BE,
						      edca_setting_UL[pHTInfo->IOTPeer]);
>>>>>>> v4.9.227
					priv->bis_cur_rdlstate = false;
				}
			} else {
				if (!priv->bis_cur_rdlstate ||
				    !priv->bcurrent_turbo_EDCA) {
					if (priv->rtllib->mode == WIRELESS_MODE_G)
<<<<<<< HEAD
						write_nic_dword(dev, EDCAPARA_BE,
							 edca_setting_DL_GMode[pHTInfo->IOTPeer]);
					else
						write_nic_dword(dev, EDCAPARA_BE,
							 edca_setting_DL[pHTInfo->IOTPeer]);
=======
						rtl92e_writel(dev, EDCAPARA_BE,
							      edca_setting_DL_GMode[pHTInfo->IOTPeer]);
					else
						rtl92e_writel(dev, EDCAPARA_BE,
							      edca_setting_DL[pHTInfo->IOTPeer]);
>>>>>>> v4.9.227
					priv->bis_cur_rdlstate = true;
				}
			}
			priv->bcurrent_turbo_EDCA = true;
		} else {
			if (curRxOkCnt > 4*curTxOkCnt) {
<<<<<<< HEAD
				if (!priv->bis_cur_rdlstate || !priv->bcurrent_turbo_EDCA) {
					if (priv->rtllib->mode == WIRELESS_MODE_G)
						write_nic_dword(dev, EDCAPARA_BE,
							 edca_setting_DL_GMode[pHTInfo->IOTPeer]);
					else
						write_nic_dword(dev, EDCAPARA_BE,
							 edca_setting_DL[pHTInfo->IOTPeer]);
=======
				if (!priv->bis_cur_rdlstate ||
				    !priv->bcurrent_turbo_EDCA) {
					if (priv->rtllib->mode == WIRELESS_MODE_G)
						rtl92e_writel(dev, EDCAPARA_BE,
							      edca_setting_DL_GMode[pHTInfo->IOTPeer]);
					else
						rtl92e_writel(dev, EDCAPARA_BE,
							      edca_setting_DL[pHTInfo->IOTPeer]);
>>>>>>> v4.9.227
					priv->bis_cur_rdlstate = true;
				}
			} else {
				if (priv->bis_cur_rdlstate ||
				    !priv->bcurrent_turbo_EDCA) {
<<<<<<< HEAD
					write_nic_dword(dev, EDCAPARA_BE,
							edca_setting_UL[pHTInfo->IOTPeer]);
=======
					rtl92e_writel(dev, EDCAPARA_BE,
						      edca_setting_UL[pHTInfo->IOTPeer]);
>>>>>>> v4.9.227
					priv->bis_cur_rdlstate = false;
				}

			}

			priv->bcurrent_turbo_EDCA = true;
		}
	} else {
		 if (priv->bcurrent_turbo_EDCA) {
			u8 tmp = AC0_BE;

<<<<<<< HEAD
			priv->rtllib->SetHwRegHandler(dev, HW_VAR_AC_PARAM, (u8 *)(&tmp));
=======
			priv->rtllib->SetHwRegHandler(dev, HW_VAR_AC_PARAM,
						      (u8 *)(&tmp));
>>>>>>> v4.9.227
			priv->bcurrent_turbo_EDCA = false;
		}
	}


dm_CheckEdcaTurbo_EXIT:
	priv->rtllib->bis_any_nonbepkts = false;
	lastTxOkCnt = priv->stats.txbytesunicast;
	lastRxOkCnt = priv->stats.rxbytesunicast;
}

<<<<<<< HEAD
static void dm_init_ctstoself(struct net_device *dev)
=======
static void _rtl92e_dm_init_cts_to_self(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv((struct net_device *)dev);

	priv->rtllib->bCTSToSelfEnable = true;
<<<<<<< HEAD
	priv->rtllib->CTSToSelfTH = CTSToSelfTHVal;
}

static void dm_ctstoself(struct net_device *dev)
=======
}

static void _rtl92e_dm_cts_to_self(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv((struct net_device *)dev);
	struct rt_hi_throughput *pHTInfo = priv->rtllib->pHTInfo;
	static unsigned long lastTxOkCnt;
	static unsigned long lastRxOkCnt;
	unsigned long curTxOkCnt = 0;
	unsigned long curRxOkCnt = 0;

	if (priv->rtllib->bCTSToSelfEnable != true) {
		pHTInfo->IOTAction &= ~HT_IOT_ACT_FORCED_CTS2SELF;
		return;
	}
	if (pHTInfo->IOTPeer == HT_IOT_PEER_BROADCOM) {
		curTxOkCnt = priv->stats.txbytesunicast - lastTxOkCnt;
		curRxOkCnt = priv->stats.rxbytesunicast - lastRxOkCnt;
		if (curRxOkCnt > 4*curTxOkCnt)
			pHTInfo->IOTAction &= ~HT_IOT_ACT_FORCED_CTS2SELF;
		else
			pHTInfo->IOTAction |= HT_IOT_ACT_FORCED_CTS2SELF;

		lastTxOkCnt = priv->stats.txbytesunicast;
		lastRxOkCnt = priv->stats.rxbytesunicast;
	}
}


<<<<<<< HEAD
static	void dm_Init_WA_Broadcom_IOT(struct net_device *dev)
=======
static void _rtl92e_dm_init_wa_broadcom_iot(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv((struct net_device *)dev);
	struct rt_hi_throughput *pHTInfo = priv->rtllib->pHTInfo;

	pHTInfo->bWAIotBroadcom = false;
	pHTInfo->WAIotTH = WAIotTHVal;
}

<<<<<<< HEAD
static	void	dm_check_pbc_gpio(struct net_device *dev)
{
}

void dm_CheckRfCtrlGPIO(void *data)
=======
static void _rtl92e_dm_check_rf_ctrl_gpio(void *data)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = container_of_dwork_rsl(data,
				  struct r8192_priv, gpio_change_rf_wq);
	struct net_device *dev = priv->rtllib->dev;
	u8 tmp1byte;
	enum rt_rf_power_state eRfPowerStateToSet;
	bool bActuallySet = false;
	char *argv[3];
	static char *RadioPowerPath = "/etc/acpi/events/RadioPower.sh";
<<<<<<< HEAD
	static char *envp[] = {"HOME=/", "TERM=linux", "PATH=/usr/bin:/bin", NULL};
=======
	static char *envp[] = {"HOME=/", "TERM=linux", "PATH=/usr/bin:/bin",
			       NULL};
>>>>>>> v4.9.227

	bActuallySet = false;

	if ((priv->up_first_time == 1) || (priv->being_init_adapter))
		return;

	if (priv->bfirst_after_down) {
		priv->bfirst_after_down = true;
		return;
	}

<<<<<<< HEAD
	tmp1byte = read_nic_byte(dev, GPI);
=======
	tmp1byte = rtl92e_readb(dev, GPI);
>>>>>>> v4.9.227

	eRfPowerStateToSet = (tmp1byte&BIT1) ?  eRfOn : eRfOff;

	if (priv->bHwRadioOff && (eRfPowerStateToSet == eRfOn)) {
		RT_TRACE(COMP_RF, "gpiochangeRF  - HW Radio ON\n");
		netdev_info(dev, "gpiochangeRF  - HW Radio ON\n");
		priv->bHwRadioOff = false;
		bActuallySet = true;
	} else if (!priv->bHwRadioOff && (eRfPowerStateToSet == eRfOff)) {
		RT_TRACE(COMP_RF, "gpiochangeRF  - HW Radio OFF\n");
		netdev_info(dev, "gpiochangeRF  - HW Radio OFF\n");
		priv->bHwRadioOff = true;
		bActuallySet = true;
	}

	if (bActuallySet) {
		mdelay(1000);
		priv->bHwRfOffAction = 1;
<<<<<<< HEAD
		MgntActSet_RF_State(dev, eRfPowerStateToSet, RF_CHANGE_BY_HW, true);
=======
		rtl92e_set_rf_state(dev, eRfPowerStateToSet, RF_CHANGE_BY_HW);
>>>>>>> v4.9.227
		if (priv->bHwRadioOff)
			argv[1] = "RFOFF";
		else
			argv[1] = "RFON";

		argv[0] = RadioPowerPath;
		argv[2] = NULL;
		call_usermodehelper(RadioPowerPath, argv, envp, UMH_WAIT_PROC);
	}
}

<<<<<<< HEAD
void	dm_rf_pathcheck_workitemcallback(void *data)
=======
void rtl92e_dm_rf_pathcheck_wq(void *data)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = container_of_dwork_rsl(data,
				  struct r8192_priv,
				  rfpath_check_wq);
	struct net_device *dev = priv->rtllib->dev;
<<<<<<< HEAD
	u8 rfpath = 0, i;

	rfpath = read_nic_byte(dev, 0xc04);
=======
	u8 rfpath, i;

	rfpath = rtl92e_readb(dev, 0xc04);
>>>>>>> v4.9.227

	for (i = 0; i < RF90_PATH_MAX; i++) {
		if (rfpath & (0x01<<i))
			priv->brfpath_rxenable[i] = true;
		else
			priv->brfpath_rxenable[i] = false;
	}
	if (!DM_RxPathSelTable.Enable)
		return;

<<<<<<< HEAD
	dm_rxpath_sel_byrssi(dev);
}

static void dm_init_rxpath_selection(struct net_device *dev)
=======
	_rtl92e_dm_rx_path_sel_byrssi(dev);
}

static void _rtl92e_dm_init_rx_path_selection(struct net_device *dev)
>>>>>>> v4.9.227
{
	u8 i;
	struct r8192_priv *priv = rtllib_priv(dev);

	DM_RxPathSelTable.Enable = 1;
	DM_RxPathSelTable.SS_TH_low = RxPathSelection_SS_TH_low;
	DM_RxPathSelTable.diff_TH = RxPathSelection_diff_TH;
	if (priv->CustomerID == RT_CID_819x_Netcore)
		DM_RxPathSelTable.cck_method = CCK_Rx_Version_2;
	else
		DM_RxPathSelTable.cck_method = CCK_Rx_Version_1;
<<<<<<< HEAD
	DM_RxPathSelTable.DbgMode = DM_DBG_OFF;
=======
>>>>>>> v4.9.227
	DM_RxPathSelTable.disabledRF = 0;
	for (i = 0; i < 4; i++) {
		DM_RxPathSelTable.rf_rssi[i] = 50;
		DM_RxPathSelTable.cck_pwdb_sta[i] = -64;
		DM_RxPathSelTable.rf_enable_rssi_th[i] = 100;
	}
}

#define PWDB_IN_RANGE	((cur_cck_pwdb < tmp_cck_max_pwdb) &&	\
			(cur_cck_pwdb > tmp_cck_sec_pwdb))

<<<<<<< HEAD
static void dm_rxpath_sel_byrssi(struct net_device *dev)
=======
static void _rtl92e_dm_rx_path_sel_byrssi(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u8 i, max_rssi_index = 0, min_rssi_index = 0;
	u8 sec_rssi_index = 0, rf_num = 0;
	u8 tmp_max_rssi = 0, tmp_min_rssi = 0, tmp_sec_rssi = 0;
	u8 cck_default_Rx = 0x2;
	u8 cck_optional_Rx = 0x3;
	long tmp_cck_max_pwdb = 0, tmp_cck_min_pwdb = 0, tmp_cck_sec_pwdb = 0;
	u8 cck_rx_ver2_max_index = 0, cck_rx_ver2_min_index = 0;
	u8 cck_rx_ver2_sec_index = 0;
	u8 cur_rf_rssi;
	long cur_cck_pwdb;
	static u8 disabled_rf_cnt, cck_Rx_Path_initialized;
	u8 update_cck_rx_path;

	if (priv->rf_type != RF_2T4R)
		return;

	if (!cck_Rx_Path_initialized) {
<<<<<<< HEAD
		DM_RxPathSelTable.cck_Rx_path = (read_nic_byte(dev, 0xa07)&0xf);
=======
		DM_RxPathSelTable.cck_Rx_path = (rtl92e_readb(dev, 0xa07)&0xf);
>>>>>>> v4.9.227
		cck_Rx_Path_initialized = 1;
	}

	DM_RxPathSelTable.disabledRF = 0xf;
<<<<<<< HEAD
	DM_RxPathSelTable.disabledRF &= ~(read_nic_byte(dev, 0xc04));
=======
	DM_RxPathSelTable.disabledRF &= ~(rtl92e_readb(dev, 0xc04));
>>>>>>> v4.9.227

	if (priv->rtllib->mode == WIRELESS_MODE_B)
		DM_RxPathSelTable.cck_method = CCK_Rx_Version_2;

	for (i = 0; i < RF90_PATH_MAX; i++) {
<<<<<<< HEAD
		if (!DM_RxPathSelTable.DbgMode)
			DM_RxPathSelTable.rf_rssi[i] = priv->stats.rx_rssi_percentage[i];
=======
		DM_RxPathSelTable.rf_rssi[i] = priv->stats.rx_rssi_percentage[i];
>>>>>>> v4.9.227

		if (priv->brfpath_rxenable[i]) {
			rf_num++;
			cur_rf_rssi = DM_RxPathSelTable.rf_rssi[i];

			if (rf_num == 1) {
				max_rssi_index = min_rssi_index = sec_rssi_index = i;
				tmp_max_rssi = tmp_min_rssi = tmp_sec_rssi = cur_rf_rssi;
			} else if (rf_num == 2) {
				if (cur_rf_rssi >= tmp_max_rssi) {
					tmp_max_rssi = cur_rf_rssi;
					max_rssi_index = i;
				} else {
					tmp_sec_rssi = tmp_min_rssi = cur_rf_rssi;
					sec_rssi_index = min_rssi_index = i;
				}
			} else {
				if (cur_rf_rssi > tmp_max_rssi) {
					tmp_sec_rssi = tmp_max_rssi;
					sec_rssi_index = max_rssi_index;
					tmp_max_rssi = cur_rf_rssi;
					max_rssi_index = i;
				} else if (cur_rf_rssi == tmp_max_rssi) {
					tmp_sec_rssi = cur_rf_rssi;
					sec_rssi_index = i;
				} else if ((cur_rf_rssi < tmp_max_rssi) &&
					   (cur_rf_rssi > tmp_sec_rssi)) {
					tmp_sec_rssi = cur_rf_rssi;
					sec_rssi_index = i;
				} else if (cur_rf_rssi == tmp_sec_rssi) {
					if (tmp_sec_rssi == tmp_min_rssi) {
						tmp_sec_rssi = cur_rf_rssi;
						sec_rssi_index = i;
					}
				} else if ((cur_rf_rssi < tmp_sec_rssi) &&
					   (cur_rf_rssi > tmp_min_rssi)) {
					;
				} else if (cur_rf_rssi == tmp_min_rssi) {
					if (tmp_sec_rssi == tmp_min_rssi) {
						tmp_min_rssi = cur_rf_rssi;
						min_rssi_index = i;
					}
				} else if (cur_rf_rssi < tmp_min_rssi) {
					tmp_min_rssi = cur_rf_rssi;
					min_rssi_index = i;
				}
			}
		}
	}

	rf_num = 0;
	if (DM_RxPathSelTable.cck_method == CCK_Rx_Version_2) {
		for (i = 0; i < RF90_PATH_MAX; i++) {
			if (priv->brfpath_rxenable[i]) {
				rf_num++;
				cur_cck_pwdb =
					 DM_RxPathSelTable.cck_pwdb_sta[i];

				if (rf_num == 1) {
					cck_rx_ver2_max_index = i;
					cck_rx_ver2_min_index = i;
					cck_rx_ver2_sec_index = i;
					tmp_cck_max_pwdb = cur_cck_pwdb;
					tmp_cck_min_pwdb = cur_cck_pwdb;
					tmp_cck_sec_pwdb = cur_cck_pwdb;
				} else if (rf_num == 2) {
					if (cur_cck_pwdb >= tmp_cck_max_pwdb) {
						tmp_cck_max_pwdb = cur_cck_pwdb;
						cck_rx_ver2_max_index = i;
					} else {
						tmp_cck_sec_pwdb = cur_cck_pwdb;
						tmp_cck_min_pwdb = cur_cck_pwdb;
						cck_rx_ver2_sec_index = i;
						cck_rx_ver2_min_index = i;
					}
				} else {
					if (cur_cck_pwdb > tmp_cck_max_pwdb) {
						tmp_cck_sec_pwdb =
							 tmp_cck_max_pwdb;
						cck_rx_ver2_sec_index =
							 cck_rx_ver2_max_index;
						tmp_cck_max_pwdb = cur_cck_pwdb;
						cck_rx_ver2_max_index = i;
					} else if (cur_cck_pwdb ==
						   tmp_cck_max_pwdb) {
						tmp_cck_sec_pwdb = cur_cck_pwdb;
						cck_rx_ver2_sec_index = i;
					} else if (PWDB_IN_RANGE) {
						tmp_cck_sec_pwdb = cur_cck_pwdb;
						cck_rx_ver2_sec_index = i;
					} else if (cur_cck_pwdb ==
						   tmp_cck_sec_pwdb) {
						if (tmp_cck_sec_pwdb ==
						    tmp_cck_min_pwdb) {
							tmp_cck_sec_pwdb =
								 cur_cck_pwdb;
							cck_rx_ver2_sec_index =
								 i;
						}
					} else if ((cur_cck_pwdb < tmp_cck_sec_pwdb) &&
						   (cur_cck_pwdb > tmp_cck_min_pwdb)) {
						;
					} else if (cur_cck_pwdb == tmp_cck_min_pwdb) {
						if (tmp_cck_sec_pwdb == tmp_cck_min_pwdb) {
							tmp_cck_min_pwdb = cur_cck_pwdb;
							cck_rx_ver2_min_index = i;
						}
					} else if (cur_cck_pwdb < tmp_cck_min_pwdb) {
						tmp_cck_min_pwdb = cur_cck_pwdb;
						cck_rx_ver2_min_index = i;
					}
				}

			}
		}
	}

	update_cck_rx_path = 0;
	if (DM_RxPathSelTable.cck_method == CCK_Rx_Version_2) {
		cck_default_Rx = cck_rx_ver2_max_index;
		cck_optional_Rx = cck_rx_ver2_sec_index;
		if (tmp_cck_max_pwdb != -64)
			update_cck_rx_path = 1;
	}

	if (tmp_min_rssi < DM_RxPathSelTable.SS_TH_low && disabled_rf_cnt < 2) {
		if ((tmp_max_rssi - tmp_min_rssi) >=
		     DM_RxPathSelTable.diff_TH) {
			DM_RxPathSelTable.rf_enable_rssi_th[min_rssi_index] =
				 tmp_max_rssi+5;
<<<<<<< HEAD
			rtl8192_setBBreg(dev, rOFDM0_TRxPathEnable,
				 0x1<<min_rssi_index, 0x0);
			rtl8192_setBBreg(dev, rOFDM1_TRxPathEnable,
				 0x1<<min_rssi_index, 0x0);
=======
			rtl92e_set_bb_reg(dev, rOFDM0_TRxPathEnable,
					  0x1<<min_rssi_index, 0x0);
			rtl92e_set_bb_reg(dev, rOFDM1_TRxPathEnable,
					  0x1<<min_rssi_index, 0x0);
>>>>>>> v4.9.227
			disabled_rf_cnt++;
		}
		if (DM_RxPathSelTable.cck_method == CCK_Rx_Version_1) {
			cck_default_Rx = max_rssi_index;
			cck_optional_Rx = sec_rssi_index;
			if (tmp_max_rssi)
				update_cck_rx_path = 1;
		}
	}

	if (update_cck_rx_path) {
		DM_RxPathSelTable.cck_Rx_path = (cck_default_Rx<<2) |
						(cck_optional_Rx);
<<<<<<< HEAD
		rtl8192_setBBreg(dev, rCCK0_AFESetting, 0x0f000000,
				 DM_RxPathSelTable.cck_Rx_path);
=======
		rtl92e_set_bb_reg(dev, rCCK0_AFESetting, 0x0f000000,
				  DM_RxPathSelTable.cck_Rx_path);
>>>>>>> v4.9.227
	}

	if (DM_RxPathSelTable.disabledRF) {
		for (i = 0; i < 4; i++) {
			if ((DM_RxPathSelTable.disabledRF>>i) & 0x1) {
				if (tmp_max_rssi >=
				    DM_RxPathSelTable.rf_enable_rssi_th[i]) {
<<<<<<< HEAD
					rtl8192_setBBreg(dev,
						 rOFDM0_TRxPathEnable, 0x1 << i,
						 0x1);
					rtl8192_setBBreg(dev,
						 rOFDM1_TRxPathEnable,
						 0x1 << i, 0x1);
=======
					rtl92e_set_bb_reg(dev,
							  rOFDM0_TRxPathEnable,
							  0x1 << i, 0x1);
					rtl92e_set_bb_reg(dev,
							  rOFDM1_TRxPathEnable,
							  0x1 << i, 0x1);
>>>>>>> v4.9.227
					DM_RxPathSelTable.rf_enable_rssi_th[i]
						 = 100;
					disabled_rf_cnt--;
				}
			}
		}
	}
}

<<<<<<< HEAD
static	void	dm_check_rx_path_selection(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	queue_delayed_work_rsl(priv->priv_wq, &priv->rfpath_check_wq, 0);
}


static void dm_init_fsync(struct net_device *dev)
=======
static void _rtl92e_dm_check_rx_path_selection(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	schedule_delayed_work(&priv->rfpath_check_wq, 0);
}


static void _rtl92e_dm_init_fsync(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	priv->rtllib->fsync_time_interval = 500;
	priv->rtllib->fsync_rate_bitmap = 0x0f000800;
	priv->rtllib->fsync_rssi_threshold = 30;
	priv->rtllib->bfsync_enable = false;
	priv->rtllib->fsync_multiple_timeinterval = 3;
	priv->rtllib->fsync_firstdiff_ratethreshold = 100;
	priv->rtllib->fsync_seconddiff_ratethreshold = 200;
	priv->rtllib->fsync_state = Default_Fsync;
	priv->framesyncMonitor = 1;

<<<<<<< HEAD
	init_timer(&priv->fsync_timer);
	setup_timer(&priv->fsync_timer, dm_fsync_timer_callback,
		   (unsigned long) dev);
}


static void dm_deInit_fsync(struct net_device *dev)
=======
	setup_timer(&priv->fsync_timer, _rtl92e_dm_fsync_timer_callback,
		    (unsigned long)dev);
}


static void _rtl92e_dm_deinit_fsync(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	del_timer_sync(&priv->fsync_timer);
}

<<<<<<< HEAD
void dm_fsync_timer_callback(unsigned long data)
=======
static void _rtl92e_dm_fsync_timer_callback(unsigned long data)
>>>>>>> v4.9.227
{
	struct net_device *dev = (struct net_device *)data;
	struct r8192_priv *priv = rtllib_priv((struct net_device *)data);
	u32 rate_index, rate_count = 0, rate_count_diff = 0;
	bool		bSwitchFromCountDiff = false;
	bool		bDoubleTimeInterval = false;

	if (priv->rtllib->state == RTLLIB_LINKED &&
	    priv->rtllib->bfsync_enable &&
	    (priv->rtllib->pHTInfo->IOTAction & HT_IOT_ACT_CDD_FSYNC)) {
		u32 rate_bitmap;

		for (rate_index = 0; rate_index <= 27; rate_index++) {
			rate_bitmap  = 1 << rate_index;
			if (priv->rtllib->fsync_rate_bitmap &  rate_bitmap)
				rate_count +=
				   priv->stats.received_rate_histogram[1]
				   [rate_index];
		}

		if (rate_count < priv->rate_record)
			rate_count_diff = 0xffffffff - rate_count +
					  priv->rate_record;
		else
			rate_count_diff = rate_count - priv->rate_record;
		if (rate_count_diff < priv->rateCountDiffRecord) {

			u32 DiffNum = priv->rateCountDiffRecord -
				      rate_count_diff;
			if (DiffNum >=
			    priv->rtllib->fsync_seconddiff_ratethreshold)
				priv->ContinueDiffCount++;
			else
				priv->ContinueDiffCount = 0;

			if (priv->ContinueDiffCount >= 2) {
				bSwitchFromCountDiff = true;
				priv->ContinueDiffCount = 0;
			}
		} else {
			priv->ContinueDiffCount = 0;
		}

		if (rate_count_diff <=
		    priv->rtllib->fsync_firstdiff_ratethreshold) {
			bSwitchFromCountDiff = true;
			priv->ContinueDiffCount = 0;
		}
		priv->rate_record = rate_count;
		priv->rateCountDiffRecord = rate_count_diff;
<<<<<<< HEAD
		RT_TRACE(COMP_HALDM, "rateRecord %d rateCount %d, rate"
			 "Countdiff %d bSwitchFsync %d\n", priv->rate_record,
			 rate_count, rate_count_diff, priv->bswitch_fsync);
=======
		RT_TRACE(COMP_HALDM,
			 "rateRecord %d rateCount %d, rateCountdiff %d bSwitchFsync %d\n",
			 priv->rate_record, rate_count, rate_count_diff,
			 priv->bswitch_fsync);
>>>>>>> v4.9.227
		if (priv->undecorated_smoothed_pwdb >
		    priv->rtllib->fsync_rssi_threshold &&
		    bSwitchFromCountDiff) {
			bDoubleTimeInterval = true;
			priv->bswitch_fsync = !priv->bswitch_fsync;
			if (priv->bswitch_fsync) {
<<<<<<< HEAD
				write_nic_byte(dev, 0xC36, 0x1c);
				write_nic_byte(dev, 0xC3e, 0x90);
			} else {
				write_nic_byte(dev, 0xC36, 0x5c);
				write_nic_byte(dev, 0xC3e, 0x96);
=======
				rtl92e_writeb(dev, 0xC36, 0x1c);
				rtl92e_writeb(dev, 0xC3e, 0x90);
			} else {
				rtl92e_writeb(dev, 0xC36, 0x5c);
				rtl92e_writeb(dev, 0xC3e, 0x96);
>>>>>>> v4.9.227
			}
		} else if (priv->undecorated_smoothed_pwdb <=
			   priv->rtllib->fsync_rssi_threshold) {
			if (priv->bswitch_fsync) {
				priv->bswitch_fsync  = false;
<<<<<<< HEAD
				write_nic_byte(dev, 0xC36, 0x5c);
				write_nic_byte(dev, 0xC3e, 0x96);
=======
				rtl92e_writeb(dev, 0xC36, 0x5c);
				rtl92e_writeb(dev, 0xC3e, 0x96);
>>>>>>> v4.9.227
			}
		}
		if (bDoubleTimeInterval) {
			if (timer_pending(&priv->fsync_timer))
				del_timer_sync(&priv->fsync_timer);
			priv->fsync_timer.expires = jiffies +
<<<<<<< HEAD
				 MSECS(priv->rtllib->fsync_time_interval *
=======
				 msecs_to_jiffies(priv->rtllib->fsync_time_interval *
>>>>>>> v4.9.227
				 priv->rtllib->fsync_multiple_timeinterval);
			add_timer(&priv->fsync_timer);
		} else {
			if (timer_pending(&priv->fsync_timer))
				del_timer_sync(&priv->fsync_timer);
			priv->fsync_timer.expires = jiffies +
<<<<<<< HEAD
				 MSECS(priv->rtllib->fsync_time_interval);
=======
				 msecs_to_jiffies(priv->rtllib->fsync_time_interval);
>>>>>>> v4.9.227
			add_timer(&priv->fsync_timer);
		}
	} else {
		if (priv->bswitch_fsync) {
			priv->bswitch_fsync  = false;
<<<<<<< HEAD
			write_nic_byte(dev, 0xC36, 0x5c);
			write_nic_byte(dev, 0xC3e, 0x96);
		}
		priv->ContinueDiffCount = 0;
		write_nic_dword(dev, rOFDM0_RxDetector2, 0x465c52cd);
	}
	RT_TRACE(COMP_HALDM, "ContinueDiffCount %d\n", priv->ContinueDiffCount);
	RT_TRACE(COMP_HALDM, "rateRecord %d rateCount %d, rateCountdiff %d "
		 "bSwitchFsync %d\n", priv->rate_record, rate_count,
		 rate_count_diff, priv->bswitch_fsync);
}

static void dm_StartHWFsync(struct net_device *dev)
=======
			rtl92e_writeb(dev, 0xC36, 0x5c);
			rtl92e_writeb(dev, 0xC3e, 0x96);
		}
		priv->ContinueDiffCount = 0;
		rtl92e_writel(dev, rOFDM0_RxDetector2, 0x465c52cd);
	}
	RT_TRACE(COMP_HALDM, "ContinueDiffCount %d\n", priv->ContinueDiffCount);
	RT_TRACE(COMP_HALDM,
		 "rateRecord %d rateCount %d, rateCountdiff %d bSwitchFsync %d\n",
		 priv->rate_record, rate_count, rate_count_diff,
		 priv->bswitch_fsync);
}

static void _rtl92e_dm_start_hw_fsync(struct net_device *dev)
>>>>>>> v4.9.227
{
	u8 rf_timing = 0x77;
	struct r8192_priv *priv = rtllib_priv(dev);

	RT_TRACE(COMP_HALDM, "%s\n", __func__);
<<<<<<< HEAD
	write_nic_dword(dev, rOFDM0_RxDetector2, 0x465c12cf);
	priv->rtllib->SetHwRegHandler(dev, HW_VAR_RF_TIMING,
				      (u8 *)(&rf_timing));
	write_nic_byte(dev, 0xc3b, 0x41);
}

static void dm_EndHWFsync(struct net_device *dev)
=======
	rtl92e_writel(dev, rOFDM0_RxDetector2, 0x465c12cf);
	priv->rtllib->SetHwRegHandler(dev, HW_VAR_RF_TIMING,
				      (u8 *)(&rf_timing));
	rtl92e_writeb(dev, 0xc3b, 0x41);
}

static void _rtl92e_dm_end_hw_fsync(struct net_device *dev)
>>>>>>> v4.9.227
{
	u8 rf_timing = 0xaa;
	struct r8192_priv *priv = rtllib_priv(dev);

	RT_TRACE(COMP_HALDM, "%s\n", __func__);
<<<<<<< HEAD
	write_nic_dword(dev, rOFDM0_RxDetector2, 0x465c52cd);
	priv->rtllib->SetHwRegHandler(dev, HW_VAR_RF_TIMING, (u8 *)
				     (&rf_timing));
	write_nic_byte(dev, 0xc3b, 0x49);
}

static void dm_EndSWFsync(struct net_device *dev)
=======
	rtl92e_writel(dev, rOFDM0_RxDetector2, 0x465c52cd);
	priv->rtllib->SetHwRegHandler(dev, HW_VAR_RF_TIMING, (u8 *)
				     (&rf_timing));
	rtl92e_writeb(dev, 0xc3b, 0x49);
}

static void _rtl92e_dm_end_sw_fsync(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	RT_TRACE(COMP_HALDM, "%s\n", __func__);
	del_timer_sync(&(priv->fsync_timer));

	if (priv->bswitch_fsync) {
		priv->bswitch_fsync  = false;

<<<<<<< HEAD
		write_nic_byte(dev, 0xC36, 0x5c);

		write_nic_byte(dev, 0xC3e, 0x96);
	}

	priv->ContinueDiffCount = 0;
	write_nic_dword(dev, rOFDM0_RxDetector2, 0x465c52cd);
}

static void dm_StartSWFsync(struct net_device *dev)
=======
		rtl92e_writeb(dev, 0xC36, 0x5c);

		rtl92e_writeb(dev, 0xC3e, 0x96);
	}

	priv->ContinueDiffCount = 0;
	rtl92e_writel(dev, rOFDM0_RxDetector2, 0x465c52cd);
}

static void _rtl92e_dm_start_sw_fsync(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	u32			rateIndex;
	u32			rateBitmap;

	RT_TRACE(COMP_HALDM, "%s\n", __func__);
	priv->rate_record = 0;
	priv->ContinueDiffCount = 0;
	priv->rateCountDiffRecord = 0;
	priv->bswitch_fsync  = false;

	if (priv->rtllib->mode == WIRELESS_MODE_N_24G) {
		priv->rtllib->fsync_firstdiff_ratethreshold = 600;
		priv->rtllib->fsync_seconddiff_ratethreshold = 0xffff;
	} else {
		priv->rtllib->fsync_firstdiff_ratethreshold = 200;
		priv->rtllib->fsync_seconddiff_ratethreshold = 200;
	}
	for (rateIndex = 0; rateIndex <= 27; rateIndex++) {
		rateBitmap  = 1 << rateIndex;
		if (priv->rtllib->fsync_rate_bitmap & rateBitmap)
			priv->rate_record +=
				 priv->stats.received_rate_histogram[1]
				[rateIndex];
	}
	if (timer_pending(&priv->fsync_timer))
		del_timer_sync(&priv->fsync_timer);
	priv->fsync_timer.expires = jiffies +
<<<<<<< HEAD
				    MSECS(priv->rtllib->fsync_time_interval);
	add_timer(&priv->fsync_timer);

	write_nic_dword(dev, rOFDM0_RxDetector2, 0x465c12cd);

}

void dm_check_fsync(struct net_device *dev)
=======
				    msecs_to_jiffies(priv->rtllib->fsync_time_interval);
	add_timer(&priv->fsync_timer);

	rtl92e_writel(dev, rOFDM0_RxDetector2, 0x465c12cd);

}

static void _rtl92e_dm_check_fsync(struct net_device *dev)
>>>>>>> v4.9.227
{
#define	RegC38_Default			0
#define	RegC38_NonFsync_Other_AP	1
#define	RegC38_Fsync_AP_BCM		2
	struct r8192_priv *priv = rtllib_priv(dev);
	static u8 reg_c38_State = RegC38_Default;
	static u32 reset_cnt;

<<<<<<< HEAD
	RT_TRACE(COMP_HALDM, "RSSI %d TimeInterval %d MultipleTimeInterval "
		 "%d\n", priv->rtllib->fsync_rssi_threshold,
		 priv->rtllib->fsync_time_interval,
		 priv->rtllib->fsync_multiple_timeinterval);
	RT_TRACE(COMP_HALDM, "RateBitmap 0x%x FirstDiffRateThreshold %d Second"
		 "DiffRateThreshold %d\n", priv->rtllib->fsync_rate_bitmap,
=======
	RT_TRACE(COMP_HALDM,
		 "RSSI %d TimeInterval %d MultipleTimeInterval %d\n",
		 priv->rtllib->fsync_rssi_threshold,
		 priv->rtllib->fsync_time_interval,
		 priv->rtllib->fsync_multiple_timeinterval);
	RT_TRACE(COMP_HALDM,
		 "RateBitmap 0x%x FirstDiffRateThreshold %d SecondDiffRateThreshold %d\n",
		 priv->rtllib->fsync_rate_bitmap,
>>>>>>> v4.9.227
		 priv->rtllib->fsync_firstdiff_ratethreshold,
		 priv->rtllib->fsync_seconddiff_ratethreshold);

	if (priv->rtllib->state == RTLLIB_LINKED &&
	    priv->rtllib->pHTInfo->IOTPeer == HT_IOT_PEER_BROADCOM) {
		if (priv->rtllib->bfsync_enable == 0) {
			switch (priv->rtllib->fsync_state) {
			case Default_Fsync:
<<<<<<< HEAD
				dm_StartHWFsync(dev);
				priv->rtllib->fsync_state = HW_Fsync;
				break;
			case SW_Fsync:
				dm_EndSWFsync(dev);
				dm_StartHWFsync(dev);
=======
				_rtl92e_dm_start_hw_fsync(dev);
				priv->rtllib->fsync_state = HW_Fsync;
				break;
			case SW_Fsync:
				_rtl92e_dm_end_sw_fsync(dev);
				_rtl92e_dm_start_hw_fsync(dev);
>>>>>>> v4.9.227
				priv->rtllib->fsync_state = HW_Fsync;
				break;
			case HW_Fsync:
			default:
				break;
			}
		} else {
			switch (priv->rtllib->fsync_state) {
			case Default_Fsync:
<<<<<<< HEAD
				dm_StartSWFsync(dev);
				priv->rtllib->fsync_state = SW_Fsync;
				break;
			case HW_Fsync:
				dm_EndHWFsync(dev);
				dm_StartSWFsync(dev);
=======
				_rtl92e_dm_start_sw_fsync(dev);
				priv->rtllib->fsync_state = SW_Fsync;
				break;
			case HW_Fsync:
				_rtl92e_dm_end_hw_fsync(dev);
				_rtl92e_dm_start_sw_fsync(dev);
>>>>>>> v4.9.227
				priv->rtllib->fsync_state = SW_Fsync;
				break;
			case SW_Fsync:
			default:
				break;

			}
		}
		if (priv->framesyncMonitor) {
			if (reg_c38_State != RegC38_Fsync_AP_BCM) {
<<<<<<< HEAD
				write_nic_byte(dev, rOFDM0_RxDetector3, 0x95);
=======
				rtl92e_writeb(dev, rOFDM0_RxDetector3, 0x95);
>>>>>>> v4.9.227

				reg_c38_State = RegC38_Fsync_AP_BCM;
			}
		}
	} else {
		switch (priv->rtllib->fsync_state) {
		case HW_Fsync:
<<<<<<< HEAD
			dm_EndHWFsync(dev);
			priv->rtllib->fsync_state = Default_Fsync;
			break;
		case SW_Fsync:
			dm_EndSWFsync(dev);
=======
			_rtl92e_dm_end_hw_fsync(dev);
			priv->rtllib->fsync_state = Default_Fsync;
			break;
		case SW_Fsync:
			_rtl92e_dm_end_sw_fsync(dev);
>>>>>>> v4.9.227
			priv->rtllib->fsync_state = Default_Fsync;
			break;
		case Default_Fsync:
		default:
			break;
		}

		if (priv->framesyncMonitor) {
			if (priv->rtllib->state == RTLLIB_LINKED) {
				if (priv->undecorated_smoothed_pwdb <=
				    RegC38_TH) {
					if (reg_c38_State !=
					    RegC38_NonFsync_Other_AP) {
<<<<<<< HEAD
							write_nic_byte(dev,
							    rOFDM0_RxDetector3,
							    0x90);
=======
						rtl92e_writeb(dev,
							      rOFDM0_RxDetector3,
							      0x90);
>>>>>>> v4.9.227

						reg_c38_State =
						     RegC38_NonFsync_Other_AP;
					}
				} else if (priv->undecorated_smoothed_pwdb >=
					   (RegC38_TH+5)) {
					if (reg_c38_State) {
<<<<<<< HEAD
						write_nic_byte(dev,
=======
						rtl92e_writeb(dev,
>>>>>>> v4.9.227
							rOFDM0_RxDetector3,
							priv->framesync);
						reg_c38_State = RegC38_Default;
					}
				}
			} else {
				if (reg_c38_State) {
<<<<<<< HEAD
					write_nic_byte(dev, rOFDM0_RxDetector3,
						       priv->framesync);
=======
					rtl92e_writeb(dev, rOFDM0_RxDetector3,
						      priv->framesync);
>>>>>>> v4.9.227
					reg_c38_State = RegC38_Default;
				}
			}
		}
	}
	if (priv->framesyncMonitor) {
		if (priv->reset_count != reset_cnt) {
<<<<<<< HEAD
			write_nic_byte(dev, rOFDM0_RxDetector3,
=======
			rtl92e_writeb(dev, rOFDM0_RxDetector3,
>>>>>>> v4.9.227
				       priv->framesync);
			reg_c38_State = RegC38_Default;
			reset_cnt = priv->reset_count;
		}
	} else {
		if (reg_c38_State) {
<<<<<<< HEAD
			write_nic_byte(dev, rOFDM0_RxDetector3,
=======
			rtl92e_writeb(dev, rOFDM0_RxDetector3,
>>>>>>> v4.9.227
				       priv->framesync);
			reg_c38_State = RegC38_Default;
		}
	}
}

<<<<<<< HEAD
void dm_shadow_init(struct net_device *dev)
{
	u8	page;
	u16	offset;

	for (page = 0; page < 5; page++)
		for (offset = 0; offset < 256; offset++)
			dm_shadow[page][offset] = read_nic_byte(dev,
						  offset+page * 256);

	for (page = 8; page < 11; page++)
		for (offset = 0; offset < 256; offset++)
			dm_shadow[page][offset] = read_nic_byte(dev,
						  offset+page * 256);

	for (page = 12; page < 15; page++)
		for (offset = 0; offset < 256; offset++)
			dm_shadow[page][offset] = read_nic_byte(dev,
						  offset+page*256);

}

/*---------------------------Define function prototype------------------------*/
static void dm_init_dynamic_txpower(struct net_device *dev)
=======
/*---------------------------Define function prototype------------------------*/
static void _rtl92e_dm_init_dynamic_tx_power(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);

	priv->rtllib->bdynamic_txpower_enable = true;
	priv->bLastDTPFlag_High = false;
	priv->bLastDTPFlag_Low = false;
	priv->bDynamicTxHighPower = false;
	priv->bDynamicTxLowPower = false;
}

<<<<<<< HEAD
static void dm_dynamic_txpower(struct net_device *dev)
=======
static void _rtl92e_dm_dynamic_tx_power(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	unsigned int txhipower_threshhold = 0;
	unsigned int txlowpower_threshold = 0;

	if (priv->rtllib->bdynamic_txpower_enable != true) {
		priv->bDynamicTxHighPower = false;
		priv->bDynamicTxLowPower = false;
		return;
	}
	if ((priv->rtllib->pHTInfo->IOTPeer == HT_IOT_PEER_ATHEROS) &&
	    (priv->rtllib->mode == IEEE_G)) {
		txhipower_threshhold = TX_POWER_ATHEROAP_THRESH_HIGH;
		txlowpower_threshold = TX_POWER_ATHEROAP_THRESH_LOW;
	} else {
		txhipower_threshhold = TX_POWER_NEAR_FIELD_THRESH_HIGH;
		txlowpower_threshold = TX_POWER_NEAR_FIELD_THRESH_LOW;
	}

	RT_TRACE(COMP_TXAGC, "priv->undecorated_smoothed_pwdb = %ld\n",
		 priv->undecorated_smoothed_pwdb);

	if (priv->rtllib->state == RTLLIB_LINKED) {
		if (priv->undecorated_smoothed_pwdb >= txhipower_threshhold) {
			priv->bDynamicTxHighPower = true;
			priv->bDynamicTxLowPower = false;
		} else {
			if (priv->undecorated_smoothed_pwdb <
			    txlowpower_threshold && priv->bDynamicTxHighPower)
				priv->bDynamicTxHighPower = false;
			if (priv->undecorated_smoothed_pwdb < 35)
				priv->bDynamicTxLowPower = true;
			else if (priv->undecorated_smoothed_pwdb >= 40)
				priv->bDynamicTxLowPower = false;
		}
	} else {
		priv->bDynamicTxHighPower = false;
		priv->bDynamicTxLowPower = false;
	}

	if ((priv->bDynamicTxHighPower != priv->bLastDTPFlag_High) ||
	    (priv->bDynamicTxLowPower != priv->bLastDTPFlag_Low)) {
		RT_TRACE(COMP_TXAGC, "SetTxPowerLevel8190()  channel = %d\n",
			 priv->rtllib->current_network.channel);

<<<<<<< HEAD
		rtl8192_phy_setTxPower(dev,
				 priv->rtllib->current_network.channel);
=======
		rtl92e_set_tx_power(dev, priv->rtllib->current_network.channel);
>>>>>>> v4.9.227
	}
	priv->bLastDTPFlag_High = priv->bDynamicTxHighPower;
	priv->bLastDTPFlag_Low = priv->bDynamicTxLowPower;

}

<<<<<<< HEAD
static void dm_check_txrateandretrycount(struct net_device *dev)
=======
static void _rtl92e_dm_check_txrateandretrycount(struct net_device *dev)
>>>>>>> v4.9.227
{
	struct r8192_priv *priv = rtllib_priv(dev);
	struct rtllib_device *ieee = priv->rtllib;

<<<<<<< HEAD
	ieee->softmac_stats.CurrentShowTxate = read_nic_byte(dev,
						 Current_Tx_Rate_Reg);

	ieee->softmac_stats.last_packet_rate = read_nic_byte(dev,
						 Initial_Tx_Rate_Reg);

	ieee->softmac_stats.txretrycount = read_nic_dword(dev,
						 Tx_Retry_Count_Reg);
}

static void dm_send_rssi_tofw(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	write_nic_byte(dev, DRIVER_RSSI, (u8)priv->undecorated_smoothed_pwdb);
=======
	ieee->softmac_stats.CurrentShowTxate = rtl92e_readb(dev,
						 Current_Tx_Rate_Reg);

	ieee->softmac_stats.last_packet_rate = rtl92e_readb(dev,
						 Initial_Tx_Rate_Reg);

	ieee->softmac_stats.txretrycount = rtl92e_readl(dev,
						 Tx_Retry_Count_Reg);
}

static void _rtl92e_dm_send_rssi_to_fw(struct net_device *dev)
{
	struct r8192_priv *priv = rtllib_priv(dev);

	rtl92e_writeb(dev, DRIVER_RSSI, (u8)priv->undecorated_smoothed_pwdb);
>>>>>>> v4.9.227
}
