/*
 * Copyright © 2008-2015 Intel Corporation
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
 *
 */

#include <drm/drmP.h>
#include <drm/drm_vma_manager.h>
#include <drm/i915_drm.h>
#include "i915_drv.h"
<<<<<<< HEAD
#include "i915_vgpu.h"
#include "i915_trace.h"
#include "intel_drv.h"
=======
#include "i915_gem_dmabuf.h"
#include "i915_vgpu.h"
#include "i915_trace.h"
#include "intel_drv.h"
#include "intel_frontbuffer.h"
#include "intel_mocs.h"
#include <linux/reservation.h>
>>>>>>> v4.9.227
#include <linux/shmem_fs.h>
#include <linux/slab.h>
#include <linux/swap.h>
#include <linux/pci.h>
#include <linux/dma-buf.h>

<<<<<<< HEAD
#define RQ_BUG_ON(expr)

static void i915_gem_object_flush_gtt_write_domain(struct drm_i915_gem_object *obj);
static void i915_gem_object_flush_cpu_write_domain(struct drm_i915_gem_object *obj);
static void
i915_gem_object_retire__write(struct drm_i915_gem_object *obj);
static void
i915_gem_object_retire__read(struct drm_i915_gem_object *obj, int ring);
=======
static void i915_gem_object_flush_gtt_write_domain(struct drm_i915_gem_object *obj);
static void i915_gem_object_flush_cpu_write_domain(struct drm_i915_gem_object *obj);
>>>>>>> v4.9.227

static bool cpu_cache_is_coherent(struct drm_device *dev,
				  enum i915_cache_level level)
{
	return HAS_LLC(dev) || level != I915_CACHE_NONE;
}

static bool cpu_write_needs_clflush(struct drm_i915_gem_object *obj)
{
<<<<<<< HEAD
=======
	if (obj->base.write_domain == I915_GEM_DOMAIN_CPU)
		return false;

>>>>>>> v4.9.227
	if (!cpu_cache_is_coherent(obj->base.dev, obj->cache_level))
		return true;

	return obj->pin_display;
}

<<<<<<< HEAD
=======
static int
insert_mappable_node(struct drm_i915_private *i915,
                     struct drm_mm_node *node, u32 size)
{
	memset(node, 0, sizeof(*node));
	return drm_mm_insert_node_in_range_generic(&i915->ggtt.base.mm, node,
						   size, 0, 0, 0,
						   i915->ggtt.mappable_end,
						   DRM_MM_SEARCH_DEFAULT,
						   DRM_MM_CREATE_DEFAULT);
}

static void
remove_mappable_node(struct drm_mm_node *node)
{
	drm_mm_remove_node(node);
}

>>>>>>> v4.9.227
/* some bookkeeping */
static void i915_gem_info_add_obj(struct drm_i915_private *dev_priv,
				  size_t size)
{
	spin_lock(&dev_priv->mm.object_stat_lock);
	dev_priv->mm.object_count++;
	dev_priv->mm.object_memory += size;
	spin_unlock(&dev_priv->mm.object_stat_lock);
}

static void i915_gem_info_remove_obj(struct drm_i915_private *dev_priv,
				     size_t size)
{
	spin_lock(&dev_priv->mm.object_stat_lock);
	dev_priv->mm.object_count--;
	dev_priv->mm.object_memory -= size;
	spin_unlock(&dev_priv->mm.object_stat_lock);
}

static int
i915_gem_wait_for_error(struct i915_gpu_error *error)
{
	int ret;

<<<<<<< HEAD
#define EXIT_COND (!i915_reset_in_progress(error) || \
		   i915_terminally_wedged(error))
	if (EXIT_COND)
=======
	if (!i915_reset_in_progress(error))
>>>>>>> v4.9.227
		return 0;

	/*
	 * Only wait 10 seconds for the gpu reset to complete to avoid hanging
	 * userspace. If it takes that long something really bad is going on and
	 * we should simply try to bail out and fail as gracefully as possible.
	 */
	ret = wait_event_interruptible_timeout(error->reset_queue,
<<<<<<< HEAD
					       EXIT_COND,
=======
					       !i915_reset_in_progress(error),
>>>>>>> v4.9.227
					       10*HZ);
	if (ret == 0) {
		DRM_ERROR("Timed out waiting for the gpu reset to complete\n");
		return -EIO;
	} else if (ret < 0) {
		return ret;
<<<<<<< HEAD
	}
#undef EXIT_COND

	return 0;
=======
	} else {
		return 0;
	}
>>>>>>> v4.9.227
}

int i915_mutex_lock_interruptible(struct drm_device *dev)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
=======
	struct drm_i915_private *dev_priv = to_i915(dev);
>>>>>>> v4.9.227
	int ret;

	ret = i915_gem_wait_for_error(&dev_priv->gpu_error);
	if (ret)
		return ret;

	ret = mutex_lock_interruptible(&dev->struct_mutex);
	if (ret)
		return ret;

<<<<<<< HEAD
	WARN_ON(i915_verify_lists(dev));
=======
>>>>>>> v4.9.227
	return 0;
}

int
i915_gem_get_aperture_ioctl(struct drm_device *dev, void *data,
			    struct drm_file *file)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct drm_i915_gem_get_aperture *args = data;
	struct i915_gtt *ggtt = &dev_priv->gtt;
=======
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct i915_ggtt *ggtt = &dev_priv->ggtt;
	struct drm_i915_gem_get_aperture *args = data;
>>>>>>> v4.9.227
	struct i915_vma *vma;
	size_t pinned;

	pinned = 0;
	mutex_lock(&dev->struct_mutex);
<<<<<<< HEAD
	list_for_each_entry(vma, &ggtt->base.active_list, mm_list)
		if (vma->pin_count)
			pinned += vma->node.size;
	list_for_each_entry(vma, &ggtt->base.inactive_list, mm_list)
		if (vma->pin_count)
			pinned += vma->node.size;
	mutex_unlock(&dev->struct_mutex);

	args->aper_size = dev_priv->gtt.base.total;
=======
	list_for_each_entry(vma, &ggtt->base.active_list, vm_link)
		if (i915_vma_is_pinned(vma))
			pinned += vma->node.size;
	list_for_each_entry(vma, &ggtt->base.inactive_list, vm_link)
		if (i915_vma_is_pinned(vma))
			pinned += vma->node.size;
	mutex_unlock(&dev->struct_mutex);

	args->aper_size = ggtt->base.total;
>>>>>>> v4.9.227
	args->aper_available_size = args->aper_size - pinned;

	return 0;
}

static int
i915_gem_object_get_pages_phys(struct drm_i915_gem_object *obj)
{
<<<<<<< HEAD
	struct address_space *mapping = file_inode(obj->base.filp)->i_mapping;
=======
	struct address_space *mapping = obj->base.filp->f_mapping;
>>>>>>> v4.9.227
	char *vaddr = obj->phys_handle->vaddr;
	struct sg_table *st;
	struct scatterlist *sg;
	int i;

	if (WARN_ON(i915_gem_object_needs_bit17_swizzle(obj)))
		return -EINVAL;

	for (i = 0; i < obj->base.size / PAGE_SIZE; i++) {
		struct page *page;
		char *src;

		page = shmem_read_mapping_page(mapping, i);
		if (IS_ERR(page))
			return PTR_ERR(page);

		src = kmap_atomic(page);
		memcpy(vaddr, src, PAGE_SIZE);
		drm_clflush_virt_range(vaddr, PAGE_SIZE);
		kunmap_atomic(src);

<<<<<<< HEAD
		page_cache_release(page);
		vaddr += PAGE_SIZE;
	}

	i915_gem_chipset_flush(obj->base.dev);
=======
		put_page(page);
		vaddr += PAGE_SIZE;
	}

	i915_gem_chipset_flush(to_i915(obj->base.dev));
>>>>>>> v4.9.227

	st = kmalloc(sizeof(*st), GFP_KERNEL);
	if (st == NULL)
		return -ENOMEM;

	if (sg_alloc_table(st, 1, GFP_KERNEL)) {
		kfree(st);
		return -ENOMEM;
	}

	sg = st->sgl;
	sg->offset = 0;
	sg->length = obj->base.size;

	sg_dma_address(sg) = obj->phys_handle->busaddr;
	sg_dma_len(sg) = obj->base.size;

	obj->pages = st;
	return 0;
}

static void
i915_gem_object_put_pages_phys(struct drm_i915_gem_object *obj)
{
	int ret;

	BUG_ON(obj->madv == __I915_MADV_PURGED);

	ret = i915_gem_object_set_to_cpu_domain(obj, true);
<<<<<<< HEAD
	if (ret) {
		/* In the event of a disaster, abandon all caches and
		 * hope for the best.
		 */
		WARN_ON(ret != -EIO);
=======
	if (WARN_ON(ret)) {
		/* In the event of a disaster, abandon all caches and
		 * hope for the best.
		 */
>>>>>>> v4.9.227
		obj->base.read_domains = obj->base.write_domain = I915_GEM_DOMAIN_CPU;
	}

	if (obj->madv == I915_MADV_DONTNEED)
		obj->dirty = 0;

	if (obj->dirty) {
<<<<<<< HEAD
		struct address_space *mapping = file_inode(obj->base.filp)->i_mapping;
=======
		struct address_space *mapping = obj->base.filp->f_mapping;
>>>>>>> v4.9.227
		char *vaddr = obj->phys_handle->vaddr;
		int i;

		for (i = 0; i < obj->base.size / PAGE_SIZE; i++) {
			struct page *page;
			char *dst;

			page = shmem_read_mapping_page(mapping, i);
			if (IS_ERR(page))
				continue;

			dst = kmap_atomic(page);
			drm_clflush_virt_range(vaddr, PAGE_SIZE);
			memcpy(dst, vaddr, PAGE_SIZE);
			kunmap_atomic(dst);

			set_page_dirty(page);
			if (obj->madv == I915_MADV_WILLNEED)
				mark_page_accessed(page);
<<<<<<< HEAD
			page_cache_release(page);
=======
			put_page(page);
>>>>>>> v4.9.227
			vaddr += PAGE_SIZE;
		}
		obj->dirty = 0;
	}

	sg_free_table(obj->pages);
	kfree(obj->pages);
}

static void
i915_gem_object_release_phys(struct drm_i915_gem_object *obj)
{
	drm_pci_free(obj->base.dev, obj->phys_handle);
}

static const struct drm_i915_gem_object_ops i915_gem_phys_ops = {
	.get_pages = i915_gem_object_get_pages_phys,
	.put_pages = i915_gem_object_put_pages_phys,
	.release = i915_gem_object_release_phys,
};

<<<<<<< HEAD
static int
drop_pages(struct drm_i915_gem_object *obj)
{
	struct i915_vma *vma, *next;
	int ret;

	drm_gem_object_reference(&obj->base);
	list_for_each_entry_safe(vma, next, &obj->vma_list, vma_link)
		if (i915_vma_unbind(vma))
			break;

	ret = i915_gem_object_put_pages(obj);
	drm_gem_object_unreference(&obj->base);
=======
int i915_gem_object_unbind(struct drm_i915_gem_object *obj)
{
	struct i915_vma *vma;
	LIST_HEAD(still_in_list);
	int ret;

	lockdep_assert_held(&obj->base.dev->struct_mutex);

	/* Closed vma are removed from the obj->vma_list - but they may
	 * still have an active binding on the object. To remove those we
	 * must wait for all rendering to complete to the object (as unbinding
	 * must anyway), and retire the requests.
	 */
	ret = i915_gem_object_wait_rendering(obj, false);
	if (ret)
		return ret;

	i915_gem_retire_requests(to_i915(obj->base.dev));

	while ((vma = list_first_entry_or_null(&obj->vma_list,
					       struct i915_vma,
					       obj_link))) {
		list_move_tail(&vma->obj_link, &still_in_list);
		ret = i915_vma_unbind(vma);
		if (ret)
			break;
	}
	list_splice(&still_in_list, &obj->vma_list);
>>>>>>> v4.9.227

	return ret;
}

<<<<<<< HEAD
=======
/**
 * Ensures that all rendering to the object has completed and the object is
 * safe to unbind from the GTT or access from the CPU.
 * @obj: i915 gem object
 * @readonly: waiting for just read access or read-write access
 */
int
i915_gem_object_wait_rendering(struct drm_i915_gem_object *obj,
			       bool readonly)
{
	struct reservation_object *resv;
	struct i915_gem_active *active;
	unsigned long active_mask;
	int idx;

	lockdep_assert_held(&obj->base.dev->struct_mutex);

	if (!readonly) {
		active = obj->last_read;
		active_mask = i915_gem_object_get_active(obj);
	} else {
		active_mask = 1;
		active = &obj->last_write;
	}

	for_each_active(active_mask, idx) {
		int ret;

		ret = i915_gem_active_wait(&active[idx],
					   &obj->base.dev->struct_mutex);
		if (ret)
			return ret;
	}

	resv = i915_gem_object_get_dmabuf_resv(obj);
	if (resv) {
		long err;

		err = reservation_object_wait_timeout_rcu(resv, !readonly, true,
							  MAX_SCHEDULE_TIMEOUT);
		if (err < 0)
			return err;
	}

	return 0;
}

/* A nonblocking variant of the above wait. Must be called prior to
 * acquiring the mutex for the object, as the object state may change
 * during this call. A reference must be held by the caller for the object.
 */
static __must_check int
__unsafe_wait_rendering(struct drm_i915_gem_object *obj,
			struct intel_rps_client *rps,
			bool readonly)
{
	struct i915_gem_active *active;
	unsigned long active_mask;
	int idx;

	active_mask = __I915_BO_ACTIVE(obj);
	if (!active_mask)
		return 0;

	if (!readonly) {
		active = obj->last_read;
	} else {
		active_mask = 1;
		active = &obj->last_write;
	}

	for_each_active(active_mask, idx) {
		int ret;

		ret = i915_gem_active_wait_unlocked(&active[idx],
						    I915_WAIT_INTERRUPTIBLE,
						    NULL, rps);
		if (ret)
			return ret;
	}

	return 0;
}

static struct intel_rps_client *to_rps_client(struct drm_file *file)
{
	struct drm_i915_file_private *fpriv = file->driver_priv;

	return &fpriv->rps;
}

>>>>>>> v4.9.227
int
i915_gem_object_attach_phys(struct drm_i915_gem_object *obj,
			    int align)
{
	drm_dma_handle_t *phys;
	int ret;

	if (obj->phys_handle) {
		if ((unsigned long)obj->phys_handle->vaddr & (align -1))
			return -EBUSY;

		return 0;
	}

	if (obj->madv != I915_MADV_WILLNEED)
		return -EFAULT;

	if (obj->base.filp == NULL)
		return -EINVAL;

<<<<<<< HEAD
	ret = drop_pages(obj);
=======
	ret = i915_gem_object_unbind(obj);
	if (ret)
		return ret;

	ret = i915_gem_object_put_pages(obj);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* create a new object */
	phys = drm_pci_alloc(obj->base.dev, obj->base.size, align);
	if (!phys)
		return -ENOMEM;

	obj->phys_handle = phys;
	obj->ops = &i915_gem_phys_ops;

	return i915_gem_object_get_pages(obj);
}

static int
i915_gem_phys_pwrite(struct drm_i915_gem_object *obj,
		     struct drm_i915_gem_pwrite *args,
		     struct drm_file *file_priv)
{
	struct drm_device *dev = obj->base.dev;
	void *vaddr = obj->phys_handle->vaddr + args->offset;
<<<<<<< HEAD
	char __user *user_data = to_user_ptr(args->data_ptr);
=======
	char __user *user_data = u64_to_user_ptr(args->data_ptr);
>>>>>>> v4.9.227
	int ret = 0;

	/* We manually control the domain here and pretend that it
	 * remains coherent i.e. in the GTT domain, like shmem_pwrite.
	 */
	ret = i915_gem_object_wait_rendering(obj, false);
	if (ret)
		return ret;

	intel_fb_obj_invalidate(obj, ORIGIN_CPU);
	if (__copy_from_user_inatomic_nocache(vaddr, user_data, args->size)) {
		unsigned long unwritten;

		/* The physical object once assigned is fixed for the lifetime
		 * of the obj, so we can safely drop the lock and continue
		 * to access vaddr.
		 */
		mutex_unlock(&dev->struct_mutex);
		unwritten = copy_from_user(vaddr, user_data, args->size);
		mutex_lock(&dev->struct_mutex);
		if (unwritten) {
			ret = -EFAULT;
			goto out;
		}
	}

	drm_clflush_virt_range(vaddr, args->size);
<<<<<<< HEAD
	i915_gem_chipset_flush(dev);
=======
	i915_gem_chipset_flush(to_i915(dev));
>>>>>>> v4.9.227

out:
	intel_fb_obj_flush(obj, false, ORIGIN_CPU);
	return ret;
}

void *i915_gem_object_alloc(struct drm_device *dev)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
=======
	struct drm_i915_private *dev_priv = to_i915(dev);
>>>>>>> v4.9.227
	return kmem_cache_zalloc(dev_priv->objects, GFP_KERNEL);
}

void i915_gem_object_free(struct drm_i915_gem_object *obj)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = obj->base.dev->dev_private;
=======
	struct drm_i915_private *dev_priv = to_i915(obj->base.dev);
>>>>>>> v4.9.227
	kmem_cache_free(dev_priv->objects, obj);
}

static int
i915_gem_create(struct drm_file *file,
		struct drm_device *dev,
		uint64_t size,
		uint32_t *handle_p)
{
	struct drm_i915_gem_object *obj;
	int ret;
	u32 handle;

	size = roundup(size, PAGE_SIZE);
	if (size == 0)
		return -EINVAL;

	/* Allocate the new object */
<<<<<<< HEAD
	obj = i915_gem_alloc_object(dev, size);
	if (obj == NULL)
		return -ENOMEM;

	ret = drm_gem_handle_create(file, &obj->base, &handle);
	/* drop reference from allocate - handle holds it now */
	drm_gem_object_unreference_unlocked(&obj->base);
=======
	obj = i915_gem_object_create(dev, size);
	if (IS_ERR(obj))
		return PTR_ERR(obj);

	ret = drm_gem_handle_create(file, &obj->base, &handle);
	/* drop reference from allocate - handle holds it now */
	i915_gem_object_put_unlocked(obj);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	*handle_p = handle;
	return 0;
}

int
i915_gem_dumb_create(struct drm_file *file,
		     struct drm_device *dev,
		     struct drm_mode_create_dumb *args)
{
	/* have to work out size/pitch and return them */
	args->pitch = ALIGN(args->width * DIV_ROUND_UP(args->bpp, 8), 64);
	args->size = args->pitch * args->height;
	return i915_gem_create(file, dev,
			       args->size, &args->handle);
}

/**
 * Creates a new mm object and returns a handle to it.
<<<<<<< HEAD
=======
 * @dev: drm device pointer
 * @data: ioctl data blob
 * @file: drm file pointer
>>>>>>> v4.9.227
 */
int
i915_gem_create_ioctl(struct drm_device *dev, void *data,
		      struct drm_file *file)
{
	struct drm_i915_gem_create *args = data;

	return i915_gem_create(file, dev,
			       args->size, &args->handle);
}

static inline int
__copy_to_user_swizzled(char __user *cpu_vaddr,
			const char *gpu_vaddr, int gpu_offset,
			int length)
{
	int ret, cpu_offset = 0;

	while (length > 0) {
		int cacheline_end = ALIGN(gpu_offset + 1, 64);
		int this_length = min(cacheline_end - gpu_offset, length);
		int swizzled_gpu_offset = gpu_offset ^ 64;

		ret = __copy_to_user(cpu_vaddr + cpu_offset,
				     gpu_vaddr + swizzled_gpu_offset,
				     this_length);
		if (ret)
			return ret + length;

		cpu_offset += this_length;
		gpu_offset += this_length;
		length -= this_length;
	}

	return 0;
}

static inline int
__copy_from_user_swizzled(char *gpu_vaddr, int gpu_offset,
			  const char __user *cpu_vaddr,
			  int length)
{
	int ret, cpu_offset = 0;

	while (length > 0) {
		int cacheline_end = ALIGN(gpu_offset + 1, 64);
		int this_length = min(cacheline_end - gpu_offset, length);
		int swizzled_gpu_offset = gpu_offset ^ 64;

		ret = __copy_from_user(gpu_vaddr + swizzled_gpu_offset,
				       cpu_vaddr + cpu_offset,
				       this_length);
		if (ret)
			return ret + length;

		cpu_offset += this_length;
		gpu_offset += this_length;
		length -= this_length;
	}

	return 0;
}

/*
 * Pins the specified object's pages and synchronizes the object with
 * GPU accesses. Sets needs_clflush to non-zero if the caller should
 * flush the object from the CPU cache.
 */
int i915_gem_obj_prepare_shmem_read(struct drm_i915_gem_object *obj,
<<<<<<< HEAD
				    int *needs_clflush)
=======
				    unsigned int *needs_clflush)
>>>>>>> v4.9.227
{
	int ret;

	*needs_clflush = 0;

<<<<<<< HEAD
	if (!obj->base.filp)
		return -EINVAL;

	if (!(obj->base.read_domains & I915_GEM_DOMAIN_CPU)) {
		/* If we're not in the cpu read domain, set ourself into the gtt
		 * read domain and manually flush cachelines (if required). This
		 * optimizes for the case when the gpu will dirty the data
		 * anyway again before the next pread happens. */
		*needs_clflush = !cpu_cache_is_coherent(obj->base.dev,
							obj->cache_level);
		ret = i915_gem_object_wait_rendering(obj, true);
		if (ret)
			return ret;
	}
=======
	if (!i915_gem_object_has_struct_page(obj))
		return -ENODEV;

	ret = i915_gem_object_wait_rendering(obj, true);
	if (ret)
		return ret;
>>>>>>> v4.9.227

	ret = i915_gem_object_get_pages(obj);
	if (ret)
		return ret;

	i915_gem_object_pin_pages(obj);

<<<<<<< HEAD
=======
	i915_gem_object_flush_gtt_write_domain(obj);

	/* If we're not in the cpu read domain, set ourself into the gtt
	 * read domain and manually flush cachelines (if required). This
	 * optimizes for the case when the gpu will dirty the data
	 * anyway again before the next pread happens.
	 */
	if (!(obj->base.read_domains & I915_GEM_DOMAIN_CPU))
		*needs_clflush = !cpu_cache_is_coherent(obj->base.dev,
							obj->cache_level);

	if (*needs_clflush && !static_cpu_has(X86_FEATURE_CLFLUSH)) {
		ret = i915_gem_object_set_to_cpu_domain(obj, false);
		if (ret)
			goto err_unpin;

		*needs_clflush = 0;
	}

	/* return with the pages pinned */
	return 0;

err_unpin:
	i915_gem_object_unpin_pages(obj);
	return ret;
}

int i915_gem_obj_prepare_shmem_write(struct drm_i915_gem_object *obj,
				     unsigned int *needs_clflush)
{
	int ret;

	*needs_clflush = 0;
	if (!i915_gem_object_has_struct_page(obj))
		return -ENODEV;

	ret = i915_gem_object_wait_rendering(obj, false);
	if (ret)
		return ret;

	ret = i915_gem_object_get_pages(obj);
	if (ret)
		return ret;

	i915_gem_object_pin_pages(obj);

	i915_gem_object_flush_gtt_write_domain(obj);

	/* If we're not in the cpu write domain, set ourself into the
	 * gtt write domain and manually flush cachelines (as required).
	 * This optimizes for the case when the gpu will use the data
	 * right away and we therefore have to clflush anyway.
	 */
	if (obj->base.write_domain != I915_GEM_DOMAIN_CPU)
		*needs_clflush |= cpu_write_needs_clflush(obj) << 1;

	/* Same trick applies to invalidate partially written cachelines read
	 * before writing.
	 */
	if (!(obj->base.read_domains & I915_GEM_DOMAIN_CPU))
		*needs_clflush |= !cpu_cache_is_coherent(obj->base.dev,
							 obj->cache_level);

	if (*needs_clflush && !static_cpu_has(X86_FEATURE_CLFLUSH)) {
		ret = i915_gem_object_set_to_cpu_domain(obj, true);
		if (ret)
			goto err_unpin;

		*needs_clflush = 0;
	}

	if ((*needs_clflush & CLFLUSH_AFTER) == 0)
		obj->cache_dirty = true;

	intel_fb_obj_invalidate(obj, ORIGIN_CPU);
	obj->dirty = 1;
	/* return with the pages pinned */
	return 0;

err_unpin:
	i915_gem_object_unpin_pages(obj);
>>>>>>> v4.9.227
	return ret;
}

/* Per-page copy function for the shmem pread fastpath.
 * Flushes invalid cachelines before reading the target if
 * needs_clflush is set. */
static int
shmem_pread_fast(struct page *page, int shmem_page_offset, int page_length,
		 char __user *user_data,
		 bool page_do_bit17_swizzling, bool needs_clflush)
{
	char *vaddr;
	int ret;

	if (unlikely(page_do_bit17_swizzling))
		return -EINVAL;

	vaddr = kmap_atomic(page);
	if (needs_clflush)
		drm_clflush_virt_range(vaddr + shmem_page_offset,
				       page_length);
	ret = __copy_to_user_inatomic(user_data,
				      vaddr + shmem_page_offset,
				      page_length);
	kunmap_atomic(vaddr);

	return ret ? -EFAULT : 0;
}

static void
shmem_clflush_swizzled_range(char *addr, unsigned long length,
			     bool swizzled)
{
	if (unlikely(swizzled)) {
		unsigned long start = (unsigned long) addr;
		unsigned long end = (unsigned long) addr + length;

		/* For swizzling simply ensure that we always flush both
		 * channels. Lame, but simple and it works. Swizzled
		 * pwrite/pread is far from a hotpath - current userspace
		 * doesn't use it at all. */
		start = round_down(start, 128);
		end = round_up(end, 128);

		drm_clflush_virt_range((void *)start, end - start);
	} else {
		drm_clflush_virt_range(addr, length);
	}

}

/* Only difference to the fast-path function is that this can handle bit17
 * and uses non-atomic copy and kmap functions. */
static int
shmem_pread_slow(struct page *page, int shmem_page_offset, int page_length,
		 char __user *user_data,
		 bool page_do_bit17_swizzling, bool needs_clflush)
{
	char *vaddr;
	int ret;

	vaddr = kmap(page);
	if (needs_clflush)
		shmem_clflush_swizzled_range(vaddr + shmem_page_offset,
					     page_length,
					     page_do_bit17_swizzling);

	if (page_do_bit17_swizzling)
		ret = __copy_to_user_swizzled(user_data,
					      vaddr, shmem_page_offset,
					      page_length);
	else
		ret = __copy_to_user(user_data,
				     vaddr + shmem_page_offset,
				     page_length);
	kunmap(page);

	return ret ? - EFAULT : 0;
}

<<<<<<< HEAD
=======
static inline unsigned long
slow_user_access(struct io_mapping *mapping,
		 uint64_t page_base, int page_offset,
		 char __user *user_data,
		 unsigned long length, bool pwrite)
{
	void __iomem *ioaddr;
	void *vaddr;
	uint64_t unwritten;

	ioaddr = io_mapping_map_wc(mapping, page_base, PAGE_SIZE);
	/* We can use the cpu mem copy function because this is X86. */
	vaddr = (void __force *)ioaddr + page_offset;
	if (pwrite)
		unwritten = __copy_from_user(vaddr, user_data, length);
	else
		unwritten = __copy_to_user(user_data, vaddr, length);

	io_mapping_unmap(ioaddr);
	return unwritten;
}

static int
i915_gem_gtt_pread(struct drm_device *dev,
		   struct drm_i915_gem_object *obj, uint64_t size,
		   uint64_t data_offset, uint64_t data_ptr)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct i915_ggtt *ggtt = &dev_priv->ggtt;
	struct i915_vma *vma;
	struct drm_mm_node node;
	char __user *user_data;
	uint64_t remain;
	uint64_t offset;
	int ret;

	vma = i915_gem_object_ggtt_pin(obj, NULL, 0, 0, PIN_MAPPABLE);
	if (!IS_ERR(vma)) {
		node.start = i915_ggtt_offset(vma);
		node.allocated = false;
		ret = i915_vma_put_fence(vma);
		if (ret) {
			i915_vma_unpin(vma);
			vma = ERR_PTR(ret);
		}
	}
	if (IS_ERR(vma)) {
		ret = insert_mappable_node(dev_priv, &node, PAGE_SIZE);
		if (ret)
			goto out;

		ret = i915_gem_object_get_pages(obj);
		if (ret) {
			remove_mappable_node(&node);
			goto out;
		}

		i915_gem_object_pin_pages(obj);
	}

	ret = i915_gem_object_set_to_gtt_domain(obj, false);
	if (ret)
		goto out_unpin;

	user_data = u64_to_user_ptr(data_ptr);
	remain = size;
	offset = data_offset;

	mutex_unlock(&dev->struct_mutex);
	if (likely(!i915.prefault_disable)) {
		ret = fault_in_pages_writeable(user_data, remain);
		if (ret) {
			mutex_lock(&dev->struct_mutex);
			goto out_unpin;
		}
	}

	while (remain > 0) {
		/* Operation in this page
		 *
		 * page_base = page offset within aperture
		 * page_offset = offset within page
		 * page_length = bytes to copy for this page
		 */
		u32 page_base = node.start;
		unsigned page_offset = offset_in_page(offset);
		unsigned page_length = PAGE_SIZE - page_offset;
		page_length = remain < page_length ? remain : page_length;
		if (node.allocated) {
			wmb();
			ggtt->base.insert_page(&ggtt->base,
					       i915_gem_object_get_dma_address(obj, offset >> PAGE_SHIFT),
					       node.start,
					       I915_CACHE_NONE, 0);
			wmb();
		} else {
			page_base += offset & PAGE_MASK;
		}
		/* This is a slow read/write as it tries to read from
		 * and write to user memory which may result into page
		 * faults, and so we cannot perform this under struct_mutex.
		 */
		if (slow_user_access(&ggtt->mappable, page_base,
				     page_offset, user_data,
				     page_length, false)) {
			ret = -EFAULT;
			break;
		}

		remain -= page_length;
		user_data += page_length;
		offset += page_length;
	}

	mutex_lock(&dev->struct_mutex);
	if (ret == 0 && (obj->base.read_domains & I915_GEM_DOMAIN_GTT) == 0) {
		/* The user has modified the object whilst we tried
		 * reading from it, and we now have no idea what domain
		 * the pages should be in. As we have just been touching
		 * them directly, flush everything back to the GTT
		 * domain.
		 */
		ret = i915_gem_object_set_to_gtt_domain(obj, false);
	}

out_unpin:
	if (node.allocated) {
		wmb();
		ggtt->base.clear_range(&ggtt->base,
				       node.start, node.size,
				       true);
		i915_gem_object_unpin_pages(obj);
		remove_mappable_node(&node);
	} else {
		i915_vma_unpin(vma);
	}
out:
	return ret;
}

>>>>>>> v4.9.227
static int
i915_gem_shmem_pread(struct drm_device *dev,
		     struct drm_i915_gem_object *obj,
		     struct drm_i915_gem_pread *args,
		     struct drm_file *file)
{
	char __user *user_data;
	ssize_t remain;
	loff_t offset;
	int shmem_page_offset, page_length, ret = 0;
	int obj_do_bit17_swizzling, page_do_bit17_swizzling;
	int prefaulted = 0;
	int needs_clflush = 0;
	struct sg_page_iter sg_iter;

<<<<<<< HEAD
	user_data = to_user_ptr(args->data_ptr);
	remain = args->size;

