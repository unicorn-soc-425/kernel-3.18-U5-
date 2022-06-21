#ifndef _ASM_X86_DMI_H
#define _ASM_X86_DMI_H

#include <linux/compiler.h>
#include <linux/init.h>
<<<<<<< HEAD

#include <asm/io.h>
=======
#include <linux/io.h>

>>>>>>> v4.9.227
#include <asm/setup.h>

static __always_inline __init void *dmi_alloc(unsigned len)
{
	return extend_brk(len, sizeof(int));
}

/* Use early IO mappings for DMI because it's initialized early */
#define dmi_early_remap		early_ioremap
#define dmi_early_unmap		early_iounmap
<<<<<<< HEAD
#define dmi_remap		ioremap
=======
#define dmi_remap		ioremap_cache
>>>>>>> v4.9.227
#define dmi_unmap		iounmap

#endif /* _ASM_X86_DMI_H */
