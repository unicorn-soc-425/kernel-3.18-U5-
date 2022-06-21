/*
 * Copyright (C) 2013 Red Hat
 * Author: Rob Clark <robdclark@gmail.com>
 *
<<<<<<< HEAD
 * Copyright (c) 2014,2017 The Linux Foundation. All rights reserved.
=======
 * Copyright (c) 2014 The Linux Foundation. All rights reserved.
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "adreno_gpu.h"
#include "msm_gem.h"
#include "msm_mmu.h"

<<<<<<< HEAD
=======
#define RB_SIZE    SZ_32K
#define RB_BLKSIZE 16

>>>>>>> v4.9.227
int adreno_get_param(struct msm_gpu *gpu, uint32_t param, uint64_t *value)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);

	switch (param) {
	case MSM_PARAM_GPU_ID:
		*value = adreno_gpu->info->revn;
		return 0;
	case MSM_PARAM_GMEM_SIZE:
		*value = adreno_gpu->gmem;
		return 0;
	case MSM_PARAM_CHIP_ID:
		*value = adreno_gpu->rev.patchid |
				(adreno_gpu->rev.minor << 8) |
				(adreno_gpu->rev.major << 16) |
				(adreno_gpu->rev.core << 24);
		return 0;
<<<<<<< HEAD
=======
	case MSM_PARAM_MAX_FREQ:
		*value = adreno_gpu->base.fast_rate;
		return 0;
	case MSM_PARAM_TIMESTAMP:
		if (adreno_gpu->funcs->get_timestamp)
			return adreno_gpu->funcs->get_timestamp(gpu, value);
		return -EINVAL;
>>>>>>> v4.9.227
	default:
		DBG("%s: invalid param: %u", gpu->name, param);
		return -EINVAL;
	}
}

#define rbmemptr(adreno_gpu, member)  \
	((adreno_gpu)->memptrs_iova + offsetof(struct adreno_rbmemptrs, member))

int adreno_hw_init(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
<<<<<<< HEAD
	int ret = 0;

	DRM_INFO("%s", gpu->name);
=======
	int ret;

	DBG("%s", gpu->name);
>>>>>>> v4.9.227

	ret = msm_gem_get_iova(gpu->rb->bo, gpu->id, &gpu->rb_iova);
	if (ret) {
		gpu->rb_iova = 0;
		dev_err(gpu->dev->dev, "could not map ringbuffer: %d\n", ret);
		return ret;
	}

<<<<<<< HEAD
	/* ring buffer address should be 32 bytes aligned */
	if (gpu->rb_iova & 0x1F)
		DRM_ERROR("ring buffer must be 32 bytes aligned\n");

=======
>>>>>>> v4.9.227
	/* Setup REG_CP_RB_CNTL: */
	adreno_gpu_write(adreno_gpu, REG_ADRENO_CP_RB_CNTL,
			/* size is log2(quad-words): */
			AXXX_CP_RB_CNTL_BUFSZ(ilog2(gpu->rb->size / 8)) |
<<<<<<< HEAD
			(1 << 27));

	/* Setup ringbuffer address: */
	adreno_gpu_write(adreno_gpu, REG_ADRENO_CP_RB_BASE,
			lower_32_bits(gpu->rb_iova));
	adreno_gpu_write(adreno_gpu, REG_ADRENO_CP_RB_BASE_HI,
			upper_32_bits(gpu->rb_iova));
=======
			AXXX_CP_RB_CNTL_BLKSZ(ilog2(RB_BLKSIZE / 8)) |
			(adreno_is_a430(adreno_gpu) ? AXXX_CP_RB_CNTL_NO_UPDATE : 0));

	/* Setup ringbuffer address: */
	adreno_gpu_write(adreno_gpu, REG_ADRENO_CP_RB_BASE, gpu->rb_iova);

	if (!adreno_is_a430(adreno_gpu))
		adreno_gpu_write(adreno_gpu, REG_ADRENO_CP_RB_RPTR_ADDR,
						rbmemptr(adreno_gpu, rptr));
>>>>>>> v4.9.227

	return 0;
}

static uint32_t get_wptr(struct msm_ringbuffer *ring)
{
	return ring->cur - ring->start;
}

