// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Compute hashes for modules files and build a merkle tree.
 *
 * Copyright (C) 2025 Sebastian Andrzej Siewior <sebastian@breakpoint.cc>
 * Copyright (C) 2025 Thomas Weißschuh <linux@weissschuh.net>
 *
 * Structure of the Merkle tree:
 *
 * The full built modules are leaf nodes. They are hashed pairwise in the order
 * of modules.order to create internal nodes. These in turn are also hashed
 * pairwise to create the next higher level of internal nodes. This is repeated
 * up to a single root node. In case of an uneven amount of node on a level, the
 * last node is paired with itself.
 *
 * The single root node can then be embedded into vmlinux to validate all modules.
 */

#define _GNU_SOURCE 1
#include <arpa/inet.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <sys/mman.h>

#include <openssl/evp.h>
#include <openssl/err.h>

#include "ssl-common.h"

#include <linux/module_signature.h>
#include <xalloc.h>

static int hash_size;
static EVP_MD_CTX *ctx;

struct file_entry {
	char *name;
	unsigned int pos;
	unsigned char hash[EVP_MAX_MD_SIZE];
};

static struct file_entry *fh_list;
static size_t num_files;

struct leaf_hash {
	unsigned char hash[EVP_MAX_MD_SIZE];
};

struct mtree {
	struct leaf_hash **l;
	unsigned int *entries;
	unsigned int levels;
};

static unsigned int get_pow2(unsigned int val)
{
	return 31 - __builtin_clz(val);
}

static unsigned int roundup_pow2(unsigned int val)
{
	return 1 << (get_pow2(val - 1) + 1);
}

static unsigned int log2_roundup(unsigned int val)
{
	/* Avoid UB in __builtin_clz() */
	if (val == 1)
		return 1;
	return get_pow2(roundup_pow2(val));
}

static void hash_data(void *p, unsigned int pos, size_t size, void *ret_hash)
{
	unsigned char magic = 0x01;
	unsigned int pos_be;

	pos_be = htonl(pos);

	ERR(EVP_DigestInit_ex(ctx, NULL, NULL) != 1, "EVP_DigestInit_ex()");
	ERR(EVP_DigestUpdate(ctx, &magic, sizeof(magic)) != 1, "EVP_DigestUpdate(magic)");
	ERR(EVP_DigestUpdate(ctx, &pos_be, sizeof(pos_be)) != 1, "EVP_DigestUpdate(pos)");
	ERR(EVP_DigestUpdate(ctx, p, size) != 1, "EVP_DigestUpdate(data)");
	ERR(EVP_DigestFinal_ex(ctx, ret_hash, NULL) != 1, "EVP_DigestFinal_ex()");
}

static void hash_entry(void *left, void *right, void *ret_hash)
{
	unsigned char magic = 0x02;

	ERR(EVP_DigestInit_ex(ctx, NULL, NULL) != 1, "EVP_DigestInit_ex()");
	ERR(EVP_DigestUpdate(ctx, &magic, sizeof(magic)) != 1, "EVP_DigestUpdate(magic)");
	ERR(EVP_DigestUpdate(ctx, left, hash_size) != 1, "EVP_DigestUpdate(left)");
	ERR(EVP_DigestUpdate(ctx, right, hash_size) != 1, "EVP_DigestUpdate(right)");
	ERR(EVP_DigestFinal_ex(ctx, ret_hash, NULL) != 1, "EVP_DigestFinal_ex()");
}

static void hash_file(struct file_entry *fe)
{
	struct stat sb;
	int fd, ret;
	void *mem;

	fd = open(fe->name, O_RDONLY);
	if (fd < 0)
		err(1, "Failed to open %s", fe->name);

	ret = fstat(fd, &sb);
	if (ret)
		err(1, "Failed to stat %s", fe->name);

	mem = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (mem == MAP_FAILED)
		err(1, "Failed to mmap %s", fe->name);

	hash_data(mem, fe->pos, sb.st_size, fe->hash);

	munmap(mem, sb.st_size);
	close(fd);
}

