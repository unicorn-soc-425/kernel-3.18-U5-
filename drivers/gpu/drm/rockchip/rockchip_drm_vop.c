/*
 * Copyright (C) Fuzhou Rockchip Electronics Co.Ltd
 * Author:Mark Yao <mark.yao@rock-chips.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <drm/drm.h>
#include <drm/drmP.h>
<<<<<<< HEAD
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
=======
#include <drm/drm_atomic.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_flip_work.h>
>>>>>>> v4.9.227
#include <drm/drm_plane_helper.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
<<<<<<< HEAD
=======
#include <linux/iopoll.h>
>>>>>>> v4.9.227
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>
#include <linux/component.h>

#include <linux/reset.h>
#include <linux/delay.h>

#include "rockchip_drm_drv.h"
#include "rockchip_drm_gem.h"
#include "rockchip_drm_fb.h"
<<<<<<< HEAD
#include "rockchip_drm_vop.h"

#define VOP_REG(off, _mask, s) \
		{.offset = off, \
		 .mask = _mask, \
		 .shift = s,}

#define __REG_SET_RELAXED(x, off, mask, shift, v) \
		vop_mask_write_relaxed(x, off, (mask) << shift, (v) << shift)
#define __REG_SET_NORMAL(x, off, mask, shift, v) \
		vop_mask_write(x, off, (mask) << shift, (v) << shift)

#define REG_SET(x, base, reg, v, mode) \
		__REG_SET_##mode(x, base + reg.offset, reg.mask, reg.shift, v)
=======
#include "rockchip_drm_psr.h"
#include "rockchip_drm_vop.h"

#define __REG_SET_RELAXED(x, off, mask, shift, v, write_mask) \
		vop_mask_write(x, off, mask, shift, v, write_mask, true)

#define __REG_SET_NORMAL(x, off, mask, shift, v, write_mask) \
		vop_mask_write(x, off, mask, shift, v, write_mask, false)

#define REG_SET(x, base, reg, v, mode) \
		__REG_SET_##mode(x, base + reg.offset, \
				 reg.mask, reg.shift, v, reg.write_mask)
#define REG_SET_MASK(x, base, reg, mask, v, mode) \
		__REG_SET_##mode(x, base + reg.offset, \
				 mask, reg.shift, v, reg.write_mask)
>>>>>>> v4.9.227

#define VOP_WIN_SET(x, win, name, v) \
		REG_SET(x, win->base, win->phy->name, v, RELAXED)
#define VOP_SCL_SET(x, win, name, v) \
		REG_SET(x, win->base, win->phy->scl->name, v, RELAXED)
<<<<<<< HEAD
#define VOP_CTRL_SET(x, name, v) \
		REG_SET(x, 0, (x)->data->ctrl->name, v, NORMAL)

=======
#define VOP_SCL_SET_EXT(x, win, name, v) \
		REG_SET(x, win->base, win->phy->scl->ext->name, v, RELAXED)
#define VOP_CTRL_SET(x, name, v) \
		REG_SET(x, 0, (x)->data->ctrl->name, v, NORMAL)

#define VOP_INTR_GET(vop, name) \
		vop_read_reg(vop, 0, &vop->data->ctrl->name)

#define VOP_INTR_SET(vop, name, mask, v) \
		REG_SET_MASK(vop, 0, vop->data->intr->name, mask, v, NORMAL)
#define VOP_INTR_SET_TYPE(vop, name, type, v) \
	do { \
		int i, reg = 0, mask = 0; \
		for (i = 0; i < vop->data->intr->nintrs; i++) { \
			if (vop->data->intr->intrs[i] & type) { \
				reg |= (v) << i; \
				mask |= 1 << i; \
			} \
		} \
		VOP_INTR_SET(vop, name, mask, reg); \
	} while (0)
#define VOP_INTR_GET_TYPE(vop, name, type) \
		vop_get_intr_type(vop, &vop->data->intr->name, type)

>>>>>>> v4.9.227
#define VOP_WIN_GET(x, win, name) \
		vop_read_reg(x, win->base, &win->phy->name)

#define VOP_WIN_GET_YRGBADDR(vop, win) \
		vop_readl(vop, win->base + win->phy->yrgb_mst.offset)

#define to_vop(x) container_of(x, struct vop, crtc)
#define to_vop_win(x) container_of(x, struct vop_win, base)

<<<<<<< HEAD
struct vop_win_state {
	struct list_head head;
	struct drm_framebuffer *fb;
	dma_addr_t yrgb_mst;
	struct drm_pending_vblank_event *event;
=======
enum vop_pending {
	VOP_PENDING_FB_UNREF,
>>>>>>> v4.9.227
};

struct vop_win {
	struct drm_plane base;
	const struct vop_win_data *data;
	struct vop *vop;
<<<<<<< HEAD

	struct list_head pending;
	struct vop_win_state *active;
=======
>>>>>>> v4.9.227
};

struct vop {
	struct drm_crtc crtc;
	struct device *dev;
	struct drm_device *drm_dev;
	bool is_enabled;

<<<<<<< HEAD
	int connector_type;
	int connector_out_mode;

=======
>>>>>>> v4.9.227
	/* mutex vsync_ work */
	struct mutex vsync_mutex;
	bool vsync_work_pending;
	struct completion dsp_hold_completion;

<<<<<<< HEAD
=======
	/* protected by dev->event_lock */
	struct drm_pending_vblank_event *event;

	struct drm_flip_work fb_unref_work;
	unsigned long pending;

	struct completion line_flag_completion;

>>>>>>> v4.9.227
	const struct vop_data *data;

	uint32_t *regsbak;
	void __iomem *regs;

	/* physical map length of vop register */
	uint32_t len;

	/* one time only one process allowed to config the register */
	spinlock_t reg_lock;
	/* lock vop irq reg */
	spinlock_t irq_lock;

	unsigned int irq;

	/* vop AHP clk */
	struct clk *hclk;
	/* vop dclk */
	struct clk *dclk;
	/* vop share memory frequency */
	struct clk *aclk;

	/* vop dclk reset */
	struct reset_control *dclk_rst;

<<<<<<< HEAD
	int pipe;

	struct vop_win win[];
};

enum vop_data_format {
	VOP_FMT_ARGB8888 = 0,
	VOP_FMT_RGB888,
	VOP_FMT_RGB565,
	VOP_FMT_YUV420SP = 4,
	VOP_FMT_YUV422SP,
	VOP_FMT_YUV444SP,
};

struct vop_reg_data {
	uint32_t offset;
	uint32_t value;
};

struct vop_reg {
	uint32_t offset;
	uint32_t shift;
	uint32_t mask;
};

struct vop_ctrl {
	struct vop_reg standby;
	struct vop_reg data_blank;
	struct vop_reg gate_en;
	struct vop_reg mmu_en;
	struct vop_reg rgb_en;
	struct vop_reg edp_en;
	struct vop_reg hdmi_en;
	struct vop_reg mipi_en;
	struct vop_reg out_mode;
	struct vop_reg dither_down;
	struct vop_reg dither_up;
	struct vop_reg pin_pol;

	struct vop_reg htotal_pw;
	struct vop_reg hact_st_end;
	struct vop_reg vtotal_pw;
	struct vop_reg vact_st_end;
	struct vop_reg hpost_st_end;
	struct vop_reg vpost_st_end;
};

struct vop_scl_regs {
	struct vop_reg cbcr_vsd_mode;
	struct vop_reg cbcr_vsu_mode;
	struct vop_reg cbcr_hsd_mode;
	struct vop_reg cbcr_ver_scl_mode;
	struct vop_reg cbcr_hor_scl_mode;
	struct vop_reg yrgb_vsd_mode;
	struct vop_reg yrgb_vsu_mode;
	struct vop_reg yrgb_hsd_mode;
	struct vop_reg yrgb_ver_scl_mode;
	struct vop_reg yrgb_hor_scl_mode;
	struct vop_reg line_load_mode;
	struct vop_reg cbcr_axi_gather_num;
	struct vop_reg yrgb_axi_gather_num;
	struct vop_reg vsd_cbcr_gt2;
	struct vop_reg vsd_cbcr_gt4;
	struct vop_reg vsd_yrgb_gt2;
	struct vop_reg vsd_yrgb_gt4;
	struct vop_reg bic_coe_sel;
	struct vop_reg cbcr_axi_gather_en;
	struct vop_reg yrgb_axi_gather_en;

	struct vop_reg lb_mode;
	struct vop_reg scale_yrgb_x;
	struct vop_reg scale_yrgb_y;
	struct vop_reg scale_cbcr_x;
	struct vop_reg scale_cbcr_y;
};

struct vop_win_phy {
	const struct vop_scl_regs *scl;
	const uint32_t *data_formats;
	uint32_t nformats;

	struct vop_reg enable;
	struct vop_reg format;
	struct vop_reg rb_swap;
	struct vop_reg act_info;
	struct vop_reg dsp_info;
	struct vop_reg dsp_st;
	struct vop_reg yrgb_mst;
	struct vop_reg uv_mst;
	struct vop_reg yrgb_vir;
	struct vop_reg uv_vir;

	struct vop_reg dst_alpha_ctl;
	struct vop_reg src_alpha_ctl;
};

struct vop_win_data {
	uint32_t base;
	const struct vop_win_phy *phy;
	enum drm_plane_type type;
};

struct vop_data {
	const struct vop_reg_data *init_table;
	unsigned int table_size;
	const struct vop_ctrl *ctrl;
	const struct vop_win_data *win;
	unsigned int win_size;
};

static const uint32_t formats_01[] = {
	DRM_FORMAT_XRGB8888,
	DRM_FORMAT_ARGB8888,
	DRM_FORMAT_XBGR8888,
	DRM_FORMAT_ABGR8888,
	DRM_FORMAT_RGB888,
	DRM_FORMAT_BGR888,
	DRM_FORMAT_RGB565,
	DRM_FORMAT_BGR565,
	DRM_FORMAT_NV12,
	DRM_FORMAT_NV16,
	DRM_FORMAT_NV24,
};

static const uint32_t formats_234[] = {
	DRM_FORMAT_XRGB8888,
	DRM_FORMAT_ARGB8888,
	DRM_FORMAT_XBGR8888,
	DRM_FORMAT_ABGR8888,
	DRM_FORMAT_RGB888,
	DRM_FORMAT_BGR888,
	DRM_FORMAT_RGB565,
	DRM_FORMAT_BGR565,
};

static const struct vop_scl_regs win_full_scl = {
	.cbcr_vsd_mode = VOP_REG(WIN0_CTRL1, 0x1, 31),
	.cbcr_vsu_mode = VOP_REG(WIN0_CTRL1, 0x1, 30),
	.cbcr_hsd_mode = VOP_REG(WIN0_CTRL1, 0x3, 28),
	.cbcr_ver_scl_mode = VOP_REG(WIN0_CTRL1, 0x3, 26),
	.cbcr_hor_scl_mode = VOP_REG(WIN0_CTRL1, 0x3, 24),
	.yrgb_vsd_mode = VOP_REG(WIN0_CTRL1, 0x1, 23),
	.yrgb_vsu_mode = VOP_REG(WIN0_CTRL1, 0x1, 22),
	.yrgb_hsd_mode = VOP_REG(WIN0_CTRL1, 0x3, 20),
	.yrgb_ver_scl_mode = VOP_REG(WIN0_CTRL1, 0x3, 18),
	.yrgb_hor_scl_mode = VOP_REG(WIN0_CTRL1, 0x3, 16),
	.line_load_mode = VOP_REG(WIN0_CTRL1, 0x1, 15),
	.cbcr_axi_gather_num = VOP_REG(WIN0_CTRL1, 0x7, 12),
	.yrgb_axi_gather_num = VOP_REG(WIN0_CTRL1, 0xf, 8),
	.vsd_cbcr_gt2 = VOP_REG(WIN0_CTRL1, 0x1, 7),
	.vsd_cbcr_gt4 = VOP_REG(WIN0_CTRL1, 0x1, 6),
	.vsd_yrgb_gt2 = VOP_REG(WIN0_CTRL1, 0x1, 5),
	.vsd_yrgb_gt4 = VOP_REG(WIN0_CTRL1, 0x1, 4),
	.bic_coe_sel = VOP_REG(WIN0_CTRL1, 0x3, 2),
	.cbcr_axi_gather_en = VOP_REG(WIN0_CTRL1, 0x1, 1),
	.yrgb_axi_gather_en = VOP_REG(WIN0_CTRL1, 0x1, 0),
	.lb_mode = VOP_REG(WIN0_CTRL0, 0x7, 5),
	.scale_yrgb_x = VOP_REG(WIN0_SCL_FACTOR_YRGB, 0xffff, 0x0),
	.scale_yrgb_y = VOP_REG(WIN0_SCL_FACTOR_YRGB, 0xffff, 16),
	.scale_cbcr_x = VOP_REG(WIN0_SCL_FACTOR_CBR, 0xffff, 0x0),
	.scale_cbcr_y = VOP_REG(WIN0_SCL_FACTOR_CBR, 0xffff, 16),
};

