/*
 * Copyright © 2013 Intel Corporation
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
 */

#include "i915_drv.h"
#include "intel_drv.h"
#include "i915_vgpu.h"

#include <linux/pm_runtime.h>

#define FORCEWAKE_ACK_TIMEOUT_MS 50

<<<<<<< HEAD
#define __raw_i915_read8(dev_priv__, reg__) readb((dev_priv__)->regs + (reg__))
#define __raw_i915_write8(dev_priv__, reg__, val__) writeb(val__, (dev_priv__)->regs + (reg__))

#define __raw_i915_read16(dev_priv__, reg__) readw((dev_priv__)->regs + (reg__))
#define __raw_i915_write16(dev_priv__, reg__, val__) writew(val__, (dev_priv__)->regs + (reg__))

#define __raw_i915_read32(dev_priv__, reg__) readl((dev_priv__)->regs + (reg__))
#define __raw_i915_write32(dev_priv__, reg__, val__) writel(val__, (dev_priv__)->regs + (reg__))

#define __raw_i915_read64(dev_priv__, reg__) readq((dev_priv__)->regs + (reg__))
#define __raw_i915_write64(dev_priv__, reg__, val__) writeq(val__, (dev_priv__)->regs + (reg__))

#define __raw_posting_read(dev_priv__, reg__) (void)__raw_i915_read32(dev_priv__, reg__)
=======
#define __raw_posting_read(dev_priv__, reg__) (void)__raw_i915_read32((dev_priv__), (reg__))
>>>>>>> v4.9.227

static const char * const forcewake_domain_names[] = {
	"render",
	"blitter",
	"media",
};

const char *
intel_uncore_forcewake_domain_to_str(const enum forcewake_domain_id id)
{
	BUILD_BUG_ON(ARRAY_SIZE(forcewake_domain_names) != FW_DOMAIN_ID_COUNT);

	if (id >= 0 && id < FW_DOMAIN_ID_COUNT)
		return forcewake_domain_names[id];

	WARN_ON(id);

	return "unknown";
}

<<<<<<< HEAD
static void
assert_device_not_suspended(struct drm_i915_private *dev_priv)
{
	WARN_ONCE(HAS_RUNTIME_PM(dev_priv->dev) && dev_priv->pm.suspended,
		  "Device suspended\n");
}

