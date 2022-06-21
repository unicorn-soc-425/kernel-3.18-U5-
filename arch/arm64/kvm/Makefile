#
# Makefile for Kernel-based Virtual Machine module
#

<<<<<<< HEAD
ccflags-y += -Ivirt/kvm -Iarch/arm64/kvm
=======
ccflags-y += -Iarch/arm64/kvm
>>>>>>> v4.9.227
CFLAGS_arm.o := -I.
CFLAGS_mmu.o := -I.

KVM=../../../virt/kvm
ARM=../../../arch/arm/kvm

obj-$(CONFIG_KVM_ARM_HOST) += kvm.o
<<<<<<< HEAD

kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/kvm_main.o $(KVM)/coalesced_mmio.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(ARM)/arm.o $(ARM)/mmu.o $(ARM)/mmio.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(ARM)/psci.o $(ARM)/perf.o

kvm-$(CONFIG_KVM_ARM_HOST) += emulate.o inject_fault.o regmap.o
kvm-$(CONFIG_KVM_ARM_HOST) += hyp.o hyp-init.o handle_exit.o
kvm-$(CONFIG_KVM_ARM_HOST) += guest.o reset.o sys_regs.o sys_regs_generic_v8.o

kvm-$(CONFIG_KVM_ARM_VGIC) += $(KVM)/arm/vgic.o
kvm-$(CONFIG_KVM_ARM_VGIC) += $(KVM)/arm/vgic-v2.o
kvm-$(CONFIG_KVM_ARM_VGIC) += vgic-v2-switch.o
kvm-$(CONFIG_KVM_ARM_VGIC) += $(KVM)/arm/vgic-v3.o
kvm-$(CONFIG_KVM_ARM_VGIC) += vgic-v3-switch.o
kvm-$(CONFIG_KVM_ARM_TIMER) += $(KVM)/arm/arch_timer.o
=======
obj-$(CONFIG_KVM_ARM_HOST) += hyp/

kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/kvm_main.o $(KVM)/coalesced_mmio.o $(KVM)/eventfd.o $(KVM)/vfio.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(ARM)/arm.o $(ARM)/mmu.o $(ARM)/mmio.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(ARM)/psci.o $(ARM)/perf.o

kvm-$(CONFIG_KVM_ARM_HOST) += inject_fault.o regmap.o
kvm-$(CONFIG_KVM_ARM_HOST) += hyp.o hyp-init.o handle_exit.o
kvm-$(CONFIG_KVM_ARM_HOST) += guest.o debug.o reset.o sys_regs.o sys_regs_generic_v8.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/arm/aarch32.o

kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/arm/vgic/vgic.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/arm/vgic/vgic-init.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/arm/vgic/vgic-irqfd.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/arm/vgic/vgic-v2.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/arm/vgic/vgic-v3.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/arm/vgic/vgic-mmio.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/arm/vgic/vgic-mmio-v2.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/arm/vgic/vgic-mmio-v3.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/arm/vgic/vgic-kvm-device.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/arm/vgic/vgic-its.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/irqchip.o
kvm-$(CONFIG_KVM_ARM_HOST) += $(KVM)/arm/arch_timer.o
kvm-$(CONFIG_KVM_ARM_PMU) += $(KVM)/arm/pmu.o
>>>>>>> v4.9.227
