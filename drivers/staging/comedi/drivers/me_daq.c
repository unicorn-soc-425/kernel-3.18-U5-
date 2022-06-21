/*
 * comedi/drivers/me_daq.c
 * Hardware driver for Meilhaus data acquisition cards:
 *   ME-2000i, ME-2600i, ME-3000vm1
 *
 * Copyright (C) 2002 Michael Hillmann <hillmann@syscongroup.de>
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
 * Driver: me_daq
 * Description: Meilhaus PCI data acquisition cards
<<<<<<< HEAD
 * Devices: (Meilhaus) ME-2600i [me-2600i]
 *          (Meilhaus) ME-2000i [me-2000i]
=======
 * Devices: [Meilhaus] ME-2600i (me-2600i), ME-2000i (me-2000i)
>>>>>>> v4.9.227
 * Author: Michael Hillmann <hillmann@syscongroup.de>
 * Status: experimental
 *
 * Configuration options: not applicable, uses PCI auto config
 *
 * Supports:
 *    Analog Input, Analog Output, Digital I/O
 */

#include <linux/module.h>
<<<<<<< HEAD
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

#include "../comedidev.h"
=======
#include <linux/interrupt.h>
#include <linux/sched.h>

#include "../comedi_pci.h"
>>>>>>> v4.9.227

#include "plx9052.h"

#define ME2600_FIRMWARE		"me2600_firmware.bin"

#define XILINX_DOWNLOAD_RESET	0x42	/* Xilinx registers */

<<<<<<< HEAD
#define ME_CONTROL_1			0x0000	/* - | W */
#define   INTERRUPT_ENABLE		(1<<15)
#define   COUNTER_B_IRQ			(1<<12)
#define   COUNTER_A_IRQ			(1<<11)
#define   CHANLIST_READY_IRQ		(1<<10)
#define   EXT_IRQ			(1<<9)
#define   ADFIFO_HALFFULL_IRQ		(1<<8)
#define   SCAN_COUNT_ENABLE		(1<<5)
#define   SIMULTANEOUS_ENABLE		(1<<4)
#define   TRIGGER_FALLING_EDGE		(1<<3)
#define   CONTINUOUS_MODE		(1<<2)
#define   DISABLE_ADC			(0<<0)
#define   SOFTWARE_TRIGGERED_ADC	(1<<0)
#define   SCAN_TRIGGERED_ADC		(2<<0)
#define   EXT_TRIGGERED_ADC		(3<<0)
#define ME_ADC_START			0x0000	/* R | - */
#define ME_CONTROL_2			0x0002	/* - | W */
#define   ENABLE_ADFIFO			(1<<10)
#define   ENABLE_CHANLIST		(1<<9)
#define   ENABLE_PORT_B			(1<<7)
#define   ENABLE_PORT_A			(1<<6)
#define   ENABLE_COUNTER_B		(1<<4)
#define   ENABLE_COUNTER_A		(1<<3)
#define   ENABLE_DAC			(1<<1)
#define   BUFFERED_DAC			(1<<0)
#define ME_DAC_UPDATE			0x0002	/* R | - */
#define ME_STATUS			0x0004	/* R | - */
#define   COUNTER_B_IRQ_PENDING		(1<<12)
#define   COUNTER_A_IRQ_PENDING		(1<<11)
#define   CHANLIST_READY_IRQ_PENDING	(1<<10)
#define   EXT_IRQ_PENDING		(1<<9)
#define   ADFIFO_HALFFULL_IRQ_PENDING	(1<<8)
#define   ADFIFO_FULL			(1<<4)
#define   ADFIFO_HALFFULL		(1<<3)
#define   ADFIFO_EMPTY			(1<<2)
#define   CHANLIST_FULL			(1<<1)
#define   FST_ACTIVE			(1<<0)
#define ME_RESET_INTERRUPT		0x0004	/* - | W */
#define ME_DIO_PORT_A			0x0006	/* R | W */
#define ME_DIO_PORT_B			0x0008	/* R | W */
#define ME_TIMER_DATA_0			0x000A	/* - | W */
#define ME_TIMER_DATA_1			0x000C	/* - | W */
#define ME_TIMER_DATA_2			0x000E	/* - | W */
#define ME_CHANNEL_LIST			0x0010	/* - | W */
#define   ADC_UNIPOLAR			(1<<6)
#define   ADC_GAIN_0			(0<<4)
#define   ADC_GAIN_1			(1<<4)
#define   ADC_GAIN_2			(2<<4)
#define   ADC_GAIN_3			(3<<4)
#define ME_READ_AD_FIFO			0x0010	/* R | - */
#define ME_DAC_CONTROL			0x0012	/* - | W */
#define   DAC_UNIPOLAR_D		(0<<4)
#define   DAC_BIPOLAR_D			(1<<4)
#define   DAC_UNIPOLAR_C		(0<<5)
#define   DAC_BIPOLAR_C			(1<<5)
#define   DAC_UNIPOLAR_B		(0<<6)
#define   DAC_BIPOLAR_B			(1<<6)
#define   DAC_UNIPOLAR_A		(0<<7)
#define   DAC_BIPOLAR_A			(1<<7)
#define   DAC_GAIN_0_D			(0<<8)
#define   DAC_GAIN_1_D			(1<<8)
#define   DAC_GAIN_0_C			(0<<9)
#define   DAC_GAIN_1_C			(1<<9)
#define   DAC_GAIN_0_B			(0<<10)
#define   DAC_GAIN_1_B			(1<<10)
#define   DAC_GAIN_0_A			(0<<11)
#define   DAC_GAIN_1_A			(1<<11)
#define ME_DAC_CONTROL_UPDATE		0x0012	/* R | - */
#define ME_DAC_DATA_A			0x0014	/* - | W */
#define ME_DAC_DATA_B			0x0016	/* - | W */
#define ME_DAC_DATA_C			0x0018	/* - | W */
#define ME_DAC_DATA_D			0x001A	/* - | W */
#define ME_COUNTER_ENDDATA_A		0x001C	/* - | W */
#define ME_COUNTER_ENDDATA_B		0x001E	/* - | W */
#define ME_COUNTER_STARTDATA_A		0x0020	/* - | W */
#define ME_COUNTER_VALUE_A		0x0020	/* R | - */
#define ME_COUNTER_STARTDATA_B		0x0022	/* - | W */
#define ME_COUNTER_VALUE_B		0x0022	/* R | - */
=======
/*
 * PCI BAR2 Memory map (dev->mmio)
 */
