<<<<<<< HEAD
/* plx9080.h
 *
 * Copyright (C) 2002,2003 Frank Mori Hess <fmhess@users.sourceforge.net>
 *
 * I modified this file from the plx9060.h header for the
 * wanXL device driver in the linux kernel,
 * for the register offsets and bit definitions.  Made minor modifications,
 * added plx9080 registers and
 * stripped out stuff that was specifically for the wanXL driver.
 * Note: I've only made sure the definitions are correct as far
 * as I make use of them.  There are still various plx9060-isms
 * left in this header file.
 *
=======
/*
 * plx9080.h
 *
 * Copyright (C) 2002,2003 Frank Mori Hess <fmhess@users.sourceforge.net>
 *
>>>>>>> v4.9.227
 ********************************************************************
 *
 * Copyright (C) 1999 RG Studio s.c.
 * Written by Krzysztof Halasa <khc@rgstudio.com.pl>
 *
 * Portions (C) SBE Inc., used by permission.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef __COMEDI_PLX9080_H
#define __COMEDI_PLX9080_H

<<<<<<< HEAD
/*  descriptor block used for chained dma transfers */
struct plx_dma_desc {
	__le32 pci_start_addr;
	__le32 local_start_addr;
	/* transfer_size is in bytes, only first 23 bits of register are used */
	__le32 transfer_size;
	/* address of next descriptor (quad word aligned), plus some
	 * additional bits (see PLX_DMA0_DESCRIPTOR_REG) */
	__le32 next;
};

/**********************************************************************
**            Register Offsets and Bit Definitions
**
** Note: All offsets zero relative.  IE. Some standard base address
** must be added to the Register Number to properly access the register.
**
**********************************************************************/

#define PLX_LAS0RNG_REG         0x0000	/* L, Local Addr Space 0 Range Register */
#define PLX_LAS1RNG_REG         0x00f0	/* L, Local Addr Space 1 Range Register */
#define  LRNG_IO           0x00000001	/* Map to: 1=I/O, 0=Mem */
#define  LRNG_ANY32        0x00000000	/* Locate anywhere in 32 bit */
#define  LRNG_LT1MB        0x00000002	/* Locate in 1st meg */
#define  LRNG_ANY64        0x00000004	/* Locate anywhere in 64 bit */
#define  LRNG_MEM_MASK     0xfffffff0	/*  bits that specify range for memory io */
#define  LRNG_IO_MASK     0xfffffffa	/*  bits that specify range for normal io */

#define PLX_LAS0MAP_REG         0x0004	/* L, Local Addr Space 0 Remap Register */
#define PLX_LAS1MAP_REG         0x00f4	/* L, Local Addr Space 1 Remap Register */
#define  LMAP_EN           0x00000001	/* Enable slave decode */
#define  LMAP_MEM_MASK     0xfffffff0	/*  bits that specify decode for memory io */
#define  LMAP_IO_MASK     0xfffffffa	/*  bits that specify decode bits for normal io */

/* Mode/Arbitration Register.
*/
#define PLX_MARB_REG         0x8	/* L, Local Arbitration Register */
#define PLX_DMAARB_REG      0xac
enum marb_bits {
	MARB_LLT_MASK = 0x000000ff,	/* Local Bus Latency Timer */
	MARB_LPT_MASK = 0x0000ff00,	/* Local Bus Pause Timer */
	MARB_LTEN = 0x00010000,	/* Latency Timer Enable */
	MARB_LPEN = 0x00020000,	/* Pause Timer Enable */
	MARB_BREQ = 0x00040000,	/* Local Bus BREQ Enable */
	MARB_DMA_PRIORITY_MASK = 0x00180000,
	MARB_LBDS_GIVE_UP_BUS_MODE = 0x00200000,	/* local bus direct slave give up bus mode */
	MARB_DS_LLOCK_ENABLE = 0x00400000,	/* direct slave LLOCKo# enable */
	MARB_PCI_REQUEST_MODE = 0x00800000,
	MARB_PCIv21_MODE = 0x01000000,	/* pci specification v2.1 mode */
	MARB_PCI_READ_NO_WRITE_MODE = 0x02000000,
	MARB_PCI_READ_WITH_WRITE_FLUSH_MODE = 0x04000000,
	MARB_GATE_TIMER_WITH_BREQ = 0x08000000,	/* gate local bus latency timer with BREQ */
	MARB_PCI_READ_NO_FLUSH_MODE = 0x10000000,
	MARB_USE_SUBSYSTEM_IDS = 0x20000000,
};

#define PLX_BIGEND_REG 0xc
enum bigend_bits {
	BIGEND_CONFIG = 0x1,	/* use big endian ordering for configuration register accesses */
	BIGEND_DIRECT_MASTER = 0x2,
	BIGEND_DIRECT_SLAVE_LOCAL0 = 0x4,
	BIGEND_ROM = 0x8,
	BIGEND_BYTE_LANE = 0x10,	/* use byte lane consisting of most significant bits instead of least significant */
	BIGEND_DIRECT_SLAVE_LOCAL1 = 0x20,
	BIGEND_DMA1 = 0x40,
	BIGEND_DMA0 = 0x80,
};

/* Note: The Expansion ROM  stuff is only relevant to the PC environment.
**       This expansion ROM code is executed by the host CPU at boot time.
**       For this reason no bit definitions are provided here.
*/
#define PLX_ROMRNG_REG         0x0010	/* L, Expn ROM Space Range Register */
#define PLX_ROMMAP_REG         0x0014	/* L, Local Addr Space Range Register */

#define PLX_REGION0_REG         0x0018	/* L, Local Bus Region 0 Descriptor */
#define  RGN_WIDTH         0x00000002	/* Local bus width bits */
#define  RGN_8BITS         0x00000000	/* 08 bit Local Bus */
#define  RGN_16BITS        0x00000001	/* 16 bit Local Bus */
#define  RGN_32BITS        0x00000002	/* 32 bit Local Bus */
#define  RGN_MWS           0x0000003C	/* Memory Access Wait States */
#define  RGN_0MWS          0x00000000
#define  RGN_1MWS          0x00000004
#define  RGN_2MWS          0x00000008
#define  RGN_3MWS          0x0000000C
#define  RGN_4MWS          0x00000010
#define  RGN_6MWS          0x00000018
#define  RGN_8MWS          0x00000020
#define  RGN_MRE           0x00000040	/* Memory Space Ready Input Enable */
#define  RGN_MBE           0x00000080	/* Memory Space Bterm Input Enable */
#define  RGN_READ_PREFETCH_DISABLE 0x00000100
#define  RGN_ROM_PREFETCH_DISABLE 0x00000200
#define  RGN_READ_PREFETCH_COUNT_ENABLE 0x00000400
#define  RGN_RWS           0x003C0000	/* Expn ROM Wait States */
#define  RGN_RRE           0x00400000	/* ROM Space Ready Input Enable */
#define  RGN_RBE           0x00800000	/* ROM Space Bterm Input Enable */
#define  RGN_MBEN          0x01000000	/* Memory Space Burst Enable */
#define  RGN_RBEN          0x04000000	/* ROM Space Burst Enable */
#define  RGN_THROT         0x08000000	/* De-assert TRDY when FIFO full */
#define  RGN_TRD           0xF0000000	/* Target Ready Delay /8 */

#define PLX_REGION1_REG         0x00f8	/* L, Local Bus Region 1 Descriptor */

#define PLX_DMRNG_REG          0x001C	/* L, Direct Master Range Register */

#define PLX_LBAPMEM_REG        0x0020	/* L, Lcl Base Addr for PCI mem space */

#define PLX_LBAPIO_REG         0x0024	/* L, Lcl Base Addr for PCI I/O space */

