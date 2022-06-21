/*******************************************************************************
<<<<<<< HEAD
  Header File to describe the DMA descriptors.
  Enhanced descriptors have been in case of DWMAC1000 Cores.
=======
  Header File to describe the DMA descriptors and related definitions.
  This is for DWMAC100 and 1000 cores.
>>>>>>> v4.9.227

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Author: Giuseppe Cavallaro <peppe.cavallaro@st.com>
*******************************************************************************/

#ifndef __DESCS_H__
#define __DESCS_H__

<<<<<<< HEAD
/* Basic descriptor structure for normal and alternate descriptors */
struct dma_desc {
	/* Receive descriptor */
	union {
		struct {
			/* RDES0 */
			u32 payload_csum_error:1;
			u32 crc_error:1;
			u32 dribbling:1;
			u32 mii_error:1;
			u32 receive_watchdog:1;
			u32 frame_type:1;
			u32 collision:1;
			u32 ipc_csum_error:1;
			u32 last_descriptor:1;
			u32 first_descriptor:1;
			u32 vlan_tag:1;
			u32 overflow_error:1;
			u32 length_error:1;
			u32 sa_filter_fail:1;
			u32 descriptor_error:1;
			u32 error_summary:1;
			u32 frame_length:14;
			u32 da_filter_fail:1;
			u32 own:1;
			/* RDES1 */
			u32 buffer1_size:11;
			u32 buffer2_size:11;
			u32 reserved1:2;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 reserved2:5;
			u32 disable_ic:1;

		} rx;
		struct {
			/* RDES0 */
			u32 rx_mac_addr:1;
			u32 crc_error:1;
			u32 dribbling:1;
			u32 error_gmii:1;
			u32 receive_watchdog:1;
			u32 frame_type:1;
			u32 late_collision:1;
			u32 ipc_csum_error:1;
			u32 last_descriptor:1;
			u32 first_descriptor:1;
			u32 vlan_tag:1;
			u32 overflow_error:1;
			u32 length_error:1;
			u32 sa_filter_fail:1;
			u32 descriptor_error:1;
			u32 error_summary:1;
			u32 frame_length:14;
			u32 da_filter_fail:1;
			u32 own:1;
			/* RDES1 */
			u32 buffer1_size:13;
			u32 reserved1:1;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 buffer2_size:13;
			u32 reserved2:2;
			u32 disable_ic:1;
		} erx;		/* -- enhanced -- */

		/* Transmit descriptor */
		struct {
			/* TDES0 */
			u32 deferred:1;
			u32 underflow_error:1;
			u32 excessive_deferral:1;
			u32 collision_count:4;
			u32 vlan_frame:1;
			u32 excessive_collisions:1;
			u32 late_collision:1;
			u32 no_carrier:1;
			u32 loss_carrier:1;
			u32 payload_error:1;
			u32 frame_flushed:1;
			u32 jabber_timeout:1;
			u32 error_summary:1;
			u32 ip_header_error:1;
			u32 time_stamp_status:1;
			u32 reserved1:13;
			u32 own:1;
			/* TDES1 */
			u32 buffer1_size:11;
			u32 buffer2_size:11;
			u32 time_stamp_enable:1;
			u32 disable_padding:1;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 crc_disable:1;
			u32 checksum_insertion:2;
			u32 first_segment:1;
			u32 last_segment:1;
			u32 interrupt:1;
		} tx;
		struct {
			/* TDES0 */
			u32 deferred:1;
			u32 underflow_error:1;
			u32 excessive_deferral:1;
			u32 collision_count:4;
			u32 vlan_frame:1;
			u32 excessive_collisions:1;
			u32 late_collision:1;
			u32 no_carrier:1;
			u32 loss_carrier:1;
			u32 payload_error:1;
			u32 frame_flushed:1;
			u32 jabber_timeout:1;
			u32 error_summary:1;
			u32 ip_header_error:1;
			u32 time_stamp_status:1;
			u32 reserved1:2;
			u32 second_address_chained:1;
			u32 end_ring:1;
			u32 checksum_insertion:2;
			u32 reserved2:1;
			u32 time_stamp_enable:1;
			u32 disable_padding:1;
			u32 crc_disable:1;
			u32 first_segment:1;
			u32 last_segment:1;
			u32 interrupt:1;
			u32 own:1;
			/* TDES1 */
			u32 buffer1_size:13;
			u32 reserved3:3;
			u32 buffer2_size:13;
			u32 reserved4:3;
		} etx;		/* -- enhanced -- */

