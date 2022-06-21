/*
 * Generic Generic NCR5380 driver
 *	
 * Copyright 1993, Drew Eckhardt
 *	Visionary Computing
 *	(Unix and Linux consulting and custom programming)
 *	drew@colorado.edu
 *      +1 (303) 440-4894
 *
 * NCR53C400 extensions (c) 1994,1995,1996, Kevin Lentin
 *    K.Lentin@cs.monash.edu.au
 *
 * NCR53C400A extensions (c) 1996, Ingmar Baumgart
 *    ingmar@gonzo.schwaben.de
 *
 * DTC3181E extensions (c) 1997, Ronald van Cuijlenborg
 * ronald.van.cuijlenborg@tip.nl or nutty@dds.nl
 *
 * Added ISAPNP support for DTC436 adapters,
 * Thomas Sailer, sailer@ife.ee.ethz.ch
 *
<<<<<<< HEAD
 * ALPHA RELEASE 1. 
 *
 * For more information, please consult 
 *
 * NCR 5380 Family
 * SCSI Protocol Controller
 * Databook
 *
 * NCR Microelectronics
 * 1635 Aeroplaza Drive
 * Colorado Springs, CO 80916
 * 1+ (719) 578-3400
 * 1+ (800) 334-5454
 */

/* 
 * TODO : flesh out DMA support, find some one actually using this (I have
 * 	a memory mapped Trantor board that works fine)
 */

/*
 * Options :
 *
 * PARITY - enable parity checking.  Not supported.
 *
 * SCSI2 - enable support for SCSI-II tagged queueing.  Untested.
 *
 * USLEEP - enable support for devices that don't disconnect.  Untested.
 *
 * The card is detected and initialized in one of several ways : 
 * 1.  With command line overrides - NCR5380=port,irq may be 
 *     used on the LILO command line to override the defaults.
 *
 * 2.  With the GENERIC_NCR5380_OVERRIDE compile time define.  This is 
 *     specified as an array of address, irq, dma, board tuples.  Ie, for
 *     one board at 0x350, IRQ5, no dma, I could say  
 *     -DGENERIC_NCR5380_OVERRIDE={{0xcc000, 5, DMA_NONE, BOARD_NCR5380}}
 * 
 * -1 should be specified for no or DMA interrupt, -2 to autoprobe for an 
 * 	IRQ line if overridden on the command line.
 *
 * 3.  When included as a module, with arguments passed on the command line:
 *         ncr_irq=xx	the interrupt
 *         ncr_addr=xx  the port or base address (for port or memory
 *              	mapped, resp.)
 *         ncr_dma=xx	the DMA
 *         ncr_5380=1	to set up for a NCR5380 board
 *         ncr_53c400=1	to set up for a NCR53C400 board
 *     e.g.
 *     modprobe g_NCR5380 ncr_irq=5 ncr_addr=0x350 ncr_5380=1
 *       for a port mapped NCR5380 board or
 *     modprobe g_NCR5380 ncr_irq=255 ncr_addr=0xc8000 ncr_53c400=1
 *       for a memory mapped NCR53C400 board with interrupts disabled.
 * 
 * 255 should be specified for no or DMA interrupt, 254 to autoprobe for an 
 * 	IRQ line if overridden on the command line.
 *     
 */

/* settings for DTC3181E card with only Mustek scanner attached */
#define USLEEP
#define USLEEP_POLL	1
#define USLEEP_SLEEP	20
#define USLEEP_WAITLONG	500

#define AUTOPROBE_IRQ
#define AUTOSENSE


#ifdef CONFIG_SCSI_GENERIC_NCR53C400
#define NCR53C400_PSEUDO_DMA 1
#define PSEUDO_DMA
#define NCR53C400
#define NCR5380_STATS
#undef NCR5380_STAT_LIMIT
#endif

#include <asm/io.h>
#include <linux/signal.h>
#include <linux/blkdev.h>
#include "scsi.h"
#include <scsi/scsi_host.h>
#include "g_NCR5380.h"
#include "NCR5380.h"
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/isapnp.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

#define NCR_NOT_SET 0
static int ncr_irq = NCR_NOT_SET;
static int ncr_dma = NCR_NOT_SET;
static int ncr_addr = NCR_NOT_SET;
static int ncr_5380 = NCR_NOT_SET;
static int ncr_53c400 = NCR_NOT_SET;
static int ncr_53c400a = NCR_NOT_SET;
static int dtc_3181e = NCR_NOT_SET;

static struct override {
	NCR5380_map_type NCR5380_map_name;
	int irq;
	int dma;
	int board;		/* Use NCR53c400, Ricoh, etc. extensions ? */
} overrides
#ifdef GENERIC_NCR5380_OVERRIDE
[] __initdata = GENERIC_NCR5380_OVERRIDE;
#else
[1] __initdata = { { 0,},};
#endif

#define NO_OVERRIDES ARRAY_SIZE(overrides)

#ifndef MODULE

/**
 *	internal_setup		-	handle lilo command string override
 *	@board:	BOARD_* identifier for the board
 *	@str: unused
 *	@ints: numeric parameters
 *
 * 	Do LILO command line initialization of the overrides array. Display
 *	errors when needed
 *
 *	Locks: none
 */

static void __init internal_setup(int board, char *str, int *ints)
{
	static int commandline_current = 0;
	switch (board) {
	case BOARD_NCR5380:
		if (ints[0] != 2 && ints[0] != 3) {
			printk(KERN_ERR "generic_NCR5380_setup : usage ncr5380=" STRVAL(NCR5380_map_name) ",irq,dma\n");
			return;
		}
		break;
	case BOARD_NCR53C400:
		if (ints[0] != 2) {
			printk(KERN_ERR "generic_NCR53C400_setup : usage ncr53c400=" STRVAL(NCR5380_map_name) ",irq\n");
			return;
		}
		break;
	case BOARD_NCR53C400A:
		if (ints[0] != 2) {
			printk(KERN_ERR "generic_NCR53C400A_setup : usage ncr53c400a=" STRVAL(NCR5380_map_name) ",irq\n");
			return;
		}
		break;
	case BOARD_DTC3181E:
		if (ints[0] != 2) {
			printk("generic_DTC3181E_setup : usage dtc3181e=" STRVAL(NCR5380_map_name) ",irq\n");
			return;
		}
		break;
	}

	if (commandline_current < NO_OVERRIDES) {
		overrides[commandline_current].NCR5380_map_name = (NCR5380_map_type) ints[1];
		overrides[commandline_current].irq = ints[2];
		if (ints[0] == 3)
			overrides[commandline_current].dma = ints[3];
		else
			overrides[commandline_current].dma = DMA_NONE;
		overrides[commandline_current].board = board;
		++commandline_current;
	}
}


/**
 * 	do_NCR53C80_setup		-	set up entry point
 *	@str: unused
 *
 *	Setup function invoked at boot to parse the ncr5380= command
 *	line.
 */

