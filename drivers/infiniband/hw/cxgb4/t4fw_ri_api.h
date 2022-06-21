/*
 * Copyright (c) 2009-2010 Chelsio, Inc. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef _T4FW_RI_API_H_
#define _T4FW_RI_API_H_

#include "t4fw_api.h"

enum fw_ri_wr_opcode {
	FW_RI_RDMA_WRITE		= 0x0,	/* IETF RDMAP v1.0 ... */
	FW_RI_READ_REQ			= 0x1,
	FW_RI_READ_RESP			= 0x2,
	FW_RI_SEND			= 0x3,
	FW_RI_SEND_WITH_INV		= 0x4,
	FW_RI_SEND_WITH_SE		= 0x5,
	FW_RI_SEND_WITH_SE_INV		= 0x6,
	FW_RI_TERMINATE			= 0x7,
	FW_RI_RDMA_INIT			= 0x8,	/* CHELSIO RI specific ... */
	FW_RI_BIND_MW			= 0x9,
	FW_RI_FAST_REGISTER		= 0xa,
	FW_RI_LOCAL_INV			= 0xb,
	FW_RI_QP_MODIFY			= 0xc,
	FW_RI_BYPASS			= 0xd,
	FW_RI_RECEIVE			= 0xe,

	FW_RI_SGE_EC_CR_RETURN		= 0xf
};

enum fw_ri_wr_flags {
	FW_RI_COMPLETION_FLAG		= 0x01,
	FW_RI_NOTIFICATION_FLAG		= 0x02,
	FW_RI_SOLICITED_EVENT_FLAG	= 0x04,
	FW_RI_READ_FENCE_FLAG		= 0x08,
	FW_RI_LOCAL_FENCE_FLAG		= 0x10,
	FW_RI_RDMA_READ_INVALIDATE	= 0x20
};

enum fw_ri_mpa_attrs {
	FW_RI_MPA_RX_MARKER_ENABLE	= 0x01,
	FW_RI_MPA_TX_MARKER_ENABLE	= 0x02,
	FW_RI_MPA_CRC_ENABLE		= 0x04,
	FW_RI_MPA_IETF_ENABLE		= 0x08
};

enum fw_ri_qp_caps {
	FW_RI_QP_RDMA_READ_ENABLE	= 0x01,
	FW_RI_QP_RDMA_WRITE_ENABLE	= 0x02,
	FW_RI_QP_BIND_ENABLE		= 0x04,
	FW_RI_QP_FAST_REGISTER_ENABLE	= 0x08,
	FW_RI_QP_STAG0_ENABLE		= 0x10
};

enum fw_ri_addr_type {
	FW_RI_ZERO_BASED_TO		= 0x00,
	FW_RI_VA_BASED_TO		= 0x01
};

enum fw_ri_mem_perms {
	FW_RI_MEM_ACCESS_REM_WRITE	= 0x01,
	FW_RI_MEM_ACCESS_REM_READ	= 0x02,
	FW_RI_MEM_ACCESS_REM		= 0x03,
	FW_RI_MEM_ACCESS_LOCAL_WRITE	= 0x04,
	FW_RI_MEM_ACCESS_LOCAL_READ	= 0x08,
	FW_RI_MEM_ACCESS_LOCAL		= 0x0C
};

enum fw_ri_stag_type {
	FW_RI_STAG_NSMR			= 0x00,
	FW_RI_STAG_SMR			= 0x01,
	FW_RI_STAG_MW			= 0x02,
	FW_RI_STAG_MW_RELAXED		= 0x03
};

enum fw_ri_data_op {
	FW_RI_DATA_IMMD			= 0x81,
	FW_RI_DATA_DSGL			= 0x82,
	FW_RI_DATA_ISGL			= 0x83
};

enum fw_ri_sgl_depth {
	FW_RI_SGL_DEPTH_MAX_SQ		= 16,
	FW_RI_SGL_DEPTH_MAX_RQ		= 4
};

struct fw_ri_dsge_pair {
	__be32	len[2];
	__be64	addr[2];
};

struct fw_ri_dsgl {
	__u8	op;
	__u8	r1;
	__be16	nsge;
	__be32	len0;
	__be64	addr0;
#ifndef C99_NOT_SUPPORTED
	struct fw_ri_dsge_pair sge[0];
#endif
};

struct fw_ri_sge {
	__be32 stag;
	__be32 len;
	__be64 to;
};

struct fw_ri_isgl {
	__u8	op;
	__u8	r1;
	__be16	nsge;
	__be32	r2;
#ifndef C99_NOT_SUPPORTED
	struct fw_ri_sge sge[0];
#endif
};

struct fw_ri_immd {
	__u8	op;
	__u8	r1;
	__be16	r2;
	__be32	immdlen;
#ifndef C99_NOT_SUPPORTED
	__u8	data[0];
#endif
};

struct fw_ri_tpte {
	__be32 valid_to_pdid;
	__be32 locread_to_qpid;
	__be32 nosnoop_pbladdr;
	__be32 len_lo;
	__be32 va_hi;
	__be32 va_lo_fbo;
	__be32 dca_mwbcnt_pstag;
	__be32 len_hi;
};

<<<<<<< HEAD
#define S_FW_RI_TPTE_VALID		31
#define M_FW_RI_TPTE_VALID		0x1
#define V_FW_RI_TPTE_VALID(x)		((x) << S_FW_RI_TPTE_VALID)
#define G_FW_RI_TPTE_VALID(x)		\
    (((x) >> S_FW_RI_TPTE_VALID) & M_FW_RI_TPTE_VALID)
#define F_FW_RI_TPTE_VALID		V_FW_RI_TPTE_VALID(1U)

#define S_FW_RI_TPTE_STAGKEY		23
#define M_FW_RI_TPTE_STAGKEY		0xff
#define V_FW_RI_TPTE_STAGKEY(x)		((x) << S_FW_RI_TPTE_STAGKEY)
#define G_FW_RI_TPTE_STAGKEY(x)		\
    (((x) >> S_FW_RI_TPTE_STAGKEY) & M_FW_RI_TPTE_STAGKEY)

#define S_FW_RI_TPTE_STAGSTATE		22
#define M_FW_RI_TPTE_STAGSTATE		0x1
#define V_FW_RI_TPTE_STAGSTATE(x)	((x) << S_FW_RI_TPTE_STAGSTATE)
#define G_FW_RI_TPTE_STAGSTATE(x)	\
    (((x) >> S_FW_RI_TPTE_STAGSTATE) & M_FW_RI_TPTE_STAGSTATE)
#define F_FW_RI_TPTE_STAGSTATE		V_FW_RI_TPTE_STAGSTATE(1U)

#define S_FW_RI_TPTE_STAGTYPE		20
#define M_FW_RI_TPTE_STAGTYPE		0x3
#define V_FW_RI_TPTE_STAGTYPE(x)	((x) << S_FW_RI_TPTE_STAGTYPE)
#define G_FW_RI_TPTE_STAGTYPE(x)	\
    (((x) >> S_FW_RI_TPTE_STAGTYPE) & M_FW_RI_TPTE_STAGTYPE)

#define S_FW_RI_TPTE_PDID		0
#define M_FW_RI_TPTE_PDID		0xfffff
#define V_FW_RI_TPTE_PDID(x)		((x) << S_FW_RI_TPTE_PDID)
#define G_FW_RI_TPTE_PDID(x)		\
    (((x) >> S_FW_RI_TPTE_PDID) & M_FW_RI_TPTE_PDID)

#define S_FW_RI_TPTE_PERM		28
#define M_FW_RI_TPTE_PERM		0xf
#define V_FW_RI_TPTE_PERM(x)		((x) << S_FW_RI_TPTE_PERM)
#define G_FW_RI_TPTE_PERM(x)		\
    (((x) >> S_FW_RI_TPTE_PERM) & M_FW_RI_TPTE_PERM)

#define S_FW_RI_TPTE_REMINVDIS		27
#define M_FW_RI_TPTE_REMINVDIS		0x1
#define V_FW_RI_TPTE_REMINVDIS(x)	((x) << S_FW_RI_TPTE_REMINVDIS)
#define G_FW_RI_TPTE_REMINVDIS(x)	\
    (((x) >> S_FW_RI_TPTE_REMINVDIS) & M_FW_RI_TPTE_REMINVDIS)
#define F_FW_RI_TPTE_REMINVDIS		V_FW_RI_TPTE_REMINVDIS(1U)

#define S_FW_RI_TPTE_ADDRTYPE		26
#define M_FW_RI_TPTE_ADDRTYPE		1
#define V_FW_RI_TPTE_ADDRTYPE(x)	((x) << S_FW_RI_TPTE_ADDRTYPE)
#define G_FW_RI_TPTE_ADDRTYPE(x)	\
    (((x) >> S_FW_RI_TPTE_ADDRTYPE) & M_FW_RI_TPTE_ADDRTYPE)
#define F_FW_RI_TPTE_ADDRTYPE		V_FW_RI_TPTE_ADDRTYPE(1U)

#define S_FW_RI_TPTE_MWBINDEN		25
#define M_FW_RI_TPTE_MWBINDEN		0x1
#define V_FW_RI_TPTE_MWBINDEN(x)	((x) << S_FW_RI_TPTE_MWBINDEN)
#define G_FW_RI_TPTE_MWBINDEN(x)	\
    (((x) >> S_FW_RI_TPTE_MWBINDEN) & M_FW_RI_TPTE_MWBINDEN)
