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

#include <linux/highmem.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/io-mapping.h>
<<<<<<< HEAD
=======
#include <linux/interrupt.h>
#include <linux/delay.h>
>>>>>>> v4.9.227
#include <linux/mlx5/driver.h>
#include <linux/mlx5/cq.h>
#include <linux/mlx5/qp.h>
#include <linux/mlx5/srq.h>
#include <linux/debugfs.h>
<<<<<<< HEAD
#include <linux/mlx5/mlx5_ifc.h>
#include "mlx5_core.h"

#define DRIVER_NAME "mlx5_core"
#define DRIVER_VERSION "2.2-1"
#define DRIVER_RELDATE	"Feb 2014"

MODULE_AUTHOR("Eli Cohen <eli@mellanox.com>");
MODULE_DESCRIPTION("Mellanox ConnectX-IB HCA core library");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION(DRIVER_VERSION);

int mlx5_core_debug_mask;
module_param_named(debug_mask, mlx5_core_debug_mask, int, 0644);
MODULE_PARM_DESC(debug_mask, "debug mask: 1 = dump cmd data, 2 = dump cmd exec time, 3 = both. Default=0");

#define MLX5_DEFAULT_PROF	2
static int prof_sel = MLX5_DEFAULT_PROF;
module_param_named(prof_sel, prof_sel, int, 0444);
MODULE_PARM_DESC(prof_sel, "profile selector. Valid range 0 - 2");

struct workqueue_struct *mlx5_core_wq;
static LIST_HEAD(intf_list);
static LIST_HEAD(dev_list);
static DEFINE_MUTEX(intf_mutex);

struct mlx5_device_context {
	struct list_head	list;
	struct mlx5_interface  *intf;
	void		       *context;
=======
#include <linux/kmod.h>
#include <linux/mlx5/mlx5_ifc.h>
#ifdef CONFIG_RFS_ACCEL
#include <linux/cpu_rmap.h>
#endif
#include <net/devlink.h>
#include "mlx5_core.h"
#include "fs_core.h"
#ifdef CONFIG_MLX5_CORE_EN
#include "eswitch.h"
#endif

MODULE_AUTHOR("Eli Cohen <eli@mellanox.com>");
MODULE_DESCRIPTION("Mellanox Connect-IB, ConnectX-4 core driver");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION(DRIVER_VERSION);

unsigned int mlx5_core_debug_mask;
module_param_named(debug_mask, mlx5_core_debug_mask, uint, 0644);
MODULE_PARM_DESC(debug_mask, "debug mask: 1 = dump cmd data, 2 = dump cmd exec time, 3 = both. Default=0");

#define MLX5_DEFAULT_PROF	2
static unsigned int prof_sel = MLX5_DEFAULT_PROF;
module_param_named(prof_sel, prof_sel, uint, 0444);
MODULE_PARM_DESC(prof_sel, "profile selector. Valid range 0 - 2");

enum {
	MLX5_ATOMIC_REQ_MODE_BE = 0x0,
	MLX5_ATOMIC_REQ_MODE_HOST_ENDIANNESS = 0x1,
>>>>>>> v4.9.227
};

static struct mlx5_profile profile[] = {
	[0] = {
		.mask           = 0,
	},
	[1] = {
		.mask		= MLX5_PROF_MASK_QP_SIZE,
		.log_max_qp	= 12,
	},
	[2] = {
		.mask		= MLX5_PROF_MASK_QP_SIZE |
				  MLX5_PROF_MASK_MR_CACHE,
		.log_max_qp	= 18,
		.mr_cache[0]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[1]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[2]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[3]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[4]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[5]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[6]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[7]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[8]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[9]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[10]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[11]	= {
			.size	= 500,
			.limit	= 250
		},
		.mr_cache[12]	= {
			.size	= 64,
			.limit	= 32
		},
		.mr_cache[13]	= {
			.size	= 32,
			.limit	= 16
		},
		.mr_cache[14]	= {
			.size	= 16,
			.limit	= 8
		},
		.mr_cache[15]	= {
			.size	= 8,
			.limit	= 4
		},
	},
};

<<<<<<< HEAD
=======
#define FW_INIT_TIMEOUT_MILI		2000
#define FW_INIT_WAIT_MS			2
#define FW_PRE_INIT_TIMEOUT_MILI	10000

static int wait_fw_init(struct mlx5_core_dev *dev, u32 max_wait_mili)
{
	unsigned long end = jiffies + msecs_to_jiffies(max_wait_mili);
	int err = 0;

	while (fw_initializing(dev)) {
		if (time_after(jiffies, end)) {
			err = -EBUSY;
			break;
		}
		msleep(FW_INIT_WAIT_MS);
	}

	return err;
}

>>>>>>> v4.9.227
static int set_dma_caps(struct pci_dev *pdev)
{
	int err;

	err = pci_set_dma_mask(pdev, DMA_BIT_MASK(64));
	if (err) {
		dev_warn(&pdev->dev, "Warning: couldn't set 64-bit PCI DMA mask\n");
		err = pci_set_dma_mask(pdev, DMA_BIT_MASK(32));
		if (err) {
			dev_err(&pdev->dev, "Can't set PCI DMA mask, aborting\n");
			return err;
		}
	}

	err = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(64));
	if (err) {
		dev_warn(&pdev->dev,
			 "Warning: couldn't set 64-bit consistent PCI DMA mask\n");
		err = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(32));
		if (err) {
			dev_err(&pdev->dev,
				"Can't set consistent PCI DMA mask, aborting\n");
			return err;
		}
	}

	dma_set_max_seg_size(&pdev->dev, 2u * 1024 * 1024 * 1024);
	return err;
}

<<<<<<< HEAD
=======
static int mlx5_pci_enable_device(struct mlx5_core_dev *dev)
{
	struct pci_dev *pdev = dev->pdev;
	int err = 0;

	mutex_lock(&dev->pci_status_mutex);
	if (dev->pci_status == MLX5_PCI_STATUS_DISABLED) {
		err = pci_enable_device(pdev);
		if (!err)
			dev->pci_status = MLX5_PCI_STATUS_ENABLED;
	}
	mutex_unlock(&dev->pci_status_mutex);

	return err;
}

static void mlx5_pci_disable_device(struct mlx5_core_dev *dev)
{
	struct pci_dev *pdev = dev->pdev;

	mutex_lock(&dev->pci_status_mutex);
	if (dev->pci_status == MLX5_PCI_STATUS_ENABLED) {
		pci_disable_device(pdev);
		dev->pci_status = MLX5_PCI_STATUS_DISABLED;
	}
	mutex_unlock(&dev->pci_status_mutex);
}

>>>>>>> v4.9.227
static int request_bar(struct pci_dev *pdev)
{
	int err = 0;

	if (!(pci_resource_flags(pdev, 0) & IORESOURCE_MEM)) {
		dev_err(&pdev->dev, "Missing registers BAR, aborting\n");
		return -ENODEV;
	}

	err = pci_request_regions(pdev, DRIVER_NAME);
	if (err)
		dev_err(&pdev->dev, "Couldn't get PCI resources, aborting\n");

	return err;
}

static void release_bar(struct pci_dev *pdev)
{
	pci_release_regions(pdev);
}

static int mlx5_enable_msix(struct mlx5_core_dev *dev)
{
<<<<<<< HEAD
	struct mlx5_eq_table *table = &dev->priv.eq_table;
	int num_eqs = 1 << dev->caps.gen.log_max_eq;
	int nvec;
	int i;

	nvec = dev->caps.gen.num_ports * num_online_cpus() + MLX5_EQ_VEC_COMP_BASE;
=======
	struct mlx5_priv *priv = &dev->priv;
	struct mlx5_eq_table *table = &priv->eq_table;
	int num_eqs = 1 << MLX5_CAP_GEN(dev, log_max_eq);
	int nvec;
	int i;

	nvec = MLX5_CAP_GEN(dev, num_ports) * num_online_cpus() +
	       MLX5_EQ_VEC_COMP_BASE;
>>>>>>> v4.9.227
	nvec = min_t(int, nvec, num_eqs);
	if (nvec <= MLX5_EQ_VEC_COMP_BASE)
		return -ENOMEM;

<<<<<<< HEAD
	table->msix_arr = kzalloc(nvec * sizeof(*table->msix_arr), GFP_KERNEL);
	if (!table->msix_arr)
		return -ENOMEM;

	for (i = 0; i < nvec; i++)
		table->msix_arr[i].entry = i;

	nvec = pci_enable_msix_range(dev->pdev, table->msix_arr,
				     MLX5_EQ_VEC_COMP_BASE, nvec);
=======
	priv->msix_arr = kcalloc(nvec, sizeof(*priv->msix_arr), GFP_KERNEL);

	priv->irq_info = kcalloc(nvec, sizeof(*priv->irq_info), GFP_KERNEL);
	if (!priv->msix_arr || !priv->irq_info)
		goto err_free_msix;

	for (i = 0; i < nvec; i++)
		priv->msix_arr[i].entry = i;

	nvec = pci_enable_msix_range(dev->pdev, priv->msix_arr,
				     MLX5_EQ_VEC_COMP_BASE + 1, nvec);
>>>>>>> v4.9.227
	if (nvec < 0)
		return nvec;

	table->num_comp_vectors = nvec - MLX5_EQ_VEC_COMP_BASE;

	return 0;
<<<<<<< HEAD
=======

err_free_msix:
	kfree(priv->irq_info);
	kfree(priv->msix_arr);
	return -ENOMEM;
>>>>>>> v4.9.227
}

