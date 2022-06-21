#
# Makefile for s390-specific library files..
#

lib-y += delay.o string.o uaccess.o find.o
<<<<<<< HEAD
obj-$(CONFIG_32BIT) += div64.o qrnnd.o ucmpdi2.o mem32.o
obj-$(CONFIG_64BIT) += mem64.o
=======
obj-y += mem.o xor.o
>>>>>>> v4.9.227
lib-$(CONFIG_SMP) += spinlock.o
lib-$(CONFIG_KPROBES) += probes.o
lib-$(CONFIG_UPROBES) += probes.o
