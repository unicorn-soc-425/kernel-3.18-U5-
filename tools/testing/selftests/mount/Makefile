# Makefile for mount selftests.
<<<<<<< HEAD

all: unprivileged-remount-test

unprivileged-remount-test: unprivileged-remount-test.c
	gcc -Wall -O2 unprivileged-remount-test.c -o unprivileged-remount-test

# Allow specific tests to be selected.
test_unprivileged_remount: unprivileged-remount-test
	@if [ -f /proc/self/uid_map ] ; then ./unprivileged-remount-test ; fi

run_tests: all test_unprivileged_remount

clean:
	rm -f unprivileged-remount-test

.PHONY: all test_unprivileged_remount
=======
CFLAGS = -Wall \
         -O2
all: unprivileged-remount-test

unprivileged-remount-test: unprivileged-remount-test.c
	$(CC) $(CFLAGS) unprivileged-remount-test.c -o unprivileged-remount-test

include ../lib.mk

TEST_PROGS := unprivileged-remount-test
override RUN_TESTS := if [ -f /proc/self/uid_map ] ; \
		      then	\
				./unprivileged-remount-test ; \
		      else	\
				echo "WARN: No /proc/self/uid_map exist, test skipped." ; \
		      fi
override EMIT_TESTS := echo "$(RUN_TESTS)"

clean:
	rm -f unprivileged-remount-test
>>>>>>> v4.9.227
