/*
 * Copyright (c) 1996, 2003 VIA Networking Technologies, Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *
 * File: mac.c
 *
 * Purpose:  MAC routines
 *
 * Author: Tevin Chen
 *
 * Date: May 21, 1996
 *
 * Functions:
<<<<<<< HEAD
 *      MACvReadAllRegs - Read All MAC Registers to buffer
 *      MACbIsRegBitsOn - Test if All test Bits On
 *      MACbIsRegBitsOff - Test if All test Bits Off
 *      MACbIsIntDisable - Test if MAC interrupt disable
 *      MACbyReadMultiAddr - Read Multicast Address Mask Pattern
 *      MACvWriteMultiAddr - Write Multicast Address Mask Pattern
 *      MACvSetMultiAddrByHash - Set Multicast Address Mask by Hash value
 *      MACvResetMultiAddrByHash - Clear Multicast Address Mask by Hash value
 *      MACvSetRxThreshold - Set Rx Threshold value
 *      MACvGetRxThreshold - Get Rx Threshold value
 *      MACvSetTxThreshold - Set Tx Threshold value
 *      MACvGetTxThreshold - Get Tx Threshold value
 *      MACvSetDmaLength - Set Dma Length value
 *      MACvGetDmaLength - Get Dma Length value
 *      MACvSetShortRetryLimit - Set 802.11 Short Retry limit
 *      MACvGetShortRetryLimit - Get 802.11 Short Retry limit
 *      MACvSetLongRetryLimit - Set 802.11 Long Retry limit
 *      MACvGetLongRetryLimit - Get 802.11 Long Retry limit
 *      MACvSetLoopbackMode - Set MAC Loopback Mode
 *      MACbIsInLoopbackMode - Test if MAC in Loopback mode
 *      MACvSetPacketFilter - Set MAC Address Filter
 *      MACvSaveContext - Save Context of MAC Registers
 *      MACvRestoreContext - Restore Context of MAC Registers
 *      MACbCompareContext - Compare if values of MAC Registers same as Context
=======
 *      MACbIsRegBitsOn - Test if All test Bits On
 *      MACbIsRegBitsOff - Test if All test Bits Off
 *      MACbIsIntDisable - Test if MAC interrupt disable
 *      MACvSetShortRetryLimit - Set 802.11 Short Retry limit
 *      MACvSetLongRetryLimit - Set 802.11 Long Retry limit
 *      MACvSetLoopbackMode - Set MAC Loopback Mode
 *      MACvSaveContext - Save Context of MAC Registers
 *      MACvRestoreContext - Restore Context of MAC Registers
>>>>>>> v4.9.227
 *      MACbSoftwareReset - Software Reset MAC
 *      MACbSafeRxOff - Turn Off MAC Rx
 *      MACbSafeTxOff - Turn Off MAC Tx
 *      MACbSafeStop - Stop MAC function
 *      MACbShutdown - Shut down MAC
 *      MACvInitialize - Initialize MAC
 *      MACvSetCurrRxDescAddr - Set Rx Descriptors Address
 *      MACvSetCurrTx0DescAddr - Set Tx0 Descriptors Address
 *      MACvSetCurrTx1DescAddr - Set Tx1 Descriptors Address
 *      MACvTimer0MicroSDelay - Micro Second Delay Loop by MAC
 *
 * Revision History:
 *      08-22-2003 Kyle Hsu     :  Porting MAC functions from sim53
<<<<<<< HEAD
 *      09-03-2003 Bryan YC Fan :  Add MACvClearBusSusInd()& MACvEnableBusSusEn()
=======
 *      09-03-2003 Bryan YC Fan :  Add MACvClearBusSusInd()&
 *				   MACvEnableBusSusEn()
>>>>>>> v4.9.227
 *      09-18-2003 Jerry Chen   :  Add MACvSetKeyEntry & MACvDisableKeyEntry
 *
 */

#include "tmacro.h"
<<<<<<< HEAD
#include "tether.h"
#include "mac.h"

unsigned short TxRate_iwconfig;//2008-5-8 <add> by chester
/*---------------------  Static Classes  ----------------------------*/

/*---------------------  Static Variables  --------------------------*/

/*---------------------  Static Functions  --------------------------*/

/*---------------------  Export Variables  --------------------------*/

/*---------------------  Export Functions  --------------------------*/

/*
 * Description:
 *      Read All MAC Registers to buffer
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *  Out:
 *      pbyMacRegs  - buffer to read
 *
 * Return Value: none
 *
 */
void MACvReadAllRegs(void __iomem *dwIoBase, unsigned char *pbyMacRegs)
{
	int ii;

	// read page0 register
	for (ii = 0; ii < MAC_MAX_CONTEXT_SIZE_PAGE0; ii++) {
		VNSvInPortB(dwIoBase + ii, pbyMacRegs);
		pbyMacRegs++;
	}

	MACvSelectPage1(dwIoBase);

	// read page1 register
	for (ii = 0; ii < MAC_MAX_CONTEXT_SIZE_PAGE1; ii++) {
		VNSvInPortB(dwIoBase + ii, pbyMacRegs);
		pbyMacRegs++;
	}

	MACvSelectPage0(dwIoBase);
}

=======
#include "mac.h"

>>>>>>> v4.9.227
/*
 * Description:
 *      Test if all test bits on
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
=======
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *      byRegOfs    - Offset of MAC Register
 *      byTestBits  - Test bits
 *  Out:
 *      none
 *
 * Return Value: true if all test bits On; otherwise false
 *
 */
