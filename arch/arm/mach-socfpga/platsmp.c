/*
 * Copyright 2010-2011 Calxeda, Inc.
 * Copyright 2012 Pavel Machek <pavel@denx.de>
 * Based on platsmp.c, Copyright (C) 2002 ARM Ltd.
 * Copyright (C) 2012 Altera Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/smp.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>

#include <asm/cacheflush.h>
#include <asm/smp_scu.h>
#include <asm/smp_plat.h>

#include "core.h"

static int socfpga_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	int trampoline_size = &secondary_trampoline_end - &secondary_trampoline;

	if (socfpga_cpu1start_addr) {
<<<<<<< HEAD
		memcpy(phys_to_virt(0), &secondary_trampoline, trampoline_size);

		__raw_writel(virt_to_phys(socfpga_secondary_startup),
			(sys_manager_base_addr + (socfpga_cpu1start_addr & 0x000000ff)));
=======
		/* This will put CPU #1 into reset. */
		writel(RSTMGR_MPUMODRST_CPU1,
		       rst_manager_base_addr + SOCFPGA_RSTMGR_MODMPURST);

		memcpy(phys_to_virt(0), &secondary_trampoline, trampoline_size);

		writel(virt_to_phys(secondary_startup),
		       sys_manager_base_addr + (socfpga_cpu1start_addr & 0x000000ff));
>>>>>>> v4.9.227

		flush_cache_all();
		smp_wmb();
		outer_clean_range(0, trampoline_size);

<<<<<<< HEAD
		/* This will release CPU #1 out of reset.*/
		__raw_writel(0, rst_manager_base_addr + 0x10);
=======
		/* This will release CPU #1 out of reset. */
		writel(0, rst_manager_base_addr + SOCFPGA_RSTMGR_MODMPURST);
>>>>>>> v4.9.227
	}

	return 0;
}

<<<<<<< HEAD
/*
 * Initialise the CPU possible map early - this describes the CPUs
 * which may be present or become present in the system.
 */
static void __init socfpga_smp_init_cpus(void)
{
	unsigned int i, ncores;

	ncores = scu_get_core_count(socfpga_scu_base_addr);

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);

	/* sanity check */
	if (ncores > num_possible_cpus()) {
		pr_warn("socfpga: no. of cores (%d) greater than configured"
			"maximum of %d - clipping\n", ncores, num_possible_cpus());
		ncores = num_possible_cpus();
	}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);
=======
static int socfpga_a10_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	int trampoline_size = &secondary_trampoline_end - &secondary_trampoline;

	if (socfpga_cpu1start_addr) {
		writel(RSTMGR_MPUMODRST_CPU1, rst_manager_base_addr +
		       SOCFPGA_A10_RSTMGR_MODMPURST);
		memcpy(phys_to_virt(0), &secondary_trampoline, trampoline_size);

		writel(virt_to_phys(secondary_startup),
		       sys_manager_base_addr + (socfpga_cpu1start_addr & 0x00000fff));

		flush_cache_all();
		smp_wmb();
		outer_clean_range(0, trampoline_size);

		/* This will release CPU #1 out of reset. */
		writel(0, rst_manager_base_addr + SOCFPGA_A10_RSTMGR_MODMPURST);
	}

	return 0;
>>>>>>> v4.9.227
}

static void __init socfpga_smp_prepare_cpus(unsigned int max_cpus)
{
<<<<<<< HEAD
	scu_enable(socfpga_scu_base_addr);
}

=======
	struct device_node *np;
	void __iomem *socfpga_scu_base_addr;

	np = of_find_compatible_node(NULL, NULL, "arm,cortex-a9-scu");
	if (!np) {
		pr_err("%s: missing scu\n", __func__);
		return;
	}

	socfpga_scu_base_addr = of_iomap(np, 0);
	if (!socfpga_scu_base_addr)
		return;
	scu_enable(socfpga_scu_base_addr);
}

#ifdef CONFIG_HOTPLUG_CPU
>>>>>>> v4.9.227
/*
 * platform-specific code to shutdown a CPU
 *
 * Called with IRQs disabled
 */
static void socfpga_cpu_die(unsigned int cpu)
{
<<<<<<< HEAD
	cpu_do_idle();

	/* We should have never returned from idle */
	panic("cpu %d unexpectedly exit from shutdown\n", cpu);
}

struct smp_operations socfpga_smp_ops __initdata = {
	.smp_init_cpus		= socfpga_smp_init_cpus,
=======
	/* Do WFI. If we wake up early, go back into WFI */
	while (1)
		cpu_do_idle();
}

/*
 * We need a dummy function so that platform_can_cpu_hotplug() knows
 * we support CPU hotplug. However, the function does not need to do
 * anything, because CPUs going offline just do WFI. We could reset
 * the CPUs but it would increase power consumption.
 */
static int socfpga_cpu_kill(unsigned int cpu)
{
	return 1;
}
#endif

static const struct smp_operations socfpga_smp_ops __initconst = {
>>>>>>> v4.9.227
	.smp_prepare_cpus	= socfpga_smp_prepare_cpus,
	.smp_boot_secondary	= socfpga_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= socfpga_cpu_die,
<<<<<<< HEAD
#endif
};
=======
	.cpu_kill		= socfpga_cpu_kill,
#endif
};

static const struct smp_operations socfpga_a10_smp_ops __initconst = {
	.smp_prepare_cpus	= socfpga_smp_prepare_cpus,
	.smp_boot_secondary	= socfpga_a10_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_die		= socfpga_cpu_die,
	.cpu_kill		= socfpga_cpu_kill,
#endif
};

CPU_METHOD_OF_DECLARE(socfpga_smp, "altr,socfpga-smp", &socfpga_smp_ops);
CPU_METHOD_OF_DECLARE(socfpga_a10_smp, "altr,socfpga-a10-smp", &socfpga_a10_smp_ops);
>>>>>>> v4.9.227
