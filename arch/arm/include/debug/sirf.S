/*
 * arch/arm/mach-prima2/include/mach/debug-macro.S
 *
 * Copyright (c) 2011 Cambridge Silicon Radio Limited, a CSR plc group company.
 *
 * Licensed under GPLv2 or later.
 */

<<<<<<< HEAD
#if defined(CONFIG_DEBUG_SIRFPRIMA2_UART1)
#define SIRFSOC_UART1_PA_BASE          0xb0060000
#elif defined(CONFIG_DEBUG_SIRFMARCO_UART1)
#define SIRFSOC_UART1_PA_BASE          0xcc060000
#else
#define SIRFSOC_UART1_PA_BASE          0
#endif

#define SIRFSOC_UART1_VA_BASE		0xFEC60000

#define SIRFSOC_UART_TXFIFO_STATUS	0x0114
#define SIRFSOC_UART_TXFIFO_DATA	0x0118

#define SIRFSOC_UART1_TXFIFO_FULL                       (1 << 5)
#define SIRFSOC_UART1_TXFIFO_EMPTY			(1 << 6)

	.macro	addruart, rp, rv, tmp
	ldr	\rp, =SIRFSOC_UART1_PA_BASE		@ physical
	ldr	\rv, =SIRFSOC_UART1_VA_BASE		@ virtual
	.endm

	.macro	senduart,rd,rx
	str	\rd, [\rx, #SIRFSOC_UART_TXFIFO_DATA]
=======
#define SIRF_LLUART_TXFIFO_STATUS	0x0114
#define SIRF_LLUART_TXFIFO_DATA	0x0118

#define SIRF_LLUART_TXFIFO_FULL                       (1 << 5)

#ifdef CONFIG_DEBUG_SIRFATLAS7_UART0
#define SIRF_LLUART_TXFIFO_EMPTY			(1 << 8)
#else
#define SIRF_LLUART_TXFIFO_EMPTY			(1 << 6)
#endif


	.macro	addruart, rp, rv, tmp
	ldr	\rp, =CONFIG_DEBUG_UART_PHYS		@ physical
	ldr	\rv, =CONFIG_DEBUG_UART_VIRT		@ virtual
	.endm

	.macro	senduart,rd,rx
	str	\rd, [\rx, #SIRF_LLUART_TXFIFO_DATA]
>>>>>>> v4.9.227
	.endm

	.macro	busyuart,rd,rx
	.endm

	.macro	waituart,rd,rx
<<<<<<< HEAD
1001:	ldr	\rd, [\rx, #SIRFSOC_UART_TXFIFO_STATUS]
	tst	\rd, #SIRFSOC_UART1_TXFIFO_EMPTY
=======
1001:	ldr	\rd, [\rx, #SIRF_LLUART_TXFIFO_STATUS]
	tst	\rd, #SIRF_LLUART_TXFIFO_EMPTY
>>>>>>> v4.9.227
	beq	1001b
	.endm

