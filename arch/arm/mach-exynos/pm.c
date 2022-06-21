/*
<<<<<<< HEAD
 * Copyright (c) 2011-2012 Samsung Electronics Co., Ltd.
=======
 * Copyright (c) 2011-2014 Samsung Electronics Co., Ltd.
>>>>>>> v4.9.227
 *		http://www.samsung.com
 *
 * EXYNOS - Power Management support
 *
 * Based on arch/arm/mach-s3c2410/pm.c
 * Copyright (c) 2006 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/init.h>
#include <linux/suspend.h>
<<<<<<< HEAD
#include <linux/syscore_ops.h>
#include <linux/cpu_pm.h>
#include <linux/io.h>
#include <linux/irqchip/arm-gic.h>
#include <linux/err.h>
#include <linux/clk.h>

#include <asm/cacheflush.h>
#include <asm/hardware/cache-l2x0.h>
#include <asm/smp_scu.h>
#include <asm/suspend.h>

#include <plat/pm-common.h>
#include <plat/regs-srom.h>
=======
#include <linux/cpu_pm.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/soc/samsung/exynos-regs-pmu.h>
#include <linux/soc/samsung/exynos-pmu.h>

#include <asm/firmware.h>
#include <asm/smp_scu.h>
#include <asm/suspend.h>
#include <asm/cacheflush.h>
>>>>>>> v4.9.227

#include <mach/map.h>

#include "common.h"
<<<<<<< HEAD
#include "regs-pmu.h"
#include "regs-sys.h"

/**
 * struct exynos_wkup_irq - Exynos GIC to PMU IRQ mapping
 * @hwirq: Hardware IRQ signal of the GIC
 * @mask: Mask in PMU wake-up mask register
 */
struct exynos_wkup_irq {
	unsigned int hwirq;
	u32 mask;
};

static struct sleep_save exynos5_sys_save[] = {
	SAVE_ITEM(EXYNOS5_SYS_I2C_CFG),
};

static struct sleep_save exynos_core_save[] = {
	/* SROM side */
	SAVE_ITEM(S5P_SROM_BW),
	SAVE_ITEM(S5P_SROM_BC0),
	SAVE_ITEM(S5P_SROM_BC1),
	SAVE_ITEM(S5P_SROM_BC2),
	SAVE_ITEM(S5P_SROM_BC3),
};

/*
 * GIC wake-up support
 */

static u32 exynos_irqwake_intmask = 0xffffffff;

static const struct exynos_wkup_irq exynos4_wkup_irq[] = {
	{ 76, BIT(1) }, /* RTC alarm */
	{ 77, BIT(2) }, /* RTC tick */
	{ /* sentinel */ },
};

static const struct exynos_wkup_irq exynos5250_wkup_irq[] = {
	{ 75, BIT(1) }, /* RTC alarm */
	{ 76, BIT(2) }, /* RTC tick */
	{ /* sentinel */ },
};

static int exynos_irq_set_wake(struct irq_data *data, unsigned int state)
{
	const struct exynos_wkup_irq *wkup_irq;

	if (soc_is_exynos5250())
		wkup_irq = exynos5250_wkup_irq;
	else
		wkup_irq = exynos4_wkup_irq;

	while (wkup_irq->mask) {
		if (wkup_irq->hwirq == data->hwirq) {
			if (!state)
				exynos_irqwake_intmask |= wkup_irq->mask;
			else
				exynos_irqwake_intmask &= ~wkup_irq->mask;
			return 0;
		}
		++wkup_irq;
	}

	return -ENOENT;
}

#define EXYNOS_BOOT_VECTOR_ADDR	(samsung_rev() == EXYNOS4210_REV_1_1 ? \
			pmu_base_addr + S5P_INFORM7 : \
			(samsung_rev() == EXYNOS4210_REV_1_0 ? \
			(sysram_base_addr + 0x24) : \
			pmu_base_addr + S5P_INFORM0))
#define EXYNOS_BOOT_VECTOR_FLAG	(samsung_rev() == EXYNOS4210_REV_1_1 ? \
			pmu_base_addr + S5P_INFORM6 : \
			(samsung_rev() == EXYNOS4210_REV_1_0 ? \
			(sysram_base_addr + 0x20) : \
			pmu_base_addr + S5P_INFORM1))

#define S5P_CHECK_AFTR  0xFCBA0D10
#define S5P_CHECK_SLEEP 0x00000BAD
=======