static struct mtree *build_merkle(struct file_entry *fh, size_t num_files)
{
	unsigned int num_cur_le, num_prev_le;
	struct mtree *mt;

	if (!num_files)
		return NULL;

	mt = xmalloc(sizeof(*mt));
	mt->levels = log2_roundup(num_files);

	mt->l = xcalloc(sizeof(*mt->l), mt->levels);

	mt->entries = xcalloc(sizeof(*mt->entries), mt->levels);
	num_cur_le = (num_files + 1) / 2;
	mt->entries[0] = num_cur_le;
	mt->l[0] = xcalloc(sizeof(**mt->l), num_cur_le);

	/* First level of pairs */
	for (unsigned int i = 0; i < num_files; i += 2) {
		/* Hash the pair, or the last file with itself if it's odd. */
		void *right = i + 1 < num_files ? fh[i + 1].hash : fh[i].hash;

		hash_entry(fh[i].hash, right, mt->l[0][i / 2].hash);
	}

	for (unsigned int i = 1; i < mt->levels; i++) {
		num_prev_le = num_cur_le;

		num_cur_le = (num_prev_le + 1) / 2;
		mt->entries[i] = num_cur_le;
		mt->l[i] = xcalloc(sizeof(**mt->l), num_cur_le);

		for (unsigned int n = 0; n < num_prev_le; n += 2) {
			/* Hash the pair, or the last with itself if it's odd. */
			void *right = n + 1 < num_prev_le ?
					      mt->l[i - 1][n + 1].hash :
					      mt->l[i - 1][n].hash;
			hash_entry(mt->l[i - 1][n].hash, right,
				   mt->l[i][n / 2].hash);
		}
	}
	return mt;
}

static void free_mtree(struct mtree *mt)
{
	if (!mt)
		return;

	for (unsigned int i = 0; i < mt->levels; i++)
		free(mt->l[i]);

	free(mt->l);
	free(mt->entries);
	free(mt);
}

static void write_be_int(int fd, unsigned int v)
{
	unsigned int be_val = htonl(v);

	if (write(fd, &be_val, sizeof(be_val)) != sizeof(be_val))
		err(1, "Failed writing to file");
}

static void write_hash(int fd, const void *h)
{
	if (write(fd, h, hash_size) != hash_size)
		err(1, "Failed writing to file");
}

static void build_proof(struct mtree *mt, unsigned int n, int fd)
{
	struct file_entry *fe, *fe_sib;

	fe = &fh_list[n];

	if ((n & 1) == 0) {
		/* No pair, hash with itself */
		if (n + 1 == num_files)
			fe_sib = fe;
		else
			fe_sib = &fh_list[n + 1];
	} else {
		fe_sib = &fh_list[n - 1];
	}
	/* First comes the node position into the file */
	write_be_int(fd, n);

	/* Next is the sibling hash, followed by hashes in the tree */
	write_hash(fd, fe_sib->hash);

	for (unsigned int i = 0; i < mt->levels - 1; i++) {
		n >>= 1;
		if ((n & 1) == 0) {
			void *h;

			/* No pair, hash with itself */
			if (n + 1 == mt->entries[i])
				h = mt->l[i][n].hash;
			else
				h = mt->l[i][n + 1].hash;

			write_hash(fd, h);
		} else {
			write_hash(fd, mt->l[i][n - 1].hash);
		}
	}
}

static void append_module_signature_magic(int fd, unsigned int sig_len)
{
	const struct module_signature sig_info = {
		.id_type	= MODULE_SIGNATURE_TYPE_MERKLE,
		.sig_len	= htonl(sig_len),
	};
	const size_t sig_str_len = sizeof(MODULE_SIGNATURE_MARKER) - 1;
	const void *sig_str = MODULE_SIGNATURE_MARKER;

	if (write(fd, &sig_info, sizeof(sig_info)) != sizeof(sig_info))
		err(1, "write(sig_info) failed");

	if (write(fd, sig_str, sig_str_len) != sig_str_len)
		err(1, "write(magic_number) failed");
}