<<<<<<< HEAD
bool MACbIsRegBitsOn(void __iomem *dwIoBase, unsigned char byRegOfs, unsigned char byTestBits)
{
	unsigned char byData;

	VNSvInPortB(dwIoBase + byRegOfs, &byData);
	return (byData & byTestBits) == byTestBits;
=======
bool MACbIsRegBitsOn(struct vnt_private *priv, unsigned char byRegOfs,
		     unsigned char byTestBits)
{
	void __iomem *io_base = priv->PortOffset;

	return (ioread8(io_base + byRegOfs) & byTestBits) == byTestBits;
>>>>>>> v4.9.227
}

/*
 * Description:
 *      Test if all test bits off
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
=======
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *      byRegOfs    - Offset of MAC Register
 *      byTestBits  - Test bits
 *  Out:
 *      none
 *
 * Return Value: true if all test bits Off; otherwise false
 *
 */
<<<<<<< HEAD
bool MACbIsRegBitsOff(void __iomem *dwIoBase, unsigned char byRegOfs, unsigned char byTestBits)
{
	unsigned char byData;

	VNSvInPortB(dwIoBase + byRegOfs, &byData);
	return !(byData & byTestBits);
=======
bool MACbIsRegBitsOff(struct vnt_private *priv, unsigned char byRegOfs,
		      unsigned char byTestBits)
{
	void __iomem *io_base = priv->PortOffset;

	return !(ioread8(io_base + byRegOfs) & byTestBits);
>>>>>>> v4.9.227
}

/*
 * Description:
 *      Test if MAC interrupt disable
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
=======
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: true if interrupt is disable; otherwise false
 *
 */
<<<<<<< HEAD
bool MACbIsIntDisable(void __iomem *dwIoBase)
{
	unsigned long dwData;

	VNSvInPortD(dwIoBase + MAC_REG_IMR, &dwData);
	if (dwData != 0)
=======
bool MACbIsIntDisable(struct vnt_private *priv)
{
	void __iomem *io_base = priv->PortOffset;

	if (ioread32(io_base + MAC_REG_IMR))
>>>>>>> v4.9.227
		return false;

	return true;
}

/*
 * Description:
<<<<<<< HEAD
 *      Read MAC Multicast Address Mask
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *      uByteidx    - Index of Mask
 *  Out:
 *      none
 *
 * Return Value: Mask Value read
 *
 */
unsigned char MACbyReadMultiAddr(void __iomem *dwIoBase, unsigned int uByteIdx)
{
	unsigned char byData;

	MACvSelectPage1(dwIoBase);
	VNSvInPortB(dwIoBase + MAC_REG_MAR0 + uByteIdx, &byData);
	MACvSelectPage0(dwIoBase);
	return byData;
}

/*
 * Description:
 *      Write MAC Multicast Address Mask
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *      uByteidx    - Index of Mask
 *      byData      - Mask Value to write
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void MACvWriteMultiAddr(void __iomem *dwIoBase, unsigned int uByteIdx, unsigned char byData)
{
	MACvSelectPage1(dwIoBase);
	VNSvOutPortB(dwIoBase + MAC_REG_MAR0 + uByteIdx, byData);
	MACvSelectPage0(dwIoBase);
}

/*
 * Description:
 *      Set this hash index into multicast address register bit
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *      byHashIdx   - Hash index to set
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void MACvSetMultiAddrByHash(void __iomem *dwIoBase, unsigned char byHashIdx)
{
	unsigned int uByteIdx;
	unsigned char byBitMask;
	unsigned char byOrgValue;

	// calculate byte position
	uByteIdx = byHashIdx / 8;
	ASSERT(uByteIdx < 8);
	// calculate bit position
	byBitMask = 1;
	byBitMask <<= (byHashIdx % 8);
	// turn on the bit
	byOrgValue = MACbyReadMultiAddr(dwIoBase, uByteIdx);
	MACvWriteMultiAddr(dwIoBase, uByteIdx, (unsigned char)(byOrgValue | byBitMask));
}

/*
 * Description:
 *      Reset this hash index into multicast address register bit
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *      byHashIdx   - Hash index to clear
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void MACvResetMultiAddrByHash(void __iomem *dwIoBase, unsigned char byHashIdx)
{
	unsigned int uByteIdx;
	unsigned char byBitMask;
	unsigned char byOrgValue;

	// calculate byte position
	uByteIdx = byHashIdx / 8;
	ASSERT(uByteIdx < 8);
	// calculate bit position
	byBitMask = 1;
	byBitMask <<= (byHashIdx % 8);
	// turn off the bit
	byOrgValue = MACbyReadMultiAddr(dwIoBase, uByteIdx);
	MACvWriteMultiAddr(dwIoBase, uByteIdx, (unsigned char)(byOrgValue & (~byBitMask)));
}

/*
 * Description:
 *      Set Rx Threshold
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *      byThreshold - Threshold Value
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void MACvSetRxThreshold(void __iomem *dwIoBase, unsigned char byThreshold)
{
	unsigned char byOrgValue;

	ASSERT(byThreshold < 4);

	// set FCR0
	VNSvInPortB(dwIoBase + MAC_REG_FCR0, &byOrgValue);
	byOrgValue = (byOrgValue & 0xCF) | (byThreshold << 4);
	VNSvOutPortB(dwIoBase + MAC_REG_FCR0, byOrgValue);
}

/*
 * Description:
 *      Get Rx Threshold
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *  Out:
 *      pbyThreshold- Threshold Value Get
 *
 * Return Value: none
 *
 */
void MACvGetRxThreshold(void __iomem *dwIoBase, unsigned char *pbyThreshold)
{
	// get FCR0
	VNSvInPortB(dwIoBase + MAC_REG_FCR0, pbyThreshold);
	*pbyThreshold = (*pbyThreshold >> 4) & 0x03;
}

/*
 * Description:
 *      Set Tx Threshold
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *      byThreshold - Threshold Value
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void MACvSetTxThreshold(void __iomem *dwIoBase, unsigned char byThreshold)
{
	unsigned char byOrgValue;

	ASSERT(byThreshold < 4);

	// set FCR0
	VNSvInPortB(dwIoBase + MAC_REG_FCR0, &byOrgValue);
	byOrgValue = (byOrgValue & 0xF3) | (byThreshold << 2);
	VNSvOutPortB(dwIoBase + MAC_REG_FCR0, byOrgValue);
}

/*
 * Description:
 *      Get Tx Threshold
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *  Out:
 *      pbyThreshold- Threshold Value Get
 *
 * Return Value: none
 *
 */
void MACvGetTxThreshold(void __iomem *dwIoBase, unsigned char *pbyThreshold)
{
	// get FCR0
	VNSvInPortB(dwIoBase + MAC_REG_FCR0, pbyThreshold);
	*pbyThreshold = (*pbyThreshold >> 2) & 0x03;
}

/*
 * Description:
 *      Set Dma Length
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *      byDmaLength - Dma Length Value
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void MACvSetDmaLength(void __iomem *dwIoBase, unsigned char byDmaLength)
{
	unsigned char byOrgValue;

	ASSERT(byDmaLength < 4);

	// set FCR0
	VNSvInPortB(dwIoBase + MAC_REG_FCR0, &byOrgValue);
	byOrgValue = (byOrgValue & 0xFC) | byDmaLength;
	VNSvOutPortB(dwIoBase + MAC_REG_FCR0, byOrgValue);
}

/*
 * Description:
 *      Get Dma Length
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *  Out:
 *      pbyDmaLength- Dma Length Value Get
 *
 * Return Value: none
 *
 */
void MACvGetDmaLength(void __iomem *dwIoBase, unsigned char *pbyDmaLength)
{
	// get FCR0
	VNSvInPortB(dwIoBase + MAC_REG_FCR0, pbyDmaLength);
	*pbyDmaLength &= 0x03;
}

/*
 * Description:
=======
>>>>>>> v4.9.227
 *      Set 802.11 Short Retry Limit
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
=======
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *      byRetryLimit- Retry Limit
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
void MACvSetShortRetryLimit(void __iomem *dwIoBase, unsigned char byRetryLimit)
{
	// set SRT
	VNSvOutPortB(dwIoBase + MAC_REG_SRT, byRetryLimit);
}

/*
 * Description:
 *      Get 802.11 Short Retry Limit
 *
 * Parameters:
 *  In:
 *      dwIoBase        - Base Address for MAC
 *  Out:
 *      pbyRetryLimit   - Retry Limit Get
 *
 * Return Value: none
 *
 */
void MACvGetShortRetryLimit(void __iomem *dwIoBase, unsigned char *pbyRetryLimit)
{
	// get SRT
	VNSvInPortB(dwIoBase + MAC_REG_SRT, pbyRetryLimit);
}
=======
void MACvSetShortRetryLimit(struct vnt_private *priv,
			    unsigned char byRetryLimit)
{
	void __iomem *io_base = priv->PortOffset;
	/* set SRT */
	iowrite8(byRetryLimit, io_base + MAC_REG_SRT);
}

>>>>>>> v4.9.227

/*
 * Description:
 *      Set 802.11 Long Retry Limit
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
=======
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *      byRetryLimit- Retry Limit
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
void MACvSetLongRetryLimit(void __iomem *dwIoBase, unsigned char byRetryLimit)
{
	// set LRT
	VNSvOutPortB(dwIoBase + MAC_REG_LRT, byRetryLimit);
}

/*
 * Description:
 *      Get 802.11 Long Retry Limit
 *
 * Parameters:
 *  In:
 *      dwIoBase        - Base Address for MAC
 *  Out:
 *      pbyRetryLimit   - Retry Limit Get
 *
 * Return Value: none
 *
 */
void MACvGetLongRetryLimit(void __iomem *dwIoBase, unsigned char *pbyRetryLimit)
{
	// get LRT
	VNSvInPortB(dwIoBase + MAC_REG_LRT, pbyRetryLimit);
=======
void MACvSetLongRetryLimit(struct vnt_private *priv,
			   unsigned char byRetryLimit)
{
	void __iomem *io_base = priv->PortOffset;
	/* set LRT */
	iowrite8(byRetryLimit, io_base + MAC_REG_LRT);
>>>>>>> v4.9.227
}

/*
 * Description:
 *      Set MAC Loopback mode
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase        - Base Address for MAC
=======
 *      io_base        - Base Address for MAC
>>>>>>> v4.9.227
 *      byLoopbackMode  - Loopback Mode
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
void MACvSetLoopbackMode(void __iomem *dwIoBase, unsigned char byLoopbackMode)
{
	unsigned char byOrgValue;

	ASSERT(byLoopbackMode < 3);
	byLoopbackMode <<= 6;
	// set TCR
	VNSvInPortB(dwIoBase + MAC_REG_TEST, &byOrgValue);
	byOrgValue = byOrgValue & 0x3F;
	byOrgValue = byOrgValue | byLoopbackMode;
	VNSvOutPortB(dwIoBase + MAC_REG_TEST, byOrgValue);
}

/*
 * Description:
 *      Test if MAC in Loopback mode
 *
 * Parameters:
 *  In:
 *      dwIoBase        - Base Address for MAC
 *  Out:
 *      none
 *
 * Return Value: true if in Loopback mode; otherwise false
 *
 */
bool MACbIsInLoopbackMode(void __iomem *dwIoBase)
{
	unsigned char byOrgValue;

	VNSvInPortB(dwIoBase + MAC_REG_TEST, &byOrgValue);
	if (byOrgValue & (TEST_LBINT | TEST_LBEXT))
		return true;
	return false;
}

/*
 * Description:
 *      Set MAC Address filter
 *
 * Parameters:
 *  In:
 *      dwIoBase        - Base Address for MAC
 *      wFilterType     - Filter Type
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void MACvSetPacketFilter(void __iomem *dwIoBase, unsigned short wFilterType)
{
	unsigned char byOldRCR;
	unsigned char byNewRCR = 0;

	// if only in DIRECTED mode, multicast-address will set to zero,
	// but if other mode exist (e.g. PROMISCUOUS), multicast-address
	// will be open
	if (wFilterType & PKT_TYPE_DIRECTED) {
		// set multicast address to accept none
		MACvSelectPage1(dwIoBase);
		VNSvOutPortD(dwIoBase + MAC_REG_MAR0, 0L);
		VNSvOutPortD(dwIoBase + MAC_REG_MAR0 + sizeof(unsigned long), 0L);
		MACvSelectPage0(dwIoBase);
	}

	if (wFilterType & (PKT_TYPE_PROMISCUOUS | PKT_TYPE_ALL_MULTICAST)) {
		// set multicast address to accept all
		MACvSelectPage1(dwIoBase);
		VNSvOutPortD(dwIoBase + MAC_REG_MAR0, 0xFFFFFFFFL);
		VNSvOutPortD(dwIoBase + MAC_REG_MAR0 + sizeof(unsigned long), 0xFFFFFFFFL);
		MACvSelectPage0(dwIoBase);
	}

	if (wFilterType & PKT_TYPE_PROMISCUOUS) {
		byNewRCR |= (RCR_RXALLTYPE | RCR_UNICAST | RCR_MULTICAST | RCR_BROADCAST);

		byNewRCR &= ~RCR_BSSID;
	}

	if (wFilterType & (PKT_TYPE_ALL_MULTICAST | PKT_TYPE_MULTICAST))
		byNewRCR |= RCR_MULTICAST;

	if (wFilterType & PKT_TYPE_BROADCAST)
		byNewRCR |= RCR_BROADCAST;

	if (wFilterType & PKT_TYPE_ERROR_CRC)
		byNewRCR |= RCR_ERRCRC;

	VNSvInPortB(dwIoBase + MAC_REG_RCR,  &byOldRCR);
	if (byNewRCR != byOldRCR) {
		// Modify the Receive Command Register
		VNSvOutPortB(dwIoBase + MAC_REG_RCR, byNewRCR);
	}
=======
void MACvSetLoopbackMode(struct vnt_private *priv, unsigned char byLoopbackMode)
{
	void __iomem *io_base = priv->PortOffset;

	byLoopbackMode <<= 6;
	/* set TCR */
	iowrite8((ioread8(io_base + MAC_REG_TEST) & 0x3f) | byLoopbackMode,
		 io_base + MAC_REG_TEST);
>>>>>>> v4.9.227
}

/*
 * Description:
 *      Save MAC registers to context buffer
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
 *  Out:
 *      pbyCxtBuf   - Context buffer
=======
 *      io_base    - Base Address for MAC
 *  Out:
 *      cxt_buf   - Context buffer
>>>>>>> v4.9.227
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
void MACvSaveContext(void __iomem *dwIoBase, unsigned char *pbyCxtBuf)
{
	int         ii;

	// read page0 register
	for (ii = 0; ii < MAC_MAX_CONTEXT_SIZE_PAGE0; ii++)
		VNSvInPortB((dwIoBase + ii), (pbyCxtBuf + ii));

	MACvSelectPage1(dwIoBase);

	// read page1 register
	for (ii = 0; ii < MAC_MAX_CONTEXT_SIZE_PAGE1; ii++)
		VNSvInPortB((dwIoBase + ii), (pbyCxtBuf + MAC_MAX_CONTEXT_SIZE_PAGE0 + ii));

	MACvSelectPage0(dwIoBase);
=======
void MACvSaveContext(struct vnt_private *priv, unsigned char *cxt_buf)
{
	void __iomem *io_base = priv->PortOffset;

	/* read page0 register */
	memcpy_fromio(cxt_buf, io_base, MAC_MAX_CONTEXT_SIZE_PAGE0);

	MACvSelectPage1(io_base);

	/* read page1 register */
	memcpy_fromio(cxt_buf + MAC_MAX_CONTEXT_SIZE_PAGE0, io_base,
		      MAC_MAX_CONTEXT_SIZE_PAGE1);

	MACvSelectPage0(io_base);
>>>>>>> v4.9.227
}

/*
 * Description:
 *      Restore MAC registers from context buffer
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
 *      pbyCxtBuf   - Context buffer
=======
 *      io_base    - Base Address for MAC
 *      cxt_buf   - Context buffer
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
void MACvRestoreContext(void __iomem *dwIoBase, unsigned char *pbyCxtBuf)
{
	int         ii;

	MACvSelectPage1(dwIoBase);
	// restore page1
	for (ii = 0; ii < MAC_MAX_CONTEXT_SIZE_PAGE1; ii++)
		VNSvOutPortB((dwIoBase + ii), *(pbyCxtBuf + MAC_MAX_CONTEXT_SIZE_PAGE0 + ii));

	MACvSelectPage0(dwIoBase);

	// restore RCR,TCR,IMR...
	for (ii = MAC_REG_RCR; ii < MAC_REG_ISR; ii++)
		VNSvOutPortB(dwIoBase + ii, *(pbyCxtBuf + ii));

	// restore MAC Config.
	for (ii = MAC_REG_LRT; ii < MAC_REG_PAGE1SEL; ii++)
		VNSvOutPortB(dwIoBase + ii, *(pbyCxtBuf + ii));

	VNSvOutPortB(dwIoBase + MAC_REG_CFG, *(pbyCxtBuf + MAC_REG_CFG));

	// restore PS Config.
	for (ii = MAC_REG_PSCFG; ii < MAC_REG_BBREGCTL; ii++)
		VNSvOutPortB(dwIoBase + ii, *(pbyCxtBuf + ii));

	// restore CURR_RX_DESC_ADDR, CURR_TX_DESC_ADDR
	VNSvOutPortD(dwIoBase + MAC_REG_TXDMAPTR0, *(unsigned long *)(pbyCxtBuf + MAC_REG_TXDMAPTR0));
	VNSvOutPortD(dwIoBase + MAC_REG_AC0DMAPTR, *(unsigned long *)(pbyCxtBuf + MAC_REG_AC0DMAPTR));
	VNSvOutPortD(dwIoBase + MAC_REG_BCNDMAPTR, *(unsigned long *)(pbyCxtBuf + MAC_REG_BCNDMAPTR));

	VNSvOutPortD(dwIoBase + MAC_REG_RXDMAPTR0, *(unsigned long *)(pbyCxtBuf + MAC_REG_RXDMAPTR0));

	VNSvOutPortD(dwIoBase + MAC_REG_RXDMAPTR1, *(unsigned long *)(pbyCxtBuf + MAC_REG_RXDMAPTR1));
}

/*
 * Description:
 *      Compare if MAC registers same as context buffer
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *      pbyCxtBuf   - Context buffer
 *  Out:
 *      none
 *
 * Return Value: true if all values are the same; otherwise false
 *
 */
bool MACbCompareContext(void __iomem *dwIoBase, unsigned char *pbyCxtBuf)
{
	unsigned long dwData;

	// compare MAC context to determine if this is a power lost init,
	// return true for power remaining init, return false for power lost init

	// compare CURR_RX_DESC_ADDR, CURR_TX_DESC_ADDR
	VNSvInPortD(dwIoBase + MAC_REG_TXDMAPTR0, &dwData);
	if (dwData != *(unsigned long *)(pbyCxtBuf + MAC_REG_TXDMAPTR0))
		return false;

	VNSvInPortD(dwIoBase + MAC_REG_AC0DMAPTR, &dwData);
	if (dwData != *(unsigned long *)(pbyCxtBuf + MAC_REG_AC0DMAPTR))
		return false;

	VNSvInPortD(dwIoBase + MAC_REG_RXDMAPTR0, &dwData);
	if (dwData != *(unsigned long *)(pbyCxtBuf + MAC_REG_RXDMAPTR0))
		return false;

	VNSvInPortD(dwIoBase + MAC_REG_RXDMAPTR1, &dwData);
	if (dwData != *(unsigned long *)(pbyCxtBuf + MAC_REG_RXDMAPTR1))
		return false;

	return true;
=======
void MACvRestoreContext(struct vnt_private *priv, unsigned char *cxt_buf)
{
	void __iomem *io_base = priv->PortOffset;

	MACvSelectPage1(io_base);
	/* restore page1 */
	memcpy_toio(io_base, cxt_buf + MAC_MAX_CONTEXT_SIZE_PAGE0,
		    MAC_MAX_CONTEXT_SIZE_PAGE1);

	MACvSelectPage0(io_base);

	/* restore RCR,TCR,IMR... */
	memcpy_toio(io_base + MAC_REG_RCR, cxt_buf + MAC_REG_RCR,
		    MAC_REG_ISR - MAC_REG_RCR);

	/* restore MAC Config. */
	memcpy_toio(io_base + MAC_REG_LRT, cxt_buf + MAC_REG_LRT,
		    MAC_REG_PAGE1SEL - MAC_REG_LRT);

	iowrite8(*(cxt_buf + MAC_REG_CFG), io_base + MAC_REG_CFG);

	/* restore PS Config. */
	memcpy_toio(io_base + MAC_REG_PSCFG, cxt_buf + MAC_REG_PSCFG,
		    MAC_REG_BBREGCTL - MAC_REG_PSCFG);

	/* restore CURR_RX_DESC_ADDR, CURR_TX_DESC_ADDR */
	iowrite32(*(u32 *)(cxt_buf + MAC_REG_TXDMAPTR0),
		  io_base + MAC_REG_TXDMAPTR0);
	iowrite32(*(u32 *)(cxt_buf + MAC_REG_AC0DMAPTR),
		  io_base + MAC_REG_AC0DMAPTR);
	iowrite32(*(u32 *)(cxt_buf + MAC_REG_BCNDMAPTR),
		  io_base + MAC_REG_BCNDMAPTR);
	iowrite32(*(u32 *)(cxt_buf + MAC_REG_RXDMAPTR0),
		  io_base + MAC_REG_RXDMAPTR0);
	iowrite32(*(u32 *)(cxt_buf + MAC_REG_RXDMAPTR1),
		  io_base + MAC_REG_RXDMAPTR1);
>>>>>>> v4.9.227
}

/*
 * Description:
 *      Software Reset MAC
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
=======
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: true if Reset Success; otherwise false
 *
 */
<<<<<<< HEAD
bool MACbSoftwareReset(void __iomem *dwIoBase)
{
	unsigned char byData;
	unsigned short ww;

	// turn on HOSTCR_SOFTRST, just write 0x01 to reset
	VNSvOutPortB(dwIoBase + MAC_REG_HOSTCR, 0x01);

	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortB(dwIoBase + MAC_REG_HOSTCR, &byData);
		if (!(byData & HOSTCR_SOFTRST))
=======
bool MACbSoftwareReset(struct vnt_private *priv)
{
	void __iomem *io_base = priv->PortOffset;
	unsigned short ww;

	/* turn on HOSTCR_SOFTRST, just write 0x01 to reset */
	iowrite8(0x01, io_base + MAC_REG_HOSTCR);

	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		if (!(ioread8(io_base + MAC_REG_HOSTCR) & HOSTCR_SOFTRST))
>>>>>>> v4.9.227
			break;
	}
	if (ww == W_MAX_TIMEOUT)
		return false;
	return true;
}

/*
 * Description:
<<<<<<< HEAD
 *      save some important register's value, then do reset, then restore register's value
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
=======
 *      save some important register's value, then do reset, then restore
 *	register's value
 *
 * Parameters:
 *  In:
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: true if success; otherwise false
 *
 */
<<<<<<< HEAD
bool MACbSafeSoftwareReset(void __iomem *dwIoBase)
=======
bool MACbSafeSoftwareReset(struct vnt_private *priv)
>>>>>>> v4.9.227
{
	unsigned char abyTmpRegData[MAC_MAX_CONTEXT_SIZE_PAGE0+MAC_MAX_CONTEXT_SIZE_PAGE1];
	bool bRetVal;

<<<<<<< HEAD
	// PATCH....
	// save some important register's value, then do
	// reset, then restore register's value

	// save MAC context
	MACvSaveContext(dwIoBase, abyTmpRegData);
	// do reset
	bRetVal = MACbSoftwareReset(dwIoBase);
	// restore MAC context, except CR0
	MACvRestoreContext(dwIoBase, abyTmpRegData);
=======
	/* PATCH....
	 * save some important register's value, then do
	 * reset, then restore register's value
	 */
	/* save MAC context */
	MACvSaveContext(priv, abyTmpRegData);
	/* do reset */
	bRetVal = MACbSoftwareReset(priv);
	/* restore MAC context, except CR0 */
	MACvRestoreContext(priv, abyTmpRegData);
>>>>>>> v4.9.227

	return bRetVal;
}

/*
 * Description:
<<<<<<< HEAD
 *      Trun Off MAC Rx
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
=======
 *      Turn Off MAC Rx
 *
 * Parameters:
 *  In:
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: true if success; otherwise false
 *
 */
<<<<<<< HEAD
bool MACbSafeRxOff(void __iomem *dwIoBase)
{
	unsigned short ww;
	unsigned long dwData;
	unsigned char byData;

	// turn off wow temp for turn off Rx safely

	// Clear RX DMA0,1
	VNSvOutPortD(dwIoBase + MAC_REG_RXDMACTL0, DMACTL_CLRRUN);
	VNSvOutPortD(dwIoBase + MAC_REG_RXDMACTL1, DMACTL_CLRRUN);
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortD(dwIoBase + MAC_REG_RXDMACTL0, &dwData);
		if (!(dwData & DMACTL_RUN))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
		DBG_PORT80(0x10);
=======
bool MACbSafeRxOff(struct vnt_private *priv)
{
	void __iomem *io_base = priv->PortOffset;
	unsigned short ww;

	/* turn off wow temp for turn off Rx safely */

	/* Clear RX DMA0,1 */
	iowrite32(DMACTL_CLRRUN, io_base + MAC_REG_RXDMACTL0);
	iowrite32(DMACTL_CLRRUN, io_base + MAC_REG_RXDMACTL1);
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		if (!(ioread32(io_base + MAC_REG_RXDMACTL0) & DMACTL_RUN))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
>>>>>>> v4.9.227
		pr_debug(" DBG_PORT80(0x10)\n");
		return false;
	}
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
<<<<<<< HEAD
		VNSvInPortD(dwIoBase + MAC_REG_RXDMACTL1, &dwData);
		if (!(dwData & DMACTL_RUN))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
		DBG_PORT80(0x11);
=======
		if (!(ioread32(io_base + MAC_REG_RXDMACTL1) & DMACTL_RUN))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
>>>>>>> v4.9.227
		pr_debug(" DBG_PORT80(0x11)\n");
		return false;
	}

<<<<<<< HEAD
	// try to safe shutdown RX
	MACvRegBitsOff(dwIoBase, MAC_REG_HOSTCR, HOSTCR_RXON);
	// W_MAX_TIMEOUT is the timeout period
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortB(dwIoBase + MAC_REG_HOSTCR, &byData);
		if (!(byData & HOSTCR_RXONST))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
		DBG_PORT80(0x12);
=======
	/* try to safe shutdown RX */
	MACvRegBitsOff(io_base, MAC_REG_HOSTCR, HOSTCR_RXON);
	/* W_MAX_TIMEOUT is the timeout period */
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		if (!(ioread8(io_base + MAC_REG_HOSTCR) & HOSTCR_RXONST))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
>>>>>>> v4.9.227
		pr_debug(" DBG_PORT80(0x12)\n");
		return false;
	}
	return true;
}

/*
 * Description:
<<<<<<< HEAD
 *      Trun Off MAC Tx
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
=======
 *      Turn Off MAC Tx
 *
 * Parameters:
 *  In:
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: true if success; otherwise false
 *
 */
<<<<<<< HEAD
bool MACbSafeTxOff(void __iomem *dwIoBase)
{
	unsigned short ww;
	unsigned long dwData;
	unsigned char byData;

	// Clear TX DMA
	//Tx0
	VNSvOutPortD(dwIoBase + MAC_REG_TXDMACTL0, DMACTL_CLRRUN);
	//AC0
	VNSvOutPortD(dwIoBase + MAC_REG_AC0DMACTL, DMACTL_CLRRUN);

	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortD(dwIoBase + MAC_REG_TXDMACTL0, &dwData);
		if (!(dwData & DMACTL_RUN))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
		DBG_PORT80(0x20);
=======
bool MACbSafeTxOff(struct vnt_private *priv)
{
	void __iomem *io_base = priv->PortOffset;
	unsigned short ww;

	/* Clear TX DMA */
	/* Tx0 */
	iowrite32(DMACTL_CLRRUN, io_base + MAC_REG_TXDMACTL0);
	/* AC0 */
	iowrite32(DMACTL_CLRRUN, io_base + MAC_REG_AC0DMACTL);

	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		if (!(ioread32(io_base + MAC_REG_TXDMACTL0) & DMACTL_RUN))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
>>>>>>> v4.9.227
		pr_debug(" DBG_PORT80(0x20)\n");
		return false;
	}
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
<<<<<<< HEAD
		VNSvInPortD(dwIoBase + MAC_REG_AC0DMACTL, &dwData);
		if (!(dwData & DMACTL_RUN))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
		DBG_PORT80(0x21);
=======
		if (!(ioread32(io_base + MAC_REG_AC0DMACTL) & DMACTL_RUN))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
>>>>>>> v4.9.227
		pr_debug(" DBG_PORT80(0x21)\n");
		return false;
	}

<<<<<<< HEAD
	// try to safe shutdown TX
	MACvRegBitsOff(dwIoBase, MAC_REG_HOSTCR, HOSTCR_TXON);

	// W_MAX_TIMEOUT is the timeout period
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortB(dwIoBase + MAC_REG_HOSTCR, &byData);
		if (!(byData & HOSTCR_TXONST))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
		DBG_PORT80(0x24);
=======
	/* try to safe shutdown TX */
	MACvRegBitsOff(io_base, MAC_REG_HOSTCR, HOSTCR_TXON);

	/* W_MAX_TIMEOUT is the timeout period */
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		if (!(ioread8(io_base + MAC_REG_HOSTCR) & HOSTCR_TXONST))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
>>>>>>> v4.9.227
		pr_debug(" DBG_PORT80(0x24)\n");
		return false;
	}
	return true;
}

