/*
 * PMU support
 *
 * Copyright (C) 2012 ARM Limited
 * Author: Will Deacon <will.deacon@arm.com>
 *
 * This code is based heavily on the ARMv7 perf event code.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
<<<<<<< HEAD
#define pr_fmt(fmt) "hw perfevents: " fmt

#include <linux/bitmap.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/export.h>
#include <linux/perf_event.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/cpu_pm.h>
#include <linux/debugfs.h>
#include <linux/of.h>

#include <asm/cputype.h>
#include <asm/irq.h>
#include <asm/irq_regs.h>
#include <asm/pmu.h>
#include <asm/stacktrace.h>

/*
 * ARMv8 supports a maximum of 32 events.
 * The cycle counter is included in this total.
 */
#define ARMPMU_MAX_HWEVENTS		32

static DEFINE_PER_CPU(struct perf_event * [ARMPMU_MAX_HWEVENTS], hw_events);
static DEFINE_PER_CPU(unsigned long [BITS_TO_LONGS(ARMPMU_MAX_HWEVENTS)], used_mask);
static DEFINE_PER_CPU(struct pmu_hw_events, cpu_hw_events);
static DEFINE_PER_CPU(u32, from_idle);
static DEFINE_PER_CPU(u32, armv8_pm_pmuserenr);
static DEFINE_PER_CPU(u32, hotplug_down);

#define to_arm_pmu(p) (container_of(p, struct arm_pmu, pmu))

/* Set at runtime when we know what CPU type we are. */
static struct arm_pmu *cpu_pmu;

int
armpmu_get_max_events(void)
{
	int max_events = 0;

	if (cpu_pmu != NULL)
		max_events = cpu_pmu->num_events;

	return max_events;
}
EXPORT_SYMBOL_GPL(armpmu_get_max_events);

int perf_num_counters(void)
{
	return armpmu_get_max_events();
}
EXPORT_SYMBOL_GPL(perf_num_counters);

#define HW_OP_UNSUPPORTED		0xFFFF

#define C(_x) \
	PERF_COUNT_HW_CACHE_##_x

#define CACHE_OP_UNSUPPORTED		0xFFFF

static int
armpmu_map_cache_event(const unsigned (*cache_map)
				      [PERF_COUNT_HW_CACHE_MAX]
				      [PERF_COUNT_HW_CACHE_OP_MAX]
				      [PERF_COUNT_HW_CACHE_RESULT_MAX],
		       u64 config)
{
	unsigned int cache_type, cache_op, cache_result, ret;

	cache_type = (config >>  0) & 0xff;
	if (cache_type >= PERF_COUNT_HW_CACHE_MAX)
		return -EINVAL;

	cache_op = (config >>  8) & 0xff;
	if (cache_op >= PERF_COUNT_HW_CACHE_OP_MAX)
		return -EINVAL;

	cache_result = (config >> 16) & 0xff;
	if (cache_result >= PERF_COUNT_HW_CACHE_RESULT_MAX)
		return -EINVAL;

	ret = (int)(*cache_map)[cache_type][cache_op][cache_result];

	if (ret == CACHE_OP_UNSUPPORTED)
		return -ENOENT;

	return ret;
}

static int
armpmu_map_event(const unsigned (*event_map)[PERF_COUNT_HW_MAX], u64 config)
{
	int mapping;

	if (config >= PERF_COUNT_HW_MAX)
		return -EINVAL;

	mapping = (*event_map)[config];
	return mapping == HW_OP_UNSUPPORTED ? -ENOENT : mapping;
}

static int
armpmu_map_raw_event(u32 raw_event_mask, u64 config)
{
	return (int)(config & raw_event_mask);
}

int map_cpu_event(struct perf_event *event,
		  const unsigned (*event_map)[PERF_COUNT_HW_MAX],
		  const unsigned (*cache_map)
					[PERF_COUNT_HW_CACHE_MAX]
					[PERF_COUNT_HW_CACHE_OP_MAX]
					[PERF_COUNT_HW_CACHE_RESULT_MAX],
		  u32 raw_event_mask)
{
	u64 config = event->attr.config;

	switch (event->attr.type) {
	case PERF_TYPE_HARDWARE:
		return armpmu_map_event(event_map, config);
	case PERF_TYPE_HW_CACHE:
		return armpmu_map_cache_event(cache_map, config);
	case PERF_TYPE_RAW:
		return armpmu_map_raw_event(raw_event_mask, config);
	}

	return -ENOENT;
}

int
armpmu_event_set_period(struct perf_event *event,
			struct hw_perf_event *hwc,
			int idx)
{
	struct arm_pmu *armpmu = to_arm_pmu(event->pmu);
	s64 left = local64_read(&hwc->period_left);
	s64 period = hwc->sample_period;
	int ret = 0;

	if (unlikely(left <= -period)) {
		left = period;
		local64_set(&hwc->period_left, left);
		hwc->last_period = period;
		ret = 1;
	}

	if (unlikely(left <= 0)) {
		left += period;
		local64_set(&hwc->period_left, left);
		hwc->last_period = period;
		ret = 1;
	}

	if (left > (s64)armpmu->max_period)
		left = armpmu->max_period;

	local64_set(&hwc->prev_count, (u64)-left);

	armpmu->write_counter(idx, (u64)(-left) & 0xffffffff);

	perf_event_update_userpage(event);

	return ret;
}

u64
armpmu_event_update(struct perf_event *event,
		    struct hw_perf_event *hwc,
		    int idx)
{
	struct arm_pmu *armpmu = to_arm_pmu(event->pmu);
	u64 delta, prev_raw_count, new_raw_count;

again:
	prev_raw_count = local64_read(&hwc->prev_count);
	new_raw_count = armpmu->read_counter(idx);

	if (local64_cmpxchg(&hwc->prev_count, prev_raw_count,
			     new_raw_count) != prev_raw_count)
		goto again;

	delta = (new_raw_count - prev_raw_count) & armpmu->max_period;

	local64_add(delta, &event->count);
	local64_sub(delta, &hwc->period_left);

	return new_raw_count;
}

static void
armpmu_read(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;

	/* Don't read disabled counters! */
	if (hwc->idx < 0)
		return;

	armpmu_event_update(event, hwc, hwc->idx);
}

static void
armpmu_stop(struct perf_event *event, int flags)
{
	struct arm_pmu *armpmu = to_arm_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;

	/*
	 * ARM pmu always has to update the counter, so ignore
	 * PERF_EF_UPDATE, see comments in armpmu_start().
	 */
	if (!(hwc->state & PERF_HES_STOPPED)) {
		armpmu->disable(hwc, hwc->idx);
		barrier(); /* why? */
		armpmu_event_update(event, hwc, hwc->idx);
		hwc->state |= PERF_HES_STOPPED | PERF_HES_UPTODATE;
	}
}

static void
armpmu_start(struct perf_event *event, int flags)
{
	struct arm_pmu *armpmu = to_arm_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;

	/*
	 * ARM pmu always has to reprogram the period, so ignore
	 * PERF_EF_RELOAD, see the comment below.
	 */
	if (flags & PERF_EF_RELOAD)
		WARN_ON_ONCE(!(hwc->state & PERF_HES_UPTODATE));

	hwc->state = 0;
	/*
	 * Set the period again. Some counters can't be stopped, so when we
	 * were stopped we simply disabled the IRQ source and the counter
	 * may have been left counting. If we don't do this step then we may
	 * get an interrupt too soon or *way* too late if the overflow has
	 * happened since disabling.
	 */
	armpmu_event_set_period(event, hwc, hwc->idx);
	armpmu->enable(hwc, hwc->idx);
}

static void
armpmu_del(struct perf_event *event, int flags)
{
	struct arm_pmu *armpmu = to_arm_pmu(event->pmu);
	struct pmu_hw_events *hw_events = armpmu->get_hw_events();
	struct hw_perf_event *hwc = &event->hw;
	int idx = hwc->idx;

	WARN_ON(idx < 0);

	armpmu_stop(event, PERF_EF_UPDATE);
	hw_events->events[idx] = NULL;
	clear_bit(idx, hw_events->used_mask);

	perf_event_update_userpage(event);
}

static int
armpmu_add(struct perf_event *event, int flags)
{
	struct arm_pmu *armpmu = to_arm_pmu(event->pmu);
	struct pmu_hw_events *hw_events = armpmu->get_hw_events();
	struct hw_perf_event *hwc = &event->hw;
	int idx;
	int err = 0;

	perf_pmu_disable(event->pmu);

	if (armpmu->check_event) {
		if (armpmu->check_event(armpmu, hwc)) {
			event->state = PERF_EVENT_STATE_OFF;
			hwc->idx = -1;
			goto out;
		}
	}

	/* If we don't have a space for the counter then finish early. */
	idx = armpmu->get_event_idx(hw_events, hwc);
	if (idx < 0) {
		err = idx;
		goto out;
	}

	/*
	 * If there is an event in the counter we are going to use then make
	 * sure it is disabled.
	 */
	event->hw.idx = idx;
	armpmu->disable(hwc, idx);
	hw_events->events[idx] = event;

	hwc->state = PERF_HES_STOPPED | PERF_HES_UPTODATE;
	if (flags & PERF_EF_START)
		armpmu_start(event, PERF_EF_RELOAD);

	/* Propagate our changes to the userspace mapping. */
	perf_event_update_userpage(event);

out:
	perf_pmu_enable(event->pmu);
	return err;
}

static int
validate_event(struct pmu *pmu, struct pmu_hw_events *hw_events,
				struct perf_event *event)
{
	struct arm_pmu *armpmu;
	struct hw_perf_event fake_event = event->hw;
	struct pmu *leader_pmu = event->group_leader->pmu;

	if (is_software_event(event))
		return 1;

	/*
	 * Reject groups spanning multiple HW PMUs (e.g. CPU + CCI). The
	 * core perf code won't check that the pmu->ctx == leader->ctx
	 * until after pmu->event_init(event).
	 */
	if (event->pmu != pmu)
		return 0;

	if (event->pmu != leader_pmu || event->state < PERF_EVENT_STATE_OFF)
		return 1;

	if (event->state == PERF_EVENT_STATE_OFF && !event->attr.enable_on_exec)
		return 1;

	armpmu = to_arm_pmu(event->pmu);
	return armpmu->get_event_idx(hw_events, &fake_event) >= 0;
}

static int
validate_group(struct perf_event *event)
{
	struct perf_event *sibling, *leader = event->group_leader;
	struct pmu_hw_events fake_pmu;
	DECLARE_BITMAP(fake_used_mask, ARMPMU_MAX_HWEVENTS);

	/*
	 * Initialise the fake PMU. We only need to populate the
	 * used_mask for the purposes of validation.
	 */
	memset(fake_used_mask, 0, sizeof(fake_used_mask));
	fake_pmu.used_mask = fake_used_mask;

	if (!validate_event(event->pmu, &fake_pmu, leader))
		return -EINVAL;

	list_for_each_entry(sibling, &leader->sibling_list, group_entry) {
		if (!validate_event(event->pmu, &fake_pmu, sibling))
			return -EINVAL;
	}

	if (!validate_event(event->pmu, &fake_pmu, event))
		return -EINVAL;

	return 0;
}

static void
armpmu_disable_percpu_irq(void *data)
{
	unsigned int irq = *(unsigned int *)data;
	disable_percpu_irq(irq);
}

static void
armpmu_release_hardware(struct arm_pmu *armpmu)
{
	armpmu->free_irq(armpmu);
}

static void
armpmu_enable_percpu_irq(void *data)
{
	unsigned int irq = *(unsigned int *)data;
	enable_percpu_irq(irq, IRQ_TYPE_NONE);
}

static int
armpmu_reserve_hardware(struct arm_pmu *armpmu)
{
	int err;
	struct platform_device *pmu_device = armpmu->plat_device;

	if (!pmu_device) {
		pr_err("no PMU device registered\n");
		return -ENODEV;
	}

	err = armpmu->request_irq(armpmu, armpmu->handle_irq);
	if (err) {
		armpmu_release_hardware(armpmu);
		return err;
	}

	armpmu->pmu_state = ARM_PMU_STATE_RUNNING;

	return 0;
}

static void
hw_perf_event_destroy(struct perf_event *event)
{
	struct arm_pmu *armpmu = to_arm_pmu(event->pmu);
	atomic_t *active_events	 = &armpmu->active_events;
	struct mutex *pmu_reserve_mutex = &armpmu->reserve_mutex;

	if (atomic_dec_and_mutex_lock(active_events, pmu_reserve_mutex)) {
		armpmu_release_hardware(armpmu);
		mutex_unlock(pmu_reserve_mutex);
	}
}

static int
event_requires_mode_exclusion(struct perf_event_attr *attr)
{
	return attr->exclude_idle || attr->exclude_user ||
	       attr->exclude_kernel || attr->exclude_hv;
}

static int
__hw_perf_event_init(struct perf_event *event)
{
	struct arm_pmu *armpmu = to_arm_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	int mapping, err;

	mapping = armpmu->map_event(event);

	if (mapping < 0) {
		pr_debug("event %x:%llx not supported\n", event->attr.type,
			 event->attr.config);
		return mapping;
	}

	/*
	 * We don't assign an index until we actually place the event onto
	 * hardware. Use -1 to signify that we haven't decided where to put it
	 * yet. For SMP systems, each core has it's own PMU so we can't do any
	 * clever allocation or constraints checking at this point.
	 */
	hwc->idx		= -1;
	hwc->config_base	= 0;
	hwc->config		= 0;
	hwc->event_base		= 0;

	/*
	 * Check whether we need to exclude the counter from certain modes.
	 */
	if ((!armpmu->set_event_filter ||
	     armpmu->set_event_filter(hwc, &event->attr)) &&
	     event_requires_mode_exclusion(&event->attr)) {
		pr_debug("ARM performance counters do not support mode exclusion\n");
		return -EPERM;
	}

	/*
	 * Store the event encoding into the config_base field.
	 */
	hwc->config_base	    |= (unsigned long)mapping;

	if (!hwc->sample_period) {
		/*
		 * For non-sampling runs, limit the sample_period to half
		 * of the counter width. That way, the new counter value
		 * is far less likely to overtake the previous one unless
		 * you have some serious IRQ latency issues.
		 */
		hwc->sample_period  = armpmu->max_period >> 1;
		hwc->last_period    = hwc->sample_period;
		local64_set(&hwc->period_left, hwc->sample_period);
	}

	err = 0;
	if (event->group_leader != event) {
		err = validate_group(event);
		if (err)
			return -EINVAL;
	}

	return err;
}

