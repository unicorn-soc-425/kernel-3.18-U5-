/*
 * Copyright (C) 2002 ARM Ltd.
 * Copyright (C) 2008 STMicroelctronics.
 * Copyright (C) 2009 ST-Ericsson.
 * Author: Srinidhi Kasagar <srinidhi.kasagar@stericsson.com>
 *
 * This file is based on arm realview platform
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/smp.h>
#include <linux/io.h>
<<<<<<< HEAD
=======
#include <linux/of.h>
#include <linux/of_address.h>
>>>>>>> v4.9.227

#include <asm/cacheflush.h>
#include <asm/smp_plat.h>
#include <asm/smp_scu.h>

#include "setup.h"

#include "db8500-regs.h"
<<<<<<< HEAD
#include "id.h"

/* This is called from headsmp.S to wakeup the secondary core */
extern void u8500_secondary_startup(void);

/*
 * Write pen_release in a way that is guaranteed to be visible to all
 * observers, irrespective of whether they're taking part in coherency
 * or not.  This is necessary for the hotplug code to work reliably.
 */
static void write_pen_release(int val)
{
	pen_release = val;
	smp_wmb();
	sync_cache_w(&pen_release);
}

static void __iomem *scu_base_addr(void)
{
	if (cpu_is_u8500_family() || cpu_is_ux540_family())
		return __io_address(U8500_SCU_BASE);
	else
		ux500_unknown_soc();

	return NULL;
}

static DEFINE_SPINLOCK(boot_lock);

static void ux500_secondary_init(unsigned int cpu)
{
	/*
	 * let the primary processor know we're out of the
	 * pen, then head off into the C entry point
	 */
	write_pen_release(-1);

	/*
	 * Synchronise with the boot thread.
	 */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

static int ux500_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	unsigned long timeout;

	/*
	 * set synchronisation state between this boot processor
	 * and the secondary one
	 */
	spin_lock(&boot_lock);

	/*
	 * The secondary processor is waiting to be released from
	 * the holding pen - release it, then wait for it to flag
	 * that it has been released by resetting pen_release.
	 */
	write_pen_release(cpu_logical_map(cpu));

	arch_send_wakeup_ipi_mask(cpumask_of(cpu));

	timeout = jiffies + (1 * HZ);
	while (time_before(jiffies, timeout)) {
		if (pen_release == -1)
			break;
	}

	/*
	 * now the secondary core is starting up let it run its
	 * calibrations, then wait for it to finish
	 */
	spin_unlock(&boot_lock);

	return pen_release != -1 ? -ENOSYS : 0;
}

static void __init wakeup_secondary(void)
{
	void __iomem *backupram;

	if (cpu_is_u8500_family() || cpu_is_ux540_family())
		backupram = __io_address(U8500_BACKUPRAM0_BASE);
	else
		ux500_unknown_soc();
=======

/* Magic triggers in backup RAM */
#define UX500_CPU1_JUMPADDR_OFFSET 0x1FF4
#define UX500_CPU1_WAKEMAGIC_OFFSET 0x1FF0

static void wakeup_secondary(void)
{
	struct device_node *np;
	static void __iomem *backupram;

	np = of_find_compatible_node(NULL, NULL, "ste,dbx500-backupram");
	if (!np) {
		pr_err("No backupram base address\n");
		return;
	}
	backupram = of_iomap(np, 0);
	of_node_put(np);
	if (!backupram) {
		pr_err("No backupram remap\n");
		return;
	}
>>>>>>> v4.9.227

	/*
	 * write the address of secondary startup into the backup ram register
	 * at offset 0x1FF4, then write the magic number 0xA1FEED01 to the
	 * backup ram register at offset 0x1FF0, which is what boot rom code
<<<<<<< HEAD
	 * is waiting for. This would wake up the secondary core from WFE
	 */
#define UX500_CPU1_JUMPADDR_OFFSET 0x1FF4
	__raw_writel(virt_to_phys(u8500_secondary_startup),
		     backupram + UX500_CPU1_JUMPADDR_OFFSET);

#define UX500_CPU1_WAKEMAGIC_OFFSET 0x1FF0
	__raw_writel(0xA1FEED01,
		     backupram + UX500_CPU1_WAKEMAGIC_OFFSET);

	/* make sure write buffer is drained */
	mb();
}

/*
 * Initialise the CPU possible map early - this describes the CPUs
 * which may be present or become present in the system.
 */
static void __init ux500_smp_init_cpus(void)
{
	void __iomem *scu_base = scu_base_addr();
	unsigned int i, ncores;

	ncores = scu_base ? scu_get_core_count(scu_base) : 1;

	/* sanity check */
	if (ncores > nr_cpu_ids) {
		pr_warn("SMP: %u cores greater than maximum (%u), clipping\n",
			ncores, nr_cpu_ids);
		ncores = nr_cpu_ids;
	}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);
=======
	 * is waiting for. This will wake up the secondary core from WFE.
	 */
	writel(virt_to_phys(secondary_startup),
	       backupram + UX500_CPU1_JUMPADDR_OFFSET);
	writel(0xA1FEED01,
	       backupram + UX500_CPU1_WAKEMAGIC_OFFSET);

	/* make sure write buffer is drained */
	mb();
	iounmap(backupram);
>>>>>>> v4.9.227
}

static void __init ux500_smp_prepare_cpus(unsigned int max_cpus)
{
<<<<<<< HEAD

	scu_enable(scu_base_addr());
	wakeup_secondary();
}

struct smp_operations ux500_smp_ops __initdata = {
	.smp_init_cpus		= ux500_smp_init_cpus,
	.smp_prepare_cpus	= ux500_smp_prepare_cpus,
	.smp_secondary_init	= ux500_secondary_init,
=======
	struct device_node *np;
	static void __iomem *scu_base;
	unsigned int ncores;
	int i;

	np = of_find_compatible_node(NULL, NULL, "arm,cortex-a9-scu");
	if (!np) {
		pr_err("No SCU base address\n");
		return;
	}
	scu_base = of_iomap(np, 0);
	of_node_put(np);
	if (!scu_base) {
		pr_err("No SCU remap\n");
		return;
	}

	scu_enable(scu_base);
	ncores = scu_get_core_count(scu_base);
	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);
	iounmap(scu_base);
}

static int ux500_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	wakeup_secondary();
	arch_send_wakeup_ipi_mask(cpumask_of(cpu));
	return 0;
}

static const struct smp_operations ux500_smp_ops __initconst = {
	.smp_prepare_cpus	= ux500_smp_prepare_cpus,
>>>>>>> v4.9.227
	.smp_boot_secondary	= ux500_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= ux500_cpu_die,
#endif
};
<<<<<<< HEAD
=======
CPU_METHOD_OF_DECLARE(ux500_smp, "ste,dbx500-smp", &ux500_smp_ops);
>>>>>>> v4.9.227
