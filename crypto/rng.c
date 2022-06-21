/*
 * Cryptographic API.
 *
 * RNG operations.
 *
 * Copyright (c) 2008 Neil Horman <nhorman@tuxdriver.com>
<<<<<<< HEAD
=======
 * Copyright (c) 2015 Herbert Xu <herbert@gondor.apana.org.au>
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 */

#include <linux/atomic.h>
#include <crypto/internal/rng.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/random.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/cryptouser.h>
#include <net/netlink.h>

<<<<<<< HEAD
=======
#include "internal.h"

>>>>>>> v4.9.227
static DEFINE_MUTEX(crypto_default_rng_lock);
struct crypto_rng *crypto_default_rng;
EXPORT_SYMBOL_GPL(crypto_default_rng);
static int crypto_default_rng_refcnt;

<<<<<<< HEAD
static int rngapi_reset(struct crypto_rng *tfm, u8 *seed, unsigned int slen)
=======
static inline struct crypto_rng *__crypto_rng_cast(struct crypto_tfm *tfm)
{
	return container_of(tfm, struct crypto_rng, base);
}

int crypto_rng_reset(struct crypto_rng *tfm, const u8 *seed, unsigned int slen)
>>>>>>> v4.9.227
{
	u8 *buf = NULL;
	int err;

	if (!seed && slen) {
		buf = kmalloc(slen, GFP_KERNEL);
		if (!buf)
			return -ENOMEM;

		get_random_bytes(buf, slen);
		seed = buf;
	}

<<<<<<< HEAD
	err = crypto_rng_alg(tfm)->rng_reset(tfm, seed, slen);

	kfree(buf);
	return err;
}

static int crypto_init_rng_ops(struct crypto_tfm *tfm, u32 type, u32 mask)
{
	struct rng_alg *alg = &tfm->__crt_alg->cra_rng;
	struct rng_tfm *ops = &tfm->crt_rng;

	ops->rng_gen_random = alg->rng_make_random;
	ops->rng_reset = rngapi_reset;

	return 0;
}

=======
	err = crypto_rng_alg(tfm)->seed(tfm, seed, slen);

	kzfree(buf);
	return err;
}
EXPORT_SYMBOL_GPL(crypto_rng_reset);

static int crypto_rng_init_tfm(struct crypto_tfm *tfm)
{
	return 0;
}

static unsigned int seedsize(struct crypto_alg *alg)
{
	struct rng_alg *ralg = container_of(alg, struct rng_alg, base);

	return ralg->seedsize;
}

>>>>>>> v4.9.227
#ifdef CONFIG_NET
static int crypto_rng_report(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_report_rng rrng;

	strncpy(rrng.type, "rng", sizeof(rrng.type));

<<<<<<< HEAD
	rrng.seedsize = alg->cra_rng.seedsize;
=======
	rrng.seedsize = seedsize(alg);
>>>>>>> v4.9.227

	if (nla_put(skb, CRYPTOCFGA_REPORT_RNG,
		    sizeof(struct crypto_report_rng), &rrng))
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -EMSGSIZE;
}
#else
static int crypto_rng_report(struct sk_buff *skb, struct crypto_alg *alg)
{
	return -ENOSYS;
}
#endif

static void crypto_rng_show(struct seq_file *m, struct crypto_alg *alg)
	__attribute__ ((unused));
static void crypto_rng_show(struct seq_file *m, struct crypto_alg *alg)
{
	seq_printf(m, "type         : rng\n");
<<<<<<< HEAD
	seq_printf(m, "seedsize     : %u\n", alg->cra_rng.seedsize);
}

static unsigned int crypto_rng_ctxsize(struct crypto_alg *alg, u32 type,
				       u32 mask)
{
	return alg->cra_ctxsize;
}

const struct crypto_type crypto_rng_type = {
	.ctxsize = crypto_rng_ctxsize,
	.init = crypto_init_rng_ops,
=======
	seq_printf(m, "seedsize     : %u\n", seedsize(alg));
}

