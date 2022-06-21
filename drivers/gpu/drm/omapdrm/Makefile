#
# Makefile for the drm device driver.  This driver provides support for the
# Direct Rendering Infrastructure (DRI)
#

<<<<<<< HEAD
ccflags-y := -Iinclude/drm -Werror
=======
obj-y += dss/
obj-y += displays/

ccflags-y := -Iinclude/drm
>>>>>>> v4.9.227
omapdrm-y := omap_drv.o \
	omap_irq.o \
	omap_debugfs.o \
	omap_crtc.o \
	omap_plane.o \
	omap_encoder.o \
	omap_connector.o \
	omap_fb.o \
<<<<<<< HEAD
	omap_fbdev.o \
=======
>>>>>>> v4.9.227
	omap_gem.o \
	omap_gem_dmabuf.o \
	omap_dmm_tiler.o \
	tcm-sita.o

<<<<<<< HEAD
=======
omapdrm-$(CONFIG_DRM_FBDEV_EMULATION) += omap_fbdev.o

>>>>>>> v4.9.227
obj-$(CONFIG_DRM_OMAP)	+= omapdrm.o