#define PLX_DMMAP_REG          0x0028	/* L, Direct Master Remap Register */
#define  DMM_MAE           0x00000001	/* Direct Mstr Memory Acc Enable */
#define  DMM_IAE           0x00000002	/* Direct Mstr I/O Acc Enable */
#define  DMM_LCK           0x00000004	/* LOCK Input Enable */
#define  DMM_PF4           0x00000008	/* Prefetch 4 Mode Enable */
#define  DMM_THROT         0x00000010	/* Assert IRDY when read FIFO full */
#define  DMM_PAF0          0x00000000	/* Programmable Almost fill level */
#define  DMM_PAF1          0x00000020	/* Programmable Almost fill level */
#define  DMM_PAF2          0x00000040	/* Programmable Almost fill level */
#define  DMM_PAF3          0x00000060	/* Programmable Almost fill level */
#define  DMM_PAF4          0x00000080	/* Programmable Almost fill level */
#define  DMM_PAF5          0x000000A0	/* Programmable Almost fill level */
#define  DMM_PAF6          0x000000C0	/* Programmable Almost fill level */
#define  DMM_PAF7          0x000000D0	/* Programmable Almost fill level */
#define  DMM_MAP           0xFFFF0000	/* Remap Address Bits */

#define PLX_CAR_REG            0x002C	/* L, Configuration Address Register */
#define  CAR_CT0           0x00000000	/* Config Type 0 */
#define  CAR_CT1           0x00000001	/* Config Type 1 */
#define  CAR_REG           0x000000FC	/* Register Number Bits */
#define  CAR_FUN           0x00000700	/* Function Number Bits */
#define  CAR_DEV           0x0000F800	/* Device Number Bits */
#define  CAR_BUS           0x00FF0000	/* Bus Number Bits */
#define  CAR_CFG           0x80000000	/* Config Spc Access Enable */

#define PLX_DBR_IN_REG         0x0060	/* L, PCI to Local Doorbell Register */

#define PLX_DBR_OUT_REG        0x0064	/* L, Local to PCI Doorbell Register */

#define PLX_INTRCS_REG         0x0068	/* L, Interrupt Control/Status Reg */
#define  ICS_AERR          0x00000001	/* Assert LSERR on ABORT */
#define  ICS_PERR          0x00000002	/* Assert LSERR on Parity Error */
#define  ICS_SERR          0x00000004	/* Generate PCI SERR# */
#define  ICS_MBIE          0x00000008	/*  mailbox interrupt enable */
#define  ICS_PIE           0x00000100	/* PCI Interrupt Enable */
#define  ICS_PDIE          0x00000200	/* PCI Doorbell Interrupt Enable */
#define  ICS_PAIE          0x00000400	/* PCI Abort Interrupt Enable */
#define  ICS_PLIE          0x00000800	/* PCI Local Int Enable */
#define  ICS_RAE           0x00001000	/* Retry Abort Enable */
#define  ICS_PDIA          0x00002000	/* PCI Doorbell Interrupt Active */
#define  ICS_PAIA          0x00004000	/* PCI Abort Interrupt Active */
#define  ICS_LIA           0x00008000	/* Local Interrupt Active */
#define  ICS_LIE           0x00010000	/* Local Interrupt Enable */
#define  ICS_LDIE          0x00020000	/* Local Doorbell Int Enable */
#define  ICS_DMA0_E        0x00040000	/* DMA #0 Interrupt Enable */
#define  ICS_DMA1_E        0x00080000	/* DMA #1 Interrupt Enable */
#define  ICS_LDIA          0x00100000	/* Local Doorbell Int Active */
#define  ICS_DMA0_A        0x00200000	/* DMA #0 Interrupt Active */
#define  ICS_DMA1_A        0x00400000	/* DMA #1 Interrupt Active */
#define  ICS_BIA           0x00800000	/* BIST Interrupt Active */
#define  ICS_TA_DM         0x01000000	/* Target Abort - Direct Master */
#define  ICS_TA_DMA0       0x02000000	/* Target Abort - DMA #0 */
#define  ICS_TA_DMA1       0x04000000	/* Target Abort - DMA #1 */
#define  ICS_TA_RA         0x08000000	/* Target Abort - Retry Timeout */
#define  ICS_MBIA(x)       (0x10000000 << ((x) & 0x3))	/*  mailbox x is active */

#define PLX_CONTROL_REG        0x006C	/* L, EEPROM Cntl & PCI Cmd Codes */
#define  CTL_RDMA          0x0000000E	/* DMA Read Command */
#define  CTL_WDMA          0x00000070	/* DMA Write Command */
#define  CTL_RMEM          0x00000600	/* Memory Read Command */
#define  CTL_WMEM          0x00007000	/* Memory Write Command */
#define  CTL_USERO         0x00010000	/* USERO output pin control bit */
#define  CTL_USERI         0x00020000	/* USERI input pin bit */
#define  CTL_EE_CLK        0x01000000	/* EEPROM Clock line */
#define  CTL_EE_CS         0x02000000	/* EEPROM Chip Select */
#define  CTL_EE_W          0x04000000	/* EEPROM Write bit */
#define  CTL_EE_R          0x08000000	/* EEPROM Read bit */
#define  CTL_EECHK         0x10000000	/* EEPROM Present bit */
#define  CTL_EERLD         0x20000000	/* EEPROM Reload Register */
#define  CTL_RESET         0x40000000	/* !! Adapter Reset !! */
#define  CTL_READY         0x80000000	/* Local Init Done */

#define PLX_ID_REG	0x70	/*  hard-coded plx vendor and device ids */

#define PLX_REVISION_REG	0x74	/*  silicon revision */

#define PLX_DMA0_MODE_REG	0x80	/*  dma channel 0 mode register */
#define PLX_DMA1_MODE_REG	0x94	/*  dma channel 0 mode register */
#define  PLX_LOCAL_BUS_16_WIDE_BITS	0x1
#define  PLX_LOCAL_BUS_32_WIDE_BITS	0x3
#define  PLX_LOCAL_BUS_WIDTH_MASK	0x3
#define  PLX_DMA_EN_READYIN_BIT	0x40	/*  enable ready in input */
#define  PLX_EN_BTERM_BIT	0x80	/*  enable BTERM# input */
#define  PLX_DMA_LOCAL_BURST_EN_BIT	0x100	/*  enable local burst mode */
#define  PLX_EN_CHAIN_BIT	0x200	/*  enables chaining */
#define  PLX_EN_DMA_DONE_INTR_BIT	0x400	/*  enables interrupt on dma done */
#define  PLX_LOCAL_ADDR_CONST_BIT	0x800	/*  hold local address constant (don't increment) */
#define  PLX_DEMAND_MODE_BIT	0x1000	/*  enables demand-mode for dma transfer */
#define  PLX_EOT_ENABLE_BIT	0x4000
#define  PLX_STOP_MODE_BIT 0x8000
#define  PLX_DMA_INTR_PCI_BIT	0x20000	/*  routes dma interrupt to pci bus (instead of local bus) */

#define PLX_DMA0_PCI_ADDRESS_REG	0x84	/*  pci address that dma transfers start at */
#define PLX_DMA1_PCI_ADDRESS_REG	0x98

#define PLX_DMA0_LOCAL_ADDRESS_REG	0x88	/*  local address that dma transfers start at */
#define PLX_DMA1_LOCAL_ADDRESS_REG	0x9c

#define PLX_DMA0_TRANSFER_SIZE_REG	0x8c	/*  number of bytes to transfer (first 23 bits) */
#define PLX_DMA1_TRANSFER_SIZE_REG	0xa0

#define PLX_DMA0_DESCRIPTOR_REG	0x90	/*  descriptor pointer register */
#define PLX_DMA1_DESCRIPTOR_REG	0xa4
#define  PLX_DESC_IN_PCI_BIT	0x1	/*  descriptor is located in pci space (not local space) */
#define  PLX_END_OF_CHAIN_BIT	0x2	/*  end of chain bit */
#define  PLX_INTR_TERM_COUNT	0x4	/*  interrupt when this descriptor's transfer is finished */
#define  PLX_XFER_LOCAL_TO_PCI 0x8	/*  transfer from local to pci bus (not pci to local) */

