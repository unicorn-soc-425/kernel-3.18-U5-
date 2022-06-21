/*
 * Copyright 2015 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 */
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <drm/drmP.h>
#include "gpu_scheduler.h"

<<<<<<< HEAD
struct amd_sched_fence *amd_sched_fence_create(struct amd_sched_entity *s_entity, void *owner)
=======
static struct kmem_cache *sched_fence_slab;

int amd_sched_fence_slab_init(void)
{
	sched_fence_slab = kmem_cache_create(
		"amd_sched_fence", sizeof(struct amd_sched_fence), 0,
		SLAB_HWCACHE_ALIGN, NULL);
	if (!sched_fence_slab)
		return -ENOMEM;

	return 0;
}

void amd_sched_fence_slab_fini(void)
{
	rcu_barrier();
	kmem_cache_destroy(sched_fence_slab);
}

struct amd_sched_fence *amd_sched_fence_create(struct amd_sched_entity *entity,
					       void *owner)
>>>>>>> v4.9.227
{
	struct amd_sched_fence *fence = NULL;
	unsigned seq;

	fence = kmem_cache_zalloc(sched_fence_slab, GFP_KERNEL);
	if (fence == NULL)
		return NULL;

<<<<<<< HEAD
	INIT_LIST_HEAD(&fence->scheduled_cb);
	fence->owner = owner;
	fence->sched = s_entity->sched;
	spin_lock_init(&fence->lock);

	seq = atomic_inc_return(&s_entity->fence_seq);
	fence_init(&fence->base, &amd_sched_fence_ops, &fence->lock,
		   s_entity->fence_context, seq);
=======
	fence->owner = owner;
	fence->sched = entity->sched;
	spin_lock_init(&fence->lock);

	seq = atomic_inc_return(&entity->fence_seq);
	fence_init(&fence->scheduled, &amd_sched_fence_ops_scheduled,
		   &fence->lock, entity->fence_context, seq);
	fence_init(&fence->finished, &amd_sched_fence_ops_finished,
		   &fence->lock, entity->fence_context + 1, seq);
>>>>>>> v4.9.227

	return fence;
}

<<<<<<< HEAD
void amd_sched_fence_signal(struct amd_sched_fence *fence)
{
	int ret = fence_signal(&fence->base);
	if (!ret)
		FENCE_TRACE(&fence->base, "signaled from irq context\n");
	else
		FENCE_TRACE(&fence->base, "was already signaled\n");
}

void amd_sched_fence_scheduled(struct amd_sched_fence *s_fence)
{
	struct fence_cb *cur, *tmp;

	set_bit(AMD_SCHED_FENCE_SCHEDULED_BIT, &s_fence->base.flags);
	list_for_each_entry_safe(cur, tmp, &s_fence->scheduled_cb, node) {
		list_del_init(&cur->node);
		cur->func(&s_fence->base, cur);
	}
=======
void amd_sched_fence_scheduled(struct amd_sched_fence *fence)
{
	int ret = fence_signal(&fence->scheduled);

	if (!ret)
		FENCE_TRACE(&fence->scheduled, "signaled from irq context\n");
	else
		FENCE_TRACE(&fence->scheduled, "was already signaled\n");
}

void amd_sched_fence_finished(struct amd_sched_fence *fence)
{
	int ret = fence_signal(&fence->finished);

	if (!ret)
		FENCE_TRACE(&fence->finished, "signaled from irq context\n");
	else
		FENCE_TRACE(&fence->finished, "was already signaled\n");
>>>>>>> v4.9.227
}

static const char *amd_sched_fence_get_driver_name(struct fence *fence)
{
	return "amd_sched";
}

static const char *amd_sched_fence_get_timeline_name(struct fence *f)
{
	struct amd_sched_fence *fence = to_amd_sched_fence(f);
	return (const char *)fence->sched->name;
}

static bool amd_sched_fence_enable_signaling(struct fence *f)
{
	return true;
}

<<<<<<< HEAD
static void amd_sched_fence_release(struct fence *f)
{
	struct amd_sched_fence *fence = to_amd_sched_fence(f);
	kmem_cache_free(sched_fence_slab, fence);
}

const struct fence_ops amd_sched_fence_ops = {
=======
/**
 * amd_sched_fence_free - free up the fence memory
 *
 * @rcu: RCU callback head
 *
 * Free up the fence memory after the RCU grace period.
 */
static void amd_sched_fence_free(struct rcu_head *rcu)
{
	struct fence *f = container_of(rcu, struct fence, rcu);
	struct amd_sched_fence *fence = to_amd_sched_fence(f);

	fence_put(fence->parent);
	kmem_cache_free(sched_fence_slab, fence);
}

/**
 * amd_sched_fence_release_scheduled - callback that fence can be freed
 *
 * @fence: fence
 *
 * This function is called when the reference count becomes zero.
 * It just RCU schedules freeing up the fence.
 */
static void amd_sched_fence_release_scheduled(struct fence *f)
{
	struct amd_sched_fence *fence = to_amd_sched_fence(f);

	call_rcu(&fence->finished.rcu, amd_sched_fence_free);
}

/**
 * amd_sched_fence_release_finished - drop extra reference
 *
 * @f: fence
 *
 * Drop the extra reference from the scheduled fence to the base fence.
 */
static void amd_sched_fence_release_finished(struct fence *f)
{
	struct amd_sched_fence *fence = to_amd_sched_fence(f);

	fence_put(&fence->scheduled);
}

const struct fence_ops amd_sched_fence_ops_scheduled = {
>>>>>>> v4.9.227
	.get_driver_name = amd_sched_fence_get_driver_name,
	.get_timeline_name = amd_sched_fence_get_timeline_name,
	.enable_signaling = amd_sched_fence_enable_signaling,
	.signaled = NULL,
	.wait = fence_default_wait,
<<<<<<< HEAD
	.release = amd_sched_fence_release,
=======
	.release = amd_sched_fence_release_scheduled,
};

const struct fence_ops amd_sched_fence_ops_finished = {
	.get_driver_name = amd_sched_fence_get_driver_name,
	.get_timeline_name = amd_sched_fence_get_timeline_name,
	.enable_signaling = amd_sched_fence_enable_signaling,
	.signaled = NULL,
	.wait = fence_default_wait,
	.release = amd_sched_fence_release_finished,
>>>>>>> v4.9.227
};
