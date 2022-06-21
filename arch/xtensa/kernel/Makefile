#
# Makefile for the Linux/Xtensa kernel.
#

extra-y := head.o vmlinux.lds

obj-y := align.o coprocessor.o entry.o irq.o pci-dma.o platform.o process.o \
	 ptrace.o setup.o signal.o stacktrace.o syscall.o time.o traps.o \
	 vectors.o

<<<<<<< HEAD
obj-$(CONFIG_KGDB) += xtensa-stub.o
=======
>>>>>>> v4.9.227
obj-$(CONFIG_PCI) += pci.o
obj-$(CONFIG_MODULES) += xtensa_ksyms.o module.o
obj-$(CONFIG_FUNCTION_TRACER) += mcount.o
obj-$(CONFIG_SMP) += smp.o mxhead.o
<<<<<<< HEAD

AFLAGS_head.o += -mtext-section-literals
=======
obj-$(CONFIG_XTENSA_VARIANT_HAVE_PERF_EVENTS) += perf_event.o
obj-$(CONFIG_HAVE_HW_BREAKPOINT) += hw_breakpoint.o

AFLAGS_head.o += -mtext-section-literals
AFLAGS_mxhead.o += -mtext-section-literals
>>>>>>> v4.9.227

# In the Xtensa architecture, assembly generates literals which must always
# precede the L32R instruction with a relative offset less than 256 kB.
# Therefore, the .text and .literal section must be combined in parenthesis
# in the linker script, such as: *(.literal .text).
#
# We need to post-process the generated vmlinux.lds scripts to convert
# *(xxx.text) to  *(xxx.literal xxx.text) for the following text sections:
#   .text .ref.text .*init.text .*exit.text .text.*
#
# Replicate rules in scripts/Makefile.build

<<<<<<< HEAD
sed-y = -e 's/\*(\(\.[a-z]*it\|\.ref\|\)\.text)/*(\1.literal \1.text)/g' \
	-e 's/\.text\.unlikely/.literal.unlikely .text.unlikely/g'	 \
	-e 's/\*(\(\.text\.[a-z]*\))/*(\1.literal \1)/g'
=======
sed-y = -e ':a; s/\*(\([^)]*\)\.text\.unlikely/*(\1.literal.unlikely .{text}.unlikely/; ta; ' \
	-e ':b; s/\*(\([^)]*\)\.text\(\.[a-z]*\)/*(\1.{text}\2.literal .{text}\2/; tb; ' \
	-e ':c; s/\*(\([^)]*\)\(\.[a-z]*it\|\.ref\)\.text/*(\1\2.literal \2.{text}/; tc; ' \
	-e ':d; s/\*(\([^)]\+ \|\)\.text/*(\1.literal .{text}/; td; ' \
	-e 's/\.{text}/.text/g'
>>>>>>> v4.9.227

quiet_cmd__cpp_lds_S = LDS     $@
cmd__cpp_lds_S = $(CPP) $(cpp_flags) -P -C -Uxtensa -D__ASSEMBLY__ $<    \
                 | sed $(sed-y) >$@

$(obj)/vmlinux.lds: $(src)/vmlinux.lds.S FORCE
	$(call if_changed_dep,_cpp_lds_S)
