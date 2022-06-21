/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Copyright (C) 2012 ARM Limited
 *
 * Author: Will Deacon <will.deacon@arm.com>
 */

#include <linux/init.h>
#include <linux/smp.h>
#include <linux/of.h>
#include <linux/delay.h>
<<<<<<< HEAD
=======
#include <linux/psci.h>

>>>>>>> v4.9.227
#include <uapi/linux/psci.h>

#include <asm/psci.h>
#include <asm/smp_plat.h>

/*
 * psci_smp assumes that the following is true about PSCI:
 *
 * cpu_suspend   Suspend the execution on a CPU
 * @state        we don't currently describe affinity levels, so just pass 0.
 * @entry_point  the first instruction to be executed on return
 * returns 0  success, < 0 on failure
 *
 * cpu_off       Power down a CPU
 * @state        we don't currently describe affinity levels, so just pass 0.
 * no return on successful call
 *
 * cpu_on        Power up a CPU
 * @cpuid        cpuid of target CPU, as from MPIDR
 * @entry_point  the first instruction to be executed on return
 * returns 0  success, < 0 on failure
 *
 * migrate       Migrate the context to a different CPU
 * @cpuid        cpuid of target CPU, as from MPIDR
 * returns 0  success, < 0 on failure
 *
 */

extern void secondary_startup(void);

static int psci_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	if (psci_ops.cpu_on)
		return psci_ops.cpu_on(cpu_logical_map(cpu),
<<<<<<< HEAD
				       __pa(secondary_startup));
=======
					virt_to_idmap(&secondary_startup));
>>>>>>> v4.9.227
	return -ENODEV;
}

#ifdef CONFIG_HOTPLUG_CPU
<<<<<<< HEAD
static int psci_cpu_disable(unsigned int cpu)
=======
int psci_cpu_disable(unsigned int cpu)
>>>>>>> v4.9.227
{
	/* Fail early if we don't have CPU_OFF support */
	if (!psci_ops.cpu_off)
		return -EOPNOTSUPP;
<<<<<<< HEAD
	return 0;
}

void __ref psci_cpu_die(unsigned int cpu)
{
       const struct psci_power_state ps = {
               .type = PSCI_POWER_STATE_TYPE_POWER_DOWN,
       };

       if (psci_ops.cpu_off)
               psci_ops.cpu_off(ps);

       /* We should never return */
       panic("psci: cpu %d failed to shutdown\n", cpu);
}

int __ref psci_cpu_kill(unsigned int cpu)
=======

	/* Trusted OS will deny CPU_OFF */
	if (psci_tos_resident_on(cpu))
		return -EPERM;

	return 0;
}

void psci_cpu_die(unsigned int cpu)
{
	u32 state = PSCI_POWER_STATE_TYPE_POWER_DOWN <<
		    PSCI_0_2_POWER_STATE_TYPE_SHIFT;

	if (psci_ops.cpu_off)
		psci_ops.cpu_off(state);

	/* We should never return */
	panic("psci: cpu %d failed to shutdown\n", cpu);
}

int psci_cpu_kill(unsigned int cpu)
>>>>>>> v4.9.227
{
	int err, i;

	if (!psci_ops.affinity_info)
		return 1;
	/*
	 * cpu_kill could race with cpu_die and we can
	 * potentially end up declaring this cpu undead
	 * while it is dying. So, try again a few times.
	 */

	for (i = 0; i < 10; i++) {
		err = psci_ops.affinity_info(cpu_logical_map(cpu), 0);
		if (err == PSCI_0_2_AFFINITY_LEVEL_OFF) {
<<<<<<< HEAD
			pr_debug("CPU%d killed.\n", cpu);
=======
			pr_info("CPU%d killed.\n", cpu);
>>>>>>> v4.9.227
			return 1;
		}

		msleep(10);
<<<<<<< HEAD
		pr_debug("Retrying again to check for CPU kill\n");
=======
		pr_info("Retrying again to check for CPU kill\n");
>>>>>>> v4.9.227
	}

	pr_warn("CPU%d may not have shut down cleanly (AFFINITY_INFO reports %d)\n",
			cpu, err);
	/* Make platform_cpu_kill() fail. */
	return 0;
}

#endif

bool __init psci_smp_available(void)
{
	/* is cpu_on available at least? */
	return (psci_ops.cpu_on != NULL);
}

<<<<<<< HEAD
struct smp_operations __initdata psci_smp_ops = {
=======
const struct smp_operations psci_smp_ops __initconst = {
>>>>>>> v4.9.227
	.smp_boot_secondary	= psci_boot_secondary,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_disable		= psci_cpu_disable,
	.cpu_die		= psci_cpu_die,
	.cpu_kill		= psci_cpu_kill,
#endif
};
