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
#include "rtllib.h"
#include <linux/etherdevice.h>
#include "rtl819x_TS.h"

static void TsSetupTimeOut(unsigned long data)
{
}

static void TsInactTimeout(unsigned long data)
{
}

static void RxPktPendingTimeout(unsigned long data)
{
	struct rx_ts_record *pRxTs = (struct rx_ts_record *)data;
	struct rtllib_device *ieee = container_of(pRxTs, struct rtllib_device,
						  RxTsRecord[pRxTs->num]);

	struct rx_reorder_entry *pReorderEntry = NULL;

	unsigned long flags = 0;
	u8 index = 0;
	bool bPktInBuf = false;

	spin_lock_irqsave(&(ieee->reorder_spinlock), flags);
	if (pRxTs->RxTimeoutIndicateSeq != 0xffff) {
		while (!list_empty(&pRxTs->RxPendingPktList)) {
			pReorderEntry = (struct rx_reorder_entry *)
					list_entry(pRxTs->RxPendingPktList.prev,
					struct rx_reorder_entry, List);
			if (index == 0)
				pRxTs->RxIndicateSeq = pReorderEntry->SeqNum;

<<<<<<< HEAD
			if (SN_LESS(pReorderEntry->SeqNum, pRxTs->RxIndicateSeq) ||
				SN_EQUAL(pReorderEntry->SeqNum, pRxTs->RxIndicateSeq)) {
=======
			if (SN_LESS(pReorderEntry->SeqNum,
				    pRxTs->RxIndicateSeq) ||
			    SN_EQUAL(pReorderEntry->SeqNum,
				     pRxTs->RxIndicateSeq)) {
>>>>>>> v4.9.227
				list_del_init(&pReorderEntry->List);

				if (SN_EQUAL(pReorderEntry->SeqNum,
				    pRxTs->RxIndicateSeq))
					pRxTs->RxIndicateSeq =
					      (pRxTs->RxIndicateSeq + 1) % 4096;

<<<<<<< HEAD
				RTLLIB_DEBUG(RTLLIB_DL_REORDER, "%s(): Indicate"
					     " SeqNum: %d\n", __func__,
					     pReorderEntry->SeqNum);
=======
				netdev_dbg(ieee->dev,
					   "%s(): Indicate SeqNum: %d\n",
					   __func__, pReorderEntry->SeqNum);
>>>>>>> v4.9.227
				ieee->stats_IndicateArray[index] =
							 pReorderEntry->prxb;
				index++;

				list_add_tail(&pReorderEntry->List,
					      &ieee->RxReorder_Unused_List);
			} else {
				bPktInBuf = true;
				break;
			}
		}
	}

	if (index > 0) {
		pRxTs->RxTimeoutIndicateSeq = 0xffff;

		if (index > REORDER_WIN_SIZE) {
<<<<<<< HEAD
			RTLLIB_DEBUG(RTLLIB_DL_ERR, "RxReorderIndicatePacket():"
				     " Rx Reorder struct buffer full!!\n");
=======
			netdev_warn(ieee->dev,
				    "%s(): Rx Reorder struct buffer full\n",
				    __func__);
>>>>>>> v4.9.227
			spin_unlock_irqrestore(&(ieee->reorder_spinlock),
					       flags);
			return;
		}
		rtllib_indicate_packets(ieee, ieee->stats_IndicateArray, index);
		bPktInBuf = false;
	}

	if (bPktInBuf && (pRxTs->RxTimeoutIndicateSeq == 0xffff)) {
		pRxTs->RxTimeoutIndicateSeq = pRxTs->RxIndicateSeq;
		mod_timer(&pRxTs->RxPktPendingTimer,  jiffies +
<<<<<<< HEAD
			  MSECS(ieee->pHTInfo->RxReorderPendingTime));
=======
			  msecs_to_jiffies(ieee->pHTInfo->RxReorderPendingTime)
			  );
>>>>>>> v4.9.227
	}
	spin_unlock_irqrestore(&(ieee->reorder_spinlock), flags);
}

