#
#  Makefile for the Linux Controller Area Network core.
#

obj-$(CONFIG_CAN)	+= can.o
<<<<<<< HEAD
can-y			:= af_can.o proc.o
=======
can-y			:= af_can.o
can-$(CONFIG_PROC_FS)	+= proc.o
>>>>>>> v4.9.227

obj-$(CONFIG_CAN_RAW)	+= can-raw.o
can-raw-y		:= raw.o

obj-$(CONFIG_CAN_BCM)	+= can-bcm.o
can-bcm-y		:= bcm.o

obj-$(CONFIG_CAN_GW)	+= can-gw.o
can-gw-y		:= gw.o