static inline void
fw_domain_reset(const struct intel_uncore_forcewake_domain *d)
{
	WARN_ON(d->reg_set == 0);
=======
static inline void
fw_domain_reset(const struct intel_uncore_forcewake_domain *d)
{
	WARN_ON(!i915_mmio_reg_valid(d->reg_set));
>>>>>>> v4.9.227
	__raw_i915_write32(d->i915, d->reg_set, d->val_reset);
}

static inline void
fw_domain_arm_timer(struct intel_uncore_forcewake_domain *d)
{
<<<<<<< HEAD
	mod_timer_pinned(&d->timer, jiffies + 1);
=======
	d->wake_count++;
	hrtimer_start_range_ns(&d->timer,
			       ktime_set(0, NSEC_PER_MSEC),
			       NSEC_PER_MSEC,
			       HRTIMER_MODE_REL);
>>>>>>> v4.9.227
}

static inline void
fw_domain_wait_ack_clear(const struct intel_uncore_forcewake_domain *d)
{
	if (wait_for_atomic((__raw_i915_read32(d->i915, d->reg_ack) &
			     FORCEWAKE_KERNEL) == 0,
			    FORCEWAKE_ACK_TIMEOUT_MS))
		DRM_ERROR("%s: timed out waiting for forcewake ack to clear.\n",
			  intel_uncore_forcewake_domain_to_str(d->id));
}

static inline void
fw_domain_get(const struct intel_uncore_forcewake_domain *d)
{
	__raw_i915_write32(d->i915, d->reg_set, d->val_set);
}

static inline void
fw_domain_wait_ack(const struct intel_uncore_forcewake_domain *d)
{
	if (wait_for_atomic((__raw_i915_read32(d->i915, d->reg_ack) &
			     FORCEWAKE_KERNEL),
			    FORCEWAKE_ACK_TIMEOUT_MS))
		DRM_ERROR("%s: timed out waiting for forcewake ack request.\n",
			  intel_uncore_forcewake_domain_to_str(d->id));
}

static inline void
fw_domain_put(const struct intel_uncore_forcewake_domain *d)
{
	__raw_i915_write32(d->i915, d->reg_set, d->val_clear);
}

static inline void
fw_domain_posting_read(const struct intel_uncore_forcewake_domain *d)
{
	/* something from same cacheline, but not from the set register */
<<<<<<< HEAD
	if (d->reg_post)
=======
	if (i915_mmio_reg_valid(d->reg_post))
>>>>>>> v4.9.227
		__raw_posting_read(d->i915, d->reg_post);
}

static void
fw_domains_get(struct drm_i915_private *dev_priv, enum forcewake_domains fw_domains)
{
	struct intel_uncore_forcewake_domain *d;
<<<<<<< HEAD
	enum forcewake_domain_id id;

	for_each_fw_domain_mask(d, fw_domains, dev_priv, id) {
		fw_domain_wait_ack_clear(d);
		fw_domain_get(d);
		fw_domain_wait_ack(d);
	}
=======

	for_each_fw_domain_masked(d, fw_domains, dev_priv) {
		fw_domain_wait_ack_clear(d);
		fw_domain_get(d);
	}

	for_each_fw_domain_masked(d, fw_domains, dev_priv)
		fw_domain_wait_ack(d);
>>>>>>> v4.9.227
}

static void
fw_domains_put(struct drm_i915_private *dev_priv, enum forcewake_domains fw_domains)
{
	struct intel_uncore_forcewake_domain *d;
<<<<<<< HEAD
	enum forcewake_domain_id id;

	for_each_fw_domain_mask(d, fw_domains, dev_priv, id) {
=======

	for_each_fw_domain_masked(d, fw_domains, dev_priv) {
>>>>>>> v4.9.227
		fw_domain_put(d);
		fw_domain_posting_read(d);
	}
}

static void
fw_domains_posting_read(struct drm_i915_private *dev_priv)
{
	struct intel_uncore_forcewake_domain *d;
<<<<<<< HEAD
	enum forcewake_domain_id id;

	/* No need to do for all, just do for first found */
	for_each_fw_domain(d, dev_priv, id) {
=======

	/* No need to do for all, just do for first found */
	for_each_fw_domain(d, dev_priv) {
>>>>>>> v4.9.227
		fw_domain_posting_read(d);
		break;
	}
}

static void
fw_domains_reset(struct drm_i915_private *dev_priv, enum forcewake_domains fw_domains)
{
	struct intel_uncore_forcewake_domain *d;
<<<<<<< HEAD
	enum forcewake_domain_id id;
=======
>>>>>>> v4.9.227

	if (dev_priv->uncore.fw_domains == 0)
		return;

<<<<<<< HEAD
	for_each_fw_domain_mask(d, fw_domains, dev_priv, id)
=======
	for_each_fw_domain_masked(d, fw_domains, dev_priv)
>>>>>>> v4.9.227
		fw_domain_reset(d);

	fw_domains_posting_read(dev_priv);
}

static void __gen6_gt_wait_for_thread_c0(struct drm_i915_private *dev_priv)
{
	/* w/a for a sporadic read returning 0 by waiting for the GT
	 * thread to wake up.
	 */
	if (wait_for_atomic_us((__raw_i915_read32(dev_priv, GEN6_GT_THREAD_STATUS_REG) &
				GEN6_GT_THREAD_STATUS_CORE_MASK) == 0, 500))
		DRM_ERROR("GT thread status wait timed out\n");
}

static void fw_domains_get_with_thread_status(struct drm_i915_private *dev_priv,
					      enum forcewake_domains fw_domains)
{
	fw_domains_get(dev_priv, fw_domains);

	/* WaRsForcewakeWaitTC0:snb,ivb,hsw,bdw,vlv */
	__gen6_gt_wait_for_thread_c0(dev_priv);
}

static void gen6_gt_check_fifodbg(struct drm_i915_private *dev_priv)
{
	u32 gtfifodbg;

	gtfifodbg = __raw_i915_read32(dev_priv, GTFIFODBG);
	if (WARN(gtfifodbg, "GT wake FIFO error 0x%x\n", gtfifodbg))
		__raw_i915_write32(dev_priv, GTFIFODBG, gtfifodbg);
}

static void fw_domains_put_with_fifo(struct drm_i915_private *dev_priv,
				     enum forcewake_domains fw_domains)
{
	fw_domains_put(dev_priv, fw_domains);
	gen6_gt_check_fifodbg(dev_priv);
}

static inline u32 fifo_free_entries(struct drm_i915_private *dev_priv)
{
	u32 count = __raw_i915_read32(dev_priv, GTFIFOCTL);

	return count & GT_FIFO_FREE_ENTRIES_MASK;
}

static int __gen6_gt_wait_for_fifo(struct drm_i915_private *dev_priv)
{
	int ret = 0;

	/* On VLV, FIFO will be shared by both SW and HW.
	 * So, we need to read the FREE_ENTRIES everytime */
<<<<<<< HEAD
	if (IS_VALLEYVIEW(dev_priv->dev))
=======
	if (IS_VALLEYVIEW(dev_priv))
>>>>>>> v4.9.227
		dev_priv->uncore.fifo_count = fifo_free_entries(dev_priv);

	if (dev_priv->uncore.fifo_count < GT_FIFO_NUM_RESERVED_ENTRIES) {
		int loop = 500;
		u32 fifo = fifo_free_entries(dev_priv);

		while (fifo <= GT_FIFO_NUM_RESERVED_ENTRIES && loop--) {
			udelay(10);
			fifo = fifo_free_entries(dev_priv);
		}
		if (WARN_ON(loop < 0 && fifo <= GT_FIFO_NUM_RESERVED_ENTRIES))
			++ret;
		dev_priv->uncore.fifo_count = fifo;
	}
	dev_priv->uncore.fifo_count--;

	return ret;
}

<<<<<<< HEAD
static void intel_uncore_fw_release_timer(unsigned long arg)
{
	struct intel_uncore_forcewake_domain *domain = (void *)arg;
	unsigned long irqflags;

	assert_device_not_suspended(domain->i915);
=======
static enum hrtimer_restart
intel_uncore_fw_release_timer(struct hrtimer *timer)
{
	struct intel_uncore_forcewake_domain *domain =
	       container_of(timer, struct intel_uncore_forcewake_domain, timer);
	unsigned long irqflags;

	assert_rpm_device_not_suspended(domain->i915);
>>>>>>> v4.9.227

	spin_lock_irqsave(&domain->i915->uncore.lock, irqflags);
	if (WARN_ON(domain->wake_count == 0))
		domain->wake_count++;

	if (--domain->wake_count == 0)
		domain->i915->uncore.funcs.force_wake_put(domain->i915,
							  1 << domain->id);

	spin_unlock_irqrestore(&domain->i915->uncore.lock, irqflags);
<<<<<<< HEAD
}

void intel_uncore_forcewake_reset(struct drm_device *dev, bool restore)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	unsigned long irqflags;
	struct intel_uncore_forcewake_domain *domain;
	int retry_count = 100;
	enum forcewake_domain_id id;
=======

	return HRTIMER_NORESTART;
}

void intel_uncore_forcewake_reset(struct drm_i915_private *dev_priv,
				  bool restore)
{
	unsigned long irqflags;
	struct intel_uncore_forcewake_domain *domain;
	int retry_count = 100;
>>>>>>> v4.9.227
	enum forcewake_domains fw = 0, active_domains;

	/* Hold uncore.lock across reset to prevent any register access
	 * with forcewake not set correctly. Wait until all pending
	 * timers are run before holding.
	 */
	while (1) {
		active_domains = 0;

<<<<<<< HEAD
		for_each_fw_domain(domain, dev_priv, id) {
			if (del_timer_sync(&domain->timer) == 0)
				continue;

			intel_uncore_fw_release_timer((unsigned long)domain);
=======
		for_each_fw_domain(domain, dev_priv) {
			if (hrtimer_cancel(&domain->timer) == 0)
				continue;

			intel_uncore_fw_release_timer(&domain->timer);
>>>>>>> v4.9.227
		}

		spin_lock_irqsave(&dev_priv->uncore.lock, irqflags);

<<<<<<< HEAD
		for_each_fw_domain(domain, dev_priv, id) {
			if (timer_pending(&domain->timer))
				active_domains |= (1 << id);
=======
		for_each_fw_domain(domain, dev_priv) {
			if (hrtimer_active(&domain->timer))
				active_domains |= domain->mask;
>>>>>>> v4.9.227
		}

		if (active_domains == 0)
			break;

		if (--retry_count == 0) {
			DRM_ERROR("Timed out waiting for forcewake timers to finish\n");
			break;
		}

		spin_unlock_irqrestore(&dev_priv->uncore.lock, irqflags);
		cond_resched();
	}

	WARN_ON(active_domains);

<<<<<<< HEAD
	for_each_fw_domain(domain, dev_priv, id)
		if (domain->wake_count)
			fw |= 1 << id;
=======
	for_each_fw_domain(domain, dev_priv)
		if (domain->wake_count)
			fw |= domain->mask;
>>>>>>> v4.9.227

	if (fw)
		dev_priv->uncore.funcs.force_wake_put(dev_priv, fw);

	fw_domains_reset(dev_priv, FORCEWAKE_ALL);

	if (restore) { /* If reset with a user forcewake, try to restore */
		if (fw)
			dev_priv->uncore.funcs.force_wake_get(dev_priv, fw);

<<<<<<< HEAD
		if (IS_GEN6(dev) || IS_GEN7(dev))
=======
		if (IS_GEN6(dev_priv) || IS_GEN7(dev_priv))
>>>>>>> v4.9.227
			dev_priv->uncore.fifo_count =
				fifo_free_entries(dev_priv);
	}

	if (!restore)
		assert_forcewakes_inactive(dev_priv);

	spin_unlock_irqrestore(&dev_priv->uncore.lock, irqflags);
}

<<<<<<< HEAD
static void intel_uncore_ellc_detect(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;

	if ((IS_HASWELL(dev) || IS_BROADWELL(dev) ||
	     INTEL_INFO(dev)->gen >= 9) &&
	    (__raw_i915_read32(dev_priv, HSW_EDRAM_PRESENT) & EDRAM_ENABLED)) {
		/* The docs do not explain exactly how the calculation can be
		 * made. It is somewhat guessable, but for now, it's always
		 * 128MB.
		 * NB: We can't write IDICR yet because we do not have gt funcs
		 * set up */
		dev_priv->ellc_size = 128;
		DRM_INFO("Found %zuMB of eLLC\n", dev_priv->ellc_size);
	}
}

static void __intel_uncore_early_sanitize(struct drm_device *dev,
					  bool restore_forcewake)
{
	struct drm_i915_private *dev_priv = dev->dev_private;

	if (HAS_FPGA_DBG_UNCLAIMED(dev))
		__raw_i915_write32(dev_priv, FPGA_DBG, FPGA_DBG_RM_NOCLAIM);

	/* clear out old GT FIFO errors */
	if (IS_GEN6(dev) || IS_GEN7(dev))
=======
static u64 gen9_edram_size(struct drm_i915_private *dev_priv)
{
	const unsigned int ways[8] = { 4, 8, 12, 16, 16, 16, 16, 16 };
	const unsigned int sets[4] = { 1, 1, 2, 2 };
	const u32 cap = dev_priv->edram_cap;

	return EDRAM_NUM_BANKS(cap) *
		ways[EDRAM_WAYS_IDX(cap)] *
		sets[EDRAM_SETS_IDX(cap)] *
		1024 * 1024;
}

u64 intel_uncore_edram_size(struct drm_i915_private *dev_priv)
{
	if (!HAS_EDRAM(dev_priv))
		return 0;

	/* The needed capability bits for size calculation
	 * are not there with pre gen9 so return 128MB always.
	 */
	if (INTEL_GEN(dev_priv) < 9)
		return 128 * 1024 * 1024;

	return gen9_edram_size(dev_priv);
}

static void intel_uncore_edram_detect(struct drm_i915_private *dev_priv)
{
	if (IS_HASWELL(dev_priv) ||
	    IS_BROADWELL(dev_priv) ||
	    INTEL_GEN(dev_priv) >= 9) {
		dev_priv->edram_cap = __raw_i915_read32(dev_priv,
							HSW_EDRAM_CAP);

		/* NB: We can't write IDICR yet because we do not have gt funcs
		 * set up */
	} else {
		dev_priv->edram_cap = 0;
	}

	if (HAS_EDRAM(dev_priv))
		DRM_INFO("Found %lluMB of eDRAM\n",
			 intel_uncore_edram_size(dev_priv) / (1024 * 1024));
}

static bool
fpga_check_for_unclaimed_mmio(struct drm_i915_private *dev_priv)
{
	u32 dbg;

	dbg = __raw_i915_read32(dev_priv, FPGA_DBG);
	if (likely(!(dbg & FPGA_DBG_RM_NOCLAIM)))
		return false;

	__raw_i915_write32(dev_priv, FPGA_DBG, FPGA_DBG_RM_NOCLAIM);

	return true;
}

static bool
vlv_check_for_unclaimed_mmio(struct drm_i915_private *dev_priv)
{
	u32 cer;

	cer = __raw_i915_read32(dev_priv, CLAIM_ER);
	if (likely(!(cer & (CLAIM_ER_OVERFLOW | CLAIM_ER_CTR_MASK))))
		return false;

	__raw_i915_write32(dev_priv, CLAIM_ER, CLAIM_ER_CLR);

	return true;
}

static bool
check_for_unclaimed_mmio(struct drm_i915_private *dev_priv)
{
	if (HAS_FPGA_DBG_UNCLAIMED(dev_priv))
		return fpga_check_for_unclaimed_mmio(dev_priv);

	if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv))
		return vlv_check_for_unclaimed_mmio(dev_priv);

	return false;
}

static void __intel_uncore_early_sanitize(struct drm_i915_private *dev_priv,
					  bool restore_forcewake)
{
	/* clear out unclaimed reg detection bit */
	if (check_for_unclaimed_mmio(dev_priv))
		DRM_DEBUG("unclaimed mmio detected on uncore init, clearing\n");

	/* clear out old GT FIFO errors */
	if (IS_GEN6(dev_priv) || IS_GEN7(dev_priv))
>>>>>>> v4.9.227
		__raw_i915_write32(dev_priv, GTFIFODBG,
				   __raw_i915_read32(dev_priv, GTFIFODBG));

	/* WaDisableShadowRegForCpd:chv */
<<<<<<< HEAD
	if (IS_CHERRYVIEW(dev)) {
=======
	if (IS_CHERRYVIEW(dev_priv)) {
>>>>>>> v4.9.227
		__raw_i915_write32(dev_priv, GTFIFOCTL,
				   __raw_i915_read32(dev_priv, GTFIFOCTL) |
				   GT_FIFO_CTL_BLOCK_ALL_POLICY_STALL |
				   GT_FIFO_CTL_RC6_POLICY_STALL);
	}

<<<<<<< HEAD
	intel_uncore_forcewake_reset(dev, restore_forcewake);
}

void intel_uncore_early_sanitize(struct drm_device *dev, bool restore_forcewake)
{
	__intel_uncore_early_sanitize(dev, restore_forcewake);
	i915_check_and_clear_faults(dev);
}

void intel_uncore_sanitize(struct drm_device *dev)
{
	/* BIOS often leaves RC6 enabled, but disable it for hw init */
	intel_disable_gt_powersave(dev);
=======
	intel_uncore_forcewake_reset(dev_priv, restore_forcewake);
}

void intel_uncore_early_sanitize(struct drm_i915_private *dev_priv,
				 bool restore_forcewake)
{
	__intel_uncore_early_sanitize(dev_priv, restore_forcewake);
	i915_check_and_clear_faults(dev_priv);
}

void intel_uncore_sanitize(struct drm_i915_private *dev_priv)
{
	i915.enable_rc6 = sanitize_rc6_option(dev_priv, i915.enable_rc6);

	/* BIOS often leaves RC6 enabled, but disable it for hw init */
	intel_sanitize_gt_powersave(dev_priv);
>>>>>>> v4.9.227
}

static void __intel_uncore_forcewake_get(struct drm_i915_private *dev_priv,
					 enum forcewake_domains fw_domains)
{
	struct intel_uncore_forcewake_domain *domain;
<<<<<<< HEAD
	enum forcewake_domain_id id;
=======
>>>>>>> v4.9.227

	if (!dev_priv->uncore.funcs.force_wake_get)
		return;

	fw_domains &= dev_priv->uncore.fw_domains;

<<<<<<< HEAD
	for_each_fw_domain_mask(domain, fw_domains, dev_priv, id) {
		if (domain->wake_count++)
			fw_domains &= ~(1 << id);
=======
	for_each_fw_domain_masked(domain, fw_domains, dev_priv) {
		if (domain->wake_count++)
			fw_domains &= ~domain->mask;
>>>>>>> v4.9.227
	}

	if (fw_domains)
		dev_priv->uncore.funcs.force_wake_get(dev_priv, fw_domains);
}

/**
 * intel_uncore_forcewake_get - grab forcewake domain references
 * @dev_priv: i915 device instance
 * @fw_domains: forcewake domains to get reference on
 *
 * This function can be used get GT's forcewake domain references.
 * Normal register access will handle the forcewake domains automatically.
 * However if some sequence requires the GT to not power down a particular
 * forcewake domains this function should be called at the beginning of the
 * sequence. And subsequently the reference should be dropped by symmetric
 * call to intel_unforce_forcewake_put(). Usually caller wants all the domains
 * to be kept awake so the @fw_domains would be then FORCEWAKE_ALL.
 */
void intel_uncore_forcewake_get(struct drm_i915_private *dev_priv,
				enum forcewake_domains fw_domains)
{
	unsigned long irqflags;

	if (!dev_priv->uncore.funcs.force_wake_get)
		return;

<<<<<<< HEAD
	WARN_ON(dev_priv->pm.suspended);
=======
	assert_rpm_wakelock_held(dev_priv);
>>>>>>> v4.9.227

	spin_lock_irqsave(&dev_priv->uncore.lock, irqflags);
	__intel_uncore_forcewake_get(dev_priv, fw_domains);
	spin_unlock_irqrestore(&dev_priv->uncore.lock, irqflags);
}

/**
 * intel_uncore_forcewake_get__locked - grab forcewake domain references
 * @dev_priv: i915 device instance
 * @fw_domains: forcewake domains to get reference on
 *
 * See intel_uncore_forcewake_get(). This variant places the onus
 * on the caller to explicitly handle the dev_priv->uncore.lock spinlock.
 */
void intel_uncore_forcewake_get__locked(struct drm_i915_private *dev_priv,
					enum forcewake_domains fw_domains)
{
	assert_spin_locked(&dev_priv->uncore.lock);

	if (!dev_priv->uncore.funcs.force_wake_get)
		return;

	__intel_uncore_forcewake_get(dev_priv, fw_domains);
}

static void __intel_uncore_forcewake_put(struct drm_i915_private *dev_priv,
					 enum forcewake_domains fw_domains)
{
	struct intel_uncore_forcewake_domain *domain;
<<<<<<< HEAD
	enum forcewake_domain_id id;
=======
>>>>>>> v4.9.227

	if (!dev_priv->uncore.funcs.force_wake_put)
		return;

	fw_domains &= dev_priv->uncore.fw_domains;

<<<<<<< HEAD
	for_each_fw_domain_mask(domain, fw_domains, dev_priv, id) {
=======
	for_each_fw_domain_masked(domain, fw_domains, dev_priv) {
>>>>>>> v4.9.227
		if (WARN_ON(domain->wake_count == 0))
			continue;

		if (--domain->wake_count)
			continue;

<<<<<<< HEAD
		domain->wake_count++;
=======
>>>>>>> v4.9.227
		fw_domain_arm_timer(domain);
	}
}

/**
 * intel_uncore_forcewake_put - release a forcewake domain reference
 * @dev_priv: i915 device instance
 * @fw_domains: forcewake domains to put references
 *
 * This function drops the device-level forcewakes for specified
 * domains obtained by intel_uncore_forcewake_get().
 */
void intel_uncore_forcewake_put(struct drm_i915_private *dev_priv,
				enum forcewake_domains fw_domains)
{
	unsigned long irqflags;

	if (!dev_priv->uncore.funcs.force_wake_put)
		return;

	spin_lock_irqsave(&dev_priv->uncore.lock, irqflags);
	__intel_uncore_forcewake_put(dev_priv, fw_domains);
	spin_unlock_irqrestore(&dev_priv->uncore.lock, irqflags);
}

/**
 * intel_uncore_forcewake_put__locked - grab forcewake domain references
 * @dev_priv: i915 device instance
 * @fw_domains: forcewake domains to get reference on
 *
 * See intel_uncore_forcewake_put(). This variant places the onus
 * on the caller to explicitly handle the dev_priv->uncore.lock spinlock.
 */
void intel_uncore_forcewake_put__locked(struct drm_i915_private *dev_priv,
					enum forcewake_domains fw_domains)
{
	assert_spin_locked(&dev_priv->uncore.lock);

	if (!dev_priv->uncore.funcs.force_wake_put)
		return;

	__intel_uncore_forcewake_put(dev_priv, fw_domains);
}

void assert_forcewakes_inactive(struct drm_i915_private *dev_priv)
{
	struct intel_uncore_forcewake_domain *domain;
<<<<<<< HEAD
	enum forcewake_domain_id id;
=======
>>>>>>> v4.9.227

	if (!dev_priv->uncore.funcs.force_wake_get)
		return;

<<<<<<< HEAD
	for_each_fw_domain(domain, dev_priv, id)
=======
	for_each_fw_domain(domain, dev_priv)
>>>>>>> v4.9.227
		WARN_ON(domain->wake_count);
}

/* We give fast paths for the really cool registers */
<<<<<<< HEAD
#define NEEDS_FORCE_WAKE(reg) \
	 ((reg) < 0x40000 && (reg) != FORCEWAKE)
=======
#define NEEDS_FORCE_WAKE(reg) ((reg) < 0x40000)

#define __gen6_reg_read_fw_domains(offset) \
({ \
	enum forcewake_domains __fwd; \
	if (NEEDS_FORCE_WAKE(offset)) \
		__fwd = FORCEWAKE_RENDER; \
	else \
		__fwd = 0; \
	__fwd; \
})
>>>>>>> v4.9.227

#define REG_RANGE(reg, start, end) ((reg) >= (start) && (reg) < (end))

#define FORCEWAKE_VLV_RENDER_RANGE_OFFSET(reg) \
	(REG_RANGE((reg), 0x2000, 0x4000) || \
	 REG_RANGE((reg), 0x5000, 0x8000) || \
	 REG_RANGE((reg), 0xB000, 0x12000) || \
	 REG_RANGE((reg), 0x2E000, 0x30000))

#define FORCEWAKE_VLV_MEDIA_RANGE_OFFSET(reg) \
	(REG_RANGE((reg), 0x12000, 0x14000) || \
	 REG_RANGE((reg), 0x22000, 0x24000) || \
	 REG_RANGE((reg), 0x30000, 0x40000))

<<<<<<< HEAD
=======
#define __vlv_reg_read_fw_domains(offset) \
({ \
	enum forcewake_domains __fwd = 0; \
	if (!NEEDS_FORCE_WAKE(offset)) \
		__fwd = 0; \
	else if (FORCEWAKE_VLV_RENDER_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_RENDER; \
	else if (FORCEWAKE_VLV_MEDIA_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_MEDIA; \
	__fwd; \
})

static const i915_reg_t gen8_shadowed_regs[] = {
	GEN6_RPNSWREQ,
	GEN6_RC_VIDEO_FREQ,
	RING_TAIL(RENDER_RING_BASE),
	RING_TAIL(GEN6_BSD_RING_BASE),
	RING_TAIL(VEBOX_RING_BASE),
	RING_TAIL(BLT_RING_BASE),
	/* TODO: Other registers are not yet used */
};

static bool is_gen8_shadowed(u32 offset)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(gen8_shadowed_regs); i++)
		if (offset == gen8_shadowed_regs[i].reg)
			return true;

	return false;
}

#define __gen8_reg_write_fw_domains(offset) \
({ \
	enum forcewake_domains __fwd; \
	if (NEEDS_FORCE_WAKE(offset) && !is_gen8_shadowed(offset)) \
		__fwd = FORCEWAKE_RENDER; \
	else \
		__fwd = 0; \
	__fwd; \
})

>>>>>>> v4.9.227
#define FORCEWAKE_CHV_RENDER_RANGE_OFFSET(reg) \
	(REG_RANGE((reg), 0x2000, 0x4000) || \
	 REG_RANGE((reg), 0x5200, 0x8000) || \
	 REG_RANGE((reg), 0x8300, 0x8500) || \
	 REG_RANGE((reg), 0xB000, 0xB480) || \
	 REG_RANGE((reg), 0xE000, 0xE800))

#define FORCEWAKE_CHV_MEDIA_RANGE_OFFSET(reg) \
	(REG_RANGE((reg), 0x8800, 0x8900) || \
	 REG_RANGE((reg), 0xD000, 0xD800) || \
	 REG_RANGE((reg), 0x12000, 0x14000) || \
	 REG_RANGE((reg), 0x1A000, 0x1C000) || \
	 REG_RANGE((reg), 0x1E800, 0x1EA00) || \
	 REG_RANGE((reg), 0x30000, 0x38000))

#define FORCEWAKE_CHV_COMMON_RANGE_OFFSET(reg) \
	(REG_RANGE((reg), 0x4000, 0x5000) || \
	 REG_RANGE((reg), 0x8000, 0x8300) || \
	 REG_RANGE((reg), 0x8500, 0x8600) || \
	 REG_RANGE((reg), 0x9000, 0xB000) || \
	 REG_RANGE((reg), 0xF000, 0x10000))

<<<<<<< HEAD
=======
#define __chv_reg_read_fw_domains(offset) \
({ \
	enum forcewake_domains __fwd = 0; \
	if (!NEEDS_FORCE_WAKE(offset)) \
		__fwd = 0; \
	else if (FORCEWAKE_CHV_RENDER_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_RENDER; \
	else if (FORCEWAKE_CHV_MEDIA_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_MEDIA; \
	else if (FORCEWAKE_CHV_COMMON_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_RENDER | FORCEWAKE_MEDIA; \
	__fwd; \
})

#define __chv_reg_write_fw_domains(offset) \
({ \
	enum forcewake_domains __fwd = 0; \
	if (!NEEDS_FORCE_WAKE(offset) || is_gen8_shadowed(offset)) \
		__fwd = 0; \
	else if (FORCEWAKE_CHV_RENDER_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_RENDER; \
	else if (FORCEWAKE_CHV_MEDIA_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_MEDIA; \
	else if (FORCEWAKE_CHV_COMMON_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_RENDER | FORCEWAKE_MEDIA; \
	__fwd; \
})

>>>>>>> v4.9.227
#define FORCEWAKE_GEN9_UNCORE_RANGE_OFFSET(reg) \
	REG_RANGE((reg), 0xB00,  0x2000)

#define FORCEWAKE_GEN9_RENDER_RANGE_OFFSET(reg) \
	(REG_RANGE((reg), 0x2000, 0x2700) || \
	 REG_RANGE((reg), 0x3000, 0x4000) || \
	 REG_RANGE((reg), 0x5200, 0x8000) || \
	 REG_RANGE((reg), 0x8140, 0x8160) || \
	 REG_RANGE((reg), 0x8300, 0x8500) || \
	 REG_RANGE((reg), 0x8C00, 0x8D00) || \
	 REG_RANGE((reg), 0xB000, 0xB480) || \
	 REG_RANGE((reg), 0xE000, 0xE900) || \
	 REG_RANGE((reg), 0x24400, 0x24800))

#define FORCEWAKE_GEN9_MEDIA_RANGE_OFFSET(reg) \
	(REG_RANGE((reg), 0x8130, 0x8140) || \
	 REG_RANGE((reg), 0x8800, 0x8A00) || \
	 REG_RANGE((reg), 0xD000, 0xD800) || \
	 REG_RANGE((reg), 0x12000, 0x14000) || \
	 REG_RANGE((reg), 0x1A000, 0x1EA00) || \
	 REG_RANGE((reg), 0x30000, 0x40000))

#define FORCEWAKE_GEN9_COMMON_RANGE_OFFSET(reg) \
	REG_RANGE((reg), 0x9400, 0x9800)

#define FORCEWAKE_GEN9_BLITTER_RANGE_OFFSET(reg) \
<<<<<<< HEAD
	((reg) < 0x40000 &&\
=======
	((reg) < 0x40000 && \
>>>>>>> v4.9.227
	 !FORCEWAKE_GEN9_UNCORE_RANGE_OFFSET(reg) && \
	 !FORCEWAKE_GEN9_RENDER_RANGE_OFFSET(reg) && \
	 !FORCEWAKE_GEN9_MEDIA_RANGE_OFFSET(reg) && \
	 !FORCEWAKE_GEN9_COMMON_RANGE_OFFSET(reg))

<<<<<<< HEAD
=======
#define SKL_NEEDS_FORCE_WAKE(reg) \
	((reg) < 0x40000 && !FORCEWAKE_GEN9_UNCORE_RANGE_OFFSET(reg))

#define __gen9_reg_read_fw_domains(offset) \
({ \
	enum forcewake_domains __fwd; \
	if (!SKL_NEEDS_FORCE_WAKE(offset)) \
		__fwd = 0; \
	else if (FORCEWAKE_GEN9_RENDER_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_RENDER; \
	else if (FORCEWAKE_GEN9_MEDIA_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_MEDIA; \
	else if (FORCEWAKE_GEN9_COMMON_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_RENDER | FORCEWAKE_MEDIA; \
	else \
		__fwd = FORCEWAKE_BLITTER; \
	__fwd; \
})

static const i915_reg_t gen9_shadowed_regs[] = {
	RING_TAIL(RENDER_RING_BASE),
	RING_TAIL(GEN6_BSD_RING_BASE),
	RING_TAIL(VEBOX_RING_BASE),
	RING_TAIL(BLT_RING_BASE),
	GEN6_RPNSWREQ,
	GEN6_RC_VIDEO_FREQ,
	/* TODO: Other registers are not yet used */
};

static bool is_gen9_shadowed(u32 offset)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(gen9_shadowed_regs); i++)
		if (offset == gen9_shadowed_regs[i].reg)
			return true;

	return false;
}

#define __gen9_reg_write_fw_domains(offset) \
({ \
	enum forcewake_domains __fwd; \
	if (!SKL_NEEDS_FORCE_WAKE(offset) || is_gen9_shadowed(offset)) \
		__fwd = 0; \
	else if (FORCEWAKE_GEN9_RENDER_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_RENDER; \
	else if (FORCEWAKE_GEN9_MEDIA_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_MEDIA; \
	else if (FORCEWAKE_GEN9_COMMON_RANGE_OFFSET(offset)) \
		__fwd = FORCEWAKE_RENDER | FORCEWAKE_MEDIA; \
	else \
		__fwd = FORCEWAKE_BLITTER; \
	__fwd; \
})

>>>>>>> v4.9.227
static void
ilk_dummy_write(struct drm_i915_private *dev_priv)
{
	/* WaIssueDummyWriteToWakeupFromRC6:ilk Issue a dummy write to wake up
	 * the chip from rc6 before touching it for real. MI_MODE is masked,
	 * hence harmless to write 0 into. */
	__raw_i915_write32(dev_priv, MI_MODE, 0);
}

static void
<<<<<<< HEAD
hsw_unclaimed_reg_debug(struct drm_i915_private *dev_priv, u32 reg, bool read,
			bool before)
{
	const char *op = read ? "reading" : "writing to";
	const char *when = before ? "before" : "after";

	if (!i915.mmio_debug)
		return;

	if (__raw_i915_read32(dev_priv, FPGA_DBG) & FPGA_DBG_RM_NOCLAIM) {
		WARN(1, "Unclaimed register detected %s %s register 0x%x\n",
		     when, op, reg);
		__raw_i915_write32(dev_priv, FPGA_DBG, FPGA_DBG_RM_NOCLAIM);
		i915.mmio_debug--; /* Only report the first N failures */
	}
}

static void
hsw_unclaimed_reg_detect(struct drm_i915_private *dev_priv)
{
	static bool mmio_debug_once = true;

	if (i915.mmio_debug || !mmio_debug_once)
		return;

	if (__raw_i915_read32(dev_priv, FPGA_DBG) & FPGA_DBG_RM_NOCLAIM) {
		DRM_DEBUG("Unclaimed register detected, "
			  "enabling oneshot unclaimed register reporting. "
			  "Please use i915.mmio_debug=N for more information.\n");
		__raw_i915_write32(dev_priv, FPGA_DBG, FPGA_DBG_RM_NOCLAIM);
		i915.mmio_debug = mmio_debug_once--;
	}
=======
__unclaimed_reg_debug(struct drm_i915_private *dev_priv,
		      const i915_reg_t reg,
		      const bool read,
		      const bool before)
{
	if (WARN(check_for_unclaimed_mmio(dev_priv) && !before,
		 "Unclaimed %s register 0x%x\n",
		 read ? "read from" : "write to",
		 i915_mmio_reg_offset(reg)))
		i915.mmio_debug--; /* Only report the first N failures */
}

static inline void
unclaimed_reg_debug(struct drm_i915_private *dev_priv,
		    const i915_reg_t reg,
		    const bool read,
		    const bool before)
{
	if (likely(!i915.mmio_debug))
		return;

	__unclaimed_reg_debug(dev_priv, reg, read, before);
>>>>>>> v4.9.227
}

#define GEN2_READ_HEADER(x) \
	u##x val = 0; \
<<<<<<< HEAD
	assert_device_not_suspended(dev_priv);
=======
	assert_rpm_wakelock_held(dev_priv);
>>>>>>> v4.9.227

#define GEN2_READ_FOOTER \
	trace_i915_reg_rw(false, reg, val, sizeof(val), trace); \
	return val

#define __gen2_read(x) \
static u##x \
<<<<<<< HEAD
gen2_read##x(struct drm_i915_private *dev_priv, off_t reg, bool trace) { \
=======
gen2_read##x(struct drm_i915_private *dev_priv, i915_reg_t reg, bool trace) { \
>>>>>>> v4.9.227
	GEN2_READ_HEADER(x); \
	val = __raw_i915_read##x(dev_priv, reg); \
	GEN2_READ_FOOTER; \
}

#define __gen5_read(x) \
static u##x \
<<<<<<< HEAD
gen5_read##x(struct drm_i915_private *dev_priv, off_t reg, bool trace) { \
=======
gen5_read##x(struct drm_i915_private *dev_priv, i915_reg_t reg, bool trace) { \
>>>>>>> v4.9.227
	GEN2_READ_HEADER(x); \
	ilk_dummy_write(dev_priv); \
	val = __raw_i915_read##x(dev_priv, reg); \
	GEN2_READ_FOOTER; \
}

__gen5_read(8)
__gen5_read(16)
__gen5_read(32)
__gen5_read(64)
__gen2_read(8)
__gen2_read(16)
__gen2_read(32)
__gen2_read(64)

#undef __gen5_read
#undef __gen2_read

#undef GEN2_READ_FOOTER
#undef GEN2_READ_HEADER

#define GEN6_READ_HEADER(x) \
<<<<<<< HEAD
	unsigned long irqflags; \
	u##x val = 0; \
	assert_device_not_suspended(dev_priv); \
	spin_lock_irqsave(&dev_priv->uncore.lock, irqflags)

#define GEN6_READ_FOOTER \
=======
	u32 offset = i915_mmio_reg_offset(reg); \
	unsigned long irqflags; \
	u##x val = 0; \
	assert_rpm_wakelock_held(dev_priv); \
	spin_lock_irqsave(&dev_priv->uncore.lock, irqflags); \
	unclaimed_reg_debug(dev_priv, reg, true, true)

#define GEN6_READ_FOOTER \
	unclaimed_reg_debug(dev_priv, reg, true, false); \
>>>>>>> v4.9.227
	spin_unlock_irqrestore(&dev_priv->uncore.lock, irqflags); \
	trace_i915_reg_rw(false, reg, val, sizeof(val), trace); \
	return val

<<<<<<< HEAD
static inline void __force_wake_get(struct drm_i915_private *dev_priv,
				    enum forcewake_domains fw_domains)
{
	struct intel_uncore_forcewake_domain *domain;
	enum forcewake_domain_id id;
=======
static inline void __force_wake_auto(struct drm_i915_private *dev_priv,
				     enum forcewake_domains fw_domains)
{
	struct intel_uncore_forcewake_domain *domain;
>>>>>>> v4.9.227

	if (WARN_ON(!fw_domains))
		return;

	/* Ideally GCC would be constant-fold and eliminate this loop */
<<<<<<< HEAD
	for_each_fw_domain_mask(domain, fw_domains, dev_priv, id) {
		if (domain->wake_count) {
			fw_domains &= ~(1 << id);
			continue;
		}

		domain->wake_count++;
=======
	for_each_fw_domain_masked(domain, fw_domains, dev_priv) {
		if (domain->wake_count) {
			fw_domains &= ~domain->mask;
			continue;
		}

>>>>>>> v4.9.227
		fw_domain_arm_timer(domain);
	}

	if (fw_domains)
		dev_priv->uncore.funcs.force_wake_get(dev_priv, fw_domains);
}

<<<<<<< HEAD
#define __vgpu_read(x) \
static u##x \
vgpu_read##x(struct drm_i915_private *dev_priv, off_t reg, bool trace) { \
	GEN6_READ_HEADER(x); \
	val = __raw_i915_read##x(dev_priv, reg); \
	GEN6_READ_FOOTER; \
}

#define __gen6_read(x) \
static u##x \
gen6_read##x(struct drm_i915_private *dev_priv, off_t reg, bool trace) { \
	GEN6_READ_HEADER(x); \
	hsw_unclaimed_reg_debug(dev_priv, reg, true, true); \
	if (NEEDS_FORCE_WAKE(reg)) \
		__force_wake_get(dev_priv, FORCEWAKE_RENDER); \
	val = __raw_i915_read##x(dev_priv, reg); \
	hsw_unclaimed_reg_debug(dev_priv, reg, true, false); \
=======
#define __gen6_read(x) \
static u##x \
gen6_read##x(struct drm_i915_private *dev_priv, i915_reg_t reg, bool trace) { \
	enum forcewake_domains fw_engine; \
	GEN6_READ_HEADER(x); \
	fw_engine = __gen6_reg_read_fw_domains(offset); \
	if (fw_engine) \
		__force_wake_auto(dev_priv, fw_engine); \
	val = __raw_i915_read##x(dev_priv, reg); \
>>>>>>> v4.9.227
	GEN6_READ_FOOTER; \
}

#define __vlv_read(x) \
static u##x \
<<<<<<< HEAD
vlv_read##x(struct drm_i915_private *dev_priv, off_t reg, bool trace) { \
	GEN6_READ_HEADER(x); \
	if (FORCEWAKE_VLV_RENDER_RANGE_OFFSET(reg)) \
		__force_wake_get(dev_priv, FORCEWAKE_RENDER); \
	else if (FORCEWAKE_VLV_MEDIA_RANGE_OFFSET(reg)) \
		__force_wake_get(dev_priv, FORCEWAKE_MEDIA); \
=======
vlv_read##x(struct drm_i915_private *dev_priv, i915_reg_t reg, bool trace) { \
	enum forcewake_domains fw_engine; \
	GEN6_READ_HEADER(x); \
	fw_engine = __vlv_reg_read_fw_domains(offset); \
	if (fw_engine) \
		__force_wake_auto(dev_priv, fw_engine); \
>>>>>>> v4.9.227
	val = __raw_i915_read##x(dev_priv, reg); \
	GEN6_READ_FOOTER; \
}

#define __chv_read(x) \
static u##x \
<<<<<<< HEAD
chv_read##x(struct drm_i915_private *dev_priv, off_t reg, bool trace) { \
	GEN6_READ_HEADER(x); \
	if (FORCEWAKE_CHV_RENDER_RANGE_OFFSET(reg)) \
		__force_wake_get(dev_priv, FORCEWAKE_RENDER); \
	else if (FORCEWAKE_CHV_MEDIA_RANGE_OFFSET(reg)) \
		__force_wake_get(dev_priv, FORCEWAKE_MEDIA); \
	else if (FORCEWAKE_CHV_COMMON_RANGE_OFFSET(reg)) \
		__force_wake_get(dev_priv, \
				 FORCEWAKE_RENDER | FORCEWAKE_MEDIA); \
=======
chv_read##x(struct drm_i915_private *dev_priv, i915_reg_t reg, bool trace) { \
	enum forcewake_domains fw_engine; \
	GEN6_READ_HEADER(x); \
	fw_engine = __chv_reg_read_fw_domains(offset); \
	if (fw_engine) \
		__force_wake_auto(dev_priv, fw_engine); \
>>>>>>> v4.9.227
	val = __raw_i915_read##x(dev_priv, reg); \
	GEN6_READ_FOOTER; \
}

<<<<<<< HEAD
#define SKL_NEEDS_FORCE_WAKE(reg) \
	 ((reg) < 0x40000 && !FORCEWAKE_GEN9_UNCORE_RANGE_OFFSET(reg))

#define __gen9_read(x) \
static u##x \
gen9_read##x(struct drm_i915_private *dev_priv, off_t reg, bool trace) { \
	enum forcewake_domains fw_engine; \
	GEN6_READ_HEADER(x); \
	hsw_unclaimed_reg_debug(dev_priv, reg, true, true); \
	if (!SKL_NEEDS_FORCE_WAKE(reg)) \
		fw_engine = 0; \
	else if (FORCEWAKE_GEN9_RENDER_RANGE_OFFSET(reg)) \
		fw_engine = FORCEWAKE_RENDER; \
	else if (FORCEWAKE_GEN9_MEDIA_RANGE_OFFSET(reg)) \
		fw_engine = FORCEWAKE_MEDIA; \
	else if (FORCEWAKE_GEN9_COMMON_RANGE_OFFSET(reg)) \
		fw_engine = FORCEWAKE_RENDER | FORCEWAKE_MEDIA; \
	else \
		fw_engine = FORCEWAKE_BLITTER; \
	if (fw_engine) \
		__force_wake_get(dev_priv, fw_engine); \
	val = __raw_i915_read##x(dev_priv, reg); \
	hsw_unclaimed_reg_debug(dev_priv, reg, true, false); \
	GEN6_READ_FOOTER; \
}

__vgpu_read(8)
__vgpu_read(16)
__vgpu_read(32)
__vgpu_read(64)
=======
#define __gen9_read(x) \
static u##x \
gen9_read##x(struct drm_i915_private *dev_priv, i915_reg_t reg, bool trace) { \
	enum forcewake_domains fw_engine; \
	GEN6_READ_HEADER(x); \
	fw_engine = __gen9_reg_read_fw_domains(offset); \
	if (fw_engine) \
		__force_wake_auto(dev_priv, fw_engine); \
	val = __raw_i915_read##x(dev_priv, reg); \
	GEN6_READ_FOOTER; \
}

>>>>>>> v4.9.227
__gen9_read(8)
__gen9_read(16)
__gen9_read(32)
__gen9_read(64)
__chv_read(8)
__chv_read(16)
__chv_read(32)
__chv_read(64)
__vlv_read(8)
__vlv_read(16)
__vlv_read(32)
__vlv_read(64)
__gen6_read(8)
__gen6_read(16)
__gen6_read(32)
__gen6_read(64)

#undef __gen9_read
#undef __chv_read
#undef __vlv_read
#undef __gen6_read
<<<<<<< HEAD
#undef __vgpu_read
#undef GEN6_READ_FOOTER
#undef GEN6_READ_HEADER

#define GEN2_WRITE_HEADER \
	trace_i915_reg_rw(true, reg, val, sizeof(val), trace); \
	assert_device_not_suspended(dev_priv); \
=======
#undef GEN6_READ_FOOTER
#undef GEN6_READ_HEADER

#define VGPU_READ_HEADER(x) \
	unsigned long irqflags; \
	u##x val = 0; \
	assert_rpm_device_not_suspended(dev_priv); \
	spin_lock_irqsave(&dev_priv->uncore.lock, irqflags)

#define VGPU_READ_FOOTER \
	spin_unlock_irqrestore(&dev_priv->uncore.lock, irqflags); \
	trace_i915_reg_rw(false, reg, val, sizeof(val), trace); \
	return val

#define __vgpu_read(x) \
static u##x \
vgpu_read##x(struct drm_i915_private *dev_priv, i915_reg_t reg, bool trace) { \
	VGPU_READ_HEADER(x); \
	val = __raw_i915_read##x(dev_priv, reg); \
	VGPU_READ_FOOTER; \
}

__vgpu_read(8)
__vgpu_read(16)
__vgpu_read(32)
__vgpu_read(64)

#undef __vgpu_read
#undef VGPU_READ_FOOTER
#undef VGPU_READ_HEADER

#define GEN2_WRITE_HEADER \
	trace_i915_reg_rw(true, reg, val, sizeof(val), trace); \
	assert_rpm_wakelock_held(dev_priv); \
>>>>>>> v4.9.227

#define GEN2_WRITE_FOOTER

#define __gen2_write(x) \
static void \
<<<<<<< HEAD
gen2_write##x(struct drm_i915_private *dev_priv, off_t reg, u##x val, bool trace) { \
=======
gen2_write##x(struct drm_i915_private *dev_priv, i915_reg_t reg, u##x val, bool trace) { \
>>>>>>> v4.9.227
	GEN2_WRITE_HEADER; \
	__raw_i915_write##x(dev_priv, reg, val); \
	GEN2_WRITE_FOOTER; \
}

#define __gen5_write(x) \
static void \
<<<<<<< HEAD
gen5_write##x(struct drm_i915_private *dev_priv, off_t reg, u##x val, bool trace) { \
=======
gen5_write##x(struct drm_i915_private *dev_priv, i915_reg_t reg, u##x val, bool trace) { \
>>>>>>> v4.9.227
	GEN2_WRITE_HEADER; \
	ilk_dummy_write(dev_priv); \
	__raw_i915_write##x(dev_priv, reg, val); \
	GEN2_WRITE_FOOTER; \
}

__gen5_write(8)
__gen5_write(16)
__gen5_write(32)
<<<<<<< HEAD
__gen5_write(64)
__gen2_write(8)
__gen2_write(16)
__gen2_write(32)
__gen2_write(64)
=======
__gen2_write(8)
__gen2_write(16)
__gen2_write(32)
>>>>>>> v4.9.227

#undef __gen5_write
#undef __gen2_write

#undef GEN2_WRITE_FOOTER
#undef GEN2_WRITE_HEADER

#define GEN6_WRITE_HEADER \
<<<<<<< HEAD
	unsigned long irqflags; \
	trace_i915_reg_rw(true, reg, val, sizeof(val), trace); \
	assert_device_not_suspended(dev_priv); \
	spin_lock_irqsave(&dev_priv->uncore.lock, irqflags)

#define GEN6_WRITE_FOOTER \
=======
	u32 offset = i915_mmio_reg_offset(reg); \
	unsigned long irqflags; \
	trace_i915_reg_rw(true, reg, val, sizeof(val), trace); \
	assert_rpm_wakelock_held(dev_priv); \
	spin_lock_irqsave(&dev_priv->uncore.lock, irqflags); \
	unclaimed_reg_debug(dev_priv, reg, false, true)

#define GEN6_WRITE_FOOTER \
	unclaimed_reg_debug(dev_priv, reg, false, false); \
>>>>>>> v4.9.227
	spin_unlock_irqrestore(&dev_priv->uncore.lock, irqflags)

#define __gen6_write(x) \
static void \
<<<<<<< HEAD
gen6_write##x(struct drm_i915_private *dev_priv, off_t reg, u##x val, bool trace) { \
	u32 __fifo_ret = 0; \
	GEN6_WRITE_HEADER; \
	if (NEEDS_FORCE_WAKE(reg)) { \
=======
gen6_write##x(struct drm_i915_private *dev_priv, i915_reg_t reg, u##x val, bool trace) { \
	u32 __fifo_ret = 0; \
	GEN6_WRITE_HEADER; \
	if (NEEDS_FORCE_WAKE(offset)) { \
>>>>>>> v4.9.227
		__fifo_ret = __gen6_gt_wait_for_fifo(dev_priv); \
	} \
	__raw_i915_write##x(dev_priv, reg, val); \
	if (unlikely(__fifo_ret)) { \
		gen6_gt_check_fifodbg(dev_priv); \
	} \
	GEN6_WRITE_FOOTER; \
}

#define __hsw_write(x) \
static void \
<<<<<<< HEAD
hsw_write##x(struct drm_i915_private *dev_priv, off_t reg, u##x val, bool trace) { \
	u32 __fifo_ret = 0; \
	GEN6_WRITE_HEADER; \
	if (NEEDS_FORCE_WAKE(reg)) { \
		__fifo_ret = __gen6_gt_wait_for_fifo(dev_priv); \
	} \
	hsw_unclaimed_reg_debug(dev_priv, reg, false, true); \
=======
hsw_write##x(struct drm_i915_private *dev_priv, i915_reg_t reg, u##x val, bool trace) { \
	u32 __fifo_ret = 0; \
	GEN6_WRITE_HEADER; \
	if (NEEDS_FORCE_WAKE(offset)) { \
		__fifo_ret = __gen6_gt_wait_for_fifo(dev_priv); \
	} \
>>>>>>> v4.9.227
	__raw_i915_write##x(dev_priv, reg, val); \
	if (unlikely(__fifo_ret)) { \
		gen6_gt_check_fifodbg(dev_priv); \
	} \
<<<<<<< HEAD
	hsw_unclaimed_reg_debug(dev_priv, reg, false, false); \
	hsw_unclaimed_reg_detect(dev_priv); \
	GEN6_WRITE_FOOTER; \
}

#define __vgpu_write(x) \
static void vgpu_write##x(struct drm_i915_private *dev_priv, \
			  off_t reg, u##x val, bool trace) { \
	GEN6_WRITE_HEADER; \
	__raw_i915_write##x(dev_priv, reg, val); \
	GEN6_WRITE_FOOTER; \
}

static const u32 gen8_shadowed_regs[] = {
	FORCEWAKE_MT,
	GEN6_RPNSWREQ,
	GEN6_RC_VIDEO_FREQ,
	RING_TAIL(RENDER_RING_BASE),
	RING_TAIL(GEN6_BSD_RING_BASE),
	RING_TAIL(VEBOX_RING_BASE),
	RING_TAIL(BLT_RING_BASE),
	/* TODO: Other registers are not yet used */
};

static bool is_gen8_shadowed(struct drm_i915_private *dev_priv, u32 reg)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(gen8_shadowed_regs); i++)
		if (reg == gen8_shadowed_regs[i])
			return true;

	return false;
}

#define __gen8_write(x) \
static void \
gen8_write##x(struct drm_i915_private *dev_priv, off_t reg, u##x val, bool trace) { \
	GEN6_WRITE_HEADER; \
	hsw_unclaimed_reg_debug(dev_priv, reg, false, true); \
	if (reg < 0x40000 && !is_gen8_shadowed(dev_priv, reg)) \
		__force_wake_get(dev_priv, FORCEWAKE_RENDER); \
	__raw_i915_write##x(dev_priv, reg, val); \
	hsw_unclaimed_reg_debug(dev_priv, reg, false, false); \
	hsw_unclaimed_reg_detect(dev_priv); \
=======
	GEN6_WRITE_FOOTER; \
}

#define __gen8_write(x) \
static void \
gen8_write##x(struct drm_i915_private *dev_priv, i915_reg_t reg, u##x val, bool trace) { \
	enum forcewake_domains fw_engine; \
	GEN6_WRITE_HEADER; \
	fw_engine = __gen8_reg_write_fw_domains(offset); \
	if (fw_engine) \
		__force_wake_auto(dev_priv, fw_engine); \
	__raw_i915_write##x(dev_priv, reg, val); \
>>>>>>> v4.9.227
	GEN6_WRITE_FOOTER; \
}

#define __chv_write(x) \
static void \
<<<<<<< HEAD
chv_write##x(struct drm_i915_private *dev_priv, off_t reg, u##x val, bool trace) { \
	bool shadowed = is_gen8_shadowed(dev_priv, reg); \
	GEN6_WRITE_HEADER; \
	if (!shadowed) { \
		if (FORCEWAKE_CHV_RENDER_RANGE_OFFSET(reg)) \
			__force_wake_get(dev_priv, FORCEWAKE_RENDER); \
		else if (FORCEWAKE_CHV_MEDIA_RANGE_OFFSET(reg)) \
			__force_wake_get(dev_priv, FORCEWAKE_MEDIA); \
		else if (FORCEWAKE_CHV_COMMON_RANGE_OFFSET(reg)) \
			__force_wake_get(dev_priv, FORCEWAKE_RENDER | FORCEWAKE_MEDIA); \
	} \
=======
chv_write##x(struct drm_i915_private *dev_priv, i915_reg_t reg, u##x val, bool trace) { \
	enum forcewake_domains fw_engine; \
	GEN6_WRITE_HEADER; \
	fw_engine = __chv_reg_write_fw_domains(offset); \
	if (fw_engine) \
		__force_wake_auto(dev_priv, fw_engine); \
>>>>>>> v4.9.227
	__raw_i915_write##x(dev_priv, reg, val); \
	GEN6_WRITE_FOOTER; \
}

<<<<<<< HEAD
static const u32 gen9_shadowed_regs[] = {
	RING_TAIL(RENDER_RING_BASE),
	RING_TAIL(GEN6_BSD_RING_BASE),
	RING_TAIL(VEBOX_RING_BASE),
	RING_TAIL(BLT_RING_BASE),
	FORCEWAKE_BLITTER_GEN9,
	FORCEWAKE_RENDER_GEN9,
	FORCEWAKE_MEDIA_GEN9,
	GEN6_RPNSWREQ,
	GEN6_RC_VIDEO_FREQ,
	/* TODO: Other registers are not yet used */
};

static bool is_gen9_shadowed(struct drm_i915_private *dev_priv, u32 reg)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(gen9_shadowed_regs); i++)
		if (reg == gen9_shadowed_regs[i])
			return true;

	return false;
}

