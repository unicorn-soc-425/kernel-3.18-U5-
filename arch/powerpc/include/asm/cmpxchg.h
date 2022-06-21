#ifndef _ASM_POWERPC_CMPXCHG_H_
#define _ASM_POWERPC_CMPXCHG_H_

#ifdef __KERNEL__
#include <linux/compiler.h>
#include <asm/synch.h>
#include <asm/asm-compat.h>
<<<<<<< HEAD
=======
#include <linux/bug.h>
>>>>>>> v4.9.227

/*
 * Atomic exchange
 *
<<<<<<< HEAD
 * Changes the memory location '*ptr' to be val and returns
 * the previous value stored there.
 */
static __always_inline unsigned long
__xchg_u32(volatile void *p, unsigned long val)
{
	unsigned long prev;

	__asm__ __volatile__(
	PPC_RELEASE_BARRIER
"1:	lwarx	%0,0,%2 \n"
	PPC405_ERR77(0,%2)
"	stwcx.	%3,0,%2 \n\
	bne-	1b"
	PPC_ACQUIRE_BARRIER
	: "=&r" (prev), "+m" (*(volatile unsigned int *)p)
	: "r" (p), "r" (val)
	: "cc", "memory");

	return prev;
}

/*
 * Atomic exchange
 *
 * Changes the memory location '*ptr' to be val and returns
 * the previous value stored there.
 */
=======
 * Changes the memory location '*p' to be val and returns
 * the previous value stored there.
 */

>>>>>>> v4.9.227
static __always_inline unsigned long
__xchg_u32_local(volatile void *p, unsigned long val)
{
	unsigned long prev;

	__asm__ __volatile__(
"1:	lwarx	%0,0,%2 \n"
	PPC405_ERR77(0,%2)
"	stwcx.	%3,0,%2 \n\
	bne-	1b"
	: "=&r" (prev), "+m" (*(volatile unsigned int *)p)
	: "r" (p), "r" (val)
	: "cc", "memory");

	return prev;
}

<<<<<<< HEAD
#ifdef CONFIG_PPC64
static __always_inline unsigned long
__xchg_u64(volatile void *p, unsigned long val)
=======
static __always_inline unsigned long
__xchg_u32_relaxed(u32 *p, unsigned long val)
>>>>>>> v4.9.227
{
	unsigned long prev;

	__asm__ __volatile__(
<<<<<<< HEAD
	PPC_RELEASE_BARRIER
"1:	ldarx	%0,0,%2 \n"
	PPC405_ERR77(0,%2)
"	stdcx.	%3,0,%2 \n\
	bne-	1b"
	PPC_ACQUIRE_BARRIER
	: "=&r" (prev), "+m" (*(volatile unsigned long *)p)
	: "r" (p), "r" (val)
	: "cc", "memory");
=======
"1:	lwarx	%0,0,%2\n"
	PPC405_ERR77(0, %2)
"	stwcx.	%3,0,%2\n"
"	bne-	1b"
	: "=&r" (prev), "+m" (*p)
	: "r" (p), "r" (val)
	: "cc");
>>>>>>> v4.9.227

	return prev;
}

<<<<<<< HEAD
=======
#ifdef CONFIG_PPC64
>>>>>>> v4.9.227
static __always_inline unsigned long
__xchg_u64_local(volatile void *p, unsigned long val)
{
	unsigned long prev;

	__asm__ __volatile__(
"1:	ldarx	%0,0,%2 \n"
	PPC405_ERR77(0,%2)
"	stdcx.	%3,0,%2 \n\
	bne-	1b"
	: "=&r" (prev), "+m" (*(volatile unsigned long *)p)
	: "r" (p), "r" (val)
	: "cc", "memory");

	return prev;
}
<<<<<<< HEAD
#endif

/*
 * This function doesn't exist, so you'll get a linker error
 * if something tries to do an invalid xchg().
 */
extern void __xchg_called_with_bad_pointer(void);

static __always_inline unsigned long
__xchg(volatile void *ptr, unsigned long x, unsigned int size)
{
	switch (size) {
	case 4:
		return __xchg_u32(ptr, x);
#ifdef CONFIG_PPC64
	case 8:
		return __xchg_u64(ptr, x);
#endif
	}
	__xchg_called_with_bad_pointer();
	return x;
}
=======

static __always_inline unsigned long
__xchg_u64_relaxed(u64 *p, unsigned long val)
{
	unsigned long prev;

	__asm__ __volatile__(
"1:	ldarx	%0,0,%2\n"
	PPC405_ERR77(0, %2)
"	stdcx.	%3,0,%2\n"
"	bne-	1b"
	: "=&r" (prev), "+m" (*p)
	: "r" (p), "r" (val)
	: "cc");

	return prev;
}
#endif
>>>>>>> v4.9.227

