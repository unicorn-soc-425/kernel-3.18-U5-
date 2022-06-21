/*
 * PowerNV setup code.
 *
 * Copyright 2011 IBM Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#undef DEBUG

#include <linux/cpu.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/tty.h>
#include <linux/reboot.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/seq_file.h>
#include <linux/of.h>
#include <linux/of_fdt.h>
#include <linux/interrupt.h>
#include <linux/bug.h>
#include <linux/pci.h>
#include <linux/cpufreq.h>

#include <asm/machdep.h>
#include <asm/firmware.h>
#include <asm/xics.h>
<<<<<<< HEAD
#include <asm/rtas.h>
#include <asm/opal.h>
#include <asm/kexec.h>
#include <asm/smp.h>

#include "powernv.h"

=======
#include <asm/opal.h>
#include <asm/kexec.h>
#include <asm/smp.h>
#include <asm/tm.h>
#include <asm/setup.h>
#include <asm/security_features.h>

#include "powernv.h"


static bool fw_feature_is(const char *state, const char *name,
			  struct device_node *fw_features)
{
	struct device_node *np;
	bool rc = false;

	np = of_get_child_by_name(fw_features, name);
	if (np) {
		rc = of_property_read_bool(np, state);
		of_node_put(np);
	}

	return rc;
}

static void init_fw_feat_flags(struct device_node *np)
{
	if (fw_feature_is("enabled", "inst-spec-barrier-ori31,31,0", np))
		security_ftr_set(SEC_FTR_SPEC_BAR_ORI31);

	if (fw_feature_is("enabled", "fw-bcctrl-serialized", np))
		security_ftr_set(SEC_FTR_BCCTRL_SERIALISED);

	if (fw_feature_is("enabled", "inst-l1d-flush-ori30,30,0", np))
		security_ftr_set(SEC_FTR_L1D_FLUSH_ORI30);

	if (fw_feature_is("enabled", "inst-l1d-flush-trig2", np))
		security_ftr_set(SEC_FTR_L1D_FLUSH_TRIG2);

	if (fw_feature_is("enabled", "fw-l1d-thread-split", np))
		security_ftr_set(SEC_FTR_L1D_THREAD_PRIV);

	if (fw_feature_is("enabled", "fw-count-cache-disabled", np))
		security_ftr_set(SEC_FTR_COUNT_CACHE_DISABLED);

	if (fw_feature_is("enabled", "fw-count-cache-flush-bcctr2,0,0", np))
		security_ftr_set(SEC_FTR_BCCTR_FLUSH_ASSIST);

	if (fw_feature_is("enabled", "needs-count-cache-flush-on-context-switch", np))
		security_ftr_set(SEC_FTR_FLUSH_COUNT_CACHE);

	/*
	 * The features below are enabled by default, so we instead look to see
	 * if firmware has *disabled* them, and clear them if so.
	 */
	if (fw_feature_is("disabled", "speculation-policy-favor-security", np))
		security_ftr_clear(SEC_FTR_FAVOUR_SECURITY);

	if (fw_feature_is("disabled", "needs-l1d-flush-msr-pr-0-to-1", np))
		security_ftr_clear(SEC_FTR_L1D_FLUSH_PR);

	if (fw_feature_is("disabled", "needs-l1d-flush-msr-hv-1-to-0", np))
		security_ftr_clear(SEC_FTR_L1D_FLUSH_HV);

	if (fw_feature_is("disabled", "needs-spec-barrier-for-bound-checks", np))
		security_ftr_clear(SEC_FTR_BNDS_CHK_SPEC_BAR);
}

static void pnv_setup_rfi_flush(void)
{
	struct device_node *np, *fw_features;
	enum l1d_flush_type type;
	bool enable;

	/* Default to fallback in case fw-features are not available */
	type = L1D_FLUSH_FALLBACK;

	np = of_find_node_by_name(NULL, "ibm,opal");
	fw_features = of_get_child_by_name(np, "fw-features");
	of_node_put(np);

	if (fw_features) {
		init_fw_feat_flags(fw_features);
		of_node_put(fw_features);

		if (security_ftr_enabled(SEC_FTR_L1D_FLUSH_TRIG2))
			type = L1D_FLUSH_MTTRIG;

		if (security_ftr_enabled(SEC_FTR_L1D_FLUSH_ORI30))
			type = L1D_FLUSH_ORI;
	}

	enable = security_ftr_enabled(SEC_FTR_FAVOUR_SECURITY) && \
		 (security_ftr_enabled(SEC_FTR_L1D_FLUSH_PR)   || \
		  security_ftr_enabled(SEC_FTR_L1D_FLUSH_HV));

	setup_rfi_flush(type, enable);
	setup_count_cache_flush();
}