#define __gen9_write(x) \
static void \
gen9_write##x(struct drm_i915_private *dev_priv, off_t reg, u##x val, \
		bool trace) { \
	enum forcewake_domains fw_engine; \
	GEN6_WRITE_HEADER; \
	hsw_unclaimed_reg_debug(dev_priv, reg, false, true); \
	if (!SKL_NEEDS_FORCE_WAKE(reg) || \
	    is_gen9_shadowed(dev_priv, reg)) \
		fw_engine = 0; \
	else if (FORCEWAKE_GEN9_RENDER_RANGE_OFFSET(reg)) \
		fw_engine = FORCEWAKE_RENDER; \
	else if (FORCEWAKE_GEN9_MEDIA_RANGE_OFFSET(reg)) \
		fw_engine = FORCEWAKE_MEDIA; \
	else if (FORCEWAKE_GEN9_COMMON_RANGE_OFFSET(reg)) \
		fw_engine = FORCEWAKE_RENDER | FORCEWAKE_MEDIA; \
	else \
		fw_engine = FORCEWAKE_BLITTER; \
	if (fw_engine) \
		__force_wake_get(dev_priv, fw_engine); \
	__raw_i915_write##x(dev_priv, reg, val); \
	hsw_unclaimed_reg_debug(dev_priv, reg, false, false); \
	hsw_unclaimed_reg_detect(dev_priv); \
=======
#define __gen9_write(x) \
static void \
gen9_write##x(struct drm_i915_private *dev_priv, i915_reg_t reg, u##x val, \
		bool trace) { \
	enum forcewake_domains fw_engine; \
	GEN6_WRITE_HEADER; \
	fw_engine = __gen9_reg_write_fw_domains(offset); \
	if (fw_engine) \
		__force_wake_auto(dev_priv, fw_engine); \
	__raw_i915_write##x(dev_priv, reg, val); \
>>>>>>> v4.9.227
	GEN6_WRITE_FOOTER; \
}

