include $(srctree)/init/Makefile.nolibc

ccflags-y				:= -I$(obj)

obj-$(CONFIG_VDSO_UAPI_TEST)		+= vdso-tests.o
vdso-tests-y				+= vdso-tests-main.o

parse_vdso-userccflags			:= -isystem usr/include

userprogs				+= vdso_test_abi
vdso_test_abi-userccflags		:= $(NOLIBC_USERCFLAGS)
vdso_test_abi-userldlibs		:= $(NOLIBC_USERLDLIBS)

userprogs				+= vdso_test_chacha
vdso_test_chacha_prog-userccflags	:= $(NOLIBC_USERCFLAGS) -idirafter $(srctree)/tools/include
vgetrandom-chacha-userccflags		:= $(NOLIBC_USERCFLAGS) \
					   -idirafter $(srctree)/tools/include \
					   -idirafter $(srctree)/tools/include/generated \
					   -idirafter $(srctree)/arch/$(or $(HEADER_ARCH),$(SRCARCH))/include \
					   -idirafter $(srctree)/include \
					   -Wa,--noexecstack -x assembler-with-cpp
vdso_test_chacha-objs			:= vdso_test_chacha_prog.o vgetrandom-chacha.o
vdso_test_chacha-userldflags		:= $(NOLIBC_USERLDFLAGS)

userprogs				+= vdso_test_clock_getres
vdso_test_clock_getres-userccflags	:= -static

userprogs				+= vdso_test_correctness
vdso_test_correctness-userccflags	:= -static -isystem usr/include

userprogs				+= vdso_test_getcpu
vdso_test_getcpu-userccflags		:= $(NOLIBC_USERCFLAGS)
vdso_test_getcpu-userldlibs		:= $(NOLIBC_USERLDLIBS)

userprogs				+= vdso_test_getrandom
vdso_test_getrandom-objs		:= vdso_test_getrandom.o parse_vdso.o
vdso_test_getrandom-userccflags		:= -isystem usr/include
vdso_test_getrandom-userldflags		:= -static

userprogs				+= vdso_test_gettimeofday
vdso_test_gettimeofday-userccflags	:= $(NOLIBC_USERCFLAGS)
vdso_test_gettimeofday-userldlibs	:= $(NOLIBC_USERLDLIBS)

$(obj)/vdso-tests-main.o: $(obj)/vdso_test_abi $(obj)/vdso_test_chacha $(obj)/vdso_test_getcpu $(obj)/vdso_test_gettimeofday
ifdef CONFIG_CC_CAN_LINK_STATIC
$(obj)/vdso-tests-main.o: $(obj)/vdso_test_correctness $(obj)/vdso_test_clock_getres $(obj)/vdso_test_getrandom
endif
