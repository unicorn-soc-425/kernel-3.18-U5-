/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
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
/*  */
/*  Description: */
/*  */
/*  This file is for 92CE/92CU dynamic mechanism only */
/*  */
/*  */
/*  */
#define _RTL8188E_DM_C_

#include <osdep_service.h>
#include <drv_types.h>

#include <rtl8188e_hal.h>

/*  Initialize GPIO setting registers */
static void dm_InitGPIOSetting(struct adapter *Adapter)
{
	u8	tmp1byte;

	tmp1byte = usb_read8(Adapter, REG_GPIO_MUXCFG);
	tmp1byte &= (GPIOSEL_GPIO | ~GPIOSEL_ENBT);

	usb_write8(Adapter, REG_GPIO_MUXCFG, tmp1byte);
}

/*  */
/*  functions */
/*  */
static void Init_ODM_ComInfo_88E(struct adapter *Adapter)
{
<<<<<<< HEAD
	struct hal_data_8188e *hal_data = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &hal_data->dmpriv;
	struct odm_dm_struct *dm_odm = &(hal_data->odmpriv);
	u8 cut_ver, fab_ver;
=======
	struct hal_data_8188e *hal_data = Adapter->HalData;
	struct dm_priv	*pdmpriv = &hal_data->dmpriv;
	struct odm_dm_struct *dm_odm = &(hal_data->odmpriv);
>>>>>>> v4.9.227

	/*  Init Value */
	memset(dm_odm, 0, sizeof(*dm_odm));

	dm_odm->Adapter = Adapter;
<<<<<<< HEAD

	ODM_CmnInfoInit(dm_odm, ODM_CMNINFO_PLATFORM, ODM_CE);

	ODM_CmnInfoInit(dm_odm, ODM_CMNINFO_IC_TYPE, ODM_RTL8188E);

	fab_ver = ODM_TSMC;
	cut_ver = ODM_CUT_A;

	ODM_CmnInfoInit(dm_odm, ODM_CMNINFO_FAB_VER, fab_ver);
	ODM_CmnInfoInit(dm_odm, ODM_CMNINFO_CUT_VER, cut_ver);

	ODM_CmnInfoInit(dm_odm, ODM_CMNINFO_MP_TEST_CHIP, IS_NORMAL_CHIP(hal_data->VersionID));

	ODM_CmnInfoInit(dm_odm, ODM_CMNINFO_PATCH_ID, hal_data->CustomerID);
	ODM_CmnInfoInit(dm_odm, ODM_CMNINFO_BWIFI_TEST, Adapter->registrypriv.wifi_spec);


	if (hal_data->rf_type == RF_1T1R)
		ODM_CmnInfoUpdate(dm_odm, ODM_CMNINFO_RF_TYPE, ODM_1T1R);
	else if (hal_data->rf_type == RF_2T2R)
		ODM_CmnInfoUpdate(dm_odm, ODM_CMNINFO_RF_TYPE, ODM_2T2R);
	else if (hal_data->rf_type == RF_1T2R)
		ODM_CmnInfoUpdate(dm_odm, ODM_CMNINFO_RF_TYPE, ODM_1T2R);

	ODM_CmnInfoInit(dm_odm, ODM_CMNINFO_RF_ANTENNA_TYPE, hal_data->TRxAntDivType);
=======
	dm_odm->SupportPlatform = ODM_CE;
	dm_odm->SupportICType = ODM_RTL8188E;
	dm_odm->CutVersion = ODM_CUT_A;
	dm_odm->bIsMPChip = hal_data->VersionID.ChipType == NORMAL_CHIP;
	dm_odm->PatchID = hal_data->CustomerID;
	dm_odm->bWIFITest = Adapter->registrypriv.wifi_spec;

	dm_odm->AntDivType = hal_data->TRxAntDivType;

	/*  Tx power tracking BB swing table. */
	/*  The base index = 12. +((12-n)/2)dB 13~?? = decrease tx pwr by -((n-12)/2)dB */
	dm_odm->BbSwingIdxOfdm = 12; /*  Set defalut value as index 12. */
	dm_odm->BbSwingIdxOfdmCurrent = 12;
	dm_odm->BbSwingFlagOfdm = false;
>>>>>>> v4.9.227

	pdmpriv->InitODMFlag =	ODM_RF_CALIBRATION |
				ODM_RF_TX_PWR_TRACK;

<<<<<<< HEAD
	ODM_CmnInfoUpdate(dm_odm, ODM_CMNINFO_ABILITY, pdmpriv->InitODMFlag);
=======
	dm_odm->SupportAbility = pdmpriv->InitODMFlag;
>>>>>>> v4.9.227
}

