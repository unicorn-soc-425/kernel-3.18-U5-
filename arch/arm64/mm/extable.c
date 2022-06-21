/*
 * Based on arch/arm/mm/extable.c
 */

<<<<<<< HEAD
#include <linux/module.h>
=======
#include <linux/extable.h>
>>>>>>> v4.9.227
#include <linux/uaccess.h>

int fixup_exception(struct pt_regs *regs)
{
	const struct exception_table_entry *fixup;

	fixup = search_exception_tables(instruction_pointer(regs));
	if (fixup)
<<<<<<< HEAD
		regs->pc = fixup->fixup;
=======
		regs->pc = (unsigned long)&fixup->fixup + fixup->fixup;
>>>>>>> v4.9.227

	return fixup != NULL;
}
