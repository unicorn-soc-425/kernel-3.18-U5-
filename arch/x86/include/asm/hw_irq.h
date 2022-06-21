#ifndef _ASM_X86_HW_IRQ_H
#define _ASM_X86_HW_IRQ_H

/*
 * (C) 1992, 1993 Linus Torvalds, (C) 1997 Ingo Molnar
 *
 * moved some of the old arch/i386/kernel/irq.h to here. VY
 *
 * IRQ/IPI changes taken from work by Thomas Radke
 * <tomsoft@informatik.tu-chemnitz.de>
 *
 * hacked by Andi Kleen for x86-64.
 * unified by tglx
 */

#include <asm/irq_vectors.h>

#ifndef __ASSEMBLY__

#include <linux/percpu.h>
#include <linux/profile.h>
#include <linux/smp.h>

#include <linux/atomic.h>
#include <asm/irq.h>
#include <asm/sections.h>

/* Interrupt handlers registered during init_IRQ */
extern asmlinkage void apic_timer_interrupt(void);
extern asmlinkage void x86_platform_ipi(void);
extern asmlinkage void kvm_posted_intr_ipi(void);
<<<<<<< HEAD
=======
extern asmlinkage void kvm_posted_intr_wakeup_ipi(void);
>>>>>>> v4.9.227
extern asmlinkage void error_interrupt(void);
extern asmlinkage void irq_work_interrupt(void);

extern asmlinkage void spurious_interrupt(void);
extern asmlinkage void thermal_interrupt(void);
extern asmlinkage void reschedule_interrupt(void);

<<<<<<< HEAD
extern asmlinkage void invalidate_interrupt(void);
extern asmlinkage void invalidate_interrupt0(void);
extern asmlinkage void invalidate_interrupt1(void);
extern asmlinkage void invalidate_interrupt2(void);
extern asmlinkage void invalidate_interrupt3(void);
extern asmlinkage void invalidate_interrupt4(void);
extern asmlinkage void invalidate_interrupt5(void);
extern asmlinkage void invalidate_interrupt6(void);
extern asmlinkage void invalidate_interrupt7(void);
extern asmlinkage void invalidate_interrupt8(void);
extern asmlinkage void invalidate_interrupt9(void);
extern asmlinkage void invalidate_interrupt10(void);
extern asmlinkage void invalidate_interrupt11(void);
extern asmlinkage void invalidate_interrupt12(void);
extern asmlinkage void invalidate_interrupt13(void);
extern asmlinkage void invalidate_interrupt14(void);
extern asmlinkage void invalidate_interrupt15(void);
extern asmlinkage void invalidate_interrupt16(void);
extern asmlinkage void invalidate_interrupt17(void);
extern asmlinkage void invalidate_interrupt18(void);
extern asmlinkage void invalidate_interrupt19(void);
extern asmlinkage void invalidate_interrupt20(void);
extern asmlinkage void invalidate_interrupt21(void);
extern asmlinkage void invalidate_interrupt22(void);
extern asmlinkage void invalidate_interrupt23(void);
extern asmlinkage void invalidate_interrupt24(void);
extern asmlinkage void invalidate_interrupt25(void);
extern asmlinkage void invalidate_interrupt26(void);
extern asmlinkage void invalidate_interrupt27(void);
extern asmlinkage void invalidate_interrupt28(void);
extern asmlinkage void invalidate_interrupt29(void);
extern asmlinkage void invalidate_interrupt30(void);
extern asmlinkage void invalidate_interrupt31(void);

extern asmlinkage void irq_move_cleanup_interrupt(void);
extern asmlinkage void reboot_interrupt(void);
extern asmlinkage void threshold_interrupt(void);
=======
extern asmlinkage void irq_move_cleanup_interrupt(void);
extern asmlinkage void reboot_interrupt(void);
extern asmlinkage void threshold_interrupt(void);
extern asmlinkage void deferred_error_interrupt(void);
>>>>>>> v4.9.227

extern asmlinkage void call_function_interrupt(void);
extern asmlinkage void call_function_single_interrupt(void);

