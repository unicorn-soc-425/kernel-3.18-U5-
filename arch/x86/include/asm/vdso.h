#ifndef _ASM_X86_VDSO_H
#define _ASM_X86_VDSO_H

#include <asm/page_types.h>
#include <linux/linkage.h>
#include <linux/init.h>

#ifndef __ASSEMBLER__

#include <linux/mm_types.h>

struct vdso_image {
	void *data;
	unsigned long size;   /* Always a multiple of PAGE_SIZE */

<<<<<<< HEAD
	/* text_mapping.pages is big enough for data/size page pointers */
	struct vm_special_mapping text_mapping;

=======
>>>>>>> v4.9.227
	unsigned long alt, alt_len;

	long sym_vvar_start;  /* Negative offset to the vvar area */

	long sym_vvar_page;
	long sym_hpet_page;
<<<<<<< HEAD
=======
	long sym_pvclock_page;
>>>>>>> v4.9.227
	long sym_VDSO32_NOTE_MASK;
	long sym___kernel_sigreturn;
	long sym___kernel_rt_sigreturn;
	long sym___kernel_vsyscall;
<<<<<<< HEAD
	long sym_VDSO32_SYSENTER_RETURN;
=======
	long sym_int80_landing_pad;
>>>>>>> v4.9.227
};

#ifdef CONFIG_X86_64
extern const struct vdso_image vdso_image_64;
#endif

#ifdef CONFIG_X86_X32
extern const struct vdso_image vdso_image_x32;
#endif

#if defined CONFIG_X86_32 || defined CONFIG_COMPAT
<<<<<<< HEAD
extern const struct vdso_image vdso_image_32_int80;
#ifdef CONFIG_COMPAT
extern const struct vdso_image vdso_image_32_syscall;
#endif
extern const struct vdso_image vdso_image_32_sysenter;

extern const struct vdso_image *selected_vdso32;
=======
extern const struct vdso_image vdso_image_32;
>>>>>>> v4.9.227
#endif

extern void __init init_vdso_image(const struct vdso_image *image);

<<<<<<< HEAD
=======
extern int map_vdso_once(const struct vdso_image *image, unsigned long addr);

>>>>>>> v4.9.227
#endif /* __ASSEMBLER__ */

#endif /* _ASM_X86_VDSO_H */
