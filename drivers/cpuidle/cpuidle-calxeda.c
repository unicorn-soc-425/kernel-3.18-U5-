/*
 * Copyright 2012 Calxeda, Inc.
 *
 * Based on arch/arm/plat-mxc/cpuidle.c: #v3.7
 * Copyright 2012 Freescale Semiconductor, Inc.
 * Copyright 2012 Linaro Ltd.
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
 *
 * Maintainer: Rob Herring <rob.herring@calxeda.com>
 */

#include <linux/cpuidle.h>
#include <linux/cpu_pm.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/platform_device.h>
<<<<<<< HEAD
#include <asm/cpuidle.h>
#include <asm/suspend.h>
#include <asm/psci.h>

static int calxeda_idle_finish(unsigned long val)
{
	const struct psci_power_state ps = {
		.type = PSCI_POWER_STATE_TYPE_POWER_DOWN,
	};
	return psci_ops.cpu_suspend(ps, __pa(cpu_resume));
=======
#include <linux/psci.h>

#include <asm/cpuidle.h>
#include <asm/suspend.h>

#include <uapi/linux/psci.h>

#define CALXEDA_IDLE_PARAM \
	((0 << PSCI_0_2_POWER_STATE_ID_SHIFT) | \
	 (0 << PSCI_0_2_POWER_STATE_AFFL_SHIFT) | \
	 (PSCI_POWER_STATE_TYPE_POWER_DOWN << PSCI_0_2_POWER_STATE_TYPE_SHIFT))

static int calxeda_idle_finish(unsigned long val)
{
	return psci_ops.cpu_suspend(CALXEDA_IDLE_PARAM, __pa(cpu_resume));
>>>>>>> v4.9.227
}

static int calxeda_pwrdown_idle(struct cpuidle_device *dev,
				struct cpuidle_driver *drv,
				int index)
{
	cpu_pm_enter();
	cpu_suspend(0, calxeda_idle_finish);
	cpu_pm_exit();

	return index;
}

static struct cpuidle_driver calxeda_idle_driver = {
	.name = "calxeda_idle",
	.states = {
		ARM_CPUIDLE_WFI_STATE,
		{
			.name = "PG",
			.desc = "Power Gate",
<<<<<<< HEAD
			.flags = CPUIDLE_FLAG_TIME_VALID,
=======
>>>>>>> v4.9.227
			.exit_latency = 30,
			.power_usage = 50,
			.target_residency = 200,
			.enter = calxeda_pwrdown_idle,
		},
	},
	.state_count = 2,
};

static int calxeda_cpuidle_probe(struct platform_device *pdev)
{
	return cpuidle_register(&calxeda_idle_driver, NULL);
}

static struct platform_driver calxeda_cpuidle_plat_driver = {
        .driver = {
                .name = "cpuidle-calxeda",
<<<<<<< HEAD
                .owner = THIS_MODULE,
        },
        .probe = calxeda_cpuidle_probe,
};

module_platform_driver(calxeda_cpuidle_plat_driver);
=======
        },
        .probe = calxeda_cpuidle_probe,
};
builtin_platform_driver(calxeda_cpuidle_plat_driver);
>>>>>>> v4.9.227
