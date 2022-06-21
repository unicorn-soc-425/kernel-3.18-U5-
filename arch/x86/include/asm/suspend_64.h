/*
 * Copyright 2001-2003 Pavel Machek <pavel@suse.cz>
 * Based on code
 * Copyright 2001 Patrick Mochel <mochel@osdl.org>
 */
#ifndef _ASM_X86_SUSPEND_64_H
#define _ASM_X86_SUSPEND_64_H

#include <asm/desc.h>
<<<<<<< HEAD
#include <asm/i387.h>
=======
#include <asm/fpu/api.h>
>>>>>>> v4.9.227

/*
 * Image of the saved processor state, used by the low level ACPI suspend to
 * RAM code and by the low level hibernation code.
 *
 * If you modify it, fix arch/x86/kernel/acpi/wakeup_64.S and make sure that
 * __save/__restore_processor_state(), defined in arch/x86/kernel/suspend_64.c,
 * still work as required.
 */
struct saved_context {
	struct pt_regs regs;
<<<<<<< HEAD
	u16 ds, es, fs, gs, ss;
	unsigned long gs_base, gs_kernel_base, fs_base;
	unsigned long cr0, cr2, cr3, cr4, cr8;
	u64 misc_enable;
	bool misc_enable_saved;
=======

	/*
	 * User CS and SS are saved in current_pt_regs().  The rest of the
	 * segment selectors need to be saved and restored here.
	 */
	u16 ds, es, fs, gs;

	/*
	 * Usermode FSBASE and GSBASE may not match the fs and gs selectors,
	 * so we save them separately.  We save the kernelmode GSBASE to
	 * restore percpu access after resume.
	 */
	unsigned long kernelmode_gs_base, usermode_gs_base, fs_base;

	unsigned long cr0, cr2, cr3, cr4, cr8;
	u64 misc_enable;
	bool misc_enable_saved;
	struct saved_msrs saved_msrs;
>>>>>>> v4.9.227
	unsigned long efer;
	u16 gdt_pad; /* Unused */
	struct desc_ptr gdt_desc;
	u16 idt_pad;
<<<<<<< HEAD
	u16 idt_limit;
	unsigned long idt_base;
=======
	struct desc_ptr idt;
>>>>>>> v4.9.227
	u16 ldt;
	u16 tss;
	unsigned long tr;
	unsigned long safety;
	unsigned long return_address;
} __attribute__((packed));

#define loaddebug(thread,register) \
	set_debugreg((thread)->debugreg##register, register)

/* routines for saving/restoring kernel state */
<<<<<<< HEAD
extern int acpi_save_state_mem(void);
extern char core_restore_code;
extern char restore_registers;
=======
extern char core_restore_code[];
extern char restore_registers[];
>>>>>>> v4.9.227

#endif /* _ASM_X86_SUSPEND_64_H */
