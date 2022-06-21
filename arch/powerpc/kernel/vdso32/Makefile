
# List of files in the vdso, has to be asm only for now

obj-vdso32-$(CONFIG_PPC64) = getcpu.o
obj-vdso32 = sigtramp.o gettimeofday.o datapage.o cacheflush.o note.o \
		$(obj-vdso32-y)

# Build rules

ifeq ($(CONFIG_PPC32),y)
CROSS32CC := $(CC)
endif

targets := $(obj-vdso32) vdso32.so vdso32.so.dbg
obj-vdso32 := $(addprefix $(obj)/, $(obj-vdso32))

GCOV_PROFILE := n
<<<<<<< HEAD

ccflags-y := -shared -fno-common -fno-builtin
ccflags-y += -nostdlib -Wl,-soname=linux-vdso32.so.1 \
		$(call cc-ldoption, -Wl$(comma)--hash-style=sysv)
=======
UBSAN_SANITIZE := n

ccflags-y := -shared -fno-common -fno-builtin
ccflags-y += -nostdlib -Wl,-soname=linux-vdso32.so.1 \
		$(call cc-ldoption, -Wl$(comma)--hash-style=both)
>>>>>>> v4.9.227
asflags-y := -D__VDSO32__ -s

obj-y += vdso32_wrapper.o
extra-y += vdso32.lds
CPPFLAGS_vdso32.lds += -P -C -Upowerpc

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
      cmd_vdso32ld = $(CROSS32CC) $(c_flags) -Wl,-T $^ -o $@
=======
      cmd_vdso32ld = $(CROSS32CC) $(c_flags) -o $@ -Wl,-T$(filter %.lds,$^) $(filter %.o,$^)
>>>>>>> v4.9.227
quiet_cmd_vdso32as = VDSO32A $@
      cmd_vdso32as = $(CROSS32CC) $(a_flags) -c -o $@ $<

# install commands for the unstripped file
quiet_cmd_vdso_install = INSTALL $@
      cmd_vdso_install = cp $(obj)/$@.dbg $(MODLIB)/vdso/$@

vdso32.so: $(obj)/vdso32.so.dbg
	@mkdir -p $(MODLIB)/vdso
	$(call cmd,vdso_install)

vdso_install: vdso32.so
