/*
 * Copyright (C) 2013 Red Hat
 * Author: Rob Clark <robdclark@gmail.com>
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

#include <linux/spinlock.h>
#include <linux/shmem_fs.h>
#include <linux/dma-buf.h>
<<<<<<< HEAD

#include "msm_drv.h"
=======
#include <linux/pfn_t.h>

#include "msm_drv.h"
#include "msm_fence.h"
>>>>>>> v4.9.227
#include "msm_gem.h"
#include "msm_gpu.h"
#include "msm_mmu.h"

static dma_addr_t physaddr(struct drm_gem_object *obj)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	struct msm_drm_private *priv = obj->dev->dev_private;
	return (((dma_addr_t)msm_obj->vram_node->start) << PAGE_SHIFT) +
			priv->vram.paddr;
}

static bool use_pages(struct drm_gem_object *obj)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	return !msm_obj->vram_node;
}

<<<<<<< HEAD
=======
/*
 * Cache sync.. this is a bit over-complicated, to fit dma-mapping
 * API.  Really GPU cache is out of scope here (handled on cmdstream)
 * and all we need to do is invalidate newly allocated pages before
 * mapping to CPU as uncached/writecombine.
 *
 * On top of this, we have the added headache, that depending on
 * display generation, the display's iommu may be wired up to either
 * the toplevel drm device (mdss), or to the mdp sub-node, meaning
 * that here we either have dma-direct or iommu ops.
 *
 * Let this be a cautionary tail of abstraction gone wrong.
 */

static void sync_for_device(struct msm_gem_object *msm_obj)
{
	struct device *dev = msm_obj->base.dev->dev;

	if (get_dma_ops(dev) && IS_ENABLED(CONFIG_ARM64)) {
		dma_sync_sg_for_device(dev, msm_obj->sgt->sgl,
			msm_obj->sgt->nents, DMA_BIDIRECTIONAL);
	} else {
		dma_map_sg(dev, msm_obj->sgt->sgl,
			msm_obj->sgt->nents, DMA_BIDIRECTIONAL);
	}
}

static void sync_for_cpu(struct msm_gem_object *msm_obj)
{
	struct device *dev = msm_obj->base.dev->dev;

	if (get_dma_ops(dev) && IS_ENABLED(CONFIG_ARM64)) {
		dma_sync_sg_for_cpu(dev, msm_obj->sgt->sgl,
			msm_obj->sgt->nents, DMA_BIDIRECTIONAL);
	} else {
		dma_unmap_sg(dev, msm_obj->sgt->sgl,
			msm_obj->sgt->nents, DMA_BIDIRECTIONAL);
	}
}

>>>>>>> v4.9.227
/* allocate pages from VRAM carveout, used when no IOMMU: */
static struct page **get_pages_vram(struct drm_gem_object *obj,
		int npages)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	struct msm_drm_private *priv = obj->dev->dev_private;
	dma_addr_t paddr;
	struct page **p;
	int ret, i;

	p = drm_malloc_ab(npages, sizeof(struct page *));
	if (!p)
		return ERR_PTR(-ENOMEM);

	ret = drm_mm_insert_node(&priv->vram.mm, msm_obj->vram_node,
			npages, 0, DRM_MM_SEARCH_DEFAULT);
	if (ret) {
		drm_free_large(p);
		return ERR_PTR(ret);
	}

	paddr = physaddr(obj);
	for (i = 0; i < npages; i++) {
		p[i] = phys_to_page(paddr);
		paddr += PAGE_SIZE;
	}

	return p;
}

<<<<<<< HEAD
static int msm_drm_alloc_buf(struct drm_gem_object *obj)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	enum dma_attr attr;
	unsigned int nr_pages;
	struct page **p = NULL;
	struct drm_device *dev = obj->dev;
	struct msm_gem_buf *buf;

	if (msm_obj->buf)
		return 0;

	buf = kzalloc(sizeof(*buf), GFP_KERNEL);
	if (!buf) {
		DRM_ERROR("%s: kzalloc failed\n", __func__);
		return -ENOMEM;
	}

	init_dma_attrs(&buf->dma_attrs);

	if (msm_obj->flags & MSM_BO_CONTIGUOUS)
		dma_set_attr(DMA_ATTR_FORCE_CONTIGUOUS, &buf->dma_attrs);

	if (msm_obj->flags & (MSM_BO_UNCACHED | MSM_BO_WC))
		attr = DMA_ATTR_WRITE_COMBINE;
	else
		attr = DMA_ATTR_NON_CONSISTENT;

	dma_set_attr(attr, &buf->dma_attrs);
	dma_set_attr(DMA_ATTR_NO_KERNEL_MAPPING, &buf->dma_attrs);

	nr_pages = obj->size >> PAGE_SHIFT;

	p = dma_alloc_attrs(dev->dev, obj->size,
				&buf->dma_addr, GFP_KERNEL, &buf->dma_attrs);
	if (!p) {
		DRM_ERROR("failed to allocate buffer.\n");
		kfree(buf);
		return -ENOMEM;
	}

	msm_obj->buf = buf;
	msm_obj->pages = p;
	return 0;
}