#define PLX_DMA0_CS_REG	0xa8	/*  command status register */
#define PLX_DMA1_CS_REG	0xa9
#define  PLX_DMA_EN_BIT	0x1	/*  enable dma channel */
#define  PLX_DMA_START_BIT	0x2	/*  start dma transfer */
#define  PLX_DMA_ABORT_BIT	0x4	/*  abort dma transfer */
#define  PLX_CLEAR_DMA_INTR_BIT	0x8	/*  clear dma interrupt */
#define  PLX_DMA_DONE_BIT	0x10	/*  transfer done status bit */

#define PLX_DMA0_THRESHOLD_REG	0xb0	/*  command status register */
=======
#include <linux/compiler.h>
#include <linux/types.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/io.h>

/**
 * struct plx_dma_desc - DMA descriptor format for PLX PCI 9080
 * @pci_start_addr:	PCI Bus address for transfer (DMAPADR).
 * @local_start_addr:	Local Bus address for transfer (DMALADR).
 * @transfer_size:	Transfer size in bytes (max 8 MiB) (DMASIZ).
 * @next:		Address of next descriptor + flags (DMADPR).
 *
 * Describes the format of a scatter-gather DMA descriptor for the PLX
 * PCI 9080.  All members are raw, little-endian register values that
 * will be transferred by the DMA engine from local or PCI memory into
 * corresponding registers for the DMA channel.
 *
 * The DMA descriptors must be aligned on a 16-byte boundary.  Bits 3:0
 * of @next contain flags describing the address space of the next
 * descriptor (local or PCI), an "end of chain" marker, an "interrupt on
 * terminal count" bit, and a data transfer direction.
 */
struct plx_dma_desc {
	__le32 pci_start_addr;
	__le32 local_start_addr;
	__le32 transfer_size;
	__le32 next;
};

/*
 * Register Offsets and Bit Definitions
 */

/* Local Address Space 0 Range Register */
#define PLX_REG_LAS0RR		0x0000
/* Local Address Space 1 Range Register */
#define PLX_REG_LAS1RR		0x00f0

#define PLX_LASRR_IO		BIT(0)		/* Map to: 1=I/O, 0=Mem */
#define PLX_LASRR_MLOC_ANY32	(BIT(1) * 0)	/* Locate anywhere in 32 bit */
#define PLX_LASRR_MLOC_LT1MB	(BIT(1) * 1)	/* Locate in 1st meg */
#define PLX_LASRR_MLOC_ANY64	(BIT(1) * 2)	/* Locate anywhere in 64 bit */
#define PLX_LASRR_MLOC_MASK	GENMASK(2, 1)	/* Memory location bits */
#define PLX_LASRR_PREFETCH	BIT(3)		/* Memory is prefetchable */
/* bits that specify range for memory space decode bits */
#define PLX_LASRR_MEM_MASK	GENMASK(31, 4)
/* bits that specify range for i/o space decode bits */
#define PLX_LASRR_IO_MASK	GENMASK(31, 2)

/* Local Address Space 0 Local Base Address (Remap) Register */
#define PLX_REG_LAS0BA		0x0004
/* Local Address Space 1 Local Base Address (Remap) Register */
#define PLX_REG_LAS1BA		0x00f4

#define PLX_LASBA_EN		BIT(0)		/* Enable slave decode */
/* bits that specify local base address for memory space */
#define PLX_LASBA_MEM_MASK	GENMASK(31, 4)
/* bits that specify local base address for i/o space */
#define PLX_LASBA_IO_MASK	GENMASK(31, 2)

/* Mode/Arbitration Register */
#define PLX_REG_MARBR		0x0008
/* DMA Arbitration Register (alias of MARBR). */
#define PLX_REG_DMAARB		0x00ac

/* Local Bus Latency Timer */
#define PLX_MARBR_LT(x)		(BIT(0) * ((x) & 0xff))
#define PLX_MARBR_LT_MASK	GENMASK(7, 0)
#define PLX_MARBR_TO_LT(r)	((r) & PLX_MARBR_LT_MASK)
/* Local Bus Pause Timer */
#define PLX_MARBR_PT(x)		(BIT(8) * ((x) & 0xff))
#define PLX_MARBR_PT_MASK	GENMASK(15, 8)
#define PLX_MARBR_TO_PT(r)	(((r) & PLX_MARBR_PT_MASK) >> 8)
/* Local Bus Latency Timer Enable */
#define PLX_MARBR_LTEN		BIT(16)
/* Local Bus Pause Timer Enable */
#define PLX_MARBR_PTEN		BIT(17)
/* Local Bus BREQ Enable */
#define PLX_MARBR_BREQEN	BIT(18)
/* DMA Channel Priority */
#define PLX_MARBR_PRIO_ROT	(BIT(19) * 0)	/* Rotational priority */
#define PLX_MARBR_PRIO_DMA0	(BIT(19) * 1)	/* DMA channel 0 has priority */
#define PLX_MARBR_PRIO_DMA1	(BIT(19) * 2)	/* DMA channel 1 has priority */
#define PLX_MARBR_PRIO_MASK	GENMASK(20, 19)
/* Local Bus Direct Slave Give Up Bus Mode */
#define PLX_MARBR_DSGUBM	BIT(21)
/* Direct Slace LLOCKo# Enable */
#define PLX_MARBR_DSLLOCKOEN	BIT(22)
/* PCI Request Mode */
#define PLX_MARBR_PCIREQM	BIT(23)
/* PCI Specification v2.1 Mode */
#define PLX_MARBR_PCIV21M	BIT(24)
/* PCI Read No Write Mode */
#define PLX_MARBR_PCIRNWM	BIT(25)
/* PCI Read with Write Flush Mode */
#define PLX_MARBR_PCIRWFM	BIT(26)
/* Gate Local Bus Latency Timer with BREQ */
#define PLX_MARBR_GLTBREQ	BIT(27)
/* PCI Read No Flush Mode */
#define PLX_MARBR_PCIRNFM	BIT(28)
/*
 * Make reads from PCI Configuration register 0 return Subsystem ID and
 * Subsystem Vendor ID instead of Device ID and Vendor ID
 */
#define PLX_MARBR_SUBSYSIDS	BIT(29)

/* Big/Little Endian Descriptor Register */
#define PLX_REG_BIGEND		0x000c

/* Configuration Register Big Endian Mode */
#define PLX_BIGEND_CONFIG	BIT(0)
/* Direct Master Big Endian Mode */
#define PLX_BIGEND_DM		BIT(1)
/* Direct Slave Address Space 0 Big Endian Mode */
#define PLX_BIGEND_DSAS0	BIT(2)
/* Direct Slave Expansion ROM Big Endian Mode */
#define PLX_BIGEND_EROM		BIT(3)
/* Big Endian Byte Lane Mode - use most significant byte lanes */
#define PLX_BIGEND_BEBLM	BIT(4)
/* Direct Slave Address Space 1 Big Endian Mode */
#define PLX_BIGEND_DSAS1	BIT(5)
/* DMA Channel 1 Big Endian Mode */
#define PLX_BIGEND_DMA1		BIT(6)
/* DMA Channel 0 Big Endian Mode */
#define PLX_BIGEND_DMA0		BIT(7)
/* DMA Channel N Big Endian Mode (N <= 1) */
#define PLX_BIGEND_DMA(n)	((n) ? PLX_BIGEND_DMA1 : PLX_BIGEND_DMA0)

/*
 * Note: The Expansion ROM  stuff is only relevant to the PC environment.
 *       This expansion ROM code is executed by the host CPU at boot time.
 *       For this reason no bit definitions are provided here.
 */

