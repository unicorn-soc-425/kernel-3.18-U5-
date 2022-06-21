# Taken from perf makefile
uname_M := $(shell uname -m 2>/dev/null || echo not)
<<<<<<< HEAD
ARCH ?= $(shell echo $(uname_M) | sed -e s/i.86/i386/)
ifeq ($(ARCH),i386)
        ARCH := x86
endif
ifeq ($(ARCH),x86_64)
	ARCH := x86
endif


all:
ifeq ($(ARCH),x86)
	gcc breakpoint_test.c -o breakpoint_test
else
	echo "Not an x86 target, can't build breakpoints selftests"
endif

run_tests:
	@./breakpoint_test || echo "breakpoints selftests: [FAIL]"

clean:
	rm -fr breakpoint_test
=======
ARCH ?= $(shell echo $(uname_M) | sed -e s/i.86/x86/ -e s/x86_64/x86/)

ifeq ($(ARCH),x86)
TEST_PROGS := breakpoint_test
endif

TEST_PROGS += step_after_suspend_test

all: $(TEST_PROGS)

include ../lib.mk

clean:
	rm -fr breakpoint_test step_after_suspend_test
>>>>>>> v4.9.227