static int msm_drm_free_buf(struct drm_gem_object *obj)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	struct msm_gem_buf *buf = msm_obj->buf;
	struct drm_device *dev = obj->dev;

	if (!buf)
		return 0;

	dma_free_attrs(dev->dev, obj->size, msm_obj->pages,
				(dma_addr_t)buf->dma_addr, &buf->dma_attrs);

	kfree(buf);
	msm_obj->buf = NULL;

	return 0;
}

int msm_drm_gem_mmap_buffer(struct drm_gem_object *obj,
				      struct vm_area_struct *vma)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	struct msm_gem_buf *buf = msm_obj->buf;
	struct drm_device *dev = obj->dev;
	unsigned long vm_size;
	int ret;

	vma->vm_flags &= ~VM_PFNMAP;
	vma->vm_pgoff = 0;

	vm_size = vma->vm_end - vma->vm_start;

	/* check if user-requested size is valid. */
	if (vm_size > obj->size)
		return -EINVAL;

	ret = dma_mmap_attrs(dev->dev, vma, msm_obj->pages,
				buf->dma_addr, obj->size,
				&buf->dma_attrs);
	if (ret < 0) {
		DRM_ERROR("failed to mmap.\n");
		return ret;
	}

	return 0;
}

=======
>>>>>>> v4.9.227
/* called with dev->struct_mutex held */
static struct page **get_pages(struct drm_gem_object *obj)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);

	if (!msm_obj->pages) {
<<<<<<< HEAD
		struct page **p;
		int npages = obj->size >> PAGE_SHIFT;

		if (use_pages(obj)) {
			if (!msm_drm_alloc_buf(obj))
				p = msm_obj->pages;
		} else {
			p = get_pages_vram(obj, npages);
		}

		if (IS_ERR(p)) {
			DRM_ERROR("could not get pages: %ld\n", PTR_ERR(p));
=======
		struct drm_device *dev = obj->dev;
		struct page **p;
		int npages = obj->size >> PAGE_SHIFT;

		if (use_pages(obj))
			p = drm_gem_get_pages(obj);
		else
			p = get_pages_vram(obj, npages);

		if (IS_ERR(p)) {
			dev_err(dev->dev, "could not get pages: %ld\n",
					PTR_ERR(p));
>>>>>>> v4.9.227
			return p;
		}

		msm_obj->pages = p;
<<<<<<< HEAD
=======

		msm_obj->sgt = drm_prime_pages_to_sg(p, npages);
		if (IS_ERR(msm_obj->sgt)) {
			void *ptr = ERR_CAST(msm_obj->sgt);

			dev_err(dev->dev, "failed to allocate sgt\n");
			msm_obj->sgt = NULL;
			return ptr;
		}

		/* For non-cached buffers, ensure the new pages are clean
		 * because display controller, GPU, etc. are not coherent:
		 */
		if (msm_obj->flags & (MSM_BO_WC|MSM_BO_UNCACHED))
			sync_for_device(msm_obj);
>>>>>>> v4.9.227
	}

	return msm_obj->pages;
}

static void put_pages(struct drm_gem_object *obj)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);

	if (msm_obj->pages) {
<<<<<<< HEAD
		if (use_pages(obj))
			msm_drm_free_buf(obj);
=======
		if (msm_obj->sgt) {
			/* For non-cached buffers, ensure the new
			 * pages are clean because display controller,
			 * GPU, etc. are not coherent:
			 */
			if (msm_obj->flags & (MSM_BO_WC|MSM_BO_UNCACHED))
				sync_for_cpu(msm_obj);

			sg_free_table(msm_obj->sgt);
			kfree(msm_obj->sgt);
		}

		if (use_pages(obj))
			drm_gem_put_pages(obj, msm_obj->pages, true, false);
>>>>>>> v4.9.227
		else {
			drm_mm_remove_node(msm_obj->vram_node);
			drm_free_large(msm_obj->pages);
		}

		msm_obj->pages = NULL;
	}
}

struct page **msm_gem_get_pages(struct drm_gem_object *obj)
{
	struct drm_device *dev = obj->dev;
	struct page **p;
	mutex_lock(&dev->struct_mutex);
	p = get_pages(obj);
	mutex_unlock(&dev->struct_mutex);
	return p;
}

void msm_gem_put_pages(struct drm_gem_object *obj)
{
	/* when we start tracking the pin count, then do something here */
}

int msm_gem_mmap_obj(struct drm_gem_object *obj,
		struct vm_area_struct *vma)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);

	vma->vm_flags &= ~VM_PFNMAP;
	vma->vm_flags |= VM_MIXEDMAP;

	if (msm_obj->flags & MSM_BO_WC) {
		vma->vm_page_prot = pgprot_writecombine(vm_get_page_prot(vma->vm_flags));
	} else if (msm_obj->flags & MSM_BO_UNCACHED) {
		vma->vm_page_prot = pgprot_noncached(vm_get_page_prot(vma->vm_flags));
	} else {
		/*
		 * Shunt off cached objs to shmem file so they have their own
		 * address_space (so unmap_mapping_range does what we want,
		 * in particular in the case of mmap'd dmabufs)
		 */
		fput(vma->vm_file);
		get_file(obj->filp);
		vma->vm_pgoff = 0;
		vma->vm_file  = obj->filp;

		vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);
	}

	return 0;
}

