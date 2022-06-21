/*
 * RNG: Random Number Generator  algorithms under the crypto API
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

#ifndef _CRYPTO_INTERNAL_RNG_H
#define _CRYPTO_INTERNAL_RNG_H

#include <crypto/algapi.h>
#include <crypto/rng.h>

<<<<<<< HEAD
extern const struct crypto_type crypto_rng_type;
=======
int crypto_register_rng(struct rng_alg *alg);
void crypto_unregister_rng(struct rng_alg *alg);
int crypto_register_rngs(struct rng_alg *algs, int count);
void crypto_unregister_rngs(struct rng_alg *algs, int count);

#if defined(CONFIG_CRYPTO_RNG) || defined(CONFIG_CRYPTO_RNG_MODULE)
int crypto_del_default_rng(void);
#else
static inline int crypto_del_default_rng(void)
{
	return 0;
}
#endif
>>>>>>> v4.9.227

static inline void *crypto_rng_ctx(struct crypto_rng *tfm)
{
	return crypto_tfm_ctx(&tfm->base);
}

<<<<<<< HEAD
=======
static inline void crypto_rng_set_entropy(struct crypto_rng *tfm,
					  const u8 *data, unsigned int len)
{
	crypto_rng_alg(tfm)->set_ent(tfm, data, len);
}

>>>>>>> v4.9.227
#endif
