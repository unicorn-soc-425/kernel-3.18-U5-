/*
 * Generic Macintosh NCR5380 driver
 *
 * Copyright 1998, Michael Schmitz <mschmitz@lbl.gov>
 *
 * derived in part from:
 */
/*
 * Generic Generic NCR5380 driver
 *
 * Copyright 1995, Russell King
<<<<<<< HEAD
 *
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

#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/ctype.h>
#include <linux/delay.h>

#include <linux/module.h>
#include <linux/signal.h>
=======
 */

#include <linux/types.h>
#include <linux/module.h>
>>>>>>> v4.9.227
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/interrupt.h>
<<<<<<< HEAD

#include <asm/io.h>
#include <asm/irq.h>

#include <asm/macintosh.h>
#include <asm/macints.h>
#include <asm/mac_via.h>

#include "scsi.h"
#include <scsi/scsi_host.h>
#include "mac_scsi.h"

/* These control the behaviour of the generic 5380 core */
#define AUTOSENSE
#define PSEUDO_DMA

#include "NCR5380.h"

#define RESET_BOOT
#define DRIVER_SETUP

extern void via_scsi_clear(void);

#ifdef RESET_BOOT
static void mac_scsi_reset_boot(struct Scsi_Host *instance);
#endif

static int setup_called = 0;
static int setup_can_queue = -1;
static int setup_cmd_per_lun = -1;
static int setup_sg_tablesize = -1;
static int setup_use_pdma = -1;
#ifdef SUPPORT_TAGS
static int setup_use_tagged_queuing = -1;
#endif
static int setup_hostid = -1;

/* Time (in jiffies) to wait after a reset; the SCSI standard calls for 250ms,
 * we usually do 0.5s to be on the safe side. But Toshiba CD-ROMs once more
 * need ten times the standard value... */
#define TOSHIBA_DELAY

#ifdef TOSHIBA_DELAY
#define	AFTER_RESET_DELAY	(5*HZ/2)
#else
#define	AFTER_RESET_DELAY	(HZ/2)
#endif

static volatile unsigned char *mac_scsi_regp = NULL;
static volatile unsigned char *mac_scsi_drq  = NULL;
static volatile unsigned char *mac_scsi_nodrq = NULL;

=======
#include <linux/platform_device.h>

#include <asm/hwtest.h>
#include <asm/io.h>
#include <asm/macints.h>
#include <asm/setup.h>

#include <scsi/scsi_host.h>

/* Definitions for the core NCR5380 driver. */

#define NCR5380_implementation_fields   unsigned char *pdma_base; \
                                        int pdma_residual

#define NCR5380_read(reg)               macscsi_read(instance, reg)
#define NCR5380_write(reg, value)       macscsi_write(instance, reg, value)

#define NCR5380_dma_xfer_len(instance, cmd, phase) \
        macscsi_dma_xfer_len(instance, cmd)
#define NCR5380_dma_recv_setup          macscsi_pread
#define NCR5380_dma_send_setup          macscsi_pwrite
#define NCR5380_dma_residual(instance)  (hostdata->pdma_residual)

#define NCR5380_intr                    macscsi_intr
#define NCR5380_queue_command           macscsi_queue_command
#define NCR5380_abort                   macscsi_abort
#define NCR5380_bus_reset               macscsi_bus_reset
#define NCR5380_info                    macscsi_info

#include "NCR5380.h"

static int setup_can_queue = -1;
module_param(setup_can_queue, int, 0);
static int setup_cmd_per_lun = -1;
module_param(setup_cmd_per_lun, int, 0);
static int setup_sg_tablesize = -1;
module_param(setup_sg_tablesize, int, 0);
static int setup_use_pdma = 512;
module_param(setup_use_pdma, int, 0);
static int setup_hostid = -1;
module_param(setup_hostid, int, 0);
static int setup_toshiba_delay = -1;
module_param(setup_toshiba_delay, int, 0);
>>>>>>> v4.9.227

/*
 * NCR 5380 register access functions
 */

<<<<<<< HEAD
#if 0
/* Debug versions */
#define CTRL(p,v) (*ctrl = (v))