int msm_gem_mmap(struct file *filp, struct vm_area_struct *vma)
{
	int ret;

	ret = drm_gem_mmap(filp, vma);
	if (ret) {
		DBG("mmap failed: %d", ret);
		return ret;
	}

	return msm_gem_mmap_obj(vma->vm_private_data, vma);
}

int msm_gem_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct drm_gem_object *obj = vma->vm_private_data;
	struct drm_device *dev = obj->dev;
<<<<<<< HEAD
=======
	struct msm_drm_private *priv = dev->dev_private;
>>>>>>> v4.9.227
	struct page **pages;
	unsigned long pfn;
	pgoff_t pgoff;
	int ret;

<<<<<<< HEAD
=======
	/* This should only happen if userspace tries to pass a mmap'd
	 * but unfaulted gem bo vaddr into submit ioctl, triggering
	 * a page fault while struct_mutex is already held.  This is
	 * not a valid use-case so just bail.
	 */
	if (priv->struct_mutex_task == current)
		return VM_FAULT_SIGBUS;

>>>>>>> v4.9.227
	/* Make sure we don't parallel update on a fault, nor move or remove
	 * something from beneath our feet
	 */
	ret = mutex_lock_interruptible(&dev->struct_mutex);
	if (ret)
		goto out;

	/* make sure we have pages attached now */
	pages = get_pages(obj);
	if (IS_ERR(pages)) {
		ret = PTR_ERR(pages);
		goto out_unlock;
	}

	/* We don't use vmf->pgoff since that has the fake offset: */
	pgoff = ((unsigned long)vmf->virtual_address -
			vma->vm_start) >> PAGE_SHIFT;

	pfn = page_to_pfn(pages[pgoff]);

<<<<<<< HEAD
	VERB("Inserting %pK pfn %lx, pa %lx", vmf->virtual_address,
			pfn, pfn << PAGE_SHIFT);

	ret = vm_insert_mixed(vma, (unsigned long)vmf->virtual_address, pfn);
=======
	VERB("Inserting %p pfn %lx, pa %lx", vmf->virtual_address,
			pfn, pfn << PAGE_SHIFT);

	ret = vm_insert_mixed(vma, (unsigned long)vmf->virtual_address,
			__pfn_to_pfn_t(pfn, PFN_DEV));
>>>>>>> v4.9.227

out_unlock:
	mutex_unlock(&dev->struct_mutex);
out:
	switch (ret) {
	case -EAGAIN:
	case 0:
	case -ERESTARTSYS:
	case -EINTR:
	case -EBUSY:
		/*
		 * EBUSY is ok: this just means that another thread
		 * already did the job.
		 */
		return VM_FAULT_NOPAGE;
	case -ENOMEM:
		return VM_FAULT_OOM;
	default:
		return VM_FAULT_SIGBUS;
	}
}

/** get mmap offset */
static uint64_t mmap_offset(struct drm_gem_object *obj)
{
	struct drm_device *dev = obj->dev;
	int ret;

	WARN_ON(!mutex_is_locked(&dev->struct_mutex));

	/* Make it mmapable */
	ret = drm_gem_create_mmap_offset(obj);

	if (ret) {
<<<<<<< HEAD
		DRM_ERROR("could not allocate mmap offset\n");
=======
		dev_err(dev->dev, "could not allocate mmap offset\n");
>>>>>>> v4.9.227
		return 0;
	}

	return drm_vma_node_offset_addr(&obj->vma_node);
}

uint64_t msm_gem_mmap_offset(struct drm_gem_object *obj)
{
	uint64_t offset;
	mutex_lock(&obj->dev->struct_mutex);
	offset = mmap_offset(obj);
	mutex_unlock(&obj->dev->struct_mutex);
	return offset;
}

<<<<<<< HEAD
=======
static void
put_iova(struct drm_gem_object *obj)
{
	struct drm_device *dev = obj->dev;
	struct msm_drm_private *priv = obj->dev->dev_private;
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	int id;

	WARN_ON(!mutex_is_locked(&dev->struct_mutex));

	for (id = 0; id < ARRAY_SIZE(msm_obj->domain); id++) {
		struct msm_mmu *mmu = priv->mmus[id];
		if (mmu && msm_obj->domain[id].iova) {
			uint32_t offset = msm_obj->domain[id].iova;
			mmu->funcs->unmap(mmu, offset, msm_obj->sgt, obj->size);
			msm_obj->domain[id].iova = 0;
		}
	}
}

>>>>>>> v4.9.227
/* should be called under struct_mutex.. although it can be called
 * from atomic context without struct_mutex to acquire an extra
 * iova ref if you know one is already held.
 *
 * That means when I do eventually need to add support for unpinning
 * the refcnt counter needs to be atomic_t.
 */
