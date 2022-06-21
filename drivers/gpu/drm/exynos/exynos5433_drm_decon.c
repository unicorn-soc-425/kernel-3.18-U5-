/* drivers/gpu/drm/exynos5433_drm_decon.c
 *
 * Copyright (C) 2015 Samsung Electronics Co.Ltd
 * Authors:
 *	Joonyoung Shim <jy0922.shim@samsung.com>
 *	Hyungwon Hwang <human.hwang@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundationr
 */

#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/component.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/pm_runtime.h>

#include <video/exynos5433_decon.h>

#include "exynos_drm_drv.h"
#include "exynos_drm_crtc.h"
<<<<<<< HEAD
=======
#include "exynos_drm_fb.h"
>>>>>>> v4.9.227
#include "exynos_drm_plane.h"
#include "exynos_drm_iommu.h"

#define WINDOWS_NR	3
<<<<<<< HEAD
#define CURSOR_WIN	2
#define MIN_FB_WIDTH_FOR_16WORD_BURST	128

=======
#define MIN_FB_WIDTH_FOR_16WORD_BURST	128

#define IFTYPE_I80	(1 << 0)
#define I80_HW_TRG	(1 << 1)
#define IFTYPE_HDMI	(1 << 2)

>>>>>>> v4.9.227
static const char * const decon_clks_name[] = {
	"pclk",
	"aclk_decon",
	"aclk_smmu_decon0x",
	"aclk_xiu_decon0x",
	"pclk_smmu_decon0x",
	"sclk_decon_vclk",
	"sclk_decon_eclk",
};

<<<<<<< HEAD
enum decon_iftype {
	IFTYPE_RGB,
	IFTYPE_I80,
	IFTYPE_HDMI
};

=======
>>>>>>> v4.9.227
enum decon_flag_bits {
	BIT_CLKS_ENABLED,
	BIT_IRQS_ENABLED,
	BIT_WIN_UPDATED,
<<<<<<< HEAD
	BIT_SUSPENDED
=======
	BIT_SUSPENDED,
	BIT_REQUEST_UPDATE
>>>>>>> v4.9.227
};

struct decon_context {
	struct device			*dev;
	struct drm_device		*drm_dev;
	struct exynos_drm_crtc		*crtc;
	struct exynos_drm_plane		planes[WINDOWS_NR];
<<<<<<< HEAD
=======
	struct exynos_drm_plane_config	configs[WINDOWS_NR];
>>>>>>> v4.9.227
	void __iomem			*addr;
	struct clk			*clks[ARRAY_SIZE(decon_clks_name)];
	int				pipe;
	unsigned long			flags;
<<<<<<< HEAD
	enum decon_iftype		out_type;
=======
	unsigned long			out_type;
>>>>>>> v4.9.227
	int				first_win;
};

static const uint32_t decon_formats[] = {
	DRM_FORMAT_XRGB1555,
	DRM_FORMAT_RGB565,
	DRM_FORMAT_XRGB8888,
	DRM_FORMAT_ARGB8888,
};

<<<<<<< HEAD
=======
static const enum drm_plane_type decon_win_types[WINDOWS_NR] = {
	DRM_PLANE_TYPE_PRIMARY,
	DRM_PLANE_TYPE_OVERLAY,
	DRM_PLANE_TYPE_CURSOR,
};

>>>>>>> v4.9.227
static inline void decon_set_bits(struct decon_context *ctx, u32 reg, u32 mask,
				  u32 val)
{
	val = (val & mask) | (readl(ctx->addr + reg) & ~mask);
	writel(val, ctx->addr + reg);
}