static int __init do_NCR5380_setup(char *str)
{
	int ints[10];

	get_options(str, ARRAY_SIZE(ints), ints);
	internal_setup(BOARD_NCR5380, str, ints);
	return 1;
}

/**
 * 	do_NCR53C400_setup		-	set up entry point
 *	@str: unused
 *	@ints: integer parameters from kernel setup code
 *
 *	Setup function invoked at boot to parse the ncr53c400= command
 *	line.
 */

static int __init do_NCR53C400_setup(char *str)
{
	int ints[10];

	get_options(str, ARRAY_SIZE(ints), ints);
	internal_setup(BOARD_NCR53C400, str, ints);
	return 1;
}

/**
 * 	do_NCR53C400A_setup	-	set up entry point
 *	@str: unused
 *	@ints: integer parameters from kernel setup code
 *
 *	Setup function invoked at boot to parse the ncr53c400a= command
 *	line.
 */

static int __init do_NCR53C400A_setup(char *str)
{
	int ints[10];

	get_options(str, ARRAY_SIZE(ints), ints);
	internal_setup(BOARD_NCR53C400A, str, ints);
	return 1;
}

/**
 * 	do_DTC3181E_setup	-	set up entry point
 *	@str: unused
 *	@ints: integer parameters from kernel setup code
 *
 *	Setup function invoked at boot to parse the dtc3181e= command
 *	line.
 */

static int __init do_DTC3181E_setup(char *str)
{
	int ints[10];

	get_options(str, ARRAY_SIZE(ints), ints);
	internal_setup(BOARD_DTC3181E, str, ints);
	return 1;
}

#endif

/**
 * 	generic_NCR5380_detect	-	look for NCR5380 controllers
 *	@tpnt: the scsi template
 *
 *	Scan for the present of NCR5380, NCR53C400, NCR53C400A, DTC3181E
 *	and DTC436(ISAPnP) controllers. If overrides have been set we use
 *	them.
 *
 *	The caller supplied NCR5380_init function is invoked from here, before
 *	the interrupt line is taken.
 *
 *	Locks: none
 */

int __init generic_NCR5380_detect(struct scsi_host_template * tpnt)
{
	static int current_override = 0;
	int count;
	unsigned int *ports;
#ifndef SCSI_G_NCR5380_MEM
	int i;
	unsigned long region_size = 16;
#endif
	static unsigned int __initdata ncr_53c400a_ports[] = {
		0x280, 0x290, 0x300, 0x310, 0x330, 0x340, 0x348, 0x350, 0
	};
	static unsigned int __initdata dtc_3181e_ports[] = {
		0x220, 0x240, 0x280, 0x2a0, 0x2c0, 0x300, 0x320, 0x340, 0
	};
	int flags = 0;
	struct Scsi_Host *instance;
#ifdef SCSI_G_NCR5380_MEM
	unsigned long base;
	void __iomem *iomem;
#endif

	if (ncr_irq != NCR_NOT_SET)
		overrides[0].irq = ncr_irq;
	if (ncr_dma != NCR_NOT_SET)
		overrides[0].dma = ncr_dma;
	if (ncr_addr != NCR_NOT_SET)
		overrides[0].NCR5380_map_name = (NCR5380_map_type) ncr_addr;
	if (ncr_5380 != NCR_NOT_SET)
		overrides[0].board = BOARD_NCR5380;
	else if (ncr_53c400 != NCR_NOT_SET)
		overrides[0].board = BOARD_NCR53C400;
	else if (ncr_53c400a != NCR_NOT_SET)
		overrides[0].board = BOARD_NCR53C400A;
	else if (dtc_3181e != NCR_NOT_SET)
		overrides[0].board = BOARD_DTC3181E;
#ifndef SCSI_G_NCR5380_MEM
	if (!current_override && isapnp_present()) {
		struct pnp_dev *dev = NULL;
		count = 0;
		while ((dev = pnp_find_dev(NULL, ISAPNP_VENDOR('D', 'T', 'C'), ISAPNP_FUNCTION(0x436e), dev))) {
			if (count >= NO_OVERRIDES)
				break;
			if (pnp_device_attach(dev) < 0)
				continue;
			if (pnp_activate_dev(dev) < 0) {
				printk(KERN_ERR "dtc436e probe: activate failed\n");
				pnp_device_detach(dev);
				continue;
			}
			if (!pnp_port_valid(dev, 0)) {
				printk(KERN_ERR "dtc436e probe: no valid port\n");
				pnp_device_detach(dev);
				continue;
			}
			if (pnp_irq_valid(dev, 0))
				overrides[count].irq = pnp_irq(dev, 0);
			else
				overrides[count].irq = SCSI_IRQ_NONE;
			if (pnp_dma_valid(dev, 0))
				overrides[count].dma = pnp_dma(dev, 0);
			else
				overrides[count].dma = DMA_NONE;
			overrides[count].NCR5380_map_name = (NCR5380_map_type) pnp_port_start(dev, 0);
			overrides[count].board = BOARD_DTC3181E;
			count++;
		}
	}
#endif
	tpnt->proc_name = "g_NCR5380";

	for (count = 0; current_override < NO_OVERRIDES; ++current_override) {
		if (!(overrides[current_override].NCR5380_map_name))
			continue;

		ports = NULL;
		switch (overrides[current_override].board) {
		case BOARD_NCR5380:
			flags = FLAG_NO_PSEUDO_DMA;
			break;
		case BOARD_NCR53C400:
			flags = FLAG_NCR53C400;
			break;
		case BOARD_NCR53C400A:
			flags = FLAG_NO_PSEUDO_DMA;
			ports = ncr_53c400a_ports;
			break;
		case BOARD_DTC3181E:
			flags = FLAG_NO_PSEUDO_DMA | FLAG_DTC3181E;
			ports = dtc_3181e_ports;
			break;
		}

#ifndef SCSI_G_NCR5380_MEM
		if (ports) {
			/* wakeup sequence for the NCR53C400A and DTC3181E */

			/* Disable the adapter and look for a free io port */
			outb(0x59, 0x779);
			outb(0xb9, 0x379);
			outb(0xc5, 0x379);
			outb(0xae, 0x379);
			outb(0xa6, 0x379);
			outb(0x00, 0x379);

			if (overrides[current_override].NCR5380_map_name != PORT_AUTO)
				for (i = 0; ports[i]; i++) {
					if (!request_region(ports[i],  16, "ncr53c80"))
						continue;
					if (overrides[current_override].NCR5380_map_name == ports[i])
						break;
					release_region(ports[i], 16);
			} else
				for (i = 0; ports[i]; i++) {
					if (!request_region(ports[i],  16, "ncr53c80"))
						continue;
					if (inb(ports[i]) == 0xff)
						break;
					release_region(ports[i], 16);
				}
			if (ports[i]) {
				/* At this point we have our region reserved */
				outb(0x59, 0x779);
				outb(0xb9, 0x379);
				outb(0xc5, 0x379);
				outb(0xae, 0x379);
				outb(0xa6, 0x379);
				outb(0x80 | i, 0x379);	/* set io port to be used */
				outb(0xc0, ports[i] + 9);
				if (inb(ports[i] + 9) != 0x80)
					continue;
				else
					overrides[current_override].NCR5380_map_name = ports[i];
			} else
				continue;
		}
		else
		{
			/* Not a 53C400A style setup - just grab */
			if(!(request_region(overrides[current_override].NCR5380_map_name, NCR5380_region_size, "ncr5380")))
				continue;
			region_size = NCR5380_region_size;
		}
#else
		base = overrides[current_override].NCR5380_map_name;
		if (!request_mem_region(base, NCR5380_region_size, "ncr5380"))
			continue;
		iomem = ioremap(base, NCR5380_region_size);
		if (!iomem) {
			release_mem_region(base, NCR5380_region_size);
			continue;
		}
#endif
		instance = scsi_register(tpnt, sizeof(struct NCR5380_hostdata));
		if (instance == NULL) {
#ifndef SCSI_G_NCR5380_MEM
			release_region(overrides[current_override].NCR5380_map_name, region_size);
#else
			iounmap(iomem);
			release_mem_region(base, NCR5380_region_size);
#endif
			continue;
		}

		instance->NCR5380_instance_name = overrides[current_override].NCR5380_map_name;
#ifndef SCSI_G_NCR5380_MEM
		instance->n_io_port = region_size;
#else
		((struct NCR5380_hostdata *)instance->hostdata)->iomem = iomem;
#endif

		NCR5380_init(instance, flags);

		if (overrides[current_override].irq != IRQ_AUTO)
			instance->irq = overrides[current_override].irq;
		else
			instance->irq = NCR5380_probe_irq(instance, 0xffff);

		if (instance->irq != SCSI_IRQ_NONE)
			if (request_irq(instance->irq, generic_NCR5380_intr,
					0, "NCR5380", instance)) {
				printk(KERN_WARNING "scsi%d : IRQ%d not free, interrupts disabled\n", instance->host_no, instance->irq);
				instance->irq = SCSI_IRQ_NONE;
			}

		if (instance->irq == SCSI_IRQ_NONE) {
			printk(KERN_INFO "scsi%d : interrupts not enabled. for better interactive performance,\n", instance->host_no);
			printk(KERN_INFO "scsi%d : please jumper the board for a free IRQ.\n", instance->host_no);
		}

		printk(KERN_INFO "scsi%d : at " STRVAL(NCR5380_map_name) " 0x%x", instance->host_no, (unsigned int) instance->NCR5380_instance_name);
		if (instance->irq == SCSI_IRQ_NONE)
			printk(" interrupts disabled");
		else
			printk(" irq %d", instance->irq);
		printk(" options CAN_QUEUE=%d  CMD_PER_LUN=%d release=%d", CAN_QUEUE, CMD_PER_LUN, GENERIC_NCR5380_PUBLIC_RELEASE);
		NCR5380_print_options(instance);
		printk("\n");

		++current_override;
		++count;
	}
	return count;
}

