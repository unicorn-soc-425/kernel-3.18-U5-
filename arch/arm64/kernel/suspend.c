#include <linux/ftrace.h>
#include <linux/percpu.h>
#include <linux/slab.h>
<<<<<<< HEAD
#include <asm/cacheflush.h>
#include <asm/cpu_ops.h>
#include <asm/debug-monitors.h>
=======
#include <asm/alternative.h>
#include <asm/cacheflush.h>
#include <asm/cpufeature.h>
#include <asm/debug-monitors.h>
#include <asm/exec.h>
>>>>>>> v4.9.227
#include <asm/pgtable.h>
#include <asm/memory.h>
#include <asm/mmu_context.h>
#include <asm/smp_plat.h>
#include <asm/suspend.h>
#include <asm/tlbflush.h>

<<<<<<< HEAD
extern int __cpu_suspend_enter(unsigned long arg, int (*fn)(unsigned long));
/*
 * This is called by __cpu_suspend_enter() to save the state, and do whatever
 * flushing is required to ensure that when the CPU goes to sleep we have
 * the necessary data available when the caches are not searched.
 *
 * ptr: CPU context virtual address
 * save_ptr: address of the location where the context physical address
 *           must be saved
 */
void notrace __cpu_suspend_save(struct cpu_suspend_ctx *ptr,
				phys_addr_t *save_ptr)
{
	*save_ptr = virt_to_phys(ptr);

	cpu_do_suspend(ptr);
	/*
	 * Only flush the context that must be retrieved with the MMU
	 * off. VA primitives ensure the flush is applied to all
	 * cache levels so context is pushed to DRAM.
	 */
	__flush_dcache_area(ptr, sizeof(*ptr));
	__flush_dcache_area(save_ptr, sizeof(*save_ptr));
}
=======
/*
 * This is allocated by cpu_suspend_init(), and used to store a pointer to
 * the 'struct sleep_stack_data' the contains a particular CPUs state.
 */
unsigned long *sleep_save_stash;
>>>>>>> v4.9.227

/*
 * This hook is provided so that cpu_suspend code can restore HW
 * breakpoints as early as possible in the resume path, before reenabling
 * debug exceptions. Code cannot be run from a CPU PM notifier since by the
 * time the notifier runs debug exceptions might have been enabled already,
 * with HW breakpoints registers content still in an unknown state.
 */
<<<<<<< HEAD
void (*hw_breakpoint_restore)(void *);
void __init cpu_suspend_set_dbg_restorer(void (*hw_bp_restore)(void *))
=======
static int (*hw_breakpoint_restore)(unsigned int);
void __init cpu_suspend_set_dbg_restorer(int (*hw_bp_restore)(unsigned int))
>>>>>>> v4.9.227
{
	/* Prevent multiple restore hook initializations */
	if (WARN_ON(hw_breakpoint_restore))
		return;
	hw_breakpoint_restore = hw_bp_restore;
}

<<<<<<< HEAD
/**
 * cpu_suspend() - function to enter a low-power state
 * @arg: argument to pass to CPU suspend operations
 *
 * Return: 0 on success, -EOPNOTSUPP if CPU suspend hook not initialized, CPU
 * operations back-end error code otherwise.
 */
int cpu_suspend(unsigned long arg)
{
	int cpu = smp_processor_id();

	/*
	 * If cpu_ops have not been registered or suspend
	 * has not been initialized, cpu_suspend call fails early.
	 */
	if (!cpu_ops[cpu] || !cpu_ops[cpu]->cpu_suspend)
		return -EOPNOTSUPP;
	return cpu_ops[cpu]->cpu_suspend(arg);
}

/*
 * __cpu_suspend
=======
void notrace __cpu_suspend_exit(void)
{
	unsigned int cpu = smp_processor_id();

	/*
	 * We are resuming from reset with the idmap active in TTBR0_EL1.
	 * We must uninstall the idmap and restore the expected MMU
	 * state before we can possibly return to userspace.
	 */
	cpu_uninstall_idmap();

	/*
	 * Restore per-cpu offset before any kernel
	 * subsystem relying on it has a chance to run.
	 */
	set_my_cpu_offset(per_cpu_offset(cpu));

	/*
	 * PSTATE was not saved over suspend/resume, re-enable any detected
	 * features that might not have been set correctly.
	 */
	asm(ALTERNATIVE("nop", SET_PSTATE_PAN(1), ARM64_HAS_PAN,
			CONFIG_ARM64_PAN));
	uao_thread_switch(current);

	/*
	 * Restore HW breakpoint registers to sane values
	 * before debug exceptions are possibly reenabled
	 * through local_dbg_restore.
	 */
	if (hw_breakpoint_restore)
		hw_breakpoint_restore(cpu);

	/*
	 * On resume, firmware implementing dynamic mitigation will
	 * have turned the mitigation on. If the user has forcefully
	 * disabled it, make sure their wishes are obeyed.
	 */
	if (arm64_get_ssbd_state() == ARM64_SSBD_FORCE_DISABLE)
		arm64_set_ssbd_mitigation(false);
}

