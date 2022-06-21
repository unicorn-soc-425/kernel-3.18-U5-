/*
 * Copyright © 2008,2010 Intel Corporation
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
 *    Chris Wilson <chris@chris-wilson.co.uk>
 *
 */

<<<<<<< HEAD
#include <drm/drmP.h>
#include <drm/i915_drm.h>
#include "i915_drv.h"
#include "i915_trace.h"
#include "intel_drv.h"
#include <linux/dma_remapping.h>
#include <linux/uaccess.h>

#define  __EXEC_OBJECT_HAS_PIN (1<<31)
#define  __EXEC_OBJECT_HAS_FENCE (1<<30)
#define  __EXEC_OBJECT_NEEDS_MAP (1<<29)
#define  __EXEC_OBJECT_NEEDS_BIAS (1<<28)

#define BATCH_OFFSET_BIAS (256*1024)

struct eb_vmas {
=======
#include <linux/dma_remapping.h>
#include <linux/reservation.h>
#include <linux/uaccess.h>

#include <drm/drmP.h>
#include <drm/i915_drm.h>

#include "i915_drv.h"
#include "i915_gem_dmabuf.h"
#include "i915_trace.h"
#include "intel_drv.h"
#include "intel_frontbuffer.h"

#define DBG_USE_CPU_RELOC 0 /* -1 force GTT relocs; 1 force CPU relocs */

#define  __EXEC_OBJECT_HAS_PIN		(1<<31)
#define  __EXEC_OBJECT_HAS_FENCE	(1<<30)
#define  __EXEC_OBJECT_NEEDS_MAP	(1<<29)
#define  __EXEC_OBJECT_NEEDS_BIAS	(1<<28)
#define  __EXEC_OBJECT_INTERNAL_FLAGS (0xf<<28) /* all of the above */

#define BATCH_OFFSET_BIAS (256*1024)

struct i915_execbuffer_params {
	struct drm_device               *dev;
	struct drm_file                 *file;
	struct i915_vma			*batch;
	u32				dispatch_flags;
	u32				args_batch_start_offset;
	u64				args_batch_len;
	struct intel_engine_cs          *engine;
	struct i915_gem_context         *ctx;
	struct drm_i915_gem_request     *request;
};

struct eb_vmas {
	struct drm_i915_private *i915;
>>>>>>> v4.9.227
	struct list_head vmas;
	int and;
	union {
		struct i915_vma *lut[0];
		struct hlist_head buckets[0];
	};
};

static struct eb_vmas *
<<<<<<< HEAD
eb_create(struct drm_i915_gem_execbuffer2 *args)
=======
eb_create(struct drm_i915_private *i915,
	  struct drm_i915_gem_execbuffer2 *args)
>>>>>>> v4.9.227
{
	struct eb_vmas *eb = NULL;

	if (args->flags & I915_EXEC_HANDLE_LUT) {
		unsigned size = args->buffer_count;
		size *= sizeof(struct i915_vma *);
		size += sizeof(struct eb_vmas);
		eb = kmalloc(size, GFP_TEMPORARY | __GFP_NOWARN | __GFP_NORETRY);
	}

	if (eb == NULL) {
		unsigned size = args->buffer_count;
		unsigned count = PAGE_SIZE / sizeof(struct hlist_head) / 2;
		BUILD_BUG_ON_NOT_POWER_OF_2(PAGE_SIZE / sizeof(struct hlist_head));
		while (count > 2*size)
			count >>= 1;
		eb = kzalloc(count*sizeof(struct hlist_head) +
			     sizeof(struct eb_vmas),
			     GFP_TEMPORARY);
		if (eb == NULL)
			return eb;

		eb->and = count - 1;
	} else
		eb->and = -args->buffer_count;

<<<<<<< HEAD
=======
	eb->i915 = i915;
>>>>>>> v4.9.227
	INIT_LIST_HEAD(&eb->vmas);
	return eb;
}

static void
eb_reset(struct eb_vmas *eb)
{
	if (eb->and >= 0)
		memset(eb->buckets, 0, (eb->and+1)*sizeof(struct hlist_head));
}

<<<<<<< HEAD
=======
static struct i915_vma *
eb_get_batch(struct eb_vmas *eb)
{
	struct i915_vma *vma = list_entry(eb->vmas.prev, typeof(*vma), exec_list);

	/*
	 * SNA is doing fancy tricks with compressing batch buffers, which leads
	 * to negative relocation deltas. Usually that works out ok since the
	 * relocate address is still positive, except when the batch is placed
	 * very low in the GTT. Ensure this doesn't happen.
	 *
	 * Note that actual hangs have only been observed on gen7, but for
	 * paranoia do it everywhere.
	 */
	if ((vma->exec_entry->flags & EXEC_OBJECT_PINNED) == 0)
		vma->exec_entry->flags |= __EXEC_OBJECT_NEEDS_BIAS;

	return vma;
}

>>>>>>> v4.9.227
static int
eb_lookup_vmas(struct eb_vmas *eb,
	       struct drm_i915_gem_exec_object2 *exec,
	       const struct drm_i915_gem_execbuffer2 *args,
	       struct i915_address_space *vm,
	       struct drm_file *file)
{
	struct drm_i915_gem_object *obj;
	struct list_head objects;
	int i, ret;

	INIT_LIST_HEAD(&objects);
	spin_lock(&file->table_lock);
	/* Grab a reference to the object and release the lock so we can lookup
	 * or create the VMA without using GFP_ATOMIC */
	for (i = 0; i < args->buffer_count; i++) {
		obj = to_intel_bo(idr_find(&file->object_idr, exec[i].handle));
		if (obj == NULL) {
			spin_unlock(&file->table_lock);
			DRM_DEBUG("Invalid object handle %d at index %d\n",
				   exec[i].handle, i);
			ret = -ENOENT;
			goto err;
		}

		if (!list_empty(&obj->obj_exec_link)) {
			spin_unlock(&file->table_lock);
			DRM_DEBUG("Object %p [handle %d, index %d] appears more than once in object list\n",
				   obj, exec[i].handle, i);
			ret = -EINVAL;
			goto err;
		}

<<<<<<< HEAD
		drm_gem_object_reference(&obj->base);
=======
		i915_gem_object_get(obj);
>>>>>>> v4.9.227
		list_add_tail(&obj->obj_exec_link, &objects);
	}
	spin_unlock(&file->table_lock);