/* Expansion ROM Range Register */
#define PLX_REG_EROMRR		0x0010
/* Expansion ROM Local Base Address (Remap) Register */
#define PLX_REG_EROMBA		0x0014

/* Local Address Space 0/Expansion ROM Bus Region Descriptor Register */
#define PLX_REG_LBRD0		0x0018
/* Local Address Space 1 Bus Region Descriptor Register */
#define PLX_REG_LBRD1		0x00f8

/* Memory Space Local Bus Width */
#define PLX_LBRD_MSWIDTH_8	(BIT(0) * 0)	/* 8 bits wide */
#define PLX_LBRD_MSWIDTH_16	(BIT(0) * 1)	/* 16 bits wide */
#define PLX_LBRD_MSWIDTH_32	(BIT(0) * 2)	/* 32 bits wide */
#define PLX_LBRD_MSWIDTH_32A	(BIT(0) * 3)	/* 32 bits wide */
#define PLX_LBRD_MSWIDTH_MASK	GENMASK(1, 0)
/* Memory Space Internal Wait States */
#define PLX_LBRD_MSIWS(x)	(BIT(2) * ((x) & 0xf))
#define PLX_LBRD_MSIWS_MASK	GENMASK(5, 2)
#define PLX_LBRD_TO_MSIWS(r)	(((r) & PLS_LBRD_MSIWS_MASK) >> 2)
/* Memory Space Ready Input Enable */
#define PLX_LBRD_MSREADYIEN	BIT(6)
/* Memory Space BTERM# Input Enable */
#define PLX_LBRD_MSBTERMIEN	BIT(7)
/* Memory Space 0 Prefetch Disable (LBRD0 only) */
#define PLX_LBRD0_MSPREDIS	BIT(8)
/* Memory Space 1 Burst Enable (LBRD1 only) */
#define PLX_LBRD1_MSBURSTEN	BIT(8)
/* Expansion ROM Space Prefetch Disable (LBRD0 only) */
#define PLX_LBRD0_EROMPREDIS	BIT(9)
/* Memory Space 1 Prefetch Disable (LBRD1 only) */
#define PLX_LBRD1_MSPREDIS	BIT(9)
/* Read Prefetch Count Enable */
#define PLX_LBRD_RPFCOUNTEN	BIT(10)
/* Prefetch Counter */
#define PLX_LBRD_PFCOUNT(x)	(BIT(11) * ((x) & 0xf))
#define PLX_LBRD_PFCOUNT_MASK	GENMASK(14, 11)
#define PLX_LBRD_TO_PFCOUNT(r)	(((r) & PLX_LBRD_PFCOUNT_MASK) >> 11)
/* Expansion ROM Space Local Bus Width (LBRD0 only) */
#define PLX_LBRD0_EROMWIDTH_8	(BIT(16) * 0)	/* 8 bits wide */
#define PLX_LBRD0_EROMWIDTH_16	(BIT(16) * 1)	/* 16 bits wide */
#define PLX_LBRD0_EROMWIDTH_32	(BIT(16) * 2)	/* 32 bits wide */
#define PLX_LBRD0_EROMWIDTH_32A	(BIT(16) * 3)	/* 32 bits wide */
#define PLX_LBRD0_EROMWIDTH_MASK	GENMASK(17, 16)
/* Expansion ROM Space Internal Wait States (LBRD0 only) */
#define PLX_LBRD0_EROMIWS(x)	(BIT(18) * ((x) & 0xf))
#define PLX_LBRD0_EROMIWS_MASK	GENMASK(21, 18)
#define PLX_LBRD0_TO_EROMIWS(r)	(((r) & PLX_LBRD0_EROMIWS_MASK) >> 18)
/* Expansion ROM Space Ready Input Enable (LBDR0 only) */
#define PLX_LBRD0_EROMREADYIEN	BIT(22)
/* Expansion ROM Space BTERM# Input Enable (LBRD0 only) */
#define PLX_LBRD0_EROMBTERMIEN	BIT(23)
/* Memory Space 0 Burst Enable (LBRD0 only) */
#define PLX_LBRD0_MSBURSTEN	BIT(24)
/* Extra Long Load From Serial EEPROM  (LBRD0 only) */
#define PLX_LBRD0_EELONGLOAD	BIT(25)
/* Expansion ROM Space Burst Enable (LBRD0 only) */
#define PLX_LBRD0_EROMBURSTEN	BIT(26)
/* Direct Slave PCI Write Mode - assert TRDY# when FIFO full (LBRD0 only) */
#define PLX_LBRD0_DSWMTRDY	BIT(27)
/* PCI Target Retry Delay Clocks / 8 (LBRD0 only) */
#define PLX_LBRD0_TRDELAY(x)	(BIT(28) * ((x) & 0xF))
#define PLX_LBRD0_TRDELAY_MASK	GENMASK(31, 28)
#define PLX_LBRD0_TO_TRDELAY(r)	(((r) & PLX_LBRD0_TRDELAY_MASK) >> 28)

/* Local Range Register for Direct Master to PCI */
#define PLX_REG_DMRR		0x001c

/* Local Bus Base Address Register for Direct Master to PCI Memory */
#define PLX_REG_DMLBAM		0x0020

/* Local Base Address Register for Direct Master to PCI IO/CFG */
#define PLX_REG_DMLBAI		0x0024

/* PCI Base Address (Remap) Register for Direct Master to PCI Memory */
#define PLX_REG_DMPBAM		0x0028

/* Direct Master Memory Access Enable */
#define PLX_DMPBAM_MEMACCEN	BIT(0)
/* Direct Master I/O Access Enable */
#define PLX_DMPBAM_IOACCEN	BIT(1)
/* LLOCK# Input Enable */
#define PLX_DMPBAM_LLOCKIEN	BIT(2)
/* Direct Master Read Prefetch Size Control (bits 12, 3) */
#define PLX_DMPBAM_RPSIZE_CONT	((BIT(12) * 0) | (BIT(3) * 0))
#define PLX_DMPBAM_RPSIZE_4	((BIT(12) * 0) | (BIT(3) * 1))
#define PLX_DMPBAM_RPSIZE_8	((BIT(12) * 1) | (BIT(3) * 0))
#define PLX_DMPBAM_RPSIZE_16	((BIT(12) * 1) | (BIT(3) * 1))
#define PLX_DMPBAM_RPSIZE_MASK	(BIT(12) | BIT(3))
/* Direct Master PCI Read Mode - deassert IRDY when FIFO full */
#define PLX_DMPBAM_RMIRDY	BIT(4)
/* Programmable Almost Full Level (bits 10, 8:5) */
#define PLX_DMPBAM_PAFL(x)	((BIT(10) * !!((x) & 0x10)) | \
				 (BIT(5) * ((x) & 0xf)))
#define PLX_DMPBAM_TO_PAFL(v)	((((BIT(10) & (v)) >> 1) | \
				  (GENMASK(8, 5) & (v))) >> 5)
#define PLX_DMPBAM_PAFL_MASK	(BIT(10) | GENMASK(8, 5))
/* Write And Invalidate Mode */
#define PLX_DMPBAM_WIM		BIT(9)
/* Direct Master Prefetch Limit */
#define PLX_DBPBAM_PFLIMIT	BIT(11)
/* I/O Remap Select */
#define PLX_DMPBAM_IOREMAPSEL	BIT(13)
/* Direct Master Write Delay */
#define PLX_DMPBAM_WDELAY_NONE	(BIT(14) * 0)
#define PLX_DMPBAM_WDELAY_4	(BIT(14) * 1)
#define PLX_DMPBAM_WDELAY_8	(BIT(14) * 2)
#define PLX_DMPBAM_WDELAY_16	(BIT(14) * 3)
#define PLX_DMPBAM_WDELAY_MASK	GENMASK(15, 14)
/* Remap of Local-to-PCI Space Into PCI Address Space */
#define PLX_DMPBAM_REMAP_MASK	GENMASK(31, 16)

