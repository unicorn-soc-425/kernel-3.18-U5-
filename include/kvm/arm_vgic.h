/*
<<<<<<< HEAD
 * Copyright (C) 2012 ARM Ltd.
 * Author: Marc Zyngier <marc.zyngier@arm.com>
=======
 * Copyright (C) 2015, 2016 ARM Ltd.
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
<<<<<<< HEAD
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __ASM_ARM_KVM_VGIC_H
#define __ASM_ARM_KVM_VGIC_H
=======
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __KVM_ARM_VGIC_H
#define __KVM_ARM_VGIC_H
>>>>>>> v4.9.227

#include <linux/kernel.h>
#include <linux/kvm.h>
#include <linux/irqreturn.h>
#include <linux/spinlock.h>
<<<<<<< HEAD
#include <linux/types.h>

#define VGIC_NR_IRQS_LEGACY	256
#define VGIC_NR_SGIS		16
#define VGIC_NR_PPIS		16
#define VGIC_NR_PRIVATE_IRQS	(VGIC_NR_SGIS + VGIC_NR_PPIS)

#define VGIC_V2_MAX_LRS		(1 << 6)
#define VGIC_V3_MAX_LRS		16
#define VGIC_MAX_IRQS		1024

/* Sanity checks... */
#if (KVM_MAX_VCPUS > 8)
#error	Invalid number of CPU interfaces
#endif

#if (VGIC_NR_IRQS_LEGACY & 31)
#error "VGIC_NR_IRQS must be a multiple of 32"
#endif

#if (VGIC_NR_IRQS_LEGACY > VGIC_MAX_IRQS)
#error "VGIC_NR_IRQS must be <= 1024"
#endif

/*
 * The GIC distributor registers describing interrupts have two parts:
 * - 32 per-CPU interrupts (SGI + PPI)
 * - a bunch of shared interrupts (SPI)
 */
struct vgic_bitmap {
	/*
	 * - One UL per VCPU for private interrupts (assumes UL is at
	 *   least 32 bits)
	 * - As many UL as necessary for shared interrupts.
	 *
	 * The private interrupts are accessed via the "private"
	 * field, one UL per vcpu (the state for vcpu n is in
	 * private[n]). The shared interrupts are accessed via the
	 * "shared" pointer (IRQn state is at bit n-32 in the bitmap).
	 */
	unsigned long *private;
	unsigned long *shared;
};

struct vgic_bytemap {
	/*
	 * - 8 u32 per VCPU for private interrupts
	 * - As many u32 as necessary for shared interrupts.
	 *
	 * The private interrupts are accessed via the "private"
	 * field, (the state for vcpu n is in private[n*8] to
	 * private[n*8 + 7]). The shared interrupts are accessed via
	 * the "shared" pointer (IRQn state is at byte (n-32)%4 of the
	 * shared[(n-32)/4] word).
	 */
	u32 *private;
	u32 *shared;
};

struct kvm_vcpu;
=======
#include <linux/static_key.h>
#include <linux/types.h>
#include <kvm/iodev.h>
#include <linux/list.h>
#include <linux/jump_label.h>

#define VGIC_V3_MAX_CPUS	255
#define VGIC_V2_MAX_CPUS	8
#define VGIC_NR_IRQS_LEGACY     256
#define VGIC_NR_SGIS		16
#define VGIC_NR_PPIS		16
#define VGIC_NR_PRIVATE_IRQS	(VGIC_NR_SGIS + VGIC_NR_PPIS)
#define VGIC_MAX_PRIVATE	(VGIC_NR_PRIVATE_IRQS - 1)
#define VGIC_MAX_SPI		1019
#define VGIC_MAX_RESERVED	1023
#define VGIC_MIN_LPI		8192
#define KVM_IRQCHIP_NUM_PINS	(1020 - 32)
>>>>>>> v4.9.227

enum vgic_type {
	VGIC_V2,		/* Good ol' GICv2 */
	VGIC_V3,		/* New fancy GICv3 */
};

<<<<<<< HEAD
#define LR_STATE_PENDING	(1 << 0)
#define LR_STATE_ACTIVE		(1 << 1)
#define LR_STATE_MASK		(3 << 0)
#define LR_EOI_INT		(1 << 2)

struct vgic_lr {
	u16	irq;
	u8	source;
	u8	state;
};

struct vgic_vmcr {
	u32	ctlr;
	u32	abpr;
	u32	bpr;
	u32	pmr;
};