static void Update_ODM_ComInfo_88E(struct adapter *Adapter)
{
	struct mlme_ext_priv	*pmlmeext = &Adapter->mlmeextpriv;
	struct mlme_priv	*pmlmepriv = &Adapter->mlmepriv;
	struct pwrctrl_priv *pwrctrlpriv = &Adapter->pwrctrlpriv;
<<<<<<< HEAD
	struct hal_data_8188e *hal_data = GET_HAL_DATA(Adapter);
=======
	struct hal_data_8188e *hal_data = Adapter->HalData;
>>>>>>> v4.9.227
	struct odm_dm_struct *dm_odm = &(hal_data->odmpriv);
	struct dm_priv	*pdmpriv = &hal_data->dmpriv;
	int i;

	pdmpriv->InitODMFlag =	ODM_BB_DIG |
				ODM_BB_RA_MASK |
				ODM_BB_DYNAMIC_TXPWR |
				ODM_BB_FA_CNT |
				ODM_BB_RSSI_MONITOR |
				ODM_BB_CCK_PD |
				ODM_BB_PWR_SAVE |
				ODM_MAC_EDCA_TURBO |
				ODM_RF_CALIBRATION |
				ODM_RF_TX_PWR_TRACK;
	if (hal_data->AntDivCfg)
		pdmpriv->InitODMFlag |= ODM_BB_ANT_DIV;

	if (Adapter->registrypriv.mp_mode == 1) {
		pdmpriv->InitODMFlag =	ODM_RF_CALIBRATION |
					ODM_RF_TX_PWR_TRACK;
	}

<<<<<<< HEAD
	ODM_CmnInfoUpdate(dm_odm, ODM_CMNINFO_ABILITY, pdmpriv->InitODMFlag);

	ODM_CmnInfoHook(dm_odm, ODM_CMNINFO_TX_UNI, &(Adapter->xmitpriv.tx_bytes));
	ODM_CmnInfoHook(dm_odm, ODM_CMNINFO_RX_UNI, &(Adapter->recvpriv.rx_bytes));
	ODM_CmnInfoHook(dm_odm, ODM_CMNINFO_WM_MODE, &(pmlmeext->cur_wireless_mode));
	ODM_CmnInfoHook(dm_odm, ODM_CMNINFO_SEC_CHNL_OFFSET, &(hal_data->nCur40MhzPrimeSC));
	ODM_CmnInfoHook(dm_odm, ODM_CMNINFO_SEC_MODE, &(Adapter->securitypriv.dot11PrivacyAlgrthm));
	ODM_CmnInfoHook(dm_odm, ODM_CMNINFO_BW, &(hal_data->CurrentChannelBW));
	ODM_CmnInfoHook(dm_odm, ODM_CMNINFO_CHNL, &(hal_data->CurrentChannel));
	ODM_CmnInfoHook(dm_odm, ODM_CMNINFO_NET_CLOSED, &(Adapter->net_closed));
	ODM_CmnInfoHook(dm_odm, ODM_CMNINFO_MP_MODE, &(Adapter->registrypriv.mp_mode));
	ODM_CmnInfoHook(dm_odm, ODM_CMNINFO_SCAN, &(pmlmepriv->bScanInProcess));
	ODM_CmnInfoHook(dm_odm, ODM_CMNINFO_POWER_SAVING, &(pwrctrlpriv->bpower_saving));
	ODM_CmnInfoInit(dm_odm, ODM_CMNINFO_RF_ANTENNA_TYPE, hal_data->TRxAntDivType);
=======
	dm_odm->SupportAbility = pdmpriv->InitODMFlag;

	dm_odm->pNumTxBytesUnicast = &Adapter->xmitpriv.tx_bytes;
	dm_odm->pNumRxBytesUnicast = &Adapter->recvpriv.rx_bytes;
	dm_odm->pWirelessMode = &pmlmeext->cur_wireless_mode;
	dm_odm->pSecChOffset = &hal_data->nCur40MhzPrimeSC;
	dm_odm->pSecurity = (u8 *)&Adapter->securitypriv.dot11PrivacyAlgrthm;
	dm_odm->pBandWidth = (u8 *)&hal_data->CurrentChannelBW;
	dm_odm->pChannel = &hal_data->CurrentChannel;
	dm_odm->pbNet_closed = (bool *)&Adapter->net_closed;
	dm_odm->mp_mode = &Adapter->registrypriv.mp_mode;
	dm_odm->pbScanInProcess = (bool *)&pmlmepriv->bScanInProcess;
	dm_odm->pbPowerSaving = (bool *)&pwrctrlpriv->bpower_saving;
	dm_odm->AntDivType = hal_data->TRxAntDivType;

	/*  Tx power tracking BB swing table. */
	/*  The base index = 12. +((12-n)/2)dB 13~?? = decrease tx pwr by -((n-12)/2)dB */
	dm_odm->BbSwingIdxOfdm = 12; /*  Set defalut value as index 12. */
	dm_odm->BbSwingIdxOfdmCurrent = 12;
	dm_odm->BbSwingFlagOfdm = false;
>>>>>>> v4.9.227

	for (i = 0; i < NUM_STA; i++)
		ODM_CmnInfoPtrArrayHook(dm_odm, ODM_CMNINFO_STA_STATUS, i, NULL);
}