/* PCI Configuration Address Register for Direct Master to PCI IO/CFG */
#define PLX_REG_DMCFGA		0x002c

/* Congiguration Type */
#define PLX_DMCFGA_TYPE0	(BIT(0) * 0)
#define PLX_DMCFGA_TYPE1	(BIT(0) * 1)
#define PLX_DMCFGA_TYPE_MASK	GENMASK(1, 0)
/* Register Number */
#define PLX_DMCFGA_REGNUM(x)	(BIT(2) * ((x) & 0x3f))
#define PLX_DMCFGA_REGNUM_MASK	GENMASK(7, 2)
#define PLX_DMCFGA_TO_REGNUM(r)	(((r) & PLX_DMCFGA_REGNUM_MASK) >> 2)
/* Function Number */
#define PLX_DMCFGA_FUNCNUM(x)	(BIT(8) * ((x) & 0x7))
#define PLX_DMCFGA_FUNCNUM_MASK	GENMASK(10, 8)
#define PLX_DMCFGA_TO_FUNCNUM(r) (((r) & PLX_DMCFGA_FUNCNUM_MASK) >> 8)
/* Device Number */
#define PLX_DMCFGA_DEVNUM(x)	(BIT(11) * ((x) & 0x1f))
#define PLX_DMCFGA_DEVNUM_MASK	GENMASK(15, 11)
#define PLX_DMCFGA_TO_DEVNUM(r)	(((r) & PLX_DMCFGA_DEVNUM_MASK) >> 11)
/* Bus Number */
#define PLX_DMCFGA_BUSNUM(x)	(BIT(16) * ((x) & 0xff))
#define PLX_DMCFGA_BUSNUM_MASK	GENMASK(23, 16)
#define PLX_DMCFGA_TO_BUSNUM(r)	(((r) & PLX_DMCFGA_BUSNUM_MASK) >> 16)
/* Configuration Enable */
#define PLX_DMCFGA_CONFIGEN	BIT(31)

/*
 * Mailbox Register N (N <= 7)
 *
 * Note that if the I2O feature is enabled (QSR[0] is set), Mailbox Register 0
 * is replaced by the Inbound Queue Port, and Mailbox Register 1 is replaced
 * by the Outbound Queue Port.  However, Mailbox Register 0 and 1 are always
 * accessible at alternative offsets if the I2O feature is enabled.
 */
#define PLX_REG_MBOX(n)		(0x0040 + (n) * 4)
#define PLX_REG_MBOX0		PLX_REG_MBOX(0)
#define PLX_REG_MBOX1		PLX_REG_MBOX(1)
#define PLX_REG_MBOX2		PLX_REG_MBOX(2)
#define PLX_REG_MBOX3		PLX_REG_MBOX(3)
#define PLX_REG_MBOX4		PLX_REG_MBOX(4)
#define PLX_REG_MBOX5		PLX_REG_MBOX(5)
#define PLX_REG_MBOX6		PLX_REG_MBOX(6)
#define PLX_REG_MBOX7		PLX_REG_MBOX(7)

/* Alternative offsets for Mailbox Registers 0 and 1 (in case I2O is enabled) */
#define PLX_REG_ALT_MBOX(n)	((n) < 2 ? 0x0078 + (n) * 4 : PLX_REG_MBOX(n))
#define PLX_REG_ALT_MBOX0	PLX_REG_ALT_MBOX(0)
#define PLX_REG_ALT_MBOX1	PLX_REG_ALT_MBOX(1)

/* PCI-to-Local Doorbell Register */
#define PLX_REG_P2LDBELL	0x0060

/* Local-to-PCI Doorbell Register */
#define PLX_REG_L2PDBELL	0x0064

/* Interrupt Control/Status Register */
#define PLX_REG_INTCSR		0x0068

/* Enable Local Bus LSERR# when PCI Bus Target Abort or Master Abort occurs */
#define PLX_INTCSR_LSEABORTEN	BIT(0)
/* Enable Local Bus LSERR# when PCI parity error occurs */
#define PLX_INTCSR_LSEPARITYEN	BIT(1)
/* Generate PCI Bus SERR# when set to 1 */
#define PLX_INTCSR_GENSERR	BIT(2)
/* Mailbox Interrupt Enable (local bus interrupts on PCI write to MBOX0-3) */
#define PLX_INTCSR_MBIEN	BIT(3)
/* PCI Interrupt Enable */
#define PLX_INTCSR_PIEN		BIT(8)
/* PCI Doorbell Interrupt Enable */
#define PLX_INTCSR_PDBIEN	BIT(9)
/* PCI Abort Interrupt Enable */
#define PLX_INTCSR_PABORTIEN	BIT(10)
/* PCI Local Interrupt Enable */
#define PLX_INTCSR_PLIEN	BIT(11)
/* Retry Abort Enable (for diagnostic purposes only) */
#define PLX_INTCSR_RAEN		BIT(12)
/* PCI Doorbell Interrupt Active (read-only) */
#define PLX_INTCSR_PDBIA	BIT(13)
/* PCI Abort Interrupt Active (read-only) */
#define PLX_INTCSR_PABORTIA	BIT(14)
/* Local Interrupt (LINTi#) Active (read-only) */
#define PLX_INTCSR_PLIA		BIT(15)
/* Local Interrupt Output (LINTo#) Enable */
#define PLX_INTCSR_LIOEN	BIT(16)
/* Local Doorbell Interrupt Enable */
#define PLX_INTCSR_LDBIEN	BIT(17)
/* DMA Channel 0 Interrupt Enable */
#define PLX_INTCSR_DMA0IEN	BIT(18)
/* DMA Channel 1 Interrupt Enable */
#define PLX_INTCSR_DMA1IEN	BIT(19)
/* DMA Channel N Interrupt Enable (N <= 1) */
#define PLX_INTCSR_DMAIEN(n)	((n) ? PLX_INTCSR_DMA1IEN : PLX_INTCSR_DMA0IEN)
/* Local Doorbell Interrupt Active (read-only) */
#define PLX_INTCSR_LDBIA	BIT(20)
/* DMA Channel 0 Interrupt Active (read-only) */
#define PLX_INTCSR_DMA0IA	BIT(21)
/* DMA Channel 1 Interrupt Active (read-only) */
#define PLX_INTCSR_DMA1IA	BIT(22)
/* DMA Channel N Interrupt Active (N <= 1) (read-only) */
#define PLX_INTCSR_DMAIA(n)	((n) ? PLX_INTCSR_DMA1IA : PLX_INTCSR_DMA0IA)
/* BIST Interrupt Active (read-only) */
#define PLX_INTCSR_BISTIA	BIT(23)
/* Direct Master Not Bus Master During Master Or Target Abort (read-only) */
#define PLX_INTCSR_ABNOTDM	BIT(24)
/* DMA Channel 0 Not Bus Master During Master Or Target Abort (read-only) */
#define PLX_INTCSR_ABNOTDMA0	BIT(25)
/* DMA Channel 1 Not Bus Master During Master Or Target Abort (read-only) */
#define PLX_INTCSR_ABNOTDMA1	BIT(26)
/* DMA Channel N Not Bus Master During Master Or Target Abort (read-only) */
#define PLX_INTCSR_ABNOTDMA(n)	((n) ? PLX_INTCSR_ABNOTDMA1 \
				     : PLX_INTCSR_ABNOTDMA0)
/* Target Abort Not Generated After 256 Master Retries (read-only) */
#define PLX_INTCSR_ABNOTRETRY	BIT(27)
/* PCI Wrote Mailbox 0 (enabled if bit 3 set) (read-only) */
#define PLX_INTCSR_MB0IA	BIT(28)
/* PCI Wrote Mailbox 1 (enabled if bit 3 set) (read-only) */
#define PLX_INTCSR_MB1IA	BIT(29)
/* PCI Wrote Mailbox 2 (enabled if bit 3 set) (read-only) */
#define PLX_INTCSR_MB2IA	BIT(30)
/* PCI Wrote Mailbox 3 (enabled if bit 3 set) (read-only) */
#define PLX_INTCSR_MB3IA	BIT(31)
/* PCI Wrote Mailbox N (N <= 3) (enabled if bit 3 set) (read-only) */
#define PLX_INTCSR_MBIA(n)	BIT(28 + (n))