	obj_do_bit17_swizzling = i915_gem_object_needs_bit17_swizzle(obj);

=======
>>>>>>> v4.9.227
	ret = i915_gem_obj_prepare_shmem_read(obj, &needs_clflush);
	if (ret)
		return ret;

<<<<<<< HEAD
	offset = args->offset;
=======
	obj_do_bit17_swizzling = i915_gem_object_needs_bit17_swizzle(obj);
	user_data = u64_to_user_ptr(args->data_ptr);
	offset = args->offset;
	remain = args->size;
>>>>>>> v4.9.227

	for_each_sg_page(obj->pages->sgl, &sg_iter, obj->pages->nents,
			 offset >> PAGE_SHIFT) {
		struct page *page = sg_page_iter_page(&sg_iter);

		if (remain <= 0)
			break;

		/* Operation in this page
		 *
		 * shmem_page_offset = offset within page in shmem file
		 * page_length = bytes to copy for this page
		 */
		shmem_page_offset = offset_in_page(offset);
		page_length = remain;
		if ((shmem_page_offset + page_length) > PAGE_SIZE)
			page_length = PAGE_SIZE - shmem_page_offset;

		page_do_bit17_swizzling = obj_do_bit17_swizzling &&
			(page_to_phys(page) & (1 << 17)) != 0;

		ret = shmem_pread_fast(page, shmem_page_offset, page_length,
				       user_data, page_do_bit17_swizzling,
				       needs_clflush);
		if (ret == 0)
			goto next_page;

		mutex_unlock(&dev->struct_mutex);

		if (likely(!i915.prefault_disable) && !prefaulted) {
<<<<<<< HEAD
			ret = fault_in_multipages_writeable(user_data, remain);
=======
			ret = fault_in_pages_writeable(user_data, remain);
>>>>>>> v4.9.227
			/* Userspace is tricking us, but we've already clobbered
			 * its pages with the prefault and promised to write the
			 * data up to the first fault. Hence ignore any errors
			 * and just continue. */
			(void)ret;
			prefaulted = 1;
		}

		ret = shmem_pread_slow(page, shmem_page_offset, page_length,
				       user_data, page_do_bit17_swizzling,
				       needs_clflush);

		mutex_lock(&dev->struct_mutex);

		if (ret)
			goto out;

next_page:
		remain -= page_length;
		user_data += page_length;
		offset += page_length;
	}

out:
<<<<<<< HEAD
	i915_gem_object_unpin_pages(obj);
=======
	i915_gem_obj_finish_shmem_access(obj);
>>>>>>> v4.9.227

	return ret;
}

/**
 * Reads data from the object referenced by handle.
<<<<<<< HEAD
=======
 * @dev: drm device pointer
 * @data: ioctl data blob
 * @file: drm file pointer
>>>>>>> v4.9.227
 *
 * On error, the contents of *data are undefined.
 */
int
i915_gem_pread_ioctl(struct drm_device *dev, void *data,
		     struct drm_file *file)
{
	struct drm_i915_gem_pread *args = data;
	struct drm_i915_gem_object *obj;
	int ret = 0;

	if (args->size == 0)
		return 0;

	if (!access_ok(VERIFY_WRITE,
<<<<<<< HEAD
		       to_user_ptr(args->data_ptr),
		       args->size))
		return -EFAULT;

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		return ret;

	obj = to_intel_bo(drm_gem_object_lookup(dev, file, args->handle));
	if (&obj->base == NULL) {
		ret = -ENOENT;
		goto unlock;
	}
=======
		       u64_to_user_ptr(args->data_ptr),
		       args->size))
		return -EFAULT;

	obj = i915_gem_object_lookup(file, args->handle);
	if (!obj)
		return -ENOENT;
>>>>>>> v4.9.227

	/* Bounds check source.  */
	if (args->offset > obj->base.size ||
	    args->size > obj->base.size - args->offset) {
		ret = -EINVAL;
<<<<<<< HEAD
		goto out;
	}

	/* prime objects have no backing filp to GEM pread/pwrite
	 * pages from.
	 */
	if (!obj->base.filp) {
		ret = -EINVAL;
		goto out;
=======
		goto err;
>>>>>>> v4.9.227
	}

	trace_i915_gem_object_pread(obj, args->offset, args->size);

<<<<<<< HEAD
	ret = i915_gem_shmem_pread(dev, obj, args, file);

out:
	drm_gem_object_unreference(&obj->base);
unlock:
	mutex_unlock(&dev->struct_mutex);
=======
	ret = __unsafe_wait_rendering(obj, to_rps_client(file), true);
	if (ret)
		goto err;

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		goto err;

	ret = i915_gem_shmem_pread(dev, obj, args, file);

	/* pread for non shmem backed objects */
	if (ret == -EFAULT || ret == -ENODEV) {
		intel_runtime_pm_get(to_i915(dev));
		ret = i915_gem_gtt_pread(dev, obj, args->size,
					args->offset, args->data_ptr);
		intel_runtime_pm_put(to_i915(dev));
	}

	i915_gem_object_put(obj);
	mutex_unlock(&dev->struct_mutex);

	return ret;

err:
	i915_gem_object_put_unlocked(obj);
>>>>>>> v4.9.227
	return ret;
}

/* This is the fast write path which cannot handle
 * page faults in the source data
 */

static inline int
fast_user_write(struct io_mapping *mapping,
		loff_t page_base, int page_offset,
		char __user *user_data,
		int length)
{
	void __iomem *vaddr_atomic;
	void *vaddr;
	unsigned long unwritten;

	vaddr_atomic = io_mapping_map_atomic_wc(mapping, page_base);
	/* We can use the cpu mem copy function because this is X86. */
	vaddr = (void __force*)vaddr_atomic + page_offset;
	unwritten = __copy_from_user_inatomic_nocache(vaddr,
						      user_data, length);
	io_mapping_unmap_atomic(vaddr_atomic);
	return unwritten;
}

/**
 * This is the fast pwrite path, where we copy the data directly from the
 * user into the GTT, uncached.
<<<<<<< HEAD
 */
static int
i915_gem_gtt_pwrite_fast(struct drm_device *dev,
=======
 * @i915: i915 device private data
 * @obj: i915 gem object
 * @args: pwrite arguments structure
 * @file: drm file pointer
 */
static int
i915_gem_gtt_pwrite_fast(struct drm_i915_private *i915,
>>>>>>> v4.9.227
			 struct drm_i915_gem_object *obj,
			 struct drm_i915_gem_pwrite *args,
			 struct drm_file *file)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
	ssize_t remain;
	loff_t offset, page_base;
	char __user *user_data;
	int page_offset, page_length, ret;

	ret = i915_gem_obj_ggtt_pin(obj, 0, PIN_MAPPABLE | PIN_NONBLOCK);
	if (ret)
		goto out;
=======
	struct i915_ggtt *ggtt = &i915->ggtt;
	struct drm_device *dev = obj->base.dev;
	struct i915_vma *vma;
	struct drm_mm_node node;
	uint64_t remain, offset;
	char __user *user_data;
	int ret;
	bool hit_slow_path = false;

	if (i915_gem_object_is_tiled(obj))
		return -EFAULT;

	vma = i915_gem_object_ggtt_pin(obj, NULL, 0, 0,
				       PIN_MAPPABLE | PIN_NONBLOCK);
	if (!IS_ERR(vma)) {
		node.start = i915_ggtt_offset(vma);
		node.allocated = false;
		ret = i915_vma_put_fence(vma);
		if (ret) {
			i915_vma_unpin(vma);
			vma = ERR_PTR(ret);
		}
	}
	if (IS_ERR(vma)) {
		ret = insert_mappable_node(i915, &node, PAGE_SIZE);
		if (ret)
			goto out;

		ret = i915_gem_object_get_pages(obj);
		if (ret) {
			remove_mappable_node(&node);
			goto out;
		}

		i915_gem_object_pin_pages(obj);
	}
>>>>>>> v4.9.227

	ret = i915_gem_object_set_to_gtt_domain(obj, true);
	if (ret)
		goto out_unpin;

<<<<<<< HEAD
	ret = i915_gem_object_put_fence(obj);
	if (ret)
		goto out_unpin;

	user_data = to_user_ptr(args->data_ptr);
	remain = args->size;

	offset = i915_gem_obj_ggtt_offset(obj) + args->offset;

	intel_fb_obj_invalidate(obj, ORIGIN_GTT);

	while (remain > 0) {
=======
	intel_fb_obj_invalidate(obj, ORIGIN_CPU);
	obj->dirty = true;

	user_data = u64_to_user_ptr(args->data_ptr);
	offset = args->offset;
	remain = args->size;
	while (remain) {
>>>>>>> v4.9.227
		/* Operation in this page
		 *
		 * page_base = page offset within aperture
		 * page_offset = offset within page
		 * page_length = bytes to copy for this page
		 */
<<<<<<< HEAD
		page_base = offset & PAGE_MASK;
		page_offset = offset_in_page(offset);
		page_length = remain;
		if ((page_offset + remain) > PAGE_SIZE)
			page_length = PAGE_SIZE - page_offset;

		/* If we get a fault while copying data, then (presumably) our
		 * source page isn't available.  Return the error and we'll
		 * retry in the slow path.
		 */
		if (fast_user_write(dev_priv->gtt.mappable, page_base,
				    page_offset, user_data, page_length)) {
			ret = -EFAULT;
			goto out_flush;
=======
		u32 page_base = node.start;
		unsigned page_offset = offset_in_page(offset);
		unsigned page_length = PAGE_SIZE - page_offset;
		page_length = remain < page_length ? remain : page_length;
		if (node.allocated) {
			wmb(); /* flush the write before we modify the GGTT */
			ggtt->base.insert_page(&ggtt->base,
					       i915_gem_object_get_dma_address(obj, offset >> PAGE_SHIFT),
					       node.start, I915_CACHE_NONE, 0);
			wmb(); /* flush modifications to the GGTT (insert_page) */
		} else {
			page_base += offset & PAGE_MASK;
		}
		/* If we get a fault while copying data, then (presumably) our
		 * source page isn't available.  Return the error and we'll
		 * retry in the slow path.
		 * If the object is non-shmem backed, we retry again with the
		 * path that handles page fault.
		 */
		if (fast_user_write(&ggtt->mappable, page_base,
				    page_offset, user_data, page_length)) {
			hit_slow_path = true;
			mutex_unlock(&dev->struct_mutex);
			if (slow_user_access(&ggtt->mappable,
					     page_base,
					     page_offset, user_data,
					     page_length, true)) {
				ret = -EFAULT;
				mutex_lock(&dev->struct_mutex);
				goto out_flush;
			}

			mutex_lock(&dev->struct_mutex);
>>>>>>> v4.9.227
		}

		remain -= page_length;
		user_data += page_length;
		offset += page_length;
	}

out_flush:
<<<<<<< HEAD
	intel_fb_obj_flush(obj, false, ORIGIN_GTT);
out_unpin:
	i915_gem_object_ggtt_unpin(obj);
=======
	if (hit_slow_path) {
		if (ret == 0 &&
		    (obj->base.read_domains & I915_GEM_DOMAIN_GTT) == 0) {
			/* The user has modified the object whilst we tried
			 * reading from it, and we now have no idea what domain
			 * the pages should be in. As we have just been touching
			 * them directly, flush everything back to the GTT
			 * domain.
			 */
			ret = i915_gem_object_set_to_gtt_domain(obj, false);
		}
	}

	intel_fb_obj_flush(obj, false, ORIGIN_CPU);
out_unpin:
	if (node.allocated) {
		wmb();
		ggtt->base.clear_range(&ggtt->base,
				       node.start, node.size,
				       true);
		i915_gem_object_unpin_pages(obj);
		remove_mappable_node(&node);
	} else {
		i915_vma_unpin(vma);
	}
>>>>>>> v4.9.227
out:
	return ret;
}

/* Per-page copy function for the shmem pwrite fastpath.
 * Flushes invalid cachelines before writing to the target if
 * needs_clflush_before is set and flushes out any written cachelines after
 * writing if needs_clflush is set. */
static int
shmem_pwrite_fast(struct page *page, int shmem_page_offset, int page_length,
		  char __user *user_data,
		  bool page_do_bit17_swizzling,
		  bool needs_clflush_before,
		  bool needs_clflush_after)
{
	char *vaddr;
	int ret;

	if (unlikely(page_do_bit17_swizzling))
		return -EINVAL;

	vaddr = kmap_atomic(page);
	if (needs_clflush_before)
		drm_clflush_virt_range(vaddr + shmem_page_offset,
				       page_length);
	ret = __copy_from_user_inatomic(vaddr + shmem_page_offset,
					user_data, page_length);
	if (needs_clflush_after)
		drm_clflush_virt_range(vaddr + shmem_page_offset,
				       page_length);
	kunmap_atomic(vaddr);

	return ret ? -EFAULT : 0;
}

/* Only difference to the fast-path function is that this can handle bit17
 * and uses non-atomic copy and kmap functions. */
static int
shmem_pwrite_slow(struct page *page, int shmem_page_offset, int page_length,
		  char __user *user_data,
		  bool page_do_bit17_swizzling,
		  bool needs_clflush_before,
		  bool needs_clflush_after)
{
	char *vaddr;
	int ret;

	vaddr = kmap(page);
	if (unlikely(needs_clflush_before || page_do_bit17_swizzling))
		shmem_clflush_swizzled_range(vaddr + shmem_page_offset,
					     page_length,
					     page_do_bit17_swizzling);
	if (page_do_bit17_swizzling)
		ret = __copy_from_user_swizzled(vaddr, shmem_page_offset,
						user_data,
						page_length);
	else
		ret = __copy_from_user(vaddr + shmem_page_offset,
				       user_data,
				       page_length);
	if (needs_clflush_after)
		shmem_clflush_swizzled_range(vaddr + shmem_page_offset,
					     page_length,
					     page_do_bit17_swizzling);
	kunmap(page);

	return ret ? -EFAULT : 0;
}

static int
i915_gem_shmem_pwrite(struct drm_device *dev,
		      struct drm_i915_gem_object *obj,
		      struct drm_i915_gem_pwrite *args,
		      struct drm_file *file)
{
	ssize_t remain;
	loff_t offset;
	char __user *user_data;
	int shmem_page_offset, page_length, ret = 0;
	int obj_do_bit17_swizzling, page_do_bit17_swizzling;
	int hit_slowpath = 0;
<<<<<<< HEAD
	int needs_clflush_after = 0;
	int needs_clflush_before = 0;
	struct sg_page_iter sg_iter;

	user_data = to_user_ptr(args->data_ptr);
	remain = args->size;

	obj_do_bit17_swizzling = i915_gem_object_needs_bit17_swizzle(obj);

	if (obj->base.write_domain != I915_GEM_DOMAIN_CPU) {
		/* If we're not in the cpu write domain, set ourself into the gtt
		 * write domain and manually flush cachelines (if required). This
		 * optimizes for the case when the gpu will use the data
		 * right away and we therefore have to clflush anyway. */
		needs_clflush_after = cpu_write_needs_clflush(obj);
		ret = i915_gem_object_wait_rendering(obj, false);
		if (ret)
			return ret;
	}
	/* Same trick applies to invalidate partially written cachelines read
	 * before writing. */
	if ((obj->base.read_domains & I915_GEM_DOMAIN_CPU) == 0)
		needs_clflush_before =
			!cpu_cache_is_coherent(dev, obj->cache_level);

	ret = i915_gem_object_get_pages(obj);
	if (ret)
		return ret;

	intel_fb_obj_invalidate(obj, ORIGIN_CPU);

	i915_gem_object_pin_pages(obj);

	offset = args->offset;
	obj->dirty = 1;
=======
	unsigned int needs_clflush;
	struct sg_page_iter sg_iter;

	ret = i915_gem_obj_prepare_shmem_write(obj, &needs_clflush);
	if (ret)
		return ret;

	obj_do_bit17_swizzling = i915_gem_object_needs_bit17_swizzle(obj);
	user_data = u64_to_user_ptr(args->data_ptr);
	offset = args->offset;
	remain = args->size;
>>>>>>> v4.9.227

	for_each_sg_page(obj->pages->sgl, &sg_iter, obj->pages->nents,
			 offset >> PAGE_SHIFT) {
		struct page *page = sg_page_iter_page(&sg_iter);
		int partial_cacheline_write;

		if (remain <= 0)
			break;

		/* Operation in this page
		 *
		 * shmem_page_offset = offset within page in shmem file
		 * page_length = bytes to copy for this page
		 */
		shmem_page_offset = offset_in_page(offset);

		page_length = remain;
		if ((shmem_page_offset + page_length) > PAGE_SIZE)
			page_length = PAGE_SIZE - shmem_page_offset;

		/* If we don't overwrite a cacheline completely we need to be
		 * careful to have up-to-date data by first clflushing. Don't
		 * overcomplicate things and flush the entire patch. */
<<<<<<< HEAD
		partial_cacheline_write = needs_clflush_before &&
=======
		partial_cacheline_write = needs_clflush & CLFLUSH_BEFORE &&
>>>>>>> v4.9.227
			((shmem_page_offset | page_length)
				& (boot_cpu_data.x86_clflush_size - 1));

		page_do_bit17_swizzling = obj_do_bit17_swizzling &&
			(page_to_phys(page) & (1 << 17)) != 0;

		ret = shmem_pwrite_fast(page, shmem_page_offset, page_length,
					user_data, page_do_bit17_swizzling,
					partial_cacheline_write,
<<<<<<< HEAD
					needs_clflush_after);
=======
					needs_clflush & CLFLUSH_AFTER);
>>>>>>> v4.9.227
		if (ret == 0)
			goto next_page;

		hit_slowpath = 1;
		mutex_unlock(&dev->struct_mutex);
		ret = shmem_pwrite_slow(page, shmem_page_offset, page_length,
					user_data, page_do_bit17_swizzling,
					partial_cacheline_write,
<<<<<<< HEAD
					needs_clflush_after);
=======
					needs_clflush & CLFLUSH_AFTER);
>>>>>>> v4.9.227

		mutex_lock(&dev->struct_mutex);

		if (ret)
			goto out;

next_page:
		remain -= page_length;
		user_data += page_length;
		offset += page_length;
	}

out:
<<<<<<< HEAD
	i915_gem_object_unpin_pages(obj);
=======
	i915_gem_obj_finish_shmem_access(obj);
>>>>>>> v4.9.227

	if (hit_slowpath) {
		/*
		 * Fixup: Flush cpu caches in case we didn't flush the dirty
		 * cachelines in-line while writing and the object moved
		 * out of the cpu write domain while we've dropped the lock.
		 */
<<<<<<< HEAD
		if (!needs_clflush_after &&
		    obj->base.write_domain != I915_GEM_DOMAIN_CPU) {
			if (i915_gem_clflush_object(obj, obj->pin_display))
				needs_clflush_after = true;
		}
	}

	if (needs_clflush_after)
		i915_gem_chipset_flush(dev);
	else
		obj->cache_dirty = true;
=======
		if (!(needs_clflush & CLFLUSH_AFTER) &&
		    obj->base.write_domain != I915_GEM_DOMAIN_CPU) {
			if (i915_gem_clflush_object(obj, obj->pin_display))
				needs_clflush |= CLFLUSH_AFTER;
		}
	}

	if (needs_clflush & CLFLUSH_AFTER)
		i915_gem_chipset_flush(to_i915(dev));
>>>>>>> v4.9.227

	intel_fb_obj_flush(obj, false, ORIGIN_CPU);
	return ret;
}

/**
 * Writes data to the object referenced by handle.
<<<<<<< HEAD
=======
 * @dev: drm device
 * @data: ioctl data blob
 * @file: drm file
>>>>>>> v4.9.227
 *
 * On error, the contents of the buffer that were to be modified are undefined.
 */
int
i915_gem_pwrite_ioctl(struct drm_device *dev, void *data,
		      struct drm_file *file)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
=======
	struct drm_i915_private *dev_priv = to_i915(dev);
>>>>>>> v4.9.227
	struct drm_i915_gem_pwrite *args = data;
	struct drm_i915_gem_object *obj;
	int ret;

	if (args->size == 0)
		return 0;

	if (!access_ok(VERIFY_READ,
<<<<<<< HEAD
		       to_user_ptr(args->data_ptr),
=======
		       u64_to_user_ptr(args->data_ptr),
>>>>>>> v4.9.227
		       args->size))
		return -EFAULT;

	if (likely(!i915.prefault_disable)) {
<<<<<<< HEAD
		ret = fault_in_multipages_readable(to_user_ptr(args->data_ptr),
=======
		ret = fault_in_pages_readable(u64_to_user_ptr(args->data_ptr),
>>>>>>> v4.9.227
						   args->size);
		if (ret)
			return -EFAULT;
	}

<<<<<<< HEAD
	intel_runtime_pm_get(dev_priv);

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		goto put_rpm;

	obj = to_intel_bo(drm_gem_object_lookup(dev, file, args->handle));
	if (&obj->base == NULL) {
		ret = -ENOENT;
		goto unlock;
	}
=======
	obj = i915_gem_object_lookup(file, args->handle);
	if (!obj)
		return -ENOENT;
>>>>>>> v4.9.227

	/* Bounds check destination. */
	if (args->offset > obj->base.size ||
	    args->size > obj->base.size - args->offset) {
		ret = -EINVAL;
<<<<<<< HEAD
		goto out;
	}

	/* prime objects have no backing filp to GEM pread/pwrite
	 * pages from.
	 */
	if (!obj->base.filp) {
		ret = -EINVAL;
		goto out;
=======
		goto err;
>>>>>>> v4.9.227
	}

	trace_i915_gem_object_pwrite(obj, args->offset, args->size);

<<<<<<< HEAD
=======
	ret = __unsafe_wait_rendering(obj, to_rps_client(file), false);
	if (ret)
		goto err;

	intel_runtime_pm_get(dev_priv);

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		goto err_rpm;

>>>>>>> v4.9.227
	ret = -EFAULT;
	/* We can only do the GTT pwrite on untiled buffers, as otherwise
	 * it would end up going through the fenced access, and we'll get
	 * different detiling behavior between reading and writing.
	 * pread/pwrite currently are reading and writing from the CPU
	 * perspective, requiring manual detiling by the client.
	 */
<<<<<<< HEAD
	if (obj->tiling_mode == I915_TILING_NONE &&
	    obj->base.write_domain != I915_GEM_DOMAIN_CPU &&
	    cpu_write_needs_clflush(obj)) {
		ret = i915_gem_gtt_pwrite_fast(dev, obj, args, file);
=======
	if (!i915_gem_object_has_struct_page(obj) ||
	    cpu_write_needs_clflush(obj)) {
		ret = i915_gem_gtt_pwrite_fast(dev_priv, obj, args, file);
>>>>>>> v4.9.227
		/* Note that the gtt paths might fail with non-page-backed user
		 * pointers (e.g. gtt mappings when moving data between
		 * textures). Fallback to the shmem path in that case. */
	}

	if (ret == -EFAULT || ret == -ENOSPC) {
		if (obj->phys_handle)
			ret = i915_gem_phys_pwrite(obj, args, file);
		else
			ret = i915_gem_shmem_pwrite(dev, obj, args, file);
	}

<<<<<<< HEAD
out:
	drm_gem_object_unreference(&obj->base);
unlock:
	mutex_unlock(&dev->struct_mutex);
put_rpm:
	intel_runtime_pm_put(dev_priv);

	return ret;
}

int
i915_gem_check_wedge(struct i915_gpu_error *error,
		     bool interruptible)
{
	if (i915_reset_in_progress(error)) {
		/* Non-interruptible callers can't handle -EAGAIN, hence return
		 * -EIO unconditionally for these. */
		if (!interruptible)
			return -EIO;

		/* Recovery complete, but the reset failed ... */
		if (i915_terminally_wedged(error))
			return -EIO;

		/*
		 * Check if GPU Reset is in progress - we need intel_ring_begin
		 * to work properly to reinit the hw state while the gpu is
		 * still marked as reset-in-progress. Handle this with a flag.
		 */
		if (!error->reload_in_reset)
			return -EAGAIN;
	}

	return 0;
}

static void fake_irq(unsigned long data)
{
	wake_up_process((struct task_struct *)data);
}

static bool missed_irq(struct drm_i915_private *dev_priv,
		       struct intel_engine_cs *ring)
{
	return test_bit(ring->id, &dev_priv->gpu_error.missed_irq_rings);
}

static unsigned long local_clock_us(unsigned *cpu)
{
	unsigned long t;

	/* Cheaply and approximately convert from nanoseconds to microseconds.
	 * The result and subsequent calculations are also defined in the same
	 * approximate microseconds units. The principal source of timing
	 * error here is from the simple truncation.
	 *
	 * Note that local_clock() is only defined wrt to the current CPU;
	 * the comparisons are no longer valid if we switch CPUs. Instead of
	 * blocking preemption for the entire busywait, we can detect the CPU
	 * switch and use that as indicator of system load and a reason to
	 * stop busywaiting, see busywait_stop().
	 */
	*cpu = get_cpu();
	t = local_clock() >> 10;
	put_cpu();

	return t;
}

static bool busywait_stop(unsigned long timeout, unsigned cpu)
{
	unsigned this_cpu;

	if (time_after(local_clock_us(&this_cpu), timeout))
		return true;

	return this_cpu != cpu;
}

static int __i915_spin_request(struct drm_i915_gem_request *req, int state)
{
	unsigned long timeout;
	unsigned cpu;

	/* When waiting for high frequency requests, e.g. during synchronous
	 * rendering split between the CPU and GPU, the finite amount of time
	 * required to set up the irq and wait upon it limits the response
	 * rate. By busywaiting on the request completion for a short while we
	 * can service the high frequency waits as quick as possible. However,
	 * if it is a slow request, we want to sleep as quickly as possible.
	 * The tradeoff between waiting and sleeping is roughly the time it
	 * takes to sleep on a request, on the order of a microsecond.
	 */

	if (req->ring->irq_refcount)
		return -EBUSY;

	/* Only spin if we know the GPU is processing this request */
	if (!i915_gem_request_started(req, true))
		return -EAGAIN;

	timeout = local_clock_us(&cpu) + 5;
	while (!need_resched()) {
		if (i915_gem_request_completed(req, true))
			return 0;

		if (signal_pending_state(state, current))
			break;

		if (busywait_stop(timeout, cpu))
			break;

		cpu_relax_lowlatency();
	}

	if (i915_gem_request_completed(req, false))
		return 0;

	return -EAGAIN;
}

/**
 * __i915_wait_request - wait until execution of request has finished
 * @req: duh!
 * @reset_counter: reset sequence associated with the given request
 * @interruptible: do an interruptible wait (normally yes)
 * @timeout: in - how long to wait (NULL forever); out - how much time remaining
 *
 * Note: It is of utmost importance that the passed in seqno and reset_counter
 * values have been read by the caller in an smp safe manner. Where read-side
 * locks are involved, it is sufficient to read the reset_counter before
 * unlocking the lock that protects the seqno. For lockless tricks, the
 * reset_counter _must_ be read before, and an appropriate smp_rmb must be
 * inserted.
 *
 * Returns 0 if the request was found within the alloted time. Else returns the
 * errno with remaining time filled in timeout argument.
 */
int __i915_wait_request(struct drm_i915_gem_request *req,
			unsigned reset_counter,
			bool interruptible,
			s64 *timeout,
			struct intel_rps_client *rps)
{
	struct intel_engine_cs *ring = i915_gem_request_get_ring(req);
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	const bool irq_test_in_progress =
		ACCESS_ONCE(dev_priv->gpu_error.test_irq_rings) & intel_ring_flag(ring);
	int state = interruptible ? TASK_INTERRUPTIBLE : TASK_UNINTERRUPTIBLE;
	DEFINE_WAIT(wait);
	unsigned long timeout_expire;
	s64 before, now;
	int ret;

	WARN(!intel_irqs_enabled(dev_priv), "IRQs disabled");

	if (list_empty(&req->list))
		return 0;

	if (i915_gem_request_completed(req, true))
		return 0;

	timeout_expire = 0;
	if (timeout) {
		if (WARN_ON(*timeout < 0))
			return -EINVAL;

		if (*timeout == 0)
			return -ETIME;

		timeout_expire = jiffies + nsecs_to_jiffies_timeout(*timeout);
	}

	if (INTEL_INFO(dev_priv)->gen >= 6)
		gen6_rps_boost(dev_priv, rps, req->emitted_jiffies);

	/* Record current time in case interrupted by signal, or wedged */
	trace_i915_gem_request_wait_begin(req);
	before = ktime_get_raw_ns();

	/* Optimistic spin for the next jiffie before touching IRQs */
	ret = __i915_spin_request(req, state);
	if (ret == 0)
		goto out;

	if (!irq_test_in_progress && WARN_ON(!ring->irq_get(ring))) {
		ret = -ENODEV;
		goto out;
	}

	for (;;) {
		struct timer_list timer;

		prepare_to_wait(&ring->irq_queue, &wait, state);

		/* We need to check whether any gpu reset happened in between
		 * the caller grabbing the seqno and now ... */
		if (reset_counter != atomic_read(&dev_priv->gpu_error.reset_counter)) {
			/* ... but upgrade the -EAGAIN to an -EIO if the gpu
			 * is truely gone. */
			ret = i915_gem_check_wedge(&dev_priv->gpu_error, interruptible);
			if (ret == 0)
				ret = -EAGAIN;
			break;
		}

		if (i915_gem_request_completed(req, false)) {
			ret = 0;
			break;
		}

		if (signal_pending_state(state, current)) {
			ret = -ERESTARTSYS;
			break;
		}

		if (timeout && time_after_eq(jiffies, timeout_expire)) {
			ret = -ETIME;
			break;
		}

		timer.function = NULL;
		if (timeout || missed_irq(dev_priv, ring)) {
			unsigned long expire;

			setup_timer_on_stack(&timer, fake_irq, (unsigned long)current);
			expire = missed_irq(dev_priv, ring) ? jiffies + 1 : timeout_expire;
			mod_timer(&timer, expire);
		}

		io_schedule();

		if (timer.function) {
			del_singleshot_timer_sync(&timer);
			destroy_timer_on_stack(&timer);
		}
	}
	if (!irq_test_in_progress)
		ring->irq_put(ring);

	finish_wait(&ring->irq_queue, &wait);

out:
	now = ktime_get_raw_ns();
	trace_i915_gem_request_wait_end(req);

	if (timeout) {
		s64 tres = *timeout - (now - before);

		*timeout = tres < 0 ? 0 : tres;

		/*
		 * Apparently ktime isn't accurate enough and occasionally has a
		 * bit of mismatch in the jiffies<->nsecs<->ktime loop. So patch
		 * things up to make the test happy. We allow up to 1 jiffy.
		 *
		 * This is a regrssion from the timespec->ktime conversion.
		 */
		if (ret == -ETIME && *timeout < jiffies_to_usecs(1)*1000)
			*timeout = 0;
	}

	return ret;
}

int i915_gem_request_add_to_client(struct drm_i915_gem_request *req,
				   struct drm_file *file)
{
	struct drm_i915_private *dev_private;
	struct drm_i915_file_private *file_priv;

	WARN_ON(!req || !file || req->file_priv);

	if (!req || !file)
		return -EINVAL;

	if (req->file_priv)
		return -EINVAL;

	dev_private = req->ring->dev->dev_private;
	file_priv = file->driver_priv;

	spin_lock(&file_priv->mm.lock);
	req->file_priv = file_priv;
	list_add_tail(&req->client_list, &file_priv->mm.request_list);
	spin_unlock(&file_priv->mm.lock);

	req->pid = get_pid(task_pid(current));

	return 0;
}

static inline void
i915_gem_request_remove_from_client(struct drm_i915_gem_request *request)
{
	struct drm_i915_file_private *file_priv = request->file_priv;

	if (!file_priv)
		return;

	spin_lock(&file_priv->mm.lock);
	list_del(&request->client_list);
	request->file_priv = NULL;
	spin_unlock(&file_priv->mm.lock);

	put_pid(request->pid);
	request->pid = NULL;
}

static void i915_gem_request_retire(struct drm_i915_gem_request *request)
{
	trace_i915_gem_request_retire(request);

	/* We know the GPU must have read the request to have
	 * sent us the seqno + interrupt, so use the position
	 * of tail of the request to update the last known position
	 * of the GPU head.
	 *
	 * Note this requires that we are always called in request
	 * completion order.
	 */
	request->ringbuf->last_retired_head = request->postfix;

	list_del_init(&request->list);
	i915_gem_request_remove_from_client(request);

	i915_gem_request_unreference(request);
}

static void
__i915_gem_request_retire__upto(struct drm_i915_gem_request *req)
{
	struct intel_engine_cs *engine = req->ring;
	struct drm_i915_gem_request *tmp;

	lockdep_assert_held(&engine->dev->struct_mutex);

	if (list_empty(&req->list))
		return;

	do {
		tmp = list_first_entry(&engine->request_list,
				       typeof(*tmp), list);

		i915_gem_request_retire(tmp);
	} while (tmp != req);

	WARN_ON(i915_verify_lists(engine->dev));
}

/**
 * Waits for a request to be signaled, and cleans up the
 * request and object lists appropriately for that event.
 */
int
i915_wait_request(struct drm_i915_gem_request *req)
{
	struct drm_device *dev;
	struct drm_i915_private *dev_priv;
	bool interruptible;
	int ret;

	BUG_ON(req == NULL);

	dev = req->ring->dev;
	dev_priv = dev->dev_private;
	interruptible = dev_priv->mm.interruptible;

	BUG_ON(!mutex_is_locked(&dev->struct_mutex));

	ret = i915_gem_check_wedge(&dev_priv->gpu_error, interruptible);
	if (ret)
		return ret;

	ret = __i915_wait_request(req,
				  atomic_read(&dev_priv->gpu_error.reset_counter),
				  interruptible, NULL, NULL);
	if (ret)
		return ret;

	__i915_gem_request_retire__upto(req);
	return 0;
}

/**
 * Ensures that all rendering to the object has completed and the object is
 * safe to unbind from the GTT or access from the CPU.
 */
int
i915_gem_object_wait_rendering(struct drm_i915_gem_object *obj,
			       bool readonly)
{
	int ret, i;

	if (!obj->active)
		return 0;

	if (readonly) {
		if (obj->last_write_req != NULL) {
			ret = i915_wait_request(obj->last_write_req);
			if (ret)
				return ret;

			i = obj->last_write_req->ring->id;
			if (obj->last_read_req[i] == obj->last_write_req)
				i915_gem_object_retire__read(obj, i);
			else
				i915_gem_object_retire__write(obj);
		}
	} else {
		for (i = 0; i < I915_NUM_RINGS; i++) {
			if (obj->last_read_req[i] == NULL)
				continue;

			ret = i915_wait_request(obj->last_read_req[i]);
			if (ret)
				return ret;

			i915_gem_object_retire__read(obj, i);
		}
		RQ_BUG_ON(obj->active);
	}

	return 0;
}

static void
i915_gem_object_retire_request(struct drm_i915_gem_object *obj,
			       struct drm_i915_gem_request *req)
{
	int ring = req->ring->id;

	if (obj->last_read_req[ring] == req)
		i915_gem_object_retire__read(obj, ring);
	else if (obj->last_write_req == req)
		i915_gem_object_retire__write(obj);

	__i915_gem_request_retire__upto(req);
}

/* A nonblocking variant of the above wait. This is a highly dangerous routine
 * as the object state may change during this call.
 */
static __must_check int
i915_gem_object_wait_rendering__nonblocking(struct drm_i915_gem_object *obj,
					    struct intel_rps_client *rps,
					    bool readonly)
{
	struct drm_device *dev = obj->base.dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct drm_i915_gem_request *requests[I915_NUM_RINGS];
	unsigned reset_counter;
	int ret, i, n = 0;

	BUG_ON(!mutex_is_locked(&dev->struct_mutex));
	BUG_ON(!dev_priv->mm.interruptible);

	if (!obj->active)
		return 0;

	ret = i915_gem_check_wedge(&dev_priv->gpu_error, true);
	if (ret)
		return ret;

	reset_counter = atomic_read(&dev_priv->gpu_error.reset_counter);

	if (readonly) {
		struct drm_i915_gem_request *req;

		req = obj->last_write_req;
		if (req == NULL)
			return 0;

		requests[n++] = i915_gem_request_reference(req);
	} else {
		for (i = 0; i < I915_NUM_RINGS; i++) {
			struct drm_i915_gem_request *req;

			req = obj->last_read_req[i];
			if (req == NULL)
				continue;

			requests[n++] = i915_gem_request_reference(req);
		}
	}

	mutex_unlock(&dev->struct_mutex);
	for (i = 0; ret == 0 && i < n; i++)
		ret = __i915_wait_request(requests[i], reset_counter, true,
					  NULL, rps);
	mutex_lock(&dev->struct_mutex);

	for (i = 0; i < n; i++) {
		if (ret == 0)
			i915_gem_object_retire_request(obj, requests[i]);
		i915_gem_request_unreference(requests[i]);
	}

	return ret;
}

static struct intel_rps_client *to_rps_client(struct drm_file *file)
{
	struct drm_i915_file_private *fpriv = file->driver_priv;
	return &fpriv->rps;
=======
	i915_gem_object_put(obj);
	mutex_unlock(&dev->struct_mutex);
	intel_runtime_pm_put(dev_priv);

	return ret;

err_rpm:
	intel_runtime_pm_put(dev_priv);
err:
	i915_gem_object_put_unlocked(obj);
	return ret;
}

static inline enum fb_op_origin
write_origin(struct drm_i915_gem_object *obj, unsigned domain)
{
	return (domain == I915_GEM_DOMAIN_GTT ?
		obj->frontbuffer_ggtt_origin : ORIGIN_CPU);
>>>>>>> v4.9.227
}

/**
 * Called when user space prepares to use an object with the CPU, either
 * through the mmap ioctl's mapping or a GTT mapping.
<<<<<<< HEAD
=======
 * @dev: drm device
 * @data: ioctl data blob
 * @file: drm file
>>>>>>> v4.9.227
 */
int
i915_gem_set_domain_ioctl(struct drm_device *dev, void *data,
			  struct drm_file *file)
{
	struct drm_i915_gem_set_domain *args = data;
	struct drm_i915_gem_object *obj;
	uint32_t read_domains = args->read_domains;
	uint32_t write_domain = args->write_domain;
	int ret;

	/* Only handle setting domains to types used by the CPU. */
<<<<<<< HEAD
	if (write_domain & I915_GEM_GPU_DOMAINS)
		return -EINVAL;

	if (read_domains & I915_GEM_GPU_DOMAINS)
=======
	if ((write_domain | read_domains) & I915_GEM_GPU_DOMAINS)
>>>>>>> v4.9.227
		return -EINVAL;

	/* Having something in the write domain implies it's in the read
	 * domain, and only that read domain.  Enforce that in the request.
	 */
	if (write_domain != 0 && read_domains != write_domain)
		return -EINVAL;

<<<<<<< HEAD
	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		return ret;

	obj = to_intel_bo(drm_gem_object_lookup(dev, file, args->handle));
	if (&obj->base == NULL) {
		ret = -ENOENT;
		goto unlock;
	}
=======
	obj = i915_gem_object_lookup(file, args->handle);
	if (!obj)
		return -ENOENT;
>>>>>>> v4.9.227

	/* Try to flush the object off the GPU without holding the lock.
	 * We will repeat the flush holding the lock in the normal manner
	 * to catch cases where we are gazumped.
	 */
<<<<<<< HEAD
	ret = i915_gem_object_wait_rendering__nonblocking(obj,
							  to_rps_client(file),
							  !write_domain);
	if (ret)
		goto unref;
=======
	ret = __unsafe_wait_rendering(obj, to_rps_client(file), !write_domain);
	if (ret)
		goto err;

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		goto err;
>>>>>>> v4.9.227

	if (read_domains & I915_GEM_DOMAIN_GTT)
		ret = i915_gem_object_set_to_gtt_domain(obj, write_domain != 0);
	else
		ret = i915_gem_object_set_to_cpu_domain(obj, write_domain != 0);

	if (write_domain != 0)
<<<<<<< HEAD
		intel_fb_obj_invalidate(obj,
					write_domain == I915_GEM_DOMAIN_GTT ?
					ORIGIN_GTT : ORIGIN_CPU);

unref:
	drm_gem_object_unreference(&obj->base);
unlock:
	mutex_unlock(&dev->struct_mutex);
	return ret;
=======
		intel_fb_obj_invalidate(obj, write_origin(obj, write_domain));

	i915_gem_object_put(obj);
	mutex_unlock(&dev->struct_mutex);
	return ret;

err:
	i915_gem_object_put_unlocked(obj);
	return ret;
>>>>>>> v4.9.227
}

/**
 * Called when user space has done writes to this buffer
<<<<<<< HEAD
=======
 * @dev: drm device
 * @data: ioctl data blob
 * @file: drm file
>>>>>>> v4.9.227
 */
int
i915_gem_sw_finish_ioctl(struct drm_device *dev, void *data,
			 struct drm_file *file)
{
	struct drm_i915_gem_sw_finish *args = data;
	struct drm_i915_gem_object *obj;
<<<<<<< HEAD
	int ret = 0;

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		return ret;

	obj = to_intel_bo(drm_gem_object_lookup(dev, file, args->handle));
	if (&obj->base == NULL) {
		ret = -ENOENT;
		goto unlock;
	}

	/* Pinned buffers may be scanout, so flush the cache */
	if (obj->pin_display)
		i915_gem_object_flush_cpu_write_domain(obj);

	drm_gem_object_unreference(&obj->base);
unlock:
	mutex_unlock(&dev->struct_mutex);
	return ret;
}

/**
 * Maps the contents of an object, returning the address it is mapped
 * into.
=======
	int err = 0;

	obj = i915_gem_object_lookup(file, args->handle);
	if (!obj)
		return -ENOENT;

	/* Pinned buffers may be scanout, so flush the cache */
	if (READ_ONCE(obj->pin_display)) {
		err = i915_mutex_lock_interruptible(dev);
		if (!err) {
			i915_gem_object_flush_cpu_write_domain(obj);
			mutex_unlock(&dev->struct_mutex);
		}
	}

	i915_gem_object_put_unlocked(obj);
	return err;
}

