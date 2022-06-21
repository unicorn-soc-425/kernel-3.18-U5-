/*
<<<<<<< HEAD
 * comedi/drivers/adv_pci1710.c
 *
 * Author: Michal Dobes <dobes@tesnet.cz>
 *
 * Thanks to ZhenGang Shang <ZhenGang.Shang@Advantech.com.cn>
 * for testing and informations.
 *
 *  hardware driver for Advantech cards:
 *   card:   PCI-1710, PCI-1710HG, PCI-1711, PCI-1713, PCI-1720, PCI-1731
 *   driver: pci1710,  pci1710hg,  pci1711,  pci1713,  pci1720,  pci1731
 *
 * Options:
 *  [0] - PCI bus number - if bus number and slot number are 0,
 *                         then driver search for first unused card
 *  [1] - PCI slot number
 *
*/
/*
Driver: adv_pci1710
Description: Advantech PCI-1710, PCI-1710HG, PCI-1711, PCI-1713,
	     Advantech PCI-1720, PCI-1731
Author: Michal Dobes <dobes@tesnet.cz>
Devices: [Advantech] PCI-1710 (adv_pci1710), PCI-1710HG (pci1710hg),
  PCI-1711 (adv_pci1710), PCI-1713, PCI-1720,
  PCI-1731
Status: works

This driver supports AI, AO, DI and DO subdevices.
AI subdevice supports cmd and insn interface,
other subdevices support only insn interface.

The PCI-1710 and PCI-1710HG have the same PCI device ID, so the
driver cannot distinguish between them, as would be normal for a
PCI driver.

Configuration options:
  [0] - PCI bus of device (optional)
  [1] - PCI slot of device (optional)
	If bus/slot is not specified, the first available PCI
	device will be used.
*/

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/interrupt.h>

#include "../comedidev.h"

#include "comedi_fc.h"
#include "8253.h"
#include "amcc_s5933.h"

/* hardware types of the cards */
#define TYPE_PCI171X	0
#define TYPE_PCI1713	2
#define TYPE_PCI1720	3

#define PCI171x_AD_DATA	 0	/* R:   A/D data */
#define PCI171x_SOFTTRG	 0	/* W:   soft trigger for A/D */
#define PCI171x_RANGE	 2	/* W:   A/D gain/range register */
#define PCI171x_MUX	 4	/* W:   A/D multiplexor control */
#define PCI171x_STATUS	 6	/* R:   status register */
#define PCI171x_CONTROL	 6	/* W:   control register */
#define PCI171x_CLRINT	 8	/* W:   clear interrupts request */
#define PCI171x_CLRFIFO	 9	/* W:   clear FIFO */
#define PCI171x_DA1	10	/* W:   D/A register */
#define PCI171x_DA2	12	/* W:   D/A register */
#define PCI171x_DAREF	14	/* W:   D/A reference control */
#define PCI171x_DI	16	/* R:   digi inputs */
#define PCI171x_DO	16	/* R:   digi inputs */

#define PCI171X_TIMER_BASE	0x18

#define PCI171x_CNT0	24	/* R/W: 8254 counter 0 */
#define PCI171x_CNT1	26	/* R/W: 8254 counter 1 */
#define PCI171x_CNT2	28	/* R/W: 8254 counter 2 */
#define PCI171x_CNTCTRL	30	/* W:   8254 counter control */

/* upper bits from status register (PCI171x_STATUS) (lower is same with control
 * reg) */
#define	Status_FE	0x0100	/* 1=FIFO is empty */
#define Status_FH	0x0200	/* 1=FIFO is half full */
#define Status_FF	0x0400	/* 1=FIFO is full, fatal error */
#define Status_IRQ	0x0800	/* 1=IRQ occurred */
/* bits from control register (PCI171x_CONTROL) */
#define Control_CNT0	0x0040	/* 1=CNT0 have external source,
				 * 0=have internal 100kHz source */
#define Control_ONEFH	0x0020	/* 1=IRQ on FIFO is half full, 0=every sample */
#define Control_IRQEN	0x0010	/* 1=enable IRQ */
#define Control_GATE	0x0008	/* 1=enable external trigger GATE (8254?) */
#define Control_EXT	0x0004	/* 1=external trigger source */
#define Control_PACER	0x0002	/* 1=enable internal 8254 trigger source */
#define Control_SW	0x0001	/* 1=enable software trigger source */
/* bits from counter control register (PCI171x_CNTCTRL) */
#define Counter_BCD     0x0001	/* 0 = binary counter, 1 = BCD counter */
#define Counter_M0      0x0002	/* M0-M2 select modes 0-5 */
#define Counter_M1      0x0004	/* 000 = mode 0, 010 = mode 2 ... */
#define Counter_M2      0x0008
#define Counter_RW0     0x0010	/* RW0/RW1 select read/write mode */
#define Counter_RW1     0x0020
#define Counter_SC0     0x0040	/* Select Counter. Only 00 or 11 may */
#define Counter_SC1     0x0080	/* be used, 00 for CNT0,
				 * 11 for read-back command */

#define PCI1720_DA0	 0	/* W:   D/A register 0 */
#define PCI1720_DA1	 2	/* W:   D/A register 1 */
#define PCI1720_DA2	 4	/* W:   D/A register 2 */
#define PCI1720_DA3	 6	/* W:   D/A register 3 */
#define PCI1720_RANGE	 8	/* R/W: D/A range register */
#define PCI1720_SYNCOUT	 9	/* W:   D/A synchronized output register */
#define PCI1720_SYNCONT	15	/* R/W: D/A synchronized control */

/* D/A synchronized control (PCI1720_SYNCONT) */
#define Syncont_SC0	 1	/* set synchronous output mode */

static const struct comedi_lrange range_pci1710_3 = {
	9, {
		BIP_RANGE(5),
		BIP_RANGE(2.5),
		BIP_RANGE(1.25),
		BIP_RANGE(0.625),
		BIP_RANGE(10),
		UNI_RANGE(10),
		UNI_RANGE(5),
		UNI_RANGE(2.5),
		UNI_RANGE(1.25)
	}
};

static const char range_codes_pci1710_3[] = { 0x00, 0x01, 0x02, 0x03, 0x04,
					      0x10, 0x11, 0x12, 0x13 };

static const struct comedi_lrange range_pci1710hg = {
	12, {
		BIP_RANGE(5),
		BIP_RANGE(0.5),
		BIP_RANGE(0.05),
		BIP_RANGE(0.005),
		BIP_RANGE(10),
		BIP_RANGE(1),
		BIP_RANGE(0.1),
		BIP_RANGE(0.01),
		UNI_RANGE(10),
		UNI_RANGE(1),
		UNI_RANGE(0.1),
		UNI_RANGE(0.01)
	}
};

static const char range_codes_pci1710hg[] = { 0x00, 0x01, 0x02, 0x03, 0x04,
					      0x05, 0x06, 0x07, 0x10, 0x11,
					      0x12, 0x13 };

static const struct comedi_lrange range_pci17x1 = {
	5, {
		BIP_RANGE(10),
		BIP_RANGE(5),
		BIP_RANGE(2.5),
		BIP_RANGE(1.25),
		BIP_RANGE(0.625)
	}
};

static const char range_codes_pci17x1[] = { 0x00, 0x01, 0x02, 0x03, 0x04 };

static const struct comedi_lrange range_pci1720 = {
	4, {
		UNI_RANGE(5),
		UNI_RANGE(10),
		BIP_RANGE(5),
		BIP_RANGE(10)
	}
};