/**
 *	generic_NCR5380_info	-	reporting string
 *	@host: NCR5380 to report on
 *
 *	Report driver information for the NCR5380
 */
 	
const char *generic_NCR5380_info(struct Scsi_Host *host)
{
	static const char string[] = "Generic NCR5380/53C400 Driver";
	return string;
}

/**
 *	generic_NCR5380_release_resources	-	free resources
 *	@instance: host adapter to clean up 
 *
 *	Free the generic interface resources from this adapter.
 *
 *	Locks: none
 */
 
int generic_NCR5380_release_resources(struct Scsi_Host *instance)
{
	NCR5380_local_declare();
	NCR5380_setup(instance);
	
	if (instance->irq != SCSI_IRQ_NONE)
		free_irq(instance->irq, instance);
	NCR5380_exit(instance);

#ifndef SCSI_G_NCR5380_MEM
	release_region(instance->NCR5380_instance_name, instance->n_io_port);
#else
	iounmap(((struct NCR5380_hostdata *)instance->hostdata)->iomem);
	release_mem_region(instance->NCR5380_instance_name, NCR5380_region_size);
#endif


	return 0;
}

#ifdef BIOSPARAM
/**
 *	generic_NCR5380_biosparam
 *	@disk: disk to compute geometry for
 *	@dev: device identifier for this disk
 *	@ip: sizes to fill in
 *
 *	Generates a BIOS / DOS compatible H-C-S mapping for the specified 
 *	device / size.
 * 
 * 	XXX Most SCSI boards use this mapping, I could be incorrect.  Someone
 *	using hard disks on a trantor should verify that this mapping
 *	corresponds to that used by the BIOS / ASPI driver by running the linux
 *	fdisk program and matching the H_C_S coordinates to what DOS uses.
 *
 *	Locks: none
 */

static int
generic_NCR5380_biosparam(struct scsi_device *sdev, struct block_device *bdev,
			  sector_t capacity, int *ip)
{
	ip[0] = 64;
	ip[1] = 32;
	ip[2] = capacity >> 11;
	return 0;
}
#endif

#ifdef NCR53C400_PSEUDO_DMA

/**
 *	NCR5380_pread		-	pseudo DMA read
=======
 * See Documentation/scsi/g_NCR5380.txt for more info.
 */

#include <asm/io.h>
#include <linux/blkdev.h>
#include <linux/module.h>
#include <scsi/scsi_host.h>
#include "g_NCR5380.h"
#include "NCR5380.h"
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/isa.h>
#include <linux/pnp.h>
#include <linux/interrupt.h>

#define MAX_CARDS 8

/* old-style parameters for compatibility */
static int ncr_irq;
static int ncr_addr;
static int ncr_5380;
static int ncr_53c400;
static int ncr_53c400a;
static int dtc_3181e;
static int hp_c2502;
module_param(ncr_irq, int, 0);
module_param(ncr_addr, int, 0);
module_param(ncr_5380, int, 0);
module_param(ncr_53c400, int, 0);
module_param(ncr_53c400a, int, 0);
module_param(dtc_3181e, int, 0);
module_param(hp_c2502, int, 0);

