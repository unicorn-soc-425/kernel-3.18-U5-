#
# Makefile for the linux pstorefs routines.
#

<<<<<<< HEAD
obj-y += pstore.o

pstore-objs += inode.o platform.o
obj-$(CONFIG_PSTORE_FTRACE)	+= ftrace.o

obj-$(CONFIG_PSTORE_PMSG)	+= pmsg.o
obj-$(CONFIG_PSTORE_PMSG_SSPLOG)	+= ss_platform_log.o
=======
obj-$(CONFIG_PSTORE) += pstore.o

pstore-objs += inode.o platform.o
pstore-$(CONFIG_PSTORE_FTRACE)	+= ftrace.o

pstore-$(CONFIG_PSTORE_PMSG)	+= pmsg.o
>>>>>>> v4.9.227

ramoops-objs += ram.o ram_core.o
obj-$(CONFIG_PSTORE_RAM)	+= ramoops.o