static void mlx5_disable_msix(struct mlx5_core_dev *dev)
{
<<<<<<< HEAD
	struct mlx5_eq_table *table = &dev->priv.eq_table;

	pci_disable_msix(dev->pdev);
	kfree(table->msix_arr);
=======
	struct mlx5_priv *priv = &dev->priv;

	pci_disable_msix(dev->pdev);
	kfree(priv->irq_info);
	kfree(priv->msix_arr);
>>>>>>> v4.9.227
}

struct mlx5_reg_host_endianess {
	u8	he;
	u8      rsvd[15];
};


#define CAP_MASK(pos, size) ((u64)((1 << (size)) - 1) << (pos))

enum {
	MLX5_CAP_BITS_RW_MASK = CAP_MASK(MLX5_CAP_OFF_CMDIF_CSUM, 2) |
				MLX5_DEV_CAP_FLAG_DCT,
};

<<<<<<< HEAD
static u16 to_fw_pkey_sz(u32 size)
=======
static u16 to_fw_pkey_sz(struct mlx5_core_dev *dev, u32 size)
>>>>>>> v4.9.227
{
	switch (size) {
	case 128:
		return 0;
	case 256:
		return 1;
	case 512:
		return 2;
	case 1024:
		return 3;
	case 2048:
		return 4;
	case 4096:
		return 5;
	default:
<<<<<<< HEAD
		pr_warn("invalid pkey table size %d\n", size);
=======
		mlx5_core_warn(dev, "invalid pkey table size %d\n", size);
>>>>>>> v4.9.227
		return 0;
	}
}

<<<<<<< HEAD
/* selectively copy writable fields clearing any reserved area
 */
static void copy_rw_fields(void *to, struct mlx5_caps *from)
{
	__be64 *flags_off = (__be64 *)MLX5_ADDR_OF(cmd_hca_cap, to, reserved_22);
	u64 v64;

	MLX5_SET(cmd_hca_cap, to, log_max_qp, from->gen.log_max_qp);
	MLX5_SET(cmd_hca_cap, to, log_max_ra_req_qp, from->gen.log_max_ra_req_qp);
	MLX5_SET(cmd_hca_cap, to, log_max_ra_res_qp, from->gen.log_max_ra_res_qp);
	MLX5_SET(cmd_hca_cap, to, pkey_table_size, from->gen.pkey_table_size);
	MLX5_SET(cmd_hca_cap, to, log_max_ra_req_dc, from->gen.log_max_ra_req_dc);
	MLX5_SET(cmd_hca_cap, to, log_max_ra_res_dc, from->gen.log_max_ra_res_dc);
	MLX5_SET(cmd_hca_cap, to, pkey_table_size, to_fw_pkey_sz(from->gen.pkey_table_size));
	v64 = from->gen.flags & MLX5_CAP_BITS_RW_MASK;
	*flags_off = cpu_to_be64(v64);
}

static u16 get_pkey_table_size(int pkey)
{
	if (pkey > MLX5_MAX_LOG_PKEY_TABLE)
		return 0;

	return MLX5_MIN_PKEY_TABLE_SIZE << pkey;
}

static void fw2drv_caps(struct mlx5_caps *caps, void *out)
{
	struct mlx5_general_caps *gen = &caps->gen;

	gen->max_srq_wqes = 1 << MLX5_GET_PR(cmd_hca_cap, out, log_max_srq_sz);
	gen->max_wqes = 1 << MLX5_GET_PR(cmd_hca_cap, out, log_max_qp_sz);
	gen->log_max_qp = MLX5_GET_PR(cmd_hca_cap, out, log_max_qp);
	gen->log_max_strq = MLX5_GET_PR(cmd_hca_cap, out, log_max_strq_sz);
	gen->log_max_srq = MLX5_GET_PR(cmd_hca_cap, out, log_max_srqs);
	gen->max_cqes = 1 << MLX5_GET_PR(cmd_hca_cap, out, log_max_cq_sz);
	gen->log_max_cq = MLX5_GET_PR(cmd_hca_cap, out, log_max_cq);
	gen->max_eqes = 1 << MLX5_GET_PR(cmd_hca_cap, out, log_max_eq_sz);
	gen->log_max_mkey = MLX5_GET_PR(cmd_hca_cap, out, log_max_mkey);
	gen->log_max_eq = MLX5_GET_PR(cmd_hca_cap, out, log_max_eq);
	gen->max_indirection = MLX5_GET_PR(cmd_hca_cap, out, max_indirection);
	gen->log_max_mrw_sz = MLX5_GET_PR(cmd_hca_cap, out, log_max_mrw_sz);
	gen->log_max_bsf_list_size = MLX5_GET_PR(cmd_hca_cap, out, log_max_bsf_list_size);
	gen->log_max_klm_list_size = MLX5_GET_PR(cmd_hca_cap, out, log_max_klm_list_size);
	gen->log_max_ra_req_dc = MLX5_GET_PR(cmd_hca_cap, out, log_max_ra_req_dc);
	gen->log_max_ra_res_dc = MLX5_GET_PR(cmd_hca_cap, out, log_max_ra_res_dc);
	gen->log_max_ra_req_qp = MLX5_GET_PR(cmd_hca_cap, out, log_max_ra_req_qp);
	gen->log_max_ra_res_qp = MLX5_GET_PR(cmd_hca_cap, out, log_max_ra_res_qp);
	gen->max_qp_counters = MLX5_GET_PR(cmd_hca_cap, out, max_qp_cnt);
	gen->pkey_table_size = get_pkey_table_size(MLX5_GET_PR(cmd_hca_cap, out, pkey_table_size));
	gen->local_ca_ack_delay = MLX5_GET_PR(cmd_hca_cap, out, local_ca_ack_delay);
	gen->num_ports = MLX5_GET_PR(cmd_hca_cap, out, num_ports);
	gen->log_max_msg = MLX5_GET_PR(cmd_hca_cap, out, log_max_msg);
	gen->stat_rate_support = MLX5_GET_PR(cmd_hca_cap, out, stat_rate_support);
	gen->flags = be64_to_cpu(*(__be64 *)MLX5_ADDR_OF(cmd_hca_cap, out, reserved_22));
	pr_debug("flags = 0x%llx\n", gen->flags);
	gen->uar_sz = MLX5_GET_PR(cmd_hca_cap, out, uar_sz);
	gen->min_log_pg_sz = MLX5_GET_PR(cmd_hca_cap, out, log_pg_sz);
	gen->bf_reg_size = MLX5_GET_PR(cmd_hca_cap, out, bf);
	gen->bf_reg_size = 1 << MLX5_GET_PR(cmd_hca_cap, out, log_bf_reg_size);
	gen->max_sq_desc_sz = MLX5_GET_PR(cmd_hca_cap, out, max_wqe_sz_sq);
	gen->max_rq_desc_sz = MLX5_GET_PR(cmd_hca_cap, out, max_wqe_sz_rq);
	gen->max_dc_sq_desc_sz = MLX5_GET_PR(cmd_hca_cap, out, max_wqe_sz_sq_dc);
	gen->max_qp_mcg = MLX5_GET_PR(cmd_hca_cap, out, max_qp_mcg);
	gen->log_max_pd = MLX5_GET_PR(cmd_hca_cap, out, log_max_pd);
	gen->log_max_xrcd = MLX5_GET_PR(cmd_hca_cap, out, log_max_xrcd);
	gen->log_uar_page_sz = MLX5_GET_PR(cmd_hca_cap, out, log_uar_page_sz);
}

static const char *caps_opmod_str(u16 opmod)
{
	switch (opmod) {
	case HCA_CAP_OPMOD_GET_MAX:
		return "GET_MAX";
	case HCA_CAP_OPMOD_GET_CUR:
		return "GET_CUR";
	default:
		return "Invalid";
	}
}

