#ifdef CONFIG_CGROUP_CPUACCT

extern void cpuacct_charge(struct task_struct *tsk, u64 cputime);
<<<<<<< HEAD
extern void cpuacct_account_field(struct task_struct *p, int index, u64 val);
=======
extern void cpuacct_account_field(struct task_struct *tsk, int index, u64 val);
>>>>>>> v4.9.227

#else

static inline void cpuacct_charge(struct task_struct *tsk, u64 cputime)
{
}

static inline void
<<<<<<< HEAD
cpuacct_account_field(struct task_struct *p, int index, u64 val)
=======
cpuacct_account_field(struct task_struct *tsk, int index, u64 val)
>>>>>>> v4.9.227
{
}

#endif
