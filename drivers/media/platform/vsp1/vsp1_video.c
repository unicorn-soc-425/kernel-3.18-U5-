/*
 * vsp1_video.c  --  R-Car VSP1 Video Node
 *
<<<<<<< HEAD
 * Copyright (C) 2013-2014 Renesas Electronics Corporation
=======
 * Copyright (C) 2013-2015 Renesas Electronics Corporation
>>>>>>> v4.9.227
 *
 * Contact: Laurent Pinchart (laurent.pinchart@ideasonboard.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/list.h>
#include <linux/module.h>
#include <linux/mutex.h>
<<<<<<< HEAD
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/v4l2-mediabus.h>
#include <linux/videodev2.h>
=======
#include <linux/slab.h>
#include <linux/v4l2-mediabus.h>
#include <linux/videodev2.h>
#include <linux/wait.h>
>>>>>>> v4.9.227

#include <media/media-entity.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-subdev.h>
<<<<<<< HEAD
#include <media/videobuf2-core.h>
=======
#include <media/videobuf2-v4l2.h>
>>>>>>> v4.9.227
#include <media/videobuf2-dma-contig.h>

#include "vsp1.h"
#include "vsp1_bru.h"
<<<<<<< HEAD
#include "vsp1_entity.h"
=======
#include "vsp1_dl.h"
#include "vsp1_entity.h"
#include "vsp1_pipe.h"
>>>>>>> v4.9.227
#include "vsp1_rwpf.h"
#include "vsp1_uds.h"
#include "vsp1_video.h"

#define VSP1_VIDEO_DEF_FORMAT		V4L2_PIX_FMT_YUYV
#define VSP1_VIDEO_DEF_WIDTH		1024
#define VSP1_VIDEO_DEF_HEIGHT		768

#define VSP1_VIDEO_MIN_WIDTH		2U
#define VSP1_VIDEO_MAX_WIDTH		8190U
#define VSP1_VIDEO_MIN_HEIGHT		2U
#define VSP1_VIDEO_MAX_HEIGHT		8190U

/* -----------------------------------------------------------------------------
 * Helper functions
 */

<<<<<<< HEAD
static const struct vsp1_format_info vsp1_video_formats[] = {
	{ V4L2_PIX_FMT_RGB332, V4L2_MBUS_FMT_ARGB8888_1X32,
	  VI6_FMT_RGB_332, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  1, { 8, 0, 0 }, false, false, 1, 1, false },
	{ V4L2_PIX_FMT_ARGB444, V4L2_MBUS_FMT_ARGB8888_1X32,
	  VI6_FMT_ARGB_4444, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS,
	  1, { 16, 0, 0 }, false, false, 1, 1, true },
	{ V4L2_PIX_FMT_XRGB444, V4L2_MBUS_FMT_ARGB8888_1X32,
	  VI6_FMT_XRGB_4444, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS,
	  1, { 16, 0, 0 }, false, false, 1, 1, true },
	{ V4L2_PIX_FMT_ARGB555, V4L2_MBUS_FMT_ARGB8888_1X32,
	  VI6_FMT_ARGB_1555, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS,
	  1, { 16, 0, 0 }, false, false, 1, 1, true },
	{ V4L2_PIX_FMT_XRGB555, V4L2_MBUS_FMT_ARGB8888_1X32,
	  VI6_FMT_XRGB_1555, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS,
	  1, { 16, 0, 0 }, false, false, 1, 1, false },
	{ V4L2_PIX_FMT_RGB565, V4L2_MBUS_FMT_ARGB8888_1X32,
	  VI6_FMT_RGB_565, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS,
	  1, { 16, 0, 0 }, false, false, 1, 1, false },
	{ V4L2_PIX_FMT_BGR24, V4L2_MBUS_FMT_ARGB8888_1X32,
	  VI6_FMT_BGR_888, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  1, { 24, 0, 0 }, false, false, 1, 1, false },
	{ V4L2_PIX_FMT_RGB24, V4L2_MBUS_FMT_ARGB8888_1X32,
	  VI6_FMT_RGB_888, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  1, { 24, 0, 0 }, false, false, 1, 1, false },
	{ V4L2_PIX_FMT_ABGR32, V4L2_MBUS_FMT_ARGB8888_1X32,
	  VI6_FMT_ARGB_8888, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS,
	  1, { 32, 0, 0 }, false, false, 1, 1, true },
	{ V4L2_PIX_FMT_XBGR32, V4L2_MBUS_FMT_ARGB8888_1X32,
	  VI6_FMT_ARGB_8888, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS,
	  1, { 32, 0, 0 }, false, false, 1, 1, false },
	{ V4L2_PIX_FMT_ARGB32, V4L2_MBUS_FMT_ARGB8888_1X32,
	  VI6_FMT_ARGB_8888, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  1, { 32, 0, 0 }, false, false, 1, 1, true },
	{ V4L2_PIX_FMT_XRGB32, V4L2_MBUS_FMT_ARGB8888_1X32,
	  VI6_FMT_ARGB_8888, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  1, { 32, 0, 0 }, false, false, 1, 1, false },
	{ V4L2_PIX_FMT_UYVY, V4L2_MBUS_FMT_AYUV8_1X32,
	  VI6_FMT_YUYV_422, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  1, { 16, 0, 0 }, false, false, 2, 1, false },
	{ V4L2_PIX_FMT_VYUY, V4L2_MBUS_FMT_AYUV8_1X32,
	  VI6_FMT_YUYV_422, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  1, { 16, 0, 0 }, false, true, 2, 1, false },
	{ V4L2_PIX_FMT_YUYV, V4L2_MBUS_FMT_AYUV8_1X32,
	  VI6_FMT_YUYV_422, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  1, { 16, 0, 0 }, true, false, 2, 1, false },
	{ V4L2_PIX_FMT_YVYU, V4L2_MBUS_FMT_AYUV8_1X32,
	  VI6_FMT_YUYV_422, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  1, { 16, 0, 0 }, true, true, 2, 1, false },
	{ V4L2_PIX_FMT_NV12M, V4L2_MBUS_FMT_AYUV8_1X32,
	  VI6_FMT_Y_UV_420, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  2, { 8, 16, 0 }, false, false, 2, 2, false },
	{ V4L2_PIX_FMT_NV21M, V4L2_MBUS_FMT_AYUV8_1X32,
	  VI6_FMT_Y_UV_420, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  2, { 8, 16, 0 }, false, true, 2, 2, false },
	{ V4L2_PIX_FMT_NV16M, V4L2_MBUS_FMT_AYUV8_1X32,
	  VI6_FMT_Y_UV_422, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  2, { 8, 16, 0 }, false, false, 2, 1, false },
	{ V4L2_PIX_FMT_NV61M, V4L2_MBUS_FMT_AYUV8_1X32,
	  VI6_FMT_Y_UV_422, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  2, { 8, 16, 0 }, false, true, 2, 1, false },
	{ V4L2_PIX_FMT_YUV420M, V4L2_MBUS_FMT_AYUV8_1X32,
	  VI6_FMT_Y_U_V_420, VI6_RPF_DSWAP_P_LLS | VI6_RPF_DSWAP_P_LWS |
	  VI6_RPF_DSWAP_P_WDS | VI6_RPF_DSWAP_P_BTS,
	  3, { 8, 8, 8 }, false, false, 2, 2, false },
};

/*
 * vsp1_get_format_info - Retrieve format information for a 4CC
 * @fourcc: the format 4CC
 *
 * Return a pointer to the format information structure corresponding to the
 * given V4L2 format 4CC, or NULL if no corresponding format can be found.
 */
static const struct vsp1_format_info *vsp1_get_format_info(u32 fourcc)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(vsp1_video_formats); ++i) {
		const struct vsp1_format_info *info = &vsp1_video_formats[i];

		if (info->fourcc == fourcc)
			return info;
	}

	return NULL;
}


=======
>>>>>>> v4.9.227
static struct v4l2_subdev *
vsp1_video_remote_subdev(struct media_pad *local, u32 *pad)
{
	struct media_pad *remote;

	remote = media_entity_remote_pad(local);
<<<<<<< HEAD
	if (remote == NULL ||
	    media_entity_type(remote->entity) != MEDIA_ENT_T_V4L2_SUBDEV)
=======
	if (!remote || !is_media_entity_v4l2_subdev(remote->entity))
>>>>>>> v4.9.227
		return NULL;

	if (pad)
		*pad = remote->index;

	return media_entity_to_v4l2_subdev(remote->entity);
}

