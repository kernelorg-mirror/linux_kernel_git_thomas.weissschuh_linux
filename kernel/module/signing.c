// SPDX-License-Identifier: GPL-2.0-or-later
/* Module signature checker
 *
 * Copyright (C) 2012 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#include <linux/types.h>
#include <linux/verification.h>
#include "internal.h"

int module_sig_check(const void *mod, size_t modlen, const void *sig, size_t siglen)
{
	return verify_pkcs7_signature(mod, modlen, sig, siglen,
				      VERIFY_USE_SECONDARY_KEYRING,
				      VERIFYING_MODULE_SIGNATURE,
				      NULL, NULL);
}
