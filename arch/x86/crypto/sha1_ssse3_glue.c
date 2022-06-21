/*
 * Cryptographic API.
 *
 * Glue code for the SHA1 Secure Hash Algorithm assembler implementation using
 * Supplemental SSE3 instructions.
 *
 * This file is based on sha1_generic.c
 *
 * Copyright (c) Alan Smithee.
 * Copyright (c) Andrew McDonald <andrew@mcdonald.org.uk>
 * Copyright (c) Jean-Francois Dive <jef@linuxbe.org>
 * Copyright (c) Mathias Krause <minipli@googlemail.com>
 * Copyright (c) Chandramouli Narayanan <mouli@linux.intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 */

#define pr_fmt(fmt)	KBUILD_MODNAME ": " fmt

#include <crypto/internal/hash.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/cryptohash.h>
#include <linux/types.h>
#include <crypto/sha.h>
<<<<<<< HEAD
#include <asm/byteorder.h>
#include <asm/i387.h>
#include <asm/xcr.h>
#include <asm/xsave.h>


asmlinkage void sha1_transform_ssse3(u32 *digest, const char *data,
				     unsigned int rounds);
#ifdef CONFIG_AS_AVX
asmlinkage void sha1_transform_avx(u32 *digest, const char *data,
				   unsigned int rounds);
#endif
#ifdef CONFIG_AS_AVX2
#define SHA1_AVX2_BLOCK_OPTSIZE	4	/* optimal 4*64 bytes of SHA1 blocks */

asmlinkage void sha1_transform_avx2(u32 *digest, const char *data,
				unsigned int rounds);
#endif

static asmlinkage void (*sha1_transform_asm)(u32 *, const char *, unsigned int);


static int sha1_ssse3_init(struct shash_desc *desc)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);

	*sctx = (struct sha1_state){
		.state = { SHA1_H0, SHA1_H1, SHA1_H2, SHA1_H3, SHA1_H4 },
	};

	return 0;
}

static int __sha1_ssse3_update(struct shash_desc *desc, const u8 *data,
			       unsigned int len, unsigned int partial)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);
	unsigned int done = 0;

	sctx->count += len;

	if (partial) {
		done = SHA1_BLOCK_SIZE - partial;
		memcpy(sctx->buffer + partial, data, done);
		sha1_transform_asm(sctx->state, sctx->buffer, 1);
	}

	if (len - done >= SHA1_BLOCK_SIZE) {
		const unsigned int rounds = (len - done) / SHA1_BLOCK_SIZE;

		sha1_transform_asm(sctx->state, data + done, rounds);
		done += rounds * SHA1_BLOCK_SIZE;
	}

	memcpy(sctx->buffer, data + done, len - done);

	return 0;
}
=======
#include <crypto/sha1_base.h>
#include <asm/fpu/api.h>

typedef void (sha1_transform_fn)(u32 *digest, const char *data,
				unsigned int rounds);

static int sha1_update(struct shash_desc *desc, const u8 *data,
			     unsigned int len, sha1_transform_fn *sha1_xform)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);

	if (!irq_fpu_usable() ||
	    (sctx->count % SHA1_BLOCK_SIZE) + len < SHA1_BLOCK_SIZE)
		return crypto_sha1_update(desc, data, len);

	/* make sure casting to sha1_block_fn() is safe */
	BUILD_BUG_ON(offsetof(struct sha1_state, state) != 0);

	kernel_fpu_begin();
	sha1_base_do_update(desc, data, len,
			    (sha1_block_fn *)sha1_xform);
	kernel_fpu_end();

	return 0;
}

static int sha1_finup(struct shash_desc *desc, const u8 *data,
		      unsigned int len, u8 *out, sha1_transform_fn *sha1_xform)
{
	if (!irq_fpu_usable())
		return crypto_sha1_finup(desc, data, len, out);

	kernel_fpu_begin();
	if (len)
		sha1_base_do_update(desc, data, len,
				    (sha1_block_fn *)sha1_xform);
	sha1_base_do_finalize(desc, (sha1_block_fn *)sha1_xform);
	kernel_fpu_end();

	return sha1_base_finish(desc, out);
}

asmlinkage void sha1_transform_ssse3(u32 *digest, const char *data,
				     unsigned int rounds);
>>>>>>> v4.9.227

static int sha1_ssse3_update(struct shash_desc *desc, const u8 *data,
			     unsigned int len)
{
<<<<<<< HEAD
	struct sha1_state *sctx = shash_desc_ctx(desc);
	unsigned int partial = sctx->count % SHA1_BLOCK_SIZE;
	int res;

	/* Handle the fast case right here */
	if (partial + len < SHA1_BLOCK_SIZE) {
		sctx->count += len;
		memcpy(sctx->buffer + partial, data, len);

		return 0;
	}

	if (!irq_fpu_usable()) {
		res = crypto_sha1_update(desc, data, len);
	} else {
		kernel_fpu_begin();
		res = __sha1_ssse3_update(desc, data, len, partial);
		kernel_fpu_end();
	}

	return res;
}

