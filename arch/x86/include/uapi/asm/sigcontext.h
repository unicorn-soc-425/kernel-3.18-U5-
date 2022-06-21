#ifndef _UAPI_ASM_X86_SIGCONTEXT_H
#define _UAPI_ASM_X86_SIGCONTEXT_H

<<<<<<< HEAD
#include <linux/compiler.h>
#include <linux/types.h>

#define FP_XSTATE_MAGIC1	0x46505853U
#define FP_XSTATE_MAGIC2	0x46505845U
#define FP_XSTATE_MAGIC2_SIZE	sizeof(FP_XSTATE_MAGIC2)

/*
 * bytes 464..511 in the current 512byte layout of fxsave/fxrstor frame
 * are reserved for SW usage. On cpu's supporting xsave/xrstor, these bytes
 * are used to extended the fpstate pointer in the sigcontext, which now
 * includes the extended state information along with fpstate information.
 *
 * Presence of FP_XSTATE_MAGIC1 at the beginning of this SW reserved
 * area and FP_XSTATE_MAGIC2 at the end of memory layout
 * (extended_size - FP_XSTATE_MAGIC2_SIZE) indicates the presence of the
 * extended state information in the memory layout pointed by the fpstate
 * pointer in sigcontext.
 */
struct _fpx_sw_bytes {
	__u32 magic1;		/* FP_XSTATE_MAGIC1 */
	__u32 extended_size;	/* total size of the layout referred by
				 * fpstate pointer in the sigcontext.
				 */
	__u64 xstate_bv;
				/* feature bit mask (including fp/sse/extended
				 * state) that is present in the memory
				 * layout.
				 */
	__u32 xstate_size;	/* actual xsave state size, based on the
				 * features saved in the layout.
				 * 'extended_size' will be greater than
				 * 'xstate_size'.
				 */
	__u32 padding[7];	/*  for future use. */
};

#ifdef __i386__
/*
 * As documented in the iBCS2 standard..
 *
 * The first part of "struct _fpstate" is just the normal i387
 * hardware setup, the extra "status" word is used to save the
 * coprocessor status word before entering the handler.
 *
 * Pentium III FXSR, SSE support
 *	Gareth Hughes <gareth@valinux.com>, May 2000
 *
 * The FPU state data structure has had to grow to accommodate the
 * extended FPU state required by the Streaming SIMD Extensions.
 * There is no documented standard to accomplish this at the moment.
 */
struct _fpreg {
	unsigned short significand[4];
	unsigned short exponent;
};

struct _fpxreg {
	unsigned short significand[4];
	unsigned short exponent;
	unsigned short padding[3];
};

struct _xmmreg {
	unsigned long element[4];
};

struct _fpstate {
	/* Regular FPU environment */
	unsigned long	cw;
	unsigned long	sw;
	unsigned long	tag;
	unsigned long	ipoff;
	unsigned long	cssel;
	unsigned long	dataoff;
	unsigned long	datasel;
	struct _fpreg	_st[8];
	unsigned short	status;
	unsigned short	magic;		/* 0xffff = regular FPU data only */

	/* FXSR FPU environment */
	unsigned long	_fxsr_env[6];	/* FXSR FPU env is ignored */
	unsigned long	mxcsr;
	unsigned long	reserved;
	struct _fpxreg	_fxsr_st[8];	/* FXSR FPU reg data is ignored */
	struct _xmmreg	_xmm[8];
	unsigned long	padding1[44];

	union {
		unsigned long	padding2[12];
		struct _fpx_sw_bytes sw_reserved; /* represents the extended
						   * state info */
	};
};

#define X86_FXSR_MAGIC		0x0000

#ifndef __KERNEL__
/*
 * User-space might still rely on the old definition:
 */
struct sigcontext {
	unsigned short gs, __gsh;
	unsigned short fs, __fsh;
	unsigned short es, __esh;
	unsigned short ds, __dsh;
	unsigned long edi;
	unsigned long esi;
	unsigned long ebp;
	unsigned long esp;
	unsigned long ebx;
	unsigned long edx;
	unsigned long ecx;
	unsigned long eax;
	unsigned long trapno;
	unsigned long err;
	unsigned long eip;
	unsigned short cs, __csh;
	unsigned long eflags;
	unsigned long esp_at_signal;
	unsigned short ss, __ssh;
	struct _fpstate __user *fpstate;
	unsigned long oldmask;
	unsigned long cr2;
};
#endif /* !__KERNEL__ */

#else /* __i386__ */

/* FXSAVE frame */
/* Note: reserved1/2 may someday contain valuable data. Always save/restore
   them when you change signal frames. */
