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
#ifndef _RTW_HT_H_
#define _RTW_HT_H_

<<<<<<< HEAD
#include <osdep_service.h>
#include "wifi.h"
=======
#include <linux/ieee80211.h>
>>>>>>> v4.9.227

struct ht_priv {
	u32	ht_option;
	u32	ampdu_enable;/* for enable Tx A-MPDU */
<<<<<<< HEAD
	u32	tx_amsdu_enable;/* for enable Tx A-MSDU */
	u32	tx_amdsu_maxlen; /*  1: 8k, 0:4k ; default:8k, for tx */
	u32	rx_ampdu_maxlen; /* for rx reordering ctrl win_sz,
				  * updated when join_callback. */
=======
>>>>>>> v4.9.227
	u8	bwmode;/*  */
	u8	ch_offset;/* PRIME_CHNL_OFFSET */
	u8	sgi;/* short GI */

	/* for processing Tx A-MPDU */
	u8	agg_enable_bitmap;
	u8	candidate_tid_bitmap;

<<<<<<< HEAD
	struct rtw_ieee80211_ht_cap ht_cap;
=======
	struct ieee80211_ht_cap ht_cap;
>>>>>>> v4.9.227
};

#endif	/* _RTL871X_HT_H_ */
