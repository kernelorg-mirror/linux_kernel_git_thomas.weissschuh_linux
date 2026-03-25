# SPDX-License-Identifier: GPL-2.0

include $(srctree)/init/Makefile.nolibc

# Any varying coverage in these files is non-deterministic
# and is generally not a function of system call inputs.
KCOV_INSTRUMENT := n

ifeq ($(CONFIG_KCSAN),y)
KBUILD_CFLAGS += -g -fno-omit-frame-pointer
endif

obj-y += update.o sync.o
obj-$(CONFIG_TREE_SRCU) += srcutree.o
obj-$(CONFIG_TINY_SRCU) += srcutiny.o
obj-$(CONFIG_RCU_TORTURE_TEST) += rcutorture.o
obj-$(CONFIG_RCU_SCALE_TEST) += rcuscale.o
obj-$(CONFIG_RCU_REF_SCALE_TEST) += refscale.o
obj-$(CONFIG_TREE_RCU) += tree.o
obj-$(CONFIG_TINY_RCU) += tiny.o
obj-$(CONFIG_RCU_NEED_SEGCBLIST) += rcu_segcblist.o

userprogs += rcutorture-init

rcutorture-init-userccflags += -static $(NOLIBC_USERCFLAGS)

quiet_cmd_gen_init_cpio = GEN     $@
      cmd_gen_init_cpio = cd $(obj); $(realpath $(objtree))/usr/gen_init_cpio -t 0 $(src)/initrd_list -o $(notdir $@)

$(obj)/rcutorture-initrd: $(objtree)/usr/gen_init_cpio $(obj)/rcutorture-init $(src)/initrd_list FORCE
	$(call if_changed,gen_init_cpio)

always-$(CONFIG_RCU_TORTURE_TEST) += rcutorture-initrd