		u64 all_flags;
	} des01;
	unsigned int des2;
	unsigned int des3;
};

/* Extended descriptor structure (supported by new SYNP GMAC generations) */
struct dma_extended_desc {
	struct dma_desc basic;
	union {
		struct {
			u32 ip_payload_type:3;
			u32 ip_hdr_err:1;
			u32 ip_payload_err:1;
			u32 ip_csum_bypassed:1;
			u32 ipv4_pkt_rcvd:1;
			u32 ipv6_pkt_rcvd:1;
			u32 msg_type:4;
			u32 ptp_frame_type:1;
			u32 ptp_ver:1;
			u32 timestamp_dropped:1;
			u32 reserved:1;
			u32 av_pkt_rcvd:1;
			u32 av_tagged_pkt_rcvd:1;
			u32 vlan_tag_priority_val:3;
			u32 reserved3:3;
			u32 l3_filter_match:1;
			u32 l4_filter_match:1;
			u32 l3_l4_filter_no_match:2;
			u32 reserved4:4;
		} erx;
		struct {
			u32 reserved;
		} etx;
	} des4;
	unsigned int des5;	/* Reserved */
	unsigned int des6;	/* Tx/Rx Timestamp Low */
	unsigned int des7;	/* Tx/Rx Timestamp High */
};

/* Transmit checksum insertion control */
enum tdes_csum_insertion {
	cic_disabled = 0,	/* Checksum Insertion Control */
	cic_only_ip = 1,	/* Only IP header */
	/* IP header but pseudoheader is not calculated */
	cic_no_pseudoheader = 2,
	cic_full = 3,		/* IP header and pseudoheader */
};

/* Extended RDES4 definitions */
#define RDES_EXT_NO_PTP			0
=======
#include <linux/bitops.h>

/* Normal receive descriptor defines */

/* RDES0 */
#define	RDES0_PAYLOAD_CSUM_ERR	BIT(0)
#define	RDES0_CRC_ERROR		BIT(1)
#define	RDES0_DRIBBLING		BIT(2)
#define	RDES0_MII_ERROR		BIT(3)
#define	RDES0_RECEIVE_WATCHDOG	BIT(4)
#define	RDES0_FRAME_TYPE	BIT(5)
#define	RDES0_COLLISION		BIT(6)
#define	RDES0_IPC_CSUM_ERROR	BIT(7)
#define	RDES0_LAST_DESCRIPTOR	BIT(8)
#define	RDES0_FIRST_DESCRIPTOR	BIT(9)
#define	RDES0_VLAN_TAG		BIT(10)
#define	RDES0_OVERFLOW_ERROR	BIT(11)
#define	RDES0_LENGTH_ERROR	BIT(12)
#define	RDES0_SA_FILTER_FAIL	BIT(13)
#define	RDES0_DESCRIPTOR_ERROR	BIT(14)
#define	RDES0_ERROR_SUMMARY	BIT(15)
#define	RDES0_FRAME_LEN_MASK	GENMASK(29, 16)
#define RDES0_FRAME_LEN_SHIFT	16
#define	RDES0_DA_FILTER_FAIL	BIT(30)
#define	RDES0_OWN		BIT(31)
			/* RDES1 */
#define	RDES1_BUFFER1_SIZE_MASK		GENMASK(10, 0)
#define	RDES1_BUFFER2_SIZE_MASK		GENMASK(21, 11)
#define	RDES1_BUFFER2_SIZE_SHIFT	11
#define	RDES1_SECOND_ADDRESS_CHAINED	BIT(24)
#define	RDES1_END_RING			BIT(25)
#define	RDES1_DISABLE_IC		BIT(31)