static inline bool
__vma_matches(struct vm_area_struct *vma, struct file *filp,
	      unsigned long addr, unsigned long size)
{
	if (vma->vm_file != filp)
		return false;

	return vma->vm_start == addr &&
	       (vma->vm_end - vma->vm_start) == PAGE_ALIGN(size);
}

/**
 * i915_gem_mmap_ioctl - Maps the contents of an object, returning the address
 *			 it is mapped to.
 * @dev: drm device
 * @data: ioctl data blob
 * @file: drm file
>>>>>>> v4.9.227
 *
 * While the mapping holds a reference on the contents of the object, it doesn't
 * imply a ref on the object itself.
 *
 * IMPORTANT:
 *
 * DRM driver writers who look a this function as an example for how to do GEM
 * mmap support, please don't implement mmap support like here. The modern way
 * to implement DRM mmap support is with an mmap offset ioctl (like
 * i915_gem_mmap_gtt) and then using the mmap syscall on the DRM fd directly.
 * That way debug tooling like valgrind will understand what's going on, hiding
 * the mmap call in a driver private ioctl will break that. The i915 driver only
 * does cpu mmaps this way because we didn't know better.
 */
int
i915_gem_mmap_ioctl(struct drm_device *dev, void *data,
		    struct drm_file *file)
{
	struct drm_i915_gem_mmap *args = data;
<<<<<<< HEAD
	struct drm_gem_object *obj;
=======
	struct drm_i915_gem_object *obj;
>>>>>>> v4.9.227
	unsigned long addr;

	if (args->flags & ~(I915_MMAP_WC))
		return -EINVAL;

<<<<<<< HEAD
	if (args->flags & I915_MMAP_WC && !cpu_has_pat)
		return -ENODEV;

	obj = drm_gem_object_lookup(dev, file, args->handle);
	if (obj == NULL)
=======
	if (args->flags & I915_MMAP_WC && !boot_cpu_has(X86_FEATURE_PAT))
		return -ENODEV;

	obj = i915_gem_object_lookup(file, args->handle);
	if (!obj)
>>>>>>> v4.9.227
		return -ENOENT;

	/* prime objects have no backing filp to GEM mmap
	 * pages from.
	 */
<<<<<<< HEAD
	if (!obj->filp) {
		drm_gem_object_unreference_unlocked(obj);
		return -EINVAL;
	}

	addr = vm_mmap(obj->filp, 0, args->size,
=======
	if (!obj->base.filp) {
		i915_gem_object_put_unlocked(obj);
		return -EINVAL;
	}

	addr = vm_mmap(obj->base.filp, 0, args->size,
>>>>>>> v4.9.227
		       PROT_READ | PROT_WRITE, MAP_SHARED,
		       args->offset);
	if (args->flags & I915_MMAP_WC) {
		struct mm_struct *mm = current->mm;
		struct vm_area_struct *vma;

<<<<<<< HEAD
		down_write(&mm->mmap_sem);
		vma = find_vma(mm, addr);
		if (vma)
=======
		if (down_write_killable(&mm->mmap_sem)) {
			i915_gem_object_put_unlocked(obj);
			return -EINTR;
		}
		vma = find_vma(mm, addr);
		if (vma && __vma_matches(vma, obj->base.filp, addr, args->size))
>>>>>>> v4.9.227
			vma->vm_page_prot =
				pgprot_writecombine(vm_get_page_prot(vma->vm_flags));
		else
			addr = -ENOMEM;
		up_write(&mm->mmap_sem);
<<<<<<< HEAD
	}
	drm_gem_object_unreference_unlocked(obj);
=======

		/* This may race, but that's ok, it only gets set */
		WRITE_ONCE(obj->frontbuffer_ggtt_origin, ORIGIN_CPU);
	}
	i915_gem_object_put_unlocked(obj);
>>>>>>> v4.9.227
	if (IS_ERR((void *)addr))
		return addr;

	args->addr_ptr = (uint64_t) addr;

	return 0;
}

<<<<<<< HEAD
/**
 * i915_gem_fault - fault a page into the GTT
 * @vma: VMA in question
=======
static unsigned int tile_row_pages(struct drm_i915_gem_object *obj)
{
	u64 size;

	size = i915_gem_object_get_stride(obj);
	size *= i915_gem_object_get_tiling(obj) == I915_TILING_Y ? 32 : 8;

	return size >> PAGE_SHIFT;
}

/**
 * i915_gem_mmap_gtt_version - report the current feature set for GTT mmaps
 *
 * A history of the GTT mmap interface:
 *
 * 0 - Everything had to fit into the GTT. Both parties of a memcpy had to
 *     aligned and suitable for fencing, and still fit into the available
 *     mappable space left by the pinned display objects. A classic problem
 *     we called the page-fault-of-doom where we would ping-pong between
 *     two objects that could not fit inside the GTT and so the memcpy
 *     would page one object in at the expense of the other between every
 *     single byte.
 *
 * 1 - Objects can be any size, and have any compatible fencing (X Y, or none
 *     as set via i915_gem_set_tiling() [DRM_I915_GEM_SET_TILING]). If the
 *     object is too large for the available space (or simply too large
 *     for the mappable aperture!), a view is created instead and faulted
 *     into userspace. (This view is aligned and sized appropriately for
 *     fenced access.)
 *
 * Restrictions:
 *
 *  * snoopable objects cannot be accessed via the GTT. It can cause machine
 *    hangs on some architectures, corruption on others. An attempt to service
 *    a GTT page fault from a snoopable object will generate a SIGBUS.
 *
 *  * the object must be able to fit into RAM (physical memory, though no
 *    limited to the mappable aperture).
 *
 *
 * Caveats:
 *
 *  * a new GTT page fault will synchronize rendering from the GPU and flush
 *    all data to system memory. Subsequent access will not be synchronized.
 *
 *  * all mappings are revoked on runtime device suspend.
 *
 *  * there are only 8, 16 or 32 fence registers to share between all users
 *    (older machines require fence register for display and blitter access
 *    as well). Contention of the fence registers will cause the previous users
 *    to be unmapped and any new access will generate new page faults.
 *
 *  * running out of memory while servicing a fault may generate a SIGBUS,
 *    rather than the expected SIGSEGV.
 */
int i915_gem_mmap_gtt_version(void)
{
	return 1;
}

/**
 * i915_gem_fault - fault a page into the GTT
 * @area: CPU VMA in question
>>>>>>> v4.9.227
 * @vmf: fault info
 *
 * The fault handler is set up by drm_gem_mmap() when a object is GTT mapped
 * from userspace.  The fault handler takes care of binding the object to
 * the GTT (if needed), allocating and programming a fence register (again,
 * only if needed based on whether the old reg is still valid or the object
 * is tiled) and inserting a new PTE into the faulting process.
 *
 * Note that the faulting process may involve evicting existing objects
 * from the GTT and/or fence registers to make room.  So performance may
 * suffer if the GTT working set is large or there are few fence registers
 * left.
<<<<<<< HEAD
 */
int i915_gem_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct drm_i915_gem_object *obj = to_intel_bo(vma->vm_private_data);
	struct drm_device *dev = obj->base.dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct i915_ggtt_view view = i915_ggtt_view_normal;
	pgoff_t page_offset;
	unsigned long pfn;
	int ret = 0;
	bool write = !!(vmf->flags & FAULT_FLAG_WRITE);

	intel_runtime_pm_get(dev_priv);

	/* We don't use vmf->pgoff since that has the fake offset */
	page_offset = ((unsigned long)vmf->virtual_address - vma->vm_start) >>
		PAGE_SHIFT;

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		goto out;

	trace_i915_gem_object_fault(obj, page_offset, true, write);

	/* Try to flush the object off the GPU first without holding the lock.
	 * Upon reacquiring the lock, we will perform our sanity checks and then
	 * repeat the flush holding the lock in the normal manner to catch cases
	 * where we are gazumped.
	 */
	ret = i915_gem_object_wait_rendering__nonblocking(obj, NULL, !write);
	if (ret)
		goto unlock;
=======
 *
 * The current feature set supported by i915_gem_fault() and thus GTT mmaps
 * is exposed via I915_PARAM_MMAP_GTT_VERSION (see i915_gem_mmap_gtt_version).
 */
int i915_gem_fault(struct vm_area_struct *area, struct vm_fault *vmf)
{
#define MIN_CHUNK_PAGES ((1 << 20) >> PAGE_SHIFT) /* 1 MiB */
	struct drm_i915_gem_object *obj = to_intel_bo(area->vm_private_data);
	struct drm_device *dev = obj->base.dev;
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct i915_ggtt *ggtt = &dev_priv->ggtt;
	bool write = !!(vmf->flags & FAULT_FLAG_WRITE);
	struct i915_vma *vma;
	pgoff_t page_offset;
	unsigned int flags;
	int ret;

	/* Sanity check that we allow writing into this object */
	if (i915_gem_object_is_readonly(obj) && write)
		return VM_FAULT_SIGBUS;

	/* We don't use vmf->pgoff since that has the fake offset */
	page_offset = ((unsigned long)vmf->virtual_address - area->vm_start) >>
		PAGE_SHIFT;

	trace_i915_gem_object_fault(obj, page_offset, true, write);

	/* Try to flush the object off the GPU first without holding the lock.
	 * Upon acquiring the lock, we will perform our sanity checks and then
	 * repeat the flush holding the lock in the normal manner to catch cases
	 * where we are gazumped.
	 */
	ret = __unsafe_wait_rendering(obj, NULL, !write);
	if (ret)
		goto err;

	intel_runtime_pm_get(dev_priv);

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		goto err_rpm;
>>>>>>> v4.9.227

	/* Access to snoopable pages through the GTT is incoherent. */
	if (obj->cache_level != I915_CACHE_NONE && !HAS_LLC(dev)) {
		ret = -EFAULT;
<<<<<<< HEAD
		goto unlock;
	}

	/* Use a partial view if the object is bigger than the aperture. */
	if (obj->base.size >= dev_priv->gtt.mappable_end &&
	    obj->tiling_mode == I915_TILING_NONE) {
		static const unsigned int chunk_size = 256; // 1 MiB
=======
		goto err_unlock;
	}

	/* If the object is smaller than a couple of partial vma, it is
	 * not worth only creating a single partial vma - we may as well
	 * clear enough space for the full object.
	 */
	flags = PIN_MAPPABLE;
	if (obj->base.size > 2 * MIN_CHUNK_PAGES << PAGE_SHIFT)
		flags |= PIN_NONBLOCK | PIN_NONFAULT;

	/* Now pin it into the GTT as needed */
	vma = i915_gem_object_ggtt_pin(obj, NULL, 0, 0, flags);
	if (IS_ERR(vma)) {
		struct i915_ggtt_view view;
		unsigned int chunk_size;

		/* Use a partial view if it is bigger than available space */
		chunk_size = MIN_CHUNK_PAGES;
		if (i915_gem_object_is_tiled(obj))
			chunk_size = roundup(chunk_size, tile_row_pages(obj));
>>>>>>> v4.9.227

		memset(&view, 0, sizeof(view));
		view.type = I915_GGTT_VIEW_PARTIAL;
		view.params.partial.offset = rounddown(page_offset, chunk_size);
		view.params.partial.size =
<<<<<<< HEAD
			min_t(unsigned int,
			      chunk_size,
			      (vma->vm_end - vma->vm_start)/PAGE_SIZE -
			      view.params.partial.offset);
	}

	/* Now pin it into the GTT if needed */
	ret = i915_gem_object_ggtt_pin(obj, &view, 0, PIN_MAPPABLE);
	if (ret)
		goto unlock;

	ret = i915_gem_object_set_to_gtt_domain(obj, write);
	if (ret)
		goto unpin;

	ret = i915_gem_object_get_fence(obj);
	if (ret)
		goto unpin;

	/* Finally, remap it using the new GTT offset */
	pfn = dev_priv->gtt.mappable_base +
		i915_gem_obj_ggtt_offset_view(obj, &view);
	pfn >>= PAGE_SHIFT;

	if (unlikely(view.type == I915_GGTT_VIEW_PARTIAL)) {
		/* Overriding existing pages in partial view does not cause
		 * us any trouble as TLBs are still valid because the fault
		 * is due to userspace losing part of the mapping or never
		 * having accessed it before (at this partials' range).
		 */
		unsigned long base = vma->vm_start +
				     (view.params.partial.offset << PAGE_SHIFT);
		unsigned int i;

		for (i = 0; i < view.params.partial.size; i++) {
			ret = vm_insert_pfn(vma, base + i * PAGE_SIZE, pfn + i);
			if (ret)
				break;
		}

		obj->fault_mappable = true;
	} else {
		if (!obj->fault_mappable) {
			unsigned long size = min_t(unsigned long,
						   vma->vm_end - vma->vm_start,
						   obj->base.size);
			int i;

			for (i = 0; i < size >> PAGE_SHIFT; i++) {
				ret = vm_insert_pfn(vma,
						    (unsigned long)vma->vm_start + i * PAGE_SIZE,
						    pfn + i);
				if (ret)
					break;
			}

			obj->fault_mappable = true;
		} else
			ret = vm_insert_pfn(vma,
					    (unsigned long)vmf->virtual_address,
					    pfn + page_offset);
	}
unpin:
	i915_gem_object_ggtt_unpin_view(obj, &view);
unlock:
	mutex_unlock(&dev->struct_mutex);
out:
=======
			min_t(unsigned int, chunk_size,
			      (area->vm_end - area->vm_start) / PAGE_SIZE -
			      view.params.partial.offset);

		/* If the partial covers the entire object, just create a
		 * normal VMA.
		 */
		if (chunk_size >= obj->base.size >> PAGE_SHIFT)
			view.type = I915_GGTT_VIEW_NORMAL;

		/* Userspace is now writing through an untracked VMA, abandon
		 * all hope that the hardware is able to track future writes.
		 */
		obj->frontbuffer_ggtt_origin = ORIGIN_CPU;

		vma = i915_gem_object_ggtt_pin(obj, &view, 0, 0, PIN_MAPPABLE);
	}
	if (IS_ERR(vma)) {
		ret = PTR_ERR(vma);
		goto err_unlock;
	}

	ret = i915_gem_object_set_to_gtt_domain(obj, write);
	if (ret)
		goto err_unpin;

	ret = i915_vma_get_fence(vma);
	if (ret)
		goto err_unpin;

	/* Finally, remap it using the new GTT offset */
	ret = remap_io_mapping(area,
			       area->vm_start + (vma->ggtt_view.params.partial.offset << PAGE_SHIFT),
			       (ggtt->mappable_base + vma->node.start) >> PAGE_SHIFT,
			       min_t(u64, vma->size, area->vm_end - area->vm_start),
			       &ggtt->mappable);
	if (ret)
		goto err_unpin;

	obj->fault_mappable = true;
err_unpin:
	__i915_vma_unpin(vma);
err_unlock:
	mutex_unlock(&dev->struct_mutex);
err_rpm:
	intel_runtime_pm_put(dev_priv);
err:
>>>>>>> v4.9.227
	switch (ret) {
	case -EIO:
		/*
		 * We eat errors when the gpu is terminally wedged to avoid
		 * userspace unduly crashing (gl has no provisions for mmaps to
		 * fail). But any other -EIO isn't ours (e.g. swap in failure)
		 * and so needs to be reported.
		 */
		if (!i915_terminally_wedged(&dev_priv->gpu_error)) {
			ret = VM_FAULT_SIGBUS;
			break;
		}
	case -EAGAIN:
		/*
		 * EAGAIN means the gpu is hung and we'll wait for the error
		 * handler to reset everything when re-faulting in
		 * i915_mutex_lock_interruptible.
		 */
	case 0:
	case -ERESTARTSYS:
	case -EINTR:
	case -EBUSY:
		/*
		 * EBUSY is ok: this just means that another thread
		 * already did the job.
		 */
		ret = VM_FAULT_NOPAGE;
		break;
	case -ENOMEM:
		ret = VM_FAULT_OOM;
		break;
	case -ENOSPC:
	case -EFAULT:
		ret = VM_FAULT_SIGBUS;
		break;
	default:
		WARN_ONCE(ret, "unhandled error in i915_gem_fault: %i\n", ret);
		ret = VM_FAULT_SIGBUS;
		break;
	}
<<<<<<< HEAD

	intel_runtime_pm_put(dev_priv);
=======
>>>>>>> v4.9.227
	return ret;
}

/**
 * i915_gem_release_mmap - remove physical page mappings
 * @obj: obj in question
 *
 * Preserve the reservation of the mmapping with the DRM core code, but
 * relinquish ownership of the pages back to the system.
 *
 * It is vital that we remove the page mapping if we have mapped a tiled
 * object through the GTT and then lose the fence register due to
 * resource pressure. Similarly if the object has been moved out of the
 * aperture, than pages mapped into userspace must be revoked. Removing the
 * mapping will then trigger a page fault on the next user access, allowing
 * fixup by i915_gem_fault().
 */
void
i915_gem_release_mmap(struct drm_i915_gem_object *obj)
{
<<<<<<< HEAD
=======
	/* Serialisation between user GTT access and our code depends upon
	 * revoking the CPU's PTE whilst the mutex is held. The next user
	 * pagefault then has to wait until we release the mutex.
	 */
	lockdep_assert_held(&obj->base.dev->struct_mutex);

>>>>>>> v4.9.227
	if (!obj->fault_mappable)
		return;

	drm_vma_node_unmap(&obj->base.vma_node,
			   obj->base.dev->anon_inode->i_mapping);
<<<<<<< HEAD
=======

	/* Ensure that the CPU's PTE are revoked and there are not outstanding
	 * memory transactions from userspace before we return. The TLB
	 * flushing implied above by changing the PTE above *should* be
	 * sufficient, an extra barrier here just provides us with a bit
	 * of paranoid documentation about our requirement to serialise
	 * memory writes before touching registers / GSM.
	 */
	wmb();

>>>>>>> v4.9.227
	obj->fault_mappable = false;
}

void
i915_gem_release_all_mmaps(struct drm_i915_private *dev_priv)
{
	struct drm_i915_gem_object *obj;

	list_for_each_entry(obj, &dev_priv->mm.bound_list, global_list)
		i915_gem_release_mmap(obj);
}

