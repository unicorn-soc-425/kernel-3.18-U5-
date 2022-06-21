#ifndef _LINUX_MMU_CONTEXT_H
#define _LINUX_MMU_CONTEXT_H

<<<<<<< HEAD
=======
#include <asm/mmu_context.h>

>>>>>>> v4.9.227
struct mm_struct;

void use_mm(struct mm_struct *mm);
void unuse_mm(struct mm_struct *mm);

<<<<<<< HEAD
=======
/* Architectures that care about IRQ state in switch_mm can override this. */
#ifndef switch_mm_irqs_off
# define switch_mm_irqs_off switch_mm
#endif

>>>>>>> v4.9.227
#endif
