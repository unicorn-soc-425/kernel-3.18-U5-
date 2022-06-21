/*
<<<<<<< HEAD
    comedi/drivers/cb_pcidas.c

    Developed by Ivan Martinez and Frank Mori Hess, with valuable help from
    David Schleef and the rest of the Comedi developers comunity.

    Copyright (C) 2001-2003 Ivan Martinez <imr@oersted.dtu.dk>
    Copyright (C) 2001,2002 Frank Mori Hess <fmhess@users.sourceforge.net>

    COMEDI - Linux Control and Measurement Device Interface
    Copyright (C) 1997-8 David A. Schleef <ds@schleef.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
/*
Driver: cb_pcidas
Description: MeasurementComputing PCI-DAS series
  with the AMCC S5933 PCI controller
Author: Ivan Martinez <imr@oersted.dtu.dk>,
  Frank Mori Hess <fmhess@users.sourceforge.net>
Updated: 2003-3-11
Devices: [Measurement Computing] PCI-DAS1602/16 (cb_pcidas),
  PCI-DAS1602/16jr, PCI-DAS1602/12, PCI-DAS1200, PCI-DAS1200jr,
  PCI-DAS1000, PCI-DAS1001, PCI_DAS1002

Status:
  There are many reports of the driver being used with most of the
  supported cards. Despite no detailed log is maintained, it can
  be said that the driver is quite tested and stable.

  The boards may be autocalibrated using the comedi_calibrate
  utility.

Configuration options: not applicable, uses PCI auto config

For commands, the scanned channels must be consecutive
(i.e. 4-5-6-7, 2-3-4,...), and must all have the same
range and aref.

AI Triggering:
   For start_src == TRIG_EXT, the A/D EXTERNAL TRIGGER IN (pin 45) is used.
   For 1602 series, the start_arg is interpreted as follows:
     start_arg == 0                   => gated trigger (level high)
     start_arg == CR_INVERT           => gated trigger (level low)
     start_arg == CR_EDGE             => Rising edge
     start_arg == CR_EDGE | CR_INVERT => Falling edge
   For the other boards the trigger will be done on rising edge
*/
/*

TODO:

analog triggering on 1602 series
*/

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

#include "../comedidev.h"

#include "8253.h"
#include "8255.h"
#include "amcc_s5933.h"
#include "comedi_fc.h"

#define AI_BUFFER_SIZE		1024	/* max ai fifo size */
#define AO_BUFFER_SIZE		1024	/* max ao fifo size */
#define NUM_CHANNELS_8800	8
#define NUM_CHANNELS_7376	1
#define NUM_CHANNELS_8402	2
#define NUM_CHANNELS_DAC08	1

/* Control/Status registers */
#define INT_ADCFIFO		0	/* INTERRUPT / ADC FIFO register */
#define   INT_EOS		0x1	/* int end of scan */
#define   INT_FHF		0x2	/* int fifo half full */
#define   INT_FNE		0x3	/* int fifo not empty */
#define   INT_MASK		0x3	/* mask of int select bits */
#define   INTE			0x4	/* int enable */
#define   DAHFIE		0x8	/* dac half full int enable */
#define   EOAIE			0x10	/* end of acq. int enable */
#define   DAHFI			0x20	/* dac half full status / clear */
#define   EOAI			0x40	/* end of acq. int status / clear */
#define   INT			0x80	/* int status / clear */
#define   EOBI			0x200	/* end of burst int status */
#define   ADHFI			0x400	/* half-full int status */
#define   ADNEI			0x800	/* fifo not empty int status (latch) */
#define   ADNE			0x1000	/* fifo not empty status (realtime) */
#define   DAEMIE		0x1000	/* dac empty int enable */
#define   LADFUL		0x2000	/* fifo overflow / clear */
#define   DAEMI			0x4000	/* dac fifo empty int status / clear */

#define ADCMUX_CONT		2	/* ADC CHANNEL MUX AND CONTROL reg */
#define   BEGIN_SCAN(x)		((x) & 0xf)
#define   END_SCAN(x)		(((x) & 0xf) << 4)
#define   GAIN_BITS(x)		(((x) & 0x3) << 8)
#define   UNIP			0x800	/* Analog front-end unipolar mode */
#define   SE			0x400	/* Inputs in single-ended mode */
#define   PACER_MASK		0x3000	/* pacer source bits */
#define   PACER_INT		0x1000	/* int. pacer */
#define   PACER_EXT_FALL	0x2000	/* ext. falling edge */
#define   PACER_EXT_RISE	0x3000	/* ext. rising edge */
#define   EOC			0x4000	/* adc not busy */

#define TRIG_CONTSTAT		 4	/* TRIGGER CONTROL/STATUS register */
#define   SW_TRIGGER		0x1	/* software start trigger */
#define   EXT_TRIGGER		0x2	/* ext. start trigger */
#define   ANALOG_TRIGGER	0x3	/* ext. analog trigger */
#define   TRIGGER_MASK		0x3	/* start trigger mask */
#define   TGPOL			0x04	/* invert trigger (1602 only) */
#define   TGSEL			0x08	/* edge/level trigerred (1602 only) */
#define   TGEN			0x10	/* enable external start trigger */
#define   BURSTE		0x20	/* burst mode enable */
#define   XTRCL			0x80	/* clear external trigger */

#define CALIBRATION_REG		6	/* CALIBRATION register */
#define   SELECT_8800_BIT	0x100	/* select 8800 caldac */
#define   SELECT_TRIMPOT_BIT	0x200	/* select ad7376 trim pot */
#define   SELECT_DAC08_BIT	0x400	/* select dac08 caldac */
#define   CAL_SRC_BITS(x)	(((x) & 0x7) << 11)
#define   CAL_EN_BIT		0x4000	/* calibration source enable */
#define   SERIAL_DATA_IN_BIT	0x8000	/* serial data bit going to caldac */

#define DAC_CSR			0x8	/* dac control and status register */
#define   DACEN			0x02	/* dac enable */
#define   DAC_MODE_UPDATE_BOTH	0x80	/* update both dacs */

static inline unsigned int DAC_RANGE(unsigned int channel, unsigned int range)
{
	return (range & 0x3) << (8 + 2 * (channel & 0x1));
}

static inline unsigned int DAC_RANGE_MASK(unsigned int channel)
{
	return 0x3 << (8 + 2 * (channel & 0x1));
};

/* bits for 1602 series only */
#define   DAC_EMPTY		0x1	/* fifo empty, read, write clear */
#define   DAC_START		0x4	/* start/arm fifo operations */
#define   DAC_PACER_MASK	0x18	/* bits that set pacer source */
#define   DAC_PACER_INT		0x8	/* int. pacing */
#define   DAC_PACER_EXT_FALL	0x10	/* ext. pacing, falling edge */
#define   DAC_PACER_EXT_RISE	0x18	/* ext. pacing, rising edge */

static inline unsigned int DAC_CHAN_EN(unsigned int channel)
{
	return 1 << (5 + (channel & 0x1));	/*  enable channel 0 or 1 */
};

/* analog input fifo */
#define ADCDATA			0	/* ADC DATA register */
#define ADCFIFOCLR		2	/* ADC FIFO CLEAR */

/* pacer, counter, dio registers */
#define ADC8254			0
#define DIO_8255		4
#define DAC8254			8

/* analog output registers for 100x, 1200 series */
static inline unsigned int DAC_DATA_REG(unsigned int channel)
{
	return 2 * (channel & 0x1);
}

/* analog output registers for 1602 series*/
#define DACDATA			0	/* DAC DATA register */
#define DACFIFOCLR		2	/* DAC FIFO CLEAR */

#define IS_UNIPOLAR		0x4	/* unipolar range mask */
=======
 * cb_pcidas.c
 * Developed by Ivan Martinez and Frank Mori Hess, with valuable help from
 * David Schleef and the rest of the Comedi developers comunity.
 *
 * Copyright (C) 2001-2003 Ivan Martinez <imr@oersted.dtu.dk>
 * Copyright (C) 2001,2002 Frank Mori Hess <fmhess@users.sourceforge.net>
 *
 * COMEDI - Linux Control and Measurement Device Interface
 * Copyright (C) 1997-8 David A. Schleef <ds@schleef.org>
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
 */

/*
 * Driver: cb_pcidas
 * Description: MeasurementComputing PCI-DAS series
 *   with the AMCC S5933 PCI controller
 * Devices: [Measurement Computing] PCI-DAS1602/16 (cb_pcidas),
 *   PCI-DAS1602/16jr, PCI-DAS1602/12, PCI-DAS1200, PCI-DAS1200jr,
 *   PCI-DAS1000, PCI-DAS1001, PCI_DAS1002
 * Author: Ivan Martinez <imr@oersted.dtu.dk>,
 *   Frank Mori Hess <fmhess@users.sourceforge.net>
 * Updated: 2003-3-11
 *
 * Status:
 * There are many reports of the driver being used with most of the
 * supported cards. Despite no detailed log is maintained, it can
 * be said that the driver is quite tested and stable.
 *
 * The boards may be autocalibrated using the comedi_calibrate
 * utility.
 *
 * Configuration options: not applicable, uses PCI auto config
 *
 * For commands, the scanned channels must be consecutive
 * (i.e. 4-5-6-7, 2-3-4,...), and must all have the same
 * range and aref.
 *
 * AI Triggering:
 * For start_src == TRIG_EXT, the A/D EXTERNAL TRIGGER IN (pin 45) is used.
 * For 1602 series, the start_arg is interpreted as follows:
 *	start_arg == 0                   => gated trigger (level high)
 *	start_arg == CR_INVERT           => gated trigger (level low)
 *	start_arg == CR_EDGE             => Rising edge
 *	start_arg == CR_EDGE | CR_INVERT => Falling edge
 * For the other boards the trigger will be done on rising edge
 */

/*
 * TODO:
 * analog triggering on 1602 series
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

#include "../comedi_pci.h"

#include "comedi_8254.h"
#include "8255.h"
#include "amcc_s5933.h"

#define AI_BUFFER_SIZE		1024	/* max ai fifo size */
#define AO_BUFFER_SIZE		1024	/* max ao fifo size */

/*
 * PCI BAR1 Register map (devpriv->pcibar1)
 */
#define PCIDAS_CTRL_REG		0x00	/* INTERRUPT / ADC FIFO register */
#define PCIDAS_CTRL_INT(x)	(((x) & 0x3) << 0)
#define PCIDAS_CTRL_INT_NONE	PCIDAS_CTRL_INT(0) /* no int selected */
#define PCIDAS_CTRL_INT_EOS	PCIDAS_CTRL_INT(1) /* int on end of scan */
#define PCIDAS_CTRL_INT_FHF	PCIDAS_CTRL_INT(2) /* int on fifo half full */
#define PCIDAS_CTRL_INT_FNE	PCIDAS_CTRL_INT(3) /* int on fifo not empty */
#define PCIDAS_CTRL_INT_MASK	PCIDAS_CTRL_INT(3) /* mask of int select bits */
#define PCIDAS_CTRL_INTE	BIT(2)	/* int enable */
#define PCIDAS_CTRL_DAHFIE	BIT(3)	/* dac half full int enable */
#define PCIDAS_CTRL_EOAIE	BIT(4)	/* end of acq. int enable */
#define PCIDAS_CTRL_DAHFI	BIT(5)	/* dac half full status / clear */
#define PCIDAS_CTRL_EOAI	BIT(6)	/* end of acq. int status / clear */
#define PCIDAS_CTRL_INT_CLR	BIT(7)	/* int status / clear */
#define PCIDAS_CTRL_EOBI	BIT(9)	/* end of burst int status */
#define PCIDAS_CTRL_ADHFI	BIT(10)	/* half-full int status */
#define PCIDAS_CTRL_ADNEI	BIT(11)	/* fifo not empty int status (latch) */
#define PCIDAS_CTRL_ADNE	BIT(12)	/* fifo not empty status (realtime) */
#define PCIDAS_CTRL_DAEMIE	BIT(12)	/* dac empty int enable */
#define PCIDAS_CTRL_LADFUL	BIT(13)	/* fifo overflow / clear */
#define PCIDAS_CTRL_DAEMI	BIT(14)	/* dac fifo empty int status / clear */

#define PCIDAS_CTRL_AI_INT	(PCIDAS_CTRL_EOAI | PCIDAS_CTRL_EOBI |   \
				 PCIDAS_CTRL_ADHFI | PCIDAS_CTRL_ADNEI | \
				 PCIDAS_CTRL_LADFUL)
#define PCIDAS_CTRL_AO_INT	(PCIDAS_CTRL_DAHFI | PCIDAS_CTRL_DAEMI)

#define PCIDAS_AI_REG		0x02	/* ADC CHANNEL MUX AND CONTROL reg */
#define PCIDAS_AI_FIRST(x)	((x) & 0xf)
#define PCIDAS_AI_LAST(x)	(((x) & 0xf) << 4)
#define PCIDAS_AI_CHAN(x)	(PCIDAS_AI_FIRST(x) | PCIDAS_AI_LAST(x))
#define PCIDAS_AI_GAIN(x)	(((x) & 0x3) << 8)
#define PCIDAS_AI_SE		BIT(10)	/* Inputs in single-ended mode */
#define PCIDAS_AI_UNIP		BIT(11)	/* Analog front-end unipolar mode */
#define PCIDAS_AI_PACER(x)	(((x) & 0x3) << 12)
#define PCIDAS_AI_PACER_SW	PCIDAS_AI_PACER(0) /* software pacer */
#define PCIDAS_AI_PACER_INT	PCIDAS_AI_PACER(1) /* int. pacer */
#define PCIDAS_AI_PACER_EXTN	PCIDAS_AI_PACER(2) /* ext. falling edge */
#define PCIDAS_AI_PACER_EXTP	PCIDAS_AI_PACER(3) /* ext. rising edge */
#define PCIDAS_AI_PACER_MASK	PCIDAS_AI_PACER(3) /* pacer source bits */
#define PCIDAS_AI_EOC		BIT(14)	/* adc not busy */