#define F_FW_RI_TPTE_MWBINDEN		V_FW_RI_TPTE_MWBINDEN(1U)

#define S_FW_RI_TPTE_PS			20
#define M_FW_RI_TPTE_PS			0x1f
#define V_FW_RI_TPTE_PS(x)		((x) << S_FW_RI_TPTE_PS)
#define G_FW_RI_TPTE_PS(x)		\
    (((x) >> S_FW_RI_TPTE_PS) & M_FW_RI_TPTE_PS)

#define S_FW_RI_TPTE_QPID		0
#define M_FW_RI_TPTE_QPID		0xfffff
#define V_FW_RI_TPTE_QPID(x)		((x) << S_FW_RI_TPTE_QPID)
#define G_FW_RI_TPTE_QPID(x)		\
    (((x) >> S_FW_RI_TPTE_QPID) & M_FW_RI_TPTE_QPID)

#define S_FW_RI_TPTE_NOSNOOP		30
#define M_FW_RI_TPTE_NOSNOOP		0x1
#define V_FW_RI_TPTE_NOSNOOP(x)		((x) << S_FW_RI_TPTE_NOSNOOP)
#define G_FW_RI_TPTE_NOSNOOP(x)		\
    (((x) >> S_FW_RI_TPTE_NOSNOOP) & M_FW_RI_TPTE_NOSNOOP)
#define F_FW_RI_TPTE_NOSNOOP		V_FW_RI_TPTE_NOSNOOP(1U)

#define S_FW_RI_TPTE_PBLADDR		0
#define M_FW_RI_TPTE_PBLADDR		0x1fffffff
#define V_FW_RI_TPTE_PBLADDR(x)		((x) << S_FW_RI_TPTE_PBLADDR)
#define G_FW_RI_TPTE_PBLADDR(x)		\
    (((x) >> S_FW_RI_TPTE_PBLADDR) & M_FW_RI_TPTE_PBLADDR)

#define S_FW_RI_TPTE_DCA		24
#define M_FW_RI_TPTE_DCA		0x1f
#define V_FW_RI_TPTE_DCA(x)		((x) << S_FW_RI_TPTE_DCA)
#define G_FW_RI_TPTE_DCA(x)		\
    (((x) >> S_FW_RI_TPTE_DCA) & M_FW_RI_TPTE_DCA)

#define S_FW_RI_TPTE_MWBCNT_PSTAG	0
#define M_FW_RI_TPTE_MWBCNT_PSTAG	0xffffff
#define V_FW_RI_TPTE_MWBCNT_PSTAT(x)	\
    ((x) << S_FW_RI_TPTE_MWBCNT_PSTAG)
#define G_FW_RI_TPTE_MWBCNT_PSTAG(x)	\
    (((x) >> S_FW_RI_TPTE_MWBCNT_PSTAG) & M_FW_RI_TPTE_MWBCNT_PSTAG)
=======
#define FW_RI_TPTE_VALID_S		31
#define FW_RI_TPTE_VALID_M		0x1
#define FW_RI_TPTE_VALID_V(x)		((x) << FW_RI_TPTE_VALID_S)
#define FW_RI_TPTE_VALID_G(x)		\
	(((x) >> FW_RI_TPTE_VALID_S) & FW_RI_TPTE_VALID_M)
#define FW_RI_TPTE_VALID_F		FW_RI_TPTE_VALID_V(1U)

#define FW_RI_TPTE_STAGKEY_S		23
#define FW_RI_TPTE_STAGKEY_M		0xff
#define FW_RI_TPTE_STAGKEY_V(x)		((x) << FW_RI_TPTE_STAGKEY_S)
#define FW_RI_TPTE_STAGKEY_G(x)		\
	(((x) >> FW_RI_TPTE_STAGKEY_S) & FW_RI_TPTE_STAGKEY_M)

#define FW_RI_TPTE_STAGSTATE_S		22
#define FW_RI_TPTE_STAGSTATE_M		0x1
#define FW_RI_TPTE_STAGSTATE_V(x)	((x) << FW_RI_TPTE_STAGSTATE_S)
#define FW_RI_TPTE_STAGSTATE_G(x)	\
	(((x) >> FW_RI_TPTE_STAGSTATE_S) & FW_RI_TPTE_STAGSTATE_M)
#define FW_RI_TPTE_STAGSTATE_F		FW_RI_TPTE_STAGSTATE_V(1U)

#define FW_RI_TPTE_STAGTYPE_S		20
#define FW_RI_TPTE_STAGTYPE_M		0x3
#define FW_RI_TPTE_STAGTYPE_V(x)	((x) << FW_RI_TPTE_STAGTYPE_S)
#define FW_RI_TPTE_STAGTYPE_G(x)	\
	(((x) >> FW_RI_TPTE_STAGTYPE_S) & FW_RI_TPTE_STAGTYPE_M)

#define FW_RI_TPTE_PDID_S		0
#define FW_RI_TPTE_PDID_M		0xfffff
#define FW_RI_TPTE_PDID_V(x)		((x) << FW_RI_TPTE_PDID_S)
#define FW_RI_TPTE_PDID_G(x)		\
	(((x) >> FW_RI_TPTE_PDID_S) & FW_RI_TPTE_PDID_M)

#define FW_RI_TPTE_PERM_S		28
#define FW_RI_TPTE_PERM_M		0xf
#define FW_RI_TPTE_PERM_V(x)		((x) << FW_RI_TPTE_PERM_S)
#define FW_RI_TPTE_PERM_G(x)		\
	(((x) >> FW_RI_TPTE_PERM_S) & FW_RI_TPTE_PERM_M)

#define FW_RI_TPTE_REMINVDIS_S		27
#define FW_RI_TPTE_REMINVDIS_M		0x1
#define FW_RI_TPTE_REMINVDIS_V(x)	((x) << FW_RI_TPTE_REMINVDIS_S)
#define FW_RI_TPTE_REMINVDIS_G(x)	\
	(((x) >> FW_RI_TPTE_REMINVDIS_S) & FW_RI_TPTE_REMINVDIS_M)
#define FW_RI_TPTE_REMINVDIS_F		FW_RI_TPTE_REMINVDIS_V(1U)

#define FW_RI_TPTE_ADDRTYPE_S		26
#define FW_RI_TPTE_ADDRTYPE_M		1
#define FW_RI_TPTE_ADDRTYPE_V(x)	((x) << FW_RI_TPTE_ADDRTYPE_S)
#define FW_RI_TPTE_ADDRTYPE_G(x)	\
	(((x) >> FW_RI_TPTE_ADDRTYPE_S) & FW_RI_TPTE_ADDRTYPE_M)
#define FW_RI_TPTE_ADDRTYPE_F		FW_RI_TPTE_ADDRTYPE_V(1U)

#define FW_RI_TPTE_MWBINDEN_S		25
#define FW_RI_TPTE_MWBINDEN_M		0x1
#define FW_RI_TPTE_MWBINDEN_V(x)	((x) << FW_RI_TPTE_MWBINDEN_S)
#define FW_RI_TPTE_MWBINDEN_G(x)	\
	(((x) >> FW_RI_TPTE_MWBINDEN_S) & FW_RI_TPTE_MWBINDEN_M)
#define FW_RI_TPTE_MWBINDEN_F		FW_RI_TPTE_MWBINDEN_V(1U)

#define FW_RI_TPTE_PS_S			20
#define FW_RI_TPTE_PS_M			0x1f
#define FW_RI_TPTE_PS_V(x)		((x) << FW_RI_TPTE_PS_S)
#define FW_RI_TPTE_PS_G(x)		\
	(((x) >> FW_RI_TPTE_PS_S) & FW_RI_TPTE_PS_M)

#define FW_RI_TPTE_QPID_S		0
#define FW_RI_TPTE_QPID_M		0xfffff
#define FW_RI_TPTE_QPID_V(x)		((x) << FW_RI_TPTE_QPID_S)
#define FW_RI_TPTE_QPID_G(x)		\
	(((x) >> FW_RI_TPTE_QPID_S) & FW_RI_TPTE_QPID_M)

#define FW_RI_TPTE_NOSNOOP_S		30
#define FW_RI_TPTE_NOSNOOP_M		0x1
#define FW_RI_TPTE_NOSNOOP_V(x)		((x) << FW_RI_TPTE_NOSNOOP_S)
#define FW_RI_TPTE_NOSNOOP_G(x)		\
	(((x) >> FW_RI_TPTE_NOSNOOP_S) & FW_RI_TPTE_NOSNOOP_M)
#define FW_RI_TPTE_NOSNOOP_F		FW_RI_TPTE_NOSNOOP_V(1U)

#define FW_RI_TPTE_PBLADDR_S		0
#define FW_RI_TPTE_PBLADDR_M		0x1fffffff
#define FW_RI_TPTE_PBLADDR_V(x)		((x) << FW_RI_TPTE_PBLADDR_S)
#define FW_RI_TPTE_PBLADDR_G(x)		\
	(((x) >> FW_RI_TPTE_PBLADDR_S) & FW_RI_TPTE_PBLADDR_M)

#define FW_RI_TPTE_DCA_S		24
#define FW_RI_TPTE_DCA_M		0x1f
#define FW_RI_TPTE_DCA_V(x)		((x) << FW_RI_TPTE_DCA_S)
#define FW_RI_TPTE_DCA_G(x)		\
	(((x) >> FW_RI_TPTE_DCA_S) & FW_RI_TPTE_DCA_M)

