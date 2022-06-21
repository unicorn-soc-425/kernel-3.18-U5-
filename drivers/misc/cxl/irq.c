/*
 * Copyright 2014 IBM Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/pid.h>
#include <asm/cputable.h>
<<<<<<< HEAD
#include <misc/cxl.h>

#include "cxl.h"

/* XXX: This is implementation specific */
static irqreturn_t handle_psl_slice_error(struct cxl_context *ctx, u64 dsisr, u64 errstat)
{
	u64 fir1, fir2, fir_slice, serr, afu_debug;

	fir1 = cxl_p1_read(ctx->afu->adapter, CXL_PSL_FIR1);
	fir2 = cxl_p1_read(ctx->afu->adapter, CXL_PSL_FIR2);
	fir_slice = cxl_p1n_read(ctx->afu, CXL_PSL_FIR_SLICE_An);
	serr = cxl_p1n_read(ctx->afu, CXL_PSL_SERR_An);
	afu_debug = cxl_p1n_read(ctx->afu, CXL_AFU_DEBUG_An);

	dev_crit(&ctx->afu->dev, "PSL ERROR STATUS: 0x%.16llx\n", errstat);
	dev_crit(&ctx->afu->dev, "PSL_FIR1: 0x%.16llx\n", fir1);
	dev_crit(&ctx->afu->dev, "PSL_FIR2: 0x%.16llx\n", fir2);
	dev_crit(&ctx->afu->dev, "PSL_SERR_An: 0x%.16llx\n", serr);
	dev_crit(&ctx->afu->dev, "PSL_FIR_SLICE_An: 0x%.16llx\n", fir_slice);
	dev_crit(&ctx->afu->dev, "CXL_PSL_AFU_DEBUG_An: 0x%.16llx\n", afu_debug);

	dev_crit(&ctx->afu->dev, "STOPPING CXL TRACE\n");
	cxl_stop_trace(ctx->afu->adapter);

	return cxl_ack_irq(ctx, 0, errstat);
}

irqreturn_t cxl_slice_irq_err(int irq, void *data)
{
	struct cxl_afu *afu = data;
	u64 fir_slice, errstat, serr, afu_debug;

	WARN(irq, "CXL SLICE ERROR interrupt %i\n", irq);

	serr = cxl_p1n_read(afu, CXL_PSL_SERR_An);
	fir_slice = cxl_p1n_read(afu, CXL_PSL_FIR_SLICE_An);
	errstat = cxl_p2n_read(afu, CXL_PSL_ErrStat_An);
	afu_debug = cxl_p1n_read(afu, CXL_AFU_DEBUG_An);
	dev_crit(&afu->dev, "PSL_SERR_An: 0x%.16llx\n", serr);
	dev_crit(&afu->dev, "PSL_FIR_SLICE_An: 0x%.16llx\n", fir_slice);
	dev_crit(&afu->dev, "CXL_PSL_ErrStat_An: 0x%.16llx\n", errstat);
	dev_crit(&afu->dev, "CXL_PSL_AFU_DEBUG_An: 0x%.16llx\n", afu_debug);

	cxl_p1n_write(afu, CXL_PSL_SERR_An, serr);

	return IRQ_HANDLED;
}

