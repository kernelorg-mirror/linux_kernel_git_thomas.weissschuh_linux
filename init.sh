#!/bin/sh

set -ex

test_mremap_vdso_32
test_mremap_vdso_64
test_syscall_vdso_32
# test_vsyscall_32
# test_vsyscall_64
# unwind_vdso_32
vdso_restorer_32
# vdso_standalone_test_x86
vdso_test_abi
vdso_test_chacha
vdso_test_clock_getres
vdso_test_correctness
vdso_test_getcpu
vdso_test_getrandom
vdso_test_gettimeofday

echo c > /proc/sysrq-trigger
