/* atomic.h: atomic operation emulation for FR-V
 *
 * For an explanation of how atomic ops work in this arch, see:
 *   Documentation/frv/atomic-ops.txt
 *
 * Copyright (C) 2004 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#ifndef _ASM_ATOMIC_H
#define _ASM_ATOMIC_H

#include <linux/types.h>
<<<<<<< HEAD
#include <asm/spr-regs.h>
=======
>>>>>>> v4.9.227
#include <asm/cmpxchg.h>
#include <asm/barrier.h>

#ifdef CONFIG_SMP
#error not SMP safe
#endif

<<<<<<< HEAD
=======
#include <asm/atomic_defs.h>

>>>>>>> v4.9.227
/*
 * Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc..
 *
 * We do not have SMP systems, so we don't have to deal with that.
 */

#define ATOMIC_INIT(i)		{ (i) }
<<<<<<< HEAD
#define atomic_read(v)		ACCESS_ONCE((v)->counter)
#define atomic_set(v, i)	(((v)->counter) = (i))

#ifndef CONFIG_FRV_OUTOFLINE_ATOMIC_OPS
static inline int atomic_add_return(int i, atomic_t *v)
{
	unsigned long val;

	asm("0:						\n"
	    "	orcc		gr0,gr0,gr0,icc3	\n"	/* set ICC3.Z */
	    "	ckeq		icc3,cc7		\n"
	    "	ld.p		%M0,%1			\n"	/* LD.P/ORCR must be atomic */
	    "	orcr		cc7,cc7,cc3		\n"	/* set CC3 to true */
	    "	add%I2		%1,%2,%1		\n"
	    "	cst.p		%1,%M0		,cc3,#1	\n"
	    "	corcc		gr29,gr29,gr0	,cc3,#1	\n"	/* clear ICC3.Z if store happens */
	    "	beq		icc3,#0,0b		\n"
	    : "+U"(v->counter), "=&r"(val)
	    : "NPr"(i)
	    : "memory", "cc7", "cc3", "icc3"
	    );

	return val;
=======
#define atomic_read(v)		READ_ONCE((v)->counter)
#define atomic_set(v, i)	WRITE_ONCE(((v)->counter), (i))

static inline int atomic_inc_return(atomic_t *v)
{
	return __atomic_add_return(1, &v->counter);
}

static inline int atomic_dec_return(atomic_t *v)
{
	return __atomic_sub_return(1, &v->counter);
}

static inline int atomic_add_return(int i, atomic_t *v)
{
	return __atomic_add_return(i, &v->counter);
>>>>>>> v4.9.227
}

static inline int atomic_sub_return(int i, atomic_t *v)
{
<<<<<<< HEAD
	unsigned long val;

	asm("0:						\n"
	    "	orcc		gr0,gr0,gr0,icc3	\n"	/* set ICC3.Z */
	    "	ckeq		icc3,cc7		\n"
	    "	ld.p		%M0,%1			\n"	/* LD.P/ORCR must be atomic */
	    "	orcr		cc7,cc7,cc3		\n"	/* set CC3 to true */
	    "	sub%I2		%1,%2,%1		\n"
	    "	cst.p		%1,%M0		,cc3,#1	\n"
	    "	corcc		gr29,gr29,gr0	,cc3,#1	\n"	/* clear ICC3.Z if store happens */
	    "	beq		icc3,#0,0b		\n"
	    : "+U"(v->counter), "=&r"(val)
	    : "NPr"(i)
	    : "memory", "cc7", "cc3", "icc3"
	    );

	return val;
}

#else

extern int atomic_add_return(int i, atomic_t *v);
extern int atomic_sub_return(int i, atomic_t *v);

#endif

=======
	return __atomic_sub_return(i, &v->counter);
}

>>>>>>> v4.9.227
static inline int atomic_add_negative(int i, atomic_t *v)
{
	return atomic_add_return(i, v) < 0;
}

<<<<<<< HEAD
static inline void atomic_add(int i, atomic_t *v)
{
	atomic_add_return(i, v);
}

static inline void atomic_sub(int i, atomic_t *v)
{
	atomic_sub_return(i, v);
}

static inline void atomic_inc(atomic_t *v)
{
	atomic_add_return(1, v);
=======
static inline void atomic_inc(atomic_t *v)
{
	atomic_inc_return(v);
>>>>>>> v4.9.227
}

static inline void atomic_dec(atomic_t *v)
{
<<<<<<< HEAD
	atomic_sub_return(1, v);
}

#define atomic_dec_return(v)		atomic_sub_return(1, (v))
#define atomic_inc_return(v)		atomic_add_return(1, (v))

=======
	atomic_dec_return(v);
}

>>>>>>> v4.9.227
#define atomic_sub_and_test(i,v)	(atomic_sub_return((i), (v)) == 0)
#define atomic_dec_and_test(v)		(atomic_sub_return(1, (v)) == 0)
#define atomic_inc_and_test(v)		(atomic_add_return(1, (v)) == 0)

/*
 * 64-bit atomic ops
 */