static const struct crypto_type crypto_rng_type = {
	.extsize = crypto_alg_extsize,
	.init_tfm = crypto_rng_init_tfm,
>>>>>>> v4.9.227
#ifdef CONFIG_PROC_FS
	.show = crypto_rng_show,
#endif
	.report = crypto_rng_report,
<<<<<<< HEAD
};
EXPORT_SYMBOL_GPL(crypto_rng_type);
=======
	.maskclear = ~CRYPTO_ALG_TYPE_MASK,
	.maskset = CRYPTO_ALG_TYPE_MASK,
	.type = CRYPTO_ALG_TYPE_RNG,
	.tfmsize = offsetof(struct crypto_rng, base),
};

struct crypto_rng *crypto_alloc_rng(const char *alg_name, u32 type, u32 mask)
{
	return crypto_alloc_tfm(alg_name, &crypto_rng_type, type, mask);
}
EXPORT_SYMBOL_GPL(crypto_alloc_rng);
>>>>>>> v4.9.227

int crypto_get_default_rng(void)
{
	struct crypto_rng *rng;
	int err;

	mutex_lock(&crypto_default_rng_lock);
	if (!crypto_default_rng) {
		rng = crypto_alloc_rng("stdrng", 0, 0);
		err = PTR_ERR(rng);
		if (IS_ERR(rng))
			goto unlock;

		err = crypto_rng_reset(rng, NULL, crypto_rng_seedsize(rng));
		if (err) {
			crypto_free_rng(rng);
			goto unlock;
		}

		crypto_default_rng = rng;
	}

	crypto_default_rng_refcnt++;
	err = 0;

unlock:
	mutex_unlock(&crypto_default_rng_lock);

	return err;
}
EXPORT_SYMBOL_GPL(crypto_get_default_rng);

void crypto_put_default_rng(void)
{
	mutex_lock(&crypto_default_rng_lock);
<<<<<<< HEAD
	if (!--crypto_default_rng_refcnt) {
		crypto_free_rng(crypto_default_rng);
		crypto_default_rng = NULL;
	}
=======
	crypto_default_rng_refcnt--;
>>>>>>> v4.9.227
	mutex_unlock(&crypto_default_rng_lock);
}
EXPORT_SYMBOL_GPL(crypto_put_default_rng);

<<<<<<< HEAD
=======
#if defined(CONFIG_CRYPTO_RNG) || defined(CONFIG_CRYPTO_RNG_MODULE)
int crypto_del_default_rng(void)
{
	int err = -EBUSY;

	mutex_lock(&crypto_default_rng_lock);
	if (crypto_default_rng_refcnt)
		goto out;

	crypto_free_rng(crypto_default_rng);
	crypto_default_rng = NULL;

	err = 0;

out:
	mutex_unlock(&crypto_default_rng_lock);

	return err;
}
EXPORT_SYMBOL_GPL(crypto_del_default_rng);
#endif

int crypto_register_rng(struct rng_alg *alg)
{
	struct crypto_alg *base = &alg->base;

	if (alg->seedsize > PAGE_SIZE / 8)
		return -EINVAL;

	base->cra_type = &crypto_rng_type;
	base->cra_flags &= ~CRYPTO_ALG_TYPE_MASK;
	base->cra_flags |= CRYPTO_ALG_TYPE_RNG;

	return crypto_register_alg(base);
}
EXPORT_SYMBOL_GPL(crypto_register_rng);

void crypto_unregister_rng(struct rng_alg *alg)
{
	crypto_unregister_alg(&alg->base);
}
EXPORT_SYMBOL_GPL(crypto_unregister_rng);

int crypto_register_rngs(struct rng_alg *algs, int count)
{
	int i, ret;

	for (i = 0; i < count; i++) {
		ret = crypto_register_rng(algs + i);
		if (ret)
			goto err;
	}

	return 0;

err:
	for (--i; i >= 0; --i)
		crypto_unregister_rng(algs + i);

	return ret;
}
EXPORT_SYMBOL_GPL(crypto_register_rngs);

void crypto_unregister_rngs(struct rng_alg *algs, int count)
{
	int i;

	for (i = count - 1; i >= 0; --i)
		crypto_unregister_rng(algs + i);
}
EXPORT_SYMBOL_GPL(crypto_unregister_rngs);

>>>>>>> v4.9.227
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Random Number Generator");