#define PCIDAS_TRIG_REG		0x04	/* TRIGGER CONTROL/STATUS register */
#define PCIDAS_TRIG_SEL(x)	(((x) & 0x3) << 0)
#define PCIDAS_TRIG_SEL_NONE	PCIDAS_TRIG_SEL(0) /* no start trigger */
#define PCIDAS_TRIG_SEL_SW	PCIDAS_TRIG_SEL(1) /* software start trigger */
#define PCIDAS_TRIG_SEL_EXT	PCIDAS_TRIG_SEL(2) /* ext. start trigger */
#define PCIDAS_TRIG_SEL_ANALOG	PCIDAS_TRIG_SEL(3) /* ext. analog trigger */
#define PCIDAS_TRIG_SEL_MASK	PCIDAS_TRIG_SEL(3) /* start trigger mask */
#define PCIDAS_TRIG_POL		BIT(2)	/* invert trigger (1602 only) */
#define PCIDAS_TRIG_MODE	BIT(3)	/* edge/level trigerred (1602 only) */
#define PCIDAS_TRIG_EN		BIT(4)	/* enable external start trigger */
#define PCIDAS_TRIG_BURSTE	BIT(5)	/* burst mode enable */
#define PCIDAS_TRIG_CLR		BIT(7)	/* clear external trigger */

#define PCIDAS_CALIB_REG	0x06	/* CALIBRATION register */
#define PCIDAS_CALIB_8800_SEL	BIT(8)	/* select 8800 caldac */
#define PCIDAS_CALIB_TRIM_SEL	BIT(9)	/* select ad7376 trim pot */
#define PCIDAS_CALIB_DAC08_SEL	BIT(10)	/* select dac08 caldac */
#define PCIDAS_CALIB_SRC(x)	(((x) & 0x7) << 11)
#define PCIDAS_CALIB_EN		BIT(14)	/* calibration source enable */
#define PCIDAS_CALIB_DATA	BIT(15)	/* serial data bit going to caldac */

#define PCIDAS_AO_REG		0x08	/* dac control and status register */
#define PCIDAS_AO_EMPTY		BIT(0)	/* fifo empty, write clear (1602) */
#define PCIDAS_AO_DACEN		BIT(1)	/* dac enable */
#define PCIDAS_AO_START		BIT(2)	/* start/arm fifo (1602) */
#define PCIDAS_AO_PACER(x)	(((x) & 0x3) << 3) /* (1602) */
#define PCIDAS_AO_PACER_SW	PCIDAS_AO_PACER(0) /* software pacer */
#define PCIDAS_AO_PACER_INT	PCIDAS_AO_PACER(1) /* int. pacer */
#define PCIDAS_AO_PACER_EXTN	PCIDAS_AO_PACER(2) /* ext. falling edge */
#define PCIDAS_AO_PACER_EXTP	PCIDAS_AO_PACER(3) /* ext. rising edge */
#define PCIDAS_AO_PACER_MASK	PCIDAS_AO_PACER(3) /* pacer source bits */
#define PCIDAS_AO_CHAN_EN(c)	BIT(5 + ((c) & 0x1))
#define PCIDAS_AO_CHAN_MASK	(PCIDAS_AO_CHAN_EN(0) | PCIDAS_AO_CHAN_EN(1))
#define PCIDAS_AO_UPDATE_BOTH	BIT(7)	/* update both dacs */
#define PCIDAS_AO_RANGE(c, r)	(((r) & 0x3) << (8 + 2 * ((c) & 0x1)))
#define PCIDAS_AO_RANGE_MASK(c)	PCIDAS_AO_RANGE((c), 0x3)

/*
 * PCI BAR2 Register map (devpriv->pcibar2)
 */
#define PCIDAS_AI_DATA_REG	0x00
#define PCIDAS_AI_FIFO_CLR_REG	0x02

/*
 * PCI BAR3 Register map (dev->iobase)
 */
#define PCIDAS_AI_8254_BASE	0x00
#define PCIDAS_8255_BASE	0x04
#define PCIDAS_AO_8254_BASE	0x08

/*
 * PCI BAR4 Register map (devpriv->pcibar4)
 */
#define PCIDAS_AO_DATA_REG(x)	(0x00 + ((x) * 2))
#define PCIDAS_AO_FIFO_REG	0x00
#define PCIDAS_AO_FIFO_CLR_REG	0x02
>>>>>>> v4.9.227

/* analog input ranges for most boards */
static const struct comedi_lrange cb_pcidas_ranges = {
	8, {
		BIP_RANGE(10),
		BIP_RANGE(5),
		BIP_RANGE(2.5),
		BIP_RANGE(1.25),
		UNI_RANGE(10),
		UNI_RANGE(5),
		UNI_RANGE(2.5),
		UNI_RANGE(1.25)
	}
};

/* pci-das1001 input ranges */
static const struct comedi_lrange cb_pcidas_alt_ranges = {
	8, {
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

/* analog output ranges */
static const struct comedi_lrange cb_pcidas_ao_ranges = {
	4, {
		BIP_RANGE(5),
		BIP_RANGE(10),
		UNI_RANGE(5),
		UNI_RANGE(10)
	}
};

<<<<<<< HEAD
enum trimpot_model {
	AD7376,
	AD8402,
};

=======
>>>>>>> v4.9.227
enum cb_pcidas_boardid {
	BOARD_PCIDAS1602_16,
	BOARD_PCIDAS1200,
	BOARD_PCIDAS1602_12,
	BOARD_PCIDAS1200_JR,
	BOARD_PCIDAS1602_16_JR,
	BOARD_PCIDAS1000,
	BOARD_PCIDAS1001,
	BOARD_PCIDAS1002,
};

struct cb_pcidas_board {
	const char *name;
<<<<<<< HEAD
	int ai_nchan;		/*  Inputs in single-ended mode */
	int ai_bits;		/*  analog input resolution */
	int ai_speed;		/*  fastest conversion period in ns */
	int ao_nchan;		/*  number of analog out channels */
	int has_ao_fifo;	/*  analog output has fifo */
	int ao_scan_speed;	/*  analog output scan speed for 1602 series */
	int fifo_size;		/*  number of samples fifo can hold */
	const struct comedi_lrange *ranges;
	enum trimpot_model trimpot;
	unsigned has_dac08:1;
	unsigned is_1602:1;
=======
	int ai_speed;		/*  fastest conversion period in ns */
	int ao_scan_speed;	/*  analog output scan speed for 1602 series */
	int fifo_size;		/*  number of samples fifo can hold */
	unsigned int is_16bit;		/* ai/ao is 1=16-bit; 0=12-bit */
	unsigned int use_alt_range:1;	/* use alternate ai range table */
	unsigned int has_ao:1;		/* has 2 analog output channels */
	unsigned int has_ao_fifo:1;	/* analog output has fifo */
	unsigned int has_ad8402:1;	/* trimpot type 1=AD8402; 0=AD7376 */
	unsigned int has_dac08:1;
	unsigned int is_1602:1;
>>>>>>> v4.9.227
};

static const struct cb_pcidas_board cb_pcidas_boards[] = {
	[BOARD_PCIDAS1602_16] = {
		.name		= "pci-das1602/16",
<<<<<<< HEAD
		.ai_nchan	= 16,
		.ai_bits	= 16,
		.ai_speed	= 5000,
		.ao_nchan	= 2,
		.has_ao_fifo	= 1,
		.ao_scan_speed	= 10000,
		.fifo_size	= 512,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD8402,
=======
		.ai_speed	= 5000,
		.ao_scan_speed	= 10000,
		.fifo_size	= 512,
		.is_16bit	= 1,
		.has_ao		= 1,
		.has_ao_fifo	= 1,
		.has_ad8402	= 1,
>>>>>>> v4.9.227
		.has_dac08	= 1,
		.is_1602	= 1,
	},
	[BOARD_PCIDAS1200] = {
		.name		= "pci-das1200",
<<<<<<< HEAD
		.ai_nchan	= 16,
		.ai_bits	= 12,
		.ai_speed	= 3200,
		.ao_nchan	= 2,
		.fifo_size	= 1024,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD7376,
	},
	[BOARD_PCIDAS1602_12] = {
		.name		= "pci-das1602/12",
		.ai_nchan	= 16,
		.ai_bits	= 12,
		.ai_speed	= 3200,
		.ao_nchan	= 2,
		.has_ao_fifo	= 1,
		.ao_scan_speed	= 4000,
		.fifo_size	= 1024,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD7376,
=======
		.ai_speed	= 3200,
		.fifo_size	= 1024,
		.has_ao		= 1,
	},
	[BOARD_PCIDAS1602_12] = {
		.name		= "pci-das1602/12",
		.ai_speed	= 3200,
		.ao_scan_speed	= 4000,
		.fifo_size	= 1024,
		.has_ao		= 1,
		.has_ao_fifo	= 1,
>>>>>>> v4.9.227
		.is_1602	= 1,
	},
	[BOARD_PCIDAS1200_JR] = {
		.name		= "pci-das1200/jr",
<<<<<<< HEAD
		.ai_nchan	= 16,
		.ai_bits	= 12,
		.ai_speed	= 3200,
		.fifo_size	= 1024,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD7376,
	},
	[BOARD_PCIDAS1602_16_JR] = {
		.name		= "pci-das1602/16/jr",
		.ai_nchan	= 16,
		.ai_bits	= 16,
		.ai_speed	= 5000,
		.fifo_size	= 512,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD8402,
=======
		.ai_speed	= 3200,
		.fifo_size	= 1024,
	},
	[BOARD_PCIDAS1602_16_JR] = {
		.name		= "pci-das1602/16/jr",
		.ai_speed	= 5000,
		.fifo_size	= 512,
		.is_16bit	= 1,
		.has_ad8402	= 1,
>>>>>>> v4.9.227
		.has_dac08	= 1,
		.is_1602	= 1,
	},
	[BOARD_PCIDAS1000] = {
		.name		= "pci-das1000",
<<<<<<< HEAD
		.ai_nchan	= 16,
		.ai_bits	= 12,
		.ai_speed	= 4000,
		.fifo_size	= 1024,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD7376,
	},
	[BOARD_PCIDAS1001] = {
		.name		= "pci-das1001",
		.ai_nchan	= 16,
		.ai_bits	= 12,
		.ai_speed	= 6800,
		.ao_nchan	= 2,
		.fifo_size	= 1024,
		.ranges		= &cb_pcidas_alt_ranges,
		.trimpot	= AD7376,
	},
	[BOARD_PCIDAS1002] = {
		.name		= "pci-das1002",
		.ai_nchan	= 16,
		.ai_bits	= 12,
		.ai_speed	= 6800,
		.ao_nchan	= 2,
		.fifo_size	= 1024,
		.ranges		= &cb_pcidas_ranges,
		.trimpot	= AD7376,
=======
		.ai_speed	= 4000,
		.fifo_size	= 1024,
	},
	[BOARD_PCIDAS1001] = {
		.name		= "pci-das1001",
		.ai_speed	= 6800,
		.fifo_size	= 1024,
		.use_alt_range	= 1,
		.has_ao		= 1,
	},
	[BOARD_PCIDAS1002] = {
		.name		= "pci-das1002",
		.ai_speed	= 6800,
		.fifo_size	= 1024,
		.has_ao		= 1,
>>>>>>> v4.9.227
	},
};

struct cb_pcidas_private {
<<<<<<< HEAD
	/* base addresses */
	unsigned long s5933_config;
	unsigned long control_status;
	unsigned long adc_fifo;
	unsigned long ao_registers;
	/* divisors of master clock for analog input pacing */
	unsigned int divisor1;
	unsigned int divisor2;
	/* number of analog input samples remaining */
	unsigned int count;
	/* bits to write to registers */
	unsigned int adc_fifo_bits;
	unsigned int s5933_intcsr_bits;
	unsigned int ao_control_bits;
	/* fifo buffers */
	unsigned short ai_buffer[AI_BUFFER_SIZE];
	unsigned short ao_buffer[AO_BUFFER_SIZE];
	/* divisors of master clock for analog output pacing */
	unsigned int ao_divisor1;
	unsigned int ao_divisor2;
	/* number of analog output samples remaining */
	unsigned int ao_count;
	unsigned int caldac_value[NUM_CHANNELS_8800];
	unsigned int trimpot_value[NUM_CHANNELS_8402];
	unsigned int dac08_value;
	unsigned int calibration_source;
};

static inline unsigned int cal_enable_bits(struct comedi_device *dev)
{
	struct cb_pcidas_private *devpriv = dev->private;

	return CAL_EN_BIT | CAL_SRC_BITS(devpriv->calibration_source);
}

=======
	struct comedi_8254 *ao_pacer;
	/* base addresses */
	unsigned long amcc;	/* pcibar0 */
	unsigned long pcibar1;
	unsigned long pcibar2;
	unsigned long pcibar4;
	/* bits to write to registers */
	unsigned int ctrl;
	unsigned int amcc_intcsr;
	unsigned int ao_ctrl;
	/* fifo buffers */
	unsigned short ai_buffer[AI_BUFFER_SIZE];
	unsigned short ao_buffer[AO_BUFFER_SIZE];
	unsigned int calib_src;
};

>>>>>>> v4.9.227
static int cb_pcidas_ai_eoc(struct comedi_device *dev,
			    struct comedi_subdevice *s,
			    struct comedi_insn *insn,
			    unsigned long context)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int status;

<<<<<<< HEAD
	status = inw(devpriv->control_status + ADCMUX_CONT);
	if (status & EOC)
=======
	status = inw(devpriv->pcibar1 + PCIDAS_AI_REG);
	if (status & PCIDAS_AI_EOC)
>>>>>>> v4.9.227
		return 0;
	return -EBUSY;
}