struct vgic_ops {
	struct vgic_lr	(*get_lr)(const struct kvm_vcpu *, int);
	void	(*set_lr)(struct kvm_vcpu *, int, struct vgic_lr);
	void	(*sync_lr_elrsr)(struct kvm_vcpu *, int, struct vgic_lr);
	u64	(*get_elrsr)(const struct kvm_vcpu *vcpu);
	u64	(*get_eisr)(const struct kvm_vcpu *vcpu);
	void	(*clear_eisr)(struct kvm_vcpu *vcpu);
	u32	(*get_interrupt_status)(const struct kvm_vcpu *vcpu);
	void	(*enable_underflow)(struct kvm_vcpu *vcpu);
	void	(*disable_underflow)(struct kvm_vcpu *vcpu);
	void	(*get_vmcr)(struct kvm_vcpu *vcpu, struct vgic_vmcr *vmcr);
	void	(*set_vmcr)(struct kvm_vcpu *vcpu, struct vgic_vmcr *vmcr);
	void	(*enable)(struct kvm_vcpu *vcpu);
};

struct vgic_params {
	/* vgic type */
	enum vgic_type	type;
	/* Physical address of vgic virtual cpu interface */
	phys_addr_t	vcpu_base;
	/* Number of list registers */
	u32		nr_lr;
	/* Interrupt number */
	unsigned int	maint_irq;
	/* Virtual control interface base address */
	void __iomem	*vctrl_base;
};

struct vgic_dist {
#ifdef CONFIG_KVM_ARM_VGIC
	spinlock_t		lock;
	bool			in_kernel;
	bool			ready;

	int			nr_cpus;
	int			nr_irqs;

	/* Virtual control interface mapping */
	void __iomem		*vctrl_base;

	/* Distributor and vcpu interface mapping in the guest */
	phys_addr_t		vgic_dist_base;
	phys_addr_t		vgic_cpu_base;

	/* Distributor enabled */
	u32			enabled;

	/* Interrupt enabled (one bit per IRQ) */
	struct vgic_bitmap	irq_enabled;

	/* Level-triggered interrupt external input is asserted */
	struct vgic_bitmap	irq_level;

	/*
	 * Interrupt state is pending on the distributor
	 */
	struct vgic_bitmap	irq_pending;

	/*
	 * Tracks writes to GICD_ISPENDRn and GICD_ICPENDRn for level-triggered
	 * interrupts.  Essentially holds the state of the flip-flop in
	 * Figure 4-10 on page 4-101 in ARM IHI 0048B.b.
	 * Once set, it is only cleared for level-triggered interrupts on
	 * guest ACKs (when we queue it) or writes to GICD_ICPENDRn.
	 */
	struct vgic_bitmap	irq_soft_pend;

	/* Level-triggered interrupt queued on VCPU interface */
	struct vgic_bitmap	irq_queued;

	/* Interrupt priority. Not used yet. */
	struct vgic_bytemap	irq_priority;

	/* Level/edge triggered */
	struct vgic_bitmap	irq_cfg;

	/*
	 * Source CPU per SGI and target CPU:
	 *
	 * Each byte represent a SGI observable on a VCPU, each bit of
	 * this byte indicating if the corresponding VCPU has
	 * generated this interrupt. This is a GICv2 feature only.
	 *
	 * For VCPUn (n < 8), irq_sgi_sources[n*16] to [n*16 + 15] are
	 * the SGIs observable on VCPUn.
	 */
	u8			*irq_sgi_sources;

	/*
	 * Target CPU for each SPI:
	 *
	 * Array of available SPI, each byte indicating the target
	 * VCPU for SPI. IRQn (n >=32) is at irq_spi_cpu[n-32].
	 */
	u8			*irq_spi_cpu;

	/*
	 * Reverse lookup of irq_spi_cpu for faster compute pending:
	 *
	 * Array of bitmaps, one per VCPU, describing if IRQn is
	 * routed to a particular VCPU.
	 */
	struct vgic_bitmap	*irq_spi_target;

	/* Bitmap indicating which CPU has something pending */
	unsigned long		*irq_pending_on_cpu;
#endif
=======
/* same for all guests, as depending only on the _host's_ GIC model */
struct vgic_global {
	/* type of the host GIC */
	enum vgic_type		type;

	/* Physical address of vgic virtual cpu interface */
	phys_addr_t		vcpu_base;

	/* GICV mapping */
	void __iomem		*vcpu_base_va;

	/* virtual control interface mapping */
	void __iomem		*vctrl_base;

	/* Number of implemented list registers */
	int			nr_lr;

	/* Maintenance IRQ number */
	unsigned int		maint_irq;

	/* maximum number of VCPUs allowed (GICv2 limits us to 8) */
	int			max_gic_vcpus;

