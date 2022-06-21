#ifndef _LINUX_BITOPS_H
#define _LINUX_BITOPS_H
#include <asm/types.h>
<<<<<<< HEAD

#ifdef	__KERNEL__
#define BIT(nr)			(1UL << (nr))
#define BIT_ULL(nr)		(1ULL << (nr))
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)	(1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)	((nr) / BITS_PER_LONG_LONG)
#define BITS_PER_BYTE		8
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))
#endif

/*
 * Create a contiguous bitmask starting at bit position @l and ending at
 * position @h. For example
 * GENMASK_ULL(39, 21) gives us the 64bit vector 0x000000ffffe00000.
 */
#define GENMASK(h, l) \
	(((~0UL) << (l)) & (~0UL >> (BITS_PER_LONG - 1 - (h))))

#define GENMASK_ULL(h, l) \
	(((~0ULL) << (l)) & (~0ULL >> (BITS_PER_LONG_LONG - 1 - (h))))
=======
#include <linux/bits.h>

#define BITS_PER_TYPE(type) (sizeof(type) * BITS_PER_BYTE)
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_TYPE(long))
>>>>>>> v4.9.227

extern unsigned int __sw_hweight8(unsigned int w);
extern unsigned int __sw_hweight16(unsigned int w);
extern unsigned int __sw_hweight32(unsigned int w);
extern unsigned long __sw_hweight64(__u64 w);

/*
 * Include this here because some architectures need generic_ffs/fls in
 * scope
 */
#include <asm/bitops.h>

#define for_each_set_bit(bit, addr, size) \
	for ((bit) = find_first_bit((addr), (size));		\
	     (bit) < (size);					\
	     (bit) = find_next_bit((addr), (size), (bit) + 1))

/* same as for_each_set_bit() but use bit as value to start with */
#define for_each_set_bit_from(bit, addr, size) \
	for ((bit) = find_next_bit((addr), (size), (bit));	\
	     (bit) < (size);					\
	     (bit) = find_next_bit((addr), (size), (bit) + 1))

#define for_each_clear_bit(bit, addr, size) \
	for ((bit) = find_first_zero_bit((addr), (size));	\
	     (bit) < (size);					\
	     (bit) = find_next_zero_bit((addr), (size), (bit) + 1))

/* same as for_each_clear_bit() but use bit as value to start with */
#define for_each_clear_bit_from(bit, addr, size) \
	for ((bit) = find_next_zero_bit((addr), (size), (bit));	\
	     (bit) < (size);					\
	     (bit) = find_next_zero_bit((addr), (size), (bit) + 1))

<<<<<<< HEAD
static __inline__ int get_bitmask_order(unsigned int count)
=======
static inline int get_bitmask_order(unsigned int count)
>>>>>>> v4.9.227
{
	int order;

	order = fls(count);
	return order;	/* We could be slightly more clever with -1 here... */
}

<<<<<<< HEAD
static __inline__ int get_count_order(unsigned int count)
{
	int order;

	order = fls(count) - 1;
	if (count & (count - 1))
		order++;
	return order;
}

static inline unsigned long hweight_long(unsigned long w)
=======
static __always_inline unsigned long hweight_long(unsigned long w)
>>>>>>> v4.9.227
{
	return sizeof(w) == 4 ? hweight32(w) : hweight64(w);
}

/**
 * rol64 - rotate a 64-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline __u64 rol64(__u64 word, unsigned int shift)
{
<<<<<<< HEAD
	return (word << shift) | (word >> (64 - shift));
=======
	return (word << (shift & 63)) | (word >> ((-shift) & 63));
>>>>>>> v4.9.227
}

/**
 * ror64 - rotate a 64-bit value right
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline __u64 ror64(__u64 word, unsigned int shift)
{
<<<<<<< HEAD
	return (word >> shift) | (word << (64 - shift));
=======
	return (word >> (shift & 63)) | (word << ((-shift) & 63));
>>>>>>> v4.9.227
}

/**
 * rol32 - rotate a 32-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline __u32 rol32(__u32 word, unsigned int shift)
{
<<<<<<< HEAD
	return (word << shift) | (word >> (32 - shift));
=======
	return (word << (shift & 31)) | (word >> ((-shift) & 31));
>>>>>>> v4.9.227
}

/**
 * ror32 - rotate a 32-bit value right
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline __u32 ror32(__u32 word, unsigned int shift)
{
<<<<<<< HEAD
	return (word >> shift) | (word << (32 - shift));
=======
	return (word >> (shift & 31)) | (word << ((-shift) & 31));
>>>>>>> v4.9.227
}

/**
 * rol16 - rotate a 16-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline __u16 rol16(__u16 word, unsigned int shift)
{
<<<<<<< HEAD
	return (word << shift) | (word >> (16 - shift));
=======
	return (word << (shift & 15)) | (word >> ((-shift) & 15));
>>>>>>> v4.9.227
}

/**
 * ror16 - rotate a 16-bit value right
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline __u16 ror16(__u16 word, unsigned int shift)
{
<<<<<<< HEAD
	return (word >> shift) | (word << (16 - shift));
=======
	return (word >> (shift & 15)) | (word << ((-shift) & 15));
>>>>>>> v4.9.227
}

/**
 * rol8 - rotate an 8-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline __u8 rol8(__u8 word, unsigned int shift)
{
<<<<<<< HEAD
	return (word << shift) | (word >> (8 - shift));
=======
	return (word << (shift & 7)) | (word >> ((-shift) & 7));
>>>>>>> v4.9.227
}

/**
 * ror8 - rotate an 8-bit value right
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline __u8 ror8(__u8 word, unsigned int shift)
{
<<<<<<< HEAD
	return (word >> shift) | (word << (8 - shift));
=======
	return (word >> (shift & 7)) | (word << ((-shift) & 7));
>>>>>>> v4.9.227
}

/**
 * sign_extend32 - sign extend a 32-bit value using specified bit as sign-bit
 * @value: value to sign extend
 * @index: 0 based bit index (0<=index<32) to sign bit
<<<<<<< HEAD
=======
 *
 * This is safe to use for 16- and 8-bit types as well.
>>>>>>> v4.9.227
 */
