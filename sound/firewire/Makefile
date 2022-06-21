<<<<<<< HEAD
snd-firewire-lib-objs := lib.o iso-resources.o packets-buffer.o \
			 fcp.o cmp.o amdtp.o
snd-dice-objs := dice.o
snd-firewire-speakers-objs := speakers.o
snd-isight-objs := isight.o
snd-scs1x-objs := scs1x.o

obj-$(CONFIG_SND_FIREWIRE_LIB) += snd-firewire-lib.o
obj-$(CONFIG_SND_DICE) += snd-dice.o
obj-$(CONFIG_SND_FIREWIRE_SPEAKERS) += snd-firewire-speakers.o
obj-$(CONFIG_SND_ISIGHT) += snd-isight.o
obj-$(CONFIG_SND_SCS1X) += snd-scs1x.o
obj-$(CONFIG_SND_FIREWORKS) += fireworks/
obj-$(CONFIG_SND_BEBOB) += bebob/
=======
# To find a header included by define_trace.h.
CFLAGS_amdtp-stream.o	:= -I$(src)

snd-firewire-lib-objs := lib.o iso-resources.o packets-buffer.o \
			 fcp.o cmp.o amdtp-stream.o amdtp-am824.o
snd-isight-objs := isight.o

obj-$(CONFIG_SND_FIREWIRE_LIB) += snd-firewire-lib.o
obj-$(CONFIG_SND_DICE) += dice/
obj-$(CONFIG_SND_OXFW) += oxfw/
obj-$(CONFIG_SND_ISIGHT) += snd-isight.o
obj-$(CONFIG_SND_FIREWORKS) += fireworks/
obj-$(CONFIG_SND_BEBOB) += bebob/
obj-$(CONFIG_SND_FIREWIRE_DIGI00X) += digi00x/
obj-$(CONFIG_SND_FIREWIRE_TASCAM) += tascam/
>>>>>>> v4.9.227
