<<<<<<< HEAD
ccflags-$(CONFIG_ARCH_MULTIPLATFORM) := -I$(srctree)/$(src)/include \
	-I$(srctree)/arch/arm/plat-orion/include
=======
ccflags-$(CONFIG_ARCH_MULTIPLATFORM) := -I$(srctree)/arch/arm/plat-orion/include
>>>>>>> v4.9.227

AFLAGS_coherency_ll.o		:= -Wa,-march=armv7-a
CFLAGS_pmsu.o			:= -march=armv7-a

obj-$(CONFIG_MACH_MVEBU_ANY)	 += system-controller.o mvebu-soc-id.o

ifeq ($(CONFIG_MACH_MVEBU_V7),y)
obj-y				 += cpu-reset.o board-v7.o coherency.o coherency_ll.o pmsu.o pmsu_ll.o
<<<<<<< HEAD
=======

obj-$(CONFIG_PM)		 += pm.o pm-board.o
>>>>>>> v4.9.227
obj-$(CONFIG_SMP)		 += platsmp.o headsmp.o platsmp-a9.o headsmp-a9.o
endif

obj-$(CONFIG_MACH_DOVE)		 += dove.o
<<<<<<< HEAD
obj-$(CONFIG_MACH_KIRKWOOD)	 += kirkwood.o kirkwood-pm.o
obj-$(CONFIG_MACH_NETXBIG)	 += netxbig.o
=======

ifeq ($(CONFIG_MACH_KIRKWOOD),y)
obj-y				 += kirkwood.o
obj-$(CONFIG_PM)		 += kirkwood-pm.o
endif
>>>>>>> v4.9.227