static char macscsi_read(struct Scsi_Host *instance, int reg)
{
  int iobase = instance->io_port;
  int i;
  int *ctrl = &((struct NCR5380_hostdata *)instance->hostdata)->ctrl;

  CTRL(iobase, 0);
  i = in_8(iobase + (reg<<4));
  CTRL(iobase, 0x40);

  return i;
}

static void macscsi_write(struct Scsi_Host *instance, int reg, int value)
{
  int iobase = instance->io_port;
  int *ctrl = &((struct NCR5380_hostdata *)instance->hostdata)->ctrl;

  CTRL(iobase, 0);
  out_8(iobase + (reg<<4), value);
  CTRL(iobase, 0x40);
}
#else

/* Fast versions */
static __inline__ char macscsi_read(struct Scsi_Host *instance, int reg)
{
  return in_8(instance->io_port + (reg<<4));
}

static __inline__ void macscsi_write(struct Scsi_Host *instance, int reg, int value)
{
  out_8(instance->io_port + (reg<<4), value);
}
#endif


/*
 * Function : mac_scsi_setup(char *str)
 *
 * Purpose : booter command line initialization of the overrides array,
 *
 * Inputs : str - comma delimited list of options
 *
 */

static int __init mac_scsi_setup(char *str) {
#ifdef DRIVER_SETUP	
	int ints[7];
	
	(void)get_options( str, ARRAY_SIZE(ints), ints);
	
	if (setup_called++ || ints[0] < 1 || ints[0] > 6) {
	    printk(KERN_WARNING "scsi: <mac5380>"
		" Usage: mac5380=<can_queue>[,<cmd_per_lun>,<sg_tablesize>,<hostid>,<use_tags>,<use_pdma>]\n");
	    printk(KERN_ALERT "scsi: <mac5380> Bad Penguin parameters?\n");
	    return 0;
	}
	    
	if (ints[0] >= 1) {
		if (ints[1] > 0)
			/* no limits on this, just > 0 */
			setup_can_queue = ints[1];
	}
	if (ints[0] >= 2) {
		if (ints[2] > 0)
			setup_cmd_per_lun = ints[2];
	}
	if (ints[0] >= 3) {
		if (ints[3] >= 0) {
			setup_sg_tablesize = ints[3];
			/* Must be <= SG_ALL (255) */
			if (setup_sg_tablesize > SG_ALL)
				setup_sg_tablesize = SG_ALL;
		}
	}
	if (ints[0] >= 4) {
		/* Must be between 0 and 7 */
		if (ints[4] >= 0 && ints[4] <= 7)
			setup_hostid = ints[4];
		else if (ints[4] > 7)
			printk(KERN_WARNING "mac_scsi_setup: invalid host ID %d !\n", ints[4] );
	}
#ifdef SUPPORT_TAGS	
	if (ints[0] >= 5) {
		if (ints[5] >= 0)
			setup_use_tagged_queuing = !!ints[5];
	}
	
	if (ints[0] == 6) {
	    if (ints[6] >= 0)
		setup_use_pdma = ints[6];
	}
#else
	if (ints[0] == 5) {
	    if (ints[5] >= 0)
		setup_use_pdma = ints[5];
	}
#endif /* SUPPORT_TAGS */
	
#endif /* DRIVER_SETUP */
=======
static inline char macscsi_read(struct Scsi_Host *instance, int reg)
{
	return in_8(instance->base + (reg << 4));
}

static inline void macscsi_write(struct Scsi_Host *instance, int reg, int value)
{
	out_8(instance->base + (reg << 4), value);
}

#ifndef MODULE
static int __init mac_scsi_setup(char *str)
{
	int ints[8];

	(void)get_options(str, ARRAY_SIZE(ints), ints);

	if (ints[0] < 1) {
		pr_err("Usage: mac5380=<can_queue>[,<cmd_per_lun>[,<sg_tablesize>[,<hostid>[,<use_tags>[,<use_pdma>[,<toshiba_delay>]]]]]]\n");
		return 0;
	}
	if (ints[0] >= 1)
		setup_can_queue = ints[1];
	if (ints[0] >= 2)
		setup_cmd_per_lun = ints[2];
	if (ints[0] >= 3)
		setup_sg_tablesize = ints[3];
	if (ints[0] >= 4)
		setup_hostid = ints[4];
	/* ints[5] (use_tagged_queuing) is ignored */
	if (ints[0] >= 6)
		setup_use_pdma = ints[6];
	if (ints[0] >= 7)
		setup_toshiba_delay = ints[7];
>>>>>>> v4.9.227
	return 1;
}

__setup("mac5380=", mac_scsi_setup);
<<<<<<< HEAD

/*
 * Function : int macscsi_detect(struct scsi_host_template * tpnt)
 *
 * Purpose : initializes mac NCR5380 driver based on the
 *	command line / compile time port and irq definitions.
 *
 * Inputs : tpnt - template for this SCSI adapter.
 *
 * Returns : 1 if a host adapter was found, 0 if not.
 *
 */
 
int __init macscsi_detect(struct scsi_host_template * tpnt)
{
    static int called = 0;
    int flags = 0;
    struct Scsi_Host *instance;

    if (!MACH_IS_MAC || called)
	return( 0 );

    if (macintosh_config->scsi_type != MAC_SCSI_OLD)
	return( 0 );

    /* setup variables */
    tpnt->can_queue =
	(setup_can_queue > 0) ? setup_can_queue : CAN_QUEUE;
    tpnt->cmd_per_lun =
	(setup_cmd_per_lun > 0) ? setup_cmd_per_lun : CMD_PER_LUN;
    tpnt->sg_tablesize = 
	(setup_sg_tablesize >= 0) ? setup_sg_tablesize : SG_TABLESIZE;

    if (setup_hostid >= 0)
	tpnt->this_id = setup_hostid;
    else {
	/* use 7 as default */
	tpnt->this_id = 7;
    }

#ifdef SUPPORT_TAGS
    if (setup_use_tagged_queuing < 0)
	setup_use_tagged_queuing = USE_TAGGED_QUEUING;
#endif

    /* Once we support multiple 5380s (e.g. DuoDock) we'll do
       something different here */
    instance = scsi_register (tpnt, sizeof(struct NCR5380_hostdata));
    if (instance == NULL)
	return 0;

    if (macintosh_config->ident == MAC_MODEL_IIFX) {
	mac_scsi_regp  = via1+0x8000;
	mac_scsi_drq   = via1+0xE000;
	mac_scsi_nodrq = via1+0xC000;
	/* The IIFX should be able to do true DMA, but pseudo-dma doesn't work */
	flags = FLAG_NO_PSEUDO_DMA;
    } else {
	mac_scsi_regp  = via1+0x10000;
	mac_scsi_drq   = via1+0x6000;
	mac_scsi_nodrq = via1+0x12000;
    }

    if (! setup_use_pdma)
	flags = FLAG_NO_PSEUDO_DMA;
	
    instance->io_port = (unsigned long) mac_scsi_regp;
    instance->irq = IRQ_MAC_SCSI;

#ifdef RESET_BOOT   
    mac_scsi_reset_boot(instance);
#endif
    
    NCR5380_init(instance, flags);

    instance->n_io_port = 255;

    ((struct NCR5380_hostdata *)instance->hostdata)->ctrl = 0;

    if (instance->irq != SCSI_IRQ_NONE)
	if (request_irq(instance->irq, NCR5380_intr, 0, "ncr5380", instance)) {
	    printk(KERN_WARNING "scsi%d: IRQ%d not free, interrupts disabled\n",
		   instance->host_no, instance->irq);
	    instance->irq = SCSI_IRQ_NONE;
	}

    printk(KERN_INFO "scsi%d: generic 5380 at port %lX irq", instance->host_no, instance->io_port);
    if (instance->irq == SCSI_IRQ_NONE)
	printk (KERN_INFO "s disabled");
    else
	printk (KERN_INFO " %d", instance->irq);
    printk(KERN_INFO " options CAN_QUEUE=%d CMD_PER_LUN=%d release=%d",
	   instance->can_queue, instance->cmd_per_lun, MACSCSI_PUBLIC_RELEASE);
    printk(KERN_INFO "\nscsi%d:", instance->host_no);
    NCR5380_print_options(instance);
    printk("\n");
    called = 1;
    return 1;
}

int macscsi_release (struct Scsi_Host *shpnt)
{
	if (shpnt->irq != SCSI_IRQ_NONE)
		free_irq(shpnt->irq, shpnt);
	NCR5380_exit(shpnt);

	return 0;
}

#ifdef RESET_BOOT
/*
 * Our 'bus reset on boot' function
 */

static void mac_scsi_reset_boot(struct Scsi_Host *instance)
{
	unsigned long end;

	NCR5380_local_declare();
	NCR5380_setup(instance);
	
	/*
	 * Do a SCSI reset to clean up the bus during initialization. No messing
	 * with the queues, interrupts, or locks necessary here.
	 */

	printk(KERN_INFO "Macintosh SCSI: resetting the SCSI bus..." );

	/* get in phase */
	NCR5380_write( TARGET_COMMAND_REG,
		      PHASE_SR_TO_TCR( NCR5380_read(STATUS_REG) ));

	/* assert RST */
	NCR5380_write( INITIATOR_COMMAND_REG, ICR_BASE | ICR_ASSERT_RST );
	/* The min. reset hold time is 25us, so 40us should be enough */
	udelay( 50 );
	/* reset RST and interrupt */
	NCR5380_write( INITIATOR_COMMAND_REG, ICR_BASE );
	NCR5380_read( RESET_PARITY_INTERRUPT_REG );

	for( end = jiffies + AFTER_RESET_DELAY; time_before(jiffies, end); )
		barrier();

	printk(KERN_INFO " done\n" );
}
#endif

const char * macscsi_info (struct Scsi_Host *spnt) {
	return "";
}

/* 
   Pseudo-DMA: (Ove Edlund)
   The code attempts to catch bus errors that occur if one for example
   "trips over the cable".
   XXX: Since bus errors in the PDMA routines never happen on my 
   computer, the bus error code is untested. 
   If the code works as intended, a bus error results in Pseudo-DMA 
   beeing disabled, meaning that the driver switches to slow handshake. 
   If bus errors are NOT extremely rare, this has to be changed. 
*/

#define CP_IO_TO_MEM(s,d,len)				\
=======
#endif /* !MODULE */

/* Pseudo DMA asm originally by Ove Edlund */

#define CP_IO_TO_MEM(s,d,n)				\
>>>>>>> v4.9.227
__asm__ __volatile__					\
    ("    cmp.w  #4,%2\n"				\
     "    bls    8f\n"					\
     "    move.w %1,%%d0\n"				\
     "    neg.b  %%d0\n"				\
     "    and.w  #3,%%d0\n"				\
     "    sub.w  %%d0,%2\n"				\
     "    bra    2f\n"					\
     " 1: move.b (%0),(%1)+\n"				\
     " 2: dbf    %%d0,1b\n"				\
     "    move.w %2,%%d0\n"				\
     "    lsr.w  #5,%%d0\n"				\
     "    bra    4f\n"					\
     " 3: move.l (%0),(%1)+\n"				\
     "31: move.l (%0),(%1)+\n"				\
     "32: move.l (%0),(%1)+\n"				\
     "33: move.l (%0),(%1)+\n"				\
     "34: move.l (%0),(%1)+\n"				\
     "35: move.l (%0),(%1)+\n"				\
     "36: move.l (%0),(%1)+\n"				\
     "37: move.l (%0),(%1)+\n"				\
     " 4: dbf    %%d0,3b\n"				\
     "    move.w %2,%%d0\n"				\
     "    lsr.w  #2,%%d0\n"				\
     "    and.w  #7,%%d0\n"				\
     "    bra    6f\n"					\
     " 5: move.l (%0),(%1)+\n"				\
     " 6: dbf    %%d0,5b\n"				\
     "    and.w  #3,%2\n"				\
     "    bra    8f\n"					\
     " 7: move.b (%0),(%1)+\n"				\
     " 8: dbf    %2,7b\n"				\
     "    moveq.l #0, %2\n"				\
     " 9: \n"						\
     ".section .fixup,\"ax\"\n"				\
     "    .even\n"					\
<<<<<<< HEAD
     "90: moveq.l #1, %2\n"				\
=======
     "91: moveq.l #1, %2\n"				\
     "    jra 9b\n"					\
     "94: moveq.l #4, %2\n"				\
>>>>>>> v4.9.227
     "    jra 9b\n"					\
     ".previous\n"					\
     ".section __ex_table,\"a\"\n"			\
     "   .align 4\n"					\
<<<<<<< HEAD
     "   .long  1b,90b\n"				\
     "   .long  3b,90b\n"				\
     "   .long 31b,90b\n"				\
     "   .long 32b,90b\n"				\
     "   .long 33b,90b\n"				\
     "   .long 34b,90b\n"				\
     "   .long 35b,90b\n"				\
     "   .long 36b,90b\n"				\
     "   .long 37b,90b\n"				\
     "   .long  5b,90b\n"				\
     "   .long  7b,90b\n"				\
     ".previous"					\
     : "=a"(s), "=a"(d), "=d"(len)			\
     : "0"(s), "1"(d), "2"(len)				\
     : "d0")


static int macscsi_pread (struct Scsi_Host *instance,
			  unsigned char *dst, int len)
{
   unsigned char *d;
   volatile unsigned char *s;

   NCR5380_local_declare();
   NCR5380_setup(instance);

   s = mac_scsi_drq+0x60;
   d = dst;

/* These conditions are derived from MacOS */

   while (!(NCR5380_read(BUS_AND_STATUS_REG) & BASR_DRQ) 
         && !(NCR5380_read(STATUS_REG) & SR_REQ))
      ;
   if (!(NCR5380_read(BUS_AND_STATUS_REG) & BASR_DRQ) 
         && (NCR5380_read(BUS_AND_STATUS_REG) & BASR_PHASE_MATCH)) {
      printk(KERN_ERR "Error in macscsi_pread\n");
      return -1;
   }

   CP_IO_TO_MEM(s, d, len);
   
   if (len != 0) {
      printk(KERN_NOTICE "Bus error in macscsi_pread\n");
      return -1;
   }
   
   return 0;
}


#define CP_MEM_TO_IO(s,d,len)				\
=======
     "   .long  1b,91b\n"				\
     "   .long  3b,94b\n"				\
     "   .long 31b,94b\n"				\
     "   .long 32b,94b\n"				\
     "   .long 33b,94b\n"				\
     "   .long 34b,94b\n"				\
     "   .long 35b,94b\n"				\
     "   .long 36b,94b\n"				\
     "   .long 37b,94b\n"				\
     "   .long  5b,94b\n"				\
     "   .long  7b,91b\n"				\
     ".previous"					\
     : "=a"(s), "=a"(d), "=d"(n)			\
     : "0"(s), "1"(d), "2"(n)				\
     : "d0")

