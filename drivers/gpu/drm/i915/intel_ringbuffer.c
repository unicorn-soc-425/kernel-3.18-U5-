/*
 * Copyright © 2008-2010 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *    Zou Nan hai <nanhai.zou@intel.com>
 *    Xiang Hai hao<haihao.xiang@intel.com>
 *
 */

<<<<<<< HEAD
=======
#include <linux/log2.h>
>>>>>>> v4.9.227
#include <drm/drmP.h>
#include "i915_drv.h"
#include <drm/i915_drm.h>
#include "i915_trace.h"
#include "intel_drv.h"

<<<<<<< HEAD
bool
intel_ring_initialized(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;

	if (!dev)
		return false;

	if (i915.enable_execlists) {
		struct intel_context *dctx = ring->default_context;
		struct intel_ringbuffer *ringbuf = dctx->engine[ring->id].ringbuf;

		return ringbuf->obj;
	} else
		return ring->buffer && ring->buffer->obj;
}
=======
/* Rough estimate of the typical request size, performing a flush,
 * set-context and then emitting the batch.
 */
#define LEGACY_REQUEST_SIZE 200
>>>>>>> v4.9.227

int __intel_ring_space(int head, int tail, int size)
{
	int space = head - tail;
	if (space <= 0)
		space += size;
	return space - I915_RING_FREE_SPACE;
}

<<<<<<< HEAD
void intel_ring_update_space(struct intel_ringbuffer *ringbuf)
{
	if (ringbuf->last_retired_head != -1) {
		ringbuf->head = ringbuf->last_retired_head;
		ringbuf->last_retired_head = -1;
	}

	ringbuf->space = __intel_ring_space(ringbuf->head & HEAD_ADDR,
					    ringbuf->tail, ringbuf->size);
}

int intel_ring_space(struct intel_ringbuffer *ringbuf)
{
	intel_ring_update_space(ringbuf);
	return ringbuf->space;
}

bool intel_ring_stopped(struct intel_engine_cs *ring)
{
	struct drm_i915_private *dev_priv = ring->dev->dev_private;
	return dev_priv->gpu_error.stop_rings & intel_ring_flag(ring);
}

static void __intel_ring_advance(struct intel_engine_cs *ring)
{
	struct intel_ringbuffer *ringbuf = ring->buffer;
	ringbuf->tail &= ringbuf->size - 1;
	if (intel_ring_stopped(ring))
		return;
	ring->write_tail(ring, ringbuf->tail);
}

static int
gen2_render_ring_flush(struct drm_i915_gem_request *req,
		       u32	invalidate_domains,
		       u32	flush_domains)
{
	struct intel_engine_cs *ring = req->ring;
=======
void intel_ring_update_space(struct intel_ring *ring)
{
	if (ring->last_retired_head != -1) {
		ring->head = ring->last_retired_head;
		ring->last_retired_head = -1;
	}

	ring->space = __intel_ring_space(ring->head & HEAD_ADDR,
					 ring->tail, ring->size);
}

static int
gen2_render_ring_flush(struct drm_i915_gem_request *req, u32 mode)
{
	struct intel_ring *ring = req->ring;
>>>>>>> v4.9.227
	u32 cmd;
	int ret;

	cmd = MI_FLUSH;
<<<<<<< HEAD
	if (((invalidate_domains|flush_domains) & I915_GEM_DOMAIN_RENDER) == 0)
		cmd |= MI_NO_WRITE_FLUSH;

	if (invalidate_domains & I915_GEM_DOMAIN_SAMPLER)
=======

	if (mode & EMIT_INVALIDATE)
>>>>>>> v4.9.227
		cmd |= MI_READ_FLUSH;

	ret = intel_ring_begin(req, 2);
	if (ret)
		return ret;

	intel_ring_emit(ring, cmd);
	intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);

	return 0;
}

static int
<<<<<<< HEAD
gen4_render_ring_flush(struct drm_i915_gem_request *req,
		       u32	invalidate_domains,
		       u32	flush_domains)
{
	struct intel_engine_cs *ring = req->ring;
	struct drm_device *dev = ring->dev;
=======
gen4_render_ring_flush(struct drm_i915_gem_request *req, u32 mode)
{
	struct intel_ring *ring = req->ring;
>>>>>>> v4.9.227
	u32 cmd;
	int ret;

	/*
	 * read/write caches:
	 *
	 * I915_GEM_DOMAIN_RENDER is always invalidated, but is
	 * only flushed if MI_NO_WRITE_FLUSH is unset.  On 965, it is
	 * also flushed at 2d versus 3d pipeline switches.
	 *
	 * read-only caches:
	 *
	 * I915_GEM_DOMAIN_SAMPLER is flushed on pre-965 if
	 * MI_READ_FLUSH is set, and is always flushed on 965.
	 *
	 * I915_GEM_DOMAIN_COMMAND may not exist?
	 *
	 * I915_GEM_DOMAIN_INSTRUCTION, which exists on 965, is
	 * invalidated when MI_EXE_FLUSH is set.
	 *
	 * I915_GEM_DOMAIN_VERTEX, which exists on 965, is
	 * invalidated with every MI_FLUSH.
	 *
	 * TLBs:
	 *
	 * On 965, TLBs associated with I915_GEM_DOMAIN_COMMAND
	 * and I915_GEM_DOMAIN_CPU in are invalidated at PTE write and
	 * I915_GEM_DOMAIN_RENDER and I915_GEM_DOMAIN_SAMPLER
	 * are flushed at any MI_FLUSH.
	 */

<<<<<<< HEAD
	cmd = MI_FLUSH | MI_NO_WRITE_FLUSH;
	if ((invalidate_domains|flush_domains) & I915_GEM_DOMAIN_RENDER)
		cmd &= ~MI_NO_WRITE_FLUSH;
	if (invalidate_domains & I915_GEM_DOMAIN_INSTRUCTION)
		cmd |= MI_EXE_FLUSH;

	if (invalidate_domains & I915_GEM_DOMAIN_COMMAND &&
	    (IS_G4X(dev) || IS_GEN5(dev)))
		cmd |= MI_INVALIDATE_ISP;
=======
	cmd = MI_FLUSH;
	if (mode & EMIT_INVALIDATE) {
		cmd |= MI_EXE_FLUSH;
		if (IS_G4X(req->i915) || IS_GEN5(req->i915))
			cmd |= MI_INVALIDATE_ISP;
	}
>>>>>>> v4.9.227

	ret = intel_ring_begin(req, 2);
	if (ret)
		return ret;

	intel_ring_emit(ring, cmd);
	intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);

	return 0;
}

/**
 * Emits a PIPE_CONTROL with a non-zero post-sync operation, for
 * implementing two workarounds on gen6.  From section 1.4.7.1
 * "PIPE_CONTROL" of the Sandy Bridge PRM volume 2 part 1:
 *
 * [DevSNB-C+{W/A}] Before any depth stall flush (including those
 * produced by non-pipelined state commands), software needs to first
 * send a PIPE_CONTROL with no bits set except Post-Sync Operation !=
 * 0.
 *
 * [Dev-SNB{W/A}]: Before a PIPE_CONTROL with Write Cache Flush Enable
 * =1, a PIPE_CONTROL with any non-zero post-sync-op is required.
 *
 * And the workaround for these two requires this workaround first:
 *
 * [Dev-SNB{W/A}]: Pipe-control with CS-stall bit set must be sent
 * BEFORE the pipe-control with a post-sync op and no write-cache
 * flushes.
 *
 * And this last workaround is tricky because of the requirements on
 * that bit.  From section 1.4.7.2.3 "Stall" of the Sandy Bridge PRM
 * volume 2 part 1:
 *
 *     "1 of the following must also be set:
 *      - Render Target Cache Flush Enable ([12] of DW1)
 *      - Depth Cache Flush Enable ([0] of DW1)
 *      - Stall at Pixel Scoreboard ([1] of DW1)
 *      - Depth Stall ([13] of DW1)
 *      - Post-Sync Operation ([13] of DW1)
 *      - Notify Enable ([8] of DW1)"
 *
 * The cache flushes require the workaround flush that triggered this
 * one, so we can't use it.  Depth stall would trigger the same.
 * Post-sync nonzero is what triggered this second workaround, so we
 * can't use that one either.  Notify enable is IRQs, which aren't
 * really our business.  That leaves only stall at scoreboard.
 */
static int
intel_emit_post_sync_nonzero_flush(struct drm_i915_gem_request *req)
{
<<<<<<< HEAD
	struct intel_engine_cs *ring = req->ring;
	u32 scratch_addr = ring->scratch.gtt_offset + 2 * CACHELINE_BYTES;
=======
	struct intel_ring *ring = req->ring;
	u32 scratch_addr =
		i915_ggtt_offset(req->engine->scratch) + 2 * CACHELINE_BYTES;
>>>>>>> v4.9.227
	int ret;

	ret = intel_ring_begin(req, 6);
	if (ret)
		return ret;

	intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(5));
	intel_ring_emit(ring, PIPE_CONTROL_CS_STALL |
			PIPE_CONTROL_STALL_AT_SCOREBOARD);
<<<<<<< HEAD
	intel_ring_emit(ring, scratch_addr | PIPE_CONTROL_GLOBAL_GTT); /* address */
=======
	intel_ring_emit(ring, scratch_addr | PIPE_CONTROL_GLOBAL_GTT);
>>>>>>> v4.9.227
	intel_ring_emit(ring, 0); /* low dword */
	intel_ring_emit(ring, 0); /* high dword */
	intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);

	ret = intel_ring_begin(req, 6);
	if (ret)
		return ret;

	intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(5));
	intel_ring_emit(ring, PIPE_CONTROL_QW_WRITE);
<<<<<<< HEAD
	intel_ring_emit(ring, scratch_addr | PIPE_CONTROL_GLOBAL_GTT); /* address */
=======
	intel_ring_emit(ring, scratch_addr | PIPE_CONTROL_GLOBAL_GTT);
>>>>>>> v4.9.227
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);

	return 0;
}

static int
<<<<<<< HEAD
gen6_render_ring_flush(struct drm_i915_gem_request *req,
		       u32 invalidate_domains, u32 flush_domains)
{
	struct intel_engine_cs *ring = req->ring;
	u32 flags = 0;
	u32 scratch_addr = ring->scratch.gtt_offset + 2 * CACHELINE_BYTES;
=======
gen6_render_ring_flush(struct drm_i915_gem_request *req, u32 mode)
{
	struct intel_ring *ring = req->ring;
	u32 scratch_addr =
		i915_ggtt_offset(req->engine->scratch) + 2 * CACHELINE_BYTES;
	u32 flags = 0;
>>>>>>> v4.9.227
	int ret;

	/* Force SNB workarounds for PIPE_CONTROL flushes */
	ret = intel_emit_post_sync_nonzero_flush(req);
	if (ret)
		return ret;

	/* Just flush everything.  Experiments have shown that reducing the
	 * number of bits based on the write domains has little performance
	 * impact.
	 */
<<<<<<< HEAD
	if (flush_domains) {
=======
	if (mode & EMIT_FLUSH) {
>>>>>>> v4.9.227
		flags |= PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH;
		flags |= PIPE_CONTROL_DEPTH_CACHE_FLUSH;
		/*
		 * Ensure that any following seqno writes only happen
		 * when the render cache is indeed flushed.
		 */
		flags |= PIPE_CONTROL_CS_STALL;
	}
<<<<<<< HEAD
	if (invalidate_domains) {
=======
	if (mode & EMIT_INVALIDATE) {
>>>>>>> v4.9.227
		flags |= PIPE_CONTROL_TLB_INVALIDATE;
		flags |= PIPE_CONTROL_INSTRUCTION_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_VF_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_CONST_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_STATE_CACHE_INVALIDATE;
		/*
		 * TLB invalidate requires a post-sync write.
		 */
		flags |= PIPE_CONTROL_QW_WRITE | PIPE_CONTROL_CS_STALL;
	}

	ret = intel_ring_begin(req, 4);
	if (ret)
		return ret;

	intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(4));
	intel_ring_emit(ring, flags);
	intel_ring_emit(ring, scratch_addr | PIPE_CONTROL_GLOBAL_GTT);
	intel_ring_emit(ring, 0);
	intel_ring_advance(ring);

	return 0;
}

static int
gen7_render_ring_cs_stall_wa(struct drm_i915_gem_request *req)
{
<<<<<<< HEAD
	struct intel_engine_cs *ring = req->ring;
=======
	struct intel_ring *ring = req->ring;
>>>>>>> v4.9.227
	int ret;

	ret = intel_ring_begin(req, 4);
	if (ret)
		return ret;

	intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(4));
<<<<<<< HEAD
	intel_ring_emit(ring, PIPE_CONTROL_CS_STALL |
			      PIPE_CONTROL_STALL_AT_SCOREBOARD);
=======
	intel_ring_emit(ring,
			PIPE_CONTROL_CS_STALL |
			PIPE_CONTROL_STALL_AT_SCOREBOARD);
>>>>>>> v4.9.227
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, 0);
	intel_ring_advance(ring);

	return 0;
}

static int
<<<<<<< HEAD
gen7_render_ring_flush(struct drm_i915_gem_request *req,
		       u32 invalidate_domains, u32 flush_domains)
{
	struct intel_engine_cs *ring = req->ring;
	u32 flags = 0;
	u32 scratch_addr = ring->scratch.gtt_offset + 2 * CACHELINE_BYTES;
=======
gen7_render_ring_flush(struct drm_i915_gem_request *req, u32 mode)
{
	struct intel_ring *ring = req->ring;
	u32 scratch_addr =
		i915_ggtt_offset(req->engine->scratch) + 2 * CACHELINE_BYTES;
	u32 flags = 0;
>>>>>>> v4.9.227
	int ret;

	/*
	 * Ensure that any following seqno writes only happen when the render
	 * cache is indeed flushed.
	 *
	 * Workaround: 4th PIPE_CONTROL command (except the ones with only
	 * read-cache invalidate bits set) must have the CS_STALL bit set. We
	 * don't try to be clever and just set it unconditionally.
	 */
	flags |= PIPE_CONTROL_CS_STALL;

	/* Just flush everything.  Experiments have shown that reducing the
	 * number of bits based on the write domains has little performance
	 * impact.
	 */
<<<<<<< HEAD
	if (flush_domains) {
=======
	if (mode & EMIT_FLUSH) {
>>>>>>> v4.9.227
		flags |= PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH;
		flags |= PIPE_CONTROL_DEPTH_CACHE_FLUSH;
		flags |= PIPE_CONTROL_DC_FLUSH_ENABLE;
		flags |= PIPE_CONTROL_FLUSH_ENABLE;
	}
<<<<<<< HEAD
	if (invalidate_domains) {
=======
	if (mode & EMIT_INVALIDATE) {
>>>>>>> v4.9.227
		flags |= PIPE_CONTROL_TLB_INVALIDATE;
		flags |= PIPE_CONTROL_INSTRUCTION_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_VF_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_CONST_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_STATE_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_MEDIA_STATE_CLEAR;
		/*
		 * TLB invalidate requires a post-sync write.
		 */
		flags |= PIPE_CONTROL_QW_WRITE;
		flags |= PIPE_CONTROL_GLOBAL_GTT_IVB;

		flags |= PIPE_CONTROL_STALL_AT_SCOREBOARD;

		/* Workaround: we must issue a pipe_control with CS-stall bit
		 * set before a pipe_control command that has the state cache
		 * invalidate bit set. */
		gen7_render_ring_cs_stall_wa(req);
	}

	ret = intel_ring_begin(req, 4);
	if (ret)
		return ret;

	intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(4));
	intel_ring_emit(ring, flags);
	intel_ring_emit(ring, scratch_addr);
	intel_ring_emit(ring, 0);
	intel_ring_advance(ring);

	return 0;
}

static int
gen8_emit_pipe_control(struct drm_i915_gem_request *req,
		       u32 flags, u32 scratch_addr)
{
<<<<<<< HEAD
	struct intel_engine_cs *ring = req->ring;
=======
	struct intel_ring *ring = req->ring;
>>>>>>> v4.9.227
	int ret;

	ret = intel_ring_begin(req, 6);
	if (ret)
		return ret;

	intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(6));
	intel_ring_emit(ring, flags);
	intel_ring_emit(ring, scratch_addr);
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, 0);
	intel_ring_advance(ring);

	return 0;
}

