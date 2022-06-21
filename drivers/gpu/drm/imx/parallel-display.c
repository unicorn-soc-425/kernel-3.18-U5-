/*
 * i.MX drm driver - parallel display implementation
 *
 * Copyright (C) 2012 Sascha Hauer, Pengutronix
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/component.h>
#include <linux/module.h>
#include <drm/drmP.h>
<<<<<<< HEAD
=======
#include <drm/drm_atomic_helper.h>
>>>>>>> v4.9.227
#include <drm/drm_fb_helper.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_panel.h>
#include <linux/videodev2.h>
#include <video/of_display_timing.h>
#include <linux/of_graph.h>

#include "imx-drm.h"

<<<<<<< HEAD
#define con_to_imxpd(x) container_of(x, struct imx_parallel_display, connector)
#define enc_to_imxpd(x) container_of(x, struct imx_parallel_display, encoder)

=======
>>>>>>> v4.9.227
struct imx_parallel_display {
	struct drm_connector connector;
	struct drm_encoder encoder;
	struct device *dev;
	void *edid;
	int edid_len;
	u32 bus_format;
<<<<<<< HEAD
	int mode_valid;
	struct drm_display_mode mode;
	struct drm_panel *panel;
};

=======
	u32 bus_flags;
	struct drm_display_mode mode;
	struct drm_panel *panel;
	struct drm_bridge *bridge;
};

static inline struct imx_parallel_display *con_to_imxpd(struct drm_connector *c)
{
	return container_of(c, struct imx_parallel_display, connector);
}

static inline struct imx_parallel_display *enc_to_imxpd(struct drm_encoder *e)
{
	return container_of(e, struct imx_parallel_display, encoder);
}

>>>>>>> v4.9.227
static enum drm_connector_status imx_pd_connector_detect(
		struct drm_connector *connector, bool force)
{
	return connector_status_connected;
}

static int imx_pd_connector_get_modes(struct drm_connector *connector)
{
	struct imx_parallel_display *imxpd = con_to_imxpd(connector);
	struct device_node *np = imxpd->dev->of_node;
	int num_modes = 0;

	if (imxpd->panel && imxpd->panel->funcs &&
	    imxpd->panel->funcs->get_modes) {
<<<<<<< HEAD
		struct drm_display_info *di = &connector->display_info;

		num_modes = imxpd->panel->funcs->get_modes(imxpd->panel);
		if (!imxpd->bus_format && di->num_bus_formats)
			imxpd->bus_format = di->bus_formats[0];
=======
		num_modes = imxpd->panel->funcs->get_modes(imxpd->panel);
>>>>>>> v4.9.227
		if (num_modes > 0)
			return num_modes;
	}

	if (imxpd->edid) {
		drm_mode_connector_update_edid_property(connector, imxpd->edid);
		num_modes = drm_add_edid_modes(connector, imxpd->edid);
	}

<<<<<<< HEAD
	if (imxpd->mode_valid) {
		struct drm_display_mode *mode = drm_mode_create(connector->dev);

		if (!mode)
			return -EINVAL;
		drm_mode_copy(mode, &imxpd->mode);
		mode->type |= DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED,
		drm_mode_probed_add(connector, mode);
		num_modes++;
	}

	if (np) {
		struct drm_display_mode *mode = drm_mode_create(connector->dev);

		if (!mode)
			return -EINVAL;
		of_get_drm_display_mode(np, &imxpd->mode, OF_USE_NATIVE_MODE);
=======
	if (np) {
		struct drm_display_mode *mode = drm_mode_create(connector->dev);
		int ret;

		if (!mode)
			return -EINVAL;

		ret = of_get_drm_display_mode(np, &imxpd->mode,
					      &imxpd->bus_flags,
					      OF_USE_NATIVE_MODE);
		if (ret)
			return ret;

>>>>>>> v4.9.227
		drm_mode_copy(mode, &imxpd->mode);
		mode->type |= DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED,
		drm_mode_probed_add(connector, mode);
		num_modes++;
	}

	return num_modes;
}

static struct drm_encoder *imx_pd_connector_best_encoder(
		struct drm_connector *connector)
{
	struct imx_parallel_display *imxpd = con_to_imxpd(connector);

	return &imxpd->encoder;
}

<<<<<<< HEAD
static void imx_pd_encoder_dpms(struct drm_encoder *encoder, int mode)
{
	struct imx_parallel_display *imxpd = enc_to_imxpd(encoder);

	if (mode != DRM_MODE_DPMS_ON)
		drm_panel_disable(imxpd->panel);
	else
		drm_panel_enable(imxpd->panel);
}

static bool imx_pd_encoder_mode_fixup(struct drm_encoder *encoder,
			   const struct drm_display_mode *mode,
			   struct drm_display_mode *adjusted_mode)
{
	return true;
}

static void imx_pd_encoder_prepare(struct drm_encoder *encoder)
{
	struct imx_parallel_display *imxpd = enc_to_imxpd(encoder);

	imx_drm_set_bus_format(encoder, imxpd->bus_format);
}

static void imx_pd_encoder_commit(struct drm_encoder *encoder)
=======
static void imx_pd_encoder_enable(struct drm_encoder *encoder)
>>>>>>> v4.9.227
{
	struct imx_parallel_display *imxpd = enc_to_imxpd(encoder);

	drm_panel_prepare(imxpd->panel);
	drm_panel_enable(imxpd->panel);
}

<<<<<<< HEAD
static void imx_pd_encoder_mode_set(struct drm_encoder *encoder,
			 struct drm_display_mode *orig_mode,
			 struct drm_display_mode *mode)
{
}

=======
>>>>>>> v4.9.227
static void imx_pd_encoder_disable(struct drm_encoder *encoder)
{
	struct imx_parallel_display *imxpd = enc_to_imxpd(encoder);

	drm_panel_disable(imxpd->panel);
	drm_panel_unprepare(imxpd->panel);
}

<<<<<<< HEAD
static struct drm_connector_funcs imx_pd_connector_funcs = {
	.dpms = drm_helper_connector_dpms,
	.fill_modes = drm_helper_probe_single_connector_modes,
	.detect = imx_pd_connector_detect,
	.destroy = imx_drm_connector_destroy,
};

static struct drm_connector_helper_funcs imx_pd_connector_helper_funcs = {
=======
static int imx_pd_encoder_atomic_check(struct drm_encoder *encoder,
				       struct drm_crtc_state *crtc_state,
				       struct drm_connector_state *conn_state)
{
	struct imx_crtc_state *imx_crtc_state = to_imx_crtc_state(crtc_state);
	struct drm_display_info *di = &conn_state->connector->display_info;
	struct imx_parallel_display *imxpd = enc_to_imxpd(encoder);

	if (!imxpd->bus_format && di->num_bus_formats) {
		imx_crtc_state->bus_flags = di->bus_flags;
		imx_crtc_state->bus_format = di->bus_formats[0];
	} else {
		imx_crtc_state->bus_flags = imxpd->bus_flags;
		imx_crtc_state->bus_format = imxpd->bus_format;
	}
	imx_crtc_state->di_hsync_pin = 2;
	imx_crtc_state->di_vsync_pin = 3;

	return 0;
}

static const struct drm_connector_funcs imx_pd_connector_funcs = {
	.dpms = drm_atomic_helper_connector_dpms,
	.fill_modes = drm_helper_probe_single_connector_modes,
	.detect = imx_pd_connector_detect,
	.destroy = imx_drm_connector_destroy,
	.reset = drm_atomic_helper_connector_reset,
	.atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_connector_destroy_state,
};

static const struct drm_connector_helper_funcs imx_pd_connector_helper_funcs = {
>>>>>>> v4.9.227
	.get_modes = imx_pd_connector_get_modes,
	.best_encoder = imx_pd_connector_best_encoder,
};

<<<<<<< HEAD
static struct drm_encoder_funcs imx_pd_encoder_funcs = {
	.destroy = imx_drm_encoder_destroy,
};

static struct drm_encoder_helper_funcs imx_pd_encoder_helper_funcs = {
	.dpms = imx_pd_encoder_dpms,
	.mode_fixup = imx_pd_encoder_mode_fixup,
	.prepare = imx_pd_encoder_prepare,
	.commit = imx_pd_encoder_commit,
	.mode_set = imx_pd_encoder_mode_set,
	.disable = imx_pd_encoder_disable,
=======
static const struct drm_encoder_funcs imx_pd_encoder_funcs = {
	.destroy = imx_drm_encoder_destroy,
};

static const struct drm_encoder_helper_funcs imx_pd_encoder_helper_funcs = {
	.enable = imx_pd_encoder_enable,
	.disable = imx_pd_encoder_disable,
	.atomic_check = imx_pd_encoder_atomic_check,
>>>>>>> v4.9.227
};

static int imx_pd_register(struct drm_device *drm,
	struct imx_parallel_display *imxpd)
{
<<<<<<< HEAD
	int ret;

	ret = imx_drm_encoder_parse_of(drm, &imxpd->encoder,
				       imxpd->dev->of_node);
=======
	struct drm_encoder *encoder = &imxpd->encoder;
	int ret;

	ret = imx_drm_encoder_parse_of(drm, encoder, imxpd->dev->of_node);
>>>>>>> v4.9.227
	if (ret)
		return ret;

	/* set the connector's dpms to OFF so that
	 * drm_helper_connector_dpms() won't return
	 * immediately since the current state is ON
	 * at this point.
	 */
	imxpd->connector.dpms = DRM_MODE_DPMS_OFF;