#define FW_RI_TPTE_MWBCNT_PSTAG_S	0
#define FW_RI_TPTE_MWBCNT_PSTAG_M	0xffffff
#define FW_RI_TPTE_MWBCNT_PSTAT_V(x)	\
	((x) << FW_RI_TPTE_MWBCNT_PSTAG_S)
#define FW_RI_TPTE_MWBCNT_PSTAG_G(x)	\
	(((x) >> FW_RI_TPTE_MWBCNT_PSTAG_S) & FW_RI_TPTE_MWBCNT_PSTAG_M)
>>>>>>> v4.9.227

enum fw_ri_res_type {
	FW_RI_RES_TYPE_SQ,
	FW_RI_RES_TYPE_RQ,
	FW_RI_RES_TYPE_CQ,
};

enum fw_ri_res_op {
	FW_RI_RES_OP_WRITE,
	FW_RI_RES_OP_RESET,
};

struct fw_ri_res {
	union fw_ri_restype {
		struct fw_ri_res_sqrq {
			__u8   restype;
			__u8   op;
			__be16 r3;
			__be32 eqid;
			__be32 r4[2];
			__be32 fetchszm_to_iqid;
			__be32 dcaen_to_eqsize;
			__be64 eqaddr;
		} sqrq;
		struct fw_ri_res_cq {
			__u8   restype;
			__u8   op;
			__be16 r3;
			__be32 iqid;
			__be32 r4[2];
			__be32 iqandst_to_iqandstindex;
			__be16 iqdroprss_to_iqesize;
			__be16 iqsize;
			__be64 iqaddr;
			__be32 iqns_iqro;
			__be32 r6_lo;
			__be64 r7;
		} cq;
	} u;
};

struct fw_ri_res_wr {
	__be32 op_nres;
	__be32 len16_pkd;
	__u64  cookie;
#ifndef C99_NOT_SUPPORTED
	struct fw_ri_res res[0];
#endif
};

<<<<<<< HEAD
#define S_FW_RI_RES_WR_NRES	0
#define M_FW_RI_RES_WR_NRES	0xff
#define V_FW_RI_RES_WR_NRES(x)	((x) << S_FW_RI_RES_WR_NRES)
#define G_FW_RI_RES_WR_NRES(x)	\
    (((x) >> S_FW_RI_RES_WR_NRES) & M_FW_RI_RES_WR_NRES)

#define S_FW_RI_RES_WR_FETCHSZM		26
#define M_FW_RI_RES_WR_FETCHSZM		0x1
#define V_FW_RI_RES_WR_FETCHSZM(x)	((x) << S_FW_RI_RES_WR_FETCHSZM)
#define G_FW_RI_RES_WR_FETCHSZM(x)	\
    (((x) >> S_FW_RI_RES_WR_FETCHSZM) & M_FW_RI_RES_WR_FETCHSZM)
#define F_FW_RI_RES_WR_FETCHSZM	V_FW_RI_RES_WR_FETCHSZM(1U)

#define S_FW_RI_RES_WR_STATUSPGNS	25
#define M_FW_RI_RES_WR_STATUSPGNS	0x1
#define V_FW_RI_RES_WR_STATUSPGNS(x)	((x) << S_FW_RI_RES_WR_STATUSPGNS)
#define G_FW_RI_RES_WR_STATUSPGNS(x)	\
    (((x) >> S_FW_RI_RES_WR_STATUSPGNS) & M_FW_RI_RES_WR_STATUSPGNS)
#define F_FW_RI_RES_WR_STATUSPGNS	V_FW_RI_RES_WR_STATUSPGNS(1U)

#define S_FW_RI_RES_WR_STATUSPGRO	24
#define M_FW_RI_RES_WR_STATUSPGRO	0x1
#define V_FW_RI_RES_WR_STATUSPGRO(x)	((x) << S_FW_RI_RES_WR_STATUSPGRO)
#define G_FW_RI_RES_WR_STATUSPGRO(x)	\
    (((x) >> S_FW_RI_RES_WR_STATUSPGRO) & M_FW_RI_RES_WR_STATUSPGRO)
#define F_FW_RI_RES_WR_STATUSPGRO	V_FW_RI_RES_WR_STATUSPGRO(1U)

#define S_FW_RI_RES_WR_FETCHNS		23
#define M_FW_RI_RES_WR_FETCHNS		0x1
#define V_FW_RI_RES_WR_FETCHNS(x)	((x) << S_FW_RI_RES_WR_FETCHNS)
#define G_FW_RI_RES_WR_FETCHNS(x)	\
    (((x) >> S_FW_RI_RES_WR_FETCHNS) & M_FW_RI_RES_WR_FETCHNS)
#define F_FW_RI_RES_WR_FETCHNS	V_FW_RI_RES_WR_FETCHNS(1U)

#define S_FW_RI_RES_WR_FETCHRO		22
#define M_FW_RI_RES_WR_FETCHRO		0x1
#define V_FW_RI_RES_WR_FETCHRO(x)	((x) << S_FW_RI_RES_WR_FETCHRO)
#define G_FW_RI_RES_WR_FETCHRO(x)	\
    (((x) >> S_FW_RI_RES_WR_FETCHRO) & M_FW_RI_RES_WR_FETCHRO)
#define F_FW_RI_RES_WR_FETCHRO	V_FW_RI_RES_WR_FETCHRO(1U)

#define S_FW_RI_RES_WR_HOSTFCMODE	20
#define M_FW_RI_RES_WR_HOSTFCMODE	0x3
#define V_FW_RI_RES_WR_HOSTFCMODE(x)	((x) << S_FW_RI_RES_WR_HOSTFCMODE)
#define G_FW_RI_RES_WR_HOSTFCMODE(x)	\
    (((x) >> S_FW_RI_RES_WR_HOSTFCMODE) & M_FW_RI_RES_WR_HOSTFCMODE)

#define S_FW_RI_RES_WR_CPRIO	19
#define M_FW_RI_RES_WR_CPRIO	0x1
#define V_FW_RI_RES_WR_CPRIO(x)	((x) << S_FW_RI_RES_WR_CPRIO)
#define G_FW_RI_RES_WR_CPRIO(x)	\
    (((x) >> S_FW_RI_RES_WR_CPRIO) & M_FW_RI_RES_WR_CPRIO)
#define F_FW_RI_RES_WR_CPRIO	V_FW_RI_RES_WR_CPRIO(1U)

#define S_FW_RI_RES_WR_ONCHIP		18
#define M_FW_RI_RES_WR_ONCHIP		0x1
#define V_FW_RI_RES_WR_ONCHIP(x)	((x) << S_FW_RI_RES_WR_ONCHIP)
#define G_FW_RI_RES_WR_ONCHIP(x)	\
    (((x) >> S_FW_RI_RES_WR_ONCHIP) & M_FW_RI_RES_WR_ONCHIP)
#define F_FW_RI_RES_WR_ONCHIP	V_FW_RI_RES_WR_ONCHIP(1U)

#define S_FW_RI_RES_WR_PCIECHN		16
#define M_FW_RI_RES_WR_PCIECHN		0x3
#define V_FW_RI_RES_WR_PCIECHN(x)	((x) << S_FW_RI_RES_WR_PCIECHN)
#define G_FW_RI_RES_WR_PCIECHN(x)	\
    (((x) >> S_FW_RI_RES_WR_PCIECHN) & M_FW_RI_RES_WR_PCIECHN)

#define S_FW_RI_RES_WR_IQID	0
#define M_FW_RI_RES_WR_IQID	0xffff
#define V_FW_RI_RES_WR_IQID(x)	((x) << S_FW_RI_RES_WR_IQID)
#define G_FW_RI_RES_WR_IQID(x)	\
    (((x) >> S_FW_RI_RES_WR_IQID) & M_FW_RI_RES_WR_IQID)

#define S_FW_RI_RES_WR_DCAEN	31
#define M_FW_RI_RES_WR_DCAEN	0x1
#define V_FW_RI_RES_WR_DCAEN(x)	((x) << S_FW_RI_RES_WR_DCAEN)
#define G_FW_RI_RES_WR_DCAEN(x)	\
    (((x) >> S_FW_RI_RES_WR_DCAEN) & M_FW_RI_RES_WR_DCAEN)
#define F_FW_RI_RES_WR_DCAEN	V_FW_RI_RES_WR_DCAEN(1U)

#define S_FW_RI_RES_WR_DCACPU		26
#define M_FW_RI_RES_WR_DCACPU		0x1f
#define V_FW_RI_RES_WR_DCACPU(x)	((x) << S_FW_RI_RES_WR_DCACPU)
#define G_FW_RI_RES_WR_DCACPU(x)	\
    (((x) >> S_FW_RI_RES_WR_DCACPU) & M_FW_RI_RES_WR_DCACPU)

#define S_FW_RI_RES_WR_FBMIN	23
#define M_FW_RI_RES_WR_FBMIN	0x7
#define V_FW_RI_RES_WR_FBMIN(x)	((x) << S_FW_RI_RES_WR_FBMIN)
#define G_FW_RI_RES_WR_FBMIN(x)	\
    (((x) >> S_FW_RI_RES_WR_FBMIN) & M_FW_RI_RES_WR_FBMIN)