static void write_merkle_root(struct mtree *mt, const char *fp)
{
	unsigned int levels;
	unsigned char *h;
	FILE *f;

	if (mt) {
		levels = mt->levels;
		h = mt->l[mt->levels - 1][0].hash;
	} else {
		levels = 0;
		h = xcalloc(1, hash_size);
	}

	f = fopen(fp, "w");
	if (!f)
		err(1, "Failed to create %s", fp);

	fprintf(f, "#include <linux/module_hashes.h>\n\n");
	fprintf(f, "const struct\n");
	fprintf(f, "module_hashes_root module_hashes_root __module_hashes_section = {\n");

	fprintf(f, "\t.levels = %u,\n", levels);
	fprintf(f, "\t.hash = {");
	for (unsigned int i = 0; i < hash_size; i++) {
		char *space = "";

		if (!(i % 8))
			fprintf(f, "\n\t\t");

		if ((i + 1) % 8)
			space = " ";

		fprintf(f, "0x%02x,%s", h[i], space);
	}
	fprintf(f, "\n\t},");

	fprintf(f, "\n};\n");

	if (fclose(f))
		err(1, "Failed to write %s", fp);

	if (!mt)
		free(h);
}

static char *xstrdup_replace_suffix(const char *str, const char *new_suffix)
{
	const char *current_suffix;
	size_t base_len;

	current_suffix = strrchr(str, '.');
	if (!current_suffix)
		errx(1, "No existing suffix in '%s'", str);

	base_len = current_suffix - str;

	return xasprintf("%.*s%s", (int)base_len, str, new_suffix);
}

static void read_modules_order(const char *fname, const char *suffix)
{
	char line[PATH_MAX];
	FILE *in;

	in = fopen(fname, "r");
	if (!in)
		err(1, "Failed to open %s", fname);

	while (fgets(line, PATH_MAX, in)) {
		struct file_entry *entry;

		fh_list = xreallocarray(fh_list, num_files + 1, sizeof(*fh_list));
		entry = &fh_list[num_files];

		entry->pos = num_files;
		entry->name = xstrdup_replace_suffix(line, suffix);
		hash_file(entry);

		num_files++;
	}

	if (ferror(in))
		errx(1, "Failed to read %s", fname);

	fclose(in);
}

static __attribute__((noreturn))
void format(void)
{
	fprintf(stderr,
		"Usage: scripts/modules-merkle-tree <kmod suffix> <root definition>\n");
	exit(2);
}

int main(int argc, char *argv[])
{
	const EVP_MD *hash_evp;
	struct mtree *mt;

	if (argc != 3)
		format();

	hash_evp = EVP_get_digestbyname("sha256");
	ERR(!hash_evp, "EVP_get_digestbyname");

	ctx = EVP_MD_CTX_new();
	ERR(!ctx, "EVP_MD_CTX_new()");

	hash_size = EVP_MD_get_size(hash_evp);
	ERR(hash_size <= 0, "EVP_get_digestbyname");

	if (EVP_DigestInit_ex(ctx, hash_evp, NULL) != 1)
		ERR(1, "EVP_DigestInit_ex()");

	read_modules_order("modules.order", argv[2]);

	mt = build_merkle(fh_list, num_files);
	write_merkle_root(mt, argv[1]);
	for (unsigned int i = 0; i < num_files; i++) {
		char *signame;
		int fd;

		signame = xstrdup_replace_suffix(fh_list[i].name, ".merkle");

		fd = open(signame, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0)
			err(1, "Can't create %s", signame);

		build_proof(mt, i, fd);
		append_module_signature_magic(fd, lseek(fd, 0, SEEK_CUR));
		if (close(fd))
			err(1, "Can't write %s", signame);
	}

	free_mtree(mt);
	for (unsigned int i = 0; i < num_files; i++)
		free(fh_list[i].name);
	free(fh_list);

	EVP_MD_CTX_free(ctx);
	return 0;
}
