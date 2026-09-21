#!/bin/bash

set -x
set -e

cat /sys/kernel/debug/dynamic_debug/control | grep rust_misc_device.rs
cat /dev/rust-misc-device
dmesg | tail
echo 'file samples/rust/rust_misc_device.rs +pd' > /sys/kernel/debug/dynamic_debug/control
cat /sys/kernel/debug/dynamic_debug/control | grep rust_misc_device.rs
cat /dev/rust-misc-device
dmesg | tail -n 40
