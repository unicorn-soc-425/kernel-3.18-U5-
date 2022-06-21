/*
 * kernel/stop_machine.c
 *
 * Copyright (C) 2008, 2005	IBM Corporation.
 * Copyright (C) 2008, 2005	Rusty Russell rusty@rustcorp.com.au
 * Copyright (C) 2010		SUSE Linux Products GmbH
 * Copyright (C) 2010		Tejun Heo <tj@kernel.org>
 *
 * This file is released under the GPLv2 and any later version.
 */
#include <linux/completion.h>
#include <linux/cpu.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/export.h>
#include <linux/percpu.h>
#include <linux/sched.h>
#include <linux/stop_machine.h>
#include <linux/interrupt.h>
#include <linux/kallsyms.h>
#include <linux/smpboot.h>
#include <linux/atomic.h>
<<<<<<< HEAD
#include <linux/lglock.h>
=======
#include <linux/nmi.h>
>>>>>>> v4.9.227

/*
 * Structure to determine completion condition and record errors.  May
 * be shared by works on different cpus.
 */
struct cpu_stop_done {
	atomic_t		nr_todo;	/* nr left to execute */
<<<<<<< HEAD
	bool			executed;	/* actually executed? */
=======
>>>>>>> v4.9.227
	int			ret;		/* collected return value */
	struct completion	completion;	/* fired if nr_todo reaches 0 */
};

/* the actual stopper, one per every possible cpu, enabled on online cpus */
struct cpu_stopper {
<<<<<<< HEAD
	spinlock_t		lock;
	bool			enabled;	/* is this stopper enabled? */
	struct list_head	works;		/* list of pending works */
};

static DEFINE_PER_CPU(struct cpu_stopper, cpu_stopper);
static DEFINE_PER_CPU(struct task_struct *, cpu_stopper_task);
static bool stop_machine_initialized = false;

/*
 * Avoids a race between stop_two_cpus and global stop_cpus, where
 * the stoppers could get queued up in reverse order, leading to
 * system deadlock. Using an lglock means stop_two_cpus remains
 * relatively cheap.
 */
DEFINE_STATIC_LGLOCK(stop_cpus_lock);
=======
	struct task_struct	*thread;

	raw_spinlock_t		lock;
	bool			enabled;	/* is this stopper enabled? */
	struct list_head	works;		/* list of pending works */

	struct cpu_stop_work	stop_work;	/* for stop_cpus */
};

static DEFINE_PER_CPU(struct cpu_stopper, cpu_stopper);
static bool stop_machine_initialized = false;

/* static data for stop_cpus */
static DEFINE_MUTEX(stop_cpus_mutex);
static bool stop_cpus_in_progress;
>>>>>>> v4.9.227

static void cpu_stop_init_done(struct cpu_stop_done *done, unsigned int nr_todo)
{
	memset(done, 0, sizeof(*done));
	atomic_set(&done->nr_todo, nr_todo);
	init_completion(&done->completion);
}

/* signal completion unless @done is NULL */
<<<<<<< HEAD
static void cpu_stop_signal_done(struct cpu_stop_done *done, bool executed)
{
	if (done) {
		if (executed)
			done->executed = true;
		if (atomic_dec_and_test(&done->nr_todo))
			complete(&done->completion);
	}
}

