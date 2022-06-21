/*
 * This file is part of the Chelsio T4 Ethernet driver for Linux.
 *
<<<<<<< HEAD
 * Copyright (c) 2003-2014 Chelsio Communications, Inc. All rights reserved.
=======
 * Copyright (c) 2003-2016 Chelsio Communications, Inc. All rights reserved.
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

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/bitmap.h>
#include <linux/crc32.h>
#include <linux/ctype.h>
#include <linux/debugfs.h>
#include <linux/err.h>
#include <linux/etherdevice.h>
#include <linux/firmware.h>
#include <linux/if.h>
#include <linux/if_vlan.h>
#include <linux/init.h>
#include <linux/log2.h>
#include <linux/mdio.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#include <linux/netdevice.h>
#include <linux/pci.h>
#include <linux/aer.h>
#include <linux/rtnetlink.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/sockios.h>
#include <linux/vmalloc.h>
#include <linux/workqueue.h>
#include <net/neighbour.h>
#include <net/netevent.h>
#include <net/addrconf.h>
<<<<<<< HEAD
#include <asm/uaccess.h>

#include "cxgb4.h"
#include "t4_regs.h"
#include "t4_msg.h"
#include "t4fw_api.h"
#include "cxgb4_dcb.h"
#include "l2t.h"

#include <../drivers/net/bonding/bonding.h>
=======
#include <net/bonding.h>
#include <net/addrconf.h>
#include <asm/uaccess.h>
#include <linux/crash_dump.h>

#include "cxgb4.h"
#include "cxgb4_filter.h"
#include "t4_regs.h"
#include "t4_values.h"
#include "t4_msg.h"
#include "t4fw_api.h"
#include "t4fw_version.h"
#include "cxgb4_dcb.h"
#include "cxgb4_debugfs.h"
#include "clip_tbl.h"
#include "l2t.h"
#include "sched.h"
#include "cxgb4_tc_u32.h"

char cxgb4_driver_name[] = KBUILD_MODNAME;
>>>>>>> v4.9.227

#ifdef DRV_VERSION
#undef DRV_VERSION
#endif
#define DRV_VERSION "2.0.0-ko"
<<<<<<< HEAD
#define DRV_DESC "Chelsio T4/T5 Network Driver"

/*
 * Max interrupt hold-off timer value in us.  Queues fall back to this value
 * under extreme memory pressure so it's largish to give the system time to
 * recover.
 */
#define MAX_SGE_TIMERVAL 200U

enum {
	/*
	 * Physical Function provisioning constants.
	 */
	PFRES_NVI = 4,			/* # of Virtual Interfaces */
	PFRES_NETHCTRL = 128,		/* # of EQs used for ETH or CTRL Qs */
	PFRES_NIQFLINT = 128,		/* # of ingress Qs/w Free List(s)/intr
					 */
	PFRES_NEQ = 256,		/* # of egress queues */
	PFRES_NIQ = 0,			/* # of ingress queues */
	PFRES_TC = 0,			/* PCI-E traffic class */
	PFRES_NEXACTF = 128,		/* # of exact MPS filters */

	PFRES_R_CAPS = FW_CMD_CAP_PF,
	PFRES_WX_CAPS = FW_CMD_CAP_PF,

#ifdef CONFIG_PCI_IOV
	/*
	 * Virtual Function provisioning constants.  We need two extra Ingress
	 * Queues with Interrupt capability to serve as the VF's Firmware
	 * Event Queue and Forwarded Interrupt Queue (when using MSI mode) --
	 * neither will have Free Lists associated with them).  For each
	 * Ethernet/Control Egress Queue and for each Free List, we need an
	 * Egress Context.
	 */
	VFRES_NPORTS = 1,		/* # of "ports" per VF */
	VFRES_NQSETS = 2,		/* # of "Queue Sets" per VF */

	VFRES_NVI = VFRES_NPORTS,	/* # of Virtual Interfaces */
	VFRES_NETHCTRL = VFRES_NQSETS,	/* # of EQs used for ETH or CTRL Qs */
	VFRES_NIQFLINT = VFRES_NQSETS+2,/* # of ingress Qs/w Free List(s)/intr */
	VFRES_NEQ = VFRES_NQSETS*2,	/* # of egress queues */
	VFRES_NIQ = 0,			/* # of non-fl/int ingress queues */
	VFRES_TC = 0,			/* PCI-E traffic class */
	VFRES_NEXACTF = 16,		/* # of exact MPS filters */

	VFRES_R_CAPS = FW_CMD_CAP_DMAQ|FW_CMD_CAP_VF|FW_CMD_CAP_PORT,
	VFRES_WX_CAPS = FW_CMD_CAP_DMAQ|FW_CMD_CAP_VF,
#endif
};

/*
 * Provide a Port Access Rights Mask for the specified PF/VF.  This is very
 * static and likely not to be useful in the long run.  We really need to
 * implement some form of persistent configuration which the firmware
 * controls.
 */
static unsigned int pfvfres_pmask(struct adapter *adapter,
				  unsigned int pf, unsigned int vf)
{
	unsigned int portn, portvec;

	/*
	 * Give PF's access to all of the ports.
	 */
	if (vf == 0)
		return FW_PFVF_CMD_PMASK_MASK;

	/*
	 * For VFs, we'll assign them access to the ports based purely on the
	 * PF.  We assign active ports in order, wrapping around if there are
	 * fewer active ports than PFs: e.g. active port[pf % nports].
	 * Unfortunately the adapter's port_info structs haven't been
	 * initialized yet so we have to compute this.
	 */
	if (adapter->params.nports == 0)
		return 0;

	portn = pf % adapter->params.nports;
	portvec = adapter->params.portvec;
	for (;;) {
		/*
		 * Isolate the lowest set bit in the port vector.  If we're at
		 * the port number that we want, return that as the pmask.
		 * otherwise mask that bit out of the port vector and
		 * decrement our port number ...
		 */
		unsigned int pmask = portvec ^ (portvec & (portvec-1));
		if (portn == 0)
			return pmask;
		portn--;
		portvec &= ~pmask;
	}
	/*NOTREACHED*/
}

enum {
	MAX_TXQ_ENTRIES      = 16384,
	MAX_CTRL_TXQ_ENTRIES = 1024,
	MAX_RSPQ_ENTRIES     = 16384,
	MAX_RX_BUFFERS       = 16384,
	MIN_TXQ_ENTRIES      = 32,
	MIN_CTRL_TXQ_ENTRIES = 32,
	MIN_RSPQ_ENTRIES     = 128,
	MIN_FL_ENTRIES       = 16
};

/* Host shadow copy of ingress filter entry.  This is in host native format
 * and doesn't match the ordering or bit order, etc. of the hardware of the
 * firmware command.  The use of bit-field structure elements is purely to
 * remind ourselves of the field size limitations and save memory in the case
 * where the filter table is large.
 */
struct filter_entry {
	/* Administrative fields for filter.
	 */
	u32 valid:1;            /* filter allocated and valid */
	u32 locked:1;           /* filter is administratively locked */

	u32 pending:1;          /* filter action is pending firmware reply */
	u32 smtidx:8;           /* Source MAC Table index for smac */
	struct l2t_entry *l2t;  /* Layer Two Table entry for dmac */

	/* The filter itself.  Most of this is a straight copy of information
	 * provided by the extended ioctl().  Some fields are translated to
	 * internal forms -- for instance the Ingress Queue ID passed in from
	 * the ioctl() is translated into the Absolute Ingress Queue ID.
	 */
	struct ch_filter_specification fs;
};
=======
const char cxgb4_driver_version[] = DRV_VERSION;
#define DRV_DESC "Chelsio T4/T5/T6 Network Driver"
>>>>>>> v4.9.227

#define DFLT_MSG_ENABLE (NETIF_MSG_DRV | NETIF_MSG_PROBE | NETIF_MSG_LINK | \
			 NETIF_MSG_TIMER | NETIF_MSG_IFDOWN | NETIF_MSG_IFUP |\
			 NETIF_MSG_RX_ERR | NETIF_MSG_TX_ERR)

<<<<<<< HEAD
#define CH_DEVICE(devid, data) { PCI_VDEVICE(CHELSIO, devid), (data) }

static const struct pci_device_id cxgb4_pci_tbl[] = {
	CH_DEVICE(0xa000, 0),  /* PE10K */
	CH_DEVICE(0x4001, -1),
	CH_DEVICE(0x4002, -1),
	CH_DEVICE(0x4003, -1),
	CH_DEVICE(0x4004, -1),
	CH_DEVICE(0x4005, -1),
	CH_DEVICE(0x4006, -1),
	CH_DEVICE(0x4007, -1),
	CH_DEVICE(0x4008, -1),
	CH_DEVICE(0x4009, -1),
	CH_DEVICE(0x400a, -1),
	CH_DEVICE(0x400d, -1),
	CH_DEVICE(0x400e, -1),
	CH_DEVICE(0x4080, -1),
	CH_DEVICE(0x4081, -1),
	CH_DEVICE(0x4082, -1),
	CH_DEVICE(0x4083, -1),
	CH_DEVICE(0x4084, -1),
	CH_DEVICE(0x4085, -1),
	CH_DEVICE(0x4086, -1),
	CH_DEVICE(0x4087, -1),
	CH_DEVICE(0x4088, -1),
	CH_DEVICE(0x4401, 4),
	CH_DEVICE(0x4402, 4),
	CH_DEVICE(0x4403, 4),
	CH_DEVICE(0x4404, 4),
	CH_DEVICE(0x4405, 4),
	CH_DEVICE(0x4406, 4),
	CH_DEVICE(0x4407, 4),
	CH_DEVICE(0x4408, 4),
	CH_DEVICE(0x4409, 4),
	CH_DEVICE(0x440a, 4),
	CH_DEVICE(0x440d, 4),
	CH_DEVICE(0x440e, 4),
	CH_DEVICE(0x4480, 4),
	CH_DEVICE(0x4481, 4),
	CH_DEVICE(0x4482, 4),
	CH_DEVICE(0x4483, 4),
	CH_DEVICE(0x4484, 4),
	CH_DEVICE(0x4485, 4),
	CH_DEVICE(0x4486, 4),
	CH_DEVICE(0x4487, 4),
	CH_DEVICE(0x4488, 4),
	CH_DEVICE(0x5001, 4),
	CH_DEVICE(0x5002, 4),
	CH_DEVICE(0x5003, 4),
	CH_DEVICE(0x5004, 4),
	CH_DEVICE(0x5005, 4),
	CH_DEVICE(0x5006, 4),
	CH_DEVICE(0x5007, 4),
	CH_DEVICE(0x5008, 4),
	CH_DEVICE(0x5009, 4),
	CH_DEVICE(0x500A, 4),
	CH_DEVICE(0x500B, 4),
	CH_DEVICE(0x500C, 4),
	CH_DEVICE(0x500D, 4),
	CH_DEVICE(0x500E, 4),
	CH_DEVICE(0x500F, 4),
	CH_DEVICE(0x5010, 4),
	CH_DEVICE(0x5011, 4),
	CH_DEVICE(0x5012, 4),
	CH_DEVICE(0x5013, 4),
	CH_DEVICE(0x5014, 4),
	CH_DEVICE(0x5015, 4),
	CH_DEVICE(0x5080, 4),
	CH_DEVICE(0x5081, 4),
	CH_DEVICE(0x5082, 4),
	CH_DEVICE(0x5083, 4),
	CH_DEVICE(0x5084, 4),
	CH_DEVICE(0x5085, 4),
	CH_DEVICE(0x5086, 4),
	CH_DEVICE(0x5087, 4),
	CH_DEVICE(0x5088, 4),
	CH_DEVICE(0x5401, 4),
	CH_DEVICE(0x5402, 4),
	CH_DEVICE(0x5403, 4),
	CH_DEVICE(0x5404, 4),
	CH_DEVICE(0x5405, 4),
	CH_DEVICE(0x5406, 4),
	CH_DEVICE(0x5407, 4),
	CH_DEVICE(0x5408, 4),
	CH_DEVICE(0x5409, 4),
	CH_DEVICE(0x540A, 4),
	CH_DEVICE(0x540B, 4),
	CH_DEVICE(0x540C, 4),
	CH_DEVICE(0x540D, 4),
	CH_DEVICE(0x540E, 4),
	CH_DEVICE(0x540F, 4),
	CH_DEVICE(0x5410, 4),
	CH_DEVICE(0x5411, 4),
	CH_DEVICE(0x5412, 4),
	CH_DEVICE(0x5413, 4),
	CH_DEVICE(0x5414, 4),
	CH_DEVICE(0x5415, 4),
	CH_DEVICE(0x5480, 4),
	CH_DEVICE(0x5481, 4),
	CH_DEVICE(0x5482, 4),
	CH_DEVICE(0x5483, 4),
	CH_DEVICE(0x5484, 4),
	CH_DEVICE(0x5485, 4),
	CH_DEVICE(0x5486, 4),
	CH_DEVICE(0x5487, 4),
	CH_DEVICE(0x5488, 4),
	{ 0, }
};

#define FW4_FNAME "cxgb4/t4fw.bin"
#define FW5_FNAME "cxgb4/t5fw.bin"
#define FW4_CFNAME "cxgb4/t4-config.txt"
#define FW5_CFNAME "cxgb4/t5-config.txt"
=======
/* Macros needed to support the PCI Device ID Table ...
 */
#define CH_PCI_DEVICE_ID_TABLE_DEFINE_BEGIN \
	static const struct pci_device_id cxgb4_pci_tbl[] = {
#define CH_PCI_DEVICE_ID_FUNCTION 0x4

/* Include PCI Device IDs for both PF4 and PF0-3 so our PCI probe() routine is
 * called for both.
 */
#define CH_PCI_DEVICE_ID_FUNCTION2 0x0

#define CH_PCI_ID_TABLE_ENTRY(devid) \
		{PCI_VDEVICE(CHELSIO, (devid)), 4}

#define CH_PCI_DEVICE_ID_TABLE_DEFINE_END \
		{ 0, } \
	}

#include "t4_pci_id_tbl.h"

#define FW4_FNAME "cxgb4/t4fw.bin"
#define FW5_FNAME "cxgb4/t5fw.bin"
#define FW6_FNAME "cxgb4/t6fw.bin"
#define FW4_CFNAME "cxgb4/t4-config.txt"
#define FW5_CFNAME "cxgb4/t5-config.txt"
#define FW6_CFNAME "cxgb4/t6-config.txt"
#define PHY_AQ1202_FIRMWARE "cxgb4/aq1202_fw.cld"
#define PHY_BCM84834_FIRMWARE "cxgb4/bcm8483.bin"
#define PHY_AQ1202_DEVICEID 0x4409
#define PHY_BCM84834_DEVICEID 0x4486
>>>>>>> v4.9.227

MODULE_DESCRIPTION(DRV_DESC);
MODULE_AUTHOR("Chelsio Communications");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_VERSION(DRV_VERSION);
MODULE_DEVICE_TABLE(pci, cxgb4_pci_tbl);
MODULE_FIRMWARE(FW4_FNAME);
MODULE_FIRMWARE(FW5_FNAME);
<<<<<<< HEAD
=======
MODULE_FIRMWARE(FW6_FNAME);
>>>>>>> v4.9.227

/*
 * Normally we're willing to become the firmware's Master PF but will be happy
 * if another PF has already become the Master and initialized the adapter.
 * Setting "force_init" will cause this driver to forcibly establish itself as
 * the Master PF and initialize the adapter.
 */
static uint force_init;

module_param(force_init, uint, 0644);
<<<<<<< HEAD
MODULE_PARM_DESC(force_init, "Forcibly become Master PF and initialize adapter");

/*
 * Normally if the firmware we connect to has Configuration File support, we
 * use that and only fall back to the old Driver-based initialization if the
 * Configuration File fails for some reason.  If force_old_init is set, then
 * we'll always use the old Driver-based initialization sequence.
 */
static uint force_old_init;

module_param(force_old_init, uint, 0644);
MODULE_PARM_DESC(force_old_init, "Force old initialization sequence");
=======
MODULE_PARM_DESC(force_init, "Forcibly become Master PF and initialize adapter,"
		 "deprecated parameter");
>>>>>>> v4.9.227

static int dflt_msg_enable = DFLT_MSG_ENABLE;

module_param(dflt_msg_enable, int, 0644);
<<<<<<< HEAD
MODULE_PARM_DESC(dflt_msg_enable, "Chelsio T4 default message enable bitmap");
=======
MODULE_PARM_DESC(dflt_msg_enable, "Chelsio T4 default message enable bitmap, "
		 "deprecated parameter");
>>>>>>> v4.9.227

/*
 * The driver uses the best interrupt scheme available on a platform in the
 * order MSI-X, MSI, legacy INTx interrupts.  This parameter determines which
 * of these schemes the driver may consider as follows:
 *
 * msi = 2: choose from among all three options
 * msi = 1: only consider MSI and INTx interrupts
 * msi = 0: force INTx interrupts
 */
static int msi = 2;

module_param(msi, int, 0644);
MODULE_PARM_DESC(msi, "whether to use INTx (0), MSI (1) or MSI-X (2)");

/*
<<<<<<< HEAD
 * Queue interrupt hold-off timer values.  Queues default to the first of these
 * upon creation.
 */
static unsigned int intr_holdoff[SGE_NTIMERS - 1] = { 5, 10, 20, 50, 100 };

module_param_array(intr_holdoff, uint, NULL, 0644);
MODULE_PARM_DESC(intr_holdoff, "values for queue interrupt hold-off timers "
		 "0..4 in microseconds");

static unsigned int intr_cnt[SGE_NCOUNTERS - 1] = { 4, 8, 16 };

module_param_array(intr_cnt, uint, NULL, 0644);
MODULE_PARM_DESC(intr_cnt,
		 "thresholds 1..3 for queue interrupt packet counters");

/*
=======
>>>>>>> v4.9.227
 * Normally we tell the chip to deliver Ingress Packets into our DMA buffers
 * offset by 2 bytes in order to have the IP headers line up on 4-byte
 * boundaries.  This is a requirement for many architectures which will throw
 * a machine check fault if an attempt is made to access one of the 4-byte IP
 * header fields on a non-4-byte boundary.  And it's a major performance issue
 * even on some architectures which allow it like some implementations of the
 * x86 ISA.  However, some architectures don't mind this and for some very
 * edge-case performance sensitive applications (like forwarding large volumes
 * of small packets), setting this DMA offset to 0 will decrease the number of
 * PCI-E Bus transfers enough to measurably affect performance.
 */
static int rx_dma_offset = 2;

<<<<<<< HEAD
static bool vf_acls;

#ifdef CONFIG_PCI_IOV
module_param(vf_acls, bool, 0644);
MODULE_PARM_DESC(vf_acls, "if set enable virtualization L2 ACL enforcement");

=======
#ifdef CONFIG_PCI_IOV
>>>>>>> v4.9.227
/* Configure the number of PCI-E Virtual Function which are to be instantiated
 * on SR-IOV Capable Physical Functions.
 */
static unsigned int num_vf[NUM_OF_PF_WITH_SRIOV];

module_param_array(num_vf, uint, NULL, 0644);
<<<<<<< HEAD
MODULE_PARM_DESC(num_vf, "number of VFs for each of PFs 0-3");
=======
MODULE_PARM_DESC(num_vf, "number of VFs for each of PFs 0-3, deprecated parameter - please use the pci sysfs interface.");
>>>>>>> v4.9.227
#endif

/* TX Queue select used to determine what algorithm to use for selecting TX
 * queue. Select between the kernel provided function (select_queue=0) or user
 * cxgb_select_queue function (select_queue=1)
 *
 * Default: select_queue=0
 */
static int select_queue;
module_param(select_queue, int, 0644);
MODULE_PARM_DESC(select_queue,
		 "Select between kernel provided method of selecting or driver method of selecting TX queue. Default is kernel method.");

<<<<<<< HEAD
/*
 * The filter TCAM has a fixed portion and a variable portion.  The fixed
 * portion can match on source/destination IP IPv4/IPv6 addresses and TCP/UDP
 * ports.  The variable portion is 36 bits which can include things like Exact
 * Match MAC Index (9 bits), Ether Type (16 bits), IP Protocol (8 bits),
 * [Inner] VLAN Tag (17 bits), etc. which, if all were somehow selected, would
 * far exceed the 36-bit budget for this "compressed" header portion of the
 * filter.  Thus, we have a scarce resource which must be carefully managed.
 *
 * By default we set this up to mostly match the set of filter matching
 * capabilities of T3 but with accommodations for some of T4's more
 * interesting features:
 *
 *   { IP Fragment (1), MPS Match Type (3), IP Protocol (8),
 *     [Inner] VLAN (17), Port (3), FCoE (1) }
 */
enum {
	TP_VLAN_PRI_MAP_DEFAULT = HW_TPL_FR_MT_PR_IV_P_FC,
	TP_VLAN_PRI_MAP_FIRST = FCOE_SHIFT,
	TP_VLAN_PRI_MAP_LAST = FRAGMENTATION_SHIFT,
};

static unsigned int tp_vlan_pri_map = TP_VLAN_PRI_MAP_DEFAULT;

module_param(tp_vlan_pri_map, uint, 0644);
MODULE_PARM_DESC(tp_vlan_pri_map, "global compressed filter configuration");

static struct dentry *cxgb4_debugfs_root;

static LIST_HEAD(adapter_list);
static DEFINE_MUTEX(uld_mutex);
/* Adapter list to be accessed from atomic context */
static LIST_HEAD(adap_rcu_list);
static DEFINE_SPINLOCK(adap_rcu_lock);
static struct cxgb4_uld_info ulds[CXGB4_ULD_MAX];
static const char *uld_str[] = { "RDMA", "iSCSI" };
=======
static struct dentry *cxgb4_debugfs_root;

LIST_HEAD(adapter_list);
DEFINE_MUTEX(uld_mutex);
>>>>>>> v4.9.227

static void link_report(struct net_device *dev)
{
	if (!netif_carrier_ok(dev))
		netdev_info(dev, "link down\n");
	else {
		static const char *fc[] = { "no", "Rx", "Tx", "Tx/Rx" };

<<<<<<< HEAD
		const char *s = "10Mbps";
=======
		const char *s;
>>>>>>> v4.9.227
		const struct port_info *p = netdev_priv(dev);

		switch (p->link_cfg.speed) {
		case 10000:
			s = "10Gbps";
			break;
		case 1000:
			s = "1000Mbps";
			break;
		case 100:
			s = "100Mbps";
			break;
		case 40000:
			s = "40Gbps";
			break;
<<<<<<< HEAD
=======
		default:
			pr_info("%s: unsupported speed: %d\n",
				dev->name, p->link_cfg.speed);
			return;
>>>>>>> v4.9.227
		}

		netdev_info(dev, "link up, %s, full-duplex, %s PAUSE\n", s,
			    fc[p->link_cfg.fc]);
	}
}

#ifdef CONFIG_CHELSIO_T4_DCB
/* Set up/tear down Data Center Bridging Priority mapping for a net device. */
static void dcb_tx_queue_prio_enable(struct net_device *dev, int enable)
{
	struct port_info *pi = netdev_priv(dev);
	struct adapter *adap = pi->adapter;
	struct sge_eth_txq *txq = &adap->sge.ethtxq[pi->first_qset];
	int i;

	/* We use a simple mapping of Port TX Queue Index to DCB
	 * Priority when we're enabling DCB.
	 */
	for (i = 0; i < pi->nqsets; i++, txq++) {
		u32 name, value;
		int err;

<<<<<<< HEAD
		name = (FW_PARAMS_MNEM(FW_PARAMS_MNEM_DMAQ) |
			FW_PARAMS_PARAM_X(FW_PARAMS_PARAM_DMAQ_EQ_DCBPRIO_ETH) |
			FW_PARAMS_PARAM_YZ(txq->q.cntxt_id));
=======
		name = (FW_PARAMS_MNEM_V(FW_PARAMS_MNEM_DMAQ) |
			FW_PARAMS_PARAM_X_V(
				FW_PARAMS_PARAM_DMAQ_EQ_DCBPRIO_ETH) |
			FW_PARAMS_PARAM_YZ_V(txq->q.cntxt_id));
>>>>>>> v4.9.227
		value = enable ? i : 0xffffffff;

		/* Since we can be called while atomic (from "interrupt
		 * level") we need to issue the Set Parameters Commannd
		 * without sleeping (timeout < 0).
		 */
<<<<<<< HEAD
		err = t4_set_params_nosleep(adap, adap->mbox, adap->fn, 0, 1,
					    &name, &value);
=======
		err = t4_set_params_timeout(adap, adap->mbox, adap->pf, 0, 1,
					    &name, &value,
					    -FW_CMD_MAX_TIMEOUT);
>>>>>>> v4.9.227

		if (err)
			dev_err(adap->pdev_dev,
				"Can't %s DCB Priority on port %d, TX Queue %d: err=%d\n",
				enable ? "set" : "unset", pi->port_id, i, -err);
		else
			txq->dcb_prio = enable ? value : 0;
	}
}
<<<<<<< HEAD
=======

static int cxgb4_dcb_enabled(const struct net_device *dev)
{
	struct port_info *pi = netdev_priv(dev);

	if (!pi->dcb.enabled)
		return 0;

	return ((pi->dcb.state == CXGB4_DCB_STATE_FW_ALLSYNCED) ||
		(pi->dcb.state == CXGB4_DCB_STATE_HOST));
}
>>>>>>> v4.9.227
#endif /* CONFIG_CHELSIO_T4_DCB */

void t4_os_link_changed(struct adapter *adapter, int port_id, int link_stat)
{
	struct net_device *dev = adapter->port[port_id];

	/* Skip changes from disabled ports. */
	if (netif_running(dev) && link_stat != netif_carrier_ok(dev)) {
		if (link_stat)
			netif_carrier_on(dev);
		else {
#ifdef CONFIG_CHELSIO_T4_DCB
<<<<<<< HEAD
			cxgb4_dcb_state_init(dev);
			dcb_tx_queue_prio_enable(dev, false);
=======
			if (cxgb4_dcb_enabled(dev)) {
				cxgb4_dcb_state_init(dev);
				dcb_tx_queue_prio_enable(dev, false);
			}
>>>>>>> v4.9.227
#endif /* CONFIG_CHELSIO_T4_DCB */
			netif_carrier_off(dev);
		}

		link_report(dev);
	}
}

void t4_os_portmod_changed(const struct adapter *adap, int port_id)
{
	static const char *mod_str[] = {
		NULL, "LR", "SR", "ER", "passive DA", "active DA", "LRM"
	};

	const struct net_device *dev = adap->port[port_id];
	const struct port_info *pi = netdev_priv(dev);

	if (pi->mod_type == FW_PORT_MOD_TYPE_NONE)
		netdev_info(dev, "port module unplugged\n");
	else if (pi->mod_type < ARRAY_SIZE(mod_str))
		netdev_info(dev, "%s module inserted\n", mod_str[pi->mod_type]);
<<<<<<< HEAD
}

/*
 * Configure the exact and hash address filters to handle a port's multicast
 * and secondary unicast MAC addresses.
 */
static int set_addr_filters(const struct net_device *dev, bool sleep)
{
	u64 mhash = 0;
	u64 uhash = 0;
	bool free = true;
	u16 filt_idx[7];
	const u8 *addr[7];
	int ret, naddr = 0;
	const struct netdev_hw_addr *ha;
	int uc_cnt = netdev_uc_count(dev);
	int mc_cnt = netdev_mc_count(dev);
	const struct port_info *pi = netdev_priv(dev);
	unsigned int mb = pi->adapter->fn;

	/* first do the secondary unicast addresses */
	netdev_for_each_uc_addr(ha, dev) {
		addr[naddr++] = ha->addr;
		if (--uc_cnt == 0 || naddr >= ARRAY_SIZE(addr)) {
			ret = t4_alloc_mac_filt(pi->adapter, mb, pi->viid, free,
					naddr, addr, filt_idx, &uhash, sleep);
			if (ret < 0)
				return ret;

			free = false;
			naddr = 0;
		}
	}

	/* next set up the multicast addresses */
	netdev_for_each_mc_addr(ha, dev) {
		addr[naddr++] = ha->addr;
		if (--mc_cnt == 0 || naddr >= ARRAY_SIZE(addr)) {
			ret = t4_alloc_mac_filt(pi->adapter, mb, pi->viid, free,
					naddr, addr, filt_idx, &mhash, sleep);
			if (ret < 0)
				return ret;

			free = false;
			naddr = 0;
		}
	}

	return t4_set_addr_hash(pi->adapter, mb, pi->viid, uhash != 0,
				uhash | mhash, sleep);
=======
	else if (pi->mod_type == FW_PORT_MOD_TYPE_NOTSUPPORTED)
		netdev_info(dev, "%s: unsupported port module inserted\n",
			    dev->name);
	else if (pi->mod_type == FW_PORT_MOD_TYPE_UNKNOWN)
		netdev_info(dev, "%s: unknown port module inserted\n",
			    dev->name);
	else if (pi->mod_type == FW_PORT_MOD_TYPE_ERROR)
		netdev_info(dev, "%s: transceiver module error\n", dev->name);
	else
		netdev_info(dev, "%s: unknown module type %d inserted\n",
			    dev->name, pi->mod_type);
>>>>>>> v4.9.227
}

int dbfifo_int_thresh = 10; /* 10 == 640 entry threshold */
module_param(dbfifo_int_thresh, int, 0644);
MODULE_PARM_DESC(dbfifo_int_thresh, "doorbell fifo interrupt threshold");

/*
 * usecs to sleep while draining the dbfifo
 */
static int dbfifo_drain_delay = 1000;
module_param(dbfifo_drain_delay, int, 0644);
MODULE_PARM_DESC(dbfifo_drain_delay,
		 "usecs to sleep while draining the dbfifo");

<<<<<<< HEAD
=======
static inline int cxgb4_set_addr_hash(struct port_info *pi)
{
	struct adapter *adap = pi->adapter;
	u64 vec = 0;
	bool ucast = false;
	struct hash_mac_addr *entry;

	/* Calculate the hash vector for the updated list and program it */
	list_for_each_entry(entry, &adap->mac_hlist, list) {
		ucast |= is_unicast_ether_addr(entry->addr);
		vec |= (1ULL << hash_mac_addr(entry->addr));
	}
	return t4_set_addr_hash(adap, adap->mbox, pi->viid, ucast,
				vec, false);
}

static int cxgb4_mac_sync(struct net_device *netdev, const u8 *mac_addr)
{
	struct port_info *pi = netdev_priv(netdev);
	struct adapter *adap = pi->adapter;
	int ret;
	u64 mhash = 0;
	u64 uhash = 0;
	bool free = false;
	bool ucast = is_unicast_ether_addr(mac_addr);
	const u8 *maclist[1] = {mac_addr};
	struct hash_mac_addr *new_entry;

	ret = t4_alloc_mac_filt(adap, adap->mbox, pi->viid, free, 1, maclist,
				NULL, ucast ? &uhash : &mhash, false);
	if (ret < 0)
		goto out;
	/* if hash != 0, then add the addr to hash addr list
	 * so on the end we will calculate the hash for the
	 * list and program it
	 */
	if (uhash || mhash) {
		new_entry = kzalloc(sizeof(*new_entry), GFP_ATOMIC);
		if (!new_entry)
			return -ENOMEM;
		ether_addr_copy(new_entry->addr, mac_addr);
		list_add_tail(&new_entry->list, &adap->mac_hlist);
		ret = cxgb4_set_addr_hash(pi);
	}
out:
	return ret < 0 ? ret : 0;
}

static int cxgb4_mac_unsync(struct net_device *netdev, const u8 *mac_addr)
{
	struct port_info *pi = netdev_priv(netdev);
	struct adapter *adap = pi->adapter;
	int ret;
	const u8 *maclist[1] = {mac_addr};
	struct hash_mac_addr *entry, *tmp;

	/* If the MAC address to be removed is in the hash addr
	 * list, delete it from the list and update hash vector
	 */
	list_for_each_entry_safe(entry, tmp, &adap->mac_hlist, list) {
		if (ether_addr_equal(entry->addr, mac_addr)) {
			list_del(&entry->list);
			kfree(entry);
			return cxgb4_set_addr_hash(pi);
		}
	}

	ret = t4_free_mac_filt(adap, adap->mbox, pi->viid, 1, maclist, false);
	return ret < 0 ? -EINVAL : 0;
}

>>>>>>> v4.9.227
/*
 * Set Rx properties of a port, such as promiscruity, address filters, and MTU.
 * If @mtu is -1 it is left unchanged.
 */
static int set_rxmode(struct net_device *dev, int mtu, bool sleep_ok)
{
<<<<<<< HEAD
	int ret;
	struct port_info *pi = netdev_priv(dev);

	ret = set_addr_filters(dev, sleep_ok);
	if (ret == 0)
		ret = t4_set_rxmode(pi->adapter, pi->adapter->fn, pi->viid, mtu,
				    (dev->flags & IFF_PROMISC) ? 1 : 0,
				    (dev->flags & IFF_ALLMULTI) ? 1 : 0, 1, -1,
				    sleep_ok);
	return ret;
=======
	struct port_info *pi = netdev_priv(dev);
	struct adapter *adapter = pi->adapter;

	__dev_uc_sync(dev, cxgb4_mac_sync, cxgb4_mac_unsync);
	__dev_mc_sync(dev, cxgb4_mac_sync, cxgb4_mac_unsync);

	return t4_set_rxmode(adapter, adapter->mbox, pi->viid, mtu,
			     (dev->flags & IFF_PROMISC) ? 1 : 0,
			     (dev->flags & IFF_ALLMULTI) ? 1 : 0, 1, -1,
			     sleep_ok);
>>>>>>> v4.9.227
}

/**
 *	link_start - enable a port
 *	@dev: the port to enable
 *
 *	Performs the MAC and PHY actions needed to enable a port.
 */
static int link_start(struct net_device *dev)
{
	int ret;
	struct port_info *pi = netdev_priv(dev);
<<<<<<< HEAD
	unsigned int mb = pi->adapter->fn;
=======
	unsigned int mb = pi->adapter->pf;
>>>>>>> v4.9.227

	/*
	 * We do not set address filters and promiscuity here, the stack does
	 * that step explicitly.
	 */
	ret = t4_set_rxmode(pi->adapter, mb, pi->viid, dev->mtu, -1, -1, -1,
			    !!(dev->features & NETIF_F_HW_VLAN_CTAG_RX), true);
	if (ret == 0) {
		ret = t4_change_mac(pi->adapter, mb, pi->viid,
				    pi->xact_addr_filt, dev->dev_addr, true,
				    true);
		if (ret >= 0) {
			pi->xact_addr_filt = ret;
			ret = 0;
		}
	}
	if (ret == 0)
<<<<<<< HEAD
		ret = t4_link_start(pi->adapter, mb, pi->tx_chan,
=======
		ret = t4_link_l1cfg(pi->adapter, mb, pi->tx_chan,
>>>>>>> v4.9.227
				    &pi->link_cfg);
	if (ret == 0) {
		local_bh_disable();
		ret = t4_enable_vi_params(pi->adapter, mb, pi->viid, true,
					  true, CXGB4_DCB_ENABLED);
		local_bh_enable();
	}

	return ret;
}

<<<<<<< HEAD
int cxgb4_dcb_enabled(const struct net_device *dev)
{
#ifdef CONFIG_CHELSIO_T4_DCB
	struct port_info *pi = netdev_priv(dev);

	if (!pi->dcb.enabled)
		return 0;

	return ((pi->dcb.state == CXGB4_DCB_STATE_FW_ALLSYNCED) ||
		(pi->dcb.state == CXGB4_DCB_STATE_HOST));
#else
	return 0;
#endif
}
EXPORT_SYMBOL(cxgb4_dcb_enabled);

=======
>>>>>>> v4.9.227
#ifdef CONFIG_CHELSIO_T4_DCB
/* Handle a Data Center Bridging update message from the firmware. */
static void dcb_rpl(struct adapter *adap, const struct fw_port_cmd *pcmd)
{
<<<<<<< HEAD
	int port = FW_PORT_CMD_PORTID_GET(ntohl(pcmd->op_to_portid));
	struct net_device *dev = adap->port[port];
=======
	int port = FW_PORT_CMD_PORTID_G(ntohl(pcmd->op_to_portid));
	struct net_device *dev = adap->port[adap->chan_map[port]];
>>>>>>> v4.9.227
	int old_dcb_enabled = cxgb4_dcb_enabled(dev);
	int new_dcb_enabled;

	cxgb4_dcb_handle_fw_update(adap, pcmd);
	new_dcb_enabled = cxgb4_dcb_enabled(dev);

	/* If the DCB has become enabled or disabled on the port then we're
	 * going to need to set up/tear down DCB Priority parameters for the
	 * TX Queues associated with the port.
	 */
	if (new_dcb_enabled != old_dcb_enabled)
		dcb_tx_queue_prio_enable(dev, new_dcb_enabled);
}
#endif /* CONFIG_CHELSIO_T4_DCB */

<<<<<<< HEAD
/* Clear a filter and release any of its resources that we own.  This also
 * clears the filter's "pending" status.
 */
static void clear_filter(struct adapter *adap, struct filter_entry *f)
{
	/* If the new or old filter have loopback rewriteing rules then we'll
	 * need to free any existing Layer Two Table (L2T) entries of the old
	 * filter rule.  The firmware will handle freeing up any Source MAC
	 * Table (SMT) entries used for rewriting Source MAC Addresses in
	 * loopback rules.
	 */
	if (f->l2t)
		cxgb4_l2t_release(f->l2t);

	/* The zeroing of the filter rule below clears the filter valid,
	 * pending, locked flags, l2t pointer, etc. so it's all we need for
	 * this operation.
	 */
	memset(f, 0, sizeof(*f));
}

/* Handle a filter write/deletion reply.
 */
static void filter_rpl(struct adapter *adap, const struct cpl_set_tcb_rpl *rpl)
{
	unsigned int idx = GET_TID(rpl);
	unsigned int nidx = idx - adap->tids.ftid_base;
	unsigned int ret;
	struct filter_entry *f;

	if (idx >= adap->tids.ftid_base && nidx <
	   (adap->tids.nftids + adap->tids.nsftids)) {
		idx = nidx;
		ret = GET_TCB_COOKIE(rpl->cookie);
		f = &adap->tids.ftid_tab[idx];

		if (ret == FW_FILTER_WR_FLT_DELETED) {
			/* Clear the filter when we get confirmation from the
			 * hardware that the filter has been deleted.
			 */
			clear_filter(adap, f);
		} else if (ret == FW_FILTER_WR_SMT_TBL_FULL) {
			dev_err(adap->pdev_dev, "filter %u setup failed due to full SMT\n",
				idx);
			clear_filter(adap, f);
		} else if (ret == FW_FILTER_WR_FLT_ADDED) {
			f->smtidx = (be64_to_cpu(rpl->oldval) >> 24) & 0xff;
			f->pending = 0;  /* asynchronous setup completed */
			f->valid = 1;
		} else {
			/* Something went wrong.  Issue a warning about the
			 * problem and clear everything out.
			 */
			dev_err(adap->pdev_dev, "filter %u setup failed with error %u\n",
				idx, ret);
			clear_filter(adap, f);
		}
	}
}

=======
>>>>>>> v4.9.227
/* Response queue handler for the FW event queue.
 */
static int fwevtq_handler(struct sge_rspq *q, const __be64 *rsp,
			  const struct pkt_gl *gl)
{
	u8 opcode = ((const struct rss_header *)rsp)->opcode;

	rsp++;                                          /* skip RSS header */

	/* FW can send EGR_UPDATEs encapsulated in a CPL_FW4_MSG.
	 */
	if (unlikely(opcode == CPL_FW4_MSG &&
	   ((const struct cpl_fw4_msg *)rsp)->type == FW_TYPE_RSSCPL)) {
		rsp++;
		opcode = ((const struct rss_header *)rsp)->opcode;
		rsp++;
		if (opcode != CPL_SGE_EGR_UPDATE) {
			dev_err(q->adap->pdev_dev, "unexpected FW4/CPL %#x on FW event queue\n"
				, opcode);
			goto out;
		}
	}

	if (likely(opcode == CPL_SGE_EGR_UPDATE)) {
		const struct cpl_sge_egr_update *p = (void *)rsp;
<<<<<<< HEAD
		unsigned int qid = EGR_QID(ntohl(p->opcode_qid));
=======
		unsigned int qid = EGR_QID_G(ntohl(p->opcode_qid));
>>>>>>> v4.9.227
		struct sge_txq *txq;

		txq = q->adap->sge.egr_map[qid - q->adap->sge.egr_start];
		txq->restarts++;
		if ((u8 *)txq < (u8 *)q->adap->sge.ofldtxq) {
			struct sge_eth_txq *eq;

			eq = container_of(txq, struct sge_eth_txq, q);
			netif_tx_wake_queue(eq->txq);
		} else {
			struct sge_ofld_txq *oq;

			oq = container_of(txq, struct sge_ofld_txq, q);
			tasklet_schedule(&oq->qresume_tsk);
		}
	} else if (opcode == CPL_FW6_MSG || opcode == CPL_FW4_MSG) {
		const struct cpl_fw6_msg *p = (void *)rsp;

#ifdef CONFIG_CHELSIO_T4_DCB
		const struct fw_port_cmd *pcmd = (const void *)p->data;
<<<<<<< HEAD
		unsigned int cmd = FW_CMD_OP_GET(ntohl(pcmd->op_to_portid));
		unsigned int action =
			FW_PORT_CMD_ACTION_GET(ntohl(pcmd->action_to_len16));

		if (cmd == FW_PORT_CMD &&
		    action == FW_PORT_ACTION_GET_PORT_INFO) {
			int port = FW_PORT_CMD_PORTID_GET(
					be32_to_cpu(pcmd->op_to_portid));
			struct net_device *dev = q->adap->port[port];
			int state_input = ((pcmd->u.info.dcbxdis_pkd &
					    FW_PORT_CMD_DCBXDIS)
=======
		unsigned int cmd = FW_CMD_OP_G(ntohl(pcmd->op_to_portid));
		unsigned int action =
			FW_PORT_CMD_ACTION_G(ntohl(pcmd->action_to_len16));

		if (cmd == FW_PORT_CMD &&
		    action == FW_PORT_ACTION_GET_PORT_INFO) {
			int port = FW_PORT_CMD_PORTID_G(
					be32_to_cpu(pcmd->op_to_portid));
			struct net_device *dev =
				q->adap->port[q->adap->chan_map[port]];
			int state_input = ((pcmd->u.info.dcbxdis_pkd &
					    FW_PORT_CMD_DCBXDIS_F)
>>>>>>> v4.9.227
					   ? CXGB4_DCB_INPUT_FW_DISABLED
					   : CXGB4_DCB_INPUT_FW_ENABLED);

			cxgb4_dcb_state_fsm(dev, state_input);
		}

		if (cmd == FW_PORT_CMD &&
		    action == FW_PORT_ACTION_L2_DCB_CFG)
			dcb_rpl(q->adap, pcmd);
		else
#endif
			if (p->type == 0)
				t4_handle_fw_rpl(q->adap, p->data);
	} else if (opcode == CPL_L2T_WRITE_RPL) {
		const struct cpl_l2t_write_rpl *p = (void *)rsp;

		do_l2t_write_rpl(q->adap, p);
	} else if (opcode == CPL_SET_TCB_RPL) {
		const struct cpl_set_tcb_rpl *p = (void *)rsp;

		filter_rpl(q->adap, p);
	} else
		dev_err(q->adap->pdev_dev,
			"unexpected CPL %#x on FW event queue\n", opcode);
out:
	return 0;
}

<<<<<<< HEAD
/**
 *	uldrx_handler - response queue handler for ULD queues
 *	@q: the response queue that received the packet
 *	@rsp: the response queue descriptor holding the offload message
 *	@gl: the gather list of packet fragments
 *
 *	Deliver an ingress offload packet to a ULD.  All processing is done by
 *	the ULD, we just maintain statistics.
 */
static int uldrx_handler(struct sge_rspq *q, const __be64 *rsp,
			 const struct pkt_gl *gl)
{
	struct sge_ofld_rxq *rxq = container_of(q, struct sge_ofld_rxq, rspq);

	/* FW can send CPLs encapsulated in a CPL_FW4_MSG.
	 */
	if (((const struct rss_header *)rsp)->opcode == CPL_FW4_MSG &&
	    ((const struct cpl_fw4_msg *)(rsp + 1))->type == FW_TYPE_RSSCPL)
		rsp += 2;

	if (ulds[q->uld].rx_handler(q->adap->uld_handle[q->uld], rsp, gl)) {
		rxq->stats.nomem++;
		return -1;
	}
	if (gl == NULL)
		rxq->stats.imm++;
	else if (gl == CXGB4_MSG_AN)
		rxq->stats.an++;
	else
		rxq->stats.pkts++;
	return 0;
}

=======
>>>>>>> v4.9.227
static void disable_msi(struct adapter *adapter)
{
	if (adapter->flags & USING_MSIX) {
		pci_disable_msix(adapter->pdev);
		adapter->flags &= ~USING_MSIX;
	} else if (adapter->flags & USING_MSI) {
		pci_disable_msi(adapter->pdev);
		adapter->flags &= ~USING_MSI;
	}
}

/*
 * Interrupt handler for non-data events used with MSI-X.
 */
static irqreturn_t t4_nondata_intr(int irq, void *cookie)
{
	struct adapter *adap = cookie;
<<<<<<< HEAD

	u32 v = t4_read_reg(adap, MYPF_REG(PL_PF_INT_CAUSE));
	if (v & PFSW) {
		adap->swintr = 1;
		t4_write_reg(adap, MYPF_REG(PL_PF_INT_CAUSE), v);
	}
	t4_slow_intr_handler(adap);
=======
	u32 v = t4_read_reg(adap, MYPF_REG(PL_PF_INT_CAUSE_A));

	if (v & PFSW_F) {
		adap->swintr = 1;
		t4_write_reg(adap, MYPF_REG(PL_PF_INT_CAUSE_A), v);
	}
	if (adap->flags & MASTER_PF)
		t4_slow_intr_handler(adap);
>>>>>>> v4.9.227
	return IRQ_HANDLED;
}

/*
 * Name the MSI-X interrupts.
 */
static void name_msix_vecs(struct adapter *adap)
{
	int i, j, msi_idx = 2, n = sizeof(adap->msix_info[0].desc);

	/* non-data interrupts */
	snprintf(adap->msix_info[0].desc, n, "%s", adap->port[0]->name);

	/* FW events */
	snprintf(adap->msix_info[1].desc, n, "%s-FWeventq",
		 adap->port[0]->name);

	/* Ethernet queues */
	for_each_port(adap, j) {
		struct net_device *d = adap->port[j];
		const struct port_info *pi = netdev_priv(d);

		for (i = 0; i < pi->nqsets; i++, msi_idx++)
			snprintf(adap->msix_info[msi_idx].desc, n, "%s-Rx%d",
				 d->name, i);
	}
<<<<<<< HEAD

	/* offload queues */
	for_each_ofldrxq(&adap->sge, i)
		snprintf(adap->msix_info[msi_idx++].desc, n, "%s-ofld%d",
			 adap->port[0]->name, i);

	for_each_rdmarxq(&adap->sge, i)
		snprintf(adap->msix_info[msi_idx++].desc, n, "%s-rdma%d",
			 adap->port[0]->name, i);

	for_each_rdmaciq(&adap->sge, i)
		snprintf(adap->msix_info[msi_idx++].desc, n, "%s-rdma-ciq%d",
			 adap->port[0]->name, i);
=======
>>>>>>> v4.9.227
}

static int request_msix_queue_irqs(struct adapter *adap)
{
	struct sge *s = &adap->sge;
<<<<<<< HEAD
	int err, ethqidx, ofldqidx = 0, rdmaqidx = 0, rdmaciqqidx = 0;
=======
	int err, ethqidx;
>>>>>>> v4.9.227
	int msi_index = 2;

	err = request_irq(adap->msix_info[1].vec, t4_sge_intr_msix, 0,
			  adap->msix_info[1].desc, &s->fw_evtq);
	if (err)
		return err;

	for_each_ethrxq(s, ethqidx) {
		err = request_irq(adap->msix_info[msi_index].vec,
				  t4_sge_intr_msix, 0,
				  adap->msix_info[msi_index].desc,
				  &s->ethrxq[ethqidx].rspq);
		if (err)
			goto unwind;
		msi_index++;
	}
<<<<<<< HEAD
	for_each_ofldrxq(s, ofldqidx) {
		err = request_irq(adap->msix_info[msi_index].vec,
				  t4_sge_intr_msix, 0,
				  adap->msix_info[msi_index].desc,
				  &s->ofldrxq[ofldqidx].rspq);
		if (err)
			goto unwind;
		msi_index++;
	}
	for_each_rdmarxq(s, rdmaqidx) {
		err = request_irq(adap->msix_info[msi_index].vec,
				  t4_sge_intr_msix, 0,
				  adap->msix_info[msi_index].desc,
				  &s->rdmarxq[rdmaqidx].rspq);
		if (err)
			goto unwind;
		msi_index++;
	}
	for_each_rdmaciq(s, rdmaciqqidx) {
		err = request_irq(adap->msix_info[msi_index].vec,
				  t4_sge_intr_msix, 0,
				  adap->msix_info[msi_index].desc,
				  &s->rdmaciq[rdmaciqqidx].rspq);
		if (err)
			goto unwind;
		msi_index++;
	}
	return 0;

unwind:
	while (--rdmaciqqidx >= 0)
		free_irq(adap->msix_info[--msi_index].vec,
			 &s->rdmaciq[rdmaciqqidx].rspq);
	while (--rdmaqidx >= 0)
		free_irq(adap->msix_info[--msi_index].vec,
			 &s->rdmarxq[rdmaqidx].rspq);
	while (--ofldqidx >= 0)
		free_irq(adap->msix_info[--msi_index].vec,
			 &s->ofldrxq[ofldqidx].rspq);
=======
	return 0;

unwind:
>>>>>>> v4.9.227
	while (--ethqidx >= 0)
		free_irq(adap->msix_info[--msi_index].vec,
			 &s->ethrxq[ethqidx].rspq);
	free_irq(adap->msix_info[1].vec, &s->fw_evtq);
	return err;
}

static void free_msix_queue_irqs(struct adapter *adap)
{
	int i, msi_index = 2;
	struct sge *s = &adap->sge;

	free_irq(adap->msix_info[1].vec, &s->fw_evtq);
	for_each_ethrxq(s, i)
		free_irq(adap->msix_info[msi_index++].vec, &s->ethrxq[i].rspq);
<<<<<<< HEAD
	for_each_ofldrxq(s, i)
		free_irq(adap->msix_info[msi_index++].vec, &s->ofldrxq[i].rspq);
	for_each_rdmarxq(s, i)
		free_irq(adap->msix_info[msi_index++].vec, &s->rdmarxq[i].rspq);
	for_each_rdmaciq(s, i)
		free_irq(adap->msix_info[msi_index++].vec, &s->rdmaciq[i].rspq);
}

/**
 *	write_rss - write the RSS table for a given port
=======
}

/**
 *	cxgb4_write_rss - write the RSS table for a given port
>>>>>>> v4.9.227
 *	@pi: the port
 *	@queues: array of queue indices for RSS
 *
 *	Sets up the portion of the HW RSS table for the port's VI to distribute
 *	packets to the Rx queues in @queues.
<<<<<<< HEAD
 */
static int write_rss(const struct port_info *pi, const u16 *queues)
{
	u16 *rss;
	int i, err;
	const struct sge_eth_rxq *q = &pi->adapter->sge.ethrxq[pi->first_qset];

=======
 *	Should never be called before setting up sge eth rx queues
 */
int cxgb4_write_rss(const struct port_info *pi, const u16 *queues)
{
	u16 *rss;
	int i, err;
	struct adapter *adapter = pi->adapter;
	const struct sge_eth_rxq *rxq;

	rxq = &adapter->sge.ethrxq[pi->first_qset];
>>>>>>> v4.9.227
	rss = kmalloc(pi->rss_size * sizeof(u16), GFP_KERNEL);
	if (!rss)
		return -ENOMEM;

	/* map the queue indices to queue ids */
	for (i = 0; i < pi->rss_size; i++, queues++)
<<<<<<< HEAD
		rss[i] = q[*queues].rspq.abs_id;

	err = t4_config_rss_range(pi->adapter, pi->adapter->fn, pi->viid, 0,
				  pi->rss_size, rss, pi->rss_size);
=======
		rss[i] = rxq[*queues].rspq.abs_id;

	err = t4_config_rss_range(adapter, adapter->pf, pi->viid, 0,
				  pi->rss_size, rss, pi->rss_size);
	/* If Tunnel All Lookup isn't specified in the global RSS
	 * Configuration, then we need to specify a default Ingress
	 * Queue for any ingress packets which aren't hashed.  We'll
	 * use our first ingress queue ...
	 */
	if (!err)
		err = t4_config_vi_rss(adapter, adapter->mbox, pi->viid,
				       FW_RSS_VI_CONFIG_CMD_IP6FOURTUPEN_F |
				       FW_RSS_VI_CONFIG_CMD_IP6TWOTUPEN_F |
				       FW_RSS_VI_CONFIG_CMD_IP4FOURTUPEN_F |
				       FW_RSS_VI_CONFIG_CMD_IP4TWOTUPEN_F |
				       FW_RSS_VI_CONFIG_CMD_UDPEN_F,
				       rss[0]);
>>>>>>> v4.9.227
	kfree(rss);
	return err;
}

/**
 *	setup_rss - configure RSS
 *	@adap: the adapter
 *
 *	Sets up RSS for each port.
 */
static int setup_rss(struct adapter *adap)
{
<<<<<<< HEAD
	int i, err;
=======
	int i, j, err;
>>>>>>> v4.9.227

	for_each_port(adap, i) {
		const struct port_info *pi = adap2pinfo(adap, i);

<<<<<<< HEAD
		err = write_rss(pi, pi->rss);
=======
		/* Fill default values with equal distribution */
		for (j = 0; j < pi->rss_size; j++)
			pi->rss[j] = j % pi->nqsets;

		err = cxgb4_write_rss(pi, pi->rss);
>>>>>>> v4.9.227
		if (err)
			return err;
	}
	return 0;
}

/*
 * Return the channel of the ingress queue with the given qid.
 */
static unsigned int rxq_to_chan(const struct sge *p, unsigned int qid)
{
	qid -= p->ingr_start;
	return netdev2pinfo(p->ingr_map[qid]->netdev)->tx_chan;
}

/*
 * Wait until all NAPI handlers are descheduled.
 */
static void quiesce_rx(struct adapter *adap)
{
	int i;

<<<<<<< HEAD
	for (i = 0; i < ARRAY_SIZE(adap->sge.ingr_map); i++) {
		struct sge_rspq *q = adap->sge.ingr_map[i];

		if (q && q->handler)
			napi_disable(&q->napi);
=======
	for (i = 0; i < adap->sge.ingr_sz; i++) {
		struct sge_rspq *q = adap->sge.ingr_map[i];

		if (q && q->handler) {
			napi_disable(&q->napi);
			local_bh_disable();
			while (!cxgb_poll_lock_napi(q))
				mdelay(1);
			local_bh_enable();
		}

	}
}

/* Disable interrupt and napi handler */
static void disable_interrupts(struct adapter *adap)
{
	if (adap->flags & FULL_INIT_DONE) {
		t4_intr_disable(adap);
		if (adap->flags & USING_MSIX) {
			free_msix_queue_irqs(adap);
			free_irq(adap->msix_info[0].vec, adap);
		} else {
			free_irq(adap->pdev->irq, adap);
		}
		quiesce_rx(adap);
>>>>>>> v4.9.227
	}
}

/*
 * Enable NAPI scheduling and interrupt generation for all Rx queues.
 */
static void enable_rx(struct adapter *adap)
{
	int i;

<<<<<<< HEAD
	for (i = 0; i < ARRAY_SIZE(adap->sge.ingr_map); i++) {
=======
	for (i = 0; i < adap->sge.ingr_sz; i++) {
>>>>>>> v4.9.227
		struct sge_rspq *q = adap->sge.ingr_map[i];

		if (!q)
			continue;
<<<<<<< HEAD
		if (q->handler)
			napi_enable(&q->napi);
		/* 0-increment GTS to start the timer and enable interrupts */
		t4_write_reg(adap, MYPF_REG(SGE_PF_GTS),
			     SEINTARM(q->intr_params) |
			     INGRESSQID(q->cntxt_id));
	}
}

=======
		if (q->handler) {
			cxgb_busy_poll_init_lock(q);
			napi_enable(&q->napi);
		}
		/* 0-increment GTS to start the timer and enable interrupts */
		t4_write_reg(adap, MYPF_REG(SGE_PF_GTS_A),
			     SEINTARM_V(q->intr_params) |
			     INGRESSQID_V(q->cntxt_id));
	}
}


static int setup_fw_sge_queues(struct adapter *adap)
{
	struct sge *s = &adap->sge;
	int err = 0;

	bitmap_zero(s->starving_fl, s->egr_sz);
	bitmap_zero(s->txq_maperr, s->egr_sz);

	if (adap->flags & USING_MSIX)
		adap->msi_idx = 1;         /* vector 0 is for non-queue interrupts */
	else {
		err = t4_sge_alloc_rxq(adap, &s->intrq, false, adap->port[0], 0,
				       NULL, NULL, NULL, -1);
		if (err)
			return err;
		adap->msi_idx = -((int)s->intrq.abs_id + 1);
	}

	err = t4_sge_alloc_rxq(adap, &s->fw_evtq, true, adap->port[0],
			       adap->msi_idx, NULL, fwevtq_handler, NULL, -1);
	return err;
}

>>>>>>> v4.9.227
/**
 *	setup_sge_queues - configure SGE Tx/Rx/response queues
 *	@adap: the adapter
 *
 *	Determines how many sets of SGE queues to use and initializes them.
 *	We support multiple queue sets per port if we have MSI-X, otherwise
 *	just one queue set per port.
 */
static int setup_sge_queues(struct adapter *adap)
{
<<<<<<< HEAD
	int err, msi_idx, i, j;
	struct sge *s = &adap->sge;

	bitmap_zero(s->starving_fl, MAX_EGRQ);
	bitmap_zero(s->txq_maperr, MAX_EGRQ);

	if (adap->flags & USING_MSIX)
		msi_idx = 1;         /* vector 0 is for non-queue interrupts */
	else {
		err = t4_sge_alloc_rxq(adap, &s->intrq, false, adap->port[0], 0,
				       NULL, NULL);
		if (err)
			return err;
		msi_idx = -((int)s->intrq.abs_id + 1);
	}

	err = t4_sge_alloc_rxq(adap, &s->fw_evtq, true, adap->port[0],
			       msi_idx, NULL, fwevtq_handler);
	if (err) {
freeout:	t4_free_sge_resources(adap);
		return err;
	}
=======
	int err, i, j;
	struct sge *s = &adap->sge;
	struct sge_uld_rxq_info *rxq_info = s->uld_rxq_info[CXGB4_ULD_RDMA];
	unsigned int cmplqid = 0;
>>>>>>> v4.9.227

	for_each_port(adap, i) {
		struct net_device *dev = adap->port[i];
		struct port_info *pi = netdev_priv(dev);
		struct sge_eth_rxq *q = &s->ethrxq[pi->first_qset];
		struct sge_eth_txq *t = &s->ethtxq[pi->first_qset];

		for (j = 0; j < pi->nqsets; j++, q++) {
<<<<<<< HEAD
			if (msi_idx > 0)
				msi_idx++;
			err = t4_sge_alloc_rxq(adap, &q->rspq, false, dev,
					       msi_idx, &q->fl,
					       t4_ethrx_handler);
=======
			if (adap->msi_idx > 0)
				adap->msi_idx++;
			err = t4_sge_alloc_rxq(adap, &q->rspq, false, dev,
					       adap->msi_idx, &q->fl,
					       t4_ethrx_handler,
					       NULL,
					       t4_get_mps_bg_map(adap,
								 pi->tx_chan));
>>>>>>> v4.9.227
			if (err)
				goto freeout;
			q->rspq.idx = j;
			memset(&q->stats, 0, sizeof(q->stats));
		}
		for (j = 0; j < pi->nqsets; j++, t++) {
			err = t4_sge_alloc_eth_txq(adap, t, dev,
					netdev_get_tx_queue(dev, j),
					s->fw_evtq.cntxt_id);
			if (err)
				goto freeout;
		}
	}

<<<<<<< HEAD
	j = s->ofldqsets / adap->params.nports; /* ofld queues per channel */
	for_each_ofldrxq(s, i) {
		struct sge_ofld_rxq *q = &s->ofldrxq[i];
		struct net_device *dev = adap->port[i / j];

		if (msi_idx > 0)
			msi_idx++;
		err = t4_sge_alloc_rxq(adap, &q->rspq, false, dev, msi_idx,
				       q->fl.size ? &q->fl : NULL,
				       uldrx_handler);
		if (err)
			goto freeout;
		memset(&q->stats, 0, sizeof(q->stats));
		s->ofld_rxq[i] = q->rspq.abs_id;
		err = t4_sge_alloc_ofld_txq(adap, &s->ofldtxq[i], dev,
=======
	j = s->ofldqsets / adap->params.nports; /* iscsi queues per channel */
	for_each_ofldtxq(s, i) {
		err = t4_sge_alloc_ofld_txq(adap, &s->ofldtxq[i],
					    adap->port[i / j],
>>>>>>> v4.9.227
					    s->fw_evtq.cntxt_id);
		if (err)
			goto freeout;
	}

<<<<<<< HEAD
	for_each_rdmarxq(s, i) {
		struct sge_ofld_rxq *q = &s->rdmarxq[i];

		if (msi_idx > 0)
			msi_idx++;
		err = t4_sge_alloc_rxq(adap, &q->rspq, false, adap->port[i],
				       msi_idx, q->fl.size ? &q->fl : NULL,
				       uldrx_handler);
		if (err)
			goto freeout;
		memset(&q->stats, 0, sizeof(q->stats));
		s->rdma_rxq[i] = q->rspq.abs_id;
	}

	for_each_rdmaciq(s, i) {
		struct sge_ofld_rxq *q = &s->rdmaciq[i];

		if (msi_idx > 0)
			msi_idx++;
		err = t4_sge_alloc_rxq(adap, &q->rspq, false, adap->port[i],
				       msi_idx, q->fl.size ? &q->fl : NULL,
				       uldrx_handler);
		if (err)
			goto freeout;
		memset(&q->stats, 0, sizeof(q->stats));
		s->rdma_ciq[i] = q->rspq.abs_id;
	}

	for_each_port(adap, i) {
		/*
		 * Note that ->rdmarxq[i].rspq.cntxt_id below is 0 if we don't
		 * have RDMA queues, and that's the right value.
		 */
		err = t4_sge_alloc_ctrl_txq(adap, &s->ctrlq[i], adap->port[i],
					    s->fw_evtq.cntxt_id,
					    s->rdmarxq[i].rspq.cntxt_id);
=======
	for_each_port(adap, i) {
		/* Note that cmplqid below is 0 if we don't
		 * have RDMA queues, and that's the right value.
		 */
		if (rxq_info)
			cmplqid	= rxq_info->uldrxq[i].rspq.cntxt_id;

		err = t4_sge_alloc_ctrl_txq(adap, &s->ctrlq[i], adap->port[i],
					    s->fw_evtq.cntxt_id, cmplqid);
>>>>>>> v4.9.227
		if (err)
			goto freeout;
	}

	t4_write_reg(adap, is_t4(adap->params.chip) ?
<<<<<<< HEAD
				MPS_TRC_RSS_CONTROL :
				MPS_T5_TRC_RSS_CONTROL,
		     RSSCONTROL(netdev2pinfo(adap->port[0])->tx_chan) |
		     QUEUENUMBER(s->ethrxq[0].rspq.abs_id));
	return 0;
=======
				MPS_TRC_RSS_CONTROL_A :
				MPS_T5_TRC_RSS_CONTROL_A,
		     RSSCONTROL_V(netdev2pinfo(adap->port[0])->tx_chan) |
		     QUEUENUMBER_V(s->ethrxq[0].rspq.abs_id));
	return 0;
freeout:
	t4_free_sge_resources(adap);
	return err;
>>>>>>> v4.9.227
}

/*
 * Allocate a chunk of memory using kmalloc or, if that fails, vmalloc.
 * The allocated memory is cleared.
 */
void *t4_alloc_mem(size_t size)
{
	void *p = kzalloc(size, GFP_KERNEL | __GFP_NOWARN);

	if (!p)
		p = vzalloc(size);
	return p;
}

/*
 * Free memory allocated through alloc_mem().
 */
<<<<<<< HEAD
static void t4_free_mem(void *addr)
{
	if (is_vmalloc_addr(addr))
		vfree(addr);
	else
		kfree(addr);
}

/* Send a Work Request to write the filter at a specified index.  We construct
 * a Firmware Filter Work Request to have the work done and put the indicated
 * filter into "pending" mode which will prevent any further actions against
 * it till we get a reply from the firmware on the completion status of the
 * request.
 */
static int set_filter_wr(struct adapter *adapter, int fidx)
{
	struct filter_entry *f = &adapter->tids.ftid_tab[fidx];
	struct sk_buff *skb;
	struct fw_filter_wr *fwr;
	unsigned int ftid;

	/* If the new filter requires loopback Destination MAC and/or VLAN
	 * rewriting then we need to allocate a Layer 2 Table (L2T) entry for
	 * the filter.
	 */
	if (f->fs.newdmac || f->fs.newvlan) {
		/* allocate L2T entry for new filter */
		f->l2t = t4_l2t_alloc_switching(adapter->l2t);
		if (f->l2t == NULL)
			return -EAGAIN;
		if (t4_l2t_set_switching(adapter, f->l2t, f->fs.vlan,
					f->fs.eport, f->fs.dmac)) {
			cxgb4_l2t_release(f->l2t);
			f->l2t = NULL;
			return -ENOMEM;
		}
	}

	ftid = adapter->tids.ftid_base + fidx;

	skb = alloc_skb(sizeof(*fwr), GFP_KERNEL | __GFP_NOFAIL);
	fwr = (struct fw_filter_wr *)__skb_put(skb, sizeof(*fwr));
	memset(fwr, 0, sizeof(*fwr));

	/* It would be nice to put most of the following in t4_hw.c but most
	 * of the work is translating the cxgbtool ch_filter_specification
	 * into the Work Request and the definition of that structure is
	 * currently in cxgbtool.h which isn't appropriate to pull into the
	 * common code.  We may eventually try to come up with a more neutral
	 * filter specification structure but for now it's easiest to simply
	 * put this fairly direct code in line ...
	 */
	fwr->op_pkd = htonl(FW_WR_OP(FW_FILTER_WR));
	fwr->len16_pkd = htonl(FW_WR_LEN16(sizeof(*fwr)/16));
	fwr->tid_to_iq =
		htonl(V_FW_FILTER_WR_TID(ftid) |
		      V_FW_FILTER_WR_RQTYPE(f->fs.type) |
		      V_FW_FILTER_WR_NOREPLY(0) |
		      V_FW_FILTER_WR_IQ(f->fs.iq));
	fwr->del_filter_to_l2tix =
		htonl(V_FW_FILTER_WR_RPTTID(f->fs.rpttid) |
		      V_FW_FILTER_WR_DROP(f->fs.action == FILTER_DROP) |
		      V_FW_FILTER_WR_DIRSTEER(f->fs.dirsteer) |
		      V_FW_FILTER_WR_MASKHASH(f->fs.maskhash) |
		      V_FW_FILTER_WR_DIRSTEERHASH(f->fs.dirsteerhash) |
		      V_FW_FILTER_WR_LPBK(f->fs.action == FILTER_SWITCH) |
		      V_FW_FILTER_WR_DMAC(f->fs.newdmac) |
		      V_FW_FILTER_WR_SMAC(f->fs.newsmac) |
		      V_FW_FILTER_WR_INSVLAN(f->fs.newvlan == VLAN_INSERT ||
					     f->fs.newvlan == VLAN_REWRITE) |
		      V_FW_FILTER_WR_RMVLAN(f->fs.newvlan == VLAN_REMOVE ||
					    f->fs.newvlan == VLAN_REWRITE) |
		      V_FW_FILTER_WR_HITCNTS(f->fs.hitcnts) |
		      V_FW_FILTER_WR_TXCHAN(f->fs.eport) |
		      V_FW_FILTER_WR_PRIO(f->fs.prio) |
		      V_FW_FILTER_WR_L2TIX(f->l2t ? f->l2t->idx : 0));
	fwr->ethtype = htons(f->fs.val.ethtype);
	fwr->ethtypem = htons(f->fs.mask.ethtype);
	fwr->frag_to_ovlan_vldm =
		(V_FW_FILTER_WR_FRAG(f->fs.val.frag) |
		 V_FW_FILTER_WR_FRAGM(f->fs.mask.frag) |
		 V_FW_FILTER_WR_IVLAN_VLD(f->fs.val.ivlan_vld) |
		 V_FW_FILTER_WR_OVLAN_VLD(f->fs.val.ovlan_vld) |
		 V_FW_FILTER_WR_IVLAN_VLDM(f->fs.mask.ivlan_vld) |
		 V_FW_FILTER_WR_OVLAN_VLDM(f->fs.mask.ovlan_vld));
	fwr->smac_sel = 0;
	fwr->rx_chan_rx_rpl_iq =
		htons(V_FW_FILTER_WR_RX_CHAN(0) |
		      V_FW_FILTER_WR_RX_RPL_IQ(adapter->sge.fw_evtq.abs_id));
	fwr->maci_to_matchtypem =
		htonl(V_FW_FILTER_WR_MACI(f->fs.val.macidx) |
		      V_FW_FILTER_WR_MACIM(f->fs.mask.macidx) |
		      V_FW_FILTER_WR_FCOE(f->fs.val.fcoe) |
		      V_FW_FILTER_WR_FCOEM(f->fs.mask.fcoe) |
		      V_FW_FILTER_WR_PORT(f->fs.val.iport) |
		      V_FW_FILTER_WR_PORTM(f->fs.mask.iport) |
		      V_FW_FILTER_WR_MATCHTYPE(f->fs.val.matchtype) |
		      V_FW_FILTER_WR_MATCHTYPEM(f->fs.mask.matchtype));
	fwr->ptcl = f->fs.val.proto;
	fwr->ptclm = f->fs.mask.proto;
	fwr->ttyp = f->fs.val.tos;
	fwr->ttypm = f->fs.mask.tos;
	fwr->ivlan = htons(f->fs.val.ivlan);
	fwr->ivlanm = htons(f->fs.mask.ivlan);
	fwr->ovlan = htons(f->fs.val.ovlan);
	fwr->ovlanm = htons(f->fs.mask.ovlan);
	memcpy(fwr->lip, f->fs.val.lip, sizeof(fwr->lip));
	memcpy(fwr->lipm, f->fs.mask.lip, sizeof(fwr->lipm));
	memcpy(fwr->fip, f->fs.val.fip, sizeof(fwr->fip));
	memcpy(fwr->fipm, f->fs.mask.fip, sizeof(fwr->fipm));
	fwr->lp = htons(f->fs.val.lport);
	fwr->lpm = htons(f->fs.mask.lport);
	fwr->fp = htons(f->fs.val.fport);
	fwr->fpm = htons(f->fs.mask.fport);
	if (f->fs.newsmac)
		memcpy(fwr->sma, f->fs.smac, sizeof(fwr->sma));

	/* Mark the filter as "pending" and ship off the Filter Work Request.
	 * When we get the Work Request Reply we'll clear the pending status.
	 */
	f->pending = 1;
	set_wr_txq(skb, CPL_PRIORITY_CONTROL, f->fs.val.iport & 0x3);
	t4_ofld_send(adapter, skb);
	return 0;
}

/* Delete the filter at a specified index.
 */
static int del_filter_wr(struct adapter *adapter, int fidx)
{
	struct filter_entry *f = &adapter->tids.ftid_tab[fidx];
	struct sk_buff *skb;
	struct fw_filter_wr *fwr;
	unsigned int len, ftid;

	len = sizeof(*fwr);
	ftid = adapter->tids.ftid_base + fidx;

	skb = alloc_skb(len, GFP_KERNEL | __GFP_NOFAIL);
	fwr = (struct fw_filter_wr *)__skb_put(skb, len);
	t4_mk_filtdelwr(ftid, fwr, adapter->sge.fw_evtq.abs_id);

	/* Mark the filter as "pending" and ship off the Filter Work Request.
	 * When we get the Work Request Reply we'll clear the pending status.
	 */
	f->pending = 1;
	t4_mgmt_tx(adapter, skb);
	return 0;
=======
void t4_free_mem(void *addr)
{
	kvfree(addr);
>>>>>>> v4.9.227
}

static u16 cxgb_select_queue(struct net_device *dev, struct sk_buff *skb,
			     void *accel_priv, select_queue_fallback_t fallback)
{
	int txq;

#ifdef CONFIG_CHELSIO_T4_DCB
	/* If a Data Center Bridging has been successfully negotiated on this
	 * link then we'll use the skb's priority to map it to a TX Queue.
	 * The skb's priority is determined via the VLAN Tag Priority Code
	 * Point field.
	 */
	if (cxgb4_dcb_enabled(dev)) {
		u16 vlan_tci;
		int err;

		err = vlan_get_tag(skb, &vlan_tci);
		if (unlikely(err)) {
			if (net_ratelimit())
				netdev_warn(dev,
					    "TX Packet without VLAN Tag on DCB Link\n");
			txq = 0;
		} else {
			txq = (vlan_tci & VLAN_PRIO_MASK) >> VLAN_PRIO_SHIFT;
<<<<<<< HEAD
=======
#ifdef CONFIG_CHELSIO_T4_FCOE
			if (skb->protocol == htons(ETH_P_FCOE))
				txq = skb->priority & 0x7;
#endif /* CONFIG_CHELSIO_T4_FCOE */
>>>>>>> v4.9.227
		}
		return txq;
	}
#endif /* CONFIG_CHELSIO_T4_DCB */

	if (select_queue) {
		txq = (skb_rx_queue_recorded(skb)
			? skb_get_rx_queue(skb)
			: smp_processor_id());

		while (unlikely(txq >= dev->real_num_tx_queues))
			txq -= dev->real_num_tx_queues;

		return txq;
	}

	return fallback(dev, skb) % dev->real_num_tx_queues;
}

<<<<<<< HEAD
static inline int is_offload(const struct adapter *adap)
{
	return adap->params.offload;
}

/*
 * Implementation of ethtool operations.
 */

static u32 get_msglevel(struct net_device *dev)
{
	return netdev2adap(dev)->msg_enable;
}

static void set_msglevel(struct net_device *dev, u32 val)
{
	netdev2adap(dev)->msg_enable = val;
}

static char stats_strings[][ETH_GSTRING_LEN] = {
	"TxOctetsOK         ",
	"TxFramesOK         ",
	"TxBroadcastFrames  ",
	"TxMulticastFrames  ",
	"TxUnicastFrames    ",
	"TxErrorFrames      ",

	"TxFrames64         ",
	"TxFrames65To127    ",
	"TxFrames128To255   ",
	"TxFrames256To511   ",
	"TxFrames512To1023  ",
	"TxFrames1024To1518 ",
	"TxFrames1519ToMax  ",

	"TxFramesDropped    ",
	"TxPauseFrames      ",
	"TxPPP0Frames       ",
	"TxPPP1Frames       ",
	"TxPPP2Frames       ",
	"TxPPP3Frames       ",
	"TxPPP4Frames       ",
	"TxPPP5Frames       ",
	"TxPPP6Frames       ",
	"TxPPP7Frames       ",

	"RxOctetsOK         ",
	"RxFramesOK         ",
	"RxBroadcastFrames  ",
	"RxMulticastFrames  ",
	"RxUnicastFrames    ",

	"RxFramesTooLong    ",
	"RxJabberErrors     ",
	"RxFCSErrors        ",
	"RxLengthErrors     ",
	"RxSymbolErrors     ",
	"RxRuntFrames       ",

	"RxFrames64         ",
	"RxFrames65To127    ",
	"RxFrames128To255   ",
	"RxFrames256To511   ",
	"RxFrames512To1023  ",
	"RxFrames1024To1518 ",
	"RxFrames1519ToMax  ",

	"RxPauseFrames      ",
	"RxPPP0Frames       ",
	"RxPPP1Frames       ",
	"RxPPP2Frames       ",
	"RxPPP3Frames       ",
	"RxPPP4Frames       ",
	"RxPPP5Frames       ",
	"RxPPP6Frames       ",
	"RxPPP7Frames       ",

	"RxBG0FramesDropped ",
	"RxBG1FramesDropped ",
	"RxBG2FramesDropped ",
	"RxBG3FramesDropped ",
	"RxBG0FramesTrunc   ",
	"RxBG1FramesTrunc   ",
	"RxBG2FramesTrunc   ",
	"RxBG3FramesTrunc   ",

	"TSO                ",
	"TxCsumOffload      ",
	"RxCsumGood         ",
	"VLANextractions    ",
	"VLANinsertions     ",
	"GROpackets         ",
	"GROmerged          ",
	"WriteCoalSuccess   ",
	"WriteCoalFail      ",
};

static int get_sset_count(struct net_device *dev, int sset)
{
	switch (sset) {
	case ETH_SS_STATS:
		return ARRAY_SIZE(stats_strings);
	default:
		return -EOPNOTSUPP;
	}
}

#define T4_REGMAP_SIZE (160 * 1024)
#define T5_REGMAP_SIZE (332 * 1024)

static int get_regs_len(struct net_device *dev)
{
	struct adapter *adap = netdev2adap(dev);
	if (is_t4(adap->params.chip))
		return T4_REGMAP_SIZE;
	else
		return T5_REGMAP_SIZE;
}

static int get_eeprom_len(struct net_device *dev)
{
	return EEPROMSIZE;
}

static void get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	struct adapter *adapter = netdev2adap(dev);

	strlcpy(info->driver, KBUILD_MODNAME, sizeof(info->driver));
	strlcpy(info->version, DRV_VERSION, sizeof(info->version));
	strlcpy(info->bus_info, pci_name(adapter->pdev),
		sizeof(info->bus_info));

	if (adapter->params.fw_vers)
		snprintf(info->fw_version, sizeof(info->fw_version),
			"%u.%u.%u.%u, TP %u.%u.%u.%u",
			FW_HDR_FW_VER_MAJOR_GET(adapter->params.fw_vers),
			FW_HDR_FW_VER_MINOR_GET(adapter->params.fw_vers),
			FW_HDR_FW_VER_MICRO_GET(adapter->params.fw_vers),
			FW_HDR_FW_VER_BUILD_GET(adapter->params.fw_vers),
			FW_HDR_FW_VER_MAJOR_GET(adapter->params.tp_vers),
			FW_HDR_FW_VER_MINOR_GET(adapter->params.tp_vers),
			FW_HDR_FW_VER_MICRO_GET(adapter->params.tp_vers),
			FW_HDR_FW_VER_BUILD_GET(adapter->params.tp_vers));
}

static void get_strings(struct net_device *dev, u32 stringset, u8 *data)
{
	if (stringset == ETH_SS_STATS)
		memcpy(data, stats_strings, sizeof(stats_strings));
}

/*
 * port stats maintained per queue of the port.  They should be in the same
 * order as in stats_strings above.
 */
struct queue_port_stats {
	u64 tso;
	u64 tx_csum;
	u64 rx_csum;
	u64 vlan_ex;
	u64 vlan_ins;
	u64 gro_pkts;
	u64 gro_merged;
};

static void collect_sge_port_stats(const struct adapter *adap,
		const struct port_info *p, struct queue_port_stats *s)
{
	int i;
	const struct sge_eth_txq *tx = &adap->sge.ethtxq[p->first_qset];
	const struct sge_eth_rxq *rx = &adap->sge.ethrxq[p->first_qset];

	memset(s, 0, sizeof(*s));
	for (i = 0; i < p->nqsets; i++, rx++, tx++) {
		s->tso += tx->tso;
		s->tx_csum += tx->tx_cso;
		s->rx_csum += rx->stats.rx_cso;
		s->vlan_ex += rx->stats.vlan_ex;
		s->vlan_ins += tx->vlan_ins;
		s->gro_pkts += rx->stats.lro_pkts;
		s->gro_merged += rx->stats.lro_merged;
	}
}

static void get_stats(struct net_device *dev, struct ethtool_stats *stats,
		      u64 *data)
{
	struct port_info *pi = netdev_priv(dev);
	struct adapter *adapter = pi->adapter;
	u32 val1, val2;

	t4_get_port_stats(adapter, pi->tx_chan, (struct port_stats *)data);

	data += sizeof(struct port_stats) / sizeof(u64);
	collect_sge_port_stats(adapter, pi, (struct queue_port_stats *)data);
	data += sizeof(struct queue_port_stats) / sizeof(u64);
	if (!is_t4(adapter->params.chip)) {
		t4_write_reg(adapter, SGE_STAT_CFG, STATSOURCE_T5(7));
		val1 = t4_read_reg(adapter, SGE_STAT_TOTAL);
		val2 = t4_read_reg(adapter, SGE_STAT_MATCH);
		*data = val1 - val2;
		data++;
		*data = val2;
		data++;
	} else {
		memset(data, 0, 2 * sizeof(u64));
		*data += 2;
	}
}

/*
 * Return a version number to identify the type of adapter.  The scheme is:
 * - bits 0..9: chip version
 * - bits 10..15: chip revision
 * - bits 16..23: register dump version
 */
static inline unsigned int mk_adap_vers(const struct adapter *ap)
{
	return CHELSIO_CHIP_VERSION(ap->params.chip) |
		(CHELSIO_CHIP_RELEASE(ap->params.chip) << 10) | (1 << 16);
}

static void reg_block_dump(struct adapter *ap, void *buf, unsigned int start,
			   unsigned int end)
{
	u32 *p = buf + start;

	for ( ; start <= end; start += sizeof(u32))
		*p++ = t4_read_reg(ap, start);
}

static void get_regs(struct net_device *dev, struct ethtool_regs *regs,
		     void *buf)
{
	static const unsigned int t4_reg_ranges[] = {
		0x1008, 0x1108,
		0x1180, 0x11b4,
		0x11fc, 0x123c,
		0x1300, 0x173c,
		0x1800, 0x18fc,
		0x3000, 0x30d8,
		0x30e0, 0x5924,
		0x5960, 0x59d4,
		0x5a00, 0x5af8,
		0x6000, 0x6098,
		0x6100, 0x6150,
		0x6200, 0x6208,
		0x6240, 0x6248,
		0x6280, 0x6338,
		0x6370, 0x638c,
		0x6400, 0x643c,
		0x6500, 0x6524,
		0x6a00, 0x6a38,
		0x6a60, 0x6a78,
		0x6b00, 0x6b84,
		0x6bf0, 0x6c84,
		0x6cf0, 0x6d84,
		0x6df0, 0x6e84,
		0x6ef0, 0x6f84,
		0x6ff0, 0x7084,
		0x70f0, 0x7184,
		0x71f0, 0x7284,
		0x72f0, 0x7384,
		0x73f0, 0x7450,
		0x7500, 0x7530,
		0x7600, 0x761c,
		0x7680, 0x76cc,
		0x7700, 0x7798,
		0x77c0, 0x77fc,
		0x7900, 0x79fc,
		0x7b00, 0x7c38,
		0x7d00, 0x7efc,
		0x8dc0, 0x8e1c,
		0x8e30, 0x8e78,
		0x8ea0, 0x8f6c,
		0x8fc0, 0x9074,
		0x90fc, 0x90fc,
		0x9400, 0x9458,
		0x9600, 0x96bc,
		0x9800, 0x9808,
		0x9820, 0x983c,
		0x9850, 0x9864,
		0x9c00, 0x9c6c,
		0x9c80, 0x9cec,
		0x9d00, 0x9d6c,
		0x9d80, 0x9dec,
		0x9e00, 0x9e6c,
		0x9e80, 0x9eec,
		0x9f00, 0x9f6c,
		0x9f80, 0x9fec,
		0xd004, 0xd03c,
		0xdfc0, 0xdfe0,
		0xe000, 0xea7c,
		0xf000, 0x11110,
		0x11118, 0x11190,
		0x19040, 0x1906c,
		0x19078, 0x19080,
		0x1908c, 0x19124,
		0x19150, 0x191b0,
		0x191d0, 0x191e8,
		0x19238, 0x1924c,
		0x193f8, 0x19474,
		0x19490, 0x194f8,
		0x19800, 0x19f30,
		0x1a000, 0x1a06c,
		0x1a0b0, 0x1a120,
		0x1a128, 0x1a138,
		0x1a190, 0x1a1c4,
		0x1a1fc, 0x1a1fc,
		0x1e040, 0x1e04c,
		0x1e284, 0x1e28c,
		0x1e2c0, 0x1e2c0,
		0x1e2e0, 0x1e2e0,
		0x1e300, 0x1e384,
		0x1e3c0, 0x1e3c8,
		0x1e440, 0x1e44c,
		0x1e684, 0x1e68c,
		0x1e6c0, 0x1e6c0,
		0x1e6e0, 0x1e6e0,
		0x1e700, 0x1e784,
		0x1e7c0, 0x1e7c8,
		0x1e840, 0x1e84c,
		0x1ea84, 0x1ea8c,
		0x1eac0, 0x1eac0,
		0x1eae0, 0x1eae0,
		0x1eb00, 0x1eb84,
		0x1ebc0, 0x1ebc8,
		0x1ec40, 0x1ec4c,
		0x1ee84, 0x1ee8c,
		0x1eec0, 0x1eec0,
		0x1eee0, 0x1eee0,
		0x1ef00, 0x1ef84,
		0x1efc0, 0x1efc8,
		0x1f040, 0x1f04c,
		0x1f284, 0x1f28c,
		0x1f2c0, 0x1f2c0,
		0x1f2e0, 0x1f2e0,
		0x1f300, 0x1f384,
		0x1f3c0, 0x1f3c8,
		0x1f440, 0x1f44c,
		0x1f684, 0x1f68c,
		0x1f6c0, 0x1f6c0,
		0x1f6e0, 0x1f6e0,
		0x1f700, 0x1f784,
		0x1f7c0, 0x1f7c8,
		0x1f840, 0x1f84c,
		0x1fa84, 0x1fa8c,
		0x1fac0, 0x1fac0,
		0x1fae0, 0x1fae0,
		0x1fb00, 0x1fb84,
		0x1fbc0, 0x1fbc8,
		0x1fc40, 0x1fc4c,
		0x1fe84, 0x1fe8c,
		0x1fec0, 0x1fec0,
		0x1fee0, 0x1fee0,
		0x1ff00, 0x1ff84,
		0x1ffc0, 0x1ffc8,
		0x20000, 0x2002c,
		0x20100, 0x2013c,
		0x20190, 0x201c8,
		0x20200, 0x20318,
		0x20400, 0x20528,
		0x20540, 0x20614,
		0x21000, 0x21040,
		0x2104c, 0x21060,
		0x210c0, 0x210ec,
		0x21200, 0x21268,
		0x21270, 0x21284,
		0x212fc, 0x21388,
		0x21400, 0x21404,
		0x21500, 0x21518,
		0x2152c, 0x2153c,
		0x21550, 0x21554,
		0x21600, 0x21600,
		0x21608, 0x21628,
		0x21630, 0x2163c,
		0x21700, 0x2171c,
		0x21780, 0x2178c,
		0x21800, 0x21c38,
		0x21c80, 0x21d7c,
		0x21e00, 0x21e04,
		0x22000, 0x2202c,
		0x22100, 0x2213c,
		0x22190, 0x221c8,
		0x22200, 0x22318,
		0x22400, 0x22528,
		0x22540, 0x22614,
		0x23000, 0x23040,
		0x2304c, 0x23060,
		0x230c0, 0x230ec,
		0x23200, 0x23268,
		0x23270, 0x23284,
		0x232fc, 0x23388,
		0x23400, 0x23404,
		0x23500, 0x23518,
		0x2352c, 0x2353c,
		0x23550, 0x23554,
		0x23600, 0x23600,
		0x23608, 0x23628,
		0x23630, 0x2363c,
		0x23700, 0x2371c,
		0x23780, 0x2378c,
		0x23800, 0x23c38,
		0x23c80, 0x23d7c,
		0x23e00, 0x23e04,
		0x24000, 0x2402c,
		0x24100, 0x2413c,
		0x24190, 0x241c8,
		0x24200, 0x24318,
		0x24400, 0x24528,
		0x24540, 0x24614,
		0x25000, 0x25040,
		0x2504c, 0x25060,
		0x250c0, 0x250ec,
		0x25200, 0x25268,
		0x25270, 0x25284,
		0x252fc, 0x25388,
		0x25400, 0x25404,
		0x25500, 0x25518,
		0x2552c, 0x2553c,
		0x25550, 0x25554,
		0x25600, 0x25600,
		0x25608, 0x25628,
		0x25630, 0x2563c,
		0x25700, 0x2571c,
		0x25780, 0x2578c,
		0x25800, 0x25c38,
		0x25c80, 0x25d7c,
		0x25e00, 0x25e04,
		0x26000, 0x2602c,
		0x26100, 0x2613c,
		0x26190, 0x261c8,
		0x26200, 0x26318,
		0x26400, 0x26528,
		0x26540, 0x26614,
		0x27000, 0x27040,
		0x2704c, 0x27060,
		0x270c0, 0x270ec,
		0x27200, 0x27268,
		0x27270, 0x27284,
		0x272fc, 0x27388,
		0x27400, 0x27404,
		0x27500, 0x27518,
		0x2752c, 0x2753c,
		0x27550, 0x27554,
		0x27600, 0x27600,
		0x27608, 0x27628,
		0x27630, 0x2763c,
		0x27700, 0x2771c,
		0x27780, 0x2778c,
		0x27800, 0x27c38,
		0x27c80, 0x27d7c,
		0x27e00, 0x27e04
	};

	static const unsigned int t5_reg_ranges[] = {
		0x1008, 0x1148,
		0x1180, 0x11b4,
		0x11fc, 0x123c,
		0x1280, 0x173c,
		0x1800, 0x18fc,
		0x3000, 0x3028,
		0x3060, 0x30d8,
		0x30e0, 0x30fc,
		0x3140, 0x357c,
		0x35a8, 0x35cc,
		0x35ec, 0x35ec,
		0x3600, 0x5624,
		0x56cc, 0x575c,
		0x580c, 0x5814,
		0x5890, 0x58bc,
		0x5940, 0x59dc,
		0x59fc, 0x5a18,
		0x5a60, 0x5a9c,
		0x5b9c, 0x5bfc,
		0x6000, 0x6040,
		0x6058, 0x614c,
		0x7700, 0x7798,
		0x77c0, 0x78fc,
		0x7b00, 0x7c54,
		0x7d00, 0x7efc,
		0x8dc0, 0x8de0,
		0x8df8, 0x8e84,
		0x8ea0, 0x8f84,
		0x8fc0, 0x90f8,
		0x9400, 0x9470,
		0x9600, 0x96f4,
		0x9800, 0x9808,
		0x9820, 0x983c,
		0x9850, 0x9864,
		0x9c00, 0x9c6c,
		0x9c80, 0x9cec,
		0x9d00, 0x9d6c,
		0x9d80, 0x9dec,
		0x9e00, 0x9e6c,
		0x9e80, 0x9eec,
		0x9f00, 0x9f6c,
		0x9f80, 0xa020,
		0xd004, 0xd03c,
		0xdfc0, 0xdfe0,
		0xe000, 0x11088,
		0x1109c, 0x11110,
		0x11118, 0x1117c,
		0x11190, 0x11204,
		0x19040, 0x1906c,
		0x19078, 0x19080,
		0x1908c, 0x19124,
		0x19150, 0x191b0,
		0x191d0, 0x191e8,
		0x19238, 0x19290,
		0x193f8, 0x19474,
		0x19490, 0x194cc,
		0x194f0, 0x194f8,
		0x19c00, 0x19c60,
		0x19c94, 0x19e10,
		0x19e50, 0x19f34,
		0x19f40, 0x19f50,
		0x19f90, 0x19fe4,
		0x1a000, 0x1a06c,
		0x1a0b0, 0x1a120,
		0x1a128, 0x1a138,
		0x1a190, 0x1a1c4,
		0x1a1fc, 0x1a1fc,
		0x1e008, 0x1e00c,
		0x1e040, 0x1e04c,
		0x1e284, 0x1e290,
		0x1e2c0, 0x1e2c0,
		0x1e2e0, 0x1e2e0,
		0x1e300, 0x1e384,
		0x1e3c0, 0x1e3c8,
		0x1e408, 0x1e40c,
		0x1e440, 0x1e44c,
		0x1e684, 0x1e690,
		0x1e6c0, 0x1e6c0,
		0x1e6e0, 0x1e6e0,
		0x1e700, 0x1e784,
		0x1e7c0, 0x1e7c8,
		0x1e808, 0x1e80c,
		0x1e840, 0x1e84c,
		0x1ea84, 0x1ea90,
		0x1eac0, 0x1eac0,
		0x1eae0, 0x1eae0,
		0x1eb00, 0x1eb84,
		0x1ebc0, 0x1ebc8,
		0x1ec08, 0x1ec0c,
		0x1ec40, 0x1ec4c,
		0x1ee84, 0x1ee90,
		0x1eec0, 0x1eec0,
		0x1eee0, 0x1eee0,
		0x1ef00, 0x1ef84,
		0x1efc0, 0x1efc8,
		0x1f008, 0x1f00c,
		0x1f040, 0x1f04c,
		0x1f284, 0x1f290,
		0x1f2c0, 0x1f2c0,
		0x1f2e0, 0x1f2e0,
		0x1f300, 0x1f384,
		0x1f3c0, 0x1f3c8,
		0x1f408, 0x1f40c,
		0x1f440, 0x1f44c,
		0x1f684, 0x1f690,
		0x1f6c0, 0x1f6c0,
		0x1f6e0, 0x1f6e0,
		0x1f700, 0x1f784,
		0x1f7c0, 0x1f7c8,
		0x1f808, 0x1f80c,
		0x1f840, 0x1f84c,
		0x1fa84, 0x1fa90,
		0x1fac0, 0x1fac0,
		0x1fae0, 0x1fae0,
		0x1fb00, 0x1fb84,
		0x1fbc0, 0x1fbc8,
		0x1fc08, 0x1fc0c,
		0x1fc40, 0x1fc4c,
		0x1fe84, 0x1fe90,
		0x1fec0, 0x1fec0,
		0x1fee0, 0x1fee0,
		0x1ff00, 0x1ff84,
		0x1ffc0, 0x1ffc8,
		0x30000, 0x30030,
		0x30100, 0x30144,
		0x30190, 0x301d0,
		0x30200, 0x30318,
		0x30400, 0x3052c,
		0x30540, 0x3061c,
		0x30800, 0x30834,
		0x308c0, 0x30908,
		0x30910, 0x309ac,
		0x30a00, 0x30a04,
		0x30a0c, 0x30a2c,
		0x30a44, 0x30a50,
		0x30a74, 0x30c24,
		0x30d08, 0x30d14,
		0x30d1c, 0x30d20,
		0x30d3c, 0x30d50,
		0x31200, 0x3120c,
		0x31220, 0x31220,
		0x31240, 0x31240,
		0x31600, 0x31600,
		0x31608, 0x3160c,
		0x31a00, 0x31a1c,
		0x31e04, 0x31e20,
		0x31e38, 0x31e3c,
		0x31e80, 0x31e80,
		0x31e88, 0x31ea8,
		0x31eb0, 0x31eb4,
		0x31ec8, 0x31ed4,
		0x31fb8, 0x32004,
		0x32208, 0x3223c,
		0x32600, 0x32630,
		0x32a00, 0x32abc,
		0x32b00, 0x32b70,
		0x33000, 0x33048,
		0x33060, 0x3309c,
		0x330f0, 0x33148,
		0x33160, 0x3319c,
		0x331f0, 0x332e4,
		0x332f8, 0x333e4,
		0x333f8, 0x33448,
		0x33460, 0x3349c,
		0x334f0, 0x33548,
		0x33560, 0x3359c,
		0x335f0, 0x336e4,
		0x336f8, 0x337e4,
		0x337f8, 0x337fc,
		0x33814, 0x33814,
		0x3382c, 0x3382c,
		0x33880, 0x3388c,
		0x338e8, 0x338ec,
		0x33900, 0x33948,
		0x33960, 0x3399c,
		0x339f0, 0x33ae4,
		0x33af8, 0x33b10,
		0x33b28, 0x33b28,
		0x33b3c, 0x33b50,
		0x33bf0, 0x33c10,
		0x33c28, 0x33c28,
		0x33c3c, 0x33c50,
		0x33cf0, 0x33cfc,
		0x34000, 0x34030,
		0x34100, 0x34144,
		0x34190, 0x341d0,
		0x34200, 0x34318,
		0x34400, 0x3452c,
		0x34540, 0x3461c,
		0x34800, 0x34834,
		0x348c0, 0x34908,
		0x34910, 0x349ac,
		0x34a00, 0x34a04,
		0x34a0c, 0x34a2c,
		0x34a44, 0x34a50,
		0x34a74, 0x34c24,
		0x34d08, 0x34d14,
		0x34d1c, 0x34d20,
		0x34d3c, 0x34d50,
		0x35200, 0x3520c,
		0x35220, 0x35220,
		0x35240, 0x35240,
		0x35600, 0x35600,
		0x35608, 0x3560c,
		0x35a00, 0x35a1c,
		0x35e04, 0x35e20,
		0x35e38, 0x35e3c,
		0x35e80, 0x35e80,
		0x35e88, 0x35ea8,
		0x35eb0, 0x35eb4,
		0x35ec8, 0x35ed4,
		0x35fb8, 0x36004,
		0x36208, 0x3623c,
		0x36600, 0x36630,
		0x36a00, 0x36abc,
		0x36b00, 0x36b70,
		0x37000, 0x37048,
		0x37060, 0x3709c,
		0x370f0, 0x37148,
		0x37160, 0x3719c,
		0x371f0, 0x372e4,
		0x372f8, 0x373e4,
		0x373f8, 0x37448,
		0x37460, 0x3749c,
		0x374f0, 0x37548,
		0x37560, 0x3759c,
		0x375f0, 0x376e4,
		0x376f8, 0x377e4,
		0x377f8, 0x377fc,
		0x37814, 0x37814,
		0x3782c, 0x3782c,
		0x37880, 0x3788c,
		0x378e8, 0x378ec,
		0x37900, 0x37948,
		0x37960, 0x3799c,
		0x379f0, 0x37ae4,
		0x37af8, 0x37b10,
		0x37b28, 0x37b28,
		0x37b3c, 0x37b50,
		0x37bf0, 0x37c10,
		0x37c28, 0x37c28,
		0x37c3c, 0x37c50,
		0x37cf0, 0x37cfc,
		0x38000, 0x38030,
		0x38100, 0x38144,
		0x38190, 0x381d0,
		0x38200, 0x38318,
		0x38400, 0x3852c,
		0x38540, 0x3861c,
		0x38800, 0x38834,
		0x388c0, 0x38908,
		0x38910, 0x389ac,
		0x38a00, 0x38a04,
		0x38a0c, 0x38a2c,
		0x38a44, 0x38a50,
		0x38a74, 0x38c24,
		0x38d08, 0x38d14,
		0x38d1c, 0x38d20,
		0x38d3c, 0x38d50,
		0x39200, 0x3920c,
		0x39220, 0x39220,
		0x39240, 0x39240,
		0x39600, 0x39600,
		0x39608, 0x3960c,
		0x39a00, 0x39a1c,
		0x39e04, 0x39e20,
		0x39e38, 0x39e3c,
		0x39e80, 0x39e80,
		0x39e88, 0x39ea8,
		0x39eb0, 0x39eb4,
		0x39ec8, 0x39ed4,
		0x39fb8, 0x3a004,
		0x3a208, 0x3a23c,
		0x3a600, 0x3a630,
		0x3aa00, 0x3aabc,
		0x3ab00, 0x3ab70,
		0x3b000, 0x3b048,
		0x3b060, 0x3b09c,
		0x3b0f0, 0x3b148,
		0x3b160, 0x3b19c,
		0x3b1f0, 0x3b2e4,
		0x3b2f8, 0x3b3e4,
		0x3b3f8, 0x3b448,
		0x3b460, 0x3b49c,
		0x3b4f0, 0x3b548,
		0x3b560, 0x3b59c,
		0x3b5f0, 0x3b6e4,
		0x3b6f8, 0x3b7e4,
		0x3b7f8, 0x3b7fc,
		0x3b814, 0x3b814,
		0x3b82c, 0x3b82c,
		0x3b880, 0x3b88c,
		0x3b8e8, 0x3b8ec,
		0x3b900, 0x3b948,
		0x3b960, 0x3b99c,
		0x3b9f0, 0x3bae4,
		0x3baf8, 0x3bb10,
		0x3bb28, 0x3bb28,
		0x3bb3c, 0x3bb50,
		0x3bbf0, 0x3bc10,
		0x3bc28, 0x3bc28,
		0x3bc3c, 0x3bc50,
		0x3bcf0, 0x3bcfc,
		0x3c000, 0x3c030,
		0x3c100, 0x3c144,
		0x3c190, 0x3c1d0,
		0x3c200, 0x3c318,
		0x3c400, 0x3c52c,
		0x3c540, 0x3c61c,
		0x3c800, 0x3c834,
		0x3c8c0, 0x3c908,
		0x3c910, 0x3c9ac,
		0x3ca00, 0x3ca04,
		0x3ca0c, 0x3ca2c,
		0x3ca44, 0x3ca50,
		0x3ca74, 0x3cc24,
		0x3cd08, 0x3cd14,
		0x3cd1c, 0x3cd20,
		0x3cd3c, 0x3cd50,
		0x3d200, 0x3d20c,
		0x3d220, 0x3d220,
		0x3d240, 0x3d240,
		0x3d600, 0x3d600,
		0x3d608, 0x3d60c,
		0x3da00, 0x3da1c,
		0x3de04, 0x3de20,
		0x3de38, 0x3de3c,
		0x3de80, 0x3de80,
		0x3de88, 0x3dea8,
		0x3deb0, 0x3deb4,
		0x3dec8, 0x3ded4,
		0x3dfb8, 0x3e004,
		0x3e208, 0x3e23c,
		0x3e600, 0x3e630,
		0x3ea00, 0x3eabc,
		0x3eb00, 0x3eb70,
		0x3f000, 0x3f048,
		0x3f060, 0x3f09c,
		0x3f0f0, 0x3f148,
		0x3f160, 0x3f19c,
		0x3f1f0, 0x3f2e4,
		0x3f2f8, 0x3f3e4,
		0x3f3f8, 0x3f448,
		0x3f460, 0x3f49c,
		0x3f4f0, 0x3f548,
		0x3f560, 0x3f59c,
		0x3f5f0, 0x3f6e4,
		0x3f6f8, 0x3f7e4,
		0x3f7f8, 0x3f7fc,
		0x3f814, 0x3f814,
		0x3f82c, 0x3f82c,
		0x3f880, 0x3f88c,
		0x3f8e8, 0x3f8ec,
		0x3f900, 0x3f948,
		0x3f960, 0x3f99c,
		0x3f9f0, 0x3fae4,
		0x3faf8, 0x3fb10,
		0x3fb28, 0x3fb28,
		0x3fb3c, 0x3fb50,
		0x3fbf0, 0x3fc10,
		0x3fc28, 0x3fc28,
		0x3fc3c, 0x3fc50,
		0x3fcf0, 0x3fcfc,
		0x40000, 0x4000c,
		0x40040, 0x40068,
		0x40080, 0x40144,
		0x40180, 0x4018c,
		0x40200, 0x40298,
		0x402ac, 0x4033c,
		0x403f8, 0x403fc,
		0x41304, 0x413c4,
		0x41400, 0x4141c,
		0x41480, 0x414d0,
		0x44000, 0x44078,
		0x440c0, 0x44278,
		0x442c0, 0x44478,
		0x444c0, 0x44678,
		0x446c0, 0x44878,
		0x448c0, 0x449fc,
		0x45000, 0x45068,
		0x45080, 0x45084,
		0x450a0, 0x450b0,
		0x45200, 0x45268,
		0x45280, 0x45284,
		0x452a0, 0x452b0,
		0x460c0, 0x460e4,
		0x47000, 0x4708c,
		0x47200, 0x47250,
		0x47400, 0x47420,
		0x47600, 0x47618,
		0x47800, 0x47814,
		0x48000, 0x4800c,
		0x48040, 0x48068,
		0x48080, 0x48144,
		0x48180, 0x4818c,
		0x48200, 0x48298,
		0x482ac, 0x4833c,
		0x483f8, 0x483fc,
		0x49304, 0x493c4,
		0x49400, 0x4941c,
		0x49480, 0x494d0,
		0x4c000, 0x4c078,
		0x4c0c0, 0x4c278,
		0x4c2c0, 0x4c478,
		0x4c4c0, 0x4c678,
		0x4c6c0, 0x4c878,
		0x4c8c0, 0x4c9fc,
		0x4d000, 0x4d068,
		0x4d080, 0x4d084,
		0x4d0a0, 0x4d0b0,
		0x4d200, 0x4d268,
		0x4d280, 0x4d284,
		0x4d2a0, 0x4d2b0,
		0x4e0c0, 0x4e0e4,
		0x4f000, 0x4f08c,
		0x4f200, 0x4f250,
		0x4f400, 0x4f420,
		0x4f600, 0x4f618,
		0x4f800, 0x4f814,
		0x50000, 0x500cc,
		0x50400, 0x50400,
		0x50800, 0x508cc,
		0x50c00, 0x50c00,
		0x51000, 0x5101c,
		0x51300, 0x51308,
	};

	int i;
	struct adapter *ap = netdev2adap(dev);
	static const unsigned int *reg_ranges;
	int arr_size = 0, buf_size = 0;

	if (is_t4(ap->params.chip)) {
		reg_ranges = &t4_reg_ranges[0];
		arr_size = ARRAY_SIZE(t4_reg_ranges);
		buf_size = T4_REGMAP_SIZE;
	} else {
		reg_ranges = &t5_reg_ranges[0];
		arr_size = ARRAY_SIZE(t5_reg_ranges);
		buf_size = T5_REGMAP_SIZE;
	}

	regs->version = mk_adap_vers(ap);

	memset(buf, 0, buf_size);
	for (i = 0; i < arr_size; i += 2)
		reg_block_dump(ap, buf, reg_ranges[i], reg_ranges[i + 1]);
}

static int restart_autoneg(struct net_device *dev)
{
	struct port_info *p = netdev_priv(dev);

	if (!netif_running(dev))
		return -EAGAIN;
	if (p->link_cfg.autoneg != AUTONEG_ENABLE)
		return -EINVAL;
	t4_restart_aneg(p->adapter, p->adapter->fn, p->tx_chan);
	return 0;
}

static int identify_port(struct net_device *dev,
			 enum ethtool_phys_id_state state)
{
	unsigned int val;
	struct adapter *adap = netdev2adap(dev);

	if (state == ETHTOOL_ID_ACTIVE)
		val = 0xffff;
	else if (state == ETHTOOL_ID_INACTIVE)
		val = 0;
	else
		return -EINVAL;

	return t4_identify_port(adap, adap->fn, netdev2pinfo(dev)->viid, val);
}

static unsigned int from_fw_linkcaps(unsigned int type, unsigned int caps)
{
	unsigned int v = 0;

	if (type == FW_PORT_TYPE_BT_SGMII || type == FW_PORT_TYPE_BT_XFI ||
	    type == FW_PORT_TYPE_BT_XAUI) {
		v |= SUPPORTED_TP;
		if (caps & FW_PORT_CAP_SPEED_100M)
			v |= SUPPORTED_100baseT_Full;
		if (caps & FW_PORT_CAP_SPEED_1G)
			v |= SUPPORTED_1000baseT_Full;
		if (caps & FW_PORT_CAP_SPEED_10G)
			v |= SUPPORTED_10000baseT_Full;
	} else if (type == FW_PORT_TYPE_KX4 || type == FW_PORT_TYPE_KX) {
		v |= SUPPORTED_Backplane;
		if (caps & FW_PORT_CAP_SPEED_1G)
			v |= SUPPORTED_1000baseKX_Full;
		if (caps & FW_PORT_CAP_SPEED_10G)
			v |= SUPPORTED_10000baseKX4_Full;
	} else if (type == FW_PORT_TYPE_KR)
		v |= SUPPORTED_Backplane | SUPPORTED_10000baseKR_Full;
	else if (type == FW_PORT_TYPE_BP_AP)
		v |= SUPPORTED_Backplane | SUPPORTED_10000baseR_FEC |
		     SUPPORTED_10000baseKR_Full | SUPPORTED_1000baseKX_Full;
	else if (type == FW_PORT_TYPE_BP4_AP)
		v |= SUPPORTED_Backplane | SUPPORTED_10000baseR_FEC |
		     SUPPORTED_10000baseKR_Full | SUPPORTED_1000baseKX_Full |
		     SUPPORTED_10000baseKX4_Full;
	else if (type == FW_PORT_TYPE_FIBER_XFI ||
		 type == FW_PORT_TYPE_FIBER_XAUI || type == FW_PORT_TYPE_SFP) {
		v |= SUPPORTED_FIBRE;
		if (caps & FW_PORT_CAP_SPEED_1G)
			v |= SUPPORTED_1000baseT_Full;
		if (caps & FW_PORT_CAP_SPEED_10G)
			v |= SUPPORTED_10000baseT_Full;
	} else if (type == FW_PORT_TYPE_BP40_BA)
		v |= SUPPORTED_40000baseSR4_Full;

	if (caps & FW_PORT_CAP_ANEG)
		v |= SUPPORTED_Autoneg;
	return v;
}

static unsigned int to_fw_linkcaps(unsigned int caps)
{
	unsigned int v = 0;

	if (caps & ADVERTISED_100baseT_Full)
		v |= FW_PORT_CAP_SPEED_100M;
	if (caps & ADVERTISED_1000baseT_Full)
		v |= FW_PORT_CAP_SPEED_1G;
	if (caps & ADVERTISED_10000baseT_Full)
		v |= FW_PORT_CAP_SPEED_10G;
	if (caps & ADVERTISED_40000baseSR4_Full)
		v |= FW_PORT_CAP_SPEED_40G;
	return v;
}

static int get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	const struct port_info *p = netdev_priv(dev);

	if (p->port_type == FW_PORT_TYPE_BT_SGMII ||
	    p->port_type == FW_PORT_TYPE_BT_XFI ||
	    p->port_type == FW_PORT_TYPE_BT_XAUI)
		cmd->port = PORT_TP;
	else if (p->port_type == FW_PORT_TYPE_FIBER_XFI ||
		 p->port_type == FW_PORT_TYPE_FIBER_XAUI)
		cmd->port = PORT_FIBRE;
	else if (p->port_type == FW_PORT_TYPE_SFP ||
		 p->port_type == FW_PORT_TYPE_QSFP_10G ||
		 p->port_type == FW_PORT_TYPE_QSFP) {
		if (p->mod_type == FW_PORT_MOD_TYPE_LR ||
		    p->mod_type == FW_PORT_MOD_TYPE_SR ||
		    p->mod_type == FW_PORT_MOD_TYPE_ER ||
		    p->mod_type == FW_PORT_MOD_TYPE_LRM)
			cmd->port = PORT_FIBRE;
		else if (p->mod_type == FW_PORT_MOD_TYPE_TWINAX_PASSIVE ||
			 p->mod_type == FW_PORT_MOD_TYPE_TWINAX_ACTIVE)
			cmd->port = PORT_DA;
		else
			cmd->port = PORT_OTHER;
	} else
		cmd->port = PORT_OTHER;

	if (p->mdio_addr >= 0) {
		cmd->phy_address = p->mdio_addr;
		cmd->transceiver = XCVR_EXTERNAL;
		cmd->mdio_support = p->port_type == FW_PORT_TYPE_BT_SGMII ?
			MDIO_SUPPORTS_C22 : MDIO_SUPPORTS_C45;
	} else {
		cmd->phy_address = 0;  /* not really, but no better option */
		cmd->transceiver = XCVR_INTERNAL;
		cmd->mdio_support = 0;
	}

	cmd->supported = from_fw_linkcaps(p->port_type, p->link_cfg.supported);
	cmd->advertising = from_fw_linkcaps(p->port_type,
					    p->link_cfg.advertising);
	ethtool_cmd_speed_set(cmd,
			      netif_carrier_ok(dev) ? p->link_cfg.speed : 0);
	cmd->duplex = DUPLEX_FULL;
	cmd->autoneg = p->link_cfg.autoneg;
	cmd->maxtxpkt = 0;
	cmd->maxrxpkt = 0;
	return 0;
}

static unsigned int speed_to_caps(int speed)
{
	if (speed == 100)
		return FW_PORT_CAP_SPEED_100M;
	if (speed == 1000)
		return FW_PORT_CAP_SPEED_1G;
	if (speed == 10000)
		return FW_PORT_CAP_SPEED_10G;
	if (speed == 40000)
		return FW_PORT_CAP_SPEED_40G;
	return 0;
}

static int set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	unsigned int cap;
	struct port_info *p = netdev_priv(dev);
	struct link_config *lc = &p->link_cfg;
	u32 speed = ethtool_cmd_speed(cmd);

	if (cmd->duplex != DUPLEX_FULL)     /* only full-duplex supported */
		return -EINVAL;

	if (!(lc->supported & FW_PORT_CAP_ANEG)) {
		/*
		 * PHY offers a single speed.  See if that's what's
		 * being requested.
		 */
		if (cmd->autoneg == AUTONEG_DISABLE &&
		    (lc->supported & speed_to_caps(speed)))
			return 0;
		return -EINVAL;
	}

	if (cmd->autoneg == AUTONEG_DISABLE) {
		cap = speed_to_caps(speed);

		if (!(lc->supported & cap) ||
		    (speed == 1000) ||
		    (speed == 10000) ||
		    (speed == 40000))
			return -EINVAL;
		lc->requested_speed = cap;
		lc->advertising = 0;
	} else {
		cap = to_fw_linkcaps(cmd->advertising);
		if (!(lc->supported & cap))
			return -EINVAL;
		lc->requested_speed = 0;
		lc->advertising = cap | FW_PORT_CAP_ANEG;
	}
	lc->autoneg = cmd->autoneg;

	if (netif_running(dev))
		return t4_link_start(p->adapter, p->adapter->fn, p->tx_chan,
				     lc);
	return 0;
}

static void get_pauseparam(struct net_device *dev,
			   struct ethtool_pauseparam *epause)
{
	struct port_info *p = netdev_priv(dev);

	epause->autoneg = (p->link_cfg.requested_fc & PAUSE_AUTONEG) != 0;
	epause->rx_pause = (p->link_cfg.fc & PAUSE_RX) != 0;
	epause->tx_pause = (p->link_cfg.fc & PAUSE_TX) != 0;
}

static int set_pauseparam(struct net_device *dev,
			  struct ethtool_pauseparam *epause)
{
	struct port_info *p = netdev_priv(dev);
	struct link_config *lc = &p->link_cfg;

	if (epause->autoneg == AUTONEG_DISABLE)
		lc->requested_fc = 0;
	else if (lc->supported & FW_PORT_CAP_ANEG)
		lc->requested_fc = PAUSE_AUTONEG;
	else
		return -EINVAL;

	if (epause->rx_pause)
		lc->requested_fc |= PAUSE_RX;
	if (epause->tx_pause)
		lc->requested_fc |= PAUSE_TX;
	if (netif_running(dev))
		return t4_link_start(p->adapter, p->adapter->fn, p->tx_chan,
				     lc);
	return 0;
}

static void get_sge_param(struct net_device *dev, struct ethtool_ringparam *e)
{
	const struct port_info *pi = netdev_priv(dev);
	const struct sge *s = &pi->adapter->sge;

	e->rx_max_pending = MAX_RX_BUFFERS;
	e->rx_mini_max_pending = MAX_RSPQ_ENTRIES;
	e->rx_jumbo_max_pending = 0;
	e->tx_max_pending = MAX_TXQ_ENTRIES;

	e->rx_pending = s->ethrxq[pi->first_qset].fl.size - 8;
	e->rx_mini_pending = s->ethrxq[pi->first_qset].rspq.size;
	e->rx_jumbo_pending = 0;
	e->tx_pending = s->ethtxq[pi->first_qset].q.size;
}

static int set_sge_param(struct net_device *dev, struct ethtool_ringparam *e)
{
	int i;
	const struct port_info *pi = netdev_priv(dev);
	struct adapter *adapter = pi->adapter;
	struct sge *s = &adapter->sge;

	if (e->rx_pending > MAX_RX_BUFFERS || e->rx_jumbo_pending ||
	    e->tx_pending > MAX_TXQ_ENTRIES ||
	    e->rx_mini_pending > MAX_RSPQ_ENTRIES ||
	    e->rx_mini_pending < MIN_RSPQ_ENTRIES ||
	    e->rx_pending < MIN_FL_ENTRIES || e->tx_pending < MIN_TXQ_ENTRIES)
		return -EINVAL;

	if (adapter->flags & FULL_INIT_DONE)
		return -EBUSY;

	for (i = 0; i < pi->nqsets; ++i) {
		s->ethtxq[pi->first_qset + i].q.size = e->tx_pending;
		s->ethrxq[pi->first_qset + i].fl.size = e->rx_pending + 8;
		s->ethrxq[pi->first_qset + i].rspq.size = e->rx_mini_pending;
	}
	return 0;
}

=======
>>>>>>> v4.9.227
static int closest_timer(const struct sge *s, int time)
{
	int i, delta, match = 0, min_delta = INT_MAX;

	for (i = 0; i < ARRAY_SIZE(s->timer_val); i++) {
		delta = time - s->timer_val[i];
		if (delta < 0)
			delta = -delta;
		if (delta < min_delta) {
			min_delta = delta;
			match = i;
		}
	}
	return match;
}

static int closest_thres(const struct sge *s, int thres)
{
	int i, delta, match = 0, min_delta = INT_MAX;

	for (i = 0; i < ARRAY_SIZE(s->counter_val); i++) {
		delta = thres - s->counter_val[i];
		if (delta < 0)
			delta = -delta;
		if (delta < min_delta) {
			min_delta = delta;
			match = i;
		}
	}
	return match;
}

<<<<<<< HEAD
/*
 * Return a queue's interrupt hold-off time in us.  0 means no timer.
 */
static unsigned int qtimer_val(const struct adapter *adap,
			       const struct sge_rspq *q)
{
	unsigned int idx = q->intr_params >> 1;

	return idx < SGE_NTIMERS ? adap->sge.timer_val[idx] : 0;
}

/**
 *	set_rspq_intr_params - set a queue's interrupt holdoff parameters
=======
/**
 *	cxgb4_set_rspq_intr_params - set a queue's interrupt holdoff parameters
>>>>>>> v4.9.227
 *	@q: the Rx queue
 *	@us: the hold-off time in us, or 0 to disable timer
 *	@cnt: the hold-off packet count, or 0 to disable counter
 *
 *	Sets an Rx queue's interrupt hold-off time and packet count.  At least
 *	one of the two needs to be enabled for the queue to generate interrupts.
 */
<<<<<<< HEAD
static int set_rspq_intr_params(struct sge_rspq *q,
				unsigned int us, unsigned int cnt)
=======
int cxgb4_set_rspq_intr_params(struct sge_rspq *q,
			       unsigned int us, unsigned int cnt)
>>>>>>> v4.9.227
{
	struct adapter *adap = q->adap;

	if ((us | cnt) == 0)
		cnt = 1;

	if (cnt) {
		int err;
		u32 v, new_idx;

		new_idx = closest_thres(&adap->sge, cnt);
		if (q->desc && q->pktcnt_idx != new_idx) {
			/* the queue has already been created, update it */
<<<<<<< HEAD
			v = FW_PARAMS_MNEM(FW_PARAMS_MNEM_DMAQ) |
			    FW_PARAMS_PARAM_X(FW_PARAMS_PARAM_DMAQ_IQ_INTCNTTHRESH) |
			    FW_PARAMS_PARAM_YZ(q->cntxt_id);
			err = t4_set_params(adap, adap->fn, adap->fn, 0, 1, &v,
					    &new_idx);
=======
			v = FW_PARAMS_MNEM_V(FW_PARAMS_MNEM_DMAQ) |
			    FW_PARAMS_PARAM_X_V(
					FW_PARAMS_PARAM_DMAQ_IQ_INTCNTTHRESH) |
			    FW_PARAMS_PARAM_YZ_V(q->cntxt_id);
			err = t4_set_params(adap, adap->mbox, adap->pf, 0, 1,
					    &v, &new_idx);
>>>>>>> v4.9.227
			if (err)
				return err;
		}
		q->pktcnt_idx = new_idx;
	}

	us = us == 0 ? 6 : closest_timer(&adap->sge, us);
<<<<<<< HEAD
	q->intr_params = QINTR_TIMER_IDX(us) | (cnt > 0 ? QINTR_CNT_EN : 0);
	return 0;
}

/**
 * set_rx_intr_params - set a net devices's RX interrupt holdoff paramete!
 * @dev: the network device
 * @us: the hold-off time in us, or 0 to disable timer
 * @cnt: the hold-off packet count, or 0 to disable counter
 *
 * Set the RX interrupt hold-off parameters for a network device.
 */
static int set_rx_intr_params(struct net_device *dev,
			      unsigned int us, unsigned int cnt)
{
	int i, err;
	struct port_info *pi = netdev_priv(dev);
	struct adapter *adap = pi->adapter;
	struct sge_eth_rxq *q = &adap->sge.ethrxq[pi->first_qset];

	for (i = 0; i < pi->nqsets; i++, q++) {
		err = set_rspq_intr_params(&q->rspq, us, cnt);
		if (err)
			return err;
	}
	return 0;
}

static int set_adaptive_rx_setting(struct net_device *dev, int adaptive_rx)
{
	int i;
	struct port_info *pi = netdev_priv(dev);
	struct adapter *adap = pi->adapter;
	struct sge_eth_rxq *q = &adap->sge.ethrxq[pi->first_qset];

	for (i = 0; i < pi->nqsets; i++, q++)
		q->rspq.adaptive_rx = adaptive_rx;

	return 0;
}

static int get_adaptive_rx_setting(struct net_device *dev)
{
	struct port_info *pi = netdev_priv(dev);
	struct adapter *adap = pi->adapter;
	struct sge_eth_rxq *q = &adap->sge.ethrxq[pi->first_qset];

	return q->rspq.adaptive_rx;
}

static int set_coalesce(struct net_device *dev, struct ethtool_coalesce *c)
{
	set_adaptive_rx_setting(dev, c->use_adaptive_rx_coalesce);
	return set_rx_intr_params(dev, c->rx_coalesce_usecs,
				  c->rx_max_coalesced_frames);
}

static int get_coalesce(struct net_device *dev, struct ethtool_coalesce *c)
{
	const struct port_info *pi = netdev_priv(dev);
	const struct adapter *adap = pi->adapter;
	const struct sge_rspq *rq = &adap->sge.ethrxq[pi->first_qset].rspq;

	c->rx_coalesce_usecs = qtimer_val(adap, rq);
	c->rx_max_coalesced_frames = (rq->intr_params & QINTR_CNT_EN) ?
		adap->sge.counter_val[rq->pktcnt_idx] : 0;
	c->use_adaptive_rx_coalesce = get_adaptive_rx_setting(dev);
	return 0;
}

/**
 *	eeprom_ptov - translate a physical EEPROM address to virtual
 *	@phys_addr: the physical EEPROM address
 *	@fn: the PCI function number
 *	@sz: size of function-specific area
 *
 *	Translate a physical EEPROM address to virtual.  The first 1K is
 *	accessed through virtual addresses starting at 31K, the rest is
 *	accessed through virtual addresses starting at 0.
 *
 *	The mapping is as follows:
 *	[0..1K) -> [31K..32K)
 *	[1K..1K+A) -> [31K-A..31K)
 *	[1K+A..ES) -> [0..ES-A-1K)
 *
 *	where A = @fn * @sz, and ES = EEPROM size.
 */
static int eeprom_ptov(unsigned int phys_addr, unsigned int fn, unsigned int sz)
{
	fn *= sz;
	if (phys_addr < 1024)
		return phys_addr + (31 << 10);
	if (phys_addr < 1024 + fn)
		return 31744 - fn + phys_addr - 1024;
	if (phys_addr < EEPROMSIZE)
		return phys_addr - 1024 - fn;
	return -EINVAL;
}

/*
 * The next two routines implement eeprom read/write from physical addresses.
 */
static int eeprom_rd_phys(struct adapter *adap, unsigned int phys_addr, u32 *v)
{
	int vaddr = eeprom_ptov(phys_addr, adap->fn, EEPROMPFSIZE);

	if (vaddr >= 0)
		vaddr = pci_read_vpd(adap->pdev, vaddr, sizeof(u32), v);
	return vaddr < 0 ? vaddr : 0;
}

static int eeprom_wr_phys(struct adapter *adap, unsigned int phys_addr, u32 v)
{
	int vaddr = eeprom_ptov(phys_addr, adap->fn, EEPROMPFSIZE);

	if (vaddr >= 0)
		vaddr = pci_write_vpd(adap->pdev, vaddr, sizeof(u32), &v);
	return vaddr < 0 ? vaddr : 0;
}

#define EEPROM_MAGIC 0x38E2F10C

static int get_eeprom(struct net_device *dev, struct ethtool_eeprom *e,
		      u8 *data)
{
	int i, err = 0;
	struct adapter *adapter = netdev2adap(dev);

	u8 *buf = kmalloc(EEPROMSIZE, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	e->magic = EEPROM_MAGIC;
	for (i = e->offset & ~3; !err && i < e->offset + e->len; i += 4)
		err = eeprom_rd_phys(adapter, i, (u32 *)&buf[i]);

	if (!err)
		memcpy(data, buf + e->offset, e->len);
	kfree(buf);
	return err;
}

static int set_eeprom(struct net_device *dev, struct ethtool_eeprom *eeprom,
		      u8 *data)
{
	u8 *buf;
	int err = 0;
	u32 aligned_offset, aligned_len, *p;
	struct adapter *adapter = netdev2adap(dev);

	if (eeprom->magic != EEPROM_MAGIC)
		return -EINVAL;

	aligned_offset = eeprom->offset & ~3;
	aligned_len = (eeprom->len + (eeprom->offset & 3) + 3) & ~3;

	if (adapter->fn > 0) {
		u32 start = 1024 + adapter->fn * EEPROMPFSIZE;

		if (aligned_offset < start ||
		    aligned_offset + aligned_len > start + EEPROMPFSIZE)
			return -EPERM;
	}

	if (aligned_offset != eeprom->offset || aligned_len != eeprom->len) {
		/*
		 * RMW possibly needed for first or last words.
		 */
		buf = kmalloc(aligned_len, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;
		err = eeprom_rd_phys(adapter, aligned_offset, (u32 *)buf);
		if (!err && aligned_len > 4)
			err = eeprom_rd_phys(adapter,
					     aligned_offset + aligned_len - 4,
					     (u32 *)&buf[aligned_len - 4]);
		if (err)
			goto out;
		memcpy(buf + (eeprom->offset & 3), data, eeprom->len);
	} else
		buf = data;

	err = t4_seeprom_wp(adapter, false);
	if (err)
		goto out;

	for (p = (u32 *)buf; !err && aligned_len; aligned_len -= 4, p++) {
		err = eeprom_wr_phys(adapter, aligned_offset, *p);
		aligned_offset += 4;
	}

	if (!err)
		err = t4_seeprom_wp(adapter, true);
out:
	if (buf != data)
		kfree(buf);
	return err;
}

static int set_flash(struct net_device *netdev, struct ethtool_flash *ef)
{
	int ret;
	const struct firmware *fw;
	struct adapter *adap = netdev2adap(netdev);
	unsigned int mbox = FW_PCIE_FW_MASTER_MASK + 1;

	ef->data[sizeof(ef->data) - 1] = '\0';
	ret = request_firmware(&fw, ef->data, adap->pdev_dev);
	if (ret < 0)
		return ret;

	/* If the adapter has been fully initialized then we'll go ahead and
	 * try to get the firmware's cooperation in upgrading to the new
	 * firmware image otherwise we'll try to do the entire job from the
	 * host ... and we always "force" the operation in this path.
	 */
	if (adap->flags & FULL_INIT_DONE)
		mbox = adap->mbox;

	ret = t4_fw_upgrade(adap, mbox, fw->data, fw->size, 1);
	release_firmware(fw);
	if (!ret)
		dev_info(adap->pdev_dev, "loaded firmware %s,"
			 " reload cxgb4 driver\n", ef->data);
	return ret;
}

#define WOL_SUPPORTED (WAKE_BCAST | WAKE_MAGIC)
#define BCAST_CRC 0xa0ccc1a6

static void get_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	wol->supported = WAKE_BCAST | WAKE_MAGIC;
	wol->wolopts = netdev2adap(dev)->wol;
	memset(&wol->sopass, 0, sizeof(wol->sopass));
}

static int set_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	int err = 0;
	struct port_info *pi = netdev_priv(dev);

	if (wol->wolopts & ~WOL_SUPPORTED)
		return -EINVAL;
	t4_wol_magic_enable(pi->adapter, pi->tx_chan,
			    (wol->wolopts & WAKE_MAGIC) ? dev->dev_addr : NULL);
	if (wol->wolopts & WAKE_BCAST) {
		err = t4_wol_pat_enable(pi->adapter, pi->tx_chan, 0xfe, ~0ULL,
					~0ULL, 0, false);
		if (!err)
			err = t4_wol_pat_enable(pi->adapter, pi->tx_chan, 1,
						~6ULL, ~0ULL, BCAST_CRC, true);
	} else
		t4_wol_pat_enable(pi->adapter, pi->tx_chan, 0, 0, 0, 0, false);
	return err;
}

=======
	q->intr_params = QINTR_TIMER_IDX_V(us) | QINTR_CNT_EN_V(cnt > 0);
	return 0;
}

>>>>>>> v4.9.227
static int cxgb_set_features(struct net_device *dev, netdev_features_t features)
{
	const struct port_info *pi = netdev_priv(dev);
	netdev_features_t changed = dev->features ^ features;
	int err;

	if (!(changed & NETIF_F_HW_VLAN_CTAG_RX))
		return 0;

<<<<<<< HEAD
	err = t4_set_rxmode(pi->adapter, pi->adapter->fn, pi->viid, -1,
=======
	err = t4_set_rxmode(pi->adapter, pi->adapter->pf, pi->viid, -1,
>>>>>>> v4.9.227
			    -1, -1, -1,
			    !!(features & NETIF_F_HW_VLAN_CTAG_RX), true);
	if (unlikely(err))
		dev->features = features ^ NETIF_F_HW_VLAN_CTAG_RX;
	return err;
}

<<<<<<< HEAD
static u32 get_rss_table_size(struct net_device *dev)
{
	const struct port_info *pi = netdev_priv(dev);

	return pi->rss_size;
}

static int get_rss_table(struct net_device *dev, u32 *p, u8 *key)
{
	const struct port_info *pi = netdev_priv(dev);
	unsigned int n = pi->rss_size;

	while (n--)
		p[n] = pi->rss[n];
	return 0;
}

static int set_rss_table(struct net_device *dev, const u32 *p, const u8 *key)
{
	unsigned int i;
	struct port_info *pi = netdev_priv(dev);

	for (i = 0; i < pi->rss_size; i++)
		pi->rss[i] = p[i];
	if (pi->adapter->flags & FULL_INIT_DONE)
		return write_rss(pi, pi->rss);
	return 0;
}

static int get_rxnfc(struct net_device *dev, struct ethtool_rxnfc *info,
		     u32 *rules)
{
	const struct port_info *pi = netdev_priv(dev);

	switch (info->cmd) {
	case ETHTOOL_GRXFH: {
		unsigned int v = pi->rss_mode;

		info->data = 0;
		switch (info->flow_type) {
		case TCP_V4_FLOW:
			if (v & FW_RSS_VI_CONFIG_CMD_IP4FOURTUPEN)
				info->data = RXH_IP_SRC | RXH_IP_DST |
					     RXH_L4_B_0_1 | RXH_L4_B_2_3;
			else if (v & FW_RSS_VI_CONFIG_CMD_IP4TWOTUPEN)
				info->data = RXH_IP_SRC | RXH_IP_DST;
			break;
		case UDP_V4_FLOW:
			if ((v & FW_RSS_VI_CONFIG_CMD_IP4FOURTUPEN) &&
			    (v & FW_RSS_VI_CONFIG_CMD_UDPEN))
				info->data = RXH_IP_SRC | RXH_IP_DST |
					     RXH_L4_B_0_1 | RXH_L4_B_2_3;
			else if (v & FW_RSS_VI_CONFIG_CMD_IP4TWOTUPEN)
				info->data = RXH_IP_SRC | RXH_IP_DST;
			break;
		case SCTP_V4_FLOW:
		case AH_ESP_V4_FLOW:
		case IPV4_FLOW:
			if (v & FW_RSS_VI_CONFIG_CMD_IP4TWOTUPEN)
				info->data = RXH_IP_SRC | RXH_IP_DST;
			break;
		case TCP_V6_FLOW:
			if (v & FW_RSS_VI_CONFIG_CMD_IP6FOURTUPEN)
				info->data = RXH_IP_SRC | RXH_IP_DST |
					     RXH_L4_B_0_1 | RXH_L4_B_2_3;
			else if (v & FW_RSS_VI_CONFIG_CMD_IP6TWOTUPEN)
				info->data = RXH_IP_SRC | RXH_IP_DST;
			break;
		case UDP_V6_FLOW:
			if ((v & FW_RSS_VI_CONFIG_CMD_IP6FOURTUPEN) &&
			    (v & FW_RSS_VI_CONFIG_CMD_UDPEN))
				info->data = RXH_IP_SRC | RXH_IP_DST |
					     RXH_L4_B_0_1 | RXH_L4_B_2_3;
			else if (v & FW_RSS_VI_CONFIG_CMD_IP6TWOTUPEN)
				info->data = RXH_IP_SRC | RXH_IP_DST;
			break;
		case SCTP_V6_FLOW:
		case AH_ESP_V6_FLOW:
		case IPV6_FLOW:
			if (v & FW_RSS_VI_CONFIG_CMD_IP6TWOTUPEN)
				info->data = RXH_IP_SRC | RXH_IP_DST;
			break;
		}
		return 0;
	}
	case ETHTOOL_GRXRINGS:
		info->data = pi->nqsets;
		return 0;
	}
	return -EOPNOTSUPP;
}

static const struct ethtool_ops cxgb_ethtool_ops = {
	.get_settings      = get_settings,
	.set_settings      = set_settings,
	.get_drvinfo       = get_drvinfo,
	.get_msglevel      = get_msglevel,
	.set_msglevel      = set_msglevel,
	.get_ringparam     = get_sge_param,
	.set_ringparam     = set_sge_param,
	.get_coalesce      = get_coalesce,
	.set_coalesce      = set_coalesce,
	.get_eeprom_len    = get_eeprom_len,
	.get_eeprom        = get_eeprom,
	.set_eeprom        = set_eeprom,
	.get_pauseparam    = get_pauseparam,
	.set_pauseparam    = set_pauseparam,
	.get_link          = ethtool_op_get_link,
	.get_strings       = get_strings,
	.set_phys_id       = identify_port,
	.nway_reset        = restart_autoneg,
	.get_sset_count    = get_sset_count,
	.get_ethtool_stats = get_stats,
	.get_regs_len      = get_regs_len,
	.get_regs          = get_regs,
	.get_wol           = get_wol,
	.set_wol           = set_wol,
	.get_rxnfc         = get_rxnfc,
	.get_rxfh_indir_size = get_rss_table_size,
	.get_rxfh	   = get_rss_table,
	.set_rxfh	   = set_rss_table,
	.flash_device      = set_flash,
};

/*
 * debugfs support
 */
static ssize_t mem_read(struct file *file, char __user *buf, size_t count,
			loff_t *ppos)
{
	loff_t pos = *ppos;
	loff_t avail = file_inode(file)->i_size;
	unsigned int mem = (uintptr_t)file->private_data & 3;
	struct adapter *adap = file->private_data - mem;
	__be32 *data;
	int ret;

	if (pos < 0)
		return -EINVAL;
	if (pos >= avail)
		return 0;
	if (count > avail - pos)
		count = avail - pos;

	data = t4_alloc_mem(count);
	if (!data)
		return -ENOMEM;

	spin_lock(&adap->win0_lock);
	ret = t4_memory_rw(adap, 0, mem, pos, count, data, T4_MEMORY_READ);
	spin_unlock(&adap->win0_lock);
	if (ret) {
		t4_free_mem(data);
		return ret;
	}
	ret = copy_to_user(buf, data, count);

	t4_free_mem(data);
	if (ret)
		return -EFAULT;

	*ppos = pos + count;
	return count;
}

static const struct file_operations mem_debugfs_fops = {
	.owner   = THIS_MODULE,
	.open    = simple_open,
	.read    = mem_read,
	.llseek  = default_llseek,
};

static void add_debugfs_mem(struct adapter *adap, const char *name,
			    unsigned int idx, unsigned int size_mb)
{
	struct dentry *de;

	de = debugfs_create_file(name, S_IRUSR, adap->debugfs_root,
				 (void *)adap + idx, &mem_debugfs_fops);
	if (de && de->d_inode)
		de->d_inode->i_size = size_mb << 20;
}

static int setup_debugfs(struct adapter *adap)
{
	int i;
	u32 size;

	if (IS_ERR_OR_NULL(adap->debugfs_root))
		return -1;

	i = t4_read_reg(adap, MA_TARGET_MEM_ENABLE);
	if (i & EDRAM0_ENABLE) {
		size = t4_read_reg(adap, MA_EDRAM0_BAR);
		add_debugfs_mem(adap, "edc0", MEM_EDC0,	EDRAM_SIZE_GET(size));
	}
	if (i & EDRAM1_ENABLE) {
		size = t4_read_reg(adap, MA_EDRAM1_BAR);
		add_debugfs_mem(adap, "edc1", MEM_EDC1, EDRAM_SIZE_GET(size));
	}
	if (is_t4(adap->params.chip)) {
		size = t4_read_reg(adap, MA_EXT_MEMORY_BAR);
		if (i & EXT_MEM_ENABLE)
			add_debugfs_mem(adap, "mc", MEM_MC,
					EXT_MEM_SIZE_GET(size));
	} else {
		if (i & EXT_MEM_ENABLE) {
			size = t4_read_reg(adap, MA_EXT_MEMORY_BAR);
			add_debugfs_mem(adap, "mc0", MEM_MC0,
					EXT_MEM_SIZE_GET(size));
		}
		if (i & EXT_MEM1_ENABLE) {
			size = t4_read_reg(adap, MA_EXT_MEMORY1_BAR);
			add_debugfs_mem(adap, "mc1", MEM_MC1,
					EXT_MEM_SIZE_GET(size));
		}
	}
	if (adap->l2t)
		debugfs_create_file("l2t", S_IRUSR, adap->debugfs_root, adap,
				    &t4_l2t_fops);
=======
static int setup_debugfs(struct adapter *adap)
{
	if (IS_ERR_OR_NULL(adap->debugfs_root))
		return -1;

#ifdef CONFIG_DEBUG_FS
	t4_setup_debugfs(adap);
#endif
>>>>>>> v4.9.227
	return 0;
}

/*
 * upper-layer driver support
 */

/*
 * Allocate an active-open TID and set it to the supplied value.
 */
int cxgb4_alloc_atid(struct tid_info *t, void *data)
{
	int atid = -1;

	spin_lock_bh(&t->atid_lock);
	if (t->afree) {
		union aopen_entry *p = t->afree;

		atid = (p - t->atid_tab) + t->atid_base;
		t->afree = p->next;
		p->data = data;
		t->atids_in_use++;
	}
	spin_unlock_bh(&t->atid_lock);
	return atid;
}
EXPORT_SYMBOL(cxgb4_alloc_atid);

/*
 * Release an active-open TID.
 */
void cxgb4_free_atid(struct tid_info *t, unsigned int atid)
{
	union aopen_entry *p = &t->atid_tab[atid - t->atid_base];

	spin_lock_bh(&t->atid_lock);
	p->next = t->afree;
	t->afree = p;
	t->atids_in_use--;
	spin_unlock_bh(&t->atid_lock);
}
EXPORT_SYMBOL(cxgb4_free_atid);

/*
 * Allocate a server TID and set it to the supplied value.
 */
int cxgb4_alloc_stid(struct tid_info *t, int family, void *data)
{
	int stid;

	spin_lock_bh(&t->stid_lock);
	if (family == PF_INET) {
		stid = find_first_zero_bit(t->stid_bmap, t->nstids);
		if (stid < t->nstids)
			__set_bit(stid, t->stid_bmap);
		else
			stid = -1;
	} else {
<<<<<<< HEAD
		stid = bitmap_find_free_region(t->stid_bmap, t->nstids, 2);
=======
		stid = bitmap_find_free_region(t->stid_bmap, t->nstids, 1);
>>>>>>> v4.9.227
		if (stid < 0)
			stid = -1;
	}
	if (stid >= 0) {
		t->stid_tab[stid].data = data;
		stid += t->stid_base;
		/* IPv6 requires max of 520 bits or 16 cells in TCAM
		 * This is equivalent to 4 TIDs. With CLIP enabled it
		 * needs 2 TIDs.
		 */
		if (family == PF_INET)
			t->stids_in_use++;
		else
<<<<<<< HEAD
			t->stids_in_use += 4;
=======
			t->stids_in_use += 2;
>>>>>>> v4.9.227
	}
	spin_unlock_bh(&t->stid_lock);
	return stid;
}
EXPORT_SYMBOL(cxgb4_alloc_stid);

/* Allocate a server filter TID and set it to the supplied value.
 */
int cxgb4_alloc_sftid(struct tid_info *t, int family, void *data)
{
	int stid;

	spin_lock_bh(&t->stid_lock);
	if (family == PF_INET) {
		stid = find_next_zero_bit(t->stid_bmap,
				t->nstids + t->nsftids, t->nstids);
		if (stid < (t->nstids + t->nsftids))
			__set_bit(stid, t->stid_bmap);
		else
			stid = -1;
	} else {
		stid = -1;
	}
	if (stid >= 0) {
		t->stid_tab[stid].data = data;
		stid -= t->nstids;
		stid += t->sftid_base;
<<<<<<< HEAD
		t->stids_in_use++;
=======
		t->sftids_in_use++;
>>>>>>> v4.9.227
	}
	spin_unlock_bh(&t->stid_lock);
	return stid;
}
EXPORT_SYMBOL(cxgb4_alloc_sftid);

/* Release a server TID.
 */
void cxgb4_free_stid(struct tid_info *t, unsigned int stid, int family)
{
	/* Is it a server filter TID? */
	if (t->nsftids && (stid >= t->sftid_base)) {
		stid -= t->sftid_base;
		stid += t->nstids;
	} else {
		stid -= t->stid_base;
	}

	spin_lock_bh(&t->stid_lock);
	if (family == PF_INET)
		__clear_bit(stid, t->stid_bmap);
	else
<<<<<<< HEAD
		bitmap_release_region(t->stid_bmap, stid, 2);
	t->stid_tab[stid].data = NULL;
	if (family == PF_INET)
		t->stids_in_use--;
	else
		t->stids_in_use -= 4;
=======
		bitmap_release_region(t->stid_bmap, stid, 1);
	t->stid_tab[stid].data = NULL;
	if (stid < t->nstids) {
		if (family == PF_INET)
			t->stids_in_use--;
		else
			t->stids_in_use -= 2;
	} else {
		t->sftids_in_use--;
	}
>>>>>>> v4.9.227
	spin_unlock_bh(&t->stid_lock);
}
EXPORT_SYMBOL(cxgb4_free_stid);

/*
 * Populate a TID_RELEASE WR.  Caller must properly size the skb.
 */
static void mk_tid_release(struct sk_buff *skb, unsigned int chan,
			   unsigned int tid)
{
	struct cpl_tid_release *req;

	set_wr_txq(skb, CPL_PRIORITY_SETUP, chan);
	req = (struct cpl_tid_release *)__skb_put(skb, sizeof(*req));
	INIT_TP_WR(req, tid);
	OPCODE_TID(req) = htonl(MK_OPCODE_TID(CPL_TID_RELEASE, tid));
}

/*
 * Queue a TID release request and if necessary schedule a work queue to
 * process it.
 */
static void cxgb4_queue_tid_release(struct tid_info *t, unsigned int chan,
				    unsigned int tid)
{
	void **p = &t->tid_tab[tid];
	struct adapter *adap = container_of(t, struct adapter, tids);

	spin_lock_bh(&adap->tid_release_lock);
	*p = adap->tid_release_head;
	/* Low 2 bits encode the Tx channel number */
	adap->tid_release_head = (void **)((uintptr_t)p | chan);
	if (!adap->tid_release_task_busy) {
		adap->tid_release_task_busy = true;
		queue_work(adap->workq, &adap->tid_release_task);
	}
	spin_unlock_bh(&adap->tid_release_lock);
}

/*
 * Process the list of pending TID release requests.
 */
static void process_tid_release_list(struct work_struct *work)
{
	struct sk_buff *skb;
	struct adapter *adap;

	adap = container_of(work, struct adapter, tid_release_task);

	spin_lock_bh(&adap->tid_release_lock);
	while (adap->tid_release_head) {
		void **p = adap->tid_release_head;
		unsigned int chan = (uintptr_t)p & 3;
		p = (void *)p - chan;

		adap->tid_release_head = *p;
		*p = NULL;
		spin_unlock_bh(&adap->tid_release_lock);

		while (!(skb = alloc_skb(sizeof(struct cpl_tid_release),
					 GFP_KERNEL)))
			schedule_timeout_uninterruptible(1);

		mk_tid_release(skb, chan, p - adap->tids.tid_tab);
		t4_ofld_send(adap, skb);
		spin_lock_bh(&adap->tid_release_lock);
	}
	adap->tid_release_task_busy = false;
	spin_unlock_bh(&adap->tid_release_lock);
}

/*
 * Release a TID and inform HW.  If we are unable to allocate the release
 * message we defer to a work queue.
 */
void cxgb4_remove_tid(struct tid_info *t, unsigned int chan, unsigned int tid)
{
<<<<<<< HEAD
	void *old;
	struct sk_buff *skb;
	struct adapter *adap = container_of(t, struct adapter, tids);

	old = t->tid_tab[tid];
	skb = alloc_skb(sizeof(struct cpl_tid_release), GFP_ATOMIC);
	if (likely(skb)) {
		t->tid_tab[tid] = NULL;
=======
	struct sk_buff *skb;
	struct adapter *adap = container_of(t, struct adapter, tids);

	WARN_ON(tid >= t->ntids);

	if (t->tid_tab[tid]) {
		t->tid_tab[tid] = NULL;
		if (t->hash_base && (tid >= t->hash_base))
			atomic_dec(&t->hash_tids_in_use);
		else
			atomic_dec(&t->tids_in_use);
	}

	skb = alloc_skb(sizeof(struct cpl_tid_release), GFP_ATOMIC);
	if (likely(skb)) {
>>>>>>> v4.9.227
		mk_tid_release(skb, chan, tid);
		t4_ofld_send(adap, skb);
	} else
		cxgb4_queue_tid_release(t, chan, tid);
<<<<<<< HEAD
	if (old)
		atomic_dec(&t->tids_in_use);
=======
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(cxgb4_remove_tid);

/*
 * Allocate and initialize the TID tables.  Returns 0 on success.
 */
static int tid_init(struct tid_info *t)
{
<<<<<<< HEAD
	size_t size;
	unsigned int stid_bmap_size;
	unsigned int natids = t->natids;
	struct adapter *adap = container_of(t, struct adapter, tids);

	stid_bmap_size = BITS_TO_LONGS(t->nstids + t->nsftids);
=======
	struct adapter *adap = container_of(t, struct adapter, tids);
	unsigned int max_ftids = t->nftids + t->nsftids;
	unsigned int natids = t->natids;
	unsigned int stid_bmap_size;
	unsigned int ftid_bmap_size;
	size_t size;

	stid_bmap_size = BITS_TO_LONGS(t->nstids + t->nsftids);
	ftid_bmap_size = BITS_TO_LONGS(t->nftids);
>>>>>>> v4.9.227
	size = t->ntids * sizeof(*t->tid_tab) +
	       natids * sizeof(*t->atid_tab) +
	       t->nstids * sizeof(*t->stid_tab) +
	       t->nsftids * sizeof(*t->stid_tab) +
	       stid_bmap_size * sizeof(long) +
<<<<<<< HEAD
	       t->nftids * sizeof(*t->ftid_tab) +
	       t->nsftids * sizeof(*t->ftid_tab);
=======
	       max_ftids * sizeof(*t->ftid_tab) +
	       ftid_bmap_size * sizeof(long);
>>>>>>> v4.9.227

	t->tid_tab = t4_alloc_mem(size);
	if (!t->tid_tab)
		return -ENOMEM;

	t->atid_tab = (union aopen_entry *)&t->tid_tab[t->ntids];
	t->stid_tab = (struct serv_entry *)&t->atid_tab[natids];
	t->stid_bmap = (unsigned long *)&t->stid_tab[t->nstids + t->nsftids];
	t->ftid_tab = (struct filter_entry *)&t->stid_bmap[stid_bmap_size];
<<<<<<< HEAD
	spin_lock_init(&t->stid_lock);
	spin_lock_init(&t->atid_lock);

	t->stids_in_use = 0;
	t->afree = NULL;
	t->atids_in_use = 0;
	atomic_set(&t->tids_in_use, 0);
=======
	t->ftid_bmap = (unsigned long *)&t->ftid_tab[max_ftids];
	spin_lock_init(&t->stid_lock);
	spin_lock_init(&t->atid_lock);
	spin_lock_init(&t->ftid_lock);

	t->stids_in_use = 0;
	t->sftids_in_use = 0;
	t->afree = NULL;
	t->atids_in_use = 0;
	atomic_set(&t->tids_in_use, 0);
	atomic_set(&t->hash_tids_in_use, 0);
>>>>>>> v4.9.227

	/* Setup the free list for atid_tab and clear the stid bitmap. */
	if (natids) {
		while (--natids)
			t->atid_tab[natids - 1].next = &t->atid_tab[natids];
		t->afree = t->atid_tab;
	}
<<<<<<< HEAD
	bitmap_zero(t->stid_bmap, t->nstids + t->nsftids);
	/* Reserve stid 0 for T4/T5 adapters */
	if (!t->stid_base &&
	    (is_t4(adap->params.chip) || is_t5(adap->params.chip)))
		__set_bit(0, t->stid_bmap);

	return 0;
}

int cxgb4_clip_get(const struct net_device *dev,
		   const struct in6_addr *lip)
{
	struct adapter *adap;
	struct fw_clip_cmd c;

	adap = netdev2adap(dev);
	memset(&c, 0, sizeof(c));
	c.op_to_write = htonl(FW_CMD_OP(FW_CLIP_CMD) |
			FW_CMD_REQUEST | FW_CMD_WRITE);
	c.alloc_to_len16 = htonl(F_FW_CLIP_CMD_ALLOC | FW_LEN16(c));
	c.ip_hi = *(__be64 *)(lip->s6_addr);
	c.ip_lo = *(__be64 *)(lip->s6_addr + 8);
	return t4_wr_mbox_meat(adap, adap->mbox, &c, sizeof(c), &c, false);
}
EXPORT_SYMBOL(cxgb4_clip_get);

int cxgb4_clip_release(const struct net_device *dev,
		       const struct in6_addr *lip)
{
	struct adapter *adap;
	struct fw_clip_cmd c;

	adap = netdev2adap(dev);
	memset(&c, 0, sizeof(c));
	c.op_to_write = htonl(FW_CMD_OP(FW_CLIP_CMD) |
			FW_CMD_REQUEST | FW_CMD_READ);
	c.alloc_to_len16 = htonl(F_FW_CLIP_CMD_FREE | FW_LEN16(c));
	c.ip_hi = *(__be64 *)(lip->s6_addr);
	c.ip_lo = *(__be64 *)(lip->s6_addr + 8);
	return t4_wr_mbox_meat(adap, adap->mbox, &c, sizeof(c), &c, false);
}
EXPORT_SYMBOL(cxgb4_clip_release);

=======

	if (is_offload(adap)) {
		bitmap_zero(t->stid_bmap, t->nstids + t->nsftids);
		/* Reserve stid 0 for T4/T5 adapters */
		if (!t->stid_base &&
		    CHELSIO_CHIP_VERSION(adap->params.chip) <= CHELSIO_T5)
			__set_bit(0, t->stid_bmap);
	}

	bitmap_zero(t->ftid_bmap, t->nftids);
	return 0;
}

>>>>>>> v4.9.227
/**
 *	cxgb4_create_server - create an IP server
 *	@dev: the device
 *	@stid: the server TID
 *	@sip: local IP address to bind server to
 *	@sport: the server's TCP port
 *	@queue: queue to direct messages from this server to
 *
 *	Create an IP server for the given port and address.
 *	Returns <0 on error and one of the %NET_XMIT_* values on success.
 */
int cxgb4_create_server(const struct net_device *dev, unsigned int stid,
			__be32 sip, __be16 sport, __be16 vlan,
			unsigned int queue)
{
	unsigned int chan;
	struct sk_buff *skb;
	struct adapter *adap;
	struct cpl_pass_open_req *req;
	int ret;

	skb = alloc_skb(sizeof(*req), GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	adap = netdev2adap(dev);
	req = (struct cpl_pass_open_req *)__skb_put(skb, sizeof(*req));
	INIT_TP_WR(req, 0);
	OPCODE_TID(req) = htonl(MK_OPCODE_TID(CPL_PASS_OPEN_REQ, stid));
	req->local_port = sport;
	req->peer_port = htons(0);
	req->local_ip = sip;
	req->peer_ip = htonl(0);
	chan = rxq_to_chan(&adap->sge, queue);
<<<<<<< HEAD
	req->opt0 = cpu_to_be64(TX_CHAN(chan));
	req->opt1 = cpu_to_be64(CONN_POLICY_ASK |
				SYN_RSS_ENABLE | SYN_RSS_QUEUE(queue));
=======
	req->opt0 = cpu_to_be64(TX_CHAN_V(chan));
	req->opt1 = cpu_to_be64(CONN_POLICY_V(CPL_CONN_POLICY_ASK) |
				SYN_RSS_ENABLE_F | SYN_RSS_QUEUE_V(queue));
>>>>>>> v4.9.227
	ret = t4_mgmt_tx(adap, skb);
	return net_xmit_eval(ret);
}
EXPORT_SYMBOL(cxgb4_create_server);

/*	cxgb4_create_server6 - create an IPv6 server
 *	@dev: the device
 *	@stid: the server TID
 *	@sip: local IPv6 address to bind server to
 *	@sport: the server's TCP port
 *	@queue: queue to direct messages from this server to
 *
 *	Create an IPv6 server for the given port and address.
 *	Returns <0 on error and one of the %NET_XMIT_* values on success.
 */
int cxgb4_create_server6(const struct net_device *dev, unsigned int stid,
			 const struct in6_addr *sip, __be16 sport,
			 unsigned int queue)
{
	unsigned int chan;
	struct sk_buff *skb;
	struct adapter *adap;
	struct cpl_pass_open_req6 *req;
	int ret;

	skb = alloc_skb(sizeof(*req), GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	adap = netdev2adap(dev);
	req = (struct cpl_pass_open_req6 *)__skb_put(skb, sizeof(*req));
	INIT_TP_WR(req, 0);
	OPCODE_TID(req) = htonl(MK_OPCODE_TID(CPL_PASS_OPEN_REQ6, stid));
	req->local_port = sport;
	req->peer_port = htons(0);
	req->local_ip_hi = *(__be64 *)(sip->s6_addr);
	req->local_ip_lo = *(__be64 *)(sip->s6_addr + 8);
	req->peer_ip_hi = cpu_to_be64(0);
	req->peer_ip_lo = cpu_to_be64(0);
	chan = rxq_to_chan(&adap->sge, queue);
<<<<<<< HEAD
	req->opt0 = cpu_to_be64(TX_CHAN(chan));
	req->opt1 = cpu_to_be64(CONN_POLICY_ASK |
				SYN_RSS_ENABLE | SYN_RSS_QUEUE(queue));
=======
	req->opt0 = cpu_to_be64(TX_CHAN_V(chan));
	req->opt1 = cpu_to_be64(CONN_POLICY_V(CPL_CONN_POLICY_ASK) |
				SYN_RSS_ENABLE_F | SYN_RSS_QUEUE_V(queue));
>>>>>>> v4.9.227
	ret = t4_mgmt_tx(adap, skb);
	return net_xmit_eval(ret);
}
EXPORT_SYMBOL(cxgb4_create_server6);

int cxgb4_remove_server(const struct net_device *dev, unsigned int stid,
			unsigned int queue, bool ipv6)
{
	struct sk_buff *skb;
	struct adapter *adap;
	struct cpl_close_listsvr_req *req;
	int ret;

	adap = netdev2adap(dev);

	skb = alloc_skb(sizeof(*req), GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	req = (struct cpl_close_listsvr_req *)__skb_put(skb, sizeof(*req));
	INIT_TP_WR(req, 0);
	OPCODE_TID(req) = htonl(MK_OPCODE_TID(CPL_CLOSE_LISTSRV_REQ, stid));
<<<<<<< HEAD
	req->reply_ctrl = htons(NO_REPLY(0) | (ipv6 ? LISTSVR_IPV6(1) :
				LISTSVR_IPV6(0)) | QUEUENO(queue));
=======
	req->reply_ctrl = htons(NO_REPLY_V(0) | (ipv6 ? LISTSVR_IPV6_V(1) :
				LISTSVR_IPV6_V(0)) | QUEUENO_V(queue));
>>>>>>> v4.9.227
	ret = t4_mgmt_tx(adap, skb);
	return net_xmit_eval(ret);
}
EXPORT_SYMBOL(cxgb4_remove_server);

/**
 *	cxgb4_best_mtu - find the entry in the MTU table closest to an MTU
 *	@mtus: the HW MTU table
 *	@mtu: the target MTU
 *	@idx: index of selected entry in the MTU table
 *
 *	Returns the index and the value in the HW MTU table that is closest to
 *	but does not exceed @mtu, unless @mtu is smaller than any value in the
 *	table, in which case that smallest available value is selected.
 */
unsigned int cxgb4_best_mtu(const unsigned short *mtus, unsigned short mtu,
			    unsigned int *idx)
{
	unsigned int i = 0;

	while (i < NMTUS - 1 && mtus[i + 1] <= mtu)
		++i;
	if (idx)
		*idx = i;
	return mtus[i];
}
EXPORT_SYMBOL(cxgb4_best_mtu);

/**
 *     cxgb4_best_aligned_mtu - find best MTU, [hopefully] data size aligned
 *     @mtus: the HW MTU table
 *     @header_size: Header Size
 *     @data_size_max: maximum Data Segment Size
 *     @data_size_align: desired Data Segment Size Alignment (2^N)
 *     @mtu_idxp: HW MTU Table Index return value pointer (possibly NULL)
 *
 *     Similar to cxgb4_best_mtu() but instead of searching the Hardware
 *     MTU Table based solely on a Maximum MTU parameter, we break that
 *     parameter up into a Header Size and Maximum Data Segment Size, and
 *     provide a desired Data Segment Size Alignment.  If we find an MTU in
 *     the Hardware MTU Table which will result in a Data Segment Size with
 *     the requested alignment _and_ that MTU isn't "too far" from the
 *     closest MTU, then we'll return that rather than the closest MTU.
 */
unsigned int cxgb4_best_aligned_mtu(const unsigned short *mtus,
				    unsigned short header_size,
				    unsigned short data_size_max,
				    unsigned short data_size_align,
				    unsigned int *mtu_idxp)
{
	unsigned short max_mtu = header_size + data_size_max;
	unsigned short data_size_align_mask = data_size_align - 1;
	int mtu_idx, aligned_mtu_idx;

	/* Scan the MTU Table till we find an MTU which is larger than our
	 * Maximum MTU or we reach the end of the table.  Along the way,
	 * record the last MTU found, if any, which will result in a Data
	 * Segment Length matching the requested alignment.
	 */
	for (mtu_idx = 0, aligned_mtu_idx = -1; mtu_idx < NMTUS; mtu_idx++) {
		unsigned short data_size = mtus[mtu_idx] - header_size;

		/* If this MTU minus the Header Size would result in a
		 * Data Segment Size of the desired alignment, remember it.
		 */
		if ((data_size & data_size_align_mask) == 0)
			aligned_mtu_idx = mtu_idx;

		/* If we're not at the end of the Hardware MTU Table and the
		 * next element is larger than our Maximum MTU, drop out of
		 * the loop.
		 */
		if (mtu_idx+1 < NMTUS && mtus[mtu_idx+1] > max_mtu)
			break;
	}

	/* If we fell out of the loop because we ran to the end of the table,
	 * then we just have to use the last [largest] entry.
	 */
	if (mtu_idx == NMTUS)
		mtu_idx--;

	/* If we found an MTU which resulted in the requested Data Segment
	 * Length alignment and that's "not far" from the largest MTU which is
	 * less than or equal to the maximum MTU, then use that.
	 */
	if (aligned_mtu_idx >= 0 &&
	    mtu_idx - aligned_mtu_idx <= 1)
		mtu_idx = aligned_mtu_idx;

	/* If the caller has passed in an MTU Index pointer, pass the
	 * MTU Index back.  Return the MTU value.
	 */
	if (mtu_idxp)
		*mtu_idxp = mtu_idx;
	return mtus[mtu_idx];
}
EXPORT_SYMBOL(cxgb4_best_aligned_mtu);

/**
<<<<<<< HEAD
=======
 *	cxgb4_tp_smt_idx - Get the Source Mac Table index for this VI
 *	@chip: chip type
 *	@viid: VI id of the given port
 *
 *	Return the SMT index for this VI.
 */
unsigned int cxgb4_tp_smt_idx(enum chip_type chip, unsigned int viid)
{
	/* In T4/T5, SMT contains 256 SMAC entries organized in
	 * 128 rows of 2 entries each.
	 * In T6, SMT contains 256 SMAC entries in 256 rows.
	 * TODO: The below code needs to be updated when we add support
	 * for 256 VFs.
	 */
	if (CHELSIO_CHIP_VERSION(chip) <= CHELSIO_T5)
		return ((viid & 0x7f) << 1);
	else
		return (viid & 0x7f);
}
EXPORT_SYMBOL(cxgb4_tp_smt_idx);

/**
>>>>>>> v4.9.227
 *	cxgb4_port_chan - get the HW channel of a port
 *	@dev: the net device for the port
 *
 *	Return the HW Tx channel of the given port.
 */
unsigned int cxgb4_port_chan(const struct net_device *dev)
{
	return netdev2pinfo(dev)->tx_chan;
}
EXPORT_SYMBOL(cxgb4_port_chan);

unsigned int cxgb4_dbfifo_count(const struct net_device *dev, int lpfifo)
{
	struct adapter *adap = netdev2adap(dev);
	u32 v1, v2, lp_count, hp_count;

<<<<<<< HEAD
	v1 = t4_read_reg(adap, A_SGE_DBFIFO_STATUS);
	v2 = t4_read_reg(adap, SGE_DBFIFO_STATUS2);
	if (is_t4(adap->params.chip)) {
		lp_count = G_LP_COUNT(v1);
		hp_count = G_HP_COUNT(v1);
	} else {
		lp_count = G_LP_COUNT_T5(v1);
		hp_count = G_HP_COUNT_T5(v2);
=======
	v1 = t4_read_reg(adap, SGE_DBFIFO_STATUS_A);
	v2 = t4_read_reg(adap, SGE_DBFIFO_STATUS2_A);
	if (is_t4(adap->params.chip)) {
		lp_count = LP_COUNT_G(v1);
		hp_count = HP_COUNT_G(v1);
	} else {
		lp_count = LP_COUNT_T5_G(v1);
		hp_count = HP_COUNT_T5_G(v2);
>>>>>>> v4.9.227
	}
	return lpfifo ? lp_count : hp_count;
}
EXPORT_SYMBOL(cxgb4_dbfifo_count);

/**
 *	cxgb4_port_viid - get the VI id of a port
 *	@dev: the net device for the port
 *
 *	Return the VI id of the given port.
 */
unsigned int cxgb4_port_viid(const struct net_device *dev)
{
	return netdev2pinfo(dev)->viid;
}
EXPORT_SYMBOL(cxgb4_port_viid);

/**
 *	cxgb4_port_idx - get the index of a port
 *	@dev: the net device for the port
 *
 *	Return the index of the given port.
 */
unsigned int cxgb4_port_idx(const struct net_device *dev)
{
	return netdev2pinfo(dev)->port_id;
}
EXPORT_SYMBOL(cxgb4_port_idx);

void cxgb4_get_tcp_stats(struct pci_dev *pdev, struct tp_tcp_stats *v4,
			 struct tp_tcp_stats *v6)
{
	struct adapter *adap = pci_get_drvdata(pdev);

	spin_lock(&adap->stats_lock);
	t4_tp_get_tcp_stats(adap, v4, v6);
	spin_unlock(&adap->stats_lock);
}
EXPORT_SYMBOL(cxgb4_get_tcp_stats);

void cxgb4_iscsi_init(struct net_device *dev, unsigned int tag_mask,
		      const unsigned int *pgsz_order)
{
	struct adapter *adap = netdev2adap(dev);

<<<<<<< HEAD
	t4_write_reg(adap, ULP_RX_ISCSI_TAGMASK, tag_mask);
	t4_write_reg(adap, ULP_RX_ISCSI_PSZ, HPZ0(pgsz_order[0]) |
		     HPZ1(pgsz_order[1]) | HPZ2(pgsz_order[2]) |
		     HPZ3(pgsz_order[3]));
=======
	t4_write_reg(adap, ULP_RX_ISCSI_TAGMASK_A, tag_mask);
	t4_write_reg(adap, ULP_RX_ISCSI_PSZ_A, HPZ0_V(pgsz_order[0]) |
		     HPZ1_V(pgsz_order[1]) | HPZ2_V(pgsz_order[2]) |
		     HPZ3_V(pgsz_order[3]));
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(cxgb4_iscsi_init);

int cxgb4_flush_eq_cache(struct net_device *dev)
{
	struct adapter *adap = netdev2adap(dev);
<<<<<<< HEAD
	int ret;

	ret = t4_fwaddrspace_write(adap, adap->mbox,
				   0xe1000000 + A_SGE_CTXT_CMD, 0x20000000);
	return ret;
=======

	return t4_sge_ctxt_flush(adap, adap->mbox);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(cxgb4_flush_eq_cache);

static int read_eq_indices(struct adapter *adap, u16 qid, u16 *pidx, u16 *cidx)
{
<<<<<<< HEAD
	u32 addr = t4_read_reg(adap, A_SGE_DBQ_CTXT_BADDR) + 24 * qid + 8;
=======
	u32 addr = t4_read_reg(adap, SGE_DBQ_CTXT_BADDR_A) + 24 * qid + 8;
>>>>>>> v4.9.227
	__be64 indices;
	int ret;

	spin_lock(&adap->win0_lock);
	ret = t4_memory_rw(adap, 0, MEM_EDC0, addr,
			   sizeof(indices), (__be32 *)&indices,
			   T4_MEMORY_READ);
	spin_unlock(&adap->win0_lock);
	if (!ret) {
		*cidx = (be64_to_cpu(indices) >> 25) & 0xffff;
		*pidx = (be64_to_cpu(indices) >> 9) & 0xffff;
	}
	return ret;
}

int cxgb4_sync_txq_pidx(struct net_device *dev, u16 qid, u16 pidx,
			u16 size)
{
	struct adapter *adap = netdev2adap(dev);
	u16 hw_pidx, hw_cidx;
	int ret;

	ret = read_eq_indices(adap, qid, &hw_pidx, &hw_cidx);
	if (ret)
		goto out;

	if (pidx != hw_pidx) {
		u16 delta;
<<<<<<< HEAD
=======
		u32 val;
>>>>>>> v4.9.227

		if (pidx >= hw_pidx)
			delta = pidx - hw_pidx;
		else
			delta = size - hw_pidx + pidx;
<<<<<<< HEAD
		wmb();
		t4_write_reg(adap, MYPF_REG(SGE_PF_KDOORBELL),
			     QID(qid) | PIDX(delta));
=======

		if (is_t4(adap->params.chip))
			val = PIDX_V(delta);
		else
			val = PIDX_T5_V(delta);
		wmb();
		t4_write_reg(adap, MYPF_REG(SGE_PF_KDOORBELL_A),
			     QID_V(qid) | val);
>>>>>>> v4.9.227
	}
out:
	return ret;
}
EXPORT_SYMBOL(cxgb4_sync_txq_pidx);

<<<<<<< HEAD
void cxgb4_disable_db_coalescing(struct net_device *dev)
{
	struct adapter *adap;

	adap = netdev2adap(dev);
	t4_set_reg_field(adap, A_SGE_DOORBELL_CONTROL, F_NOCOALESCE,
			 F_NOCOALESCE);
}
EXPORT_SYMBOL(cxgb4_disable_db_coalescing);

void cxgb4_enable_db_coalescing(struct net_device *dev)
{
	struct adapter *adap;

	adap = netdev2adap(dev);
	t4_set_reg_field(adap, A_SGE_DOORBELL_CONTROL, F_NOCOALESCE, 0);
}
EXPORT_SYMBOL(cxgb4_enable_db_coalescing);

=======
>>>>>>> v4.9.227
int cxgb4_read_tpte(struct net_device *dev, u32 stag, __be32 *tpte)
{
	struct adapter *adap;
	u32 offset, memtype, memaddr;
<<<<<<< HEAD
	u32 edc0_size, edc1_size, mc0_size, mc1_size;
=======
	u32 edc0_size, edc1_size, mc0_size, mc1_size, size;
>>>>>>> v4.9.227
	u32 edc0_end, edc1_end, mc0_end, mc1_end;
	int ret;

	adap = netdev2adap(dev);

	offset = ((stag >> 8) * 32) + adap->vres.stag.start;

	/* Figure out where the offset lands in the Memory Type/Address scheme.
	 * This code assumes that the memory is laid out starting at offset 0
	 * with no breaks as: EDC0, EDC1, MC0, MC1. All cards have both EDC0
	 * and EDC1.  Some cards will have neither MC0 nor MC1, most cards have
	 * MC0, and some have both MC0 and MC1.
	 */
<<<<<<< HEAD
	edc0_size = EDRAM_SIZE_GET(t4_read_reg(adap, MA_EDRAM0_BAR)) << 20;
	edc1_size = EDRAM_SIZE_GET(t4_read_reg(adap, MA_EDRAM1_BAR)) << 20;
	mc0_size = EXT_MEM_SIZE_GET(t4_read_reg(adap, MA_EXT_MEMORY_BAR)) << 20;
=======
	size = t4_read_reg(adap, MA_EDRAM0_BAR_A);
	edc0_size = EDRAM0_SIZE_G(size) << 20;
	size = t4_read_reg(adap, MA_EDRAM1_BAR_A);
	edc1_size = EDRAM1_SIZE_G(size) << 20;
	size = t4_read_reg(adap, MA_EXT_MEMORY0_BAR_A);
	mc0_size = EXT_MEM0_SIZE_G(size) << 20;
>>>>>>> v4.9.227

	edc0_end = edc0_size;
	edc1_end = edc0_end + edc1_size;
	mc0_end = edc1_end + mc0_size;

	if (offset < edc0_end) {
		memtype = MEM_EDC0;
		memaddr = offset;
	} else if (offset < edc1_end) {
		memtype = MEM_EDC1;
		memaddr = offset - edc0_end;
	} else {
		if (offset < mc0_end) {
			memtype = MEM_MC0;
			memaddr = offset - edc1_end;
<<<<<<< HEAD
		} else if (is_t4(adap->params.chip)) {
			/* T4 only has a single memory channel */
			goto err;
		} else {
			mc1_size = EXT_MEM_SIZE_GET(
					t4_read_reg(adap,
						    MA_EXT_MEMORY1_BAR)) << 20;
=======
		} else if (is_t5(adap->params.chip)) {
			size = t4_read_reg(adap, MA_EXT_MEMORY1_BAR_A);
			mc1_size = EXT_MEM1_SIZE_G(size) << 20;
>>>>>>> v4.9.227
			mc1_end = mc0_end + mc1_size;
			if (offset < mc1_end) {
				memtype = MEM_MC1;
				memaddr = offset - mc0_end;
			} else {
				/* offset beyond the end of any memory */
				goto err;
			}
<<<<<<< HEAD
=======
		} else {
			/* T4/T6 only has a single memory channel */
			goto err;
>>>>>>> v4.9.227
		}
	}

	spin_lock(&adap->win0_lock);
	ret = t4_memory_rw(adap, 0, memtype, memaddr, 32, tpte, T4_MEMORY_READ);
	spin_unlock(&adap->win0_lock);
	return ret;

err:
	dev_err(adap->pdev_dev, "stag %#x, offset %#x out of range\n",
		stag, offset);
	return -EINVAL;
}
EXPORT_SYMBOL(cxgb4_read_tpte);

u64 cxgb4_read_sge_timestamp(struct net_device *dev)
{
	u32 hi, lo;
	struct adapter *adap;

	adap = netdev2adap(dev);
<<<<<<< HEAD
	lo = t4_read_reg(adap, SGE_TIMESTAMP_LO);
	hi = GET_TSVAL(t4_read_reg(adap, SGE_TIMESTAMP_HI));
=======
	lo = t4_read_reg(adap, SGE_TIMESTAMP_LO_A);
	hi = TSVAL_G(t4_read_reg(adap, SGE_TIMESTAMP_HI_A));
>>>>>>> v4.9.227

	return ((u64)hi << 32) | (u64)lo;
}
EXPORT_SYMBOL(cxgb4_read_sge_timestamp);

<<<<<<< HEAD
=======
int cxgb4_bar2_sge_qregs(struct net_device *dev,
			 unsigned int qid,
			 enum cxgb4_bar2_qtype qtype,
			 int user,
			 u64 *pbar2_qoffset,
			 unsigned int *pbar2_qid)
{
	return t4_bar2_sge_qregs(netdev2adap(dev),
				 qid,
				 (qtype == CXGB4_BAR2_QTYPE_EGRESS
				  ? T4_BAR2_QTYPE_EGRESS
				  : T4_BAR2_QTYPE_INGRESS),
				 user,
				 pbar2_qoffset,
				 pbar2_qid);
}
EXPORT_SYMBOL(cxgb4_bar2_sge_qregs);

>>>>>>> v4.9.227
static struct pci_driver cxgb4_driver;

static void check_neigh_update(struct neighbour *neigh)
{
	const struct device *parent;
	const struct net_device *netdev = neigh->dev;

	if (netdev->priv_flags & IFF_802_1Q_VLAN)
		netdev = vlan_dev_real_dev(netdev);
	parent = netdev->dev.parent;
	if (parent && parent->driver == &cxgb4_driver.driver)
		t4_l2t_update(dev_get_drvdata(parent), neigh);
}

static int netevent_cb(struct notifier_block *nb, unsigned long event,
		       void *data)
{
	switch (event) {
	case NETEVENT_NEIGH_UPDATE:
		check_neigh_update(data);
		break;
	case NETEVENT_REDIRECT:
	default:
		break;
	}
	return 0;
}

static bool netevent_registered;
static struct notifier_block cxgb4_netevent_nb = {
	.notifier_call = netevent_cb
};

static void drain_db_fifo(struct adapter *adap, int usecs)
{
	u32 v1, v2, lp_count, hp_count;

	do {
<<<<<<< HEAD
		v1 = t4_read_reg(adap, A_SGE_DBFIFO_STATUS);
		v2 = t4_read_reg(adap, SGE_DBFIFO_STATUS2);
		if (is_t4(adap->params.chip)) {
			lp_count = G_LP_COUNT(v1);
			hp_count = G_HP_COUNT(v1);
		} else {
			lp_count = G_LP_COUNT_T5(v1);
			hp_count = G_HP_COUNT_T5(v2);
=======
		v1 = t4_read_reg(adap, SGE_DBFIFO_STATUS_A);
		v2 = t4_read_reg(adap, SGE_DBFIFO_STATUS2_A);
		if (is_t4(adap->params.chip)) {
			lp_count = LP_COUNT_G(v1);
			hp_count = HP_COUNT_G(v1);
		} else {
			lp_count = LP_COUNT_T5_G(v1);
			hp_count = HP_COUNT_T5_G(v2);
>>>>>>> v4.9.227
		}

		if (lp_count == 0 && hp_count == 0)
			break;
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(usecs_to_jiffies(usecs));
	} while (1);
}

static void disable_txq_db(struct sge_txq *q)
{
	unsigned long flags;

	spin_lock_irqsave(&q->db_lock, flags);
	q->db_disabled = 1;
	spin_unlock_irqrestore(&q->db_lock, flags);
}

static void enable_txq_db(struct adapter *adap, struct sge_txq *q)
{
	spin_lock_irq(&q->db_lock);
	if (q->db_pidx_inc) {
		/* Make sure that all writes to the TX descriptors
		 * are committed before we tell HW about them.
		 */
		wmb();
<<<<<<< HEAD
		t4_write_reg(adap, MYPF_REG(SGE_PF_KDOORBELL),
			     QID(q->cntxt_id) | PIDX(q->db_pidx_inc));
=======
		t4_write_reg(adap, MYPF_REG(SGE_PF_KDOORBELL_A),
			     QID_V(q->cntxt_id) | PIDX_V(q->db_pidx_inc));
>>>>>>> v4.9.227
		q->db_pidx_inc = 0;
	}
	q->db_disabled = 0;
	spin_unlock_irq(&q->db_lock);
}

static void disable_dbs(struct adapter *adap)
{
	int i;

	for_each_ethrxq(&adap->sge, i)
		disable_txq_db(&adap->sge.ethtxq[i].q);
<<<<<<< HEAD
	for_each_ofldrxq(&adap->sge, i)
=======
	for_each_ofldtxq(&adap->sge, i)
>>>>>>> v4.9.227
		disable_txq_db(&adap->sge.ofldtxq[i].q);
	for_each_port(adap, i)
		disable_txq_db(&adap->sge.ctrlq[i].q);
}

static void enable_dbs(struct adapter *adap)
{
	int i;

	for_each_ethrxq(&adap->sge, i)
		enable_txq_db(adap, &adap->sge.ethtxq[i].q);
<<<<<<< HEAD
	for_each_ofldrxq(&adap->sge, i)
=======
	for_each_ofldtxq(&adap->sge, i)
>>>>>>> v4.9.227
		enable_txq_db(adap, &adap->sge.ofldtxq[i].q);
	for_each_port(adap, i)
		enable_txq_db(adap, &adap->sge.ctrlq[i].q);
}

static void notify_rdma_uld(struct adapter *adap, enum cxgb4_control cmd)
{
<<<<<<< HEAD
	if (adap->uld_handle[CXGB4_ULD_RDMA])
		ulds[CXGB4_ULD_RDMA].control(adap->uld_handle[CXGB4_ULD_RDMA],
				cmd);
=======
	enum cxgb4_uld type = CXGB4_ULD_RDMA;

	if (adap->uld && adap->uld[type].handle)
		adap->uld[type].control(adap->uld[type].handle, cmd);
>>>>>>> v4.9.227
}

static void process_db_full(struct work_struct *work)
{
	struct adapter *adap;

	adap = container_of(work, struct adapter, db_full_task);

	drain_db_fifo(adap, dbfifo_drain_delay);
	enable_dbs(adap);
	notify_rdma_uld(adap, CXGB4_CONTROL_DB_EMPTY);
<<<<<<< HEAD
	t4_set_reg_field(adap, SGE_INT_ENABLE3,
			 DBFIFO_HP_INT | DBFIFO_LP_INT,
			 DBFIFO_HP_INT | DBFIFO_LP_INT);
=======
	if (CHELSIO_CHIP_VERSION(adap->params.chip) <= CHELSIO_T5)
		t4_set_reg_field(adap, SGE_INT_ENABLE3_A,
				 DBFIFO_HP_INT_F | DBFIFO_LP_INT_F,
				 DBFIFO_HP_INT_F | DBFIFO_LP_INT_F);
	else
		t4_set_reg_field(adap, SGE_INT_ENABLE3_A,
				 DBFIFO_LP_INT_F, DBFIFO_LP_INT_F);
>>>>>>> v4.9.227
}

static void sync_txq_pidx(struct adapter *adap, struct sge_txq *q)
{
	u16 hw_pidx, hw_cidx;
	int ret;

	spin_lock_irq(&q->db_lock);
	ret = read_eq_indices(adap, (u16)q->cntxt_id, &hw_pidx, &hw_cidx);
	if (ret)
		goto out;
	if (q->db_pidx != hw_pidx) {
		u16 delta;
<<<<<<< HEAD
=======
		u32 val;
>>>>>>> v4.9.227

		if (q->db_pidx >= hw_pidx)
			delta = q->db_pidx - hw_pidx;
		else
			delta = q->size - hw_pidx + q->db_pidx;
<<<<<<< HEAD
		wmb();
		t4_write_reg(adap, MYPF_REG(SGE_PF_KDOORBELL),
			     QID(q->cntxt_id) | PIDX(delta));
=======

		if (is_t4(adap->params.chip))
			val = PIDX_V(delta);
		else
			val = PIDX_T5_V(delta);
		wmb();
		t4_write_reg(adap, MYPF_REG(SGE_PF_KDOORBELL_A),
			     QID_V(q->cntxt_id) | val);
>>>>>>> v4.9.227
	}
out:
	q->db_disabled = 0;
	q->db_pidx_inc = 0;
	spin_unlock_irq(&q->db_lock);
	if (ret)
		CH_WARN(adap, "DB drop recovery failed.\n");
}
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
static void recover_all_queues(struct adapter *adap)
{
	int i;

	for_each_ethrxq(&adap->sge, i)
		sync_txq_pidx(adap, &adap->sge.ethtxq[i].q);
<<<<<<< HEAD
	for_each_ofldrxq(&adap->sge, i)
=======
	for_each_ofldtxq(&adap->sge, i)
>>>>>>> v4.9.227
		sync_txq_pidx(adap, &adap->sge.ofldtxq[i].q);
	for_each_port(adap, i)
		sync_txq_pidx(adap, &adap->sge.ctrlq[i].q);
}

static void process_db_drop(struct work_struct *work)
{
	struct adapter *adap;

	adap = container_of(work, struct adapter, db_drop_task);

	if (is_t4(adap->params.chip)) {
		drain_db_fifo(adap, dbfifo_drain_delay);
		notify_rdma_uld(adap, CXGB4_CONTROL_DB_DROP);
		drain_db_fifo(adap, dbfifo_drain_delay);
		recover_all_queues(adap);
		drain_db_fifo(adap, dbfifo_drain_delay);
		enable_dbs(adap);
		notify_rdma_uld(adap, CXGB4_CONTROL_DB_EMPTY);
<<<<<<< HEAD
	} else {
		u32 dropped_db = t4_read_reg(adap, 0x010ac);
		u16 qid = (dropped_db >> 15) & 0x1ffff;
		u16 pidx_inc = dropped_db & 0x1fff;
		unsigned int s_qpp;
		unsigned short udb_density;
		unsigned long qpshift;
		int page;
		u32 udb;

		dev_warn(adap->pdev_dev,
			 "Dropped DB 0x%x qid %d bar2 %d coalesce %d pidx %d\n",
			 dropped_db, qid,
			 (dropped_db >> 14) & 1,
			 (dropped_db >> 13) & 1,
			 pidx_inc);

		drain_db_fifo(adap, 1);

		s_qpp = QUEUESPERPAGEPF1 * adap->fn;
		udb_density = 1 << QUEUESPERPAGEPF0_GET(t4_read_reg(adap,
				SGE_EGRESS_QUEUES_PER_PAGE_PF) >> s_qpp);
		qpshift = PAGE_SHIFT - ilog2(udb_density);
		udb = qid << qpshift;
		udb &= PAGE_MASK;
		page = udb / PAGE_SIZE;
		udb += (qid - (page * udb_density)) * 128;

		writel(PIDX(pidx_inc),  adap->bar2 + udb + 8);
=======
	} else if (is_t5(adap->params.chip)) {
		u32 dropped_db = t4_read_reg(adap, 0x010ac);
		u16 qid = (dropped_db >> 15) & 0x1ffff;
		u16 pidx_inc = dropped_db & 0x1fff;
		u64 bar2_qoffset;
		unsigned int bar2_qid;
		int ret;

		ret = t4_bar2_sge_qregs(adap, qid, T4_BAR2_QTYPE_EGRESS,
					0, &bar2_qoffset, &bar2_qid);
		if (ret)
			dev_err(adap->pdev_dev, "doorbell drop recovery: "
				"qid=%d, pidx_inc=%d\n", qid, pidx_inc);
		else
			writel(PIDX_T5_V(pidx_inc) | QID_V(bar2_qid),
			       adap->bar2 + bar2_qoffset + SGE_UDB_KDOORBELL);
>>>>>>> v4.9.227

		/* Re-enable BAR2 WC */
		t4_set_reg_field(adap, 0x10b0, 1<<15, 1<<15);
	}

<<<<<<< HEAD
	t4_set_reg_field(adap, A_SGE_DOORBELL_CONTROL, F_DROPPED_DB, 0);
=======
	if (CHELSIO_CHIP_VERSION(adap->params.chip) <= CHELSIO_T5)
		t4_set_reg_field(adap, SGE_DOORBELL_CONTROL_A, DROPPED_DB_F, 0);
>>>>>>> v4.9.227
}

void t4_db_full(struct adapter *adap)
{
	if (is_t4(adap->params.chip)) {
		disable_dbs(adap);
		notify_rdma_uld(adap, CXGB4_CONTROL_DB_FULL);
<<<<<<< HEAD
		t4_set_reg_field(adap, SGE_INT_ENABLE3,
				 DBFIFO_HP_INT | DBFIFO_LP_INT, 0);
=======
		t4_set_reg_field(adap, SGE_INT_ENABLE3_A,
				 DBFIFO_HP_INT_F | DBFIFO_LP_INT_F, 0);
>>>>>>> v4.9.227
		queue_work(adap->workq, &adap->db_full_task);
	}
}

void t4_db_dropped(struct adapter *adap)
{
	if (is_t4(adap->params.chip)) {
		disable_dbs(adap);
		notify_rdma_uld(adap, CXGB4_CONTROL_DB_FULL);
	}
	queue_work(adap->workq, &adap->db_drop_task);
}

<<<<<<< HEAD
static void uld_attach(struct adapter *adap, unsigned int uld)
{
	void *handle;
	struct cxgb4_lld_info lli;
	unsigned short i;

	lli.pdev = adap->pdev;
	lli.pf = adap->fn;
	lli.l2t = adap->l2t;
	lli.tids = &adap->tids;
	lli.ports = adap->port;
	lli.vr = &adap->vres;
	lli.mtus = adap->params.mtus;
	if (uld == CXGB4_ULD_RDMA) {
		lli.rxq_ids = adap->sge.rdma_rxq;
		lli.ciq_ids = adap->sge.rdma_ciq;
		lli.nrxq = adap->sge.rdmaqs;
		lli.nciq = adap->sge.rdmaciqs;
	} else if (uld == CXGB4_ULD_ISCSI) {
		lli.rxq_ids = adap->sge.ofld_rxq;
		lli.nrxq = adap->sge.ofldqsets;
	}
	lli.ntxq = adap->sge.ofldqsets;
	lli.nchan = adap->params.nports;
	lli.nports = adap->params.nports;
	lli.wr_cred = adap->params.ofldq_wr_cred;
	lli.adapter_type = adap->params.chip;
	lli.iscsi_iolen = MAXRXDATA_GET(t4_read_reg(adap, TP_PARA_REG2));
	lli.cclk_ps = 1000000000 / adap->params.vpd.cclk;
	lli.udb_density = 1 << QUEUESPERPAGEPF0_GET(
			t4_read_reg(adap, SGE_EGRESS_QUEUES_PER_PAGE_PF) >>
			(adap->fn * 4));
	lli.ucq_density = 1 << QUEUESPERPAGEPF0_GET(
			t4_read_reg(adap, SGE_INGRESS_QUEUES_PER_PAGE_PF) >>
			(adap->fn * 4));
	lli.filt_mode = adap->params.tp.vlan_pri_map;
	/* MODQ_REQ_MAP sets queues 0-3 to chan 0-3 */
	for (i = 0; i < NCHAN; i++)
		lli.tx_modq[i] = i;
	lli.gts_reg = adap->regs + MYPF_REG(SGE_PF_GTS);
	lli.db_reg = adap->regs + MYPF_REG(SGE_PF_KDOORBELL);
	lli.fw_vers = adap->params.fw_vers;
	lli.dbfifo_int_thresh = dbfifo_int_thresh;
	lli.sge_ingpadboundary = adap->sge.fl_align;
	lli.sge_egrstatuspagesize = adap->sge.stat_len;
	lli.sge_pktshift = adap->sge.pktshift;
	lli.enable_fw_ofld_conn = adap->flags & FW_OFLD_CONN;
	lli.max_ordird_qp = adap->params.max_ordird_qp;
	lli.max_ird_adapter = adap->params.max_ird_adapter;
	lli.ulptx_memwrite_dsgl = adap->params.ulptx_memwrite_dsgl;

	handle = ulds[uld].add(&lli);
	if (IS_ERR(handle)) {
		dev_warn(adap->pdev_dev,
			 "could not attach to the %s driver, error %ld\n",
			 uld_str[uld], PTR_ERR(handle));
		return;
	}

	adap->uld_handle[uld] = handle;

=======
void t4_register_netevent_notifier(void)
{
>>>>>>> v4.9.227
	if (!netevent_registered) {
		register_netevent_notifier(&cxgb4_netevent_nb);
		netevent_registered = true;
	}
<<<<<<< HEAD

	if (adap->flags & FULL_INIT_DONE)
		ulds[uld].state_change(handle, CXGB4_STATE_UP);
}

static void attach_ulds(struct adapter *adap)
{
	unsigned int i;

	spin_lock(&adap_rcu_lock);
	list_add_tail_rcu(&adap->rcu_node, &adap_rcu_list);
	spin_unlock(&adap_rcu_lock);

	mutex_lock(&uld_mutex);
	list_add_tail(&adap->list_node, &adapter_list);
	for (i = 0; i < CXGB4_ULD_MAX; i++)
		if (ulds[i].add)
			uld_attach(adap, i);
	mutex_unlock(&uld_mutex);
=======
>>>>>>> v4.9.227
}

static void detach_ulds(struct adapter *adap)
{
	unsigned int i;

	mutex_lock(&uld_mutex);
	list_del(&adap->list_node);
	for (i = 0; i < CXGB4_ULD_MAX; i++)
<<<<<<< HEAD
		if (adap->uld_handle[i]) {
			ulds[i].state_change(adap->uld_handle[i],
					     CXGB4_STATE_DETACH);
			adap->uld_handle[i] = NULL;
=======
		if (adap->uld && adap->uld[i].handle) {
			adap->uld[i].state_change(adap->uld[i].handle,
					     CXGB4_STATE_DETACH);
			adap->uld[i].handle = NULL;
>>>>>>> v4.9.227
		}
	if (netevent_registered && list_empty(&adapter_list)) {
		unregister_netevent_notifier(&cxgb4_netevent_nb);
		netevent_registered = false;
	}
	mutex_unlock(&uld_mutex);
<<<<<<< HEAD

	spin_lock(&adap_rcu_lock);
	list_del_rcu(&adap->rcu_node);
	spin_unlock(&adap_rcu_lock);
=======
>>>>>>> v4.9.227
}

static void notify_ulds(struct adapter *adap, enum cxgb4_state new_state)
{
	unsigned int i;

	mutex_lock(&uld_mutex);
	for (i = 0; i < CXGB4_ULD_MAX; i++)
<<<<<<< HEAD
		if (adap->uld_handle[i])
			ulds[i].state_change(adap->uld_handle[i], new_state);
	mutex_unlock(&uld_mutex);
}

/**
 *	cxgb4_register_uld - register an upper-layer driver
 *	@type: the ULD type
 *	@p: the ULD methods
 *
 *	Registers an upper-layer driver with this driver and notifies the ULD
 *	about any presently available devices that support its type.  Returns
 *	%-EBUSY if a ULD of the same type is already registered.
 */
int cxgb4_register_uld(enum cxgb4_uld type, const struct cxgb4_uld_info *p)
{
	int ret = 0;
	struct adapter *adap;

	if (type >= CXGB4_ULD_MAX)
		return -EINVAL;
	mutex_lock(&uld_mutex);
	if (ulds[type].add) {
		ret = -EBUSY;
		goto out;
	}
	ulds[type] = *p;
	list_for_each_entry(adap, &adapter_list, list_node)
		uld_attach(adap, type);
out:	mutex_unlock(&uld_mutex);
	return ret;
}
EXPORT_SYMBOL(cxgb4_register_uld);

/**
 *	cxgb4_unregister_uld - unregister an upper-layer driver
 *	@type: the ULD type
 *
 *	Unregisters an existing upper-layer driver.
 */
int cxgb4_unregister_uld(enum cxgb4_uld type)
{
	struct adapter *adap;

	if (type >= CXGB4_ULD_MAX)
		return -EINVAL;
	mutex_lock(&uld_mutex);
	list_for_each_entry(adap, &adapter_list, list_node)
		adap->uld_handle[type] = NULL;
	ulds[type].add = NULL;
	mutex_unlock(&uld_mutex);
	return 0;
}
EXPORT_SYMBOL(cxgb4_unregister_uld);

/* Check if netdev on which event is occured belongs to us or not. Return
 * success (true) if it belongs otherwise failure (false).
 * Called with rcu_read_lock() held.
 */
#if IS_ENABLED(CONFIG_IPV6)
static bool cxgb4_netdev(const struct net_device *netdev)
{
	struct adapter *adap;
	int i;

	list_for_each_entry_rcu(adap, &adap_rcu_list, rcu_node)
		for (i = 0; i < MAX_NPORTS; i++)
			if (adap->port[i] == netdev)
				return true;
	return false;
}

static int clip_add(struct net_device *event_dev, struct inet6_ifaddr *ifa,
		    unsigned long event)
{
	int ret = NOTIFY_DONE;

	rcu_read_lock();
	if (cxgb4_netdev(event_dev)) {
		switch (event) {
		case NETDEV_UP:
			ret = cxgb4_clip_get(event_dev,
				(const struct in6_addr *)ifa->addr.s6_addr);
			if (ret < 0) {
				rcu_read_unlock();
				return ret;
			}
			ret = NOTIFY_OK;
			break;
		case NETDEV_DOWN:
			cxgb4_clip_release(event_dev,
				(const struct in6_addr *)ifa->addr.s6_addr);
			ret = NOTIFY_OK;
=======
		if (adap->uld && adap->uld[i].handle)
			adap->uld[i].state_change(adap->uld[i].handle,
						  new_state);
	mutex_unlock(&uld_mutex);
}

#if IS_ENABLED(CONFIG_IPV6)
static int cxgb4_inet6addr_handler(struct notifier_block *this,
				   unsigned long event, void *data)
{
	struct inet6_ifaddr *ifa = data;
	struct net_device *event_dev = ifa->idev->dev;
	const struct device *parent = NULL;
#if IS_ENABLED(CONFIG_BONDING)
	struct adapter *adap;
#endif
	if (event_dev->priv_flags & IFF_802_1Q_VLAN)
		event_dev = vlan_dev_real_dev(event_dev);
#if IS_ENABLED(CONFIG_BONDING)
	if (event_dev->flags & IFF_MASTER) {
		list_for_each_entry(adap, &adapter_list, list_node) {
			switch (event) {
			case NETDEV_UP:
				cxgb4_clip_get(adap->port[0],
					       (const u32 *)ifa, 1);
				break;
			case NETDEV_DOWN:
				cxgb4_clip_release(adap->port[0],
						   (const u32 *)ifa, 1);
				break;
			default:
				break;
			}
		}
		return NOTIFY_OK;
	}
#endif

	if (event_dev)
		parent = event_dev->dev.parent;

	if (parent && parent->driver == &cxgb4_driver.driver) {
		switch (event) {
		case NETDEV_UP:
			cxgb4_clip_get(event_dev, (const u32 *)ifa, 1);
			break;
		case NETDEV_DOWN:
			cxgb4_clip_release(event_dev, (const u32 *)ifa, 1);
>>>>>>> v4.9.227
			break;
		default:
			break;
		}
	}
<<<<<<< HEAD
	rcu_read_unlock();
	return ret;
}

static int cxgb4_inet6addr_handler(struct notifier_block *this,
		unsigned long event, void *data)
{
	struct inet6_ifaddr *ifa = data;
	struct net_device *event_dev;
	int ret = NOTIFY_DONE;
	struct bonding *bond = netdev_priv(ifa->idev->dev);
	struct list_head *iter;
	struct slave *slave;
	struct pci_dev *first_pdev = NULL;

	if (ifa->idev->dev->priv_flags & IFF_802_1Q_VLAN) {
		event_dev = vlan_dev_real_dev(ifa->idev->dev);
		ret = clip_add(event_dev, ifa, event);
	} else if (ifa->idev->dev->flags & IFF_MASTER) {
		/* It is possible that two different adapters are bonded in one
		 * bond. We need to find such different adapters and add clip
		 * in all of them only once.
		 */
		bond_for_each_slave(bond, slave, iter) {
			if (!first_pdev) {
				ret = clip_add(slave->dev, ifa, event);
				/* If clip_add is success then only initialize
				 * first_pdev since it means it is our device
				 */
				if (ret == NOTIFY_OK)
					first_pdev = to_pci_dev(
							slave->dev->dev.parent);
			} else if (first_pdev !=
				   to_pci_dev(slave->dev->dev.parent))
					ret = clip_add(slave->dev, ifa, event);
		}
	} else
		ret = clip_add(ifa->idev->dev, ifa, event);

	return ret;
}

=======
	return NOTIFY_OK;
}

static bool inet6addr_registered;
>>>>>>> v4.9.227
static struct notifier_block cxgb4_inet6addr_notifier = {
	.notifier_call = cxgb4_inet6addr_handler
};

<<<<<<< HEAD
/* Retrieves IPv6 addresses from a root device (bond, vlan) associated with
 * a physical device.
 * The physical device reference is needed to send the actul CLIP command.
 */
static int update_dev_clip(struct net_device *root_dev, struct net_device *dev)
{
	struct inet6_dev *idev = NULL;
	struct inet6_ifaddr *ifa;
	int ret = 0;

	idev = __in6_dev_get(root_dev);
	if (!idev)
		return ret;

	read_lock_bh(&idev->lock);
	list_for_each_entry(ifa, &idev->addr_list, if_list) {
		ret = cxgb4_clip_get(dev,
				(const struct in6_addr *)ifa->addr.s6_addr);
		if (ret < 0)
			break;
	}
	read_unlock_bh(&idev->lock);

	return ret;
}

static int update_root_dev_clip(struct net_device *dev)
{
	struct net_device *root_dev = NULL;
	int i, ret = 0;

	/* First populate the real net device's IPv6 addresses */
	ret = update_dev_clip(dev, dev);
	if (ret)
		return ret;

	/* Parse all bond and vlan devices layered on top of the physical dev */
	root_dev = netdev_master_upper_dev_get_rcu(dev);
	if (root_dev) {
		ret = update_dev_clip(root_dev, dev);
		if (ret)
			return ret;
	}

	for (i = 0; i < VLAN_N_VID; i++) {
		root_dev = __vlan_find_dev_deep_rcu(dev, htons(ETH_P_8021Q), i);
		if (!root_dev)
			continue;

		ret = update_dev_clip(root_dev, dev);
		if (ret)
			break;
	}
	return ret;
}

=======
>>>>>>> v4.9.227
static void update_clip(const struct adapter *adap)
{
	int i;
	struct net_device *dev;
	int ret;

	rcu_read_lock();

	for (i = 0; i < MAX_NPORTS; i++) {
		dev = adap->port[i];
		ret = 0;

		if (dev)
<<<<<<< HEAD
			ret = update_root_dev_clip(dev);
=======
			ret = cxgb4_update_root_dev_clip(dev);
>>>>>>> v4.9.227

		if (ret < 0)
			break;
	}
	rcu_read_unlock();
}
#endif /* IS_ENABLED(CONFIG_IPV6) */

/**
 *	cxgb_up - enable the adapter
 *	@adap: adapter being enabled
 *
 *	Called when the first port is enabled, this function performs the
 *	actions necessary to make an adapter operational, such as completing
 *	the initialization of HW modules, and enabling interrupts.
 *
 *	Must be called with the rtnl lock held.
 */
static int cxgb_up(struct adapter *adap)
{
	int err;

<<<<<<< HEAD
	err = setup_sge_queues(adap);
	if (err)
		goto out;
=======
	mutex_lock(&uld_mutex);
	err = setup_sge_queues(adap);
	if (err)
		goto rel_lock;
>>>>>>> v4.9.227
	err = setup_rss(adap);
	if (err)
		goto freeq;

	if (adap->flags & USING_MSIX) {
		name_msix_vecs(adap);
		err = request_irq(adap->msix_info[0].vec, t4_nondata_intr, 0,
				  adap->msix_info[0].desc, adap);
		if (err)
			goto irq_err;
<<<<<<< HEAD

=======
>>>>>>> v4.9.227
		err = request_msix_queue_irqs(adap);
		if (err) {
			free_irq(adap->msix_info[0].vec, adap);
			goto irq_err;
		}
	} else {
		err = request_irq(adap->pdev->irq, t4_intr_handler(adap),
				  (adap->flags & USING_MSI) ? 0 : IRQF_SHARED,
				  adap->port[0]->name, adap);
		if (err)
			goto irq_err;
	}

<<<<<<< HEAD
	mutex_lock(&uld_mutex);
=======
>>>>>>> v4.9.227
	enable_rx(adap);
	t4_sge_start(adap);
	t4_intr_enable(adap);
	adap->flags |= FULL_INIT_DONE;
	mutex_unlock(&uld_mutex);

	notify_ulds(adap, CXGB4_STATE_UP);
#if IS_ENABLED(CONFIG_IPV6)
	update_clip(adap);
#endif
<<<<<<< HEAD
 out:
	return err;
=======
	return err;

>>>>>>> v4.9.227
 irq_err:
	dev_err(adap->pdev_dev, "request_irq failed, err %d\n", err);
 freeq:
	t4_free_sge_resources(adap);
<<<<<<< HEAD
	goto out;
=======
 rel_lock:
	mutex_unlock(&uld_mutex);
	return err;
>>>>>>> v4.9.227
}

static void cxgb_down(struct adapter *adapter)
{
<<<<<<< HEAD
	t4_intr_disable(adapter);
=======
>>>>>>> v4.9.227
	cancel_work_sync(&adapter->tid_release_task);
	cancel_work_sync(&adapter->db_full_task);
	cancel_work_sync(&adapter->db_drop_task);
	adapter->tid_release_task_busy = false;
	adapter->tid_release_head = NULL;

<<<<<<< HEAD
	if (adapter->flags & USING_MSIX) {
		free_msix_queue_irqs(adapter);
		free_irq(adapter->msix_info[0].vec, adapter);
	} else
		free_irq(adapter->pdev->irq, adapter);
	quiesce_rx(adapter);
	t4_sge_stop(adapter);
	t4_free_sge_resources(adapter);
=======
	t4_sge_stop(adapter);
	t4_free_sge_resources(adapter);

>>>>>>> v4.9.227
	adapter->flags &= ~FULL_INIT_DONE;
}

/*
 * net_device operations
 */
static int cxgb_open(struct net_device *dev)
{
	int err;
	struct port_info *pi = netdev_priv(dev);
	struct adapter *adapter = pi->adapter;

	netif_carrier_off(dev);

	if (!(adapter->flags & FULL_INIT_DONE)) {
		err = cxgb_up(adapter);
		if (err < 0)
			return err;
	}

	err = link_start(dev);
	if (!err)
		netif_tx_start_all_queues(dev);
	return err;
}

static int cxgb_close(struct net_device *dev)
{
	struct port_info *pi = netdev_priv(dev);
	struct adapter *adapter = pi->adapter;

	netif_tx_stop_all_queues(dev);
	netif_carrier_off(dev);
<<<<<<< HEAD
	return t4_enable_vi(adapter, adapter->fn, pi->viid, false, false);
}

/* Return an error number if the indicated filter isn't writable ...
 */
static int writable_filter(struct filter_entry *f)
{
	if (f->locked)
		return -EPERM;
	if (f->pending)
		return -EBUSY;

	return 0;
}

/* Delete the filter at the specified index (if valid).  The checks for all
 * the common problems with doing this like the filter being locked, currently
 * pending in another operation, etc.
 */
static int delete_filter(struct adapter *adapter, unsigned int fidx)
{
	struct filter_entry *f;
	int ret;

	if (fidx >= adapter->tids.nftids + adapter->tids.nsftids)
		return -EINVAL;

	f = &adapter->tids.ftid_tab[fidx];
	ret = writable_filter(f);
	if (ret)
		return ret;
	if (f->valid)
		return del_filter_wr(adapter, fidx);

	return 0;
=======
	return t4_enable_vi(adapter, adapter->pf, pi->viid, false, false);
>>>>>>> v4.9.227
}

int cxgb4_create_server_filter(const struct net_device *dev, unsigned int stid,
		__be32 sip, __be16 sport, __be16 vlan,
		unsigned int queue, unsigned char port, unsigned char mask)
{
	int ret;
	struct filter_entry *f;
	struct adapter *adap;
	int i;
	u8 *val;

	adap = netdev2adap(dev);

	/* Adjust stid to correct filter index */
	stid -= adap->tids.sftid_base;
	stid += adap->tids.nftids;

	/* Check to make sure the filter requested is writable ...
	 */
	f = &adap->tids.ftid_tab[stid];
	ret = writable_filter(f);
	if (ret)
		return ret;

	/* Clear out any old resources being used by the filter before
	 * we start constructing the new filter.
	 */
	if (f->valid)
		clear_filter(adap, f);

	/* Clear out filter specifications */
	memset(&f->fs, 0, sizeof(struct ch_filter_specification));
	f->fs.val.lport = cpu_to_be16(sport);
	f->fs.mask.lport  = ~0;
	val = (u8 *)&sip;
	if ((val[0] | val[1] | val[2] | val[3]) != 0) {
		for (i = 0; i < 4; i++) {
			f->fs.val.lip[i] = val[i];
			f->fs.mask.lip[i] = ~0;
		}
<<<<<<< HEAD
		if (adap->params.tp.vlan_pri_map & F_PORT) {
=======
		if (adap->params.tp.vlan_pri_map & PORT_F) {
>>>>>>> v4.9.227
			f->fs.val.iport = port;
			f->fs.mask.iport = mask;
		}
	}

<<<<<<< HEAD
	if (adap->params.tp.vlan_pri_map & F_PROTOCOL) {
=======
	if (adap->params.tp.vlan_pri_map & PROTOCOL_F) {
>>>>>>> v4.9.227
		f->fs.val.proto = IPPROTO_TCP;
		f->fs.mask.proto = ~0;
	}

	f->fs.dirsteer = 1;
	f->fs.iq = queue;
	/* Mark filter as locked */
	f->locked = 1;
	f->fs.rpttid = 1;

	ret = set_filter_wr(adap, stid);
	if (ret) {
		clear_filter(adap, f);
		return ret;
	}

	return 0;
}
EXPORT_SYMBOL(cxgb4_create_server_filter);

int cxgb4_remove_server_filter(const struct net_device *dev, unsigned int stid,
		unsigned int queue, bool ipv6)
{
<<<<<<< HEAD
	int ret;
=======
>>>>>>> v4.9.227
	struct filter_entry *f;
	struct adapter *adap;

	adap = netdev2adap(dev);

	/* Adjust stid to correct filter index */
	stid -= adap->tids.sftid_base;
	stid += adap->tids.nftids;

	f = &adap->tids.ftid_tab[stid];
	/* Unlock the filter */
	f->locked = 0;

<<<<<<< HEAD
	ret = delete_filter(adap, stid);
	if (ret)
		return ret;

	return 0;
=======
	return delete_filter(adap, stid);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL(cxgb4_remove_server_filter);

static struct rtnl_link_stats64 *cxgb_get_stats(struct net_device *dev,
						struct rtnl_link_stats64 *ns)
{
	struct port_stats stats;
	struct port_info *p = netdev_priv(dev);
	struct adapter *adapter = p->adapter;

	/* Block retrieving statistics during EEH error
	 * recovery. Otherwise, the recovery might fail
	 * and the PCI device will be removed permanently
	 */
	spin_lock(&adapter->stats_lock);
	if (!netif_device_present(dev)) {
		spin_unlock(&adapter->stats_lock);
		return ns;
	}
<<<<<<< HEAD
	t4_get_port_stats(adapter, p->tx_chan, &stats);
=======
	t4_get_port_stats_offset(adapter, p->tx_chan, &stats,
				 &p->stats_base);
>>>>>>> v4.9.227
	spin_unlock(&adapter->stats_lock);

	ns->tx_bytes   = stats.tx_octets;
	ns->tx_packets = stats.tx_frames;
	ns->rx_bytes   = stats.rx_octets;
	ns->rx_packets = stats.rx_frames;
	ns->multicast  = stats.rx_mcast_frames;

	/* detailed rx_errors */
	ns->rx_length_errors = stats.rx_jabber + stats.rx_too_long +
			       stats.rx_runt;
	ns->rx_over_errors   = 0;
	ns->rx_crc_errors    = stats.rx_fcs_err;
	ns->rx_frame_errors  = stats.rx_symbol_err;
	ns->rx_fifo_errors   = stats.rx_ovflow0 + stats.rx_ovflow1 +
			       stats.rx_ovflow2 + stats.rx_ovflow3 +
			       stats.rx_trunc0 + stats.rx_trunc1 +
			       stats.rx_trunc2 + stats.rx_trunc3;
	ns->rx_missed_errors = 0;

	/* detailed tx_errors */
	ns->tx_aborted_errors   = 0;
	ns->tx_carrier_errors   = 0;
	ns->tx_fifo_errors      = 0;
	ns->tx_heartbeat_errors = 0;
	ns->tx_window_errors    = 0;

	ns->tx_errors = stats.tx_error_frames;
	ns->rx_errors = stats.rx_symbol_err + stats.rx_fcs_err +
		ns->rx_length_errors + stats.rx_len_err + ns->rx_fifo_errors;
	return ns;
}

static int cxgb_ioctl(struct net_device *dev, struct ifreq *req, int cmd)
{
	unsigned int mbox;
	int ret = 0, prtad, devad;
	struct port_info *pi = netdev_priv(dev);
	struct mii_ioctl_data *data = (struct mii_ioctl_data *)&req->ifr_data;

	switch (cmd) {
	case SIOCGMIIPHY:
		if (pi->mdio_addr < 0)
			return -EOPNOTSUPP;
		data->phy_id = pi->mdio_addr;
		break;
	case SIOCGMIIREG:
	case SIOCSMIIREG:
		if (mdio_phy_id_is_c45(data->phy_id)) {
			prtad = mdio_phy_id_prtad(data->phy_id);
			devad = mdio_phy_id_devad(data->phy_id);
		} else if (data->phy_id < 32) {
			prtad = data->phy_id;
			devad = 0;
			data->reg_num &= 0x1f;
		} else
			return -EINVAL;

<<<<<<< HEAD
		mbox = pi->adapter->fn;
=======
		mbox = pi->adapter->pf;
>>>>>>> v4.9.227
		if (cmd == SIOCGMIIREG)
			ret = t4_mdio_rd(pi->adapter, mbox, prtad, devad,
					 data->reg_num, &data->val_out);
		else
			ret = t4_mdio_wr(pi->adapter, mbox, prtad, devad,
					 data->reg_num, data->val_in);
		break;
<<<<<<< HEAD
=======
	case SIOCGHWTSTAMP:
		return copy_to_user(req->ifr_data, &pi->tstamp_config,
				    sizeof(pi->tstamp_config)) ?
			-EFAULT : 0;
	case SIOCSHWTSTAMP:
		if (copy_from_user(&pi->tstamp_config, req->ifr_data,
				   sizeof(pi->tstamp_config)))
			return -EFAULT;

		switch (pi->tstamp_config.rx_filter) {
		case HWTSTAMP_FILTER_NONE:
			pi->rxtstamp = false;
			break;
		case HWTSTAMP_FILTER_ALL:
			pi->rxtstamp = true;
			break;
		default:
			pi->tstamp_config.rx_filter = HWTSTAMP_FILTER_NONE;
			return -ERANGE;
		}

		return copy_to_user(req->ifr_data, &pi->tstamp_config,
				    sizeof(pi->tstamp_config)) ?
			-EFAULT : 0;
>>>>>>> v4.9.227
	default:
		return -EOPNOTSUPP;
	}
	return ret;
}

static void cxgb_set_rxmode(struct net_device *dev)
{
	/* unfortunately we can't return errors to the stack */
	set_rxmode(dev, -1, false);
}

static int cxgb_change_mtu(struct net_device *dev, int new_mtu)
{
	int ret;
	struct port_info *pi = netdev_priv(dev);

	if (new_mtu < 81 || new_mtu > MAX_MTU)         /* accommodate SACK */
		return -EINVAL;
<<<<<<< HEAD
	ret = t4_set_rxmode(pi->adapter, pi->adapter->fn, pi->viid, new_mtu, -1,
=======
	ret = t4_set_rxmode(pi->adapter, pi->adapter->pf, pi->viid, new_mtu, -1,
>>>>>>> v4.9.227
			    -1, -1, -1, true);
	if (!ret)
		dev->mtu = new_mtu;
	return ret;
}

<<<<<<< HEAD
=======
#ifdef CONFIG_PCI_IOV
static int dummy_open(struct net_device *dev)
{
	/* Turn carrier off since we don't have to transmit anything on this
	 * interface.
	 */
	netif_carrier_off(dev);
	return 0;
}

/* Fill MAC address that will be assigned by the FW */
static void fill_vf_station_mac_addr(struct adapter *adap)
{
	unsigned int i;
	u8 hw_addr[ETH_ALEN], macaddr[ETH_ALEN];
	int err;
	u8 *na;
	u16 a, b;

	err = t4_get_raw_vpd_params(adap, &adap->params.vpd);
	if (!err) {
		na = adap->params.vpd.na;
		for (i = 0; i < ETH_ALEN; i++)
			hw_addr[i] = (hex2val(na[2 * i + 0]) * 16 +
				      hex2val(na[2 * i + 1]));
		a = (hw_addr[0] << 8) | hw_addr[1];
		b = (hw_addr[1] << 8) | hw_addr[2];
		a ^= b;
		a |= 0x0200;    /* locally assigned Ethernet MAC address */
		a &= ~0x0100;   /* not a multicast Ethernet MAC address */
		macaddr[0] = a >> 8;
		macaddr[1] = a & 0xff;

		for (i = 2; i < 5; i++)
			macaddr[i] = hw_addr[i + 1];

		for (i = 0; i < adap->num_vfs; i++) {
			macaddr[5] = adap->pf * 16 + i;
			ether_addr_copy(adap->vfinfo[i].vf_mac_addr, macaddr);
		}
	}
}

static int cxgb_set_vf_mac(struct net_device *dev, int vf, u8 *mac)
{
	struct port_info *pi = netdev_priv(dev);
	struct adapter *adap = pi->adapter;
	int ret;

	/* verify MAC addr is valid */
	if (!is_valid_ether_addr(mac)) {
		dev_err(pi->adapter->pdev_dev,
			"Invalid Ethernet address %pM for VF %d\n",
			mac, vf);
		return -EINVAL;
	}

	dev_info(pi->adapter->pdev_dev,
		 "Setting MAC %pM on VF %d\n", mac, vf);
	ret = t4_set_vf_mac_acl(adap, vf + 1, 1, mac);
	if (!ret)
		ether_addr_copy(adap->vfinfo[vf].vf_mac_addr, mac);
	return ret;
}

static int cxgb_get_vf_config(struct net_device *dev,
			      int vf, struct ifla_vf_info *ivi)
{
	struct port_info *pi = netdev_priv(dev);
	struct adapter *adap = pi->adapter;

	if (vf >= adap->num_vfs)
		return -EINVAL;
	ivi->vf = vf;
	ether_addr_copy(ivi->mac, adap->vfinfo[vf].vf_mac_addr);
	return 0;
}
#endif

>>>>>>> v4.9.227
static int cxgb_set_mac_addr(struct net_device *dev, void *p)
{
	int ret;
	struct sockaddr *addr = p;
	struct port_info *pi = netdev_priv(dev);

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

<<<<<<< HEAD
	ret = t4_change_mac(pi->adapter, pi->adapter->fn, pi->viid,
=======
	ret = t4_change_mac(pi->adapter, pi->adapter->pf, pi->viid,
>>>>>>> v4.9.227
			    pi->xact_addr_filt, addr->sa_data, true, true);
	if (ret < 0)
		return ret;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);
	pi->xact_addr_filt = ret;
	return 0;
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void cxgb_netpoll(struct net_device *dev)
{
	struct port_info *pi = netdev_priv(dev);
	struct adapter *adap = pi->adapter;

	if (adap->flags & USING_MSIX) {
		int i;
		struct sge_eth_rxq *rx = &adap->sge.ethrxq[pi->first_qset];

		for (i = pi->nqsets; i; i--, rx++)
			t4_sge_intr_msix(0, &rx->rspq);
	} else
		t4_intr_handler(adap)(0, adap);
}
#endif

<<<<<<< HEAD
=======
static int cxgb_set_tx_maxrate(struct net_device *dev, int index, u32 rate)
{
	struct port_info *pi = netdev_priv(dev);
	struct adapter *adap = pi->adapter;
	struct sched_class *e;
	struct ch_sched_params p;
	struct ch_sched_queue qe;
	u32 req_rate;
	int err = 0;

	if (!can_sched(dev))
		return -ENOTSUPP;

	if (index < 0 || index > pi->nqsets - 1)
		return -EINVAL;

	if (!(adap->flags & FULL_INIT_DONE)) {
		dev_err(adap->pdev_dev,
			"Failed to rate limit on queue %d. Link Down?\n",
			index);
		return -EINVAL;
	}

	/* Convert from Mbps to Kbps */
	req_rate = rate << 10;

	/* Max rate is 10 Gbps */
	if (req_rate >= SCHED_MAX_RATE_KBPS) {
		dev_err(adap->pdev_dev,
			"Invalid rate %u Mbps, Max rate is %u Gbps\n",
			rate, SCHED_MAX_RATE_KBPS);
		return -ERANGE;
	}

	/* First unbind the queue from any existing class */
	memset(&qe, 0, sizeof(qe));
	qe.queue = index;
	qe.class = SCHED_CLS_NONE;

	err = cxgb4_sched_class_unbind(dev, (void *)(&qe), SCHED_QUEUE);
	if (err) {
		dev_err(adap->pdev_dev,
			"Unbinding Queue %d on port %d fail. Err: %d\n",
			index, pi->port_id, err);
		return err;
	}

	/* Queue already unbound */
	if (!req_rate)
		return 0;

	/* Fetch any available unused or matching scheduling class */
	memset(&p, 0, sizeof(p));
	p.type = SCHED_CLASS_TYPE_PACKET;
	p.u.params.level    = SCHED_CLASS_LEVEL_CL_RL;
	p.u.params.mode     = SCHED_CLASS_MODE_CLASS;
	p.u.params.rateunit = SCHED_CLASS_RATEUNIT_BITS;
	p.u.params.ratemode = SCHED_CLASS_RATEMODE_ABS;
	p.u.params.channel  = pi->tx_chan;
	p.u.params.class    = SCHED_CLS_NONE;
	p.u.params.minrate  = 0;
	p.u.params.maxrate  = req_rate;
	p.u.params.weight   = 0;
	p.u.params.pktsize  = dev->mtu;

	e = cxgb4_sched_class_alloc(dev, &p);
	if (!e)
		return -ENOMEM;

	/* Bind the queue to a scheduling class */
	memset(&qe, 0, sizeof(qe));
	qe.queue = index;
	qe.class = e->idx;

	err = cxgb4_sched_class_bind(dev, (void *)(&qe), SCHED_QUEUE);
	if (err)
		dev_err(adap->pdev_dev,
			"Queue rate limiting failed. Err: %d\n", err);
	return err;
}

static int cxgb_setup_tc(struct net_device *dev, u32 handle, __be16 proto,
			 struct tc_to_netdev *tc)
{
	struct port_info *pi = netdev2pinfo(dev);
	struct adapter *adap = netdev2adap(dev);

	if (!(adap->flags & FULL_INIT_DONE)) {
		dev_err(adap->pdev_dev,
			"Failed to setup tc on port %d. Link Down?\n",
			pi->port_id);
		return -EINVAL;
	}

	if (TC_H_MAJ(handle) == TC_H_MAJ(TC_H_INGRESS) &&
	    tc->type == TC_SETUP_CLSU32) {
		switch (tc->cls_u32->command) {
		case TC_CLSU32_NEW_KNODE:
		case TC_CLSU32_REPLACE_KNODE:
			return cxgb4_config_knode(dev, proto, tc->cls_u32);
		case TC_CLSU32_DELETE_KNODE:
			return cxgb4_delete_knode(dev, proto, tc->cls_u32);
		default:
			return -EOPNOTSUPP;
		}
	}

	return -EOPNOTSUPP;
}

static netdev_features_t cxgb_fix_features(struct net_device *dev,
					   netdev_features_t features)
{
	/* Disable GRO, if RX_CSUM is disabled */
	if (!(features & NETIF_F_RXCSUM))
		features &= ~NETIF_F_GRO;

	return features;
}

>>>>>>> v4.9.227
static const struct net_device_ops cxgb4_netdev_ops = {
	.ndo_open             = cxgb_open,
	.ndo_stop             = cxgb_close,
	.ndo_start_xmit       = t4_eth_xmit,
	.ndo_select_queue     =	cxgb_select_queue,
	.ndo_get_stats64      = cxgb_get_stats,
	.ndo_set_rx_mode      = cxgb_set_rxmode,
	.ndo_set_mac_address  = cxgb_set_mac_addr,
	.ndo_set_features     = cxgb_set_features,
	.ndo_validate_addr    = eth_validate_addr,
	.ndo_do_ioctl         = cxgb_ioctl,
	.ndo_change_mtu       = cxgb_change_mtu,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller  = cxgb_netpoll,
#endif
<<<<<<< HEAD
=======
#ifdef CONFIG_CHELSIO_T4_FCOE
	.ndo_fcoe_enable      = cxgb_fcoe_enable,
	.ndo_fcoe_disable     = cxgb_fcoe_disable,
#endif /* CONFIG_CHELSIO_T4_FCOE */
#ifdef CONFIG_NET_RX_BUSY_POLL
	.ndo_busy_poll        = cxgb_busy_poll,
#endif
	.ndo_set_tx_maxrate   = cxgb_set_tx_maxrate,
	.ndo_setup_tc         = cxgb_setup_tc,
	.ndo_fix_features     = cxgb_fix_features,
};

#ifdef CONFIG_PCI_IOV
static const struct net_device_ops cxgb4_mgmt_netdev_ops = {
	.ndo_open             = dummy_open,
	.ndo_set_vf_mac       = cxgb_set_vf_mac,
	.ndo_get_vf_config    = cxgb_get_vf_config,
};
#endif

static void get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	struct adapter *adapter = netdev2adap(dev);

	strlcpy(info->driver, cxgb4_driver_name, sizeof(info->driver));
	strlcpy(info->version, cxgb4_driver_version,
		sizeof(info->version));
	strlcpy(info->bus_info, pci_name(adapter->pdev),
		sizeof(info->bus_info));
}

static const struct ethtool_ops cxgb4_mgmt_ethtool_ops = {
	.get_drvinfo       = get_drvinfo,
>>>>>>> v4.9.227
};

void t4_fatal_err(struct adapter *adap)
{
<<<<<<< HEAD
	t4_set_reg_field(adap, SGE_CONTROL, GLOBALENABLE, 0);
=======
	t4_set_reg_field(adap, SGE_CONTROL_A, GLOBALENABLE_F, 0);
>>>>>>> v4.9.227
	t4_intr_disable(adap);
	dev_alert(adap->pdev_dev, "encountered fatal error, adapter stopped\n");
}

<<<<<<< HEAD
/* Return the specified PCI-E Configuration Space register from our Physical
 * Function.  We try first via a Firmware LDST Command since we prefer to let
 * the firmware own all of these registers, but if that fails we go for it
 * directly ourselves.
 */
static u32 t4_read_pcie_cfg4(struct adapter *adap, int reg)
{
	struct fw_ldst_cmd ldst_cmd;
	u32 val;
	int ret;

	/* Construct and send the Firmware LDST Command to retrieve the
	 * specified PCI-E Configuration Space register.
	 */
	memset(&ldst_cmd, 0, sizeof(ldst_cmd));
	ldst_cmd.op_to_addrspace =
		htonl(FW_CMD_OP(FW_LDST_CMD) |
		      FW_CMD_REQUEST |
		      FW_CMD_READ |
		      FW_LDST_CMD_ADDRSPACE(FW_LDST_ADDRSPC_FUNC_PCIE));
	ldst_cmd.cycles_to_len16 = htonl(FW_LEN16(ldst_cmd));
	ldst_cmd.u.pcie.select_naccess = FW_LDST_CMD_NACCESS(1);
	ldst_cmd.u.pcie.ctrl_to_fn =
		(FW_LDST_CMD_LC | FW_LDST_CMD_FN(adap->fn));
	ldst_cmd.u.pcie.r = reg;
	ret = t4_wr_mbox(adap, adap->mbox, &ldst_cmd, sizeof(ldst_cmd),
			 &ldst_cmd);

	/* If the LDST Command suucceeded, exctract the returned register
	 * value.  Otherwise read it directly ourself.
	 */
	if (ret == 0)
		val = ntohl(ldst_cmd.u.pcie.data[0]);
	else
		t4_hw_pci_read_cfg4(adap, reg, &val);

	return val;
}

static void setup_memwin(struct adapter *adap)
{
	u32 mem_win0_base, mem_win1_base, mem_win2_base, mem_win2_aperture;

	if (is_t4(adap->params.chip)) {
		u32 bar0;

		/* Truncation intentional: we only read the bottom 32-bits of
		 * the 64-bit BAR0/BAR1 ...  We use the hardware backdoor
		 * mechanism to read BAR0 instead of using
		 * pci_resource_start() because we could be operating from
		 * within a Virtual Machine which is trapping our accesses to
		 * our Configuration Space and we need to set up the PCI-E
		 * Memory Window decoders with the actual addresses which will
		 * be coming across the PCI-E link.
		 */
		bar0 = t4_read_pcie_cfg4(adap, PCI_BASE_ADDRESS_0);
		bar0 &= PCI_BASE_ADDRESS_MEM_MASK;
		adap->t4_bar0 = bar0;

		mem_win0_base = bar0 + MEMWIN0_BASE;
		mem_win1_base = bar0 + MEMWIN1_BASE;
		mem_win2_base = bar0 + MEMWIN2_BASE;
		mem_win2_aperture = MEMWIN2_APERTURE;
	} else {
		/* For T5, only relative offset inside the PCIe BAR is passed */
		mem_win0_base = MEMWIN0_BASE;
		mem_win1_base = MEMWIN1_BASE;
		mem_win2_base = MEMWIN2_BASE_T5;
		mem_win2_aperture = MEMWIN2_APERTURE_T5;
	}
	t4_write_reg(adap, PCIE_MEM_ACCESS_REG(PCIE_MEM_ACCESS_BASE_WIN, 0),
		     mem_win0_base | BIR(0) |
		     WINDOW(ilog2(MEMWIN0_APERTURE) - 10));
	t4_write_reg(adap, PCIE_MEM_ACCESS_REG(PCIE_MEM_ACCESS_BASE_WIN, 1),
		     mem_win1_base | BIR(0) |
		     WINDOW(ilog2(MEMWIN1_APERTURE) - 10));
	t4_write_reg(adap, PCIE_MEM_ACCESS_REG(PCIE_MEM_ACCESS_BASE_WIN, 2),
		     mem_win2_base | BIR(0) |
		     WINDOW(ilog2(mem_win2_aperture) - 10));
	t4_read_reg(adap, PCIE_MEM_ACCESS_REG(PCIE_MEM_ACCESS_BASE_WIN, 2));
=======
static void setup_memwin(struct adapter *adap)
{
	u32 nic_win_base = t4_get_util_window(adap);

	t4_setup_memwin(adap, nic_win_base, MEMWIN_NIC);
>>>>>>> v4.9.227
}

static void setup_memwin_rdma(struct adapter *adap)
{
	if (adap->vres.ocq.size) {
		u32 start;
		unsigned int sz_kb;

		start = t4_read_pcie_cfg4(adap, PCI_BASE_ADDRESS_2);
		start &= PCI_BASE_ADDRESS_MEM_MASK;
		start += OCQ_WIN_OFFSET(adap->pdev, &adap->vres);
		sz_kb = roundup_pow_of_two(adap->vres.ocq.size) >> 10;
		t4_write_reg(adap,
<<<<<<< HEAD
			     PCIE_MEM_ACCESS_REG(PCIE_MEM_ACCESS_BASE_WIN, 3),
			     start | BIR(1) | WINDOW(ilog2(sz_kb)));
		t4_write_reg(adap,
			     PCIE_MEM_ACCESS_REG(PCIE_MEM_ACCESS_OFFSET, 3),
			     adap->vres.ocq.start);
		t4_read_reg(adap,
			    PCIE_MEM_ACCESS_REG(PCIE_MEM_ACCESS_OFFSET, 3));
=======
			     PCIE_MEM_ACCESS_REG(PCIE_MEM_ACCESS_BASE_WIN_A, 3),
			     start | BIR_V(1) | WINDOW_V(ilog2(sz_kb)));
		t4_write_reg(adap,
			     PCIE_MEM_ACCESS_REG(PCIE_MEM_ACCESS_OFFSET_A, 3),
			     adap->vres.ocq.start);
		t4_read_reg(adap,
			    PCIE_MEM_ACCESS_REG(PCIE_MEM_ACCESS_OFFSET_A, 3));
>>>>>>> v4.9.227
	}
}

static int adap_init1(struct adapter *adap, struct fw_caps_config_cmd *c)
{
	u32 v;
	int ret;

	/* get device capabilities */
	memset(c, 0, sizeof(*c));
<<<<<<< HEAD
	c->op_to_write = htonl(FW_CMD_OP(FW_CAPS_CONFIG_CMD) |
			       FW_CMD_REQUEST | FW_CMD_READ);
	c->cfvalid_to_len16 = htonl(FW_LEN16(*c));
	ret = t4_wr_mbox(adap, adap->fn, c, sizeof(*c), c);
	if (ret < 0)
		return ret;

	/* select capabilities we'll be using */
	if (c->niccaps & htons(FW_CAPS_CONFIG_NIC_VM)) {
		if (!vf_acls)
			c->niccaps ^= htons(FW_CAPS_CONFIG_NIC_VM);
		else
			c->niccaps = htons(FW_CAPS_CONFIG_NIC_VM);
	} else if (vf_acls) {
		dev_err(adap->pdev_dev, "virtualization ACLs not supported");
		return ret;
	}
	c->op_to_write = htonl(FW_CMD_OP(FW_CAPS_CONFIG_CMD) |
			       FW_CMD_REQUEST | FW_CMD_WRITE);
	ret = t4_wr_mbox(adap, adap->fn, c, sizeof(*c), NULL);
	if (ret < 0)
		return ret;

	ret = t4_config_glbl_rss(adap, adap->fn,
				 FW_RSS_GLB_CONFIG_CMD_MODE_BASICVIRTUAL,
				 FW_RSS_GLB_CONFIG_CMD_TNLMAPEN |
				 FW_RSS_GLB_CONFIG_CMD_TNLALLLKP);
	if (ret < 0)
		return ret;

	ret = t4_cfg_pfvf(adap, adap->fn, adap->fn, 0, MAX_EGRQ, 64, MAX_INGQ,
			  0, 0, 4, 0xf, 0xf, 16, FW_CMD_CAP_PF, FW_CMD_CAP_PF);
=======
	c->op_to_write = htonl(FW_CMD_OP_V(FW_CAPS_CONFIG_CMD) |
			       FW_CMD_REQUEST_F | FW_CMD_READ_F);
	c->cfvalid_to_len16 = htonl(FW_LEN16(*c));
	ret = t4_wr_mbox(adap, adap->mbox, c, sizeof(*c), c);
	if (ret < 0)
		return ret;

	c->op_to_write = htonl(FW_CMD_OP_V(FW_CAPS_CONFIG_CMD) |
			       FW_CMD_REQUEST_F | FW_CMD_WRITE_F);
	ret = t4_wr_mbox(adap, adap->mbox, c, sizeof(*c), NULL);
	if (ret < 0)
		return ret;

	ret = t4_config_glbl_rss(adap, adap->pf,
				 FW_RSS_GLB_CONFIG_CMD_MODE_BASICVIRTUAL,
				 FW_RSS_GLB_CONFIG_CMD_TNLMAPEN_F |
				 FW_RSS_GLB_CONFIG_CMD_TNLALLLKP_F);
	if (ret < 0)
		return ret;

	ret = t4_cfg_pfvf(adap, adap->mbox, adap->pf, 0, adap->sge.egr_sz, 64,
			  MAX_INGQ, 0, 0, 4, 0xf, 0xf, 16, FW_CMD_CAP_PF,
			  FW_CMD_CAP_PF);
>>>>>>> v4.9.227
	if (ret < 0)
		return ret;

	t4_sge_init(adap);

	/* tweak some settings */
<<<<<<< HEAD
	t4_write_reg(adap, TP_SHIFT_CNT, 0x64f8849);
	t4_write_reg(adap, ULP_RX_TDDP_PSZ, HPZ0(PAGE_SHIFT - 12));
	t4_write_reg(adap, TP_PIO_ADDR, TP_INGRESS_CONFIG);
	v = t4_read_reg(adap, TP_PIO_DATA);
	t4_write_reg(adap, TP_PIO_DATA, v & ~CSUM_HAS_PSEUDO_HDR);

	/* first 4 Tx modulation queues point to consecutive Tx channels */
	adap->params.tp.tx_modq_map = 0xE4;
	t4_write_reg(adap, A_TP_TX_MOD_QUEUE_REQ_MAP,
		     V_TX_MOD_QUEUE_REQ_MAP(adap->params.tp.tx_modq_map));

	/* associate each Tx modulation queue with consecutive Tx channels */
	v = 0x84218421;
	t4_write_indirect(adap, TP_PIO_ADDR, TP_PIO_DATA,
			  &v, 1, A_TP_TX_SCHED_HDR);
	t4_write_indirect(adap, TP_PIO_ADDR, TP_PIO_DATA,
			  &v, 1, A_TP_TX_SCHED_FIFO);
	t4_write_indirect(adap, TP_PIO_ADDR, TP_PIO_DATA,
			  &v, 1, A_TP_TX_SCHED_PCMD);

#define T4_TX_MODQ_10G_WEIGHT_DEFAULT 16 /* in KB units */
	if (is_offload(adap)) {
		t4_write_reg(adap, A_TP_TX_MOD_QUEUE_WEIGHT0,
			     V_TX_MODQ_WEIGHT0(T4_TX_MODQ_10G_WEIGHT_DEFAULT) |
			     V_TX_MODQ_WEIGHT1(T4_TX_MODQ_10G_WEIGHT_DEFAULT) |
			     V_TX_MODQ_WEIGHT2(T4_TX_MODQ_10G_WEIGHT_DEFAULT) |
			     V_TX_MODQ_WEIGHT3(T4_TX_MODQ_10G_WEIGHT_DEFAULT));
		t4_write_reg(adap, A_TP_TX_MOD_CHANNEL_WEIGHT,
			     V_TX_MODQ_WEIGHT0(T4_TX_MODQ_10G_WEIGHT_DEFAULT) |
			     V_TX_MODQ_WEIGHT1(T4_TX_MODQ_10G_WEIGHT_DEFAULT) |
			     V_TX_MODQ_WEIGHT2(T4_TX_MODQ_10G_WEIGHT_DEFAULT) |
			     V_TX_MODQ_WEIGHT3(T4_TX_MODQ_10G_WEIGHT_DEFAULT));
	}

	/* get basic stuff going */
	return t4_early_init(adap, adap->fn);
=======
	t4_write_reg(adap, TP_SHIFT_CNT_A, 0x64f8849);
	t4_write_reg(adap, ULP_RX_TDDP_PSZ_A, HPZ0_V(PAGE_SHIFT - 12));
	t4_write_reg(adap, TP_PIO_ADDR_A, TP_INGRESS_CONFIG_A);
	v = t4_read_reg(adap, TP_PIO_DATA_A);
	t4_write_reg(adap, TP_PIO_DATA_A, v & ~CSUM_HAS_PSEUDO_HDR_F);

	/* first 4 Tx modulation queues point to consecutive Tx channels */
	adap->params.tp.tx_modq_map = 0xE4;
	t4_write_reg(adap, TP_TX_MOD_QUEUE_REQ_MAP_A,
		     TX_MOD_QUEUE_REQ_MAP_V(adap->params.tp.tx_modq_map));

	/* associate each Tx modulation queue with consecutive Tx channels */
	v = 0x84218421;
	t4_write_indirect(adap, TP_PIO_ADDR_A, TP_PIO_DATA_A,
			  &v, 1, TP_TX_SCHED_HDR_A);
	t4_write_indirect(adap, TP_PIO_ADDR_A, TP_PIO_DATA_A,
			  &v, 1, TP_TX_SCHED_FIFO_A);
	t4_write_indirect(adap, TP_PIO_ADDR_A, TP_PIO_DATA_A,
			  &v, 1, TP_TX_SCHED_PCMD_A);

#define T4_TX_MODQ_10G_WEIGHT_DEFAULT 16 /* in KB units */
	if (is_offload(adap)) {
		t4_write_reg(adap, TP_TX_MOD_QUEUE_WEIGHT0_A,
			     TX_MODQ_WEIGHT0_V(T4_TX_MODQ_10G_WEIGHT_DEFAULT) |
			     TX_MODQ_WEIGHT1_V(T4_TX_MODQ_10G_WEIGHT_DEFAULT) |
			     TX_MODQ_WEIGHT2_V(T4_TX_MODQ_10G_WEIGHT_DEFAULT) |
			     TX_MODQ_WEIGHT3_V(T4_TX_MODQ_10G_WEIGHT_DEFAULT));
		t4_write_reg(adap, TP_TX_MOD_CHANNEL_WEIGHT_A,
			     TX_MODQ_WEIGHT0_V(T4_TX_MODQ_10G_WEIGHT_DEFAULT) |
			     TX_MODQ_WEIGHT1_V(T4_TX_MODQ_10G_WEIGHT_DEFAULT) |
			     TX_MODQ_WEIGHT2_V(T4_TX_MODQ_10G_WEIGHT_DEFAULT) |
			     TX_MODQ_WEIGHT3_V(T4_TX_MODQ_10G_WEIGHT_DEFAULT));
	}

	/* get basic stuff going */
	return t4_early_init(adap, adap->pf);
>>>>>>> v4.9.227
}

/*
 * Max # of ATIDs.  The absolute HW max is 16K but we keep it lower.
 */
#define MAX_ATIDS 8192U

/*
 * Phase 0 of initialization: contact FW, obtain config, perform basic init.
 *
 * If the firmware we're dealing with has Configuration File support, then
 * we use that to perform all configuration
 */

/*
 * Tweak configuration based on module parameters, etc.  Most of these have
 * defaults assigned to them by Firmware Configuration Files (if we're using
 * them) but need to be explicitly set if we're using hard-coded
 * initialization.  But even in the case of using Firmware Configuration
 * Files, we'd like to expose the ability to change these via module
 * parameters so these are essentially common tweaks/settings for
 * Configuration Files and hard-coded initialization ...
 */
static int adap_init0_tweaks(struct adapter *adapter)
{
	/*
	 * Fix up various Host-Dependent Parameters like Page Size, Cache
	 * Line Size, etc.  The firmware default is for a 4KB Page Size and
	 * 64B Cache Line Size ...
	 */
	t4_fixup_host_params(adapter, PAGE_SIZE, L1_CACHE_BYTES);

	/*
	 * Process module parameters which affect early initialization.
	 */
	if (rx_dma_offset != 2 && rx_dma_offset != 0) {
		dev_err(&adapter->pdev->dev,
			"Ignoring illegal rx_dma_offset=%d, using 2\n",
			rx_dma_offset);
		rx_dma_offset = 2;
	}
<<<<<<< HEAD
	t4_set_reg_field(adapter, SGE_CONTROL,
			 PKTSHIFT_MASK,
			 PKTSHIFT(rx_dma_offset));
=======
	t4_set_reg_field(adapter, SGE_CONTROL_A,
			 PKTSHIFT_V(PKTSHIFT_M),
			 PKTSHIFT_V(rx_dma_offset));
>>>>>>> v4.9.227

	/*
	 * Don't include the "IP Pseudo Header" in CPL_RX_PKT checksums: Linux
	 * adds the pseudo header itself.
	 */
<<<<<<< HEAD
	t4_tp_wr_bits_indirect(adapter, TP_INGRESS_CONFIG,
			       CSUM_HAS_PSEUDO_HDR, 0);
=======
	t4_tp_wr_bits_indirect(adapter, TP_INGRESS_CONFIG_A,
			       CSUM_HAS_PSEUDO_HDR_F, 0);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
=======
/* 10Gb/s-BT PHY Support. chip-external 10Gb/s-BT PHYs are complex chips
 * unto themselves and they contain their own firmware to perform their
 * tasks ...
 */
static int phy_aq1202_version(const u8 *phy_fw_data,
			      size_t phy_fw_size)
{
	int offset;

	/* At offset 0x8 you're looking for the primary image's
	 * starting offset which is 3 Bytes wide
	 *
	 * At offset 0xa of the primary image, you look for the offset
	 * of the DRAM segment which is 3 Bytes wide.
	 *
	 * The FW version is at offset 0x27e of the DRAM and is 2 Bytes
	 * wide
	 */
	#define be16(__p) (((__p)[0] << 8) | (__p)[1])
	#define le16(__p) ((__p)[0] | ((__p)[1] << 8))
	#define le24(__p) (le16(__p) | ((__p)[2] << 16))

	offset = le24(phy_fw_data + 0x8) << 12;
	offset = le24(phy_fw_data + offset + 0xa);
	return be16(phy_fw_data + offset + 0x27e);

	#undef be16
	#undef le16
	#undef le24
}

static struct info_10gbt_phy_fw {
	unsigned int phy_fw_id;		/* PCI Device ID */
	char *phy_fw_file;		/* /lib/firmware/ PHY Firmware file */
	int (*phy_fw_version)(const u8 *phy_fw_data, size_t phy_fw_size);
	int phy_flash;			/* Has FLASH for PHY Firmware */
} phy_info_array[] = {
	{
		PHY_AQ1202_DEVICEID,
		PHY_AQ1202_FIRMWARE,
		phy_aq1202_version,
		1,
	},
	{
		PHY_BCM84834_DEVICEID,
		PHY_BCM84834_FIRMWARE,
		NULL,
		0,
	},
	{ 0, NULL, NULL },
};

static struct info_10gbt_phy_fw *find_phy_info(int devid)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(phy_info_array); i++) {
		if (phy_info_array[i].phy_fw_id == devid)
			return &phy_info_array[i];
	}
	return NULL;
}

/* Handle updating of chip-external 10Gb/s-BT PHY firmware.  This needs to
 * happen after the FW_RESET_CMD but before the FW_INITIALIZE_CMD.  On error
 * we return a negative error number.  If we transfer new firmware we return 1
 * (from t4_load_phy_fw()).  If we don't do anything we return 0.
 */
static int adap_init0_phy(struct adapter *adap)
{
	const struct firmware *phyf;
	int ret;
	struct info_10gbt_phy_fw *phy_info;

	/* Use the device ID to determine which PHY file to flash.
	 */
	phy_info = find_phy_info(adap->pdev->device);
	if (!phy_info) {
		dev_warn(adap->pdev_dev,
			 "No PHY Firmware file found for this PHY\n");
		return -EOPNOTSUPP;
	}

	/* If we have a T4 PHY firmware file under /lib/firmware/cxgb4/, then
	 * use that. The adapter firmware provides us with a memory buffer
	 * where we can load a PHY firmware file from the host if we want to
	 * override the PHY firmware File in flash.
	 */
	ret = request_firmware_direct(&phyf, phy_info->phy_fw_file,
				      adap->pdev_dev);
	if (ret < 0) {
		/* For adapters without FLASH attached to PHY for their
		 * firmware, it's obviously a fatal error if we can't get the
		 * firmware to the adapter.  For adapters with PHY firmware
		 * FLASH storage, it's worth a warning if we can't find the
		 * PHY Firmware but we'll neuter the error ...
		 */
		dev_err(adap->pdev_dev, "unable to find PHY Firmware image "
			"/lib/firmware/%s, error %d\n",
			phy_info->phy_fw_file, -ret);
		if (phy_info->phy_flash) {
			int cur_phy_fw_ver = 0;

			t4_phy_fw_ver(adap, &cur_phy_fw_ver);
			dev_warn(adap->pdev_dev, "continuing with, on-adapter "
				 "FLASH copy, version %#x\n", cur_phy_fw_ver);
			ret = 0;
		}

		return ret;
	}

	/* Load PHY Firmware onto adapter.
	 */
	ret = t4_load_phy_fw(adap, MEMWIN_NIC, &adap->win0_lock,
			     phy_info->phy_fw_version,
			     (u8 *)phyf->data, phyf->size);
	if (ret < 0)
		dev_err(adap->pdev_dev, "PHY Firmware transfer error %d\n",
			-ret);
	else if (ret > 0) {
		int new_phy_fw_ver = 0;

		if (phy_info->phy_fw_version)
			new_phy_fw_ver = phy_info->phy_fw_version(phyf->data,
								  phyf->size);
		dev_info(adap->pdev_dev, "Successfully transferred PHY "
			 "Firmware /lib/firmware/%s, version %#x\n",
			 phy_info->phy_fw_file, new_phy_fw_ver);
	}

	release_firmware(phyf);

	return ret;
}

>>>>>>> v4.9.227
/*
 * Attempt to initialize the adapter via a Firmware Configuration File.
 */
static int adap_init0_config(struct adapter *adapter, int reset)
{
	struct fw_caps_config_cmd caps_cmd;
	const struct firmware *cf;
	unsigned long mtype = 0, maddr = 0;
	u32 finiver, finicsum, cfcsum;
	int ret;
	int config_issued = 0;
	char *fw_config_file, fw_config_file_path[256];
	char *config_name = NULL;

	/*
	 * Reset device if necessary.
	 */
	if (reset) {
		ret = t4_fw_reset(adapter, adapter->mbox,
<<<<<<< HEAD
				  PIORSTMODE | PIORST);
=======
				  PIORSTMODE_F | PIORST_F);
>>>>>>> v4.9.227
		if (ret < 0)
			goto bye;
	}

<<<<<<< HEAD
=======
	/* If this is a 10Gb/s-BT adapter make sure the chip-external
	 * 10Gb/s-BT PHYs have up-to-date firmware.  Note that this step needs
	 * to be performed after any global adapter RESET above since some
	 * PHYs only have local RAM copies of the PHY firmware.
	 */
	if (is_10gbt_device(adapter->pdev->device)) {
		ret = adap_init0_phy(adapter);
		if (ret < 0)
			goto bye;
	}
>>>>>>> v4.9.227
	/*
	 * If we have a T4 configuration file under /lib/firmware/cxgb4/,
	 * then use that.  Otherwise, use the configuration file stored
	 * in the adapter flash ...
	 */
	switch (CHELSIO_CHIP_VERSION(adapter->params.chip)) {
	case CHELSIO_T4:
		fw_config_file = FW4_CFNAME;
		break;
	case CHELSIO_T5:
		fw_config_file = FW5_CFNAME;
		break;
<<<<<<< HEAD
=======
	case CHELSIO_T6:
		fw_config_file = FW6_CFNAME;
		break;
>>>>>>> v4.9.227
	default:
		dev_err(adapter->pdev_dev, "Device %d is not supported\n",
		       adapter->pdev->device);
		ret = -EINVAL;
		goto bye;
	}

	ret = request_firmware(&cf, fw_config_file, adapter->pdev_dev);
	if (ret < 0) {
		config_name = "On FLASH";
		mtype = FW_MEMTYPE_CF_FLASH;
		maddr = t4_flash_cfg_addr(adapter);
	} else {
		u32 params[7], val[7];

		sprintf(fw_config_file_path,
			"/lib/firmware/%s", fw_config_file);
		config_name = fw_config_file_path;

		if (cf->size >= FLASH_CFG_MAX_SIZE)
			ret = -ENOMEM;
		else {
<<<<<<< HEAD
			params[0] = (FW_PARAMS_MNEM(FW_PARAMS_MNEM_DEV) |
			     FW_PARAMS_PARAM_X(FW_PARAMS_PARAM_DEV_CF));
			ret = t4_query_params(adapter, adapter->mbox,
					      adapter->fn, 0, 1, params, val);
=======
			params[0] = (FW_PARAMS_MNEM_V(FW_PARAMS_MNEM_DEV) |
			     FW_PARAMS_PARAM_X_V(FW_PARAMS_PARAM_DEV_CF));
			ret = t4_query_params(adapter, adapter->mbox,
					      adapter->pf, 0, 1, params, val);
>>>>>>> v4.9.227
			if (ret == 0) {
				/*
				 * For t4_memory_rw() below addresses and
				 * sizes have to be in terms of multiples of 4
				 * bytes.  So, if the Configuration File isn't
				 * a multiple of 4 bytes in length we'll have
				 * to write that out separately since we can't
				 * guarantee that the bytes following the
				 * residual byte in the buffer returned by
				 * request_firmware() are zeroed out ...
				 */
				size_t resid = cf->size & 0x3;
				size_t size = cf->size & ~0x3;
				__be32 *data = (__be32 *)cf->data;

<<<<<<< HEAD
				mtype = FW_PARAMS_PARAM_Y_GET(val[0]);
				maddr = FW_PARAMS_PARAM_Z_GET(val[0]) << 16;
=======
				mtype = FW_PARAMS_PARAM_Y_G(val[0]);
				maddr = FW_PARAMS_PARAM_Z_G(val[0]) << 16;
>>>>>>> v4.9.227

				spin_lock(&adapter->win0_lock);
				ret = t4_memory_rw(adapter, 0, mtype, maddr,
						   size, data, T4_MEMORY_WRITE);
				if (ret == 0 && resid != 0) {
					union {
						__be32 word;
						char buf[4];
					} last;
					int i;

					last.word = data[size >> 2];
					for (i = resid; i < 4; i++)
						last.buf[i] = 0;
					ret = t4_memory_rw(adapter, 0, mtype,
							   maddr + size,
							   4, &last.word,
							   T4_MEMORY_WRITE);
				}
				spin_unlock(&adapter->win0_lock);
			}
		}

		release_firmware(cf);
		if (ret)
			goto bye;
	}

	/*
	 * Issue a Capability Configuration command to the firmware to get it
	 * to parse the Configuration File.  We don't use t4_fw_config_file()
	 * because we want the ability to modify various features after we've
	 * processed the configuration file ...
	 */
	memset(&caps_cmd, 0, sizeof(caps_cmd));
	caps_cmd.op_to_write =
<<<<<<< HEAD
		htonl(FW_CMD_OP(FW_CAPS_CONFIG_CMD) |
		      FW_CMD_REQUEST |
		      FW_CMD_READ);
	caps_cmd.cfvalid_to_len16 =
		htonl(FW_CAPS_CONFIG_CMD_CFVALID |
		      FW_CAPS_CONFIG_CMD_MEMTYPE_CF(mtype) |
		      FW_CAPS_CONFIG_CMD_MEMADDR64K_CF(maddr >> 16) |
=======
		htonl(FW_CMD_OP_V(FW_CAPS_CONFIG_CMD) |
		      FW_CMD_REQUEST_F |
		      FW_CMD_READ_F);
	caps_cmd.cfvalid_to_len16 =
		htonl(FW_CAPS_CONFIG_CMD_CFVALID_F |
		      FW_CAPS_CONFIG_CMD_MEMTYPE_CF_V(mtype) |
		      FW_CAPS_CONFIG_CMD_MEMADDR64K_CF_V(maddr >> 16) |
>>>>>>> v4.9.227
		      FW_LEN16(caps_cmd));
	ret = t4_wr_mbox(adapter, adapter->mbox, &caps_cmd, sizeof(caps_cmd),
			 &caps_cmd);

	/* If the CAPS_CONFIG failed with an ENOENT (for a Firmware
	 * Configuration File in FLASH), our last gasp effort is to use the
	 * Firmware Configuration File which is embedded in the firmware.  A
	 * very few early versions of the firmware didn't have one embedded
	 * but we can ignore those.
	 */
	if (ret == -ENOENT) {
		memset(&caps_cmd, 0, sizeof(caps_cmd));
		caps_cmd.op_to_write =
<<<<<<< HEAD
			htonl(FW_CMD_OP(FW_CAPS_CONFIG_CMD) |
					FW_CMD_REQUEST |
					FW_CMD_READ);
=======
			htonl(FW_CMD_OP_V(FW_CAPS_CONFIG_CMD) |
					FW_CMD_REQUEST_F |
					FW_CMD_READ_F);
>>>>>>> v4.9.227
		caps_cmd.cfvalid_to_len16 = htonl(FW_LEN16(caps_cmd));
		ret = t4_wr_mbox(adapter, adapter->mbox, &caps_cmd,
				sizeof(caps_cmd), &caps_cmd);
		config_name = "Firmware Default";
	}

	config_issued = 1;
	if (ret < 0)
		goto bye;

	finiver = ntohl(caps_cmd.finiver);
	finicsum = ntohl(caps_cmd.finicsum);
	cfcsum = ntohl(caps_cmd.cfcsum);
	if (finicsum != cfcsum)
		dev_warn(adapter->pdev_dev, "Configuration File checksum "\
			 "mismatch: [fini] csum=%#x, computed csum=%#x\n",
			 finicsum, cfcsum);

	/*
	 * And now tell the firmware to use the configuration we just loaded.
	 */
	caps_cmd.op_to_write =
<<<<<<< HEAD
		htonl(FW_CMD_OP(FW_CAPS_CONFIG_CMD) |
		      FW_CMD_REQUEST |
		      FW_CMD_WRITE);
=======
		htonl(FW_CMD_OP_V(FW_CAPS_CONFIG_CMD) |
		      FW_CMD_REQUEST_F |
		      FW_CMD_WRITE_F);
>>>>>>> v4.9.227
	caps_cmd.cfvalid_to_len16 = htonl(FW_LEN16(caps_cmd));
	ret = t4_wr_mbox(adapter, adapter->mbox, &caps_cmd, sizeof(caps_cmd),
			 NULL);
	if (ret < 0)
		goto bye;

	/*
	 * Tweak configuration based on system architecture, module
	 * parameters, etc.
	 */
	ret = adap_init0_tweaks(adapter);
	if (ret < 0)
		goto bye;

	/*
	 * And finally tell the firmware to initialize itself using the
	 * parameters from the Configuration File.
	 */
	ret = t4_fw_initialize(adapter, adapter->mbox);
	if (ret < 0)
		goto bye;

<<<<<<< HEAD
	/*
	 * Return successfully and note that we're operating with parameters
	 * not supplied by the driver, rather than from hard-wired
	 * initialization constants burried in the driver.
	 */
	adapter->flags |= USING_SOFT_PARAMS;
=======
	/* Emit Firmware Configuration File information and return
	 * successfully.
	 */
>>>>>>> v4.9.227
	dev_info(adapter->pdev_dev, "Successfully configured using Firmware "\
		 "Configuration File \"%s\", version %#x, computed checksum %#x\n",
		 config_name, finiver, cfcsum);
	return 0;

	/*
	 * Something bad happened.  Return the error ...  (If the "error"
	 * is that there's no Configuration File on the adapter we don't
	 * want to issue a warning since this is fairly common.)
	 */
bye:
	if (config_issued && ret != -ENOENT)
		dev_warn(adapter->pdev_dev, "\"%s\" configuration file error %d\n",
			 config_name, -ret);
	return ret;
}

<<<<<<< HEAD
/*
 * Attempt to initialize the adapter via hard-coded, driver supplied
 * parameters ...
 */
static int adap_init0_no_config(struct adapter *adapter, int reset)
{
	struct sge *s = &adapter->sge;
	struct fw_caps_config_cmd caps_cmd;
	u32 v;
	int i, ret;

	/*
	 * Reset device if necessary
	 */
	if (reset) {
		ret = t4_fw_reset(adapter, adapter->mbox,
				  PIORSTMODE | PIORST);
		if (ret < 0)
			goto bye;
	}

	/*
	 * Get device capabilities and select which we'll be using.
	 */
	memset(&caps_cmd, 0, sizeof(caps_cmd));
	caps_cmd.op_to_write = htonl(FW_CMD_OP(FW_CAPS_CONFIG_CMD) |
				     FW_CMD_REQUEST | FW_CMD_READ);
	caps_cmd.cfvalid_to_len16 = htonl(FW_LEN16(caps_cmd));
	ret = t4_wr_mbox(adapter, adapter->mbox, &caps_cmd, sizeof(caps_cmd),
			 &caps_cmd);
	if (ret < 0)
		goto bye;

	if (caps_cmd.niccaps & htons(FW_CAPS_CONFIG_NIC_VM)) {
		if (!vf_acls)
			caps_cmd.niccaps ^= htons(FW_CAPS_CONFIG_NIC_VM);
		else
			caps_cmd.niccaps = htons(FW_CAPS_CONFIG_NIC_VM);
	} else if (vf_acls) {
		dev_err(adapter->pdev_dev, "virtualization ACLs not supported");
		goto bye;
	}
	caps_cmd.op_to_write = htonl(FW_CMD_OP(FW_CAPS_CONFIG_CMD) |
			      FW_CMD_REQUEST | FW_CMD_WRITE);
	ret = t4_wr_mbox(adapter, adapter->mbox, &caps_cmd, sizeof(caps_cmd),
			 NULL);
	if (ret < 0)
		goto bye;

	/*
	 * Tweak configuration based on system architecture, module
	 * parameters, etc.
	 */
	ret = adap_init0_tweaks(adapter);
	if (ret < 0)
		goto bye;

	/*
	 * Select RSS Global Mode we want to use.  We use "Basic Virtual"
	 * mode which maps each Virtual Interface to its own section of
	 * the RSS Table and we turn on all map and hash enables ...
	 */
	adapter->flags |= RSS_TNLALLLOOKUP;
	ret = t4_config_glbl_rss(adapter, adapter->mbox,
				 FW_RSS_GLB_CONFIG_CMD_MODE_BASICVIRTUAL,
				 FW_RSS_GLB_CONFIG_CMD_TNLMAPEN |
				 FW_RSS_GLB_CONFIG_CMD_HASHTOEPLITZ |
				 ((adapter->flags & RSS_TNLALLLOOKUP) ?
					FW_RSS_GLB_CONFIG_CMD_TNLALLLKP : 0));
	if (ret < 0)
		goto bye;

	/*
	 * Set up our own fundamental resource provisioning ...
	 */
	ret = t4_cfg_pfvf(adapter, adapter->mbox, adapter->fn, 0,
			  PFRES_NEQ, PFRES_NETHCTRL,
			  PFRES_NIQFLINT, PFRES_NIQ,
			  PFRES_TC, PFRES_NVI,
			  FW_PFVF_CMD_CMASK_MASK,
			  pfvfres_pmask(adapter, adapter->fn, 0),
			  PFRES_NEXACTF,
			  PFRES_R_CAPS, PFRES_WX_CAPS);
	if (ret < 0)
		goto bye;

	/*
	 * Perform low level SGE initialization.  We need to do this before we
	 * send the firmware the INITIALIZE command because that will cause
	 * any other PF Drivers which are waiting for the Master
	 * Initialization to proceed forward.
	 */
	for (i = 0; i < SGE_NTIMERS - 1; i++)
		s->timer_val[i] = min(intr_holdoff[i], MAX_SGE_TIMERVAL);
	s->timer_val[SGE_NTIMERS - 1] = MAX_SGE_TIMERVAL;
	s->counter_val[0] = 1;
	for (i = 1; i < SGE_NCOUNTERS; i++)
		s->counter_val[i] = min(intr_cnt[i - 1],
					THRESHOLD_0_GET(THRESHOLD_0_MASK));
	t4_sge_init(adapter);

#ifdef CONFIG_PCI_IOV
	/*
	 * Provision resource limits for Virtual Functions.  We currently
	 * grant them all the same static resource limits except for the Port
	 * Access Rights Mask which we're assigning based on the PF.  All of
	 * the static provisioning stuff for both the PF and VF really needs
	 * to be managed in a persistent manner for each device which the
	 * firmware controls.
	 */
	{
		int pf, vf;

		for (pf = 0; pf < ARRAY_SIZE(num_vf); pf++) {
			if (num_vf[pf] <= 0)
				continue;

			/* VF numbering starts at 1! */
			for (vf = 1; vf <= num_vf[pf]; vf++) {
				ret = t4_cfg_pfvf(adapter, adapter->mbox,
						  pf, vf,
						  VFRES_NEQ, VFRES_NETHCTRL,
						  VFRES_NIQFLINT, VFRES_NIQ,
						  VFRES_TC, VFRES_NVI,
						  FW_PFVF_CMD_CMASK_MASK,
						  pfvfres_pmask(
						  adapter, pf, vf),
						  VFRES_NEXACTF,
						  VFRES_R_CAPS, VFRES_WX_CAPS);
				if (ret < 0)
					dev_warn(adapter->pdev_dev,
						 "failed to "\
						 "provision pf/vf=%d/%d; "
						 "err=%d\n", pf, vf, ret);
			}
		}
	}
#endif

	/*
	 * Set up the default filter mode.  Later we'll want to implement this
	 * via a firmware command, etc. ...  This needs to be done before the
	 * firmare initialization command ...  If the selected set of fields
	 * isn't equal to the default value, we'll need to make sure that the
	 * field selections will fit in the 36-bit budget.
	 */
	if (tp_vlan_pri_map != TP_VLAN_PRI_MAP_DEFAULT) {
		int j, bits = 0;

		for (j = TP_VLAN_PRI_MAP_FIRST; j <= TP_VLAN_PRI_MAP_LAST; j++)
			switch (tp_vlan_pri_map & (1 << j)) {
			case 0:
				/* compressed filter field not enabled */
				break;
			case FCOE_MASK:
				bits +=  1;
				break;
			case PORT_MASK:
				bits +=  3;
				break;
			case VNIC_ID_MASK:
				bits += 17;
				break;
			case VLAN_MASK:
				bits += 17;
				break;
			case TOS_MASK:
				bits +=  8;
				break;
			case PROTOCOL_MASK:
				bits +=  8;
				break;
			case ETHERTYPE_MASK:
				bits += 16;
				break;
			case MACMATCH_MASK:
				bits +=  9;
				break;
			case MPSHITTYPE_MASK:
				bits +=  3;
				break;
			case FRAGMENTATION_MASK:
				bits +=  1;
				break;
			}

		if (bits > 36) {
			dev_err(adapter->pdev_dev,
				"tp_vlan_pri_map=%#x needs %d bits > 36;"\
				" using %#x\n", tp_vlan_pri_map, bits,
				TP_VLAN_PRI_MAP_DEFAULT);
			tp_vlan_pri_map = TP_VLAN_PRI_MAP_DEFAULT;
		}
	}
	v = tp_vlan_pri_map;
	t4_write_indirect(adapter, TP_PIO_ADDR, TP_PIO_DATA,
			  &v, 1, TP_VLAN_PRI_MAP);

	/*
	 * We need Five Tuple Lookup mode to be set in TP_GLOBAL_CONFIG order
	 * to support any of the compressed filter fields above.  Newer
	 * versions of the firmware do this automatically but it doesn't hurt
	 * to set it here.  Meanwhile, we do _not_ need to set Lookup Every
	 * Packet in TP_INGRESS_CONFIG to support matching non-TCP packets
	 * since the firmware automatically turns this on and off when we have
	 * a non-zero number of filters active (since it does have a
	 * performance impact).
	 */
	if (tp_vlan_pri_map)
		t4_set_reg_field(adapter, TP_GLOBAL_CONFIG,
				 FIVETUPLELOOKUP_MASK,
				 FIVETUPLELOOKUP_MASK);

	/*
	 * Tweak some settings.
	 */
	t4_write_reg(adapter, TP_SHIFT_CNT, SYNSHIFTMAX(6) |
		     RXTSHIFTMAXR1(4) | RXTSHIFTMAXR2(15) |
		     PERSHIFTBACKOFFMAX(8) | PERSHIFTMAX(8) |
		     KEEPALIVEMAXR1(4) | KEEPALIVEMAXR2(9));

	/*
	 * Get basic stuff going by issuing the Firmware Initialize command.
	 * Note that this _must_ be after all PFVF commands ...
	 */
	ret = t4_fw_initialize(adapter, adapter->mbox);
	if (ret < 0)
		goto bye;

	/*
	 * Return successfully!
	 */
	dev_info(adapter->pdev_dev, "Successfully configured using built-in "\
		 "driver parameters\n");
	return 0;

	/*
	 * Something bad happened.  Return the error ...
	 */
bye:
	return ret;
}

=======
>>>>>>> v4.9.227
static struct fw_info fw_info_array[] = {
	{
		.chip = CHELSIO_T4,
		.fs_name = FW4_CFNAME,
		.fw_mod_name = FW4_FNAME,
		.fw_hdr = {
			.chip = FW_HDR_CHIP_T4,
			.fw_ver = __cpu_to_be32(FW_VERSION(T4)),
			.intfver_nic = FW_INTFVER(T4, NIC),
			.intfver_vnic = FW_INTFVER(T4, VNIC),
			.intfver_ri = FW_INTFVER(T4, RI),
			.intfver_iscsi = FW_INTFVER(T4, ISCSI),
			.intfver_fcoe = FW_INTFVER(T4, FCOE),
		},
	}, {
		.chip = CHELSIO_T5,
		.fs_name = FW5_CFNAME,
		.fw_mod_name = FW5_FNAME,
		.fw_hdr = {
			.chip = FW_HDR_CHIP_T5,
			.fw_ver = __cpu_to_be32(FW_VERSION(T5)),
			.intfver_nic = FW_INTFVER(T5, NIC),
			.intfver_vnic = FW_INTFVER(T5, VNIC),
			.intfver_ri = FW_INTFVER(T5, RI),
			.intfver_iscsi = FW_INTFVER(T5, ISCSI),
			.intfver_fcoe = FW_INTFVER(T5, FCOE),
		},
<<<<<<< HEAD
	}
=======
	}, {
		.chip = CHELSIO_T6,
		.fs_name = FW6_CFNAME,
		.fw_mod_name = FW6_FNAME,
		.fw_hdr = {
			.chip = FW_HDR_CHIP_T6,
			.fw_ver = __cpu_to_be32(FW_VERSION(T6)),
			.intfver_nic = FW_INTFVER(T6, NIC),
			.intfver_vnic = FW_INTFVER(T6, VNIC),
			.intfver_ofld = FW_INTFVER(T6, OFLD),
			.intfver_ri = FW_INTFVER(T6, RI),
			.intfver_iscsipdu = FW_INTFVER(T6, ISCSIPDU),
			.intfver_iscsi = FW_INTFVER(T6, ISCSI),
			.intfver_fcoepdu = FW_INTFVER(T6, FCOEPDU),
			.intfver_fcoe = FW_INTFVER(T6, FCOE),
		},
	}

>>>>>>> v4.9.227
};

static struct fw_info *find_fw_info(int chip)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(fw_info_array); i++) {
		if (fw_info_array[i].chip == chip)
			return &fw_info_array[i];
	}
	return NULL;
}

/*
 * Phase 0 of initialization: contact FW, obtain config, perform basic init.
 */
static int adap_init0(struct adapter *adap)
{
	int ret;
	u32 v, port_vec;
	enum dev_state state;
	u32 params[7], val[7];
	struct fw_caps_config_cmd caps_cmd;
	int reset = 1;

<<<<<<< HEAD
	/*
	 * Contact FW, advertising Master capability (and potentially forcing
	 * ourselves as the Master PF if our module parameter force_init is
	 * set).
	 */
	ret = t4_fw_hello(adap, adap->mbox, adap->fn,
			  force_init ? MASTER_MUST : MASTER_MAY,
			  &state);
=======
	/* Grab Firmware Device Log parameters as early as possible so we have
	 * access to it for debugging, etc.
	 */
	ret = t4_init_devlog_params(adap);
	if (ret < 0)
		return ret;

	/* Contact FW, advertising Master capability */
	ret = t4_fw_hello(adap, adap->mbox, adap->mbox,
			  is_kdump_kernel() ? MASTER_MUST : MASTER_MAY, &state);
>>>>>>> v4.9.227
	if (ret < 0) {
		dev_err(adap->pdev_dev, "could not connect to FW, error %d\n",
			ret);
		return ret;
	}
	if (ret == adap->mbox)
		adap->flags |= MASTER_PF;
<<<<<<< HEAD
	if (force_init && state == DEV_STATE_INIT)
		state = DEV_STATE_UNINIT;
=======
>>>>>>> v4.9.227

	/*
	 * If we're the Master PF Driver and the device is uninitialized,
	 * then let's consider upgrading the firmware ...  (We always want
	 * to check the firmware version number in order to A. get it for
	 * later reporting and B. to warn if the currently loaded firmware
	 * is excessively mismatched relative to the driver.)
	 */
	t4_get_fw_version(adap, &adap->params.fw_vers);
<<<<<<< HEAD
	t4_get_tp_version(adap, &adap->params.tp_vers);
=======
	t4_get_bs_version(adap, &adap->params.bs_vers);
	t4_get_tp_version(adap, &adap->params.tp_vers);
	t4_get_exprom_version(adap, &adap->params.er_vers);

	ret = t4_check_fw_version(adap);
	/* If firmware is too old (not supported by driver) force an update. */
	if (ret)
		state = DEV_STATE_UNINIT;
>>>>>>> v4.9.227
	if ((adap->flags & MASTER_PF) && state != DEV_STATE_INIT) {
		struct fw_info *fw_info;
		struct fw_hdr *card_fw;
		const struct firmware *fw;
		const u8 *fw_data = NULL;
		unsigned int fw_size = 0;

		/* This is the firmware whose headers the driver was compiled
		 * against
		 */
		fw_info = find_fw_info(CHELSIO_CHIP_VERSION(adap->params.chip));
		if (fw_info == NULL) {
			dev_err(adap->pdev_dev,
				"unable to get firmware info for chip %d.\n",
				CHELSIO_CHIP_VERSION(adap->params.chip));
			return -EINVAL;
		}

		/* allocate memory to read the header of the firmware on the
		 * card
		 */
		card_fw = t4_alloc_mem(sizeof(*card_fw));

		/* Get FW from from /lib/firmware/ */
		ret = request_firmware(&fw, fw_info->fw_mod_name,
				       adap->pdev_dev);
		if (ret < 0) {
			dev_err(adap->pdev_dev,
				"unable to load firmware image %s, error %d\n",
				fw_info->fw_mod_name, ret);
		} else {
			fw_data = fw->data;
			fw_size = fw->size;
		}

		/* upgrade FW logic */
		ret = t4_prep_fw(adap, fw_info, fw_data, fw_size, card_fw,
				 state, &reset);

		/* Cleaning up */
<<<<<<< HEAD
		if (fw != NULL)
			release_firmware(fw);
=======
		release_firmware(fw);
>>>>>>> v4.9.227
		t4_free_mem(card_fw);

		if (ret < 0)
			goto bye;
	}

	/*
	 * Grab VPD parameters.  This should be done after we establish a
	 * connection to the firmware since some of the VPD parameters
	 * (notably the Core Clock frequency) are retrieved via requests to
	 * the firmware.  On the other hand, we need these fairly early on
	 * so we do this right after getting ahold of the firmware.
	 */
<<<<<<< HEAD
	ret = get_vpd_params(adap, &adap->params.vpd);
=======
	ret = t4_get_vpd_params(adap, &adap->params.vpd);
>>>>>>> v4.9.227
	if (ret < 0)
		goto bye;

	/*
	 * Find out what ports are available to us.  Note that we need to do
	 * this before calling adap_init0_no_config() since it needs nports
	 * and portvec ...
	 */
	v =
<<<<<<< HEAD
	    FW_PARAMS_MNEM(FW_PARAMS_MNEM_DEV) |
	    FW_PARAMS_PARAM_X(FW_PARAMS_PARAM_DEV_PORTVEC);
	ret = t4_query_params(adap, adap->mbox, adap->fn, 0, 1, &v, &port_vec);
=======
	    FW_PARAMS_MNEM_V(FW_PARAMS_MNEM_DEV) |
	    FW_PARAMS_PARAM_X_V(FW_PARAMS_PARAM_DEV_PORTVEC);
	ret = t4_query_params(adap, adap->mbox, adap->pf, 0, 1, &v, &port_vec);
>>>>>>> v4.9.227
	if (ret < 0)
		goto bye;

	adap->params.nports = hweight32(port_vec);
	adap->params.portvec = port_vec;

<<<<<<< HEAD
	/*
	 * If the firmware is initialized already (and we're not forcing a
	 * master initialization), note that we're living with existing
	 * adapter parameters.  Otherwise, it's time to try initializing the
	 * adapter ...
=======
	/* If the firmware is initialized already, emit a simply note to that
	 * effect. Otherwise, it's time to try initializing the adapter.
>>>>>>> v4.9.227
	 */
	if (state == DEV_STATE_INIT) {
		dev_info(adap->pdev_dev, "Coming up as %s: "\
			 "Adapter already initialized\n",
			 adap->flags & MASTER_PF ? "MASTER" : "SLAVE");
<<<<<<< HEAD
		adap->flags |= USING_SOFT_PARAMS;
=======
>>>>>>> v4.9.227
	} else {
		dev_info(adap->pdev_dev, "Coming up as MASTER: "\
			 "Initializing adapter\n");

<<<<<<< HEAD
		/*
		 * If the firmware doesn't support Configuration
		 * Files warn user and exit,
		 */
		if (ret < 0)
			dev_warn(adap->pdev_dev, "Firmware doesn't support "
				 "configuration file.\n");
		if (force_old_init)
			ret = adap_init0_no_config(adap, reset);
		else {
			/*
			 * Find out whether we're dealing with a version of
			 * the firmware which has configuration file support.
			 */
			params[0] = (FW_PARAMS_MNEM(FW_PARAMS_MNEM_DEV) |
				     FW_PARAMS_PARAM_X(FW_PARAMS_PARAM_DEV_CF));
			ret = t4_query_params(adap, adap->mbox, adap->fn, 0, 1,
					      params, val);

			/*
			 * If the firmware doesn't support Configuration
			 * Files, use the old Driver-based, hard-wired
			 * initialization.  Otherwise, try using the
			 * Configuration File support and fall back to the
			 * Driver-based initialization if there's no
			 * Configuration File found.
			 */
			if (ret < 0)
				ret = adap_init0_no_config(adap, reset);
			else {
				/*
				 * The firmware provides us with a memory
				 * buffer where we can load a Configuration
				 * File from the host if we want to override
				 * the Configuration File in flash.
				 */

				ret = adap_init0_config(adap, reset);
				if (ret == -ENOENT) {
					dev_info(adap->pdev_dev,
					    "No Configuration File present "
					    "on adapter. Using hard-wired "
					    "configuration parameters.\n");
					ret = adap_init0_no_config(adap, reset);
				}
			}
		}
		if (ret < 0) {
			dev_err(adap->pdev_dev,
				"could not initialize adapter, error %d\n",
				-ret);
=======
		/* Find out whether we're dealing with a version of the
		 * firmware which has configuration file support.
		 */
		params[0] = (FW_PARAMS_MNEM_V(FW_PARAMS_MNEM_DEV) |
			     FW_PARAMS_PARAM_X_V(FW_PARAMS_PARAM_DEV_CF));
		ret = t4_query_params(adap, adap->mbox, adap->pf, 0, 1,
				      params, val);

		/* If the firmware doesn't support Configuration Files,
		 * return an error.
		 */
		if (ret < 0) {
			dev_err(adap->pdev_dev, "firmware doesn't support "
				"Firmware Configuration Files\n");
			goto bye;
		}

		/* The firmware provides us with a memory buffer where we can
		 * load a Configuration File from the host if we want to
		 * override the Configuration File in flash.
		 */
		ret = adap_init0_config(adap, reset);
		if (ret == -ENOENT) {
			dev_err(adap->pdev_dev, "no Configuration File "
				"present on adapter.\n");
			goto bye;
		}
		if (ret < 0) {
			dev_err(adap->pdev_dev, "could not initialize "
				"adapter, error %d\n", -ret);
>>>>>>> v4.9.227
			goto bye;
		}
	}

<<<<<<< HEAD
	/*
	 * If we're living with non-hard-coded parameters (either from a
	 * Firmware Configuration File or values programmed by a different PF
	 * Driver), give the SGE code a chance to pull in anything that it
	 * needs ...  Note that this must be called after we retrieve our VPD
	 * parameters in order to know how to convert core ticks to seconds.
	 */
	if (adap->flags & USING_SOFT_PARAMS) {
		ret = t4_sge_init(adap);
		if (ret < 0)
			goto bye;
	}
=======
	/* Give the SGE code a chance to pull in anything that it needs ...
	 * Note that this must be called after we retrieve our VPD parameters
	 * in order to know how to convert core ticks to seconds, etc.
	 */
	ret = t4_sge_init(adap);
	if (ret < 0)
		goto bye;
>>>>>>> v4.9.227

	if (is_bypass_device(adap->pdev->device))
		adap->params.bypass = 1;

	/*
	 * Grab some of our basic fundamental operating parameters.
	 */
#define FW_PARAM_DEV(param) \
<<<<<<< HEAD
	(FW_PARAMS_MNEM(FW_PARAMS_MNEM_DEV) | \
	FW_PARAMS_PARAM_X(FW_PARAMS_PARAM_DEV_##param))

#define FW_PARAM_PFVF(param) \
	FW_PARAMS_MNEM(FW_PARAMS_MNEM_PFVF) | \
	FW_PARAMS_PARAM_X(FW_PARAMS_PARAM_PFVF_##param)|  \
	FW_PARAMS_PARAM_Y(0) | \
	FW_PARAMS_PARAM_Z(0)
=======
	(FW_PARAMS_MNEM_V(FW_PARAMS_MNEM_DEV) | \
	FW_PARAMS_PARAM_X_V(FW_PARAMS_PARAM_DEV_##param))

#define FW_PARAM_PFVF(param) \
	FW_PARAMS_MNEM_V(FW_PARAMS_MNEM_PFVF) | \
	FW_PARAMS_PARAM_X_V(FW_PARAMS_PARAM_PFVF_##param)|  \
	FW_PARAMS_PARAM_Y_V(0) | \
	FW_PARAMS_PARAM_Z_V(0)
>>>>>>> v4.9.227

	params[0] = FW_PARAM_PFVF(EQ_START);
	params[1] = FW_PARAM_PFVF(L2T_START);
	params[2] = FW_PARAM_PFVF(L2T_END);
	params[3] = FW_PARAM_PFVF(FILTER_START);
	params[4] = FW_PARAM_PFVF(FILTER_END);
	params[5] = FW_PARAM_PFVF(IQFLINT_START);
<<<<<<< HEAD
	ret = t4_query_params(adap, adap->mbox, adap->fn, 0, 6, params, val);
=======
	ret = t4_query_params(adap, adap->mbox, adap->pf, 0, 6, params, val);
>>>>>>> v4.9.227
	if (ret < 0)
		goto bye;
	adap->sge.egr_start = val[0];
	adap->l2t_start = val[1];
	adap->l2t_end = val[2];
	adap->tids.ftid_base = val[3];
	adap->tids.nftids = val[4] - val[3] + 1;
	adap->sge.ingr_start = val[5];

<<<<<<< HEAD
	/* query params related to active filter region */
	params[0] = FW_PARAM_PFVF(ACTIVE_FILTER_START);
	params[1] = FW_PARAM_PFVF(ACTIVE_FILTER_END);
	ret = t4_query_params(adap, adap->mbox, adap->fn, 0, 2, params, val);
=======
	/* qids (ingress/egress) returned from firmware can be anywhere
	 * in the range from EQ(IQFLINT)_START to EQ(IQFLINT)_END.
	 * Hence driver needs to allocate memory for this range to
	 * store the queue info. Get the highest IQFLINT/EQ index returned
	 * in FW_EQ_*_CMD.alloc command.
	 */
	params[0] = FW_PARAM_PFVF(EQ_END);
	params[1] = FW_PARAM_PFVF(IQFLINT_END);
	ret = t4_query_params(adap, adap->mbox, adap->pf, 0, 2, params, val);
	if (ret < 0)
		goto bye;
	adap->sge.egr_sz = val[0] - adap->sge.egr_start + 1;
	adap->sge.ingr_sz = val[1] - adap->sge.ingr_start + 1;

	adap->sge.egr_map = kcalloc(adap->sge.egr_sz,
				    sizeof(*adap->sge.egr_map), GFP_KERNEL);
	if (!adap->sge.egr_map) {
		ret = -ENOMEM;
		goto bye;
	}

	adap->sge.ingr_map = kcalloc(adap->sge.ingr_sz,
				     sizeof(*adap->sge.ingr_map), GFP_KERNEL);
	if (!adap->sge.ingr_map) {
		ret = -ENOMEM;
		goto bye;
	}

	/* Allocate the memory for the vaious egress queue bitmaps
	 * ie starving_fl, txq_maperr and blocked_fl.
	 */
	adap->sge.starving_fl =	kcalloc(BITS_TO_LONGS(adap->sge.egr_sz),
					sizeof(long), GFP_KERNEL);
	if (!adap->sge.starving_fl) {
		ret = -ENOMEM;
		goto bye;
	}

	adap->sge.txq_maperr = kcalloc(BITS_TO_LONGS(adap->sge.egr_sz),
				       sizeof(long), GFP_KERNEL);
	if (!adap->sge.txq_maperr) {
		ret = -ENOMEM;
		goto bye;
	}

#ifdef CONFIG_DEBUG_FS
	adap->sge.blocked_fl = kcalloc(BITS_TO_LONGS(adap->sge.egr_sz),
				       sizeof(long), GFP_KERNEL);
	if (!adap->sge.blocked_fl) {
		ret = -ENOMEM;
		goto bye;
	}
#endif

	params[0] = FW_PARAM_PFVF(CLIP_START);
	params[1] = FW_PARAM_PFVF(CLIP_END);
	ret = t4_query_params(adap, adap->mbox, adap->pf, 0, 2, params, val);
	if (ret < 0)
		goto bye;
	adap->clipt_start = val[0];
	adap->clipt_end = val[1];

	/* We don't yet have a PARAMs calls to retrieve the number of Traffic
	 * Classes supported by the hardware/firmware so we hard code it here
	 * for now.
	 */
	adap->params.nsched_cls = is_t4(adap->params.chip) ? 15 : 16;

	/* query params related to active filter region */
	params[0] = FW_PARAM_PFVF(ACTIVE_FILTER_START);
	params[1] = FW_PARAM_PFVF(ACTIVE_FILTER_END);
	ret = t4_query_params(adap, adap->mbox, adap->pf, 0, 2, params, val);
>>>>>>> v4.9.227
	/* If Active filter size is set we enable establishing
	 * offload connection through firmware work request
	 */
	if ((val[0] != val[1]) && (ret >= 0)) {
		adap->flags |= FW_OFLD_CONN;
		adap->tids.aftid_base = val[0];
		adap->tids.aftid_end = val[1];
	}

	/* If we're running on newer firmware, let it know that we're
	 * prepared to deal with encapsulated CPL messages.  Older
	 * firmware won't understand this and we'll just get
	 * unencapsulated messages ...
	 */
	params[0] = FW_PARAM_PFVF(CPLFW4MSG_ENCAP);
	val[0] = 1;
<<<<<<< HEAD
	(void) t4_set_params(adap, adap->mbox, adap->fn, 0, 1, params, val);
=======
	(void)t4_set_params(adap, adap->mbox, adap->pf, 0, 1, params, val);
>>>>>>> v4.9.227

	/*
	 * Find out whether we're allowed to use the T5+ ULPTX MEMWRITE DSGL
	 * capability.  Earlier versions of the firmware didn't have the
	 * ULPTX_MEMWRITE_DSGL so we'll interpret a query failure as no
	 * permission to use ULPTX MEMWRITE DSGL.
	 */
	if (is_t4(adap->params.chip)) {
		adap->params.ulptx_memwrite_dsgl = false;
	} else {
		params[0] = FW_PARAM_DEV(ULPTX_MEMWRITE_DSGL);
<<<<<<< HEAD
		ret = t4_query_params(adap, adap->mbox, adap->fn, 0,
=======
		ret = t4_query_params(adap, adap->mbox, adap->pf, 0,
>>>>>>> v4.9.227
				      1, params, val);
		adap->params.ulptx_memwrite_dsgl = (ret == 0 && val[0] != 0);
	}

<<<<<<< HEAD
=======
	/* See if FW supports FW_RI_FR_NSMR_TPTE_WR work request */
	params[0] = FW_PARAM_DEV(RI_FR_NSMR_TPTE_WR);
	ret = t4_query_params(adap, adap->mbox, adap->pf, 0,
			      1, params, val);
	adap->params.fr_nsmr_tpte_wr_support = (ret == 0 && val[0] != 0);

>>>>>>> v4.9.227
	/*
	 * Get device capabilities so we can determine what resources we need
	 * to manage.
	 */
	memset(&caps_cmd, 0, sizeof(caps_cmd));
<<<<<<< HEAD
	caps_cmd.op_to_write = htonl(FW_CMD_OP(FW_CAPS_CONFIG_CMD) |
				     FW_CMD_REQUEST | FW_CMD_READ);
=======
	caps_cmd.op_to_write = htonl(FW_CMD_OP_V(FW_CAPS_CONFIG_CMD) |
				     FW_CMD_REQUEST_F | FW_CMD_READ_F);
>>>>>>> v4.9.227
	caps_cmd.cfvalid_to_len16 = htonl(FW_LEN16(caps_cmd));
	ret = t4_wr_mbox(adap, adap->mbox, &caps_cmd, sizeof(caps_cmd),
			 &caps_cmd);
	if (ret < 0)
		goto bye;

	if (caps_cmd.ofldcaps) {
		/* query offload-related parameters */
		params[0] = FW_PARAM_DEV(NTID);
		params[1] = FW_PARAM_PFVF(SERVER_START);
		params[2] = FW_PARAM_PFVF(SERVER_END);
		params[3] = FW_PARAM_PFVF(TDDP_START);
		params[4] = FW_PARAM_PFVF(TDDP_END);
		params[5] = FW_PARAM_DEV(FLOWC_BUFFIFO_SZ);
<<<<<<< HEAD
		ret = t4_query_params(adap, adap->mbox, adap->fn, 0, 6,
=======
		ret = t4_query_params(adap, adap->mbox, adap->pf, 0, 6,
>>>>>>> v4.9.227
				      params, val);
		if (ret < 0)
			goto bye;
		adap->tids.ntids = val[0];
		adap->tids.natids = min(adap->tids.ntids / 2, MAX_ATIDS);
		adap->tids.stid_base = val[1];
		adap->tids.nstids = val[2] - val[1] + 1;
		/*
<<<<<<< HEAD
		 * Setup server filter region. Divide the availble filter
=======
		 * Setup server filter region. Divide the available filter
>>>>>>> v4.9.227
		 * region into two parts. Regular filters get 1/3rd and server
		 * filters get 2/3rd part. This is only enabled if workarond
		 * path is enabled.
		 * 1. For regular filters.
		 * 2. Server filter: This are special filters which are used
		 * to redirect SYN packets to offload queue.
		 */
		if (adap->flags & FW_OFLD_CONN && !is_bypass(adap)) {
			adap->tids.sftid_base = adap->tids.ftid_base +
					DIV_ROUND_UP(adap->tids.nftids, 3);
			adap->tids.nsftids = adap->tids.nftids -
					 DIV_ROUND_UP(adap->tids.nftids, 3);
			adap->tids.nftids = adap->tids.sftid_base -
						adap->tids.ftid_base;
		}
		adap->vres.ddp.start = val[3];
		adap->vres.ddp.size = val[4] - val[3] + 1;
		adap->params.ofldq_wr_cred = val[5];

		adap->params.offload = 1;
<<<<<<< HEAD
=======
		adap->num_ofld_uld += 1;
>>>>>>> v4.9.227
	}
	if (caps_cmd.rdmacaps) {
		params[0] = FW_PARAM_PFVF(STAG_START);
		params[1] = FW_PARAM_PFVF(STAG_END);
		params[2] = FW_PARAM_PFVF(RQ_START);
		params[3] = FW_PARAM_PFVF(RQ_END);
		params[4] = FW_PARAM_PFVF(PBL_START);
		params[5] = FW_PARAM_PFVF(PBL_END);
<<<<<<< HEAD
		ret = t4_query_params(adap, adap->mbox, adap->fn, 0, 6,
=======
		ret = t4_query_params(adap, adap->mbox, adap->pf, 0, 6,
>>>>>>> v4.9.227
				      params, val);
		if (ret < 0)
			goto bye;
		adap->vres.stag.start = val[0];
		adap->vres.stag.size = val[1] - val[0] + 1;
		adap->vres.rq.start = val[2];
		adap->vres.rq.size = val[3] - val[2] + 1;
		adap->vres.pbl.start = val[4];
		adap->vres.pbl.size = val[5] - val[4] + 1;

		params[0] = FW_PARAM_PFVF(SQRQ_START);
		params[1] = FW_PARAM_PFVF(SQRQ_END);
		params[2] = FW_PARAM_PFVF(CQ_START);
		params[3] = FW_PARAM_PFVF(CQ_END);
		params[4] = FW_PARAM_PFVF(OCQ_START);
		params[5] = FW_PARAM_PFVF(OCQ_END);
<<<<<<< HEAD
		ret = t4_query_params(adap, adap->mbox, adap->fn, 0, 6, params,
=======
		ret = t4_query_params(adap, adap->mbox, adap->pf, 0, 6, params,
>>>>>>> v4.9.227
				      val);
		if (ret < 0)
			goto bye;
		adap->vres.qp.start = val[0];
		adap->vres.qp.size = val[1] - val[0] + 1;
		adap->vres.cq.start = val[2];
		adap->vres.cq.size = val[3] - val[2] + 1;
		adap->vres.ocq.start = val[4];
		adap->vres.ocq.size = val[5] - val[4] + 1;

		params[0] = FW_PARAM_DEV(MAXORDIRD_QP);
		params[1] = FW_PARAM_DEV(MAXIRD_ADAPTER);
<<<<<<< HEAD
		ret = t4_query_params(adap, adap->mbox, adap->fn, 0, 2, params,
=======
		ret = t4_query_params(adap, adap->mbox, adap->pf, 0, 2, params,
>>>>>>> v4.9.227
				      val);
		if (ret < 0) {
			adap->params.max_ordird_qp = 8;
			adap->params.max_ird_adapter = 32 * adap->tids.ntids;
			ret = 0;
		} else {
			adap->params.max_ordird_qp = val[0];
			adap->params.max_ird_adapter = val[1];
		}
		dev_info(adap->pdev_dev,
			 "max_ordird_qp %d max_ird_adapter %d\n",
			 adap->params.max_ordird_qp,
			 adap->params.max_ird_adapter);
<<<<<<< HEAD
=======
		adap->num_ofld_uld += 2;
>>>>>>> v4.9.227
	}
	if (caps_cmd.iscsicaps) {
		params[0] = FW_PARAM_PFVF(ISCSI_START);
		params[1] = FW_PARAM_PFVF(ISCSI_END);
<<<<<<< HEAD
		ret = t4_query_params(adap, adap->mbox, adap->fn, 0, 2,
=======
		ret = t4_query_params(adap, adap->mbox, adap->pf, 0, 2,
>>>>>>> v4.9.227
				      params, val);
		if (ret < 0)
			goto bye;
		adap->vres.iscsi.start = val[0];
		adap->vres.iscsi.size = val[1] - val[0] + 1;
<<<<<<< HEAD
=======
		/* LIO target and cxgb4i initiaitor */
		adap->num_ofld_uld += 2;
	}
	if (caps_cmd.cryptocaps) {
		/* Should query params here...TODO */
		adap->params.crypto |= ULP_CRYPTO_LOOKASIDE;
		adap->num_uld += 1;
>>>>>>> v4.9.227
	}
#undef FW_PARAM_PFVF
#undef FW_PARAM_DEV

	/* The MTU/MSS Table is initialized by now, so load their values.  If
	 * we're initializing the adapter, then we'll make any modifications
	 * we want to the MTU/MSS Table and also initialize the congestion
	 * parameters.
	 */
	t4_read_mtu_tbl(adap, adap->params.mtus, NULL);
	if (state != DEV_STATE_INIT) {
		int i;

		/* The default MTU Table contains values 1492 and 1500.
		 * However, for TCP, it's better to have two values which are
		 * a multiple of 8 +/- 4 bytes apart near this popular MTU.
		 * This allows us to have a TCP Data Payload which is a
		 * multiple of 8 regardless of what combination of TCP Options
		 * are in use (always a multiple of 4 bytes) which is
		 * important for performance reasons.  For instance, if no
		 * options are in use, then we have a 20-byte IP header and a
		 * 20-byte TCP header.  In this case, a 1500-byte MSS would
		 * result in a TCP Data Payload of 1500 - 40 == 1460 bytes
		 * which is not a multiple of 8.  So using an MSS of 1488 in
		 * this case results in a TCP Data Payload of 1448 bytes which
		 * is a multiple of 8.  On the other hand, if 12-byte TCP Time
		 * Stamps have been negotiated, then an MTU of 1500 bytes
		 * results in a TCP Data Payload of 1448 bytes which, as
		 * above, is a multiple of 8 bytes ...
		 */
		for (i = 0; i < NMTUS; i++)
			if (adap->params.mtus[i] == 1492) {
				adap->params.mtus[i] = 1488;
				break;
			}

		t4_load_mtus(adap, adap->params.mtus, adap->params.a_wnd,
			     adap->params.b_wnd);
	}
<<<<<<< HEAD
	t4_init_tp_params(adap);
	adap->flags |= FW_OK;
=======
	t4_init_sge_params(adap);
	adap->flags |= FW_OK;
	t4_init_tp_params(adap);
>>>>>>> v4.9.227
	return 0;

	/*
	 * Something bad happened.  If a command timed out or failed with EIO
	 * FW does not operate within its spec or something catastrophic
	 * happened to HW/FW, stop issuing commands.
	 */
bye:
<<<<<<< HEAD
=======
	kfree(adap->sge.egr_map);
	kfree(adap->sge.ingr_map);
	kfree(adap->sge.starving_fl);
	kfree(adap->sge.txq_maperr);
#ifdef CONFIG_DEBUG_FS
	kfree(adap->sge.blocked_fl);
#endif
>>>>>>> v4.9.227
	if (ret != -ETIMEDOUT && ret != -EIO)
		t4_fw_bye(adap, adap->mbox);
	return ret;
}

/* EEH callbacks */

static pci_ers_result_t eeh_err_detected(struct pci_dev *pdev,
					 pci_channel_state_t state)
{
	int i;
	struct adapter *adap = pci_get_drvdata(pdev);

	if (!adap)
		goto out;

	rtnl_lock();
	adap->flags &= ~FW_OK;
	notify_ulds(adap, CXGB4_STATE_START_RECOVERY);
	spin_lock(&adap->stats_lock);
	for_each_port(adap, i) {
		struct net_device *dev = adap->port[i];

		netif_device_detach(dev);
		netif_carrier_off(dev);
	}
	spin_unlock(&adap->stats_lock);
<<<<<<< HEAD
=======
	disable_interrupts(adap);
>>>>>>> v4.9.227
	if (adap->flags & FULL_INIT_DONE)
		cxgb_down(adap);
	rtnl_unlock();
	if ((adap->flags & DEV_ENABLED)) {
		pci_disable_device(pdev);
		adap->flags &= ~DEV_ENABLED;
	}
out:	return state == pci_channel_io_perm_failure ?
		PCI_ERS_RESULT_DISCONNECT : PCI_ERS_RESULT_NEED_RESET;
}

static pci_ers_result_t eeh_slot_reset(struct pci_dev *pdev)
{
	int i, ret;
	struct fw_caps_config_cmd c;
	struct adapter *adap = pci_get_drvdata(pdev);

	if (!adap) {
		pci_restore_state(pdev);
		pci_save_state(pdev);
		return PCI_ERS_RESULT_RECOVERED;
	}

	if (!(adap->flags & DEV_ENABLED)) {
		if (pci_enable_device(pdev)) {
			dev_err(&pdev->dev, "Cannot reenable PCI "
					    "device after reset\n");
			return PCI_ERS_RESULT_DISCONNECT;
		}
		adap->flags |= DEV_ENABLED;
	}

	pci_set_master(pdev);
	pci_restore_state(pdev);
	pci_save_state(pdev);
	pci_cleanup_aer_uncorrect_error_status(pdev);

	if (t4_wait_dev_ready(adap->regs) < 0)
		return PCI_ERS_RESULT_DISCONNECT;
<<<<<<< HEAD
	if (t4_fw_hello(adap, adap->fn, adap->fn, MASTER_MUST, NULL) < 0)
=======
	if (t4_fw_hello(adap, adap->mbox, adap->pf, MASTER_MUST, NULL) < 0)
>>>>>>> v4.9.227
		return PCI_ERS_RESULT_DISCONNECT;
	adap->flags |= FW_OK;
	if (adap_init1(adap, &c))
		return PCI_ERS_RESULT_DISCONNECT;

	for_each_port(adap, i) {
		struct port_info *p = adap2pinfo(adap, i);

<<<<<<< HEAD
		ret = t4_alloc_vi(adap, adap->fn, p->tx_chan, adap->fn, 0, 1,
=======
		ret = t4_alloc_vi(adap, adap->mbox, p->tx_chan, adap->pf, 0, 1,
>>>>>>> v4.9.227
				  NULL, NULL);
		if (ret < 0)
			return PCI_ERS_RESULT_DISCONNECT;
		p->viid = ret;
		p->xact_addr_filt = -1;
	}

	t4_load_mtus(adap, adap->params.mtus, adap->params.a_wnd,
		     adap->params.b_wnd);
	setup_memwin(adap);
	if (cxgb_up(adap))
		return PCI_ERS_RESULT_DISCONNECT;
	return PCI_ERS_RESULT_RECOVERED;
}

static void eeh_resume(struct pci_dev *pdev)
{
	int i;
	struct adapter *adap = pci_get_drvdata(pdev);

	if (!adap)
		return;

	rtnl_lock();
	for_each_port(adap, i) {
		struct net_device *dev = adap->port[i];

		if (netif_running(dev)) {
			link_start(dev);
			cxgb_set_rxmode(dev);
		}
		netif_device_attach(dev);
	}
	rtnl_unlock();
}

static const struct pci_error_handlers cxgb4_eeh = {
	.error_detected = eeh_err_detected,
	.slot_reset     = eeh_slot_reset,
	.resume         = eeh_resume,
};

<<<<<<< HEAD
static inline bool is_x_10g_port(const struct link_config *lc)
{
	return (lc->supported & FW_PORT_CAP_SPEED_10G) != 0 ||
	       (lc->supported & FW_PORT_CAP_SPEED_40G) != 0;
}

static inline void init_rspq(struct adapter *adap, struct sge_rspq *q,
			     unsigned int us, unsigned int cnt,
			     unsigned int size, unsigned int iqe_size)
{
	q->adap = adap;
	set_rspq_intr_params(q, us, cnt);
	q->iqe_len = iqe_size;
	q->size = size;
=======
/* Return true if the Link Configuration supports "High Speeds" (those greater
 * than 1Gb/s).
 */
static inline bool is_x_10g_port(const struct link_config *lc)
{
	unsigned int speeds, high_speeds;

	speeds = FW_PORT_CAP_SPEED_V(FW_PORT_CAP_SPEED_G(lc->supported));
	high_speeds = speeds & ~(FW_PORT_CAP_SPEED_100M | FW_PORT_CAP_SPEED_1G);

	return high_speeds != 0;
>>>>>>> v4.9.227
}

/*
 * Perform default configuration of DMA queues depending on the number and type
 * of ports we found and the number of available CPUs.  Most settings can be
 * modified by the admin prior to actual use.
 */
static void cfg_queues(struct adapter *adap)
{
	struct sge *s = &adap->sge;
	int i, n10g = 0, qidx = 0;
#ifndef CONFIG_CHELSIO_T4_DCB
	int q10g = 0;
#endif
<<<<<<< HEAD
	int ciq_size;
=======

	/* Reduce memory usage in kdump environment, disable all offload.
	 */
	if (is_kdump_kernel()) {
		adap->params.offload = 0;
		adap->params.crypto = 0;
	} else if (is_uld(adap) && t4_uld_mem_alloc(adap)) {
		adap->params.offload = 0;
		adap->params.crypto = 0;
	}
>>>>>>> v4.9.227

	for_each_port(adap, i)
		n10g += is_x_10g_port(&adap2pinfo(adap, i)->link_cfg);
#ifdef CONFIG_CHELSIO_T4_DCB
	/* For Data Center Bridging support we need to be able to support up
	 * to 8 Traffic Priorities; each of which will be assigned to its
	 * own TX Queue in order to prevent Head-Of-Line Blocking.
	 */
	if (adap->params.nports * 8 > MAX_ETH_QSETS) {
		dev_err(adap->pdev_dev, "MAX_ETH_QSETS=%d < %d!\n",
			MAX_ETH_QSETS, adap->params.nports * 8);
		BUG_ON(1);
	}

	for_each_port(adap, i) {
		struct port_info *pi = adap2pinfo(adap, i);

		pi->first_qset = qidx;
		pi->nqsets = 8;
		qidx += pi->nqsets;
	}
#else /* !CONFIG_CHELSIO_T4_DCB */
	/*
	 * We default to 1 queue per non-10G port and up to # of cores queues
	 * per 10G port.
	 */
	if (n10g)
		q10g = (MAX_ETH_QSETS - (adap->params.nports - n10g)) / n10g;
	if (q10g > netif_get_num_default_rss_queues())
		q10g = netif_get_num_default_rss_queues();

	for_each_port(adap, i) {
		struct port_info *pi = adap2pinfo(adap, i);

		pi->first_qset = qidx;
		pi->nqsets = is_x_10g_port(&pi->link_cfg) ? q10g : 1;
		qidx += pi->nqsets;
	}
#endif /* !CONFIG_CHELSIO_T4_DCB */

	s->ethqsets = qidx;
	s->max_ethqsets = qidx;   /* MSI-X may lower it later */

<<<<<<< HEAD
	if (is_offload(adap)) {
=======
	if (is_uld(adap)) {
>>>>>>> v4.9.227
		/*
		 * For offload we use 1 queue/channel if all ports are up to 1G,
		 * otherwise we divide all available queues amongst the channels
		 * capped by the number of available cores.
		 */
		if (n10g) {
<<<<<<< HEAD
			i = min_t(int, ARRAY_SIZE(s->ofldrxq),
				  num_online_cpus());
			s->ofldqsets = roundup(i, adap->params.nports);
		} else
			s->ofldqsets = adap->params.nports;
		/* For RDMA one Rx queue per channel suffices */
		s->rdmaqs = adap->params.nports;
		s->rdmaciqs = adap->params.nports;
=======
			i = min_t(int, MAX_OFLD_QSETS, num_online_cpus());
			s->ofldqsets = roundup(i, adap->params.nports);
		} else {
			s->ofldqsets = adap->params.nports;
		}
>>>>>>> v4.9.227
	}

	for (i = 0; i < ARRAY_SIZE(s->ethrxq); i++) {
		struct sge_eth_rxq *r = &s->ethrxq[i];

		init_rspq(adap, &r->rspq, 5, 10, 1024, 64);
		r->fl.size = 72;
	}

	for (i = 0; i < ARRAY_SIZE(s->ethtxq); i++)
		s->ethtxq[i].q.size = 1024;

	for (i = 0; i < ARRAY_SIZE(s->ctrlq); i++)
		s->ctrlq[i].q.size = 512;

	for (i = 0; i < ARRAY_SIZE(s->ofldtxq); i++)
		s->ofldtxq[i].q.size = 1024;

<<<<<<< HEAD
	for (i = 0; i < ARRAY_SIZE(s->ofldrxq); i++) {
		struct sge_ofld_rxq *r = &s->ofldrxq[i];

		init_rspq(adap, &r->rspq, 5, 1, 1024, 64);
		r->rspq.uld = CXGB4_ULD_ISCSI;
		r->fl.size = 72;
	}

	for (i = 0; i < ARRAY_SIZE(s->rdmarxq); i++) {
		struct sge_ofld_rxq *r = &s->rdmarxq[i];

		init_rspq(adap, &r->rspq, 5, 1, 511, 64);
		r->rspq.uld = CXGB4_ULD_RDMA;
		r->fl.size = 72;
	}

	ciq_size = 64 + adap->vres.cq.size + adap->tids.nftids;
	if (ciq_size > SGE_MAX_IQ_SIZE) {
		CH_WARN(adap, "CIQ size too small for available IQs\n");
		ciq_size = SGE_MAX_IQ_SIZE;
	}

	for (i = 0; i < ARRAY_SIZE(s->rdmaciq); i++) {
		struct sge_ofld_rxq *r = &s->rdmaciq[i];

		init_rspq(adap, &r->rspq, 5, 1, ciq_size, 64);
		r->rspq.uld = CXGB4_ULD_RDMA;
	}

	init_rspq(adap, &s->fw_evtq, 0, 1, 1024, 64);
	init_rspq(adap, &s->intrq, 0, 1, 2 * MAX_INGQ, 64);
=======
	init_rspq(adap, &s->fw_evtq, 0, 1, 1024, 64);
	init_rspq(adap, &s->intrq, 0, 1, 512, 64);
>>>>>>> v4.9.227
}

/*
 * Reduce the number of Ethernet queues across all ports to at most n.
 * n provides at least one queue per port.
 */
static void reduce_ethqs(struct adapter *adap, int n)
{
	int i;
	struct port_info *pi;

	while (n < adap->sge.ethqsets)
		for_each_port(adap, i) {
			pi = adap2pinfo(adap, i);
			if (pi->nqsets > 1) {
				pi->nqsets--;
				adap->sge.ethqsets--;
				if (adap->sge.ethqsets <= n)
					break;
			}
		}

	n = 0;
	for_each_port(adap, i) {
		pi = adap2pinfo(adap, i);
		pi->first_qset = n;
		n += pi->nqsets;
	}
}

<<<<<<< HEAD
=======
static int get_msix_info(struct adapter *adap)
{
	struct uld_msix_info *msix_info;
	unsigned int max_ingq = 0;

	if (is_offload(adap))
		max_ingq += MAX_OFLD_QSETS * adap->num_ofld_uld;
	if (is_pci_uld(adap))
		max_ingq += MAX_OFLD_QSETS * adap->num_uld;

	if (!max_ingq)
		goto out;

	msix_info = kcalloc(max_ingq, sizeof(*msix_info), GFP_KERNEL);
	if (!msix_info)
		return -ENOMEM;

	adap->msix_bmap_ulds.msix_bmap = kcalloc(BITS_TO_LONGS(max_ingq),
						 sizeof(long), GFP_KERNEL);
	if (!adap->msix_bmap_ulds.msix_bmap) {
		kfree(msix_info);
		return -ENOMEM;
	}
	spin_lock_init(&adap->msix_bmap_ulds.lock);
	adap->msix_info_ulds = msix_info;
out:
	return 0;
}

static void free_msix_info(struct adapter *adap)
{
	if (!(adap->num_uld && adap->num_ofld_uld))
		return;

	kfree(adap->msix_info_ulds);
	kfree(adap->msix_bmap_ulds.msix_bmap);
}

>>>>>>> v4.9.227
/* 2 MSI-X vectors needed for the FW queue and non-data interrupts */
#define EXTRA_VECS 2

static int enable_msix(struct adapter *adap)
{
<<<<<<< HEAD
	int ofld_need = 0;
	int i, want, need;
	struct sge *s = &adap->sge;
	unsigned int nchan = adap->params.nports;
	struct msix_entry entries[MAX_INGQ + 1];

	for (i = 0; i < ARRAY_SIZE(entries); ++i)
=======
	int ofld_need = 0, uld_need = 0;
	int i, j, want, need, allocated;
	struct sge *s = &adap->sge;
	unsigned int nchan = adap->params.nports;
	struct msix_entry *entries;
	int max_ingq = MAX_INGQ;

	if (is_pci_uld(adap))
		max_ingq += (MAX_OFLD_QSETS * adap->num_uld);
	if (is_offload(adap))
		max_ingq += (MAX_OFLD_QSETS * adap->num_ofld_uld);
	entries = kmalloc(sizeof(*entries) * (max_ingq + 1),
			  GFP_KERNEL);
	if (!entries)
		return -ENOMEM;

	/* map for msix */
	if (get_msix_info(adap)) {
		adap->params.offload = 0;
		adap->params.crypto = 0;
	}

	for (i = 0; i < max_ingq + 1; ++i)
>>>>>>> v4.9.227
		entries[i].entry = i;

	want = s->max_ethqsets + EXTRA_VECS;
	if (is_offload(adap)) {
<<<<<<< HEAD
		want += s->rdmaqs + s->rdmaciqs + s->ofldqsets;
		/* need nchan for each possible ULD */
		ofld_need = 3 * nchan;
=======
		want += adap->num_ofld_uld * s->ofldqsets;
		ofld_need = adap->num_ofld_uld * nchan;
	}
	if (is_pci_uld(adap)) {
		want += adap->num_uld * s->ofldqsets;
		uld_need = adap->num_uld * nchan;
>>>>>>> v4.9.227
	}
#ifdef CONFIG_CHELSIO_T4_DCB
	/* For Data Center Bridging we need 8 Ethernet TX Priority Queues for
	 * each port.
	 */
<<<<<<< HEAD
	need = 8 * adap->params.nports + EXTRA_VECS + ofld_need;
#else
	need = adap->params.nports + EXTRA_VECS + ofld_need;
#endif
	want = pci_enable_msix_range(adap->pdev, entries, need, want);
	if (want < 0)
		return want;

	/*
	 * Distribute available vectors to the various queue groups.
	 * Every group gets its minimum requirement and NIC gets top
	 * priority for leftovers.
	 */
	i = want - EXTRA_VECS - ofld_need;
=======
	need = 8 * adap->params.nports + EXTRA_VECS + ofld_need + uld_need;
#else
	need = adap->params.nports + EXTRA_VECS + ofld_need + uld_need;
#endif
	allocated = pci_enable_msix_range(adap->pdev, entries, need, want);
	if (allocated < 0) {
		dev_info(adap->pdev_dev, "not enough MSI-X vectors left,"
			 " not using MSI-X\n");
		kfree(entries);
		return allocated;
	}

	/* Distribute available vectors to the various queue groups.
	 * Every group gets its minimum requirement and NIC gets top
	 * priority for leftovers.
	 */
	i = allocated - EXTRA_VECS - ofld_need - uld_need;
>>>>>>> v4.9.227
	if (i < s->max_ethqsets) {
		s->max_ethqsets = i;
		if (i < s->ethqsets)
			reduce_ethqs(adap, i);
	}
<<<<<<< HEAD
	if (is_offload(adap)) {
		i = want - EXTRA_VECS - s->max_ethqsets;
		i -= ofld_need - nchan;
		s->ofldqsets = (i / nchan) * nchan;  /* round down */
	}
	for (i = 0; i < want; ++i)
		adap->msix_info[i].vec = entries[i].vector;

=======
	if (is_uld(adap)) {
		if (allocated < want)
			s->nqs_per_uld = nchan;
		else
			s->nqs_per_uld = s->ofldqsets;
	}

	for (i = 0; i < (s->max_ethqsets + EXTRA_VECS); ++i)
		adap->msix_info[i].vec = entries[i].vector;
	if (is_uld(adap)) {
		for (j = 0 ; i < allocated; ++i, j++) {
			adap->msix_info_ulds[j].vec = entries[i].vector;
			adap->msix_info_ulds[j].idx = i;
		}
		adap->msix_bmap_ulds.mapsize = j;
	}
	dev_info(adap->pdev_dev, "%d MSI-X vectors allocated, "
		 "nic %d per uld %d\n",
		 allocated, s->max_ethqsets, s->nqs_per_uld);

	kfree(entries);
>>>>>>> v4.9.227
	return 0;
}

#undef EXTRA_VECS

static int init_rss(struct adapter *adap)
{
<<<<<<< HEAD
	unsigned int i, j;
=======
	unsigned int i;
	int err;

	err = t4_init_rss_mode(adap, adap->mbox);
	if (err)
		return err;
>>>>>>> v4.9.227

	for_each_port(adap, i) {
		struct port_info *pi = adap2pinfo(adap, i);

		pi->rss = kcalloc(pi->rss_size, sizeof(u16), GFP_KERNEL);
		if (!pi->rss)
			return -ENOMEM;
<<<<<<< HEAD
		for (j = 0; j < pi->rss_size; j++)
			pi->rss[j] = ethtool_rxfh_indir_default(j, pi->nqsets);
=======
>>>>>>> v4.9.227
	}
	return 0;
}

<<<<<<< HEAD
=======
static int cxgb4_get_pcie_dev_link_caps(struct adapter *adap,
					enum pci_bus_speed *speed,
					enum pcie_link_width *width)
{
	u32 lnkcap1, lnkcap2;
	int err1, err2;

#define  PCIE_MLW_CAP_SHIFT 4   /* start of MLW mask in link capabilities */

	*speed = PCI_SPEED_UNKNOWN;
	*width = PCIE_LNK_WIDTH_UNKNOWN;

	err1 = pcie_capability_read_dword(adap->pdev, PCI_EXP_LNKCAP,
					  &lnkcap1);
	err2 = pcie_capability_read_dword(adap->pdev, PCI_EXP_LNKCAP2,
					  &lnkcap2);
	if (!err2 && lnkcap2) { /* PCIe r3.0-compliant */
		if (lnkcap2 & PCI_EXP_LNKCAP2_SLS_8_0GB)
			*speed = PCIE_SPEED_8_0GT;
		else if (lnkcap2 & PCI_EXP_LNKCAP2_SLS_5_0GB)
			*speed = PCIE_SPEED_5_0GT;
		else if (lnkcap2 & PCI_EXP_LNKCAP2_SLS_2_5GB)
			*speed = PCIE_SPEED_2_5GT;
	}
	if (!err1) {
		*width = (lnkcap1 & PCI_EXP_LNKCAP_MLW) >> PCIE_MLW_CAP_SHIFT;
		if (!lnkcap2) { /* pre-r3.0 */
			if (lnkcap1 & PCI_EXP_LNKCAP_SLS_5_0GB)
				*speed = PCIE_SPEED_5_0GT;
			else if (lnkcap1 & PCI_EXP_LNKCAP_SLS_2_5GB)
				*speed = PCIE_SPEED_2_5GT;
		}
	}

	if (*speed == PCI_SPEED_UNKNOWN || *width == PCIE_LNK_WIDTH_UNKNOWN)
		return err1 ? err1 : err2 ? err2 : -EINVAL;
	return 0;
}

static void cxgb4_check_pcie_caps(struct adapter *adap)
{
	enum pcie_link_width width, width_cap;
	enum pci_bus_speed speed, speed_cap;

#define PCIE_SPEED_STR(speed) \
	(speed == PCIE_SPEED_8_0GT ? "8.0GT/s" : \
	 speed == PCIE_SPEED_5_0GT ? "5.0GT/s" : \
	 speed == PCIE_SPEED_2_5GT ? "2.5GT/s" : \
	 "Unknown")

	if (cxgb4_get_pcie_dev_link_caps(adap, &speed_cap, &width_cap)) {
		dev_warn(adap->pdev_dev,
			 "Unable to determine PCIe device BW capabilities\n");
		return;
	}

	if (pcie_get_minimum_link(adap->pdev, &speed, &width) ||
	    speed == PCI_SPEED_UNKNOWN || width == PCIE_LNK_WIDTH_UNKNOWN) {
		dev_warn(adap->pdev_dev,
			 "Unable to determine PCI Express bandwidth.\n");
		return;
	}

	dev_info(adap->pdev_dev, "PCIe link speed is %s, device supports %s\n",
		 PCIE_SPEED_STR(speed), PCIE_SPEED_STR(speed_cap));
	dev_info(adap->pdev_dev, "PCIe link width is x%d, device supports x%d\n",
		 width, width_cap);
	if (speed < speed_cap || width < width_cap)
		dev_info(adap->pdev_dev,
			 "A slot with more lanes and/or higher speed is "
			 "suggested for optimal performance.\n");
}

/* Dump basic information about the adapter */
static void print_adapter_info(struct adapter *adapter)
{
	/* Device information */
	dev_info(adapter->pdev_dev, "Chelsio %s rev %d\n",
		 adapter->params.vpd.id,
		 CHELSIO_CHIP_RELEASE(adapter->params.chip));
	dev_info(adapter->pdev_dev, "S/N: %s, P/N: %s\n",
		 adapter->params.vpd.sn, adapter->params.vpd.pn);

	/* Firmware Version */
	if (!adapter->params.fw_vers)
		dev_warn(adapter->pdev_dev, "No firmware loaded\n");
	else
		dev_info(adapter->pdev_dev, "Firmware version: %u.%u.%u.%u\n",
			 FW_HDR_FW_VER_MAJOR_G(adapter->params.fw_vers),
			 FW_HDR_FW_VER_MINOR_G(adapter->params.fw_vers),
			 FW_HDR_FW_VER_MICRO_G(adapter->params.fw_vers),
			 FW_HDR_FW_VER_BUILD_G(adapter->params.fw_vers));

	/* Bootstrap Firmware Version. (Some adapters don't have Bootstrap
	 * Firmware, so dev_info() is more appropriate here.)
	 */
	if (!adapter->params.bs_vers)
		dev_info(adapter->pdev_dev, "No bootstrap loaded\n");
	else
		dev_info(adapter->pdev_dev, "Bootstrap version: %u.%u.%u.%u\n",
			 FW_HDR_FW_VER_MAJOR_G(adapter->params.bs_vers),
			 FW_HDR_FW_VER_MINOR_G(adapter->params.bs_vers),
			 FW_HDR_FW_VER_MICRO_G(adapter->params.bs_vers),
			 FW_HDR_FW_VER_BUILD_G(adapter->params.bs_vers));

	/* TP Microcode Version */
	if (!adapter->params.tp_vers)
		dev_warn(adapter->pdev_dev, "No TP Microcode loaded\n");
	else
		dev_info(adapter->pdev_dev,
			 "TP Microcode version: %u.%u.%u.%u\n",
			 FW_HDR_FW_VER_MAJOR_G(adapter->params.tp_vers),
			 FW_HDR_FW_VER_MINOR_G(adapter->params.tp_vers),
			 FW_HDR_FW_VER_MICRO_G(adapter->params.tp_vers),
			 FW_HDR_FW_VER_BUILD_G(adapter->params.tp_vers));

	/* Expansion ROM version */
	if (!adapter->params.er_vers)
		dev_info(adapter->pdev_dev, "No Expansion ROM loaded\n");
	else
		dev_info(adapter->pdev_dev,
			 "Expansion ROM version: %u.%u.%u.%u\n",
			 FW_HDR_FW_VER_MAJOR_G(adapter->params.er_vers),
			 FW_HDR_FW_VER_MINOR_G(adapter->params.er_vers),
			 FW_HDR_FW_VER_MICRO_G(adapter->params.er_vers),
			 FW_HDR_FW_VER_BUILD_G(adapter->params.er_vers));

	/* Software/Hardware configuration */
	dev_info(adapter->pdev_dev, "Configuration: %sNIC %s, %s capable\n",
		 is_offload(adapter) ? "R" : "",
		 ((adapter->flags & USING_MSIX) ? "MSI-X" :
		  (adapter->flags & USING_MSI) ? "MSI" : ""),
		 is_offload(adapter) ? "Offload" : "non-Offload");
}

>>>>>>> v4.9.227
static void print_port_info(const struct net_device *dev)
{
	char buf[80];
	char *bufp = buf;
	const char *spd = "";
	const struct port_info *pi = netdev_priv(dev);
	const struct adapter *adap = pi->adapter;

	if (adap->params.pci.speed == PCI_EXP_LNKSTA_CLS_2_5GB)
		spd = " 2.5 GT/s";
	else if (adap->params.pci.speed == PCI_EXP_LNKSTA_CLS_5_0GB)
		spd = " 5 GT/s";
	else if (adap->params.pci.speed == PCI_EXP_LNKSTA_CLS_8_0GB)
		spd = " 8 GT/s";

	if (pi->link_cfg.supported & FW_PORT_CAP_SPEED_100M)
		bufp += sprintf(bufp, "100/");
	if (pi->link_cfg.supported & FW_PORT_CAP_SPEED_1G)
		bufp += sprintf(bufp, "1000/");
	if (pi->link_cfg.supported & FW_PORT_CAP_SPEED_10G)
		bufp += sprintf(bufp, "10G/");
<<<<<<< HEAD
	if (pi->link_cfg.supported & FW_PORT_CAP_SPEED_40G)
		bufp += sprintf(bufp, "40G/");
=======
	if (pi->link_cfg.supported & FW_PORT_CAP_SPEED_25G)
		bufp += sprintf(bufp, "25G/");
	if (pi->link_cfg.supported & FW_PORT_CAP_SPEED_40G)
		bufp += sprintf(bufp, "40G/");
	if (pi->link_cfg.supported & FW_PORT_CAP_SPEED_100G)
		bufp += sprintf(bufp, "100G/");
>>>>>>> v4.9.227
	if (bufp != buf)
		--bufp;
	sprintf(bufp, "BASE-%s", t4_get_port_type_description(pi->port_type));

<<<<<<< HEAD
	netdev_info(dev, "Chelsio %s rev %d %s %sNIC PCIe x%d%s%s\n",
		    adap->params.vpd.id,
		    CHELSIO_CHIP_RELEASE(adap->params.chip), buf,
		    is_offload(adap) ? "R" : "", adap->params.pci.width, spd,
		    (adap->flags & USING_MSIX) ? " MSI-X" :
		    (adap->flags & USING_MSI) ? " MSI" : "");
	netdev_info(dev, "S/N: %s, P/N: %s\n",
		    adap->params.vpd.sn, adap->params.vpd.pn);
=======
	netdev_info(dev, "%s: Chelsio %s (%s) %s\n",
		    dev->name, adap->params.vpd.id, adap->name, buf);
>>>>>>> v4.9.227
}

static void enable_pcie_relaxed_ordering(struct pci_dev *dev)
{
	pcie_capability_set_word(dev, PCI_EXP_DEVCTL, PCI_EXP_DEVCTL_RELAX_EN);
}

/*
 * Free the following resources:
 * - memory used for tables
 * - MSI/MSI-X
 * - net devices
 * - resources FW is holding for us
 */
static void free_some_resources(struct adapter *adapter)
{
	unsigned int i;

	t4_free_mem(adapter->l2t);
<<<<<<< HEAD
	t4_free_mem(adapter->tids.tid_tab);
=======
	t4_cleanup_sched(adapter);
	t4_free_mem(adapter->tids.tid_tab);
	cxgb4_cleanup_tc_u32(adapter);
	kfree(adapter->sge.egr_map);
	kfree(adapter->sge.ingr_map);
	kfree(adapter->sge.starving_fl);
	kfree(adapter->sge.txq_maperr);
#ifdef CONFIG_DEBUG_FS
	kfree(adapter->sge.blocked_fl);
#endif
>>>>>>> v4.9.227
	disable_msi(adapter);

	for_each_port(adapter, i)
		if (adapter->port[i]) {
<<<<<<< HEAD
=======
			struct port_info *pi = adap2pinfo(adapter, i);

			if (pi->viid != 0)
				t4_free_vi(adapter, adapter->mbox, adapter->pf,
					   0, pi->viid);
>>>>>>> v4.9.227
			kfree(adap2pinfo(adapter, i)->rss);
			free_netdev(adapter->port[i]);
		}
	if (adapter->flags & FW_OK)
<<<<<<< HEAD
		t4_fw_bye(adapter, adapter->fn);
=======
		t4_fw_bye(adapter, adapter->pf);
>>>>>>> v4.9.227
}

#define TSO_FLAGS (NETIF_F_TSO | NETIF_F_TSO6 | NETIF_F_TSO_ECN)
#define VLAN_FEAT (NETIF_F_SG | NETIF_F_IP_CSUM | TSO_FLAGS | \
		   NETIF_F_IPV6_CSUM | NETIF_F_HIGHDMA)
#define SEGMENT_SIZE 128

<<<<<<< HEAD
=======
static int get_chip_type(struct pci_dev *pdev, u32 pl_rev)
{
	u16 device_id;

	/* Retrieve adapter's device ID */
	pci_read_config_word(pdev, PCI_DEVICE_ID, &device_id);

	switch (device_id >> 12) {
	case CHELSIO_T4:
		return CHELSIO_CHIP_CODE(CHELSIO_T4, pl_rev);
	case CHELSIO_T5:
		return CHELSIO_CHIP_CODE(CHELSIO_T5, pl_rev);
	case CHELSIO_T6:
		return CHELSIO_CHIP_CODE(CHELSIO_T6, pl_rev);
	default:
		dev_err(&pdev->dev, "Device %d is not supported\n",
			device_id);
	}
	return -EINVAL;
}

#ifdef CONFIG_PCI_IOV
static void dummy_setup(struct net_device *dev)
{
	dev->type = ARPHRD_NONE;
	dev->mtu = 0;
	dev->hard_header_len = 0;
	dev->addr_len = 0;
	dev->tx_queue_len = 0;
	dev->flags |= IFF_NOARP;
	dev->priv_flags |= IFF_NO_QUEUE;

	/* Initialize the device structure. */
	dev->netdev_ops = &cxgb4_mgmt_netdev_ops;
	dev->ethtool_ops = &cxgb4_mgmt_ethtool_ops;
	dev->destructor = free_netdev;
}

static int config_mgmt_dev(struct pci_dev *pdev)
{
	struct adapter *adap = pci_get_drvdata(pdev);
	struct net_device *netdev;
	struct port_info *pi;
	char name[IFNAMSIZ];
	int err;

	snprintf(name, IFNAMSIZ, "mgmtpf%d%d", adap->adap_idx, adap->pf);
	netdev = alloc_netdev(0, name, NET_NAME_UNKNOWN, dummy_setup);
	if (!netdev)
		return -ENOMEM;

	pi = netdev_priv(netdev);
	pi->adapter = adap;
	SET_NETDEV_DEV(netdev, &pdev->dev);

	adap->port[0] = netdev;

	err = register_netdev(adap->port[0]);
	if (err) {
		pr_info("Unable to register VF mgmt netdev %s\n", name);
		free_netdev(adap->port[0]);
		adap->port[0] = NULL;
		return err;
	}
	return 0;
}

static int cxgb4_iov_configure(struct pci_dev *pdev, int num_vfs)
{
	struct adapter *adap = pci_get_drvdata(pdev);
	int err = 0;
	int current_vfs = pci_num_vf(pdev);
	u32 pcie_fw;

	pcie_fw = readl(adap->regs + PCIE_FW_A);
	/* Check if cxgb4 is the MASTER and fw is initialized */
	if (!(pcie_fw & PCIE_FW_INIT_F) ||
	    !(pcie_fw & PCIE_FW_MASTER_VLD_F) ||
	    PCIE_FW_MASTER_G(pcie_fw) != 4) {
		dev_warn(&pdev->dev,
			 "cxgb4 driver needs to be MASTER to support SRIOV\n");
		return -EOPNOTSUPP;
	}

	/* If any of the VF's is already assigned to Guest OS, then
	 * SRIOV for the same cannot be modified
	 */
	if (current_vfs && pci_vfs_assigned(pdev)) {
		dev_err(&pdev->dev,
			"Cannot modify SR-IOV while VFs are assigned\n");
		num_vfs = current_vfs;
		return num_vfs;
	}

	/* Disable SRIOV when zero is passed.
	 * One needs to disable SRIOV before modifying it, else
	 * stack throws the below warning:
	 * " 'n' VFs already enabled. Disable before enabling 'm' VFs."
	 */
	if (!num_vfs) {
		pci_disable_sriov(pdev);
		if (adap->port[0]) {
			unregister_netdev(adap->port[0]);
			adap->port[0] = NULL;
		}
		/* free VF resources */
		kfree(adap->vfinfo);
		adap->vfinfo = NULL;
		adap->num_vfs = 0;
		return num_vfs;
	}

	if (num_vfs != current_vfs) {
		err = pci_enable_sriov(pdev, num_vfs);
		if (err)
			return err;

		adap->num_vfs = num_vfs;
		err = config_mgmt_dev(pdev);
		if (err)
			return err;
	}

	adap->vfinfo = kcalloc(adap->num_vfs,
			       sizeof(struct vf_info), GFP_KERNEL);
	if (adap->vfinfo)
		fill_vf_station_mac_addr(adap);
	return num_vfs;
}
#endif

>>>>>>> v4.9.227
static int init_one(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int func, i, err, s_qpp, qpp, num_seg;
	struct port_info *pi;
	bool highdma = false;
	struct adapter *adapter = NULL;
<<<<<<< HEAD
	void __iomem *regs;
=======
	struct net_device *netdev;
	void __iomem *regs;
	u32 whoami, pl_rev;
	enum chip_type chip;
	static int adap_idx = 1;
>>>>>>> v4.9.227

	printk_once(KERN_INFO "%s - version %s\n", DRV_DESC, DRV_VERSION);

	err = pci_request_regions(pdev, KBUILD_MODNAME);
	if (err) {
		/* Just info, some other driver may have claimed the device. */
		dev_info(&pdev->dev, "cannot obtain PCI resources\n");
		return err;
	}

	err = pci_enable_device(pdev);
	if (err) {
		dev_err(&pdev->dev, "cannot enable PCI device\n");
		goto out_release_regions;
	}

	regs = pci_ioremap_bar(pdev, 0);
	if (!regs) {
		dev_err(&pdev->dev, "cannot map device registers\n");
		err = -ENOMEM;
		goto out_disable_device;
	}

	err = t4_wait_dev_ready(regs);
	if (err < 0)
		goto out_unmap_bar0;

	/* We control everything through one PF */
<<<<<<< HEAD
	func = SOURCEPF_GET(readl(regs + PL_WHOAMI));
	if (func != ent->driver_data) {
		iounmap(regs);
=======
	whoami = readl(regs + PL_WHOAMI_A);
	pl_rev = REV_G(readl(regs + PL_REV_A));
	chip = get_chip_type(pdev, pl_rev);
	func = CHELSIO_CHIP_VERSION(chip) <= CHELSIO_T5 ?
		SOURCEPF_G(whoami) : T6_SOURCEPF_G(whoami);
	if (func != ent->driver_data) {
#ifndef CONFIG_PCI_IOV
		iounmap(regs);
#endif
>>>>>>> v4.9.227
		pci_disable_device(pdev);
		pci_save_state(pdev);        /* to restore SR-IOV later */
		goto sriov;
	}

	if (!pci_set_dma_mask(pdev, DMA_BIT_MASK(64))) {
		highdma = true;
		err = pci_set_consistent_dma_mask(pdev, DMA_BIT_MASK(64));
		if (err) {
			dev_err(&pdev->dev, "unable to obtain 64-bit DMA for "
				"coherent allocations\n");
			goto out_unmap_bar0;
		}
	} else {
		err = pci_set_dma_mask(pdev, DMA_BIT_MASK(32));
		if (err) {
			dev_err(&pdev->dev, "no usable DMA configuration\n");
			goto out_unmap_bar0;
		}
	}

	pci_enable_pcie_error_reporting(pdev);
	enable_pcie_relaxed_ordering(pdev);
	pci_set_master(pdev);
	pci_save_state(pdev);

	adapter = kzalloc(sizeof(*adapter), GFP_KERNEL);
	if (!adapter) {
		err = -ENOMEM;
		goto out_unmap_bar0;
	}
<<<<<<< HEAD
=======
	adap_idx++;
>>>>>>> v4.9.227

	adapter->workq = create_singlethread_workqueue("cxgb4");
	if (!adapter->workq) {
		err = -ENOMEM;
		goto out_free_adapter;
	}

<<<<<<< HEAD
=======
	adapter->mbox_log = kzalloc(sizeof(*adapter->mbox_log) +
				    (sizeof(struct mbox_cmd) *
				     T4_OS_LOG_MBOX_CMDS),
				    GFP_KERNEL);
	if (!adapter->mbox_log) {
		err = -ENOMEM;
		goto out_free_adapter;
	}
	adapter->mbox_log->size = T4_OS_LOG_MBOX_CMDS;

>>>>>>> v4.9.227
	/* PCI device has been enabled */
	adapter->flags |= DEV_ENABLED;

	adapter->regs = regs;
	adapter->pdev = pdev;
	adapter->pdev_dev = &pdev->dev;
<<<<<<< HEAD
	adapter->mbox = func;
	adapter->fn = func;
=======
	adapter->name = pci_name(pdev);
	adapter->mbox = func;
	adapter->pf = func;
>>>>>>> v4.9.227
	adapter->msg_enable = dflt_msg_enable;
	memset(adapter->chan_map, 0xff, sizeof(adapter->chan_map));

	spin_lock_init(&adapter->stats_lock);
	spin_lock_init(&adapter->tid_release_lock);
	spin_lock_init(&adapter->win0_lock);

	INIT_WORK(&adapter->tid_release_task, process_tid_release_list);
	INIT_WORK(&adapter->db_full_task, process_db_full);
	INIT_WORK(&adapter->db_drop_task, process_db_drop);

	err = t4_prep_adapter(adapter);
	if (err)
		goto out_free_adapter;


	if (!is_t4(adapter->params.chip)) {
<<<<<<< HEAD
		s_qpp = QUEUESPERPAGEPF1 * adapter->fn;
		qpp = 1 << QUEUESPERPAGEPF0_GET(t4_read_reg(adapter,
		      SGE_EGRESS_QUEUES_PER_PAGE_PF) >> s_qpp);
=======
		s_qpp = (QUEUESPERPAGEPF0_S +
			(QUEUESPERPAGEPF1_S - QUEUESPERPAGEPF0_S) *
			adapter->pf);
		qpp = 1 << QUEUESPERPAGEPF0_G(t4_read_reg(adapter,
		      SGE_EGRESS_QUEUES_PER_PAGE_PF_A) >> s_qpp);
>>>>>>> v4.9.227
		num_seg = PAGE_SIZE / SEGMENT_SIZE;

		/* Each segment size is 128B. Write coalescing is enabled only
		 * when SGE_EGRESS_QUEUES_PER_PAGE_PF reg value for the
		 * queue is less no of segments that can be accommodated in
		 * a page size.
		 */
		if (qpp > num_seg) {
			dev_err(&pdev->dev,
				"Incorrect number of egress queues per page\n");
			err = -EINVAL;
			goto out_free_adapter;
		}
		adapter->bar2 = ioremap_wc(pci_resource_start(pdev, 2),
		pci_resource_len(pdev, 2));
		if (!adapter->bar2) {
			dev_err(&pdev->dev, "cannot map device bar2 region\n");
			err = -ENOMEM;
			goto out_free_adapter;
		}
	}

	setup_memwin(adapter);
	err = adap_init0(adapter);
<<<<<<< HEAD
=======
#ifdef CONFIG_DEBUG_FS
	bitmap_zero(adapter->sge.blocked_fl, adapter->sge.egr_sz);
#endif
>>>>>>> v4.9.227
	setup_memwin_rdma(adapter);
	if (err)
		goto out_unmap_bar;

<<<<<<< HEAD
	for_each_port(adapter, i) {
		struct net_device *netdev;

=======
	/* configure SGE_STAT_CFG_A to read WC stats */
	if (!is_t4(adapter->params.chip))
		t4_write_reg(adapter, SGE_STAT_CFG_A, STATSOURCE_T5_V(7) |
			     (is_t5(adapter->params.chip) ? STATMODE_V(0) :
			      T6_STATMODE_V(0)));

	/* Initialize hash mac addr list */
	INIT_LIST_HEAD(&adapter->mac_hlist);

	for_each_port(adapter, i) {
>>>>>>> v4.9.227
		netdev = alloc_etherdev_mq(sizeof(struct port_info),
					   MAX_ETH_QSETS);
		if (!netdev) {
			err = -ENOMEM;
			goto out_free_dev;
		}

		SET_NETDEV_DEV(netdev, &pdev->dev);

		adapter->port[i] = netdev;
		pi = netdev_priv(netdev);
		pi->adapter = adapter;
		pi->xact_addr_filt = -1;
		pi->port_id = i;
		netdev->irq = pdev->irq;

		netdev->hw_features = NETIF_F_SG | TSO_FLAGS |
			NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM |
			NETIF_F_RXCSUM | NETIF_F_RXHASH |
<<<<<<< HEAD
			NETIF_F_HW_VLAN_CTAG_TX | NETIF_F_HW_VLAN_CTAG_RX;
=======
			NETIF_F_HW_VLAN_CTAG_TX | NETIF_F_HW_VLAN_CTAG_RX |
			NETIF_F_HW_TC;
>>>>>>> v4.9.227
		if (highdma)
			netdev->hw_features |= NETIF_F_HIGHDMA;
		netdev->features |= netdev->hw_features;
		netdev->vlan_features = netdev->features & VLAN_FEAT;

		netdev->priv_flags |= IFF_UNICAST_FLT;

		netdev->netdev_ops = &cxgb4_netdev_ops;
#ifdef CONFIG_CHELSIO_T4_DCB
		netdev->dcbnl_ops = &cxgb4_dcb_ops;
		cxgb4_dcb_state_init(netdev);
#endif
<<<<<<< HEAD
		netdev->ethtool_ops = &cxgb_ethtool_ops;
=======
		cxgb4_set_ethtool_ops(netdev);
>>>>>>> v4.9.227
	}

	pci_set_drvdata(pdev, adapter);

	if (adapter->flags & FW_OK) {
		err = t4_port_init(adapter, func, func, 0);
		if (err)
			goto out_free_dev;
<<<<<<< HEAD
	}

	/*
	 * Configure queues and allocate tables now, they can be needed as
=======
	} else if (adapter->params.nports == 1) {
		/* If we don't have a connection to the firmware -- possibly
		 * because of an error -- grab the raw VPD parameters so we
		 * can set the proper MAC Address on the debug network
		 * interface that we've created.
		 */
		u8 hw_addr[ETH_ALEN];
		u8 *na = adapter->params.vpd.na;

		err = t4_get_raw_vpd_params(adapter, &adapter->params.vpd);
		if (!err) {
			for (i = 0; i < ETH_ALEN; i++)
				hw_addr[i] = (hex2val(na[2 * i + 0]) * 16 +
					      hex2val(na[2 * i + 1]));
			t4_set_hw_addr(adapter, 0, hw_addr);
		}
	}

	/* Configure queues and allocate tables now, they can be needed as
>>>>>>> v4.9.227
	 * soon as the first register_netdev completes.
	 */
	cfg_queues(adapter);

<<<<<<< HEAD
	adapter->l2t = t4_init_l2t();
=======
	adapter->l2t = t4_init_l2t(adapter->l2t_start, adapter->l2t_end);
>>>>>>> v4.9.227
	if (!adapter->l2t) {
		/* We tolerate a lack of L2T, giving up some functionality */
		dev_warn(&pdev->dev, "could not allocate L2T, continuing\n");
		adapter->params.offload = 0;
	}

<<<<<<< HEAD
	if (is_offload(adapter) && tid_init(&adapter->tids) < 0) {
		dev_warn(&pdev->dev, "could not allocate TID table, "
			 "continuing\n");
		adapter->params.offload = 0;
=======
#if IS_ENABLED(CONFIG_IPV6)
	if ((CHELSIO_CHIP_VERSION(adapter->params.chip) <= CHELSIO_T5) &&
	    (!(t4_read_reg(adapter, LE_DB_CONFIG_A) & ASLIPCOMPEN_F))) {
		/* CLIP functionality is not present in hardware,
		 * hence disable all offload features
		 */
		dev_warn(&pdev->dev,
			 "CLIP not enabled in hardware, continuing\n");
		adapter->params.offload = 0;
	} else {
		adapter->clipt = t4_init_clip_tbl(adapter->clipt_start,
						  adapter->clipt_end);
		if (!adapter->clipt) {
			/* We tolerate a lack of clip_table, giving up
			 * some functionality
			 */
			dev_warn(&pdev->dev,
				 "could not allocate Clip table, continuing\n");
			adapter->params.offload = 0;
		}
	}
#endif

	for_each_port(adapter, i) {
		pi = adap2pinfo(adapter, i);
		pi->sched_tbl = t4_init_sched(adapter->params.nsched_cls);
		if (!pi->sched_tbl)
			dev_warn(&pdev->dev,
				 "could not activate scheduling on port %d\n",
				 i);
	}

	if (tid_init(&adapter->tids) < 0) {
		dev_warn(&pdev->dev, "could not allocate TID table, "
			 "continuing\n");
		adapter->params.offload = 0;
	} else {
		adapter->tc_u32 = cxgb4_init_tc_u32(adapter,
						    CXGB4_MAX_LINK_HANDLE);
		if (!adapter->tc_u32)
			dev_warn(&pdev->dev,
				 "could not offload tc u32, continuing\n");
	}

	if (is_offload(adapter)) {
		if (t4_read_reg(adapter, LE_DB_CONFIG_A) & HASHEN_F) {
			u32 hash_base, hash_reg;

			if (chip <= CHELSIO_T5) {
				hash_reg = LE_DB_TID_HASHBASE_A;
				hash_base = t4_read_reg(adapter, hash_reg);
				adapter->tids.hash_base = hash_base / 4;
			} else {
				hash_reg = T6_LE_DB_HASH_TID_BASE_A;
				hash_base = t4_read_reg(adapter, hash_reg);
				adapter->tids.hash_base = hash_base;
			}
		}
>>>>>>> v4.9.227
	}

	/* See what interrupts we'll be using */
	if (msi > 1 && enable_msix(adapter) == 0)
		adapter->flags |= USING_MSIX;
<<<<<<< HEAD
	else if (msi > 0 && pci_enable_msi(pdev) == 0)
		adapter->flags |= USING_MSI;
=======
	else if (msi > 0 && pci_enable_msi(pdev) == 0) {
		adapter->flags |= USING_MSI;
		if (msi > 1)
			free_msix_info(adapter);
	}

	/* check for PCI Express bandwidth capabiltites */
	cxgb4_check_pcie_caps(adapter);
>>>>>>> v4.9.227

	err = init_rss(adapter);
	if (err)
		goto out_free_dev;

<<<<<<< HEAD
=======
	err = setup_fw_sge_queues(adapter);
	if (err) {
		dev_err(adapter->pdev_dev,
			"FW sge queue allocation failed, err %d", err);
		goto out_free_dev;
	}

>>>>>>> v4.9.227
	/*
	 * The card is now ready to go.  If any errors occur during device
	 * registration we do not fail the whole card but rather proceed only
	 * with the ports we manage to register successfully.  However we must
	 * register at least one net device.
	 */
	for_each_port(adapter, i) {
		pi = adap2pinfo(adapter, i);
<<<<<<< HEAD
=======
		adapter->port[i]->dev_port = pi->lport;
>>>>>>> v4.9.227
		netif_set_real_num_tx_queues(adapter->port[i], pi->nqsets);
		netif_set_real_num_rx_queues(adapter->port[i], pi->nqsets);

		err = register_netdev(adapter->port[i]);
		if (err)
			break;
		adapter->chan_map[pi->tx_chan] = i;
		print_port_info(adapter->port[i]);
	}
	if (i == 0) {
		dev_err(&pdev->dev, "could not register any net devices\n");
		goto out_free_dev;
	}
	if (err) {
		dev_warn(&pdev->dev, "only %d net devices registered\n", i);
		err = 0;
	}

	if (cxgb4_debugfs_root) {
		adapter->debugfs_root = debugfs_create_dir(pci_name(pdev),
							   cxgb4_debugfs_root);
		setup_debugfs(adapter);
	}

	/* PCIe EEH recovery on powerpc platforms needs fundamental reset */
	pdev->needs_freset = 1;

<<<<<<< HEAD
	if (is_offload(adapter))
		attach_ulds(adapter);

sriov:
#ifdef CONFIG_PCI_IOV
	if (func < ARRAY_SIZE(num_vf) && num_vf[func] > 0)
=======
	if (is_uld(adapter)) {
		mutex_lock(&uld_mutex);
		list_add_tail(&adapter->list_node, &adapter_list);
		mutex_unlock(&uld_mutex);
	}

	print_adapter_info(adapter);
	return 0;

sriov:
#ifdef CONFIG_PCI_IOV
	if (func < ARRAY_SIZE(num_vf) && num_vf[func] > 0) {
		dev_warn(&pdev->dev,
			 "Enabling SR-IOV VFs using the num_vf module "
			 "parameter is deprecated - please use the pci sysfs "
			 "interface instead.\n");
>>>>>>> v4.9.227
		if (pci_enable_sriov(pdev, num_vf[func]) == 0)
			dev_info(&pdev->dev,
				 "instantiated %u virtual functions\n",
				 num_vf[func]);
<<<<<<< HEAD
#endif
	return 0;

 out_free_dev:
	free_some_resources(adapter);
=======
	}

	adapter = kzalloc(sizeof(*adapter), GFP_KERNEL);
	if (!adapter) {
		err = -ENOMEM;
		goto free_pci_region;
	}

	adapter->pdev = pdev;
	adapter->pdev_dev = &pdev->dev;
	adapter->name = pci_name(pdev);
	adapter->mbox = func;
	adapter->pf = func;
	adapter->regs = regs;
	adapter->adap_idx = adap_idx;
	adapter->mbox_log = kzalloc(sizeof(*adapter->mbox_log) +
				    (sizeof(struct mbox_cmd) *
				     T4_OS_LOG_MBOX_CMDS),
				    GFP_KERNEL);
	if (!adapter->mbox_log) {
		err = -ENOMEM;
		goto free_adapter;
	}
	pci_set_drvdata(pdev, adapter);
	return 0;

 free_adapter:
	kfree(adapter);
 free_pci_region:
	iounmap(regs);
	pci_disable_sriov(pdev);
	pci_release_regions(pdev);
	return err;
#else
	return 0;
#endif

 out_free_dev:
	t4_free_sge_resources(adapter);
	free_some_resources(adapter);
	if (adapter->flags & USING_MSIX)
		free_msix_info(adapter);
	if (adapter->num_uld || adapter->num_ofld_uld)
		t4_uld_mem_free(adapter);
>>>>>>> v4.9.227
 out_unmap_bar:
	if (!is_t4(adapter->params.chip))
		iounmap(adapter->bar2);
 out_free_adapter:
	if (adapter->workq)
		destroy_workqueue(adapter->workq);

<<<<<<< HEAD
=======
	kfree(adapter->mbox_log);
>>>>>>> v4.9.227
	kfree(adapter);
 out_unmap_bar0:
	iounmap(regs);
 out_disable_device:
	pci_disable_pcie_error_reporting(pdev);
	pci_disable_device(pdev);
 out_release_regions:
	pci_release_regions(pdev);
	return err;
}

static void remove_one(struct pci_dev *pdev)
{
	struct adapter *adapter = pci_get_drvdata(pdev);
<<<<<<< HEAD

#ifdef CONFIG_PCI_IOV
	pci_disable_sriov(pdev);

#endif

	if (adapter) {
=======
	struct hash_mac_addr *entry, *tmp;

	if (!adapter) {
		pci_release_regions(pdev);
		return;
	}

	if (adapter->pf == 4) {
>>>>>>> v4.9.227
		int i;

		/* Tear down per-adapter Work Queue first since it can contain
		 * references to our adapter data structure.
		 */
		destroy_workqueue(adapter->workq);

<<<<<<< HEAD
		if (is_offload(adapter))
			detach_ulds(adapter);

=======
		if (is_uld(adapter))
			detach_ulds(adapter);

		disable_interrupts(adapter);

>>>>>>> v4.9.227
		for_each_port(adapter, i)
			if (adapter->port[i]->reg_state == NETREG_REGISTERED)
				unregister_netdev(adapter->port[i]);

		debugfs_remove_recursive(adapter->debugfs_root);

		/* If we allocated filters, free up state associated with any
		 * valid filters ...
		 */
<<<<<<< HEAD
		if (adapter->tids.ftid_tab) {
			struct filter_entry *f = &adapter->tids.ftid_tab[0];
			for (i = 0; i < (adapter->tids.nftids +
					adapter->tids.nsftids); i++, f++)
				if (f->valid)
					clear_filter(adapter, f);
		}
=======
		clear_all_filters(adapter);
>>>>>>> v4.9.227

		if (adapter->flags & FULL_INIT_DONE)
			cxgb_down(adapter);

<<<<<<< HEAD
		free_some_resources(adapter);
=======
		if (adapter->flags & USING_MSIX)
			free_msix_info(adapter);
		if (adapter->num_uld || adapter->num_ofld_uld)
			t4_uld_mem_free(adapter);
		free_some_resources(adapter);
		list_for_each_entry_safe(entry, tmp, &adapter->mac_hlist,
					 list) {
			list_del(&entry->list);
			kfree(entry);
		}

#if IS_ENABLED(CONFIG_IPV6)
		t4_cleanup_clip_tbl(adapter);
#endif
>>>>>>> v4.9.227
		iounmap(adapter->regs);
		if (!is_t4(adapter->params.chip))
			iounmap(adapter->bar2);
		pci_disable_pcie_error_reporting(pdev);
		if ((adapter->flags & DEV_ENABLED)) {
			pci_disable_device(pdev);
			adapter->flags &= ~DEV_ENABLED;
		}
		pci_release_regions(pdev);
<<<<<<< HEAD
		synchronize_rcu();
		kfree(adapter);
	} else
		pci_release_regions(pdev);
=======
		kfree(adapter->mbox_log);
		synchronize_rcu();
		kfree(adapter);
	}
#ifdef CONFIG_PCI_IOV
	else {
		if (adapter->port[0])
			unregister_netdev(adapter->port[0]);
		iounmap(adapter->regs);
		kfree(adapter->vfinfo);
		kfree(adapter);
		pci_disable_sriov(pdev);
		pci_release_regions(pdev);
	}
#endif
}

/* "Shutdown" quiesces the device, stopping Ingress Packet and Interrupt
 * delivery.  This is essentially a stripped down version of the PCI remove()
 * function where we do the minimal amount of work necessary to shutdown any
 * further activity.
 */
static void shutdown_one(struct pci_dev *pdev)
{
	struct adapter *adapter = pci_get_drvdata(pdev);

	/* As with remove_one() above (see extended comment), we only want do
	 * do cleanup on PCI Devices which went all the way through init_one()
	 * ...
	 */
	if (!adapter) {
		pci_release_regions(pdev);
		return;
	}

	if (adapter->pf == 4) {
		int i;

		for_each_port(adapter, i)
			if (adapter->port[i]->reg_state == NETREG_REGISTERED)
				cxgb_close(adapter->port[i]);

		t4_uld_clean_up(adapter);
		disable_interrupts(adapter);
		disable_msi(adapter);

		t4_sge_stop(adapter);
		if (adapter->flags & FW_OK)
			t4_fw_bye(adapter, adapter->mbox);
	}
#ifdef CONFIG_PCI_IOV
	else {
		if (adapter->port[0])
			unregister_netdev(adapter->port[0]);
		iounmap(adapter->regs);
		kfree(adapter->vfinfo);
		kfree(adapter);
		pci_disable_sriov(pdev);
		pci_release_regions(pdev);
	}
#endif
>>>>>>> v4.9.227
}

static struct pci_driver cxgb4_driver = {
	.name     = KBUILD_MODNAME,
	.id_table = cxgb4_pci_tbl,
	.probe    = init_one,
	.remove   = remove_one,
<<<<<<< HEAD
	.shutdown = remove_one,
=======
	.shutdown = shutdown_one,
#ifdef CONFIG_PCI_IOV
	.sriov_configure = cxgb4_iov_configure,
#endif
>>>>>>> v4.9.227
	.err_handler = &cxgb4_eeh,
};

static int __init cxgb4_init_module(void)
{
	int ret;

	/* Debugfs support is optional, just warn if this fails */
	cxgb4_debugfs_root = debugfs_create_dir(KBUILD_MODNAME, NULL);
	if (!cxgb4_debugfs_root)
		pr_warn("could not create debugfs entry, continuing\n");

	ret = pci_register_driver(&cxgb4_driver);
	if (ret < 0)
<<<<<<< HEAD
		debugfs_remove(cxgb4_debugfs_root);

#if IS_ENABLED(CONFIG_IPV6)
	register_inet6addr_notifier(&cxgb4_inet6addr_notifier);
#endif

=======
		goto err_pci;

#if IS_ENABLED(CONFIG_IPV6)
	if (!inet6addr_registered) {
		ret = register_inet6addr_notifier(&cxgb4_inet6addr_notifier);
		if (ret)
			pci_unregister_driver(&cxgb4_driver);
		else
			inet6addr_registered = true;
	}
#endif

	if (ret == 0)
		return ret;

err_pci:
	debugfs_remove(cxgb4_debugfs_root);

>>>>>>> v4.9.227
	return ret;
}

static void __exit cxgb4_cleanup_module(void)
{
#if IS_ENABLED(CONFIG_IPV6)
<<<<<<< HEAD
	unregister_inet6addr_notifier(&cxgb4_inet6addr_notifier);
=======
	if (inet6addr_registered) {
		unregister_inet6addr_notifier(&cxgb4_inet6addr_notifier);
		inet6addr_registered = false;
	}
>>>>>>> v4.9.227
#endif
	pci_unregister_driver(&cxgb4_driver);
	debugfs_remove(cxgb4_debugfs_root);  /* NULL ok */
}

module_init(cxgb4_init_module);
module_exit(cxgb4_cleanup_module);
