/* In-software asymmetric public-key crypto subtype
 *
 * See Documentation/crypto/asymmetric-keys.txt
 *
 * Copyright (C) 2012 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */

#define pr_fmt(fmt) "PKEY: "fmt
#include <linux/module.h>
#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
<<<<<<< HEAD
#include <keys/asymmetric-subtype.h>
#include "public_key.h"

MODULE_LICENSE("GPL");

const char *const pkey_algo_name[PKEY_ALGO__LAST] = {
	[PKEY_ALGO_DSA]		= "DSA",
	[PKEY_ALGO_RSA]		= "RSA",
};
EXPORT_SYMBOL_GPL(pkey_algo_name);

const struct public_key_algorithm *pkey_algo[PKEY_ALGO__LAST] = {
#if defined(CONFIG_PUBLIC_KEY_ALGO_RSA) || \
	defined(CONFIG_PUBLIC_KEY_ALGO_RSA_MODULE)
	[PKEY_ALGO_RSA]		= &RSA_public_key_algorithm,
#endif
};
EXPORT_SYMBOL_GPL(pkey_algo);

const char *const pkey_id_type_name[PKEY_ID_TYPE__LAST] = {
	[PKEY_ID_PGP]		= "PGP",
	[PKEY_ID_X509]		= "X509",
};
EXPORT_SYMBOL_GPL(pkey_id_type_name);

=======
#include <linux/scatterlist.h>
#include <keys/asymmetric-subtype.h>
#include <crypto/public_key.h>
#include <crypto/akcipher.h>

MODULE_LICENSE("GPL");

>>>>>>> v4.9.227
/*
 * Provide a part of a description of the key for /proc/keys.
 */
static void public_key_describe(const struct key *asymmetric_key,
				struct seq_file *m)
{
<<<<<<< HEAD
	struct public_key *key = asymmetric_key->payload.data;

	if (key)
		seq_printf(m, "%s.%s",
			   pkey_id_type_name[key->id_type], key->algo->name);
=======
	struct public_key *key = asymmetric_key->payload.data[asym_crypto];

	if (key)
		seq_printf(m, "%s.%s", key->id_type, key->pkey_algo);
>>>>>>> v4.9.227
}

/*
 * Destroy a public key algorithm key.
 */
<<<<<<< HEAD
void public_key_destroy(void *payload)
{
	struct public_key *key = payload;
	int i;

	if (key) {
		for (i = 0; i < ARRAY_SIZE(key->mpi); i++)
			mpi_free(key->mpi[i]);
		kfree(key);
	}
}
EXPORT_SYMBOL_GPL(public_key_destroy);
=======
void public_key_free(struct public_key *key)
{
	if (key) {
		kfree(key->key);
		kfree(key);
	}
}
EXPORT_SYMBOL_GPL(public_key_free);

/*
 * Destroy a public key algorithm key.
 */
static void public_key_destroy(void *payload0, void *payload3)
{
	public_key_free(payload0);
	public_key_signature_free(payload3);
}

struct public_key_completion {
	struct completion completion;
	int err;
};

static void public_key_verify_done(struct crypto_async_request *req, int err)
{
	struct public_key_completion *compl = req->data;

	if (err == -EINPROGRESS)
		return;

	compl->err = err;
	complete(&compl->completion);
}
>>>>>>> v4.9.227

/*
 * Verify a signature using a public key.
 */
