/*
 *  Probe for F81216A LPC to 4 UART
 *
<<<<<<< HEAD
 *  Based on drivers/tty/serial/8250_pnp.c, by Russell King, et al
 *
 *  Copyright (C) 2014 Ricardo Ribalda, Qtechnology A/S
=======
 *  Copyright (C) 2014-2016 Ricardo Ribalda, Qtechnology A/S
>>>>>>> v4.9.227
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 */
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/pnp.h>
#include <linux/kernel.h>
#include <linux/serial_core.h>
<<<<<<< HEAD
#include  "8250.h"

#define ADDR_PORT 0x4E
#define DATA_PORT 0x4F
#define ENTRY_KEY 0x77
#define EXIT_KEY 0xAA
#define CHIP_ID1  0x20
#define CHIP_ID1_VAL 0x02
#define CHIP_ID2  0x21
#define CHIP_ID2_VAL 0x16
=======
#include <linux/irq.h>
#include  "8250.h"

#define ADDR_PORT 0
#define DATA_PORT 1
#define EXIT_KEY 0xAA
#define CHIP_ID1  0x20
#define CHIP_ID2  0x21
#define CHIP_ID_0 0x1602
#define CHIP_ID_1 0x0501
>>>>>>> v4.9.227
#define VENDOR_ID1 0x23
#define VENDOR_ID1_VAL 0x19
#define VENDOR_ID2 0x24
#define VENDOR_ID2_VAL 0x34
<<<<<<< HEAD
#define LDN 0x7

=======
#define IO_ADDR1 0x61
#define IO_ADDR2 0x60
#define LDN 0x7

#define FINTEK_IRQ_MODE	0x70
#define IRQ_SHARE	BIT(4)
#define IRQ_MODE_MASK	(BIT(6) | BIT(5))
#define IRQ_LEVEL_LOW	0
#define IRQ_EDGE_HIGH	BIT(5)

>>>>>>> v4.9.227
#define RS485  0xF0
#define RTS_INVERT BIT(5)
#define RS485_URA BIT(4)
#define RXW4C_IRA BIT(3)
#define TXW4C_IRA BIT(2)

<<<<<<< HEAD
#define DRIVER_NAME "8250_fintek"

static int fintek_8250_enter_key(void){

	if (!request_muxed_region(ADDR_PORT, 2, DRIVER_NAME))
		return -EBUSY;

	outb(ENTRY_KEY, ADDR_PORT);
	outb(ENTRY_KEY, ADDR_PORT);
	return 0;
}

static void fintek_8250_exit_key(void){

	outb(EXIT_KEY, ADDR_PORT);
	release_region(ADDR_PORT, 2);
}

static int fintek_8250_get_index(resource_size_t base_addr)
{
	resource_size_t base[] = {0x3f8, 0x2f8, 0x3e8, 0x2e8};
	int i;

	for (i = 0; i < ARRAY_SIZE(base); i++)
		if (base_addr == base[i])
			return i;

	return -ENODEV;
}