__gen9_write(8)
__gen9_write(16)
__gen9_write(32)
<<<<<<< HEAD
__gen9_write(64)
__chv_write(8)
__chv_write(16)
__chv_write(32)
__chv_write(64)
__gen8_write(8)
__gen8_write(16)
__gen8_write(32)
__gen8_write(64)
__hsw_write(8)
__hsw_write(16)
__hsw_write(32)
__hsw_write(64)
__gen6_write(8)
__gen6_write(16)
__gen6_write(32)
__gen6_write(64)
__vgpu_write(8)
__vgpu_write(16)
__vgpu_write(32)
__vgpu_write(64)
=======
__chv_write(8)
__chv_write(16)
__chv_write(32)
__gen8_write(8)
__gen8_write(16)
__gen8_write(32)
__hsw_write(8)
__hsw_write(16)
__hsw_write(32)
__gen6_write(8)
__gen6_write(16)
__gen6_write(32)
>>>>>>> v4.9.227

#undef __gen9_write
#undef __chv_write
#undef __gen8_write
#undef __hsw_write
#undef __gen6_write
<<<<<<< HEAD
#undef __vgpu_write
#undef GEN6_WRITE_FOOTER
#undef GEN6_WRITE_HEADER

=======
#undef GEN6_WRITE_FOOTER
#undef GEN6_WRITE_HEADER

