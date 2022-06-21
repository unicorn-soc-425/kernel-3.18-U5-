/*
<<<<<<< HEAD
 * Copyright (C) 2011 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcrosoft.com>
 *
 * Under GPLv2
 */

struct at91_init_soc {
	int builtin;
	u32 extern_irq;
	unsigned int *default_irq_priority;
	void (*map_io)(void);
	void (*ioremap_registers)(void);
	void (*register_clocks)(void);
	void (*register_devices)(void);
	void (*init)(void);
	void (*init_time)(void);
};

extern struct at91_init_soc at91_boot_soc;
extern struct at91_init_soc at91rm9200_soc;
extern struct at91_init_soc at91sam9260_soc;
extern struct at91_init_soc at91sam9261_soc;
extern struct at91_init_soc at91sam9263_soc;
extern struct at91_init_soc at91sam9g45_soc;
extern struct at91_init_soc at91sam9rl_soc;
extern struct at91_init_soc at91sam9x5_soc;
extern struct at91_init_soc at91sam9n12_soc;
extern struct at91_init_soc sama5d3_soc;
extern struct at91_init_soc sama5d4_soc;

#define AT91_SOC_START(_name)				\
struct at91_init_soc __initdata _name##_soc		\
 __used							\
						= {	\
	.builtin	= 1,				\

#define AT91_SOC_END					\
};

static inline int at91_soc_is_enabled(void)
{
	return at91_boot_soc.builtin;
}

#if !defined(CONFIG_SOC_AT91RM9200)
#define at91rm9200_soc	at91_boot_soc
#endif

#if !defined(CONFIG_SOC_AT91SAM9260)
#define at91sam9260_soc	at91_boot_soc
#endif

#if !defined(CONFIG_SOC_AT91SAM9261)
#define at91sam9261_soc	at91_boot_soc
#endif

#if !defined(CONFIG_SOC_AT91SAM9263)
#define at91sam9263_soc	at91_boot_soc
#endif

#if !defined(CONFIG_SOC_AT91SAM9G45)
#define at91sam9g45_soc	at91_boot_soc
#endif

#if !defined(CONFIG_SOC_AT91SAM9RL)
#define at91sam9rl_soc	at91_boot_soc
#endif

#if !defined(CONFIG_SOC_AT91SAM9X5)
#define at91sam9x5_soc	at91_boot_soc
#endif

#if !defined(CONFIG_SOC_AT91SAM9N12)
#define at91sam9n12_soc	at91_boot_soc
#endif

#if !defined(CONFIG_SOC_SAMA5D3)
#define sama5d3_soc	at91_boot_soc
#endif

#if !defined(CONFIG_SOC_SAMA5D4)
#define sama5d4_soc	at91_boot_soc
#endif
=======
 * Copyright (C) 2015 Atmel
 *
 * Boris Brezillon <boris.brezillon@free-electrons.com
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 *
 */

#ifndef __AT91_SOC_H
#define __AT91_SOC_H

#include <linux/sys_soc.h>

struct at91_soc {
	u32 cidr_match;
	u32 exid_match;
	const char *name;
	const char *family;
};

#define AT91_SOC(__cidr, __exid, __name, __family)		\
	{							\
		.cidr_match = (__cidr),				\
		.exid_match = (__exid),				\
		.name = (__name),				\
		.family = (__family),				\
	}

struct soc_device * __init
at91_soc_init(const struct at91_soc *socs);

#define AT91RM9200_CIDR_MATCH		0x09290780

#define AT91SAM9260_CIDR_MATCH		0x019803a0
#define AT91SAM9261_CIDR_MATCH		0x019703a0
#define AT91SAM9263_CIDR_MATCH		0x019607a0
#define AT91SAM9G20_CIDR_MATCH		0x019905a0
#define AT91SAM9RL64_CIDR_MATCH		0x019b03a0
#define AT91SAM9G45_CIDR_MATCH		0x019b05a0
#define AT91SAM9X5_CIDR_MATCH		0x019a05a0
#define AT91SAM9N12_CIDR_MATCH		0x019a07a0

#define AT91SAM9M11_EXID_MATCH		0x00000001
#define AT91SAM9M10_EXID_MATCH		0x00000002
#define AT91SAM9G46_EXID_MATCH		0x00000003
#define AT91SAM9G45_EXID_MATCH		0x00000004

#define AT91SAM9G15_EXID_MATCH		0x00000000
#define AT91SAM9G35_EXID_MATCH		0x00000001
#define AT91SAM9X35_EXID_MATCH		0x00000002
#define AT91SAM9G25_EXID_MATCH		0x00000003
#define AT91SAM9X25_EXID_MATCH		0x00000004

#define AT91SAM9CN12_EXID_MATCH		0x00000005
#define AT91SAM9N12_EXID_MATCH		0x00000006
#define AT91SAM9CN11_EXID_MATCH		0x00000009

#define AT91SAM9XE128_CIDR_MATCH	0x329973a0
#define AT91SAM9XE256_CIDR_MATCH	0x329a93a0
#define AT91SAM9XE512_CIDR_MATCH	0x329aa3a0

#define SAMA5D2_CIDR_MATCH		0x0a5c08c0
#define SAMA5D21CU_EXID_MATCH		0x0000005a
#define SAMA5D22CU_EXID_MATCH		0x00000059
#define SAMA5D22CN_EXID_MATCH		0x00000069
#define SAMA5D23CU_EXID_MATCH		0x00000058
#define SAMA5D24CX_EXID_MATCH		0x00000004
#define SAMA5D24CU_EXID_MATCH		0x00000014
#define SAMA5D26CU_EXID_MATCH		0x00000012
#define SAMA5D27CU_EXID_MATCH		0x00000011
#define SAMA5D27CN_EXID_MATCH		0x00000021
#define SAMA5D28CU_EXID_MATCH		0x00000010
#define SAMA5D28CN_EXID_MATCH		0x00000020

#define SAMA5D3_CIDR_MATCH		0x0a5c07c0
#define SAMA5D31_EXID_MATCH		0x00444300
#define SAMA5D33_EXID_MATCH		0x00414300
#define SAMA5D34_EXID_MATCH		0x00414301
#define SAMA5D35_EXID_MATCH		0x00584300
#define SAMA5D36_EXID_MATCH		0x00004301

#define SAMA5D4_CIDR_MATCH		0x0a5c07c0
#define SAMA5D41_EXID_MATCH		0x00000001
#define SAMA5D42_EXID_MATCH		0x00000002
#define SAMA5D43_EXID_MATCH		0x00000003
#define SAMA5D44_EXID_MATCH		0x00000004

#endif /* __AT91_SOC_H */
>>>>>>> v4.9.227