#define S_FW_RI_RES_WR_FBMAX	20
#define M_FW_RI_RES_WR_FBMAX	0x7
#define V_FW_RI_RES_WR_FBMAX(x)	((x) << S_FW_RI_RES_WR_FBMAX)
#define G_FW_RI_RES_WR_FBMAX(x)	\
    (((x) >> S_FW_RI_RES_WR_FBMAX) & M_FW_RI_RES_WR_FBMAX)

#define S_FW_RI_RES_WR_CIDXFTHRESHO	19
#define M_FW_RI_RES_WR_CIDXFTHRESHO	0x1
#define V_FW_RI_RES_WR_CIDXFTHRESHO(x)	((x) << S_FW_RI_RES_WR_CIDXFTHRESHO)
#define G_FW_RI_RES_WR_CIDXFTHRESHO(x)	\
    (((x) >> S_FW_RI_RES_WR_CIDXFTHRESHO) & M_FW_RI_RES_WR_CIDXFTHRESHO)
#define F_FW_RI_RES_WR_CIDXFTHRESHO	V_FW_RI_RES_WR_CIDXFTHRESHO(1U)

#define S_FW_RI_RES_WR_CIDXFTHRESH	16
#define M_FW_RI_RES_WR_CIDXFTHRESH	0x7
#define V_FW_RI_RES_WR_CIDXFTHRESH(x)	((x) << S_FW_RI_RES_WR_CIDXFTHRESH)
#define G_FW_RI_RES_WR_CIDXFTHRESH(x)	\
    (((x) >> S_FW_RI_RES_WR_CIDXFTHRESH) & M_FW_RI_RES_WR_CIDXFTHRESH)

#define S_FW_RI_RES_WR_EQSIZE		0
#define M_FW_RI_RES_WR_EQSIZE		0xffff
#define V_FW_RI_RES_WR_EQSIZE(x)	((x) << S_FW_RI_RES_WR_EQSIZE)
#define G_FW_RI_RES_WR_EQSIZE(x)	\
    (((x) >> S_FW_RI_RES_WR_EQSIZE) & M_FW_RI_RES_WR_EQSIZE)

#define S_FW_RI_RES_WR_IQANDST		15
#define M_FW_RI_RES_WR_IQANDST		0x1
#define V_FW_RI_RES_WR_IQANDST(x)	((x) << S_FW_RI_RES_WR_IQANDST)
#define G_FW_RI_RES_WR_IQANDST(x)	\
    (((x) >> S_FW_RI_RES_WR_IQANDST) & M_FW_RI_RES_WR_IQANDST)
#define F_FW_RI_RES_WR_IQANDST	V_FW_RI_RES_WR_IQANDST(1U)

#define S_FW_RI_RES_WR_IQANUS		14
#define M_FW_RI_RES_WR_IQANUS		0x1
#define V_FW_RI_RES_WR_IQANUS(x)	((x) << S_FW_RI_RES_WR_IQANUS)
#define G_FW_RI_RES_WR_IQANUS(x)	\
    (((x) >> S_FW_RI_RES_WR_IQANUS) & M_FW_RI_RES_WR_IQANUS)
#define F_FW_RI_RES_WR_IQANUS	V_FW_RI_RES_WR_IQANUS(1U)

#define S_FW_RI_RES_WR_IQANUD		12
#define M_FW_RI_RES_WR_IQANUD		0x3
#define V_FW_RI_RES_WR_IQANUD(x)	((x) << S_FW_RI_RES_WR_IQANUD)
#define G_FW_RI_RES_WR_IQANUD(x)	\
    (((x) >> S_FW_RI_RES_WR_IQANUD) & M_FW_RI_RES_WR_IQANUD)

#define S_FW_RI_RES_WR_IQANDSTINDEX	0
#define M_FW_RI_RES_WR_IQANDSTINDEX	0xfff
#define V_FW_RI_RES_WR_IQANDSTINDEX(x)	((x) << S_FW_RI_RES_WR_IQANDSTINDEX)
#define G_FW_RI_RES_WR_IQANDSTINDEX(x)	\
    (((x) >> S_FW_RI_RES_WR_IQANDSTINDEX) & M_FW_RI_RES_WR_IQANDSTINDEX)

#define S_FW_RI_RES_WR_IQDROPRSS	15
#define M_FW_RI_RES_WR_IQDROPRSS	0x1
#define V_FW_RI_RES_WR_IQDROPRSS(x)	((x) << S_FW_RI_RES_WR_IQDROPRSS)
#define G_FW_RI_RES_WR_IQDROPRSS(x)	\
    (((x) >> S_FW_RI_RES_WR_IQDROPRSS) & M_FW_RI_RES_WR_IQDROPRSS)
#define F_FW_RI_RES_WR_IQDROPRSS	V_FW_RI_RES_WR_IQDROPRSS(1U)

#define S_FW_RI_RES_WR_IQGTSMODE	14
#define M_FW_RI_RES_WR_IQGTSMODE	0x1
#define V_FW_RI_RES_WR_IQGTSMODE(x)	((x) << S_FW_RI_RES_WR_IQGTSMODE)
#define G_FW_RI_RES_WR_IQGTSMODE(x)	\
    (((x) >> S_FW_RI_RES_WR_IQGTSMODE) & M_FW_RI_RES_WR_IQGTSMODE)
#define F_FW_RI_RES_WR_IQGTSMODE	V_FW_RI_RES_WR_IQGTSMODE(1U)

#define S_FW_RI_RES_WR_IQPCIECH		12
#define M_FW_RI_RES_WR_IQPCIECH		0x3
#define V_FW_RI_RES_WR_IQPCIECH(x)	((x) << S_FW_RI_RES_WR_IQPCIECH)
#define G_FW_RI_RES_WR_IQPCIECH(x)	\
    (((x) >> S_FW_RI_RES_WR_IQPCIECH) & M_FW_RI_RES_WR_IQPCIECH)

#define S_FW_RI_RES_WR_IQDCAEN		11
#define M_FW_RI_RES_WR_IQDCAEN		0x1
#define V_FW_RI_RES_WR_IQDCAEN(x)	((x) << S_FW_RI_RES_WR_IQDCAEN)
#define G_FW_RI_RES_WR_IQDCAEN(x)	\
    (((x) >> S_FW_RI_RES_WR_IQDCAEN) & M_FW_RI_RES_WR_IQDCAEN)
#define F_FW_RI_RES_WR_IQDCAEN	V_FW_RI_RES_WR_IQDCAEN(1U)

#define S_FW_RI_RES_WR_IQDCACPU		6
#define M_FW_RI_RES_WR_IQDCACPU		0x1f
#define V_FW_RI_RES_WR_IQDCACPU(x)	((x) << S_FW_RI_RES_WR_IQDCACPU)
#define G_FW_RI_RES_WR_IQDCACPU(x)	\
    (((x) >> S_FW_RI_RES_WR_IQDCACPU) & M_FW_RI_RES_WR_IQDCACPU)

#define S_FW_RI_RES_WR_IQINTCNTTHRESH		4
#define M_FW_RI_RES_WR_IQINTCNTTHRESH		0x3
#define V_FW_RI_RES_WR_IQINTCNTTHRESH(x)	\
    ((x) << S_FW_RI_RES_WR_IQINTCNTTHRESH)
#define G_FW_RI_RES_WR_IQINTCNTTHRESH(x)	\
    (((x) >> S_FW_RI_RES_WR_IQINTCNTTHRESH) & M_FW_RI_RES_WR_IQINTCNTTHRESH)

#define S_FW_RI_RES_WR_IQO	3
#define M_FW_RI_RES_WR_IQO	0x1
#define V_FW_RI_RES_WR_IQO(x)	((x) << S_FW_RI_RES_WR_IQO)
#define G_FW_RI_RES_WR_IQO(x)	\
    (((x) >> S_FW_RI_RES_WR_IQO) & M_FW_RI_RES_WR_IQO)
#define F_FW_RI_RES_WR_IQO	V_FW_RI_RES_WR_IQO(1U)

#define S_FW_RI_RES_WR_IQCPRIO		2
#define M_FW_RI_RES_WR_IQCPRIO		0x1
#define V_FW_RI_RES_WR_IQCPRIO(x)	((x) << S_FW_RI_RES_WR_IQCPRIO)
#define G_FW_RI_RES_WR_IQCPRIO(x)	\
    (((x) >> S_FW_RI_RES_WR_IQCPRIO) & M_FW_RI_RES_WR_IQCPRIO)
#define F_FW_RI_RES_WR_IQCPRIO	V_FW_RI_RES_WR_IQCPRIO(1U)

#define S_FW_RI_RES_WR_IQESIZE		0
#define M_FW_RI_RES_WR_IQESIZE		0x3
#define V_FW_RI_RES_WR_IQESIZE(x)	((x) << S_FW_RI_RES_WR_IQESIZE)
#define G_FW_RI_RES_WR_IQESIZE(x)	\
    (((x) >> S_FW_RI_RES_WR_IQESIZE) & M_FW_RI_RES_WR_IQESIZE)

#define S_FW_RI_RES_WR_IQNS	31
#define M_FW_RI_RES_WR_IQNS	0x1
#define V_FW_RI_RES_WR_IQNS(x)	((x) << S_FW_RI_RES_WR_IQNS)
#define G_FW_RI_RES_WR_IQNS(x)	\
    (((x) >> S_FW_RI_RES_WR_IQNS) & M_FW_RI_RES_WR_IQNS)
