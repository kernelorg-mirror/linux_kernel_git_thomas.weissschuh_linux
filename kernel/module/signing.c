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

/*
 * Verify the signature on a module.
 */
static int mod_verify_sig(const void *mod, struct load_info *info)
{
	struct module_signature ms;
	size_t sig_len, modlen = info->len;
	int ret;

	if (modlen <= sizeof(ms))
		return -EBADMSG;

	memcpy(&ms, mod + (modlen - sizeof(ms)), sizeof(ms));

	if (ms.id_type != MODULE_SIGNATURE_TYPE_PKCS7) {
		pr_err("module: not signed with expected PKCS#7 message\n");
		return -ENOPKG;
	}

	ret = mod_check_sig(&ms, modlen, "module");
	if (ret)
		return ret;

	sig_len = be32_to_cpu(ms.sig_len);
	modlen -= sig_len + sizeof(ms);
	info->len = modlen;

	return verify_pkcs7_signature(mod, modlen, mod + modlen, sig_len,
				      VERIFY_USE_SECONDARY_KEYRING,
				      VERIFYING_MODULE_SIGNATURE,
				      NULL, NULL);
}

int module_sig_check(struct load_info *info, int flags)
{
	int err;
	const unsigned long markerlen = sizeof(MODULE_SIGNATURE_MARKER) - 1;
	const void *mod = info->hdr;
	bool mangled_module = flags & (MODULE_INIT_IGNORE_MODVERSIONS |
				       MODULE_INIT_IGNORE_VERMAGIC);
	/*
	 * Do not allow mangled modules as a module with version information
	 * removed is no longer the module that was signed.
	 */
	if (mangled_module ||
	    info->len <= markerlen ||
	    memcmp(mod + info->len - markerlen, MODULE_SIGNATURE_MARKER, markerlen) != 0)
		return -ENODATA;

	/* We truncate the module to discard the signature */
	info->len -= markerlen;

	err = mod_verify_sig(mod, info);
	if (err)
		return err;

	info->sig_ok = true;
	return 0;
}