/*
 * Description:
 *      Stop MAC function
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
=======
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: true if success; otherwise false
 *
 */
<<<<<<< HEAD
bool MACbSafeStop(void __iomem *dwIoBase)
{
	MACvRegBitsOff(dwIoBase, MAC_REG_TCR, TCR_AUTOBCNTX);

	if (!MACbSafeRxOff(dwIoBase)) {
		DBG_PORT80(0xA1);
		pr_debug(" MACbSafeRxOff == false)\n");
		MACbSafeSoftwareReset(dwIoBase);
		return false;
	}
	if (!MACbSafeTxOff(dwIoBase)) {
		DBG_PORT80(0xA2);
		pr_debug(" MACbSafeTxOff == false)\n");
		MACbSafeSoftwareReset(dwIoBase);
		return false;
	}

	MACvRegBitsOff(dwIoBase, MAC_REG_HOSTCR, HOSTCR_MACEN);
=======
bool MACbSafeStop(struct vnt_private *priv)
{
	void __iomem *io_base = priv->PortOffset;

	MACvRegBitsOff(io_base, MAC_REG_TCR, TCR_AUTOBCNTX);

	if (!MACbSafeRxOff(priv)) {
		pr_debug(" MACbSafeRxOff == false)\n");
		MACbSafeSoftwareReset(priv);
		return false;
	}
	if (!MACbSafeTxOff(priv)) {
		pr_debug(" MACbSafeTxOff == false)\n");
		MACbSafeSoftwareReset(priv);
		return false;
	}

	MACvRegBitsOff(io_base, MAC_REG_HOSTCR, HOSTCR_MACEN);
>>>>>>> v4.9.227

	return true;
}

/*
 * Description:
 *      Shut Down MAC
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
=======
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: true if success; otherwise false
 *
 */
<<<<<<< HEAD
bool MACbShutdown(void __iomem *dwIoBase)
{
	// disable MAC IMR
	MACvIntDisable(dwIoBase);
	MACvSetLoopbackMode(dwIoBase, MAC_LB_INTERNAL);
	// stop the adapter
	if (!MACbSafeStop(dwIoBase)) {
		MACvSetLoopbackMode(dwIoBase, MAC_LB_NONE);
		return false;
	}
	MACvSetLoopbackMode(dwIoBase, MAC_LB_NONE);
=======
bool MACbShutdown(struct vnt_private *priv)
{
	void __iomem *io_base = priv->PortOffset;
	/* disable MAC IMR */
	MACvIntDisable(io_base);
	MACvSetLoopbackMode(priv, MAC_LB_INTERNAL);
	/* stop the adapter */
	if (!MACbSafeStop(priv)) {
		MACvSetLoopbackMode(priv, MAC_LB_NONE);
		return false;
	}
	MACvSetLoopbackMode(priv, MAC_LB_NONE);
>>>>>>> v4.9.227
	return true;
}

/*
 * Description:
 *      Initialize MAC
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
=======
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
void MACvInitialize(void __iomem *dwIoBase)
{
	// clear sticky bits
	MACvClearStckDS(dwIoBase);
	// disable force PME-enable
	VNSvOutPortB(dwIoBase + MAC_REG_PMC1, PME_OVR);
	// only 3253 A

	// do reset
	MACbSoftwareReset(dwIoBase);

	// reset TSF counter
	VNSvOutPortB(dwIoBase + MAC_REG_TFTCTL, TFTCTL_TSFCNTRST);
	// enable TSF counter
	VNSvOutPortB(dwIoBase + MAC_REG_TFTCTL, TFTCTL_TSFCNTREN);

	// set packet filter
	// receive directed and broadcast address

	MACvSetPacketFilter(dwIoBase, PKT_TYPE_DIRECTED | PKT_TYPE_BROADCAST);
=======
void MACvInitialize(struct vnt_private *priv)
{
	void __iomem *io_base = priv->PortOffset;
	/* clear sticky bits */
	MACvClearStckDS(io_base);
	/* disable force PME-enable */
	iowrite8(PME_OVR, io_base + MAC_REG_PMC1);
	/* only 3253 A */

	/* do reset */
	MACbSoftwareReset(priv);

	/* reset TSF counter */
	iowrite8(TFTCTL_TSFCNTRST, io_base + MAC_REG_TFTCTL);
	/* enable TSF counter */
	iowrite8(TFTCTL_TSFCNTREN, io_base + MAC_REG_TFTCTL);
>>>>>>> v4.9.227
}

/*
 * Description:
 *      Set the chip with current rx descriptor address
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase        - Base Address for MAC
 *      dwCurrDescAddr  - Descriptor Address
=======
 *      io_base        - Base Address for MAC
 *      curr_desc_addr  - Descriptor Address
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
void MACvSetCurrRx0DescAddr(void __iomem *dwIoBase, unsigned long dwCurrDescAddr)
{
	unsigned short ww;
	unsigned char byData;
	unsigned char byOrgDMACtl;

	VNSvInPortB(dwIoBase + MAC_REG_RXDMACTL0, &byOrgDMACtl);
	if (byOrgDMACtl & DMACTL_RUN)
		VNSvOutPortB(dwIoBase + MAC_REG_RXDMACTL0+2, DMACTL_RUN);

	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortB(dwIoBase + MAC_REG_RXDMACTL0, &byData);
		if (!(byData & DMACTL_RUN))
			break;
	}

	if (ww == W_MAX_TIMEOUT)
		DBG_PORT80(0x13);

	VNSvOutPortD(dwIoBase + MAC_REG_RXDMAPTR0, dwCurrDescAddr);
	if (byOrgDMACtl & DMACTL_RUN)
		VNSvOutPortB(dwIoBase + MAC_REG_RXDMACTL0, DMACTL_RUN);
=======
void MACvSetCurrRx0DescAddr(struct vnt_private *priv, u32 curr_desc_addr)
{
	void __iomem *io_base = priv->PortOffset;
	unsigned short ww;
	unsigned char org_dma_ctl;

	org_dma_ctl = ioread8(io_base + MAC_REG_RXDMACTL0);
	if (org_dma_ctl & DMACTL_RUN)
		iowrite8(DMACTL_RUN, io_base + MAC_REG_RXDMACTL0 + 2);

	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		if (!(ioread8(io_base + MAC_REG_RXDMACTL0) & DMACTL_RUN))
			break;
	}

	iowrite32(curr_desc_addr, io_base + MAC_REG_RXDMAPTR0);
	if (org_dma_ctl & DMACTL_RUN)
		iowrite8(DMACTL_RUN, io_base + MAC_REG_RXDMACTL0);
>>>>>>> v4.9.227
}

/*
 * Description:
 *      Set the chip with current rx descriptor address
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase        - Base Address for MAC
 *      dwCurrDescAddr  - Descriptor Address
=======
 *      io_base        - Base Address for MAC
 *      curr_desc_addr  - Descriptor Address
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
void MACvSetCurrRx1DescAddr(void __iomem *dwIoBase, unsigned long dwCurrDescAddr)
{
	unsigned short ww;
	unsigned char byData;
	unsigned char byOrgDMACtl;

	VNSvInPortB(dwIoBase + MAC_REG_RXDMACTL1, &byOrgDMACtl);
	if (byOrgDMACtl & DMACTL_RUN)
		VNSvOutPortB(dwIoBase + MAC_REG_RXDMACTL1+2, DMACTL_RUN);

	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortB(dwIoBase + MAC_REG_RXDMACTL1, &byData);
		if (!(byData & DMACTL_RUN))
			break;
	}
	if (ww == W_MAX_TIMEOUT)
		DBG_PORT80(0x14);

	VNSvOutPortD(dwIoBase + MAC_REG_RXDMAPTR1, dwCurrDescAddr);
	if (byOrgDMACtl & DMACTL_RUN)
		VNSvOutPortB(dwIoBase + MAC_REG_RXDMACTL1, DMACTL_RUN);
=======
void MACvSetCurrRx1DescAddr(struct vnt_private *priv, u32 curr_desc_addr)
{
	void __iomem *io_base = priv->PortOffset;
	unsigned short ww;
	unsigned char org_dma_ctl;

	org_dma_ctl = ioread8(io_base + MAC_REG_RXDMACTL1);
	if (org_dma_ctl & DMACTL_RUN)
		iowrite8(DMACTL_RUN, io_base + MAC_REG_RXDMACTL1 + 2);

	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		if (!(ioread8(io_base + MAC_REG_RXDMACTL1) & DMACTL_RUN))
			break;
	}

	iowrite32(curr_desc_addr, io_base + MAC_REG_RXDMAPTR1);
	if (org_dma_ctl & DMACTL_RUN)
		iowrite8(DMACTL_RUN, io_base + MAC_REG_RXDMACTL1);
>>>>>>> v4.9.227

}

/*
 * Description:
 *      Set the chip with current tx0 descriptor address
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase        - Base Address for MAC
 *      dwCurrDescAddr  - Descriptor Address
=======
 *      io_base        - Base Address for MAC
 *      curr_desc_addr  - Descriptor Address
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
void MACvSetCurrTx0DescAddrEx(void __iomem *dwIoBase, unsigned long dwCurrDescAddr)
{
	unsigned short ww;
	unsigned char byData;
	unsigned char byOrgDMACtl;

	VNSvInPortB(dwIoBase + MAC_REG_TXDMACTL0, &byOrgDMACtl);
	if (byOrgDMACtl & DMACTL_RUN)
		VNSvOutPortB(dwIoBase + MAC_REG_TXDMACTL0+2, DMACTL_RUN);

	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortB(dwIoBase + MAC_REG_TXDMACTL0, &byData);
		if (!(byData & DMACTL_RUN))
			break;
	}
	if (ww == W_MAX_TIMEOUT)
		DBG_PORT80(0x25);

	VNSvOutPortD(dwIoBase + MAC_REG_TXDMAPTR0, dwCurrDescAddr);
	if (byOrgDMACtl & DMACTL_RUN)
		VNSvOutPortB(dwIoBase + MAC_REG_TXDMACTL0, DMACTL_RUN);
=======
void MACvSetCurrTx0DescAddrEx(struct vnt_private *priv,
			      u32 curr_desc_addr)
{
	void __iomem *io_base = priv->PortOffset;
	unsigned short ww;
	unsigned char org_dma_ctl;

	org_dma_ctl = ioread8(io_base + MAC_REG_TXDMACTL0);
	if (org_dma_ctl & DMACTL_RUN)
		iowrite8(DMACTL_RUN, io_base + MAC_REG_TXDMACTL0 + 2);

	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		if (!(ioread8(io_base + MAC_REG_TXDMACTL0) & DMACTL_RUN))
			break;
	}

	iowrite32(curr_desc_addr, io_base + MAC_REG_TXDMAPTR0);
	if (org_dma_ctl & DMACTL_RUN)
		iowrite8(DMACTL_RUN, io_base + MAC_REG_TXDMACTL0);
>>>>>>> v4.9.227
}

/*
 * Description:
 *      Set the chip with current AC0 descriptor address
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase        - Base Address for MAC
 *      dwCurrDescAddr  - Descriptor Address
=======
 *      io_base        - Base Address for MAC
 *      curr_desc_addr  - Descriptor Address
>>>>>>> v4.9.227
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
//TxDMA1 = AC0DMA
void MACvSetCurrAC0DescAddrEx(void __iomem *dwIoBase, unsigned long dwCurrDescAddr)
{
	unsigned short ww;
	unsigned char byData;
	unsigned char byOrgDMACtl;

	VNSvInPortB(dwIoBase + MAC_REG_AC0DMACTL, &byOrgDMACtl);
	if (byOrgDMACtl & DMACTL_RUN)
		VNSvOutPortB(dwIoBase + MAC_REG_AC0DMACTL+2, DMACTL_RUN);

	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortB(dwIoBase + MAC_REG_AC0DMACTL, &byData);
		if (!(byData & DMACTL_RUN))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
		DBG_PORT80(0x26);
		pr_debug(" DBG_PORT80(0x26)\n");
	}
	VNSvOutPortD(dwIoBase + MAC_REG_AC0DMAPTR, dwCurrDescAddr);
	if (byOrgDMACtl & DMACTL_RUN)
		VNSvOutPortB(dwIoBase + MAC_REG_AC0DMACTL, DMACTL_RUN);
}

void MACvSetCurrTXDescAddr(int iTxType, void __iomem *dwIoBase, unsigned long dwCurrDescAddr)
{
	if (iTxType == TYPE_AC0DMA)
		MACvSetCurrAC0DescAddrEx(dwIoBase, dwCurrDescAddr);
	else if (iTxType == TYPE_TXDMA0)
		MACvSetCurrTx0DescAddrEx(dwIoBase, dwCurrDescAddr);
=======
/* TxDMA1 = AC0DMA */
void MACvSetCurrAC0DescAddrEx(struct vnt_private *priv,
			      u32 curr_desc_addr)
{
	void __iomem *io_base = priv->PortOffset;
	unsigned short ww;
	unsigned char org_dma_ctl;

	org_dma_ctl = ioread8(io_base + MAC_REG_AC0DMACTL);
	if (org_dma_ctl & DMACTL_RUN)
		iowrite8(DMACTL_RUN, io_base + MAC_REG_AC0DMACTL + 2);

	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		if (!(ioread8(io_base + MAC_REG_AC0DMACTL) & DMACTL_RUN))
			break;
	}
	if (ww == W_MAX_TIMEOUT)
		pr_debug(" DBG_PORT80(0x26)\n");
	iowrite32(curr_desc_addr, io_base + MAC_REG_AC0DMAPTR);
	if (org_dma_ctl & DMACTL_RUN)
		iowrite8(DMACTL_RUN, io_base + MAC_REG_AC0DMACTL);
}

