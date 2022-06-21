/*
<<<<<<< HEAD
 * Copyright (c) 2013, Mellanox Technologies inc.  All rights reserved.
=======
 * Copyright (c) 2013-2015, Mellanox Technologies. All rights reserved.
>>>>>>> v4.9.227
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
 *
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

#include <linux/module.h>
#include <rdma/ib_umem.h>
<<<<<<< HEAD
#include "mlx5_ib.h"
#include "user.h"
=======
#include <rdma/ib_cache.h>
#include <rdma/ib_user_verbs.h>
#include "mlx5_ib.h"
>>>>>>> v4.9.227

/* not supported currently */
static int wq_signature;

enum {
	MLX5_IB_ACK_REQ_FREQ	= 8,
};

enum {
	MLX5_IB_DEFAULT_SCHED_QUEUE	= 0x83,
	MLX5_IB_DEFAULT_QP0_SCHED_QUEUE	= 0x3f,
	MLX5_IB_LINK_TYPE_IB		= 0,
	MLX5_IB_LINK_TYPE_ETH		= 1
};

enum {
	MLX5_IB_SQ_STRIDE	= 6,
<<<<<<< HEAD
	MLX5_IB_CACHE_LINE_SIZE	= 64,
=======
>>>>>>> v4.9.227
};

static const u32 mlx5_ib_opcode[] = {
	[IB_WR_SEND]				= MLX5_OPCODE_SEND,
<<<<<<< HEAD
=======
	[IB_WR_LSO]				= MLX5_OPCODE_LSO,
>>>>>>> v4.9.227
	[IB_WR_SEND_WITH_IMM]			= MLX5_OPCODE_SEND_IMM,
	[IB_WR_RDMA_WRITE]			= MLX5_OPCODE_RDMA_WRITE,
	[IB_WR_RDMA_WRITE_WITH_IMM]		= MLX5_OPCODE_RDMA_WRITE_IMM,
	[IB_WR_RDMA_READ]			= MLX5_OPCODE_RDMA_READ,
	[IB_WR_ATOMIC_CMP_AND_SWP]		= MLX5_OPCODE_ATOMIC_CS,
	[IB_WR_ATOMIC_FETCH_AND_ADD]		= MLX5_OPCODE_ATOMIC_FA,
	[IB_WR_SEND_WITH_INV]			= MLX5_OPCODE_SEND_INVAL,
	[IB_WR_LOCAL_INV]			= MLX5_OPCODE_UMR,
<<<<<<< HEAD
	[IB_WR_FAST_REG_MR]			= MLX5_OPCODE_UMR,
=======
	[IB_WR_REG_MR]				= MLX5_OPCODE_UMR,
>>>>>>> v4.9.227
	[IB_WR_MASKED_ATOMIC_CMP_AND_SWP]	= MLX5_OPCODE_ATOMIC_MASKED_CS,
	[IB_WR_MASKED_ATOMIC_FETCH_AND_ADD]	= MLX5_OPCODE_ATOMIC_MASKED_FA,
	[MLX5_IB_WR_UMR]			= MLX5_OPCODE_UMR,
};

<<<<<<< HEAD
struct umr_wr {
	u64				virt_addr;
	struct ib_pd		       *pd;
	unsigned int			page_shift;
	unsigned int			npages;
	u32				length;
	int				access_flags;
	u32				mkey;
};

=======
struct mlx5_wqe_eth_pad {
	u8 rsvd0[16];
};

enum raw_qp_set_mask_map {
	MLX5_RAW_QP_MOD_SET_RQ_Q_CTR_ID		= 1UL << 0,
};

struct mlx5_modify_raw_qp_param {
	u16 operation;

	u32 set_mask; /* raw_qp_set_mask_map */
	u8 rq_q_ctr_id;
};

static void get_cqs(enum ib_qp_type qp_type,
		    struct ib_cq *ib_send_cq, struct ib_cq *ib_recv_cq,
		    struct mlx5_ib_cq **send_cq, struct mlx5_ib_cq **recv_cq);

>>>>>>> v4.9.227
static int is_qp0(enum ib_qp_type qp_type)
{
	return qp_type == IB_QPT_SMI;
}

<<<<<<< HEAD
static int is_qp1(enum ib_qp_type qp_type)
{
	return qp_type == IB_QPT_GSI;
}

=======
>>>>>>> v4.9.227
static int is_sqp(enum ib_qp_type qp_type)
{
	return is_qp0(qp_type) || is_qp1(qp_type);
}

static void *get_wqe(struct mlx5_ib_qp *qp, int offset)
{
	return mlx5_buf_offset(&qp->buf, offset);
}

static void *get_recv_wqe(struct mlx5_ib_qp *qp, int n)
{
	return get_wqe(qp, qp->rq.offset + (n << qp->rq.wqe_shift));
}

void *mlx5_get_send_wqe(struct mlx5_ib_qp *qp, int n)
{
	return get_wqe(qp, qp->sq.offset + (n << MLX5_IB_SQ_STRIDE));
}

<<<<<<< HEAD
=======
/**
 * mlx5_ib_read_user_wqe() - Copy a user-space WQE to kernel space.
 *
 * @qp: QP to copy from.
 * @send: copy from the send queue when non-zero, use the receive queue
 *	  otherwise.
 * @wqe_index:  index to start copying from. For send work queues, the
 *		wqe_index is in units of MLX5_SEND_WQE_BB.
 *		For receive work queue, it is the number of work queue
 *		element in the queue.
 * @buffer: destination buffer.
 * @length: maximum number of bytes to copy.
 *
 * Copies at least a single WQE, but may copy more data.
 *
 * Return: the number of bytes copied, or an error code.
 */
int mlx5_ib_read_user_wqe(struct mlx5_ib_qp *qp, int send, int wqe_index,
			  void *buffer, u32 length,
			  struct mlx5_ib_qp_base *base)
{
	struct ib_device *ibdev = qp->ibqp.device;
	struct mlx5_ib_dev *dev = to_mdev(ibdev);
	struct mlx5_ib_wq *wq = send ? &qp->sq : &qp->rq;
	size_t offset;
	size_t wq_end;
	struct ib_umem *umem = base->ubuffer.umem;
	u32 first_copy_length;
	int wqe_length;
	int ret;

	if (wq->wqe_cnt == 0) {
		mlx5_ib_dbg(dev, "mlx5_ib_read_user_wqe for a QP with wqe_cnt == 0. qp_type: 0x%x\n",
			    qp->ibqp.qp_type);
		return -EINVAL;
	}

	offset = wq->offset + ((wqe_index % wq->wqe_cnt) << wq->wqe_shift);
	wq_end = wq->offset + (wq->wqe_cnt << wq->wqe_shift);

	if (send && length < sizeof(struct mlx5_wqe_ctrl_seg))
		return -EINVAL;

	if (offset > umem->length ||
	    (send && offset + sizeof(struct mlx5_wqe_ctrl_seg) > umem->length))
		return -EINVAL;

	first_copy_length = min_t(u32, offset + length, wq_end) - offset;
	ret = ib_umem_copy_from(buffer, umem, offset, first_copy_length);
	if (ret)
		return ret;

	if (send) {
		struct mlx5_wqe_ctrl_seg *ctrl = buffer;
		int ds = be32_to_cpu(ctrl->qpn_ds) & MLX5_WQE_CTRL_DS_MASK;

		wqe_length = ds * MLX5_WQE_DS_UNITS;
	} else {
		wqe_length = 1 << wq->wqe_shift;
	}

	if (wqe_length <= first_copy_length)
		return first_copy_length;

	ret = ib_umem_copy_from(buffer + first_copy_length, umem, wq->offset,
				wqe_length - first_copy_length);
	if (ret)
		return ret;

	return wqe_length;
}

>>>>>>> v4.9.227
static void mlx5_ib_qp_event(struct mlx5_core_qp *qp, int type)
{
	struct ib_qp *ibqp = &to_mibqp(qp)->ibqp;
	struct ib_event event;

<<<<<<< HEAD
	if (type == MLX5_EVENT_TYPE_PATH_MIG)
		to_mibqp(qp)->port = to_mibqp(qp)->alt_port;
=======
	if (type == MLX5_EVENT_TYPE_PATH_MIG) {
		/* This event is only valid for trans_qps */
		to_mibqp(qp)->port = to_mibqp(qp)->trans_qp.alt_port;
	}
>>>>>>> v4.9.227

	if (ibqp->event_handler) {
		event.device     = ibqp->device;
		event.element.qp = ibqp;
		switch (type) {
		case MLX5_EVENT_TYPE_PATH_MIG:
			event.event = IB_EVENT_PATH_MIG;
			break;
		case MLX5_EVENT_TYPE_COMM_EST:
			event.event = IB_EVENT_COMM_EST;
			break;
		case MLX5_EVENT_TYPE_SQ_DRAINED:
			event.event = IB_EVENT_SQ_DRAINED;
			break;
		case MLX5_EVENT_TYPE_SRQ_LAST_WQE:
			event.event = IB_EVENT_QP_LAST_WQE_REACHED;
			break;
		case MLX5_EVENT_TYPE_WQ_CATAS_ERROR:
			event.event = IB_EVENT_QP_FATAL;
			break;
		case MLX5_EVENT_TYPE_PATH_MIG_FAILED:
			event.event = IB_EVENT_PATH_MIG_ERR;
			break;
		case MLX5_EVENT_TYPE_WQ_INVAL_REQ_ERROR:
			event.event = IB_EVENT_QP_REQ_ERR;
			break;
		case MLX5_EVENT_TYPE_WQ_ACCESS_ERROR:
			event.event = IB_EVENT_QP_ACCESS_ERR;
			break;
		default:
			pr_warn("mlx5_ib: Unexpected event type %d on QP %06x\n", type, qp->qpn);
			return;
		}

		ibqp->event_handler(&event, ibqp->qp_context);
	}
}