static int
<<<<<<< HEAD
gen8_render_ring_flush(struct drm_i915_gem_request *req,
		       u32 invalidate_domains, u32 flush_domains)
{
	u32 flags = 0;
	u32 scratch_addr = req->ring->scratch.gtt_offset + 2 * CACHELINE_BYTES;
=======
gen8_render_ring_flush(struct drm_i915_gem_request *req, u32 mode)
{
	u32 scratch_addr =
		i915_ggtt_offset(req->engine->scratch) + 2 * CACHELINE_BYTES;
	u32 flags = 0;
>>>>>>> v4.9.227
	int ret;

	flags |= PIPE_CONTROL_CS_STALL;

<<<<<<< HEAD
	if (flush_domains) {
=======
	if (mode & EMIT_FLUSH) {
>>>>>>> v4.9.227
		flags |= PIPE_CONTROL_RENDER_TARGET_CACHE_FLUSH;
		flags |= PIPE_CONTROL_DEPTH_CACHE_FLUSH;
		flags |= PIPE_CONTROL_DC_FLUSH_ENABLE;
		flags |= PIPE_CONTROL_FLUSH_ENABLE;
	}
<<<<<<< HEAD
	if (invalidate_domains) {
=======
	if (mode & EMIT_INVALIDATE) {
>>>>>>> v4.9.227
		flags |= PIPE_CONTROL_TLB_INVALIDATE;
		flags |= PIPE_CONTROL_INSTRUCTION_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_VF_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_CONST_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_STATE_CACHE_INVALIDATE;
		flags |= PIPE_CONTROL_QW_WRITE;
		flags |= PIPE_CONTROL_GLOBAL_GTT_IVB;

		/* WaCsStallBeforeStateCacheInvalidate:bdw,chv */
		ret = gen8_emit_pipe_control(req,
					     PIPE_CONTROL_CS_STALL |
					     PIPE_CONTROL_STALL_AT_SCOREBOARD,
					     0);
		if (ret)
			return ret;
	}

	return gen8_emit_pipe_control(req, flags, scratch_addr);
}

<<<<<<< HEAD
static void ring_write_tail(struct intel_engine_cs *ring,
			    u32 value)
{
	struct drm_i915_private *dev_priv = ring->dev->dev_private;
	I915_WRITE_TAIL(ring, value);
}

u64 intel_ring_get_active_head(struct intel_engine_cs *ring)
{
	struct drm_i915_private *dev_priv = ring->dev->dev_private;
	u64 acthd;

	if (INTEL_INFO(ring->dev)->gen >= 8)
		acthd = I915_READ64_2x32(RING_ACTHD(ring->mmio_base),
					 RING_ACTHD_UDW(ring->mmio_base));
	else if (INTEL_INFO(ring->dev)->gen >= 4)
		acthd = I915_READ(RING_ACTHD(ring->mmio_base));
=======
u64 intel_engine_get_active_head(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	u64 acthd;

	if (INTEL_GEN(dev_priv) >= 8)
		acthd = I915_READ64_2x32(RING_ACTHD(engine->mmio_base),
					 RING_ACTHD_UDW(engine->mmio_base));
	else if (INTEL_GEN(dev_priv) >= 4)
		acthd = I915_READ(RING_ACTHD(engine->mmio_base));
>>>>>>> v4.9.227
	else
		acthd = I915_READ(ACTHD);

	return acthd;
}

<<<<<<< HEAD
static void ring_setup_phys_status_page(struct intel_engine_cs *ring)
{
	struct drm_i915_private *dev_priv = ring->dev->dev_private;
	u32 addr;

	addr = dev_priv->status_page_dmah->busaddr;
	if (INTEL_INFO(ring->dev)->gen >= 4)
=======
static void ring_setup_phys_status_page(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	u32 addr;

	addr = dev_priv->status_page_dmah->busaddr;
	if (INTEL_GEN(dev_priv) >= 4)
>>>>>>> v4.9.227
		addr |= (dev_priv->status_page_dmah->busaddr >> 28) & 0xf0;
	I915_WRITE(HWS_PGA, addr);
}

<<<<<<< HEAD
static void intel_ring_setup_status_page(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = ring->dev->dev_private;
	u32 mmio = 0;
=======
static void intel_ring_setup_status_page(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	i915_reg_t mmio;
>>>>>>> v4.9.227

	/* The ring status page addresses are no longer next to the rest of
	 * the ring registers as of gen7.
	 */
<<<<<<< HEAD
	if (IS_GEN7(dev)) {
		switch (ring->id) {
=======
	if (IS_GEN7(dev_priv)) {
		switch (engine->id) {
>>>>>>> v4.9.227
		case RCS:
			mmio = RENDER_HWS_PGA_GEN7;
			break;
		case BCS:
			mmio = BLT_HWS_PGA_GEN7;
			break;
		/*
		 * VCS2 actually doesn't exist on Gen7. Only shut up
		 * gcc switch check warning
		 */
		case VCS2:
		case VCS:
			mmio = BSD_HWS_PGA_GEN7;
			break;
		case VECS:
			mmio = VEBOX_HWS_PGA_GEN7;
			break;
		}
<<<<<<< HEAD
	} else if (IS_GEN6(ring->dev)) {
		mmio = RING_HWS_PGA_GEN6(ring->mmio_base);
	} else {
		/* XXX: gen8 returns to sanity */
		mmio = RING_HWS_PGA(ring->mmio_base);
	}

	I915_WRITE(mmio, (u32)ring->status_page.gfx_addr);
=======
	} else if (IS_GEN6(dev_priv)) {
		mmio = RING_HWS_PGA_GEN6(engine->mmio_base);
	} else {
		/* XXX: gen8 returns to sanity */
		mmio = RING_HWS_PGA(engine->mmio_base);
	}

	I915_WRITE(mmio, engine->status_page.ggtt_offset);
>>>>>>> v4.9.227
	POSTING_READ(mmio);

	/*
	 * Flush the TLB for this page
	 *
	 * FIXME: These two bits have disappeared on gen8, so a question
	 * arises: do we still need this and if so how should we go about
	 * invalidating the TLB?
	 */
<<<<<<< HEAD
	if (INTEL_INFO(dev)->gen >= 6 && INTEL_INFO(dev)->gen < 8) {
		u32 reg = RING_INSTPM(ring->mmio_base);

		/* ring should be idle before issuing a sync flush*/
		WARN_ON((I915_READ_MODE(ring) & MODE_IDLE) == 0);
=======
	if (IS_GEN(dev_priv, 6, 7)) {
		i915_reg_t reg = RING_INSTPM(engine->mmio_base);

		/* ring should be idle before issuing a sync flush*/
		WARN_ON((I915_READ_MODE(engine) & MODE_IDLE) == 0);
>>>>>>> v4.9.227

		I915_WRITE(reg,
			   _MASKED_BIT_ENABLE(INSTPM_TLB_INVALIDATE |
					      INSTPM_SYNC_FLUSH));
<<<<<<< HEAD
		if (wait_for((I915_READ(reg) & INSTPM_SYNC_FLUSH) == 0,
			     1000))
			DRM_ERROR("%s: wait for SyncFlush to complete for TLB invalidation timed out\n",
				  ring->name);
	}
}

static bool stop_ring(struct intel_engine_cs *ring)
{
	struct drm_i915_private *dev_priv = to_i915(ring->dev);

	if (!IS_GEN2(ring->dev)) {
		I915_WRITE_MODE(ring, _MASKED_BIT_ENABLE(STOP_RING));
		if (wait_for((I915_READ_MODE(ring) & MODE_IDLE) != 0, 1000)) {
			DRM_ERROR("%s : timed out trying to stop ring\n", ring->name);
=======
		if (intel_wait_for_register(dev_priv,
					    reg, INSTPM_SYNC_FLUSH, 0,
					    1000))
			DRM_ERROR("%s: wait for SyncFlush to complete for TLB invalidation timed out\n",
				  engine->name);
	}
}

static bool stop_ring(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	if (INTEL_GEN(dev_priv) > 2) {
		I915_WRITE_MODE(engine, _MASKED_BIT_ENABLE(STOP_RING));
		if (intel_wait_for_register(dev_priv,
					    RING_MI_MODE(engine->mmio_base),
					    MODE_IDLE,
					    MODE_IDLE,
					    1000)) {
			DRM_ERROR("%s : timed out trying to stop ring\n",
				  engine->name);
>>>>>>> v4.9.227
			/* Sometimes we observe that the idle flag is not
			 * set even though the ring is empty. So double
			 * check before giving up.
			 */
<<<<<<< HEAD
			if (I915_READ_HEAD(ring) != I915_READ_TAIL(ring))
=======
			if (I915_READ_HEAD(engine) != I915_READ_TAIL(engine))
>>>>>>> v4.9.227
				return false;
		}
	}

<<<<<<< HEAD
	I915_WRITE_CTL(ring, 0);
	I915_WRITE_HEAD(ring, 0);
	ring->write_tail(ring, 0);

	if (!IS_GEN2(ring->dev)) {
		(void)I915_READ_CTL(ring);
		I915_WRITE_MODE(ring, _MASKED_BIT_DISABLE(STOP_RING));
	}

	return (I915_READ_HEAD(ring) & HEAD_ADDR) == 0;
}

static int init_ring_common(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_ringbuffer *ringbuf = ring->buffer;
	struct drm_i915_gem_object *obj = ringbuf->obj;
=======
	I915_WRITE_CTL(engine, 0);
	I915_WRITE_HEAD(engine, 0);
	I915_WRITE_TAIL(engine, 0);

	if (INTEL_GEN(dev_priv) > 2) {
		(void)I915_READ_CTL(engine);
		I915_WRITE_MODE(engine, _MASKED_BIT_DISABLE(STOP_RING));
	}

	return (I915_READ_HEAD(engine) & HEAD_ADDR) == 0;
}

static int init_ring_common(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	struct intel_ring *ring = engine->buffer;
>>>>>>> v4.9.227
	int ret = 0;

	intel_uncore_forcewake_get(dev_priv, FORCEWAKE_ALL);

<<<<<<< HEAD
	if (!stop_ring(ring)) {
		/* G45 ring initialization often fails to reset head to zero */
		DRM_DEBUG_KMS("%s head not reset to zero "
			      "ctl %08x head %08x tail %08x start %08x\n",
			      ring->name,
			      I915_READ_CTL(ring),
			      I915_READ_HEAD(ring),
			      I915_READ_TAIL(ring),
			      I915_READ_START(ring));

		if (!stop_ring(ring)) {
			DRM_ERROR("failed to set %s head to zero "
				  "ctl %08x head %08x tail %08x start %08x\n",
				  ring->name,
				  I915_READ_CTL(ring),
				  I915_READ_HEAD(ring),
				  I915_READ_TAIL(ring),
				  I915_READ_START(ring));
=======
	if (!stop_ring(engine)) {
		/* G45 ring initialization often fails to reset head to zero */
		DRM_DEBUG_KMS("%s head not reset to zero "
			      "ctl %08x head %08x tail %08x start %08x\n",
			      engine->name,
			      I915_READ_CTL(engine),
			      I915_READ_HEAD(engine),
			      I915_READ_TAIL(engine),
			      I915_READ_START(engine));

		if (!stop_ring(engine)) {
			DRM_ERROR("failed to set %s head to zero "
				  "ctl %08x head %08x tail %08x start %08x\n",
				  engine->name,
				  I915_READ_CTL(engine),
				  I915_READ_HEAD(engine),
				  I915_READ_TAIL(engine),
				  I915_READ_START(engine));
>>>>>>> v4.9.227
			ret = -EIO;
			goto out;
		}
	}

<<<<<<< HEAD
	if (I915_NEED_GFX_HWS(dev))
		intel_ring_setup_status_page(ring);
	else
		ring_setup_phys_status_page(ring);

	/* Enforce ordering by reading HEAD register back */
	I915_READ_HEAD(ring);
=======
	if (HWS_NEEDS_PHYSICAL(dev_priv))
		ring_setup_phys_status_page(engine);
	else
		intel_ring_setup_status_page(engine);

	intel_engine_reset_breadcrumbs(engine);

	/* Enforce ordering by reading HEAD register back */
	I915_READ_HEAD(engine);
>>>>>>> v4.9.227

	/* Initialize the ring. This must happen _after_ we've cleared the ring
	 * registers with the above sequence (the readback of the HEAD registers
	 * also enforces ordering), otherwise the hw might lose the new ring
	 * register values. */
<<<<<<< HEAD
	I915_WRITE_START(ring, i915_gem_obj_ggtt_offset(obj));

	/* WaClearRingBufHeadRegAtInit:ctg,elk */
	if (I915_READ_HEAD(ring))
		DRM_DEBUG("%s initialization failed [head=%08x], fudging\n",
			  ring->name, I915_READ_HEAD(ring));
	I915_WRITE_HEAD(ring, 0);
	(void)I915_READ_HEAD(ring);

	I915_WRITE_CTL(ring,
			((ringbuf->size - PAGE_SIZE) & RING_NR_PAGES)
			| RING_VALID);

	/* If the head is still not zero, the ring is dead */
	if (wait_for((I915_READ_CTL(ring) & RING_VALID) != 0 &&
		     I915_READ_START(ring) == i915_gem_obj_ggtt_offset(obj) &&
		     (I915_READ_HEAD(ring) & HEAD_ADDR) == 0, 50)) {
		DRM_ERROR("%s initialization failed "
			  "ctl %08x (valid? %d) head %08x tail %08x start %08x [expected %08lx]\n",
			  ring->name,
			  I915_READ_CTL(ring), I915_READ_CTL(ring) & RING_VALID,
			  I915_READ_HEAD(ring), I915_READ_TAIL(ring),
			  I915_READ_START(ring), (unsigned long)i915_gem_obj_ggtt_offset(obj));
=======
	I915_WRITE_START(engine, i915_ggtt_offset(ring->vma));

	/* WaClearRingBufHeadRegAtInit:ctg,elk */
	if (I915_READ_HEAD(engine))
		DRM_DEBUG("%s initialization failed [head=%08x], fudging\n",
			  engine->name, I915_READ_HEAD(engine));

	intel_ring_update_space(ring);
	I915_WRITE_HEAD(engine, ring->head);
	I915_WRITE_TAIL(engine, ring->tail);
	(void)I915_READ_TAIL(engine);

	I915_WRITE_CTL(engine,
			((ring->size - PAGE_SIZE) & RING_NR_PAGES)
			| RING_VALID);

	/* If the head is still not zero, the ring is dead */
	if (intel_wait_for_register_fw(dev_priv, RING_CTL(engine->mmio_base),
				       RING_VALID, RING_VALID,
				       50)) {
		DRM_ERROR("%s initialization failed "
			  "ctl %08x (valid? %d) head %08x [%08x] tail %08x [%08x] start %08x [expected %08x]\n",
			  engine->name,
			  I915_READ_CTL(engine),
			  I915_READ_CTL(engine) & RING_VALID,
			  I915_READ_HEAD(engine), ring->head,
			  I915_READ_TAIL(engine), ring->tail,
			  I915_READ_START(engine),
			  i915_ggtt_offset(ring->vma));
>>>>>>> v4.9.227
		ret = -EIO;
		goto out;
	}

<<<<<<< HEAD
	ringbuf->last_retired_head = -1;
	ringbuf->head = I915_READ_HEAD(ring);
	ringbuf->tail = I915_READ_TAIL(ring) & TAIL_ADDR;
	intel_ring_update_space(ringbuf);

	memset(&ring->hangcheck, 0, sizeof(ring->hangcheck));
=======
	intel_engine_init_hangcheck(engine);
>>>>>>> v4.9.227

out:
	intel_uncore_forcewake_put(dev_priv, FORCEWAKE_ALL);

	return ret;
}

<<<<<<< HEAD
void
intel_fini_pipe_control(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;

	if (ring->scratch.obj == NULL)
		return;

	if (INTEL_INFO(dev)->gen >= 5) {
		kunmap(sg_page(ring->scratch.obj->pages->sgl));
		i915_gem_object_ggtt_unpin(ring->scratch.obj);
	}

	drm_gem_object_unreference(&ring->scratch.obj->base);
	ring->scratch.obj = NULL;
}

int
intel_init_pipe_control(struct intel_engine_cs *ring)
{
	int ret;

	WARN_ON(ring->scratch.obj);

	ring->scratch.obj = i915_gem_alloc_object(ring->dev, 4096);
	if (ring->scratch.obj == NULL) {
		DRM_ERROR("Failed to allocate seqno page\n");
		ret = -ENOMEM;
		goto err;
	}

	ret = i915_gem_object_set_cache_level(ring->scratch.obj, I915_CACHE_LLC);
	if (ret)
		goto err_unref;

	ret = i915_gem_obj_ggtt_pin(ring->scratch.obj, 4096, 0);
	if (ret)
		goto err_unref;

	ring->scratch.gtt_offset = i915_gem_obj_ggtt_offset(ring->scratch.obj);
	ring->scratch.cpu_page = kmap(sg_page(ring->scratch.obj->pages->sgl));
	if (ring->scratch.cpu_page == NULL) {
		ret = -ENOMEM;
		goto err_unpin;
	}

	DRM_DEBUG_DRIVER("%s pipe control offset: 0x%08x\n",
			 ring->name, ring->scratch.gtt_offset);
	return 0;

err_unpin:
	i915_gem_object_ggtt_unpin(ring->scratch.obj);
err_unref:
	drm_gem_object_unreference(&ring->scratch.obj->base);
err:
	return ret;
=======
static void reset_ring_common(struct intel_engine_cs *engine,
			      struct drm_i915_gem_request *request)
{
	struct intel_ring *ring = request->ring;

	ring->head = request->postfix;
	ring->last_retired_head = -1;
>>>>>>> v4.9.227
}

static int intel_ring_workarounds_emit(struct drm_i915_gem_request *req)
{
<<<<<<< HEAD
	int ret, i;
	struct intel_engine_cs *ring = req->ring;
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct i915_workarounds *w = &dev_priv->workarounds;
=======
	struct intel_ring *ring = req->ring;
	struct i915_workarounds *w = &req->i915->workarounds;
	int ret, i;
>>>>>>> v4.9.227

	if (w->count == 0)
		return 0;

<<<<<<< HEAD
	ring->gpu_caches_dirty = true;
	ret = intel_ring_flush_all_caches(req);
=======
	ret = req->engine->emit_flush(req, EMIT_BARRIER);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	ret = intel_ring_begin(req, (w->count * 2 + 2));
	if (ret)
		return ret;

	intel_ring_emit(ring, MI_LOAD_REGISTER_IMM(w->count));
	for (i = 0; i < w->count; i++) {
<<<<<<< HEAD
		intel_ring_emit(ring, w->reg[i].addr);
=======
		intel_ring_emit_reg(ring, w->reg[i].addr);
>>>>>>> v4.9.227
		intel_ring_emit(ring, w->reg[i].value);
	}
	intel_ring_emit(ring, MI_NOOP);

	intel_ring_advance(ring);

<<<<<<< HEAD
	ring->gpu_caches_dirty = true;
	ret = intel_ring_flush_all_caches(req);
=======
	ret = req->engine->emit_flush(req, EMIT_BARRIER);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	DRM_DEBUG_DRIVER("Number of Workarounds emitted: %d\n", w->count);

	return 0;
}

static int intel_rcs_ctx_init(struct drm_i915_gem_request *req)
{
	int ret;

	ret = intel_ring_workarounds_emit(req);
	if (ret != 0)
		return ret;

	ret = i915_gem_render_state_init(req);
	if (ret)
<<<<<<< HEAD
		DRM_ERROR("init render state: %d\n", ret);

	return ret;
}

static int wa_add(struct drm_i915_private *dev_priv,
		  const u32 addr, const u32 mask, const u32 val)
=======
		return ret;

	return 0;
}

static int wa_add(struct drm_i915_private *dev_priv,
		  i915_reg_t addr,
		  const u32 mask, const u32 val)
>>>>>>> v4.9.227
{
	const u32 idx = dev_priv->workarounds.count;

	if (WARN_ON(idx >= I915_MAX_WA_REGS))
		return -ENOSPC;

	dev_priv->workarounds.reg[idx].addr = addr;
	dev_priv->workarounds.reg[idx].value = val;
	dev_priv->workarounds.reg[idx].mask = mask;

	dev_priv->workarounds.count++;

	return 0;
}

#define WA_REG(addr, mask, val) do { \
		const int r = wa_add(dev_priv, (addr), (mask), (val)); \
		if (r) \
			return r; \
	} while (0)

#define WA_SET_BIT_MASKED(addr, mask) \
	WA_REG(addr, (mask), _MASKED_BIT_ENABLE(mask))

#define WA_CLR_BIT_MASKED(addr, mask) \
	WA_REG(addr, (mask), _MASKED_BIT_DISABLE(mask))

#define WA_SET_FIELD_MASKED(addr, mask, value) \
	WA_REG(addr, mask, _MASKED_FIELD(mask, value))

#define WA_SET_BIT(addr, mask) WA_REG(addr, mask, I915_READ(addr) | (mask))
#define WA_CLR_BIT(addr, mask) WA_REG(addr, mask, I915_READ(addr) & ~(mask))

#define WA_WRITE(addr, val) WA_REG(addr, 0xffffffff, val)

<<<<<<< HEAD
static int gen8_init_workarounds(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
=======
static int wa_ring_whitelist_reg(struct intel_engine_cs *engine,
				 i915_reg_t reg)
{
	struct drm_i915_private *dev_priv = engine->i915;
	struct i915_workarounds *wa = &dev_priv->workarounds;
	const uint32_t index = wa->hw_whitelist_count[engine->id];

	if (WARN_ON(index >= RING_MAX_NONPRIV_SLOTS))
		return -EINVAL;

	WA_WRITE(RING_FORCE_TO_NONPRIV(engine->mmio_base, index),
		 i915_mmio_reg_offset(reg));
	wa->hw_whitelist_count[engine->id]++;

	return 0;
}

static int gen8_init_workarounds(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
>>>>>>> v4.9.227

	WA_SET_BIT_MASKED(INSTPM, INSTPM_FORCE_ORDERING);

	/* WaDisableAsyncFlipPerfMode:bdw,chv */
	WA_SET_BIT_MASKED(MI_MODE, ASYNC_FLIP_PERF_DISABLE);

	/* WaDisablePartialInstShootdown:bdw,chv */
	WA_SET_BIT_MASKED(GEN8_ROW_CHICKEN,
			  PARTIAL_INSTRUCTION_SHOOTDOWN_DISABLE);

	/* Use Force Non-Coherent whenever executing a 3D context. This is a
	 * workaround for for a possible hang in the unlikely event a TLB
	 * invalidation occurs during a PSD flush.
	 */
	/* WaForceEnableNonCoherent:bdw,chv */
	/* WaHdcDisableFetchWhenMasked:bdw,chv */
	WA_SET_BIT_MASKED(HDC_CHICKEN0,
			  HDC_DONOT_FETCH_MEM_WHEN_MASKED |
			  HDC_FORCE_NON_COHERENT);

	/* From the Haswell PRM, Command Reference: Registers, CACHE_MODE_0:
	 * "The Hierarchical Z RAW Stall Optimization allows non-overlapping
	 *  polygons in the same 8x4 pixel/sample area to be processed without
	 *  stalling waiting for the earlier ones to write to Hierarchical Z
	 *  buffer."
	 *
	 * This optimization is off by default for BDW and CHV; turn it on.
	 */
	WA_CLR_BIT_MASKED(CACHE_MODE_0_GEN7, HIZ_RAW_STALL_OPT_DISABLE);

	/* Wa4x4STCOptimizationDisable:bdw,chv */
	WA_SET_BIT_MASKED(CACHE_MODE_1, GEN8_4x4_STC_OPTIMIZATION_DISABLE);

	/*
	 * BSpec recommends 8x4 when MSAA is used,
	 * however in practice 16x4 seems fastest.
	 *
	 * Note that PS/WM thread counts depend on the WIZ hashing
	 * disable bit, which we don't touch here, but it's good
	 * to keep in mind (see 3DSTATE_PS and 3DSTATE_WM).
	 */
	WA_SET_FIELD_MASKED(GEN7_GT_MODE,
			    GEN6_WIZ_HASHING_MASK,
			    GEN6_WIZ_HASHING_16x4);

	return 0;
}

<<<<<<< HEAD
static int bdw_init_workarounds(struct intel_engine_cs *ring)
{
	int ret;
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;

	ret = gen8_init_workarounds(ring);
=======
static int bdw_init_workarounds(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	int ret;

	ret = gen8_init_workarounds(engine);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* WaDisableThreadStallDopClockGating:bdw (pre-production) */
	WA_SET_BIT_MASKED(GEN8_ROW_CHICKEN, STALL_DOP_GATING_DISABLE);

	/* WaDisableDopClockGating:bdw */
	WA_SET_BIT_MASKED(GEN7_ROW_CHICKEN2,
			  DOP_CLOCK_GATING_DISABLE);

	WA_SET_BIT_MASKED(HALF_SLICE_CHICKEN3,
			  GEN8_SAMPLER_POWER_BYPASS_DIS);

	WA_SET_BIT_MASKED(HDC_CHICKEN0,
			  /* WaForceContextSaveRestoreNonCoherent:bdw */
			  HDC_FORCE_CONTEXT_SAVE_RESTORE_NON_COHERENT |
			  /* WaDisableFenceDestinationToSLM:bdw (pre-prod) */
<<<<<<< HEAD
			  (IS_BDW_GT3(dev) ? HDC_FENCE_DEST_SLM_DISABLE : 0));
=======
			  (IS_BDW_GT3(dev_priv) ? HDC_FENCE_DEST_SLM_DISABLE : 0));
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int chv_init_workarounds(struct intel_engine_cs *ring)
{
	int ret;
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;

	ret = gen8_init_workarounds(ring);
=======
static int chv_init_workarounds(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	int ret;

	ret = gen8_init_workarounds(engine);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* WaDisableThreadStallDopClockGating:chv */
	WA_SET_BIT_MASKED(GEN8_ROW_CHICKEN, STALL_DOP_GATING_DISABLE);

	/* Improve HiZ throughput on CHV. */
	WA_SET_BIT_MASKED(HIZ_CHICKEN, CHV_HZ_8X8_MODE_IN_1X);

	return 0;
}

<<<<<<< HEAD
static int gen9_init_workarounds(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	uint32_t tmp;

	/* WaEnableLbsSlaRetryTimerDecrement:skl */
	I915_WRITE(BDW_SCRATCH1, I915_READ(BDW_SCRATCH1) |
		   GEN9_LBS_SLA_RETRY_TIMER_DECREMENT_ENABLE);

	/* WaDisableKillLogic:bxt,skl */
	I915_WRITE(GAM_ECOCHK, I915_READ(GAM_ECOCHK) |
		   ECOCHK_DIS_TLB);

	/* WaDisablePartialInstShootdown:skl,bxt */
	WA_SET_BIT_MASKED(GEN8_ROW_CHICKEN,
			  PARTIAL_INSTRUCTION_SHOOTDOWN_DISABLE);

	/* Syncing dependencies between camera and graphics:skl,bxt */
	WA_SET_BIT_MASKED(HALF_SLICE_CHICKEN3,
			  GEN9_DISABLE_OCL_OOB_SUPPRESS_LOGIC);

	if ((IS_SKYLAKE(dev) && (INTEL_REVID(dev) == SKL_REVID_A0 ||
	    INTEL_REVID(dev) == SKL_REVID_B0)) ||
	    (IS_BROXTON(dev) && INTEL_REVID(dev) < BXT_REVID_B0)) {
		/* WaDisableDgMirrorFixInHalfSliceChicken5:skl,bxt */
		WA_CLR_BIT_MASKED(GEN9_HALF_SLICE_CHICKEN5,
				  GEN9_DG_MIRROR_FIX_ENABLE);
	}

	if ((IS_SKYLAKE(dev) && INTEL_REVID(dev) <= SKL_REVID_B0) ||
	    (IS_BROXTON(dev) && INTEL_REVID(dev) < BXT_REVID_B0)) {
		/* WaSetDisablePixMaskCammingAndRhwoInCommonSliceChicken:skl,bxt */
=======
static int gen9_init_workarounds(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	int ret;

	/* WaConextSwitchWithConcurrentTLBInvalidate:skl,bxt,kbl */
	I915_WRITE(GEN9_CSFE_CHICKEN1_RCS, _MASKED_BIT_ENABLE(GEN9_PREEMPT_GPGPU_SYNC_SWITCH_DISABLE));

	/* WaEnableLbsSlaRetryTimerDecrement:skl,bxt,kbl */
	I915_WRITE(BDW_SCRATCH1, I915_READ(BDW_SCRATCH1) |
		   GEN9_LBS_SLA_RETRY_TIMER_DECREMENT_ENABLE);

	/* WaDisableKillLogic:bxt,skl,kbl */
	I915_WRITE(GAM_ECOCHK, I915_READ(GAM_ECOCHK) |
		   ECOCHK_DIS_TLB);

	/* WaClearFlowControlGpgpuContextSave:skl,bxt,kbl */
	/* WaDisablePartialInstShootdown:skl,bxt,kbl */
	WA_SET_BIT_MASKED(GEN8_ROW_CHICKEN,
			  FLOW_CONTROL_ENABLE |
			  PARTIAL_INSTRUCTION_SHOOTDOWN_DISABLE);

	/* Syncing dependencies between camera and graphics:skl,bxt,kbl */
	WA_SET_BIT_MASKED(HALF_SLICE_CHICKEN3,
			  GEN9_DISABLE_OCL_OOB_SUPPRESS_LOGIC);

	/* WaDisableDgMirrorFixInHalfSliceChicken5:skl,bxt */
	if (IS_SKL_REVID(dev_priv, 0, SKL_REVID_B0) ||
	    IS_BXT_REVID(dev_priv, 0, BXT_REVID_A1))
		WA_CLR_BIT_MASKED(GEN9_HALF_SLICE_CHICKEN5,
				  GEN9_DG_MIRROR_FIX_ENABLE);

	/* WaSetDisablePixMaskCammingAndRhwoInCommonSliceChicken:skl,bxt */
	if (IS_SKL_REVID(dev_priv, 0, SKL_REVID_B0) ||
	    IS_BXT_REVID(dev_priv, 0, BXT_REVID_A1)) {
>>>>>>> v4.9.227
		WA_SET_BIT_MASKED(GEN7_COMMON_SLICE_CHICKEN1,
				  GEN9_RHWO_OPTIMIZATION_DISABLE);
		/*
		 * WA also requires GEN9_SLICE_COMMON_ECO_CHICKEN0[14:14] to be set
		 * but we do that in per ctx batchbuffer as there is an issue
		 * with this register not getting restored on ctx restore
		 */
	}

<<<<<<< HEAD
	if ((IS_SKYLAKE(dev) && INTEL_REVID(dev) >= SKL_REVID_C0) ||
	    IS_BROXTON(dev)) {
		/* WaEnableYV12BugFixInHalfSliceChicken7:skl,bxt */
		WA_SET_BIT_MASKED(GEN9_HALF_SLICE_CHICKEN7,
				  GEN9_ENABLE_YV12_BUGFIX);
	}

	/* Wa4x4STCOptimizationDisable:skl,bxt */
	/* WaDisablePartialResolveInVc:skl,bxt */
	WA_SET_BIT_MASKED(CACHE_MODE_1, (GEN8_4x4_STC_OPTIMIZATION_DISABLE |
					 GEN9_PARTIAL_RESOLVE_IN_VC_DISABLE));

	/* WaCcsTlbPrefetchDisable:skl,bxt */
=======
	/* WaEnableYV12BugFixInHalfSliceChicken7:skl,bxt,kbl */
	/* WaEnableSamplerGPGPUPreemptionSupport:skl,bxt,kbl */
	WA_SET_BIT_MASKED(GEN9_HALF_SLICE_CHICKEN7,
			  GEN9_ENABLE_YV12_BUGFIX |
			  GEN9_ENABLE_GPGPU_PREEMPTION);

	/* Wa4x4STCOptimizationDisable:skl,bxt,kbl */
	/* WaDisablePartialResolveInVc:skl,bxt,kbl */
	WA_SET_BIT_MASKED(CACHE_MODE_1, (GEN8_4x4_STC_OPTIMIZATION_DISABLE |
					 GEN9_PARTIAL_RESOLVE_IN_VC_DISABLE));

	/* WaCcsTlbPrefetchDisable:skl,bxt,kbl */
>>>>>>> v4.9.227
	WA_CLR_BIT_MASKED(GEN9_HALF_SLICE_CHICKEN5,
			  GEN9_CCS_TLB_PREFETCH_ENABLE);

	/* WaDisableMaskBasedCammingInRCC:skl,bxt */
<<<<<<< HEAD
	if ((IS_SKYLAKE(dev) && INTEL_REVID(dev) == SKL_REVID_C0) ||
	    (IS_BROXTON(dev) && INTEL_REVID(dev) < BXT_REVID_B0))
		WA_SET_BIT_MASKED(SLICE_ECO_CHICKEN0,
				  PIXEL_MASK_CAMMING_DISABLE);

	/* WaForceContextSaveRestoreNonCoherent:skl,bxt */
	tmp = HDC_FORCE_CONTEXT_SAVE_RESTORE_NON_COHERENT;
	if ((IS_SKYLAKE(dev) && INTEL_REVID(dev) == SKL_REVID_F0) ||
	    (IS_BROXTON(dev) && INTEL_REVID(dev) >= BXT_REVID_B0))
		tmp |= HDC_FORCE_CSR_NON_COHERENT_OVR_DISABLE;
	WA_SET_BIT_MASKED(HDC_CHICKEN0, tmp);

	/* WaDisableSamplerPowerBypassForSOPingPong:skl,bxt */
	if (IS_SKYLAKE(dev) ||
	    (IS_BROXTON(dev) && INTEL_REVID(dev) <= BXT_REVID_B0)) {
		WA_SET_BIT_MASKED(HALF_SLICE_CHICKEN3,
				  GEN8_SAMPLER_POWER_BYPASS_DIS);
	}

	/* WaDisableSTUnitPowerOptimization:skl,bxt */
	WA_SET_BIT_MASKED(HALF_SLICE_CHICKEN2, GEN8_ST_PO_DISABLE);

	return 0;
}

static int skl_tune_iz_hashing(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
=======
	if (IS_SKL_REVID(dev_priv, SKL_REVID_C0, SKL_REVID_C0) ||
	    IS_BXT_REVID(dev_priv, 0, BXT_REVID_A1))
		WA_SET_BIT_MASKED(SLICE_ECO_CHICKEN0,
				  PIXEL_MASK_CAMMING_DISABLE);

	/* WaForceContextSaveRestoreNonCoherent:skl,bxt,kbl */
	WA_SET_BIT_MASKED(HDC_CHICKEN0,
			  HDC_FORCE_CONTEXT_SAVE_RESTORE_NON_COHERENT |
			  HDC_FORCE_CSR_NON_COHERENT_OVR_DISABLE);

	/* WaForceEnableNonCoherent and WaDisableHDCInvalidation are
	 * both tied to WaForceContextSaveRestoreNonCoherent
	 * in some hsds for skl. We keep the tie for all gen9. The
	 * documentation is a bit hazy and so we want to get common behaviour,
	 * even though there is no clear evidence we would need both on kbl/bxt.
	 * This area has been source of system hangs so we play it safe
	 * and mimic the skl regardless of what bspec says.
	 *
	 * Use Force Non-Coherent whenever executing a 3D context. This
	 * is a workaround for a possible hang in the unlikely event
	 * a TLB invalidation occurs during a PSD flush.
	 */

	/* WaForceEnableNonCoherent:skl,bxt,kbl */
	WA_SET_BIT_MASKED(HDC_CHICKEN0,
			  HDC_FORCE_NON_COHERENT);

	/* WaDisableHDCInvalidation:skl,bxt,kbl */
	I915_WRITE(GAM_ECOCHK, I915_READ(GAM_ECOCHK) |
		   BDW_DISABLE_HDC_INVALIDATION);

	/* WaDisableSamplerPowerBypassForSOPingPong:skl,bxt,kbl */
	if (IS_SKYLAKE(dev_priv) ||
	    IS_KABYLAKE(dev_priv) ||
	    IS_BXT_REVID(dev_priv, 0, BXT_REVID_B0))
		WA_SET_BIT_MASKED(HALF_SLICE_CHICKEN3,
				  GEN8_SAMPLER_POWER_BYPASS_DIS);

	/* WaDisableSTUnitPowerOptimization:skl,bxt,kbl */
	WA_SET_BIT_MASKED(HALF_SLICE_CHICKEN2, GEN8_ST_PO_DISABLE);

	/* WaOCLCoherentLineFlush:skl,bxt,kbl */
	I915_WRITE(GEN8_L3SQCREG4, (I915_READ(GEN8_L3SQCREG4) |
				    GEN8_LQSC_FLUSH_COHERENT_LINES));

	/* WaVFEStateAfterPipeControlwithMediaStateClear:skl,bxt */
	ret = wa_ring_whitelist_reg(engine, GEN9_CTX_PREEMPT_REG);
	if (ret)
		return ret;

	/* WaEnablePreemptionGranularityControlByUMD:skl,bxt,kbl */
	ret= wa_ring_whitelist_reg(engine, GEN8_CS_CHICKEN1);
	if (ret)
		return ret;

	/* WaAllowUMDToModifyHDCChicken1:skl,bxt,kbl */
	ret = wa_ring_whitelist_reg(engine, GEN8_HDC_CHICKEN1);
	if (ret)
		return ret;

	return 0;
}

static int skl_tune_iz_hashing(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
>>>>>>> v4.9.227
	u8 vals[3] = { 0, 0, 0 };
	unsigned int i;

	for (i = 0; i < 3; i++) {
		u8 ss;

		/*
		 * Only consider slices where one, and only one, subslice has 7
		 * EUs
		 */
<<<<<<< HEAD
		if (hweight8(dev_priv->info.subslice_7eu[i]) != 1)
=======
		if (!is_power_of_2(INTEL_INFO(dev_priv)->sseu.subslice_7eu[i]))
>>>>>>> v4.9.227
			continue;

		/*
		 * subslice_7eu[i] != 0 (because of the check above) and
		 * ss_max == 4 (maximum number of subslices possible per slice)
		 *
		 * ->    0 <= ss <= 3;
		 */
<<<<<<< HEAD
		ss = ffs(dev_priv->info.subslice_7eu[i]) - 1;
=======
		ss = ffs(INTEL_INFO(dev_priv)->sseu.subslice_7eu[i]) - 1;
>>>>>>> v4.9.227
		vals[i] = 3 - ss;
	}

	if (vals[0] == 0 && vals[1] == 0 && vals[2] == 0)
		return 0;

	/* Tune IZ hashing. See intel_device_info_runtime_init() */
	WA_SET_FIELD_MASKED(GEN7_GT_MODE,
			    GEN9_IZ_HASHING_MASK(2) |
			    GEN9_IZ_HASHING_MASK(1) |
			    GEN9_IZ_HASHING_MASK(0),
			    GEN9_IZ_HASHING(2, vals[2]) |
			    GEN9_IZ_HASHING(1, vals[1]) |
			    GEN9_IZ_HASHING(0, vals[0]));

	return 0;
}

<<<<<<< HEAD
static int skl_init_workarounds(struct intel_engine_cs *ring)
{
	int ret;
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;

	ret = gen9_init_workarounds(ring);
	if (ret)
		return ret;

	if (INTEL_REVID(dev) <= SKL_REVID_D0) {
		/* WaDisableHDCInvalidation:skl */
		I915_WRITE(GAM_ECOCHK, I915_READ(GAM_ECOCHK) |
			   BDW_DISABLE_HDC_INVALIDATION);

=======
static int skl_init_workarounds(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	int ret;

	ret = gen9_init_workarounds(engine);
	if (ret)
		return ret;

	/*
	 * Actual WA is to disable percontext preemption granularity control
	 * until D0 which is the default case so this is equivalent to
	 * !WaDisablePerCtxtPreemptionGranularityControl:skl
	 */
	if (IS_SKL_REVID(dev_priv, SKL_REVID_E0, REVID_FOREVER)) {
		I915_WRITE(GEN7_FF_SLICE_CS_CHICKEN1,
			   _MASKED_BIT_ENABLE(GEN9_FFSC_PERCTX_PREEMPT_CTRL));
	}

	if (IS_SKL_REVID(dev_priv, 0, SKL_REVID_E0)) {
>>>>>>> v4.9.227
		/* WaDisableChickenBitTSGBarrierAckForFFSliceCS:skl */
		I915_WRITE(FF_SLICE_CS_CHICKEN2,
			   _MASKED_BIT_ENABLE(GEN9_TSG_BARRIER_ACK_DISABLE));
	}

	/* GEN8_L3SQCREG4 has a dependency with WA batch so any new changes
	 * involving this register should also be added to WA batch as required.
	 */
<<<<<<< HEAD
	if (INTEL_REVID(dev) <= SKL_REVID_E0)
=======
	if (IS_SKL_REVID(dev_priv, 0, SKL_REVID_E0))
>>>>>>> v4.9.227
		/* WaDisableLSQCROPERFforOCL:skl */
		I915_WRITE(GEN8_L3SQCREG4, I915_READ(GEN8_L3SQCREG4) |
			   GEN8_LQSC_RO_PERF_DIS);

	/* WaEnableGapsTsvCreditFix:skl */
<<<<<<< HEAD
	if (IS_SKYLAKE(dev) && (INTEL_REVID(dev) >= SKL_REVID_C0)) {
=======
	if (IS_SKL_REVID(dev_priv, SKL_REVID_C0, REVID_FOREVER)) {
>>>>>>> v4.9.227
		I915_WRITE(GEN8_GARBCNTL, (I915_READ(GEN8_GARBCNTL) |
					   GEN9_GAPS_TSV_CREDIT_DISABLE));
	}

	/* WaDisablePowerCompilerClockGating:skl */
<<<<<<< HEAD
	if (INTEL_REVID(dev) == SKL_REVID_B0)
		WA_SET_BIT_MASKED(HIZ_CHICKEN,
				  BDW_HIZ_POWER_COMPILER_CLOCK_GATING_DISABLE);

	if (INTEL_REVID(dev) <= SKL_REVID_D0) {
		/*
		 *Use Force Non-Coherent whenever executing a 3D context. This
		 * is a workaround for a possible hang in the unlikely event
		 * a TLB invalidation occurs during a PSD flush.
		 */
		/* WaForceEnableNonCoherent:skl */
		WA_SET_BIT_MASKED(HDC_CHICKEN0,
				  HDC_FORCE_NON_COHERENT);
	}

	if (INTEL_REVID(dev) == SKL_REVID_C0 ||
	    INTEL_REVID(dev) == SKL_REVID_D0)
		/* WaBarrierPerformanceFixDisable:skl */
=======
	if (IS_SKL_REVID(dev_priv, SKL_REVID_B0, SKL_REVID_B0))
		WA_SET_BIT_MASKED(HIZ_CHICKEN,
				  BDW_HIZ_POWER_COMPILER_CLOCK_GATING_DISABLE);

	/* WaBarrierPerformanceFixDisable:skl */
	if (IS_SKL_REVID(dev_priv, SKL_REVID_C0, SKL_REVID_D0))
>>>>>>> v4.9.227
		WA_SET_BIT_MASKED(HDC_CHICKEN0,
				  HDC_FENCE_DEST_SLM_DISABLE |
				  HDC_BARRIER_PERFORMANCE_DISABLE);

	/* WaDisableSbeCacheDispatchPortSharing:skl */
<<<<<<< HEAD
	if (INTEL_REVID(dev) <= SKL_REVID_F0) {
		WA_SET_BIT_MASKED(
			GEN7_HALF_SLICE_CHICKEN1,
			GEN7_SBE_SS_CACHE_DISPATCH_PORT_SHARING_DISABLE);
	}

	return skl_tune_iz_hashing(ring);
}

static int bxt_init_workarounds(struct intel_engine_cs *ring)
{
	int ret;
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;

	ret = gen9_init_workarounds(ring);
=======
	if (IS_SKL_REVID(dev_priv, 0, SKL_REVID_F0))
		WA_SET_BIT_MASKED(
			GEN7_HALF_SLICE_CHICKEN1,
			GEN7_SBE_SS_CACHE_DISPATCH_PORT_SHARING_DISABLE);

	/* WaDisableGafsUnitClkGating:skl */
	WA_SET_BIT(GEN7_UCGCTL4, GEN8_EU_GAUNIT_CLOCK_GATE_DISABLE);

	/* WaInPlaceDecompressionHang:skl */
	if (IS_SKL_REVID(dev_priv, SKL_REVID_H0, REVID_FOREVER))
		WA_SET_BIT(GEN9_GAMT_ECO_REG_RW_IA,
			   GAMT_ECO_ENABLE_IN_PLACE_DECOMPRESS);

	/* WaDisableLSQCROPERFforOCL:skl */
	ret = wa_ring_whitelist_reg(engine, GEN8_L3SQCREG4);
	if (ret)
		return ret;

	return skl_tune_iz_hashing(engine);
}

static int bxt_init_workarounds(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	int ret;

	ret = gen9_init_workarounds(engine);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* WaStoreMultiplePTEenable:bxt */
	/* This is a requirement according to Hardware specification */
<<<<<<< HEAD
	if (INTEL_REVID(dev) == BXT_REVID_A0)
		I915_WRITE(TILECTL, I915_READ(TILECTL) | TILECTL_TLBPF);

	/* WaSetClckGatingDisableMedia:bxt */
	if (INTEL_REVID(dev) == BXT_REVID_A0) {
=======
	if (IS_BXT_REVID(dev_priv, 0, BXT_REVID_A1))
		I915_WRITE(TILECTL, I915_READ(TILECTL) | TILECTL_TLBPF);

	/* WaSetClckGatingDisableMedia:bxt */
	if (IS_BXT_REVID(dev_priv, 0, BXT_REVID_A1)) {
>>>>>>> v4.9.227
		I915_WRITE(GEN7_MISCCPCTL, (I915_READ(GEN7_MISCCPCTL) &
					    ~GEN8_DOP_CLOCK_GATE_MEDIA_ENABLE));
	}

	/* WaDisableThreadStallDopClockGating:bxt */
	WA_SET_BIT_MASKED(GEN8_ROW_CHICKEN,
			  STALL_DOP_GATING_DISABLE);

<<<<<<< HEAD
	/* WaDisableSbeCacheDispatchPortSharing:bxt */
	if (INTEL_REVID(dev) <= BXT_REVID_B0) {
=======
	/* WaDisablePooledEuLoadBalancingFix:bxt */
	if (IS_BXT_REVID(dev_priv, BXT_REVID_B0, REVID_FOREVER)) {
		WA_SET_BIT_MASKED(FF_SLICE_CS_CHICKEN2,
				  GEN9_POOLED_EU_LOAD_BALANCING_FIX_DISABLE);
	}

	/* WaDisableSbeCacheDispatchPortSharing:bxt */
	if (IS_BXT_REVID(dev_priv, 0, BXT_REVID_B0)) {
>>>>>>> v4.9.227
		WA_SET_BIT_MASKED(
			GEN7_HALF_SLICE_CHICKEN1,
			GEN7_SBE_SS_CACHE_DISPATCH_PORT_SHARING_DISABLE);
	}

<<<<<<< HEAD
	return 0;
}

int init_workarounds_ring(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;

	WARN_ON(ring->id != RCS);

	dev_priv->workarounds.count = 0;

	if (IS_BROADWELL(dev))
		return bdw_init_workarounds(ring);

	if (IS_CHERRYVIEW(dev))
		return chv_init_workarounds(ring);

	if (IS_SKYLAKE(dev))
		return skl_init_workarounds(ring);

	if (IS_BROXTON(dev))
		return bxt_init_workarounds(ring);
=======
	/* WaDisableObjectLevelPreemptionForTrifanOrPolygon:bxt */
	/* WaDisableObjectLevelPreemptionForInstancedDraw:bxt */
	/* WaDisableObjectLevelPreemtionForInstanceId:bxt */
	/* WaDisableLSQCROPERFforOCL:bxt */
	if (IS_BXT_REVID(dev_priv, 0, BXT_REVID_A1)) {
		ret = wa_ring_whitelist_reg(engine, GEN9_CS_DEBUG_MODE1);
		if (ret)
			return ret;

		ret = wa_ring_whitelist_reg(engine, GEN8_L3SQCREG4);
		if (ret)
			return ret;
	}

	/* WaProgramL3SqcReg1DefaultForPerf:bxt */
	if (IS_BXT_REVID(dev_priv, BXT_REVID_B0, REVID_FOREVER))
		I915_WRITE(GEN8_L3SQCREG1, L3_GENERAL_PRIO_CREDITS(62) |
					   L3_HIGH_PRIO_CREDITS(2));

	/* WaToEnableHwFixForPushConstHWBug:bxt */
	if (IS_BXT_REVID(dev_priv, BXT_REVID_C0, REVID_FOREVER))
		WA_SET_BIT_MASKED(COMMON_SLICE_CHICKEN2,
				  GEN8_SBE_DISABLE_REPLAY_BUF_OPTIMIZATION);

	/* WaInPlaceDecompressionHang:bxt */
	if (IS_BXT_REVID(dev_priv, BXT_REVID_C0, REVID_FOREVER))
		WA_SET_BIT(GEN9_GAMT_ECO_REG_RW_IA,
			   GAMT_ECO_ENABLE_IN_PLACE_DECOMPRESS);

	return 0;
}

static int kbl_init_workarounds(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	int ret;

	ret = gen9_init_workarounds(engine);
	if (ret)
		return ret;

	/* WaEnableGapsTsvCreditFix:kbl */
	I915_WRITE(GEN8_GARBCNTL, (I915_READ(GEN8_GARBCNTL) |
				   GEN9_GAPS_TSV_CREDIT_DISABLE));

	/* WaDisableDynamicCreditSharing:kbl */
	if (IS_KBL_REVID(dev_priv, 0, KBL_REVID_B0))
		WA_SET_BIT(GAMT_CHKN_BIT_REG,
			   GAMT_CHKN_DISABLE_DYNAMIC_CREDIT_SHARING);

	/* WaDisableFenceDestinationToSLM:kbl (pre-prod) */
	if (IS_KBL_REVID(dev_priv, KBL_REVID_A0, KBL_REVID_A0))
		WA_SET_BIT_MASKED(HDC_CHICKEN0,
				  HDC_FENCE_DEST_SLM_DISABLE);

	/* WaToEnableHwFixForPushConstHWBug:kbl */
	if (IS_KBL_REVID(dev_priv, KBL_REVID_C0, REVID_FOREVER))
		WA_SET_BIT_MASKED(COMMON_SLICE_CHICKEN2,
				  GEN8_SBE_DISABLE_REPLAY_BUF_OPTIMIZATION);

	/* WaDisableGafsUnitClkGating:kbl */
	WA_SET_BIT(GEN7_UCGCTL4, GEN8_EU_GAUNIT_CLOCK_GATE_DISABLE);

	/* WaDisableSbeCacheDispatchPortSharing:kbl */
	WA_SET_BIT_MASKED(
		GEN7_HALF_SLICE_CHICKEN1,
		GEN7_SBE_SS_CACHE_DISPATCH_PORT_SHARING_DISABLE);

	/* WaInPlaceDecompressionHang:kbl */
	WA_SET_BIT(GEN9_GAMT_ECO_REG_RW_IA,
		   GAMT_ECO_ENABLE_IN_PLACE_DECOMPRESS);

	/* WaDisableLSQCROPERFforOCL:kbl */
	ret = wa_ring_whitelist_reg(engine, GEN8_L3SQCREG4);
	if (ret)
		return ret;

	return 0;
}

int init_workarounds_ring(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	WARN_ON(engine->id != RCS);

	dev_priv->workarounds.count = 0;
	dev_priv->workarounds.hw_whitelist_count[RCS] = 0;

	if (IS_BROADWELL(dev_priv))
		return bdw_init_workarounds(engine);

	if (IS_CHERRYVIEW(dev_priv))
		return chv_init_workarounds(engine);

	if (IS_SKYLAKE(dev_priv))
		return skl_init_workarounds(engine);

	if (IS_BROXTON(dev_priv))
		return bxt_init_workarounds(engine);

	if (IS_KABYLAKE(dev_priv))
		return kbl_init_workarounds(engine);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int init_render_ring(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	int ret = init_ring_common(ring);
=======
static int init_render_ring(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	int ret = init_ring_common(engine);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* WaTimedSingleVertexDispatch:cl,bw,ctg,elk,ilk,snb */
<<<<<<< HEAD
	if (INTEL_INFO(dev)->gen >= 4 && INTEL_INFO(dev)->gen < 7)
=======
	if (IS_GEN(dev_priv, 4, 6))
>>>>>>> v4.9.227
		I915_WRITE(MI_MODE, _MASKED_BIT_ENABLE(VS_TIMER_DISPATCH));

	/* We need to disable the AsyncFlip performance optimisations in order
	 * to use MI_WAIT_FOR_EVENT within the CS. It should already be
	 * programmed to '1' on all products.
	 *
	 * WaDisableAsyncFlipPerfMode:snb,ivb,hsw,vlv
	 */
<<<<<<< HEAD
	if (INTEL_INFO(dev)->gen >= 6 && INTEL_INFO(dev)->gen < 8)
=======
	if (IS_GEN(dev_priv, 6, 7))
>>>>>>> v4.9.227
		I915_WRITE(MI_MODE, _MASKED_BIT_ENABLE(ASYNC_FLIP_PERF_DISABLE));

	/* Required for the hardware to program scanline values for waiting */
	/* WaEnableFlushTlbInvalidationMode:snb */
<<<<<<< HEAD
	if (INTEL_INFO(dev)->gen == 6)
=======
	if (IS_GEN6(dev_priv))
>>>>>>> v4.9.227
		I915_WRITE(GFX_MODE,
			   _MASKED_BIT_ENABLE(GFX_TLB_INVALIDATE_EXPLICIT));

	/* WaBCSVCSTlbInvalidationMode:ivb,vlv,hsw */
<<<<<<< HEAD
	if (IS_GEN7(dev))
=======
	if (IS_GEN7(dev_priv))
>>>>>>> v4.9.227
		I915_WRITE(GFX_MODE_GEN7,
			   _MASKED_BIT_ENABLE(GFX_TLB_INVALIDATE_EXPLICIT) |
			   _MASKED_BIT_ENABLE(GFX_REPLAY_MODE));

<<<<<<< HEAD
	if (IS_GEN6(dev)) {
=======
	if (IS_GEN6(dev_priv)) {
>>>>>>> v4.9.227
		/* From the Sandybridge PRM, volume 1 part 3, page 24:
		 * "If this bit is set, STCunit will have LRA as replacement
		 *  policy. [...] This bit must be reset.  LRA replacement
		 *  policy is not supported."
		 */
		I915_WRITE(CACHE_MODE_0,
			   _MASKED_BIT_DISABLE(CM0_STC_EVICT_DISABLE_LRA_SNB));
	}

<<<<<<< HEAD
	if (INTEL_INFO(dev)->gen >= 6 && INTEL_INFO(dev)->gen < 8)
		I915_WRITE(INSTPM, _MASKED_BIT_ENABLE(INSTPM_FORCE_ORDERING));

	if (HAS_L3_DPF(dev))
		I915_WRITE_IMR(ring, ~GT_PARITY_ERROR(dev));

	return init_workarounds_ring(ring);
}

static void render_ring_cleanup(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;

	if (dev_priv->semaphore_obj) {
		i915_gem_object_ggtt_unpin(dev_priv->semaphore_obj);
		drm_gem_object_unreference(&dev_priv->semaphore_obj->base);
		dev_priv->semaphore_obj = NULL;
	}

	intel_fini_pipe_control(ring);
}

static int gen8_rcs_signal(struct drm_i915_gem_request *signaller_req,
			   unsigned int num_dwords)
{
#define MBOX_UPDATE_DWORDS 8
	struct intel_engine_cs *signaller = signaller_req->ring;
	struct drm_device *dev = signaller->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *waiter;
	int i, ret, num_rings;

	num_rings = hweight32(INTEL_INFO(dev)->ring_mask);
	num_dwords += (num_rings-1) * MBOX_UPDATE_DWORDS;
#undef MBOX_UPDATE_DWORDS

	ret = intel_ring_begin(signaller_req, num_dwords);
	if (ret)
		return ret;

	for_each_ring(waiter, dev_priv, i) {
		u32 seqno;
		u64 gtt_offset = signaller->semaphore.signal_ggtt[i];
		if (gtt_offset == MI_SEMAPHORE_SYNC_INVALID)
			continue;

		seqno = i915_gem_request_get_seqno(signaller_req);
		intel_ring_emit(signaller, GFX_OP_PIPE_CONTROL(6));
		intel_ring_emit(signaller, PIPE_CONTROL_GLOBAL_GTT_IVB |
					   PIPE_CONTROL_QW_WRITE |
					   PIPE_CONTROL_FLUSH_ENABLE);
		intel_ring_emit(signaller, lower_32_bits(gtt_offset));
		intel_ring_emit(signaller, upper_32_bits(gtt_offset));
		intel_ring_emit(signaller, seqno);
		intel_ring_emit(signaller, 0);
		intel_ring_emit(signaller, MI_SEMAPHORE_SIGNAL |
					   MI_SEMAPHORE_TARGET(waiter->id));
		intel_ring_emit(signaller, 0);
	}
=======
	if (IS_GEN(dev_priv, 6, 7))
		I915_WRITE(INSTPM, _MASKED_BIT_ENABLE(INSTPM_FORCE_ORDERING));

	if (INTEL_INFO(dev_priv)->gen >= 6)
		I915_WRITE_IMR(engine, ~engine->irq_keep_mask);

	return init_workarounds_ring(engine);
}

static void render_ring_cleanup(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	i915_vma_unpin_and_release(&dev_priv->semaphore);
}

static int gen8_rcs_signal(struct drm_i915_gem_request *req)
{
	struct intel_ring *ring = req->ring;
	struct drm_i915_private *dev_priv = req->i915;
	struct intel_engine_cs *waiter;
	enum intel_engine_id id;
	int ret, num_rings;

	num_rings = INTEL_INFO(dev_priv)->num_rings;
	ret = intel_ring_begin(req, (num_rings-1) * 8);
	if (ret)
		return ret;

	for_each_engine_id(waiter, dev_priv, id) {
		u64 gtt_offset = req->engine->semaphore.signal_ggtt[id];
		if (gtt_offset == MI_SEMAPHORE_SYNC_INVALID)
			continue;

		intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(6));
		intel_ring_emit(ring,
				PIPE_CONTROL_GLOBAL_GTT_IVB |
				PIPE_CONTROL_QW_WRITE |
				PIPE_CONTROL_CS_STALL);
		intel_ring_emit(ring, lower_32_bits(gtt_offset));
		intel_ring_emit(ring, upper_32_bits(gtt_offset));
		intel_ring_emit(ring, req->fence.seqno);
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring,
				MI_SEMAPHORE_SIGNAL |
				MI_SEMAPHORE_TARGET(waiter->hw_id));
		intel_ring_emit(ring, 0);
	}
	intel_ring_advance(ring);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int gen8_xcs_signal(struct drm_i915_gem_request *signaller_req,
			   unsigned int num_dwords)
{
#define MBOX_UPDATE_DWORDS 6
	struct intel_engine_cs *signaller = signaller_req->ring;
	struct drm_device *dev = signaller->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *waiter;
	int i, ret, num_rings;

	num_rings = hweight32(INTEL_INFO(dev)->ring_mask);
	num_dwords += (num_rings-1) * MBOX_UPDATE_DWORDS;
#undef MBOX_UPDATE_DWORDS

	ret = intel_ring_begin(signaller_req, num_dwords);
	if (ret)
		return ret;

	for_each_ring(waiter, dev_priv, i) {
		u32 seqno;
		u64 gtt_offset = signaller->semaphore.signal_ggtt[i];
		if (gtt_offset == MI_SEMAPHORE_SYNC_INVALID)
			continue;

		seqno = i915_gem_request_get_seqno(signaller_req);
		intel_ring_emit(signaller, (MI_FLUSH_DW + 1) |
					   MI_FLUSH_DW_OP_STOREDW);
		intel_ring_emit(signaller, lower_32_bits(gtt_offset) |
					   MI_FLUSH_DW_USE_GTT);
		intel_ring_emit(signaller, upper_32_bits(gtt_offset));
		intel_ring_emit(signaller, seqno);
		intel_ring_emit(signaller, MI_SEMAPHORE_SIGNAL |
					   MI_SEMAPHORE_TARGET(waiter->id));
		intel_ring_emit(signaller, 0);
	}
=======
static int gen8_xcs_signal(struct drm_i915_gem_request *req)
{
	struct intel_ring *ring = req->ring;
	struct drm_i915_private *dev_priv = req->i915;
	struct intel_engine_cs *waiter;
	enum intel_engine_id id;
	int ret, num_rings;

	num_rings = INTEL_INFO(dev_priv)->num_rings;
	ret = intel_ring_begin(req, (num_rings-1) * 6);
	if (ret)
		return ret;

	for_each_engine_id(waiter, dev_priv, id) {
		u64 gtt_offset = req->engine->semaphore.signal_ggtt[id];
		if (gtt_offset == MI_SEMAPHORE_SYNC_INVALID)
			continue;

		intel_ring_emit(ring,
				(MI_FLUSH_DW + 1) | MI_FLUSH_DW_OP_STOREDW);
		intel_ring_emit(ring,
				lower_32_bits(gtt_offset) |
				MI_FLUSH_DW_USE_GTT);
		intel_ring_emit(ring, upper_32_bits(gtt_offset));
		intel_ring_emit(ring, req->fence.seqno);
		intel_ring_emit(ring,
				MI_SEMAPHORE_SIGNAL |
				MI_SEMAPHORE_TARGET(waiter->hw_id));
		intel_ring_emit(ring, 0);
	}
	intel_ring_advance(ring);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int gen6_signal(struct drm_i915_gem_request *signaller_req,
		       unsigned int num_dwords)
{
	struct intel_engine_cs *signaller = signaller_req->ring;
	struct drm_device *dev = signaller->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *useless;
	int i, ret, num_rings;

#define MBOX_UPDATE_DWORDS 3
	num_rings = hweight32(INTEL_INFO(dev)->ring_mask);
	num_dwords += round_up((num_rings-1) * MBOX_UPDATE_DWORDS, 2);
#undef MBOX_UPDATE_DWORDS

	ret = intel_ring_begin(signaller_req, num_dwords);
	if (ret)
		return ret;

	for_each_ring(useless, dev_priv, i) {
		u32 mbox_reg = signaller->semaphore.mbox.signal[i];
		if (mbox_reg != GEN6_NOSYNC) {
			u32 seqno = i915_gem_request_get_seqno(signaller_req);
			intel_ring_emit(signaller, MI_LOAD_REGISTER_IMM(1));
			intel_ring_emit(signaller, mbox_reg);
			intel_ring_emit(signaller, seqno);
=======
static int gen6_signal(struct drm_i915_gem_request *req)
{
	struct intel_ring *ring = req->ring;
	struct drm_i915_private *dev_priv = req->i915;
	struct intel_engine_cs *engine;
	int ret, num_rings;

	num_rings = INTEL_INFO(dev_priv)->num_rings;
	ret = intel_ring_begin(req, round_up((num_rings-1) * 3, 2));
	if (ret)
		return ret;

	for_each_engine(engine, dev_priv) {
		i915_reg_t mbox_reg;

		if (!(BIT(engine->hw_id) & GEN6_SEMAPHORES_MASK))
			continue;

		mbox_reg = req->engine->semaphore.mbox.signal[engine->hw_id];
		if (i915_mmio_reg_valid(mbox_reg)) {
			intel_ring_emit(ring, MI_LOAD_REGISTER_IMM(1));
			intel_ring_emit_reg(ring, mbox_reg);
			intel_ring_emit(ring, req->fence.seqno);
>>>>>>> v4.9.227
		}
	}

	/* If num_dwords was rounded, make sure the tail pointer is correct */
	if (num_rings % 2 == 0)
<<<<<<< HEAD
		intel_ring_emit(signaller, MI_NOOP);
=======
		intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);

	return 0;
}

static void i9xx_submit_request(struct drm_i915_gem_request *request)
{
	struct drm_i915_private *dev_priv = request->i915;

	I915_WRITE_TAIL(request->engine,
			intel_ring_offset(request->ring, request->tail));
}

static int i9xx_emit_request(struct drm_i915_gem_request *req)
{
	struct intel_ring *ring = req->ring;
	int ret;

	ret = intel_ring_begin(req, 4);
	if (ret)
		return ret;

	intel_ring_emit(ring, MI_STORE_DWORD_INDEX);
	intel_ring_emit(ring, I915_GEM_HWS_INDEX << MI_STORE_DWORD_INDEX_SHIFT);
	intel_ring_emit(ring, req->fence.seqno);
	intel_ring_emit(ring, MI_USER_INTERRUPT);
	intel_ring_advance(ring);

	req->tail = ring->tail;
>>>>>>> v4.9.227

	return 0;
}

/**
<<<<<<< HEAD
 * gen6_add_request - Update the semaphore mailbox registers
=======
 * gen6_sema_emit_request - Update the semaphore mailbox registers
>>>>>>> v4.9.227
 *
 * @request - request to write to the ring
 *
 * Update the mailbox registers in the *other* rings with the current seqno.
 * This acts like a signal in the canonical semaphore.
 */
<<<<<<< HEAD
static int
gen6_add_request(struct drm_i915_gem_request *req)
{
	struct intel_engine_cs *ring = req->ring;
	int ret;

	if (ring->semaphore.signal)
		ret = ring->semaphore.signal(req, 4);
	else
		ret = intel_ring_begin(req, 4);

	if (ret)
		return ret;

	intel_ring_emit(ring, MI_STORE_DWORD_INDEX);
	intel_ring_emit(ring, I915_GEM_HWS_INDEX << MI_STORE_DWORD_INDEX_SHIFT);
	intel_ring_emit(ring, i915_gem_request_get_seqno(req));
	intel_ring_emit(ring, MI_USER_INTERRUPT);
	__intel_ring_advance(ring);

	return 0;
}

static inline bool i915_gem_has_seqno_wrapped(struct drm_device *dev,
					      u32 seqno)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	return dev_priv->last_seqno < seqno;
=======
static int gen6_sema_emit_request(struct drm_i915_gem_request *req)
{
	int ret;

	ret = req->engine->semaphore.signal(req);
	if (ret)
		return ret;

	return i9xx_emit_request(req);
}

static int gen8_render_emit_request(struct drm_i915_gem_request *req)
{
	struct intel_engine_cs *engine = req->engine;
	struct intel_ring *ring = req->ring;
	int ret;

	if (engine->semaphore.signal) {
		ret = engine->semaphore.signal(req);
		if (ret)
			return ret;
	}

	ret = intel_ring_begin(req, 8);
	if (ret)
		return ret;

	intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(6));
	intel_ring_emit(ring, (PIPE_CONTROL_GLOBAL_GTT_IVB |
			       PIPE_CONTROL_CS_STALL |
			       PIPE_CONTROL_QW_WRITE));
	intel_ring_emit(ring, intel_hws_seqno_address(engine));
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, i915_gem_request_get_seqno(req));
	/* We're thrashing one dword of HWS. */
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, MI_USER_INTERRUPT);
	intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);

	req->tail = ring->tail;

	return 0;
>>>>>>> v4.9.227
}

/**
 * intel_ring_sync - sync the waiter to the signaller on seqno
 *
 * @waiter - ring that is waiting
 * @signaller - ring which has, or will signal
 * @seqno - seqno which the waiter will block on
 */

static int
<<<<<<< HEAD
gen8_ring_sync(struct drm_i915_gem_request *waiter_req,
	       struct intel_engine_cs *signaller,
	       u32 seqno)
{
	struct intel_engine_cs *waiter = waiter_req->ring;
	struct drm_i915_private *dev_priv = waiter->dev->dev_private;
	int ret;

	ret = intel_ring_begin(waiter_req, 4);
	if (ret)
		return ret;

	intel_ring_emit(waiter, MI_SEMAPHORE_WAIT |
				MI_SEMAPHORE_GLOBAL_GTT |
				MI_SEMAPHORE_POLL |
				MI_SEMAPHORE_SAD_GTE_SDD);
	intel_ring_emit(waiter, seqno);
	intel_ring_emit(waiter,
			lower_32_bits(GEN8_WAIT_OFFSET(waiter, signaller->id)));
	intel_ring_emit(waiter,
			upper_32_bits(GEN8_WAIT_OFFSET(waiter, signaller->id)));
	intel_ring_advance(waiter);
=======
gen8_ring_sync_to(struct drm_i915_gem_request *req,
		  struct drm_i915_gem_request *signal)
{
	struct intel_ring *ring = req->ring;
	struct drm_i915_private *dev_priv = req->i915;
	u64 offset = GEN8_WAIT_OFFSET(req->engine, signal->engine->id);
	struct i915_hw_ppgtt *ppgtt;
	int ret;

	ret = intel_ring_begin(req, 4);
	if (ret)
		return ret;

	intel_ring_emit(ring,
			MI_SEMAPHORE_WAIT |
			MI_SEMAPHORE_GLOBAL_GTT |
			MI_SEMAPHORE_SAD_GTE_SDD);
	intel_ring_emit(ring, signal->fence.seqno);
	intel_ring_emit(ring, lower_32_bits(offset));
	intel_ring_emit(ring, upper_32_bits(offset));
	intel_ring_advance(ring);

	/* When the !RCS engines idle waiting upon a semaphore, they lose their
	 * pagetables and we must reload them before executing the batch.
	 * We do this on the i915_switch_context() following the wait and
	 * before the dispatch.
	 */
	ppgtt = req->ctx->ppgtt;
	if (ppgtt && req->engine->id != RCS)
		ppgtt->pd_dirty_rings |= intel_engine_flag(req->engine);
>>>>>>> v4.9.227
	return 0;
}

static int
<<<<<<< HEAD
gen6_ring_sync(struct drm_i915_gem_request *waiter_req,
	       struct intel_engine_cs *signaller,
	       u32 seqno)
{
	struct intel_engine_cs *waiter = waiter_req->ring;
	u32 dw1 = MI_SEMAPHORE_MBOX |
		  MI_SEMAPHORE_COMPARE |
		  MI_SEMAPHORE_REGISTER;
	u32 wait_mbox = signaller->semaphore.mbox.wait[waiter->id];
	int ret;

=======
gen6_ring_sync_to(struct drm_i915_gem_request *req,
		  struct drm_i915_gem_request *signal)
{
	struct intel_ring *ring = req->ring;
	u32 dw1 = MI_SEMAPHORE_MBOX |
		  MI_SEMAPHORE_COMPARE |
		  MI_SEMAPHORE_REGISTER;
	u32 wait_mbox = signal->engine->semaphore.mbox.wait[req->engine->hw_id];
	int ret;

	WARN_ON(wait_mbox == MI_SEMAPHORE_SYNC_INVALID);

	ret = intel_ring_begin(req, 4);
	if (ret)
		return ret;

	intel_ring_emit(ring, dw1 | wait_mbox);
>>>>>>> v4.9.227
	/* Throughout all of the GEM code, seqno passed implies our current
	 * seqno is >= the last seqno executed. However for hardware the
	 * comparison is strictly greater than.
	 */
<<<<<<< HEAD
	seqno -= 1;

	WARN_ON(wait_mbox == MI_SEMAPHORE_SYNC_INVALID);

	ret = intel_ring_begin(waiter_req, 4);
	if (ret)
		return ret;

	/* If seqno wrap happened, omit the wait with no-ops */
	if (likely(!i915_gem_has_seqno_wrapped(waiter->dev, seqno))) {
		intel_ring_emit(waiter, dw1 | wait_mbox);
		intel_ring_emit(waiter, seqno);
		intel_ring_emit(waiter, 0);
		intel_ring_emit(waiter, MI_NOOP);
	} else {
		intel_ring_emit(waiter, MI_NOOP);
		intel_ring_emit(waiter, MI_NOOP);
		intel_ring_emit(waiter, MI_NOOP);
		intel_ring_emit(waiter, MI_NOOP);
	}
	intel_ring_advance(waiter);
=======
	intel_ring_emit(ring, signal->fence.seqno - 1);
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
#define PIPE_CONTROL_FLUSH(ring__, addr__)					\
do {									\
	intel_ring_emit(ring__, GFX_OP_PIPE_CONTROL(4) | PIPE_CONTROL_QW_WRITE |		\
		 PIPE_CONTROL_DEPTH_STALL);				\
	intel_ring_emit(ring__, (addr__) | PIPE_CONTROL_GLOBAL_GTT);			\
	intel_ring_emit(ring__, 0);							\
	intel_ring_emit(ring__, 0);							\
} while (0)

static int
pc_render_add_request(struct drm_i915_gem_request *req)
{
	struct intel_engine_cs *ring = req->ring;
	u32 scratch_addr = ring->scratch.gtt_offset + 2 * CACHELINE_BYTES;
	int ret;

	/* For Ironlake, MI_USER_INTERRUPT was deprecated and apparently
	 * incoherent with writes to memory, i.e. completely fubar,
	 * so we need to use PIPE_NOTIFY instead.
	 *
	 * However, we also need to workaround the qword write
	 * incoherence by flushing the 6 PIPE_NOTIFY buffers out to
	 * memory before requesting an interrupt.
	 */
	ret = intel_ring_begin(req, 32);
	if (ret)
		return ret;

	intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(4) | PIPE_CONTROL_QW_WRITE |
			PIPE_CONTROL_WRITE_FLUSH |
			PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE);
	intel_ring_emit(ring, ring->scratch.gtt_offset | PIPE_CONTROL_GLOBAL_GTT);
	intel_ring_emit(ring, i915_gem_request_get_seqno(req));
	intel_ring_emit(ring, 0);
	PIPE_CONTROL_FLUSH(ring, scratch_addr);
	scratch_addr += 2 * CACHELINE_BYTES; /* write to separate cachelines */
	PIPE_CONTROL_FLUSH(ring, scratch_addr);
	scratch_addr += 2 * CACHELINE_BYTES;
	PIPE_CONTROL_FLUSH(ring, scratch_addr);
	scratch_addr += 2 * CACHELINE_BYTES;
	PIPE_CONTROL_FLUSH(ring, scratch_addr);
	scratch_addr += 2 * CACHELINE_BYTES;
	PIPE_CONTROL_FLUSH(ring, scratch_addr);
	scratch_addr += 2 * CACHELINE_BYTES;
	PIPE_CONTROL_FLUSH(ring, scratch_addr);

	intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(4) | PIPE_CONTROL_QW_WRITE |
			PIPE_CONTROL_WRITE_FLUSH |
			PIPE_CONTROL_TEXTURE_CACHE_INVALIDATE |
			PIPE_CONTROL_NOTIFY);
	intel_ring_emit(ring, ring->scratch.gtt_offset | PIPE_CONTROL_GLOBAL_GTT);
	intel_ring_emit(ring, i915_gem_request_get_seqno(req));
	intel_ring_emit(ring, 0);
	__intel_ring_advance(ring);

	return 0;
}

static u32
gen6_ring_get_seqno(struct intel_engine_cs *ring, bool lazy_coherency)
{
	/* Workaround to force correct ordering between irq and seqno writes on
	 * ivb (and maybe also on snb) by reading from a CS register (like
	 * ACTHD) before reading the status page. */
	if (!lazy_coherency) {
		struct drm_i915_private *dev_priv = ring->dev->dev_private;
		POSTING_READ(RING_ACTHD(ring->mmio_base));
	}

	return intel_read_status_page(ring, I915_GEM_HWS_INDEX);
}

static u32
ring_get_seqno(struct intel_engine_cs *ring, bool lazy_coherency)
{
	return intel_read_status_page(ring, I915_GEM_HWS_INDEX);
}

static void
ring_set_seqno(struct intel_engine_cs *ring, u32 seqno)
{
	intel_write_status_page(ring, I915_GEM_HWS_INDEX, seqno);
}

static u32
pc_render_get_seqno(struct intel_engine_cs *ring, bool lazy_coherency)
{
	return ring->scratch.cpu_page[0];
}

static void
pc_render_set_seqno(struct intel_engine_cs *ring, u32 seqno)
{
	ring->scratch.cpu_page[0] = seqno;
}

static bool
gen5_ring_get_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	if (WARN_ON(!intel_irqs_enabled(dev_priv)))
		return false;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (ring->irq_refcount++ == 0)
		gen5_enable_gt_irq(dev_priv, ring->irq_enable_mask);
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);

	return true;
}

static void
gen5_ring_put_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (--ring->irq_refcount == 0)
		gen5_disable_gt_irq(dev_priv, ring->irq_enable_mask);
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);
}

static bool
i9xx_ring_get_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	if (!intel_irqs_enabled(dev_priv))
		return false;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (ring->irq_refcount++ == 0) {
		dev_priv->irq_mask &= ~ring->irq_enable_mask;
		I915_WRITE(IMR, dev_priv->irq_mask);
		POSTING_READ(IMR);
	}
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);

	return true;
}

static void
i9xx_ring_put_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (--ring->irq_refcount == 0) {
		dev_priv->irq_mask |= ring->irq_enable_mask;
		I915_WRITE(IMR, dev_priv->irq_mask);
		POSTING_READ(IMR);
	}
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);
}

static bool
i8xx_ring_get_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	if (!intel_irqs_enabled(dev_priv))
		return false;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (ring->irq_refcount++ == 0) {
		dev_priv->irq_mask &= ~ring->irq_enable_mask;
		I915_WRITE16(IMR, dev_priv->irq_mask);
		POSTING_READ16(IMR);
	}
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);

	return true;
}

static void
i8xx_ring_put_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (--ring->irq_refcount == 0) {
		dev_priv->irq_mask |= ring->irq_enable_mask;
		I915_WRITE16(IMR, dev_priv->irq_mask);
		POSTING_READ16(IMR);
	}
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);
}

static int
bsd_ring_flush(struct drm_i915_gem_request *req,
	       u32     invalidate_domains,
	       u32     flush_domains)
{
	struct intel_engine_cs *ring = req->ring;
=======
static void
gen5_seqno_barrier(struct intel_engine_cs *engine)
{
	/* MI_STORE are internally buffered by the GPU and not flushed
	 * either by MI_FLUSH or SyncFlush or any other combination of
	 * MI commands.
	 *
	 * "Only the submission of the store operation is guaranteed.
	 * The write result will be complete (coherent) some time later
	 * (this is practically a finite period but there is no guaranteed
	 * latency)."
	 *
	 * Empirically, we observe that we need a delay of at least 75us to
	 * be sure that the seqno write is visible by the CPU.
	 */
	usleep_range(125, 250);
}

static void
gen6_seqno_barrier(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	/* Workaround to force correct ordering between irq and seqno writes on
	 * ivb (and maybe also on snb) by reading from a CS register (like
	 * ACTHD) before reading the status page.
	 *
	 * Note that this effectively stalls the read by the time it takes to
	 * do a memory transaction, which more or less ensures that the write
	 * from the GPU has sufficient time to invalidate the CPU cacheline.
	 * Alternatively we could delay the interrupt from the CS ring to give
	 * the write time to land, but that would incur a delay after every
	 * batch i.e. much more frequent than a delay when waiting for the
	 * interrupt (with the same net latency).
	 *
	 * Also note that to prevent whole machine hangs on gen7, we have to
	 * take the spinlock to guard against concurrent cacheline access.
	 */
	spin_lock_irq(&dev_priv->uncore.lock);
	POSTING_READ_FW(RING_ACTHD(engine->mmio_base));
	spin_unlock_irq(&dev_priv->uncore.lock);
}

static void
gen5_irq_enable(struct intel_engine_cs *engine)
{
	gen5_enable_gt_irq(engine->i915, engine->irq_enable_mask);
}

static void
gen5_irq_disable(struct intel_engine_cs *engine)
{
	gen5_disable_gt_irq(engine->i915, engine->irq_enable_mask);
}

static void
i9xx_irq_enable(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	dev_priv->irq_mask &= ~engine->irq_enable_mask;
	I915_WRITE(IMR, dev_priv->irq_mask);
	POSTING_READ_FW(RING_IMR(engine->mmio_base));
}

static void
i9xx_irq_disable(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	dev_priv->irq_mask |= engine->irq_enable_mask;
	I915_WRITE(IMR, dev_priv->irq_mask);
}

static void
i8xx_irq_enable(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	dev_priv->irq_mask &= ~engine->irq_enable_mask;
	I915_WRITE16(IMR, dev_priv->irq_mask);
	POSTING_READ16(RING_IMR(engine->mmio_base));
}

static void
i8xx_irq_disable(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	dev_priv->irq_mask |= engine->irq_enable_mask;
	I915_WRITE16(IMR, dev_priv->irq_mask);
}

static int
bsd_ring_flush(struct drm_i915_gem_request *req, u32 mode)
{
	struct intel_ring *ring = req->ring;
>>>>>>> v4.9.227
	int ret;

	ret = intel_ring_begin(req, 2);
	if (ret)
		return ret;

	intel_ring_emit(ring, MI_FLUSH);
	intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);
	return 0;
}

<<<<<<< HEAD
static int
i9xx_add_request(struct drm_i915_gem_request *req)
{
	struct intel_engine_cs *ring = req->ring;
	int ret;

	ret = intel_ring_begin(req, 4);
	if (ret)
		return ret;

	intel_ring_emit(ring, MI_STORE_DWORD_INDEX);
	intel_ring_emit(ring, I915_GEM_HWS_INDEX << MI_STORE_DWORD_INDEX_SHIFT);
	intel_ring_emit(ring, i915_gem_request_get_seqno(req));
	intel_ring_emit(ring, MI_USER_INTERRUPT);
	__intel_ring_advance(ring);

	return 0;
}

static bool
gen6_ring_get_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	if (WARN_ON(!intel_irqs_enabled(dev_priv)))
		return false;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (ring->irq_refcount++ == 0) {
		if (HAS_L3_DPF(dev) && ring->id == RCS)
			I915_WRITE_IMR(ring,
				       ~(ring->irq_enable_mask |
					 GT_PARITY_ERROR(dev)));
		else
			I915_WRITE_IMR(ring, ~ring->irq_enable_mask);
		gen5_enable_gt_irq(dev_priv, ring->irq_enable_mask);
	}
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);

	return true;
}

static void
gen6_ring_put_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (--ring->irq_refcount == 0) {
		if (HAS_L3_DPF(dev) && ring->id == RCS)
			I915_WRITE_IMR(ring, ~GT_PARITY_ERROR(dev));
		else
			I915_WRITE_IMR(ring, ~0);
		gen5_disable_gt_irq(dev_priv, ring->irq_enable_mask);
	}
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);
}

static bool
hsw_vebox_get_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	if (WARN_ON(!intel_irqs_enabled(dev_priv)))
		return false;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (ring->irq_refcount++ == 0) {
		I915_WRITE_IMR(ring, ~ring->irq_enable_mask);
		gen6_enable_pm_irq(dev_priv, ring->irq_enable_mask);
	}
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);

	return true;
}

static void
hsw_vebox_put_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (--ring->irq_refcount == 0) {
		I915_WRITE_IMR(ring, ~0);
		gen6_disable_pm_irq(dev_priv, ring->irq_enable_mask);
	}
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);
}

static bool
gen8_ring_get_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	if (WARN_ON(!intel_irqs_enabled(dev_priv)))
		return false;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (ring->irq_refcount++ == 0) {
		if (HAS_L3_DPF(dev) && ring->id == RCS) {
			I915_WRITE_IMR(ring,
				       ~(ring->irq_enable_mask |
					 GT_RENDER_L3_PARITY_ERROR_INTERRUPT));
		} else {
			I915_WRITE_IMR(ring, ~ring->irq_enable_mask);
		}
		POSTING_READ(RING_IMR(ring->mmio_base));
	}
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);

	return true;
}

static void
gen8_ring_put_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (--ring->irq_refcount == 0) {
		if (HAS_L3_DPF(dev) && ring->id == RCS) {
			I915_WRITE_IMR(ring,
				       ~GT_RENDER_L3_PARITY_ERROR_INTERRUPT);
		} else {
			I915_WRITE_IMR(ring, ~0);
		}
		POSTING_READ(RING_IMR(ring->mmio_base));
	}
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);
}

