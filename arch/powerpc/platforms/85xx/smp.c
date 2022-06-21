/*
 * Author: Andy Fleming <afleming@freescale.com>
 * 	   Kumar Gala <galak@kernel.crashing.org>
 *
<<<<<<< HEAD
 * Copyright 2006-2008, 2011-2012 Freescale Semiconductor Inc.
=======
 * Copyright 2006-2008, 2011-2012, 2015 Freescale Semiconductor Inc.
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/of.h>
<<<<<<< HEAD
#include <linux/of_address.h>
#include <linux/kexec.h>
#include <linux/highmem.h>
#include <linux/cpu.h>
=======
#include <linux/kexec.h>
#include <linux/highmem.h>
#include <linux/cpu.h>
#include <linux/fsl/guts.h>
>>>>>>> v4.9.227

#include <asm/machdep.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/mpic.h>
#include <asm/cacheflush.h>
#include <asm/dbell.h>
<<<<<<< HEAD
#include <asm/fsl_guts.h>
#include <asm/code-patching.h>
#include <asm/cputhreads.h>
=======
#include <asm/code-patching.h>
#include <asm/cputhreads.h>
#include <asm/fsl_pm.h>
>>>>>>> v4.9.227

#include <sysdev/fsl_soc.h>
#include <sysdev/mpic.h>
#include "smp.h"

struct epapr_spin_table {
	u32	addr_h;
	u32	addr_l;
	u32	r3_h;
	u32	r3_l;
	u32	reserved;
	u32	pir;
};

<<<<<<< HEAD
static struct ccsr_guts __iomem *guts;
=======
#ifdef CONFIG_HOTPLUG_CPU
>>>>>>> v4.9.227
static u64 timebase;
static int tb_req;
static int tb_valid;

<<<<<<< HEAD
static void mpc85xx_timebase_freeze(int freeze)
{
	uint32_t mask;

	mask = CCSR_GUTS_DEVDISR_TB0 | CCSR_GUTS_DEVDISR_TB1;
	if (freeze)
		setbits32(&guts->devdisr, mask);
	else
		clrbits32(&guts->devdisr, mask);

	in_be32(&guts->devdisr);
}

=======
>>>>>>> v4.9.227
static void mpc85xx_give_timebase(void)
{
	unsigned long flags;

	local_irq_save(flags);
<<<<<<< HEAD
=======
	hard_irq_disable();
>>>>>>> v4.9.227

	while (!tb_req)
		barrier();
	tb_req = 0;

<<<<<<< HEAD
	mpc85xx_timebase_freeze(1);
=======
	qoriq_pm_ops->freeze_time_base(true);
>>>>>>> v4.9.227
#ifdef CONFIG_PPC64
	/*
	 * e5500/e6500 have a workaround for erratum A-006958 in place
	 * that will reread the timebase until TBL is non-zero.
	 * That would be a bad thing when the timebase is frozen.
	 *
	 * Thus, we read it manually, and instead of checking that
	 * TBL is non-zero, we ensure that TB does not change.  We don't
	 * do that for the main mftb implementation, because it requires
	 * a scratch register
	 */
	{
		u64 prev;

		asm volatile("mfspr %0, %1" : "=r" (timebase) :
			     "i" (SPRN_TBRL));

		do {
			prev = timebase;
			asm volatile("mfspr %0, %1" : "=r" (timebase) :
				     "i" (SPRN_TBRL));
		} while (prev != timebase);
	}
#else
	timebase = get_tb();
#endif
	mb();
	tb_valid = 1;

	while (tb_valid)
		barrier();

<<<<<<< HEAD
	mpc85xx_timebase_freeze(0);
=======
	qoriq_pm_ops->freeze_time_base(false);
>>>>>>> v4.9.227

	local_irq_restore(flags);
}

static void mpc85xx_take_timebase(void)
{
	unsigned long flags;

	local_irq_save(flags);
<<<<<<< HEAD
=======
	hard_irq_disable();
>>>>>>> v4.9.227

	tb_req = 1;
	while (!tb_valid)
		barrier();

	set_tb(timebase >> 32, timebase & 0xffffffff);
	isync();
	tb_valid = 0;

	local_irq_restore(flags);
}

