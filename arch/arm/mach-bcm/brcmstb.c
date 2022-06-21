/*
 * Copyright (C) 2013-2014 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/init.h>
<<<<<<< HEAD
#include <linux/of_platform.h>
=======
#include <linux/irqchip.h>
#include <linux/of_platform.h>
#include <linux/soc/brcmstb/brcmstb.h>
>>>>>>> v4.9.227

#include <asm/mach-types.h>
#include <asm/mach/arch.h>

<<<<<<< HEAD
static const char *brcmstb_match[] __initconst = {
=======
/*
 * Storage for debug-macro.S's state.
 *
 * This must be in .data not .bss so that it gets initialized each time the
 * kernel is loaded. The data is declared here rather than debug-macro.S so
 * that multiple inclusions of debug-macro.S point at the same data.
 */
u32 brcmstb_uart_config[3] = {
	/* Debug UART initialization required */
	1,
	/* Debug UART physical address */
	0,
	/* Debug UART virtual address */
	0,
};

static void __init brcmstb_init_irq(void)
{
	irqchip_init();
	brcmstb_biuctrl_init();
}

static const char *const brcmstb_match[] __initconst = {
>>>>>>> v4.9.227
	"brcm,bcm7445",
	"brcm,brcmstb",
	NULL
};

DT_MACHINE_START(BRCMSTB, "Broadcom STB (Flattened Device Tree)")
	.dt_compat	= brcmstb_match,
<<<<<<< HEAD
=======
	.init_irq	= brcmstb_init_irq,
>>>>>>> v4.9.227
MACHINE_END
