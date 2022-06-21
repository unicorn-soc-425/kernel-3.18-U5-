/*
 * vsp1_wpf.c  --  R-Car VSP1 Write Pixel Formatter
 *
 * Copyright (C) 2013-2014 Renesas Electronics Corporation
 *
 * Contact: Laurent Pinchart (laurent.pinchart@ideasonboard.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/device.h>

#include <media/v4l2-subdev.h>

#include "vsp1.h"
<<<<<<< HEAD
#include "vsp1_rwpf.h"
#include "vsp1_video.h"

#define WPF_MAX_WIDTH				2048
#define WPF_MAX_HEIGHT				2048
=======
#include "vsp1_dl.h"
#include "vsp1_pipe.h"
#include "vsp1_rwpf.h"
#include "vsp1_video.h"

#define WPF_GEN2_MAX_WIDTH			2048U
#define WPF_GEN2_MAX_HEIGHT			2048U
#define WPF_GEN3_MAX_WIDTH			8190U
#define WPF_GEN3_MAX_HEIGHT			8190U
>>>>>>> v4.9.227

/* -----------------------------------------------------------------------------
 * Device Access
 */

<<<<<<< HEAD
static inline u32 vsp1_wpf_read(struct vsp1_rwpf *wpf, u32 reg)
{
	return vsp1_read(wpf->entity.vsp1,
			 reg + wpf->entity.index * VI6_WPF_OFFSET);
}

