#ifndef __ARCH_MACH_COMMON_H
#define __ARCH_MACH_COMMON_H

<<<<<<< HEAD
extern void shmobile_earlytimer_init(void);
extern void shmobile_init_delay(void);
struct twd_local_timer;
extern void shmobile_setup_console(void);
extern void shmobile_boot_vector(void);
extern unsigned long shmobile_boot_fn;
extern unsigned long shmobile_boot_arg;
=======
extern void shmobile_init_delay(void);
extern void shmobile_boot_vector(void);
extern unsigned long shmobile_boot_fn;
>>>>>>> v4.9.227
extern unsigned long shmobile_boot_size;
extern void shmobile_smp_boot(void);
extern void shmobile_smp_sleep(void);
extern void shmobile_smp_hook(unsigned int cpu, unsigned long fn,
			      unsigned long arg);
<<<<<<< HEAD
extern int shmobile_smp_cpu_disable(unsigned int cpu);
extern void shmobile_invalidate_start(void);
extern void shmobile_boot_scu(void);
extern void shmobile_smp_scu_prepare_cpus(unsigned int max_cpus);
extern void shmobile_smp_scu_cpu_die(unsigned int cpu);
extern int shmobile_smp_scu_cpu_kill(unsigned int cpu);
extern void shmobile_smp_apmu_prepare_cpus(unsigned int max_cpus);
extern int shmobile_smp_apmu_boot_secondary(unsigned int cpu,
					    struct task_struct *idle);
extern void shmobile_smp_apmu_cpu_die(unsigned int cpu);
extern int shmobile_smp_apmu_cpu_kill(unsigned int cpu);
struct clk;
extern int shmobile_clk_init(void);
extern void shmobile_handle_irq_intc(struct pt_regs *);
extern struct platform_suspend_ops shmobile_suspend_ops;
struct cpuidle_driver;
extern void shmobile_cpuidle_set_driver(struct cpuidle_driver *drv);
=======
extern bool shmobile_smp_cpu_can_disable(unsigned int cpu);
extern bool shmobile_smp_init_fallback_ops(void);
extern void shmobile_boot_scu(void);
extern void shmobile_smp_scu_prepare_cpus(phys_addr_t scu_base_phys,
					  unsigned int max_cpus);
extern void shmobile_smp_scu_cpu_die(unsigned int cpu);
extern int shmobile_smp_scu_cpu_kill(unsigned int cpu);
extern struct platform_suspend_ops shmobile_suspend_ops;
>>>>>>> v4.9.227

#ifdef CONFIG_SUSPEND
int shmobile_suspend_init(void);
void shmobile_smp_apmu_suspend_init(void);
#else
static inline int shmobile_suspend_init(void) { return 0; }
static inline void shmobile_smp_apmu_suspend_init(void) { }
#endif

<<<<<<< HEAD
#ifdef CONFIG_CPU_IDLE
int shmobile_cpuidle_init(void);
#else
static inline int shmobile_cpuidle_init(void) { return 0; }
#endif

#ifdef CONFIG_CPU_FREQ
int shmobile_cpufreq_init(void);
#else
static inline int shmobile_cpufreq_init(void) { return 0; }
#endif

extern void __iomem *shmobile_scu_base;

static inline void __init shmobile_init_late(void)
{
	shmobile_suspend_init();
	shmobile_cpuidle_init();
	shmobile_cpufreq_init();
=======
static inline void __init shmobile_init_late(void)
{
	shmobile_suspend_init();
>>>>>>> v4.9.227
}

#endif /* __ARCH_MACH_COMMON_H */
