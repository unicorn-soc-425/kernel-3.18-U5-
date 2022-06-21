/*
 * Cache operations for the cache instruction.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * (C) Copyright 1996, 97, 99, 2002, 03 Ralf Baechle
 * (C) Copyright 1999 Silicon Graphics, Inc.
 */
#ifndef __ASM_CACHEOPS_H
#define __ASM_CACHEOPS_H

/*
<<<<<<< HEAD
 * Cache Operations available on all MIPS processors with R4000-style caches
 */
#define Index_Invalidate_I		0x00
#define Index_Writeback_Inv_D		0x01
#define Index_Load_Tag_I		0x04
#define Index_Load_Tag_D		0x05
#define Index_Store_Tag_I		0x08
#define Index_Store_Tag_D		0x09
#define Hit_Invalidate_I		0x10
#define Hit_Invalidate_D		0x11
#define Hit_Writeback_Inv_D		0x15
=======
 * Most cache ops are split into a 2 bit field identifying the cache, and a 3
 * bit field identifying the cache operation.
 */
#define CacheOp_Cache			0x03
#define CacheOp_Op			0x1c

#define Cache_I				0x00
#define Cache_D				0x01
#define Cache_T				0x02
#define Cache_V				0x02 /* Loongson-3 */
#define Cache_S				0x03

#define Index_Writeback_Inv		0x00
#define Index_Load_Tag			0x04
#define Index_Store_Tag			0x08
#define Hit_Invalidate			0x10
#define Hit_Writeback_Inv		0x14	/* not with Cache_I though */
#define Hit_Writeback			0x18

/*
 * Cache Operations available on all MIPS processors with R4000-style caches
 */
#define Index_Invalidate_I		(Cache_I | Index_Writeback_Inv)
#define Index_Writeback_Inv_D		(Cache_D | Index_Writeback_Inv)
#define Index_Load_Tag_I		(Cache_I | Index_Load_Tag)
#define Index_Load_Tag_D		(Cache_D | Index_Load_Tag)
#define Index_Store_Tag_I		(Cache_I | Index_Store_Tag)
#define Index_Store_Tag_D		(Cache_D | Index_Store_Tag)
#define Hit_Invalidate_I		(Cache_I | Hit_Invalidate)
#define Hit_Invalidate_D		(Cache_D | Hit_Invalidate)
#define Hit_Writeback_Inv_D		(Cache_D | Hit_Writeback_Inv)
>>>>>>> v4.9.227

/*
 * R4000-specific cacheops
 */
<<<<<<< HEAD
#define Create_Dirty_Excl_D		0x0d
#define Fill				0x14
#define Hit_Writeback_I			0x18
#define Hit_Writeback_D			0x19
=======
#define Create_Dirty_Excl_D		(Cache_D | 0x0c)
#define Fill				(Cache_I | 0x14)
#define Hit_Writeback_I			(Cache_I | Hit_Writeback)
#define Hit_Writeback_D			(Cache_D | Hit_Writeback)
>>>>>>> v4.9.227

/*
 * R4000SC and R4400SC-specific cacheops
 */
<<<<<<< HEAD
#define Index_Invalidate_SI		0x02
#define Index_Writeback_Inv_SD		0x03
#define Index_Load_Tag_SI		0x06
#define Index_Load_Tag_SD		0x07
#define Index_Store_Tag_SI		0x0A
#define Index_Store_Tag_SD		0x0B
#define Create_Dirty_Excl_SD		0x0f
#define Hit_Invalidate_SI		0x12
#define Hit_Invalidate_SD		0x13
#define Hit_Writeback_Inv_SD		0x17
#define Hit_Writeback_SD		0x1b
#define Hit_Set_Virtual_SI		0x1e
#define Hit_Set_Virtual_SD		0x1f
=======
#define Cache_SI			0x02
#define Cache_SD			0x03