static __always_inline unsigned long
__xchg_local(volatile void *ptr, unsigned long x, unsigned int size)
{
	switch (size) {
	case 4:
		return __xchg_u32_local(ptr, x);
#ifdef CONFIG_PPC64
	case 8:
		return __xchg_u64_local(ptr, x);
#endif
	}
<<<<<<< HEAD
	__xchg_called_with_bad_pointer();
	return x;
}
#define xchg(ptr,x)							     \
  ({									     \
     __typeof__(*(ptr)) _x_ = (x);					     \
     (__typeof__(*(ptr))) __xchg((ptr), (unsigned long)_x_, sizeof(*(ptr))); \
  })

=======
	BUILD_BUG_ON_MSG(1, "Unsupported size for __xchg");
	return x;
}

static __always_inline unsigned long
__xchg_relaxed(void *ptr, unsigned long x, unsigned int size)
{
	switch (size) {
	case 4:
		return __xchg_u32_relaxed(ptr, x);
#ifdef CONFIG_PPC64
	case 8:
		return __xchg_u64_relaxed(ptr, x);
#endif
	}
	BUILD_BUG_ON_MSG(1, "Unsupported size for __xchg_local");
	return x;
}
>>>>>>> v4.9.227
#define xchg_local(ptr,x)						     \
  ({									     \
     __typeof__(*(ptr)) _x_ = (x);					     \
     (__typeof__(*(ptr))) __xchg_local((ptr),				     \
     		(unsigned long)_x_, sizeof(*(ptr))); 			     \
  })

<<<<<<< HEAD
=======
#define xchg_relaxed(ptr, x)						\
({									\
	__typeof__(*(ptr)) _x_ = (x);					\
	(__typeof__(*(ptr))) __xchg_relaxed((ptr),			\
			(unsigned long)_x_, sizeof(*(ptr)));		\
})
>>>>>>> v4.9.227
/*
 * Compare and exchange - if *p == old, set it to new,
 * and return the old value of *p.
 */
<<<<<<< HEAD
#define __HAVE_ARCH_CMPXCHG	1
=======
>>>>>>> v4.9.227

static __always_inline unsigned long
__cmpxchg_u32(volatile unsigned int *p, unsigned long old, unsigned long new)
{
	unsigned int prev;

	__asm__ __volatile__ (
<<<<<<< HEAD
	PPC_RELEASE_BARRIER
=======
	PPC_ATOMIC_ENTRY_BARRIER
>>>>>>> v4.9.227
"1:	lwarx	%0,0,%2		# __cmpxchg_u32\n\
	cmpw	0,%0,%3\n\
	bne-	2f\n"
	PPC405_ERR77(0,%2)
"	stwcx.	%4,0,%2\n\
	bne-	1b"
<<<<<<< HEAD
	PPC_ACQUIRE_BARRIER
=======
	PPC_ATOMIC_EXIT_BARRIER
>>>>>>> v4.9.227
	"\n\
2:"
	: "=&r" (prev), "+m" (*p)
	: "r" (p), "r" (old), "r" (new)
	: "cc", "memory");

	return prev;
}

static __always_inline unsigned long
__cmpxchg_u32_local(volatile unsigned int *p, unsigned long old,
			unsigned long new)
{
	unsigned int prev;

	__asm__ __volatile__ (
"1:	lwarx	%0,0,%2		# __cmpxchg_u32\n\
	cmpw	0,%0,%3\n\
	bne-	2f\n"
	PPC405_ERR77(0,%2)
"	stwcx.	%4,0,%2\n\
	bne-	1b"
	"\n\
2:"
	: "=&r" (prev), "+m" (*p)
	: "r" (p), "r" (old), "r" (new)
	: "cc", "memory");

	return prev;
}

<<<<<<< HEAD
=======
static __always_inline unsigned long
__cmpxchg_u32_relaxed(u32 *p, unsigned long old, unsigned long new)
{
	unsigned long prev;

	__asm__ __volatile__ (
"1:	lwarx	%0,0,%2		# __cmpxchg_u32_relaxed\n"
"	cmpw	0,%0,%3\n"
"	bne-	2f\n"
	PPC405_ERR77(0, %2)
"	stwcx.	%4,0,%2\n"
"	bne-	1b\n"
"2:"
	: "=&r" (prev), "+m" (*p)
	: "r" (p), "r" (old), "r" (new)
	: "cc");

	return prev;
}

