# List of files in the vdso, has to be asm only for now

<<<<<<< HEAD
obj-vdso32 = gettimeofday.o clock_getres.o clock_gettime.o note.o
=======
KCOV_INSTRUMENT := n

obj-vdso32 = gettimeofday.o clock_getres.o clock_gettime.o note.o getcpu.o
>>>>>>> v4.9.227

# Build rules

targets := $(obj-vdso32) vdso32.so vdso32.so.dbg
obj-vdso32 := $(addprefix $(obj)/, $(obj-vdso32))

KBUILD_AFLAGS_31 := $(filter-out -m64,$(KBUILD_AFLAGS))
KBUILD_AFLAGS_31 += -m31 -s

KBUILD_CFLAGS_31 := $(filter-out -m64,$(KBUILD_CFLAGS))
KBUILD_CFLAGS_31 += -m31 -fPIC -shared -fno-common -fno-builtin
KBUILD_CFLAGS_31 += -nostdlib -Wl,-soname=linux-vdso32.so.1 \
<<<<<<< HEAD
			$(call cc-ldoption, -Wl$(comma)--hash-style=sysv)
=======
			$(call cc-ldoption, -Wl$(comma)--hash-style=both)
>>>>>>> v4.9.227

$(targets:%=$(obj)/%.dbg): KBUILD_CFLAGS = $(KBUILD_CFLAGS_31)
$(targets:%=$(obj)/%.dbg): KBUILD_AFLAGS = $(KBUILD_AFLAGS_31)

obj-y += vdso32_wrapper.o
extra-y += vdso32.lds
CPPFLAGS_vdso32.lds += -P -C -U$(ARCH)

<<<<<<< HEAD
# Disable gcov profiling for VDSO code
GCOV_PROFILE := n
=======
# Disable gcov profiling, ubsan and kasan for VDSO code
GCOV_PROFILE := n
UBSAN_SANITIZE := n
KASAN_SANITIZE := n
>>>>>>> v4.9.227

# Force dependency (incbin is bad)
$(obj)/vdso32_wrapper.o : $(obj)/vdso32.so

# link rule for the .so file, .lds has to be first
<<<<<<< HEAD
$(obj)/vdso32.so.dbg: $(src)/vdso32.lds $(obj-vdso32)
=======
$(obj)/vdso32.so.dbg: $(src)/vdso32.lds $(obj-vdso32) FORCE
>>>>>>> v4.9.227
	$(call if_changed,vdso32ld)

# strip rule for the .so file
$(obj)/%.so: OBJCOPYFLAGS := -S
$(obj)/%.so: $(obj)/%.so.dbg FORCE
	$(call if_changed,objcopy)

# assembly rules for the .S files
<<<<<<< HEAD
$(obj-vdso32): %.o: %.S
=======
$(obj-vdso32): %.o: %.S FORCE
>>>>>>> v4.9.227
	$(call if_changed_dep,vdso32as)

# actual build commands
quiet_cmd_vdso32ld = VDSO32L $@
<<<<<<< HEAD
      cmd_vdso32ld = $(CC) $(c_flags) -Wl,-T $^ -o $@
=======
      cmd_vdso32ld = $(CC) $(c_flags) -Wl,-T $(filter %.lds %.o,$^) -o $@
>>>>>>> v4.9.227
quiet_cmd_vdso32as = VDSO32A $@
      cmd_vdso32as = $(CC) $(a_flags) -c -o $@ $<

# install commands for the unstripped file
quiet_cmd_vdso_install = INSTALL $@
      cmd_vdso_install = cp $(obj)/$@.dbg $(MODLIB)/vdso/$@

vdso32.so: $(obj)/vdso32.so.dbg
	@mkdir -p $(MODLIB)/vdso
	$(call cmd,vdso_install)

vdso_install: vdso32.so