	/* Only needed for the legacy KVM_CREATE_IRQCHIP */
	bool			can_emulate_gicv2;

	/* GIC system register CPU interface */
	struct static_key_false gicv3_cpuif;
};

extern struct vgic_global kvm_vgic_global_state;

#define VGIC_V2_MAX_LRS		(1 << 6)
#define VGIC_V3_MAX_LRS		16
#define VGIC_V3_LR_INDEX(lr)	(VGIC_V3_MAX_LRS - 1 - lr)

enum vgic_irq_config {
	VGIC_CONFIG_EDGE = 0,
	VGIC_CONFIG_LEVEL
};

struct vgic_irq {
	spinlock_t irq_lock;		/* Protects the content of the struct */
	struct list_head lpi_list;	/* Used to link all LPIs together */
	struct list_head ap_list;

	struct kvm_vcpu *vcpu;		/* SGIs and PPIs: The VCPU
					 * SPIs and LPIs: The VCPU whose ap_list
					 * this is queued on.
					 */

	struct kvm_vcpu *target_vcpu;	/* The VCPU that this interrupt should
					 * be sent to, as a result of the
					 * targets reg (v2) or the
					 * affinity reg (v3).
					 */

	u32 intid;			/* Guest visible INTID */
	bool pending;
	bool line_level;		/* Level only */
	bool soft_pending;		/* Level only */
	bool active;			/* not used for LPIs */
	bool enabled;
	bool hw;			/* Tied to HW IRQ */
	struct kref refcount;		/* Used for LPIs */
	u32 hwintid;			/* HW INTID number */
	union {
		u8 targets;			/* GICv2 target VCPUs mask */
		u32 mpidr;			/* GICv3 target VCPU */
	};
	u8 source;			/* GICv2 SGIs only */
	u8 priority;
	enum vgic_irq_config config;	/* Level or edge */
};

struct vgic_register_region;
struct vgic_its;

enum iodev_type {
	IODEV_CPUIF,
	IODEV_DIST,
	IODEV_REDIST,
	IODEV_ITS
};

struct vgic_io_device {
	gpa_t base_addr;
	union {
		struct kvm_vcpu *redist_vcpu;
		struct vgic_its *its;
	};
	const struct vgic_register_region *regions;
	enum iodev_type iodev_type;
	int nr_regions;
	struct kvm_io_device dev;
};

struct vgic_its {
	/* The base address of the ITS control register frame */
	gpa_t			vgic_its_base;

	bool			enabled;
	bool			initialized;
	struct vgic_io_device	iodev;
	struct kvm_device	*dev;

	/* These registers correspond to GITS_BASER{0,1} */
	u64			baser_device_table;
	u64			baser_coll_table;

	/* Protects the command queue */
	struct mutex		cmd_lock;
	u64			cbaser;
	u32			creadr;
	u32			cwriter;

	/* Protects the device and collection lists */
	struct mutex		its_lock;
	struct list_head	device_list;
	struct list_head	collection_list;
};

struct vgic_dist {
	bool			in_kernel;
	bool			ready;
	bool			initialized;

	/* vGIC model the kernel emulates for the guest (GICv2 or GICv3) */
	u32			vgic_model;

	/* Do injected MSIs require an additional device ID? */
	bool			msis_require_devid;

	int			nr_spis;

	/* TODO: Consider moving to global state */
	/* Virtual control interface mapping */
	void __iomem		*vctrl_base;

	/* base addresses in guest physical address space: */
	gpa_t			vgic_dist_base;		/* distributor */
	union {
		/* either a GICv2 CPU interface */
		gpa_t			vgic_cpu_base;
		/* or a number of GICv3 redistributor regions */
		gpa_t			vgic_redist_base;
	};

	/* distributor enabled */
	bool			enabled;

	struct vgic_irq		*spis;

	struct vgic_io_device	dist_iodev;

	bool			has_its;

	/*
	 * Contains the attributes and gpa of the LPI configuration table.
	 * Since we report GICR_TYPER.CommonLPIAff as 0b00, we can share
	 * one address across all redistributors.
	 * GICv3 spec: 6.1.2 "LPI Configuration tables"
	 */
	u64			propbaser;

