<<<<<<< HEAD
CC = $(CROSS_COMPILE)gcc
=======
>>>>>>> v4.9.227
CFLAGS = -Wall

test_objs = open-unlink create-read

all: $(test_objs)

<<<<<<< HEAD
run_tests: all
	@/bin/bash ./efivarfs.sh || echo "efivarfs selftests: [FAIL]"
=======
TEST_PROGS := efivarfs.sh
TEST_FILES := $(test_objs)

include ../lib.mk
>>>>>>> v4.9.227

clean:
	rm -f $(test_objs)