/*
 * cmpxchg family don't have order guarantee if cmp part fails, therefore we
 * can avoid superfluous barriers if we use assembly code to implement
 * cmpxchg() and cmpxchg_acquire(), however we don't do the similar for
 * cmpxchg_release() because that will result in putting a barrier in the
 * middle of a ll/sc loop, which is probably a bad idea. For example, this
 * might cause the conditional store more likely to fail.
 */
static __always_inline unsigned long
__cmpxchg_u32_acquire(u32 *p, unsigned long old, unsigned long new)
{
	unsigned long prev;

	__asm__ __volatile__ (
"1:	lwarx	%0,0,%2		# __cmpxchg_u32_acquire\n"
"	cmpw	0,%0,%3\n"
"	bne-	2f\n"
	PPC405_ERR77(0, %2)
"	stwcx.	%4,0,%2\n"
"	bne-	1b\n"
	PPC_ACQUIRE_BARRIER
	"\n"
"2:"
	: "=&r" (prev), "+m" (*p)
	: "r" (p), "r" (old), "r" (new)
	: "cc", "memory");

	return prev;
}

>>>>>>> v4.9.227
#ifdef CONFIG_PPC64
static __always_inline unsigned long
__cmpxchg_u64(volatile unsigned long *p, unsigned long old, unsigned long new)
{
	unsigned long prev;

	__asm__ __volatile__ (
<<<<<<< HEAD
	PPC_RELEASE_BARRIER
=======
	PPC_ATOMIC_ENTRY_BARRIER
>>>>>>> v4.9.227
"1:	ldarx	%0,0,%2		# __cmpxchg_u64\n\
	cmpd	0,%0,%3\n\
	bne-	2f\n\
	stdcx.	%4,0,%2\n\
	bne-	1b"
<<<<<<< HEAD
	PPC_ACQUIRE_BARRIER
=======
	PPC_ATOMIC_EXIT_BARRIER
>>>>>>> v4.9.227
	"\n\
2:"
	: "=&r" (prev), "+m" (*p)
	: "r" (p), "r" (old), "r" (new)
	: "cc", "memory");

	return prev;
}

static __always_inline unsigned long
__cmpxchg_u64_local(volatile unsigned long *p, unsigned long old,
			unsigned long new)
{
	unsigned long prev;

	__asm__ __volatile__ (
"1:	ldarx	%0,0,%2		# __cmpxchg_u64\n\
	cmpd	0,%0,%3\n\
	bne-	2f\n\
	stdcx.	%4,0,%2\n\
	bne-	1b"
	"\n\
2:"
	: "=&r" (prev), "+m" (*p)
	: "r" (p), "r" (old), "r" (new)
	: "cc", "memory");

	return prev;
}
<<<<<<< HEAD
#endif

/* This function doesn't exist, so you'll get a linker error
   if something tries to do an invalid cmpxchg().  */
extern void __cmpxchg_called_with_bad_pointer(void);
=======

static __always_inline unsigned long
__cmpxchg_u64_relaxed(u64 *p, unsigned long old, unsigned long new)
{
	unsigned long prev;

	__asm__ __volatile__ (
"1:	ldarx	%0,0,%2		# __cmpxchg_u64_relaxed\n"
"	cmpd	0,%0,%3\n"
"	bne-	2f\n"
"	stdcx.	%4,0,%2\n"
"	bne-	1b\n"
"2:"
	: "=&r" (prev), "+m" (*p)
	: "r" (p), "r" (old), "r" (new)
	: "cc");

	return prev;
}

static __always_inline unsigned long
__cmpxchg_u64_acquire(u64 *p, unsigned long old, unsigned long new)
{
	unsigned long prev;

	__asm__ __volatile__ (
"1:	ldarx	%0,0,%2		# __cmpxchg_u64_acquire\n"
"	cmpd	0,%0,%3\n"
"	bne-	2f\n"
"	stdcx.	%4,0,%2\n"
"	bne-	1b\n"
	PPC_ACQUIRE_BARRIER
	"\n"
"2:"
	: "=&r" (prev), "+m" (*p)
	: "r" (p), "r" (old), "r" (new)
	: "cc", "memory");

	return prev;
}
#endif
>>>>>>> v4.9.227

static __always_inline unsigned long
__cmpxchg(volatile void *ptr, unsigned long old, unsigned long new,
	  unsigned int size)
{
	switch (size) {
	case 4:
		return __cmpxchg_u32(ptr, old, new);
#ifdef CONFIG_PPC64
	case 8:
		return __cmpxchg_u64(ptr, old, new);
#endif
	}
<<<<<<< HEAD
	__cmpxchg_called_with_bad_pointer();
=======
	BUILD_BUG_ON_MSG(1, "Unsupported size for __cmpxchg");
>>>>>>> v4.9.227
	return old;
}