void MACvSetCurrTXDescAddr(int iTxType, struct vnt_private *priv,
			   u32 curr_desc_addr)
{
	if (iTxType == TYPE_AC0DMA)
		MACvSetCurrAC0DescAddrEx(priv, curr_desc_addr);
	else if (iTxType == TYPE_TXDMA0)
		MACvSetCurrTx0DescAddrEx(priv, curr_desc_addr);
>>>>>>> v4.9.227
}

/*
 * Description:
 *      Micro Second Delay via MAC
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
=======
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *      uDelay      - Delay time (timer resolution is 4 us)
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
void MACvTimer0MicroSDelay(void __iomem *dwIoBase, unsigned int uDelay)
{
	unsigned char byValue;
	unsigned int uu, ii;

	VNSvOutPortB(dwIoBase + MAC_REG_TMCTL0, 0);
	VNSvOutPortD(dwIoBase + MAC_REG_TMDATA0, uDelay);
	VNSvOutPortB(dwIoBase + MAC_REG_TMCTL0, (TMCTL_TMD | TMCTL_TE));
	for (ii = 0; ii < 66; ii++) {  // assume max PCI clock is 66Mhz
		for (uu = 0; uu < uDelay; uu++) {
			VNSvInPortB(dwIoBase + MAC_REG_TMCTL0, &byValue);
			if ((byValue == 0) ||
			    (byValue & TMCTL_TSUSP)) {
				VNSvOutPortB(dwIoBase + MAC_REG_TMCTL0, 0);
=======
void MACvTimer0MicroSDelay(struct vnt_private *priv, unsigned int uDelay)
{
	void __iomem *io_base = priv->PortOffset;
	unsigned char byValue;
	unsigned int uu, ii;

	iowrite8(0, io_base + MAC_REG_TMCTL0);
	iowrite32(uDelay, io_base + MAC_REG_TMDATA0);
	iowrite8((TMCTL_TMD | TMCTL_TE), io_base + MAC_REG_TMCTL0);
	for (ii = 0; ii < 66; ii++) {  /* assume max PCI clock is 66Mhz */
		for (uu = 0; uu < uDelay; uu++) {
			byValue = ioread8(io_base + MAC_REG_TMCTL0);
			if ((byValue == 0) ||
			    (byValue & TMCTL_TSUSP)) {
				iowrite8(0, io_base + MAC_REG_TMCTL0);
>>>>>>> v4.9.227
				return;
			}
		}
	}
