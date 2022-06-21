/*
 * drivers/cpufreq/cpufreq_governor.c
 *
 * CPUFREQ governors common code
 *
 * Copyright	(C) 2001 Russell King
 *		(C) 2003 Venkatesh Pallipadi <venkatesh.pallipadi@intel.com>.
 *		(C) 2003 Jun Nakajima <jun.nakajima@intel.com>
 *		(C) 2009 Alexander Clouter <alex@digriz.org.uk>
 *		(c) 2012 Viresh Kumar <viresh.kumar@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/export.h>
#include <linux/kernel_stat.h>
<<<<<<< HEAD
=======
#include <linux/sched.h>
>>>>>>> v4.9.227
#include <linux/slab.h>

#include "cpufreq_governor.h"

<<<<<<< HEAD
static struct attribute_group *get_sysfs_attr(struct dbs_data *dbs_data)
{
	if (have_governor_per_policy())
		return dbs_data->cdata->attr_group_gov_pol;
	else
		return dbs_data->cdata->attr_group_gov_sys;
}

void dbs_check_cpu(struct dbs_data *dbs_data, int cpu)
{
	struct cpu_dbs_common_info *cdbs = dbs_data->cdata->get_cpu_cdbs(cpu);
	struct od_dbs_tuners *od_tuners = dbs_data->tuners;
	struct cs_dbs_tuners *cs_tuners = dbs_data->tuners;
	struct cpufreq_policy *policy;
	unsigned int sampling_rate;
	unsigned int max_load = 0;
	unsigned int ignore_nice;
	unsigned int j;

	if (dbs_data->cdata->governor == GOV_ONDEMAND) {
		struct od_cpu_dbs_info_s *od_dbs_info =
				dbs_data->cdata->get_cpu_dbs_info_s(cpu);

		/*
		 * Sometimes, the ondemand governor uses an additional
		 * multiplier to give long delays. So apply this multiplier to
		 * the 'sampling_rate', so as to keep the wake-up-from-idle
		 * detection logic a bit conservative.
		 */
		sampling_rate = od_tuners->sampling_rate;
		sampling_rate *= od_dbs_info->rate_mult;

		ignore_nice = od_tuners->ignore_nice_load;
	} else {
		sampling_rate = cs_tuners->sampling_rate;
		ignore_nice = cs_tuners->ignore_nice_load;
	}

	policy = cdbs->cur_policy;

	/* Get Absolute Load */
	for_each_cpu(j, policy->cpus) {
		struct cpu_dbs_common_info *j_cdbs;
		u64 cur_wall_time, cur_idle_time;
		unsigned int idle_time, wall_time;
		unsigned int load;
		int io_busy = 0;

		j_cdbs = dbs_data->cdata->get_cpu_cdbs(j);

		/*
		 * For the purpose of ondemand, waiting for disk IO is
		 * an indication that you're performance critical, and
		 * not that the system is actually idle. So do not add
		 * the iowait time to the cpu idle time.
		 */
		if (dbs_data->cdata->governor == GOV_ONDEMAND)
			io_busy = od_tuners->io_is_busy;
		cur_idle_time = get_cpu_idle_time(j, &cur_wall_time, io_busy);

		wall_time = (unsigned int)
			(cur_wall_time - j_cdbs->prev_cpu_wall);
		j_cdbs->prev_cpu_wall = cur_wall_time;

		idle_time = (unsigned int)
			(cur_idle_time - j_cdbs->prev_cpu_idle);
		j_cdbs->prev_cpu_idle = cur_idle_time;

		if (ignore_nice) {
			u64 cur_nice;
			unsigned long cur_nice_jiffies;

			cur_nice = kcpustat_cpu(j).cpustat[CPUTIME_NICE] -
					 cdbs->prev_cpu_nice;
			/*
			 * Assumption: nice time between sampling periods will
			 * be less than 2^32 jiffies for 32 bit sys
			 */
			cur_nice_jiffies = (unsigned long)
					cputime64_to_jiffies64(cur_nice);

			cdbs->prev_cpu_nice =
				kcpustat_cpu(j).cpustat[CPUTIME_NICE];
			idle_time += jiffies_to_usecs(cur_nice_jiffies);
		}

		if (unlikely(!wall_time || wall_time < idle_time))
			continue;

		/*
		 * If the CPU had gone completely idle, and a task just woke up
		 * on this CPU now, it would be unfair to calculate 'load' the
		 * usual way for this elapsed time-window, because it will show
		 * near-zero load, irrespective of how CPU intensive that task
		 * actually is. This is undesirable for latency-sensitive bursty
		 * workloads.
		 *
		 * To avoid this, we reuse the 'load' from the previous
		 * time-window and give this task a chance to start with a
		 * reasonably high CPU frequency. (However, we shouldn't over-do
		 * this copy, lest we get stuck at a high load (high frequency)
		 * for too long, even when the current system load has actually
		 * dropped down. So we perform the copy only once, upon the
		 * first wake-up from idle.)
		 *
		 * Detecting this situation is easy: the governor's deferrable
		 * timer would not have fired during CPU-idle periods. Hence
		 * an unusually large 'wall_time' (as compared to the sampling
		 * rate) indicates this scenario.
		 *
		 * prev_load can be zero in two cases and we must recalculate it
		 * for both cases:
		 * - during long idle intervals
		 * - explicitly set to zero
		 */
		if (unlikely(wall_time > (2 * sampling_rate) &&
			     j_cdbs->prev_load)) {
			load = j_cdbs->prev_load;

			/*
			 * Perform a destructive copy, to ensure that we copy
			 * the previous load only once, upon the first wake-up
			 * from idle.
			 */
			j_cdbs->prev_load = 0;
		} else {
			load = 100 * (wall_time - idle_time) / wall_time;
=======
static DEFINE_PER_CPU(struct cpu_dbs_info, cpu_dbs);

static DEFINE_MUTEX(gov_dbs_data_mutex);

/* Common sysfs tunables */
/**
 * store_sampling_rate - update sampling rate effective immediately if needed.
 *
 * If new rate is smaller than the old, simply updating
 * dbs.sampling_rate might not be appropriate. For example, if the
 * original sampling_rate was 1 second and the requested new sampling rate is 10
 * ms because the user needs immediate reaction from ondemand governor, but not
 * sure if higher frequency will be required or not, then, the governor may
 * change the sampling rate too late; up to 1 second later. Thus, if we are
 * reducing the sampling rate, we need to make the new value effective
 * immediately.
 *
 * This must be called with dbs_data->mutex held, otherwise traversing
 * policy_dbs_list isn't safe.
 */
ssize_t store_sampling_rate(struct gov_attr_set *attr_set, const char *buf,
			    size_t count)
{
	struct dbs_data *dbs_data = to_dbs_data(attr_set);
	struct policy_dbs_info *policy_dbs;
	unsigned int rate;
	int ret;
	ret = sscanf(buf, "%u", &rate);
	if (ret != 1)
		return -EINVAL;

	dbs_data->sampling_rate = max(rate, dbs_data->min_sampling_rate);

	/*
	 * We are operating under dbs_data->mutex and so the list and its
	 * entries can't be freed concurrently.
	 */
	list_for_each_entry(policy_dbs, &attr_set->policy_list, list) {
		mutex_lock(&policy_dbs->timer_mutex);
		/*
		 * On 32-bit architectures this may race with the
		 * sample_delay_ns read in dbs_update_util_handler(), but that
		 * really doesn't matter.  If the read returns a value that's
		 * too big, the sample will be skipped, but the next invocation
		 * of dbs_update_util_handler() (when the update has been
		 * completed) will take a sample.
		 *
		 * If this runs in parallel with dbs_work_handler(), we may end
		 * up overwriting the sample_delay_ns value that it has just
		 * written, but it will be corrected next time a sample is
		 * taken, so it shouldn't be significant.
		 */
		gov_update_sample_delay(policy_dbs, 0);
		mutex_unlock(&policy_dbs->timer_mutex);
	}

	return count;
}
EXPORT_SYMBOL_GPL(store_sampling_rate);

/**
 * gov_update_cpu_data - Update CPU load data.
 * @dbs_data: Top-level governor data pointer.
 *
 * Update CPU load data for all CPUs in the domain governed by @dbs_data
 * (that may be a single policy or a bunch of them if governor tunables are
 * system-wide).
 *
 * Call under the @dbs_data mutex.
 */
void gov_update_cpu_data(struct dbs_data *dbs_data)
{
	struct policy_dbs_info *policy_dbs;

	list_for_each_entry(policy_dbs, &dbs_data->attr_set.policy_list, list) {
		unsigned int j;

		for_each_cpu(j, policy_dbs->policy->cpus) {
			struct cpu_dbs_info *j_cdbs = &per_cpu(cpu_dbs, j);

			j_cdbs->prev_cpu_idle = get_cpu_idle_time(j, &j_cdbs->prev_update_time,
								  dbs_data->io_is_busy);
			if (dbs_data->ignore_nice_load)
				j_cdbs->prev_cpu_nice = kcpustat_cpu(j).cpustat[CPUTIME_NICE];
		}
	}
}
EXPORT_SYMBOL_GPL(gov_update_cpu_data);

unsigned int dbs_update(struct cpufreq_policy *policy)
{
	struct policy_dbs_info *policy_dbs = policy->governor_data;
	struct dbs_data *dbs_data = policy_dbs->dbs_data;
	unsigned int ignore_nice = dbs_data->ignore_nice_load;
	unsigned int max_load = 0;
	unsigned int sampling_rate, io_busy, j;

	/*
	 * Sometimes governors may use an additional multiplier to increase
	 * sample delays temporarily.  Apply that multiplier to sampling_rate
	 * so as to keep the wake-up-from-idle detection logic a bit
	 * conservative.
	 */
	sampling_rate = dbs_data->sampling_rate * policy_dbs->rate_mult;
	/*
	 * For the purpose of ondemand, waiting for disk IO is an indication
	 * that you're performance critical, and not that the system is actually
	 * idle, so do not add the iowait time to the CPU idle time then.
	 */
	io_busy = dbs_data->io_is_busy;

	/* Get Absolute Load */
	for_each_cpu(j, policy->cpus) {
		struct cpu_dbs_info *j_cdbs = &per_cpu(cpu_dbs, j);
		u64 update_time, cur_idle_time;
		unsigned int idle_time, time_elapsed;
		unsigned int load;

		cur_idle_time = get_cpu_idle_time(j, &update_time, io_busy);

		time_elapsed = update_time - j_cdbs->prev_update_time;
		j_cdbs->prev_update_time = update_time;

		idle_time = cur_idle_time - j_cdbs->prev_cpu_idle;
		j_cdbs->prev_cpu_idle = cur_idle_time;

		if (ignore_nice) {
			u64 cur_nice = kcpustat_cpu(j).cpustat[CPUTIME_NICE];

			idle_time += div_u64(cur_nice - j_cdbs->prev_cpu_nice, NSEC_PER_USEC);
			j_cdbs->prev_cpu_nice = cur_nice;
		}

		if (unlikely(!time_elapsed)) {
			/*
			 * That can only happen when this function is called
			 * twice in a row with a very short interval between the
			 * calls, so the previous load value can be used then.
			 */
			load = j_cdbs->prev_load;
		} else if (unlikely(time_elapsed > 2 * sampling_rate &&
				    j_cdbs->prev_load)) {
			/*
			 * If the CPU had gone completely idle and a task has
			 * just woken up on this CPU now, it would be unfair to
			 * calculate 'load' the usual way for this elapsed
			 * time-window, because it would show near-zero load,
			 * irrespective of how CPU intensive that task actually
			 * was. This is undesirable for latency-sensitive bursty
			 * workloads.
			 *
			 * To avoid this, reuse the 'load' from the previous
			 * time-window and give this task a chance to start with
			 * a reasonably high CPU frequency. However, that
			 * shouldn't be over-done, lest we get stuck at a high
			 * load (high frequency) for too long, even when the
			 * current system load has actually dropped down, so
			 * clear prev_load to guarantee that the load will be
			 * computed again next time.
			 *
			 * Detecting this situation is easy: the governor's
			 * utilization update handler would not have run during
			 * CPU-idle periods.  Hence, an unusually large
			 * 'time_elapsed' (as compared to the sampling rate)
			 * indicates this scenario.
			 */
			load = j_cdbs->prev_load;
			j_cdbs->prev_load = 0;
		} else {
			if (time_elapsed >= idle_time) {
				load = 100 * (time_elapsed - idle_time) / time_elapsed;
			} else {
				/*
				 * That can happen if idle_time is returned by
				 * get_cpu_idle_time_jiffy().  In that case
				 * idle_time is roughly equal to the difference
				 * between time_elapsed and "busy time" obtained
				 * from CPU statistics.  Then, the "busy time"
				 * can end up being greater than time_elapsed
				 * (for example, if jiffies_64 and the CPU
				 * statistics are updated by different CPUs),
				 * so idle_time may in fact be negative.  That
				 * means, though, that the CPU was busy all
				 * the time (on the rough average) during the
				 * last sampling interval and 100 can be
				 * returned as the load.
				 */
				load = (int)idle_time < 0 ? 100 : 0;
			}
>>>>>>> v4.9.227
			j_cdbs->prev_load = load;
		}

		if (load > max_load)
			max_load = load;
	}
<<<<<<< HEAD

	dbs_data->cdata->gov_check_cpu(cpu, max_load);
}
EXPORT_SYMBOL_GPL(dbs_check_cpu);

static inline void __gov_queue_work(int cpu, struct dbs_data *dbs_data,
		unsigned int delay)
{
	struct cpu_dbs_common_info *cdbs = dbs_data->cdata->get_cpu_cdbs(cpu);

	mod_delayed_work_on(cpu, system_wq, &cdbs->work, delay);
}

void gov_queue_work(struct dbs_data *dbs_data, struct cpufreq_policy *policy,
		unsigned int delay, bool all_cpus)
{
	int i;

	mutex_lock(&cpufreq_governor_lock);
	if (!policy->governor_enabled)
		goto out_unlock;

	if (!all_cpus) {
		/*
		 * Use raw_smp_processor_id() to avoid preemptible warnings.
		 * We know that this is only called with all_cpus == false from
		 * works that have been queued with *_work_on() functions and
		 * those works are canceled during CPU_DOWN_PREPARE so they
		 * can't possibly run on any other CPU.
		 */
		__gov_queue_work(raw_smp_processor_id(), dbs_data, delay);
	} else {
		for_each_cpu(i, policy->cpus)
			__gov_queue_work(i, dbs_data, delay);
	}

out_unlock:
	mutex_unlock(&cpufreq_governor_lock);
}
EXPORT_SYMBOL_GPL(gov_queue_work);

static inline void gov_cancel_work(struct dbs_data *dbs_data,
		struct cpufreq_policy *policy)
{
	struct cpu_dbs_common_info *cdbs;
	int i;

	for_each_cpu(i, policy->cpus) {
		cdbs = dbs_data->cdata->get_cpu_cdbs(i);
		cancel_delayed_work_sync(&cdbs->work);
	}
}

/* Will return if we need to evaluate cpu load again or not */
bool need_load_eval(struct cpu_dbs_common_info *cdbs,
		unsigned int sampling_rate)
{
	if (policy_is_shared(cdbs->cur_policy)) {
		ktime_t time_now = ktime_get();
		s64 delta_us = ktime_us_delta(time_now, cdbs->time_stamp);

		/* Do nothing if we recently have sampled */
		if (delta_us < (s64)(sampling_rate / 2))
			return false;
		else
			cdbs->time_stamp = time_now;
	}

	return true;
}
EXPORT_SYMBOL_GPL(need_load_eval);

static void set_sampling_rate(struct dbs_data *dbs_data,
		unsigned int sampling_rate)
{
	if (dbs_data->cdata->governor == GOV_CONSERVATIVE) {
		struct cs_dbs_tuners *cs_tuners = dbs_data->tuners;
		cs_tuners->sampling_rate = sampling_rate;
	} else {
		struct od_dbs_tuners *od_tuners = dbs_data->tuners;
		od_tuners->sampling_rate = sampling_rate;
	}
}

int cpufreq_governor_dbs(struct cpufreq_policy *policy,
		struct common_dbs_data *cdata, unsigned int event)
{
	struct dbs_data *dbs_data;
	struct od_cpu_dbs_info_s *od_dbs_info = NULL;
	struct cs_cpu_dbs_info_s *cs_dbs_info = NULL;
	struct od_ops *od_ops = NULL;
	struct od_dbs_tuners *od_tuners = NULL;
	struct cs_dbs_tuners *cs_tuners = NULL;
	struct cpu_dbs_common_info *cpu_cdbs;
	unsigned int sampling_rate, latency, ignore_nice, j, cpu = policy->cpu;
	int io_busy = 0;
	int rc;

	if (have_governor_per_policy())
		dbs_data = policy->governor_data;
	else
		dbs_data = cdata->gdbs_data;

	WARN_ON(!dbs_data && (event != CPUFREQ_GOV_POLICY_INIT));

	switch (event) {
	case CPUFREQ_GOV_POLICY_INIT:
		if (have_governor_per_policy()) {
			WARN_ON(dbs_data);
		} else if (dbs_data) {
			dbs_data->usage_count++;
			policy->governor_data = dbs_data;
			return 0;
		}

		dbs_data = kzalloc(sizeof(*dbs_data), GFP_KERNEL);
		if (!dbs_data) {
			pr_err("%s: POLICY_INIT: kzalloc failed\n", __func__);
			return -ENOMEM;
		}

		dbs_data->cdata = cdata;
		dbs_data->usage_count = 1;
		rc = cdata->init(dbs_data);
		if (rc) {
			pr_err("%s: POLICY_INIT: init() failed\n", __func__);
			kfree(dbs_data);
			return rc;
		}

		if (!have_governor_per_policy())
			WARN_ON(cpufreq_get_global_kobject());

		rc = sysfs_create_group(get_governor_parent_kobj(policy),
				get_sysfs_attr(dbs_data));
		if (rc) {
			cdata->exit(dbs_data);
			kfree(dbs_data);
			return rc;
		}

		policy->governor_data = dbs_data;

		/* policy latency is in ns. Convert it to us first */
		latency = policy->cpuinfo.transition_latency / 1000;
		if (latency == 0)
			latency = 1;

		/* Bring kernel and HW constraints together */
		dbs_data->min_sampling_rate = max(dbs_data->min_sampling_rate,
				MIN_LATENCY_MULTIPLIER * latency);
		set_sampling_rate(dbs_data, max(dbs_data->min_sampling_rate,
					latency * LATENCY_MULTIPLIER));

		if ((cdata->governor == GOV_CONSERVATIVE) &&
				(!policy->governor->initialized)) {
			struct cs_ops *cs_ops = dbs_data->cdata->gov_ops;

			cpufreq_register_notifier(cs_ops->notifier_block,
					CPUFREQ_TRANSITION_NOTIFIER);
		}

		if (!have_governor_per_policy())
			cdata->gdbs_data = dbs_data;

		return 0;
	case CPUFREQ_GOV_POLICY_EXIT:
		if (!--dbs_data->usage_count) {
			sysfs_remove_group(get_governor_parent_kobj(policy),
					get_sysfs_attr(dbs_data));

			if (!have_governor_per_policy())
				cpufreq_put_global_kobject();

			if ((dbs_data->cdata->governor == GOV_CONSERVATIVE) &&
				(policy->governor->initialized == 1)) {
				struct cs_ops *cs_ops = dbs_data->cdata->gov_ops;

				cpufreq_unregister_notifier(cs_ops->notifier_block,
						CPUFREQ_TRANSITION_NOTIFIER);
			}

			cdata->exit(dbs_data);
			kfree(dbs_data);
			cdata->gdbs_data = NULL;
		}

		policy->governor_data = NULL;
		return 0;
	}

	cpu_cdbs = dbs_data->cdata->get_cpu_cdbs(cpu);

	if (dbs_data->cdata->governor == GOV_CONSERVATIVE) {
		cs_tuners = dbs_data->tuners;
		cs_dbs_info = dbs_data->cdata->get_cpu_dbs_info_s(cpu);
		sampling_rate = cs_tuners->sampling_rate;
		ignore_nice = cs_tuners->ignore_nice_load;
	} else {
		od_tuners = dbs_data->tuners;
		od_dbs_info = dbs_data->cdata->get_cpu_dbs_info_s(cpu);
		sampling_rate = od_tuners->sampling_rate;
		ignore_nice = od_tuners->ignore_nice_load;
		od_ops = dbs_data->cdata->gov_ops;
		io_busy = od_tuners->io_is_busy;
	}

	switch (event) {
	case CPUFREQ_GOV_START:
		if (!policy->cur)
			return -EINVAL;

		mutex_lock(&dbs_data->mutex);

		for_each_cpu(j, policy->cpus) {
			struct cpu_dbs_common_info *j_cdbs =
				dbs_data->cdata->get_cpu_cdbs(j);
			unsigned int prev_load;

			j_cdbs->cpu = j;
			j_cdbs->cur_policy = policy;
			j_cdbs->prev_cpu_idle = get_cpu_idle_time(j,
					       &j_cdbs->prev_cpu_wall, io_busy);

			prev_load = (unsigned int)
				(j_cdbs->prev_cpu_wall - j_cdbs->prev_cpu_idle);
			j_cdbs->prev_load = 100 * prev_load /
					(unsigned int) j_cdbs->prev_cpu_wall;

			if (ignore_nice)
				j_cdbs->prev_cpu_nice =
					kcpustat_cpu(j).cpustat[CPUTIME_NICE];

			mutex_init(&j_cdbs->timer_mutex);
			INIT_DEFERRABLE_WORK(&j_cdbs->work,
					     dbs_data->cdata->gov_dbs_timer);
		}

		if (dbs_data->cdata->governor == GOV_CONSERVATIVE) {
			cs_dbs_info->down_skip = 0;
			cs_dbs_info->enable = 1;
			cs_dbs_info->requested_freq = policy->cur;
		} else {
			od_dbs_info->rate_mult = 1;
			od_dbs_info->sample_type = OD_NORMAL_SAMPLE;
			od_ops->powersave_bias_init_cpu(cpu);
		}

		mutex_unlock(&dbs_data->mutex);

		/* Initiate timer time stamp */
		cpu_cdbs->time_stamp = ktime_get();

		gov_queue_work(dbs_data, policy,
				delay_for_sampling_rate(sampling_rate), true);
		break;

	case CPUFREQ_GOV_STOP:
		if (dbs_data->cdata->governor == GOV_CONSERVATIVE)
			cs_dbs_info->enable = 0;

		gov_cancel_work(dbs_data, policy);

		mutex_lock(&dbs_data->mutex);
		mutex_destroy(&cpu_cdbs->timer_mutex);
		cpu_cdbs->cur_policy = NULL;

		mutex_unlock(&dbs_data->mutex);

		break;

	case CPUFREQ_GOV_LIMITS:
		mutex_lock(&dbs_data->mutex);
		if (!cpu_cdbs->cur_policy) {
			mutex_unlock(&dbs_data->mutex);
			break;
		}
		mutex_lock(&cpu_cdbs->timer_mutex);
		if (policy->max < cpu_cdbs->cur_policy->cur)
			__cpufreq_driver_target(cpu_cdbs->cur_policy,
					policy->max, CPUFREQ_RELATION_H);
		else if (policy->min > cpu_cdbs->cur_policy->cur)
			__cpufreq_driver_target(cpu_cdbs->cur_policy,
					policy->min, CPUFREQ_RELATION_L);
		dbs_check_cpu(dbs_data, cpu);
		mutex_unlock(&cpu_cdbs->timer_mutex);
		mutex_unlock(&dbs_data->mutex);
		break;
	}
	return 0;
}
EXPORT_SYMBOL_GPL(cpufreq_governor_dbs);
=======
	return max_load;
}
EXPORT_SYMBOL_GPL(dbs_update);

static void dbs_work_handler(struct work_struct *work)
{
	struct policy_dbs_info *policy_dbs;
	struct cpufreq_policy *policy;
	struct dbs_governor *gov;

	policy_dbs = container_of(work, struct policy_dbs_info, work);
	policy = policy_dbs->policy;
	gov = dbs_governor_of(policy);

	/*
	 * Make sure cpufreq_governor_limits() isn't evaluating load or the
	 * ondemand governor isn't updating the sampling rate in parallel.
	 */
	mutex_lock(&policy_dbs->timer_mutex);
	gov_update_sample_delay(policy_dbs, gov->gov_dbs_timer(policy));
	mutex_unlock(&policy_dbs->timer_mutex);

	/* Allow the utilization update handler to queue up more work. */
	atomic_set(&policy_dbs->work_count, 0);
	/*
	 * If the update below is reordered with respect to the sample delay
	 * modification, the utilization update handler may end up using a stale
	 * sample delay value.
	 */
	smp_wmb();
	policy_dbs->work_in_progress = false;
}

static void dbs_irq_work(struct irq_work *irq_work)
{
	struct policy_dbs_info *policy_dbs;

	policy_dbs = container_of(irq_work, struct policy_dbs_info, irq_work);
	schedule_work_on(smp_processor_id(), &policy_dbs->work);
}

static void dbs_update_util_handler(struct update_util_data *data, u64 time,
				    unsigned int flags)
{
	struct cpu_dbs_info *cdbs = container_of(data, struct cpu_dbs_info, update_util);
	struct policy_dbs_info *policy_dbs = cdbs->policy_dbs;
	u64 delta_ns, lst;

	/*
	 * The work may not be allowed to be queued up right now.
	 * Possible reasons:
	 * - Work has already been queued up or is in progress.
	 * - It is too early (too little time from the previous sample).
	 */
	if (policy_dbs->work_in_progress)
		return;

	/*
	 * If the reads below are reordered before the check above, the value
	 * of sample_delay_ns used in the computation may be stale.
	 */
	smp_rmb();
	lst = READ_ONCE(policy_dbs->last_sample_time);
	delta_ns = time - lst;
	if ((s64)delta_ns < policy_dbs->sample_delay_ns)
		return;

	/*
	 * If the policy is not shared, the irq_work may be queued up right away
	 * at this point.  Otherwise, we need to ensure that only one of the
	 * CPUs sharing the policy will do that.
	 */
	if (policy_dbs->is_shared) {
		if (!atomic_add_unless(&policy_dbs->work_count, 1, 1))
			return;

		/*
		 * If another CPU updated last_sample_time in the meantime, we
		 * shouldn't be here, so clear the work counter and bail out.
		 */
		if (unlikely(lst != READ_ONCE(policy_dbs->last_sample_time))) {
			atomic_set(&policy_dbs->work_count, 0);
			return;
		}
	}

	policy_dbs->last_sample_time = time;
	policy_dbs->work_in_progress = true;
	irq_work_queue(&policy_dbs->irq_work);
}

static void gov_set_update_util(struct policy_dbs_info *policy_dbs,
				unsigned int delay_us)
{
	struct cpufreq_policy *policy = policy_dbs->policy;
	int cpu;

	gov_update_sample_delay(policy_dbs, delay_us);
	policy_dbs->last_sample_time = 0;

	for_each_cpu(cpu, policy->cpus) {
		struct cpu_dbs_info *cdbs = &per_cpu(cpu_dbs, cpu);

		cpufreq_add_update_util_hook(cpu, &cdbs->update_util,
					     dbs_update_util_handler);
	}
}

static inline void gov_clear_update_util(struct cpufreq_policy *policy)
{
	int i;

	for_each_cpu(i, policy->cpus)
		cpufreq_remove_update_util_hook(i);

	synchronize_sched();
}

static struct policy_dbs_info *alloc_policy_dbs_info(struct cpufreq_policy *policy,
						     struct dbs_governor *gov)
{
	struct policy_dbs_info *policy_dbs;
	int j;

	/* Allocate memory for per-policy governor data. */
	policy_dbs = gov->alloc();
	if (!policy_dbs)
		return NULL;

	policy_dbs->policy = policy;
	mutex_init(&policy_dbs->timer_mutex);
	atomic_set(&policy_dbs->work_count, 0);
	init_irq_work(&policy_dbs->irq_work, dbs_irq_work);
	INIT_WORK(&policy_dbs->work, dbs_work_handler);

	/* Set policy_dbs for all CPUs, online+offline */
	for_each_cpu(j, policy->related_cpus) {
		struct cpu_dbs_info *j_cdbs = &per_cpu(cpu_dbs, j);

		j_cdbs->policy_dbs = policy_dbs;
	}
	return policy_dbs;
}

static void free_policy_dbs_info(struct policy_dbs_info *policy_dbs,
				 struct dbs_governor *gov)
{
	int j;

	mutex_destroy(&policy_dbs->timer_mutex);

	for_each_cpu(j, policy_dbs->policy->related_cpus) {
		struct cpu_dbs_info *j_cdbs = &per_cpu(cpu_dbs, j);

		j_cdbs->policy_dbs = NULL;
		j_cdbs->update_util.func = NULL;
	}
	gov->free(policy_dbs);
}

int cpufreq_dbs_governor_init(struct cpufreq_policy *policy)
{
	struct dbs_governor *gov = dbs_governor_of(policy);
	struct dbs_data *dbs_data;
	struct policy_dbs_info *policy_dbs;
	unsigned int latency;
	int ret = 0;

	/* State should be equivalent to EXIT */
	if (policy->governor_data)
		return -EBUSY;

	policy_dbs = alloc_policy_dbs_info(policy, gov);
	if (!policy_dbs)
		return -ENOMEM;

	/* Protect gov->gdbs_data against concurrent updates. */
	mutex_lock(&gov_dbs_data_mutex);

	dbs_data = gov->gdbs_data;
	if (dbs_data) {
		if (WARN_ON(have_governor_per_policy())) {
			ret = -EINVAL;
			goto free_policy_dbs_info;
		}
		policy_dbs->dbs_data = dbs_data;
		policy->governor_data = policy_dbs;

		gov_attr_set_get(&dbs_data->attr_set, &policy_dbs->list);
		goto out;
	}

	dbs_data = kzalloc(sizeof(*dbs_data), GFP_KERNEL);
	if (!dbs_data) {
		ret = -ENOMEM;
		goto free_policy_dbs_info;
	}

	gov_attr_set_init(&dbs_data->attr_set, &policy_dbs->list);

	ret = gov->init(dbs_data);
	if (ret)
		goto free_policy_dbs_info;

	/* policy latency is in ns. Convert it to us first */
	latency = policy->cpuinfo.transition_latency / 1000;
	if (latency == 0)
		latency = 1;

	/* Bring kernel and HW constraints together */
	dbs_data->min_sampling_rate = max(dbs_data->min_sampling_rate,
					  MIN_LATENCY_MULTIPLIER * latency);
	dbs_data->sampling_rate = max(dbs_data->min_sampling_rate,
				      LATENCY_MULTIPLIER * latency);

	if (!have_governor_per_policy())
		gov->gdbs_data = dbs_data;

	policy_dbs->dbs_data = dbs_data;
	policy->governor_data = policy_dbs;

	gov->kobj_type.sysfs_ops = &governor_sysfs_ops;
	ret = kobject_init_and_add(&dbs_data->attr_set.kobj, &gov->kobj_type,
				   get_governor_parent_kobj(policy),
				   "%s", gov->gov.name);
	if (!ret)
		goto out;

	/* Failure, so roll back. */
	pr_err("initialization failed (dbs_data kobject init error %d)\n", ret);

	kobject_put(&dbs_data->attr_set.kobj);

	policy->governor_data = NULL;

	if (!have_governor_per_policy())
		gov->gdbs_data = NULL;
	gov->exit(dbs_data);
	kfree(dbs_data);

free_policy_dbs_info:
	free_policy_dbs_info(policy_dbs, gov);

out:
	mutex_unlock(&gov_dbs_data_mutex);
	return ret;
}
EXPORT_SYMBOL_GPL(cpufreq_dbs_governor_init);

void cpufreq_dbs_governor_exit(struct cpufreq_policy *policy)
{
	struct dbs_governor *gov = dbs_governor_of(policy);
	struct policy_dbs_info *policy_dbs = policy->governor_data;
	struct dbs_data *dbs_data = policy_dbs->dbs_data;
	unsigned int count;

	/* Protect gov->gdbs_data against concurrent updates. */
	mutex_lock(&gov_dbs_data_mutex);

	count = gov_attr_set_put(&dbs_data->attr_set, &policy_dbs->list);

	policy->governor_data = NULL;

	if (!count) {
		if (!have_governor_per_policy())
			gov->gdbs_data = NULL;

		gov->exit(dbs_data);
		kfree(dbs_data);
	}

	free_policy_dbs_info(policy_dbs, gov);

	mutex_unlock(&gov_dbs_data_mutex);
}
EXPORT_SYMBOL_GPL(cpufreq_dbs_governor_exit);

int cpufreq_dbs_governor_start(struct cpufreq_policy *policy)
{
	struct dbs_governor *gov = dbs_governor_of(policy);
	struct policy_dbs_info *policy_dbs = policy->governor_data;
	struct dbs_data *dbs_data = policy_dbs->dbs_data;
	unsigned int sampling_rate, ignore_nice, j;
	unsigned int io_busy;

	if (!policy->cur)
		return -EINVAL;

	policy_dbs->is_shared = policy_is_shared(policy);
	policy_dbs->rate_mult = 1;

	sampling_rate = dbs_data->sampling_rate;
	ignore_nice = dbs_data->ignore_nice_load;
	io_busy = dbs_data->io_is_busy;

	for_each_cpu(j, policy->cpus) {
		struct cpu_dbs_info *j_cdbs = &per_cpu(cpu_dbs, j);

		j_cdbs->prev_cpu_idle = get_cpu_idle_time(j, &j_cdbs->prev_update_time, io_busy);
		/*
		 * Make the first invocation of dbs_update() compute the load.
		 */
		j_cdbs->prev_load = 0;

		if (ignore_nice)
			j_cdbs->prev_cpu_nice = kcpustat_cpu(j).cpustat[CPUTIME_NICE];
	}

	gov->start(policy);

	gov_set_update_util(policy_dbs, sampling_rate);
	return 0;
}
EXPORT_SYMBOL_GPL(cpufreq_dbs_governor_start);

void cpufreq_dbs_governor_stop(struct cpufreq_policy *policy)
{
	struct policy_dbs_info *policy_dbs = policy->governor_data;

	gov_clear_update_util(policy_dbs->policy);
	irq_work_sync(&policy_dbs->irq_work);
	cancel_work_sync(&policy_dbs->work);
	atomic_set(&policy_dbs->work_count, 0);
	policy_dbs->work_in_progress = false;
}
EXPORT_SYMBOL_GPL(cpufreq_dbs_governor_stop);

void cpufreq_dbs_governor_limits(struct cpufreq_policy *policy)
{
	struct policy_dbs_info *policy_dbs = policy->governor_data;

	mutex_lock(&policy_dbs->timer_mutex);
	cpufreq_policy_apply_limits(policy);
	gov_update_sample_delay(policy_dbs, 0);

	mutex_unlock(&policy_dbs->timer_mutex);
}
EXPORT_SYMBOL_GPL(cpufreq_dbs_governor_limits);
>>>>>>> v4.9.227
