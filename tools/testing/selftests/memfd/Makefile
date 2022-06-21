<<<<<<< HEAD
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -I../../../../include/uapi/
CFLAGS += -I../../../../include/

all:
	gcc $(CFLAGS) memfd_test.c -o memfd_test

run_tests: all
	gcc $(CFLAGS) memfd_test.c -o memfd_test
	@./memfd_test || echo "memfd_test: [FAIL]"

build_fuse:
	gcc $(CFLAGS) fuse_mnt.c `pkg-config fuse --cflags --libs` -o fuse_mnt
	gcc $(CFLAGS) fuse_test.c -o fuse_test
=======
CC = $(CROSS_COMPILE)gcc
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -I../../../../include/uapi/
CFLAGS += -I../../../../include/
CFLAGS += -I../../../../usr/include/

TEST_PROGS := memfd_test

all: $(TEST_PROGS)

include ../lib.mk

build_fuse: fuse_mnt fuse_test

fuse_mnt.o: CFLAGS += $(shell pkg-config fuse --cflags)
fuse_mnt: LDFLAGS += $(shell pkg-config fuse --libs)
>>>>>>> v4.9.227

run_fuse: build_fuse
	@./run_fuse_test.sh || echo "fuse_test: [FAIL]"

clean:
	$(RM) memfd_test fuse_test