	/* Protects the lpi_list and the count value below. */
	spinlock_t		lpi_list_lock;
	struct list_head	lpi_list_head;
	int			lpi_list_count;
>>>>>>> v4.9.227
};

struct vgic_v2_cpu_if {
	u32		vgic_hcr;
	u32		vgic_vmcr;
	u32		vgic_misr;	/* Saved only */
	u64		vgic_eisr;	/* Saved only */
	u64		vgic_elrsr;	/* Saved only */
	u32		vgic_apr;
	u32		vgic_lr[VGIC_V2_MAX_LRS];
};

struct vgic_v3_cpu_if {
<<<<<<< HEAD
#ifdef CONFIG_ARM_GIC_V3
	u32		vgic_hcr;
	u32		vgic_vmcr;
=======
	u32		vgic_hcr;
	u32		vgic_vmcr;
	u32		vgic_sre;	/* Restored only, change ignored */
>>>>>>> v4.9.227
	u32		vgic_misr;	/* Saved only */
	u32		vgic_eisr;	/* Saved only */
	u32		vgic_elrsr;	/* Saved only */
	u32		vgic_ap0r[4];
	u32		vgic_ap1r[4];
	u64		vgic_lr[VGIC_V3_MAX_LRS];
<<<<<<< HEAD
#endif
};

struct vgic_cpu {
#ifdef CONFIG_KVM_ARM_VGIC
	/* per IRQ to LR mapping */
	u8		*vgic_irq_lr_map;

	/* Pending interrupts on this VCPU */
	DECLARE_BITMAP(	pending_percpu, VGIC_NR_PRIVATE_IRQS);
	unsigned long	*pending_shared;

	/* Bitmap of used/free list registers */
	DECLARE_BITMAP(	lr_used, VGIC_V2_MAX_LRS);

	/* Number of list registers on this CPU */
	int		nr_lr;

=======
};

struct vgic_cpu {
>>>>>>> v4.9.227
	/* CPU vif control registers for world switch */
	union {
		struct vgic_v2_cpu_if	vgic_v2;
		struct vgic_v3_cpu_if	vgic_v3;
	};
<<<<<<< HEAD
#endif
};

#define LR_EMPTY	0xff

#define INT_STATUS_EOI		(1 << 0)
#define INT_STATUS_UNDERFLOW	(1 << 1)

struct kvm;
struct kvm_vcpu;
struct kvm_run;
struct kvm_exit_mmio;

#ifdef CONFIG_KVM_ARM_VGIC
int kvm_vgic_addr(struct kvm *kvm, unsigned long type, u64 *addr, bool write);
int kvm_vgic_hyp_init(void);
int kvm_vgic_map_resources(struct kvm *kvm);
int kvm_vgic_create(struct kvm *kvm);
void kvm_vgic_destroy(struct kvm *kvm);
void kvm_vgic_vcpu_destroy(struct kvm_vcpu *vcpu);
void kvm_vgic_flush_hwstate(struct kvm_vcpu *vcpu);
void kvm_vgic_sync_hwstate(struct kvm_vcpu *vcpu);
int kvm_vgic_inject_irq(struct kvm *kvm, int cpuid, unsigned int irq_num,
			bool level);
int kvm_vgic_vcpu_pending_irq(struct kvm_vcpu *vcpu);
bool vgic_handle_mmio(struct kvm_vcpu *vcpu, struct kvm_run *run,
		      struct kvm_exit_mmio *mmio);

#define irqchip_in_kernel(k)	(!!((k)->arch.vgic.in_kernel))
#define vgic_initialized(k)	((k)->arch.vgic.ready)

int vgic_v2_probe(struct device_node *vgic_node,
		  const struct vgic_ops **ops,
		  const struct vgic_params **params);
#ifdef CONFIG_ARM_GIC_V3
int vgic_v3_probe(struct device_node *vgic_node,
		  const struct vgic_ops **ops,
		  const struct vgic_params **params);
#else
static inline int vgic_v3_probe(struct device_node *vgic_node,
				const struct vgic_ops **ops,
				const struct vgic_params **params)
{
	return -ENODEV;
}
#endif

#else
static inline int kvm_vgic_hyp_init(void)
{
	return 0;
}

static inline int kvm_vgic_set_addr(struct kvm *kvm, unsigned long type, u64 addr)
{
	return 0;
}

static inline int kvm_vgic_addr(struct kvm *kvm, unsigned long type, u64 *addr, bool write)
{
	return -ENXIO;
}

static inline int kvm_vgic_map_resources(struct kvm *kvm)
{
	return 0;
}

static inline int kvm_vgic_create(struct kvm *kvm)
{
	return 0;
}

static inline void kvm_vgic_destroy(struct kvm *kvm)
{
}

static inline void kvm_vgic_vcpu_destroy(struct kvm_vcpu *vcpu)
{
}

static inline int kvm_vgic_vcpu_init(struct kvm_vcpu *vcpu)
{
	return 0;
}

static inline void kvm_vgic_flush_hwstate(struct kvm_vcpu *vcpu) {}
static inline void kvm_vgic_sync_hwstate(struct kvm_vcpu *vcpu) {}

static inline int kvm_vgic_inject_irq(struct kvm *kvm, int cpuid,
				      unsigned int irq_num, bool level)
{
	return 0;
}

static inline int kvm_vgic_vcpu_pending_irq(struct kvm_vcpu *vcpu)
{
	return 0;
}

static inline bool vgic_handle_mmio(struct kvm_vcpu *vcpu, struct kvm_run *run,
				    struct kvm_exit_mmio *mmio)
{
	return false;
}

static inline int irqchip_in_kernel(struct kvm *kvm)
{
	return 0;
}

static inline bool vgic_initialized(struct kvm *kvm)
{
	return true;
}
#endif

#endif
=======

