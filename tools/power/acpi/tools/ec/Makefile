<<<<<<< HEAD
ec_access: ec_access.o
	$(ECHO) "  LD      " $@
	$(QUIET) $(LD) $(CFLAGS) $(LDFLAGS) $< -o $@
	$(QUIET) $(STRIPCMD) $@

%.o: %.c
	$(ECHO) "  CC      " $@
	$(QUIET) $(CC) -c $(CFLAGS) -o $@ $<

all: ec_access

install:
	$(INSTALL) -d $(DESTDIR)${sbindir}
	$(INSTALL_PROGRAM) ec_access $(DESTDIR)${sbindir}

uninstall:
	- rm -f $(DESTDIR)${sbindir}/ec_access

clean:
	-rm -f $(OUTPUT)ec_access

.PHONY: all install uninstall
=======
# tools/power/acpi/tools/acpidump/Makefile - ACPI tool Makefile
#
# Copyright (c) 2015, Intel Corporation
#   Author: Lv Zheng <lv.zheng@intel.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; version 2
# of the License.

include ../../Makefile.config

TOOL = ec
TOOL_OBJS = \
	ec_access.o

include ../../Makefile.rules
>>>>>>> v4.9.227