/*
 * Serial EEPROM Control, PCI Command Codes, User I/O Control,
 * Init Control Register
 */
#define PLX_REG_CNTRL		0x006c

/* PCI Read Command Code For DMA */
#define PLX_CNTRL_CCRDMA(x)	(BIT(0) * ((x) & 0xf))
#define PLX_CNTRL_CCRDMA_MASK	GENMASK(3, 0)
#define PLX_CNTRL_TO_CCRDMA(r)	((r) & PLX_CNTRL_CCRDMA_MASK)
#define PLX_CNTRL_CCRDMA_NORMAL	PLX_CNTRL_CCRDMA(14)	/* value after reset */
/* PCI Write Command Code For DMA 0 */
#define PLX_CNTRL_CCWDMA(x)	(BIT(4) * ((x) & 0xf))
#define PLX_CNTRL_CCWDMA_MASK	GENMASK(7, 4)
#define PLX_CNTRL_TO_CCWDMA(r)	(((r) & PLX_CNTRL_CCWDMA_MASK) >> 4)
#define PLX_CNTRL_CCWDMA_NORMAL	PLX_CNTRL_CCWDMA(7)	/* value after reset */
/* PCI Memory Read Command Code For Direct Master */
#define PLX_CNTRL_CCRDM(x)	(BIT(8) * ((x) & 0xf))
#define PLX_CNTRL_CCRDM_MASK	GENMASK(11, 8)
#define PLX_CNTRL_TO_CCRDM(r)	(((r) & PLX_CNTRL_CCRDM_MASK) >> 8)
#define PLX_CNTRL_CCRDM_NORMAL	PLX_CNTRL_CCRDM(6)	/* value after reset */
/* PCI Memory Write Command Code For Direct Master */
#define PLX_CNTRL_CCWDM(x)	(BIT(12) * ((x) & 0xf))
#define PLX_CNTRL_CCWDM_MASK	GENMASK(15, 12)
#define PLX_CNTRL_TO_CCWDM(r)	(((r) & PLX_CNTRL_CCWDM_MASK) >> 12)
#define PLX_CNTRL_CCWDM_NORMAL	PLX_CNTRL_CCWDM(7)	/* value after reset */
/* General Purpose Output (USERO) */
#define PLX_CNTRL_USERO		BIT(16)
/* General Purpose Input (USERI) (read-only) */
#define PLX_CNTRL_USERI		BIT(17)
/* Serial EEPROM Clock Output (EESK) */
#define PLX_CNTRL_EESK		BIT(24)
/* Serial EEPROM Chip Select Output (EECS) */
#define PLX_CNTRL_EECS		BIT(25)
/* Serial EEPROM Data Write Bit (EEDI (sic)) */
#define PLX_CNTRL_EEWB		BIT(26)
/* Serial EEPROM Data Read Bit (EEDO (sic)) (read-only) */
#define PLX_CNTRL_EERB		BIT(27)
/* Serial EEPROM Present (read-only) */
#define PLX_CNTRL_EEPRESENT	BIT(28)
/* Reload Configuration Registers from EEPROM */
#define PLX_CNTRL_EERELOAD	BIT(29)
/* PCI Adapter Software Reset (asserts LRESETo#) */
#define PLX_CNTRL_RESET		BIT(30)
/* Local Init Status (read-only) */
#define PLX_CNTRL_INITDONE	BIT(31)
/*
 * Combined command code stuff for convenience.
 */
#define PLX_CNTRL_CC_MASK	\
	(PLX_CNTRL_CCRDMA_MASK | PLX_CNTRL_CCWDMA_MASK | \
	 PLX_CNTRL_CCRDM_MASK | PLX_CNTRL_CCWDM_MASK)
#define PLX_CNTRL_CC_NORMAL	\
	(PLX_CNTRL_CCRDMA_NORMAL | PLX_CNTRL_CCWDMA_NORMAL | \
	 PLX_CNTRL_CCRDM_NORMAL | PLX_CNTRL_CCWDM_NORMAL) /* val after reset */

/* PCI Permanent Configuration ID Register (hard-coded PLX vendor and device) */
#define PLX_REG_PCIHIDR		0x0070

/* Hard-coded ID for PLX PCI 9080 */
#define PLX_PCIHIDR_9080	0x908010b5

/* PCI Permanent Revision ID Register (hard-coded silicon revision) (8-bit). */
#define PLX_REG_PCIHREV		0x0074

/* DMA Channel N Mode Register (N <= 1) */
#define PLX_REG_DMAMODE(n)	((n) ? PLX_REG_DMAMODE1 : PLX_REG_DMAMODE0)
#define PLX_REG_DMAMODE0	0x0080
#define PLX_REG_DMAMODE1	0x0094

/* Local Bus Width */
#define PLX_DMAMODE_WIDTH_8	(BIT(0) * 0)	/* 8 bits wide */
#define PLX_DMAMODE_WIDTH_16	(BIT(0) * 1)	/* 16 bits wide */
#define PLX_DMAMODE_WIDTH_32	(BIT(0) * 2)	/* 32 bits wide */
#define PLX_DMAMODE_WIDTH_32A	(BIT(0) * 3)	/* 32 bits wide */
#define PLX_DMAMODE_WIDTH_MASK	GENMASK(1, 0)
/* Internal Wait States */
#define PLX_DMAMODE_IWS(x)	(BIT(2) * ((x) & 0xf))
#define PLX_DMAMODE_IWS_MASK	GENMASK(5, 2)
#define PLX_DMAMODE_TO_IWS(r)	(((r) & PLX_DMAMODE_IWS_MASK) >> 2)
/* Ready Input Enable */
#define PLX_DMAMODE_READYIEN	BIT(6)
/* BTERM# Input Enable */
#define PLX_DMAMODE_BTERMIEN	BIT(7)
/* Local Burst Enable */
#define PLX_DMAMODE_BURSTEN	BIT(8)
/* Chaining Enable */
#define PLX_DMAMODE_CHAINEN	BIT(9)
/* Done Interrupt Enable */
#define PLX_DMAMODE_DONEIEN	BIT(10)
/* Hold Local Address Constant */
#define PLX_DMAMODE_LACONST	BIT(11)
/* Demand Mode */
#define PLX_DMAMODE_DEMAND	BIT(12)
/* Write And Invalidate Mode */
#define PLX_DMAMODE_WINVALIDATE	BIT(13)
/* DMA EOT Enable - enables EOT0# or EOT1# input pin */
#define PLX_DMAMODE_EOTEN	BIT(14)
/* DMA Stop Data Transfer Mode - 0:BLAST; 1:EOT asserted or DREQ deasserted */
#define PLX_DMAMODE_STOP	BIT(15)
/* DMA Clear Count Mode - count in descriptor cleared on completion */
#define PLX_DMAMODE_CLRCOUNT	BIT(16)
/* DMA Channel Interrupt Select - 0:local bus interrupt; 1:PCI interrupt */
#define PLX_DMAMODE_INTRPCI	BIT(17)

/* DMA Channel N PCI Address Register (N <= 1) */
#define PLX_REG_DMAPADR(n)	((n) ? PLX_REG_DMAPADR1 : PLX_REG_DMAPADR0)
#define PLX_REG_DMAPADR0	0x0084
#define PLX_REG_DMAPADR1	0x0098

/* DMA Channel N Local Address Register (N <= 1) */
#define PLX_REG_DMALADR(n)	((n) ? PLX_REG_DMALADR1 : PLX_REG_DMALADR0)
#define PLX_REG_DMALADR0	0x0088
#define PLX_REG_DMALADR1	0x009c