#ifdef CONFIG_TRACING
/* Interrupt handlers registered during init_IRQ */
extern void trace_apic_timer_interrupt(void);
extern void trace_x86_platform_ipi(void);
extern void trace_error_interrupt(void);
extern void trace_irq_work_interrupt(void);
extern void trace_spurious_interrupt(void);
extern void trace_thermal_interrupt(void);
extern void trace_reschedule_interrupt(void);
extern void trace_threshold_interrupt(void);
<<<<<<< HEAD
=======
extern void trace_deferred_error_interrupt(void);
>>>>>>> v4.9.227
extern void trace_call_function_interrupt(void);
extern void trace_call_function_single_interrupt(void);
#define trace_irq_move_cleanup_interrupt  irq_move_cleanup_interrupt
#define trace_reboot_interrupt  reboot_interrupt
#define trace_kvm_posted_intr_ipi kvm_posted_intr_ipi
<<<<<<< HEAD
#endif /* CONFIG_TRACING */

/* IOAPIC */
#define IO_APIC_IRQ(x) (((x) >= NR_IRQS_LEGACY) || ((1<<(x)) & io_apic_irqs))
extern unsigned long io_apic_irqs;

extern void setup_IO_APIC(void);
extern void disable_IO_APIC(void);

struct io_apic_irq_attr {
	int ioapic;
	int ioapic_pin;
	int trigger;
	int polarity;
};

static inline void set_io_apic_irq_attr(struct io_apic_irq_attr *irq_attr,
					int ioapic, int ioapic_pin,
					int trigger, int polarity)
{
	irq_attr->ioapic	= ioapic;
	irq_attr->ioapic_pin	= ioapic_pin;
	irq_attr->trigger	= trigger;
	irq_attr->polarity	= polarity;
}

/* Intel specific interrupt remapping information */
struct irq_2_iommu {
	struct intel_iommu *iommu;
	u16 irte_index;
	u16 sub_handle;
	u8  irte_mask;
};

/* AMD specific interrupt remapping information */
struct irq_2_irte {
	u16 devid; /* Device ID for IRTE table */
	u16 index; /* Index into IRTE table*/
};

/*
 * This is performance-critical, we want to do it O(1)
 *
 * Most irqs are mapped 1:1 with pins.
 */
struct irq_cfg {
	struct irq_pin_list	*irq_2_pin;
	cpumask_var_t		domain;
	cpumask_var_t		old_domain;
	u8			vector;
	u8			move_in_progress : 1;
#ifdef CONFIG_IRQ_REMAP
	u8			remapped : 1;
	union {
		struct irq_2_iommu irq_2_iommu;
		struct irq_2_irte  irq_2_irte;
	};
#endif
};

extern int assign_irq_vector(int, struct irq_cfg *, const struct cpumask *);
extern void send_cleanup_vector(struct irq_cfg *);

struct irq_data;
int __ioapic_set_affinity(struct irq_data *, const struct cpumask *,
			  unsigned int *dest_id);
extern int IO_APIC_get_PCI_irq_vector(int bus, int devfn, int pin, struct io_apic_irq_attr *irq_attr);
extern void setup_ioapic_dest(void);

extern void enable_IO_APIC(void);
=======
#define trace_kvm_posted_intr_wakeup_ipi kvm_posted_intr_wakeup_ipi
#endif /* CONFIG_TRACING */

#ifdef	CONFIG_X86_LOCAL_APIC
struct irq_data;
struct pci_dev;
struct msi_desc;

enum irq_alloc_type {
	X86_IRQ_ALLOC_TYPE_IOAPIC = 1,
	X86_IRQ_ALLOC_TYPE_HPET,
	X86_IRQ_ALLOC_TYPE_MSI,
	X86_IRQ_ALLOC_TYPE_MSIX,
	X86_IRQ_ALLOC_TYPE_DMAR,
	X86_IRQ_ALLOC_TYPE_UV,
};

