/*
 * at24.h - platform_data for the at24 (generic eeprom) driver
 * (C) Copyright 2008 by Pengutronix
 * (C) Copyright 2012 by Wolfram Sang
 * same license as the driver
 */

#ifndef _LINUX_AT24_H
#define _LINUX_AT24_H

#include <linux/types.h>
<<<<<<< HEAD
#include <linux/memory.h>
=======
#include <linux/nvmem-consumer.h>
#include <linux/bitops.h>
>>>>>>> v4.9.227

/**
 * struct at24_platform_data - data to set up at24 (generic eeprom) driver
 * @byte_len: size of eeprom in byte
 * @page_size: number of byte which can be written in one go
 * @flags: tunable options, check AT24_FLAG_* defines
 * @setup: an optional callback invoked after eeprom is probed; enables kernel
<<<<<<< HEAD
	code to access eeprom via memory_accessor, see example
=======
	code to access eeprom via nvmem, see example
>>>>>>> v4.9.227
 * @context: optional parameter passed to setup()
 *
 * If you set up a custom eeprom type, please double-check the parameters.
 * Especially page_size needs extra care, as you risk data loss if your value
 * is bigger than what the chip actually supports!
 *
 * An example in pseudo code for a setup() callback:
 *
<<<<<<< HEAD
 * void get_mac_addr(struct memory_accessor *mem_acc, void *context)
=======
 * void get_mac_addr(struct nvmem_device *nvmem, void *context)
>>>>>>> v4.9.227
 * {
 *	u8 *mac_addr = ethernet_pdata->mac_addr;
 *	off_t offset = context;
 *
 *	// Read MAC addr from EEPROM
<<<<<<< HEAD
 *	if (mem_acc->read(mem_acc, mac_addr, offset, ETH_ALEN) == ETH_ALEN)
=======
 *	if (nvmem_device_read(nvmem, offset, ETH_ALEN, mac_addr) == ETH_ALEN)
>>>>>>> v4.9.227
 *		pr_info("Read MAC addr from EEPROM: %pM\n", mac_addr);
 * }
 *
 * This function pointer and context can now be set up in at24_platform_data.
 */

struct at24_platform_data {
	u32		byte_len;		/* size (sum of all addr) */
	u16		page_size;		/* for writes */
	u8		flags;
<<<<<<< HEAD
#define AT24_FLAG_ADDR16	0x80	/* address pointer is 16 bit */
#define AT24_FLAG_READONLY	0x40	/* sysfs-entry will be read-only */
#define AT24_FLAG_IRUGO		0x20	/* sysfs-entry will be world-readable */
#define AT24_FLAG_TAKE8ADDR	0x10	/* take always 8 addresses (24c00) */

	void		(*setup)(struct memory_accessor *, void *context);
=======
#define AT24_FLAG_ADDR16	BIT(7)	/* address pointer is 16 bit */
#define AT24_FLAG_READONLY	BIT(6)	/* sysfs-entry will be read-only */
#define AT24_FLAG_IRUGO		BIT(5)	/* sysfs-entry will be world-readable */
#define AT24_FLAG_TAKE8ADDR	BIT(4)	/* take always 8 addresses (24c00) */
#define AT24_FLAG_SERIAL	BIT(3)	/* factory-programmed serial number */
#define AT24_FLAG_MAC		BIT(2)	/* factory-programmed mac address */

	void		(*setup)(struct nvmem_device *nvmem, void *context);
>>>>>>> v4.9.227
	void		*context;
};

#endif /* _LINUX_AT24_H */
