/*
 * Cryptographic API.
 *
 * Glue code for the SHA512 Secure Hash Algorithm assembler
 * implementation using supplemental SSE3 / AVX / AVX2 instructions.
 *
 * This file is based on sha512_generic.c
 *
 * Copyright (C) 2013 Intel Corporation
 * Author: Tim Chen <tim.c.chen@linux.intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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

#include <linux/string.h>

asmlinkage void sha512_transform_ssse3(const char *data, u64 *digest,
				     u64 rounds);
#ifdef CONFIG_AS_AVX
asmlinkage void sha512_transform_avx(const char *data, u64 *digest,
				     u64 rounds);
#endif
#ifdef CONFIG_AS_AVX2
asmlinkage void sha512_transform_rorx(const char *data, u64 *digest,
				     u64 rounds);
#endif

static asmlinkage void (*sha512_transform_asm)(const char *, u64 *, u64);


static int sha512_ssse3_init(struct shash_desc *desc)
{
	struct sha512_state *sctx = shash_desc_ctx(desc);

	sctx->state[0] = SHA512_H0;
	sctx->state[1] = SHA512_H1;
	sctx->state[2] = SHA512_H2;
	sctx->state[3] = SHA512_H3;
	sctx->state[4] = SHA512_H4;
	sctx->state[5] = SHA512_H5;
	sctx->state[6] = SHA512_H6;
	sctx->state[7] = SHA512_H7;
	sctx->count[0] = sctx->count[1] = 0;
=======
#include <crypto/sha512_base.h>
#include <asm/fpu/api.h>

#include <linux/string.h>

asmlinkage void sha512_transform_ssse3(u64 *digest, const char *data,
				       u64 rounds);

typedef void (sha512_transform_fn)(u64 *digest, const char *data, u64 rounds);

static int sha512_update(struct shash_desc *desc, const u8 *data,
		       unsigned int len, sha512_transform_fn *sha512_xform)
{
	struct sha512_state *sctx = shash_desc_ctx(desc);

	if (!irq_fpu_usable() ||
	    (sctx->count[0] % SHA512_BLOCK_SIZE) + len < SHA512_BLOCK_SIZE)
		return crypto_sha512_update(desc, data, len);

	/* make sure casting to sha512_block_fn() is safe */
	BUILD_BUG_ON(offsetof(struct sha512_state, state) != 0);

	kernel_fpu_begin();
	sha512_base_do_update(desc, data, len,
			      (sha512_block_fn *)sha512_xform);
	kernel_fpu_end();
>>>>>>> v4.9.227

	return 0;
}

<<<<<<< HEAD
static int __sha512_ssse3_update(struct shash_desc *desc, const u8 *data,
			       unsigned int len, unsigned int partial)
{
	struct sha512_state *sctx = shash_desc_ctx(desc);
	unsigned int done = 0;

	sctx->count[0] += len;
	if (sctx->count[0] < len)
		sctx->count[1]++;

	if (partial) {
		done = SHA512_BLOCK_SIZE - partial;
		memcpy(sctx->buf + partial, data, done);
		sha512_transform_asm(sctx->buf, sctx->state, 1);
	}

	if (len - done >= SHA512_BLOCK_SIZE) {
		const unsigned int rounds = (len - done) / SHA512_BLOCK_SIZE;

		sha512_transform_asm(data + done, sctx->state, (u64) rounds);

		done += rounds * SHA512_BLOCK_SIZE;
	}

	memcpy(sctx->buf, data + done, len - done);

	return 0;
}

static int sha512_ssse3_update(struct shash_desc *desc, const u8 *data,
			     unsigned int len)
{
	struct sha512_state *sctx = shash_desc_ctx(desc);
	unsigned int partial = sctx->count[0] % SHA512_BLOCK_SIZE;
	int res;

	/* Handle the fast case right here */
	if (partial + len < SHA512_BLOCK_SIZE) {
		sctx->count[0] += len;
		if (sctx->count[0] < len)
			sctx->count[1]++;
		memcpy(sctx->buf + partial, data, len);

		return 0;
	}

	if (!irq_fpu_usable()) {
		res = crypto_sha512_update(desc, data, len);
	} else {
		kernel_fpu_begin();
		res = __sha512_ssse3_update(desc, data, len, partial);
		kernel_fpu_end();
	}

	return res;
}

=======
static int sha512_finup(struct shash_desc *desc, const u8 *data,
	      unsigned int len, u8 *out, sha512_transform_fn *sha512_xform)
{
	if (!irq_fpu_usable())
		return crypto_sha512_finup(desc, data, len, out);

	kernel_fpu_begin();
	if (len)
		sha512_base_do_update(desc, data, len,
				      (sha512_block_fn *)sha512_xform);
	sha512_base_do_finalize(desc, (sha512_block_fn *)sha512_xform);
	kernel_fpu_end();

	return sha512_base_finish(desc, out);
}

