/*
 * Copyright © 2014 Intel Corporation
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
 *    Ben Widawsky <ben@bwidawsk.net>
 *    Michel Thierry <michel.thierry@intel.com>
 *    Thomas Daniel <thomas.daniel@intel.com>
 *    Oscar Mateo <oscar.mateo@intel.com>
 *
 */

/**
 * DOC: Logical Rings, Logical Ring Contexts and Execlists
 *
 * Motivation:
 * GEN8 brings an expansion of the HW contexts: "Logical Ring Contexts".
 * These expanded contexts enable a number of new abilities, especially
 * "Execlists" (also implemented in this file).
 *
 * One of the main differences with the legacy HW contexts is that logical
 * ring contexts incorporate many more things to the context's state, like
 * PDPs or ringbuffer control registers:
 *
 * The reason why PDPs are included in the context is straightforward: as
 * PPGTTs (per-process GTTs) are actually per-context, having the PDPs
 * contained there mean you don't need to do a ppgtt->switch_mm yourself,
 * instead, the GPU will do it for you on the context switch.
 *
 * But, what about the ringbuffer control registers (head, tail, etc..)?
 * shouldn't we just need a set of those per engine command streamer? This is
 * where the name "Logical Rings" starts to make sense: by virtualizing the
 * rings, the engine cs shifts to a new "ring buffer" with every context
 * switch. When you want to submit a workload to the GPU you: A) choose your
 * context, B) find its appropriate virtualized ring, C) write commands to it
 * and then, finally, D) tell the GPU to switch to that context.
 *
 * Instead of the legacy MI_SET_CONTEXT, the way you tell the GPU to switch
 * to a contexts is via a context execution list, ergo "Execlists".
 *
 * LRC implementation:
 * Regarding the creation of contexts, we have:
 *
 * - One global default context.
 * - One local default context for each opened fd.
 * - One local extra context for each context create ioctl call.
 *
 * Now that ringbuffers belong per-context (and not per-engine, like before)
 * and that contexts are uniquely tied to a given engine (and not reusable,
 * like before) we need:
 *
 * - One ringbuffer per-engine inside each context.
 * - One backing object per-engine inside each context.
 *
 * The global default context starts its life with these new objects fully
 * allocated and populated. The local default context for each opened fd is
 * more complex, because we don't know at creation time which engine is going
 * to use them. To handle this, we have implemented a deferred creation of LR
 * contexts:
 *
 * The local context starts its life as a hollow or blank holder, that only
 * gets populated for a given engine once we receive an execbuffer. If later
 * on we receive another execbuffer ioctl for the same context but a different
 * engine, we allocate/populate a new ringbuffer and context backing object and
 * so on.
 *
 * Finally, regarding local contexts created using the ioctl call: as they are
 * only allowed with the render ring, we can allocate & populate them right
 * away (no need to defer anything, at least for now).
 *
 * Execlists implementation:
 * Execlists are the new method by which, on gen8+ hardware, workloads are
 * submitted for execution (as opposed to the legacy, ringbuffer-based, method).
 * This method works as follows:
 *
 * When a request is committed, its commands (the BB start and any leading or
 * trailing commands, like the seqno breadcrumbs) are placed in the ringbuffer
 * for the appropriate context. The tail pointer in the hardware context is not
 * updated at this time, but instead, kept by the driver in the ringbuffer
 * structure. A structure representing this request is added to a request queue
 * for the appropriate engine: this structure contains a copy of the context's
 * tail after the request was written to the ring buffer and a pointer to the
 * context itself.
 *
 * If the engine's request queue was empty before the request was added, the
 * queue is processed immediately. Otherwise the queue will be processed during
 * a context switch interrupt. In any case, elements on the queue will get sent
 * (in pairs) to the GPU's ExecLists Submit Port (ELSP, for short) with a
 * globally unique 20-bits submission ID.
 *
 * When execution of a request completes, the GPU updates the context status
 * buffer with a context complete event and generates a context switch interrupt.
 * During the interrupt handling, the driver examines the events in the buffer:
 * for each context complete event, if the announced ID matches that on the head
 * of the request queue, then that request is retired and removed from the queue.
 *
 * After processing, if any requests were retired and the queue is not empty
 * then a new execution list can be submitted. The two requests at the front of
 * the queue are next to be submitted but since a context may not occur twice in
 * an execution list, if subsequent requests have the same ID as the first then
 * the two requests must be combined. This is done simply by discarding requests
 * at the head of the queue until either only one requests is left (in which case
 * we use a NULL second context) or the first two requests have unique IDs.
 *
 * By always executing the first two requests in the queue the driver ensures
 * that the GPU is kept as busy as possible. In the case where a single context
 * completes but a second context is still executing, the request for this second
 * context will be at the head of the queue when we remove the first one. This
 * request will then be resubmitted along with a new request for a different context,
 * which will cause the hardware to continue executing the second request and queue
 * the new request (the GPU detects the condition of a context getting preempted
 * with the same context and optimizes the context switch flow by not doing
 * preemption, but just sampling the new tail pointer).
 *
 */
<<<<<<< HEAD
=======
#include <linux/interrupt.h>
>>>>>>> v4.9.227

#include <drm/drmP.h>
#include <drm/i915_drm.h>
#include "i915_drv.h"
#include "intel_mocs.h"

#define GEN9_LR_CONTEXT_RENDER_SIZE (22 * PAGE_SIZE)
#define GEN8_LR_CONTEXT_RENDER_SIZE (20 * PAGE_SIZE)
#define GEN8_LR_CONTEXT_OTHER_SIZE (2 * PAGE_SIZE)

#define RING_EXECLIST_QFULL		(1 << 0x2)
#define RING_EXECLIST1_VALID		(1 << 0x3)
#define RING_EXECLIST0_VALID		(1 << 0x4)
#define RING_EXECLIST_ACTIVE_STATUS	(3 << 0xE)
#define RING_EXECLIST1_ACTIVE		(1 << 0x11)
#define RING_EXECLIST0_ACTIVE		(1 << 0x12)

#define GEN8_CTX_STATUS_IDLE_ACTIVE	(1 << 0)
#define GEN8_CTX_STATUS_PREEMPTED	(1 << 1)
#define GEN8_CTX_STATUS_ELEMENT_SWITCH	(1 << 2)
#define GEN8_CTX_STATUS_ACTIVE_IDLE	(1 << 3)
#define GEN8_CTX_STATUS_COMPLETE	(1 << 4)
#define GEN8_CTX_STATUS_LITE_RESTORE	(1 << 15)

<<<<<<< HEAD
=======
#define GEN8_CTX_STATUS_COMPLETED_MASK \
	 (GEN8_CTX_STATUS_ACTIVE_IDLE | \
	  GEN8_CTX_STATUS_PREEMPTED | \
	  GEN8_CTX_STATUS_ELEMENT_SWITCH)

>>>>>>> v4.9.227
#define CTX_LRI_HEADER_0		0x01
#define CTX_CONTEXT_CONTROL		0x02
#define CTX_RING_HEAD			0x04
#define CTX_RING_TAIL			0x06
#define CTX_RING_BUFFER_START		0x08
#define CTX_RING_BUFFER_CONTROL		0x0a
#define CTX_BB_HEAD_U			0x0c
#define CTX_BB_HEAD_L			0x0e
#define CTX_BB_STATE			0x10
#define CTX_SECOND_BB_HEAD_U		0x12
#define CTX_SECOND_BB_HEAD_L		0x14
#define CTX_SECOND_BB_STATE		0x16
#define CTX_BB_PER_CTX_PTR		0x18
#define CTX_RCS_INDIRECT_CTX		0x1a
#define CTX_RCS_INDIRECT_CTX_OFFSET	0x1c
#define CTX_LRI_HEADER_1		0x21
#define CTX_CTX_TIMESTAMP		0x22
#define CTX_PDP3_UDW			0x24
#define CTX_PDP3_LDW			0x26
#define CTX_PDP2_UDW			0x28
#define CTX_PDP2_LDW			0x2a
#define CTX_PDP1_UDW			0x2c
#define CTX_PDP1_LDW			0x2e
#define CTX_PDP0_UDW			0x30
#define CTX_PDP0_LDW			0x32
#define CTX_LRI_HEADER_2		0x41
#define CTX_R_PWR_CLK_STATE		0x42
#define CTX_GPGPU_CSR_BASE_ADDRESS	0x44

#define GEN8_CTX_VALID (1<<0)
#define GEN8_CTX_FORCE_PD_RESTORE (1<<1)
#define GEN8_CTX_FORCE_RESTORE (1<<2)
#define GEN8_CTX_L3LLC_COHERENT (1<<5)
#define GEN8_CTX_PRIVILEGE (1<<8)

<<<<<<< HEAD
#define ASSIGN_CTX_PDP(ppgtt, reg_state, n) { \
	const u64 _addr = i915_page_dir_dma_addr((ppgtt), (n));	\
	reg_state[CTX_PDP ## n ## _UDW+1] = upper_32_bits(_addr); \
	reg_state[CTX_PDP ## n ## _LDW+1] = lower_32_bits(_addr); \
}

#define ASSIGN_CTX_PML4(ppgtt, reg_state) { \
	reg_state[CTX_PDP0_UDW + 1] = upper_32_bits(px_dma(&ppgtt->pml4)); \
	reg_state[CTX_PDP0_LDW + 1] = lower_32_bits(px_dma(&ppgtt->pml4)); \
}

enum {
	ADVANCED_CONTEXT = 0,
	LEGACY_32B_CONTEXT,
	ADVANCED_AD_CONTEXT,
	LEGACY_64B_CONTEXT
};
#define GEN8_CTX_ADDRESSING_MODE_SHIFT 3
#define GEN8_CTX_ADDRESSING_MODE(dev)  (USES_FULL_48BIT_PPGTT(dev) ?\
		LEGACY_64B_CONTEXT :\
		LEGACY_32B_CONTEXT)
enum {
=======
#define ASSIGN_CTX_REG(reg_state, pos, reg, val) do { \
	(reg_state)[(pos)+0] = i915_mmio_reg_offset(reg); \
	(reg_state)[(pos)+1] = (val); \
} while (0)

#define ASSIGN_CTX_PDP(ppgtt, reg_state, n) do {		\
	const u64 _addr = i915_page_dir_dma_addr((ppgtt), (n));	\
	reg_state[CTX_PDP ## n ## _UDW+1] = upper_32_bits(_addr); \
	reg_state[CTX_PDP ## n ## _LDW+1] = lower_32_bits(_addr); \
} while (0)

#define ASSIGN_CTX_PML4(ppgtt, reg_state) do { \
	reg_state[CTX_PDP0_UDW + 1] = upper_32_bits(px_dma(&ppgtt->pml4)); \
	reg_state[CTX_PDP0_LDW + 1] = lower_32_bits(px_dma(&ppgtt->pml4)); \
} while (0)

enum {
>>>>>>> v4.9.227
	FAULT_AND_HANG = 0,
	FAULT_AND_HALT, /* Debug only */
	FAULT_AND_STREAM,
	FAULT_AND_CONTINUE /* Unsupported */
};
#define GEN8_CTX_ID_SHIFT 32
<<<<<<< HEAD
#define CTX_RCS_INDIRECT_CTX_OFFSET_DEFAULT  0x17

static int intel_lr_context_pin(struct drm_i915_gem_request *rq);
static void lrc_setup_hardware_status_page(struct intel_engine_cs *ring,
		struct drm_i915_gem_object *default_ctx_obj);


/**
 * intel_sanitize_enable_execlists() - sanitize i915.enable_execlists
 * @dev: DRM device.
=======
#define GEN8_CTX_ID_WIDTH 21
#define GEN8_CTX_RCS_INDIRECT_CTX_OFFSET_DEFAULT	0x17
#define GEN9_CTX_RCS_INDIRECT_CTX_OFFSET_DEFAULT	0x26

/* Typical size of the average request (2 pipecontrols and a MI_BB) */
#define EXECLISTS_REQUEST_SIZE 64 /* bytes */

#define WA_TAIL_DWORDS 2

static int execlists_context_deferred_alloc(struct i915_gem_context *ctx,
					    struct intel_engine_cs *engine);
static int intel_lr_context_pin(struct i915_gem_context *ctx,
				struct intel_engine_cs *engine);
static void execlists_init_reg_state(u32 *reg_state,
				     struct i915_gem_context *ctx,
				     struct intel_engine_cs *engine,
				     struct intel_ring *ring);

/**
 * intel_sanitize_enable_execlists() - sanitize i915.enable_execlists
 * @dev_priv: i915 device private
>>>>>>> v4.9.227
 * @enable_execlists: value of i915.enable_execlists module parameter.
 *
 * Only certain platforms support Execlists (the prerequisites being
 * support for Logical Ring Contexts and Aliasing PPGTT or better).
 *
 * Return: 1 if Execlists is supported and has to be enabled.
 */