static int set_rq_size(struct mlx5_ib_dev *dev, struct ib_qp_cap *cap,
		       int has_rq, struct mlx5_ib_qp *qp, struct mlx5_ib_create_qp *ucmd)
{
<<<<<<< HEAD
	struct mlx5_general_caps *gen;
	int wqe_size;
	int wq_size;

	gen = &dev->mdev->caps.gen;
	/* Sanity check RQ size before proceeding */
	if (cap->max_recv_wr  > gen->max_wqes)
=======
	int wqe_size;
	int wq_size;

	/* Sanity check RQ size before proceeding */
	if (cap->max_recv_wr > (1 << MLX5_CAP_GEN(dev->mdev, log_max_qp_sz)))
>>>>>>> v4.9.227
		return -EINVAL;

	if (!has_rq) {
		qp->rq.max_gs = 0;
		qp->rq.wqe_cnt = 0;
		qp->rq.wqe_shift = 0;
<<<<<<< HEAD
=======
		cap->max_recv_wr = 0;
		cap->max_recv_sge = 0;
>>>>>>> v4.9.227
	} else {
		if (ucmd) {
			qp->rq.wqe_cnt = ucmd->rq_wqe_count;
			if (ucmd->rq_wqe_shift > BITS_PER_BYTE * sizeof(ucmd->rq_wqe_shift))
				return -EINVAL;
			qp->rq.wqe_shift = ucmd->rq_wqe_shift;
			if ((1 << qp->rq.wqe_shift) / sizeof(struct mlx5_wqe_data_seg) < qp->wq_sig)
				return -EINVAL;
			qp->rq.max_gs = (1 << qp->rq.wqe_shift) / sizeof(struct mlx5_wqe_data_seg) - qp->wq_sig;
			qp->rq.max_post = qp->rq.wqe_cnt;
		} else {
			wqe_size = qp->wq_sig ? sizeof(struct mlx5_wqe_signature_seg) : 0;
			wqe_size += cap->max_recv_sge * sizeof(struct mlx5_wqe_data_seg);
			wqe_size = roundup_pow_of_two(wqe_size);
			wq_size = roundup_pow_of_two(cap->max_recv_wr) * wqe_size;
			wq_size = max_t(int, wq_size, MLX5_SEND_WQE_BB);
			qp->rq.wqe_cnt = wq_size / wqe_size;
<<<<<<< HEAD
			if (wqe_size > gen->max_rq_desc_sz) {
				mlx5_ib_dbg(dev, "wqe_size %d, max %d\n",
					    wqe_size,
					    gen->max_rq_desc_sz);
=======
			if (wqe_size > MLX5_CAP_GEN(dev->mdev, max_wqe_sz_rq)) {
				mlx5_ib_dbg(dev, "wqe_size %d, max %d\n",
					    wqe_size,
					    MLX5_CAP_GEN(dev->mdev,
							 max_wqe_sz_rq));
>>>>>>> v4.9.227
				return -EINVAL;
			}
			qp->rq.wqe_shift = ilog2(wqe_size);
			qp->rq.max_gs = (1 << qp->rq.wqe_shift) / sizeof(struct mlx5_wqe_data_seg) - qp->wq_sig;
			qp->rq.max_post = qp->rq.wqe_cnt;
		}
	}

	return 0;
}

<<<<<<< HEAD
static int sq_overhead(enum ib_qp_type qp_type)
{
	int size = 0;

	switch (qp_type) {
=======
static int sq_overhead(struct ib_qp_init_attr *attr)
{
	int size = 0;

	switch (attr->qp_type) {
>>>>>>> v4.9.227
	case IB_QPT_XRC_INI:
		size += sizeof(struct mlx5_wqe_xrc_seg);
		/* fall through */
	case IB_QPT_RC:
		size += sizeof(struct mlx5_wqe_ctrl_seg) +
<<<<<<< HEAD
			sizeof(struct mlx5_wqe_atomic_seg) +
			sizeof(struct mlx5_wqe_raddr_seg);
=======
			max(sizeof(struct mlx5_wqe_atomic_seg) +
			    sizeof(struct mlx5_wqe_raddr_seg),
			    sizeof(struct mlx5_wqe_umr_ctrl_seg) +
			    sizeof(struct mlx5_mkey_seg));
>>>>>>> v4.9.227
		break;

	case IB_QPT_XRC_TGT:
		return 0;

	case IB_QPT_UC:
		size += sizeof(struct mlx5_wqe_ctrl_seg) +
<<<<<<< HEAD
			sizeof(struct mlx5_wqe_raddr_seg) +
			sizeof(struct mlx5_wqe_umr_ctrl_seg) +
			sizeof(struct mlx5_mkey_seg);
		break;

	case IB_QPT_UD:
	case IB_QPT_SMI:
	case IB_QPT_GSI:
=======
			max(sizeof(struct mlx5_wqe_raddr_seg),
			    sizeof(struct mlx5_wqe_umr_ctrl_seg) +
			    sizeof(struct mlx5_mkey_seg));
		break;

	case IB_QPT_UD:
		if (attr->create_flags & IB_QP_CREATE_IPOIB_UD_LSO)
			size += sizeof(struct mlx5_wqe_eth_pad) +
				sizeof(struct mlx5_wqe_eth_seg);
		/* fall through */
	case IB_QPT_SMI:
	case MLX5_IB_QPT_HW_GSI:
>>>>>>> v4.9.227
		size += sizeof(struct mlx5_wqe_ctrl_seg) +
			sizeof(struct mlx5_wqe_datagram_seg);
		break;

	case MLX5_IB_QPT_REG_UMR:
		size += sizeof(struct mlx5_wqe_ctrl_seg) +
			sizeof(struct mlx5_wqe_umr_ctrl_seg) +
			sizeof(struct mlx5_mkey_seg);
		break;

	default:
		return -EINVAL;
	}

	return size;
}

static int calc_send_wqe(struct ib_qp_init_attr *attr)
{
	int inl_size = 0;
	int size;

<<<<<<< HEAD
	size = sq_overhead(attr->qp_type);
=======
	size = sq_overhead(attr);
>>>>>>> v4.9.227
	if (size < 0)
		return size;

	if (attr->cap.max_inline_data) {
		inl_size = size + sizeof(struct mlx5_wqe_inline_seg) +
			attr->cap.max_inline_data;
	}

	size += attr->cap.max_send_sge * sizeof(struct mlx5_wqe_data_seg);
	if (attr->create_flags & IB_QP_CREATE_SIGNATURE_EN &&
	    ALIGN(max_t(int, inl_size, size), MLX5_SEND_WQE_BB) < MLX5_SIG_WQE_SIZE)
			return MLX5_SIG_WQE_SIZE;
	else
		return ALIGN(max_t(int, inl_size, size), MLX5_SEND_WQE_BB);
}

<<<<<<< HEAD
static int calc_sq_size(struct mlx5_ib_dev *dev, struct ib_qp_init_attr *attr,
			struct mlx5_ib_qp *qp)
{
	struct mlx5_general_caps *gen;
	int wqe_size;
	int wq_size;

	gen = &dev->mdev->caps.gen;
=======
static int get_send_sge(struct ib_qp_init_attr *attr, int wqe_size)
{
	int max_sge;

	if (attr->qp_type == IB_QPT_RC)
		max_sge = (min_t(int, wqe_size, 512) -
			   sizeof(struct mlx5_wqe_ctrl_seg) -
			   sizeof(struct mlx5_wqe_raddr_seg)) /
			sizeof(struct mlx5_wqe_data_seg);
	else if (attr->qp_type == IB_QPT_XRC_INI)
		max_sge = (min_t(int, wqe_size, 512) -
			   sizeof(struct mlx5_wqe_ctrl_seg) -
			   sizeof(struct mlx5_wqe_xrc_seg) -
			   sizeof(struct mlx5_wqe_raddr_seg)) /
			sizeof(struct mlx5_wqe_data_seg);
	else
		max_sge = (wqe_size - sq_overhead(attr)) /
			sizeof(struct mlx5_wqe_data_seg);

	return min_t(int, max_sge, wqe_size - sq_overhead(attr) /
		     sizeof(struct mlx5_wqe_data_seg));
}

static int calc_sq_size(struct mlx5_ib_dev *dev, struct ib_qp_init_attr *attr,
			struct mlx5_ib_qp *qp)
{
	int wqe_size;
	int wq_size;

>>>>>>> v4.9.227
	if (!attr->cap.max_send_wr)
		return 0;

	wqe_size = calc_send_wqe(attr);
	mlx5_ib_dbg(dev, "wqe_size %d\n", wqe_size);
	if (wqe_size < 0)
		return wqe_size;

<<<<<<< HEAD
	if (wqe_size > gen->max_sq_desc_sz) {
		mlx5_ib_dbg(dev, "wqe_size(%d) > max_sq_desc_sz(%d)\n",
			    wqe_size, gen->max_sq_desc_sz);
		return -EINVAL;
	}

	qp->max_inline_data = wqe_size - sq_overhead(attr->qp_type) -
		sizeof(struct mlx5_wqe_inline_seg);
=======
	if (wqe_size > MLX5_CAP_GEN(dev->mdev, max_wqe_sz_sq)) {
		mlx5_ib_dbg(dev, "wqe_size(%d) > max_sq_desc_sz(%d)\n",
			    wqe_size, MLX5_CAP_GEN(dev->mdev, max_wqe_sz_sq));
		return -EINVAL;
	}

	qp->max_inline_data = wqe_size - sq_overhead(attr) -
			      sizeof(struct mlx5_wqe_inline_seg);
>>>>>>> v4.9.227
	attr->cap.max_inline_data = qp->max_inline_data;

	if (attr->create_flags & IB_QP_CREATE_SIGNATURE_EN)
		qp->signature_en = true;

	wq_size = roundup_pow_of_two(attr->cap.max_send_wr * wqe_size);
	qp->sq.wqe_cnt = wq_size / MLX5_SEND_WQE_BB;
<<<<<<< HEAD
	if (qp->sq.wqe_cnt > gen->max_wqes) {
		mlx5_ib_dbg(dev, "wqe count(%d) exceeds limits(%d)\n",
			    qp->sq.wqe_cnt, gen->max_wqes);
		return -ENOMEM;
	}
	qp->sq.wqe_shift = ilog2(MLX5_SEND_WQE_BB);
	qp->sq.max_gs = attr->cap.max_send_sge;
=======
	if (qp->sq.wqe_cnt > (1 << MLX5_CAP_GEN(dev->mdev, log_max_qp_sz))) {
		mlx5_ib_dbg(dev, "wqe count(%d) exceeds limits(%d)\n",
			    qp->sq.wqe_cnt,
			    1 << MLX5_CAP_GEN(dev->mdev, log_max_qp_sz));
		return -ENOMEM;
	}
	qp->sq.wqe_shift = ilog2(MLX5_SEND_WQE_BB);
	qp->sq.max_gs = get_send_sge(attr, wqe_size);
	if (qp->sq.max_gs < attr->cap.max_send_sge)
		return -ENOMEM;

	attr->cap.max_send_sge = qp->sq.max_gs;
>>>>>>> v4.9.227
	qp->sq.max_post = wq_size / wqe_size;
	attr->cap.max_send_wr = qp->sq.max_post;

	return wq_size;
}

static int set_user_buf_size(struct mlx5_ib_dev *dev,
			    struct mlx5_ib_qp *qp,
<<<<<<< HEAD
			    struct mlx5_ib_create_qp *ucmd)
{
	struct mlx5_general_caps *gen;
	int desc_sz = 1 << qp->sq.wqe_shift;

	gen = &dev->mdev->caps.gen;
	if (desc_sz > gen->max_sq_desc_sz) {
		mlx5_ib_warn(dev, "desc_sz %d, max_sq_desc_sz %d\n",
			     desc_sz, gen->max_sq_desc_sz);
=======
			    struct mlx5_ib_create_qp *ucmd,
			    struct mlx5_ib_qp_base *base,
			    struct ib_qp_init_attr *attr)
{
	int desc_sz = 1 << qp->sq.wqe_shift;

	if (desc_sz > MLX5_CAP_GEN(dev->mdev, max_wqe_sz_sq)) {
		mlx5_ib_warn(dev, "desc_sz %d, max_sq_desc_sz %d\n",
			     desc_sz, MLX5_CAP_GEN(dev->mdev, max_wqe_sz_sq));
>>>>>>> v4.9.227
		return -EINVAL;
	}

	if (ucmd->sq_wqe_count && ((1 << ilog2(ucmd->sq_wqe_count)) != ucmd->sq_wqe_count)) {
		mlx5_ib_warn(dev, "sq_wqe_count %d, sq_wqe_count %d\n",
			     ucmd->sq_wqe_count, ucmd->sq_wqe_count);
		return -EINVAL;
	}

	qp->sq.wqe_cnt = ucmd->sq_wqe_count;

<<<<<<< HEAD
	if (qp->sq.wqe_cnt > gen->max_wqes) {
		mlx5_ib_warn(dev, "wqe_cnt %d, max_wqes %d\n",
			     qp->sq.wqe_cnt, gen->max_wqes);
		return -EINVAL;
	}

	qp->buf_size = (qp->rq.wqe_cnt << qp->rq.wqe_shift) +
		(qp->sq.wqe_cnt << 6);
=======
	if (qp->sq.wqe_cnt > (1 << MLX5_CAP_GEN(dev->mdev, log_max_qp_sz))) {
		mlx5_ib_warn(dev, "wqe_cnt %d, max_wqes %d\n",
			     qp->sq.wqe_cnt,
			     1 << MLX5_CAP_GEN(dev->mdev, log_max_qp_sz));
		return -EINVAL;
	}

	if (attr->qp_type == IB_QPT_RAW_PACKET) {
		base->ubuffer.buf_size = qp->rq.wqe_cnt << qp->rq.wqe_shift;
		qp->raw_packet_qp.sq.ubuffer.buf_size = qp->sq.wqe_cnt << 6;
	} else {
		base->ubuffer.buf_size = (qp->rq.wqe_cnt << qp->rq.wqe_shift) +
					 (qp->sq.wqe_cnt << 6);
	}
>>>>>>> v4.9.227

	return 0;
}

static int qp_has_rq(struct ib_qp_init_attr *attr)
{
	if (attr->qp_type == IB_QPT_XRC_INI ||
	    attr->qp_type == IB_QPT_XRC_TGT || attr->srq ||
	    attr->qp_type == MLX5_IB_QPT_REG_UMR ||
	    !attr->cap.max_recv_wr)
		return 0;

	return 1;
}

static int first_med_uuar(void)
{
	return 1;
}

static int next_uuar(int n)
{
	n++;

	while (((n % 4) & 2))
		n++;

	return n;
}

static int num_med_uuar(struct mlx5_uuar_info *uuari)
{
	int n;

	n = uuari->num_uars * MLX5_NON_FP_BF_REGS_PER_PAGE -
		uuari->num_low_latency_uuars - 1;

	return n >= 0 ? n : 0;
}

static int max_uuari(struct mlx5_uuar_info *uuari)
{
	return uuari->num_uars * 4;
}

static int first_hi_uuar(struct mlx5_uuar_info *uuari)
{
	int med;
	int i;
	int t;

	med = num_med_uuar(uuari);
	for (t = 0, i = first_med_uuar();; i = next_uuar(i)) {
		t++;
		if (t == med)
			return next_uuar(i);
	}

	return 0;
}

static int alloc_high_class_uuar(struct mlx5_uuar_info *uuari)
{
	int i;

	for (i = first_hi_uuar(uuari); i < max_uuari(uuari); i = next_uuar(i)) {
		if (!test_bit(i, uuari->bitmap)) {
			set_bit(i, uuari->bitmap);
			uuari->count[i]++;
			return i;
		}
	}

	return -ENOMEM;
}

static int alloc_med_class_uuar(struct mlx5_uuar_info *uuari)
{
	int minidx = first_med_uuar();
	int i;

	for (i = first_med_uuar(); i < first_hi_uuar(uuari); i = next_uuar(i)) {
		if (uuari->count[i] < uuari->count[minidx])
			minidx = i;
	}

	uuari->count[minidx]++;
	return minidx;
}

static int alloc_uuar(struct mlx5_uuar_info *uuari,
		      enum mlx5_ib_latency_class lat)
{
	int uuarn = -EINVAL;

	mutex_lock(&uuari->lock);
	switch (lat) {
	case MLX5_IB_LATENCY_CLASS_LOW:
		uuarn = 0;
		uuari->count[uuarn]++;
		break;

	case MLX5_IB_LATENCY_CLASS_MEDIUM:
		if (uuari->ver < 2)
			uuarn = -ENOMEM;
		else
			uuarn = alloc_med_class_uuar(uuari);
		break;

	case MLX5_IB_LATENCY_CLASS_HIGH:
		if (uuari->ver < 2)
			uuarn = -ENOMEM;
		else
			uuarn = alloc_high_class_uuar(uuari);
		break;

	case MLX5_IB_LATENCY_CLASS_FAST_PATH:
		uuarn = 2;
		break;
	}
	mutex_unlock(&uuari->lock);

	return uuarn;
}

static void free_med_class_uuar(struct mlx5_uuar_info *uuari, int uuarn)
{
	clear_bit(uuarn, uuari->bitmap);
	--uuari->count[uuarn];
}

static void free_high_class_uuar(struct mlx5_uuar_info *uuari, int uuarn)
{
	clear_bit(uuarn, uuari->bitmap);
	--uuari->count[uuarn];
}

static void free_uuar(struct mlx5_uuar_info *uuari, int uuarn)
{
	int nuuars = uuari->num_uars * MLX5_BF_REGS_PER_PAGE;
	int high_uuar = nuuars - uuari->num_low_latency_uuars;

	mutex_lock(&uuari->lock);
	if (uuarn == 0) {
		--uuari->count[uuarn];
		goto out;
	}

	if (uuarn < high_uuar) {
		free_med_class_uuar(uuari, uuarn);
		goto out;
	}

	free_high_class_uuar(uuari, uuarn);

out:
	mutex_unlock(&uuari->lock);
}

static enum mlx5_qp_state to_mlx5_state(enum ib_qp_state state)
{
	switch (state) {
	case IB_QPS_RESET:	return MLX5_QP_STATE_RST;
	case IB_QPS_INIT:	return MLX5_QP_STATE_INIT;
	case IB_QPS_RTR:	return MLX5_QP_STATE_RTR;
	case IB_QPS_RTS:	return MLX5_QP_STATE_RTS;
	case IB_QPS_SQD:	return MLX5_QP_STATE_SQD;
	case IB_QPS_SQE:	return MLX5_QP_STATE_SQER;
	case IB_QPS_ERR:	return MLX5_QP_STATE_ERR;
	default:		return -1;
	}
}

static int to_mlx5_st(enum ib_qp_type type)
{
	switch (type) {
	case IB_QPT_RC:			return MLX5_QP_ST_RC;
	case IB_QPT_UC:			return MLX5_QP_ST_UC;
	case IB_QPT_UD:			return MLX5_QP_ST_UD;
	case MLX5_IB_QPT_REG_UMR:	return MLX5_QP_ST_REG_UMR;
	case IB_QPT_XRC_INI:
	case IB_QPT_XRC_TGT:		return MLX5_QP_ST_XRC;
	case IB_QPT_SMI:		return MLX5_QP_ST_QP0;
<<<<<<< HEAD
	case IB_QPT_GSI:		return MLX5_QP_ST_QP1;
	case IB_QPT_RAW_IPV6:		return MLX5_QP_ST_RAW_IPV6;
	case IB_QPT_RAW_ETHERTYPE:	return MLX5_QP_ST_RAW_ETHERTYPE;
	case IB_QPT_RAW_PACKET:
=======
	case MLX5_IB_QPT_HW_GSI:	return MLX5_QP_ST_QP1;
	case IB_QPT_RAW_IPV6:		return MLX5_QP_ST_RAW_IPV6;
	case IB_QPT_RAW_PACKET:
	case IB_QPT_RAW_ETHERTYPE:	return MLX5_QP_ST_RAW_ETHERTYPE;
>>>>>>> v4.9.227
	case IB_QPT_MAX:
	default:		return -EINVAL;
	}
}

<<<<<<< HEAD
=======
static void mlx5_ib_lock_cqs(struct mlx5_ib_cq *send_cq,
			     struct mlx5_ib_cq *recv_cq);
static void mlx5_ib_unlock_cqs(struct mlx5_ib_cq *send_cq,
			       struct mlx5_ib_cq *recv_cq);

>>>>>>> v4.9.227
static int uuarn_to_uar_index(struct mlx5_uuar_info *uuari, int uuarn)
{
	return uuari->uars[uuarn / MLX5_BF_REGS_PER_PAGE].index;
}

<<<<<<< HEAD
static int create_user_qp(struct mlx5_ib_dev *dev, struct ib_pd *pd,
			  struct mlx5_ib_qp *qp, struct ib_udata *udata,
			  struct mlx5_create_qp_mbox_in **in,
			  struct mlx5_ib_create_qp_resp *resp, int *inlen)
{
	struct mlx5_ib_ucontext *context;
	struct mlx5_ib_create_qp ucmd;
=======
static int mlx5_ib_umem_get(struct mlx5_ib_dev *dev,
			    struct ib_pd *pd,
			    unsigned long addr, size_t size,
			    struct ib_umem **umem,
			    int *npages, int *page_shift, int *ncont,
			    u32 *offset)
{
	int err;

	*umem = ib_umem_get(pd->uobject->context, addr, size, 0, 0);
	if (IS_ERR(*umem)) {
		mlx5_ib_dbg(dev, "umem_get failed\n");
		return PTR_ERR(*umem);
	}

	mlx5_ib_cont_pages(*umem, addr, npages, page_shift, ncont, NULL);

	err = mlx5_ib_get_buf_offset(addr, *page_shift, offset);
	if (err) {
		mlx5_ib_warn(dev, "bad offset\n");
		goto err_umem;
	}

	mlx5_ib_dbg(dev, "addr 0x%lx, size %zu, npages %d, page_shift %d, ncont %d, offset %d\n",
		    addr, size, *npages, *page_shift, *ncont, *offset);

	return 0;

err_umem:
	ib_umem_release(*umem);
	*umem = NULL;

	return err;
}

static void destroy_user_rq(struct ib_pd *pd, struct mlx5_ib_rwq *rwq)
{
	struct mlx5_ib_ucontext *context;

	context = to_mucontext(pd->uobject->context);
	mlx5_ib_db_unmap_user(context, &rwq->db);
	if (rwq->umem)
		ib_umem_release(rwq->umem);
}

static int create_user_rq(struct mlx5_ib_dev *dev, struct ib_pd *pd,
			  struct mlx5_ib_rwq *rwq,
			  struct mlx5_ib_create_wq *ucmd)
{
	struct mlx5_ib_ucontext *context;
	int page_shift = 0;
	int npages;
	u32 offset = 0;
	int ncont = 0;
	int err;

	if (!ucmd->buf_addr)
		return -EINVAL;

	context = to_mucontext(pd->uobject->context);
	rwq->umem = ib_umem_get(pd->uobject->context, ucmd->buf_addr,
			       rwq->buf_size, 0, 0);
	if (IS_ERR(rwq->umem)) {
		mlx5_ib_dbg(dev, "umem_get failed\n");
		err = PTR_ERR(rwq->umem);
		return err;
	}

	mlx5_ib_cont_pages(rwq->umem, ucmd->buf_addr, &npages, &page_shift,
			   &ncont, NULL);
	err = mlx5_ib_get_buf_offset(ucmd->buf_addr, page_shift,
				     &rwq->rq_page_offset);
	if (err) {
		mlx5_ib_warn(dev, "bad offset\n");
		goto err_umem;
	}

	rwq->rq_num_pas = ncont;
	rwq->page_shift = page_shift;
	rwq->log_page_size =  page_shift - MLX5_ADAPTER_PAGE_SHIFT;
	rwq->wq_sig = !!(ucmd->flags & MLX5_WQ_FLAG_SIGNATURE);

	mlx5_ib_dbg(dev, "addr 0x%llx, size %zd, npages %d, page_shift %d, ncont %d, offset %d\n",
		    (unsigned long long)ucmd->buf_addr, rwq->buf_size,
		    npages, page_shift, ncont, offset);

	err = mlx5_ib_db_map_user(context, ucmd->db_addr, &rwq->db);
	if (err) {
		mlx5_ib_dbg(dev, "map failed\n");
		goto err_umem;
	}

	rwq->create_type = MLX5_WQ_USER;
	return 0;

err_umem:
	ib_umem_release(rwq->umem);
	return err;
}

static int create_user_qp(struct mlx5_ib_dev *dev, struct ib_pd *pd,
			  struct mlx5_ib_qp *qp, struct ib_udata *udata,
			  struct ib_qp_init_attr *attr,
			  u32 **in,
			  struct mlx5_ib_create_qp_resp *resp, int *inlen,
			  struct mlx5_ib_qp_base *base)
{
	struct mlx5_ib_ucontext *context;
	struct mlx5_ib_create_qp ucmd;
	struct mlx5_ib_ubuffer *ubuffer = &base->ubuffer;
>>>>>>> v4.9.227
	int page_shift = 0;
	int uar_index;
	int npages;
	u32 offset = 0;
	int uuarn;
	int ncont = 0;
<<<<<<< HEAD
=======
	__be64 *pas;
	void *qpc;
>>>>>>> v4.9.227
	int err;

	err = ib_copy_from_udata(&ucmd, udata, sizeof(ucmd));
	if (err) {
		mlx5_ib_dbg(dev, "copy failed\n");
		return err;
	}

	context = to_mucontext(pd->uobject->context);
	/*
	 * TBD: should come from the verbs when we have the API
	 */
<<<<<<< HEAD
	uuarn = alloc_uuar(&context->uuari, MLX5_IB_LATENCY_CLASS_HIGH);
	if (uuarn < 0) {
		mlx5_ib_dbg(dev, "failed to allocate low latency UUAR\n");
		mlx5_ib_dbg(dev, "reverting to medium latency\n");
		uuarn = alloc_uuar(&context->uuari, MLX5_IB_LATENCY_CLASS_MEDIUM);
		if (uuarn < 0) {
			mlx5_ib_dbg(dev, "failed to allocate medium latency UUAR\n");
			mlx5_ib_dbg(dev, "reverting to high latency\n");
			uuarn = alloc_uuar(&context->uuari, MLX5_IB_LATENCY_CLASS_LOW);
			if (uuarn < 0) {
				mlx5_ib_warn(dev, "uuar allocation failed\n");
				return uuarn;
=======
	if (qp->flags & MLX5_IB_QP_CROSS_CHANNEL)
		/* In CROSS_CHANNEL CQ and QP must use the same UAR */
		uuarn = MLX5_CROSS_CHANNEL_UUAR;
	else {
		uuarn = alloc_uuar(&context->uuari, MLX5_IB_LATENCY_CLASS_HIGH);
		if (uuarn < 0) {
			mlx5_ib_dbg(dev, "failed to allocate low latency UUAR\n");
			mlx5_ib_dbg(dev, "reverting to medium latency\n");
			uuarn = alloc_uuar(&context->uuari, MLX5_IB_LATENCY_CLASS_MEDIUM);
			if (uuarn < 0) {
				mlx5_ib_dbg(dev, "failed to allocate medium latency UUAR\n");
				mlx5_ib_dbg(dev, "reverting to high latency\n");
				uuarn = alloc_uuar(&context->uuari, MLX5_IB_LATENCY_CLASS_LOW);
				if (uuarn < 0) {
					mlx5_ib_warn(dev, "uuar allocation failed\n");
					return uuarn;
				}
>>>>>>> v4.9.227
			}
		}
	}

	uar_index = uuarn_to_uar_index(&context->uuari, uuarn);
	mlx5_ib_dbg(dev, "uuarn 0x%x, uar_index 0x%x\n", uuarn, uar_index);

	qp->rq.offset = 0;
	qp->sq.wqe_shift = ilog2(MLX5_SEND_WQE_BB);
	qp->sq.offset = qp->rq.wqe_cnt << qp->rq.wqe_shift;

<<<<<<< HEAD
	err = set_user_buf_size(dev, qp, &ucmd);
	if (err)
		goto err_uuar;

	if (ucmd.buf_addr && qp->buf_size) {
		qp->umem = ib_umem_get(pd->uobject->context, ucmd.buf_addr,
				       qp->buf_size, 0, 0);
		if (IS_ERR(qp->umem)) {
			mlx5_ib_dbg(dev, "umem_get failed\n");
			err = PTR_ERR(qp->umem);
			goto err_uuar;
		}
	} else {
		qp->umem = NULL;
	}

	if (qp->umem) {
		mlx5_ib_cont_pages(qp->umem, ucmd.buf_addr, &npages, &page_shift,
				   &ncont, NULL);
		err = mlx5_ib_get_buf_offset(ucmd.buf_addr, page_shift, &offset);
		if (err) {
			mlx5_ib_warn(dev, "bad offset\n");
			goto err_umem;
		}
		mlx5_ib_dbg(dev, "addr 0x%llx, size %d, npages %d, page_shift %d, ncont %d, offset %d\n",
			    ucmd.buf_addr, qp->buf_size, npages, page_shift, ncont, offset);
	}

	*inlen = sizeof(**in) + sizeof(*(*in)->pas) * ncont;
=======
	err = set_user_buf_size(dev, qp, &ucmd, base, attr);
	if (err)
		goto err_uuar;

	if (ucmd.buf_addr && ubuffer->buf_size) {
		ubuffer->buf_addr = ucmd.buf_addr;
		err = mlx5_ib_umem_get(dev, pd, ubuffer->buf_addr,
				       ubuffer->buf_size,
				       &ubuffer->umem, &npages, &page_shift,
				       &ncont, &offset);
		if (err)
			goto err_uuar;
	} else {
		ubuffer->umem = NULL;
	}

	*inlen = MLX5_ST_SZ_BYTES(create_qp_in) +
		 MLX5_FLD_SZ_BYTES(create_qp_in, pas[0]) * ncont;
>>>>>>> v4.9.227
	*in = mlx5_vzalloc(*inlen);
	if (!*in) {
		err = -ENOMEM;
		goto err_umem;
	}
<<<<<<< HEAD
	if (qp->umem)
		mlx5_ib_populate_pas(dev, qp->umem, page_shift, (*in)->pas, 0);
	(*in)->ctx.log_pg_sz_remote_qpn =
		cpu_to_be32((page_shift - MLX5_ADAPTER_PAGE_SHIFT) << 24);
	(*in)->ctx.params2 = cpu_to_be32(offset << 6);

	(*in)->ctx.qp_counter_set_usr_page = cpu_to_be32(uar_index);
=======

	pas = (__be64 *)MLX5_ADDR_OF(create_qp_in, *in, pas);
	if (ubuffer->umem)
		mlx5_ib_populate_pas(dev, ubuffer->umem, page_shift, pas, 0);

	qpc = MLX5_ADDR_OF(create_qp_in, *in, qpc);

	MLX5_SET(qpc, qpc, log_page_size, page_shift - MLX5_ADAPTER_PAGE_SHIFT);
	MLX5_SET(qpc, qpc, page_offset, offset);

	MLX5_SET(qpc, qpc, uar_page, uar_index);
>>>>>>> v4.9.227
	resp->uuar_index = uuarn;
	qp->uuarn = uuarn;

	err = mlx5_ib_db_map_user(context, ucmd.db_addr, &qp->db);
	if (err) {
		mlx5_ib_dbg(dev, "map failed\n");
		goto err_free;
	}

	err = ib_copy_to_udata(udata, resp, sizeof(*resp));
	if (err) {
		mlx5_ib_dbg(dev, "copy failed\n");
		goto err_unmap;
	}
	qp->create_type = MLX5_QP_USER;

	return 0;

err_unmap:
	mlx5_ib_db_unmap_user(context, &qp->db);

err_free:
<<<<<<< HEAD
	mlx5_vfree(*in);

err_umem:
	if (qp->umem)
		ib_umem_release(qp->umem);
=======
	kvfree(*in);

err_umem:
	if (ubuffer->umem)
		ib_umem_release(ubuffer->umem);
>>>>>>> v4.9.227

err_uuar:
	free_uuar(&context->uuari, uuarn);
	return err;
}

<<<<<<< HEAD
static void destroy_qp_user(struct ib_pd *pd, struct mlx5_ib_qp *qp)
=======
static void destroy_qp_user(struct ib_pd *pd, struct mlx5_ib_qp *qp,
			    struct mlx5_ib_qp_base *base)
>>>>>>> v4.9.227
{
	struct mlx5_ib_ucontext *context;

	context = to_mucontext(pd->uobject->context);
	mlx5_ib_db_unmap_user(context, &qp->db);
<<<<<<< HEAD
	if (qp->umem)
		ib_umem_release(qp->umem);
=======
	if (base->ubuffer.umem)
		ib_umem_release(base->ubuffer.umem);
>>>>>>> v4.9.227
	free_uuar(&context->uuari, qp->uuarn);
}

static int create_kernel_qp(struct mlx5_ib_dev *dev,
			    struct ib_qp_init_attr *init_attr,
			    struct mlx5_ib_qp *qp,
<<<<<<< HEAD
			    struct mlx5_create_qp_mbox_in **in, int *inlen)
=======
			    u32 **in, int *inlen,
			    struct mlx5_ib_qp_base *base)
>>>>>>> v4.9.227
{
	enum mlx5_ib_latency_class lc = MLX5_IB_LATENCY_CLASS_LOW;
	struct mlx5_uuar_info *uuari;
	int uar_index;
<<<<<<< HEAD
=======
	void *qpc;
>>>>>>> v4.9.227
	int uuarn;
	int err;

	uuari = &dev->mdev->priv.uuari;
<<<<<<< HEAD
	if (init_attr->create_flags & ~(IB_QP_CREATE_SIGNATURE_EN | IB_QP_CREATE_BLOCK_MULTICAST_LOOPBACK))
=======
	if (init_attr->create_flags & ~(IB_QP_CREATE_SIGNATURE_EN |
					IB_QP_CREATE_BLOCK_MULTICAST_LOOPBACK |
					IB_QP_CREATE_IPOIB_UD_LSO |
					mlx5_ib_create_qp_sqpn_qp1()))
>>>>>>> v4.9.227
		return -EINVAL;

	if (init_attr->qp_type == MLX5_IB_QPT_REG_UMR)
		lc = MLX5_IB_LATENCY_CLASS_FAST_PATH;

	uuarn = alloc_uuar(uuari, lc);
	if (uuarn < 0) {
		mlx5_ib_dbg(dev, "\n");
		return -ENOMEM;
	}

	qp->bf = &uuari->bfs[uuarn];
	uar_index = qp->bf->uar->index;

	err = calc_sq_size(dev, init_attr, qp);
	if (err < 0) {
		mlx5_ib_dbg(dev, "err %d\n", err);
		goto err_uuar;
	}

	qp->rq.offset = 0;
	qp->sq.offset = qp->rq.wqe_cnt << qp->rq.wqe_shift;
<<<<<<< HEAD
	qp->buf_size = err + (qp->rq.wqe_cnt << qp->rq.wqe_shift);

	err = mlx5_buf_alloc(dev->mdev, qp->buf_size, PAGE_SIZE * 2, &qp->buf);
=======
	base->ubuffer.buf_size = err + (qp->rq.wqe_cnt << qp->rq.wqe_shift);

	err = mlx5_buf_alloc(dev->mdev, base->ubuffer.buf_size, &qp->buf);
>>>>>>> v4.9.227
	if (err) {
		mlx5_ib_dbg(dev, "err %d\n", err);
		goto err_uuar;
	}

	qp->sq.qend = mlx5_get_send_wqe(qp, qp->sq.wqe_cnt);
<<<<<<< HEAD
	*inlen = sizeof(**in) + sizeof(*(*in)->pas) * qp->buf.npages;
=======
	*inlen = MLX5_ST_SZ_BYTES(create_qp_in) +
		 MLX5_FLD_SZ_BYTES(create_qp_in, pas[0]) * qp->buf.npages;
>>>>>>> v4.9.227
	*in = mlx5_vzalloc(*inlen);
	if (!*in) {
		err = -ENOMEM;
		goto err_buf;
	}
<<<<<<< HEAD
	(*in)->ctx.qp_counter_set_usr_page = cpu_to_be32(uar_index);
	(*in)->ctx.log_pg_sz_remote_qpn =
		cpu_to_be32((qp->buf.page_shift - MLX5_ADAPTER_PAGE_SHIFT) << 24);
	/* Set "fast registration enabled" for all kernel QPs */
	(*in)->ctx.params1 |= cpu_to_be32(1 << 11);
	(*in)->ctx.sq_crq_size |= cpu_to_be16(1 << 4);

	mlx5_fill_page_array(&qp->buf, (*in)->pas);
=======

	qpc = MLX5_ADDR_OF(create_qp_in, *in, qpc);
	MLX5_SET(qpc, qpc, uar_page, uar_index);
	MLX5_SET(qpc, qpc, log_page_size, qp->buf.page_shift - MLX5_ADAPTER_PAGE_SHIFT);

	/* Set "fast registration enabled" for all kernel QPs */
	MLX5_SET(qpc, qpc, fre, 1);
	MLX5_SET(qpc, qpc, rlky, 1);

	if (init_attr->create_flags & mlx5_ib_create_qp_sqpn_qp1()) {
		MLX5_SET(qpc, qpc, deth_sqpn, 1);
		qp->flags |= MLX5_IB_QP_SQPN_QP1;
	}

	mlx5_fill_page_array(&qp->buf,
			     (__be64 *)MLX5_ADDR_OF(create_qp_in, *in, pas));
>>>>>>> v4.9.227

	err = mlx5_db_alloc(dev->mdev, &qp->db);
	if (err) {
		mlx5_ib_dbg(dev, "err %d\n", err);
		goto err_free;
	}

<<<<<<< HEAD
	qp->db.db[0] = 0;
	qp->db.db[1] = 0;

=======
>>>>>>> v4.9.227
	qp->sq.wrid = kmalloc(qp->sq.wqe_cnt * sizeof(*qp->sq.wrid), GFP_KERNEL);
	qp->sq.wr_data = kmalloc(qp->sq.wqe_cnt * sizeof(*qp->sq.wr_data), GFP_KERNEL);
	qp->rq.wrid = kmalloc(qp->rq.wqe_cnt * sizeof(*qp->rq.wrid), GFP_KERNEL);
	qp->sq.w_list = kmalloc(qp->sq.wqe_cnt * sizeof(*qp->sq.w_list), GFP_KERNEL);
	qp->sq.wqe_head = kmalloc(qp->sq.wqe_cnt * sizeof(*qp->sq.wqe_head), GFP_KERNEL);

	if (!qp->sq.wrid || !qp->sq.wr_data || !qp->rq.wrid ||
	    !qp->sq.w_list || !qp->sq.wqe_head) {
		err = -ENOMEM;
		goto err_wrid;
	}
	qp->create_type = MLX5_QP_KERNEL;

	return 0;

err_wrid:
	mlx5_db_free(dev->mdev, &qp->db);
	kfree(qp->sq.wqe_head);
	kfree(qp->sq.w_list);
	kfree(qp->sq.wrid);
	kfree(qp->sq.wr_data);
	kfree(qp->rq.wrid);

err_free:
<<<<<<< HEAD
	mlx5_vfree(*in);
=======
	kvfree(*in);
>>>>>>> v4.9.227

err_buf:
	mlx5_buf_free(dev->mdev, &qp->buf);

err_uuar:
	free_uuar(&dev->mdev->priv.uuari, uuarn);
	return err;
}

static void destroy_qp_kernel(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp)
{
	mlx5_db_free(dev->mdev, &qp->db);
	kfree(qp->sq.wqe_head);
	kfree(qp->sq.w_list);
	kfree(qp->sq.wrid);
	kfree(qp->sq.wr_data);
	kfree(qp->rq.wrid);
	mlx5_buf_free(dev->mdev, &qp->buf);
	free_uuar(&dev->mdev->priv.uuari, qp->bf->uuarn);
}

<<<<<<< HEAD
static __be32 get_rx_type(struct mlx5_ib_qp *qp, struct ib_qp_init_attr *attr)
{
	if (attr->srq || (attr->qp_type == IB_QPT_XRC_TGT) ||
	    (attr->qp_type == IB_QPT_XRC_INI))
		return cpu_to_be32(MLX5_SRQ_RQ);
	else if (!qp->has_rq)
		return cpu_to_be32(MLX5_ZERO_LEN_RQ);
	else
		return cpu_to_be32(MLX5_NON_ZERO_RQ);
=======
static u32 get_rx_type(struct mlx5_ib_qp *qp, struct ib_qp_init_attr *attr)
{
	if (attr->srq || (attr->qp_type == IB_QPT_XRC_TGT) ||
	    (attr->qp_type == IB_QPT_XRC_INI))
		return MLX5_SRQ_RQ;
	else if (!qp->has_rq)
		return MLX5_ZERO_LEN_RQ;
	else
		return MLX5_NON_ZERO_RQ;
>>>>>>> v4.9.227
}

static int is_connected(enum ib_qp_type qp_type)
{
	if (qp_type == IB_QPT_RC || qp_type == IB_QPT_UC)
		return 1;

	return 0;
}

<<<<<<< HEAD
=======
static int create_raw_packet_qp_tis(struct mlx5_ib_dev *dev,
				    struct mlx5_ib_sq *sq, u32 tdn)
{
	u32 in[MLX5_ST_SZ_DW(create_tis_in)] = {0};
	void *tisc = MLX5_ADDR_OF(create_tis_in, in, ctx);

	MLX5_SET(tisc, tisc, transport_domain, tdn);
	return mlx5_core_create_tis(dev->mdev, in, sizeof(in), &sq->tisn);
}

static void destroy_raw_packet_qp_tis(struct mlx5_ib_dev *dev,
				      struct mlx5_ib_sq *sq)
{
	mlx5_core_destroy_tis(dev->mdev, sq->tisn);
}

static int create_raw_packet_qp_sq(struct mlx5_ib_dev *dev,
				   struct mlx5_ib_sq *sq, void *qpin,
				   struct ib_pd *pd)
{
	struct mlx5_ib_ubuffer *ubuffer = &sq->ubuffer;
	__be64 *pas;
	void *in;
	void *sqc;
	void *qpc = MLX5_ADDR_OF(create_qp_in, qpin, qpc);
	void *wq;
	int inlen;
	int err;
	int page_shift = 0;
	int npages;
	int ncont = 0;
	u32 offset = 0;

	err = mlx5_ib_umem_get(dev, pd, ubuffer->buf_addr, ubuffer->buf_size,
			       &sq->ubuffer.umem, &npages, &page_shift,
			       &ncont, &offset);
	if (err)
		return err;

	inlen = MLX5_ST_SZ_BYTES(create_sq_in) + sizeof(u64) * ncont;
	in = mlx5_vzalloc(inlen);
	if (!in) {
		err = -ENOMEM;
		goto err_umem;
	}

	sqc = MLX5_ADDR_OF(create_sq_in, in, ctx);
	MLX5_SET(sqc, sqc, flush_in_error_en, 1);
	MLX5_SET(sqc, sqc, state, MLX5_SQC_STATE_RST);
	MLX5_SET(sqc, sqc, user_index, MLX5_GET(qpc, qpc, user_index));
	MLX5_SET(sqc, sqc, cqn, MLX5_GET(qpc, qpc, cqn_snd));
	MLX5_SET(sqc, sqc, tis_lst_sz, 1);
	MLX5_SET(sqc, sqc, tis_num_0, sq->tisn);

	wq = MLX5_ADDR_OF(sqc, sqc, wq);
	MLX5_SET(wq, wq, wq_type, MLX5_WQ_TYPE_CYCLIC);
	MLX5_SET(wq, wq, pd, MLX5_GET(qpc, qpc, pd));
	MLX5_SET(wq, wq, uar_page, MLX5_GET(qpc, qpc, uar_page));
	MLX5_SET64(wq, wq, dbr_addr, MLX5_GET64(qpc, qpc, dbr_addr));
	MLX5_SET(wq, wq, log_wq_stride, ilog2(MLX5_SEND_WQE_BB));
	MLX5_SET(wq, wq, log_wq_sz, MLX5_GET(qpc, qpc, log_sq_size));
	MLX5_SET(wq, wq, log_wq_pg_sz,  page_shift - MLX5_ADAPTER_PAGE_SHIFT);
	MLX5_SET(wq, wq, page_offset, offset);

	pas = (__be64 *)MLX5_ADDR_OF(wq, wq, pas);
	mlx5_ib_populate_pas(dev, sq->ubuffer.umem, page_shift, pas, 0);

	err = mlx5_core_create_sq_tracked(dev->mdev, in, inlen, &sq->base.mqp);

	kvfree(in);

	if (err)
		goto err_umem;

	return 0;

err_umem:
	ib_umem_release(sq->ubuffer.umem);
	sq->ubuffer.umem = NULL;

	return err;
}

static void destroy_raw_packet_qp_sq(struct mlx5_ib_dev *dev,
				     struct mlx5_ib_sq *sq)
{
	mlx5_core_destroy_sq_tracked(dev->mdev, &sq->base.mqp);
	ib_umem_release(sq->ubuffer.umem);
}

static size_t get_rq_pas_size(void *qpc)
{
	u32 log_page_size = MLX5_GET(qpc, qpc, log_page_size) + 12;
	u32 log_rq_stride = MLX5_GET(qpc, qpc, log_rq_stride);
	u32 log_rq_size   = MLX5_GET(qpc, qpc, log_rq_size);
	u32 page_offset   = MLX5_GET(qpc, qpc, page_offset);
	u32 po_quanta	  = 1 << (log_page_size - 6);
	u32 rq_sz	  = 1 << (log_rq_size + 4 + log_rq_stride);
	u32 page_size	  = 1 << log_page_size;
	u32 rq_sz_po      = rq_sz + (page_offset * po_quanta);
	u32 rq_num_pas	  = (rq_sz_po + page_size - 1) / page_size;

	return rq_num_pas * sizeof(u64);
}

static int create_raw_packet_qp_rq(struct mlx5_ib_dev *dev,
				   struct mlx5_ib_rq *rq, void *qpin,
				   size_t qpinlen)
{
	struct mlx5_ib_qp *mqp = rq->base.container_mibqp;
	__be64 *pas;
	__be64 *qp_pas;
	void *in;
	void *rqc;
	void *wq;
	void *qpc = MLX5_ADDR_OF(create_qp_in, qpin, qpc);
	size_t rq_pas_size = get_rq_pas_size(qpc);
	size_t inlen;
	int err;

	if (qpinlen < rq_pas_size + MLX5_BYTE_OFF(create_qp_in, pas))
		return -EINVAL;

	inlen = MLX5_ST_SZ_BYTES(create_rq_in) + rq_pas_size;
	in = mlx5_vzalloc(inlen);
	if (!in)
		return -ENOMEM;

	rqc = MLX5_ADDR_OF(create_rq_in, in, ctx);
	MLX5_SET(rqc, rqc, vsd, 1);
	MLX5_SET(rqc, rqc, mem_rq_type, MLX5_RQC_MEM_RQ_TYPE_MEMORY_RQ_INLINE);
	MLX5_SET(rqc, rqc, state, MLX5_RQC_STATE_RST);
	MLX5_SET(rqc, rqc, flush_in_error_en, 1);
	MLX5_SET(rqc, rqc, user_index, MLX5_GET(qpc, qpc, user_index));
	MLX5_SET(rqc, rqc, cqn, MLX5_GET(qpc, qpc, cqn_rcv));

	if (mqp->flags & MLX5_IB_QP_CAP_SCATTER_FCS)
		MLX5_SET(rqc, rqc, scatter_fcs, 1);

	wq = MLX5_ADDR_OF(rqc, rqc, wq);
	MLX5_SET(wq, wq, wq_type, MLX5_WQ_TYPE_CYCLIC);
	MLX5_SET(wq, wq, end_padding_mode,
		 MLX5_GET(qpc, qpc, end_padding_mode));
	MLX5_SET(wq, wq, page_offset, MLX5_GET(qpc, qpc, page_offset));
	MLX5_SET(wq, wq, pd, MLX5_GET(qpc, qpc, pd));
	MLX5_SET64(wq, wq, dbr_addr, MLX5_GET64(qpc, qpc, dbr_addr));
	MLX5_SET(wq, wq, log_wq_stride, MLX5_GET(qpc, qpc, log_rq_stride) + 4);
	MLX5_SET(wq, wq, log_wq_pg_sz, MLX5_GET(qpc, qpc, log_page_size));
	MLX5_SET(wq, wq, log_wq_sz, MLX5_GET(qpc, qpc, log_rq_size));

	pas = (__be64 *)MLX5_ADDR_OF(wq, wq, pas);
	qp_pas = (__be64 *)MLX5_ADDR_OF(create_qp_in, qpin, pas);
	memcpy(pas, qp_pas, rq_pas_size);

	err = mlx5_core_create_rq_tracked(dev->mdev, in, inlen, &rq->base.mqp);

	kvfree(in);

	return err;
}

static void destroy_raw_packet_qp_rq(struct mlx5_ib_dev *dev,
				     struct mlx5_ib_rq *rq)
{
	mlx5_core_destroy_rq_tracked(dev->mdev, &rq->base.mqp);
}

static int create_raw_packet_qp_tir(struct mlx5_ib_dev *dev,
				    struct mlx5_ib_rq *rq, u32 tdn)
{
	u32 *in;
	void *tirc;
	int inlen;
	int err;

	inlen = MLX5_ST_SZ_BYTES(create_tir_in);
	in = mlx5_vzalloc(inlen);
	if (!in)
		return -ENOMEM;

	tirc = MLX5_ADDR_OF(create_tir_in, in, ctx);
	MLX5_SET(tirc, tirc, disp_type, MLX5_TIRC_DISP_TYPE_DIRECT);
	MLX5_SET(tirc, tirc, inline_rqn, rq->base.mqp.qpn);
	MLX5_SET(tirc, tirc, transport_domain, tdn);

	err = mlx5_core_create_tir(dev->mdev, in, inlen, &rq->tirn);

	kvfree(in);

	return err;
}

static void destroy_raw_packet_qp_tir(struct mlx5_ib_dev *dev,
				      struct mlx5_ib_rq *rq)
{
	mlx5_core_destroy_tir(dev->mdev, rq->tirn);
}

static int create_raw_packet_qp(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp,
				u32 *in, size_t inlen,
				struct ib_pd *pd)
{
	struct mlx5_ib_raw_packet_qp *raw_packet_qp = &qp->raw_packet_qp;
	struct mlx5_ib_sq *sq = &raw_packet_qp->sq;
	struct mlx5_ib_rq *rq = &raw_packet_qp->rq;
	struct ib_uobject *uobj = pd->uobject;
	struct ib_ucontext *ucontext = uobj->context;
	struct mlx5_ib_ucontext *mucontext = to_mucontext(ucontext);
	int err;
	u32 tdn = mucontext->tdn;

	if (qp->sq.wqe_cnt) {
		err = create_raw_packet_qp_tis(dev, sq, tdn);
		if (err)
			return err;

		err = create_raw_packet_qp_sq(dev, sq, in, pd);
		if (err)
			goto err_destroy_tis;

		sq->base.container_mibqp = qp;
	}

	if (qp->rq.wqe_cnt) {
		rq->base.container_mibqp = qp;

		err = create_raw_packet_qp_rq(dev, rq, in, inlen);
		if (err)
			goto err_destroy_sq;


		err = create_raw_packet_qp_tir(dev, rq, tdn);
		if (err)
			goto err_destroy_rq;
	}

	qp->trans_qp.base.mqp.qpn = qp->sq.wqe_cnt ? sq->base.mqp.qpn :
						     rq->base.mqp.qpn;

	return 0;

err_destroy_rq:
	destroy_raw_packet_qp_rq(dev, rq);
err_destroy_sq:
	if (!qp->sq.wqe_cnt)
		return err;
	destroy_raw_packet_qp_sq(dev, sq);
err_destroy_tis:
	destroy_raw_packet_qp_tis(dev, sq);

	return err;
}

static void destroy_raw_packet_qp(struct mlx5_ib_dev *dev,
				  struct mlx5_ib_qp *qp)
{
	struct mlx5_ib_raw_packet_qp *raw_packet_qp = &qp->raw_packet_qp;
	struct mlx5_ib_sq *sq = &raw_packet_qp->sq;
	struct mlx5_ib_rq *rq = &raw_packet_qp->rq;

	if (qp->rq.wqe_cnt) {
		destroy_raw_packet_qp_tir(dev, rq);
		destroy_raw_packet_qp_rq(dev, rq);
	}

	if (qp->sq.wqe_cnt) {
		destroy_raw_packet_qp_sq(dev, sq);
		destroy_raw_packet_qp_tis(dev, sq);
	}
}

static void raw_packet_qp_copy_info(struct mlx5_ib_qp *qp,
				    struct mlx5_ib_raw_packet_qp *raw_packet_qp)
{
	struct mlx5_ib_sq *sq = &raw_packet_qp->sq;
	struct mlx5_ib_rq *rq = &raw_packet_qp->rq;

	sq->sq = &qp->sq;
	rq->rq = &qp->rq;
	sq->doorbell = &qp->db;
	rq->doorbell = &qp->db;
}

static void destroy_rss_raw_qp_tir(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp)
{
	mlx5_core_destroy_tir(dev->mdev, qp->rss_qp.tirn);
}

static int create_rss_raw_qp_tir(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp,
				 struct ib_pd *pd,
				 struct ib_qp_init_attr *init_attr,
				 struct ib_udata *udata)
{
	struct ib_uobject *uobj = pd->uobject;
	struct ib_ucontext *ucontext = uobj->context;
	struct mlx5_ib_ucontext *mucontext = to_mucontext(ucontext);
	struct mlx5_ib_create_qp_resp resp = {};
	int inlen;
	int err;
	u32 *in;
	void *tirc;
	void *hfso;
	u32 selected_fields = 0;
	size_t min_resp_len;
	u32 tdn = mucontext->tdn;
	struct mlx5_ib_create_qp_rss ucmd = {};
	size_t required_cmd_sz;

	if (init_attr->qp_type != IB_QPT_RAW_PACKET)
		return -EOPNOTSUPP;

	if (init_attr->create_flags || init_attr->send_cq)
		return -EINVAL;

	min_resp_len = offsetof(typeof(resp), uuar_index) + sizeof(resp.uuar_index);
	if (udata->outlen < min_resp_len)
		return -EINVAL;

	required_cmd_sz = offsetof(typeof(ucmd), reserved1) + sizeof(ucmd.reserved1);
	if (udata->inlen < required_cmd_sz) {
		mlx5_ib_dbg(dev, "invalid inlen\n");
		return -EINVAL;
	}

	if (udata->inlen > sizeof(ucmd) &&
	    !ib_is_udata_cleared(udata, sizeof(ucmd),
				 udata->inlen - sizeof(ucmd))) {
		mlx5_ib_dbg(dev, "inlen is not supported\n");
		return -EOPNOTSUPP;
	}

	if (ib_copy_from_udata(&ucmd, udata, min(sizeof(ucmd), udata->inlen))) {
		mlx5_ib_dbg(dev, "copy failed\n");
		return -EFAULT;
	}

	if (ucmd.comp_mask) {
		mlx5_ib_dbg(dev, "invalid comp mask\n");
		return -EOPNOTSUPP;
	}

	if (memchr_inv(ucmd.reserved, 0, sizeof(ucmd.reserved)) || ucmd.reserved1) {
		mlx5_ib_dbg(dev, "invalid reserved\n");
		return -EOPNOTSUPP;
	}

	err = ib_copy_to_udata(udata, &resp, min_resp_len);
	if (err) {
		mlx5_ib_dbg(dev, "copy failed\n");
		return -EINVAL;
	}

	inlen = MLX5_ST_SZ_BYTES(create_tir_in);
	in = mlx5_vzalloc(inlen);
	if (!in)
		return -ENOMEM;

	tirc = MLX5_ADDR_OF(create_tir_in, in, ctx);
	MLX5_SET(tirc, tirc, disp_type,
		 MLX5_TIRC_DISP_TYPE_INDIRECT);
	MLX5_SET(tirc, tirc, indirect_table,
		 init_attr->rwq_ind_tbl->ind_tbl_num);
	MLX5_SET(tirc, tirc, transport_domain, tdn);

	hfso = MLX5_ADDR_OF(tirc, tirc, rx_hash_field_selector_outer);
	switch (ucmd.rx_hash_function) {
	case MLX5_RX_HASH_FUNC_TOEPLITZ:
	{
		void *rss_key = MLX5_ADDR_OF(tirc, tirc, rx_hash_toeplitz_key);
		size_t len = MLX5_FLD_SZ_BYTES(tirc, rx_hash_toeplitz_key);

		if (len != ucmd.rx_key_len) {
			err = -EINVAL;
			goto err;
		}

		MLX5_SET(tirc, tirc, rx_hash_fn, MLX5_RX_HASH_FN_TOEPLITZ);
		memcpy(rss_key, ucmd.rx_hash_key, len);
		break;
	}
	default:
		err = -EOPNOTSUPP;
		goto err;
	}

	if (!ucmd.rx_hash_fields_mask) {
		/* special case when this TIR serves as steering entry without hashing */
		if (!init_attr->rwq_ind_tbl->log_ind_tbl_size)
			goto create_tir;
		err = -EINVAL;
		goto err;
	}

	if (((ucmd.rx_hash_fields_mask & MLX5_RX_HASH_SRC_IPV4) ||
	     (ucmd.rx_hash_fields_mask & MLX5_RX_HASH_DST_IPV4)) &&
	     ((ucmd.rx_hash_fields_mask & MLX5_RX_HASH_SRC_IPV6) ||
	     (ucmd.rx_hash_fields_mask & MLX5_RX_HASH_DST_IPV6))) {
		err = -EINVAL;
		goto err;
	}

	/* If none of IPV4 & IPV6 SRC/DST was set - this bit field is ignored */
	if ((ucmd.rx_hash_fields_mask & MLX5_RX_HASH_SRC_IPV4) ||
	    (ucmd.rx_hash_fields_mask & MLX5_RX_HASH_DST_IPV4))
		MLX5_SET(rx_hash_field_select, hfso, l3_prot_type,
			 MLX5_L3_PROT_TYPE_IPV4);
	else if ((ucmd.rx_hash_fields_mask & MLX5_RX_HASH_SRC_IPV6) ||
		 (ucmd.rx_hash_fields_mask & MLX5_RX_HASH_DST_IPV6))
		MLX5_SET(rx_hash_field_select, hfso, l3_prot_type,
			 MLX5_L3_PROT_TYPE_IPV6);

	if (((ucmd.rx_hash_fields_mask & MLX5_RX_HASH_SRC_PORT_TCP) ||
	     (ucmd.rx_hash_fields_mask & MLX5_RX_HASH_DST_PORT_TCP)) &&
	     ((ucmd.rx_hash_fields_mask & MLX5_RX_HASH_SRC_PORT_UDP) ||
	     (ucmd.rx_hash_fields_mask & MLX5_RX_HASH_DST_PORT_UDP))) {
		err = -EINVAL;
		goto err;
	}

	/* If none of TCP & UDP SRC/DST was set - this bit field is ignored */
	if ((ucmd.rx_hash_fields_mask & MLX5_RX_HASH_SRC_PORT_TCP) ||
	    (ucmd.rx_hash_fields_mask & MLX5_RX_HASH_DST_PORT_TCP))
		MLX5_SET(rx_hash_field_select, hfso, l4_prot_type,
			 MLX5_L4_PROT_TYPE_TCP);
	else if ((ucmd.rx_hash_fields_mask & MLX5_RX_HASH_SRC_PORT_UDP) ||
		 (ucmd.rx_hash_fields_mask & MLX5_RX_HASH_DST_PORT_UDP))
		MLX5_SET(rx_hash_field_select, hfso, l4_prot_type,
			 MLX5_L4_PROT_TYPE_UDP);

	if ((ucmd.rx_hash_fields_mask & MLX5_RX_HASH_SRC_IPV4) ||
	    (ucmd.rx_hash_fields_mask & MLX5_RX_HASH_SRC_IPV6))
		selected_fields |= MLX5_HASH_FIELD_SEL_SRC_IP;

	if ((ucmd.rx_hash_fields_mask & MLX5_RX_HASH_DST_IPV4) ||
	    (ucmd.rx_hash_fields_mask & MLX5_RX_HASH_DST_IPV6))
		selected_fields |= MLX5_HASH_FIELD_SEL_DST_IP;

	if ((ucmd.rx_hash_fields_mask & MLX5_RX_HASH_SRC_PORT_TCP) ||
	    (ucmd.rx_hash_fields_mask & MLX5_RX_HASH_SRC_PORT_UDP))
		selected_fields |= MLX5_HASH_FIELD_SEL_L4_SPORT;

	if ((ucmd.rx_hash_fields_mask & MLX5_RX_HASH_DST_PORT_TCP) ||
	    (ucmd.rx_hash_fields_mask & MLX5_RX_HASH_DST_PORT_UDP))
		selected_fields |= MLX5_HASH_FIELD_SEL_L4_DPORT;

	MLX5_SET(rx_hash_field_select, hfso, selected_fields, selected_fields);

create_tir:
	err = mlx5_core_create_tir(dev->mdev, in, inlen, &qp->rss_qp.tirn);

	if (err)
		goto err;

	kvfree(in);
	/* qpn is reserved for that QP */
	qp->trans_qp.base.mqp.qpn = 0;
	qp->flags |= MLX5_IB_QP_RSS;
	return 0;

err:
	kvfree(in);
	return err;
}

>>>>>>> v4.9.227
static int create_qp_common(struct mlx5_ib_dev *dev, struct ib_pd *pd,
			    struct ib_qp_init_attr *init_attr,
			    struct ib_udata *udata, struct mlx5_ib_qp *qp)
{
	struct mlx5_ib_resources *devr = &dev->devr;
<<<<<<< HEAD
	struct mlx5_ib_create_qp_resp resp;
	struct mlx5_create_qp_mbox_in *in;
	struct mlx5_general_caps *gen;
	struct mlx5_ib_create_qp ucmd;
	int inlen = sizeof(*in);
	int err;

	gen = &dev->mdev->caps.gen;
=======
	int inlen = MLX5_ST_SZ_BYTES(create_qp_in);
	struct mlx5_core_dev *mdev = dev->mdev;
	struct mlx5_ib_create_qp_resp resp;
	struct mlx5_ib_cq *send_cq;
	struct mlx5_ib_cq *recv_cq;
	unsigned long flags;
	u32 uidx = MLX5_IB_DEFAULT_UIDX;
	struct mlx5_ib_create_qp ucmd;
	struct mlx5_ib_qp_base *base;
	int mlx5_st;
	void *qpc;
	u32 *in;
	int err;

	base = init_attr->qp_type == IB_QPT_RAW_PACKET ?
	       &qp->raw_packet_qp.rq.base :
	       &qp->trans_qp.base;

	if (init_attr->qp_type != IB_QPT_RAW_PACKET)
		mlx5_ib_odp_create_qp(qp);

>>>>>>> v4.9.227
	mutex_init(&qp->mutex);
	spin_lock_init(&qp->sq.lock);
	spin_lock_init(&qp->rq.lock);

<<<<<<< HEAD
	if (init_attr->create_flags & IB_QP_CREATE_BLOCK_MULTICAST_LOOPBACK) {
		if (!(gen->flags & MLX5_DEV_CAP_FLAG_BLOCK_MCAST)) {
=======
	mlx5_st = to_mlx5_st(init_attr->qp_type);
	if (mlx5_st < 0)
		return -EINVAL;

	if (init_attr->rwq_ind_tbl) {
		if (!udata)
			return -ENOSYS;

		err = create_rss_raw_qp_tir(dev, qp, pd, init_attr, udata);
		return err;
	}

	if (init_attr->create_flags & IB_QP_CREATE_BLOCK_MULTICAST_LOOPBACK) {
		if (!MLX5_CAP_GEN(mdev, block_lb_mc)) {
>>>>>>> v4.9.227
			mlx5_ib_dbg(dev, "block multicast loopback isn't supported\n");
			return -EINVAL;
		} else {
			qp->flags |= MLX5_IB_QP_BLOCK_MULTICAST_LOOPBACK;
		}
	}

<<<<<<< HEAD
=======
	if (init_attr->create_flags &
			(IB_QP_CREATE_CROSS_CHANNEL |
			 IB_QP_CREATE_MANAGED_SEND |
			 IB_QP_CREATE_MANAGED_RECV)) {
		if (!MLX5_CAP_GEN(mdev, cd)) {
			mlx5_ib_dbg(dev, "cross-channel isn't supported\n");
			return -EINVAL;
		}
		if (init_attr->create_flags & IB_QP_CREATE_CROSS_CHANNEL)
			qp->flags |= MLX5_IB_QP_CROSS_CHANNEL;
		if (init_attr->create_flags & IB_QP_CREATE_MANAGED_SEND)
			qp->flags |= MLX5_IB_QP_MANAGED_SEND;
		if (init_attr->create_flags & IB_QP_CREATE_MANAGED_RECV)
			qp->flags |= MLX5_IB_QP_MANAGED_RECV;
	}

	if (init_attr->qp_type == IB_QPT_UD &&
	    (init_attr->create_flags & IB_QP_CREATE_IPOIB_UD_LSO))
		if (!MLX5_CAP_GEN(mdev, ipoib_basic_offloads)) {
			mlx5_ib_dbg(dev, "ipoib UD lso qp isn't supported\n");
			return -EOPNOTSUPP;
		}

	if (init_attr->create_flags & IB_QP_CREATE_SCATTER_FCS) {
		if (init_attr->qp_type != IB_QPT_RAW_PACKET) {
			mlx5_ib_dbg(dev, "Scatter FCS is supported only for Raw Packet QPs");
			return -EOPNOTSUPP;
		}
		if (!MLX5_CAP_GEN(dev->mdev, eth_net_offloads) ||
		    !MLX5_CAP_ETH(dev->mdev, scatter_fcs)) {
			mlx5_ib_dbg(dev, "Scatter FCS isn't supported\n");
			return -EOPNOTSUPP;
		}
		qp->flags |= MLX5_IB_QP_CAP_SCATTER_FCS;
	}

>>>>>>> v4.9.227
	if (init_attr->sq_sig_type == IB_SIGNAL_ALL_WR)
		qp->sq_signal_bits = MLX5_WQE_CTRL_CQ_UPDATE;

	if (pd && pd->uobject) {
		if (ib_copy_from_udata(&ucmd, udata, sizeof(ucmd))) {
			mlx5_ib_dbg(dev, "copy failed\n");
			return -EFAULT;
		}

<<<<<<< HEAD
=======
		err = get_qp_user_index(to_mucontext(pd->uobject->context),
					&ucmd, udata->inlen, &uidx);
		if (err)
			return err;

>>>>>>> v4.9.227
		qp->wq_sig = !!(ucmd.flags & MLX5_QP_FLAG_SIGNATURE);
		qp->scat_cqe = !!(ucmd.flags & MLX5_QP_FLAG_SCATTER_CQE);
	} else {
		qp->wq_sig = !!wq_signature;
	}

	qp->has_rq = qp_has_rq(init_attr);
	err = set_rq_size(dev, &init_attr->cap, qp->has_rq,
			  qp, (pd && pd->uobject) ? &ucmd : NULL);
	if (err) {
		mlx5_ib_dbg(dev, "err %d\n", err);
		return err;
	}

	if (pd) {
		if (pd->uobject) {
<<<<<<< HEAD
=======
			__u32 max_wqes =
				1 << MLX5_CAP_GEN(mdev, log_max_qp_sz);
>>>>>>> v4.9.227
			mlx5_ib_dbg(dev, "requested sq_wqe_count (%d)\n", ucmd.sq_wqe_count);
			if (ucmd.rq_wqe_shift != qp->rq.wqe_shift ||
			    ucmd.rq_wqe_count != qp->rq.wqe_cnt) {
				mlx5_ib_dbg(dev, "invalid rq params\n");
				return -EINVAL;
			}
<<<<<<< HEAD
			if (ucmd.sq_wqe_count > gen->max_wqes) {
				mlx5_ib_dbg(dev, "requested sq_wqe_count (%d) > max allowed (%d)\n",
					    ucmd.sq_wqe_count, gen->max_wqes);
				return -EINVAL;
			}
			err = create_user_qp(dev, pd, qp, udata, &in, &resp, &inlen);
			if (err)
				mlx5_ib_dbg(dev, "err %d\n", err);
		} else {
			err = create_kernel_qp(dev, init_attr, qp, &in, &inlen);
			if (err)
				mlx5_ib_dbg(dev, "err %d\n", err);
			else
				qp->pa_lkey = to_mpd(pd)->pa_lkey;
=======
			if (ucmd.sq_wqe_count > max_wqes) {
				mlx5_ib_dbg(dev, "requested sq_wqe_count (%d) > max allowed (%d)\n",
					    ucmd.sq_wqe_count, max_wqes);
				return -EINVAL;
			}
			if (init_attr->create_flags &
			    mlx5_ib_create_qp_sqpn_qp1()) {
				mlx5_ib_dbg(dev, "user-space is not allowed to create UD QPs spoofing as QP1\n");
				return -EINVAL;
			}
			err = create_user_qp(dev, pd, qp, udata, init_attr, &in,
					     &resp, &inlen, base);
			if (err)
				mlx5_ib_dbg(dev, "err %d\n", err);
		} else {
			err = create_kernel_qp(dev, init_attr, qp, &in, &inlen,
					       base);
			if (err)
				mlx5_ib_dbg(dev, "err %d\n", err);
>>>>>>> v4.9.227
		}

		if (err)
			return err;
	} else {
<<<<<<< HEAD
		in = mlx5_vzalloc(sizeof(*in));
=======
		in = mlx5_vzalloc(inlen);
>>>>>>> v4.9.227
		if (!in)
			return -ENOMEM;

		qp->create_type = MLX5_QP_EMPTY;
	}

	if (is_sqp(init_attr->qp_type))
		qp->port = init_attr->port_num;

<<<<<<< HEAD
	in->ctx.flags = cpu_to_be32(to_mlx5_st(init_attr->qp_type) << 16 |
				    MLX5_QP_PM_MIGRATED << 11);

	if (init_attr->qp_type != MLX5_IB_QPT_REG_UMR)
		in->ctx.flags_pd = cpu_to_be32(to_mpd(pd ? pd : devr->p0)->pdn);
	else
		in->ctx.flags_pd = cpu_to_be32(MLX5_QP_LAT_SENSITIVE);

	if (qp->wq_sig)
		in->ctx.flags_pd |= cpu_to_be32(MLX5_QP_ENABLE_SIG);

	if (qp->flags & MLX5_IB_QP_BLOCK_MULTICAST_LOOPBACK)
		in->ctx.flags_pd |= cpu_to_be32(MLX5_QP_BLOCK_MCAST);
=======
	qpc = MLX5_ADDR_OF(create_qp_in, in, qpc);

	MLX5_SET(qpc, qpc, st, mlx5_st);
	MLX5_SET(qpc, qpc, pm_state, MLX5_QP_PM_MIGRATED);

	if (init_attr->qp_type != MLX5_IB_QPT_REG_UMR)
		MLX5_SET(qpc, qpc, pd, to_mpd(pd ? pd : devr->p0)->pdn);
	else
		MLX5_SET(qpc, qpc, latency_sensitive, 1);


	if (qp->wq_sig)
		MLX5_SET(qpc, qpc, wq_signature, 1);

	if (qp->flags & MLX5_IB_QP_BLOCK_MULTICAST_LOOPBACK)
		MLX5_SET(qpc, qpc, block_lb_mc, 1);

	if (qp->flags & MLX5_IB_QP_CROSS_CHANNEL)
		MLX5_SET(qpc, qpc, cd_master, 1);
	if (qp->flags & MLX5_IB_QP_MANAGED_SEND)
		MLX5_SET(qpc, qpc, cd_slave_send, 1);
	if (qp->flags & MLX5_IB_QP_MANAGED_RECV)
		MLX5_SET(qpc, qpc, cd_slave_receive, 1);
>>>>>>> v4.9.227

	if (qp->scat_cqe && is_connected(init_attr->qp_type)) {
		int rcqe_sz;
		int scqe_sz;

		rcqe_sz = mlx5_ib_get_cqe_size(dev, init_attr->recv_cq);
		scqe_sz = mlx5_ib_get_cqe_size(dev, init_attr->send_cq);

		if (rcqe_sz == 128)
<<<<<<< HEAD
			in->ctx.cs_res = MLX5_RES_SCAT_DATA64_CQE;
		else
			in->ctx.cs_res = MLX5_RES_SCAT_DATA32_CQE;

		if (init_attr->sq_sig_type == IB_SIGNAL_ALL_WR) {
			if (scqe_sz == 128)
				in->ctx.cs_req = MLX5_REQ_SCAT_DATA64_CQE;
			else
				in->ctx.cs_req = MLX5_REQ_SCAT_DATA32_CQE;
=======
			MLX5_SET(qpc, qpc, cs_res, MLX5_RES_SCAT_DATA64_CQE);
		else
			MLX5_SET(qpc, qpc, cs_res, MLX5_RES_SCAT_DATA32_CQE);

		if (init_attr->sq_sig_type == IB_SIGNAL_ALL_WR) {
			if (scqe_sz == 128)
				MLX5_SET(qpc, qpc, cs_req, MLX5_REQ_SCAT_DATA64_CQE);
			else
				MLX5_SET(qpc, qpc, cs_req, MLX5_REQ_SCAT_DATA32_CQE);
>>>>>>> v4.9.227
		}
	}

	if (qp->rq.wqe_cnt) {
<<<<<<< HEAD
		in->ctx.rq_size_stride = (qp->rq.wqe_shift - 4);
		in->ctx.rq_size_stride |= ilog2(qp->rq.wqe_cnt) << 3;
	}

	in->ctx.rq_type_srqn = get_rx_type(qp, init_attr);

	if (qp->sq.wqe_cnt)
		in->ctx.sq_crq_size |= cpu_to_be16(ilog2(qp->sq.wqe_cnt) << 11);
	else
		in->ctx.sq_crq_size |= cpu_to_be16(0x8000);
=======
		MLX5_SET(qpc, qpc, log_rq_stride, qp->rq.wqe_shift - 4);
		MLX5_SET(qpc, qpc, log_rq_size, ilog2(qp->rq.wqe_cnt));
	}

	MLX5_SET(qpc, qpc, rq_type, get_rx_type(qp, init_attr));

	if (qp->sq.wqe_cnt)
		MLX5_SET(qpc, qpc, log_sq_size, ilog2(qp->sq.wqe_cnt));
	else
		MLX5_SET(qpc, qpc, no_sq, 1);
>>>>>>> v4.9.227

	/* Set default resources */
	switch (init_attr->qp_type) {
	case IB_QPT_XRC_TGT:
<<<<<<< HEAD
		in->ctx.cqn_recv = cpu_to_be32(to_mcq(devr->c0)->mcq.cqn);
		in->ctx.cqn_send = cpu_to_be32(to_mcq(devr->c0)->mcq.cqn);
		in->ctx.rq_type_srqn |= cpu_to_be32(to_msrq(devr->s0)->msrq.srqn);
		in->ctx.xrcd = cpu_to_be32(to_mxrcd(init_attr->xrcd)->xrcdn);
		break;
	case IB_QPT_XRC_INI:
		in->ctx.cqn_recv = cpu_to_be32(to_mcq(devr->c0)->mcq.cqn);
		in->ctx.xrcd = cpu_to_be32(to_mxrcd(devr->x1)->xrcdn);
		in->ctx.rq_type_srqn |= cpu_to_be32(to_msrq(devr->s0)->msrq.srqn);
		break;
	default:
		if (init_attr->srq) {
			in->ctx.xrcd = cpu_to_be32(to_mxrcd(devr->x0)->xrcdn);
			in->ctx.rq_type_srqn |= cpu_to_be32(to_msrq(init_attr->srq)->msrq.srqn);
		} else {
			in->ctx.xrcd = cpu_to_be32(to_mxrcd(devr->x1)->xrcdn);
			in->ctx.rq_type_srqn |= cpu_to_be32(to_msrq(devr->s0)->msrq.srqn);
=======
		MLX5_SET(qpc, qpc, cqn_rcv, to_mcq(devr->c0)->mcq.cqn);
		MLX5_SET(qpc, qpc, cqn_snd, to_mcq(devr->c0)->mcq.cqn);
		MLX5_SET(qpc, qpc, srqn_rmpn_xrqn, to_msrq(devr->s0)->msrq.srqn);
		MLX5_SET(qpc, qpc, xrcd, to_mxrcd(init_attr->xrcd)->xrcdn);
		break;
	case IB_QPT_XRC_INI:
		MLX5_SET(qpc, qpc, cqn_rcv, to_mcq(devr->c0)->mcq.cqn);
		MLX5_SET(qpc, qpc, xrcd, to_mxrcd(devr->x1)->xrcdn);
		MLX5_SET(qpc, qpc, srqn_rmpn_xrqn, to_msrq(devr->s0)->msrq.srqn);
		break;
	default:
		if (init_attr->srq) {
			MLX5_SET(qpc, qpc, xrcd, to_mxrcd(devr->x0)->xrcdn);
			MLX5_SET(qpc, qpc, srqn_rmpn_xrqn, to_msrq(init_attr->srq)->msrq.srqn);
		} else {
			MLX5_SET(qpc, qpc, xrcd, to_mxrcd(devr->x1)->xrcdn);
			MLX5_SET(qpc, qpc, srqn_rmpn_xrqn, to_msrq(devr->s1)->msrq.srqn);
>>>>>>> v4.9.227
		}
	}

	if (init_attr->send_cq)
<<<<<<< HEAD
		in->ctx.cqn_send = cpu_to_be32(to_mcq(init_attr->send_cq)->mcq.cqn);

	if (init_attr->recv_cq)
		in->ctx.cqn_recv = cpu_to_be32(to_mcq(init_attr->recv_cq)->mcq.cqn);

	in->ctx.db_rec_addr = cpu_to_be64(qp->db.dma);

	err = mlx5_core_create_qp(dev->mdev, &qp->mqp, in, inlen);
=======
		MLX5_SET(qpc, qpc, cqn_snd, to_mcq(init_attr->send_cq)->mcq.cqn);

	if (init_attr->recv_cq)
		MLX5_SET(qpc, qpc, cqn_rcv, to_mcq(init_attr->recv_cq)->mcq.cqn);

	MLX5_SET64(qpc, qpc, dbr_addr, qp->db.dma);

	/* 0xffffff means we ask to work with cqe version 0 */
	if (MLX5_CAP_GEN(mdev, cqe_version) == MLX5_CQE_VERSION_V1)
		MLX5_SET(qpc, qpc, user_index, uidx);

	/* we use IB_QP_CREATE_IPOIB_UD_LSO to indicates ipoib qp */
	if (init_attr->qp_type == IB_QPT_UD &&
	    (init_attr->create_flags & IB_QP_CREATE_IPOIB_UD_LSO)) {
		MLX5_SET(qpc, qpc, ulp_stateless_offload_mode, 1);
		qp->flags |= MLX5_IB_QP_LSO;
	}

	if (inlen < 0) {
		err = -EINVAL;
		goto err;
	}

	if (init_attr->qp_type == IB_QPT_RAW_PACKET) {
		qp->raw_packet_qp.sq.ubuffer.buf_addr = ucmd.sq_buf_addr;
		raw_packet_qp_copy_info(qp, &qp->raw_packet_qp);
		err = create_raw_packet_qp(dev, qp, in, inlen, pd);
	} else {
		err = mlx5_core_create_qp(dev->mdev, &base->mqp, in, inlen);
	}

>>>>>>> v4.9.227
	if (err) {
		mlx5_ib_dbg(dev, "create qp failed\n");
		goto err_create;
	}

<<<<<<< HEAD
	mlx5_vfree(in);
	/* Hardware wants QPN written in big-endian order (after
	 * shifting) for send doorbell.  Precompute this value to save
	 * a little bit when posting sends.
	 */
	qp->doorbell_qpn = swab32(qp->mqp.qpn << 8);

	qp->mqp.event = mlx5_ib_qp_event;
=======
	kvfree(in);

	base->container_mibqp = qp;
	base->mqp.event = mlx5_ib_qp_event;

	get_cqs(init_attr->qp_type, init_attr->send_cq, init_attr->recv_cq,
		&send_cq, &recv_cq);
	spin_lock_irqsave(&dev->reset_flow_resource_lock, flags);
	mlx5_ib_lock_cqs(send_cq, recv_cq);
	/* Maintain device to QPs access, needed for further handling via reset
	 * flow
	 */
	list_add_tail(&qp->qps_list, &dev->qp_list);
	/* Maintain CQ to QPs access, needed for further handling via reset flow
	 */
	if (send_cq)
		list_add_tail(&qp->cq_send_list, &send_cq->list_send_qp);
	if (recv_cq)
		list_add_tail(&qp->cq_recv_list, &recv_cq->list_recv_qp);
	mlx5_ib_unlock_cqs(send_cq, recv_cq);
	spin_unlock_irqrestore(&dev->reset_flow_resource_lock, flags);
>>>>>>> v4.9.227

	return 0;

err_create:
	if (qp->create_type == MLX5_QP_USER)
<<<<<<< HEAD
		destroy_qp_user(pd, qp);
	else if (qp->create_type == MLX5_QP_KERNEL)
		destroy_qp_kernel(dev, qp);

	mlx5_vfree(in);
=======
		destroy_qp_user(pd, qp, base);
	else if (qp->create_type == MLX5_QP_KERNEL)
		destroy_qp_kernel(dev, qp);

err:
	kvfree(in);
>>>>>>> v4.9.227
	return err;
}

static void mlx5_ib_lock_cqs(struct mlx5_ib_cq *send_cq, struct mlx5_ib_cq *recv_cq)
	__acquires(&send_cq->lock) __acquires(&recv_cq->lock)
{
	if (send_cq) {
		if (recv_cq) {
			if (send_cq->mcq.cqn < recv_cq->mcq.cqn)  {
<<<<<<< HEAD
				spin_lock_irq(&send_cq->lock);
				spin_lock_nested(&recv_cq->lock,
						 SINGLE_DEPTH_NESTING);
			} else if (send_cq->mcq.cqn == recv_cq->mcq.cqn) {
				spin_lock_irq(&send_cq->lock);
				__acquire(&recv_cq->lock);
			} else {
				spin_lock_irq(&recv_cq->lock);
=======
				spin_lock(&send_cq->lock);
				spin_lock_nested(&recv_cq->lock,
						 SINGLE_DEPTH_NESTING);
			} else if (send_cq->mcq.cqn == recv_cq->mcq.cqn) {
				spin_lock(&send_cq->lock);
				__acquire(&recv_cq->lock);
			} else {
				spin_lock(&recv_cq->lock);
>>>>>>> v4.9.227
				spin_lock_nested(&send_cq->lock,
						 SINGLE_DEPTH_NESTING);
			}
		} else {
<<<<<<< HEAD
			spin_lock_irq(&send_cq->lock);
		}
	} else if (recv_cq) {
		spin_lock_irq(&recv_cq->lock);
=======
			spin_lock(&send_cq->lock);
			__acquire(&recv_cq->lock);
		}
	} else if (recv_cq) {
		spin_lock(&recv_cq->lock);
		__acquire(&send_cq->lock);
	} else {
		__acquire(&send_cq->lock);
		__acquire(&recv_cq->lock);
>>>>>>> v4.9.227
	}
}

static void mlx5_ib_unlock_cqs(struct mlx5_ib_cq *send_cq, struct mlx5_ib_cq *recv_cq)
	__releases(&send_cq->lock) __releases(&recv_cq->lock)
{
	if (send_cq) {
		if (recv_cq) {
			if (send_cq->mcq.cqn < recv_cq->mcq.cqn)  {
				spin_unlock(&recv_cq->lock);
<<<<<<< HEAD
				spin_unlock_irq(&send_cq->lock);
			} else if (send_cq->mcq.cqn == recv_cq->mcq.cqn) {
				__release(&recv_cq->lock);
				spin_unlock_irq(&send_cq->lock);
			} else {
				spin_unlock(&send_cq->lock);
				spin_unlock_irq(&recv_cq->lock);
			}
		} else {
			spin_unlock_irq(&send_cq->lock);
		}
	} else if (recv_cq) {
		spin_unlock_irq(&recv_cq->lock);
=======
				spin_unlock(&send_cq->lock);
			} else if (send_cq->mcq.cqn == recv_cq->mcq.cqn) {
				__release(&recv_cq->lock);
				spin_unlock(&send_cq->lock);
			} else {
				spin_unlock(&send_cq->lock);
				spin_unlock(&recv_cq->lock);
			}
		} else {
			__release(&recv_cq->lock);
			spin_unlock(&send_cq->lock);
		}
	} else if (recv_cq) {
		__release(&send_cq->lock);
		spin_unlock(&recv_cq->lock);
	} else {
		__release(&recv_cq->lock);
		__release(&send_cq->lock);
>>>>>>> v4.9.227
	}
}

static struct mlx5_ib_pd *get_pd(struct mlx5_ib_qp *qp)
{
	return to_mpd(qp->ibqp.pd);
}

<<<<<<< HEAD
static void get_cqs(struct mlx5_ib_qp *qp,
		    struct mlx5_ib_cq **send_cq, struct mlx5_ib_cq **recv_cq)
{
	switch (qp->ibqp.qp_type) {
=======
static void get_cqs(enum ib_qp_type qp_type,
		    struct ib_cq *ib_send_cq, struct ib_cq *ib_recv_cq,
		    struct mlx5_ib_cq **send_cq, struct mlx5_ib_cq **recv_cq)
{
	switch (qp_type) {
>>>>>>> v4.9.227
	case IB_QPT_XRC_TGT:
		*send_cq = NULL;
		*recv_cq = NULL;
		break;
	case MLX5_IB_QPT_REG_UMR:
	case IB_QPT_XRC_INI:
<<<<<<< HEAD
		*send_cq = to_mcq(qp->ibqp.send_cq);
=======
		*send_cq = ib_send_cq ? to_mcq(ib_send_cq) : NULL;
>>>>>>> v4.9.227
		*recv_cq = NULL;
		break;

	case IB_QPT_SMI:
<<<<<<< HEAD
	case IB_QPT_GSI:
=======
	case MLX5_IB_QPT_HW_GSI:
>>>>>>> v4.9.227
	case IB_QPT_RC:
	case IB_QPT_UC:
	case IB_QPT_UD:
	case IB_QPT_RAW_IPV6:
	case IB_QPT_RAW_ETHERTYPE:
<<<<<<< HEAD
		*send_cq = to_mcq(qp->ibqp.send_cq);
		*recv_cq = to_mcq(qp->ibqp.recv_cq);
		break;

	case IB_QPT_RAW_PACKET:
=======
	case IB_QPT_RAW_PACKET:
		*send_cq = ib_send_cq ? to_mcq(ib_send_cq) : NULL;
		*recv_cq = ib_recv_cq ? to_mcq(ib_recv_cq) : NULL;
		break;

>>>>>>> v4.9.227
	case IB_QPT_MAX:
	default:
		*send_cq = NULL;
		*recv_cq = NULL;
		break;
	}
}

<<<<<<< HEAD
static void destroy_qp_common(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp)
{
	struct mlx5_ib_cq *send_cq, *recv_cq;
	struct mlx5_modify_qp_mbox_in *in;
	int err;

	in = kzalloc(sizeof(*in), GFP_KERNEL);
	if (!in)
		return;
	if (qp->state != IB_QPS_RESET)
		if (mlx5_core_qp_modify(dev->mdev, to_mlx5_state(qp->state),
					MLX5_QP_STATE_RST, in, sizeof(*in), &qp->mqp))
			mlx5_ib_warn(dev, "mlx5_ib: modify QP %06x to RESET failed\n",
				     qp->mqp.qpn);

	get_cqs(qp, &send_cq, &recv_cq);

	if (qp->create_type == MLX5_QP_KERNEL) {
		mlx5_ib_lock_cqs(send_cq, recv_cq);
		__mlx5_ib_cq_clean(recv_cq, qp->mqp.qpn,
				   qp->ibqp.srq ? to_msrq(qp->ibqp.srq) : NULL);
		if (send_cq != recv_cq)
			__mlx5_ib_cq_clean(send_cq, qp->mqp.qpn, NULL);
		mlx5_ib_unlock_cqs(send_cq, recv_cq);
	}

	err = mlx5_core_destroy_qp(dev->mdev, &qp->mqp);
	if (err)
		mlx5_ib_warn(dev, "failed to destroy QP 0x%x\n", qp->mqp.qpn);
	kfree(in);

=======
static int modify_raw_packet_qp(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp,
				const struct mlx5_modify_raw_qp_param *raw_qp_param,
				u8 lag_tx_affinity);

static void destroy_qp_common(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp)
{
	struct mlx5_ib_cq *send_cq, *recv_cq;
	struct mlx5_ib_qp_base *base = &qp->trans_qp.base;
	unsigned long flags;
	int err;

	if (qp->ibqp.rwq_ind_tbl) {
		destroy_rss_raw_qp_tir(dev, qp);
		return;
	}

	base = qp->ibqp.qp_type == IB_QPT_RAW_PACKET ?
	       &qp->raw_packet_qp.rq.base :
	       &qp->trans_qp.base;

	if (qp->state != IB_QPS_RESET) {
		if (qp->ibqp.qp_type != IB_QPT_RAW_PACKET) {
			mlx5_ib_qp_disable_pagefaults(qp);
			err = mlx5_core_qp_modify(dev->mdev,
						  MLX5_CMD_OP_2RST_QP, 0,
						  NULL, &base->mqp);
		} else {
			struct mlx5_modify_raw_qp_param raw_qp_param = {
				.operation = MLX5_CMD_OP_2RST_QP
			};

			err = modify_raw_packet_qp(dev, qp, &raw_qp_param, 0);
		}
		if (err)
			mlx5_ib_warn(dev, "mlx5_ib: modify QP 0x%06x to RESET failed\n",
				     base->mqp.qpn);
	}

	get_cqs(qp->ibqp.qp_type, qp->ibqp.send_cq, qp->ibqp.recv_cq,
		&send_cq, &recv_cq);

	spin_lock_irqsave(&dev->reset_flow_resource_lock, flags);
	mlx5_ib_lock_cqs(send_cq, recv_cq);
	/* del from lists under both locks above to protect reset flow paths */
	list_del(&qp->qps_list);
	if (send_cq)
		list_del(&qp->cq_send_list);

	if (recv_cq)
		list_del(&qp->cq_recv_list);

	if (qp->create_type == MLX5_QP_KERNEL) {
		__mlx5_ib_cq_clean(recv_cq, base->mqp.qpn,
				   qp->ibqp.srq ? to_msrq(qp->ibqp.srq) : NULL);
		if (send_cq != recv_cq)
			__mlx5_ib_cq_clean(send_cq, base->mqp.qpn,
					   NULL);
	}
	mlx5_ib_unlock_cqs(send_cq, recv_cq);
	spin_unlock_irqrestore(&dev->reset_flow_resource_lock, flags);

	if (qp->ibqp.qp_type == IB_QPT_RAW_PACKET) {
		destroy_raw_packet_qp(dev, qp);
	} else {
		err = mlx5_core_destroy_qp(dev->mdev, &base->mqp);
		if (err)
			mlx5_ib_warn(dev, "failed to destroy QP 0x%x\n",
				     base->mqp.qpn);
	}
>>>>>>> v4.9.227

	if (qp->create_type == MLX5_QP_KERNEL)
		destroy_qp_kernel(dev, qp);
	else if (qp->create_type == MLX5_QP_USER)
<<<<<<< HEAD
		destroy_qp_user(&get_pd(qp)->ibpd, qp);
=======
		destroy_qp_user(&get_pd(qp)->ibpd, qp, base);
>>>>>>> v4.9.227
}

static const char *ib_qp_type_str(enum ib_qp_type type)
{
	switch (type) {
	case IB_QPT_SMI:
		return "IB_QPT_SMI";
	case IB_QPT_GSI:
		return "IB_QPT_GSI";
	case IB_QPT_RC:
		return "IB_QPT_RC";
	case IB_QPT_UC:
		return "IB_QPT_UC";
	case IB_QPT_UD:
		return "IB_QPT_UD";
	case IB_QPT_RAW_IPV6:
		return "IB_QPT_RAW_IPV6";
	case IB_QPT_RAW_ETHERTYPE:
		return "IB_QPT_RAW_ETHERTYPE";
	case IB_QPT_XRC_INI:
		return "IB_QPT_XRC_INI";
	case IB_QPT_XRC_TGT:
		return "IB_QPT_XRC_TGT";
	case IB_QPT_RAW_PACKET:
		return "IB_QPT_RAW_PACKET";
	case MLX5_IB_QPT_REG_UMR:
		return "MLX5_IB_QPT_REG_UMR";
	case IB_QPT_MAX:
	default:
		return "Invalid QP type";
	}
}

struct ib_qp *mlx5_ib_create_qp(struct ib_pd *pd,
				struct ib_qp_init_attr *init_attr,
				struct ib_udata *udata)
{
<<<<<<< HEAD
	struct mlx5_general_caps *gen;
=======
>>>>>>> v4.9.227
	struct mlx5_ib_dev *dev;
	struct mlx5_ib_qp *qp;
	u16 xrcdn = 0;
	int err;

	if (pd) {
		dev = to_mdev(pd->device);
<<<<<<< HEAD
=======

		if (init_attr->qp_type == IB_QPT_RAW_PACKET) {
			if (!pd->uobject) {
				mlx5_ib_dbg(dev, "Raw Packet QP is not supported for kernel consumers\n");
				return ERR_PTR(-EINVAL);
			} else if (!to_mucontext(pd->uobject->context)->cqe_version) {
				mlx5_ib_dbg(dev, "Raw Packet QP is only supported for CQE version > 0\n");
				return ERR_PTR(-EINVAL);
			}
		}
>>>>>>> v4.9.227
	} else {
		/* being cautious here */
		if (init_attr->qp_type != IB_QPT_XRC_TGT &&
		    init_attr->qp_type != MLX5_IB_QPT_REG_UMR) {
			pr_warn("%s: no PD for transport %s\n", __func__,
				ib_qp_type_str(init_attr->qp_type));
			return ERR_PTR(-EINVAL);
		}
		dev = to_mdev(to_mxrcd(init_attr->xrcd)->ibxrcd.device);
	}
<<<<<<< HEAD
	gen = &dev->mdev->caps.gen;
=======
>>>>>>> v4.9.227

	switch (init_attr->qp_type) {
	case IB_QPT_XRC_TGT:
	case IB_QPT_XRC_INI:
<<<<<<< HEAD
		if (!(gen->flags & MLX5_DEV_CAP_FLAG_XRC)) {
=======
		if (!MLX5_CAP_GEN(dev->mdev, xrc)) {
>>>>>>> v4.9.227
			mlx5_ib_dbg(dev, "XRC not supported\n");
			return ERR_PTR(-ENOSYS);
		}
		init_attr->recv_cq = NULL;
		if (init_attr->qp_type == IB_QPT_XRC_TGT) {
			xrcdn = to_mxrcd(init_attr->xrcd)->xrcdn;
			init_attr->send_cq = NULL;
		}

		/* fall through */
<<<<<<< HEAD
=======
	case IB_QPT_RAW_PACKET:
>>>>>>> v4.9.227
	case IB_QPT_RC:
	case IB_QPT_UC:
	case IB_QPT_UD:
	case IB_QPT_SMI:
<<<<<<< HEAD
	case IB_QPT_GSI:
=======
	case MLX5_IB_QPT_HW_GSI:
>>>>>>> v4.9.227
	case MLX5_IB_QPT_REG_UMR:
		qp = kzalloc(sizeof(*qp), GFP_KERNEL);
		if (!qp)
			return ERR_PTR(-ENOMEM);

		err = create_qp_common(dev, pd, init_attr, udata, qp);
		if (err) {
			mlx5_ib_dbg(dev, "create_qp_common failed\n");
			kfree(qp);
			return ERR_PTR(err);
		}

		if (is_qp0(init_attr->qp_type))
			qp->ibqp.qp_num = 0;
		else if (is_qp1(init_attr->qp_type))
			qp->ibqp.qp_num = 1;
		else
<<<<<<< HEAD
			qp->ibqp.qp_num = qp->mqp.qpn;

		mlx5_ib_dbg(dev, "ib qpnum 0x%x, mlx qpn 0x%x, rcqn 0x%x, scqn 0x%x\n",
			    qp->ibqp.qp_num, qp->mqp.qpn, to_mcq(init_attr->recv_cq)->mcq.cqn,
			    to_mcq(init_attr->send_cq)->mcq.cqn);

		qp->xrcdn = xrcdn;

		break;

	case IB_QPT_RAW_IPV6:
	case IB_QPT_RAW_ETHERTYPE:
	case IB_QPT_RAW_PACKET:
=======
			qp->ibqp.qp_num = qp->trans_qp.base.mqp.qpn;

		mlx5_ib_dbg(dev, "ib qpnum 0x%x, mlx qpn 0x%x, rcqn 0x%x, scqn 0x%x\n",
			    qp->ibqp.qp_num, qp->trans_qp.base.mqp.qpn,
			    init_attr->recv_cq ? to_mcq(init_attr->recv_cq)->mcq.cqn : -1,
			    init_attr->send_cq ? to_mcq(init_attr->send_cq)->mcq.cqn : -1);

		qp->trans_qp.xrcdn = xrcdn;

		break;

	case IB_QPT_GSI:
		return mlx5_ib_gsi_create_qp(pd, init_attr);

	case IB_QPT_RAW_IPV6:
	case IB_QPT_RAW_ETHERTYPE:
>>>>>>> v4.9.227
	case IB_QPT_MAX:
	default:
		mlx5_ib_dbg(dev, "unsupported qp type %d\n",
			    init_attr->qp_type);
		/* Don't support raw QPs */
		return ERR_PTR(-EINVAL);
	}

	return &qp->ibqp;
}

int mlx5_ib_destroy_qp(struct ib_qp *qp)
{
	struct mlx5_ib_dev *dev = to_mdev(qp->device);
	struct mlx5_ib_qp *mqp = to_mqp(qp);

<<<<<<< HEAD
=======
	if (unlikely(qp->qp_type == IB_QPT_GSI))
		return mlx5_ib_gsi_destroy_qp(qp);

>>>>>>> v4.9.227
	destroy_qp_common(dev, mqp);

	kfree(mqp);

	return 0;
}

static __be32 to_mlx5_access_flags(struct mlx5_ib_qp *qp, const struct ib_qp_attr *attr,
				   int attr_mask)
{
	u32 hw_access_flags = 0;
	u8 dest_rd_atomic;
	u32 access_flags;

	if (attr_mask & IB_QP_MAX_DEST_RD_ATOMIC)
		dest_rd_atomic = attr->max_dest_rd_atomic;
	else
<<<<<<< HEAD
		dest_rd_atomic = qp->resp_depth;
=======
		dest_rd_atomic = qp->trans_qp.resp_depth;
>>>>>>> v4.9.227

	if (attr_mask & IB_QP_ACCESS_FLAGS)
		access_flags = attr->qp_access_flags;
	else
<<<<<<< HEAD
		access_flags = qp->atomic_rd_en;
=======
		access_flags = qp->trans_qp.atomic_rd_en;
>>>>>>> v4.9.227

	if (!dest_rd_atomic)
		access_flags &= IB_ACCESS_REMOTE_WRITE;

	if (access_flags & IB_ACCESS_REMOTE_READ)
		hw_access_flags |= MLX5_QP_BIT_RRE;
	if (access_flags & IB_ACCESS_REMOTE_ATOMIC)
		hw_access_flags |= (MLX5_QP_BIT_RAE | MLX5_ATOMIC_MODE_CX);
	if (access_flags & IB_ACCESS_REMOTE_WRITE)
		hw_access_flags |= MLX5_QP_BIT_RWE;

	return cpu_to_be32(hw_access_flags);
}

enum {
	MLX5_PATH_FLAG_FL	= 1 << 0,
	MLX5_PATH_FLAG_FREE_AR	= 1 << 1,
	MLX5_PATH_FLAG_COUNTER	= 1 << 2,
};

static int ib_rate_to_mlx5(struct mlx5_ib_dev *dev, u8 rate)
{
<<<<<<< HEAD
	struct mlx5_general_caps *gen;

	gen = &dev->mdev->caps.gen;
	if (rate == IB_RATE_PORT_CURRENT) {
		return 0;
	} else if (rate < IB_RATE_2_5_GBPS || rate > IB_RATE_300_GBPS) {
		return -EINVAL;
	} else {
		while (rate != IB_RATE_2_5_GBPS &&
		       !(1 << (rate + MLX5_STAT_RATE_OFFSET) &
			 gen->stat_rate_support))
			--rate;
	}

	return rate + MLX5_STAT_RATE_OFFSET;
}

static int mlx5_set_path(struct mlx5_ib_dev *dev, const struct ib_ah_attr *ah,
			 struct mlx5_qp_path *path, u8 port, int attr_mask,
			 u32 path_flags, const struct ib_qp_attr *attr)
{
	struct mlx5_general_caps *gen;
	int err;

	gen = &dev->mdev->caps.gen;
	path->fl = (path_flags & MLX5_PATH_FLAG_FL) ? 0x80 : 0;
	path->free_ar = (path_flags & MLX5_PATH_FLAG_FREE_AR) ? 0x80 : 0;

	if (attr_mask & IB_QP_PKEY_INDEX)
		path->pkey_index = attr->pkey_index;

	path->grh_mlid	= ah->src_path_bits & 0x7f;
	path->rlid	= cpu_to_be16(ah->dlid);

	if (ah->ah_flags & IB_AH_GRH) {
		if (ah->grh.sgid_index >= gen->port[port - 1].gid_table_len) {
			pr_err(KERN_ERR "sgid_index (%u) too large. max is %d\n",
			       ah->grh.sgid_index, gen->port[port - 1].gid_table_len);
			return -EINVAL;
		}
		path->grh_mlid |= 1 << 7;
=======
	if (rate == IB_RATE_PORT_CURRENT)
		return 0;

	if (rate < IB_RATE_2_5_GBPS || rate > IB_RATE_300_GBPS)
		return -EINVAL;

	while (rate != IB_RATE_PORT_CURRENT &&
	       !(1 << (rate + MLX5_STAT_RATE_OFFSET) &
		 MLX5_CAP_GEN(dev->mdev, stat_rate_support)))
		--rate;

	return rate ? rate + MLX5_STAT_RATE_OFFSET : rate;
}

static int modify_raw_packet_eth_prio(struct mlx5_core_dev *dev,
				      struct mlx5_ib_sq *sq, u8 sl)
{
	void *in;
	void *tisc;
	int inlen;
	int err;

	inlen = MLX5_ST_SZ_BYTES(modify_tis_in);
	in = mlx5_vzalloc(inlen);
	if (!in)
		return -ENOMEM;

	MLX5_SET(modify_tis_in, in, bitmask.prio, 1);

	tisc = MLX5_ADDR_OF(modify_tis_in, in, ctx);
	MLX5_SET(tisc, tisc, prio, ((sl & 0x7) << 1));

	err = mlx5_core_modify_tis(dev, sq->tisn, in, inlen);

	kvfree(in);

	return err;
}

static int modify_raw_packet_tx_affinity(struct mlx5_core_dev *dev,
					 struct mlx5_ib_sq *sq, u8 tx_affinity)
{
	void *in;
	void *tisc;
	int inlen;
	int err;

	inlen = MLX5_ST_SZ_BYTES(modify_tis_in);
	in = mlx5_vzalloc(inlen);
	if (!in)
		return -ENOMEM;

	MLX5_SET(modify_tis_in, in, bitmask.lag_tx_port_affinity, 1);

	tisc = MLX5_ADDR_OF(modify_tis_in, in, ctx);
	MLX5_SET(tisc, tisc, lag_tx_port_affinity, tx_affinity);

	err = mlx5_core_modify_tis(dev, sq->tisn, in, inlen);

	kvfree(in);

	return err;
}

static int mlx5_set_path(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp,
			 const struct ib_ah_attr *ah,
			 struct mlx5_qp_path *path, u8 port, int attr_mask,
			 u32 path_flags, const struct ib_qp_attr *attr,
			 bool alt)
{
	enum rdma_link_layer ll = rdma_port_get_link_layer(&dev->ib_dev, port);
	int err;
	enum ib_gid_type gid_type;

	if (attr_mask & IB_QP_PKEY_INDEX)
		path->pkey_index = cpu_to_be16(alt ? attr->alt_pkey_index :
						     attr->pkey_index);

	if (ah->ah_flags & IB_AH_GRH) {
		if (ah->grh.sgid_index >=
		    dev->mdev->port_caps[port - 1].gid_table_len) {
			pr_err("sgid_index (%u) too large. max is %d\n",
			       ah->grh.sgid_index,
			       dev->mdev->port_caps[port - 1].gid_table_len);
			return -EINVAL;
		}
	}

	if (ll == IB_LINK_LAYER_ETHERNET) {
		if (!(ah->ah_flags & IB_AH_GRH))
			return -EINVAL;
		err = mlx5_get_roce_gid_type(dev, port, ah->grh.sgid_index,
					     &gid_type);
		if (err)
			return err;
		memcpy(path->rmac, ah->dmac, sizeof(ah->dmac));
		path->udp_sport = mlx5_get_roce_udp_sport(dev, port,
							  ah->grh.sgid_index);
		path->dci_cfi_prio_sl = (ah->sl & 0x7) << 4;
		if (gid_type == IB_GID_TYPE_ROCE_UDP_ENCAP)
			path->ecn_dscp = (ah->grh.traffic_class >> 2) & 0x3f;
	} else {
		path->fl_free_ar = (path_flags & MLX5_PATH_FLAG_FL) ? 0x80 : 0;
		path->fl_free_ar |=
			(path_flags & MLX5_PATH_FLAG_FREE_AR) ? 0x40 : 0;
		path->rlid = cpu_to_be16(ah->dlid);
		path->grh_mlid = ah->src_path_bits & 0x7f;
		if (ah->ah_flags & IB_AH_GRH)
			path->grh_mlid	|= 1 << 7;
		path->dci_cfi_prio_sl = ah->sl & 0xf;
	}

	if (ah->ah_flags & IB_AH_GRH) {
>>>>>>> v4.9.227
		path->mgid_index = ah->grh.sgid_index;
		path->hop_limit  = ah->grh.hop_limit;
		path->tclass_flowlabel =
			cpu_to_be32((ah->grh.traffic_class << 20) |
				    (ah->grh.flow_label));
		memcpy(path->rgid, ah->grh.dgid.raw, 16);
	}

	err = ib_rate_to_mlx5(dev, ah->static_rate);
	if (err < 0)
		return err;
	path->static_rate = err;
	path->port = port;

	if (attr_mask & IB_QP_TIMEOUT)
<<<<<<< HEAD
		path->ackto_lt = attr->timeout << 3;

	path->sl = ah->sl & 0xf;
=======
		path->ackto_lt = (alt ? attr->alt_timeout : attr->timeout) << 3;

	if ((qp->ibqp.qp_type == IB_QPT_RAW_PACKET) && qp->sq.wqe_cnt)
		return modify_raw_packet_eth_prio(dev->mdev,
						  &qp->raw_packet_qp.sq,
						  ah->sl & 0xf);
>>>>>>> v4.9.227

	return 0;
}

static enum mlx5_qp_optpar opt_mask[MLX5_QP_NUM_STATE][MLX5_QP_NUM_STATE][MLX5_QP_ST_MAX] = {
	[MLX5_QP_STATE_INIT] = {
		[MLX5_QP_STATE_INIT] = {
			[MLX5_QP_ST_RC] = MLX5_QP_OPTPAR_RRE		|
					  MLX5_QP_OPTPAR_RAE		|
					  MLX5_QP_OPTPAR_RWE		|
					  MLX5_QP_OPTPAR_PKEY_INDEX	|
					  MLX5_QP_OPTPAR_PRI_PORT,
			[MLX5_QP_ST_UC] = MLX5_QP_OPTPAR_RWE		|
					  MLX5_QP_OPTPAR_PKEY_INDEX	|
					  MLX5_QP_OPTPAR_PRI_PORT,
			[MLX5_QP_ST_UD] = MLX5_QP_OPTPAR_PKEY_INDEX	|
					  MLX5_QP_OPTPAR_Q_KEY		|
					  MLX5_QP_OPTPAR_PRI_PORT,
<<<<<<< HEAD
=======
			[MLX5_QP_ST_XRC] = MLX5_QP_OPTPAR_RRE		|
					  MLX5_QP_OPTPAR_RAE		|
					  MLX5_QP_OPTPAR_RWE		|
					  MLX5_QP_OPTPAR_PKEY_INDEX	|
					  MLX5_QP_OPTPAR_PRI_PORT,
>>>>>>> v4.9.227
		},
		[MLX5_QP_STATE_RTR] = {
			[MLX5_QP_ST_RC] = MLX5_QP_OPTPAR_ALT_ADDR_PATH  |
					  MLX5_QP_OPTPAR_RRE            |
					  MLX5_QP_OPTPAR_RAE            |
					  MLX5_QP_OPTPAR_RWE            |
					  MLX5_QP_OPTPAR_PKEY_INDEX,
			[MLX5_QP_ST_UC] = MLX5_QP_OPTPAR_ALT_ADDR_PATH  |
					  MLX5_QP_OPTPAR_RWE            |
					  MLX5_QP_OPTPAR_PKEY_INDEX,
			[MLX5_QP_ST_UD] = MLX5_QP_OPTPAR_PKEY_INDEX     |
					  MLX5_QP_OPTPAR_Q_KEY,
			[MLX5_QP_ST_MLX] = MLX5_QP_OPTPAR_PKEY_INDEX	|
					   MLX5_QP_OPTPAR_Q_KEY,
			[MLX5_QP_ST_XRC] = MLX5_QP_OPTPAR_ALT_ADDR_PATH |
					  MLX5_QP_OPTPAR_RRE            |
					  MLX5_QP_OPTPAR_RAE            |
					  MLX5_QP_OPTPAR_RWE            |
					  MLX5_QP_OPTPAR_PKEY_INDEX,
		},
	},
	[MLX5_QP_STATE_RTR] = {
		[MLX5_QP_STATE_RTS] = {
			[MLX5_QP_ST_RC] = MLX5_QP_OPTPAR_ALT_ADDR_PATH	|
					  MLX5_QP_OPTPAR_RRE		|
					  MLX5_QP_OPTPAR_RAE		|
					  MLX5_QP_OPTPAR_RWE		|
					  MLX5_QP_OPTPAR_PM_STATE	|
					  MLX5_QP_OPTPAR_RNR_TIMEOUT,
			[MLX5_QP_ST_UC] = MLX5_QP_OPTPAR_ALT_ADDR_PATH	|
					  MLX5_QP_OPTPAR_RWE		|
					  MLX5_QP_OPTPAR_PM_STATE,
			[MLX5_QP_ST_UD] = MLX5_QP_OPTPAR_Q_KEY,
<<<<<<< HEAD
=======
			[MLX5_QP_ST_XRC] = MLX5_QP_OPTPAR_ALT_ADDR_PATH	|
					  MLX5_QP_OPTPAR_RRE		|
					  MLX5_QP_OPTPAR_RAE		|
					  MLX5_QP_OPTPAR_RWE		|
					  MLX5_QP_OPTPAR_PM_STATE	|
					  MLX5_QP_OPTPAR_RNR_TIMEOUT,
>>>>>>> v4.9.227
		},
	},
	[MLX5_QP_STATE_RTS] = {
		[MLX5_QP_STATE_RTS] = {
			[MLX5_QP_ST_RC] = MLX5_QP_OPTPAR_RRE		|
					  MLX5_QP_OPTPAR_RAE		|
					  MLX5_QP_OPTPAR_RWE		|
					  MLX5_QP_OPTPAR_RNR_TIMEOUT	|
					  MLX5_QP_OPTPAR_PM_STATE	|
					  MLX5_QP_OPTPAR_ALT_ADDR_PATH,
			[MLX5_QP_ST_UC] = MLX5_QP_OPTPAR_RWE		|
					  MLX5_QP_OPTPAR_PM_STATE	|
					  MLX5_QP_OPTPAR_ALT_ADDR_PATH,
			[MLX5_QP_ST_UD] = MLX5_QP_OPTPAR_Q_KEY		|
					  MLX5_QP_OPTPAR_SRQN		|
					  MLX5_QP_OPTPAR_CQN_RCV,
<<<<<<< HEAD
=======
			[MLX5_QP_ST_XRC] = MLX5_QP_OPTPAR_RRE		|
					  MLX5_QP_OPTPAR_RAE		|
					  MLX5_QP_OPTPAR_RWE		|
					  MLX5_QP_OPTPAR_RNR_TIMEOUT	|
					  MLX5_QP_OPTPAR_PM_STATE	|
					  MLX5_QP_OPTPAR_ALT_ADDR_PATH,
>>>>>>> v4.9.227
		},
	},
	[MLX5_QP_STATE_SQER] = {
		[MLX5_QP_STATE_RTS] = {
			[MLX5_QP_ST_UD]	 = MLX5_QP_OPTPAR_Q_KEY,
			[MLX5_QP_ST_MLX] = MLX5_QP_OPTPAR_Q_KEY,
			[MLX5_QP_ST_UC]	 = MLX5_QP_OPTPAR_RWE,
			[MLX5_QP_ST_RC]	 = MLX5_QP_OPTPAR_RNR_TIMEOUT	|
					   MLX5_QP_OPTPAR_RWE		|
					   MLX5_QP_OPTPAR_RAE		|
					   MLX5_QP_OPTPAR_RRE,
<<<<<<< HEAD
=======
			[MLX5_QP_ST_XRC]  = MLX5_QP_OPTPAR_RNR_TIMEOUT	|
					   MLX5_QP_OPTPAR_RWE		|
					   MLX5_QP_OPTPAR_RAE		|
					   MLX5_QP_OPTPAR_RRE,
>>>>>>> v4.9.227
		},
	},
};

static int ib_nr_to_mlx5_nr(int ib_mask)
{
	switch (ib_mask) {
	case IB_QP_STATE:
		return 0;
	case IB_QP_CUR_STATE:
		return 0;
	case IB_QP_EN_SQD_ASYNC_NOTIFY:
		return 0;
	case IB_QP_ACCESS_FLAGS:
		return MLX5_QP_OPTPAR_RWE | MLX5_QP_OPTPAR_RRE |
			MLX5_QP_OPTPAR_RAE;
	case IB_QP_PKEY_INDEX:
		return MLX5_QP_OPTPAR_PKEY_INDEX;
	case IB_QP_PORT:
		return MLX5_QP_OPTPAR_PRI_PORT;
	case IB_QP_QKEY:
		return MLX5_QP_OPTPAR_Q_KEY;
	case IB_QP_AV:
		return MLX5_QP_OPTPAR_PRIMARY_ADDR_PATH |
			MLX5_QP_OPTPAR_PRI_PORT;
	case IB_QP_PATH_MTU:
		return 0;
	case IB_QP_TIMEOUT:
		return MLX5_QP_OPTPAR_ACK_TIMEOUT;
	case IB_QP_RETRY_CNT:
		return MLX5_QP_OPTPAR_RETRY_COUNT;
	case IB_QP_RNR_RETRY:
		return MLX5_QP_OPTPAR_RNR_RETRY;
	case IB_QP_RQ_PSN:
		return 0;
	case IB_QP_MAX_QP_RD_ATOMIC:
		return MLX5_QP_OPTPAR_SRA_MAX;
	case IB_QP_ALT_PATH:
		return MLX5_QP_OPTPAR_ALT_ADDR_PATH;
	case IB_QP_MIN_RNR_TIMER:
		return MLX5_QP_OPTPAR_RNR_TIMEOUT;
	case IB_QP_SQ_PSN:
		return 0;
	case IB_QP_MAX_DEST_RD_ATOMIC:
		return MLX5_QP_OPTPAR_RRA_MAX | MLX5_QP_OPTPAR_RWE |
			MLX5_QP_OPTPAR_RRE | MLX5_QP_OPTPAR_RAE;
	case IB_QP_PATH_MIG_STATE:
		return MLX5_QP_OPTPAR_PM_STATE;
	case IB_QP_CAP:
		return 0;
	case IB_QP_DEST_QPN:
		return 0;
	}
	return 0;
}

static int ib_mask_to_mlx5_opt(int ib_mask)
{
	int result = 0;
	int i;

	for (i = 0; i < 8 * sizeof(int); i++) {
		if ((1 << i) & ib_mask)
			result |= ib_nr_to_mlx5_nr(1 << i);
	}

	return result;
}

<<<<<<< HEAD
=======
static int modify_raw_packet_qp_rq(struct mlx5_ib_dev *dev,
				   struct mlx5_ib_rq *rq, int new_state,
				   const struct mlx5_modify_raw_qp_param *raw_qp_param)
{
	void *in;
	void *rqc;
	int inlen;
	int err;

	inlen = MLX5_ST_SZ_BYTES(modify_rq_in);
	in = mlx5_vzalloc(inlen);
	if (!in)
		return -ENOMEM;

	MLX5_SET(modify_rq_in, in, rq_state, rq->state);

	rqc = MLX5_ADDR_OF(modify_rq_in, in, ctx);
	MLX5_SET(rqc, rqc, state, new_state);

	if (raw_qp_param->set_mask & MLX5_RAW_QP_MOD_SET_RQ_Q_CTR_ID) {
		if (MLX5_CAP_GEN(dev->mdev, modify_rq_counter_set_id)) {
			MLX5_SET64(modify_rq_in, in, modify_bitmask,
				   MLX5_MODIFY_RQ_IN_MODIFY_BITMASK_MODIFY_RQ_COUNTER_SET_ID);
			MLX5_SET(rqc, rqc, counter_set_id, raw_qp_param->rq_q_ctr_id);
		} else
			pr_info_once("%s: RAW PACKET QP counters are not supported on current FW\n",
				     dev->ib_dev.name);
	}

	err = mlx5_core_modify_rq(dev->mdev, rq->base.mqp.qpn, in, inlen);
	if (err)
		goto out;

	rq->state = new_state;

out:
	kvfree(in);
	return err;
}

static int modify_raw_packet_qp_sq(struct mlx5_core_dev *dev,
				   struct mlx5_ib_sq *sq, int new_state)
{
	void *in;
	void *sqc;
	int inlen;
	int err;

	inlen = MLX5_ST_SZ_BYTES(modify_sq_in);
	in = mlx5_vzalloc(inlen);
	if (!in)
		return -ENOMEM;

	MLX5_SET(modify_sq_in, in, sq_state, sq->state);

	sqc = MLX5_ADDR_OF(modify_sq_in, in, ctx);
	MLX5_SET(sqc, sqc, state, new_state);

	err = mlx5_core_modify_sq(dev, sq->base.mqp.qpn, in, inlen);
	if (err)
		goto out;

	sq->state = new_state;

out:
	kvfree(in);
	return err;
}

static int modify_raw_packet_qp(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp,
				const struct mlx5_modify_raw_qp_param *raw_qp_param,
				u8 tx_affinity)
{
	struct mlx5_ib_raw_packet_qp *raw_packet_qp = &qp->raw_packet_qp;
	struct mlx5_ib_rq *rq = &raw_packet_qp->rq;
	struct mlx5_ib_sq *sq = &raw_packet_qp->sq;
	int rq_state;
	int sq_state;
	int err;

	switch (raw_qp_param->operation) {
	case MLX5_CMD_OP_RST2INIT_QP:
		rq_state = MLX5_RQC_STATE_RDY;
		sq_state = MLX5_SQC_STATE_RDY;
		break;
	case MLX5_CMD_OP_2ERR_QP:
		rq_state = MLX5_RQC_STATE_ERR;
		sq_state = MLX5_SQC_STATE_ERR;
		break;
	case MLX5_CMD_OP_2RST_QP:
		rq_state = MLX5_RQC_STATE_RST;
		sq_state = MLX5_SQC_STATE_RST;
		break;
	case MLX5_CMD_OP_INIT2INIT_QP:
	case MLX5_CMD_OP_INIT2RTR_QP:
	case MLX5_CMD_OP_RTR2RTS_QP:
	case MLX5_CMD_OP_RTS2RTS_QP:
		if (raw_qp_param->set_mask)
			return -EINVAL;
		else
			return 0;
	default:
		WARN_ON(1);
		return -EINVAL;
	}

	if (qp->rq.wqe_cnt) {
		err = modify_raw_packet_qp_rq(dev, rq, rq_state, raw_qp_param);
		if (err)
			return err;
	}

	if (qp->sq.wqe_cnt) {
		if (tx_affinity) {
			err = modify_raw_packet_tx_affinity(dev->mdev, sq,
							    tx_affinity);
			if (err)
				return err;
		}

		return modify_raw_packet_qp_sq(dev->mdev, sq, sq_state);
	}

	return 0;
}

>>>>>>> v4.9.227
static int __mlx5_ib_modify_qp(struct ib_qp *ibqp,
			       const struct ib_qp_attr *attr, int attr_mask,
			       enum ib_qp_state cur_state, enum ib_qp_state new_state)
{
<<<<<<< HEAD
	struct mlx5_ib_dev *dev = to_mdev(ibqp->device);
	struct mlx5_ib_qp *qp = to_mqp(ibqp);
	struct mlx5_ib_cq *send_cq, *recv_cq;
	struct mlx5_qp_context *context;
	struct mlx5_general_caps *gen;
	struct mlx5_modify_qp_mbox_in *in;
	struct mlx5_ib_pd *pd;
=======
	static const u16 optab[MLX5_QP_NUM_STATE][MLX5_QP_NUM_STATE] = {
		[MLX5_QP_STATE_RST] = {
			[MLX5_QP_STATE_RST]	= MLX5_CMD_OP_2RST_QP,
			[MLX5_QP_STATE_ERR]	= MLX5_CMD_OP_2ERR_QP,
			[MLX5_QP_STATE_INIT]	= MLX5_CMD_OP_RST2INIT_QP,
		},
		[MLX5_QP_STATE_INIT]  = {
			[MLX5_QP_STATE_RST]	= MLX5_CMD_OP_2RST_QP,
			[MLX5_QP_STATE_ERR]	= MLX5_CMD_OP_2ERR_QP,
			[MLX5_QP_STATE_INIT]	= MLX5_CMD_OP_INIT2INIT_QP,
			[MLX5_QP_STATE_RTR]	= MLX5_CMD_OP_INIT2RTR_QP,
		},
		[MLX5_QP_STATE_RTR]   = {
			[MLX5_QP_STATE_RST]	= MLX5_CMD_OP_2RST_QP,
			[MLX5_QP_STATE_ERR]	= MLX5_CMD_OP_2ERR_QP,
			[MLX5_QP_STATE_RTS]	= MLX5_CMD_OP_RTR2RTS_QP,
		},
		[MLX5_QP_STATE_RTS]   = {
			[MLX5_QP_STATE_RST]	= MLX5_CMD_OP_2RST_QP,
			[MLX5_QP_STATE_ERR]	= MLX5_CMD_OP_2ERR_QP,
			[MLX5_QP_STATE_RTS]	= MLX5_CMD_OP_RTS2RTS_QP,
		},
		[MLX5_QP_STATE_SQD] = {
			[MLX5_QP_STATE_RST]	= MLX5_CMD_OP_2RST_QP,
			[MLX5_QP_STATE_ERR]	= MLX5_CMD_OP_2ERR_QP,
		},
		[MLX5_QP_STATE_SQER] = {
			[MLX5_QP_STATE_RST]	= MLX5_CMD_OP_2RST_QP,
			[MLX5_QP_STATE_ERR]	= MLX5_CMD_OP_2ERR_QP,
			[MLX5_QP_STATE_RTS]	= MLX5_CMD_OP_SQERR2RTS_QP,
		},
		[MLX5_QP_STATE_ERR] = {
			[MLX5_QP_STATE_RST]	= MLX5_CMD_OP_2RST_QP,
			[MLX5_QP_STATE_ERR]	= MLX5_CMD_OP_2ERR_QP,
		}
	};

	struct mlx5_ib_dev *dev = to_mdev(ibqp->device);
	struct mlx5_ib_qp *qp = to_mqp(ibqp);
	struct mlx5_ib_qp_base *base = &qp->trans_qp.base;
	struct mlx5_ib_cq *send_cq, *recv_cq;
	struct mlx5_qp_context *context;
	struct mlx5_ib_pd *pd;
	struct mlx5_ib_port *mibport = NULL;
>>>>>>> v4.9.227
	enum mlx5_qp_state mlx5_cur, mlx5_new;
	enum mlx5_qp_optpar optpar;
	int sqd_event;
	int mlx5_st;
	int err;
<<<<<<< HEAD

	gen = &dev->mdev->caps.gen;
	in = kzalloc(sizeof(*in), GFP_KERNEL);
	if (!in)
		return -ENOMEM;

	context = &in->ctx;
	err = to_mlx5_st(ibqp->qp_type);
	if (err < 0)
		goto out;
=======
	u16 op;
	u8 tx_affinity = 0;

	context = kzalloc(sizeof(*context), GFP_KERNEL);
	if (!context)
		return -ENOMEM;

	err = to_mlx5_st(ibqp->qp_type);
	if (err < 0) {
		mlx5_ib_dbg(dev, "unsupported qp type %d\n", ibqp->qp_type);
		goto out;
	}
>>>>>>> v4.9.227

	context->flags = cpu_to_be32(err << 16);

	if (!(attr_mask & IB_QP_PATH_MIG_STATE)) {
		context->flags |= cpu_to_be32(MLX5_QP_PM_MIGRATED << 11);
	} else {
		switch (attr->path_mig_state) {
		case IB_MIG_MIGRATED:
			context->flags |= cpu_to_be32(MLX5_QP_PM_MIGRATED << 11);
			break;
		case IB_MIG_REARM:
			context->flags |= cpu_to_be32(MLX5_QP_PM_REARM << 11);
			break;
		case IB_MIG_ARMED:
			context->flags |= cpu_to_be32(MLX5_QP_PM_ARMED << 11);
			break;
		}
	}

<<<<<<< HEAD
	if (ibqp->qp_type == IB_QPT_GSI || ibqp->qp_type == IB_QPT_SMI) {
=======
	if ((cur_state == IB_QPS_RESET) && (new_state == IB_QPS_INIT)) {
		if ((ibqp->qp_type == IB_QPT_RC) ||
		    (ibqp->qp_type == IB_QPT_UD &&
		     !(qp->flags & MLX5_IB_QP_SQPN_QP1)) ||
		    (ibqp->qp_type == IB_QPT_UC) ||
		    (ibqp->qp_type == IB_QPT_RAW_PACKET) ||
		    (ibqp->qp_type == IB_QPT_XRC_INI) ||
		    (ibqp->qp_type == IB_QPT_XRC_TGT)) {
			if (mlx5_lag_is_active(dev->mdev)) {
				tx_affinity = (unsigned int)atomic_add_return(1,
						&dev->roce.next_port) %
						MLX5_MAX_PORTS + 1;
				context->flags |= cpu_to_be32(tx_affinity << 24);
			}
		}
	}

	if (is_sqp(ibqp->qp_type)) {
>>>>>>> v4.9.227
		context->mtu_msgmax = (IB_MTU_256 << 5) | 8;
	} else if (ibqp->qp_type == IB_QPT_UD ||
		   ibqp->qp_type == MLX5_IB_QPT_REG_UMR) {
		context->mtu_msgmax = (IB_MTU_4096 << 5) | 12;
	} else if (attr_mask & IB_QP_PATH_MTU) {
		if (attr->path_mtu < IB_MTU_256 ||
		    attr->path_mtu > IB_MTU_4096) {
			mlx5_ib_warn(dev, "invalid mtu %d\n", attr->path_mtu);
			err = -EINVAL;
			goto out;
		}
<<<<<<< HEAD
		context->mtu_msgmax = (attr->path_mtu << 5) | gen->log_max_msg;
=======
		context->mtu_msgmax = (attr->path_mtu << 5) |
				      (u8)MLX5_CAP_GEN(dev->mdev, log_max_msg);
>>>>>>> v4.9.227
	}

	if (attr_mask & IB_QP_DEST_QPN)
		context->log_pg_sz_remote_qpn = cpu_to_be32(attr->dest_qp_num);

	if (attr_mask & IB_QP_PKEY_INDEX)
<<<<<<< HEAD
		context->pri_path.pkey_index = attr->pkey_index;
=======
		context->pri_path.pkey_index = cpu_to_be16(attr->pkey_index);
>>>>>>> v4.9.227

	/* todo implement counter_index functionality */

	if (is_sqp(ibqp->qp_type))
		context->pri_path.port = qp->port;

	if (attr_mask & IB_QP_PORT)
		context->pri_path.port = attr->port_num;

	if (attr_mask & IB_QP_AV) {
<<<<<<< HEAD
		err = mlx5_set_path(dev, &attr->ah_attr, &context->pri_path,
				    attr_mask & IB_QP_PORT ? attr->port_num : qp->port,
				    attr_mask, 0, attr);
=======
		err = mlx5_set_path(dev, qp, &attr->ah_attr, &context->pri_path,
				    attr_mask & IB_QP_PORT ? attr->port_num : qp->port,
				    attr_mask, 0, attr, false);
>>>>>>> v4.9.227
		if (err)
			goto out;
	}

	if (attr_mask & IB_QP_TIMEOUT)
		context->pri_path.ackto_lt |= attr->timeout << 3;

	if (attr_mask & IB_QP_ALT_PATH) {
<<<<<<< HEAD
		err = mlx5_set_path(dev, &attr->alt_ah_attr, &context->alt_path,
				    attr->alt_port_num, attr_mask, 0, attr);
=======
		err = mlx5_set_path(dev, qp, &attr->alt_ah_attr,
				    &context->alt_path,
				    attr->alt_port_num,
				    attr_mask | IB_QP_PKEY_INDEX | IB_QP_TIMEOUT,
				    0, attr, true);
>>>>>>> v4.9.227
		if (err)
			goto out;
	}

	pd = get_pd(qp);
<<<<<<< HEAD
	get_cqs(qp, &send_cq, &recv_cq);
=======
	get_cqs(qp->ibqp.qp_type, qp->ibqp.send_cq, qp->ibqp.recv_cq,
		&send_cq, &recv_cq);
>>>>>>> v4.9.227

	context->flags_pd = cpu_to_be32(pd ? pd->pdn : to_mpd(dev->devr.p0)->pdn);
	context->cqn_send = send_cq ? cpu_to_be32(send_cq->mcq.cqn) : 0;
	context->cqn_recv = recv_cq ? cpu_to_be32(recv_cq->mcq.cqn) : 0;
	context->params1  = cpu_to_be32(MLX5_IB_ACK_REQ_FREQ << 28);

	if (attr_mask & IB_QP_RNR_RETRY)
		context->params1 |= cpu_to_be32(attr->rnr_retry << 13);

	if (attr_mask & IB_QP_RETRY_CNT)
		context->params1 |= cpu_to_be32(attr->retry_cnt << 16);

	if (attr_mask & IB_QP_MAX_QP_RD_ATOMIC) {
		if (attr->max_rd_atomic)
			context->params1 |=
				cpu_to_be32(fls(attr->max_rd_atomic - 1) << 21);
	}

	if (attr_mask & IB_QP_SQ_PSN)
		context->next_send_psn = cpu_to_be32(attr->sq_psn);

	if (attr_mask & IB_QP_MAX_DEST_RD_ATOMIC) {
		if (attr->max_dest_rd_atomic)
			context->params2 |=
				cpu_to_be32(fls(attr->max_dest_rd_atomic - 1) << 21);
	}

	if (attr_mask & (IB_QP_ACCESS_FLAGS | IB_QP_MAX_DEST_RD_ATOMIC))
		context->params2 |= to_mlx5_access_flags(qp, attr, attr_mask);

	if (attr_mask & IB_QP_MIN_RNR_TIMER)
		context->rnr_nextrecvpsn |= cpu_to_be32(attr->min_rnr_timer << 24);

	if (attr_mask & IB_QP_RQ_PSN)
		context->rnr_nextrecvpsn |= cpu_to_be32(attr->rq_psn);

	if (attr_mask & IB_QP_QKEY)
		context->qkey = cpu_to_be32(attr->qkey);

	if (qp->rq.wqe_cnt && cur_state == IB_QPS_RESET && new_state == IB_QPS_INIT)
		context->db_rec_addr = cpu_to_be64(qp->db.dma);

	if (cur_state == IB_QPS_RTS && new_state == IB_QPS_SQD	&&
	    attr_mask & IB_QP_EN_SQD_ASYNC_NOTIFY && attr->en_sqd_async_notify)
		sqd_event = 1;
	else
		sqd_event = 0;

<<<<<<< HEAD
	if (!ibqp->uobject && cur_state == IB_QPS_RESET && new_state == IB_QPS_INIT)
		context->sq_crq_size |= cpu_to_be16(1 << 4);

=======
	if (cur_state == IB_QPS_RESET && new_state == IB_QPS_INIT) {
		u8 port_num = (attr_mask & IB_QP_PORT ? attr->port_num :
			       qp->port) - 1;
		mibport = &dev->port[port_num];
		context->qp_counter_set_usr_page |=
			cpu_to_be32((u32)(mibport->q_cnt_id) << 24);
	}

	if (!ibqp->uobject && cur_state == IB_QPS_RESET && new_state == IB_QPS_INIT)
		context->sq_crq_size |= cpu_to_be16(1 << 4);

	if (qp->flags & MLX5_IB_QP_SQPN_QP1)
		context->deth_sqpn = cpu_to_be32(1);
>>>>>>> v4.9.227

	mlx5_cur = to_mlx5_state(cur_state);
	mlx5_new = to_mlx5_state(new_state);
	mlx5_st = to_mlx5_st(ibqp->qp_type);
	if (mlx5_st < 0)
		goto out;

<<<<<<< HEAD
	optpar = ib_mask_to_mlx5_opt(attr_mask);
	optpar &= opt_mask[mlx5_cur][mlx5_new][mlx5_st];
	in->optparam = cpu_to_be32(optpar);
	err = mlx5_core_qp_modify(dev->mdev, to_mlx5_state(cur_state),
				  to_mlx5_state(new_state), in, sqd_event,
				  &qp->mqp);
	if (err)
		goto out;

	qp->state = new_state;

	if (attr_mask & IB_QP_ACCESS_FLAGS)
		qp->atomic_rd_en = attr->qp_access_flags;
	if (attr_mask & IB_QP_MAX_DEST_RD_ATOMIC)
		qp->resp_depth = attr->max_dest_rd_atomic;
	if (attr_mask & IB_QP_PORT)
		qp->port = attr->port_num;
	if (attr_mask & IB_QP_ALT_PATH)
		qp->alt_port = attr->alt_port_num;
=======
	/* If moving to a reset or error state, we must disable page faults on
	 * this QP and flush all current page faults. Otherwise a stale page
	 * fault may attempt to work on this QP after it is reset and moved
	 * again to RTS, and may cause the driver and the device to get out of
	 * sync. */
	if (cur_state != IB_QPS_RESET && cur_state != IB_QPS_ERR &&
	    (new_state == IB_QPS_RESET || new_state == IB_QPS_ERR) &&
	    (qp->ibqp.qp_type != IB_QPT_RAW_PACKET))
		mlx5_ib_qp_disable_pagefaults(qp);

	if (mlx5_cur >= MLX5_QP_NUM_STATE || mlx5_new >= MLX5_QP_NUM_STATE ||
	    !optab[mlx5_cur][mlx5_new]) {
		err = -EINVAL;
		goto out;
	}

	op = optab[mlx5_cur][mlx5_new];
	optpar = ib_mask_to_mlx5_opt(attr_mask);
	optpar &= opt_mask[mlx5_cur][mlx5_new][mlx5_st];

	if (qp->ibqp.qp_type == IB_QPT_RAW_PACKET) {
		struct mlx5_modify_raw_qp_param raw_qp_param = {};

		raw_qp_param.operation = op;
		if (cur_state == IB_QPS_RESET && new_state == IB_QPS_INIT) {
			raw_qp_param.rq_q_ctr_id = mibport->q_cnt_id;
			raw_qp_param.set_mask |= MLX5_RAW_QP_MOD_SET_RQ_Q_CTR_ID;
		}
		err = modify_raw_packet_qp(dev, qp, &raw_qp_param, tx_affinity);
	} else {
		err = mlx5_core_qp_modify(dev->mdev, op, optpar, context,
					  &base->mqp);
	}

	if (err)
		goto out;

	if (cur_state == IB_QPS_RESET && new_state == IB_QPS_INIT &&
	    (qp->ibqp.qp_type != IB_QPT_RAW_PACKET))
		mlx5_ib_qp_enable_pagefaults(qp);

	qp->state = new_state;

	if (attr_mask & IB_QP_ACCESS_FLAGS)
		qp->trans_qp.atomic_rd_en = attr->qp_access_flags;
	if (attr_mask & IB_QP_MAX_DEST_RD_ATOMIC)
		qp->trans_qp.resp_depth = attr->max_dest_rd_atomic;
	if (attr_mask & IB_QP_PORT)
		qp->port = attr->port_num;
	if (attr_mask & IB_QP_ALT_PATH)
		qp->trans_qp.alt_port = attr->alt_port_num;
>>>>>>> v4.9.227

	/*
	 * If we moved a kernel QP to RESET, clean up all old CQ
	 * entries and reinitialize the QP.
	 */
<<<<<<< HEAD
	if (new_state == IB_QPS_RESET && !ibqp->uobject) {
		mlx5_ib_cq_clean(recv_cq, qp->mqp.qpn,
				 ibqp->srq ? to_msrq(ibqp->srq) : NULL);
		if (send_cq != recv_cq)
			mlx5_ib_cq_clean(send_cq, qp->mqp.qpn, NULL);
=======
	if (new_state == IB_QPS_RESET &&
	    !ibqp->uobject && ibqp->qp_type != IB_QPT_XRC_TGT) {
		mlx5_ib_cq_clean(recv_cq, base->mqp.qpn,
				 ibqp->srq ? to_msrq(ibqp->srq) : NULL);
		if (send_cq != recv_cq)
			mlx5_ib_cq_clean(send_cq, base->mqp.qpn, NULL);
>>>>>>> v4.9.227

		qp->rq.head = 0;
		qp->rq.tail = 0;
		qp->sq.head = 0;
		qp->sq.tail = 0;
		qp->sq.cur_post = 0;
		qp->sq.last_poll = 0;
		qp->db.db[MLX5_RCV_DBR] = 0;
		qp->db.db[MLX5_SND_DBR] = 0;
	}

out:
<<<<<<< HEAD
	kfree(in);
=======
	kfree(context);
>>>>>>> v4.9.227
	return err;
}

int mlx5_ib_modify_qp(struct ib_qp *ibqp, struct ib_qp_attr *attr,
		      int attr_mask, struct ib_udata *udata)
{
	struct mlx5_ib_dev *dev = to_mdev(ibqp->device);
	struct mlx5_ib_qp *qp = to_mqp(ibqp);
<<<<<<< HEAD
	enum ib_qp_state cur_state, new_state;
	struct mlx5_general_caps *gen;
	int err = -EINVAL;
	int port;

	gen = &dev->mdev->caps.gen;
=======
	enum ib_qp_type qp_type;
	enum ib_qp_state cur_state, new_state;
	int err = -EINVAL;
	int port;
	enum rdma_link_layer ll = IB_LINK_LAYER_UNSPECIFIED;

	if (ibqp->rwq_ind_tbl)
		return -ENOSYS;

	if (unlikely(ibqp->qp_type == IB_QPT_GSI))
		return mlx5_ib_gsi_modify_qp(ibqp, attr, attr_mask);

	qp_type = (unlikely(ibqp->qp_type == MLX5_IB_QPT_HW_GSI)) ?
		IB_QPT_GSI : ibqp->qp_type;

>>>>>>> v4.9.227
	mutex_lock(&qp->mutex);

	cur_state = attr_mask & IB_QP_CUR_STATE ? attr->cur_qp_state : qp->state;
	new_state = attr_mask & IB_QP_STATE ? attr->qp_state : cur_state;

<<<<<<< HEAD
	if (ibqp->qp_type != MLX5_IB_QPT_REG_UMR &&
	    !ib_modify_qp_is_ok(cur_state, new_state, ibqp->qp_type, attr_mask,
				IB_LINK_LAYER_UNSPECIFIED))
		goto out;

	if ((attr_mask & IB_QP_PORT) &&
	    (attr->port_num == 0 || attr->port_num > gen->num_ports))
		goto out;

	if (attr_mask & IB_QP_PKEY_INDEX) {
		port = attr_mask & IB_QP_PORT ? attr->port_num : qp->port;
		if (attr->pkey_index >= gen->port[port - 1].pkey_table_len)
			goto out;
	}

	if (attr_mask & IB_QP_MAX_QP_RD_ATOMIC &&
	    attr->max_rd_atomic > (1 << gen->log_max_ra_res_qp))
		goto out;

	if (attr_mask & IB_QP_MAX_DEST_RD_ATOMIC &&
	    attr->max_dest_rd_atomic > (1 << gen->log_max_ra_req_qp))
		goto out;
=======
	if (!(cur_state == new_state && cur_state == IB_QPS_RESET)) {
		port = attr_mask & IB_QP_PORT ? attr->port_num : qp->port;
		ll = dev->ib_dev.get_link_layer(&dev->ib_dev, port);
	}

	if (qp_type != MLX5_IB_QPT_REG_UMR &&
	    !ib_modify_qp_is_ok(cur_state, new_state, qp_type, attr_mask, ll)) {
		mlx5_ib_dbg(dev, "invalid QP state transition from %d to %d, qp_type %d, attr_mask 0x%x\n",
			    cur_state, new_state, ibqp->qp_type, attr_mask);
		goto out;
	}

	if ((attr_mask & IB_QP_PORT) &&
	    (attr->port_num == 0 ||
	     attr->port_num > MLX5_CAP_GEN(dev->mdev, num_ports))) {
		mlx5_ib_dbg(dev, "invalid port number %d. number of ports is %d\n",
			    attr->port_num, dev->num_ports);
		goto out;
	}

	if (attr_mask & IB_QP_PKEY_INDEX) {
		port = attr_mask & IB_QP_PORT ? attr->port_num : qp->port;
		if (attr->pkey_index >=
		    dev->mdev->port_caps[port - 1].pkey_table_len) {
			mlx5_ib_dbg(dev, "invalid pkey index %d\n",
				    attr->pkey_index);
			goto out;
		}
	}

	if (attr_mask & IB_QP_MAX_QP_RD_ATOMIC &&
	    attr->max_rd_atomic >
	    (1 << MLX5_CAP_GEN(dev->mdev, log_max_ra_res_qp))) {
		mlx5_ib_dbg(dev, "invalid max_rd_atomic value %d\n",
			    attr->max_rd_atomic);
		goto out;
	}

	if (attr_mask & IB_QP_MAX_DEST_RD_ATOMIC &&
	    attr->max_dest_rd_atomic >
	    (1 << MLX5_CAP_GEN(dev->mdev, log_max_ra_req_qp))) {
		mlx5_ib_dbg(dev, "invalid max_dest_rd_atomic value %d\n",
			    attr->max_dest_rd_atomic);
		goto out;
	}
>>>>>>> v4.9.227

	if (cur_state == new_state && cur_state == IB_QPS_RESET) {
		err = 0;
		goto out;
	}

	err = __mlx5_ib_modify_qp(ibqp, attr, attr_mask, cur_state, new_state);

out:
	mutex_unlock(&qp->mutex);
	return err;
}

static int mlx5_wq_overflow(struct mlx5_ib_wq *wq, int nreq, struct ib_cq *ib_cq)
{
	struct mlx5_ib_cq *cq;
	unsigned cur;

	cur = wq->head - wq->tail;
	if (likely(cur + nreq < wq->max_post))
		return 0;

	cq = to_mcq(ib_cq);
	spin_lock(&cq->lock);
	cur = wq->head - wq->tail;
	spin_unlock(&cq->lock);

	return cur + nreq >= wq->max_post;
}

static __always_inline void set_raddr_seg(struct mlx5_wqe_raddr_seg *rseg,
					  u64 remote_addr, u32 rkey)
{
	rseg->raddr    = cpu_to_be64(remote_addr);
	rseg->rkey     = cpu_to_be32(rkey);
	rseg->reserved = 0;
}

<<<<<<< HEAD
static void set_datagram_seg(struct mlx5_wqe_datagram_seg *dseg,
			     struct ib_send_wr *wr)
{
	memcpy(&dseg->av, &to_mah(wr->wr.ud.ah)->av, sizeof(struct mlx5_av));
	dseg->av.dqp_dct = cpu_to_be32(wr->wr.ud.remote_qpn | MLX5_EXTENDED_UD_AV);
	dseg->av.key.qkey.qkey = cpu_to_be32(wr->wr.ud.remote_qkey);
=======
static void *set_eth_seg(struct mlx5_wqe_eth_seg *eseg,
			 struct ib_send_wr *wr, void *qend,
			 struct mlx5_ib_qp *qp, int *size)
{
	void *seg = eseg;

	memset(eseg, 0, sizeof(struct mlx5_wqe_eth_seg));

	if (wr->send_flags & IB_SEND_IP_CSUM)
		eseg->cs_flags = MLX5_ETH_WQE_L3_CSUM |
				 MLX5_ETH_WQE_L4_CSUM;

	seg += sizeof(struct mlx5_wqe_eth_seg);
	*size += sizeof(struct mlx5_wqe_eth_seg) / 16;

	if (wr->opcode == IB_WR_LSO) {
		struct ib_ud_wr *ud_wr = container_of(wr, struct ib_ud_wr, wr);
		int size_of_inl_hdr_start = sizeof(eseg->inline_hdr_start);
		u64 left, leftlen, copysz;
		void *pdata = ud_wr->header;

		left = ud_wr->hlen;
		eseg->mss = cpu_to_be16(ud_wr->mss);
		eseg->inline_hdr_sz = cpu_to_be16(left);

		/*
		 * check if there is space till the end of queue, if yes,
		 * copy all in one shot, otherwise copy till the end of queue,
		 * rollback and than the copy the left
		 */
		leftlen = qend - (void *)eseg->inline_hdr_start;
		copysz = min_t(u64, leftlen, left);

		memcpy(seg - size_of_inl_hdr_start, pdata, copysz);

		if (likely(copysz > size_of_inl_hdr_start)) {
			seg += ALIGN(copysz - size_of_inl_hdr_start, 16);
			*size += ALIGN(copysz - size_of_inl_hdr_start, 16) / 16;
		}

		if (unlikely(copysz < left)) { /* the last wqe in the queue */
			seg = mlx5_get_send_wqe(qp, 0);
			left -= copysz;
			pdata += copysz;
			memcpy(seg, pdata, left);
			seg += ALIGN(left, 16);
			*size += ALIGN(left, 16) / 16;
		}
	}

	return seg;
}

static void set_datagram_seg(struct mlx5_wqe_datagram_seg *dseg,
			     struct ib_send_wr *wr)
{
	memcpy(&dseg->av, &to_mah(ud_wr(wr)->ah)->av, sizeof(struct mlx5_av));
	dseg->av.dqp_dct = cpu_to_be32(ud_wr(wr)->remote_qpn | MLX5_EXTENDED_UD_AV);
	dseg->av.key.qkey.qkey = cpu_to_be32(ud_wr(wr)->remote_qkey);
>>>>>>> v4.9.227
}

static void set_data_ptr_seg(struct mlx5_wqe_data_seg *dseg, struct ib_sge *sg)
{
	dseg->byte_count = cpu_to_be32(sg->length);
	dseg->lkey       = cpu_to_be32(sg->lkey);
	dseg->addr       = cpu_to_be64(sg->addr);
}

static __be16 get_klm_octo(int npages)
{
	return cpu_to_be16(ALIGN(npages, 8) / 2);
}

static __be64 frwr_mkey_mask(void)
{
	u64 result;

	result = MLX5_MKEY_MASK_LEN		|
		MLX5_MKEY_MASK_PAGE_SIZE	|
		MLX5_MKEY_MASK_START_ADDR	|
		MLX5_MKEY_MASK_EN_RINVAL	|
		MLX5_MKEY_MASK_KEY		|
		MLX5_MKEY_MASK_LR		|
		MLX5_MKEY_MASK_LW		|
		MLX5_MKEY_MASK_RR		|
		MLX5_MKEY_MASK_RW		|
		MLX5_MKEY_MASK_A		|
		MLX5_MKEY_MASK_SMALL_FENCE	|
		MLX5_MKEY_MASK_FREE;

	return cpu_to_be64(result);
}

static __be64 sig_mkey_mask(void)
{
	u64 result;

	result = MLX5_MKEY_MASK_LEN		|
		MLX5_MKEY_MASK_PAGE_SIZE	|
		MLX5_MKEY_MASK_START_ADDR	|
		MLX5_MKEY_MASK_EN_SIGERR	|
		MLX5_MKEY_MASK_EN_RINVAL	|
		MLX5_MKEY_MASK_KEY		|
		MLX5_MKEY_MASK_LR		|
		MLX5_MKEY_MASK_LW		|
		MLX5_MKEY_MASK_RR		|
		MLX5_MKEY_MASK_RW		|
		MLX5_MKEY_MASK_SMALL_FENCE	|
		MLX5_MKEY_MASK_FREE		|
		MLX5_MKEY_MASK_BSF_EN;

	return cpu_to_be64(result);
}

<<<<<<< HEAD
static void set_frwr_umr_segment(struct mlx5_wqe_umr_ctrl_seg *umr,
				 struct ib_send_wr *wr, int li)
{
	memset(umr, 0, sizeof(*umr));

	if (li) {
		umr->mkey_mask = cpu_to_be64(MLX5_MKEY_MASK_FREE);
		umr->flags = 1 << 7;
		return;
	}

	umr->flags = (1 << 5); /* fail if not free */
	umr->klm_octowords = get_klm_octo(wr->wr.fast_reg.page_list_len);
	umr->mkey_mask = frwr_mkey_mask();
}

static void set_reg_umr_segment(struct mlx5_wqe_umr_ctrl_seg *umr,
				struct ib_send_wr *wr)
{
	struct umr_wr *umrwr = (struct umr_wr *)&wr->wr.fast_reg;
	u64 mask;

	memset(umr, 0, sizeof(*umr));

	if (!(wr->send_flags & MLX5_IB_SEND_UMR_UNREG)) {
		umr->flags = 1 << 5; /* fail if not free */
		umr->klm_octowords = get_klm_octo(umrwr->npages);
		mask =  MLX5_MKEY_MASK_LEN		|
			MLX5_MKEY_MASK_PAGE_SIZE	|
			MLX5_MKEY_MASK_START_ADDR	|
			MLX5_MKEY_MASK_PD		|
			MLX5_MKEY_MASK_LR		|
			MLX5_MKEY_MASK_LW		|
			MLX5_MKEY_MASK_KEY		|
			MLX5_MKEY_MASK_RR		|
			MLX5_MKEY_MASK_RW		|
			MLX5_MKEY_MASK_A		|
			MLX5_MKEY_MASK_FREE;
		umr->mkey_mask = cpu_to_be64(mask);
	} else {
		umr->flags = 2 << 5; /* fail if free */
		mask = MLX5_MKEY_MASK_FREE;
		umr->mkey_mask = cpu_to_be64(mask);
	}

	if (!wr->num_sge)
		umr->flags |= (1 << 7); /* inline */
=======
static void set_reg_umr_seg(struct mlx5_wqe_umr_ctrl_seg *umr,
				struct mlx5_ib_mr *mr)
{
	int ndescs = mr->ndescs;

	memset(umr, 0, sizeof(*umr));

	if (mr->access_mode == MLX5_MKC_ACCESS_MODE_KLMS)
		/* KLMs take twice the size of MTTs */
		ndescs *= 2;

	umr->flags = MLX5_UMR_CHECK_NOT_FREE;
	umr->klm_octowords = get_klm_octo(ndescs);
	umr->mkey_mask = frwr_mkey_mask();
}

static void set_linv_umr_seg(struct mlx5_wqe_umr_ctrl_seg *umr)
{
	memset(umr, 0, sizeof(*umr));
	umr->mkey_mask = cpu_to_be64(MLX5_MKEY_MASK_FREE);
	umr->flags = 1 << 7;
}

static __be64 get_umr_reg_mr_mask(void)
{
	u64 result;

	result = MLX5_MKEY_MASK_LEN		|
		 MLX5_MKEY_MASK_PAGE_SIZE	|
		 MLX5_MKEY_MASK_START_ADDR	|
		 MLX5_MKEY_MASK_PD		|
		 MLX5_MKEY_MASK_LR		|
		 MLX5_MKEY_MASK_LW		|
		 MLX5_MKEY_MASK_KEY		|
		 MLX5_MKEY_MASK_RR		|
		 MLX5_MKEY_MASK_RW		|
		 MLX5_MKEY_MASK_A		|
		 MLX5_MKEY_MASK_FREE;

	return cpu_to_be64(result);
}

static __be64 get_umr_unreg_mr_mask(void)
{
	u64 result;

	result = MLX5_MKEY_MASK_FREE;

	return cpu_to_be64(result);
}

static __be64 get_umr_update_mtt_mask(void)
{
	u64 result;

	result = MLX5_MKEY_MASK_FREE;

	return cpu_to_be64(result);
}

static __be64 get_umr_update_translation_mask(void)
{
	u64 result;

	result = MLX5_MKEY_MASK_LEN |
		 MLX5_MKEY_MASK_PAGE_SIZE |
		 MLX5_MKEY_MASK_START_ADDR |
		 MLX5_MKEY_MASK_KEY |
		 MLX5_MKEY_MASK_FREE;

	return cpu_to_be64(result);
}

static __be64 get_umr_update_access_mask(void)
{
	u64 result;

	result = MLX5_MKEY_MASK_LW |
		 MLX5_MKEY_MASK_RR |
		 MLX5_MKEY_MASK_RW |
		 MLX5_MKEY_MASK_A |
		 MLX5_MKEY_MASK_KEY |
		 MLX5_MKEY_MASK_FREE;

	return cpu_to_be64(result);
}

static __be64 get_umr_update_pd_mask(void)
{
	u64 result;

	result = MLX5_MKEY_MASK_PD |
		 MLX5_MKEY_MASK_KEY |
		 MLX5_MKEY_MASK_FREE;

	return cpu_to_be64(result);
}

static void set_reg_umr_segment(struct mlx5_wqe_umr_ctrl_seg *umr,
				struct ib_send_wr *wr)
{
	struct mlx5_umr_wr *umrwr = umr_wr(wr);

	memset(umr, 0, sizeof(*umr));

	if (wr->send_flags & MLX5_IB_SEND_UMR_FAIL_IF_FREE)
		umr->flags = MLX5_UMR_CHECK_FREE; /* fail if free */
	else
		umr->flags = MLX5_UMR_CHECK_NOT_FREE; /* fail if not free */

	if (!(wr->send_flags & MLX5_IB_SEND_UMR_UNREG)) {
		umr->klm_octowords = get_klm_octo(umrwr->npages);
		if (wr->send_flags & MLX5_IB_SEND_UMR_UPDATE_MTT) {
			umr->mkey_mask = get_umr_update_mtt_mask();
			umr->bsf_octowords = get_klm_octo(umrwr->target.offset);
			umr->flags |= MLX5_UMR_TRANSLATION_OFFSET_EN;
		}
		if (wr->send_flags & MLX5_IB_SEND_UMR_UPDATE_TRANSLATION)
			umr->mkey_mask |= get_umr_update_translation_mask();
		if (wr->send_flags & MLX5_IB_SEND_UMR_UPDATE_ACCESS)
			umr->mkey_mask |= get_umr_update_access_mask();
		if (wr->send_flags & MLX5_IB_SEND_UMR_UPDATE_PD)
			umr->mkey_mask |= get_umr_update_pd_mask();
		if (!umr->mkey_mask)
			umr->mkey_mask = get_umr_reg_mr_mask();
	} else {
		umr->mkey_mask = get_umr_unreg_mr_mask();
	}

	if (!wr->num_sge)
		umr->flags |= MLX5_UMR_INLINE;
>>>>>>> v4.9.227
}

static u8 get_umr_flags(int acc)
{
	return (acc & IB_ACCESS_REMOTE_ATOMIC ? MLX5_PERM_ATOMIC       : 0) |
	       (acc & IB_ACCESS_REMOTE_WRITE  ? MLX5_PERM_REMOTE_WRITE : 0) |
	       (acc & IB_ACCESS_REMOTE_READ   ? MLX5_PERM_REMOTE_READ  : 0) |
	       (acc & IB_ACCESS_LOCAL_WRITE   ? MLX5_PERM_LOCAL_WRITE  : 0) |
		MLX5_PERM_LOCAL_READ | MLX5_PERM_UMR_EN;
}

<<<<<<< HEAD
static void set_mkey_segment(struct mlx5_mkey_seg *seg, struct ib_send_wr *wr,
			     int li, int *writ)
{
	memset(seg, 0, sizeof(*seg));
	if (li) {
		seg->status = 1 << 6;
		return;
	}

	seg->flags = get_umr_flags(wr->wr.fast_reg.access_flags) |
		     MLX5_ACCESS_MODE_MTT;
	*writ = seg->flags & (MLX5_PERM_LOCAL_WRITE | IB_ACCESS_REMOTE_WRITE);
	seg->qpn_mkey7_0 = cpu_to_be32((wr->wr.fast_reg.rkey & 0xff) | 0xffffff00);
	seg->flags_pd = cpu_to_be32(MLX5_MKEY_REMOTE_INVAL);
	seg->start_addr = cpu_to_be64(wr->wr.fast_reg.iova_start);
	seg->len = cpu_to_be64(wr->wr.fast_reg.length);
	seg->xlt_oct_size = cpu_to_be32((wr->wr.fast_reg.page_list_len + 1) / 2);
	seg->log2_page_size = wr->wr.fast_reg.page_shift;
=======
static void set_reg_mkey_seg(struct mlx5_mkey_seg *seg,
			     struct mlx5_ib_mr *mr,
			     u32 key, int access)
{
	int ndescs = ALIGN(mr->ndescs, 8) >> 1;

	memset(seg, 0, sizeof(*seg));

	if (mr->access_mode == MLX5_MKC_ACCESS_MODE_MTT)
		seg->log2_page_size = ilog2(mr->ibmr.page_size);
	else if (mr->access_mode == MLX5_MKC_ACCESS_MODE_KLMS)
		/* KLMs take twice the size of MTTs */
		ndescs *= 2;

	seg->flags = get_umr_flags(access) | mr->access_mode;
	seg->qpn_mkey7_0 = cpu_to_be32((key & 0xff) | 0xffffff00);
	seg->flags_pd = cpu_to_be32(MLX5_MKEY_REMOTE_INVAL);
	seg->start_addr = cpu_to_be64(mr->ibmr.iova);
	seg->len = cpu_to_be64(mr->ibmr.length);
	seg->xlt_oct_size = cpu_to_be32(ndescs);
}

static void set_linv_mkey_seg(struct mlx5_mkey_seg *seg)
{
	memset(seg, 0, sizeof(*seg));
	seg->status = MLX5_MKEY_STATUS_FREE;
>>>>>>> v4.9.227
}

static void set_reg_mkey_segment(struct mlx5_mkey_seg *seg, struct ib_send_wr *wr)
{
<<<<<<< HEAD
	memset(seg, 0, sizeof(*seg));
	if (wr->send_flags & MLX5_IB_SEND_UMR_UNREG) {
		seg->status = 1 << 6;
		return;
	}

	seg->flags = convert_access(wr->wr.fast_reg.access_flags);
	seg->flags_pd = cpu_to_be32(to_mpd((struct ib_pd *)wr->wr.fast_reg.page_list)->pdn);
	seg->start_addr = cpu_to_be64(wr->wr.fast_reg.iova_start);
	seg->len = cpu_to_be64(wr->wr.fast_reg.length);
	seg->log2_page_size = wr->wr.fast_reg.page_shift;
	seg->qpn_mkey7_0 = cpu_to_be32(0xffffff00 |
				       mlx5_mkey_variant(wr->wr.fast_reg.rkey));
}

static void set_frwr_pages(struct mlx5_wqe_data_seg *dseg,
			   struct ib_send_wr *wr,
			   struct mlx5_core_dev *mdev,
			   struct mlx5_ib_pd *pd,
			   int writ)
{
	struct mlx5_ib_fast_reg_page_list *mfrpl = to_mfrpl(wr->wr.fast_reg.page_list);
	u64 *page_list = wr->wr.fast_reg.page_list->page_list;
	u64 perm = MLX5_EN_RD | (writ ? MLX5_EN_WR : 0);
	int i;

	for (i = 0; i < wr->wr.fast_reg.page_list_len; i++)
		mfrpl->mapped_page_list[i] = cpu_to_be64(page_list[i] | perm);
	dseg->addr = cpu_to_be64(mfrpl->map);
	dseg->byte_count = cpu_to_be32(ALIGN(sizeof(u64) * wr->wr.fast_reg.page_list_len, 64));
	dseg->lkey = cpu_to_be32(pd->pa_lkey);
=======
	struct mlx5_umr_wr *umrwr = umr_wr(wr);

	memset(seg, 0, sizeof(*seg));
	if (wr->send_flags & MLX5_IB_SEND_UMR_UNREG) {
		seg->status = MLX5_MKEY_STATUS_FREE;
		return;
	}

	seg->flags = convert_access(umrwr->access_flags);
	if (!(wr->send_flags & MLX5_IB_SEND_UMR_UPDATE_MTT)) {
		if (umrwr->pd)
			seg->flags_pd = cpu_to_be32(to_mpd(umrwr->pd)->pdn);
		seg->start_addr = cpu_to_be64(umrwr->target.virt_addr);
	}
	seg->len = cpu_to_be64(umrwr->length);
	seg->log2_page_size = umrwr->page_shift;
	seg->qpn_mkey7_0 = cpu_to_be32(0xffffff00 |
				       mlx5_mkey_variant(umrwr->mkey));
}

static void set_reg_data_seg(struct mlx5_wqe_data_seg *dseg,
			     struct mlx5_ib_mr *mr,
			     struct mlx5_ib_pd *pd)
{
	int bcount = mr->desc_size * mr->ndescs;

	dseg->addr = cpu_to_be64(mr->desc_map);
	dseg->byte_count = cpu_to_be32(ALIGN(bcount, 64));
	dseg->lkey = cpu_to_be32(pd->ibpd.local_dma_lkey);
>>>>>>> v4.9.227
}

static __be32 send_ieth(struct ib_send_wr *wr)
{
	switch (wr->opcode) {
	case IB_WR_SEND_WITH_IMM:
	case IB_WR_RDMA_WRITE_WITH_IMM:
		return wr->ex.imm_data;

	case IB_WR_SEND_WITH_INV:
		return cpu_to_be32(wr->ex.invalidate_rkey);

	default:
		return 0;
	}
}

static u8 calc_sig(void *wqe, int size)
{
	u8 *p = wqe;
	u8 res = 0;
	int i;

	for (i = 0; i < size; i++)
		res ^= p[i];

	return ~res;
}

static u8 wq_sig(void *wqe)
{
	return calc_sig(wqe, (*((u8 *)wqe + 8) & 0x3f) << 4);
}

static int set_data_inl_seg(struct mlx5_ib_qp *qp, struct ib_send_wr *wr,
			    void *wqe, int *sz)
{
	struct mlx5_wqe_inline_seg *seg;
	void *qend = qp->sq.qend;
	void *addr;
	int inl = 0;
	int copy;
	int len;
	int i;

	seg = wqe;
	wqe += sizeof(*seg);
	for (i = 0; i < wr->num_sge; i++) {
		addr = (void *)(unsigned long)(wr->sg_list[i].addr);
		len  = wr->sg_list[i].length;
		inl += len;

		if (unlikely(inl > qp->max_inline_data))
			return -ENOMEM;

		if (unlikely(wqe + len > qend)) {
			copy = qend - wqe;
			memcpy(wqe, addr, copy);
			addr += copy;
			len -= copy;
			wqe = mlx5_get_send_wqe(qp, 0);
		}
		memcpy(wqe, addr, len);
		wqe += len;
	}

	seg->byte_count = cpu_to_be32(inl | MLX5_INLINE_SEG);

	*sz = ALIGN(inl + sizeof(seg->byte_count), 16) / 16;

	return 0;
}

static u16 prot_field_size(enum ib_signature_type type)
{
	switch (type) {
	case IB_SIG_TYPE_T10_DIF:
		return MLX5_DIF_SIZE;
	default:
		return 0;
	}
}

static u8 bs_selector(int block_size)
{
	switch (block_size) {
	case 512:	    return 0x1;
	case 520:	    return 0x2;
	case 4096:	    return 0x3;
	case 4160:	    return 0x4;
	case 1073741824:    return 0x5;
	default:	    return 0;
	}
}

static void mlx5_fill_inl_bsf(struct ib_sig_domain *domain,
			      struct mlx5_bsf_inl *inl)
{
	/* Valid inline section and allow BSF refresh */
	inl->vld_refresh = cpu_to_be16(MLX5_BSF_INL_VALID |
				       MLX5_BSF_REFRESH_DIF);
	inl->dif_apptag = cpu_to_be16(domain->sig.dif.app_tag);
	inl->dif_reftag = cpu_to_be32(domain->sig.dif.ref_tag);
	/* repeating block */
	inl->rp_inv_seed = MLX5_BSF_REPEAT_BLOCK;
	inl->sig_type = domain->sig.dif.bg_type == IB_T10DIF_CRC ?
			MLX5_DIF_CRC : MLX5_DIF_IPCS;

	if (domain->sig.dif.ref_remap)
		inl->dif_inc_ref_guard_check |= MLX5_BSF_INC_REFTAG;

	if (domain->sig.dif.app_escape) {
		if (domain->sig.dif.ref_escape)
			inl->dif_inc_ref_guard_check |= MLX5_BSF_APPREF_ESCAPE;
		else
			inl->dif_inc_ref_guard_check |= MLX5_BSF_APPTAG_ESCAPE;
	}

	inl->dif_app_bitmask_check =
		cpu_to_be16(domain->sig.dif.apptag_check_mask);
}

static int mlx5_set_bsf(struct ib_mr *sig_mr,
			struct ib_sig_attrs *sig_attrs,
			struct mlx5_bsf *bsf, u32 data_size)
{
	struct mlx5_core_sig_ctx *msig = to_mmr(sig_mr)->sig;
	struct mlx5_bsf_basic *basic = &bsf->basic;
	struct ib_sig_domain *mem = &sig_attrs->mem;
	struct ib_sig_domain *wire = &sig_attrs->wire;

	memset(bsf, 0, sizeof(*bsf));

	/* Basic + Extended + Inline */
	basic->bsf_size_sbs = 1 << 7;
	/* Input domain check byte mask */
	basic->check_byte_mask = sig_attrs->check_mask;
	basic->raw_data_size = cpu_to_be32(data_size);

	/* Memory domain */
	switch (sig_attrs->mem.sig_type) {
	case IB_SIG_TYPE_NONE:
		break;
	case IB_SIG_TYPE_T10_DIF:
		basic->mem.bs_selector = bs_selector(mem->sig.dif.pi_interval);
		basic->m_bfs_psv = cpu_to_be32(msig->psv_memory.psv_idx);
		mlx5_fill_inl_bsf(mem, &bsf->m_inl);
		break;
	default:
		return -EINVAL;
	}

	/* Wire domain */
	switch (sig_attrs->wire.sig_type) {
	case IB_SIG_TYPE_NONE:
		break;
	case IB_SIG_TYPE_T10_DIF:
		if (mem->sig.dif.pi_interval == wire->sig.dif.pi_interval &&
		    mem->sig_type == wire->sig_type) {
			/* Same block structure */
			basic->bsf_size_sbs |= 1 << 4;
			if (mem->sig.dif.bg_type == wire->sig.dif.bg_type)
				basic->wire.copy_byte_mask |= MLX5_CPY_GRD_MASK;
			if (mem->sig.dif.app_tag == wire->sig.dif.app_tag)
				basic->wire.copy_byte_mask |= MLX5_CPY_APP_MASK;
			if (mem->sig.dif.ref_tag == wire->sig.dif.ref_tag)
				basic->wire.copy_byte_mask |= MLX5_CPY_REF_MASK;
		} else
			basic->wire.bs_selector = bs_selector(wire->sig.dif.pi_interval);

		basic->w_bfs_psv = cpu_to_be32(msig->psv_wire.psv_idx);
		mlx5_fill_inl_bsf(wire, &bsf->w_inl);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

<<<<<<< HEAD
static int set_sig_data_segment(struct ib_send_wr *wr, struct mlx5_ib_qp *qp,
				void **seg, int *size)
{
	struct ib_sig_attrs *sig_attrs = wr->wr.sig_handover.sig_attrs;
	struct ib_mr *sig_mr = wr->wr.sig_handover.sig_mr;
	struct mlx5_bsf *bsf;
	u32 data_len = wr->sg_list->length;
	u32 data_key = wr->sg_list->lkey;
	u64 data_va = wr->sg_list->addr;
	int ret;
	int wqe_size;

	if (!wr->wr.sig_handover.prot ||
	    (data_key == wr->wr.sig_handover.prot->lkey &&
	     data_va == wr->wr.sig_handover.prot->addr &&
	     data_len == wr->wr.sig_handover.prot->length)) {
=======
static int set_sig_data_segment(struct ib_sig_handover_wr *wr,
				struct mlx5_ib_qp *qp, void **seg, int *size)
{
	struct ib_sig_attrs *sig_attrs = wr->sig_attrs;
	struct ib_mr *sig_mr = wr->sig_mr;
	struct mlx5_bsf *bsf;
	u32 data_len = wr->wr.sg_list->length;
	u32 data_key = wr->wr.sg_list->lkey;
	u64 data_va = wr->wr.sg_list->addr;
	int ret;
	int wqe_size;

	if (!wr->prot ||
	    (data_key == wr->prot->lkey &&
	     data_va == wr->prot->addr &&
	     data_len == wr->prot->length)) {
>>>>>>> v4.9.227
		/**
		 * Source domain doesn't contain signature information
		 * or data and protection are interleaved in memory.
		 * So need construct:
		 *                  ------------------
		 *                 |     data_klm     |
		 *                  ------------------
		 *                 |       BSF        |
		 *                  ------------------
		 **/
		struct mlx5_klm *data_klm = *seg;

		data_klm->bcount = cpu_to_be32(data_len);
		data_klm->key = cpu_to_be32(data_key);
		data_klm->va = cpu_to_be64(data_va);
		wqe_size = ALIGN(sizeof(*data_klm), 64);
	} else {
		/**
		 * Source domain contains signature information
		 * So need construct a strided block format:
		 *               ---------------------------
		 *              |     stride_block_ctrl     |
		 *               ---------------------------
		 *              |          data_klm         |
		 *               ---------------------------
		 *              |          prot_klm         |
		 *               ---------------------------
		 *              |             BSF           |
		 *               ---------------------------
		 **/
		struct mlx5_stride_block_ctrl_seg *sblock_ctrl;
		struct mlx5_stride_block_entry *data_sentry;
		struct mlx5_stride_block_entry *prot_sentry;
<<<<<<< HEAD
		u32 prot_key = wr->wr.sig_handover.prot->lkey;
		u64 prot_va = wr->wr.sig_handover.prot->addr;
=======
		u32 prot_key = wr->prot->lkey;
		u64 prot_va = wr->prot->addr;
>>>>>>> v4.9.227
		u16 block_size = sig_attrs->mem.sig.dif.pi_interval;
		int prot_size;

		sblock_ctrl = *seg;
		data_sentry = (void *)sblock_ctrl + sizeof(*sblock_ctrl);
		prot_sentry = (void *)data_sentry + sizeof(*data_sentry);

		prot_size = prot_field_size(sig_attrs->mem.sig_type);
		if (!prot_size) {
			pr_err("Bad block size given: %u\n", block_size);
			return -EINVAL;
		}
		sblock_ctrl->bcount_per_cycle = cpu_to_be32(block_size +
							    prot_size);
		sblock_ctrl->op = cpu_to_be32(MLX5_STRIDE_BLOCK_OP);
		sblock_ctrl->repeat_count = cpu_to_be32(data_len / block_size);
		sblock_ctrl->num_entries = cpu_to_be16(2);

		data_sentry->bcount = cpu_to_be16(block_size);
		data_sentry->key = cpu_to_be32(data_key);
		data_sentry->va = cpu_to_be64(data_va);
		data_sentry->stride = cpu_to_be16(block_size);

		prot_sentry->bcount = cpu_to_be16(prot_size);
		prot_sentry->key = cpu_to_be32(prot_key);
		prot_sentry->va = cpu_to_be64(prot_va);
		prot_sentry->stride = cpu_to_be16(prot_size);

		wqe_size = ALIGN(sizeof(*sblock_ctrl) + sizeof(*data_sentry) +
				 sizeof(*prot_sentry), 64);
	}

	*seg += wqe_size;
	*size += wqe_size / 16;
	if (unlikely((*seg == qp->sq.qend)))
		*seg = mlx5_get_send_wqe(qp, 0);

	bsf = *seg;
	ret = mlx5_set_bsf(sig_mr, sig_attrs, bsf, data_len);
	if (ret)
		return -EINVAL;

	*seg += sizeof(*bsf);
	*size += sizeof(*bsf) / 16;
	if (unlikely((*seg == qp->sq.qend)))
		*seg = mlx5_get_send_wqe(qp, 0);

	return 0;
}

static void set_sig_mkey_segment(struct mlx5_mkey_seg *seg,
<<<<<<< HEAD
				 struct ib_send_wr *wr, u32 nelements,
				 u32 length, u32 pdn)
{
	struct ib_mr *sig_mr = wr->wr.sig_handover.sig_mr;
=======
				 struct ib_sig_handover_wr *wr, u32 nelements,
				 u32 length, u32 pdn)
{
	struct ib_mr *sig_mr = wr->sig_mr;
>>>>>>> v4.9.227
	u32 sig_key = sig_mr->rkey;
	u8 sigerr = to_mmr(sig_mr)->sig->sigerr_count & 1;

	memset(seg, 0, sizeof(*seg));

<<<<<<< HEAD
	seg->flags = get_umr_flags(wr->wr.sig_handover.access_flags) |
				   MLX5_ACCESS_MODE_KLM;
=======
	seg->flags = get_umr_flags(wr->access_flags) |
				   MLX5_MKC_ACCESS_MODE_KLMS;
>>>>>>> v4.9.227
	seg->qpn_mkey7_0 = cpu_to_be32((sig_key & 0xff) | 0xffffff00);
	seg->flags_pd = cpu_to_be32(MLX5_MKEY_REMOTE_INVAL | sigerr << 26 |
				    MLX5_MKEY_BSF_EN | pdn);
	seg->len = cpu_to_be64(length);
	seg->xlt_oct_size = cpu_to_be32(be16_to_cpu(get_klm_octo(nelements)));
	seg->bsfs_octo_size = cpu_to_be32(MLX5_MKEY_BSF_OCTO_SIZE);
}

static void set_sig_umr_segment(struct mlx5_wqe_umr_ctrl_seg *umr,
<<<<<<< HEAD
				struct ib_send_wr *wr, u32 nelements)
=======
				u32 nelements)
>>>>>>> v4.9.227
{
	memset(umr, 0, sizeof(*umr));

	umr->flags = MLX5_FLAGS_INLINE | MLX5_FLAGS_CHECK_FREE;
	umr->klm_octowords = get_klm_octo(nelements);
	umr->bsf_octowords = cpu_to_be16(MLX5_MKEY_BSF_OCTO_SIZE);
	umr->mkey_mask = sig_mkey_mask();
}


<<<<<<< HEAD
static int set_sig_umr_wr(struct ib_send_wr *wr, struct mlx5_ib_qp *qp,
			  void **seg, int *size)
{
	struct mlx5_ib_mr *sig_mr = to_mmr(wr->wr.sig_handover.sig_mr);
=======
static int set_sig_umr_wr(struct ib_send_wr *send_wr, struct mlx5_ib_qp *qp,
			  void **seg, int *size)
{
	struct ib_sig_handover_wr *wr = sig_handover_wr(send_wr);
	struct mlx5_ib_mr *sig_mr = to_mmr(wr->sig_mr);
>>>>>>> v4.9.227
	u32 pdn = get_pd(qp)->pdn;
	u32 klm_oct_size;
	int region_len, ret;

<<<<<<< HEAD
	if (unlikely(wr->num_sge != 1) ||
	    unlikely(wr->wr.sig_handover.access_flags &
		     IB_ACCESS_REMOTE_ATOMIC) ||
=======
	if (unlikely(wr->wr.num_sge != 1) ||
	    unlikely(wr->access_flags & IB_ACCESS_REMOTE_ATOMIC) ||
>>>>>>> v4.9.227
	    unlikely(!sig_mr->sig) || unlikely(!qp->signature_en) ||
	    unlikely(!sig_mr->sig->sig_status_checked))
		return -EINVAL;

	/* length of the protected region, data + protection */
<<<<<<< HEAD
	region_len = wr->sg_list->length;
	if (wr->wr.sig_handover.prot &&
	    (wr->wr.sig_handover.prot->lkey != wr->sg_list->lkey  ||
	     wr->wr.sig_handover.prot->addr != wr->sg_list->addr  ||
	     wr->wr.sig_handover.prot->length != wr->sg_list->length))
		region_len += wr->wr.sig_handover.prot->length;
=======
	region_len = wr->wr.sg_list->length;
	if (wr->prot &&
	    (wr->prot->lkey != wr->wr.sg_list->lkey  ||
	     wr->prot->addr != wr->wr.sg_list->addr  ||
	     wr->prot->length != wr->wr.sg_list->length))
		region_len += wr->prot->length;
>>>>>>> v4.9.227

	/**
	 * KLM octoword size - if protection was provided
	 * then we use strided block format (3 octowords),
	 * else we use single KLM (1 octoword)
	 **/
<<<<<<< HEAD
	klm_oct_size = wr->wr.sig_handover.prot ? 3 : 1;

	set_sig_umr_segment(*seg, wr, klm_oct_size);
=======
	klm_oct_size = wr->prot ? 3 : 1;

	set_sig_umr_segment(*seg, klm_oct_size);
>>>>>>> v4.9.227
	*seg += sizeof(struct mlx5_wqe_umr_ctrl_seg);
	*size += sizeof(struct mlx5_wqe_umr_ctrl_seg) / 16;
	if (unlikely((*seg == qp->sq.qend)))
		*seg = mlx5_get_send_wqe(qp, 0);

	set_sig_mkey_segment(*seg, wr, klm_oct_size, region_len, pdn);
	*seg += sizeof(struct mlx5_mkey_seg);
	*size += sizeof(struct mlx5_mkey_seg) / 16;
	if (unlikely((*seg == qp->sq.qend)))
		*seg = mlx5_get_send_wqe(qp, 0);

	ret = set_sig_data_segment(wr, qp, seg, size);
	if (ret)
		return ret;

	sig_mr->sig->sig_status_checked = false;
	return 0;
}

static int set_psv_wr(struct ib_sig_domain *domain,
		      u32 psv_idx, void **seg, int *size)
{
	struct mlx5_seg_set_psv *psv_seg = *seg;

	memset(psv_seg, 0, sizeof(*psv_seg));
	psv_seg->psv_num = cpu_to_be32(psv_idx);
	switch (domain->sig_type) {
	case IB_SIG_TYPE_NONE:
		break;
	case IB_SIG_TYPE_T10_DIF:
		psv_seg->transient_sig = cpu_to_be32(domain->sig.dif.bg << 16 |
						     domain->sig.dif.app_tag);
		psv_seg->ref_tag = cpu_to_be32(domain->sig.dif.ref_tag);
		break;
	default:
		pr_err("Bad signature type given.\n");
		return 1;
	}

	*seg += sizeof(*psv_seg);
	*size += sizeof(*psv_seg) / 16;

	return 0;
}

<<<<<<< HEAD
static int set_frwr_li_wr(void **seg, struct ib_send_wr *wr, int *size,
			  struct mlx5_core_dev *mdev, struct mlx5_ib_pd *pd, struct mlx5_ib_qp *qp)
{
	int writ = 0;
	int li;

	li = wr->opcode == IB_WR_LOCAL_INV ? 1 : 0;
	if (unlikely(wr->send_flags & IB_SEND_INLINE))
		return -EINVAL;

	set_frwr_umr_segment(*seg, wr, li);
=======
static int set_reg_wr(struct mlx5_ib_qp *qp,
		      struct ib_reg_wr *wr,
		      void **seg, int *size)
{
	struct mlx5_ib_mr *mr = to_mmr(wr->mr);
	struct mlx5_ib_pd *pd = to_mpd(qp->ibqp.pd);

	if (unlikely(wr->wr.send_flags & IB_SEND_INLINE)) {
		mlx5_ib_warn(to_mdev(qp->ibqp.device),
			     "Invalid IB_SEND_INLINE send flag\n");
		return -EINVAL;
	}

	set_reg_umr_seg(*seg, mr);
>>>>>>> v4.9.227
	*seg += sizeof(struct mlx5_wqe_umr_ctrl_seg);
	*size += sizeof(struct mlx5_wqe_umr_ctrl_seg) / 16;
	if (unlikely((*seg == qp->sq.qend)))
		*seg = mlx5_get_send_wqe(qp, 0);
<<<<<<< HEAD
	set_mkey_segment(*seg, wr, li, &writ);
=======

	set_reg_mkey_seg(*seg, mr, wr->key, wr->access);
>>>>>>> v4.9.227
	*seg += sizeof(struct mlx5_mkey_seg);
	*size += sizeof(struct mlx5_mkey_seg) / 16;
	if (unlikely((*seg == qp->sq.qend)))
		*seg = mlx5_get_send_wqe(qp, 0);
<<<<<<< HEAD
	if (!li) {
		if (unlikely(wr->wr.fast_reg.page_list_len >
			     wr->wr.fast_reg.page_list->max_page_list_len))
			return	-ENOMEM;

		set_frwr_pages(*seg, wr, mdev, pd, writ);
		*seg += sizeof(struct mlx5_wqe_data_seg);
		*size += (sizeof(struct mlx5_wqe_data_seg) / 16);
	}
	return 0;
}

=======

	set_reg_data_seg(*seg, mr, pd);
	*seg += sizeof(struct mlx5_wqe_data_seg);
	*size += (sizeof(struct mlx5_wqe_data_seg) / 16);

	return 0;
}

static void set_linv_wr(struct mlx5_ib_qp *qp, void **seg, int *size)
{
	set_linv_umr_seg(*seg);
	*seg += sizeof(struct mlx5_wqe_umr_ctrl_seg);
	*size += sizeof(struct mlx5_wqe_umr_ctrl_seg) / 16;
	if (unlikely((*seg == qp->sq.qend)))
		*seg = mlx5_get_send_wqe(qp, 0);
	set_linv_mkey_seg(*seg);
	*seg += sizeof(struct mlx5_mkey_seg);
	*size += sizeof(struct mlx5_mkey_seg) / 16;
	if (unlikely((*seg == qp->sq.qend)))
		*seg = mlx5_get_send_wqe(qp, 0);
}

>>>>>>> v4.9.227
static void dump_wqe(struct mlx5_ib_qp *qp, int idx, int size_16)
{
	__be32 *p = NULL;
	int tidx = idx;
	int i, j;

	pr_debug("dump wqe at %p\n", mlx5_get_send_wqe(qp, tidx));
	for (i = 0, j = 0; i < size_16 * 4; i += 4, j += 4) {
		if ((i & 0xf) == 0) {
			void *buf = mlx5_get_send_wqe(qp, tidx);
			tidx = (tidx + 1) & (qp->sq.wqe_cnt - 1);
			p = buf;
			j = 0;
		}
		pr_debug("%08x %08x %08x %08x\n", be32_to_cpu(p[j]),
			 be32_to_cpu(p[j + 1]), be32_to_cpu(p[j + 2]),
			 be32_to_cpu(p[j + 3]));
	}
}

static void mlx5_bf_copy(u64 __iomem *dst, u64 *src,
			 unsigned bytecnt, struct mlx5_ib_qp *qp)
{
	while (bytecnt > 0) {
		__iowrite64_copy(dst++, src++, 8);
		__iowrite64_copy(dst++, src++, 8);
		__iowrite64_copy(dst++, src++, 8);
		__iowrite64_copy(dst++, src++, 8);
		__iowrite64_copy(dst++, src++, 8);
		__iowrite64_copy(dst++, src++, 8);
		__iowrite64_copy(dst++, src++, 8);
		__iowrite64_copy(dst++, src++, 8);
		bytecnt -= 64;
		if (unlikely(src == qp->sq.qend))
			src = mlx5_get_send_wqe(qp, 0);
	}
}

<<<<<<< HEAD
static u8 get_fence(u8 fence, struct ib_send_wr *wr)
{
	if (unlikely(wr->opcode == IB_WR_LOCAL_INV &&
		     wr->send_flags & IB_SEND_FENCE))
		return MLX5_FENCE_MODE_STRONG_ORDERING;

	if (unlikely(fence)) {
		if (wr->send_flags & IB_SEND_FENCE)
			return MLX5_FENCE_MODE_SMALL_AND_FENCE;
		else
			return fence;

	} else {
		return 0;
	}
}

static int begin_wqe(struct mlx5_ib_qp *qp, void **seg,
		     struct mlx5_wqe_ctrl_seg **ctrl,
		     struct ib_send_wr *wr, int *idx,
		     int *size, int nreq)
{
	int err = 0;

	if (unlikely(mlx5_wq_overflow(&qp->sq, nreq, qp->ibqp.send_cq))) {
		err = -ENOMEM;
		return err;
	}
=======
static int begin_wqe(struct mlx5_ib_qp *qp, void **seg,
		     struct mlx5_wqe_ctrl_seg **ctrl,
		     struct ib_send_wr *wr, unsigned *idx,
		     int *size, int nreq)
{
	if (unlikely(mlx5_wq_overflow(&qp->sq, nreq, qp->ibqp.send_cq)))
		return -ENOMEM;
>>>>>>> v4.9.227

	*idx = qp->sq.cur_post & (qp->sq.wqe_cnt - 1);
	*seg = mlx5_get_send_wqe(qp, *idx);
	*ctrl = *seg;
	*(uint32_t *)(*seg + 8) = 0;
	(*ctrl)->imm = send_ieth(wr);
	(*ctrl)->fm_ce_se = qp->sq_signal_bits |
		(wr->send_flags & IB_SEND_SIGNALED ?
		 MLX5_WQE_CTRL_CQ_UPDATE : 0) |
		(wr->send_flags & IB_SEND_SOLICITED ?
		 MLX5_WQE_CTRL_SOLICITED : 0);

	*seg += sizeof(**ctrl);
	*size = sizeof(**ctrl) / 16;

<<<<<<< HEAD
	return err;
=======
	return 0;
>>>>>>> v4.9.227
}

static void finish_wqe(struct mlx5_ib_qp *qp,
		       struct mlx5_wqe_ctrl_seg *ctrl,
		       u8 size, unsigned idx, u64 wr_id,
<<<<<<< HEAD
		       int nreq, u8 fence, u8 next_fence,
		       u32 mlx5_opcode)
=======
		       int nreq, u8 fence, u32 mlx5_opcode)
>>>>>>> v4.9.227
{
	u8 opmod = 0;

	ctrl->opmod_idx_opcode = cpu_to_be32(((u32)(qp->sq.cur_post) << 8) |
					     mlx5_opcode | ((u32)opmod << 24));
<<<<<<< HEAD
	ctrl->qpn_ds = cpu_to_be32(size | (qp->mqp.qpn << 8));
	ctrl->fm_ce_se |= fence;
	qp->fm_cache = next_fence;
=======
	ctrl->qpn_ds = cpu_to_be32(size | (qp->trans_qp.base.mqp.qpn << 8));
	ctrl->fm_ce_se |= fence;
>>>>>>> v4.9.227
	if (unlikely(qp->wq_sig))
		ctrl->signature = wq_sig(ctrl);

	qp->sq.wrid[idx] = wr_id;
	qp->sq.w_list[idx].opcode = mlx5_opcode;
	qp->sq.wqe_head[idx] = qp->sq.head + nreq;
	qp->sq.cur_post += DIV_ROUND_UP(size * 16, MLX5_SEND_WQE_BB);
	qp->sq.w_list[idx].next = qp->sq.cur_post;
}


int mlx5_ib_post_send(struct ib_qp *ibqp, struct ib_send_wr *wr,
		      struct ib_send_wr **bad_wr)
{
	struct mlx5_wqe_ctrl_seg *ctrl = NULL;  /* compiler warning */
	struct mlx5_ib_dev *dev = to_mdev(ibqp->device);
	struct mlx5_core_dev *mdev = dev->mdev;
<<<<<<< HEAD
	struct mlx5_ib_qp *qp = to_mqp(ibqp);
	struct mlx5_ib_mr *mr;
	struct mlx5_wqe_data_seg *dpseg;
	struct mlx5_wqe_xrc_seg *xrc;
	struct mlx5_bf *bf = qp->bf;
	int uninitialized_var(size);
	void *qend = qp->sq.qend;
=======
	struct mlx5_ib_qp *qp;
	struct mlx5_ib_mr *mr;
	struct mlx5_wqe_data_seg *dpseg;
	struct mlx5_wqe_xrc_seg *xrc;
	struct mlx5_bf *bf;
	int uninitialized_var(size);
	void *qend;
>>>>>>> v4.9.227
	unsigned long flags;
	unsigned idx;
	int err = 0;
	int inl = 0;
	int num_sge;
	void *seg;
	int nreq;
	int i;
	u8 next_fence = 0;
	u8 fence;

<<<<<<< HEAD
	spin_lock_irqsave(&qp->sq.lock, flags);

=======
	if (unlikely(ibqp->qp_type == IB_QPT_GSI))
		return mlx5_ib_gsi_post_send(ibqp, wr, bad_wr);

	qp = to_mqp(ibqp);
	bf = qp->bf;
	qend = qp->sq.qend;

	spin_lock_irqsave(&qp->sq.lock, flags);

	if (mdev->state == MLX5_DEVICE_STATE_INTERNAL_ERROR) {
		err = -EIO;
		*bad_wr = wr;
		nreq = 0;
		goto out;
	}

>>>>>>> v4.9.227
	for (nreq = 0; wr; nreq++, wr = wr->next) {
		if (unlikely(wr->opcode >= ARRAY_SIZE(mlx5_ib_opcode))) {
			mlx5_ib_warn(dev, "\n");
			err = -EINVAL;
			*bad_wr = wr;
			goto out;
		}

<<<<<<< HEAD
		fence = qp->fm_cache;
		num_sge = wr->num_sge;
		if (unlikely(num_sge > qp->sq.max_gs)) {
			mlx5_ib_warn(dev, "\n");
			err = -ENOMEM;
=======
		num_sge = wr->num_sge;
		if (unlikely(num_sge > qp->sq.max_gs)) {
			mlx5_ib_warn(dev, "\n");
			err = -EINVAL;
>>>>>>> v4.9.227
			*bad_wr = wr;
			goto out;
		}

		err = begin_wqe(qp, &seg, &ctrl, wr, &idx, &size, nreq);
		if (err) {
			mlx5_ib_warn(dev, "\n");
			err = -ENOMEM;
			*bad_wr = wr;
			goto out;
		}

<<<<<<< HEAD
		switch (ibqp->qp_type) {
		case IB_QPT_XRC_INI:
			xrc = seg;
			xrc->xrc_srqn = htonl(wr->xrc_remote_srq_num);
=======
		if (wr->opcode == IB_WR_REG_MR) {
			fence = dev->umr_fence;
			next_fence = MLX5_FENCE_MODE_INITIATOR_SMALL;
		} else  {
			if (wr->send_flags & IB_SEND_FENCE) {
				if (qp->next_fence)
					fence = MLX5_FENCE_MODE_SMALL_AND_FENCE;
				else
					fence = MLX5_FENCE_MODE_FENCE;
			} else {
				fence = qp->next_fence;
			}
		}

		switch (ibqp->qp_type) {
		case IB_QPT_XRC_INI:
			xrc = seg;
>>>>>>> v4.9.227
			seg += sizeof(*xrc);
			size += sizeof(*xrc) / 16;
			/* fall through */
		case IB_QPT_RC:
			switch (wr->opcode) {
			case IB_WR_RDMA_READ:
			case IB_WR_RDMA_WRITE:
			case IB_WR_RDMA_WRITE_WITH_IMM:
<<<<<<< HEAD
				set_raddr_seg(seg, wr->wr.rdma.remote_addr,
					      wr->wr.rdma.rkey);
=======
				set_raddr_seg(seg, rdma_wr(wr)->remote_addr,
					      rdma_wr(wr)->rkey);
>>>>>>> v4.9.227
				seg += sizeof(struct mlx5_wqe_raddr_seg);
				size += sizeof(struct mlx5_wqe_raddr_seg) / 16;
				break;

			case IB_WR_ATOMIC_CMP_AND_SWP:
			case IB_WR_ATOMIC_FETCH_AND_ADD:
			case IB_WR_MASKED_ATOMIC_CMP_AND_SWP:
				mlx5_ib_warn(dev, "Atomic operations are not supported yet\n");
				err = -ENOSYS;
				*bad_wr = wr;
				goto out;

			case IB_WR_LOCAL_INV:
<<<<<<< HEAD
				next_fence = MLX5_FENCE_MODE_INITIATOR_SMALL;
				qp->sq.wr_data[idx] = IB_WR_LOCAL_INV;
				ctrl->imm = cpu_to_be32(wr->ex.invalidate_rkey);
				err = set_frwr_li_wr(&seg, wr, &size, mdev, to_mpd(ibqp->pd), qp);
				if (err) {
					mlx5_ib_warn(dev, "\n");
					*bad_wr = wr;
					goto out;
				}
				num_sge = 0;
				break;

			case IB_WR_FAST_REG_MR:
				next_fence = MLX5_FENCE_MODE_INITIATOR_SMALL;
				qp->sq.wr_data[idx] = IB_WR_FAST_REG_MR;
				ctrl->imm = cpu_to_be32(wr->wr.fast_reg.rkey);
				err = set_frwr_li_wr(&seg, wr, &size, mdev, to_mpd(ibqp->pd), qp);
				if (err) {
					mlx5_ib_warn(dev, "\n");
=======
				qp->sq.wr_data[idx] = IB_WR_LOCAL_INV;
				ctrl->imm = cpu_to_be32(wr->ex.invalidate_rkey);
				set_linv_wr(qp, &seg, &size);
				num_sge = 0;
				break;

			case IB_WR_REG_MR:
				qp->sq.wr_data[idx] = IB_WR_REG_MR;
				ctrl->imm = cpu_to_be32(reg_wr(wr)->key);
				err = set_reg_wr(qp, reg_wr(wr), &seg, &size);
				if (err) {
>>>>>>> v4.9.227
					*bad_wr = wr;
					goto out;
				}
				num_sge = 0;
				break;

			case IB_WR_REG_SIG_MR:
				qp->sq.wr_data[idx] = IB_WR_REG_SIG_MR;
<<<<<<< HEAD
				mr = to_mmr(wr->wr.sig_handover.sig_mr);
=======
				mr = to_mmr(sig_handover_wr(wr)->sig_mr);
>>>>>>> v4.9.227

				ctrl->imm = cpu_to_be32(mr->ibmr.rkey);
				err = set_sig_umr_wr(wr, qp, &seg, &size);
				if (err) {
					mlx5_ib_warn(dev, "\n");
					*bad_wr = wr;
					goto out;
				}

<<<<<<< HEAD
				finish_wqe(qp, ctrl, size, idx, wr->wr_id,
					   nreq, get_fence(fence, wr),
					   next_fence, MLX5_OPCODE_UMR);
=======
				finish_wqe(qp, ctrl, size, idx, wr->wr_id, nreq,
					   fence, MLX5_OPCODE_UMR);
>>>>>>> v4.9.227
				/*
				 * SET_PSV WQEs are not signaled and solicited
				 * on error
				 */
				wr->send_flags &= ~IB_SEND_SIGNALED;
				wr->send_flags |= IB_SEND_SOLICITED;
				err = begin_wqe(qp, &seg, &ctrl, wr,
						&idx, &size, nreq);
				if (err) {
					mlx5_ib_warn(dev, "\n");
					err = -ENOMEM;
					*bad_wr = wr;
					goto out;
				}

<<<<<<< HEAD
				err = set_psv_wr(&wr->wr.sig_handover.sig_attrs->mem,
=======
				err = set_psv_wr(&sig_handover_wr(wr)->sig_attrs->mem,
>>>>>>> v4.9.227
						 mr->sig->psv_memory.psv_idx, &seg,
						 &size);
				if (err) {
					mlx5_ib_warn(dev, "\n");
					*bad_wr = wr;
					goto out;
				}

<<<<<<< HEAD
				finish_wqe(qp, ctrl, size, idx, wr->wr_id,
					   nreq, get_fence(fence, wr),
					   next_fence, MLX5_OPCODE_SET_PSV);
=======
				finish_wqe(qp, ctrl, size, idx, wr->wr_id, nreq,
					   fence, MLX5_OPCODE_SET_PSV);
>>>>>>> v4.9.227
				err = begin_wqe(qp, &seg, &ctrl, wr,
						&idx, &size, nreq);
				if (err) {
					mlx5_ib_warn(dev, "\n");
					err = -ENOMEM;
					*bad_wr = wr;
					goto out;
				}

<<<<<<< HEAD
				next_fence = MLX5_FENCE_MODE_INITIATOR_SMALL;
				err = set_psv_wr(&wr->wr.sig_handover.sig_attrs->wire,
=======
				err = set_psv_wr(&sig_handover_wr(wr)->sig_attrs->wire,
>>>>>>> v4.9.227
						 mr->sig->psv_wire.psv_idx, &seg,
						 &size);
				if (err) {
					mlx5_ib_warn(dev, "\n");
					*bad_wr = wr;
					goto out;
				}

<<<<<<< HEAD
				finish_wqe(qp, ctrl, size, idx, wr->wr_id,
					   nreq, get_fence(fence, wr),
					   next_fence, MLX5_OPCODE_SET_PSV);
=======
				finish_wqe(qp, ctrl, size, idx, wr->wr_id, nreq,
					   fence, MLX5_OPCODE_SET_PSV);
				qp->next_fence = MLX5_FENCE_MODE_INITIATOR_SMALL;
>>>>>>> v4.9.227
				num_sge = 0;
				goto skip_psv;

			default:
				break;
			}
			break;

		case IB_QPT_UC:
			switch (wr->opcode) {
			case IB_WR_RDMA_WRITE:
			case IB_WR_RDMA_WRITE_WITH_IMM:
<<<<<<< HEAD
				set_raddr_seg(seg, wr->wr.rdma.remote_addr,
					      wr->wr.rdma.rkey);
=======
				set_raddr_seg(seg, rdma_wr(wr)->remote_addr,
					      rdma_wr(wr)->rkey);
>>>>>>> v4.9.227
				seg  += sizeof(struct mlx5_wqe_raddr_seg);
				size += sizeof(struct mlx5_wqe_raddr_seg) / 16;
				break;

			default:
				break;
			}
			break;

<<<<<<< HEAD
		case IB_QPT_UD:
		case IB_QPT_SMI:
		case IB_QPT_GSI:
=======
		case IB_QPT_SMI:
		case MLX5_IB_QPT_HW_GSI:
>>>>>>> v4.9.227
			set_datagram_seg(seg, wr);
			seg += sizeof(struct mlx5_wqe_datagram_seg);
			size += sizeof(struct mlx5_wqe_datagram_seg) / 16;
			if (unlikely((seg == qend)))
				seg = mlx5_get_send_wqe(qp, 0);
			break;
<<<<<<< HEAD

=======
		case IB_QPT_UD:
			set_datagram_seg(seg, wr);
			seg += sizeof(struct mlx5_wqe_datagram_seg);
			size += sizeof(struct mlx5_wqe_datagram_seg) / 16;

			if (unlikely((seg == qend)))
				seg = mlx5_get_send_wqe(qp, 0);

			/* handle qp that supports ud offload */
			if (qp->flags & IB_QP_CREATE_IPOIB_UD_LSO) {
				struct mlx5_wqe_eth_pad *pad;

				pad = seg;
				memset(pad, 0, sizeof(struct mlx5_wqe_eth_pad));
				seg += sizeof(struct mlx5_wqe_eth_pad);
				size += sizeof(struct mlx5_wqe_eth_pad) / 16;

				seg = set_eth_seg(seg, wr, qend, qp, &size);

				if (unlikely((seg == qend)))
					seg = mlx5_get_send_wqe(qp, 0);
			}
			break;
>>>>>>> v4.9.227
		case MLX5_IB_QPT_REG_UMR:
			if (wr->opcode != MLX5_IB_WR_UMR) {
				err = -EINVAL;
				mlx5_ib_warn(dev, "bad opcode\n");
				goto out;
			}
			qp->sq.wr_data[idx] = MLX5_IB_WR_UMR;
<<<<<<< HEAD
			ctrl->imm = cpu_to_be32(wr->wr.fast_reg.rkey);
=======
			ctrl->imm = cpu_to_be32(umr_wr(wr)->mkey);
>>>>>>> v4.9.227
			set_reg_umr_segment(seg, wr);
			seg += sizeof(struct mlx5_wqe_umr_ctrl_seg);
			size += sizeof(struct mlx5_wqe_umr_ctrl_seg) / 16;
			if (unlikely((seg == qend)))
				seg = mlx5_get_send_wqe(qp, 0);
			set_reg_mkey_segment(seg, wr);
			seg += sizeof(struct mlx5_mkey_seg);
			size += sizeof(struct mlx5_mkey_seg) / 16;
			if (unlikely((seg == qend)))
				seg = mlx5_get_send_wqe(qp, 0);
			break;

		default:
			break;
		}

		if (wr->send_flags & IB_SEND_INLINE && num_sge) {
			int uninitialized_var(sz);

			err = set_data_inl_seg(qp, wr, seg, &sz);
			if (unlikely(err)) {
				mlx5_ib_warn(dev, "\n");
				*bad_wr = wr;
				goto out;
			}
			inl = 1;
			size += sz;
		} else {
			dpseg = seg;
			for (i = 0; i < num_sge; i++) {
				if (unlikely(dpseg == qend)) {
					seg = mlx5_get_send_wqe(qp, 0);
					dpseg = seg;
				}
				if (likely(wr->sg_list[i].length)) {
					set_data_ptr_seg(dpseg, wr->sg_list + i);
					size += sizeof(struct mlx5_wqe_data_seg) / 16;
					dpseg++;
				}
			}
		}

<<<<<<< HEAD
		finish_wqe(qp, ctrl, size, idx, wr->wr_id, nreq,
			   get_fence(fence, wr), next_fence,
=======
		qp->next_fence = next_fence;
		finish_wqe(qp, ctrl, size, idx, wr->wr_id, nreq, fence,
>>>>>>> v4.9.227
			   mlx5_ib_opcode[wr->opcode]);
skip_psv:
		if (0)
			dump_wqe(qp, idx, size);
	}

out:
	if (likely(nreq)) {
		qp->sq.head += nreq;

		/* Make sure that descriptors are written before
		 * updating doorbell record and ringing the doorbell
		 */
		wmb();

		qp->db.db[MLX5_SND_DBR] = cpu_to_be32(qp->sq.cur_post);

		/* Make sure doorbell record is visible to the HCA before
		 * we hit doorbell */
		wmb();

		if (bf->need_lock)
			spin_lock(&bf->lock);
<<<<<<< HEAD
=======
		else
			__acquire(&bf->lock);
>>>>>>> v4.9.227

		/* TBD enable WC */
		if (0 && nreq == 1 && bf->uuarn && inl && size > 1 && size <= bf->buf_size / 16) {
			mlx5_bf_copy(bf->reg + bf->offset, (u64 *)ctrl, ALIGN(size * 16, 64), qp);
			/* wc_wmb(); */
		} else {
			mlx5_write64((__be32 *)ctrl, bf->regreg + bf->offset,
				     MLX5_GET_DOORBELL_LOCK(&bf->lock32));
			/* Make sure doorbells don't leak out of SQ spinlock
			 * and reach the HCA out of order.
			 */
			mmiowb();
		}
		bf->offset ^= bf->buf_size;
		if (bf->need_lock)
			spin_unlock(&bf->lock);
<<<<<<< HEAD
=======
		else
			__release(&bf->lock);
>>>>>>> v4.9.227
	}

	spin_unlock_irqrestore(&qp->sq.lock, flags);

	return err;
}

static void set_sig_seg(struct mlx5_rwqe_sig *sig, int size)
{
	sig->signature = calc_sig(sig, size);
}

int mlx5_ib_post_recv(struct ib_qp *ibqp, struct ib_recv_wr *wr,
		      struct ib_recv_wr **bad_wr)
{
	struct mlx5_ib_qp *qp = to_mqp(ibqp);
	struct mlx5_wqe_data_seg *scat;
	struct mlx5_rwqe_sig *sig;
<<<<<<< HEAD
=======
	struct mlx5_ib_dev *dev = to_mdev(ibqp->device);
	struct mlx5_core_dev *mdev = dev->mdev;
>>>>>>> v4.9.227
	unsigned long flags;
	int err = 0;
	int nreq;
	int ind;
	int i;

<<<<<<< HEAD
	spin_lock_irqsave(&qp->rq.lock, flags);

=======
	if (unlikely(ibqp->qp_type == IB_QPT_GSI))
		return mlx5_ib_gsi_post_recv(ibqp, wr, bad_wr);

	spin_lock_irqsave(&qp->rq.lock, flags);

	if (mdev->state == MLX5_DEVICE_STATE_INTERNAL_ERROR) {
		err = -EIO;
		*bad_wr = wr;
		nreq = 0;
		goto out;
	}

>>>>>>> v4.9.227
	ind = qp->rq.head & (qp->rq.wqe_cnt - 1);

	for (nreq = 0; wr; nreq++, wr = wr->next) {
		if (mlx5_wq_overflow(&qp->rq, nreq, qp->ibqp.recv_cq)) {
			err = -ENOMEM;
			*bad_wr = wr;
			goto out;
		}

		if (unlikely(wr->num_sge > qp->rq.max_gs)) {
			err = -EINVAL;
			*bad_wr = wr;
			goto out;
		}

		scat = get_recv_wqe(qp, ind);
		if (qp->wq_sig)
			scat++;

		for (i = 0; i < wr->num_sge; i++)
			set_data_ptr_seg(scat + i, wr->sg_list + i);

		if (i < qp->rq.max_gs) {
			scat[i].byte_count = 0;
			scat[i].lkey       = cpu_to_be32(MLX5_INVALID_LKEY);
			scat[i].addr       = 0;
		}

		if (qp->wq_sig) {
			sig = (struct mlx5_rwqe_sig *)scat;
			set_sig_seg(sig, (qp->rq.max_gs + 1) << 2);
		}

		qp->rq.wrid[ind] = wr->wr_id;

		ind = (ind + 1) & (qp->rq.wqe_cnt - 1);
	}

out:
	if (likely(nreq)) {
		qp->rq.head += nreq;

		/* Make sure that descriptors are written before
		 * doorbell record.
		 */
		wmb();

		*qp->db.db = cpu_to_be32(qp->rq.head & 0xffff);
	}

	spin_unlock_irqrestore(&qp->rq.lock, flags);

	return err;
}

static inline enum ib_qp_state to_ib_qp_state(enum mlx5_qp_state mlx5_state)
{
	switch (mlx5_state) {
	case MLX5_QP_STATE_RST:      return IB_QPS_RESET;
	case MLX5_QP_STATE_INIT:     return IB_QPS_INIT;
	case MLX5_QP_STATE_RTR:      return IB_QPS_RTR;
	case MLX5_QP_STATE_RTS:      return IB_QPS_RTS;
	case MLX5_QP_STATE_SQ_DRAINING:
	case MLX5_QP_STATE_SQD:      return IB_QPS_SQD;
	case MLX5_QP_STATE_SQER:     return IB_QPS_SQE;
	case MLX5_QP_STATE_ERR:      return IB_QPS_ERR;
	default:		     return -1;
	}
}

static inline enum ib_mig_state to_ib_mig_state(int mlx5_mig_state)
{
	switch (mlx5_mig_state) {
	case MLX5_QP_PM_ARMED:		return IB_MIG_ARMED;
	case MLX5_QP_PM_REARM:		return IB_MIG_REARM;
	case MLX5_QP_PM_MIGRATED:	return IB_MIG_MIGRATED;
	default: return -1;
	}
}

static int to_ib_qp_access_flags(int mlx5_flags)
{
	int ib_flags = 0;

	if (mlx5_flags & MLX5_QP_BIT_RRE)
		ib_flags |= IB_ACCESS_REMOTE_READ;
	if (mlx5_flags & MLX5_QP_BIT_RWE)
		ib_flags |= IB_ACCESS_REMOTE_WRITE;
	if (mlx5_flags & MLX5_QP_BIT_RAE)
		ib_flags |= IB_ACCESS_REMOTE_ATOMIC;

	return ib_flags;
}

static void to_ib_ah_attr(struct mlx5_ib_dev *ibdev, struct ib_ah_attr *ib_ah_attr,
				struct mlx5_qp_path *path)
{
	struct mlx5_core_dev *dev = ibdev->mdev;

	memset(ib_ah_attr, 0, sizeof(*ib_ah_attr));
	ib_ah_attr->port_num	  = path->port;

	if (ib_ah_attr->port_num == 0 ||
<<<<<<< HEAD
	    ib_ah_attr->port_num > dev->caps.gen.num_ports)
		return;

	ib_ah_attr->sl = path->sl & 0xf;
=======
	    ib_ah_attr->port_num > MLX5_CAP_GEN(dev, num_ports))
		return;

	ib_ah_attr->sl = path->dci_cfi_prio_sl & 0xf;
>>>>>>> v4.9.227

	ib_ah_attr->dlid	  = be16_to_cpu(path->rlid);
	ib_ah_attr->src_path_bits = path->grh_mlid & 0x7f;
	ib_ah_attr->static_rate   = path->static_rate ? path->static_rate - 5 : 0;
	ib_ah_attr->ah_flags      = (path->grh_mlid & (1 << 7)) ? IB_AH_GRH : 0;
	if (ib_ah_attr->ah_flags) {
		ib_ah_attr->grh.sgid_index = path->mgid_index;
		ib_ah_attr->grh.hop_limit  = path->hop_limit;
		ib_ah_attr->grh.traffic_class =
			(be32_to_cpu(path->tclass_flowlabel) >> 20) & 0xff;
		ib_ah_attr->grh.flow_label =
			be32_to_cpu(path->tclass_flowlabel) & 0xfffff;
		memcpy(ib_ah_attr->grh.dgid.raw,
		       path->rgid, sizeof(ib_ah_attr->grh.dgid.raw));
	}
}

<<<<<<< HEAD
int mlx5_ib_query_qp(struct ib_qp *ibqp, struct ib_qp_attr *qp_attr, int qp_attr_mask,
		     struct ib_qp_init_attr *qp_init_attr)
{
	struct mlx5_ib_dev *dev = to_mdev(ibqp->device);
	struct mlx5_ib_qp *qp = to_mqp(ibqp);
	struct mlx5_query_qp_mbox_out *outb;
	struct mlx5_qp_context *context;
	int mlx5_state;
	int err = 0;

	mutex_lock(&qp->mutex);
	outb = kzalloc(sizeof(*outb), GFP_KERNEL);
	if (!outb) {
		err = -ENOMEM;
		goto out;
	}
	context = &outb->ctx;
	err = mlx5_core_qp_query(dev->mdev, &qp->mqp, outb, sizeof(*outb));
	if (err)
		goto out_free;
=======
static int query_raw_packet_qp_sq_state(struct mlx5_ib_dev *dev,
					struct mlx5_ib_sq *sq,
					u8 *sq_state)
{
	void *out;
	void *sqc;
	int inlen;
	int err;

	inlen = MLX5_ST_SZ_BYTES(query_sq_out);
	out = mlx5_vzalloc(inlen);
	if (!out)
		return -ENOMEM;

	err = mlx5_core_query_sq(dev->mdev, sq->base.mqp.qpn, out);
	if (err)
		goto out;

	sqc = MLX5_ADDR_OF(query_sq_out, out, sq_context);
	*sq_state = MLX5_GET(sqc, sqc, state);
	sq->state = *sq_state;

out:
	kvfree(out);
	return err;
}

static int query_raw_packet_qp_rq_state(struct mlx5_ib_dev *dev,
					struct mlx5_ib_rq *rq,
					u8 *rq_state)
{
	void *out;
	void *rqc;
	int inlen;
	int err;

	inlen = MLX5_ST_SZ_BYTES(query_rq_out);
	out = mlx5_vzalloc(inlen);
	if (!out)
		return -ENOMEM;

	err = mlx5_core_query_rq(dev->mdev, rq->base.mqp.qpn, out);
	if (err)
		goto out;

	rqc = MLX5_ADDR_OF(query_rq_out, out, rq_context);
	*rq_state = MLX5_GET(rqc, rqc, state);
	rq->state = *rq_state;

out:
	kvfree(out);
	return err;
}

static int sqrq_state_to_qp_state(u8 sq_state, u8 rq_state,
				  struct mlx5_ib_qp *qp, u8 *qp_state)
{
	static const u8 sqrq_trans[MLX5_RQ_NUM_STATE][MLX5_SQ_NUM_STATE] = {
		[MLX5_RQC_STATE_RST] = {
			[MLX5_SQC_STATE_RST]	= IB_QPS_RESET,
			[MLX5_SQC_STATE_RDY]	= MLX5_QP_STATE_BAD,
			[MLX5_SQC_STATE_ERR]	= MLX5_QP_STATE_BAD,
			[MLX5_SQ_STATE_NA]	= IB_QPS_RESET,
		},
		[MLX5_RQC_STATE_RDY] = {
			[MLX5_SQC_STATE_RST]	= MLX5_QP_STATE_BAD,
			[MLX5_SQC_STATE_RDY]	= MLX5_QP_STATE,
			[MLX5_SQC_STATE_ERR]	= IB_QPS_SQE,
			[MLX5_SQ_STATE_NA]	= MLX5_QP_STATE,
		},
		[MLX5_RQC_STATE_ERR] = {
			[MLX5_SQC_STATE_RST]    = MLX5_QP_STATE_BAD,
			[MLX5_SQC_STATE_RDY]	= MLX5_QP_STATE_BAD,
			[MLX5_SQC_STATE_ERR]	= IB_QPS_ERR,
			[MLX5_SQ_STATE_NA]	= IB_QPS_ERR,
		},
		[MLX5_RQ_STATE_NA] = {
			[MLX5_SQC_STATE_RST]    = IB_QPS_RESET,
			[MLX5_SQC_STATE_RDY]	= MLX5_QP_STATE,
			[MLX5_SQC_STATE_ERR]	= MLX5_QP_STATE,
			[MLX5_SQ_STATE_NA]	= MLX5_QP_STATE_BAD,
		},
	};

	*qp_state = sqrq_trans[rq_state][sq_state];

	if (*qp_state == MLX5_QP_STATE_BAD) {
		WARN(1, "Buggy Raw Packet QP state, SQ 0x%x state: 0x%x, RQ 0x%x state: 0x%x",
		     qp->raw_packet_qp.sq.base.mqp.qpn, sq_state,
		     qp->raw_packet_qp.rq.base.mqp.qpn, rq_state);
		return -EINVAL;
	}

	if (*qp_state == MLX5_QP_STATE)
		*qp_state = qp->state;

	return 0;
}

static int query_raw_packet_qp_state(struct mlx5_ib_dev *dev,
				     struct mlx5_ib_qp *qp,
				     u8 *raw_packet_qp_state)
{
	struct mlx5_ib_raw_packet_qp *raw_packet_qp = &qp->raw_packet_qp;
	struct mlx5_ib_sq *sq = &raw_packet_qp->sq;
	struct mlx5_ib_rq *rq = &raw_packet_qp->rq;
	int err;
	u8 sq_state = MLX5_SQ_STATE_NA;
	u8 rq_state = MLX5_RQ_STATE_NA;

	if (qp->sq.wqe_cnt) {
		err = query_raw_packet_qp_sq_state(dev, sq, &sq_state);
		if (err)
			return err;
	}

	if (qp->rq.wqe_cnt) {
		err = query_raw_packet_qp_rq_state(dev, rq, &rq_state);
		if (err)
			return err;
	}

	return sqrq_state_to_qp_state(sq_state, rq_state, qp,
				      raw_packet_qp_state);
}

static int query_qp_attr(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp,
			 struct ib_qp_attr *qp_attr)
{
	int outlen = MLX5_ST_SZ_BYTES(query_qp_out);
	struct mlx5_qp_context *context;
	int mlx5_state;
	u32 *outb;
	int err = 0;

	outb = kzalloc(outlen, GFP_KERNEL);
	if (!outb)
		return -ENOMEM;

	err = mlx5_core_qp_query(dev->mdev, &qp->trans_qp.base.mqp, outb,
				 outlen);
	if (err)
		goto out;

	/* FIXME: use MLX5_GET rather than mlx5_qp_context manual struct */
	context = (struct mlx5_qp_context *)MLX5_ADDR_OF(query_qp_out, outb, qpc);
>>>>>>> v4.9.227

	mlx5_state = be32_to_cpu(context->flags) >> 28;

	qp->state		     = to_ib_qp_state(mlx5_state);
<<<<<<< HEAD
	qp_attr->qp_state	     = qp->state;
=======
>>>>>>> v4.9.227
	qp_attr->path_mtu	     = context->mtu_msgmax >> 5;
	qp_attr->path_mig_state	     =
		to_ib_mig_state((be32_to_cpu(context->flags) >> 11) & 0x3);
	qp_attr->qkey		     = be32_to_cpu(context->qkey);
	qp_attr->rq_psn		     = be32_to_cpu(context->rnr_nextrecvpsn) & 0xffffff;
	qp_attr->sq_psn		     = be32_to_cpu(context->next_send_psn) & 0xffffff;
	qp_attr->dest_qp_num	     = be32_to_cpu(context->log_pg_sz_remote_qpn) & 0xffffff;
	qp_attr->qp_access_flags     =
		to_ib_qp_access_flags(be32_to_cpu(context->params2));

	if (qp->ibqp.qp_type == IB_QPT_RC || qp->ibqp.qp_type == IB_QPT_UC) {
		to_ib_ah_attr(dev, &qp_attr->ah_attr, &context->pri_path);
		to_ib_ah_attr(dev, &qp_attr->alt_ah_attr, &context->alt_path);
<<<<<<< HEAD
		qp_attr->alt_pkey_index = context->alt_path.pkey_index & 0x7f;
		qp_attr->alt_port_num	= qp_attr->alt_ah_attr.port_num;
	}

	qp_attr->pkey_index = context->pri_path.pkey_index & 0x7f;
=======
		qp_attr->alt_pkey_index =
			be16_to_cpu(context->alt_path.pkey_index);
		qp_attr->alt_port_num	= qp_attr->alt_ah_attr.port_num;
	}

	qp_attr->pkey_index = be16_to_cpu(context->pri_path.pkey_index);
>>>>>>> v4.9.227
	qp_attr->port_num = context->pri_path.port;

	/* qp_attr->en_sqd_async_notify is only applicable in modify qp */
	qp_attr->sq_draining = mlx5_state == MLX5_QP_STATE_SQ_DRAINING;

	qp_attr->max_rd_atomic = 1 << ((be32_to_cpu(context->params1) >> 21) & 0x7);

	qp_attr->max_dest_rd_atomic =
		1 << ((be32_to_cpu(context->params2) >> 21) & 0x7);
	qp_attr->min_rnr_timer	    =
		(be32_to_cpu(context->rnr_nextrecvpsn) >> 24) & 0x1f;
	qp_attr->timeout	    = context->pri_path.ackto_lt >> 3;
	qp_attr->retry_cnt	    = (be32_to_cpu(context->params1) >> 16) & 0x7;
	qp_attr->rnr_retry	    = (be32_to_cpu(context->params1) >> 13) & 0x7;
	qp_attr->alt_timeout	    = context->alt_path.ackto_lt >> 3;
<<<<<<< HEAD
=======

out:
	kfree(outb);
	return err;
}

int mlx5_ib_query_qp(struct ib_qp *ibqp, struct ib_qp_attr *qp_attr,
		     int qp_attr_mask, struct ib_qp_init_attr *qp_init_attr)
{
	struct mlx5_ib_dev *dev = to_mdev(ibqp->device);
	struct mlx5_ib_qp *qp = to_mqp(ibqp);
	int err = 0;
	u8 raw_packet_qp_state;

	if (ibqp->rwq_ind_tbl)
		return -ENOSYS;

	if (unlikely(ibqp->qp_type == IB_QPT_GSI))
		return mlx5_ib_gsi_query_qp(ibqp, qp_attr, qp_attr_mask,
					    qp_init_attr);

#ifdef CONFIG_INFINIBAND_ON_DEMAND_PAGING
	/*
	 * Wait for any outstanding page faults, in case the user frees memory
	 * based upon this query's result.
	 */
	flush_workqueue(mlx5_ib_page_fault_wq);
#endif

	mutex_lock(&qp->mutex);

	if (qp->ibqp.qp_type == IB_QPT_RAW_PACKET) {
		err = query_raw_packet_qp_state(dev, qp, &raw_packet_qp_state);
		if (err)
			goto out;
		qp->state = raw_packet_qp_state;
		qp_attr->port_num = 1;
	} else {
		err = query_qp_attr(dev, qp, qp_attr);
		if (err)
			goto out;
	}

	qp_attr->qp_state	     = qp->state;
>>>>>>> v4.9.227
	qp_attr->cur_qp_state	     = qp_attr->qp_state;
	qp_attr->cap.max_recv_wr     = qp->rq.wqe_cnt;
	qp_attr->cap.max_recv_sge    = qp->rq.max_gs;

	if (!ibqp->uobject) {
<<<<<<< HEAD
		qp_attr->cap.max_send_wr  = qp->sq.wqe_cnt;
		qp_attr->cap.max_send_sge = qp->sq.max_gs;
=======
		qp_attr->cap.max_send_wr  = qp->sq.max_post;
		qp_attr->cap.max_send_sge = qp->sq.max_gs;
		qp_init_attr->qp_context = ibqp->qp_context;
>>>>>>> v4.9.227
	} else {
		qp_attr->cap.max_send_wr  = 0;
		qp_attr->cap.max_send_sge = 0;
	}

<<<<<<< HEAD
	/* We don't support inline sends for kernel QPs (yet), and we
	 * don't know what userspace's value should be.
	 */
	qp_attr->cap.max_inline_data = 0;
=======
	qp_init_attr->qp_type = ibqp->qp_type;
	qp_init_attr->recv_cq = ibqp->recv_cq;
	qp_init_attr->send_cq = ibqp->send_cq;
	qp_init_attr->srq = ibqp->srq;
	qp_attr->cap.max_inline_data = qp->max_inline_data;
>>>>>>> v4.9.227

	qp_init_attr->cap	     = qp_attr->cap;

	qp_init_attr->create_flags = 0;
	if (qp->flags & MLX5_IB_QP_BLOCK_MULTICAST_LOOPBACK)
		qp_init_attr->create_flags |= IB_QP_CREATE_BLOCK_MULTICAST_LOOPBACK;

<<<<<<< HEAD
	qp_init_attr->sq_sig_type = qp->sq_signal_bits & MLX5_WQE_CTRL_CQ_UPDATE ?
		IB_SIGNAL_ALL_WR : IB_SIGNAL_REQ_WR;

out_free:
	kfree(outb);

=======
	if (qp->flags & MLX5_IB_QP_CROSS_CHANNEL)
		qp_init_attr->create_flags |= IB_QP_CREATE_CROSS_CHANNEL;
	if (qp->flags & MLX5_IB_QP_MANAGED_SEND)
		qp_init_attr->create_flags |= IB_QP_CREATE_MANAGED_SEND;
	if (qp->flags & MLX5_IB_QP_MANAGED_RECV)
		qp_init_attr->create_flags |= IB_QP_CREATE_MANAGED_RECV;
	if (qp->flags & MLX5_IB_QP_SQPN_QP1)
		qp_init_attr->create_flags |= mlx5_ib_create_qp_sqpn_qp1();

	qp_init_attr->sq_sig_type = qp->sq_signal_bits & MLX5_WQE_CTRL_CQ_UPDATE ?
		IB_SIGNAL_ALL_WR : IB_SIGNAL_REQ_WR;

>>>>>>> v4.9.227
out:
	mutex_unlock(&qp->mutex);
	return err;
}

struct ib_xrcd *mlx5_ib_alloc_xrcd(struct ib_device *ibdev,
					  struct ib_ucontext *context,
					  struct ib_udata *udata)
{
	struct mlx5_ib_dev *dev = to_mdev(ibdev);
<<<<<<< HEAD
	struct mlx5_general_caps *gen;
	struct mlx5_ib_xrcd *xrcd;
	int err;

	gen = &dev->mdev->caps.gen;
	if (!(gen->flags & MLX5_DEV_CAP_FLAG_XRC))
=======
	struct mlx5_ib_xrcd *xrcd;
	int err;

	if (!MLX5_CAP_GEN(dev->mdev, xrc))
>>>>>>> v4.9.227
		return ERR_PTR(-ENOSYS);

	xrcd = kmalloc(sizeof(*xrcd), GFP_KERNEL);
	if (!xrcd)
		return ERR_PTR(-ENOMEM);

	err = mlx5_core_xrcd_alloc(dev->mdev, &xrcd->xrcdn);
	if (err) {
		kfree(xrcd);
		return ERR_PTR(-ENOMEM);
	}

	return &xrcd->ibxrcd;
}

int mlx5_ib_dealloc_xrcd(struct ib_xrcd *xrcd)
{
	struct mlx5_ib_dev *dev = to_mdev(xrcd->device);
	u32 xrcdn = to_mxrcd(xrcd)->xrcdn;
	int err;

	err = mlx5_core_xrcd_dealloc(dev->mdev, xrcdn);
	if (err)
		mlx5_ib_warn(dev, "failed to dealloc xrcdn 0x%x\n", xrcdn);

	kfree(xrcd);
	return 0;
}
<<<<<<< HEAD
=======

static void mlx5_ib_wq_event(struct mlx5_core_qp *core_qp, int type)
{
	struct mlx5_ib_rwq *rwq = to_mibrwq(core_qp);
	struct mlx5_ib_dev *dev = to_mdev(rwq->ibwq.device);
	struct ib_event event;

	if (rwq->ibwq.event_handler) {
		event.device     = rwq->ibwq.device;
		event.element.wq = &rwq->ibwq;
		switch (type) {
		case MLX5_EVENT_TYPE_WQ_CATAS_ERROR:
			event.event = IB_EVENT_WQ_FATAL;
			break;
		default:
			mlx5_ib_warn(dev, "Unexpected event type %d on WQ %06x\n", type, core_qp->qpn);
			return;
		}

		rwq->ibwq.event_handler(&event, rwq->ibwq.wq_context);
	}
}

static int  create_rq(struct mlx5_ib_rwq *rwq, struct ib_pd *pd,
		      struct ib_wq_init_attr *init_attr)
{
	struct mlx5_ib_dev *dev;
	__be64 *rq_pas0;
	void *in;
	void *rqc;
	void *wq;
	int inlen;
	int err;

	dev = to_mdev(pd->device);

	inlen = MLX5_ST_SZ_BYTES(create_rq_in) + sizeof(u64) * rwq->rq_num_pas;
	in = mlx5_vzalloc(inlen);
	if (!in)
		return -ENOMEM;

	rqc = MLX5_ADDR_OF(create_rq_in, in, ctx);
	MLX5_SET(rqc,  rqc, mem_rq_type,
		 MLX5_RQC_MEM_RQ_TYPE_MEMORY_RQ_INLINE);
	MLX5_SET(rqc, rqc, user_index, rwq->user_index);
	MLX5_SET(rqc,  rqc, cqn, to_mcq(init_attr->cq)->mcq.cqn);
	MLX5_SET(rqc,  rqc, state, MLX5_RQC_STATE_RST);
	MLX5_SET(rqc,  rqc, flush_in_error_en, 1);
	wq = MLX5_ADDR_OF(rqc, rqc, wq);
	MLX5_SET(wq, wq, wq_type, MLX5_WQ_TYPE_CYCLIC);
	MLX5_SET(wq, wq, end_padding_mode, MLX5_WQ_END_PAD_MODE_ALIGN);
	MLX5_SET(wq, wq, log_wq_stride, rwq->log_rq_stride);
	MLX5_SET(wq, wq, log_wq_sz, rwq->log_rq_size);
	MLX5_SET(wq, wq, pd, to_mpd(pd)->pdn);
	MLX5_SET(wq, wq, page_offset, rwq->rq_page_offset);
	MLX5_SET(wq, wq, log_wq_pg_sz, rwq->log_page_size);
	MLX5_SET(wq, wq, wq_signature, rwq->wq_sig);
	MLX5_SET64(wq, wq, dbr_addr, rwq->db.dma);
	rq_pas0 = (__be64 *)MLX5_ADDR_OF(wq, wq, pas);
	mlx5_ib_populate_pas(dev, rwq->umem, rwq->page_shift, rq_pas0, 0);
	err = mlx5_core_create_rq_tracked(dev->mdev, in, inlen, &rwq->core_qp);
	kvfree(in);
	return err;
}

static int set_user_rq_size(struct mlx5_ib_dev *dev,
			    struct ib_wq_init_attr *wq_init_attr,
			    struct mlx5_ib_create_wq *ucmd,
			    struct mlx5_ib_rwq *rwq)
{
	/* Sanity check RQ size before proceeding */
	if (wq_init_attr->max_wr > (1 << MLX5_CAP_GEN(dev->mdev, log_max_wq_sz)))
		return -EINVAL;

	if (!ucmd->rq_wqe_count)
		return -EINVAL;

	rwq->wqe_count = ucmd->rq_wqe_count;
	rwq->wqe_shift = ucmd->rq_wqe_shift;
	rwq->buf_size = (rwq->wqe_count << rwq->wqe_shift);
	rwq->log_rq_stride = rwq->wqe_shift;
	rwq->log_rq_size = ilog2(rwq->wqe_count);
	return 0;
}

static int prepare_user_rq(struct ib_pd *pd,
			   struct ib_wq_init_attr *init_attr,
			   struct ib_udata *udata,
			   struct mlx5_ib_rwq *rwq)
{
	struct mlx5_ib_dev *dev = to_mdev(pd->device);
	struct mlx5_ib_create_wq ucmd = {};
	int err;
	size_t required_cmd_sz;

	required_cmd_sz = offsetof(typeof(ucmd), reserved) + sizeof(ucmd.reserved);
	if (udata->inlen < required_cmd_sz) {
		mlx5_ib_dbg(dev, "invalid inlen\n");
		return -EINVAL;
	}

	if (udata->inlen > sizeof(ucmd) &&
	    !ib_is_udata_cleared(udata, sizeof(ucmd),
				 udata->inlen - sizeof(ucmd))) {
		mlx5_ib_dbg(dev, "inlen is not supported\n");
		return -EOPNOTSUPP;
	}

	if (ib_copy_from_udata(&ucmd, udata, min(sizeof(ucmd), udata->inlen))) {
		mlx5_ib_dbg(dev, "copy failed\n");
		return -EFAULT;
	}

	if (ucmd.comp_mask) {
		mlx5_ib_dbg(dev, "invalid comp mask\n");
		return -EOPNOTSUPP;
	}

	if (ucmd.reserved) {
		mlx5_ib_dbg(dev, "invalid reserved\n");
		return -EOPNOTSUPP;
	}

	err = set_user_rq_size(dev, init_attr, &ucmd, rwq);
	if (err) {
		mlx5_ib_dbg(dev, "err %d\n", err);
		return err;
	}

	err = create_user_rq(dev, pd, rwq, &ucmd);
	if (err) {
		mlx5_ib_dbg(dev, "err %d\n", err);
		if (err)
			return err;
	}

	rwq->user_index = ucmd.user_index;
	return 0;
}

struct ib_wq *mlx5_ib_create_wq(struct ib_pd *pd,
				struct ib_wq_init_attr *init_attr,
				struct ib_udata *udata)
{
	struct mlx5_ib_dev *dev;
	struct mlx5_ib_rwq *rwq;
	struct mlx5_ib_create_wq_resp resp = {};
	size_t min_resp_len;
	int err;

	if (!udata)
		return ERR_PTR(-ENOSYS);

	min_resp_len = offsetof(typeof(resp), reserved) + sizeof(resp.reserved);
	if (udata->outlen && udata->outlen < min_resp_len)
		return ERR_PTR(-EINVAL);

	dev = to_mdev(pd->device);
	switch (init_attr->wq_type) {
	case IB_WQT_RQ:
		rwq = kzalloc(sizeof(*rwq), GFP_KERNEL);
		if (!rwq)
			return ERR_PTR(-ENOMEM);
		err = prepare_user_rq(pd, init_attr, udata, rwq);
		if (err)
			goto err;
		err = create_rq(rwq, pd, init_attr);
		if (err)
			goto err_user_rq;
		break;
	default:
		mlx5_ib_dbg(dev, "unsupported wq type %d\n",
			    init_attr->wq_type);
		return ERR_PTR(-EINVAL);
	}

	rwq->ibwq.wq_num = rwq->core_qp.qpn;
	rwq->ibwq.state = IB_WQS_RESET;
	if (udata->outlen) {
		resp.response_length = offsetof(typeof(resp), response_length) +
				sizeof(resp.response_length);
		err = ib_copy_to_udata(udata, &resp, resp.response_length);
		if (err)
			goto err_copy;
	}

	rwq->core_qp.event = mlx5_ib_wq_event;
	rwq->ibwq.event_handler = init_attr->event_handler;
	return &rwq->ibwq;

err_copy:
	mlx5_core_destroy_rq_tracked(dev->mdev, &rwq->core_qp);
err_user_rq:
	destroy_user_rq(pd, rwq);
err:
	kfree(rwq);
	return ERR_PTR(err);
}

int mlx5_ib_destroy_wq(struct ib_wq *wq)
{
	struct mlx5_ib_dev *dev = to_mdev(wq->device);
	struct mlx5_ib_rwq *rwq = to_mrwq(wq);

	mlx5_core_destroy_rq_tracked(dev->mdev, &rwq->core_qp);
	destroy_user_rq(wq->pd, rwq);
	kfree(rwq);

	return 0;
}

struct ib_rwq_ind_table *mlx5_ib_create_rwq_ind_table(struct ib_device *device,
						      struct ib_rwq_ind_table_init_attr *init_attr,
						      struct ib_udata *udata)
{
	struct mlx5_ib_dev *dev = to_mdev(device);
	struct mlx5_ib_rwq_ind_table *rwq_ind_tbl;
	int sz = 1 << init_attr->log_ind_tbl_size;
	struct mlx5_ib_create_rwq_ind_tbl_resp resp = {};
	size_t min_resp_len;
	int inlen;
	int err;
	int i;
	u32 *in;
	void *rqtc;

	if (udata->inlen > 0 &&
	    !ib_is_udata_cleared(udata, 0,
				 udata->inlen))
		return ERR_PTR(-EOPNOTSUPP);

	if (init_attr->log_ind_tbl_size >
	    MLX5_CAP_GEN(dev->mdev, log_max_rqt_size)) {
		mlx5_ib_dbg(dev, "log_ind_tbl_size = %d is bigger than supported = %d\n",
			    init_attr->log_ind_tbl_size,
			    MLX5_CAP_GEN(dev->mdev, log_max_rqt_size));
		return ERR_PTR(-EINVAL);
	}

	min_resp_len = offsetof(typeof(resp), reserved) + sizeof(resp.reserved);
	if (udata->outlen && udata->outlen < min_resp_len)
		return ERR_PTR(-EINVAL);

	rwq_ind_tbl = kzalloc(sizeof(*rwq_ind_tbl), GFP_KERNEL);
	if (!rwq_ind_tbl)
		return ERR_PTR(-ENOMEM);

	inlen = MLX5_ST_SZ_BYTES(create_rqt_in) + sizeof(u32) * sz;
	in = mlx5_vzalloc(inlen);
	if (!in) {
		err = -ENOMEM;
		goto err;
	}

	rqtc = MLX5_ADDR_OF(create_rqt_in, in, rqt_context);

	MLX5_SET(rqtc, rqtc, rqt_actual_size, sz);
	MLX5_SET(rqtc, rqtc, rqt_max_size, sz);

	for (i = 0; i < sz; i++)
		MLX5_SET(rqtc, rqtc, rq_num[i], init_attr->ind_tbl[i]->wq_num);

	err = mlx5_core_create_rqt(dev->mdev, in, inlen, &rwq_ind_tbl->rqtn);
	kvfree(in);

	if (err)
		goto err;

	rwq_ind_tbl->ib_rwq_ind_tbl.ind_tbl_num = rwq_ind_tbl->rqtn;
	if (udata->outlen) {
		resp.response_length = offsetof(typeof(resp), response_length) +
					sizeof(resp.response_length);
		err = ib_copy_to_udata(udata, &resp, resp.response_length);
		if (err)
			goto err_copy;
	}

	return &rwq_ind_tbl->ib_rwq_ind_tbl;

err_copy:
	mlx5_core_destroy_rqt(dev->mdev, rwq_ind_tbl->rqtn);
err:
	kfree(rwq_ind_tbl);
	return ERR_PTR(err);
}

int mlx5_ib_destroy_rwq_ind_table(struct ib_rwq_ind_table *ib_rwq_ind_tbl)
{
	struct mlx5_ib_rwq_ind_table *rwq_ind_tbl = to_mrwq_ind_table(ib_rwq_ind_tbl);
	struct mlx5_ib_dev *dev = to_mdev(ib_rwq_ind_tbl->device);

	mlx5_core_destroy_rqt(dev->mdev, rwq_ind_tbl->rqtn);

	kfree(rwq_ind_tbl);
	return 0;
}

int mlx5_ib_modify_wq(struct ib_wq *wq, struct ib_wq_attr *wq_attr,
		      u32 wq_attr_mask, struct ib_udata *udata)
{
	struct mlx5_ib_dev *dev = to_mdev(wq->device);
	struct mlx5_ib_rwq *rwq = to_mrwq(wq);
	struct mlx5_ib_modify_wq ucmd = {};
	size_t required_cmd_sz;
	int curr_wq_state;
	int wq_state;
	int inlen;
	int err;
	void *rqc;
	void *in;

	required_cmd_sz = offsetof(typeof(ucmd), reserved) + sizeof(ucmd.reserved);
	if (udata->inlen < required_cmd_sz)
		return -EINVAL;

	if (udata->inlen > sizeof(ucmd) &&
	    !ib_is_udata_cleared(udata, sizeof(ucmd),
				 udata->inlen - sizeof(ucmd)))
		return -EOPNOTSUPP;

	if (ib_copy_from_udata(&ucmd, udata, min(sizeof(ucmd), udata->inlen)))
		return -EFAULT;

	if (ucmd.comp_mask || ucmd.reserved)
		return -EOPNOTSUPP;

	inlen = MLX5_ST_SZ_BYTES(modify_rq_in);
	in = mlx5_vzalloc(inlen);
	if (!in)
		return -ENOMEM;

	rqc = MLX5_ADDR_OF(modify_rq_in, in, ctx);

	curr_wq_state = (wq_attr_mask & IB_WQ_CUR_STATE) ?
		wq_attr->curr_wq_state : wq->state;
	wq_state = (wq_attr_mask & IB_WQ_STATE) ?
		wq_attr->wq_state : curr_wq_state;
	if (curr_wq_state == IB_WQS_ERR)
		curr_wq_state = MLX5_RQC_STATE_ERR;
	if (wq_state == IB_WQS_ERR)
		wq_state = MLX5_RQC_STATE_ERR;
	MLX5_SET(modify_rq_in, in, rq_state, curr_wq_state);
	MLX5_SET(rqc, rqc, state, wq_state);

	err = mlx5_core_modify_rq(dev->mdev, rwq->core_qp.qpn, in, inlen);
	kvfree(in);
	if (!err)
		rwq->ibwq.state = (wq_state == MLX5_RQC_STATE_ERR) ? IB_WQS_ERR : wq_state;

	return err;
}
>>>>>>> v4.9.227