static const struct vop_win_phy win01_data = {
	.scl = &win_full_scl,
	.data_formats = formats_01,
	.nformats = ARRAY_SIZE(formats_01),
	.enable = VOP_REG(WIN0_CTRL0, 0x1, 0),
	.format = VOP_REG(WIN0_CTRL0, 0x7, 1),
	.rb_swap = VOP_REG(WIN0_CTRL0, 0x1, 12),
	.act_info = VOP_REG(WIN0_ACT_INFO, 0x1fff1fff, 0),
	.dsp_info = VOP_REG(WIN0_DSP_INFO, 0x0fff0fff, 0),
	.dsp_st = VOP_REG(WIN0_DSP_ST, 0x1fff1fff, 0),
	.yrgb_mst = VOP_REG(WIN0_YRGB_MST, 0xffffffff, 0),
	.uv_mst = VOP_REG(WIN0_CBR_MST, 0xffffffff, 0),
	.yrgb_vir = VOP_REG(WIN0_VIR, 0x3fff, 0),
	.uv_vir = VOP_REG(WIN0_VIR, 0x3fff, 16),
	.src_alpha_ctl = VOP_REG(WIN0_SRC_ALPHA_CTRL, 0xff, 0),
	.dst_alpha_ctl = VOP_REG(WIN0_DST_ALPHA_CTRL, 0xff, 0),
};

static const struct vop_win_phy win23_data = {
	.data_formats = formats_234,
	.nformats = ARRAY_SIZE(formats_234),
	.enable = VOP_REG(WIN2_CTRL0, 0x1, 0),
	.format = VOP_REG(WIN2_CTRL0, 0x7, 1),
	.rb_swap = VOP_REG(WIN2_CTRL0, 0x1, 12),
	.dsp_info = VOP_REG(WIN2_DSP_INFO0, 0x0fff0fff, 0),
	.dsp_st = VOP_REG(WIN2_DSP_ST0, 0x1fff1fff, 0),
	.yrgb_mst = VOP_REG(WIN2_MST0, 0xffffffff, 0),
	.yrgb_vir = VOP_REG(WIN2_VIR0_1, 0x1fff, 0),
	.src_alpha_ctl = VOP_REG(WIN2_SRC_ALPHA_CTRL, 0xff, 0),
	.dst_alpha_ctl = VOP_REG(WIN2_DST_ALPHA_CTRL, 0xff, 0),
};

static const struct vop_ctrl ctrl_data = {
	.standby = VOP_REG(SYS_CTRL, 0x1, 22),
	.gate_en = VOP_REG(SYS_CTRL, 0x1, 23),
	.mmu_en = VOP_REG(SYS_CTRL, 0x1, 20),
	.rgb_en = VOP_REG(SYS_CTRL, 0x1, 12),
	.hdmi_en = VOP_REG(SYS_CTRL, 0x1, 13),
	.edp_en = VOP_REG(SYS_CTRL, 0x1, 14),
	.mipi_en = VOP_REG(SYS_CTRL, 0x1, 15),
	.dither_down = VOP_REG(DSP_CTRL1, 0xf, 1),
	.dither_up = VOP_REG(DSP_CTRL1, 0x1, 6),
	.data_blank = VOP_REG(DSP_CTRL0, 0x1, 19),
	.out_mode = VOP_REG(DSP_CTRL0, 0xf, 0),
	.pin_pol = VOP_REG(DSP_CTRL0, 0xf, 4),
	.htotal_pw = VOP_REG(DSP_HTOTAL_HS_END, 0x1fff1fff, 0),
	.hact_st_end = VOP_REG(DSP_HACT_ST_END, 0x1fff1fff, 0),
	.vtotal_pw = VOP_REG(DSP_VTOTAL_VS_END, 0x1fff1fff, 0),
	.vact_st_end = VOP_REG(DSP_VACT_ST_END, 0x1fff1fff, 0),
	.hpost_st_end = VOP_REG(POST_DSP_HACT_INFO, 0x1fff1fff, 0),
	.vpost_st_end = VOP_REG(POST_DSP_VACT_INFO, 0x1fff1fff, 0),
};

static const struct vop_reg_data vop_init_reg_table[] = {
	{SYS_CTRL, 0x00c00000},
	{DSP_CTRL0, 0x00000000},
	{WIN0_CTRL0, 0x00000080},
	{WIN1_CTRL0, 0x00000080},
	/* TODO: Win2/3 support multiple area function, but we haven't found
	 * a suitable way to use it yet, so let's just use them as other windows
	 * with only area 0 enabled.
	 */
	{WIN2_CTRL0, 0x00000010},
	{WIN3_CTRL0, 0x00000010},
};

/*
 * Note: rk3288 has a dedicated 'cursor' window, however, that window requires
 * special support to get alpha blending working.  For now, just use overlay
 * window 3 for the drm cursor.
 *
 */
static const struct vop_win_data rk3288_vop_win_data[] = {
	{ .base = 0x00, .phy = &win01_data, .type = DRM_PLANE_TYPE_PRIMARY },
	{ .base = 0x40, .phy = &win01_data, .type = DRM_PLANE_TYPE_OVERLAY },
	{ .base = 0x00, .phy = &win23_data, .type = DRM_PLANE_TYPE_OVERLAY },
	{ .base = 0x50, .phy = &win23_data, .type = DRM_PLANE_TYPE_CURSOR },
};

static const struct vop_data rk3288_vop = {
	.init_table = vop_init_reg_table,
	.table_size = ARRAY_SIZE(vop_init_reg_table),
	.ctrl = &ctrl_data,
	.win = rk3288_vop_win_data,
	.win_size = ARRAY_SIZE(rk3288_vop_win_data),
};

static const struct of_device_id vop_driver_dt_match[] = {
	{ .compatible = "rockchip,rk3288-vop",
	  .data = &rk3288_vop },
	{},
};
MODULE_DEVICE_TABLE(of, vop_driver_dt_match);

=======
	struct vop_win win[];
};

>>>>>>> v4.9.227
static inline void vop_writel(struct vop *vop, uint32_t offset, uint32_t v)
{
	writel(v, vop->regs + offset);
	vop->regsbak[offset >> 2] = v;
}

static inline uint32_t vop_readl(struct vop *vop, uint32_t offset)
{
	return readl(vop->regs + offset);
}

static inline uint32_t vop_read_reg(struct vop *vop, uint32_t base,
				    const struct vop_reg *reg)
{
	return (vop_readl(vop, base + reg->offset) >> reg->shift) & reg->mask;
}

<<<<<<< HEAD
static inline void vop_cfg_done(struct vop *vop)
{
	writel(0x01, vop->regs + REG_CFG_DONE);
}

static inline void vop_mask_write(struct vop *vop, uint32_t offset,
				  uint32_t mask, uint32_t v)
{
	if (mask) {
		uint32_t cached_val = vop->regsbak[offset >> 2];

		cached_val = (cached_val & ~mask) | v;
		writel(cached_val, vop->regs + offset);
		vop->regsbak[offset >> 2] = cached_val;
	}
}

