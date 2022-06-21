#
# linux/arch/arm/tools/Makefile
#
# Copyright (C) 2001 Russell King
#

<<<<<<< HEAD
include/generated/mach-types.h: $(src)/gen-mach-types $(src)/mach-types
	@$(kecho) '  Generating $@'
	@mkdir -p $(dir $@)
	$(Q)$(AWK) -f $^ > $@ || { rm -f $@; /bin/false; }
=======
quiet_cmd_gen_mach = GEN     $@
      cmd_gen_mach = mkdir -p $(dir $@) && \
		     $(AWK) -f $(filter-out $(PHONY),$^) > $@ || \
		     { rm -f $@; /bin/false; }

include/generated/mach-types.h: $(src)/gen-mach-types $(src)/mach-types FORCE
	$(call if_changed,gen_mach)
>>>>>>> v4.9.227