<<<<<<< HEAD
int intel_sanitize_enable_execlists(struct drm_device *dev, int enable_execlists)
{
	WARN_ON(i915.enable_ppgtt == -1);

	/* On platforms with execlist available, vGPU will only
	 * support execlist mode, no ring buffer mode.
	 */
	if (HAS_LOGICAL_RING_CONTEXTS(dev) && intel_vgpu_active(dev))
		return 1;

	if (INTEL_INFO(dev)->gen >= 9)
=======
int intel_sanitize_enable_execlists(struct drm_i915_private *dev_priv, int enable_execlists)
{
	/* On platforms with execlist available, vGPU will only
	 * support execlist mode, no ring buffer mode.
	 */
	if (HAS_LOGICAL_RING_CONTEXTS(dev_priv) && intel_vgpu_active(dev_priv))
		return 1;

	if (INTEL_GEN(dev_priv) >= 9)
>>>>>>> v4.9.227
		return 1;

	if (enable_execlists == 0)
		return 0;

<<<<<<< HEAD
	if (HAS_LOGICAL_RING_CONTEXTS(dev) && USES_PPGTT(dev) &&
=======
	if (HAS_LOGICAL_RING_CONTEXTS(dev_priv) &&
	    USES_PPGTT(dev_priv) &&
>>>>>>> v4.9.227
	    i915.use_mmio_flip >= 0)
		return 1;

	return 0;
}

<<<<<<< HEAD
/**
 * intel_execlists_ctx_id() - get the Execlists Context ID
 * @ctx_obj: Logical Ring Context backing object.
 *
 * Do not confuse with ctx->id! Unfortunately we have a name overload
 * here: the old context ID we pass to userspace as a handler so that
 * they can refer to a context, and the new context ID we pass to the
 * ELSP so that the GPU can inform us of the context status via
 * interrupts.
 *
 * Return: 20-bits globally unique context ID.
 */
u32 intel_execlists_ctx_id(struct drm_i915_gem_object *ctx_obj)
{
	u32 lrca = i915_gem_obj_ggtt_offset(ctx_obj) +
			LRC_PPHWSP_PN * PAGE_SIZE;

	/* LRCA is required to be 4K aligned so the more significant 20 bits
	 * are globally unique */
	return lrca >> 12;
}

static bool disable_lite_restore_wa(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;

	return ((IS_SKYLAKE(dev) && INTEL_REVID(dev) <= SKL_REVID_B0) ||
		(IS_BROXTON(dev) && INTEL_REVID(dev) == BXT_REVID_A0)) &&
	       (ring->id == VCS || ring->id == VCS2);
}

uint64_t intel_lr_context_descriptor(struct intel_context *ctx,
				     struct intel_engine_cs *ring)
{
	struct drm_i915_gem_object *ctx_obj = ctx->engine[ring->id].state;
	uint64_t desc;
	uint64_t lrca = i915_gem_obj_ggtt_offset(ctx_obj) +
			LRC_PPHWSP_PN * PAGE_SIZE;

	WARN_ON(lrca & 0xFFFFFFFF00000FFFULL);

	desc = GEN8_CTX_VALID;
	desc |= GEN8_CTX_ADDRESSING_MODE(dev) << GEN8_CTX_ADDRESSING_MODE_SHIFT;
	if (IS_GEN8(ctx_obj->base.dev))
		desc |= GEN8_CTX_L3LLC_COHERENT;
	desc |= GEN8_CTX_PRIVILEGE;
	desc |= lrca;
	desc |= (u64)intel_execlists_ctx_id(ctx_obj) << GEN8_CTX_ID_SHIFT;

	/* TODO: WaDisableLiteRestore when we start using semaphore
	 * signalling between Command Streamers */
	/* desc |= GEN8_CTX_FORCE_RESTORE; */

	/* WaEnableForceRestoreInCtxtDescForVCS:skl */
	/* WaEnableForceRestoreInCtxtDescForVCS:bxt */
	if (disable_lite_restore_wa(ring))
		desc |= GEN8_CTX_FORCE_RESTORE;

	return desc;
}

static void execlists_elsp_write(struct drm_i915_gem_request *rq0,
				 struct drm_i915_gem_request *rq1)
{

	struct intel_engine_cs *ring = rq0->ring;
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	uint64_t desc[2];

	if (rq1) {
		desc[1] = intel_lr_context_descriptor(rq1->ctx, rq1->ring);
		rq1->elsp_submitted++;
	} else {
		desc[1] = 0;
	}

	desc[0] = intel_lr_context_descriptor(rq0->ctx, rq0->ring);
	rq0->elsp_submitted++;

	/* You must always write both descriptors in the order below. */
	spin_lock(&dev_priv->uncore.lock);
	intel_uncore_forcewake_get__locked(dev_priv, FORCEWAKE_ALL);
	I915_WRITE_FW(RING_ELSP(ring), upper_32_bits(desc[1]));
	I915_WRITE_FW(RING_ELSP(ring), lower_32_bits(desc[1]));

	I915_WRITE_FW(RING_ELSP(ring), upper_32_bits(desc[0]));
	/* The context is automatically loaded after the following */
	I915_WRITE_FW(RING_ELSP(ring), lower_32_bits(desc[0]));

	/* ELSP is a wo register, use another nearby reg for posting */
	POSTING_READ_FW(RING_EXECLIST_STATUS_LO(ring));
	intel_uncore_forcewake_put__locked(dev_priv, FORCEWAKE_ALL);
	spin_unlock(&dev_priv->uncore.lock);
}

static int execlists_update_context(struct drm_i915_gem_request *rq)
{
	struct intel_engine_cs *ring = rq->ring;
	struct i915_hw_ppgtt *ppgtt = rq->ctx->ppgtt;
	struct drm_i915_gem_object *ctx_obj = rq->ctx->engine[ring->id].state;
	struct drm_i915_gem_object *rb_obj = rq->ringbuf->obj;
	struct page *page;
	uint32_t *reg_state;

	BUG_ON(!ctx_obj);
	WARN_ON(!i915_gem_obj_is_pinned(ctx_obj));
	WARN_ON(!i915_gem_obj_is_pinned(rb_obj));

	page = i915_gem_object_get_page(ctx_obj, LRC_STATE_PN);
	reg_state = kmap_atomic(page);

	reg_state[CTX_RING_TAIL+1] = rq->tail;
	reg_state[CTX_RING_BUFFER_START+1] = i915_gem_obj_ggtt_offset(rb_obj);

	if (ppgtt && !USES_FULL_48BIT_PPGTT(ppgtt->base.dev)) {
		/* True 32b PPGTT with dynamic page allocation: update PDP
		 * registers and point the unallocated PDPs to scratch page.
		 * PML4 is allocated during ppgtt init, so this is not needed
		 * in 48-bit mode.
		 */
		ASSIGN_CTX_PDP(ppgtt, reg_state, 3);
		ASSIGN_CTX_PDP(ppgtt, reg_state, 2);
		ASSIGN_CTX_PDP(ppgtt, reg_state, 1);
		ASSIGN_CTX_PDP(ppgtt, reg_state, 0);
	}

	kunmap_atomic(reg_state);

	return 0;
}

static void execlists_submit_requests(struct drm_i915_gem_request *rq0,
				      struct drm_i915_gem_request *rq1)
{
	execlists_update_context(rq0);

	if (rq1)
		execlists_update_context(rq1);

	execlists_elsp_write(rq0, rq1);
}

static void execlists_context_unqueue(struct intel_engine_cs *ring)
{
	struct drm_i915_gem_request *req0 = NULL, *req1 = NULL;
	struct drm_i915_gem_request *cursor = NULL, *tmp = NULL;

	assert_spin_locked(&ring->execlist_lock);

	/*
	 * If irqs are not active generate a warning as batches that finish
	 * without the irqs may get lost and a GPU Hang may occur.
	 */
	WARN_ON(!intel_irqs_enabled(ring->dev->dev_private));

	if (list_empty(&ring->execlist_queue))
		return;

	/* Try to read in pairs */
	list_for_each_entry_safe(cursor, tmp, &ring->execlist_queue,
				 execlist_link) {
		if (!req0) {
			req0 = cursor;
		} else if (req0->ctx == cursor->ctx) {
			/* Same ctx: ignore first request, as second request
			 * will update tail past first request's workload */
			cursor->elsp_submitted = req0->elsp_submitted;
			list_del(&req0->execlist_link);
			list_add_tail(&req0->execlist_link,
				&ring->execlist_retired_req_list);
			req0 = cursor;
		} else {
			req1 = cursor;
			break;
		}
	}

	if (IS_GEN8(ring->dev) || IS_GEN9(ring->dev)) {
		/*
		 * WaIdleLiteRestore: make sure we never cause a lite
		 * restore with HEAD==TAIL
		 */
		if (req0->elsp_submitted) {
			/*
			 * Apply the wa NOOPS to prevent ring:HEAD == req:TAIL
			 * as we resubmit the request. See gen8_emit_request()
			 * for where we prepare the padding after the end of the
			 * request.
			 */
			struct intel_ringbuffer *ringbuf;

			ringbuf = req0->ctx->engine[ring->id].ringbuf;
			req0->tail += 8;
			req0->tail &= ringbuf->size - 1;
		}
	}

	WARN_ON(req1 && req1->elsp_submitted);

	execlists_submit_requests(req0, req1);
}

static bool execlists_check_remove_request(struct intel_engine_cs *ring,
					   u32 request_id)
{
	struct drm_i915_gem_request *head_req;

	assert_spin_locked(&ring->execlist_lock);

	head_req = list_first_entry_or_null(&ring->execlist_queue,
					    struct drm_i915_gem_request,
					    execlist_link);

	if (head_req != NULL) {
		struct drm_i915_gem_object *ctx_obj =
				head_req->ctx->engine[ring->id].state;
		if (intel_execlists_ctx_id(ctx_obj) == request_id) {
			WARN(head_req->elsp_submitted == 0,
			     "Never submitted head request\n");

			if (--head_req->elsp_submitted <= 0) {
				list_del(&head_req->execlist_link);
				list_add_tail(&head_req->execlist_link,
					&ring->execlist_retired_req_list);
				return true;
			}
		}
	}

	return false;
}

/**
 * intel_lrc_irq_handler() - handle Context Switch interrupts
 * @ring: Engine Command Streamer to handle.
 *
 * Check the unread Context Status Buffers and manage the submission of new
 * contexts to the ELSP accordingly.
 */
void intel_lrc_irq_handler(struct intel_engine_cs *ring)
{
	struct drm_i915_private *dev_priv = ring->dev->dev_private;
	u32 status_pointer;
	u8 read_pointer;
	u8 write_pointer;
	u32 status = 0;
	u32 status_id;
	u32 submit_contexts = 0;

	status_pointer = I915_READ(RING_CONTEXT_STATUS_PTR(ring));

	read_pointer = ring->next_context_status_buffer;
	write_pointer = status_pointer & GEN8_CSB_PTR_MASK;
	if (read_pointer > write_pointer)
		write_pointer += GEN8_CSB_ENTRIES;

	spin_lock(&ring->execlist_lock);

	while (read_pointer < write_pointer) {
		read_pointer++;
		status = I915_READ(RING_CONTEXT_STATUS_BUF_LO(ring, read_pointer % GEN8_CSB_ENTRIES));
		status_id = I915_READ(RING_CONTEXT_STATUS_BUF_HI(ring, read_pointer % GEN8_CSB_ENTRIES));

		if (status & GEN8_CTX_STATUS_IDLE_ACTIVE)
			continue;

		if (status & GEN8_CTX_STATUS_PREEMPTED) {
			if (status & GEN8_CTX_STATUS_LITE_RESTORE) {
				if (execlists_check_remove_request(ring, status_id))
					WARN(1, "Lite Restored request removed from queue\n");
			} else
				WARN(1, "Preemption without Lite Restore\n");
		}

		 if ((status & GEN8_CTX_STATUS_ACTIVE_IDLE) ||
		     (status & GEN8_CTX_STATUS_ELEMENT_SWITCH)) {
			if (execlists_check_remove_request(ring, status_id))
				submit_contexts++;
		}
	}

	if (disable_lite_restore_wa(ring)) {
		/* Prevent a ctx to preempt itself */
		if ((status & GEN8_CTX_STATUS_ACTIVE_IDLE) &&
		    (submit_contexts != 0))
			execlists_context_unqueue(ring);
	} else if (submit_contexts != 0) {
		execlists_context_unqueue(ring);
	}

	spin_unlock(&ring->execlist_lock);

	WARN(submit_contexts > 2, "More than two context complete events?\n");
	ring->next_context_status_buffer = write_pointer % GEN8_CSB_ENTRIES;

	I915_WRITE(RING_CONTEXT_STATUS_PTR(ring),
		   _MASKED_FIELD(GEN8_CSB_PTR_MASK << 8,
				 ((u32)ring->next_context_status_buffer &
				  GEN8_CSB_PTR_MASK) << 8));
}

static int execlists_context_queue(struct drm_i915_gem_request *request)
{
	struct intel_engine_cs *ring = request->ring;
	struct drm_i915_gem_request *cursor;
	int num_elements = 0;

	if (request->ctx != ring->default_context)
		intel_lr_context_pin(request);

	i915_gem_request_reference(request);

	spin_lock_irq(&ring->execlist_lock);

	list_for_each_entry(cursor, &ring->execlist_queue, execlist_link)
		if (++num_elements > 2)
			break;

	if (num_elements > 2) {
		struct drm_i915_gem_request *tail_req;

		tail_req = list_last_entry(&ring->execlist_queue,
					   struct drm_i915_gem_request,
					   execlist_link);

		if (request->ctx == tail_req->ctx) {
			WARN(tail_req->elsp_submitted != 0,
				"More than 2 already-submitted reqs queued\n");
			list_del(&tail_req->execlist_link);
			list_add_tail(&tail_req->execlist_link,
				&ring->execlist_retired_req_list);
		}
	}

	list_add_tail(&request->execlist_link, &ring->execlist_queue);
	if (num_elements == 0)
		execlists_context_unqueue(ring);

	spin_unlock_irq(&ring->execlist_lock);

	return 0;
}

static int logical_ring_invalidate_all_caches(struct drm_i915_gem_request *req)
{
	struct intel_engine_cs *ring = req->ring;
	uint32_t flush_domains;
	int ret;

	flush_domains = 0;
	if (ring->gpu_caches_dirty)
		flush_domains = I915_GEM_GPU_DOMAINS;

	ret = ring->emit_flush(req, I915_GEM_GPU_DOMAINS, flush_domains);
	if (ret)
		return ret;

	ring->gpu_caches_dirty = false;
	return 0;
}

static int execlists_move_to_gpu(struct drm_i915_gem_request *req,
				 struct list_head *vmas)
{
	const unsigned other_rings = ~intel_ring_flag(req->ring);
	struct i915_vma *vma;
	uint32_t flush_domains = 0;
	bool flush_chipset = false;
	int ret;

	list_for_each_entry(vma, vmas, exec_list) {
		struct drm_i915_gem_object *obj = vma->obj;

		if (obj->active & other_rings) {
			ret = i915_gem_object_sync(obj, req->ring, &req);
			if (ret)
				return ret;
		}

		if (obj->base.write_domain & I915_GEM_DOMAIN_CPU)
			flush_chipset |= i915_gem_clflush_object(obj, false);

		flush_domains |= obj->base.write_domain;
	}

	if (flush_domains & I915_GEM_DOMAIN_GTT)
		wmb();

	/* Unconditionally invalidate gpu caches and ensure that we do flush
	 * any residual writes from the previous batch.
	 */
	return logical_ring_invalidate_all_caches(req);
=======
static void
logical_ring_init_platform_invariants(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	engine->disable_lite_restore_wa =
		(IS_SKL_REVID(dev_priv, 0, SKL_REVID_B0) ||
		 IS_BXT_REVID(dev_priv, 0, BXT_REVID_A1)) &&
		(engine->id == VCS || engine->id == VCS2);

	engine->ctx_desc_template = GEN8_CTX_VALID;
	if (IS_GEN8(dev_priv))
		engine->ctx_desc_template |= GEN8_CTX_L3LLC_COHERENT;
	engine->ctx_desc_template |= GEN8_CTX_PRIVILEGE;

	/* TODO: WaDisableLiteRestore when we start using semaphore
	 * signalling between Command Streamers */
	/* ring->ctx_desc_template |= GEN8_CTX_FORCE_RESTORE; */

	/* WaEnableForceRestoreInCtxtDescForVCS:skl */
	/* WaEnableForceRestoreInCtxtDescForVCS:bxt */
	if (engine->disable_lite_restore_wa)
		engine->ctx_desc_template |= GEN8_CTX_FORCE_RESTORE;
}

/**
 * intel_lr_context_descriptor_update() - calculate & cache the descriptor
 * 					  descriptor for a pinned context
 * @ctx: Context to work on
 * @engine: Engine the descriptor will be used with
 *
 * The context descriptor encodes various attributes of a context,
 * including its GTT address and some flags. Because it's fairly
 * expensive to calculate, we'll just do it once and cache the result,
 * which remains valid until the context is unpinned.
 *
 * This is what a descriptor looks like, from LSB to MSB::
 *
 *      bits  0-11:    flags, GEN8_CTX_* (cached in ctx_desc_template)
 *      bits 12-31:    LRCA, GTT address of (the HWSP of) this context
 *      bits 32-52:    ctx ID, a globally unique tag
 *      bits 53-54:    mbz, reserved for use by hardware
 *      bits 55-63:    group ID, currently unused and set to 0
 */
static void
intel_lr_context_descriptor_update(struct i915_gem_context *ctx,
				   struct intel_engine_cs *engine)
{
	struct intel_context *ce = &ctx->engine[engine->id];
	u64 desc;

	BUILD_BUG_ON(MAX_CONTEXT_HW_ID > (1<<GEN8_CTX_ID_WIDTH));

	desc = ctx->desc_template;				/* bits  3-4  */
	desc |= engine->ctx_desc_template;			/* bits  0-11 */
	desc |= i915_ggtt_offset(ce->state) + LRC_PPHWSP_PN * PAGE_SIZE;
								/* bits 12-31 */
	desc |= (u64)ctx->hw_id << GEN8_CTX_ID_SHIFT;		/* bits 32-52 */

	ce->lrc_desc = desc;
}

uint64_t intel_lr_context_descriptor(struct i915_gem_context *ctx,
				     struct intel_engine_cs *engine)
{
	return ctx->engine[engine->id].lrc_desc;
}

static inline void
execlists_context_status_change(struct drm_i915_gem_request *rq,
				unsigned long status)
{
	/*
	 * Only used when GVT-g is enabled now. When GVT-g is disabled,
	 * The compiler should eliminate this function as dead-code.
	 */
	if (!IS_ENABLED(CONFIG_DRM_I915_GVT))
		return;

	atomic_notifier_call_chain(&rq->ctx->status_notifier, status, rq);
}

static void
execlists_update_context_pdps(struct i915_hw_ppgtt *ppgtt, u32 *reg_state)
{
	ASSIGN_CTX_PDP(ppgtt, reg_state, 3);
	ASSIGN_CTX_PDP(ppgtt, reg_state, 2);
	ASSIGN_CTX_PDP(ppgtt, reg_state, 1);
	ASSIGN_CTX_PDP(ppgtt, reg_state, 0);
}

static u64 execlists_update_context(struct drm_i915_gem_request *rq)
{
	struct intel_context *ce = &rq->ctx->engine[rq->engine->id];
	struct i915_hw_ppgtt *ppgtt = rq->ctx->ppgtt;
	u32 *reg_state = ce->lrc_reg_state;

	reg_state[CTX_RING_TAIL+1] = intel_ring_offset(rq->ring, rq->tail);

	/*
	 * True 32b PPGTT with dynamic page allocation: update PDP
	 * registers and point the unallocated PDPs to scratch page.
	 * PML4 is allocated during ppgtt init, so this is not needed
	 * in 48-bit mode.
	 */
	if (ppgtt && !USES_FULL_48BIT_PPGTT(ppgtt->base.dev))
		execlists_update_context_pdps(ppgtt, reg_state);

	/*
	 * Make sure the context image is complete before we submit it to HW.
	 *
	 * Ostensibly, writes (including the WCB) should be flushed prior to
	 * an uncached write such as our mmio register access, the empirical
	 * evidence (esp. on Braswell) suggests that the WC write into memory
	 * may not be visible to the HW prior to the completion of the UC
	 * register write and that we may begin execution from the context
	 * before its image is complete leading to invalid PD chasing.
	 *
	 * Furthermore, Braswell, at least, wants a full mb to be sure that
	 * the writes are coherent in memory (visible to the GPU) prior to
	 * execution, and not just visible to other CPUs (as is the result of
	 * wmb).
	 */
	mb();
	return ce->lrc_desc;
}

static void execlists_submit_ports(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	struct execlist_port *port = engine->execlist_port;
	u32 __iomem *elsp =
		dev_priv->regs + i915_mmio_reg_offset(RING_ELSP(engine));
	u64 desc[2];

	if (!port[0].count)
		execlists_context_status_change(port[0].request,
						INTEL_CONTEXT_SCHEDULE_IN);
	desc[0] = execlists_update_context(port[0].request);
	engine->preempt_wa = port[0].count++; /* bdw only? fixed on skl? */

	if (port[1].request) {
		GEM_BUG_ON(port[1].count);
		execlists_context_status_change(port[1].request,
						INTEL_CONTEXT_SCHEDULE_IN);
		desc[1] = execlists_update_context(port[1].request);
		port[1].count = 1;
	} else {
		desc[1] = 0;
	}
	GEM_BUG_ON(desc[0] == desc[1]);

	/* You must always write both descriptors in the order below. */
	writel(upper_32_bits(desc[1]), elsp);
	writel(lower_32_bits(desc[1]), elsp);

	writel(upper_32_bits(desc[0]), elsp);
	/* The context is automatically loaded after the following */
	writel(lower_32_bits(desc[0]), elsp);
}

static bool ctx_single_port_submission(const struct i915_gem_context *ctx)
{
	return (IS_ENABLED(CONFIG_DRM_I915_GVT) &&
		ctx->execlists_force_single_submission);
}

static bool can_merge_ctx(const struct i915_gem_context *prev,
			  const struct i915_gem_context *next)
{
	if (prev != next)
		return false;

	if (ctx_single_port_submission(prev))
		return false;

	return true;
}

static void execlists_dequeue(struct intel_engine_cs *engine)
{
	struct drm_i915_gem_request *cursor, *last;
	struct execlist_port *port = engine->execlist_port;
	bool submit = false;

	last = port->request;
	if (last)
		/* WaIdleLiteRestore:bdw,skl
		 * Apply the wa NOOPs to prevent ring:HEAD == req:TAIL
		 * as we resubmit the request. See gen8_emit_request()
		 * for where we prepare the padding after the end of the
		 * request.
		 */
		last->tail = last->wa_tail;

	GEM_BUG_ON(port[1].request);

	/* Hardware submission is through 2 ports. Conceptually each port
	 * has a (RING_START, RING_HEAD, RING_TAIL) tuple. RING_START is
	 * static for a context, and unique to each, so we only execute
	 * requests belonging to a single context from each ring. RING_HEAD
	 * is maintained by the CS in the context image, it marks the place
	 * where it got up to last time, and through RING_TAIL we tell the CS
	 * where we want to execute up to this time.
	 *
	 * In this list the requests are in order of execution. Consecutive
	 * requests from the same context are adjacent in the ringbuffer. We
	 * can combine these requests into a single RING_TAIL update:
	 *
	 *              RING_HEAD...req1...req2
	 *                                    ^- RING_TAIL
	 * since to execute req2 the CS must first execute req1.
	 *
	 * Our goal then is to point each port to the end of a consecutive
	 * sequence of requests as being the most optimal (fewest wake ups
	 * and context switches) submission.
	 */

	spin_lock(&engine->execlist_lock);
	list_for_each_entry(cursor, &engine->execlist_queue, execlist_link) {
		/* Can we combine this request with the current port? It has to
		 * be the same context/ringbuffer and not have any exceptions
		 * (e.g. GVT saying never to combine contexts).
		 *
		 * If we can combine the requests, we can execute both by
		 * updating the RING_TAIL to point to the end of the second
		 * request, and so we never need to tell the hardware about
		 * the first.
		 */
		if (last && !can_merge_ctx(cursor->ctx, last->ctx)) {
			/* If we are on the second port and cannot combine
			 * this request with the last, then we are done.
			 */
			if (port != engine->execlist_port)
				break;

			/* If GVT overrides us we only ever submit port[0],
			 * leaving port[1] empty. Note that we also have
			 * to be careful that we don't queue the same
			 * context (even though a different request) to
			 * the second port.
			 */
			if (ctx_single_port_submission(cursor->ctx))
				break;

			GEM_BUG_ON(last->ctx == cursor->ctx);

			i915_gem_request_assign(&port->request, last);
			port++;
		}
		last = cursor;
		submit = true;
	}
	if (submit) {
		/* Decouple all the requests submitted from the queue */
		engine->execlist_queue.next = &cursor->execlist_link;
		cursor->execlist_link.prev = &engine->execlist_queue;

		i915_gem_request_assign(&port->request, last);
	}
	spin_unlock(&engine->execlist_lock);

	if (submit)
		execlists_submit_ports(engine);
}

static bool execlists_elsp_idle(struct intel_engine_cs *engine)
{
	return !engine->execlist_port[0].request;
}

static bool execlists_elsp_ready(struct intel_engine_cs *engine)
{
	int port;

	port = 1; /* wait for a free slot */
	if (engine->disable_lite_restore_wa || engine->preempt_wa)
		port = 0; /* wait for GPU to be idle before continuing */

	return !engine->execlist_port[port].request;
}

/*
 * Check the unread Context Status Buffers and manage the submission of new
 * contexts to the ELSP accordingly.
 */
static void intel_lrc_irq_handler(unsigned long data)
{
	struct intel_engine_cs *engine = (struct intel_engine_cs *)data;
	struct execlist_port *port = engine->execlist_port;
	struct drm_i915_private *dev_priv = engine->i915;

	intel_uncore_forcewake_get(dev_priv, engine->fw_domains);

	if (!execlists_elsp_idle(engine)) {
		u32 __iomem *csb_mmio =
			dev_priv->regs + i915_mmio_reg_offset(RING_CONTEXT_STATUS_PTR(engine));
		u32 __iomem *buf =
			dev_priv->regs + i915_mmio_reg_offset(RING_CONTEXT_STATUS_BUF_LO(engine, 0));
		unsigned int csb, head, tail;

		csb = readl(csb_mmio);
		head = GEN8_CSB_READ_PTR(csb);
		tail = GEN8_CSB_WRITE_PTR(csb);
		if (tail < head)
			tail += GEN8_CSB_ENTRIES;
		while (head < tail) {
			unsigned int idx = ++head % GEN8_CSB_ENTRIES;
			unsigned int status = readl(buf + 2 * idx);

			if (!(status & GEN8_CTX_STATUS_COMPLETED_MASK))
				continue;

			GEM_BUG_ON(port[0].count == 0);
			if (--port[0].count == 0) {
				GEM_BUG_ON(status & GEN8_CTX_STATUS_PREEMPTED);
				execlists_context_status_change(port[0].request,
								INTEL_CONTEXT_SCHEDULE_OUT);

				i915_gem_request_put(port[0].request);
				port[0] = port[1];
				memset(&port[1], 0, sizeof(port[1]));

				engine->preempt_wa = false;
			}

			GEM_BUG_ON(port[0].count == 0 &&
				   !(status & GEN8_CTX_STATUS_ACTIVE_IDLE));
		}

		writel(_MASKED_FIELD(GEN8_CSB_READ_PTR_MASK,
				     GEN8_CSB_WRITE_PTR(csb) << 8),
		       csb_mmio);
	}

	if (execlists_elsp_ready(engine))
		execlists_dequeue(engine);

	intel_uncore_forcewake_put(dev_priv, engine->fw_domains);
}

static void execlists_submit_request(struct drm_i915_gem_request *request)
{
	struct intel_engine_cs *engine = request->engine;
	unsigned long flags;

	spin_lock_irqsave(&engine->execlist_lock, flags);

	list_add_tail(&request->execlist_link, &engine->execlist_queue);
	if (execlists_elsp_idle(engine))
		tasklet_hi_schedule(&engine->irq_tasklet);

	spin_unlock_irqrestore(&engine->execlist_lock, flags);
>>>>>>> v4.9.227
}

int intel_logical_ring_alloc_request_extras(struct drm_i915_gem_request *request)
{
<<<<<<< HEAD
	int ret;

	request->ringbuf = request->ctx->engine[request->ring->id].ringbuf;

	if (request->ctx != request->ring->default_context) {
		ret = intel_lr_context_pin(request);
=======
	struct intel_engine_cs *engine = request->engine;
	struct intel_context *ce = &request->ctx->engine[engine->id];
	int ret;

	/* Flush enough space to reduce the likelihood of waiting after
	 * we start building the request - in which case we will just
	 * have to repeat work.
	 */
	request->reserved_space += EXECLISTS_REQUEST_SIZE;

	if (!ce->state) {
		ret = execlists_context_deferred_alloc(request->ctx, engine);
>>>>>>> v4.9.227
		if (ret)
			return ret;
	}

<<<<<<< HEAD
	return 0;
}

static int logical_ring_wait_for_space(struct drm_i915_gem_request *req,
				       int bytes)
{
	struct intel_ringbuffer *ringbuf = req->ringbuf;
	struct intel_engine_cs *ring = req->ring;
	struct drm_i915_gem_request *target;
	unsigned space;
	int ret;

	if (intel_ring_space(ringbuf) >= bytes)
		return 0;

	/* The whole point of reserving space is to not wait! */
	WARN_ON(ringbuf->reserved_in_use);

	list_for_each_entry(target, &ring->request_list, list) {
		/*
		 * The request queue is per-engine, so can contain requests
		 * from multiple ringbuffers. Here, we must ignore any that
		 * aren't from the ringbuffer we're considering.
		 */
		if (target->ringbuf != ringbuf)
			continue;

		/* Would completion of this request free enough space? */
		space = __intel_ring_space(target->postfix, ringbuf->tail,
					   ringbuf->size);
		if (space >= bytes)
			break;
	}

	if (WARN_ON(&target->list == &ring->request_list))
		return -ENOSPC;

	ret = i915_wait_request(target);
	if (ret)
		return ret;

	ringbuf->space = space;
	return 0;
}

/*
 * intel_logical_ring_advance_and_submit() - advance the tail and submit the workload
=======
	request->ring = ce->ring;

	ret = intel_lr_context_pin(request->ctx, engine);
	if (ret)
		return ret;

	if (i915.enable_guc_submission) {
		/*
		 * Check that the GuC has space for the request before
		 * going any further, as the i915_add_request() call
		 * later on mustn't fail ...
		 */
		ret = i915_guc_wq_reserve(request);
		if (ret)
			goto err_unpin;
	}

	ret = intel_ring_begin(request, 0);
	if (ret)
		goto err_unreserve;

	if (!ce->initialised) {
		ret = engine->init_context(request);
		if (ret)
			goto err_unreserve;

		ce->initialised = true;
	}

	/* Note that after this point, we have committed to using
	 * this request as it is being used to both track the
	 * state of engine initialisation and liveness of the
	 * golden renderstate above. Think twice before you try
	 * to cancel/unwind this request now.
	 */

	request->reserved_space -= EXECLISTS_REQUEST_SIZE;
	return 0;

err_unreserve:
	if (i915.enable_guc_submission)
		i915_guc_wq_unreserve(request);
err_unpin:
	intel_lr_context_unpin(request->ctx, engine);
	return ret;
}

/*
 * intel_logical_ring_advance() - advance the tail and prepare for submission
>>>>>>> v4.9.227
 * @request: Request to advance the logical ringbuffer of.
 *
 * The tail is updated in our logical ringbuffer struct, not in the actual context. What
 * really happens during submission is that the context and current tail will be placed
 * on a queue waiting for the ELSP to be ready to accept a new context submission. At that
 * point, the tail *inside* the context is updated and the ELSP written to.
 */
<<<<<<< HEAD
static void
intel_logical_ring_advance_and_submit(struct drm_i915_gem_request *request)
{
	struct intel_engine_cs *ring = request->ring;
	struct drm_i915_private *dev_priv = request->i915;

	intel_logical_ring_advance(request->ringbuf);

	request->tail = request->ringbuf->tail;

	if (intel_ring_stopped(ring))
		return;

	if (dev_priv->guc.execbuf_client)
		i915_guc_submit(dev_priv->guc.execbuf_client, request);
	else
		execlists_context_queue(request);
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

static int logical_ring_prepare(struct drm_i915_gem_request *req, int bytes)
{
	struct intel_ringbuffer *ringbuf = req->ringbuf;
	int remain_usable = ringbuf->effective_size - ringbuf->tail;
	int remain_actual = ringbuf->size - ringbuf->tail;
	int ret, total_bytes, wait_bytes = 0;
	bool need_wrap = false;

	if (ringbuf->reserved_in_use)
		total_bytes = bytes;
	else
		total_bytes = bytes + ringbuf->reserved_size;

	if (unlikely(bytes > remain_usable)) {
		/*
		 * Not enough space for the basic request. So need to flush
		 * out the remainder and then wait for base + reserved.
		 */
		wait_bytes = remain_actual + total_bytes;
		need_wrap = true;
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
		ret = logical_ring_wait_for_space(req, wait_bytes);
		if (unlikely(ret))
			return ret;

		if (need_wrap)
			__wrap_ring_buffer(ringbuf);
	}

	return 0;
}

/**
 * intel_logical_ring_begin() - prepare the logical ringbuffer to accept some commands
 *
 * @req: The request to start some new work for
 * @num_dwords: number of DWORDs that we plan to write to the ringbuffer.
 *
 * The ringbuffer might not be ready to accept the commands right away (maybe it needs to
 * be wrapped, or wait a bit for the tail to be updated). This function takes care of that
 * and also preallocates a request (every workload submission is still mediated through
 * requests, same as it did with legacy ringbuffer submission).
 *
 * Return: non-zero if the ringbuffer is not ready to be written to.
 */
int intel_logical_ring_begin(struct drm_i915_gem_request *req, int num_dwords)
{
	struct drm_i915_private *dev_priv;
	int ret;

	WARN_ON(req == NULL);
	dev_priv = req->ring->dev->dev_private;

	ret = i915_gem_check_wedge(&dev_priv->gpu_error,
				   dev_priv->mm.interruptible);
	if (ret)
		return ret;

	ret = logical_ring_prepare(req, num_dwords * sizeof(uint32_t));
	if (ret)
		return ret;

	req->ringbuf->space -= num_dwords * sizeof(uint32_t);
	return 0;
}

int intel_logical_ring_reserve_space(struct drm_i915_gem_request *request)
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

	return intel_logical_ring_begin(request, 0);
}

/**
 * execlists_submission() - submit a batchbuffer for execution, Execlists style
 * @dev: DRM device.
 * @file: DRM file.
 * @ring: Engine Command Streamer to submit to.
 * @ctx: Context to employ for this submission.
 * @args: execbuffer call arguments.
 * @vmas: list of vmas.
 * @batch_obj: the batchbuffer to submit.
 * @exec_start: batchbuffer start virtual address pointer.
 * @dispatch_flags: translated execbuffer call flags.
 *
 * This is the evil twin version of i915_gem_ringbuffer_submission. It abstracts
 * away the submission details of the execbuffer ioctl call.
 *
 * Return: non-zero if the submission fails.
 */
int intel_execlists_submission(struct i915_execbuffer_params *params,
			       struct drm_i915_gem_execbuffer2 *args,
			       struct list_head *vmas)
{
	struct drm_device       *dev = params->dev;
	struct intel_engine_cs  *ring = params->ring;
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_ringbuffer *ringbuf = params->ctx->engine[ring->id].ringbuf;
	u64 exec_start;
	int instp_mode;
	u32 instp_mask;
	int ret;

	instp_mode = args->flags & I915_EXEC_CONSTANTS_MASK;
	instp_mask = I915_EXEC_CONSTANTS_MASK;
	switch (instp_mode) {
	case I915_EXEC_CONSTANTS_REL_GENERAL:
	case I915_EXEC_CONSTANTS_ABSOLUTE:
	case I915_EXEC_CONSTANTS_REL_SURFACE:
		if (instp_mode != 0 && ring != &dev_priv->ring[RCS]) {
			DRM_DEBUG("non-0 rel constants mode on non-RCS\n");
			return -EINVAL;
		}

		if (instp_mode != dev_priv->relative_constants_mode) {
			if (instp_mode == I915_EXEC_CONSTANTS_REL_SURFACE) {
				DRM_DEBUG("rel surface constants mode invalid on gen5+\n");
				return -EINVAL;
			}

			/* The HW changed the meaning on this bit on gen6 */
			instp_mask &= ~I915_EXEC_CONSTANTS_REL_SURFACE;
		}
		break;
	default:
		DRM_DEBUG("execbuf with unknown constants: %d\n", instp_mode);
		return -EINVAL;
	}

	if (args->flags & I915_EXEC_GEN7_SOL_RESET) {
		DRM_DEBUG("sol reset is gen7 only\n");
		return -EINVAL;
	}

	ret = execlists_move_to_gpu(params->request, vmas);
	if (ret)
		return ret;

	if (ring == &dev_priv->ring[RCS] &&
	    instp_mode != dev_priv->relative_constants_mode) {
		ret = intel_logical_ring_begin(params->request, 4);
		if (ret)
			return ret;

		intel_logical_ring_emit(ringbuf, MI_NOOP);
		intel_logical_ring_emit(ringbuf, MI_LOAD_REGISTER_IMM(1));
		intel_logical_ring_emit(ringbuf, INSTPM);
		intel_logical_ring_emit(ringbuf, instp_mask << 16 | instp_mode);
		intel_logical_ring_advance(ringbuf);

		dev_priv->relative_constants_mode = instp_mode;
	}

	exec_start = params->batch_obj_vm_offset +
		     args->batch_start_offset;

	ret = ring->emit_bb_start(params->request, exec_start, params->dispatch_flags);
	if (ret)
		return ret;

	trace_i915_gem_ring_dispatch(params->request, params->dispatch_flags);

	i915_gem_execbuffer_move_to_active(vmas, params->request);
	i915_gem_execbuffer_retire_commands(params);

	return 0;
}

void intel_execlists_retire_requests(struct intel_engine_cs *ring)
{
	struct drm_i915_gem_request *req, *tmp;
	struct list_head retired_list;

	WARN_ON(!mutex_is_locked(&ring->dev->struct_mutex));
	if (list_empty(&ring->execlist_retired_req_list))
		return;

	INIT_LIST_HEAD(&retired_list);
	spin_lock_irq(&ring->execlist_lock);
	list_replace_init(&ring->execlist_retired_req_list, &retired_list);
	spin_unlock_irq(&ring->execlist_lock);

	list_for_each_entry_safe(req, tmp, &retired_list, execlist_link) {
		struct intel_context *ctx = req->ctx;
		struct drm_i915_gem_object *ctx_obj =
				ctx->engine[ring->id].state;

		if (ctx_obj && (ctx != ring->default_context))
			intel_lr_context_unpin(req);
		list_del(&req->execlist_link);
		i915_gem_request_unreference(req);
	}
}

void intel_logical_ring_stop(struct intel_engine_cs *ring)
{
	struct drm_i915_private *dev_priv = ring->dev->dev_private;
	int ret;

	if (!intel_ring_initialized(ring))
		return;

	ret = intel_ring_idle(ring);
	if (ret && !i915_reset_in_progress(&to_i915(ring->dev)->gpu_error))
		DRM_ERROR("failed to quiesce %s whilst cleaning up: %d\n",
			  ring->name, ret);

	/* TODO: Is this correct with Execlists enabled? */
	I915_WRITE_MODE(ring, _MASKED_BIT_ENABLE(STOP_RING));
	if (wait_for_atomic((I915_READ_MODE(ring) & MODE_IDLE) != 0, 1000)) {
		DRM_ERROR("%s :timed out trying to stop ring\n", ring->name);
		return;
	}
	I915_WRITE_MODE(ring, _MASKED_BIT_DISABLE(STOP_RING));
}

int logical_ring_flush_all_caches(struct drm_i915_gem_request *req)
{
	struct intel_engine_cs *ring = req->ring;
	int ret;

	if (!ring->gpu_caches_dirty)
		return 0;

	ret = ring->emit_flush(req, 0, I915_GEM_GPU_DOMAINS);
	if (ret)
		return ret;

	ring->gpu_caches_dirty = false;
	return 0;
}

static int intel_lr_context_do_pin(struct intel_engine_cs *ring,
		struct drm_i915_gem_object *ctx_obj,
		struct intel_ringbuffer *ringbuf)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	int ret = 0;

	WARN_ON(!mutex_is_locked(&ring->dev->struct_mutex));
	ret = i915_gem_obj_ggtt_pin(ctx_obj, GEN8_LR_CONTEXT_ALIGN,
			PIN_OFFSET_BIAS | GUC_WOPCM_TOP);
	if (ret)
		return ret;

	ret = intel_pin_and_map_ringbuffer_obj(ring->dev, ringbuf);
	if (ret)
		goto unpin_ctx_obj;

	ctx_obj->dirty = true;

	/* Invalidate GuC TLB. */
	if (i915.enable_guc_submission)
		I915_WRITE(GEN8_GTCR, GEN8_GTCR_INVALIDATE);

	return ret;

unpin_ctx_obj:
	i915_gem_object_ggtt_unpin(ctx_obj);

	return ret;
}

static int intel_lr_context_pin(struct drm_i915_gem_request *rq)
{
	int ret = 0;
	struct intel_engine_cs *ring = rq->ring;
	struct drm_i915_gem_object *ctx_obj = rq->ctx->engine[ring->id].state;
	struct intel_ringbuffer *ringbuf = rq->ringbuf;

	if (rq->ctx->engine[ring->id].pin_count++ == 0) {
		ret = intel_lr_context_do_pin(ring, ctx_obj, ringbuf);
		if (ret)
			goto reset_pin_count;
	}
	return ret;

reset_pin_count:
	rq->ctx->engine[ring->id].pin_count = 0;
	return ret;
}

void intel_lr_context_unpin(struct drm_i915_gem_request *rq)
{
	struct intel_engine_cs *ring = rq->ring;
	struct drm_i915_gem_object *ctx_obj = rq->ctx->engine[ring->id].state;
	struct intel_ringbuffer *ringbuf = rq->ringbuf;

	if (ctx_obj) {
		WARN_ON(!mutex_is_locked(&ring->dev->struct_mutex));
		if (--rq->ctx->engine[ring->id].pin_count == 0) {
			intel_unpin_ringbuffer_obj(ringbuf);
			i915_gem_object_ggtt_unpin(ctx_obj);
		}
	}
=======
static int
intel_logical_ring_advance(struct drm_i915_gem_request *request)
{
	struct intel_ring *ring = request->ring;
	struct intel_engine_cs *engine = request->engine;

	intel_ring_advance(ring);
	request->tail = ring->tail;

	/*
	 * Here we add two extra NOOPs as padding to avoid
	 * lite restore of a context with HEAD==TAIL.
	 *
	 * Caller must reserve WA_TAIL_DWORDS for us!
	 */
	intel_ring_emit(ring, MI_NOOP);
	intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);
	request->wa_tail = ring->tail;

	/* We keep the previous context alive until we retire the following
	 * request. This ensures that any the context object is still pinned
	 * for any residual writes the HW makes into it on the context switch
	 * into the next object following the breadcrumb. Otherwise, we may
	 * retire the context too early.
	 */
	request->previous_context = engine->last_context;
	engine->last_context = request->ctx;
	return 0;
}

static int intel_lr_context_pin(struct i915_gem_context *ctx,
				struct intel_engine_cs *engine)
{
	struct intel_context *ce = &ctx->engine[engine->id];
	void *vaddr;
	int ret;

	lockdep_assert_held(&ctx->i915->drm.struct_mutex);

	if (ce->pin_count++)
		return 0;

	ret = i915_vma_pin(ce->state, 0, GEN8_LR_CONTEXT_ALIGN,
			   PIN_OFFSET_BIAS | GUC_WOPCM_TOP | PIN_GLOBAL);
	if (ret)
		goto err;

	vaddr = i915_gem_object_pin_map(ce->state->obj, I915_MAP_WB);
	if (IS_ERR(vaddr)) {
		ret = PTR_ERR(vaddr);
		goto unpin_vma;
	}

	ret = intel_ring_pin(ce->ring);
	if (ret)
		goto unpin_map;

	intel_lr_context_descriptor_update(ctx, engine);

	ce->lrc_reg_state = vaddr + LRC_STATE_PN * PAGE_SIZE;
	ce->lrc_reg_state[CTX_RING_BUFFER_START+1] =
		i915_ggtt_offset(ce->ring->vma);

	ce->state->obj->dirty = true;

	/* Invalidate GuC TLB. */
	if (i915.enable_guc_submission) {
		struct drm_i915_private *dev_priv = ctx->i915;
		I915_WRITE(GEN8_GTCR, GEN8_GTCR_INVALIDATE);
	}

	i915_gem_context_get(ctx);
	return 0;

unpin_map:
	i915_gem_object_unpin_map(ce->state->obj);
unpin_vma:
	__i915_vma_unpin(ce->state);
err:
	ce->pin_count = 0;
	return ret;
}

void intel_lr_context_unpin(struct i915_gem_context *ctx,
			    struct intel_engine_cs *engine)
{
	struct intel_context *ce = &ctx->engine[engine->id];

	lockdep_assert_held(&ctx->i915->drm.struct_mutex);
	GEM_BUG_ON(ce->pin_count == 0);

	if (--ce->pin_count)
		return;

	intel_ring_unpin(ce->ring);

	i915_gem_object_unpin_map(ce->state->obj);
	i915_vma_unpin(ce->state);

	i915_gem_context_put(ctx);
>>>>>>> v4.9.227
}

static int intel_logical_ring_workarounds_emit(struct drm_i915_gem_request *req)
{
	int ret, i;
<<<<<<< HEAD
	struct intel_engine_cs *ring = req->ring;
	struct intel_ringbuffer *ringbuf = req->ringbuf;
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct i915_workarounds *w = &dev_priv->workarounds;

	if (WARN_ON_ONCE(w->count == 0))
		return 0;

	ring->gpu_caches_dirty = true;
	ret = logical_ring_flush_all_caches(req);
	if (ret)
		return ret;

	ret = intel_logical_ring_begin(req, w->count * 2 + 2);
	if (ret)
		return ret;

	intel_logical_ring_emit(ringbuf, MI_LOAD_REGISTER_IMM(w->count));
	for (i = 0; i < w->count; i++) {
		intel_logical_ring_emit(ringbuf, w->reg[i].addr);
		intel_logical_ring_emit(ringbuf, w->reg[i].value);
	}
	intel_logical_ring_emit(ringbuf, MI_NOOP);

	intel_logical_ring_advance(ringbuf);

	ring->gpu_caches_dirty = true;
	ret = logical_ring_flush_all_caches(req);
=======
	struct intel_ring *ring = req->ring;
	struct i915_workarounds *w = &req->i915->workarounds;

	if (w->count == 0)
		return 0;

	ret = req->engine->emit_flush(req, EMIT_BARRIER);
	if (ret)
		return ret;

	ret = intel_ring_begin(req, w->count * 2 + 2);
	if (ret)
		return ret;

	intel_ring_emit(ring, MI_LOAD_REGISTER_IMM(w->count));
	for (i = 0; i < w->count; i++) {
		intel_ring_emit_reg(ring, w->reg[i].addr);
		intel_ring_emit(ring, w->reg[i].value);
	}
	intel_ring_emit(ring, MI_NOOP);

	intel_ring_advance(ring);

	ret = req->engine->emit_flush(req, EMIT_BARRIER);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	return 0;
}

#define wa_ctx_emit(batch, index, cmd)					\
	do {								\
		int __index = (index)++;				\
		if (WARN_ON(__index >= (PAGE_SIZE / sizeof(uint32_t)))) { \
			return -ENOSPC;					\
		}							\
		batch[__index] = (cmd);					\
	} while (0)

<<<<<<< HEAD
=======
#define wa_ctx_emit_reg(batch, index, reg) \
	wa_ctx_emit((batch), (index), i915_mmio_reg_offset(reg))
>>>>>>> v4.9.227

/*
 * In this WA we need to set GEN8_L3SQCREG4[21:21] and reset it after
 * PIPE_CONTROL instruction. This is required for the flush to happen correctly
 * but there is a slight complication as this is applied in WA batch where the
 * values are only initialized once so we cannot take register value at the
 * beginning and reuse it further; hence we save its value to memory, upload a
 * constant value with bit21 set and then we restore it back with the saved value.
 * To simplify the WA, a constant value is formed by using the default value
 * of this register. This shouldn't be a problem because we are only modifying
 * it for a short period and this batch in non-premptible. We can ofcourse
 * use additional instructions that read the actual value of the register
 * at that time and set our bit of interest but it makes the WA complicated.
 *
 * This WA is also required for Gen9 so extracting as a function avoids
 * code duplication.
 */
<<<<<<< HEAD
static inline int gen8_emit_flush_coherentl3_wa(struct intel_engine_cs *ring,
						uint32_t *const batch,
						uint32_t index)
{
	uint32_t l3sqc4_flush = (0x40400000 | GEN8_LQSC_FLUSH_COHERENT_LINES);

	/*
	 * WaDisableLSQCROPERFforOCL:skl
=======
static inline int gen8_emit_flush_coherentl3_wa(struct intel_engine_cs *engine,
						uint32_t *batch,
						uint32_t index)
{
	struct drm_i915_private *dev_priv = engine->i915;
	uint32_t l3sqc4_flush = (0x40400000 | GEN8_LQSC_FLUSH_COHERENT_LINES);

	/*
	 * WaDisableLSQCROPERFforOCL:skl,kbl
>>>>>>> v4.9.227
	 * This WA is implemented in skl_init_clock_gating() but since
	 * this batch updates GEN8_L3SQCREG4 with default value we need to
	 * set this bit here to retain the WA during flush.
	 */
<<<<<<< HEAD
	if (IS_SKYLAKE(ring->dev) && INTEL_REVID(ring->dev) <= SKL_REVID_E0)
=======
	if (IS_SKL_REVID(dev_priv, 0, SKL_REVID_E0))
>>>>>>> v4.9.227
		l3sqc4_flush |= GEN8_LQSC_RO_PERF_DIS;

	wa_ctx_emit(batch, index, (MI_STORE_REGISTER_MEM_GEN8 |
				   MI_SRM_LRM_GLOBAL_GTT));
<<<<<<< HEAD
	wa_ctx_emit(batch, index, GEN8_L3SQCREG4);
	wa_ctx_emit(batch, index, ring->scratch.gtt_offset + 256);
	wa_ctx_emit(batch, index, 0);

	wa_ctx_emit(batch, index, MI_LOAD_REGISTER_IMM(1));
	wa_ctx_emit(batch, index, GEN8_L3SQCREG4);
=======
	wa_ctx_emit_reg(batch, index, GEN8_L3SQCREG4);
	wa_ctx_emit(batch, index, i915_ggtt_offset(engine->scratch) + 256);
	wa_ctx_emit(batch, index, 0);

	wa_ctx_emit(batch, index, MI_LOAD_REGISTER_IMM(1));
	wa_ctx_emit_reg(batch, index, GEN8_L3SQCREG4);
>>>>>>> v4.9.227
	wa_ctx_emit(batch, index, l3sqc4_flush);

	wa_ctx_emit(batch, index, GFX_OP_PIPE_CONTROL(6));
	wa_ctx_emit(batch, index, (PIPE_CONTROL_CS_STALL |
				   PIPE_CONTROL_DC_FLUSH_ENABLE));
	wa_ctx_emit(batch, index, 0);
	wa_ctx_emit(batch, index, 0);
	wa_ctx_emit(batch, index, 0);
	wa_ctx_emit(batch, index, 0);

	wa_ctx_emit(batch, index, (MI_LOAD_REGISTER_MEM_GEN8 |
				   MI_SRM_LRM_GLOBAL_GTT));
<<<<<<< HEAD
	wa_ctx_emit(batch, index, GEN8_L3SQCREG4);
	wa_ctx_emit(batch, index, ring->scratch.gtt_offset + 256);
=======
	wa_ctx_emit_reg(batch, index, GEN8_L3SQCREG4);
	wa_ctx_emit(batch, index, i915_ggtt_offset(engine->scratch) + 256);
>>>>>>> v4.9.227
	wa_ctx_emit(batch, index, 0);

	return index;
}

static inline uint32_t wa_ctx_start(struct i915_wa_ctx_bb *wa_ctx,
				    uint32_t offset,
				    uint32_t start_alignment)
{
	return wa_ctx->offset = ALIGN(offset, start_alignment);
}

static inline int wa_ctx_end(struct i915_wa_ctx_bb *wa_ctx,
			     uint32_t offset,
			     uint32_t size_alignment)
{
	wa_ctx->size = offset - wa_ctx->offset;

	WARN(wa_ctx->size % size_alignment,
	     "wa_ctx_bb failed sanity checks: size %d is not aligned to %d\n",
	     wa_ctx->size, size_alignment);
	return 0;
}

<<<<<<< HEAD
/**
 * gen8_init_indirectctx_bb() - initialize indirect ctx batch with WA
 *
 * @ring: only applicable for RCS
 * @wa_ctx: structure representing wa_ctx
 *  offset: specifies start of the batch, should be cache-aligned. This is updated
 *    with the offset value received as input.
 *  size: size of the batch in DWORDS but HW expects in terms of cachelines
 * @batch: page in which WA are loaded
 * @offset: This field specifies the start of the batch, it should be
 *  cache-aligned otherwise it is adjusted accordingly.
 *  Typically we only have one indirect_ctx and per_ctx batch buffer which are
 *  initialized at the beginning and shared across all contexts but this field
 *  helps us to have multiple batches at different offsets and select them based
 *  on a criteria. At the moment this batch always start at the beginning of the page
 *  and at this point we don't have multiple wa_ctx batch buffers.
 *
 *  The number of WA applied are not known at the beginning; we use this field
 *  to return the no of DWORDS written.
 *
 *  It is to be noted that this batch does not contain MI_BATCH_BUFFER_END
 *  so it adds NOOPs as padding to make it cacheline aligned.
 *  MI_BATCH_BUFFER_END will be added to perctx batch and both of them together
 *  makes a complete batch buffer.
 *
 * Return: non-zero if we exceed the PAGE_SIZE limit.
 */

static int gen8_init_indirectctx_bb(struct intel_engine_cs *ring,
				    struct i915_wa_ctx_bb *wa_ctx,
				    uint32_t *const batch,
=======
/*
 * Typically we only have one indirect_ctx and per_ctx batch buffer which are
 * initialized at the beginning and shared across all contexts but this field
 * helps us to have multiple batches at different offsets and select them based
 * on a criteria. At the moment this batch always start at the beginning of the page
 * and at this point we don't have multiple wa_ctx batch buffers.
 *
 * The number of WA applied are not known at the beginning; we use this field
 * to return the no of DWORDS written.
 *
 * It is to be noted that this batch does not contain MI_BATCH_BUFFER_END
 * so it adds NOOPs as padding to make it cacheline aligned.
 * MI_BATCH_BUFFER_END will be added to perctx batch and both of them together
 * makes a complete batch buffer.
 */
static int gen8_init_indirectctx_bb(struct intel_engine_cs *engine,
				    struct i915_wa_ctx_bb *wa_ctx,
				    uint32_t *batch,
>>>>>>> v4.9.227
				    uint32_t *offset)
{
	uint32_t scratch_addr;
	uint32_t index = wa_ctx_start(wa_ctx, *offset, CACHELINE_DWORDS);

	/* WaDisableCtxRestoreArbitration:bdw,chv */
	wa_ctx_emit(batch, index, MI_ARB_ON_OFF | MI_ARB_DISABLE);

	/* WaFlushCoherentL3CacheLinesAtContextSwitch:bdw */
<<<<<<< HEAD
	if (IS_BROADWELL(ring->dev)) {
		int rc = gen8_emit_flush_coherentl3_wa(ring, batch, index);
=======
	if (IS_BROADWELL(engine->i915)) {
		int rc = gen8_emit_flush_coherentl3_wa(engine, batch, index);
>>>>>>> v4.9.227
		if (rc < 0)
			return rc;
		index = rc;
	}

	/* WaClearSlmSpaceAtContextSwitch:bdw,chv */
	/* Actual scratch location is at 128 bytes offset */
<<<<<<< HEAD
	scratch_addr = ring->scratch.gtt_offset + 2*CACHELINE_BYTES;
=======
	scratch_addr = i915_ggtt_offset(engine->scratch) + 2 * CACHELINE_BYTES;
>>>>>>> v4.9.227

	wa_ctx_emit(batch, index, GFX_OP_PIPE_CONTROL(6));
	wa_ctx_emit(batch, index, (PIPE_CONTROL_FLUSH_L3 |
				   PIPE_CONTROL_GLOBAL_GTT_IVB |
				   PIPE_CONTROL_CS_STALL |
				   PIPE_CONTROL_QW_WRITE));
	wa_ctx_emit(batch, index, scratch_addr);
	wa_ctx_emit(batch, index, 0);
	wa_ctx_emit(batch, index, 0);
	wa_ctx_emit(batch, index, 0);

	/* Pad to end of cacheline */
	while (index % CACHELINE_DWORDS)
		wa_ctx_emit(batch, index, MI_NOOP);

	/*
	 * MI_BATCH_BUFFER_END is not required in Indirect ctx BB because
	 * execution depends on the length specified in terms of cache lines
	 * in the register CTX_RCS_INDIRECT_CTX
	 */

	return wa_ctx_end(wa_ctx, *offset = index, CACHELINE_DWORDS);
}

<<<<<<< HEAD
/**
 * gen8_init_perctx_bb() - initialize per ctx batch with WA
 *
 * @ring: only applicable for RCS
 * @wa_ctx: structure representing wa_ctx
 *  offset: specifies start of the batch, should be cache-aligned.
 *  size: size of the batch in DWORDS but HW expects in terms of cachelines
 * @batch: page in which WA are loaded
 * @offset: This field specifies the start of this batch.
 *   This batch is started immediately after indirect_ctx batch. Since we ensure
 *   that indirect_ctx ends on a cacheline this batch is aligned automatically.
 *
 *   The number of DWORDS written are returned using this field.
=======
/*
 *  This batch is started immediately after indirect_ctx batch. Since we ensure
 *  that indirect_ctx ends on a cacheline this batch is aligned automatically.
 *
 *  The number of DWORDS written are returned using this field.
>>>>>>> v4.9.227
 *
 *  This batch is terminated with MI_BATCH_BUFFER_END and so we need not add padding
 *  to align it with cacheline as padding after MI_BATCH_BUFFER_END is redundant.
 */
<<<<<<< HEAD
static int gen8_init_perctx_bb(struct intel_engine_cs *ring,
			       struct i915_wa_ctx_bb *wa_ctx,
			       uint32_t *const batch,
=======
static int gen8_init_perctx_bb(struct intel_engine_cs *engine,
			       struct i915_wa_ctx_bb *wa_ctx,
			       uint32_t *batch,
>>>>>>> v4.9.227
			       uint32_t *offset)
{
	uint32_t index = wa_ctx_start(wa_ctx, *offset, CACHELINE_DWORDS);

	/* WaDisableCtxRestoreArbitration:bdw,chv */
	wa_ctx_emit(batch, index, MI_ARB_ON_OFF | MI_ARB_ENABLE);

	wa_ctx_emit(batch, index, MI_BATCH_BUFFER_END);

	return wa_ctx_end(wa_ctx, *offset = index, 1);
}

<<<<<<< HEAD
static int gen9_init_indirectctx_bb(struct intel_engine_cs *ring,
				    struct i915_wa_ctx_bb *wa_ctx,
				    uint32_t *const batch,
				    uint32_t *offset)
{
	int ret;
	struct drm_device *dev = ring->dev;
	uint32_t index = wa_ctx_start(wa_ctx, *offset, CACHELINE_DWORDS);

	/* WaDisableCtxRestoreArbitration:skl,bxt */
	if ((IS_SKYLAKE(dev) && (INTEL_REVID(dev) <= SKL_REVID_D0)) ||
	    (IS_BROXTON(dev) && (INTEL_REVID(dev) == BXT_REVID_A0)))
		wa_ctx_emit(batch, index, MI_ARB_ON_OFF | MI_ARB_DISABLE);

	/* WaFlushCoherentL3CacheLinesAtContextSwitch:skl,bxt */
	ret = gen8_emit_flush_coherentl3_wa(ring, batch, index);
=======
static int gen9_init_indirectctx_bb(struct intel_engine_cs *engine,
				    struct i915_wa_ctx_bb *wa_ctx,
				    uint32_t *batch,
				    uint32_t *offset)
{
	int ret;
	struct drm_i915_private *dev_priv = engine->i915;
	uint32_t index = wa_ctx_start(wa_ctx, *offset, CACHELINE_DWORDS);
	u32 scratch_addr =
			i915_ggtt_offset(engine->scratch) + 2 * CACHELINE_BYTES;

	/* WaDisableCtxRestoreArbitration:skl,bxt */
	if (IS_SKL_REVID(dev_priv, 0, SKL_REVID_D0) ||
	    IS_BXT_REVID(dev_priv, 0, BXT_REVID_A1))
		wa_ctx_emit(batch, index, MI_ARB_ON_OFF | MI_ARB_DISABLE);

	/* WaFlushCoherentL3CacheLinesAtContextSwitch:skl,bxt */
	ret = gen8_emit_flush_coherentl3_wa(engine, batch, index);
>>>>>>> v4.9.227
	if (ret < 0)
		return ret;
	index = ret;

<<<<<<< HEAD
=======
	/* WaDisableGatherAtSetShaderCommonSlice:skl,bxt,kbl */
	wa_ctx_emit(batch, index, MI_LOAD_REGISTER_IMM(1));
	wa_ctx_emit_reg(batch, index, COMMON_SLICE_CHICKEN2);
	wa_ctx_emit(batch, index, _MASKED_BIT_DISABLE(
			    GEN9_DISABLE_GATHER_AT_SET_SHADER_COMMON_SLICE));
	wa_ctx_emit(batch, index, MI_NOOP);

	/* WaClearSlmSpaceAtContextSwitch:skl,bxt,kbl,glk,cfl */
	/* Actual scratch location is at 128 bytes offset */
	wa_ctx_emit(batch, index, GFX_OP_PIPE_CONTROL(6));
	wa_ctx_emit(batch, index, (PIPE_CONTROL_FLUSH_L3 |
				   PIPE_CONTROL_GLOBAL_GTT_IVB |
				   PIPE_CONTROL_CS_STALL |
				   PIPE_CONTROL_QW_WRITE));
	wa_ctx_emit(batch, index, scratch_addr);
	wa_ctx_emit(batch, index, 0);
	wa_ctx_emit(batch, index, 0);
	wa_ctx_emit(batch, index, 0);

	/* WaMediaPoolStateCmdInWABB:bxt */
	if (HAS_POOLED_EU(engine->i915)) {
		/*
		 * EU pool configuration is setup along with golden context
		 * during context initialization. This value depends on
		 * device type (2x6 or 3x6) and needs to be updated based
		 * on which subslice is disabled especially for 2x6
		 * devices, however it is safe to load default
		 * configuration of 3x6 device instead of masking off
		 * corresponding bits because HW ignores bits of a disabled
		 * subslice and drops down to appropriate config. Please
		 * see render_state_setup() in i915_gem_render_state.c for
		 * possible configurations, to avoid duplication they are
		 * not shown here again.
		 */
		u32 eu_pool_config = 0x00777000;
		wa_ctx_emit(batch, index, GEN9_MEDIA_POOL_STATE);
		wa_ctx_emit(batch, index, GEN9_MEDIA_POOL_ENABLE);
		wa_ctx_emit(batch, index, eu_pool_config);
		wa_ctx_emit(batch, index, 0);
		wa_ctx_emit(batch, index, 0);
		wa_ctx_emit(batch, index, 0);
	}

>>>>>>> v4.9.227
	/* Pad to end of cacheline */
	while (index % CACHELINE_DWORDS)
		wa_ctx_emit(batch, index, MI_NOOP);

	return wa_ctx_end(wa_ctx, *offset = index, CACHELINE_DWORDS);
}

<<<<<<< HEAD
static int gen9_init_perctx_bb(struct intel_engine_cs *ring,
			       struct i915_wa_ctx_bb *wa_ctx,
			       uint32_t *const batch,
			       uint32_t *offset)
{
	struct drm_device *dev = ring->dev;
	uint32_t index = wa_ctx_start(wa_ctx, *offset, CACHELINE_DWORDS);

	/* WaSetDisablePixMaskCammingAndRhwoInCommonSliceChicken:skl,bxt */
	if ((IS_SKYLAKE(dev) && (INTEL_REVID(dev) <= SKL_REVID_B0)) ||
	    (IS_BROXTON(dev) && (INTEL_REVID(dev) == BXT_REVID_A0))) {
		wa_ctx_emit(batch, index, MI_LOAD_REGISTER_IMM(1));
		wa_ctx_emit(batch, index, GEN9_SLICE_COMMON_ECO_CHICKEN0);
=======
static int gen9_init_perctx_bb(struct intel_engine_cs *engine,
			       struct i915_wa_ctx_bb *wa_ctx,
			       uint32_t *batch,
			       uint32_t *offset)
{
	uint32_t index = wa_ctx_start(wa_ctx, *offset, CACHELINE_DWORDS);

	/* WaSetDisablePixMaskCammingAndRhwoInCommonSliceChicken:skl,bxt */
	if (IS_SKL_REVID(engine->i915, 0, SKL_REVID_B0) ||
	    IS_BXT_REVID(engine->i915, 0, BXT_REVID_A1)) {
		wa_ctx_emit(batch, index, MI_LOAD_REGISTER_IMM(1));
		wa_ctx_emit_reg(batch, index, GEN9_SLICE_COMMON_ECO_CHICKEN0);
>>>>>>> v4.9.227
		wa_ctx_emit(batch, index,
			    _MASKED_BIT_ENABLE(DISABLE_PIXEL_MASK_CAMMING));
		wa_ctx_emit(batch, index, MI_NOOP);
	}

<<<<<<< HEAD
	/* WaDisableCtxRestoreArbitration:skl,bxt */
	if ((IS_SKYLAKE(dev) && (INTEL_REVID(dev) <= SKL_REVID_D0)) ||
	    (IS_BROXTON(dev) && (INTEL_REVID(dev) == BXT_REVID_A0)))
=======
	/* WaClearTdlStateAckDirtyBits:bxt */
	if (IS_BXT_REVID(engine->i915, 0, BXT_REVID_B0)) {
		wa_ctx_emit(batch, index, MI_LOAD_REGISTER_IMM(4));

		wa_ctx_emit_reg(batch, index, GEN8_STATE_ACK);
		wa_ctx_emit(batch, index, _MASKED_BIT_DISABLE(GEN9_SUBSLICE_TDL_ACK_BITS));

		wa_ctx_emit_reg(batch, index, GEN9_STATE_ACK_SLICE1);
		wa_ctx_emit(batch, index, _MASKED_BIT_DISABLE(GEN9_SUBSLICE_TDL_ACK_BITS));

		wa_ctx_emit_reg(batch, index, GEN9_STATE_ACK_SLICE2);
		wa_ctx_emit(batch, index, _MASKED_BIT_DISABLE(GEN9_SUBSLICE_TDL_ACK_BITS));

		wa_ctx_emit_reg(batch, index, GEN7_ROW_CHICKEN2);
		/* dummy write to CS, mask bits are 0 to ensure the register is not modified */
		wa_ctx_emit(batch, index, 0x0);
		wa_ctx_emit(batch, index, MI_NOOP);
	}

	/* WaDisableCtxRestoreArbitration:skl,bxt */
	if (IS_SKL_REVID(engine->i915, 0, SKL_REVID_D0) ||
	    IS_BXT_REVID(engine->i915, 0, BXT_REVID_A1))
>>>>>>> v4.9.227
		wa_ctx_emit(batch, index, MI_ARB_ON_OFF | MI_ARB_ENABLE);

	wa_ctx_emit(batch, index, MI_BATCH_BUFFER_END);

	return wa_ctx_end(wa_ctx, *offset = index, 1);
}

<<<<<<< HEAD
static int lrc_setup_wa_ctx_obj(struct intel_engine_cs *ring, u32 size)
{
	int ret;

	ring->wa_ctx.obj = i915_gem_alloc_object(ring->dev, PAGE_ALIGN(size));
	if (!ring->wa_ctx.obj) {
		DRM_DEBUG_DRIVER("alloc LRC WA ctx backing obj failed.\n");
		return -ENOMEM;
	}

	ret = i915_gem_obj_ggtt_pin(ring->wa_ctx.obj, PAGE_SIZE, 0);
	if (ret) {
		DRM_DEBUG_DRIVER("pin LRC WA ctx backing obj failed: %d\n",
				 ret);
		drm_gem_object_unreference(&ring->wa_ctx.obj->base);
		return ret;
	}

	return 0;
}

static void lrc_destroy_wa_ctx_obj(struct intel_engine_cs *ring)
{
	if (ring->wa_ctx.obj) {
		i915_gem_object_ggtt_unpin(ring->wa_ctx.obj);
		drm_gem_object_unreference(&ring->wa_ctx.obj->base);
		ring->wa_ctx.obj = NULL;
	}
}

static int intel_init_workaround_bb(struct intel_engine_cs *ring)
{
	int ret;
	uint32_t *batch;
	uint32_t offset;
	struct page *page;
	struct i915_ctx_workarounds *wa_ctx = &ring->wa_ctx;

	WARN_ON(ring->id != RCS);

	/* update this when WA for higher Gen are added */
	if (INTEL_INFO(ring->dev)->gen > 9) {
		DRM_ERROR("WA batch buffer is not initialized for Gen%d\n",
			  INTEL_INFO(ring->dev)->gen);
=======
static int lrc_setup_wa_ctx_obj(struct intel_engine_cs *engine, u32 size)
{
	struct drm_i915_gem_object *obj;
	struct i915_vma *vma;
	int err;

	obj = i915_gem_object_create(&engine->i915->drm, PAGE_ALIGN(size));
	if (IS_ERR(obj))
		return PTR_ERR(obj);

	vma = i915_vma_create(obj, &engine->i915->ggtt.base, NULL);
	if (IS_ERR(vma)) {
		err = PTR_ERR(vma);
		goto err;
	}

	err = i915_vma_pin(vma, 0, PAGE_SIZE, PIN_GLOBAL | PIN_HIGH);
	if (err)
		goto err;

	engine->wa_ctx.vma = vma;
	return 0;

err:
	i915_gem_object_put(obj);
	return err;
}

static void lrc_destroy_wa_ctx_obj(struct intel_engine_cs *engine)
{
	i915_vma_unpin_and_release(&engine->wa_ctx.vma);
}

static int intel_init_workaround_bb(struct intel_engine_cs *engine)
{
	struct i915_ctx_workarounds *wa_ctx = &engine->wa_ctx;
	uint32_t *batch;
	uint32_t offset;
	struct page *page;
	int ret;

	WARN_ON(engine->id != RCS);

	/* update this when WA for higher Gen are added */
	if (INTEL_GEN(engine->i915) > 9) {
		DRM_ERROR("WA batch buffer is not initialized for Gen%d\n",
			  INTEL_GEN(engine->i915));
>>>>>>> v4.9.227
		return 0;
	}

	/* some WA perform writes to scratch page, ensure it is valid */
<<<<<<< HEAD
	if (ring->scratch.obj == NULL) {
		DRM_ERROR("scratch page not allocated for %s\n", ring->name);
		return -EINVAL;
	}

	ret = lrc_setup_wa_ctx_obj(ring, PAGE_SIZE);
=======
	if (!engine->scratch) {
		DRM_ERROR("scratch page not allocated for %s\n", engine->name);
		return -EINVAL;
	}

	ret = lrc_setup_wa_ctx_obj(engine, PAGE_SIZE);
>>>>>>> v4.9.227
	if (ret) {
		DRM_DEBUG_DRIVER("Failed to setup context WA page: %d\n", ret);
		return ret;
	}

<<<<<<< HEAD
	page = i915_gem_object_get_page(wa_ctx->obj, 0);
	batch = kmap_atomic(page);
	offset = 0;

	if (INTEL_INFO(ring->dev)->gen == 8) {
		ret = gen8_init_indirectctx_bb(ring,
=======
	page = i915_gem_object_get_dirty_page(wa_ctx->vma->obj, 0);
	batch = kmap_atomic(page);
	offset = 0;

	if (IS_GEN8(engine->i915)) {
		ret = gen8_init_indirectctx_bb(engine,
>>>>>>> v4.9.227
					       &wa_ctx->indirect_ctx,
					       batch,
					       &offset);
		if (ret)
			goto out;

<<<<<<< HEAD
		ret = gen8_init_perctx_bb(ring,
=======
		ret = gen8_init_perctx_bb(engine,
>>>>>>> v4.9.227
					  &wa_ctx->per_ctx,
					  batch,
					  &offset);
		if (ret)
			goto out;
<<<<<<< HEAD
	} else if (INTEL_INFO(ring->dev)->gen == 9) {
		ret = gen9_init_indirectctx_bb(ring,
=======
	} else if (IS_GEN9(engine->i915)) {
		ret = gen9_init_indirectctx_bb(engine,
>>>>>>> v4.9.227
					       &wa_ctx->indirect_ctx,
					       batch,
					       &offset);
		if (ret)
			goto out;

<<<<<<< HEAD
		ret = gen9_init_perctx_bb(ring,
=======
		ret = gen9_init_perctx_bb(engine,
>>>>>>> v4.9.227
					  &wa_ctx->per_ctx,
					  batch,
					  &offset);
		if (ret)
			goto out;
	}

out:
	kunmap_atomic(batch);
	if (ret)
<<<<<<< HEAD
		lrc_destroy_wa_ctx_obj(ring);
=======
		lrc_destroy_wa_ctx_obj(engine);
>>>>>>> v4.9.227

	return ret;
}

<<<<<<< HEAD
static int gen8_init_common_ring(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	u8 next_context_status_buffer_hw;

	lrc_setup_hardware_status_page(ring,
				ring->default_context->engine[ring->id].state);

	I915_WRITE_IMR(ring, ~(ring->irq_enable_mask | ring->irq_keep_mask));
	I915_WRITE(RING_HWSTAM(ring->mmio_base), 0xffffffff);

	if (ring->status_page.obj) {
		I915_WRITE(RING_HWS_PGA(ring->mmio_base),
			   (u32)ring->status_page.gfx_addr);
		POSTING_READ(RING_HWS_PGA(ring->mmio_base));
	}

	I915_WRITE(RING_MODE_GEN7(ring),
		   _MASKED_BIT_DISABLE(GFX_REPLAY_MODE) |
		   _MASKED_BIT_ENABLE(GFX_RUN_LIST_ENABLE));
	POSTING_READ(RING_MODE_GEN7(ring));

	/*
	 * Instead of resetting the Context Status Buffer (CSB) read pointer to
	 * zero, we need to read the write pointer from hardware and use its
	 * value because "this register is power context save restored".
	 * Effectively, these states have been observed:
	 *
	 *      | Suspend-to-idle (freeze) | Suspend-to-RAM (mem) |
	 * BDW  | CSB regs not reset       | CSB regs reset       |
	 * CHT  | CSB regs not reset       | CSB regs not reset   |
	 */
	next_context_status_buffer_hw = (I915_READ(RING_CONTEXT_STATUS_PTR(ring))
						   & GEN8_CSB_PTR_MASK);

	/*
	 * When the CSB registers are reset (also after power-up / gpu reset),
	 * CSB write pointer is set to all 1's, which is not valid, use '5' in
	 * this special case, so the first element read is CSB[0].
	 */
	if (next_context_status_buffer_hw == GEN8_CSB_PTR_MASK)
		next_context_status_buffer_hw = (GEN8_CSB_ENTRIES - 1);

	ring->next_context_status_buffer = next_context_status_buffer_hw;
	DRM_DEBUG_DRIVER("Execlists enabled for %s\n", ring->name);

	memset(&ring->hangcheck, 0, sizeof(ring->hangcheck));
=======
static void lrc_init_hws(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	I915_WRITE(RING_HWS_PGA(engine->mmio_base),
		   engine->status_page.ggtt_offset);
	POSTING_READ(RING_HWS_PGA(engine->mmio_base));
}

static int gen8_init_common_ring(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	int ret;

	ret = intel_mocs_init_engine(engine);
	if (ret)
		return ret;

	lrc_init_hws(engine);

	intel_engine_reset_breadcrumbs(engine);

	I915_WRITE(RING_HWSTAM(engine->mmio_base), 0xffffffff);

	I915_WRITE(RING_MODE_GEN7(engine),
		   _MASKED_BIT_DISABLE(GFX_REPLAY_MODE) |
		   _MASKED_BIT_ENABLE(GFX_RUN_LIST_ENABLE));

	DRM_DEBUG_DRIVER("Execlists enabled for %s\n", engine->name);

	intel_engine_init_hangcheck(engine);

	if (!execlists_elsp_idle(engine))
		execlists_submit_ports(engine);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int gen8_init_render_ring(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	int ret;

	ret = gen8_init_common_ring(ring);
=======
static int gen8_init_render_ring(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	int ret;

	ret = gen8_init_common_ring(engine);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* We need to disable the AsyncFlip performance optimisations in order
	 * to use MI_WAIT_FOR_EVENT within the CS. It should already be
	 * programmed to '1' on all products.
	 *
	 * WaDisableAsyncFlipPerfMode:snb,ivb,hsw,vlv,bdw,chv
	 */
	I915_WRITE(MI_MODE, _MASKED_BIT_ENABLE(ASYNC_FLIP_PERF_DISABLE));

	I915_WRITE(INSTPM, _MASKED_BIT_ENABLE(INSTPM_FORCE_ORDERING));

<<<<<<< HEAD
	return init_workarounds_ring(ring);
}

static int gen9_init_render_ring(struct intel_engine_cs *ring)
{
	int ret;

	ret = gen8_init_common_ring(ring);
	if (ret)
		return ret;

	return init_workarounds_ring(ring);
=======
	return init_workarounds_ring(engine);
}

static int gen9_init_render_ring(struct intel_engine_cs *engine)
{
	int ret;

	ret = gen8_init_common_ring(engine);
	if (ret)
		return ret;

	return init_workarounds_ring(engine);
}

static void reset_common_ring(struct intel_engine_cs *engine,
			      struct drm_i915_gem_request *request)
{
	struct drm_i915_private *dev_priv = engine->i915;
	struct execlist_port *port = engine->execlist_port;
	struct intel_context *ce = &request->ctx->engine[engine->id];

	/* We want a simple context + ring to execute the breadcrumb update.
	 * We cannot rely on the context being intact across the GPU hang,
	 * so clear it and rebuild just what we need for the breadcrumb.
	 * All pending requests for this context will be zapped, and any
	 * future request will be after userspace has had the opportunity
	 * to recreate its own state.
	 */
	execlists_init_reg_state(ce->lrc_reg_state,
				 request->ctx, engine, ce->ring);

	/* Move the RING_HEAD onto the breadcrumb, past the hanging batch */
	ce->lrc_reg_state[CTX_RING_BUFFER_START+1] =
		i915_ggtt_offset(ce->ring->vma);
	ce->lrc_reg_state[CTX_RING_HEAD+1] = request->postfix;

	request->ring->head = request->postfix;
	request->ring->last_retired_head = -1;
	intel_ring_update_space(request->ring);

	if (i915.enable_guc_submission)
		return;

	/* Catch up with any missed context-switch interrupts */
	I915_WRITE(RING_CONTEXT_STATUS_PTR(engine), _MASKED_FIELD(0xffff, 0));
	if (request->ctx != port[0].request->ctx) {
		i915_gem_request_put(port[0].request);
		port[0] = port[1];
		memset(&port[1], 0, sizeof(port[1]));
	}

	/* CS is stopped, and we will resubmit both ports on resume */
	GEM_BUG_ON(request->ctx != port[0].request->ctx);
	port[0].count = 0;
	port[1].count = 0;

	/* Reset WaIdleLiteRestore:bdw,skl as well */
	request->tail = request->wa_tail - WA_TAIL_DWORDS * sizeof(u32);
>>>>>>> v4.9.227
}

static int intel_logical_ring_emit_pdps(struct drm_i915_gem_request *req)
{
	struct i915_hw_ppgtt *ppgtt = req->ctx->ppgtt;
<<<<<<< HEAD
	struct intel_engine_cs *ring = req->ring;
	struct intel_ringbuffer *ringbuf = req->ringbuf;
	const int num_lri_cmds = GEN8_LEGACY_PDPES * 2;
	int i, ret;

	ret = intel_logical_ring_begin(req, num_lri_cmds * 2 + 2);
	if (ret)
		return ret;

	intel_logical_ring_emit(ringbuf, MI_LOAD_REGISTER_IMM(num_lri_cmds));
	for (i = GEN8_LEGACY_PDPES - 1; i >= 0; i--) {
		const dma_addr_t pd_daddr = i915_page_dir_dma_addr(ppgtt, i);

		intel_logical_ring_emit(ringbuf, GEN8_RING_PDP_UDW(ring, i));
		intel_logical_ring_emit(ringbuf, upper_32_bits(pd_daddr));
		intel_logical_ring_emit(ringbuf, GEN8_RING_PDP_LDW(ring, i));
		intel_logical_ring_emit(ringbuf, lower_32_bits(pd_daddr));
	}

	intel_logical_ring_emit(ringbuf, MI_NOOP);
	intel_logical_ring_advance(ringbuf);
=======
	struct intel_ring *ring = req->ring;
	struct intel_engine_cs *engine = req->engine;
	const int num_lri_cmds = GEN8_LEGACY_PDPES * 2;
	int i, ret;

	ret = intel_ring_begin(req, num_lri_cmds * 2 + 2);
	if (ret)
		return ret;

	intel_ring_emit(ring, MI_LOAD_REGISTER_IMM(num_lri_cmds));
	for (i = GEN8_LEGACY_PDPES - 1; i >= 0; i--) {
		const dma_addr_t pd_daddr = i915_page_dir_dma_addr(ppgtt, i);

		intel_ring_emit_reg(ring, GEN8_RING_PDP_UDW(engine, i));
		intel_ring_emit(ring, upper_32_bits(pd_daddr));
		intel_ring_emit_reg(ring, GEN8_RING_PDP_LDW(engine, i));
		intel_ring_emit(ring, lower_32_bits(pd_daddr));
	}

	intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);
>>>>>>> v4.9.227

	return 0;
}

static int gen8_emit_bb_start(struct drm_i915_gem_request *req,
<<<<<<< HEAD
			      u64 offset, unsigned dispatch_flags)
{
	struct intel_ringbuffer *ringbuf = req->ringbuf;
=======
			      u64 offset, u32 len,
			      unsigned int dispatch_flags)
{
	struct intel_ring *ring = req->ring;
>>>>>>> v4.9.227
	bool ppgtt = !(dispatch_flags & I915_DISPATCH_SECURE);
	int ret;

	/* Don't rely in hw updating PDPs, specially in lite-restore.
	 * Ideally, we should set Force PD Restore in ctx descriptor,
	 * but we can't. Force Restore would be a second option, but
	 * it is unsafe in case of lite-restore (because the ctx is
	 * not idle). PML4 is allocated during ppgtt init so this is
	 * not needed in 48-bit.*/
	if (req->ctx->ppgtt &&
<<<<<<< HEAD
	    (intel_ring_flag(req->ring) & req->ctx->ppgtt->pd_dirty_rings)) {
		if (!USES_FULL_48BIT_PPGTT(req->i915) &&
		    !intel_vgpu_active(req->i915->dev)) {
=======
	    (intel_engine_flag(req->engine) & req->ctx->ppgtt->pd_dirty_rings)) {
		if (!USES_FULL_48BIT_PPGTT(req->i915) &&
		    !intel_vgpu_active(req->i915)) {
>>>>>>> v4.9.227
			ret = intel_logical_ring_emit_pdps(req);
			if (ret)
				return ret;
		}

<<<<<<< HEAD
		req->ctx->ppgtt->pd_dirty_rings &= ~intel_ring_flag(req->ring);
	}

	ret = intel_logical_ring_begin(req, 4);
=======
		req->ctx->ppgtt->pd_dirty_rings &= ~intel_engine_flag(req->engine);
	}

	ret = intel_ring_begin(req, 4);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* FIXME(BDW): Address space and security selectors. */
<<<<<<< HEAD
	intel_logical_ring_emit(ringbuf, MI_BATCH_BUFFER_START_GEN8 |
				(ppgtt<<8) |
				(dispatch_flags & I915_DISPATCH_RS ?
				 MI_BATCH_RESOURCE_STREAMER : 0));
	intel_logical_ring_emit(ringbuf, lower_32_bits(offset));
	intel_logical_ring_emit(ringbuf, upper_32_bits(offset));
	intel_logical_ring_emit(ringbuf, MI_NOOP);
	intel_logical_ring_advance(ringbuf);
=======
	intel_ring_emit(ring, MI_BATCH_BUFFER_START_GEN8 |
			(ppgtt<<8) |
			(dispatch_flags & I915_DISPATCH_RS ?
			 MI_BATCH_RESOURCE_STREAMER : 0));
	intel_ring_emit(ring, lower_32_bits(offset));
	intel_ring_emit(ring, upper_32_bits(offset));
	intel_ring_emit(ring, MI_NOOP);
	intel_ring_advance(ring);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static bool gen8_logical_ring_get_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	if (WARN_ON(!intel_irqs_enabled(dev_priv)))
		return false;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (ring->irq_refcount++ == 0) {
		I915_WRITE_IMR(ring, ~(ring->irq_enable_mask | ring->irq_keep_mask));
		POSTING_READ(RING_IMR(ring->mmio_base));
	}
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);

	return true;
}

static void gen8_logical_ring_put_irq(struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long flags;

	spin_lock_irqsave(&dev_priv->irq_lock, flags);
	if (--ring->irq_refcount == 0) {
		I915_WRITE_IMR(ring, ~ring->irq_keep_mask);
		POSTING_READ(RING_IMR(ring->mmio_base));
	}
	spin_unlock_irqrestore(&dev_priv->irq_lock, flags);
}

static int gen8_emit_flush(struct drm_i915_gem_request *request,
			   u32 invalidate_domains,
			   u32 unused)
{
	struct intel_ringbuffer *ringbuf = request->ringbuf;
	struct intel_engine_cs *ring = ringbuf->ring;
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	uint32_t cmd;
	int ret;

	ret = intel_logical_ring_begin(request, 4);
=======
static void gen8_logical_ring_enable_irq(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	I915_WRITE_IMR(engine,
		       ~(engine->irq_enable_mask | engine->irq_keep_mask));
	POSTING_READ_FW(RING_IMR(engine->mmio_base));
}

static void gen8_logical_ring_disable_irq(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	I915_WRITE_IMR(engine, ~engine->irq_keep_mask);
}

static int gen8_emit_flush(struct drm_i915_gem_request *request, u32 mode)
{
	struct intel_ring *ring = request->ring;
	u32 cmd;
	int ret;

	ret = intel_ring_begin(request, 4);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	cmd = MI_FLUSH_DW + 1;

	/* We always require a command barrier so that subsequent
	 * commands, such as breadcrumb interrupts, are strictly ordered
	 * wrt the contents of the write cache being flushed to memory
	 * (and thus being coherent from the CPU).
	 */
	cmd |= MI_FLUSH_DW_STORE_INDEX | MI_FLUSH_DW_OP_STOREDW;

<<<<<<< HEAD
	if (invalidate_domains & I915_GEM_GPU_DOMAINS) {
		cmd |= MI_INVALIDATE_TLB;
		if (ring == &dev_priv->ring[VCS])
			cmd |= MI_INVALIDATE_BSD;
	}

	intel_logical_ring_emit(ringbuf, cmd);
	intel_logical_ring_emit(ringbuf,
				I915_GEM_HWS_SCRATCH_ADDR |
				MI_FLUSH_DW_USE_GTT);
	intel_logical_ring_emit(ringbuf, 0); /* upper addr */
	intel_logical_ring_emit(ringbuf, 0); /* value */
	intel_logical_ring_advance(ringbuf);
=======
	if (mode & EMIT_INVALIDATE) {
		cmd |= MI_INVALIDATE_TLB;
		if (request->engine->id == VCS)
			cmd |= MI_INVALIDATE_BSD;
	}

	intel_ring_emit(ring, cmd);
	intel_ring_emit(ring,
			I915_GEM_HWS_SCRATCH_ADDR |
			MI_FLUSH_DW_USE_GTT);
	intel_ring_emit(ring, 0); /* upper addr */
	intel_ring_emit(ring, 0); /* value */
	intel_ring_advance(ring);
>>>>>>> v4.9.227

	return 0;
}

static int gen8_emit_flush_render(struct drm_i915_gem_request *request,
<<<<<<< HEAD
				  u32 invalidate_domains,
				  u32 flush_domains)
{
	struct intel_ringbuffer *ringbuf = request->ringbuf;
	struct intel_engine_cs *ring = ringbuf->ring;
	u32 scratch_addr = ring->scratch.gtt_offset + 2 * CACHELINE_BYTES;
	bool vf_flush_wa;
	u32 flags = 0;
	int ret;

	flags |= PIPE_CONTROL_CS_STALL;

	if (flush_domains) {
=======
				  u32 mode)
{
	struct intel_ring *ring = request->ring;
	struct intel_engine_cs *engine = request->engine;
	u32 scratch_addr =
		i915_ggtt_offset(engine->scratch) + 2 * CACHELINE_BYTES;
	bool vf_flush_wa = false, dc_flush_wa = false;
	u32 flags = 0;
	int ret;
	int len;

	flags |= PIPE_CONTROL_CS_STALL;

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
<<<<<<< HEAD
	}

	/*
	 * On GEN9+ Before VF_CACHE_INVALIDATE we need to emit a NULL pipe
	 * control.
	 */
	vf_flush_wa = INTEL_INFO(ring->dev)->gen >= 9 &&
		      flags & PIPE_CONTROL_VF_CACHE_INVALIDATE;

	ret = intel_logical_ring_begin(request, vf_flush_wa ? 12 : 6);
=======

		/*
		 * On GEN9: before VF_CACHE_INVALIDATE we need to emit a NULL
		 * pipe control.
		 */
		if (IS_GEN9(request->i915))
			vf_flush_wa = true;

		/* WaForGAMHang:kbl */
		if (IS_KBL_REVID(request->i915, 0, KBL_REVID_B0))
			dc_flush_wa = true;
	}

	len = 6;

	if (vf_flush_wa)
		len += 6;

	if (dc_flush_wa)
		len += 12;

	ret = intel_ring_begin(request, len);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	if (vf_flush_wa) {
<<<<<<< HEAD
		intel_logical_ring_emit(ringbuf, GFX_OP_PIPE_CONTROL(6));
		intel_logical_ring_emit(ringbuf, 0);
		intel_logical_ring_emit(ringbuf, 0);
		intel_logical_ring_emit(ringbuf, 0);
		intel_logical_ring_emit(ringbuf, 0);
		intel_logical_ring_emit(ringbuf, 0);
	}

	intel_logical_ring_emit(ringbuf, GFX_OP_PIPE_CONTROL(6));
	intel_logical_ring_emit(ringbuf, flags);
	intel_logical_ring_emit(ringbuf, scratch_addr);
	intel_logical_ring_emit(ringbuf, 0);
	intel_logical_ring_emit(ringbuf, 0);
	intel_logical_ring_emit(ringbuf, 0);
	intel_logical_ring_advance(ringbuf);
=======
		intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(6));
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring, 0);
	}

	if (dc_flush_wa) {
		intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(6));
		intel_ring_emit(ring, PIPE_CONTROL_DC_FLUSH_ENABLE);
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring, 0);
	}

	intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(6));
	intel_ring_emit(ring, flags);
	intel_ring_emit(ring, scratch_addr);
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, 0);

	if (dc_flush_wa) {
		intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(6));
		intel_ring_emit(ring, PIPE_CONTROL_CS_STALL);
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring, 0);
		intel_ring_emit(ring, 0);
	}

	intel_ring_advance(ring);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static u32 gen8_get_seqno(struct intel_engine_cs *ring, bool lazy_coherency)
{
	return intel_read_status_page(ring, I915_GEM_HWS_INDEX);
}

static void gen8_set_seqno(struct intel_engine_cs *ring, u32 seqno)
{
	intel_write_status_page(ring, I915_GEM_HWS_INDEX, seqno);
}

static u32 bxt_a_get_seqno(struct intel_engine_cs *ring, bool lazy_coherency)
{

=======
static void bxt_a_seqno_barrier(struct intel_engine_cs *engine)
{
>>>>>>> v4.9.227
	/*
	 * On BXT A steppings there is a HW coherency issue whereby the
	 * MI_STORE_DATA_IMM storing the completed request's seqno
	 * occasionally doesn't invalidate the CPU cache. Work around this by
	 * clflushing the corresponding cacheline whenever the caller wants
	 * the coherency to be guaranteed. Note that this cacheline is known
	 * to be clean at this point, since we only write it in
	 * bxt_a_set_seqno(), where we also do a clflush after the write. So
	 * this clflush in practice becomes an invalidate operation.
	 */
<<<<<<< HEAD

	if (!lazy_coherency)
		intel_flush_status_page(ring, I915_GEM_HWS_INDEX);

	return intel_read_status_page(ring, I915_GEM_HWS_INDEX);
}

static void bxt_a_set_seqno(struct intel_engine_cs *ring, u32 seqno)
{
	intel_write_status_page(ring, I915_GEM_HWS_INDEX, seqno);

	/* See bxt_a_get_seqno() explaining the reason for the clflush. */
	intel_flush_status_page(ring, I915_GEM_HWS_INDEX);
}

static int gen8_emit_request(struct drm_i915_gem_request *request)
{
	struct intel_ringbuffer *ringbuf = request->ringbuf;
	struct intel_engine_cs *ring = ringbuf->ring;
	u32 cmd;
	int ret;

	/*
	 * Reserve space for 2 NOOPs at the end of each request to be
	 * used as a workaround for not being allowed to do lite
	 * restore with HEAD==TAIL (WaIdleLiteRestore).
	 */
	ret = intel_logical_ring_begin(request, 8);
	if (ret)
		return ret;

	cmd = MI_STORE_DWORD_IMM_GEN4;
	cmd |= MI_GLOBAL_GTT;

	intel_logical_ring_emit(ringbuf, cmd);
	intel_logical_ring_emit(ringbuf,
				(ring->status_page.gfx_addr +
				(I915_GEM_HWS_INDEX << MI_STORE_DWORD_INDEX_SHIFT)));
	intel_logical_ring_emit(ringbuf, 0);
	intel_logical_ring_emit(ringbuf, i915_gem_request_get_seqno(request));
	intel_logical_ring_emit(ringbuf, MI_USER_INTERRUPT);
	intel_logical_ring_emit(ringbuf, MI_NOOP);
	intel_logical_ring_advance_and_submit(request);

	/*
	 * Here we add two extra NOOPs as padding to avoid
	 * lite restore of a context with HEAD==TAIL.
	 */
	intel_logical_ring_emit(ringbuf, MI_NOOP);
	intel_logical_ring_emit(ringbuf, MI_NOOP);
	intel_logical_ring_advance(ringbuf);

	return 0;
}

static int intel_lr_context_render_state_init(struct drm_i915_gem_request *req)
{
	struct render_state so;
	int ret;

	ret = i915_gem_render_state_prepare(req->ring, &so);
	if (ret)
		return ret;

	if (so.rodata == NULL)
		return 0;

	ret = req->ring->emit_bb_start(req, so.ggtt_offset,
				       I915_DISPATCH_SECURE);
	if (ret)
		goto out;

	ret = req->ring->emit_bb_start(req,
				       (so.ggtt_offset + so.aux_batch_offset),
				       I915_DISPATCH_SECURE);
	if (ret)
		goto out;

	i915_vma_move_to_active(i915_gem_obj_to_ggtt(so.obj), req);

out:
	i915_gem_render_state_fini(&so);
	return ret;
=======
	intel_flush_status_page(engine, I915_GEM_HWS_INDEX);
}

/*
 * Reserve space for 2 NOOPs at the end of each request to be
 * used as a workaround for not being allowed to do lite
 * restore with HEAD==TAIL (WaIdleLiteRestore).
 */

static int gen8_emit_request(struct drm_i915_gem_request *request)
{
	struct intel_ring *ring = request->ring;
	int ret;

	ret = intel_ring_begin(request, 6 + WA_TAIL_DWORDS);
	if (ret)
		return ret;

	/* w/a: bit 5 needs to be zero for MI_FLUSH_DW address. */
	BUILD_BUG_ON(I915_GEM_HWS_INDEX_ADDR & (1 << 5));

	intel_ring_emit(ring, (MI_FLUSH_DW + 1) | MI_FLUSH_DW_OP_STOREDW);
	intel_ring_emit(ring,
			intel_hws_seqno_address(request->engine) |
			MI_FLUSH_DW_USE_GTT);
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, request->fence.seqno);
	intel_ring_emit(ring, MI_USER_INTERRUPT);
	intel_ring_emit(ring, MI_NOOP);
	return intel_logical_ring_advance(request);
}

static int gen8_emit_request_render(struct drm_i915_gem_request *request)
{
	struct intel_ring *ring = request->ring;
	int ret;

	ret = intel_ring_begin(request, 8 + WA_TAIL_DWORDS);
	if (ret)
		return ret;

	/* We're using qword write, seqno should be aligned to 8 bytes. */
	BUILD_BUG_ON(I915_GEM_HWS_INDEX & 1);

	/* w/a for post sync ops following a GPGPU operation we
	 * need a prior CS_STALL, which is emitted by the flush
	 * following the batch.
	 */
	intel_ring_emit(ring, GFX_OP_PIPE_CONTROL(6));
	intel_ring_emit(ring,
			(PIPE_CONTROL_GLOBAL_GTT_IVB |
			 PIPE_CONTROL_CS_STALL |
			 PIPE_CONTROL_QW_WRITE));
	intel_ring_emit(ring, intel_hws_seqno_address(request->engine));
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, i915_gem_request_get_seqno(request));
	/* We're thrashing one dword of HWS. */
	intel_ring_emit(ring, 0);
	intel_ring_emit(ring, MI_USER_INTERRUPT);
	intel_ring_emit(ring, MI_NOOP);
	return intel_logical_ring_advance(request);
>>>>>>> v4.9.227
}

static int gen8_init_rcs_context(struct drm_i915_gem_request *req)
{
	int ret;

	ret = intel_logical_ring_workarounds_emit(req);
	if (ret)
		return ret;

	ret = intel_rcs_context_init_mocs(req);
	/*
	 * Failing to program the MOCS is non-fatal.The system will not
	 * run at peak performance. So generate an error and carry on.
	 */
	if (ret)
		DRM_ERROR("MOCS failed to program: expect performance issues.\n");

<<<<<<< HEAD
	return intel_lr_context_render_state_init(req);
=======
	return i915_gem_render_state_init(req);
>>>>>>> v4.9.227
}

/**
 * intel_logical_ring_cleanup() - deallocate the Engine Command Streamer
<<<<<<< HEAD
 *
 * @ring: Engine Command Streamer.
 *
 */
void intel_logical_ring_cleanup(struct intel_engine_cs *ring)
{
	struct drm_i915_private *dev_priv;

	if (!intel_ring_initialized(ring))
		return;

	dev_priv = ring->dev->dev_private;

	intel_logical_ring_stop(ring);
	WARN_ON((I915_READ_MODE(ring) & MODE_IDLE) == 0);

	if (ring->cleanup)
		ring->cleanup(ring);

	i915_cmd_parser_fini_ring(ring);
	i915_gem_batch_pool_fini(&ring->batch_pool);

	if (ring->status_page.obj) {
		kunmap(sg_page(ring->status_page.obj->pages->sgl));
		ring->status_page.obj = NULL;
	}

	lrc_destroy_wa_ctx_obj(ring);
}

static int logical_ring_init(struct drm_device *dev, struct intel_engine_cs *ring)
{
	int ret;

	/* Intentionally left blank. */
	ring->buffer = NULL;

	ring->dev = dev;
	INIT_LIST_HEAD(&ring->active_list);
	INIT_LIST_HEAD(&ring->request_list);
	i915_gem_batch_pool_init(dev, &ring->batch_pool);
	init_waitqueue_head(&ring->irq_queue);

	INIT_LIST_HEAD(&ring->execlist_queue);
	INIT_LIST_HEAD(&ring->execlist_retired_req_list);
	spin_lock_init(&ring->execlist_lock);

	ret = i915_cmd_parser_init_ring(ring);
	if (ret)
		return ret;

	ret = intel_lr_context_deferred_alloc(ring->default_context, ring);
	if (ret)
		return ret;

	/* As this is the default context, always pin it */
	ret = intel_lr_context_do_pin(
			ring,
			ring->default_context->engine[ring->id].state,
			ring->default_context->engine[ring->id].ringbuf);
	if (ret) {
		DRM_ERROR(
			"Failed to pin and map ringbuffer %s: %d\n",
			ring->name, ret);
		return ret;
	}

	return ret;
}

static int logical_render_ring_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring = &dev_priv->ring[RCS];
	int ret;

	ring->name = "render ring";
	ring->id = RCS;
	ring->mmio_base = RENDER_RING_BASE;
	ring->irq_enable_mask =
		GT_RENDER_USER_INTERRUPT << GEN8_RCS_IRQ_SHIFT;
	ring->irq_keep_mask =
		GT_CONTEXT_SWITCH_INTERRUPT << GEN8_RCS_IRQ_SHIFT;
	if (HAS_L3_DPF(dev))
		ring->irq_keep_mask |= GT_RENDER_L3_PARITY_ERROR_INTERRUPT;

	if (INTEL_INFO(dev)->gen >= 9)
		ring->init_hw = gen9_init_render_ring;
	else
		ring->init_hw = gen8_init_render_ring;
	ring->init_context = gen8_init_rcs_context;
	ring->cleanup = intel_fini_pipe_control;
	if (IS_BROXTON(dev) && INTEL_REVID(dev) < BXT_REVID_B0) {
		ring->get_seqno = bxt_a_get_seqno;
		ring->set_seqno = bxt_a_set_seqno;
	} else {
		ring->get_seqno = gen8_get_seqno;
		ring->set_seqno = gen8_set_seqno;
	}
	ring->emit_request = gen8_emit_request;
	ring->emit_flush = gen8_emit_flush_render;
	ring->irq_get = gen8_logical_ring_get_irq;
	ring->irq_put = gen8_logical_ring_put_irq;
	ring->emit_bb_start = gen8_emit_bb_start;

	ring->dev = dev;

	ret = intel_init_pipe_control(ring);
	if (ret)
		return ret;

	ret = intel_init_workaround_bb(ring);
=======
 * @engine: Engine Command Streamer.
 */
void intel_logical_ring_cleanup(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv;

	if (!intel_engine_initialized(engine))
		return;

	/*
	 * Tasklet cannot be active at this point due intel_mark_active/idle
	 * so this is just for documentation.
	 */
	if (WARN_ON(test_bit(TASKLET_STATE_SCHED, &engine->irq_tasklet.state)))
		tasklet_kill(&engine->irq_tasklet);

	dev_priv = engine->i915;

	if (engine->buffer) {
		WARN_ON((I915_READ_MODE(engine) & MODE_IDLE) == 0);
	}

	if (engine->cleanup)
		engine->cleanup(engine);

	intel_engine_cleanup_common(engine);

	if (engine->status_page.vma) {
		i915_gem_object_unpin_map(engine->status_page.vma->obj);
		engine->status_page.vma = NULL;
	}
	intel_lr_context_unpin(dev_priv->kernel_context, engine);

	lrc_destroy_wa_ctx_obj(engine);
	engine->i915 = NULL;
}

void intel_execlists_enable_submission(struct drm_i915_private *dev_priv)
{
	struct intel_engine_cs *engine;

	for_each_engine(engine, dev_priv)
		engine->submit_request = execlists_submit_request;
}

static void
logical_ring_default_vfuncs(struct intel_engine_cs *engine)
{
	/* Default vfuncs which can be overriden by each engine. */
	engine->init_hw = gen8_init_common_ring;
	engine->reset_hw = reset_common_ring;
	engine->emit_flush = gen8_emit_flush;
	engine->emit_request = gen8_emit_request;
	engine->submit_request = execlists_submit_request;

	engine->irq_enable = gen8_logical_ring_enable_irq;
	engine->irq_disable = gen8_logical_ring_disable_irq;
	engine->emit_bb_start = gen8_emit_bb_start;
	if (IS_BXT_REVID(engine->i915, 0, BXT_REVID_A1))
		engine->irq_seqno_barrier = bxt_a_seqno_barrier;
}

static inline void
logical_ring_default_irqs(struct intel_engine_cs *engine)
{
	unsigned shift = engine->irq_shift;
	engine->irq_enable_mask = GT_RENDER_USER_INTERRUPT << shift;
	engine->irq_keep_mask = GT_CONTEXT_SWITCH_INTERRUPT << shift;
}

static int
lrc_setup_hws(struct intel_engine_cs *engine, struct i915_vma *vma)
{
	const int hws_offset = LRC_PPHWSP_PN * PAGE_SIZE;
	void *hws;

	/* The HWSP is part of the default context object in LRC mode. */
	hws = i915_gem_object_pin_map(vma->obj, I915_MAP_WB);
	if (IS_ERR(hws))
		return PTR_ERR(hws);

	engine->status_page.page_addr = hws + hws_offset;
	engine->status_page.ggtt_offset = i915_ggtt_offset(vma) + hws_offset;
	engine->status_page.vma = vma;

	return 0;
}

static void
logical_ring_setup(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	enum forcewake_domains fw_domains;

	intel_engine_setup_common(engine);

	/* Intentionally left blank. */
	engine->buffer = NULL;

	fw_domains = intel_uncore_forcewake_for_reg(dev_priv,
						    RING_ELSP(engine),
						    FW_REG_WRITE);

	fw_domains |= intel_uncore_forcewake_for_reg(dev_priv,
						     RING_CONTEXT_STATUS_PTR(engine),
						     FW_REG_READ | FW_REG_WRITE);

	fw_domains |= intel_uncore_forcewake_for_reg(dev_priv,
						     RING_CONTEXT_STATUS_BUF_BASE(engine),
						     FW_REG_READ);

	engine->fw_domains = fw_domains;

	tasklet_init(&engine->irq_tasklet,
		     intel_lrc_irq_handler, (unsigned long)engine);

	logical_ring_init_platform_invariants(engine);
	logical_ring_default_vfuncs(engine);
	logical_ring_default_irqs(engine);
}

static int
logical_ring_init(struct intel_engine_cs *engine)
{
	struct i915_gem_context *dctx = engine->i915->kernel_context;
	int ret;

	ret = intel_engine_init_common(engine);
	if (ret)
		goto error;

	ret = execlists_context_deferred_alloc(dctx, engine);
	if (ret)
		goto error;

	/* As this is the default context, always pin it */
	ret = intel_lr_context_pin(dctx, engine);
	if (ret) {
		DRM_ERROR("Failed to pin context for %s: %d\n",
			  engine->name, ret);
		goto error;
	}

	/* And setup the hardware status page. */
	ret = lrc_setup_hws(engine, dctx->engine[engine->id].state);
	if (ret) {
		DRM_ERROR("Failed to set up hws %s: %d\n", engine->name, ret);
		goto error;
	}

	return 0;

error:
	intel_logical_ring_cleanup(engine);
	return ret;
}

int logical_render_ring_init(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	int ret;

	logical_ring_setup(engine);

	if (HAS_L3_DPF(dev_priv))
		engine->irq_keep_mask |= GT_RENDER_L3_PARITY_ERROR_INTERRUPT;

	/* Override some for render ring. */
	if (INTEL_GEN(dev_priv) >= 9)
		engine->init_hw = gen9_init_render_ring;
	else
		engine->init_hw = gen8_init_render_ring;
	engine->init_context = gen8_init_rcs_context;
	engine->emit_flush = gen8_emit_flush_render;
	engine->emit_request = gen8_emit_request_render;

	ret = intel_engine_create_scratch(engine, 4096);
	if (ret)
		return ret;

	ret = intel_init_workaround_bb(engine);
>>>>>>> v4.9.227
	if (ret) {
		/*
		 * We continue even if we fail to initialize WA batch
		 * because we only expect rare glitches but nothing
		 * critical to prevent us from using GPU
		 */
		DRM_ERROR("WA batch buffer initialization failed: %d\n",
			  ret);
	}

<<<<<<< HEAD
	ret = logical_ring_init(dev, ring);
	if (ret) {
		lrc_destroy_wa_ctx_obj(ring);
=======
	ret = logical_ring_init(engine);
	if (ret) {
		lrc_destroy_wa_ctx_obj(engine);
>>>>>>> v4.9.227
	}

	return ret;
}

<<<<<<< HEAD
static int logical_bsd_ring_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring = &dev_priv->ring[VCS];

	ring->name = "bsd ring";
	ring->id = VCS;
	ring->mmio_base = GEN6_BSD_RING_BASE;
	ring->irq_enable_mask =
		GT_RENDER_USER_INTERRUPT << GEN8_VCS1_IRQ_SHIFT;
	ring->irq_keep_mask =
		GT_CONTEXT_SWITCH_INTERRUPT << GEN8_VCS1_IRQ_SHIFT;

	ring->init_hw = gen8_init_common_ring;
	if (IS_BROXTON(dev) && INTEL_REVID(dev) < BXT_REVID_B0) {
		ring->get_seqno = bxt_a_get_seqno;
		ring->set_seqno = bxt_a_set_seqno;
	} else {
		ring->get_seqno = gen8_get_seqno;
		ring->set_seqno = gen8_set_seqno;
	}
	ring->emit_request = gen8_emit_request;
	ring->emit_flush = gen8_emit_flush;
	ring->irq_get = gen8_logical_ring_get_irq;
	ring->irq_put = gen8_logical_ring_put_irq;
	ring->emit_bb_start = gen8_emit_bb_start;

	return logical_ring_init(dev, ring);
}

static int logical_bsd2_ring_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring = &dev_priv->ring[VCS2];

	ring->name = "bds2 ring";
	ring->id = VCS2;
	ring->mmio_base = GEN8_BSD2_RING_BASE;
	ring->irq_enable_mask =
		GT_RENDER_USER_INTERRUPT << GEN8_VCS2_IRQ_SHIFT;
	ring->irq_keep_mask =
		GT_CONTEXT_SWITCH_INTERRUPT << GEN8_VCS2_IRQ_SHIFT;

	ring->init_hw = gen8_init_common_ring;
	ring->get_seqno = gen8_get_seqno;
	ring->set_seqno = gen8_set_seqno;
	ring->emit_request = gen8_emit_request;
	ring->emit_flush = gen8_emit_flush;
	ring->irq_get = gen8_logical_ring_get_irq;
	ring->irq_put = gen8_logical_ring_put_irq;
	ring->emit_bb_start = gen8_emit_bb_start;

	return logical_ring_init(dev, ring);
}

static int logical_blt_ring_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring = &dev_priv->ring[BCS];

	ring->name = "blitter ring";
	ring->id = BCS;
	ring->mmio_base = BLT_RING_BASE;
	ring->irq_enable_mask =
		GT_RENDER_USER_INTERRUPT << GEN8_BCS_IRQ_SHIFT;
	ring->irq_keep_mask =
		GT_CONTEXT_SWITCH_INTERRUPT << GEN8_BCS_IRQ_SHIFT;

	ring->init_hw = gen8_init_common_ring;
	if (IS_BROXTON(dev) && INTEL_REVID(dev) < BXT_REVID_B0) {
		ring->get_seqno = bxt_a_get_seqno;
		ring->set_seqno = bxt_a_set_seqno;
	} else {
		ring->get_seqno = gen8_get_seqno;
		ring->set_seqno = gen8_set_seqno;
	}
	ring->emit_request = gen8_emit_request;
	ring->emit_flush = gen8_emit_flush;
	ring->irq_get = gen8_logical_ring_get_irq;
	ring->irq_put = gen8_logical_ring_put_irq;
	ring->emit_bb_start = gen8_emit_bb_start;

	return logical_ring_init(dev, ring);
}

static int logical_vebox_ring_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring = &dev_priv->ring[VECS];

	ring->name = "video enhancement ring";
	ring->id = VECS;
	ring->mmio_base = VEBOX_RING_BASE;
	ring->irq_enable_mask =
		GT_RENDER_USER_INTERRUPT << GEN8_VECS_IRQ_SHIFT;
	ring->irq_keep_mask =
		GT_CONTEXT_SWITCH_INTERRUPT << GEN8_VECS_IRQ_SHIFT;

	ring->init_hw = gen8_init_common_ring;
	if (IS_BROXTON(dev) && INTEL_REVID(dev) < BXT_REVID_B0) {
		ring->get_seqno = bxt_a_get_seqno;
		ring->set_seqno = bxt_a_set_seqno;
	} else {
		ring->get_seqno = gen8_get_seqno;
		ring->set_seqno = gen8_set_seqno;
	}
	ring->emit_request = gen8_emit_request;
	ring->emit_flush = gen8_emit_flush;
	ring->irq_get = gen8_logical_ring_get_irq;
	ring->irq_put = gen8_logical_ring_put_irq;
	ring->emit_bb_start = gen8_emit_bb_start;

	return logical_ring_init(dev, ring);
}

/**
 * intel_logical_rings_init() - allocate, populate and init the Engine Command Streamers
 * @dev: DRM device.
 *
 * This function inits the engines for an Execlists submission style (the equivalent in the
 * legacy ringbuffer submission world would be i915_gem_init_rings). It does it only for
 * those engines that are present in the hardware.
 *
 * Return: non-zero if the initialization failed.
 */
int intel_logical_rings_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	int ret;

	ret = logical_render_ring_init(dev);
	if (ret)
		return ret;

	if (HAS_BSD(dev)) {
		ret = logical_bsd_ring_init(dev);
		if (ret)
			goto cleanup_render_ring;
	}

	if (HAS_BLT(dev)) {
		ret = logical_blt_ring_init(dev);
		if (ret)
			goto cleanup_bsd_ring;
	}

	if (HAS_VEBOX(dev)) {
		ret = logical_vebox_ring_init(dev);
		if (ret)
			goto cleanup_blt_ring;
	}

	if (HAS_BSD2(dev)) {
		ret = logical_bsd2_ring_init(dev);
		if (ret)
			goto cleanup_vebox_ring;
	}

	return 0;

cleanup_vebox_ring:
	intel_logical_ring_cleanup(&dev_priv->ring[VECS]);
cleanup_blt_ring:
	intel_logical_ring_cleanup(&dev_priv->ring[BCS]);
cleanup_bsd_ring:
	intel_logical_ring_cleanup(&dev_priv->ring[VCS]);
cleanup_render_ring:
	intel_logical_ring_cleanup(&dev_priv->ring[RCS]);

	return ret;
}

static u32
make_rpcs(struct drm_device *dev)
=======
int logical_xcs_ring_init(struct intel_engine_cs *engine)
{
	logical_ring_setup(engine);

	return logical_ring_init(engine);
}

static u32
make_rpcs(struct drm_i915_private *dev_priv)
>>>>>>> v4.9.227
{
	u32 rpcs = 0;

	/*
	 * No explicit RPCS request is needed to ensure full
	 * slice/subslice/EU enablement prior to Gen9.
	*/
<<<<<<< HEAD
	if (INTEL_INFO(dev)->gen < 9)
=======
	if (INTEL_GEN(dev_priv) < 9)
>>>>>>> v4.9.227
		return 0;

	/*
	 * Starting in Gen9, render power gating can leave
	 * slice/subslice/EU in a partially enabled state. We
	 * must make an explicit request through RPCS for full
	 * enablement.
	*/
<<<<<<< HEAD
	if (INTEL_INFO(dev)->has_slice_pg) {
		rpcs |= GEN8_RPCS_S_CNT_ENABLE;
		rpcs |= INTEL_INFO(dev)->slice_total <<
=======
	if (INTEL_INFO(dev_priv)->sseu.has_slice_pg) {
		rpcs |= GEN8_RPCS_S_CNT_ENABLE;
		rpcs |= hweight8(INTEL_INFO(dev_priv)->sseu.slice_mask) <<
>>>>>>> v4.9.227
			GEN8_RPCS_S_CNT_SHIFT;
		rpcs |= GEN8_RPCS_ENABLE;
	}

<<<<<<< HEAD
	if (INTEL_INFO(dev)->has_subslice_pg) {
		rpcs |= GEN8_RPCS_SS_CNT_ENABLE;
		rpcs |= INTEL_INFO(dev)->subslice_per_slice <<
=======
	if (INTEL_INFO(dev_priv)->sseu.has_subslice_pg) {
		rpcs |= GEN8_RPCS_SS_CNT_ENABLE;
		rpcs |= hweight8(INTEL_INFO(dev_priv)->sseu.subslice_mask) <<
>>>>>>> v4.9.227
			GEN8_RPCS_SS_CNT_SHIFT;
		rpcs |= GEN8_RPCS_ENABLE;
	}

<<<<<<< HEAD
	if (INTEL_INFO(dev)->has_eu_pg) {
		rpcs |= INTEL_INFO(dev)->eu_per_subslice <<
			GEN8_RPCS_EU_MIN_SHIFT;
		rpcs |= INTEL_INFO(dev)->eu_per_subslice <<
=======
	if (INTEL_INFO(dev_priv)->sseu.has_eu_pg) {
		rpcs |= INTEL_INFO(dev_priv)->sseu.eu_per_subslice <<
			GEN8_RPCS_EU_MIN_SHIFT;
		rpcs |= INTEL_INFO(dev_priv)->sseu.eu_per_subslice <<
>>>>>>> v4.9.227
			GEN8_RPCS_EU_MAX_SHIFT;
		rpcs |= GEN8_RPCS_ENABLE;
	}

	return rpcs;
}

<<<<<<< HEAD
static int
populate_lr_context(struct intel_context *ctx, struct drm_i915_gem_object *ctx_obj,
		    struct intel_engine_cs *ring, struct intel_ringbuffer *ringbuf)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct i915_hw_ppgtt *ppgtt = ctx->ppgtt;
	struct page *page;
	uint32_t *reg_state;
	int ret;

	if (!ppgtt)
		ppgtt = dev_priv->mm.aliasing_ppgtt;

	ret = i915_gem_object_set_to_cpu_domain(ctx_obj, true);
	if (ret) {
		DRM_DEBUG_DRIVER("Could not set to CPU domain\n");
		return ret;
	}

	ret = i915_gem_object_get_pages(ctx_obj);
	if (ret) {
		DRM_DEBUG_DRIVER("Could not get object pages\n");
		return ret;
	}

	i915_gem_object_pin_pages(ctx_obj);

	/* The second page of the context object contains some fields which must
	 * be set up prior to the first execution. */
	page = i915_gem_object_get_page(ctx_obj, LRC_STATE_PN);
	reg_state = kmap_atomic(page);
=======
static u32 intel_lr_indirect_ctx_offset(struct intel_engine_cs *engine)
{
	u32 indirect_ctx_offset;

	switch (INTEL_GEN(engine->i915)) {
	default:
		MISSING_CASE(INTEL_GEN(engine->i915));
		/* fall through */
	case 9:
		indirect_ctx_offset =
			GEN9_CTX_RCS_INDIRECT_CTX_OFFSET_DEFAULT;
		break;
	case 8:
		indirect_ctx_offset =
			GEN8_CTX_RCS_INDIRECT_CTX_OFFSET_DEFAULT;
		break;
	}

	return indirect_ctx_offset;
}

static void execlists_init_reg_state(u32 *reg_state,
				     struct i915_gem_context *ctx,
				     struct intel_engine_cs *engine,
				     struct intel_ring *ring)
{
	struct drm_i915_private *dev_priv = engine->i915;
	struct i915_hw_ppgtt *ppgtt = ctx->ppgtt ?: dev_priv->mm.aliasing_ppgtt;
>>>>>>> v4.9.227

	/* A context is actually a big batch buffer with several MI_LOAD_REGISTER_IMM
	 * commands followed by (reg, value) pairs. The values we are setting here are
	 * only for the first context restore: on a subsequent save, the GPU will
	 * recreate this batchbuffer with new values (including all the missing
	 * MI_LOAD_REGISTER_IMM commands that we are not initializing here). */
<<<<<<< HEAD
	if (ring->id == RCS)
		reg_state[CTX_LRI_HEADER_0] = MI_LOAD_REGISTER_IMM(14);
	else
		reg_state[CTX_LRI_HEADER_0] = MI_LOAD_REGISTER_IMM(11);
	reg_state[CTX_LRI_HEADER_0] |= MI_LRI_FORCE_POSTED;
	reg_state[CTX_CONTEXT_CONTROL] = RING_CONTEXT_CONTROL(ring);
	reg_state[CTX_CONTEXT_CONTROL+1] =
		_MASKED_BIT_ENABLE(CTX_CTRL_INHIBIT_SYN_CTX_SWITCH |
				   CTX_CTRL_ENGINE_CTX_RESTORE_INHIBIT |
				   CTX_CTRL_RS_CTX_ENABLE);
	reg_state[CTX_RING_HEAD] = RING_HEAD(ring->mmio_base);
	reg_state[CTX_RING_HEAD+1] = 0;
	reg_state[CTX_RING_TAIL] = RING_TAIL(ring->mmio_base);
	reg_state[CTX_RING_TAIL+1] = 0;
	reg_state[CTX_RING_BUFFER_START] = RING_START(ring->mmio_base);
	/* Ring buffer start address is not known until the buffer is pinned.
	 * It is written to the context image in execlists_update_context()
	 */
	reg_state[CTX_RING_BUFFER_CONTROL] = RING_CTL(ring->mmio_base);
	reg_state[CTX_RING_BUFFER_CONTROL+1] =
			((ringbuf->size - PAGE_SIZE) & RING_NR_PAGES) | RING_VALID;
	reg_state[CTX_BB_HEAD_U] = ring->mmio_base + 0x168;
	reg_state[CTX_BB_HEAD_U+1] = 0;
	reg_state[CTX_BB_HEAD_L] = ring->mmio_base + 0x140;
	reg_state[CTX_BB_HEAD_L+1] = 0;
	reg_state[CTX_BB_STATE] = ring->mmio_base + 0x110;
	reg_state[CTX_BB_STATE+1] = (1<<5);
	reg_state[CTX_SECOND_BB_HEAD_U] = ring->mmio_base + 0x11c;
	reg_state[CTX_SECOND_BB_HEAD_U+1] = 0;
	reg_state[CTX_SECOND_BB_HEAD_L] = ring->mmio_base + 0x114;
	reg_state[CTX_SECOND_BB_HEAD_L+1] = 0;
	reg_state[CTX_SECOND_BB_STATE] = ring->mmio_base + 0x118;
	reg_state[CTX_SECOND_BB_STATE+1] = 0;
	if (ring->id == RCS) {
		reg_state[CTX_BB_PER_CTX_PTR] = ring->mmio_base + 0x1c0;
		reg_state[CTX_BB_PER_CTX_PTR+1] = 0;
		reg_state[CTX_RCS_INDIRECT_CTX] = ring->mmio_base + 0x1c4;
		reg_state[CTX_RCS_INDIRECT_CTX+1] = 0;
		reg_state[CTX_RCS_INDIRECT_CTX_OFFSET] = ring->mmio_base + 0x1c8;
		reg_state[CTX_RCS_INDIRECT_CTX_OFFSET+1] = 0;
		if (ring->wa_ctx.obj) {
			struct i915_ctx_workarounds *wa_ctx = &ring->wa_ctx;
			uint32_t ggtt_offset = i915_gem_obj_ggtt_offset(wa_ctx->obj);
=======
	reg_state[CTX_LRI_HEADER_0] =
		MI_LOAD_REGISTER_IMM(engine->id == RCS ? 14 : 11) | MI_LRI_FORCE_POSTED;
	ASSIGN_CTX_REG(reg_state, CTX_CONTEXT_CONTROL,
		       RING_CONTEXT_CONTROL(engine),
		       _MASKED_BIT_ENABLE(CTX_CTRL_INHIBIT_SYN_CTX_SWITCH |
					  CTX_CTRL_ENGINE_CTX_RESTORE_INHIBIT |
					  (HAS_RESOURCE_STREAMER(dev_priv) ?
					   CTX_CTRL_RS_CTX_ENABLE : 0)));
	ASSIGN_CTX_REG(reg_state, CTX_RING_HEAD, RING_HEAD(engine->mmio_base),
		       0);
	ASSIGN_CTX_REG(reg_state, CTX_RING_TAIL, RING_TAIL(engine->mmio_base),
		       0);
	ASSIGN_CTX_REG(reg_state, CTX_RING_BUFFER_START,
		       RING_START(engine->mmio_base), 0);
	ASSIGN_CTX_REG(reg_state, CTX_RING_BUFFER_CONTROL,
		       RING_CTL(engine->mmio_base),
		       ((ring->size - PAGE_SIZE) & RING_NR_PAGES) | RING_VALID);
	ASSIGN_CTX_REG(reg_state, CTX_BB_HEAD_U,
		       RING_BBADDR_UDW(engine->mmio_base), 0);
	ASSIGN_CTX_REG(reg_state, CTX_BB_HEAD_L,
		       RING_BBADDR(engine->mmio_base), 0);
	ASSIGN_CTX_REG(reg_state, CTX_BB_STATE,
		       RING_BBSTATE(engine->mmio_base),
		       RING_BB_PPGTT);
	ASSIGN_CTX_REG(reg_state, CTX_SECOND_BB_HEAD_U,
		       RING_SBBADDR_UDW(engine->mmio_base), 0);
	ASSIGN_CTX_REG(reg_state, CTX_SECOND_BB_HEAD_L,
		       RING_SBBADDR(engine->mmio_base), 0);
	ASSIGN_CTX_REG(reg_state, CTX_SECOND_BB_STATE,
		       RING_SBBSTATE(engine->mmio_base), 0);
	if (engine->id == RCS) {
		ASSIGN_CTX_REG(reg_state, CTX_BB_PER_CTX_PTR,
			       RING_BB_PER_CTX_PTR(engine->mmio_base), 0);
		ASSIGN_CTX_REG(reg_state, CTX_RCS_INDIRECT_CTX,
			       RING_INDIRECT_CTX(engine->mmio_base), 0);
		ASSIGN_CTX_REG(reg_state, CTX_RCS_INDIRECT_CTX_OFFSET,
			       RING_INDIRECT_CTX_OFFSET(engine->mmio_base), 0);
		if (engine->wa_ctx.vma) {
			struct i915_ctx_workarounds *wa_ctx = &engine->wa_ctx;
			u32 ggtt_offset = i915_ggtt_offset(wa_ctx->vma);
>>>>>>> v4.9.227

			reg_state[CTX_RCS_INDIRECT_CTX+1] =
				(ggtt_offset + wa_ctx->indirect_ctx.offset * sizeof(uint32_t)) |
				(wa_ctx->indirect_ctx.size / CACHELINE_DWORDS);

			reg_state[CTX_RCS_INDIRECT_CTX_OFFSET+1] =
<<<<<<< HEAD
				CTX_RCS_INDIRECT_CTX_OFFSET_DEFAULT << 6;
=======
				intel_lr_indirect_ctx_offset(engine) << 6;
>>>>>>> v4.9.227

			reg_state[CTX_BB_PER_CTX_PTR+1] =
				(ggtt_offset + wa_ctx->per_ctx.offset * sizeof(uint32_t)) |
				0x01;
		}
	}
<<<<<<< HEAD
	reg_state[CTX_LRI_HEADER_1] = MI_LOAD_REGISTER_IMM(9);
	reg_state[CTX_LRI_HEADER_1] |= MI_LRI_FORCE_POSTED;
	reg_state[CTX_CTX_TIMESTAMP] = ring->mmio_base + 0x3a8;
	reg_state[CTX_CTX_TIMESTAMP+1] = 0;
	reg_state[CTX_PDP3_UDW] = GEN8_RING_PDP_UDW(ring, 3);
	reg_state[CTX_PDP3_LDW] = GEN8_RING_PDP_LDW(ring, 3);
	reg_state[CTX_PDP2_UDW] = GEN8_RING_PDP_UDW(ring, 2);
	reg_state[CTX_PDP2_LDW] = GEN8_RING_PDP_LDW(ring, 2);
	reg_state[CTX_PDP1_UDW] = GEN8_RING_PDP_UDW(ring, 1);
	reg_state[CTX_PDP1_LDW] = GEN8_RING_PDP_LDW(ring, 1);
	reg_state[CTX_PDP0_UDW] = GEN8_RING_PDP_UDW(ring, 0);
	reg_state[CTX_PDP0_LDW] = GEN8_RING_PDP_LDW(ring, 0);
=======
	reg_state[CTX_LRI_HEADER_1] = MI_LOAD_REGISTER_IMM(9) | MI_LRI_FORCE_POSTED;
	ASSIGN_CTX_REG(reg_state, CTX_CTX_TIMESTAMP,
		       RING_CTX_TIMESTAMP(engine->mmio_base), 0);
	/* PDP values well be assigned later if needed */
	ASSIGN_CTX_REG(reg_state, CTX_PDP3_UDW, GEN8_RING_PDP_UDW(engine, 3),
		       0);
	ASSIGN_CTX_REG(reg_state, CTX_PDP3_LDW, GEN8_RING_PDP_LDW(engine, 3),
		       0);
	ASSIGN_CTX_REG(reg_state, CTX_PDP2_UDW, GEN8_RING_PDP_UDW(engine, 2),
		       0);
	ASSIGN_CTX_REG(reg_state, CTX_PDP2_LDW, GEN8_RING_PDP_LDW(engine, 2),
		       0);
	ASSIGN_CTX_REG(reg_state, CTX_PDP1_UDW, GEN8_RING_PDP_UDW(engine, 1),
		       0);
	ASSIGN_CTX_REG(reg_state, CTX_PDP1_LDW, GEN8_RING_PDP_LDW(engine, 1),
		       0);
	ASSIGN_CTX_REG(reg_state, CTX_PDP0_UDW, GEN8_RING_PDP_UDW(engine, 0),
		       0);
	ASSIGN_CTX_REG(reg_state, CTX_PDP0_LDW, GEN8_RING_PDP_LDW(engine, 0),
		       0);
>>>>>>> v4.9.227

	if (USES_FULL_48BIT_PPGTT(ppgtt->base.dev)) {
		/* 64b PPGTT (48bit canonical)
		 * PDP0_DESCRIPTOR contains the base address to PML4 and
		 * other PDP Descriptors are ignored.
		 */
		ASSIGN_CTX_PML4(ppgtt, reg_state);
	} else {
		/* 32b PPGTT
		 * PDP*_DESCRIPTOR contains the base address of space supported.
		 * With dynamic page allocation, PDPs may not be allocated at
		 * this point. Point the unallocated PDPs to the scratch page
		 */
<<<<<<< HEAD
		ASSIGN_CTX_PDP(ppgtt, reg_state, 3);
		ASSIGN_CTX_PDP(ppgtt, reg_state, 2);
		ASSIGN_CTX_PDP(ppgtt, reg_state, 1);
		ASSIGN_CTX_PDP(ppgtt, reg_state, 0);
	}

	if (ring->id == RCS) {
		reg_state[CTX_LRI_HEADER_2] = MI_LOAD_REGISTER_IMM(1);
		reg_state[CTX_R_PWR_CLK_STATE] = GEN8_R_PWR_CLK_STATE;
		reg_state[CTX_R_PWR_CLK_STATE+1] = make_rpcs(dev);
	}

	kunmap_atomic(reg_state);

	ctx_obj->dirty = 1;
	set_page_dirty(page);
	i915_gem_object_unpin_pages(ctx_obj);
=======
		execlists_update_context_pdps(ppgtt, reg_state);
	}

	if (engine->id == RCS) {
		reg_state[CTX_LRI_HEADER_2] = MI_LOAD_REGISTER_IMM(1);
		ASSIGN_CTX_REG(reg_state, CTX_R_PWR_CLK_STATE, GEN8_R_PWR_CLK_STATE,
			       make_rpcs(dev_priv));
	}
}

static int
populate_lr_context(struct i915_gem_context *ctx,
		    struct drm_i915_gem_object *ctx_obj,
		    struct intel_engine_cs *engine,
		    struct intel_ring *ring)
{
	void *vaddr;
	int ret;

	ret = i915_gem_object_set_to_cpu_domain(ctx_obj, true);
	if (ret) {
		DRM_DEBUG_DRIVER("Could not set to CPU domain\n");
		return ret;
	}

	vaddr = i915_gem_object_pin_map(ctx_obj, I915_MAP_WB);
	if (IS_ERR(vaddr)) {
		ret = PTR_ERR(vaddr);
		DRM_DEBUG_DRIVER("Could not map object pages! (%d)\n", ret);
		return ret;
	}
	ctx_obj->dirty = true;

	/* The second page of the context object contains some fields which must
	 * be set up prior to the first execution. */

	execlists_init_reg_state(vaddr + LRC_STATE_PN * PAGE_SIZE,
				 ctx, engine, ring);

	i915_gem_object_unpin_map(ctx_obj);
>>>>>>> v4.9.227

	return 0;
}

/**
<<<<<<< HEAD
 * intel_lr_context_free() - free the LRC specific bits of a context
 * @ctx: the LR context to free.
 *
 * The real context freeing is done in i915_gem_context_free: this only
 * takes care of the bits that are LRC related: the per-engine backing
 * objects and the logical ringbuffer.
 */
void intel_lr_context_free(struct intel_context *ctx)
{
	int i;

	for (i = 0; i < I915_NUM_RINGS; i++) {
		struct drm_i915_gem_object *ctx_obj = ctx->engine[i].state;

		if (ctx_obj) {
			struct intel_ringbuffer *ringbuf =
					ctx->engine[i].ringbuf;
			struct intel_engine_cs *ring = ringbuf->ring;

			if (ctx == ring->default_context) {
				intel_unpin_ringbuffer_obj(ringbuf);
				i915_gem_object_ggtt_unpin(ctx_obj);
			}
			WARN_ON(ctx->engine[ring->id].pin_count);
			intel_ringbuffer_free(ringbuf);
			drm_gem_object_unreference(&ctx_obj->base);
		}
	}
}

static uint32_t get_lr_context_size(struct intel_engine_cs *ring)
{
	int ret = 0;

	WARN_ON(INTEL_INFO(ring->dev)->gen < 8);

	switch (ring->id) {
	case RCS:
		if (INTEL_INFO(ring->dev)->gen >= 9)
=======
 * intel_lr_context_size() - return the size of the context for an engine
 * @engine: which engine to find the context size for
 *
 * Each engine may require a different amount of space for a context image,
 * so when allocating (or copying) an image, this function can be used to
 * find the right size for the specific engine.
 *
 * Return: size (in bytes) of an engine-specific context image
 *
 * Note: this size includes the HWSP, which is part of the context image
 * in LRC mode, but does not include the "shared data page" used with
 * GuC submission. The caller should account for this if using the GuC.
 */
uint32_t intel_lr_context_size(struct intel_engine_cs *engine)
{
	int ret = 0;

	WARN_ON(INTEL_GEN(engine->i915) < 8);

	switch (engine->id) {
	case RCS:
		if (INTEL_GEN(engine->i915) >= 9)
>>>>>>> v4.9.227
			ret = GEN9_LR_CONTEXT_RENDER_SIZE;
		else
			ret = GEN8_LR_CONTEXT_RENDER_SIZE;
		break;
	case VCS:
	case BCS:
	case VECS:
	case VCS2:
		ret = GEN8_LR_CONTEXT_OTHER_SIZE;
		break;
	}

	return ret;
}

<<<<<<< HEAD
static void lrc_setup_hardware_status_page(struct intel_engine_cs *ring,
		struct drm_i915_gem_object *default_ctx_obj)
{
	struct drm_i915_private *dev_priv = ring->dev->dev_private;
	struct page *page;

	/* The HWSP is part of the default context object in LRC mode. */
	ring->status_page.gfx_addr = i915_gem_obj_ggtt_offset(default_ctx_obj)
			+ LRC_PPHWSP_PN * PAGE_SIZE;
	page = i915_gem_object_get_page(default_ctx_obj, LRC_PPHWSP_PN);
	ring->status_page.page_addr = kmap(page);
	ring->status_page.obj = default_ctx_obj;

	I915_WRITE(RING_HWS_PGA(ring->mmio_base),
			(u32)ring->status_page.gfx_addr);
	POSTING_READ(RING_HWS_PGA(ring->mmio_base));
}

/**
 * intel_lr_context_deferred_alloc() - create the LRC specific bits of a context
 * @ctx: LR context to create.
 * @ring: engine to be used with the context.
 *
 * This function can be called more than once, with different engines, if we plan
 * to use the context with them. The context backing objects and the ringbuffers
 * (specially the ringbuffer backing objects) suck a lot of memory up, and that's why
 * the creation is a deferred call: it's better to make sure first that we need to use
 * a given ring with the context.
 *
 * Return: non-zero on error.
 */

int intel_lr_context_deferred_alloc(struct intel_context *ctx,
				     struct intel_engine_cs *ring)
{
	struct drm_device *dev = ring->dev;
	struct drm_i915_gem_object *ctx_obj;
	uint32_t context_size;
	struct intel_ringbuffer *ringbuf;
	int ret;

	WARN_ON(ctx->legacy_hw_ctx.rcs_state != NULL);
	WARN_ON(ctx->engine[ring->id].state);

	context_size = round_up(get_lr_context_size(ring), 4096);
=======
static int execlists_context_deferred_alloc(struct i915_gem_context *ctx,
					    struct intel_engine_cs *engine)
{
	struct drm_i915_gem_object *ctx_obj;
	struct intel_context *ce = &ctx->engine[engine->id];
	struct i915_vma *vma;
	uint32_t context_size;
	struct intel_ring *ring;
	int ret;

	WARN_ON(ce->state);

	context_size = round_up(intel_lr_context_size(engine), 4096);
>>>>>>> v4.9.227

	/* One extra page as the sharing data between driver and GuC */
	context_size += PAGE_SIZE * LRC_PPHWSP_PN;

<<<<<<< HEAD
	ctx_obj = i915_gem_alloc_object(dev, context_size);
	if (!ctx_obj) {
		DRM_DEBUG_DRIVER("Alloc LRC backing obj failed.\n");
		return -ENOMEM;
	}

	ringbuf = intel_engine_create_ringbuffer(ring, 4 * PAGE_SIZE);
	if (IS_ERR(ringbuf)) {
		ret = PTR_ERR(ringbuf);
		goto error_deref_obj;
	}

	ret = populate_lr_context(ctx, ctx_obj, ring, ringbuf);
	if (ret) {
		DRM_DEBUG_DRIVER("Failed to populate LRC: %d\n", ret);
		goto error_ringbuf;
	}

	ctx->engine[ring->id].ringbuf = ringbuf;
	ctx->engine[ring->id].state = ctx_obj;

	if (ctx != ring->default_context && ring->init_context) {
		struct drm_i915_gem_request *req;

		ret = i915_gem_request_alloc(ring,
			ctx, &req);
		if (ret) {
			DRM_ERROR("ring create req: %d\n",
				ret);
			goto error_ringbuf;
		}

		ret = ring->init_context(req);
		if (ret) {
			DRM_ERROR("ring init context: %d\n",
				ret);
			i915_gem_request_cancel(req);
			goto error_ringbuf;
		}
		i915_add_request_no_flush(req);
	}
	return 0;

error_ringbuf:
	intel_ringbuffer_free(ringbuf);
error_deref_obj:
	drm_gem_object_unreference(&ctx_obj->base);
	ctx->engine[ring->id].ringbuf = NULL;
	ctx->engine[ring->id].state = NULL;
	return ret;
}

void intel_lr_context_reset(struct drm_device *dev,
			struct intel_context *ctx)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring;
	int i;

	for_each_ring(ring, dev_priv, i) {
		struct drm_i915_gem_object *ctx_obj =
				ctx->engine[ring->id].state;
		struct intel_ringbuffer *ringbuf =
				ctx->engine[ring->id].ringbuf;
		uint32_t *reg_state;
		struct page *page;

		if (!ctx_obj)
			continue;

		if (i915_gem_object_get_pages(ctx_obj)) {
			WARN(1, "Failed get_pages for context obj\n");
			continue;
		}
		page = i915_gem_object_get_page(ctx_obj, LRC_STATE_PN);
		reg_state = kmap_atomic(page);
=======
	ctx_obj = i915_gem_object_create(&ctx->i915->drm, context_size);
	if (IS_ERR(ctx_obj)) {
		DRM_DEBUG_DRIVER("Alloc LRC backing obj failed.\n");
		return PTR_ERR(ctx_obj);
	}

	vma = i915_vma_create(ctx_obj, &ctx->i915->ggtt.base, NULL);
	if (IS_ERR(vma)) {
		ret = PTR_ERR(vma);
		goto error_deref_obj;
	}

	ring = intel_engine_create_ring(engine, ctx->ring_size);
	if (IS_ERR(ring)) {
		ret = PTR_ERR(ring);
		goto error_deref_obj;
	}

	ret = populate_lr_context(ctx, ctx_obj, engine, ring);
	if (ret) {
		DRM_DEBUG_DRIVER("Failed to populate LRC: %d\n", ret);
		goto error_ring_free;
	}

	ce->ring = ring;
	ce->state = vma;
	ce->initialised = engine->init_context == NULL;

	return 0;

error_ring_free:
	intel_ring_free(ring);
error_deref_obj:
	i915_gem_object_put(ctx_obj);
	return ret;
}

void intel_lr_context_resume(struct drm_i915_private *dev_priv)
{
	struct i915_gem_context *ctx = dev_priv->kernel_context;
	struct intel_engine_cs *engine;

	for_each_engine(engine, dev_priv) {
		struct intel_context *ce = &ctx->engine[engine->id];
		void *vaddr;
		uint32_t *reg_state;

		if (!ce->state)
			continue;

		vaddr = i915_gem_object_pin_map(ce->state->obj, I915_MAP_WB);
		if (WARN_ON(IS_ERR(vaddr)))
			continue;

		reg_state = vaddr + LRC_STATE_PN * PAGE_SIZE;
>>>>>>> v4.9.227

		reg_state[CTX_RING_HEAD+1] = 0;
		reg_state[CTX_RING_TAIL+1] = 0;

<<<<<<< HEAD
		kunmap_atomic(reg_state);

		ringbuf->head = 0;
		ringbuf->tail = 0;
=======
		ce->state->obj->dirty = true;
		i915_gem_object_unpin_map(ce->state->obj);

		ce->ring->head = 0;
		ce->ring->tail = 0;
>>>>>>> v4.9.227
	}
}
