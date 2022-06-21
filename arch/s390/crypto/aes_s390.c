/*
 * Cryptographic API.
 *
 * s390 implementation of the AES Cipher Algorithm.
 *
 * s390 Version:
 *   Copyright IBM Corp. 2005, 2007
 *   Author(s): Jan Glauber (jang@de.ibm.com)
 *		Sebastian Siewior (sebastian@breakpoint.cc> SW-Fallback
 *
 * Derived from "crypto/aes_generic.c"
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 */

#define KMSG_COMPONENT "aes_s390"
#define pr_fmt(fmt) KMSG_COMPONENT ": " fmt

#include <crypto/aes.h>
#include <crypto/algapi.h>
<<<<<<< HEAD
#include <linux/err.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include "crypt_s390.h"

#define AES_KEYLEN_128		1
#define AES_KEYLEN_192		2
#define AES_KEYLEN_256		4

static u8 *ctrblk;
static DEFINE_SPINLOCK(ctrblk_lock);
static char keylen_flag;

struct s390_aes_ctx {
	u8 key[AES_MAX_KEY_SIZE];
	long enc;
	long dec;
	int key_len;
	union {
		struct crypto_blkcipher *blk;
=======
#include <crypto/internal/skcipher.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/cpufeature.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/fips.h>
#include <crypto/xts.h>
#include <asm/cpacf.h>

static u8 *ctrblk;
static DEFINE_SPINLOCK(ctrblk_lock);

static cpacf_mask_t km_functions, kmc_functions, kmctr_functions;

struct s390_aes_ctx {
	u8 key[AES_MAX_KEY_SIZE];
	int key_len;
	unsigned long fc;
	union {
		struct crypto_skcipher *blk;
>>>>>>> v4.9.227
		struct crypto_cipher *cip;
	} fallback;
};

<<<<<<< HEAD
struct pcc_param {
	u8 key[32];
	u8 tweak[16];
	u8 block[16];
	u8 bit[16];
	u8 xts[16];
};

struct s390_xts_ctx {
	u8 key[32];
	u8 pcc_key[32];
	long enc;
	long dec;
	int key_len;
	struct crypto_blkcipher *fallback;
};

/*
 * Check if the key_len is supported by the HW.
 * Returns 0 if it is, a positive number if it is not and software fallback is
 * required or a negative number in case the key size is not valid
 */
static int need_fallback(unsigned int key_len)
{
	switch (key_len) {
	case 16:
		if (!(keylen_flag & AES_KEYLEN_128))
			return 1;
		break;
	case 24:
		if (!(keylen_flag & AES_KEYLEN_192))
			return 1;
		break;
	case 32:
		if (!(keylen_flag & AES_KEYLEN_256))
			return 1;
		break;
	default:
		return -1;
		break;
	}
	return 0;
}

=======
struct s390_xts_ctx {
	u8 key[32];
	u8 pcc_key[32];
	int key_len;
	unsigned long fc;
	struct crypto_skcipher *fallback;
};

>>>>>>> v4.9.227
static int setkey_fallback_cip(struct crypto_tfm *tfm, const u8 *in_key,
		unsigned int key_len)
{
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);
	int ret;

	sctx->fallback.cip->base.crt_flags &= ~CRYPTO_TFM_REQ_MASK;
	sctx->fallback.cip->base.crt_flags |= (tfm->crt_flags &
			CRYPTO_TFM_REQ_MASK);

	ret = crypto_cipher_setkey(sctx->fallback.cip, in_key, key_len);
	if (ret) {
		tfm->crt_flags &= ~CRYPTO_TFM_RES_MASK;
		tfm->crt_flags |= (sctx->fallback.cip->base.crt_flags &
				CRYPTO_TFM_RES_MASK);
	}
	return ret;
}

static int aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
		       unsigned int key_len)
{
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);
<<<<<<< HEAD
	u32 *flags = &tfm->crt_flags;
	int ret;

	ret = need_fallback(key_len);
	if (ret < 0) {
		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	sctx->key_len = key_len;
	if (!ret) {
		memcpy(sctx->key, in_key, key_len);
		return 0;
	}

	return setkey_fallback_cip(tfm, in_key, key_len);
=======
	unsigned long fc;

	/* Pick the correct function code based on the key length */
	fc = (key_len == 16) ? CPACF_KM_AES_128 :
	     (key_len == 24) ? CPACF_KM_AES_192 :
	     (key_len == 32) ? CPACF_KM_AES_256 : 0;

	/* Check if the function code is available */
	sctx->fc = (fc && cpacf_test_func(&km_functions, fc)) ? fc : 0;
	if (!sctx->fc)
		return setkey_fallback_cip(tfm, in_key, key_len);

	sctx->key_len = key_len;
	memcpy(sctx->key, in_key, key_len);
	return 0;
>>>>>>> v4.9.227
}

static void aes_encrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
<<<<<<< HEAD
	const struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);

	if (unlikely(need_fallback(sctx->key_len))) {
		crypto_cipher_encrypt_one(sctx->fallback.cip, out, in);
		return;
	}

	switch (sctx->key_len) {
	case 16:
		crypt_s390_km(KM_AES_128_ENCRYPT, &sctx->key, out, in,
			      AES_BLOCK_SIZE);
		break;
	case 24:
		crypt_s390_km(KM_AES_192_ENCRYPT, &sctx->key, out, in,
			      AES_BLOCK_SIZE);
		break;
	case 32:
		crypt_s390_km(KM_AES_256_ENCRYPT, &sctx->key, out, in,
			      AES_BLOCK_SIZE);
		break;
	}
=======
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);

	if (unlikely(!sctx->fc)) {
		crypto_cipher_encrypt_one(sctx->fallback.cip, out, in);
		return;
	}
	cpacf_km(sctx->fc, &sctx->key, out, in, AES_BLOCK_SIZE);
>>>>>>> v4.9.227
}

static void aes_decrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
<<<<<<< HEAD
	const struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);

	if (unlikely(need_fallback(sctx->key_len))) {
		crypto_cipher_decrypt_one(sctx->fallback.cip, out, in);
		return;
	}

	switch (sctx->key_len) {
	case 16:
		crypt_s390_km(KM_AES_128_DECRYPT, &sctx->key, out, in,
			      AES_BLOCK_SIZE);
		break;
	case 24:
		crypt_s390_km(KM_AES_192_DECRYPT, &sctx->key, out, in,
			      AES_BLOCK_SIZE);
		break;
	case 32:
		crypt_s390_km(KM_AES_256_DECRYPT, &sctx->key, out, in,
			      AES_BLOCK_SIZE);
		break;
	}