/*
 * cpu_suspend
>>>>>>> v4.9.227
 *
 * arg: argument to pass to the finisher function
 * fn: finisher function pointer
 *
 */
<<<<<<< HEAD
int __cpu_suspend(unsigned long arg, int (*fn)(unsigned long))
{
	struct mm_struct *mm = current->active_mm;
	int ret;
	unsigned long flags;
=======
int cpu_suspend(unsigned long arg, int (*fn)(unsigned long))
{
	int ret = 0;
	unsigned long flags;
	struct sleep_stack_data state;
>>>>>>> v4.9.227

	/*
	 * From this point debug exceptions are disabled to prevent
	 * updates to mdscr register (saved and restored along with
	 * general purpose registers) from kernel debuggers.
	 */
	local_dbg_save(flags);

	/*
	 * Function graph tracer state gets incosistent when the kernel
	 * calls functions that never return (aka suspend finishers) hence
	 * disable graph tracing during their execution.
	 */
	pause_graph_tracing();

<<<<<<< HEAD
	/*
	 * mm context saved on the stack, it will be restored when
	 * the cpu comes out of reset through the identity mapped
	 * page tables, so that the thread address space is properly
	 * set-up on function return.
	 */
	ret = __cpu_suspend_enter(arg, fn);
	if (ret == 0) {
		/*
		 * We are resuming from reset with TTBR0_EL1 set to the
		 * idmap to enable the MMU; restore the active_mm mappings in
		 * TTBR0_EL1 unless the active_mm == &init_mm, in which case
		 * the thread entered __cpu_suspend with TTBR0_EL1 set to
		 * reserved TTBR0 page tables and should be restored as such.
		 */
		if (mm == &init_mm)
			cpu_set_reserved_ttbr0();
		else
			cpu_switch_mm(mm->pgd, mm);

		local_flush_tlb_all();

		/*
		 * Restore per-cpu offset before any kernel
		 * subsystem relying on it has a chance to run.
		 */
		set_my_cpu_offset(per_cpu_offset(smp_processor_id()));

		/*
		 * Restore HW breakpoint registers to sane values
		 * before debug exceptions are possibly reenabled
		 * through local_dbg_restore.
		 */
		if (hw_breakpoint_restore)
			hw_breakpoint_restore(NULL);
=======
	if (__cpu_suspend_enter(&state)) {
		/* Call the suspend finisher */
		ret = fn(arg);

		/*
		 * Never gets here, unless the suspend finisher fails.
		 * Successful cpu_suspend() should return from cpu_resume(),
		 * returning through this code path is considered an error
		 * If the return value is set to 0 force ret = -EOPNOTSUPP
		 * to make sure a proper error condition is propagated
		 */
		if (!ret)
			ret = -EOPNOTSUPP;
	} else {
		__cpu_suspend_exit();
>>>>>>> v4.9.227
	}

	unpause_graph_tracing();

	/*
	 * Restore pstate flags. OS lock and mdscr have been already
	 * restored, so from this point onwards, debugging is fully
	 * renabled if it was enabled when core started shutdown.
	 */
	local_dbg_restore(flags);

	return ret;
}

<<<<<<< HEAD
struct sleep_save_sp sleep_save_sp;
phys_addr_t sleep_idmap_phys;

static int __init cpu_suspend_init(void)
{
	void *ctx_ptr;

	/* ctx_ptr is an array of physical addresses */
	ctx_ptr = kcalloc(mpidr_hash_size(), sizeof(phys_addr_t), GFP_KERNEL);

	if (WARN_ON(!ctx_ptr))
		return -ENOMEM;

	sleep_save_sp.save_ptr_stash = ctx_ptr;
	sleep_save_sp.save_ptr_stash_phys = virt_to_phys(ctx_ptr);
	sleep_idmap_phys = virt_to_phys(idmap_pg_dir);
	__flush_dcache_area(&sleep_save_sp, sizeof(struct sleep_save_sp));
	__flush_dcache_area(&sleep_idmap_phys, sizeof(sleep_idmap_phys));

=======
static int __init cpu_suspend_init(void)
{
	/* ctx_ptr is an array of physical addresses */
	sleep_save_stash = kcalloc(mpidr_hash_size(), sizeof(*sleep_save_stash),
				   GFP_KERNEL);

	if (WARN_ON(!sleep_save_stash))
		return -ENOMEM;

>>>>>>> v4.9.227
	return 0;
}
early_initcall(cpu_suspend_init);