static int irq[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
module_param_array(irq, int, NULL, 0);
MODULE_PARM_DESC(irq, "IRQ number(s)");

static int base[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
module_param_array(base, int, NULL, 0);
MODULE_PARM_DESC(base, "base address(es)");

static int card[] = { -1, -1, -1, -1, -1, -1, -1, -1 };
module_param_array(card, int, NULL, 0);
MODULE_PARM_DESC(card, "card type (0=NCR5380, 1=NCR53C400, 2=NCR53C400A, 3=DTC3181E, 4=HP C2502)");

MODULE_LICENSE("GPL");

#ifndef SCSI_G_NCR5380_MEM
/*
 * Configure I/O address of 53C400A or DTC436 by writing magic numbers
 * to ports 0x779 and 0x379.
 */
static void magic_configure(int idx, u8 irq, u8 magic[])
{
	u8 cfg = 0;

	outb(magic[0], 0x779);
	outb(magic[1], 0x379);
	outb(magic[2], 0x379);
	outb(magic[3], 0x379);
	outb(magic[4], 0x379);

	/* allowed IRQs for HP C2502 */
	if (irq != 2 && irq != 3 && irq != 4 && irq != 5 && irq != 7)
		irq = 0;
	if (idx >= 0 && idx <= 7)
		cfg = 0x80 | idx | (irq << 4);
	outb(cfg, 0x379);
}
#endif

static int generic_NCR5380_init_one(struct scsi_host_template *tpnt,
			struct device *pdev, int base, int irq, int board)
{
	unsigned int *ports;
	u8 *magic = NULL;
#ifndef SCSI_G_NCR5380_MEM
	int i;
	int port_idx = -1;
	unsigned long region_size;
#endif
	static unsigned int ncr_53c400a_ports[] = {
		0x280, 0x290, 0x300, 0x310, 0x330, 0x340, 0x348, 0x350, 0
	};
	static unsigned int dtc_3181e_ports[] = {
		0x220, 0x240, 0x280, 0x2a0, 0x2c0, 0x300, 0x320, 0x340, 0
	};
	static u8 ncr_53c400a_magic[] = {	/* 53C400A & DTC436 */
		0x59, 0xb9, 0xc5, 0xae, 0xa6
	};
	static u8 hp_c2502_magic[] = {	/* HP C2502 */
		0x0f, 0x22, 0xf0, 0x20, 0x80
	};
	int flags, ret;
	struct Scsi_Host *instance;
	struct NCR5380_hostdata *hostdata;
#ifdef SCSI_G_NCR5380_MEM
	void __iomem *iomem;
	resource_size_t iomem_size;
#endif

	ports = NULL;
	flags = 0;
	switch (board) {
	case BOARD_NCR5380:
		flags = FLAG_NO_PSEUDO_DMA | FLAG_DMA_FIXUP;
		break;
	case BOARD_NCR53C400A:
		ports = ncr_53c400a_ports;
		magic = ncr_53c400a_magic;
		break;
	case BOARD_HP_C2502:
		ports = ncr_53c400a_ports;
		magic = hp_c2502_magic;
		break;
	case BOARD_DTC3181E:
		ports = dtc_3181e_ports;
		magic = ncr_53c400a_magic;
		break;
	}

#ifndef SCSI_G_NCR5380_MEM
	if (ports && magic) {
		/* wakeup sequence for the NCR53C400A and DTC3181E */

		/* Disable the adapter and look for a free io port */
		magic_configure(-1, 0, magic);

		region_size = 16;
		if (base)
			for (i = 0; ports[i]; i++) {
				if (base == ports[i]) {	/* index found */
					if (!request_region(ports[i],
							    region_size,
							    "ncr53c80"))
						return -EBUSY;
					break;
				}
			}
		else
			for (i = 0; ports[i]; i++) {
				if (!request_region(ports[i], region_size,
						    "ncr53c80"))
					continue;
				if (inb(ports[i]) == 0xff)
					break;
				release_region(ports[i], region_size);
			}
		if (ports[i]) {
			/* At this point we have our region reserved */
			magic_configure(i, 0, magic); /* no IRQ yet */
			base = ports[i];
			outb(0xc0, base + 9);
			if (inb(base + 9) != 0x80) {
				ret = -ENODEV;
				goto out_release;
			}
			port_idx = i;
		} else
			return -EINVAL;
	}
	else
	{
		/* NCR5380 - no configuration, just grab */
		region_size = 8;
		if (!base || !request_region(base, region_size, "ncr5380"))
			return -EBUSY;
	}
#else
	iomem_size = NCR53C400_region_size;
	if (!request_mem_region(base, iomem_size, "ncr5380"))
		return -EBUSY;
	iomem = ioremap(base, iomem_size);
	if (!iomem) {
		release_mem_region(base, iomem_size);
		return -ENOMEM;
	}
#endif
	instance = scsi_host_alloc(tpnt, sizeof(struct NCR5380_hostdata));
	if (instance == NULL) {
		ret = -ENOMEM;
		goto out_release;
	}
	hostdata = shost_priv(instance);

#ifndef SCSI_G_NCR5380_MEM
	instance->io_port = base;
	instance->n_io_port = region_size;
	hostdata->io_width = 1; /* 8-bit PDMA by default */

	/*
	 * On NCR53C400 boards, NCR5380 registers are mapped 8 past
	 * the base address.
	 */
	switch (board) {
	case BOARD_NCR53C400:
		instance->io_port += 8;
		hostdata->c400_ctl_status = 0;
		hostdata->c400_blk_cnt = 1;
		hostdata->c400_host_buf = 4;
		break;
	case BOARD_DTC3181E:
		hostdata->io_width = 2;	/* 16-bit PDMA */
		/* fall through */
	case BOARD_NCR53C400A:
	case BOARD_HP_C2502:
		hostdata->c400_ctl_status = 9;
		hostdata->c400_blk_cnt = 10;
		hostdata->c400_host_buf = 8;
		break;
	}
#else
	instance->base = base;
	hostdata->iomem = iomem;
	hostdata->iomem_size = iomem_size;
	switch (board) {
	case BOARD_NCR53C400:
		hostdata->c400_ctl_status = 0x100;
		hostdata->c400_blk_cnt = 0x101;
		hostdata->c400_host_buf = 0x104;
		break;
	case BOARD_DTC3181E:
	case BOARD_NCR53C400A:
	case BOARD_HP_C2502:
		pr_err(DRV_MODULE_NAME ": unknown register offsets\n");
		ret = -EINVAL;
		goto out_unregister;
	}
#endif

	ret = NCR5380_init(instance, flags | FLAG_LATE_DMA_SETUP);
	if (ret)
		goto out_unregister;

	switch (board) {
	case BOARD_NCR53C400:
	case BOARD_DTC3181E:
	case BOARD_NCR53C400A:
	case BOARD_HP_C2502:
		NCR5380_write(hostdata->c400_ctl_status, CSR_BASE);
	}

	NCR5380_maybe_reset_bus(instance);

	if (irq != IRQ_AUTO)
		instance->irq = irq;
	else
		instance->irq = NCR5380_probe_irq(instance, 0xffff);

	/* Compatibility with documented NCR5380 kernel parameters */
	if (instance->irq == 255)
		instance->irq = NO_IRQ;

	if (instance->irq != NO_IRQ) {
#ifndef SCSI_G_NCR5380_MEM
		/* set IRQ for HP C2502 */
		if (board == BOARD_HP_C2502)
			magic_configure(port_idx, instance->irq, magic);
#endif
		if (request_irq(instance->irq, generic_NCR5380_intr,
				0, "NCR5380", instance)) {
			printk(KERN_WARNING "scsi%d : IRQ%d not free, interrupts disabled\n", instance->host_no, instance->irq);
			instance->irq = NO_IRQ;
		}
	}

	if (instance->irq == NO_IRQ) {
		printk(KERN_INFO "scsi%d : interrupts not enabled. for better interactive performance,\n", instance->host_no);
		printk(KERN_INFO "scsi%d : please jumper the board for a free IRQ.\n", instance->host_no);
	}

	ret = scsi_add_host(instance, pdev);
	if (ret)
		goto out_free_irq;
	scsi_scan_host(instance);
	dev_set_drvdata(pdev, instance);
	return 0;

out_free_irq:
	if (instance->irq != NO_IRQ)
		free_irq(instance->irq, instance);
	NCR5380_exit(instance);
out_unregister:
	scsi_host_put(instance);
out_release:
#ifndef SCSI_G_NCR5380_MEM
	release_region(base, region_size);
#else
	iounmap(iomem);
	release_mem_region(base, iomem_size);
#endif
	return ret;
}

static void generic_NCR5380_release_resources(struct Scsi_Host *instance)
{
	scsi_remove_host(instance);
	if (instance->irq != NO_IRQ)
		free_irq(instance->irq, instance);
	NCR5380_exit(instance);
#ifndef SCSI_G_NCR5380_MEM
	release_region(instance->io_port, instance->n_io_port);
#else
	{
		struct NCR5380_hostdata *hostdata = shost_priv(instance);

		iounmap(hostdata->iomem);
		release_mem_region(instance->base, hostdata->iomem_size);
	}
#endif
	scsi_host_put(instance);
}

/**
 *	generic_NCR5380_pread - pseudo DMA read
>>>>>>> v4.9.227
 *	@instance: adapter to read from
 *	@dst: buffer to read into
 *	@len: buffer length
 *
 *	Perform a pseudo DMA mode read from an NCR53C400 or equivalent
 *	controller
 */
 
<<<<<<< HEAD
static inline int NCR5380_pread(struct Scsi_Host *instance, unsigned char *dst, int len)
{
	int blocks = len / 128;
	int start = 0;
	int bl;

	NCR5380_local_declare();
	NCR5380_setup(instance);

	NCR5380_write(C400_CONTROL_STATUS_REG, CSR_BASE | CSR_TRANS_DIR);
	NCR5380_write(C400_BLOCK_COUNTER_REG, blocks);
	while (1) {
		if ((bl = NCR5380_read(C400_BLOCK_COUNTER_REG)) == 0) {
			break;
		}
		if (NCR5380_read(C400_CONTROL_STATUS_REG) & CSR_GATED_53C80_IRQ) {
			printk(KERN_ERR "53C400r: Got 53C80_IRQ start=%d, blocks=%d\n", start, blocks);
			return -1;
		}
		while (NCR5380_read(C400_CONTROL_STATUS_REG) & CSR_HOST_BUF_NOT_RDY);

#ifndef SCSI_G_NCR5380_MEM
		{
			int i;
			for (i = 0; i < 128; i++)
				dst[start + i] = NCR5380_read(C400_HOST_BUFFER);
		}
#else
		/* implies SCSI_G_NCR5380_MEM */
		memcpy_fromio(dst + start, iomem + NCR53C400_host_buffer, 128);
=======
static inline int generic_NCR5380_pread(struct Scsi_Host *instance,
                                        unsigned char *dst, int len)
{
	struct NCR5380_hostdata *hostdata = shost_priv(instance);
	int blocks = len / 128;
	int start = 0;

	NCR5380_write(hostdata->c400_ctl_status, CSR_BASE | CSR_TRANS_DIR);
	NCR5380_write(hostdata->c400_blk_cnt, blocks);
	while (1) {
		if (NCR5380_read(hostdata->c400_blk_cnt) == 0)
			break;
		if (NCR5380_read(hostdata->c400_ctl_status) & CSR_GATED_53C80_IRQ) {
			printk(KERN_ERR "53C400r: Got 53C80_IRQ start=%d, blocks=%d\n", start, blocks);
			return -1;
		}
		while (NCR5380_read(hostdata->c400_ctl_status) & CSR_HOST_BUF_NOT_RDY)
			; /* FIXME - no timeout */

#ifndef SCSI_G_NCR5380_MEM
		if (hostdata->io_width == 2)
			insw(instance->io_port + hostdata->c400_host_buf,
							dst + start, 64);
		else
			insb(instance->io_port + hostdata->c400_host_buf,
							dst + start, 128);
#else
		/* implies SCSI_G_NCR5380_MEM */
		memcpy_fromio(dst + start,
		              hostdata->iomem + NCR53C400_host_buffer, 128);
>>>>>>> v4.9.227
#endif
		start += 128;
		blocks--;
	}

	if (blocks) {
<<<<<<< HEAD
		while (NCR5380_read(C400_CONTROL_STATUS_REG) & CSR_HOST_BUF_NOT_RDY)
		{
			// FIXME - no timeout
		}

#ifndef SCSI_G_NCR5380_MEM
		{
			int i;	
			for (i = 0; i < 128; i++)
				dst[start + i] = NCR5380_read(C400_HOST_BUFFER);
		}
#else
		/* implies SCSI_G_NCR5380_MEM */
		memcpy_fromio(dst + start, iomem + NCR53C400_host_buffer, 128);
=======
		while (NCR5380_read(hostdata->c400_ctl_status) & CSR_HOST_BUF_NOT_RDY)
			; /* FIXME - no timeout */

#ifndef SCSI_G_NCR5380_MEM
		if (hostdata->io_width == 2)
			insw(instance->io_port + hostdata->c400_host_buf,
							dst + start, 64);
		else
			insb(instance->io_port + hostdata->c400_host_buf,
							dst + start, 128);
#else
		/* implies SCSI_G_NCR5380_MEM */
		memcpy_fromio(dst + start,
		              hostdata->iomem + NCR53C400_host_buffer, 128);
>>>>>>> v4.9.227
#endif
		start += 128;
		blocks--;
	}

<<<<<<< HEAD
	if (!(NCR5380_read(C400_CONTROL_STATUS_REG) & CSR_GATED_53C80_IRQ))
		printk("53C400r: no 53C80 gated irq after transfer");

#if 0
	/*
	 *	DON'T DO THIS - THEY NEVER ARRIVE!
	 */
	printk("53C400r: Waiting for 53C80 registers\n");
	while (NCR5380_read(C400_CONTROL_STATUS_REG) & CSR_53C80_REG)
		;
#endif
	if (!(NCR5380_read(BUS_AND_STATUS_REG) & BASR_END_DMA_TRANSFER))
		printk(KERN_ERR "53C400r: no end dma signal\n");
		
	NCR5380_write(MODE_REG, MR_BASE);
	NCR5380_read(RESET_PARITY_INTERRUPT_REG);
=======
	if (!(NCR5380_read(hostdata->c400_ctl_status) & CSR_GATED_53C80_IRQ))
		printk("53C400r: no 53C80 gated irq after transfer");

	/* wait for 53C80 registers to be available */
	while (!(NCR5380_read(hostdata->c400_ctl_status) & CSR_53C80_REG))
		;

	if (!(NCR5380_read(BUS_AND_STATUS_REG) & BASR_END_DMA_TRANSFER))
		printk(KERN_ERR "53C400r: no end dma signal\n");
		
>>>>>>> v4.9.227
	return 0;
}

/**
<<<<<<< HEAD
 *	NCR5380_write		-	pseudo DMA write
=======
 *	generic_NCR5380_pwrite - pseudo DMA write
>>>>>>> v4.9.227
 *	@instance: adapter to read from
 *	@dst: buffer to read into
 *	@len: buffer length
 *
 *	Perform a pseudo DMA mode read from an NCR53C400 or equivalent
 *	controller
 */

<<<<<<< HEAD
static inline int NCR5380_pwrite(struct Scsi_Host *instance, unsigned char *src, int len)
{
	int blocks = len / 128;
	int start = 0;
	int bl;
	int i;

	NCR5380_local_declare();
	NCR5380_setup(instance);

	NCR5380_write(C400_CONTROL_STATUS_REG, CSR_BASE);
	NCR5380_write(C400_BLOCK_COUNTER_REG, blocks);
	while (1) {
		if (NCR5380_read(C400_CONTROL_STATUS_REG) & CSR_GATED_53C80_IRQ) {
=======
static inline int generic_NCR5380_pwrite(struct Scsi_Host *instance,
                                         unsigned char *src, int len)
{
	struct NCR5380_hostdata *hostdata = shost_priv(instance);
	int blocks = len / 128;
	int start = 0;

	NCR5380_write(hostdata->c400_ctl_status, CSR_BASE);
	NCR5380_write(hostdata->c400_blk_cnt, blocks);
	while (1) {
		if (NCR5380_read(hostdata->c400_ctl_status) & CSR_GATED_53C80_IRQ) {
>>>>>>> v4.9.227
			printk(KERN_ERR "53C400w: Got 53C80_IRQ start=%d, blocks=%d\n", start, blocks);
			return -1;
		}

<<<<<<< HEAD
		if ((bl = NCR5380_read(C400_BLOCK_COUNTER_REG)) == 0) {
			break;
		}
		while (NCR5380_read(C400_CONTROL_STATUS_REG) & CSR_HOST_BUF_NOT_RDY)
			; // FIXME - timeout
#ifndef SCSI_G_NCR5380_MEM
		{
			for (i = 0; i < 128; i++)
				NCR5380_write(C400_HOST_BUFFER, src[start + i]);
		}
#else
		/* implies SCSI_G_NCR5380_MEM */
		memcpy_toio(iomem + NCR53C400_host_buffer, src + start, 128);
=======
		if (NCR5380_read(hostdata->c400_blk_cnt) == 0)
			break;
		while (NCR5380_read(hostdata->c400_ctl_status) & CSR_HOST_BUF_NOT_RDY)
			; // FIXME - timeout
#ifndef SCSI_G_NCR5380_MEM
		if (hostdata->io_width == 2)
			outsw(instance->io_port + hostdata->c400_host_buf,
							src + start, 64);
		else
			outsb(instance->io_port + hostdata->c400_host_buf,
							src + start, 128);
#else
		/* implies SCSI_G_NCR5380_MEM */
		memcpy_toio(hostdata->iomem + NCR53C400_host_buffer,
		            src + start, 128);
>>>>>>> v4.9.227
#endif
		start += 128;
		blocks--;
	}
	if (blocks) {
<<<<<<< HEAD
		while (NCR5380_read(C400_CONTROL_STATUS_REG) & CSR_HOST_BUF_NOT_RDY)
			; // FIXME - no timeout

#ifndef SCSI_G_NCR5380_MEM
		{
			for (i = 0; i < 128; i++)
				NCR5380_write(C400_HOST_BUFFER, src[start + i]);
		}
#else
		/* implies SCSI_G_NCR5380_MEM */
		memcpy_toio(iomem + NCR53C400_host_buffer, src + start, 128);
=======
		while (NCR5380_read(hostdata->c400_ctl_status) & CSR_HOST_BUF_NOT_RDY)
			; // FIXME - no timeout

#ifndef SCSI_G_NCR5380_MEM
		if (hostdata->io_width == 2)
			outsw(instance->io_port + hostdata->c400_host_buf,
							src + start, 64);
		else
			outsb(instance->io_port + hostdata->c400_host_buf,
							src + start, 128);
#else
		/* implies SCSI_G_NCR5380_MEM */
		memcpy_toio(hostdata->iomem + NCR53C400_host_buffer,
		            src + start, 128);
>>>>>>> v4.9.227
#endif
		start += 128;
		blocks--;
	}

<<<<<<< HEAD
#if 0
	printk("53C400w: waiting for registers to be available\n");
	THEY NEVER DO ! while (NCR5380_read(C400_CONTROL_STATUS_REG) & CSR_53C80_REG);
	printk("53C400w: Got em\n");
#endif

	/* Let's wait for this instead - could be ugly */
	/* All documentation says to check for this. Maybe my hardware is too
	 * fast. Waiting for it seems to work fine! KLL
	 */
	while (!(i = NCR5380_read(C400_CONTROL_STATUS_REG) & CSR_GATED_53C80_IRQ))
		;	// FIXME - no timeout

	/*
	 * I know. i is certainly != 0 here but the loop is new. See previous
	 * comment.
	 */
	if (i) {
		if (!((i = NCR5380_read(BUS_AND_STATUS_REG)) & BASR_END_DMA_TRANSFER))
			printk(KERN_ERR "53C400w: No END OF DMA bit - WHOOPS! BASR=%0x\n", i);
	} else
		printk(KERN_ERR "53C400w: no 53C80 gated irq after transfer (last block)\n");

#if 0
	if (!(NCR5380_read(BUS_AND_STATUS_REG) & BASR_END_DMA_TRANSFER)) {
		printk(KERN_ERR "53C400w: no end dma signal\n");
	}
#endif
=======
	/* wait for 53C80 registers to be available */
	while (!(NCR5380_read(hostdata->c400_ctl_status) & CSR_53C80_REG)) {
		udelay(4); /* DTC436 chip hangs without this */
		/* FIXME - no timeout */
	}

	if (!(NCR5380_read(BUS_AND_STATUS_REG) & BASR_END_DMA_TRANSFER)) {
		printk(KERN_ERR "53C400w: no end dma signal\n");
	}

>>>>>>> v4.9.227
	while (!(NCR5380_read(TARGET_COMMAND_REG) & TCR_LAST_BYTE_SENT))
		; 	// TIMEOUT
	return 0;
}
<<<<<<< HEAD
#endif				/* PSEUDO_DMA */
=======

static int generic_NCR5380_dma_xfer_len(struct Scsi_Host *instance,
                                        struct scsi_cmnd *cmd)
{
	struct NCR5380_hostdata *hostdata = shost_priv(instance);
	int transfersize = cmd->transfersize;

	if (hostdata->flags & FLAG_NO_PSEUDO_DMA)
		return 0;

	/* Limit transfers to 32K, for xx400 & xx406
	 * pseudoDMA that transfers in 128 bytes blocks.
	 */
	if (transfersize > 32 * 1024 && cmd->SCp.this_residual &&
	    !(cmd->SCp.this_residual % transfersize))
		transfersize = 32 * 1024;

	/* 53C400 datasheet: non-modulo-128-byte transfers should use PIO */
	if (transfersize % 128)
		transfersize = 0;

	return transfersize;
}
>>>>>>> v4.9.227

/*
 *	Include the NCR5380 core code that we build our driver around	
 */
 
#include "NCR5380.c"

<<<<<<< HEAD
#define PRINTP(x) seq_printf(m, x)
#define ANDP ,

static void sprint_opcode(struct seq_file *m, int opcode)
{
	PRINTP("0x%02x " ANDP opcode);
}

static void sprint_command(struct seq_file *m, unsigned char *command)
{
	int i, s;
	sprint_opcode(m, command[0]);
	for (i = 1, s = COMMAND_SIZE(command[0]); i < s; ++i)
		PRINTP("%02x " ANDP command[i]);
	PRINTP("\n");
}

/**
 *	sprintf_Scsi_Cmnd	-	print a scsi command
 *	@m: seq_fil to print into
 *	@cmd: SCSI command block
 *	
 *	Print out the target and command data in hex
 */

static void sprint_Scsi_Cmnd(struct seq_file *m, Scsi_Cmnd * cmd)
{
	PRINTP("host number %d destination target %d, lun %llu\n" ANDP cmd->device->host->host_no ANDP cmd->device->id ANDP cmd->device->lun);
	PRINTP("        command = ");
	sprint_command(m, cmd->cmnd);
}

/**
 *	generic_NCR5380_proc_info	-	/proc for NCR5380 driver
 *	@buffer: buffer to print into
 *	@start: start position
 *	@offset: offset into buffer
 *	@len: length
 *	@hostno: instance to affect
 *	@inout: read/write
 *
 *	Provide the procfs information for the 5380 controller. We fill
 *	this with useful debugging information including the commands
 *	being executed, disconnected command queue and the statistical
 *	data
 *
 *	Locks: global cli/lock for queue walk
 */
 
static int generic_NCR5380_show_info(struct seq_file *m, struct Scsi_Host *scsi_ptr)
{
	NCR5380_local_declare();
	unsigned long flags;
	unsigned char status;
	int i;
	Scsi_Cmnd *ptr;
	struct NCR5380_hostdata *hostdata;
#ifdef NCR5380_STATS
	struct scsi_device *dev;
#endif

	NCR5380_setup(scsi_ptr);
	hostdata = (struct NCR5380_hostdata *) scsi_ptr->hostdata;

	spin_lock_irqsave(scsi_ptr->host_lock, flags);
	PRINTP("SCSI host number %d : %s\n" ANDP scsi_ptr->host_no ANDP scsi_ptr->hostt->name);
	PRINTP("Generic NCR5380 driver version %d\n" ANDP GENERIC_NCR5380_PUBLIC_RELEASE);
	PRINTP("NCR5380 core version %d\n" ANDP NCR5380_PUBLIC_RELEASE);
#ifdef NCR53C400
	PRINTP("NCR53C400 extension version %d\n" ANDP NCR53C400_PUBLIC_RELEASE);
	PRINTP("NCR53C400 card%s detected\n" ANDP(((struct NCR5380_hostdata *) scsi_ptr->hostdata)->flags & FLAG_NCR53C400) ? "" : " not");
# if NCR53C400_PSEUDO_DMA
	PRINTP("NCR53C400 pseudo DMA used\n");
# endif
#else
	PRINTP("NO NCR53C400 driver extensions\n");
#endif
	PRINTP("Using %s mapping at %s 0x%lx, " ANDP STRVAL(NCR5380_map_config) ANDP STRVAL(NCR5380_map_name) ANDP scsi_ptr->NCR5380_instance_name);
	if (scsi_ptr->irq == SCSI_IRQ_NONE)
		PRINTP("no interrupt\n");
	else
		PRINTP("on interrupt %d\n" ANDP scsi_ptr->irq);

#ifdef NCR5380_STATS
	if (hostdata->connected || hostdata->issue_queue || hostdata->disconnected_queue)
		PRINTP("There are commands pending, transfer rates may be crud\n");
	if (hostdata->pendingr)
		PRINTP("  %d pending reads" ANDP hostdata->pendingr);
	if (hostdata->pendingw)
		PRINTP("  %d pending writes" ANDP hostdata->pendingw);
	if (hostdata->pendingr || hostdata->pendingw)
		PRINTP("\n");
	shost_for_each_device(dev, scsi_ptr) {
		unsigned long br = hostdata->bytes_read[dev->id];
		unsigned long bw = hostdata->bytes_write[dev->id];
		long tr = hostdata->time_read[dev->id] / HZ;
		long tw = hostdata->time_write[dev->id] / HZ;

		PRINTP("  T:%d %s " ANDP dev->id ANDP scsi_device_type(dev->type));
		for (i = 0; i < 8; i++)
			if (dev->vendor[i] >= 0x20)
				seq_putc(m, dev->vendor[i]);
		seq_putc(m, ' ');
		for (i = 0; i < 16; i++)
			if (dev->model[i] >= 0x20)
				seq_putc(m, dev->model[i]);
		seq_putc(m, ' ');
		for (i = 0; i < 4; i++)
			if (dev->rev[i] >= 0x20)
				seq_putc(m, dev->rev[i]);
		seq_putc(m, ' ');

		PRINTP("\n%10ld kb read    in %5ld secs" ANDP br / 1024 ANDP tr);
		if (tr)
			PRINTP(" @ %5ld bps" ANDP br / tr);

		PRINTP("\n%10ld kb written in %5ld secs" ANDP bw / 1024 ANDP tw);
		if (tw)
			PRINTP(" @ %5ld bps" ANDP bw / tw);
		PRINTP("\n");
	}
#endif

	status = NCR5380_read(STATUS_REG);
	if (!(status & SR_REQ))
		PRINTP("REQ not asserted, phase unknown.\n");
	else {
		for (i = 0; (phases[i].value != PHASE_UNKNOWN) && (phases[i].value != (status & PHASE_MASK)); ++i);
		PRINTP("Phase %s\n" ANDP phases[i].name);
	}

	if (!hostdata->connected) {
		PRINTP("No currently connected command\n");
	} else {
		sprint_Scsi_Cmnd(m, (Scsi_Cmnd *) hostdata->connected);
	}

	PRINTP("issue_queue\n");

	for (ptr = (Scsi_Cmnd *) hostdata->issue_queue; ptr; ptr = (Scsi_Cmnd *) ptr->host_scribble)
		sprint_Scsi_Cmnd(m, ptr);

	PRINTP("disconnected_queue\n");

	for (ptr = (Scsi_Cmnd *) hostdata->disconnected_queue; ptr; ptr = (Scsi_Cmnd *) ptr->host_scribble)
		sprint_Scsi_Cmnd(m, ptr);

	spin_unlock_irqrestore(scsi_ptr->host_lock, flags);
	return 0;
}

#undef PRINTP
#undef ANDP

static struct scsi_host_template driver_template = {
	.show_info      	= generic_NCR5380_show_info,
	.name           	= "Generic NCR5380/NCR53C400 Scsi Driver",
	.detect         	= generic_NCR5380_detect,
	.release        	= generic_NCR5380_release_resources,
	.info           	= generic_NCR5380_info,
	.queuecommand   	= generic_NCR5380_queue_command,
	.eh_abort_handler	= generic_NCR5380_abort,
	.eh_bus_reset_handler	= generic_NCR5380_bus_reset,
	.bios_param     	= NCR5380_BIOSPARAM,
	.can_queue      	= CAN_QUEUE,
        .this_id        	= 7,
        .sg_tablesize   	= SG_ALL,
	.cmd_per_lun    	= CMD_PER_LUN,
        .use_clustering		= DISABLE_CLUSTERING,
};
#include <linux/module.h>
#include "scsi_module.c"

module_param(ncr_irq, int, 0);
module_param(ncr_dma, int, 0);
module_param(ncr_addr, int, 0);
module_param(ncr_5380, int, 0);
module_param(ncr_53c400, int, 0);
module_param(ncr_53c400a, int, 0);
module_param(dtc_3181e, int, 0);
MODULE_LICENSE("GPL");

#ifndef SCSI_G_NCR5380_MEM
static struct isapnp_device_id id_table[] = {
	{
	 ISAPNP_ANY_ID, ISAPNP_ANY_ID,
	 ISAPNP_VENDOR('D', 'T', 'C'), ISAPNP_FUNCTION(0x436e),
	 0},
	{0}
};

MODULE_DEVICE_TABLE(isapnp, id_table);
#endif

__setup("ncr5380=", do_NCR5380_setup);
__setup("ncr53c400=", do_NCR53C400_setup);
__setup("ncr53c400a=", do_NCR53C400A_setup);
__setup("dtc3181e=", do_DTC3181E_setup);
=======
static struct scsi_host_template driver_template = {
	.module			= THIS_MODULE,
	.proc_name		= DRV_MODULE_NAME,
	.name			= "Generic NCR5380/NCR53C400 SCSI",
	.info			= generic_NCR5380_info,
	.queuecommand		= generic_NCR5380_queue_command,
	.eh_abort_handler	= generic_NCR5380_abort,
	.eh_bus_reset_handler	= generic_NCR5380_bus_reset,
	.can_queue		= 16,
	.this_id		= 7,
	.sg_tablesize		= SG_ALL,
	.cmd_per_lun		= 2,
	.use_clustering		= DISABLE_CLUSTERING,
	.cmd_size		= NCR5380_CMD_SIZE,
	.max_sectors		= 128,
};


static int generic_NCR5380_isa_match(struct device *pdev, unsigned int ndev)
{
	int ret = generic_NCR5380_init_one(&driver_template, pdev, base[ndev],
					  irq[ndev], card[ndev]);
	if (ret) {
		if (base[ndev])
			printk(KERN_WARNING "Card not found at address 0x%03x\n",
			       base[ndev]);
		return 0;
	}

	return 1;
}

static int generic_NCR5380_isa_remove(struct device *pdev,
				   unsigned int ndev)
{
	generic_NCR5380_release_resources(dev_get_drvdata(pdev));
	dev_set_drvdata(pdev, NULL);
	return 0;
}

static struct isa_driver generic_NCR5380_isa_driver = {
	.match		= generic_NCR5380_isa_match,
	.remove		= generic_NCR5380_isa_remove,
	.driver		= {
		.name	= DRV_MODULE_NAME
	},
};

#if !defined(SCSI_G_NCR5380_MEM) && defined(CONFIG_PNP)
static struct pnp_device_id generic_NCR5380_pnp_ids[] = {
	{ .id = "DTC436e", .driver_data = BOARD_DTC3181E },
	{ .id = "" }
};
MODULE_DEVICE_TABLE(pnp, generic_NCR5380_pnp_ids);

static int generic_NCR5380_pnp_probe(struct pnp_dev *pdev,
			       const struct pnp_device_id *id)
{
	int base, irq;

	if (pnp_activate_dev(pdev) < 0)
		return -EBUSY;

	base = pnp_port_start(pdev, 0);
	irq = pnp_irq(pdev, 0);

	return generic_NCR5380_init_one(&driver_template, &pdev->dev, base, irq,
				       id->driver_data);
}

static void generic_NCR5380_pnp_remove(struct pnp_dev *pdev)
{
	generic_NCR5380_release_resources(pnp_get_drvdata(pdev));
	pnp_set_drvdata(pdev, NULL);
}

static struct pnp_driver generic_NCR5380_pnp_driver = {
	.name		= DRV_MODULE_NAME,
	.id_table	= generic_NCR5380_pnp_ids,
	.probe		= generic_NCR5380_pnp_probe,
	.remove		= generic_NCR5380_pnp_remove,
};
#endif /* !defined(SCSI_G_NCR5380_MEM) && defined(CONFIG_PNP) */

static int pnp_registered, isa_registered;

static int __init generic_NCR5380_init(void)
{
	int ret = 0;

	/* compatibility with old-style parameters */
	if (irq[0] == 0 && base[0] == 0 && card[0] == -1) {
		irq[0] = ncr_irq;
		base[0] = ncr_addr;
		if (ncr_5380)
			card[0] = BOARD_NCR5380;
		if (ncr_53c400)
			card[0] = BOARD_NCR53C400;
		if (ncr_53c400a)
			card[0] = BOARD_NCR53C400A;
		if (dtc_3181e)
			card[0] = BOARD_DTC3181E;
		if (hp_c2502)
			card[0] = BOARD_HP_C2502;
	}

#if !defined(SCSI_G_NCR5380_MEM) && defined(CONFIG_PNP)
	if (!pnp_register_driver(&generic_NCR5380_pnp_driver))
		pnp_registered = 1;
#endif
	ret = isa_register_driver(&generic_NCR5380_isa_driver, MAX_CARDS);
	if (!ret)
		isa_registered = 1;

	return (pnp_registered || isa_registered) ? 0 : ret;
}

static void __exit generic_NCR5380_exit(void)
{
#if !defined(SCSI_G_NCR5380_MEM) && defined(CONFIG_PNP)
	if (pnp_registered)
		pnp_unregister_driver(&generic_NCR5380_pnp_driver);
#endif
	if (isa_registered)
		isa_unregister_driver(&generic_NCR5380_isa_driver);
}

module_init(generic_NCR5380_init);
module_exit(generic_NCR5380_exit);
>>>>>>> v4.9.227
