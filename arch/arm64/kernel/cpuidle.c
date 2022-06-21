/*
 * ARM64 CPU idle arch support
 *
 * Copyright (C) 2014 ARM Ltd.
 * Author: Lorenzo Pieralisi <lorenzo.pieralisi@arm.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

<<<<<<< HEAD
=======
#include <linux/acpi.h>
#include <linux/cpuidle.h>
#include <linux/cpu_pm.h>
>>>>>>> v4.9.227
#include <linux/of.h>
#include <linux/of_device.h>

#include <asm/cpuidle.h>
#include <asm/cpu_ops.h>

<<<<<<< HEAD
int cpu_init_idle(unsigned int cpu)
{
	int ret = -EOPNOTSUPP;
	struct device_node *cpu_node = of_cpu_device_node_get(cpu);

	if (!cpu_node)
		return -ENODEV;

	if (cpu_ops[cpu] && cpu_ops[cpu]->cpu_init_idle)
		ret = cpu_ops[cpu]->cpu_init_idle(cpu_node, cpu);

	of_node_put(cpu_node);
	return ret;
}
=======
int arm_cpuidle_init(unsigned int cpu)
{
	int ret = -EOPNOTSUPP;

	if (cpu_ops[cpu] && cpu_ops[cpu]->cpu_suspend &&
			cpu_ops[cpu]->cpu_init_idle)
		ret = cpu_ops[cpu]->cpu_init_idle(cpu);

	return ret;
}

/**
 * cpu_suspend() - function to enter a low-power idle state
 * @arg: argument to pass to CPU suspend operations
 *
 * Return: 0 on success, -EOPNOTSUPP if CPU suspend hook not initialized, CPU
 * operations back-end error code otherwise.
 */
int arm_cpuidle_suspend(int index)
{
	int cpu = smp_processor_id();

	return cpu_ops[cpu]->cpu_suspend(index);
}

#ifdef CONFIG_ACPI

#include <acpi/processor.h>

int acpi_processor_ffh_lpi_probe(unsigned int cpu)
{
	return arm_cpuidle_init(cpu);
}

int acpi_processor_ffh_lpi_enter(struct acpi_lpi_state *lpi)
{
	return CPU_PM_CPU_IDLE_ENTER(arm_cpuidle_suspend, lpi->index);
}
#endif
>>>>>>> v4.9.227