static int macscsi_pread(struct Scsi_Host *instance,
                         unsigned char *dst, int len)
{
	struct NCR5380_hostdata *hostdata = shost_priv(instance);
	unsigned char *s = hostdata->pdma_base + (INPUT_DATA_REG << 4);
	unsigned char *d = dst;
	int n = len;
	int transferred;

	while (!NCR5380_poll_politely(instance, BUS_AND_STATUS_REG,
	                              BASR_DRQ | BASR_PHASE_MATCH,
	                              BASR_DRQ | BASR_PHASE_MATCH, HZ / 64)) {
		CP_IO_TO_MEM(s, d, n);

		transferred = d - dst - n;
		hostdata->pdma_residual = len - transferred;

		/* No bus error. */
		if (n == 0)
			return 0;

		/* Target changed phase early? */
		if (NCR5380_poll_politely2(instance, STATUS_REG, SR_REQ, SR_REQ,
		                           BUS_AND_STATUS_REG, BASR_ACK, BASR_ACK, HZ / 64) < 0)
			scmd_printk(KERN_ERR, hostdata->connected,
			            "%s: !REQ and !ACK\n", __func__);
		if (!(NCR5380_read(BUS_AND_STATUS_REG) & BASR_PHASE_MATCH))
			return 0;

		dsprintk(NDEBUG_PSEUDO_DMA, instance,
		         "%s: bus error (%d/%d)\n", __func__, transferred, len);
		NCR5380_dprint(NDEBUG_PSEUDO_DMA, instance);
		d = dst + transferred;
		n = len - transferred;
	}

	scmd_printk(KERN_ERR, hostdata->connected,
	            "%s: phase mismatch or !DRQ\n", __func__);
	NCR5380_dprint(NDEBUG_PSEUDO_DMA, instance);
	return -1;
}


