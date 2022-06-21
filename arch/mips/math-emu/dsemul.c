<<<<<<< HEAD
=======
#include <linux/err.h>
#include <linux/slab.h>

>>>>>>> v4.9.227
#include <asm/branch.h>
#include <asm/cacheflush.h>
#include <asm/fpu_emulator.h>
#include <asm/inst.h>
#include <asm/mipsregs.h>
#include <asm/uaccess.h>

<<<<<<< HEAD
#include "ieee754.h"

/*
 * Emulate the arbritrary instruction ir at xcp->cp0_epc.  Required when
 * we have to emulate the instruction in a COP1 branch delay slot.  Do
 * not change cp0_epc due to the instruction
 *
 * According to the spec:
 * 1) it shouldn't be a branch :-)
 * 2) it can be a COP instruction :-(
 * 3) if we are tring to run a protected memory space we must take
 *    special care on memory access instructions :-(
 */

/*
 * "Trampoline" return routine to catch exception following
 *  execution of delay-slot instruction execution.
 */

struct emuframe {
	mips_instruction	emul;
	mips_instruction	badinst;
	mips_instruction	cookie;
	unsigned long		epc;
};

int mips_dsemul(struct pt_regs *regs, mips_instruction ir, unsigned long cpc)
{
	extern asmlinkage void handle_dsemulret(void);
	struct emuframe __user *fr;
	int err;

	if ((get_isa16_mode(regs->cp0_epc) && ((ir >> 16) == MM_NOP16)) ||
		(ir == 0)) {
		/* NOP is easy */
		regs->cp0_epc = cpc;
		clear_delay_slot(regs);
		return 0;
	}

	pr_debug("dsemul %lx %lx\n", regs->cp0_epc, cpc);

	/*
	 * The strategy is to push the instruction onto the user stack
	 * and put a trap after it which we can catch and jump to
	 * the required address any alternative apart from full
	 * instruction emulation!!.
	 *
	 * Algorithmics used a system call instruction, and
	 * borrowed that vector.  MIPS/Linux version is a bit
	 * more heavyweight in the interests of portability and
	 * multiprocessor support.  For Linux we generate a
	 * an unaligned access and force an address error exception.
	 *
	 * For embedded systems (stand-alone) we prefer to use a
	 * non-existing CP1 instruction. This prevents us from emulating
	 * branches, but gives us a cleaner interface to the exception
	 * handler (single entry point).
	 */

	/* Ensure that the two instructions are in the same cache line */
	fr = (struct emuframe __user *)
		((regs->regs[29] - sizeof(struct emuframe)) & ~0x7);

	/* Verify that the stack pointer is not competely insane */
	if (unlikely(!access_ok(VERIFY_WRITE, fr, sizeof(struct emuframe))))
		return SIGBUS;

	if (get_isa16_mode(regs->cp0_epc)) {
		err = __put_user(ir >> 16, (u16 __user *)(&fr->emul));
		err |= __put_user(ir & 0xffff, (u16 __user *)((long)(&fr->emul) + 2));
		err |= __put_user(BREAK_MATH >> 16, (u16 __user *)(&fr->badinst));
		err |= __put_user(BREAK_MATH & 0xffff, (u16 __user *)((long)(&fr->badinst) + 2));
	} else {
		err = __put_user(ir, &fr->emul);
		err |= __put_user((mips_instruction)BREAK_MATH, &fr->badinst);
	}

	err |= __put_user((mips_instruction)BD_COOKIE, &fr->cookie);
	err |= __put_user(cpc, &fr->epc);

	if (unlikely(err)) {
		MIPS_FPU_EMU_INC_STATS(errors);
		return SIGBUS;
	}

	regs->cp0_epc = ((unsigned long) &fr->emul) |
		get_isa16_mode(regs->cp0_epc);

	flush_cache_sigtramp((unsigned long)&fr->badinst);

	return SIGILL;		/* force out of emulation loop */
}