static int armpmu_event_init(struct perf_event *event)
{
	struct arm_pmu *armpmu = to_arm_pmu(event->pmu);
	int err = 0;
	atomic_t *active_events = &armpmu->active_events;

	if (armpmu->map_event(event) == -ENOENT)
		return -ENOENT;

	event->destroy = hw_perf_event_destroy;

	if (!atomic_inc_not_zero(active_events)) {
		mutex_lock(&armpmu->reserve_mutex);
		if (atomic_read(active_events) == 0)
			err = armpmu_reserve_hardware(armpmu);

		if (!err)
			atomic_inc(active_events);
		mutex_unlock(&armpmu->reserve_mutex);
	}

	if (err)
		return err;

	err = __hw_perf_event_init(event);
	if (err)
		hw_perf_event_destroy(event);

	return err;
}

static void armpmu_enable(struct pmu *pmu)
{
	struct arm_pmu *armpmu = to_arm_pmu(pmu);
	struct pmu_hw_events *hw_events = armpmu->get_hw_events();
	int enabled = bitmap_weight(hw_events->used_mask, armpmu->num_events);
	int idx;

	if (*hw_events->from_idle) {
		for (idx = 0; idx <= armpmu->num_events; ++idx) {
			struct perf_event *event = hw_events->events[idx];
			struct hw_perf_event *hwc = &event->hw;

			if (!event)
				continue;

			armpmu->enable(hwc, hwc->idx);
		}

		/* Reset bit so we don't needlessly re-enable counters.*/
		*hw_events->from_idle = 0;
		/* Don't start the PMU before enabling counters after idle. */
		isb();
	}

	if (enabled)
		armpmu->start();
}

static void armpmu_disable(struct pmu *pmu)
{
	struct arm_pmu *armpmu = to_arm_pmu(pmu);
	armpmu->stop();
}

static void __init armpmu_init(struct arm_pmu *armpmu)
{
	atomic_set(&armpmu->active_events, 0);
	mutex_init(&armpmu->reserve_mutex);

	armpmu->pmu = (struct pmu) {
		.pmu_enable	= armpmu_enable,
		.pmu_disable	= armpmu_disable,
		.event_init	= armpmu_event_init,
		.add		= armpmu_add,
		.del		= armpmu_del,
		.start		= armpmu_start,
		.stop		= armpmu_stop,
		.read		= armpmu_read,
	};
}

int __init armpmu_register(struct arm_pmu *armpmu, char *name, int type)
{
	armpmu_init(armpmu);
	return perf_pmu_register(&armpmu->pmu, name, type);
}

/*
 * ARMv8 PMUv3 Performance Events handling code.
 * Common event types.
 */
enum armv8_pmuv3_perf_types {
	/* Required events. */
	ARMV8_PMUV3_PERFCTR_PMNC_SW_INCR			= 0x00,
	ARMV8_PMUV3_PERFCTR_L1_DCACHE_REFILL			= 0x03,
	ARMV8_PMUV3_PERFCTR_L1_DCACHE_ACCESS			= 0x04,
	ARMV8_PMUV3_PERFCTR_PC_BRANCH_MIS_PRED			= 0x10,
	ARMV8_PMUV3_PERFCTR_CLOCK_CYCLES			= 0x11,
	ARMV8_PMUV3_PERFCTR_PC_BRANCH_PRED			= 0x12,

	/* At least one of the following is required. */
	ARMV8_PMUV3_PERFCTR_INSTR_EXECUTED			= 0x08,
	ARMV8_PMUV3_PERFCTR_OP_SPEC				= 0x1B,

	/* Common architectural events. */
	ARMV8_PMUV3_PERFCTR_MEM_READ				= 0x06,
	ARMV8_PMUV3_PERFCTR_MEM_WRITE				= 0x07,
	ARMV8_PMUV3_PERFCTR_EXC_TAKEN				= 0x09,
	ARMV8_PMUV3_PERFCTR_EXC_EXECUTED			= 0x0A,
	ARMV8_PMUV3_PERFCTR_CID_WRITE				= 0x0B,
	ARMV8_PMUV3_PERFCTR_PC_WRITE				= 0x0C,
	ARMV8_PMUV3_PERFCTR_PC_IMM_BRANCH			= 0x0D,
	ARMV8_PMUV3_PERFCTR_PC_PROC_RETURN			= 0x0E,
	ARMV8_PMUV3_PERFCTR_MEM_UNALIGNED_ACCESS		= 0x0F,
	ARMV8_PMUV3_PERFCTR_TTBR_WRITE				= 0x1C,

	/* Common microarchitectural events. */
	ARMV8_PMUV3_PERFCTR_L1_ICACHE_REFILL			= 0x01,
	ARMV8_PMUV3_PERFCTR_ITLB_REFILL				= 0x02,
	ARMV8_PMUV3_PERFCTR_DTLB_REFILL				= 0x05,
	ARMV8_PMUV3_PERFCTR_MEM_ACCESS				= 0x13,
	ARMV8_PMUV3_PERFCTR_L1_ICACHE_ACCESS			= 0x14,
	ARMV8_PMUV3_PERFCTR_L1_DCACHE_WB			= 0x15,
	ARMV8_PMUV3_PERFCTR_L2_CACHE_ACCESS			= 0x16,
	ARMV8_PMUV3_PERFCTR_L2_CACHE_REFILL			= 0x17,
	ARMV8_PMUV3_PERFCTR_L2_CACHE_WB				= 0x18,
	ARMV8_PMUV3_PERFCTR_BUS_ACCESS				= 0x19,
	ARMV8_PMUV3_PERFCTR_MEM_ERROR				= 0x1A,
	ARMV8_PMUV3_PERFCTR_BUS_CYCLES				= 0x1D,
};

/* PMUv3 HW events mapping. */
const unsigned armv8_pmuv3_perf_map[PERF_COUNT_HW_MAX] = {
	[PERF_COUNT_HW_CPU_CYCLES]		= ARMV8_PMUV3_PERFCTR_CLOCK_CYCLES,
	[PERF_COUNT_HW_INSTRUCTIONS]		= ARMV8_PMUV3_PERFCTR_INSTR_EXECUTED,
	[PERF_COUNT_HW_CACHE_REFERENCES]	= ARMV8_PMUV3_PERFCTR_L1_DCACHE_ACCESS,
	[PERF_COUNT_HW_CACHE_MISSES]		= ARMV8_PMUV3_PERFCTR_L1_DCACHE_REFILL,
	[PERF_COUNT_HW_BRANCH_INSTRUCTIONS]	= HW_OP_UNSUPPORTED,
	[PERF_COUNT_HW_BRANCH_MISSES]		= ARMV8_PMUV3_PERFCTR_PC_BRANCH_MIS_PRED,
	[PERF_COUNT_HW_BUS_CYCLES]		= HW_OP_UNSUPPORTED,
	[PERF_COUNT_HW_STALLED_CYCLES_FRONTEND]	= HW_OP_UNSUPPORTED,
	[PERF_COUNT_HW_STALLED_CYCLES_BACKEND]	= HW_OP_UNSUPPORTED,
};