static int sha512_ssse3_update(struct shash_desc *desc, const u8 *data,
		       unsigned int len)
{
	return sha512_update(desc, data, len, sha512_transform_ssse3);
}

static int sha512_ssse3_finup(struct shash_desc *desc, const u8 *data,
	      unsigned int len, u8 *out)
{
	return sha512_finup(desc, data, len, out, sha512_transform_ssse3);
}
>>>>>>> v4.9.227

/* Add padding and return the message digest. */
static int sha512_ssse3_final(struct shash_desc *desc, u8 *out)
{
<<<<<<< HEAD
	struct sha512_state *sctx = shash_desc_ctx(desc);
	unsigned int i, index, padlen;
	__be64 *dst = (__be64 *)out;
	__be64 bits[2];
	static const u8 padding[SHA512_BLOCK_SIZE] = { 0x80, };

	/* save number of bits */
	bits[1] = cpu_to_be64(sctx->count[0] << 3);
	bits[0] = cpu_to_be64(sctx->count[1] << 3 | sctx->count[0] >> 61);

	/* Pad out to 112 mod 128 and append length */
	index = sctx->count[0] & 0x7f;
	padlen = (index < 112) ? (112 - index) : ((128+112) - index);

	if (!irq_fpu_usable()) {
		crypto_sha512_update(desc, padding, padlen);
		crypto_sha512_update(desc, (const u8 *)&bits, sizeof(bits));
	} else {
		kernel_fpu_begin();
		/* We need to fill a whole block for __sha512_ssse3_update() */
		if (padlen <= 112) {
			sctx->count[0] += padlen;
			if (sctx->count[0] < padlen)
				sctx->count[1]++;
			memcpy(sctx->buf + index, padding, padlen);
		} else {
			__sha512_ssse3_update(desc, padding, padlen, index);
		}
		__sha512_ssse3_update(desc, (const u8 *)&bits,
					sizeof(bits), 112);
		kernel_fpu_end();
	}

	/* Store state in digest */
	for (i = 0; i < 8; i++)
		dst[i] = cpu_to_be64(sctx->state[i]);

	/* Wipe context */
	memset(sctx, 0, sizeof(*sctx));

	return 0;
}

static int sha512_ssse3_export(struct shash_desc *desc, void *out)
{
	struct sha512_state *sctx = shash_desc_ctx(desc);

	memcpy(out, sctx, sizeof(*sctx));

	return 0;
}

static int sha512_ssse3_import(struct shash_desc *desc, const void *in)
{
	struct sha512_state *sctx = shash_desc_ctx(desc);

	memcpy(sctx, in, sizeof(*sctx));

	return 0;
}

static int sha384_ssse3_init(struct shash_desc *desc)
{
	struct sha512_state *sctx = shash_desc_ctx(desc);

	sctx->state[0] = SHA384_H0;
	sctx->state[1] = SHA384_H1;
	sctx->state[2] = SHA384_H2;
	sctx->state[3] = SHA384_H3;
	sctx->state[4] = SHA384_H4;
	sctx->state[5] = SHA384_H5;
	sctx->state[6] = SHA384_H6;
	sctx->state[7] = SHA384_H7;

	sctx->count[0] = sctx->count[1] = 0;

	return 0;
}

static int sha384_ssse3_final(struct shash_desc *desc, u8 *hash)
{
	u8 D[SHA512_DIGEST_SIZE];

	sha512_ssse3_final(desc, D);

	memcpy(hash, D, SHA384_DIGEST_SIZE);
	memset(D, 0, SHA512_DIGEST_SIZE);

	return 0;
}