static inline void __iomem *exynos_boot_vector_addr(void)
{
	if (samsung_rev() == EXYNOS4210_REV_1_1)
		return pmu_base_addr + S5P_INFORM7;
	else if (samsung_rev() == EXYNOS4210_REV_1_0)
		return sysram_base_addr + 0x24;
	return pmu_base_addr + S5P_INFORM0;
}

static inline void __iomem *exynos_boot_vector_flag(void)
{
	if (samsung_rev() == EXYNOS4210_REV_1_1)
		return pmu_base_addr + S5P_INFORM6;
	else if (samsung_rev() == EXYNOS4210_REV_1_0)
		return sysram_base_addr + 0x20;
	return pmu_base_addr + S5P_INFORM1;
}

#define S5P_CHECK_AFTR  0xFCBA0D10
>>>>>>> v4.9.227

/* For Cortex-A9 Diagnostic and Power control register */
static unsigned int save_arm_register[2];

<<<<<<< HEAD
static void exynos_cpu_save_register(void)
=======
void exynos_cpu_save_register(void)
>>>>>>> v4.9.227
{
	unsigned long tmp;

	/* Save Power control register */
	asm ("mrc p15, 0, %0, c15, c0, 0"
	     : "=r" (tmp) : : "cc");

	save_arm_register[0] = tmp;

	/* Save Diagnostic register */
	asm ("mrc p15, 0, %0, c15, c0, 1"
	     : "=r" (tmp) : : "cc");

	save_arm_register[1] = tmp;
}

<<<<<<< HEAD
static void exynos_cpu_restore_register(void)
=======
void exynos_cpu_restore_register(void)
>>>>>>> v4.9.227
{
	unsigned long tmp;

	/* Restore Power control register */
	tmp = save_arm_register[0];

	asm volatile ("mcr p15, 0, %0, c15, c0, 0"
		      : : "r" (tmp)
		      : "cc");

	/* Restore Diagnostic register */
	tmp = save_arm_register[1];

	asm volatile ("mcr p15, 0, %0, c15, c0, 1"
		      : : "r" (tmp)
		      : "cc");
}

<<<<<<< HEAD
static void exynos_pm_central_suspend(void)
=======
void exynos_pm_central_suspend(void)
>>>>>>> v4.9.227
{
	unsigned long tmp;

	/* Setting Central Sequence Register for power down mode */
	tmp = pmu_raw_readl(S5P_CENTRAL_SEQ_CONFIGURATION);
	tmp &= ~S5P_CENTRAL_LOWPWR_CFG;
	pmu_raw_writel(tmp, S5P_CENTRAL_SEQ_CONFIGURATION);
}

<<<<<<< HEAD
static int exynos_pm_central_resume(void)
=======
int exynos_pm_central_resume(void)
>>>>>>> v4.9.227
{
	unsigned long tmp;

	/*
	 * If PMU failed while entering sleep mode, WFI will be
	 * ignored by PMU and then exiting cpu_do_idle().
	 * S5P_CENTRAL_LOWPWR_CFG bit will not be set automatically
	 * in this situation.
	 */
	tmp = pmu_raw_readl(S5P_CENTRAL_SEQ_CONFIGURATION);
	if (!(tmp & S5P_CENTRAL_LOWPWR_CFG)) {
		tmp |= S5P_CENTRAL_LOWPWR_CFG;
		pmu_raw_writel(tmp, S5P_CENTRAL_SEQ_CONFIGURATION);
		/* clear the wakeup state register */
		pmu_raw_writel(0x0, S5P_WAKEUP_STAT);
		/* No need to perform below restore code */
		return -1;
	}

	return 0;
}

/* Ext-GIC nIRQ/nFIQ is the only wakeup source in AFTR */
static void exynos_set_wakeupmask(long mask)
{
	pmu_raw_writel(mask, S5P_WAKEUP_MASK);
<<<<<<< HEAD
=======
	if (soc_is_exynos3250())
		pmu_raw_writel(0x0, S5P_WAKEUP_MASK2);
>>>>>>> v4.9.227
}

static void exynos_cpu_set_boot_vector(long flags)
{
<<<<<<< HEAD
	__raw_writel(virt_to_phys(exynos_cpu_resume), EXYNOS_BOOT_VECTOR_ADDR);
	__raw_writel(flags, EXYNOS_BOOT_VECTOR_FLAG);
=======
	writel_relaxed(virt_to_phys(exynos_cpu_resume),
		       exynos_boot_vector_addr());
	writel_relaxed(flags, exynos_boot_vector_flag());
>>>>>>> v4.9.227
}