/* Enhanced receive descriptor defines */

/* RDES0 (similar to normal RDES) */
#define	 ERDES0_RX_MAC_ADDR	BIT(0)

/* RDES1: completely differ from normal desc definitions */
#define	ERDES1_BUFFER1_SIZE_MASK	GENMASK(12, 0)
#define	ERDES1_SECOND_ADDRESS_CHAINED	BIT(14)
#define	ERDES1_END_RING			BIT(15)
#define	ERDES1_BUFFER2_SIZE_MASK	GENMASK(28, 16)
#define ERDES1_BUFFER2_SIZE_SHIFT	16
#define	ERDES1_DISABLE_IC		BIT(31)

/* Normal transmit descriptor defines */
/* TDES0 */
#define	TDES0_DEFERRED			BIT(0)
#define	TDES0_UNDERFLOW_ERROR		BIT(1)
#define	TDES0_EXCESSIVE_DEFERRAL	BIT(2)
#define	TDES0_COLLISION_COUNT_MASK	GENMASK(6, 3)
#define	TDES0_VLAN_FRAME		BIT(7)
#define	TDES0_EXCESSIVE_COLLISIONS	BIT(8)
#define	TDES0_LATE_COLLISION		BIT(9)
#define	TDES0_NO_CARRIER		BIT(10)
#define	TDES0_LOSS_CARRIER		BIT(11)
#define	TDES0_PAYLOAD_ERROR		BIT(12)
#define	TDES0_FRAME_FLUSHED		BIT(13)
#define	TDES0_JABBER_TIMEOUT		BIT(14)
#define	TDES0_ERROR_SUMMARY		BIT(15)
#define	TDES0_IP_HEADER_ERROR		BIT(16)
#define	TDES0_TIME_STAMP_STATUS		BIT(17)
#define	TDES0_OWN			((u32)BIT(31))	/* silence sparse */
/* TDES1 */
#define	TDES1_BUFFER1_SIZE_MASK		GENMASK(10, 0)
#define	TDES1_BUFFER2_SIZE_MASK		GENMASK(21, 11)
#define	TDES1_BUFFER2_SIZE_SHIFT	11
#define	TDES1_TIME_STAMP_ENABLE		BIT(22)
#define	TDES1_DISABLE_PADDING		BIT(23)
#define	TDES1_SECOND_ADDRESS_CHAINED	BIT(24)
#define	TDES1_END_RING			BIT(25)
#define	TDES1_CRC_DISABLE		BIT(26)
#define	TDES1_CHECKSUM_INSERTION_MASK	GENMASK(28, 27)
#define	TDES1_CHECKSUM_INSERTION_SHIFT	27
#define	TDES1_FIRST_SEGMENT		BIT(29)
#define	TDES1_LAST_SEGMENT		BIT(30)
#define	TDES1_INTERRUPT			BIT(31)

/* Enhanced transmit descriptor defines */
/* TDES0 */
#define	ETDES0_DEFERRED			BIT(0)
#define	ETDES0_UNDERFLOW_ERROR		BIT(1)
#define	ETDES0_EXCESSIVE_DEFERRAL	BIT(2)
#define	ETDES0_COLLISION_COUNT_MASK	GENMASK(6, 3)
#define	ETDES0_VLAN_FRAME		BIT(7)
#define	ETDES0_EXCESSIVE_COLLISIONS	BIT(8)
#define	ETDES0_LATE_COLLISION		BIT(9)
#define	ETDES0_NO_CARRIER		BIT(10)
#define	ETDES0_LOSS_CARRIER		BIT(11)
#define	ETDES0_PAYLOAD_ERROR		BIT(12)
#define	ETDES0_FRAME_FLUSHED		BIT(13)
#define	ETDES0_JABBER_TIMEOUT		BIT(14)
#define	ETDES0_ERROR_SUMMARY		BIT(15)
#define	ETDES0_IP_HEADER_ERROR		BIT(16)
#define	ETDES0_TIME_STAMP_STATUS	BIT(17)
#define	ETDES0_SECOND_ADDRESS_CHAINED	BIT(20)
#define	ETDES0_END_RING			BIT(21)
#define	ETDES0_CHECKSUM_INSERTION_MASK	GENMASK(23, 22)
#define	ETDES0_CHECKSUM_INSERTION_SHIFT	22
#define	ETDES0_TIME_STAMP_ENABLE	BIT(25)
#define	ETDES0_DISABLE_PADDING		BIT(26)
#define	ETDES0_CRC_DISABLE		BIT(27)
#define	ETDES0_FIRST_SEGMENT		BIT(28)
#define	ETDES0_LAST_SEGMENT		BIT(29)
#define	ETDES0_INTERRUPT		BIT(30)
#define	ETDES0_OWN			((u32)BIT(31))	/* silence sparse */
/* TDES1 */
#define	ETDES1_BUFFER1_SIZE_MASK	GENMASK(12, 0)
#define	ETDES1_BUFFER2_SIZE_MASK	GENMASK(28, 16)
#define	ETDES1_BUFFER2_SIZE_SHIFT	16

