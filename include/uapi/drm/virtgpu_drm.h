/*
 * Copyright 2013 Red Hat
 * All Rights Reserved.
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
 * THE AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef VIRTGPU_DRM_H
#define VIRTGPU_DRM_H

<<<<<<< HEAD
#include <stddef.h>
#include "drm/drm.h"
=======
#include "drm.h"

#if defined(__cplusplus)
extern "C" {
#endif
>>>>>>> v4.9.227

/* Please note that modifications to all structs defined here are
 * subject to backwards-compatibility constraints.
 *
<<<<<<< HEAD
 * Do not use pointers, use uint64_t instead for 32 bit / 64 bit user/kernel
=======
 * Do not use pointers, use __u64 instead for 32 bit / 64 bit user/kernel
>>>>>>> v4.9.227
 * compatibility Keep fields aligned to their size
 */

#define DRM_VIRTGPU_MAP         0x01
#define DRM_VIRTGPU_EXECBUFFER  0x02
#define DRM_VIRTGPU_GETPARAM    0x03
#define DRM_VIRTGPU_RESOURCE_CREATE 0x04
#define DRM_VIRTGPU_RESOURCE_INFO     0x05
#define DRM_VIRTGPU_TRANSFER_FROM_HOST 0x06
#define DRM_VIRTGPU_TRANSFER_TO_HOST 0x07
#define DRM_VIRTGPU_WAIT     0x08
#define DRM_VIRTGPU_GET_CAPS  0x09

struct drm_virtgpu_map {
<<<<<<< HEAD
	uint64_t offset; /* use for mmap system call */
	uint32_t handle;
	uint32_t pad;
};

struct drm_virtgpu_execbuffer {
	uint32_t		flags;		/* for future use */
	uint32_t size;
	uint64_t command; /* void* */
	uint64_t bo_handles;
	uint32_t num_bo_handles;
	uint32_t pad;
};

#define VIRTGPU_PARAM_3D_FEATURES 1 /* do we have 3D features in the hw */

struct drm_virtgpu_getparam {
	uint64_t param;
	uint64_t value;
=======
	__u64 offset; /* use for mmap system call */
	__u32 handle;
	__u32 pad;
};

struct drm_virtgpu_execbuffer {
	__u32		flags;		/* for future use */
	__u32 size;
	__u64 command; /* void* */
	__u64 bo_handles;
	__u32 num_bo_handles;
	__u32 pad;
};

#define VIRTGPU_PARAM_3D_FEATURES 1 /* do we have 3D features in the hw */
#define VIRTGPU_PARAM_CAPSET_QUERY_FIX 2 /* do we have the capset fix */

struct drm_virtgpu_getparam {
	__u64 param;
	__u64 value;
>>>>>>> v4.9.227
};

/* NO_BO flags? NO resource flag? */
/* resource flag for y_0_top */
struct drm_virtgpu_resource_create {
<<<<<<< HEAD
	uint32_t target;
	uint32_t format;
	uint32_t bind;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t array_size;
	uint32_t last_level;
	uint32_t nr_samples;
	uint32_t flags;
	uint32_t bo_handle; /* if this is set - recreate a new resource attached to this bo ? */
	uint32_t res_handle;  /* returned by kernel */
	uint32_t size;        /* validate transfer in the host */
	uint32_t stride;      /* validate transfer in the host */
};

struct drm_virtgpu_resource_info {
	uint32_t bo_handle;
	uint32_t res_handle;
	uint32_t size;
	uint32_t stride;
};

struct drm_virtgpu_3d_box {
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t w;
	uint32_t h;
	uint32_t d;
};

struct drm_virtgpu_3d_transfer_to_host {
	uint32_t bo_handle;
	struct drm_virtgpu_3d_box box;
	uint32_t level;
	uint32_t offset;
};

