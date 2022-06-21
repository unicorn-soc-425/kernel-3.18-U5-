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

#include "msm_drv.h"
#include "msm_kms.h"

#include "drm_crtc.h"
#include "drm_crtc_helper.h"

struct msm_framebuffer {
	struct drm_framebuffer base;
	const struct msm_format *format;
	struct drm_gem_object *planes[MAX_PLANE];
};
#define to_msm_framebuffer(x) container_of(x, struct msm_framebuffer, base)


static int msm_framebuffer_create_handle(struct drm_framebuffer *fb,
		struct drm_file *file_priv,
		unsigned int *handle)
{
	struct msm_framebuffer *msm_fb = to_msm_framebuffer(fb);
	return drm_gem_handle_create(file_priv,
			msm_fb->planes[0], handle);
}

static void msm_framebuffer_destroy(struct drm_framebuffer *fb)
{
	struct msm_framebuffer *msm_fb = to_msm_framebuffer(fb);
	int i, n = drm_format_num_planes(fb->pixel_format);

<<<<<<< HEAD
	DBG("destroy: FB ID: %d (%pK)", fb->base.id, fb);
=======
	DBG("destroy: FB ID: %d (%p)", fb->base.id, fb);
>>>>>>> v4.9.227

	drm_framebuffer_cleanup(fb);

	for (i = 0; i < n; i++) {
		struct drm_gem_object *bo = msm_fb->planes[i];
<<<<<<< HEAD
		if (bo)
			drm_gem_object_unreference_unlocked(bo);
=======

		drm_gem_object_unreference_unlocked(bo);
>>>>>>> v4.9.227
	}

	kfree(msm_fb);
}

<<<<<<< HEAD
static int msm_framebuffer_dirty(struct drm_framebuffer *fb,
		struct drm_file *file_priv, unsigned flags, unsigned color,
		struct drm_clip_rect *clips, unsigned num_clips)
{
	return 0;
}

static const struct drm_framebuffer_funcs msm_framebuffer_funcs = {
	.create_handle = msm_framebuffer_create_handle,
	.destroy = msm_framebuffer_destroy,
	.dirty = msm_framebuffer_dirty,
=======
static const struct drm_framebuffer_funcs msm_framebuffer_funcs = {
	.create_handle = msm_framebuffer_create_handle,
	.destroy = msm_framebuffer_destroy,
>>>>>>> v4.9.227
};

#ifdef CONFIG_DEBUG_FS
void msm_framebuffer_describe(struct drm_framebuffer *fb, struct seq_file *m)
{
	struct msm_framebuffer *msm_fb = to_msm_framebuffer(fb);
	int i, n = drm_format_num_planes(fb->pixel_format);

	seq_printf(m, "fb: %dx%d@%4.4s (%2d, ID:%d)\n",
			fb->width, fb->height, (char *)&fb->pixel_format,
<<<<<<< HEAD
			fb->refcount.refcount.counter, fb->base.id);
=======
			drm_framebuffer_read_refcount(fb), fb->base.id);
>>>>>>> v4.9.227

	for (i = 0; i < n; i++) {
		seq_printf(m, "   %d: offset=%d pitch=%d, obj: ",
				i, fb->offsets[i], fb->pitches[i]);
		msm_gem_describe(msm_fb->planes[i], m);
	}
}
#endif

/* prepare/pin all the fb's bo's for scanout.  Note that it is not valid
 * to prepare an fb more multiple different initiator 'id's.  But that
 * should be fine, since only the scanout (mdpN) side of things needs
 * this, the gpu doesn't care about fb's.
 */
int msm_framebuffer_prepare(struct drm_framebuffer *fb, int id)
{
	struct msm_framebuffer *msm_fb = to_msm_framebuffer(fb);
	int ret, i, n = drm_format_num_planes(fb->pixel_format);
	uint32_t iova;

	for (i = 0; i < n; i++) {
		ret = msm_gem_get_iova(msm_fb->planes[i], id, &iova);
		DBG("FB[%u]: iova[%d]: %08x (%d)", fb->base.id, i, iova, ret);
		if (ret)
			return ret;
	}

	return 0;
}

