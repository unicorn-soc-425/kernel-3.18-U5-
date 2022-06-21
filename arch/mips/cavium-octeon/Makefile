#
# Makefile for the Cavium Octeon specific kernel interface routines
# under Linux.
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2005-2009 Cavium Networks
#

<<<<<<< HEAD
CFLAGS_octeon-platform.o = -I$(src)/../../../scripts/dtc/libfdt
CFLAGS_setup.o = -I$(src)/../../../scripts/dtc/libfdt

=======
>>>>>>> v4.9.227
obj-y := cpu.o setup.o octeon-platform.o octeon-irq.o csrc-octeon.o
obj-y += dma-octeon.o
obj-y += octeon-memcpy.o
obj-y += executive/
<<<<<<< HEAD
=======
obj-y += crypto/
>>>>>>> v4.9.227

obj-$(CONFIG_MTD)		      += flash_setup.o
obj-$(CONFIG_SMP)		      += smp.o
obj-$(CONFIG_OCTEON_ILM)	      += oct_ilm.o