/* queue @work to @stopper.  if offline, @work is completed immediately */
static void cpu_stop_queue_work(unsigned int cpu, struct cpu_stop_work *work)
{
	struct cpu_stopper *stopper = &per_cpu(cpu_stopper, cpu);
	struct task_struct *p = per_cpu(cpu_stopper_task, cpu);

	unsigned long flags;

	spin_lock_irqsave(&stopper->lock, flags);

	if (stopper->enabled) {
		list_add_tail(&work->list, &stopper->works);
		wake_up_process(p);
	} else
		cpu_stop_signal_done(work->done, false);

	spin_unlock_irqrestore(&stopper->lock, flags);
=======
static void cpu_stop_signal_done(struct cpu_stop_done *done)
{
	if (atomic_dec_and_test(&done->nr_todo))
		complete(&done->completion);
}

static void __cpu_stop_queue_work(struct cpu_stopper *stopper,
					struct cpu_stop_work *work)
{
	list_add_tail(&work->list, &stopper->works);
	wake_up_process(stopper->thread);
}

/* queue @work to @stopper.  if offline, @work is completed immediately */
static bool cpu_stop_queue_work(unsigned int cpu, struct cpu_stop_work *work)
{
	struct cpu_stopper *stopper = &per_cpu(cpu_stopper, cpu);
	unsigned long flags;
	bool enabled;

	raw_spin_lock_irqsave(&stopper->lock, flags);
	enabled = stopper->enabled;
	if (enabled)
		__cpu_stop_queue_work(stopper, work);
	else if (work->done)
		cpu_stop_signal_done(work->done);
	raw_spin_unlock_irqrestore(&stopper->lock, flags);

	return enabled;
>>>>>>> v4.9.227
}

/**
 * stop_one_cpu - stop a cpu
 * @cpu: cpu to stop
 * @fn: function to execute
 * @arg: argument to @fn
 *
 * Execute @fn(@arg) on @cpu.  @fn is run in a process context with
 * the highest priority preempting any task on the cpu and
 * monopolizing it.  This function returns after the execution is
 * complete.
 *
 * This function doesn't guarantee @cpu stays online till @fn
 * completes.  If @cpu goes down in the middle, execution may happen
 * partially or fully on different cpus.  @fn should either be ready
 * for that or the caller should ensure that @cpu stays online until
 * this function completes.
 *
 * CONTEXT:
 * Might sleep.
 *
 * RETURNS:
 * -ENOENT if @fn(@arg) was not executed because @cpu was offline;
 * otherwise, the return value of @fn.
 */
int stop_one_cpu(unsigned int cpu, cpu_stop_fn_t fn, void *arg)
{
	struct cpu_stop_done done;
	struct cpu_stop_work work = { .fn = fn, .arg = arg, .done = &done };

	cpu_stop_init_done(&done, 1);
<<<<<<< HEAD
	cpu_stop_queue_work(cpu, &work);
	wait_for_completion(&done.completion);
	return done.executed ? done.ret : -ENOENT;
=======
	if (!cpu_stop_queue_work(cpu, &work))
		return -ENOENT;
	/*
	 * In case @cpu == smp_proccessor_id() we can avoid a sleep+wakeup
	 * cycle by doing a preemption:
	 */
	cond_resched();
	wait_for_completion(&done.completion);
	return done.ret;
>>>>>>> v4.9.227
}

/* This controls the threads on each CPU. */
enum multi_stop_state {
	/* Dummy starting state for thread. */
	MULTI_STOP_NONE,
	/* Awaiting everyone to be scheduled. */
	MULTI_STOP_PREPARE,
	/* Disable interrupts. */
	MULTI_STOP_DISABLE_IRQ,
	/* Run the function */
	MULTI_STOP_RUN,
	/* Exit */
	MULTI_STOP_EXIT,
};

struct multi_stop_data {
<<<<<<< HEAD
	int			(*fn)(void *);
=======
	cpu_stop_fn_t		fn;
>>>>>>> v4.9.227
	void			*data;
	/* Like num_online_cpus(), but hotplug cpu uses us, so we need this. */
	unsigned int		num_threads;
	const struct cpumask	*active_cpus;

	enum multi_stop_state	state;
	atomic_t		thread_ack;
};

static void set_state(struct multi_stop_data *msdata,
		      enum multi_stop_state newstate)
{
	/* Reset ack counter. */
	atomic_set(&msdata->thread_ack, msdata->num_threads);
	smp_wmb();
	msdata->state = newstate;
}

/* Last one to ack a state moves to the next state. */
static void ack_state(struct multi_stop_data *msdata)
{
	if (atomic_dec_and_test(&msdata->thread_ack))
		set_state(msdata, msdata->state + 1);
}

/* This is the cpu_stop function which stops the CPU. */
static int multi_cpu_stop(void *data)
{
	struct multi_stop_data *msdata = data;
	enum multi_stop_state curstate = MULTI_STOP_NONE;
	int cpu = smp_processor_id(), err = 0;
	unsigned long flags;
	bool is_active;

	/*
	 * When called from stop_machine_from_inactive_cpu(), irq might
	 * already be disabled.  Save the state and restore it on exit.
	 */
	local_save_flags(flags);

	if (!msdata->active_cpus)
		is_active = cpu == cpumask_first(cpu_online_mask);
	else
		is_active = cpumask_test_cpu(cpu, msdata->active_cpus);

	/* Simple state machine */
	do {
		/* Chill out and ensure we re-read multi_stop_state. */
		cpu_relax();
		if (msdata->state != curstate) {
			curstate = msdata->state;
			switch (curstate) {
			case MULTI_STOP_DISABLE_IRQ:
				local_irq_disable();
				hard_irq_disable();
				break;
			case MULTI_STOP_RUN:
				if (is_active)
					err = msdata->fn(msdata->data);
				break;
			default:
				break;
			}
			ack_state(msdata);
<<<<<<< HEAD
=======
		} else if (curstate > MULTI_STOP_PREPARE) {
			/*
			 * At this stage all other CPUs we depend on must spin
			 * in the same loop. Any reason for hard-lockup should
			 * be detected and reported on their side.
			 */
			touch_nmi_watchdog();
>>>>>>> v4.9.227
		}
	} while (curstate != MULTI_STOP_EXIT);

	local_irq_restore(flags);
	return err;
}

<<<<<<< HEAD
struct irq_cpu_stop_queue_work_info {
	int cpu1;
	int cpu2;
	struct cpu_stop_work *work1;
	struct cpu_stop_work *work2;
};

/*
 * This function is always run with irqs and preemption disabled.
 * This guarantees that both work1 and work2 get queued, before
 * our local migrate thread gets the chance to preempt us.
 */
static void irq_cpu_stop_queue_work(void *arg)
{
	struct irq_cpu_stop_queue_work_info *info = arg;
	cpu_stop_queue_work(info->cpu1, info->work1);
	cpu_stop_queue_work(info->cpu2, info->work2);
}

=======
static int cpu_stop_queue_two_works(int cpu1, struct cpu_stop_work *work1,
				    int cpu2, struct cpu_stop_work *work2)
{
	struct cpu_stopper *stopper1 = per_cpu_ptr(&cpu_stopper, cpu1);
	struct cpu_stopper *stopper2 = per_cpu_ptr(&cpu_stopper, cpu2);
	int err;
retry:
	raw_spin_lock_irq(&stopper1->lock);
	raw_spin_lock_nested(&stopper2->lock, SINGLE_DEPTH_NESTING);

	err = -ENOENT;
	if (!stopper1->enabled || !stopper2->enabled)
		goto unlock;
	/*
	 * Ensure that if we race with __stop_cpus() the stoppers won't get
	 * queued up in reverse order leading to system deadlock.
	 *
	 * We can't miss stop_cpus_in_progress if queue_stop_cpus_work() has
	 * queued a work on cpu1 but not on cpu2, we hold both locks.
	 *
	 * It can be falsely true but it is safe to spin until it is cleared,
	 * queue_stop_cpus_work() does everything under preempt_disable().
	 */
	err = -EDEADLK;
	if (unlikely(stop_cpus_in_progress))
			goto unlock;

	err = 0;
	__cpu_stop_queue_work(stopper1, work1);
	__cpu_stop_queue_work(stopper2, work2);
unlock:
	raw_spin_unlock(&stopper2->lock);
	raw_spin_unlock_irq(&stopper1->lock);

	if (unlikely(err == -EDEADLK)) {
		while (stop_cpus_in_progress)
			cpu_relax();
		goto retry;
	}
	return err;
}
>>>>>>> v4.9.227
/**
 * stop_two_cpus - stops two cpus
 * @cpu1: the cpu to stop
 * @cpu2: the other cpu to stop
 * @fn: function to execute
 * @arg: argument to @fn
 *
 * Stops both the current and specified CPU and runs @fn on one of them.
 *
 * returns when both are completed.
 */
int stop_two_cpus(unsigned int cpu1, unsigned int cpu2, cpu_stop_fn_t fn, void *arg)
{
	struct cpu_stop_done done;
	struct cpu_stop_work work1, work2;
<<<<<<< HEAD
	struct irq_cpu_stop_queue_work_info call_args;
	struct multi_stop_data msdata;

	preempt_disable();
=======
	struct multi_stop_data msdata;

>>>>>>> v4.9.227
	msdata = (struct multi_stop_data){
		.fn = fn,
		.data = arg,
		.num_threads = 2,
		.active_cpus = cpumask_of(cpu1),
	};

	work1 = work2 = (struct cpu_stop_work){
		.fn = multi_cpu_stop,
		.arg = &msdata,
		.done = &done
	};

<<<<<<< HEAD
	call_args = (struct irq_cpu_stop_queue_work_info){
		.cpu1 = cpu1,
		.cpu2 = cpu2,
		.work1 = &work1,
		.work2 = &work2,
	};

	cpu_stop_init_done(&done, 2);
	set_state(&msdata, MULTI_STOP_PREPARE);

	/*
	 * If we observe both CPUs active we know _cpu_down() cannot yet have
	 * queued its stop_machine works and therefore ours will get executed
	 * first. Or its not either one of our CPUs that's getting unplugged,
	 * in which case we don't care.
	 *
	 * This relies on the stopper workqueues to be FIFO.
	 */
	if (!cpu_active(cpu1) || !cpu_active(cpu2)) {
		preempt_enable();
		return -ENOENT;
	}

	lg_local_lock(&stop_cpus_lock);
	/*
	 * Queuing needs to be done by the lowest numbered CPU, to ensure
	 * that works are always queued in the same order on every CPU.
	 * This prevents deadlocks.
	 */
	smp_call_function_single(min(cpu1, cpu2),
				 &irq_cpu_stop_queue_work,
				 &call_args, 1);
	lg_local_unlock(&stop_cpus_lock);
	preempt_enable();

	wait_for_completion(&done.completion);

	return done.executed ? done.ret : -ENOENT;
=======
	cpu_stop_init_done(&done, 2);
	set_state(&msdata, MULTI_STOP_PREPARE);

	if (cpu1 > cpu2)
		swap(cpu1, cpu2);
	if (cpu_stop_queue_two_works(cpu1, &work1, cpu2, &work2))
		return -ENOENT;

	wait_for_completion(&done.completion);
	return done.ret;
>>>>>>> v4.9.227
}

/**
 * stop_one_cpu_nowait - stop a cpu but don't wait for completion
 * @cpu: cpu to stop
 * @fn: function to execute
 * @arg: argument to @fn
 * @work_buf: pointer to cpu_stop_work structure
 *
 * Similar to stop_one_cpu() but doesn't wait for completion.  The
 * caller is responsible for ensuring @work_buf is currently unused
 * and will remain untouched until stopper starts executing @fn.
 *
 * CONTEXT:
 * Don't care.
<<<<<<< HEAD
 */
void stop_one_cpu_nowait(unsigned int cpu, cpu_stop_fn_t fn, void *arg,
			struct cpu_stop_work *work_buf)
{
	*work_buf = (struct cpu_stop_work){ .fn = fn, .arg = arg, };
	cpu_stop_queue_work(cpu, work_buf);
}

/* static data for stop_cpus */
static DEFINE_MUTEX(stop_cpus_mutex);
static DEFINE_PER_CPU(struct cpu_stop_work, stop_cpus_work);

static void queue_stop_cpus_work(const struct cpumask *cpumask,
=======
 *
 * RETURNS:
 * true if cpu_stop_work was queued successfully and @fn will be called,
 * false otherwise.
 */
bool stop_one_cpu_nowait(unsigned int cpu, cpu_stop_fn_t fn, void *arg,
			struct cpu_stop_work *work_buf)
{
	*work_buf = (struct cpu_stop_work){ .fn = fn, .arg = arg, };
	return cpu_stop_queue_work(cpu, work_buf);
}

static bool queue_stop_cpus_work(const struct cpumask *cpumask,
>>>>>>> v4.9.227
				 cpu_stop_fn_t fn, void *arg,
				 struct cpu_stop_done *done)
{
	struct cpu_stop_work *work;
	unsigned int cpu;
<<<<<<< HEAD

	/* initialize works and done */
	for_each_cpu(cpu, cpumask) {
		work = &per_cpu(stop_cpus_work, cpu);
		work->fn = fn;
		work->arg = arg;
		work->done = done;
	}
=======
	bool queued = false;
>>>>>>> v4.9.227

	/*
	 * Disable preemption while queueing to avoid getting
	 * preempted by a stopper which might wait for other stoppers
	 * to enter @fn which can lead to deadlock.
	 */
<<<<<<< HEAD
	lg_global_lock(&stop_cpus_lock);
	for_each_cpu(cpu, cpumask)
		cpu_stop_queue_work(cpu, &per_cpu(stop_cpus_work, cpu));
	lg_global_unlock(&stop_cpus_lock);
=======
	preempt_disable();
	stop_cpus_in_progress = true;
	for_each_cpu(cpu, cpumask) {
		work = &per_cpu(cpu_stopper.stop_work, cpu);
		work->fn = fn;
		work->arg = arg;
		work->done = done;
		if (cpu_stop_queue_work(cpu, work))
			queued = true;
	}
	stop_cpus_in_progress = false;
	preempt_enable();

	return queued;
>>>>>>> v4.9.227
}

static int __stop_cpus(const struct cpumask *cpumask,
		       cpu_stop_fn_t fn, void *arg)
{
	struct cpu_stop_done done;

	cpu_stop_init_done(&done, cpumask_weight(cpumask));
<<<<<<< HEAD
	queue_stop_cpus_work(cpumask, fn, arg, &done);
	wait_for_completion(&done.completion);
	return done.executed ? done.ret : -ENOENT;
=======
	if (!queue_stop_cpus_work(cpumask, fn, arg, &done))
		return -ENOENT;
	wait_for_completion(&done.completion);
	return done.ret;
>>>>>>> v4.9.227
}

/**
 * stop_cpus - stop multiple cpus
 * @cpumask: cpus to stop
 * @fn: function to execute
 * @arg: argument to @fn
 *
 * Execute @fn(@arg) on online cpus in @cpumask.  On each target cpu,
 * @fn is run in a process context with the highest priority
 * preempting any task on the cpu and monopolizing it.  This function
 * returns after all executions are complete.
 *
 * This function doesn't guarantee the cpus in @cpumask stay online
 * till @fn completes.  If some cpus go down in the middle, execution
 * on the cpu may happen partially or fully on different cpus.  @fn
 * should either be ready for that or the caller should ensure that
 * the cpus stay online until this function completes.
 *
 * All stop_cpus() calls are serialized making it safe for @fn to wait
 * for all cpus to start executing it.
 *
 * CONTEXT:
 * Might sleep.
 *
 * RETURNS:
 * -ENOENT if @fn(@arg) was not executed at all because all cpus in
 * @cpumask were offline; otherwise, 0 if all executions of @fn
 * returned 0, any non zero return value if any returned non zero.
 */
int stop_cpus(const struct cpumask *cpumask, cpu_stop_fn_t fn, void *arg)
{
	int ret;

	/* static works are used, process one request at a time */
	mutex_lock(&stop_cpus_mutex);
	ret = __stop_cpus(cpumask, fn, arg);
	mutex_unlock(&stop_cpus_mutex);
	return ret;
}

/**
 * try_stop_cpus - try to stop multiple cpus
 * @cpumask: cpus to stop
 * @fn: function to execute
 * @arg: argument to @fn
 *
 * Identical to stop_cpus() except that it fails with -EAGAIN if
 * someone else is already using the facility.
 *
 * CONTEXT:
 * Might sleep.
 *
 * RETURNS:
 * -EAGAIN if someone else is already stopping cpus, -ENOENT if
 * @fn(@arg) was not executed at all because all cpus in @cpumask were
 * offline; otherwise, 0 if all executions of @fn returned 0, any non
 * zero return value if any returned non zero.
 */
int try_stop_cpus(const struct cpumask *cpumask, cpu_stop_fn_t fn, void *arg)
{
	int ret;

	/* static works are used, process one request at a time */
	if (!mutex_trylock(&stop_cpus_mutex))
		return -EAGAIN;
	ret = __stop_cpus(cpumask, fn, arg);
	mutex_unlock(&stop_cpus_mutex);
	return ret;
}

static int cpu_stop_should_run(unsigned int cpu)
{
	struct cpu_stopper *stopper = &per_cpu(cpu_stopper, cpu);
	unsigned long flags;
	int run;

<<<<<<< HEAD
	spin_lock_irqsave(&stopper->lock, flags);
	run = !list_empty(&stopper->works);
	spin_unlock_irqrestore(&stopper->lock, flags);
=======
	raw_spin_lock_irqsave(&stopper->lock, flags);
	run = !list_empty(&stopper->works);
	raw_spin_unlock_irqrestore(&stopper->lock, flags);
>>>>>>> v4.9.227
	return run;
}

static void cpu_stopper_thread(unsigned int cpu)
{
	struct cpu_stopper *stopper = &per_cpu(cpu_stopper, cpu);
	struct cpu_stop_work *work;
<<<<<<< HEAD
	int ret;

repeat:
	work = NULL;
	spin_lock_irq(&stopper->lock);
=======

repeat:
	work = NULL;
	raw_spin_lock_irq(&stopper->lock);
>>>>>>> v4.9.227
	if (!list_empty(&stopper->works)) {
		work = list_first_entry(&stopper->works,
					struct cpu_stop_work, list);
		list_del_init(&work->list);
	}
<<<<<<< HEAD
	spin_unlock_irq(&stopper->lock);
=======
	raw_spin_unlock_irq(&stopper->lock);
>>>>>>> v4.9.227

	if (work) {
		cpu_stop_fn_t fn = work->fn;
		void *arg = work->arg;
		struct cpu_stop_done *done = work->done;
<<<<<<< HEAD
		char ksym_buf[KSYM_NAME_LEN] __maybe_unused;

		/* cpu stop callbacks are not allowed to sleep */
		preempt_disable();

		ret = fn(arg);
		if (ret)
			done->ret = ret;

		/* restore preemption and check it's still balanced */
		preempt_enable();
		WARN_ONCE(preempt_count(),
			  "cpu_stop: %s(%p) leaked preempt count\n",
			  kallsyms_lookup((unsigned long)fn, NULL, NULL, NULL,
					  ksym_buf), arg);

		cpu_stop_signal_done(done, true);
=======
		int ret;

		/* cpu stop callbacks must not sleep, make in_atomic() == T */
		preempt_count_inc();
		ret = fn(arg);
		if (done) {
			if (ret)
				done->ret = ret;
			cpu_stop_signal_done(done);
		}
		preempt_count_dec();
		WARN_ONCE(preempt_count(),
			  "cpu_stop: %pf(%p) leaked preempt count\n", fn, arg);
>>>>>>> v4.9.227
		goto repeat;
	}
}

<<<<<<< HEAD
=======
void stop_machine_park(int cpu)
{
	struct cpu_stopper *stopper = &per_cpu(cpu_stopper, cpu);
	/*
	 * Lockless. cpu_stopper_thread() will take stopper->lock and flush
	 * the pending works before it parks, until then it is fine to queue
	 * the new works.
	 */
	stopper->enabled = false;
	kthread_park(stopper->thread);
}

>>>>>>> v4.9.227
extern void sched_set_stop_task(int cpu, struct task_struct *stop);

static void cpu_stop_create(unsigned int cpu)
{
<<<<<<< HEAD
	sched_set_stop_task(cpu, per_cpu(cpu_stopper_task, cpu));
=======
	sched_set_stop_task(cpu, per_cpu(cpu_stopper.thread, cpu));
>>>>>>> v4.9.227
}

static void cpu_stop_park(unsigned int cpu)
{
	struct cpu_stopper *stopper = &per_cpu(cpu_stopper, cpu);
<<<<<<< HEAD
	struct cpu_stop_work *work;
	unsigned long flags;

	/* drain remaining works */
	spin_lock_irqsave(&stopper->lock, flags);
	list_for_each_entry(work, &stopper->works, list)
		cpu_stop_signal_done(work->done, false);
	stopper->enabled = false;
	spin_unlock_irqrestore(&stopper->lock, flags);
}

static void cpu_stop_unpark(unsigned int cpu)
{
	struct cpu_stopper *stopper = &per_cpu(cpu_stopper, cpu);

	spin_lock_irq(&stopper->lock);
	stopper->enabled = true;
	spin_unlock_irq(&stopper->lock);
}

static struct smp_hotplug_thread cpu_stop_threads = {
	.store			= &cpu_stopper_task,
=======

	WARN_ON(!list_empty(&stopper->works));
}

void stop_machine_unpark(int cpu)
{
	struct cpu_stopper *stopper = &per_cpu(cpu_stopper, cpu);

	stopper->enabled = true;
	kthread_unpark(stopper->thread);
}

static struct smp_hotplug_thread cpu_stop_threads = {
	.store			= &cpu_stopper.thread,
>>>>>>> v4.9.227
	.thread_should_run	= cpu_stop_should_run,
	.thread_fn		= cpu_stopper_thread,
	.thread_comm		= "migration/%u",
	.create			= cpu_stop_create,
<<<<<<< HEAD
	.setup			= cpu_stop_unpark,
	.park			= cpu_stop_park,
	.pre_unpark		= cpu_stop_unpark,
=======
	.park			= cpu_stop_park,
>>>>>>> v4.9.227
	.selfparking		= true,
};

static int __init cpu_stop_init(void)
{
	unsigned int cpu;

	for_each_possible_cpu(cpu) {
		struct cpu_stopper *stopper = &per_cpu(cpu_stopper, cpu);

<<<<<<< HEAD
		spin_lock_init(&stopper->lock);
=======
		raw_spin_lock_init(&stopper->lock);
>>>>>>> v4.9.227
		INIT_LIST_HEAD(&stopper->works);
	}

	BUG_ON(smpboot_register_percpu_thread(&cpu_stop_threads));
<<<<<<< HEAD
=======
	stop_machine_unpark(raw_smp_processor_id());
>>>>>>> v4.9.227
	stop_machine_initialized = true;
	return 0;
}
early_initcall(cpu_stop_init);

<<<<<<< HEAD
#ifdef CONFIG_STOP_MACHINE

int __stop_machine(int (*fn)(void *), void *data, const struct cpumask *cpus)
=======
static int __stop_machine(cpu_stop_fn_t fn, void *data, const struct cpumask *cpus)
>>>>>>> v4.9.227
{
	struct multi_stop_data msdata = {
		.fn = fn,
		.data = data,
		.num_threads = num_online_cpus(),
		.active_cpus = cpus,
	};

	if (!stop_machine_initialized) {
		/*
		 * Handle the case where stop_machine() is called
		 * early in boot before stop_machine() has been
		 * initialized.
		 */
		unsigned long flags;
		int ret;

		WARN_ON_ONCE(msdata.num_threads != 1);

		local_irq_save(flags);
		hard_irq_disable();
		ret = (*fn)(data);
		local_irq_restore(flags);

		return ret;
	}

	/* Set the initial state and stop all online cpus. */
	set_state(&msdata, MULTI_STOP_PREPARE);
	return stop_cpus(cpu_online_mask, multi_cpu_stop, &msdata);
}

<<<<<<< HEAD
int stop_machine(int (*fn)(void *), void *data, const struct cpumask *cpus)
=======
int stop_machine(cpu_stop_fn_t fn, void *data, const struct cpumask *cpus)
>>>>>>> v4.9.227
{
	int ret;

	/* No CPUs can come up or down during this. */
	get_online_cpus();
	ret = __stop_machine(fn, data, cpus);
	put_online_cpus();
	return ret;
}
EXPORT_SYMBOL_GPL(stop_machine);

/**
 * stop_machine_from_inactive_cpu - stop_machine() from inactive CPU
 * @fn: the function to run
 * @data: the data ptr for the @fn()
 * @cpus: the cpus to run the @fn() on (NULL = any online cpu)
 *
 * This is identical to stop_machine() but can be called from a CPU which
 * is not active.  The local CPU is in the process of hotplug (so no other
 * CPU hotplug can start) and not marked active and doesn't have enough
 * context to sleep.
 *
 * This function provides stop_machine() functionality for such state by
 * using busy-wait for synchronization and executing @fn directly for local
 * CPU.
 *
 * CONTEXT:
 * Local CPU is inactive.  Temporarily stops all active CPUs.
 *
 * RETURNS:
 * 0 if all executions of @fn returned 0, any non zero return value if any
 * returned non zero.
 */
<<<<<<< HEAD
int stop_machine_from_inactive_cpu(int (*fn)(void *), void *data,
=======
int stop_machine_from_inactive_cpu(cpu_stop_fn_t fn, void *data,
>>>>>>> v4.9.227
				  const struct cpumask *cpus)
{
	struct multi_stop_data msdata = { .fn = fn, .data = data,
					    .active_cpus = cpus };
	struct cpu_stop_done done;
	int ret;

	/* Local CPU must be inactive and CPU hotplug in progress. */
	BUG_ON(cpu_active(raw_smp_processor_id()));
	msdata.num_threads = num_active_cpus() + 1;	/* +1 for local */

	/* No proper task established and can't sleep - busy wait for lock. */
	while (!mutex_trylock(&stop_cpus_mutex))
		cpu_relax();

	/* Schedule work on other CPUs and execute directly for local CPU */
	set_state(&msdata, MULTI_STOP_PREPARE);
	cpu_stop_init_done(&done, num_active_cpus());
	queue_stop_cpus_work(cpu_active_mask, multi_cpu_stop, &msdata,
			     &done);
	ret = multi_cpu_stop(&msdata);

	/* Busy wait for completion. */
	while (!completion_done(&done.completion))
		cpu_relax();

	mutex_unlock(&stop_cpus_mutex);
	return ret ?: done.ret;
}
<<<<<<< HEAD

#endif	/* CONFIG_STOP_MACHINE */
=======
>>>>>>> v4.9.227