struct _fpstate {
	__u16	cwd;
	__u16	swd;
	__u16	twd;		/* Note this is not the same as the
				   32bit/x87/FSAVE twd */
	__u16	fop;
	__u64	rip;
	__u64	rdp;
	__u32	mxcsr;
	__u32	mxcsr_mask;
	__u32	st_space[32];	/* 8*16 bytes for each FP-reg */
	__u32	xmm_space[64];	/* 16*16 bytes for each XMM-reg  */
	__u32	reserved2[12];
	union {
		__u32	reserved3[12];
		struct _fpx_sw_bytes sw_reserved; /* represents the extended
						   * state information */
	};
};

#ifndef __KERNEL__
/*
 * User-space might still rely on the old definition:
 */
struct sigcontext {
	__u64 r8;
	__u64 r9;
	__u64 r10;
	__u64 r11;
	__u64 r12;
	__u64 r13;
	__u64 r14;
	__u64 r15;
	__u64 rdi;
	__u64 rsi;
	__u64 rbp;
	__u64 rbx;
	__u64 rdx;
	__u64 rax;
	__u64 rcx;
	__u64 rsp;
	__u64 rip;
	__u64 eflags;		/* RFLAGS */
	__u16 cs;
	__u16 gs;
	__u16 fs;
	__u16 __pad0;
	__u64 err;
	__u64 trapno;
	__u64 oldmask;
	__u64 cr2;
	struct _fpstate __user *fpstate;	/* zero when no FPU context */
#ifdef __ILP32__
	__u32 __fpstate_pad;
#endif
	__u64 reserved1[8];
};
#endif /* !__KERNEL__ */

#endif /* !__i386__ */

struct _xsave_hdr {
	__u64 xstate_bv;
	__u64 reserved1[2];
	__u64 reserved2[5];
};

struct _ymmh_state {
	/* 16 * 16 bytes for each YMMH-reg */
	__u32 ymmh_space[64];
};

/*
 * Extended state pointed by the fpstate pointer in the sigcontext.
 * In addition to the fpstate, information encoded in the xstate_hdr
 * indicates the presence of other extended state information
 * supported by the processor and OS.
 */
struct _xstate {
	struct _fpstate fpstate;
	struct _xsave_hdr xstate_hdr;
	struct _ymmh_state ymmh;
	/* new processor state extensions go here */
};

=======
/*
 * Linux signal context definitions. The sigcontext includes a complex
 * hierarchy of CPU and FPU state, available to user-space (on the stack) when
 * a signal handler is executed.
 *
 * As over the years this ABI grew from its very simple roots towards
 * supporting more and more CPU state organically, some of the details (which
 * were rather clever hacks back in the days) became a bit quirky by today.
 *
 * The current ABI includes flexible provisions for future extensions, so we
 * won't have to grow new quirks for quite some time. Promise!
 */

#include <linux/compiler.h>
#include <linux/types.h>

#define FP_XSTATE_MAGIC1		0x46505853U
#define FP_XSTATE_MAGIC2		0x46505845U
#define FP_XSTATE_MAGIC2_SIZE		sizeof(FP_XSTATE_MAGIC2)

/*
 * Bytes 464..511 in the current 512-byte layout of the FXSAVE/FXRSTOR frame
 * are reserved for SW usage. On CPUs supporting XSAVE/XRSTOR, these bytes are
 * used to extend the fpstate pointer in the sigcontext, which now includes the
 * extended state information along with fpstate information.
 *
 * If sw_reserved.magic1 == FP_XSTATE_MAGIC1 then there's a
 * sw_reserved.extended_size bytes large extended context area present. (The
 * last 32-bit word of this extended area (at the
 * fpstate+extended_size-FP_XSTATE_MAGIC2_SIZE address) is set to
 * FP_XSTATE_MAGIC2 so that you can sanity check your size calculations.)
 *
 * This extended area typically grows with newer CPUs that have larger and
 * larger XSAVE areas.
 */
struct _fpx_sw_bytes {
	/*
	 * If set to FP_XSTATE_MAGIC1 then this is an xstate context.
	 * 0 if a legacy frame.
	 */
	__u32				magic1;

	/*
	 * Total size of the fpstate area:
	 *
	 *  - if magic1 == 0 then it's sizeof(struct _fpstate)
	 *  - if magic1 == FP_XSTATE_MAGIC1 then it's sizeof(struct _xstate)
	 *    plus extensions (if any)
	 */
	__u32				extended_size;

	/*
	 * Feature bit mask (including FP/SSE/extended state) that is present
	 * in the memory layout:
	 */
	__u64				xfeatures;

	/*
	 * Actual XSAVE state size, based on the xfeatures saved in the layout.
	 * 'extended_size' is greater than 'xstate_size':
	 */
	__u32				xstate_size;

	/* For future use: */
	__u32				padding[7];
};