<<<<<<< HEAD
	drm_encoder_helper_add(&imxpd->encoder, &imx_pd_encoder_helper_funcs);
	drm_encoder_init(drm, &imxpd->encoder, &imx_pd_encoder_funcs,
			 DRM_MODE_ENCODER_NONE);

	drm_connector_helper_add(&imxpd->connector,
			&imx_pd_connector_helper_funcs);
	drm_connector_init(drm, &imxpd->connector, &imx_pd_connector_funcs,
			   DRM_MODE_CONNECTOR_VGA);
=======
	drm_encoder_helper_add(encoder, &imx_pd_encoder_helper_funcs);
	drm_encoder_init(drm, encoder, &imx_pd_encoder_funcs,
			 DRM_MODE_ENCODER_NONE, NULL);

	if (!imxpd->bridge) {
		drm_connector_helper_add(&imxpd->connector,
				&imx_pd_connector_helper_funcs);
		drm_connector_init(drm, &imxpd->connector,
				   &imx_pd_connector_funcs,
				   DRM_MODE_CONNECTOR_VGA);
	}
>>>>>>> v4.9.227

	if (imxpd->panel)
		drm_panel_attach(imxpd->panel, &imxpd->connector);

<<<<<<< HEAD
	drm_mode_connector_attach_encoder(&imxpd->connector, &imxpd->encoder);

	imxpd->connector.encoder = &imxpd->encoder;