typedef struct {
<<<<<<< HEAD
	volatile long long counter;
=======
	long long counter;
>>>>>>> v4.9.227
} atomic64_t;

#define ATOMIC64_INIT(i)	{ (i) }

<<<<<<< HEAD
static inline long long atomic64_read(atomic64_t *v)
=======
static inline long long atomic64_read(const atomic64_t *v)
>>>>>>> v4.9.227
{
	long long counter;

	asm("ldd%I1 %M1,%0"
	    : "=e"(counter)
	    : "m"(v->counter));
<<<<<<< HEAD
=======

>>>>>>> v4.9.227
	return counter;
}

static inline void atomic64_set(atomic64_t *v, long long i)
{
	asm volatile("std%I0 %1,%M0"
		     : "=m"(v->counter)
		     : "e"(i));
}

<<<<<<< HEAD
extern long long atomic64_inc_return(atomic64_t *v);
extern long long atomic64_dec_return(atomic64_t *v);
extern long long atomic64_add_return(long long i, atomic64_t *v);
extern long long atomic64_sub_return(long long i, atomic64_t *v);
=======
static inline long long atomic64_inc_return(atomic64_t *v)
{
	return __atomic64_add_return(1, &v->counter);
}

static inline long long atomic64_dec_return(atomic64_t *v)
{
	return __atomic64_sub_return(1, &v->counter);
}

static inline long long atomic64_add_return(long long i, atomic64_t *v)
{
	return __atomic64_add_return(i, &v->counter);
}

static inline long long atomic64_sub_return(long long i, atomic64_t *v)
{
	return __atomic64_sub_return(i, &v->counter);
}
>>>>>>> v4.9.227

static inline long long atomic64_add_negative(long long i, atomic64_t *v)
{
	return atomic64_add_return(i, v) < 0;
}

<<<<<<< HEAD
static inline void atomic64_add(long long i, atomic64_t *v)
{
	atomic64_add_return(i, v);
}

static inline void atomic64_sub(long long i, atomic64_t *v)
{
	atomic64_sub_return(i, v);
}

=======
>>>>>>> v4.9.227
static inline void atomic64_inc(atomic64_t *v)
{
	atomic64_inc_return(v);
}

static inline void atomic64_dec(atomic64_t *v)
{
	atomic64_dec_return(v);
}

#define atomic64_sub_and_test(i,v)	(atomic64_sub_return((i), (v)) == 0)
#define atomic64_dec_and_test(v)	(atomic64_dec_return((v)) == 0)
#define atomic64_inc_and_test(v)	(atomic64_inc_return((v)) == 0)
<<<<<<< HEAD
=======
#define atomic64_inc_not_zero(v)	atomic64_add_unless((v), 1, 0)
>>>>>>> v4.9.227

#define atomic_cmpxchg(v, old, new)	(cmpxchg(&(v)->counter, old, new))
#define atomic_xchg(v, new)		(xchg(&(v)->counter, new))
#define atomic64_cmpxchg(v, old, new)	(__cmpxchg_64(old, new, &(v)->counter))
#define atomic64_xchg(v, new)		(__xchg_64(new, &(v)->counter))

static __inline__ int __atomic_add_unless(atomic_t *v, int a, int u)
{
	int c, old;
	c = atomic_read(v);
	for (;;) {
		if (unlikely(c == (u)))
			break;
		old = atomic_cmpxchg((v), c, c + (a));
		if (likely(old == c))
			break;
		c = old;
	}
	return c;
}

<<<<<<< HEAD
=======
static inline int atomic64_add_unless(atomic64_t *v, long long i, long long u)
{
	long long c, old;

	c = atomic64_read(v);
	for (;;) {
		if (unlikely(c == u))
			break;
		old = atomic64_cmpxchg(v, c, c + i);
		if (likely(old == c))
			break;
		c = old;
	}
	return c != u;
}

static inline long long atomic64_dec_if_positive(atomic64_t *v)
{
	long long c, old, dec;

	c = atomic64_read(v);
	for (;;) {
		dec = c - 1;
		if (unlikely(dec < 0))
			break;
		old = atomic64_cmpxchg((v), c, dec);
		if (likely(old == c))
			break;
		c = old;
	}
		return dec;
}

#define ATOMIC_OP(op)							\
static inline int atomic_fetch_##op(int i, atomic_t *v)			\
{									\
	return __atomic32_fetch_##op(i, &v->counter);			\
}									\
static inline void atomic_##op(int i, atomic_t *v)			\
{									\
	(void)__atomic32_fetch_##op(i, &v->counter);			\
}									\
									\
static inline long long atomic64_fetch_##op(long long i, atomic64_t *v)	\
{									\
	return __atomic64_fetch_##op(i, &v->counter);			\
}									\
static inline void atomic64_##op(long long i, atomic64_t *v)		\
{									\
	(void)__atomic64_fetch_##op(i, &v->counter);			\
}

ATOMIC_OP(or)
ATOMIC_OP(and)
ATOMIC_OP(xor)
ATOMIC_OP(add)
ATOMIC_OP(sub)

#undef ATOMIC_OP
>>>>>>> v4.9.227

#endif /* _ASM_ATOMIC_H */