	unsigned int used_lrs;
	struct vgic_irq private_irqs[VGIC_NR_PRIVATE_IRQS];

	spinlock_t ap_list_lock;	/* Protects the ap_list */

	/*
	 * List of IRQs that this VCPU should consider because they are either
	 * Active or Pending (hence the name; AP list), or because they recently
	 * were one of the two and need to be migrated off this list to another
	 * VCPU.
	 */
	struct list_head ap_list_head;

	u64 live_lrs;

	/*
	 * Members below are used with GICv3 emulation only and represent
	 * parts of the redistributor.
	 */
	struct vgic_io_device	rd_iodev;
	struct vgic_io_device	sgi_iodev;

	/* Contains the attributes and gpa of the LPI pending tables. */
	u64 pendbaser;

	bool lpis_enabled;
};

extern struct static_key_false vgic_v2_cpuif_trap;

int kvm_vgic_addr(struct kvm *kvm, unsigned long type, u64 *addr, bool write);
void kvm_vgic_early_init(struct kvm *kvm);
int kvm_vgic_create(struct kvm *kvm, u32 type);
void kvm_vgic_destroy(struct kvm *kvm);
void kvm_vgic_vcpu_early_init(struct kvm_vcpu *vcpu);
void kvm_vgic_vcpu_destroy(struct kvm_vcpu *vcpu);
int kvm_vgic_map_resources(struct kvm *kvm);
int kvm_vgic_hyp_init(void);

int kvm_vgic_inject_irq(struct kvm *kvm, int cpuid, unsigned int intid,
			bool level);
int kvm_vgic_inject_mapped_irq(struct kvm *kvm, int cpuid, unsigned int intid,
			       bool level);
int kvm_vgic_map_phys_irq(struct kvm_vcpu *vcpu, u32 virt_irq, u32 phys_irq);
int kvm_vgic_unmap_phys_irq(struct kvm_vcpu *vcpu, unsigned int virt_irq);
bool kvm_vgic_map_is_active(struct kvm_vcpu *vcpu, unsigned int virt_irq);

int kvm_vgic_vcpu_pending_irq(struct kvm_vcpu *vcpu);

#define irqchip_in_kernel(k)	(!!((k)->arch.vgic.in_kernel))
#define vgic_initialized(k)	((k)->arch.vgic.initialized)
#define vgic_ready(k)		((k)->arch.vgic.ready)
#define vgic_valid_spi(k, i)	(((i) >= VGIC_NR_PRIVATE_IRQS) && \
			((i) < (k)->arch.vgic.nr_spis + VGIC_NR_PRIVATE_IRQS))

bool kvm_vcpu_has_pending_irqs(struct kvm_vcpu *vcpu);
void kvm_vgic_sync_hwstate(struct kvm_vcpu *vcpu);
void kvm_vgic_flush_hwstate(struct kvm_vcpu *vcpu);

void vgic_v3_dispatch_sgi(struct kvm_vcpu *vcpu, u64 reg);

/**
 * kvm_vgic_get_max_vcpus - Get the maximum number of VCPUs allowed by HW
 *
 * The host's GIC naturally limits the maximum amount of VCPUs a guest
 * can use.
 */
static inline int kvm_vgic_get_max_vcpus(void)
{
	return kvm_vgic_global_state.max_gic_vcpus;
}

int kvm_send_userspace_msi(struct kvm *kvm, struct kvm_msi *msi);

/**
 * kvm_vgic_setup_default_irq_routing:
 * Setup a default flat gsi routing table mapping all SPIs
 */
int kvm_vgic_setup_default_irq_routing(struct kvm *kvm);

#endif /* __KVM_ARM_VGIC_H */
>>>>>>> v4.9.227
