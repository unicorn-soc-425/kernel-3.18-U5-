/*
	Mantis PCI bridge driver

	Copyright (C) Manu Abraham (abraham.manu@gmail.com)

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <asm/io.h>

#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
<<<<<<< HEAD
=======
#include <linux/pci.h>
>>>>>>> v4.9.227

#include "dmxdev.h"
#include "dvbdev.h"
#include "dvb_demux.h"
#include "dvb_frontend.h"
#include "dvb_net.h"

#include "mantis_common.h"
#include "mantis_reg.h"
#include "mantis_uart.h"
<<<<<<< HEAD
=======
#include "mantis_input.h"
>>>>>>> v4.9.227

struct mantis_uart_params {
	enum mantis_baud	baud_rate;
	enum mantis_parity	parity;
};

static struct {
	char string[7];
} rates[5] = {
	{ "9600" },
	{ "19200" },
	{ "38400" },
	{ "57600" },
	{ "115200" }
};

static struct {
	char string[5];
} parity[3] = {
	{ "NONE" },
	{ "ODD" },
	{ "EVEN" }
};

<<<<<<< HEAD
#define UART_MAX_BUF			16

static int mantis_uart_read(struct mantis_pci *mantis, u8 *data)
{
	struct mantis_hwconfig *config = mantis->hwconfig;
	u32 stat = 0, i;

	/* get data */
	for (i = 0; i < (config->bytes + 1); i++) {

		stat = mmread(MANTIS_UART_STAT);

		if (stat & MANTIS_UART_RXFIFO_FULL) {
			dprintk(MANTIS_ERROR, 1, "RX Fifo FULL");
		}
		data[i] = mmread(MANTIS_UART_RXD) & 0x3f;

		dprintk(MANTIS_DEBUG, 1, "Reading ... <%02x>", data[i] & 0x3f);

		if (data[i] & (1 << 7)) {
			dprintk(MANTIS_ERROR, 1, "UART framing error");
			return -EINVAL;
		}
		if (data[i] & (1 << 6)) {
			dprintk(MANTIS_ERROR, 1, "UART parity error");
			return -EINVAL;
		}
	}

	return 0;
=======
static void mantis_uart_read(struct mantis_pci *mantis)
{
	struct mantis_hwconfig *config = mantis->hwconfig;
	int i, scancode = 0, err = 0;

	/* get data */
	dprintk(MANTIS_DEBUG, 1, "UART Reading ...");
	for (i = 0; i < (config->bytes + 1); i++) {
		int data = mmread(MANTIS_UART_RXD);

		dprintk(MANTIS_DEBUG, 0, " <%02x>", data);

		scancode = (scancode << 8) | (data & 0x3f);
		err |= data;

		if (data & (1 << 7))
			dprintk(MANTIS_ERROR, 1, "UART framing error");

		if (data & (1 << 6))
			dprintk(MANTIS_ERROR, 1, "UART parity error");
	}
	dprintk(MANTIS_DEBUG, 0, "\n");

	if ((err & 0xC0) == 0)
		mantis_input_process(mantis, scancode);
>>>>>>> v4.9.227
}

static void mantis_uart_work(struct work_struct *work)
{
	struct mantis_pci *mantis = container_of(work, struct mantis_pci, uart_work);
<<<<<<< HEAD
	struct mantis_hwconfig *config = mantis->hwconfig;
	u8 buf[16];
	int i;

	mantis_uart_read(mantis, buf);

	for (i = 0; i < (config->bytes + 1); i++)
		dprintk(MANTIS_INFO, 1, "UART BUF:%d <%02x> ", i, buf[i]);

	dprintk(MANTIS_DEBUG, 0, "\n");
=======
	u32 stat;

	stat = mmread(MANTIS_UART_STAT);

	if (stat & MANTIS_UART_RXFIFO_FULL)
		dprintk(MANTIS_ERROR, 1, "RX Fifo FULL");

	/*
	 * MANTIS_UART_RXFIFO_DATA is only set if at least
	 * config->bytes + 1 bytes are in the FIFO.
	 */
	while (stat & MANTIS_UART_RXFIFO_DATA) {
		mantis_uart_read(mantis);
		stat = mmread(MANTIS_UART_STAT);
	}

	/* re-enable UART (RX) interrupt */
	mantis_unmask_ints(mantis, MANTIS_INT_IRQ1);
>>>>>>> v4.9.227
}