static void TsAddBaProcess(unsigned long data)
{
	struct tx_ts_record *pTxTs = (struct tx_ts_record *)data;
	u8 num = pTxTs->num;
	struct rtllib_device *ieee = container_of(pTxTs, struct rtllib_device,
				     TxTsRecord[num]);

	TsInitAddBA(ieee, pTxTs, BA_POLICY_IMMEDIATE, false);
<<<<<<< HEAD
	RTLLIB_DEBUG(RTLLIB_DL_BA, "TsAddBaProcess(): ADDBA Req is "
		     "started!!\n");
=======
	netdev_dbg(ieee->dev, "%s(): ADDBA Req is started\n", __func__);
>>>>>>> v4.9.227
}

static void ResetTsCommonInfo(struct ts_common_info *pTsCommonInfo)
{
<<<<<<< HEAD
	memset(pTsCommonInfo->Addr, 0, 6);
=======
	eth_zero_addr(pTsCommonInfo->Addr);
>>>>>>> v4.9.227
	memset(&pTsCommonInfo->TSpec, 0, sizeof(union tspec_body));
	memset(&pTsCommonInfo->TClass, 0, sizeof(union qos_tclas)*TCLAS_NUM);
	pTsCommonInfo->TClasProc = 0;
	pTsCommonInfo->TClasNum = 0;
}

static void ResetTxTsEntry(struct tx_ts_record *pTS)
{
	ResetTsCommonInfo(&pTS->TsCommonInfo);
	pTS->TxCurSeq = 0;
	pTS->bAddBaReqInProgress = false;
	pTS->bAddBaReqDelayed = false;
	pTS->bUsingBa = false;
	pTS->bDisable_AddBa = false;
	ResetBaEntry(&pTS->TxAdmittedBARecord);
	ResetBaEntry(&pTS->TxPendingBARecord);
}

static void ResetRxTsEntry(struct rx_ts_record *pTS)
{
	ResetTsCommonInfo(&pTS->TsCommonInfo);
	pTS->RxIndicateSeq = 0xffff;
	pTS->RxTimeoutIndicateSeq = 0xffff;
	ResetBaEntry(&pTS->RxAdmittedBARecord);
}