static int exynos_aftr_finisher(unsigned long flags)
{
<<<<<<< HEAD
	exynos_set_wakeupmask(0x0000ff3e);
	exynos_cpu_set_boot_vector(S5P_CHECK_AFTR);
	/* Set value of power down register for aftr mode */
	exynos_sys_powerdown_conf(SYS_AFTR);
	cpu_do_idle();
=======
	int ret;

	exynos_set_wakeupmask(soc_is_exynos3250() ? 0x40003ffe : 0x0000ff3e);
	/* Set value of power down register for aftr mode */
	exynos_sys_powerdown_conf(SYS_AFTR);

	ret = call_firmware_op(do_idle, FW_DO_IDLE_AFTR);
	if (ret == -ENOSYS) {
		if (read_cpuid_part() == ARM_CPU_PART_CORTEX_A9)
			exynos_cpu_save_register();
		exynos_cpu_set_boot_vector(S5P_CHECK_AFTR);
		cpu_do_idle();
	}
>>>>>>> v4.9.227

	return 1;
}

void exynos_enter_aftr(void)
{
<<<<<<< HEAD
	cpu_pm_enter();

	exynos_pm_central_suspend();
	if (read_cpuid_part() == ARM_CPU_PART_CORTEX_A9)
		exynos_cpu_save_register();
=======
	unsigned int cpuid = smp_processor_id();

	cpu_pm_enter();

	if (soc_is_exynos3250())
		exynos_set_boot_flag(cpuid, C2_STATE);

	exynos_pm_central_suspend();

	if (of_machine_is_compatible("samsung,exynos4212") ||
	    of_machine_is_compatible("samsung,exynos4412")) {
		/* Setting SEQ_OPTION register */
		pmu_raw_writel(S5P_USE_STANDBY_WFI0 | S5P_USE_STANDBY_WFE0,
			       S5P_CENTRAL_SEQ_OPTION);
	}
>>>>>>> v4.9.227

	cpu_suspend(0, exynos_aftr_finisher);

	if (read_cpuid_part() == ARM_CPU_PART_CORTEX_A9) {
		scu_enable(S5P_VA_SCU);
<<<<<<< HEAD
		exynos_cpu_restore_register();
=======
		if (call_firmware_op(resume) == -ENOSYS)
			exynos_cpu_restore_register();
>>>>>>> v4.9.227
	}

	exynos_pm_central_resume();

<<<<<<< HEAD
	cpu_pm_exit();
}

static int exynos_cpu_suspend(unsigned long arg)
{
#ifdef CONFIG_CACHE_L2X0
	outer_flush_all();
#endif

	if (soc_is_exynos5250())
		flush_cache_all();

	/* issue the standby signal into the pm unit. */
	cpu_do_idle();

	pr_info("Failed to suspend the system\n");
	return 1; /* Aborting suspend */
}

static void exynos_pm_prepare(void)
{
	unsigned int tmp;

	/* Set wake-up mask registers */
	pmu_raw_writel(exynos_get_eint_wake_mask(), S5P_EINT_WAKEUP_MASK);
	pmu_raw_writel(exynos_irqwake_intmask & ~(1 << 31), S5P_WAKEUP_MASK);

	s3c_pm_do_save(exynos_core_save, ARRAY_SIZE(exynos_core_save));

	if (soc_is_exynos5250()) {
		s3c_pm_do_save(exynos5_sys_save, ARRAY_SIZE(exynos5_sys_save));
		/* Disable USE_RETENTION of JPEG_MEM_OPTION */
		tmp = pmu_raw_readl(EXYNOS5_JPEG_MEM_OPTION);
		tmp &= ~EXYNOS5_OPTION_USE_RETENTION;
		pmu_raw_writel(tmp, EXYNOS5_JPEG_MEM_OPTION);
	}

	/* Set value of power down register for sleep mode */

	exynos_sys_powerdown_conf(SYS_SLEEP);
	pmu_raw_writel(S5P_CHECK_SLEEP, S5P_INFORM1);

	/* ensure at least INFORM0 has the resume address */

	pmu_raw_writel(virt_to_phys(exynos_cpu_resume), S5P_INFORM0);
}