/* DMA Channel N Transfer Size (Bytes) Register (N <= 1) (first 23 bits) */
#define PLX_REG_DMASIZ(n)	((n) ? PLX_REG_DMASIZ1 : PLX_REG_DMASIZ0)
#define PLX_REG_DMASIZ0		0x008c
#define PLX_REG_DMASIZ1		0x00a0

/* DMA Channel N Descriptor Pointer Register (N <= 1) */
#define PLX_REG_DMADPR(n)	((n) ? PLX_REG_DMADPR1 : PLX_REG_DMADPR0)
#define PLX_REG_DMADPR0		0x0090
#define PLX_REG_DMADPR1		0x00a4

/* Descriptor Located In PCI Address Space (not local address space) */
#define PLX_DMADPR_DESCPCI	BIT(0)
/* End Of Chain */
#define PLX_DMADPR_CHAINEND	BIT(1)
/* Interrupt After Terminal Count */
#define PLX_DMADPR_TCINTR	BIT(2)
/* Direction Of Transfer Local Bus To PCI (not PCI to local) */
#define PLX_DMADPR_XFERL2P	BIT(3)
/* Next Descriptor Address Bits 31:4 (16 byte boundary) */
#define PLX_DMADPR_NEXT_MASK	GENMASK(31, 4)

/* DMA Channel N Command/Status Register (N <= 1) (8-bit) */
#define PLX_REG_DMACSR(n)	((n) ? PLX_REG_DMACSR1 : PLX_REG_DMACSR0)
#define PLX_REG_DMACSR0		0x00a8
#define PLX_REG_DMACSR1		0x00a9

/* Channel Enable */
#define PLX_DMACSR_ENABLE	BIT(0)
/* Channel Start - write 1 to start transfer (write-only) */
#define PLX_DMACSR_START	BIT(1)
/* Channel Abort - write 1 to abort transfer (write-only) */
#define PLX_DMACSR_ABORT	BIT(2)
/* Clear Interrupt - write 1 to clear DMA Channel Interrupt (write-only) */
#define PLX_DMACSR_CLEARINTR	BIT(3)
/* Channel Done - transfer complete/inactive (read-only) */
#define PLX_DMACSR_DONE		BIT(4)

/* DMA Threshold Register */
#define PLX_REG_DMATHR		0x00b0

/*
 * DMA Threshold constraints:
 * (C0PLAF + 1) + (C0PLAE + 1) <= 32
 * (C0LPAF + 1) + (C0LPAE + 1) <= 32
 * (C1PLAF + 1) + (C1PLAE + 1) <= 16
 * (C1LPAF + 1) + (C1LPAE + 1) <= 16
 */

/* DMA Channel 0 PCI-to-Local Almost Full (divided by 2, minus 1) */
#define PLX_DMATHR_C0PLAF(x)	(BIT(0) * ((x) & 0xf))
#define PLX_DMATHR_C0PLAF_MASK	GENMASK(3, 0)
#define PLX_DMATHR_TO_C0PLAF(r)	((r) & PLX_DMATHR_C0PLAF_MASK)
/* DMA Channel 0 Local-to-PCI Almost Empty (divided by 2, minus 1) */
#define PLX_DMATHR_C0LPAE(x)	(BIT(4) * ((x) & 0xf))
#define PLX_DMATHR_C0LPAE_MASK	GENMASK(7, 4)
#define PLX_DMATHR_TO_C0LPAE(r)	(((r) & PLX_DMATHR_C0LPAE_MASK) >> 4)
/* DMA Channel 0 Local-to-PCI Almost Full (divided by 2, minus 1) */
#define PLX_DMATHR_C0LPAF(x)	(BIT(8) * ((x) & 0xf))
#define PLX_DMATHR_C0LPAF_MASK	GENMASK(11, 8)
#define PLX_DMATHR_TO_C0LPAF(r)	(((r) & PLX_DMATHR_C0LPAF_MASK) >> 8)
/* DMA Channel 0 PCI-to-Local Almost Empty (divided by 2, minus 1) */
#define PLX_DMATHR_C0PLAE(x)	(BIT(12) * ((x) & 0xf))
#define PLX_DMATHR_C0PLAE_MASK	GENMASK(15, 12)
#define PLX_DMATHR_TO_C0PLAE(r)	(((r) & PLX_DMATHR_C0PLAE_MASK) >> 12)
/* DMA Channel 1 PCI-to-Local Almost Full (divided by 2, minus 1) */
#define PLX_DMATHR_C1PLAF(x)	(BIT(16) * ((x) & 0xf))
#define PLX_DMATHR_C1PLAF_MASK	GENMASK(19, 16)
#define PLX_DMATHR_TO_C1PLAF(r)	(((r) & PLX_DMATHR_C1PLAF_MASK) >> 16)
/* DMA Channel 1 Local-to-PCI Almost Empty (divided by 2, minus 1) */
#define PLX_DMATHR_C1LPAE(x)	(BIT(20) * ((x) & 0xf))
#define PLX_DMATHR_C1LPAE_MASK	GENMASK(23, 20)
#define PLX_DMATHR_TO_C1LPAE(r)	(((r) & PLX_DMATHR_C1LPAE_MASK) >> 20)
/* DMA Channel 1 Local-to-PCI Almost Full (divided by 2, minus 1) */
#define PLX_DMATHR_C1LPAF(x)	(BIT(24) * ((x) & 0xf))
#define PLX_DMATHR_C1LPAF_MASK	GENMASK(27, 24)
#define PLX_DMATHR_TO_C1LPAF(r)	(((r) & PLX_DMATHR_C1LPAF_MASK) >> 24)
/* DMA Channel 1 PCI-to-Local Almost Empty (divided by 2, minus 1) */
#define PLX_DMATHR_C1PLAE(x)	(BIT(28) * ((x) & 0xf))
#define PLX_DMATHR_C1PLAE_MASK	GENMASK(31, 28)
#define PLX_DMATHR_TO_C1PLAE(r)	(((r) & PLX_DMATHR_C1PLAE_MASK) >> 28)

/*
 * Messaging Queue Registers OPLFIS, OPLFIM, IQP, OQP, MQCR, QBAR, IFHPR,
 * IFTPR, IPHPR, IPTPR, OFHPR, OFTPR, OPHPR, OPTPR, and QSR have been omitted.
 * They are used by the I2O feature.  (IQP and OQP occupy the usual offsets of
 * the MBOX0 and MBOX1 registers if the I2O feature is enabled, but MBOX0 and
 * MBOX1 are accessible via alternative offsets.
 */

/* Queue Status/Control Register */
#define PLX_REG_QSR		0x00e8

/* Value of QSR after reset - disables I2O feature completely. */
#define PLX_QSR_VALUE_AFTER_RESET	0x00000050
>>>>>>> v4.9.227

/*
 * Accesses near the end of memory can cause the PLX chip
 * to pre-fetch data off of end-of-ram.  Limit the size of
 * memory so host-side accesses cannot occur.
 */

#define PLX_PREFETCH   32

<<<<<<< HEAD
/*
 * The PCI Interface, via the PCI-9060 Chip, has up to eight (8) Mailbox
 * Registers.  The PUTS (Power-Up Test Suite) handles the board-side
 * interface/interaction using the first 4 registers.  Specifications for
 * the use of the full PUTS' command and status interface is contained
 * within a separate SBE PUTS Manual.  The Host-Side Device Driver only
 * uses a subset of the full PUTS interface.
 */

/*****************************************/
/***    MAILBOX #(-1) - MEM ACCESS STS ***/
/*****************************************/

#define MBX_STS_VALID      0x57584744	/* 'WXGD' */
#define MBX_STS_DILAV      0x44475857	/* swapped = 'DGXW' */