>>>>>>> v4.9.227
static void __init pnv_setup_arch(void)
{
	set_arch_panic_timeout(10, ARCH_PANIC_TIMEOUT);

<<<<<<< HEAD
=======
	pnv_setup_rfi_flush();
	setup_stf_barrier();

>>>>>>> v4.9.227
	/* Initialize SMP */
	pnv_smp_init();

	/* Setup PCI */
	pnv_pci_init();

	/* Setup RTC and NVRAM callbacks */
	if (firmware_has_feature(FW_FEATURE_OPAL))
		opal_nvram_init();

	/* Enable NAP mode */
	powersave_nap = 1;

	/* XXX PMCS */
}

<<<<<<< HEAD
static void __init pnv_init_early(void)
=======
static void __init pnv_init(void)
>>>>>>> v4.9.227
{
	/*
	 * Initialize the LPC bus now so that legacy serial
	 * ports can be found on it
	 */
	opal_lpc_init();

#ifdef CONFIG_HVC_OPAL
	if (firmware_has_feature(FW_FEATURE_OPAL))
		hvc_opal_init_early();
	else
#endif
		add_preferred_console("hvc", 0, NULL);
}

static void __init pnv_init_IRQ(void)
{
	xics_init();

	WARN_ON(!ppc_md.get_irq);
}

static void pnv_show_cpuinfo(struct seq_file *m)
{
	struct device_node *root;
	const char *model = "";

	root = of_find_node_by_path("/");
	if (root)
		model = of_get_property(root, "model", NULL);
	seq_printf(m, "machine\t\t: PowerNV %s\n", model);
<<<<<<< HEAD
	if (firmware_has_feature(FW_FEATURE_OPALv3))
		seq_printf(m, "firmware\t: OPAL v3\n");
	else if (firmware_has_feature(FW_FEATURE_OPALv2))
		seq_printf(m, "firmware\t: OPAL v2\n");
	else if (firmware_has_feature(FW_FEATURE_OPAL))
		seq_printf(m, "firmware\t: OPAL v1\n");
=======
	if (firmware_has_feature(FW_FEATURE_OPAL))
		seq_printf(m, "firmware\t: OPAL\n");
>>>>>>> v4.9.227
	else
		seq_printf(m, "firmware\t: BML\n");
	of_node_put(root);
}

static void pnv_prepare_going_down(void)
{
	/*
	 * Disable all notifiers from OPAL, we can't
	 * service interrupts anymore anyway
	 */
<<<<<<< HEAD
	opal_notifier_disable();
=======
	opal_event_shutdown();
>>>>>>> v4.9.227

	/* Soft disable interrupts */
	local_irq_disable();

	/*
	 * Return secondary CPUs to firwmare if a flash update
	 * is pending otherwise we will get all sort of error
	 * messages about CPU being stuck etc.. This will also
	 * have the side effect of hard disabling interrupts so
	 * past this point, the kernel is effectively dead.
	 */
	opal_flash_term_callback();
}

static void  __noreturn pnv_restart(char *cmd)
{
	long rc = OPAL_BUSY;

	pnv_prepare_going_down();

	while (rc == OPAL_BUSY || rc == OPAL_BUSY_EVENT) {
		rc = opal_cec_reboot();
		if (rc == OPAL_BUSY_EVENT)
			opal_poll_events(NULL);
		else
			mdelay(10);
	}
	for (;;)
		opal_poll_events(NULL);
}

static void __noreturn pnv_power_off(void)
{
	long rc = OPAL_BUSY;

	pnv_prepare_going_down();

	while (rc == OPAL_BUSY || rc == OPAL_BUSY_EVENT) {
		rc = opal_cec_power_down(0);
		if (rc == OPAL_BUSY_EVENT)
			opal_poll_events(NULL);
		else
			mdelay(10);
	}
	for (;;)
		opal_poll_events(NULL);
}