/*
 * As documented in the iBCS2 standard:
 *
 * The first part of "struct _fpstate" is just the normal i387 hardware setup,
 * the extra "status" word is used to save the coprocessor status word before
 * entering the handler.
 *
 * The FPU state data structure has had to grow to accommodate the extended FPU
 * state required by the Streaming SIMD Extensions.  There is no documented
 * standard to accomplish this at the moment.
 */

/* 10-byte legacy floating point register: */
struct _fpreg {
	__u16				significand[4];
	__u16				exponent;
};

/* 16-byte floating point register: */
struct _fpxreg {
	__u16				significand[4];
	__u16				exponent;
	__u16				padding[3];
};

/* 16-byte XMM register: */
struct _xmmreg {
	__u32				element[4];
};

#define X86_FXSR_MAGIC			0x0000

/*
 * The 32-bit FPU frame:
 */
struct _fpstate_32 {
	/* Legacy FPU environment: */
	__u32				cw;
	__u32				sw;
	__u32				tag;
	__u32				ipoff;
	__u32				cssel;
	__u32				dataoff;
	__u32				datasel;
	struct _fpreg			_st[8];
	__u16				status;
	__u16				magic;		/* 0xffff: regular FPU data only */
							/* 0x0000: FXSR FPU data */

	/* FXSR FPU environment */
	__u32				_fxsr_env[6];	/* FXSR FPU env is ignored */
	__u32				mxcsr;
	__u32				reserved;
	struct _fpxreg			_fxsr_st[8];	/* FXSR FPU reg data is ignored */
	struct _xmmreg			_xmm[8];	/* First 8 XMM registers */
	union {
		__u32			padding1[44];	/* Second 8 XMM registers plus padding */
		__u32			padding[44];	/* Alias name for old user-space */
	};

	union {
		__u32			padding2[12];
		struct _fpx_sw_bytes	sw_reserved;	/* Potential extended state is encoded here */
	};
};

/*
 * The 64-bit FPU frame. (FXSAVE format and later)
 *
 * Note1: If sw_reserved.magic1 == FP_XSTATE_MAGIC1 then the structure is
 *        larger: 'struct _xstate'. Note that 'struct _xstate' embedds
 *        'struct _fpstate' so that you can always assume the _fpstate portion
 *        exists so that you can check the magic value.
 *
 * Note2: Reserved fields may someday contain valuable data. Always
 *	  save/restore them when you change signal frames.
 */
struct _fpstate_64 {
	__u16				cwd;
	__u16				swd;
	/* Note this is not the same as the 32-bit/x87/FSAVE twd: */
	__u16				twd;
	__u16				fop;
	__u64				rip;
	__u64				rdp;
	__u32				mxcsr;
	__u32				mxcsr_mask;
	__u32				st_space[32];	/*  8x  FP registers, 16 bytes each */
	__u32				xmm_space[64];	/* 16x XMM registers, 16 bytes each */
	__u32				reserved2[12];
	union {
		__u32			reserved3[12];
		struct _fpx_sw_bytes	sw_reserved;	/* Potential extended state is encoded here */
	};
};

#ifdef __i386__
# define _fpstate _fpstate_32
#else
# define _fpstate _fpstate_64
#endif

struct _header {
	__u64				xfeatures;
	__u64				reserved1[2];
	__u64				reserved2[5];
};

struct _ymmh_state {
	/* 16x YMM registers, 16 bytes each: */
	__u32				ymmh_space[64];
};

/*
 * Extended state pointed to by sigcontext::fpstate.
 *
 * In addition to the fpstate, information encoded in _xstate::xstate_hdr
 * indicates the presence of other extended state information supported
 * by the CPU and kernel:
 */
struct _xstate {
	struct _fpstate			fpstate;
	struct _header			xstate_hdr;
	struct _ymmh_state		ymmh;
	/* New processor state extensions go here: */
};

/*
 * The 32-bit signal frame:
 */
struct sigcontext_32 {
	__u16				gs, __gsh;
	__u16				fs, __fsh;
	__u16				es, __esh;
	__u16				ds, __dsh;
	__u32				di;
	__u32				si;
	__u32				bp;
	__u32				sp;
	__u32				bx;
	__u32				dx;
	__u32				cx;
	__u32				ax;
	__u32				trapno;
	__u32				err;
	__u32				ip;
	__u16				cs, __csh;
	__u32				flags;
	__u32				sp_at_signal;
	__u16				ss, __ssh;

	/*
	 * fpstate is really (struct _fpstate *) or (struct _xstate *)
	 * depending on the FP_XSTATE_MAGIC1 encoded in the SW reserved
	 * bytes of (struct _fpstate) and FP_XSTATE_MAGIC2 present at the end
	 * of extended memory layout. See comments at the definition of
	 * (struct _fpx_sw_bytes)
	 */
	__u32				fpstate; /* Zero when no FPU/extended context */
	__u32				oldmask;
	__u32				cr2;
};