<<<<<<< HEAD
static int cb_pcidas_ai_rinsn(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_insn *insn, unsigned int *data)
=======
static int cb_pcidas_ai_insn_read(struct comedi_device *dev,
				  struct comedi_subdevice *s,
				  struct comedi_insn *insn,
				  unsigned int *data)
>>>>>>> v4.9.227
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int range = CR_RANGE(insn->chanspec);
	unsigned int aref = CR_AREF(insn->chanspec);
	unsigned int bits;
	int ret;
	int n;

	/* enable calibration input if appropriate */
	if (insn->chanspec & CR_ALT_SOURCE) {
<<<<<<< HEAD
		outw(cal_enable_bits(dev),
		     devpriv->control_status + CALIBRATION_REG);
		chan = 0;
	} else {
		outw(0, devpriv->control_status + CALIBRATION_REG);
	}

	/* set mux limits and gain */
	bits = BEGIN_SCAN(chan) | END_SCAN(chan) | GAIN_BITS(range);
	/* set unipolar/bipolar */
	if (range & IS_UNIPOLAR)
		bits |= UNIP;
	/* set single-ended/differential */
	if (aref != AREF_DIFF)
		bits |= SE;
	outw(bits, devpriv->control_status + ADCMUX_CONT);

	/* clear fifo */
	outw(0, devpriv->adc_fifo + ADCFIFOCLR);
=======
		outw(PCIDAS_CALIB_EN | PCIDAS_CALIB_SRC(devpriv->calib_src),
		     devpriv->pcibar1 + PCIDAS_CALIB_REG);
		chan = 0;
	} else {
		outw(0, devpriv->pcibar1 + PCIDAS_CALIB_REG);
	}

	/* set mux limits and gain */
	bits = PCIDAS_AI_CHAN(chan) | PCIDAS_AI_GAIN(range);
	/* set unipolar/bipolar */
	if (comedi_range_is_unipolar(s, range))
		bits |= PCIDAS_AI_UNIP;
	/* set single-ended/differential */
	if (aref != AREF_DIFF)
		bits |= PCIDAS_AI_SE;
	outw(bits, devpriv->pcibar1 + PCIDAS_AI_REG);

	/* clear fifo */
	outw(0, devpriv->pcibar2 + PCIDAS_AI_FIFO_CLR_REG);
>>>>>>> v4.9.227

	/* convert n samples */
	for (n = 0; n < insn->n; n++) {
		/* trigger conversion */
<<<<<<< HEAD
		outw(0, devpriv->adc_fifo + ADCDATA);
=======
		outw(0, devpriv->pcibar2 + PCIDAS_AI_DATA_REG);
>>>>>>> v4.9.227

		/* wait for conversion to end */
		ret = comedi_timeout(dev, s, insn, cb_pcidas_ai_eoc, 0);
		if (ret)
			return ret;

		/* read data */
<<<<<<< HEAD
		data[n] = inw(devpriv->adc_fifo + ADCDATA);
=======
		data[n] = inw(devpriv->pcibar2 + PCIDAS_AI_DATA_REG);
>>>>>>> v4.9.227
	}

	/* return the number of samples read/written */
	return n;
}

<<<<<<< HEAD
static int ai_config_insn(struct comedi_device *dev, struct comedi_subdevice *s,
			  struct comedi_insn *insn, unsigned int *data)
=======
static int cb_pcidas_ai_insn_config(struct comedi_device *dev,
				    struct comedi_subdevice *s,
				    struct comedi_insn *insn,
				    unsigned int *data)
>>>>>>> v4.9.227
{
	struct cb_pcidas_private *devpriv = dev->private;
	int id = data[0];
	unsigned int source = data[1];

	switch (id) {
	case INSN_CONFIG_ALT_SOURCE:
		if (source >= 8) {
			dev_err(dev->class_dev,
				"invalid calibration source: %i\n",
				source);
			return -EINVAL;
		}
<<<<<<< HEAD
		devpriv->calibration_source = source;
=======
		devpriv->calib_src = source;
>>>>>>> v4.9.227
		break;
	default:
		return -EINVAL;
	}
	return insn->n;
}

/* analog output insn for pcidas-1000 and 1200 series */
<<<<<<< HEAD
static int cb_pcidas_ao_nofifo_winsn(struct comedi_device *dev,
				     struct comedi_subdevice *s,
				     struct comedi_insn *insn,
				     unsigned int *data)
=======
static int cb_pcidas_ao_nofifo_insn_write(struct comedi_device *dev,
					  struct comedi_subdevice *s,
					  struct comedi_insn *insn,
					  unsigned int *data)
>>>>>>> v4.9.227
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int range = CR_RANGE(insn->chanspec);
<<<<<<< HEAD
	unsigned long flags;

	/* set channel and range */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->ao_control_bits &= (~DAC_MODE_UPDATE_BOTH &
				     ~DAC_RANGE_MASK(chan));
	devpriv->ao_control_bits |= (DACEN | DAC_RANGE(chan, range));
	outw(devpriv->ao_control_bits, devpriv->control_status + DAC_CSR);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/* remember value for readback */
	s->readback[chan] = data[0];

	/* send data */
	outw(data[0], devpriv->ao_registers + DAC_DATA_REG(chan));
=======
	unsigned int val = s->readback[chan];
	unsigned long flags;
	int i;

	/* set channel and range */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->ao_ctrl &= ~(PCIDAS_AO_UPDATE_BOTH |
			      PCIDAS_AO_RANGE_MASK(chan));
	devpriv->ao_ctrl |= PCIDAS_AO_DACEN | PCIDAS_AO_RANGE(chan, range);
	outw(devpriv->ao_ctrl, devpriv->pcibar1 + PCIDAS_AO_REG);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	for (i = 0; i < insn->n; i++) {
		val = data[i];
		outw(val, devpriv->pcibar4 + PCIDAS_AO_DATA_REG(chan));
	}

	s->readback[chan] = val;
>>>>>>> v4.9.227

	return insn->n;
}

/* analog output insn for pcidas-1602 series */
<<<<<<< HEAD
static int cb_pcidas_ao_fifo_winsn(struct comedi_device *dev,
				   struct comedi_subdevice *s,
				   struct comedi_insn *insn, unsigned int *data)
=======
static int cb_pcidas_ao_fifo_insn_write(struct comedi_device *dev,
					struct comedi_subdevice *s,
					struct comedi_insn *insn,
					unsigned int *data)
>>>>>>> v4.9.227
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int range = CR_RANGE(insn->chanspec);
<<<<<<< HEAD
	unsigned long flags;

	/* clear dac fifo */
	outw(0, devpriv->ao_registers + DACFIFOCLR);

	/* set channel and range */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->ao_control_bits &= (~DAC_CHAN_EN(0) & ~DAC_CHAN_EN(1) &
				     ~DAC_RANGE_MASK(chan) & ~DAC_PACER_MASK);
	devpriv->ao_control_bits |= (DACEN | DAC_RANGE(chan, range) |
				     DAC_CHAN_EN(chan) | DAC_START);
	outw(devpriv->ao_control_bits, devpriv->control_status + DAC_CSR);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/* remember value for readback */
	s->readback[chan] = data[0];

	/* send data */
	outw(data[0], devpriv->ao_registers + DACDATA);
=======
	unsigned int val = s->readback[chan];
	unsigned long flags;
	int i;

	/* clear dac fifo */
	outw(0, devpriv->pcibar4 + PCIDAS_AO_FIFO_CLR_REG);

	/* set channel and range */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->ao_ctrl &= ~(PCIDAS_AO_CHAN_MASK | PCIDAS_AO_RANGE_MASK(chan) |
			      PCIDAS_AO_PACER_MASK);
	devpriv->ao_ctrl |= PCIDAS_AO_DACEN | PCIDAS_AO_RANGE(chan, range) |
			    PCIDAS_AO_CHAN_EN(chan) | PCIDAS_AO_START;
	outw(devpriv->ao_ctrl, devpriv->pcibar1 + PCIDAS_AO_REG);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	for (i = 0; i < insn->n; i++) {
		val = data[i];
		outw(val, devpriv->pcibar4 + PCIDAS_AO_FIFO_REG);
	}

	s->readback[chan] = val;
>>>>>>> v4.9.227

	return insn->n;
}

<<<<<<< HEAD
static int wait_for_nvram_ready(unsigned long s5933_base_addr)
{
	static const int timeout = 1000;
	unsigned int i;

	for (i = 0; i < timeout; i++) {
		if ((inb(s5933_base_addr +
			 AMCC_OP_REG_MCSR_NVCMD) & MCSR_NV_BUSY)
		    == 0)
			return 0;
		udelay(1);
	}
	return -1;
}

static int nvram_read(struct comedi_device *dev, unsigned int address,
			uint8_t *data)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned long iobase = devpriv->s5933_config;

	if (wait_for_nvram_ready(iobase) < 0)
		return -ETIMEDOUT;

	outb(MCSR_NV_ENABLE | MCSR_NV_LOAD_LOW_ADDR,
	     iobase + AMCC_OP_REG_MCSR_NVCMD);
	outb(address & 0xff, iobase + AMCC_OP_REG_MCSR_NVDATA);
	outb(MCSR_NV_ENABLE | MCSR_NV_LOAD_HIGH_ADDR,
	     iobase + AMCC_OP_REG_MCSR_NVCMD);
	outb((address >> 8) & 0xff, iobase + AMCC_OP_REG_MCSR_NVDATA);
	outb(MCSR_NV_ENABLE | MCSR_NV_READ, iobase + AMCC_OP_REG_MCSR_NVCMD);

	if (wait_for_nvram_ready(iobase) < 0)
		return -ETIMEDOUT;

	*data = inb(iobase + AMCC_OP_REG_MCSR_NVDATA);

	return 0;
}

static int eeprom_read_insn(struct comedi_device *dev,
			    struct comedi_subdevice *s,
			    struct comedi_insn *insn, unsigned int *data)
{
	uint8_t nvram_data;
	int retval;

	retval = nvram_read(dev, CR_CHAN(insn->chanspec), &nvram_data);
	if (retval < 0)
		return retval;

	data[0] = nvram_data;

	return 1;
}

static void write_calibration_bitstream(struct comedi_device *dev,
					unsigned int register_bits,
					unsigned int bitstream,
					unsigned int bitstream_length)
{
	struct cb_pcidas_private *devpriv = dev->private;
	static const int write_delay = 1;
	unsigned int bit;

	for (bit = 1 << (bitstream_length - 1); bit; bit >>= 1) {
		if (bitstream & bit)
			register_bits |= SERIAL_DATA_IN_BIT;
		else
			register_bits &= ~SERIAL_DATA_IN_BIT;
		udelay(write_delay);
		outw(register_bits, devpriv->control_status + CALIBRATION_REG);
	}
}

static int caldac_8800_write(struct comedi_device *dev, unsigned int address,
			     uint8_t value)
{
	struct cb_pcidas_private *devpriv = dev->private;
	static const int num_caldac_channels = 8;
	static const int bitstream_length = 11;
	unsigned int bitstream = ((address & 0x7) << 8) | value;
	static const int caldac_8800_udelay = 1;

	if (address >= num_caldac_channels) {
		dev_err(dev->class_dev, "illegal caldac channel\n");
		return -1;
	}

	if (value == devpriv->caldac_value[address])
		return 1;

	devpriv->caldac_value[address] = value;

	write_calibration_bitstream(dev, cal_enable_bits(dev), bitstream,
				    bitstream_length);

	udelay(caldac_8800_udelay);
	outw(cal_enable_bits(dev) | SELECT_8800_BIT,
	     devpriv->control_status + CALIBRATION_REG);
	udelay(caldac_8800_udelay);
	outw(cal_enable_bits(dev), devpriv->control_status + CALIBRATION_REG);

	return 1;
}

static int caldac_write_insn(struct comedi_device *dev,
			     struct comedi_subdevice *s,
			     struct comedi_insn *insn, unsigned int *data)
{
	const unsigned int channel = CR_CHAN(insn->chanspec);

	return caldac_8800_write(dev, channel, data[0]);
}

static int caldac_read_insn(struct comedi_device *dev,
			    struct comedi_subdevice *s,
			    struct comedi_insn *insn, unsigned int *data)
{
	struct cb_pcidas_private *devpriv = dev->private;

	data[0] = devpriv->caldac_value[CR_CHAN(insn->chanspec)];

	return 1;
}

/* 1602/16 pregain offset */
static void dac08_write(struct comedi_device *dev, unsigned int value)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned long cal_reg;

	if (devpriv->dac08_value != value) {
		devpriv->dac08_value = value;

		cal_reg = devpriv->control_status + CALIBRATION_REG;

		value &= 0xff;
		value |= cal_enable_bits(dev);

		/* latch the new value into the caldac */
		outw(value, cal_reg);
		udelay(1);
		outw(value | SELECT_DAC08_BIT, cal_reg);
		udelay(1);
		outw(value, cal_reg);
		udelay(1);
	}
}

