/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_MFD_CROS_EC_H
#define __LINUX_MFD_CROS_EC_H

#include <linux/compiler_attributes.h>

struct cros_ec_dev;
struct platform_device;

static __always_inline struct cros_ec_dev *cros_ec_mfd_get_ec_dev(struct platform_device *pdev)
{
	return dev_get_drvdata(pdev->dev.parent);
}

#endif /* __LINUX_MFD_CROS_EC_H */