#define F_FW_RI_RES_WR_IQNS	V_FW_RI_RES_WR_IQNS(1U)

#define S_FW_RI_RES_WR_IQRO	30
#define M_FW_RI_RES_WR_IQRO	0x1
#define V_FW_RI_RES_WR_IQRO(x)	((x) << S_FW_RI_RES_WR_IQRO)
#define G_FW_RI_RES_WR_IQRO(x)	\
    (((x) >> S_FW_RI_RES_WR_IQRO) & M_FW_RI_RES_WR_IQRO)
#define F_FW_RI_RES_WR_IQRO	V_FW_RI_RES_WR_IQRO(1U)
=======
#define FW_RI_RES_WR_NRES_S	0
#define FW_RI_RES_WR_NRES_M	0xff
#define FW_RI_RES_WR_NRES_V(x)	((x) << FW_RI_RES_WR_NRES_S)
#define FW_RI_RES_WR_NRES_G(x)	\
	(((x) >> FW_RI_RES_WR_NRES_S) & FW_RI_RES_WR_NRES_M)

#define FW_RI_RES_WR_FETCHSZM_S		26
#define FW_RI_RES_WR_FETCHSZM_M		0x1
#define FW_RI_RES_WR_FETCHSZM_V(x)	((x) << FW_RI_RES_WR_FETCHSZM_S)
#define FW_RI_RES_WR_FETCHSZM_G(x)	\
	(((x) >> FW_RI_RES_WR_FETCHSZM_S) & FW_RI_RES_WR_FETCHSZM_M)
#define FW_RI_RES_WR_FETCHSZM_F	FW_RI_RES_WR_FETCHSZM_V(1U)

#define FW_RI_RES_WR_STATUSPGNS_S	25
#define FW_RI_RES_WR_STATUSPGNS_M	0x1
#define FW_RI_RES_WR_STATUSPGNS_V(x)	((x) << FW_RI_RES_WR_STATUSPGNS_S)
#define FW_RI_RES_WR_STATUSPGNS_G(x)	\
	(((x) >> FW_RI_RES_WR_STATUSPGNS_S) & FW_RI_RES_WR_STATUSPGNS_M)
#define FW_RI_RES_WR_STATUSPGNS_F	FW_RI_RES_WR_STATUSPGNS_V(1U)

#define FW_RI_RES_WR_STATUSPGRO_S	24
#define FW_RI_RES_WR_STATUSPGRO_M	0x1
#define FW_RI_RES_WR_STATUSPGRO_V(x)	((x) << FW_RI_RES_WR_STATUSPGRO_S)
#define FW_RI_RES_WR_STATUSPGRO_G(x)	\
	(((x) >> FW_RI_RES_WR_STATUSPGRO_S) & FW_RI_RES_WR_STATUSPGRO_M)
#define FW_RI_RES_WR_STATUSPGRO_F	FW_RI_RES_WR_STATUSPGRO_V(1U)

#define FW_RI_RES_WR_FETCHNS_S		23
#define FW_RI_RES_WR_FETCHNS_M		0x1
#define FW_RI_RES_WR_FETCHNS_V(x)	((x) << FW_RI_RES_WR_FETCHNS_S)
#define FW_RI_RES_WR_FETCHNS_G(x)	\
	(((x) >> FW_RI_RES_WR_FETCHNS_S) & FW_RI_RES_WR_FETCHNS_M)
#define FW_RI_RES_WR_FETCHNS_F	FW_RI_RES_WR_FETCHNS_V(1U)

#define FW_RI_RES_WR_FETCHRO_S		22
#define FW_RI_RES_WR_FETCHRO_M		0x1
#define FW_RI_RES_WR_FETCHRO_V(x)	((x) << FW_RI_RES_WR_FETCHRO_S)
#define FW_RI_RES_WR_FETCHRO_G(x)	\
	(((x) >> FW_RI_RES_WR_FETCHRO_S) & FW_RI_RES_WR_FETCHRO_M)
#define FW_RI_RES_WR_FETCHRO_F	FW_RI_RES_WR_FETCHRO_V(1U)

#define FW_RI_RES_WR_HOSTFCMODE_S	20
#define FW_RI_RES_WR_HOSTFCMODE_M	0x3
#define FW_RI_RES_WR_HOSTFCMODE_V(x)	((x) << FW_RI_RES_WR_HOSTFCMODE_S)
#define FW_RI_RES_WR_HOSTFCMODE_G(x)	\
	(((x) >> FW_RI_RES_WR_HOSTFCMODE_S) & FW_RI_RES_WR_HOSTFCMODE_M)

#define FW_RI_RES_WR_CPRIO_S	19
#define FW_RI_RES_WR_CPRIO_M	0x1
#define FW_RI_RES_WR_CPRIO_V(x)	((x) << FW_RI_RES_WR_CPRIO_S)
#define FW_RI_RES_WR_CPRIO_G(x)	\
	(((x) >> FW_RI_RES_WR_CPRIO_S) & FW_RI_RES_WR_CPRIO_M)
#define FW_RI_RES_WR_CPRIO_F	FW_RI_RES_WR_CPRIO_V(1U)

#define FW_RI_RES_WR_ONCHIP_S		18
#define FW_RI_RES_WR_ONCHIP_M		0x1
#define FW_RI_RES_WR_ONCHIP_V(x)	((x) << FW_RI_RES_WR_ONCHIP_S)
#define FW_RI_RES_WR_ONCHIP_G(x)	\
	(((x) >> FW_RI_RES_WR_ONCHIP_S) & FW_RI_RES_WR_ONCHIP_M)
#define FW_RI_RES_WR_ONCHIP_F	FW_RI_RES_WR_ONCHIP_V(1U)

#define FW_RI_RES_WR_PCIECHN_S		16
#define FW_RI_RES_WR_PCIECHN_M		0x3
#define FW_RI_RES_WR_PCIECHN_V(x)	((x) << FW_RI_RES_WR_PCIECHN_S)
#define FW_RI_RES_WR_PCIECHN_G(x)	\
	(((x) >> FW_RI_RES_WR_PCIECHN_S) & FW_RI_RES_WR_PCIECHN_M)

#define FW_RI_RES_WR_IQID_S	0
#define FW_RI_RES_WR_IQID_M	0xffff
#define FW_RI_RES_WR_IQID_V(x)	((x) << FW_RI_RES_WR_IQID_S)
#define FW_RI_RES_WR_IQID_G(x)	\
	(((x) >> FW_RI_RES_WR_IQID_S) & FW_RI_RES_WR_IQID_M)

#define FW_RI_RES_WR_DCAEN_S	31
#define FW_RI_RES_WR_DCAEN_M	0x1
#define FW_RI_RES_WR_DCAEN_V(x)	((x) << FW_RI_RES_WR_DCAEN_S)
#define FW_RI_RES_WR_DCAEN_G(x)	\
	(((x) >> FW_RI_RES_WR_DCAEN_S) & FW_RI_RES_WR_DCAEN_M)
#define FW_RI_RES_WR_DCAEN_F	FW_RI_RES_WR_DCAEN_V(1U)

#define FW_RI_RES_WR_DCACPU_S		26
#define FW_RI_RES_WR_DCACPU_M		0x1f
#define FW_RI_RES_WR_DCACPU_V(x)	((x) << FW_RI_RES_WR_DCACPU_S)
#define FW_RI_RES_WR_DCACPU_G(x)	\
	(((x) >> FW_RI_RES_WR_DCACPU_S) & FW_RI_RES_WR_DCACPU_M)

#define FW_RI_RES_WR_FBMIN_S	23
#define FW_RI_RES_WR_FBMIN_M	0x7
#define FW_RI_RES_WR_FBMIN_V(x)	((x) << FW_RI_RES_WR_FBMIN_S)
#define FW_RI_RES_WR_FBMIN_G(x)	\
	(((x) >> FW_RI_RES_WR_FBMIN_S) & FW_RI_RES_WR_FBMIN_M)

#define FW_RI_RES_WR_FBMAX_S	20
#define FW_RI_RES_WR_FBMAX_M	0x7
#define FW_RI_RES_WR_FBMAX_V(x)	((x) << FW_RI_RES_WR_FBMAX_S)
#define FW_RI_RES_WR_FBMAX_G(x)	\
	(((x) >> FW_RI_RES_WR_FBMAX_S) & FW_RI_RES_WR_FBMAX_M)

#define FW_RI_RES_WR_CIDXFTHRESHO_S	19
#define FW_RI_RES_WR_CIDXFTHRESHO_M	0x1
#define FW_RI_RES_WR_CIDXFTHRESHO_V(x)	((x) << FW_RI_RES_WR_CIDXFTHRESHO_S)
#define FW_RI_RES_WR_CIDXFTHRESHO_G(x)	\
	(((x) >> FW_RI_RES_WR_CIDXFTHRESHO_S) & FW_RI_RES_WR_CIDXFTHRESHO_M)
#define FW_RI_RES_WR_CIDXFTHRESHO_F	FW_RI_RES_WR_CIDXFTHRESHO_V(1U)

#define FW_RI_RES_WR_CIDXFTHRESH_S	16
#define FW_RI_RES_WR_CIDXFTHRESH_M	0x7
#define FW_RI_RES_WR_CIDXFTHRESH_V(x)	((x) << FW_RI_RES_WR_CIDXFTHRESH_S)
#define FW_RI_RES_WR_CIDXFTHRESH_G(x)	\
	(((x) >> FW_RI_RES_WR_CIDXFTHRESH_S) & FW_RI_RES_WR_CIDXFTHRESH_M)