static int dac08_write_insn(struct comedi_device *dev,
			    struct comedi_subdevice *s,
			    struct comedi_insn *insn, unsigned int *data)
{
	int i;

	for (i = 0; i < insn->n; i++)
		dac08_write(dev, data[i]);
=======
static int cb_pcidas_eeprom_ready(struct comedi_device *dev,
				  struct comedi_subdevice *s,
				  struct comedi_insn *insn,
				  unsigned long context)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int status;

	status = inb(devpriv->amcc + AMCC_OP_REG_MCSR_NVCMD);
	if ((status & MCSR_NV_BUSY) == 0)
		return 0;
	return -EBUSY;
}

static int cb_pcidas_eeprom_insn_read(struct comedi_device *dev,
				      struct comedi_subdevice *s,
				      struct comedi_insn *insn,
				      unsigned int *data)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	int ret;
	int i;

	for (i = 0; i < insn->n; i++) {
		/* make sure eeprom is ready */
		ret = comedi_timeout(dev, s, insn, cb_pcidas_eeprom_ready, 0);
		if (ret)
			return ret;

		/* set address (chan) and read operation */
		outb(MCSR_NV_ENABLE | MCSR_NV_LOAD_LOW_ADDR,
		     devpriv->amcc + AMCC_OP_REG_MCSR_NVCMD);
		outb(chan & 0xff, devpriv->amcc + AMCC_OP_REG_MCSR_NVDATA);
		outb(MCSR_NV_ENABLE | MCSR_NV_LOAD_HIGH_ADDR,
		     devpriv->amcc + AMCC_OP_REG_MCSR_NVCMD);
		outb((chan >> 8) & 0xff,
		     devpriv->amcc + AMCC_OP_REG_MCSR_NVDATA);
		outb(MCSR_NV_ENABLE | MCSR_NV_READ,
		     devpriv->amcc + AMCC_OP_REG_MCSR_NVCMD);

		/* wait for data to be returned */
		ret = comedi_timeout(dev, s, insn, cb_pcidas_eeprom_ready, 0);
		if (ret)
			return ret;

		data[i] = inb(devpriv->amcc + AMCC_OP_REG_MCSR_NVDATA);
	}
>>>>>>> v4.9.227

	return insn->n;
}

<<<<<<< HEAD
static int dac08_read_insn(struct comedi_device *dev,
			   struct comedi_subdevice *s, struct comedi_insn *insn,
			   unsigned int *data)
{
	struct cb_pcidas_private *devpriv = dev->private;

	data[0] = devpriv->dac08_value;

	return 1;
}

static int trimpot_7376_write(struct comedi_device *dev, uint8_t value)
{
	struct cb_pcidas_private *devpriv = dev->private;
	static const int bitstream_length = 7;
	unsigned int bitstream = value & 0x7f;
	unsigned int register_bits;
	static const int ad7376_udelay = 1;

	register_bits = cal_enable_bits(dev) | SELECT_TRIMPOT_BIT;
	udelay(ad7376_udelay);
	outw(register_bits, devpriv->control_status + CALIBRATION_REG);

	write_calibration_bitstream(dev, register_bits, bitstream,
				    bitstream_length);

	udelay(ad7376_udelay);
	outw(cal_enable_bits(dev), devpriv->control_status + CALIBRATION_REG);

	return 0;
}

/* For 1602/16 only
 * ch 0 : adc gain
 * ch 1 : adc postgain offset */
static int trimpot_8402_write(struct comedi_device *dev, unsigned int channel,
			      uint8_t value)
{
	struct cb_pcidas_private *devpriv = dev->private;
	static const int bitstream_length = 10;
	unsigned int bitstream = ((channel & 0x3) << 8) | (value & 0xff);
	unsigned int register_bits;
	static const int ad8402_udelay = 1;

	register_bits = cal_enable_bits(dev) | SELECT_TRIMPOT_BIT;
	udelay(ad8402_udelay);
	outw(register_bits, devpriv->control_status + CALIBRATION_REG);

	write_calibration_bitstream(dev, register_bits, bitstream,
				    bitstream_length);

	udelay(ad8402_udelay);
	outw(cal_enable_bits(dev), devpriv->control_status + CALIBRATION_REG);

	return 0;
}

static int cb_pcidas_trimpot_write(struct comedi_device *dev,
				   unsigned int channel, unsigned int value)
{
	const struct cb_pcidas_board *thisboard = dev->board_ptr;
	struct cb_pcidas_private *devpriv = dev->private;

	if (devpriv->trimpot_value[channel] == value)
		return 1;

	devpriv->trimpot_value[channel] = value;
	switch (thisboard->trimpot) {
	case AD7376:
		trimpot_7376_write(dev, value);
		break;
	case AD8402:
		trimpot_8402_write(dev, channel, value);
		break;
	default:
		dev_err(dev->class_dev, "driver bug?\n");
		return -1;
	}

	return 1;
}

static int trimpot_write_insn(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_insn *insn, unsigned int *data)
{
	unsigned int channel = CR_CHAN(insn->chanspec);

	return cb_pcidas_trimpot_write(dev, channel, data[0]);
}

static int trimpot_read_insn(struct comedi_device *dev,
			     struct comedi_subdevice *s,
			     struct comedi_insn *insn, unsigned int *data)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int channel = CR_CHAN(insn->chanspec);

	data[0] = devpriv->trimpot_value[channel];

	return 1;
=======
static void cb_pcidas_calib_write(struct comedi_device *dev,
				  unsigned int val, unsigned int len,
				  bool trimpot)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int calib_bits;
	unsigned int bit;

	calib_bits = PCIDAS_CALIB_EN | PCIDAS_CALIB_SRC(devpriv->calib_src);
	if (trimpot) {
		/* select trimpot */
		calib_bits |= PCIDAS_CALIB_TRIM_SEL;
		outw(calib_bits, devpriv->pcibar1 + PCIDAS_CALIB_REG);
	}

	/* write bitstream to calibration device */
	for (bit = 1 << (len - 1); bit; bit >>= 1) {
		if (val & bit)
			calib_bits |= PCIDAS_CALIB_DATA;
		else
			calib_bits &= ~PCIDAS_CALIB_DATA;
		udelay(1);
		outw(calib_bits, devpriv->pcibar1 + PCIDAS_CALIB_REG);
	}
	udelay(1);

	calib_bits = PCIDAS_CALIB_EN | PCIDAS_CALIB_SRC(devpriv->calib_src);

	if (!trimpot) {
		/* select caldac */
		outw(calib_bits | PCIDAS_CALIB_8800_SEL,
		     devpriv->pcibar1 + PCIDAS_CALIB_REG);
		udelay(1);
	}

	/* latch value to trimpot/caldac */
	outw(calib_bits, devpriv->pcibar1 + PCIDAS_CALIB_REG);
}

static int cb_pcidas_caldac_insn_write(struct comedi_device *dev,
				       struct comedi_subdevice *s,
				       struct comedi_insn *insn,
				       unsigned int *data)
{
	unsigned int chan = CR_CHAN(insn->chanspec);

	if (insn->n) {
		unsigned int val = data[insn->n - 1];

		if (s->readback[chan] != val) {
			/* write 11-bit channel/value to caldac */
			cb_pcidas_calib_write(dev, (chan << 8) | val, 11,
					      false);
			s->readback[chan] = val;
		}
	}

	return insn->n;
}

static void cb_pcidas_dac08_write(struct comedi_device *dev, unsigned int val)
{
	struct cb_pcidas_private *devpriv = dev->private;

	val |= PCIDAS_CALIB_EN | PCIDAS_CALIB_SRC(devpriv->calib_src);

	/* latch the new value into the caldac */
	outw(val, devpriv->pcibar1 + PCIDAS_CALIB_REG);
	udelay(1);
	outw(val | PCIDAS_CALIB_DAC08_SEL,
	     devpriv->pcibar1 + PCIDAS_CALIB_REG);
	udelay(1);
	outw(val, devpriv->pcibar1 + PCIDAS_CALIB_REG);
	udelay(1);
}

static int cb_pcidas_dac08_insn_write(struct comedi_device *dev,
				      struct comedi_subdevice *s,
				      struct comedi_insn *insn,
				      unsigned int *data)
{
	unsigned int chan = CR_CHAN(insn->chanspec);

	if (insn->n) {
		unsigned int val = data[insn->n - 1];

		if (s->readback[chan] != val) {
			cb_pcidas_dac08_write(dev, val);
			s->readback[chan] = val;
		}
	}

	return insn->n;
}

static void cb_pcidas_trimpot_write(struct comedi_device *dev,
				    unsigned int chan, unsigned int val)
{
	const struct cb_pcidas_board *board = dev->board_ptr;

	if (board->has_ad8402) {
		/* write 10-bit channel/value to AD8402 trimpot */
		cb_pcidas_calib_write(dev, (chan << 8) | val, 10, true);
	} else {
		/* write 7-bit value to AD7376 trimpot */
		cb_pcidas_calib_write(dev, val, 7, true);
	}
}

static int cb_pcidas_trimpot_insn_write(struct comedi_device *dev,
					struct comedi_subdevice *s,
					struct comedi_insn *insn,
					unsigned int *data)
{
	unsigned int chan = CR_CHAN(insn->chanspec);

	if (insn->n) {
		unsigned int val = data[insn->n - 1];

		if (s->readback[chan] != val) {
			cb_pcidas_trimpot_write(dev, chan, val);
			s->readback[chan] = val;
		}
	}

	return insn->n;
>>>>>>> v4.9.227
}

static int cb_pcidas_ai_check_chanlist(struct comedi_device *dev,
				       struct comedi_subdevice *s,
				       struct comedi_cmd *cmd)
{
	unsigned int chan0 = CR_CHAN(cmd->chanlist[0]);
	unsigned int range0 = CR_RANGE(cmd->chanlist[0]);
	int i;

	for (i = 1; i < cmd->chanlist_len; i++) {
		unsigned int chan = CR_CHAN(cmd->chanlist[i]);
		unsigned int range = CR_RANGE(cmd->chanlist[i]);

		if (chan != (chan0 + i) % s->n_chan) {
			dev_dbg(dev->class_dev,
				"entries in chanlist must be consecutive channels, counting upwards\n");
			return -EINVAL;
		}

		if (range != range0) {
			dev_dbg(dev->class_dev,
				"entries in chanlist must all have the same gain\n");
			return -EINVAL;
		}
	}
	return 0;
}

