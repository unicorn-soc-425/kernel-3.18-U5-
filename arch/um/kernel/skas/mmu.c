/*
<<<<<<< HEAD
=======
 * Copyright (C) 2015 Thomas Meyer (thomas@m3y3r.de)
>>>>>>> v4.9.227
 * Copyright (C) 2002 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 * Licensed under the GPL
 */

#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>
<<<<<<< HEAD
=======
#include <asm/sections.h>
>>>>>>> v4.9.227
#include <as-layout.h>
#include <os.h>
#include <skas.h>

<<<<<<< HEAD
extern int __syscall_stub_start;

=======
>>>>>>> v4.9.227
static int init_stub_pte(struct mm_struct *mm, unsigned long proc,
			 unsigned long kernel)
{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	pgd = pgd_offset(mm, proc);
	pud = pud_alloc(mm, pgd, proc);
	if (!pud)
		goto out;

	pmd = pmd_alloc(mm, pud, proc);
	if (!pmd)
		goto out_pmd;

<<<<<<< HEAD
	pte = pte_alloc_map(mm, NULL, pmd, proc);
=======
	pte = pte_alloc_map(mm, pmd, proc);
>>>>>>> v4.9.227
	if (!pte)
		goto out_pte;

	*pte = mk_pte(virt_to_page(kernel), __pgprot(_PAGE_PRESENT));
	*pte = pte_mkread(*pte);
	return 0;

 out_pte:
	pmd_free(mm, pmd);
 out_pmd:
	pud_free(mm, pud);
 out:
	return -ENOMEM;
}

int init_new_context(struct task_struct *task, struct mm_struct *mm)
{
 	struct mm_context *from_mm = NULL;
	struct mm_context *to_mm = &mm->context;
	unsigned long stack = 0;
	int ret = -ENOMEM;

<<<<<<< HEAD
	if (skas_needs_stub) {
		stack = get_zeroed_page(GFP_KERNEL);
		if (stack == 0)
			goto out;
	}
=======
	stack = get_zeroed_page(GFP_KERNEL);
	if (stack == 0)
		goto out;
>>>>>>> v4.9.227

	to_mm->id.stack = stack;
	if (current->mm != NULL && current->mm != &init_mm)
		from_mm = &current->mm->context;

<<<<<<< HEAD
	if (proc_mm) {
		ret = new_mm(stack);
		if (ret < 0) {
			printk(KERN_ERR "init_new_context_skas - "
			       "new_mm failed, errno = %d\n", ret);
			goto out_free;
		}
		to_mm->id.u.mm_fd = ret;
	}
	else {
		if (from_mm)
			to_mm->id.u.pid = copy_context_skas0(stack,
							     from_mm->id.u.pid);
		else to_mm->id.u.pid = start_userspace(stack);

		if (to_mm->id.u.pid < 0) {
			ret = to_mm->id.u.pid;
			goto out_free;
		}
=======
	block_signals();
	if (from_mm)
		to_mm->id.u.pid = copy_context_skas0(stack,
						     from_mm->id.u.pid);
	else to_mm->id.u.pid = start_userspace(stack);
	unblock_signals();

	if (to_mm->id.u.pid < 0) {
		ret = to_mm->id.u.pid;
		goto out_free;
>>>>>>> v4.9.227
	}

	ret = init_new_ldt(to_mm, from_mm);
	if (ret < 0) {
		printk(KERN_ERR "init_new_context_skas - init_ldt"
		       " failed, errno = %d\n", ret);
		goto out_free;
	}

	return 0;

 out_free:
	if (to_mm->id.stack != 0)
		free_page(to_mm->id.stack);
 out:
	return ret;
}

void uml_setup_stubs(struct mm_struct *mm)
{
	int err, ret;

<<<<<<< HEAD
	if (!skas_needs_stub)
		return;

	ret = init_stub_pte(mm, STUB_CODE,
			    (unsigned long) &__syscall_stub_start);
=======
	ret = init_stub_pte(mm, STUB_CODE,
			    (unsigned long) __syscall_stub_start);
>>>>>>> v4.9.227
	if (ret)
		goto out;

	ret = init_stub_pte(mm, STUB_DATA, mm->context.id.stack);
	if (ret)
		goto out;

<<<<<<< HEAD
	mm->context.stub_pages[0] = virt_to_page(&__syscall_stub_start);
=======
	mm->context.stub_pages[0] = virt_to_page(__syscall_stub_start);
>>>>>>> v4.9.227
	mm->context.stub_pages[1] = virt_to_page(mm->context.id.stack);

	/* dup_mmap already holds mmap_sem */
	err = install_special_mapping(mm, STUB_START, STUB_END - STUB_START,
				      VM_READ | VM_MAYREAD | VM_EXEC |
				      VM_MAYEXEC | VM_DONTCOPY | VM_PFNMAP,
				      mm->context.stub_pages);
	if (err) {
		printk(KERN_ERR "install_special_mapping returned %d\n", err);
		goto out;
	}
	return;

out:
	force_sigsegv(SIGSEGV, current);
}

void arch_exit_mmap(struct mm_struct *mm)
{
	pte_t *pte;

	pte = virt_to_pte(mm, STUB_CODE);
	if (pte != NULL)
		pte_clear(mm, STUB_CODE, pte);

	pte = virt_to_pte(mm, STUB_DATA);
	if (pte == NULL)
		return;

	pte_clear(mm, STUB_DATA, pte);
}

void destroy_context(struct mm_struct *mm)
{
	struct mm_context *mmu = &mm->context;

<<<<<<< HEAD
	if (proc_mm)
		os_close_file(mmu->id.u.mm_fd);
	else {
		/*
		 * If init_new_context wasn't called, this will be
		 * zero, resulting in a kill(0), which will result in the
		 * whole UML suddenly dying.  Also, cover negative and
		 * 1 cases, since they shouldn't happen either.
		 */
		if (mmu->id.u.pid < 2) {
			printk(KERN_ERR "corrupt mm_context - pid = %d\n",
			       mmu->id.u.pid);
			return;
		}
		os_kill_ptraced_process(mmu->id.u.pid, 1);
	}

	if (skas_needs_stub)
		free_page(mmu->id.stack);

=======
	/*
	 * If init_new_context wasn't called, this will be
	 * zero, resulting in a kill(0), which will result in the
	 * whole UML suddenly dying.  Also, cover negative and
	 * 1 cases, since they shouldn't happen either.
	 */
	if (mmu->id.u.pid < 2) {
		printk(KERN_ERR "corrupt mm_context - pid = %d\n",
		       mmu->id.u.pid);
		return;
	}
	os_kill_ptraced_process(mmu->id.u.pid, 1);

	free_page(mmu->id.stack);
>>>>>>> v4.9.227
	free_ldt(mmu);
}