#define FW_RI_RES_WR_EQSIZE_S		0
#define FW_RI_RES_WR_EQSIZE_M		0xffff
#define FW_RI_RES_WR_EQSIZE_V(x)	((x) << FW_RI_RES_WR_EQSIZE_S)
#define FW_RI_RES_WR_EQSIZE_G(x)	\
	(((x) >> FW_RI_RES_WR_EQSIZE_S) & FW_RI_RES_WR_EQSIZE_M)

#define FW_RI_RES_WR_IQANDST_S		15
#define FW_RI_RES_WR_IQANDST_M		0x1
#define FW_RI_RES_WR_IQANDST_V(x)	((x) << FW_RI_RES_WR_IQANDST_S)
#define FW_RI_RES_WR_IQANDST_G(x)	\
	(((x) >> FW_RI_RES_WR_IQANDST_S) & FW_RI_RES_WR_IQANDST_M)
#define FW_RI_RES_WR_IQANDST_F	FW_RI_RES_WR_IQANDST_V(1U)

#define FW_RI_RES_WR_IQANUS_S		14
#define FW_RI_RES_WR_IQANUS_M		0x1
#define FW_RI_RES_WR_IQANUS_V(x)	((x) << FW_RI_RES_WR_IQANUS_S)
#define FW_RI_RES_WR_IQANUS_G(x)	\
	(((x) >> FW_RI_RES_WR_IQANUS_S) & FW_RI_RES_WR_IQANUS_M)
#define FW_RI_RES_WR_IQANUS_F	FW_RI_RES_WR_IQANUS_V(1U)

#define FW_RI_RES_WR_IQANUD_S		12
#define FW_RI_RES_WR_IQANUD_M		0x3
#define FW_RI_RES_WR_IQANUD_V(x)	((x) << FW_RI_RES_WR_IQANUD_S)
#define FW_RI_RES_WR_IQANUD_G(x)	\
	(((x) >> FW_RI_RES_WR_IQANUD_S) & FW_RI_RES_WR_IQANUD_M)

#define FW_RI_RES_WR_IQANDSTINDEX_S	0
#define FW_RI_RES_WR_IQANDSTINDEX_M	0xfff
#define FW_RI_RES_WR_IQANDSTINDEX_V(x)	((x) << FW_RI_RES_WR_IQANDSTINDEX_S)
#define FW_RI_RES_WR_IQANDSTINDEX_G(x)	\
	(((x) >> FW_RI_RES_WR_IQANDSTINDEX_S) & FW_RI_RES_WR_IQANDSTINDEX_M)

#define FW_RI_RES_WR_IQDROPRSS_S	15
#define FW_RI_RES_WR_IQDROPRSS_M	0x1
#define FW_RI_RES_WR_IQDROPRSS_V(x)	((x) << FW_RI_RES_WR_IQDROPRSS_S)
#define FW_RI_RES_WR_IQDROPRSS_G(x)	\
	(((x) >> FW_RI_RES_WR_IQDROPRSS_S) & FW_RI_RES_WR_IQDROPRSS_M)
#define FW_RI_RES_WR_IQDROPRSS_F	FW_RI_RES_WR_IQDROPRSS_V(1U)

#define FW_RI_RES_WR_IQGTSMODE_S	14
#define FW_RI_RES_WR_IQGTSMODE_M	0x1
#define FW_RI_RES_WR_IQGTSMODE_V(x)	((x) << FW_RI_RES_WR_IQGTSMODE_S)
#define FW_RI_RES_WR_IQGTSMODE_G(x)	\
	(((x) >> FW_RI_RES_WR_IQGTSMODE_S) & FW_RI_RES_WR_IQGTSMODE_M)
#define FW_RI_RES_WR_IQGTSMODE_F	FW_RI_RES_WR_IQGTSMODE_V(1U)

#define FW_RI_RES_WR_IQPCIECH_S		12
#define FW_RI_RES_WR_IQPCIECH_M		0x3
#define FW_RI_RES_WR_IQPCIECH_V(x)	((x) << FW_RI_RES_WR_IQPCIECH_S)
#define FW_RI_RES_WR_IQPCIECH_G(x)	\
	(((x) >> FW_RI_RES_WR_IQPCIECH_S) & FW_RI_RES_WR_IQPCIECH_M)

#define FW_RI_RES_WR_IQDCAEN_S		11
#define FW_RI_RES_WR_IQDCAEN_M		0x1
#define FW_RI_RES_WR_IQDCAEN_V(x)	((x) << FW_RI_RES_WR_IQDCAEN_S)
#define FW_RI_RES_WR_IQDCAEN_G(x)	\
	(((x) >> FW_RI_RES_WR_IQDCAEN_S) & FW_RI_RES_WR_IQDCAEN_M)
#define FW_RI_RES_WR_IQDCAEN_F	FW_RI_RES_WR_IQDCAEN_V(1U)

#define FW_RI_RES_WR_IQDCACPU_S		6
#define FW_RI_RES_WR_IQDCACPU_M		0x1f
#define FW_RI_RES_WR_IQDCACPU_V(x)	((x) << FW_RI_RES_WR_IQDCACPU_S)
#define FW_RI_RES_WR_IQDCACPU_G(x)	\
	(((x) >> FW_RI_RES_WR_IQDCACPU_S) & FW_RI_RES_WR_IQDCACPU_M)

#define FW_RI_RES_WR_IQINTCNTTHRESH_S		4
#define FW_RI_RES_WR_IQINTCNTTHRESH_M		0x3
#define FW_RI_RES_WR_IQINTCNTTHRESH_V(x)	\
	((x) << FW_RI_RES_WR_IQINTCNTTHRESH_S)
#define FW_RI_RES_WR_IQINTCNTTHRESH_G(x)	\
	(((x) >> FW_RI_RES_WR_IQINTCNTTHRESH_S) & FW_RI_RES_WR_IQINTCNTTHRESH_M)

#define FW_RI_RES_WR_IQO_S	3
#define FW_RI_RES_WR_IQO_M	0x1
#define FW_RI_RES_WR_IQO_V(x)	((x) << FW_RI_RES_WR_IQO_S)
#define FW_RI_RES_WR_IQO_G(x)	\
	(((x) >> FW_RI_RES_WR_IQO_S) & FW_RI_RES_WR_IQO_M)
#define FW_RI_RES_WR_IQO_F	FW_RI_RES_WR_IQO_V(1U)

#define FW_RI_RES_WR_IQCPRIO_S		2
#define FW_RI_RES_WR_IQCPRIO_M		0x1
#define FW_RI_RES_WR_IQCPRIO_V(x)	((x) << FW_RI_RES_WR_IQCPRIO_S)
#define FW_RI_RES_WR_IQCPRIO_G(x)	\
	(((x) >> FW_RI_RES_WR_IQCPRIO_S) & FW_RI_RES_WR_IQCPRIO_M)
#define FW_RI_RES_WR_IQCPRIO_F	FW_RI_RES_WR_IQCPRIO_V(1U)

#define FW_RI_RES_WR_IQESIZE_S		0
#define FW_RI_RES_WR_IQESIZE_M		0x3
#define FW_RI_RES_WR_IQESIZE_V(x)	((x) << FW_RI_RES_WR_IQESIZE_S)
#define FW_RI_RES_WR_IQESIZE_G(x)	\
	(((x) >> FW_RI_RES_WR_IQESIZE_S) & FW_RI_RES_WR_IQESIZE_M)

#define FW_RI_RES_WR_IQNS_S	31
#define FW_RI_RES_WR_IQNS_M	0x1
#define FW_RI_RES_WR_IQNS_V(x)	((x) << FW_RI_RES_WR_IQNS_S)
#define FW_RI_RES_WR_IQNS_G(x)	\
	(((x) >> FW_RI_RES_WR_IQNS_S) & FW_RI_RES_WR_IQNS_M)
#define FW_RI_RES_WR_IQNS_F	FW_RI_RES_WR_IQNS_V(1U)

#define FW_RI_RES_WR_IQRO_S	30
#define FW_RI_RES_WR_IQRO_M	0x1
#define FW_RI_RES_WR_IQRO_V(x)	((x) << FW_RI_RES_WR_IQRO_S)
#define FW_RI_RES_WR_IQRO_G(x)	\
	(((x) >> FW_RI_RES_WR_IQRO_S) & FW_RI_RES_WR_IQRO_M)
#define FW_RI_RES_WR_IQRO_F	FW_RI_RES_WR_IQRO_V(1U)
>>>>>>> v4.9.227

struct fw_ri_rdma_write_wr {
	__u8   opcode;
	__u8   flags;
	__u16  wrid;
	__u8   r1[3];
	__u8   len16;
	__be64 r2;
	__be32 plen;
	__be32 stag_sink;
	__be64 to_sink;
#ifndef C99_NOT_SUPPORTED
	union {
		struct fw_ri_immd immd_src[0];
		struct fw_ri_isgl isgl_src[0];
	} u;
#endif
};

struct fw_ri_send_wr {
	__u8   opcode;
	__u8   flags;
	__u16  wrid;
	__u8   r1[3];
	__u8   len16;
	__be32 sendop_pkd;
	__be32 stag_inv;
	__be32 plen;
	__be32 r3;
	__be64 r4;
#ifndef C99_NOT_SUPPORTED
	union {
		struct fw_ri_immd immd_src[0];
		struct fw_ri_isgl isgl_src[0];
	} u;
#endif
};