=======
	return sha1_update(desc, data, len,
			(sha1_transform_fn *) sha1_transform_ssse3);
}

static int sha1_ssse3_finup(struct shash_desc *desc, const u8 *data,
			      unsigned int len, u8 *out)
{
	return sha1_finup(desc, data, len, out,
			(sha1_transform_fn *) sha1_transform_ssse3);
}
>>>>>>> v4.9.227

/* Add padding and return the message digest. */
static int sha1_ssse3_final(struct shash_desc *desc, u8 *out)
{
<<<<<<< HEAD
	struct sha1_state *sctx = shash_desc_ctx(desc);
	unsigned int i, index, padlen;
	__be32 *dst = (__be32 *)out;
	__be64 bits;
	static const u8 padding[SHA1_BLOCK_SIZE] = { 0x80, };

	bits = cpu_to_be64(sctx->count << 3);

	/* Pad out to 56 mod 64 and append length */
	index = sctx->count % SHA1_BLOCK_SIZE;
	padlen = (index < 56) ? (56 - index) : ((SHA1_BLOCK_SIZE+56) - index);
	if (!irq_fpu_usable()) {
		crypto_sha1_update(desc, padding, padlen);
		crypto_sha1_update(desc, (const u8 *)&bits, sizeof(bits));
	} else {
		kernel_fpu_begin();
		/* We need to fill a whole block for __sha1_ssse3_update() */
		if (padlen <= 56) {
			sctx->count += padlen;
			memcpy(sctx->buffer + index, padding, padlen);
		} else {
			__sha1_ssse3_update(desc, padding, padlen, index);
		}
		__sha1_ssse3_update(desc, (const u8 *)&bits, sizeof(bits), 56);
		kernel_fpu_end();
	}

	/* Store state in digest */
	for (i = 0; i < 5; i++)
		dst[i] = cpu_to_be32(sctx->state[i]);

	/* Wipe context */
	memset(sctx, 0, sizeof(*sctx));

	return 0;
}

static int sha1_ssse3_export(struct shash_desc *desc, void *out)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);

	memcpy(out, sctx, sizeof(*sctx));

	return 0;
}

static int sha1_ssse3_import(struct shash_desc *desc, const void *in)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);

	memcpy(sctx, in, sizeof(*sctx));

	return 0;
}

#ifdef CONFIG_AS_AVX2
=======
	return sha1_ssse3_finup(desc, NULL, 0, out);
}

