/*
 * SAMSUNG EXYNOS Flattened Device Tree enabled machine
 *
 * Copyright (c) 2010-2014 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/io.h>
<<<<<<< HEAD
#include <linux/kernel.h>
#include <linux/serial_s3c.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_fdt.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/pm_domain.h>
#include <linux/irqchip.h>
=======
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_fdt.h>
#include <linux/platform_device.h>
#include <linux/irqchip.h>
#include <linux/soc/samsung/exynos-regs-pmu.h>
>>>>>>> v4.9.227

#include <asm/cacheflush.h>
#include <asm/hardware/cache-l2x0.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
<<<<<<< HEAD
#include <asm/memory.h>

#include "common.h"
#include "mfc.h"
#include "regs-pmu.h"
#include "regs-sys.h"

void __iomem *pmu_base_addr;

static struct map_desc exynos4_iodesc[] __initdata = {
	{
		.virtual	= (unsigned long)S3C_VA_SYS,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_SYSCON),
		.length		= SZ_64K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S3C_VA_TIMER,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_TIMER),
		.length		= SZ_16K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S3C_VA_WATCHDOG,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_WATCHDOG),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S5P_VA_SROMC,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_SROMC),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S5P_VA_SYSTIMER,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_SYSTIMER),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S5P_VA_COMBINER_BASE,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_COMBINER),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S5P_VA_GIC_CPU,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_GIC_CPU),
		.length		= SZ_64K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S5P_VA_GIC_DIST,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_GIC_DIST),
		.length		= SZ_64K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S5P_VA_CMU,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_CMU),
		.length		= SZ_128K,
		.type		= MT_DEVICE,
	}, {
=======

#include <mach/map.h>
#include <plat/cpu.h>

#include "common.h"

static struct map_desc exynos4_iodesc[] __initdata = {
	{
>>>>>>> v4.9.227
		.virtual	= (unsigned long)S5P_VA_COREPERI_BASE,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_COREPERI),
		.length		= SZ_8K,
		.type		= MT_DEVICE,
<<<<<<< HEAD
	}, {
		.virtual	= (unsigned long)S5P_VA_L2CC,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_L2CC),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S5P_VA_DMC0,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_DMC0),
		.length		= SZ_64K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S5P_VA_DMC1,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_DMC1),
		.length		= SZ_64K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S3C_VA_USB_HSPHY,
		.pfn		= __phys_to_pfn(EXYNOS4_PA_HSPHY),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	},
};

static struct map_desc exynos5_iodesc[] __initdata = {
	{
		.virtual	= (unsigned long)S3C_VA_SYS,
		.pfn		= __phys_to_pfn(EXYNOS5_PA_SYSCON),
		.length		= SZ_64K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S3C_VA_TIMER,
		.pfn		= __phys_to_pfn(EXYNOS5_PA_TIMER),
		.length		= SZ_16K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S3C_VA_WATCHDOG,
		.pfn		= __phys_to_pfn(EXYNOS5_PA_WATCHDOG),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S5P_VA_SROMC,
		.pfn		= __phys_to_pfn(EXYNOS5_PA_SROMC),
		.length		= SZ_4K,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (unsigned long)S5P_VA_CMU,
		.pfn		= __phys_to_pfn(EXYNOS5_PA_CMU),
		.length		= 144 * SZ_1K,
		.type		= MT_DEVICE,
	},
};

static void exynos_restart(enum reboot_mode mode, const char *cmd)
{
	struct device_node *np;
	u32 val = 0x1;
	void __iomem *addr = pmu_base_addr + EXYNOS_SWRESET;

	if (of_machine_is_compatible("samsung,exynos5440")) {
		u32 status;
		np = of_find_compatible_node(NULL, NULL, "samsung,exynos5440-clock");

		addr = of_iomap(np, 0) + 0xbc;
		status = __raw_readl(addr);

		addr = of_iomap(np, 0) + 0xcc;
		val = __raw_readl(addr);

		val = (val & 0xffff0000) | (status & 0xffff);
	}

	__raw_writel(val, addr);
}

=======
	},
};

>>>>>>> v4.9.227
static struct platform_device exynos_cpuidle = {
	.name              = "exynos_cpuidle",
#ifdef CONFIG_ARM_EXYNOS_CPUIDLE
	.dev.platform_data = exynos_enter_aftr,
#endif
	.id                = -1,
};

void __iomem *sysram_base_addr;
void __iomem *sysram_ns_base_addr;

void __init exynos_sysram_init(void)
{
	struct device_node *node;

	for_each_compatible_node(node, NULL, "samsung,exynos4210-sysram") {
		if (!of_device_is_available(node))
			continue;
		sysram_base_addr = of_iomap(node, 0);
		break;
	}

	for_each_compatible_node(node, NULL, "samsung,exynos4210-sysram-ns") {
		if (!of_device_is_available(node))
			continue;
		sysram_ns_base_addr = of_iomap(node, 0);
		break;
	}
}

static void __init exynos_init_late(void)
{
	if (of_machine_is_compatible("samsung,exynos5440"))
		/* to be supported later */
		return;

	exynos_pm_init();
}

