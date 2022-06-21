/*
 * vsp1.h  --  R-Car VSP1 Driver
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
#ifndef __VSP1_H__
#define __VSP1_H__

#include <linux/io.h>
#include <linux/list.h>
#include <linux/mutex.h>
<<<<<<< HEAD
#include <linux/platform_data/vsp1.h>
=======
>>>>>>> v4.9.227

#include <media/media-device.h>
#include <media/v4l2-device.h>
#include <media/v4l2-subdev.h>

#include "vsp1_regs.h"

struct clk;
struct device;
<<<<<<< HEAD

struct vsp1_platform_data;
struct vsp1_bru;
=======
struct rcar_fcp_device;

struct vsp1_drm;
struct vsp1_entity;
struct vsp1_platform_data;
struct vsp1_bru;
struct vsp1_clu;
>>>>>>> v4.9.227
struct vsp1_hsit;
struct vsp1_lif;
struct vsp1_lut;
struct vsp1_rwpf;
struct vsp1_sru;
struct vsp1_uds;

#define VSP1_MAX_RPF		5
#define VSP1_MAX_UDS		3
#define VSP1_MAX_WPF		4

<<<<<<< HEAD
struct vsp1_device {
	struct device *dev;
	struct vsp1_platform_data *pdata;

	void __iomem *mmio;
	struct clk *clock;

	struct mutex lock;
	int ref_count;

	struct vsp1_bru *bru;
=======
#define VSP1_HAS_LIF		(1 << 0)
#define VSP1_HAS_LUT		(1 << 1)
#define VSP1_HAS_SRU		(1 << 2)
#define VSP1_HAS_BRU		(1 << 3)
#define VSP1_HAS_CLU		(1 << 4)
#define VSP1_HAS_WPF_VFLIP	(1 << 5)
#define VSP1_HAS_WPF_HFLIP	(1 << 6)

struct vsp1_device_info {
	u32 version;
	const char *model;
	unsigned int gen;
	unsigned int features;
	unsigned int rpf_count;
	unsigned int uds_count;
	unsigned int wpf_count;
	unsigned int num_bru_inputs;
	bool uapi;
};

struct vsp1_device {
	struct device *dev;
	const struct vsp1_device_info *info;
	u32 version;

	void __iomem *mmio;
	struct rcar_fcp_device *fcp;

	struct vsp1_bru *bru;
	struct vsp1_clu *clu;
>>>>>>> v4.9.227
	struct vsp1_hsit *hsi;
	struct vsp1_hsit *hst;
	struct vsp1_lif *lif;
	struct vsp1_lut *lut;
	struct vsp1_rwpf *rpf[VSP1_MAX_RPF];
	struct vsp1_sru *sru;
	struct vsp1_uds *uds[VSP1_MAX_UDS];
	struct vsp1_rwpf *wpf[VSP1_MAX_WPF];

	struct list_head entities;
<<<<<<< HEAD

	struct v4l2_device v4l2_dev;
	struct media_device media_dev;
=======
	struct list_head videos;

	struct v4l2_device v4l2_dev;
	struct media_device media_dev;
	struct media_entity_operations media_ops;

	struct vsp1_drm *drm;
>>>>>>> v4.9.227
};

int vsp1_device_get(struct vsp1_device *vsp1);
void vsp1_device_put(struct vsp1_device *vsp1);

<<<<<<< HEAD
=======
int vsp1_reset_wpf(struct vsp1_device *vsp1, unsigned int index);

>>>>>>> v4.9.227
static inline u32 vsp1_read(struct vsp1_device *vsp1, u32 reg)
{
	return ioread32(vsp1->mmio + reg);
}

static inline void vsp1_write(struct vsp1_device *vsp1, u32 reg, u32 data)
{
	iowrite32(data, vsp1->mmio + reg);
}

#endif /* __VSP1_H__ */
