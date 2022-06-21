/*
 * vsp1_lut.c  --  R-Car VSP1 Look-Up Table
 *
 * Copyright (C) 2013 Renesas Corporation
 *
 * Contact: Laurent Pinchart (laurent.pinchart@ideasonboard.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/device.h>
#include <linux/gfp.h>
<<<<<<< HEAD
#include <linux/vsp1.h>
=======
>>>>>>> v4.9.227

#include <media/v4l2-subdev.h>

#include "vsp1.h"
<<<<<<< HEAD
=======
#include "vsp1_dl.h"
>>>>>>> v4.9.227
#include "vsp1_lut.h"

#define LUT_MIN_SIZE				4U
#define LUT_MAX_SIZE				8190U

/* -----------------------------------------------------------------------------
 * Device Access
 */

<<<<<<< HEAD
static inline u32 vsp1_lut_read(struct vsp1_lut *lut, u32 reg)
{
	return vsp1_read(lut->entity.vsp1, reg);
}

static inline void vsp1_lut_write(struct vsp1_lut *lut, u32 reg, u32 data)
{
	vsp1_write(lut->entity.vsp1, reg, data);
}

/* -----------------------------------------------------------------------------
 * V4L2 Subdevice Core Operations
 */

static void lut_configure(struct vsp1_lut *lut, struct vsp1_lut_config *config)
{
	memcpy_toio(lut->entity.vsp1->mmio + VI6_LUT_TABLE, config->lut,
		    sizeof(config->lut));
}

static long lut_ioctl(struct v4l2_subdev *subdev, unsigned int cmd, void *arg)
{
	struct vsp1_lut *lut = to_lut(subdev);

	switch (cmd) {
	case VIDIOC_VSP1_LUT_CONFIG:
		lut_configure(lut, arg);
		return 0;

	default:
		return -ENOIOCTLCMD;
	}
}

/* -----------------------------------------------------------------------------
 * V4L2 Subdevice Video Operations
 */