<<<<<<< HEAD
int public_key_verify_signature(const struct public_key *pk,
				const struct public_key_signature *sig)
{
	const struct public_key_algorithm *algo;

	BUG_ON(!pk);
	BUG_ON(!pk->mpi[0]);
	BUG_ON(!pk->mpi[1]);
	BUG_ON(!sig);
	BUG_ON(!sig->digest);
	BUG_ON(!sig->mpi[0]);

	algo = pk->algo;
	if (!algo) {
		if (pk->pkey_algo >= PKEY_ALGO__LAST)
			return -ENOPKG;
		algo = pkey_algo[pk->pkey_algo];
		if (!algo)
			return -ENOPKG;
	}

	if (!algo->verify_signature)
		return -ENOTSUPP;

	if (sig->nr_mpi != algo->n_sig_mpi) {
		pr_debug("Signature has %u MPI not %u\n",
			 sig->nr_mpi, algo->n_sig_mpi);
		return -EINVAL;
	}

	return algo->verify_signature(pk, sig);
=======
int public_key_verify_signature(const struct public_key *pkey,
				const struct public_key_signature *sig)
{
	struct public_key_completion compl;
	struct crypto_akcipher *tfm;
	struct akcipher_request *req;
	struct scatterlist sig_sg, digest_sg;
	const char *alg_name;
	char alg_name_buf[CRYPTO_MAX_ALG_NAME];
	void *output;
	unsigned int outlen;
	int ret = -ENOMEM;

	pr_devel("==>%s()\n", __func__);

	BUG_ON(!pkey);
	BUG_ON(!sig);
	BUG_ON(!sig->s);

	if (!sig->digest)
		return -ENOPKG;

	alg_name = sig->pkey_algo;
	if (strcmp(sig->pkey_algo, "rsa") == 0) {
		/* The data wangled by the RSA algorithm is typically padded
		 * and encoded in some manner, such as EMSA-PKCS1-1_5 [RFC3447
		 * sec 8.2].
		 */
		if (snprintf(alg_name_buf, CRYPTO_MAX_ALG_NAME,
			     "pkcs1pad(rsa,%s)", sig->hash_algo
			     ) >= CRYPTO_MAX_ALG_NAME)
			return -EINVAL;
		alg_name = alg_name_buf;
	}

	tfm = crypto_alloc_akcipher(alg_name, 0, 0);
	if (IS_ERR(tfm))
		return PTR_ERR(tfm);

	req = akcipher_request_alloc(tfm, GFP_KERNEL);
	if (!req)
		goto error_free_tfm;

	ret = crypto_akcipher_set_pub_key(tfm, pkey->key, pkey->keylen);
	if (ret)
		goto error_free_req;

	outlen = crypto_akcipher_maxsize(tfm);
	output = kmalloc(outlen, GFP_KERNEL);
	if (!output)
		goto error_free_req;

	sg_init_one(&sig_sg, sig->s, sig->s_size);
	sg_init_one(&digest_sg, output, outlen);
	akcipher_request_set_crypt(req, &sig_sg, &digest_sg, sig->s_size,
				   outlen);
	init_completion(&compl.completion);
	akcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG |
				      CRYPTO_TFM_REQ_MAY_SLEEP,
				      public_key_verify_done, &compl);

	/* Perform the verification calculation.  This doesn't actually do the
	 * verification, but rather calculates the hash expected by the
	 * signature and returns that to us.
	 */
	ret = crypto_akcipher_verify(req);
	if ((ret == -EINPROGRESS) || (ret == -EBUSY)) {
		wait_for_completion(&compl.completion);
		ret = compl.err;
	}
	if (ret < 0)
		goto out_free_output;

	/* Do the actual verification step. */
	if (req->dst_len != sig->digest_size ||
	    memcmp(sig->digest, output, sig->digest_size) != 0)
		ret = -EKEYREJECTED;

out_free_output:
	kfree(output);
error_free_req:
	akcipher_request_free(req);
error_free_tfm:
	crypto_free_akcipher(tfm);
	pr_devel("<==%s() = %d\n", __func__, ret);
	return ret;
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(public_key_verify_signature);

static int public_key_verify_signature_2(const struct key *key,
					 const struct public_key_signature *sig)
{
<<<<<<< HEAD
	const struct public_key *pk = key->payload.data;
=======
	const struct public_key *pk = key->payload.data[asym_crypto];
>>>>>>> v4.9.227
	return public_key_verify_signature(pk, sig);
}

/*
 * Public key algorithm asymmetric key subtype
 */
struct asymmetric_key_subtype public_key_subtype = {
	.owner			= THIS_MODULE,
	.name			= "public_key",
	.name_len		= sizeof("public_key") - 1,
	.describe		= public_key_describe,
	.destroy		= public_key_destroy,
	.verify_signature	= public_key_verify_signature_2,
};
EXPORT_SYMBOL_GPL(public_key_subtype);