int msm_gem_get_iova_locked(struct drm_gem_object *obj, int id,
		uint32_t *iova)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	int ret = 0;

	if (!msm_obj->domain[id].iova) {
		struct msm_drm_private *priv = obj->dev->dev_private;
<<<<<<< HEAD
		int npages = obj->size >> PAGE_SHIFT;
=======
>>>>>>> v4.9.227
		struct page **pages = get_pages(obj);

		if (IS_ERR(pages))
			return PTR_ERR(pages);

<<<<<<< HEAD
		if (!msm_obj->domain[id].sgt) {
			msm_obj->domain[id].sgt =
					drm_prime_pages_to_sg(pages, npages);
			if (IS_ERR(msm_obj->domain[id].sgt)) {
				DRM_ERROR("failed to allocate sgt\n");
				return PTR_ERR(msm_obj->domain[id].sgt);
			}
		}

		if (iommu_present(&platform_bus_type)) {
			struct msm_mmu *mmu = priv->mmus[id];
=======
		if (iommu_present(&platform_bus_type)) {
			struct msm_mmu *mmu = priv->mmus[id];
			uint32_t offset;
>>>>>>> v4.9.227

			if (WARN_ON(!mmu))
				return -EINVAL;

<<<<<<< HEAD
			if (obj->import_attach && mmu->funcs->map_dma_buf) {
				ret = mmu->funcs->map_dma_buf(mmu,
						msm_obj->domain[id].sgt,
						obj->import_attach->dmabuf,
						DMA_BIDIRECTIONAL);
				if (ret) {
					DRM_ERROR("Unable to map dma buf\n");
					return ret;
				}
				msm_obj->domain[id].iova =
				sg_dma_address(msm_obj->domain[id].sgt->sgl);
			} else if (!use_pages(obj)) {
				/* use vram */
				dma_addr_t pa = physaddr(obj);

				ret = mmu->funcs->map(mmu, pa,
						msm_obj->domain[id].sgt,
						IOMMU_READ | IOMMU_NOEXEC);
				if (ret) {
					DRM_ERROR("Unable to map phy buf=%pK\n",
						(void *)pa);
					return ret;
				}
				msm_obj->domain[id].iova = pa;
			} else {
				if (msm_obj->flags &
						(MSM_BO_WC|MSM_BO_UNCACHED))
					dma_map_sg(mmu->dev,
						msm_obj->domain[id].sgt->sgl,
						msm_obj->domain[id].sgt->nents,
						DMA_BIDIRECTIONAL);
				msm_obj->domain[id].iova =
				sg_dma_address(msm_obj->domain[id].sgt->sgl);
			}
			DRM_DEBUG("iova=%pK\n",
					(void *)msm_obj->domain[id].iova);
		} else {
			WARN_ONCE(1, "physical address being used\n");
=======
			offset = (uint32_t)mmap_offset(obj);
			ret = mmu->funcs->map(mmu, offset, msm_obj->sgt,
					obj->size, IOMMU_READ | IOMMU_WRITE);
			msm_obj->domain[id].iova = offset;
		} else {
>>>>>>> v4.9.227
			msm_obj->domain[id].iova = physaddr(obj);
		}
	}

	if (!ret)
		*iova = msm_obj->domain[id].iova;

	return ret;
}

/* get iova, taking a reference.  Should have a matching put */
int msm_gem_get_iova(struct drm_gem_object *obj, int id, uint32_t *iova)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	int ret;

	/* this is safe right now because we don't unmap until the
	 * bo is deleted:
	 */
	if (msm_obj->domain[id].iova) {
		*iova = msm_obj->domain[id].iova;
		return 0;
	}

	mutex_lock(&obj->dev->struct_mutex);
	ret = msm_gem_get_iova_locked(obj, id, iova);
	mutex_unlock(&obj->dev->struct_mutex);
	return ret;
}

/* get iova without taking a reference, used in places where you have
 * already done a 'msm_gem_get_iova()'.
 */
uint32_t msm_gem_iova(struct drm_gem_object *obj, int id)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	WARN_ON(!msm_obj->domain[id].iova);
	return msm_obj->domain[id].iova;
}

void msm_gem_put_iova(struct drm_gem_object *obj, int id)
{
	// XXX TODO ..
	// NOTE: probably don't need a _locked() version.. we wouldn't
	// normally unmap here, but instead just mark that it could be
	// unmapped (if the iova refcnt drops to zero), but then later
	// if another _get_iova_locked() fails we can start unmapping
	// things that are no longer needed..
}

int msm_gem_dumb_create(struct drm_file *file, struct drm_device *dev,
		struct drm_mode_create_dumb *args)
{
	args->pitch = align_pitch(args->width, args->bpp);
	args->size  = PAGE_ALIGN(args->pitch * args->height);
	return msm_gem_new_handle(dev, file, args->size,
			MSM_BO_SCANOUT | MSM_BO_WC, &args->handle);
}

int msm_gem_dumb_map_offset(struct drm_file *file, struct drm_device *dev,
		uint32_t handle, uint64_t *offset)
{
	struct drm_gem_object *obj;
	int ret = 0;

	/* GEM does all our handle to object mapping */
<<<<<<< HEAD
	obj = drm_gem_object_lookup(dev, file, handle);
=======
	obj = drm_gem_object_lookup(file, handle);
>>>>>>> v4.9.227
	if (obj == NULL) {
		ret = -ENOENT;
		goto fail;
	}

	*offset = msm_gem_mmap_offset(obj);

	drm_gem_object_unreference_unlocked(obj);

fail:
	return ret;
}

<<<<<<< HEAD
void *msm_gem_vaddr_locked(struct drm_gem_object *obj)
=======
void *msm_gem_get_vaddr_locked(struct drm_gem_object *obj)
>>>>>>> v4.9.227
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	WARN_ON(!mutex_is_locked(&obj->dev->struct_mutex));
	if (!msm_obj->vaddr) {
		struct page **pages = get_pages(obj);
		if (IS_ERR(pages))
			return ERR_CAST(pages);
		msm_obj->vaddr = vmap(pages, obj->size >> PAGE_SHIFT,
				VM_MAP, pgprot_writecombine(PAGE_KERNEL));
<<<<<<< HEAD
	}
	return msm_obj->vaddr;
}