=======
	if (imxpd->bridge) {
		imxpd->bridge->encoder = encoder;
		encoder->bridge = imxpd->bridge;
		ret = drm_bridge_attach(drm, imxpd->bridge);
		if (ret < 0) {
			dev_err(imxpd->dev, "failed to attach bridge: %d\n",
				ret);
			return ret;
		}
	} else {
		drm_mode_connector_attach_encoder(&imxpd->connector, encoder);
	}
>>>>>>> v4.9.227

	return 0;
}

static int imx_pd_bind(struct device *dev, struct device *master, void *data)
{
	struct drm_device *drm = data;
	struct device_node *np = dev->of_node;
<<<<<<< HEAD
	struct device_node *port;
	const u8 *edidp;
	struct imx_parallel_display *imxpd;
	int ret;
=======
	struct device_node *ep;
	const u8 *edidp;
	struct imx_parallel_display *imxpd;
	int ret;
	u32 bus_format = 0;
>>>>>>> v4.9.227
	const char *fmt;

	imxpd = devm_kzalloc(dev, sizeof(*imxpd), GFP_KERNEL);
	if (!imxpd)
		return -ENOMEM;

	edidp = of_get_property(np, "edid", &imxpd->edid_len);
	if (edidp)
		imxpd->edid = kmemdup(edidp, imxpd->edid_len, GFP_KERNEL);

	ret = of_property_read_string(np, "interface-pix-fmt", &fmt);
	if (!ret) {
		if (!strcmp(fmt, "rgb24"))
<<<<<<< HEAD
			imxpd->bus_format = MEDIA_BUS_FMT_RGB888_1X24;
		else if (!strcmp(fmt, "rgb565"))
			imxpd->bus_format = MEDIA_BUS_FMT_RGB565_1X16;
		else if (!strcmp(fmt, "bgr666"))
			imxpd->bus_format = MEDIA_BUS_FMT_RGB666_1X18;
		else if (!strcmp(fmt, "lvds666"))
			imxpd->bus_format = MEDIA_BUS_FMT_RGB666_1X24_CPADHI;
	}

	/* port@1 is the output port */
	port = of_graph_get_port_by_id(np, 1);
	if (port) {
		struct device_node *endpoint, *remote;

		endpoint = of_get_child_by_name(port, "endpoint");
		if (endpoint) {
			remote = of_graph_get_remote_port_parent(endpoint);
			if (remote)
				imxpd->panel = of_drm_find_panel(remote);
			if (!imxpd->panel)
				return -EPROBE_DEFER;
		}
=======
			bus_format = MEDIA_BUS_FMT_RGB888_1X24;
		else if (!strcmp(fmt, "rgb565"))
			bus_format = MEDIA_BUS_FMT_RGB565_1X16;
		else if (!strcmp(fmt, "bgr666"))
			bus_format = MEDIA_BUS_FMT_RGB666_1X18;
		else if (!strcmp(fmt, "lvds666"))
			bus_format = MEDIA_BUS_FMT_RGB666_1X24_CPADHI;
	}
	imxpd->bus_format = bus_format;

	/* port@1 is the output port */
	ep = of_graph_get_endpoint_by_regs(np, 1, -1);
	if (ep) {
		struct device_node *remote;

		remote = of_graph_get_remote_port_parent(ep);
		if (!remote) {
			dev_warn(dev, "endpoint %s not connected\n",
				 ep->full_name);
			of_node_put(ep);
			return -ENODEV;
		}
		of_node_put(ep);

		imxpd->panel = of_drm_find_panel(remote);
		if (imxpd->panel) {
			dev_dbg(dev, "found panel %s\n", remote->full_name);
		} else {
			imxpd->bridge = of_drm_find_bridge(remote);
			if (imxpd->bridge)
				dev_dbg(dev, "found bridge %s\n",
					remote->full_name);
		}
		if (!imxpd->panel && !imxpd->bridge) {
			dev_dbg(dev, "waiting for panel or bridge %s\n",
				remote->full_name);
			of_node_put(remote);
			return -EPROBE_DEFER;
		}
		of_node_put(remote);
>>>>>>> v4.9.227
	}

	imxpd->dev = dev;

	ret = imx_pd_register(drm, imxpd);
	if (ret)
		return ret;

	dev_set_drvdata(dev, imxpd);

	return 0;
}