static inline void vop_mask_write_relaxed(struct vop *vop, uint32_t offset,
					  uint32_t mask, uint32_t v)
{
	if (mask) {
		uint32_t cached_val = vop->regsbak[offset >> 2];

		cached_val = (cached_val & ~mask) | v;
		writel_relaxed(cached_val, vop->regs + offset);
		vop->regsbak[offset >> 2] = cached_val;
	}
=======
static inline void vop_mask_write(struct vop *vop, uint32_t offset,
				  uint32_t mask, uint32_t shift, uint32_t v,
				  bool write_mask, bool relaxed)
{
	if (!mask)
		return;

	if (write_mask) {
		v = ((v << shift) & 0xffff) | (mask << (shift + 16));
	} else {
		uint32_t cached_val = vop->regsbak[offset >> 2];

		v = (cached_val & ~(mask << shift)) | ((v & mask) << shift);
		vop->regsbak[offset >> 2] = v;
	}

	if (relaxed)
		writel_relaxed(v, vop->regs + offset);
	else
		writel(v, vop->regs + offset);
}

static inline uint32_t vop_get_intr_type(struct vop *vop,
					 const struct vop_reg *reg, int type)
{
	uint32_t i, ret = 0;
	uint32_t regs = vop_read_reg(vop, 0, reg);

	for (i = 0; i < vop->data->intr->nintrs; i++) {
		if ((type & vop->data->intr->intrs[i]) && (regs & 1 << i))
			ret |= vop->data->intr->intrs[i];
	}

	return ret;
}

static inline void vop_cfg_done(struct vop *vop)
{
	VOP_CTRL_SET(vop, cfg_done, 1);
>>>>>>> v4.9.227
}

static bool has_rb_swapped(uint32_t format)
{
	switch (format) {
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_ABGR8888:
	case DRM_FORMAT_BGR888:
	case DRM_FORMAT_BGR565:
		return true;
	default:
		return false;
	}
}

static enum vop_data_format vop_convert_format(uint32_t format)
{
	switch (format) {
	case DRM_FORMAT_XRGB8888:
	case DRM_FORMAT_ARGB8888:
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_ABGR8888:
		return VOP_FMT_ARGB8888;
	case DRM_FORMAT_RGB888:
	case DRM_FORMAT_BGR888:
		return VOP_FMT_RGB888;
	case DRM_FORMAT_RGB565:
	case DRM_FORMAT_BGR565:
		return VOP_FMT_RGB565;
	case DRM_FORMAT_NV12:
		return VOP_FMT_YUV420SP;
	case DRM_FORMAT_NV16:
		return VOP_FMT_YUV422SP;
	case DRM_FORMAT_NV24:
		return VOP_FMT_YUV444SP;
	default:
<<<<<<< HEAD
		DRM_ERROR("unsupport format[%08x]\n", format);
=======
		DRM_ERROR("unsupported format[%08x]\n", format);
>>>>>>> v4.9.227
		return -EINVAL;
	}
}

static bool is_yuv_support(uint32_t format)
{
	switch (format) {
	case DRM_FORMAT_NV12:
	case DRM_FORMAT_NV16:
	case DRM_FORMAT_NV24:
		return true;
	default:
		return false;
	}
}

static bool is_alpha_support(uint32_t format)
{
	switch (format) {
	case DRM_FORMAT_ARGB8888:
	case DRM_FORMAT_ABGR8888:
		return true;
	default:
		return false;
	}
}

static uint16_t scl_vop_cal_scale(enum scale_mode mode, uint32_t src,
				  uint32_t dst, bool is_horizontal,
				  int vsu_mode, int *vskiplines)
{
	uint16_t val = 1 << SCL_FT_DEFAULT_FIXPOINT_SHIFT;

	if (is_horizontal) {
		if (mode == SCALE_UP)
			val = GET_SCL_FT_BIC(src, dst);
		else if (mode == SCALE_DOWN)
			val = GET_SCL_FT_BILI_DN(src, dst);
	} else {
		if (mode == SCALE_UP) {
			if (vsu_mode == SCALE_UP_BIL)
				val = GET_SCL_FT_BILI_UP(src, dst);
			else
				val = GET_SCL_FT_BIC(src, dst);
		} else if (mode == SCALE_DOWN) {
			if (vskiplines) {
				*vskiplines = scl_get_vskiplines(src, dst);
				val = scl_get_bili_dn_vskip(src, dst,
							    *vskiplines);
			} else {
				val = GET_SCL_FT_BILI_DN(src, dst);
			}
		}
	}

	return val;
}

static void scl_vop_cal_scl_fac(struct vop *vop, const struct vop_win_data *win,
			     uint32_t src_w, uint32_t src_h, uint32_t dst_w,
			     uint32_t dst_h, uint32_t pixel_format)
{
	uint16_t yrgb_hor_scl_mode, yrgb_ver_scl_mode;
	uint16_t cbcr_hor_scl_mode = SCALE_NONE;
	uint16_t cbcr_ver_scl_mode = SCALE_NONE;
	int hsub = drm_format_horz_chroma_subsampling(pixel_format);
	int vsub = drm_format_vert_chroma_subsampling(pixel_format);
	bool is_yuv = is_yuv_support(pixel_format);
	uint16_t cbcr_src_w = src_w / hsub;
	uint16_t cbcr_src_h = src_h / vsub;
	uint16_t vsu_mode;
	uint16_t lb_mode;
	uint32_t val;
<<<<<<< HEAD
	int vskiplines;

	if (dst_w > 3840) {
		DRM_ERROR("Maximum destination width (3840) exceeded\n");
=======
	int vskiplines = 0;

	if (dst_w > 3840) {
		DRM_DEV_ERROR(vop->dev, "Maximum dst width (3840) exceeded\n");
		return;
	}

	if (!win->phy->scl->ext) {
		VOP_SCL_SET(vop, win, scale_yrgb_x,
			    scl_cal_scale2(src_w, dst_w));
		VOP_SCL_SET(vop, win, scale_yrgb_y,
			    scl_cal_scale2(src_h, dst_h));
		if (is_yuv) {
			VOP_SCL_SET(vop, win, scale_cbcr_x,
				    scl_cal_scale2(cbcr_src_w, dst_w));
			VOP_SCL_SET(vop, win, scale_cbcr_y,
				    scl_cal_scale2(cbcr_src_h, dst_h));
		}
>>>>>>> v4.9.227
		return;
	}

	yrgb_hor_scl_mode = scl_get_scl_mode(src_w, dst_w);
	yrgb_ver_scl_mode = scl_get_scl_mode(src_h, dst_h);

	if (is_yuv) {
		cbcr_hor_scl_mode = scl_get_scl_mode(cbcr_src_w, dst_w);
		cbcr_ver_scl_mode = scl_get_scl_mode(cbcr_src_h, dst_h);
		if (cbcr_hor_scl_mode == SCALE_DOWN)
			lb_mode = scl_vop_cal_lb_mode(dst_w, true);
		else
			lb_mode = scl_vop_cal_lb_mode(cbcr_src_w, true);
	} else {
		if (yrgb_hor_scl_mode == SCALE_DOWN)
			lb_mode = scl_vop_cal_lb_mode(dst_w, false);
		else
			lb_mode = scl_vop_cal_lb_mode(src_w, false);
	}

<<<<<<< HEAD
	VOP_SCL_SET(vop, win, lb_mode, lb_mode);
	if (lb_mode == LB_RGB_3840X2) {
		if (yrgb_ver_scl_mode != SCALE_NONE) {
			DRM_ERROR("ERROR : not allow yrgb ver scale\n");
			return;
		}
		if (cbcr_ver_scl_mode != SCALE_NONE) {
			DRM_ERROR("ERROR : not allow cbcr ver scale\n");
=======
	VOP_SCL_SET_EXT(vop, win, lb_mode, lb_mode);
	if (lb_mode == LB_RGB_3840X2) {
		if (yrgb_ver_scl_mode != SCALE_NONE) {
			DRM_DEV_ERROR(vop->dev, "not allow yrgb ver scale\n");
			return;
		}
		if (cbcr_ver_scl_mode != SCALE_NONE) {
			DRM_DEV_ERROR(vop->dev, "not allow cbcr ver scale\n");
>>>>>>> v4.9.227
			return;
		}
		vsu_mode = SCALE_UP_BIL;
	} else if (lb_mode == LB_RGB_2560X4) {
		vsu_mode = SCALE_UP_BIL;
	} else {
		vsu_mode = SCALE_UP_BIC;
	}

	val = scl_vop_cal_scale(yrgb_hor_scl_mode, src_w, dst_w,
				true, 0, NULL);
	VOP_SCL_SET(vop, win, scale_yrgb_x, val);
	val = scl_vop_cal_scale(yrgb_ver_scl_mode, src_h, dst_h,
				false, vsu_mode, &vskiplines);
	VOP_SCL_SET(vop, win, scale_yrgb_y, val);

<<<<<<< HEAD
	VOP_SCL_SET(vop, win, vsd_yrgb_gt4, vskiplines == 4);
	VOP_SCL_SET(vop, win, vsd_yrgb_gt2, vskiplines == 2);

	VOP_SCL_SET(vop, win, yrgb_hor_scl_mode, yrgb_hor_scl_mode);
	VOP_SCL_SET(vop, win, yrgb_ver_scl_mode, yrgb_ver_scl_mode);
	VOP_SCL_SET(vop, win, yrgb_hsd_mode, SCALE_DOWN_BIL);
	VOP_SCL_SET(vop, win, yrgb_vsd_mode, SCALE_DOWN_BIL);
	VOP_SCL_SET(vop, win, yrgb_vsu_mode, vsu_mode);
=======
	VOP_SCL_SET_EXT(vop, win, vsd_yrgb_gt4, vskiplines == 4);
	VOP_SCL_SET_EXT(vop, win, vsd_yrgb_gt2, vskiplines == 2);

	VOP_SCL_SET_EXT(vop, win, yrgb_hor_scl_mode, yrgb_hor_scl_mode);
	VOP_SCL_SET_EXT(vop, win, yrgb_ver_scl_mode, yrgb_ver_scl_mode);
	VOP_SCL_SET_EXT(vop, win, yrgb_hsd_mode, SCALE_DOWN_BIL);
	VOP_SCL_SET_EXT(vop, win, yrgb_vsd_mode, SCALE_DOWN_BIL);
	VOP_SCL_SET_EXT(vop, win, yrgb_vsu_mode, vsu_mode);
>>>>>>> v4.9.227
	if (is_yuv) {
		val = scl_vop_cal_scale(cbcr_hor_scl_mode, cbcr_src_w,
					dst_w, true, 0, NULL);
		VOP_SCL_SET(vop, win, scale_cbcr_x, val);
		val = scl_vop_cal_scale(cbcr_ver_scl_mode, cbcr_src_h,
					dst_h, false, vsu_mode, &vskiplines);
		VOP_SCL_SET(vop, win, scale_cbcr_y, val);

<<<<<<< HEAD
		VOP_SCL_SET(vop, win, vsd_cbcr_gt4, vskiplines == 4);
		VOP_SCL_SET(vop, win, vsd_cbcr_gt2, vskiplines == 2);
		VOP_SCL_SET(vop, win, cbcr_hor_scl_mode, cbcr_hor_scl_mode);
		VOP_SCL_SET(vop, win, cbcr_ver_scl_mode, cbcr_ver_scl_mode);
		VOP_SCL_SET(vop, win, cbcr_hsd_mode, SCALE_DOWN_BIL);
		VOP_SCL_SET(vop, win, cbcr_vsd_mode, SCALE_DOWN_BIL);
		VOP_SCL_SET(vop, win, cbcr_vsu_mode, vsu_mode);
=======
		VOP_SCL_SET_EXT(vop, win, vsd_cbcr_gt4, vskiplines == 4);
		VOP_SCL_SET_EXT(vop, win, vsd_cbcr_gt2, vskiplines == 2);
		VOP_SCL_SET_EXT(vop, win, cbcr_hor_scl_mode, cbcr_hor_scl_mode);
		VOP_SCL_SET_EXT(vop, win, cbcr_ver_scl_mode, cbcr_ver_scl_mode);
		VOP_SCL_SET_EXT(vop, win, cbcr_hsd_mode, SCALE_DOWN_BIL);
		VOP_SCL_SET_EXT(vop, win, cbcr_vsd_mode, SCALE_DOWN_BIL);
		VOP_SCL_SET_EXT(vop, win, cbcr_vsu_mode, vsu_mode);
>>>>>>> v4.9.227
	}
}

static void vop_dsp_hold_valid_irq_enable(struct vop *vop)
{
	unsigned long flags;

	if (WARN_ON(!vop->is_enabled))
		return;

	spin_lock_irqsave(&vop->irq_lock, flags);

<<<<<<< HEAD
	vop_mask_write(vop, INTR_CTRL0, DSP_HOLD_VALID_INTR_MASK,
		       DSP_HOLD_VALID_INTR_EN(1));
=======
	VOP_INTR_SET_TYPE(vop, clear, DSP_HOLD_VALID_INTR, 1);
	VOP_INTR_SET_TYPE(vop, enable, DSP_HOLD_VALID_INTR, 1);
>>>>>>> v4.9.227

	spin_unlock_irqrestore(&vop->irq_lock, flags);
}

static void vop_dsp_hold_valid_irq_disable(struct vop *vop)
{
	unsigned long flags;

	if (WARN_ON(!vop->is_enabled))
		return;

	spin_lock_irqsave(&vop->irq_lock, flags);

<<<<<<< HEAD
	vop_mask_write(vop, INTR_CTRL0, DSP_HOLD_VALID_INTR_MASK,
		       DSP_HOLD_VALID_INTR_EN(0));
=======
	VOP_INTR_SET_TYPE(vop, enable, DSP_HOLD_VALID_INTR, 0);
>>>>>>> v4.9.227

	spin_unlock_irqrestore(&vop->irq_lock, flags);
}

<<<<<<< HEAD
static void vop_enable(struct drm_crtc *crtc)
=======
/*
 * (1) each frame starts at the start of the Vsync pulse which is signaled by
 *     the "FRAME_SYNC" interrupt.
 * (2) the active data region of each frame ends at dsp_vact_end
 * (3) we should program this same number (dsp_vact_end) into dsp_line_frag_num,
 *      to get "LINE_FLAG" interrupt at the end of the active on screen data.
 *
 * VOP_INTR_CTRL0.dsp_line_frag_num = VOP_DSP_VACT_ST_END.dsp_vact_end
 * Interrupts
 * LINE_FLAG -------------------------------+
 * FRAME_SYNC ----+                         |
 *                |                         |
 *                v                         v
 *                | Vsync | Vbp |  Vactive  | Vfp |
 *                        ^     ^           ^     ^
 *                        |     |           |     |
 *                        |     |           |     |
 * dsp_vs_end ------------+     |           |     |   VOP_DSP_VTOTAL_VS_END
 * dsp_vact_start --------------+           |     |   VOP_DSP_VACT_ST_END
 * dsp_vact_end ----------------------------+     |   VOP_DSP_VACT_ST_END
 * dsp_total -------------------------------------+   VOP_DSP_VTOTAL_VS_END
 */
static bool vop_line_flag_irq_is_enabled(struct vop *vop)
{
	uint32_t line_flag_irq;
	unsigned long flags;

	spin_lock_irqsave(&vop->irq_lock, flags);

	line_flag_irq = VOP_INTR_GET_TYPE(vop, enable, LINE_FLAG_INTR);

	spin_unlock_irqrestore(&vop->irq_lock, flags);

	return !!line_flag_irq;
}

static void vop_line_flag_irq_enable(struct vop *vop, int line_num)
{
	unsigned long flags;

	if (WARN_ON(!vop->is_enabled))
		return;

	spin_lock_irqsave(&vop->irq_lock, flags);

	VOP_CTRL_SET(vop, line_flag_num[0], line_num);
	VOP_INTR_SET_TYPE(vop, clear, LINE_FLAG_INTR, 1);
	VOP_INTR_SET_TYPE(vop, enable, LINE_FLAG_INTR, 1);

	spin_unlock_irqrestore(&vop->irq_lock, flags);
}

static void vop_line_flag_irq_disable(struct vop *vop)
{
	unsigned long flags;

	if (WARN_ON(!vop->is_enabled))
		return;

	spin_lock_irqsave(&vop->irq_lock, flags);

	VOP_INTR_SET_TYPE(vop, enable, LINE_FLAG_INTR, 0);

	spin_unlock_irqrestore(&vop->irq_lock, flags);
}

static int vop_enable(struct drm_crtc *crtc)
>>>>>>> v4.9.227
{
	struct vop *vop = to_vop(crtc);
	int ret;

<<<<<<< HEAD
	if (vop->is_enabled)
		return;

	ret = pm_runtime_get_sync(vop->dev);
	if (ret < 0) {
		dev_err(vop->dev, "failed to get pm runtime: %d\n", ret);
		return;
	}

	ret = clk_enable(vop->hclk);
	if (ret < 0) {
		dev_err(vop->dev, "failed to enable hclk - %d\n", ret);
		return;
	}

	ret = clk_enable(vop->dclk);
	if (ret < 0) {
		dev_err(vop->dev, "failed to enable dclk - %d\n", ret);
		goto err_disable_hclk;
	}

	ret = clk_enable(vop->aclk);
	if (ret < 0) {
		dev_err(vop->dev, "failed to enable aclk - %d\n", ret);
		goto err_disable_dclk;
	}
=======
	ret = pm_runtime_get_sync(vop->dev);
	if (ret < 0) {
		dev_err(vop->dev, "failed to get pm runtime: %d\n", ret);
		return ret;
	}

	ret = clk_enable(vop->hclk);
	if (WARN_ON(ret < 0))
		goto err_put_pm_runtime;

	ret = clk_enable(vop->dclk);
	if (WARN_ON(ret < 0))
		goto err_disable_hclk;

	ret = clk_enable(vop->aclk);
	if (WARN_ON(ret < 0))
		goto err_disable_dclk;
>>>>>>> v4.9.227

	/*
	 * Slave iommu shares power, irq and clock with vop.  It was associated
	 * automatically with this master device via common driver code.
	 * Now that we have enabled the clock we attach it to the shared drm
	 * mapping.
	 */
	ret = rockchip_drm_dma_attach_device(vop->drm_dev, vop->dev);
	if (ret) {
		dev_err(vop->dev, "failed to attach dma mapping, %d\n", ret);
		goto err_disable_aclk;
	}

	memcpy(vop->regs, vop->regsbak, vop->len);
	/*
	 * At here, vop clock & iommu is enable, R/W vop regs would be safe.
	 */
	vop->is_enabled = true;

	spin_lock(&vop->reg_lock);

	VOP_CTRL_SET(vop, standby, 0);

	spin_unlock(&vop->reg_lock);

	enable_irq(vop->irq);

<<<<<<< HEAD
	drm_vblank_on(vop->drm_dev, vop->pipe);

	return;
=======
	drm_crtc_vblank_on(crtc);

	return 0;
>>>>>>> v4.9.227

err_disable_aclk:
	clk_disable(vop->aclk);
err_disable_dclk:
	clk_disable(vop->dclk);
err_disable_hclk:
	clk_disable(vop->hclk);
<<<<<<< HEAD
}

static void vop_disable(struct drm_crtc *crtc)
{
	struct vop *vop = to_vop(crtc);

	if (!vop->is_enabled)
		return;

	drm_vblank_off(crtc->dev, vop->pipe);
=======
err_put_pm_runtime:
	pm_runtime_put_sync(vop->dev);
	return ret;
}

static void vop_crtc_disable(struct drm_crtc *crtc)
{
	struct vop *vop = to_vop(crtc);
	int i;

	WARN_ON(vop->event);

	rockchip_drm_psr_deactivate(&vop->crtc);

	/*
	 * We need to make sure that all windows are disabled before we
	 * disable that crtc. Otherwise we might try to scan from a destroyed
	 * buffer later.
	 */
	for (i = 0; i < vop->data->win_size; i++) {
		struct vop_win *vop_win = &vop->win[i];
		const struct vop_win_data *win = vop_win->data;

		spin_lock(&vop->reg_lock);
		VOP_WIN_SET(vop, win, enable, 0);
		spin_unlock(&vop->reg_lock);
	}

	drm_crtc_vblank_off(crtc);
>>>>>>> v4.9.227

	/*
	 * Vop standby will take effect at end of current frame,
	 * if dsp hold valid irq happen, it means standby complete.
	 *
	 * we must wait standby complete when we want to disable aclk,
	 * if not, memory bus maybe dead.
	 */
	reinit_completion(&vop->dsp_hold_completion);
	vop_dsp_hold_valid_irq_enable(vop);

	spin_lock(&vop->reg_lock);

	VOP_CTRL_SET(vop, standby, 1);

	spin_unlock(&vop->reg_lock);

	wait_for_completion(&vop->dsp_hold_completion);

	vop_dsp_hold_valid_irq_disable(vop);

	disable_irq(vop->irq);

	vop->is_enabled = false;

	/*
	 * vop standby complete, so iommu detach is safe.
	 */
	rockchip_drm_dma_detach_device(vop->drm_dev, vop->dev);

	clk_disable(vop->dclk);
	clk_disable(vop->aclk);
	clk_disable(vop->hclk);
	pm_runtime_put(vop->dev);
<<<<<<< HEAD
}

/*
 * Caller must hold vsync_mutex.
 */
static struct drm_framebuffer *vop_win_last_pending_fb(struct vop_win *vop_win)
{
	struct vop_win_state *last;
	struct vop_win_state *active = vop_win->active;

	if (list_empty(&vop_win->pending))
		return active ? active->fb : NULL;

	last = list_last_entry(&vop_win->pending, struct vop_win_state, head);
	return last ? last->fb : NULL;
}

/*
 * Caller must hold vsync_mutex.
 */
static int vop_win_queue_fb(struct vop_win *vop_win,
			    struct drm_framebuffer *fb, dma_addr_t yrgb_mst,
			    struct drm_pending_vblank_event *event)
{
	struct vop_win_state *state;

	state = kzalloc(sizeof(*state), GFP_KERNEL);
	if (!state)
		return -ENOMEM;

	state->fb = fb;
	state->yrgb_mst = yrgb_mst;
	state->event = event;

	list_add_tail(&state->head, &vop_win->pending);

	return 0;
}

static int vop_update_plane_event(struct drm_plane *plane,
				  struct drm_crtc *crtc,
				  struct drm_framebuffer *fb, int crtc_x,
				  int crtc_y, unsigned int crtc_w,
				  unsigned int crtc_h, uint32_t src_x,
				  uint32_t src_y, uint32_t src_w,
				  uint32_t src_h,
				  struct drm_pending_vblank_event *event)
{
	struct vop_win *vop_win = to_vop_win(plane);
	const struct vop_win_data *win = vop_win->data;
	struct vop *vop = to_vop(crtc);
	struct drm_gem_object *obj;
	struct rockchip_gem_object *rk_obj;
	struct drm_gem_object *uv_obj;
	struct rockchip_gem_object *rk_uv_obj;
	unsigned long offset;
	unsigned int actual_w;
	unsigned int actual_h;
	unsigned int dsp_stx;
	unsigned int dsp_sty;
	unsigned int y_vir_stride;
	unsigned int uv_vir_stride = 0;
	dma_addr_t yrgb_mst;
	dma_addr_t uv_mst = 0;
	enum vop_data_format format;
	uint32_t val;
	bool is_alpha;
	bool rb_swap;
	bool is_yuv;
	bool visible;
	int ret;
	struct drm_rect dest = {
		.x1 = crtc_x,
		.y1 = crtc_y,
		.x2 = crtc_x + crtc_w,
		.y2 = crtc_y + crtc_h,
	};
	struct drm_rect src = {
		/* 16.16 fixed point */
		.x1 = src_x,
		.y1 = src_y,
		.x2 = src_x + src_w,
		.y2 = src_y + src_h,
	};
	const struct drm_rect clip = {
		.x2 = crtc->mode.hdisplay,
		.y2 = crtc->mode.vdisplay,
	};
	bool can_position = plane->type != DRM_PLANE_TYPE_PRIMARY;
=======

	if (crtc->state->event && !crtc->state->active) {
		spin_lock_irq(&crtc->dev->event_lock);
		drm_crtc_send_vblank_event(crtc, crtc->state->event);
		spin_unlock_irq(&crtc->dev->event_lock);

		crtc->state->event = NULL;
	}
}

static void vop_plane_destroy(struct drm_plane *plane)
{
	drm_plane_cleanup(plane);
}

static int vop_plane_atomic_check(struct drm_plane *plane,
			   struct drm_plane_state *state)
{
	struct drm_crtc *crtc = state->crtc;
	struct drm_crtc_state *crtc_state;
	struct drm_framebuffer *fb = state->fb;
	struct vop_win *vop_win = to_vop_win(plane);
	const struct vop_win_data *win = vop_win->data;
	int ret;
	struct drm_rect clip;
>>>>>>> v4.9.227
	int min_scale = win->phy->scl ? FRAC_16_16(1, 8) :
					DRM_PLANE_HELPER_NO_SCALING;
	int max_scale = win->phy->scl ? FRAC_16_16(8, 1) :
					DRM_PLANE_HELPER_NO_SCALING;

<<<<<<< HEAD
	ret = drm_plane_helper_check_update(plane, crtc, fb,
					    &src, &dest, &clip,
					    min_scale,
					    max_scale,
					    can_position, false, &visible);
	if (ret)
		return ret;

	if (!visible)
		return 0;

	is_alpha = is_alpha_support(fb->pixel_format);
	rb_swap = has_rb_swapped(fb->pixel_format);
	is_yuv = is_yuv_support(fb->pixel_format);

	format = vop_convert_format(fb->pixel_format);
	if (format < 0)
		return format;

	obj = rockchip_fb_get_gem_obj(fb, 0);
	if (!obj) {
		DRM_ERROR("fail to get rockchip gem object from framebuffer\n");
		return -EINVAL;
	}

	rk_obj = to_rockchip_obj(obj);

	if (is_yuv) {
		/*
		 * Src.x1 can be odd when do clip, but yuv plane start point
		 * need align with 2 pixel.
		 */
		val = (src.x1 >> 16) % 2;
		src.x1 += val << 16;
		src.x2 += val << 16;
	}

	actual_w = (src.x2 - src.x1) >> 16;
	actual_h = (src.y2 - src.y1) >> 16;

	dsp_stx = dest.x1 + crtc->mode.htotal - crtc->mode.hsync_start;
	dsp_sty = dest.y1 + crtc->mode.vtotal - crtc->mode.vsync_start;

	offset = (src.x1 >> 16) * drm_format_plane_cpp(fb->pixel_format, 0);
	offset += (src.y1 >> 16) * fb->pitches[0];

	yrgb_mst = rk_obj->dma_addr + offset + fb->offsets[0];
	y_vir_stride = fb->pitches[0] >> 2;

	if (is_yuv) {
=======
	if (!crtc || !fb)
		return 0;

	crtc_state = drm_atomic_get_existing_crtc_state(state->state, crtc);
	if (WARN_ON(!crtc_state))
		return -EINVAL;

	clip.x1 = 0;
	clip.y1 = 0;
	clip.x2 = crtc_state->adjusted_mode.hdisplay;
	clip.y2 = crtc_state->adjusted_mode.vdisplay;

	ret = drm_plane_helper_check_state(state, &clip,
					   min_scale, max_scale,
					   true, true);
	if (ret)
		return ret;

	if (!state->visible)
		return 0;

	ret = vop_convert_format(fb->pixel_format);
	if (ret < 0)
		return ret;

	/*
	 * Src.x1 can be odd when do clip, but yuv plane start point
	 * need align with 2 pixel.
	 */
	if (is_yuv_support(fb->pixel_format) && ((state->src.x1 >> 16) % 2))
		return -EINVAL;

	return 0;
}

static void vop_plane_atomic_disable(struct drm_plane *plane,
				     struct drm_plane_state *old_state)
{
	struct vop_win *vop_win = to_vop_win(plane);
	const struct vop_win_data *win = vop_win->data;
	struct vop *vop = to_vop(old_state->crtc);

	if (!old_state->crtc)
		return;

	spin_lock(&vop->reg_lock);

	VOP_WIN_SET(vop, win, enable, 0);

	spin_unlock(&vop->reg_lock);
}

static void vop_plane_atomic_update(struct drm_plane *plane,
		struct drm_plane_state *old_state)
{
	struct drm_plane_state *state = plane->state;
	struct drm_crtc *crtc = state->crtc;
	struct vop_win *vop_win = to_vop_win(plane);
	const struct vop_win_data *win = vop_win->data;
	struct vop *vop = to_vop(state->crtc);
	struct drm_framebuffer *fb = state->fb;
	unsigned int actual_w, actual_h;
	unsigned int dsp_stx, dsp_sty;
	uint32_t act_info, dsp_info, dsp_st;
	struct drm_rect *src = &state->src;
	struct drm_rect *dest = &state->dst;
	struct drm_gem_object *obj, *uv_obj;
	struct rockchip_gem_object *rk_obj, *rk_uv_obj;
	unsigned long offset;
	dma_addr_t dma_addr;
	uint32_t val;
	bool rb_swap;
	int format;

	/*
	 * can't update plane when vop is disabled.
	 */
	if (WARN_ON(!crtc))
		return;

	if (WARN_ON(!vop->is_enabled))
		return;

	if (!state->visible) {
		vop_plane_atomic_disable(plane, old_state);
		return;
	}

	obj = rockchip_fb_get_gem_obj(fb, 0);
	rk_obj = to_rockchip_obj(obj);

	actual_w = drm_rect_width(src) >> 16;
	actual_h = drm_rect_height(src) >> 16;
	act_info = (actual_h - 1) << 16 | ((actual_w - 1) & 0xffff);

	dsp_info = (drm_rect_height(dest) - 1) << 16;
	dsp_info |= (drm_rect_width(dest) - 1) & 0xffff;

	dsp_stx = dest->x1 + crtc->mode.htotal - crtc->mode.hsync_start;
	dsp_sty = dest->y1 + crtc->mode.vtotal - crtc->mode.vsync_start;
	dsp_st = dsp_sty << 16 | (dsp_stx & 0xffff);

	offset = (src->x1 >> 16) * drm_format_plane_cpp(fb->pixel_format, 0);
	offset += (src->y1 >> 16) * fb->pitches[0];
	dma_addr = rk_obj->dma_addr + offset + fb->offsets[0];

	format = vop_convert_format(fb->pixel_format);

	spin_lock(&vop->reg_lock);

	VOP_WIN_SET(vop, win, format, format);
	VOP_WIN_SET(vop, win, yrgb_vir, fb->pitches[0] >> 2);
	VOP_WIN_SET(vop, win, yrgb_mst, dma_addr);
	if (is_yuv_support(fb->pixel_format)) {
>>>>>>> v4.9.227
		int hsub = drm_format_horz_chroma_subsampling(fb->pixel_format);
		int vsub = drm_format_vert_chroma_subsampling(fb->pixel_format);
		int bpp = drm_format_plane_cpp(fb->pixel_format, 1);

		uv_obj = rockchip_fb_get_gem_obj(fb, 1);
<<<<<<< HEAD
		if (!uv_obj) {
			DRM_ERROR("fail to get uv object from framebuffer\n");
			return -EINVAL;
		}
		rk_uv_obj = to_rockchip_obj(uv_obj);
		uv_vir_stride = fb->pitches[1] >> 2;

		offset = (src.x1 >> 16) * bpp / hsub;
		offset += (src.y1 >> 16) * fb->pitches[1] / vsub;

		uv_mst = rk_uv_obj->dma_addr + offset + fb->offsets[1];
	}

	/*
	 * If this plane update changes the plane's framebuffer, (or more
	 * precisely, if this update has a different framebuffer than the last
	 * update), enqueue it so we can track when it completes.
	 *
	 * Only when we discover that this update has completed, can we
	 * unreference any previous framebuffers.
	 */
	mutex_lock(&vop->vsync_mutex);
	if (fb != vop_win_last_pending_fb(vop_win)) {
		ret = drm_vblank_get(plane->dev, vop->pipe);
		if (ret) {
			DRM_ERROR("failed to get vblank, %d\n", ret);
			mutex_unlock(&vop->vsync_mutex);
			return ret;
		}

		drm_framebuffer_reference(fb);

		ret = vop_win_queue_fb(vop_win, fb, yrgb_mst, event);
		if (ret) {
			drm_vblank_put(plane->dev, vop->pipe);
			mutex_unlock(&vop->vsync_mutex);
			return ret;
		}

		vop->vsync_work_pending = true;
	}
	mutex_unlock(&vop->vsync_mutex);

	spin_lock(&vop->reg_lock);

	VOP_WIN_SET(vop, win, format, format);
	VOP_WIN_SET(vop, win, yrgb_vir, y_vir_stride);
	VOP_WIN_SET(vop, win, yrgb_mst, yrgb_mst);
	if (is_yuv) {
		VOP_WIN_SET(vop, win, uv_vir, uv_vir_stride);
		VOP_WIN_SET(vop, win, uv_mst, uv_mst);
=======
		rk_uv_obj = to_rockchip_obj(uv_obj);

		offset = (src->x1 >> 16) * bpp / hsub;
		offset += (src->y1 >> 16) * fb->pitches[1] / vsub;

		dma_addr = rk_uv_obj->dma_addr + offset + fb->offsets[1];
		VOP_WIN_SET(vop, win, uv_vir, fb->pitches[1] >> 2);
		VOP_WIN_SET(vop, win, uv_mst, dma_addr);
>>>>>>> v4.9.227
	}

	if (win->phy->scl)
		scl_vop_cal_scl_fac(vop, win, actual_w, actual_h,
<<<<<<< HEAD
				    dest.x2 - dest.x1, dest.y2 - dest.y1,
				    fb->pixel_format);

	val = (actual_h - 1) << 16;
	val |= (actual_w - 1) & 0xffff;
	VOP_WIN_SET(vop, win, act_info, val);

	val = (dest.y2 - dest.y1 - 1) << 16;
	val |= (dest.x2 - dest.x1 - 1) & 0xffff;
	VOP_WIN_SET(vop, win, dsp_info, val);
	val = dsp_sty << 16;
	val |= dsp_stx & 0xffff;
	VOP_WIN_SET(vop, win, dsp_st, val);
	VOP_WIN_SET(vop, win, rb_swap, rb_swap);

	if (is_alpha) {
=======
				    drm_rect_width(dest), drm_rect_height(dest),
				    fb->pixel_format);

	VOP_WIN_SET(vop, win, act_info, act_info);
	VOP_WIN_SET(vop, win, dsp_info, dsp_info);
	VOP_WIN_SET(vop, win, dsp_st, dsp_st);

	rb_swap = has_rb_swapped(fb->pixel_format);
	VOP_WIN_SET(vop, win, rb_swap, rb_swap);

	if (is_alpha_support(fb->pixel_format)) {
>>>>>>> v4.9.227
		VOP_WIN_SET(vop, win, dst_alpha_ctl,
			    DST_FACTOR_M0(ALPHA_SRC_INVERSE));
		val = SRC_ALPHA_EN(1) | SRC_COLOR_M0(ALPHA_SRC_PRE_MUL) |
			SRC_ALPHA_M0(ALPHA_STRAIGHT) |
			SRC_BLEND_M0(ALPHA_PER_PIX) |
			SRC_ALPHA_CAL_M0(ALPHA_NO_SATURATION) |
			SRC_FACTOR_M0(ALPHA_ONE);
		VOP_WIN_SET(vop, win, src_alpha_ctl, val);
	} else {
		VOP_WIN_SET(vop, win, src_alpha_ctl, SRC_ALPHA_EN(0));
	}

	VOP_WIN_SET(vop, win, enable, 1);
<<<<<<< HEAD

	vop_cfg_done(vop);
	spin_unlock(&vop->reg_lock);

	return 0;
}

static int vop_update_plane(struct drm_plane *plane, struct drm_crtc *crtc,
			    struct drm_framebuffer *fb, int crtc_x, int crtc_y,
			    unsigned int crtc_w, unsigned int crtc_h,
			    uint32_t src_x, uint32_t src_y, uint32_t src_w,
			    uint32_t src_h)
{
	return vop_update_plane_event(plane, crtc, fb, crtc_x, crtc_y, crtc_w,
				      crtc_h, src_x, src_y, src_w, src_h,
				      NULL);
}

static int vop_update_primary_plane(struct drm_crtc *crtc,
				    struct drm_pending_vblank_event *event)
{
	unsigned int crtc_w, crtc_h;

	crtc_w = crtc->primary->fb->width - crtc->x;
	crtc_h = crtc->primary->fb->height - crtc->y;

	return vop_update_plane_event(crtc->primary, crtc, crtc->primary->fb,
				      0, 0, crtc_w, crtc_h, crtc->x << 16,
				      crtc->y << 16, crtc_w << 16,
				      crtc_h << 16, event);
}

static int vop_disable_plane(struct drm_plane *plane)
{
	struct vop_win *vop_win = to_vop_win(plane);
	const struct vop_win_data *win = vop_win->data;
	struct vop *vop;
	int ret;

	if (!plane->crtc)
		return 0;

	vop = to_vop(plane->crtc);

	ret = drm_vblank_get(plane->dev, vop->pipe);
	if (ret) {
		DRM_ERROR("failed to get vblank, %d\n", ret);
		return ret;
	}

	mutex_lock(&vop->vsync_mutex);

	ret = vop_win_queue_fb(vop_win, NULL, 0, NULL);
	if (ret) {
		drm_vblank_put(plane->dev, vop->pipe);
		mutex_unlock(&vop->vsync_mutex);
		return ret;
	}

	vop->vsync_work_pending = true;
	mutex_unlock(&vop->vsync_mutex);

	spin_lock(&vop->reg_lock);
	VOP_WIN_SET(vop, win, enable, 0);
	vop_cfg_done(vop);
	spin_unlock(&vop->reg_lock);

	return 0;
}

static void vop_plane_destroy(struct drm_plane *plane)
{
	vop_disable_plane(plane);
	drm_plane_cleanup(plane);
}

static const struct drm_plane_funcs vop_plane_funcs = {
	.update_plane = vop_update_plane,
	.disable_plane = vop_disable_plane,
	.destroy = vop_plane_destroy,
};

int rockchip_drm_crtc_mode_config(struct drm_crtc *crtc,
				  int connector_type,
				  int out_mode)
{
	struct vop *vop = to_vop(crtc);

	vop->connector_type = connector_type;
	vop->connector_out_mode = out_mode;

	return 0;
}
EXPORT_SYMBOL_GPL(rockchip_drm_crtc_mode_config);
=======
	spin_unlock(&vop->reg_lock);
}

static const struct drm_plane_helper_funcs plane_helper_funcs = {
	.atomic_check = vop_plane_atomic_check,
	.atomic_update = vop_plane_atomic_update,
	.atomic_disable = vop_plane_atomic_disable,
};

static const struct drm_plane_funcs vop_plane_funcs = {
	.update_plane	= drm_atomic_helper_update_plane,
	.disable_plane	= drm_atomic_helper_disable_plane,
	.destroy = vop_plane_destroy,
	.reset = drm_atomic_helper_plane_reset,
	.atomic_duplicate_state = drm_atomic_helper_plane_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_plane_destroy_state,
};
>>>>>>> v4.9.227

static int vop_crtc_enable_vblank(struct drm_crtc *crtc)
{
	struct vop *vop = to_vop(crtc);
	unsigned long flags;

<<<<<<< HEAD
	if (!vop->is_enabled)
=======
	if (WARN_ON(!vop->is_enabled))
>>>>>>> v4.9.227
		return -EPERM;

	spin_lock_irqsave(&vop->irq_lock, flags);

<<<<<<< HEAD
	vop_mask_write(vop, INTR_CTRL0, FS_INTR_MASK, FS_INTR_EN(1));
=======
	VOP_INTR_SET_TYPE(vop, clear, FS_INTR, 1);
	VOP_INTR_SET_TYPE(vop, enable, FS_INTR, 1);
>>>>>>> v4.9.227

	spin_unlock_irqrestore(&vop->irq_lock, flags);

	return 0;
}

static void vop_crtc_disable_vblank(struct drm_crtc *crtc)
{
	struct vop *vop = to_vop(crtc);
	unsigned long flags;

<<<<<<< HEAD
	if (!vop->is_enabled)
		return;

	spin_lock_irqsave(&vop->irq_lock, flags);
	vop_mask_write(vop, INTR_CTRL0, FS_INTR_MASK, FS_INTR_EN(0));
=======
	if (WARN_ON(!vop->is_enabled))
		return;

	spin_lock_irqsave(&vop->irq_lock, flags);

	VOP_INTR_SET_TYPE(vop, enable, FS_INTR, 0);

>>>>>>> v4.9.227
	spin_unlock_irqrestore(&vop->irq_lock, flags);
}

static const struct rockchip_crtc_funcs private_crtc_funcs = {
	.enable_vblank = vop_crtc_enable_vblank,
	.disable_vblank = vop_crtc_disable_vblank,
};

<<<<<<< HEAD
static void vop_crtc_dpms(struct drm_crtc *crtc, int mode)
{
	DRM_DEBUG_KMS("crtc[%d] mode[%d]\n", crtc->base.id, mode);

	switch (mode) {
	case DRM_MODE_DPMS_ON:
		vop_enable(crtc);
		break;
	case DRM_MODE_DPMS_STANDBY:
	case DRM_MODE_DPMS_SUSPEND:
	case DRM_MODE_DPMS_OFF:
		vop_disable(crtc);
		break;
	default:
		DRM_DEBUG_KMS("unspecified mode %d\n", mode);
		break;
	}
}

static void vop_crtc_prepare(struct drm_crtc *crtc)
{
	vop_crtc_dpms(crtc, DRM_MODE_DPMS_ON);
}

=======
>>>>>>> v4.9.227
static bool vop_crtc_mode_fixup(struct drm_crtc *crtc,
				const struct drm_display_mode *mode,
				struct drm_display_mode *adjusted_mode)
{
<<<<<<< HEAD
	if (adjusted_mode->htotal == 0 || adjusted_mode->vtotal == 0)
		return false;
=======
	struct vop *vop = to_vop(crtc);

	adjusted_mode->clock =
		DIV_ROUND_UP(clk_round_rate(vop->dclk, mode->clock * 1000),
			     1000);
>>>>>>> v4.9.227

	return true;
}

<<<<<<< HEAD
static int vop_crtc_mode_set_base(struct drm_crtc *crtc, int x, int y,
				  struct drm_framebuffer *old_fb)
{
	int ret;

	crtc->x = x;
	crtc->y = y;

	ret = vop_update_primary_plane(crtc, NULL);
	if (ret < 0) {
		DRM_ERROR("fail to update plane\n");
		return ret;
	}

	return 0;
}

static int vop_crtc_mode_set(struct drm_crtc *crtc,
			     struct drm_display_mode *mode,
			     struct drm_display_mode *adjusted_mode,
			     int x, int y, struct drm_framebuffer *fb)
{
	struct vop *vop = to_vop(crtc);
=======
static void vop_crtc_enable(struct drm_crtc *crtc)
{
	struct vop *vop = to_vop(crtc);
	struct rockchip_crtc_state *s = to_rockchip_crtc_state(crtc->state);
	struct drm_display_mode *adjusted_mode = &crtc->state->adjusted_mode;
>>>>>>> v4.9.227
	u16 hsync_len = adjusted_mode->hsync_end - adjusted_mode->hsync_start;
	u16 hdisplay = adjusted_mode->hdisplay;
	u16 htotal = adjusted_mode->htotal;
	u16 hact_st = adjusted_mode->htotal - adjusted_mode->hsync_start;
	u16 hact_end = hact_st + hdisplay;
	u16 vdisplay = adjusted_mode->vdisplay;
	u16 vtotal = adjusted_mode->vtotal;
	u16 vsync_len = adjusted_mode->vsync_end - adjusted_mode->vsync_start;
	u16 vact_st = adjusted_mode->vtotal - adjusted_mode->vsync_start;
	u16 vact_end = vact_st + vdisplay;
<<<<<<< HEAD
	int ret, ret_clk;
	uint32_t val;

	/*
	 * disable dclk to stop frame scan, so that we can safe config mode and
	 * enable iommu.
	 */
	clk_disable(vop->dclk);

	switch (vop->connector_type) {
	case DRM_MODE_CONNECTOR_LVDS:
		VOP_CTRL_SET(vop, rgb_en, 1);
		break;
	case DRM_MODE_CONNECTOR_eDP:
		VOP_CTRL_SET(vop, edp_en, 1);
		break;
	case DRM_MODE_CONNECTOR_HDMIA:
		VOP_CTRL_SET(vop, hdmi_en, 1);
		break;
	default:
		DRM_ERROR("unsupport connector_type[%d]\n",
			  vop->connector_type);
		ret = -EINVAL;
		goto out;
	};
	VOP_CTRL_SET(vop, out_mode, vop->connector_out_mode);

	val = 0x8;
	val |= (adjusted_mode->flags & DRM_MODE_FLAG_NHSYNC) ? 0 : 1;
	val |= (adjusted_mode->flags & DRM_MODE_FLAG_NVSYNC) ? 0 : (1 << 1);
	VOP_CTRL_SET(vop, pin_pol, val);
=======
	uint32_t pin_pol, val;
	int ret;

	WARN_ON(vop->event);

	ret = vop_enable(crtc);
	if (ret) {
		DRM_DEV_ERROR(vop->dev, "Failed to enable vop (%d)\n", ret);
		return;
	}

	/*
	 * If dclk rate is zero, mean that scanout is stop,
	 * we don't need wait any more.
	 */
	if (clk_get_rate(vop->dclk)) {
		/*
		 * Rk3288 vop timing register is immediately, when configure
		 * display timing on display time, may cause tearing.
		 *
		 * Vop standby will take effect at end of current frame,
		 * if dsp hold valid irq happen, it means standby complete.
		 *
		 * mode set:
		 *    standby and wait complete --> |----
		 *                                  | display time
		 *                                  |----
		 *                                  |---> dsp hold irq
		 *     configure display timing --> |
		 *         standby exit             |
		 *                                  | new frame start.
		 */

		reinit_completion(&vop->dsp_hold_completion);
		vop_dsp_hold_valid_irq_enable(vop);

		spin_lock(&vop->reg_lock);

		VOP_CTRL_SET(vop, standby, 1);

		spin_unlock(&vop->reg_lock);

		wait_for_completion(&vop->dsp_hold_completion);

		vop_dsp_hold_valid_irq_disable(vop);
	}

	pin_pol = 0x8;
	pin_pol |= (adjusted_mode->flags & DRM_MODE_FLAG_NHSYNC) ? 0 : 1;
	pin_pol |= (adjusted_mode->flags & DRM_MODE_FLAG_NVSYNC) ? 0 : (1 << 1);
	VOP_CTRL_SET(vop, pin_pol, pin_pol);

	switch (s->output_type) {
	case DRM_MODE_CONNECTOR_LVDS:
		VOP_CTRL_SET(vop, rgb_en, 1);
		VOP_CTRL_SET(vop, rgb_pin_pol, pin_pol);
		break;
	case DRM_MODE_CONNECTOR_eDP:
		VOP_CTRL_SET(vop, edp_pin_pol, pin_pol);
		VOP_CTRL_SET(vop, edp_en, 1);
		break;
	case DRM_MODE_CONNECTOR_HDMIA:
		VOP_CTRL_SET(vop, hdmi_pin_pol, pin_pol);
		VOP_CTRL_SET(vop, hdmi_en, 1);
		break;
	case DRM_MODE_CONNECTOR_DSI:
		VOP_CTRL_SET(vop, mipi_pin_pol, pin_pol);
		VOP_CTRL_SET(vop, mipi_en, 1);
		break;
	default:
		DRM_DEV_ERROR(vop->dev, "unsupported connector_type [%d]\n",
			      s->output_type);
	}
	VOP_CTRL_SET(vop, out_mode, s->output_mode);
>>>>>>> v4.9.227

	VOP_CTRL_SET(vop, htotal_pw, (htotal << 16) | hsync_len);
	val = hact_st << 16;
	val |= hact_end;
	VOP_CTRL_SET(vop, hact_st_end, val);
	VOP_CTRL_SET(vop, hpost_st_end, val);

	VOP_CTRL_SET(vop, vtotal_pw, (vtotal << 16) | vsync_len);
	val = vact_st << 16;
	val |= vact_end;
	VOP_CTRL_SET(vop, vact_st_end, val);
	VOP_CTRL_SET(vop, vpost_st_end, val);

<<<<<<< HEAD
	ret = vop_crtc_mode_set_base(crtc, x, y, fb);
	if (ret)
		goto out;

	/*
	 * reset dclk, take all mode config affect, so the clk would run in
	 * correct frame.
	 */
	reset_control_assert(vop->dclk_rst);
	usleep_range(10, 20);
	reset_control_deassert(vop->dclk_rst);

	clk_set_rate(vop->dclk, adjusted_mode->clock * 1000);
out:
	ret_clk = clk_enable(vop->dclk);
	if (ret_clk < 0) {
		dev_err(vop->dev, "failed to enable dclk - %d\n", ret_clk);
		return ret_clk;
	}

	return ret;
}

static void vop_crtc_commit(struct drm_crtc *crtc)
{
}

static const struct drm_crtc_helper_funcs vop_crtc_helper_funcs = {
	.dpms = vop_crtc_dpms,
	.prepare = vop_crtc_prepare,
	.mode_fixup = vop_crtc_mode_fixup,
	.mode_set = vop_crtc_mode_set,
	.mode_set_base = vop_crtc_mode_set_base,
	.commit = vop_crtc_commit,
};

static int vop_crtc_page_flip(struct drm_crtc *crtc,
			      struct drm_framebuffer *fb,
			      struct drm_pending_vblank_event *event,
			      uint32_t page_flip_flags)
{
	struct vop *vop = to_vop(crtc);
	struct drm_framebuffer *old_fb = crtc->primary->fb;
	int ret;

	/* when the page flip is requested, crtc should be on */
	if (!vop->is_enabled) {
		DRM_DEBUG("page flip request rejected because crtc is off.\n");
		return 0;
	}

	crtc->primary->fb = fb;

	ret = vop_update_primary_plane(crtc, event);
	if (ret)
		crtc->primary->fb = old_fb;

	return ret;
}

static void vop_win_state_complete(struct vop_win *vop_win,
				   struct vop_win_state *state)
{
	struct vop *vop = vop_win->vop;
	struct drm_crtc *crtc = &vop->crtc;
	struct drm_device *drm = crtc->dev;
	unsigned long flags;

	if (state->event) {
		spin_lock_irqsave(&drm->event_lock, flags);
		drm_crtc_send_vblank_event(crtc, state->event);
		spin_unlock_irqrestore(&drm->event_lock, flags);
	}

	list_del(&state->head);
	drm_vblank_put(crtc->dev, vop->pipe);
}

=======
	clk_set_rate(vop->dclk, adjusted_mode->clock * 1000);

	VOP_CTRL_SET(vop, standby, 0);

	rockchip_drm_psr_activate(&vop->crtc);
}

static bool vop_fs_irq_is_pending(struct vop *vop)
{
	return VOP_INTR_GET_TYPE(vop, status, FS_INTR);
}

static void vop_wait_for_irq_handler(struct vop *vop)
{
	bool pending;
	int ret;

	/*
	 * Spin until frame start interrupt status bit goes low, which means
	 * that interrupt handler was invoked and cleared it. The timeout of
	 * 10 msecs is really too long, but it is just a safety measure if
	 * something goes really wrong. The wait will only happen in the very
	 * unlikely case of a vblank happening exactly at the same time and
	 * shouldn't exceed microseconds range.
	 */
	ret = readx_poll_timeout_atomic(vop_fs_irq_is_pending, vop, pending,
					!pending, 0, 10 * 1000);
	if (ret)
		DRM_DEV_ERROR(vop->dev, "VOP vblank IRQ stuck for 10 ms\n");

	synchronize_irq(vop->irq);
}

static void vop_crtc_atomic_flush(struct drm_crtc *crtc,
				  struct drm_crtc_state *old_crtc_state)
{
	struct drm_atomic_state *old_state = old_crtc_state->state;
	struct drm_plane_state *old_plane_state;
	struct vop *vop = to_vop(crtc);
	struct drm_plane *plane;
	int i;

	if (WARN_ON(!vop->is_enabled))
		return;

	spin_lock(&vop->reg_lock);

	vop_cfg_done(vop);

	spin_unlock(&vop->reg_lock);

	/*
	 * There is a (rather unlikely) possiblity that a vblank interrupt
	 * fired before we set the cfg_done bit. To avoid spuriously
	 * signalling flip completion we need to wait for it to finish.
	 */
	vop_wait_for_irq_handler(vop);

	spin_lock_irq(&crtc->dev->event_lock);
	if (crtc->state->event) {
		WARN_ON(drm_crtc_vblank_get(crtc) != 0);
		WARN_ON(vop->event);

		vop->event = crtc->state->event;
		crtc->state->event = NULL;
	}
	spin_unlock_irq(&crtc->dev->event_lock);

	for_each_plane_in_state(old_state, plane, old_plane_state, i) {
		if (!old_plane_state->fb)
			continue;

		if (old_plane_state->fb == plane->state->fb)
			continue;

		drm_framebuffer_reference(old_plane_state->fb);
		drm_flip_work_queue(&vop->fb_unref_work, old_plane_state->fb);
		set_bit(VOP_PENDING_FB_UNREF, &vop->pending);
		WARN_ON(drm_crtc_vblank_get(crtc) != 0);
	}
}

static void vop_crtc_atomic_begin(struct drm_crtc *crtc,
				  struct drm_crtc_state *old_crtc_state)
{
	rockchip_drm_psr_flush(crtc);
}

static const struct drm_crtc_helper_funcs vop_crtc_helper_funcs = {
	.enable = vop_crtc_enable,
	.disable = vop_crtc_disable,
	.mode_fixup = vop_crtc_mode_fixup,
	.atomic_flush = vop_crtc_atomic_flush,
	.atomic_begin = vop_crtc_atomic_begin,
};

>>>>>>> v4.9.227
static void vop_crtc_destroy(struct drm_crtc *crtc)
{
	drm_crtc_cleanup(crtc);
}

<<<<<<< HEAD
static const struct drm_crtc_funcs vop_crtc_funcs = {
	.set_config = drm_crtc_helper_set_config,
	.page_flip = vop_crtc_page_flip,
	.destroy = vop_crtc_destroy,
};

static bool vop_win_state_is_active(struct vop_win *vop_win,
				    struct vop_win_state *state)
{
	bool active = false;

	if (state->fb) {
		dma_addr_t yrgb_mst;

		/* check yrgb_mst to tell if pending_fb is now front */
		yrgb_mst = VOP_WIN_GET_YRGBADDR(vop_win->vop, vop_win->data);

		active = (yrgb_mst == state->yrgb_mst);
	} else {
		bool enabled;

		/* if enable bit is clear, plane is now disabled */
		enabled = VOP_WIN_GET(vop_win->vop, vop_win->data, enable);

		active = (enabled == 0);
	}

	return active;
}

static void vop_win_state_destroy(struct vop_win_state *state)
{
	struct drm_framebuffer *fb = state->fb;

	if (fb)
		drm_framebuffer_unreference(fb);

	kfree(state);
}

static void vop_win_update_state(struct vop_win *vop_win)
{
	struct vop_win_state *state, *n, *new_active = NULL;

	/* Check if any pending states are now active */
	list_for_each_entry(state, &vop_win->pending, head)
		if (vop_win_state_is_active(vop_win, state)) {
			new_active = state;
			break;
		}

	if (!new_active)
		return;

	/*
	 * Destroy any 'skipped' pending states - states that were queued
	 * before the newly active state.
	 */
	list_for_each_entry_safe(state, n, &vop_win->pending, head) {
		if (state == new_active)
			break;
		vop_win_state_complete(vop_win, state);
		vop_win_state_destroy(state);
	}

	vop_win_state_complete(vop_win, new_active);

	if (vop_win->active)
		vop_win_state_destroy(vop_win->active);
	vop_win->active = new_active;
}

static bool vop_win_has_pending_state(struct vop_win *vop_win)
{
	return !list_empty(&vop_win->pending);
}

static irqreturn_t vop_isr_thread(int irq, void *data)
{
	struct vop *vop = data;
	const struct vop_data *vop_data = vop->data;
	unsigned int i;

	mutex_lock(&vop->vsync_mutex);

	if (!vop->vsync_work_pending)
		goto done;

	vop->vsync_work_pending = false;

	for (i = 0; i < vop_data->win_size; i++) {
		struct vop_win *vop_win = &vop->win[i];

		vop_win_update_state(vop_win);
		if (vop_win_has_pending_state(vop_win))
			vop->vsync_work_pending = true;
	}

done:
	mutex_unlock(&vop->vsync_mutex);

	return IRQ_HANDLED;
=======
static void vop_crtc_reset(struct drm_crtc *crtc)
{
	if (crtc->state)
		__drm_atomic_helper_crtc_destroy_state(crtc->state);
	kfree(crtc->state);

	crtc->state = kzalloc(sizeof(struct rockchip_crtc_state), GFP_KERNEL);
	if (crtc->state)
		crtc->state->crtc = crtc;
}

static struct drm_crtc_state *vop_crtc_duplicate_state(struct drm_crtc *crtc)
{
	struct rockchip_crtc_state *rockchip_state;

	rockchip_state = kzalloc(sizeof(*rockchip_state), GFP_KERNEL);
	if (!rockchip_state)
		return NULL;

	__drm_atomic_helper_crtc_duplicate_state(crtc, &rockchip_state->base);
	return &rockchip_state->base;
}

static void vop_crtc_destroy_state(struct drm_crtc *crtc,
				   struct drm_crtc_state *state)
{
	struct rockchip_crtc_state *s = to_rockchip_crtc_state(state);

	__drm_atomic_helper_crtc_destroy_state(&s->base);
	kfree(s);
}

static const struct drm_crtc_funcs vop_crtc_funcs = {
	.set_config = drm_atomic_helper_set_config,
	.page_flip = drm_atomic_helper_page_flip,
	.destroy = vop_crtc_destroy,
	.reset = vop_crtc_reset,
	.atomic_duplicate_state = vop_crtc_duplicate_state,
	.atomic_destroy_state = vop_crtc_destroy_state,
};

static void vop_fb_unref_worker(struct drm_flip_work *work, void *val)
{
	struct vop *vop = container_of(work, struct vop, fb_unref_work);
	struct drm_framebuffer *fb = val;

	drm_crtc_vblank_put(&vop->crtc);
	drm_framebuffer_unreference(fb);
}

static void vop_handle_vblank(struct vop *vop)
{
	struct drm_device *drm = vop->drm_dev;
	struct drm_crtc *crtc = &vop->crtc;
	unsigned long flags;

	spin_lock_irqsave(&drm->event_lock, flags);
	if (vop->event) {
		drm_crtc_send_vblank_event(crtc, vop->event);
		drm_crtc_vblank_put(crtc);
		vop->event = NULL;
	}
	spin_unlock_irqrestore(&drm->event_lock, flags);

	if (test_and_clear_bit(VOP_PENDING_FB_UNREF, &vop->pending))
		drm_flip_work_commit(&vop->fb_unref_work, system_unbound_wq);
>>>>>>> v4.9.227
}

static irqreturn_t vop_isr(int irq, void *data)
{
	struct vop *vop = data;
<<<<<<< HEAD
	uint32_t intr0_reg, active_irqs;
=======
	struct drm_crtc *crtc = &vop->crtc;
	uint32_t active_irqs;
>>>>>>> v4.9.227
	unsigned long flags;
	int ret = IRQ_NONE;

	/*
<<<<<<< HEAD
	 * INTR_CTRL0 register has interrupt status, enable and clear bits, we
	 * must hold irq_lock to avoid a race with enable/disable_vblank().
	*/
	spin_lock_irqsave(&vop->irq_lock, flags);
	intr0_reg = vop_readl(vop, INTR_CTRL0);
	active_irqs = intr0_reg & INTR_MASK;
	/* Clear all active interrupt sources */
	if (active_irqs)
		vop_writel(vop, INTR_CTRL0,
			   intr0_reg | (active_irqs << INTR_CLR_SHIFT));
=======
	 * interrupt register has interrupt status, enable and clear bits, we
	 * must hold irq_lock to avoid a race with enable/disable_vblank().
	*/
	spin_lock_irqsave(&vop->irq_lock, flags);

	active_irqs = VOP_INTR_GET_TYPE(vop, status, INTR_MASK);
	/* Clear all active interrupt sources */
	if (active_irqs)
		VOP_INTR_SET_TYPE(vop, clear, active_irqs, 1);

>>>>>>> v4.9.227
	spin_unlock_irqrestore(&vop->irq_lock, flags);

	/* This is expected for vop iommu irqs, since the irq is shared */
	if (!active_irqs)
		return IRQ_NONE;

	if (active_irqs & DSP_HOLD_VALID_INTR) {
		complete(&vop->dsp_hold_completion);
		active_irqs &= ~DSP_HOLD_VALID_INTR;
		ret = IRQ_HANDLED;
	}

<<<<<<< HEAD
	if (active_irqs & FS_INTR) {
		drm_handle_vblank(vop->drm_dev, vop->pipe);
		active_irqs &= ~FS_INTR;
		ret = (vop->vsync_work_pending) ? IRQ_WAKE_THREAD : IRQ_HANDLED;
=======
	if (active_irqs & LINE_FLAG_INTR) {
		complete(&vop->line_flag_completion);
		active_irqs &= ~LINE_FLAG_INTR;
		ret = IRQ_HANDLED;
	}

	if (active_irqs & FS_INTR) {
		drm_crtc_handle_vblank(crtc);
		vop_handle_vblank(vop);
		active_irqs &= ~FS_INTR;
		ret = IRQ_HANDLED;
>>>>>>> v4.9.227
	}

	/* Unhandled irqs are spurious. */
	if (active_irqs)
<<<<<<< HEAD
		DRM_ERROR("Unknown VOP IRQs: %#02x\n", active_irqs);
=======
		DRM_DEV_ERROR(vop->dev, "Unknown VOP IRQs: %#02x\n",
			      active_irqs);
>>>>>>> v4.9.227

	return ret;
}

static int vop_create_crtc(struct vop *vop)
{
	const struct vop_data *vop_data = vop->data;
	struct device *dev = vop->dev;
	struct drm_device *drm_dev = vop->drm_dev;
<<<<<<< HEAD
	struct drm_plane *primary = NULL, *cursor = NULL, *plane;
=======
	struct drm_plane *primary = NULL, *cursor = NULL, *plane, *tmp;
>>>>>>> v4.9.227
	struct drm_crtc *crtc = &vop->crtc;
	struct device_node *port;
	int ret;
	int i;

	/*
	 * Create drm_plane for primary and cursor planes first, since we need
	 * to pass them to drm_crtc_init_with_planes, which sets the
	 * "possible_crtcs" to the newly initialized crtc.
	 */
	for (i = 0; i < vop_data->win_size; i++) {
		struct vop_win *vop_win = &vop->win[i];
		const struct vop_win_data *win_data = vop_win->data;

		if (win_data->type != DRM_PLANE_TYPE_PRIMARY &&
		    win_data->type != DRM_PLANE_TYPE_CURSOR)
			continue;

		ret = drm_universal_plane_init(vop->drm_dev, &vop_win->base,
					       0, &vop_plane_funcs,
					       win_data->phy->data_formats,
					       win_data->phy->nformats,
<<<<<<< HEAD
					       win_data->type);
		if (ret) {
			DRM_ERROR("failed to initialize plane\n");
=======
					       win_data->type, NULL);
		if (ret) {
			DRM_DEV_ERROR(vop->dev, "failed to init plane %d\n",
				      ret);
>>>>>>> v4.9.227
			goto err_cleanup_planes;
		}

		plane = &vop_win->base;
<<<<<<< HEAD
=======
		drm_plane_helper_add(plane, &plane_helper_funcs);
>>>>>>> v4.9.227
		if (plane->type == DRM_PLANE_TYPE_PRIMARY)
			primary = plane;
		else if (plane->type == DRM_PLANE_TYPE_CURSOR)
			cursor = plane;
	}

	ret = drm_crtc_init_with_planes(drm_dev, crtc, primary, cursor,
<<<<<<< HEAD
					&vop_crtc_funcs);
	if (ret)
		return ret;
=======
					&vop_crtc_funcs, NULL);
	if (ret)
		goto err_cleanup_planes;
>>>>>>> v4.9.227

	drm_crtc_helper_add(crtc, &vop_crtc_helper_funcs);

	/*
	 * Create drm_planes for overlay windows with possible_crtcs restricted
	 * to the newly created crtc.
	 */
	for (i = 0; i < vop_data->win_size; i++) {
		struct vop_win *vop_win = &vop->win[i];
		const struct vop_win_data *win_data = vop_win->data;
		unsigned long possible_crtcs = 1 << drm_crtc_index(crtc);

		if (win_data->type != DRM_PLANE_TYPE_OVERLAY)
			continue;

		ret = drm_universal_plane_init(vop->drm_dev, &vop_win->base,
					       possible_crtcs,
					       &vop_plane_funcs,
					       win_data->phy->data_formats,
					       win_data->phy->nformats,
<<<<<<< HEAD
					       win_data->type);
		if (ret) {
			DRM_ERROR("failed to initialize overlay plane\n");
			goto err_cleanup_crtc;
		}
=======
					       win_data->type, NULL);
		if (ret) {
			DRM_DEV_ERROR(vop->dev, "failed to init overlay %d\n",
				      ret);
			goto err_cleanup_crtc;
		}
		drm_plane_helper_add(&vop_win->base, &plane_helper_funcs);
>>>>>>> v4.9.227
	}

	port = of_get_child_by_name(dev->of_node, "port");
	if (!port) {
<<<<<<< HEAD
		DRM_ERROR("no port node found in %s\n",
			  dev->of_node->full_name);
		goto err_cleanup_crtc;
	}

	init_completion(&vop->dsp_hold_completion);
	crtc->port = port;
	vop->pipe = drm_crtc_index(crtc);
	rockchip_register_crtc_funcs(drm_dev, &private_crtc_funcs, vop->pipe);
=======
		DRM_DEV_ERROR(vop->dev, "no port node found in %s\n",
			      dev->of_node->full_name);
		ret = -ENOENT;
		goto err_cleanup_crtc;
	}

	drm_flip_work_init(&vop->fb_unref_work, "fb_unref",
			   vop_fb_unref_worker);

	init_completion(&vop->dsp_hold_completion);
	init_completion(&vop->line_flag_completion);
	crtc->port = port;
	rockchip_register_crtc_funcs(crtc, &private_crtc_funcs);
>>>>>>> v4.9.227

	return 0;

err_cleanup_crtc:
	drm_crtc_cleanup(crtc);
err_cleanup_planes:
<<<<<<< HEAD
	list_for_each_entry(plane, &drm_dev->mode_config.plane_list, head)
=======
	list_for_each_entry_safe(plane, tmp, &drm_dev->mode_config.plane_list,
				 head)
>>>>>>> v4.9.227
		drm_plane_cleanup(plane);
	return ret;
}

static void vop_destroy_crtc(struct vop *vop)
{
	struct drm_crtc *crtc = &vop->crtc;
<<<<<<< HEAD

	rockchip_unregister_crtc_funcs(vop->drm_dev, vop->pipe);
	of_node_put(crtc->port);
	drm_crtc_cleanup(crtc);
=======
	struct drm_device *drm_dev = vop->drm_dev;
	struct drm_plane *plane, *tmp;

	rockchip_unregister_crtc_funcs(crtc);
	of_node_put(crtc->port);

	/*
	 * We need to cleanup the planes now.  Why?
	 *
	 * The planes are "&vop->win[i].base".  That means the memory is
	 * all part of the big "struct vop" chunk of memory.  That memory
	 * was devm allocated and associated with this component.  We need to
	 * free it ourselves before vop_unbind() finishes.
	 */
	list_for_each_entry_safe(plane, tmp, &drm_dev->mode_config.plane_list,
				 head)
		vop_plane_destroy(plane);

	/*
	 * Destroy CRTC after vop_plane_destroy() since vop_disable_plane()
	 * references the CRTC.
	 */
	drm_crtc_cleanup(crtc);
	drm_flip_work_cleanup(&vop->fb_unref_work);
>>>>>>> v4.9.227
}

static int vop_initial(struct vop *vop)
{
	const struct vop_data *vop_data = vop->data;
	const struct vop_reg_data *init_table = vop_data->init_table;
	struct reset_control *ahb_rst;
	int i, ret;

	vop->hclk = devm_clk_get(vop->dev, "hclk_vop");
	if (IS_ERR(vop->hclk)) {
		dev_err(vop->dev, "failed to get hclk source\n");
		return PTR_ERR(vop->hclk);
	}
	vop->aclk = devm_clk_get(vop->dev, "aclk_vop");
	if (IS_ERR(vop->aclk)) {
		dev_err(vop->dev, "failed to get aclk source\n");
		return PTR_ERR(vop->aclk);
	}
	vop->dclk = devm_clk_get(vop->dev, "dclk_vop");
	if (IS_ERR(vop->dclk)) {
		dev_err(vop->dev, "failed to get dclk source\n");
		return PTR_ERR(vop->dclk);
	}

<<<<<<< HEAD
	ret = clk_prepare(vop->dclk);
	if (ret < 0) {
		dev_err(vop->dev, "failed to prepare dclk\n");
		return ret;
=======
	ret = pm_runtime_get_sync(vop->dev);
	if (ret < 0) {
		dev_err(vop->dev, "failed to get pm runtime: %d\n", ret);
		return ret;
	}

	ret = clk_prepare(vop->dclk);
	if (ret < 0) {
		dev_err(vop->dev, "failed to prepare dclk\n");
		goto err_put_pm_runtime;
>>>>>>> v4.9.227
	}

	/* Enable both the hclk and aclk to setup the vop */
	ret = clk_prepare_enable(vop->hclk);
	if (ret < 0) {
		dev_err(vop->dev, "failed to prepare/enable hclk\n");
		goto err_unprepare_dclk;
	}

	ret = clk_prepare_enable(vop->aclk);
	if (ret < 0) {
		dev_err(vop->dev, "failed to prepare/enable aclk\n");
		goto err_disable_hclk;
	}

	/*
	 * do hclk_reset, reset all vop registers.
	 */
	ahb_rst = devm_reset_control_get(vop->dev, "ahb");
	if (IS_ERR(ahb_rst)) {
		dev_err(vop->dev, "failed to get ahb reset\n");
		ret = PTR_ERR(ahb_rst);
		goto err_disable_aclk;
	}
	reset_control_assert(ahb_rst);
	usleep_range(10, 20);
	reset_control_deassert(ahb_rst);

<<<<<<< HEAD
=======
	VOP_INTR_SET_TYPE(vop, clear, INTR_MASK, 1);
	VOP_INTR_SET_TYPE(vop, enable, INTR_MASK, 0);

>>>>>>> v4.9.227
	memcpy(vop->regsbak, vop->regs, vop->len);

	for (i = 0; i < vop_data->table_size; i++)
		vop_writel(vop, init_table[i].offset, init_table[i].value);

	for (i = 0; i < vop_data->win_size; i++) {
		const struct vop_win_data *win = &vop_data->win[i];

		VOP_WIN_SET(vop, win, enable, 0);
	}

	vop_cfg_done(vop);

	/*
	 * do dclk_reset, let all config take affect.
	 */
	vop->dclk_rst = devm_reset_control_get(vop->dev, "dclk");
	if (IS_ERR(vop->dclk_rst)) {
		dev_err(vop->dev, "failed to get dclk reset\n");
		ret = PTR_ERR(vop->dclk_rst);
		goto err_disable_aclk;
	}
	reset_control_assert(vop->dclk_rst);
	usleep_range(10, 20);
	reset_control_deassert(vop->dclk_rst);

	clk_disable(vop->hclk);
	clk_disable(vop->aclk);

	vop->is_enabled = false;

<<<<<<< HEAD
=======
	pm_runtime_put_sync(vop->dev);

>>>>>>> v4.9.227
	return 0;

err_disable_aclk:
	clk_disable_unprepare(vop->aclk);
err_disable_hclk:
	clk_disable_unprepare(vop->hclk);
err_unprepare_dclk:
	clk_unprepare(vop->dclk);
<<<<<<< HEAD
=======
err_put_pm_runtime:
	pm_runtime_put_sync(vop->dev);
>>>>>>> v4.9.227
	return ret;
}

/*
 * Initialize the vop->win array elements.
 */
static void vop_win_init(struct vop *vop)
{
	const struct vop_data *vop_data = vop->data;
	unsigned int i;

	for (i = 0; i < vop_data->win_size; i++) {
		struct vop_win *vop_win = &vop->win[i];
		const struct vop_win_data *win_data = &vop_data->win[i];

		vop_win->data = win_data;
		vop_win->vop = vop;
<<<<<<< HEAD
		INIT_LIST_HEAD(&vop_win->pending);
	}
}

static int vop_bind(struct device *dev, struct device *master, void *data)
{
	struct platform_device *pdev = to_platform_device(dev);
	const struct of_device_id *of_id;
=======
	}
}

/**
 * rockchip_drm_wait_line_flag - acqiure the give line flag event
 * @crtc: CRTC to enable line flag
 * @line_num: interested line number
 * @mstimeout: millisecond for timeout
 *
 * Driver would hold here until the interested line flag interrupt have
 * happened or timeout to wait.
 *
 * Returns:
 * Zero on success, negative errno on failure.
 */
int rockchip_drm_wait_line_flag(struct drm_crtc *crtc, unsigned int line_num,
				unsigned int mstimeout)
{
	struct vop *vop = to_vop(crtc);
	unsigned long jiffies_left;

	if (!crtc || !vop->is_enabled)
		return -ENODEV;

	if (line_num > crtc->mode.vtotal || mstimeout <= 0)
		return -EINVAL;

	if (vop_line_flag_irq_is_enabled(vop))
		return -EBUSY;

	reinit_completion(&vop->line_flag_completion);
	vop_line_flag_irq_enable(vop, line_num);

	jiffies_left = wait_for_completion_timeout(&vop->line_flag_completion,
						   msecs_to_jiffies(mstimeout));
	vop_line_flag_irq_disable(vop);

	if (jiffies_left == 0) {
		dev_err(vop->dev, "Timeout waiting for IRQ\n");
		return -ETIMEDOUT;
	}

	return 0;
}
EXPORT_SYMBOL(rockchip_drm_wait_line_flag);

static int vop_bind(struct device *dev, struct device *master, void *data)
{
	struct platform_device *pdev = to_platform_device(dev);
>>>>>>> v4.9.227
	const struct vop_data *vop_data;
	struct drm_device *drm_dev = data;
	struct vop *vop;
	struct resource *res;
	size_t alloc_size;
	int ret, irq;

<<<<<<< HEAD
	of_id = of_match_device(vop_driver_dt_match, dev);
	vop_data = of_id->data;
=======
	vop_data = of_device_get_match_data(dev);
>>>>>>> v4.9.227
	if (!vop_data)
		return -ENODEV;

	/* Allocate vop struct and its vop_win array */
	alloc_size = sizeof(*vop) + sizeof(*vop->win) * vop_data->win_size;
	vop = devm_kzalloc(dev, alloc_size, GFP_KERNEL);
	if (!vop)
		return -ENOMEM;

	vop->dev = dev;
	vop->data = vop_data;
	vop->drm_dev = drm_dev;
	dev_set_drvdata(dev, vop);

	vop_win_init(vop);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	vop->len = resource_size(res);
	vop->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(vop->regs))
		return PTR_ERR(vop->regs);

	vop->regsbak = devm_kzalloc(dev, vop->len, GFP_KERNEL);
	if (!vop->regsbak)
		return -ENOMEM;

<<<<<<< HEAD
	ret = vop_initial(vop);
	if (ret < 0) {
		dev_err(&pdev->dev, "cannot initial vop dev - err %d\n", ret);
		return ret;
	}

=======
>>>>>>> v4.9.227
	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(dev, "cannot find irq for vop\n");
		return irq;
	}
	vop->irq = (unsigned int)irq;

	spin_lock_init(&vop->reg_lock);
	spin_lock_init(&vop->irq_lock);

	mutex_init(&vop->vsync_mutex);

<<<<<<< HEAD
	ret = devm_request_threaded_irq(dev, vop->irq, vop_isr, vop_isr_thread,
					IRQF_SHARED, dev_name(dev), vop);
	if (ret)
		return ret;

	/* IRQ is initially disabled; it gets enabled in power_on */
	disable_irq(vop->irq);

=======
>>>>>>> v4.9.227
	ret = vop_create_crtc(vop);
	if (ret)
		return ret;

	pm_runtime_enable(&pdev->dev);
<<<<<<< HEAD
	return 0;
=======

	ret = vop_initial(vop);
	if (ret < 0) {
		dev_err(&pdev->dev, "cannot initial vop dev - err %d\n", ret);
		goto err_disable_pm_runtime;
	}

	ret = devm_request_irq(dev, vop->irq, vop_isr,
			       IRQF_SHARED, dev_name(dev), vop);
	if (ret)
		goto err_disable_pm_runtime;

	/* IRQ is initially disabled; it gets enabled in power_on */
	disable_irq(vop->irq);

	return 0;

err_disable_pm_runtime:
	pm_runtime_disable(&pdev->dev);
	vop_destroy_crtc(vop);
	return ret;
>>>>>>> v4.9.227
}

static void vop_unbind(struct device *dev, struct device *master, void *data)
{
	struct vop *vop = dev_get_drvdata(dev);

	pm_runtime_disable(dev);
	vop_destroy_crtc(vop);
}

<<<<<<< HEAD
static const struct component_ops vop_component_ops = {
	.bind = vop_bind,
	.unbind = vop_unbind,
};

static int vop_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	if (!dev->of_node) {
		dev_err(dev, "can't find vop devices\n");
		return -ENODEV;
	}

	return component_add(dev, &vop_component_ops);
}

static int vop_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &vop_component_ops);

	return 0;
}

struct platform_driver vop_platform_driver = {
	.probe = vop_probe,
	.remove = vop_remove,
	.driver = {
		.name = "rockchip-vop",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(vop_driver_dt_match),
	},
};

module_platform_driver(vop_platform_driver);

MODULE_AUTHOR("Mark Yao <mark.yao@rock-chips.com>");
MODULE_DESCRIPTION("ROCKCHIP VOP Driver");
MODULE_LICENSE("GPL v2");
=======
const struct component_ops vop_component_ops = {
	.bind = vop_bind,
	.unbind = vop_unbind,
};
EXPORT_SYMBOL_GPL(vop_component_ops);
>>>>>>> v4.9.227