static int cb_pcidas_ai_cmdtest(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_cmd *cmd)
{
<<<<<<< HEAD
	const struct cb_pcidas_board *thisboard = dev->board_ptr;
	struct cb_pcidas_private *devpriv = dev->private;
=======
	const struct cb_pcidas_board *board = dev->board_ptr;
>>>>>>> v4.9.227
	int err = 0;
	unsigned int arg;

	/* Step 1 : check if triggers are trivially valid */

<<<<<<< HEAD
	err |= cfc_check_trigger_src(&cmd->start_src, TRIG_NOW | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->scan_begin_src,
					TRIG_FOLLOW | TRIG_TIMER | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->convert_src,
					TRIG_TIMER | TRIG_NOW | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->scan_end_src, TRIG_COUNT);
	err |= cfc_check_trigger_src(&cmd->stop_src, TRIG_COUNT | TRIG_NONE);
=======
	err |= comedi_check_trigger_src(&cmd->start_src, TRIG_NOW | TRIG_EXT);
	err |= comedi_check_trigger_src(&cmd->scan_begin_src,
					TRIG_FOLLOW | TRIG_TIMER | TRIG_EXT);
	err |= comedi_check_trigger_src(&cmd->convert_src,
					TRIG_TIMER | TRIG_NOW | TRIG_EXT);
	err |= comedi_check_trigger_src(&cmd->scan_end_src, TRIG_COUNT);
	err |= comedi_check_trigger_src(&cmd->stop_src, TRIG_COUNT | TRIG_NONE);
>>>>>>> v4.9.227

	if (err)
		return 1;

	/* Step 2a : make sure trigger sources are unique */

<<<<<<< HEAD
	err |= cfc_check_trigger_is_unique(cmd->start_src);
	err |= cfc_check_trigger_is_unique(cmd->scan_begin_src);
	err |= cfc_check_trigger_is_unique(cmd->convert_src);
	err |= cfc_check_trigger_is_unique(cmd->stop_src);
=======
	err |= comedi_check_trigger_is_unique(cmd->start_src);
	err |= comedi_check_trigger_is_unique(cmd->scan_begin_src);
	err |= comedi_check_trigger_is_unique(cmd->convert_src);
	err |= comedi_check_trigger_is_unique(cmd->stop_src);
>>>>>>> v4.9.227

	/* Step 2b : and mutually compatible */

	if (cmd->scan_begin_src == TRIG_FOLLOW && cmd->convert_src == TRIG_NOW)
		err |= -EINVAL;
	if (cmd->scan_begin_src != TRIG_FOLLOW && cmd->convert_src != TRIG_NOW)
		err |= -EINVAL;
	if (cmd->start_src == TRIG_EXT &&
	    (cmd->convert_src == TRIG_EXT || cmd->scan_begin_src == TRIG_EXT))
		err |= -EINVAL;

	if (err)
		return 2;

	/* Step 3: check if arguments are trivially valid */

	switch (cmd->start_src) {
	case TRIG_NOW:
<<<<<<< HEAD
		err |= cfc_check_trigger_arg_is(&cmd->start_arg, 0);
=======
		err |= comedi_check_trigger_arg_is(&cmd->start_arg, 0);
>>>>>>> v4.9.227
		break;
	case TRIG_EXT:
		/* External trigger, only CR_EDGE and CR_INVERT flags allowed */
		if ((cmd->start_arg
		     & (CR_FLAGS_MASK & ~(CR_EDGE | CR_INVERT))) != 0) {
			cmd->start_arg &= ~(CR_FLAGS_MASK &
						~(CR_EDGE | CR_INVERT));
			err |= -EINVAL;
		}
<<<<<<< HEAD
		if (!thisboard->is_1602 && (cmd->start_arg & CR_INVERT)) {
=======
		if (!board->is_1602 && (cmd->start_arg & CR_INVERT)) {
>>>>>>> v4.9.227
			cmd->start_arg &= (CR_FLAGS_MASK & ~CR_INVERT);
			err |= -EINVAL;
		}
		break;
	}

<<<<<<< HEAD
	if (cmd->scan_begin_src == TRIG_TIMER)
		err |= cfc_check_trigger_arg_min(&cmd->scan_begin_arg,
				thisboard->ai_speed * cmd->chanlist_len);

	if (cmd->convert_src == TRIG_TIMER)
		err |= cfc_check_trigger_arg_min(&cmd->convert_arg,
						 thisboard->ai_speed);

	err |= cfc_check_trigger_arg_is(&cmd->scan_end_arg, cmd->chanlist_len);

	if (cmd->stop_src == TRIG_COUNT)
		err |= cfc_check_trigger_arg_min(&cmd->stop_arg, 1);
	else	/* TRIG_NONE */
		err |= cfc_check_trigger_arg_is(&cmd->stop_arg, 0);
=======
	if (cmd->scan_begin_src == TRIG_TIMER) {
		err |= comedi_check_trigger_arg_min(&cmd->scan_begin_arg,
						    board->ai_speed *
						    cmd->chanlist_len);
	}

	if (cmd->convert_src == TRIG_TIMER) {
		err |= comedi_check_trigger_arg_min(&cmd->convert_arg,
						    board->ai_speed);
	}

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

	if (cmd->scan_begin_src == TRIG_TIMER) {
		arg = cmd->scan_begin_arg;
<<<<<<< HEAD
		i8253_cascade_ns_to_timer(I8254_OSC_BASE_10MHZ,
					  &devpriv->divisor1,
					  &devpriv->divisor2,
					  &arg, cmd->flags);
		err |= cfc_check_trigger_arg_is(&cmd->scan_begin_arg, arg);
	}
	if (cmd->convert_src == TRIG_TIMER) {
		arg = cmd->convert_arg;
		i8253_cascade_ns_to_timer(I8254_OSC_BASE_10MHZ,
					  &devpriv->divisor1,
					  &devpriv->divisor2,
					  &arg, cmd->flags);
		err |= cfc_check_trigger_arg_is(&cmd->convert_arg, arg);
=======
		comedi_8254_cascade_ns_to_timer(dev->pacer, &arg, cmd->flags);
		err |= comedi_check_trigger_arg_is(&cmd->scan_begin_arg, arg);
	}
	if (cmd->convert_src == TRIG_TIMER) {
		arg = cmd->convert_arg;
		comedi_8254_cascade_ns_to_timer(dev->pacer, &arg, cmd->flags);
		err |= comedi_check_trigger_arg_is(&cmd->convert_arg, arg);
>>>>>>> v4.9.227
	}

	if (err)
		return 4;

	/* Step 5: check channel list if it exists */
	if (cmd->chanlist && cmd->chanlist_len > 0)
		err |= cb_pcidas_ai_check_chanlist(dev, s, cmd);

	if (err)
		return 5;

	return 0;
}

<<<<<<< HEAD
static void cb_pcidas_ai_load_counters(struct comedi_device *dev)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned long timer_base = dev->iobase + ADC8254;

	i8254_set_mode(timer_base, 0, 1, I8254_MODE2 | I8254_BINARY);
	i8254_set_mode(timer_base, 0, 2, I8254_MODE2 | I8254_BINARY);

	i8254_write(timer_base, 0, 1, devpriv->divisor1);
	i8254_write(timer_base, 0, 2, devpriv->divisor2);
}

static int cb_pcidas_ai_cmd(struct comedi_device *dev,
			    struct comedi_subdevice *s)
{
	const struct cb_pcidas_board *thisboard = dev->board_ptr;
	struct cb_pcidas_private *devpriv = dev->private;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &async->cmd;
	unsigned int bits;
	unsigned long flags;

	/*  make sure CAL_EN_BIT is disabled */
	outw(0, devpriv->control_status + CALIBRATION_REG);
	/*  initialize before settings pacer source and count values */
	outw(0, devpriv->control_status + TRIG_CONTSTAT);
	/*  clear fifo */
	outw(0, devpriv->adc_fifo + ADCFIFOCLR);

	/*  set mux limits, gain and pacer source */
	bits = BEGIN_SCAN(CR_CHAN(cmd->chanlist[0])) |
	    END_SCAN(CR_CHAN(cmd->chanlist[cmd->chanlist_len - 1])) |
	    GAIN_BITS(CR_RANGE(cmd->chanlist[0]));
	/*  set unipolar/bipolar */
	if (CR_RANGE(cmd->chanlist[0]) & IS_UNIPOLAR)
		bits |= UNIP;
	/*  set singleended/differential */
	if (CR_AREF(cmd->chanlist[0]) != AREF_DIFF)
		bits |= SE;
	/*  set pacer source */
	if (cmd->convert_src == TRIG_EXT || cmd->scan_begin_src == TRIG_EXT)
		bits |= PACER_EXT_RISE;
	else
		bits |= PACER_INT;
	outw(bits, devpriv->control_status + ADCMUX_CONT);

	/*  load counters */
	if (cmd->scan_begin_src == TRIG_TIMER || cmd->convert_src == TRIG_TIMER)
		cb_pcidas_ai_load_counters(dev);

	/*  set number of conversions */
	if (cmd->stop_src == TRIG_COUNT)
		devpriv->count = cmd->chanlist_len * cmd->stop_arg;
	/*  enable interrupts */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->adc_fifo_bits |= INTE;
	devpriv->adc_fifo_bits &= ~INT_MASK;
	if (cmd->flags & CMDF_WAKE_EOS) {
		if (cmd->convert_src == TRIG_NOW && cmd->chanlist_len > 1) {
			/* interrupt end of burst */
			devpriv->adc_fifo_bits |= INT_EOS;
		} else {
			/* interrupt fifo not empty */
			devpriv->adc_fifo_bits |= INT_FNE;
		}
	} else {
		/* interrupt fifo half full */
		devpriv->adc_fifo_bits |= INT_FHF;
	}

	/*  enable (and clear) interrupts */
	outw(devpriv->adc_fifo_bits | EOAI | INT | LADFUL,
	     devpriv->control_status + INT_ADCFIFO);
=======
static int cb_pcidas_ai_cmd(struct comedi_device *dev,
			    struct comedi_subdevice *s)
{
	const struct cb_pcidas_board *board = dev->board_ptr;
	struct cb_pcidas_private *devpriv = dev->private;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &async->cmd;
	unsigned int range0 = CR_RANGE(cmd->chanlist[0]);
	unsigned int bits;
	unsigned long flags;

	/*  make sure PCIDAS_CALIB_EN is disabled */
	outw(0, devpriv->pcibar1 + PCIDAS_CALIB_REG);
	/*  initialize before settings pacer source and count values */
	outw(PCIDAS_TRIG_SEL_NONE, devpriv->pcibar1 + PCIDAS_TRIG_REG);
	/*  clear fifo */
	outw(0, devpriv->pcibar2 + PCIDAS_AI_FIFO_CLR_REG);

	/*  set mux limits, gain and pacer source */
	bits = PCIDAS_AI_FIRST(CR_CHAN(cmd->chanlist[0])) |
	       PCIDAS_AI_LAST(CR_CHAN(cmd->chanlist[cmd->chanlist_len - 1])) |
	       PCIDAS_AI_GAIN(range0);
	/*  set unipolar/bipolar */
	if (comedi_range_is_unipolar(s, range0))
		bits |= PCIDAS_AI_UNIP;
	/*  set singleended/differential */
	if (CR_AREF(cmd->chanlist[0]) != AREF_DIFF)
		bits |= PCIDAS_AI_SE;
	/*  set pacer source */
	if (cmd->convert_src == TRIG_EXT || cmd->scan_begin_src == TRIG_EXT)
		bits |= PCIDAS_AI_PACER_EXTP;
	else
		bits |= PCIDAS_AI_PACER_INT;
	outw(bits, devpriv->pcibar1 + PCIDAS_AI_REG);

	/*  load counters */
	if (cmd->scan_begin_src == TRIG_TIMER ||
	    cmd->convert_src == TRIG_TIMER) {
		comedi_8254_update_divisors(dev->pacer);
		comedi_8254_pacer_enable(dev->pacer, 1, 2, true);
	}

	/*  enable interrupts */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->ctrl |= PCIDAS_CTRL_INTE;
	devpriv->ctrl &= ~PCIDAS_CTRL_INT_MASK;
	if (cmd->flags & CMDF_WAKE_EOS) {
		if (cmd->convert_src == TRIG_NOW && cmd->chanlist_len > 1) {
			/* interrupt end of burst */
			devpriv->ctrl |= PCIDAS_CTRL_INT_EOS;
		} else {
			/* interrupt fifo not empty */
			devpriv->ctrl |= PCIDAS_CTRL_INT_FNE;
		}
	} else {
		/* interrupt fifo half full */
		devpriv->ctrl |= PCIDAS_CTRL_INT_FHF;
	}

	/*  enable (and clear) interrupts */
	outw(devpriv->ctrl |
	     PCIDAS_CTRL_EOAI | PCIDAS_CTRL_INT_CLR | PCIDAS_CTRL_LADFUL,
	     devpriv->pcibar1 + PCIDAS_CTRL_REG);
>>>>>>> v4.9.227
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/*  set start trigger and burst mode */
	bits = 0;
	if (cmd->start_src == TRIG_NOW) {
<<<<<<< HEAD
		bits |= SW_TRIGGER;
	} else {	/* TRIG_EXT */
		bits |= EXT_TRIGGER | TGEN | XTRCL;
		if (thisboard->is_1602) {
			if (cmd->start_arg & CR_INVERT)
				bits |= TGPOL;
			if (cmd->start_arg & CR_EDGE)
				bits |= TGSEL;
		}
	}
	if (cmd->convert_src == TRIG_NOW && cmd->chanlist_len > 1)
		bits |= BURSTE;
	outw(bits, devpriv->control_status + TRIG_CONTSTAT);
=======
		bits |= PCIDAS_TRIG_SEL_SW;
	} else {	/* TRIG_EXT */
		bits |= PCIDAS_TRIG_SEL_EXT | PCIDAS_TRIG_EN | PCIDAS_TRIG_CLR;
		if (board->is_1602) {
			if (cmd->start_arg & CR_INVERT)
				bits |= PCIDAS_TRIG_POL;
			if (cmd->start_arg & CR_EDGE)
				bits |= PCIDAS_TRIG_MODE;
		}
	}
	if (cmd->convert_src == TRIG_NOW && cmd->chanlist_len > 1)
		bits |= PCIDAS_TRIG_BURSTE;
	outw(bits, devpriv->pcibar1 + PCIDAS_TRIG_REG);
>>>>>>> v4.9.227

	return 0;
}

static int cb_pcidas_ao_check_chanlist(struct comedi_device *dev,
				       struct comedi_subdevice *s,
				       struct comedi_cmd *cmd)
{
	unsigned int chan0 = CR_CHAN(cmd->chanlist[0]);

	if (cmd->chanlist_len > 1) {
		unsigned int chan1 = CR_CHAN(cmd->chanlist[1]);

		if (chan0 != 0 || chan1 != 1) {
			dev_dbg(dev->class_dev,
				"channels must be ordered channel 0, channel 1 in chanlist\n");
			return -EINVAL;
		}
	}

	return 0;
}

static int cb_pcidas_ao_cmdtest(struct comedi_device *dev,
				struct comedi_subdevice *s,
				struct comedi_cmd *cmd)
{
<<<<<<< HEAD
	const struct cb_pcidas_board *thisboard = dev->board_ptr;
	struct cb_pcidas_private *devpriv = dev->private;
	int err = 0;
	unsigned int arg;

	/* Step 1 : check if triggers are trivially valid */

	err |= cfc_check_trigger_src(&cmd->start_src, TRIG_INT);
	err |= cfc_check_trigger_src(&cmd->scan_begin_src,
					TRIG_TIMER | TRIG_EXT);
	err |= cfc_check_trigger_src(&cmd->convert_src, TRIG_NOW);
	err |= cfc_check_trigger_src(&cmd->scan_end_src, TRIG_COUNT);
	err |= cfc_check_trigger_src(&cmd->stop_src, TRIG_COUNT | TRIG_NONE);
=======
	const struct cb_pcidas_board *board = dev->board_ptr;
	struct cb_pcidas_private *devpriv = dev->private;
	int err = 0;

	/* Step 1 : check if triggers are trivially valid */

