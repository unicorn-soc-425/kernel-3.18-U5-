#ifndef ASM_EDAC_H
#define ASM_EDAC_H

<<<<<<< HEAD
/* ECC atomic, DMA, SMP and interrupt safe scrub function */

static inline void atomic_scrub(void *va, u32 size)
=======
#include <asm/compiler.h>

/* ECC atomic, DMA, SMP and interrupt safe scrub function */

static inline void edac_atomic_scrub(void *va, u32 size)
>>>>>>> v4.9.227
{
	unsigned long *virt_addr = va;
	unsigned long temp;
	u32 i;

	for (i = 0; i < size / sizeof(unsigned long); i++) {
		/*
		 * Very carefully read and write to memory atomically
		 * so we are interrupt, DMA and SMP safe.
		 *
		 * Intel: asm("lock; addl $0, %0"::"m"(*virt_addr));
		 */

		__asm__ __volatile__ (
		"	.set	mips2					\n"
<<<<<<< HEAD
		"1:	ll	%0, %1		# atomic_scrub		\n"
=======
		"1:	ll	%0, %1		# edac_atomic_scrub	\n"
>>>>>>> v4.9.227
		"	addu	%0, $0					\n"
		"	sc	%0, %1					\n"
		"	beqz	%0, 1b					\n"
		"	.set	mips0					\n"
<<<<<<< HEAD
		: "=&r" (temp), "=m" (*virt_addr)
		: "m" (*virt_addr));
=======
		: "=&r" (temp), "=" GCC_OFF_SMALL_ASM() (*virt_addr)
		: GCC_OFF_SMALL_ASM() (*virt_addr));
>>>>>>> v4.9.227

		virt_addr++;
	}
}

#endif