<<<<<<< HEAD
static uint32_t get_rptr(struct msm_ringbuffer *ring)
{
	struct msm_gpu *gpu = ring->gpu;
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);

	return adreno_gpu_read(adreno_gpu, REG_ADRENO_CP_RB_RPTR);
=======
/* Use this helper to read rptr, since a430 doesn't update rptr in memory */
static uint32_t get_rptr(struct adreno_gpu *adreno_gpu)
{
	if (adreno_is_a430(adreno_gpu))
		return adreno_gpu->memptrs->rptr = adreno_gpu_read(
			adreno_gpu, REG_ADRENO_CP_RB_RPTR);
	else
		return adreno_gpu->memptrs->rptr;
>>>>>>> v4.9.227
}

uint32_t adreno_last_fence(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	return adreno_gpu->memptrs->fence;
}

void adreno_recover(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	struct drm_device *dev = gpu->dev;
	int ret;

	gpu->funcs->pm_suspend(gpu);

	/* reset ringbuffer: */
	gpu->rb->cur = gpu->rb->start;

<<<<<<< HEAD
	/* reset completed fence seqno, just discard anything pending: */
	adreno_gpu->memptrs->fence = gpu->submitted_fence;
=======
	/* reset completed fence seqno: */
	adreno_gpu->memptrs->fence = gpu->fctx->completed_fence;
>>>>>>> v4.9.227
	adreno_gpu->memptrs->rptr  = 0;
	adreno_gpu->memptrs->wptr  = 0;

	gpu->funcs->pm_resume(gpu);
	ret = gpu->funcs->hw_init(gpu);
	if (ret) {
		dev_err(dev->dev, "gpu hw init failed: %d\n", ret);
		/* hmm, oh well? */
	}
}

<<<<<<< HEAD
int adreno_submit(struct msm_gpu *gpu, struct msm_gem_submit *submit,
=======
void adreno_submit(struct msm_gpu *gpu, struct msm_gem_submit *submit,
>>>>>>> v4.9.227
		struct msm_file_private *ctx)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	struct msm_drm_private *priv = gpu->dev->dev_private;
	struct msm_ringbuffer *ring = gpu->rb;
	unsigned i;

	for (i = 0; i < submit->nr_cmds; i++) {
		switch (submit->cmd[i].type) {
		case MSM_SUBMIT_CMD_IB_TARGET_BUF:
			/* ignore IB-targets */
			break;
		case MSM_SUBMIT_CMD_CTX_RESTORE_BUF:
			/* ignore if there has not been a ctx switch: */
			if (priv->lastctx == ctx)
				break;
		case MSM_SUBMIT_CMD_BUF:
<<<<<<< HEAD
			OUT_PKT7(ring, CP_INDIRECT_BUFFER_PFE, 3);
			OUT_RING(ring, lower_32_bits(submit->cmd[i].iova));
			OUT_RING(ring, upper_32_bits(submit->cmd[i].iova));
			OUT_RING(ring, submit->cmd[i].size);
=======
			OUT_PKT3(ring, adreno_is_a430(adreno_gpu) ?
				CP_INDIRECT_BUFFER_PFE : CP_INDIRECT_BUFFER_PFD, 2);
			OUT_RING(ring, submit->cmd[i].iova);
			OUT_RING(ring, submit->cmd[i].size);
			OUT_PKT2(ring);
>>>>>>> v4.9.227
			break;
		}
	}

<<<<<<< HEAD
	OUT_PKT7(ring, CP_WAIT_FOR_IDLE, 0);
	OUT_PKT7(ring, CP_EVENT_WRITE, 4);
	OUT_RING(ring, CACHE_FLUSH_TS | (1 << 31));
	OUT_RING(ring, lower_32_bits(rbmemptr(adreno_gpu, fence)));
	OUT_RING(ring, upper_32_bits(rbmemptr(adreno_gpu, fence)));
	OUT_RING(ring, submit->fence);

	gpu->funcs->flush(gpu);

	return 0;
=======
	OUT_PKT0(ring, REG_AXXX_CP_SCRATCH_REG2, 1);
	OUT_RING(ring, submit->fence->seqno);

	if (adreno_is_a3xx(adreno_gpu) || adreno_is_a4xx(adreno_gpu)) {
		/* Flush HLSQ lazy updates to make sure there is nothing
		 * pending for indirect loads after the timestamp has
		 * passed:
		 */
		OUT_PKT3(ring, CP_EVENT_WRITE, 1);
		OUT_RING(ring, HLSQ_FLUSH);

		OUT_PKT3(ring, CP_WAIT_FOR_IDLE, 1);
		OUT_RING(ring, 0x00000000);
	}

	OUT_PKT3(ring, CP_EVENT_WRITE, 3);
	OUT_RING(ring, CACHE_FLUSH_TS);
	OUT_RING(ring, rbmemptr(adreno_gpu, fence));
	OUT_RING(ring, submit->fence->seqno);

	/* we could maybe be clever and only CP_COND_EXEC the interrupt: */
	OUT_PKT3(ring, CP_INTERRUPT, 1);
	OUT_RING(ring, 0x80000000);

	/* Workaround for missing irq issue on 8x16/a306.  Unsure if the
	 * root cause is a platform issue or some a306 quirk, but this
	 * keeps things humming along:
	 */
	if (adreno_is_a306(adreno_gpu)) {
		OUT_PKT3(ring, CP_WAIT_FOR_IDLE, 1);
		OUT_RING(ring, 0x00000000);
		OUT_PKT3(ring, CP_INTERRUPT, 1);
		OUT_RING(ring, 0x80000000);
	}

#if 0
	if (adreno_is_a3xx(adreno_gpu)) {
		/* Dummy set-constant to trigger context rollover */
		OUT_PKT3(ring, CP_SET_CONSTANT, 2);
		OUT_RING(ring, CP_REG(REG_A3XX_HLSQ_CL_KERNEL_GROUP_X_REG));
		OUT_RING(ring, 0x00000000);
	}
#endif

	gpu->funcs->flush(gpu);
>>>>>>> v4.9.227
}

