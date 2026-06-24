// SPDX-License-Identifier: GPL-2.0

#include <linux/cache.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/sysfs.h>

asm (
"	.pushsection .rodata, \"a\"	\n"
"	.global vdso_debug_data		\n"
"vdso_debug_data:			\n"
"	.incbin \"vdso_debug.tar.xz\"	\n"
"	.global vdso_debug_data_end	\n"
"vdso_debug_data_end:			\n"
"	.popsection			\n"
);

extern char vdso_debug_data[];
extern char vdso_debug_data_end[];

static struct bin_attribute vdso_debug_attr __ro_after_init =
	__BIN_ATTR_SIMPLE_RO(vdso_debug.tar.xz, 0444);

static int __init vdso_debug_init(void)
{
	vdso_debug_attr.private = vdso_debug_data;
	vdso_debug_attr.size = vdso_debug_data_end - vdso_debug_data;

	return sysfs_create_bin_file(kernel_kobj, &vdso_debug_attr);
}

static void __exit vdso_debug_exit(void)
{
	sysfs_remove_bin_file(kernel_kobj, &vdso_debug_attr);
}

module_init(vdso_debug_init);
module_exit(vdso_debug_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thomas Weißschuh <thomas.weissschuh@linutronix.de>");
MODULE_DESCRIPTION("Provide vDSO debug information at runtime");