#define Index_Invalidate_SI		(Cache_SI | Index_Writeback_Inv)
#define Index_Writeback_Inv_SD		(Cache_SD | Index_Writeback_Inv)
#define Index_Load_Tag_SI		(Cache_SI | Index_Load_Tag)
#define Index_Load_Tag_SD		(Cache_SD | Index_Load_Tag)
#define Index_Store_Tag_SI		(Cache_SI | Index_Store_Tag)
#define Index_Store_Tag_SD		(Cache_SD | Index_Store_Tag)
#define Create_Dirty_Excl_SD		(Cache_SD | 0x0c)
#define Hit_Invalidate_SI		(Cache_SI | Hit_Invalidate)
#define Hit_Invalidate_SD		(Cache_SD | Hit_Invalidate)
#define Hit_Writeback_Inv_SD		(Cache_SD | Hit_Writeback_Inv)
#define Hit_Writeback_SD		(Cache_SD | Hit_Writeback)
#define Hit_Set_Virtual_SI		(Cache_SI | 0x1c)
#define Hit_Set_Virtual_SD		(Cache_SD | 0x1c)
>>>>>>> v4.9.227

/*
 * R5000-specific cacheops
 */
<<<<<<< HEAD
#define R5K_Page_Invalidate_S		0x17
=======
#define R5K_Page_Invalidate_S		(Cache_S | 0x14)
>>>>>>> v4.9.227

/*
 * RM7000-specific cacheops
 */
<<<<<<< HEAD
#define Page_Invalidate_T		0x16
#define Index_Store_Tag_T		0x0a
#define Index_Load_Tag_T		0x06
=======
#define Page_Invalidate_T		(Cache_T | 0x14)
#define Index_Store_Tag_T		(Cache_T | Index_Store_Tag)
#define Index_Load_Tag_T		(Cache_T | Index_Load_Tag)
>>>>>>> v4.9.227

/*
 * R10000-specific cacheops
 *
 * Cacheops 0x02, 0x06, 0x0a, 0x0c-0x0e, 0x16, 0x1a and 0x1e are unused.
 * Most of the _S cacheops are identical to the R4000SC _SD cacheops.
 */
<<<<<<< HEAD
#define Index_Writeback_Inv_S		0x03
#define Index_Load_Tag_S		0x07
#define Index_Store_Tag_S		0x0B
#define Hit_Invalidate_S		0x13
#define Cache_Barrier			0x14
#define Hit_Writeback_Inv_S		0x17
#define Index_Load_Data_I		0x18
#define Index_Load_Data_D		0x19
#define Index_Load_Data_S		0x1b
#define Index_Store_Data_I		0x1c
#define Index_Store_Data_D		0x1d
#define Index_Store_Data_S		0x1f
=======
#define Index_Writeback_Inv_S		(Cache_S | Index_Writeback_Inv)
#define Index_Load_Tag_S		(Cache_S | Index_Load_Tag)
#define Index_Store_Tag_S		(Cache_S | Index_Store_Tag)
#define Hit_Invalidate_S		(Cache_S | Hit_Invalidate)
#define Cache_Barrier			0x14
#define Hit_Writeback_Inv_S		(Cache_S | Hit_Writeback_Inv)
#define Index_Load_Data_I		(Cache_I | 0x18)
#define Index_Load_Data_D		(Cache_D | 0x18)
#define Index_Load_Data_S		(Cache_S | 0x18)
#define Index_Store_Data_I		(Cache_I | 0x1c)
#define Index_Store_Data_D		(Cache_D | 0x1c)
#define Index_Store_Data_S		(Cache_S | 0x1c)
>>>>>>> v4.9.227

/*
 * Loongson2-specific cacheops
 */
<<<<<<< HEAD
#define Hit_Invalidate_I_Loongson2	0x00
=======
#define Hit_Invalidate_I_Loongson2	(Cache_I | 0x00)

/*
 * Loongson3-specific cacheops
 */
#define Index_Writeback_Inv_V		(Cache_V | Index_Writeback_Inv)
>>>>>>> v4.9.227

#endif	/* __ASM_CACHEOPS_H */
