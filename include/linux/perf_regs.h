#ifndef _LINUX_PERF_REGS_H
#define _LINUX_PERF_REGS_H

<<<<<<< HEAD
=======
struct perf_regs {
	__u64		abi;
	struct pt_regs	*regs;
};

>>>>>>> v4.9.227
#ifdef CONFIG_HAVE_PERF_REGS
#include <asm/perf_regs.h>
u64 perf_reg_value(struct pt_regs *regs, int idx);
int perf_reg_validate(u64 mask);
u64 perf_reg_abi(struct task_struct *task);
<<<<<<< HEAD
=======
void perf_get_regs_user(struct perf_regs *regs_user,
			struct pt_regs *regs,
			struct pt_regs *regs_user_copy);
>>>>>>> v4.9.227
#else
static inline u64 perf_reg_value(struct pt_regs *regs, int idx)
{
	return 0;
}

static inline int perf_reg_validate(u64 mask)
{
	return mask ? -ENOSYS : 0;
}

static inline u64 perf_reg_abi(struct task_struct *task)
{
	return PERF_SAMPLE_REGS_ABI_NONE;
}
<<<<<<< HEAD
=======

static inline void perf_get_regs_user(struct perf_regs *regs_user,
				      struct pt_regs *regs,
				      struct pt_regs *regs_user_copy)
{
	regs_user->regs = task_pt_regs(current);
	regs_user->abi = perf_reg_abi(current);
}
>>>>>>> v4.9.227
#endif /* CONFIG_HAVE_PERF_REGS */
#endif /* _LINUX_PERF_REGS_H */