struct irq_alloc_info {
	enum irq_alloc_type	type;
	u32			flags;
	const struct cpumask	*mask;	/* CPU mask for vector allocation */
	union {
		int		unused;
#ifdef	CONFIG_HPET_TIMER
		struct {
			int		hpet_id;
			int		hpet_index;
			void		*hpet_data;
		};
#endif
#ifdef	CONFIG_PCI_MSI
		struct {
			struct pci_dev	*msi_dev;
			irq_hw_number_t	msi_hwirq;
		};
#endif
#ifdef	CONFIG_X86_IO_APIC
		struct {
			int		ioapic_id;
			int		ioapic_pin;
			int		ioapic_node;
			u32		ioapic_trigger : 1;
			u32		ioapic_polarity : 1;
			u32		ioapic_valid : 1;
			struct IO_APIC_route_entry *ioapic_entry;
		};
#endif
#ifdef	CONFIG_DMAR_TABLE
		struct {
			int		dmar_id;
			void		*dmar_data;
		};
#endif
#ifdef	CONFIG_HT_IRQ
		struct {
			int		ht_pos;
			int		ht_idx;
			struct pci_dev	*ht_dev;
			void		*ht_update;
		};
#endif
#ifdef	CONFIG_X86_UV
		struct {
			int		uv_limit;
			int		uv_blade;
			unsigned long	uv_offset;
			char		*uv_name;
		};
#endif
#if IS_ENABLED(CONFIG_VMD)
		struct {
			struct msi_desc *desc;
		};
#endif
	};
};

struct irq_cfg {
	unsigned int		dest_apicid;
	u8			vector;
	u8			old_vector;
};

extern struct irq_cfg *irq_cfg(unsigned int irq);
extern struct irq_cfg *irqd_cfg(struct irq_data *irq_data);
extern void lock_vector_lock(void);
extern void unlock_vector_lock(void);
extern void setup_vector_irq(int cpu);
#ifdef CONFIG_SMP
extern void send_cleanup_vector(struct irq_cfg *);
extern void irq_complete_move(struct irq_cfg *cfg);
#else
static inline void send_cleanup_vector(struct irq_cfg *c) { }
static inline void irq_complete_move(struct irq_cfg *c) { }
#endif

extern void apic_ack_edge(struct irq_data *data);
#else	/*  CONFIG_X86_LOCAL_APIC */
static inline void lock_vector_lock(void) {}
static inline void unlock_vector_lock(void) {}
#endif	/* CONFIG_X86_LOCAL_APIC */
>>>>>>> v4.9.227

/* Statistics */
extern atomic_t irq_err_count;
extern atomic_t irq_mis_count;

<<<<<<< HEAD
/* EISA */
extern void eisa_set_level_irq(unsigned int irq);

/* SMP */
extern __visible void smp_apic_timer_interrupt(struct pt_regs *);
extern __visible void smp_spurious_interrupt(struct pt_regs *);
extern __visible void smp_x86_platform_ipi(struct pt_regs *);
extern __visible void smp_error_interrupt(struct pt_regs *);
#ifdef CONFIG_X86_IO_APIC
extern asmlinkage void smp_irq_move_cleanup_interrupt(void);
#endif
#ifdef CONFIG_SMP
extern __visible void smp_reschedule_interrupt(struct pt_regs *);
extern __visible void smp_call_function_interrupt(struct pt_regs *);
extern __visible void smp_call_function_single_interrupt(struct pt_regs *);
extern __visible void smp_invalidate_interrupt(struct pt_regs *);
#endif

extern void (*__initconst interrupt[NR_VECTORS-FIRST_EXTERNAL_VECTOR])(void);
#ifdef CONFIG_TRACING
#define trace_interrupt interrupt
#endif

#define VECTOR_UNDEFINED	(-1)
#define VECTOR_RETRIGGERED	(-2)

typedef int vector_irq_t[NR_VECTORS];
DECLARE_PER_CPU(vector_irq_t, vector_irq);
extern void setup_vector_irq(int cpu);

#ifdef CONFIG_X86_IO_APIC
extern void lock_vector_lock(void);
extern void unlock_vector_lock(void);
extern void __setup_vector_irq(int cpu);
#else
static inline void lock_vector_lock(void) {}
static inline void unlock_vector_lock(void) {}
static inline void __setup_vector_irq(int cpu) {}
#endif
=======
extern void elcr_set_level_irq(unsigned int irq);

extern char irq_entries_start[];
#ifdef CONFIG_TRACING
#define trace_irq_entries_start irq_entries_start
#endif

#define VECTOR_UNUSED		NULL
#define VECTOR_RETRIGGERED	((void *)~0UL)

typedef struct irq_desc* vector_irq_t[NR_VECTORS];
DECLARE_PER_CPU_USER_MAPPED(vector_irq_t, vector_irq);
>>>>>>> v4.9.227

#endif /* !ASSEMBLY_ */

#endif /* _ASM_X86_HW_IRQ_H */
