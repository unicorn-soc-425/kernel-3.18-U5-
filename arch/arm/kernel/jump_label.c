#include <linux/kernel.h>
#include <linux/jump_label.h>
<<<<<<< HEAD

#include "insn.h"
#include "patch.h"
=======
#include <asm/patch.h>
#include <asm/insn.h>
>>>>>>> v4.9.227

#ifdef HAVE_JUMP_LABEL

static void __arch_jump_label_transform(struct jump_entry *entry,
					enum jump_label_type type,
					bool is_static)
{
	void *addr = (void *)entry->code;
	unsigned int insn;

<<<<<<< HEAD
	if (type == JUMP_LABEL_ENABLE)
=======
	if (type == JUMP_LABEL_JMP)
>>>>>>> v4.9.227
		insn = arm_gen_branch(entry->code, entry->target);
	else
		insn = arm_gen_nop();

	if (is_static)
		__patch_text_early(addr, insn);
	else
		patch_text(addr, insn);
}

void arch_jump_label_transform(struct jump_entry *entry,
			       enum jump_label_type type)
{
	__arch_jump_label_transform(entry, type, false);
}

void arch_jump_label_transform_static(struct jump_entry *entry,
				      enum jump_label_type type)
{
	__arch_jump_label_transform(entry, type, true);
}

#endif