static int vsp1_video_verify_format(struct vsp1_video *video)
{
	struct v4l2_subdev_format fmt;
	struct v4l2_subdev *subdev;
	int ret;

	subdev = vsp1_video_remote_subdev(&video->pad, &fmt.pad);
	if (subdev == NULL)
		return -EINVAL;

	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	ret = v4l2_subdev_call(subdev, pad, get_fmt, NULL, &fmt);
	if (ret < 0)
		return ret == -ENOIOCTLCMD ? -EINVAL : ret;

<<<<<<< HEAD
	if (video->fmtinfo->mbus != fmt.format.code ||
	    video->format.height != fmt.format.height ||
	    video->format.width != fmt.format.width)
=======
	if (video->rwpf->fmtinfo->mbus != fmt.format.code ||
	    video->rwpf->format.height != fmt.format.height ||
	    video->rwpf->format.width != fmt.format.width)
>>>>>>> v4.9.227
		return -EINVAL;

	return 0;
}

static int __vsp1_video_try_format(struct vsp1_video *video,
				   struct v4l2_pix_format_mplane *pix,
				   const struct vsp1_format_info **fmtinfo)
{
	static const u32 xrgb_formats[][2] = {
		{ V4L2_PIX_FMT_RGB444, V4L2_PIX_FMT_XRGB444 },
		{ V4L2_PIX_FMT_RGB555, V4L2_PIX_FMT_XRGB555 },
		{ V4L2_PIX_FMT_BGR32, V4L2_PIX_FMT_XBGR32 },
		{ V4L2_PIX_FMT_RGB32, V4L2_PIX_FMT_XRGB32 },
	};

	const struct vsp1_format_info *info;
	unsigned int width = pix->width;
	unsigned int height = pix->height;
	unsigned int i;

	/* Backward compatibility: replace deprecated RGB formats by their XRGB
	 * equivalent. This selects the format older userspace applications want
	 * while still exposing the new format.
	 */
	for (i = 0; i < ARRAY_SIZE(xrgb_formats); ++i) {
		if (xrgb_formats[i][0] == pix->pixelformat) {
			pix->pixelformat = xrgb_formats[i][1];
			break;
		}
	}

	/* Retrieve format information and select the default format if the
	 * requested format isn't supported.
	 */
<<<<<<< HEAD
	info = vsp1_get_format_info(pix->pixelformat);
	if (info == NULL)
		info = vsp1_get_format_info(VSP1_VIDEO_DEF_FORMAT);
=======
	info = vsp1_get_format_info(video->vsp1, pix->pixelformat);
	if (info == NULL)
		info = vsp1_get_format_info(video->vsp1, VSP1_VIDEO_DEF_FORMAT);
>>>>>>> v4.9.227

	pix->pixelformat = info->fourcc;
	pix->colorspace = V4L2_COLORSPACE_SRGB;
	pix->field = V4L2_FIELD_NONE;
	memset(pix->reserved, 0, sizeof(pix->reserved));

	/* Align the width and height for YUV 4:2:2 and 4:2:0 formats. */
	width = round_down(width, info->hsub);
	height = round_down(height, info->vsub);

	/* Clamp the width and height. */
	pix->width = clamp(width, VSP1_VIDEO_MIN_WIDTH, VSP1_VIDEO_MAX_WIDTH);
	pix->height = clamp(height, VSP1_VIDEO_MIN_HEIGHT,
			    VSP1_VIDEO_MAX_HEIGHT);

	/* Compute and clamp the stride and image size. While not documented in
	 * the datasheet, strides not aligned to a multiple of 128 bytes result
	 * in image corruption.
	 */
<<<<<<< HEAD
	for (i = 0; i < max(info->planes, 2U); ++i) {
=======
	for (i = 0; i < min(info->planes, 2U); ++i) {
>>>>>>> v4.9.227
		unsigned int hsub = i > 0 ? info->hsub : 1;
		unsigned int vsub = i > 0 ? info->vsub : 1;
		unsigned int align = 128;
		unsigned int bpl;

		bpl = clamp_t(unsigned int, pix->plane_fmt[i].bytesperline,
			      pix->width / hsub * info->bpp[i] / 8,
			      round_down(65535U, align));

		pix->plane_fmt[i].bytesperline = round_up(bpl, align);
		pix->plane_fmt[i].sizeimage = pix->plane_fmt[i].bytesperline
					    * pix->height / vsub;
	}

	if (info->planes == 3) {
		/* The second and third planes must have the same stride. */
		pix->plane_fmt[2].bytesperline = pix->plane_fmt[1].bytesperline;
		pix->plane_fmt[2].sizeimage = pix->plane_fmt[1].sizeimage;
	}

	pix->num_planes = info->planes;

	if (fmtinfo)
		*fmtinfo = info;

	return 0;
}

<<<<<<< HEAD
static bool
vsp1_video_format_adjust(struct vsp1_video *video,
			 const struct v4l2_pix_format_mplane *format,
			 struct v4l2_pix_format_mplane *adjust)
{
	unsigned int i;

	*adjust = *format;
	__vsp1_video_try_format(video, adjust, NULL);

	if (format->width != adjust->width ||
	    format->height != adjust->height ||
	    format->pixelformat != adjust->pixelformat ||
	    format->num_planes != adjust->num_planes)
		return false;

	for (i = 0; i < format->num_planes; ++i) {
		if (format->plane_fmt[i].bytesperline !=
		    adjust->plane_fmt[i].bytesperline)
			return false;

		adjust->plane_fmt[i].sizeimage =
			max(adjust->plane_fmt[i].sizeimage,
			    format->plane_fmt[i].sizeimage);
	}

	return true;
=======
/* -----------------------------------------------------------------------------
 * VSP1 Partition Algorithm support
 */

static void vsp1_video_pipeline_setup_partitions(struct vsp1_pipeline *pipe)
{
	struct vsp1_device *vsp1 = pipe->output->entity.vsp1;
	const struct v4l2_mbus_framefmt *format;
	struct vsp1_entity *entity;
	unsigned int div_size;

	format = vsp1_entity_get_pad_format(&pipe->output->entity,
					    pipe->output->entity.config,
					    RWPF_PAD_SOURCE);
	div_size = format->width;

	/* Gen2 hardware doesn't require image partitioning. */
	if (vsp1->info->gen == 2) {
		pipe->div_size = div_size;
		pipe->partitions = 1;
		return;
	}

	list_for_each_entry(entity, &pipe->entities, list_pipe) {
		unsigned int entity_max = VSP1_VIDEO_MAX_WIDTH;

		if (entity->ops->max_width) {
			entity_max = entity->ops->max_width(entity, pipe);
			if (entity_max)
				div_size = min(div_size, entity_max);
		}
	}

	pipe->div_size = div_size;
	pipe->partitions = DIV_ROUND_UP(format->width, div_size);
}

/**
 * vsp1_video_partition - Calculate the active partition output window
 *
 * @div_size: pre-determined maximum partition division size
 * @index: partition index
 *
 * Returns a v4l2_rect describing the partition window.
 */
static struct v4l2_rect vsp1_video_partition(struct vsp1_pipeline *pipe,
					     unsigned int div_size,
					     unsigned int index)
{
	const struct v4l2_mbus_framefmt *format;
	struct v4l2_rect partition;
	unsigned int modulus;

	format = vsp1_entity_get_pad_format(&pipe->output->entity,
					    pipe->output->entity.config,
					    RWPF_PAD_SOURCE);

	/* A single partition simply processes the output size in full. */
	if (pipe->partitions <= 1) {
		partition.left = 0;
		partition.top = 0;
		partition.width = format->width;
		partition.height = format->height;
		return partition;
	}

	/* Initialise the partition with sane starting conditions. */
	partition.left = index * div_size;
	partition.top = 0;
	partition.width = div_size;
	partition.height = format->height;

	modulus = format->width % div_size;

	/*
	 * We need to prevent the last partition from being smaller than the
	 * *minimum* width of the hardware capabilities.
	 *
	 * If the modulus is less than half of the partition size,
	 * the penultimate partition is reduced to half, which is added
	 * to the final partition: |1234|1234|1234|12|341|
	 * to prevents this:       |1234|1234|1234|1234|1|.
	 */
	if (modulus) {
		/*
		 * pipe->partitions is 1 based, whilst index is a 0 based index.
		 * Normalise this locally.
		 */
		unsigned int partitions = pipe->partitions - 1;

		if (modulus < div_size / 2) {
			if (index == partitions - 1) {
				/* Halve the penultimate partition. */
				partition.width = div_size / 2;
			} else if (index == partitions) {
				/* Increase the final partition. */
				partition.width = (div_size / 2) + modulus;
				partition.left -= div_size / 2;
			}
		} else if (index == partitions) {
			partition.width = modulus;
		}
	}

	return partition;
>>>>>>> v4.9.227
}

/* -----------------------------------------------------------------------------
 * Pipeline Management
 */

<<<<<<< HEAD
static int vsp1_pipeline_validate_branch(struct vsp1_pipeline *pipe,
					 struct vsp1_rwpf *input,
					 struct vsp1_rwpf *output)
{
	struct vsp1_entity *entity;
	unsigned int entities = 0;
	struct media_pad *pad;
	bool bru_found = false;

	input->location.left = 0;
	input->location.top = 0;
=======
/*
 * vsp1_video_complete_buffer - Complete the current buffer
 * @video: the video node
 *
 * This function completes the current buffer by filling its sequence number,
 * time stamp and payload size, and hands it back to the videobuf core.
 *
 * When operating in DU output mode (deep pipeline to the DU through the LIF),
 * the VSP1 needs to constantly supply frames to the display. In that case, if
 * no other buffer is queued, reuse the one that has just been processed instead
 * of handing it back to the videobuf core.
 *
 * Return the next queued buffer or NULL if the queue is empty.
 */
static struct vsp1_vb2_buffer *
vsp1_video_complete_buffer(struct vsp1_video *video)
{
	struct vsp1_pipeline *pipe = video->rwpf->pipe;
	struct vsp1_vb2_buffer *next = NULL;
	struct vsp1_vb2_buffer *done;
	unsigned long flags;
	unsigned int i;

	spin_lock_irqsave(&video->irqlock, flags);

	if (list_empty(&video->irqqueue)) {
		spin_unlock_irqrestore(&video->irqlock, flags);
		return NULL;
	}

	done = list_first_entry(&video->irqqueue,
				struct vsp1_vb2_buffer, queue);

	/* In DU output mode reuse the buffer if the list is singular. */
	if (pipe->lif && list_is_singular(&video->irqqueue)) {
		spin_unlock_irqrestore(&video->irqlock, flags);
		return done;
	}

	list_del(&done->queue);

	if (!list_empty(&video->irqqueue))
		next = list_first_entry(&video->irqqueue,
					struct vsp1_vb2_buffer, queue);

	spin_unlock_irqrestore(&video->irqlock, flags);

	done->buf.sequence = pipe->sequence;
	done->buf.vb2_buf.timestamp = ktime_get_ns();
	for (i = 0; i < done->buf.vb2_buf.num_planes; ++i)
		vb2_set_plane_payload(&done->buf.vb2_buf, i,
				      vb2_plane_size(&done->buf.vb2_buf, i));
	vb2_buffer_done(&done->buf.vb2_buf, VB2_BUF_STATE_DONE);

	return next;
}

static void vsp1_video_frame_end(struct vsp1_pipeline *pipe,
				 struct vsp1_rwpf *rwpf)
{
	struct vsp1_video *video = rwpf->video;
	struct vsp1_vb2_buffer *buf;

	buf = vsp1_video_complete_buffer(video);
	if (buf == NULL)
		return;

	video->rwpf->mem = buf->mem;
	pipe->buffers_ready |= 1 << video->pipe_index;
}

static void vsp1_video_pipeline_run_partition(struct vsp1_pipeline *pipe,
					      struct vsp1_dl_list *dl)
{
	struct vsp1_entity *entity;

	pipe->partition = vsp1_video_partition(pipe, pipe->div_size,
					       pipe->current_partition);

	list_for_each_entry(entity, &pipe->entities, list_pipe) {
		if (entity->ops->configure)
			entity->ops->configure(entity, pipe, dl,
					       VSP1_ENTITY_PARAMS_PARTITION);
	}
}

static void vsp1_video_pipeline_run(struct vsp1_pipeline *pipe)
{
	struct vsp1_device *vsp1 = pipe->output->entity.vsp1;
	struct vsp1_entity *entity;

	if (!pipe->dl)
		pipe->dl = vsp1_dl_list_get(pipe->output->dlm);

	/*
	 * Start with the runtime parameters as the configure operation can
	 * compute/cache information needed when configuring partitions. This
	 * is the case with flipping in the WPF.
	 */
	list_for_each_entry(entity, &pipe->entities, list_pipe) {
		if (entity->ops->configure)
			entity->ops->configure(entity, pipe, pipe->dl,
					       VSP1_ENTITY_PARAMS_RUNTIME);
	}

	/* Run the first partition */
	pipe->current_partition = 0;
	vsp1_video_pipeline_run_partition(pipe, pipe->dl);

	/* Process consecutive partitions as necessary */
	for (pipe->current_partition = 1;
	     pipe->current_partition < pipe->partitions;
	     pipe->current_partition++) {
		struct vsp1_dl_list *dl;

		/*
		 * Partition configuration operations will utilise
		 * the pipe->current_partition variable to determine
		 * the work they should complete.
		 */
		dl = vsp1_dl_list_get(pipe->output->dlm);

		/*
		 * An incomplete chain will still function, but output only
		 * the partitions that had a dl available. The frame end
		 * interrupt will be marked on the last dl in the chain.
		 */
		if (!dl) {
			dev_err(vsp1->dev, "Failed to obtain a dl list. Frame will be incomplete\n");
			break;
		}

		vsp1_video_pipeline_run_partition(pipe, dl);
		vsp1_dl_list_add_chain(pipe->dl, dl);
	}

	/* Complete, and commit the head display list. */
	vsp1_dl_list_commit(pipe->dl);
	pipe->dl = NULL;

	vsp1_pipeline_run(pipe);
}

static void vsp1_video_pipeline_frame_end(struct vsp1_pipeline *pipe)
{
	struct vsp1_device *vsp1 = pipe->output->entity.vsp1;
	enum vsp1_pipeline_state state;
	unsigned long flags;
	unsigned int i;

	spin_lock_irqsave(&pipe->irqlock, flags);

	/* Complete buffers on all video nodes. */
	for (i = 0; i < vsp1->info->rpf_count; ++i) {
		if (!pipe->inputs[i])
			continue;

		vsp1_video_frame_end(pipe, pipe->inputs[i]);
	}

	vsp1_video_frame_end(pipe, pipe->output);

	state = pipe->state;
	pipe->state = VSP1_PIPELINE_STOPPED;

	/* If a stop has been requested, mark the pipeline as stopped and
	 * return. Otherwise restart the pipeline if ready.
	 */
	if (state == VSP1_PIPELINE_STOPPING)
		wake_up(&pipe->wq);
	else if (vsp1_pipeline_ready(pipe))
		vsp1_video_pipeline_run(pipe);

	spin_unlock_irqrestore(&pipe->irqlock, flags);
}

static int vsp1_video_pipeline_build_branch(struct vsp1_pipeline *pipe,
					    struct vsp1_rwpf *input,
					    struct vsp1_rwpf *output)
{
	struct media_entity_enum ent_enum;
	struct vsp1_entity *entity;
	struct media_pad *pad;
	bool bru_found = false;
	int ret;

	ret = media_entity_enum_init(&ent_enum, &input->entity.vsp1->media_dev);
	if (ret < 0)
		return ret;
>>>>>>> v4.9.227

	pad = media_entity_remote_pad(&input->entity.pads[RWPF_PAD_SOURCE]);

	while (1) {
<<<<<<< HEAD
		if (pad == NULL)
			return -EPIPE;

		/* We've reached a video node, that shouldn't have happened. */
		if (media_entity_type(pad->entity) != MEDIA_ENT_T_V4L2_SUBDEV)
			return -EPIPE;

		entity = to_vsp1_entity(media_entity_to_v4l2_subdev(pad->entity));

		/* A BRU is present in the pipeline, store the compose rectangle
		 * location in the input RPF for use when configuring the RPF.
		 */
		if (entity->type == VSP1_ENTITY_BRU) {
			struct vsp1_bru *bru = to_bru(&entity->subdev);
			struct v4l2_rect *rect =
				&bru->inputs[pad->index].compose;

			bru->inputs[pad->index].rpf = input;

			input->location.left = rect->left;
			input->location.top = rect->top;
=======
		if (pad == NULL) {
			ret = -EPIPE;
			goto out;
		}

		/* We've reached a video node, that shouldn't have happened. */
		if (!is_media_entity_v4l2_subdev(pad->entity)) {
			ret = -EPIPE;
			goto out;
		}

		entity = to_vsp1_entity(
			media_entity_to_v4l2_subdev(pad->entity));

		/* A BRU is present in the pipeline, store the BRU input pad
		 * number in the input RPF for use when configuring the RPF.
		 */
		if (entity->type == VSP1_ENTITY_BRU) {
			struct vsp1_bru *bru = to_bru(&entity->subdev);

			bru->inputs[pad->index].rpf = input;
			input->bru_input = pad->index;
>>>>>>> v4.9.227

			bru_found = true;
		}

		/* We've reached the WPF, we're done. */
		if (entity->type == VSP1_ENTITY_WPF)
			break;

		/* Ensure the branch has no loop. */
<<<<<<< HEAD
		if (entities & (1 << entity->subdev.entity.id))
			return -EPIPE;

		entities |= 1 << entity->subdev.entity.id;

		/* UDS can't be chained. */
		if (entity->type == VSP1_ENTITY_UDS) {
			if (pipe->uds)
				return -EPIPE;
=======
		if (media_entity_enum_test_and_set(&ent_enum,
						   &entity->subdev.entity)) {
			ret = -EPIPE;
			goto out;
		}

		/* UDS can't be chained. */
		if (entity->type == VSP1_ENTITY_UDS) {
			if (pipe->uds) {
				ret = -EPIPE;
				goto out;
			}
>>>>>>> v4.9.227

			pipe->uds = entity;
			pipe->uds_input = bru_found ? pipe->bru
					: &input->entity;
		}

		/* Follow the source link. The link setup operations ensure
		 * that the output fan-out can't be more than one, there is thus
		 * no need to verify here that only a single source link is
		 * activated.
		 */
		pad = &entity->pads[entity->source_pad];
		pad = media_entity_remote_pad(pad);
	}

	/* The last entity must be the output WPF. */
	if (entity != &output->entity)
<<<<<<< HEAD
		return -EPIPE;

	return 0;
}

static void __vsp1_pipeline_cleanup(struct vsp1_pipeline *pipe)
{
	if (pipe->bru) {
		struct vsp1_bru *bru = to_bru(&pipe->bru->subdev);
		unsigned int i;

		for (i = 0; i < ARRAY_SIZE(bru->inputs); ++i)
			bru->inputs[i].rpf = NULL;
	}

	INIT_LIST_HEAD(&pipe->entities);
	pipe->state = VSP1_PIPELINE_STOPPED;
	pipe->buffers_ready = 0;
	pipe->num_video = 0;
	pipe->num_inputs = 0;
	pipe->output = NULL;
	pipe->bru = NULL;
	pipe->lif = NULL;
	pipe->uds = NULL;
}

static int vsp1_pipeline_validate(struct vsp1_pipeline *pipe,
				  struct vsp1_video *video)
{
	struct media_entity_graph graph;
	struct media_entity *entity = &video->video.entity;
	struct media_device *mdev = entity->parent;
	unsigned int i;
	int ret;

	mutex_lock(&mdev->graph_mutex);

	/* Walk the graph to locate the entities and video nodes. */
=======
		ret = -EPIPE;

out:
	media_entity_enum_cleanup(&ent_enum);

	return ret;
}

static int vsp1_video_pipeline_build(struct vsp1_pipeline *pipe,
				     struct vsp1_video *video)
{
	struct media_entity_graph graph;
	struct media_entity *entity = &video->video.entity;
	struct media_device *mdev = entity->graph_obj.mdev;
	unsigned int i;
	int ret;

	/* Walk the graph to locate the entities and video nodes. */
	ret = media_entity_graph_walk_init(&graph, mdev);
	if (ret)
		return ret;

>>>>>>> v4.9.227
	media_entity_graph_walk_start(&graph, entity);

	while ((entity = media_entity_graph_walk_next(&graph))) {
		struct v4l2_subdev *subdev;
		struct vsp1_rwpf *rwpf;
		struct vsp1_entity *e;

<<<<<<< HEAD
		if (media_entity_type(entity) != MEDIA_ENT_T_V4L2_SUBDEV) {
			pipe->num_video++;
			continue;
		}
=======
		if (!is_media_entity_v4l2_subdev(entity))
			continue;
>>>>>>> v4.9.227

		subdev = media_entity_to_v4l2_subdev(entity);
		e = to_vsp1_entity(subdev);
		list_add_tail(&e->list_pipe, &pipe->entities);

		if (e->type == VSP1_ENTITY_RPF) {
			rwpf = to_rwpf(subdev);
<<<<<<< HEAD
			pipe->inputs[pipe->num_inputs++] = rwpf;
			rwpf->video.pipe_index = pipe->num_inputs;
		} else if (e->type == VSP1_ENTITY_WPF) {
			rwpf = to_rwpf(subdev);
			pipe->output = to_rwpf(subdev);
			rwpf->video.pipe_index = 0;
=======
			pipe->inputs[rwpf->entity.index] = rwpf;
			rwpf->video->pipe_index = ++pipe->num_inputs;
			rwpf->pipe = pipe;
		} else if (e->type == VSP1_ENTITY_WPF) {
			rwpf = to_rwpf(subdev);
			pipe->output = rwpf;
			rwpf->video->pipe_index = 0;
			rwpf->pipe = pipe;
>>>>>>> v4.9.227
		} else if (e->type == VSP1_ENTITY_LIF) {
			pipe->lif = e;
		} else if (e->type == VSP1_ENTITY_BRU) {
			pipe->bru = e;
		}
	}

<<<<<<< HEAD
	mutex_unlock(&mdev->graph_mutex);

	/* We need one output and at least one input. */
	if (pipe->num_inputs == 0 || !pipe->output) {
		ret = -EPIPE;
		goto error;
	}
=======
	media_entity_graph_walk_cleanup(&graph);

	/* We need one output and at least one input. */
	if (pipe->num_inputs == 0 || !pipe->output)
		return -EPIPE;
>>>>>>> v4.9.227

	/* Follow links downstream for each input and make sure the graph
	 * contains no loop and that all branches end at the output WPF.
	 */
<<<<<<< HEAD
	for (i = 0; i < pipe->num_inputs; ++i) {
		ret = vsp1_pipeline_validate_branch(pipe, pipe->inputs[i],
						    pipe->output);
		if (ret < 0)
			goto error;
	}

	return 0;

error:
	__vsp1_pipeline_cleanup(pipe);
	return ret;
}

static int vsp1_pipeline_init(struct vsp1_pipeline *pipe,
			      struct vsp1_video *video)
{
	int ret;

	mutex_lock(&pipe->lock);

	/* If we're the first user validate and initialize the pipeline. */
	if (pipe->use_count == 0) {
		ret = vsp1_pipeline_validate(pipe, video);
		if (ret < 0)
			goto done;
	}

	pipe->use_count++;
	ret = 0;

done:
	mutex_unlock(&pipe->lock);
	return ret;
}

static void vsp1_pipeline_cleanup(struct vsp1_pipeline *pipe)
{
	mutex_lock(&pipe->lock);

	/* If we're the last user clean up the pipeline. */
	if (--pipe->use_count == 0)
		__vsp1_pipeline_cleanup(pipe);

	mutex_unlock(&pipe->lock);
}

static void vsp1_pipeline_run(struct vsp1_pipeline *pipe)
{
	struct vsp1_device *vsp1 = pipe->output->entity.vsp1;

	vsp1_write(vsp1, VI6_CMD(pipe->output->entity.index), VI6_CMD_STRCMD);
	pipe->state = VSP1_PIPELINE_RUNNING;
	pipe->buffers_ready = 0;
}

static int vsp1_pipeline_stop(struct vsp1_pipeline *pipe)
{
	struct vsp1_entity *entity;
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&pipe->irqlock, flags);
	if (pipe->state == VSP1_PIPELINE_RUNNING)
		pipe->state = VSP1_PIPELINE_STOPPING;
	spin_unlock_irqrestore(&pipe->irqlock, flags);

	ret = wait_event_timeout(pipe->wq, pipe->state == VSP1_PIPELINE_STOPPED,
				 msecs_to_jiffies(500));
	ret = ret == 0 ? -ETIMEDOUT : 0;

	list_for_each_entry(entity, &pipe->entities, list_pipe) {
		if (entity->route && entity->route->reg)
			vsp1_write(entity->vsp1, entity->route->reg,
				   VI6_DPR_NODE_UNUSED);

		v4l2_subdev_call(&entity->subdev, video, s_stream, 0);
	}

	return ret;
}

static bool vsp1_pipeline_ready(struct vsp1_pipeline *pipe)
{
	unsigned int mask;

	mask = ((1 << pipe->num_inputs) - 1) << 1;
	if (!pipe->lif)
		mask |= 1 << 0;

	return pipe->buffers_ready == mask;
}

/*
 * vsp1_video_complete_buffer - Complete the current buffer
 * @video: the video node
 *
 * This function completes the current buffer by filling its sequence number,
 * time stamp and payload size, and hands it back to the videobuf core.
 *
 * When operating in DU output mode (deep pipeline to the DU through the LIF),
 * the VSP1 needs to constantly supply frames to the display. In that case, if
 * no other buffer is queued, reuse the one that has just been processed instead
 * of handing it back to the videobuf core.
 *
 * Return the next queued buffer or NULL if the queue is empty.
 */
static struct vsp1_video_buffer *
vsp1_video_complete_buffer(struct vsp1_video *video)
{
	struct vsp1_pipeline *pipe = to_vsp1_pipeline(&video->video.entity);
	struct vsp1_video_buffer *next = NULL;
	struct vsp1_video_buffer *done;
	unsigned long flags;
	unsigned int i;

	spin_lock_irqsave(&video->irqlock, flags);

	if (list_empty(&video->irqqueue)) {
		spin_unlock_irqrestore(&video->irqlock, flags);
		return NULL;
	}

	done = list_first_entry(&video->irqqueue,
				struct vsp1_video_buffer, queue);

	/* In DU output mode reuse the buffer if the list is singular. */
	if (pipe->lif && list_is_singular(&video->irqqueue)) {
		spin_unlock_irqrestore(&video->irqlock, flags);
		return done;
	}

	list_del(&done->queue);

	if (!list_empty(&video->irqqueue))
		next = list_first_entry(&video->irqqueue,
					struct vsp1_video_buffer, queue);

	spin_unlock_irqrestore(&video->irqlock, flags);

	done->buf.v4l2_buf.sequence = video->sequence++;
	v4l2_get_timestamp(&done->buf.v4l2_buf.timestamp);
	for (i = 0; i < done->buf.num_planes; ++i)
		vb2_set_plane_payload(&done->buf, i, done->length[i]);
	vb2_buffer_done(&done->buf, VB2_BUF_STATE_DONE);

	return next;
}

static void vsp1_video_frame_end(struct vsp1_pipeline *pipe,
				 struct vsp1_video *video)
{
	struct vsp1_video_buffer *buf;
	unsigned long flags;

	buf = vsp1_video_complete_buffer(video);
	if (buf == NULL)
		return;

	spin_lock_irqsave(&pipe->irqlock, flags);

	video->ops->queue(video, buf);
	pipe->buffers_ready |= 1 << video->pipe_index;

	spin_unlock_irqrestore(&pipe->irqlock, flags);
}

void vsp1_pipeline_frame_end(struct vsp1_pipeline *pipe)
{
	enum vsp1_pipeline_state state;
	unsigned long flags;
	unsigned int i;

	if (pipe == NULL)
		return;

	/* Complete buffers on all video nodes. */
	for (i = 0; i < pipe->num_inputs; ++i)
		vsp1_video_frame_end(pipe, &pipe->inputs[i]->video);

	if (!pipe->lif)
		vsp1_video_frame_end(pipe, &pipe->output->video);

	spin_lock_irqsave(&pipe->irqlock, flags);

	state = pipe->state;
	pipe->state = VSP1_PIPELINE_STOPPED;

	/* If a stop has been requested, mark the pipeline as stopped and
	 * return.
	 */
	if (state == VSP1_PIPELINE_STOPPING) {
		wake_up(&pipe->wq);
		goto done;
	}

	/* Restart the pipeline if ready. */
	if (vsp1_pipeline_ready(pipe))
		vsp1_pipeline_run(pipe);

done:
	spin_unlock_irqrestore(&pipe->irqlock, flags);
}

/*
 * Propagate the alpha value through the pipeline.
 *
 * As the UDS has restricted scaling capabilities when the alpha component needs
 * to be scaled, we disable alpha scaling when the UDS input has a fixed alpha
 * value. The UDS then outputs a fixed alpha value which needs to be programmed
 * from the input RPF alpha.
 */
void vsp1_pipeline_propagate_alpha(struct vsp1_pipeline *pipe,
				   struct vsp1_entity *input,
				   unsigned int alpha)
{
	struct vsp1_entity *entity;
	struct media_pad *pad;

	pad = media_entity_remote_pad(&input->pads[RWPF_PAD_SOURCE]);

	while (pad) {
		if (media_entity_type(pad->entity) != MEDIA_ENT_T_V4L2_SUBDEV)
			break;

		entity = to_vsp1_entity(media_entity_to_v4l2_subdev(pad->entity));

		/* The BRU background color has a fixed alpha value set to 255,
		 * the output alpha value is thus always equal to 255.
		 */
		if (entity->type == VSP1_ENTITY_BRU)
			alpha = 255;

		if (entity->type == VSP1_ENTITY_UDS) {
			struct vsp1_uds *uds = to_uds(&entity->subdev);

			vsp1_uds_set_alpha(uds, alpha);
			break;
		}

		pad = &entity->pads[entity->source_pad];
		pad = media_entity_remote_pad(pad);
	}
=======
	for (i = 0; i < video->vsp1->info->rpf_count; ++i) {
		if (!pipe->inputs[i])
			continue;

		ret = vsp1_video_pipeline_build_branch(pipe, pipe->inputs[i],
						       pipe->output);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static int vsp1_video_pipeline_init(struct vsp1_pipeline *pipe,
				    struct vsp1_video *video)
{
	vsp1_pipeline_init(pipe);

	pipe->frame_end = vsp1_video_pipeline_frame_end;

	return vsp1_video_pipeline_build(pipe, video);
}

static struct vsp1_pipeline *vsp1_video_pipeline_get(struct vsp1_video *video)
{
	struct vsp1_pipeline *pipe;
	int ret;

	/* Get a pipeline object for the video node. If a pipeline has already
	 * been allocated just increment its reference count and return it.
	 * Otherwise allocate a new pipeline and initialize it, it will be freed
	 * when the last reference is released.
	 */
	if (!video->rwpf->pipe) {
		pipe = kzalloc(sizeof(*pipe), GFP_KERNEL);
		if (!pipe)
			return ERR_PTR(-ENOMEM);

		ret = vsp1_video_pipeline_init(pipe, video);
		if (ret < 0) {
			vsp1_pipeline_reset(pipe);
			kfree(pipe);
			return ERR_PTR(ret);
		}
	} else {
		pipe = video->rwpf->pipe;
		kref_get(&pipe->kref);
	}

	return pipe;
}

static void vsp1_video_pipeline_release(struct kref *kref)
{
	struct vsp1_pipeline *pipe = container_of(kref, typeof(*pipe), kref);

	vsp1_pipeline_reset(pipe);
	kfree(pipe);
}

static void vsp1_video_pipeline_put(struct vsp1_pipeline *pipe)
{
	struct media_device *mdev = &pipe->output->entity.vsp1->media_dev;

	mutex_lock(&mdev->graph_mutex);
	kref_put(&pipe->kref, vsp1_video_pipeline_release);
	mutex_unlock(&mdev->graph_mutex);
>>>>>>> v4.9.227
}

/* -----------------------------------------------------------------------------
 * videobuf2 Queue Operations
 */

static int
<<<<<<< HEAD
vsp1_video_queue_setup(struct vb2_queue *vq, const struct v4l2_format *fmt,
		     unsigned int *nbuffers, unsigned int *nplanes,
		     unsigned int sizes[], void *alloc_ctxs[])
{
	struct vsp1_video *video = vb2_get_drv_priv(vq);
	const struct v4l2_pix_format_mplane *format;
	struct v4l2_pix_format_mplane pix_mp;
	unsigned int i;

	if (fmt) {
		/* Make sure the format is valid and adjust the sizeimage field
		 * if needed.
		 */
		if (!vsp1_video_format_adjust(video, &fmt->fmt.pix_mp, &pix_mp))
			return -EINVAL;

		format = &pix_mp;
	} else {
		format = &video->format;
=======
vsp1_video_queue_setup(struct vb2_queue *vq,
		       unsigned int *nbuffers, unsigned int *nplanes,
		       unsigned int sizes[], struct device *alloc_devs[])
{
	struct vsp1_video *video = vb2_get_drv_priv(vq);
	const struct v4l2_pix_format_mplane *format = &video->rwpf->format;
	unsigned int i;

	if (*nplanes) {
		if (*nplanes != format->num_planes)
			return -EINVAL;

		for (i = 0; i < *nplanes; i++)
			if (sizes[i] < format->plane_fmt[i].sizeimage)
				return -EINVAL;
		return 0;
>>>>>>> v4.9.227
	}

	*nplanes = format->num_planes;

<<<<<<< HEAD
	for (i = 0; i < format->num_planes; ++i) {
		sizes[i] = format->plane_fmt[i].sizeimage;
		alloc_ctxs[i] = video->alloc_ctx;
	}
=======
	for (i = 0; i < format->num_planes; ++i)
		sizes[i] = format->plane_fmt[i].sizeimage;
>>>>>>> v4.9.227

	return 0;
}

static int vsp1_video_buffer_prepare(struct vb2_buffer *vb)
{
<<<<<<< HEAD
	struct vsp1_video *video = vb2_get_drv_priv(vb->vb2_queue);
	struct vsp1_video_buffer *buf = to_vsp1_video_buffer(vb);
	const struct v4l2_pix_format_mplane *format = &video->format;
=======
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct vsp1_video *video = vb2_get_drv_priv(vb->vb2_queue);
	struct vsp1_vb2_buffer *buf = to_vsp1_vb2_buffer(vbuf);
	const struct v4l2_pix_format_mplane *format = &video->rwpf->format;
>>>>>>> v4.9.227
	unsigned int i;

	if (vb->num_planes < format->num_planes)
		return -EINVAL;

	for (i = 0; i < vb->num_planes; ++i) {
<<<<<<< HEAD
		buf->addr[i] = vb2_dma_contig_plane_dma_addr(vb, i);
		buf->length[i] = vb2_plane_size(vb, i);

		if (buf->length[i] < format->plane_fmt[i].sizeimage)
			return -EINVAL;
	}

=======
		buf->mem.addr[i] = vb2_dma_contig_plane_dma_addr(vb, i);

		if (vb2_plane_size(vb, i) < format->plane_fmt[i].sizeimage)
			return -EINVAL;
	}

	for ( ; i < 3; ++i)
		buf->mem.addr[i] = 0;

>>>>>>> v4.9.227
	return 0;
}

static void vsp1_video_buffer_queue(struct vb2_buffer *vb)
{
<<<<<<< HEAD
	struct vsp1_video *video = vb2_get_drv_priv(vb->vb2_queue);
	struct vsp1_pipeline *pipe = to_vsp1_pipeline(&video->video.entity);
	struct vsp1_video_buffer *buf = to_vsp1_video_buffer(vb);
=======
	struct vb2_v4l2_buffer *vbuf = to_vb2_v4l2_buffer(vb);
	struct vsp1_video *video = vb2_get_drv_priv(vb->vb2_queue);
	struct vsp1_pipeline *pipe = video->rwpf->pipe;
	struct vsp1_vb2_buffer *buf = to_vsp1_vb2_buffer(vbuf);
>>>>>>> v4.9.227
	unsigned long flags;
	bool empty;

	spin_lock_irqsave(&video->irqlock, flags);
	empty = list_empty(&video->irqqueue);
	list_add_tail(&buf->queue, &video->irqqueue);
	spin_unlock_irqrestore(&video->irqlock, flags);

	if (!empty)
		return;

	spin_lock_irqsave(&pipe->irqlock, flags);

<<<<<<< HEAD
	video->ops->queue(video, buf);
=======
	video->rwpf->mem = buf->mem;
>>>>>>> v4.9.227
	pipe->buffers_ready |= 1 << video->pipe_index;

	if (vb2_is_streaming(&video->queue) &&
	    vsp1_pipeline_ready(pipe))
<<<<<<< HEAD
		vsp1_pipeline_run(pipe);
=======
		vsp1_video_pipeline_run(pipe);
>>>>>>> v4.9.227

	spin_unlock_irqrestore(&pipe->irqlock, flags);
}

<<<<<<< HEAD
static void vsp1_entity_route_setup(struct vsp1_entity *source)
{
	struct vsp1_entity *sink;

	if (source->route->reg == 0)
		return;

	sink = container_of(source->sink, struct vsp1_entity, subdev.entity);
	vsp1_write(source->vsp1, source->route->reg,
		   sink->route->inputs[source->sink_pad]);
=======
static int vsp1_video_setup_pipeline(struct vsp1_pipeline *pipe)
{
	struct vsp1_entity *entity;

	/* Determine this pipelines sizes for image partitioning support. */
	vsp1_video_pipeline_setup_partitions(pipe);

	/* Prepare the display list. */
	pipe->dl = vsp1_dl_list_get(pipe->output->dlm);
	if (!pipe->dl)
		return -ENOMEM;

	if (pipe->uds) {
		struct vsp1_uds *uds = to_uds(&pipe->uds->subdev);

		/* If a BRU is present in the pipeline before the UDS, the alpha
		 * component doesn't need to be scaled as the BRU output alpha
		 * value is fixed to 255. Otherwise we need to scale the alpha
		 * component only when available at the input RPF.
		 */
		if (pipe->uds_input->type == VSP1_ENTITY_BRU) {
			uds->scale_alpha = false;
		} else {
			struct vsp1_rwpf *rpf =
				to_rwpf(&pipe->uds_input->subdev);

			uds->scale_alpha = rpf->fmtinfo->alpha;
		}
	}

	list_for_each_entry(entity, &pipe->entities, list_pipe) {
		vsp1_entity_route_setup(entity, pipe->dl);

		if (entity->ops->configure)
			entity->ops->configure(entity, pipe, pipe->dl,
					       VSP1_ENTITY_PARAMS_INIT);
	}

	return 0;
>>>>>>> v4.9.227
}

static int vsp1_video_start_streaming(struct vb2_queue *vq, unsigned int count)
{
	struct vsp1_video *video = vb2_get_drv_priv(vq);
<<<<<<< HEAD
	struct vsp1_pipeline *pipe = to_vsp1_pipeline(&video->video.entity);
	struct vsp1_entity *entity;
=======
	struct vsp1_pipeline *pipe = video->rwpf->pipe;
	bool start_pipeline = false;
>>>>>>> v4.9.227
	unsigned long flags;
	int ret;

	mutex_lock(&pipe->lock);
<<<<<<< HEAD
	if (pipe->stream_count == pipe->num_video - 1) {
		if (pipe->uds) {
			struct vsp1_uds *uds = to_uds(&pipe->uds->subdev);

			/* If a BRU is present in the pipeline before the UDS,
			 * the alpha component doesn't need to be scaled as the
			 * BRU output alpha value is fixed to 255. Otherwise we
			 * need to scale the alpha component only when available
			 * at the input RPF.
			 */
			if (pipe->uds_input->type == VSP1_ENTITY_BRU) {
				uds->scale_alpha = false;
			} else {
				struct vsp1_rwpf *rpf =
					to_rwpf(&pipe->uds_input->subdev);

				uds->scale_alpha = rpf->video.fmtinfo->alpha;
			}
		}

		list_for_each_entry(entity, &pipe->entities, list_pipe) {
			vsp1_entity_route_setup(entity);

			ret = v4l2_subdev_call(&entity->subdev, video,
					       s_stream, 1);
			if (ret < 0) {
				mutex_unlock(&pipe->lock);
				return ret;
			}
		}
=======
	if (pipe->stream_count == pipe->num_inputs) {
		ret = vsp1_video_setup_pipeline(pipe);
		if (ret < 0) {
			mutex_unlock(&pipe->lock);
			return ret;
		}

		start_pipeline = true;
>>>>>>> v4.9.227
	}

	pipe->stream_count++;
	mutex_unlock(&pipe->lock);

<<<<<<< HEAD
	spin_lock_irqsave(&pipe->irqlock, flags);
	if (vsp1_pipeline_ready(pipe))
		vsp1_pipeline_run(pipe);
=======
	/*
	 * vsp1_pipeline_ready() is not sufficient to establish that all streams
	 * are prepared and the pipeline is configured, as multiple streams
	 * can race through streamon with buffers already queued; Therefore we
	 * don't even attempt to start the pipeline until the last stream has
	 * called through here.
	 */
	if (!start_pipeline)
		return 0;

	spin_lock_irqsave(&pipe->irqlock, flags);
	if (vsp1_pipeline_ready(pipe))
		vsp1_video_pipeline_run(pipe);
>>>>>>> v4.9.227
	spin_unlock_irqrestore(&pipe->irqlock, flags);

	return 0;
}

static void vsp1_video_stop_streaming(struct vb2_queue *vq)
{
	struct vsp1_video *video = vb2_get_drv_priv(vq);
<<<<<<< HEAD
	struct vsp1_pipeline *pipe = to_vsp1_pipeline(&video->video.entity);
	struct vsp1_video_buffer *buffer;
	unsigned long flags;
	int ret;

	mutex_lock(&pipe->lock);
	if (--pipe->stream_count == 0) {
=======
	struct vsp1_pipeline *pipe = video->rwpf->pipe;
	struct vsp1_vb2_buffer *buffer;
	unsigned long flags;
	int ret;

	/*
	 * Clear the buffers ready flag to make sure the device won't be started
	 * by a QBUF on the video node on the other side of the pipeline.
	 */
	spin_lock_irqsave(&video->irqlock, flags);
	pipe->buffers_ready &= ~(1 << video->pipe_index);
	spin_unlock_irqrestore(&video->irqlock, flags);

	mutex_lock(&pipe->lock);
	if (--pipe->stream_count == pipe->num_inputs) {
>>>>>>> v4.9.227
		/* Stop the pipeline. */
		ret = vsp1_pipeline_stop(pipe);
		if (ret == -ETIMEDOUT)
			dev_err(video->vsp1->dev, "pipeline stop timeout\n");
<<<<<<< HEAD
	}
	mutex_unlock(&pipe->lock);

	vsp1_pipeline_cleanup(pipe);
	media_entity_pipeline_stop(&video->video.entity);
=======

		vsp1_dl_list_put(pipe->dl);
		pipe->dl = NULL;
	}
	mutex_unlock(&pipe->lock);

	media_entity_pipeline_stop(&video->video.entity);
	vsp1_video_pipeline_put(pipe);
>>>>>>> v4.9.227

	/* Remove all buffers from the IRQ queue. */
	spin_lock_irqsave(&video->irqlock, flags);
	list_for_each_entry(buffer, &video->irqqueue, queue)
<<<<<<< HEAD
		vb2_buffer_done(&buffer->buf, VB2_BUF_STATE_ERROR);
=======
		vb2_buffer_done(&buffer->buf.vb2_buf, VB2_BUF_STATE_ERROR);
>>>>>>> v4.9.227
	INIT_LIST_HEAD(&video->irqqueue);
	spin_unlock_irqrestore(&video->irqlock, flags);
}

<<<<<<< HEAD
static struct vb2_ops vsp1_video_queue_qops = {
=======
static const struct vb2_ops vsp1_video_queue_qops = {
>>>>>>> v4.9.227
	.queue_setup = vsp1_video_queue_setup,
	.buf_prepare = vsp1_video_buffer_prepare,
	.buf_queue = vsp1_video_buffer_queue,
	.wait_prepare = vb2_ops_wait_prepare,
	.wait_finish = vb2_ops_wait_finish,
	.start_streaming = vsp1_video_start_streaming,
	.stop_streaming = vsp1_video_stop_streaming,
};

/* -----------------------------------------------------------------------------
 * V4L2 ioctls
 */

static int
vsp1_video_querycap(struct file *file, void *fh, struct v4l2_capability *cap)
{
	struct v4l2_fh *vfh = file->private_data;
	struct vsp1_video *video = to_vsp1_video(vfh->vdev);

	cap->capabilities = V4L2_CAP_DEVICE_CAPS | V4L2_CAP_STREAMING
			  | V4L2_CAP_VIDEO_CAPTURE_MPLANE
			  | V4L2_CAP_VIDEO_OUTPUT_MPLANE;

	if (video->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
		cap->device_caps = V4L2_CAP_VIDEO_CAPTURE_MPLANE
				 | V4L2_CAP_STREAMING;
	else
		cap->device_caps = V4L2_CAP_VIDEO_OUTPUT_MPLANE
				 | V4L2_CAP_STREAMING;

	strlcpy(cap->driver, "vsp1", sizeof(cap->driver));
	strlcpy(cap->card, video->video.name, sizeof(cap->card));
	snprintf(cap->bus_info, sizeof(cap->bus_info), "platform:%s",
		 dev_name(video->vsp1->dev));

	return 0;
}

static int
vsp1_video_get_format(struct file *file, void *fh, struct v4l2_format *format)
{
	struct v4l2_fh *vfh = file->private_data;
	struct vsp1_video *video = to_vsp1_video(vfh->vdev);

	if (format->type != video->queue.type)
		return -EINVAL;

	mutex_lock(&video->lock);
<<<<<<< HEAD
	format->fmt.pix_mp = video->format;
=======
	format->fmt.pix_mp = video->rwpf->format;
>>>>>>> v4.9.227
	mutex_unlock(&video->lock);

	return 0;
}

static int
vsp1_video_try_format(struct file *file, void *fh, struct v4l2_format *format)
{
	struct v4l2_fh *vfh = file->private_data;
	struct vsp1_video *video = to_vsp1_video(vfh->vdev);

	if (format->type != video->queue.type)
		return -EINVAL;

	return __vsp1_video_try_format(video, &format->fmt.pix_mp, NULL);
}

static int
vsp1_video_set_format(struct file *file, void *fh, struct v4l2_format *format)
{
	struct v4l2_fh *vfh = file->private_data;
	struct vsp1_video *video = to_vsp1_video(vfh->vdev);
	const struct vsp1_format_info *info;
	int ret;

	if (format->type != video->queue.type)
		return -EINVAL;

	ret = __vsp1_video_try_format(video, &format->fmt.pix_mp, &info);
	if (ret < 0)
		return ret;

	mutex_lock(&video->lock);

	if (vb2_is_busy(&video->queue)) {
		ret = -EBUSY;
		goto done;
	}

<<<<<<< HEAD
	video->format = format->fmt.pix_mp;
	video->fmtinfo = info;
=======
	video->rwpf->format = format->fmt.pix_mp;
	video->rwpf->fmtinfo = info;
>>>>>>> v4.9.227

done:
	mutex_unlock(&video->lock);
	return ret;
}

static int
vsp1_video_streamon(struct file *file, void *fh, enum v4l2_buf_type type)
{
	struct v4l2_fh *vfh = file->private_data;
	struct vsp1_video *video = to_vsp1_video(vfh->vdev);
<<<<<<< HEAD
=======
	struct media_device *mdev = &video->vsp1->media_dev;
>>>>>>> v4.9.227
	struct vsp1_pipeline *pipe;
	int ret;

	if (video->queue.owner && video->queue.owner != file->private_data)
		return -EBUSY;

<<<<<<< HEAD
	video->sequence = 0;

	/* Start streaming on the pipeline. No link touching an entity in the
	 * pipeline can be activated or deactivated once streaming is started.
	 *
	 * Use the VSP1 pipeline object embedded in the first video object that
	 * starts streaming.
	 */
	pipe = video->video.entity.pipe
	     ? to_vsp1_pipeline(&video->video.entity) : &video->pipe;

	ret = media_entity_pipeline_start(&video->video.entity, &pipe->pipe);
	if (ret < 0)
		return ret;
=======
	/* Get a pipeline for the video node and start streaming on it. No link
	 * touching an entity in the pipeline can be activated or deactivated
	 * once streaming is started.
	 */
	mutex_lock(&mdev->graph_mutex);

	pipe = vsp1_video_pipeline_get(video);
	if (IS_ERR(pipe)) {
		mutex_unlock(&mdev->graph_mutex);
		return PTR_ERR(pipe);
	}

	ret = __media_entity_pipeline_start(&video->video.entity, &pipe->pipe);
	if (ret < 0) {
		mutex_unlock(&mdev->graph_mutex);
		goto err_pipe;
	}

	mutex_unlock(&mdev->graph_mutex);
>>>>>>> v4.9.227

	/* Verify that the configured format matches the output of the connected
	 * subdev.
	 */
	ret = vsp1_video_verify_format(video);
	if (ret < 0)
		goto err_stop;

<<<<<<< HEAD
	ret = vsp1_pipeline_init(pipe, video);
	if (ret < 0)
		goto err_stop;

	/* Start the queue. */
	ret = vb2_streamon(&video->queue, type);
	if (ret < 0)
		goto err_cleanup;

	return 0;

err_cleanup:
	vsp1_pipeline_cleanup(pipe);
err_stop:
	media_entity_pipeline_stop(&video->video.entity);
=======
	/* Start the queue. */
	ret = vb2_streamon(&video->queue, type);
	if (ret < 0)
		goto err_stop;

	return 0;

err_stop:
	media_entity_pipeline_stop(&video->video.entity);
err_pipe:
	vsp1_video_pipeline_put(pipe);
>>>>>>> v4.9.227
	return ret;
}

static const struct v4l2_ioctl_ops vsp1_video_ioctl_ops = {
	.vidioc_querycap		= vsp1_video_querycap,
	.vidioc_g_fmt_vid_cap_mplane	= vsp1_video_get_format,
	.vidioc_s_fmt_vid_cap_mplane	= vsp1_video_set_format,
	.vidioc_try_fmt_vid_cap_mplane	= vsp1_video_try_format,
	.vidioc_g_fmt_vid_out_mplane	= vsp1_video_get_format,
	.vidioc_s_fmt_vid_out_mplane	= vsp1_video_set_format,
	.vidioc_try_fmt_vid_out_mplane	= vsp1_video_try_format,
	.vidioc_reqbufs			= vb2_ioctl_reqbufs,
	.vidioc_querybuf		= vb2_ioctl_querybuf,
	.vidioc_qbuf			= vb2_ioctl_qbuf,
	.vidioc_dqbuf			= vb2_ioctl_dqbuf,
	.vidioc_create_bufs		= vb2_ioctl_create_bufs,
	.vidioc_prepare_buf		= vb2_ioctl_prepare_buf,
	.vidioc_streamon		= vsp1_video_streamon,
	.vidioc_streamoff		= vb2_ioctl_streamoff,
};

/* -----------------------------------------------------------------------------
 * V4L2 File Operations
 */

static int vsp1_video_open(struct file *file)
{
	struct vsp1_video *video = video_drvdata(file);
	struct v4l2_fh *vfh;
	int ret = 0;

	vfh = kzalloc(sizeof(*vfh), GFP_KERNEL);
	if (vfh == NULL)
		return -ENOMEM;

	v4l2_fh_init(vfh, &video->video);
	v4l2_fh_add(vfh);

	file->private_data = vfh;

	ret = vsp1_device_get(video->vsp1);
	if (ret < 0) {
		v4l2_fh_del(vfh);
		kfree(vfh);
	}

	return ret;
}

static int vsp1_video_release(struct file *file)
{
	struct vsp1_video *video = video_drvdata(file);
	struct v4l2_fh *vfh = file->private_data;

	mutex_lock(&video->lock);
	if (video->queue.owner == vfh) {
		vb2_queue_release(&video->queue);
		video->queue.owner = NULL;
	}
	mutex_unlock(&video->lock);

	vsp1_device_put(video->vsp1);

	v4l2_fh_release(file);

	file->private_data = NULL;

	return 0;
}

<<<<<<< HEAD
static struct v4l2_file_operations vsp1_video_fops = {
=======
static const struct v4l2_file_operations vsp1_video_fops = {
>>>>>>> v4.9.227
	.owner = THIS_MODULE,
	.unlocked_ioctl = video_ioctl2,
	.open = vsp1_video_open,
	.release = vsp1_video_release,
	.poll = vb2_fop_poll,
	.mmap = vb2_fop_mmap,
};

/* -----------------------------------------------------------------------------
 * Initialization and Cleanup
 */

<<<<<<< HEAD
int vsp1_video_init(struct vsp1_video *video, struct vsp1_entity *rwpf)
{
	const char *direction;
	int ret;

	switch (video->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
		direction = "output";
		video->pad.flags = MEDIA_PAD_FL_SINK;
		break;

	case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
		direction = "input";
		video->pad.flags = MEDIA_PAD_FL_SOURCE;
		video->video.vfl_dir = VFL_DIR_TX;
		break;

	default:
		return -EINVAL;
	}

	video->rwpf = rwpf;

=======
struct vsp1_video *vsp1_video_create(struct vsp1_device *vsp1,
				     struct vsp1_rwpf *rwpf)
{
	struct vsp1_video *video;
	const char *direction;
	int ret;

	video = devm_kzalloc(vsp1->dev, sizeof(*video), GFP_KERNEL);
	if (!video)
		return ERR_PTR(-ENOMEM);

	rwpf->video = video;

	video->vsp1 = vsp1;
	video->rwpf = rwpf;

	if (rwpf->entity.type == VSP1_ENTITY_RPF) {
		direction = "input";
		video->type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
		video->pad.flags = MEDIA_PAD_FL_SOURCE;
		video->video.vfl_dir = VFL_DIR_TX;
	} else {
		direction = "output";
		video->type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		video->pad.flags = MEDIA_PAD_FL_SINK;
		video->video.vfl_dir = VFL_DIR_RX;
	}

>>>>>>> v4.9.227
	mutex_init(&video->lock);
	spin_lock_init(&video->irqlock);
	INIT_LIST_HEAD(&video->irqqueue);

<<<<<<< HEAD
	mutex_init(&video->pipe.lock);
	spin_lock_init(&video->pipe.irqlock);
	INIT_LIST_HEAD(&video->pipe.entities);
	init_waitqueue_head(&video->pipe.wq);
	video->pipe.state = VSP1_PIPELINE_STOPPED;

	/* Initialize the media entity... */
	ret = media_entity_init(&video->video.entity, 1, &video->pad, 0);
	if (ret < 0)
		return ret;

	/* ... and the format ... */
	video->fmtinfo = vsp1_get_format_info(VSP1_VIDEO_DEF_FORMAT);
	video->format.pixelformat = video->fmtinfo->fourcc;
	video->format.colorspace = V4L2_COLORSPACE_SRGB;
	video->format.field = V4L2_FIELD_NONE;
	video->format.width = VSP1_VIDEO_DEF_WIDTH;
	video->format.height = VSP1_VIDEO_DEF_HEIGHT;
	video->format.num_planes = 1;
	video->format.plane_fmt[0].bytesperline =
		video->format.width * video->fmtinfo->bpp[0] / 8;
	video->format.plane_fmt[0].sizeimage =
		video->format.plane_fmt[0].bytesperline * video->format.height;
=======
	/* Initialize the media entity... */
	ret = media_entity_pads_init(&video->video.entity, 1, &video->pad);
	if (ret < 0)
		return ERR_PTR(ret);

	/* ... and the format ... */
	rwpf->format.pixelformat = VSP1_VIDEO_DEF_FORMAT;
	rwpf->format.width = VSP1_VIDEO_DEF_WIDTH;
	rwpf->format.height = VSP1_VIDEO_DEF_HEIGHT;
	__vsp1_video_try_format(video, &rwpf->format, &rwpf->fmtinfo);
>>>>>>> v4.9.227

	/* ... and the video node... */
	video->video.v4l2_dev = &video->vsp1->v4l2_dev;
	video->video.fops = &vsp1_video_fops;
	snprintf(video->video.name, sizeof(video->video.name), "%s %s",
<<<<<<< HEAD
		 rwpf->subdev.name, direction);
=======
		 rwpf->entity.subdev.name, direction);
>>>>>>> v4.9.227
	video->video.vfl_type = VFL_TYPE_GRABBER;
	video->video.release = video_device_release_empty;
	video->video.ioctl_ops = &vsp1_video_ioctl_ops;

	video_set_drvdata(&video->video, video);

<<<<<<< HEAD
	/* ... and the buffers queue... */
	video->alloc_ctx = vb2_dma_contig_init_ctx(video->vsp1->dev);
	if (IS_ERR(video->alloc_ctx)) {
		ret = PTR_ERR(video->alloc_ctx);
		goto error;
	}

=======
>>>>>>> v4.9.227
	video->queue.type = video->type;
	video->queue.io_modes = VB2_MMAP | VB2_USERPTR | VB2_DMABUF;
	video->queue.lock = &video->lock;
	video->queue.drv_priv = video;
<<<<<<< HEAD
	video->queue.buf_struct_size = sizeof(struct vsp1_video_buffer);
	video->queue.ops = &vsp1_video_queue_qops;
	video->queue.mem_ops = &vb2_dma_contig_memops;
	video->queue.timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_COPY;
=======
	video->queue.buf_struct_size = sizeof(struct vsp1_vb2_buffer);
	video->queue.ops = &vsp1_video_queue_qops;
	video->queue.mem_ops = &vb2_dma_contig_memops;
	video->queue.timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_COPY;
	video->queue.dev = video->vsp1->dev;
>>>>>>> v4.9.227
	ret = vb2_queue_init(&video->queue);
	if (ret < 0) {
		dev_err(video->vsp1->dev, "failed to initialize vb2 queue\n");
		goto error;
	}

	/* ... and register the video device. */
	video->video.queue = &video->queue;
	ret = video_register_device(&video->video, VFL_TYPE_GRABBER, -1);
	if (ret < 0) {
		dev_err(video->vsp1->dev, "failed to register video device\n");
		goto error;
	}

<<<<<<< HEAD
	return 0;

error:
	vb2_dma_contig_cleanup_ctx(video->alloc_ctx);
	vsp1_video_cleanup(video);
	return ret;
=======
	return video;

error:
	vsp1_video_cleanup(video);
	return ERR_PTR(ret);
>>>>>>> v4.9.227
}

void vsp1_video_cleanup(struct vsp1_video *video)
{
	if (video_is_registered(&video->video))
		video_unregister_device(&video->video);

<<<<<<< HEAD
	vb2_dma_contig_cleanup_ctx(video->alloc_ctx);
=======
>>>>>>> v4.9.227
	media_entity_cleanup(&video->video.entity);
}