<<<<<<< HEAD
	VNSvOutPortB(dwIoBase + MAC_REG_TMCTL0, 0);
=======
	iowrite8(0, io_base + MAC_REG_TMCTL0);
>>>>>>> v4.9.227
}

/*
 * Description:
 *      Micro Second One shot timer via MAC
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase    - Base Address for MAC
=======
 *      io_base    - Base Address for MAC
>>>>>>> v4.9.227
 *      uDelay      - Delay time
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
void MACvOneShotTimer0MicroSec(void __iomem *dwIoBase, unsigned int uDelayTime)
{
	VNSvOutPortB(dwIoBase + MAC_REG_TMCTL0, 0);
	VNSvOutPortD(dwIoBase + MAC_REG_TMDATA0, uDelayTime);
	VNSvOutPortB(dwIoBase + MAC_REG_TMCTL0, (TMCTL_TMD | TMCTL_TE));
}

/*
 * Description:
 *      Micro Second One shot timer via MAC
 *
 * Parameters:
 *  In:
 *      dwIoBase    - Base Address for MAC
 *      uDelay      - Delay time
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void MACvOneShotTimer1MicroSec(void __iomem *dwIoBase, unsigned int uDelayTime)
{
	VNSvOutPortB(dwIoBase + MAC_REG_TMCTL1, 0);
	VNSvOutPortD(dwIoBase + MAC_REG_TMDATA1, uDelayTime);
	VNSvOutPortB(dwIoBase + MAC_REG_TMCTL1, (TMCTL_TMD | TMCTL_TE));
}

void MACvSetMISCFifo(void __iomem *dwIoBase, unsigned short wOffset, unsigned long dwData)
{
	if (wOffset > 273)
		return;
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset);
	VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, dwData);
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
}

bool MACbTxDMAOff(void __iomem *dwIoBase, unsigned int idx)
{
	unsigned char byData;
	unsigned int ww = 0;

	if (idx == TYPE_TXDMA0) {
		VNSvOutPortB(dwIoBase + MAC_REG_TXDMACTL0+2, DMACTL_RUN);
		for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
			VNSvInPortB(dwIoBase + MAC_REG_TXDMACTL0, &byData);
			if (!(byData & DMACTL_RUN))
				break;
		}
	} else if (idx == TYPE_AC0DMA) {
		VNSvOutPortB(dwIoBase + MAC_REG_AC0DMACTL+2, DMACTL_RUN);
		for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
			VNSvInPortB(dwIoBase + MAC_REG_AC0DMACTL, &byData);
			if (!(byData & DMACTL_RUN))
				break;
		}
	}
	if (ww == W_MAX_TIMEOUT) {
		DBG_PORT80(0x29);
		pr_debug(" DBG_PORT80(0x29)\n");
		return false;
	}
	return true;
}

void MACvClearBusSusInd(void __iomem *dwIoBase)
{
	unsigned long dwOrgValue;
	unsigned int ww;
	// check if BcnSusInd enabled
	VNSvInPortD(dwIoBase + MAC_REG_ENCFG , &dwOrgValue);
	if (!(dwOrgValue & EnCFG_BcnSusInd))
		return;
	//Set BcnSusClr
	dwOrgValue = dwOrgValue | EnCFG_BcnSusClr;
	VNSvOutPortD(dwIoBase + MAC_REG_ENCFG, dwOrgValue);
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortD(dwIoBase + MAC_REG_ENCFG , &dwOrgValue);
		if (!(dwOrgValue & EnCFG_BcnSusInd))
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
		DBG_PORT80(0x33);
		pr_debug(" DBG_PORT80(0x33)\n");
	}
}

void MACvEnableBusSusEn(void __iomem *dwIoBase)
{
	unsigned char byOrgValue;
	unsigned long dwOrgValue;
	unsigned int ww;
	// check if BcnSusInd enabled
	VNSvInPortB(dwIoBase + MAC_REG_CFG , &byOrgValue);

	//Set BcnSusEn
	byOrgValue = byOrgValue | CFG_BCNSUSEN;
	VNSvOutPortB(dwIoBase + MAC_REG_ENCFG, byOrgValue);
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortD(dwIoBase + MAC_REG_ENCFG , &dwOrgValue);
		if (dwOrgValue & EnCFG_BcnSusInd)
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
		DBG_PORT80(0x34);
		pr_debug(" DBG_PORT80(0x34)\n");
	}
}

bool MACbFlushSYNCFifo(void __iomem *dwIoBase)
{
	unsigned char byOrgValue;
	unsigned int ww;
	// Read MACCR
	VNSvInPortB(dwIoBase + MAC_REG_MACCR , &byOrgValue);

	// Set SYNCFLUSH
	byOrgValue = byOrgValue | MACCR_SYNCFLUSH;
	VNSvOutPortB(dwIoBase + MAC_REG_MACCR, byOrgValue);

	// Check if SyncFlushOK
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortB(dwIoBase + MAC_REG_MACCR , &byOrgValue);
		if (byOrgValue & MACCR_SYNCFLUSHOK)
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
		DBG_PORT80(0x35);
		pr_debug(" DBG_PORT80(0x33)\n");
	}
	return true;
}

bool MACbPSWakeup(void __iomem *dwIoBase)
{
	unsigned char byOrgValue;
	unsigned int ww;
	// Read PSCTL
	if (MACbIsRegBitsOff(dwIoBase, MAC_REG_PSCTL, PSCTL_PS))
		return true;

	// Disable PS
	MACvRegBitsOff(dwIoBase, MAC_REG_PSCTL, PSCTL_PSEN);

	// Check if SyncFlushOK
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		VNSvInPortB(dwIoBase + MAC_REG_PSCTL , &byOrgValue);
		if (byOrgValue & PSCTL_WAKEDONE)
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
		DBG_PORT80(0x36);
=======
void MACvOneShotTimer1MicroSec(struct vnt_private *priv,
			       unsigned int uDelayTime)
{
	void __iomem *io_base = priv->PortOffset;

	iowrite8(0, io_base + MAC_REG_TMCTL1);
	iowrite32(uDelayTime, io_base + MAC_REG_TMDATA1);
	iowrite8((TMCTL_TMD | TMCTL_TE), io_base + MAC_REG_TMCTL1);
}

void MACvSetMISCFifo(struct vnt_private *priv, unsigned short offset,
		     u32 data)
{
	void __iomem *io_base = priv->PortOffset;

	if (offset > 273)
		return;
	iowrite16(offset, io_base + MAC_REG_MISCFFNDEX);
	iowrite32(data, io_base + MAC_REG_MISCFFDATA);
	iowrite16(MISCFFCTL_WRITE, io_base + MAC_REG_MISCFFCTL);
}

bool MACbPSWakeup(struct vnt_private *priv)
{
	void __iomem *io_base = priv->PortOffset;
	unsigned int ww;
	/* Read PSCTL */
	if (MACbIsRegBitsOff(priv, MAC_REG_PSCTL, PSCTL_PS))
		return true;

	/* Disable PS */
	MACvRegBitsOff(io_base, MAC_REG_PSCTL, PSCTL_PSEN);

	/* Check if SyncFlushOK */
	for (ww = 0; ww < W_MAX_TIMEOUT; ww++) {
		if (ioread8(io_base + MAC_REG_PSCTL) & PSCTL_WAKEDONE)
			break;
	}
	if (ww == W_MAX_TIMEOUT) {
>>>>>>> v4.9.227
		pr_debug(" DBG_PORT80(0x33)\n");
		return false;
	}
	return true;
}