static struct shash_alg algs[] = { {
	.digestsize	=	SHA512_DIGEST_SIZE,
	.init		=	sha512_ssse3_init,
	.update		=	sha512_ssse3_update,
	.final		=	sha512_ssse3_final,
	.export		=	sha512_ssse3_export,
	.import		=	sha512_ssse3_import,
	.descsize	=	sizeof(struct sha512_state),
	.statesize	=	sizeof(struct sha512_state),
=======
	return sha512_ssse3_finup(desc, NULL, 0, out);
}

static struct shash_alg sha512_ssse3_algs[] = { {
	.digestsize	=	SHA512_DIGEST_SIZE,
	.init		=	sha512_base_init,
	.update		=	sha512_ssse3_update,
	.final		=	sha512_ssse3_final,
	.finup		=	sha512_ssse3_finup,
	.descsize	=	sizeof(struct sha512_state),
>>>>>>> v4.9.227
	.base		=	{
		.cra_name	=	"sha512",
		.cra_driver_name =	"sha512-ssse3",
		.cra_priority	=	150,
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	SHA512_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
},  {
	.digestsize	=	SHA384_DIGEST_SIZE,
<<<<<<< HEAD
	.init		=	sha384_ssse3_init,
	.update		=	sha512_ssse3_update,
	.final		=	sha384_ssse3_final,
	.export		=	sha512_ssse3_export,
	.import		=	sha512_ssse3_import,
	.descsize	=	sizeof(struct sha512_state),
	.statesize	=	sizeof(struct sha512_state),
=======
	.init		=	sha384_base_init,
	.update		=	sha512_ssse3_update,
	.final		=	sha512_ssse3_final,
	.finup		=	sha512_ssse3_finup,
	.descsize	=	sizeof(struct sha512_state),
>>>>>>> v4.9.227
	.base		=	{
		.cra_name	=	"sha384",
		.cra_driver_name =	"sha384-ssse3",
		.cra_priority	=	150,
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	SHA384_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
} };

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

=======
static int register_sha512_ssse3(void)
{
	if (boot_cpu_has(X86_FEATURE_SSSE3))
		return crypto_register_shashes(sha512_ssse3_algs,
			ARRAY_SIZE(sha512_ssse3_algs));
	return 0;
}

static void unregister_sha512_ssse3(void)
{
	if (boot_cpu_has(X86_FEATURE_SSSE3))
		crypto_unregister_shashes(sha512_ssse3_algs,
			ARRAY_SIZE(sha512_ssse3_algs));
}

#ifdef CONFIG_AS_AVX
asmlinkage void sha512_transform_avx(u64 *digest, const char *data,
				     u64 rounds);
static bool avx_usable(void)
{
	if (!cpu_has_xfeatures(XFEATURE_MASK_SSE | XFEATURE_MASK_YMM, NULL)) {
		if (boot_cpu_has(X86_FEATURE_AVX))
			pr_info("AVX detected but unusable.\n");
>>>>>>> v4.9.227
		return false;
	}

	return true;
}
<<<<<<< HEAD
=======

static int sha512_avx_update(struct shash_desc *desc, const u8 *data,
		       unsigned int len)
{
	return sha512_update(desc, data, len, sha512_transform_avx);
}

static int sha512_avx_finup(struct shash_desc *desc, const u8 *data,
	      unsigned int len, u8 *out)
{
	return sha512_finup(desc, data, len, out, sha512_transform_avx);
}

/* Add padding and return the message digest. */
static int sha512_avx_final(struct shash_desc *desc, u8 *out)
{
	return sha512_avx_finup(desc, NULL, 0, out);
}

static struct shash_alg sha512_avx_algs[] = { {
	.digestsize	=	SHA512_DIGEST_SIZE,
	.init		=	sha512_base_init,
	.update		=	sha512_avx_update,
	.final		=	sha512_avx_final,
	.finup		=	sha512_avx_finup,
	.descsize	=	sizeof(struct sha512_state),
	.base		=	{
		.cra_name	=	"sha512",
		.cra_driver_name =	"sha512-avx",
		.cra_priority	=	160,
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	SHA512_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
},  {
	.digestsize	=	SHA384_DIGEST_SIZE,
	.init		=	sha384_base_init,
	.update		=	sha512_avx_update,
	.final		=	sha512_avx_final,
	.finup		=	sha512_avx_finup,
	.descsize	=	sizeof(struct sha512_state),
	.base		=	{
		.cra_name	=	"sha384",
		.cra_driver_name =	"sha384-avx",
		.cra_priority	=	160,
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	SHA384_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
} };

static int register_sha512_avx(void)
{
	if (avx_usable())
		return crypto_register_shashes(sha512_avx_algs,
			ARRAY_SIZE(sha512_avx_algs));
	return 0;
}

static void unregister_sha512_avx(void)
{
	if (avx_usable())
		crypto_unregister_shashes(sha512_avx_algs,
			ARRAY_SIZE(sha512_avx_algs));
}
#else
static inline int register_sha512_avx(void) { return 0; }
static inline void unregister_sha512_avx(void) { }
#endif

#if defined(CONFIG_AS_AVX2) && defined(CONFIG_AS_AVX)
asmlinkage void sha512_transform_rorx(u64 *digest, const char *data,
				      u64 rounds);

static int sha512_avx2_update(struct shash_desc *desc, const u8 *data,
		       unsigned int len)
{
	return sha512_update(desc, data, len, sha512_transform_rorx);
}

static int sha512_avx2_finup(struct shash_desc *desc, const u8 *data,
	      unsigned int len, u8 *out)
{
	return sha512_finup(desc, data, len, out, sha512_transform_rorx);
}

/* Add padding and return the message digest. */
static int sha512_avx2_final(struct shash_desc *desc, u8 *out)
{
	return sha512_avx2_finup(desc, NULL, 0, out);
}

static struct shash_alg sha512_avx2_algs[] = { {
	.digestsize	=	SHA512_DIGEST_SIZE,
	.init		=	sha512_base_init,
	.update		=	sha512_avx2_update,
	.final		=	sha512_avx2_final,
	.finup		=	sha512_avx2_finup,
	.descsize	=	sizeof(struct sha512_state),
	.base		=	{
		.cra_name	=	"sha512",
		.cra_driver_name =	"sha512-avx2",
		.cra_priority	=	170,
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	SHA512_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
},  {
	.digestsize	=	SHA384_DIGEST_SIZE,
	.init		=	sha384_base_init,
	.update		=	sha512_avx2_update,
	.final		=	sha512_avx2_final,
	.finup		=	sha512_avx2_finup,
	.descsize	=	sizeof(struct sha512_state),
	.base		=	{
		.cra_name	=	"sha384",
		.cra_driver_name =	"sha384-avx2",
		.cra_priority	=	170,
		.cra_flags	=	CRYPTO_ALG_TYPE_SHASH,
		.cra_blocksize	=	SHA384_BLOCK_SIZE,
		.cra_module	=	THIS_MODULE,
	}
} };

static bool avx2_usable(void)
{
	if (avx_usable() && boot_cpu_has(X86_FEATURE_AVX2) &&
		    boot_cpu_has(X86_FEATURE_BMI2))
		return true;

	return false;
}

static int register_sha512_avx2(void)
{
	if (avx2_usable())
		return crypto_register_shashes(sha512_avx2_algs,
			ARRAY_SIZE(sha512_avx2_algs));
	return 0;
}

static void unregister_sha512_avx2(void)
{
	if (avx2_usable())
		crypto_unregister_shashes(sha512_avx2_algs,
			ARRAY_SIZE(sha512_avx2_algs));
}
#else
static inline int register_sha512_avx2(void) { return 0; }
static inline void unregister_sha512_avx2(void) { }
>>>>>>> v4.9.227
#endif

static int __init sha512_ssse3_mod_init(void)
{
<<<<<<< HEAD
	/* test for SSSE3 first */
	if (cpu_has_ssse3)
		sha512_transform_asm = sha512_transform_ssse3;

#ifdef CONFIG_AS_AVX
	/* allow AVX to override SSSE3, it's a little faster */
	if (avx_usable()) {
#ifdef CONFIG_AS_AVX2
		if (boot_cpu_has(X86_FEATURE_AVX2))
			sha512_transform_asm = sha512_transform_rorx;
		else
#endif
			sha512_transform_asm = sha512_transform_avx;
	}
#endif

	if (sha512_transform_asm) {
#ifdef CONFIG_AS_AVX
		if (sha512_transform_asm == sha512_transform_avx)
			pr_info("Using AVX optimized SHA-512 implementation\n");
#ifdef CONFIG_AS_AVX2
		else if (sha512_transform_asm == sha512_transform_rorx)
			pr_info("Using AVX2 optimized SHA-512 implementation\n");
#endif
		else
#endif
			pr_info("Using SSSE3 optimized SHA-512 implementation\n");
		return crypto_register_shashes(algs, ARRAY_SIZE(algs));
	}
	pr_info("Neither AVX nor SSSE3 is available/usable.\n");

=======

	if (register_sha512_ssse3())
		goto fail;

	if (register_sha512_avx()) {
		unregister_sha512_ssse3();
		goto fail;
	}

	if (register_sha512_avx2()) {
		unregister_sha512_avx();
		unregister_sha512_ssse3();
		goto fail;
	}

	return 0;
fail:
>>>>>>> v4.9.227
	return -ENODEV;
}

static void __exit sha512_ssse3_mod_fini(void)
{
<<<<<<< HEAD
	crypto_unregister_shashes(algs, ARRAY_SIZE(algs));
=======
	unregister_sha512_avx2();
	unregister_sha512_avx();
	unregister_sha512_ssse3();
>>>>>>> v4.9.227
}

module_init(sha512_ssse3_mod_init);
module_exit(sha512_ssse3_mod_fini);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SHA512 Secure Hash Algorithm, Supplemental SSE3 accelerated");

MODULE_ALIAS_CRYPTO("sha512");
<<<<<<< HEAD
MODULE_ALIAS_CRYPTO("sha384");
=======
MODULE_ALIAS_CRYPTO("sha512-ssse3");
MODULE_ALIAS_CRYPTO("sha512-avx");
MODULE_ALIAS_CRYPTO("sha512-avx2");
MODULE_ALIAS_CRYPTO("sha384");
MODULE_ALIAS_CRYPTO("sha384-ssse3");
MODULE_ALIAS_CRYPTO("sha384-avx");
MODULE_ALIAS_CRYPTO("sha384-avx2");
>>>>>>> v4.9.227
