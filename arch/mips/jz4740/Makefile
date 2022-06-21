#
# Makefile for the Ingenic JZ4740.
#

# Object file lists.

<<<<<<< HEAD
obj-y += prom.o irq.o time.o reset.o setup.o \
	gpio.o clock.o platform.o timer.o serial.o

obj-$(CONFIG_DEBUG_FS) += clock-debugfs.o
=======
obj-y += prom.o time.o reset.o setup.o \
	platform.o timer.o

obj-$(CONFIG_MACH_JZ4740) += gpio.o

CFLAGS_setup.o = -I$(src)/../../../scripts/dtc/libfdt
>>>>>>> v4.9.227

# board specific support

obj-$(CONFIG_JZ4740_QI_LB60)	+= board-qi_lb60.o

# PM support

obj-$(CONFIG_PM) += pm.o
