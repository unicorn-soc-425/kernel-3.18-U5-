/*
 * Copyright 2012 Pavel Machek <pavel@denx.de>
<<<<<<< HEAD
 * Copyright (C) 2012 Altera Corporation
=======
 * Copyright (C) 2012-2015 Altera Corporation
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __MACH_CORE_H
#define __MACH_CORE_H

#define SOCFPGA_RSTMGR_CTRL	0x04
<<<<<<< HEAD
#define SOCFPGA_RSTMGR_MODPERRST	0x14
#define SOCFPGA_RSTMGR_BRGMODRST	0x1c

=======
#define SOCFPGA_RSTMGR_MODMPURST	0x10
#define SOCFPGA_RSTMGR_MODPERRST	0x14
#define SOCFPGA_RSTMGR_BRGMODRST	0x1c

#define SOCFPGA_A10_RSTMGR_CTRL		0xC
#define SOCFPGA_A10_RSTMGR_MODMPURST	0x20

>>>>>>> v4.9.227
/* System Manager bits */
#define RSTMGR_CTRL_SWCOLDRSTREQ	0x1	/* Cold Reset */
#define RSTMGR_CTRL_SWWARMRSTREQ	0x2	/* Warm Reset */

<<<<<<< HEAD
extern void socfpga_secondary_startup(void);
extern void __iomem *socfpga_scu_base_addr;

extern void socfpga_init_clocks(void);
extern void socfpga_sysmgr_init(void);

extern void __iomem *sys_manager_base_addr;
extern void __iomem *rst_manager_base_addr;

extern struct smp_operations socfpga_smp_ops;
=======
#define RSTMGR_MPUMODRST_CPU1		0x2     /* CPU1 Reset */

extern void socfpga_init_clocks(void);
extern void socfpga_sysmgr_init(void);
void socfpga_init_l2_ecc(void);
void socfpga_init_ocram_ecc(void);
void socfpga_init_arria10_l2_ecc(void);
void socfpga_init_arria10_ocram_ecc(void);

extern void __iomem *sys_manager_base_addr;
extern void __iomem *rst_manager_base_addr;
extern void __iomem *sdr_ctl_base_addr;

u32 socfpga_sdram_self_refresh(u32 sdr_base);
extern unsigned int socfpga_sdram_self_refresh_sz;

>>>>>>> v4.9.227
extern char secondary_trampoline, secondary_trampoline_end;

extern unsigned long socfpga_cpu1start_addr;

<<<<<<< HEAD
#define SOCFPGA_SCU_VIRT_BASE   0xfffec000
=======
#define SOCFPGA_SCU_VIRT_BASE   0xfee00000
>>>>>>> v4.9.227

#endif
