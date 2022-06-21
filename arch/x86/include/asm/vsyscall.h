#ifndef _ASM_X86_VSYSCALL_H
#define _ASM_X86_VSYSCALL_H

#include <linux/seqlock.h>
#include <uapi/asm/vsyscall.h>

<<<<<<< HEAD
#define VGETCPU_RDTSCP	1
#define VGETCPU_LSL	2

/* kernel space (writeable) */
extern int vgetcpu_mode;
extern struct timezone sys_tz;

#include <asm/vvar.h>

=======
#ifdef CONFIG_X86_VSYSCALL_EMULATION
>>>>>>> v4.9.227
extern void map_vsyscall(void);

/*
 * Called on instruction fetch fault in vsyscall page.
 * Returns true if handled.
 */
extern bool emulate_vsyscall(struct pt_regs *regs, unsigned long address);
<<<<<<< HEAD

#ifdef CONFIG_X86_64

#define VGETCPU_CPU_MASK 0xfff

static inline unsigned int __getcpu(void)
{
	unsigned int p;

	if (VVAR(vgetcpu_mode) == VGETCPU_RDTSCP) {
		/* Load per CPU data from RDTSCP */
		native_read_tscp(&p);
	} else {
		/* Load per CPU data from GDT */
		asm volatile ("lsl %1,%0" : "=r" (p) : "r" (__PER_CPU_SEG));
	}

	return p;
}
#endif /* CONFIG_X86_64 */
=======
extern bool vsyscall_enabled(void);
#else
static inline void map_vsyscall(void) {}
static inline bool emulate_vsyscall(struct pt_regs *regs, unsigned long address)
{
	return false;
}
static inline bool vsyscall_enabled(void) { return false; }
#endif
extern unsigned long vsyscall_pgprot;
>>>>>>> v4.9.227

#endif /* _ASM_X86_VSYSCALL_H */
