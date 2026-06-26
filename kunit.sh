#/bin/bash

set -e
set -x

./tools/testing/kunit/kunit.py run --arch x86_64 --kconfig_add CONFIG_POSIX_AUX_CLOCKS=y --kconfig_add CONFIG_WERROR=y auxclock