<<<<<<< HEAD
#ifdef CONFIG_HOTPLUG_CPU
static void smp_85xx_mach_cpu_die(void)
{
	unsigned int cpu = smp_processor_id();
	u32 tmp;

	local_irq_disable();
	idle_task_exit();
	generic_set_cpu_dead(cpu);
	mb();

	mtspr(SPRN_TCR, 0);

	__flush_disable_L1();
	tmp = (mfspr(SPRN_HID0) & ~(HID0_DOZE|HID0_SLEEP)) | HID0_NAP;
	mtspr(SPRN_HID0, tmp);
	isync();

	/* Enter NAP mode. */
	tmp = mfmsr();
	tmp |= MSR_WE;
	mb();
	mtmsr(tmp);
	isync();
=======
static void smp_85xx_mach_cpu_die(void)
{
	unsigned int cpu = smp_processor_id();

	local_irq_disable();
	hard_irq_disable();
	/* mask all irqs to prevent cpu wakeup */
	qoriq_pm_ops->irq_mask(cpu);

	idle_task_exit();

	mtspr(SPRN_TCR, 0);
	mtspr(SPRN_TSR, mfspr(SPRN_TSR));

	generic_set_cpu_dead(cpu);

	cur_cpu_spec->cpu_down_flush();

	qoriq_pm_ops->cpu_die(cpu);
>>>>>>> v4.9.227

	while (1)
		;
}
<<<<<<< HEAD
#endif

=======

static void qoriq_cpu_kill(unsigned int cpu)
{
	int i;

	for (i = 0; i < 500; i++) {
		if (is_cpu_dead(cpu)) {
#ifdef CONFIG_PPC64
			paca[cpu].cpu_start = 0;
#endif
			return;
		}
		msleep(20);
	}
	pr_err("CPU%d didn't die...\n", cpu);
}
#endif

/*
 * To keep it compatible with old boot program which uses
 * cache-inhibit spin table, we need to flush the cache
 * before accessing spin table to invalidate any staled data.
 * We also need to flush the cache after writing to spin
 * table to push data out.
 */
>>>>>>> v4.9.227
static inline void flush_spin_table(void *spin_table)
{
	flush_dcache_range((ulong)spin_table,
		(ulong)spin_table + sizeof(struct epapr_spin_table));
}

static inline u32 read_spin_table_addr_l(void *spin_table)
{
	flush_dcache_range((ulong)spin_table,
		(ulong)spin_table + sizeof(struct epapr_spin_table));
	return in_be32(&((struct epapr_spin_table *)spin_table)->addr_l);
}

#ifdef CONFIG_PPC64
static void wake_hw_thread(void *info)
{
	void fsl_secondary_thread_init(void);
<<<<<<< HEAD
	unsigned long imsr1, inia1;
	int nr = *(const int *)info;

	imsr1 = MSR_KERNEL;
	inia1 = *(unsigned long *)fsl_secondary_thread_init;

	mttmr(TMRN_IMSR1, imsr1);
	mttmr(TMRN_INIA1, inia1);
	mtspr(SPRN_TENS, TEN_THREAD(1));

	smp_generic_kick_cpu(nr);
}
#endif

static int smp_85xx_kick_cpu(int nr)
{
	unsigned long flags;
	const u64 *cpu_rel_addr;
	__iomem struct epapr_spin_table *spin_table;
	struct device_node *np;
	int hw_cpu = get_hard_smp_processor_id(nr);
	int ioremappable;
	int ret = 0;

	WARN_ON(nr < 0 || nr >= NR_CPUS);
	WARN_ON(hw_cpu < 0 || hw_cpu >= NR_CPUS);

	pr_debug("smp_85xx_kick_cpu: kick CPU #%d\n", nr);

#ifdef CONFIG_PPC64
	/* Threads don't use the spin table */
	if (cpu_thread_in_core(nr) != 0) {
		int primary = cpu_first_thread_sibling(nr);

		if (WARN_ON_ONCE(!cpu_has_feature(CPU_FTR_SMT)))
			return -ENOENT;

		if (cpu_thread_in_core(nr) != 1) {
			pr_err("%s: cpu %d: invalid hw thread %d\n",
			       __func__, nr, cpu_thread_in_core(nr));
			return -ENOENT;
		}

		if (!cpu_online(primary)) {
			pr_err("%s: cpu %d: primary %d not online\n",
			       __func__, nr, primary);
			return -ENOENT;
		}

		smp_call_function_single(primary, wake_hw_thread, &nr, 0);
		return 0;
	}
#endif

	np = of_get_cpu_node(nr, NULL);
	cpu_rel_addr = of_get_property(np, "cpu-release-addr", NULL);

	if (cpu_rel_addr == NULL) {
		printk(KERN_ERR "No cpu-release-addr for cpu %d\n", nr);
=======
	unsigned long inia;
	int cpu = *(const int *)info;

	inia = *(unsigned long *)fsl_secondary_thread_init;
	book3e_start_thread(cpu_thread_in_core(cpu), inia);
}
#endif

static int smp_85xx_start_cpu(int cpu)
{
	int ret = 0;
	struct device_node *np;
	const u64 *cpu_rel_addr;
	unsigned long flags;
	int ioremappable;
	int hw_cpu = get_hard_smp_processor_id(cpu);
	struct epapr_spin_table __iomem *spin_table;

	np = of_get_cpu_node(cpu, NULL);
	cpu_rel_addr = of_get_property(np, "cpu-release-addr", NULL);
	if (!cpu_rel_addr) {
		pr_err("No cpu-release-addr for cpu %d\n", cpu);
>>>>>>> v4.9.227
		return -ENOENT;
	}

	/*
	 * A secondary core could be in a spinloop in the bootpage
	 * (0xfffff000), somewhere in highmem, or somewhere in lowmem.
	 * The bootpage and highmem can be accessed via ioremap(), but
	 * we need to directly access the spinloop if its in lowmem.
	 */
	ioremappable = *cpu_rel_addr > virt_to_phys(high_memory);

	/* Map the spin table */
	if (ioremappable)
		spin_table = ioremap_prot(*cpu_rel_addr,
			sizeof(struct epapr_spin_table), _PAGE_COHERENT);
	else
		spin_table = phys_to_virt(*cpu_rel_addr);

	local_irq_save(flags);
<<<<<<< HEAD
#ifdef CONFIG_PPC32
#ifdef CONFIG_HOTPLUG_CPU
	/* Corresponding to generic_set_cpu_dead() */
	generic_set_cpu_up(nr);

	if (system_state == SYSTEM_RUNNING) {
		/*
		 * To keep it compatible with old boot program which uses
		 * cache-inhibit spin table, we need to flush the cache
		 * before accessing spin table to invalidate any staled data.
		 * We also need to flush the cache after writing to spin
		 * table to push data out.
		 */
		flush_spin_table(spin_table);
		out_be32(&spin_table->addr_l, 0);
		flush_spin_table(spin_table);

=======
	hard_irq_disable();

	if (qoriq_pm_ops)
		qoriq_pm_ops->cpu_up_prepare(cpu);

	/* if cpu is not spinning, reset it */
	if (read_spin_table_addr_l(spin_table) != 1) {
>>>>>>> v4.9.227
		/*
		 * We don't set the BPTR register here since it already points
		 * to the boot page properly.
		 */
<<<<<<< HEAD
		mpic_reset_core(nr);
=======
		mpic_reset_core(cpu);
>>>>>>> v4.9.227

		/*
		 * wait until core is ready...
		 * We need to invalidate the stale data, in case the boot
		 * loader uses a cache-inhibited spin table.
		 */
		if (!spin_event_timeout(
				read_spin_table_addr_l(spin_table) == 1,
				10000, 100)) {
<<<<<<< HEAD
			pr_err("%s: timeout waiting for core %d to reset\n",
							__func__, hw_cpu);
			ret = -ENOENT;
			goto out;
		}

		/*  clear the acknowledge status */
		__secondary_hold_acknowledge = -1;
	}
#endif
	flush_spin_table(spin_table);
	out_be32(&spin_table->pir, hw_cpu);
	out_be32(&spin_table->addr_l, __pa(__early_start));
	flush_spin_table(spin_table);

	/* Wait a bit for the CPU to ack. */
	if (!spin_event_timeout(__secondary_hold_acknowledge == hw_cpu,
					10000, 100)) {
		pr_err("%s: timeout waiting for core %d to ack\n",
						__func__, hw_cpu);
		ret = -ENOENT;
		goto out;
	}
out:
#else
	smp_generic_kick_cpu(nr);

	flush_spin_table(spin_table);
	out_be32(&spin_table->pir, hw_cpu);
	out_be64((u64 *)(&spin_table->addr_h),
		__pa(ppc_function_entry(generic_secondary_smp_init)));
	flush_spin_table(spin_table);
#endif

=======
			pr_err("timeout waiting for cpu %d to reset\n",
				hw_cpu);
			ret = -EAGAIN;
			goto err;
		}
	}

	flush_spin_table(spin_table);
	out_be32(&spin_table->pir, hw_cpu);
#ifdef CONFIG_PPC64
	out_be64((u64 *)(&spin_table->addr_h),
		__pa(ppc_function_entry(generic_secondary_smp_init)));
#else
	out_be32(&spin_table->addr_l, __pa(__early_start));
#endif
	flush_spin_table(spin_table);
err:
>>>>>>> v4.9.227
	local_irq_restore(flags);

	if (ioremappable)
		iounmap(spin_table);

	return ret;
}

<<<<<<< HEAD
=======
static int smp_85xx_kick_cpu(int nr)
{
	int ret = 0;
#ifdef CONFIG_PPC64
	int primary = nr;
#endif

	WARN_ON(nr < 0 || nr >= num_possible_cpus());

	pr_debug("kick CPU #%d\n", nr);

#ifdef CONFIG_PPC64
	if (threads_per_core == 2) {
		if (WARN_ON_ONCE(!cpu_has_feature(CPU_FTR_SMT)))
			return -ENOENT;

		booting_thread_hwid = cpu_thread_in_core(nr);
		primary = cpu_first_thread_sibling(nr);

		if (qoriq_pm_ops)
			qoriq_pm_ops->cpu_up_prepare(nr);

		/*
		 * If either thread in the core is online, use it to start
		 * the other.
		 */
		if (cpu_online(primary)) {
			smp_call_function_single(primary,
					wake_hw_thread, &nr, 1);
			goto done;
		} else if (cpu_online(primary + 1)) {
			smp_call_function_single(primary + 1,
					wake_hw_thread, &nr, 1);
			goto done;
		}

		/*
		 * If getting here, it means both threads in the core are
		 * offline. So start the primary thread, then it will start
		 * the thread specified in booting_thread_hwid, the one
		 * corresponding to nr.
		 */

	} else if (threads_per_core == 1) {
		/*
		 * If one core has only one thread, set booting_thread_hwid to
		 * an invalid value.
		 */
		booting_thread_hwid = INVALID_THREAD_HWID;

	} else if (threads_per_core > 2) {
		pr_err("Do not support more than 2 threads per CPU.");
		return -EINVAL;
	}

	ret = smp_85xx_start_cpu(primary);
	if (ret)
		return ret;

done:
	paca[nr].cpu_start = 1;
	generic_set_cpu_up(nr);

	return ret;
#else
	ret = smp_85xx_start_cpu(nr);
	if (ret)
		return ret;

	generic_set_cpu_up(nr);

	return ret;
#endif
}

>>>>>>> v4.9.227
struct smp_ops_t smp_85xx_ops = {
	.kick_cpu = smp_85xx_kick_cpu,
	.cpu_bootable = smp_generic_cpu_bootable,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_disable	= generic_cpu_disable,
	.cpu_die	= generic_cpu_die,
#endif
<<<<<<< HEAD
#ifdef CONFIG_KEXEC
=======
#if defined(CONFIG_KEXEC) && !defined(CONFIG_PPC64)
>>>>>>> v4.9.227
	.give_timebase	= smp_generic_give_timebase,
	.take_timebase	= smp_generic_take_timebase,
#endif
};

#ifdef CONFIG_KEXEC
<<<<<<< HEAD
=======
#ifdef CONFIG_PPC32
>>>>>>> v4.9.227
atomic_t kexec_down_cpus = ATOMIC_INIT(0);

void mpc85xx_smp_kexec_cpu_down(int crash_shutdown, int secondary)
{
	local_irq_disable();

	if (secondary) {
<<<<<<< HEAD
=======
		cur_cpu_spec->cpu_down_flush();
>>>>>>> v4.9.227
		atomic_inc(&kexec_down_cpus);
		/* loop forever */
		while (1);
	}
}

static void mpc85xx_smp_kexec_down(void *arg)
{
	if (ppc_md.kexec_cpu_down)
		ppc_md.kexec_cpu_down(0,1);
}
<<<<<<< HEAD

static void map_and_flush(unsigned long paddr)
{
	struct page *page = pfn_to_page(paddr >> PAGE_SHIFT);
	unsigned long kaddr  = (unsigned long)kmap(page);

	flush_dcache_range(kaddr, kaddr + PAGE_SIZE);
	kunmap(page);
}

/**
 * Before we reset the other cores, we need to flush relevant cache
 * out to memory so we don't get anything corrupted, some of these flushes
 * are performed out of an overabundance of caution as interrupts are not
 * disabled yet and we can switch cores
 */
static void mpc85xx_smp_flush_dcache_kexec(struct kimage *image)
{
	kimage_entry_t *ptr, entry;
	unsigned long paddr;
	int i;

	if (image->type == KEXEC_TYPE_DEFAULT) {
		/* normal kexec images are stored in temporary pages */
		for (ptr = &image->head; (entry = *ptr) && !(entry & IND_DONE);
		     ptr = (entry & IND_INDIRECTION) ?
				phys_to_virt(entry & PAGE_MASK) : ptr + 1) {
			if (!(entry & IND_DESTINATION)) {
				map_and_flush(entry);
			}
		}
		/* flush out last IND_DONE page */
		map_and_flush(entry);
	} else {
		/* crash type kexec images are copied to the crash region */
		for (i = 0; i < image->nr_segments; i++) {
			struct kexec_segment *seg = &image->segment[i];
			for (paddr = seg->mem; paddr < seg->mem + seg->memsz;
			     paddr += PAGE_SIZE) {
				map_and_flush(paddr);
			}
		}
	}

	/* also flush the kimage struct to be passed in as well */
	flush_dcache_range((unsigned long)image,
			   (unsigned long)image + sizeof(*image));
}

static void mpc85xx_smp_machine_kexec(struct kimage *image)
{
	int timeout = INT_MAX;
	int i, num_cpus = num_present_cpus();

	mpc85xx_smp_flush_dcache_kexec(image);

=======
#else
void mpc85xx_smp_kexec_cpu_down(int crash_shutdown, int secondary)
{
	int cpu = smp_processor_id();
	int sibling = cpu_last_thread_sibling(cpu);
	bool notified = false;
	int disable_cpu;
	int disable_threadbit = 0;
	long start = mftb();
	long now;

	local_irq_disable();
	hard_irq_disable();
	mpic_teardown_this_cpu(secondary);

	if (cpu == crashing_cpu && cpu_thread_in_core(cpu) != 0) {
		/*
		 * We enter the crash kernel on whatever cpu crashed,
		 * even if it's a secondary thread.  If that's the case,
		 * disable the corresponding primary thread.
		 */
		disable_threadbit = 1;
		disable_cpu = cpu_first_thread_sibling(cpu);
	} else if (sibling != crashing_cpu &&
		   cpu_thread_in_core(cpu) == 0 &&
		   cpu_thread_in_core(sibling) != 0) {
		disable_threadbit = 2;
		disable_cpu = sibling;
	}

	if (disable_threadbit) {
		while (paca[disable_cpu].kexec_state < KEXEC_STATE_REAL_MODE) {
			barrier();
			now = mftb();
			if (!notified && now - start > 1000000) {
				pr_info("%s/%d: waiting for cpu %d to enter KEXEC_STATE_REAL_MODE (%d)\n",
					__func__, smp_processor_id(),
					disable_cpu,
					paca[disable_cpu].kexec_state);
				notified = true;
			}
		}

		if (notified) {
			pr_info("%s: cpu %d done waiting\n",
				__func__, disable_cpu);
		}

		mtspr(SPRN_TENC, disable_threadbit);
		while (mfspr(SPRN_TENSR) & disable_threadbit)
			cpu_relax();
	}
}
#endif

static void mpc85xx_smp_machine_kexec(struct kimage *image)
{
#ifdef CONFIG_PPC32
	int timeout = INT_MAX;
	int i, num_cpus = num_present_cpus();

>>>>>>> v4.9.227
	if (image->type == KEXEC_TYPE_DEFAULT)
		smp_call_function(mpc85xx_smp_kexec_down, NULL, 0);

	while ( (atomic_read(&kexec_down_cpus) != (num_cpus - 1)) &&
		( timeout > 0 ) )
	{
		timeout--;
	}

	if ( !timeout )
		printk(KERN_ERR "Unable to bring down secondary cpu(s)");

	for_each_online_cpu(i)
	{
		if ( i == smp_processor_id() ) continue;
		mpic_reset_core(i);
	}
<<<<<<< HEAD
=======
#endif
>>>>>>> v4.9.227

	default_machine_kexec(image);
}
#endif /* CONFIG_KEXEC */

static void smp_85xx_basic_setup(int cpu_nr)
{
	if (cpu_has_feature(CPU_FTR_DBELL))
		doorbell_setup_this_cpu();
}

static void smp_85xx_setup_cpu(int cpu_nr)
{
	mpic_setup_this_cpu();
	smp_85xx_basic_setup(cpu_nr);
}

<<<<<<< HEAD
static const struct of_device_id mpc85xx_smp_guts_ids[] = {
	{ .compatible = "fsl,mpc8572-guts", },
	{ .compatible = "fsl,p1020-guts", },
	{ .compatible = "fsl,p1021-guts", },
	{ .compatible = "fsl,p1022-guts", },
	{ .compatible = "fsl,p1023-guts", },
	{ .compatible = "fsl,p2020-guts", },
	{},
};

=======
>>>>>>> v4.9.227
void __init mpc85xx_smp_init(void)
{
	struct device_node *np;


	np = of_find_node_by_type(NULL, "open-pic");
	if (np) {
		smp_85xx_ops.probe = smp_mpic_probe;
		smp_85xx_ops.setup_cpu = smp_85xx_setup_cpu;
		smp_85xx_ops.message_pass = smp_mpic_message_pass;
	} else
		smp_85xx_ops.setup_cpu = smp_85xx_basic_setup;

	if (cpu_has_feature(CPU_FTR_DBELL)) {
		/*
		 * If left NULL, .message_pass defaults to
		 * smp_muxed_ipi_message_pass
		 */
		smp_85xx_ops.message_pass = NULL;
		smp_85xx_ops.cause_ipi = doorbell_cause_ipi;
		smp_85xx_ops.probe = NULL;
	}

<<<<<<< HEAD
	np = of_find_matching_node(NULL, mpc85xx_smp_guts_ids);
	if (np) {
		guts = of_iomap(np, 0);
		of_node_put(np);
		if (!guts) {
			pr_err("%s: Could not map guts node address\n",
								__func__);
			return;
		}
		smp_85xx_ops.give_timebase = mpc85xx_give_timebase;
		smp_85xx_ops.take_timebase = mpc85xx_take_timebase;
#ifdef CONFIG_HOTPLUG_CPU
		ppc_md.cpu_die = smp_85xx_mach_cpu_die;
#endif
	}

=======
#ifdef CONFIG_HOTPLUG_CPU
#ifdef CONFIG_FSL_CORENET_RCPM
	fsl_rcpm_init();
#endif

#ifdef CONFIG_FSL_PMC
	mpc85xx_setup_pmc();
#endif
	if (qoriq_pm_ops) {
		smp_85xx_ops.give_timebase = mpc85xx_give_timebase;
		smp_85xx_ops.take_timebase = mpc85xx_take_timebase;
		ppc_md.cpu_die = smp_85xx_mach_cpu_die;
		smp_85xx_ops.cpu_die = qoriq_cpu_kill;
	}
#endif
>>>>>>> v4.9.227
	smp_ops = &smp_85xx_ops;

#ifdef CONFIG_KEXEC
	ppc_md.kexec_cpu_down = mpc85xx_smp_kexec_cpu_down;
	ppc_md.machine_kexec = mpc85xx_smp_machine_kexec;
#endif
}