static int
i965_dispatch_execbuffer(struct drm_i915_gem_request *req,
			 u64 offset, u32 length,
			 unsigned dispatch_flags)
{
	struct intel_engine_cs *ring = req->ring;
=======
static void
gen6_irq_enable(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	I915_WRITE_IMR(engine,
		       ~(engine->irq_enable_mask |
			 engine->irq_keep_mask));
	gen5_enable_gt_irq(dev_priv, engine->irq_enable_mask);
}

static void
gen6_irq_disable(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	I915_WRITE_IMR(engine, ~engine->irq_keep_mask);
	gen5_disable_gt_irq(dev_priv, engine->irq_enable_mask);
}

static void
hsw_vebox_irq_enable(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	I915_WRITE_IMR(engine, ~engine->irq_enable_mask);
	gen6_enable_pm_irq(dev_priv, engine->irq_enable_mask);
}

static void
hsw_vebox_irq_disable(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	I915_WRITE_IMR(engine, ~0);
	gen6_disable_pm_irq(dev_priv, engine->irq_enable_mask);
}

static void
gen8_irq_enable(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	I915_WRITE_IMR(engine,
		       ~(engine->irq_enable_mask |
			 engine->irq_keep_mask));
	POSTING_READ_FW(RING_IMR(engine->mmio_base));
}

static void
gen8_irq_disable(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	I915_WRITE_IMR(engine, ~engine->irq_keep_mask);
}

static int
i965_emit_bb_start(struct drm_i915_gem_request *req,
		   u64 offset, u32 length,
		   unsigned int dispatch_flags)
{
	struct intel_ring *ring = req->ring;
>>>>>>> v4.9.227
	int ret;

	ret = intel_ring_begin(req, 2);
	if (ret)
		return ret;

	intel_ring_emit(ring,
			MI_BATCH_BUFFER_START |
			MI_BATCH_GTT |
			(dispatch_flags & I915_DISPATCH_SECURE ?
			 0 : MI_BATCH_NON_SECURE_I965));
	intel_ring_emit(ring, offset);
	intel_ring_advance(ring);

	return 0;
}

/* Just userspace ABI convention to limit the wa batch bo to a resonable size */
#define I830_BATCH_LIMIT (256*1024)
#define I830_TLB_ENTRIES (2)
#define I830_WA_SIZE max(I830_TLB_ENTRIES*4096, I830_BATCH_LIMIT)
static int
<<<<<<< HEAD
i830_dispatch_execbuffer(struct drm_i915_gem_request *req,
			 u64 offset, u32 len,
			 unsigned dispatch_flags)
{
	struct intel_engine_cs *ring = req->ring;
	u32 cs_offset = ring->scratch.gtt_offset;
=======
i830_emit_bb_start(struct drm_i915_gem_request *req,
		   u64 offset, u32 len,
		   unsigned int dispatch_flags)
{
	struct intel_ring *ring = req->ring;
	u32 cs_offset = i915_ggtt_offset(req->engine->scratch);
>>>>>>> v4.9.227
	int ret;

	ret = intel_ring_begin(req, 6);
	if (ret)
		return ret;

	/* Evict the invalid PTE TLBs */
	intel_ring_emit(ring, COLOR_BLT_CMD | BLT_WRITE_RGBA);
	intel_ring_emit(ring, BLT_DEPTH_32 | BLT_ROP_COLOR_COPY | 4096);
	intel_ring_emit(ring, I830_TLB_ENTRIES << 16 | 4); /* load each page */
	intel_ring_emit(ring, cs_offset);
	intel_ring_emit(ring, 0xdeadbeef);
	intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);

	if ((dispatch_flags & I915_DISPATCH_PINNED) == 0) {
		if (len > I830_BATCH_LIMIT)
			return -ENOSPC;

		ret = intel_ring_begin(req, 6 + 2);
		if (ret)
			return ret;

		/* Blit the batch (which has now all relocs applied) to the
		 * stable batch scratch bo area (so that the CS never
		 * stumbles over its tlb invalidation bug) ...
		 */
		intel_ring_emit(ring, SRC_COPY_BLT_CMD | BLT_WRITE_RGBA);
<<<<<<< HEAD
		intel_ring_emit(ring, BLT_DEPTH_32 | BLT_ROP_SRC_COPY | 4096);
=======
		intel_ring_emit(ring,
				BLT_DEPTH_32 | BLT_ROP_SRC_COPY | 4096);
>>>>>>> v4.9.227
		intel_ring_emit(ring, DIV_ROUND_UP(len, 4096) << 16 | 4096);
		intel_ring_emit(ring, cs_offset);
		intel_ring_emit(ring, 4096);
		intel_ring_emit(ring, offset);

		intel_ring_emit(ring, MI_FLUSH);
		intel_ring_emit(ring, MI_NOOP);
		intel_ring_advance(ring);

		/* ... and execute it. */
		offset = cs_offset;
	}

<<<<<<< HEAD
	ret = intel_ring_begin(req, 4);
	if (ret)
		return ret;

	intel_ring_emit(ring, MI_BATCH_BUFFER);
	intel_ring_emit(ring, offset | (dispatch_flags & I915_DISPATCH_SECURE ?
					0 : MI_BATCH_NON_SECURE));
	intel_ring_emit(ring, offset + len - 8);
	intel_ring_emit(ring, MI_NOOP);
=======
	ret = intel_ring_begin(req, 2);
	if (ret)
		return ret;

	intel_ring_emit(ring, MI_BATCH_BUFFER_START | MI_BATCH_GTT);
	intel_ring_emit(ring, offset | (dispatch_flags & I915_DISPATCH_SECURE ?
					0 : MI_BATCH_NON_SECURE));
>>>>>>> v4.9.227
	intel_ring_advance(ring);

	return 0;
}

static int
<<<<<<< HEAD
i915_dispatch_execbuffer(struct drm_i915_gem_request *req,
			 u64 offset, u32 len,
			 unsigned dispatch_flags)
{
	struct intel_engine_cs *ring = req->ring;
=======
i915_emit_bb_start(struct drm_i915_gem_request *req,
		   u64 offset, u32 len,
		   unsigned int dispatch_flags)
{
	struct intel_ring *ring = req->ring;
>>>>>>> v4.9.227
	int ret;

	ret = intel_ring_begin(req, 2);
	if (ret)
		return ret;

	intel_ring_emit(ring, MI_BATCH_BUFFER_START | MI_BATCH_GTT);
	intel_ring_emit(ring, offset | (dispatch_flags & I915_DISPATCH_SECURE ?
					0 : MI_BATCH_NON_SECURE));
	intel_ring_advance(ring);

	return 0;
}

<<<<<<< HEAD
static void cleanup_phys_status_page(struct intel_engine_cs *ring)
{
	struct drm_i915_private *dev_priv = to_i915(ring->dev);
=======
static void cleanup_phys_status_page(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
>>>>>>> v4.9.227

	if (!dev_priv->status_page_dmah)
		return;

<<<<<<< HEAD
	drm_pci_free(ring->dev, dev_priv->status_page_dmah);
	ring->status_page.page_addr = NULL;
}

static void cleanup_status_page(struct intel_engine_cs *ring)
{
	struct drm_i915_gem_object *obj;

	obj = ring->status_page.obj;
	if (obj == NULL)
		return;

	kunmap(sg_page(obj->pages->sgl));
	i915_gem_object_ggtt_unpin(obj);
	drm_gem_object_unreference(&obj->base);
	ring->status_page.obj = NULL;
}

static int init_status_page(struct intel_engine_cs *ring)
{
	struct drm_i915_gem_object *obj = ring->status_page.obj;

	if (obj == NULL) {
		unsigned flags;
		int ret;

		obj = i915_gem_alloc_object(ring->dev, 4096);
		if (obj == NULL) {
			DRM_ERROR("Failed to allocate status page\n");
			return -ENOMEM;
		}

		ret = i915_gem_object_set_cache_level(obj, I915_CACHE_LLC);
		if (ret)
			goto err_unref;

		flags = 0;
		if (!HAS_LLC(ring->dev))
			/* On g33, we cannot place HWS above 256MiB, so
			 * restrict its pinning to the low mappable arena.
			 * Though this restriction is not documented for
			 * gen4, gen5, or byt, they also behave similarly
			 * and hang if the HWS is placed at the top of the
			 * GTT. To generalise, it appears that all !llc
			 * platforms have issues with us placing the HWS
			 * above the mappable region (even though we never
			 * actualy map it).
			 */
			flags |= PIN_MAPPABLE;
		ret = i915_gem_obj_ggtt_pin(obj, 4096, flags);
		if (ret) {
err_unref:
			drm_gem_object_unreference(&obj->base);
			return ret;
		}

		ring->status_page.obj = obj;
	}

	ring->status_page.gfx_addr = i915_gem_obj_ggtt_offset(obj);
	ring->status_page.page_addr = kmap(sg_page(obj->pages->sgl));
	memset(ring->status_page.page_addr, 0, PAGE_SIZE);

	DRM_DEBUG_DRIVER("%s hws offset: 0x%08x\n",
			ring->name, ring->status_page.gfx_addr);

	return 0;
}

static int init_phys_status_page(struct intel_engine_cs *ring)
{
	struct drm_i915_private *dev_priv = ring->dev->dev_private;

	if (!dev_priv->status_page_dmah) {
		dev_priv->status_page_dmah =
			drm_pci_alloc(ring->dev, PAGE_SIZE, PAGE_SIZE);
		if (!dev_priv->status_page_dmah)
			return -ENOMEM;
	}

	ring->status_page.page_addr = dev_priv->status_page_dmah->vaddr;
	memset(ring->status_page.page_addr, 0, PAGE_SIZE);

	return 0;
}

void intel_unpin_ringbuffer_obj(struct intel_ringbuffer *ringbuf)
{
	iounmap(ringbuf->virtual_start);
	ringbuf->virtual_start = NULL;
	i915_gem_object_ggtt_unpin(ringbuf->obj);
}

int intel_pin_and_map_ringbuffer_obj(struct drm_device *dev,
				     struct intel_ringbuffer *ringbuf)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct drm_i915_gem_object *obj = ringbuf->obj;
	int ret;

	ret = i915_gem_obj_ggtt_pin(obj, PAGE_SIZE, PIN_MAPPABLE);
	if (ret)
		return ret;

	ret = i915_gem_object_set_to_gtt_domain(obj, true);
	if (ret) {
		i915_gem_object_ggtt_unpin(obj);
		return ret;
	}

	ringbuf->virtual_start = ioremap_wc(dev_priv->gtt.mappable_base +
			i915_gem_obj_ggtt_offset(obj), ringbuf->size);
	if (ringbuf->virtual_start == NULL) {
		i915_gem_object_ggtt_unpin(obj);
		return -EINVAL;
	}

	return 0;
}

static void intel_destroy_ringbuffer_obj(struct intel_ringbuffer *ringbuf)
{
	drm_gem_object_unreference(&ringbuf->obj->base);
	ringbuf->obj = NULL;
}

static int intel_alloc_ringbuffer_obj(struct drm_device *dev,
				      struct intel_ringbuffer *ringbuf)
{
	struct drm_i915_gem_object *obj;

	obj = NULL;
	if (!HAS_LLC(dev))
		obj = i915_gem_object_create_stolen(dev, ringbuf->size);
	if (obj == NULL)
		obj = i915_gem_alloc_object(dev, ringbuf->size);
	if (obj == NULL)
		return -ENOMEM;

	/* mark ring buffers as read-only from GPU side by default */
	obj->gt_ro = 1;

	ringbuf->obj = obj;
=======
	drm_pci_free(&dev_priv->drm, dev_priv->status_page_dmah);
	engine->status_page.page_addr = NULL;
}

static void cleanup_status_page(struct intel_engine_cs *engine)
{
	struct i915_vma *vma;

	vma = fetch_and_zero(&engine->status_page.vma);
	if (!vma)
		return;

	i915_vma_unpin(vma);
	i915_gem_object_unpin_map(vma->obj);
	i915_vma_put(vma);
}

static int init_status_page(struct intel_engine_cs *engine)
{
	struct drm_i915_gem_object *obj;
	struct i915_vma *vma;
	unsigned int flags;
	int ret;

	obj = i915_gem_object_create(&engine->i915->drm, 4096);
	if (IS_ERR(obj)) {
		DRM_ERROR("Failed to allocate status page\n");
		return PTR_ERR(obj);
	}

	ret = i915_gem_object_set_cache_level(obj, I915_CACHE_LLC);
	if (ret)
		goto err;

	vma = i915_vma_create(obj, &engine->i915->ggtt.base, NULL);
	if (IS_ERR(vma)) {
		ret = PTR_ERR(vma);
		goto err;
	}

	flags = PIN_GLOBAL;
	if (!HAS_LLC(engine->i915))
		/* On g33, we cannot place HWS above 256MiB, so
		 * restrict its pinning to the low mappable arena.
		 * Though this restriction is not documented for
		 * gen4, gen5, or byt, they also behave similarly
		 * and hang if the HWS is placed at the top of the
		 * GTT. To generalise, it appears that all !llc
		 * platforms have issues with us placing the HWS
		 * above the mappable region (even though we never
		 * actualy map it).
		 */
		flags |= PIN_MAPPABLE;
	ret = i915_vma_pin(vma, 0, 4096, flags);
	if (ret)
		goto err;

	engine->status_page.vma = vma;
	engine->status_page.ggtt_offset = i915_ggtt_offset(vma);
	engine->status_page.page_addr =
		i915_gem_object_pin_map(obj, I915_MAP_WB);

	DRM_DEBUG_DRIVER("%s hws offset: 0x%08x\n",
			 engine->name, i915_ggtt_offset(vma));
	return 0;

err:
	i915_gem_object_put(obj);
	return ret;
}

static int init_phys_status_page(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	dev_priv->status_page_dmah =
		drm_pci_alloc(&dev_priv->drm, PAGE_SIZE, PAGE_SIZE);
	if (!dev_priv->status_page_dmah)
		return -ENOMEM;

	engine->status_page.page_addr = dev_priv->status_page_dmah->vaddr;
	memset(engine->status_page.page_addr, 0, PAGE_SIZE);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
struct intel_ringbuffer *
intel_engine_create_ringbuffer(struct intel_engine_cs *engine, int size)
{
	struct intel_ringbuffer *ring;
	int ret;

	ring = kzalloc(sizeof(*ring), GFP_KERNEL);
	if (ring == NULL)
		return ERR_PTR(-ENOMEM);

	ring->ring = engine;
=======
int intel_ring_pin(struct intel_ring *ring)
{
	/* Ring wraparound at offset 0 sometimes hangs. No idea why. */
	unsigned int flags = PIN_GLOBAL | PIN_OFFSET_BIAS | 4096;
	enum i915_map_type map;
	struct i915_vma *vma = ring->vma;
	void *addr;
	int ret;

	GEM_BUG_ON(ring->vaddr);

	map = HAS_LLC(ring->engine->i915) ? I915_MAP_WB : I915_MAP_WC;

	if (vma->obj->stolen)
		flags |= PIN_MAPPABLE;

	if (!(vma->flags & I915_VMA_GLOBAL_BIND)) {
		if (flags & PIN_MAPPABLE || map == I915_MAP_WC)
			ret = i915_gem_object_set_to_gtt_domain(vma->obj, true);
		else
			ret = i915_gem_object_set_to_cpu_domain(vma->obj, true);
		if (unlikely(ret))
			return ret;
	}

	ret = i915_vma_pin(vma, 0, PAGE_SIZE, flags);
	if (unlikely(ret))
		return ret;

	if (i915_vma_is_map_and_fenceable(vma))
		addr = (void __force *)i915_vma_pin_iomap(vma);
	else
		addr = i915_gem_object_pin_map(vma->obj, map);
	if (IS_ERR(addr))
		goto err;

	ring->vaddr = addr;
	return 0;

err:
	i915_vma_unpin(vma);
	return PTR_ERR(addr);
}

void intel_ring_unpin(struct intel_ring *ring)
{
	GEM_BUG_ON(!ring->vma);
	GEM_BUG_ON(!ring->vaddr);

	if (i915_vma_is_map_and_fenceable(ring->vma))
		i915_vma_unpin_iomap(ring->vma);
	else
		i915_gem_object_unpin_map(ring->vma->obj);
	ring->vaddr = NULL;

	i915_vma_unpin(ring->vma);
}

static struct i915_vma *
intel_ring_create_vma(struct drm_i915_private *dev_priv, int size)
{
	struct i915_address_space *vm = &dev_priv->ggtt.base;
	struct drm_i915_gem_object *obj;
	struct i915_vma *vma;

	obj = i915_gem_object_create_stolen(&dev_priv->drm, size);
	if (!obj)
		obj = i915_gem_object_create(&dev_priv->drm, size);
	if (IS_ERR(obj))
		return ERR_CAST(obj);

	/*
	 * Mark ring buffers as read-only from GPU side (so no stray overwrites)
	 * if supported by the platform's GGTT.
	 */
	if (vm->has_read_only)
		i915_gem_object_set_readonly(obj);

	vma = i915_vma_create(obj, vm, NULL);
	if (IS_ERR(vma))
		goto err;

	return vma;

err:
	i915_gem_object_put(obj);
	return vma;
}

struct intel_ring *
intel_engine_create_ring(struct intel_engine_cs *engine, int size)
{
	struct intel_ring *ring;
	struct i915_vma *vma;

	GEM_BUG_ON(!is_power_of_2(size));

	ring = kzalloc(sizeof(*ring), GFP_KERNEL);
	if (!ring)
		return ERR_PTR(-ENOMEM);

	ring->engine = engine;

	INIT_LIST_HEAD(&ring->request_list);
>>>>>>> v4.9.227

	ring->size = size;
	/* Workaround an erratum on the i830 which causes a hang if
	 * the TAIL pointer points to within the last 2 cachelines
	 * of the buffer.
	 */
	ring->effective_size = size;
<<<<<<< HEAD
	if (IS_I830(engine->dev) || IS_845G(engine->dev))
=======
	if (IS_I830(engine->i915) || IS_845G(engine->i915))
>>>>>>> v4.9.227
		ring->effective_size -= 2 * CACHELINE_BYTES;

	ring->last_retired_head = -1;
	intel_ring_update_space(ring);

<<<<<<< HEAD
	ret = intel_alloc_ringbuffer_obj(engine->dev, ring);
	if (ret) {
		DRM_ERROR("Failed to allocate ringbuffer %s: %d\n",
			  engine->name, ret);
		kfree(ring);
		return ERR_PTR(ret);
	}
=======
	vma = intel_ring_create_vma(engine->i915, size);
	if (IS_ERR(vma)) {
		kfree(ring);
		return ERR_CAST(vma);
	}
	ring->vma = vma;
>>>>>>> v4.9.227

	return ring;
}

void
<<<<<<< HEAD
intel_ringbuffer_free(struct intel_ringbuffer *ring)
{
	intel_destroy_ringbuffer_obj(ring);
	kfree(ring);
}

static int intel_init_ring_buffer(struct drm_device *dev,
				  struct intel_engine_cs *ring)
{
	struct intel_ringbuffer *ringbuf;
	int ret;

	WARN_ON(ring->buffer);

	ring->dev = dev;
	INIT_LIST_HEAD(&ring->active_list);
	INIT_LIST_HEAD(&ring->request_list);
	INIT_LIST_HEAD(&ring->execlist_queue);
	i915_gem_batch_pool_init(dev, &ring->batch_pool);
	memset(ring->semaphore.sync_seqno, 0, sizeof(ring->semaphore.sync_seqno));

	init_waitqueue_head(&ring->irq_queue);

	ringbuf = intel_engine_create_ringbuffer(ring, 32 * PAGE_SIZE);
	if (IS_ERR(ringbuf))
		return PTR_ERR(ringbuf);
	ring->buffer = ringbuf;

	if (I915_NEED_GFX_HWS(dev)) {
		ret = init_status_page(ring);
		if (ret)
			goto error;
	} else {
		WARN_ON(ring->id != RCS);
		ret = init_phys_status_page(ring);
=======
intel_ring_free(struct intel_ring *ring)
{
	i915_vma_put(ring->vma);
	kfree(ring);
}

static int intel_ring_context_pin(struct i915_gem_context *ctx,
				  struct intel_engine_cs *engine)
{
	struct intel_context *ce = &ctx->engine[engine->id];
	int ret;

	lockdep_assert_held(&ctx->i915->drm.struct_mutex);

	if (ce->pin_count++)
		return 0;

	if (ce->state) {
		ret = i915_gem_object_set_to_gtt_domain(ce->state->obj, false);
		if (ret)
			goto error;

		ret = i915_vma_pin(ce->state, 0, ctx->ggtt_alignment,
				   PIN_GLOBAL | PIN_HIGH);
		if (ret)
			goto error;
	}

	/* The kernel context is only used as a placeholder for flushing the
	 * active context. It is never used for submitting user rendering and
	 * as such never requires the golden render context, and so we can skip
	 * emitting it when we switch to the kernel context. This is required
	 * as during eviction we cannot allocate and pin the renderstate in
	 * order to initialise the context.
	 */
	if (ctx == ctx->i915->kernel_context)
		ce->initialised = true;

	i915_gem_context_get(ctx);
	return 0;

error:
	ce->pin_count = 0;
	return ret;
}

static void intel_ring_context_unpin(struct i915_gem_context *ctx,
				     struct intel_engine_cs *engine)
{
	struct intel_context *ce = &ctx->engine[engine->id];

	lockdep_assert_held(&ctx->i915->drm.struct_mutex);

	if (--ce->pin_count)
		return;

	if (ce->state)
		i915_vma_unpin(ce->state);

	i915_gem_context_put(ctx);
}

static int intel_init_ring_buffer(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	struct intel_ring *ring;
	int ret;

	WARN_ON(engine->buffer);

	intel_engine_setup_common(engine);

	memset(engine->semaphore.sync_seqno, 0,
	       sizeof(engine->semaphore.sync_seqno));

	ret = intel_engine_init_common(engine);
	if (ret)
		goto error;

	/* We may need to do things with the shrinker which
	 * require us to immediately switch back to the default
	 * context. This can cause a problem as pinning the
	 * default context also requires GTT space which may not
	 * be available. To avoid this we always pin the default
	 * context.
	 */
	ret = intel_ring_context_pin(dev_priv->kernel_context, engine);
	if (ret)
		goto error;

	ring = intel_engine_create_ring(engine, 32 * PAGE_SIZE);
	if (IS_ERR(ring)) {
		ret = PTR_ERR(ring);
		goto error;
	}

	if (HWS_NEEDS_PHYSICAL(dev_priv)) {
		WARN_ON(engine->id != RCS);
		ret = init_phys_status_page(engine);
		if (ret)
			goto error;
	} else {
		ret = init_status_page(engine);
>>>>>>> v4.9.227
		if (ret)
			goto error;
	}

<<<<<<< HEAD
	ret = intel_pin_and_map_ringbuffer_obj(dev, ringbuf);
	if (ret) {
		DRM_ERROR("Failed to pin and map ringbuffer %s: %d\n",
				ring->name, ret);
		intel_destroy_ringbuffer_obj(ringbuf);
		goto error;
	}

	ret = i915_cmd_parser_init_ring(ring);
	if (ret)
		goto error;
=======
	ret = intel_ring_pin(ring);
	if (ret) {
		intel_ring_free(ring);
		goto error;
	}
	engine->buffer = ring;
>>>>>>> v4.9.227

	return 0;

error:
<<<<<<< HEAD
	intel_ringbuffer_free(ringbuf);
	ring->buffer = NULL;
	return ret;
}

void intel_cleanup_ring_buffer(struct intel_engine_cs *ring)
{
	struct drm_i915_private *dev_priv;

	if (!intel_ring_initialized(ring))
		return;

	dev_priv = to_i915(ring->dev);

	intel_stop_ring_buffer(ring);
	WARN_ON(!IS_GEN2(ring->dev) && (I915_READ_MODE(ring) & MODE_IDLE) == 0);

	intel_unpin_ringbuffer_obj(ring->buffer);
	intel_ringbuffer_free(ring->buffer);
	ring->buffer = NULL;

	if (ring->cleanup)
		ring->cleanup(ring);

	if (I915_NEED_GFX_HWS(ring->dev)) {
		cleanup_status_page(ring);
	} else {
		WARN_ON(ring->id != RCS);
		cleanup_phys_status_page(ring);
	}

	i915_cmd_parser_fini_ring(ring);
	i915_gem_batch_pool_fini(&ring->batch_pool);
}

static int ring_wait_for_space(struct intel_engine_cs *ring, int n)
{
	struct intel_ringbuffer *ringbuf = ring->buffer;
	struct drm_i915_gem_request *request;
	unsigned space;
	int ret;

	if (intel_ring_space(ringbuf) >= n)
		return 0;

	/* The whole point of reserving space is to not wait! */
	WARN_ON(ringbuf->reserved_in_use);

	list_for_each_entry(request, &ring->request_list, list) {
		space = __intel_ring_space(request->postfix, ringbuf->tail,
					   ringbuf->size);
		if (space >= n)
			break;
	}

	if (WARN_ON(&request->list == &ring->request_list))
		return -ENOSPC;

	ret = i915_wait_request(request);
	if (ret)
		return ret;

	ringbuf->space = space;
	return 0;
}

static void __wrap_ring_buffer(struct intel_ringbuffer *ringbuf)
{
	uint32_t __iomem *virt;
	int rem = ringbuf->size - ringbuf->tail;

	virt = ringbuf->virtual_start + ringbuf->tail;
	rem /= 4;
	while (rem--)
		iowrite32(MI_NOOP, virt++);

	ringbuf->tail = 0;
	intel_ring_update_space(ringbuf);
}

int intel_ring_idle(struct intel_engine_cs *ring)
{
	struct drm_i915_gem_request *req;

	/* Wait upon the last request to be completed */
	if (list_empty(&ring->request_list))
		return 0;

	req = list_entry(ring->request_list.prev,
			struct drm_i915_gem_request,
			list);

	/* Make sure we do not trigger any retires */
	return __i915_wait_request(req,
				   atomic_read(&to_i915(ring->dev)->gpu_error.reset_counter),
				   to_i915(ring->dev)->mm.interruptible,
				   NULL, NULL);
=======
	intel_engine_cleanup(engine);
	return ret;
}

void intel_engine_cleanup(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv;

	if (!intel_engine_initialized(engine))
		return;

	dev_priv = engine->i915;

	if (engine->buffer) {
		WARN_ON(INTEL_GEN(dev_priv) > 2 &&
			(I915_READ_MODE(engine) & MODE_IDLE) == 0);

		intel_ring_unpin(engine->buffer);
		intel_ring_free(engine->buffer);
		engine->buffer = NULL;
	}

	if (engine->cleanup)
		engine->cleanup(engine);

	if (HWS_NEEDS_PHYSICAL(dev_priv)) {
		WARN_ON(engine->id != RCS);
		cleanup_phys_status_page(engine);
	} else {
		cleanup_status_page(engine);
	}

	intel_engine_cleanup_common(engine);

	intel_ring_context_unpin(dev_priv->kernel_context, engine);

	engine->i915 = NULL;
}

void intel_legacy_submission_resume(struct drm_i915_private *dev_priv)
{
	struct intel_engine_cs *engine;

	for_each_engine(engine, dev_priv) {
		engine->buffer->head = engine->buffer->tail;
		engine->buffer->last_retired_head = -1;
	}
>>>>>>> v4.9.227
}

int intel_ring_alloc_request_extras(struct drm_i915_gem_request *request)
{
<<<<<<< HEAD
	request->ringbuf = request->ring->buffer;
	return 0;
}

int intel_ring_reserve_space(struct drm_i915_gem_request *request)
{
	/*
	 * The first call merely notes the reserve request and is common for
	 * all back ends. The subsequent localised _begin() call actually
	 * ensures that the reservation is available. Without the begin, if
	 * the request creator immediately submitted the request without
	 * adding any commands to it then there might not actually be
	 * sufficient room for the submission commands.
	 */
	intel_ring_reserved_space_reserve(request->ringbuf, MIN_SPACE_FOR_ADD_REQUEST);

	return intel_ring_begin(request, 0);
}

void intel_ring_reserved_space_reserve(struct intel_ringbuffer *ringbuf, int size)
{
	WARN_ON(ringbuf->reserved_size);
	WARN_ON(ringbuf->reserved_in_use);

	ringbuf->reserved_size = size;
}

void intel_ring_reserved_space_cancel(struct intel_ringbuffer *ringbuf)
{
	WARN_ON(ringbuf->reserved_in_use);

	ringbuf->reserved_size   = 0;
	ringbuf->reserved_in_use = false;
}

void intel_ring_reserved_space_use(struct intel_ringbuffer *ringbuf)
{
	WARN_ON(ringbuf->reserved_in_use);

	ringbuf->reserved_in_use = true;
	ringbuf->reserved_tail   = ringbuf->tail;
}

void intel_ring_reserved_space_end(struct intel_ringbuffer *ringbuf)
{
	WARN_ON(!ringbuf->reserved_in_use);
	if (ringbuf->tail > ringbuf->reserved_tail) {
		WARN(ringbuf->tail > ringbuf->reserved_tail + ringbuf->reserved_size,
		     "request reserved size too small: %d vs %d!\n",
		     ringbuf->tail - ringbuf->reserved_tail, ringbuf->reserved_size);
	} else {
		/*
		 * The ring was wrapped while the reserved space was in use.
		 * That means that some unknown amount of the ring tail was
		 * no-op filled and skipped. Thus simply adding the ring size
		 * to the tail and doing the above space check will not work.
		 * Rather than attempt to track how much tail was skipped,
		 * it is much simpler to say that also skipping the sanity
		 * check every once in a while is not a big issue.
		 */
	}

	ringbuf->reserved_size   = 0;
	ringbuf->reserved_in_use = false;
}

static int __intel_ring_prepare(struct intel_engine_cs *ring, int bytes)
{
	struct intel_ringbuffer *ringbuf = ring->buffer;
	int remain_usable = ringbuf->effective_size - ringbuf->tail;
	int remain_actual = ringbuf->size - ringbuf->tail;
	int ret, total_bytes, wait_bytes = 0;
	bool need_wrap = false;

	if (ringbuf->reserved_in_use)
		total_bytes = bytes;
	else
		total_bytes = bytes + ringbuf->reserved_size;
=======
	int ret;

	/* Flush enough space to reduce the likelihood of waiting after
	 * we start building the request - in which case we will just
	 * have to repeat work.
	 */
	request->reserved_space += LEGACY_REQUEST_SIZE;

	request->ring = request->engine->buffer;

	ret = intel_ring_begin(request, 0);
	if (ret)
		return ret;

	request->reserved_space -= LEGACY_REQUEST_SIZE;
	return 0;
}

static int wait_for_space(struct drm_i915_gem_request *req, int bytes)
{
	struct intel_ring *ring = req->ring;
	struct drm_i915_gem_request *target;
	int ret;

	intel_ring_update_space(ring);
	if (ring->space >= bytes)
		return 0;

	/*
	 * Space is reserved in the ringbuffer for finalising the request,
	 * as that cannot be allowed to fail. During request finalisation,
	 * reserved_space is set to 0 to stop the overallocation and the
	 * assumption is that then we never need to wait (which has the
	 * risk of failing with EINTR).
	 *
	 * See also i915_gem_request_alloc() and i915_add_request().
	 */
	GEM_BUG_ON(!req->reserved_space);

	list_for_each_entry(target, &ring->request_list, ring_link) {
		unsigned space;

		/* Would completion of this request free enough space? */
		space = __intel_ring_space(target->postfix, ring->tail,
					   ring->size);
		if (space >= bytes)
			break;
	}

	if (WARN_ON(&target->ring_link == &ring->request_list))
		return -ENOSPC;

	ret = i915_wait_request(target,
				I915_WAIT_INTERRUPTIBLE | I915_WAIT_LOCKED,
				NULL, NO_WAITBOOST);
	if (ret)
		return ret;

	i915_gem_request_retire_upto(target);

	intel_ring_update_space(ring);
	GEM_BUG_ON(ring->space < bytes);
	return 0;
}

int intel_ring_begin(struct drm_i915_gem_request *req, int num_dwords)
{
	struct intel_ring *ring = req->ring;
	int remain_actual = ring->size - ring->tail;
	int remain_usable = ring->effective_size - ring->tail;
	int bytes = num_dwords * sizeof(u32);
	int total_bytes, wait_bytes;
	bool need_wrap = false;

	total_bytes = bytes + req->reserved_space;
>>>>>>> v4.9.227

	if (unlikely(bytes > remain_usable)) {
		/*
		 * Not enough space for the basic request. So need to flush
		 * out the remainder and then wait for base + reserved.
		 */
		wait_bytes = remain_actual + total_bytes;
		need_wrap = true;
<<<<<<< HEAD
	} else {
		if (unlikely(total_bytes > remain_usable)) {
			/*
			 * The base request will fit but the reserved space
			 * falls off the end. So don't need an immediate wrap
			 * and only need to effectively wait for the reserved
			 * size space from the start of ringbuffer.
			 */
			wait_bytes = remain_actual + ringbuf->reserved_size;
		} else if (total_bytes > ringbuf->space) {
			/* No wrapping required, just waiting. */
			wait_bytes = total_bytes;
		}
	}

	if (wait_bytes) {
		ret = ring_wait_for_space(ring, wait_bytes);
		if (unlikely(ret))
			return ret;

		if (need_wrap)
			__wrap_ring_buffer(ringbuf);
	}

	return 0;
}

int intel_ring_begin(struct drm_i915_gem_request *req,
		     int num_dwords)
{
	struct intel_engine_cs *ring;
	struct drm_i915_private *dev_priv;
	int ret;

	WARN_ON(req == NULL);
	ring = req->ring;
	dev_priv = ring->dev->dev_private;

	ret = i915_gem_check_wedge(&dev_priv->gpu_error,
				   dev_priv->mm.interruptible);
	if (ret)
		return ret;

	ret = __intel_ring_prepare(ring, num_dwords * sizeof(uint32_t));
	if (ret)
		return ret;

	ring->buffer->space -= num_dwords * sizeof(uint32_t);
=======
	} else if (unlikely(total_bytes > remain_usable)) {
		/*
		 * The base request will fit but the reserved space
		 * falls off the end. So we don't need an immediate wrap
		 * and only need to effectively wait for the reserved
		 * size space from the start of ringbuffer.
		 */
		wait_bytes = remain_actual + req->reserved_space;
	} else {
		/* No wrapping required, just waiting. */
		wait_bytes = total_bytes;
	}

	if (wait_bytes > ring->space) {
		int ret = wait_for_space(req, wait_bytes);
		if (unlikely(ret))
			return ret;
	}

	if (unlikely(need_wrap)) {
		GEM_BUG_ON(remain_actual > ring->space);
		GEM_BUG_ON(ring->tail + remain_actual > ring->size);

		/* Fill the tail with MI_NOOP */
		memset(ring->vaddr + ring->tail, 0, remain_actual);
		ring->tail = 0;
		ring->space -= remain_actual;
	}

	ring->space -= bytes;
	GEM_BUG_ON(ring->space < 0);
>>>>>>> v4.9.227
	return 0;
}

/* Align the ring tail to a cacheline boundary */
int intel_ring_cacheline_align(struct drm_i915_gem_request *req)
{
<<<<<<< HEAD
	struct intel_engine_cs *ring = req->ring;
	int num_dwords = (ring->buffer->tail & (CACHELINE_BYTES - 1)) / sizeof(uint32_t);
=======
	struct intel_ring *ring = req->ring;
	int num_dwords =
		(ring->tail & (CACHELINE_BYTES - 1)) / sizeof(uint32_t);
>>>>>>> v4.9.227
	int ret;

	if (num_dwords == 0)
		return 0;

	num_dwords = CACHELINE_BYTES / sizeof(uint32_t) - num_dwords;
	ret = intel_ring_begin(req, num_dwords);
	if (ret)
		return ret;

	while (num_dwords--)
		intel_ring_emit(ring, MI_NOOP);

	intel_ring_advance(ring);

	return 0;
}

<<<<<<< HEAD
void intel_ring_init_seqno(struct intel_engine_cs *ring, u32 seqno)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;

	if (INTEL_INFO(dev)->gen == 6 || INTEL_INFO(dev)->gen == 7) {
		I915_WRITE(RING_SYNC_0(ring->mmio_base), 0);
		I915_WRITE(RING_SYNC_1(ring->mmio_base), 0);
		if (HAS_VEBOX(dev))
			I915_WRITE(RING_SYNC_2(ring->mmio_base), 0);
	}

	ring->set_seqno(ring, seqno);
	ring->hangcheck.seqno = seqno;
}

static void gen6_bsd_ring_write_tail(struct intel_engine_cs *ring,
				     u32 value)
{
	struct drm_i915_private *dev_priv = ring->dev->dev_private;
=======
static void gen6_bsd_submit_request(struct drm_i915_gem_request *request)
{
	struct drm_i915_private *dev_priv = request->i915;

	intel_uncore_forcewake_get(dev_priv, FORCEWAKE_ALL);
>>>>>>> v4.9.227

       /* Every tail move must follow the sequence below */

	/* Disable notification that the ring is IDLE. The GT
	 * will then assume that it is busy and bring it out of rc6.
	 */
<<<<<<< HEAD
	I915_WRITE(GEN6_BSD_SLEEP_PSMI_CONTROL,
		   _MASKED_BIT_ENABLE(GEN6_BSD_SLEEP_MSG_DISABLE));

	/* Clear the context id. Here be magic! */
	I915_WRITE64(GEN6_BSD_RNCID, 0x0);

	/* Wait for the ring not to be idle, i.e. for it to wake up. */
	if (wait_for((I915_READ(GEN6_BSD_SLEEP_PSMI_CONTROL) &
		      GEN6_BSD_SLEEP_INDICATOR) == 0,
		     50))
		DRM_ERROR("timed out waiting for the BSD ring to wake up\n");

	/* Now that the ring is fully powered up, update the tail */
	I915_WRITE_TAIL(ring, value);
	POSTING_READ(RING_TAIL(ring->mmio_base));
=======
	I915_WRITE_FW(GEN6_BSD_SLEEP_PSMI_CONTROL,
		      _MASKED_BIT_ENABLE(GEN6_BSD_SLEEP_MSG_DISABLE));

	/* Clear the context id. Here be magic! */
	I915_WRITE64_FW(GEN6_BSD_RNCID, 0x0);

	/* Wait for the ring not to be idle, i.e. for it to wake up. */
	if (intel_wait_for_register_fw(dev_priv,
				       GEN6_BSD_SLEEP_PSMI_CONTROL,
				       GEN6_BSD_SLEEP_INDICATOR,
				       0,
				       50))
		DRM_ERROR("timed out waiting for the BSD ring to wake up\n");

	/* Now that the ring is fully powered up, update the tail */
	i9xx_submit_request(request);
>>>>>>> v4.9.227

	/* Let the ring send IDLE messages to the GT again,
	 * and so let it sleep to conserve power when idle.
	 */
<<<<<<< HEAD
	I915_WRITE(GEN6_BSD_SLEEP_PSMI_CONTROL,
		   _MASKED_BIT_DISABLE(GEN6_BSD_SLEEP_MSG_DISABLE));
}

static int gen6_bsd_ring_flush(struct drm_i915_gem_request *req,
			       u32 invalidate, u32 flush)
{
	struct intel_engine_cs *ring = req->ring;
=======
	I915_WRITE_FW(GEN6_BSD_SLEEP_PSMI_CONTROL,
		      _MASKED_BIT_DISABLE(GEN6_BSD_SLEEP_MSG_DISABLE));

	intel_uncore_forcewake_put(dev_priv, FORCEWAKE_ALL);
}

static int gen6_bsd_ring_flush(struct drm_i915_gem_request *req, u32 mode)
{
	struct intel_ring *ring = req->ring;
>>>>>>> v4.9.227
	uint32_t cmd;
	int ret;

	ret = intel_ring_begin(req, 4);
	if (ret)
		return ret;

	cmd = MI_FLUSH_DW;
<<<<<<< HEAD
	if (INTEL_INFO(ring->dev)->gen >= 8)
=======
	if (INTEL_GEN(req->i915) >= 8)
>>>>>>> v4.9.227
		cmd += 1;

	/* We always require a command barrier so that subsequent
	 * commands, such as breadcrumb interrupts, are strictly ordered
	 * wrt the contents of the write cache being flushed to memory
	 * (and thus being coherent from the CPU).
	 */
	cmd |= MI_FLUSH_DW_STORE_INDEX | MI_FLUSH_DW_OP_STOREDW;

	/*
	 * Bspec vol 1c.5 - video engine command streamer:
	 * "If ENABLED, all TLBs will be invalidated once the flush
	 * operation is complete. This bit is only valid when the
	 * Post-Sync Operation field is a value of 1h or 3h."
	 */
<<<<<<< HEAD
	if (invalidate & I915_GEM_GPU_DOMAINS)
=======
	if (mode & EMIT_INVALIDATE)
>>>>>>> v4.9.227
		cmd |= MI_INVALIDATE_TLB | MI_INVALIDATE_BSD;

	intel_ring_emit(ring, cmd);
	intel_ring_emit(ring, I915_GEM_HWS_SCRATCH_ADDR | MI_FLUSH_DW_USE_GTT);
<<<<<<< HEAD
	if (INTEL_INFO(ring->dev)->gen >= 8) {
=======
	if (INTEL_GEN(req->i915) >= 8) {
>>>>>>> v4.9.227
		intel_ring_emit(ring, 0); /* upper addr */
		intel_ring_emit(ring, 0); /* value */
	} else  {
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring, MI_NOOP);
	}
	intel_ring_advance(ring);
	return 0;
}

static int
<<<<<<< HEAD
gen8_ring_dispatch_execbuffer(struct drm_i915_gem_request *req,
			      u64 offset, u32 len,
			      unsigned dispatch_flags)
{
	struct intel_engine_cs *ring = req->ring;
	bool ppgtt = USES_PPGTT(ring->dev) &&
=======
gen8_emit_bb_start(struct drm_i915_gem_request *req,
		   u64 offset, u32 len,
		   unsigned int dispatch_flags)
{
	struct intel_ring *ring = req->ring;
	bool ppgtt = USES_PPGTT(req->i915) &&
>>>>>>> v4.9.227
			!(dispatch_flags & I915_DISPATCH_SECURE);
	int ret;

	ret = intel_ring_begin(req, 4);
	if (ret)
		return ret;

	/* FIXME(BDW): Address space and security selectors. */
	intel_ring_emit(ring, MI_BATCH_BUFFER_START_GEN8 | (ppgtt<<8) |
			(dispatch_flags & I915_DISPATCH_RS ?
			 MI_BATCH_RESOURCE_STREAMER : 0));
	intel_ring_emit(ring, lower_32_bits(offset));
	intel_ring_emit(ring, upper_32_bits(offset));
	intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);

	return 0;
}

static int
<<<<<<< HEAD
hsw_ring_dispatch_execbuffer(struct drm_i915_gem_request *req,
			     u64 offset, u32 len,
			     unsigned dispatch_flags)
{
	struct intel_engine_cs *ring = req->ring;
=======
hsw_emit_bb_start(struct drm_i915_gem_request *req,
		  u64 offset, u32 len,
		  unsigned int dispatch_flags)
{
	struct intel_ring *ring = req->ring;
>>>>>>> v4.9.227
	int ret;

	ret = intel_ring_begin(req, 2);
	if (ret)
		return ret;

	intel_ring_emit(ring,
			MI_BATCH_BUFFER_START |
			(dispatch_flags & I915_DISPATCH_SECURE ?
			 0 : MI_BATCH_PPGTT_HSW | MI_BATCH_NON_SECURE_HSW) |
			(dispatch_flags & I915_DISPATCH_RS ?
			 MI_BATCH_RESOURCE_STREAMER : 0));
	/* bit0-7 is the length on GEN6+ */
	intel_ring_emit(ring, offset);
	intel_ring_advance(ring);

	return 0;
}

static int
<<<<<<< HEAD
gen6_ring_dispatch_execbuffer(struct drm_i915_gem_request *req,
			      u64 offset, u32 len,
			      unsigned dispatch_flags)
{
	struct intel_engine_cs *ring = req->ring;
=======
gen6_emit_bb_start(struct drm_i915_gem_request *req,
		   u64 offset, u32 len,
		   unsigned int dispatch_flags)
{
	struct intel_ring *ring = req->ring;
>>>>>>> v4.9.227
	int ret;

	ret = intel_ring_begin(req, 2);
	if (ret)
		return ret;

	intel_ring_emit(ring,
			MI_BATCH_BUFFER_START |
			(dispatch_flags & I915_DISPATCH_SECURE ?
			 0 : MI_BATCH_NON_SECURE_I965));
	/* bit0-7 is the length on GEN6+ */
	intel_ring_emit(ring, offset);
	intel_ring_advance(ring);

	return 0;
}

/* Blitter support (SandyBridge+) */

<<<<<<< HEAD
static int gen6_ring_flush(struct drm_i915_gem_request *req,
			   u32 invalidate, u32 flush)
{
	struct intel_engine_cs *ring = req->ring;
	struct drm_device *dev = ring->dev;
=======
static int gen6_ring_flush(struct drm_i915_gem_request *req, u32 mode)
{
	struct intel_ring *ring = req->ring;
>>>>>>> v4.9.227
	uint32_t cmd;
	int ret;

	ret = intel_ring_begin(req, 4);
	if (ret)
		return ret;

	cmd = MI_FLUSH_DW;
<<<<<<< HEAD
	if (INTEL_INFO(dev)->gen >= 8)
=======
	if (INTEL_GEN(req->i915) >= 8)
>>>>>>> v4.9.227
		cmd += 1;

	/* We always require a command barrier so that subsequent
	 * commands, such as breadcrumb interrupts, are strictly ordered
	 * wrt the contents of the write cache being flushed to memory
	 * (and thus being coherent from the CPU).
	 */
	cmd |= MI_FLUSH_DW_STORE_INDEX | MI_FLUSH_DW_OP_STOREDW;

	/*
	 * Bspec vol 1c.3 - blitter engine command streamer:
	 * "If ENABLED, all TLBs will be invalidated once the flush
	 * operation is complete. This bit is only valid when the
	 * Post-Sync Operation field is a value of 1h or 3h."
	 */
<<<<<<< HEAD
	if (invalidate & I915_GEM_DOMAIN_RENDER)
		cmd |= MI_INVALIDATE_TLB;
	intel_ring_emit(ring, cmd);
	intel_ring_emit(ring, I915_GEM_HWS_SCRATCH_ADDR | MI_FLUSH_DW_USE_GTT);
	if (INTEL_INFO(dev)->gen >= 8) {
=======
	if (mode & EMIT_INVALIDATE)
		cmd |= MI_INVALIDATE_TLB;
	intel_ring_emit(ring, cmd);
	intel_ring_emit(ring,
			I915_GEM_HWS_SCRATCH_ADDR | MI_FLUSH_DW_USE_GTT);
	if (INTEL_GEN(req->i915) >= 8) {
>>>>>>> v4.9.227
		intel_ring_emit(ring, 0); /* upper addr */
		intel_ring_emit(ring, 0); /* value */
	} else  {
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring, MI_NOOP);
	}
	intel_ring_advance(ring);

	return 0;
}

<<<<<<< HEAD
int intel_init_render_ring_buffer(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring = &dev_priv->ring[RCS];
	struct drm_i915_gem_object *obj;
	int ret;

	ring->name = "render ring";
	ring->id = RCS;
	ring->mmio_base = RENDER_RING_BASE;

	if (INTEL_INFO(dev)->gen >= 8) {
		if (i915_semaphore_is_enabled(dev)) {
			obj = i915_gem_alloc_object(dev, 4096);
			if (obj == NULL) {
				DRM_ERROR("Failed to allocate semaphore bo. Disabling semaphores\n");
				i915.semaphores = 0;
			} else {
				i915_gem_object_set_cache_level(obj, I915_CACHE_LLC);
				ret = i915_gem_obj_ggtt_pin(obj, 0, PIN_NONBLOCK);
				if (ret != 0) {
					drm_gem_object_unreference(&obj->base);
					DRM_ERROR("Failed to pin semaphore bo. Disabling semaphores\n");
					i915.semaphores = 0;
				} else
					dev_priv->semaphore_obj = obj;
			}
		}

		ring->init_context = intel_rcs_ctx_init;
		ring->add_request = gen6_add_request;
		ring->flush = gen8_render_ring_flush;
		ring->irq_get = gen8_ring_get_irq;
		ring->irq_put = gen8_ring_put_irq;
		ring->irq_enable_mask = GT_RENDER_USER_INTERRUPT;
		ring->get_seqno = gen6_ring_get_seqno;
		ring->set_seqno = ring_set_seqno;
		if (i915_semaphore_is_enabled(dev)) {
			WARN_ON(!dev_priv->semaphore_obj);
			ring->semaphore.sync_to = gen8_ring_sync;
			ring->semaphore.signal = gen8_rcs_signal;
			GEN8_RING_SEMAPHORE_INIT;
		}
	} else if (INTEL_INFO(dev)->gen >= 6) {
		ring->init_context = intel_rcs_ctx_init;
		ring->add_request = gen6_add_request;
		ring->flush = gen7_render_ring_flush;
		if (INTEL_INFO(dev)->gen == 6)
			ring->flush = gen6_render_ring_flush;
		ring->irq_get = gen6_ring_get_irq;
		ring->irq_put = gen6_ring_put_irq;
		ring->irq_enable_mask = GT_RENDER_USER_INTERRUPT;
		ring->get_seqno = gen6_ring_get_seqno;
		ring->set_seqno = ring_set_seqno;
		if (i915_semaphore_is_enabled(dev)) {
			ring->semaphore.sync_to = gen6_ring_sync;
			ring->semaphore.signal = gen6_signal;
			/*
			 * The current semaphore is only applied on pre-gen8
			 * platform.  And there is no VCS2 ring on the pre-gen8
			 * platform. So the semaphore between RCS and VCS2 is
			 * initialized as INVALID.  Gen8 will initialize the
			 * sema between VCS2 and RCS later.
			 */
			ring->semaphore.mbox.wait[RCS] = MI_SEMAPHORE_SYNC_INVALID;
			ring->semaphore.mbox.wait[VCS] = MI_SEMAPHORE_SYNC_RV;
			ring->semaphore.mbox.wait[BCS] = MI_SEMAPHORE_SYNC_RB;
			ring->semaphore.mbox.wait[VECS] = MI_SEMAPHORE_SYNC_RVE;
			ring->semaphore.mbox.wait[VCS2] = MI_SEMAPHORE_SYNC_INVALID;
			ring->semaphore.mbox.signal[RCS] = GEN6_NOSYNC;
			ring->semaphore.mbox.signal[VCS] = GEN6_VRSYNC;
			ring->semaphore.mbox.signal[BCS] = GEN6_BRSYNC;
			ring->semaphore.mbox.signal[VECS] = GEN6_VERSYNC;
			ring->semaphore.mbox.signal[VCS2] = GEN6_NOSYNC;
		}
	} else if (IS_GEN5(dev)) {
		ring->add_request = pc_render_add_request;
		ring->flush = gen4_render_ring_flush;
		ring->get_seqno = pc_render_get_seqno;
		ring->set_seqno = pc_render_set_seqno;
		ring->irq_get = gen5_ring_get_irq;
		ring->irq_put = gen5_ring_put_irq;
		ring->irq_enable_mask = GT_RENDER_USER_INTERRUPT |
					GT_RENDER_PIPECTL_NOTIFY_INTERRUPT;
	} else {
		ring->add_request = i9xx_add_request;
		if (INTEL_INFO(dev)->gen < 4)
			ring->flush = gen2_render_ring_flush;
		else
			ring->flush = gen4_render_ring_flush;
		ring->get_seqno = ring_get_seqno;
		ring->set_seqno = ring_set_seqno;
		if (IS_GEN2(dev)) {
			ring->irq_get = i8xx_ring_get_irq;
			ring->irq_put = i8xx_ring_put_irq;
		} else {
			ring->irq_get = i9xx_ring_get_irq;
			ring->irq_put = i9xx_ring_put_irq;
		}
		ring->irq_enable_mask = I915_USER_INTERRUPT;
	}
	ring->write_tail = ring_write_tail;

	if (IS_HASWELL(dev))
		ring->dispatch_execbuffer = hsw_ring_dispatch_execbuffer;
	else if (IS_GEN8(dev))
		ring->dispatch_execbuffer = gen8_ring_dispatch_execbuffer;
	else if (INTEL_INFO(dev)->gen >= 6)
		ring->dispatch_execbuffer = gen6_ring_dispatch_execbuffer;
	else if (INTEL_INFO(dev)->gen >= 4)
		ring->dispatch_execbuffer = i965_dispatch_execbuffer;
	else if (IS_I830(dev) || IS_845G(dev))
		ring->dispatch_execbuffer = i830_dispatch_execbuffer;
	else
		ring->dispatch_execbuffer = i915_dispatch_execbuffer;
	ring->init_hw = init_render_ring;
	ring->cleanup = render_ring_cleanup;

	/* Workaround batchbuffer to combat CS tlb bug. */
	if (HAS_BROKEN_CS_TLB(dev)) {
		obj = i915_gem_alloc_object(dev, I830_WA_SIZE);
		if (obj == NULL) {
			DRM_ERROR("Failed to allocate batch bo\n");
			return -ENOMEM;
		}

		ret = i915_gem_obj_ggtt_pin(obj, 0, 0);
		if (ret != 0) {
			drm_gem_object_unreference(&obj->base);
			DRM_ERROR("Failed to ping batch bo\n");
			return ret;
		}

		ring->scratch.obj = obj;
		ring->scratch.gtt_offset = i915_gem_obj_ggtt_offset(obj);
	}

	ret = intel_init_ring_buffer(dev, ring);
	if (ret)
		return ret;

	if (INTEL_INFO(dev)->gen >= 5) {
		ret = intel_init_pipe_control(ring);
=======
static void intel_ring_init_semaphores(struct drm_i915_private *dev_priv,
				       struct intel_engine_cs *engine)
{
	struct drm_i915_gem_object *obj;
	int ret, i;

	if (!i915.semaphores)
		return;

	if (INTEL_GEN(dev_priv) >= 8 && !dev_priv->semaphore) {
		struct i915_vma *vma;

		obj = i915_gem_object_create(&dev_priv->drm, 4096);
		if (IS_ERR(obj))
			goto err;

		vma = i915_vma_create(obj, &dev_priv->ggtt.base, NULL);
		if (IS_ERR(vma))
			goto err_obj;

		ret = i915_gem_object_set_to_gtt_domain(obj, false);
		if (ret)
			goto err_obj;

		ret = i915_vma_pin(vma, 0, 0, PIN_GLOBAL | PIN_HIGH);
		if (ret)
			goto err_obj;

		dev_priv->semaphore = vma;
	}

	if (INTEL_GEN(dev_priv) >= 8) {
		u32 offset = i915_ggtt_offset(dev_priv->semaphore);

		engine->semaphore.sync_to = gen8_ring_sync_to;
		engine->semaphore.signal = gen8_xcs_signal;

		for (i = 0; i < I915_NUM_ENGINES; i++) {
			u32 ring_offset;

			if (i != engine->id)
				ring_offset = offset + GEN8_SEMAPHORE_OFFSET(engine->id, i);
			else
				ring_offset = MI_SEMAPHORE_SYNC_INVALID;

			engine->semaphore.signal_ggtt[i] = ring_offset;
		}
	} else if (INTEL_GEN(dev_priv) >= 6) {
		engine->semaphore.sync_to = gen6_ring_sync_to;
		engine->semaphore.signal = gen6_signal;

		/*
		 * The current semaphore is only applied on pre-gen8
		 * platform.  And there is no VCS2 ring on the pre-gen8
		 * platform. So the semaphore between RCS and VCS2 is
		 * initialized as INVALID.  Gen8 will initialize the
		 * sema between VCS2 and RCS later.
		 */
		for (i = 0; i < GEN6_NUM_SEMAPHORES; i++) {
			static const struct {
				u32 wait_mbox;
				i915_reg_t mbox_reg;
			} sem_data[GEN6_NUM_SEMAPHORES][GEN6_NUM_SEMAPHORES] = {
				[RCS_HW] = {
					[VCS_HW] =  { .wait_mbox = MI_SEMAPHORE_SYNC_RV,  .mbox_reg = GEN6_VRSYNC },
					[BCS_HW] =  { .wait_mbox = MI_SEMAPHORE_SYNC_RB,  .mbox_reg = GEN6_BRSYNC },
					[VECS_HW] = { .wait_mbox = MI_SEMAPHORE_SYNC_RVE, .mbox_reg = GEN6_VERSYNC },
				},
				[VCS_HW] = {
					[RCS_HW] =  { .wait_mbox = MI_SEMAPHORE_SYNC_VR,  .mbox_reg = GEN6_RVSYNC },
					[BCS_HW] =  { .wait_mbox = MI_SEMAPHORE_SYNC_VB,  .mbox_reg = GEN6_BVSYNC },
					[VECS_HW] = { .wait_mbox = MI_SEMAPHORE_SYNC_VVE, .mbox_reg = GEN6_VEVSYNC },
				},
				[BCS_HW] = {
					[RCS_HW] =  { .wait_mbox = MI_SEMAPHORE_SYNC_BR,  .mbox_reg = GEN6_RBSYNC },
					[VCS_HW] =  { .wait_mbox = MI_SEMAPHORE_SYNC_BV,  .mbox_reg = GEN6_VBSYNC },
					[VECS_HW] = { .wait_mbox = MI_SEMAPHORE_SYNC_BVE, .mbox_reg = GEN6_VEBSYNC },
				},
				[VECS_HW] = {
					[RCS_HW] =  { .wait_mbox = MI_SEMAPHORE_SYNC_VER, .mbox_reg = GEN6_RVESYNC },
					[VCS_HW] =  { .wait_mbox = MI_SEMAPHORE_SYNC_VEV, .mbox_reg = GEN6_VVESYNC },
					[BCS_HW] =  { .wait_mbox = MI_SEMAPHORE_SYNC_VEB, .mbox_reg = GEN6_BVESYNC },
				},
			};
			u32 wait_mbox;
			i915_reg_t mbox_reg;

			if (i == engine->hw_id) {
				wait_mbox = MI_SEMAPHORE_SYNC_INVALID;
				mbox_reg = GEN6_NOSYNC;
			} else {
				wait_mbox = sem_data[engine->hw_id][i].wait_mbox;
				mbox_reg = sem_data[engine->hw_id][i].mbox_reg;
			}

			engine->semaphore.mbox.wait[i] = wait_mbox;
			engine->semaphore.mbox.signal[i] = mbox_reg;
		}
	}

	return;

err_obj:
	i915_gem_object_put(obj);
err:
	DRM_DEBUG_DRIVER("Failed to allocate space for semaphores, disabling\n");
	i915.semaphores = 0;
}

static void intel_ring_init_irq(struct drm_i915_private *dev_priv,
				struct intel_engine_cs *engine)
{
	engine->irq_enable_mask = GT_RENDER_USER_INTERRUPT << engine->irq_shift;

	if (INTEL_GEN(dev_priv) >= 8) {
		engine->irq_enable = gen8_irq_enable;
		engine->irq_disable = gen8_irq_disable;
		engine->irq_seqno_barrier = gen6_seqno_barrier;
	} else if (INTEL_GEN(dev_priv) >= 6) {
		engine->irq_enable = gen6_irq_enable;
		engine->irq_disable = gen6_irq_disable;
		engine->irq_seqno_barrier = gen6_seqno_barrier;
	} else if (INTEL_GEN(dev_priv) >= 5) {
		engine->irq_enable = gen5_irq_enable;
		engine->irq_disable = gen5_irq_disable;
		engine->irq_seqno_barrier = gen5_seqno_barrier;
	} else if (INTEL_GEN(dev_priv) >= 3) {
		engine->irq_enable = i9xx_irq_enable;
		engine->irq_disable = i9xx_irq_disable;
	} else {
		engine->irq_enable = i8xx_irq_enable;
		engine->irq_disable = i8xx_irq_disable;
	}
}

static void intel_ring_default_vfuncs(struct drm_i915_private *dev_priv,
				      struct intel_engine_cs *engine)
{
	intel_ring_init_irq(dev_priv, engine);
	intel_ring_init_semaphores(dev_priv, engine);

	engine->init_hw = init_ring_common;
	engine->reset_hw = reset_ring_common;

	engine->emit_request = i9xx_emit_request;
	if (i915.semaphores)
		engine->emit_request = gen6_sema_emit_request;
	engine->submit_request = i9xx_submit_request;

	if (INTEL_GEN(dev_priv) >= 8)
		engine->emit_bb_start = gen8_emit_bb_start;
	else if (INTEL_GEN(dev_priv) >= 6)
		engine->emit_bb_start = gen6_emit_bb_start;
	else if (INTEL_GEN(dev_priv) >= 4)
		engine->emit_bb_start = i965_emit_bb_start;
	else if (IS_I830(dev_priv) || IS_845G(dev_priv))
		engine->emit_bb_start = i830_emit_bb_start;
	else
		engine->emit_bb_start = i915_emit_bb_start;
}

int intel_init_render_ring_buffer(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	int ret;

	intel_ring_default_vfuncs(dev_priv, engine);

	if (HAS_L3_DPF(dev_priv))
		engine->irq_keep_mask = GT_RENDER_L3_PARITY_ERROR_INTERRUPT;

	if (INTEL_GEN(dev_priv) >= 8) {
		engine->init_context = intel_rcs_ctx_init;
		engine->emit_request = gen8_render_emit_request;
		engine->emit_flush = gen8_render_ring_flush;
		if (i915.semaphores)
			engine->semaphore.signal = gen8_rcs_signal;
	} else if (INTEL_GEN(dev_priv) >= 6) {
		engine->init_context = intel_rcs_ctx_init;
		engine->emit_flush = gen7_render_ring_flush;
		if (IS_GEN6(dev_priv))
			engine->emit_flush = gen6_render_ring_flush;
	} else if (IS_GEN5(dev_priv)) {
		engine->emit_flush = gen4_render_ring_flush;
	} else {
		if (INTEL_GEN(dev_priv) < 4)
			engine->emit_flush = gen2_render_ring_flush;
		else
			engine->emit_flush = gen4_render_ring_flush;
		engine->irq_enable_mask = I915_USER_INTERRUPT;
	}

	if (IS_HASWELL(dev_priv))
		engine->emit_bb_start = hsw_emit_bb_start;

	engine->init_hw = init_render_ring;
	engine->cleanup = render_ring_cleanup;

	ret = intel_init_ring_buffer(engine);
	if (ret)
		return ret;

	if (INTEL_GEN(dev_priv) >= 6) {
		ret = intel_engine_create_scratch(engine, 4096);
		if (ret)
			return ret;
	} else if (HAS_BROKEN_CS_TLB(dev_priv)) {
		ret = intel_engine_create_scratch(engine, I830_WA_SIZE);
>>>>>>> v4.9.227
		if (ret)
			return ret;
	}

	return 0;
}

<<<<<<< HEAD
int intel_init_bsd_ring_buffer(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring = &dev_priv->ring[VCS];

	ring->name = "bsd ring";
	ring->id = VCS;

	ring->write_tail = ring_write_tail;
	if (INTEL_INFO(dev)->gen >= 6) {
		ring->mmio_base = GEN6_BSD_RING_BASE;
		/* gen6 bsd needs a special wa for tail updates */
		if (IS_GEN6(dev))
			ring->write_tail = gen6_bsd_ring_write_tail;
		ring->flush = gen6_bsd_ring_flush;
		ring->add_request = gen6_add_request;
		ring->get_seqno = gen6_ring_get_seqno;
		ring->set_seqno = ring_set_seqno;
		if (INTEL_INFO(dev)->gen >= 8) {
			ring->irq_enable_mask =
				GT_RENDER_USER_INTERRUPT << GEN8_VCS1_IRQ_SHIFT;
			ring->irq_get = gen8_ring_get_irq;
			ring->irq_put = gen8_ring_put_irq;
			ring->dispatch_execbuffer =
				gen8_ring_dispatch_execbuffer;
			if (i915_semaphore_is_enabled(dev)) {
				ring->semaphore.sync_to = gen8_ring_sync;
				ring->semaphore.signal = gen8_xcs_signal;
				GEN8_RING_SEMAPHORE_INIT;
			}
		} else {
			ring->irq_enable_mask = GT_BSD_USER_INTERRUPT;
			ring->irq_get = gen6_ring_get_irq;
			ring->irq_put = gen6_ring_put_irq;
			ring->dispatch_execbuffer =
				gen6_ring_dispatch_execbuffer;
			if (i915_semaphore_is_enabled(dev)) {
				ring->semaphore.sync_to = gen6_ring_sync;
				ring->semaphore.signal = gen6_signal;
				ring->semaphore.mbox.wait[RCS] = MI_SEMAPHORE_SYNC_VR;
				ring->semaphore.mbox.wait[VCS] = MI_SEMAPHORE_SYNC_INVALID;
				ring->semaphore.mbox.wait[BCS] = MI_SEMAPHORE_SYNC_VB;
				ring->semaphore.mbox.wait[VECS] = MI_SEMAPHORE_SYNC_VVE;
				ring->semaphore.mbox.wait[VCS2] = MI_SEMAPHORE_SYNC_INVALID;
				ring->semaphore.mbox.signal[RCS] = GEN6_RVSYNC;
				ring->semaphore.mbox.signal[VCS] = GEN6_NOSYNC;
				ring->semaphore.mbox.signal[BCS] = GEN6_BVSYNC;
				ring->semaphore.mbox.signal[VECS] = GEN6_VEVSYNC;
				ring->semaphore.mbox.signal[VCS2] = GEN6_NOSYNC;
			}
		}
	} else {
		ring->mmio_base = BSD_RING_BASE;
		ring->flush = bsd_ring_flush;
		ring->add_request = i9xx_add_request;
		ring->get_seqno = ring_get_seqno;
		ring->set_seqno = ring_set_seqno;
		if (IS_GEN5(dev)) {
			ring->irq_enable_mask = ILK_BSD_USER_INTERRUPT;
			ring->irq_get = gen5_ring_get_irq;
			ring->irq_put = gen5_ring_put_irq;
		} else {
			ring->irq_enable_mask = I915_BSD_USER_INTERRUPT;
			ring->irq_get = i9xx_ring_get_irq;
			ring->irq_put = i9xx_ring_put_irq;
		}
		ring->dispatch_execbuffer = i965_dispatch_execbuffer;
	}
	ring->init_hw = init_ring_common;

	return intel_init_ring_buffer(dev, ring);
=======
int intel_init_bsd_ring_buffer(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	intel_ring_default_vfuncs(dev_priv, engine);

	if (INTEL_GEN(dev_priv) >= 6) {
		/* gen6 bsd needs a special wa for tail updates */
		if (IS_GEN6(dev_priv))
			engine->submit_request = gen6_bsd_submit_request;
		engine->emit_flush = gen6_bsd_ring_flush;
		if (INTEL_GEN(dev_priv) < 8)
			engine->irq_enable_mask = GT_BSD_USER_INTERRUPT;
	} else {
		engine->mmio_base = BSD_RING_BASE;
		engine->emit_flush = bsd_ring_flush;
		if (IS_GEN5(dev_priv))
			engine->irq_enable_mask = ILK_BSD_USER_INTERRUPT;
		else
			engine->irq_enable_mask = I915_BSD_USER_INTERRUPT;
	}

	return intel_init_ring_buffer(engine);
>>>>>>> v4.9.227
}

/**
 * Initialize the second BSD ring (eg. Broadwell GT3, Skylake GT3)
 */
<<<<<<< HEAD
int intel_init_bsd2_ring_buffer(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring = &dev_priv->ring[VCS2];

	ring->name = "bsd2 ring";
	ring->id = VCS2;

	ring->write_tail = ring_write_tail;
	ring->mmio_base = GEN8_BSD2_RING_BASE;
	ring->flush = gen6_bsd_ring_flush;
	ring->add_request = gen6_add_request;
	ring->get_seqno = gen6_ring_get_seqno;
	ring->set_seqno = ring_set_seqno;
	ring->irq_enable_mask =
			GT_RENDER_USER_INTERRUPT << GEN8_VCS2_IRQ_SHIFT;
	ring->irq_get = gen8_ring_get_irq;
	ring->irq_put = gen8_ring_put_irq;
	ring->dispatch_execbuffer =
			gen8_ring_dispatch_execbuffer;
	if (i915_semaphore_is_enabled(dev)) {
		ring->semaphore.sync_to = gen8_ring_sync;
		ring->semaphore.signal = gen8_xcs_signal;
		GEN8_RING_SEMAPHORE_INIT;
	}
	ring->init_hw = init_ring_common;

	return intel_init_ring_buffer(dev, ring);
}

int intel_init_blt_ring_buffer(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring = &dev_priv->ring[BCS];

	ring->name = "blitter ring";
	ring->id = BCS;

	ring->mmio_base = BLT_RING_BASE;
	ring->write_tail = ring_write_tail;
	ring->flush = gen6_ring_flush;
	ring->add_request = gen6_add_request;
	ring->get_seqno = gen6_ring_get_seqno;
	ring->set_seqno = ring_set_seqno;
	if (INTEL_INFO(dev)->gen >= 8) {
		ring->irq_enable_mask =
			GT_RENDER_USER_INTERRUPT << GEN8_BCS_IRQ_SHIFT;
		ring->irq_get = gen8_ring_get_irq;
		ring->irq_put = gen8_ring_put_irq;
		ring->dispatch_execbuffer = gen8_ring_dispatch_execbuffer;
		if (i915_semaphore_is_enabled(dev)) {
			ring->semaphore.sync_to = gen8_ring_sync;
			ring->semaphore.signal = gen8_xcs_signal;
			GEN8_RING_SEMAPHORE_INIT;
		}
	} else {
		ring->irq_enable_mask = GT_BLT_USER_INTERRUPT;
		ring->irq_get = gen6_ring_get_irq;
		ring->irq_put = gen6_ring_put_irq;
		ring->dispatch_execbuffer = gen6_ring_dispatch_execbuffer;
		if (i915_semaphore_is_enabled(dev)) {
			ring->semaphore.signal = gen6_signal;
			ring->semaphore.sync_to = gen6_ring_sync;
			/*
			 * The current semaphore is only applied on pre-gen8
			 * platform.  And there is no VCS2 ring on the pre-gen8
			 * platform. So the semaphore between BCS and VCS2 is
			 * initialized as INVALID.  Gen8 will initialize the
			 * sema between BCS and VCS2 later.
			 */
			ring->semaphore.mbox.wait[RCS] = MI_SEMAPHORE_SYNC_BR;
			ring->semaphore.mbox.wait[VCS] = MI_SEMAPHORE_SYNC_BV;
			ring->semaphore.mbox.wait[BCS] = MI_SEMAPHORE_SYNC_INVALID;
			ring->semaphore.mbox.wait[VECS] = MI_SEMAPHORE_SYNC_BVE;
			ring->semaphore.mbox.wait[VCS2] = MI_SEMAPHORE_SYNC_INVALID;
			ring->semaphore.mbox.signal[RCS] = GEN6_RBSYNC;
			ring->semaphore.mbox.signal[VCS] = GEN6_VBSYNC;
			ring->semaphore.mbox.signal[BCS] = GEN6_NOSYNC;
			ring->semaphore.mbox.signal[VECS] = GEN6_VEBSYNC;
			ring->semaphore.mbox.signal[VCS2] = GEN6_NOSYNC;
		}
	}
	ring->init_hw = init_ring_common;

	return intel_init_ring_buffer(dev, ring);
}

int intel_init_vebox_ring_buffer(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring = &dev_priv->ring[VECS];

	ring->name = "video enhancement ring";
	ring->id = VECS;

	ring->mmio_base = VEBOX_RING_BASE;
	ring->write_tail = ring_write_tail;
	ring->flush = gen6_ring_flush;
	ring->add_request = gen6_add_request;
	ring->get_seqno = gen6_ring_get_seqno;
	ring->set_seqno = ring_set_seqno;

	if (INTEL_INFO(dev)->gen >= 8) {
		ring->irq_enable_mask =
			GT_RENDER_USER_INTERRUPT << GEN8_VECS_IRQ_SHIFT;
		ring->irq_get = gen8_ring_get_irq;
		ring->irq_put = gen8_ring_put_irq;
		ring->dispatch_execbuffer = gen8_ring_dispatch_execbuffer;
		if (i915_semaphore_is_enabled(dev)) {
			ring->semaphore.sync_to = gen8_ring_sync;
			ring->semaphore.signal = gen8_xcs_signal;
			GEN8_RING_SEMAPHORE_INIT;
		}
	} else {
		ring->irq_enable_mask = PM_VEBOX_USER_INTERRUPT;
		ring->irq_get = hsw_vebox_get_irq;
		ring->irq_put = hsw_vebox_put_irq;
		ring->dispatch_execbuffer = gen6_ring_dispatch_execbuffer;
		if (i915_semaphore_is_enabled(dev)) {
			ring->semaphore.sync_to = gen6_ring_sync;
			ring->semaphore.signal = gen6_signal;
			ring->semaphore.mbox.wait[RCS] = MI_SEMAPHORE_SYNC_VER;
			ring->semaphore.mbox.wait[VCS] = MI_SEMAPHORE_SYNC_VEV;
			ring->semaphore.mbox.wait[BCS] = MI_SEMAPHORE_SYNC_VEB;
			ring->semaphore.mbox.wait[VECS] = MI_SEMAPHORE_SYNC_INVALID;
			ring->semaphore.mbox.wait[VCS2] = MI_SEMAPHORE_SYNC_INVALID;
			ring->semaphore.mbox.signal[RCS] = GEN6_RVESYNC;
			ring->semaphore.mbox.signal[VCS] = GEN6_VVESYNC;
			ring->semaphore.mbox.signal[BCS] = GEN6_BVESYNC;
			ring->semaphore.mbox.signal[VECS] = GEN6_NOSYNC;
			ring->semaphore.mbox.signal[VCS2] = GEN6_NOSYNC;
		}
	}
	ring->init_hw = init_ring_common;

	return intel_init_ring_buffer(dev, ring);
}

int
intel_ring_flush_all_caches(struct drm_i915_gem_request *req)
{
	struct intel_engine_cs *ring = req->ring;
	int ret;

	if (!ring->gpu_caches_dirty)
		return 0;

	ret = ring->flush(req, 0, I915_GEM_GPU_DOMAINS);
	if (ret)
		return ret;

	trace_i915_gem_ring_flush(req, 0, I915_GEM_GPU_DOMAINS);

	ring->gpu_caches_dirty = false;
	return 0;
}

int
intel_ring_invalidate_all_caches(struct drm_i915_gem_request *req)
{
	struct intel_engine_cs *ring = req->ring;
	uint32_t flush_domains;
	int ret;

	flush_domains = 0;
	if (ring->gpu_caches_dirty)
		flush_domains = I915_GEM_GPU_DOMAINS;

	ret = ring->flush(req, I915_GEM_GPU_DOMAINS, flush_domains);
	if (ret)
		return ret;

	trace_i915_gem_ring_flush(req, I915_GEM_GPU_DOMAINS, flush_domains);

	ring->gpu_caches_dirty = false;
	return 0;
}

void
intel_stop_ring_buffer(struct intel_engine_cs *ring)
{
	int ret;

	if (!intel_ring_initialized(ring))
		return;

	ret = intel_ring_idle(ring);
	if (ret && !i915_reset_in_progress(&to_i915(ring->dev)->gpu_error))
		DRM_ERROR("failed to quiesce %s whilst cleaning up: %d\n",
			  ring->name, ret);

	stop_ring(ring);
=======
int intel_init_bsd2_ring_buffer(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	intel_ring_default_vfuncs(dev_priv, engine);

	engine->emit_flush = gen6_bsd_ring_flush;

	return intel_init_ring_buffer(engine);
}

int intel_init_blt_ring_buffer(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	intel_ring_default_vfuncs(dev_priv, engine);

	engine->emit_flush = gen6_ring_flush;
	if (INTEL_GEN(dev_priv) < 8)
		engine->irq_enable_mask = GT_BLT_USER_INTERRUPT;

	return intel_init_ring_buffer(engine);
}

int intel_init_vebox_ring_buffer(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	intel_ring_default_vfuncs(dev_priv, engine);

	engine->emit_flush = gen6_ring_flush;

	if (INTEL_GEN(dev_priv) < 8) {
		engine->irq_enable_mask = PM_VEBOX_USER_INTERRUPT;
		engine->irq_enable = hsw_vebox_irq_enable;
		engine->irq_disable = hsw_vebox_irq_disable;
	}

	return intel_init_ring_buffer(engine);
>>>>>>> v4.9.227
}