/*
 * The 64-bit signal frame:
 */
struct sigcontext_64 {
	__u64				r8;
	__u64				r9;
	__u64				r10;
	__u64				r11;
	__u64				r12;
	__u64				r13;
	__u64				r14;
	__u64				r15;
	__u64				di;
	__u64				si;
	__u64				bp;
	__u64				bx;
	__u64				dx;
	__u64				ax;
	__u64				cx;
	__u64				sp;
	__u64				ip;
	__u64				flags;
	__u16				cs;
	__u16				gs;
	__u16				fs;
	__u16				ss;
	__u64				err;
	__u64				trapno;
	__u64				oldmask;
	__u64				cr2;

	/*
	 * fpstate is really (struct _fpstate *) or (struct _xstate *)
	 * depending on the FP_XSTATE_MAGIC1 encoded in the SW reserved
	 * bytes of (struct _fpstate) and FP_XSTATE_MAGIC2 present at the end
	 * of extended memory layout. See comments at the definition of
	 * (struct _fpx_sw_bytes)
	 */
	__u64				fpstate; /* Zero when no FPU/extended context */
	__u64				reserved1[8];
};

/*
 * Create the real 'struct sigcontext' type:
 */
#ifdef __KERNEL__
# ifdef __i386__
#  define sigcontext sigcontext_32
# else
#  define sigcontext sigcontext_64
# endif
#endif

/*
 * The old user-space sigcontext definition, just in case user-space still
 * relies on it. The kernel definition (in asm/sigcontext.h) has unified
 * field names but otherwise the same layout.
 */
#ifndef __KERNEL__

#define _fpstate_ia32			_fpstate_32
#define sigcontext_ia32			sigcontext_32


# ifdef __i386__
struct sigcontext {
	__u16				gs, __gsh;
	__u16				fs, __fsh;
	__u16				es, __esh;
	__u16				ds, __dsh;
	__u32				edi;
	__u32				esi;
	__u32				ebp;
	__u32				esp;
	__u32				ebx;
	__u32				edx;
	__u32				ecx;
	__u32				eax;
	__u32				trapno;
	__u32				err;
	__u32				eip;
	__u16				cs, __csh;
	__u32				eflags;
	__u32				esp_at_signal;
	__u16				ss, __ssh;
	struct _fpstate __user		*fpstate;
	__u32				oldmask;
	__u32				cr2;
};
# else /* __x86_64__: */
struct sigcontext {
	__u64				r8;
	__u64				r9;
	__u64				r10;
	__u64				r11;
	__u64				r12;
	__u64				r13;
	__u64				r14;
	__u64				r15;
	__u64				rdi;
	__u64				rsi;
	__u64				rbp;
	__u64				rbx;
	__u64				rdx;
	__u64				rax;
	__u64				rcx;
	__u64				rsp;
	__u64				rip;
	__u64				eflags;		/* RFLAGS */
	__u16				cs;

	/*
	 * Prior to 2.5.64 ("[PATCH] x86-64 updates for 2.5.64-bk3"),
	 * Linux saved and restored fs and gs in these slots.  This
	 * was counterproductive, as fsbase and gsbase were never
	 * saved, so arch_prctl was presumably unreliable.
	 *
	 * These slots should never be reused without extreme caution:
	 *
	 *  - Some DOSEMU versions stash fs and gs in these slots manually,
	 *    thus overwriting anything the kernel expects to be preserved
	 *    in these slots.
	 *
	 *  - If these slots are ever needed for any other purpose,
	 *    there is some risk that very old 64-bit binaries could get
	 *    confused.  I doubt that many such binaries still work,
	 *    though, since the same patch in 2.5.64 also removed the
	 *    64-bit set_thread_area syscall, so it appears that there
	 *    is no TLS API beyond modify_ldt that works in both pre-
	 *    and post-2.5.64 kernels.
	 *
	 * If the kernel ever adds explicit fs, gs, fsbase, and gsbase
	 * save/restore, it will most likely need to be opt-in and use
	 * different context slots.
	 */
	__u16				gs;
	__u16				fs;
	union {
		__u16			ss;	/* If UC_SIGCONTEXT_SS */
		__u16			__pad0;	/* Alias name for old (!UC_SIGCONTEXT_SS) user-space */
	};
	__u64				err;
	__u64				trapno;
	__u64				oldmask;
	__u64				cr2;
	struct _fpstate __user		*fpstate;	/* Zero when no FPU context */
#  ifdef __ILP32__
	__u32				__fpstate_pad;
#  endif
	__u64				reserved1[8];
};
# endif /* __x86_64__ */
#endif /* !__KERNEL__ */

>>>>>>> v4.9.227
#endif /* _UAPI_ASM_X86_SIGCONTEXT_H */