void TSInitialize(struct rtllib_device *ieee)
{
	struct tx_ts_record *pTxTS  = ieee->TxTsRecord;
	struct rx_ts_record *pRxTS  = ieee->RxTsRecord;
	struct rx_reorder_entry *pRxReorderEntry = ieee->RxReorderEntry;
	u8				count = 0;

<<<<<<< HEAD
	RTLLIB_DEBUG(RTLLIB_DL_TS, "==========>%s()\n", __func__);
=======
	netdev_vdbg(ieee->dev, "%s()\n", __func__);
>>>>>>> v4.9.227
	INIT_LIST_HEAD(&ieee->Tx_TS_Admit_List);
	INIT_LIST_HEAD(&ieee->Tx_TS_Pending_List);
	INIT_LIST_HEAD(&ieee->Tx_TS_Unused_List);

	for (count = 0; count < TOTAL_TS_NUM; count++) {
		pTxTS->num = count;
<<<<<<< HEAD
		_setup_timer(&pTxTS->TsCommonInfo.SetupTimer,
			    TsSetupTimeOut,
			    (unsigned long) pTxTS);

		_setup_timer(&pTxTS->TsCommonInfo.InactTimer,
			    TsInactTimeout,
			    (unsigned long) pTxTS);

		_setup_timer(&pTxTS->TsAddBaTimer,
			    TsAddBaProcess,
			    (unsigned long) pTxTS);

		_setup_timer(&pTxTS->TxPendingBARecord.Timer,
			    BaSetupTimeOut,
			    (unsigned long) pTxTS);
		_setup_timer(&pTxTS->TxAdmittedBARecord.Timer,
=======
		setup_timer(&pTxTS->TsCommonInfo.SetupTimer,
			    TsSetupTimeOut,
			    (unsigned long) pTxTS);

		setup_timer(&pTxTS->TsCommonInfo.InactTimer,
			    TsInactTimeout,
			    (unsigned long) pTxTS);

		setup_timer(&pTxTS->TsAddBaTimer,
			    TsAddBaProcess,
			    (unsigned long) pTxTS);

		setup_timer(&pTxTS->TxPendingBARecord.Timer,
			    BaSetupTimeOut,
			    (unsigned long) pTxTS);
		setup_timer(&pTxTS->TxAdmittedBARecord.Timer,
>>>>>>> v4.9.227
			    TxBaInactTimeout,
			    (unsigned long) pTxTS);

		ResetTxTsEntry(pTxTS);
		list_add_tail(&pTxTS->TsCommonInfo.List,
				&ieee->Tx_TS_Unused_List);
		pTxTS++;
	}

	INIT_LIST_HEAD(&ieee->Rx_TS_Admit_List);
	INIT_LIST_HEAD(&ieee->Rx_TS_Pending_List);
	INIT_LIST_HEAD(&ieee->Rx_TS_Unused_List);
	for (count = 0; count < TOTAL_TS_NUM; count++) {
		pRxTS->num = count;
		INIT_LIST_HEAD(&pRxTS->RxPendingPktList);

<<<<<<< HEAD
		_setup_timer(&pRxTS->TsCommonInfo.SetupTimer,
			    TsSetupTimeOut,
			    (unsigned long) pRxTS);

		_setup_timer(&pRxTS->TsCommonInfo.InactTimer,
			    TsInactTimeout,
			    (unsigned long) pRxTS);

		_setup_timer(&pRxTS->RxAdmittedBARecord.Timer,
			    RxBaInactTimeout,
			    (unsigned long) pRxTS);

		_setup_timer(&pRxTS->RxPktPendingTimer,
=======
		setup_timer(&pRxTS->TsCommonInfo.SetupTimer,
			    TsSetupTimeOut,
			    (unsigned long) pRxTS);

		setup_timer(&pRxTS->TsCommonInfo.InactTimer,
			    TsInactTimeout,
			    (unsigned long) pRxTS);

		setup_timer(&pRxTS->RxAdmittedBARecord.Timer,
			    RxBaInactTimeout,
			    (unsigned long) pRxTS);

		setup_timer(&pRxTS->RxPktPendingTimer,
>>>>>>> v4.9.227
			    RxPktPendingTimeout,
			    (unsigned long) pRxTS);

		ResetRxTsEntry(pRxTS);
		list_add_tail(&pRxTS->TsCommonInfo.List,
			      &ieee->Rx_TS_Unused_List);
		pRxTS++;
	}
	INIT_LIST_HEAD(&ieee->RxReorder_Unused_List);
	for (count = 0; count < REORDER_ENTRY_NUM; count++) {
		list_add_tail(&pRxReorderEntry->List,
			      &ieee->RxReorder_Unused_List);
		if (count == (REORDER_ENTRY_NUM-1))
			break;
		pRxReorderEntry = &ieee->RxReorderEntry[count+1];
	}

}

static void AdmitTS(struct rtllib_device *ieee,
		    struct ts_common_info *pTsCommonInfo, u32 InactTime)
{
	del_timer_sync(&pTsCommonInfo->SetupTimer);
	del_timer_sync(&pTsCommonInfo->InactTimer);

	if (InactTime != 0)
		mod_timer(&pTsCommonInfo->InactTimer, jiffies +
<<<<<<< HEAD
			  MSECS(InactTime));
=======
			  msecs_to_jiffies(InactTime));
>>>>>>> v4.9.227
}

static struct ts_common_info *SearchAdmitTRStream(struct rtllib_device *ieee,
						  u8 *Addr, u8 TID,
						  enum tr_select TxRxSelect)
{
	u8	dir;
	bool	search_dir[4] = {0};
	struct list_head *psearch_list;
	struct ts_common_info *pRet = NULL;

	if (ieee->iw_mode == IW_MODE_MASTER) {
		if (TxRxSelect == TX_DIR) {
			search_dir[DIR_DOWN] = true;
			search_dir[DIR_BI_DIR] = true;
		} else {
			search_dir[DIR_UP] = true;
			search_dir[DIR_BI_DIR] = true;
		}
	} else if (ieee->iw_mode == IW_MODE_ADHOC) {
		if (TxRxSelect == TX_DIR)
			search_dir[DIR_UP] = true;
		else
			search_dir[DIR_DOWN] = true;
	} else {
		if (TxRxSelect == TX_DIR) {
			search_dir[DIR_UP] = true;
			search_dir[DIR_BI_DIR] = true;
			search_dir[DIR_DIRECT] = true;
		} else {
			search_dir[DIR_DOWN] = true;
			search_dir[DIR_BI_DIR] = true;
			search_dir[DIR_DIRECT] = true;
		}
	}

	if (TxRxSelect == TX_DIR)
		psearch_list = &ieee->Tx_TS_Admit_List;
	else
		psearch_list = &ieee->Rx_TS_Admit_List;

	for (dir = 0; dir <= DIR_BI_DIR; dir++) {
		if (!search_dir[dir])
			continue;
		list_for_each_entry(pRet, psearch_list, List) {
<<<<<<< HEAD
			if (memcmp(pRet->Addr, Addr, 6) == 0)
				if (pRet->TSpec.f.TSInfo.field.ucTSID == TID)
					if (pRet->TSpec.f.TSInfo.field.ucDirection == dir)
						break;
=======
			if (memcmp(pRet->Addr, Addr, 6) == 0 &&
			    pRet->TSpec.f.TSInfo.field.ucTSID == TID &&
			    pRet->TSpec.f.TSInfo.field.ucDirection == dir)
				break;
>>>>>>> v4.9.227

		}
		if (&pRet->List  != psearch_list)
			break;
	}

	if (pRet && &pRet->List  != psearch_list)
		return pRet;
	return NULL;
}

static void MakeTSEntry(struct ts_common_info *pTsCommonInfo, u8 *Addr,
			union tspec_body *pTSPEC, union qos_tclas *pTCLAS,
			u8 TCLAS_Num, u8 TCLAS_Proc)
{
	u8	count;

	if (pTsCommonInfo == NULL)
		return;

	memcpy(pTsCommonInfo->Addr, Addr, 6);

	if (pTSPEC != NULL)
		memcpy((u8 *)(&(pTsCommonInfo->TSpec)), (u8 *)pTSPEC,
			sizeof(union tspec_body));

	for (count = 0; count < TCLAS_Num; count++)
		memcpy((u8 *)(&(pTsCommonInfo->TClass[count])),
		       (u8 *)pTCLAS, sizeof(union qos_tclas));

	pTsCommonInfo->TClasProc = TCLAS_Proc;
	pTsCommonInfo->TClasNum = TCLAS_Num;
}

bool GetTs(struct rtllib_device *ieee, struct ts_common_info **ppTS,
	   u8 *Addr, u8 TID, enum tr_select TxRxSelect, bool bAddNewTs)
{
	u8	UP = 0;
<<<<<<< HEAD

	if (is_multicast_ether_addr(Addr)) {
		RTLLIB_DEBUG(RTLLIB_DL_ERR, "ERR! get TS for Broadcast or "
			     "Multicast\n");
=======
	union tspec_body TSpec;
	union qos_tsinfo *pTSInfo = &TSpec.f.TSInfo;
	struct list_head *pUnusedList;
	struct list_head *pAddmitList;
	enum direction_value Dir;

	if (is_multicast_ether_addr(Addr)) {
		netdev_warn(ieee->dev, "Get TS for Broadcast or Multicast\n");
>>>>>>> v4.9.227
		return false;
	}
	if (ieee->current_network.qos_data.supported == 0) {
		UP = 0;
	} else {
<<<<<<< HEAD
		if (!IsACValid(TID)) {
			RTLLIB_DEBUG(RTLLIB_DL_ERR, "ERR! in %s(), TID(%d) is "
				     "not valid\n", __func__, TID);
			return false;
		}

=======
>>>>>>> v4.9.227
		switch (TID) {
		case 0:
		case 3:
			UP = 0;
			break;
		case 1:
		case 2:
			UP = 2;
			break;
		case 4:
		case 5:
			UP = 5;
			break;
		case 6:
		case 7:
			UP = 7;
			break;
<<<<<<< HEAD
=======
		default:
			netdev_warn(ieee->dev, "%s(): TID(%d) is not valid\n",
				    __func__, TID);
			return false;
>>>>>>> v4.9.227
		}
	}

	*ppTS = SearchAdmitTRStream(ieee, Addr, UP, TxRxSelect);
<<<<<<< HEAD
	if (*ppTS != NULL) {
		return true;
	} else {
		if (!bAddNewTs) {
			RTLLIB_DEBUG(RTLLIB_DL_TS, "add new TS failed"
				     "(tid:%d)\n", UP);
			return false;
		} else {
			union tspec_body TSpec;
			union qos_tsinfo *pTSInfo = &TSpec.f.TSInfo;
			struct list_head *pUnusedList =
				(TxRxSelect == TX_DIR) ?
				(&ieee->Tx_TS_Unused_List) :
				(&ieee->Rx_TS_Unused_List);

			struct list_head *pAddmitList =
				(TxRxSelect == TX_DIR) ?
				(&ieee->Tx_TS_Admit_List) :
				(&ieee->Rx_TS_Admit_List);

			enum direction_value Dir =
				 (ieee->iw_mode == IW_MODE_MASTER) ?
				 ((TxRxSelect == TX_DIR) ? DIR_DOWN : DIR_UP) :
				 ((TxRxSelect == TX_DIR) ? DIR_UP : DIR_DOWN);
			RTLLIB_DEBUG(RTLLIB_DL_TS, "to add Ts\n");
			if (!list_empty(pUnusedList)) {
				(*ppTS) = list_entry(pUnusedList->next,
					  struct ts_common_info, List);
				list_del_init(&(*ppTS)->List);
				if (TxRxSelect == TX_DIR) {
					struct tx_ts_record *tmp =
						container_of(*ppTS,
						struct tx_ts_record,
						TsCommonInfo);
					ResetTxTsEntry(tmp);
				} else {
					struct rx_ts_record *tmp =
						 container_of(*ppTS,
						 struct rx_ts_record,
						 TsCommonInfo);
					ResetRxTsEntry(tmp);
				}

				RTLLIB_DEBUG(RTLLIB_DL_TS, "to init current TS"
					     ", UP:%d, Dir:%d, addr: %pM"
					     " ppTs=%p\n", UP, Dir,
					      Addr, *ppTS);
				pTSInfo->field.ucTrafficType = 0;
				pTSInfo->field.ucTSID = UP;
				pTSInfo->field.ucDirection = Dir;
				pTSInfo->field.ucAccessPolicy = 1;
				pTSInfo->field.ucAggregation = 0;
				pTSInfo->field.ucPSB = 0;
				pTSInfo->field.ucUP = UP;
				pTSInfo->field.ucTSInfoAckPolicy = 0;
				pTSInfo->field.ucSchedule = 0;

				MakeTSEntry(*ppTS, Addr, &TSpec, NULL, 0, 0);
				AdmitTS(ieee, *ppTS, 0);
				list_add_tail(&((*ppTS)->List), pAddmitList);

				return true;
			} else {
				RTLLIB_DEBUG(RTLLIB_DL_ERR, "ERR!!in function "
					     "%s() There is not enough dir=%d"
					     "(0=up down=1) TS record to be "
					     "used!!", __func__, Dir);
				return false;
			}
		}
	}
}

static void RemoveTsEntry(struct rtllib_device *ieee, struct ts_common_info *pTs,
			  enum tr_select TxRxSelect)
=======
	if (*ppTS != NULL)
		return true;

	if (!bAddNewTs) {
		netdev_dbg(ieee->dev, "add new TS failed(tid:%d)\n", UP);
		return false;
	}

	pUnusedList = (TxRxSelect == TX_DIR) ?
				(&ieee->Tx_TS_Unused_List) :
				(&ieee->Rx_TS_Unused_List);

	pAddmitList = (TxRxSelect == TX_DIR) ?
				(&ieee->Tx_TS_Admit_List) :
				(&ieee->Rx_TS_Admit_List);

	Dir = (ieee->iw_mode == IW_MODE_MASTER) ?
				((TxRxSelect == TX_DIR) ? DIR_DOWN : DIR_UP) :
				((TxRxSelect == TX_DIR) ? DIR_UP : DIR_DOWN);

	if (!list_empty(pUnusedList)) {
		(*ppTS) = list_entry(pUnusedList->next,
			  struct ts_common_info, List);
		list_del_init(&(*ppTS)->List);
		if (TxRxSelect == TX_DIR) {
			struct tx_ts_record *tmp =
				container_of(*ppTS,
				struct tx_ts_record,
				TsCommonInfo);
			ResetTxTsEntry(tmp);
		} else {
			struct rx_ts_record *tmp =
				 container_of(*ppTS,
				 struct rx_ts_record,
				 TsCommonInfo);
			ResetRxTsEntry(tmp);
		}

		netdev_dbg(ieee->dev,
			   "to init current TS, UP:%d, Dir:%d, addr: %pM ppTs=%p\n",
			   UP, Dir, Addr, *ppTS);
		pTSInfo->field.ucTrafficType = 0;
		pTSInfo->field.ucTSID = UP;
		pTSInfo->field.ucDirection = Dir;
		pTSInfo->field.ucAccessPolicy = 1;
		pTSInfo->field.ucAggregation = 0;
		pTSInfo->field.ucPSB = 0;
		pTSInfo->field.ucUP = UP;
		pTSInfo->field.ucTSInfoAckPolicy = 0;
		pTSInfo->field.ucSchedule = 0;

		MakeTSEntry(*ppTS, Addr, &TSpec, NULL, 0, 0);
		AdmitTS(ieee, *ppTS, 0);
		list_add_tail(&((*ppTS)->List), pAddmitList);

		return true;
	}

	netdev_warn(ieee->dev,
		    "There is not enough dir=%d(0=up down=1) TS record to be used!",
		    Dir);
	return false;
}

static void RemoveTsEntry(struct rtllib_device *ieee,
			  struct ts_common_info *pTs, enum tr_select TxRxSelect)
>>>>>>> v4.9.227
{
	del_timer_sync(&pTs->SetupTimer);
	del_timer_sync(&pTs->InactTimer);
	TsInitDelBA(ieee, pTs, TxRxSelect);

	if (TxRxSelect == RX_DIR) {
		struct rx_reorder_entry *pRxReorderEntry;
		struct rx_ts_record *pRxTS = (struct rx_ts_record *)pTs;

		if (timer_pending(&pRxTS->RxPktPendingTimer))
			del_timer_sync(&pRxTS->RxPktPendingTimer);

		while (!list_empty(&pRxTS->RxPendingPktList)) {
			pRxReorderEntry = (struct rx_reorder_entry *)
					list_entry(pRxTS->RxPendingPktList.prev,
					struct rx_reorder_entry, List);
<<<<<<< HEAD
			RTLLIB_DEBUG(RTLLIB_DL_REORDER, "%s(): Delete SeqNum "
				     "%d!\n", __func__,
				     pRxReorderEntry->SeqNum);
=======
			netdev_dbg(ieee->dev,  "%s(): Delete SeqNum %d!\n",
				   __func__, pRxReorderEntry->SeqNum);
>>>>>>> v4.9.227
			list_del_init(&pRxReorderEntry->List);
			{
				int i = 0;
				struct rtllib_rxb *prxb = pRxReorderEntry->prxb;

				if (unlikely(!prxb))
					return;
				for (i = 0; i < prxb->nr_subframes; i++)
					dev_kfree_skb(prxb->subframes[i]);
				kfree(prxb);
				prxb = NULL;
			}
			list_add_tail(&pRxReorderEntry->List,
				      &ieee->RxReorder_Unused_List);
		}
	} else {
		struct tx_ts_record *pTxTS = (struct tx_ts_record *)pTs;

		del_timer_sync(&pTxTS->TsAddBaTimer);
	}
}

void RemovePeerTS(struct rtllib_device *ieee, u8 *Addr)
{
	struct ts_common_info *pTS, *pTmpTS;

<<<<<<< HEAD
	printk(KERN_INFO "===========>RemovePeerTS, %pM\n", Addr);
=======
	netdev_info(ieee->dev, "===========>RemovePeerTS, %pM\n", Addr);
>>>>>>> v4.9.227

	list_for_each_entry_safe(pTS, pTmpTS, &ieee->Tx_TS_Pending_List, List) {
		if (memcmp(pTS->Addr, Addr, 6) == 0) {
			RemoveTsEntry(ieee, pTS, TX_DIR);
			list_del_init(&pTS->List);
			list_add_tail(&pTS->List, &ieee->Tx_TS_Unused_List);
		}
	}

	list_for_each_entry_safe(pTS, pTmpTS, &ieee->Tx_TS_Admit_List, List) {
		if (memcmp(pTS->Addr, Addr, 6) == 0) {
<<<<<<< HEAD
			printk(KERN_INFO "====>remove Tx_TS_admin_list\n");
=======
			netdev_info(ieee->dev,
				    "====>remove Tx_TS_admin_list\n");
>>>>>>> v4.9.227
			RemoveTsEntry(ieee, pTS, TX_DIR);
			list_del_init(&pTS->List);
			list_add_tail(&pTS->List, &ieee->Tx_TS_Unused_List);
		}
	}

	list_for_each_entry_safe(pTS, pTmpTS, &ieee->Rx_TS_Pending_List, List) {
		if (memcmp(pTS->Addr, Addr, 6) == 0) {
			RemoveTsEntry(ieee, pTS, RX_DIR);
			list_del_init(&pTS->List);
			list_add_tail(&pTS->List, &ieee->Rx_TS_Unused_List);
		}
	}

	list_for_each_entry_safe(pTS, pTmpTS, &ieee->Rx_TS_Admit_List, List) {
		if (memcmp(pTS->Addr, Addr, 6) == 0) {
			RemoveTsEntry(ieee, pTS, RX_DIR);
			list_del_init(&pTS->List);
			list_add_tail(&pTS->List, &ieee->Rx_TS_Unused_List);
		}
	}
}
EXPORT_SYMBOL(RemovePeerTS);

void RemoveAllTS(struct rtllib_device *ieee)
{
	struct ts_common_info *pTS, *pTmpTS;

	list_for_each_entry_safe(pTS, pTmpTS, &ieee->Tx_TS_Pending_List, List) {
		RemoveTsEntry(ieee, pTS, TX_DIR);
		list_del_init(&pTS->List);
		list_add_tail(&pTS->List, &ieee->Tx_TS_Unused_List);
	}

	list_for_each_entry_safe(pTS, pTmpTS, &ieee->Tx_TS_Admit_List, List) {
		RemoveTsEntry(ieee, pTS, TX_DIR);
		list_del_init(&pTS->List);
		list_add_tail(&pTS->List, &ieee->Tx_TS_Unused_List);
	}

	list_for_each_entry_safe(pTS, pTmpTS, &ieee->Rx_TS_Pending_List, List) {
		RemoveTsEntry(ieee, pTS, RX_DIR);
		list_del_init(&pTS->List);
		list_add_tail(&pTS->List, &ieee->Rx_TS_Unused_List);
	}

	list_for_each_entry_safe(pTS, pTmpTS, &ieee->Rx_TS_Admit_List, List) {
		RemoveTsEntry(ieee, pTS, RX_DIR);
		list_del_init(&pTS->List);
		list_add_tail(&pTS->List, &ieee->Rx_TS_Unused_List);
	}
}

void TsStartAddBaProcess(struct rtllib_device *ieee, struct tx_ts_record *pTxTS)
{
	if (pTxTS->bAddBaReqInProgress == false) {
		pTxTS->bAddBaReqInProgress = true;

		if (pTxTS->bAddBaReqDelayed) {
<<<<<<< HEAD
			RTLLIB_DEBUG(RTLLIB_DL_BA, "TsStartAddBaProcess(): "
				     "Delayed Start ADDBA after 60 sec!!\n");
			mod_timer(&pTxTS->TsAddBaTimer, jiffies +
				  MSECS(TS_ADDBA_DELAY));
		} else {
			RTLLIB_DEBUG(RTLLIB_DL_BA, "TsStartAddBaProcess(): "
				     "Immediately Start ADDBA now!!\n");
			mod_timer(&pTxTS->TsAddBaTimer, jiffies+10);
		}
	} else
		RTLLIB_DEBUG(RTLLIB_DL_BA, "%s()==>BA timer is already added\n",
			     __func__);
=======
			netdev_dbg(ieee->dev, "Start ADDBA after 60 sec!!\n");
			mod_timer(&pTxTS->TsAddBaTimer, jiffies +
				  msecs_to_jiffies(TS_ADDBA_DELAY));
		} else {
			netdev_dbg(ieee->dev, "Immediately Start ADDBA\n");
			mod_timer(&pTxTS->TsAddBaTimer, jiffies+10);
		}
	} else
		netdev_dbg(ieee->dev, "BA timer is already added\n");
>>>>>>> v4.9.227
}