void *msm_gem_vaddr(struct drm_gem_object *obj)
{
	void *ret;
	mutex_lock(&obj->dev->struct_mutex);
	ret = msm_gem_vaddr_locked(obj);
=======
		if (msm_obj->vaddr == NULL)
			return ERR_PTR(-ENOMEM);
	}
	msm_obj->vmap_count++;
	return msm_obj->vaddr;
}

void *msm_gem_get_vaddr(struct drm_gem_object *obj)
{
	void *ret;
	mutex_lock(&obj->dev->struct_mutex);
	ret = msm_gem_get_vaddr_locked(obj);
>>>>>>> v4.9.227
	mutex_unlock(&obj->dev->struct_mutex);
	return ret;
}

<<<<<<< HEAD
/* setup callback for when bo is no longer busy..
 * TODO probably want to differentiate read vs write..
 */
int msm_gem_queue_inactive_cb(struct drm_gem_object *obj,
		struct msm_fence_cb *cb)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	uint32_t fence = msm_gem_fence(msm_obj,
			MSM_PREP_READ | MSM_PREP_WRITE);
	return msm_queue_fence_cb(obj->dev, cb, fence);
}

void msm_gem_move_to_active(struct drm_gem_object *obj,
		struct msm_gpu *gpu, bool write, uint32_t fence)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	struct drm_device *dev = obj->dev;

	WARN_ON(!mutex_is_locked(&dev->struct_mutex));

	msm_obj->gpu = gpu;
	if (write)
		msm_obj->write_timestamp = fence;
	else
		msm_obj->read_timestamp = fence;
=======
void msm_gem_put_vaddr_locked(struct drm_gem_object *obj)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	WARN_ON(!mutex_is_locked(&obj->dev->struct_mutex));
	WARN_ON(msm_obj->vmap_count < 1);
	msm_obj->vmap_count--;
}

void msm_gem_put_vaddr(struct drm_gem_object *obj)
{
	mutex_lock(&obj->dev->struct_mutex);
	msm_gem_put_vaddr_locked(obj);
	mutex_unlock(&obj->dev->struct_mutex);
}

/* Update madvise status, returns true if not purged, else
 * false or -errno.
 */
int msm_gem_madvise(struct drm_gem_object *obj, unsigned madv)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);

	WARN_ON(!mutex_is_locked(&obj->dev->struct_mutex));

	if (msm_obj->madv != __MSM_MADV_PURGED)
		msm_obj->madv = madv;

	return (msm_obj->madv != __MSM_MADV_PURGED);
}

void msm_gem_purge(struct drm_gem_object *obj)
{
	struct drm_device *dev = obj->dev;
	struct msm_gem_object *msm_obj = to_msm_bo(obj);

	WARN_ON(!mutex_is_locked(&dev->struct_mutex));
	WARN_ON(!is_purgeable(msm_obj));
	WARN_ON(obj->import_attach);

	put_iova(obj);

	msm_gem_vunmap(obj);

	put_pages(obj);

	msm_obj->madv = __MSM_MADV_PURGED;

	drm_vma_node_unmap(&obj->vma_node, dev->anon_inode->i_mapping);
	drm_gem_free_mmap_offset(obj);

	/* Our goal here is to return as much of the memory as
	 * is possible back to the system as we are called from OOM.
	 * To do this we must instruct the shmfs to drop all of its
	 * backing pages, *now*.
	 */
	shmem_truncate_range(file_inode(obj->filp), 0, (loff_t)-1);

	invalidate_mapping_pages(file_inode(obj->filp)->i_mapping,
			0, (loff_t)-1);
}

void msm_gem_vunmap(struct drm_gem_object *obj)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);

	if (!msm_obj->vaddr || WARN_ON(!is_vunmapable(msm_obj)))
		return;

	vunmap(msm_obj->vaddr);
	msm_obj->vaddr = NULL;
}

/* must be called before _move_to_active().. */
int msm_gem_sync_object(struct drm_gem_object *obj,
		struct msm_fence_context *fctx, bool exclusive)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	struct reservation_object_list *fobj;
	struct fence *fence;
	int i, ret;

	if (!exclusive) {
		/* NOTE: _reserve_shared() must happen before _add_shared_fence(),
		 * which makes this a slightly strange place to call it.  OTOH this
		 * is a convenient can-fail point to hook it in.  (And similar to
		 * how etnaviv and nouveau handle this.)
		 */
		ret = reservation_object_reserve_shared(msm_obj->resv);
		if (ret)
			return ret;
	}

	fobj = reservation_object_get_list(msm_obj->resv);
	if (!fobj || (fobj->shared_count == 0)) {
		fence = reservation_object_get_excl(msm_obj->resv);
		/* don't need to wait on our own fences, since ring is fifo */
		if (fence && (fence->context != fctx->context)) {
			ret = fence_wait(fence, true);
			if (ret)
				return ret;
		}
	}

	if (!exclusive || !fobj)
		return 0;

	for (i = 0; i < fobj->shared_count; i++) {
		fence = rcu_dereference_protected(fobj->shared[i],
						reservation_object_held(msm_obj->resv));
		if (fence->context != fctx->context) {
			ret = fence_wait(fence, true);
			if (ret)
				return ret;
		}
	}

	return 0;
}

