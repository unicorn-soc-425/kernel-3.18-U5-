CC		= $(CROSS_COMPILE)gcc
BUILD_OUTPUT	:= $(CURDIR)
<<<<<<< HEAD
PREFIX		:= /usr
DESTDIR		:=
=======
PREFIX		?= /usr
DESTDIR		?=
>>>>>>> v4.9.227

ifeq ("$(origin O)", "command line")
	BUILD_OUTPUT := $(O)
endif

turbostat : turbostat.c
<<<<<<< HEAD
CFLAGS +=	-Wall
CFLAGS +=	-DMSRHEADER='"../../../../arch/x86/include/uapi/asm/msr-index.h"'
=======
CFLAGS +=	-Wall -I../../../include
CFLAGS +=	-DMSRHEADER='"../../../../arch/x86/include/asm/msr-index.h"'
>>>>>>> v4.9.227

%: %.c
	@mkdir -p $(BUILD_OUTPUT)
	$(CC) $(CFLAGS) $< -o $(BUILD_OUTPUT)/$@

.PHONY : clean
clean :
	@rm -f $(BUILD_OUTPUT)/turbostat

install : turbostat
	install -d  $(DESTDIR)$(PREFIX)/bin
	install $(BUILD_OUTPUT)/turbostat $(DESTDIR)$(PREFIX)/bin/turbostat
	install -d  $(DESTDIR)$(PREFIX)/share/man/man8
	install turbostat.8 $(DESTDIR)$(PREFIX)/share/man/man8