=======
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);

	if (unlikely(!sctx->fc)) {
		crypto_cipher_decrypt_one(sctx->fallback.cip, out, in);
		return;
	}
	cpacf_km(sctx->fc | CPACF_DECRYPT,
		 &sctx->key, out, in, AES_BLOCK_SIZE);
>>>>>>> v4.9.227
}

static int fallback_init_cip(struct crypto_tfm *tfm)
{
	const char *name = tfm->__crt_alg->cra_name;
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);

	sctx->fallback.cip = crypto_alloc_cipher(name, 0,
			CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK);

	if (IS_ERR(sctx->fallback.cip)) {
		pr_err("Allocating AES fallback algorithm %s failed\n",
		       name);
		return PTR_ERR(sctx->fallback.cip);
	}

	return 0;
}

static void fallback_exit_cip(struct crypto_tfm *tfm)
{
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);

	crypto_free_cipher(sctx->fallback.cip);
	sctx->fallback.cip = NULL;
}

static struct crypto_alg aes_alg = {
	.cra_name		=	"aes",
	.cra_driver_name	=	"aes-s390",
<<<<<<< HEAD
	.cra_priority		=	CRYPT_S390_PRIORITY,
=======
	.cra_priority		=	300,
>>>>>>> v4.9.227
	.cra_flags		=	CRYPTO_ALG_TYPE_CIPHER |
					CRYPTO_ALG_NEED_FALLBACK,
	.cra_blocksize		=	AES_BLOCK_SIZE,
	.cra_ctxsize		=	sizeof(struct s390_aes_ctx),
	.cra_module		=	THIS_MODULE,
	.cra_init               =       fallback_init_cip,
	.cra_exit               =       fallback_exit_cip,
	.cra_u			=	{
		.cipher = {
			.cia_min_keysize	=	AES_MIN_KEY_SIZE,
			.cia_max_keysize	=	AES_MAX_KEY_SIZE,
			.cia_setkey		=	aes_set_key,
			.cia_encrypt		=	aes_encrypt,
			.cia_decrypt		=	aes_decrypt,
		}
	}
};

static int setkey_fallback_blk(struct crypto_tfm *tfm, const u8 *key,
		unsigned int len)
{
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);
	unsigned int ret;

<<<<<<< HEAD
	sctx->fallback.blk->base.crt_flags &= ~CRYPTO_TFM_REQ_MASK;
	sctx->fallback.blk->base.crt_flags |= (tfm->crt_flags &
			CRYPTO_TFM_REQ_MASK);

	ret = crypto_blkcipher_setkey(sctx->fallback.blk, key, len);
	if (ret) {
		tfm->crt_flags &= ~CRYPTO_TFM_RES_MASK;
		tfm->crt_flags |= (sctx->fallback.blk->base.crt_flags &
				CRYPTO_TFM_RES_MASK);
	}
=======
	crypto_skcipher_clear_flags(sctx->fallback.blk, CRYPTO_TFM_REQ_MASK);
	crypto_skcipher_set_flags(sctx->fallback.blk, tfm->crt_flags &
						      CRYPTO_TFM_REQ_MASK);

	ret = crypto_skcipher_setkey(sctx->fallback.blk, key, len);

	tfm->crt_flags &= ~CRYPTO_TFM_RES_MASK;
	tfm->crt_flags |= crypto_skcipher_get_flags(sctx->fallback.blk) &
			  CRYPTO_TFM_RES_MASK;

>>>>>>> v4.9.227
	return ret;
}

static int fallback_blk_dec(struct blkcipher_desc *desc,
		struct scatterlist *dst, struct scatterlist *src,
		unsigned int nbytes)
{
	unsigned int ret;
<<<<<<< HEAD
	struct crypto_blkcipher *tfm;
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);

	tfm = desc->tfm;
	desc->tfm = sctx->fallback.blk;

	ret = crypto_blkcipher_decrypt_iv(desc, dst, src, nbytes);

	desc->tfm = tfm;
=======
	struct crypto_blkcipher *tfm = desc->tfm;
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(tfm);
	SKCIPHER_REQUEST_ON_STACK(req, sctx->fallback.blk);

	skcipher_request_set_tfm(req, sctx->fallback.blk);
	skcipher_request_set_callback(req, desc->flags, NULL, NULL);
	skcipher_request_set_crypt(req, src, dst, nbytes, desc->info);

	ret = crypto_skcipher_decrypt(req);

	skcipher_request_zero(req);
>>>>>>> v4.9.227
	return ret;
}

static int fallback_blk_enc(struct blkcipher_desc *desc,
		struct scatterlist *dst, struct scatterlist *src,
		unsigned int nbytes)
{
	unsigned int ret;
<<<<<<< HEAD
	struct crypto_blkcipher *tfm;
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);

	tfm = desc->tfm;
	desc->tfm = sctx->fallback.blk;

	ret = crypto_blkcipher_encrypt_iv(desc, dst, src, nbytes);

	desc->tfm = tfm;
=======
	struct crypto_blkcipher *tfm = desc->tfm;
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(tfm);
	SKCIPHER_REQUEST_ON_STACK(req, sctx->fallback.blk);

	skcipher_request_set_tfm(req, sctx->fallback.blk);
	skcipher_request_set_callback(req, desc->flags, NULL, NULL);
	skcipher_request_set_crypt(req, src, dst, nbytes, desc->info);

	ret = crypto_skcipher_encrypt(req);
>>>>>>> v4.9.227
	return ret;
}

static int ecb_aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
			   unsigned int key_len)
{
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);
<<<<<<< HEAD
	int ret;

	ret = need_fallback(key_len);
	if (ret > 0) {
		sctx->key_len = key_len;
		return setkey_fallback_blk(tfm, in_key, key_len);
	}

	switch (key_len) {
	case 16:
		sctx->enc = KM_AES_128_ENCRYPT;
		sctx->dec = KM_AES_128_DECRYPT;
		break;
	case 24:
		sctx->enc = KM_AES_192_ENCRYPT;
		sctx->dec = KM_AES_192_DECRYPT;
		break;
	case 32:
		sctx->enc = KM_AES_256_ENCRYPT;
		sctx->dec = KM_AES_256_DECRYPT;
		break;
	}

	return aes_set_key(tfm, in_key, key_len);
}

static int ecb_aes_crypt(struct blkcipher_desc *desc, long func, void *param,
			 struct blkcipher_walk *walk)
{
	int ret = blkcipher_walk_virt(desc, walk);
	unsigned int nbytes;