static int __init exynos_fdt_map_chipid(unsigned long node, const char *uname,
					int depth, void *data)
{
	struct map_desc iodesc;
	const __be32 *reg;
	int len;

	if (!of_flat_dt_is_compatible(node, "samsung,exynos4210-chipid") &&
		!of_flat_dt_is_compatible(node, "samsung,exynos5440-clock"))
		return 0;

	reg = of_get_flat_dt_prop(node, "reg", &len);
	if (reg == NULL || len != (sizeof(unsigned long) * 2))
		return 0;

	iodesc.pfn = __phys_to_pfn(be32_to_cpu(reg[0]));
	iodesc.length = be32_to_cpu(reg[1]) - 1;
	iodesc.virtual = (unsigned long)S5P_VA_CHIPID;
	iodesc.type = MT_DEVICE;
	iotable_init(&iodesc, 1);
	return 1;
}

/*
 * exynos_map_io
 *
 * register the standard cpu IO areas
 */
static void __init exynos_map_io(void)
{
	if (soc_is_exynos4())
		iotable_init(exynos4_iodesc, ARRAY_SIZE(exynos4_iodesc));
<<<<<<< HEAD

	if (soc_is_exynos5())
		iotable_init(exynos5_iodesc, ARRAY_SIZE(exynos5_iodesc));
=======
>>>>>>> v4.9.227
}

static void __init exynos_init_io(void)
{
	debug_ll_io_init();

	of_scan_flat_dt(exynos_fdt_map_chipid, NULL);

	/* detect cpu id and rev. */
	s5p_init_cpu(S5P_VA_CHIPID);

	exynos_map_io();
}

