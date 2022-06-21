#ifndef __ASM_PARISC_BITSPERLONG_H
#define __ASM_PARISC_BITSPERLONG_H

#if defined(__LP64__)
#define __BITS_PER_LONG 64
<<<<<<< HEAD
#define SHIFT_PER_LONG 6
#else
#define __BITS_PER_LONG 32
#define SHIFT_PER_LONG 5
=======
#else
#define __BITS_PER_LONG 32
>>>>>>> v4.9.227
#endif

#include <asm-generic/bitsperlong.h>

#endif /* __ASM_PARISC_BITSPERLONG_H */