const unsigned armv8_pmuv3_perf_cache_map[PERF_COUNT_HW_CACHE_MAX]
					 [PERF_COUNT_HW_CACHE_OP_MAX]
					 [PERF_COUNT_HW_CACHE_RESULT_MAX] = {
	[C(L1D)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1_DCACHE_ACCESS,
			[C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1_DCACHE_REFILL,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1_DCACHE_ACCESS,
			[C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1_DCACHE_REFILL,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
	[C(L1I)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
	[C(LL)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
	[C(DTLB)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
	[C(ITLB)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
	[C(BPU)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_PC_BRANCH_PRED,
			[C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_PC_BRANCH_MIS_PRED,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_PC_BRANCH_PRED,
			[C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_PC_BRANCH_MIS_PRED,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
	[C(NODE)] = {
		[C(OP_READ)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_WRITE)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
		[C(OP_PREFETCH)] = {
			[C(RESULT_ACCESS)]	= CACHE_OP_UNSUPPORTED,
			[C(RESULT_MISS)]	= CACHE_OP_UNSUPPORTED,
		},
	},
=======

#include <asm/irq_regs.h>
#include <asm/perf_event.h>
#include <asm/sysreg.h>
#include <asm/virt.h>

#include <linux/acpi.h>
#include <linux/of.h>
#include <linux/perf/arm_pmu.h>
#include <linux/platform_device.h>

/*
 * ARMv8 PMUv3 Performance Events handling code.
 * Common event types (some are defined in asm/perf_event.h).
 */

/* At least one of the following is required. */
#define ARMV8_PMUV3_PERFCTR_INST_RETIRED			0x08
#define ARMV8_PMUV3_PERFCTR_INST_SPEC				0x1B

/* Common architectural events. */
#define ARMV8_PMUV3_PERFCTR_LD_RETIRED				0x06
#define ARMV8_PMUV3_PERFCTR_ST_RETIRED				0x07
#define ARMV8_PMUV3_PERFCTR_EXC_TAKEN				0x09
#define ARMV8_PMUV3_PERFCTR_EXC_RETURN				0x0A
#define ARMV8_PMUV3_PERFCTR_CID_WRITE_RETIRED			0x0B
#define ARMV8_PMUV3_PERFCTR_PC_WRITE_RETIRED			0x0C
#define ARMV8_PMUV3_PERFCTR_BR_IMMED_RETIRED			0x0D
#define ARMV8_PMUV3_PERFCTR_BR_RETURN_RETIRED			0x0E
#define ARMV8_PMUV3_PERFCTR_UNALIGNED_LDST_RETIRED		0x0F
#define ARMV8_PMUV3_PERFCTR_TTBR_WRITE_RETIRED			0x1C
#define ARMV8_PMUV3_PERFCTR_CHAIN				0x1E
#define ARMV8_PMUV3_PERFCTR_BR_RETIRED				0x21

/* Common microarchitectural events. */
#define ARMV8_PMUV3_PERFCTR_L1I_CACHE_REFILL			0x01
#define ARMV8_PMUV3_PERFCTR_L1I_TLB_REFILL			0x02
#define ARMV8_PMUV3_PERFCTR_L1D_TLB_REFILL			0x05
#define ARMV8_PMUV3_PERFCTR_MEM_ACCESS				0x13
#define ARMV8_PMUV3_PERFCTR_L1I_CACHE				0x14
#define ARMV8_PMUV3_PERFCTR_L1D_CACHE_WB			0x15
#define ARMV8_PMUV3_PERFCTR_L2D_CACHE				0x16
#define ARMV8_PMUV3_PERFCTR_L2D_CACHE_REFILL			0x17
#define ARMV8_PMUV3_PERFCTR_L2D_CACHE_WB			0x18
#define ARMV8_PMUV3_PERFCTR_BUS_ACCESS				0x19
#define ARMV8_PMUV3_PERFCTR_MEMORY_ERROR			0x1A
#define ARMV8_PMUV3_PERFCTR_BUS_CYCLES				0x1D
#define ARMV8_PMUV3_PERFCTR_L1D_CACHE_ALLOCATE			0x1F
#define ARMV8_PMUV3_PERFCTR_L2D_CACHE_ALLOCATE			0x20
#define ARMV8_PMUV3_PERFCTR_BR_MIS_PRED_RETIRED			0x22
#define ARMV8_PMUV3_PERFCTR_STALL_FRONTEND			0x23
#define ARMV8_PMUV3_PERFCTR_STALL_BACKEND			0x24
#define ARMV8_PMUV3_PERFCTR_L1D_TLB				0x25
#define ARMV8_PMUV3_PERFCTR_L1I_TLB				0x26
#define ARMV8_PMUV3_PERFCTR_L2I_CACHE				0x27
#define ARMV8_PMUV3_PERFCTR_L2I_CACHE_REFILL			0x28
#define ARMV8_PMUV3_PERFCTR_L3D_CACHE_ALLOCATE			0x29
#define ARMV8_PMUV3_PERFCTR_L3D_CACHE_REFILL			0x2A
#define ARMV8_PMUV3_PERFCTR_L3D_CACHE				0x2B
#define ARMV8_PMUV3_PERFCTR_L3D_CACHE_WB			0x2C
#define ARMV8_PMUV3_PERFCTR_L2D_TLB_REFILL			0x2D
#define ARMV8_PMUV3_PERFCTR_L2I_TLB_REFILL			0x2E
#define ARMV8_PMUV3_PERFCTR_L2D_TLB				0x2F
#define ARMV8_PMUV3_PERFCTR_L2I_TLB				0x30

/* ARMv8 recommended implementation defined event types */
#define ARMV8_IMPDEF_PERFCTR_L1D_CACHE_RD			0x40
#define ARMV8_IMPDEF_PERFCTR_L1D_CACHE_WR			0x41
#define ARMV8_IMPDEF_PERFCTR_L1D_CACHE_REFILL_RD		0x42
#define ARMV8_IMPDEF_PERFCTR_L1D_CACHE_REFILL_WR		0x43
#define ARMV8_IMPDEF_PERFCTR_L1D_CACHE_REFILL_INNER		0x44
#define ARMV8_IMPDEF_PERFCTR_L1D_CACHE_REFILL_OUTER		0x45
#define ARMV8_IMPDEF_PERFCTR_L1D_CACHE_WB_VICTIM		0x46
#define ARMV8_IMPDEF_PERFCTR_L1D_CACHE_WB_CLEAN			0x47
#define ARMV8_IMPDEF_PERFCTR_L1D_CACHE_INVAL			0x48

#define ARMV8_IMPDEF_PERFCTR_L1D_TLB_REFILL_RD			0x4C
#define ARMV8_IMPDEF_PERFCTR_L1D_TLB_REFILL_WR			0x4D
#define ARMV8_IMPDEF_PERFCTR_L1D_TLB_RD				0x4E
#define ARMV8_IMPDEF_PERFCTR_L1D_TLB_WR				0x4F
#define ARMV8_IMPDEF_PERFCTR_L2D_CACHE_RD			0x50
#define ARMV8_IMPDEF_PERFCTR_L2D_CACHE_WR			0x51
#define ARMV8_IMPDEF_PERFCTR_L2D_CACHE_REFILL_RD		0x52
#define ARMV8_IMPDEF_PERFCTR_L2D_CACHE_REFILL_WR		0x53

#define ARMV8_IMPDEF_PERFCTR_L2D_CACHE_WB_VICTIM		0x56
#define ARMV8_IMPDEF_PERFCTR_L2D_CACHE_WB_CLEAN			0x57
#define ARMV8_IMPDEF_PERFCTR_L2D_CACHE_INVAL			0x58

#define ARMV8_IMPDEF_PERFCTR_L2D_TLB_REFILL_RD			0x5C
#define ARMV8_IMPDEF_PERFCTR_L2D_TLB_REFILL_WR			0x5D
#define ARMV8_IMPDEF_PERFCTR_L2D_TLB_RD				0x5E
#define ARMV8_IMPDEF_PERFCTR_L2D_TLB_WR				0x5F

#define ARMV8_IMPDEF_PERFCTR_BUS_ACCESS_RD			0x60
#define ARMV8_IMPDEF_PERFCTR_BUS_ACCESS_WR			0x61
#define ARMV8_IMPDEF_PERFCTR_BUS_ACCESS_SHARED			0x62
#define ARMV8_IMPDEF_PERFCTR_BUS_ACCESS_NOT_SHARED		0x63
#define ARMV8_IMPDEF_PERFCTR_BUS_ACCESS_NORMAL			0x64
#define ARMV8_IMPDEF_PERFCTR_BUS_ACCESS_PERIPH			0x65

#define ARMV8_IMPDEF_PERFCTR_MEM_ACCESS_RD			0x66
#define ARMV8_IMPDEF_PERFCTR_MEM_ACCESS_WR			0x67
#define ARMV8_IMPDEF_PERFCTR_UNALIGNED_LD_SPEC			0x68
#define ARMV8_IMPDEF_PERFCTR_UNALIGNED_ST_SPEC			0x69
#define ARMV8_IMPDEF_PERFCTR_UNALIGNED_LDST_SPEC		0x6A

#define ARMV8_IMPDEF_PERFCTR_LDREX_SPEC				0x6C
#define ARMV8_IMPDEF_PERFCTR_STREX_PASS_SPEC			0x6D
#define ARMV8_IMPDEF_PERFCTR_STREX_FAIL_SPEC			0x6E
#define ARMV8_IMPDEF_PERFCTR_STREX_SPEC				0x6F
#define ARMV8_IMPDEF_PERFCTR_LD_SPEC				0x70
#define ARMV8_IMPDEF_PERFCTR_ST_SPEC				0x71
#define ARMV8_IMPDEF_PERFCTR_LDST_SPEC				0x72
#define ARMV8_IMPDEF_PERFCTR_DP_SPEC				0x73
#define ARMV8_IMPDEF_PERFCTR_ASE_SPEC				0x74
#define ARMV8_IMPDEF_PERFCTR_VFP_SPEC				0x75
#define ARMV8_IMPDEF_PERFCTR_PC_WRITE_SPEC			0x76
#define ARMV8_IMPDEF_PERFCTR_CRYPTO_SPEC			0x77
#define ARMV8_IMPDEF_PERFCTR_BR_IMMED_SPEC			0x78
#define ARMV8_IMPDEF_PERFCTR_BR_RETURN_SPEC			0x79
#define ARMV8_IMPDEF_PERFCTR_BR_INDIRECT_SPEC			0x7A

#define ARMV8_IMPDEF_PERFCTR_ISB_SPEC				0x7C
#define ARMV8_IMPDEF_PERFCTR_DSB_SPEC				0x7D
#define ARMV8_IMPDEF_PERFCTR_DMB_SPEC				0x7E

#define ARMV8_IMPDEF_PERFCTR_EXC_UNDEF				0x81
#define ARMV8_IMPDEF_PERFCTR_EXC_SVC				0x82
#define ARMV8_IMPDEF_PERFCTR_EXC_PABORT				0x83
#define ARMV8_IMPDEF_PERFCTR_EXC_DABORT				0x84

#define ARMV8_IMPDEF_PERFCTR_EXC_IRQ				0x86
#define ARMV8_IMPDEF_PERFCTR_EXC_FIQ				0x87
#define ARMV8_IMPDEF_PERFCTR_EXC_SMC				0x88

#define ARMV8_IMPDEF_PERFCTR_EXC_HVC				0x8A
#define ARMV8_IMPDEF_PERFCTR_EXC_TRAP_PABORT			0x8B
#define ARMV8_IMPDEF_PERFCTR_EXC_TRAP_DABORT			0x8C
#define ARMV8_IMPDEF_PERFCTR_EXC_TRAP_OTHER			0x8D
#define ARMV8_IMPDEF_PERFCTR_EXC_TRAP_IRQ			0x8E
#define ARMV8_IMPDEF_PERFCTR_EXC_TRAP_FIQ			0x8F
#define ARMV8_IMPDEF_PERFCTR_RC_LD_SPEC				0x90
#define ARMV8_IMPDEF_PERFCTR_RC_ST_SPEC				0x91

#define ARMV8_IMPDEF_PERFCTR_L3D_CACHE_RD			0xA0
#define ARMV8_IMPDEF_PERFCTR_L3D_CACHE_WR			0xA1
#define ARMV8_IMPDEF_PERFCTR_L3D_CACHE_REFILL_RD		0xA2
#define ARMV8_IMPDEF_PERFCTR_L3D_CACHE_REFILL_WR		0xA3

#define ARMV8_IMPDEF_PERFCTR_L3D_CACHE_WB_VICTIM		0xA6
#define ARMV8_IMPDEF_PERFCTR_L3D_CACHE_WB_CLEAN			0xA7
#define ARMV8_IMPDEF_PERFCTR_L3D_CACHE_INVAL			0xA8

/* ARMv8 Cortex-A53 specific event types. */
#define ARMV8_A53_PERFCTR_PREF_LINEFILL				0xC2

/* ARMv8 Cavium ThunderX specific event types. */
#define ARMV8_THUNDER_PERFCTR_L1D_CACHE_MISS_ST			0xE9
#define ARMV8_THUNDER_PERFCTR_L1D_CACHE_PREF_ACCESS		0xEA
#define ARMV8_THUNDER_PERFCTR_L1D_CACHE_PREF_MISS		0xEB
#define ARMV8_THUNDER_PERFCTR_L1I_CACHE_PREF_ACCESS		0xEC
#define ARMV8_THUNDER_PERFCTR_L1I_CACHE_PREF_MISS		0xED

/* PMUv3 HW events mapping. */

/*
 * ARMv8 Architectural defined events, not all of these may
 * be supported on any given implementation. Undefined events will
 * be disabled at run-time.
 */
static const unsigned armv8_pmuv3_perf_map[PERF_COUNT_HW_MAX] = {
	PERF_MAP_ALL_UNSUPPORTED,
	[PERF_COUNT_HW_CPU_CYCLES]		= ARMV8_PMUV3_PERFCTR_CPU_CYCLES,
	[PERF_COUNT_HW_INSTRUCTIONS]		= ARMV8_PMUV3_PERFCTR_INST_RETIRED,
	[PERF_COUNT_HW_CACHE_REFERENCES]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE,
	[PERF_COUNT_HW_CACHE_MISSES]		= ARMV8_PMUV3_PERFCTR_L1D_CACHE_REFILL,
	[PERF_COUNT_HW_BRANCH_INSTRUCTIONS]	= ARMV8_PMUV3_PERFCTR_PC_WRITE_RETIRED,
	[PERF_COUNT_HW_BRANCH_MISSES]		= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
	[PERF_COUNT_HW_BUS_CYCLES]		= ARMV8_PMUV3_PERFCTR_BUS_CYCLES,
	[PERF_COUNT_HW_STALLED_CYCLES_FRONTEND]	= ARMV8_PMUV3_PERFCTR_STALL_FRONTEND,
	[PERF_COUNT_HW_STALLED_CYCLES_BACKEND]	= ARMV8_PMUV3_PERFCTR_STALL_BACKEND,
};

/* ARM Cortex-A53 HW events mapping. */
static const unsigned armv8_a53_perf_map[PERF_COUNT_HW_MAX] = {
	PERF_MAP_ALL_UNSUPPORTED,
	[PERF_COUNT_HW_CPU_CYCLES]		= ARMV8_PMUV3_PERFCTR_CPU_CYCLES,
	[PERF_COUNT_HW_INSTRUCTIONS]		= ARMV8_PMUV3_PERFCTR_INST_RETIRED,
	[PERF_COUNT_HW_CACHE_REFERENCES]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE,
	[PERF_COUNT_HW_CACHE_MISSES]		= ARMV8_PMUV3_PERFCTR_L1D_CACHE_REFILL,
	[PERF_COUNT_HW_BRANCH_INSTRUCTIONS]	= ARMV8_PMUV3_PERFCTR_PC_WRITE_RETIRED,
	[PERF_COUNT_HW_BRANCH_MISSES]		= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
	[PERF_COUNT_HW_BUS_CYCLES]		= ARMV8_PMUV3_PERFCTR_BUS_CYCLES,
};

/* ARM Cortex-A57 and Cortex-A72 events mapping. */
static const unsigned armv8_a57_perf_map[PERF_COUNT_HW_MAX] = {
	PERF_MAP_ALL_UNSUPPORTED,
	[PERF_COUNT_HW_CPU_CYCLES]		= ARMV8_PMUV3_PERFCTR_CPU_CYCLES,
	[PERF_COUNT_HW_INSTRUCTIONS]		= ARMV8_PMUV3_PERFCTR_INST_RETIRED,
	[PERF_COUNT_HW_CACHE_REFERENCES]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE,
	[PERF_COUNT_HW_CACHE_MISSES]		= ARMV8_PMUV3_PERFCTR_L1D_CACHE_REFILL,
	[PERF_COUNT_HW_BRANCH_MISSES]		= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
	[PERF_COUNT_HW_BUS_CYCLES]		= ARMV8_PMUV3_PERFCTR_BUS_CYCLES,
};

static const unsigned armv8_thunder_perf_map[PERF_COUNT_HW_MAX] = {
	PERF_MAP_ALL_UNSUPPORTED,
	[PERF_COUNT_HW_CPU_CYCLES]		= ARMV8_PMUV3_PERFCTR_CPU_CYCLES,
	[PERF_COUNT_HW_INSTRUCTIONS]		= ARMV8_PMUV3_PERFCTR_INST_RETIRED,
	[PERF_COUNT_HW_CACHE_REFERENCES]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE,
	[PERF_COUNT_HW_CACHE_MISSES]		= ARMV8_PMUV3_PERFCTR_L1D_CACHE_REFILL,
	[PERF_COUNT_HW_BRANCH_INSTRUCTIONS]	= ARMV8_PMUV3_PERFCTR_PC_WRITE_RETIRED,
	[PERF_COUNT_HW_BRANCH_MISSES]		= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
	[PERF_COUNT_HW_STALLED_CYCLES_FRONTEND] = ARMV8_PMUV3_PERFCTR_STALL_FRONTEND,
	[PERF_COUNT_HW_STALLED_CYCLES_BACKEND]	= ARMV8_PMUV3_PERFCTR_STALL_BACKEND,
};

/* Broadcom Vulcan events mapping */
static const unsigned armv8_vulcan_perf_map[PERF_COUNT_HW_MAX] = {
	PERF_MAP_ALL_UNSUPPORTED,
	[PERF_COUNT_HW_CPU_CYCLES]		= ARMV8_PMUV3_PERFCTR_CPU_CYCLES,
	[PERF_COUNT_HW_INSTRUCTIONS]		= ARMV8_PMUV3_PERFCTR_INST_RETIRED,
	[PERF_COUNT_HW_CACHE_REFERENCES]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE,
	[PERF_COUNT_HW_CACHE_MISSES]		= ARMV8_PMUV3_PERFCTR_L1D_CACHE_REFILL,
	[PERF_COUNT_HW_BRANCH_INSTRUCTIONS]	= ARMV8_PMUV3_PERFCTR_BR_RETIRED,
	[PERF_COUNT_HW_BRANCH_MISSES]		= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
	[PERF_COUNT_HW_BUS_CYCLES]		= ARMV8_PMUV3_PERFCTR_BUS_CYCLES,
	[PERF_COUNT_HW_STALLED_CYCLES_FRONTEND]	= ARMV8_PMUV3_PERFCTR_STALL_FRONTEND,
	[PERF_COUNT_HW_STALLED_CYCLES_BACKEND]	= ARMV8_PMUV3_PERFCTR_STALL_BACKEND,
};

static const unsigned armv8_pmuv3_perf_cache_map[PERF_COUNT_HW_CACHE_MAX]
						[PERF_COUNT_HW_CACHE_OP_MAX]
						[PERF_COUNT_HW_CACHE_RESULT_MAX] = {
	PERF_CACHE_MAP_ALL_UNSUPPORTED,

	[C(L1D)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE,
	[C(L1D)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE_REFILL,
	[C(L1D)][C(OP_WRITE)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE,
	[C(L1D)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE_REFILL,

	[C(L1I)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1I_CACHE,
	[C(L1I)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1I_CACHE_REFILL,

	[C(DTLB)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1D_TLB_REFILL,
	[C(DTLB)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1D_TLB,

	[C(ITLB)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1I_TLB_REFILL,
	[C(ITLB)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1I_TLB,

	[C(BPU)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_BR_PRED,
	[C(BPU)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
	[C(BPU)][C(OP_WRITE)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_BR_PRED,
	[C(BPU)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
};

static const unsigned armv8_a53_perf_cache_map[PERF_COUNT_HW_CACHE_MAX]
					      [PERF_COUNT_HW_CACHE_OP_MAX]
					      [PERF_COUNT_HW_CACHE_RESULT_MAX] = {
	PERF_CACHE_MAP_ALL_UNSUPPORTED,

	[C(L1D)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE,
	[C(L1D)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE_REFILL,
	[C(L1D)][C(OP_WRITE)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE,
	[C(L1D)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1D_CACHE_REFILL,
	[C(L1D)][C(OP_PREFETCH)][C(RESULT_MISS)] = ARMV8_A53_PERFCTR_PREF_LINEFILL,

	[C(L1I)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1I_CACHE,
	[C(L1I)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1I_CACHE_REFILL,

	[C(ITLB)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1I_TLB_REFILL,

	[C(BPU)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_BR_PRED,
	[C(BPU)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
	[C(BPU)][C(OP_WRITE)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_BR_PRED,
	[C(BPU)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
};

static const unsigned armv8_a57_perf_cache_map[PERF_COUNT_HW_CACHE_MAX]
					      [PERF_COUNT_HW_CACHE_OP_MAX]
					      [PERF_COUNT_HW_CACHE_RESULT_MAX] = {
	PERF_CACHE_MAP_ALL_UNSUPPORTED,

	[C(L1D)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_IMPDEF_PERFCTR_L1D_CACHE_RD,
	[C(L1D)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_IMPDEF_PERFCTR_L1D_CACHE_REFILL_RD,
	[C(L1D)][C(OP_WRITE)][C(RESULT_ACCESS)]	= ARMV8_IMPDEF_PERFCTR_L1D_CACHE_WR,
	[C(L1D)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_IMPDEF_PERFCTR_L1D_CACHE_REFILL_WR,

	[C(L1I)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1I_CACHE,
	[C(L1I)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1I_CACHE_REFILL,

	[C(DTLB)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_IMPDEF_PERFCTR_L1D_TLB_REFILL_RD,
	[C(DTLB)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_IMPDEF_PERFCTR_L1D_TLB_REFILL_WR,

	[C(ITLB)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1I_TLB_REFILL,

	[C(BPU)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_BR_PRED,
	[C(BPU)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
	[C(BPU)][C(OP_WRITE)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_BR_PRED,
	[C(BPU)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
};

static const unsigned armv8_thunder_perf_cache_map[PERF_COUNT_HW_CACHE_MAX]
						   [PERF_COUNT_HW_CACHE_OP_MAX]
						   [PERF_COUNT_HW_CACHE_RESULT_MAX] = {
	PERF_CACHE_MAP_ALL_UNSUPPORTED,

	[C(L1D)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_IMPDEF_PERFCTR_L1D_CACHE_RD,
	[C(L1D)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_IMPDEF_PERFCTR_L1D_CACHE_REFILL_RD,
	[C(L1D)][C(OP_WRITE)][C(RESULT_ACCESS)]	= ARMV8_IMPDEF_PERFCTR_L1D_CACHE_WR,
	[C(L1D)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_THUNDER_PERFCTR_L1D_CACHE_MISS_ST,
	[C(L1D)][C(OP_PREFETCH)][C(RESULT_ACCESS)] = ARMV8_THUNDER_PERFCTR_L1D_CACHE_PREF_ACCESS,
	[C(L1D)][C(OP_PREFETCH)][C(RESULT_MISS)] = ARMV8_THUNDER_PERFCTR_L1D_CACHE_PREF_MISS,

	[C(L1I)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1I_CACHE,
	[C(L1I)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1I_CACHE_REFILL,
	[C(L1I)][C(OP_PREFETCH)][C(RESULT_ACCESS)] = ARMV8_THUNDER_PERFCTR_L1I_CACHE_PREF_ACCESS,
	[C(L1I)][C(OP_PREFETCH)][C(RESULT_MISS)] = ARMV8_THUNDER_PERFCTR_L1I_CACHE_PREF_MISS,

	[C(DTLB)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_IMPDEF_PERFCTR_L1D_TLB_RD,
	[C(DTLB)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_IMPDEF_PERFCTR_L1D_TLB_REFILL_RD,
	[C(DTLB)][C(OP_WRITE)][C(RESULT_ACCESS)] = ARMV8_IMPDEF_PERFCTR_L1D_TLB_WR,
	[C(DTLB)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_IMPDEF_PERFCTR_L1D_TLB_REFILL_WR,

	[C(ITLB)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1I_TLB_REFILL,

	[C(BPU)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_BR_PRED,
	[C(BPU)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
	[C(BPU)][C(OP_WRITE)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_BR_PRED,
	[C(BPU)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
};

static const unsigned armv8_vulcan_perf_cache_map[PERF_COUNT_HW_CACHE_MAX]
					      [PERF_COUNT_HW_CACHE_OP_MAX]
					      [PERF_COUNT_HW_CACHE_RESULT_MAX] = {
	PERF_CACHE_MAP_ALL_UNSUPPORTED,

	[C(L1D)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_IMPDEF_PERFCTR_L1D_CACHE_RD,
	[C(L1D)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_IMPDEF_PERFCTR_L1D_CACHE_REFILL_RD,
	[C(L1D)][C(OP_WRITE)][C(RESULT_ACCESS)]	= ARMV8_IMPDEF_PERFCTR_L1D_CACHE_WR,
	[C(L1D)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_IMPDEF_PERFCTR_L1D_CACHE_REFILL_WR,

	[C(L1I)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1I_CACHE,
	[C(L1I)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1I_CACHE_REFILL,

	[C(ITLB)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_L1I_TLB_REFILL,
	[C(ITLB)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_L1I_TLB,

	[C(DTLB)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_IMPDEF_PERFCTR_L1D_TLB_RD,
	[C(DTLB)][C(OP_WRITE)][C(RESULT_ACCESS)] = ARMV8_IMPDEF_PERFCTR_L1D_TLB_WR,
	[C(DTLB)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_IMPDEF_PERFCTR_L1D_TLB_REFILL_RD,
	[C(DTLB)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_IMPDEF_PERFCTR_L1D_TLB_REFILL_WR,

	[C(BPU)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_BR_PRED,
	[C(BPU)][C(OP_READ)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,
	[C(BPU)][C(OP_WRITE)][C(RESULT_ACCESS)]	= ARMV8_PMUV3_PERFCTR_BR_PRED,
	[C(BPU)][C(OP_WRITE)][C(RESULT_MISS)]	= ARMV8_PMUV3_PERFCTR_BR_MIS_PRED,

	[C(NODE)][C(OP_READ)][C(RESULT_ACCESS)]	= ARMV8_IMPDEF_PERFCTR_BUS_ACCESS_RD,
	[C(NODE)][C(OP_WRITE)][C(RESULT_ACCESS)] = ARMV8_IMPDEF_PERFCTR_BUS_ACCESS_WR,
};

static ssize_t
armv8pmu_events_sysfs_show(struct device *dev,
			   struct device_attribute *attr, char *page)
{
	struct perf_pmu_events_attr *pmu_attr;

	pmu_attr = container_of(attr, struct perf_pmu_events_attr, attr);

	return sprintf(page, "event=0x%03llx\n", pmu_attr->id);
}

#define ARMV8_EVENT_ATTR_RESOLVE(m) #m
#define ARMV8_EVENT_ATTR(name, config) \
	PMU_EVENT_ATTR(name, armv8_event_attr_##name, \
		       config, armv8pmu_events_sysfs_show)

ARMV8_EVENT_ATTR(sw_incr, ARMV8_PMUV3_PERFCTR_SW_INCR);
ARMV8_EVENT_ATTR(l1i_cache_refill, ARMV8_PMUV3_PERFCTR_L1I_CACHE_REFILL);
ARMV8_EVENT_ATTR(l1i_tlb_refill, ARMV8_PMUV3_PERFCTR_L1I_TLB_REFILL);
ARMV8_EVENT_ATTR(l1d_cache_refill, ARMV8_PMUV3_PERFCTR_L1D_CACHE_REFILL);
ARMV8_EVENT_ATTR(l1d_cache, ARMV8_PMUV3_PERFCTR_L1D_CACHE);
ARMV8_EVENT_ATTR(l1d_tlb_refill, ARMV8_PMUV3_PERFCTR_L1D_TLB_REFILL);
ARMV8_EVENT_ATTR(ld_retired, ARMV8_PMUV3_PERFCTR_LD_RETIRED);
ARMV8_EVENT_ATTR(st_retired, ARMV8_PMUV3_PERFCTR_ST_RETIRED);
ARMV8_EVENT_ATTR(inst_retired, ARMV8_PMUV3_PERFCTR_INST_RETIRED);
ARMV8_EVENT_ATTR(exc_taken, ARMV8_PMUV3_PERFCTR_EXC_TAKEN);
ARMV8_EVENT_ATTR(exc_return, ARMV8_PMUV3_PERFCTR_EXC_RETURN);
ARMV8_EVENT_ATTR(cid_write_retired, ARMV8_PMUV3_PERFCTR_CID_WRITE_RETIRED);
ARMV8_EVENT_ATTR(pc_write_retired, ARMV8_PMUV3_PERFCTR_PC_WRITE_RETIRED);
ARMV8_EVENT_ATTR(br_immed_retired, ARMV8_PMUV3_PERFCTR_BR_IMMED_RETIRED);
ARMV8_EVENT_ATTR(br_return_retired, ARMV8_PMUV3_PERFCTR_BR_RETURN_RETIRED);
ARMV8_EVENT_ATTR(unaligned_ldst_retired, ARMV8_PMUV3_PERFCTR_UNALIGNED_LDST_RETIRED);
ARMV8_EVENT_ATTR(br_mis_pred, ARMV8_PMUV3_PERFCTR_BR_MIS_PRED);
ARMV8_EVENT_ATTR(cpu_cycles, ARMV8_PMUV3_PERFCTR_CPU_CYCLES);
ARMV8_EVENT_ATTR(br_pred, ARMV8_PMUV3_PERFCTR_BR_PRED);
ARMV8_EVENT_ATTR(mem_access, ARMV8_PMUV3_PERFCTR_MEM_ACCESS);
ARMV8_EVENT_ATTR(l1i_cache, ARMV8_PMUV3_PERFCTR_L1I_CACHE);
ARMV8_EVENT_ATTR(l1d_cache_wb, ARMV8_PMUV3_PERFCTR_L1D_CACHE_WB);
ARMV8_EVENT_ATTR(l2d_cache, ARMV8_PMUV3_PERFCTR_L2D_CACHE);
ARMV8_EVENT_ATTR(l2d_cache_refill, ARMV8_PMUV3_PERFCTR_L2D_CACHE_REFILL);
ARMV8_EVENT_ATTR(l2d_cache_wb, ARMV8_PMUV3_PERFCTR_L2D_CACHE_WB);
ARMV8_EVENT_ATTR(bus_access, ARMV8_PMUV3_PERFCTR_BUS_ACCESS);
ARMV8_EVENT_ATTR(memory_error, ARMV8_PMUV3_PERFCTR_MEMORY_ERROR);
ARMV8_EVENT_ATTR(inst_spec, ARMV8_PMUV3_PERFCTR_INST_SPEC);
ARMV8_EVENT_ATTR(ttbr_write_retired, ARMV8_PMUV3_PERFCTR_TTBR_WRITE_RETIRED);
ARMV8_EVENT_ATTR(bus_cycles, ARMV8_PMUV3_PERFCTR_BUS_CYCLES);
/* Don't expose the chain event in /sys, since it's useless in isolation */
ARMV8_EVENT_ATTR(l1d_cache_allocate, ARMV8_PMUV3_PERFCTR_L1D_CACHE_ALLOCATE);
ARMV8_EVENT_ATTR(l2d_cache_allocate, ARMV8_PMUV3_PERFCTR_L2D_CACHE_ALLOCATE);
ARMV8_EVENT_ATTR(br_retired, ARMV8_PMUV3_PERFCTR_BR_RETIRED);
ARMV8_EVENT_ATTR(br_mis_pred_retired, ARMV8_PMUV3_PERFCTR_BR_MIS_PRED_RETIRED);
ARMV8_EVENT_ATTR(stall_frontend, ARMV8_PMUV3_PERFCTR_STALL_FRONTEND);
ARMV8_EVENT_ATTR(stall_backend, ARMV8_PMUV3_PERFCTR_STALL_BACKEND);
ARMV8_EVENT_ATTR(l1d_tlb, ARMV8_PMUV3_PERFCTR_L1D_TLB);
ARMV8_EVENT_ATTR(l1i_tlb, ARMV8_PMUV3_PERFCTR_L1I_TLB);
ARMV8_EVENT_ATTR(l2i_cache, ARMV8_PMUV3_PERFCTR_L2I_CACHE);
ARMV8_EVENT_ATTR(l2i_cache_refill, ARMV8_PMUV3_PERFCTR_L2I_CACHE_REFILL);
ARMV8_EVENT_ATTR(l3d_cache_allocate, ARMV8_PMUV3_PERFCTR_L3D_CACHE_ALLOCATE);
ARMV8_EVENT_ATTR(l3d_cache_refill, ARMV8_PMUV3_PERFCTR_L3D_CACHE_REFILL);
ARMV8_EVENT_ATTR(l3d_cache, ARMV8_PMUV3_PERFCTR_L3D_CACHE);
ARMV8_EVENT_ATTR(l3d_cache_wb, ARMV8_PMUV3_PERFCTR_L3D_CACHE_WB);
ARMV8_EVENT_ATTR(l2d_tlb_refill, ARMV8_PMUV3_PERFCTR_L2D_TLB_REFILL);
ARMV8_EVENT_ATTR(l2i_tlb_refill, ARMV8_PMUV3_PERFCTR_L2I_TLB_REFILL);
ARMV8_EVENT_ATTR(l2d_tlb, ARMV8_PMUV3_PERFCTR_L2D_TLB);
ARMV8_EVENT_ATTR(l2i_tlb, ARMV8_PMUV3_PERFCTR_L2I_TLB);

static struct attribute *armv8_pmuv3_event_attrs[] = {
	&armv8_event_attr_sw_incr.attr.attr,
	&armv8_event_attr_l1i_cache_refill.attr.attr,
	&armv8_event_attr_l1i_tlb_refill.attr.attr,
	&armv8_event_attr_l1d_cache_refill.attr.attr,
	&armv8_event_attr_l1d_cache.attr.attr,
	&armv8_event_attr_l1d_tlb_refill.attr.attr,
	&armv8_event_attr_ld_retired.attr.attr,
	&armv8_event_attr_st_retired.attr.attr,
	&armv8_event_attr_inst_retired.attr.attr,
	&armv8_event_attr_exc_taken.attr.attr,
	&armv8_event_attr_exc_return.attr.attr,
	&armv8_event_attr_cid_write_retired.attr.attr,
	&armv8_event_attr_pc_write_retired.attr.attr,
	&armv8_event_attr_br_immed_retired.attr.attr,
	&armv8_event_attr_br_return_retired.attr.attr,
	&armv8_event_attr_unaligned_ldst_retired.attr.attr,
	&armv8_event_attr_br_mis_pred.attr.attr,
	&armv8_event_attr_cpu_cycles.attr.attr,
	&armv8_event_attr_br_pred.attr.attr,
	&armv8_event_attr_mem_access.attr.attr,
	&armv8_event_attr_l1i_cache.attr.attr,
	&armv8_event_attr_l1d_cache_wb.attr.attr,
	&armv8_event_attr_l2d_cache.attr.attr,
	&armv8_event_attr_l2d_cache_refill.attr.attr,
	&armv8_event_attr_l2d_cache_wb.attr.attr,
	&armv8_event_attr_bus_access.attr.attr,
	&armv8_event_attr_memory_error.attr.attr,
	&armv8_event_attr_inst_spec.attr.attr,
	&armv8_event_attr_ttbr_write_retired.attr.attr,
	&armv8_event_attr_bus_cycles.attr.attr,
	&armv8_event_attr_l1d_cache_allocate.attr.attr,
	&armv8_event_attr_l2d_cache_allocate.attr.attr,
	&armv8_event_attr_br_retired.attr.attr,
	&armv8_event_attr_br_mis_pred_retired.attr.attr,
	&armv8_event_attr_stall_frontend.attr.attr,
	&armv8_event_attr_stall_backend.attr.attr,
	&armv8_event_attr_l1d_tlb.attr.attr,
	&armv8_event_attr_l1i_tlb.attr.attr,
	&armv8_event_attr_l2i_cache.attr.attr,
	&armv8_event_attr_l2i_cache_refill.attr.attr,
	&armv8_event_attr_l3d_cache_allocate.attr.attr,
	&armv8_event_attr_l3d_cache_refill.attr.attr,
	&armv8_event_attr_l3d_cache.attr.attr,
	&armv8_event_attr_l3d_cache_wb.attr.attr,
	&armv8_event_attr_l2d_tlb_refill.attr.attr,
	&armv8_event_attr_l2i_tlb_refill.attr.attr,
	&armv8_event_attr_l2d_tlb.attr.attr,
	&armv8_event_attr_l2i_tlb.attr.attr,
	NULL,
};

static umode_t
armv8pmu_event_attr_is_visible(struct kobject *kobj,
			       struct attribute *attr, int unused)
{
	struct device *dev = kobj_to_dev(kobj);
	struct pmu *pmu = dev_get_drvdata(dev);
	struct arm_pmu *cpu_pmu = container_of(pmu, struct arm_pmu, pmu);
	struct perf_pmu_events_attr *pmu_attr;

	pmu_attr = container_of(attr, struct perf_pmu_events_attr, attr.attr);

	if (test_bit(pmu_attr->id, cpu_pmu->pmceid_bitmap))
		return attr->mode;

	return 0;
}

static struct attribute_group armv8_pmuv3_events_attr_group = {
	.name = "events",
	.attrs = armv8_pmuv3_event_attrs,
	.is_visible = armv8pmu_event_attr_is_visible,
};

PMU_FORMAT_ATTR(event, "config:0-9");

static struct attribute *armv8_pmuv3_format_attrs[] = {
	&format_attr_event.attr,
	NULL,
};

static struct attribute_group armv8_pmuv3_format_attr_group = {
	.name = "format",
	.attrs = armv8_pmuv3_format_attrs,
>>>>>>> v4.9.227
};

/*
 * Perf Events' indices
 */
#define	ARMV8_IDX_CYCLE_COUNTER	0
#define	ARMV8_IDX_COUNTER0	1
<<<<<<< HEAD
#define	ARMV8_IDX_COUNTER_LAST	(ARMV8_IDX_CYCLE_COUNTER + cpu_pmu->num_events - 1)

#define	ARMV8_MAX_COUNTERS	32
#define	ARMV8_COUNTER_MASK	(ARMV8_MAX_COUNTERS - 1)
=======
#define	ARMV8_IDX_COUNTER_LAST(cpu_pmu) \
	(ARMV8_IDX_CYCLE_COUNTER + cpu_pmu->num_events - 1)
>>>>>>> v4.9.227

/*
 * ARMv8 low level PMU access
 */

/*
 * Perf Event to low level counters mapping
 */
#define	ARMV8_IDX_TO_COUNTER(x)	\
<<<<<<< HEAD
	(((x) - ARMV8_IDX_COUNTER0) & ARMV8_COUNTER_MASK)

/*
 * Per-CPU PMCR: config reg
 */
#define ARMV8_PMCR_E		(1 << 0) /* Enable all counters */
#define ARMV8_PMCR_P		(1 << 1) /* Reset all counters */
#define ARMV8_PMCR_C		(1 << 2) /* Cycle counter reset */
#define ARMV8_PMCR_D		(1 << 3) /* CCNT counts every 64th cpu cycle */
#define ARMV8_PMCR_X		(1 << 4) /* Export to ETM */
#define ARMV8_PMCR_DP		(1 << 5) /* Disable CCNT if non-invasive debug*/
#define	ARMV8_PMCR_N_SHIFT	11	 /* Number of counters supported */
#define	ARMV8_PMCR_N_MASK	0x1f
#define	ARMV8_PMCR_MASK		0x3f	 /* Mask for writable bits */

/*
 * PMOVSR: counters overflow flag status reg
 */
#define	ARMV8_OVSR_MASK		0xffffffff	/* Mask for writable bits */
#define	ARMV8_OVERFLOWED_MASK	ARMV8_OVSR_MASK

/*
 * PMXEVTYPER: Event selection reg
 */
#define	ARMV8_EVTYPE_MASK	0xc80003ff	/* Mask for writable bits */
#define	ARMV8_EVTYPE_EVENT	0x3ff		/* Mask for EVENT bits */

/*
 * Event filters for PMUv3
 */
#define	ARMV8_EXCLUDE_EL1	(1 << 31)
#define	ARMV8_EXCLUDE_EL0	(1 << 30)
#define	ARMV8_INCLUDE_EL2	(1 << 27)

static inline u32 armv8pmu_pmcr_read(void)
{
	u32 val;
	asm volatile("mrs %0, pmcr_el0" : "=r" (val));
	return val;
}

inline void armv8pmu_pmcr_write(u32 val)
{
	val &= ARMV8_PMCR_MASK;
	isb();
	asm volatile("msr pmcr_el0, %0" :: "r" (val));
=======
	(((x) - ARMV8_IDX_COUNTER0) & ARMV8_PMU_COUNTER_MASK)

static inline u32 armv8pmu_pmcr_read(void)
{
	return read_sysreg(pmcr_el0);
}

static inline void armv8pmu_pmcr_write(u32 val)
{
	val &= ARMV8_PMU_PMCR_MASK;
	isb();
	write_sysreg(val, pmcr_el0);
>>>>>>> v4.9.227
}

static inline int armv8pmu_has_overflowed(u32 pmovsr)
{
<<<<<<< HEAD
	return pmovsr & ARMV8_OVERFLOWED_MASK;
}

static inline int armv8pmu_counter_valid(int idx)
{
	return idx >= ARMV8_IDX_CYCLE_COUNTER && idx <= ARMV8_IDX_COUNTER_LAST;
=======
	return pmovsr & ARMV8_PMU_OVERFLOWED_MASK;
}

static inline int armv8pmu_counter_valid(struct arm_pmu *cpu_pmu, int idx)
{
	return idx >= ARMV8_IDX_CYCLE_COUNTER &&
		idx <= ARMV8_IDX_COUNTER_LAST(cpu_pmu);
>>>>>>> v4.9.227
}

static inline int armv8pmu_counter_has_overflowed(u32 pmnc, int idx)
{
<<<<<<< HEAD
	int ret = 0;
	u32 counter;

	if (!armv8pmu_counter_valid(idx)) {
		pr_err("CPU%u checking wrong counter %d overflow status\n",
			smp_processor_id(), idx);
	} else {
		counter = ARMV8_IDX_TO_COUNTER(idx);
		ret = pmnc & BIT(counter);
	}

	return ret;
=======
	return pmnc & BIT(ARMV8_IDX_TO_COUNTER(idx));
>>>>>>> v4.9.227
}

static inline int armv8pmu_select_counter(int idx)
{
<<<<<<< HEAD
	u32 counter;

	if (!armv8pmu_counter_valid(idx)) {
		pr_err("CPU%u selecting wrong PMNC counter %d\n",
			smp_processor_id(), idx);
		return -EINVAL;
	}

	counter = ARMV8_IDX_TO_COUNTER(idx);
	asm volatile("msr pmselr_el0, %0" :: "r" (counter));
=======
	u32 counter = ARMV8_IDX_TO_COUNTER(idx);
	write_sysreg(counter, pmselr_el0);
>>>>>>> v4.9.227
	isb();

	return idx;
}

<<<<<<< HEAD
static inline u32 armv8pmu_read_counter(int idx)
{
	u32 value = 0;

	if (!armv8pmu_counter_valid(idx))
		pr_err("CPU%u reading wrong counter %d\n",
			smp_processor_id(), idx);
	else if (idx == ARMV8_IDX_CYCLE_COUNTER)
		asm volatile("mrs %0, pmccntr_el0" : "=r" (value));
	else if (armv8pmu_select_counter(idx) == idx)
		asm volatile("mrs %0, pmxevcntr_el0" : "=r" (value));
=======
static inline u32 armv8pmu_read_counter(struct perf_event *event)
{
	struct arm_pmu *cpu_pmu = to_arm_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	int idx = hwc->idx;
	u32 value = 0;

	if (!armv8pmu_counter_valid(cpu_pmu, idx))
		pr_err("CPU%u reading wrong counter %d\n",
			smp_processor_id(), idx);
	else if (idx == ARMV8_IDX_CYCLE_COUNTER)
		value = read_sysreg(pmccntr_el0);
	else if (armv8pmu_select_counter(idx) == idx)
		value = read_sysreg(pmxevcntr_el0);
>>>>>>> v4.9.227

	return value;
}

<<<<<<< HEAD
static inline void armv8pmu_write_counter(int idx, u32 value)
{
	if (!armv8pmu_counter_valid(idx))
		pr_err("CPU%u writing wrong counter %d\n",
			smp_processor_id(), idx);
	else if (idx == ARMV8_IDX_CYCLE_COUNTER)
		asm volatile("msr pmccntr_el0, %0" :: "r" (value));
	else if (armv8pmu_select_counter(idx) == idx)
		asm volatile("msr pmxevcntr_el0, %0" :: "r" (value));
}

inline void armv8pmu_write_evtype(int idx, u32 val)
{
	if (armv8pmu_select_counter(idx) == idx) {
		val &= ARMV8_EVTYPE_MASK;
		asm volatile("msr pmxevtyper_el0, %0" :: "r" (val));
	}
}

inline int armv8pmu_enable_counter(int idx)
{
	u32 counter;

	if (!armv8pmu_counter_valid(idx)) {
		pr_err("CPU%u enabling wrong PMNC counter %d\n",
			smp_processor_id(), idx);
		return -EINVAL;
	}

	counter = ARMV8_IDX_TO_COUNTER(idx);
	asm volatile("msr pmcntenset_el0, %0" :: "r" (BIT(counter)));
	return idx;
}

inline int armv8pmu_disable_counter(int idx)
{
	u32 counter;

	if (!armv8pmu_counter_valid(idx)) {
		pr_err("CPU%u disabling wrong PMNC counter %d\n",
			smp_processor_id(), idx);
		return -EINVAL;
	}

	counter = ARMV8_IDX_TO_COUNTER(idx);
	asm volatile("msr pmcntenclr_el0, %0" :: "r" (BIT(counter)));
	return idx;
}

inline int armv8pmu_enable_intens(int idx)
{
	u32 counter;

	if (!armv8pmu_counter_valid(idx)) {
		pr_err("CPU%u enabling wrong PMNC counter IRQ enable %d\n",
			smp_processor_id(), idx);
		return -EINVAL;
	}

	counter = ARMV8_IDX_TO_COUNTER(idx);
	asm volatile("msr pmintenset_el1, %0" :: "r" (BIT(counter)));
	return idx;
}

inline int armv8pmu_disable_intens(int idx)
{
	u32 counter;

	if (!armv8pmu_counter_valid(idx)) {
		pr_err("CPU%u disabling wrong PMNC counter IRQ enable %d\n",
			smp_processor_id(), idx);
		return -EINVAL;
	}

	counter = ARMV8_IDX_TO_COUNTER(idx);
	asm volatile("msr pmintenclr_el1, %0" :: "r" (BIT(counter)));
	isb();
	/* Clear the overflow flag in case an interrupt is pending. */
	asm volatile("msr pmovsclr_el0, %0" :: "r" (BIT(counter)));
	isb();
	return idx;
}

inline u32 armv8pmu_getreset_flags(void)
=======
static inline void armv8pmu_write_counter(struct perf_event *event, u32 value)
{
	struct arm_pmu *cpu_pmu = to_arm_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	int idx = hwc->idx;

	if (!armv8pmu_counter_valid(cpu_pmu, idx))
		pr_err("CPU%u writing wrong counter %d\n",
			smp_processor_id(), idx);
	else if (idx == ARMV8_IDX_CYCLE_COUNTER) {
		/*
		 * Set the upper 32bits as this is a 64bit counter but we only
		 * count using the lower 32bits and we want an interrupt when
		 * it overflows.
		 */
		u64 value64 = 0xffffffff00000000ULL | value;

		write_sysreg(value64, pmccntr_el0);
	} else if (armv8pmu_select_counter(idx) == idx)
		write_sysreg(value, pmxevcntr_el0);
}

static inline void armv8pmu_write_evtype(int idx, u32 val)
{
	if (armv8pmu_select_counter(idx) == idx) {
		val &= ARMV8_PMU_EVTYPE_MASK;
		write_sysreg(val, pmxevtyper_el0);
	}
}

static inline int armv8pmu_enable_counter(int idx)
{
	u32 counter = ARMV8_IDX_TO_COUNTER(idx);
	write_sysreg(BIT(counter), pmcntenset_el0);
	return idx;
}

static inline int armv8pmu_disable_counter(int idx)
{
	u32 counter = ARMV8_IDX_TO_COUNTER(idx);
	write_sysreg(BIT(counter), pmcntenclr_el0);
	return idx;
}

static inline int armv8pmu_enable_intens(int idx)
{
	u32 counter = ARMV8_IDX_TO_COUNTER(idx);
	write_sysreg(BIT(counter), pmintenset_el1);
	return idx;
}

static inline int armv8pmu_disable_intens(int idx)
{
	u32 counter = ARMV8_IDX_TO_COUNTER(idx);
	write_sysreg(BIT(counter), pmintenclr_el1);
	isb();
	/* Clear the overflow flag in case an interrupt is pending. */
	write_sysreg(BIT(counter), pmovsclr_el0);
	isb();

	return idx;
}

static inline u32 armv8pmu_getreset_flags(void)
>>>>>>> v4.9.227
{
	u32 value;

	/* Read */
<<<<<<< HEAD
	asm volatile("mrs %0, pmovsclr_el0" : "=r" (value));

	/* Write to clear flags */
	value &= ARMV8_OVSR_MASK;
	asm volatile("msr pmovsclr_el0, %0" :: "r" (value));
=======
	value = read_sysreg(pmovsclr_el0);

	/* Write to clear flags */
	value &= ARMV8_PMU_OVSR_MASK;
	write_sysreg(value, pmovsclr_el0);
>>>>>>> v4.9.227

	return value;
}

<<<<<<< HEAD
static void armv8pmu_enable_event(struct hw_perf_event *hwc, int idx)
{
	unsigned long flags;
	struct pmu_hw_events *events = cpu_pmu->get_hw_events();
	u64 prev_count = local64_read(&hwc->prev_count);
=======
static void armv8pmu_enable_event(struct perf_event *event)
{
	unsigned long flags;
	struct hw_perf_event *hwc = &event->hw;
	struct arm_pmu *cpu_pmu = to_arm_pmu(event->pmu);
	struct pmu_hw_events *events = this_cpu_ptr(cpu_pmu->hw_events);
	int idx = hwc->idx;
>>>>>>> v4.9.227

	/*
	 * Enable counter and interrupt, and set the counter to count
	 * the event that we're interested in.
	 */
	raw_spin_lock_irqsave(&events->pmu_lock, flags);

	/*
	 * Disable counter
	 */
	armv8pmu_disable_counter(idx);

	/*
	 * Set event (if destined for PMNx counters).
	 */
	armv8pmu_write_evtype(idx, hwc->config_base);

	/*
	 * Enable interrupt for this counter
	 */
	armv8pmu_enable_intens(idx);

	/*
<<<<<<< HEAD
	 * Restore previous value
	 */
	armv8pmu_write_counter(idx, prev_count & 0xffffffff);

	/*
=======
>>>>>>> v4.9.227
	 * Enable counter
	 */
	armv8pmu_enable_counter(idx);

	raw_spin_unlock_irqrestore(&events->pmu_lock, flags);
}

<<<<<<< HEAD
static void armv8pmu_disable_event(struct hw_perf_event *hwc, int idx)
{
	unsigned long flags;
	struct pmu_hw_events *events = cpu_pmu->get_hw_events();
=======
static void armv8pmu_disable_event(struct perf_event *event)
{
	unsigned long flags;
	struct hw_perf_event *hwc = &event->hw;
	struct arm_pmu *cpu_pmu = to_arm_pmu(event->pmu);
	struct pmu_hw_events *events = this_cpu_ptr(cpu_pmu->hw_events);
	int idx = hwc->idx;
>>>>>>> v4.9.227

	/*
	 * Disable counter and interrupt
	 */
	raw_spin_lock_irqsave(&events->pmu_lock, flags);

	/*
	 * Disable counter
	 */
	armv8pmu_disable_counter(idx);

	/*
	 * Disable interrupt for this counter
	 */
	armv8pmu_disable_intens(idx);

	raw_spin_unlock_irqrestore(&events->pmu_lock, flags);
}

<<<<<<< HEAD
static int armv8pmu_request_irq(struct arm_pmu *cpu_pmu, irq_handler_t handler)
{
	int err, irq;
	unsigned int i, irqs;
	struct platform_device *pmu_device = cpu_pmu->plat_device;

	irqs = min(pmu_device->num_resources, num_possible_cpus());
	if (!irqs) {
		pr_err("no irqs for PMUs defined\n");
		return -ENODEV;
	}

	irq = platform_get_irq(pmu_device, 0);
	if (irq <= 0) {
		pr_err("failed to get valid irq for PMU device\n");
		return -ENODEV;
	}

	if (irq_is_percpu(irq)) {
		err = request_percpu_irq(irq, handler,
				"arm-pmu", &cpu_hw_events);

		if (err) {
			pr_err("unable to request percpu IRQ%d for ARM PMU counters\n",
					irq);
			return err;
		}

		on_each_cpu(armpmu_enable_percpu_irq, &irq, 1);
	} else {
		for (i = 0; i < irqs; ++i) {
			err = 0;
			irq = platform_get_irq(pmu_device, i);
			if (irq <= 0)
				continue;

			/*
			 * If we have a single PMU interrupt that we can't
			 * shift, assume that we're running on a uniprocessor
			 * machine and continue.
			 * Otherwise, continue without this interrupt.
			 */
			if (irq_set_affinity(irq, cpumask_of(i)) && irqs > 1) {
				pr_warn("unable to set irq affinity (irq=%d, cpu=%u)\n",
						irq, i);
				continue;
			}

			err = request_irq(irq, handler, IRQF_NOBALANCING,
					"arm-pmu", cpu_pmu);
			if (err) {
				pr_err("unable to request IRQ%d for ARM PMU counters\n",
						irq);
				return err;
			}

			cpumask_set_cpu(i, &cpu_pmu->active_irqs);
		}
	}

	return 0;
}

static void armv8pmu_free_irq(struct arm_pmu *cpu_pmu)
{
	int irq;
	unsigned int i, irqs;
	struct platform_device *pmu_device = cpu_pmu->plat_device;

	irqs = min(pmu_device->num_resources, num_possible_cpus());
	if (!irqs)
		return;

	irq = platform_get_irq(pmu_device, 0);
	if (irq <= 0)
		return;

	/*
	 * If a cpu comes online during this function, do not enable its irq.
	 * If a cpu goes offline, it should disable its irq.
	 */
	cpu_pmu->pmu_state = ARM_PMU_STATE_GOING_DOWN;

	if (irq_is_percpu(irq)) {
		on_each_cpu(armpmu_disable_percpu_irq, &irq, 1);
		free_percpu_irq(irq, &cpu_hw_events);
	} else {
		for (i = 0; i < irqs; ++i) {
			if (!cpumask_test_and_clear_cpu(i,
							&cpu_pmu->active_irqs))
				continue;
			irq = platform_get_irq(pmu_device, i);
			if (irq > 0)
				free_irq(irq, cpu_pmu);
		}
	}
	cpu_pmu->pmu_state = ARM_PMU_STATE_OFF;
}

=======
>>>>>>> v4.9.227
static irqreturn_t armv8pmu_handle_irq(int irq_num, void *dev)
{
	u32 pmovsr;
	struct perf_sample_data data;
<<<<<<< HEAD
	struct pmu_hw_events *cpuc;
=======
	struct arm_pmu *cpu_pmu = (struct arm_pmu *)dev;
	struct pmu_hw_events *cpuc = this_cpu_ptr(cpu_pmu->hw_events);
>>>>>>> v4.9.227
	struct pt_regs *regs;
	int idx;

	/*
	 * Get and reset the IRQ flags
	 */
	pmovsr = armv8pmu_getreset_flags();

	/*
	 * Did an overflow occur?
	 */
	if (!armv8pmu_has_overflowed(pmovsr))
		return IRQ_NONE;

	/*
	 * Handle the counter(s) overflow(s)
	 */
	regs = get_irq_regs();

<<<<<<< HEAD
	cpuc = this_cpu_ptr(&cpu_hw_events);
=======
>>>>>>> v4.9.227
	for (idx = 0; idx < cpu_pmu->num_events; ++idx) {
		struct perf_event *event = cpuc->events[idx];
		struct hw_perf_event *hwc;

		/* Ignore if we don't have an event. */
		if (!event)
			continue;

		/*
		 * We have a single interrupt for all counters. Check that
		 * each counter has overflowed before we process it.
		 */
		if (!armv8pmu_counter_has_overflowed(pmovsr, idx))
			continue;

		hwc = &event->hw;
<<<<<<< HEAD
		armpmu_event_update(event, hwc, idx);
		perf_sample_data_init(&data, 0, hwc->last_period);
		if (!armpmu_event_set_period(event, hwc, idx))
			continue;

		if (perf_event_overflow(event, &data, regs))
			cpu_pmu->disable(hwc, idx);
=======
		armpmu_event_update(event);
		perf_sample_data_init(&data, 0, hwc->last_period);
		if (!armpmu_event_set_period(event))
			continue;

		if (perf_event_overflow(event, &data, regs))
			cpu_pmu->disable(event);
>>>>>>> v4.9.227
	}

	/*
	 * Handle the pending perf events.
	 *
	 * Note: this call *must* be run with interrupts disabled. For
	 * platforms that can have the PMU interrupts raised as an NMI, this
	 * will not work.
	 */
	irq_work_run();

	return IRQ_HANDLED;
}

<<<<<<< HEAD
static void armv8pmu_start(void)
{
	unsigned long flags;
	struct pmu_hw_events *events = cpu_pmu->get_hw_events();

	raw_spin_lock_irqsave(&events->pmu_lock, flags);
	/* Enable all counters */
	armv8pmu_pmcr_write(armv8pmu_pmcr_read() | ARMV8_PMCR_E);
	raw_spin_unlock_irqrestore(&events->pmu_lock, flags);
}

static void armv8pmu_stop(void)
{
	unsigned long flags;
	struct pmu_hw_events *events = cpu_pmu->get_hw_events();

	raw_spin_lock_irqsave(&events->pmu_lock, flags);
	/* Disable all counters */
	armv8pmu_pmcr_write(armv8pmu_pmcr_read() & ~ARMV8_PMCR_E);
=======
static void armv8pmu_start(struct arm_pmu *cpu_pmu)
{
	unsigned long flags;
	struct pmu_hw_events *events = this_cpu_ptr(cpu_pmu->hw_events);

	raw_spin_lock_irqsave(&events->pmu_lock, flags);
	/* Enable all counters */
	armv8pmu_pmcr_write(armv8pmu_pmcr_read() | ARMV8_PMU_PMCR_E);
	raw_spin_unlock_irqrestore(&events->pmu_lock, flags);
}

static void armv8pmu_stop(struct arm_pmu *cpu_pmu)
{
	unsigned long flags;
	struct pmu_hw_events *events = this_cpu_ptr(cpu_pmu->hw_events);

	raw_spin_lock_irqsave(&events->pmu_lock, flags);
	/* Disable all counters */
	armv8pmu_pmcr_write(armv8pmu_pmcr_read() & ~ARMV8_PMU_PMCR_E);
>>>>>>> v4.9.227
	raw_spin_unlock_irqrestore(&events->pmu_lock, flags);
}

static int armv8pmu_get_event_idx(struct pmu_hw_events *cpuc,
<<<<<<< HEAD
				  struct hw_perf_event *event)
{
	int idx;
	unsigned long evtype = event->config_base & ARMV8_EVTYPE_EVENT;

	/* Place the first cycle counter request into the cycle counter. */
	if (evtype == ARMV8_PMUV3_PERFCTR_CLOCK_CYCLES) {
		if (!test_and_set_bit(ARMV8_IDX_CYCLE_COUNTER, cpuc->used_mask))
			return ARMV8_IDX_CYCLE_COUNTER;
=======
				  struct perf_event *event)
{
	int idx;
	struct arm_pmu *cpu_pmu = to_arm_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	unsigned long evtype = hwc->config_base & ARMV8_PMU_EVTYPE_EVENT;

	/* Always place a cycle counter into the cycle counter. */
	if (evtype == ARMV8_PMUV3_PERFCTR_CPU_CYCLES) {
		if (test_and_set_bit(ARMV8_IDX_CYCLE_COUNTER, cpuc->used_mask))
			return -EAGAIN;

		return ARMV8_IDX_CYCLE_COUNTER;
>>>>>>> v4.9.227
	}

	/*
	 * For anything other than a cycle counter, try and use
	 * the events counters
	 */
	for (idx = ARMV8_IDX_COUNTER0; idx < cpu_pmu->num_events; ++idx) {
		if (!test_and_set_bit(idx, cpuc->used_mask))
			return idx;
	}

	/* The counters are all in use. */
	return -EAGAIN;
}

/*
 * Add an event filter to a given event. This will only work for PMUv2 PMUs.
 */
static int armv8pmu_set_event_filter(struct hw_perf_event *event,
				     struct perf_event_attr *attr)
{
	unsigned long config_base = 0;

<<<<<<< HEAD
	if (attr->exclude_user)
		config_base |= ARMV8_EXCLUDE_EL0;
	if (attr->exclude_kernel)
		config_base |= ARMV8_EXCLUDE_EL1;
	if (!attr->exclude_hv)
		config_base |= ARMV8_INCLUDE_EL2;
=======
	if (attr->exclude_idle)
		return -EPERM;

	/*
	 * If we're running in hyp mode, then we *are* the hypervisor.
	 * Therefore we ignore exclude_hv in this configuration, since
	 * there's no hypervisor to sample anyway. This is consistent
	 * with other architectures (x86 and Power).
	 */
	if (is_kernel_in_hyp_mode()) {
		if (!attr->exclude_kernel)
			config_base |= ARMV8_PMU_INCLUDE_EL2;
	} else {
		if (attr->exclude_kernel)
			config_base |= ARMV8_PMU_EXCLUDE_EL1;
		if (!attr->exclude_hv)
			config_base |= ARMV8_PMU_INCLUDE_EL2;
	}
	if (attr->exclude_user)
		config_base |= ARMV8_PMU_EXCLUDE_EL0;
>>>>>>> v4.9.227

	/*
	 * Install the filter into config_base as this is used to
	 * construct the event type.
	 */
	event->config_base = config_base;

	return 0;
}

<<<<<<< HEAD
#ifdef CONFIG_PERF_EVENTS_USERMODE
static void armv8pmu_init_usermode(void)
{
	/* Enable access from userspace. */
	asm volatile("msr pmuserenr_el0, %0" :: "r" (0xF));

}
#else
static inline void armv8pmu_init_usermode(void)
{
	/* Disable access from userspace. */
	asm volatile("msr pmuserenr_el0, %0" :: "r" (0));

}
#endif

static void armv8pmu_reset(void *info)
{
	u32 idx, nb_cnt = cpu_pmu->num_events;

	/* The counter and interrupt enable registers are unknown at reset. */
	for (idx = ARMV8_IDX_CYCLE_COUNTER; idx < nb_cnt; ++idx)
		armv8pmu_disable_event(NULL, idx);

	/* Initialize & Reset PMNC: C and P bits. */
	armv8pmu_pmcr_write(armv8pmu_pmcr_read() | ARMV8_PMCR_P | ARMV8_PMCR_C);

	armv8pmu_init_usermode();
=======
static void armv8pmu_reset(void *info)
{
	struct arm_pmu *cpu_pmu = (struct arm_pmu *)info;
	u32 idx, nb_cnt = cpu_pmu->num_events;

	/* The counter and interrupt enable registers are unknown at reset. */
	for (idx = ARMV8_IDX_CYCLE_COUNTER; idx < nb_cnt; ++idx) {
		armv8pmu_disable_counter(idx);
		armv8pmu_disable_intens(idx);
	}

	/*
	 * Initialize & Reset PMNC. Request overflow interrupt for
	 * 64 bit cycle counter but cheat in armv8pmu_write_counter().
	 */
	armv8pmu_pmcr_write(ARMV8_PMU_PMCR_P | ARMV8_PMU_PMCR_C |
			    ARMV8_PMU_PMCR_LC);
>>>>>>> v4.9.227
}

static int armv8_pmuv3_map_event(struct perf_event *event)
{
<<<<<<< HEAD
	return map_cpu_event(event, &armv8_pmuv3_perf_map,
				&armv8_pmuv3_perf_cache_map,
				ARMV8_EVTYPE_EVENT);
}

static void armv8pmu_save_pm_registers(void *hcpu)
{
	u32 val;
	u64 lcpu = (u64)hcpu;
	int cpu = (int)lcpu;

	asm volatile("mrs %0, pmuserenr_el0" : "=r" (val));
	per_cpu(armv8_pm_pmuserenr, cpu) = val;
}

static void armv8pmu_restore_pm_registers(void *hcpu)
{
	u32 val;
	u64 lcpu = (u64)hcpu;
	int cpu = (int)lcpu;

	val = per_cpu(armv8_pm_pmuserenr, cpu);
	if (val != 0)
		asm volatile("msr pmuserenr_el0, %0" :: "r" (val));
}

static struct arm_pmu armv8pmu = {
	.handle_irq		= armv8pmu_handle_irq,
	.enable			= armv8pmu_enable_event,
	.disable		= armv8pmu_disable_event,
	.read_counter		= armv8pmu_read_counter,
	.write_counter		= armv8pmu_write_counter,
	.get_event_idx		= armv8pmu_get_event_idx,
	.start			= armv8pmu_start,
	.stop			= armv8pmu_stop,
	.reset			= armv8pmu_reset,
	.request_irq		= armv8pmu_request_irq,
	.free_irq		= armv8pmu_free_irq,
	.save_pm_registers	= armv8pmu_save_pm_registers,
	.restore_pm_registers	= armv8pmu_restore_pm_registers,
	.max_period		= (1LLU << 32) - 1,
};

static u32 __init armv8pmu_read_num_pmnc_events(void)
{
	u32 nb_cnt;

	/* Read the nb of CNTx counters supported from PMNC */
	nb_cnt = (armv8pmu_pmcr_read() >> ARMV8_PMCR_N_SHIFT) & ARMV8_PMCR_N_MASK;

	/* Add the CPU cycles counter and return */
	return nb_cnt + 1;
}

static struct arm_pmu *__init armv8_pmuv3_pmu_init(void)
{
	armv8pmu.name			= "arm/armv8-pmuv3";
	armv8pmu.map_event		= armv8_pmuv3_map_event;
	armv8pmu.num_events		= armv8pmu_read_num_pmnc_events();
	armv8pmu.set_event_filter	= armv8pmu_set_event_filter;
	return &armv8pmu;
}

/*
 * Ensure the PMU has sane values out of reset.
 * This requires SMP to be available, so exists as a separate initcall.
 */
static int __init
cpu_pmu_reset(void)
{
	if (cpu_pmu && cpu_pmu->reset)
		return on_each_cpu(cpu_pmu->reset, NULL, 1);
	return 0;
}
arch_initcall(cpu_pmu_reset);

static int cpu_has_active_perf(int cpu)
{
	struct pmu_hw_events *hw_events;
	int enabled;

	if (!cpu_pmu)
		return 0;
	hw_events = &per_cpu(cpu_hw_events, cpu);
	enabled = bitmap_weight(hw_events->used_mask, cpu_pmu->num_events);

	if (enabled)
		/*Even one event's existence is good enough.*/
		return 1;

	return 0;
}

static void armpmu_update_counters(void *x)
{
	struct pmu_hw_events *hw_events;
	int idx;

	if (!cpu_pmu)
		return;

	hw_events = cpu_pmu->get_hw_events();

	for (idx = 0; idx <= cpu_pmu->num_events; ++idx) {
		struct perf_event *event = hw_events->events[idx];

		if (!event)
			continue;

		cpu_pmu->pmu.read(event);
	}
}

static void armpmu_idle_update(void)
{
	struct pmu_hw_events *hw_events;
	int idx;

	if (!cpu_pmu)
		return;

	hw_events = cpu_pmu->get_hw_events();

	for (idx = 0; idx <= cpu_pmu->num_events; ++idx) {
		struct perf_event *event = hw_events->events[idx];

		if (!event || !event->attr.exclude_idle)
			continue;

		cpu_pmu->pmu.read(event);
	}
}

static void armpmu_hotplug_enable(void *parm_pmu)
{
	struct arm_pmu *armpmu = parm_pmu;
	struct pmu *pmu = &(armpmu->pmu);
	struct pmu_hw_events *hw_events = armpmu->get_hw_events();
	int idx;

	for (idx = 0; idx <= armpmu->num_events; ++idx) {
		struct perf_event *event = hw_events->events[idx];
		if (!event)
			continue;

		event->state = event->hotplug_save_state;
		pmu->start(event, 0);
	}
	per_cpu(hotplug_down, smp_processor_id()) = 0;
}

static void armpmu_hotplug_disable(void *parm_pmu)
{
	struct arm_pmu *armpmu = parm_pmu;
	struct pmu *pmu = &(armpmu->pmu);
	struct pmu_hw_events *hw_events = armpmu->get_hw_events();
	int idx;

	for (idx = 0; idx <= armpmu->num_events; ++idx) {
		struct perf_event *event = hw_events->events[idx];
		if (!event)
			continue;

		event->hotplug_save_state = event->state;
		/*
		 * Prevent timer tick handler perf callback from enabling
		 * this event and potentially generating an interrupt
		 * before the CPU goes down.
		 */
		event->state = PERF_EVENT_STATE_OFF;
		pmu->stop(event, 0);
	}
	per_cpu(hotplug_down, smp_processor_id()) = 1;
}

/*
 * PMU hardware loses all context when a CPU goes offline.
 * When a CPU is hotplugged back in, since some hardware registers are
 * UNKNOWN at reset, the PMU must be explicitly reset to avoid reading
 * junk values out of them.
 */
static int __cpuinit cpu_pmu_notify(struct notifier_block *b,
				    unsigned long action, void *hcpu)
{
	int irq;
	struct pmu *pmu;
	u64 lcpu = (u64)hcpu;
	int cpu = (int)lcpu;
	unsigned long masked_action = action & ~CPU_TASKS_FROZEN;
	int ret = NOTIFY_DONE;

	if ((masked_action != CPU_DOWN_PREPARE) &&
	    (masked_action != CPU_DOWN_FAILED) &&
	    (masked_action != CPU_STARTING))
		return NOTIFY_DONE;

	if (masked_action == CPU_STARTING)
		ret = NOTIFY_OK;

	if (!cpu_pmu)
		return ret;

	switch (masked_action) {
	case CPU_DOWN_PREPARE:
		if (cpu_pmu->save_pm_registers)
			smp_call_function_single(cpu,
				cpu_pmu->save_pm_registers, hcpu, 1);
		if (cpu_pmu->pmu_state != ARM_PMU_STATE_OFF) {
			if (cpu_has_active_perf(cpu))
				smp_call_function_single(cpu,
					armpmu_hotplug_disable, cpu_pmu, 1);
			/* Disarm the PMU IRQ before disappearing. */
			if (cpu_pmu->plat_device) {
				irq = cpu_pmu->percpu_irq;
				smp_call_function_single(cpu,
					    armpmu_disable_percpu_irq, &irq, 1);
			}
		}
		break;

	case CPU_STARTING:
	case CPU_DOWN_FAILED:
		/* Reset PMU to clear counters for ftrace buffer */
		if (cpu_pmu->reset)
			cpu_pmu->reset(NULL);
		if (cpu_pmu->restore_pm_registers)
			cpu_pmu->restore_pm_registers(hcpu);
		if (cpu_pmu->pmu_state == ARM_PMU_STATE_RUNNING) {
			/* Arm the PMU IRQ before appearing. */
			if (cpu_pmu->plat_device) {
				irq = cpu_pmu->percpu_irq;
				armpmu_enable_percpu_irq(&irq);
			}
			if (cpu_has_active_perf(cpu)) {
				armpmu_hotplug_enable(cpu_pmu);
				pmu = &cpu_pmu->pmu;
				pmu->pmu_enable(pmu);
			}
		}
		break;
	}
	return ret;
}

static struct notifier_block cpu_pmu_hotplug_notifier __cpuinitdata = {
	.notifier_call = cpu_pmu_notify,
};

static int perf_cpu_pm_notifier(struct notifier_block *self, unsigned long cmd,
		void *v)
{
	struct pmu *pmu;
	u64 lcpu = smp_processor_id();
	int cpu = (int)lcpu;

	if (!cpu_pmu)
		return NOTIFY_OK;

	/* If the cpu is going down, don't do anything here */
	if (per_cpu(hotplug_down, cpu))
		return NOTIFY_OK;

	switch (cmd) {
	case CPU_PM_ENTER:
		if (cpu_pmu->save_pm_registers)
			cpu_pmu->save_pm_registers((void *)lcpu);
		if (cpu_has_active_perf(cpu)) {
			armpmu_update_counters(NULL);
			pmu = &cpu_pmu->pmu;
			pmu->pmu_disable(pmu);
		}
		break;

	case CPU_PM_ENTER_FAILED:
	case CPU_PM_EXIT:
		if (cpu_has_active_perf(cpu) && cpu_pmu->reset)
			cpu_pmu->reset(NULL);
		if (cpu_pmu->restore_pm_registers)
			cpu_pmu->restore_pm_registers((void *)lcpu);
		if (cpu_has_active_perf(cpu)) {
			/*
			 * Flip this bit so armpmu_enable knows it needs
			 * to re-enable active counters.
			 */
			__get_cpu_var(from_idle) = 1;
			pmu = &cpu_pmu->pmu;
			pmu->pmu_enable(pmu);
		}
		break;
	}

	return NOTIFY_OK;
}

static struct notifier_block perf_cpu_pm_notifier_block = {
	.notifier_call = perf_cpu_pm_notifier,
};

static int perf_cpu_idle_notifier(struct notifier_block *nb,
				unsigned long action, void *data)
{
	if (action == IDLE_START)
		armpmu_idle_update();

	return NOTIFY_OK;
}

static struct notifier_block perf_cpu_idle_nb = {
	.notifier_call = perf_cpu_idle_notifier,
};

/*
 * PMU platform driver and devicetree bindings.
 */
static const struct of_device_id armpmu_of_device_ids[] = {
	{.compatible = "arm,armv8-pmuv3"},
#ifdef CONFIG_ARCH_MSM8996
	{.compatible = "qcom,kryo-pmuv3", .data = kryo_pmu_init},
#endif
	{},
};

static int armpmu_device_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	const struct of_device_id *of_id;
	int (*init_fn)(struct arm_pmu *);

	if (!cpu_pmu)
		return -ENODEV;

	if (node) {
		of_id = of_match_node(armpmu_of_device_ids, node);
		if (of_id) {
			init_fn = of_id->data;
			if (init_fn)
				init_fn(cpu_pmu);
		}
	}

	cpu_pmu->plat_device = pdev;
	cpu_pmu->percpu_irq = platform_get_irq(cpu_pmu->plat_device, 0);
	return 0;
}

static struct platform_driver armpmu_driver = {
	.driver		= {
		.name	= "arm-pmu",
		.of_match_table = armpmu_of_device_ids,
	},
	.probe		= armpmu_device_probe,
};

static int __init register_pmu_driver(void)
{
	int err;

	err = register_cpu_notifier(&cpu_pmu_hotplug_notifier);
	if (err)
		return err;

	err = cpu_pm_register_notifier(&perf_cpu_pm_notifier_block);
	if (err)
		goto err_cpu_pm;

	idle_notifier_register(&perf_cpu_idle_nb);

	err = platform_driver_register(&armpmu_driver);
	if (err)
		goto err_driver;
	return 0;

err_driver:
	cpu_pm_unregister_notifier(&perf_cpu_pm_notifier_block);
	idle_notifier_unregister(&perf_cpu_idle_nb);
err_cpu_pm:
	unregister_cpu_notifier(&cpu_pmu_hotplug_notifier);
	return err;
}
device_initcall(register_pmu_driver);

static struct pmu_hw_events *armpmu_get_cpu_events(void)
{
	return this_cpu_ptr(&cpu_hw_events);
}

static void __init cpu_pmu_init(struct arm_pmu *armpmu)
{
	int cpu;
	for_each_possible_cpu(cpu) {
		struct pmu_hw_events *events = &per_cpu(cpu_hw_events, cpu);
		events->events = per_cpu(hw_events, cpu);
		events->used_mask = per_cpu(used_mask, cpu);
		events->from_idle = &per_cpu(from_idle, cpu);
		raw_spin_lock_init(&events->pmu_lock);
	}
	armpmu->get_hw_events = armpmu_get_cpu_events;
}

static int __init init_hw_perf_events(void)
{
	u64 dfr = read_system_reg(SYS_ID_AA64DFR0_EL1);

	switch (cpuid_feature_extract_field(dfr, ID_AA64DFR0_PMUVER_SHIFT)) {
	case 0x1:	/* PMUv3 */
		cpu_pmu = armv8_pmuv3_pmu_init();
		break;
	}

	if (cpu_pmu) {
		pr_info("enabled with %s PMU driver, %d counters available\n",
			cpu_pmu->name, cpu_pmu->num_events);
		cpu_pmu_init(cpu_pmu);
		armpmu_register(cpu_pmu, "cpu", PERF_TYPE_RAW);
	} else {
		pr_info("no hardware support available\n");
	}

	return 0;
}
early_initcall(init_hw_perf_events);

/*
 * Callchain handling code.
 */
struct frame_tail {
	struct frame_tail	__user *fp;
	unsigned long		lr;
} __attribute__((packed));

/*
 * Get the return address for a single stackframe and return a pointer to the
 * next frame tail.
 */
static struct frame_tail __user *
user_backtrace(struct frame_tail __user *tail,
	       struct perf_callchain_entry *entry)
{
	struct frame_tail buftail;
	unsigned long err;

	/* Also check accessibility of one struct frame_tail beyond */
	if (!access_ok(VERIFY_READ, tail, sizeof(buftail)))
		return NULL;

	pagefault_disable();
	err = __copy_from_user_inatomic(&buftail, tail, sizeof(buftail));
	pagefault_enable();

	if (err)
		return NULL;

	perf_callchain_store(entry, buftail.lr);

	/*
	 * Frame pointers should strictly progress back up the stack
	 * (towards higher addresses).
	 */
	if (tail >= buftail.fp)
		return NULL;

	return buftail.fp;
}

#ifdef CONFIG_COMPAT
/*
 * The registers we're interested in are at the end of the variable
 * length saved register structure. The fp points at the end of this
 * structure so the address of this struct is:
 * (struct compat_frame_tail *)(xxx->fp)-1
 *
 * This code has been adapted from the ARM OProfile support.
 */
struct compat_frame_tail {
	compat_uptr_t	fp; /* a (struct compat_frame_tail *) in compat mode */
	u32		sp;
	u32		lr;
} __attribute__((packed));

static struct compat_frame_tail __user *
compat_user_backtrace(struct compat_frame_tail __user *tail,
		      struct perf_callchain_entry *entry)
{
	struct compat_frame_tail buftail;
	unsigned long err;

	/* Also check accessibility of one struct frame_tail beyond */
	if (!access_ok(VERIFY_READ, tail, sizeof(buftail)))
		return NULL;

	pagefault_disable();
	err = __copy_from_user_inatomic(&buftail, tail, sizeof(buftail));
	pagefault_enable();

	if (err)
		return NULL;

	perf_callchain_store(entry, buftail.lr);

	/*
	 * Frame pointers should strictly progress back up the stack
	 * (towards higher addresses).
	 */
	if (tail + 1 >= (struct compat_frame_tail __user *)
			compat_ptr(buftail.fp))
		return NULL;

	return (struct compat_frame_tail __user *)compat_ptr(buftail.fp) - 1;
}
#endif /* CONFIG_COMPAT */

void perf_callchain_user(struct perf_callchain_entry *entry,
			 struct pt_regs *regs)
{
	if (perf_guest_cbs && perf_guest_cbs->is_in_guest()) {
		/* We don't support guest os callchain now */
		return;
	}

	perf_callchain_store(entry, regs->pc);

	if (!compat_user_mode(regs)) {
		/* AARCH64 mode */
		struct frame_tail __user *tail;

		tail = (struct frame_tail __user *)regs->regs[29];

		while (entry->nr < PERF_MAX_STACK_DEPTH &&
		       tail && !((unsigned long)tail & 0xf))
			tail = user_backtrace(tail, entry);
	} else {
#ifdef CONFIG_COMPAT
		/* AARCH32 compat mode */
		struct compat_frame_tail __user *tail;

		tail = (struct compat_frame_tail __user *)regs->compat_fp - 1;

		while ((entry->nr < PERF_MAX_STACK_DEPTH) &&
			tail && !((unsigned long)tail & 0x3))
			tail = compat_user_backtrace(tail, entry);
#endif
	}
}

/*
 * Gets called by walk_stackframe() for every stackframe. This will be called
 * whist unwinding the stackframe and is like a subroutine return so we use
 * the PC.
 */
static int callchain_trace(struct stackframe *frame, void *data)
{
	struct perf_callchain_entry *entry = data;
	perf_callchain_store(entry, frame->pc);
	return 0;
}

void perf_callchain_kernel(struct perf_callchain_entry *entry,
			   struct pt_regs *regs)
{
	struct stackframe frame;

	if (perf_guest_cbs && perf_guest_cbs->is_in_guest()) {
		/* We don't support guest os callchain now */
		return;
	}

	frame.fp = regs->regs[29];
	frame.sp = regs->sp;
	frame.pc = regs->pc;

	walk_stackframe(&frame, callchain_trace, entry);
}

unsigned long perf_instruction_pointer(struct pt_regs *regs)
{
	if (perf_guest_cbs && perf_guest_cbs->is_in_guest())
		return perf_guest_cbs->get_guest_ip();

	return instruction_pointer(regs);
}

unsigned long perf_misc_flags(struct pt_regs *regs)
{
	int misc = 0;

	if (perf_guest_cbs && perf_guest_cbs->is_in_guest()) {
		if (perf_guest_cbs->is_user_mode())
			misc |= PERF_RECORD_MISC_GUEST_USER;
		else
			misc |= PERF_RECORD_MISC_GUEST_KERNEL;
	} else {
		if (user_mode(regs))
			misc |= PERF_RECORD_MISC_USER;
		else
			misc |= PERF_RECORD_MISC_KERNEL;
	}

	return misc;
}

static struct dentry *perf_debug_dir;

struct dentry *perf_create_debug_dir(void)
{
	if (!perf_debug_dir)
		perf_debug_dir = debugfs_create_dir("msm_perf", NULL);
	return perf_debug_dir;
}

#ifdef CONFIG_PERF_EVENTS_RESET_PMU_DEBUGFS
static __ref void reset_pmu_force(void)
{
	int cpu, ret;
	u32 save_online_mask = 0;

	for_each_possible_cpu(cpu) {
		if (!cpu_online(cpu)) {
			save_online_mask |= BIT(cpu);
			lock_device_hotplug();
			ret = device_online(get_cpu_device(cpu));
			unlock_device_hotplug();
			if (ret)
				pr_err("Failed to bring up CPU: %d, ret: %d\n",
				       cpu, ret);
		}
	}
	if (cpu_pmu && cpu_pmu->reset)
		on_each_cpu(cpu_pmu->reset, NULL, 1);
	if (cpu_pmu && cpu_pmu->plat_device)
		armpmu_release_hardware(cpu_pmu);
	for_each_possible_cpu(cpu) {
		if ((save_online_mask & BIT(cpu)) && cpu_online(cpu)) {
			lock_device_hotplug();
			ret = device_offline(get_cpu_device(cpu));
			unlock_device_hotplug();
			if (ret)
				pr_err("Failed to bring down CPU: %d, ret: %d\n",
						cpu, ret);
		}
	}
}

static int write_enabled_perfpmu_action(void *data, u64 val)
{
	if (val != 0)
		reset_pmu_force();
	return 0;
}

DEFINE_SIMPLE_ATTRIBUTE(fops_pmuaction,
		NULL, write_enabled_perfpmu_action, "%llu\n");

int __init init_pmu_actions(void)
{
	struct dentry *dir;
	struct dentry *file;
	unsigned int value = 1;

	dir = perf_create_debug_dir();
	if (!dir)
		return -ENOMEM;
	file = debugfs_create_file("resetpmu", 0220, dir,
		&value, &fops_pmuaction);
	if (!file)
		return -ENOMEM;
	return 0;
}
#else
int __init init_pmu_actions(void)
{
	return 0;
}
#endif
late_initcall(init_pmu_actions);
=======
	int hw_event_id;
	struct arm_pmu *armpmu = to_arm_pmu(event->pmu);

	hw_event_id = armpmu_map_event(event, &armv8_pmuv3_perf_map,
				       &armv8_pmuv3_perf_cache_map,
				       ARMV8_PMU_EVTYPE_EVENT);
	if (hw_event_id < 0)
		return hw_event_id;

	/* disable micro/arch events not supported by this PMU */
	if ((hw_event_id < ARMV8_PMUV3_MAX_COMMON_EVENTS) &&
		!test_bit(hw_event_id, armpmu->pmceid_bitmap)) {
			return -EOPNOTSUPP;
	}

	return hw_event_id;
}

static int armv8_a53_map_event(struct perf_event *event)
{
	return armpmu_map_event(event, &armv8_a53_perf_map,
				&armv8_a53_perf_cache_map,
				ARMV8_PMU_EVTYPE_EVENT);
}

static int armv8_a57_map_event(struct perf_event *event)
{
	return armpmu_map_event(event, &armv8_a57_perf_map,
				&armv8_a57_perf_cache_map,
				ARMV8_PMU_EVTYPE_EVENT);
}

static int armv8_thunder_map_event(struct perf_event *event)
{
	return armpmu_map_event(event, &armv8_thunder_perf_map,
				&armv8_thunder_perf_cache_map,
				ARMV8_PMU_EVTYPE_EVENT);
}

static int armv8_vulcan_map_event(struct perf_event *event)
{
	return armpmu_map_event(event, &armv8_vulcan_perf_map,
				&armv8_vulcan_perf_cache_map,
				ARMV8_PMU_EVTYPE_EVENT);
}

static void __armv8pmu_probe_pmu(void *info)
{
	struct arm_pmu *cpu_pmu = info;
	u32 pmceid[2];

	/* Read the nb of CNTx counters supported from PMNC */
	cpu_pmu->num_events = (armv8pmu_pmcr_read() >> ARMV8_PMU_PMCR_N_SHIFT)
		& ARMV8_PMU_PMCR_N_MASK;

	/* Add the CPU cycles counter */
	cpu_pmu->num_events += 1;

	pmceid[0] = read_sysreg(pmceid0_el0);
	pmceid[1] = read_sysreg(pmceid1_el0);

	bitmap_from_u32array(cpu_pmu->pmceid_bitmap,
			     ARMV8_PMUV3_MAX_COMMON_EVENTS, pmceid,
			     ARRAY_SIZE(pmceid));
}

static int armv8pmu_probe_pmu(struct arm_pmu *cpu_pmu)
{
	return smp_call_function_any(&cpu_pmu->supported_cpus,
				    __armv8pmu_probe_pmu,
				    cpu_pmu, 1);
}

static void armv8_pmu_init(struct arm_pmu *cpu_pmu)
{
	cpu_pmu->handle_irq		= armv8pmu_handle_irq,
	cpu_pmu->enable			= armv8pmu_enable_event,
	cpu_pmu->disable		= armv8pmu_disable_event,
	cpu_pmu->read_counter		= armv8pmu_read_counter,
	cpu_pmu->write_counter		= armv8pmu_write_counter,
	cpu_pmu->get_event_idx		= armv8pmu_get_event_idx,
	cpu_pmu->start			= armv8pmu_start,
	cpu_pmu->stop			= armv8pmu_stop,
	cpu_pmu->reset			= armv8pmu_reset,
	cpu_pmu->max_period		= (1LLU << 32) - 1,
	cpu_pmu->set_event_filter	= armv8pmu_set_event_filter;
}

static int armv8_pmuv3_init(struct arm_pmu *cpu_pmu)
{
	armv8_pmu_init(cpu_pmu);
	cpu_pmu->name			= "armv8_pmuv3";
	cpu_pmu->map_event		= armv8_pmuv3_map_event;
	cpu_pmu->attr_groups[ARMPMU_ATTR_GROUP_EVENTS] =
		&armv8_pmuv3_events_attr_group;
	cpu_pmu->attr_groups[ARMPMU_ATTR_GROUP_FORMATS] =
		&armv8_pmuv3_format_attr_group;
	return armv8pmu_probe_pmu(cpu_pmu);
}

static int armv8_a53_pmu_init(struct arm_pmu *cpu_pmu)
{
	armv8_pmu_init(cpu_pmu);
	cpu_pmu->name			= "armv8_cortex_a53";
	cpu_pmu->map_event		= armv8_a53_map_event;
	cpu_pmu->attr_groups[ARMPMU_ATTR_GROUP_EVENTS] =
		&armv8_pmuv3_events_attr_group;
	cpu_pmu->attr_groups[ARMPMU_ATTR_GROUP_FORMATS] =
		&armv8_pmuv3_format_attr_group;
	return armv8pmu_probe_pmu(cpu_pmu);
}

static int armv8_a57_pmu_init(struct arm_pmu *cpu_pmu)
{
	armv8_pmu_init(cpu_pmu);
	cpu_pmu->name			= "armv8_cortex_a57";
	cpu_pmu->map_event		= armv8_a57_map_event;
	cpu_pmu->attr_groups[ARMPMU_ATTR_GROUP_EVENTS] =
		&armv8_pmuv3_events_attr_group;
	cpu_pmu->attr_groups[ARMPMU_ATTR_GROUP_FORMATS] =
		&armv8_pmuv3_format_attr_group;
	return armv8pmu_probe_pmu(cpu_pmu);
}

static int armv8_a72_pmu_init(struct arm_pmu *cpu_pmu)
{
	armv8_pmu_init(cpu_pmu);
	cpu_pmu->name			= "armv8_cortex_a72";
	cpu_pmu->map_event		= armv8_a57_map_event;
	cpu_pmu->attr_groups[ARMPMU_ATTR_GROUP_EVENTS] =
		&armv8_pmuv3_events_attr_group;
	cpu_pmu->attr_groups[ARMPMU_ATTR_GROUP_FORMATS] =
		&armv8_pmuv3_format_attr_group;
	return armv8pmu_probe_pmu(cpu_pmu);
}

static int armv8_thunder_pmu_init(struct arm_pmu *cpu_pmu)
{
	armv8_pmu_init(cpu_pmu);
	cpu_pmu->name			= "armv8_cavium_thunder";
	cpu_pmu->map_event		= armv8_thunder_map_event;
	cpu_pmu->attr_groups[ARMPMU_ATTR_GROUP_EVENTS] =
		&armv8_pmuv3_events_attr_group;
	cpu_pmu->attr_groups[ARMPMU_ATTR_GROUP_FORMATS] =
		&armv8_pmuv3_format_attr_group;
	return armv8pmu_probe_pmu(cpu_pmu);
}

static int armv8_vulcan_pmu_init(struct arm_pmu *cpu_pmu)
{
	armv8_pmu_init(cpu_pmu);
	cpu_pmu->name			= "armv8_brcm_vulcan";
	cpu_pmu->map_event		= armv8_vulcan_map_event;
	cpu_pmu->attr_groups[ARMPMU_ATTR_GROUP_EVENTS] =
		&armv8_pmuv3_events_attr_group;
	cpu_pmu->attr_groups[ARMPMU_ATTR_GROUP_FORMATS] =
		&armv8_pmuv3_format_attr_group;
	return armv8pmu_probe_pmu(cpu_pmu);
}

static const struct of_device_id armv8_pmu_of_device_ids[] = {
	{.compatible = "arm,armv8-pmuv3",	.data = armv8_pmuv3_init},
	{.compatible = "arm,cortex-a53-pmu",	.data = armv8_a53_pmu_init},
	{.compatible = "arm,cortex-a57-pmu",	.data = armv8_a57_pmu_init},
	{.compatible = "arm,cortex-a72-pmu",	.data = armv8_a72_pmu_init},
	{.compatible = "cavium,thunder-pmu",	.data = armv8_thunder_pmu_init},
	{.compatible = "brcm,vulcan-pmu",	.data = armv8_vulcan_pmu_init},
	{},
};

/*
 * Non DT systems have their micro/arch events probed at run-time.
 * A fairly complete list of generic events are provided and ones that
 * aren't supported by the current PMU are disabled.
 */
static const struct pmu_probe_info armv8_pmu_probe_table[] = {
	PMU_PROBE(0, 0, armv8_pmuv3_init), /* enable all defined counters */
	{ /* sentinel value */ }
};

static int armv8_pmu_device_probe(struct platform_device *pdev)
{
	if (acpi_disabled)
		return arm_pmu_device_probe(pdev, armv8_pmu_of_device_ids,
					    NULL);

	return arm_pmu_device_probe(pdev, armv8_pmu_of_device_ids,
				    armv8_pmu_probe_table);
}

static struct platform_driver armv8_pmu_driver = {
	.driver		= {
		.name	= ARMV8_PMU_PDEV_NAME,
		.of_match_table = armv8_pmu_of_device_ids,
		.suppress_bind_attrs = true,
	},
	.probe		= armv8_pmu_device_probe,
};

builtin_platform_driver(armv8_pmu_driver);
>>>>>>> v4.9.227
