<<<<<<< HEAD
obj-y			+= drm/ vga/ msm/
obj-$(CONFIG_TEGRA_HOST1X)	+= host1x/
=======
# drm/tegra depends on host1x, so if both drivers are built-in care must be
# taken to initialize them in the correct order. Link order is the only way
# to ensure this currently.
obj-$(CONFIG_TEGRA_HOST1X)	+= host1x/
obj-y			+= drm/ vga/
>>>>>>> v4.9.227
obj-$(CONFIG_IMX_IPUV3_CORE)	+= ipu-v3/