void adreno_flush(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
<<<<<<< HEAD
	uint32_t wptr = get_wptr(gpu->rb);

	/* ensure writes to ringbuffer have hit system memory: */
	mb();
	adreno_gpu_write(adreno_gpu, REG_ADRENO_CP_RB_WPTR, wptr);
}

void adreno_idle(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
=======
>>>>>>> v4.9.227
	uint32_t wptr;

	/*
	 * Mask wptr value that we calculate to fit in the HW range. This is
	 * to account for the possibility that the last command fit exactly into
	 * the ringbuffer and rb->next hasn't wrapped to zero yet
	 */
	wptr = get_wptr(gpu->rb) & ((gpu->rb->size / 4) - 1);

<<<<<<< HEAD
	/* wait for CP to drain ringbuffer: */
	if (spin_until(adreno_gpu->memptrs->rptr == wptr))
		DRM_ERROR("%s: timeout waiting to drain ringbuffer!\n",
			  gpu->name);
=======
	/* ensure writes to ringbuffer have hit system memory: */
	mb();

	adreno_gpu_write(adreno_gpu, REG_ADRENO_CP_RB_WPTR, wptr);
}

void adreno_idle(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	uint32_t wptr = get_wptr(gpu->rb);
	int ret;

	/* wait for CP to drain ringbuffer: */
	ret = spin_until(get_rptr(adreno_gpu) == wptr);

	if (ret)
		DRM_ERROR("%s: timeout waiting to drain ringbuffer!\n", gpu->name);
>>>>>>> v4.9.227

	/* TODO maybe we need to reset GPU here to recover from hang? */
}

#ifdef CONFIG_DEBUG_FS
void adreno_show(struct msm_gpu *gpu, struct seq_file *m)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	int i;

	seq_printf(m, "revision: %d (%d.%d.%d.%d)\n",
			adreno_gpu->info->revn, adreno_gpu->rev.core,
			adreno_gpu->rev.major, adreno_gpu->rev.minor,
			adreno_gpu->rev.patchid);

	seq_printf(m, "fence:    %d/%d\n", adreno_gpu->memptrs->fence,
<<<<<<< HEAD
			gpu->submitted_fence);