#define VGPU_WRITE_HEADER \
	unsigned long irqflags; \
	trace_i915_reg_rw(true, reg, val, sizeof(val), trace); \
	assert_rpm_device_not_suspended(dev_priv); \
	spin_lock_irqsave(&dev_priv->uncore.lock, irqflags)

#define VGPU_WRITE_FOOTER \
	spin_unlock_irqrestore(&dev_priv->uncore.lock, irqflags)

#define __vgpu_write(x) \
static void vgpu_write##x(struct drm_i915_private *dev_priv, \
			  i915_reg_t reg, u##x val, bool trace) { \
	VGPU_WRITE_HEADER; \
	__raw_i915_write##x(dev_priv, reg, val); \
	VGPU_WRITE_FOOTER; \
}

__vgpu_write(8)
__vgpu_write(16)
__vgpu_write(32)

#undef __vgpu_write
#undef VGPU_WRITE_FOOTER
#undef VGPU_WRITE_HEADER

>>>>>>> v4.9.227
#define ASSIGN_WRITE_MMIO_VFUNCS(x) \
do { \
	dev_priv->uncore.funcs.mmio_writeb = x##_write8; \
	dev_priv->uncore.funcs.mmio_writew = x##_write16; \
	dev_priv->uncore.funcs.mmio_writel = x##_write32; \
<<<<<<< HEAD
	dev_priv->uncore.funcs.mmio_writeq = x##_write64; \
=======
>>>>>>> v4.9.227
} while (0)

#define ASSIGN_READ_MMIO_VFUNCS(x) \
do { \
	dev_priv->uncore.funcs.mmio_readb = x##_read8; \
	dev_priv->uncore.funcs.mmio_readw = x##_read16; \
	dev_priv->uncore.funcs.mmio_readl = x##_read32; \
	dev_priv->uncore.funcs.mmio_readq = x##_read64; \
} while (0)