/*
 * Description:
 *      Set the Key by MISCFIFO
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase        - Base Address for MAC
=======
 *      io_base        - Base Address for MAC
>>>>>>> v4.9.227
 *
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */

<<<<<<< HEAD
void MACvSetKeyEntry(void __iomem *dwIoBase, unsigned short wKeyCtl, unsigned int uEntryIdx,
		     unsigned int uKeyIdx, unsigned char *pbyAddr, u32 *pdwKey, unsigned char byLocalID)
{
	unsigned short wOffset;
	u32 dwData;
=======
void MACvSetKeyEntry(struct vnt_private *priv, unsigned short wKeyCtl,
		     unsigned int uEntryIdx, unsigned int uKeyIdx,
		     unsigned char *pbyAddr, u32 *pdwKey,
		     unsigned char byLocalID)
{
	void __iomem *io_base = priv->PortOffset;
	unsigned short offset;
	u32 data;
>>>>>>> v4.9.227
	int     ii;

	if (byLocalID <= 1)
		return;

	pr_debug("MACvSetKeyEntry\n");
<<<<<<< HEAD
	wOffset = MISCFIFO_KEYETRY0;
	wOffset += (uEntryIdx * MISCFIFO_KEYENTRYSIZE);

	dwData = 0;
	dwData |= wKeyCtl;
	dwData <<= 16;
	dwData |= MAKEWORD(*(pbyAddr+4), *(pbyAddr+5));
	pr_debug("1. wOffset: %d, Data: %X, KeyCtl:%X\n",
		 wOffset, dwData, wKeyCtl);

	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset);
	VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, dwData);
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
	wOffset++;

	dwData = 0;
	dwData |= *(pbyAddr+3);
	dwData <<= 8;
	dwData |= *(pbyAddr+2);
	dwData <<= 8;
	dwData |= *(pbyAddr+1);
	dwData <<= 8;
	dwData |= *(pbyAddr+0);
	pr_debug("2. wOffset: %d, Data: %X\n", wOffset, dwData);

	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset);
	VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, dwData);
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
	wOffset++;

	wOffset += (uKeyIdx * 4);
	for (ii = 0; ii < 4; ii++) {
		// always push 128 bits
		pr_debug("3.(%d) wOffset: %d, Data: %X\n",
			 ii, wOffset+ii, *pdwKey);
		VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset+ii);
		VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, *pdwKey++);
		VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