	while ((nbytes = walk->nbytes)) {
		/* only use complete blocks */
		unsigned int n = nbytes & ~(AES_BLOCK_SIZE - 1);
		u8 *out = walk->dst.virt.addr;
		u8 *in = walk->src.virt.addr;

		ret = crypt_s390_km(func, param, out, in, n);
		if (ret < 0 || ret != n)
			return -EIO;

		nbytes &= AES_BLOCK_SIZE - 1;
		ret = blkcipher_walk_done(desc, walk, nbytes);
=======
	unsigned long fc;

	/* Pick the correct function code based on the key length */
	fc = (key_len == 16) ? CPACF_KM_AES_128 :
	     (key_len == 24) ? CPACF_KM_AES_192 :
	     (key_len == 32) ? CPACF_KM_AES_256 : 0;

	/* Check if the function code is available */
	sctx->fc = (fc && cpacf_test_func(&km_functions, fc)) ? fc : 0;
	if (!sctx->fc)
		return setkey_fallback_blk(tfm, in_key, key_len);

	sctx->key_len = key_len;
	memcpy(sctx->key, in_key, key_len);
	return 0;
}

static int ecb_aes_crypt(struct blkcipher_desc *desc, unsigned long modifier,
			 struct blkcipher_walk *walk)
{
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);
	unsigned int nbytes, n;
	int ret;

	ret = blkcipher_walk_virt(desc, walk);
	while ((nbytes = walk->nbytes) >= AES_BLOCK_SIZE) {
		/* only use complete blocks */
		n = nbytes & ~(AES_BLOCK_SIZE - 1);
		cpacf_km(sctx->fc | modifier, sctx->key,
			 walk->dst.virt.addr, walk->src.virt.addr, n);
		ret = blkcipher_walk_done(desc, walk, nbytes - n);
>>>>>>> v4.9.227
	}

	return ret;
}

static int ecb_aes_encrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;

<<<<<<< HEAD
	if (unlikely(need_fallback(sctx->key_len)))
		return fallback_blk_enc(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return ecb_aes_crypt(desc, sctx->enc, sctx->key, &walk);
=======
	if (unlikely(!sctx->fc))
		return fallback_blk_enc(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return ecb_aes_crypt(desc, 0, &walk);
>>>>>>> v4.9.227
}

static int ecb_aes_decrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;

<<<<<<< HEAD
	if (unlikely(need_fallback(sctx->key_len)))
		return fallback_blk_dec(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return ecb_aes_crypt(desc, sctx->dec, sctx->key, &walk);
=======
	if (unlikely(!sctx->fc))
		return fallback_blk_dec(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return ecb_aes_crypt(desc, CPACF_DECRYPT, &walk);
>>>>>>> v4.9.227
}

static int fallback_init_blk(struct crypto_tfm *tfm)
{
	const char *name = tfm->__crt_alg->cra_name;
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);

<<<<<<< HEAD
	sctx->fallback.blk = crypto_alloc_blkcipher(name, 0,
			CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK);
=======
	sctx->fallback.blk = crypto_alloc_skcipher(name, 0,
						   CRYPTO_ALG_ASYNC |
						   CRYPTO_ALG_NEED_FALLBACK);
>>>>>>> v4.9.227

	if (IS_ERR(sctx->fallback.blk)) {
		pr_err("Allocating AES fallback algorithm %s failed\n",
		       name);
		return PTR_ERR(sctx->fallback.blk);
	}

	return 0;
}

static void fallback_exit_blk(struct crypto_tfm *tfm)
{
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);

<<<<<<< HEAD
	crypto_free_blkcipher(sctx->fallback.blk);
	sctx->fallback.blk = NULL;
=======
	crypto_free_skcipher(sctx->fallback.blk);
>>>>>>> v4.9.227
}

static struct crypto_alg ecb_aes_alg = {
	.cra_name		=	"ecb(aes)",
	.cra_driver_name	=	"ecb-aes-s390",
<<<<<<< HEAD
	.cra_priority		=	CRYPT_S390_COMPOSITE_PRIORITY,
=======
	.cra_priority		=	400,	/* combo: aes + ecb */
>>>>>>> v4.9.227
	.cra_flags		=	CRYPTO_ALG_TYPE_BLKCIPHER |
					CRYPTO_ALG_NEED_FALLBACK,
	.cra_blocksize		=	AES_BLOCK_SIZE,
	.cra_ctxsize		=	sizeof(struct s390_aes_ctx),
	.cra_type		=	&crypto_blkcipher_type,
	.cra_module		=	THIS_MODULE,
	.cra_init		=	fallback_init_blk,
	.cra_exit		=	fallback_exit_blk,
	.cra_u			=	{
		.blkcipher = {
			.min_keysize		=	AES_MIN_KEY_SIZE,
			.max_keysize		=	AES_MAX_KEY_SIZE,
			.setkey			=	ecb_aes_set_key,
			.encrypt		=	ecb_aes_encrypt,
			.decrypt		=	ecb_aes_decrypt,
		}
	}
};

static int cbc_aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
			   unsigned int key_len)
{
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);
<<<<<<< HEAD
	int ret;

	ret = need_fallback(key_len);
	if (ret > 0) {
		sctx->key_len = key_len;
		return setkey_fallback_blk(tfm, in_key, key_len);
	}

	switch (key_len) {
	case 16:
		sctx->enc = KMC_AES_128_ENCRYPT;
		sctx->dec = KMC_AES_128_DECRYPT;
		break;
	case 24:
		sctx->enc = KMC_AES_192_ENCRYPT;
		sctx->dec = KMC_AES_192_DECRYPT;
		break;
	case 32:
		sctx->enc = KMC_AES_256_ENCRYPT;
		sctx->dec = KMC_AES_256_DECRYPT;
		break;
	}

	return aes_set_key(tfm, in_key, key_len);
}

static int cbc_aes_crypt(struct blkcipher_desc *desc, long func,
			 struct blkcipher_walk *walk)
{
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);
	int ret = blkcipher_walk_virt(desc, walk);
	unsigned int nbytes = walk->nbytes;
=======
	unsigned long fc;

	/* Pick the correct function code based on the key length */
	fc = (key_len == 16) ? CPACF_KMC_AES_128 :
	     (key_len == 24) ? CPACF_KMC_AES_192 :
	     (key_len == 32) ? CPACF_KMC_AES_256 : 0;

	/* Check if the function code is available */
	sctx->fc = (fc && cpacf_test_func(&kmc_functions, fc)) ? fc : 0;
	if (!sctx->fc)
		return setkey_fallback_blk(tfm, in_key, key_len);

	sctx->key_len = key_len;
	memcpy(sctx->key, in_key, key_len);
	return 0;
}

static int cbc_aes_crypt(struct blkcipher_desc *desc, unsigned long modifier,
			 struct blkcipher_walk *walk)
{
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);
	unsigned int nbytes, n;
	int ret;
>>>>>>> v4.9.227
	struct {
		u8 iv[AES_BLOCK_SIZE];
		u8 key[AES_MAX_KEY_SIZE];
	} param;

