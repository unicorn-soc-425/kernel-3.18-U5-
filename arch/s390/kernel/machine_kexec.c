/*
 * Copyright IBM Corp. 2005, 2011
 *
 * Author(s): Rolf Adelsberger,
 *	      Heiko Carstens <heiko.carstens@de.ibm.com>
 *	      Michael Holzheu <holzheu@linux.vnet.ibm.com>
 */

#include <linux/device.h>
#include <linux/mm.h>
#include <linux/kexec.h>
#include <linux/delay.h>
#include <linux/reboot.h>
#include <linux/ftrace.h>
#include <linux/debug_locks.h>
#include <linux/suspend.h>
#include <asm/cio.h>
#include <asm/setup.h>
#include <asm/pgtable.h>
#include <asm/pgalloc.h>
#include <asm/smp.h>
#include <asm/reset.h>
#include <asm/ipl.h>
#include <asm/diag.h>
#include <asm/elf.h>
#include <asm/asm-offsets.h>
<<<<<<< HEAD
=======
#include <asm/cacheflush.h>
>>>>>>> v4.9.227
#include <asm/os_info.h>
#include <asm/switch_to.h>

typedef void (*relocate_kernel_t)(kimage_entry_t *, unsigned long);

extern const unsigned char relocate_kernel[];
extern const unsigned long long relocate_kernel_len;

#ifdef CONFIG_CRASH_DUMP

/*
<<<<<<< HEAD
 * Create ELF notes for one CPU
 */
static void add_elf_notes(int cpu)
{
	struct save_area *sa = (void *) 4608 + store_prefix();
	void *ptr;

	memcpy((void *) (4608UL + sa->pref_reg), sa, sizeof(*sa));
	ptr = (u64 *) per_cpu_ptr(crash_notes, cpu);
	ptr = fill_cpu_elf_notes(ptr, sa, NULL);
	memset(ptr, 0, sizeof(struct elf_note));
}

/*
 * Initialize CPU ELF notes
 */
static void setup_regs(void)
{
	unsigned long sa = S390_lowcore.prefixreg_save_area + SAVE_AREA_BASE;
	struct _lowcore *lc;
	int cpu, this_cpu;

	/* Get lowcore pointer from store status of this CPU (absolute zero) */
	lc = (struct _lowcore *)(unsigned long)S390_lowcore.prefixreg_save_area;
	this_cpu = smp_find_processor_id(stap());
	add_elf_notes(this_cpu);
	for_each_online_cpu(cpu) {
		if (cpu == this_cpu)
			continue;
		if (smp_store_status(cpu))
			continue;
		add_elf_notes(cpu);
	}
	if (MACHINE_HAS_VX)
		save_vx_regs_safe((void *) lc->vector_save_area_addr);
	/* Copy dump CPU store status info to absolute zero */
	memcpy((void *) SAVE_AREA_BASE, (void *) sa, sizeof(struct save_area));
}

/*
=======
>>>>>>> v4.9.227
 * PM notifier callback for kdump
 */
static int machine_kdump_pm_cb(struct notifier_block *nb, unsigned long action,
			       void *ptr)
{
	switch (action) {
	case PM_SUSPEND_PREPARE:
	case PM_HIBERNATION_PREPARE:
<<<<<<< HEAD
		if (crashk_res.start)
			crash_map_reserved_pages();
		break;
	case PM_POST_SUSPEND:
	case PM_POST_HIBERNATION:
		if (crashk_res.start)
			crash_unmap_reserved_pages();
=======
		if (kexec_crash_image)
			arch_kexec_unprotect_crashkres();
		break;
	case PM_POST_SUSPEND:
	case PM_POST_HIBERNATION:
		if (kexec_crash_image)
			arch_kexec_protect_crashkres();
>>>>>>> v4.9.227
		break;
	default:
		return NOTIFY_DONE;
	}
	return NOTIFY_OK;
}

static int __init machine_kdump_pm_init(void)
{
	pm_notifier(machine_kdump_pm_cb, 0);
	return 0;
}
arch_initcall(machine_kdump_pm_init);
<<<<<<< HEAD
#endif

/*
 * Start kdump: We expect here that a store status has been done on our CPU
 */