	err |= comedi_check_trigger_src(&cmd->start_src, TRIG_INT);
	err |= comedi_check_trigger_src(&cmd->scan_begin_src,
					TRIG_TIMER | TRIG_EXT);
	err |= comedi_check_trigger_src(&cmd->convert_src, TRIG_NOW);
	err |= comedi_check_trigger_src(&cmd->scan_end_src, TRIG_COUNT);
	err |= comedi_check_trigger_src(&cmd->stop_src, TRIG_COUNT | TRIG_NONE);
>>>>>>> v4.9.227

	if (err)
		return 1;

	/* Step 2a : make sure trigger sources are unique */

<<<<<<< HEAD
	err |= cfc_check_trigger_is_unique(cmd->scan_begin_src);
	err |= cfc_check_trigger_is_unique(cmd->stop_src);
=======
	err |= comedi_check_trigger_is_unique(cmd->scan_begin_src);
	err |= comedi_check_trigger_is_unique(cmd->stop_src);
>>>>>>> v4.9.227

	/* Step 2b : and mutually compatible */

	if (err)
		return 2;

	/* Step 3: check if arguments are trivially valid */

<<<<<<< HEAD
	err |= cfc_check_trigger_arg_is(&cmd->start_arg, 0);

	if (cmd->scan_begin_src == TRIG_TIMER)
		err |= cfc_check_trigger_arg_min(&cmd->scan_begin_arg,
						 thisboard->ao_scan_speed);

	err |= cfc_check_trigger_arg_is(&cmd->scan_end_arg, cmd->chanlist_len);

	if (cmd->stop_src == TRIG_COUNT)
		err |= cfc_check_trigger_arg_min(&cmd->stop_arg, 1);
	else	/* TRIG_NONE */
		err |= cfc_check_trigger_arg_is(&cmd->stop_arg, 0);
=======
	err |= comedi_check_trigger_arg_is(&cmd->start_arg, 0);

	if (cmd->scan_begin_src == TRIG_TIMER) {
		err |= comedi_check_trigger_arg_min(&cmd->scan_begin_arg,
						    board->ao_scan_speed);
	}

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

	if (cmd->scan_begin_src == TRIG_TIMER) {
<<<<<<< HEAD
		arg = cmd->scan_begin_arg;
		i8253_cascade_ns_to_timer(I8254_OSC_BASE_10MHZ,
					  &devpriv->ao_divisor1,
					  &devpriv->ao_divisor2,
					  &arg, cmd->flags);
		err |= cfc_check_trigger_arg_is(&cmd->scan_begin_arg, arg);
=======
		unsigned int arg = cmd->scan_begin_arg;

		comedi_8254_cascade_ns_to_timer(devpriv->ao_pacer,
						&arg, cmd->flags);
		err |= comedi_check_trigger_arg_is(&cmd->scan_begin_arg, arg);
>>>>>>> v4.9.227
	}

	if (err)
		return 4;

	/* Step 5: check channel list if it exists */
	if (cmd->chanlist && cmd->chanlist_len > 0)
		err |= cb_pcidas_ao_check_chanlist(dev, s, cmd);

	if (err)
		return 5;

	return 0;
}

<<<<<<< HEAD
/* cancel analog input command */
static int cb_pcidas_cancel(struct comedi_device *dev,
			    struct comedi_subdevice *s)
=======
static int cb_pcidas_ai_cancel(struct comedi_device *dev,
			       struct comedi_subdevice *s)
>>>>>>> v4.9.227
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned long flags;

	spin_lock_irqsave(&dev->spinlock, flags);
	/*  disable interrupts */
<<<<<<< HEAD
	devpriv->adc_fifo_bits &= ~INTE & ~EOAIE;
	outw(devpriv->adc_fifo_bits, devpriv->control_status + INT_ADCFIFO);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/*  disable start trigger source and burst mode */
	outw(0, devpriv->control_status + TRIG_CONTSTAT);
	/*  software pacer source */
	outw(0, devpriv->control_status + ADCMUX_CONT);
=======
	devpriv->ctrl &= ~(PCIDAS_CTRL_INTE | PCIDAS_CTRL_EOAIE);
	outw(devpriv->ctrl, devpriv->pcibar1 + PCIDAS_CTRL_REG);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/*  disable start trigger source and burst mode */
	outw(PCIDAS_TRIG_SEL_NONE, devpriv->pcibar1 + PCIDAS_TRIG_REG);
	outw(PCIDAS_AI_PACER_SW, devpriv->pcibar1 + PCIDAS_AI_REG);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
=======
static void cb_pcidas_ao_load_fifo(struct comedi_device *dev,
				   struct comedi_subdevice *s,
				   unsigned int nsamples)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int nbytes;

	nsamples = comedi_nsamples_left(s, nsamples);
	nbytes = comedi_buf_read_samples(s, devpriv->ao_buffer, nsamples);

	nsamples = comedi_bytes_to_samples(s, nbytes);
	outsw(devpriv->pcibar4 + PCIDAS_AO_FIFO_REG,
	      devpriv->ao_buffer, nsamples);
}

>>>>>>> v4.9.227
static int cb_pcidas_ao_inttrig(struct comedi_device *dev,
				struct comedi_subdevice *s,
				unsigned int trig_num)
{
<<<<<<< HEAD
	const struct cb_pcidas_board *thisboard = dev->board_ptr;
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int num_bytes, num_points = thisboard->fifo_size;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &s->async->cmd;
=======
	const struct cb_pcidas_board *board = dev->board_ptr;
	struct cb_pcidas_private *devpriv = dev->private;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &async->cmd;
>>>>>>> v4.9.227
	unsigned long flags;

	if (trig_num != cmd->start_arg)
		return -EINVAL;

<<<<<<< HEAD
	/*  load up fifo */
	if (cmd->stop_src == TRIG_COUNT && devpriv->ao_count < num_points)
		num_points = devpriv->ao_count;

	num_bytes = cfc_read_array_from_buffer(s, devpriv->ao_buffer,
					       num_points * sizeof(short));
	num_points = num_bytes / sizeof(short);

	if (cmd->stop_src == TRIG_COUNT)
		devpriv->ao_count -= num_points;
	/*  write data to board's fifo */
	outsw(devpriv->ao_registers + DACDATA, devpriv->ao_buffer, num_bytes);

	/*  enable dac half-full and empty interrupts */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->adc_fifo_bits |= DAEMIE | DAHFIE;

	/*  enable and clear interrupts */
	outw(devpriv->adc_fifo_bits | DAEMI | DAHFI,
	     devpriv->control_status + INT_ADCFIFO);

	/*  start dac */
	devpriv->ao_control_bits |= DAC_START | DACEN | DAC_EMPTY;
	outw(devpriv->ao_control_bits, devpriv->control_status + DAC_CSR);
=======
	cb_pcidas_ao_load_fifo(dev, s, board->fifo_size);

	/*  enable dac half-full and empty interrupts */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->ctrl |= PCIDAS_CTRL_DAEMIE | PCIDAS_CTRL_DAHFIE;

	/*  enable and clear interrupts */
	outw(devpriv->ctrl | PCIDAS_CTRL_DAEMI | PCIDAS_CTRL_DAHFI,
	     devpriv->pcibar1 + PCIDAS_CTRL_REG);

	/*  start dac */
	devpriv->ao_ctrl |= PCIDAS_AO_START | PCIDAS_AO_DACEN | PCIDAS_AO_EMPTY;
	outw(devpriv->ao_ctrl, devpriv->pcibar1 + PCIDAS_AO_REG);
>>>>>>> v4.9.227

	spin_unlock_irqrestore(&dev->spinlock, flags);

	async->inttrig = NULL;

	return 0;
}

<<<<<<< HEAD
static void cb_pcidas_ao_load_counters(struct comedi_device *dev)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned long timer_base = dev->iobase + DAC8254;

	i8254_set_mode(timer_base, 0, 1, I8254_MODE2 | I8254_BINARY);
	i8254_set_mode(timer_base, 0, 2, I8254_MODE2 | I8254_BINARY);

	i8254_write(timer_base, 0, 1, devpriv->ao_divisor1);
	i8254_write(timer_base, 0, 2, devpriv->ao_divisor2);
}

=======
>>>>>>> v4.9.227
static int cb_pcidas_ao_cmd(struct comedi_device *dev,
			    struct comedi_subdevice *s)
{
	struct cb_pcidas_private *devpriv = dev->private;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &async->cmd;
	unsigned int i;
	unsigned long flags;

	/*  set channel limits, gain */
	spin_lock_irqsave(&dev->spinlock, flags);
	for (i = 0; i < cmd->chanlist_len; i++) {
<<<<<<< HEAD
		/*  enable channel */
		devpriv->ao_control_bits |=
		    DAC_CHAN_EN(CR_CHAN(cmd->chanlist[i]));
		/*  set range */
		devpriv->ao_control_bits |= DAC_RANGE(CR_CHAN(cmd->chanlist[i]),
						      CR_RANGE(cmd->
							       chanlist[i]));
	}

	/*  disable analog out before settings pacer source and count values */
	outw(devpriv->ao_control_bits, devpriv->control_status + DAC_CSR);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/*  clear fifo */
	outw(0, devpriv->ao_registers + DACFIFOCLR);

	/*  load counters */
	if (cmd->scan_begin_src == TRIG_TIMER)
		cb_pcidas_ao_load_counters(dev);

	/*  set number of conversions */
	if (cmd->stop_src == TRIG_COUNT)
		devpriv->ao_count = cmd->chanlist_len * cmd->stop_arg;
=======
		unsigned int chan = CR_CHAN(cmd->chanlist[i]);
		unsigned int range = CR_RANGE(cmd->chanlist[i]);

		/*  enable channel */
		devpriv->ao_ctrl |= PCIDAS_AO_CHAN_EN(chan);
		/*  set range */
		devpriv->ao_ctrl |= PCIDAS_AO_RANGE(chan, range);
	}

	/*  disable analog out before settings pacer source and count values */
	outw(devpriv->ao_ctrl, devpriv->pcibar1 + PCIDAS_AO_REG);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/*  clear fifo */
	outw(0, devpriv->pcibar4 + PCIDAS_AO_FIFO_CLR_REG);

	/*  load counters */
	if (cmd->scan_begin_src == TRIG_TIMER) {
		comedi_8254_update_divisors(devpriv->ao_pacer);
		comedi_8254_pacer_enable(devpriv->ao_pacer, 1, 2, true);
	}

>>>>>>> v4.9.227
	/*  set pacer source */
	spin_lock_irqsave(&dev->spinlock, flags);
	switch (cmd->scan_begin_src) {
	case TRIG_TIMER:
<<<<<<< HEAD
		devpriv->ao_control_bits |= DAC_PACER_INT;
		break;
	case TRIG_EXT:
		devpriv->ao_control_bits |= DAC_PACER_EXT_RISE;
=======
		devpriv->ao_ctrl |= PCIDAS_AO_PACER_INT;
		break;
	case TRIG_EXT:
		devpriv->ao_ctrl |= PCIDAS_AO_PACER_EXTP;
>>>>>>> v4.9.227
		break;
	default:
		spin_unlock_irqrestore(&dev->spinlock, flags);
		dev_err(dev->class_dev, "error setting dac pacer source\n");
		return -1;
	}
	spin_unlock_irqrestore(&dev->spinlock, flags);

	async->inttrig = cb_pcidas_ao_inttrig;

	return 0;
}

<<<<<<< HEAD
/* cancel analog output command */
=======
>>>>>>> v4.9.227
static int cb_pcidas_ao_cancel(struct comedi_device *dev,
			       struct comedi_subdevice *s)
{
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned long flags;

	spin_lock_irqsave(&dev->spinlock, flags);
	/*  disable interrupts */
<<<<<<< HEAD
	devpriv->adc_fifo_bits &= ~DAHFIE & ~DAEMIE;
	outw(devpriv->adc_fifo_bits, devpriv->control_status + INT_ADCFIFO);

	/*  disable output */
	devpriv->ao_control_bits &= ~DACEN & ~DAC_PACER_MASK;
	outw(devpriv->ao_control_bits, devpriv->control_status + DAC_CSR);
=======
	devpriv->ctrl &= ~(PCIDAS_CTRL_DAHFIE | PCIDAS_CTRL_DAEMIE);
	outw(devpriv->ctrl, devpriv->pcibar1 + PCIDAS_CTRL_REG);

	/*  disable output */
	devpriv->ao_ctrl &= ~(PCIDAS_AO_DACEN | PCIDAS_AO_PACER_MASK);
	outw(devpriv->ao_ctrl, devpriv->pcibar1 + PCIDAS_AO_REG);
>>>>>>> v4.9.227
	spin_unlock_irqrestore(&dev->spinlock, flags);

	return 0;
}

