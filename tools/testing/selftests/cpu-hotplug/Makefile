all:

<<<<<<< HEAD
run_tests:
	@/bin/bash ./on-off-test.sh || echo "cpu-hotplug selftests: [FAIL]"

run_full_test:
	@/bin/bash ./on-off-test.sh -a || echo "cpu-hotplug selftests: [FAIL]"
=======
TEST_PROGS := cpu-on-off-test.sh

include ../lib.mk

run_full_test:
	@/bin/bash ./cpu-on-off-test.sh -a || echo "cpu-hotplug selftests: [FAIL]"
>>>>>>> v4.9.227

clean:
