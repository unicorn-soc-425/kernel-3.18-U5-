/*
 *  arch/mips/include/asm/prom.h
 *
 *  Copyright (C) 2010 Cisco Systems Inc. <dediao@cisco.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef __ASM_PROM_H
#define __ASM_PROM_H

<<<<<<< HEAD
#ifdef CONFIG_OF
=======
#ifdef CONFIG_USE_OF
>>>>>>> v4.9.227
#include <linux/bug.h>
#include <linux/io.h>
#include <linux/types.h>
#include <asm/bootinfo.h>

extern void device_tree_init(void);

struct boot_param_header;

extern void __dt_setup_arch(void *bph);
<<<<<<< HEAD

#define dt_setup_arch(sym)						\
({									\
	extern char __dtb_##sym##_begin[];				\
									\
	__dt_setup_arch(__dtb_##sym##_begin);				\
})
=======
extern int __dt_register_buses(const char *bus0, const char *bus1);
>>>>>>> v4.9.227

#else /* CONFIG_OF */
static inline void device_tree_init(void) { }
#endif /* CONFIG_OF */

extern char *mips_get_machine_name(void);
extern void mips_set_machine_name(const char *name);

#endif /* __ASM_PROM_H */
