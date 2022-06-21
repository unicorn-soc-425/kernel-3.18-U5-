/* PE Binary parser bits
 *
 * Copyright (C) 2014 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */

<<<<<<< HEAD
#include <linux/verify_pefile.h>
=======
>>>>>>> v4.9.227
#include <crypto/pkcs7.h>
#include <crypto/hash_info.h>

struct pefile_context {
	unsigned	header_size;
	unsigned	image_checksum_offset;
	unsigned	cert_dirent_offset;
	unsigned	n_data_dirents;
	unsigned	n_sections;
	unsigned	certs_size;
	unsigned	sig_offset;
	unsigned	sig_len;
	const struct section_header *secs;
<<<<<<< HEAD
	struct pkcs7_message *pkcs7;
=======
>>>>>>> v4.9.227

	/* PKCS#7 MS Individual Code Signing content */
	const void	*digest;		/* Digest */
	unsigned	digest_len;		/* Digest length */
<<<<<<< HEAD
	enum hash_algo	digest_algo;		/* Digest algorithm */
=======
	const char	*digest_algo;		/* Digest algorithm */
>>>>>>> v4.9.227
};

#define kenter(FMT, ...)					\
	pr_devel("==> %s("FMT")\n", __func__, ##__VA_ARGS__)
#define kleave(FMT, ...) \
	pr_devel("<== %s()"FMT"\n", __func__, ##__VA_ARGS__)

/*
 * mscode_parser.c
 */
<<<<<<< HEAD
extern int mscode_parse(struct pefile_context *ctx);
=======
extern int mscode_parse(void *_ctx, const void *content_data, size_t data_len,
			size_t asn1hdrlen);
>>>>>>> v4.9.227