void msm_gem_move_to_active(struct drm_gem_object *obj,
		struct msm_gpu *gpu, bool exclusive, struct fence *fence)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	WARN_ON(msm_obj->madv != MSM_MADV_WILLNEED);
	msm_obj->gpu = gpu;
	if (exclusive)
		reservation_object_add_excl_fence(msm_obj->resv, fence);
	else
		reservation_object_add_shared_fence(msm_obj->resv, fence);
>>>>>>> v4.9.227
	list_del_init(&msm_obj->mm_list);
	list_add_tail(&msm_obj->mm_list, &gpu->active_list);
}

void msm_gem_move_to_inactive(struct drm_gem_object *obj)
{
	struct drm_device *dev = obj->dev;
	struct msm_drm_private *priv = dev->dev_private;
	struct msm_gem_object *msm_obj = to_msm_bo(obj);

	WARN_ON(!mutex_is_locked(&dev->struct_mutex));

	msm_obj->gpu = NULL;
<<<<<<< HEAD
	msm_obj->read_timestamp = 0;
	msm_obj->write_timestamp = 0;
=======
>>>>>>> v4.9.227
	list_del_init(&msm_obj->mm_list);
	list_add_tail(&msm_obj->mm_list, &priv->inactive_list);
}

int msm_gem_cpu_prep(struct drm_gem_object *obj, uint32_t op, ktime_t *timeout)
{
<<<<<<< HEAD
	struct drm_device *dev = obj->dev;
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	int ret = 0;

	if (is_active(msm_obj)) {
		uint32_t fence = msm_gem_fence(msm_obj, op);

		if (op & MSM_PREP_NOSYNC)
			timeout = NULL;

		ret = msm_wait_fence(dev, fence, timeout, true);
	}

	/* TODO cache maintenance */

	return ret;
=======
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	bool write = !!(op & MSM_PREP_WRITE);
	unsigned long remain =
		op & MSM_PREP_NOSYNC ? 0 : timeout_to_jiffies(timeout);
	long ret;

	ret = reservation_object_wait_timeout_rcu(msm_obj->resv, write,
						  true,  remain);
	if (ret == 0)
		return remain == 0 ? -EBUSY : -ETIMEDOUT;
	else if (ret < 0)
		return ret;

	/* TODO cache maintenance */

	return 0;
>>>>>>> v4.9.227
}

int msm_gem_cpu_fini(struct drm_gem_object *obj)
{
	/* TODO cache maintenance */
	return 0;
}