void msm_framebuffer_cleanup(struct drm_framebuffer *fb, int id)
{
	struct msm_framebuffer *msm_fb = to_msm_framebuffer(fb);
	int i, n = drm_format_num_planes(fb->pixel_format);

	for (i = 0; i < n; i++)
		msm_gem_put_iova(msm_fb->planes[i], id);
}

uint32_t msm_framebuffer_iova(struct drm_framebuffer *fb, int id, int plane)
{
	struct msm_framebuffer *msm_fb = to_msm_framebuffer(fb);
	if (!msm_fb->planes[plane])
		return 0;
	return msm_gem_iova(msm_fb->planes[plane], id) + fb->offsets[plane];
}

struct drm_gem_object *msm_framebuffer_bo(struct drm_framebuffer *fb, int plane)
{
	struct msm_framebuffer *msm_fb = to_msm_framebuffer(fb);
	return msm_fb->planes[plane];
}

const struct msm_format *msm_framebuffer_format(struct drm_framebuffer *fb)
{
<<<<<<< HEAD
	return fb ? (to_msm_framebuffer(fb))->format : NULL;
}

struct drm_framebuffer *msm_framebuffer_create(struct drm_device *dev,
		struct drm_file *file, struct drm_mode_fb_cmd2 *mode_cmd)
=======
	struct msm_framebuffer *msm_fb = to_msm_framebuffer(fb);
	return msm_fb->format;
}

struct drm_framebuffer *msm_framebuffer_create(struct drm_device *dev,
		struct drm_file *file, const struct drm_mode_fb_cmd2 *mode_cmd)
>>>>>>> v4.9.227
{
	struct drm_gem_object *bos[4] = {0};
	struct drm_framebuffer *fb;
	int ret, i, n = drm_format_num_planes(mode_cmd->pixel_format);

	for (i = 0; i < n; i++) {
<<<<<<< HEAD
		bos[i] = drm_gem_object_lookup(dev, file,
				mode_cmd->handles[i]);
=======
		bos[i] = drm_gem_object_lookup(file, mode_cmd->handles[i]);
>>>>>>> v4.9.227
		if (!bos[i]) {
			ret = -ENXIO;
			goto out_unref;
		}
	}

	fb = msm_framebuffer_init(dev, mode_cmd, bos);
	if (IS_ERR(fb)) {
		ret = PTR_ERR(fb);
		goto out_unref;
	}

	return fb;

out_unref:
	for (i = 0; i < n; i++)
		drm_gem_object_unreference_unlocked(bos[i]);
	return ERR_PTR(ret);
}

struct drm_framebuffer *msm_framebuffer_init(struct drm_device *dev,
<<<<<<< HEAD
		struct drm_mode_fb_cmd2 *mode_cmd, struct drm_gem_object **bos)
=======
		const struct drm_mode_fb_cmd2 *mode_cmd, struct drm_gem_object **bos)