<<<<<<< HEAD
static void handle_ao_interrupt(struct comedi_device *dev, unsigned int status)
{
	const struct cb_pcidas_board *thisboard = dev->board_ptr;
=======
static unsigned int cb_pcidas_ao_interrupt(struct comedi_device *dev,
					   unsigned int status)
{
	const struct cb_pcidas_board *board = dev->board_ptr;
>>>>>>> v4.9.227
	struct cb_pcidas_private *devpriv = dev->private;
	struct comedi_subdevice *s = dev->write_subdev;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &async->cmd;
<<<<<<< HEAD
	unsigned int half_fifo = thisboard->fifo_size / 2;
	unsigned int num_points;
	unsigned long flags;

	if (status & DAEMI) {
		/*  clear dac empty interrupt latch */
		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->adc_fifo_bits | DAEMI,
		     devpriv->control_status + INT_ADCFIFO);
		spin_unlock_irqrestore(&dev->spinlock, flags);
		if (inw(devpriv->ao_registers + DAC_CSR) & DAC_EMPTY) {
			if (cmd->stop_src == TRIG_NONE ||
			    (cmd->stop_src == TRIG_COUNT
			     && devpriv->ao_count)) {
				dev_err(dev->class_dev, "dac fifo underflow\n");
				async->events |= COMEDI_CB_ERROR;
			}
			async->events |= COMEDI_CB_EOA;
		}
	} else if (status & DAHFI) {
		unsigned int num_bytes;

		/*  figure out how many points we are writing to fifo */
		num_points = half_fifo;
		if (cmd->stop_src == TRIG_COUNT &&
		    devpriv->ao_count < num_points)
			num_points = devpriv->ao_count;
		num_bytes =
		    cfc_read_array_from_buffer(s, devpriv->ao_buffer,
					       num_points * sizeof(short));
		num_points = num_bytes / sizeof(short);

		if (cmd->stop_src == TRIG_COUNT)
			devpriv->ao_count -= num_points;
		/*  write data to board's fifo */
		outsw(devpriv->ao_registers + DACDATA, devpriv->ao_buffer,
		      num_points);
		/*  clear half-full interrupt latch */
		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->adc_fifo_bits | DAHFI,
		     devpriv->control_status + INT_ADCFIFO);
		spin_unlock_irqrestore(&dev->spinlock, flags);
	}

	cfc_handle_events(dev, s);
}

static irqreturn_t cb_pcidas_interrupt(int irq, void *d)
{
	struct comedi_device *dev = (struct comedi_device *)d;
	const struct cb_pcidas_board *thisboard = dev->board_ptr;
	struct cb_pcidas_private *devpriv = dev->private;
	struct comedi_subdevice *s = dev->read_subdev;
	struct comedi_async *async;
	struct comedi_cmd *cmd;
	int status, s5933_status;
	int half_fifo = thisboard->fifo_size / 2;
	unsigned int num_samples, i;
	static const int timeout = 10000;
	unsigned long flags;

	if (!dev->attached)
		return IRQ_NONE;

	async = s->async;
	cmd = &async->cmd;

	s5933_status = inl(devpriv->s5933_config + AMCC_OP_REG_INTCSR);

	if ((INTCSR_INTR_ASSERTED & s5933_status) == 0)
		return IRQ_NONE;

	/*  make sure mailbox 4 is empty */
	inl_p(devpriv->s5933_config + AMCC_OP_REG_IMB4);
	/*  clear interrupt on amcc s5933 */
	outl(devpriv->s5933_intcsr_bits | INTCSR_INBOX_INTR_STATUS,
	     devpriv->s5933_config + AMCC_OP_REG_INTCSR);

	status = inw(devpriv->control_status + INT_ADCFIFO);

	/*  check for analog output interrupt */
	if (status & (DAHFI | DAEMI))
		handle_ao_interrupt(dev, status);
	/*  check for analog input interrupts */
	/*  if fifo half-full */
	if (status & ADHFI) {
		/*  read data */
		num_samples = half_fifo;
		if (cmd->stop_src == TRIG_COUNT &&
		    num_samples > devpriv->count) {
			num_samples = devpriv->count;
		}
		insw(devpriv->adc_fifo + ADCDATA, devpriv->ai_buffer,
		     num_samples);
		cfc_write_array_to_buffer(s, devpriv->ai_buffer,
					  num_samples * sizeof(short));
		devpriv->count -= num_samples;
		if (cmd->stop_src == TRIG_COUNT && devpriv->count == 0)
			async->events |= COMEDI_CB_EOA;
		/*  clear half-full interrupt latch */
		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->adc_fifo_bits | INT,
		     devpriv->control_status + INT_ADCFIFO);
		spin_unlock_irqrestore(&dev->spinlock, flags);
		/*  else if fifo not empty */
	} else if (status & (ADNEI | EOBI)) {
		for (i = 0; i < timeout; i++) {
			/*  break if fifo is empty */
			if ((ADNE & inw(devpriv->control_status +
					INT_ADCFIFO)) == 0)
				break;
			cfc_write_to_buffer(s, inw(devpriv->adc_fifo));
			if (cmd->stop_src == TRIG_COUNT &&
			    --devpriv->count == 0) {
				/* end of acquisition */
=======
	unsigned int irq_clr = 0;

	if (status & PCIDAS_CTRL_DAEMI) {
		irq_clr |= PCIDAS_CTRL_DAEMI;

		if (inw(devpriv->pcibar4 + PCIDAS_AO_REG) & PCIDAS_AO_EMPTY) {
			if (cmd->stop_src == TRIG_COUNT &&
			    async->scans_done >= cmd->stop_arg) {
				async->events |= COMEDI_CB_EOA;
			} else {
				dev_err(dev->class_dev, "dac fifo underflow\n");
				async->events |= COMEDI_CB_ERROR;
			}
		}
	} else if (status & PCIDAS_CTRL_DAHFI) {
		irq_clr |= PCIDAS_CTRL_DAHFI;

		cb_pcidas_ao_load_fifo(dev, s, board->fifo_size / 2);
	}

	comedi_handle_events(dev, s);

	return irq_clr;
}

static unsigned int cb_pcidas_ai_interrupt(struct comedi_device *dev,
					   unsigned int status)
{
	const struct cb_pcidas_board *board = dev->board_ptr;
	struct cb_pcidas_private *devpriv = dev->private;
	struct comedi_subdevice *s = dev->read_subdev;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &async->cmd;
	unsigned int irq_clr = 0;

	if (status & PCIDAS_CTRL_ADHFI) {
		unsigned int num_samples;

		irq_clr |= PCIDAS_CTRL_INT_CLR;

		/* FIFO is half-full - read data */
		num_samples = comedi_nsamples_left(s, board->fifo_size / 2);
		insw(devpriv->pcibar2 + PCIDAS_AI_DATA_REG,
		     devpriv->ai_buffer, num_samples);
		comedi_buf_write_samples(s, devpriv->ai_buffer, num_samples);

		if (cmd->stop_src == TRIG_COUNT &&
		    async->scans_done >= cmd->stop_arg)
			async->events |= COMEDI_CB_EOA;
	} else if (status & (PCIDAS_CTRL_ADNEI | PCIDAS_CTRL_EOBI)) {
		unsigned int i;

		irq_clr |= PCIDAS_CTRL_INT_CLR;

		/* FIFO is not empty - read data until empty or timeoout */
		for (i = 0; i < 10000; i++) {
			unsigned short val;

			/*  break if fifo is empty */
			if ((inw(devpriv->pcibar1 + PCIDAS_CTRL_REG) &
			    PCIDAS_CTRL_ADNE) == 0)
				break;
			val = inw(devpriv->pcibar2 + PCIDAS_AI_DATA_REG);
			comedi_buf_write_samples(s, &val, 1);

			if (cmd->stop_src == TRIG_COUNT &&
			    async->scans_done >= cmd->stop_arg) {
>>>>>>> v4.9.227
				async->events |= COMEDI_CB_EOA;
				break;
			}
		}
<<<<<<< HEAD
		/*  clear not-empty interrupt latch */
		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->adc_fifo_bits | INT,
		     devpriv->control_status + INT_ADCFIFO);
		spin_unlock_irqrestore(&dev->spinlock, flags);
	} else if (status & EOAI) {
		dev_err(dev->class_dev,
			"bug! encountered end of acquisition interrupt?\n");
		/*  clear EOA interrupt latch */
		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->adc_fifo_bits | EOAI,
		     devpriv->control_status + INT_ADCFIFO);
		spin_unlock_irqrestore(&dev->spinlock, flags);
	}
	/* check for fifo overflow */
	if (status & LADFUL) {
		dev_err(dev->class_dev, "fifo overflow\n");
		/*  clear overflow interrupt latch */
		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->adc_fifo_bits | LADFUL,
		     devpriv->control_status + INT_ADCFIFO);
		spin_unlock_irqrestore(&dev->spinlock, flags);
		async->events |= COMEDI_CB_EOA | COMEDI_CB_ERROR;
	}

	cfc_handle_events(dev, s);
=======
	} else if (status & PCIDAS_CTRL_EOAI) {
		irq_clr |= PCIDAS_CTRL_EOAI;

		dev_err(dev->class_dev,
			"bug! encountered end of acquisition interrupt?\n");
	}

	/* check for fifo overflow */
	if (status & PCIDAS_CTRL_LADFUL) {
		irq_clr |= PCIDAS_CTRL_LADFUL;

		dev_err(dev->class_dev, "fifo overflow\n");
		async->events |= COMEDI_CB_ERROR;
	}

	comedi_handle_events(dev, s);

	return irq_clr;
}

static irqreturn_t cb_pcidas_interrupt(int irq, void *d)
{
	struct comedi_device *dev = d;
	struct cb_pcidas_private *devpriv = dev->private;
	unsigned int irq_clr = 0;
	unsigned int amcc_status;
	unsigned int status;

	if (!dev->attached)
		return IRQ_NONE;

	amcc_status = inl(devpriv->amcc + AMCC_OP_REG_INTCSR);

	if ((INTCSR_INTR_ASSERTED & amcc_status) == 0)
		return IRQ_NONE;

	/*  make sure mailbox 4 is empty */
	inl_p(devpriv->amcc + AMCC_OP_REG_IMB4);
	/*  clear interrupt on amcc s5933 */
	outl(devpriv->amcc_intcsr | INTCSR_INBOX_INTR_STATUS,
	     devpriv->amcc + AMCC_OP_REG_INTCSR);

	status = inw(devpriv->pcibar1 + PCIDAS_CTRL_REG);

	/* handle analog output interrupts */
	if (status & PCIDAS_CTRL_AO_INT)
		irq_clr |= cb_pcidas_ao_interrupt(dev, status);

	/* handle analog input interrupts */
	if (status & PCIDAS_CTRL_AI_INT)
		irq_clr |= cb_pcidas_ai_interrupt(dev, status);

	if (irq_clr) {
		unsigned long flags;

		spin_lock_irqsave(&dev->spinlock, flags);
		outw(devpriv->ctrl | irq_clr,
		     devpriv->pcibar1 + PCIDAS_CTRL_REG);
		spin_unlock_irqrestore(&dev->spinlock, flags);
	}
>>>>>>> v4.9.227

	return IRQ_HANDLED;
}

static int cb_pcidas_auto_attach(struct comedi_device *dev,
				 unsigned long context)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
<<<<<<< HEAD
	const struct cb_pcidas_board *thisboard = NULL;
=======
	const struct cb_pcidas_board *board = NULL;
>>>>>>> v4.9.227
	struct cb_pcidas_private *devpriv;
	struct comedi_subdevice *s;
	int i;
	int ret;

	if (context < ARRAY_SIZE(cb_pcidas_boards))
<<<<<<< HEAD
		thisboard = &cb_pcidas_boards[context];
	if (!thisboard)
		return -ENODEV;
	dev->board_ptr  = thisboard;
	dev->board_name = thisboard->name;
=======
		board = &cb_pcidas_boards[context];
	if (!board)
		return -ENODEV;
	dev->board_ptr  = board;
	dev->board_name = board->name;
>>>>>>> v4.9.227

	devpriv = comedi_alloc_devpriv(dev, sizeof(*devpriv));
	if (!devpriv)
		return -ENOMEM;

	ret = comedi_pci_enable(dev);
	if (ret)
		return ret;

<<<<<<< HEAD
	devpriv->s5933_config = pci_resource_start(pcidev, 0);
	devpriv->control_status = pci_resource_start(pcidev, 1);
	devpriv->adc_fifo = pci_resource_start(pcidev, 2);
	dev->iobase = pci_resource_start(pcidev, 3);
	if (thisboard->ao_nchan)
		devpriv->ao_registers = pci_resource_start(pcidev, 4);

	/*  disable and clear interrupts on amcc s5933 */
	outl(INTCSR_INBOX_INTR_STATUS,
	     devpriv->s5933_config + AMCC_OP_REG_INTCSR);
=======
	devpriv->amcc = pci_resource_start(pcidev, 0);
	devpriv->pcibar1 = pci_resource_start(pcidev, 1);
	devpriv->pcibar2 = pci_resource_start(pcidev, 2);
	dev->iobase = pci_resource_start(pcidev, 3);
	if (board->has_ao)
		devpriv->pcibar4 = pci_resource_start(pcidev, 4);

	/*  disable and clear interrupts on amcc s5933 */
	outl(INTCSR_INBOX_INTR_STATUS,
	     devpriv->amcc + AMCC_OP_REG_INTCSR);
>>>>>>> v4.9.227

	ret = request_irq(pcidev->irq, cb_pcidas_interrupt, IRQF_SHARED,
			  dev->board_name, dev);
	if (ret) {
		dev_dbg(dev->class_dev, "unable to allocate irq %d\n",
			pcidev->irq);
		return ret;
	}
	dev->irq = pcidev->irq;

<<<<<<< HEAD
=======
	dev->pacer = comedi_8254_init(dev->iobase + PCIDAS_AI_8254_BASE,
				      I8254_OSC_BASE_10MHZ, I8254_IO8, 0);
	if (!dev->pacer)
		return -ENOMEM;

	devpriv->ao_pacer = comedi_8254_init(dev->iobase + PCIDAS_AO_8254_BASE,
					     I8254_OSC_BASE_10MHZ,
					     I8254_IO8, 0);
	if (!devpriv->ao_pacer)
		return -ENOMEM;