static int lut_s_stream(struct v4l2_subdev *subdev, int enable)
{
	struct vsp1_lut *lut = to_lut(subdev);

	if (!enable)
		return 0;

	vsp1_lut_write(lut, VI6_LUT_CTRL, VI6_LUT_CTRL_EN);
=======
static inline void vsp1_lut_write(struct vsp1_lut *lut, struct vsp1_dl_list *dl,
				  u32 reg, u32 data)
{
	vsp1_dl_list_write(dl, reg, data);
}

/* -----------------------------------------------------------------------------
 * Controls
 */

#define V4L2_CID_VSP1_LUT_TABLE			(V4L2_CID_USER_BASE | 0x1001)

static int lut_set_table(struct vsp1_lut *lut, struct v4l2_ctrl *ctrl)
{
	struct vsp1_dl_body *dlb;
	unsigned int i;

	dlb = vsp1_dl_fragment_alloc(lut->entity.vsp1, 256);
	if (!dlb)
		return -ENOMEM;

	for (i = 0; i < 256; ++i)
		vsp1_dl_fragment_write(dlb, VI6_LUT_TABLE + 4 * i,
				       ctrl->p_new.p_u32[i]);

	spin_lock_irq(&lut->lock);
	swap(lut->lut, dlb);
	spin_unlock_irq(&lut->lock);

	vsp1_dl_fragment_free(dlb);
	return 0;
}

static int lut_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct vsp1_lut *lut =
		container_of(ctrl->handler, struct vsp1_lut, ctrls);

	switch (ctrl->id) {
	case V4L2_CID_VSP1_LUT_TABLE:
		lut_set_table(lut, ctrl);
		break;
	}
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
=======
static const struct v4l2_ctrl_ops lut_ctrl_ops = {
	.s_ctrl = lut_s_ctrl,
};

static const struct v4l2_ctrl_config lut_table_control = {
	.ops = &lut_ctrl_ops,
	.id = V4L2_CID_VSP1_LUT_TABLE,
	.name = "Look-Up Table",
	.type = V4L2_CTRL_TYPE_U32,
	.min = 0x00000000,
	.max = 0x00ffffff,
	.step = 1,
	.def = 0,
	.dims = { 256},
};

>>>>>>> v4.9.227
/* -----------------------------------------------------------------------------
 * V4L2 Subdevice Pad Operations
 */

static int lut_enum_mbus_code(struct v4l2_subdev *subdev,
<<<<<<< HEAD
			      struct v4l2_subdev_fh *fh,
			      struct v4l2_subdev_mbus_code_enum *code)
{
	static const unsigned int codes[] = {
		V4L2_MBUS_FMT_ARGB8888_1X32,
		V4L2_MBUS_FMT_AHSV8888_1X32,
		V4L2_MBUS_FMT_AYUV8_1X32,
	};
	struct v4l2_mbus_framefmt *format;

	if (code->pad == LUT_PAD_SINK) {
		if (code->index >= ARRAY_SIZE(codes))
			return -EINVAL;

		code->code = codes[code->index];
	} else {
		/* The LUT can't perform format conversion, the sink format is
		 * always identical to the source format.
		 */
		if (code->index)
			return -EINVAL;

		format = v4l2_subdev_get_try_format(fh, LUT_PAD_SINK);
		code->code = format->code;
	}

	return 0;
}

static int lut_enum_frame_size(struct v4l2_subdev *subdev,
			       struct v4l2_subdev_fh *fh,
			       struct v4l2_subdev_frame_size_enum *fse)
{
	struct v4l2_mbus_framefmt *format;

	format = v4l2_subdev_get_try_format(fh, fse->pad);

	if (fse->index || fse->code != format->code)
		return -EINVAL;

	if (fse->pad == LUT_PAD_SINK) {
		fse->min_width = LUT_MIN_SIZE;
		fse->max_width = LUT_MAX_SIZE;
		fse->min_height = LUT_MIN_SIZE;
		fse->max_height = LUT_MAX_SIZE;
	} else {
		/* The size on the source pad are fixed and always identical to
		 * the size on the sink pad.
		 */
		fse->min_width = format->width;
		fse->max_width = format->width;
		fse->min_height = format->height;
		fse->max_height = format->height;
	}

	return 0;
}

static int lut_get_format(struct v4l2_subdev *subdev, struct v4l2_subdev_fh *fh,
			  struct v4l2_subdev_format *fmt)
{
	struct vsp1_lut *lut = to_lut(subdev);

	fmt->format = *vsp1_entity_get_pad_format(&lut->entity, fh, fmt->pad,
						  fmt->which);

	return 0;
}

static int lut_set_format(struct v4l2_subdev *subdev, struct v4l2_subdev_fh *fh,
			  struct v4l2_subdev_format *fmt)
{
	struct vsp1_lut *lut = to_lut(subdev);
	struct v4l2_mbus_framefmt *format;

	/* Default to YUV if the requested format is not supported. */
	if (fmt->format.code != V4L2_MBUS_FMT_ARGB8888_1X32 &&
	    fmt->format.code != V4L2_MBUS_FMT_AHSV8888_1X32 &&
	    fmt->format.code != V4L2_MBUS_FMT_AYUV8_1X32)
		fmt->format.code = V4L2_MBUS_FMT_AYUV8_1X32;

	format = vsp1_entity_get_pad_format(&lut->entity, fh, fmt->pad,
					    fmt->which);
=======
			      struct v4l2_subdev_pad_config *cfg,
			      struct v4l2_subdev_mbus_code_enum *code)
{
	static const unsigned int codes[] = {
		MEDIA_BUS_FMT_ARGB8888_1X32,
		MEDIA_BUS_FMT_AHSV8888_1X32,
		MEDIA_BUS_FMT_AYUV8_1X32,
	};

	return vsp1_subdev_enum_mbus_code(subdev, cfg, code, codes,
					  ARRAY_SIZE(codes));
}

static int lut_enum_frame_size(struct v4l2_subdev *subdev,
			       struct v4l2_subdev_pad_config *cfg,
			       struct v4l2_subdev_frame_size_enum *fse)
{
	return vsp1_subdev_enum_frame_size(subdev, cfg, fse, LUT_MIN_SIZE,
					   LUT_MIN_SIZE, LUT_MAX_SIZE,
					   LUT_MAX_SIZE);
}

static int lut_set_format(struct v4l2_subdev *subdev,
			  struct v4l2_subdev_pad_config *cfg,
			  struct v4l2_subdev_format *fmt)
{
	struct vsp1_lut *lut = to_lut(subdev);
	struct v4l2_subdev_pad_config *config;
	struct v4l2_mbus_framefmt *format;
	int ret = 0;

	mutex_lock(&lut->entity.lock);

	config = vsp1_entity_get_pad_config(&lut->entity, cfg, fmt->which);
	if (!config) {
		ret = -EINVAL;
		goto done;
	}

	/* Default to YUV if the requested format is not supported. */
	if (fmt->format.code != MEDIA_BUS_FMT_ARGB8888_1X32 &&
	    fmt->format.code != MEDIA_BUS_FMT_AHSV8888_1X32 &&
	    fmt->format.code != MEDIA_BUS_FMT_AYUV8_1X32)
		fmt->format.code = MEDIA_BUS_FMT_AYUV8_1X32;

	format = vsp1_entity_get_pad_format(&lut->entity, config, fmt->pad);
>>>>>>> v4.9.227

	if (fmt->pad == LUT_PAD_SOURCE) {
		/* The LUT output format can't be modified. */
		fmt->format = *format;
<<<<<<< HEAD
		return 0;
	}

=======
		goto done;
	}

	format->code = fmt->format.code;
>>>>>>> v4.9.227
	format->width = clamp_t(unsigned int, fmt->format.width,
				LUT_MIN_SIZE, LUT_MAX_SIZE);
	format->height = clamp_t(unsigned int, fmt->format.height,
				 LUT_MIN_SIZE, LUT_MAX_SIZE);
	format->field = V4L2_FIELD_NONE;
	format->colorspace = V4L2_COLORSPACE_SRGB;

	fmt->format = *format;

	/* Propagate the format to the source pad. */
<<<<<<< HEAD
	format = vsp1_entity_get_pad_format(&lut->entity, fh, LUT_PAD_SOURCE,
					    fmt->which);
	*format = fmt->format;

	return 0;
=======
	format = vsp1_entity_get_pad_format(&lut->entity, config,
					    LUT_PAD_SOURCE);
	*format = fmt->format;

done:
	mutex_unlock(&lut->entity.lock);
	return ret;
>>>>>>> v4.9.227
}