void rtl8188e_InitHalDm(struct adapter *Adapter)
{
<<<<<<< HEAD
	struct hal_data_8188e *hal_data = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &hal_data->dmpriv;
	struct odm_dm_struct *dm_odm = &(hal_data->odmpriv);
=======
	struct dm_priv	*pdmpriv = &Adapter->HalData->dmpriv;
	struct odm_dm_struct *dm_odm = &(Adapter->HalData->odmpriv);
>>>>>>> v4.9.227

	dm_InitGPIOSetting(Adapter);
	pdmpriv->DM_Type = DM_Type_ByDriver;
	pdmpriv->DMFlag = DYNAMIC_FUNC_DISABLE;
	Update_ODM_ComInfo_88E(Adapter);
	ODM_DMInit(dm_odm);
<<<<<<< HEAD
	Adapter->fix_rate = 0xFF;
}

void rtl8188e_HalDmWatchDog(struct adapter *Adapter)
{
	bool fw_cur_in_ps = false;
	bool fw_ps_awake = true;
	u8 hw_init_completed = false;
	struct hal_data_8188e *hal_data = GET_HAL_DATA(Adapter);
=======
}

void rtw_hal_dm_watchdog(struct adapter *Adapter)
{
	u8 hw_init_completed = false;
	struct mlme_priv *pmlmepriv = NULL;
	u8 bLinked = false;
>>>>>>> v4.9.227

	hw_init_completed = Adapter->hw_init_completed;

	if (!hw_init_completed)
		goto skip_dm;

<<<<<<< HEAD
	fw_cur_in_ps = Adapter->pwrctrlpriv.bFwCurrentInPSMode;
	rtw_hal_get_hwreg(Adapter, HW_VAR_FWLPS_RF_ON, (u8 *)(&fw_ps_awake));

	/*  Fw is under p2p powersaving mode, driver should stop dynamic mechanism. */
	/*  modifed by thomas. 2011.06.11. */
	if (Adapter->wdinfo.p2p_ps_mode)
		fw_ps_awake = false;

	/* ODM */
	if (hw_init_completed) {
		struct mlme_priv *pmlmepriv = &Adapter->mlmepriv;
		u8 bLinked = false;

		if ((check_fwstate(pmlmepriv, WIFI_AP_STATE)) ||
		    (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE))) {
			if (Adapter->stapriv.asoc_sta_count > 2)
				bLinked = true;
		} else {/* Station mode */
			if (check_fwstate(pmlmepriv, _FW_LINKED))
				bLinked = true;
		}

		ODM_CmnInfoUpdate(&hal_data->odmpriv, ODM_CMNINFO_LINK, bLinked);
		ODM_DMWatchdog(&hal_data->odmpriv);
	}