struct drm_virtgpu_3d_transfer_from_host {
	uint32_t bo_handle;
	struct drm_virtgpu_3d_box box;
	uint32_t level;
	uint32_t offset;
=======
	__u32 target;
	__u32 format;
	__u32 bind;
	__u32 width;
	__u32 height;
	__u32 depth;
	__u32 array_size;
	__u32 last_level;
	__u32 nr_samples;
	__u32 flags;
	__u32 bo_handle; /* if this is set - recreate a new resource attached to this bo ? */
	__u32 res_handle;  /* returned by kernel */
	__u32 size;        /* validate transfer in the host */
	__u32 stride;      /* validate transfer in the host */
};

struct drm_virtgpu_resource_info {
	__u32 bo_handle;
	__u32 res_handle;
	__u32 size;
	__u32 stride;
};

struct drm_virtgpu_3d_box {
	__u32 x;
	__u32 y;
	__u32 z;
	__u32 w;
	__u32 h;
	__u32 d;
};

struct drm_virtgpu_3d_transfer_to_host {
	__u32 bo_handle;
	struct drm_virtgpu_3d_box box;
	__u32 level;
	__u32 offset;
};

struct drm_virtgpu_3d_transfer_from_host {
	__u32 bo_handle;
	struct drm_virtgpu_3d_box box;
	__u32 level;
	__u32 offset;
>>>>>>> v4.9.227
};

#define VIRTGPU_WAIT_NOWAIT 1 /* like it */
struct drm_virtgpu_3d_wait {
<<<<<<< HEAD
	uint32_t handle; /* 0 is an invalid handle */
	uint32_t flags;
};

struct drm_virtgpu_get_caps {
	uint32_t cap_set_id;
	uint32_t cap_set_ver;
	uint64_t addr;
	uint32_t size;
	uint32_t pad;
=======
	__u32 handle; /* 0 is an invalid handle */
	__u32 flags;
};

struct drm_virtgpu_get_caps {
	__u32 cap_set_id;
	__u32 cap_set_ver;
	__u64 addr;
	__u32 size;
	__u32 pad;
>>>>>>> v4.9.227
};

#define DRM_IOCTL_VIRTGPU_MAP \
	DRM_IOWR(DRM_COMMAND_BASE + DRM_VIRTGPU_MAP, struct drm_virtgpu_map)

#define DRM_IOCTL_VIRTGPU_EXECBUFFER \
	DRM_IOW(DRM_COMMAND_BASE + DRM_VIRTGPU_EXECBUFFER,\
		struct drm_virtgpu_execbuffer)

#define DRM_IOCTL_VIRTGPU_GETPARAM \
	DRM_IOWR(DRM_COMMAND_BASE + DRM_VIRTGPU_GETPARAM,\
		struct drm_virtgpu_getparam)

#define DRM_IOCTL_VIRTGPU_RESOURCE_CREATE			\
	DRM_IOWR(DRM_COMMAND_BASE + DRM_VIRTGPU_RESOURCE_CREATE,	\
		struct drm_virtgpu_resource_create)

#define DRM_IOCTL_VIRTGPU_RESOURCE_INFO \
	DRM_IOWR(DRM_COMMAND_BASE + DRM_VIRTGPU_RESOURCE_INFO, \
		 struct drm_virtgpu_resource_info)

#define DRM_IOCTL_VIRTGPU_TRANSFER_FROM_HOST \
	DRM_IOWR(DRM_COMMAND_BASE + DRM_VIRTGPU_TRANSFER_FROM_HOST,	\
		struct drm_virtgpu_3d_transfer_from_host)

#define DRM_IOCTL_VIRTGPU_TRANSFER_TO_HOST \
	DRM_IOWR(DRM_COMMAND_BASE + DRM_VIRTGPU_TRANSFER_TO_HOST,	\
		struct drm_virtgpu_3d_transfer_to_host)

#define DRM_IOCTL_VIRTGPU_WAIT				\
	DRM_IOWR(DRM_COMMAND_BASE + DRM_VIRTGPU_WAIT,	\
		struct drm_virtgpu_3d_wait)

#define DRM_IOCTL_VIRTGPU_GET_CAPS \
	DRM_IOWR(DRM_COMMAND_BASE + DRM_VIRTGPU_GET_CAPS, \
	struct drm_virtgpu_get_caps)

<<<<<<< HEAD
=======
#if defined(__cplusplus)
}
#endif

>>>>>>> v4.9.227
#endif