	i = 0;
	while (!list_empty(&objects)) {
		struct i915_vma *vma;

		obj = list_first_entry(&objects,
				       struct drm_i915_gem_object,
				       obj_exec_link);

		/*
		 * NOTE: We can leak any vmas created here when something fails
		 * later on. But that's no issue since vma_unbind can deal with
		 * vmas which are not actually bound. And since only
		 * lookup_or_create exists as an interface to get at the vma
		 * from the (obj, vm) we don't run the risk of creating
		 * duplicated vmas for the same vm.
		 */
<<<<<<< HEAD
		vma = i915_gem_obj_lookup_or_create_vma(obj, vm);
		if (IS_ERR(vma)) {
=======
		vma = i915_gem_obj_lookup_or_create_vma(obj, vm, NULL);
		if (unlikely(IS_ERR(vma))) {
>>>>>>> v4.9.227
			DRM_DEBUG("Failed to lookup VMA\n");
			ret = PTR_ERR(vma);
			goto err;
		}

		/* Transfer ownership from the objects list to the vmas list. */
		list_add_tail(&vma->exec_list, &eb->vmas);
		list_del_init(&obj->obj_exec_link);

		vma->exec_entry = &exec[i];
		if (eb->and < 0) {
			eb->lut[i] = vma;
		} else {
			uint32_t handle = args->flags & I915_EXEC_HANDLE_LUT ? i : exec[i].handle;
			vma->exec_handle = handle;
			hlist_add_head(&vma->exec_node,
				       &eb->buckets[handle & eb->and]);
		}
		++i;
	}

	return 0;


err:
	while (!list_empty(&objects)) {
		obj = list_first_entry(&objects,
				       struct drm_i915_gem_object,
				       obj_exec_link);
		list_del_init(&obj->obj_exec_link);
<<<<<<< HEAD
		drm_gem_object_unreference(&obj->base);
=======
		i915_gem_object_put(obj);
>>>>>>> v4.9.227
	}
	/*
	 * Objects already transfered to the vmas list will be unreferenced by
	 * eb_destroy.
	 */

	return ret;
}

static struct i915_vma *eb_get_vma(struct eb_vmas *eb, unsigned long handle)
{
	if (eb->and < 0) {
		if (handle >= -eb->and)
			return NULL;
		return eb->lut[handle];
	} else {
		struct hlist_head *head;
<<<<<<< HEAD
		struct hlist_node *node;

		head = &eb->buckets[handle & eb->and];
		hlist_for_each(node, head) {
			struct i915_vma *vma;

			vma = hlist_entry(node, struct i915_vma, exec_node);
=======
		struct i915_vma *vma;

		head = &eb->buckets[handle & eb->and];
		hlist_for_each_entry(vma, head, exec_node) {
>>>>>>> v4.9.227
			if (vma->exec_handle == handle)
				return vma;
		}
		return NULL;
	}
}

static void
i915_gem_execbuffer_unreserve_vma(struct i915_vma *vma)
{
	struct drm_i915_gem_exec_object2 *entry;
<<<<<<< HEAD
	struct drm_i915_gem_object *obj = vma->obj;
=======
>>>>>>> v4.9.227

	if (!drm_mm_node_allocated(&vma->node))
		return;

	entry = vma->exec_entry;

	if (entry->flags & __EXEC_OBJECT_HAS_FENCE)
<<<<<<< HEAD
		i915_gem_object_unpin_fence(obj);

	if (entry->flags & __EXEC_OBJECT_HAS_PIN)
		vma->pin_count--;
=======
		i915_vma_unpin_fence(vma);

	if (entry->flags & __EXEC_OBJECT_HAS_PIN)
		__i915_vma_unpin(vma);
>>>>>>> v4.9.227

	entry->flags &= ~(__EXEC_OBJECT_HAS_FENCE | __EXEC_OBJECT_HAS_PIN);
}

static void eb_destroy(struct eb_vmas *eb)
{
	while (!list_empty(&eb->vmas)) {
		struct i915_vma *vma;

		vma = list_first_entry(&eb->vmas,
				       struct i915_vma,
				       exec_list);
		list_del_init(&vma->exec_list);
		i915_gem_execbuffer_unreserve_vma(vma);
<<<<<<< HEAD
		drm_gem_object_unreference(&vma->obj->base);
=======
		i915_vma_put(vma);
>>>>>>> v4.9.227
	}
	kfree(eb);
}

static inline int use_cpu_reloc(struct drm_i915_gem_object *obj)
{
<<<<<<< HEAD
=======
	if (!i915_gem_object_has_struct_page(obj))
		return false;

	if (DBG_USE_CPU_RELOC)
		return DBG_USE_CPU_RELOC > 0;

>>>>>>> v4.9.227
	return (HAS_LLC(obj->base.dev) ||
		obj->base.write_domain == I915_GEM_DOMAIN_CPU ||
		obj->cache_level != I915_CACHE_NONE);
}

<<<<<<< HEAD
static int
relocate_entry_cpu(struct drm_i915_gem_object *obj,
		   struct drm_i915_gem_relocation_entry *reloc,
		   uint64_t target_offset)
{
	struct drm_device *dev = obj->base.dev;
	uint32_t page_offset = offset_in_page(reloc->offset);
	uint64_t delta = reloc->delta + target_offset;
	char *vaddr;
	int ret;

	ret = i915_gem_object_set_to_cpu_domain(obj, true);
	if (ret)
		return ret;

	vaddr = kmap_atomic(i915_gem_object_get_page(obj,
				reloc->offset >> PAGE_SHIFT));
	*(uint32_t *)(vaddr + page_offset) = lower_32_bits(delta);

	if (INTEL_INFO(dev)->gen >= 8) {
		page_offset = offset_in_page(page_offset + sizeof(uint32_t));

		if (page_offset == 0) {
			kunmap_atomic(vaddr);
			vaddr = kmap_atomic(i915_gem_object_get_page(obj,
			    (reloc->offset + sizeof(uint32_t)) >> PAGE_SHIFT));
		}

		*(uint32_t *)(vaddr + page_offset) = upper_32_bits(delta);
	}

	kunmap_atomic(vaddr);
=======
/* Used to convert any address to canonical form.
 * Starting from gen8, some commands (e.g. STATE_BASE_ADDRESS,
 * MI_LOAD_REGISTER_MEM and others, see Broadwell PRM Vol2a) require the
 * addresses to be in a canonical form:
 * "GraphicsAddress[63:48] are ignored by the HW and assumed to be in correct
 * canonical form [63:48] == [47]."
 */
#define GEN8_HIGH_ADDRESS_BIT 47
static inline uint64_t gen8_canonical_addr(uint64_t address)
{
	return sign_extend64(address, GEN8_HIGH_ADDRESS_BIT);
}

static inline uint64_t gen8_noncanonical_addr(uint64_t address)
{
	return address & ((1ULL << (GEN8_HIGH_ADDRESS_BIT + 1)) - 1);
}

static inline uint64_t
relocation_target(const struct drm_i915_gem_relocation_entry *reloc,
		  uint64_t target_offset)
{
	return gen8_canonical_addr((int)reloc->delta + target_offset);
}

struct reloc_cache {
	struct drm_i915_private *i915;
	struct drm_mm_node node;
	unsigned long vaddr;
	unsigned int page;
	bool use_64bit_reloc;
};

static void reloc_cache_init(struct reloc_cache *cache,
			     struct drm_i915_private *i915)
{
	cache->page = -1;
	cache->vaddr = 0;
	cache->i915 = i915;
	cache->use_64bit_reloc = INTEL_GEN(cache->i915) >= 8;
	cache->node.allocated = false;
}

static inline void *unmask_page(unsigned long p)
{
	return (void *)(uintptr_t)(p & PAGE_MASK);
}

static inline unsigned int unmask_flags(unsigned long p)
{
	return p & ~PAGE_MASK;
}

#define KMAP 0x4 /* after CLFLUSH_FLAGS */

static void reloc_cache_fini(struct reloc_cache *cache)
{
	void *vaddr;

	if (!cache->vaddr)
		return;

	vaddr = unmask_page(cache->vaddr);
	if (cache->vaddr & KMAP) {
		if (cache->vaddr & CLFLUSH_AFTER)
			mb();

		kunmap_atomic(vaddr);
		i915_gem_obj_finish_shmem_access((struct drm_i915_gem_object *)cache->node.mm);
	} else {
		wmb();
		io_mapping_unmap_atomic((void __iomem *)vaddr);
		if (cache->node.allocated) {
			struct i915_ggtt *ggtt = &cache->i915->ggtt;

			ggtt->base.clear_range(&ggtt->base,
					       cache->node.start,
					       cache->node.size,
					       true);
			drm_mm_remove_node(&cache->node);
		} else {
			i915_vma_unpin((struct i915_vma *)cache->node.mm);
		}
	}
}

static void *reloc_kmap(struct drm_i915_gem_object *obj,
			struct reloc_cache *cache,
			int page)
{
	void *vaddr;

	if (cache->vaddr) {
		kunmap_atomic(unmask_page(cache->vaddr));
	} else {
		unsigned int flushes;
		int ret;

		ret = i915_gem_obj_prepare_shmem_write(obj, &flushes);
		if (ret)
			return ERR_PTR(ret);

		BUILD_BUG_ON(KMAP & CLFLUSH_FLAGS);
		BUILD_BUG_ON((KMAP | CLFLUSH_FLAGS) & PAGE_MASK);

		cache->vaddr = flushes | KMAP;
		cache->node.mm = (void *)obj;
		if (flushes)
			mb();
	}

	vaddr = kmap_atomic(i915_gem_object_get_dirty_page(obj, page));
	cache->vaddr = unmask_flags(cache->vaddr) | (unsigned long)vaddr;
	cache->page = page;

	return vaddr;
}

static void *reloc_iomap(struct drm_i915_gem_object *obj,
			 struct reloc_cache *cache,
			 int page)
{
	struct i915_ggtt *ggtt = &cache->i915->ggtt;
	unsigned long offset;
	void *vaddr;

	if (cache->node.allocated) {
		wmb();
		ggtt->base.insert_page(&ggtt->base,
				       i915_gem_object_get_dma_address(obj, page),
				       cache->node.start, I915_CACHE_NONE, 0);
		cache->page = page;
		return unmask_page(cache->vaddr);
	}

	if (cache->vaddr) {
		io_mapping_unmap_atomic(unmask_page(cache->vaddr));
	} else {
		struct i915_vma *vma;
		int ret;

		if (use_cpu_reloc(obj))
			return NULL;

		ret = i915_gem_object_set_to_gtt_domain(obj, true);
		if (ret)
			return ERR_PTR(ret);

		vma = i915_gem_object_ggtt_pin(obj, NULL, 0, 0,
					       PIN_MAPPABLE | PIN_NONBLOCK);
		if (IS_ERR(vma)) {
			memset(&cache->node, 0, sizeof(cache->node));
			ret = drm_mm_insert_node_in_range_generic
				(&ggtt->base.mm, &cache->node,
				 4096, 0, 0,
				 0, ggtt->mappable_end,
				 DRM_MM_SEARCH_DEFAULT,
				 DRM_MM_CREATE_DEFAULT);
			if (ret) /* no inactive aperture space, use cpu reloc */
				return NULL;
		} else {
			ret = i915_vma_put_fence(vma);
			if (ret) {
				i915_vma_unpin(vma);
				return ERR_PTR(ret);
			}

			cache->node.start = vma->node.start;
			cache->node.mm = (void *)vma;
		}
	}

	offset = cache->node.start;
	if (cache->node.allocated) {
		ggtt->base.insert_page(&ggtt->base,
				       i915_gem_object_get_dma_address(obj, page),
				       offset, I915_CACHE_NONE, 0);
	} else {
		offset += page << PAGE_SHIFT;
	}

	vaddr = io_mapping_map_atomic_wc(&cache->i915->ggtt.mappable, offset);
	cache->page = page;
	cache->vaddr = (unsigned long)vaddr;

	return vaddr;
}

static void *reloc_vaddr(struct drm_i915_gem_object *obj,
			 struct reloc_cache *cache,
			 int page)
{
	void *vaddr;

	if (cache->page == page) {
		vaddr = unmask_page(cache->vaddr);
	} else {
		vaddr = NULL;
		if ((cache->vaddr & KMAP) == 0)
			vaddr = reloc_iomap(obj, cache, page);
		if (!vaddr)
			vaddr = reloc_kmap(obj, cache, page);
	}

	return vaddr;
}

static void clflush_write32(u32 *addr, u32 value, unsigned int flushes)
{
	if (unlikely(flushes & (CLFLUSH_BEFORE | CLFLUSH_AFTER))) {
		if (flushes & CLFLUSH_BEFORE) {
			clflushopt(addr);
			mb();
		}

		*addr = value;

		/* Writes to the same cacheline are serialised by the CPU
		 * (including clflush). On the write path, we only require
		 * that it hits memory in an orderly fashion and place
		 * mb barriers at the start and end of the relocation phase
		 * to ensure ordering of clflush wrt to the system.
		 */
		if (flushes & CLFLUSH_AFTER)
			clflushopt(addr);
	} else
		*addr = value;
}

static int
relocate_entry(struct drm_i915_gem_object *obj,
	       const struct drm_i915_gem_relocation_entry *reloc,
	       struct reloc_cache *cache,
	       u64 target_offset)
{
	u64 offset = reloc->offset;
	bool wide = cache->use_64bit_reloc;
	void *vaddr;

	target_offset = relocation_target(reloc, target_offset);
repeat:
	vaddr = reloc_vaddr(obj, cache, offset >> PAGE_SHIFT);
	if (IS_ERR(vaddr))
		return PTR_ERR(vaddr);

	clflush_write32(vaddr + offset_in_page(offset),
			lower_32_bits(target_offset),
			cache->vaddr);

	if (wide) {
		offset += sizeof(u32);
		target_offset >>= 32;
		wide = false;
		goto repeat;
	}
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int
relocate_entry_gtt(struct drm_i915_gem_object *obj,
		   struct drm_i915_gem_relocation_entry *reloc,
		   uint64_t target_offset)
{
	struct drm_device *dev = obj->base.dev;
	struct drm_i915_private *dev_priv = dev->dev_private;
	uint64_t delta = reloc->delta + target_offset;
	uint64_t offset;
	void __iomem *reloc_page;
	int ret;

	ret = i915_gem_object_set_to_gtt_domain(obj, true);
	if (ret)
		return ret;

	ret = i915_gem_object_put_fence(obj);
	if (ret)
		return ret;

	/* Map the page containing the relocation we're going to perform.  */
	offset = i915_gem_obj_ggtt_offset(obj);
	offset += reloc->offset;
	reloc_page = io_mapping_map_atomic_wc(dev_priv->gtt.mappable,
					      offset & PAGE_MASK);
	iowrite32(lower_32_bits(delta), reloc_page + offset_in_page(offset));

	if (INTEL_INFO(dev)->gen >= 8) {
		offset += sizeof(uint32_t);

		if (offset_in_page(offset) == 0) {
			io_mapping_unmap_atomic(reloc_page);
			reloc_page =
				io_mapping_map_atomic_wc(dev_priv->gtt.mappable,
							 offset);
		}

		iowrite32(upper_32_bits(delta),
			  reloc_page + offset_in_page(offset));
	}

	io_mapping_unmap_atomic(reloc_page);

	return 0;
}

static void
clflush_write32(void *addr, uint32_t value)
{
	/* This is not a fast path, so KISS. */
	drm_clflush_virt_range(addr, sizeof(uint32_t));
	*(uint32_t *)addr = value;
	drm_clflush_virt_range(addr, sizeof(uint32_t));
}

static int
relocate_entry_clflush(struct drm_i915_gem_object *obj,
		       struct drm_i915_gem_relocation_entry *reloc,
		       uint64_t target_offset)
{
	struct drm_device *dev = obj->base.dev;
	uint32_t page_offset = offset_in_page(reloc->offset);
	uint64_t delta = (int)reloc->delta + target_offset;
	char *vaddr;
	int ret;

	ret = i915_gem_object_set_to_gtt_domain(obj, true);
	if (ret)
		return ret;

	vaddr = kmap_atomic(i915_gem_object_get_page(obj,
				reloc->offset >> PAGE_SHIFT));
	clflush_write32(vaddr + page_offset, lower_32_bits(delta));

	if (INTEL_INFO(dev)->gen >= 8) {
		page_offset = offset_in_page(page_offset + sizeof(uint32_t));

		if (page_offset == 0) {
			kunmap_atomic(vaddr);
			vaddr = kmap_atomic(i915_gem_object_get_page(obj,
			    (reloc->offset + sizeof(uint32_t)) >> PAGE_SHIFT));
		}

		clflush_write32(vaddr + page_offset, upper_32_bits(delta));
	}

	kunmap_atomic(vaddr);

	return 0;
=======
static bool object_is_idle(struct drm_i915_gem_object *obj)
{
	unsigned long active = i915_gem_object_get_active(obj);
	int idx;

	for_each_active(active, idx) {
		if (!i915_gem_active_is_idle(&obj->last_read[idx],
					     &obj->base.dev->struct_mutex))
			return false;
	}

	return true;
>>>>>>> v4.9.227
}

static int
i915_gem_execbuffer_relocate_entry(struct drm_i915_gem_object *obj,
				   struct eb_vmas *eb,
<<<<<<< HEAD
				   struct drm_i915_gem_relocation_entry *reloc)
=======
				   struct drm_i915_gem_relocation_entry *reloc,
				   struct reloc_cache *cache)
>>>>>>> v4.9.227
{
	struct drm_device *dev = obj->base.dev;
	struct drm_gem_object *target_obj;
	struct drm_i915_gem_object *target_i915_obj;
	struct i915_vma *target_vma;
	uint64_t target_offset;
	int ret;

	/* we've already hold a reference to all valid objects */
	target_vma = eb_get_vma(eb, reloc->target_handle);
	if (unlikely(target_vma == NULL))
		return -ENOENT;
	target_i915_obj = target_vma->obj;
	target_obj = &target_vma->obj->base;

<<<<<<< HEAD
	target_offset = target_vma->node.start;
=======
	target_offset = gen8_canonical_addr(target_vma->node.start);
>>>>>>> v4.9.227

	/* Sandybridge PPGTT errata: We need a global gtt mapping for MI and
	 * pipe_control writes because the gpu doesn't properly redirect them
	 * through the ppgtt for non_secure batchbuffers. */
	if (unlikely(IS_GEN6(dev) &&
	    reloc->write_domain == I915_GEM_DOMAIN_INSTRUCTION)) {
		ret = i915_vma_bind(target_vma, target_i915_obj->cache_level,
				    PIN_GLOBAL);
		if (WARN_ONCE(ret, "Unexpected failure to bind target VMA!"))
			return ret;
	}

	/* Validate that the target is in a valid r/w GPU domain */
	if (unlikely(reloc->write_domain & (reloc->write_domain - 1))) {
		DRM_DEBUG("reloc with multiple write domains: "
			  "obj %p target %d offset %d "
			  "read %08x write %08x",
			  obj, reloc->target_handle,
			  (int) reloc->offset,
			  reloc->read_domains,
			  reloc->write_domain);
		return -EINVAL;
	}
	if (unlikely((reloc->write_domain | reloc->read_domains)
		     & ~I915_GEM_GPU_DOMAINS)) {
		DRM_DEBUG("reloc with read/write non-GPU domains: "
			  "obj %p target %d offset %d "
			  "read %08x write %08x",
			  obj, reloc->target_handle,
			  (int) reloc->offset,
			  reloc->read_domains,
			  reloc->write_domain);
		return -EINVAL;
	}

	target_obj->pending_read_domains |= reloc->read_domains;
	target_obj->pending_write_domain |= reloc->write_domain;

	/* If the relocation already has the right value in it, no
	 * more work needs to be done.
	 */
	if (target_offset == reloc->presumed_offset)
		return 0;

	/* Check that the relocation address is valid... */
	if (unlikely(reloc->offset >
<<<<<<< HEAD
		obj->base.size - (INTEL_INFO(dev)->gen >= 8 ? 8 : 4))) {
=======
		     obj->base.size - (cache->use_64bit_reloc ? 8 : 4))) {
>>>>>>> v4.9.227
		DRM_DEBUG("Relocation beyond object bounds: "
			  "obj %p target %d offset %d size %d.\n",
			  obj, reloc->target_handle,
			  (int) reloc->offset,
			  (int) obj->base.size);
		return -EINVAL;
	}
	if (unlikely(reloc->offset & 3)) {
		DRM_DEBUG("Relocation not 4-byte aligned: "
			  "obj %p target %d offset %d.\n",
			  obj, reloc->target_handle,
			  (int) reloc->offset);
		return -EINVAL;
	}

	/* We can't wait for rendering with pagefaults disabled */
<<<<<<< HEAD
	if (obj->active && pagefault_disabled())
		return -EFAULT;

	if (use_cpu_reloc(obj))
		ret = relocate_entry_cpu(obj, reloc, target_offset);
	else if (obj->map_and_fenceable)
		ret = relocate_entry_gtt(obj, reloc, target_offset);
	else if (cpu_has_clflush)
		ret = relocate_entry_clflush(obj, reloc, target_offset);
	else {
		WARN_ONCE(1, "Impossible case in relocation handling\n");
		ret = -ENODEV;
	}

=======
	if (pagefault_disabled() && !object_is_idle(obj))
		return -EFAULT;

	ret = relocate_entry(obj, reloc, cache, target_offset);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* and update the user's relocation entry */
	reloc->presumed_offset = target_offset;
<<<<<<< HEAD

=======
>>>>>>> v4.9.227
	return 0;
}

static int
i915_gem_execbuffer_relocate_vma(struct i915_vma *vma,
				 struct eb_vmas *eb)
{
#define N_RELOC(x) ((x) / sizeof(struct drm_i915_gem_relocation_entry))
	struct drm_i915_gem_relocation_entry stack_reloc[N_RELOC(512)];
	struct drm_i915_gem_relocation_entry __user *user_relocs;
	struct drm_i915_gem_exec_object2 *entry = vma->exec_entry;
<<<<<<< HEAD
	int remain, ret;

	user_relocs = to_user_ptr(entry->relocs_ptr);
=======
	struct reloc_cache cache;
	int remain, ret = 0;

	user_relocs = u64_to_user_ptr(entry->relocs_ptr);
	reloc_cache_init(&cache, eb->i915);
>>>>>>> v4.9.227

	remain = entry->relocation_count;
	while (remain) {
		struct drm_i915_gem_relocation_entry *r = stack_reloc;
		int count = remain;
		if (count > ARRAY_SIZE(stack_reloc))
			count = ARRAY_SIZE(stack_reloc);
		remain -= count;

<<<<<<< HEAD
		if (__copy_from_user_inatomic(r, user_relocs, count*sizeof(r[0])))
			return -EFAULT;
=======
		if (__copy_from_user_inatomic(r, user_relocs, count*sizeof(r[0]))) {
			ret = -EFAULT;
			goto out;
		}
>>>>>>> v4.9.227

		do {
			u64 offset = r->presumed_offset;

<<<<<<< HEAD
			ret = i915_gem_execbuffer_relocate_entry(vma->obj, eb, r);
			if (ret)
				return ret;

			if (r->presumed_offset != offset &&
			    __copy_to_user_inatomic(&user_relocs->presumed_offset,
						    &r->presumed_offset,
						    sizeof(r->presumed_offset))) {
				return -EFAULT;
=======
			ret = i915_gem_execbuffer_relocate_entry(vma->obj, eb, r, &cache);
			if (ret)
				goto out;

			if (r->presumed_offset != offset &&
			    __put_user(r->presumed_offset,
				       &user_relocs->presumed_offset)) {
				ret = -EFAULT;
				goto out;
>>>>>>> v4.9.227
			}

			user_relocs++;
			r++;
		} while (--count);
	}

<<<<<<< HEAD
	return 0;
=======
out:
	reloc_cache_fini(&cache);
	return ret;
>>>>>>> v4.9.227
#undef N_RELOC
}

static int
i915_gem_execbuffer_relocate_vma_slow(struct i915_vma *vma,
				      struct eb_vmas *eb,
				      struct drm_i915_gem_relocation_entry *relocs)
{
	const struct drm_i915_gem_exec_object2 *entry = vma->exec_entry;
<<<<<<< HEAD
	int i, ret;

	for (i = 0; i < entry->relocation_count; i++) {
		ret = i915_gem_execbuffer_relocate_entry(vma->obj, eb, &relocs[i]);
		if (ret)
			return ret;
	}

	return 0;
=======
	struct reloc_cache cache;
	int i, ret = 0;

	reloc_cache_init(&cache, eb->i915);
	for (i = 0; i < entry->relocation_count; i++) {
		ret = i915_gem_execbuffer_relocate_entry(vma->obj, eb, &relocs[i], &cache);
		if (ret)
			break;
	}
	reloc_cache_fini(&cache);

	return ret;
>>>>>>> v4.9.227
}

static int
i915_gem_execbuffer_relocate(struct eb_vmas *eb)
{
	struct i915_vma *vma;
	int ret = 0;

	/* This is the fast path and we cannot handle a pagefault whilst
	 * holding the struct mutex lest the user pass in the relocations
	 * contained within a mmaped bo. For in such a case we, the page
	 * fault handler would call i915_gem_fault() and we would try to
	 * acquire the struct mutex again. Obviously this is bad and so
	 * lockdep complains vehemently.
	 */
	pagefault_disable();
	list_for_each_entry(vma, &eb->vmas, exec_list) {
		ret = i915_gem_execbuffer_relocate_vma(vma, eb);
		if (ret)
			break;
	}
	pagefault_enable();

	return ret;
}

static bool only_mappable_for_reloc(unsigned int flags)
{
	return (flags & (EXEC_OBJECT_NEEDS_FENCE | __EXEC_OBJECT_NEEDS_MAP)) ==
		__EXEC_OBJECT_NEEDS_MAP;
}

static int
i915_gem_execbuffer_reserve_vma(struct i915_vma *vma,
<<<<<<< HEAD
				struct intel_engine_cs *ring,
=======
				struct intel_engine_cs *engine,
>>>>>>> v4.9.227
				bool *need_reloc)
{
	struct drm_i915_gem_object *obj = vma->obj;
	struct drm_i915_gem_exec_object2 *entry = vma->exec_entry;
	uint64_t flags;
	int ret;

	flags = PIN_USER;
	if (entry->flags & EXEC_OBJECT_NEEDS_GTT)
		flags |= PIN_GLOBAL;

	if (!drm_mm_node_allocated(&vma->node)) {
		/* Wa32bitGeneralStateOffset & Wa32bitInstructionBaseOffset,
		 * limit address to the first 4GBs for unflagged objects.
		 */
		if ((entry->flags & EXEC_OBJECT_SUPPORTS_48B_ADDRESS) == 0)
			flags |= PIN_ZONE_4G;
		if (entry->flags & __EXEC_OBJECT_NEEDS_MAP)
			flags |= PIN_GLOBAL | PIN_MAPPABLE;
		if (entry->flags & __EXEC_OBJECT_NEEDS_BIAS)
			flags |= BATCH_OFFSET_BIAS | PIN_OFFSET_BIAS;
<<<<<<< HEAD
=======
		if (entry->flags & EXEC_OBJECT_PINNED)
			flags |= entry->offset | PIN_OFFSET_FIXED;
>>>>>>> v4.9.227
		if ((flags & PIN_MAPPABLE) == 0)
			flags |= PIN_HIGH;
	}

<<<<<<< HEAD
	ret = i915_gem_object_pin(obj, vma->vm, entry->alignment, flags);
	if ((ret == -ENOSPC  || ret == -E2BIG) &&
	    only_mappable_for_reloc(entry->flags))
		ret = i915_gem_object_pin(obj, vma->vm,
					  entry->alignment,
					  flags & ~PIN_MAPPABLE);
=======
	ret = i915_vma_pin(vma,
			   entry->pad_to_size,
			   entry->alignment,
			   flags);
	if ((ret == -ENOSPC || ret == -E2BIG) &&
	    only_mappable_for_reloc(entry->flags))
		ret = i915_vma_pin(vma,
				   entry->pad_to_size,
				   entry->alignment,
				   flags & ~PIN_MAPPABLE);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	entry->flags |= __EXEC_OBJECT_HAS_PIN;

	if (entry->flags & EXEC_OBJECT_NEEDS_FENCE) {
<<<<<<< HEAD
		ret = i915_gem_object_get_fence(obj);
		if (ret)
			return ret;

		if (i915_gem_object_pin_fence(obj))
=======
		ret = i915_vma_get_fence(vma);
		if (ret)
			return ret;

		if (i915_vma_pin_fence(vma))
>>>>>>> v4.9.227
			entry->flags |= __EXEC_OBJECT_HAS_FENCE;
	}

	if (entry->offset != vma->node.start) {
		entry->offset = vma->node.start;
		*need_reloc = true;
	}

	if (entry->flags & EXEC_OBJECT_WRITE) {
		obj->base.pending_read_domains = I915_GEM_DOMAIN_RENDER;
		obj->base.pending_write_domain = I915_GEM_DOMAIN_RENDER;
	}

	return 0;
}

static bool
need_reloc_mappable(struct i915_vma *vma)
{
	struct drm_i915_gem_exec_object2 *entry = vma->exec_entry;

	if (entry->relocation_count == 0)
		return false;

<<<<<<< HEAD
	if (!i915_is_ggtt(vma->vm))
=======
	if (!i915_vma_is_ggtt(vma))
>>>>>>> v4.9.227
		return false;

	/* See also use_cpu_reloc() */
	if (HAS_LLC(vma->obj->base.dev))
		return false;

	if (vma->obj->base.write_domain == I915_GEM_DOMAIN_CPU)
		return false;

	return true;
}

static bool
eb_vma_misplaced(struct i915_vma *vma)
{
	struct drm_i915_gem_exec_object2 *entry = vma->exec_entry;
<<<<<<< HEAD
	struct drm_i915_gem_object *obj = vma->obj;

	WARN_ON(entry->flags & __EXEC_OBJECT_NEEDS_MAP &&
	       !i915_is_ggtt(vma->vm));
=======

	WARN_ON(entry->flags & __EXEC_OBJECT_NEEDS_MAP &&
		!i915_vma_is_ggtt(vma));
>>>>>>> v4.9.227

	if (entry->alignment &&
	    vma->node.start & (entry->alignment - 1))
		return true;

<<<<<<< HEAD
=======
	if (vma->node.size < entry->pad_to_size)
		return true;

	if (entry->flags & EXEC_OBJECT_PINNED &&
	    vma->node.start != entry->offset)
		return true;

>>>>>>> v4.9.227
	if (entry->flags & __EXEC_OBJECT_NEEDS_BIAS &&
	    vma->node.start < BATCH_OFFSET_BIAS)
		return true;

	/* avoid costly ping-pong once a batch bo ended up non-mappable */
<<<<<<< HEAD
	if (entry->flags & __EXEC_OBJECT_NEEDS_MAP && !obj->map_and_fenceable)
=======
	if (entry->flags & __EXEC_OBJECT_NEEDS_MAP &&
	    !i915_vma_is_map_and_fenceable(vma))
>>>>>>> v4.9.227
		return !only_mappable_for_reloc(entry->flags);

	if ((entry->flags & EXEC_OBJECT_SUPPORTS_48B_ADDRESS) == 0 &&
	    (vma->node.start + vma->node.size - 1) >> 32)
		return true;

	return false;
}

static int
<<<<<<< HEAD
i915_gem_execbuffer_reserve(struct intel_engine_cs *ring,
			    struct list_head *vmas,
			    struct intel_context *ctx,
=======
i915_gem_execbuffer_reserve(struct intel_engine_cs *engine,
			    struct list_head *vmas,
			    struct i915_gem_context *ctx,
>>>>>>> v4.9.227
			    bool *need_relocs)
{
	struct drm_i915_gem_object *obj;
	struct i915_vma *vma;
	struct i915_address_space *vm;
	struct list_head ordered_vmas;
<<<<<<< HEAD
	bool has_fenced_gpu_access = INTEL_INFO(ring->dev)->gen < 4;
	int retry;

	i915_gem_retire_requests_ring(ring);

	vm = list_first_entry(vmas, struct i915_vma, exec_list)->vm;

	INIT_LIST_HEAD(&ordered_vmas);
=======
	struct list_head pinned_vmas;
	bool has_fenced_gpu_access = INTEL_GEN(engine->i915) < 4;
	int retry;

	vm = list_first_entry(vmas, struct i915_vma, exec_list)->vm;

	INIT_LIST_HEAD(&ordered_vmas);
	INIT_LIST_HEAD(&pinned_vmas);
>>>>>>> v4.9.227
	while (!list_empty(vmas)) {
		struct drm_i915_gem_exec_object2 *entry;
		bool need_fence, need_mappable;

		vma = list_first_entry(vmas, struct i915_vma, exec_list);
		obj = vma->obj;
		entry = vma->exec_entry;

		if (ctx->flags & CONTEXT_NO_ZEROMAP)
			entry->flags |= __EXEC_OBJECT_NEEDS_BIAS;

		if (!has_fenced_gpu_access)
			entry->flags &= ~EXEC_OBJECT_NEEDS_FENCE;
		need_fence =
			entry->flags & EXEC_OBJECT_NEEDS_FENCE &&
<<<<<<< HEAD
			obj->tiling_mode != I915_TILING_NONE;
		need_mappable = need_fence || need_reloc_mappable(vma);

		if (need_mappable) {
=======
			i915_gem_object_is_tiled(obj);
		need_mappable = need_fence || need_reloc_mappable(vma);

		if (entry->flags & EXEC_OBJECT_PINNED)
			list_move_tail(&vma->exec_list, &pinned_vmas);
		else if (need_mappable) {
>>>>>>> v4.9.227
			entry->flags |= __EXEC_OBJECT_NEEDS_MAP;
			list_move(&vma->exec_list, &ordered_vmas);
		} else
			list_move_tail(&vma->exec_list, &ordered_vmas);

		obj->base.pending_read_domains = I915_GEM_GPU_DOMAINS & ~I915_GEM_DOMAIN_COMMAND;
		obj->base.pending_write_domain = 0;
	}
	list_splice(&ordered_vmas, vmas);
<<<<<<< HEAD
=======
	list_splice(&pinned_vmas, vmas);
>>>>>>> v4.9.227

	/* Attempt to pin all of the buffers into the GTT.
	 * This is done in 3 phases:
	 *
	 * 1a. Unbind all objects that do not match the GTT constraints for
	 *     the execbuffer (fenceable, mappable, alignment etc).
	 * 1b. Increment pin count for already bound objects.
	 * 2.  Bind new objects.
	 * 3.  Decrement pin count.
	 *
	 * This avoid unnecessary unbinding of later objects in order to make
	 * room for the earlier objects *unless* we need to defragment.
	 */
	retry = 0;
	do {
		int ret = 0;

		/* Unbind any ill-fitting objects or pin. */
		list_for_each_entry(vma, vmas, exec_list) {
			if (!drm_mm_node_allocated(&vma->node))
				continue;

			if (eb_vma_misplaced(vma))
				ret = i915_vma_unbind(vma);
			else
<<<<<<< HEAD
				ret = i915_gem_execbuffer_reserve_vma(vma, ring, need_relocs);
=======
				ret = i915_gem_execbuffer_reserve_vma(vma,
								      engine,
								      need_relocs);
>>>>>>> v4.9.227
			if (ret)
				goto err;
		}

		/* Bind fresh objects */
		list_for_each_entry(vma, vmas, exec_list) {
			if (drm_mm_node_allocated(&vma->node))
				continue;

<<<<<<< HEAD
			ret = i915_gem_execbuffer_reserve_vma(vma, ring, need_relocs);
=======
			ret = i915_gem_execbuffer_reserve_vma(vma, engine,
							      need_relocs);
>>>>>>> v4.9.227
			if (ret)
				goto err;
		}

err:
		if (ret != -ENOSPC || retry++)
			return ret;

		/* Decrement pin count for bound objects */
		list_for_each_entry(vma, vmas, exec_list)
			i915_gem_execbuffer_unreserve_vma(vma);

		ret = i915_gem_evict_vm(vm, true);
		if (ret)
			return ret;
	} while (1);
}

static int
i915_gem_execbuffer_relocate_slow(struct drm_device *dev,
				  struct drm_i915_gem_execbuffer2 *args,
				  struct drm_file *file,
<<<<<<< HEAD
				  struct intel_engine_cs *ring,
				  struct eb_vmas *eb,
				  struct drm_i915_gem_exec_object2 *exec,
				  struct intel_context *ctx)
=======
				  struct intel_engine_cs *engine,
				  struct eb_vmas *eb,
				  struct drm_i915_gem_exec_object2 *exec,
				  struct i915_gem_context *ctx)
>>>>>>> v4.9.227
{
	struct drm_i915_gem_relocation_entry *reloc;
	struct i915_address_space *vm;
	struct i915_vma *vma;
	bool need_relocs;
	int *reloc_offset;
	int i, total, ret;
	unsigned count = args->buffer_count;

	vm = list_first_entry(&eb->vmas, struct i915_vma, exec_list)->vm;

	/* We may process another execbuffer during the unlock... */
	while (!list_empty(&eb->vmas)) {
		vma = list_first_entry(&eb->vmas, struct i915_vma, exec_list);
		list_del_init(&vma->exec_list);
		i915_gem_execbuffer_unreserve_vma(vma);
<<<<<<< HEAD
		drm_gem_object_unreference(&vma->obj->base);
=======
		i915_vma_put(vma);
>>>>>>> v4.9.227
	}

	mutex_unlock(&dev->struct_mutex);

	total = 0;
	for (i = 0; i < count; i++)
		total += exec[i].relocation_count;

	reloc_offset = drm_malloc_ab(count, sizeof(*reloc_offset));
	reloc = drm_malloc_ab(total, sizeof(*reloc));
	if (reloc == NULL || reloc_offset == NULL) {
		drm_free_large(reloc);
		drm_free_large(reloc_offset);
		mutex_lock(&dev->struct_mutex);
		return -ENOMEM;
	}

	total = 0;
	for (i = 0; i < count; i++) {
		struct drm_i915_gem_relocation_entry __user *user_relocs;
		u64 invalid_offset = (u64)-1;
		int j;

<<<<<<< HEAD
		user_relocs = to_user_ptr(exec[i].relocs_ptr);
=======
		user_relocs = u64_to_user_ptr(exec[i].relocs_ptr);
>>>>>>> v4.9.227

		if (copy_from_user(reloc+total, user_relocs,
				   exec[i].relocation_count * sizeof(*reloc))) {
			ret = -EFAULT;
			mutex_lock(&dev->struct_mutex);
			goto err;
		}

		/* As we do not update the known relocation offsets after
		 * relocating (due to the complexities in lock handling),
		 * we need to mark them as invalid now so that we force the
		 * relocation processing next time. Just in case the target
		 * object is evicted and then rebound into its old
		 * presumed_offset before the next execbuffer - if that
		 * happened we would make the mistake of assuming that the
		 * relocations were valid.
		 */
		for (j = 0; j < exec[i].relocation_count; j++) {
			if (__copy_to_user(&user_relocs[j].presumed_offset,
					   &invalid_offset,
					   sizeof(invalid_offset))) {
				ret = -EFAULT;
				mutex_lock(&dev->struct_mutex);
				goto err;
			}
		}

		reloc_offset[i] = total;
		total += exec[i].relocation_count;
	}

	ret = i915_mutex_lock_interruptible(dev);
	if (ret) {
		mutex_lock(&dev->struct_mutex);
		goto err;
	}

	/* reacquire the objects */
	eb_reset(eb);
	ret = eb_lookup_vmas(eb, exec, args, vm, file);
	if (ret)
		goto err;

	need_relocs = (args->flags & I915_EXEC_NO_RELOC) == 0;
<<<<<<< HEAD
	ret = i915_gem_execbuffer_reserve(ring, &eb->vmas, ctx, &need_relocs);
=======
	ret = i915_gem_execbuffer_reserve(engine, &eb->vmas, ctx,
					  &need_relocs);
>>>>>>> v4.9.227
	if (ret)
		goto err;

	list_for_each_entry(vma, &eb->vmas, exec_list) {
		int offset = vma->exec_entry - exec;
		ret = i915_gem_execbuffer_relocate_vma_slow(vma, eb,
							    reloc + reloc_offset[offset]);
		if (ret)
			goto err;
	}

	/* Leave the user relocations as are, this is the painfully slow path,
	 * and we want to avoid the complication of dropping the lock whilst
	 * having buffers reserved in the aperture and so causing spurious
	 * ENOSPC for random operations.
	 */

err:
	drm_free_large(reloc);
	drm_free_large(reloc_offset);
	return ret;
}

<<<<<<< HEAD
=======
static unsigned int eb_other_engines(struct drm_i915_gem_request *req)
{
	unsigned int mask;

	mask = ~intel_engine_flag(req->engine) & I915_BO_ACTIVE_MASK;
	mask <<= I915_BO_ACTIVE_SHIFT;

	return mask;
}

>>>>>>> v4.9.227
static int
i915_gem_execbuffer_move_to_gpu(struct drm_i915_gem_request *req,
				struct list_head *vmas)
{
<<<<<<< HEAD
	const unsigned other_rings = ~intel_ring_flag(req->ring);
	struct i915_vma *vma;
	uint32_t flush_domains = 0;
	bool flush_chipset = false;
=======
	const unsigned int other_rings = eb_other_engines(req);
	struct i915_vma *vma;
>>>>>>> v4.9.227
	int ret;

	list_for_each_entry(vma, vmas, exec_list) {
		struct drm_i915_gem_object *obj = vma->obj;
<<<<<<< HEAD

		if (obj->active & other_rings) {
			ret = i915_gem_object_sync(obj, req->ring, &req);
=======
		struct reservation_object *resv;

		if (obj->flags & other_rings) {
			ret = i915_gem_request_await_object
				(req, obj, obj->base.pending_write_domain);
>>>>>>> v4.9.227
			if (ret)
				return ret;
		}

<<<<<<< HEAD
		if (obj->base.write_domain & I915_GEM_DOMAIN_CPU)
			flush_chipset |= i915_gem_clflush_object(obj, false);

		flush_domains |= obj->base.write_domain;
	}

	if (flush_chipset)
		i915_gem_chipset_flush(req->ring->dev);

	if (flush_domains & I915_GEM_DOMAIN_GTT)
		wmb();

	/* Unconditionally invalidate gpu caches and ensure that we do flush
	 * any residual writes from the previous batch.
	 */
	return intel_ring_invalidate_all_caches(req);
=======
		resv = i915_gem_object_get_dmabuf_resv(obj);
		if (resv) {
			ret = i915_sw_fence_await_reservation
				(&req->submit, resv, &i915_fence_ops,
				 obj->base.pending_write_domain, 10*HZ,
				 GFP_KERNEL | __GFP_NOWARN);
			if (ret < 0)
				return ret;
		}

		if (obj->base.write_domain & I915_GEM_DOMAIN_CPU)
			i915_gem_clflush_object(obj, false);
	}

	/* Unconditionally flush any chipset caches (for streaming writes). */
	i915_gem_chipset_flush(req->engine->i915);

	/* Unconditionally invalidate GPU caches and TLBs. */
	return req->engine->emit_flush(req, EMIT_INVALIDATE);
>>>>>>> v4.9.227
}

static bool
i915_gem_check_execbuffer(struct drm_i915_gem_execbuffer2 *exec)
{
	if (exec->flags & __I915_EXEC_UNKNOWN_FLAGS)
		return false;

	/* Kernel clipping was a DRI1 misfeature */
	if (exec->num_cliprects || exec->cliprects_ptr)
		return false;

	if (exec->DR4 == 0xffffffff) {
		DRM_DEBUG("UXA submitting garbage DR4, fixing up\n");
		exec->DR4 = 0;
	}
	if (exec->DR1 || exec->DR4)
		return false;

	if ((exec->batch_start_offset | exec->batch_len) & 0x7)
		return false;

	return true;
}

static int
validate_exec_list(struct drm_device *dev,
		   struct drm_i915_gem_exec_object2 *exec,
		   int count)
{
	unsigned relocs_total = 0;
	unsigned relocs_max = UINT_MAX / sizeof(struct drm_i915_gem_relocation_entry);
	unsigned invalid_flags;
	int i;

<<<<<<< HEAD
=======
	/* INTERNAL flags must not overlap with external ones */
	BUILD_BUG_ON(__EXEC_OBJECT_INTERNAL_FLAGS & ~__EXEC_OBJECT_UNKNOWN_FLAGS);

>>>>>>> v4.9.227
	invalid_flags = __EXEC_OBJECT_UNKNOWN_FLAGS;
	if (USES_FULL_PPGTT(dev))
		invalid_flags |= EXEC_OBJECT_NEEDS_GTT;

	for (i = 0; i < count; i++) {
<<<<<<< HEAD
		char __user *ptr = to_user_ptr(exec[i].relocs_ptr);
=======
		char __user *ptr = u64_to_user_ptr(exec[i].relocs_ptr);
>>>>>>> v4.9.227
		int length; /* limited by fault_in_pages_readable() */

		if (exec[i].flags & invalid_flags)
			return -EINVAL;

<<<<<<< HEAD
		if (exec[i].alignment && !is_power_of_2(exec[i].alignment))
			return -EINVAL;

=======
		/* Offset can be used as input (EXEC_OBJECT_PINNED), reject
		 * any non-page-aligned or non-canonical addresses.
		 */
		if (exec[i].flags & EXEC_OBJECT_PINNED) {
			if (exec[i].offset !=
			    gen8_canonical_addr(exec[i].offset & PAGE_MASK))
				return -EINVAL;
		}

		/* From drm_mm perspective address space is continuous,
		 * so from this point we're always using non-canonical
		 * form internally.
		 */
		exec[i].offset = gen8_noncanonical_addr(exec[i].offset);

		if (exec[i].alignment && !is_power_of_2(exec[i].alignment))
			return -EINVAL;

		/* pad_to_size was once a reserved field, so sanitize it */
		if (exec[i].flags & EXEC_OBJECT_PAD_TO_SIZE) {
			if (offset_in_page(exec[i].pad_to_size))
				return -EINVAL;
		} else {
			exec[i].pad_to_size = 0;
		}

>>>>>>> v4.9.227
		/* First check for malicious input causing overflow in
		 * the worst case where we need to allocate the entire
		 * relocation tree as a single array.
		 */
		if (exec[i].relocation_count > relocs_max - relocs_total)
			return -EINVAL;
		relocs_total += exec[i].relocation_count;

		length = exec[i].relocation_count *
			sizeof(struct drm_i915_gem_relocation_entry);
		/*
		 * We must check that the entire relocation array is safe
		 * to read, but since we may need to update the presumed
		 * offsets during execution, check for full write access.
		 */
		if (!access_ok(VERIFY_WRITE, ptr, length))
			return -EFAULT;

		if (likely(!i915.prefault_disable)) {
<<<<<<< HEAD
			if (fault_in_multipages_readable(ptr, length))
=======
			if (fault_in_pages_readable(ptr, length))
>>>>>>> v4.9.227
				return -EFAULT;
		}
	}

	return 0;
}

<<<<<<< HEAD
static struct intel_context *
i915_gem_validate_context(struct drm_device *dev, struct drm_file *file,
			  struct intel_engine_cs *ring, const u32 ctx_id)
{
	struct intel_context *ctx = NULL;
	struct i915_ctx_hang_stats *hs;

	if (ring->id != RCS && ctx_id != DEFAULT_CONTEXT_HANDLE)
		return ERR_PTR(-EINVAL);

	ctx = i915_gem_context_get(file->driver_priv, ctx_id);
=======
static struct i915_gem_context *
i915_gem_validate_context(struct drm_device *dev, struct drm_file *file,
			  struct intel_engine_cs *engine, const u32 ctx_id)
{
	struct i915_gem_context *ctx;
	struct i915_ctx_hang_stats *hs;

	ctx = i915_gem_context_lookup(file->driver_priv, ctx_id);
>>>>>>> v4.9.227
	if (IS_ERR(ctx))
		return ctx;

	hs = &ctx->hang_stats;
	if (hs->banned) {
		DRM_DEBUG("Context %u tried to submit while banned\n", ctx_id);
		return ERR_PTR(-EIO);
	}

<<<<<<< HEAD
	if (i915.enable_execlists && !ctx->engine[ring->id].state) {
		int ret = intel_lr_context_deferred_alloc(ctx, ring);
		if (ret) {
			DRM_DEBUG("Could not create LRC %u: %d\n", ctx_id, ret);
			return ERR_PTR(ret);
		}
	}

	return ctx;
}

void
i915_gem_execbuffer_move_to_active(struct list_head *vmas,
				   struct drm_i915_gem_request *req)
{
	struct intel_engine_cs *ring = i915_gem_request_get_ring(req);
	struct i915_vma *vma;

	list_for_each_entry(vma, vmas, exec_list) {
		struct drm_i915_gem_exec_object2 *entry = vma->exec_entry;
=======
	return ctx;
}

static bool gpu_write_needs_clflush(struct drm_i915_gem_object *obj)
{
	return !(obj->cache_level == I915_CACHE_NONE ||
		 obj->cache_level == I915_CACHE_WT);
}

void i915_vma_move_to_active(struct i915_vma *vma,
			     struct drm_i915_gem_request *req,
			     unsigned int flags)
{
	struct drm_i915_gem_object *obj = vma->obj;
	const unsigned int idx = req->engine->id;

	GEM_BUG_ON(!drm_mm_node_allocated(&vma->node));

	obj->dirty = 1; /* be paranoid  */

	/* Add a reference if we're newly entering the active list.
	 * The order in which we add operations to the retirement queue is
	 * vital here: mark_active adds to the start of the callback list,
	 * such that subsequent callbacks are called first. Therefore we
	 * add the active reference first and queue for it to be dropped
	 * *last*.
	 */
	if (!i915_gem_object_is_active(obj))
		i915_gem_object_get(obj);
	i915_gem_object_set_active(obj, idx);
	i915_gem_active_set(&obj->last_read[idx], req);

	if (flags & EXEC_OBJECT_WRITE) {
		i915_gem_active_set(&obj->last_write, req);

		intel_fb_obj_invalidate(obj, ORIGIN_CS);

		/* update for the implicit flush after a batch */
		obj->base.write_domain &= ~I915_GEM_GPU_DOMAINS;
		if (!obj->cache_dirty && gpu_write_needs_clflush(obj))
			obj->cache_dirty = true;
	}

	if (flags & EXEC_OBJECT_NEEDS_FENCE)
		i915_gem_active_set(&vma->last_fence, req);

	i915_vma_set_active(vma, idx);
	i915_gem_active_set(&vma->last_read[idx], req);
	list_move_tail(&vma->vm_link, &vma->vm->active_list);
}

static void eb_export_fence(struct drm_i915_gem_object *obj,
			    struct drm_i915_gem_request *req,
			    unsigned int flags)
{
	struct reservation_object *resv;

	resv = i915_gem_object_get_dmabuf_resv(obj);
	if (!resv)
		return;

	/* Ignore errors from failing to allocate the new fence, we can't
	 * handle an error right now. Worst case should be missed
	 * synchronisation leading to rendering corruption.
	 */
	ww_mutex_lock(&resv->lock, NULL);
	if (flags & EXEC_OBJECT_WRITE)
		reservation_object_add_excl_fence(resv, &req->fence);
	else if (reservation_object_reserve_shared(resv) == 0)
		reservation_object_add_shared_fence(resv, &req->fence);
	ww_mutex_unlock(&resv->lock);
}

static void
i915_gem_execbuffer_move_to_active(struct list_head *vmas,
				   struct drm_i915_gem_request *req)
{
	struct i915_vma *vma;

	list_for_each_entry(vma, vmas, exec_list) {
>>>>>>> v4.9.227
		struct drm_i915_gem_object *obj = vma->obj;
		u32 old_read = obj->base.read_domains;
		u32 old_write = obj->base.write_domain;

<<<<<<< HEAD
		obj->dirty = 1; /* be paranoid  */
		obj->base.write_domain = obj->base.pending_write_domain;
		if (obj->base.write_domain == 0)
			obj->base.pending_read_domains |= obj->base.read_domains;
		obj->base.read_domains = obj->base.pending_read_domains;

		i915_vma_move_to_active(vma, req);
		if (obj->base.write_domain) {
			i915_gem_request_assign(&obj->last_write_req, req);

			intel_fb_obj_invalidate(obj, ORIGIN_CS);

			/* update for the implicit flush after a batch */
			obj->base.write_domain &= ~I915_GEM_GPU_DOMAINS;
		}
		if (entry->flags & EXEC_OBJECT_NEEDS_FENCE) {
			i915_gem_request_assign(&obj->last_fenced_req, req);
			if (entry->flags & __EXEC_OBJECT_HAS_FENCE) {
				struct drm_i915_private *dev_priv = to_i915(ring->dev);
				list_move_tail(&dev_priv->fence_regs[obj->fence_reg].lru_list,
					       &dev_priv->mm.fence_list);
			}
		}

=======
		obj->base.write_domain = obj->base.pending_write_domain;
		if (obj->base.write_domain)
			vma->exec_entry->flags |= EXEC_OBJECT_WRITE;
		else
			obj->base.pending_read_domains |= obj->base.read_domains;
		obj->base.read_domains = obj->base.pending_read_domains;

		i915_vma_move_to_active(vma, req, vma->exec_entry->flags);
		eb_export_fence(obj, req, vma->exec_entry->flags);
>>>>>>> v4.9.227
		trace_i915_gem_object_change_domain(obj, old_read, old_write);
	}
}

<<<<<<< HEAD
void
i915_gem_execbuffer_retire_commands(struct i915_execbuffer_params *params)
{
	/* Unconditionally force add_request to emit a full flush. */
	params->ring->gpu_caches_dirty = true;

	/* Add a breadcrumb for the completion of the batch buffer */
	__i915_add_request(params->request, params->batch_obj, true);
}

static int
i915_reset_gen7_sol_offsets(struct drm_device *dev,
			    struct drm_i915_gem_request *req)
{
	struct intel_engine_cs *ring = req->ring;
	struct drm_i915_private *dev_priv = dev->dev_private;
	int ret, i;

	if (!IS_GEN7(dev) || ring != &dev_priv->ring[RCS]) {
=======
static int
i915_reset_gen7_sol_offsets(struct drm_i915_gem_request *req)
{
	struct intel_ring *ring = req->ring;
	int ret, i;

	if (!IS_GEN7(req->i915) || req->engine->id != RCS) {
>>>>>>> v4.9.227
		DRM_DEBUG("sol reset is gen7/rcs only\n");
		return -EINVAL;
	}

	ret = intel_ring_begin(req, 4 * 3);
	if (ret)
		return ret;

	for (i = 0; i < 4; i++) {
		intel_ring_emit(ring, MI_LOAD_REGISTER_IMM(1));
<<<<<<< HEAD
		intel_ring_emit(ring, GEN7_SO_WRITE_OFFSET(i));
=======
		intel_ring_emit_reg(ring, GEN7_SO_WRITE_OFFSET(i));
>>>>>>> v4.9.227
		intel_ring_emit(ring, 0);
	}

	intel_ring_advance(ring);

	return 0;
}

<<<<<<< HEAD
static struct drm_i915_gem_object*
i915_gem_execbuffer_parse(struct intel_engine_cs *ring,
			  struct drm_i915_gem_exec_object2 *shadow_exec_entry,
			  struct eb_vmas *eb,
			  struct drm_i915_gem_object *batch_obj,
			  u32 batch_start_offset,
			  u32 batch_len,
			  bool is_master)
{
	struct drm_i915_gem_object *shadow_batch_obj;
	struct i915_vma *vma;
	int ret;

	shadow_batch_obj = i915_gem_batch_pool_get(&ring->batch_pool,
						   PAGE_ALIGN(batch_len));
	if (IS_ERR(shadow_batch_obj))
		return shadow_batch_obj;

	ret = i915_parse_cmds(ring,
			      batch_obj,
			      shadow_batch_obj,
			      batch_start_offset,
			      batch_len,
			      is_master);
	if (ret)
		goto err;

	ret = i915_gem_obj_ggtt_pin(shadow_batch_obj, 0, 0);
	if (ret)
		goto err;

	i915_gem_object_unpin_pages(shadow_batch_obj);

	memset(shadow_exec_entry, 0, sizeof(*shadow_exec_entry));

	vma = i915_gem_obj_to_ggtt(shadow_batch_obj);
	vma->exec_entry = shadow_exec_entry;
	vma->exec_entry->flags = __EXEC_OBJECT_HAS_PIN;
	drm_gem_object_reference(&shadow_batch_obj->base);
	list_add_tail(&vma->exec_list, &eb->vmas);

	shadow_batch_obj->base.pending_read_domains = I915_GEM_DOMAIN_COMMAND;

	return shadow_batch_obj;

err:
	i915_gem_object_unpin_pages(shadow_batch_obj);
	if (ret == -EACCES) /* unhandled chained batch */
		return batch_obj;
	else
		return ERR_PTR(ret);
}

int
i915_gem_ringbuffer_submission(struct i915_execbuffer_params *params,
			       struct drm_i915_gem_execbuffer2 *args,
			       struct list_head *vmas)
{
	struct drm_device *dev = params->dev;
	struct intel_engine_cs *ring = params->ring;
	struct drm_i915_private *dev_priv = dev->dev_private;
	u64 exec_start, exec_len;
	int instp_mode;
	u32 instp_mask;
=======
static struct i915_vma*
shadow_batch_pin(struct drm_i915_gem_object *obj, struct i915_address_space *vm)
{
	struct drm_i915_private *dev_priv = to_i915(obj->base.dev);
	u64 flags;

	/*
	 * PPGTT backed shadow buffers must be mapped RO, to prevent
	 * post-scan tampering
	 */
	if (CMDPARSER_USES_GGTT(dev_priv)) {
		flags = PIN_GLOBAL;
		vm = &dev_priv->ggtt.base;
	} else if (vm->has_read_only) {
		flags = PIN_USER;
		i915_gem_object_set_readonly(obj);
	} else {
		DRM_DEBUG("Cannot prevent post-scan tampering without RO capable vm\n");
		return ERR_PTR(-EINVAL);
	}

	return i915_gem_object_pin(obj, vm, NULL, 0, 0, flags);
}

static struct i915_vma *
i915_gem_execbuffer_parse(struct intel_engine_cs *engine,
			  struct drm_i915_gem_exec_object2 *shadow_exec_entry,
			  struct i915_execbuffer_params *params,
			  struct eb_vmas *eb,
			  struct i915_address_space *vm)
{
	struct drm_i915_gem_object *batch_obj = params->batch->obj;
	struct drm_i915_gem_object *shadow_batch_obj;
	struct i915_vma *vma;
	u64 batch_start;
	u32 batch_start_offset = params->args_batch_start_offset;
	u32 batch_len = params->args_batch_len;
	u64 shadow_batch_start;
	int ret;


	shadow_batch_obj = i915_gem_batch_pool_get(&engine->batch_pool,
						   PAGE_ALIGN(batch_len));
	if (IS_ERR(shadow_batch_obj))
		return ERR_CAST(shadow_batch_obj);

	vma = shadow_batch_pin(shadow_batch_obj, vm);
	if (IS_ERR(vma))
		goto out;

	batch_start = gen8_canonical_addr(params->batch->node.start) +
		      batch_start_offset;
	shadow_batch_start = gen8_canonical_addr(vma->node.start);

	ret = intel_engine_cmd_parser(params->ctx,
				      engine,
				      batch_obj,
				      batch_start,
				      batch_start_offset,
				      batch_len,
				      shadow_batch_obj,
				      shadow_batch_start);
	if (ret) {
		i915_vma_unpin(vma);

		/*
		 * Unsafe GGTT-backed buffers can still be submitted safely
		 * as non-secure.
		 * For PPGTT backing however, we have no choice but to forcibly
		 * reject unsafe buffers
		 */
		if (CMDPARSER_USES_GGTT(eb->i915) && (ret == -EACCES))
			/* Execute original buffer non-secure */
			vma = NULL;
		else
			vma = ERR_PTR(ret);

		goto out;
	}

	memset(shadow_exec_entry, 0, sizeof(*shadow_exec_entry));

	vma->exec_entry = shadow_exec_entry;
	vma->exec_entry->flags = __EXEC_OBJECT_HAS_PIN;
	i915_gem_object_get(shadow_batch_obj);
	list_add_tail(&vma->exec_list, &eb->vmas);

out:
	i915_gem_object_unpin_pages(shadow_batch_obj);
	return vma;
}

static int
execbuf_submit(struct i915_execbuffer_params *params,
	       struct drm_i915_gem_execbuffer2 *args,
	       struct list_head *vmas)
{
	u64 exec_start, exec_len;
>>>>>>> v4.9.227
	int ret;

	ret = i915_gem_execbuffer_move_to_gpu(params->request, vmas);
	if (ret)
		return ret;

	ret = i915_switch_context(params->request);
	if (ret)
		return ret;

<<<<<<< HEAD
	WARN(params->ctx->ppgtt && params->ctx->ppgtt->pd_dirty_rings & (1<<ring->id),
	     "%s didn't clear reload\n", ring->name);

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
			if (INTEL_INFO(dev)->gen < 4) {
				DRM_DEBUG("no rel constants on pre-gen4\n");
				return -EINVAL;
			}

			if (INTEL_INFO(dev)->gen > 5 &&
			    instp_mode == I915_EXEC_CONSTANTS_REL_SURFACE) {
				DRM_DEBUG("rel surface constants mode invalid on gen5+\n");
				return -EINVAL;
			}

			/* The HW changed the meaning on this bit on gen6 */
			if (INTEL_INFO(dev)->gen >= 6)
				instp_mask &= ~I915_EXEC_CONSTANTS_REL_SURFACE;
		}
		break;
	default:
		DRM_DEBUG("execbuf with unknown constants: %d\n", instp_mode);
		return -EINVAL;
	}

	if (ring == &dev_priv->ring[RCS] &&
	    instp_mode != dev_priv->relative_constants_mode) {
		ret = intel_ring_begin(params->request, 4);
		if (ret)
			return ret;

		intel_ring_emit(ring, MI_NOOP);
		intel_ring_emit(ring, MI_LOAD_REGISTER_IMM(1));
		intel_ring_emit(ring, INSTPM);
		intel_ring_emit(ring, instp_mask << 16 | instp_mode);
		intel_ring_advance(ring);

		dev_priv->relative_constants_mode = instp_mode;
	}

	if (args->flags & I915_EXEC_GEN7_SOL_RESET) {
		ret = i915_reset_gen7_sol_offsets(dev, params->request);
=======
	if (args->flags & I915_EXEC_CONSTANTS_MASK) {
		DRM_DEBUG("I915_EXEC_CONSTANTS_* unsupported\n");
		return -EINVAL;
	}

	if (args->flags & I915_EXEC_GEN7_SOL_RESET) {
		ret = i915_reset_gen7_sol_offsets(params->request);
>>>>>>> v4.9.227
		if (ret)
			return ret;
	}

<<<<<<< HEAD
	exec_len   = args->batch_len;
	exec_start = params->batch_obj_vm_offset +
		     params->args_batch_start_offset;

	ret = ring->dispatch_execbuffer(params->request,
					exec_start, exec_len,
					params->dispatch_flags);
=======
	exec_len   = params->args_batch_len;
	exec_start = params->batch->node.start +
		     params->args_batch_start_offset;

	ret = params->engine->emit_bb_start(params->request,
					    exec_start, exec_len,
					    params->dispatch_flags);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	trace_i915_gem_ring_dispatch(params->request, params->dispatch_flags);

	i915_gem_execbuffer_move_to_active(vmas, params->request);
<<<<<<< HEAD
	i915_gem_execbuffer_retire_commands(params);
=======
>>>>>>> v4.9.227

	return 0;
}

/**
 * Find one BSD ring to dispatch the corresponding BSD command.
<<<<<<< HEAD
 * The Ring ID is returned.
 */
static int gen8_dispatch_bsd_ring(struct drm_device *dev,
				  struct drm_file *file)
{
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct drm_i915_file_private *file_priv = file->driver_priv;

	/* Check whether the file_priv is using one ring */
	if (file_priv->bsd_ring)
		return file_priv->bsd_ring->id;
	else {
		/* If no, use the ping-pong mechanism to select one ring */
		int ring_id;

		mutex_lock(&dev->struct_mutex);
		if (dev_priv->mm.bsd_ring_dispatch_index == 0) {
			ring_id = VCS;
			dev_priv->mm.bsd_ring_dispatch_index = 1;
		} else {
			ring_id = VCS2;
			dev_priv->mm.bsd_ring_dispatch_index = 0;
		}
		file_priv->bsd_ring = &dev_priv->ring[ring_id];
		mutex_unlock(&dev->struct_mutex);
		return ring_id;
	}
}

static struct drm_i915_gem_object *
eb_get_batch(struct eb_vmas *eb)
{
	struct i915_vma *vma = list_entry(eb->vmas.prev, typeof(*vma), exec_list);

	/*
	 * SNA is doing fancy tricks with compressing batch buffers, which leads
	 * to negative relocation deltas. Usually that works out ok since the
	 * relocate address is still positive, except when the batch is placed
	 * very low in the GTT. Ensure this doesn't happen.
	 *
	 * Note that actual hangs have only been observed on gen7, but for
	 * paranoia do it everywhere.
	 */
	vma->exec_entry->flags |= __EXEC_OBJECT_NEEDS_BIAS;

	return vma->obj;
=======
 * The engine index is returned.
 */
static unsigned int
gen8_dispatch_bsd_engine(struct drm_i915_private *dev_priv,
			 struct drm_file *file)
{
	struct drm_i915_file_private *file_priv = file->driver_priv;

	/* Check whether the file_priv has already selected one ring. */
	if ((int)file_priv->bsd_engine < 0)
		file_priv->bsd_engine = atomic_fetch_xor(1,
			 &dev_priv->mm.bsd_engine_dispatch_index);

	return file_priv->bsd_engine;
}

#define I915_USER_RINGS (4)

static const enum intel_engine_id user_ring_map[I915_USER_RINGS + 1] = {
	[I915_EXEC_DEFAULT]	= RCS,
	[I915_EXEC_RENDER]	= RCS,
	[I915_EXEC_BLT]		= BCS,
	[I915_EXEC_BSD]		= VCS,
	[I915_EXEC_VEBOX]	= VECS
};

static struct intel_engine_cs *
eb_select_engine(struct drm_i915_private *dev_priv,
		 struct drm_file *file,
		 struct drm_i915_gem_execbuffer2 *args)
{
	unsigned int user_ring_id = args->flags & I915_EXEC_RING_MASK;
	struct intel_engine_cs *engine;

	if (user_ring_id > I915_USER_RINGS) {
		DRM_DEBUG("execbuf with unknown ring: %u\n", user_ring_id);
		return NULL;
	}

	if ((user_ring_id != I915_EXEC_BSD) &&
	    ((args->flags & I915_EXEC_BSD_MASK) != 0)) {
		DRM_DEBUG("execbuf with non bsd ring but with invalid "
			  "bsd dispatch flags: %d\n", (int)(args->flags));
		return NULL;
	}

	if (user_ring_id == I915_EXEC_BSD && HAS_BSD2(dev_priv)) {
		unsigned int bsd_idx = args->flags & I915_EXEC_BSD_MASK;

		if (bsd_idx == I915_EXEC_BSD_DEFAULT) {
			bsd_idx = gen8_dispatch_bsd_engine(dev_priv, file);
		} else if (bsd_idx >= I915_EXEC_BSD_RING1 &&
			   bsd_idx <= I915_EXEC_BSD_RING2) {
			bsd_idx >>= I915_EXEC_BSD_SHIFT;
			bsd_idx--;
		} else {
			DRM_DEBUG("execbuf with unknown bsd ring: %u\n",
				  bsd_idx);
			return NULL;
		}

		engine = &dev_priv->engine[_VCS(bsd_idx)];
	} else {
		engine = &dev_priv->engine[user_ring_map[user_ring_id]];
	}

	if (!intel_engine_initialized(engine)) {
		DRM_DEBUG("execbuf with invalid ring: %u\n", user_ring_id);
		return NULL;
	}

	return engine;
>>>>>>> v4.9.227
}

static int
i915_gem_do_execbuffer(struct drm_device *dev, void *data,
		       struct drm_file *file,
		       struct drm_i915_gem_execbuffer2 *args,
		       struct drm_i915_gem_exec_object2 *exec)
{
<<<<<<< HEAD
	struct drm_i915_private *dev_priv = dev->dev_private;
	struct eb_vmas *eb;
	struct drm_i915_gem_object *batch_obj;
	struct drm_i915_gem_exec_object2 shadow_exec_entry;
	struct intel_engine_cs *ring;
	struct intel_context *ctx;
=======
	struct drm_i915_private *dev_priv = to_i915(dev);
	struct i915_ggtt *ggtt = &dev_priv->ggtt;
	struct eb_vmas *eb;
	struct drm_i915_gem_exec_object2 shadow_exec_entry;
	struct intel_engine_cs *engine;
	struct i915_gem_context *ctx;
>>>>>>> v4.9.227
	struct i915_address_space *vm;
	struct i915_execbuffer_params params_master; /* XXX: will be removed later */
	struct i915_execbuffer_params *params = &params_master;
	const u32 ctx_id = i915_execbuffer2_get_context_id(*args);
	u32 dispatch_flags;
	int ret;
	bool need_relocs;

	if (!i915_gem_check_execbuffer(args))
		return -EINVAL;

	ret = validate_exec_list(dev, exec, args->buffer_count);
	if (ret)
		return ret;

	dispatch_flags = 0;
	if (args->flags & I915_EXEC_SECURE) {
<<<<<<< HEAD
		if (!file->is_master || !capable(CAP_SYS_ADMIN))
		    return -EPERM;
=======
		if (INTEL_GEN(dev_priv) >= 11)
			return -ENODEV;

		/* Return -EPERM to trigger fallback code on old binaries. */
		if (!HAS_SECURE_BATCHES(dev_priv))
			return -EPERM;

		if (!drm_is_current_master(file) || !capable(CAP_SYS_ADMIN))
			return -EPERM;
>>>>>>> v4.9.227

		dispatch_flags |= I915_DISPATCH_SECURE;
	}
	if (args->flags & I915_EXEC_IS_PINNED)
		dispatch_flags |= I915_DISPATCH_PINNED;

<<<<<<< HEAD
	if ((args->flags & I915_EXEC_RING_MASK) > LAST_USER_RING) {
		DRM_DEBUG("execbuf with unknown ring: %d\n",
			  (int)(args->flags & I915_EXEC_RING_MASK));
		return -EINVAL;
	}

	if (((args->flags & I915_EXEC_RING_MASK) != I915_EXEC_BSD) &&
	    ((args->flags & I915_EXEC_BSD_MASK) != 0)) {
		DRM_DEBUG("execbuf with non bsd ring but with invalid "
			"bsd dispatch flags: %d\n", (int)(args->flags));
		return -EINVAL;
	} 

	if ((args->flags & I915_EXEC_RING_MASK) == I915_EXEC_DEFAULT)
		ring = &dev_priv->ring[RCS];
	else if ((args->flags & I915_EXEC_RING_MASK) == I915_EXEC_BSD) {
		if (HAS_BSD2(dev)) {
			int ring_id;

			switch (args->flags & I915_EXEC_BSD_MASK) {
			case I915_EXEC_BSD_DEFAULT:
				ring_id = gen8_dispatch_bsd_ring(dev, file);
				ring = &dev_priv->ring[ring_id];
				break;
			case I915_EXEC_BSD_RING1:
				ring = &dev_priv->ring[VCS];
				break;
			case I915_EXEC_BSD_RING2:
				ring = &dev_priv->ring[VCS2];
				break;
			default:
				DRM_DEBUG("execbuf with unknown bsd ring: %d\n",
					  (int)(args->flags & I915_EXEC_BSD_MASK));
				return -EINVAL;
			}
		} else
			ring = &dev_priv->ring[VCS];
	} else
		ring = &dev_priv->ring[(args->flags & I915_EXEC_RING_MASK) - 1];

	if (!intel_ring_initialized(ring)) {
		DRM_DEBUG("execbuf with invalid ring: %d\n",
			  (int)(args->flags & I915_EXEC_RING_MASK));
		return -EINVAL;
	}
=======
	engine = eb_select_engine(dev_priv, file, args);
	if (!engine)
		return -EINVAL;
>>>>>>> v4.9.227

	if (args->buffer_count < 1) {
		DRM_DEBUG("execbuf with %d buffers\n", args->buffer_count);
		return -EINVAL;
	}

	if (args->flags & I915_EXEC_RESOURCE_STREAMER) {
		if (!HAS_RESOURCE_STREAMER(dev)) {
			DRM_DEBUG("RS is only allowed for Haswell, Gen8 and above\n");
			return -EINVAL;
		}
<<<<<<< HEAD
		if (ring->id != RCS) {
			DRM_DEBUG("RS is not available on %s\n",
				 ring->name);
=======
		if (engine->id != RCS) {
			DRM_DEBUG("RS is not available on %s\n",
				 engine->name);
>>>>>>> v4.9.227
			return -EINVAL;
		}

		dispatch_flags |= I915_DISPATCH_RS;
	}

<<<<<<< HEAD
=======
	/* Take a local wakeref for preparing to dispatch the execbuf as
	 * we expect to access the hardware fairly frequently in the
	 * process. Upon first dispatch, we acquire another prolonged
	 * wakeref that we hold until the GPU has been idle for at least
	 * 100ms.
	 */
>>>>>>> v4.9.227
	intel_runtime_pm_get(dev_priv);

	ret = i915_mutex_lock_interruptible(dev);
	if (ret)
		goto pre_mutex_err;

<<<<<<< HEAD
	ctx = i915_gem_validate_context(dev, file, ring, ctx_id);
=======
	ctx = i915_gem_validate_context(dev, file, engine, ctx_id);
>>>>>>> v4.9.227
	if (IS_ERR(ctx)) {
		mutex_unlock(&dev->struct_mutex);
		ret = PTR_ERR(ctx);
		goto pre_mutex_err;
	}

<<<<<<< HEAD
	i915_gem_context_reference(ctx);
=======
	i915_gem_context_get(ctx);
>>>>>>> v4.9.227

	if (ctx->ppgtt)
		vm = &ctx->ppgtt->base;
	else
<<<<<<< HEAD
		vm = &dev_priv->gtt.base;

	memset(&params_master, 0x00, sizeof(params_master));

	eb = eb_create(args);
	if (eb == NULL) {
		i915_gem_context_unreference(ctx);
=======
		vm = &ggtt->base;

	memset(&params_master, 0x00, sizeof(params_master));

	eb = eb_create(dev_priv, args);
	if (eb == NULL) {
		i915_gem_context_put(ctx);
>>>>>>> v4.9.227
		mutex_unlock(&dev->struct_mutex);
		ret = -ENOMEM;
		goto pre_mutex_err;
	}

	/* Look up object handles */
	ret = eb_lookup_vmas(eb, exec, args, vm, file);
	if (ret)
		goto err;

	/* take note of the batch buffer before we might reorder the lists */
<<<<<<< HEAD
	batch_obj = eb_get_batch(eb);

	/* Move the objects en-masse into the GTT, evicting if necessary. */
	need_relocs = (args->flags & I915_EXEC_NO_RELOC) == 0;
	ret = i915_gem_execbuffer_reserve(ring, &eb->vmas, ctx, &need_relocs);
=======
	params->batch = eb_get_batch(eb);

	/* Move the objects en-masse into the GTT, evicting if necessary. */
	need_relocs = (args->flags & I915_EXEC_NO_RELOC) == 0;
	ret = i915_gem_execbuffer_reserve(engine, &eb->vmas, ctx,
					  &need_relocs);
>>>>>>> v4.9.227
	if (ret)
		goto err;

	/* The objects are in their final locations, apply the relocations. */
	if (need_relocs)
		ret = i915_gem_execbuffer_relocate(eb);
	if (ret) {
		if (ret == -EFAULT) {
<<<<<<< HEAD
			ret = i915_gem_execbuffer_relocate_slow(dev, args, file, ring,
=======
			ret = i915_gem_execbuffer_relocate_slow(dev, args, file,
								engine,
>>>>>>> v4.9.227
								eb, exec, ctx);
			BUG_ON(!mutex_is_locked(&dev->struct_mutex));
		}
		if (ret)
			goto err;
	}

	/* Set the pending read domains for the batch buffer to COMMAND */
<<<<<<< HEAD
	if (batch_obj->base.pending_write_domain) {
=======
	if (params->batch->obj->base.pending_write_domain) {
>>>>>>> v4.9.227
		DRM_DEBUG("Attempting to use self-modifying batch buffer\n");
		ret = -EINVAL;
		goto err;
	}
<<<<<<< HEAD

	params->args_batch_start_offset = args->batch_start_offset;
	if (i915_needs_cmd_parser(ring) && args->batch_len) {
		struct drm_i915_gem_object *parsed_batch_obj;

		parsed_batch_obj = i915_gem_execbuffer_parse(ring,
						      &shadow_exec_entry,
						      eb,
						      batch_obj,
						      args->batch_start_offset,
						      args->batch_len,
						      file->is_master);
		if (IS_ERR(parsed_batch_obj)) {
			ret = PTR_ERR(parsed_batch_obj);
			goto err;
		}

		/*
		 * parsed_batch_obj == batch_obj means batch not fully parsed:
		 * Accept, but don't promote to secure.
		 */

		if (parsed_batch_obj != batch_obj) {
			/*
			 * Batch parsed and accepted:
			 *
			 * Set the DISPATCH_SECURE bit to remove the NON_SECURE
			 * bit from MI_BATCH_BUFFER_START commands issued in
			 * the dispatch_execbuffer implementations. We
			 * specifically don't want that set on batches the
			 * command parser has accepted.
			 */
			dispatch_flags |= I915_DISPATCH_SECURE;
			params->args_batch_start_offset = 0;
			batch_obj = parsed_batch_obj;
		}
	}

	batch_obj->base.pending_read_domains |= I915_GEM_DOMAIN_COMMAND;
=======
	if (args->batch_start_offset > params->batch->size ||
	    args->batch_len > params->batch->size - args->batch_start_offset) {
		DRM_DEBUG("Attempting to use out-of-bounds batch\n");
		ret = -EINVAL;
		goto err;
	}

	params->ctx = ctx;
	params->args_batch_start_offset = args->batch_start_offset;
	params->args_batch_len = args->batch_len;
	if (args->batch_len == 0)
		params->args_batch_len = params->batch->size - params->args_batch_start_offset;

	if (intel_engine_requires_cmd_parser(engine) ||
	    (intel_engine_using_cmd_parser(engine) && args->batch_len)) {
		struct i915_vma *vma;

		vma = i915_gem_execbuffer_parse(engine, &shadow_exec_entry,
						params, eb, vm);
		if (IS_ERR(vma)) {
			ret = PTR_ERR(vma);
			goto err;
		}

		if (vma) {
			if (CMDPARSER_USES_GGTT(dev_priv))
				dispatch_flags |= I915_DISPATCH_SECURE;
			params->args_batch_start_offset = 0;
			params->batch = vma;
		}
	}

	params->batch->obj->base.pending_read_domains |= I915_GEM_DOMAIN_COMMAND;
>>>>>>> v4.9.227

	/* snb/ivb/vlv conflate the "batch in ppgtt" bit with the "non-secure
	 * batch" bit. Hence we need to pin secure batches into the global gtt.
	 * hsw should have this fixed, but bdw mucks it up again. */
	if (dispatch_flags & I915_DISPATCH_SECURE) {
<<<<<<< HEAD
=======
		struct drm_i915_gem_object *obj = params->batch->obj;
		struct i915_vma *vma;

>>>>>>> v4.9.227
		/*
		 * So on first glance it looks freaky that we pin the batch here
		 * outside of the reservation loop. But:
		 * - The batch is already pinned into the relevant ppgtt, so we
		 *   already have the backing storage fully allocated.
		 * - No other BO uses the global gtt (well contexts, but meh),
		 *   so we don't really have issues with multiple objects not
		 *   fitting due to fragmentation.
		 * So this is actually safe.
		 */
<<<<<<< HEAD
		ret = i915_gem_obj_ggtt_pin(batch_obj, 0, 0);
		if (ret)
			goto err;

		params->batch_obj_vm_offset = i915_gem_obj_ggtt_offset(batch_obj);
	} else
		params->batch_obj_vm_offset = i915_gem_obj_offset(batch_obj, vm);

	/* Allocate a request for this batch buffer nice and early. */
	ret = i915_gem_request_alloc(ring, ctx, &params->request);
	if (ret)
		goto err_batch_unpin;

	ret = i915_gem_request_add_to_client(params->request, file);
	if (ret)
		goto err_batch_unpin;
=======
		vma = i915_gem_object_ggtt_pin(obj, NULL, 0, 0, 0);
		if (IS_ERR(vma)) {
			ret = PTR_ERR(vma);
			goto err;
		}

		params->batch = vma;
	}

	/* Allocate a request for this batch buffer nice and early. */
	params->request = i915_gem_request_alloc(engine, ctx);
	if (IS_ERR(params->request)) {
		ret = PTR_ERR(params->request);
		goto err_batch_unpin;
	}

	/* Whilst this request exists, batch_obj will be on the
	 * active_list, and so will hold the active reference. Only when this
	 * request is retired will the the batch_obj be moved onto the
	 * inactive_list and lose its active reference. Hence we do not need
	 * to explicitly hold another reference here.
	 */
	params->request->batch = params->batch;

	ret = i915_gem_request_add_to_client(params->request, file);
	if (ret)
		goto err_request;
>>>>>>> v4.9.227

	/*
	 * Save assorted stuff away to pass through to *_submission().
	 * NB: This data should be 'persistent' and not local as it will
	 * kept around beyond the duration of the IOCTL once the GPU
	 * scheduler arrives.
	 */
	params->dev                     = dev;
	params->file                    = file;
<<<<<<< HEAD
	params->ring                    = ring;
	params->dispatch_flags          = dispatch_flags;
	params->batch_obj               = batch_obj;
	params->ctx                     = ctx;

	ret = dev_priv->gt.execbuf_submit(params, args, &eb->vmas);
=======
	params->engine                    = engine;
	params->dispatch_flags          = dispatch_flags;

	ret = execbuf_submit(params, args, &eb->vmas);
err_request:
	__i915_add_request(params->request, ret == 0);
>>>>>>> v4.9.227

err_batch_unpin:
	/*
	 * FIXME: We crucially rely upon the active tracking for the (ppgtt)
	 * batch vma for correctness. For less ugly and less fragility this
	 * needs to be adjusted to also track the ggtt batch vma properly as
	 * active.
	 */
	if (dispatch_flags & I915_DISPATCH_SECURE)
<<<<<<< HEAD
		i915_gem_object_ggtt_unpin(batch_obj);

err:
	/* the request owns the ref now */
	i915_gem_context_unreference(ctx);
	eb_destroy(eb);

	/*
	 * If the request was created but not successfully submitted then it
	 * must be freed again. If it was submitted then it is being tracked
	 * on the active request list and no clean up is required here.
	 */
	if (ret && params->request)
		i915_gem_request_cancel(params->request);

=======
		i915_vma_unpin(params->batch);
err:
	/* the request owns the ref now */
	i915_gem_context_put(ctx);
	eb_destroy(eb);

>>>>>>> v4.9.227
	mutex_unlock(&dev->struct_mutex);

pre_mutex_err:
	/* intel_gpu_busy should also get a ref, so it will free when the device
	 * is really idle. */
	intel_runtime_pm_put(dev_priv);
	return ret;
}

/*
 * Legacy execbuffer just creates an exec2 list from the original exec object
 * list array and passes it to the real function.
 */
int
i915_gem_execbuffer(struct drm_device *dev, void *data,
		    struct drm_file *file)
{
	struct drm_i915_gem_execbuffer *args = data;
	struct drm_i915_gem_execbuffer2 exec2;
	struct drm_i915_gem_exec_object *exec_list = NULL;
	struct drm_i915_gem_exec_object2 *exec2_list = NULL;
	int ret, i;

	if (args->buffer_count < 1) {
		DRM_DEBUG("execbuf with %d buffers\n", args->buffer_count);
		return -EINVAL;
	}

	/* Copy in the exec list from userland */
	exec_list = drm_malloc_ab(sizeof(*exec_list), args->buffer_count);
	exec2_list = drm_malloc_ab(sizeof(*exec2_list), args->buffer_count);
	if (exec_list == NULL || exec2_list == NULL) {
		DRM_DEBUG("Failed to allocate exec list for %d buffers\n",
			  args->buffer_count);
		drm_free_large(exec_list);
		drm_free_large(exec2_list);
		return -ENOMEM;
	}
	ret = copy_from_user(exec_list,
<<<<<<< HEAD
			     to_user_ptr(args->buffers_ptr),
=======
			     u64_to_user_ptr(args->buffers_ptr),
>>>>>>> v4.9.227
			     sizeof(*exec_list) * args->buffer_count);
	if (ret != 0) {
		DRM_DEBUG("copy %d exec entries failed %d\n",
			  args->buffer_count, ret);
		drm_free_large(exec_list);
		drm_free_large(exec2_list);
		return -EFAULT;
	}

	for (i = 0; i < args->buffer_count; i++) {
		exec2_list[i].handle = exec_list[i].handle;
		exec2_list[i].relocation_count = exec_list[i].relocation_count;
		exec2_list[i].relocs_ptr = exec_list[i].relocs_ptr;
		exec2_list[i].alignment = exec_list[i].alignment;
		exec2_list[i].offset = exec_list[i].offset;
		if (INTEL_INFO(dev)->gen < 4)
			exec2_list[i].flags = EXEC_OBJECT_NEEDS_FENCE;
		else
			exec2_list[i].flags = 0;
	}

	exec2.buffers_ptr = args->buffers_ptr;
	exec2.buffer_count = args->buffer_count;
	exec2.batch_start_offset = args->batch_start_offset;
	exec2.batch_len = args->batch_len;
	exec2.DR1 = args->DR1;
	exec2.DR4 = args->DR4;
	exec2.num_cliprects = args->num_cliprects;
	exec2.cliprects_ptr = args->cliprects_ptr;
	exec2.flags = I915_EXEC_RENDER;
	i915_execbuffer2_set_context_id(exec2, 0);

	ret = i915_gem_do_execbuffer(dev, data, file, &exec2, exec2_list);
	if (!ret) {
		struct drm_i915_gem_exec_object __user *user_exec_list =
<<<<<<< HEAD
			to_user_ptr(args->buffers_ptr);

		/* Copy the new buffer offsets back to the user's exec list. */
		for (i = 0; i < args->buffer_count; i++) {
=======
			u64_to_user_ptr(args->buffers_ptr);

		/* Copy the new buffer offsets back to the user's exec list. */
		for (i = 0; i < args->buffer_count; i++) {
			exec2_list[i].offset =
				gen8_canonical_addr(exec2_list[i].offset);
>>>>>>> v4.9.227
			ret = __copy_to_user(&user_exec_list[i].offset,
					     &exec2_list[i].offset,
					     sizeof(user_exec_list[i].offset));
			if (ret) {
				ret = -EFAULT;
				DRM_DEBUG("failed to copy %d exec entries "
					  "back to user (%d)\n",
					  args->buffer_count, ret);
				break;
			}
		}
	}

	drm_free_large(exec_list);
	drm_free_large(exec2_list);
	return ret;
}

int
i915_gem_execbuffer2(struct drm_device *dev, void *data,
		     struct drm_file *file)
{
	struct drm_i915_gem_execbuffer2 *args = data;
	struct drm_i915_gem_exec_object2 *exec2_list = NULL;
	int ret;

	if (args->buffer_count < 1 ||
	    args->buffer_count > UINT_MAX / sizeof(*exec2_list)) {
		DRM_DEBUG("execbuf2 with %d buffers\n", args->buffer_count);
		return -EINVAL;
	}

	if (args->rsvd2 != 0) {
		DRM_DEBUG("dirty rvsd2 field\n");
		return -EINVAL;
	}

<<<<<<< HEAD
	exec2_list = kmalloc(sizeof(*exec2_list)*args->buffer_count,
			     GFP_TEMPORARY | __GFP_NOWARN | __GFP_NORETRY);
	if (exec2_list == NULL)
		exec2_list = drm_malloc_ab(sizeof(*exec2_list),
					   args->buffer_count);
=======
	exec2_list = drm_malloc_gfp(args->buffer_count,
				    sizeof(*exec2_list),
				    GFP_TEMPORARY);
>>>>>>> v4.9.227
	if (exec2_list == NULL) {
		DRM_DEBUG("Failed to allocate exec list for %d buffers\n",
			  args->buffer_count);
		return -ENOMEM;
	}
	ret = copy_from_user(exec2_list,
<<<<<<< HEAD
			     to_user_ptr(args->buffers_ptr),
=======
			     u64_to_user_ptr(args->buffers_ptr),
>>>>>>> v4.9.227
			     sizeof(*exec2_list) * args->buffer_count);
	if (ret != 0) {
		DRM_DEBUG("copy %d exec entries failed %d\n",
			  args->buffer_count, ret);
		drm_free_large(exec2_list);
		return -EFAULT;
	}

	ret = i915_gem_do_execbuffer(dev, data, file, args, exec2_list);
	if (!ret) {
		/* Copy the new buffer offsets back to the user's exec list. */
		struct drm_i915_gem_exec_object2 __user *user_exec_list =
<<<<<<< HEAD
				   to_user_ptr(args->buffers_ptr);
		int i;

		for (i = 0; i < args->buffer_count; i++) {
=======
				   u64_to_user_ptr(args->buffers_ptr);
		int i;

		for (i = 0; i < args->buffer_count; i++) {
			exec2_list[i].offset =
				gen8_canonical_addr(exec2_list[i].offset);
>>>>>>> v4.9.227
			ret = __copy_to_user(&user_exec_list[i].offset,
					     &exec2_list[i].offset,
					     sizeof(user_exec_list[i].offset));
			if (ret) {
				ret = -EFAULT;
				DRM_DEBUG("failed to copy %d exec entries "
					  "back to user\n",
					  args->buffer_count);
				break;
			}
		}
	}

	drm_free_large(exec2_list);
	return ret;
}