<<<<<<< HEAD
#define S_FW_RI_SEND_WR_SENDOP		0
#define M_FW_RI_SEND_WR_SENDOP		0xf
#define V_FW_RI_SEND_WR_SENDOP(x)	((x) << S_FW_RI_SEND_WR_SENDOP)
#define G_FW_RI_SEND_WR_SENDOP(x)	\
    (((x) >> S_FW_RI_SEND_WR_SENDOP) & M_FW_RI_SEND_WR_SENDOP)
=======
#define FW_RI_SEND_WR_SENDOP_S		0
#define FW_RI_SEND_WR_SENDOP_M		0xf
#define FW_RI_SEND_WR_SENDOP_V(x)	((x) << FW_RI_SEND_WR_SENDOP_S)
#define FW_RI_SEND_WR_SENDOP_G(x)	\
	(((x) >> FW_RI_SEND_WR_SENDOP_S) & FW_RI_SEND_WR_SENDOP_M)
>>>>>>> v4.9.227

struct fw_ri_rdma_read_wr {
	__u8   opcode;
	__u8   flags;
	__u16  wrid;
	__u8   r1[3];
	__u8   len16;
	__be64 r2;
	__be32 stag_sink;
	__be32 to_sink_hi;
	__be32 to_sink_lo;
	__be32 plen;
	__be32 stag_src;
	__be32 to_src_hi;
	__be32 to_src_lo;
	__be32 r5;
};

struct fw_ri_recv_wr {
	__u8   opcode;
	__u8   r1;
	__u16  wrid;
	__u8   r2[3];
	__u8   len16;
	struct fw_ri_isgl isgl;
};

struct fw_ri_bind_mw_wr {
	__u8   opcode;
	__u8   flags;
	__u16  wrid;
	__u8   r1[3];
	__u8   len16;
	__u8   qpbinde_to_dcacpu;
	__u8   pgsz_shift;
	__u8   addr_type;
	__u8   mem_perms;
	__be32 stag_mr;
	__be32 stag_mw;
	__be32 r3;
	__be64 len_mw;
	__be64 va_fbo;
	__be64 r4;
};

<<<<<<< HEAD
#define S_FW_RI_BIND_MW_WR_QPBINDE	6
#define M_FW_RI_BIND_MW_WR_QPBINDE	0x1
#define V_FW_RI_BIND_MW_WR_QPBINDE(x)	((x) << S_FW_RI_BIND_MW_WR_QPBINDE)
#define G_FW_RI_BIND_MW_WR_QPBINDE(x)	\
    (((x) >> S_FW_RI_BIND_MW_WR_QPBINDE) & M_FW_RI_BIND_MW_WR_QPBINDE)
#define F_FW_RI_BIND_MW_WR_QPBINDE	V_FW_RI_BIND_MW_WR_QPBINDE(1U)

#define S_FW_RI_BIND_MW_WR_NS		5
#define M_FW_RI_BIND_MW_WR_NS		0x1
#define V_FW_RI_BIND_MW_WR_NS(x)	((x) << S_FW_RI_BIND_MW_WR_NS)
#define G_FW_RI_BIND_MW_WR_NS(x)	\
    (((x) >> S_FW_RI_BIND_MW_WR_NS) & M_FW_RI_BIND_MW_WR_NS)
#define F_FW_RI_BIND_MW_WR_NS	V_FW_RI_BIND_MW_WR_NS(1U)

#define S_FW_RI_BIND_MW_WR_DCACPU	0
#define M_FW_RI_BIND_MW_WR_DCACPU	0x1f
#define V_FW_RI_BIND_MW_WR_DCACPU(x)	((x) << S_FW_RI_BIND_MW_WR_DCACPU)
#define G_FW_RI_BIND_MW_WR_DCACPU(x)	\
    (((x) >> S_FW_RI_BIND_MW_WR_DCACPU) & M_FW_RI_BIND_MW_WR_DCACPU)
=======
#define FW_RI_BIND_MW_WR_QPBINDE_S	6
#define FW_RI_BIND_MW_WR_QPBINDE_M	0x1
#define FW_RI_BIND_MW_WR_QPBINDE_V(x)	((x) << FW_RI_BIND_MW_WR_QPBINDE_S)
#define FW_RI_BIND_MW_WR_QPBINDE_G(x)	\
	(((x) >> FW_RI_BIND_MW_WR_QPBINDE_S) & FW_RI_BIND_MW_WR_QPBINDE_M)
#define FW_RI_BIND_MW_WR_QPBINDE_F	FW_RI_BIND_MW_WR_QPBINDE_V(1U)

#define FW_RI_BIND_MW_WR_NS_S		5
#define FW_RI_BIND_MW_WR_NS_M		0x1
#define FW_RI_BIND_MW_WR_NS_V(x)	((x) << FW_RI_BIND_MW_WR_NS_S)
#define FW_RI_BIND_MW_WR_NS_G(x)	\
	(((x) >> FW_RI_BIND_MW_WR_NS_S) & FW_RI_BIND_MW_WR_NS_M)
#define FW_RI_BIND_MW_WR_NS_F	FW_RI_BIND_MW_WR_NS_V(1U)

#define FW_RI_BIND_MW_WR_DCACPU_S	0
#define FW_RI_BIND_MW_WR_DCACPU_M	0x1f
#define FW_RI_BIND_MW_WR_DCACPU_V(x)	((x) << FW_RI_BIND_MW_WR_DCACPU_S)
#define FW_RI_BIND_MW_WR_DCACPU_G(x)	\
	(((x) >> FW_RI_BIND_MW_WR_DCACPU_S) & FW_RI_BIND_MW_WR_DCACPU_M)
>>>>>>> v4.9.227

struct fw_ri_fr_nsmr_wr {
	__u8   opcode;
	__u8   flags;
	__u16  wrid;
	__u8   r1[3];
	__u8   len16;
	__u8   qpbinde_to_dcacpu;
	__u8   pgsz_shift;
	__u8   addr_type;
	__u8   mem_perms;
	__be32 stag;
	__be32 len_hi;
	__be32 len_lo;
	__be32 va_hi;
	__be32 va_lo_fbo;
};

<<<<<<< HEAD
#define S_FW_RI_FR_NSMR_WR_QPBINDE	6
#define M_FW_RI_FR_NSMR_WR_QPBINDE	0x1
#define V_FW_RI_FR_NSMR_WR_QPBINDE(x)	((x) << S_FW_RI_FR_NSMR_WR_QPBINDE)
#define G_FW_RI_FR_NSMR_WR_QPBINDE(x)	\
    (((x) >> S_FW_RI_FR_NSMR_WR_QPBINDE) & M_FW_RI_FR_NSMR_WR_QPBINDE)
#define F_FW_RI_FR_NSMR_WR_QPBINDE	V_FW_RI_FR_NSMR_WR_QPBINDE(1U)

#define S_FW_RI_FR_NSMR_WR_NS		5
#define M_FW_RI_FR_NSMR_WR_NS		0x1
#define V_FW_RI_FR_NSMR_WR_NS(x)	((x) << S_FW_RI_FR_NSMR_WR_NS)
#define G_FW_RI_FR_NSMR_WR_NS(x)	\
    (((x) >> S_FW_RI_FR_NSMR_WR_NS) & M_FW_RI_FR_NSMR_WR_NS)
#define F_FW_RI_FR_NSMR_WR_NS	V_FW_RI_FR_NSMR_WR_NS(1U)

#define S_FW_RI_FR_NSMR_WR_DCACPU	0
#define M_FW_RI_FR_NSMR_WR_DCACPU	0x1f
#define V_FW_RI_FR_NSMR_WR_DCACPU(x)	((x) << S_FW_RI_FR_NSMR_WR_DCACPU)
#define G_FW_RI_FR_NSMR_WR_DCACPU(x)	\
    (((x) >> S_FW_RI_FR_NSMR_WR_DCACPU) & M_FW_RI_FR_NSMR_WR_DCACPU)
=======
#define FW_RI_FR_NSMR_WR_QPBINDE_S	6
#define FW_RI_FR_NSMR_WR_QPBINDE_M	0x1
#define FW_RI_FR_NSMR_WR_QPBINDE_V(x)	((x) << FW_RI_FR_NSMR_WR_QPBINDE_S)
#define FW_RI_FR_NSMR_WR_QPBINDE_G(x)	\
	(((x) >> FW_RI_FR_NSMR_WR_QPBINDE_S) & FW_RI_FR_NSMR_WR_QPBINDE_M)
#define FW_RI_FR_NSMR_WR_QPBINDE_F	FW_RI_FR_NSMR_WR_QPBINDE_V(1U)

#define FW_RI_FR_NSMR_WR_NS_S		5
#define FW_RI_FR_NSMR_WR_NS_M		0x1
#define FW_RI_FR_NSMR_WR_NS_V(x)	((x) << FW_RI_FR_NSMR_WR_NS_S)
#define FW_RI_FR_NSMR_WR_NS_G(x)	\
	(((x) >> FW_RI_FR_NSMR_WR_NS_S) & FW_RI_FR_NSMR_WR_NS_M)
#define FW_RI_FR_NSMR_WR_NS_F	FW_RI_FR_NSMR_WR_NS_V(1U)