static __always_inline unsigned long
__cmpxchg_local(volatile void *ptr, unsigned long old, unsigned long new,
	  unsigned int size)
{
	switch (size) {
	case 4:
		return __cmpxchg_u32_local(ptr, old, new);
#ifdef CONFIG_PPC64
	case 8:
		return __cmpxchg_u64_local(ptr, old, new);
#endif
	}
<<<<<<< HEAD
	__cmpxchg_called_with_bad_pointer();
	return old;
}

=======
	BUILD_BUG_ON_MSG(1, "Unsupported size for __cmpxchg_local");
	return old;
}

static __always_inline unsigned long
__cmpxchg_relaxed(void *ptr, unsigned long old, unsigned long new,
		  unsigned int size)
{
	switch (size) {
	case 4:
		return __cmpxchg_u32_relaxed(ptr, old, new);
#ifdef CONFIG_PPC64
	case 8:
		return __cmpxchg_u64_relaxed(ptr, old, new);
#endif
	}
	BUILD_BUG_ON_MSG(1, "Unsupported size for __cmpxchg_relaxed");
	return old;
}

static __always_inline unsigned long
__cmpxchg_acquire(void *ptr, unsigned long old, unsigned long new,
		  unsigned int size)
{
	switch (size) {
	case 4:
		return __cmpxchg_u32_acquire(ptr, old, new);
#ifdef CONFIG_PPC64
	case 8:
		return __cmpxchg_u64_acquire(ptr, old, new);
#endif
	}
	BUILD_BUG_ON_MSG(1, "Unsupported size for __cmpxchg_acquire");
	return old;
}
>>>>>>> v4.9.227
#define cmpxchg(ptr, o, n)						 \
  ({									 \
     __typeof__(*(ptr)) _o_ = (o);					 \
     __typeof__(*(ptr)) _n_ = (n);					 \
     (__typeof__(*(ptr))) __cmpxchg((ptr), (unsigned long)_o_,		 \
				    (unsigned long)_n_, sizeof(*(ptr))); \
  })


#define cmpxchg_local(ptr, o, n)					 \
  ({									 \
     __typeof__(*(ptr)) _o_ = (o);					 \
     __typeof__(*(ptr)) _n_ = (n);					 \
     (__typeof__(*(ptr))) __cmpxchg_local((ptr), (unsigned long)_o_,	 \
				    (unsigned long)_n_, sizeof(*(ptr))); \
  })

<<<<<<< HEAD
=======
#define cmpxchg_relaxed(ptr, o, n)					\
({									\
	__typeof__(*(ptr)) _o_ = (o);					\
	__typeof__(*(ptr)) _n_ = (n);					\
	(__typeof__(*(ptr))) __cmpxchg_relaxed((ptr),			\
			(unsigned long)_o_, (unsigned long)_n_,		\
			sizeof(*(ptr)));				\
})

#define cmpxchg_acquire(ptr, o, n)					\
({									\
	__typeof__(*(ptr)) _o_ = (o);					\
	__typeof__(*(ptr)) _n_ = (n);					\
	(__typeof__(*(ptr))) __cmpxchg_acquire((ptr),			\
			(unsigned long)_o_, (unsigned long)_n_,		\
			sizeof(*(ptr)));				\
})
>>>>>>> v4.9.227
#ifdef CONFIG_PPC64
#define cmpxchg64(ptr, o, n)						\
  ({									\
	BUILD_BUG_ON(sizeof(*(ptr)) != 8);				\
	cmpxchg((ptr), (o), (n));					\
  })
#define cmpxchg64_local(ptr, o, n)					\
  ({									\
	BUILD_BUG_ON(sizeof(*(ptr)) != 8);				\
	cmpxchg_local((ptr), (o), (n));					\
  })
<<<<<<< HEAD
#define cmpxchg64_relaxed	cmpxchg64_local
=======
#define cmpxchg64_relaxed(ptr, o, n)					\
({									\
	BUILD_BUG_ON(sizeof(*(ptr)) != 8);				\
	cmpxchg_relaxed((ptr), (o), (n));				\
})
#define cmpxchg64_acquire(ptr, o, n)					\
({									\
	BUILD_BUG_ON(sizeof(*(ptr)) != 8);				\
	cmpxchg_acquire((ptr), (o), (n));				\
})
>>>>>>> v4.9.227
#else
#include <asm-generic/cmpxchg-local.h>
#define cmpxchg64_local(ptr, o, n) __cmpxchg64_local_generic((ptr), (o), (n))
#endif

#endif /* __KERNEL__ */
#endif /* _ASM_POWERPC_CMPXCHG_H_ */
