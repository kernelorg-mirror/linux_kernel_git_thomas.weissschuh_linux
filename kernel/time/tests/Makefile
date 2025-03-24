# SPDX-License-Identifier: GPL-2.0

include $(srctree)/init/Makefile.nolibc

ccflags-y					:= -I$(obj)

obj-$(CONFIG_TIMER_UAPI_TEST)			+= timer-tests.o
timer-tests-y					+= timer-tests-main.o

userprogs					+= timer_test_posix_timers
timer_test_posix_timers-userccflags		:= -static -I$(srctree)

obj-$(CONFIG_TIME_NS_UAPI_TEST)			+= timens-tests.o
timens-tests-y					+= timens-tests-main.o

userprogs					+= timens_test_clock_nanosleep
timens_test_clock_nanosleep-userccflags		:= -static

userprogs					+= timens_test_exec
timens_test_exec-userccflags			:= $(NOLIBC_USERCFLAGS)
timens_test_exec-userldlibs			:= $(NOLIBC_USERLDLIBS)

userprogs					+= timens_test_futex
timens_test_futex-userccflags			:= $(NOLIBC_USERCFLAGS)

userprogs					+= timens_test_gettime_perf
timens_test_gettime_perf-userccflags		:= -static

userprogs					+= timens_test_procfs
timens_test_procfs-userccflags			:= $(NOLIBC_USERCFLAGS)
timens_test_procfs-userldlibs			:= $(NOLIBC_USERLDLIBS)

userprogs					+= timens_test_timens
timens_test_timens-userccflags			:= -static

userprogs					+= timens_test_timer
timens_test_timer-userccflags			:= $(NOLIBC_USERCFLAGS)

userprogs					+= timens_test_timerfd
timens_test_timerfd-userccflags			:= $(NOLIBC_USERCFLAGS)

userprogs					+= timens_test_vfork_exec
timens_test_vfork_exec-userccflags		:= -static

$(obj)/timer-tests-main.o: $(obj)/timens_test_exec $(obj)/timens_test_futex $(obj)/timens_test_procfs $(obj)/timens_test_timer $(obj)/timens_test_timerfd
ifdef CONFIG_CC_CAN_LINK_STATIC
$(obj)/timer-tests-main.o: $(obj)/timer_test_posix_timers $(obj)/timens_test_clock_nanosleep $(obj)/timens_test_gettime_perf $(obj)/timens_test_vfork_exec $(obj)/timens_test_timens
endif