static void __do_machine_kdump(void *image)
{
#ifdef CONFIG_CRASH_DUMP
	int (*start_kdump)(int) = (void *)((struct kimage *) image)->start;

	setup_regs();
	__load_psw_mask(PSW_MASK_BASE | PSW_DEFAULT_KEY | PSW_MASK_EA | PSW_MASK_BA);
	start_kdump(1);
#endif
}

/*
=======

/*
 * Reset the system, copy boot CPU registers to absolute zero,
 * and jump to the kdump image
 */
static void __do_machine_kdump(void *image)
{
	int (*start_kdump)(int);
	unsigned long prefix;

	/* store_status() saved the prefix register to lowcore */
	prefix = (unsigned long) S390_lowcore.prefixreg_save_area;

	/* Now do the reset  */
	s390_reset_system();

	/*
	 * Copy dump CPU store status info to absolute zero.
	 * This need to be done *after* s390_reset_system set the
	 * prefix register of this CPU to zero
	 */
	memcpy((void *) __LC_FPREGS_SAVE_AREA,
	       (void *)(prefix + __LC_FPREGS_SAVE_AREA), 512);

	__load_psw_mask(PSW_MASK_BASE | PSW_DEFAULT_KEY | PSW_MASK_EA | PSW_MASK_BA);
	start_kdump = (void *)((struct kimage *) image)->start;
	start_kdump(1);

	/* Die if start_kdump returns */
	disabled_wait((unsigned long) __builtin_return_address(0));
}

/*
 * Start kdump: create a LGR log entry, store status of all CPUs and
 * branch to __do_machine_kdump.
 */
static noinline void __machine_kdump(void *image)
{
	int this_cpu, cpu;

	lgr_info_log();
	/* Get status of the other CPUs */
	this_cpu = smp_find_processor_id(stap());
	for_each_online_cpu(cpu) {
		if (cpu == this_cpu)
			continue;
		if (smp_store_status(cpu))
			continue;
	}
	/* Store status of the boot CPU */
	if (MACHINE_HAS_VX)
		save_vx_regs((void *) &S390_lowcore.vector_save_area);
	/*
	 * To create a good backchain for this CPU in the dump store_status
	 * is passed the address of a function. The address is saved into
	 * the PSW save area of the boot CPU and the function is invoked as
	 * a tail call of store_status. The backchain in the dump will look
	 * like this:
	 *   restart_int_handler ->  __machine_kexec -> __do_machine_kdump
	 * The call to store_status() will not return.
	 */
	store_status(__do_machine_kdump, image);
}
#endif

/*
>>>>>>> v4.9.227
 * Check if kdump checksums are valid: We call purgatory with parameter "0"
 */
static int kdump_csum_valid(struct kimage *image)
{
#ifdef CONFIG_CRASH_DUMP
	int (*start_kdump)(int) = (void *)image->start;
	int rc;

	__arch_local_irq_stnsm(0xfb); /* disable DAT */
	rc = start_kdump(0);
	__arch_local_irq_stosm(0x04); /* enable DAT */
	return rc ? 0 : -EINVAL;
#else
	return -EINVAL;
#endif
}

<<<<<<< HEAD
/*
 * Map or unmap crashkernel memory
 */
static void crash_map_pages(int enable)
{
	unsigned long size = resource_size(&crashk_res);

	BUG_ON(crashk_res.start % KEXEC_CRASH_MEM_ALIGN ||
	       size % KEXEC_CRASH_MEM_ALIGN);
	if (enable)
		vmem_add_mapping(crashk_res.start, size);
	else {
		vmem_remove_mapping(crashk_res.start, size);
		if (size)
			os_info_crashkernel_add(crashk_res.start, size);
		else
			os_info_crashkernel_add(0, 0);
	}
}

/*
 * Map crashkernel memory
 */
void crash_map_reserved_pages(void)
{
	crash_map_pages(1);
}

/*
 * Unmap crashkernel memory
 */
void crash_unmap_reserved_pages(void)
{
	crash_map_pages(0);
}

=======
#ifdef CONFIG_CRASH_DUMP

void crash_free_reserved_phys_range(unsigned long begin, unsigned long end)
{
	unsigned long addr, size;

	for (addr = begin; addr < end; addr += PAGE_SIZE)
		free_reserved_page(pfn_to_page(addr >> PAGE_SHIFT));
	size = begin - crashk_res.start;
	if (size)
		os_info_crashkernel_add(crashk_res.start, size);
	else
		os_info_crashkernel_add(0, 0);
}

