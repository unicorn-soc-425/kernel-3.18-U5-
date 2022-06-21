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
#ifndef __INC_DOT11D_H
#define __INC_DOT11D_H

#include "rtllib.h"

<<<<<<< HEAD


=======
>>>>>>> v4.9.227
struct chnl_txpow_triple {
	u8 FirstChnl;
	u8  NumChnls;
	u8  MaxTxPowerInDbm;
};

enum dot11d_state {
	DOT11D_STATE_NONE = 0,
	DOT11D_STATE_LEARNED,
	DOT11D_STATE_DONE,
};

/**
 * struct rt_dot11d_info * @CountryIeLen: value greater than 0 if @CountryIeBuf contains
 *		  valid country information element.
 * @channel_map: holds channel values
 *		0 - invalid,
 *		1 - valid (active scan),
 *		2 - valid (passive scan)
 * @CountryIeSrcAddr - Source AP of the country IE
 */

struct rt_dot11d_info {
<<<<<<< HEAD

=======
>>>>>>> v4.9.227
	bool bEnabled;

	u16 CountryIeLen;
	u8  CountryIeBuf[MAX_IE_LEN];
	u8  CountryIeSrcAddr[6];
	u8  CountryIeWatchdog;

<<<<<<< HEAD
	u8  channel_map[MAX_CHANNEL_NUMBER+1];
	u8  MaxTxPwrDbmList[MAX_CHANNEL_NUMBER+1];
=======
	u8  channel_map[MAX_CHANNEL_NUMBER + 1];
	u8  MaxTxPwrDbmList[MAX_CHANNEL_NUMBER + 1];
>>>>>>> v4.9.227

	enum dot11d_state State;
};

static inline void cpMacAddr(unsigned char *des, unsigned char *src)
{
	memcpy(des, src, 6);
}

#define GET_DOT11D_INFO(__pIeeeDev)			\
	 ((struct rt_dot11d_info *)((__pIeeeDev)->pDot11dInfo))

#define IS_DOT11D_ENABLE(__pIeeeDev)			\
	 (GET_DOT11D_INFO(__pIeeeDev)->bEnabled)
#define IS_COUNTRY_IE_VALID(__pIeeeDev)			\
	(GET_DOT11D_INFO(__pIeeeDev)->CountryIeLen > 0)

#define IS_EQUAL_CIE_SRC(__pIeeeDev, __pTa)		\
<<<<<<< HEAD
	 eqMacAddr(GET_DOT11D_INFO(__pIeeeDev)->CountryIeSrcAddr, __pTa)
#define UPDATE_CIE_SRC(__pIeeeDev, __pTa)		\
	cpMacAddr(GET_DOT11D_INFO(__pIeeeDev)->CountryIeSrcAddr, __pTa)

#define IS_COUNTRY_IE_CHANGED(__pIeeeDev, __Ie) \
	(((__Ie).Length == 0 || (__Ie).Length !=	\
	GET_DOT11D_INFO(__pIeeeDev)->CountryIeLen) ?	\
	false : (!memcmp(GET_DOT11D_INFO(__pIeeeDev)->CountryIeBuf,	\
	(__Ie).Octet, (__Ie).Length)))

#define CIE_WATCHDOG_TH 1
=======
	 ether_addr_equal_unaligned( \
		GET_DOT11D_INFO(__pIeeeDev)->CountryIeSrcAddr, __pTa)
#define UPDATE_CIE_SRC(__pIeeeDev, __pTa)		\
	cpMacAddr(GET_DOT11D_INFO(__pIeeeDev)->CountryIeSrcAddr, __pTa)

>>>>>>> v4.9.227
#define GET_CIE_WATCHDOG(__pIeeeDev)				\
	 (GET_DOT11D_INFO(__pIeeeDev)->CountryIeWatchdog)
static inline void RESET_CIE_WATCHDOG(struct rtllib_device *__pIeeeDev)
{
	GET_CIE_WATCHDOG(__pIeeeDev) = 0;
}
<<<<<<< HEAD
#define UPDATE_CIE_WATCHDOG(__pIeeeDev) (++GET_CIE_WATCHDOG(__pIeeeDev))

#define IS_DOT11D_STATE_DONE(__pIeeeDev)			\
	(GET_DOT11D_INFO(__pIeeeDev)->State == DOT11D_STATE_DONE)
=======

#define UPDATE_CIE_WATCHDOG(__pIeeeDev) (++GET_CIE_WATCHDOG(__pIeeeDev))
>>>>>>> v4.9.227

void dot11d_init(struct rtllib_device *dev);
void Dot11d_Channelmap(u8 channel_plan, struct rtllib_device *ieee);
void Dot11d_Reset(struct rtllib_device *dev);
void Dot11d_UpdateCountryIe(struct rtllib_device *dev, u8 *pTaddr,
			    u16 CoutryIeLen, u8 *pCoutryIe);
<<<<<<< HEAD
u8 DOT11D_GetMaxTxPwrInDbm(struct rtllib_device *dev, u8 Channel);
void DOT11D_ScanComplete(struct rtllib_device *dev);
int ToLegalChannel(struct rtllib_device *dev, u8 channel);
=======
void DOT11D_ScanComplete(struct rtllib_device *dev);
>>>>>>> v4.9.227

#endif