=======
	offset = MISCFIFO_KEYETRY0;
	offset += (uEntryIdx * MISCFIFO_KEYENTRYSIZE);

	data = 0;
	data |= wKeyCtl;
	data <<= 16;
	data |= MAKEWORD(*(pbyAddr + 4), *(pbyAddr + 5));
	pr_debug("1. offset: %d, Data: %X, KeyCtl:%X\n",
		 offset, data, wKeyCtl);

	iowrite16(offset, io_base + MAC_REG_MISCFFNDEX);
	iowrite32(data, io_base + MAC_REG_MISCFFDATA);
	iowrite16(MISCFFCTL_WRITE, io_base + MAC_REG_MISCFFCTL);
	offset++;

	data = 0;
	data |= *(pbyAddr + 3);
	data <<= 8;
	data |= *(pbyAddr + 2);
	data <<= 8;
	data |= *(pbyAddr + 1);
	data <<= 8;
	data |= *pbyAddr;
	pr_debug("2. offset: %d, Data: %X\n", offset, data);

	iowrite16(offset, io_base + MAC_REG_MISCFFNDEX);
	iowrite32(data, io_base + MAC_REG_MISCFFDATA);
	iowrite16(MISCFFCTL_WRITE, io_base + MAC_REG_MISCFFCTL);
	offset++;

	offset += (uKeyIdx * 4);
	for (ii = 0; ii < 4; ii++) {
		/* always push 128 bits */
		pr_debug("3.(%d) offset: %d, Data: %X\n",
			 ii, offset + ii, *pdwKey);
		iowrite16(offset + ii, io_base + MAC_REG_MISCFFNDEX);
		iowrite32(*pdwKey++, io_base + MAC_REG_MISCFFDATA);
		iowrite16(MISCFFCTL_WRITE, io_base + MAC_REG_MISCFFCTL);
>>>>>>> v4.9.227
	}
}