#define ME_CTRL1_REG			0x00	/* R (ai start) | W */
#define   ME_CTRL1_INT_ENA		BIT(15)
#define   ME_CTRL1_COUNTER_B_IRQ	BIT(12)
#define   ME_CTRL1_COUNTER_A_IRQ	BIT(11)
#define   ME_CTRL1_CHANLIST_READY_IRQ	BIT(10)
#define   ME_CTRL1_EXT_IRQ		BIT(9)
#define   ME_CTRL1_ADFIFO_HALFFULL_IRQ	BIT(8)
#define   ME_CTRL1_SCAN_COUNT_ENA	BIT(5)
#define   ME_CTRL1_SIMULTANEOUS_ENA	BIT(4)
#define   ME_CTRL1_TRIGGER_FALLING_EDGE	BIT(3)
#define   ME_CTRL1_CONTINUOUS_MODE	BIT(2)
#define   ME_CTRL1_ADC_MODE(x)		(((x) & 0x3) << 0)
#define   ME_CTRL1_ADC_MODE_DISABLE	ME_CTRL1_ADC_MODE(0)
#define   ME_CTRL1_ADC_MODE_SOFT_TRIG	ME_CTRL1_ADC_MODE(1)
#define   ME_CTRL1_ADC_MODE_SCAN_TRIG	ME_CTRL1_ADC_MODE(2)
#define   ME_CTRL1_ADC_MODE_EXT_TRIG	ME_CTRL1_ADC_MODE(3)
#define   ME_CTRL1_ADC_MODE_MASK	ME_CTRL1_ADC_MODE(3)
#define ME_CTRL2_REG			0x02	/* R (dac update) | W */
#define   ME_CTRL2_ADFIFO_ENA		BIT(10)
#define   ME_CTRL2_CHANLIST_ENA		BIT(9)
#define   ME_CTRL2_PORT_B_ENA		BIT(7)
#define   ME_CTRL2_PORT_A_ENA		BIT(6)
#define   ME_CTRL2_COUNTER_B_ENA	BIT(4)
#define   ME_CTRL2_COUNTER_A_ENA	BIT(3)
#define   ME_CTRL2_DAC_ENA		BIT(1)
#define   ME_CTRL2_BUFFERED_DAC		BIT(0)
#define ME_STATUS_REG			0x04	/* R | W (clears interrupts) */
#define   ME_STATUS_COUNTER_B_IRQ	BIT(12)
#define   ME_STATUS_COUNTER_A_IRQ	BIT(11)
#define   ME_STATUS_CHANLIST_READY_IRQ	BIT(10)
#define   ME_STATUS_EXT_IRQ		BIT(9)
#define   ME_STATUS_ADFIFO_HALFFULL_IRQ	BIT(8)
#define   ME_STATUS_ADFIFO_FULL		BIT(4)
#define   ME_STATUS_ADFIFO_HALFFULL	BIT(3)
#define   ME_STATUS_ADFIFO_EMPTY	BIT(2)
#define   ME_STATUS_CHANLIST_FULL	BIT(1)
#define   ME_STATUS_FST_ACTIVE		BIT(0)
#define ME_DIO_PORT_A_REG		0x06	/* R | W */
#define ME_DIO_PORT_B_REG		0x08	/* R | W */
#define ME_TIMER_DATA_REG(x)		(0x0a + ((x) * 2))	/* - | W */
#define ME_AI_FIFO_REG			0x10	/* R (fifo) | W (chanlist) */
#define   ME_AI_FIFO_CHANLIST_DIFF	BIT(7)
#define   ME_AI_FIFO_CHANLIST_UNIPOLAR	BIT(6)
#define   ME_AI_FIFO_CHANLIST_GAIN(x)	(((x) & 0x3) << 4)
#define   ME_AI_FIFO_CHANLIST_CHAN(x)	(((x) & 0xf) << 0)
#define ME_DAC_CTRL_REG			0x12	/* R (updates) | W */
#define   ME_DAC_CTRL_BIPOLAR(x)	BIT(7 - ((x) & 0x3))
#define   ME_DAC_CTRL_GAIN(x)		BIT(11 - ((x) & 0x3))
#define   ME_DAC_CTRL_MASK(x)		(ME_DAC_CTRL_BIPOLAR(x) |	\
					 ME_DAC_CTRL_GAIN(x))