<<<<<<< HEAD
static const struct of_device_id exynos_dt_pmu_match[] = {
	{ .compatible = "samsung,exynos3250-pmu" },
	{ .compatible = "samsung,exynos4210-pmu" },
	{ .compatible = "samsung,exynos4212-pmu" },
	{ .compatible = "samsung,exynos4412-pmu" },
	{ .compatible = "samsung,exynos5250-pmu" },
	{ .compatible = "samsung,exynos5260-pmu" },
	{ .compatible = "samsung,exynos5410-pmu" },
	{ .compatible = "samsung,exynos5420-pmu" },
=======
/*
 * Set or clear the USE_DELAYED_RESET_ASSERTION option. Used by smp code
 * and suspend.
 *
 * This is necessary only on Exynos4 SoCs. When system is running
 * USE_DELAYED_RESET_ASSERTION should be set so the ARM CLK clock down
 * feature could properly detect global idle state when secondary CPU is
 * powered down.
 *
 * However this should not be set when such system is going into suspend.
 */
void exynos_set_delayed_reset_assertion(bool enable)
{
	if (of_machine_is_compatible("samsung,exynos4")) {
		unsigned int tmp, core_id;

		for (core_id = 0; core_id < num_possible_cpus(); core_id++) {
			tmp = pmu_raw_readl(EXYNOS_ARM_CORE_OPTION(core_id));
			if (enable)
				tmp |= S5P_USE_DELAYED_RESET_ASSERTION;
			else
				tmp &= ~(S5P_USE_DELAYED_RESET_ASSERTION);
			pmu_raw_writel(tmp, EXYNOS_ARM_CORE_OPTION(core_id));
		}
	}
}

/*
 * Apparently, these SoCs are not able to wake-up from suspend using
 * the PMU. Too bad. Should they suddenly become capable of such a
 * feat, the matches below should be moved to suspend.c.
 */
static const struct of_device_id exynos_dt_pmu_match[] = {
	{ .compatible = "samsung,exynos5260-pmu" },
	{ .compatible = "samsung,exynos5410-pmu" },
>>>>>>> v4.9.227
	{ /*sentinel*/ },
};

static void exynos_map_pmu(void)
{
	struct device_node *np;

	np = of_find_matching_node(NULL, exynos_dt_pmu_match);
	if (np)
		pmu_base_addr = of_iomap(np, 0);
<<<<<<< HEAD

	if (!pmu_base_addr)
		panic("failed to find exynos pmu register\n");
=======
>>>>>>> v4.9.227
}

static void __init exynos_init_irq(void)
{
	irqchip_init();
	/*
	 * Since platsmp.c needs pmu base address by the time
	 * DT is not unflatten so we can't use DT APIs before
	 * init_irq
	 */
	exynos_map_pmu();
}

static void __init exynos_dt_machine_init(void)
{
<<<<<<< HEAD
	struct device_node *i2c_np;
	const char *i2c_compat = "samsung,s3c2440-i2c";
	unsigned int tmp;
	int id;

	/*
	 * Exynos5's legacy i2c controller and new high speed i2c
	 * controller have muxed interrupt sources. By default the
	 * interrupts for 4-channel HS-I2C controller are enabled.
	 * If node for first four channels of legacy i2c controller
	 * are available then re-configure the interrupts via the
	 * system register.
	 */
	if (soc_is_exynos5()) {
		for_each_compatible_node(i2c_np, NULL, i2c_compat) {
			if (of_device_is_available(i2c_np)) {
				id = of_alias_get_id(i2c_np, "i2c");
				if (id < 4) {
					tmp = readl(EXYNOS5_SYS_I2C_CFG);
					writel(tmp & ~(0x1 << id),
							EXYNOS5_SYS_I2C_CFG);
				}
			}
		}
	}

=======
>>>>>>> v4.9.227
	/*
	 * This is called from smp_prepare_cpus if we've built for SMP, but
	 * we still need to set it up for PM and firmware ops if not.
	 */
	if (!IS_ENABLED(CONFIG_SMP))
		exynos_sysram_init();

<<<<<<< HEAD
	if (of_machine_is_compatible("samsung,exynos4210") ||
			of_machine_is_compatible("samsung,exynos5250"))
		platform_device_register(&exynos_cpuidle);

	platform_device_register_simple("exynos-cpufreq", -1, NULL, 0);

	of_platform_populate(NULL, of_default_bus_match_table, NULL, NULL);
}

static char const *exynos_dt_compat[] __initconst = {
=======
#if defined(CONFIG_SMP) && defined(CONFIG_ARM_EXYNOS_CPUIDLE)
	if (of_machine_is_compatible("samsung,exynos4210") ||
	    of_machine_is_compatible("samsung,exynos3250"))
		exynos_cpuidle.dev.platform_data = &cpuidle_coupled_exynos_data;
#endif
	if (of_machine_is_compatible("samsung,exynos4210") ||
	    of_machine_is_compatible("samsung,exynos4212") ||
	    (of_machine_is_compatible("samsung,exynos4412") &&
	     of_machine_is_compatible("samsung,trats2")) ||
	    of_machine_is_compatible("samsung,exynos3250") ||
	    of_machine_is_compatible("samsung,exynos5250"))
		platform_device_register(&exynos_cpuidle);
}

static char const *const exynos_dt_compat[] __initconst = {
>>>>>>> v4.9.227
	"samsung,exynos3",
	"samsung,exynos3250",
	"samsung,exynos4",
	"samsung,exynos4210",
	"samsung,exynos4212",
	"samsung,exynos4412",
<<<<<<< HEAD
=======
	"samsung,exynos4415",
>>>>>>> v4.9.227
	"samsung,exynos5",
	"samsung,exynos5250",
	"samsung,exynos5260",
	"samsung,exynos5420",
	"samsung,exynos5440",
	NULL
};

<<<<<<< HEAD
static void __init exynos_reserve(void)
{
#ifdef CONFIG_S5P_DEV_MFC
	int i;
	char *mfc_mem[] = {
		"samsung,mfc-v5",
		"samsung,mfc-v6",
		"samsung,mfc-v7",
	};

	for (i = 0; i < ARRAY_SIZE(mfc_mem); i++)
		if (of_scan_flat_dt(s5p_fdt_alloc_mfc_mem, mfc_mem[i]))
			break;
#endif
}

=======
>>>>>>> v4.9.227
static void __init exynos_dt_fixup(void)
{
	/*
	 * Some versions of uboot pass garbage entries in the memory node,
	 * use the old CONFIG_ARM_NR_BANKS
	 */
	of_fdt_limit_memory(8);
}

DT_MACHINE_START(EXYNOS_DT, "SAMSUNG EXYNOS (Flattened Device Tree)")
	/* Maintainer: Thomas Abraham <thomas.abraham@linaro.org> */
	/* Maintainer: Kukjin Kim <kgene.kim@samsung.com> */
	.l2c_aux_val	= 0x3c400001,
	.l2c_aux_mask	= 0xc20fffff,
	.smp		= smp_ops(exynos_smp_ops),
	.map_io		= exynos_init_io,
	.init_early	= exynos_firmware_init,
	.init_irq	= exynos_init_irq,
	.init_machine	= exynos_dt_machine_init,
	.init_late	= exynos_init_late,
	.dt_compat	= exynos_dt_compat,
<<<<<<< HEAD
	.restart	= exynos_restart,
	.reserve	= exynos_reserve,
=======
>>>>>>> v4.9.227
	.dt_fixup	= exynos_dt_fixup,
MACHINE_END