static inline void vsp1_wpf_write(struct vsp1_rwpf *wpf, u32 reg, u32 data)
{
	vsp1_write(wpf->entity.vsp1,
		   reg + wpf->entity.index * VI6_WPF_OFFSET, data);
=======
static inline void vsp1_wpf_write(struct vsp1_rwpf *wpf,
				  struct vsp1_dl_list *dl, u32 reg, u32 data)
{
	vsp1_dl_list_write(dl, reg + wpf->entity.index * VI6_WPF_OFFSET, data);
>>>>>>> v4.9.227
}

/* -----------------------------------------------------------------------------
 * Controls
 */

<<<<<<< HEAD
static int wpf_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct vsp1_rwpf *wpf =
		container_of(ctrl->handler, struct vsp1_rwpf, ctrls);
	u32 value;

	if (!vsp1_entity_is_streaming(&wpf->entity))
		return 0;

	switch (ctrl->id) {
	case V4L2_CID_ALPHA_COMPONENT:
		value = vsp1_wpf_read(wpf, VI6_WPF_OUTFMT);
		value &= ~VI6_WPF_OUTFMT_PDV_MASK;
		value |= ctrl->val << VI6_WPF_OUTFMT_PDV_SHIFT;
		vsp1_wpf_write(wpf, VI6_WPF_OUTFMT, value);
		break;
=======
enum wpf_flip_ctrl {
	WPF_CTRL_VFLIP = 0,
	WPF_CTRL_HFLIP = 1,
	WPF_CTRL_MAX,
};

static int vsp1_wpf_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct vsp1_rwpf *wpf =
		container_of(ctrl->handler, struct vsp1_rwpf, ctrls);
	unsigned int i;
	u32 flip = 0;

	switch (ctrl->id) {
	case V4L2_CID_HFLIP:
	case V4L2_CID_VFLIP:
		for (i = 0; i < WPF_CTRL_MAX; ++i) {
			if (wpf->flip.ctrls[i])
				flip |= wpf->flip.ctrls[i]->val ? BIT(i) : 0;
		}

		spin_lock_irq(&wpf->flip.lock);
		wpf->flip.pending = flip;
		spin_unlock_irq(&wpf->flip.lock);
		break;

	default:
		return -EINVAL;
>>>>>>> v4.9.227
	}

	return 0;
}

<<<<<<< HEAD
static const struct v4l2_ctrl_ops wpf_ctrl_ops = {
	.s_ctrl = wpf_s_ctrl,
};

=======
static const struct v4l2_ctrl_ops vsp1_wpf_ctrl_ops = {
	.s_ctrl = vsp1_wpf_s_ctrl,
};

static int wpf_init_controls(struct vsp1_rwpf *wpf)
{
	struct vsp1_device *vsp1 = wpf->entity.vsp1;
	unsigned int num_flip_ctrls;

	spin_lock_init(&wpf->flip.lock);

	if (wpf->entity.index != 0) {
		/* Only WPF0 supports flipping. */
		num_flip_ctrls = 0;
	} else if (vsp1->info->features & VSP1_HAS_WPF_HFLIP) {
		/* When horizontal flip is supported the WPF implements two
		 * controls (horizontal flip and vertical flip).
		 */
		num_flip_ctrls = 2;
	} else if (vsp1->info->features & VSP1_HAS_WPF_VFLIP) {
		/* When only vertical flip is supported the WPF implements a
		 * single control (vertical flip).
		 */
		num_flip_ctrls = 1;
	} else {
		/* Otherwise flipping is not supported. */
		num_flip_ctrls = 0;
	}

	vsp1_rwpf_init_ctrls(wpf, num_flip_ctrls);

	if (num_flip_ctrls >= 1) {
		wpf->flip.ctrls[WPF_CTRL_VFLIP] =
			v4l2_ctrl_new_std(&wpf->ctrls, &vsp1_wpf_ctrl_ops,
					  V4L2_CID_VFLIP, 0, 1, 1, 0);
	}

	if (num_flip_ctrls == 2) {
		wpf->flip.ctrls[WPF_CTRL_HFLIP] =
			v4l2_ctrl_new_std(&wpf->ctrls, &vsp1_wpf_ctrl_ops,
					  V4L2_CID_HFLIP, 0, 1, 1, 0);

		v4l2_ctrl_cluster(2, wpf->flip.ctrls);
	}

	if (wpf->ctrls.error) {
		dev_err(vsp1->dev, "wpf%u: failed to initialize controls\n",
			wpf->entity.index);
		return wpf->ctrls.error;
	}

	return 0;
}

>>>>>>> v4.9.227
/* -----------------------------------------------------------------------------
 * V4L2 Subdevice Core Operations
 */

static int wpf_s_stream(struct v4l2_subdev *subdev, int enable)
{
<<<<<<< HEAD
	struct vsp1_pipeline *pipe = to_vsp1_pipeline(&subdev->entity);
	struct vsp1_rwpf *wpf = to_rwpf(subdev);
	struct vsp1_device *vsp1 = wpf->entity.vsp1;
	const struct v4l2_rect *crop = &wpf->crop;
	unsigned int i;
	u32 srcrpf = 0;
	u32 outfmt = 0;
	int ret;

	ret = vsp1_entity_set_streaming(&wpf->entity, enable);
	if (ret < 0)
		return ret;

	if (!enable) {
		vsp1_write(vsp1, VI6_WPF_IRQ_ENB(wpf->entity.index), 0);
		vsp1_wpf_write(wpf, VI6_WPF_SRCRPF, 0);
		return 0;
	}

	/* Sources. If the pipeline has a single input configure it as the
	 * master layer. Otherwise configure all inputs as sub-layers and
	 * select the virtual RPF as the master layer.
	 */
	for (i = 0; i < pipe->num_inputs; ++i) {
		struct vsp1_rwpf *input = pipe->inputs[i];

		srcrpf |= pipe->num_inputs == 1
			? VI6_WPF_SRCRPF_RPF_ACT_MST(input->entity.index)
			: VI6_WPF_SRCRPF_RPF_ACT_SUB(input->entity.index);
	}

	if (pipe->num_inputs > 1)
		srcrpf |= VI6_WPF_SRCRPF_VIRACT_MST;

	vsp1_wpf_write(wpf, VI6_WPF_SRCRPF, srcrpf);

	/* Destination stride. */
	if (!pipe->lif) {
		struct v4l2_pix_format_mplane *format = &wpf->video.format;

		vsp1_wpf_write(wpf, VI6_WPF_DSTM_STRIDE_Y,
			       format->plane_fmt[0].bytesperline);
		if (format->num_planes > 1)
			vsp1_wpf_write(wpf, VI6_WPF_DSTM_STRIDE_C,
				       format->plane_fmt[1].bytesperline);
	}

	vsp1_wpf_write(wpf, VI6_WPF_HSZCLIP, VI6_WPF_SZCLIP_EN |
		       (crop->left << VI6_WPF_SZCLIP_OFST_SHIFT) |
		       (crop->width << VI6_WPF_SZCLIP_SIZE_SHIFT));
	vsp1_wpf_write(wpf, VI6_WPF_VSZCLIP, VI6_WPF_SZCLIP_EN |
		       (crop->top << VI6_WPF_SZCLIP_OFST_SHIFT) |
		       (crop->height << VI6_WPF_SZCLIP_SIZE_SHIFT));

	/* Format */
	if (!pipe->lif) {
		const struct vsp1_format_info *fmtinfo = wpf->video.fmtinfo;
=======
	struct vsp1_rwpf *wpf = to_rwpf(subdev);
	struct vsp1_device *vsp1 = wpf->entity.vsp1;

	if (enable)
		return 0;

	/* Write to registers directly when stopping the stream as there will be
	 * no pipeline run to apply the display list.
	 */
	vsp1_write(vsp1, VI6_WPF_IRQ_ENB(wpf->entity.index), 0);
	vsp1_write(vsp1, wpf->entity.index * VI6_WPF_OFFSET +
		   VI6_WPF_SRCRPF, 0);

	return 0;
}

/* -----------------------------------------------------------------------------
 * V4L2 Subdevice Operations
 */

static const struct v4l2_subdev_video_ops wpf_video_ops = {
	.s_stream = wpf_s_stream,
};

static const struct v4l2_subdev_ops wpf_ops = {
	.video	= &wpf_video_ops,
	.pad    = &vsp1_rwpf_pad_ops,
};

/* -----------------------------------------------------------------------------
 * VSP1 Entity Operations
 */

static void vsp1_wpf_destroy(struct vsp1_entity *entity)
{
	struct vsp1_rwpf *wpf = entity_to_rwpf(entity);

	vsp1_dlm_destroy(wpf->dlm);
}

static void wpf_configure(struct vsp1_entity *entity,
			  struct vsp1_pipeline *pipe,
			  struct vsp1_dl_list *dl,
			  enum vsp1_entity_params params)
{
	struct vsp1_rwpf *wpf = to_rwpf(&entity->subdev);
	struct vsp1_device *vsp1 = wpf->entity.vsp1;
	const struct v4l2_mbus_framefmt *source_format;
	const struct v4l2_mbus_framefmt *sink_format;
	unsigned int i;
	u32 outfmt = 0;
	u32 srcrpf = 0;

	if (params == VSP1_ENTITY_PARAMS_RUNTIME) {
		const unsigned int mask = BIT(WPF_CTRL_VFLIP)
					| BIT(WPF_CTRL_HFLIP);
		unsigned long flags;

		spin_lock_irqsave(&wpf->flip.lock, flags);
		wpf->flip.active = (wpf->flip.active & ~mask)
				 | (wpf->flip.pending & mask);
		spin_unlock_irqrestore(&wpf->flip.lock, flags);

		outfmt = (wpf->alpha << VI6_WPF_OUTFMT_PDV_SHIFT) | wpf->outfmt;

		if (wpf->flip.active & BIT(WPF_CTRL_VFLIP))
			outfmt |= VI6_WPF_OUTFMT_FLP;
		if (wpf->flip.active & BIT(WPF_CTRL_HFLIP))
			outfmt |= VI6_WPF_OUTFMT_HFLP;

		vsp1_wpf_write(wpf, dl, VI6_WPF_OUTFMT, outfmt);
		return;
	}

	sink_format = vsp1_entity_get_pad_format(&wpf->entity,
						 wpf->entity.config,
						 RWPF_PAD_SINK);
	source_format = vsp1_entity_get_pad_format(&wpf->entity,
						   wpf->entity.config,
						   RWPF_PAD_SOURCE);

	if (params == VSP1_ENTITY_PARAMS_PARTITION) {
		const struct v4l2_pix_format_mplane *format = &wpf->format;
		struct vsp1_rwpf_memory mem = wpf->mem;
		unsigned int flip = wpf->flip.active;
		unsigned int width = source_format->width;
		unsigned int height = source_format->height;
		unsigned int offset;

		/*
		 * Cropping. The partition algorithm can split the image into
		 * multiple slices.
		 */
		if (pipe->partitions > 1)
			width = pipe->partition.width;

		vsp1_wpf_write(wpf, dl, VI6_WPF_HSZCLIP, VI6_WPF_SZCLIP_EN |
			       (0 << VI6_WPF_SZCLIP_OFST_SHIFT) |
			       (width << VI6_WPF_SZCLIP_SIZE_SHIFT));
		vsp1_wpf_write(wpf, dl, VI6_WPF_VSZCLIP, VI6_WPF_SZCLIP_EN |
			       (0 << VI6_WPF_SZCLIP_OFST_SHIFT) |
			       (height << VI6_WPF_SZCLIP_SIZE_SHIFT));

		if (pipe->lif)
			return;

		/*
		 * Update the memory offsets based on flipping configuration.
		 * The destination addresses point to the locations where the
		 * VSP starts writing to memory, which can be different corners
		 * of the image depending on vertical flipping.
		 */
		if (pipe->partitions > 1) {
			const struct vsp1_format_info *fmtinfo = wpf->fmtinfo;

			/*
			 * Horizontal flipping is handled through a line buffer
			 * and doesn't modify the start address, but still needs
			 * to be handled when image partitioning is in effect to
			 * order the partitions correctly.
			 */
			if (flip & BIT(WPF_CTRL_HFLIP))
				offset = format->width - pipe->partition.left
					- pipe->partition.width;
			else
				offset = pipe->partition.left;

			mem.addr[0] += offset * fmtinfo->bpp[0] / 8;
			if (format->num_planes > 1) {
				mem.addr[1] += offset / fmtinfo->hsub
					     * fmtinfo->bpp[1] / 8;
				mem.addr[2] += offset / fmtinfo->hsub
					     * fmtinfo->bpp[2] / 8;
			}
		}

		if (flip & BIT(WPF_CTRL_VFLIP)) {
			mem.addr[0] += (format->height - 1)
				     * format->plane_fmt[0].bytesperline;

			if (format->num_planes > 1) {
				offset = (format->height / wpf->fmtinfo->vsub - 1)
				       * format->plane_fmt[1].bytesperline;
				mem.addr[1] += offset;
				mem.addr[2] += offset;
			}
		}

		vsp1_wpf_write(wpf, dl, VI6_WPF_DSTM_ADDR_Y, mem.addr[0]);
		vsp1_wpf_write(wpf, dl, VI6_WPF_DSTM_ADDR_C0, mem.addr[1]);
		vsp1_wpf_write(wpf, dl, VI6_WPF_DSTM_ADDR_C1, mem.addr[2]);
		return;
	}

	/* Format */
	if (!pipe->lif) {
		const struct v4l2_pix_format_mplane *format = &wpf->format;
		const struct vsp1_format_info *fmtinfo = wpf->fmtinfo;
>>>>>>> v4.9.227

		outfmt = fmtinfo->hwfmt << VI6_WPF_OUTFMT_WRFMT_SHIFT;

		if (fmtinfo->alpha)
			outfmt |= VI6_WPF_OUTFMT_PXA;
		if (fmtinfo->swap_yc)
			outfmt |= VI6_WPF_OUTFMT_SPYCS;
		if (fmtinfo->swap_uv)
			outfmt |= VI6_WPF_OUTFMT_SPUVS;

<<<<<<< HEAD
		vsp1_wpf_write(wpf, VI6_WPF_DSWAP, fmtinfo->swap);
	}

	if (wpf->entity.formats[RWPF_PAD_SINK].code !=
	    wpf->entity.formats[RWPF_PAD_SOURCE].code)
		outfmt |= VI6_WPF_OUTFMT_CSC;

	/* Take the control handler lock to ensure that the PDV value won't be
	 * changed behind our back by a set control operation.
	 */
	mutex_lock(wpf->ctrls.lock);
	outfmt |= vsp1_wpf_read(wpf, VI6_WPF_OUTFMT) & VI6_WPF_OUTFMT_PDV_MASK;
	vsp1_wpf_write(wpf, VI6_WPF_OUTFMT, outfmt);
	mutex_unlock(wpf->ctrls.lock);

	vsp1_write(vsp1, VI6_DPR_WPF_FPORCH(wpf->entity.index),
		   VI6_DPR_WPF_FPORCH_FP_WPFN);

	vsp1_write(vsp1, VI6_WPF_WRBCK_CTRL, 0);

	/* Enable interrupts */
	vsp1_write(vsp1, VI6_WPF_IRQ_STA(wpf->entity.index), 0);
	vsp1_write(vsp1, VI6_WPF_IRQ_ENB(wpf->entity.index),
		   VI6_WFP_IRQ_ENB_FREE);

	return 0;
}

/* -----------------------------------------------------------------------------
 * V4L2 Subdevice Operations
 */

static struct v4l2_subdev_video_ops wpf_video_ops = {
	.s_stream = wpf_s_stream,
};

static struct v4l2_subdev_pad_ops wpf_pad_ops = {
	.enum_mbus_code = vsp1_rwpf_enum_mbus_code,
	.enum_frame_size = vsp1_rwpf_enum_frame_size,
	.get_fmt = vsp1_rwpf_get_format,
	.set_fmt = vsp1_rwpf_set_format,
	.get_selection = vsp1_rwpf_get_selection,
	.set_selection = vsp1_rwpf_set_selection,
};

static struct v4l2_subdev_ops wpf_ops = {
	.video	= &wpf_video_ops,
	.pad    = &wpf_pad_ops,
};

/* -----------------------------------------------------------------------------
 * Video Device Operations
 */

static void wpf_vdev_queue(struct vsp1_video *video,
			   struct vsp1_video_buffer *buf)
{
	struct vsp1_rwpf *wpf = container_of(video, struct vsp1_rwpf, video);

	vsp1_wpf_write(wpf, VI6_WPF_DSTM_ADDR_Y, buf->addr[0]);
	if (buf->buf.num_planes > 1)
		vsp1_wpf_write(wpf, VI6_WPF_DSTM_ADDR_C0, buf->addr[1]);
	if (buf->buf.num_planes > 2)
		vsp1_wpf_write(wpf, VI6_WPF_DSTM_ADDR_C1, buf->addr[2]);
}

static const struct vsp1_video_operations wpf_vdev_ops = {
	.queue = wpf_vdev_queue,
=======
		/* Destination stride and byte swapping. */
		vsp1_wpf_write(wpf, dl, VI6_WPF_DSTM_STRIDE_Y,
			       format->plane_fmt[0].bytesperline);
		if (format->num_planes > 1)
			vsp1_wpf_write(wpf, dl, VI6_WPF_DSTM_STRIDE_C,
				       format->plane_fmt[1].bytesperline);

		vsp1_wpf_write(wpf, dl, VI6_WPF_DSWAP, fmtinfo->swap);

		if (vsp1->info->features & VSP1_HAS_WPF_HFLIP &&
		    wpf->entity.index == 0)
			vsp1_wpf_write(wpf, dl, VI6_WPF_ROT_CTRL,
				       VI6_WPF_ROT_CTRL_LN16 |
				       (256 << VI6_WPF_ROT_CTRL_LMEM_WD_SHIFT));
	}

	if (sink_format->code != source_format->code)
		outfmt |= VI6_WPF_OUTFMT_CSC;

	wpf->outfmt = outfmt;

	vsp1_dl_list_write(dl, VI6_DPR_WPF_FPORCH(wpf->entity.index),
			   VI6_DPR_WPF_FPORCH_FP_WPFN);

	vsp1_dl_list_write(dl, VI6_WPF_WRBCK_CTRL, 0);

	/* Sources. If the pipeline has a single input and BRU is not used,
	 * configure it as the master layer. Otherwise configure all
	 * inputs as sub-layers and select the virtual RPF as the master
	 * layer.
	 */
	for (i = 0; i < vsp1->info->rpf_count; ++i) {
		struct vsp1_rwpf *input = pipe->inputs[i];

		if (!input)
			continue;

		srcrpf |= (!pipe->bru && pipe->num_inputs == 1)
			? VI6_WPF_SRCRPF_RPF_ACT_MST(input->entity.index)
			: VI6_WPF_SRCRPF_RPF_ACT_SUB(input->entity.index);
	}

	if (pipe->bru || pipe->num_inputs > 1)
		srcrpf |= VI6_WPF_SRCRPF_VIRACT_MST;

	vsp1_wpf_write(wpf, dl, VI6_WPF_SRCRPF, srcrpf);

	/* Enable interrupts */
	vsp1_dl_list_write(dl, VI6_WPF_IRQ_STA(wpf->entity.index), 0);
	vsp1_dl_list_write(dl, VI6_WPF_IRQ_ENB(wpf->entity.index),
			   VI6_WFP_IRQ_ENB_DFEE);
}

static const struct vsp1_entity_operations wpf_entity_ops = {
	.destroy = vsp1_wpf_destroy,
	.configure = wpf_configure,
>>>>>>> v4.9.227
};

/* -----------------------------------------------------------------------------
 * Initialization and Cleanup
 */

struct vsp1_rwpf *vsp1_wpf_create(struct vsp1_device *vsp1, unsigned int index)
{
<<<<<<< HEAD
	struct v4l2_subdev *subdev;
	struct vsp1_video *video;
	struct vsp1_rwpf *wpf;
	unsigned int flags;
=======
	struct vsp1_rwpf *wpf;
	char name[6];
>>>>>>> v4.9.227
	int ret;

	wpf = devm_kzalloc(vsp1->dev, sizeof(*wpf), GFP_KERNEL);
	if (wpf == NULL)
		return ERR_PTR(-ENOMEM);

<<<<<<< HEAD
	wpf->max_width = WPF_MAX_WIDTH;
	wpf->max_height = WPF_MAX_HEIGHT;

	wpf->entity.type = VSP1_ENTITY_WPF;
	wpf->entity.index = index;

	ret = vsp1_entity_init(vsp1, &wpf->entity, 2);
	if (ret < 0)
		return ERR_PTR(ret);

	/* Initialize the V4L2 subdev. */
	subdev = &wpf->entity.subdev;
	v4l2_subdev_init(subdev, &wpf_ops);

	subdev->entity.ops = &vsp1_media_ops;
	subdev->internal_ops = &vsp1_subdev_internal_ops;
	snprintf(subdev->name, sizeof(subdev->name), "%s wpf.%u",
		 dev_name(vsp1->dev), index);
	v4l2_set_subdevdata(subdev, wpf);
	subdev->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

	vsp1_entity_init_formats(subdev, NULL);

	/* Initialize the control handler. */
	v4l2_ctrl_handler_init(&wpf->ctrls, 1);
	v4l2_ctrl_new_std(&wpf->ctrls, &wpf_ctrl_ops, V4L2_CID_ALPHA_COMPONENT,
			  0, 255, 1, 255);

	wpf->entity.subdev.ctrl_handler = &wpf->ctrls;

	if (wpf->ctrls.error) {
		dev_err(vsp1->dev, "wpf%u: failed to initialize controls\n",
			index);
		ret = wpf->ctrls.error;
		goto error;
	}

	/* Initialize the video device. */
	video = &wpf->video;

	video->type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	video->vsp1 = vsp1;
	video->ops = &wpf_vdev_ops;

	ret = vsp1_video_init(video, &wpf->entity);
	if (ret < 0)
		goto error;

	wpf->entity.video = video;

	/* Connect the video device to the WPF. All connections are immutable
	 * except for the WPF0 source link if a LIF is present.
	 */
	flags = MEDIA_LNK_FL_ENABLED;
	if (!(vsp1->pdata->features & VSP1_HAS_LIF) || index != 0)
		flags |= MEDIA_LNK_FL_IMMUTABLE;

	ret = media_entity_create_link(&wpf->entity.subdev.entity,
				       RWPF_PAD_SOURCE,
				       &wpf->video.video.entity, 0, flags);
	if (ret < 0)
		goto error;

	wpf->entity.sink = &wpf->video.video.entity;
=======
	if (vsp1->info->gen == 2) {
		wpf->max_width = WPF_GEN2_MAX_WIDTH;
		wpf->max_height = WPF_GEN2_MAX_HEIGHT;
	} else {
		wpf->max_width = WPF_GEN3_MAX_WIDTH;
		wpf->max_height = WPF_GEN3_MAX_HEIGHT;
	}

	wpf->entity.ops = &wpf_entity_ops;
	wpf->entity.type = VSP1_ENTITY_WPF;
	wpf->entity.index = index;

	sprintf(name, "wpf.%u", index);
	ret = vsp1_entity_init(vsp1, &wpf->entity, name, 2, &wpf_ops,
			       MEDIA_ENT_F_PROC_VIDEO_PIXEL_FORMATTER);
	if (ret < 0)
		return ERR_PTR(ret);

	/* Initialize the display list manager. */
	wpf->dlm = vsp1_dlm_create(vsp1, index, 64);
	if (!wpf->dlm) {
		ret = -ENOMEM;
		goto error;
	}

	/* Initialize the control handler. */
	ret = wpf_init_controls(wpf);
	if (ret < 0) {
		dev_err(vsp1->dev, "wpf%u: failed to initialize controls\n",
			index);
		goto error;
	}

	v4l2_ctrl_handler_setup(&wpf->ctrls);
>>>>>>> v4.9.227

	return wpf;

error:
	vsp1_entity_destroy(&wpf->entity);
	return ERR_PTR(ret);
}
