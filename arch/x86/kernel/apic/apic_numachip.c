/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Numascale NumaConnect-Specific APIC Code
 *
 * Copyright (C) 2011 Numascale AS. All rights reserved.
 *
 * Send feedback to <support@numascale.com>
 *
 */

<<<<<<< HEAD
#include <linux/errno.h>
#include <linux/threads.h>
#include <linux/cpumask.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/hardirq.h>
#include <linux/delay.h>

#include <asm/numachip/numachip.h>
#include <asm/numachip/numachip_csr.h>
#include <asm/smp.h>
#include <asm/apic.h>
#include <asm/ipi.h>
#include <asm/apic_flat_64.h>
#include <asm/pgtable.h>

static int numachip_system __read_mostly;

static const struct apic apic_numachip;

static unsigned int get_apic_id(unsigned long x)
{
	unsigned long value;
	unsigned int id;

	rdmsrl(MSR_FAM10H_NODE_ID, value);
	id = ((x >> 24) & 0xffU) | ((value << 2) & 0xff00U);
=======
#include <linux/init.h>

#include <asm/numachip/numachip.h>
#include <asm/numachip/numachip_csr.h>
#include <asm/ipi.h>
#include <asm/apic_flat_64.h>
#include <asm/pgtable.h>
#include <asm/pci_x86.h>

u8 numachip_system __read_mostly;
static const struct apic apic_numachip1;
static const struct apic apic_numachip2;
static void (*numachip_apic_icr_write)(int apicid, unsigned int val) __read_mostly;

static unsigned int numachip1_get_apic_id(unsigned long x)
{
	unsigned long value;
	unsigned int id = (x >> 24) & 0xff;

	if (static_cpu_has(X86_FEATURE_NODEID_MSR)) {
		rdmsrl(MSR_FAM10H_NODE_ID, value);
		id |= (value << 2) & 0xff00;
	}
>>>>>>> v4.9.227

	return id;
}

<<<<<<< HEAD
static unsigned long set_apic_id(unsigned int id)
{
	unsigned long x;

	x = ((id & 0xffU) << 24);
	return x;
}

static unsigned int read_xapic_id(void)
{
	return get_apic_id(apic_read(APIC_ID));
=======
static unsigned long numachip1_set_apic_id(unsigned int id)
{
	return (id & 0xff) << 24;
}

static unsigned int numachip2_get_apic_id(unsigned long x)
{
	u64 mcfg;

	rdmsrl(MSR_FAM10H_MMIO_CONF_BASE, mcfg);
	return ((mcfg >> (28 - 8)) & 0xfff00) | (x >> 24);
}

static unsigned long numachip2_set_apic_id(unsigned int id)
{
	return id << 24;
>>>>>>> v4.9.227
}

static int numachip_apic_id_valid(int apicid)
{
	/* Trust what bootloader passes in MADT */
	return 1;
}

static int numachip_apic_id_registered(void)
{
<<<<<<< HEAD
	return physid_isset(read_xapic_id(), phys_cpu_present_map);
=======
	return 1;
>>>>>>> v4.9.227
}

static int numachip_phys_pkg_id(int initial_apic_id, int index_msb)
{
	return initial_apic_id >> index_msb;
}

<<<<<<< HEAD
static int numachip_wakeup_secondary(int phys_apicid, unsigned long start_rip)
{
	union numachip_csr_g3_ext_irq_gen int_gen;

	int_gen.s._destination_apic_id = phys_apicid;
	int_gen.s._vector = 0;
	int_gen.s._msgtype = APIC_DM_INIT >> 8;
	int_gen.s._index = 0;

	write_lcsr(CSR_G3_EXT_IRQ_GEN, int_gen.v);

	int_gen.s._msgtype = APIC_DM_STARTUP >> 8;
	int_gen.s._vector = start_rip >> 12;

	write_lcsr(CSR_G3_EXT_IRQ_GEN, int_gen.v);

	atomic_set(&init_deasserted, 1);
=======
static void numachip1_apic_icr_write(int apicid, unsigned int val)
{
	write_lcsr(CSR_G3_EXT_IRQ_GEN, (apicid << 16) | val);
}

static void numachip2_apic_icr_write(int apicid, unsigned int val)
{
	numachip2_write32_lcsr(NUMACHIP2_APIC_ICR, (apicid << 12) | val);
}

static int numachip_wakeup_secondary(int phys_apicid, unsigned long start_rip)
{
	numachip_apic_icr_write(phys_apicid, APIC_DM_INIT);
	numachip_apic_icr_write(phys_apicid, APIC_DM_STARTUP |
		(start_rip >> 12));

>>>>>>> v4.9.227
	return 0;
}

static void numachip_send_IPI_one(int cpu, int vector)
{
<<<<<<< HEAD
	union numachip_csr_g3_ext_irq_gen int_gen;
	int apicid = per_cpu(x86_cpu_to_apicid, cpu);

	int_gen.s._destination_apic_id = apicid;
	int_gen.s._vector = vector;
	int_gen.s._msgtype = (vector == NMI_VECTOR ? APIC_DM_NMI : APIC_DM_FIXED) >> 8;
	int_gen.s._index = 0;

	write_lcsr(CSR_G3_EXT_IRQ_GEN, int_gen.v);
=======
	int local_apicid, apicid = per_cpu(x86_cpu_to_apicid, cpu);
	unsigned int dmode;

	preempt_disable();
	local_apicid = __this_cpu_read(x86_cpu_to_apicid);

	/* Send via local APIC where non-local part matches */
	if (!((apicid ^ local_apicid) >> NUMACHIP_LAPIC_BITS)) {
		unsigned long flags;

		local_irq_save(flags);
		__default_send_IPI_dest_field(apicid, vector,
			APIC_DEST_PHYSICAL);
		local_irq_restore(flags);
		preempt_enable();
		return;
	}
	preempt_enable();

	dmode = (vector == NMI_VECTOR) ? APIC_DM_NMI : APIC_DM_FIXED;
	numachip_apic_icr_write(apicid, dmode | vector);
>>>>>>> v4.9.227
}

static void numachip_send_IPI_mask(const struct cpumask *mask, int vector)
{
	unsigned int cpu;

	for_each_cpu(cpu, mask)
		numachip_send_IPI_one(cpu, vector);
}

static void numachip_send_IPI_mask_allbutself(const struct cpumask *mask,
						int vector)
{
	unsigned int this_cpu = smp_processor_id();
	unsigned int cpu;

	for_each_cpu(cpu, mask) {
		if (cpu != this_cpu)
			numachip_send_IPI_one(cpu, vector);
	}
}

static void numachip_send_IPI_allbutself(int vector)
{
	unsigned int this_cpu = smp_processor_id();
	unsigned int cpu;

	for_each_online_cpu(cpu) {
		if (cpu != this_cpu)
			numachip_send_IPI_one(cpu, vector);
	}
}

static void numachip_send_IPI_all(int vector)
{
	numachip_send_IPI_mask(cpu_online_mask, vector);
}

static void numachip_send_IPI_self(int vector)
{
<<<<<<< HEAD
	__default_send_IPI_shortcut(APIC_DEST_SELF, vector, APIC_DEST_PHYSICAL);
}

static int __init numachip_probe(void)
{
	return apic == &apic_numachip;
}

static void __init map_csrs(void)
{
	printk(KERN_INFO "NumaChip: Mapping local CSR space (%016llx - %016llx)\n",
		NUMACHIP_LCSR_BASE, NUMACHIP_LCSR_BASE + NUMACHIP_LCSR_SIZE - 1);
	init_extra_mapping_uc(NUMACHIP_LCSR_BASE, NUMACHIP_LCSR_SIZE);

	printk(KERN_INFO "NumaChip: Mapping global CSR space (%016llx - %016llx)\n",
		NUMACHIP_GCSR_BASE, NUMACHIP_GCSR_BASE + NUMACHIP_GCSR_SIZE - 1);
	init_extra_mapping_uc(NUMACHIP_GCSR_BASE, NUMACHIP_GCSR_SIZE);
=======
	apic_write(APIC_SELF_IPI, vector);
}

static int __init numachip1_probe(void)
{
	return apic == &apic_numachip1;
}

static int __init numachip2_probe(void)
{
	return apic == &apic_numachip2;
>>>>>>> v4.9.227
}

static void fixup_cpu_id(struct cpuinfo_x86 *c, int node)
{
<<<<<<< HEAD

	if (c->phys_proc_id != node) {
		c->phys_proc_id = node;
		per_cpu(cpu_llc_id, smp_processor_id()) = node;
	}
=======
	u64 val;
	u32 nodes = 1;

	this_cpu_write(cpu_llc_id, node);

	/* Account for nodes per socket in multi-core-module processors */
	if (static_cpu_has(X86_FEATURE_NODEID_MSR)) {
		rdmsrl(MSR_FAM10H_NODE_ID, val);
		nodes = ((val >> 3) & 7) + 1;
	}

	c->phys_proc_id = node / nodes;
>>>>>>> v4.9.227
}

static int __init numachip_system_init(void)
{
<<<<<<< HEAD
	unsigned int val;

	if (!numachip_system)
		return 0;
=======
	/* Map the LCSR area and set up the apic_icr_write function */
	switch (numachip_system) {
	case 1:
		init_extra_mapping_uc(NUMACHIP_LCSR_BASE, NUMACHIP_LCSR_SIZE);
		numachip_apic_icr_write = numachip1_apic_icr_write;
		break;
	case 2:
		init_extra_mapping_uc(NUMACHIP2_LCSR_BASE, NUMACHIP2_LCSR_SIZE);
		numachip_apic_icr_write = numachip2_apic_icr_write;
		break;
	default:
		return 0;
	}
>>>>>>> v4.9.227

	x86_cpuinit.fixup_cpu_id = fixup_cpu_id;
	x86_init.pci.arch_init = pci_numachip_init;

<<<<<<< HEAD
	map_csrs();

	val = read_lcsr(CSR_G0_NODE_IDS);
	printk(KERN_INFO "NumaChip: Local NodeID = %08x\n", val);

=======
>>>>>>> v4.9.227
	return 0;
}
early_initcall(numachip_system_init);

<<<<<<< HEAD
static int numachip_acpi_madt_oem_check(char *oem_id, char *oem_table_id)
{
	if (!strncmp(oem_id, "NUMASC", 6)) {
		numachip_system = 1;
		return 1;
	}

	return 0;
}

static const struct apic apic_numachip __refconst = {

	.name				= "NumaConnect system",
	.probe				= numachip_probe,
	.acpi_madt_oem_check		= numachip_acpi_madt_oem_check,
=======
static int numachip1_acpi_madt_oem_check(char *oem_id, char *oem_table_id)
{
	if ((strncmp(oem_id, "NUMASC", 6) != 0) ||
	    (strncmp(oem_table_id, "NCONNECT", 8) != 0))
		return 0;

	numachip_system = 1;

	return 1;
}

static int numachip2_acpi_madt_oem_check(char *oem_id, char *oem_table_id)
{
	if ((strncmp(oem_id, "NUMASC", 6) != 0) ||
	    (strncmp(oem_table_id, "NCONECT2", 8) != 0))
		return 0;

	numachip_system = 2;

	return 1;
}

/* APIC IPIs are queued */
static void numachip_apic_wait_icr_idle(void)
{
}

/* APIC NMI IPIs are queued */
static u32 numachip_safe_apic_wait_icr_idle(void)
{
	return 0;
}

static const struct apic apic_numachip1 __refconst = {
	.name				= "NumaConnect system",
	.probe				= numachip1_probe,
	.acpi_madt_oem_check		= numachip1_acpi_madt_oem_check,
>>>>>>> v4.9.227
	.apic_id_valid			= numachip_apic_id_valid,
	.apic_id_registered		= numachip_apic_id_registered,

	.irq_delivery_mode		= dest_Fixed,
	.irq_dest_mode			= 0, /* physical */

	.target_cpus			= online_target_cpus,
	.disable_esr			= 0,
	.dest_logical			= 0,
	.check_apicid_used		= NULL,

	.vector_allocation_domain	= default_vector_allocation_domain,
	.init_apic_ldr			= flat_init_apic_ldr,

	.ioapic_phys_id_map		= NULL,
	.setup_apic_routing		= NULL,
	.cpu_present_to_apicid		= default_cpu_present_to_apicid,
	.apicid_to_cpu_present		= NULL,
	.check_phys_apicid_present	= default_check_phys_apicid_present,
	.phys_pkg_id			= numachip_phys_pkg_id,

<<<<<<< HEAD
	.get_apic_id			= get_apic_id,
	.set_apic_id			= set_apic_id,
	.apic_id_mask			= 0xffU << 24,

	.cpu_mask_to_apicid_and		= default_cpu_mask_to_apicid_and,

=======
	.get_apic_id			= numachip1_get_apic_id,
	.set_apic_id			= numachip1_set_apic_id,

	.cpu_mask_to_apicid_and		= default_cpu_mask_to_apicid_and,

	.send_IPI			= numachip_send_IPI_one,
>>>>>>> v4.9.227
	.send_IPI_mask			= numachip_send_IPI_mask,
	.send_IPI_mask_allbutself	= numachip_send_IPI_mask_allbutself,
	.send_IPI_allbutself		= numachip_send_IPI_allbutself,
	.send_IPI_all			= numachip_send_IPI_all,
	.send_IPI_self			= numachip_send_IPI_self,

	.wakeup_secondary_cpu		= numachip_wakeup_secondary,
<<<<<<< HEAD
	.wait_for_init_deassert		= false,
=======
>>>>>>> v4.9.227
	.inquire_remote_apic		= NULL, /* REMRD not supported */

	.read				= native_apic_mem_read,
	.write				= native_apic_mem_write,
	.eoi_write			= native_apic_mem_write,
	.icr_read			= native_apic_icr_read,
	.icr_write			= native_apic_icr_write,
<<<<<<< HEAD
	.wait_icr_idle			= native_apic_wait_icr_idle,
	.safe_wait_icr_idle		= native_safe_apic_wait_icr_idle,
};
apic_driver(apic_numachip);

=======
	.wait_icr_idle			= numachip_apic_wait_icr_idle,
	.safe_wait_icr_idle		= numachip_safe_apic_wait_icr_idle,
};

apic_driver(apic_numachip1);

static const struct apic apic_numachip2 __refconst = {
	.name				= "NumaConnect2 system",
	.probe				= numachip2_probe,
	.acpi_madt_oem_check		= numachip2_acpi_madt_oem_check,
	.apic_id_valid			= numachip_apic_id_valid,
	.apic_id_registered		= numachip_apic_id_registered,

	.irq_delivery_mode		= dest_Fixed,
	.irq_dest_mode			= 0, /* physical */

	.target_cpus			= online_target_cpus,
	.disable_esr			= 0,
	.dest_logical			= 0,
	.check_apicid_used		= NULL,

	.vector_allocation_domain	= default_vector_allocation_domain,
	.init_apic_ldr			= flat_init_apic_ldr,

	.ioapic_phys_id_map		= NULL,
	.setup_apic_routing		= NULL,
	.cpu_present_to_apicid		= default_cpu_present_to_apicid,
	.apicid_to_cpu_present		= NULL,
	.check_phys_apicid_present	= default_check_phys_apicid_present,
	.phys_pkg_id			= numachip_phys_pkg_id,

	.get_apic_id			= numachip2_get_apic_id,
	.set_apic_id			= numachip2_set_apic_id,

	.cpu_mask_to_apicid_and		= default_cpu_mask_to_apicid_and,

	.send_IPI			= numachip_send_IPI_one,
	.send_IPI_mask			= numachip_send_IPI_mask,
	.send_IPI_mask_allbutself	= numachip_send_IPI_mask_allbutself,
	.send_IPI_allbutself		= numachip_send_IPI_allbutself,
	.send_IPI_all			= numachip_send_IPI_all,
	.send_IPI_self			= numachip_send_IPI_self,

	.wakeup_secondary_cpu		= numachip_wakeup_secondary,
	.inquire_remote_apic		= NULL, /* REMRD not supported */

	.read				= native_apic_mem_read,
	.write				= native_apic_mem_write,
	.eoi_write			= native_apic_mem_write,
	.icr_read			= native_apic_icr_read,
	.icr_write			= native_apic_icr_write,
	.wait_icr_idle			= numachip_apic_wait_icr_idle,
	.safe_wait_icr_idle		= numachip_safe_apic_wait_icr_idle,
};

apic_driver(apic_numachip2);
>>>>>>> v4.9.227