static void __noreturn pnv_halt(void)
{
	pnv_power_off();
}

static void pnv_progress(char *s, unsigned short hex)
{
}

<<<<<<< HEAD
static int pnv_dma_set_mask(struct device *dev, u64 dma_mask)
{
	if (dev_is_pci(dev))
		return pnv_pci_dma_set_mask(to_pci_dev(dev), dma_mask);
	return __dma_set_mask(dev, dma_mask);
}

static u64 pnv_dma_get_required_mask(struct device *dev)
{
	if (dev_is_pci(dev))
		return pnv_pci_dma_get_required_mask(to_pci_dev(dev));

	return __dma_get_required_mask(dev);
}

=======
>>>>>>> v4.9.227
static void pnv_shutdown(void)
{
	/* Let the PCI code clear up IODA tables */
	pnv_pci_shutdown();

	/*
	 * Stop OPAL activity: Unregister all OPAL interrupts so they
	 * don't fire up while we kexec and make sure all potentially
	 * DMA'ing ops are complete (such as dump retrieval).
	 */
	opal_shutdown();
}

#ifdef CONFIG_KEXEC
static void pnv_kexec_wait_secondaries_down(void)
{
	int my_cpu, i, notified = -1;

	my_cpu = get_cpu();

	for_each_online_cpu(i) {
		uint8_t status;
<<<<<<< HEAD
		int64_t rc;
=======
		int64_t rc, timeout = 1000;
>>>>>>> v4.9.227

		if (i == my_cpu)
			continue;

		for (;;) {
			rc = opal_query_cpu_status(get_hard_smp_processor_id(i),
						   &status);
			if (rc != OPAL_SUCCESS || status != OPAL_THREAD_STARTED)
				break;
			barrier();
			if (i != notified) {
				printk(KERN_INFO "kexec: waiting for cpu %d "
				       "(physical %d) to enter OPAL\n",
				       i, paca[i].hw_cpu_id);
				notified = i;
			}
<<<<<<< HEAD
=======

			/*
			 * On crash secondaries might be unreachable or hung,
			 * so timeout if we've waited too long
			 * */
			mdelay(1);
			if (timeout-- == 0) {
				printk(KERN_ERR "kexec: timed out waiting for "
				       "cpu %d (physical %d) to enter OPAL\n",
				       i, paca[i].hw_cpu_id);
				break;
			}
>>>>>>> v4.9.227
		}
	}
}

static void pnv_kexec_cpu_down(int crash_shutdown, int secondary)
{
	xics_kexec_teardown_cpu(secondary);

<<<<<<< HEAD
	/* On OPAL v3, we return all CPUs to firmware */

	if (!firmware_has_feature(FW_FEATURE_OPALv3))
=======
	/* On OPAL, we return all CPUs to firmware */

	if (!firmware_has_feature(FW_FEATURE_OPAL))
>>>>>>> v4.9.227
		return;

	if (secondary) {
		/* Return secondary CPUs to firmware on OPAL v3 */
		mb();
		get_paca()->kexec_state = KEXEC_STATE_REAL_MODE;
		mb();

		/* Return the CPU to OPAL */
		opal_return_cpu();
<<<<<<< HEAD
	} else if (crash_shutdown) {
		/*
		 * On crash, we don't wait for secondaries to go
		 * down as they might be unreachable or hung, so
		 * instead we just wait a bit and move on.
		 */
		mdelay(1);
	} else {
		/* Primary waits for the secondaries to have reached OPAL */
		pnv_kexec_wait_secondaries_down();
=======
	} else {
		/* Primary waits for the secondaries to have reached OPAL */
		pnv_kexec_wait_secondaries_down();

		/*
		 * We might be running as little-endian - now that interrupts
		 * are disabled, reset the HILE bit to big-endian so we don't
		 * take interrupts in the wrong endian later
		 */
		opal_reinit_cpus(OPAL_REINIT_CPUS_HILE_BE);
>>>>>>> v4.9.227
	}
}
#endif /* CONFIG_KEXEC */

#ifdef CONFIG_MEMORY_HOTPLUG_SPARSE
static unsigned long pnv_memory_block_size(void)
{
	return 256UL * 1024 * 1024;
}
#endif

