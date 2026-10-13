#!/bin/sh
# SPDX-License-Identifier: GPL-2.0

CC_VERSION_TEXT="$(LC_ALL=C $@ --version 2>/dev/null | head -n 1)"

case "${CC_VERSION_TEXT}" in
	*"clang"*)
		xxx="--ld-path=${LD}"
		;;
esac

cat << "END" | $@ ${xxx} -Werror -Wl,--fatal-warnings -x c - -o /dev/null >/dev/null 2>&1
#include <stdio.h>
int main(void)
{
	printf("\n");
	return 0;
}
END