>>>>>>> v4.9.227
{
	struct msm_drm_private *priv = dev->dev_private;
	struct msm_kms *kms = priv->kms;
	struct msm_framebuffer *msm_fb = NULL;
	struct drm_framebuffer *fb;
	const struct msm_format *format;
<<<<<<< HEAD
	int ret, i, num_planes;
	unsigned int hsub, vsub;
	bool is_modified = false;

	DBG("create framebuffer: dev=%pK, mode_cmd=%pK (%dx%d@%4.4s)",
			dev, mode_cmd, mode_cmd->width, mode_cmd->height,
			(char *)&mode_cmd->pixel_format);

	num_planes = drm_format_num_planes(mode_cmd->pixel_format);
	hsub = drm_format_horz_chroma_subsampling(mode_cmd->pixel_format);
	vsub = drm_format_vert_chroma_subsampling(mode_cmd->pixel_format);

	format = kms->funcs->get_format(kms, mode_cmd->pixel_format,
			mode_cmd->modifier, num_planes);
=======
	int ret, i, n;
	unsigned int hsub, vsub;

	DBG("create framebuffer: dev=%p, mode_cmd=%p (%dx%d@%4.4s)",
			dev, mode_cmd, mode_cmd->width, mode_cmd->height,
			(char *)&mode_cmd->pixel_format);

	n = drm_format_num_planes(mode_cmd->pixel_format);
	hsub = drm_format_horz_chroma_subsampling(mode_cmd->pixel_format);
	vsub = drm_format_vert_chroma_subsampling(mode_cmd->pixel_format);

	format = kms->funcs->get_format(kms, mode_cmd->pixel_format);
>>>>>>> v4.9.227
	if (!format) {
		dev_err(dev->dev, "unsupported pixel format: %4.4s\n",
				(char *)&mode_cmd->pixel_format);
		ret = -EINVAL;
		goto fail;
	}

	msm_fb = kzalloc(sizeof(*msm_fb), GFP_KERNEL);
	if (!msm_fb) {
		ret = -ENOMEM;
		goto fail;
	}

	fb = &msm_fb->base;

	msm_fb->format = format;

<<<<<<< HEAD
	if (mode_cmd->flags & DRM_MODE_FB_MODIFIERS) {
		for (i = 0; i < ARRAY_SIZE(mode_cmd->modifier); i++) {
			if (mode_cmd->modifier[i]) {
				is_modified = true;
				break;
			}
		}
	}

	if (num_planes > ARRAY_SIZE(msm_fb->planes)) {
=======
	if (n > ARRAY_SIZE(msm_fb->planes)) {
>>>>>>> v4.9.227
		ret = -EINVAL;
		goto fail;
	}

<<<<<<< HEAD
	if (is_modified) {
		if (!kms->funcs->check_modified_format) {
			dev_err(dev->dev, "can't check modified fb format\n");
			ret = -EINVAL;
			goto fail;
		} else {
			ret = kms->funcs->check_modified_format(
				kms, msm_fb->format, mode_cmd, bos);
			if (ret)
				goto fail;
		}
	} else {
		for (i = 0; i < num_planes; i++) {
			unsigned int width = mode_cmd->width / (i ? hsub : 1);
			unsigned int height = mode_cmd->height / (i ? vsub : 1);
			unsigned int min_size;
			unsigned int cpp;

			cpp = drm_format_plane_cpp(mode_cmd->pixel_format, i);

			min_size = (height - 1) * mode_cmd->pitches[i]
				 + width * cpp
				 + mode_cmd->offsets[i];

			if (bos[i]->size < min_size) {
				ret = -EINVAL;
				goto fail;
			}
		}
	}

	for (i = 0; i < num_planes; i++)
		msm_fb->planes[i] = bos[i];
=======
	for (i = 0; i < n; i++) {
		unsigned int width = mode_cmd->width / (i ? hsub : 1);
		unsigned int height = mode_cmd->height / (i ? vsub : 1);
		unsigned int min_size;

		min_size = (height - 1) * mode_cmd->pitches[i]
			 + width * drm_format_plane_cpp(mode_cmd->pixel_format, i)
			 + mode_cmd->offsets[i];

		if (bos[i]->size < min_size) {
			ret = -EINVAL;
			goto fail;
		}

		msm_fb->planes[i] = bos[i];
	}
>>>>>>> v4.9.227

	drm_helper_mode_fill_fb_struct(fb, mode_cmd);

	ret = drm_framebuffer_init(dev, fb, &msm_framebuffer_funcs);
	if (ret) {
		dev_err(dev->dev, "framebuffer init failed: %d\n", ret);
		goto fail;
	}

<<<<<<< HEAD
	DBG("create: FB ID: %d (%pK)", fb->base.id, fb);
=======
	DBG("create: FB ID: %d (%p)", fb->base.id, fb);
>>>>>>> v4.9.227

	return fb;

fail:
	kfree(msm_fb);

	return ERR_PTR(ret);
}
