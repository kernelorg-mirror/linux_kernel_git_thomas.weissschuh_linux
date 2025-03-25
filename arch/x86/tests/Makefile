# SPDX-License-Identifier: GPL-2.0

include $(srctree)/init/Makefile.nolibc

obj-$(CONFIG_X86_UAPI_TEST) += x86-tests.o
x86-tests-y += x86-tests-main.o

userprogs += x86_test_mremap_vdso
x86_test_mremap_vdso-userccflags := $(NOLIBC_USERCFLAGS)
CFLAGS_x86-tests-main.o := -Wa,-I$(obj)
$(obj)/x86-tests-main.o: $(obj)/x86_test_mremap_vdso
