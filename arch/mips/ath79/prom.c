/*
 *  Atheros AR71XX/AR724X/AR913X specific prom routines
 *
<<<<<<< HEAD
=======
 *  Copyright (C) 2015 Laurent Fasnacht <l@libres.ch>
>>>>>>> v4.9.227
 *  Copyright (C) 2008-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/string.h>
<<<<<<< HEAD

#include <asm/bootinfo.h>
#include <asm/addrspace.h>

#include "common.h"

static inline int is_valid_ram_addr(void *addr)
{
	if (((u32) addr > KSEG0) &&
	    ((u32) addr < (KSEG0 + ATH79_MEM_SIZE_MAX)))
		return 1;

	if (((u32) addr > KSEG1) &&
	    ((u32) addr < (KSEG1 + ATH79_MEM_SIZE_MAX)))
		return 1;

	return 0;
}

static __init void ath79_prom_init_cmdline(int argc, char **argv)
{
	int i;

	if (!is_valid_ram_addr(argv))
		return;

	for (i = 0; i < argc; i++)
		if (is_valid_ram_addr(argv[i])) {
			strlcat(arcs_cmdline, " ", sizeof(arcs_cmdline));
			strlcat(arcs_cmdline, argv[i], sizeof(arcs_cmdline));
		}
}

void __init prom_init(void)
{
	ath79_prom_init_cmdline(fw_arg0, (char **)fw_arg1);
=======
#include <linux/initrd.h>

#include <asm/bootinfo.h>
#include <asm/addrspace.h>
#include <asm/fw/fw.h>

#include "common.h"

void __init prom_init(void)
{
	fw_init_cmdline();

#ifdef CONFIG_BLK_DEV_INITRD
	/* Read the initrd address from the firmware environment */
	initrd_start = fw_getenvl("initrd_start");
	if (initrd_start) {
		initrd_start = KSEG0ADDR(initrd_start);
		initrd_end = initrd_start + fw_getenvl("initrd_size");
	}
#endif
>>>>>>> v4.9.227
}

void __init prom_free_prom_memory(void)
{
	/* We do not have to prom memory to free */
}