/*****************************************/
/***    MAILBOX #0  -  PUTS STATUS     ***/
/*****************************************/

#define MBX_STS_MASK       0x000000ff	/* PUTS Status Register bits */
#define MBX_STS_TMASK      0x0000000f	/* register bits for TEST number */

#define MBX_STS_PCIRESET   0x00000100	/* Host issued PCI reset request */
#define MBX_STS_BUSY       0x00000080	/* PUTS is in progress */
#define MBX_STS_ERROR      0x00000040	/* PUTS has failed */
#define MBX_STS_RESERVED   0x000000c0	/* Undefined -> status in transition.
					   We are in process of changing
					   bits; we SET Error bit before
					   RESET of Busy bit */

#define MBX_RESERVED_5     0x00000020	/* FYI: reserved/unused bit */
#define MBX_RESERVED_4     0x00000010	/* FYI: reserved/unused bit */

/******************************************/
/***    MAILBOX #1  -  PUTS COMMANDS    ***/
/******************************************/

/*
 * Any attempt to execute an unimplement command results in the PUTS
 * interface executing a NOOP and continuing as if the offending command
 * completed normally.  Note: this supplies a simple method to interrogate
 * mailbox command processing functionality.
 */

#define MBX_CMD_MASK       0xffff0000	/* PUTS Command Register bits */

#define MBX_CMD_ABORTJ     0x85000000	/* abort and jump */
#define MBX_CMD_RESETP     0x86000000	/* reset and pause at start */
#define MBX_CMD_PAUSE      0x87000000	/* pause immediately */
#define MBX_CMD_PAUSEC     0x88000000	/* pause on completion */
#define MBX_CMD_RESUME     0x89000000	/* resume operation */
#define MBX_CMD_STEP       0x8a000000	/* single step tests */

#define MBX_CMD_BSWAP      0x8c000000	/* identify byte swap scheme */
#define MBX_CMD_BSWAP_0    0x8c000000	/* use scheme 0 */
#define MBX_CMD_BSWAP_1    0x8c000001	/* use scheme 1 */

#define MBX_CMD_SETHMS     0x8d000000	/* setup host memory access window
					   size */
#define MBX_CMD_SETHBA     0x8e000000	/* setup host memory access base
					   address */
#define MBX_CMD_MGO        0x8f000000	/* perform memory setup and continue
					   (IE. Done) */
#define MBX_CMD_NOOP       0xFF000000	/* dummy, illegal command */

/*****************************************/
/***    MAILBOX #2  -  MEMORY SIZE     ***/
/*****************************************/

#define MBX_MEMSZ_MASK     0xffff0000	/* PUTS Memory Size Register bits */

#define MBX_MEMSZ_128KB    0x00020000	/* 128 kilobyte board */
#define MBX_MEMSZ_256KB    0x00040000	/* 256 kilobyte board */
#define MBX_MEMSZ_512KB    0x00080000	/* 512 kilobyte board */
#define MBX_MEMSZ_1MB      0x00100000	/* 1 megabyte board */
#define MBX_MEMSZ_2MB      0x00200000	/* 2 megabyte board */
#define MBX_MEMSZ_4MB      0x00400000	/* 4 megabyte board */
#define MBX_MEMSZ_8MB      0x00800000	/* 8 megabyte board */
#define MBX_MEMSZ_16MB     0x01000000	/* 16 megabyte board */

/***************************************/
/***    MAILBOX #2  -  BOARD TYPE    ***/
/***************************************/

#define MBX_BTYPE_MASK          0x0000ffff	/* PUTS Board Type Register */
#define MBX_BTYPE_FAMILY_MASK   0x0000ff00	/* PUTS Board Family Register */
#define MBX_BTYPE_SUBTYPE_MASK  0x000000ff	/* PUTS Board Subtype */

#define MBX_BTYPE_PLX9060       0x00000100	/* PLX family type */
#define MBX_BTYPE_PLX9080       0x00000300	/* PLX wanXL100s family type */

#define MBX_BTYPE_WANXL_4       0x00000104	/* wanXL400, 4-port */
#define MBX_BTYPE_WANXL_2       0x00000102	/* wanXL200, 2-port */
#define MBX_BTYPE_WANXL_1s      0x00000301	/* wanXL100s, 1-port */
#define MBX_BTYPE_WANXL_1t      0x00000401	/* wanXL100T1, 1-port */

/*****************************************/
/***    MAILBOX #3  -  SHMQ MAILBOX    ***/
/*****************************************/

#define MBX_SMBX_MASK           0x000000ff	/* PUTS SHMQ Mailbox bits */

/***************************************/
/***    GENERIC HOST-SIDE DRIVER     ***/
/***************************************/

#define MBX_ERR    0
#define MBX_OK     1

/* mailbox check routine - type of testing */
#define MBXCHK_STS      0x00	/* check for PUTS status */
#define MBXCHK_NOWAIT   0x01	/* dont care about PUTS status */

/* system allocates this many bytes for address mapping mailbox space */
#define MBX_ADDR_SPACE_360 0x80	/* wanXL100s/200/400 */
#define MBX_ADDR_MASK_360 (MBX_ADDR_SPACE_360-1)

static inline int plx9080_abort_dma(void __iomem *iobase, unsigned int channel)
{
	void __iomem *dma_cs_addr;
	uint8_t dma_status;
	const int timeout = 10000;
	unsigned int i;

	if (channel)
		dma_cs_addr = iobase + PLX_DMA1_CS_REG;
	else
		dma_cs_addr = iobase + PLX_DMA0_CS_REG;

	/*  abort dma transfer if necessary */
	dma_status = readb(dma_cs_addr);
	if ((dma_status & PLX_DMA_EN_BIT) == 0)
		return 0;

	/*  wait to make sure done bit is zero */
	for (i = 0; (dma_status & PLX_DMA_DONE_BIT) && i < timeout; i++) {
=======
/**
 * plx9080_abort_dma - Abort a PLX PCI 9080 DMA transfer
 * @iobase:	Remapped base address of configuration registers.
 * @channel:	DMA channel number (0 or 1).
 *
 * Aborts the DMA transfer on the channel, which must have been enabled
 * and started beforehand.
 *
 * Return:
 *	%0 on success.
 *	-%ETIMEDOUT if timed out waiting for abort to complete.
 */
static inline int plx9080_abort_dma(void __iomem *iobase, unsigned int channel)
{
	void __iomem *dma_cs_addr;
	u8 dma_status;
	const int timeout = 10000;
	unsigned int i;

	dma_cs_addr = iobase + PLX_REG_DMACSR(channel);

	/* abort dma transfer if necessary */
	dma_status = readb(dma_cs_addr);
	if ((dma_status & PLX_DMACSR_ENABLE) == 0)
		return 0;

	/* wait to make sure done bit is zero */
	for (i = 0; (dma_status & PLX_DMACSR_DONE) && i < timeout; i++) {
>>>>>>> v4.9.227
		udelay(1);
		dma_status = readb(dma_cs_addr);
	}
	if (i == timeout)
		return -ETIMEDOUT;

<<<<<<< HEAD
	/*  disable and abort channel */
	writeb(PLX_DMA_ABORT_BIT, dma_cs_addr);
	/*  wait for dma done bit */
	dma_status = readb(dma_cs_addr);
	for (i = 0; (dma_status & PLX_DMA_DONE_BIT) == 0 && i < timeout; i++) {
=======
	/* disable and abort channel */
	writeb(PLX_DMACSR_ABORT, dma_cs_addr);
	/* wait for dma done bit */
	dma_status = readb(dma_cs_addr);
	for (i = 0; (dma_status & PLX_DMACSR_DONE) == 0 && i < timeout; i++) {
>>>>>>> v4.9.227
		udelay(1);
		dma_status = readb(dma_cs_addr);
	}
	if (i == timeout)
		return -ETIMEDOUT;

	return 0;
}

#endif /* __COMEDI_PLX9080_H */