/*
 * Description:
 *      Disable the Key Entry by MISCFIFO
 *
 * Parameters:
 *  In:
<<<<<<< HEAD
 *      dwIoBase        - Base Address for MAC
=======
 *      io_base        - Base Address for MAC
>>>>>>> v4.9.227
 *
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
<<<<<<< HEAD
void MACvDisableKeyEntry(void __iomem *dwIoBase, unsigned int uEntryIdx)
{
	unsigned short wOffset;

	wOffset = MISCFIFO_KEYETRY0;
	wOffset += (uEntryIdx * MISCFIFO_KEYENTRYSIZE);

	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset);
	VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, 0);
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
}

/*
 * Description:
 *      Set the default Key (KeyEntry[10]) by MISCFIFO
 *
 * Parameters:
 *  In:
 *      dwIoBase        - Base Address for MAC
 *
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */

void MACvSetDefaultKeyEntry(void __iomem *dwIoBase, unsigned int uKeyLen,
			    unsigned int uKeyIdx, unsigned long *pdwKey, unsigned char byLocalID)
{
	unsigned short wOffset;
	unsigned long dwData;
	int     ii;

	if (byLocalID <= 1)
		return;

	pr_debug("MACvSetDefaultKeyEntry\n");
	wOffset = MISCFIFO_KEYETRY0;
	wOffset += (10 * MISCFIFO_KEYENTRYSIZE);

	wOffset++;
	wOffset++;
	wOffset += (uKeyIdx * 4);
	// always push 128 bits
	for (ii = 0; ii < 3; ii++) {
		pr_debug("(%d) wOffset: %d, Data: %lX\n",
			 ii, wOffset+ii, *pdwKey);
		VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset+ii);
		VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, *pdwKey++);
		VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
	}
	dwData = *pdwKey;
	if (uKeyLen == WLAN_WEP104_KEYLEN)
		dwData |= 0x80000000;

	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset+3);
	VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, dwData);
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
	pr_debug("End. wOffset: %d, Data: %lX\n", wOffset+3, dwData);
}

/*
 * Description:
 *      Enable default Key (KeyEntry[10]) by MISCFIFO
 *
 * Parameters:
 *  In:
 *      dwIoBase        - Base Address for MAC
 *
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
/*
  void MACvEnableDefaultKey(void __iomem *dwIoBase, unsigned char byLocalID)
  {
  unsigned short wOffset;
  unsigned long dwData;

  if (byLocalID <= 1)
  return;

  wOffset = MISCFIFO_KEYETRY0;
  wOffset += (10 * MISCFIFO_KEYENTRYSIZE);

  dwData = 0xC0440000;
  VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset);
  VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, dwData);
  VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
  pr_debug("MACvEnableDefaultKey: wOffset: %d, Data: %lX\n", wOffset, dwData);

  }
*/

/*
 * Description:
 *      Disable default Key (KeyEntry[10]) by MISCFIFO
 *
 * Parameters:
 *  In:
 *      dwIoBase        - Base Address for MAC
 *
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void MACvDisableDefaultKey(void __iomem *dwIoBase)
{
	unsigned short wOffset;
	unsigned long dwData;

	wOffset = MISCFIFO_KEYETRY0;
	wOffset += (10 * MISCFIFO_KEYENTRYSIZE);

	dwData = 0x0;
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset);
	VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, dwData);
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
	pr_debug("MACvDisableDefaultKey: wOffset: %d, Data: %lX\n",
		 wOffset, dwData);
}

/*
 * Description:
 *      Set the default TKIP Group Key (KeyEntry[10]) by MISCFIFO
 *
 * Parameters:
 *  In:
 *      dwIoBase        - Base Address for MAC
 *
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void MACvSetDefaultTKIPKeyEntry(void __iomem *dwIoBase, unsigned int uKeyLen,
				unsigned int uKeyIdx, unsigned long *pdwKey, unsigned char byLocalID)
{
	unsigned short wOffset;
	unsigned long dwData;
	int     ii;

	if (byLocalID <= 1)
		return;

	pr_debug("MACvSetDefaultTKIPKeyEntry\n");
	wOffset = MISCFIFO_KEYETRY0;
	// Kyle test : change offset from 10 -> 0
	wOffset += (10 * MISCFIFO_KEYENTRYSIZE);

	dwData = 0xC0660000;
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset);
	VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, dwData);
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
	wOffset++;

	dwData = 0;
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset);
	VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, dwData);
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
	wOffset++;

	wOffset += (uKeyIdx * 4);
	pr_debug("1. wOffset: %d, Data: %lX, idx:%d\n",
		 wOffset, *pdwKey, uKeyIdx);
	// always push 128 bits
	for (ii = 0; ii < 4; ii++) {
		pr_debug("2.(%d) wOffset: %d, Data: %lX\n",
			 ii, wOffset+ii, *pdwKey);
		VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset+ii);
		VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, *pdwKey++);
		VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
	}
}

/*
 * Description:
 *      Set the Key Control by MISCFIFO
 *
 * Parameters:
 *  In:
 *      dwIoBase        - Base Address for MAC
 *
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */

void MACvSetDefaultKeyCtl(void __iomem *dwIoBase, unsigned short wKeyCtl, unsigned int uEntryIdx, unsigned char byLocalID)
{
	unsigned short wOffset;
	unsigned long dwData;

	if (byLocalID <= 1)
		return;

	pr_debug("MACvSetKeyEntry\n");
	wOffset = MISCFIFO_KEYETRY0;
	wOffset += (uEntryIdx * MISCFIFO_KEYENTRYSIZE);

	dwData = 0;
	dwData |= wKeyCtl;
	dwData <<= 16;
	dwData |= 0xffff;
	pr_debug("1. wOffset: %d, Data: %lX, KeyCtl:%X\n",
		 wOffset, dwData, wKeyCtl);

	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset);
	VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, dwData);
	VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
=======
void MACvDisableKeyEntry(struct vnt_private *priv, unsigned int uEntryIdx)
{
	void __iomem *io_base = priv->PortOffset;
	unsigned short offset;

	offset = MISCFIFO_KEYETRY0;
	offset += (uEntryIdx * MISCFIFO_KEYENTRYSIZE);

	iowrite16(offset, io_base + MAC_REG_MISCFFNDEX);
	iowrite32(0, io_base + MAC_REG_MISCFFDATA);
	iowrite16(MISCFFCTL_WRITE, io_base + MAC_REG_MISCFFCTL);
>>>>>>> v4.9.227
}