#define CP_MEM_TO_IO(s,d,n)				\
>>>>>>> v4.9.227
__asm__ __volatile__					\
    ("    cmp.w  #4,%2\n"				\
     "    bls    8f\n"					\
     "    move.w %0,%%d0\n"				\
     "    neg.b  %%d0\n"				\
     "    and.w  #3,%%d0\n"				\
     "    sub.w  %%d0,%2\n"				\
     "    bra    2f\n"					\
     " 1: move.b (%0)+,(%1)\n"				\
     " 2: dbf    %%d0,1b\n"				\
     "    move.w %2,%%d0\n"				\
     "    lsr.w  #5,%%d0\n"				\
     "    bra    4f\n"					\
     " 3: move.l (%0)+,(%1)\n"				\
     "31: move.l (%0)+,(%1)\n"				\
     "32: move.l (%0)+,(%1)\n"				\
     "33: move.l (%0)+,(%1)\n"				\
     "34: move.l (%0)+,(%1)\n"				\
     "35: move.l (%0)+,(%1)\n"				\
     "36: move.l (%0)+,(%1)\n"				\
     "37: move.l (%0)+,(%1)\n"				\
     " 4: dbf    %%d0,3b\n"				\
     "    move.w %2,%%d0\n"				\
     "    lsr.w  #2,%%d0\n"				\
     "    and.w  #7,%%d0\n"				\
     "    bra    6f\n"					\
     " 5: move.l (%0)+,(%1)\n"				\
     " 6: dbf    %%d0,5b\n"				\
     "    and.w  #3,%2\n"				\
     "    bra    8f\n"					\
     " 7: move.b (%0)+,(%1)\n"				\
     " 8: dbf    %2,7b\n"				\
     "    moveq.l #0, %2\n"				\
     " 9: \n"						\
     ".section .fixup,\"ax\"\n"				\
     "    .even\n"					\
<<<<<<< HEAD
     "90: moveq.l #1, %2\n"				\
=======
     "91: moveq.l #1, %2\n"				\
     "    jra 9b\n"					\
     "94: moveq.l #4, %2\n"				\
>>>>>>> v4.9.227
     "    jra 9b\n"					\
     ".previous\n"					\
     ".section __ex_table,\"a\"\n"			\
     "   .align 4\n"					\
<<<<<<< HEAD
     "   .long  1b,90b\n"				\
     "   .long  3b,90b\n"				\
     "   .long 31b,90b\n"				\
     "   .long 32b,90b\n"				\
     "   .long 33b,90b\n"				\
     "   .long 34b,90b\n"				\
     "   .long 35b,90b\n"				\
     "   .long 36b,90b\n"				\
     "   .long 37b,90b\n"				\
     "   .long  5b,90b\n"				\
     "   .long  7b,90b\n"				\
     ".previous"					\
     : "=a"(s), "=a"(d), "=d"(len)			\
     : "0"(s), "1"(d), "2"(len)				\
     : "d0")