static void fw_domain_init(struct drm_i915_private *dev_priv,
			   enum forcewake_domain_id domain_id,
<<<<<<< HEAD
			   u32 reg_set, u32 reg_ack)
=======
			   i915_reg_t reg_set,
			   i915_reg_t reg_ack)
>>>>>>> v4.9.227
{
	struct intel_uncore_forcewake_domain *d;

	if (WARN_ON(domain_id >= FW_DOMAIN_ID_COUNT))
		return;

	d = &dev_priv->uncore.fw_domain[domain_id];

	WARN_ON(d->wake_count);

	d->wake_count = 0;
	d->reg_set = reg_set;
	d->reg_ack = reg_ack;

	if (IS_GEN6(dev_priv)) {
		d->val_reset = 0;
		d->val_set = FORCEWAKE_KERNEL;
		d->val_clear = 0;
	} else {
		/* WaRsClearFWBitsAtReset:bdw,skl */
		d->val_reset = _MASKED_BIT_DISABLE(0xffff);
		d->val_set = _MASKED_BIT_ENABLE(FORCEWAKE_KERNEL);
		d->val_clear = _MASKED_BIT_DISABLE(FORCEWAKE_KERNEL);
	}

<<<<<<< HEAD
	if (IS_VALLEYVIEW(dev_priv))
		d->reg_post = FORCEWAKE_ACK_VLV;
	else if (IS_GEN6(dev_priv) || IS_GEN7(dev_priv) || IS_GEN8(dev_priv))
		d->reg_post = ECOBUS;
	else
		d->reg_post = 0;
=======
	if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv))
		d->reg_post = FORCEWAKE_ACK_VLV;
	else if (IS_GEN6(dev_priv) || IS_GEN7(dev_priv) || IS_GEN8(dev_priv))
		d->reg_post = ECOBUS;
>>>>>>> v4.9.227

	d->i915 = dev_priv;
	d->id = domain_id;

<<<<<<< HEAD
	setup_timer(&d->timer, intel_uncore_fw_release_timer, (unsigned long)d);
=======
	BUILD_BUG_ON(FORCEWAKE_RENDER != (1 << FW_DOMAIN_ID_RENDER));
	BUILD_BUG_ON(FORCEWAKE_BLITTER != (1 << FW_DOMAIN_ID_BLITTER));
	BUILD_BUG_ON(FORCEWAKE_MEDIA != (1 << FW_DOMAIN_ID_MEDIA));

	d->mask = 1 << domain_id;

	hrtimer_init(&d->timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	d->timer.function = intel_uncore_fw_release_timer;
>>>>>>> v4.9.227

	dev_priv->uncore.fw_domains |= (1 << domain_id);

	fw_domain_reset(d);
}