static void crash_protect_pages(int protect)
{
	unsigned long size;

	if (!crashk_res.end)
		return;
	size = resource_size(&crashk_res);
	if (protect)
		set_memory_ro(crashk_res.start, size >> PAGE_SHIFT);
	else
		set_memory_rw(crashk_res.start, size >> PAGE_SHIFT);
}

void arch_kexec_protect_crashkres(void)
{
	crash_protect_pages(1);
}

void arch_kexec_unprotect_crashkres(void)
{
	crash_protect_pages(0);
}

#endif

>>>>>>> v4.9.227
/*
 * Give back memory to hypervisor before new kdump is loaded
 */
static int machine_kexec_prepare_kdump(void)
{
#ifdef CONFIG_CRASH_DUMP
	if (MACHINE_IS_VM)
		diag10_range(PFN_DOWN(crashk_res.start),
			     PFN_DOWN(crashk_res.end - crashk_res.start + 1));
	return 0;
#else
	return -EINVAL;
#endif
}

int machine_kexec_prepare(struct kimage *image)
{
	void *reboot_code_buffer;

	/* Can't replace kernel image since it is read-only. */
	if (ipl_flags & IPL_NSS_VALID)
		return -EOPNOTSUPP;

	if (image->type == KEXEC_TYPE_CRASH)
		return machine_kexec_prepare_kdump();

	/* We don't support anything but the default image type for now. */
	if (image->type != KEXEC_TYPE_DEFAULT)
		return -EINVAL;

	/* Get the destination where the assembler code should be copied to.*/
	reboot_code_buffer = (void *) page_to_phys(image->control_code_page);

	/* Then copy it */
	memcpy(reboot_code_buffer, relocate_kernel, relocate_kernel_len);
	return 0;
}

void machine_kexec_cleanup(struct kimage *image)
{
}

void arch_crash_save_vmcoreinfo(void)
{
	VMCOREINFO_SYMBOL(lowcore_ptr);
	VMCOREINFO_SYMBOL(high_memory);
	VMCOREINFO_LENGTH(lowcore_ptr, NR_CPUS);
}

void machine_shutdown(void)
{
}

void machine_crash_shutdown(struct pt_regs *regs)
{
}

/*
 * Do normal kexec
 */
static void __do_machine_kexec(void *data)
{
	relocate_kernel_t data_mover;
	struct kimage *image = data;

<<<<<<< HEAD
=======
	s390_reset_system();
>>>>>>> v4.9.227
	data_mover = (relocate_kernel_t) page_to_phys(image->control_code_page);

	/* Call the moving routine */
	(*data_mover)(&image->head, image->start);
<<<<<<< HEAD
=======

	/* Die if kexec returns */
	disabled_wait((unsigned long) __builtin_return_address(0));
>>>>>>> v4.9.227
}

/*
 * Reset system and call either kdump or normal kexec
 */
static void __machine_kexec(void *data)
{
<<<<<<< HEAD
	struct kimage *image = data;

=======
>>>>>>> v4.9.227
	__arch_local_irq_stosm(0x04); /* enable DAT */
	pfault_fini();
	tracing_off();
	debug_locks_off();
<<<<<<< HEAD
	if (image->type == KEXEC_TYPE_CRASH) {
		lgr_info_log();
		s390_reset_system(__do_machine_kdump, data);
	} else {
		s390_reset_system(__do_machine_kexec, data);
	}
	disabled_wait((unsigned long) __builtin_return_address(0));
=======
#ifdef CONFIG_CRASH_DUMP
	if (((struct kimage *) data)->type == KEXEC_TYPE_CRASH)
		__machine_kdump(data);
#endif
	__do_machine_kexec(data);
>>>>>>> v4.9.227
}

/*
 * Do either kdump or normal kexec. In case of kdump we first ask
 * purgatory, if kdump checksums are valid.
 */
void machine_kexec(struct kimage *image)
{
	if (image->type == KEXEC_TYPE_CRASH && !kdump_csum_valid(image))
		return;
	tracer_disable();
	smp_send_stop();
	smp_call_ipl_cpu(__machine_kexec, image);
}