static int macscsi_pwrite (struct Scsi_Host *instance,
				  unsigned char *src, int len)
{
   unsigned char *s;
   volatile unsigned char *d;

   NCR5380_local_declare();
   NCR5380_setup(instance);

   s = src;
   d = mac_scsi_drq;
   
/* These conditions are derived from MacOS */

   while (!(NCR5380_read(BUS_AND_STATUS_REG) & BASR_DRQ) 
         && (!(NCR5380_read(STATUS_REG) & SR_REQ) 
            || (NCR5380_read(BUS_AND_STATUS_REG) & BASR_PHASE_MATCH))) 
      ;
   if (!(NCR5380_read(BUS_AND_STATUS_REG) & BASR_DRQ)) {
      printk(KERN_ERR "Error in macscsi_pwrite\n");
      return -1;
   }

   CP_MEM_TO_IO(s, d, len);   

   if (len != 0) {
      printk(KERN_NOTICE "Bus error in macscsi_pwrite\n");
      return -1;
   }
   
   return 0;
}


#include "NCR5380.c"

static struct scsi_host_template driver_template = {
	.proc_name			= "Mac5380",
	.show_info			= macscsi_show_info,
	.write_info			= macscsi_write_info,
	.name				= "Macintosh NCR5380 SCSI",
	.detect				= macscsi_detect,
	.release			= macscsi_release,
	.info				= macscsi_info,
	.queuecommand			= macscsi_queue_command,
	.eh_abort_handler		= macscsi_abort,
	.eh_bus_reset_handler		= macscsi_bus_reset,
	.can_queue			= CAN_QUEUE,
	.this_id			= 7,
	.sg_tablesize			= SG_ALL,
	.cmd_per_lun			= CMD_PER_LUN,
	.use_clustering			= DISABLE_CLUSTERING
};