int mlx5_core_get_caps(struct mlx5_core_dev *dev, struct mlx5_caps *caps,
		       u16 opmod)
{
	u8 in[MLX5_ST_SZ_BYTES(query_hca_cap_in)];
	int out_sz = MLX5_ST_SZ_BYTES(query_hca_cap_out);
	void *out;
=======
static int mlx5_core_get_caps_mode(struct mlx5_core_dev *dev,
				   enum mlx5_cap_type cap_type,
				   enum mlx5_cap_mode cap_mode)
{
	u8 in[MLX5_ST_SZ_BYTES(query_hca_cap_in)];
	int out_sz = MLX5_ST_SZ_BYTES(query_hca_cap_out);
	void *out, *hca_caps;
	u16 opmod = (cap_type << 1) | (cap_mode & 0x01);
>>>>>>> v4.9.227
	int err;

	memset(in, 0, sizeof(in));
	out = kzalloc(out_sz, GFP_KERNEL);
	if (!out)
		return -ENOMEM;
<<<<<<< HEAD
	MLX5_SET(query_hca_cap_in, in, opcode, MLX5_CMD_OP_QUERY_HCA_CAP);
	MLX5_SET(query_hca_cap_in, in, op_mod, opmod);
	err = mlx5_cmd_exec(dev, in, sizeof(in), out, out_sz);
	if (err)
		goto query_ex;

	err = mlx5_cmd_status_to_err_v2(out);
	if (err) {
		mlx5_core_warn(dev, "query max hca cap failed, %d\n", err);
		goto query_ex;
	}
	mlx5_core_dbg(dev, "%s\n", caps_opmod_str(opmod));
	fw2drv_caps(caps, MLX5_ADDR_OF(query_hca_cap_out, out, capability_struct));

=======

	MLX5_SET(query_hca_cap_in, in, opcode, MLX5_CMD_OP_QUERY_HCA_CAP);
	MLX5_SET(query_hca_cap_in, in, op_mod, opmod);
	err = mlx5_cmd_exec(dev, in, sizeof(in), out, out_sz);
	if (err) {
		mlx5_core_warn(dev,
			       "QUERY_HCA_CAP : type(%x) opmode(%x) Failed(%d)\n",
			       cap_type, cap_mode, err);
		goto query_ex;
	}

	hca_caps =  MLX5_ADDR_OF(query_hca_cap_out, out, capability);

	switch (cap_mode) {
	case HCA_CAP_OPMOD_GET_MAX:
		memcpy(dev->hca_caps_max[cap_type], hca_caps,
		       MLX5_UN_SZ_BYTES(hca_cap_union));
		break;
	case HCA_CAP_OPMOD_GET_CUR:
		memcpy(dev->hca_caps_cur[cap_type], hca_caps,
		       MLX5_UN_SZ_BYTES(hca_cap_union));
		break;
	default:
		mlx5_core_warn(dev,
			       "Tried to query dev cap type(%x) with wrong opmode(%x)\n",
			       cap_type, cap_mode);
		err = -EINVAL;
		break;
	}
>>>>>>> v4.9.227
query_ex:
	kfree(out);
	return err;
}

<<<<<<< HEAD
static int set_caps(struct mlx5_core_dev *dev, void *in, int in_sz)
{
	u32 out[MLX5_ST_SZ_DW(set_hca_cap_out)];
	int err;

	memset(out, 0, sizeof(out));

	MLX5_SET(set_hca_cap_in, in, opcode, MLX5_CMD_OP_SET_HCA_CAP);
	err = mlx5_cmd_exec(dev, in, in_sz, out, sizeof(out));
	if (err)
		return err;

	err = mlx5_cmd_status_to_err_v2(out);

=======
int mlx5_core_get_caps(struct mlx5_core_dev *dev, enum mlx5_cap_type cap_type)
{
	int ret;

	ret = mlx5_core_get_caps_mode(dev, cap_type, HCA_CAP_OPMOD_GET_CUR);
	if (ret)
		return ret;
	return mlx5_core_get_caps_mode(dev, cap_type, HCA_CAP_OPMOD_GET_MAX);
}

static int set_caps(struct mlx5_core_dev *dev, void *in, int in_sz, int opmod)
{
	u32 out[MLX5_ST_SZ_DW(set_hca_cap_out)] = {0};

	MLX5_SET(set_hca_cap_in, in, opcode, MLX5_CMD_OP_SET_HCA_CAP);
	MLX5_SET(set_hca_cap_in, in, op_mod, opmod << 1);
	return mlx5_cmd_exec(dev, in, in_sz, out, sizeof(out));
}

static int handle_hca_cap_atomic(struct mlx5_core_dev *dev)
{
	void *set_ctx;
	void *set_hca_cap;
	int set_sz = MLX5_ST_SZ_BYTES(set_hca_cap_in);
	int req_endianness;
	int err;

	if (MLX5_CAP_GEN(dev, atomic)) {
		err = mlx5_core_get_caps(dev, MLX5_CAP_ATOMIC);
		if (err)
			return err;
	} else {
		return 0;
	}

	req_endianness =
		MLX5_CAP_ATOMIC(dev,
				supported_atomic_req_8B_endianess_mode_1);

	if (req_endianness != MLX5_ATOMIC_REQ_MODE_HOST_ENDIANNESS)
		return 0;

	set_ctx = kzalloc(set_sz, GFP_KERNEL);
	if (!set_ctx)
		return -ENOMEM;

	set_hca_cap = MLX5_ADDR_OF(set_hca_cap_in, set_ctx, capability);

	/* Set requestor to host endianness */
	MLX5_SET(atomic_caps, set_hca_cap, atomic_req_8B_endianess_mode,
		 MLX5_ATOMIC_REQ_MODE_HOST_ENDIANNESS);

	err = set_caps(dev, set_ctx, set_sz, MLX5_SET_HCA_CAP_OP_MOD_ATOMIC);

	kfree(set_ctx);
>>>>>>> v4.9.227
	return err;
}

static int handle_hca_cap(struct mlx5_core_dev *dev)
{
	void *set_ctx = NULL;
	struct mlx5_profile *prof = dev->profile;
<<<<<<< HEAD
	struct mlx5_caps *cur_caps = NULL;
	struct mlx5_caps *max_caps = NULL;
	int err = -ENOMEM;
	int set_sz = MLX5_ST_SZ_BYTES(set_hca_cap_in);
=======
	int err = -ENOMEM;
	int set_sz = MLX5_ST_SZ_BYTES(set_hca_cap_in);
	void *set_hca_cap;
>>>>>>> v4.9.227

	set_ctx = kzalloc(set_sz, GFP_KERNEL);
	if (!set_ctx)
		goto query_ex;

<<<<<<< HEAD
	max_caps = kzalloc(sizeof(*max_caps), GFP_KERNEL);
	if (!max_caps)
		goto query_ex;

	cur_caps = kzalloc(sizeof(*cur_caps), GFP_KERNEL);
	if (!cur_caps)
		goto query_ex;

	err = mlx5_core_get_caps(dev, max_caps, HCA_CAP_OPMOD_GET_MAX);
	if (err)
		goto query_ex;

	err = mlx5_core_get_caps(dev, cur_caps, HCA_CAP_OPMOD_GET_CUR);
	if (err)
		goto query_ex;

	/* we limit the size of the pkey table to 128 entries for now */
	cur_caps->gen.pkey_table_size = 128;

	if (prof->mask & MLX5_PROF_MASK_QP_SIZE)
		cur_caps->gen.log_max_qp = prof->log_max_qp;

	/* disable checksum */
	cur_caps->gen.flags &= ~MLX5_DEV_CAP_FLAG_CMDIF_CSUM;

	copy_rw_fields(MLX5_ADDR_OF(set_hca_cap_in, set_ctx, hca_capability_struct),
		       cur_caps);
	err = set_caps(dev, set_ctx, set_sz);

query_ex:
	kfree(cur_caps);
	kfree(max_caps);
	kfree(set_ctx);

=======
	err = mlx5_core_get_caps(dev, MLX5_CAP_GENERAL);
	if (err)
		goto query_ex;

	set_hca_cap = MLX5_ADDR_OF(set_hca_cap_in, set_ctx,
				   capability);
	memcpy(set_hca_cap, dev->hca_caps_cur[MLX5_CAP_GENERAL],
	       MLX5_ST_SZ_BYTES(cmd_hca_cap));

	mlx5_core_dbg(dev, "Current Pkey table size %d Setting new size %d\n",
		      mlx5_to_sw_pkey_sz(MLX5_CAP_GEN(dev, pkey_table_size)),
		      128);
	/* we limit the size of the pkey table to 128 entries for now */
	MLX5_SET(cmd_hca_cap, set_hca_cap, pkey_table_size,
		 to_fw_pkey_sz(dev, 128));

	/* Check log_max_qp from HCA caps to set in current profile */
	if (MLX5_CAP_GEN_MAX(dev, log_max_qp) < profile[prof_sel].log_max_qp) {
		mlx5_core_warn(dev, "log_max_qp value in current profile is %d, changing it to HCA capability limit (%d)\n",
			       profile[prof_sel].log_max_qp,
			       MLX5_CAP_GEN_MAX(dev, log_max_qp));
		profile[prof_sel].log_max_qp = MLX5_CAP_GEN_MAX(dev, log_max_qp);
	}
	if (prof->mask & MLX5_PROF_MASK_QP_SIZE)
		MLX5_SET(cmd_hca_cap, set_hca_cap, log_max_qp,
			 prof->log_max_qp);

	/* disable cmdif checksum */
	MLX5_SET(cmd_hca_cap, set_hca_cap, cmdif_checksum, 0);

	MLX5_SET(cmd_hca_cap, set_hca_cap, log_uar_page_sz, PAGE_SHIFT - 12);

	err = set_caps(dev, set_ctx, set_sz,
		       MLX5_SET_HCA_CAP_OP_MOD_GENERAL_DEVICE);

query_ex:
	kfree(set_ctx);
>>>>>>> v4.9.227
	return err;
}

static int set_hca_ctrl(struct mlx5_core_dev *dev)
{
	struct mlx5_reg_host_endianess he_in;
	struct mlx5_reg_host_endianess he_out;
	int err;

<<<<<<< HEAD
=======
	if (!mlx5_core_is_pf(dev))
		return 0;

>>>>>>> v4.9.227
	memset(&he_in, 0, sizeof(he_in));
	he_in.he = MLX5_SET_HOST_ENDIANNESS;
	err = mlx5_core_access_reg(dev, &he_in,  sizeof(he_in),
					&he_out, sizeof(he_out),
					MLX5_REG_HOST_ENDIANNESS, 0, 1);
	return err;
}

<<<<<<< HEAD
static int mlx5_core_enable_hca(struct mlx5_core_dev *dev)
{
	int err;
	struct mlx5_enable_hca_mbox_in in;
	struct mlx5_enable_hca_mbox_out out;

	memset(&in, 0, sizeof(in));
	memset(&out, 0, sizeof(out));
	in.hdr.opcode = cpu_to_be16(MLX5_CMD_OP_ENABLE_HCA);
	err = mlx5_cmd_exec(dev, &in, sizeof(in), &out, sizeof(out));
	if (err)
		return err;

	if (out.hdr.status)
		return mlx5_cmd_status_to_err(&out.hdr);
=======
int mlx5_core_enable_hca(struct mlx5_core_dev *dev, u16 func_id)
{
	u32 out[MLX5_ST_SZ_DW(enable_hca_out)] = {0};
	u32 in[MLX5_ST_SZ_DW(enable_hca_in)]   = {0};

	MLX5_SET(enable_hca_in, in, opcode, MLX5_CMD_OP_ENABLE_HCA);
	MLX5_SET(enable_hca_in, in, function_id, func_id);
	return mlx5_cmd_exec(dev, &in, sizeof(in), &out, sizeof(out));
}

int mlx5_core_disable_hca(struct mlx5_core_dev *dev, u16 func_id)
{
	u32 out[MLX5_ST_SZ_DW(disable_hca_out)] = {0};
	u32 in[MLX5_ST_SZ_DW(disable_hca_in)]   = {0};

	MLX5_SET(disable_hca_in, in, opcode, MLX5_CMD_OP_DISABLE_HCA);
	MLX5_SET(disable_hca_in, in, function_id, func_id);
	return mlx5_cmd_exec(dev, in, sizeof(in), out, sizeof(out));
}

cycle_t mlx5_read_internal_timer(struct mlx5_core_dev *dev)
{
	u32 timer_h, timer_h1, timer_l;

	timer_h = ioread32be(&dev->iseg->internal_timer_h);
	timer_l = ioread32be(&dev->iseg->internal_timer_l);
	timer_h1 = ioread32be(&dev->iseg->internal_timer_h);
	if (timer_h != timer_h1) /* wrap around */
		timer_l = ioread32be(&dev->iseg->internal_timer_l);

	return (cycle_t)timer_l | (cycle_t)timer_h1 << 32;
}

static int mlx5_irq_set_affinity_hint(struct mlx5_core_dev *mdev, int i)
{
	struct mlx5_priv *priv  = &mdev->priv;
	struct msix_entry *msix = priv->msix_arr;
	int irq                 = msix[i + MLX5_EQ_VEC_COMP_BASE].vector;

	if (!zalloc_cpumask_var(&priv->irq_info[i].mask, GFP_KERNEL)) {
		mlx5_core_warn(mdev, "zalloc_cpumask_var failed");
		return -ENOMEM;
	}

	cpumask_set_cpu(cpumask_local_spread(i, priv->numa_node),
			priv->irq_info[i].mask);

	if (IS_ENABLED(CONFIG_SMP) &&
	    irq_set_affinity_hint(irq, priv->irq_info[i].mask))
		mlx5_core_warn(mdev, "irq_set_affinity_hint failed, irq 0x%.4x", irq);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int mlx5_core_disable_hca(struct mlx5_core_dev *dev)
{
	int err;
	struct mlx5_disable_hca_mbox_in in;
	struct mlx5_disable_hca_mbox_out out;

	memset(&in, 0, sizeof(in));
	memset(&out, 0, sizeof(out));
	in.hdr.opcode = cpu_to_be16(MLX5_CMD_OP_DISABLE_HCA);
	err = mlx5_cmd_exec(dev, &in, sizeof(in), &out, sizeof(out));
	if (err)
		return err;

	if (out.hdr.status)
		return mlx5_cmd_status_to_err(&out.hdr);

	return 0;
}

static int mlx5_dev_init(struct mlx5_core_dev *dev, struct pci_dev *pdev)
{
	struct mlx5_priv *priv = &dev->priv;
	int err;

	dev->pdev = pdev;
=======
static void mlx5_irq_clear_affinity_hint(struct mlx5_core_dev *mdev, int i)
{
	struct mlx5_priv *priv  = &mdev->priv;
	struct msix_entry *msix = priv->msix_arr;
	int irq                 = msix[i + MLX5_EQ_VEC_COMP_BASE].vector;

	irq_set_affinity_hint(irq, NULL);
	free_cpumask_var(priv->irq_info[i].mask);
}

static int mlx5_irq_set_affinity_hints(struct mlx5_core_dev *mdev)
{
	int err;
	int i;

	for (i = 0; i < mdev->priv.eq_table.num_comp_vectors; i++) {
		err = mlx5_irq_set_affinity_hint(mdev, i);
		if (err)
			goto err_out;
	}

	return 0;

err_out:
	for (i--; i >= 0; i--)
		mlx5_irq_clear_affinity_hint(mdev, i);

	return err;
}

static void mlx5_irq_clear_affinity_hints(struct mlx5_core_dev *mdev)
{
	int i;

	for (i = 0; i < mdev->priv.eq_table.num_comp_vectors; i++)
		mlx5_irq_clear_affinity_hint(mdev, i);
}

int mlx5_vector2eqn(struct mlx5_core_dev *dev, int vector, int *eqn,
		    unsigned int *irqn)
{
	struct mlx5_eq_table *table = &dev->priv.eq_table;
	struct mlx5_eq *eq, *n;
	int err = -ENOENT;

	spin_lock(&table->lock);
	list_for_each_entry_safe(eq, n, &table->comp_eqs_list, list) {
		if (eq->index == vector) {
			*eqn = eq->eqn;
			*irqn = eq->irqn;
			err = 0;
			break;
		}
	}
	spin_unlock(&table->lock);

	return err;
}
EXPORT_SYMBOL(mlx5_vector2eqn);

struct mlx5_eq *mlx5_eqn2eq(struct mlx5_core_dev *dev, int eqn)
{
	struct mlx5_eq_table *table = &dev->priv.eq_table;
	struct mlx5_eq *eq;

	spin_lock(&table->lock);
	list_for_each_entry(eq, &table->comp_eqs_list, list)
		if (eq->eqn == eqn) {
			spin_unlock(&table->lock);
			return eq;
		}

	spin_unlock(&table->lock);

	return ERR_PTR(-ENOENT);
}

static void free_comp_eqs(struct mlx5_core_dev *dev)
{
	struct mlx5_eq_table *table = &dev->priv.eq_table;
	struct mlx5_eq *eq, *n;

#ifdef CONFIG_RFS_ACCEL
	if (dev->rmap) {
		free_irq_cpu_rmap(dev->rmap);
		dev->rmap = NULL;
	}
#endif
	spin_lock(&table->lock);
	list_for_each_entry_safe(eq, n, &table->comp_eqs_list, list) {
		list_del(&eq->list);
		spin_unlock(&table->lock);
		if (mlx5_destroy_unmap_eq(dev, eq))
			mlx5_core_warn(dev, "failed to destroy EQ 0x%x\n",
				       eq->eqn);
		kfree(eq);
		spin_lock(&table->lock);
	}
	spin_unlock(&table->lock);
}

static int alloc_comp_eqs(struct mlx5_core_dev *dev)
{
	struct mlx5_eq_table *table = &dev->priv.eq_table;
	char name[MLX5_MAX_IRQ_NAME];
	struct mlx5_eq *eq;
	int ncomp_vec;
	int nent;
	int err;
	int i;

	INIT_LIST_HEAD(&table->comp_eqs_list);
	ncomp_vec = table->num_comp_vectors;
	nent = MLX5_COMP_EQ_SIZE;
#ifdef CONFIG_RFS_ACCEL
	dev->rmap = alloc_irq_cpu_rmap(ncomp_vec);
	if (!dev->rmap)
		return -ENOMEM;
#endif
	for (i = 0; i < ncomp_vec; i++) {
		eq = kzalloc(sizeof(*eq), GFP_KERNEL);
		if (!eq) {
			err = -ENOMEM;
			goto clean;
		}

#ifdef CONFIG_RFS_ACCEL
		irq_cpu_rmap_add(dev->rmap,
				 dev->priv.msix_arr[i + MLX5_EQ_VEC_COMP_BASE].vector);
#endif
		snprintf(name, MLX5_MAX_IRQ_NAME, "mlx5_comp%d", i);
		err = mlx5_create_map_eq(dev, eq,
					 i + MLX5_EQ_VEC_COMP_BASE, nent, 0,
					 name, &dev->priv.uuari.uars[0]);
		if (err) {
			kfree(eq);
			goto clean;
		}
		mlx5_core_dbg(dev, "allocated completion EQN %d\n", eq->eqn);
		eq->index = i;
		spin_lock(&table->lock);
		list_add_tail(&eq->list, &table->comp_eqs_list);
		spin_unlock(&table->lock);
	}

	return 0;

clean:
	free_comp_eqs(dev);
	return err;
}

static int mlx5_core_set_issi(struct mlx5_core_dev *dev)
{
	u32 query_in[MLX5_ST_SZ_DW(query_issi_in)]   = {0};
	u32 query_out[MLX5_ST_SZ_DW(query_issi_out)] = {0};
	u32 sup_issi;
	int err;

	MLX5_SET(query_issi_in, query_in, opcode, MLX5_CMD_OP_QUERY_ISSI);
	err = mlx5_cmd_exec(dev, query_in, sizeof(query_in),
			    query_out, sizeof(query_out));
	if (err) {
		u32 syndrome;
		u8 status;

		mlx5_cmd_mbox_status(query_out, &status, &syndrome);
		if (!status || syndrome == MLX5_DRIVER_SYND) {
			mlx5_core_err(dev, "Failed to query ISSI err(%d) status(%d) synd(%d)\n",
				      err, status, syndrome);
			return err;
		}

		mlx5_core_warn(dev, "Query ISSI is not supported by FW, ISSI is 0\n");
		dev->issi = 0;
		return 0;
	}

	sup_issi = MLX5_GET(query_issi_out, query_out, supported_issi_dw0);

	if (sup_issi & (1 << 1)) {
		u32 set_in[MLX5_ST_SZ_DW(set_issi_in)]   = {0};
		u32 set_out[MLX5_ST_SZ_DW(set_issi_out)] = {0};

		MLX5_SET(set_issi_in, set_in, opcode, MLX5_CMD_OP_SET_ISSI);
		MLX5_SET(set_issi_in, set_in, current_issi, 1);
		err = mlx5_cmd_exec(dev, set_in, sizeof(set_in),
				    set_out, sizeof(set_out));
		if (err) {
			mlx5_core_err(dev, "Failed to set ISSI to 1 err(%d)\n",
				      err);
			return err;
		}

		dev->issi = 1;

		return 0;
	} else if (sup_issi & (1 << 0) || !sup_issi) {
		return 0;
	}

	return -ENOTSUPP;
}


static int mlx5_pci_init(struct mlx5_core_dev *dev, struct mlx5_priv *priv)
{
	struct pci_dev *pdev = dev->pdev;
	int err = 0;

>>>>>>> v4.9.227
	pci_set_drvdata(dev->pdev, dev);
	strncpy(priv->name, dev_name(&pdev->dev), MLX5_MAX_NAME_LEN);
	priv->name[MLX5_MAX_NAME_LEN - 1] = 0;

	mutex_init(&priv->pgdir_mutex);
	INIT_LIST_HEAD(&priv->pgdir_list);
	spin_lock_init(&priv->mkey_lock);

<<<<<<< HEAD
	priv->dbg_root = debugfs_create_dir(dev_name(&pdev->dev), mlx5_debugfs_root);
	if (!priv->dbg_root)
		return -ENOMEM;

	err = pci_enable_device(pdev);
=======
	mutex_init(&priv->alloc_mutex);

	priv->numa_node = dev_to_node(&dev->pdev->dev);

	if (mlx5_debugfs_root)
		priv->dbg_root =
			debugfs_create_dir(pci_name(pdev), mlx5_debugfs_root);

	err = mlx5_pci_enable_device(dev);
>>>>>>> v4.9.227
	if (err) {
		dev_err(&pdev->dev, "Cannot enable PCI device, aborting\n");
		goto err_dbg;
	}

	err = request_bar(pdev);
	if (err) {
		dev_err(&pdev->dev, "error requesting BARs, aborting\n");
		goto err_disable;
	}

	pci_set_master(pdev);

	err = set_dma_caps(pdev);
	if (err) {
		dev_err(&pdev->dev, "Failed setting DMA capabilities mask, aborting\n");
		goto err_clr_master;
	}

	dev->iseg_base = pci_resource_start(dev->pdev, 0);
	dev->iseg = ioremap(dev->iseg_base, sizeof(*dev->iseg));
	if (!dev->iseg) {
		err = -ENOMEM;
		dev_err(&pdev->dev, "Failed mapping initialization segment, aborting\n");
		goto err_clr_master;
	}
<<<<<<< HEAD
	dev_info(&pdev->dev, "firmware version: %d.%d.%d\n", fw_rev_maj(dev),
		 fw_rev_min(dev), fw_rev_sub(dev));

	err = mlx5_cmd_init(dev);
	if (err) {
		dev_err(&pdev->dev, "Failed initializing command interface, aborting\n");
		goto err_unmap;
	}

	mlx5_pagealloc_init(dev);

	err = mlx5_core_enable_hca(dev);
	if (err) {
		dev_err(&pdev->dev, "enable hca failed\n");
		goto err_pagealloc_cleanup;
=======

	return 0;

err_clr_master:
	pci_clear_master(dev->pdev);
	release_bar(dev->pdev);
err_disable:
	mlx5_pci_disable_device(dev);

err_dbg:
	debugfs_remove(priv->dbg_root);
	return err;
}

static void mlx5_pci_close(struct mlx5_core_dev *dev, struct mlx5_priv *priv)
{
	iounmap(dev->iseg);
	pci_clear_master(dev->pdev);
	release_bar(dev->pdev);
	mlx5_pci_disable_device(dev);
	debugfs_remove_recursive(priv->dbg_root);
}

static int mlx5_init_once(struct mlx5_core_dev *dev, struct mlx5_priv *priv)
{
	struct pci_dev *pdev = dev->pdev;
	int err;

	err = mlx5_query_board_id(dev);
	if (err) {
		dev_err(&pdev->dev, "query board id failed\n");
		goto out;
	}

	err = mlx5_eq_init(dev);
	if (err) {
		dev_err(&pdev->dev, "failed to initialize eq\n");
		goto out;
	}

	MLX5_INIT_DOORBELL_LOCK(&priv->cq_uar_lock);

	err = mlx5_init_cq_table(dev);
	if (err) {
		dev_err(&pdev->dev, "failed to initialize cq table\n");
		goto err_eq_cleanup;
	}

	mlx5_init_qp_table(dev);

	mlx5_init_srq_table(dev);

	mlx5_init_mkey_table(dev);

	err = mlx5_init_rl_table(dev);
	if (err) {
		dev_err(&pdev->dev, "Failed to init rate limiting\n");
		goto err_tables_cleanup;
	}

#ifdef CONFIG_MLX5_CORE_EN
	err = mlx5_eswitch_init(dev);
	if (err) {
		dev_err(&pdev->dev, "Failed to init eswitch %d\n", err);
		goto err_rl_cleanup;
	}
#endif

	err = mlx5_sriov_init(dev);
	if (err) {
		dev_err(&pdev->dev, "Failed to init sriov %d\n", err);
		goto err_eswitch_cleanup;
	}

	return 0;

err_eswitch_cleanup:
#ifdef CONFIG_MLX5_CORE_EN
	mlx5_eswitch_cleanup(dev->priv.eswitch);

err_rl_cleanup:
#endif
	mlx5_cleanup_rl_table(dev);

err_tables_cleanup:
	mlx5_cleanup_mkey_table(dev);
	mlx5_cleanup_srq_table(dev);
	mlx5_cleanup_qp_table(dev);
	mlx5_cleanup_cq_table(dev);

err_eq_cleanup:
	mlx5_eq_cleanup(dev);

out:
	return err;
}

static void mlx5_cleanup_once(struct mlx5_core_dev *dev)
{
	mlx5_sriov_cleanup(dev);
#ifdef CONFIG_MLX5_CORE_EN
	mlx5_eswitch_cleanup(dev->priv.eswitch);
#endif
	mlx5_cleanup_rl_table(dev);
	mlx5_cleanup_mkey_table(dev);
	mlx5_cleanup_srq_table(dev);
	mlx5_cleanup_qp_table(dev);
	mlx5_cleanup_cq_table(dev);
	mlx5_eq_cleanup(dev);
}

static int mlx5_load_one(struct mlx5_core_dev *dev, struct mlx5_priv *priv,
			 bool boot)
{
	struct pci_dev *pdev = dev->pdev;
	int err;

	mutex_lock(&dev->intf_state_mutex);
	if (test_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state)) {
		dev_warn(&dev->pdev->dev, "%s: interface is up, NOP\n",
			 __func__);
		goto out;
	}

	dev_info(&pdev->dev, "firmware version: %d.%d.%d\n", fw_rev_maj(dev),
		 fw_rev_min(dev), fw_rev_sub(dev));

	/* on load removing any previous indication of internal error, device is
	 * up
	 */
	dev->state = MLX5_DEVICE_STATE_UP;

	/* wait for firmware to accept initialization segments configurations
	 */
	err = wait_fw_init(dev, FW_PRE_INIT_TIMEOUT_MILI);
	if (err) {
		dev_err(&dev->pdev->dev, "Firmware over %d MS in pre-initializing state, aborting\n",
			FW_PRE_INIT_TIMEOUT_MILI);
		goto out_err;
	}

	err = mlx5_cmd_init(dev);
	if (err) {
		dev_err(&pdev->dev, "Failed initializing command interface, aborting\n");
		goto out_err;
	}

	err = wait_fw_init(dev, FW_INIT_TIMEOUT_MILI);
	if (err) {
		dev_err(&dev->pdev->dev, "Firmware over %d MS in initializing state, aborting\n",
			FW_INIT_TIMEOUT_MILI);
		goto err_cmd_cleanup;
	}

	err = mlx5_core_enable_hca(dev, 0);
	if (err) {
		dev_err(&pdev->dev, "enable hca failed\n");
		goto err_cmd_cleanup;
	}

	err = mlx5_core_set_issi(dev);
	if (err) {
		dev_err(&pdev->dev, "failed to set issi\n");
		goto err_disable_hca;
>>>>>>> v4.9.227
	}

	err = mlx5_satisfy_startup_pages(dev, 1);
	if (err) {
		dev_err(&pdev->dev, "failed to allocate boot pages\n");
		goto err_disable_hca;
	}

	err = set_hca_ctrl(dev);
	if (err) {
		dev_err(&pdev->dev, "set_hca_ctrl failed\n");
		goto reclaim_boot_pages;
	}

	err = handle_hca_cap(dev);
	if (err) {
		dev_err(&pdev->dev, "handle_hca_cap failed\n");
		goto reclaim_boot_pages;
	}

<<<<<<< HEAD
=======
	err = handle_hca_cap_atomic(dev);
	if (err) {
		dev_err(&pdev->dev, "handle_hca_cap_atomic failed\n");
		goto reclaim_boot_pages;
	}

>>>>>>> v4.9.227
	err = mlx5_satisfy_startup_pages(dev, 0);
	if (err) {
		dev_err(&pdev->dev, "failed to allocate init pages\n");
		goto reclaim_boot_pages;
	}

	err = mlx5_pagealloc_start(dev);
	if (err) {
		dev_err(&pdev->dev, "mlx5_pagealloc_start failed\n");
		goto reclaim_boot_pages;
	}

	err = mlx5_cmd_init_hca(dev);
	if (err) {
		dev_err(&pdev->dev, "init hca failed\n");
		goto err_pagealloc_stop;
	}

	mlx5_start_health_poll(dev);

<<<<<<< HEAD
	err = mlx5_cmd_query_hca_cap(dev, &dev->caps);
=======
	err = mlx5_query_hca_caps(dev);
>>>>>>> v4.9.227
	if (err) {
		dev_err(&pdev->dev, "query hca failed\n");
		goto err_stop_poll;
	}

<<<<<<< HEAD
	err = mlx5_cmd_query_adapter(dev);
	if (err) {
		dev_err(&pdev->dev, "query adapter failed\n");
=======
	if (boot && mlx5_init_once(dev, priv)) {
		dev_err(&pdev->dev, "sw objs init failed\n");
>>>>>>> v4.9.227
		goto err_stop_poll;
	}

	err = mlx5_enable_msix(dev);
	if (err) {
		dev_err(&pdev->dev, "enable msix failed\n");
<<<<<<< HEAD
		goto err_stop_poll;
	}

	err = mlx5_eq_init(dev);
	if (err) {
		dev_err(&pdev->dev, "failed to initialize eq\n");
		goto disable_msix;
=======
		goto err_cleanup_once;
>>>>>>> v4.9.227
	}

	err = mlx5_alloc_uuars(dev, &priv->uuari);
	if (err) {
		dev_err(&pdev->dev, "Failed allocating uar, aborting\n");
<<<<<<< HEAD
		goto err_eq_cleanup;
=======
		goto err_disable_msix;
>>>>>>> v4.9.227
	}

	err = mlx5_start_eqs(dev);
	if (err) {
		dev_err(&pdev->dev, "Failed to start pages and async EQs\n");
		goto err_free_uar;
	}

<<<<<<< HEAD
	MLX5_INIT_DOORBELL_LOCK(&priv->cq_uar_lock);

	mlx5_init_cq_table(dev);
	mlx5_init_qp_table(dev);
	mlx5_init_srq_table(dev);
	mlx5_init_mr_table(dev);

	return 0;

err_free_uar:
	mlx5_free_uuars(dev, &priv->uuari);

err_eq_cleanup:
	mlx5_eq_cleanup(dev);

disable_msix:
	mlx5_disable_msix(dev);

err_stop_poll:
	mlx5_stop_health_poll(dev);
	if (mlx5_cmd_teardown_hca(dev)) {
		dev_err(&dev->pdev->dev, "tear_down_hca failed, skip cleanup\n");
		return err;
=======
	err = alloc_comp_eqs(dev);
	if (err) {
		dev_err(&pdev->dev, "Failed to alloc completion EQs\n");
		goto err_stop_eqs;
	}

	err = mlx5_irq_set_affinity_hints(dev);
	if (err) {
		dev_err(&pdev->dev, "Failed to alloc affinity hint cpumask\n");
		goto err_affinity_hints;
	}

	err = mlx5_init_fs(dev);
	if (err) {
		dev_err(&pdev->dev, "Failed to init flow steering\n");
		goto err_fs;
	}

#ifdef CONFIG_MLX5_CORE_EN
	mlx5_eswitch_attach(dev->priv.eswitch);
#endif

	err = mlx5_sriov_attach(dev);
	if (err) {
		dev_err(&pdev->dev, "sriov init failed %d\n", err);
		goto err_sriov;
	}

	if (mlx5_device_registered(dev)) {
		mlx5_attach_device(dev);
	} else {
		err = mlx5_register_device(dev);
		if (err) {
			dev_err(&pdev->dev, "mlx5_register_device failed %d\n", err);
			goto err_reg_dev;
		}
	}

	clear_bit(MLX5_INTERFACE_STATE_DOWN, &dev->intf_state);
	set_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state);
out:
	mutex_unlock(&dev->intf_state_mutex);

	return 0;

err_reg_dev:
	mlx5_sriov_detach(dev);

err_sriov:
#ifdef CONFIG_MLX5_CORE_EN
	mlx5_eswitch_detach(dev->priv.eswitch);
#endif
	mlx5_cleanup_fs(dev);

err_fs:
	mlx5_irq_clear_affinity_hints(dev);

err_affinity_hints:
	free_comp_eqs(dev);

err_stop_eqs:
	mlx5_stop_eqs(dev);

err_free_uar:
	mlx5_free_uuars(dev, &priv->uuari);

err_disable_msix:
	mlx5_disable_msix(dev);

err_cleanup_once:
	if (boot)
		mlx5_cleanup_once(dev);

err_stop_poll:
	mlx5_stop_health_poll(dev, boot);
	if (mlx5_cmd_teardown_hca(dev)) {
		dev_err(&dev->pdev->dev, "tear_down_hca failed, skip cleanup\n");
		goto out_err;
>>>>>>> v4.9.227
	}

err_pagealloc_stop:
	mlx5_pagealloc_stop(dev);

reclaim_boot_pages:
	mlx5_reclaim_startup_pages(dev);

err_disable_hca:
<<<<<<< HEAD
	mlx5_core_disable_hca(dev);

err_pagealloc_cleanup:
	mlx5_pagealloc_cleanup(dev);
	mlx5_cmd_cleanup(dev);

err_unmap:
	iounmap(dev->iseg);

err_clr_master:
	pci_clear_master(dev->pdev);
	release_bar(dev->pdev);

err_disable:
	pci_disable_device(dev->pdev);

err_dbg:
	debugfs_remove(priv->dbg_root);
	return err;
}
EXPORT_SYMBOL(mlx5_dev_init);

static void mlx5_dev_cleanup(struct mlx5_core_dev *dev)
{
	struct mlx5_priv *priv = &dev->priv;

	mlx5_cleanup_srq_table(dev);
	mlx5_cleanup_qp_table(dev);
	mlx5_cleanup_cq_table(dev);
	mlx5_stop_eqs(dev);
	mlx5_free_uuars(dev, &priv->uuari);
	mlx5_eq_cleanup(dev);
	mlx5_disable_msix(dev);
	mlx5_stop_health_poll(dev);
	if (mlx5_cmd_teardown_hca(dev)) {
		dev_err(&dev->pdev->dev, "tear_down_hca failed, skip cleanup\n");
		return;
	}
	mlx5_pagealloc_stop(dev);
	mlx5_reclaim_startup_pages(dev);
	mlx5_core_disable_hca(dev);
	mlx5_pagealloc_cleanup(dev);
	mlx5_cmd_cleanup(dev);
	iounmap(dev->iseg);
	pci_clear_master(dev->pdev);
	release_bar(dev->pdev);
	pci_disable_device(dev->pdev);
	debugfs_remove(priv->dbg_root);
}

static void mlx5_add_device(struct mlx5_interface *intf, struct mlx5_priv *priv)
{
	struct mlx5_device_context *dev_ctx;
	struct mlx5_core_dev *dev = container_of(priv, struct mlx5_core_dev, priv);

	dev_ctx = kmalloc(sizeof(*dev_ctx), GFP_KERNEL);
	if (!dev_ctx) {
		pr_warn("mlx5_add_device: alloc context failed\n");
		return;
	}

	dev_ctx->intf    = intf;
	dev_ctx->context = intf->add(dev);

	if (dev_ctx->context) {
		spin_lock_irq(&priv->ctx_lock);
		list_add_tail(&dev_ctx->list, &priv->ctx_list);
		spin_unlock_irq(&priv->ctx_lock);
	} else {
		kfree(dev_ctx);
	}
}

static void mlx5_remove_device(struct mlx5_interface *intf, struct mlx5_priv *priv)
{
	struct mlx5_device_context *dev_ctx;
	struct mlx5_core_dev *dev = container_of(priv, struct mlx5_core_dev, priv);

	list_for_each_entry(dev_ctx, &priv->ctx_list, list)
		if (dev_ctx->intf == intf) {
			spin_lock_irq(&priv->ctx_lock);
			list_del(&dev_ctx->list);
			spin_unlock_irq(&priv->ctx_lock);

			intf->remove(dev, dev_ctx->context);
			kfree(dev_ctx);
			return;
		}
}
static int mlx5_register_device(struct mlx5_core_dev *dev)
{
	struct mlx5_priv *priv = &dev->priv;
	struct mlx5_interface *intf;

	mutex_lock(&intf_mutex);
	list_add_tail(&priv->dev_list, &dev_list);
	list_for_each_entry(intf, &intf_list, list)
		mlx5_add_device(intf, priv);
	mutex_unlock(&intf_mutex);

	return 0;
}
static void mlx5_unregister_device(struct mlx5_core_dev *dev)
{
	struct mlx5_priv *priv = &dev->priv;
	struct mlx5_interface *intf;

	mutex_lock(&intf_mutex);
	list_for_each_entry(intf, &intf_list, list)
		mlx5_remove_device(intf, priv);
	list_del(&priv->dev_list);
	mutex_unlock(&intf_mutex);
}

int mlx5_register_interface(struct mlx5_interface *intf)
{
	struct mlx5_priv *priv;

	if (!intf->add || !intf->remove)
		return -EINVAL;

	mutex_lock(&intf_mutex);
	list_add_tail(&intf->list, &intf_list);
	list_for_each_entry(priv, &dev_list, dev_list)
		mlx5_add_device(intf, priv);
	mutex_unlock(&intf_mutex);

	return 0;
}
EXPORT_SYMBOL(mlx5_register_interface);

void mlx5_unregister_interface(struct mlx5_interface *intf)
{
	struct mlx5_priv *priv;

	mutex_lock(&intf_mutex);
	list_for_each_entry(priv, &dev_list, dev_list)
	       mlx5_remove_device(intf, priv);
	list_del(&intf->list);
	mutex_unlock(&intf_mutex);
}
EXPORT_SYMBOL(mlx5_unregister_interface);

static void mlx5_core_event(struct mlx5_core_dev *dev, enum mlx5_dev_event event,
			    unsigned long param)
{
	struct mlx5_priv *priv = &dev->priv;
	struct mlx5_device_context *dev_ctx;
	unsigned long flags;

	spin_lock_irqsave(&priv->ctx_lock, flags);

	list_for_each_entry(dev_ctx, &priv->ctx_list, list)
		if (dev_ctx->intf->event)
			dev_ctx->intf->event(dev, dev_ctx->context, event, param);

	spin_unlock_irqrestore(&priv->ctx_lock, flags);
=======
	mlx5_core_disable_hca(dev, 0);

err_cmd_cleanup:
	mlx5_cmd_cleanup(dev);

out_err:
	dev->state = MLX5_DEVICE_STATE_INTERNAL_ERROR;
	mutex_unlock(&dev->intf_state_mutex);

	return err;
}

static int mlx5_unload_one(struct mlx5_core_dev *dev, struct mlx5_priv *priv,
			   bool cleanup)
{
	int err = 0;

	if (cleanup)
		mlx5_drain_health_recovery(dev);

	mutex_lock(&dev->intf_state_mutex);
	if (test_bit(MLX5_INTERFACE_STATE_DOWN, &dev->intf_state)) {
		dev_warn(&dev->pdev->dev, "%s: interface is down, NOP\n",
			 __func__);
		if (cleanup)
			mlx5_cleanup_once(dev);
		goto out;
	}

	if (mlx5_device_registered(dev))
		mlx5_detach_device(dev);

	mlx5_sriov_detach(dev);
#ifdef CONFIG_MLX5_CORE_EN
	mlx5_eswitch_detach(dev->priv.eswitch);
#endif
	mlx5_cleanup_fs(dev);
	mlx5_irq_clear_affinity_hints(dev);
	free_comp_eqs(dev);
	mlx5_stop_eqs(dev);
	mlx5_free_uuars(dev, &priv->uuari);
	mlx5_disable_msix(dev);
	if (cleanup)
		mlx5_cleanup_once(dev);
	mlx5_stop_health_poll(dev, cleanup);
	err = mlx5_cmd_teardown_hca(dev);
	if (err) {
		dev_err(&dev->pdev->dev, "tear_down_hca failed, skip cleanup\n");
		goto out;
	}
	mlx5_pagealloc_stop(dev);
	mlx5_reclaim_startup_pages(dev);
	mlx5_core_disable_hca(dev, 0);
	mlx5_cmd_cleanup(dev);

out:
	clear_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state);
	set_bit(MLX5_INTERFACE_STATE_DOWN, &dev->intf_state);
	mutex_unlock(&dev->intf_state_mutex);
	return err;
>>>>>>> v4.9.227
}

struct mlx5_core_event_handler {
	void (*event)(struct mlx5_core_dev *dev,
		      enum mlx5_dev_event event,
		      void *data);
};

<<<<<<< HEAD
=======
static const struct devlink_ops mlx5_devlink_ops = {
#ifdef CONFIG_MLX5_CORE_EN
	.eswitch_mode_set = mlx5_devlink_eswitch_mode_set,
	.eswitch_mode_get = mlx5_devlink_eswitch_mode_get,
#endif
};

#define MLX5_IB_MOD "mlx5_ib"
>>>>>>> v4.9.227
static int init_one(struct pci_dev *pdev,
		    const struct pci_device_id *id)
{
	struct mlx5_core_dev *dev;
<<<<<<< HEAD
	struct mlx5_priv *priv;
	int err;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev) {
		dev_err(&pdev->dev, "kzalloc failed\n");
		return -ENOMEM;
	}
	priv = &dev->priv;

	pci_set_drvdata(pdev, dev);

	if (prof_sel < 0 || prof_sel >= ARRAY_SIZE(profile)) {
		pr_warn("selected profile out of range, selecting default (%d)\n",
			MLX5_DEFAULT_PROF);
		prof_sel = MLX5_DEFAULT_PROF;
	}
	dev->profile = &profile[prof_sel];
	dev->event = mlx5_core_event;

	INIT_LIST_HEAD(&priv->ctx_list);
	spin_lock_init(&priv->ctx_lock);
	err = mlx5_dev_init(dev, pdev);
	if (err) {
		dev_err(&pdev->dev, "mlx5_dev_init failed %d\n", err);
		goto out;
	}

	err = mlx5_register_device(dev);
	if (err) {
		dev_err(&pdev->dev, "mlx5_register_device failed %d\n", err);
		goto out_init;
	}

	return 0;

out_init:
	mlx5_dev_cleanup(dev);
out:
	kfree(dev);
	return err;
}
static void remove_one(struct pci_dev *pdev)
{
	struct mlx5_core_dev *dev  = pci_get_drvdata(pdev);

	mlx5_unregister_device(dev);
	mlx5_dev_cleanup(dev);
	kfree(dev);
}

static const struct pci_device_id mlx5_core_pci_table[] = {
	{ PCI_VDEVICE(MELLANOX, 4113) }, /* MT4113 Connect-IB */
	{ PCI_VDEVICE(MELLANOX, 4115) }, /* ConnectX-4 */
=======
	struct devlink *devlink;
	struct mlx5_priv *priv;
	int err;

	devlink = devlink_alloc(&mlx5_devlink_ops, sizeof(*dev));
	if (!devlink) {
		dev_err(&pdev->dev, "kzalloc failed\n");
		return -ENOMEM;
	}

	dev = devlink_priv(devlink);
	priv = &dev->priv;
	priv->pci_dev_data = id->driver_data;

	pci_set_drvdata(pdev, dev);

	dev->pdev = pdev;
	dev->event = mlx5_core_event;
	dev->profile = &profile[prof_sel];

	INIT_LIST_HEAD(&priv->ctx_list);
	spin_lock_init(&priv->ctx_lock);
	mutex_init(&dev->pci_status_mutex);
	mutex_init(&dev->intf_state_mutex);
	err = mlx5_pci_init(dev, priv);
	if (err) {
		dev_err(&pdev->dev, "mlx5_pci_init failed with error code %d\n", err);
		goto clean_dev;
	}

	err = mlx5_health_init(dev);
	if (err) {
		dev_err(&pdev->dev, "mlx5_health_init failed with error code %d\n", err);
		goto close_pci;
	}

	mlx5_pagealloc_init(dev);

	err = mlx5_load_one(dev, priv, true);
	if (err) {
		dev_err(&pdev->dev, "mlx5_load_one failed with error code %d\n", err);
		goto clean_health;
	}

	err = request_module_nowait(MLX5_IB_MOD);
	if (err)
		pr_info("failed request module on %s\n", MLX5_IB_MOD);

	err = devlink_register(devlink, &pdev->dev);
	if (err)
		goto clean_load;

	pci_save_state(pdev);
	return 0;

clean_load:
	mlx5_unload_one(dev, priv, true);
clean_health:
	mlx5_pagealloc_cleanup(dev);
	mlx5_health_cleanup(dev);
close_pci:
	mlx5_pci_close(dev, priv);
clean_dev:
	pci_set_drvdata(pdev, NULL);
	devlink_free(devlink);

	return err;
}

static void remove_one(struct pci_dev *pdev)
{
	struct mlx5_core_dev *dev  = pci_get_drvdata(pdev);
	struct devlink *devlink = priv_to_devlink(dev);
	struct mlx5_priv *priv = &dev->priv;

	devlink_unregister(devlink);
	mlx5_unregister_device(dev);

	if (mlx5_unload_one(dev, priv, true)) {
		dev_err(&dev->pdev->dev, "mlx5_unload_one failed\n");
		mlx5_health_cleanup(dev);
		return;
	}

	mlx5_pagealloc_cleanup(dev);
	mlx5_health_cleanup(dev);
	mlx5_pci_close(dev, priv);
	pci_set_drvdata(pdev, NULL);
	devlink_free(devlink);
}

static pci_ers_result_t mlx5_pci_err_detected(struct pci_dev *pdev,
					      pci_channel_state_t state)
{
	struct mlx5_core_dev *dev = pci_get_drvdata(pdev);
	struct mlx5_priv *priv = &dev->priv;

	dev_info(&pdev->dev, "%s was called\n", __func__);

	mlx5_enter_error_state(dev);
	mlx5_unload_one(dev, priv, false);
	/* In case of kernel call drain the health wq */
	if (state) {
		mlx5_drain_health_wq(dev);
		mlx5_pci_disable_device(dev);
	}

	return state == pci_channel_io_perm_failure ?
		PCI_ERS_RESULT_DISCONNECT : PCI_ERS_RESULT_NEED_RESET;
}

/* wait for the device to show vital signs by waiting
 * for the health counter to start counting.
 */
static int wait_vital(struct pci_dev *pdev)
{
	struct mlx5_core_dev *dev = pci_get_drvdata(pdev);
	struct mlx5_core_health *health = &dev->priv.health;
	const int niter = 100;
	u32 last_count = 0;
	u32 count;
	int i;

	for (i = 0; i < niter; i++) {
		count = ioread32be(health->health_counter);
		if (count && count != 0xffffffff) {
			if (last_count && last_count != count) {
				dev_info(&pdev->dev, "Counter value 0x%x after %d iterations\n", count, i);
				return 0;
			}
			last_count = count;
		}
		msleep(50);
	}

	return -ETIMEDOUT;
}

static pci_ers_result_t mlx5_pci_slot_reset(struct pci_dev *pdev)
{
	struct mlx5_core_dev *dev = pci_get_drvdata(pdev);
	int err;

	dev_info(&pdev->dev, "%s was called\n", __func__);

	err = mlx5_pci_enable_device(dev);
	if (err) {
		dev_err(&pdev->dev, "%s: mlx5_pci_enable_device failed with error code: %d\n"
			, __func__, err);
		return PCI_ERS_RESULT_DISCONNECT;
	}

	pci_set_master(pdev);
	pci_restore_state(pdev);
	pci_save_state(pdev);

	if (wait_vital(pdev)) {
		dev_err(&pdev->dev, "%s: wait_vital timed out\n", __func__);
		return PCI_ERS_RESULT_DISCONNECT;
	}

	return PCI_ERS_RESULT_RECOVERED;
}

static void mlx5_pci_resume(struct pci_dev *pdev)
{
	struct mlx5_core_dev *dev = pci_get_drvdata(pdev);
	struct mlx5_priv *priv = &dev->priv;
	int err;

	dev_info(&pdev->dev, "%s was called\n", __func__);

	err = mlx5_load_one(dev, priv, false);
	if (err)
		dev_err(&pdev->dev, "%s: mlx5_load_one failed with error code: %d\n"
			, __func__, err);
	else
		dev_info(&pdev->dev, "%s: device recovered\n", __func__);
}

static const struct pci_error_handlers mlx5_err_handler = {
	.error_detected = mlx5_pci_err_detected,
	.slot_reset	= mlx5_pci_slot_reset,
	.resume		= mlx5_pci_resume
};

static void shutdown(struct pci_dev *pdev)
{
	struct mlx5_core_dev *dev  = pci_get_drvdata(pdev);
	struct mlx5_priv *priv = &dev->priv;

	dev_info(&pdev->dev, "Shutdown was called\n");
	/* Notify mlx5 clients that the kernel is being shut down */
	set_bit(MLX5_INTERFACE_STATE_SHUTDOWN, &dev->intf_state);
	mlx5_unload_one(dev, priv, false);
	mlx5_pci_disable_device(dev);
}

static const struct pci_device_id mlx5_core_pci_table[] = {
	{ PCI_VDEVICE(MELLANOX, 0x1011) },			/* Connect-IB */
	{ PCI_VDEVICE(MELLANOX, 0x1012), MLX5_PCI_DEV_IS_VF},	/* Connect-IB VF */
	{ PCI_VDEVICE(MELLANOX, 0x1013) },			/* ConnectX-4 */
	{ PCI_VDEVICE(MELLANOX, 0x1014), MLX5_PCI_DEV_IS_VF},	/* ConnectX-4 VF */
	{ PCI_VDEVICE(MELLANOX, 0x1015) },			/* ConnectX-4LX */
	{ PCI_VDEVICE(MELLANOX, 0x1016), MLX5_PCI_DEV_IS_VF},	/* ConnectX-4LX VF */
	{ PCI_VDEVICE(MELLANOX, 0x1017) },			/* ConnectX-5, PCIe 3.0 */
	{ PCI_VDEVICE(MELLANOX, 0x1018), MLX5_PCI_DEV_IS_VF},	/* ConnectX-5 VF */
	{ PCI_VDEVICE(MELLANOX, 0x1019) },			/* ConnectX-5, PCIe 4.0 */
>>>>>>> v4.9.227
	{ 0, }
};

MODULE_DEVICE_TABLE(pci, mlx5_core_pci_table);

<<<<<<< HEAD
=======
void mlx5_disable_device(struct mlx5_core_dev *dev)
{
	mlx5_pci_err_detected(dev->pdev, 0);
}

void mlx5_recover_device(struct mlx5_core_dev *dev)
{
	mlx5_pci_disable_device(dev);
	if (mlx5_pci_slot_reset(dev->pdev) == PCI_ERS_RESULT_RECOVERED)
		mlx5_pci_resume(dev->pdev);
}

>>>>>>> v4.9.227
static struct pci_driver mlx5_core_driver = {
	.name           = DRIVER_NAME,
	.id_table       = mlx5_core_pci_table,
	.probe          = init_one,
<<<<<<< HEAD
	.remove         = remove_one
};

=======
	.remove         = remove_one,
	.shutdown	= shutdown,
	.err_handler	= &mlx5_err_handler,
	.sriov_configure   = mlx5_core_sriov_configure,
};

static void mlx5_core_verify_params(void)
{
	if (prof_sel >= ARRAY_SIZE(profile)) {
		pr_warn("mlx5_core: WARNING: Invalid module parameter prof_sel %d, valid range 0-%zu, changing back to default(%d)\n",
			prof_sel,
			ARRAY_SIZE(profile) - 1,
			MLX5_DEFAULT_PROF);
		prof_sel = MLX5_DEFAULT_PROF;
	}
}

>>>>>>> v4.9.227
static int __init init(void)
{
	int err;

<<<<<<< HEAD
	mlx5_register_debugfs();
	mlx5_core_wq = create_singlethread_workqueue("mlx5_core_wq");
	if (!mlx5_core_wq) {
		err = -ENOMEM;
		goto err_debug;
	}
	mlx5_health_init();

	err = pci_register_driver(&mlx5_core_driver);
	if (err)
		goto err_health;

	return 0;

err_health:
	mlx5_health_cleanup();
	destroy_workqueue(mlx5_core_wq);
=======
	mlx5_core_verify_params();
	mlx5_register_debugfs();

	err = pci_register_driver(&mlx5_core_driver);
	if (err)
		goto err_debug;

#ifdef CONFIG_MLX5_CORE_EN
	mlx5e_init();
#endif

	return 0;

>>>>>>> v4.9.227
err_debug:
	mlx5_unregister_debugfs();
	return err;
}

static void __exit cleanup(void)
{
<<<<<<< HEAD
	pci_unregister_driver(&mlx5_core_driver);
	mlx5_health_cleanup();
	destroy_workqueue(mlx5_core_wq);
=======
#ifdef CONFIG_MLX5_CORE_EN
	mlx5e_cleanup();
#endif
	pci_unregister_driver(&mlx5_core_driver);
>>>>>>> v4.9.227
	mlx5_unregister_debugfs();
}

module_init(init);
module_exit(cleanup);