/* Extended Receive descriptor definitions */
#define	ERDES4_IP_PAYLOAD_TYPE_MASK	GENMASK(2, 6)
#define	ERDES4_IP_HDR_ERR		BIT(3)
#define	ERDES4_IP_PAYLOAD_ERR		BIT(4)
#define	ERDES4_IP_CSUM_BYPASSED		BIT(5)
#define	ERDES4_IPV4_PKT_RCVD		BIT(6)
#define	ERDES4_IPV6_PKT_RCVD		BIT(7)
#define	ERDES4_MSG_TYPE_MASK		GENMASK(11, 8)
#define	ERDES4_PTP_FRAME_TYPE		BIT(12)
#define	ERDES4_PTP_VER			BIT(13)
#define	ERDES4_TIMESTAMP_DROPPED	BIT(14)
#define	ERDES4_AV_PKT_RCVD		BIT(16)
#define	ERDES4_AV_TAGGED_PKT_RCVD	BIT(17)
#define	ERDES4_VLAN_TAG_PRI_VAL_MASK	GENMASK(20, 18)
#define	ERDES4_L3_FILTER_MATCH		BIT(24)
#define	ERDES4_L4_FILTER_MATCH		BIT(25)
#define	ERDES4_L3_L4_FILT_NO_MATCH_MASK	GENMASK(27, 26)

/* Extended RDES4 message type definitions */
#define RDES_EXT_NO_PTP			0x0
>>>>>>> v4.9.227
#define RDES_EXT_SYNC			0x1
#define RDES_EXT_FOLLOW_UP		0x2
#define RDES_EXT_DELAY_REQ		0x3
#define RDES_EXT_DELAY_RESP		0x4
#define RDES_EXT_PDELAY_REQ		0x5
#define RDES_EXT_PDELAY_RESP		0x6
#define RDES_EXT_PDELAY_FOLLOW_UP	0x7
<<<<<<< HEAD
=======
#define RDES_PTP_ANNOUNCE		0x8
#define RDES_PTP_MANAGEMENT		0x9
#define RDES_PTP_SIGNALING		0xa
#define RDES_PTP_PKT_RESERVED_TYPE	0xf

/* Basic descriptor structure for normal and alternate descriptors */
struct dma_desc {
	__le32 des0;
	__le32 des1;
	__le32 des2;
	__le32 des3;
};

/* Extended descriptor structure (e.g. >= databook 3.50a) */
struct dma_extended_desc {
	struct dma_desc basic;	/* Basic descriptors */
	__le32 des4;	/* Extended Status */
	__le32 des5;	/* Reserved */
	__le32 des6;	/* Tx/Rx Timestamp Low */
	__le32 des7;	/* Tx/Rx Timestamp High */
};

/* Transmit checksum insertion control */
#define	TX_CIC_FULL	3	/* Include IP header and pseudoheader */
>>>>>>> v4.9.227

#endif /* __DESCS_H__ */
