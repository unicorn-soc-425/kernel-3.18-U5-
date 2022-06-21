#ifndef _ASM_ARM64_XEN_EVENTS_H
#define _ASM_ARM64_XEN_EVENTS_H

#include <asm/ptrace.h>
#include <asm/atomic.h>

enum ipi_vector {
	XEN_PLACEHOLDER_VECTOR,

	/* Xen IPIs go here */
	XEN_NR_IPIS,
};

static inline int xen_irqs_disabled(struct pt_regs *regs)
{
	return raw_irqs_disabled_flags((unsigned long) regs->pstate);
}

#define xchg_xen_ulong(ptr, val) xchg((ptr), (val))

<<<<<<< HEAD
=======
/* Rebind event channel is supported by default */
static inline bool xen_support_evtchn_rebind(void)
{
	return true;
}

>>>>>>> v4.9.227
#endif /* _ASM_ARM64_XEN_EVENTS_H */
