ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
obj-$(if $(KBUILD_EXTMOD),m,$(CONFIG_BENCH_MATH64)) := bench_math64.o

bench_math64-y := ops.o kunit.o

else
# normal makefile
KDIR ?= /lib/modules/`uname -r`/build

modules:

%:
	$(MAKE) -C $(KDIR) M=$$PWD $@

endif