<<<<<<< HEAD
	if (!nbytes)
		goto out;

	memcpy(param.iv, walk->iv, AES_BLOCK_SIZE);
	memcpy(param.key, sctx->key, sctx->key_len);
	do {
		/* only use complete blocks */
		unsigned int n = nbytes & ~(AES_BLOCK_SIZE - 1);
		u8 *out = walk->dst.virt.addr;
		u8 *in = walk->src.virt.addr;

		ret = crypt_s390_kmc(func, &param, out, in, n);
		if (ret < 0 || ret != n)
			return -EIO;

		nbytes &= AES_BLOCK_SIZE - 1;
		ret = blkcipher_walk_done(desc, walk, nbytes);
	} while ((nbytes = walk->nbytes));
	memcpy(walk->iv, param.iv, AES_BLOCK_SIZE);

out:
=======
	ret = blkcipher_walk_virt(desc, walk);
	memcpy(param.iv, walk->iv, AES_BLOCK_SIZE);
	memcpy(param.key, sctx->key, sctx->key_len);
	while ((nbytes = walk->nbytes) >= AES_BLOCK_SIZE) {
		/* only use complete blocks */
		n = nbytes & ~(AES_BLOCK_SIZE - 1);
		cpacf_kmc(sctx->fc | modifier, &param,
			  walk->dst.virt.addr, walk->src.virt.addr, n);
		ret = blkcipher_walk_done(desc, walk, nbytes - n);
	}
	memcpy(walk->iv, param.iv, AES_BLOCK_SIZE);
>>>>>>> v4.9.227
	return ret;
}

static int cbc_aes_encrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;