/* -----------------------------------------------------------------------------
 * V4L2 Subdevice Operations
 */

<<<<<<< HEAD
static struct v4l2_subdev_core_ops lut_core_ops = {
	.ioctl = lut_ioctl,
};

static struct v4l2_subdev_video_ops lut_video_ops = {
	.s_stream = lut_s_stream,
};

static struct v4l2_subdev_pad_ops lut_pad_ops = {
	.enum_mbus_code = lut_enum_mbus_code,
	.enum_frame_size = lut_enum_frame_size,
	.get_fmt = lut_get_format,
	.set_fmt = lut_set_format,
};

static struct v4l2_subdev_ops lut_ops = {
	.core	= &lut_core_ops,
	.video	= &lut_video_ops,
=======
static const struct v4l2_subdev_pad_ops lut_pad_ops = {
	.init_cfg = vsp1_entity_init_cfg,
	.enum_mbus_code = lut_enum_mbus_code,
	.enum_frame_size = lut_enum_frame_size,
	.get_fmt = vsp1_subdev_get_pad_format,
	.set_fmt = lut_set_format,
};

static const struct v4l2_subdev_ops lut_ops = {
>>>>>>> v4.9.227
	.pad    = &lut_pad_ops,
};

/* -----------------------------------------------------------------------------
<<<<<<< HEAD
=======
 * VSP1 Entity Operations
 */

static void lut_configure(struct vsp1_entity *entity,
			  struct vsp1_pipeline *pipe,
			  struct vsp1_dl_list *dl,
			  enum vsp1_entity_params params)
{
	struct vsp1_lut *lut = to_lut(&entity->subdev);
	struct vsp1_dl_body *dlb;
	unsigned long flags;

