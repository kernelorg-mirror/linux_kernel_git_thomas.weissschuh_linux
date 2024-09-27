#!/bin/bash

set -e

mkdir -p initrd/tests
INSTALL_PATH="$(realpath initrd/tests)"
export INSTALL_PATH

pushd tools/testing/selftests/vDSO/

make -j20
make install

popd

pushd tools/testing/selftests/x86/

make -j20
make install

popd

pushd tools/testing/selftests/timens

make -j20
make install

popd

RUN="--run init.sh"
RUN=

~/bin/make-tiny-image.py $RUN --output initramfs.cpio ./init.sh ./initrd/tests/* /home/t-8ch/src/vdsotest/vdsotest ./vdso-test-all.sh

qemu-system-x86_64 -display none -no-reboot -kernel out/x86_64/arch/x86/boot/bzImage -initrd initramfs.cpio -serial stdio -M pc -append "console=ttyS0,9600 i8042.noaux panic=-1"
