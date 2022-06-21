/*
 * BPF Jit compiler for s390, help functions.
 *
<<<<<<< HEAD
 * Copyright IBM Corp. 2012
 *
 * Author(s): Martin Schwidefsky <schwidefsky@de.ibm.com>
 */
#include <linux/linkage.h>

/*
 * Calling convention:
 * registers %r2, %r6-%r8, %r10-%r11, %r13, %r15 are call saved
 *   %r2: skb pointer
 *   %r3: offset parameter
 *   %r5: BPF A accumulator
 *   %r8: return address
 *   %r9: save register for skb pointer
 *   %r10: skb->data
 *   %r11: skb->len - skb->data_len (headlen)
 *   %r12: BPF X accumulator
=======
 * Copyright IBM Corp. 2012,2015
 *
 * Author(s): Martin Schwidefsky <schwidefsky@de.ibm.com>
 *	      Michael Holzheu <holzheu@linux.vnet.ibm.com>
 */

#include <linux/linkage.h>
#include <asm/nospec-insn.h>
#include "bpf_jit.h"

/*
 * Calling convention:
 * registers %r7-%r10, %r11,%r13, and %r15 are call saved
 *
 * Input (64 bit):
 *   %r3 (%b2) = offset into skb data
 *   %r6 (%b5) = return address
 *   %r7 (%b6) = skb pointer
 *   %r12      = skb data pointer
 *
 * Output:
 *   %r14= %b0 = return value (read skb value)
 *
 * Work registers: %r2,%r4,%r5,%r14
>>>>>>> v4.9.227
 *
 * skb_copy_bits takes 4 parameters:
 *   %r2 = skb pointer
 *   %r3 = offset into skb data
<<<<<<< HEAD
 *   %r4 = length to copy
 *   %r5 = pointer to temp buffer
 */
#define SKBDATA	%r8

	/* A = *(u32 *) (skb->data+K+X) */
ENTRY(sk_load_word_ind)
	ar	%r3,%r12		# offset += X
	bmr	%r8			# < 0 -> return with cc

	/* A = *(u32 *) (skb->data+K) */
ENTRY(sk_load_word)
	llgfr	%r1,%r3			# extend offset
	ahi	%r3,4			# offset + 4
	clr	%r11,%r3		# hlen <= offset + 4 ?
	jl	sk_load_word_slow
	l	%r5,0(%r1,%r10)		# get word from skb
	xr	%r1,%r1			# set cc to zero
	br	%r8

sk_load_word_slow:
	lgr	%r9,%r2			# save %r2
	lhi	%r4,4			# 4 bytes
	la	%r5,160(%r15)		# pointer to temp buffer
	brasl	%r14,skb_copy_bits	# get data from skb
	l	%r5,160(%r15)		# load result from temp buffer
	ltgr	%r2,%r2			# set cc to (%r2 != 0)
	lgr	%r2,%r9			# restore %r2
	br	%r8

	/* A = *(u16 *) (skb->data+K+X) */
ENTRY(sk_load_half_ind)
	ar	%r3,%r12		# offset += X
	bmr	%r8			# < 0 -> return with cc

	/* A = *(u16 *) (skb->data+K) */
ENTRY(sk_load_half)
	llgfr	%r1,%r3			# extend offset
	ahi	%r3,2			# offset + 2
	clr	%r11,%r3		# hlen <= offset + 2 ?
	jl	sk_load_half_slow
	llgh	%r5,0(%r1,%r10)		# get half from skb
	xr	%r1,%r1			# set cc to zero
	br	%r8

sk_load_half_slow:
	lgr	%r9,%r2			# save %r2
	lhi	%r4,2			# 2 bytes
	la	%r5,162(%r15)		# pointer to temp buffer
	brasl	%r14,skb_copy_bits	# get data from skb
	xc	160(2,%r15),160(%r15)
	l	%r5,160(%r15)		# load result from temp buffer
	ltgr	%r2,%r2			# set cc to (%r2 != 0)
	lgr	%r2,%r9			# restore %r2
	br	%r8

	/* A = *(u8 *) (skb->data+K+X) */
ENTRY(sk_load_byte_ind)
	ar	%r3,%r12		# offset += X
	bmr	%r8			# < 0 -> return with cc

	/* A = *(u8 *) (skb->data+K) */
ENTRY(sk_load_byte)
	llgfr	%r1,%r3			# extend offset
	clr	%r11,%r3		# hlen < offset ?
	jle	sk_load_byte_slow
	lhi	%r5,0
	ic	%r5,0(%r1,%r10)		# get byte from skb
	xr	%r1,%r1			# set cc to zero
	br	%r8

sk_load_byte_slow:
	lgr	%r9,%r2			# save %r2
	lhi	%r4,1			# 1 bytes
	la	%r5,163(%r15)		# pointer to temp buffer
	brasl	%r14,skb_copy_bits	# get data from skb
	xc	160(3,%r15),160(%r15)
	l	%r5,160(%r15)		# load result from temp buffer
	ltgr	%r2,%r2			# set cc to (%r2 != 0)
	lgr	%r2,%r9			# restore %r2
	br	%r8

	/* A = (*(u8 *)(skb->data+K) & 0xf) << 2 */
