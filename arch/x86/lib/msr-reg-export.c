<<<<<<< HEAD
#include <linux/module.h>
=======
#include <linux/export.h>
>>>>>>> v4.9.227
#include <asm/msr.h>

EXPORT_SYMBOL(rdmsr_safe_regs);
EXPORT_SYMBOL(wrmsr_safe_regs);