static inline __s32 sign_extend32(__u32 value, int index)
{
	__u8 shift = 31 - index;
	return (__s32)(value << shift) >> shift;
}

/**
 * sign_extend64 - sign extend a 64-bit value using specified bit as sign-bit
 * @value: value to sign extend
 * @index: 0 based bit index (0<=index<64) to sign bit
 */
static inline __s64 sign_extend64(__u64 value, int index)
{
	__u8 shift = 63 - index;
	return (__s64)(value << shift) >> shift;
}

static inline unsigned fls_long(unsigned long l)
{
	if (sizeof(l) == 4)
		return fls(l);
	return fls64(l);
}

<<<<<<< HEAD
=======
static inline int get_count_order(unsigned int count)
{
	int order;

	order = fls(count) - 1;
	if (count & (count - 1))
		order++;
	return order;
}

/**
 * get_count_order_long - get order after rounding @l up to power of 2
 * @l: parameter
 *
 * it is same as get_count_order() but with long type parameter
 */
static inline int get_count_order_long(unsigned long l)
{
	if (l == 0UL)
		return -1;
	else if (l & (l - 1UL))
		return (int)fls_long(l);
	else
		return (int)fls_long(l) - 1;
}

>>>>>>> v4.9.227
/**
 * __ffs64 - find first set bit in a 64 bit word
 * @word: The 64 bit word
 *
 * On 64 bit arches this is a synomyn for __ffs
 * The result is not defined if no bits are set, so check that @word
 * is non-zero before calling this.
 */
static inline unsigned long __ffs64(u64 word)
{
#if BITS_PER_LONG == 32
	if (((u32)word) == 0UL)
		return __ffs((u32)(word >> 32)) + 32;
#elif BITS_PER_LONG != 64
#error BITS_PER_LONG not 32 or 64
#endif
	return __ffs((unsigned long)word);
}

#ifdef __KERNEL__

#ifndef set_mask_bits
#define set_mask_bits(ptr, _mask, _bits)	\
({								\
	const typeof(*ptr) mask = (_mask), bits = (_bits);	\
	typeof(*ptr) old, new;					\
								\
	do {							\
		old = ACCESS_ONCE(*ptr);			\
		new = (old & ~mask) | bits;			\
	} while (cmpxchg(ptr, old, new) != old);		\
								\
	new;							\
})
#endif

<<<<<<< HEAD
=======
#ifndef bit_clear_unless
#define bit_clear_unless(ptr, _clear, _test)	\
({								\
	const typeof(*ptr) clear = (_clear), test = (_test);	\
	typeof(*ptr) old, new;					\
								\
	do {							\
		old = ACCESS_ONCE(*ptr);			\
		new = old & ~clear;				\
	} while (!(old & test) &&				\
		 cmpxchg(ptr, old, new) != old);		\
								\
	!(old & test);						\
})
#endif

>>>>>>> v4.9.227
#ifndef find_last_bit
/**
 * find_last_bit - find the last set bit in a memory region
 * @addr: The address to start the search at
<<<<<<< HEAD
 * @size: The maximum size to search
 *
 * Returns the bit number of the first set bit, or size.
=======
 * @size: The number of bits to search
 *
 * Returns the bit number of the last set bit, or size.
>>>>>>> v4.9.227
 */
extern unsigned long find_last_bit(const unsigned long *addr,
				   unsigned long size);
#endif

#endif /* __KERNEL__ */
#endif