static int decon_enable_vblank(struct exynos_drm_crtc *crtc)
{
	struct decon_context *ctx = crtc->ctx;
	u32 val;

	if (test_bit(BIT_SUSPENDED, &ctx->flags))
		return -EPERM;

<<<<<<< HEAD
	if (test_and_set_bit(BIT_IRQS_ENABLED, &ctx->flags)) {
		val = VIDINTCON0_INTEN;
		if (ctx->out_type == IFTYPE_I80)
=======
	if (!test_and_set_bit(BIT_IRQS_ENABLED, &ctx->flags)) {
		val = VIDINTCON0_INTEN;
		if (ctx->out_type & IFTYPE_I80)
>>>>>>> v4.9.227
			val |= VIDINTCON0_FRAMEDONE;
		else
			val |= VIDINTCON0_INTFRMEN;

		writel(val, ctx->addr + DECON_VIDINTCON0);
	}

	return 0;
}

static void decon_disable_vblank(struct exynos_drm_crtc *crtc)
{
	struct decon_context *ctx = crtc->ctx;

	if (test_bit(BIT_SUSPENDED, &ctx->flags))
		return;

	if (test_and_clear_bit(BIT_IRQS_ENABLED, &ctx->flags))
		writel(0, ctx->addr + DECON_VIDINTCON0);
}

static void decon_setup_trigger(struct decon_context *ctx)
{
<<<<<<< HEAD
	u32 val = (ctx->out_type != IFTYPE_HDMI)
		? TRIGCON_TRIGEN_PER_F | TRIGCON_TRIGEN_F |
		  TRIGCON_TE_AUTO_MASK | TRIGCON_SWTRIGEN
		: TRIGCON_TRIGEN_PER_F | TRIGCON_TRIGEN_F |
		  TRIGCON_HWTRIGMASK_I80_RGB | TRIGCON_HWTRIGEN_I80_RGB;
=======
	u32 val = !(ctx->out_type & I80_HW_TRG)
		? TRIGCON_TRIGEN_PER_F | TRIGCON_TRIGEN_F |
		  TRIGCON_TE_AUTO_MASK | TRIGCON_SWTRIGEN
		: TRIGCON_TRIGEN_PER_F | TRIGCON_TRIGEN_F |
		  TRIGCON_HWTRIGMASK | TRIGCON_HWTRIGEN;
>>>>>>> v4.9.227
	writel(val, ctx->addr + DECON_TRIGCON);
}

static void decon_commit(struct exynos_drm_crtc *crtc)
{
	struct decon_context *ctx = crtc->ctx;
	struct drm_display_mode *m = &crtc->base.mode;
	u32 val;

	if (test_bit(BIT_SUSPENDED, &ctx->flags))
		return;

<<<<<<< HEAD
	if (ctx->out_type == IFTYPE_HDMI) {
=======
	if (ctx->out_type & IFTYPE_HDMI) {
>>>>>>> v4.9.227
		m->crtc_hsync_start = m->crtc_hdisplay + 10;
		m->crtc_hsync_end = m->crtc_htotal - 92;
		m->crtc_vsync_start = m->crtc_vdisplay + 1;
		m->crtc_vsync_end = m->crtc_vsync_start + 1;
	}

	decon_set_bits(ctx, DECON_VIDCON0, VIDCON0_ENVID, 0);

	/* enable clock gate */
	val = CMU_CLKGAGE_MODE_SFR_F | CMU_CLKGAGE_MODE_MEM_F;
	writel(val, ctx->addr + DECON_CMU);

<<<<<<< HEAD
	/* lcd on and use command if */
	val = VIDOUT_LCD_ON;
	if (ctx->out_type == IFTYPE_I80)
		val |= VIDOUT_COMMAND_IF;
	else
		val |= VIDOUT_RGB_IF;
=======
	if (ctx->out_type & (IFTYPE_I80 | I80_HW_TRG))
		decon_setup_trigger(ctx);

	/* lcd on and use command if */
	val = VIDOUT_LCD_ON;
	if (ctx->out_type & IFTYPE_I80) {
		val |= VIDOUT_COMMAND_IF;
	} else {
		val |= VIDOUT_RGB_IF;
	}

>>>>>>> v4.9.227
	writel(val, ctx->addr + DECON_VIDOUTCON0);

	val = VIDTCON2_LINEVAL(m->vdisplay - 1) |
		VIDTCON2_HOZVAL(m->hdisplay - 1);
	writel(val, ctx->addr + DECON_VIDTCON2);

<<<<<<< HEAD
	if (ctx->out_type != IFTYPE_I80) {
=======
	if (!(ctx->out_type & IFTYPE_I80)) {
>>>>>>> v4.9.227
		val = VIDTCON00_VBPD_F(
				m->crtc_vtotal - m->crtc_vsync_end - 1) |
			VIDTCON00_VFPD_F(
				m->crtc_vsync_start - m->crtc_vdisplay - 1);
		writel(val, ctx->addr + DECON_VIDTCON00);

		val = VIDTCON01_VSPW_F(
				m->crtc_vsync_end - m->crtc_vsync_start - 1);
		writel(val, ctx->addr + DECON_VIDTCON01);

		val = VIDTCON10_HBPD_F(
				m->crtc_htotal - m->crtc_hsync_end - 1) |
			VIDTCON10_HFPD_F(
				m->crtc_hsync_start - m->crtc_hdisplay - 1);
		writel(val, ctx->addr + DECON_VIDTCON10);

		val = VIDTCON11_HSPW_F(
				m->crtc_hsync_end - m->crtc_hsync_start - 1);
		writel(val, ctx->addr + DECON_VIDTCON11);
	}

<<<<<<< HEAD
	decon_setup_trigger(ctx);

	/* enable output and display signal */
	decon_set_bits(ctx, DECON_VIDCON0, VIDCON0_ENVID | VIDCON0_ENVID_F, ~0);
=======
	/* enable output and display signal */
	decon_set_bits(ctx, DECON_VIDCON0, VIDCON0_ENVID | VIDCON0_ENVID_F, ~0);

	decon_set_bits(ctx, DECON_UPDATE, STANDALONE_UPDATE_F, ~0);
>>>>>>> v4.9.227
}

static void decon_win_set_pixfmt(struct decon_context *ctx, unsigned int win,
				 struct drm_framebuffer *fb)
{
	unsigned long val;

	val = readl(ctx->addr + DECON_WINCONx(win));
<<<<<<< HEAD
	val &= ~WINCONx_BPPMODE_MASK;
=======
	val &= WINCONx_ENWIN_F;
>>>>>>> v4.9.227

	switch (fb->pixel_format) {
	case DRM_FORMAT_XRGB1555:
		val |= WINCONx_BPPMODE_16BPP_I1555;
		val |= WINCONx_HAWSWP_F;
		val |= WINCONx_BURSTLEN_16WORD;
		break;
	case DRM_FORMAT_RGB565:
		val |= WINCONx_BPPMODE_16BPP_565;
		val |= WINCONx_HAWSWP_F;
		val |= WINCONx_BURSTLEN_16WORD;
		break;
	case DRM_FORMAT_XRGB8888:
		val |= WINCONx_BPPMODE_24BPP_888;
		val |= WINCONx_WSWP_F;
		val |= WINCONx_BURSTLEN_16WORD;
		break;
	case DRM_FORMAT_ARGB8888:
		val |= WINCONx_BPPMODE_32BPP_A8888;
		val |= WINCONx_WSWP_F | WINCONx_BLD_PIX_F | WINCONx_ALPHA_SEL_F;
		val |= WINCONx_BURSTLEN_16WORD;
		break;
	default:
		DRM_ERROR("Proper pixel format is not set\n");
		return;
	}

	DRM_DEBUG_KMS("bpp = %u\n", fb->bits_per_pixel);

	/*
	 * In case of exynos, setting dma-burst to 16Word causes permanent
	 * tearing for very small buffers, e.g. cursor buffer. Burst Mode
	 * switching which is based on plane size is not recommended as
	 * plane size varies a lot towards the end of the screen and rapid
	 * movement causes unstable DMA which results into iommu crash/tear.
	 */

	if (fb->width < MIN_FB_WIDTH_FOR_16WORD_BURST) {
		val &= ~WINCONx_BURSTLEN_MASK;
		val |= WINCONx_BURSTLEN_8WORD;
	}

	writel(val, ctx->addr + DECON_WINCONx(win));
}

static void decon_shadow_protect_win(struct decon_context *ctx, int win,
					bool protect)
{
	decon_set_bits(ctx, DECON_SHADOWCON, SHADOWCON_Wx_PROTECT(win),
		       protect ? ~0 : 0);
}

<<<<<<< HEAD
static void decon_atomic_begin(struct exynos_drm_crtc *crtc,
					struct exynos_drm_plane *plane)
{
	struct decon_context *ctx = crtc->ctx;
=======
static void decon_atomic_begin(struct exynos_drm_crtc *crtc)
{
	struct decon_context *ctx = crtc->ctx;
	int i;
>>>>>>> v4.9.227

	if (test_bit(BIT_SUSPENDED, &ctx->flags))
		return;

<<<<<<< HEAD
	decon_shadow_protect_win(ctx, plane->zpos, true);
=======
	for (i = ctx->first_win; i < WINDOWS_NR; i++)
		decon_shadow_protect_win(ctx, i, true);
>>>>>>> v4.9.227
}

#define BIT_VAL(x, e, s) (((x) & ((1 << ((e) - (s) + 1)) - 1)) << (s))
#define COORDINATE_X(x) BIT_VAL((x), 23, 12)
#define COORDINATE_Y(x) BIT_VAL((x), 11, 0)

static void decon_update_plane(struct exynos_drm_crtc *crtc,
			       struct exynos_drm_plane *plane)
{
<<<<<<< HEAD
	struct decon_context *ctx = crtc->ctx;
	struct drm_plane_state *state = plane->base.state;
	unsigned int win = plane->zpos;
	unsigned int bpp = state->fb->bits_per_pixel >> 3;
	unsigned int pitch = state->fb->pitches[0];
=======
	struct exynos_drm_plane_state *state =
				to_exynos_plane_state(plane->base.state);
	struct decon_context *ctx = crtc->ctx;
	struct drm_framebuffer *fb = state->base.fb;
	unsigned int win = plane->index;
	unsigned int bpp = fb->bits_per_pixel >> 3;
	unsigned int pitch = fb->pitches[0];
	dma_addr_t dma_addr = exynos_drm_fb_dma_addr(fb, 0);
>>>>>>> v4.9.227
	u32 val;

	if (test_bit(BIT_SUSPENDED, &ctx->flags))
		return;

<<<<<<< HEAD
	val = COORDINATE_X(plane->crtc_x) | COORDINATE_Y(plane->crtc_y);
	writel(val, ctx->addr + DECON_VIDOSDxA(win));

	val = COORDINATE_X(plane->crtc_x + plane->crtc_w - 1) |
		COORDINATE_Y(plane->crtc_y + plane->crtc_h - 1);
	writel(val, ctx->addr + DECON_VIDOSDxB(win));

	val = VIDOSD_Wx_ALPHA_R_F(0x0) | VIDOSD_Wx_ALPHA_G_F(0x0) |
		VIDOSD_Wx_ALPHA_B_F(0x0);
=======
	val = COORDINATE_X(state->crtc.x) | COORDINATE_Y(state->crtc.y);
	writel(val, ctx->addr + DECON_VIDOSDxA(win));

	val = COORDINATE_X(state->crtc.x + state->crtc.w - 1) |
		COORDINATE_Y(state->crtc.y + state->crtc.h - 1);
	writel(val, ctx->addr + DECON_VIDOSDxB(win));

	val = VIDOSD_Wx_ALPHA_R_F(0xff) | VIDOSD_Wx_ALPHA_G_F(0xff) |
		VIDOSD_Wx_ALPHA_B_F(0xff);
>>>>>>> v4.9.227
	writel(val, ctx->addr + DECON_VIDOSDxC(win));

	val = VIDOSD_Wx_ALPHA_R_F(0x0) | VIDOSD_Wx_ALPHA_G_F(0x0) |
		VIDOSD_Wx_ALPHA_B_F(0x0);
	writel(val, ctx->addr + DECON_VIDOSDxD(win));

<<<<<<< HEAD
	writel(plane->dma_addr[0], ctx->addr + DECON_VIDW0xADD0B0(win));

	val = plane->dma_addr[0] + pitch * plane->crtc_h;
	writel(val, ctx->addr + DECON_VIDW0xADD1B0(win));

	if (ctx->out_type != IFTYPE_HDMI)
		val = BIT_VAL(pitch - plane->crtc_w * bpp, 27, 14)
			| BIT_VAL(plane->crtc_w * bpp, 13, 0);
	else
		val = BIT_VAL(pitch - plane->crtc_w * bpp, 29, 15)
			| BIT_VAL(plane->crtc_w * bpp, 14, 0);
	writel(val, ctx->addr + DECON_VIDW0xADD2(win));

	decon_win_set_pixfmt(ctx, win, state->fb);

	/* window enable */
	decon_set_bits(ctx, DECON_WINCONx(win), WINCONx_ENWIN_F, ~0);

	/* standalone update */
	decon_set_bits(ctx, DECON_UPDATE, STANDALONE_UPDATE_F, ~0);
=======
	writel(dma_addr, ctx->addr + DECON_VIDW0xADD0B0(win));

	val = dma_addr + pitch * state->src.h;
	writel(val, ctx->addr + DECON_VIDW0xADD1B0(win));

	if (!(ctx->out_type & IFTYPE_HDMI))
		val = BIT_VAL(pitch - state->crtc.w * bpp, 27, 14)
			| BIT_VAL(state->crtc.w * bpp, 13, 0);
	else
		val = BIT_VAL(pitch - state->crtc.w * bpp, 29, 15)
			| BIT_VAL(state->crtc.w * bpp, 14, 0);
	writel(val, ctx->addr + DECON_VIDW0xADD2(win));

	decon_win_set_pixfmt(ctx, win, fb);

	/* window enable */
	decon_set_bits(ctx, DECON_WINCONx(win), WINCONx_ENWIN_F, ~0);
	set_bit(BIT_REQUEST_UPDATE, &ctx->flags);
>>>>>>> v4.9.227
}

static void decon_disable_plane(struct exynos_drm_crtc *crtc,
				struct exynos_drm_plane *plane)
{
	struct decon_context *ctx = crtc->ctx;
<<<<<<< HEAD
	unsigned int win = plane->zpos;
=======
	unsigned int win = plane->index;
>>>>>>> v4.9.227

	if (test_bit(BIT_SUSPENDED, &ctx->flags))
		return;

<<<<<<< HEAD
	decon_shadow_protect_win(ctx, win, true);

	/* window disable */
	decon_set_bits(ctx, DECON_WINCONx(win), WINCONx_ENWIN_F, 0);

	decon_shadow_protect_win(ctx, win, false);

	/* standalone update */
	decon_set_bits(ctx, DECON_UPDATE, STANDALONE_UPDATE_F, ~0);
}

static void decon_atomic_flush(struct exynos_drm_crtc *crtc,
				struct exynos_drm_plane *plane)
{
	struct decon_context *ctx = crtc->ctx;
=======
	decon_set_bits(ctx, DECON_WINCONx(win), WINCONx_ENWIN_F, 0);
	set_bit(BIT_REQUEST_UPDATE, &ctx->flags);
}

static void decon_atomic_flush(struct exynos_drm_crtc *crtc)
{
	struct decon_context *ctx = crtc->ctx;
	int i;
>>>>>>> v4.9.227

	if (test_bit(BIT_SUSPENDED, &ctx->flags))
		return;

<<<<<<< HEAD
	decon_shadow_protect_win(ctx, plane->zpos, false);

	if (ctx->out_type == IFTYPE_I80)
=======
	for (i = ctx->first_win; i < WINDOWS_NR; i++)
		decon_shadow_protect_win(ctx, i, false);

	if (test_and_clear_bit(BIT_REQUEST_UPDATE, &ctx->flags))
		decon_set_bits(ctx, DECON_UPDATE, STANDALONE_UPDATE_F, ~0);

	if (ctx->out_type & IFTYPE_I80)
>>>>>>> v4.9.227
		set_bit(BIT_WIN_UPDATED, &ctx->flags);
}

static void decon_swreset(struct decon_context *ctx)
{
	unsigned int tries;

	writel(0, ctx->addr + DECON_VIDCON0);
	for (tries = 2000; tries; --tries) {
		if (~readl(ctx->addr + DECON_VIDCON0) & VIDCON0_STOP_STATUS)
			break;
		udelay(10);
	}

	WARN(tries == 0, "failed to disable DECON\n");

	writel(VIDCON0_SWRESET, ctx->addr + DECON_VIDCON0);
	for (tries = 2000; tries; --tries) {
		if (~readl(ctx->addr + DECON_VIDCON0) & VIDCON0_SWRESET)
			break;
		udelay(10);
	}

	WARN(tries == 0, "failed to software reset DECON\n");

<<<<<<< HEAD
	if (ctx->out_type != IFTYPE_HDMI)
=======
	if (!(ctx->out_type & IFTYPE_HDMI))
>>>>>>> v4.9.227
		return;

	writel(VIDCON0_CLKVALUP | VIDCON0_VLCKFREE, ctx->addr + DECON_VIDCON0);
	decon_set_bits(ctx, DECON_CMU,
		       CMU_CLKGAGE_MODE_SFR_F | CMU_CLKGAGE_MODE_MEM_F, ~0);
	writel(VIDCON1_VCLK_RUN_VDEN_DISABLE, ctx->addr + DECON_VIDCON1);
	writel(CRCCTRL_CRCEN | CRCCTRL_CRCSTART_F | CRCCTRL_CRCCLKEN,
	       ctx->addr + DECON_CRCCTRL);
<<<<<<< HEAD
	decon_setup_trigger(ctx);
=======
>>>>>>> v4.9.227
}

static void decon_enable(struct exynos_drm_crtc *crtc)
{
	struct decon_context *ctx = crtc->ctx;
<<<<<<< HEAD
	int ret;
	int i;
=======
>>>>>>> v4.9.227

	if (!test_and_clear_bit(BIT_SUSPENDED, &ctx->flags))
		return;

	pm_runtime_get_sync(ctx->dev);

<<<<<<< HEAD
	for (i = 0; i < ARRAY_SIZE(decon_clks_name); i++) {
		ret = clk_prepare_enable(ctx->clks[i]);
		if (ret < 0)
			goto err;
	}

	set_bit(BIT_CLKS_ENABLED, &ctx->flags);

=======
	exynos_drm_pipe_clk_enable(crtc, true);

	set_bit(BIT_CLKS_ENABLED, &ctx->flags);

	decon_swreset(ctx);

>>>>>>> v4.9.227
	/* if vblank was enabled status, enable it again. */
	if (test_and_clear_bit(BIT_IRQS_ENABLED, &ctx->flags))
		decon_enable_vblank(ctx->crtc);

	decon_commit(ctx->crtc);
<<<<<<< HEAD

	return;
err:
	while (--i >= 0)
		clk_disable_unprepare(ctx->clks[i]);

	set_bit(BIT_SUSPENDED, &ctx->flags);
=======
>>>>>>> v4.9.227
}

static void decon_disable(struct exynos_drm_crtc *crtc)
{
	struct decon_context *ctx = crtc->ctx;
	int i;

	if (test_bit(BIT_SUSPENDED, &ctx->flags))
		return;

	/*
	 * We need to make sure that all windows are disabled before we
	 * suspend that connector. Otherwise we might try to scan from
	 * a destroyed buffer later.
	 */
	for (i = ctx->first_win; i < WINDOWS_NR; i++)
		decon_disable_plane(crtc, &ctx->planes[i]);

	decon_swreset(ctx);

<<<<<<< HEAD
	for (i = 0; i < ARRAY_SIZE(decon_clks_name); i++)
		clk_disable_unprepare(ctx->clks[i]);

	clear_bit(BIT_CLKS_ENABLED, &ctx->flags);

=======
	clear_bit(BIT_CLKS_ENABLED, &ctx->flags);

	exynos_drm_pipe_clk_enable(crtc, false);

>>>>>>> v4.9.227
	pm_runtime_put_sync(ctx->dev);

	set_bit(BIT_SUSPENDED, &ctx->flags);
}

<<<<<<< HEAD
void decon_te_irq_handler(struct exynos_drm_crtc *crtc)
{
	struct decon_context *ctx = crtc->ctx;

	if (!test_bit(BIT_CLKS_ENABLED, &ctx->flags))
=======
static void decon_te_irq_handler(struct exynos_drm_crtc *crtc)
{
	struct decon_context *ctx = crtc->ctx;

	if (!test_bit(BIT_CLKS_ENABLED, &ctx->flags) ||
	    (ctx->out_type & I80_HW_TRG))
>>>>>>> v4.9.227
		return;

	if (test_and_clear_bit(BIT_WIN_UPDATED, &ctx->flags))
		decon_set_bits(ctx, DECON_TRIGCON, TRIGCON_SWTRIGCMD, ~0);
<<<<<<< HEAD

	drm_crtc_handle_vblank(&ctx->crtc->base);
=======
>>>>>>> v4.9.227
}

static void decon_clear_channels(struct exynos_drm_crtc *crtc)
{
	struct decon_context *ctx = crtc->ctx;
	int win, i, ret;

	DRM_DEBUG_KMS("%s\n", __FILE__);

	for (i = 0; i < ARRAY_SIZE(decon_clks_name); i++) {
		ret = clk_prepare_enable(ctx->clks[i]);
		if (ret < 0)
			goto err;
	}

	for (win = 0; win < WINDOWS_NR; win++) {
		decon_shadow_protect_win(ctx, win, true);
		decon_set_bits(ctx, DECON_WINCONx(win), WINCONx_ENWIN_F, 0);
		decon_shadow_protect_win(ctx, win, false);
<<<<<<< HEAD
		decon_set_bits(ctx, DECON_UPDATE, STANDALONE_UPDATE_F, ~0);
	}
=======
	}

	decon_set_bits(ctx, DECON_UPDATE, STANDALONE_UPDATE_F, ~0);

>>>>>>> v4.9.227
	/* TODO: wait for possible vsync */
	msleep(50);

err:
	while (--i >= 0)
		clk_disable_unprepare(ctx->clks[i]);
}

static struct exynos_drm_crtc_ops decon_crtc_ops = {
	.enable			= decon_enable,
	.disable		= decon_disable,
<<<<<<< HEAD
	.commit			= decon_commit,
=======
>>>>>>> v4.9.227
	.enable_vblank		= decon_enable_vblank,
	.disable_vblank		= decon_disable_vblank,
	.atomic_begin		= decon_atomic_begin,
	.update_plane		= decon_update_plane,
	.disable_plane		= decon_disable_plane,
	.atomic_flush		= decon_atomic_flush,
	.te_handler		= decon_te_irq_handler,
};

static int decon_bind(struct device *dev, struct device *master, void *data)
{
	struct decon_context *ctx = dev_get_drvdata(dev);
	struct drm_device *drm_dev = data;
	struct exynos_drm_private *priv = drm_dev->dev_private;
	struct exynos_drm_plane *exynos_plane;
	enum exynos_drm_output_type out_type;
<<<<<<< HEAD
	enum drm_plane_type type;
=======
>>>>>>> v4.9.227
	unsigned int win;
	int ret;

	ctx->drm_dev = drm_dev;
	ctx->pipe = priv->pipe++;

	for (win = ctx->first_win; win < WINDOWS_NR; win++) {
		int tmp = (win == ctx->first_win) ? 0 : win;

<<<<<<< HEAD
		type = exynos_plane_get_type(tmp, CURSOR_WIN);
		ret = exynos_plane_init(drm_dev, &ctx->planes[win],
				1 << ctx->pipe, type, decon_formats,
				ARRAY_SIZE(decon_formats), win);
=======
		ctx->configs[win].pixel_formats = decon_formats;
		ctx->configs[win].num_pixel_formats = ARRAY_SIZE(decon_formats);
		ctx->configs[win].zpos = win;
		ctx->configs[win].type = decon_win_types[tmp];

		ret = exynos_plane_init(drm_dev, &ctx->planes[win], win,
					1 << ctx->pipe, &ctx->configs[win]);
>>>>>>> v4.9.227
		if (ret)
			return ret;
	}

	exynos_plane = &ctx->planes[ctx->first_win];
<<<<<<< HEAD
	out_type = (ctx->out_type == IFTYPE_HDMI) ? EXYNOS_DISPLAY_TYPE_HDMI
=======
	out_type = (ctx->out_type & IFTYPE_HDMI) ? EXYNOS_DISPLAY_TYPE_HDMI
>>>>>>> v4.9.227
						  : EXYNOS_DISPLAY_TYPE_LCD;
	ctx->crtc = exynos_drm_crtc_create(drm_dev, &exynos_plane->base,
					ctx->pipe, out_type,
					&decon_crtc_ops, ctx);
	if (IS_ERR(ctx->crtc)) {
		ret = PTR_ERR(ctx->crtc);
		goto err;
	}

	decon_clear_channels(ctx->crtc);

	ret = drm_iommu_attach_device(drm_dev, dev);
	if (ret)
		goto err;

	return ret;
err:
	priv->pipe--;
	return ret;
}

static void decon_unbind(struct device *dev, struct device *master, void *data)
{
	struct decon_context *ctx = dev_get_drvdata(dev);

	decon_disable(ctx->crtc);

	/* detach this sub driver from iommu mapping if supported. */
	drm_iommu_detach_device(ctx->drm_dev, ctx->dev);
}

static const struct component_ops decon_component_ops = {
	.bind	= decon_bind,
	.unbind = decon_unbind,
};

static irqreturn_t decon_irq_handler(int irq, void *dev_id)
{
	struct decon_context *ctx = dev_id;
	u32 val;
<<<<<<< HEAD
	int win;
=======
>>>>>>> v4.9.227

	if (!test_bit(BIT_CLKS_ENABLED, &ctx->flags))
		goto out;

	val = readl(ctx->addr + DECON_VIDINTCON1);
	val &= VIDINTCON1_INTFRMDONEPEND | VIDINTCON1_INTFRMPEND;

	if (val) {
<<<<<<< HEAD
		for (win = ctx->first_win; win < WINDOWS_NR ; win++) {
			struct exynos_drm_plane *plane = &ctx->planes[win];

			if (!plane->pending_fb)
				continue;

			exynos_drm_crtc_finish_update(ctx->crtc, plane);
		}

		/* clear */
		writel(val, ctx->addr + DECON_VIDINTCON1);
=======
		writel(val, ctx->addr + DECON_VIDINTCON1);
		drm_crtc_handle_vblank(&ctx->crtc->base);
>>>>>>> v4.9.227
	}

out:
	return IRQ_HANDLED;
}

<<<<<<< HEAD
static const struct of_device_id exynos5433_decon_driver_dt_match[] = {
	{
		.compatible = "samsung,exynos5433-decon",
		.data = (void *)IFTYPE_RGB
	},
	{
		.compatible = "samsung,exynos5433-decon-tv",
		.data = (void *)IFTYPE_HDMI
=======
#ifdef CONFIG_PM
static int exynos5433_decon_suspend(struct device *dev)
{
	struct decon_context *ctx = dev_get_drvdata(dev);
	int i = ARRAY_SIZE(decon_clks_name);

	while (--i >= 0)
		clk_disable_unprepare(ctx->clks[i]);

	return 0;
}

static int exynos5433_decon_resume(struct device *dev)
{
	struct decon_context *ctx = dev_get_drvdata(dev);
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(decon_clks_name); i++) {
		ret = clk_prepare_enable(ctx->clks[i]);
		if (ret < 0)
			goto err;
	}

	return 0;

err:
	while (--i >= 0)
		clk_disable_unprepare(ctx->clks[i]);

	return ret;
}
#endif

static const struct dev_pm_ops exynos5433_decon_pm_ops = {
	SET_RUNTIME_PM_OPS(exynos5433_decon_suspend, exynos5433_decon_resume,
			   NULL)
};

static const struct of_device_id exynos5433_decon_driver_dt_match[] = {
	{
		.compatible = "samsung,exynos5433-decon",
		.data = (void *)I80_HW_TRG
	},
	{
		.compatible = "samsung,exynos5433-decon-tv",
		.data = (void *)(I80_HW_TRG | IFTYPE_HDMI)
>>>>>>> v4.9.227
	},
	{},
};
MODULE_DEVICE_TABLE(of, exynos5433_decon_driver_dt_match);

static int exynos5433_decon_probe(struct platform_device *pdev)
{
<<<<<<< HEAD
	const struct of_device_id *of_id;
=======
>>>>>>> v4.9.227
	struct device *dev = &pdev->dev;
	struct decon_context *ctx;
	struct resource *res;
	int ret;
	int i;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	__set_bit(BIT_SUSPENDED, &ctx->flags);
	ctx->dev = dev;
<<<<<<< HEAD

	of_id = of_match_device(exynos5433_decon_driver_dt_match, &pdev->dev);
	ctx->out_type = (enum decon_iftype)of_id->data;

	if (ctx->out_type == IFTYPE_HDMI)
		ctx->first_win = 1;
	else if (of_get_child_by_name(dev->of_node, "i80-if-timings"))
		ctx->out_type = IFTYPE_I80;
=======
	ctx->out_type = (unsigned long)of_device_get_match_data(dev);

	if (ctx->out_type & IFTYPE_HDMI) {
		ctx->first_win = 1;
	} else if (of_get_child_by_name(dev->of_node, "i80-if-timings")) {
		ctx->out_type |= IFTYPE_I80;
	}
>>>>>>> v4.9.227

	for (i = 0; i < ARRAY_SIZE(decon_clks_name); i++) {
		struct clk *clk;

		clk = devm_clk_get(ctx->dev, decon_clks_name[i]);
		if (IS_ERR(clk))
			return PTR_ERR(clk);

		ctx->clks[i] = clk;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "cannot find IO resource\n");
		return -ENXIO;
	}

	ctx->addr = devm_ioremap_resource(dev, res);
	if (IS_ERR(ctx->addr)) {
		dev_err(dev, "ioremap failed\n");
		return PTR_ERR(ctx->addr);
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_IRQ,
<<<<<<< HEAD
			(ctx->out_type == IFTYPE_I80) ? "lcd_sys" : "vsync");
=======
			(ctx->out_type & IFTYPE_I80) ? "lcd_sys" : "vsync");
>>>>>>> v4.9.227
	if (!res) {
		dev_err(dev, "cannot find IRQ resource\n");
		return -ENXIO;
	}

	ret = devm_request_irq(dev, res->start, decon_irq_handler, 0,
			       "drm_decon", ctx);
	if (ret < 0) {
		dev_err(dev, "lcd_sys irq request failed\n");
		return ret;
	}

	platform_set_drvdata(pdev, ctx);

	pm_runtime_enable(dev);

	ret = component_add(dev, &decon_component_ops);
	if (ret)
		goto err_disable_pm_runtime;

	return 0;

err_disable_pm_runtime:
	pm_runtime_disable(dev);

	return ret;
}

static int exynos5433_decon_remove(struct platform_device *pdev)
{
	pm_runtime_disable(&pdev->dev);

	component_del(&pdev->dev, &decon_component_ops);

	return 0;
}

struct platform_driver exynos5433_decon_driver = {
	.probe		= exynos5433_decon_probe,
	.remove		= exynos5433_decon_remove,
	.driver		= {
		.name	= "exynos5433-decon",
<<<<<<< HEAD
=======
		.pm	= &exynos5433_decon_pm_ops,
>>>>>>> v4.9.227
		.of_match_table = exynos5433_decon_driver_dt_match,
	},
};