#define ME_AO_DATA_REG(x)		(0x14 + ((x) * 2))	/* - | W */
#define ME_COUNTER_ENDDATA_REG(x)	(0x1c + ((x) * 2))	/* - | W */
#define ME_COUNTER_STARTDATA_REG(x)	(0x20 + ((x) * 2))	/* - | W */
#define ME_COUNTER_VALUE_REG(x)		(0x20 + ((x) * 2))	/* R | - */
>>>>>>> v4.9.227

static const struct comedi_lrange me_ai_range = {
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

static const struct comedi_lrange me_ao_range = {
	3, {
		BIP_RANGE(10),
		BIP_RANGE(5),
		UNI_RANGE(10)
	}
};

enum me_boardid {
	BOARD_ME2600,
	BOARD_ME2000,
};

struct me_board {
	const char *name;
	int needs_firmware;
	int has_ao;
};

static const struct me_board me_boards[] = {
	[BOARD_ME2600] = {
		.name		= "me-2600i",
		.needs_firmware	= 1,
		.has_ao		= 1,
	},
	[BOARD_ME2000] = {
		.name		= "me-2000i",
	},
};

struct me_private_data {
	void __iomem *plx_regbase;	/* PLX configuration base address */

<<<<<<< HEAD
	unsigned short control_1;	/* Mirror of CONTROL_1 register */
	unsigned short control_2;	/* Mirror of CONTROL_2 register */
	unsigned short dac_control;	/* Mirror of the DAC_CONTROL register */
};

static inline void sleep(unsigned sec)
{
	current->state = TASK_INTERRUPTIBLE;
	schedule_timeout(sec * HZ);
=======
	unsigned short ctrl1;		/* Mirror of CONTROL_1 register */
	unsigned short ctrl2;		/* Mirror of CONTROL_2 register */
	unsigned short dac_ctrl;	/* Mirror of the DAC_CONTROL register */
};

static inline void sleep(unsigned int sec)
{
	schedule_timeout_interruptible(sec * HZ);
>>>>>>> v4.9.227
}

static int me_dio_insn_config(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_insn *insn,
			      unsigned int *data)
{
	struct me_private_data *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int mask;
	int ret;

	if (chan < 16)
		mask = 0x0000ffff;
	else
		mask = 0xffff0000;

	ret = comedi_dio_insn_config(dev, s, insn, data, mask);
	if (ret)
		return ret;

	if (s->io_bits & 0x0000ffff)
<<<<<<< HEAD
		devpriv->control_2 |= ENABLE_PORT_A;
	else
		devpriv->control_2 &= ~ENABLE_PORT_A;
	if (s->io_bits & 0xffff0000)
		devpriv->control_2 |= ENABLE_PORT_B;
	else
		devpriv->control_2 &= ~ENABLE_PORT_B;

	writew(devpriv->control_2, dev->mmio + ME_CONTROL_2);
=======
		devpriv->ctrl2 |= ME_CTRL2_PORT_A_ENA;
	else
		devpriv->ctrl2 &= ~ME_CTRL2_PORT_A_ENA;
	if (s->io_bits & 0xffff0000)
		devpriv->ctrl2 |= ME_CTRL2_PORT_B_ENA;
	else
		devpriv->ctrl2 &= ~ME_CTRL2_PORT_B_ENA;

	writew(devpriv->ctrl2, dev->mmio + ME_CTRL2_REG);
>>>>>>> v4.9.227

	return insn->n;
}

static int me_dio_insn_bits(struct comedi_device *dev,
			    struct comedi_subdevice *s,
			    struct comedi_insn *insn,
			    unsigned int *data)
{
<<<<<<< HEAD
	void __iomem *mmio_porta = dev->mmio + ME_DIO_PORT_A;
	void __iomem *mmio_portb = dev->mmio + ME_DIO_PORT_B;
=======
	void __iomem *mmio_porta = dev->mmio + ME_DIO_PORT_A_REG;
	void __iomem *mmio_portb = dev->mmio + ME_DIO_PORT_B_REG;
>>>>>>> v4.9.227
	unsigned int mask;
	unsigned int val;

	mask = comedi_dio_update_state(s, data);
	if (mask) {
		if (mask & 0x0000ffff)
			writew((s->state & 0xffff), mmio_porta);
		if (mask & 0xffff0000)
			writew(((s->state >> 16) & 0xffff), mmio_portb);
	}

	if (s->io_bits & 0x0000ffff)
		val = s->state & 0xffff;
	else
		val = readw(mmio_porta);

	if (s->io_bits & 0xffff0000)
		val |= (s->state & 0xffff0000);
	else
		val |= (readw(mmio_portb) << 16);

	data[1] = val;

	return insn->n;
}

static int me_ai_eoc(struct comedi_device *dev,
		     struct comedi_subdevice *s,
		     struct comedi_insn *insn,
		     unsigned long context)
{
	unsigned int status;

<<<<<<< HEAD
	status = readw(dev->mmio + ME_STATUS);
	if ((status & 0x0004) == 0)
=======
	status = readw(dev->mmio + ME_STATUS_REG);
	if ((status & ME_STATUS_ADFIFO_EMPTY) == 0)
>>>>>>> v4.9.227
		return 0;
	return -EBUSY;
}

static int me_ai_insn_read(struct comedi_device *dev,
			   struct comedi_subdevice *s,
			   struct comedi_insn *insn,
			   unsigned int *data)
{
<<<<<<< HEAD
	struct me_private_data *dev_private = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int rang = CR_RANGE(insn->chanspec);
	unsigned int aref = CR_AREF(insn->chanspec);
	unsigned short val;
	int ret;

	/* stop any running conversion */
	dev_private->control_1 &= 0xFFFC;
	writew(dev_private->control_1, dev->mmio + ME_CONTROL_1);

	/* clear chanlist and ad fifo */
	dev_private->control_2 &= ~(ENABLE_ADFIFO | ENABLE_CHANLIST);
	writew(dev_private->control_2, dev->mmio + ME_CONTROL_2);

	/* reset any pending interrupt */
	writew(0x00, dev->mmio + ME_RESET_INTERRUPT);

	/* enable the chanlist and ADC fifo */
	dev_private->control_2 |= (ENABLE_ADFIFO | ENABLE_CHANLIST);
	writew(dev_private->control_2, dev->mmio + ME_CONTROL_2);

	/* write to channel list fifo */
	val = chan & 0x0f;			/* b3:b0 channel */
	val |= (rang & 0x03) << 4;		/* b5:b4 gain */
	val |= (rang & 0x04) << 4;		/* b6 polarity */
	val |= ((aref & AREF_DIFF) ? 0x80 : 0);	/* b7 differential */
	writew(val & 0xff, dev->mmio + ME_CHANNEL_LIST);

	/* set ADC mode to software trigger */
	dev_private->control_1 |= SOFTWARE_TRIGGERED_ADC;
	writew(dev_private->control_1, dev->mmio + ME_CONTROL_1);

	/* start conversion by reading from ADC_START */
	readw(dev->mmio + ME_ADC_START);

	/* wait for ADC fifo not empty flag */
	ret = comedi_timeout(dev, s, insn, me_ai_eoc, 0);
	if (ret)
		return ret;

	/* get value from ADC fifo */
	val = readw(dev->mmio + ME_READ_AD_FIFO);
	val = (val ^ 0x800) & 0x0fff;
	data[0] = val;

	/* stop any running conversion */
	dev_private->control_1 &= 0xFFFC;
	writew(dev_private->control_1, dev->mmio + ME_CONTROL_1);

	return 1;
=======
	struct me_private_data *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int range = CR_RANGE(insn->chanspec);
	unsigned int aref = CR_AREF(insn->chanspec);
	unsigned int val;
	int ret = 0;
	int i;

	/*
	 * For differential operation, there are only 8 input channels
	 * and only bipolar ranges are available.
	 */
	if (aref & AREF_DIFF) {
		if (chan > 7 || comedi_range_is_unipolar(s, range))
			return -EINVAL;
	}

	/* clear chanlist and ad fifo */
	devpriv->ctrl2 &= ~(ME_CTRL2_ADFIFO_ENA | ME_CTRL2_CHANLIST_ENA);
	writew(devpriv->ctrl2, dev->mmio + ME_CTRL2_REG);

	writew(0x00, dev->mmio + ME_STATUS_REG);	/* clear interrupts */

	/* enable the chanlist and ADC fifo */
	devpriv->ctrl2 |= (ME_CTRL2_ADFIFO_ENA | ME_CTRL2_CHANLIST_ENA);
	writew(devpriv->ctrl2, dev->mmio + ME_CTRL2_REG);

	/* write to channel list fifo */
	val = ME_AI_FIFO_CHANLIST_CHAN(chan) | ME_AI_FIFO_CHANLIST_GAIN(range);
	if (comedi_range_is_unipolar(s, range))
		val |= ME_AI_FIFO_CHANLIST_UNIPOLAR;
	if (aref & AREF_DIFF)
		val |= ME_AI_FIFO_CHANLIST_DIFF;
	writew(val, dev->mmio + ME_AI_FIFO_REG);

	/* set ADC mode to software trigger */
	devpriv->ctrl1 |= ME_CTRL1_ADC_MODE_SOFT_TRIG;
	writew(devpriv->ctrl1, dev->mmio + ME_CTRL1_REG);

	for (i = 0; i < insn->n; i++) {
		/* start ai conversion */
		readw(dev->mmio + ME_CTRL1_REG);

		/* wait for ADC fifo not empty flag */
		ret = comedi_timeout(dev, s, insn, me_ai_eoc, 0);
		if (ret)
			break;

		/* get value from ADC fifo */
		val = readw(dev->mmio + ME_AI_FIFO_REG) & s->maxdata;

		/* munge 2's complement value to offset binary */
		data[i] = comedi_offset_munge(s, val);
	}

	/* stop any running conversion */
	devpriv->ctrl1 &= ~ME_CTRL1_ADC_MODE_MASK;
	writew(devpriv->ctrl1, dev->mmio + ME_CTRL1_REG);

	return ret ? ret : insn->n;
>>>>>>> v4.9.227
}

static int me_ao_insn_write(struct comedi_device *dev,
			    struct comedi_subdevice *s,
			    struct comedi_insn *insn,
			    unsigned int *data)
{
<<<<<<< HEAD
	struct me_private_data *dev_private = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int rang = CR_RANGE(insn->chanspec);
=======
	struct me_private_data *devpriv = dev->private;
	unsigned int chan = CR_CHAN(insn->chanspec);
	unsigned int range = CR_RANGE(insn->chanspec);
>>>>>>> v4.9.227
	unsigned int val = s->readback[chan];
	int i;

	/* Enable all DAC */
<<<<<<< HEAD
	dev_private->control_2 |= ENABLE_DAC;
	writew(dev_private->control_2, dev->mmio + ME_CONTROL_2);

	/* and set DAC to "buffered" mode */
	dev_private->control_2 |= BUFFERED_DAC;
	writew(dev_private->control_2, dev->mmio + ME_CONTROL_2);

	/* Set dac-control register */
	for (i = 0; i < insn->n; i++) {
		/* clear bits for this channel */
		dev_private->dac_control &= ~(0x0880 >> chan);
		if (rang == 0)
			dev_private->dac_control |=
			    ((DAC_BIPOLAR_A | DAC_GAIN_1_A) >> chan);
		else if (rang == 1)
			dev_private->dac_control |=
			    ((DAC_BIPOLAR_A | DAC_GAIN_0_A) >> chan);
	}
	writew(dev_private->dac_control, dev->mmio + ME_DAC_CONTROL);

	/* Update dac-control register */
	readw(dev->mmio + ME_DAC_CONTROL_UPDATE);
=======
	devpriv->ctrl2 |= ME_CTRL2_DAC_ENA;
	writew(devpriv->ctrl2, dev->mmio + ME_CTRL2_REG);

	/* and set DAC to "buffered" mode */
	devpriv->ctrl2 |= ME_CTRL2_BUFFERED_DAC;
	writew(devpriv->ctrl2, dev->mmio + ME_CTRL2_REG);

	/* Set dac-control register */
	devpriv->dac_ctrl &= ~ME_DAC_CTRL_MASK(chan);
	if (range == 0)
		devpriv->dac_ctrl |= ME_DAC_CTRL_GAIN(chan);
	if (comedi_range_is_bipolar(s, range))
		devpriv->dac_ctrl |= ME_DAC_CTRL_BIPOLAR(chan);
	writew(devpriv->dac_ctrl, dev->mmio + ME_DAC_CTRL_REG);

	/* Update dac-control register */
	readw(dev->mmio + ME_DAC_CTRL_REG);
>>>>>>> v4.9.227

	/* Set data register */
	for (i = 0; i < insn->n; i++) {
		val = data[i];

<<<<<<< HEAD
		writew(val, dev->mmio + ME_DAC_DATA_A + (chan << 1));
=======
		writew(val, dev->mmio + ME_AO_DATA_REG(chan));
>>>>>>> v4.9.227
	}
	s->readback[chan] = val;

	/* Update dac with data registers */
<<<<<<< HEAD
	readw(dev->mmio + ME_DAC_UPDATE);
=======
	readw(dev->mmio + ME_CTRL2_REG);
>>>>>>> v4.9.227

	return insn->n;
}

static int me2600_xilinx_download(struct comedi_device *dev,
				  const u8 *data, size_t size,
				  unsigned long context)
{
<<<<<<< HEAD
	struct me_private_data *dev_private = dev->private;
=======
	struct me_private_data *devpriv = dev->private;
>>>>>>> v4.9.227
	unsigned int value;
	unsigned int file_length;
	unsigned int i;

	/* disable irq's on PLX */
<<<<<<< HEAD
	writel(0x00, dev_private->plx_regbase + PLX9052_INTCSR);
=======
	writel(0x00, devpriv->plx_regbase + PLX9052_INTCSR);
>>>>>>> v4.9.227

	/* First, make a dummy read to reset xilinx */
	value = readw(dev->mmio + XILINX_DOWNLOAD_RESET);

	/* Wait until reset is over */
	sleep(1);

	/* Write a dummy value to Xilinx */
	writeb(0x00, dev->mmio + 0x0);
	sleep(1);

	/*
	 * Format of the firmware
	 * Build longs from the byte-wise coded header
	 * Byte 1-3:   length of the array
	 * Byte 4-7:   version
	 * Byte 8-11:  date
	 * Byte 12-15: reserved
	 */
	if (size < 16)
		return -EINVAL;

	file_length = (((unsigned int)data[0] & 0xff) << 24) +
	    (((unsigned int)data[1] & 0xff) << 16) +
	    (((unsigned int)data[2] & 0xff) << 8) +
	    ((unsigned int)data[3] & 0xff);

	/*
	 * Loop for writing firmware byte by byte to xilinx
	 * Firmware data start at offset 16
	 */
	for (i = 0; i < file_length; i++)
		writeb((data[16 + i] & 0xff), dev->mmio + 0x0);

	/* Write 5 dummy values to xilinx */
	for (i = 0; i < 5; i++)
		writeb(0x00, dev->mmio + 0x0);

	/* Test if there was an error during download -> INTB was thrown */
<<<<<<< HEAD
	value = readl(dev_private->plx_regbase + PLX9052_INTCSR);
	if (value & PLX9052_INTCSR_LI2STAT) {
		/* Disable interrupt */
		writel(0x00, dev_private->plx_regbase + PLX9052_INTCSR);
=======
	value = readl(devpriv->plx_regbase + PLX9052_INTCSR);
	if (value & PLX9052_INTCSR_LI2STAT) {
		/* Disable interrupt */
		writel(0x00, devpriv->plx_regbase + PLX9052_INTCSR);
>>>>>>> v4.9.227
		dev_err(dev->class_dev, "Xilinx download failed\n");
		return -EIO;
	}

	/* Wait until the Xilinx is ready for real work */
	sleep(1);

	/* Enable PLX-Interrupts */
	writel(PLX9052_INTCSR_LI1ENAB |
	       PLX9052_INTCSR_LI1POL |
	       PLX9052_INTCSR_PCIENAB,
<<<<<<< HEAD
	       dev_private->plx_regbase + PLX9052_INTCSR);
=======
	       devpriv->plx_regbase + PLX9052_INTCSR);
>>>>>>> v4.9.227

	return 0;
}

static int me_reset(struct comedi_device *dev)
{
<<<<<<< HEAD
	struct me_private_data *dev_private = dev->private;

	/* Reset board */
	writew(0x00, dev->mmio + ME_CONTROL_1);
	writew(0x00, dev->mmio + ME_CONTROL_2);
	writew(0x00, dev->mmio + ME_RESET_INTERRUPT);
	writew(0x00, dev->mmio + ME_DAC_CONTROL);

	/* Save values in the board context */
	dev_private->dac_control = 0;
	dev_private->control_1 = 0;
	dev_private->control_2 = 0;
=======
	struct me_private_data *devpriv = dev->private;

	/* Reset board */
	writew(0x00, dev->mmio + ME_CTRL1_REG);
	writew(0x00, dev->mmio + ME_CTRL2_REG);
	writew(0x00, dev->mmio + ME_STATUS_REG);	/* clear interrupts */
	writew(0x00, dev->mmio + ME_DAC_CTRL_REG);

	/* Save values in the board context */
	devpriv->dac_ctrl = 0;
	devpriv->ctrl1 = 0;
	devpriv->ctrl2 = 0;
>>>>>>> v4.9.227

	return 0;
}

static int me_auto_attach(struct comedi_device *dev,
			  unsigned long context)
{
	struct pci_dev *pcidev = comedi_to_pci_dev(dev);
	const struct me_board *board = NULL;
<<<<<<< HEAD
	struct me_private_data *dev_private;
=======
	struct me_private_data *devpriv;
>>>>>>> v4.9.227
	struct comedi_subdevice *s;
	int ret;

	if (context < ARRAY_SIZE(me_boards))
		board = &me_boards[context];
	if (!board)
		return -ENODEV;
	dev->board_ptr = board;
	dev->board_name = board->name;

<<<<<<< HEAD
	dev_private = comedi_alloc_devpriv(dev, sizeof(*dev_private));
	if (!dev_private)
=======
	devpriv = comedi_alloc_devpriv(dev, sizeof(*devpriv));
	if (!devpriv)
>>>>>>> v4.9.227
		return -ENOMEM;

	ret = comedi_pci_enable(dev);
	if (ret)
		return ret;

<<<<<<< HEAD
	dev_private->plx_regbase = pci_ioremap_bar(pcidev, 0);
	if (!dev_private->plx_regbase)
=======
	devpriv->plx_regbase = pci_ioremap_bar(pcidev, 0);
	if (!devpriv->plx_regbase)
>>>>>>> v4.9.227
		return -ENOMEM;

	dev->mmio = pci_ioremap_bar(pcidev, 2);
	if (!dev->mmio)
		return -ENOMEM;

	/* Download firmware and reset card */
	if (board->needs_firmware) {
		ret = comedi_load_firmware(dev, &comedi_to_pci_dev(dev)->dev,
					   ME2600_FIRMWARE,
					   me2600_xilinx_download, 0);
		if (ret < 0)
			return ret;
	}
	me_reset(dev);

	ret = comedi_alloc_subdevices(dev, 3);
	if (ret)
		return ret;

	s = &dev->subdevices[0];
	s->type		= COMEDI_SUBD_AI;
<<<<<<< HEAD
	s->subdev_flags	= SDF_READABLE | SDF_COMMON;
=======
	s->subdev_flags	= SDF_READABLE | SDF_COMMON | SDF_DIFF;
>>>>>>> v4.9.227
	s->n_chan	= 16;
	s->maxdata	= 0x0fff;
	s->len_chanlist	= 16;
	s->range_table	= &me_ai_range;
	s->insn_read	= me_ai_insn_read;

	s = &dev->subdevices[1];
	if (board->has_ao) {
		s->type		= COMEDI_SUBD_AO;
<<<<<<< HEAD
		s->subdev_flags	= SDF_WRITEABLE | SDF_COMMON;
=======
		s->subdev_flags	= SDF_WRITABLE | SDF_COMMON;
>>>>>>> v4.9.227
		s->n_chan	= 4;
		s->maxdata	= 0x0fff;
		s->len_chanlist	= 4;
		s->range_table	= &me_ao_range;
		s->insn_write	= me_ao_insn_write;
<<<<<<< HEAD
		s->insn_read	= comedi_readback_insn_read;
=======
>>>>>>> v4.9.227

		ret = comedi_alloc_subdev_readback(s);
		if (ret)
			return ret;
	} else {
		s->type = COMEDI_SUBD_UNUSED;
	}

	s = &dev->subdevices[2];
	s->type		= COMEDI_SUBD_DIO;
<<<<<<< HEAD
	s->subdev_flags	= SDF_READABLE | SDF_WRITEABLE;
=======
	s->subdev_flags	= SDF_READABLE | SDF_WRITABLE;
>>>>>>> v4.9.227
	s->n_chan	= 32;
	s->maxdata	= 1;
	s->len_chanlist	= 32;
	s->range_table	= &range_digital;
	s->insn_bits	= me_dio_insn_bits;
	s->insn_config	= me_dio_insn_config;

	return 0;
}

static void me_detach(struct comedi_device *dev)
{
<<<<<<< HEAD
	struct me_private_data *dev_private = dev->private;

	if (dev_private) {
		if (dev->mmio)
			me_reset(dev);
		if (dev_private->plx_regbase)
			iounmap(dev_private->plx_regbase);
=======
	struct me_private_data *devpriv = dev->private;

	if (devpriv) {
		if (dev->mmio)
			me_reset(dev);
		if (devpriv->plx_regbase)
			iounmap(devpriv->plx_regbase);
>>>>>>> v4.9.227
	}
	comedi_pci_detach(dev);
}

static struct comedi_driver me_daq_driver = {
	.driver_name	= "me_daq",
	.module		= THIS_MODULE,
	.auto_attach	= me_auto_attach,
	.detach		= me_detach,
};

static int me_daq_pci_probe(struct pci_dev *dev,
			    const struct pci_device_id *id)
{
	return comedi_pci_auto_config(dev, &me_daq_driver, id->driver_data);
}

static const struct pci_device_id me_daq_pci_table[] = {
	{ PCI_VDEVICE(MEILHAUS, 0x2600), BOARD_ME2600 },
	{ PCI_VDEVICE(MEILHAUS, 0x2000), BOARD_ME2000 },
	{ 0 }
};
MODULE_DEVICE_TABLE(pci, me_daq_pci_table);

static struct pci_driver me_daq_pci_driver = {
	.name		= "me_daq",
	.id_table	= me_daq_pci_table,
	.probe		= me_daq_pci_probe,
	.remove		= comedi_pci_auto_unconfig,
};
module_comedi_pci_driver(me_daq_driver, me_daq_pci_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi low-level driver");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE(ME2600_FIRMWARE);