static int fintek_8250_check_id(void)
{

	outb(CHIP_ID1, ADDR_PORT);
	if (inb(DATA_PORT) != CHIP_ID1_VAL)
		return -ENODEV;

	outb(CHIP_ID2, ADDR_PORT);
	if (inb(DATA_PORT) != CHIP_ID2_VAL)
		return -ENODEV;

	outb(VENDOR_ID1, ADDR_PORT);
	if (inb(DATA_PORT) != VENDOR_ID1_VAL)
		return -ENODEV;

	outb(VENDOR_ID2, ADDR_PORT);
	if (inb(DATA_PORT) != VENDOR_ID2_VAL)
=======
struct fintek_8250 {
	u16 base_port;
	u8 index;
	u8 key;
};

static int fintek_8250_enter_key(u16 base_port, u8 key)
{
	if (!request_muxed_region(base_port, 2, "8250_fintek"))
		return -EBUSY;

	/* Force to deactive all SuperIO in this base_port */
	outb(EXIT_KEY, base_port + ADDR_PORT);

	outb(key, base_port + ADDR_PORT);
	outb(key, base_port + ADDR_PORT);
	return 0;
}

static void fintek_8250_exit_key(u16 base_port)
{

	outb(EXIT_KEY, base_port + ADDR_PORT);
	release_region(base_port + ADDR_PORT, 2);
}

static int fintek_8250_check_id(u16 base_port)
{
	u16 chip;

	outb(VENDOR_ID1, base_port + ADDR_PORT);
	if (inb(base_port + DATA_PORT) != VENDOR_ID1_VAL)
		return -ENODEV;

	outb(VENDOR_ID2, base_port + ADDR_PORT);
	if (inb(base_port + DATA_PORT) != VENDOR_ID2_VAL)
		return -ENODEV;

	outb(CHIP_ID1, base_port + ADDR_PORT);
	chip = inb(base_port + DATA_PORT);
	outb(CHIP_ID2, base_port + ADDR_PORT);
	chip |= inb(base_port + DATA_PORT) << 8;

	if (chip != CHIP_ID_0 && chip != CHIP_ID_1)
>>>>>>> v4.9.227
		return -ENODEV;

	return 0;
}

<<<<<<< HEAD
static int fintek_8250_rs4850_config(struct uart_8250_port *uart,
			      struct serial_rs485 *rs485)
{
	uint8_t config = 0;
	int index = fintek_8250_get_index(uart->port.iobase);

	if (index < 0)
=======
static int fintek_8250_rs485_config(struct uart_port *port,
			      struct serial_rs485 *rs485)
{
	uint8_t config = 0;
	struct fintek_8250 *pdata = port->private_data;

	if (!pdata)
>>>>>>> v4.9.227
		return -EINVAL;

	if (rs485->flags & SER_RS485_ENABLED)
		memset(rs485->padding, 0, sizeof(rs485->padding));
	else
		memset(rs485, 0, sizeof(*rs485));

	rs485->flags &= SER_RS485_ENABLED | SER_RS485_RTS_ON_SEND |
			SER_RS485_RTS_AFTER_SEND;

	if (rs485->delay_rts_before_send) {
		rs485->delay_rts_before_send = 1;
		config |= TXW4C_IRA;
	}

	if (rs485->delay_rts_after_send) {
		rs485->delay_rts_after_send = 1;
		config |= RXW4C_IRA;
	}

	if ((!!(rs485->flags & SER_RS485_RTS_ON_SEND)) ==
			(!!(rs485->flags & SER_RS485_RTS_AFTER_SEND)))
		rs485->flags &= ~SER_RS485_ENABLED;
	else
		config |= RS485_URA;

	if (rs485->flags & SER_RS485_RTS_ON_SEND)
		config |= RTS_INVERT;

<<<<<<< HEAD
	if (fintek_8250_enter_key())
		return -EBUSY;

	outb(LDN, ADDR_PORT);
	outb(index, DATA_PORT);
	outb(RS485, ADDR_PORT);
	outb(config, DATA_PORT);
	fintek_8250_exit_key();
=======
	if (fintek_8250_enter_key(pdata->base_port, pdata->key))
		return -EBUSY;

	outb(LDN, pdata->base_port + ADDR_PORT);
	outb(pdata->index, pdata->base_port + DATA_PORT);
	outb(RS485, pdata->base_port + ADDR_PORT);
	outb(config, pdata->base_port + DATA_PORT);
	fintek_8250_exit_key(pdata->base_port);

	port->rs485 = *rs485;
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int
fintek_8250_probe(struct pnp_dev *dev, const struct pnp_device_id *dev_id)
{
	int line;
	struct uart_8250_port uart;
	int ret;

	if (!pnp_port_valid(dev, 0))
		return -ENODEV;

	if (fintek_8250_get_index(pnp_port_start(dev, 0)) < 0)
		return -ENODEV;

	/* Enable configuration registers*/
	if (fintek_8250_enter_key())
		return -EBUSY;

	/*Check ID*/
	ret = fintek_8250_check_id();
	fintek_8250_exit_key();
	if (ret)
		return ret;

	memset(&uart, 0, sizeof(uart));
	if (!pnp_irq_valid(dev, 0))
		return -ENODEV;
	uart.port.irq = pnp_irq(dev, 0);
	uart.port.iobase = pnp_port_start(dev, 0);
	uart.port.iotype = UPIO_PORT;
	uart.rs485_config = fintek_8250_rs4850_config;

	uart.port.flags |= UPF_SKIP_TEST | UPF_BOOT_AUTOCONF;
	if (pnp_irq_flags(dev, 0) & IORESOURCE_IRQ_SHAREABLE)
		uart.port.flags |= UPF_SHARE_IRQ;
	uart.port.uartclk = 1843200;
	uart.port.dev = &dev->dev;

	line = serial8250_register_8250_port(&uart);
	if (line < 0)
		return -ENODEV;

	pnp_set_drvdata(dev, (void *)((long)line + 1));
	return 0;
}

static void fintek_8250_remove(struct pnp_dev *dev)
{
	long line = (long)pnp_get_drvdata(dev);

	if (line)
		serial8250_unregister_port(line - 1);
}

#ifdef CONFIG_PM
static int fintek_8250_suspend(struct pnp_dev *dev, pm_message_t state)
{
	long line = (long)pnp_get_drvdata(dev);

	if (!line)
		return -ENODEV;
	serial8250_suspend_port(line - 1);
	return 0;
}

static int fintek_8250_resume(struct pnp_dev *dev)
{
	long line = (long)pnp_get_drvdata(dev);

	if (!line)
		return -ENODEV;
	serial8250_resume_port(line - 1);
	return 0;
}
#else
#define fintek_8250_suspend NULL
#define fintek_8250_resume NULL
#endif /* CONFIG_PM */

static const struct pnp_device_id fintek_dev_table[] = {
	/* Qtechnology Panel PC / IO1000 */
	{ "PNP0501"},
	{}
};

MODULE_DEVICE_TABLE(pnp, fintek_dev_table);

static struct pnp_driver fintek_8250_driver = {
	.name		= DRIVER_NAME,
	.probe		= fintek_8250_probe,
	.remove		= fintek_8250_remove,
	.suspend	= fintek_8250_suspend,
	.resume		= fintek_8250_resume,
	.id_table	= fintek_dev_table,
};

static int fintek_8250_init(void)
{
	return pnp_register_driver(&fintek_8250_driver);
}
module_init(fintek_8250_init);

static void fintek_8250_exit(void)
{
	pnp_unregister_driver(&fintek_8250_driver);
}
module_exit(fintek_8250_exit);

MODULE_DESCRIPTION("Fintek F812164 module");
MODULE_AUTHOR("Ricardo Ribalda <ricardo.ribalda@gmail.com>");
MODULE_LICENSE("GPL");
=======
static int find_base_port(struct fintek_8250 *pdata, u16 io_address)
{
	static const u16 addr[] = {0x4e, 0x2e};
	static const u8 keys[] = {0x77, 0xa0, 0x87, 0x67};
	int i, j, k;

	for (i = 0; i < ARRAY_SIZE(addr); i++) {
		for (j = 0; j < ARRAY_SIZE(keys); j++) {

			if (fintek_8250_enter_key(addr[i], keys[j]))
				continue;
			if (fintek_8250_check_id(addr[i])) {
				fintek_8250_exit_key(addr[i]);
				continue;
			}

			for (k = 0; k < 4; k++) {
				u16 aux;

				outb(LDN, addr[i] + ADDR_PORT);
				outb(k, addr[i] + DATA_PORT);

				outb(IO_ADDR1, addr[i] + ADDR_PORT);
				aux = inb(addr[i] + DATA_PORT);
				outb(IO_ADDR2, addr[i] + ADDR_PORT);
				aux |= inb(addr[i] + DATA_PORT) << 8;
				if (aux != io_address)
					continue;

				fintek_8250_exit_key(addr[i]);
				pdata->key = keys[j];
				pdata->base_port = addr[i];
				pdata->index = k;

				return 0;
			}

			fintek_8250_exit_key(addr[i]);
		}
	}

	return -ENODEV;
}

static int fintek_8250_set_irq_mode(struct fintek_8250 *pdata, bool level_mode)
{
	int status;
	u8 tmp;

	status = fintek_8250_enter_key(pdata->base_port, pdata->key);
	if (status)
		return status;

	outb(LDN, pdata->base_port + ADDR_PORT);
	outb(pdata->index, pdata->base_port + DATA_PORT);

	outb(FINTEK_IRQ_MODE, pdata->base_port + ADDR_PORT);
	tmp = inb(pdata->base_port + DATA_PORT);

	tmp &= ~IRQ_MODE_MASK;
	tmp |= IRQ_SHARE;
	if (!level_mode)
		tmp |= IRQ_EDGE_HIGH;

	outb(tmp, pdata->base_port + DATA_PORT);
	fintek_8250_exit_key(pdata->base_port);
	return 0;
}

int fintek_8250_probe(struct uart_8250_port *uart)
{
	struct fintek_8250 *pdata;
	struct fintek_8250 probe_data;
	struct irq_data *irq_data = irq_get_irq_data(uart->port.irq);
	bool level_mode = irqd_is_level_type(irq_data);

	if (find_base_port(&probe_data, uart->port.iobase))
		return -ENODEV;

	pdata = devm_kzalloc(uart->port.dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;

	memcpy(pdata, &probe_data, sizeof(probe_data));
	uart->port.rs485_config = fintek_8250_rs485_config;
	uart->port.private_data = pdata;

	return fintek_8250_set_irq_mode(pdata, level_mode);
}
>>>>>>> v4.9.227
