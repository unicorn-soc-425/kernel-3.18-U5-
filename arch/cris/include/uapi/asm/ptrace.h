<<<<<<< HEAD
#include <arch/ptrace.h>
=======
#ifdef __arch_v32
#include <asm/ptrace_v32.h>
#else
#include <asm/ptrace_v10.h>
#endif
>>>>>>> v4.9.227