=======
			gpu->fctx->last_fence);
	seq_printf(m, "rptr:     %d\n", get_rptr(adreno_gpu));
	seq_printf(m, "wptr:     %d\n", adreno_gpu->memptrs->wptr);
	seq_printf(m, "rb wptr:  %d\n", get_wptr(gpu->rb));

>>>>>>> v4.9.227
	gpu->funcs->pm_resume(gpu);

	/* dump these out in a form that can be parsed by demsm: */
	seq_printf(m, "IO:region %s 00000000 00020000\n", gpu->name);
	for (i = 0; adreno_gpu->registers[i] != ~0; i += 2) {
		uint32_t start = adreno_gpu->registers[i];
		uint32_t end   = adreno_gpu->registers[i+1];
		uint32_t addr;

		for (addr = start; addr <= end; addr++) {
			uint32_t val = gpu_read(gpu, addr);
			seq_printf(m, "IO:R %08x %08x\n", addr<<2, val);
		}
	}

	gpu->funcs->pm_suspend(gpu);
}
#endif

/* Dump common gpu status and scratch registers on any hang, to make
 * the hangcheck logs more useful.  The scratch registers seem always
 * safe to read when GPU has hung (unlike some other regs, depending
 * on how the GPU hung), and they are useful to match up to cmdstream
 * dumps when debugging hangs:
 */