#include "scsi_module.c"
=======
     "   .long  1b,91b\n"				\
     "   .long  3b,94b\n"				\
     "   .long 31b,94b\n"				\
     "   .long 32b,94b\n"				\
     "   .long 33b,94b\n"				\
     "   .long 34b,94b\n"				\
     "   .long 35b,94b\n"				\
     "   .long 36b,94b\n"				\
     "   .long 37b,94b\n"				\
     "   .long  5b,94b\n"				\
     "   .long  7b,91b\n"				\
     ".previous"					\
     : "=a"(s), "=a"(d), "=d"(n)			\
     : "0"(s), "1"(d), "2"(n)				\
     : "d0")

static int macscsi_pwrite(struct Scsi_Host *instance,
                          unsigned char *src, int len)
{
	struct NCR5380_hostdata *hostdata = shost_priv(instance);
	unsigned char *s = src;
	unsigned char *d = hostdata->pdma_base + (OUTPUT_DATA_REG << 4);
	int n = len;
	int transferred;

	while (!NCR5380_poll_politely(instance, BUS_AND_STATUS_REG,
	                              BASR_DRQ | BASR_PHASE_MATCH,
	                              BASR_DRQ | BASR_PHASE_MATCH, HZ / 64)) {
		CP_MEM_TO_IO(s, d, n);

		transferred = s - src - n;
		hostdata->pdma_residual = len - transferred;

		/* Target changed phase early? */
		if (NCR5380_poll_politely2(instance, STATUS_REG, SR_REQ, SR_REQ,
		                           BUS_AND_STATUS_REG, BASR_ACK, BASR_ACK, HZ / 64) < 0)
			scmd_printk(KERN_ERR, hostdata->connected,
			            "%s: !REQ and !ACK\n", __func__);
		if (!(NCR5380_read(BUS_AND_STATUS_REG) & BASR_PHASE_MATCH))
			return 0;

		/* No bus error. */
		if (n == 0) {
			if (NCR5380_poll_politely(instance, TARGET_COMMAND_REG,
			                          TCR_LAST_BYTE_SENT,
			                          TCR_LAST_BYTE_SENT, HZ / 64) < 0)
				scmd_printk(KERN_ERR, hostdata->connected,
				            "%s: Last Byte Sent timeout\n", __func__);
			return 0;
		}

		dsprintk(NDEBUG_PSEUDO_DMA, instance,
		         "%s: bus error (%d/%d)\n", __func__, transferred, len);
		NCR5380_dprint(NDEBUG_PSEUDO_DMA, instance);
		s = src + transferred;
		n = len - transferred;
	}

	scmd_printk(KERN_ERR, hostdata->connected,
	            "%s: phase mismatch or !DRQ\n", __func__);
	NCR5380_dprint(NDEBUG_PSEUDO_DMA, instance);

	return -1;
}