<<<<<<< HEAD
	if (unlikely(need_fallback(sctx->key_len)))
		return fallback_blk_enc(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return cbc_aes_crypt(desc, sctx->enc, &walk);
=======
	if (unlikely(!sctx->fc))
		return fallback_blk_enc(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return cbc_aes_crypt(desc, 0, &walk);
>>>>>>> v4.9.227
}

static int cbc_aes_decrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;

<<<<<<< HEAD
	if (unlikely(need_fallback(sctx->key_len)))
		return fallback_blk_dec(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return cbc_aes_crypt(desc, sctx->dec, &walk);
=======
	if (unlikely(!sctx->fc))
		return fallback_blk_dec(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return cbc_aes_crypt(desc, CPACF_DECRYPT, &walk);
>>>>>>> v4.9.227
}

static struct crypto_alg cbc_aes_alg = {
	.cra_name		=	"cbc(aes)",
	.cra_driver_name	=	"cbc-aes-s390",
<<<<<<< HEAD
	.cra_priority		=	CRYPT_S390_COMPOSITE_PRIORITY,
=======
	.cra_priority		=	400,	/* combo: aes + cbc */
>>>>>>> v4.9.227
	.cra_flags		=	CRYPTO_ALG_TYPE_BLKCIPHER |
					CRYPTO_ALG_NEED_FALLBACK,
	.cra_blocksize		=	AES_BLOCK_SIZE,
	.cra_ctxsize		=	sizeof(struct s390_aes_ctx),
	.cra_type		=	&crypto_blkcipher_type,
	.cra_module		=	THIS_MODULE,
	.cra_init		=	fallback_init_blk,
	.cra_exit		=	fallback_exit_blk,
	.cra_u			=	{
		.blkcipher = {
			.min_keysize		=	AES_MIN_KEY_SIZE,
			.max_keysize		=	AES_MAX_KEY_SIZE,
			.ivsize			=	AES_BLOCK_SIZE,
			.setkey			=	cbc_aes_set_key,
			.encrypt		=	cbc_aes_encrypt,
			.decrypt		=	cbc_aes_decrypt,
		}
	}
};

static int xts_fallback_setkey(struct crypto_tfm *tfm, const u8 *key,
				   unsigned int len)
{
	struct s390_xts_ctx *xts_ctx = crypto_tfm_ctx(tfm);
	unsigned int ret;

<<<<<<< HEAD
	xts_ctx->fallback->base.crt_flags &= ~CRYPTO_TFM_REQ_MASK;
	xts_ctx->fallback->base.crt_flags |= (tfm->crt_flags &
			CRYPTO_TFM_REQ_MASK);

	ret = crypto_blkcipher_setkey(xts_ctx->fallback, key, len);
	if (ret) {
		tfm->crt_flags &= ~CRYPTO_TFM_RES_MASK;
		tfm->crt_flags |= (xts_ctx->fallback->base.crt_flags &
				CRYPTO_TFM_RES_MASK);
	}
=======
	crypto_skcipher_clear_flags(xts_ctx->fallback, CRYPTO_TFM_REQ_MASK);
	crypto_skcipher_set_flags(xts_ctx->fallback, tfm->crt_flags &
						     CRYPTO_TFM_REQ_MASK);

	ret = crypto_skcipher_setkey(xts_ctx->fallback, key, len);

	tfm->crt_flags &= ~CRYPTO_TFM_RES_MASK;
	tfm->crt_flags |= crypto_skcipher_get_flags(xts_ctx->fallback) &
			  CRYPTO_TFM_RES_MASK;

>>>>>>> v4.9.227
	return ret;
}

static int xts_fallback_decrypt(struct blkcipher_desc *desc,
		struct scatterlist *dst, struct scatterlist *src,
		unsigned int nbytes)
{
<<<<<<< HEAD
	struct s390_xts_ctx *xts_ctx = crypto_blkcipher_ctx(desc->tfm);
	struct crypto_blkcipher *tfm;
	unsigned int ret;

	tfm = desc->tfm;
	desc->tfm = xts_ctx->fallback;

	ret = crypto_blkcipher_decrypt_iv(desc, dst, src, nbytes);

	desc->tfm = tfm;
=======
	struct crypto_blkcipher *tfm = desc->tfm;
	struct s390_xts_ctx *xts_ctx = crypto_blkcipher_ctx(tfm);
	SKCIPHER_REQUEST_ON_STACK(req, xts_ctx->fallback);
	unsigned int ret;

	skcipher_request_set_tfm(req, xts_ctx->fallback);
	skcipher_request_set_callback(req, desc->flags, NULL, NULL);
	skcipher_request_set_crypt(req, src, dst, nbytes, desc->info);

	ret = crypto_skcipher_decrypt(req);

	skcipher_request_zero(req);
>>>>>>> v4.9.227
	return ret;
}

static int xts_fallback_encrypt(struct blkcipher_desc *desc,
		struct scatterlist *dst, struct scatterlist *src,
		unsigned int nbytes)
{
<<<<<<< HEAD
	struct s390_xts_ctx *xts_ctx = crypto_blkcipher_ctx(desc->tfm);
	struct crypto_blkcipher *tfm;
	unsigned int ret;

	tfm = desc->tfm;
	desc->tfm = xts_ctx->fallback;

	ret = crypto_blkcipher_encrypt_iv(desc, dst, src, nbytes);

	desc->tfm = tfm;
=======
	struct crypto_blkcipher *tfm = desc->tfm;
	struct s390_xts_ctx *xts_ctx = crypto_blkcipher_ctx(tfm);
	SKCIPHER_REQUEST_ON_STACK(req, xts_ctx->fallback);
	unsigned int ret;

	skcipher_request_set_tfm(req, xts_ctx->fallback);
	skcipher_request_set_callback(req, desc->flags, NULL, NULL);
	skcipher_request_set_crypt(req, src, dst, nbytes, desc->info);

	ret = crypto_skcipher_encrypt(req);

	skcipher_request_zero(req);
>>>>>>> v4.9.227
	return ret;
}

static int xts_aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
			   unsigned int key_len)
{
	struct s390_xts_ctx *xts_ctx = crypto_tfm_ctx(tfm);
<<<<<<< HEAD
	u32 *flags = &tfm->crt_flags;

	switch (key_len) {
	case 32:
		xts_ctx->enc = KM_XTS_128_ENCRYPT;
		xts_ctx->dec = KM_XTS_128_DECRYPT;
		memcpy(xts_ctx->key + 16, in_key, 16);
		memcpy(xts_ctx->pcc_key + 16, in_key + 16, 16);
		break;
	case 48:
		xts_ctx->enc = 0;
		xts_ctx->dec = 0;
		xts_fallback_setkey(tfm, in_key, key_len);
		break;
	case 64:
		xts_ctx->enc = KM_XTS_256_ENCRYPT;
		xts_ctx->dec = KM_XTS_256_DECRYPT;
		memcpy(xts_ctx->key, in_key, 32);
		memcpy(xts_ctx->pcc_key, in_key + 32, 32);
		break;
	default:
		*flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}
	xts_ctx->key_len = key_len;
	return 0;
}

static int xts_aes_crypt(struct blkcipher_desc *desc, long func,
			 struct s390_xts_ctx *xts_ctx,
			 struct blkcipher_walk *walk)
{
	unsigned int offset = (xts_ctx->key_len >> 1) & 0x10;
	int ret = blkcipher_walk_virt(desc, walk);
	unsigned int nbytes = walk->nbytes;
	unsigned int n;
	u8 *in, *out;
	struct pcc_param pcc_param;
=======
	unsigned long fc;
	int err;

	err = xts_check_key(tfm, in_key, key_len);
	if (err)
		return err;

	/* In fips mode only 128 bit or 256 bit keys are valid */
	if (fips_enabled && key_len != 32 && key_len != 64) {
		tfm->crt_flags |= CRYPTO_TFM_RES_BAD_KEY_LEN;
		return -EINVAL;
	}

	/* Pick the correct function code based on the key length */
	fc = (key_len == 32) ? CPACF_KM_XTS_128 :
	     (key_len == 64) ? CPACF_KM_XTS_256 : 0;

	/* Check if the function code is available */
	xts_ctx->fc = (fc && cpacf_test_func(&km_functions, fc)) ? fc : 0;
	if (!xts_ctx->fc)
		return xts_fallback_setkey(tfm, in_key, key_len);

	/* Split the XTS key into the two subkeys */
	key_len = key_len / 2;
	xts_ctx->key_len = key_len;
	memcpy(xts_ctx->key, in_key, key_len);
	memcpy(xts_ctx->pcc_key, in_key + key_len, key_len);
	return 0;
}

static int xts_aes_crypt(struct blkcipher_desc *desc, unsigned long modifier,
			 struct blkcipher_walk *walk)
{
	struct s390_xts_ctx *xts_ctx = crypto_blkcipher_ctx(desc->tfm);
	unsigned int offset, nbytes, n;
	int ret;
	struct {
		u8 key[32];
		u8 tweak[16];
		u8 block[16];
		u8 bit[16];
		u8 xts[16];
	} pcc_param;
>>>>>>> v4.9.227
	struct {
		u8 key[32];
		u8 init[16];
	} xts_param;

<<<<<<< HEAD
	if (!nbytes)
		goto out;

=======
	ret = blkcipher_walk_virt(desc, walk);
	offset = xts_ctx->key_len & 0x10;
>>>>>>> v4.9.227
	memset(pcc_param.block, 0, sizeof(pcc_param.block));
	memset(pcc_param.bit, 0, sizeof(pcc_param.bit));
	memset(pcc_param.xts, 0, sizeof(pcc_param.xts));
	memcpy(pcc_param.tweak, walk->iv, sizeof(pcc_param.tweak));
<<<<<<< HEAD
	memcpy(pcc_param.key, xts_ctx->pcc_key, 32);
	ret = crypt_s390_pcc(func, &pcc_param.key[offset]);
	if (ret < 0)
		return -EIO;

	memcpy(xts_param.key, xts_ctx->key, 32);
	memcpy(xts_param.init, pcc_param.xts, 16);
	do {
		/* only use complete blocks */
		n = nbytes & ~(AES_BLOCK_SIZE - 1);
		out = walk->dst.virt.addr;
		in = walk->src.virt.addr;

		ret = crypt_s390_km(func, &xts_param.key[offset], out, in, n);
		if (ret < 0 || ret != n)
			return -EIO;

		nbytes &= AES_BLOCK_SIZE - 1;
		ret = blkcipher_walk_done(desc, walk, nbytes);
	} while ((nbytes = walk->nbytes));
out:
=======
	memcpy(pcc_param.key + offset, xts_ctx->pcc_key, xts_ctx->key_len);
	cpacf_pcc(xts_ctx->fc, pcc_param.key + offset);

	memcpy(xts_param.key + offset, xts_ctx->key, xts_ctx->key_len);
	memcpy(xts_param.init, pcc_param.xts, 16);

	while ((nbytes = walk->nbytes) >= AES_BLOCK_SIZE) {
		/* only use complete blocks */
		n = nbytes & ~(AES_BLOCK_SIZE - 1);
		cpacf_km(xts_ctx->fc | modifier, xts_param.key + offset,
			 walk->dst.virt.addr, walk->src.virt.addr, n);
		ret = blkcipher_walk_done(desc, walk, nbytes - n);
	}
>>>>>>> v4.9.227
	return ret;
}

static int xts_aes_encrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	struct s390_xts_ctx *xts_ctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;

<<<<<<< HEAD
	if (unlikely(xts_ctx->key_len == 48))
		return xts_fallback_encrypt(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return xts_aes_crypt(desc, xts_ctx->enc, xts_ctx, &walk);
=======
	if (!nbytes)
		return -EINVAL;

	if (unlikely(!xts_ctx->fc))
		return xts_fallback_encrypt(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return xts_aes_crypt(desc, 0, &walk);
>>>>>>> v4.9.227
}

static int xts_aes_decrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	struct s390_xts_ctx *xts_ctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;

<<<<<<< HEAD
	if (unlikely(xts_ctx->key_len == 48))
		return xts_fallback_decrypt(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return xts_aes_crypt(desc, xts_ctx->dec, xts_ctx, &walk);
=======
	if (!nbytes)
		return -EINVAL;

	if (unlikely(!xts_ctx->fc))
		return xts_fallback_decrypt(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return xts_aes_crypt(desc, CPACF_DECRYPT, &walk);
>>>>>>> v4.9.227
}

static int xts_fallback_init(struct crypto_tfm *tfm)
{
	const char *name = tfm->__crt_alg->cra_name;
	struct s390_xts_ctx *xts_ctx = crypto_tfm_ctx(tfm);

<<<<<<< HEAD
	xts_ctx->fallback = crypto_alloc_blkcipher(name, 0,
			CRYPTO_ALG_ASYNC | CRYPTO_ALG_NEED_FALLBACK);
=======
	xts_ctx->fallback = crypto_alloc_skcipher(name, 0,
						  CRYPTO_ALG_ASYNC |
						  CRYPTO_ALG_NEED_FALLBACK);
>>>>>>> v4.9.227

	if (IS_ERR(xts_ctx->fallback)) {
		pr_err("Allocating XTS fallback algorithm %s failed\n",
		       name);
		return PTR_ERR(xts_ctx->fallback);
	}
	return 0;
}

static void xts_fallback_exit(struct crypto_tfm *tfm)
{
	struct s390_xts_ctx *xts_ctx = crypto_tfm_ctx(tfm);

<<<<<<< HEAD
	crypto_free_blkcipher(xts_ctx->fallback);
	xts_ctx->fallback = NULL;
=======
	crypto_free_skcipher(xts_ctx->fallback);
>>>>>>> v4.9.227
}

static struct crypto_alg xts_aes_alg = {
	.cra_name		=	"xts(aes)",
	.cra_driver_name	=	"xts-aes-s390",
<<<<<<< HEAD
	.cra_priority		=	CRYPT_S390_COMPOSITE_PRIORITY,
=======
	.cra_priority		=	400,	/* combo: aes + xts */
>>>>>>> v4.9.227
	.cra_flags		=	CRYPTO_ALG_TYPE_BLKCIPHER |
					CRYPTO_ALG_NEED_FALLBACK,
	.cra_blocksize		=	AES_BLOCK_SIZE,
	.cra_ctxsize		=	sizeof(struct s390_xts_ctx),
	.cra_type		=	&crypto_blkcipher_type,
	.cra_module		=	THIS_MODULE,
	.cra_init		=	xts_fallback_init,
	.cra_exit		=	xts_fallback_exit,
	.cra_u			=	{
		.blkcipher = {
			.min_keysize		=	2 * AES_MIN_KEY_SIZE,
			.max_keysize		=	2 * AES_MAX_KEY_SIZE,
			.ivsize			=	AES_BLOCK_SIZE,
			.setkey			=	xts_aes_set_key,
			.encrypt		=	xts_aes_encrypt,
			.decrypt		=	xts_aes_decrypt,
		}
	}
};

<<<<<<< HEAD
static int xts_aes_alg_reg;

=======
>>>>>>> v4.9.227
static int ctr_aes_set_key(struct crypto_tfm *tfm, const u8 *in_key,
			   unsigned int key_len)
{
	struct s390_aes_ctx *sctx = crypto_tfm_ctx(tfm);
<<<<<<< HEAD

	switch (key_len) {
	case 16:
		sctx->enc = KMCTR_AES_128_ENCRYPT;
		sctx->dec = KMCTR_AES_128_DECRYPT;
		break;
	case 24:
		sctx->enc = KMCTR_AES_192_ENCRYPT;
		sctx->dec = KMCTR_AES_192_DECRYPT;
		break;
	case 32:
		sctx->enc = KMCTR_AES_256_ENCRYPT;
		sctx->dec = KMCTR_AES_256_DECRYPT;
		break;
	}

	return aes_set_key(tfm, in_key, key_len);
}

static unsigned int __ctrblk_init(u8 *ctrptr, unsigned int nbytes)
=======
	unsigned long fc;

	/* Pick the correct function code based on the key length */
	fc = (key_len == 16) ? CPACF_KMCTR_AES_128 :
	     (key_len == 24) ? CPACF_KMCTR_AES_192 :
	     (key_len == 32) ? CPACF_KMCTR_AES_256 : 0;

	/* Check if the function code is available */
	sctx->fc = (fc && cpacf_test_func(&kmctr_functions, fc)) ? fc : 0;
	if (!sctx->fc)
		return setkey_fallback_blk(tfm, in_key, key_len);

	sctx->key_len = key_len;
	memcpy(sctx->key, in_key, key_len);
	return 0;
}

static unsigned int __ctrblk_init(u8 *ctrptr, u8 *iv, unsigned int nbytes)
>>>>>>> v4.9.227
{
	unsigned int i, n;

	/* only use complete blocks, max. PAGE_SIZE */
<<<<<<< HEAD
	n = (nbytes > PAGE_SIZE) ? PAGE_SIZE : nbytes & ~(AES_BLOCK_SIZE - 1);
	for (i = AES_BLOCK_SIZE; i < n; i += AES_BLOCK_SIZE) {
		memcpy(ctrptr + i, ctrptr + i - AES_BLOCK_SIZE,
		       AES_BLOCK_SIZE);
		crypto_inc(ctrptr + i, AES_BLOCK_SIZE);
=======
	memcpy(ctrptr, iv, AES_BLOCK_SIZE);
	n = (nbytes > PAGE_SIZE) ? PAGE_SIZE : nbytes & ~(AES_BLOCK_SIZE - 1);
	for (i = (n / AES_BLOCK_SIZE) - 1; i > 0; i--) {
		memcpy(ctrptr + AES_BLOCK_SIZE, ctrptr, AES_BLOCK_SIZE);
		crypto_inc(ctrptr + AES_BLOCK_SIZE, AES_BLOCK_SIZE);
		ctrptr += AES_BLOCK_SIZE;
>>>>>>> v4.9.227
	}
	return n;
}

<<<<<<< HEAD
static int ctr_aes_crypt(struct blkcipher_desc *desc, long func,
			 struct s390_aes_ctx *sctx, struct blkcipher_walk *walk)
{
	int ret = blkcipher_walk_virt_block(desc, walk, AES_BLOCK_SIZE);
	unsigned int n, nbytes;
	u8 buf[AES_BLOCK_SIZE], ctrbuf[AES_BLOCK_SIZE];
	u8 *out, *in, *ctrptr = ctrbuf;

	if (!walk->nbytes)
		return ret;

	if (spin_trylock(&ctrblk_lock))
		ctrptr = ctrblk;

	memcpy(ctrptr, walk->iv, AES_BLOCK_SIZE);
	while ((nbytes = walk->nbytes) >= AES_BLOCK_SIZE) {
		out = walk->dst.virt.addr;
		in = walk->src.virt.addr;
		while (nbytes >= AES_BLOCK_SIZE) {
			if (ctrptr == ctrblk)
				n = __ctrblk_init(ctrptr, nbytes);
			else
				n = AES_BLOCK_SIZE;
			ret = crypt_s390_kmctr(func, sctx->key, out, in,
					       n, ctrptr);
			if (ret < 0 || ret != n) {
				if (ctrptr == ctrblk)
					spin_unlock(&ctrblk_lock);
				return -EIO;
			}
			if (n > AES_BLOCK_SIZE)
				memcpy(ctrptr, ctrptr + n - AES_BLOCK_SIZE,
				       AES_BLOCK_SIZE);
			crypto_inc(ctrptr, AES_BLOCK_SIZE);
			out += n;
			in += n;
			nbytes -= n;
		}
		ret = blkcipher_walk_done(desc, walk, nbytes);
	}
	if (ctrptr == ctrblk) {
		if (nbytes)
			memcpy(ctrbuf, ctrptr, AES_BLOCK_SIZE);
		else
			memcpy(walk->iv, ctrptr, AES_BLOCK_SIZE);
		spin_unlock(&ctrblk_lock);
	} else {
		if (!nbytes)
			memcpy(walk->iv, ctrptr, AES_BLOCK_SIZE);
	}
=======
static int ctr_aes_crypt(struct blkcipher_desc *desc, unsigned long modifier,
			 struct blkcipher_walk *walk)
{
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);
	u8 buf[AES_BLOCK_SIZE], *ctrptr;
	unsigned int n, nbytes;
	int ret, locked;

	locked = spin_trylock(&ctrblk_lock);

	ret = blkcipher_walk_virt_block(desc, walk, AES_BLOCK_SIZE);
	while ((nbytes = walk->nbytes) >= AES_BLOCK_SIZE) {
		n = AES_BLOCK_SIZE;
		if (nbytes >= 2*AES_BLOCK_SIZE && locked)
			n = __ctrblk_init(ctrblk, walk->iv, nbytes);
		ctrptr = (n > AES_BLOCK_SIZE) ? ctrblk : walk->iv;
		cpacf_kmctr(sctx->fc | modifier, sctx->key,
			    walk->dst.virt.addr, walk->src.virt.addr,
			    n, ctrptr);
		if (ctrptr == ctrblk)
			memcpy(walk->iv, ctrptr + n - AES_BLOCK_SIZE,
			       AES_BLOCK_SIZE);
		crypto_inc(walk->iv, AES_BLOCK_SIZE);
		ret = blkcipher_walk_done(desc, walk, nbytes - n);
	}
	if (locked)
		spin_unlock(&ctrblk_lock);
>>>>>>> v4.9.227
	/*
	 * final block may be < AES_BLOCK_SIZE, copy only nbytes
	 */
	if (nbytes) {
<<<<<<< HEAD
		out = walk->dst.virt.addr;
		in = walk->src.virt.addr;
		ret = crypt_s390_kmctr(func, sctx->key, buf, in,
				       AES_BLOCK_SIZE, ctrbuf);
		if (ret < 0 || ret != AES_BLOCK_SIZE)
			return -EIO;
		memcpy(out, buf, nbytes);
		crypto_inc(ctrbuf, AES_BLOCK_SIZE);
		ret = blkcipher_walk_done(desc, walk, 0);
		memcpy(walk->iv, ctrbuf, AES_BLOCK_SIZE);
=======
		cpacf_kmctr(sctx->fc | modifier, sctx->key,
			    buf, walk->src.virt.addr,
			    AES_BLOCK_SIZE, walk->iv);
		memcpy(walk->dst.virt.addr, buf, nbytes);
		crypto_inc(walk->iv, AES_BLOCK_SIZE);
		ret = blkcipher_walk_done(desc, walk, 0);
>>>>>>> v4.9.227
	}

	return ret;
}

static int ctr_aes_encrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;

<<<<<<< HEAD
	blkcipher_walk_init(&walk, dst, src, nbytes);
	return ctr_aes_crypt(desc, sctx->enc, sctx, &walk);
=======
	if (unlikely(!sctx->fc))
		return fallback_blk_enc(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return ctr_aes_crypt(desc, 0, &walk);
>>>>>>> v4.9.227
}

static int ctr_aes_decrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	struct s390_aes_ctx *sctx = crypto_blkcipher_ctx(desc->tfm);
	struct blkcipher_walk walk;

<<<<<<< HEAD
	blkcipher_walk_init(&walk, dst, src, nbytes);
	return ctr_aes_crypt(desc, sctx->dec, sctx, &walk);
=======
	if (unlikely(!sctx->fc))
		return fallback_blk_dec(desc, dst, src, nbytes);

	blkcipher_walk_init(&walk, dst, src, nbytes);
	return ctr_aes_crypt(desc, CPACF_DECRYPT, &walk);
>>>>>>> v4.9.227
}

static struct crypto_alg ctr_aes_alg = {
	.cra_name		=	"ctr(aes)",
	.cra_driver_name	=	"ctr-aes-s390",
<<<<<<< HEAD
	.cra_priority		=	CRYPT_S390_COMPOSITE_PRIORITY,
	.cra_flags		=	CRYPTO_ALG_TYPE_BLKCIPHER,
=======
	.cra_priority		=	400,	/* combo: aes + ctr */
	.cra_flags		=	CRYPTO_ALG_TYPE_BLKCIPHER |
					CRYPTO_ALG_NEED_FALLBACK,
>>>>>>> v4.9.227
	.cra_blocksize		=	1,
	.cra_ctxsize		=	sizeof(struct s390_aes_ctx),
	.cra_type		=	&crypto_blkcipher_type,
	.cra_module		=	THIS_MODULE,
<<<<<<< HEAD
=======
	.cra_init		=	fallback_init_blk,
	.cra_exit		=	fallback_exit_blk,
>>>>>>> v4.9.227
	.cra_u			=	{
		.blkcipher = {
			.min_keysize		=	AES_MIN_KEY_SIZE,
			.max_keysize		=	AES_MAX_KEY_SIZE,
			.ivsize			=	AES_BLOCK_SIZE,
			.setkey			=	ctr_aes_set_key,
			.encrypt		=	ctr_aes_encrypt,
			.decrypt		=	ctr_aes_decrypt,
		}
	}
};

<<<<<<< HEAD
static int ctr_aes_alg_reg;
=======
static struct crypto_alg *aes_s390_algs_ptr[5];
static int aes_s390_algs_num;

static int aes_s390_register_alg(struct crypto_alg *alg)
{
	int ret;

	ret = crypto_register_alg(alg);
	if (!ret)
		aes_s390_algs_ptr[aes_s390_algs_num++] = alg;
	return ret;
}

static void aes_s390_fini(void)
{
	while (aes_s390_algs_num--)
		crypto_unregister_alg(aes_s390_algs_ptr[aes_s390_algs_num]);
	if (ctrblk)
		free_page((unsigned long) ctrblk);
}
>>>>>>> v4.9.227

static int __init aes_s390_init(void)
{
	int ret;

<<<<<<< HEAD
	if (crypt_s390_func_available(KM_AES_128_ENCRYPT, CRYPT_S390_MSA))
		keylen_flag |= AES_KEYLEN_128;
	if (crypt_s390_func_available(KM_AES_192_ENCRYPT, CRYPT_S390_MSA))
		keylen_flag |= AES_KEYLEN_192;
	if (crypt_s390_func_available(KM_AES_256_ENCRYPT, CRYPT_S390_MSA))
		keylen_flag |= AES_KEYLEN_256;

	if (!keylen_flag)
		return -EOPNOTSUPP;

	/* z9 109 and z9 BC/EC only support 128 bit key length */
	if (keylen_flag == AES_KEYLEN_128)
		pr_info("AES hardware acceleration is only available for"
			" 128-bit keys\n");

	ret = crypto_register_alg(&aes_alg);
	if (ret)
		goto aes_err;

	ret = crypto_register_alg(&ecb_aes_alg);
	if (ret)
		goto ecb_aes_err;

	ret = crypto_register_alg(&cbc_aes_alg);
	if (ret)
		goto cbc_aes_err;

	if (crypt_s390_func_available(KM_XTS_128_ENCRYPT,
			CRYPT_S390_MSA | CRYPT_S390_MSA4) &&
	    crypt_s390_func_available(KM_XTS_256_ENCRYPT,
			CRYPT_S390_MSA | CRYPT_S390_MSA4)) {
		ret = crypto_register_alg(&xts_aes_alg);
		if (ret)
			goto xts_aes_err;
		xts_aes_alg_reg = 1;
	}

	if (crypt_s390_func_available(KMCTR_AES_128_ENCRYPT,
				CRYPT_S390_MSA | CRYPT_S390_MSA4) &&
	    crypt_s390_func_available(KMCTR_AES_192_ENCRYPT,
				CRYPT_S390_MSA | CRYPT_S390_MSA4) &&
	    crypt_s390_func_available(KMCTR_AES_256_ENCRYPT,
				CRYPT_S390_MSA | CRYPT_S390_MSA4)) {
		ctrblk = (u8 *) __get_free_page(GFP_KERNEL);
		if (!ctrblk) {
			ret = -ENOMEM;
			goto ctr_aes_err;
		}
		ret = crypto_register_alg(&ctr_aes_alg);
		if (ret) {
			free_page((unsigned long) ctrblk);
			goto ctr_aes_err;
		}
		ctr_aes_alg_reg = 1;
	}

out:
	return ret;

ctr_aes_err:
	crypto_unregister_alg(&xts_aes_alg);
xts_aes_err:
	crypto_unregister_alg(&cbc_aes_alg);
cbc_aes_err:
	crypto_unregister_alg(&ecb_aes_alg);
ecb_aes_err:
	crypto_unregister_alg(&aes_alg);
aes_err:
	goto out;
}

static void __exit aes_s390_fini(void)
{
	if (ctr_aes_alg_reg) {
		crypto_unregister_alg(&ctr_aes_alg);
		free_page((unsigned long) ctrblk);
	}
	if (xts_aes_alg_reg)
		crypto_unregister_alg(&xts_aes_alg);
	crypto_unregister_alg(&cbc_aes_alg);
	crypto_unregister_alg(&ecb_aes_alg);
	crypto_unregister_alg(&aes_alg);
}

module_init(aes_s390_init);
=======
	/* Query available functions for KM, KMC and KMCTR */
	cpacf_query(CPACF_KM, &km_functions);
	cpacf_query(CPACF_KMC, &kmc_functions);
	cpacf_query(CPACF_KMCTR, &kmctr_functions);

	if (cpacf_test_func(&km_functions, CPACF_KM_AES_128) ||
	    cpacf_test_func(&km_functions, CPACF_KM_AES_192) ||
	    cpacf_test_func(&km_functions, CPACF_KM_AES_256)) {
		ret = aes_s390_register_alg(&aes_alg);
		if (ret)
			goto out_err;
		ret = aes_s390_register_alg(&ecb_aes_alg);
		if (ret)
			goto out_err;
	}

	if (cpacf_test_func(&kmc_functions, CPACF_KMC_AES_128) ||
	    cpacf_test_func(&kmc_functions, CPACF_KMC_AES_192) ||
	    cpacf_test_func(&kmc_functions, CPACF_KMC_AES_256)) {
		ret = aes_s390_register_alg(&cbc_aes_alg);
		if (ret)
			goto out_err;
	}

	if (cpacf_test_func(&km_functions, CPACF_KM_XTS_128) ||
	    cpacf_test_func(&km_functions, CPACF_KM_XTS_256)) {
		ret = aes_s390_register_alg(&xts_aes_alg);
		if (ret)
			goto out_err;
	}

	if (cpacf_test_func(&kmctr_functions, CPACF_KMCTR_AES_128) ||
	    cpacf_test_func(&kmctr_functions, CPACF_KMCTR_AES_192) ||
	    cpacf_test_func(&kmctr_functions, CPACF_KMCTR_AES_256)) {
		ctrblk = (u8 *) __get_free_page(GFP_KERNEL);
		if (!ctrblk) {
			ret = -ENOMEM;
			goto out_err;
		}
		ret = aes_s390_register_alg(&ctr_aes_alg);
		if (ret)
			goto out_err;
	}

	return 0;
out_err:
	aes_s390_fini();
	return ret;
}

module_cpu_feature_match(MSA, aes_s390_init);
>>>>>>> v4.9.227
module_exit(aes_s390_fini);

MODULE_ALIAS_CRYPTO("aes-all");

MODULE_DESCRIPTION("Rijndael (AES) Cipher Algorithm");
MODULE_LICENSE("GPL");