static int exynos_pm_suspend(void)
{
	unsigned long tmp;

	exynos_pm_central_suspend();

	/* Setting SEQ_OPTION register */

	tmp = (S5P_USE_STANDBY_WFI0 | S5P_USE_STANDBY_WFE0);
	pmu_raw_writel(tmp, S5P_CENTRAL_SEQ_OPTION);

	if (read_cpuid_part() == ARM_CPU_PART_CORTEX_A9)
		exynos_cpu_save_register();

	return 0;
}

static void exynos_pm_resume(void)
{
	if (exynos_pm_central_resume())
		goto early_wakeup;

	if (read_cpuid_part() == ARM_CPU_PART_CORTEX_A9)
		exynos_cpu_restore_register();

	/* For release retention */

	pmu_raw_writel((1 << 28), S5P_PAD_RET_MAUDIO_OPTION);
	pmu_raw_writel((1 << 28), S5P_PAD_RET_GPIO_OPTION);
	pmu_raw_writel((1 << 28), S5P_PAD_RET_UART_OPTION);
	pmu_raw_writel((1 << 28), S5P_PAD_RET_MMCA_OPTION);
	pmu_raw_writel((1 << 28), S5P_PAD_RET_MMCB_OPTION);
	pmu_raw_writel((1 << 28), S5P_PAD_RET_EBIA_OPTION);
	pmu_raw_writel((1 << 28), S5P_PAD_RET_EBIB_OPTION);

	if (soc_is_exynos5250())
		s3c_pm_do_restore(exynos5_sys_save,
			ARRAY_SIZE(exynos5_sys_save));

	s3c_pm_do_restore_core(exynos_core_save, ARRAY_SIZE(exynos_core_save));

	if (read_cpuid_part() == ARM_CPU_PART_CORTEX_A9)
		scu_enable(S5P_VA_SCU);

early_wakeup:

	/* Clear SLEEP mode set in INFORM1 */
	pmu_raw_writel(0x0, S5P_INFORM1);

	return;
}

static struct syscore_ops exynos_pm_syscore_ops = {
	.suspend	= exynos_pm_suspend,
	.resume		= exynos_pm_resume,
};

/*
 * Suspend Ops
 */

static int exynos_suspend_enter(suspend_state_t state)
{
	int ret;

	s3c_pm_debug_init();

	S3C_PMDBG("%s: suspending the system...\n", __func__);

	S3C_PMDBG("%s: wakeup masks: %08x,%08x\n", __func__,
			exynos_irqwake_intmask, exynos_get_eint_wake_mask());

	if (exynos_irqwake_intmask == -1U
	    && exynos_get_eint_wake_mask() == -1U) {
		pr_err("%s: No wake-up sources!\n", __func__);
		pr_err("%s: Aborting sleep\n", __func__);
		return -EINVAL;
	}

	s3c_pm_save_uarts();
	exynos_pm_prepare();
	flush_cache_all();
	s3c_pm_check_store();

	ret = cpu_suspend(0, exynos_cpu_suspend);
	if (ret)
		return ret;

	s3c_pm_restore_uarts();

	S3C_PMDBG("%s: wakeup stat: %08x\n", __func__,
			pmu_raw_readl(S5P_WAKEUP_STAT));

	s3c_pm_check_restore();

	S3C_PMDBG("%s: resuming the system...\n", __func__);

	return 0;
}

static int exynos_suspend_prepare(void)
{
	s3c_pm_check_prepare();

	return 0;
}

static void exynos_suspend_finish(void)
{
	s3c_pm_check_cleanup();
}

static const struct platform_suspend_ops exynos_suspend_ops = {
	.enter		= exynos_suspend_enter,
	.prepare	= exynos_suspend_prepare,
	.finish		= exynos_suspend_finish,
	.valid		= suspend_valid_only_mem,
};

void __init exynos_pm_init(void)
{
	u32 tmp;

	/* Platform-specific GIC callback */
	gic_arch_extn.irq_set_wake = exynos_irq_set_wake;

	/* All wakeup disable */
	tmp = pmu_raw_readl(S5P_WAKEUP_MASK);
	tmp |= ((0xFF << 8) | (0x1F << 1));
	pmu_raw_writel(tmp, S5P_WAKEUP_MASK);

	register_syscore_ops(&exynos_pm_syscore_ops);
	suspend_set_ops(&exynos_suspend_ops);
}
=======
	if (soc_is_exynos3250())
		exynos_clear_boot_flag(cpuid, C2_STATE);

	cpu_pm_exit();
}