static irqreturn_t cxl_irq_err(int irq, void *data)
{
	struct cxl *adapter = data;
	u64 fir1, fir2, err_ivte;

	WARN(1, "CXL ERROR interrupt %i\n", irq);

	err_ivte = cxl_p1_read(adapter, CXL_PSL_ErrIVTE);
	dev_crit(&adapter->dev, "PSL_ErrIVTE: 0x%.16llx\n", err_ivte);

	dev_crit(&adapter->dev, "STOPPING CXL TRACE\n");
	cxl_stop_trace(adapter);

	fir1 = cxl_p1_read(adapter, CXL_PSL_FIR1);
	fir2 = cxl_p1_read(adapter, CXL_PSL_FIR2);

	dev_crit(&adapter->dev, "PSL_FIR1: 0x%.16llx\nPSL_FIR2: 0x%.16llx\n", fir1, fir2);

	return IRQ_HANDLED;
=======
#include <misc/cxl-base.h>

#include "cxl.h"
#include "trace.h"

static int afu_irq_range_start(void)
{
	if (cpu_has_feature(CPU_FTR_HVMODE))
		return 1;
	return 0;
>>>>>>> v4.9.227
}

static irqreturn_t schedule_cxl_fault(struct cxl_context *ctx, u64 dsisr, u64 dar)
{
	ctx->dsisr = dsisr;
	ctx->dar = dar;
	schedule_work(&ctx->fault_work);
	return IRQ_HANDLED;
}

<<<<<<< HEAD
static irqreturn_t cxl_irq(int irq, void *data)
{
	struct cxl_context *ctx = data;
	struct cxl_irq_info irq_info;
	u64 dsisr, dar;
	int result;

	if ((result = cxl_get_irq(ctx, &irq_info))) {
		WARN(1, "Unable to get CXL IRQ Info: %i\n", result);
		return IRQ_HANDLED;
	}

	dsisr = irq_info.dsisr;
	dar = irq_info.dar;
=======
irqreturn_t cxl_irq(int irq, struct cxl_context *ctx, struct cxl_irq_info *irq_info)
{
	u64 dsisr, dar;

	dsisr = irq_info->dsisr;
	dar = irq_info->dar;

	trace_cxl_psl_irq(ctx, irq, dsisr, dar);
>>>>>>> v4.9.227

	pr_devel("CXL interrupt %i for afu pe: %i DSISR: %#llx DAR: %#llx\n", irq, ctx->pe, dsisr, dar);

	if (dsisr & CXL_PSL_DSISR_An_DS) {
		/*
		 * We don't inherently need to sleep to handle this, but we do
		 * need to get a ref to the task's mm, which we can't do from
		 * irq context without the potential for a deadlock since it
		 * takes the task_lock. An alternate option would be to keep a
		 * reference to the task's mm the entire time it has cxl open,
		 * but to do that we need to solve the issue where we hold a
		 * ref to the mm, but the mm can hold a ref to the fd after an
		 * mmap preventing anything from being cleaned up.
		 */
		pr_devel("Scheduling segment miss handling for later pe: %i\n", ctx->pe);
		return schedule_cxl_fault(ctx, dsisr, dar);
	}

	if (dsisr & CXL_PSL_DSISR_An_M)
		pr_devel("CXL interrupt: PTE not found\n");
	if (dsisr & CXL_PSL_DSISR_An_P)
		pr_devel("CXL interrupt: Storage protection violation\n");
	if (dsisr & CXL_PSL_DSISR_An_A)
		pr_devel("CXL interrupt: AFU lock access to write through or cache inhibited storage\n");
	if (dsisr & CXL_PSL_DSISR_An_S)
		pr_devel("CXL interrupt: Access was afu_wr or afu_zero\n");
	if (dsisr & CXL_PSL_DSISR_An_K)
		pr_devel("CXL interrupt: Access not permitted by virtual page class key protection\n");

	if (dsisr & CXL_PSL_DSISR_An_DM) {
		/*
		 * In some cases we might be able to handle the fault
		 * immediately if hash_page would succeed, but we still need
		 * the task's mm, which as above we can't get without a lock
		 */
		pr_devel("Scheduling page fault handling for later pe: %i\n", ctx->pe);
		return schedule_cxl_fault(ctx, dsisr, dar);
	}
	if (dsisr & CXL_PSL_DSISR_An_ST)
		WARN(1, "CXL interrupt: Segment Table PTE not found\n");
	if (dsisr & CXL_PSL_DSISR_An_UR)
		pr_devel("CXL interrupt: AURP PTE not found\n");
	if (dsisr & CXL_PSL_DSISR_An_PE)
<<<<<<< HEAD
		return handle_psl_slice_error(ctx, dsisr, irq_info.errstat);
	if (dsisr & CXL_PSL_DSISR_An_AE) {
		pr_devel("CXL interrupt: AFU Error %.llx\n", irq_info.afu_err);
=======
		return cxl_ops->handle_psl_slice_error(ctx, dsisr,
						irq_info->errstat);
	if (dsisr & CXL_PSL_DSISR_An_AE) {
		pr_devel("CXL interrupt: AFU Error 0x%016llx\n", irq_info->afu_err);
>>>>>>> v4.9.227

		if (ctx->pending_afu_err) {
			/*
			 * This shouldn't happen - the PSL treats these errors
			 * as fatal and will have reset the AFU, so there's not
			 * much point buffering multiple AFU errors.
			 * OTOH if we DO ever see a storm of these come in it's
			 * probably best that we log them somewhere:
			 */
			dev_err_ratelimited(&ctx->afu->dev, "CXL AFU Error "
<<<<<<< HEAD
					    "undelivered to pe %i: %.llx\n",
					    ctx->pe, irq_info.afu_err);
		} else {
			spin_lock(&ctx->lock);
			ctx->afu_err = irq_info.afu_err;
=======
					    "undelivered to pe %i: 0x%016llx\n",
					    ctx->pe, irq_info->afu_err);
		} else {
			spin_lock(&ctx->lock);
			ctx->afu_err = irq_info->afu_err;
>>>>>>> v4.9.227
			ctx->pending_afu_err = 1;
			spin_unlock(&ctx->lock);

			wake_up_all(&ctx->wq);
		}

<<<<<<< HEAD
		cxl_ack_irq(ctx, CXL_PSL_TFC_An_A, 0);
=======
		cxl_ops->ack_irq(ctx, CXL_PSL_TFC_An_A, 0);
>>>>>>> v4.9.227
		return IRQ_HANDLED;
	}
	if (dsisr & CXL_PSL_DSISR_An_OC)
		pr_devel("CXL interrupt: OS Context Warning\n");

	WARN(1, "Unhandled CXL PSL IRQ\n");
	return IRQ_HANDLED;
}

<<<<<<< HEAD
static irqreturn_t cxl_irq_multiplexed(int irq, void *data)
{
	struct cxl_afu *afu = data;
	struct cxl_context *ctx;
	int ph = cxl_p2n_read(afu, CXL_PSL_PEHandle_An) & 0xffff;
	int ret;

	rcu_read_lock();
	ctx = idr_find(&afu->contexts_idr, ph);
	if (ctx) {
		ret = cxl_irq(irq, ctx);
		rcu_read_unlock();
		return ret;
	}
	rcu_read_unlock();

	WARN(1, "Unable to demultiplex CXL PSL IRQ\n");
	return IRQ_HANDLED;
}

=======
>>>>>>> v4.9.227
static irqreturn_t cxl_irq_afu(int irq, void *data)
{
	struct cxl_context *ctx = data;
	irq_hw_number_t hwirq = irqd_to_hwirq(irq_get_irq_data(irq));
<<<<<<< HEAD
	int irq_off, afu_irq = 1;
	__u16 range;
	int r;

	for (r = 1; r < CXL_IRQ_RANGES; r++) {
=======
	int irq_off, afu_irq = 0;
	__u16 range;
	int r;

	/*
	 * Look for the interrupt number.
	 * On bare-metal, we know range 0 only contains the PSL
	 * interrupt so we could start counting at range 1 and initialize
	 * afu_irq at 1.
	 * In a guest, range 0 also contains AFU interrupts, so it must
	 * be counted for. Therefore we initialize afu_irq at 0 to take into
	 * account the PSL interrupt.
	 *
	 * For code-readability, it just seems easier to go over all
	 * the ranges on bare-metal and guest. The end result is the same.
	 */
	for (r = 0; r < CXL_IRQ_RANGES; r++) {
>>>>>>> v4.9.227
		irq_off = hwirq - ctx->irqs.offset[r];
		range = ctx->irqs.range[r];
		if (irq_off >= 0 && irq_off < range) {
			afu_irq += irq_off;
			break;
		}
		afu_irq += range;
	}
	if (unlikely(r >= CXL_IRQ_RANGES)) {
<<<<<<< HEAD
		WARN(1, "Recieved AFU IRQ out of range for pe %i (virq %i hwirq %lx)\n",
=======
		WARN(1, "Received AFU IRQ out of range for pe %i (virq %i hwirq %lx)\n",
>>>>>>> v4.9.227
		     ctx->pe, irq, hwirq);
		return IRQ_HANDLED;
	}

<<<<<<< HEAD
=======
	trace_cxl_afu_irq(ctx, afu_irq, irq, hwirq);
>>>>>>> v4.9.227
	pr_devel("Received AFU interrupt %i for pe: %i (virq %i hwirq %lx)\n",
	       afu_irq, ctx->pe, irq, hwirq);

	if (unlikely(!ctx->irq_bitmap)) {
<<<<<<< HEAD
		WARN(1, "Recieved AFU IRQ for context with no IRQ bitmap\n");
=======
		WARN(1, "Received AFU IRQ for context with no IRQ bitmap\n");
>>>>>>> v4.9.227
		return IRQ_HANDLED;
	}
	spin_lock(&ctx->lock);
	set_bit(afu_irq - 1, ctx->irq_bitmap);
	ctx->pending_irq = true;
	spin_unlock(&ctx->lock);

	wake_up_all(&ctx->wq);

	return IRQ_HANDLED;
}

unsigned int cxl_map_irq(struct cxl *adapter, irq_hw_number_t hwirq,
<<<<<<< HEAD
			 irq_handler_t handler, void *cookie)
=======
			 irq_handler_t handler, void *cookie, const char *name)
>>>>>>> v4.9.227
{
	unsigned int virq;
	int result;

	/* IRQ Domain? */
	virq = irq_create_mapping(NULL, hwirq);
	if (!virq) {
		dev_warn(&adapter->dev, "cxl_map_irq: irq_create_mapping failed\n");
		return 0;
	}

<<<<<<< HEAD
	cxl_setup_irq(adapter, hwirq, virq);

	pr_devel("hwirq %#lx mapped to virq %u\n", hwirq, virq);

	result = request_irq(virq, handler, 0, "cxl", cookie);
=======
	if (cxl_ops->setup_irq)
		cxl_ops->setup_irq(adapter, hwirq, virq);

	pr_devel("hwirq %#lx mapped to virq %u\n", hwirq, virq);

	result = request_irq(virq, handler, 0, name, cookie);
>>>>>>> v4.9.227
	if (result) {
		dev_warn(&adapter->dev, "cxl_map_irq: request_irq failed: %i\n", result);
		return 0;
	}

	return virq;
}

void cxl_unmap_irq(unsigned int virq, void *cookie)
{
	free_irq(virq, cookie);
}

<<<<<<< HEAD
static int cxl_register_one_irq(struct cxl *adapter,
				irq_handler_t handler,
				void *cookie,
				irq_hw_number_t *dest_hwirq,
				unsigned int *dest_virq)
{
	int hwirq, virq;

	if ((hwirq = cxl_alloc_one_irq(adapter)) < 0)
		return hwirq;

	if (!(virq = cxl_map_irq(adapter, hwirq, handler, cookie)))
=======
int cxl_register_one_irq(struct cxl *adapter,
			irq_handler_t handler,
			void *cookie,
			irq_hw_number_t *dest_hwirq,
			unsigned int *dest_virq,
			const char *name)
{
	int hwirq, virq;

	if ((hwirq = cxl_ops->alloc_one_irq(adapter)) < 0)
		return hwirq;

	if (!(virq = cxl_map_irq(adapter, hwirq, handler, cookie, name)))
>>>>>>> v4.9.227
		goto err;

	*dest_hwirq = hwirq;
	*dest_virq = virq;

	return 0;

err:
<<<<<<< HEAD
	cxl_release_one_irq(adapter, hwirq);
	return -ENOMEM;
}

int cxl_register_psl_err_irq(struct cxl *adapter)
{
	int rc;

	if ((rc = cxl_register_one_irq(adapter, cxl_irq_err, adapter,
				       &adapter->err_hwirq,
				       &adapter->err_virq)))
		return rc;

	cxl_p1_write(adapter, CXL_PSL_ErrIVTE, adapter->err_hwirq & 0xffff);

	return 0;
}

void cxl_release_psl_err_irq(struct cxl *adapter)
{
	cxl_p1_write(adapter, CXL_PSL_ErrIVTE, 0x0000000000000000);
	cxl_unmap_irq(adapter->err_virq, adapter);
	cxl_release_one_irq(adapter, adapter->err_hwirq);
}

int cxl_register_serr_irq(struct cxl_afu *afu)
{
	u64 serr;
	int rc;

	if ((rc = cxl_register_one_irq(afu->adapter, cxl_slice_irq_err, afu,
				       &afu->serr_hwirq,
				       &afu->serr_virq)))
		return rc;

	serr = cxl_p1n_read(afu, CXL_PSL_SERR_An);
	serr = (serr & 0x00ffffffffff0000ULL) | (afu->serr_hwirq & 0xffff);
	cxl_p1n_write(afu, CXL_PSL_SERR_An, serr);

	return 0;
}

void cxl_release_serr_irq(struct cxl_afu *afu)
{
	cxl_p1n_write(afu, CXL_PSL_SERR_An, 0x0000000000000000);
	cxl_unmap_irq(afu->serr_virq, afu);
	cxl_release_one_irq(afu->adapter, afu->serr_hwirq);
}

int cxl_register_psl_irq(struct cxl_afu *afu)
{
	return cxl_register_one_irq(afu->adapter, cxl_irq_multiplexed, afu,
			&afu->psl_hwirq, &afu->psl_virq);
}

void cxl_release_psl_irq(struct cxl_afu *afu)
{
	cxl_unmap_irq(afu->psl_virq, afu);
	cxl_release_one_irq(afu->adapter, afu->psl_hwirq);
}

int afu_register_irqs(struct cxl_context *ctx, u32 count)
{
	irq_hw_number_t hwirq;
	int rc, r, i;

	if ((rc = cxl_alloc_irq_ranges(&ctx->irqs, ctx->afu->adapter, count)))
		return rc;

	/* Multiplexed PSL Interrupt */
	ctx->irqs.offset[0] = ctx->afu->psl_hwirq;
	ctx->irqs.range[0] = 1;
=======
	cxl_ops->release_one_irq(adapter, hwirq);
	return -ENOMEM;
}

void afu_irq_name_free(struct cxl_context *ctx)
{
	struct cxl_irq_name *irq_name, *tmp;

	list_for_each_entry_safe(irq_name, tmp, &ctx->irq_names, list) {
		kfree(irq_name->name);
		list_del(&irq_name->list);
		kfree(irq_name);
	}
}

int afu_allocate_irqs(struct cxl_context *ctx, u32 count)
{
	int rc, r, i, j = 1;
	struct cxl_irq_name *irq_name;
	int alloc_count;

	/*
	 * In native mode, range 0 is reserved for the multiplexed
	 * PSL interrupt. It has been allocated when the AFU was initialized.
	 *
	 * In a guest, the PSL interrupt is not mutliplexed, but per-context,
	 * and is the first interrupt from range 0. It still needs to be
	 * allocated, so bump the count by one.
	 */
	if (cpu_has_feature(CPU_FTR_HVMODE))
		alloc_count = count;
	else
		alloc_count = count + 1;

	if ((rc = cxl_ops->alloc_irq_ranges(&ctx->irqs, ctx->afu->adapter,
							alloc_count)))
		return rc;

	if (cpu_has_feature(CPU_FTR_HVMODE)) {
		/* Multiplexed PSL Interrupt */
		ctx->irqs.offset[0] = ctx->afu->native->psl_hwirq;
		ctx->irqs.range[0] = 1;
	}
>>>>>>> v4.9.227

	ctx->irq_count = count;
	ctx->irq_bitmap = kcalloc(BITS_TO_LONGS(count),
				  sizeof(*ctx->irq_bitmap), GFP_KERNEL);
	if (!ctx->irq_bitmap)
<<<<<<< HEAD
		return -ENOMEM;
	for (r = 1; r < CXL_IRQ_RANGES; r++) {
		hwirq = ctx->irqs.offset[r];
		for (i = 0; i < ctx->irqs.range[r]; hwirq++, i++) {
			cxl_map_irq(ctx->afu->adapter, hwirq,
				     cxl_irq_afu, ctx);
		}
	}

	return 0;
}

void afu_release_irqs(struct cxl_context *ctx)
=======
		goto out;

	/*
	 * Allocate names first.  If any fail, bail out before allocating
	 * actual hardware IRQs.
	 */
	for (r = afu_irq_range_start(); r < CXL_IRQ_RANGES; r++) {
		for (i = 0; i < ctx->irqs.range[r]; i++) {
			irq_name = kmalloc(sizeof(struct cxl_irq_name),
					   GFP_KERNEL);
			if (!irq_name)
				goto out;
			irq_name->name = kasprintf(GFP_KERNEL, "cxl-%s-pe%i-%i",
						   dev_name(&ctx->afu->dev),
						   ctx->pe, j);
			if (!irq_name->name) {
				kfree(irq_name);
				goto out;
			}
			/* Add to tail so next look get the correct order */
			list_add_tail(&irq_name->list, &ctx->irq_names);
			j++;
		}
	}
	return 0;

out:
	cxl_ops->release_irq_ranges(&ctx->irqs, ctx->afu->adapter);
	afu_irq_name_free(ctx);
	return -ENOMEM;
}

static void afu_register_hwirqs(struct cxl_context *ctx)
{
	irq_hw_number_t hwirq;
	struct cxl_irq_name *irq_name;
	int r, i;
	irqreturn_t (*handler)(int irq, void *data);

	/* We've allocated all memory now, so let's do the irq allocations */
	irq_name = list_first_entry(&ctx->irq_names, struct cxl_irq_name, list);
	for (r = afu_irq_range_start(); r < CXL_IRQ_RANGES; r++) {
		hwirq = ctx->irqs.offset[r];
		for (i = 0; i < ctx->irqs.range[r]; hwirq++, i++) {
			if (r == 0 && i == 0)
				/*
				 * The very first interrupt of range 0 is
				 * always the PSL interrupt, but we only
				 * need to connect a handler for guests,
				 * because there's one PSL interrupt per
				 * context.
				 * On bare-metal, the PSL interrupt is
				 * multiplexed and was setup when the AFU
				 * was configured.
				 */
				handler = cxl_ops->psl_interrupt;
			else
				handler = cxl_irq_afu;
			cxl_map_irq(ctx->afu->adapter, hwirq, handler, ctx,
				irq_name->name);
			irq_name = list_next_entry(irq_name, list);
		}
	}
}

int afu_register_irqs(struct cxl_context *ctx, u32 count)
{
	int rc;

	rc = afu_allocate_irqs(ctx, count);
	if (rc)
		return rc;

	afu_register_hwirqs(ctx);
	return 0;
}

void afu_release_irqs(struct cxl_context *ctx, void *cookie)
>>>>>>> v4.9.227
{
	irq_hw_number_t hwirq;
	unsigned int virq;
	int r, i;

<<<<<<< HEAD
	for (r = 1; r < CXL_IRQ_RANGES; r++) {
=======
	for (r = afu_irq_range_start(); r < CXL_IRQ_RANGES; r++) {
>>>>>>> v4.9.227
		hwirq = ctx->irqs.offset[r];
		for (i = 0; i < ctx->irqs.range[r]; hwirq++, i++) {
			virq = irq_find_mapping(NULL, hwirq);
			if (virq)
<<<<<<< HEAD
				cxl_unmap_irq(virq, ctx);
		}
	}

	cxl_release_irq_ranges(&ctx->irqs, ctx->afu->adapter);
=======
				cxl_unmap_irq(virq, cookie);
		}
	}

	afu_irq_name_free(ctx);
	cxl_ops->release_irq_ranges(&ctx->irqs, ctx->afu->adapter);

	ctx->irq_count = 0;
}

void cxl_afu_decode_psl_serr(struct cxl_afu *afu, u64 serr)
{
	dev_crit(&afu->dev,
		 "PSL Slice error received. Check AFU for root cause.\n");
	dev_crit(&afu->dev, "PSL_SERR_An: 0x%016llx\n", serr);
	if (serr & CXL_PSL_SERR_An_afuto)
		dev_crit(&afu->dev, "AFU MMIO Timeout\n");
	if (serr & CXL_PSL_SERR_An_afudis)
		dev_crit(&afu->dev,
			 "MMIO targeted Accelerator that was not enabled\n");
	if (serr & CXL_PSL_SERR_An_afuov)
		dev_crit(&afu->dev, "AFU CTAG Overflow\n");
	if (serr & CXL_PSL_SERR_An_badsrc)
		dev_crit(&afu->dev, "Bad Interrupt Source\n");
	if (serr & CXL_PSL_SERR_An_badctx)
		dev_crit(&afu->dev, "Bad Context Handle\n");
	if (serr & CXL_PSL_SERR_An_llcmdis)
		dev_crit(&afu->dev, "LLCMD to Disabled AFU\n");
	if (serr & CXL_PSL_SERR_An_llcmdto)
		dev_crit(&afu->dev, "LLCMD Timeout to AFU\n");
	if (serr & CXL_PSL_SERR_An_afupar)
		dev_crit(&afu->dev, "AFU MMIO Parity Error\n");
	if (serr & CXL_PSL_SERR_An_afudup)
		dev_crit(&afu->dev, "AFU MMIO Duplicate CTAG Error\n");
	if (serr & CXL_PSL_SERR_An_AE)
		dev_crit(&afu->dev,
			 "AFU asserted JDONE with JERROR in AFU Directed Mode\n");
>>>>>>> v4.9.227
}
