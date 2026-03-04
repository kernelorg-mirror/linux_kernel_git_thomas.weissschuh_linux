// SPDX-License-Identifier: GPL-2.0-or-later
/* Module signature checker
 *
 * Copyright (C) 2012 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/module_signature.h>
#include <linux/string.h>
#include <linux/verification.h>
#include <uapi/linux/module.h>
#include "internal.h"

static int mod_split_sig(struct load_info *info, int flags, size_t *sig_len)
{
	const unsigned long markerlen = sizeof(MODULE_SIGNATURE_MARKER) - 1;
	const char *module_marker = (char *)info->hdr + info->len - markerlen;
	bool mangled_module = flags & (MODULE_INIT_IGNORE_MODVERSIONS |
				       MODULE_INIT_IGNORE_VERMAGIC);
	struct module_signature ms;
	int ret;

	/*
	 * Do not allow mangled modules as a module with version information
	 * removed is no longer the module that was signed.
	 */
	if (mangled_module ||
	    info->len <= markerlen ||
	    memcmp(module_marker, MODULE_SIGNATURE_MARKER, markerlen) != 0)
		return -ENODATA;

	/* We truncate the module to discard the signature */
	info->len -= markerlen;

	if (info->len <= sizeof(ms))
		return -EBADMSG;

	memcpy(&ms, (char *)info->hdr + (info->len - sizeof(ms)), sizeof(ms));

	if (ms.id_type != MODULE_SIGNATURE_TYPE_PKCS7) {
		pr_err("module: not signed with expected PKCS#7 message\n");
		return -ENOPKG;
	}

	ret = mod_check_sig(&ms, info->len, "module");
	if (ret)
		return ret;

	*sig_len = be32_to_cpu(ms.sig_len);
	info->len -= (*sig_len + sizeof(ms));
	return 0;
}

int module_sig_check(struct load_info *info, int flags)
{
	size_t sig_len;
	int err;

	err = mod_split_sig(info, flags, &sig_len);
	if (err)
		return err;

	err = verify_pkcs7_signature(info->hdr, info->len,
				     (char *)info->hdr + info->len, sig_len,
				     VERIFY_USE_SECONDARY_KEYRING,
				     VERIFYING_MODULE_SIGNATURE,
				     NULL, NULL);
	if (err)
		return err;

	info->sig_ok = true;
	return 0;
}