#if defined(CONFIG_SMP) && defined(CONFIG_ARM_EXYNOS_CPUIDLE)
static atomic_t cpu1_wakeup = ATOMIC_INIT(0);

static int exynos_cpu0_enter_aftr(void)
{
	int ret = -1;

	/*
	 * If the other cpu is powered on, we have to power it off, because
	 * the AFTR state won't work otherwise
	 */
	if (cpu_online(1)) {
		/*
		 * We reach a sync point with the coupled idle state, we know
		 * the other cpu will power down itself or will abort the
		 * sequence, let's wait for one of these to happen
		 */
		while (exynos_cpu_power_state(1)) {
			unsigned long boot_addr;

			/*
			 * The other cpu may skip idle and boot back
			 * up again
			 */
			if (atomic_read(&cpu1_wakeup))
				goto abort;

			/*
			 * The other cpu may bounce through idle and
			 * boot back up again, getting stuck in the
			 * boot rom code
			 */
			ret = exynos_get_boot_addr(1, &boot_addr);
			if (ret)
				goto fail;
			ret = -1;
			if (boot_addr == 0)
				goto abort;

			cpu_relax();
		}
	}

	exynos_enter_aftr();
	ret = 0;

abort:
	if (cpu_online(1)) {
		unsigned long boot_addr = virt_to_phys(exynos_cpu_resume);

		/*
		 * Set the boot vector to something non-zero
		 */
		ret = exynos_set_boot_addr(1, boot_addr);
		if (ret)
			goto fail;
		dsb();

		/*
		 * Turn on cpu1 and wait for it to be on
		 */
		exynos_cpu_power_up(1);
		while (exynos_cpu_power_state(1) != S5P_CORE_LOCAL_PWR_EN)
			cpu_relax();

		if (soc_is_exynos3250()) {
			while (!pmu_raw_readl(S5P_PMU_SPARE2) &&
			       !atomic_read(&cpu1_wakeup))
				cpu_relax();

			if (!atomic_read(&cpu1_wakeup))
				exynos_core_restart(1);
		}

		while (!atomic_read(&cpu1_wakeup)) {
			smp_rmb();

			/*
			 * Poke cpu1 out of the boot rom
			 */

			ret = exynos_set_boot_addr(1, boot_addr);
			if (ret)
				goto fail;

			call_firmware_op(cpu_boot, 1);

			if (soc_is_exynos3250())
				dsb_sev();
			else
				arch_send_wakeup_ipi_mask(cpumask_of(1));
		}
	}
fail:
	return ret;
}

static int exynos_wfi_finisher(unsigned long flags)
{
	if (soc_is_exynos3250())
		flush_cache_all();
	cpu_do_idle();

	return -1;
}

static int exynos_cpu1_powerdown(void)
{
	int ret = -1;

	/*
	 * Idle sequence for cpu1
	 */
	if (cpu_pm_enter())
		goto cpu1_aborted;

	/*
	 * Turn off cpu 1
	 */
	exynos_cpu_power_down(1);

	if (soc_is_exynos3250())
		pmu_raw_writel(0, S5P_PMU_SPARE2);

	ret = cpu_suspend(0, exynos_wfi_finisher);

	cpu_pm_exit();

cpu1_aborted:
	dsb();
	/*
	 * Notify cpu 0 that cpu 1 is awake
	 */
	atomic_set(&cpu1_wakeup, 1);

	return ret;
}

static void exynos_pre_enter_aftr(void)
{
	unsigned long boot_addr = virt_to_phys(exynos_cpu_resume);

	(void)exynos_set_boot_addr(1, boot_addr);
}

static void exynos_post_enter_aftr(void)
{
	atomic_set(&cpu1_wakeup, 0);
}

struct cpuidle_exynos_data cpuidle_coupled_exynos_data = {
	.cpu0_enter_aftr		= exynos_cpu0_enter_aftr,
	.cpu1_powerdown		= exynos_cpu1_powerdown,
	.pre_enter_aftr		= exynos_pre_enter_aftr,
	.post_enter_aftr		= exynos_post_enter_aftr,
};
#endif /* CONFIG_SMP && CONFIG_ARM_EXYNOS_CPUIDLE */
>>>>>>> v4.9.227