static const struct comedi_lrange range_pci171x_da = {
	2, {
		UNI_RANGE(5),
		UNI_RANGE(10)
=======
 * adv_pci1710.c
 * Comedi driver for Advantech PCI-1710 series boards
 * Author: Michal Dobes <dobes@tesnet.cz>
 *
 * Thanks to ZhenGang Shang <ZhenGang.Shang@Advantech.com.cn>
 * for testing and information.
 */

/*
 * Driver: adv_pci1710
 * Description: Comedi driver for Advantech PCI-1710 series boards
 * Devices: [Advantech] PCI-1710 (adv_pci1710), PCI-1710HG, PCI-1711,
 *   PCI-1713, PCI-1731
 * Author: Michal Dobes <dobes@tesnet.cz>
 * Updated: Fri, 29 Oct 2015 17:19:35 -0700
 * Status: works
 *
 * Configuration options: not applicable, uses PCI auto config
 *
 * This driver supports AI, AO, DI and DO subdevices.
 * AI subdevice supports cmd and insn interface,
 * other subdevices support only insn interface.
 *
 * The PCI-1710 and PCI-1710HG have the same PCI device ID, so the
 * driver cannot distinguish between them, as would be normal for a
 * PCI driver.
 */

#include <linux/module.h>
#include <linux/interrupt.h>

#include "../comedi_pci.h"

#include "comedi_8254.h"
#include "amcc_s5933.h"

/*
 * PCI BAR2 Register map (dev->iobase)
 */
#define PCI171X_AD_DATA_REG	0x00	/* R:   A/D data */
#define PCI171X_SOFTTRG_REG	0x00	/* W:   soft trigger for A/D */
#define PCI171X_RANGE_REG	0x02	/* W:   A/D gain/range register */
#define PCI171X_RANGE_DIFF	BIT(5)
#define PCI171X_RANGE_UNI	BIT(4)
#define PCI171X_RANGE_GAIN(x)	(((x) & 0x7) << 0)
#define PCI171X_MUX_REG		0x04	/* W:   A/D multiplexor control */
#define PCI171X_MUX_CHANH(x)	(((x) & 0xff) << 8)
#define PCI171X_MUX_CHANL(x)	(((x) & 0xff) << 0)
#define PCI171X_MUX_CHAN(x)	(PCI171X_MUX_CHANH(x) | PCI171X_MUX_CHANL(x))
#define PCI171X_STATUS_REG	0x06	/* R:   status register */
#define PCI171X_STATUS_IRQ	BIT(11)	/* 1=IRQ occurred */
#define PCI171X_STATUS_FF	BIT(10)	/* 1=FIFO is full, fatal error */
#define PCI171X_STATUS_FH	BIT(9)	/* 1=FIFO is half full */
#define PCI171X_STATUS_FE	BIT(8)	/* 1=FIFO is empty */
#define PCI171X_CTRL_REG	0x06	/* W:   control register */
#define PCI171X_CTRL_CNT0	BIT(6)	/* 1=ext. clk, 0=int. 100kHz clk */
#define PCI171X_CTRL_ONEFH	BIT(5)	/* 1=on FIFO half full, 0=on sample */
#define PCI171X_CTRL_IRQEN	BIT(4)	/* 1=enable IRQ */
#define PCI171X_CTRL_GATE	BIT(3)	/* 1=enable ext. trigger GATE (8254?) */
#define PCI171X_CTRL_EXT	BIT(2)	/* 1=enable ext. trigger source */
#define PCI171X_CTRL_PACER	BIT(1)	/* 1=enable int. 8254 trigger source */
#define PCI171X_CTRL_SW		BIT(0)	/* 1=enable software trigger source */
#define PCI171X_CLRINT_REG	0x08	/* W:   clear interrupts request */
#define PCI171X_CLRFIFO_REG	0x09	/* W:   clear FIFO */
#define PCI171X_DA_REG(x)	(0x0a + ((x) * 2)) /* W:   D/A register */
#define PCI171X_DAREF_REG	0x0e	/* W:   D/A reference control */
#define PCI171X_DAREF(c, r)	(((r) & 0x3) << ((c) * 2))
#define PCI171X_DAREF_MASK(c)	PCI171X_DAREF((c), 0x3)
#define PCI171X_DI_REG		0x10	/* R:   digital inputs */
#define PCI171X_DO_REG		0x10	/* W:   digital outputs */
#define PCI171X_TIMER_BASE	0x18	/* R/W: 8254 timer */

static const struct comedi_lrange pci1710_ai_range = {
	9, {
		BIP_RANGE(5),		/* gain 1   (0x00) */
		BIP_RANGE(2.5),		/* gain 2   (0x01) */
		BIP_RANGE(1.25),	/* gain 4   (0x02) */
		BIP_RANGE(0.625),	/* gain 8   (0x03) */
		BIP_RANGE(10),		/* gain 0.5 (0x04) */
		UNI_RANGE(10),		/* gain 1   (0x00 | UNI) */
		UNI_RANGE(5),		/* gain 2   (0x01 | UNI) */
		UNI_RANGE(2.5),		/* gain 4   (0x02 | UNI) */
		UNI_RANGE(1.25)		/* gain 8   (0x03 | UNI) */
	}
};

static const struct comedi_lrange pci1710hg_ai_range = {
	12, {
		BIP_RANGE(5),		/* gain 1    (0x00) */
		BIP_RANGE(0.5),		/* gain 10   (0x01) */
		BIP_RANGE(0.05),	/* gain 100  (0x02) */
		BIP_RANGE(0.005),	/* gain 1000 (0x03) */
		BIP_RANGE(10),		/* gain 0.5  (0x04) */
		BIP_RANGE(1),		/* gain 5    (0x05) */
		BIP_RANGE(0.1),		/* gain 50   (0x06) */
		BIP_RANGE(0.01),	/* gain 500  (0x07) */
		UNI_RANGE(10),		/* gain 1    (0x00 | UNI) */
		UNI_RANGE(1),		/* gain 10   (0x01 | UNI) */
		UNI_RANGE(0.1),		/* gain 100  (0x02 | UNI) */
		UNI_RANGE(0.01)		/* gain 1000 (0x03 | UNI) */
	}
};

static const struct comedi_lrange pci1711_ai_range = {
	5, {
		BIP_RANGE(10),		/* gain 1  (0x00) */
		BIP_RANGE(5),		/* gain 2  (0x01) */
		BIP_RANGE(2.5),		/* gain 4  (0x02) */
		BIP_RANGE(1.25),	/* gain 8  (0x03) */
		BIP_RANGE(0.625)	/* gain 16 (0x04) */
	}
};

static const struct comedi_lrange pci171x_ao_range = {
	3, {
		UNI_RANGE(5),		/* internal -5V ref */
		UNI_RANGE(10),		/* internal -10V ref */
		RANGE_ext(0, 1)		/* external -Vref (+/-10V max) */
>>>>>>> v4.9.227
	}
};

enum pci1710_boardid {
	BOARD_PCI1710,
	BOARD_PCI1710HG,
	BOARD_PCI1711,
	BOARD_PCI1713,
<<<<<<< HEAD
	BOARD_PCI1720,
=======
>>>>>>> v4.9.227
	BOARD_PCI1731,
};

struct boardtype {
<<<<<<< HEAD
	const char *name;	/*  board name */
	char have_irq;		/*  1=card support IRQ */
	char cardtype;		/*  0=1710& co. 2=1713, ... */
	int n_aichan;		/*  num of A/D chans */
	int n_aichand;		/*  num of A/D chans in diff mode */
	int n_aochan;		/*  num of D/A chans */
	int n_dichan;		/*  num of DI chans */
	int n_dochan;		/*  num of DO chans */
	int n_counter;		/*  num of counters */
	int ai_maxdata;		/*  resolution of A/D */
	int ao_maxdata;		/*  resolution of D/A */
	const struct comedi_lrange *rangelist_ai;	/*  rangelist for A/D */
	const char *rangecode_ai;	/*  range codes for programming */
	const struct comedi_lrange *rangelist_ao;	/*  rangelist for D/A */
	unsigned int ai_ns_min;	/*  max sample speed of card v ns */
	unsigned int fifo_half_size;	/*  size of FIFO/2 */
=======
	const char *name;
	const struct comedi_lrange *ai_range;
	unsigned int is_pci1711:1;
	unsigned int is_pci1713:1;
	unsigned int has_ao:1;
>>>>>>> v4.9.227
};

static const struct boardtype boardtypes[] = {
	[BOARD_PCI1710] = {
		.name		= "pci1710",
<<<<<<< HEAD
		.have_irq	= 1,
		.cardtype	= TYPE_PCI171X,
		.n_aichan	= 16,
		.n_aichand	= 8,
		.n_aochan	= 2,
		.n_dichan	= 16,
		.n_dochan	= 16,
		.n_counter	= 1,
		.ai_maxdata	= 0x0fff,
		.ao_maxdata	= 0x0fff,
		.rangelist_ai	= &range_pci1710_3,
		.rangecode_ai	= range_codes_pci1710_3,
		.rangelist_ao	= &range_pci171x_da,
		.ai_ns_min	= 10000,
		.fifo_half_size	= 2048,
	},
	[BOARD_PCI1710HG] = {
		.name		= "pci1710hg",
		.have_irq	= 1,
		.cardtype	= TYPE_PCI171X,
		.n_aichan	= 16,
		.n_aichand	= 8,
		.n_aochan	= 2,
		.n_dichan	= 16,
		.n_dochan	= 16,
		.n_counter	= 1,
		.ai_maxdata	= 0x0fff,
		.ao_maxdata	= 0x0fff,
		.rangelist_ai	= &range_pci1710hg,
		.rangecode_ai	= range_codes_pci1710hg,
		.rangelist_ao	= &range_pci171x_da,
		.ai_ns_min	= 10000,
		.fifo_half_size	= 2048,
	},
	[BOARD_PCI1711] = {
		.name		= "pci1711",
		.have_irq	= 1,
		.cardtype	= TYPE_PCI171X,
		.n_aichan	= 16,
		.n_aochan	= 2,
		.n_dichan	= 16,
		.n_dochan	= 16,
		.n_counter	= 1,
		.ai_maxdata	= 0x0fff,
		.ao_maxdata	= 0x0fff,
		.rangelist_ai	= &range_pci17x1,
		.rangecode_ai	= range_codes_pci17x1,
		.rangelist_ao	= &range_pci171x_da,
		.ai_ns_min	= 10000,
		.fifo_half_size	= 512,
	},
	[BOARD_PCI1713] = {
		.name		= "pci1713",
		.have_irq	= 1,
		.cardtype	= TYPE_PCI1713,
		.n_aichan	= 32,
		.n_aichand	= 16,
		.ai_maxdata	= 0x0fff,
		.rangelist_ai	= &range_pci1710_3,
		.rangecode_ai	= range_codes_pci1710_3,
		.ai_ns_min	= 10000,
		.fifo_half_size	= 2048,
	},
	[BOARD_PCI1720] = {
		.name		= "pci1720",
		.cardtype	= TYPE_PCI1720,
		.n_aochan	= 4,
		.ao_maxdata	= 0x0fff,
		.rangelist_ao	= &range_pci1720,
	},
	[BOARD_PCI1731] = {
		.name		= "pci1731",
		.have_irq	= 1,
		.cardtype	= TYPE_PCI171X,
		.n_aichan	= 16,
		.n_dichan	= 16,
		.n_dochan	= 16,
		.ai_maxdata	= 0x0fff,
		.rangelist_ai	= &range_pci17x1,
		.rangecode_ai	= range_codes_pci17x1,
		.ai_ns_min	= 10000,
		.fifo_half_size	= 512,
=======
		.ai_range	= &pci1710_ai_range,
		.has_ao		= 1,
	},
	[BOARD_PCI1710HG] = {
		.name		= "pci1710hg",
		.ai_range	= &pci1710hg_ai_range,
		.has_ao		= 1,
	},
	[BOARD_PCI1711] = {
		.name		= "pci1711",
		.ai_range	= &pci1711_ai_range,
		.is_pci1711	= 1,
		.has_ao		= 1,
	},
	[BOARD_PCI1713] = {
		.name		= "pci1713",
		.ai_range	= &pci1710_ai_range,
		.is_pci1713	= 1,
	},
	[BOARD_PCI1731] = {
		.name		= "pci1731",
		.ai_range	= &pci1711_ai_range,
		.is_pci1711	= 1,
>>>>>>> v4.9.227
	},
};

struct pci1710_private {
<<<<<<< HEAD
	unsigned int CntrlReg;	/*  Control register */
	unsigned int ai_act_scan;	/*  how many scans we finished */
	unsigned char ai_et;
	unsigned int ai_et_CntrlReg;
	unsigned int ai_et_MuxVal;
	unsigned int next_divisor1;
	unsigned int next_divisor2;
	unsigned int divisor1;
	unsigned int divisor2;
	unsigned int act_chanlist[32];	/*  list of scanned channel */
	unsigned char saved_seglen;	/* len of the non-repeating chanlist */
	unsigned char da_ranges;	/*  copy of D/A outpit range register */
	unsigned short ao_data[4];	/*  data output buffer */
	unsigned int cnt0_write_wait;	/* after a write, wait for update of the
					 * internal state */
};

/*  used for gain list programming */
static const unsigned int muxonechan[] = {
	0x0000, 0x0101, 0x0202, 0x0303, 0x0404, 0x0505, 0x0606, 0x0707,
	0x0808, 0x0909, 0x0a0a, 0x0b0b, 0x0c0c, 0x0d0d, 0x0e0e, 0x0f0f,
	0x1010, 0x1111, 0x1212, 0x1313, 0x1414, 0x1515, 0x1616, 0x1717,
	0x1818, 0x1919, 0x1a1a, 0x1b1b, 0x1c1c, 0x1d1d, 0x1e1e, 0x1f1f
};

static int pci171x_ai_dropout(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      unsigned int chan,
			      unsigned int val)
{
	const struct boardtype *board = dev->board_ptr;
	struct pci1710_private *devpriv = dev->private;

	if (board->cardtype != TYPE_PCI1713) {
		if ((val & 0xf000) != devpriv->act_chanlist[chan]) {
			dev_err(dev->class_dev,
				"A/D data droput: received from channel %d, expected %d\n",
				(val >> 12) & 0xf,
				(devpriv->act_chanlist[chan] >> 12) & 0xf);
			return -ENODATA;
		}
	}
	return 0;
}

static int pci171x_ai_check_chanlist(struct comedi_device *dev,
=======
	unsigned int max_samples;
	unsigned int ctrl;	/* control register value */
	unsigned int ctrl_ext;	/* used to switch from TRIG_EXT to TRIG_xxx */
	unsigned int mux_scan;	/* used to set the channel interval to scan */
	unsigned char ai_et;
	unsigned int act_chanlist[32];	/*  list of scanned channel */
	unsigned char saved_seglen;	/* len of the non-repeating chanlist */
	unsigned char da_ranges;	/*  copy of D/A outpit range register */
	unsigned char unipolar_gain;	/* adjust for unipolar gain codes */
};

static int pci1710_ai_check_chanlist(struct comedi_device *dev,
>>>>>>> v4.9.227
				     struct comedi_subdevice *s,
				     struct comedi_cmd *cmd)
{
	struct pci1710_private *devpriv = dev->private;
	unsigned int chan0 = CR_CHAN(cmd->chanlist[0]);
	unsigned int last_aref = CR_AREF(cmd->chanlist[0]);
	unsigned int next_chan = (chan0 + 1) % s->n_chan;
	unsigned int chansegment[32];
	unsigned int seglen;
	int i;

	if (cmd->chanlist_len == 1) {
		devpriv->saved_seglen = cmd->chanlist_len;
		return 0;
	}

	/* first channel is always ok */
	chansegment[0] = cmd->chanlist[0];

	for (i = 1; i < cmd->chanlist_len; i++) {
		unsigned int chan = CR_CHAN(cmd->chanlist[i]);
		unsigned int aref = CR_AREF(cmd->chanlist[i]);

		if (cmd->chanlist[0] == cmd->chanlist[i])
			break;	/*  we detected a loop, stop */

		if (aref == AREF_DIFF && (chan & 1)) {
			dev_err(dev->class_dev,
				"Odd channel cannot be differential input!\n");
			return -EINVAL;
		}

		if (last_aref == AREF_DIFF)
			next_chan = (next_chan + 1) % s->n_chan;
		if (chan != next_chan) {
			dev_err(dev->class_dev,
				"channel list must be continuous! chanlist[%i]=%d but must be %d or %d!\n",
				i, chan, next_chan, chan0);
			return -EINVAL;
		}

		/* next correct channel in list */
		chansegment[i] = cmd->chanlist[i];
		last_aref = aref;
	}
	seglen = i;

	for (i = 0; i < cmd->chanlist_len; i++) {
		if (cmd->chanlist[i] != chansegment[i % seglen]) {
			dev_err(dev->class_dev,
				"bad channel, reference or range number! chanlist[%i]=%d,%d,%d and not %d,%d,%d!\n",
				i, CR_CHAN(chansegment[i]),
				CR_RANGE(chansegment[i]),
				CR_AREF(chansegment[i]),
				CR_CHAN(cmd->chanlist[i % seglen]),
				CR_RANGE(cmd->chanlist[i % seglen]),
				CR_AREF(chansegment[i % seglen]));
			return -EINVAL;
		}
	}
	devpriv->saved_seglen = seglen;

	return 0;
}

<<<<<<< HEAD
static void setup_channel_list(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       unsigned int *chanlist, unsigned int n_chan,
			       unsigned int seglen)
{
	const struct boardtype *this_board = dev->board_ptr;
	struct pci1710_private *devpriv = dev->private;
	unsigned int i, range, chanprog;

	for (i = 0; i < seglen; i++) {	/*  store range list to card */
		chanprog = muxonechan[CR_CHAN(chanlist[i])];
		outw(chanprog, dev->iobase + PCI171x_MUX); /* select channel */
		range = this_board->rangecode_ai[CR_RANGE(chanlist[i])];
		if (CR_AREF(chanlist[i]) == AREF_DIFF)
			range |= 0x0020;
		outw(range, dev->iobase + PCI171x_RANGE); /* select gain */
		devpriv->act_chanlist[i] =
			(CR_CHAN(chanlist[i]) << 12) & 0xf000;
	}
	for ( ; i < n_chan; i++) { /* store remainder of channel list */
		devpriv->act_chanlist[i] =
			(CR_CHAN(chanlist[i]) << 12) & 0xf000;
	}

	devpriv->ai_et_MuxVal =
		CR_CHAN(chanlist[0]) | (CR_CHAN(chanlist[seglen - 1]) << 8);
	/* select channel interval to scan */
	outw(devpriv->ai_et_MuxVal, dev->iobase + PCI171x_MUX);
}

static int pci171x_ai_eoc(struct comedi_device *dev,
=======
static void pci1710_ai_setup_chanlist(struct comedi_device *dev,
				      struct comedi_subdevice *s,
				      unsigned int *chanlist,
				      unsigned int n_chan,
				      unsigned int seglen)
{
	struct pci1710_private *devpriv = dev->private;
	unsigned int first_chan = CR_CHAN(chanlist[0]);
	unsigned int last_chan = CR_CHAN(chanlist[seglen - 1]);
	unsigned int i;

	for (i = 0; i < seglen; i++) {	/*  store range list to card */
		unsigned int chan = CR_CHAN(chanlist[i]);
		unsigned int range = CR_RANGE(chanlist[i]);
		unsigned int aref = CR_AREF(chanlist[i]);
		unsigned int rangeval = 0;

		if (aref == AREF_DIFF)
			rangeval |= PCI171X_RANGE_DIFF;
		if (comedi_range_is_unipolar(s, range)) {
			rangeval |= PCI171X_RANGE_UNI;
			range -= devpriv->unipolar_gain;
		}
		rangeval |= PCI171X_RANGE_GAIN(range);

		/* select channel and set range */
		outw(PCI171X_MUX_CHAN(chan), dev->iobase + PCI171X_MUX_REG);
		outw(rangeval, dev->iobase + PCI171X_RANGE_REG);

		devpriv->act_chanlist[i] = chan;
	}
	for ( ; i < n_chan; i++)	/* store remainder of channel list */
		devpriv->act_chanlist[i] = CR_CHAN(chanlist[i]);

	/* select channel interval to scan */
	devpriv->mux_scan = PCI171X_MUX_CHANL(first_chan) |
			    PCI171X_MUX_CHANH(last_chan);
	outw(devpriv->mux_scan, dev->iobase + PCI171X_MUX_REG);
}

static int pci1710_ai_eoc(struct comedi_device *dev,
>>>>>>> v4.9.227
			  struct comedi_subdevice *s,
			  struct comedi_insn *insn,
			  unsigned long context)
{
	unsigned int status;

<<<<<<< HEAD
	status = inw(dev->iobase + PCI171x_STATUS);
	if ((status & Status_FE) == 0)
=======
	status = inw(dev->iobase + PCI171X_STATUS_REG);
	if ((status & PCI171X_STATUS_FE) == 0)
>>>>>>> v4.9.227
		return 0;
	return -EBUSY;
}

<<<<<<< HEAD
static int pci171x_insn_read_ai(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn, unsigned int *data)
=======
static int pci1710_ai_read_sample(struct comedi_device *dev,
				  struct comedi_subdevice *s,
				  unsigned int cur_chan,
				  unsigned int *val)
{
	const struct boardtype *board = dev->board_ptr;
	struct pci1710_private *devpriv = dev->private;
	unsigned int sample;
	unsigned int chan;

	sample = inw(dev->iobase + PCI171X_AD_DATA_REG);
	if (!board->is_pci1713) {
		/*
		 * The upper 4 bits of the 16-bit sample are the channel number
		 * that the sample was acquired from. Verify that this channel
		 * number matches the expected channel number.
		 */
		chan = sample >> 12;
		if (chan != devpriv->act_chanlist[cur_chan]) {
			dev_err(dev->class_dev,
				"A/D data droput: received from channel %d, expected %d\n",
				chan, devpriv->act_chanlist[cur_chan]);
			return -ENODATA;
		}
	}
	*val = sample & s->maxdata;
	return 0;
}

static int pci1710_ai_insn_read(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn,
				unsigned int *data)
>>>>>>> v4.9.227
{
	struct pci1710_private *devpriv = dev->private;
	int ret = 0;
	int i;

<<<<<<< HEAD
	devpriv->CntrlReg &= Control_CNT0;
	devpriv->CntrlReg |= Control_SW;	/*  set software trigger */
	outw(devpriv->CntrlReg, dev->iobase + PCI171x_CONTROL);
	outb(0, dev->iobase + PCI171x_CLRFIFO);
	outb(0, dev->iobase + PCI171x_CLRINT);

	setup_channel_list(dev, s, &insn->chanspec, 1, 1);
=======
	/* enable software trigger */
	devpriv->ctrl |= PCI171X_CTRL_SW;
	outw(devpriv->ctrl, dev->iobase + PCI171X_CTRL_REG);

	outb(0, dev->iobase + PCI171X_CLRFIFO_REG);
	outb(0, dev->iobase + PCI171X_CLRINT_REG);

	pci1710_ai_setup_chanlist(dev, s, &insn->chanspec, 1, 1);
>>>>>>> v4.9.227

	for (i = 0; i < insn->n; i++) {
		unsigned int val;

<<<<<<< HEAD
		outw(0, dev->iobase + PCI171x_SOFTTRG);	/* start conversion */

		ret = comedi_timeout(dev, s, insn, pci171x_ai_eoc, 0);
		if (ret)
			break;

		val = inw(dev->iobase + PCI171x_AD_DATA);
		ret = pci171x_ai_dropout(dev, s, 0, val);
		if (ret)
			break;

		data[i] = val & s->maxdata;
	}

	outb(0, dev->iobase + PCI171x_CLRFIFO);
	outb(0, dev->iobase + PCI171x_CLRINT);
=======
		/* start conversion */
		outw(0, dev->iobase + PCI171X_SOFTTRG_REG);

		ret = comedi_timeout(dev, s, insn, pci1710_ai_eoc, 0);
		if (ret)
			break;

		ret = pci1710_ai_read_sample(dev, s, 0, &val);
		if (ret)
			break;

		data[i] = val;
	}

	/* disable software trigger */
	devpriv->ctrl &= ~PCI171X_CTRL_SW;
	outw(devpriv->ctrl, dev->iobase + PCI171X_CTRL_REG);

	outb(0, dev->iobase + PCI171X_CLRFIFO_REG);
	outb(0, dev->iobase + PCI171X_CLRINT_REG);
>>>>>>> v4.9.227

	return ret ? ret : insn->n;
}

<<<<<<< HEAD
/*
==============================================================================
*/
static int pci171x_insn_write_ao(struct comedi_device *dev,
				 struct comedi_subdevice *s,
				 struct comedi_insn *insn, unsigned int *data)
{
	struct pci1710_private *devpriv = dev->private;
	unsigned int val;
	int n, chan, range, ofs;

	chan = CR_CHAN(insn->chanspec);
	range = CR_RANGE(insn->chanspec);
	if (chan) {
		devpriv->da_ranges &= 0xfb;
		devpriv->da_ranges |= (range << 2);
		outw(devpriv->da_ranges, dev->iobase + PCI171x_DAREF);
		ofs = PCI171x_DA2;
	} else {
		devpriv->da_ranges &= 0xfe;
		devpriv->da_ranges |= range;
		outw(devpriv->da_ranges, dev->iobase + PCI171x_DAREF);
		ofs = PCI171x_DA1;
	}
	val = devpriv->ao_data[chan];

	for (n = 0; n < insn->n; n++) {
		val = data[n];
		outw(val, dev->iobase + ofs);
	}

	devpriv->ao_data[chan] = val;

	return n;

}

/*
==============================================================================
*/
static int pci171x_insn_read_ao(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn, unsigned int *data)
{
	struct pci1710_private *devpriv = dev->private;
	int n, chan;

	chan = CR_CHAN(insn->chanspec);
	for (n = 0; n < insn->n; n++)
		data[n] = devpriv->ao_data[chan];

	return n;
}

/*
==============================================================================
*/
static int pci171x_insn_bits_di(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn, unsigned int *data)
{
	data[1] = inw(dev->iobase + PCI171x_DI);

	return insn->n;
}

static int pci171x_insn_bits_do(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn,
				unsigned int *data)
{
	if (comedi_dio_update_state(s, data))
		outw(s->state, dev->iobase + PCI171x_DO);

	data[1] = s->state;

	return insn->n;
}

static void pci171x_start_pacer(struct comedi_device *dev,
				bool load_counters)
{
	struct pci1710_private *devpriv = dev->private;
	unsigned long timer_base = dev->iobase + PCI171X_TIMER_BASE;

	i8254_set_mode(timer_base, 1, 2, I8254_MODE2 | I8254_BINARY);
	i8254_set_mode(timer_base, 1, 1, I8254_MODE2 | I8254_BINARY);

	if (load_counters) {
		i8254_write(timer_base, 1, 2, devpriv->divisor2);
		i8254_write(timer_base, 1, 1, devpriv->divisor1);
	}
}

/*
==============================================================================
*/
static int pci171x_insn_counter_read(struct comedi_device *dev,
				     struct comedi_subdevice *s,
				     struct comedi_insn *insn,
				     unsigned int *data)
{
	unsigned int msb, lsb, ccntrl;
	int i;

	ccntrl = 0xD2;		/* count only */
	for (i = 0; i < insn->n; i++) {
		outw(ccntrl, dev->iobase + PCI171x_CNTCTRL);

		lsb = inw(dev->iobase + PCI171x_CNT0) & 0xFF;
		msb = inw(dev->iobase + PCI171x_CNT0) & 0xFF;

		data[0] = lsb | (msb << 8);
	}

	return insn->n;
}

/*
==============================================================================
*/
static int pci171x_insn_counter_write(struct comedi_device *dev,
				      struct comedi_subdevice *s,
				      struct comedi_insn *insn,
				      unsigned int *data)
{
	struct pci1710_private *devpriv = dev->private;
	uint msb, lsb, ccntrl, status;

	lsb = data[0] & 0x00FF;
	msb = (data[0] & 0xFF00) >> 8;

	/* write lsb, then msb */
	outw(lsb, dev->iobase + PCI171x_CNT0);
	outw(msb, dev->iobase + PCI171x_CNT0);

	if (devpriv->cnt0_write_wait) {
		/* wait for the new count to be loaded */
		ccntrl = 0xE2;
		do {
			outw(ccntrl, dev->iobase + PCI171x_CNTCTRL);
			status = inw(dev->iobase + PCI171x_CNT0) & 0xFF;
		} while (status & 0x40);
	}

	return insn->n;
}

/*
==============================================================================
*/
static int pci171x_insn_counter_config(struct comedi_device *dev,
				       struct comedi_subdevice *s,
				       struct comedi_insn *insn,
				       unsigned int *data)
{
#ifdef unused
	/* This doesn't work like a normal Comedi counter config */
	struct pci1710_private *devpriv = dev->private;
	uint ccntrl = 0;

	devpriv->cnt0_write_wait = data[0] & 0x20;

	/* internal or external clock? */
	if (!(data[0] & 0x10)) {	/* internal */
		devpriv->CntrlReg &= ~Control_CNT0;
	} else {
		devpriv->CntrlReg |= Control_CNT0;
	}
	outw(devpriv->CntrlReg, dev->iobase + PCI171x_CONTROL);

	if (data[0] & 0x01)
		ccntrl |= Counter_M0;
	if (data[0] & 0x02)
		ccntrl |= Counter_M1;
	if (data[0] & 0x04)
		ccntrl |= Counter_M2;
	if (data[0] & 0x08)
		ccntrl |= Counter_BCD;
	ccntrl |= Counter_RW0;	/* set read/write mode */
	ccntrl |= Counter_RW1;
	outw(ccntrl, dev->iobase + PCI171x_CNTCTRL);
#endif

	return 1;
}

/*
==============================================================================
*/
static int pci1720_insn_write_ao(struct comedi_device *dev,
				 struct comedi_subdevice *s,
				 struct comedi_insn *insn, unsigned int *data)
{
	struct pci1710_private *devpriv = dev->private;
	unsigned int val;
	int n, rangereg, chan;

	chan = CR_CHAN(insn->chanspec);
	rangereg = devpriv->da_ranges & (~(0x03 << (chan << 1)));
	rangereg |= (CR_RANGE(insn->chanspec) << (chan << 1));
	if (rangereg != devpriv->da_ranges) {
		outb(rangereg, dev->iobase + PCI1720_RANGE);
		devpriv->da_ranges = rangereg;
	}
	val = devpriv->ao_data[chan];

	for (n = 0; n < insn->n; n++) {
		val = data[n];
		outw(val, dev->iobase + PCI1720_DA0 + (chan << 1));
		outb(0, dev->iobase + PCI1720_SYNCOUT);	/*  update outputs */
	}

	devpriv->ao_data[chan] = val;

	return n;
}

/*
==============================================================================
*/
static int pci171x_ai_cancel(struct comedi_device *dev,
			     struct comedi_subdevice *s)
{
	const struct boardtype *this_board = dev->board_ptr;
	struct pci1710_private *devpriv = dev->private;

	switch (this_board->cardtype) {
	default:
		devpriv->CntrlReg &= Control_CNT0;
		devpriv->CntrlReg |= Control_SW;
		/* reset any operations */
		outw(devpriv->CntrlReg, dev->iobase + PCI171x_CONTROL);
		pci171x_start_pacer(dev, false);
		outb(0, dev->iobase + PCI171x_CLRFIFO);
		outb(0, dev->iobase + PCI171x_CLRINT);
		break;
	}

	devpriv->ai_act_scan = 0;
	s->async->cur_chan = 0;
=======
static int pci1710_ai_cancel(struct comedi_device *dev,
			     struct comedi_subdevice *s)
{
	struct pci1710_private *devpriv = dev->private;

	/* disable A/D triggers and interrupt sources */
	devpriv->ctrl &= PCI171X_CTRL_CNT0;	/* preserve counter 0 clk src */
	outw(devpriv->ctrl, dev->iobase + PCI171X_CTRL_REG);

	/* disable pacer */
	comedi_8254_pacer_enable(dev->pacer, 1, 2, false);

	/* clear A/D FIFO and any pending interrutps */
	outb(0, dev->iobase + PCI171X_CLRFIFO_REG);
	outb(0, dev->iobase + PCI171X_CLRINT_REG);
>>>>>>> v4.9.227

	return 0;
}

static void pci1710_handle_every_sample(struct comedi_device *dev,
					struct comedi_subdevice *s)
{
<<<<<<< HEAD
	struct pci1710_private *devpriv = dev->private;
=======
>>>>>>> v4.9.227
	struct comedi_cmd *cmd = &s->async->cmd;
	unsigned int status;
	unsigned int val;
	int ret;

<<<<<<< HEAD
	status = inw(dev->iobase + PCI171x_STATUS);
	if (status & Status_FE) {
		dev_dbg(dev->class_dev, "A/D FIFO empty (%4x)\n", status);
		s->async->events |= COMEDI_CB_EOA | COMEDI_CB_ERROR;
		cfc_handle_events(dev, s);
		return;
	}
	if (status & Status_FF) {
		dev_dbg(dev->class_dev,
			"A/D FIFO Full status (Fatal Error!) (%4x)\n", status);
		s->async->events |= COMEDI_CB_EOA | COMEDI_CB_ERROR;
		cfc_handle_events(dev, s);
		return;
	}

	outb(0, dev->iobase + PCI171x_CLRINT);	/*  clear our INT request */

	for (; !(inw(dev->iobase + PCI171x_STATUS) & Status_FE);) {
		val = inw(dev->iobase + PCI171x_AD_DATA);
		ret = pci171x_ai_dropout(dev, s, s->async->cur_chan, val);
		if (ret) {
			s->async->events |= COMEDI_CB_EOA | COMEDI_CB_ERROR;
			break;
		}

		comedi_buf_put(s, val & s->maxdata);

		s->async->cur_chan++;
		if (s->async->cur_chan >= cmd->chanlist_len)
			s->async->cur_chan = 0;


		if (s->async->cur_chan == 0) {	/*  one scan done */
			devpriv->ai_act_scan++;
			if (cmd->stop_src == TRIG_COUNT &&
			    devpriv->ai_act_scan >= cmd->stop_arg) {
				/*  all data sampled */
				s->async->events |= COMEDI_CB_EOA;
				break;
			}
		}
	}

	outb(0, dev->iobase + PCI171x_CLRINT);	/*  clear our INT request */

	cfc_handle_events(dev, s);
}

/*
==============================================================================
*/
static int move_block_from_fifo(struct comedi_device *dev,
				struct comedi_subdevice *s, int n, int turn)
{
	struct pci1710_private *devpriv = dev->private;
	struct comedi_cmd *cmd = &s->async->cmd;
	unsigned int val;
	int ret;
	int i;

	for (i = 0; i < n; i++) {
		val = inw(dev->iobase + PCI171x_AD_DATA);

		ret = pci171x_ai_dropout(dev, s, s->async->cur_chan, val);
		if (ret) {
			s->async->events |= COMEDI_CB_EOA | COMEDI_CB_ERROR;
			return ret;
		}

		comedi_buf_put(s, val & s->maxdata);

		s->async->cur_chan++;
		if (s->async->cur_chan >= cmd->chanlist_len) {
			s->async->cur_chan = 0;
			devpriv->ai_act_scan++;
		}
	}
	return 0;
=======
	status = inw(dev->iobase + PCI171X_STATUS_REG);
	if (status & PCI171X_STATUS_FE) {
		dev_dbg(dev->class_dev, "A/D FIFO empty (%4x)\n", status);
		s->async->events |= COMEDI_CB_ERROR;
		return;
	}
	if (status & PCI171X_STATUS_FF) {
		dev_dbg(dev->class_dev,
			"A/D FIFO Full status (Fatal Error!) (%4x)\n", status);
		s->async->events |= COMEDI_CB_ERROR;
		return;
	}

	outb(0, dev->iobase + PCI171X_CLRINT_REG);

	for (; !(inw(dev->iobase + PCI171X_STATUS_REG) & PCI171X_STATUS_FE);) {
		ret = pci1710_ai_read_sample(dev, s, s->async->cur_chan, &val);
		if (ret) {
			s->async->events |= COMEDI_CB_ERROR;
			break;
		}

		comedi_buf_write_samples(s, &val, 1);

		if (cmd->stop_src == TRIG_COUNT &&
		    s->async->scans_done >= cmd->stop_arg) {
			s->async->events |= COMEDI_CB_EOA;
			break;
		}
	}

	outb(0, dev->iobase + PCI171X_CLRINT_REG);
>>>>>>> v4.9.227
}

static void pci1710_handle_fifo(struct comedi_device *dev,
				struct comedi_subdevice *s)
{
<<<<<<< HEAD
	const struct boardtype *this_board = dev->board_ptr;
	struct pci1710_private *devpriv = dev->private;
	struct comedi_cmd *cmd = &s->async->cmd;
	int m, samplesinbuf;

	m = inw(dev->iobase + PCI171x_STATUS);
	if (!(m & Status_FH)) {
		dev_dbg(dev->class_dev, "A/D FIFO not half full! (%4x)\n", m);
		s->async->events |= COMEDI_CB_EOA | COMEDI_CB_ERROR;
		cfc_handle_events(dev, s);
		return;
	}
	if (m & Status_FF) {
		dev_dbg(dev->class_dev,
			"A/D FIFO Full status (Fatal Error!) (%4x)\n", m);
		s->async->events |= COMEDI_CB_EOA | COMEDI_CB_ERROR;
		cfc_handle_events(dev, s);
		return;
	}

	samplesinbuf = this_board->fifo_half_size;
	if (samplesinbuf * sizeof(short) >= s->async->prealloc_bufsz) {
		m = s->async->prealloc_bufsz / sizeof(short);
		if (move_block_from_fifo(dev, s, m, 0))
			return;
		samplesinbuf -= m;
	}

	if (samplesinbuf) {
		if (move_block_from_fifo(dev, s, samplesinbuf, 1))
			return;
	}

	if (cmd->stop_src == TRIG_COUNT &&
	    devpriv->ai_act_scan >= cmd->stop_arg) {
		/* all data sampled */
		s->async->events |= COMEDI_CB_EOA;
		cfc_handle_events(dev, s);
		return;
	}
	outb(0, dev->iobase + PCI171x_CLRINT);	/*  clear our INT request */

	cfc_handle_events(dev, s);
}

/*
==============================================================================
*/
static irqreturn_t interrupt_service_pci1710(int irq, void *d)
=======
	struct pci1710_private *devpriv = dev->private;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &async->cmd;
	unsigned int status;
	int i;

	status = inw(dev->iobase + PCI171X_STATUS_REG);
	if (!(status & PCI171X_STATUS_FH)) {
		dev_dbg(dev->class_dev, "A/D FIFO not half full!\n");
		async->events |= COMEDI_CB_ERROR;
		return;
	}
	if (status & PCI171X_STATUS_FF) {
		dev_dbg(dev->class_dev,
			"A/D FIFO Full status (Fatal Error!)\n");
		async->events |= COMEDI_CB_ERROR;
		return;
	}

	for (i = 0; i < devpriv->max_samples; i++) {
		unsigned int val;
		int ret;

		ret = pci1710_ai_read_sample(dev, s, s->async->cur_chan, &val);
		if (ret) {
			s->async->events |= COMEDI_CB_ERROR;
			break;
		}

		if (!comedi_buf_write_samples(s, &val, 1))
			break;

		if (cmd->stop_src == TRIG_COUNT &&
		    async->scans_done >= cmd->stop_arg) {
			async->events |= COMEDI_CB_EOA;
			break;
		}
	}

	outb(0, dev->iobase + PCI171X_CLRINT_REG);
}

static irqreturn_t pci1710_irq_handler(int irq, void *d)
>>>>>>> v4.9.227
{
	struct comedi_device *dev = d;
	struct pci1710_private *devpriv = dev->private;
	struct comedi_subdevice *s;
	struct comedi_cmd *cmd;

	if (!dev->attached)	/*  is device attached? */
		return IRQ_NONE;	/*  no, exit */

	s = dev->read_subdev;
	cmd = &s->async->cmd;

	/*  is this interrupt from our board? */
<<<<<<< HEAD
	if (!(inw(dev->iobase + PCI171x_STATUS) & Status_IRQ))
=======
	if (!(inw(dev->iobase + PCI171X_STATUS_REG) & PCI171X_STATUS_IRQ))
>>>>>>> v4.9.227
		return IRQ_NONE;	/*  no, exit */

	if (devpriv->ai_et) {	/*  Switch from initial TRIG_EXT to TRIG_xxx. */
		devpriv->ai_et = 0;
<<<<<<< HEAD
		devpriv->CntrlReg &= Control_CNT0;
		devpriv->CntrlReg |= Control_SW; /* set software trigger */
		outw(devpriv->CntrlReg, dev->iobase + PCI171x_CONTROL);
		devpriv->CntrlReg = devpriv->ai_et_CntrlReg;
		outb(0, dev->iobase + PCI171x_CLRFIFO);
		outb(0, dev->iobase + PCI171x_CLRINT);
		outw(devpriv->ai_et_MuxVal, dev->iobase + PCI171x_MUX);
		outw(devpriv->CntrlReg, dev->iobase + PCI171x_CONTROL);
		pci171x_start_pacer(dev, true);
=======
		devpriv->ctrl &= PCI171X_CTRL_CNT0;
		devpriv->ctrl |= PCI171X_CTRL_SW; /* set software trigger */
		outw(devpriv->ctrl, dev->iobase + PCI171X_CTRL_REG);
		devpriv->ctrl = devpriv->ctrl_ext;
		outb(0, dev->iobase + PCI171X_CLRFIFO_REG);
		outb(0, dev->iobase + PCI171X_CLRINT_REG);
		/* no sample on this interrupt; reset the channel interval */
		outw(devpriv->mux_scan, dev->iobase + PCI171X_MUX_REG);
		outw(devpriv->ctrl, dev->iobase + PCI171X_CTRL_REG);
		comedi_8254_pacer_enable(dev->pacer, 1, 2, true);
>>>>>>> v4.9.227
		return IRQ_HANDLED;
	}

	if (cmd->flags & CMDF_WAKE_EOS)
		pci1710_handle_every_sample(dev, s);
	else
		pci1710_handle_fifo(dev, s);

<<<<<<< HEAD
	return IRQ_HANDLED;
}

static int pci171x_ai_cmd(struct comedi_device *dev, struct comedi_subdevice *s)
=======
	comedi_handle_events(dev, s);

	return IRQ_HANDLED;
}

static int pci1710_ai_cmd(struct comedi_device *dev, struct comedi_subdevice *s)
>>>>>>> v4.9.227
{
	struct pci1710_private *devpriv = dev->private;
	struct comedi_cmd *cmd = &s->async->cmd;

<<<<<<< HEAD
	pci171x_start_pacer(dev, false);

	setup_channel_list(dev, s, cmd->chanlist, cmd->chanlist_len,
			   devpriv->saved_seglen);

	outb(0, dev->iobase + PCI171x_CLRFIFO);
	outb(0, dev->iobase + PCI171x_CLRINT);

	devpriv->ai_act_scan = 0;
	s->async->cur_chan = 0;

	devpriv->CntrlReg &= Control_CNT0;
	if ((cmd->flags & CMDF_WAKE_EOS) == 0)
		devpriv->CntrlReg |= Control_ONEFH;

	devpriv->divisor1 = devpriv->next_divisor1;
	devpriv->divisor2 = devpriv->next_divisor2;

	if (cmd->convert_src == TRIG_TIMER) {
		devpriv->CntrlReg |= Control_PACER | Control_IRQEN;
		if (cmd->start_src == TRIG_EXT) {
			devpriv->ai_et_CntrlReg = devpriv->CntrlReg;
			devpriv->CntrlReg &=
			    ~(Control_PACER | Control_ONEFH | Control_GATE);
			devpriv->CntrlReg |= Control_EXT;
=======
	pci1710_ai_setup_chanlist(dev, s, cmd->chanlist, cmd->chanlist_len,
				  devpriv->saved_seglen);

	outb(0, dev->iobase + PCI171X_CLRFIFO_REG);
	outb(0, dev->iobase + PCI171X_CLRINT_REG);

	devpriv->ctrl &= PCI171X_CTRL_CNT0;
	if ((cmd->flags & CMDF_WAKE_EOS) == 0)
		devpriv->ctrl |= PCI171X_CTRL_ONEFH;

	if (cmd->convert_src == TRIG_TIMER) {
		comedi_8254_update_divisors(dev->pacer);

		devpriv->ctrl |= PCI171X_CTRL_PACER | PCI171X_CTRL_IRQEN;
		if (cmd->start_src == TRIG_EXT) {
			devpriv->ctrl_ext = devpriv->ctrl;
			devpriv->ctrl &= ~(PCI171X_CTRL_PACER |
					   PCI171X_CTRL_ONEFH |
					   PCI171X_CTRL_GATE);
			devpriv->ctrl |= PCI171X_CTRL_EXT;
>>>>>>> v4.9.227
			devpriv->ai_et = 1;
		} else {	/* TRIG_NOW */
			devpriv->ai_et = 0;
		}
<<<<<<< HEAD
		outw(devpriv->CntrlReg, dev->iobase + PCI171x_CONTROL);

		if (cmd->start_src == TRIG_NOW)
			pci171x_start_pacer(dev, true);
	} else {	/* TRIG_EXT */
		devpriv->CntrlReg |= Control_EXT | Control_IRQEN;
		outw(devpriv->CntrlReg, dev->iobase + PCI171x_CONTROL);
=======
		outw(devpriv->ctrl, dev->iobase + PCI171X_CTRL_REG);

		if (cmd->start_src == TRIG_NOW)
			comedi_8254_pacer_enable(dev->pacer, 1, 2, true);
	} else {	/* TRIG_EXT */
		devpriv->ctrl |= PCI171X_CTRL_EXT | PCI171X_CTRL_IRQEN;
		outw(devpriv->ctrl, dev->iobase + PCI171X_CTRL_REG);
>>>>>>> v4.9.227
	}

	return 0;
}

<<<<<<< HEAD
/*
==============================================================================
*/
static int pci171x_ai_cmdtest(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_cmd *cmd)
{
	const struct boardtype *this_board = dev->board_ptr;
	struct pci1710_private *devpriv = dev->private;
	int err = 0;
	unsigned int arg;

	/* Step 1 : check if triggers are trivially valid */

	err |= cfc_check_trigger_src(&cmd->start_src, TRIG_NOW | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->scan_begin_src, TRIG_FOLLOW);
	err |= cfc_check_trigger_src(&cmd->convert_src, TRIG_TIMER | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->scan_end_src, TRIG_COUNT);
	err |= cfc_check_trigger_src(&cmd->stop_src, TRIG_COUNT | TRIG_NONE);
=======
static int pci1710_ai_cmdtest(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_cmd *cmd)
{
	int err = 0;

	/* Step 1 : check if triggers are trivially valid */

	err |= comedi_check_trigger_src(&cmd->start_src, TRIG_NOW | TRIG_EXT);
	err |= comedi_check_trigger_src(&cmd->scan_begin_src, TRIG_FOLLOW);
	err |= comedi_check_trigger_src(&cmd->convert_src,
					TRIG_TIMER | TRIG_EXT);
	err |= comedi_check_trigger_src(&cmd->scan_end_src, TRIG_COUNT);
	err |= comedi_check_trigger_src(&cmd->stop_src, TRIG_COUNT | TRIG_NONE);
>>>>>>> v4.9.227

	if (err)
		return 1;

	/* step 2a: make sure trigger sources are unique */

<<<<<<< HEAD
	err |= cfc_check_trigger_is_unique(cmd->start_src);
	err |= cfc_check_trigger_is_unique(cmd->convert_src);
	err |= cfc_check_trigger_is_unique(cmd->stop_src);
=======
	err |= comedi_check_trigger_is_unique(cmd->start_src);
	err |= comedi_check_trigger_is_unique(cmd->convert_src);
	err |= comedi_check_trigger_is_unique(cmd->stop_src);
>>>>>>> v4.9.227

	/* step 2b: and mutually compatible */

	if (err)
		return 2;

	/* Step 3: check if arguments are trivially valid */

<<<<<<< HEAD
	err |= cfc_check_trigger_arg_is(&cmd->start_arg, 0);
	err |= cfc_check_trigger_arg_is(&cmd->scan_begin_arg, 0);

	if (cmd->convert_src == TRIG_TIMER)
		err |= cfc_check_trigger_arg_min(&cmd->convert_arg,
						 this_board->ai_ns_min);
	else	/* TRIG_FOLLOW */
		err |= cfc_check_trigger_arg_is(&cmd->convert_arg, 0);

	err |= cfc_check_trigger_arg_is(&cmd->scan_end_arg, cmd->chanlist_len);

	if (cmd->stop_src == TRIG_COUNT)
		err |= cfc_check_trigger_arg_min(&cmd->stop_arg, 1);
	else	/* TRIG_NONE */
		err |= cfc_check_trigger_arg_is(&cmd->stop_arg, 0);
=======
	err |= comedi_check_trigger_arg_is(&cmd->start_arg, 0);
	err |= comedi_check_trigger_arg_is(&cmd->scan_begin_arg, 0);

	if (cmd->convert_src == TRIG_TIMER)
		err |= comedi_check_trigger_arg_min(&cmd->convert_arg, 10000);
	else	/* TRIG_FOLLOW */
		err |= comedi_check_trigger_arg_is(&cmd->convert_arg, 0);

	err |= comedi_check_trigger_arg_is(&cmd->scan_end_arg,
					   cmd->chanlist_len);

	if (cmd->stop_src == TRIG_COUNT)
		err |= comedi_check_trigger_arg_min(&cmd->stop_arg, 1);
	else	/* TRIG_NONE */
		err |= comedi_check_trigger_arg_is(&cmd->stop_arg, 0);
>>>>>>> v4.9.227

	if (err)
		return 3;

	/* step 4: fix up any arguments */

	if (cmd->convert_src == TRIG_TIMER) {
<<<<<<< HEAD
		arg = cmd->convert_arg;
		i8253_cascade_ns_to_timer(I8254_OSC_BASE_10MHZ,
					  &devpriv->next_divisor1,
					  &devpriv->next_divisor2,
					  &arg, cmd->flags);
		err |= cfc_check_trigger_arg_is(&cmd->convert_arg, arg);
=======
		unsigned int arg = cmd->convert_arg;

		comedi_8254_cascade_ns_to_timer(dev->pacer, &arg, cmd->flags);
		err |= comedi_check_trigger_arg_is(&cmd->convert_arg, arg);
>>>>>>> v4.9.227
	}

	if (err)
		return 4;

	/* Step 5: check channel list */

<<<<<<< HEAD
	err |= pci171x_ai_check_chanlist(dev, s, cmd);
=======
	err |= pci1710_ai_check_chanlist(dev, s, cmd);
>>>>>>> v4.9.227

	if (err)
		return 5;

	return 0;
}

<<<<<<< HEAD
/*
==============================================================================
*/
static int pci171x_reset(struct comedi_device *dev)
{
	const struct boardtype *this_board = dev->board_ptr;
	struct pci1710_private *devpriv = dev->private;

	outw(0x30, dev->iobase + PCI171x_CNTCTRL);
	/* Software trigger, CNT0=external */
	devpriv->CntrlReg = Control_SW | Control_CNT0;
	/* reset any operations */
	outw(devpriv->CntrlReg, dev->iobase + PCI171x_CONTROL);
	outb(0, dev->iobase + PCI171x_CLRFIFO);	/*  clear FIFO */
	outb(0, dev->iobase + PCI171x_CLRINT);	/*  clear INT request */
	pci171x_start_pacer(dev, false);
	devpriv->da_ranges = 0;
	if (this_board->n_aochan) {
		/* set DACs to 0..5V */
		outb(devpriv->da_ranges, dev->iobase + PCI171x_DAREF);
		outw(0, dev->iobase + PCI171x_DA1); /* set DA outputs to 0V */
		devpriv->ao_data[0] = 0x0000;
		if (this_board->n_aochan > 1) {
			outw(0, dev->iobase + PCI171x_DA2);
			devpriv->ao_data[1] = 0x0000;
		}
	}
	outw(0, dev->iobase + PCI171x_DO);	/*  digital outputs to 0 */
	outb(0, dev->iobase + PCI171x_CLRFIFO);	/*  clear FIFO */
	outb(0, dev->iobase + PCI171x_CLRINT);	/*  clear INT request */

	return 0;
}

/*
==============================================================================
*/
static int pci1720_reset(struct comedi_device *dev)
{
	struct pci1710_private *devpriv = dev->private;
	/* set synchronous output mode */
	outb(Syncont_SC0, dev->iobase + PCI1720_SYNCONT);
	devpriv->da_ranges = 0xAA;
	/* set all ranges to +/-5V */
	outb(devpriv->da_ranges, dev->iobase + PCI1720_RANGE);
	outw(0x0800, dev->iobase + PCI1720_DA0);	/*  set outputs to 0V */
	outw(0x0800, dev->iobase + PCI1720_DA1);
	outw(0x0800, dev->iobase + PCI1720_DA2);
	outw(0x0800, dev->iobase + PCI1720_DA3);
	outb(0, dev->iobase + PCI1720_SYNCOUT);	/*  update outputs */
	devpriv->ao_data[0] = 0x0800;
	devpriv->ao_data[1] = 0x0800;
	devpriv->ao_data[2] = 0x0800;
	devpriv->ao_data[3] = 0x0800;
	return 0;
}

/*
==============================================================================
*/
static int pci1710_reset(struct comedi_device *dev)
{
	const struct boardtype *this_board = dev->board_ptr;

	switch (this_board->cardtype) {
	case TYPE_PCI1720:
		return pci1720_reset(dev);
	default:
		return pci171x_reset(dev);
	}
=======
static int pci1710_ao_insn_write(struct comedi_device *dev,
				 struct comedi_subdevice *s,
				 struct comedi_insn *insn,
				 unsigned int *data)
{
	struct pci1710_private *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int range = CR_RANGE(insn->chanspec);
	unsigned int val = s->readback[chan];
	int i;

	devpriv->da_ranges &= ~PCI171X_DAREF_MASK(chan);
	devpriv->da_ranges |= PCI171X_DAREF(chan, range);
	outw(devpriv->da_ranges, dev->iobase + PCI171X_DAREF_REG);

	for (i = 0; i < insn->n; i++) {
		val = data[i];
		outw(val, dev->iobase + PCI171X_DA_REG(chan));
	}

	s->readback[chan] = val;

	return insn->n;
}

static int pci1710_di_insn_bits(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn,
				unsigned int *data)
{
	data[1] = inw(dev->iobase + PCI171X_DI_REG);

	return insn->n;
}

static int pci1710_do_insn_bits(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_insn *insn,
				unsigned int *data)
{
	if (comedi_dio_update_state(s, data))
		outw(s->state, dev->iobase + PCI171X_DO_REG);

	data[1] = s->state;

	return insn->n;
}

static int pci1710_counter_insn_config(struct comedi_device *dev,
				       struct comedi_subdevice *s,
				       struct comedi_insn *insn,
				       unsigned int *data)
{
	struct pci1710_private *devpriv = dev->private;

	switch (data[0]) {
	case INSN_CONFIG_SET_CLOCK_SRC:
		switch (data[1]) {
		case 0:	/* internal */
			devpriv->ctrl_ext &= ~PCI171X_CTRL_CNT0;
			break;
		case 1:	/* external */
			devpriv->ctrl_ext |= PCI171X_CTRL_CNT0;
			break;
		default:
			return -EINVAL;
		}
		outw(devpriv->ctrl_ext, dev->iobase + PCI171X_CTRL_REG);
		break;
	case INSN_CONFIG_GET_CLOCK_SRC:
		if (devpriv->ctrl_ext & PCI171X_CTRL_CNT0) {
			data[1] = 1;
			data[2] = 0;
		} else {
			data[1] = 0;
			data[2] = I8254_OSC_BASE_1MHZ;
		}
		break;
	default:
		return -EINVAL;
	}

	return insn->n;
}

static void pci1710_reset(struct comedi_device *dev)
{
	const struct boardtype *board = dev->board_ptr;

	/*
	 * Disable A/D triggers and interrupt sources, set counter 0
	 * to use internal 1 MHz clock.
	 */
	outw(0, dev->iobase + PCI171X_CTRL_REG);

	/* clear A/D FIFO and any pending interrutps */
	outb(0, dev->iobase + PCI171X_CLRFIFO_REG);
	outb(0, dev->iobase + PCI171X_CLRINT_REG);

	if (board->has_ao) {
		/* set DACs to 0..5V and outputs to 0V */
		outb(0, dev->iobase + PCI171X_DAREF_REG);
		outw(0, dev->iobase + PCI171X_DA_REG(0));
		outw(0, dev->iobase + PCI171X_DA_REG(1));
	}

	/* set digital outputs to 0 */
	outw(0, dev->iobase + PCI171X_DO_REG);
>>>>>>> v4.9.227
}

static int pci1710_auto_attach(struct comedi_device *dev,
			       unsigned long context)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
<<<<<<< HEAD
	const struct boardtype *this_board = NULL;
	struct pci1710_private *devpriv;
	struct comedi_subdevice *s;
	int ret, subdev, n_subdevices;

	if (context < ARRAY_SIZE(boardtypes))
		this_board = &boardtypes[context];
	if (!this_board)
		return -ENODEV;
	dev->board_ptr = this_board;
	dev->board_name = this_board->name;
=======
	const struct boardtype *board = NULL;
	struct pci1710_private *devpriv;
	struct comedi_subdevice *s;
	int ret, subdev, n_subdevices;
	int i;

	if (context < ARRAY_SIZE(boardtypes))
		board = &boardtypes[context];
	if (!board)
		return -ENODEV;
	dev->board_ptr = board;
	dev->board_name = board->name;
>>>>>>> v4.9.227

	devpriv = comedi_alloc_devpriv(dev, sizeof(*devpriv));
	if (!devpriv)
		return -ENOMEM;

	ret = comedi_pci_enable(dev);
	if (ret)
		return ret;
	dev->iobase = pci_resource_start(pcidev, 2);

<<<<<<< HEAD
	n_subdevices = 0;
	if (this_board->n_aichan)
		n_subdevices++;
	if (this_board->n_aochan)
		n_subdevices++;
	if (this_board->n_dichan)
		n_subdevices++;
	if (this_board->n_dochan)
		n_subdevices++;
	if (this_board->n_counter)
		n_subdevices++;
=======
	dev->pacer = comedi_8254_init(dev->iobase + PCI171X_TIMER_BASE,
				      I8254_OSC_BASE_10MHZ, I8254_IO16, 0);
	if (!dev->pacer)
		return -ENOMEM;

	n_subdevices = 1;	/* all boards have analog inputs */
	if (board->has_ao)
		n_subdevices++;
	if (!board->is_pci1713) {
		/*
		 * All other boards have digital inputs and outputs as
		 * well as a user counter.
		 */
		n_subdevices += 3;
	}
>>>>>>> v4.9.227

	ret = comedi_alloc_subdevices(dev, n_subdevices);
	if (ret)
		return ret;

	pci1710_reset(dev);

<<<<<<< HEAD
	if (this_board->have_irq && pcidev->irq) {
		ret = request_irq(pcidev->irq, interrupt_service_pci1710,
=======
	if (pcidev->irq) {
		ret = request_irq(pcidev->irq, pci1710_irq_handler,
>>>>>>> v4.9.227
				  IRQF_SHARED, dev->board_name, dev);
		if (ret == 0)
			dev->irq = pcidev->irq;
	}

	subdev = 0;

<<<<<<< HEAD
	if (this_board->n_aichan) {
		s = &dev->subdevices[subdev];
		s->type = COMEDI_SUBD_AI;
		s->subdev_flags = SDF_READABLE | SDF_COMMON | SDF_GROUND;
		if (this_board->n_aichand)
			s->subdev_flags |= SDF_DIFF;
		s->n_chan = this_board->n_aichan;
		s->maxdata = this_board->ai_maxdata;
		s->range_table = this_board->rangelist_ai;
		s->insn_read = pci171x_insn_read_ai;
		if (dev->irq) {
			dev->read_subdev = s;
			s->subdev_flags |= SDF_CMD_READ;
			s->len_chanlist = s->n_chan;
			s->do_cmdtest = pci171x_ai_cmdtest;
			s->do_cmd = pci171x_ai_cmd;
			s->cancel = pci171x_ai_cancel;
		}
		subdev++;
	}

	if (this_board->n_aochan) {
		s = &dev->subdevices[subdev];
		s->type = COMEDI_SUBD_AO;
		s->subdev_flags = SDF_WRITABLE | SDF_GROUND | SDF_COMMON;
		s->n_chan = this_board->n_aochan;
		s->maxdata = this_board->ao_maxdata;
		s->len_chanlist = this_board->n_aochan;
		s->range_table = this_board->rangelist_ao;
		switch (this_board->cardtype) {
		case TYPE_PCI1720:
			s->insn_write = pci1720_insn_write_ao;
			break;
		default:
			s->insn_write = pci171x_insn_write_ao;
			break;
		}
		s->insn_read = pci171x_insn_read_ao;
		subdev++;
	}

	if (this_board->n_dichan) {
		s = &dev->subdevices[subdev];
		s->type = COMEDI_SUBD_DI;
		s->subdev_flags = SDF_READABLE | SDF_GROUND | SDF_COMMON;
		s->n_chan = this_board->n_dichan;
		s->maxdata = 1;
		s->len_chanlist = this_board->n_dichan;
		s->range_table = &range_digital;
		s->insn_bits = pci171x_insn_bits_di;
		subdev++;
	}

	if (this_board->n_dochan) {
		s = &dev->subdevices[subdev];
		s->type = COMEDI_SUBD_DO;
		s->subdev_flags = SDF_WRITABLE | SDF_GROUND | SDF_COMMON;
		s->n_chan = this_board->n_dochan;
		s->maxdata = 1;
		s->len_chanlist = this_board->n_dochan;
		s->range_table = &range_digital;
		s->insn_bits = pci171x_insn_bits_do;
		subdev++;
	}

	if (this_board->n_counter) {
		s = &dev->subdevices[subdev];
		s->type = COMEDI_SUBD_COUNTER;
		s->subdev_flags = SDF_READABLE | SDF_WRITABLE;
		s->n_chan = this_board->n_counter;
		s->len_chanlist = this_board->n_counter;
		s->maxdata = 0xffff;
		s->range_table = &range_unknown;
		s->insn_read = pci171x_insn_counter_read;
		s->insn_write = pci171x_insn_counter_write;
		s->insn_config = pci171x_insn_counter_config;
		subdev++;
	}
=======
	/* Analog Input subdevice */
	s = &dev->subdevices[subdev++];
	s->type		= COMEDI_SUBD_AI;
	s->subdev_flags	= SDF_READABLE | SDF_GROUND;
	if (!board->is_pci1711)
		s->subdev_flags	|= SDF_DIFF;
	s->n_chan	= board->is_pci1713 ? 32 : 16;
	s->maxdata	= 0x0fff;
	s->range_table	= board->ai_range;
	s->insn_read	= pci1710_ai_insn_read;
	if (dev->irq) {
		dev->read_subdev = s;
		s->subdev_flags	|= SDF_CMD_READ;
		s->len_chanlist	= s->n_chan;
		s->do_cmdtest	= pci1710_ai_cmdtest;
		s->do_cmd	= pci1710_ai_cmd;
		s->cancel	= pci1710_ai_cancel;
	}

	/* find the value needed to adjust for unipolar gain codes */
	for (i = 0; i < s->range_table->length; i++) {
		if (comedi_range_is_unipolar(s, i)) {
			devpriv->unipolar_gain = i;
			break;
		}
	}

	if (board->has_ao) {
		/* Analog Output subdevice */
		s = &dev->subdevices[subdev++];
		s->type		= COMEDI_SUBD_AO;
		s->subdev_flags	= SDF_WRITABLE | SDF_GROUND;
		s->n_chan	= 2;
		s->maxdata	= 0x0fff;
		s->range_table	= &pci171x_ao_range;
		s->insn_write	= pci1710_ao_insn_write;

		ret = comedi_alloc_subdev_readback(s);
		if (ret)
			return ret;
	}

	if (!board->is_pci1713) {
		/* Digital Input subdevice */
		s = &dev->subdevices[subdev++];
		s->type		= COMEDI_SUBD_DI;
		s->subdev_flags	= SDF_READABLE;
		s->n_chan	= 16;
		s->maxdata	= 1;
		s->range_table	= &range_digital;
		s->insn_bits	= pci1710_di_insn_bits;

		/* Digital Output subdevice */
		s = &dev->subdevices[subdev++];
		s->type		= COMEDI_SUBD_DO;
		s->subdev_flags	= SDF_WRITABLE;
		s->n_chan	= 16;
		s->maxdata	= 1;
		s->range_table	= &range_digital;
		s->insn_bits	= pci1710_do_insn_bits;

		/* Counter subdevice (8254) */
		s = &dev->subdevices[subdev++];
		comedi_8254_subdevice_init(s, dev->pacer);

		dev->pacer->insn_config = pci1710_counter_insn_config;

		/* counters 1 and 2 are used internally for the pacer */
		comedi_8254_set_busy(dev->pacer, 1, true);
		comedi_8254_set_busy(dev->pacer, 2, true);
	}

	/* max_samples is half the FIFO size (2 bytes/sample) */
	devpriv->max_samples = (board->is_pci1711) ? 512 : 2048;
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static void pci1710_detach(struct comedi_device *dev)
{
	if (dev->iobase)
		pci1710_reset(dev);
	comedi_pci_detach(dev);
}

=======
>>>>>>> v4.9.227
static struct comedi_driver adv_pci1710_driver = {
	.driver_name	= "adv_pci1710",
	.module		= THIS_MODULE,
	.auto_attach	= pci1710_auto_attach,
<<<<<<< HEAD
	.detach		= pci1710_detach,
=======
	.detach		= comedi_pci_detach,
>>>>>>> v4.9.227
};

static int adv_pci1710_pci_probe(struct pci_dev *dev,
				 const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &adv_pci1710_driver,
				      id->driver_data);
}

static const struct pci_device_id adv_pci1710_pci_table[] = {
	{
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710,
			       PCI_VENDOR_ID_PLX, PCI_DEVICE_ID_PLX_9050),
		.driver_data = BOARD_PCI1710,
	}, {
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710,
			       PCI_VENDOR_ID_ADVANTECH, 0x0000),
		.driver_data = BOARD_PCI1710,
	}, {
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710,
			       PCI_VENDOR_ID_ADVANTECH, 0xb100),
		.driver_data = BOARD_PCI1710,
	}, {
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710,
			       PCI_VENDOR_ID_ADVANTECH, 0xb200),
		.driver_data = BOARD_PCI1710,
	}, {
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710,
			       PCI_VENDOR_ID_ADVANTECH, 0xc100),
		.driver_data = BOARD_PCI1710,
	}, {
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710,
			       PCI_VENDOR_ID_ADVANTECH, 0xc200),
		.driver_data = BOARD_PCI1710,
	}, {
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710, 0x1000, 0xd100),
		.driver_data = BOARD_PCI1710,
	}, {
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710,
			       PCI_VENDOR_ID_ADVANTECH, 0x0002),
		.driver_data = BOARD_PCI1710HG,
	}, {
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710,
			       PCI_VENDOR_ID_ADVANTECH, 0xb102),
		.driver_data = BOARD_PCI1710HG,
	}, {
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710,
			       PCI_VENDOR_ID_ADVANTECH, 0xb202),
		.driver_data = BOARD_PCI1710HG,
	}, {
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710,
			       PCI_VENDOR_ID_ADVANTECH, 0xc102),
		.driver_data = BOARD_PCI1710HG,
	}, {
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710,
			       PCI_VENDOR_ID_ADVANTECH, 0xc202),
		.driver_data = BOARD_PCI1710HG,
	}, {
		PCI_DEVICE_SUB(PCI_VENDOR_ID_ADVANTECH, 0x1710, 0x1000, 0xd102),
		.driver_data = BOARD_PCI1710HG,
	},
	{ PCI_VDEVICE(ADVANTECH, 0x1711), BOARD_PCI1711 },
	{ PCI_VDEVICE(ADVANTECH, 0x1713), BOARD_PCI1713 },
<<<<<<< HEAD
	{ PCI_VDEVICE(ADVANTECH, 0x1720), BOARD_PCI1720 },
=======
>>>>>>> v4.9.227
	{ PCI_VDEVICE(ADVANTECH, 0x1731), BOARD_PCI1731 },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, adv_pci1710_pci_table);

static struct pci_driver adv_pci1710_pci_driver = {
	.name		= "adv_pci1710",
	.id_table	= adv_pci1710_pci_table,
	.probe		= adv_pci1710_pci_probe,
	.remove		= comedi_pci_auto_unconfig,
};
module_comedi_pci_driver(adv_pci1710_driver, adv_pci1710_pci_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
<<<<<<< HEAD
MODULE_DESCRIPTION("Comedi low-level driver");
=======
MODULE_DESCRIPTION("Comedi: Advantech PCI-1710 Series Multifunction DAS Cards");
>>>>>>> v4.9.227
MODULE_LICENSE("GPL");