static void __init pnv_setup_machdep_opal(void)
{
	ppc_md.get_boot_time = opal_get_boot_time;
<<<<<<< HEAD
	ppc_md.get_rtc_time = opal_get_rtc_time;
	ppc_md.set_rtc_time = opal_set_rtc_time;
	ppc_md.restart = pnv_restart;
	ppc_md.power_off = pnv_power_off;
=======
	ppc_md.restart = pnv_restart;
	pm_power_off = pnv_power_off;
>>>>>>> v4.9.227
	ppc_md.halt = pnv_halt;
	ppc_md.machine_check_exception = opal_machine_check;
	ppc_md.mce_check_early_recovery = opal_mce_check_early_recovery;
	ppc_md.hmi_exception_early = opal_hmi_exception_early;
	ppc_md.handle_hmi_exception = opal_handle_hmi_exception;
}

<<<<<<< HEAD
#ifdef CONFIG_PPC_POWERNV_RTAS
static void __init pnv_setup_machdep_rtas(void)
{
	if (rtas_token("get-time-of-day") != RTAS_UNKNOWN_SERVICE) {
		ppc_md.get_boot_time = rtas_get_boot_time;
		ppc_md.get_rtc_time = rtas_get_rtc_time;
		ppc_md.set_rtc_time = rtas_set_rtc_time;
	}
	ppc_md.restart = rtas_restart;
	ppc_md.power_off = rtas_power_off;
	ppc_md.halt = rtas_halt;
}
#endif /* CONFIG_PPC_POWERNV_RTAS */

static int __init pnv_probe(void)
{
	unsigned long root = of_get_flat_dt_root();

	if (!of_flat_dt_is_compatible(root, "ibm,powernv"))
		return 0;

	hpte_init_native();

	if (firmware_has_feature(FW_FEATURE_OPAL))
		pnv_setup_machdep_opal();
#ifdef CONFIG_PPC_POWERNV_RTAS
	else if (rtas.base)
		pnv_setup_machdep_rtas();
#endif /* CONFIG_PPC_POWERNV_RTAS */

	pr_debug("PowerNV detected !\n");

=======
static int __init pnv_probe(void)
{
	if (!of_machine_is_compatible("ibm,powernv"))
		return 0;

	if (firmware_has_feature(FW_FEATURE_OPAL))
		pnv_setup_machdep_opal();

	pr_debug("PowerNV detected !\n");

	pnv_init();

>>>>>>> v4.9.227
	return 1;
}

/*
 * Returns the cpu frequency for 'cpu' in Hz. This is used by
 * /proc/cpuinfo
 */
static unsigned long pnv_get_proc_freq(unsigned int cpu)
{
	unsigned long ret_freq;

	ret_freq = cpufreq_get(cpu) * 1000ul;

	/*
	 * If the backend cpufreq driver does not exist,
         * then fallback to old way of reporting the clockrate.
	 */
	if (!ret_freq)
		ret_freq = ppc_proc_freq;
	return ret_freq;
}

define_machine(powernv) {
	.name			= "PowerNV",
	.probe			= pnv_probe,
<<<<<<< HEAD
	.init_early		= pnv_init_early,
=======
>>>>>>> v4.9.227
	.setup_arch		= pnv_setup_arch,
	.init_IRQ		= pnv_init_IRQ,
	.show_cpuinfo		= pnv_show_cpuinfo,
	.get_proc_freq          = pnv_get_proc_freq,
	.progress		= pnv_progress,
	.machine_shutdown	= pnv_shutdown,
<<<<<<< HEAD
	.power_save             = power7_idle,
	.calibrate_decr		= generic_calibrate_decr,
	.dma_set_mask		= pnv_dma_set_mask,
	.dma_get_required_mask	= pnv_dma_get_required_mask,
=======
	.power_save             = NULL,
	.calibrate_decr		= generic_calibrate_decr,
>>>>>>> v4.9.227
#ifdef CONFIG_KEXEC
	.kexec_cpu_down		= pnv_kexec_cpu_down,
#endif
#ifdef CONFIG_MEMORY_HOTPLUG_SPARSE
	.memory_block_size	= pnv_memory_block_size,
#endif
};