static int mantis_uart_setup(struct mantis_pci *mantis,
			     struct mantis_uart_params *params)
{
	u32 reg;

	mmwrite((mmread(MANTIS_UART_CTL) | (params->parity & 0x3)), MANTIS_UART_CTL);

	reg = mmread(MANTIS_UART_BAUD);

	switch (params->baud_rate) {
	case MANTIS_BAUD_9600:
		reg |= 0xd8;
		break;
	case MANTIS_BAUD_19200:
		reg |= 0x6c;
		break;
	case MANTIS_BAUD_38400:
		reg |= 0x36;
		break;
	case MANTIS_BAUD_57600:
		reg |= 0x23;
		break;
	case MANTIS_BAUD_115200:
		reg |= 0x11;
		break;
	default:
		return -EINVAL;
	}

	mmwrite(reg, MANTIS_UART_BAUD);

	return 0;
}

int mantis_uart_init(struct mantis_pci *mantis)
{
	struct mantis_hwconfig *config = mantis->hwconfig;
	struct mantis_uart_params params;

	/* default parity: */
	params.baud_rate = config->baud_rate;
	params.parity = config->parity;
	dprintk(MANTIS_INFO, 1, "Initializing UART @ %sbps parity:%s",
		rates[params.baud_rate].string,
		parity[params.parity].string);

<<<<<<< HEAD
	init_waitqueue_head(&mantis->uart_wq);
	spin_lock_init(&mantis->uart_lock);

=======
>>>>>>> v4.9.227
	INIT_WORK(&mantis->uart_work, mantis_uart_work);

	/* disable interrupt */
	mmwrite(mmread(MANTIS_UART_CTL) & 0xffef, MANTIS_UART_CTL);

	mantis_uart_setup(mantis, &params);

	/* default 1 byte */
	mmwrite((mmread(MANTIS_UART_BAUD) | (config->bytes << 8)), MANTIS_UART_BAUD);

	/* flush buffer */
	mmwrite((mmread(MANTIS_UART_CTL) | MANTIS_UART_RXFLUSH), MANTIS_UART_CTL);

	/* enable interrupt */
<<<<<<< HEAD
	mmwrite(mmread(MANTIS_INT_MASK) | 0x800, MANTIS_INT_MASK);
	mmwrite(mmread(MANTIS_UART_CTL) | MANTIS_UART_RXINT, MANTIS_UART_CTL);
=======
	mmwrite(mmread(MANTIS_UART_CTL) | MANTIS_UART_RXINT, MANTIS_UART_CTL);
	mantis_unmask_ints(mantis, MANTIS_INT_IRQ1);
>>>>>>> v4.9.227

	schedule_work(&mantis->uart_work);
	dprintk(MANTIS_DEBUG, 1, "UART successfully initialized");

	return 0;
}
EXPORT_SYMBOL_GPL(mantis_uart_init);

void mantis_uart_exit(struct mantis_pci *mantis)
{
	/* disable interrupt */
<<<<<<< HEAD
=======
	mantis_mask_ints(mantis, MANTIS_INT_IRQ1);
>>>>>>> v4.9.227
	mmwrite(mmread(MANTIS_UART_CTL) & 0xffef, MANTIS_UART_CTL);
	flush_work(&mantis->uart_work);
}
EXPORT_SYMBOL_GPL(mantis_uart_exit);