static int macscsi_dma_xfer_len(struct Scsi_Host *instance,
                                struct scsi_cmnd *cmd)
{
	struct NCR5380_hostdata *hostdata = shost_priv(instance);

	if (hostdata->flags & FLAG_NO_PSEUDO_DMA ||
	    cmd->SCp.this_residual < setup_use_pdma)
		return 0;

	return cmd->SCp.this_residual;
}

#include "NCR5380.c"

#define DRV_MODULE_NAME         "mac_scsi"
#define PFX                     DRV_MODULE_NAME ": "

static struct scsi_host_template mac_scsi_template = {
	.module			= THIS_MODULE,
	.proc_name		= DRV_MODULE_NAME,
	.name			= "Macintosh NCR5380 SCSI",
	.info			= macscsi_info,
	.queuecommand		= macscsi_queue_command,
	.eh_abort_handler	= macscsi_abort,
	.eh_bus_reset_handler	= macscsi_bus_reset,
	.can_queue		= 16,
	.this_id		= 7,
	.sg_tablesize		= 1,
	.cmd_per_lun		= 2,
	.use_clustering		= DISABLE_CLUSTERING,
	.cmd_size		= NCR5380_CMD_SIZE,
	.max_sectors		= 128,
};

static int __init mac_scsi_probe(struct platform_device *pdev)
{
	struct Scsi_Host *instance;
	int error;
	int host_flags = 0;
	struct resource *irq, *pio_mem, *pdma_mem = NULL;

	pio_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!pio_mem)
		return -ENODEV;

	pdma_mem = platform_get_resource(pdev, IORESOURCE_MEM, 1);

	irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);

	if (!hwreg_present((unsigned char *)pio_mem->start +
	                   (STATUS_REG << 4))) {
		pr_info(PFX "no device detected at %pap\n", &pio_mem->start);
		return -ENODEV;
	}

	if (setup_can_queue > 0)
		mac_scsi_template.can_queue = setup_can_queue;
	if (setup_cmd_per_lun > 0)
		mac_scsi_template.cmd_per_lun = setup_cmd_per_lun;
	if (setup_sg_tablesize > 0)
		mac_scsi_template.sg_tablesize = setup_sg_tablesize;
	if (setup_hostid >= 0)
		mac_scsi_template.this_id = setup_hostid & 7;

	instance = scsi_host_alloc(&mac_scsi_template,
	                           sizeof(struct NCR5380_hostdata));
	if (!instance)
		return -ENOMEM;

	instance->base = pio_mem->start;
	if (irq)
		instance->irq = irq->start;
	else
		instance->irq = NO_IRQ;

	if (pdma_mem && setup_use_pdma) {
		struct NCR5380_hostdata *hostdata = shost_priv(instance);

		hostdata->pdma_base = (unsigned char *)pdma_mem->start;
	} else
		host_flags |= FLAG_NO_PSEUDO_DMA;

	host_flags |= setup_toshiba_delay > 0 ? FLAG_TOSHIBA_DELAY : 0;

	error = NCR5380_init(instance, host_flags | FLAG_LATE_DMA_SETUP);
	if (error)
		goto fail_init;

	if (instance->irq != NO_IRQ) {
		error = request_irq(instance->irq, macscsi_intr, IRQF_SHARED,
		                    "NCR5380", instance);
		if (error)
			goto fail_irq;
	}

	NCR5380_maybe_reset_bus(instance);

	error = scsi_add_host(instance, NULL);
	if (error)
		goto fail_host;

	platform_set_drvdata(pdev, instance);

	scsi_scan_host(instance);
	return 0;

fail_host:
	if (instance->irq != NO_IRQ)
		free_irq(instance->irq, instance);
fail_irq:
	NCR5380_exit(instance);
fail_init:
	scsi_host_put(instance);
	return error;
}

static int __exit mac_scsi_remove(struct platform_device *pdev)
{
	struct Scsi_Host *instance = platform_get_drvdata(pdev);

	scsi_remove_host(instance);
	if (instance->irq != NO_IRQ)
		free_irq(instance->irq, instance);
	NCR5380_exit(instance);
	scsi_host_put(instance);
	return 0;
}

static struct platform_driver mac_scsi_driver = {
	.remove = __exit_p(mac_scsi_remove),
	.driver = {
		.name	= DRV_MODULE_NAME,
	},
};

module_platform_driver_probe(mac_scsi_driver, mac_scsi_probe);

MODULE_ALIAS("platform:" DRV_MODULE_NAME);
MODULE_LICENSE("GPL");
>>>>>>> v4.9.227