	switch (params) {
	case VSP1_ENTITY_PARAMS_INIT:
		vsp1_lut_write(lut, dl, VI6_LUT_CTRL, VI6_LUT_CTRL_EN);
		break;

	case VSP1_ENTITY_PARAMS_PARTITION:
		break;

	case VSP1_ENTITY_PARAMS_RUNTIME:
		spin_lock_irqsave(&lut->lock, flags);
		dlb = lut->lut;
		lut->lut = NULL;
		spin_unlock_irqrestore(&lut->lock, flags);

		if (dlb)
			vsp1_dl_list_add_fragment(dl, dlb);
		break;
	}
}

static const struct vsp1_entity_operations lut_entity_ops = {
	.configure = lut_configure,
};

/* -----------------------------------------------------------------------------
>>>>>>> v4.9.227
 * Initialization and Cleanup
 */

struct vsp1_lut *vsp1_lut_create(struct vsp1_device *vsp1)
{
<<<<<<< HEAD
	struct v4l2_subdev *subdev;
=======
>>>>>>> v4.9.227
	struct vsp1_lut *lut;
	int ret;

	lut = devm_kzalloc(vsp1->dev, sizeof(*lut), GFP_KERNEL);
	if (lut == NULL)
		return ERR_PTR(-ENOMEM);

<<<<<<< HEAD
	lut->entity.type = VSP1_ENTITY_LUT;

	ret = vsp1_entity_init(vsp1, &lut->entity, 2);
	if (ret < 0)
		return ERR_PTR(ret);

	/* Initialize the V4L2 subdev. */
	subdev = &lut->entity.subdev;
	v4l2_subdev_init(subdev, &lut_ops);

	subdev->entity.ops = &vsp1_media_ops;
	subdev->internal_ops = &vsp1_subdev_internal_ops;
	snprintf(subdev->name, sizeof(subdev->name), "%s lut",
		 dev_name(vsp1->dev));
	v4l2_set_subdevdata(subdev, lut);
	subdev->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

	vsp1_entity_init_formats(subdev, NULL);
=======
	spin_lock_init(&lut->lock);

	lut->entity.ops = &lut_entity_ops;
	lut->entity.type = VSP1_ENTITY_LUT;

	ret = vsp1_entity_init(vsp1, &lut->entity, "lut", 2, &lut_ops,
			       MEDIA_ENT_F_PROC_VIDEO_LUT);
	if (ret < 0)
		return ERR_PTR(ret);

	/* Initialize the control handler. */
	v4l2_ctrl_handler_init(&lut->ctrls, 1);
	v4l2_ctrl_new_custom(&lut->ctrls, &lut_table_control, NULL);

	lut->entity.subdev.ctrl_handler = &lut->ctrls;

	if (lut->ctrls.error) {
		dev_err(vsp1->dev, "lut: failed to initialize controls\n");
		ret = lut->ctrls.error;
		vsp1_entity_destroy(&lut->entity);
		return ERR_PTR(ret);
	}

	v4l2_ctrl_handler_setup(&lut->ctrls);
>>>>>>> v4.9.227

	return lut;
}