ENTRY(sk_load_byte_msh)
	llgfr	%r1,%r3			# extend offset
	clr	%r11,%r3		# hlen < offset ?
	jle	sk_load_byte_slow
	lhi	%r12,0
	ic	%r12,0(%r1,%r10)	# get byte from skb
	nill	%r12,0x0f
	sll	%r12,2
	xr	%r1,%r1			# set cc to zero
	br	%r8

sk_load_byte_msh_slow:
	lgr	%r9,%r2			# save %r2
	lhi	%r4,2			# 2 bytes
	la	%r5,162(%r15)		# pointer to temp buffer
	brasl	%r14,skb_copy_bits	# get data from skb
	xc	160(3,%r15),160(%r15)
	l	%r12,160(%r15)		# load result from temp buffer
	nill	%r12,0x0f
	sll	%r12,2
	ltgr	%r2,%r2			# set cc to (%r2 != 0)
	lgr	%r2,%r9			# restore %r2
	br	%r8
=======
 *   %r4 = pointer to temp buffer
 *   %r5 = length to copy
 *   Return value in %r2: 0 = ok
 *
 * bpf_internal_load_pointer_neg_helper takes 3 parameters:
 *   %r2 = skb pointer
 *   %r3 = offset into data
 *   %r4 = length to copy
 *   Return value in %r2: Pointer to data
 */

#define SKF_MAX_NEG_OFF	-0x200000	/* SKF_LL_OFF from filter.h */

/*
 * Load SIZE bytes from SKB
 */
#define sk_load_common(NAME, SIZE, LOAD)				\
ENTRY(sk_load_##NAME);							\
	ltgr	%r3,%r3;		/* Is offset negative? */	\
	jl	sk_load_##NAME##_slow_neg;				\
ENTRY(sk_load_##NAME##_pos);						\
	aghi	%r3,SIZE;		/* Offset + SIZE */		\
	clg	%r3,STK_OFF_HLEN(%r15);	/* Offset + SIZE > hlen? */	\
	jh	sk_load_##NAME##_slow;					\
	LOAD	%r14,-SIZE(%r3,%r12);	/* Get data from skb */		\
	B_EX	OFF_OK,%r6;		/* Return */			\
									\
sk_load_##NAME##_slow:;							\
	lgr	%r2,%r7;		/* Arg1 = skb pointer */	\
	aghi	%r3,-SIZE;		/* Arg2 = offset */		\
	la	%r4,STK_OFF_TMP(%r15);	/* Arg3 = temp bufffer */	\
	lghi	%r5,SIZE;		/* Arg4 = size */		\
	brasl	%r14,skb_copy_bits;	/* Get data from skb */		\
	LOAD	%r14,STK_OFF_TMP(%r15);	/* Load from temp bufffer */	\
	ltgr	%r2,%r2;		/* Set cc to (%r2 != 0) */	\
	BR_EX	%r6;			/* Return */

sk_load_common(word, 4, llgf)	/* r14 = *(u32 *) (skb->data+offset) */
sk_load_common(half, 2, llgh)	/* r14 = *(u16 *) (skb->data+offset) */

	GEN_BR_THUNK %r6
	GEN_B_THUNK OFF_OK,%r6

/*
 * Load 1 byte from SKB (optimized version)
 */
	/* r14 = *(u8 *) (skb->data+offset) */
ENTRY(sk_load_byte)
	ltgr	%r3,%r3			# Is offset negative?
	jl	sk_load_byte_slow_neg
ENTRY(sk_load_byte_pos)
	clg	%r3,STK_OFF_HLEN(%r15)	# Offset >= hlen?
	jnl	sk_load_byte_slow
	llgc	%r14,0(%r3,%r12)	# Get byte from skb
	B_EX	OFF_OK,%r6		# Return OK

sk_load_byte_slow:
	lgr	%r2,%r7			# Arg1 = skb pointer
					# Arg2 = offset
	la	%r4,STK_OFF_TMP(%r15)	# Arg3 = pointer to temp buffer
	lghi	%r5,1			# Arg4 = size (1 byte)
	brasl	%r14,skb_copy_bits	# Get data from skb
	llgc	%r14,STK_OFF_TMP(%r15)	# Load result from temp buffer
	ltgr	%r2,%r2			# Set cc to (%r2 != 0)
	BR_EX	%r6			# Return cc

#define sk_negative_common(NAME, SIZE, LOAD)				\
sk_load_##NAME##_slow_neg:;						\
	cgfi	%r3,SKF_MAX_NEG_OFF;					\
	jl	bpf_error;						\
	lgr	%r2,%r7;		/* Arg1 = skb pointer */	\
					/* Arg2 = offset */		\
	lghi	%r4,SIZE;		/* Arg3 = size */		\
	brasl	%r14,bpf_internal_load_pointer_neg_helper;		\
	ltgr	%r2,%r2;						\
	jz	bpf_error;						\
	LOAD	%r14,0(%r2);		/* Get data from pointer */	\
	xr	%r3,%r3;		/* Set cc to zero */		\
	BR_EX	%r6;			/* Return cc */

sk_negative_common(word, 4, llgf)
sk_negative_common(half, 2, llgh)
sk_negative_common(byte, 1, llgc)

bpf_error:
# force a return 0 from jit handler
	ltgr	%r15,%r15	# Set condition code
	BR_EX	%r6
>>>>>>> v4.9.227