#ifdef CONFIG_DEBUG_FS
<<<<<<< HEAD
void msm_gem_describe(struct drm_gem_object *obj, struct seq_file *m)
{
	struct drm_device *dev = obj->dev;
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	uint64_t off = drm_vma_node_start(&obj->vma_node);

	WARN_ON(!mutex_is_locked(&dev->struct_mutex));
	seq_printf(m, "%08x: %c(r=%u,w=%u) %2d (%2d) %08llx %pK %zu\n",
			msm_obj->flags, is_active(msm_obj) ? 'A' : 'I',
			msm_obj->read_timestamp, msm_obj->write_timestamp,
			obj->name, obj->refcount.refcount.counter,
			off, msm_obj->vaddr, obj->size);
=======
static void describe_fence(struct fence *fence, const char *type,
		struct seq_file *m)
{
	if (!fence_is_signaled(fence))
		seq_printf(m, "\t%9s: %s %s seq %u\n", type,
				fence->ops->get_driver_name(fence),
				fence->ops->get_timeline_name(fence),
				fence->seqno);
}

void msm_gem_describe(struct drm_gem_object *obj, struct seq_file *m)
{
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	struct reservation_object *robj = msm_obj->resv;
	struct reservation_object_list *fobj;
	struct fence *fence;
	uint64_t off = drm_vma_node_start(&obj->vma_node);
	const char *madv;

	WARN_ON(!mutex_is_locked(&obj->dev->struct_mutex));

	switch (msm_obj->madv) {
	case __MSM_MADV_PURGED:
		madv = " purged";
		break;
	case MSM_MADV_DONTNEED:
		madv = " purgeable";
		break;
	case MSM_MADV_WILLNEED:
	default:
		madv = "";
		break;
	}

	seq_printf(m, "%08x: %c %2d (%2d) %08llx %p %zu%s\n",
			msm_obj->flags, is_active(msm_obj) ? 'A' : 'I',
			obj->name, obj->refcount.refcount.counter,
			off, msm_obj->vaddr, obj->size, madv);

	rcu_read_lock();
	fobj = rcu_dereference(robj->fence);
	if (fobj) {
		unsigned int i, shared_count = fobj->shared_count;

		for (i = 0; i < shared_count; i++) {
			fence = rcu_dereference(fobj->shared[i]);
			describe_fence(fence, "Shared", m);
		}
	}

	fence = rcu_dereference(robj->fence_excl);
	if (fence)
		describe_fence(fence, "Exclusive", m);
	rcu_read_unlock();
>>>>>>> v4.9.227
}

void msm_gem_describe_objects(struct list_head *list, struct seq_file *m)
{
	struct msm_gem_object *msm_obj;
	int count = 0;
	size_t size = 0;

	list_for_each_entry(msm_obj, list, mm_list) {
		struct drm_gem_object *obj = &msm_obj->base;
		seq_printf(m, "   ");
		msm_gem_describe(obj, m);
		count++;
		size += obj->size;
	}

	seq_printf(m, "Total %d objects, %zu bytes\n", count, size);
}
#endif

void msm_gem_free_object(struct drm_gem_object *obj)
{
	struct drm_device *dev = obj->dev;
<<<<<<< HEAD
	struct msm_drm_private *priv = obj->dev->dev_private;
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
	int id;
=======
	struct msm_gem_object *msm_obj = to_msm_bo(obj);
>>>>>>> v4.9.227

	WARN_ON(!mutex_is_locked(&dev->struct_mutex));

	/* object should not be on active list: */
	WARN_ON(is_active(msm_obj));

	list_del(&msm_obj->mm_list);

<<<<<<< HEAD
	for (id = 0; id < ARRAY_SIZE(msm_obj->domain); id++) {
		struct msm_mmu *mmu = priv->mmus[id];
		if (mmu && msm_obj->domain[id].iova) {
			if (obj->import_attach && mmu->funcs->unmap_dma_buf) {
				mmu->funcs->unmap_dma_buf(mmu,
						msm_obj->domain[id].sgt,
						obj->import_attach->dmabuf,
						DMA_BIDIRECTIONAL);
			} else if (!use_pages(obj)) {
				uint32_t offset = msm_obj->domain[id].iova;

				mmu->funcs->unmap(mmu, offset,
					msm_obj->domain[id].sgt);
			} else {
				dma_unmap_sg(mmu->dev,
					msm_obj->domain[id].sgt->sgl,
					msm_obj->domain[id].sgt->nents,
					DMA_BIDIRECTIONAL);
			}
			msm_obj->domain[id].iova = 0;
		}

		if (msm_obj->domain[id].sgt) {
			sg_free_table(msm_obj->domain[id].sgt);
			kfree(msm_obj->domain[id].sgt);
			msm_obj->domain[id].sgt = NULL;
		}
	}

	if (obj->import_attach) {
		if (msm_obj->vaddr)
			dma_buf_vunmap(obj->import_attach->dmabuf,
					msm_obj->vaddr);
=======
	put_iova(obj);

	if (obj->import_attach) {
		if (msm_obj->vaddr)
			dma_buf_vunmap(obj->import_attach->dmabuf, msm_obj->vaddr);
>>>>>>> v4.9.227

		/* Don't drop the pages for imported dmabuf, as they are not
		 * ours, just free the array we allocated:
		 */
<<<<<<< HEAD
		if (msm_obj->pages) {
			drm_free_large(msm_obj->pages);
			msm_obj->pages = NULL;
		}

		drm_prime_gem_destroy(obj, msm_obj->import_sgt);
	} else {
		if (msm_obj->vaddr)
			vunmap(msm_obj->vaddr);
=======
		if (msm_obj->pages)
			drm_free_large(msm_obj->pages);

		drm_prime_gem_destroy(obj, msm_obj->sgt);
	} else {
		msm_gem_vunmap(obj);
>>>>>>> v4.9.227
		put_pages(obj);
	}

	if (msm_obj->resv == &msm_obj->_resv)
		reservation_object_fini(msm_obj->resv);

	drm_gem_object_release(obj);

	kfree(msm_obj);
}

/* convenience method to construct a GEM buffer object, and userspace handle */
int msm_gem_new_handle(struct drm_device *dev, struct drm_file *file,
		uint32_t size, uint32_t flags, uint32_t *handle)
{
	struct drm_gem_object *obj;
	int ret;

	ret = mutex_lock_interruptible(&dev->struct_mutex);
	if (ret)
		return ret;

	obj = msm_gem_new(dev, size, flags);

	mutex_unlock(&dev->struct_mutex);

	if (IS_ERR(obj))
		return PTR_ERR(obj);

	ret = drm_gem_handle_create(file, obj, handle);

	/* drop reference from allocate - handle holds it now */
	drm_gem_object_unreference_unlocked(obj);

	return ret;
}

static int msm_gem_new_impl(struct drm_device *dev,
		uint32_t size, uint32_t flags,
<<<<<<< HEAD
=======
		struct reservation_object *resv,
>>>>>>> v4.9.227
		struct drm_gem_object **obj)
{
	struct msm_drm_private *priv = dev->dev_private;
	struct msm_gem_object *msm_obj;
	unsigned sz;
	bool use_vram = false;

<<<<<<< HEAD
=======
	WARN_ON(!mutex_is_locked(&dev->struct_mutex));

>>>>>>> v4.9.227
	switch (flags & MSM_BO_CACHE_MASK) {
	case MSM_BO_UNCACHED:
	case MSM_BO_CACHED:
	case MSM_BO_WC:
		break;
	default:
<<<<<<< HEAD
		DRM_ERROR("invalid cache flag: %x\n",
=======
		dev_err(dev->dev, "invalid cache flag: %x\n",
>>>>>>> v4.9.227
				(flags & MSM_BO_CACHE_MASK));
		return -EINVAL;
	}

	if (!iommu_present(&platform_bus_type))
		use_vram = true;
	else if ((flags & MSM_BO_STOLEN) && priv->vram.size)
		use_vram = true;

	if (WARN_ON(use_vram && !priv->vram.size))
		return -EINVAL;

	sz = sizeof(*msm_obj);
	if (use_vram)
		sz += sizeof(struct drm_mm_node);

	msm_obj = kzalloc(sz, GFP_KERNEL);
	if (!msm_obj)
		return -ENOMEM;

	if (use_vram)
		msm_obj->vram_node = (void *)&msm_obj[1];

	msm_obj->flags = flags;
<<<<<<< HEAD

	msm_obj->resv = &msm_obj->_resv;
	reservation_object_init(msm_obj->resv);

	if (use_vram) {
		/* Update start page index */
		struct msm_gem_object *pos;
		dma_addr_t start = 0;

		list_for_each_entry(pos, &priv->inactive_list, mm_list) {
			struct drm_gem_object *gem_obj = &pos->base;
			struct drm_mm_node *vram_node = pos->vram_node;

			if (vram_node)
				start += vram_node->start;
			start += (gem_obj->size >> PAGE_SHIFT);
		}
		msm_obj->vram_node->start = start;
=======
	msm_obj->madv = MSM_MADV_WILLNEED;

	if (resv) {
		msm_obj->resv = resv;
	} else {
		msm_obj->resv = &msm_obj->_resv;
		reservation_object_init(msm_obj->resv);
>>>>>>> v4.9.227
	}

	INIT_LIST_HEAD(&msm_obj->submit_entry);
	list_add_tail(&msm_obj->mm_list, &priv->inactive_list);

	*obj = &msm_obj->base;

	return 0;
}

struct drm_gem_object *msm_gem_new(struct drm_device *dev,
		uint32_t size, uint32_t flags)
{
	struct drm_gem_object *obj = NULL;
	int ret;

	WARN_ON(!mutex_is_locked(&dev->struct_mutex));

	size = PAGE_ALIGN(size);

<<<<<<< HEAD
	ret = msm_gem_new_impl(dev, size, flags, &obj);
=======
	ret = msm_gem_new_impl(dev, size, flags, NULL, &obj);
>>>>>>> v4.9.227
	if (ret)
		goto fail;

	if (use_pages(obj)) {
		ret = drm_gem_object_init(dev, obj, size);
		if (ret)
			goto fail;
	} else {
		drm_gem_private_object_init(dev, obj, size);
	}

	return obj;

fail:
<<<<<<< HEAD
	if (obj)
		drm_gem_object_unreference(obj);

=======
	drm_gem_object_unreference(obj);
>>>>>>> v4.9.227
	return ERR_PTR(ret);
}

struct drm_gem_object *msm_gem_import(struct drm_device *dev,
<<<<<<< HEAD
		uint32_t size, struct sg_table *sgt)
{
	struct msm_gem_object *msm_obj;
	struct drm_gem_object *obj;
=======
		struct dma_buf *dmabuf, struct sg_table *sgt)
{
	struct msm_gem_object *msm_obj;
	struct drm_gem_object *obj;
	uint32_t size;
>>>>>>> v4.9.227
	int ret, npages;

	/* if we don't have IOMMU, don't bother pretending we can import: */
	if (!iommu_present(&platform_bus_type)) {
<<<<<<< HEAD
		DRM_ERROR("cannot import without IOMMU\n");
		return ERR_PTR(-EINVAL);
	}

	size = PAGE_ALIGN(size);

	mutex_lock(&dev->struct_mutex);
	ret = msm_gem_new_impl(dev, size, MSM_BO_WC, &obj);
	mutex_unlock(&dev->struct_mutex);
=======
		dev_err(dev->dev, "cannot import without IOMMU\n");
		return ERR_PTR(-EINVAL);
	}

	size = PAGE_ALIGN(dmabuf->size);

	/* Take mutex so we can modify the inactive list in msm_gem_new_impl */
	mutex_lock(&dev->struct_mutex);
	ret = msm_gem_new_impl(dev, size, MSM_BO_WC, dmabuf->resv, &obj);
	mutex_unlock(&dev->struct_mutex);

>>>>>>> v4.9.227
	if (ret)
		goto fail;

	drm_gem_private_object_init(dev, obj, size);

	npages = size / PAGE_SIZE;

	msm_obj = to_msm_bo(obj);
<<<<<<< HEAD
	msm_obj->import_sgt = sgt;
=======
	msm_obj->sgt = sgt;
>>>>>>> v4.9.227
	msm_obj->pages = drm_malloc_ab(npages, sizeof(struct page *));
	if (!msm_obj->pages) {
		ret = -ENOMEM;
		goto fail;
	}

<<<<<<< HEAD
	ret = drm_prime_sg_to_page_addr_arrays(sgt, msm_obj->pages, NULL,
						 npages);
=======
	ret = drm_prime_sg_to_page_addr_arrays(sgt, msm_obj->pages, NULL, npages);
>>>>>>> v4.9.227
	if (ret)
		goto fail;

	return obj;

fail:
<<<<<<< HEAD
	if (obj)
		drm_gem_object_unreference_unlocked(obj);

=======
	drm_gem_object_unreference_unlocked(obj);
>>>>>>> v4.9.227
	return ERR_PTR(ret);
}