int do_dsemulret(struct pt_regs *xcp)
{
	struct emuframe __user *fr;
	unsigned long epc;
	u32 insn, cookie;
	int err = 0;
	u16 instr[2];

	fr = (struct emuframe __user *)
		(msk_isa16_mode(xcp->cp0_epc) - sizeof(mips_instruction));

	/*
	 * If we can't even access the area, something is very wrong, but we'll
	 * leave that to the default handling
	 */
	if (!access_ok(VERIFY_READ, fr, sizeof(struct emuframe)))
		return 0;

	/*
	 * Do some sanity checking on the stackframe:
	 *
	 *  - Is the instruction pointed to by the EPC an BREAK_MATH?
	 *  - Is the following memory word the BD_COOKIE?
	 */
	if (get_isa16_mode(xcp->cp0_epc)) {
		err = __get_user(instr[0], (u16 __user *)(&fr->badinst));
		err |= __get_user(instr[1], (u16 __user *)((long)(&fr->badinst) + 2));
		insn = (instr[0] << 16) | instr[1];
	} else {
		err = __get_user(insn, &fr->badinst);
	}
	err |= __get_user(cookie, &fr->cookie);

	if (unlikely(err || (insn != BREAK_MATH) || (cookie != BD_COOKIE))) {
		MIPS_FPU_EMU_INC_STATS(errors);
		return 0;
	}

	/*
	 * At this point, we are satisfied that it's a BD emulation trap.  Yes,
	 * a user might have deliberately put two malformed and useless
	 * instructions in a row in his program, in which case he's in for a
	 * nasty surprise - the next instruction will be treated as a
	 * continuation address!  Alas, this seems to be the only way that we
	 * can handle signals, recursion, and longjmps() in the context of
	 * emulating the branch delay instruction.
	 */

	pr_debug("dsemulret\n");

	if (__get_user(epc, &fr->epc)) {		/* Saved EPC */
		/* This is not a good situation to be in */
		force_sig(SIGBUS, current);

		return 0;
	}

	/* Set EPC to return to post-branch instruction */
	xcp->cp0_epc = epc;

	return 1;
=======
/**
 * struct emuframe - The 'emulation' frame structure
 * @emul:	The instruction to 'emulate'.
 * @badinst:	A break instruction to cause a return to the kernel.
 *
 * This structure defines the frames placed within the delay slot emulation
 * page in response to a call to mips_dsemul(). Each thread may be allocated
 * only one frame at any given time. The kernel stores within it the
 * instruction to be 'emulated' followed by a break instruction, then
 * executes the frame in user mode. The break causes a trap to the kernel
 * which leads to do_dsemulret() being called unless the instruction in
 * @emul causes a trap itself, is a branch, or a signal is delivered to
 * the thread. In these cases the allocated frame will either be reused by
 * a subsequent delay slot 'emulation', or be freed during signal delivery or
 * upon thread exit.
 *
 * This approach is used because:
 *
 * - Actually emulating all instructions isn't feasible. We would need to
 *   be able to handle instructions from all revisions of the MIPS ISA,
 *   all ASEs & all vendor instruction set extensions. This would be a
 *   whole lot of work & continual maintenance burden as new instructions
 *   are introduced, and in the case of some vendor extensions may not
 *   even be possible. Thus we need to take the approach of actually
 *   executing the instruction.
 *
 * - We must execute the instruction within user context. If we were to
 *   execute the instruction in kernel mode then it would have access to
 *   kernel resources without very careful checks, leaving us with a
 *   high potential for security or stability issues to arise.
 *
 * - We used to place the frame on the users stack, but this requires
 *   that the stack be executable. This is bad for security so the
 *   per-process page is now used instead.
 *
 * - The instruction in @emul may be something entirely invalid for a
 *   delay slot. The user may (intentionally or otherwise) place a branch
 *   in a delay slot, or a kernel mode instruction, or something else
 *   which generates an exception. Thus we can't rely upon the break in
 *   @badinst always being hit. For this reason we track the index of the
 *   frame allocated to each thread, allowing us to clean it up at later
 *   points such as signal delivery or thread exit.
 *
 * - The user may generate a fake struct emuframe if they wish, invoking
 *   the BRK_MEMU break instruction themselves. We must therefore not
 *   trust that BRK_MEMU means there's actually a valid frame allocated
 *   to the thread, and must not allow the user to do anything they
 *   couldn't already.
 */
struct emuframe {
	mips_instruction	emul;
	mips_instruction	badinst;
};

static const int emupage_frame_count = PAGE_SIZE / sizeof(struct emuframe);

static inline __user struct emuframe *dsemul_page(void)
{
	return (__user struct emuframe *)STACK_TOP;
}

static int alloc_emuframe(void)
{
	mm_context_t *mm_ctx = &current->mm->context;
	int idx;

retry:
	spin_lock(&mm_ctx->bd_emupage_lock);

	/* Ensure we have an allocation bitmap */
	if (!mm_ctx->bd_emupage_allocmap) {
		mm_ctx->bd_emupage_allocmap =
			kcalloc(BITS_TO_LONGS(emupage_frame_count),
					      sizeof(unsigned long),
				GFP_ATOMIC);

		if (!mm_ctx->bd_emupage_allocmap) {
			idx = BD_EMUFRAME_NONE;
			goto out_unlock;
		}
	}

	/* Attempt to allocate a single bit/frame */
	idx = bitmap_find_free_region(mm_ctx->bd_emupage_allocmap,
				      emupage_frame_count, 0);
	if (idx < 0) {
		/*
		 * Failed to allocate a frame. We'll wait until one becomes
		 * available. We unlock the page so that other threads actually
		 * get the opportunity to free their frames, which means
		 * technically the result of bitmap_full may be incorrect.
		 * However the worst case is that we repeat all this and end up
		 * back here again.
		 */
		spin_unlock(&mm_ctx->bd_emupage_lock);
		if (!wait_event_killable(mm_ctx->bd_emupage_queue,
			!bitmap_full(mm_ctx->bd_emupage_allocmap,
				     emupage_frame_count)))
			goto retry;

		/* Received a fatal signal - just give in */
		return BD_EMUFRAME_NONE;
	}

	/* Success! */
	pr_debug("allocate emuframe %d to %d\n", idx, current->pid);
out_unlock:
	spin_unlock(&mm_ctx->bd_emupage_lock);
	return idx;
}

static void free_emuframe(int idx, struct mm_struct *mm)
{
	mm_context_t *mm_ctx = &mm->context;

	spin_lock(&mm_ctx->bd_emupage_lock);

	pr_debug("free emuframe %d from %d\n", idx, current->pid);
	bitmap_clear(mm_ctx->bd_emupage_allocmap, idx, 1);

	/* If some thread is waiting for a frame, now's its chance */
	wake_up(&mm_ctx->bd_emupage_queue);

	spin_unlock(&mm_ctx->bd_emupage_lock);
}

static bool within_emuframe(struct pt_regs *regs)
{
	unsigned long base = (unsigned long)dsemul_page();

	if (regs->cp0_epc < base)
		return false;
	if (regs->cp0_epc >= (base + PAGE_SIZE))
		return false;

	return true;
}

bool dsemul_thread_cleanup(struct task_struct *tsk)
{
	int fr_idx;

	/* Clear any allocated frame, retrieving its index */
	fr_idx = atomic_xchg(&tsk->thread.bd_emu_frame, BD_EMUFRAME_NONE);

	/* If no frame was allocated, we're done */
	if (fr_idx == BD_EMUFRAME_NONE)
		return false;

	task_lock(tsk);

	/* Free the frame that this thread had allocated */
	if (tsk->mm)
		free_emuframe(fr_idx, tsk->mm);

	task_unlock(tsk);
	return true;
}

bool dsemul_thread_rollback(struct pt_regs *regs)
{
	struct emuframe __user *fr;
	int fr_idx;

	/* Do nothing if we're not executing from a frame */
	if (!within_emuframe(regs))
		return false;

	/* Find the frame being executed */
	fr_idx = atomic_read(&current->thread.bd_emu_frame);
	if (fr_idx == BD_EMUFRAME_NONE)
		return false;
	fr = &dsemul_page()[fr_idx];

	/*
	 * If the PC is at the emul instruction, roll back to the branch. If
	 * PC is at the badinst (break) instruction, we've already emulated the
	 * instruction so progress to the continue PC. If it's anything else
	 * then something is amiss & the user has branched into some other area
	 * of the emupage - we'll free the allocated frame anyway.
	 */
	if (msk_isa16_mode(regs->cp0_epc) == (unsigned long)&fr->emul)
		regs->cp0_epc = current->thread.bd_emu_branch_pc;
	else if (msk_isa16_mode(regs->cp0_epc) == (unsigned long)&fr->badinst)
		regs->cp0_epc = current->thread.bd_emu_cont_pc;

	atomic_set(&current->thread.bd_emu_frame, BD_EMUFRAME_NONE);
	free_emuframe(fr_idx, current->mm);
	return true;
}

void dsemul_mm_cleanup(struct mm_struct *mm)
{
	mm_context_t *mm_ctx = &mm->context;

	kfree(mm_ctx->bd_emupage_allocmap);
}

int mips_dsemul(struct pt_regs *regs, mips_instruction ir,
		unsigned long branch_pc, unsigned long cont_pc)
{
	int isa16 = get_isa16_mode(regs->cp0_epc);
	mips_instruction break_math;
	unsigned long fr_uaddr;
	struct emuframe fr;
	int fr_idx, ret;

	/* NOP is easy */
	if (ir == 0)
		return -1;

	/* microMIPS instructions */
	if (isa16) {
		union mips_instruction insn = { .word = ir };

		/* NOP16 aka MOVE16 $0, $0 */
		if ((ir >> 16) == MM_NOP16)
			return -1;

		/* ADDIUPC */
		if (insn.mm_a_format.opcode == mm_addiupc_op) {
			unsigned int rs;
			s32 v;

			rs = (((insn.mm_a_format.rs + 0xe) & 0xf) + 2);
			v = regs->cp0_epc & ~3;
			v += insn.mm_a_format.simmediate << 2;
			regs->regs[rs] = (long)v;
			return -1;
		}
	}

	pr_debug("dsemul 0x%08lx cont at 0x%08lx\n", regs->cp0_epc, cont_pc);

	/* Allocate a frame if we don't already have one */
	fr_idx = atomic_read(&current->thread.bd_emu_frame);
	if (fr_idx == BD_EMUFRAME_NONE)
		fr_idx = alloc_emuframe();
	if (fr_idx == BD_EMUFRAME_NONE)
		return SIGBUS;

	/* Retrieve the appropriately encoded break instruction */
	break_math = BREAK_MATH(isa16);

	/* Write the instructions to the frame */
	if (isa16) {
		union mips_instruction _emul = {
			.halfword = { ir >> 16, ir }
		};
		union mips_instruction _badinst = {
			.halfword = { break_math >> 16, break_math }
		};

		fr.emul = _emul.word;
		fr.badinst = _badinst.word;
	} else {
		fr.emul = ir;
		fr.badinst = break_math;
	}

	/* Write the frame to user memory */
	fr_uaddr = (unsigned long)&dsemul_page()[fr_idx];
	ret = access_process_vm(current, fr_uaddr, &fr, sizeof(fr),
				FOLL_FORCE | FOLL_WRITE);
	if (unlikely(ret != sizeof(fr))) {
		MIPS_FPU_EMU_INC_STATS(errors);
		free_emuframe(fr_idx, current->mm);
		return SIGBUS;
	}

	/* Record the PC of the branch, PC to continue from & frame index */
	current->thread.bd_emu_branch_pc = branch_pc;
	current->thread.bd_emu_cont_pc = cont_pc;
	atomic_set(&current->thread.bd_emu_frame, fr_idx);

	/* Change user register context to execute the frame */
	regs->cp0_epc = fr_uaddr | isa16;

	return 0;
}

bool do_dsemulret(struct pt_regs *xcp)
{
	/* Cleanup the allocated frame, returning if there wasn't one */
	if (!dsemul_thread_cleanup(current)) {
		MIPS_FPU_EMU_INC_STATS(errors);
		return false;
	}

	/* Set EPC to return to post-branch instruction */
	xcp->cp0_epc = current->thread.bd_emu_cont_pc;
	pr_debug("dsemulret to 0x%08lx\n", xcp->cp0_epc);
	MIPS_FPU_EMU_INC_STATS(ds_emul);
	return true;
>>>>>>> v4.9.227
}
