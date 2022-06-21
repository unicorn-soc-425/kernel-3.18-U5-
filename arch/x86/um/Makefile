#
# Copyright (C) 2002 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
#

ifeq ($(CONFIG_X86_32),y)
	BITS := 32
else
	BITS := 64
endif

<<<<<<< HEAD
obj-y = bug.o bugs_$(BITS).o delay.o fault.o ksyms.o ldt.o \
=======
obj-y = bug.o bugs_$(BITS).o delay.o fault.o ldt.o \
>>>>>>> v4.9.227
	ptrace_$(BITS).o ptrace_user.o setjmp_$(BITS).o signal.o \
	stub_$(BITS).o stub_segv.o \
	sys_call_table_$(BITS).o sysrq_$(BITS).o tls_$(BITS).o \
	mem_$(BITS).o subarch.o os-$(OS)/

ifeq ($(CONFIG_X86_32),y)

obj-y += checksum_32.o
<<<<<<< HEAD
obj-$(CONFIG_BINFMT_ELF) += elfcore.o

subarch-y = ../lib/string_32.o ../lib/atomic64_32.o ../lib/atomic64_cx8_32.o
subarch-$(CONFIG_RWSEM_XCHGADD_ALGORITHM) += ../lib/rwsem.o
subarch-$(CONFIG_HIGHMEM) += ../mm/highmem_32.o
=======
obj-$(CONFIG_ELF_CORE) += elfcore.o

subarch-y = ../lib/string_32.o ../lib/atomic64_32.o ../lib/atomic64_cx8_32.o
subarch-$(CONFIG_RWSEM_XCHGADD_ALGORITHM) += ../lib/rwsem.o
>>>>>>> v4.9.227

else

obj-y += syscalls_64.o vdso/

<<<<<<< HEAD
subarch-y = ../lib/csum-partial_64.o ../lib/memcpy_64.o ../lib/thunk_64.o \
=======
subarch-y = ../lib/csum-partial_64.o ../lib/memcpy_64.o ../entry/thunk_64.o \
>>>>>>> v4.9.227
		../lib/rwsem.o

endif

subarch-$(CONFIG_MODULES) += ../kernel/module.o

USER_OBJS := bugs_$(BITS).o ptrace_user.o fault.o

extra-y += user-offsets.s
$(obj)/user-offsets.s: c_flags = -Wp,-MD,$(depfile) $(USER_CFLAGS) \
	-Iarch/x86/include/generated

UNPROFILE_OBJS := stub_segv.o
CFLAGS_stub_segv.o := $(CFLAGS_NO_HARDENING)

include arch/um/scripts/Makefile.rules