<<<<<<< HEAD
static void intel_uncore_fw_domains_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;

	if (INTEL_INFO(dev_priv->dev)->gen <= 5)
		return;

	if (IS_GEN9(dev)) {
=======
static void intel_uncore_fw_domains_init(struct drm_i915_private *dev_priv)
{
	if (INTEL_INFO(dev_priv)->gen <= 5)
		return;

	if (IS_GEN9(dev_priv)) {
>>>>>>> v4.9.227
		dev_priv->uncore.funcs.force_wake_get = fw_domains_get;
		dev_priv->uncore.funcs.force_wake_put = fw_domains_put;
		fw_domain_init(dev_priv, FW_DOMAIN_ID_RENDER,
			       FORCEWAKE_RENDER_GEN9,
			       FORCEWAKE_ACK_RENDER_GEN9);
		fw_domain_init(dev_priv, FW_DOMAIN_ID_BLITTER,
			       FORCEWAKE_BLITTER_GEN9,
			       FORCEWAKE_ACK_BLITTER_GEN9);
		fw_domain_init(dev_priv, FW_DOMAIN_ID_MEDIA,
			       FORCEWAKE_MEDIA_GEN9, FORCEWAKE_ACK_MEDIA_GEN9);
<<<<<<< HEAD
	} else if (IS_VALLEYVIEW(dev)) {
		dev_priv->uncore.funcs.force_wake_get = fw_domains_get;
		if (!IS_CHERRYVIEW(dev))
=======
	} else if (IS_VALLEYVIEW(dev_priv) || IS_CHERRYVIEW(dev_priv)) {
		dev_priv->uncore.funcs.force_wake_get = fw_domains_get;
		if (!IS_CHERRYVIEW(dev_priv))
>>>>>>> v4.9.227
			dev_priv->uncore.funcs.force_wake_put =
				fw_domains_put_with_fifo;
		else
			dev_priv->uncore.funcs.force_wake_put = fw_domains_put;
		fw_domain_init(dev_priv, FW_DOMAIN_ID_RENDER,
			       FORCEWAKE_VLV, FORCEWAKE_ACK_VLV);
		fw_domain_init(dev_priv, FW_DOMAIN_ID_MEDIA,
			       FORCEWAKE_MEDIA_VLV, FORCEWAKE_ACK_MEDIA_VLV);
<<<<<<< HEAD
	} else if (IS_HASWELL(dev) || IS_BROADWELL(dev)) {
		dev_priv->uncore.funcs.force_wake_get =
			fw_domains_get_with_thread_status;
		if (IS_HASWELL(dev))
=======
	} else if (IS_HASWELL(dev_priv) || IS_BROADWELL(dev_priv)) {
		dev_priv->uncore.funcs.force_wake_get =
			fw_domains_get_with_thread_status;
		if (IS_HASWELL(dev_priv))
>>>>>>> v4.9.227
			dev_priv->uncore.funcs.force_wake_put =
				fw_domains_put_with_fifo;
		else
			dev_priv->uncore.funcs.force_wake_put = fw_domains_put;
		fw_domain_init(dev_priv, FW_DOMAIN_ID_RENDER,
			       FORCEWAKE_MT, FORCEWAKE_ACK_HSW);
<<<<<<< HEAD
	} else if (IS_IVYBRIDGE(dev)) {
=======
	} else if (IS_IVYBRIDGE(dev_priv)) {
>>>>>>> v4.9.227
		u32 ecobus;

		/* IVB configs may use multi-threaded forcewake */

		/* A small trick here - if the bios hasn't configured
		 * MT forcewake, and if the device is in RC6, then
		 * force_wake_mt_get will not wake the device and the
		 * ECOBUS read will return zero. Which will be
		 * (correctly) interpreted by the test below as MT
		 * forcewake being disabled.
		 */
		dev_priv->uncore.funcs.force_wake_get =
			fw_domains_get_with_thread_status;
		dev_priv->uncore.funcs.force_wake_put =
			fw_domains_put_with_fifo;

		/* We need to init first for ECOBUS access and then
		 * determine later if we want to reinit, in case of MT access is
		 * not working. In this stage we don't know which flavour this
		 * ivb is, so it is better to reset also the gen6 fw registers
		 * before the ecobus check.
		 */

		__raw_i915_write32(dev_priv, FORCEWAKE, 0);
		__raw_posting_read(dev_priv, ECOBUS);

		fw_domain_init(dev_priv, FW_DOMAIN_ID_RENDER,
			       FORCEWAKE_MT, FORCEWAKE_MT_ACK);

<<<<<<< HEAD
		mutex_lock(&dev->struct_mutex);
		fw_domains_get_with_thread_status(dev_priv, FORCEWAKE_ALL);
		ecobus = __raw_i915_read32(dev_priv, ECOBUS);
		fw_domains_put_with_fifo(dev_priv, FORCEWAKE_ALL);
		mutex_unlock(&dev->struct_mutex);
=======
		spin_lock_irq(&dev_priv->uncore.lock);
		fw_domains_get_with_thread_status(dev_priv, FORCEWAKE_ALL);
		ecobus = __raw_i915_read32(dev_priv, ECOBUS);
		fw_domains_put_with_fifo(dev_priv, FORCEWAKE_ALL);
		spin_unlock_irq(&dev_priv->uncore.lock);
>>>>>>> v4.9.227

		if (!(ecobus & FORCEWAKE_MT_ENABLE)) {
			DRM_INFO("No MT forcewake available on Ivybridge, this can result in issues\n");
			DRM_INFO("when using vblank-synced partial screen updates.\n");
			fw_domain_init(dev_priv, FW_DOMAIN_ID_RENDER,
				       FORCEWAKE, FORCEWAKE_ACK);
		}
<<<<<<< HEAD
	} else if (IS_GEN6(dev)) {
=======
	} else if (IS_GEN6(dev_priv)) {
>>>>>>> v4.9.227
		dev_priv->uncore.funcs.force_wake_get =
			fw_domains_get_with_thread_status;
		dev_priv->uncore.funcs.force_wake_put =
			fw_domains_put_with_fifo;
		fw_domain_init(dev_priv, FW_DOMAIN_ID_RENDER,
			       FORCEWAKE, FORCEWAKE_ACK);
	}

	/* All future platforms are expected to require complex power gating */
	WARN_ON(dev_priv->uncore.fw_domains == 0);
}

<<<<<<< HEAD
void intel_uncore_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;

	i915_check_vgpu(dev);

	intel_uncore_ellc_detect(dev);
	intel_uncore_fw_domains_init(dev);
	__intel_uncore_early_sanitize(dev, false);

	switch (INTEL_INFO(dev)->gen) {
=======
void intel_uncore_init(struct drm_i915_private *dev_priv)
{
	i915_check_vgpu(dev_priv);

	intel_uncore_edram_detect(dev_priv);
	intel_uncore_fw_domains_init(dev_priv);
	__intel_uncore_early_sanitize(dev_priv, false);

	dev_priv->uncore.unclaimed_mmio_check = 1;

	switch (INTEL_INFO(dev_priv)->gen) {
>>>>>>> v4.9.227
	default:
	case 9:
		ASSIGN_WRITE_MMIO_VFUNCS(gen9);
		ASSIGN_READ_MMIO_VFUNCS(gen9);
		break;
	case 8:
<<<<<<< HEAD
		if (IS_CHERRYVIEW(dev)) {
=======
		if (IS_CHERRYVIEW(dev_priv)) {
>>>>>>> v4.9.227
			ASSIGN_WRITE_MMIO_VFUNCS(chv);
			ASSIGN_READ_MMIO_VFUNCS(chv);

		} else {
			ASSIGN_WRITE_MMIO_VFUNCS(gen8);
			ASSIGN_READ_MMIO_VFUNCS(gen6);
		}
		break;
	case 7:
	case 6:
<<<<<<< HEAD
		if (IS_HASWELL(dev)) {
=======
		if (IS_HASWELL(dev_priv)) {
>>>>>>> v4.9.227
			ASSIGN_WRITE_MMIO_VFUNCS(hsw);
		} else {
			ASSIGN_WRITE_MMIO_VFUNCS(gen6);
		}

<<<<<<< HEAD
		if (IS_VALLEYVIEW(dev)) {
=======
		if (IS_VALLEYVIEW(dev_priv)) {
>>>>>>> v4.9.227
			ASSIGN_READ_MMIO_VFUNCS(vlv);
		} else {
			ASSIGN_READ_MMIO_VFUNCS(gen6);
		}
		break;
	case 5:
		ASSIGN_WRITE_MMIO_VFUNCS(gen5);
		ASSIGN_READ_MMIO_VFUNCS(gen5);
		break;
	case 4:
	case 3:
	case 2:
		ASSIGN_WRITE_MMIO_VFUNCS(gen2);
		ASSIGN_READ_MMIO_VFUNCS(gen2);
		break;
	}

<<<<<<< HEAD
	if (intel_vgpu_active(dev)) {
=======
	if (intel_vgpu_active(dev_priv)) {
>>>>>>> v4.9.227
		ASSIGN_WRITE_MMIO_VFUNCS(vgpu);
		ASSIGN_READ_MMIO_VFUNCS(vgpu);
	}

<<<<<<< HEAD
	i915_check_and_clear_faults(dev);
=======
	i915_check_and_clear_faults(dev_priv);
>>>>>>> v4.9.227
}
#undef ASSIGN_WRITE_MMIO_VFUNCS
#undef ASSIGN_READ_MMIO_VFUNCS

<<<<<<< HEAD
void intel_uncore_fini(struct drm_device *dev)
{
	/* Paranoia: make sure we have disabled everything before we exit. */
	intel_uncore_sanitize(dev);
	intel_uncore_forcewake_reset(dev, false);
}

#define GEN_RANGE(l, h) GENMASK(h, l)

static const struct register_whitelist {
	uint64_t offset;
=======
void intel_uncore_fini(struct drm_i915_private *dev_priv)
{
	/* Paranoia: make sure we have disabled everything before we exit. */
	intel_uncore_sanitize(dev_priv);
	intel_uncore_forcewake_reset(dev_priv, false);
}

#define GEN_RANGE(l, h) GENMASK((h) - 1, (l) - 1)

static const struct register_whitelist {
	i915_reg_t offset_ldw, offset_udw;
>>>>>>> v4.9.227
	uint32_t size;
	/* supported gens, 0x10 for 4, 0x30 for 4 and 5, etc. */
	uint32_t gen_bitmask;
} whitelist[] = {
<<<<<<< HEAD
	{ RING_TIMESTAMP(RENDER_RING_BASE), 8, GEN_RANGE(4, 9) },
=======
	{ .offset_ldw = RING_TIMESTAMP(RENDER_RING_BASE),
	  .offset_udw = RING_TIMESTAMP_UDW(RENDER_RING_BASE),
	  .size = 8, .gen_bitmask = GEN_RANGE(4, 9) },
>>>>>>> v4.9.227
};

int i915_reg_read_ioctl(struct drm_device *dev,
			void *data, struct drm_file *file)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct drm_i915_reg_read *reg = data;
	struct register_whitelist const *entry = whitelist;
	unsigned size;
	u64 offset;
	int i, ret = 0;

	for (i = 0; i < ARRAY_SIZE(whitelist); i++, entry++) {
		if (entry->offset == (reg->offset & -entry->size) &&
		    (1 << INTEL_INFO(dev)->gen & entry->gen_bitmask))
=======
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct drm_i915_reg_read *reg = data;
	struct register_whitelist const *entry = whitelist;
	unsigned size;
	i915_reg_t offset_ldw, offset_udw;
	int i, ret = 0;

	for (i = 0; i < ARRAY_SIZE(whitelist); i++, entry++) {
		if (i915_mmio_reg_offset(entry->offset_ldw) == (reg->offset & -entry->size) &&
		    (INTEL_INFO(dev)->gen_mask & entry->gen_bitmask))
>>>>>>> v4.9.227
			break;
	}

	if (i == ARRAY_SIZE(whitelist))
		return -EINVAL;

	/* We use the low bits to encode extra flags as the register should
	 * be naturally aligned (and those that are not so aligned merely
	 * limit the available flags for that register).
	 */
<<<<<<< HEAD
	offset = entry->offset;
	size = entry->size;
	size |= reg->offset ^ offset;
=======
	offset_ldw = entry->offset_ldw;
	offset_udw = entry->offset_udw;
	size = entry->size;
	size |= reg->offset ^ i915_mmio_reg_offset(offset_ldw);
>>>>>>> v4.9.227

	intel_runtime_pm_get(dev_priv);

	switch (size) {
	case 8 | 1:
<<<<<<< HEAD
		reg->val = I915_READ64_2x32(offset, offset+4);
		break;
	case 8:
		reg->val = I915_READ64(offset);
		break;
	case 4:
		reg->val = I915_READ(offset);
		break;
	case 2:
		reg->val = I915_READ16(offset);
		break;
	case 1:
		reg->val = I915_READ8(offset);
=======
		reg->val = I915_READ64_2x32(offset_ldw, offset_udw);
		break;
	case 8:
		reg->val = I915_READ64(offset_ldw);
		break;
	case 4:
		reg->val = I915_READ(offset_ldw);
		break;
	case 2:
		reg->val = I915_READ16(offset_ldw);
		break;
	case 1:
		reg->val = I915_READ8(offset_ldw);
>>>>>>> v4.9.227
		break;
	default:
		ret = -EINVAL;
		goto out;
	}

out:
	intel_runtime_pm_put(dev_priv);
	return ret;
}

<<<<<<< HEAD
int i915_get_reset_stats_ioctl(struct drm_device *dev,
			       void *data, struct drm_file *file)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct drm_i915_reset_stats *args = data;
	struct i915_ctx_hang_stats *hs;
	struct intel_context *ctx;
	int ret;

	if (args->flags || args->pad)
		return -EINVAL;

	if (args->ctx_id == DEFAULT_CONTEXT_HANDLE && !capable(CAP_SYS_ADMIN))
		return -EPERM;

	ret = mutex_lock_interruptible(&dev->struct_mutex);
	if (ret)
		return ret;

	ctx = i915_gem_context_get(file->driver_priv, args->ctx_id);
	if (IS_ERR(ctx)) {
		mutex_unlock(&dev->struct_mutex);
		return PTR_ERR(ctx);
	}
	hs = &ctx->hang_stats;

	if (capable(CAP_SYS_ADMIN))
		args->reset_count = i915_reset_count(&dev_priv->gpu_error);
	else
		args->reset_count = 0;

	args->batch_active = hs->batch_active;
	args->batch_pending = hs->batch_pending;

	mutex_unlock(&dev->struct_mutex);

	return 0;
}

static int i915_reset_complete(struct drm_device *dev)
{
	u8 gdrst;
	pci_read_config_byte(dev->pdev, I915_GDRST, &gdrst);
	return (gdrst & GRDOM_RESET_STATUS) == 0;
}

static int i915_do_reset(struct drm_device *dev)
{
	/* assert reset for at least 20 usec */
	pci_write_config_byte(dev->pdev, I915_GDRST, GRDOM_RESET_ENABLE);
	udelay(20);
	pci_write_config_byte(dev->pdev, I915_GDRST, 0);

	return wait_for(i915_reset_complete(dev), 500);
}

static int g4x_reset_complete(struct drm_device *dev)
{
	u8 gdrst;
	pci_read_config_byte(dev->pdev, I915_GDRST, &gdrst);
	return (gdrst & GRDOM_RESET_ENABLE) == 0;
}

static int g33_do_reset(struct drm_device *dev)
{
	pci_write_config_byte(dev->pdev, I915_GDRST, GRDOM_RESET_ENABLE);
	return wait_for(g4x_reset_complete(dev), 500);
}

static int g4x_do_reset(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	int ret;

	pci_write_config_byte(dev->pdev, I915_GDRST,
			      GRDOM_RENDER | GRDOM_RESET_ENABLE);
	ret =  wait_for(g4x_reset_complete(dev), 500);
=======
static int i915_reset_complete(struct pci_dev *pdev)
{
	u8 gdrst;
	pci_read_config_byte(pdev, I915_GDRST, &gdrst);
	return (gdrst & GRDOM_RESET_STATUS) == 0;
}

static int i915_do_reset(struct drm_i915_private *dev_priv, unsigned engine_mask)
{
	struct pci_dev *pdev = dev_priv->drm.pdev;

	/* assert reset for at least 20 usec */
	pci_write_config_byte(pdev, I915_GDRST, GRDOM_RESET_ENABLE);
	udelay(20);
	pci_write_config_byte(pdev, I915_GDRST, 0);

	return wait_for(i915_reset_complete(pdev), 500);
}

static int g4x_reset_complete(struct pci_dev *pdev)
{
	u8 gdrst;
	pci_read_config_byte(pdev, I915_GDRST, &gdrst);
	return (gdrst & GRDOM_RESET_ENABLE) == 0;
}

static int g33_do_reset(struct drm_i915_private *dev_priv, unsigned engine_mask)
{
	struct pci_dev *pdev = dev_priv->drm.pdev;
	pci_write_config_byte(pdev, I915_GDRST, GRDOM_RESET_ENABLE);
	return wait_for(g4x_reset_complete(pdev), 500);
}

static int g4x_do_reset(struct drm_i915_private *dev_priv, unsigned engine_mask)
{
	struct pci_dev *pdev = dev_priv->drm.pdev;
	int ret;

	pci_write_config_byte(pdev, I915_GDRST,
			      GRDOM_RENDER | GRDOM_RESET_ENABLE);
	ret =  wait_for(g4x_reset_complete(pdev), 500);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* WaVcpClkGateDisableForMediaReset:ctg,elk */
	I915_WRITE(VDECCLK_GATE_D, I915_READ(VDECCLK_GATE_D) | VCP_UNIT_CLOCK_GATE_DISABLE);
	POSTING_READ(VDECCLK_GATE_D);

<<<<<<< HEAD
	pci_write_config_byte(dev->pdev, I915_GDRST,
			      GRDOM_MEDIA | GRDOM_RESET_ENABLE);
	ret =  wait_for(g4x_reset_complete(dev), 500);
=======
	pci_write_config_byte(pdev, I915_GDRST,
			      GRDOM_MEDIA | GRDOM_RESET_ENABLE);
	ret =  wait_for(g4x_reset_complete(pdev), 500);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* WaVcpClkGateDisableForMediaReset:ctg,elk */
	I915_WRITE(VDECCLK_GATE_D, I915_READ(VDECCLK_GATE_D) & ~VCP_UNIT_CLOCK_GATE_DISABLE);
	POSTING_READ(VDECCLK_GATE_D);

<<<<<<< HEAD
	pci_write_config_byte(dev->pdev, I915_GDRST, 0);
=======
	pci_write_config_byte(pdev, I915_GDRST, 0);
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int ironlake_do_reset(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
=======
static int ironlake_do_reset(struct drm_i915_private *dev_priv,
			     unsigned engine_mask)
{
>>>>>>> v4.9.227
	int ret;

	I915_WRITE(ILK_GDSR,
		   ILK_GRDOM_RENDER | ILK_GRDOM_RESET_ENABLE);
<<<<<<< HEAD
	ret = wait_for((I915_READ(ILK_GDSR) &
			ILK_GRDOM_RESET_ENABLE) == 0, 500);
=======
	ret = intel_wait_for_register(dev_priv,
				      ILK_GDSR, ILK_GRDOM_RESET_ENABLE, 0,
				      500);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	I915_WRITE(ILK_GDSR,
		   ILK_GRDOM_MEDIA | ILK_GRDOM_RESET_ENABLE);
<<<<<<< HEAD
	ret = wait_for((I915_READ(ILK_GDSR) &
			ILK_GRDOM_RESET_ENABLE) == 0, 500);
=======
	ret = intel_wait_for_register(dev_priv,
				      ILK_GDSR, ILK_GRDOM_RESET_ENABLE, 0,
				      500);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	I915_WRITE(ILK_GDSR, 0);

	return 0;
}

<<<<<<< HEAD
static int gen6_do_reset(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	int	ret;

	/* Reset the chip */

=======
/* Reset the hardware domains (GENX_GRDOM_*) specified by mask */
static int gen6_hw_domain_reset(struct drm_i915_private *dev_priv,
				u32 hw_domain_mask)
{
>>>>>>> v4.9.227
	/* GEN6_GDRST is not in the gt power well, no need to check
	 * for fifo space for the write or forcewake the chip for
	 * the read
	 */
<<<<<<< HEAD
	__raw_i915_write32(dev_priv, GEN6_GDRST, GEN6_GRDOM_FULL);

	/* Spin waiting for the device to ack the reset request */
	ret = wait_for((__raw_i915_read32(dev_priv, GEN6_GDRST) & GEN6_GRDOM_FULL) == 0, 500);

	intel_uncore_forcewake_reset(dev, true);
=======
	__raw_i915_write32(dev_priv, GEN6_GDRST, hw_domain_mask);

	/* Spin waiting for the device to ack the reset requests */
	return intel_wait_for_register_fw(dev_priv,
					  GEN6_GDRST, hw_domain_mask, 0,
					  500);
}

/**
 * gen6_reset_engines - reset individual engines
 * @dev_priv: i915 device
 * @engine_mask: mask of intel_ring_flag() engines or ALL_ENGINES for full reset
 *
 * This function will reset the individual engines that are set in engine_mask.
 * If you provide ALL_ENGINES as mask, full global domain reset will be issued.
 *
 * Note: It is responsibility of the caller to handle the difference between
 * asking full domain reset versus reset for all available individual engines.
 *
 * Returns 0 on success, nonzero on error.
 */
static int gen6_reset_engines(struct drm_i915_private *dev_priv,
			      unsigned engine_mask)
{
	struct intel_engine_cs *engine;
	const u32 hw_engine_mask[I915_NUM_ENGINES] = {
		[RCS] = GEN6_GRDOM_RENDER,
		[BCS] = GEN6_GRDOM_BLT,
		[VCS] = GEN6_GRDOM_MEDIA,
		[VCS2] = GEN8_GRDOM_MEDIA2,
		[VECS] = GEN6_GRDOM_VECS,
	};
	u32 hw_mask;
	int ret;

	if (engine_mask == ALL_ENGINES) {
		hw_mask = GEN6_GRDOM_FULL;
	} else {
		unsigned int tmp;

		hw_mask = 0;
		for_each_engine_masked(engine, dev_priv, engine_mask, tmp)
			hw_mask |= hw_engine_mask[engine->id];
	}

	ret = gen6_hw_domain_reset(dev_priv, hw_mask);

	intel_uncore_forcewake_reset(dev_priv, true);
>>>>>>> v4.9.227

	return ret;
}

<<<<<<< HEAD
static int wait_for_register(struct drm_i915_private *dev_priv,
			     const u32 reg,
			     const u32 mask,
			     const u32 value,
			     const unsigned long timeout_ms)
{
	return wait_for((I915_READ(reg) & mask) == value, timeout_ms);
}

static int gen8_do_reset(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *engine;
	int i;

	for_each_ring(engine, dev_priv, i) {
		I915_WRITE(RING_RESET_CTL(engine->mmio_base),
			   _MASKED_BIT_ENABLE(RESET_CTL_REQUEST_RESET));

		if (wait_for_register(dev_priv,
				      RING_RESET_CTL(engine->mmio_base),
				      RESET_CTL_READY_TO_RESET,
				      RESET_CTL_READY_TO_RESET,
				      700)) {
			DRM_ERROR("%s: reset request timeout\n", engine->name);
			goto not_ready;
		}
	}

	return gen6_do_reset(dev);

not_ready:
	for_each_ring(engine, dev_priv, i)
		I915_WRITE(RING_RESET_CTL(engine->mmio_base),
			   _MASKED_BIT_DISABLE(RESET_CTL_REQUEST_RESET));
=======
/**
 * intel_wait_for_register_fw - wait until register matches expected state
 * @dev_priv: the i915 device
 * @reg: the register to read
 * @mask: mask to apply to register value
 * @value: expected value
 * @timeout_ms: timeout in millisecond
 *
 * This routine waits until the target register @reg contains the expected
 * @value after applying the @mask, i.e. it waits until ::
 *
 *     (I915_READ_FW(reg) & mask) == value
 *
 * Otherwise, the wait will timeout after @timeout_ms milliseconds.
 *
 * Note that this routine assumes the caller holds forcewake asserted, it is
 * not suitable for very long waits. See intel_wait_for_register() if you
 * wish to wait without holding forcewake for the duration (i.e. you expect
 * the wait to be slow).
 *
 * Returns 0 if the register matches the desired condition, or -ETIMEOUT.
 */
int intel_wait_for_register_fw(struct drm_i915_private *dev_priv,
			       i915_reg_t reg,
			       const u32 mask,
			       const u32 value,
			       const unsigned long timeout_ms)
{
#define done ((I915_READ_FW(reg) & mask) == value)
	int ret = wait_for_us(done, 2);
	if (ret)
		ret = wait_for(done, timeout_ms);
	return ret;
#undef done
}

/**
 * intel_wait_for_register - wait until register matches expected state
 * @dev_priv: the i915 device
 * @reg: the register to read
 * @mask: mask to apply to register value
 * @value: expected value
 * @timeout_ms: timeout in millisecond
 *
 * This routine waits until the target register @reg contains the expected
 * @value after applying the @mask, i.e. it waits until ::
 *
 *     (I915_READ(reg) & mask) == value
 *
 * Otherwise, the wait will timeout after @timeout_ms milliseconds.
 *
 * Returns 0 if the register matches the desired condition, or -ETIMEOUT.
 */
int intel_wait_for_register(struct drm_i915_private *dev_priv,
			    i915_reg_t reg,
			    const u32 mask,
			    const u32 value,
			    const unsigned long timeout_ms)
{

	unsigned fw =
		intel_uncore_forcewake_for_reg(dev_priv, reg, FW_REG_READ);
	int ret;

	intel_uncore_forcewake_get(dev_priv, fw);
	ret = wait_for_us((I915_READ_FW(reg) & mask) == value, 2);
	intel_uncore_forcewake_put(dev_priv, fw);
	if (ret)
		ret = wait_for((I915_READ_NOTRACE(reg) & mask) == value,
			       timeout_ms);

	return ret;
}

static int gen8_request_engine_reset(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;
	int ret;

	I915_WRITE_FW(RING_RESET_CTL(engine->mmio_base),
		      _MASKED_BIT_ENABLE(RESET_CTL_REQUEST_RESET));

	ret = intel_wait_for_register_fw(dev_priv,
					 RING_RESET_CTL(engine->mmio_base),
					 RESET_CTL_READY_TO_RESET,
					 RESET_CTL_READY_TO_RESET,
					 700);
	if (ret)
		DRM_ERROR("%s: reset request timeout\n", engine->name);

	return ret;
}

static void gen8_unrequest_engine_reset(struct intel_engine_cs *engine)
{
	struct drm_i915_private *dev_priv = engine->i915;

	I915_WRITE_FW(RING_RESET_CTL(engine->mmio_base),
		      _MASKED_BIT_DISABLE(RESET_CTL_REQUEST_RESET));
}

static int gen8_reset_engines(struct drm_i915_private *dev_priv,
			      unsigned engine_mask)
{
	struct intel_engine_cs *engine;
	unsigned int tmp;

	for_each_engine_masked(engine, dev_priv, engine_mask, tmp)
		if (gen8_request_engine_reset(engine))
			goto not_ready;

	return gen6_reset_engines(dev_priv, engine_mask);

not_ready:
	for_each_engine_masked(engine, dev_priv, engine_mask, tmp)
		gen8_unrequest_engine_reset(engine);
>>>>>>> v4.9.227

	return -EIO;
}

<<<<<<< HEAD
static int (*intel_get_gpu_reset(struct drm_device *dev))(struct drm_device *)
=======
typedef int (*reset_func)(struct drm_i915_private *, unsigned engine_mask);

static reset_func intel_get_gpu_reset(struct drm_i915_private *dev_priv)
>>>>>>> v4.9.227
{
	if (!i915.reset)
		return NULL;

<<<<<<< HEAD
	if (INTEL_INFO(dev)->gen >= 8)
		return gen8_do_reset;
	else if (INTEL_INFO(dev)->gen >= 6)
		return gen6_do_reset;
	else if (IS_GEN5(dev))
		return ironlake_do_reset;
	else if (IS_G4X(dev))
		return g4x_do_reset;
	else if (IS_G33(dev))
		return g33_do_reset;
	else if (INTEL_INFO(dev)->gen >= 3)
=======
	if (INTEL_INFO(dev_priv)->gen >= 8)
		return gen8_reset_engines;
	else if (INTEL_INFO(dev_priv)->gen >= 6)
		return gen6_reset_engines;
	else if (IS_GEN5(dev_priv))
		return ironlake_do_reset;
	else if (IS_G4X(dev_priv))
		return g4x_do_reset;
	else if (IS_G33(dev_priv))
		return g33_do_reset;
	else if (INTEL_INFO(dev_priv)->gen >= 3)
>>>>>>> v4.9.227
		return i915_do_reset;
	else
		return NULL;
}

<<<<<<< HEAD
int intel_gpu_reset(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	int (*reset)(struct drm_device *);
	int ret;

	reset = intel_get_gpu_reset(dev);
=======
int intel_gpu_reset(struct drm_i915_private *dev_priv, unsigned engine_mask)
{
	reset_func reset;
	int ret;

	reset = intel_get_gpu_reset(dev_priv);
>>>>>>> v4.9.227
	if (reset == NULL)
		return -ENODEV;

	/* If the power well sleeps during the reset, the reset
	 * request may be dropped and never completes (causing -EIO).
	 */
	intel_uncore_forcewake_get(dev_priv, FORCEWAKE_ALL);
<<<<<<< HEAD
	ret = reset(dev);
=======
	ret = reset(dev_priv, engine_mask);
>>>>>>> v4.9.227
	intel_uncore_forcewake_put(dev_priv, FORCEWAKE_ALL);

	return ret;
}

<<<<<<< HEAD
bool intel_has_gpu_reset(struct drm_device *dev)
{
	return intel_get_gpu_reset(dev) != NULL;
}

void intel_uncore_check_errors(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;

	if (HAS_FPGA_DBG_UNCLAIMED(dev) &&
	    (__raw_i915_read32(dev_priv, FPGA_DBG) & FPGA_DBG_RM_NOCLAIM)) {
		DRM_ERROR("Unclaimed register before interrupt\n");
		__raw_i915_write32(dev_priv, FPGA_DBG, FPGA_DBG_RM_NOCLAIM);
	}
=======
bool intel_has_gpu_reset(struct drm_i915_private *dev_priv)
{
	return intel_get_gpu_reset(dev_priv) != NULL;
}

int intel_guc_reset(struct drm_i915_private *dev_priv)
{
	int ret;
	unsigned long irqflags;

	if (!HAS_GUC(dev_priv))
		return -EINVAL;

	intel_uncore_forcewake_get(dev_priv, FORCEWAKE_ALL);
	spin_lock_irqsave(&dev_priv->uncore.lock, irqflags);

	ret = gen6_hw_domain_reset(dev_priv, GEN9_GRDOM_GUC);

	spin_unlock_irqrestore(&dev_priv->uncore.lock, irqflags);
	intel_uncore_forcewake_put(dev_priv, FORCEWAKE_ALL);

	return ret;
}

bool intel_uncore_unclaimed_mmio(struct drm_i915_private *dev_priv)
{
	return check_for_unclaimed_mmio(dev_priv);
}

bool
intel_uncore_arm_unclaimed_mmio_detection(struct drm_i915_private *dev_priv)
{
	if (unlikely(i915.mmio_debug ||
		     dev_priv->uncore.unclaimed_mmio_check <= 0))
		return false;

	if (unlikely(intel_uncore_unclaimed_mmio(dev_priv))) {
		DRM_DEBUG("Unclaimed register detected, "
			  "enabling oneshot unclaimed register reporting. "
			  "Please use i915.mmio_debug=N for more information.\n");
		i915.mmio_debug++;
		dev_priv->uncore.unclaimed_mmio_check--;
		return true;
	}

	return false;
}

static enum forcewake_domains
intel_uncore_forcewake_for_read(struct drm_i915_private *dev_priv,
				i915_reg_t reg)
{
	enum forcewake_domains fw_domains;

	if (intel_vgpu_active(dev_priv))
		return 0;

	switch (INTEL_GEN(dev_priv)) {
	case 9:
		fw_domains = __gen9_reg_read_fw_domains(i915_mmio_reg_offset(reg));
		break;
	case 8:
		if (IS_CHERRYVIEW(dev_priv))
			fw_domains = __chv_reg_read_fw_domains(i915_mmio_reg_offset(reg));
		else
			fw_domains = __gen6_reg_read_fw_domains(i915_mmio_reg_offset(reg));
		break;
	case 7:
	case 6:
		if (IS_VALLEYVIEW(dev_priv))
			fw_domains = __vlv_reg_read_fw_domains(i915_mmio_reg_offset(reg));
		else
			fw_domains = __gen6_reg_read_fw_domains(i915_mmio_reg_offset(reg));
		break;
	default:
		MISSING_CASE(INTEL_INFO(dev_priv)->gen);
	case 5: /* forcewake was introduced with gen6 */
	case 4:
	case 3:
	case 2:
		return 0;
	}

	WARN_ON(fw_domains & ~dev_priv->uncore.fw_domains);

	return fw_domains;
}

static enum forcewake_domains
intel_uncore_forcewake_for_write(struct drm_i915_private *dev_priv,
				 i915_reg_t reg)
{
	enum forcewake_domains fw_domains;

	if (intel_vgpu_active(dev_priv))
		return 0;

	switch (INTEL_GEN(dev_priv)) {
	case 9:
		fw_domains = __gen9_reg_write_fw_domains(i915_mmio_reg_offset(reg));
		break;
	case 8:
		if (IS_CHERRYVIEW(dev_priv))
			fw_domains = __chv_reg_write_fw_domains(i915_mmio_reg_offset(reg));
		else
			fw_domains = __gen8_reg_write_fw_domains(i915_mmio_reg_offset(reg));
		break;
	case 7:
	case 6:
		fw_domains = FORCEWAKE_RENDER;
		break;
	default:
		MISSING_CASE(INTEL_INFO(dev_priv)->gen);
	case 5:
	case 4:
	case 3:
	case 2:
		return 0;
	}

	WARN_ON(fw_domains & ~dev_priv->uncore.fw_domains);

	return fw_domains;
}

/**
 * intel_uncore_forcewake_for_reg - which forcewake domains are needed to access
 * 				    a register
 * @dev_priv: pointer to struct drm_i915_private
 * @reg: register in question
 * @op: operation bitmask of FW_REG_READ and/or FW_REG_WRITE
 *
 * Returns a set of forcewake domains required to be taken with for example
 * intel_uncore_forcewake_get for the specified register to be accessible in the
 * specified mode (read, write or read/write) with raw mmio accessors.
 *
 * NOTE: On Gen6 and Gen7 write forcewake domain (FORCEWAKE_RENDER) requires the
 * callers to do FIFO management on their own or risk losing writes.
 */
enum forcewake_domains
intel_uncore_forcewake_for_reg(struct drm_i915_private *dev_priv,
			       i915_reg_t reg, unsigned int op)
{
	enum forcewake_domains fw_domains = 0;

	WARN_ON(!op);

	if (op & FW_REG_READ)
		fw_domains = intel_uncore_forcewake_for_read(dev_priv, reg);

	if (op & FW_REG_WRITE)
		fw_domains |= intel_uncore_forcewake_for_write(dev_priv, reg);

	return fw_domains;
>>>>>>> v4.9.227
}