#define FW_RI_FR_NSMR_WR_DCACPU_S	0
#define FW_RI_FR_NSMR_WR_DCACPU_M	0x1f
#define FW_RI_FR_NSMR_WR_DCACPU_V(x)	((x) << FW_RI_FR_NSMR_WR_DCACPU_S)
#define FW_RI_FR_NSMR_WR_DCACPU_G(x)	\
	(((x) >> FW_RI_FR_NSMR_WR_DCACPU_S) & FW_RI_FR_NSMR_WR_DCACPU_M)

struct fw_ri_fr_nsmr_tpte_wr {
	__u8	opcode;
	__u8   flags;
	__u16  wrid;
	__u8   r1[3];
	__u8   len16;
	__be32  r2;
	__be32  stag;
	struct fw_ri_tpte tpte;
	__u64  pbl[2];
};
>>>>>>> v4.9.227

struct fw_ri_inv_lstag_wr {
	__u8   opcode;
	__u8   flags;
	__u16  wrid;
	__u8   r1[3];
	__u8   len16;
	__be32 r2;
	__be32 stag_inv;
};

enum fw_ri_type {
	FW_RI_TYPE_INIT,
	FW_RI_TYPE_FINI,
	FW_RI_TYPE_TERMINATE
};

enum fw_ri_init_p2ptype {
	FW_RI_INIT_P2PTYPE_RDMA_WRITE		= FW_RI_RDMA_WRITE,
	FW_RI_INIT_P2PTYPE_READ_REQ		= FW_RI_READ_REQ,
	FW_RI_INIT_P2PTYPE_SEND			= FW_RI_SEND,
	FW_RI_INIT_P2PTYPE_SEND_WITH_INV	= FW_RI_SEND_WITH_INV,
	FW_RI_INIT_P2PTYPE_SEND_WITH_SE		= FW_RI_SEND_WITH_SE,
	FW_RI_INIT_P2PTYPE_SEND_WITH_SE_INV	= FW_RI_SEND_WITH_SE_INV,
	FW_RI_INIT_P2PTYPE_DISABLED		= 0xf,
};

struct fw_ri_wr {
	__be32 op_compl;
	__be32 flowid_len16;
	__u64  cookie;
	union fw_ri {
		struct fw_ri_init {
			__u8   type;
			__u8   mpareqbit_p2ptype;
			__u8   r4[2];
			__u8   mpa_attrs;
			__u8   qp_caps;
			__be16 nrqe;
			__be32 pdid;
			__be32 qpid;
			__be32 sq_eqid;
			__be32 rq_eqid;
			__be32 scqid;
			__be32 rcqid;
			__be32 ord_max;
			__be32 ird_max;
			__be32 iss;
			__be32 irs;
			__be32 hwrqsize;
			__be32 hwrqaddr;
			__be64 r5;
			union fw_ri_init_p2p {
				struct fw_ri_rdma_write_wr write;
				struct fw_ri_rdma_read_wr read;
				struct fw_ri_send_wr send;
			} u;
		} init;
		struct fw_ri_fini {
			__u8   type;
			__u8   r3[7];
			__be64 r4;
		} fini;
		struct fw_ri_terminate {
			__u8   type;
			__u8   r3[3];
			__be32 immdlen;
			__u8   termmsg[40];
		} terminate;
	} u;
};

<<<<<<< HEAD
#define S_FW_RI_WR_MPAREQBIT	7
#define M_FW_RI_WR_MPAREQBIT	0x1
#define V_FW_RI_WR_MPAREQBIT(x)	((x) << S_FW_RI_WR_MPAREQBIT)
#define G_FW_RI_WR_MPAREQBIT(x)	\
    (((x) >> S_FW_RI_WR_MPAREQBIT) & M_FW_RI_WR_MPAREQBIT)
#define F_FW_RI_WR_MPAREQBIT	V_FW_RI_WR_MPAREQBIT(1U)

#define S_FW_RI_WR_P2PTYPE	0
#define M_FW_RI_WR_P2PTYPE	0xf
#define V_FW_RI_WR_P2PTYPE(x)	((x) << S_FW_RI_WR_P2PTYPE)
#define G_FW_RI_WR_P2PTYPE(x)	\
    (((x) >> S_FW_RI_WR_P2PTYPE) & M_FW_RI_WR_P2PTYPE)

struct tcp_options {
	__be16 mss;
	__u8 wsf;
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u8:4;
	__u8 unknown:1;
	__u8:1;
	__u8 sack:1;
	__u8 tstamp:1;
#else
	__u8 tstamp:1;
	__u8 sack:1;
	__u8:1;
	__u8 unknown:1;
	__u8:4;
#endif
};

struct cpl_pass_accept_req {
	union opcode_tid ot;
	__be16 rsvd;
	__be16 len;
	__be32 hdr_len;
	__be16 vlan;
	__be16 l2info;
	__be32 tos_stid;
	struct tcp_options tcpopt;
};

/* cpl_pass_accept_req.hdr_len fields */
#define S_SYN_RX_CHAN    0
#define M_SYN_RX_CHAN    0xF
#define V_SYN_RX_CHAN(x) ((x) << S_SYN_RX_CHAN)
#define G_SYN_RX_CHAN(x) (((x) >> S_SYN_RX_CHAN) & M_SYN_RX_CHAN)

#define S_TCP_HDR_LEN    10
#define M_TCP_HDR_LEN    0x3F
#define V_TCP_HDR_LEN(x) ((x) << S_TCP_HDR_LEN)
#define G_TCP_HDR_LEN(x) (((x) >> S_TCP_HDR_LEN) & M_TCP_HDR_LEN)

#define S_IP_HDR_LEN    16
#define M_IP_HDR_LEN    0x3FF
#define V_IP_HDR_LEN(x) ((x) << S_IP_HDR_LEN)
#define G_IP_HDR_LEN(x) (((x) >> S_IP_HDR_LEN) & M_IP_HDR_LEN)

#define S_ETH_HDR_LEN    26
#define M_ETH_HDR_LEN    0x1F
#define V_ETH_HDR_LEN(x) ((x) << S_ETH_HDR_LEN)
#define G_ETH_HDR_LEN(x) (((x) >> S_ETH_HDR_LEN) & M_ETH_HDR_LEN)

/* cpl_pass_accept_req.l2info fields */
#define S_SYN_MAC_IDX    0
#define M_SYN_MAC_IDX    0x1FF
#define V_SYN_MAC_IDX(x) ((x) << S_SYN_MAC_IDX)
#define G_SYN_MAC_IDX(x) (((x) >> S_SYN_MAC_IDX) & M_SYN_MAC_IDX)

#define S_SYN_XACT_MATCH    9
#define V_SYN_XACT_MATCH(x) ((x) << S_SYN_XACT_MATCH)
#define F_SYN_XACT_MATCH    V_SYN_XACT_MATCH(1U)

#define S_SYN_INTF    12
#define M_SYN_INTF    0xF
#define V_SYN_INTF(x) ((x) << S_SYN_INTF)
#define G_SYN_INTF(x) (((x) >> S_SYN_INTF) & M_SYN_INTF)

struct ulptx_idata {
	__be32 cmd_more;
	__be32 len;
};

#define S_ULPTX_NSGE    0
#define M_ULPTX_NSGE    0xFFFF
#define V_ULPTX_NSGE(x) ((x) << S_ULPTX_NSGE)

#define S_RX_DACK_MODE    29
#define M_RX_DACK_MODE    0x3
#define V_RX_DACK_MODE(x) ((x) << S_RX_DACK_MODE)
#define G_RX_DACK_MODE(x) (((x) >> S_RX_DACK_MODE) & M_RX_DACK_MODE)

#define S_RX_DACK_CHANGE    31
#define V_RX_DACK_CHANGE(x) ((x) << S_RX_DACK_CHANGE)
#define F_RX_DACK_CHANGE    V_RX_DACK_CHANGE(1U)

enum {                     /* TCP congestion control algorithms */
	CONG_ALG_RENO,
	CONG_ALG_TAHOE,
	CONG_ALG_NEWRENO,
	CONG_ALG_HIGHSPEED
};

#define S_CONG_CNTRL    14
#define M_CONG_CNTRL    0x3
#define V_CONG_CNTRL(x) ((x) << S_CONG_CNTRL)
#define G_CONG_CNTRL(x) (((x) >> S_CONG_CNTRL) & M_CONG_CNTRL)

#define CONG_CNTRL_VALID   (1 << 18)
=======
#define FW_RI_WR_MPAREQBIT_S	7
#define FW_RI_WR_MPAREQBIT_M	0x1
#define FW_RI_WR_MPAREQBIT_V(x)	((x) << FW_RI_WR_MPAREQBIT_S)
#define FW_RI_WR_MPAREQBIT_G(x)	\
	(((x) >> FW_RI_WR_MPAREQBIT_S) & FW_RI_WR_MPAREQBIT_M)
#define FW_RI_WR_MPAREQBIT_F	FW_RI_WR_MPAREQBIT_V(1U)

#define FW_RI_WR_P2PTYPE_S	0
#define FW_RI_WR_P2PTYPE_M	0xf
#define FW_RI_WR_P2PTYPE_V(x)	((x) << FW_RI_WR_P2PTYPE_S)
#define FW_RI_WR_P2PTYPE_G(x)	\
	(((x) >> FW_RI_WR_P2PTYPE_S) & FW_RI_WR_P2PTYPE_M)
>>>>>>> v4.9.227

#endif /* _T4FW_RI_API_H_ */