>>>>>>> v4.9.227
	ret = comedi_alloc_subdevices(dev, 7);
	if (ret)
		return ret;

<<<<<<< HEAD
	s = &dev->subdevices[0];
	/* analog input subdevice */
	dev->read_subdev = s;
	s->type = COMEDI_SUBD_AI;
	s->subdev_flags = SDF_READABLE | SDF_GROUND | SDF_DIFF | SDF_CMD_READ;
	/* WARNING: Number of inputs in differential mode is ignored */
	s->n_chan = thisboard->ai_nchan;
	s->len_chanlist = thisboard->ai_nchan;
	s->maxdata = (1 << thisboard->ai_bits) - 1;
	s->range_table = thisboard->ranges;
	s->insn_read = cb_pcidas_ai_rinsn;
	s->insn_config = ai_config_insn;
	s->do_cmd = cb_pcidas_ai_cmd;
	s->do_cmdtest = cb_pcidas_ai_cmdtest;
	s->cancel = cb_pcidas_cancel;

	/* analog output subdevice */
	s = &dev->subdevices[1];
	if (thisboard->ao_nchan) {
		s->type = COMEDI_SUBD_AO;
		s->subdev_flags = SDF_READABLE | SDF_WRITABLE | SDF_GROUND;
		s->n_chan = thisboard->ao_nchan;
		/*
		 * analog out resolution is the same as
		 * analog input resolution, so use ai_bits
		 */
		s->maxdata = (1 << thisboard->ai_bits) - 1;
		s->range_table = &cb_pcidas_ao_ranges;
		/* default to no fifo (*insn_write) */
		s->insn_write = cb_pcidas_ao_nofifo_winsn;
		s->insn_read = comedi_readback_insn_read;
=======
	/* Analog Input subdevice */
	s = &dev->subdevices[0];
	s->type		= COMEDI_SUBD_AI;
	s->subdev_flags	= SDF_READABLE | SDF_GROUND | SDF_DIFF;
	s->n_chan	= 16;
	s->maxdata	= board->is_16bit ? 0xffff : 0x0fff;
	s->range_table	= board->use_alt_range ? &cb_pcidas_alt_ranges
					       : &cb_pcidas_ranges;
	s->insn_read	= cb_pcidas_ai_insn_read;
	s->insn_config	= cb_pcidas_ai_insn_config;
	if (dev->irq) {
		dev->read_subdev = s;
		s->subdev_flags	|= SDF_CMD_READ;
		s->len_chanlist	= s->n_chan;
		s->do_cmd	= cb_pcidas_ai_cmd;
		s->do_cmdtest	= cb_pcidas_ai_cmdtest;
		s->cancel	= cb_pcidas_ai_cancel;
	}

	/* Analog Output subdevice */
	s = &dev->subdevices[1];
	if (board->has_ao) {
		s->type		= COMEDI_SUBD_AO;
		s->subdev_flags	= SDF_WRITABLE | SDF_GROUND;
		s->n_chan	= 2;
		s->maxdata	= board->is_16bit ? 0xffff : 0x0fff;
		s->range_table	= &cb_pcidas_ao_ranges;
		s->insn_write	= (board->has_ao_fifo)
					? cb_pcidas_ao_fifo_insn_write
					: cb_pcidas_ao_nofifo_insn_write;
>>>>>>> v4.9.227

		ret = comedi_alloc_subdev_readback(s);
		if (ret)
			return ret;

<<<<<<< HEAD
		if (thisboard->has_ao_fifo) {
			dev->write_subdev = s;
			s->subdev_flags |= SDF_CMD_WRITE;
			/* use fifo (*insn_write) instead */
			s->insn_write = cb_pcidas_ao_fifo_winsn;
			s->do_cmdtest = cb_pcidas_ao_cmdtest;
			s->do_cmd = cb_pcidas_ao_cmd;
			s->cancel = cb_pcidas_ao_cancel;
		}
	} else {
		s->type = COMEDI_SUBD_UNUSED;
=======
		if (dev->irq && board->has_ao_fifo) {
			dev->write_subdev = s;
			s->subdev_flags	|= SDF_CMD_WRITE;
			s->do_cmdtest	= cb_pcidas_ao_cmdtest;
			s->do_cmd	= cb_pcidas_ao_cmd;
			s->cancel	= cb_pcidas_ao_cancel;
		}
	} else {
		s->type		= COMEDI_SUBD_UNUSED;
>>>>>>> v4.9.227
	}

	/* 8255 */
	s = &dev->subdevices[2];
<<<<<<< HEAD
	ret = subdev_8255_init(dev, s, NULL, DIO_8255);
	if (ret)
		return ret;

	/*  serial EEPROM, */
	s = &dev->subdevices[3];
	s->type = COMEDI_SUBD_MEMORY;
	s->subdev_flags = SDF_READABLE | SDF_INTERNAL;
	s->n_chan = 256;
	s->maxdata = 0xff;
	s->insn_read = eeprom_read_insn;

	/*  8800 caldac */
	s = &dev->subdevices[4];
	s->type = COMEDI_SUBD_CALIB;
	s->subdev_flags = SDF_READABLE | SDF_WRITABLE | SDF_INTERNAL;
	s->n_chan = NUM_CHANNELS_8800;
	s->maxdata = 0xff;
	s->insn_read = caldac_read_insn;
	s->insn_write = caldac_write_insn;
	for (i = 0; i < s->n_chan; i++)
		caldac_8800_write(dev, i, s->maxdata / 2);

	/*  trim potentiometer */
	s = &dev->subdevices[5];
	s->type = COMEDI_SUBD_CALIB;
	s->subdev_flags = SDF_READABLE | SDF_WRITABLE | SDF_INTERNAL;
	if (thisboard->trimpot == AD7376) {
		s->n_chan = NUM_CHANNELS_7376;
		s->maxdata = 0x7f;
	} else {
		s->n_chan = NUM_CHANNELS_8402;
		s->maxdata = 0xff;
	}
	s->insn_read = trimpot_read_insn;
	s->insn_write = trimpot_write_insn;
	for (i = 0; i < s->n_chan; i++)
		cb_pcidas_trimpot_write(dev, i, s->maxdata / 2);

	/*  dac08 caldac */
	s = &dev->subdevices[6];
	if (thisboard->has_dac08) {
		s->type = COMEDI_SUBD_CALIB;
		s->subdev_flags = SDF_READABLE | SDF_WRITABLE | SDF_INTERNAL;
		s->n_chan = NUM_CHANNELS_DAC08;
		s->insn_read = dac08_read_insn;
		s->insn_write = dac08_write_insn;
		s->maxdata = 0xff;
		dac08_write(dev, s->maxdata / 2);
	} else
		s->type = COMEDI_SUBD_UNUSED;

	/*  make sure mailbox 4 is empty */
	inl(devpriv->s5933_config + AMCC_OP_REG_IMB4);
	/* Set bits to enable incoming mailbox interrupts on amcc s5933. */
	devpriv->s5933_intcsr_bits =
	    INTCSR_INBOX_BYTE(3) | INTCSR_INBOX_SELECT(3) |
	    INTCSR_INBOX_FULL_INT;
	/*  clear and enable interrupt on amcc s5933 */
	outl(devpriv->s5933_intcsr_bits | INTCSR_INBOX_INTR_STATUS,
	     devpriv->s5933_config + AMCC_OP_REG_INTCSR);
=======
	ret = subdev_8255_init(dev, s, NULL, PCIDAS_8255_BASE);
	if (ret)
		return ret;

	/* Memory subdevice - serial EEPROM */
	s = &dev->subdevices[3];
	s->type		= COMEDI_SUBD_MEMORY;
	s->subdev_flags	= SDF_READABLE | SDF_INTERNAL;
	s->n_chan	= 256;
	s->maxdata	= 0xff;
	s->insn_read	= cb_pcidas_eeprom_insn_read;

	/* Calibration subdevice - 8800 caldac */
	s = &dev->subdevices[4];
	s->type		= COMEDI_SUBD_CALIB;
	s->subdev_flags	= SDF_WRITABLE | SDF_INTERNAL;
	s->n_chan	= 8;
	s->maxdata	= 0xff;
	s->insn_write	= cb_pcidas_caldac_insn_write;

	ret = comedi_alloc_subdev_readback(s);
	if (ret)
		return ret;

	for (i = 0; i < s->n_chan; i++) {
		unsigned int val = s->maxdata / 2;

		/* write 11-bit channel/value to caldac */
		cb_pcidas_calib_write(dev, (i << 8) | val, 11, false);
		s->readback[i] = val;
	}

	/* Calibration subdevice - trim potentiometer */
	s = &dev->subdevices[5];
	s->type		= COMEDI_SUBD_CALIB;
	s->subdev_flags	= SDF_WRITABLE | SDF_INTERNAL;
	if (board->has_ad8402) {
		/*
		 * pci-das1602/16 have an AD8402 trimpot:
		 *   chan 0 : adc gain
		 *   chan 1 : adc postgain offset
		 */
		s->n_chan	= 2;
		s->maxdata	= 0xff;
	} else {
		/* all other boards have an AD7376 trimpot */
		s->n_chan	= 1;
		s->maxdata	= 0x7f;
	}
	s->insn_write	= cb_pcidas_trimpot_insn_write;

	ret = comedi_alloc_subdev_readback(s);
	if (ret)
		return ret;

	for (i = 0; i < s->n_chan; i++) {
		cb_pcidas_trimpot_write(dev, i, s->maxdata / 2);
		s->readback[i] = s->maxdata / 2;
	}

	/* Calibration subdevice - pci-das1602/16 pregain offset (dac08) */
	s = &dev->subdevices[6];
	if (board->has_dac08) {
		s->type		= COMEDI_SUBD_CALIB;
		s->subdev_flags	= SDF_WRITABLE | SDF_INTERNAL;
		s->n_chan	= 1;
		s->maxdata	= 0xff;
		s->insn_write	= cb_pcidas_dac08_insn_write;

		ret = comedi_alloc_subdev_readback(s);
		if (ret)
			return ret;

		for (i = 0; i < s->n_chan; i++) {
			cb_pcidas_dac08_write(dev, s->maxdata / 2);
			s->readback[i] = s->maxdata / 2;
		}
	} else {
		s->type		= COMEDI_SUBD_UNUSED;
	}

	/*  make sure mailbox 4 is empty */
	inl(devpriv->amcc + AMCC_OP_REG_IMB4);
	/* Set bits to enable incoming mailbox interrupts on amcc s5933. */
	devpriv->amcc_intcsr = INTCSR_INBOX_BYTE(3) | INTCSR_INBOX_SELECT(3) |
			       INTCSR_INBOX_FULL_INT;
	/*  clear and enable interrupt on amcc s5933 */
	outl(devpriv->amcc_intcsr | INTCSR_INBOX_INTR_STATUS,
	     devpriv->amcc + AMCC_OP_REG_INTCSR);
>>>>>>> v4.9.227

	return 0;
}

static void cb_pcidas_detach(struct comedi_device *dev)
{
	struct cb_pcidas_private *devpriv = dev->private;

<<<<<<< HEAD
	if (devpriv && devpriv->s5933_config) {
		outl(INTCSR_INBOX_INTR_STATUS,
		     devpriv->s5933_config + AMCC_OP_REG_INTCSR);
=======
	if (devpriv) {
		if (devpriv->amcc)
			outl(INTCSR_INBOX_INTR_STATUS,
			     devpriv->amcc + AMCC_OP_REG_INTCSR);
		kfree(devpriv->ao_pacer);
>>>>>>> v4.9.227
	}
	comedi_pci_detach(dev);
}

static struct comedi_driver cb_pcidas_driver = {
	.driver_name	= "cb_pcidas",
	.module		= THIS_MODULE,
	.auto_attach	= cb_pcidas_auto_attach,
	.detach		= cb_pcidas_detach,
};

static int cb_pcidas_pci_probe(struct pci_dev *dev,
			       const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &cb_pcidas_driver,
				      id->driver_data);
}

static const struct pci_device_id cb_pcidas_pci_table[] = {
	{ PCI_VDEVICE(CB, 0x0001), BOARD_PCIDAS1602_16 },
	{ PCI_VDEVICE(CB, 0x000f), BOARD_PCIDAS1200 },
	{ PCI_VDEVICE(CB, 0x0010), BOARD_PCIDAS1602_12 },
	{ PCI_VDEVICE(CB, 0x0019), BOARD_PCIDAS1200_JR },
	{ PCI_VDEVICE(CB, 0x001c), BOARD_PCIDAS1602_16_JR },
	{ PCI_VDEVICE(CB, 0x004c), BOARD_PCIDAS1000 },
	{ PCI_VDEVICE(CB, 0x001a), BOARD_PCIDAS1001 },
	{ PCI_VDEVICE(CB, 0x001b), BOARD_PCIDAS1002 },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, cb_pcidas_pci_table);

static struct pci_driver cb_pcidas_pci_driver = {
	.name		= "cb_pcidas",
	.id_table	= cb_pcidas_pci_table,
	.probe		= cb_pcidas_pci_probe,
	.remove		= comedi_pci_auto_unconfig,
};
module_comedi_pci_driver(cb_pcidas_driver, cb_pcidas_pci_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
<<<<<<< HEAD
MODULE_DESCRIPTION("Comedi low-level driver");
=======
MODULE_DESCRIPTION("Comedi driver for MeasurementComputing PCI-DAS series");
>>>>>>> v4.9.227
MODULE_LICENSE("GPL");