<<<<<<< HEAD
uint32_t
i915_gem_get_gtt_size(struct drm_device *dev, uint32_t size, int tiling_mode)
{
	uint32_t gtt_size;

	if (INTEL_INFO(dev)->gen >= 4 ||
=======
/**
 * i915_gem_get_ggtt_size - return required global GTT size for an object
 * @dev_priv: i915 device
 * @size: object size
 * @tiling_mode: tiling mode
 *
 * Return the required global GTT size for an object, taking into account
 * potential fence register mapping.
 */
u64 i915_gem_get_ggtt_size(struct drm_i915_private *dev_priv,
			   u64 size, int tiling_mode)
{
	u64 ggtt_size;

	GEM_BUG_ON(size == 0);

	if (INTEL_GEN(dev_priv) >= 4 ||
>>>>>>> v4.9.227
	    tiling_mode == I915_TILING_NONE)
		return size;

	/* Previous chips need a power-of-two fence region when tiling */
<<<<<<< HEAD
	if (INTEL_INFO(dev)->gen == 3)
		gtt_size = 1024*1024;
	else
		gtt_size = 512*1024;

	while (gtt_size < size)
		gtt_size <<= 1;

	return gtt_size;
}

/**
 * i915_gem_get_gtt_alignment - return required GTT alignment for an object
 * @obj: object to check
 *
 * Return the required GTT alignment for an object, taking into account
 * potential fence register mapping.
 */
uint32_t
i915_gem_get_gtt_alignment(struct drm_device *dev, uint32_t size,
			   int tiling_mode, bool fenced)
{
=======
	if (IS_GEN3(dev_priv))
		ggtt_size = 1024*1024;
	else
		ggtt_size = 512*1024;

	while (ggtt_size < size)
		ggtt_size <<= 1;

	return ggtt_size;
}

/**
 * i915_gem_get_ggtt_alignment - return required global GTT alignment
 * @dev_priv: i915 device
 * @size: object size
 * @tiling_mode: tiling mode
 * @fenced: is fenced alignment required or not
 *
 * Return the required global GTT alignment for an object, taking into account
 * potential fence register mapping.
 */
u64 i915_gem_get_ggtt_alignment(struct drm_i915_private *dev_priv, u64 size,
				int tiling_mode, bool fenced)
{
	GEM_BUG_ON(size == 0);

>>>>>>> v4.9.227
	/*
	 * Minimum alignment is 4k (GTT page size), but might be greater
	 * if a fence register is needed for the object.
	 */
<<<<<<< HEAD
	if (INTEL_INFO(dev)->gen >= 4 || (!fenced && IS_G33(dev)) ||
=======
	if (INTEL_GEN(dev_priv) >= 4 || (!fenced && IS_G33(dev_priv)) ||
>>>>>>> v4.9.227
	    tiling_mode == I915_TILING_NONE)
		return 4096;

	/*
	 * Previous chips need to be aligned to the size of the smallest
	 * fence register that can contain the object.
	 */
<<<<<<< HEAD
	return i915_gem_get_gtt_size(dev, size, tiling_mode);
=======
	return i915_gem_get_ggtt_size(dev_priv, size, tiling_mode);
>>>>>>> v4.9.227
}

static int i915_gem_object_create_mmap_offset(struct drm_i915_gem_object *obj)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = obj->base.dev->dev_private;
	int ret;

	if (drm_vma_node_has_offset(&obj->base.vma_node))
		return 0;

	dev_priv->mm.shrinker_no_lock_stealing = true;

	ret = drm_gem_create_mmap_offset(&obj->base);
	if (ret != -ENOSPC)
		goto out;

	/* Badly fragmented mmap space? The only way we can recover
	 * space is by destroying unwanted objects. We can't randomly release
	 * mmap_offsets as userspace expects them to be persistent for the
	 * lifetime of the objects. The closest we can is to release the
	 * offsets on purgeable objects by truncating it and marking it purged,
	 * which prevents userspace from ever using that object again.
	 */
	i915_gem_shrink(dev_priv,
			obj->base.size >> PAGE_SHIFT,
			I915_SHRINK_BOUND |
			I915_SHRINK_UNBOUND |
			I915_SHRINK_PURGEABLE);
	ret = drm_gem_create_mmap_offset(&obj->base);
	if (ret != -ENOSPC)
		goto out;

	i915_gem_shrink_all(dev_priv);
	ret = drm_gem_create_mmap_offset(&obj->base);
out:
	dev_priv->mm.shrinker_no_lock_stealing = false;

	return ret;
=======
	struct drm_i915_private *dev_priv = to_i915(obj->base.dev);
	int err;

	err = drm_gem_create_mmap_offset(&obj->base);
	if (!err)
		return 0;

	/* We can idle the GPU locklessly to flush stale objects, but in order
	 * to claim that space for ourselves, we need to take the big
	 * struct_mutex to free the requests+objects and allocate our slot.
	 */
	err = i915_gem_wait_for_idle(dev_priv, I915_WAIT_INTERRUPTIBLE);
	if (err)
		return err;

	err = i915_mutex_lock_interruptible(&dev_priv->drm);
	if (!err) {
		i915_gem_retire_requests(dev_priv);
		err = drm_gem_create_mmap_offset(&obj->base);
		mutex_unlock(&dev_priv->drm.struct_mutex);
	}

	return err;
>>>>>>> v4.9.227
}

static void i915_gem_object_free_mmap_offset(struct drm_i915_gem_object *obj)
{
	drm_gem_free_mmap_offset(&obj->base);
}

int
i915_gem_mmap_gtt(struct drm_file *file,
		  struct drm_device *dev,
		  uint32_t handle,
		  uint64_t *offset)
{
	struct drm_i915_gem_object *obj;
	int ret;

<<<<<<< HEAD
	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		return ret;

	obj = to_intel_bo(drm_gem_object_lookup(dev, file, handle));
	if (&obj->base == NULL) {
		ret = -ENOENT;
		goto unlock;
	}

	if (obj->madv != I915_MADV_WILLNEED) {
		DRM_DEBUG("Attempting to mmap a purgeable buffer\n");
		ret = -EFAULT;
		goto out;
	}

	ret = i915_gem_object_create_mmap_offset(obj);
	if (ret)
		goto out;

	*offset = drm_vma_node_offset_addr(&obj->base.vma_node);

out:
	drm_gem_object_unreference(&obj->base);
unlock:
	mutex_unlock(&dev->struct_mutex);
=======
	obj = i915_gem_object_lookup(file, handle);
	if (!obj)
		return -ENOENT;

	ret = i915_gem_object_create_mmap_offset(obj);
	if (ret == 0)
		*offset = drm_vma_node_offset_addr(&obj->base.vma_node);

	i915_gem_object_put_unlocked(obj);
>>>>>>> v4.9.227
	return ret;
}

/**
 * i915_gem_mmap_gtt_ioctl - prepare an object for GTT mmap'ing
 * @dev: DRM device
 * @data: GTT mapping ioctl data
 * @file: GEM object info
 *
 * Simply returns the fake offset to userspace so it can mmap it.
 * The mmap call will end up in drm_gem_mmap(), which will set things
 * up so we can get faults in the handler above.
 *
 * The fault handler will take care of binding the object into the GTT
 * (since it may have been evicted to make room for something), allocating
 * a fence register, and mapping the appropriate aperture address into
 * userspace.
 */
int
i915_gem_mmap_gtt_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file)
{
	struct drm_i915_gem_mmap_gtt *args = data;

	return i915_gem_mmap_gtt(file, dev, args->handle, &args->offset);
}

/* Immediately discard the backing storage */
static void
i915_gem_object_truncate(struct drm_i915_gem_object *obj)
{
	i915_gem_object_free_mmap_offset(obj);

	if (obj->base.filp == NULL)
		return;

	/* Our goal here is to return as much of the memory as
	 * is possible back to the system as we are called from OOM.
	 * To do this we must instruct the shmfs to drop all of its
	 * backing pages, *now*.
	 */
	shmem_truncate_range(file_inode(obj->base.filp), 0, (loff_t)-1);
	obj->madv = __I915_MADV_PURGED;
}

/* Try to discard unwanted pages */
static void
i915_gem_object_invalidate(struct drm_i915_gem_object *obj)
{
	struct address_space *mapping;

	switch (obj->madv) {
	case I915_MADV_DONTNEED:
		i915_gem_object_truncate(obj);
	case __I915_MADV_PURGED:
		return;
	}

	if (obj->base.filp == NULL)
		return;

<<<<<<< HEAD
	mapping = file_inode(obj->base.filp)->i_mapping,
=======
	mapping = obj->base.filp->f_mapping,
>>>>>>> v4.9.227
	invalidate_mapping_pages(mapping, 0, (loff_t)-1);
}

static void
i915_gem_object_put_pages_gtt(struct drm_i915_gem_object *obj)
{
<<<<<<< HEAD
	struct sg_page_iter sg_iter;
=======
	struct sgt_iter sgt_iter;
	struct page *page;
>>>>>>> v4.9.227
	int ret;

	BUG_ON(obj->madv == __I915_MADV_PURGED);

	ret = i915_gem_object_set_to_cpu_domain(obj, true);
<<<<<<< HEAD
	if (ret) {
		/* In the event of a disaster, abandon all caches and
		 * hope for the best.
		 */
		WARN_ON(ret != -EIO);
=======
	if (WARN_ON(ret)) {
		/* In the event of a disaster, abandon all caches and
		 * hope for the best.
		 */
>>>>>>> v4.9.227
		i915_gem_clflush_object(obj, true);
		obj->base.read_domains = obj->base.write_domain = I915_GEM_DOMAIN_CPU;
	}

	i915_gem_gtt_finish_object(obj);

	if (i915_gem_object_needs_bit17_swizzle(obj))
		i915_gem_object_save_bit_17_swizzle(obj);

	if (obj->madv == I915_MADV_DONTNEED)
		obj->dirty = 0;

<<<<<<< HEAD
	for_each_sg_page(obj->pages->sgl, &sg_iter, obj->pages->nents, 0) {
		struct page *page = sg_page_iter_page(&sg_iter);

=======
	for_each_sgt_page(page, sgt_iter, obj->pages) {
>>>>>>> v4.9.227
		if (obj->dirty)
			set_page_dirty(page);

		if (obj->madv == I915_MADV_WILLNEED)
			mark_page_accessed(page);

<<<<<<< HEAD
		page_cache_release(page);
=======
		put_page(page);
>>>>>>> v4.9.227
	}
	obj->dirty = 0;

	sg_free_table(obj->pages);
	kfree(obj->pages);
}

int
i915_gem_object_put_pages(struct drm_i915_gem_object *obj)
{
	const struct drm_i915_gem_object_ops *ops = obj->ops;

	if (obj->pages == NULL)
		return 0;

	if (obj->pages_pin_count)
		return -EBUSY;

<<<<<<< HEAD
	BUG_ON(i915_gem_obj_bound_any(obj));
=======
	GEM_BUG_ON(obj->bind_count);
>>>>>>> v4.9.227

	/* ->put_pages might need to allocate memory for the bit17 swizzle
	 * array, hence protect them from being reaped by removing them from gtt
	 * lists early. */
	list_del(&obj->global_list);

<<<<<<< HEAD
=======
	if (obj->mapping) {
		void *ptr;

		ptr = ptr_mask_bits(obj->mapping);
		if (is_vmalloc_addr(ptr))
			vunmap(ptr);
		else
			kunmap(kmap_to_page(ptr));

		obj->mapping = NULL;
	}

>>>>>>> v4.9.227
	ops->put_pages(obj);
	obj->pages = NULL;

	i915_gem_object_invalidate(obj);

	return 0;
}

static int
i915_gem_object_get_pages_gtt(struct drm_i915_gem_object *obj)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = obj->base.dev->dev_private;
=======
	struct drm_i915_private *dev_priv = to_i915(obj->base.dev);
>>>>>>> v4.9.227
	int page_count, i;
	struct address_space *mapping;
	struct sg_table *st;
	struct scatterlist *sg;
<<<<<<< HEAD
	struct sg_page_iter sg_iter;
=======
	struct sgt_iter sgt_iter;
>>>>>>> v4.9.227
	struct page *page;
	unsigned long last_pfn = 0;	/* suppress gcc warning */
	int ret;
	gfp_t gfp;

	/* Assert that the object is not currently in any GPU domain. As it
	 * wasn't in the GTT, there shouldn't be any way it could have been in
	 * a GPU cache
	 */
	BUG_ON(obj->base.read_domains & I915_GEM_GPU_DOMAINS);
	BUG_ON(obj->base.write_domain & I915_GEM_GPU_DOMAINS);

	st = kmalloc(sizeof(*st), GFP_KERNEL);
	if (st == NULL)
		return -ENOMEM;

	page_count = obj->base.size / PAGE_SIZE;
	if (sg_alloc_table(st, page_count, GFP_KERNEL)) {
		kfree(st);
		return -ENOMEM;
	}

	/* Get the list of pages out of our struct file.  They'll be pinned
	 * at this point until we release them.
	 *
	 * Fail silently without starting the shrinker
	 */
<<<<<<< HEAD
	mapping = file_inode(obj->base.filp)->i_mapping;
=======
	mapping = obj->base.filp->f_mapping;
>>>>>>> v4.9.227
	gfp = mapping_gfp_constraint(mapping, ~(__GFP_IO | __GFP_RECLAIM));
	gfp |= __GFP_NORETRY | __GFP_NOWARN;
	sg = st->sgl;
	st->nents = 0;
	for (i = 0; i < page_count; i++) {
		page = shmem_read_mapping_page_gfp(mapping, i, gfp);
		if (IS_ERR(page)) {
			i915_gem_shrink(dev_priv,
					page_count,
					I915_SHRINK_BOUND |
					I915_SHRINK_UNBOUND |
					I915_SHRINK_PURGEABLE);
			page = shmem_read_mapping_page_gfp(mapping, i, gfp);
		}
		if (IS_ERR(page)) {
			/* We've tried hard to allocate the memory by reaping
			 * our own buffer, now let the real VM do its job and
			 * go down in flames if truly OOM.
			 */
			i915_gem_shrink_all(dev_priv);
			page = shmem_read_mapping_page(mapping, i);
			if (IS_ERR(page)) {
				ret = PTR_ERR(page);
<<<<<<< HEAD
				goto err_pages;
=======
				goto err_sg;
>>>>>>> v4.9.227
			}
		}
#ifdef CONFIG_SWIOTLB
		if (swiotlb_nr_tbl()) {
			st->nents++;
			sg_set_page(sg, page, PAGE_SIZE, 0);
			sg = sg_next(sg);
			continue;
		}
#endif
		if (!i || page_to_pfn(page) != last_pfn + 1) {
			if (i)
				sg = sg_next(sg);
			st->nents++;
			sg_set_page(sg, page, PAGE_SIZE, 0);
		} else {
			sg->length += PAGE_SIZE;
		}
		last_pfn = page_to_pfn(page);

		/* Check that the i965g/gm workaround works. */
		WARN_ON((gfp & __GFP_DMA32) && (last_pfn >= 0x00100000UL));
	}
#ifdef CONFIG_SWIOTLB
	if (!swiotlb_nr_tbl())
#endif
		sg_mark_end(sg);
	obj->pages = st;

	ret = i915_gem_gtt_prepare_object(obj);
	if (ret)
		goto err_pages;

	if (i915_gem_object_needs_bit17_swizzle(obj))
		i915_gem_object_do_bit_17_swizzle(obj);

<<<<<<< HEAD
	if (obj->tiling_mode != I915_TILING_NONE &&
=======
	if (i915_gem_object_is_tiled(obj) &&
>>>>>>> v4.9.227
	    dev_priv->quirks & QUIRK_PIN_SWIZZLED_PAGES)
		i915_gem_object_pin_pages(obj);

	return 0;

<<<<<<< HEAD
err_pages:
	sg_mark_end(sg);
	for_each_sg_page(st->sgl, &sg_iter, st->nents, 0)
		page_cache_release(sg_page_iter_page(&sg_iter));
=======
err_sg:
	sg_mark_end(sg);
err_pages:
	for_each_sgt_page(page, sgt_iter, st)
		put_page(page);
>>>>>>> v4.9.227
	sg_free_table(st);
	kfree(st);

	/* shmemfs first checks if there is enough memory to allocate the page
	 * and reports ENOSPC should there be insufficient, along with the usual
	 * ENOMEM for a genuine allocation failure.
	 *
	 * We use ENOSPC in our driver to mean that we have run out of aperture
	 * space and so want to translate the error from shmemfs back to our
	 * usual understanding of ENOMEM.
	 */
	if (ret == -ENOSPC)
		ret = -ENOMEM;

	return ret;
}

/* Ensure that the associated pages are gathered from the backing storage
 * and pinned into our object. i915_gem_object_get_pages() may be called
 * multiple times before they are released by a single call to
 * i915_gem_object_put_pages() - once the pages are no longer referenced
 * either as a result of memory pressure (reaping pages under the shrinker)
 * or as the object is itself released.
 */
int
i915_gem_object_get_pages(struct drm_i915_gem_object *obj)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = obj->base.dev->dev_private;
=======
	struct drm_i915_private *dev_priv = to_i915(obj->base.dev);
>>>>>>> v4.9.227
	const struct drm_i915_gem_object_ops *ops = obj->ops;
	int ret;

	if (obj->pages)
		return 0;

	if (obj->madv != I915_MADV_WILLNEED) {
		DRM_DEBUG("Attempting to obtain a purgeable object\n");
		return -EFAULT;
	}

	BUG_ON(obj->pages_pin_count);

	ret = ops->get_pages(obj);
	if (ret)
		return ret;

	list_add_tail(&obj->global_list, &dev_priv->mm.unbound_list);

	obj->get_page.sg = obj->pages->sgl;
	obj->get_page.last = 0;

	return 0;
}

<<<<<<< HEAD
void i915_vma_move_to_active(struct i915_vma *vma,
			     struct drm_i915_gem_request *req)
{
	struct drm_i915_gem_object *obj = vma->obj;
	struct intel_engine_cs *ring;

	ring = i915_gem_request_get_ring(req);

	/* Add a reference if we're newly entering the active list. */
	if (obj->active == 0)
		drm_gem_object_reference(&obj->base);
	obj->active |= intel_ring_flag(ring);

	list_move_tail(&obj->ring_list[ring->id], &ring->active_list);
	i915_gem_request_assign(&obj->last_read_req[ring->id], req);

	list_move_tail(&vma->mm_list, &vma->vm->active_list);
}

static void
i915_gem_object_retire__write(struct drm_i915_gem_object *obj)
{
	RQ_BUG_ON(obj->last_write_req == NULL);
	RQ_BUG_ON(!(obj->active & intel_ring_flag(obj->last_write_req->ring)));

	i915_gem_request_assign(&obj->last_write_req, NULL);
=======
/* The 'mapping' part of i915_gem_object_pin_map() below */
static void *i915_gem_object_map(const struct drm_i915_gem_object *obj,
				 enum i915_map_type type)
{
	unsigned long n_pages = obj->base.size >> PAGE_SHIFT;
	struct sg_table *sgt = obj->pages;
	struct sgt_iter sgt_iter;
	struct page *page;
	struct page *stack_pages[32];
	struct page **pages = stack_pages;
	unsigned long i = 0;
	pgprot_t pgprot;
	void *addr;

	/* A single page can always be kmapped */
	if (n_pages == 1 && type == I915_MAP_WB)
		return kmap(sg_page(sgt->sgl));

	if (n_pages > ARRAY_SIZE(stack_pages)) {
		/* Too big for stack -- allocate temporary array instead */
		pages = drm_malloc_gfp(n_pages, sizeof(*pages), GFP_TEMPORARY);
		if (!pages)
			return NULL;
	}

	for_each_sgt_page(page, sgt_iter, sgt)
		pages[i++] = page;

	/* Check that we have the expected number of pages */
	GEM_BUG_ON(i != n_pages);

	switch (type) {
	case I915_MAP_WB:
		pgprot = PAGE_KERNEL;
		break;
	case I915_MAP_WC:
		pgprot = pgprot_writecombine(PAGE_KERNEL_IO);
		break;
	}
	addr = vmap(pages, n_pages, 0, pgprot);

	if (pages != stack_pages)
		drm_free_large(pages);

	return addr;
}

/* get, pin, and map the pages of the object into kernel space */
void *i915_gem_object_pin_map(struct drm_i915_gem_object *obj,
			      enum i915_map_type type)
{
	enum i915_map_type has_type;
	bool pinned;
	void *ptr;
	int ret;

	lockdep_assert_held(&obj->base.dev->struct_mutex);
	GEM_BUG_ON(!i915_gem_object_has_struct_page(obj));

	ret = i915_gem_object_get_pages(obj);
	if (ret)
		return ERR_PTR(ret);

	i915_gem_object_pin_pages(obj);
	pinned = obj->pages_pin_count > 1;

	ptr = ptr_unpack_bits(obj->mapping, has_type);
	if (ptr && has_type != type) {
		if (pinned) {
			ret = -EBUSY;
			goto err;
		}

		if (is_vmalloc_addr(ptr))
			vunmap(ptr);
		else
			kunmap(kmap_to_page(ptr));

		ptr = obj->mapping = NULL;
	}

	if (!ptr) {
		ptr = i915_gem_object_map(obj, type);
		if (!ptr) {
			ret = -ENOMEM;
			goto err;
		}

		obj->mapping = ptr_pack_bits(ptr, type);
	}

	return ptr;

err:
	i915_gem_object_unpin_pages(obj);
	return ERR_PTR(ret);
}

static void
i915_gem_object_retire__write(struct i915_gem_active *active,
			      struct drm_i915_gem_request *request)
{
	struct drm_i915_gem_object *obj =
		container_of(active, struct drm_i915_gem_object, last_write);

>>>>>>> v4.9.227
	intel_fb_obj_flush(obj, true, ORIGIN_CS);
}

static void
<<<<<<< HEAD
i915_gem_object_retire__read(struct drm_i915_gem_object *obj, int ring)
{
	struct i915_vma *vma;

	RQ_BUG_ON(obj->last_read_req[ring] == NULL);
	RQ_BUG_ON(!(obj->active & (1 << ring)));

	list_del_init(&obj->ring_list[ring]);
	i915_gem_request_assign(&obj->last_read_req[ring], NULL);

	if (obj->last_write_req && obj->last_write_req->ring->id == ring)
		i915_gem_object_retire__write(obj);

	obj->active &= ~(1 << ring);
	if (obj->active)
=======
i915_gem_object_retire__read(struct i915_gem_active *active,
			     struct drm_i915_gem_request *request)
{
	int idx = request->engine->id;
	struct drm_i915_gem_object *obj =
		container_of(active, struct drm_i915_gem_object, last_read[idx]);

	GEM_BUG_ON(!i915_gem_object_has_active_engine(obj, idx));

	i915_gem_object_clear_active(obj, idx);
	if (i915_gem_object_is_active(obj))
>>>>>>> v4.9.227
		return;

	/* Bump our place on the bound list to keep it roughly in LRU order
	 * so that we don't steal from recently used but inactive objects
	 * (unless we are forced to ofc!)
	 */
<<<<<<< HEAD
	list_move_tail(&obj->global_list,
		       &to_i915(obj->base.dev)->mm.bound_list);

	list_for_each_entry(vma, &obj->vma_list, vma_link) {
		if (!list_empty(&vma->mm_list))
			list_move_tail(&vma->mm_list, &vma->vm->inactive_list);
	}

	i915_gem_request_assign(&obj->last_fenced_req, NULL);
	drm_gem_object_unreference(&obj->base);
}

static int
i915_gem_init_seqno(struct drm_device *dev, u32 seqno)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring;
	int ret, i, j;

	/* Carefully retire all requests without writing to the rings */
	for_each_ring(ring, dev_priv, i) {
		ret = intel_ring_idle(ring);
		if (ret)
			return ret;
	}
	i915_gem_retire_requests(dev);

	/* Finally reset hw state */
	for_each_ring(ring, dev_priv, i) {
		intel_ring_init_seqno(ring, seqno);

		for (j = 0; j < ARRAY_SIZE(ring->semaphore.sync_seqno); j++)
			ring->semaphore.sync_seqno[j] = 0;
	}

	return 0;
}

int i915_gem_set_seqno(struct drm_device *dev, u32 seqno)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	int ret;

	if (seqno == 0)
		return -EINVAL;

	/* HWS page needs to be set less than what we
	 * will inject to ring
	 */
	ret = i915_gem_init_seqno(dev, seqno - 1);
	if (ret)
		return ret;

	/* Carefully set the last_seqno value so that wrap
	 * detection still works
	 */
	dev_priv->next_seqno = seqno;
	dev_priv->last_seqno = seqno - 1;
	if (dev_priv->last_seqno == 0)
		dev_priv->last_seqno--;

	return 0;
}

int
i915_gem_get_seqno(struct drm_device *dev, u32 *seqno)
{
	struct drm_i915_private *dev_priv = dev->dev_private;

	/* reserve 0 for non-seqno */
	if (dev_priv->next_seqno == 0) {
		int ret = i915_gem_init_seqno(dev, 0);
		if (ret)
			return ret;

		dev_priv->next_seqno = 1;
	}

	*seqno = dev_priv->last_seqno = dev_priv->next_seqno++;
	return 0;
}

/*
 * NB: This function is not allowed to fail. Doing so would mean the the
 * request is not being tracked for completion but the work itself is
 * going to happen on the hardware. This would be a Bad Thing(tm).
 */
void __i915_add_request(struct drm_i915_gem_request *request,
			struct drm_i915_gem_object *obj,
			bool flush_caches)
{
	struct intel_engine_cs *ring;
	struct drm_i915_private *dev_priv;
	struct intel_ringbuffer *ringbuf;
	u32 request_start;
	int ret;

	if (WARN_ON(request == NULL))
		return;

	ring = request->ring;
	dev_priv = ring->dev->dev_private;
	ringbuf = request->ringbuf;

	/*
	 * To ensure that this call will not fail, space for its emissions
	 * should already have been reserved in the ring buffer. Let the ring
	 * know that it is time to use that space up.
	 */
	intel_ring_reserved_space_use(ringbuf);

	request_start = intel_ring_get_tail(ringbuf);
	/*
	 * Emit any outstanding flushes - execbuf can fail to emit the flush
	 * after having emitted the batchbuffer command. Hence we need to fix
	 * things up similar to emitting the lazy request. The difference here
	 * is that the flush _must_ happen before the next request, no matter
	 * what.
	 */
	if (flush_caches) {
		if (i915.enable_execlists)
			ret = logical_ring_flush_all_caches(request);
		else
			ret = intel_ring_flush_all_caches(request);
		/* Not allowed to fail! */
		WARN(ret, "*_ring_flush_all_caches failed: %d!\n", ret);
	}

	/* Record the position of the start of the request so that
	 * should we detect the updated seqno part-way through the
	 * GPU processing the request, we never over-estimate the
	 * position of the head.
	 */
	request->postfix = intel_ring_get_tail(ringbuf);

	if (i915.enable_execlists)
		ret = ring->emit_request(request);
	else {
		ret = ring->add_request(request);

		request->tail = intel_ring_get_tail(ringbuf);
	}
	/* Not allowed to fail! */
	WARN(ret, "emit|add_request failed: %d!\n", ret);

	request->head = request_start;

	/* Whilst this request exists, batch_obj will be on the
	 * active_list, and so will hold the active reference. Only when this
	 * request is retired will the the batch_obj be moved onto the
	 * inactive_list and lose its active reference. Hence we do not need
	 * to explicitly hold another reference here.
	 */
	request->batch_obj = obj;

	request->emitted_jiffies = jiffies;
	request->previous_seqno = ring->last_submitted_seqno;
	ring->last_submitted_seqno = request->seqno;
	list_add_tail(&request->list, &ring->request_list);

	trace_i915_gem_request_add(request);

	i915_queue_hangcheck(ring->dev);

	queue_delayed_work(dev_priv->wq,
			   &dev_priv->mm.retire_work,
			   round_jiffies_up_relative(HZ));
	intel_mark_busy(dev_priv->dev);

	/* Sanity check that the reserved size was large enough. */
	intel_ring_reserved_space_end(ringbuf);
}

static bool i915_context_is_banned(struct drm_i915_private *dev_priv,
				   const struct intel_context *ctx)
{
	unsigned long elapsed;

	elapsed = get_seconds() - ctx->hang_stats.guilty_ts;

	if (ctx->hang_stats.banned)
		return true;

	if (ctx->hang_stats.ban_period_seconds &&
	    elapsed <= ctx->hang_stats.ban_period_seconds) {
		if (!i915_gem_context_is_default(ctx)) {
			DRM_DEBUG("context hanging too fast, banning!\n");
			return true;
		} else if (i915_stop_ring_allow_ban(dev_priv)) {
			if (i915_stop_ring_allow_warn(dev_priv))
				DRM_ERROR("gpu hanging too fast, banning!\n");
			return true;
		}
=======
	if (obj->bind_count)
		list_move_tail(&obj->global_list,
			       &request->i915->mm.bound_list);

	i915_gem_object_put(obj);
}

static bool i915_context_is_banned(const struct i915_gem_context *ctx)
{
	unsigned long elapsed;

	if (ctx->hang_stats.banned)
		return true;

	elapsed = get_seconds() - ctx->hang_stats.guilty_ts;
	if (ctx->hang_stats.ban_period_seconds &&
	    elapsed <= ctx->hang_stats.ban_period_seconds) {
		DRM_DEBUG("context hanging too fast, banning!\n");
		return true;
>>>>>>> v4.9.227
	}

	return false;
}

<<<<<<< HEAD
static void i915_set_reset_status(struct drm_i915_private *dev_priv,
				  struct intel_context *ctx,
				  const bool guilty)
{
	struct i915_ctx_hang_stats *hs;

	if (WARN_ON(!ctx))
		return;

	hs = &ctx->hang_stats;

	if (guilty) {
		hs->banned = i915_context_is_banned(dev_priv, ctx);
=======
static void i915_set_reset_status(struct i915_gem_context *ctx,
				  const bool guilty)
{
	struct i915_ctx_hang_stats *hs = &ctx->hang_stats;

	if (guilty) {
		hs->banned = i915_context_is_banned(ctx);
>>>>>>> v4.9.227
		hs->batch_active++;
		hs->guilty_ts = get_seconds();
	} else {
		hs->batch_pending++;
	}
}

<<<<<<< HEAD
void i915_gem_request_free(struct kref *req_ref)
{
	struct drm_i915_gem_request *req = container_of(req_ref,
						 typeof(*req), ref);
	struct intel_context *ctx = req->ctx;

	if (req->file_priv)
		i915_gem_request_remove_from_client(req);

	if (ctx) {
		if (i915.enable_execlists) {
			if (ctx != req->ring->default_context)
				intel_lr_context_unpin(req);
		}

		i915_gem_context_unreference(ctx);
	}

	kmem_cache_free(req->i915->requests, req);
}

int i915_gem_request_alloc(struct intel_engine_cs *ring,
			   struct intel_context *ctx,
			   struct drm_i915_gem_request **req_out)
{
	struct drm_i915_private *dev_priv = to_i915(ring->dev);
	struct drm_i915_gem_request *req;
	int ret;

	if (!req_out)
		return -EINVAL;

	*req_out = NULL;

	req = kmem_cache_zalloc(dev_priv->requests, GFP_KERNEL);
	if (req == NULL)
		return -ENOMEM;

	ret = i915_gem_get_seqno(ring->dev, &req->seqno);
	if (ret)
		goto err;

	kref_init(&req->ref);
	req->i915 = dev_priv;
	req->ring = ring;
	req->ctx  = ctx;
	i915_gem_context_reference(req->ctx);

	if (i915.enable_execlists)
		ret = intel_logical_ring_alloc_request_extras(req);
	else
		ret = intel_ring_alloc_request_extras(req);
	if (ret) {
		i915_gem_context_unreference(req->ctx);
		goto err;
	}

	/*
	 * Reserve space in the ring buffer for all the commands required to
	 * eventually emit this request. This is to guarantee that the
	 * i915_add_request() call can't fail. Note that the reserve may need
	 * to be redone if the request is not actually submitted straight
	 * away, e.g. because a GPU scheduler has deferred it.
	 */
	if (i915.enable_execlists)
		ret = intel_logical_ring_reserve_space(req);
	else
		ret = intel_ring_reserve_space(req);
	if (ret) {
		/*
		 * At this point, the request is fully allocated even if not
		 * fully prepared. Thus it can be cleaned up using the proper
		 * free code.
		 */
		i915_gem_request_cancel(req);
		return ret;
	}

	*req_out = req;
	return 0;

err:
	kmem_cache_free(dev_priv->requests, req);
	return ret;
}

void i915_gem_request_cancel(struct drm_i915_gem_request *req)
{
	intel_ring_reserved_space_cancel(req->ringbuf);

	i915_gem_request_unreference(req);
}

struct drm_i915_gem_request *
i915_gem_find_active_request(struct intel_engine_cs *ring)
{
	struct drm_i915_gem_request *request;

	list_for_each_entry(request, &ring->request_list, list) {
		if (i915_gem_request_completed(request, false))
			continue;

=======
struct drm_i915_gem_request *
i915_gem_find_active_request(struct intel_engine_cs *engine)
{
	struct drm_i915_gem_request *request;

	/* We are called by the error capture and reset at a random
	 * point in time. In particular, note that neither is crucially
	 * ordered with an interrupt. After a hang, the GPU is dead and we
	 * assume that no more writes can happen (we waited long enough for
	 * all writes that were in transaction to be flushed) - adding an
	 * extra delay for a recent interrupt is pointless. Hence, we do
	 * not need an engine->irq_seqno_barrier() before the seqno reads.
	 */
	list_for_each_entry(request, &engine->request_list, link) {
		if (i915_gem_request_completed(request))
			continue;

		if (!i915_sw_fence_done(&request->submit))
			break;

>>>>>>> v4.9.227
		return request;
	}

	return NULL;
}

<<<<<<< HEAD
static void i915_gem_reset_ring_status(struct drm_i915_private *dev_priv,
				       struct intel_engine_cs *ring)
{
	struct drm_i915_gem_request *request;
	bool ring_hung;

	request = i915_gem_find_active_request(ring);

	if (request == NULL)
		return;

	ring_hung = ring->hangcheck.score >= HANGCHECK_SCORE_RING_HUNG;

	i915_set_reset_status(dev_priv, request->ctx, ring_hung);

	list_for_each_entry_continue(request, &ring->request_list, list)
		i915_set_reset_status(dev_priv, request->ctx, false);
}

static void i915_gem_reset_ring_cleanup(struct drm_i915_private *dev_priv,
					struct intel_engine_cs *ring)
{
	while (!list_empty(&ring->active_list)) {
		struct drm_i915_gem_object *obj;

		obj = list_first_entry(&ring->active_list,
				       struct drm_i915_gem_object,
				       ring_list[ring->id]);

		i915_gem_object_retire__read(obj, ring->id);
	}
=======
static void reset_request(struct drm_i915_gem_request *request)
{
	void *vaddr = request->ring->vaddr;
	u32 head;

	/* As this request likely depends on state from the lost
	 * context, clear out all the user operations leaving the
	 * breadcrumb at the end (so we get the fence notifications).
	 */
	head = request->head;
	if (request->postfix < head) {
		memset(vaddr + head, 0, request->ring->size - head);
		head = 0;
	}
	memset(vaddr + head, 0, request->postfix - head);
}

static void i915_gem_reset_engine(struct intel_engine_cs *engine)
{
	struct drm_i915_gem_request *request;
	struct i915_gem_context *incomplete_ctx;
	bool ring_hung;

	/* Ensure irq handler finishes, and not run again. */
	tasklet_kill(&engine->irq_tasklet);
	if (engine->irq_seqno_barrier)
		engine->irq_seqno_barrier(engine);

	request = i915_gem_find_active_request(engine);
	if (!request)
		return;

	ring_hung = engine->hangcheck.score >= HANGCHECK_SCORE_RING_HUNG;
	i915_set_reset_status(request->ctx, ring_hung);
	if (!ring_hung)
		return;

	DRM_DEBUG_DRIVER("resetting %s to restart from tail of request 0x%x\n",
			 engine->name, request->fence.seqno);

	/* Setup the CS to resume from the breadcrumb of the hung request */
	engine->reset_hw(engine, request);

	/* Users of the default context do not rely on logical state
	 * preserved between batches. They have to emit full state on
	 * every batch and so it is safe to execute queued requests following
	 * the hang.
	 *
	 * Other contexts preserve state, now corrupt. We want to skip all
	 * queued requests that reference the corrupt context.
	 */
	incomplete_ctx = request->ctx;
	if (i915_gem_context_is_default(incomplete_ctx))
		return;

	list_for_each_entry_continue(request, &engine->request_list, link)
		if (request->ctx == incomplete_ctx)
			reset_request(request);
}

void i915_gem_reset(struct drm_i915_private *dev_priv)
{
	struct intel_engine_cs *engine;

	i915_gem_retire_requests(dev_priv);

	for_each_engine(engine, dev_priv)
		i915_gem_reset_engine(engine);

	i915_gem_restore_fences(&dev_priv->drm);

	if (dev_priv->gt.awake) {
		intel_sanitize_gt_powersave(dev_priv);
		intel_enable_gt_powersave(dev_priv);
		if (INTEL_GEN(dev_priv) >= 6)
			gen6_rps_busy(dev_priv);
	}
}

static void nop_submit_request(struct drm_i915_gem_request *request)
{
}

static void i915_gem_cleanup_engine(struct intel_engine_cs *engine)
{
	engine->submit_request = nop_submit_request;

	/* Mark all pending requests as complete so that any concurrent
	 * (lockless) lookup doesn't try and wait upon the request as we
	 * reset it.
	 */
	intel_engine_init_seqno(engine, engine->last_submitted_seqno);
>>>>>>> v4.9.227

	/*
	 * Clear the execlists queue up before freeing the requests, as those
	 * are the ones that keep the context and ringbuffer backing objects
	 * pinned in place.
	 */
<<<<<<< HEAD
	while (!list_empty(&ring->execlist_queue)) {
		struct drm_i915_gem_request *submit_req;

		submit_req = list_first_entry(&ring->execlist_queue,
				struct drm_i915_gem_request,
				execlist_link);
		list_del(&submit_req->execlist_link);

		if (submit_req->ctx != ring->default_context)
			intel_lr_context_unpin(submit_req);

		i915_gem_request_unreference(submit_req);
	}

	/*
	 * We must free the requests after all the corresponding objects have
	 * been moved off active lists. Which is the same order as the normal
	 * retire_requests function does. This is important if object hold
	 * implicit references on things like e.g. ppgtt address spaces through
	 * the request.
	 */
	while (!list_empty(&ring->request_list)) {
		struct drm_i915_gem_request *request;

		request = list_first_entry(&ring->request_list,
					   struct drm_i915_gem_request,
					   list);

		i915_gem_request_retire(request);
	}
}

void i915_gem_reset(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring;
	int i;

	/*
	 * Before we free the objects from the requests, we need to inspect
	 * them for finding the guilty party. As the requests only borrow
	 * their reference to the objects, the inspection must be done first.
	 */
	for_each_ring(ring, dev_priv, i)
		i915_gem_reset_ring_status(dev_priv, ring);

	for_each_ring(ring, dev_priv, i)
		i915_gem_reset_ring_cleanup(dev_priv, ring);

	i915_gem_context_reset(dev);

	i915_gem_restore_fences(dev);

	WARN_ON(i915_verify_lists(dev));
}

/**
 * This function clears the request list as sequence numbers are passed.
 */
void
i915_gem_retire_requests_ring(struct intel_engine_cs *ring)
{
	WARN_ON(i915_verify_lists(ring->dev));

	/* Retire requests first as we use it above for the early return.
	 * If we retire requests last, we may use a later seqno and so clear
	 * the requests lists without clearing the active list, leading to
	 * confusion.
	 */
	while (!list_empty(&ring->request_list)) {
		struct drm_i915_gem_request *request;

		request = list_first_entry(&ring->request_list,
					   struct drm_i915_gem_request,
					   list);

		if (!i915_gem_request_completed(request, true))
			break;

		i915_gem_request_retire(request);
	}

	/* Move any buffers on the active list that are no longer referenced
	 * by the ringbuffer to the flushing/inactive lists as appropriate,
	 * before we free the context associated with the requests.
	 */
	while (!list_empty(&ring->active_list)) {
		struct drm_i915_gem_object *obj;

		obj = list_first_entry(&ring->active_list,
				      struct drm_i915_gem_object,
				      ring_list[ring->id]);

		if (!list_empty(&obj->last_read_req[ring->id]->list))
			break;

		i915_gem_object_retire__read(obj, ring->id);
	}

	if (unlikely(ring->trace_irq_req &&
		     i915_gem_request_completed(ring->trace_irq_req, true))) {
		ring->irq_put(ring);
		i915_gem_request_assign(&ring->trace_irq_req, NULL);
	}

	WARN_ON(i915_verify_lists(ring->dev));
}

bool
i915_gem_retire_requests(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring;
	bool idle = true;
	int i;

	for_each_ring(ring, dev_priv, i) {
		i915_gem_retire_requests_ring(ring);
		idle &= list_empty(&ring->request_list);
		if (i915.enable_execlists) {
			unsigned long flags;

			spin_lock_irqsave(&ring->execlist_lock, flags);
			idle &= list_empty(&ring->execlist_queue);
			spin_unlock_irqrestore(&ring->execlist_lock, flags);

			intel_execlists_retire_requests(ring);
		}
	}

	if (idle)
		mod_delayed_work(dev_priv->wq,
				   &dev_priv->mm.idle_work,
				   msecs_to_jiffies(100));

	return idle;
=======

	if (i915.enable_execlists) {
		spin_lock(&engine->execlist_lock);
		INIT_LIST_HEAD(&engine->execlist_queue);
		i915_gem_request_put(engine->execlist_port[0].request);
		i915_gem_request_put(engine->execlist_port[1].request);
		memset(engine->execlist_port, 0, sizeof(engine->execlist_port));
		spin_unlock(&engine->execlist_lock);
	}

	engine->i915->gt.active_engines &= ~intel_engine_flag(engine);
}

void i915_gem_set_wedged(struct drm_i915_private *dev_priv)
{
	struct intel_engine_cs *engine;

	lockdep_assert_held(&dev_priv->drm.struct_mutex);
	set_bit(I915_WEDGED, &dev_priv->gpu_error.flags);

	i915_gem_context_lost(dev_priv);
	for_each_engine(engine, dev_priv)
		i915_gem_cleanup_engine(engine);
	mod_delayed_work(dev_priv->wq, &dev_priv->gt.idle_work, 0);

	i915_gem_retire_requests(dev_priv);
>>>>>>> v4.9.227
}

static void
i915_gem_retire_work_handler(struct work_struct *work)
{
	struct drm_i915_private *dev_priv =
<<<<<<< HEAD
		container_of(work, typeof(*dev_priv), mm.retire_work.work);
	struct drm_device *dev = dev_priv->dev;
	bool idle;

	/* Come back later if the device is busy... */
	idle = false;
	if (mutex_trylock(&dev->struct_mutex)) {
		idle = i915_gem_retire_requests(dev);
		mutex_unlock(&dev->struct_mutex);
	}
	if (!idle)
		queue_delayed_work(dev_priv->wq, &dev_priv->mm.retire_work,
				   round_jiffies_up_relative(HZ));
=======
		container_of(work, typeof(*dev_priv), gt.retire_work.work);
	struct drm_device *dev = &dev_priv->drm;

	/* Come back later if the device is busy... */
	if (mutex_trylock(&dev->struct_mutex)) {
		i915_gem_retire_requests(dev_priv);
		mutex_unlock(&dev->struct_mutex);
	}

	/* Keep the retire handler running until we are finally idle.
	 * We do not need to do this test under locking as in the worst-case
	 * we queue the retire worker once too often.
	 */
	if (READ_ONCE(dev_priv->gt.awake)) {
		i915_queue_hangcheck(dev_priv);
		queue_delayed_work(dev_priv->wq,
				   &dev_priv->gt.retire_work,
				   round_jiffies_up_relative(HZ));
	}
>>>>>>> v4.9.227
}

static void
i915_gem_idle_work_handler(struct work_struct *work)
{
	struct drm_i915_private *dev_priv =
<<<<<<< HEAD
		container_of(work, typeof(*dev_priv), mm.idle_work.work);
	struct drm_device *dev = dev_priv->dev;
	struct intel_engine_cs *ring;
	int i;

	for_each_ring(ring, dev_priv, i)
		if (!list_empty(&ring->request_list))
			return;

	intel_mark_idle(dev);

	if (mutex_trylock(&dev->struct_mutex)) {
		struct intel_engine_cs *ring;
		int i;

		for_each_ring(ring, dev_priv, i)
			i915_gem_batch_pool_fini(&ring->batch_pool);

		mutex_unlock(&dev->struct_mutex);
	}
}

/**
 * Ensures that an object will eventually get non-busy by flushing any required
 * write domains, emitting any outstanding lazy request and retiring and
 * completed requests.
 */
static int
i915_gem_object_flush_active(struct drm_i915_gem_object *obj)
{
	int i;

	if (!obj->active)
		return 0;

	for (i = 0; i < I915_NUM_RINGS; i++) {
		struct drm_i915_gem_request *req;

		req = obj->last_read_req[i];
		if (req == NULL)
			continue;

		if (list_empty(&req->list))
			goto retire;

		if (i915_gem_request_completed(req, true)) {
			__i915_gem_request_retire__upto(req);
retire:
			i915_gem_object_retire__read(obj, i);
		}
	}

	return 0;
=======
		container_of(work, typeof(*dev_priv), gt.idle_work.work);
	struct drm_device *dev = &dev_priv->drm;
	struct intel_engine_cs *engine;
	bool rearm_hangcheck;

	if (!READ_ONCE(dev_priv->gt.awake))
		return;

	if (READ_ONCE(dev_priv->gt.active_engines))
		return;

	rearm_hangcheck =
		cancel_delayed_work_sync(&dev_priv->gpu_error.hangcheck_work);

	if (!mutex_trylock(&dev->struct_mutex)) {
		/* Currently busy, come back later */
		mod_delayed_work(dev_priv->wq,
				 &dev_priv->gt.idle_work,
				 msecs_to_jiffies(50));
		goto out_rearm;
	}

	if (dev_priv->gt.active_engines)
		goto out_unlock;

	for_each_engine(engine, dev_priv)
		i915_gem_batch_pool_fini(&engine->batch_pool);

	GEM_BUG_ON(!dev_priv->gt.awake);
	dev_priv->gt.awake = false;
	rearm_hangcheck = false;

	if (INTEL_GEN(dev_priv) >= 6)
		gen6_rps_idle(dev_priv);

	if (NEEDS_RC6_CTX_CORRUPTION_WA(dev_priv)) {
		i915_rc6_ctx_wa_check(dev_priv);
		intel_uncore_forcewake_put(dev_priv, FORCEWAKE_ALL);
	}

	intel_runtime_pm_put(dev_priv);
out_unlock:
	mutex_unlock(&dev->struct_mutex);

out_rearm:
	if (rearm_hangcheck) {
		GEM_BUG_ON(!dev_priv->gt.awake);
		i915_queue_hangcheck(dev_priv);
	}
}

void i915_gem_close_object(struct drm_gem_object *gem, struct drm_file *file)
{
	struct drm_i915_gem_object *obj = to_intel_bo(gem);
	struct drm_i915_file_private *fpriv = file->driver_priv;
	struct i915_vma *vma, *vn;

	mutex_lock(&obj->base.dev->struct_mutex);
	list_for_each_entry_safe(vma, vn, &obj->vma_list, obj_link)
		if (vma->vm->file == fpriv)
			i915_vma_close(vma);
	mutex_unlock(&obj->base.dev->struct_mutex);
>>>>>>> v4.9.227
}

/**
 * i915_gem_wait_ioctl - implements DRM_IOCTL_I915_GEM_WAIT
<<<<<<< HEAD
 * @DRM_IOCTL_ARGS: standard ioctl arguments
=======
 * @dev: drm device pointer
 * @data: ioctl data blob
 * @file: drm file pointer
>>>>>>> v4.9.227
 *
 * Returns 0 if successful, else an error is returned with the remaining time in
 * the timeout parameter.
 *  -ETIME: object is still busy after timeout
 *  -ERESTARTSYS: signal interrupted the wait
 *  -ENONENT: object doesn't exist
 * Also possible, but rare:
 *  -EAGAIN: GPU wedged
 *  -ENOMEM: damn
 *  -ENODEV: Internal IRQ fail
 *  -E?: The add request failed
 *
 * The wait ioctl with a timeout of 0 reimplements the busy ioctl. With any
 * non-zero timeout parameter the wait ioctl will wait for the given number of
 * nanoseconds on an object becoming unbusy. Since the wait itself does so
 * without holding struct_mutex the object may become re-busied before this
 * function completes. A similar but shorter * race condition exists in the busy
 * ioctl
 */
int
i915_gem_wait_ioctl(struct drm_device *dev, void *data, struct drm_file *file)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct drm_i915_gem_wait *args = data;
	struct drm_i915_gem_object *obj;
	struct drm_i915_gem_request *req[I915_NUM_RINGS];
	unsigned reset_counter;
	int i, n = 0;
	int ret;
=======
	struct drm_i915_gem_wait *args = data;
	struct intel_rps_client *rps = to_rps_client(file);
	struct drm_i915_gem_object *obj;
	unsigned long active;
	int idx, ret = 0;
>>>>>>> v4.9.227

	if (args->flags != 0)
		return -EINVAL;

<<<<<<< HEAD
	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		return ret;

	obj = to_intel_bo(drm_gem_object_lookup(dev, file, args->bo_handle));
	if (&obj->base == NULL) {
		mutex_unlock(&dev->struct_mutex);
		return -ENOENT;
	}

	/* Need to make sure the object gets inactive eventually. */
	ret = i915_gem_object_flush_active(obj);
	if (ret)
		goto out;

	if (!obj->active)
		goto out;

	/* Do this after OLR check to make sure we make forward progress polling
	 * on this IOCTL with a timeout == 0 (like busy ioctl)
	 */
	if (args->timeout_ns == 0) {
		ret = -ETIME;
		goto out;
	}

	drm_gem_object_unreference(&obj->base);
	reset_counter = atomic_read(&dev_priv->gpu_error.reset_counter);

	for (i = 0; i < I915_NUM_RINGS; i++) {
		if (obj->last_read_req[i] == NULL)
			continue;

		req[n++] = i915_gem_request_reference(obj->last_read_req[i]);
	}

	mutex_unlock(&dev->struct_mutex);

	for (i = 0; i < n; i++) {
		if (ret == 0)
			ret = __i915_wait_request(req[i], reset_counter, true,
						  args->timeout_ns > 0 ? &args->timeout_ns : NULL,
						  file->driver_priv);
		i915_gem_request_unreference__unlocked(req[i]);
	}
	return ret;

out:
	drm_gem_object_unreference(&obj->base);
	mutex_unlock(&dev->struct_mutex);
	return ret;
}

static int
__i915_gem_object_sync(struct drm_i915_gem_object *obj,
		       struct intel_engine_cs *to,
		       struct drm_i915_gem_request *from_req,
		       struct drm_i915_gem_request **to_req)
{
	struct intel_engine_cs *from;
	int ret;

	from = i915_gem_request_get_ring(from_req);
	if (to == from)
		return 0;

	if (i915_gem_request_completed(from_req, true))
		return 0;

	if (!i915_semaphore_is_enabled(obj->base.dev)) {
		struct drm_i915_private *i915 = to_i915(obj->base.dev);
		ret = __i915_wait_request(from_req,
					  atomic_read(&i915->gpu_error.reset_counter),
					  i915->mm.interruptible,
					  NULL,
					  &i915->rps.semaphores);
		if (ret)
			return ret;

		i915_gem_object_retire_request(obj, from_req);
	} else {
		int idx = intel_ring_sync_index(from, to);
		u32 seqno = i915_gem_request_get_seqno(from_req);

		WARN_ON(!to_req);

		if (seqno <= from->semaphore.sync_seqno[idx])
			return 0;

		if (*to_req == NULL) {
			ret = i915_gem_request_alloc(to, to->default_context, to_req);
			if (ret)
				return ret;
		}

		trace_i915_gem_ring_sync_to(*to_req, from, from_req);
		ret = to->semaphore.sync_to(*to_req, from, seqno);
		if (ret)
			return ret;

		/* We use last_read_req because sync_to()
		 * might have just caused seqno wrap under
		 * the radar.
		 */
		from->semaphore.sync_seqno[idx] =
			i915_gem_request_get_seqno(obj->last_read_req[from->id]);
	}

	return 0;
}

/**
 * i915_gem_object_sync - sync an object to a ring.
 *
 * @obj: object which may be in use on another ring.
 * @to: ring we wish to use the object on. May be NULL.
 * @to_req: request we wish to use the object for. See below.
 *          This will be allocated and returned if a request is
 *          required but not passed in.
 *
 * This code is meant to abstract object synchronization with the GPU.
 * Calling with NULL implies synchronizing the object with the CPU
 * rather than a particular GPU ring. Conceptually we serialise writes
 * between engines inside the GPU. We only allow one engine to write
 * into a buffer at any time, but multiple readers. To ensure each has
 * a coherent view of memory, we must:
 *
 * - If there is an outstanding write request to the object, the new
 *   request must wait for it to complete (either CPU or in hw, requests
 *   on the same ring will be naturally ordered).
 *
 * - If we are a write request (pending_write_domain is set), the new
 *   request must wait for outstanding read requests to complete.
 *
 * For CPU synchronisation (NULL to) no request is required. For syncing with
 * rings to_req must be non-NULL. However, a request does not have to be
 * pre-allocated. If *to_req is NULL and sync commands will be emitted then a
 * request will be allocated automatically and returned through *to_req. Note
 * that it is not guaranteed that commands will be emitted (because the system
 * might already be idle). Hence there is no need to create a request that
 * might never have any work submitted. Note further that if a request is
 * returned in *to_req, it is the responsibility of the caller to submit
 * that request (after potentially adding more work to it).
 *
 * Returns 0 if successful, else propagates up the lower layer error.
 */
int
i915_gem_object_sync(struct drm_i915_gem_object *obj,
		     struct intel_engine_cs *to,
		     struct drm_i915_gem_request **to_req)
{
	const bool readonly = obj->base.pending_write_domain == 0;
	struct drm_i915_gem_request *req[I915_NUM_RINGS];
	int ret, i, n;

	if (!obj->active)
		return 0;

	if (to == NULL)
		return i915_gem_object_wait_rendering(obj, readonly);

	n = 0;
	if (readonly) {
		if (obj->last_write_req)
			req[n++] = obj->last_write_req;
	} else {
		for (i = 0; i < I915_NUM_RINGS; i++)
			if (obj->last_read_req[i])
				req[n++] = obj->last_read_req[i];
	}
	for (i = 0; i < n; i++) {
		ret = __i915_gem_object_sync(obj, to, req[i], to_req);
		if (ret)
			return ret;
	}

	return 0;
}

static void i915_gem_object_finish_gtt(struct drm_i915_gem_object *obj)
{
	u32 old_write_domain, old_read_domains;

	/* Force a pagefault for domain tracking on next user access */
	i915_gem_release_mmap(obj);

	if ((obj->base.read_domains & I915_GEM_DOMAIN_GTT) == 0)
		return;

	/* Wait for any direct GTT access to complete */
	mb();

	old_read_domains = obj->base.read_domains;
	old_write_domain = obj->base.write_domain;

	obj->base.read_domains &= ~I915_GEM_DOMAIN_GTT;
	obj->base.write_domain &= ~I915_GEM_DOMAIN_GTT;

	trace_i915_gem_object_change_domain(obj,
					    old_read_domains,
					    old_write_domain);
}

static int __i915_vma_unbind(struct i915_vma *vma, bool wait)
{
	struct drm_i915_gem_object *obj = vma->obj;
	struct drm_i915_private *dev_priv = obj->base.dev->dev_private;
	int ret;

	if (list_empty(&vma->vma_link))
		return 0;

	if (!drm_mm_node_allocated(&vma->node)) {
		i915_gem_vma_destroy(vma);
		return 0;
	}

	if (vma->pin_count)
		return -EBUSY;

	BUG_ON(obj->pages == NULL);

	if (wait) {
		ret = i915_gem_object_wait_rendering(obj, false);
		if (ret)
			return ret;
	}

	if (i915_is_ggtt(vma->vm) &&
	    vma->ggtt_view.type == I915_GGTT_VIEW_NORMAL) {
		i915_gem_object_finish_gtt(obj);

		/* release the fence reg _after_ flushing */
		ret = i915_gem_object_put_fence(obj);
		if (ret)
			return ret;
	}

	trace_i915_vma_unbind(vma);

	vma->vm->unbind_vma(vma);
	vma->bound = 0;

	list_del_init(&vma->mm_list);
	if (i915_is_ggtt(vma->vm)) {
		if (vma->ggtt_view.type == I915_GGTT_VIEW_NORMAL) {
			obj->map_and_fenceable = false;
		} else if (vma->ggtt_view.pages) {
			sg_free_table(vma->ggtt_view.pages);
			kfree(vma->ggtt_view.pages);
		}
		vma->ggtt_view.pages = NULL;
	}

	drm_mm_remove_node(&vma->node);
	i915_gem_vma_destroy(vma);

	/* Since the unbound list is global, only move to that list if
	 * no more VMAs exist. */
	if (list_empty(&obj->vma_list))
		list_move_tail(&obj->global_list, &dev_priv->mm.unbound_list);
=======
	obj = i915_gem_object_lookup(file, args->bo_handle);
	if (!obj)
		return -ENOENT;

	active = __I915_BO_ACTIVE(obj);
	for_each_active(active, idx) {
		s64 *timeout = args->timeout_ns >= 0 ? &args->timeout_ns : NULL;
		ret = i915_gem_active_wait_unlocked(&obj->last_read[idx],
						    I915_WAIT_INTERRUPTIBLE,
						    timeout, rps);
		if (ret)
			break;
	}

	i915_gem_object_put_unlocked(obj);
	return ret;
}

static void __i915_vma_iounmap(struct i915_vma *vma)
{
	GEM_BUG_ON(i915_vma_is_pinned(vma));

	if (vma->iomap == NULL)
		return;

	io_mapping_unmap(vma->iomap);
	vma->iomap = NULL;
}

int i915_vma_unbind(struct i915_vma *vma)
{
	struct drm_i915_gem_object *obj = vma->obj;
	unsigned long active;
	int ret;

	/* First wait upon any activity as retiring the request may
	 * have side-effects such as unpinning or even unbinding this vma.
	 */
	active = i915_vma_get_active(vma);
	if (active) {
		int idx;

		/* When a closed VMA is retired, it is unbound - eek.
		 * In order to prevent it from being recursively closed,
		 * take a pin on the vma so that the second unbind is
		 * aborted.
		 */
		__i915_vma_pin(vma);

		for_each_active(active, idx) {
			ret = i915_gem_active_retire(&vma->last_read[idx],
						   &vma->vm->dev->struct_mutex);
			if (ret)
				break;
		}

		__i915_vma_unpin(vma);
		if (ret)
			return ret;

		GEM_BUG_ON(i915_vma_is_active(vma));
	}

	if (i915_vma_is_pinned(vma))
		return -EBUSY;

	if (!drm_mm_node_allocated(&vma->node))
		goto destroy;

	GEM_BUG_ON(obj->bind_count == 0);
	GEM_BUG_ON(!obj->pages);

	if (i915_vma_is_map_and_fenceable(vma)) {
		/* release the fence reg _after_ flushing */
		ret = i915_vma_put_fence(vma);
		if (ret)
			return ret;

		/* Force a pagefault for domain tracking on next user access */
		i915_gem_release_mmap(obj);

		__i915_vma_iounmap(vma);
		vma->flags &= ~I915_VMA_CAN_FENCE;
	}

	if (likely(!vma->vm->closed)) {
		trace_i915_vma_unbind(vma);
		vma->vm->unbind_vma(vma);
	}
	vma->flags &= ~(I915_VMA_GLOBAL_BIND | I915_VMA_LOCAL_BIND);

	drm_mm_remove_node(&vma->node);
	list_move_tail(&vma->vm_link, &vma->vm->unbound_list);

	if (vma->pages != obj->pages) {
		GEM_BUG_ON(!vma->pages);
		sg_free_table(vma->pages);
		kfree(vma->pages);
	}
	vma->pages = NULL;

	/* Since the unbound list is global, only move to that list if
	 * no more VMAs exist. */
	if (--obj->bind_count == 0)
		list_move_tail(&obj->global_list,
			       &to_i915(obj->base.dev)->mm.unbound_list);
>>>>>>> v4.9.227

	/* And finally now the object is completely decoupled from this vma,
	 * we can drop its hold on the backing storage and allow it to be
	 * reaped by the shrinker.
	 */
	i915_gem_object_unpin_pages(obj);

<<<<<<< HEAD
	return 0;
}

int i915_vma_unbind(struct i915_vma *vma)
{
	return __i915_vma_unbind(vma, true);
}

int __i915_vma_unbind_no_wait(struct i915_vma *vma)
{
	return __i915_vma_unbind(vma, false);
}

int i915_gpu_idle(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring;
	int ret, i;

	/* Flush everything onto the inactive list. */
	for_each_ring(ring, dev_priv, i) {
		if (!i915.enable_execlists) {
			struct drm_i915_gem_request *req;

			ret = i915_gem_request_alloc(ring, ring->default_context, &req);
			if (ret)
				return ret;

			ret = i915_switch_context(req);
			if (ret) {
				i915_gem_request_cancel(req);
				return ret;
			}

			i915_add_request_no_flush(req);
		}

		ret = intel_ring_idle(ring);
=======
destroy:
	if (unlikely(i915_vma_is_closed(vma)))
		i915_vma_destroy(vma);

	return 0;
}

int i915_gem_wait_for_idle(struct drm_i915_private *dev_priv,
			   unsigned int flags)
{
	struct intel_engine_cs *engine;
	int ret;

	for_each_engine(engine, dev_priv) {
		if (engine->last_context == NULL)
			continue;

		ret = intel_engine_idle(engine, flags);
>>>>>>> v4.9.227
		if (ret)
			return ret;
	}

<<<<<<< HEAD
	WARN_ON(i915_verify_lists(dev));
=======
>>>>>>> v4.9.227
	return 0;
}

static bool i915_gem_valid_gtt_space(struct i915_vma *vma,
				     unsigned long cache_level)
{
	struct drm_mm_node *gtt_space = &vma->node;
	struct drm_mm_node *other;

	/*
	 * On some machines we have to be careful when putting differing types
	 * of snoopable memory together to avoid the prefetcher crossing memory
	 * domains and dying. During vm initialisation, we decide whether or not
	 * these constraints apply and set the drm_mm.color_adjust
	 * appropriately.
	 */
	if (vma->vm->mm.color_adjust == NULL)
		return true;

	if (!drm_mm_node_allocated(gtt_space))
		return true;

	if (list_empty(&gtt_space->node_list))
		return true;

	other = list_entry(gtt_space->node_list.prev, struct drm_mm_node, node_list);
	if (other->allocated && !other->hole_follows && other->color != cache_level)
		return false;

	other = list_entry(gtt_space->node_list.next, struct drm_mm_node, node_list);
	if (other->allocated && !gtt_space->hole_follows && other->color != cache_level)
		return false;

	return true;
}

/**
<<<<<<< HEAD
 * Finds free space in the GTT aperture and binds the object or a view of it
 * there.
 */
static struct i915_vma *
i915_gem_object_bind_to_vm(struct drm_i915_gem_object *obj,
			   struct i915_address_space *vm,
			   const struct i915_ggtt_view *ggtt_view,
			   unsigned alignment,
			   uint64_t flags)
{
	struct drm_device *dev = obj->base.dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	u32 fence_alignment, unfenced_alignment;
	u32 search_flag, alloc_flag;
	u64 start, end;
	u64 size, fence_size;
	struct i915_vma *vma;
	int ret;

	if (i915_is_ggtt(vm)) {
		u32 view_size;

		if (WARN_ON(!ggtt_view))
			return ERR_PTR(-EINVAL);

		view_size = i915_ggtt_view_size(obj, ggtt_view);

		fence_size = i915_gem_get_gtt_size(dev,
						   view_size,
						   obj->tiling_mode);
		fence_alignment = i915_gem_get_gtt_alignment(dev,
							     view_size,
							     obj->tiling_mode,
							     true);
		unfenced_alignment = i915_gem_get_gtt_alignment(dev,
								view_size,
								obj->tiling_mode,
								false);
		size = flags & PIN_MAPPABLE ? fence_size : view_size;
	} else {
		fence_size = i915_gem_get_gtt_size(dev,
						   obj->base.size,
						   obj->tiling_mode);
		fence_alignment = i915_gem_get_gtt_alignment(dev,
							     obj->base.size,
							     obj->tiling_mode,
							     true);
		unfenced_alignment =
			i915_gem_get_gtt_alignment(dev,
						   obj->base.size,
						   obj->tiling_mode,
						   false);
		size = flags & PIN_MAPPABLE ? fence_size : obj->base.size;
	}

	start = flags & PIN_OFFSET_BIAS ? flags & PIN_OFFSET_MASK : 0;
	end = vm->total;
	if (flags & PIN_MAPPABLE)
		end = min_t(u64, end, dev_priv->gtt.mappable_end);
	if (flags & PIN_ZONE_4G)
		end = min_t(u64, end, (1ULL << 32));

	if (alignment == 0)
		alignment = flags & PIN_MAPPABLE ? fence_alignment :
						unfenced_alignment;
	if (flags & PIN_MAPPABLE && alignment & (fence_alignment - 1)) {
		DRM_DEBUG("Invalid object (view type=%u) alignment requested %u\n",
			  ggtt_view ? ggtt_view->type : 0,
			  alignment);
		return ERR_PTR(-EINVAL);
	}
=======
 * i915_vma_insert - finds a slot for the vma in its address space
 * @vma: the vma
 * @size: requested size in bytes (can be larger than the VMA)
 * @alignment: required alignment
 * @flags: mask of PIN_* flags to use
 *
 * First we try to allocate some free space that meets the requirements for
 * the VMA. Failiing that, if the flags permit, it will evict an old VMA,
 * preferrably the oldest idle entry to make room for the new VMA.
 *
 * Returns:
 * 0 on success, negative error code otherwise.
 */
static int
i915_vma_insert(struct i915_vma *vma, u64 size, u64 alignment, u64 flags)
{
	struct drm_i915_private *dev_priv = to_i915(vma->vm->dev);
	struct drm_i915_gem_object *obj = vma->obj;
	u64 start, end;
	int ret;

	GEM_BUG_ON(vma->flags & (I915_VMA_GLOBAL_BIND | I915_VMA_LOCAL_BIND));
	GEM_BUG_ON(drm_mm_node_allocated(&vma->node));

	size = max(size, vma->size);
	if (flags & PIN_MAPPABLE)
		size = i915_gem_get_ggtt_size(dev_priv, size,
					      i915_gem_object_get_tiling(obj));

	alignment = max(max(alignment, vma->display_alignment),
			i915_gem_get_ggtt_alignment(dev_priv, size,
						    i915_gem_object_get_tiling(obj),
						    flags & PIN_MAPPABLE));

	start = flags & PIN_OFFSET_BIAS ? flags & PIN_OFFSET_MASK : 0;

	end = vma->vm->total;
	if (flags & PIN_MAPPABLE)
		end = min_t(u64, end, dev_priv->ggtt.mappable_end);
	if (flags & PIN_ZONE_4G)
		end = min_t(u64, end, (1ULL << 32) - PAGE_SIZE);
>>>>>>> v4.9.227

	/* If binding the object/GGTT view requires more space than the entire
	 * aperture has, reject it early before evicting everything in a vain
	 * attempt to find space.
	 */
	if (size > end) {
<<<<<<< HEAD
		DRM_DEBUG("Attempting to bind an object (view type=%u) larger than the aperture: size=%llu > %s aperture=%llu\n",
			  ggtt_view ? ggtt_view->type : 0,
			  size,
			  flags & PIN_MAPPABLE ? "mappable" : "total",
			  end);
		return ERR_PTR(-E2BIG);
=======
		DRM_DEBUG("Attempting to bind an object larger than the aperture: request=%llu [object=%zd] > %s aperture=%llu\n",
			  size, obj->base.size,
			  flags & PIN_MAPPABLE ? "mappable" : "total",
			  end);
		return -E2BIG;
>>>>>>> v4.9.227
	}

	ret = i915_gem_object_get_pages(obj);
	if (ret)
<<<<<<< HEAD
		return ERR_PTR(ret);

	i915_gem_object_pin_pages(obj);

	vma = ggtt_view ? i915_gem_obj_lookup_or_create_ggtt_vma(obj, ggtt_view) :
			  i915_gem_obj_lookup_or_create_vma(obj, vm);

	if (IS_ERR(vma))
		goto err_unpin;

	if (flags & PIN_HIGH) {
		search_flag = DRM_MM_SEARCH_BELOW;
		alloc_flag = DRM_MM_CREATE_TOP;
	} else {
		search_flag = DRM_MM_SEARCH_DEFAULT;
		alloc_flag = DRM_MM_CREATE_DEFAULT;
	}

search_free:
	ret = drm_mm_insert_node_in_range_generic(&vm->mm, &vma->node,
						  size, alignment,
						  obj->cache_level,
						  start, end,
						  search_flag,
						  alloc_flag);
	if (ret) {
		ret = i915_gem_evict_something(dev, vm, size, alignment,
					       obj->cache_level,
					       start, end,
					       flags);
		if (ret == 0)
			goto search_free;

		goto err_free_vma;
	}
	if (WARN_ON(!i915_gem_valid_gtt_space(vma, obj->cache_level))) {
		ret = -EINVAL;
		goto err_remove_node;
	}

	trace_i915_vma_bind(vma, flags);
	ret = i915_vma_bind(vma, obj->cache_level, flags);
	if (ret)
		goto err_remove_node;

	list_move_tail(&obj->global_list, &dev_priv->mm.bound_list);
	list_add_tail(&vma->mm_list, &vm->inactive_list);

	return vma;

err_remove_node:
	drm_mm_remove_node(&vma->node);
err_free_vma:
	i915_gem_vma_destroy(vma);
	vma = ERR_PTR(ret);
err_unpin:
	i915_gem_object_unpin_pages(obj);
	return vma;
=======
		return ret;

	i915_gem_object_pin_pages(obj);

	if (flags & PIN_OFFSET_FIXED) {
		u64 offset = flags & PIN_OFFSET_MASK;
		if (offset & (alignment - 1) || offset > end - size) {
			ret = -EINVAL;
			goto err_unpin;
		}

		vma->node.start = offset;
		vma->node.size = size;
		vma->node.color = obj->cache_level;
		ret = drm_mm_reserve_node(&vma->vm->mm, &vma->node);
		if (ret) {
			ret = i915_gem_evict_for_vma(vma);
			if (ret == 0)
				ret = drm_mm_reserve_node(&vma->vm->mm, &vma->node);
			if (ret)
				goto err_unpin;
		}
	} else {
		u32 search_flag, alloc_flag;

		if (flags & PIN_HIGH) {
			search_flag = DRM_MM_SEARCH_BELOW;
			alloc_flag = DRM_MM_CREATE_TOP;
		} else {
			search_flag = DRM_MM_SEARCH_DEFAULT;
			alloc_flag = DRM_MM_CREATE_DEFAULT;
		}

		/* We only allocate in PAGE_SIZE/GTT_PAGE_SIZE (4096) chunks,
		 * so we know that we always have a minimum alignment of 4096.
		 * The drm_mm range manager is optimised to return results
		 * with zero alignment, so where possible use the optimal
		 * path.
		 */
		if (alignment <= 4096)
			alignment = 0;

search_free:
		ret = drm_mm_insert_node_in_range_generic(&vma->vm->mm,
							  &vma->node,
							  size, alignment,
							  obj->cache_level,
							  start, end,
							  search_flag,
							  alloc_flag);
		if (ret) {
			ret = i915_gem_evict_something(vma->vm, size, alignment,
						       obj->cache_level,
						       start, end,
						       flags);
			if (ret == 0)
				goto search_free;

			goto err_unpin;
		}
	}
	GEM_BUG_ON(!i915_gem_valid_gtt_space(vma, obj->cache_level));

	list_move_tail(&obj->global_list, &dev_priv->mm.bound_list);
	list_move_tail(&vma->vm_link, &vma->vm->inactive_list);
	obj->bind_count++;

	return 0;

err_unpin:
	i915_gem_object_unpin_pages(obj);
	return ret;
>>>>>>> v4.9.227
}

bool
i915_gem_clflush_object(struct drm_i915_gem_object *obj,
			bool force)
{
	/* If we don't have a page list set up, then we're not pinned
	 * to GPU, and we can ignore the cache flush because it'll happen
	 * again at bind time.
	 */
	if (obj->pages == NULL)
		return false;

	/*
	 * Stolen memory is always coherent with the GPU as it is explicitly
	 * marked as wc by the system, or the system is cache-coherent.
	 */
	if (obj->stolen || obj->phys_handle)
		return false;

	/* If the GPU is snooping the contents of the CPU cache,
	 * we do not need to manually clear the CPU cache lines.  However,
	 * the caches are only snooped when the render cache is
	 * flushed/invalidated.  As we always have to emit invalidations
	 * and flushes when moving into and out of the RENDER domain, correct
	 * snooping behaviour occurs naturally as the result of our domain
	 * tracking.
	 */
	if (!force && cpu_cache_is_coherent(obj->base.dev, obj->cache_level)) {
		obj->cache_dirty = true;
		return false;
	}

	trace_i915_gem_object_clflush(obj);
	drm_clflush_sg(obj->pages);
	obj->cache_dirty = false;

	return true;
}

/** Flushes the GTT write domain for the object if it's dirty. */
static void
i915_gem_object_flush_gtt_write_domain(struct drm_i915_gem_object *obj)
{
<<<<<<< HEAD
	uint32_t old_write_domain;
=======
	struct drm_i915_private *dev_priv = to_i915(obj->base.dev);
>>>>>>> v4.9.227

	if (obj->base.write_domain != I915_GEM_DOMAIN_GTT)
		return;

	/* No actual flushing is required for the GTT write domain.  Writes
<<<<<<< HEAD
	 * to it immediately go to main memory as far as we know, so there's
=======
	 * to it "immediately" go to main memory as far as we know, so there's
>>>>>>> v4.9.227
	 * no chipset flush.  It also doesn't land in render cache.
	 *
	 * However, we do have to enforce the order so that all writes through
	 * the GTT land before any writes to the device, such as updates to
	 * the GATT itself.
<<<<<<< HEAD
	 */
	wmb();

	old_write_domain = obj->base.write_domain;
	obj->base.write_domain = 0;

	intel_fb_obj_flush(obj, false, ORIGIN_GTT);

	trace_i915_gem_object_change_domain(obj,
					    obj->base.read_domains,
					    old_write_domain);
=======
	 *
	 * We also have to wait a bit for the writes to land from the GTT.
	 * An uncached read (i.e. mmio) seems to be ideal for the round-trip
	 * timing. This issue has only been observed when switching quickly
	 * between GTT writes and CPU reads from inside the kernel on recent hw,
	 * and it appears to only affect discrete GTT blocks (i.e. on LLC
	 * system agents we cannot reproduce this behaviour).
	 */
	wmb();
	if (INTEL_GEN(dev_priv) >= 6 && !HAS_LLC(dev_priv))
		POSTING_READ(RING_ACTHD(dev_priv->engine[RCS].mmio_base));

	intel_fb_obj_flush(obj, false, write_origin(obj, I915_GEM_DOMAIN_GTT));

	obj->base.write_domain = 0;
	trace_i915_gem_object_change_domain(obj,
					    obj->base.read_domains,
					    I915_GEM_DOMAIN_GTT);
>>>>>>> v4.9.227
}

/** Flushes the CPU write domain for the object if it's dirty. */
static void
i915_gem_object_flush_cpu_write_domain(struct drm_i915_gem_object *obj)
{
<<<<<<< HEAD
	uint32_t old_write_domain;

=======
>>>>>>> v4.9.227
	if (obj->base.write_domain != I915_GEM_DOMAIN_CPU)
		return;

	if (i915_gem_clflush_object(obj, obj->pin_display))
<<<<<<< HEAD
		i915_gem_chipset_flush(obj->base.dev);

	old_write_domain = obj->base.write_domain;
	obj->base.write_domain = 0;

	intel_fb_obj_flush(obj, false, ORIGIN_CPU);

	trace_i915_gem_object_change_domain(obj,
					    obj->base.read_domains,
					    old_write_domain);
=======
		i915_gem_chipset_flush(to_i915(obj->base.dev));

	intel_fb_obj_flush(obj, false, ORIGIN_CPU);

	obj->base.write_domain = 0;
	trace_i915_gem_object_change_domain(obj,
					    obj->base.read_domains,
					    I915_GEM_DOMAIN_CPU);
}

static void i915_gem_object_bump_inactive_ggtt(struct drm_i915_gem_object *obj)
{
	struct i915_vma *vma;

	list_for_each_entry(vma, &obj->vma_list, obj_link) {
		if (!i915_vma_is_ggtt(vma))
			continue;

		if (i915_vma_is_active(vma))
			continue;

		if (!drm_mm_node_allocated(&vma->node))
			continue;

		list_move_tail(&vma->vm_link, &vma->vm->inactive_list);
	}
>>>>>>> v4.9.227
}

/**
 * Moves a single object to the GTT read, and possibly write domain.
<<<<<<< HEAD
=======
 * @obj: object to act on
 * @write: ask for write access or read only
>>>>>>> v4.9.227
 *
 * This function returns when the move is complete, including waiting on
 * flushes to occur.
 */
int
i915_gem_object_set_to_gtt_domain(struct drm_i915_gem_object *obj, bool write)
{
	uint32_t old_write_domain, old_read_domains;
<<<<<<< HEAD
	struct i915_vma *vma;
	int ret;

	if (obj->base.write_domain == I915_GEM_DOMAIN_GTT)
		return 0;

=======
	int ret;

>>>>>>> v4.9.227
	ret = i915_gem_object_wait_rendering(obj, !write);
	if (ret)
		return ret;

<<<<<<< HEAD
=======
	if (obj->base.write_domain == I915_GEM_DOMAIN_GTT)
		return 0;

>>>>>>> v4.9.227
	/* Flush and acquire obj->pages so that we are coherent through
	 * direct access in memory with previous cached writes through
	 * shmemfs and that our cache domain tracking remains valid.
	 * For example, if the obj->filp was moved to swap without us
	 * being notified and releasing the pages, we would mistakenly
	 * continue to assume that the obj remained out of the CPU cached
	 * domain.
	 */
	ret = i915_gem_object_get_pages(obj);
	if (ret)
		return ret;

	i915_gem_object_flush_cpu_write_domain(obj);

	/* Serialise direct access to this object with the barriers for
	 * coherent writes from the GPU, by effectively invalidating the
	 * GTT domain upon first access.
	 */
	if ((obj->base.read_domains & I915_GEM_DOMAIN_GTT) == 0)
		mb();

	old_write_domain = obj->base.write_domain;
	old_read_domains = obj->base.read_domains;

	/* It should now be out of any other write domains, and we can update
	 * the domain values for our changes.
	 */
	BUG_ON((obj->base.write_domain & ~I915_GEM_DOMAIN_GTT) != 0);
	obj->base.read_domains |= I915_GEM_DOMAIN_GTT;
	if (write) {
		obj->base.read_domains = I915_GEM_DOMAIN_GTT;
		obj->base.write_domain = I915_GEM_DOMAIN_GTT;
		obj->dirty = 1;
	}

	trace_i915_gem_object_change_domain(obj,
					    old_read_domains,
					    old_write_domain);

	/* And bump the LRU for this access */
<<<<<<< HEAD
	vma = i915_gem_obj_to_ggtt(obj);
	if (vma && drm_mm_node_allocated(&vma->node) && !obj->active)
		list_move_tail(&vma->mm_list,
			       &to_i915(obj->base.dev)->gtt.base.inactive_list);
=======
	i915_gem_object_bump_inactive_ggtt(obj);
>>>>>>> v4.9.227

	return 0;
}

/**
 * Changes the cache-level of an object across all VMA.
<<<<<<< HEAD
=======
 * @obj: object to act on
 * @cache_level: new cache level to set for the object
>>>>>>> v4.9.227
 *
 * After this function returns, the object will be in the new cache-level
 * across all GTT and the contents of the backing storage will be coherent,
 * with respect to the new cache-level. In order to keep the backing storage
 * coherent for all users, we only allow a single cache level to be set
 * globally on the object and prevent it from being changed whilst the
 * hardware is reading from the object. That is if the object is currently
 * on the scanout it will be set to uncached (or equivalent display
 * cache coherency) and all non-MOCS GPU access will also be uncached so
 * that all direct access to the scanout remains coherent.
 */
int i915_gem_object_set_cache_level(struct drm_i915_gem_object *obj,
				    enum i915_cache_level cache_level)
{
<<<<<<< HEAD
	struct drm_device *dev = obj->base.dev;
	struct i915_vma *vma, *next;
	bool bound = false;
=======
	struct i915_vma *vma;
>>>>>>> v4.9.227
	int ret = 0;

	if (obj->cache_level == cache_level)
		goto out;

	/* Inspect the list of currently bound VMA and unbind any that would
	 * be invalid given the new cache-level. This is principally to
	 * catch the issue of the CS prefetch crossing page boundaries and
	 * reading an invalid PTE on older architectures.
	 */
<<<<<<< HEAD
	list_for_each_entry_safe(vma, next, &obj->vma_list, vma_link) {
		if (!drm_mm_node_allocated(&vma->node))
			continue;

		if (vma->pin_count) {
=======
restart:
	list_for_each_entry(vma, &obj->vma_list, obj_link) {
		if (!drm_mm_node_allocated(&vma->node))
			continue;

		if (i915_vma_is_pinned(vma)) {
>>>>>>> v4.9.227
			DRM_DEBUG("can not change the cache level of pinned objects\n");
			return -EBUSY;
		}

<<<<<<< HEAD
		if (!i915_gem_valid_gtt_space(vma, cache_level)) {
			ret = i915_vma_unbind(vma);
			if (ret)
				return ret;
		} else
			bound = true;
=======
		if (i915_gem_valid_gtt_space(vma, cache_level))
			continue;

		ret = i915_vma_unbind(vma);
		if (ret)
			return ret;

		/* As unbinding may affect other elements in the
		 * obj->vma_list (due to side-effects from retiring
		 * an active vma), play safe and restart the iterator.
		 */
		goto restart;
>>>>>>> v4.9.227
	}

	/* We can reuse the existing drm_mm nodes but need to change the
	 * cache-level on the PTE. We could simply unbind them all and
	 * rebind with the correct cache-level on next use. However since
	 * we already have a valid slot, dma mapping, pages etc, we may as
	 * rewrite the PTE in the belief that doing so tramples upon less
	 * state and so involves less work.
	 */
<<<<<<< HEAD
	if (bound) {
=======
	if (obj->bind_count) {
>>>>>>> v4.9.227
		/* Before we change the PTE, the GPU must not be accessing it.
		 * If we wait upon the object, we know that all the bound
		 * VMA are no longer active.
		 */
		ret = i915_gem_object_wait_rendering(obj, false);
		if (ret)
			return ret;

<<<<<<< HEAD
		if (!HAS_LLC(dev) && cache_level != I915_CACHE_NONE) {
=======
		if (!HAS_LLC(obj->base.dev) && cache_level != I915_CACHE_NONE) {
>>>>>>> v4.9.227
			/* Access to snoopable pages through the GTT is
			 * incoherent and on some machines causes a hard
			 * lockup. Relinquish the CPU mmaping to force
			 * userspace to refault in the pages and we can
			 * then double check if the GTT mapping is still
			 * valid for that pointer access.
			 */
			i915_gem_release_mmap(obj);

			/* As we no longer need a fence for GTT access,
			 * we can relinquish it now (and so prevent having
			 * to steal a fence from someone else on the next
			 * fence request). Note GPU activity would have
			 * dropped the fence as all snoopable access is
			 * supposed to be linear.
			 */
<<<<<<< HEAD
			ret = i915_gem_object_put_fence(obj);
			if (ret)
				return ret;
=======
			list_for_each_entry(vma, &obj->vma_list, obj_link) {
				ret = i915_vma_put_fence(vma);
				if (ret)
					return ret;
			}
>>>>>>> v4.9.227
		} else {
			/* We either have incoherent backing store and
			 * so no GTT access or the architecture is fully
			 * coherent. In such cases, existing GTT mmaps
			 * ignore the cache bit in the PTE and we can
			 * rewrite it without confusing the GPU or having
			 * to force userspace to fault back in its mmaps.
			 */
		}

<<<<<<< HEAD
		list_for_each_entry(vma, &obj->vma_list, vma_link) {
=======
		list_for_each_entry(vma, &obj->vma_list, obj_link) {
>>>>>>> v4.9.227
			if (!drm_mm_node_allocated(&vma->node))
				continue;

			ret = i915_vma_bind(vma, cache_level, PIN_UPDATE);
			if (ret)
				return ret;
		}
	}

<<<<<<< HEAD
	list_for_each_entry(vma, &obj->vma_list, vma_link)
=======
	list_for_each_entry(vma, &obj->vma_list, obj_link)
>>>>>>> v4.9.227
		vma->node.color = cache_level;
	obj->cache_level = cache_level;

out:
	/* Flush the dirty CPU caches to the backing storage so that the
	 * object is now coherent at its new cache level (with respect
	 * to the access domain).
	 */
<<<<<<< HEAD
	if (obj->cache_dirty &&
	    obj->base.write_domain != I915_GEM_DOMAIN_CPU &&
	    cpu_write_needs_clflush(obj)) {
		if (i915_gem_clflush_object(obj, true))
			i915_gem_chipset_flush(obj->base.dev);
=======
	if (obj->cache_dirty && cpu_write_needs_clflush(obj)) {
		if (i915_gem_clflush_object(obj, true))
			i915_gem_chipset_flush(to_i915(obj->base.dev));
>>>>>>> v4.9.227
	}

	return 0;
}

int i915_gem_get_caching_ioctl(struct drm_device *dev, void *data,
			       struct drm_file *file)
{
	struct drm_i915_gem_caching *args = data;
	struct drm_i915_gem_object *obj;

<<<<<<< HEAD
	obj = to_intel_bo(drm_gem_object_lookup(dev, file, args->handle));
	if (&obj->base == NULL)
=======
	obj = i915_gem_object_lookup(file, args->handle);
	if (!obj)
>>>>>>> v4.9.227
		return -ENOENT;

	switch (obj->cache_level) {
	case I915_CACHE_LLC:
	case I915_CACHE_L3_LLC:
		args->caching = I915_CACHING_CACHED;
		break;

	case I915_CACHE_WT:
		args->caching = I915_CACHING_DISPLAY;
		break;

	default:
		args->caching = I915_CACHING_NONE;
		break;
	}

<<<<<<< HEAD
	drm_gem_object_unreference_unlocked(&obj->base);
=======
	i915_gem_object_put_unlocked(obj);
>>>>>>> v4.9.227
	return 0;
}

int i915_gem_set_caching_ioctl(struct drm_device *dev, void *data,
			       struct drm_file *file)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
=======
	struct drm_i915_private *dev_priv = to_i915(dev);
>>>>>>> v4.9.227
	struct drm_i915_gem_caching *args = data;
	struct drm_i915_gem_object *obj;
	enum i915_cache_level level;
	int ret;

	switch (args->caching) {
	case I915_CACHING_NONE:
		level = I915_CACHE_NONE;
		break;
	case I915_CACHING_CACHED:
		/*
		 * Due to a HW issue on BXT A stepping, GPU stores via a
		 * snooped mapping may leave stale data in a corresponding CPU
		 * cacheline, whereas normally such cachelines would get
		 * invalidated.
		 */
<<<<<<< HEAD
		if (IS_BROXTON(dev) && INTEL_REVID(dev) < BXT_REVID_B0)
=======
		if (!HAS_LLC(dev) && !HAS_SNOOP(dev))
>>>>>>> v4.9.227
			return -ENODEV;

		level = I915_CACHE_LLC;
		break;
	case I915_CACHING_DISPLAY:
		level = HAS_WT(dev) ? I915_CACHE_WT : I915_CACHE_NONE;
		break;
	default:
		return -EINVAL;
	}

	intel_runtime_pm_get(dev_priv);

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		goto rpm_put;

<<<<<<< HEAD
	obj = to_intel_bo(drm_gem_object_lookup(dev, file, args->handle));
	if (&obj->base == NULL) {
=======
	obj = i915_gem_object_lookup(file, args->handle);
	if (!obj) {
>>>>>>> v4.9.227
		ret = -ENOENT;
		goto unlock;
	}

	ret = i915_gem_object_set_cache_level(obj, level);

<<<<<<< HEAD
	drm_gem_object_unreference(&obj->base);
=======
	i915_gem_object_put(obj);
>>>>>>> v4.9.227
unlock:
	mutex_unlock(&dev->struct_mutex);
rpm_put:
	intel_runtime_pm_put(dev_priv);

	return ret;
}

/*
 * Prepare buffer for display plane (scanout, cursors, etc).
 * Can be called from an uninterruptible phase (modesetting) and allows
 * any flushes to be pipelined (for pageflips).
 */
<<<<<<< HEAD
int
i915_gem_object_pin_to_display_plane(struct drm_i915_gem_object *obj,
				     u32 alignment,
				     struct intel_engine_cs *pipelined,
				     struct drm_i915_gem_request **pipelined_request,
				     const struct i915_ggtt_view *view)
{
	u32 old_read_domains, old_write_domain;
	int ret;

	ret = i915_gem_object_sync(obj, pipelined, pipelined_request);
	if (ret)
		return ret;

=======
struct i915_vma *
i915_gem_object_pin_to_display_plane(struct drm_i915_gem_object *obj,
				     u32 alignment,
				     const struct i915_ggtt_view *view)
{
	struct i915_vma *vma;
	u32 old_read_domains, old_write_domain;
	int ret;

>>>>>>> v4.9.227
	/* Mark the pin_display early so that we account for the
	 * display coherency whilst setting up the cache domains.
	 */
	obj->pin_display++;

	/* The display engine is not coherent with the LLC cache on gen6.  As
	 * a result, we make sure that the pinning that is about to occur is
	 * done with uncached PTEs. This is lowest common denominator for all
	 * chipsets.
	 *
	 * However for gen6+, we could do better by using the GFDT bit instead
	 * of uncaching, which would allow us to flush all the LLC-cached data
	 * with that bit in the PTE to main memory with just one PIPE_CONTROL.
	 */
	ret = i915_gem_object_set_cache_level(obj,
					      HAS_WT(obj->base.dev) ? I915_CACHE_WT : I915_CACHE_NONE);
<<<<<<< HEAD
	if (ret)
		goto err_unpin_display;

	/* As the user may map the buffer once pinned in the display plane
	 * (e.g. libkms for the bootup splash), we have to ensure that we
	 * always use map_and_fenceable for all scanout buffers.
	 */
	ret = i915_gem_object_ggtt_pin(obj, view, alignment,
				       view->type == I915_GGTT_VIEW_NORMAL ?
				       PIN_MAPPABLE : 0);
	if (ret)
		goto err_unpin_display;

=======
	if (ret) {
		vma = ERR_PTR(ret);
		goto err_unpin_display;
	}

	/* As the user may map the buffer once pinned in the display plane
	 * (e.g. libkms for the bootup splash), we have to ensure that we
	 * always use map_and_fenceable for all scanout buffers. However,
	 * it may simply be too big to fit into mappable, in which case
	 * put it anyway and hope that userspace can cope (but always first
	 * try to preserve the existing ABI).
	 */
	vma = ERR_PTR(-ENOSPC);
	if (view->type == I915_GGTT_VIEW_NORMAL)
		vma = i915_gem_object_ggtt_pin(obj, view, 0, alignment,
					       PIN_MAPPABLE | PIN_NONBLOCK);
	if (IS_ERR(vma)) {
		struct drm_i915_private *i915 = to_i915(obj->base.dev);
		unsigned int flags;

		/* Valleyview is definitely limited to scanning out the first
		 * 512MiB. Lets presume this behaviour was inherited from the
		 * g4x display engine and that all earlier gen are similarly
		 * limited. Testing suggests that it is a little more
		 * complicated than this. For example, Cherryview appears quite
		 * happy to scanout from anywhere within its global aperture.
		 */
		flags = 0;
		if (HAS_GMCH_DISPLAY(i915))
			flags = PIN_MAPPABLE;
		vma = i915_gem_object_ggtt_pin(obj, view, 0, alignment, flags);
	}
	if (IS_ERR(vma))
		goto err_unpin_display;

	vma->display_alignment = max_t(u64, vma->display_alignment, alignment);

>>>>>>> v4.9.227
	i915_gem_object_flush_cpu_write_domain(obj);

	old_write_domain = obj->base.write_domain;
	old_read_domains = obj->base.read_domains;

	/* It should now be out of any other write domains, and we can update
	 * the domain values for our changes.
	 */
	obj->base.write_domain = 0;
	obj->base.read_domains |= I915_GEM_DOMAIN_GTT;

	trace_i915_gem_object_change_domain(obj,
					    old_read_domains,
					    old_write_domain);

<<<<<<< HEAD
	return 0;

err_unpin_display:
	obj->pin_display--;
	return ret;
}

void
i915_gem_object_unpin_from_display_plane(struct drm_i915_gem_object *obj,
					 const struct i915_ggtt_view *view)
{
	if (WARN_ON(obj->pin_display == 0))
		return;

	i915_gem_object_ggtt_unpin_view(obj, view);

	obj->pin_display--;
=======
	return vma;

err_unpin_display:
	obj->pin_display--;
	return vma;
}

void
i915_gem_object_unpin_from_display_plane(struct i915_vma *vma)
{
	if (WARN_ON(vma->obj->pin_display == 0))
		return;

	if (--vma->obj->pin_display == 0)
		vma->display_alignment = 0;

	/* Bump the LRU to try and avoid premature eviction whilst flipping  */
	if (!i915_vma_is_active(vma))
		list_move_tail(&vma->vm_link, &vma->vm->inactive_list);

	i915_vma_unpin(vma);
>>>>>>> v4.9.227
}

/**
 * Moves a single object to the CPU read, and possibly write domain.
<<<<<<< HEAD
=======
 * @obj: object to act on
 * @write: requesting write or read-only access
>>>>>>> v4.9.227
 *
 * This function returns when the move is complete, including waiting on
 * flushes to occur.
 */
int
i915_gem_object_set_to_cpu_domain(struct drm_i915_gem_object *obj, bool write)
{
	uint32_t old_write_domain, old_read_domains;
	int ret;

<<<<<<< HEAD
	if (obj->base.write_domain == I915_GEM_DOMAIN_CPU)
		return 0;

=======
>>>>>>> v4.9.227
	ret = i915_gem_object_wait_rendering(obj, !write);
	if (ret)
		return ret;

<<<<<<< HEAD
=======
	if (obj->base.write_domain == I915_GEM_DOMAIN_CPU)
		return 0;

>>>>>>> v4.9.227
	i915_gem_object_flush_gtt_write_domain(obj);

	old_write_domain = obj->base.write_domain;
	old_read_domains = obj->base.read_domains;

	/* Flush the CPU cache if it's still invalid. */
	if ((obj->base.read_domains & I915_GEM_DOMAIN_CPU) == 0) {
		i915_gem_clflush_object(obj, false);

		obj->base.read_domains |= I915_GEM_DOMAIN_CPU;
	}

	/* It should now be out of any other write domains, and we can update
	 * the domain values for our changes.
	 */
	BUG_ON((obj->base.write_domain & ~I915_GEM_DOMAIN_CPU) != 0);

	/* If we're writing through the CPU, then the GPU read domains will
	 * need to be invalidated at next use.
	 */
	if (write) {
		obj->base.read_domains = I915_GEM_DOMAIN_CPU;
		obj->base.write_domain = I915_GEM_DOMAIN_CPU;
	}

	trace_i915_gem_object_change_domain(obj,
					    old_read_domains,
					    old_write_domain);

	return 0;
}

/* Throttle our rendering by waiting until the ring has completed our requests
 * emitted over 20 msec ago.
 *
 * Note that if we were to use the current jiffies each time around the loop,
 * we wouldn't escape the function with any frames outstanding if the time to
 * render a frame was over 20ms.
 *
 * This should get us reasonable parallelism between CPU and GPU but also
 * relatively low latency when blocking on a particular request to finish.
 */
static int
i915_gem_ring_throttle(struct drm_device *dev, struct drm_file *file)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct drm_i915_file_private *file_priv = file->driver_priv;
	unsigned long recent_enough = jiffies - DRM_I915_THROTTLE_JIFFIES;
	struct drm_i915_gem_request *request, *target = NULL;
	unsigned reset_counter;
=======
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct drm_i915_file_private *file_priv = file->driver_priv;
	unsigned long recent_enough = jiffies - DRM_I915_THROTTLE_JIFFIES;
	struct drm_i915_gem_request *request, *target = NULL;
>>>>>>> v4.9.227
	int ret;

	ret = i915_gem_wait_for_error(&dev_priv->gpu_error);
	if (ret)
		return ret;

<<<<<<< HEAD
	ret = i915_gem_check_wedge(&dev_priv->gpu_error, false);
	if (ret)
		return ret;
=======
	/* ABI: return -EIO if already wedged */
	if (i915_terminally_wedged(&dev_priv->gpu_error))
		return -EIO;
>>>>>>> v4.9.227

	spin_lock(&file_priv->mm.lock);
	list_for_each_entry(request, &file_priv->mm.request_list, client_list) {
		if (time_after_eq(request->emitted_jiffies, recent_enough))
			break;

		/*
		 * Note that the request might not have been submitted yet.
		 * In which case emitted_jiffies will be zero.
		 */
		if (!request->emitted_jiffies)
			continue;

		target = request;
	}
<<<<<<< HEAD
	reset_counter = atomic_read(&dev_priv->gpu_error.reset_counter);
	if (target)
		i915_gem_request_reference(target);
=======
	if (target)
		i915_gem_request_get(target);
>>>>>>> v4.9.227
	spin_unlock(&file_priv->mm.lock);

	if (target == NULL)
		return 0;

<<<<<<< HEAD
	ret = __i915_wait_request(target, reset_counter, true, NULL, NULL);
	if (ret == 0)
		queue_delayed_work(dev_priv->wq, &dev_priv->mm.retire_work, 0);

	i915_gem_request_unreference__unlocked(target);
=======
	ret = i915_wait_request(target, I915_WAIT_INTERRUPTIBLE, NULL, NULL);
	i915_gem_request_put(target);
>>>>>>> v4.9.227

	return ret;
}

static bool
<<<<<<< HEAD
i915_vma_misplaced(struct i915_vma *vma, uint32_t alignment, uint64_t flags)
{
	struct drm_i915_gem_object *obj = vma->obj;

	if (alignment &&
	    vma->node.start & (alignment - 1))
		return true;

	if (flags & PIN_MAPPABLE && !obj->map_and_fenceable)
=======
i915_vma_misplaced(struct i915_vma *vma, u64 size, u64 alignment, u64 flags)
{
	if (!drm_mm_node_allocated(&vma->node))
		return false;

	if (vma->node.size < size)
		return true;

	if (alignment && vma->node.start & (alignment - 1))
		return true;

	if (flags & PIN_MAPPABLE && !i915_vma_is_map_and_fenceable(vma))
>>>>>>> v4.9.227
		return true;

	if (flags & PIN_OFFSET_BIAS &&
	    vma->node.start < (flags & PIN_OFFSET_MASK))
		return true;

<<<<<<< HEAD
=======
	if (flags & PIN_OFFSET_FIXED &&
	    vma->node.start != (flags & PIN_OFFSET_MASK))
		return true;

>>>>>>> v4.9.227
	return false;
}

void __i915_vma_set_map_and_fenceable(struct i915_vma *vma)
{
	struct drm_i915_gem_object *obj = vma->obj;
<<<<<<< HEAD
	bool mappable, fenceable;
	u32 fence_size, fence_alignment;

	fence_size = i915_gem_get_gtt_size(obj->base.dev,
					   obj->base.size,
					   obj->tiling_mode);
	fence_alignment = i915_gem_get_gtt_alignment(obj->base.dev,
						     obj->base.size,
						     obj->tiling_mode,
						     true);
=======
	struct drm_i915_private *dev_priv = to_i915(obj->base.dev);
	bool mappable, fenceable;
	u32 fence_size, fence_alignment;

	fence_size = i915_gem_get_ggtt_size(dev_priv,
					    vma->size,
					    i915_gem_object_get_tiling(obj));
	fence_alignment = i915_gem_get_ggtt_alignment(dev_priv,
						      vma->size,
						      i915_gem_object_get_tiling(obj),
						      true);
>>>>>>> v4.9.227

	fenceable = (vma->node.size == fence_size &&
		     (vma->node.start & (fence_alignment - 1)) == 0);

	mappable = (vma->node.start + fence_size <=
<<<<<<< HEAD
		    to_i915(obj->base.dev)->gtt.mappable_end);

	obj->map_and_fenceable = mappable && fenceable;
}

static int
i915_gem_object_do_pin(struct drm_i915_gem_object *obj,
		       struct i915_address_space *vm,
		       const struct i915_ggtt_view *ggtt_view,
		       uint32_t alignment,
		       uint64_t flags)
{
	struct drm_i915_private *dev_priv = obj->base.dev->dev_private;
	struct i915_vma *vma;
	unsigned bound;
	int ret;

	if (WARN_ON(vm == &dev_priv->mm.aliasing_ppgtt->base))
		return -ENODEV;

	if (WARN_ON(flags & (PIN_GLOBAL | PIN_MAPPABLE) && !i915_is_ggtt(vm)))
		return -EINVAL;

	if (WARN_ON((flags & (PIN_MAPPABLE | PIN_GLOBAL)) == PIN_MAPPABLE))
		return -EINVAL;

	if (WARN_ON(i915_is_ggtt(vm) != !!ggtt_view))
		return -EINVAL;

	vma = ggtt_view ? i915_gem_obj_to_ggtt_view(obj, ggtt_view) :
			  i915_gem_obj_to_vma(obj, vm);

	if (IS_ERR(vma))
		return PTR_ERR(vma);

	if (vma) {
		if (WARN_ON(vma->pin_count == DRM_I915_GEM_OBJECT_MAX_PIN_COUNT))
			return -EBUSY;

		if (i915_vma_misplaced(vma, alignment, flags)) {
			WARN(vma->pin_count,
			     "bo is already pinned in %s with incorrect alignment:"
			     " offset=%08x %08x, req.alignment=%x, req.map_and_fenceable=%d,"
			     " obj->map_and_fenceable=%d\n",
			     ggtt_view ? "ggtt" : "ppgtt",
			     upper_32_bits(vma->node.start),
			     lower_32_bits(vma->node.start),
			     alignment,
			     !!(flags & PIN_MAPPABLE),
			     obj->map_and_fenceable);
			ret = i915_vma_unbind(vma);
			if (ret)
				return ret;

			vma = NULL;
		}
	}

	bound = vma ? vma->bound : 0;
	if (vma == NULL || !drm_mm_node_allocated(&vma->node)) {
		vma = i915_gem_object_bind_to_vm(obj, vm, ggtt_view, alignment,
						 flags);
		if (IS_ERR(vma))
			return PTR_ERR(vma);
	} else {
		ret = i915_vma_bind(vma, obj->cache_level, flags);
		if (ret)
			return ret;
	}

	if (ggtt_view && ggtt_view->type == I915_GGTT_VIEW_NORMAL &&
	    (bound ^ vma->bound) & GLOBAL_BIND) {
		__i915_vma_set_map_and_fenceable(vma);
		WARN_ON(flags & PIN_MAPPABLE && !obj->map_and_fenceable);
	}

	vma->pin_count++;
	return 0;
}

int
i915_gem_object_pin(struct drm_i915_gem_object *obj,
		    struct i915_address_space *vm,
		    uint32_t alignment,
		    uint64_t flags)
{
	return i915_gem_object_do_pin(obj, vm,
				      i915_is_ggtt(vm) ? &i915_ggtt_view_normal : NULL,
				      alignment, flags);
}

int
i915_gem_object_ggtt_pin(struct drm_i915_gem_object *obj,
			 const struct i915_ggtt_view *view,
			 uint32_t alignment,
			 uint64_t flags)
{
	if (WARN_ONCE(!view, "no view specified"))
		return -EINVAL;

	return i915_gem_object_do_pin(obj, i915_obj_to_ggtt(obj), view,
				      alignment, flags | PIN_GLOBAL);
}

void
i915_gem_object_ggtt_unpin_view(struct drm_i915_gem_object *obj,
				const struct i915_ggtt_view *view)
{
	struct i915_vma *vma = i915_gem_obj_to_ggtt_view(obj, view);

	BUG_ON(!vma);
	WARN_ON(vma->pin_count == 0);
	WARN_ON(!i915_gem_obj_ggtt_bound_view(obj, view));

	--vma->pin_count;
=======
		    dev_priv->ggtt.mappable_end);

	/*
	 * Explicitly disable for rotated VMA since the display does not
	 * need the fence and the VMA is not accessible to other users.
	 */
	if (mappable && fenceable &&
	    vma->ggtt_view.type != I915_GGTT_VIEW_ROTATED)
		vma->flags |= I915_VMA_CAN_FENCE;
	else
		vma->flags &= ~I915_VMA_CAN_FENCE;
}

int __i915_vma_do_pin(struct i915_vma *vma,
		      u64 size, u64 alignment, u64 flags)
{
	unsigned int bound = vma->flags;
	int ret;

	GEM_BUG_ON((flags & (PIN_GLOBAL | PIN_USER)) == 0);
	GEM_BUG_ON((flags & PIN_GLOBAL) && !i915_vma_is_ggtt(vma));

	if (WARN_ON(bound & I915_VMA_PIN_OVERFLOW)) {
		ret = -EBUSY;
		goto err;
	}

	if ((bound & I915_VMA_BIND_MASK) == 0) {
		ret = i915_vma_insert(vma, size, alignment, flags);
		if (ret)
			goto err;
	}

	ret = i915_vma_bind(vma, vma->obj->cache_level, flags);
	if (ret)
		goto err;

	if ((bound ^ vma->flags) & I915_VMA_GLOBAL_BIND)
		__i915_vma_set_map_and_fenceable(vma);

	GEM_BUG_ON(i915_vma_misplaced(vma, size, alignment, flags));
	return 0;

err:
	__i915_vma_unpin(vma);
	return ret;
}

struct i915_vma *
i915_gem_object_ggtt_pin(struct drm_i915_gem_object *obj,
			 const struct i915_ggtt_view *view,
			 u64 size,
			 u64 alignment,
			 u64 flags)
{
	struct i915_address_space *vm = &to_i915(obj->base.dev)->ggtt.base;

	return i915_gem_object_pin(obj, vm, view, size, alignment,
				   flags | PIN_GLOBAL);
}

struct i915_vma *
i915_gem_object_pin(struct drm_i915_gem_object *obj,
		    struct i915_address_space *vm,
		    const struct i915_ggtt_view *view,
		    u64 size,
		    u64 alignment,
		    u64 flags)
{
	struct i915_vma *vma;
	int ret;

	vma = i915_gem_obj_lookup_or_create_vma(obj, vm, view);
	if (IS_ERR(vma))
		return vma;

	if (i915_vma_misplaced(vma, size, alignment, flags)) {
		if (flags & PIN_NONBLOCK &&
		    (i915_vma_is_pinned(vma) || i915_vma_is_active(vma)))
			return ERR_PTR(-ENOSPC);

		WARN(i915_vma_is_pinned(vma),
		     "bo is already pinned in ggtt with incorrect alignment:"
		     " offset=%08x, req.alignment=%llx,"
		     " req.map_and_fenceable=%d, vma->map_and_fenceable=%d\n",
		     i915_ggtt_offset(vma), alignment,
		     !!(flags & PIN_MAPPABLE),
		     i915_vma_is_map_and_fenceable(vma));
		ret = i915_vma_unbind(vma);
		if (ret)
			return ERR_PTR(ret);
	}

	ret = i915_vma_pin(vma, size, alignment, flags);
	if (ret)
		return ERR_PTR(ret);

	return vma;
}

static __always_inline unsigned int __busy_read_flag(unsigned int id)
{
	/* Note that we could alias engines in the execbuf API, but
	 * that would be very unwise as it prevents userspace from
	 * fine control over engine selection. Ahem.
	 *
	 * This should be something like EXEC_MAX_ENGINE instead of
	 * I915_NUM_ENGINES.
	 */
	BUILD_BUG_ON(I915_NUM_ENGINES > 16);
	return 0x10000 << id;
}

static __always_inline unsigned int __busy_write_id(unsigned int id)
{
	/* The uABI guarantees an active writer is also amongst the read
	 * engines. This would be true if we accessed the activity tracking
	 * under the lock, but as we perform the lookup of the object and
	 * its activity locklessly we can not guarantee that the last_write
	 * being active implies that we have set the same engine flag from
	 * last_read - hence we always set both read and write busy for
	 * last_write.
	 */
	return id | __busy_read_flag(id);
}

static __always_inline unsigned int
__busy_set_if_active(const struct i915_gem_active *active,
		     unsigned int (*flag)(unsigned int id))
{
	struct drm_i915_gem_request *request;

	request = rcu_dereference(active->request);
	if (!request || i915_gem_request_completed(request))
		return 0;

	/* This is racy. See __i915_gem_active_get_rcu() for an in detail
	 * discussion of how to handle the race correctly, but for reporting
	 * the busy state we err on the side of potentially reporting the
	 * wrong engine as being busy (but we guarantee that the result
	 * is at least self-consistent).
	 *
	 * As we use SLAB_DESTROY_BY_RCU, the request may be reallocated
	 * whilst we are inspecting it, even under the RCU read lock as we are.
	 * This means that there is a small window for the engine and/or the
	 * seqno to have been overwritten. The seqno will always be in the
	 * future compared to the intended, and so we know that if that
	 * seqno is idle (on whatever engine) our request is idle and the
	 * return 0 above is correct.
	 *
	 * The issue is that if the engine is switched, it is just as likely
	 * to report that it is busy (but since the switch happened, we know
	 * the request should be idle). So there is a small chance that a busy
	 * result is actually the wrong engine.
	 *
	 * So why don't we care?
	 *
	 * For starters, the busy ioctl is a heuristic that is by definition
	 * racy. Even with perfect serialisation in the driver, the hardware
	 * state is constantly advancing - the state we report to the user
	 * is stale.
	 *
	 * The critical information for the busy-ioctl is whether the object
	 * is idle as userspace relies on that to detect whether its next
	 * access will stall, or if it has missed submitting commands to
	 * the hardware allowing the GPU to stall. We never generate a
	 * false-positive for idleness, thus busy-ioctl is reliable at the
	 * most fundamental level, and we maintain the guarantee that a
	 * busy object left to itself will eventually become idle (and stay
	 * idle!).
	 *
	 * We allow ourselves the leeway of potentially misreporting the busy
	 * state because that is an optimisation heuristic that is constantly
	 * in flux. Being quickly able to detect the busy/idle state is much
	 * more important than accurate logging of exactly which engines were
	 * busy.
	 *
	 * For accuracy in reporting the engine, we could use
	 *
	 *	result = 0;
	 *	request = __i915_gem_active_get_rcu(active);
	 *	if (request) {
	 *		if (!i915_gem_request_completed(request))
	 *			result = flag(request->engine->exec_id);
	 *		i915_gem_request_put(request);
	 *	}
	 *
	 * but that still remains susceptible to both hardware and userspace
	 * races. So we accept making the result of that race slightly worse,
	 * given the rarity of the race and its low impact on the result.
	 */
	return flag(READ_ONCE(request->engine->exec_id));
}

static __always_inline unsigned int
busy_check_reader(const struct i915_gem_active *active)
{
	return __busy_set_if_active(active, __busy_read_flag);
}

static __always_inline unsigned int
busy_check_writer(const struct i915_gem_active *active)
{
	return __busy_set_if_active(active, __busy_write_id);
>>>>>>> v4.9.227
}

int
i915_gem_busy_ioctl(struct drm_device *dev, void *data,
		    struct drm_file *file)
{
	struct drm_i915_gem_busy *args = data;
	struct drm_i915_gem_object *obj;
<<<<<<< HEAD
	int ret;

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		return ret;

	obj = to_intel_bo(drm_gem_object_lookup(dev, file, args->handle));
	if (&obj->base == NULL) {
		ret = -ENOENT;
		goto unlock;
	}

	/* Count all active objects as busy, even if they are currently not used
	 * by the gpu. Users of this interface expect objects to eventually
	 * become non-busy without any further actions, therefore emit any
	 * necessary flushes here.
	 */
	ret = i915_gem_object_flush_active(obj);
	if (ret)
		goto unref;

	BUILD_BUG_ON(I915_NUM_RINGS > 16);
	args->busy = obj->active << 16;
	if (obj->last_write_req)
		args->busy |= obj->last_write_req->ring->id;

unref:
	drm_gem_object_unreference(&obj->base);
unlock:
	mutex_unlock(&dev->struct_mutex);
	return ret;
=======
	unsigned long active;

	obj = i915_gem_object_lookup(file, args->handle);
	if (!obj)
		return -ENOENT;

	args->busy = 0;
	active = __I915_BO_ACTIVE(obj);
	if (active) {
		int idx;

		/* Yes, the lookups are intentionally racy.
		 *
		 * First, we cannot simply rely on __I915_BO_ACTIVE. We have
		 * to regard the value as stale and as our ABI guarantees
		 * forward progress, we confirm the status of each active
		 * request with the hardware.
		 *
		 * Even though we guard the pointer lookup by RCU, that only
		 * guarantees that the pointer and its contents remain
		 * dereferencable and does *not* mean that the request we
		 * have is the same as the one being tracked by the object.
		 *
		 * Consider that we lookup the request just as it is being
		 * retired and freed. We take a local copy of the pointer,
		 * but before we add its engine into the busy set, the other
		 * thread reallocates it and assigns it to a task on another
		 * engine with a fresh and incomplete seqno. Guarding against
		 * that requires careful serialisation and reference counting,
		 * i.e. using __i915_gem_active_get_request_rcu(). We don't,
		 * instead we expect that if the result is busy, which engines
		 * are busy is not completely reliable - we only guarantee
		 * that the object was busy.
		 */
		rcu_read_lock();

		for_each_active(active, idx)
			args->busy |= busy_check_reader(&obj->last_read[idx]);

		/* For ABI sanity, we only care that the write engine is in
		 * the set of read engines. This should be ensured by the
		 * ordering of setting last_read/last_write in
		 * i915_vma_move_to_active(), and then in reverse in retire.
		 * However, for good measure, we always report the last_write
		 * request as a busy read as well as being a busy write.
		 *
		 * We don't care that the set of active read/write engines
		 * may change during construction of the result, as it is
		 * equally liable to change before userspace can inspect
		 * the result.
		 */
		args->busy |= busy_check_writer(&obj->last_write);

		rcu_read_unlock();
	}

	i915_gem_object_put_unlocked(obj);
	return 0;
>>>>>>> v4.9.227
}

int
i915_gem_throttle_ioctl(struct drm_device *dev, void *data,
			struct drm_file *file_priv)
{
	return i915_gem_ring_throttle(dev, file_priv);
}

int
i915_gem_madvise_ioctl(struct drm_device *dev, void *data,
		       struct drm_file *file_priv)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
=======
	struct drm_i915_private *dev_priv = to_i915(dev);
>>>>>>> v4.9.227
	struct drm_i915_gem_madvise *args = data;
	struct drm_i915_gem_object *obj;
	int ret;

	switch (args->madv) {
	case I915_MADV_DONTNEED:
	case I915_MADV_WILLNEED:
	    break;
	default:
	    return -EINVAL;
	}

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		return ret;

<<<<<<< HEAD
	obj = to_intel_bo(drm_gem_object_lookup(dev, file_priv, args->handle));
	if (&obj->base == NULL) {
=======
	obj = i915_gem_object_lookup(file_priv, args->handle);
	if (!obj) {
>>>>>>> v4.9.227
		ret = -ENOENT;
		goto unlock;
	}

<<<<<<< HEAD
	if (i915_gem_obj_is_pinned(obj)) {
		ret = -EINVAL;
		goto out;
	}

	if (obj->pages &&
	    obj->tiling_mode != I915_TILING_NONE &&
=======
	if (obj->pages &&
	    i915_gem_object_is_tiled(obj) &&
>>>>>>> v4.9.227
	    dev_priv->quirks & QUIRK_PIN_SWIZZLED_PAGES) {
		if (obj->madv == I915_MADV_WILLNEED)
			i915_gem_object_unpin_pages(obj);
		if (args->madv == I915_MADV_WILLNEED)
			i915_gem_object_pin_pages(obj);
	}

	if (obj->madv != __I915_MADV_PURGED)
		obj->madv = args->madv;

	/* if the object is no longer attached, discard its backing storage */
	if (obj->madv == I915_MADV_DONTNEED && obj->pages == NULL)
		i915_gem_object_truncate(obj);

	args->retained = obj->madv != __I915_MADV_PURGED;

<<<<<<< HEAD
out:
	drm_gem_object_unreference(&obj->base);
=======
	i915_gem_object_put(obj);
>>>>>>> v4.9.227
unlock:
	mutex_unlock(&dev->struct_mutex);
	return ret;
}

void i915_gem_object_init(struct drm_i915_gem_object *obj,
			  const struct drm_i915_gem_object_ops *ops)
{
	int i;

	INIT_LIST_HEAD(&obj->global_list);
<<<<<<< HEAD
	for (i = 0; i < I915_NUM_RINGS; i++)
		INIT_LIST_HEAD(&obj->ring_list[i]);
=======
	for (i = 0; i < I915_NUM_ENGINES; i++)
		init_request_active(&obj->last_read[i],
				    i915_gem_object_retire__read);
	init_request_active(&obj->last_write,
			    i915_gem_object_retire__write);
>>>>>>> v4.9.227
	INIT_LIST_HEAD(&obj->obj_exec_link);
	INIT_LIST_HEAD(&obj->vma_list);
	INIT_LIST_HEAD(&obj->batch_pool_link);

	obj->ops = ops;

<<<<<<< HEAD
	obj->fence_reg = I915_FENCE_REG_NONE;
	obj->madv = I915_MADV_WILLNEED;

	i915_gem_info_add_obj(obj->base.dev->dev_private, obj->base.size);
}

static const struct drm_i915_gem_object_ops i915_gem_object_ops = {
=======
	obj->frontbuffer_ggtt_origin = ORIGIN_GTT;
	obj->madv = I915_MADV_WILLNEED;

	i915_gem_info_add_obj(to_i915(obj->base.dev), obj->base.size);
}

static const struct drm_i915_gem_object_ops i915_gem_object_ops = {
	.flags = I915_GEM_OBJECT_HAS_STRUCT_PAGE,
>>>>>>> v4.9.227
	.get_pages = i915_gem_object_get_pages_gtt,
	.put_pages = i915_gem_object_put_pages_gtt,
};

<<<<<<< HEAD
struct drm_i915_gem_object *i915_gem_alloc_object(struct drm_device *dev,
=======
struct drm_i915_gem_object *i915_gem_object_create(struct drm_device *dev,
>>>>>>> v4.9.227
						  size_t size)
{
	struct drm_i915_gem_object *obj;
	struct address_space *mapping;
	gfp_t mask;
<<<<<<< HEAD

	obj = i915_gem_object_alloc(dev);
	if (obj == NULL)
		return NULL;

	if (drm_gem_object_init(dev, &obj->base, size) != 0) {
		i915_gem_object_free(obj);
		return NULL;
	}
=======
	int ret;

	obj = i915_gem_object_alloc(dev);
	if (obj == NULL)
		return ERR_PTR(-ENOMEM);

	ret = drm_gem_object_init(dev, &obj->base, size);
	if (ret)
		goto fail;
>>>>>>> v4.9.227

	mask = GFP_HIGHUSER | __GFP_RECLAIMABLE;
	if (IS_CRESTLINE(dev) || IS_BROADWATER(dev)) {
		/* 965gm cannot relocate objects above 4GiB. */
		mask &= ~__GFP_HIGHMEM;
		mask |= __GFP_DMA32;
	}

<<<<<<< HEAD
	mapping = file_inode(obj->base.filp)->i_mapping;
=======
	mapping = obj->base.filp->f_mapping;
>>>>>>> v4.9.227
	mapping_set_gfp_mask(mapping, mask);

	i915_gem_object_init(obj, &i915_gem_object_ops);

	obj->base.write_domain = I915_GEM_DOMAIN_CPU;
	obj->base.read_domains = I915_GEM_DOMAIN_CPU;

	if (HAS_LLC(dev)) {
		/* On some devices, we can have the GPU use the LLC (the CPU
		 * cache) for about a 10% performance improvement
		 * compared to uncached.  Graphics requests other than
		 * display scanout are coherent with the CPU in
		 * accessing this cache.  This means in this mode we
		 * don't need to clflush on the CPU side, and on the
		 * GPU side we only need to flush internal caches to
		 * get data visible to the CPU.
		 *
		 * However, we maintain the display planes as UC, and so
		 * need to rebind when first used as such.
		 */
		obj->cache_level = I915_CACHE_LLC;
	} else
		obj->cache_level = I915_CACHE_NONE;

	trace_i915_gem_object_create(obj);

	return obj;
<<<<<<< HEAD
=======

fail:
	i915_gem_object_free(obj);

	return ERR_PTR(ret);
>>>>>>> v4.9.227
}

static bool discard_backing_storage(struct drm_i915_gem_object *obj)
{
	/* If we are the last user of the backing storage (be it shmemfs
	 * pages or stolen etc), we know that the pages are going to be
	 * immediately released. In this case, we can then skip copying
	 * back the contents from the GPU.
	 */

	if (obj->madv != I915_MADV_WILLNEED)
		return false;

	if (obj->base.filp == NULL)
		return true;

	/* At first glance, this looks racy, but then again so would be
	 * userspace racing mmap against close. However, the first external
	 * reference to the filp can only be obtained through the
	 * i915_gem_mmap_ioctl() which safeguards us against the user
	 * acquiring such a reference whilst we are in the middle of
	 * freeing the object.
	 */
	return atomic_long_read(&obj->base.filp->f_count) == 1;
}

void i915_gem_free_object(struct drm_gem_object *gem_obj)
{
	struct drm_i915_gem_object *obj = to_intel_bo(gem_obj);
	struct drm_device *dev = obj->base.dev;
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
=======
	struct drm_i915_private *dev_priv = to_i915(dev);
>>>>>>> v4.9.227
	struct i915_vma *vma, *next;

	intel_runtime_pm_get(dev_priv);

	trace_i915_gem_object_destroy(obj);

<<<<<<< HEAD
	list_for_each_entry_safe(vma, next, &obj->vma_list, vma_link) {
		int ret;

		vma->pin_count = 0;
		ret = i915_vma_unbind(vma);
		if (WARN_ON(ret == -ERESTARTSYS)) {
			bool was_interruptible;

			was_interruptible = dev_priv->mm.interruptible;
			dev_priv->mm.interruptible = false;

			WARN_ON(i915_vma_unbind(vma));

			dev_priv->mm.interruptible = was_interruptible;
		}
	}
=======
	/* All file-owned VMA should have been released by this point through
	 * i915_gem_close_object(), or earlier by i915_gem_context_close().
	 * However, the object may also be bound into the global GTT (e.g.
	 * older GPUs without per-process support, or for direct access through
	 * the GTT either for the user or for scanout). Those VMA still need to
	 * unbound now.
	 */
	list_for_each_entry_safe(vma, next, &obj->vma_list, obj_link) {
		GEM_BUG_ON(!i915_vma_is_ggtt(vma));
		GEM_BUG_ON(i915_vma_is_active(vma));
		vma->flags &= ~I915_VMA_PIN_MASK;
		i915_vma_close(vma);
	}
	GEM_BUG_ON(obj->bind_count);
>>>>>>> v4.9.227

	/* Stolen objects don't hold a ref, but do hold pin count. Fix that up
	 * before progressing. */
	if (obj->stolen)
		i915_gem_object_unpin_pages(obj);

<<<<<<< HEAD
	WARN_ON(obj->frontbuffer_bits);

	if (obj->pages && obj->madv == I915_MADV_WILLNEED &&
	    dev_priv->quirks & QUIRK_PIN_SWIZZLED_PAGES &&
	    obj->tiling_mode != I915_TILING_NONE)
=======
	WARN_ON(atomic_read(&obj->frontbuffer_bits));

	if (obj->pages && obj->madv == I915_MADV_WILLNEED &&
	    dev_priv->quirks & QUIRK_PIN_SWIZZLED_PAGES &&
	    i915_gem_object_is_tiled(obj))
>>>>>>> v4.9.227
		i915_gem_object_unpin_pages(obj);

	if (WARN_ON(obj->pages_pin_count))
		obj->pages_pin_count = 0;
	if (discard_backing_storage(obj))
		obj->madv = I915_MADV_DONTNEED;
	i915_gem_object_put_pages(obj);
<<<<<<< HEAD
	i915_gem_object_free_mmap_offset(obj);
=======
>>>>>>> v4.9.227

	BUG_ON(obj->pages);

	if (obj->base.import_attach)
		drm_prime_gem_destroy(&obj->base, NULL);

	if (obj->ops->release)
		obj->ops->release(obj);

	drm_gem_object_release(&obj->base);
	i915_gem_info_remove_obj(dev_priv, obj->base.size);

	kfree(obj->bit_17);
	i915_gem_object_free(obj);

	intel_runtime_pm_put(dev_priv);
}

<<<<<<< HEAD
struct i915_vma *i915_gem_obj_to_vma(struct drm_i915_gem_object *obj,
				     struct i915_address_space *vm)
{
	struct i915_vma *vma;
	list_for_each_entry(vma, &obj->vma_list, vma_link) {
		if (i915_is_ggtt(vma->vm) &&
		    vma->ggtt_view.type != I915_GGTT_VIEW_NORMAL)
			continue;
		if (vma->vm == vm)
			return vma;
	}
	return NULL;
}

struct i915_vma *i915_gem_obj_to_ggtt_view(struct drm_i915_gem_object *obj,
					   const struct i915_ggtt_view *view)
{
	struct i915_address_space *ggtt = i915_obj_to_ggtt(obj);
	struct i915_vma *vma;

	if (WARN_ONCE(!view, "no view specified"))
		return ERR_PTR(-EINVAL);

	list_for_each_entry(vma, &obj->vma_list, vma_link)
		if (vma->vm == ggtt &&
		    i915_ggtt_view_equal(&vma->ggtt_view, view))
			return vma;
	return NULL;
}

void i915_gem_vma_destroy(struct i915_vma *vma)
{
	struct i915_address_space *vm = NULL;
	WARN_ON(vma->node.allocated);

	/* Keep the vma as a placeholder in the execbuffer reservation lists */
	if (!list_empty(&vma->exec_list))
		return;

	vm = vma->vm;

	if (!i915_is_ggtt(vm))
		i915_ppgtt_put(i915_vm_to_ppgtt(vm));

	list_del(&vma->vma_link);

	kmem_cache_free(to_i915(vma->obj->base.dev)->vmas, vma);
}

static void
i915_gem_stop_ringbuffers(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring;
	int i;

	for_each_ring(ring, dev_priv, i)
		dev_priv->gt.stop_ring(ring);
}

int
i915_gem_suspend(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	int ret = 0;

	mutex_lock(&dev->struct_mutex);
	ret = i915_gpu_idle(dev);
	if (ret)
		goto err;

	i915_gem_retire_requests(dev);

	i915_gem_stop_ringbuffers(dev);
	mutex_unlock(&dev->struct_mutex);

	cancel_delayed_work_sync(&dev_priv->gpu_error.hangcheck_work);
	cancel_delayed_work_sync(&dev_priv->mm.retire_work);
	flush_delayed_work(&dev_priv->mm.idle_work);
=======
int i915_gem_suspend(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	int ret;

	intel_suspend_gt_powersave(dev_priv);

	mutex_lock(&dev->struct_mutex);

	/* We have to flush all the executing contexts to main memory so
	 * that they can saved in the hibernation image. To ensure the last
	 * context image is coherent, we have to switch away from it. That
	 * leaves the dev_priv->kernel_context still active when
	 * we actually suspend, and its image in memory may not match the GPU
	 * state. Fortunately, the kernel_context is disposable and we do
	 * not rely on its state.
	 */
	ret = i915_gem_switch_to_kernel_context(dev_priv);
	if (ret)
		goto err;

	ret = i915_gem_wait_for_idle(dev_priv,
				     I915_WAIT_INTERRUPTIBLE |
				     I915_WAIT_LOCKED);
	if (ret)
		goto err;

	i915_gem_retire_requests(dev_priv);

	i915_gem_context_lost(dev_priv);
	mutex_unlock(&dev->struct_mutex);

	cancel_delayed_work_sync(&dev_priv->gpu_error.hangcheck_work);
	cancel_delayed_work_sync(&dev_priv->gt.retire_work);
	flush_delayed_work(&dev_priv->gt.idle_work);
>>>>>>> v4.9.227

	/* Assert that we sucessfully flushed all the work and
	 * reset the GPU back to its idle, low power state.
	 */
<<<<<<< HEAD
	WARN_ON(dev_priv->mm.busy);
=======
	WARN_ON(dev_priv->gt.awake);
>>>>>>> v4.9.227

	return 0;

err:
	mutex_unlock(&dev->struct_mutex);
	return ret;
}

<<<<<<< HEAD
int i915_gem_l3_remap(struct drm_i915_gem_request *req, int slice)
{
	struct intel_engine_cs *ring = req->ring;
	struct drm_device *dev = ring->dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	u32 reg_base = GEN7_L3LOG_BASE + (slice * 0x200);
	u32 *remap_info = dev_priv->l3_parity.remap_info[slice];
	int i, ret;

	if (!HAS_L3_DPF(dev) || !remap_info)
		return 0;

	ret = intel_ring_begin(req, GEN7_L3LOG_SIZE / 4 * 3);
	if (ret)
		return ret;

	/*
	 * Note: We do not worry about the concurrent register cacheline hang
	 * here because no other code should access these registers other than
	 * at initialization time.
	 */
	for (i = 0; i < GEN7_L3LOG_SIZE; i += 4) {
		intel_ring_emit(ring, MI_LOAD_REGISTER_IMM(1));
		intel_ring_emit(ring, reg_base + i);
		intel_ring_emit(ring, remap_info[i/4]);
	}

	intel_ring_advance(ring);

	return ret;
=======
void i915_gem_resume(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = to_i915(dev);

	mutex_lock(&dev->struct_mutex);
	i915_gem_restore_gtt_mappings(dev);

	/* As we didn't flush the kernel context before suspend, we cannot
	 * guarantee that the context image is complete. So let's just reset
	 * it and start again.
	 */
	dev_priv->gt.resume(dev_priv);

	mutex_unlock(&dev->struct_mutex);
>>>>>>> v4.9.227
}

void i915_gem_init_swizzling(struct drm_device *dev)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
=======
	struct drm_i915_private *dev_priv = to_i915(dev);
>>>>>>> v4.9.227

	if (INTEL_INFO(dev)->gen < 5 ||
	    dev_priv->mm.bit_6_swizzle_x == I915_BIT_6_SWIZZLE_NONE)
		return;

	I915_WRITE(DISP_ARB_CTL, I915_READ(DISP_ARB_CTL) |
				 DISP_TILE_SURFACE_SWIZZLING);

	if (IS_GEN5(dev))
		return;

	I915_WRITE(TILECTL, I915_READ(TILECTL) | TILECTL_SWZCTL);
	if (IS_GEN6(dev))
		I915_WRITE(ARB_MODE, _MASKED_BIT_ENABLE(ARB_MODE_SWIZZLE_SNB));
	else if (IS_GEN7(dev))
		I915_WRITE(ARB_MODE, _MASKED_BIT_ENABLE(ARB_MODE_SWIZZLE_IVB));
	else if (IS_GEN8(dev))
		I915_WRITE(GAMTARBMODE, _MASKED_BIT_ENABLE(ARB_MODE_SWIZZLE_BDW));
	else
		BUG();
}

static void init_unused_ring(struct drm_device *dev, u32 base)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
=======
	struct drm_i915_private *dev_priv = to_i915(dev);
>>>>>>> v4.9.227

	I915_WRITE(RING_CTL(base), 0);
	I915_WRITE(RING_HEAD(base), 0);
	I915_WRITE(RING_TAIL(base), 0);
	I915_WRITE(RING_START(base), 0);
}

static void init_unused_rings(struct drm_device *dev)
{
	if (IS_I830(dev)) {
		init_unused_ring(dev, PRB1_BASE);
		init_unused_ring(dev, SRB0_BASE);
		init_unused_ring(dev, SRB1_BASE);
		init_unused_ring(dev, SRB2_BASE);
		init_unused_ring(dev, SRB3_BASE);
	} else if (IS_GEN2(dev)) {
		init_unused_ring(dev, SRB0_BASE);
		init_unused_ring(dev, SRB1_BASE);
	} else if (IS_GEN3(dev)) {
		init_unused_ring(dev, PRB1_BASE);
		init_unused_ring(dev, PRB2_BASE);
	}
}

<<<<<<< HEAD
int i915_gem_init_rings(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	int ret;

	ret = intel_init_render_ring_buffer(dev);
	if (ret)
		return ret;

	if (HAS_BSD(dev)) {
		ret = intel_init_bsd_ring_buffer(dev);
		if (ret)
			goto cleanup_render_ring;
	}

	if (HAS_BLT(dev)) {
		ret = intel_init_blt_ring_buffer(dev);
		if (ret)
			goto cleanup_bsd_ring;
	}

	if (HAS_VEBOX(dev)) {
		ret = intel_init_vebox_ring_buffer(dev);
		if (ret)
			goto cleanup_blt_ring;
	}

	if (HAS_BSD2(dev)) {
		ret = intel_init_bsd2_ring_buffer(dev);
		if (ret)
			goto cleanup_vebox_ring;
	}

	return 0;

cleanup_vebox_ring:
	intel_cleanup_ring_buffer(&dev_priv->ring[VECS]);
cleanup_blt_ring:
	intel_cleanup_ring_buffer(&dev_priv->ring[BCS]);
cleanup_bsd_ring:
	intel_cleanup_ring_buffer(&dev_priv->ring[VCS]);
cleanup_render_ring:
	intel_cleanup_ring_buffer(&dev_priv->ring[RCS]);

	return ret;
}

int
i915_gem_init_hw(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring;
	int ret, i, j;

	if (INTEL_INFO(dev)->gen < 6 && !intel_enable_gtt())
		return -EIO;
=======
int
i915_gem_init_hw(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct intel_engine_cs *engine;
	int ret;
>>>>>>> v4.9.227

	/* Double layer security blanket, see i915_gem_init() */
	intel_uncore_forcewake_get(dev_priv, FORCEWAKE_ALL);

<<<<<<< HEAD
	if (dev_priv->ellc_size)
=======
	if (HAS_EDRAM(dev) && INTEL_GEN(dev_priv) < 9)
>>>>>>> v4.9.227
		I915_WRITE(HSW_IDICR, I915_READ(HSW_IDICR) | IDIHASHMSK(0xf));

	if (IS_HASWELL(dev))
		I915_WRITE(MI_PREDICATE_RESULT_2, IS_HSW_GT3(dev) ?
			   LOWER_SLICE_ENABLED : LOWER_SLICE_DISABLED);

	if (HAS_PCH_NOP(dev)) {
		if (IS_IVYBRIDGE(dev)) {
			u32 temp = I915_READ(GEN7_MSG_CTL);
			temp &= ~(WAIT_FOR_PCH_FLR_ACK | WAIT_FOR_PCH_RESET_ACK);
			I915_WRITE(GEN7_MSG_CTL, temp);
		} else if (INTEL_INFO(dev)->gen >= 7) {
			u32 temp = I915_READ(HSW_NDE_RSTWRN_OPT);
			temp &= ~RESET_PCH_HANDSHAKE_ENABLE;
			I915_WRITE(HSW_NDE_RSTWRN_OPT, temp);
		}
	}

	i915_gem_init_swizzling(dev);

	/*
	 * At least 830 can leave some of the unused rings
	 * "active" (ie. head != tail) after resume which
	 * will prevent c3 entry. Makes sure all unused rings
	 * are totally idle.
	 */
	init_unused_rings(dev);

<<<<<<< HEAD
	BUG_ON(!dev_priv->ring[RCS].default_context);
=======
	BUG_ON(!dev_priv->kernel_context);
>>>>>>> v4.9.227

	ret = i915_ppgtt_init_hw(dev);
	if (ret) {
		DRM_ERROR("PPGTT enable HW failed %d\n", ret);
		goto out;
	}

	/* Need to do basic initialisation of all rings first: */
<<<<<<< HEAD
	for_each_ring(ring, dev_priv, i) {
		ret = ring->init_hw(ring);
=======
	for_each_engine(engine, dev_priv) {
		ret = engine->init_hw(engine);
>>>>>>> v4.9.227
		if (ret)
			goto out;
	}

<<<<<<< HEAD
	/* We can't enable contexts until all firmware is loaded */
	if (HAS_GUC_UCODE(dev)) {
		ret = intel_guc_ucode_load(dev);
		if (ret) {
			/*
			 * If we got an error and GuC submission is enabled, map
			 * the error to -EIO so the GPU will be declared wedged.
			 * OTOH, if we didn't intend to use the GuC anyway, just
			 * discard the error and carry on.
			 */
			DRM_ERROR("Failed to initialize GuC, error %d%s\n", ret,
				  i915.enable_guc_submission ? "" :
				  " (ignored)");
			ret = i915.enable_guc_submission ? -EIO : 0;
			if (ret)
				goto out;
		}
	}

	/*
	 * Increment the next seqno by 0x100 so we have a visible break
	 * on re-initialisation
	 */
	ret = i915_gem_set_seqno(dev, dev_priv->next_seqno+0x100);
	if (ret)
		goto out;

	/* Now it is safe to go back round and do everything else: */
	for_each_ring(ring, dev_priv, i) {
		struct drm_i915_gem_request *req;

		WARN_ON(!ring->default_context);

		ret = i915_gem_request_alloc(ring, ring->default_context, &req);
		if (ret) {
			i915_gem_cleanup_ringbuffer(dev);
			goto out;
		}

		if (ring->id == RCS) {
			for (j = 0; j < NUM_L3_SLICES(dev); j++)
				i915_gem_l3_remap(req, j);
		}

		ret = i915_ppgtt_init_ring(req);
		if (ret && ret != -EIO) {
			DRM_ERROR("PPGTT enable ring #%d failed %d\n", i, ret);
			i915_gem_request_cancel(req);
			i915_gem_cleanup_ringbuffer(dev);
			goto out;
		}

		ret = i915_gem_context_enable(req);
		if (ret && ret != -EIO) {
			DRM_ERROR("Context enable ring #%d failed %d\n", i, ret);
			i915_gem_request_cancel(req);
			i915_gem_cleanup_ringbuffer(dev);
			goto out;
		}

		i915_add_request_no_flush(req);
	}

=======
	intel_mocs_init_l3cc_table(dev);

	/* We can't enable contexts until all firmware is loaded */
	ret = intel_guc_setup(dev);
	if (ret)
		goto out;

>>>>>>> v4.9.227
out:
	intel_uncore_forcewake_put(dev_priv, FORCEWAKE_ALL);
	return ret;
}

<<<<<<< HEAD
int i915_gem_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	int ret;

	i915.enable_execlists = intel_sanitize_enable_execlists(dev,
			i915.enable_execlists);

	mutex_lock(&dev->struct_mutex);

	if (IS_VALLEYVIEW(dev)) {
		/* VLVA0 (potential hack), BIOS isn't actually waking us */
		I915_WRITE(VLV_GTLC_WAKE_CTRL, VLV_GTLC_ALLOWWAKEREQ);
		if (wait_for((I915_READ(VLV_GTLC_PW_STATUS) &
			      VLV_GTLC_ALLOWWAKEACK), 10))
			DRM_DEBUG_DRIVER("allow wake ack timed out\n");
	}

	if (!i915.enable_execlists) {
		dev_priv->gt.execbuf_submit = i915_gem_ringbuffer_submission;
		dev_priv->gt.init_rings = i915_gem_init_rings;
		dev_priv->gt.cleanup_ring = intel_cleanup_ring_buffer;
		dev_priv->gt.stop_ring = intel_stop_ring_buffer;
	} else {
		dev_priv->gt.execbuf_submit = intel_execlists_submission;
		dev_priv->gt.init_rings = intel_logical_rings_init;
		dev_priv->gt.cleanup_ring = intel_logical_ring_cleanup;
		dev_priv->gt.stop_ring = intel_logical_ring_stop;
=======
bool intel_sanitize_semaphores(struct drm_i915_private *dev_priv, int value)
{
	if (INTEL_INFO(dev_priv)->gen < 6)
		return false;

	/* TODO: make semaphores and Execlists play nicely together */
	if (i915.enable_execlists)
		return false;

	if (value >= 0)
		return value;

#ifdef CONFIG_INTEL_IOMMU
	/* Enable semaphores on SNB when IO remapping is off */
	if (INTEL_INFO(dev_priv)->gen == 6 && intel_iommu_gfx_mapped)
		return false;
#endif

	return true;
}

int i915_gem_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	int ret;

	mutex_lock(&dev->struct_mutex);

	if (!i915.enable_execlists) {
		dev_priv->gt.resume = intel_legacy_submission_resume;
		dev_priv->gt.cleanup_engine = intel_engine_cleanup;
	} else {
		dev_priv->gt.resume = intel_lr_context_resume;
		dev_priv->gt.cleanup_engine = intel_logical_ring_cleanup;
>>>>>>> v4.9.227
	}

	/* This is just a security blanket to placate dragons.
	 * On some systems, we very sporadically observe that the first TLBs
	 * used by the CS may be stale, despite us poking the TLB reset. If
	 * we hold the forcewake during initialisation these problems
	 * just magically go away.
	 */
	intel_uncore_forcewake_get(dev_priv, FORCEWAKE_ALL);

<<<<<<< HEAD
	ret = i915_gem_init_userptr(dev);
	if (ret)
		goto out_unlock;

	i915_gem_init_global_gtt(dev);

=======
	i915_gem_init_userptr(dev_priv);

	ret = i915_gem_init_ggtt(dev_priv);
	if (ret)
		goto out_unlock;

>>>>>>> v4.9.227
	ret = i915_gem_context_init(dev);
	if (ret)
		goto out_unlock;

<<<<<<< HEAD
	ret = dev_priv->gt.init_rings(dev);
=======
	ret = intel_engines_init(dev);
>>>>>>> v4.9.227
	if (ret)
		goto out_unlock;

	ret = i915_gem_init_hw(dev);
	if (ret == -EIO) {
<<<<<<< HEAD
		/* Allow ring initialisation to fail by marking the GPU as
=======
		/* Allow engine initialisation to fail by marking the GPU as
>>>>>>> v4.9.227
		 * wedged. But we only want to do this where the GPU is angry,
		 * for all other failure, such as an allocation failure, bail.
		 */
		DRM_ERROR("Failed to initialize GPU, declaring it wedged\n");
<<<<<<< HEAD
		atomic_or(I915_WEDGED, &dev_priv->gpu_error.reset_counter);
=======
		i915_gem_set_wedged(dev_priv);
>>>>>>> v4.9.227
		ret = 0;
	}

out_unlock:
	intel_uncore_forcewake_put(dev_priv, FORCEWAKE_ALL);
	mutex_unlock(&dev->struct_mutex);

	return ret;
}

void
<<<<<<< HEAD
i915_gem_cleanup_ringbuffer(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct intel_engine_cs *ring;
	int i;

	for_each_ring(ring, dev_priv, i)
		dev_priv->gt.cleanup_ring(ring);

    if (i915.enable_execlists)
            /*
             * Neither the BIOS, ourselves or any other kernel
             * expects the system to be in execlists mode on startup,
             * so we need to reset the GPU back to legacy mode.
             */
            intel_gpu_reset(dev);
}

static void
init_ring_lists(struct intel_engine_cs *ring)
{
	INIT_LIST_HEAD(&ring->active_list);
	INIT_LIST_HEAD(&ring->request_list);
}

void
i915_gem_load(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
=======
i915_gem_cleanup_engines(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct intel_engine_cs *engine;

	for_each_engine(engine, dev_priv)
		dev_priv->gt.cleanup_engine(engine);
}

static void
init_engine_lists(struct intel_engine_cs *engine)
{
	INIT_LIST_HEAD(&engine->request_list);
}

void
i915_gem_load_init_fences(struct drm_i915_private *dev_priv)
{
	struct drm_device *dev = &dev_priv->drm;
	int i;

	if (INTEL_INFO(dev_priv)->gen >= 7 && !IS_VALLEYVIEW(dev_priv) &&
	    !IS_CHERRYVIEW(dev_priv))
		dev_priv->num_fence_regs = 32;
	else if (INTEL_INFO(dev_priv)->gen >= 4 || IS_I945G(dev_priv) ||
		 IS_I945GM(dev_priv) || IS_G33(dev_priv))
		dev_priv->num_fence_regs = 16;
	else
		dev_priv->num_fence_regs = 8;

	if (intel_vgpu_active(dev_priv))
		dev_priv->num_fence_regs =
				I915_READ(vgtif_reg(avail_rs.fence_num));

	/* Initialize fence registers to zero */
	for (i = 0; i < dev_priv->num_fence_regs; i++) {
		struct drm_i915_fence_reg *fence = &dev_priv->fence_regs[i];

		fence->i915 = dev_priv;
		fence->id = i;
		list_add_tail(&fence->link, &dev_priv->mm.fence_list);
	}
	i915_gem_restore_fences(dev);

	i915_gem_detect_bit_6_swizzle(dev);
}

void
i915_gem_load_init(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = to_i915(dev);
>>>>>>> v4.9.227
	int i;

	dev_priv->objects =
		kmem_cache_create("i915_gem_object",
				  sizeof(struct drm_i915_gem_object), 0,
				  SLAB_HWCACHE_ALIGN,
				  NULL);
	dev_priv->vmas =
		kmem_cache_create("i915_gem_vma",
				  sizeof(struct i915_vma), 0,
				  SLAB_HWCACHE_ALIGN,
				  NULL);
	dev_priv->requests =
		kmem_cache_create("i915_gem_request",
				  sizeof(struct drm_i915_gem_request), 0,
<<<<<<< HEAD
				  SLAB_HWCACHE_ALIGN,
				  NULL);

	INIT_LIST_HEAD(&dev_priv->vm_list);
=======
				  SLAB_HWCACHE_ALIGN |
				  SLAB_RECLAIM_ACCOUNT |
				  SLAB_DESTROY_BY_RCU,
				  NULL);

>>>>>>> v4.9.227
	INIT_LIST_HEAD(&dev_priv->context_list);
	INIT_LIST_HEAD(&dev_priv->mm.unbound_list);
	INIT_LIST_HEAD(&dev_priv->mm.bound_list);
	INIT_LIST_HEAD(&dev_priv->mm.fence_list);
<<<<<<< HEAD
	for (i = 0; i < I915_NUM_RINGS; i++)
		init_ring_lists(&dev_priv->ring[i]);
	for (i = 0; i < I915_MAX_NUM_FENCES; i++)
		INIT_LIST_HEAD(&dev_priv->fence_regs[i].lru_list);
	INIT_DELAYED_WORK(&dev_priv->mm.retire_work,
			  i915_gem_retire_work_handler);
	INIT_DELAYED_WORK(&dev_priv->mm.idle_work,
			  i915_gem_idle_work_handler);
	init_waitqueue_head(&dev_priv->gpu_error.reset_queue);

	dev_priv->relative_constants_mode = I915_EXEC_CONSTANTS_REL_GENERAL;

	if (INTEL_INFO(dev)->gen >= 7 && !IS_VALLEYVIEW(dev))
		dev_priv->num_fence_regs = 32;
	else if (INTEL_INFO(dev)->gen >= 4 || IS_I945G(dev) || IS_I945GM(dev) || IS_G33(dev))
		dev_priv->num_fence_regs = 16;
	else
		dev_priv->num_fence_regs = 8;

	if (intel_vgpu_active(dev))
		dev_priv->num_fence_regs =
				I915_READ(vgtif_reg(avail_rs.fence_num));

	/*
	 * Set initial sequence number for requests.
	 * Using this number allows the wraparound to happen early,
	 * catching any obvious problems.
	 */
	dev_priv->next_seqno = ((u32)~0 - 0x1100);
	dev_priv->last_seqno = ((u32)~0 - 0x1101);

	/* Initialize fence registers to zero */
	INIT_LIST_HEAD(&dev_priv->mm.fence_list);
	i915_gem_restore_fences(dev);

	i915_gem_detect_bit_6_swizzle(dev);
=======
	for (i = 0; i < I915_NUM_ENGINES; i++)
		init_engine_lists(&dev_priv->engine[i]);
	INIT_DELAYED_WORK(&dev_priv->gt.retire_work,
			  i915_gem_retire_work_handler);
	INIT_DELAYED_WORK(&dev_priv->gt.idle_work,
			  i915_gem_idle_work_handler);
	init_waitqueue_head(&dev_priv->gpu_error.wait_queue);
	init_waitqueue_head(&dev_priv->gpu_error.reset_queue);

>>>>>>> v4.9.227
	init_waitqueue_head(&dev_priv->pending_flip_queue);

	dev_priv->mm.interruptible = true;

<<<<<<< HEAD
	i915_gem_shrinker_init(dev_priv);

	mutex_init(&dev_priv->fb_tracking.lock);
=======
	atomic_set(&dev_priv->mm.bsd_engine_dispatch_index, 0);

	spin_lock_init(&dev_priv->fb_tracking.lock);
}

void i915_gem_load_cleanup(struct drm_device *dev)
{
	struct drm_i915_private *dev_priv = to_i915(dev);

	kmem_cache_destroy(dev_priv->requests);
	kmem_cache_destroy(dev_priv->vmas);
	kmem_cache_destroy(dev_priv->objects);

	/* And ensure that our DESTROY_BY_RCU slabs are truly destroyed */
	rcu_barrier();
}

int i915_gem_freeze(struct drm_i915_private *dev_priv)
{
	intel_runtime_pm_get(dev_priv);

	mutex_lock(&dev_priv->drm.struct_mutex);
	i915_gem_shrink_all(dev_priv);
	mutex_unlock(&dev_priv->drm.struct_mutex);

	intel_runtime_pm_put(dev_priv);

	return 0;
}

int i915_gem_freeze_late(struct drm_i915_private *dev_priv)
{
	struct drm_i915_gem_object *obj;
	struct list_head *phases[] = {
		&dev_priv->mm.unbound_list,
		&dev_priv->mm.bound_list,
		NULL
	}, **p;

	/* Called just before we write the hibernation image.
	 *
	 * We need to update the domain tracking to reflect that the CPU
	 * will be accessing all the pages to create and restore from the
	 * hibernation, and so upon restoration those pages will be in the
	 * CPU domain.
	 *
	 * To make sure the hibernation image contains the latest state,
	 * we update that state just before writing out the image.
	 *
	 * To try and reduce the hibernation image, we manually shrink
	 * the objects as well.
	 */

	mutex_lock(&dev_priv->drm.struct_mutex);
	i915_gem_shrink(dev_priv, -1UL, I915_SHRINK_UNBOUND);

	for (p = phases; *p; p++) {
		list_for_each_entry(obj, *p, global_list) {
			obj->base.read_domains = I915_GEM_DOMAIN_CPU;
			obj->base.write_domain = I915_GEM_DOMAIN_CPU;
		}
	}
	mutex_unlock(&dev_priv->drm.struct_mutex);

	return 0;
>>>>>>> v4.9.227
}

void i915_gem_release(struct drm_device *dev, struct drm_file *file)
{
	struct drm_i915_file_private *file_priv = file->driver_priv;
<<<<<<< HEAD
=======
	struct drm_i915_gem_request *request;
>>>>>>> v4.9.227

	/* Clean up our request list when the client is going away, so that
	 * later retire_requests won't dereference our soon-to-be-gone
	 * file_priv.
	 */
	spin_lock(&file_priv->mm.lock);
<<<<<<< HEAD
	while (!list_empty(&file_priv->mm.request_list)) {
		struct drm_i915_gem_request *request;

		request = list_first_entry(&file_priv->mm.request_list,
					   struct drm_i915_gem_request,
					   client_list);
		list_del(&request->client_list);
		request->file_priv = NULL;
	}
=======
	list_for_each_entry(request, &file_priv->mm.request_list, client_list)
		request->file_priv = NULL;
>>>>>>> v4.9.227
	spin_unlock(&file_priv->mm.lock);

	if (!list_empty(&file_priv->rps.link)) {
		spin_lock(&to_i915(dev)->rps.client_lock);
		list_del(&file_priv->rps.link);
		spin_unlock(&to_i915(dev)->rps.client_lock);
	}
}

int i915_gem_open(struct drm_device *dev, struct drm_file *file)
{
	struct drm_i915_file_private *file_priv;
	int ret;

	DRM_DEBUG_DRIVER("\n");

	file_priv = kzalloc(sizeof(*file_priv), GFP_KERNEL);
	if (!file_priv)
		return -ENOMEM;

	file->driver_priv = file_priv;
<<<<<<< HEAD
	file_priv->dev_priv = dev->dev_private;
=======
	file_priv->dev_priv = to_i915(dev);
>>>>>>> v4.9.227
	file_priv->file = file;
	INIT_LIST_HEAD(&file_priv->rps.link);

	spin_lock_init(&file_priv->mm.lock);
	INIT_LIST_HEAD(&file_priv->mm.request_list);

<<<<<<< HEAD
=======
	file_priv->bsd_engine = -1;

>>>>>>> v4.9.227
	ret = i915_gem_context_open(dev, file);
	if (ret)
		kfree(file_priv);

	return ret;
}

/**
 * i915_gem_track_fb - update frontbuffer tracking
 * @old: current GEM buffer for the frontbuffer slots
 * @new: new GEM buffer for the frontbuffer slots
 * @frontbuffer_bits: bitmask of frontbuffer slots
 *
 * This updates the frontbuffer tracking bits @frontbuffer_bits by clearing them
 * from @old and setting them in @new. Both @old and @new can be NULL.
 */
void i915_gem_track_fb(struct drm_i915_gem_object *old,
		       struct drm_i915_gem_object *new,
		       unsigned frontbuffer_bits)
{
<<<<<<< HEAD
	if (old) {
		WARN_ON(!mutex_is_locked(&old->base.dev->struct_mutex));
		WARN_ON(!(old->frontbuffer_bits & frontbuffer_bits));
		old->frontbuffer_bits &= ~frontbuffer_bits;
	}

	if (new) {
		WARN_ON(!mutex_is_locked(&new->base.dev->struct_mutex));
		WARN_ON(new->frontbuffer_bits & frontbuffer_bits);
		new->frontbuffer_bits |= frontbuffer_bits;
	}
}

/* All the new VM stuff */
u64 i915_gem_obj_offset(struct drm_i915_gem_object *o,
			struct i915_address_space *vm)
{
	struct drm_i915_private *dev_priv = o->base.dev->dev_private;
	struct i915_vma *vma;

	WARN_ON(vm == &dev_priv->mm.aliasing_ppgtt->base);

	list_for_each_entry(vma, &o->vma_list, vma_link) {
		if (i915_is_ggtt(vma->vm) &&
		    vma->ggtt_view.type != I915_GGTT_VIEW_NORMAL)
			continue;
		if (vma->vm == vm)
			return vma->node.start;
	}

	WARN(1, "%s vma for this object not found.\n",
	     i915_is_ggtt(vm) ? "global" : "ppgtt");
	return -1;
}

u64 i915_gem_obj_ggtt_offset_view(struct drm_i915_gem_object *o,
				  const struct i915_ggtt_view *view)
{
	struct i915_address_space *ggtt = i915_obj_to_ggtt(o);
	struct i915_vma *vma;

	list_for_each_entry(vma, &o->vma_list, vma_link)
		if (vma->vm == ggtt &&
		    i915_ggtt_view_equal(&vma->ggtt_view, view))
			return vma->node.start;

	WARN(1, "global vma for this object not found. (view=%u)\n", view->type);
	return -1;
}

bool i915_gem_obj_bound(struct drm_i915_gem_object *o,
			struct i915_address_space *vm)
{
	struct i915_vma *vma;

	list_for_each_entry(vma, &o->vma_list, vma_link) {
		if (i915_is_ggtt(vma->vm) &&
		    vma->ggtt_view.type != I915_GGTT_VIEW_NORMAL)
			continue;
		if (vma->vm == vm && drm_mm_node_allocated(&vma->node))
			return true;
	}

	return false;
}

bool i915_gem_obj_ggtt_bound_view(struct drm_i915_gem_object *o,
				  const struct i915_ggtt_view *view)
{
	struct i915_address_space *ggtt = i915_obj_to_ggtt(o);
	struct i915_vma *vma;

	list_for_each_entry(vma, &o->vma_list, vma_link)
		if (vma->vm == ggtt &&
		    i915_ggtt_view_equal(&vma->ggtt_view, view) &&
		    drm_mm_node_allocated(&vma->node))
			return true;

	return false;
}

bool i915_gem_obj_bound_any(struct drm_i915_gem_object *o)
{
	struct i915_vma *vma;

	list_for_each_entry(vma, &o->vma_list, vma_link)
		if (drm_mm_node_allocated(&vma->node))
			return true;

	return false;
}

unsigned long i915_gem_obj_size(struct drm_i915_gem_object *o,
				struct i915_address_space *vm)
{
	struct drm_i915_private *dev_priv = o->base.dev->dev_private;
	struct i915_vma *vma;

	WARN_ON(vm == &dev_priv->mm.aliasing_ppgtt->base);

	BUG_ON(list_empty(&o->vma_list));

	list_for_each_entry(vma, &o->vma_list, vma_link) {
		if (i915_is_ggtt(vma->vm) &&
		    vma->ggtt_view.type != I915_GGTT_VIEW_NORMAL)
			continue;
		if (vma->vm == vm)
			return vma->node.size;
	}
	return 0;
}

bool i915_gem_obj_is_pinned(struct drm_i915_gem_object *obj)
{
	struct i915_vma *vma;
	list_for_each_entry(vma, &obj->vma_list, vma_link)
		if (vma->pin_count > 0)
			return true;

	return false;
=======
	/* Control of individual bits within the mask are guarded by
	 * the owning plane->mutex, i.e. we can never see concurrent
	 * manipulation of individual bits. But since the bitfield as a whole
	 * is updated using RMW, we need to use atomics in order to update
	 * the bits.
	 */
	BUILD_BUG_ON(INTEL_FRONTBUFFER_BITS_PER_PIPE * I915_MAX_PIPES >
		     sizeof(atomic_t) * BITS_PER_BYTE);

	if (old) {
		WARN_ON(!(atomic_read(&old->frontbuffer_bits) & frontbuffer_bits));
		atomic_andnot(frontbuffer_bits, &old->frontbuffer_bits);
	}

	if (new) {
		WARN_ON(atomic_read(&new->frontbuffer_bits) & frontbuffer_bits);
		atomic_or(frontbuffer_bits, &new->frontbuffer_bits);
	}
}

/* Like i915_gem_object_get_page(), but mark the returned page dirty */
struct page *
i915_gem_object_get_dirty_page(struct drm_i915_gem_object *obj, int n)
{
	struct page *page;

	/* Only default objects have per-page dirty tracking */
	if (WARN_ON(!i915_gem_object_has_struct_page(obj)))
		return NULL;

	page = i915_gem_object_get_page(obj, n);
	set_page_dirty(page);
	return page;
>>>>>>> v4.9.227
}

/* Allocate a new GEM object and fill it with the supplied data */
struct drm_i915_gem_object *
i915_gem_object_create_from_data(struct drm_device *dev,
			         const void *data, size_t size)
{
	struct drm_i915_gem_object *obj;
	struct sg_table *sg;
	size_t bytes;
	int ret;

<<<<<<< HEAD
	obj = i915_gem_alloc_object(dev, round_up(size, PAGE_SIZE));
	if (IS_ERR_OR_NULL(obj))
=======
	obj = i915_gem_object_create(dev, round_up(size, PAGE_SIZE));
	if (IS_ERR(obj))
>>>>>>> v4.9.227
		return obj;

	ret = i915_gem_object_set_to_cpu_domain(obj, true);
	if (ret)
		goto fail;

	ret = i915_gem_object_get_pages(obj);
	if (ret)
		goto fail;

	i915_gem_object_pin_pages(obj);
	sg = obj->pages;
	bytes = sg_copy_from_buffer(sg->sgl, sg->nents, (void *)data, size);
<<<<<<< HEAD
=======
	obj->dirty = 1;		/* Backing store is now out of date */
>>>>>>> v4.9.227
	i915_gem_object_unpin_pages(obj);

	if (WARN_ON(bytes != size)) {
		DRM_ERROR("Incomplete copy, wrote %zu of %zu", bytes, size);
		ret = -EFAULT;
		goto fail;
	}

	return obj;

fail:
<<<<<<< HEAD
	drm_gem_object_unreference(&obj->base);
=======
	i915_gem_object_put(obj);
>>>>>>> v4.9.227
	return ERR_PTR(ret);
}