static void imx_pd_unbind(struct device *dev, struct device *master,
	void *data)
{
	struct imx_parallel_display *imxpd = dev_get_drvdata(dev);

<<<<<<< HEAD
	imxpd->encoder.funcs->destroy(&imxpd->encoder);
	imxpd->connector.funcs->destroy(&imxpd->connector);
=======
	if (imxpd->bridge)
		drm_bridge_detach(imxpd->bridge);
	if (imxpd->panel)
		drm_panel_detach(imxpd->panel);
>>>>>>> v4.9.227

	kfree(imxpd->edid);
}

static const struct component_ops imx_pd_ops = {
	.bind	= imx_pd_bind,
	.unbind	= imx_pd_unbind,
};

static int imx_pd_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &imx_pd_ops);
}

static int imx_pd_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &imx_pd_ops);

	return 0;
}

static const struct of_device_id imx_pd_dt_ids[] = {
	{ .compatible = "fsl,imx-parallel-display", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, imx_pd_dt_ids);

static struct platform_driver imx_pd_driver = {
	.probe		= imx_pd_probe,
	.remove		= imx_pd_remove,
	.driver		= {
		.of_match_table = imx_pd_dt_ids,
		.name	= "imx-parallel-display",
	},
};

module_platform_driver(imx_pd_driver);

MODULE_DESCRIPTION("i.MX parallel display driver");
MODULE_AUTHOR("Sascha Hauer, Pengutronix");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:imx-parallel-display");
