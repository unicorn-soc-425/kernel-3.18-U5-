#
# Makefile for the linux kernel, U8500 machine.
#

<<<<<<< HEAD
obj-y				:= cpu.o id.o timer.o pm.o
obj-$(CONFIG_CACHE_L2X0)	+= cache-l2x0.o
obj-$(CONFIG_UX500_SOC_DB8500)	+= cpu-db8500.o
obj-$(CONFIG_MACH_MOP500)	+= board-mop500-regulators.o \
				board-mop500-audio.o
obj-$(CONFIG_SMP)		+= platsmp.o headsmp.o
obj-$(CONFIG_HOTPLUG_CPU)	+= hotplug.o
=======
obj-y				:= pm.o
obj-$(CONFIG_UX500_SOC_DB8500)	+= cpu-db8500.o
obj-$(CONFIG_MACH_MOP500)	+= board-mop500-audio.o
obj-$(CONFIG_SMP)		+= platsmp.o
obj-$(CONFIG_HOTPLUG_CPU)	+= hotplug.o
obj-$(CONFIG_PM_GENERIC_DOMAINS) += pm_domains.o
>>>>>>> v4.9.227

CFLAGS_hotplug.o		+= -march=armv7-a