static struct shash_alg sha1_ssse3_alg = {
	.digestsize	=	SHA1_DIGEST_SIZE,
	.init		=	sha1_base_init,
	.update		=	sha1_ssse3_update,
	.final		=	sha1_ssse3_final,
	.finup		=	sha1_ssse3_finup,
	.descsize	=	sizeof(struct sha1_state),
	.base		=	{
		.cra_name	=	"sha1",
		.cra_driver_name =	"sha1-ssse3",
		.cra_priority	=	150,
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	SHA1_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
};

static int register_sha1_ssse3(void)
{
	if (boot_cpu_has(X86_FEATURE_SSSE3))
		return crypto_register_shash(&sha1_ssse3_alg);
	return 0;
}

static void unregister_sha1_ssse3(void)
{
	if (boot_cpu_has(X86_FEATURE_SSSE3))
		crypto_unregister_shash(&sha1_ssse3_alg);
}

#ifdef CONFIG_AS_AVX
asmlinkage void sha1_transform_avx(u32 *digest, const char *data,
				   unsigned int rounds);

static int sha1_avx_update(struct shash_desc *desc, const u8 *data,
			     unsigned int len)
{
	return sha1_update(desc, data, len,
			(sha1_transform_fn *) sha1_transform_avx);
}

static int sha1_avx_finup(struct shash_desc *desc, const u8 *data,
			      unsigned int len, u8 *out)
{
	return sha1_finup(desc, data, len, out,
			(sha1_transform_fn *) sha1_transform_avx);
}

static int sha1_avx_final(struct shash_desc *desc, u8 *out)
{
	return sha1_avx_finup(desc, NULL, 0, out);
}

static struct shash_alg sha1_avx_alg = {
	.digestsize	=	SHA1_DIGEST_SIZE,
	.init		=	sha1_base_init,
	.update		=	sha1_avx_update,
	.final		=	sha1_avx_final,
	.finup		=	sha1_avx_finup,
	.descsize	=	sizeof(struct sha1_state),
	.base		=	{
		.cra_name	=	"sha1",
		.cra_driver_name =	"sha1-avx",
		.cra_priority	=	160,
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	SHA1_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
};

static bool avx_usable(void)
{
	if (!cpu_has_xfeatures(XFEATURE_MASK_SSE | XFEATURE_MASK_YMM, NULL)) {
		if (boot_cpu_has(X86_FEATURE_AVX))
			pr_info("AVX detected but unusable.\n");
		return false;
	}

	return true;
}

static int register_sha1_avx(void)
{
	if (avx_usable())
		return crypto_register_shash(&sha1_avx_alg);
	return 0;
}

static void unregister_sha1_avx(void)
{
	if (avx_usable())
		crypto_unregister_shash(&sha1_avx_alg);
}

#else  /* CONFIG_AS_AVX */
static inline int register_sha1_avx(void) { return 0; }
static inline void unregister_sha1_avx(void) { }
#endif /* CONFIG_AS_AVX */


#if defined(CONFIG_AS_AVX2) && (CONFIG_AS_AVX)
#define SHA1_AVX2_BLOCK_OPTSIZE	4	/* optimal 4*64 bytes of SHA1 blocks */

asmlinkage void sha1_transform_avx2(u32 *digest, const char *data,
				    unsigned int rounds);

static bool avx2_usable(void)
{
	if (avx_usable() && boot_cpu_has(X86_FEATURE_AVX2)
		&& boot_cpu_has(X86_FEATURE_BMI1)
		&& boot_cpu_has(X86_FEATURE_BMI2))
		return true;

	return false;
}

>>>>>>> v4.9.227
static void sha1_apply_transform_avx2(u32 *digest, const char *data,
				unsigned int rounds)
{
	/* Select the optimal transform based on data block size */
	if (rounds >= SHA1_AVX2_BLOCK_OPTSIZE)
		sha1_transform_avx2(digest, data, rounds);
	else
		sha1_transform_avx(digest, data, rounds);
}
<<<<<<< HEAD
#endif

static struct shash_alg alg = {
	.digestsize	=	SHA1_DIGEST_SIZE,
	.init		=	sha1_ssse3_init,
	.update		=	sha1_ssse3_update,
	.final		=	sha1_ssse3_final,
	.export		=	sha1_ssse3_export,
	.import		=	sha1_ssse3_import,
	.descsize	=	sizeof(struct sha1_state),
	.statesize	=	sizeof(struct sha1_state),
	.base		=	{
		.cra_name	=	"sha1",
		.cra_driver_name=	"sha1-ssse3",
		.cra_priority	=	150,
=======

static int sha1_avx2_update(struct shash_desc *desc, const u8 *data,
			     unsigned int len)
{
	return sha1_update(desc, data, len,
		(sha1_transform_fn *) sha1_apply_transform_avx2);
}

static int sha1_avx2_finup(struct shash_desc *desc, const u8 *data,
			      unsigned int len, u8 *out)
{
	return sha1_finup(desc, data, len, out,
		(sha1_transform_fn *) sha1_apply_transform_avx2);
}

static int sha1_avx2_final(struct shash_desc *desc, u8 *out)
{
	return sha1_avx2_finup(desc, NULL, 0, out);
}

static struct shash_alg sha1_avx2_alg = {
	.digestsize	=	SHA1_DIGEST_SIZE,
	.init		=	sha1_base_init,
	.update		=	sha1_avx2_update,
	.final		=	sha1_avx2_final,
	.finup		=	sha1_avx2_finup,
	.descsize	=	sizeof(struct sha1_state),
	.base		=	{
		.cra_name	=	"sha1",
		.cra_driver_name =	"sha1-avx2",
		.cra_priority	=	170,
>>>>>>> v4.9.227
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	SHA1_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
};

<<<<<<< HEAD
#ifdef CONFIG_AS_AVX
static bool __init avx_usable(void)
{
	u64 xcr0;

	if (!cpu_has_avx || !cpu_has_osxsave)
		return false;

	xcr0 = xgetbv(XCR_XFEATURE_ENABLED_MASK);
	if ((xcr0 & (XSTATE_SSE | XSTATE_YMM)) != (XSTATE_SSE | XSTATE_YMM)) {
		pr_info("AVX detected but unusable.\n");

		return false;
	}

	return true;
}

#ifdef CONFIG_AS_AVX2
static bool __init avx2_usable(void)
{
	if (false && avx_usable() && cpu_has_avx2 && boot_cpu_has(X86_FEATURE_BMI1) &&
	    boot_cpu_has(X86_FEATURE_BMI2))
		return true;

	return false;
}
#endif
=======
static int register_sha1_avx2(void)
{
	if (avx2_usable())
		return crypto_register_shash(&sha1_avx2_alg);
	return 0;
}

static void unregister_sha1_avx2(void)
{
	if (avx2_usable())
		crypto_unregister_shash(&sha1_avx2_alg);
}

#else
static inline int register_sha1_avx2(void) { return 0; }
static inline void unregister_sha1_avx2(void) { }
#endif

#ifdef CONFIG_AS_SHA1_NI
asmlinkage void sha1_ni_transform(u32 *digest, const char *data,
				   unsigned int rounds);

static int sha1_ni_update(struct shash_desc *desc, const u8 *data,
			     unsigned int len)
{
	return sha1_update(desc, data, len,
		(sha1_transform_fn *) sha1_ni_transform);
}

static int sha1_ni_finup(struct shash_desc *desc, const u8 *data,
			      unsigned int len, u8 *out)
{
	return sha1_finup(desc, data, len, out,
		(sha1_transform_fn *) sha1_ni_transform);
}

static int sha1_ni_final(struct shash_desc *desc, u8 *out)
{
	return sha1_ni_finup(desc, NULL, 0, out);
}

static struct shash_alg sha1_ni_alg = {
	.digestsize	=	SHA1_DIGEST_SIZE,
	.init		=	sha1_base_init,
	.update		=	sha1_ni_update,
	.final		=	sha1_ni_final,
	.finup		=	sha1_ni_finup,
	.descsize	=	sizeof(struct sha1_state),
	.base		=	{
		.cra_name	=	"sha1",
		.cra_driver_name =	"sha1-ni",
		.cra_priority	=	250,
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	SHA1_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
};

static int register_sha1_ni(void)
{
	if (boot_cpu_has(X86_FEATURE_SHA_NI))
		return crypto_register_shash(&sha1_ni_alg);
	return 0;
}

static void unregister_sha1_ni(void)
{
	if (boot_cpu_has(X86_FEATURE_SHA_NI))
		crypto_unregister_shash(&sha1_ni_alg);
}

#else
static inline int register_sha1_ni(void) { return 0; }
static inline void unregister_sha1_ni(void) { }
>>>>>>> v4.9.227
#endif

static int __init sha1_ssse3_mod_init(void)
{
<<<<<<< HEAD
	char *algo_name;

	/* test for SSSE3 first */
	if (cpu_has_ssse3) {
		sha1_transform_asm = sha1_transform_ssse3;
		algo_name = "SSSE3";
	}

#ifdef CONFIG_AS_AVX
	/* allow AVX to override SSSE3, it's a little faster */
	if (avx_usable()) {
		sha1_transform_asm = sha1_transform_avx;
		algo_name = "AVX";
#ifdef CONFIG_AS_AVX2
		/* allow AVX2 to override AVX, it's a little faster */
		if (avx2_usable()) {
			sha1_transform_asm = sha1_apply_transform_avx2;
			algo_name = "AVX2";
		}
#endif
	}
#endif

	if (sha1_transform_asm) {
		pr_info("Using %s optimized SHA-1 implementation\n", algo_name);
		return crypto_register_shash(&alg);
	}
	pr_info("Neither AVX nor AVX2 nor SSSE3 is available/usable.\n");

=======
	if (register_sha1_ssse3())
		goto fail;

	if (register_sha1_avx()) {
		unregister_sha1_ssse3();
		goto fail;
	}

	if (register_sha1_avx2()) {
		unregister_sha1_avx();
		unregister_sha1_ssse3();
		goto fail;
	}

	if (register_sha1_ni()) {
		unregister_sha1_avx2();
		unregister_sha1_avx();
		unregister_sha1_ssse3();
		goto fail;
	}

	return 0;
fail:
>>>>>>> v4.9.227
	return -ENODEV;
}

static void __exit sha1_ssse3_mod_fini(void)
{
<<<<<<< HEAD
	crypto_unregister_shash(&alg);
=======
	unregister_sha1_ni();
	unregister_sha1_avx2();
	unregister_sha1_avx();
	unregister_sha1_ssse3();
>>>>>>> v4.9.227
}

module_init(sha1_ssse3_mod_init);
module_exit(sha1_ssse3_mod_fini);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SHA1 Secure Hash Algorithm, Supplemental SSE3 accelerated");

MODULE_ALIAS_CRYPTO("sha1");
<<<<<<< HEAD
=======
MODULE_ALIAS_CRYPTO("sha1-ssse3");
MODULE_ALIAS_CRYPTO("sha1-avx");
MODULE_ALIAS_CRYPTO("sha1-avx2");
#ifdef CONFIG_AS_SHA1_NI
MODULE_ALIAS_CRYPTO("sha1-ni");
#endif
>>>>>>> v4.9.227