void adreno_dump_info(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	int i;

<<<<<<< HEAD
	pr_info("revision: %d (%d.%d.%d.%d)\n",
=======
	printk("revision: %d (%d.%d.%d.%d)\n",
>>>>>>> v4.9.227
			adreno_gpu->info->revn, adreno_gpu->rev.core,
			adreno_gpu->rev.major, adreno_gpu->rev.minor,
			adreno_gpu->rev.patchid);

<<<<<<< HEAD
	pr_info("fence:    %d/%d\n", adreno_gpu->memptrs->fence,
			gpu->submitted_fence);
	pr_info("rb wptr:  %d\n", get_wptr(gpu->rb));

	for (i = 0; i < 8; i++) {
		pr_info("CP_SCRATCH_REG%d: %u\n", i,
=======
	printk("fence:    %d/%d\n", adreno_gpu->memptrs->fence,
			gpu->fctx->last_fence);
	printk("rptr:     %d\n", get_rptr(adreno_gpu));
	printk("wptr:     %d\n", adreno_gpu->memptrs->wptr);
	printk("rb wptr:  %d\n", get_wptr(gpu->rb));

	for (i = 0; i < 8; i++) {
		printk("CP_SCRATCH_REG%d: %u\n", i,
>>>>>>> v4.9.227
			gpu_read(gpu, REG_AXXX_CP_SCRATCH_REG0 + i));
	}
}

/* would be nice to not have to duplicate the _show() stuff with printk(): */
void adreno_dump(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	int i;

	/* dump these out in a form that can be parsed by demsm: */
<<<<<<< HEAD
	pr_info("IO:region %s 00000000 00020000\n", gpu->name);
=======
	printk("IO:region %s 00000000 00020000\n", gpu->name);
>>>>>>> v4.9.227
	for (i = 0; adreno_gpu->registers[i] != ~0; i += 2) {
		uint32_t start = adreno_gpu->registers[i];
		uint32_t end   = adreno_gpu->registers[i+1];
		uint32_t addr;

		for (addr = start; addr <= end; addr++) {
			uint32_t val = gpu_read(gpu, addr);
<<<<<<< HEAD

			pr_info("IO:R %08x %08x\n", addr<<2, val);
=======
			printk("IO:R %08x %08x\n", addr<<2, val);
>>>>>>> v4.9.227
		}
	}
}

<<<<<<< HEAD
#define GSL_RB_NOP_SIZEDWORDS      2
static bool ring_freewords(struct msm_gpu *gpu, uint32_t ndwords)
{
	uint32_t size = gpu->rb->size / 4;
	uint32_t wptr = get_wptr(gpu->rb);
	uint32_t rptr = get_rptr(gpu->rb);

	if (rptr <= wptr) {
		if ((wptr + ndwords) <=
				(size - GSL_RB_NOP_SIZEDWORDS))
			return true;
		/*
		 * There isn't enough space toward the end of ringbuffer. So
		 * look for space from the beginning of ringbuffer upto the
		 * read pointer.
		 */
		if (ndwords < rptr) {
			OUT_RING(gpu->rb, cp_pkt7(CP_NOP, (size - wptr - 1)));
			gpu->rb->cur = gpu->rb->start;
			return true;
		}
	}

	if ((wptr + ndwords) < rptr)
		return true;

	return false;
=======
static uint32_t ring_freewords(struct msm_gpu *gpu)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);
	uint32_t size = gpu->rb->size / 4;
	uint32_t wptr = get_wptr(gpu->rb);
	uint32_t rptr = get_rptr(adreno_gpu);
	return (rptr + (size - 1) - wptr) % size;
>>>>>>> v4.9.227
}

void adreno_wait_ring(struct msm_gpu *gpu, uint32_t ndwords)
{
<<<<<<< HEAD
	if (spin_until(ring_freewords(gpu, ndwords)))
		DRM_ERROR("%s: timeout waiting for ringbuffer space\n",
			  gpu->name);
=======
	if (spin_until(ring_freewords(gpu) >= ndwords))
		DRM_ERROR("%s: timeout waiting for ringbuffer space\n", gpu->name);
>>>>>>> v4.9.227
}

static const char *iommu_ports[] = {
		"gfx3d_user", "gfx3d_priv",
		"gfx3d1_user", "gfx3d1_priv",
};

int adreno_gpu_init(struct drm_device *drm, struct platform_device *pdev,
<<<<<<< HEAD
		struct adreno_gpu *adreno_gpu,
		const struct adreno_gpu_funcs *funcs)
=======
		struct adreno_gpu *adreno_gpu, const struct adreno_gpu_funcs *funcs)
>>>>>>> v4.9.227
{
	struct adreno_platform_config *config = pdev->dev.platform_data;
	struct msm_gpu *gpu = &adreno_gpu->base;
	struct msm_mmu *mmu;
	int ret;

	adreno_gpu->funcs = funcs;
	adreno_gpu->info = adreno_info(config->rev);
	adreno_gpu->gmem = adreno_gpu->info->gmem;
	adreno_gpu->revn = adreno_gpu->info->revn;
	adreno_gpu->rev = config->rev;

	gpu->fast_rate = config->fast_rate;
	gpu->slow_rate = config->slow_rate;
	gpu->bus_freq  = config->bus_freq;
<<<<<<< HEAD
#ifdef CONFIG_MSM_BUS_SCALING
=======
#ifdef DOWNSTREAM_CONFIG_MSM_BUS_SCALING
>>>>>>> v4.9.227
	gpu->bus_scale_table = config->bus_scale_table;
#endif

	DBG("fast_rate=%u, slow_rate=%u, bus_freq=%u",
			gpu->fast_rate, gpu->slow_rate, gpu->bus_freq);

	ret = msm_gpu_init(drm, pdev, &adreno_gpu->base, &funcs->base,
<<<<<<< HEAD
			   adreno_gpu->info->name,
			   "kgsl_3d0_reg_memory", "kgsl_3d0_irq");
	if (ret)
		return ret;

	ret = request_firmware(&adreno_gpu->pm4,
			       adreno_gpu->info->pm4fw, drm->dev);
=======
			adreno_gpu->info->name, "kgsl_3d0_reg_memory", "kgsl_3d0_irq",
			RB_SIZE);
	if (ret)
		return ret;

	ret = request_firmware(&adreno_gpu->pm4, adreno_gpu->info->pm4fw, drm->dev);
>>>>>>> v4.9.227
	if (ret) {
		dev_err(drm->dev, "failed to load %s PM4 firmware: %d\n",
				adreno_gpu->info->pm4fw, ret);
		return ret;
	}

<<<<<<< HEAD
	ret = request_firmware(&adreno_gpu->pfp,
			adreno_gpu->info->pfpfw, drm->dev);
=======
	ret = request_firmware(&adreno_gpu->pfp, adreno_gpu->info->pfpfw, drm->dev);
>>>>>>> v4.9.227
	if (ret) {
		dev_err(drm->dev, "failed to load %s PFP firmware: %d\n",
				adreno_gpu->info->pfpfw, ret);
		return ret;
	}

	mmu = gpu->mmu;
	if (mmu) {
		ret = mmu->funcs->attach(mmu, iommu_ports,
				ARRAY_SIZE(iommu_ports));
		if (ret)
			return ret;
	}

	mutex_lock(&drm->struct_mutex);
	adreno_gpu->memptrs_bo = msm_gem_new(drm, sizeof(*adreno_gpu->memptrs),
			MSM_BO_UNCACHED);
	mutex_unlock(&drm->struct_mutex);
	if (IS_ERR(adreno_gpu->memptrs_bo)) {
		ret = PTR_ERR(adreno_gpu->memptrs_bo);
		adreno_gpu->memptrs_bo = NULL;
		dev_err(drm->dev, "could not allocate memptrs: %d\n", ret);
		return ret;
	}

<<<<<<< HEAD
	adreno_gpu->memptrs = msm_gem_vaddr(adreno_gpu->memptrs_bo);
	if (!adreno_gpu->memptrs) {
=======
	adreno_gpu->memptrs = msm_gem_get_vaddr(adreno_gpu->memptrs_bo);
	if (IS_ERR(adreno_gpu->memptrs)) {
>>>>>>> v4.9.227
		dev_err(drm->dev, "could not vmap memptrs\n");
		return -ENOMEM;
	}

	ret = msm_gem_get_iova(adreno_gpu->memptrs_bo, gpu->id,
			&adreno_gpu->memptrs_iova);
	if (ret) {
		dev_err(drm->dev, "could not map memptrs: %d\n", ret);
		return ret;
	}

<<<<<<< HEAD
	adreno_perfcounter_start(adreno_gpu);

=======
>>>>>>> v4.9.227
	return 0;
}

void adreno_gpu_cleanup(struct adreno_gpu *gpu)
{
	if (gpu->memptrs_bo) {
<<<<<<< HEAD
		if (gpu->memptrs_iova)
			msm_gem_put_iova(gpu->memptrs_bo, gpu->base.id);
=======
		if (gpu->memptrs)
			msm_gem_put_vaddr(gpu->memptrs_bo);

		if (gpu->memptrs_iova)
			msm_gem_put_iova(gpu->memptrs_bo, gpu->base.id);

>>>>>>> v4.9.227
		drm_gem_object_unreference_unlocked(gpu->memptrs_bo);
	}
	release_firmware(gpu->pm4);
	release_firmware(gpu->pfp);
<<<<<<< HEAD
	if (gpu->pm4_bo)
		msm_gem_free_object(gpu->pm4_bo);
	if (gpu->pfp_bo)
		msm_gem_free_object(gpu->pfp_bo);
	msm_gpu_cleanup(&gpu->base);
}

int adreno_perfcounter_read(struct msm_gpu *gpu,
	struct drm_perfcounter_read_group __user *reads, unsigned int count)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);

	return adreno_perfcounter_read_group(adreno_gpu, reads, count);
}

int adreno_perfcounter_query(struct msm_gpu *gpu, unsigned int groupid,
	unsigned int __user *countables, unsigned int count,
		unsigned int *max_counters)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);

	return adreno_perfcounter_query_group(adreno_gpu, groupid,
		&countables, count, &max_counters);
}

int adreno_perfcounter_msm_get(struct msm_gpu *gpu, unsigned int groupid,
	unsigned int countable, unsigned int *offset, unsigned int *offset_hi,
		unsigned int flags)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);

	return adreno_perfcounter_get(adreno_gpu, groupid, countable,
		&offset, &offset_hi, flags);
}

int adreno_perfcounter_msm_put(struct msm_gpu *gpu, unsigned int groupid,
	unsigned int countable, unsigned int flags)
{
	struct adreno_gpu *adreno_gpu = to_adreno_gpu(gpu);

	return adreno_perfcounter_put(adreno_gpu, groupid,
		countable, flags);
}
=======
	msm_gpu_cleanup(&gpu->base);
}
>>>>>>> v4.9.227