=======
	/* ODM */
	pmlmepriv = &Adapter->mlmepriv;

	if ((check_fwstate(pmlmepriv, WIFI_AP_STATE)) ||
	    (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE |
			   WIFI_ADHOC_MASTER_STATE))) {
		if (Adapter->stapriv.asoc_sta_count > 2)
			bLinked = true;
	} else {/* Station mode */
		if (check_fwstate(pmlmepriv, _FW_LINKED))
			bLinked = true;
	}

	Adapter->HalData->odmpriv.bLinked = bLinked;
	ODM_DMWatchdog(&Adapter->HalData->odmpriv);
>>>>>>> v4.9.227
skip_dm:
	/*  Check GPIO to determine current RF on/off and Pbc status. */
	/*  Check Hardware Radio ON/OFF or not */
	return;
}

<<<<<<< HEAD
void rtl8188e_init_dm_priv(struct adapter *Adapter)
{
	struct hal_data_8188e *hal_data = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &hal_data->dmpriv;
	struct odm_dm_struct *podmpriv = &hal_data->odmpriv;
=======
void rtw_hal_dm_init(struct adapter *Adapter)
{
	struct dm_priv	*pdmpriv = &Adapter->HalData->dmpriv;
	struct odm_dm_struct *podmpriv = &Adapter->HalData->odmpriv;
>>>>>>> v4.9.227

	memset(pdmpriv, 0, sizeof(struct dm_priv));
	Init_ODM_ComInfo_88E(Adapter);
	ODM_InitDebugSetting(podmpriv);
}

/*  Add new function to reset the state of antenna diversity before link. */
/*  Compare RSSI for deciding antenna */
<<<<<<< HEAD
void AntDivCompare8188E(struct adapter *Adapter, struct wlan_bssid_ex *dst, struct wlan_bssid_ex *src)
{
	struct hal_data_8188e *hal_data = GET_HAL_DATA(Adapter);

	if (0 != hal_data->AntDivCfg) {
=======
void rtw_hal_antdiv_rssi_compared(struct adapter *Adapter, struct wlan_bssid_ex *dst, struct wlan_bssid_ex *src)
{
	if (0 != Adapter->HalData->AntDivCfg) {
>>>>>>> v4.9.227
		/* select optimum_antenna for before linked =>For antenna diversity */
		if (dst->Rssi >=  src->Rssi) {/* keep org parameter */
			src->Rssi = dst->Rssi;
			src->PhyInfo.Optimum_antenna = dst->PhyInfo.Optimum_antenna;
		}
	}
}

/*  Add new function to reset the state of antenna diversity before link. */
<<<<<<< HEAD
u8 AntDivBeforeLink8188E(struct adapter *Adapter)
{
	struct hal_data_8188e *hal_data = GET_HAL_DATA(Adapter);
	struct odm_dm_struct *dm_odm = &hal_data->odmpriv;
=======
u8 rtw_hal_antdiv_before_linked(struct adapter *Adapter)
{
	struct odm_dm_struct *dm_odm = &Adapter->HalData->odmpriv;
>>>>>>> v4.9.227
	struct sw_ant_switch *dm_swat_tbl = &dm_odm->DM_SWAT_Table;
	struct mlme_priv *pmlmepriv = &(Adapter->mlmepriv);

	/*  Condition that does not need to use antenna diversity. */
<<<<<<< HEAD
	if (hal_data->AntDivCfg == 0)
=======
	if (Adapter->HalData->AntDivCfg == 0)
>>>>>>> v4.9.227
		return false;

	if (check_fwstate(pmlmepriv, _FW_LINKED))
		return false;

	if (dm_swat_tbl->SWAS_NoLink_State == 0) {
		/* switch channel */
		dm_swat_tbl->SWAS_NoLink_State = 1;
		dm_swat_tbl->CurAntenna = (dm_swat_tbl->CurAntenna == Antenna_A) ? Antenna_B : Antenna_A;

		rtw_antenna_select_cmd(Adapter, dm_swat_tbl->CurAntenna, false);
		return true;
	} else {
		dm_swat_tbl->SWAS_NoLink_State = 0;
		return false;
	}
}
