/*
 * AEAD: Authenticated Encryption with Associated Data
 *
 * This file provides API support for AEAD algorithms.
 *
<<<<<<< HEAD
 * Copyright (c) 2007 Herbert Xu <herbert@gondor.apana.org.au>
=======
 * Copyright (c) 2007-2015 Herbert Xu <herbert@gondor.apana.org.au>
>>>>>>> v4.9.227
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 */

<<<<<<< HEAD
#include <crypto/internal/aead.h>
=======
#include <crypto/internal/geniv.h>
#include <crypto/internal/rng.h>
#include <crypto/null.h>
#include <crypto/scatterwalk.h>
>>>>>>> v4.9.227
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rtnetlink.h>
<<<<<<< HEAD
#include <linux/sched.h>
=======
>>>>>>> v4.9.227
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/cryptouser.h>
#include <net/netlink.h>

#include "internal.h"

static int setkey_unaligned(struct crypto_aead *tfm, const u8 *key,
			    unsigned int keylen)
{
<<<<<<< HEAD
	struct aead_alg *aead = crypto_aead_alg(tfm);
=======
>>>>>>> v4.9.227
	unsigned long alignmask = crypto_aead_alignmask(tfm);
	int ret;
	u8 *buffer, *alignbuffer;
	unsigned long absize;

	absize = keylen + alignmask;
	buffer = kmalloc(absize, GFP_ATOMIC);
	if (!buffer)
		return -ENOMEM;

	alignbuffer = (u8 *)ALIGN((unsigned long)buffer, alignmask + 1);
	memcpy(alignbuffer, key, keylen);
<<<<<<< HEAD
	ret = aead->setkey(tfm, alignbuffer, keylen);
=======
	ret = crypto_aead_alg(tfm)->setkey(tfm, alignbuffer, keylen);
>>>>>>> v4.9.227
	memset(alignbuffer, 0, keylen);
	kfree(buffer);
	return ret;
}

<<<<<<< HEAD
static int setkey(struct crypto_aead *tfm, const u8 *key, unsigned int keylen)
{
	struct aead_alg *aead = crypto_aead_alg(tfm);
=======
int crypto_aead_setkey(struct crypto_aead *tfm,
		       const u8 *key, unsigned int keylen)
{
>>>>>>> v4.9.227
	unsigned long alignmask = crypto_aead_alignmask(tfm);

	if ((unsigned long)key & alignmask)
		return setkey_unaligned(tfm, key, keylen);

<<<<<<< HEAD
	return aead->setkey(tfm, key, keylen);
}

int crypto_aead_setauthsize(struct crypto_aead *tfm, unsigned int authsize)
{
	struct aead_tfm *crt = crypto_aead_crt(tfm);
	int err;

	if (authsize > crypto_aead_alg(tfm)->maxauthsize)
		return -EINVAL;

	if (crypto_aead_alg(tfm)->setauthsize) {
		err = crypto_aead_alg(tfm)->setauthsize(crt->base, authsize);
=======
	return crypto_aead_alg(tfm)->setkey(tfm, key, keylen);
}
EXPORT_SYMBOL_GPL(crypto_aead_setkey);

int crypto_aead_setauthsize(struct crypto_aead *tfm, unsigned int authsize)
{
	int err;

	if (authsize > crypto_aead_maxauthsize(tfm))
		return -EINVAL;

	if (crypto_aead_alg(tfm)->setauthsize) {
		err = crypto_aead_alg(tfm)->setauthsize(tfm, authsize);
>>>>>>> v4.9.227
		if (err)
			return err;
	}

<<<<<<< HEAD
	crypto_aead_crt(crt->base)->authsize = authsize;
	crt->authsize = authsize;
=======
	tfm->authsize = authsize;
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL_GPL(crypto_aead_setauthsize);

<<<<<<< HEAD
static unsigned int crypto_aead_ctxsize(struct crypto_alg *alg, u32 type,
					u32 mask)
{
	return alg->cra_ctxsize;
}

static int no_givcrypt(struct aead_givcrypt_request *req)
{
	return -ENOSYS;
}

static int crypto_init_aead_ops(struct crypto_tfm *tfm, u32 type, u32 mask)
{
	struct aead_alg *alg = &tfm->__crt_alg->cra_aead;
	struct aead_tfm *crt = &tfm->crt_aead;

	if (max(alg->maxauthsize, alg->ivsize) > PAGE_SIZE / 8)
		return -EINVAL;

	crt->setkey = tfm->__crt_alg->cra_flags & CRYPTO_ALG_GENIV ?
		      alg->setkey : setkey;
	crt->encrypt = alg->encrypt;
	crt->decrypt = alg->decrypt;
	crt->givencrypt = alg->givencrypt ?: no_givcrypt;
	crt->givdecrypt = alg->givdecrypt ?: no_givcrypt;
	crt->base = __crypto_aead_cast(tfm);
	crt->ivsize = alg->ivsize;
	crt->authsize = alg->maxauthsize;
=======
static void crypto_aead_exit_tfm(struct crypto_tfm *tfm)
{
	struct crypto_aead *aead = __crypto_aead_cast(tfm);
	struct aead_alg *alg = crypto_aead_alg(aead);

	alg->exit(aead);
}

static int crypto_aead_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_aead *aead = __crypto_aead_cast(tfm);
	struct aead_alg *alg = crypto_aead_alg(aead);

	aead->authsize = alg->maxauthsize;

	if (alg->exit)
		aead->base.exit = crypto_aead_exit_tfm;

	if (alg->init)
		return alg->init(aead);
>>>>>>> v4.9.227

	return 0;
}

#ifdef CONFIG_NET
static int crypto_aead_report(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_report_aead raead;
<<<<<<< HEAD
	struct aead_alg *aead = &alg->cra_aead;

	strncpy(raead.type, "aead", sizeof(raead.type));
	strncpy(raead.geniv, aead->geniv ?: "<built-in>", sizeof(raead.geniv));
=======
	struct aead_alg *aead = container_of(alg, struct aead_alg, base);

	strncpy(raead.type, "aead", sizeof(raead.type));
	strncpy(raead.geniv, "<none>", sizeof(raead.geniv));
>>>>>>> v4.9.227

	raead.blocksize = alg->cra_blocksize;
	raead.maxauthsize = aead->maxauthsize;
	raead.ivsize = aead->ivsize;

	if (nla_put(skb, CRYPTOCFGA_REPORT_AEAD,
		    sizeof(struct crypto_report_aead), &raead))
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -EMSGSIZE;
}
#else
static int crypto_aead_report(struct sk_buff *skb, struct crypto_alg *alg)
{
	return -ENOSYS;
}
#endif

static void crypto_aead_show(struct seq_file *m, struct crypto_alg *alg)
	__attribute__ ((unused));
static void crypto_aead_show(struct seq_file *m, struct crypto_alg *alg)
{
<<<<<<< HEAD
	struct aead_alg *aead = &alg->cra_aead;
=======
	struct aead_alg *aead = container_of(alg, struct aead_alg, base);
>>>>>>> v4.9.227

	seq_printf(m, "type         : aead\n");
	seq_printf(m, "async        : %s\n", alg->cra_flags & CRYPTO_ALG_ASYNC ?
					     "yes" : "no");
	seq_printf(m, "blocksize    : %u\n", alg->cra_blocksize);
	seq_printf(m, "ivsize       : %u\n", aead->ivsize);
	seq_printf(m, "maxauthsize  : %u\n", aead->maxauthsize);
<<<<<<< HEAD
	seq_printf(m, "geniv        : %s\n", aead->geniv ?: "<built-in>");
}

const struct crypto_type crypto_aead_type = {
	.ctxsize = crypto_aead_ctxsize,
	.init = crypto_init_aead_ops,
=======
	seq_printf(m, "geniv        : <none>\n");
}

static void crypto_aead_free_instance(struct crypto_instance *inst)
{
	struct aead_instance *aead = aead_instance(inst);

	if (!aead->free) {
		inst->tmpl->free(inst);
		return;
	}

	aead->free(aead);
}

static const struct crypto_type crypto_aead_type = {
	.extsize = crypto_alg_extsize,
	.init_tfm = crypto_aead_init_tfm,
	.free = crypto_aead_free_instance,
>>>>>>> v4.9.227
#ifdef CONFIG_PROC_FS
	.show = crypto_aead_show,
#endif
	.report = crypto_aead_report,
<<<<<<< HEAD
};
EXPORT_SYMBOL_GPL(crypto_aead_type);

static int aead_null_givencrypt(struct aead_givcrypt_request *req)
{
	return crypto_aead_encrypt(&req->areq);
}

static int aead_null_givdecrypt(struct aead_givcrypt_request *req)
{
	return crypto_aead_decrypt(&req->areq);
}

static int crypto_init_nivaead_ops(struct crypto_tfm *tfm, u32 type, u32 mask)
{
	struct aead_alg *alg = &tfm->__crt_alg->cra_aead;
	struct aead_tfm *crt = &tfm->crt_aead;

	if (max(alg->maxauthsize, alg->ivsize) > PAGE_SIZE / 8)
		return -EINVAL;

	crt->setkey = setkey;
	crt->encrypt = alg->encrypt;
	crt->decrypt = alg->decrypt;
	if (!alg->ivsize) {
		crt->givencrypt = aead_null_givencrypt;
		crt->givdecrypt = aead_null_givdecrypt;
	}
	crt->base = __crypto_aead_cast(tfm);
	crt->ivsize = alg->ivsize;
	crt->authsize = alg->maxauthsize;

	return 0;
}

#ifdef CONFIG_NET
static int crypto_nivaead_report(struct sk_buff *skb, struct crypto_alg *alg)
{
	struct crypto_report_aead raead;
	struct aead_alg *aead = &alg->cra_aead;

	strncpy(raead.type, "nivaead", sizeof(raead.type));
	strncpy(raead.geniv, aead->geniv, sizeof(raead.geniv));

	raead.blocksize = alg->cra_blocksize;
	raead.maxauthsize = aead->maxauthsize;
	raead.ivsize = aead->ivsize;

	if (nla_put(skb, CRYPTOCFGA_REPORT_AEAD,
		    sizeof(struct crypto_report_aead), &raead))
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -EMSGSIZE;
}
#else
static int crypto_nivaead_report(struct sk_buff *skb, struct crypto_alg *alg)
{
	return -ENOSYS;
}
#endif


static void crypto_nivaead_show(struct seq_file *m, struct crypto_alg *alg)
	__attribute__ ((unused));
static void crypto_nivaead_show(struct seq_file *m, struct crypto_alg *alg)
{
	struct aead_alg *aead = &alg->cra_aead;

	seq_printf(m, "type         : nivaead\n");
	seq_printf(m, "async        : %s\n", alg->cra_flags & CRYPTO_ALG_ASYNC ?
					     "yes" : "no");
	seq_printf(m, "blocksize    : %u\n", alg->cra_blocksize);
	seq_printf(m, "ivsize       : %u\n", aead->ivsize);
	seq_printf(m, "maxauthsize  : %u\n", aead->maxauthsize);
	seq_printf(m, "geniv        : %s\n", aead->geniv);
}

const struct crypto_type crypto_nivaead_type = {
	.ctxsize = crypto_aead_ctxsize,
	.init = crypto_init_nivaead_ops,
#ifdef CONFIG_PROC_FS
	.show = crypto_nivaead_show,
#endif
	.report = crypto_nivaead_report,
};
EXPORT_SYMBOL_GPL(crypto_nivaead_type);

static int crypto_grab_nivaead(struct crypto_aead_spawn *spawn,
			       const char *name, u32 type, u32 mask)
{
	struct crypto_alg *alg;
	int err;

	type &= ~(CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_GENIV);
	type |= CRYPTO_ALG_TYPE_AEAD;
	mask |= CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_GENIV;

	alg = crypto_alg_mod_lookup(name, type, mask);
	if (IS_ERR(alg))
		return PTR_ERR(alg);

	err = crypto_init_spawn(&spawn->base, alg, spawn->base.inst, mask);
	crypto_mod_put(alg);
	return err;
}

struct crypto_instance *aead_geniv_alloc(struct crypto_template *tmpl,
					 struct rtattr **tb, u32 type,
					 u32 mask)
=======
	.maskclear = ~CRYPTO_ALG_TYPE_MASK,
	.maskset = CRYPTO_ALG_TYPE_MASK,
	.type = CRYPTO_ALG_TYPE_AEAD,
	.tfmsize = offsetof(struct crypto_aead, base),
};

static int aead_geniv_setkey(struct crypto_aead *tfm,
			     const u8 *key, unsigned int keylen)
{
	struct aead_geniv_ctx *ctx = crypto_aead_ctx(tfm);

	return crypto_aead_setkey(ctx->child, key, keylen);
}

static int aead_geniv_setauthsize(struct crypto_aead *tfm,
				  unsigned int authsize)
{
	struct aead_geniv_ctx *ctx = crypto_aead_ctx(tfm);

	return crypto_aead_setauthsize(ctx->child, authsize);
}

struct aead_instance *aead_geniv_alloc(struct crypto_template *tmpl,
				       struct rtattr **tb, u32 type, u32 mask)
>>>>>>> v4.9.227
{
	const char *name;
	struct crypto_aead_spawn *spawn;
	struct crypto_attr_type *algt;
<<<<<<< HEAD
	struct crypto_instance *inst;
	struct crypto_alg *alg;
=======
	struct aead_instance *inst;
	struct aead_alg *alg;
	unsigned int ivsize;
	unsigned int maxauthsize;
>>>>>>> v4.9.227
	int err;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
		return ERR_CAST(algt);

<<<<<<< HEAD
	if ((algt->type ^ (CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_GENIV)) &
	    algt->mask)
=======
	if ((algt->type ^ CRYPTO_ALG_TYPE_AEAD) & algt->mask)
>>>>>>> v4.9.227
		return ERR_PTR(-EINVAL);

	name = crypto_attr_alg_name(tb[1]);
	if (IS_ERR(name))
		return ERR_CAST(name);

	inst = kzalloc(sizeof(*inst) + sizeof(*spawn), GFP_KERNEL);
	if (!inst)
		return ERR_PTR(-ENOMEM);

<<<<<<< HEAD
	spawn = crypto_instance_ctx(inst);
=======
	spawn = aead_instance_ctx(inst);
>>>>>>> v4.9.227

	/* Ignore async algorithms if necessary. */
	mask |= crypto_requires_sync(algt->type, algt->mask);

<<<<<<< HEAD
	crypto_set_aead_spawn(spawn, inst);
	err = crypto_grab_nivaead(spawn, name, type, mask);
	if (err)
		goto err_free_inst;

	alg = crypto_aead_spawn_alg(spawn);

	err = -EINVAL;
	if (!alg->cra_aead.ivsize)
		goto err_drop_alg;

	/*
	 * This is only true if we're constructing an algorithm with its
	 * default IV generator.  For the default generator we elide the
	 * template name and double-check the IV generator.
	 */
	if (algt->mask & CRYPTO_ALG_GENIV) {
		if (strcmp(tmpl->name, alg->cra_aead.geniv))
			goto err_drop_alg;

		memcpy(inst->alg.cra_name, alg->cra_name, CRYPTO_MAX_ALG_NAME);
		memcpy(inst->alg.cra_driver_name, alg->cra_driver_name,
		       CRYPTO_MAX_ALG_NAME);
	} else {
		err = -ENAMETOOLONG;
		if (snprintf(inst->alg.cra_name, CRYPTO_MAX_ALG_NAME,
			     "%s(%s)", tmpl->name, alg->cra_name) >=
		    CRYPTO_MAX_ALG_NAME)
			goto err_drop_alg;
		if (snprintf(inst->alg.cra_driver_name, CRYPTO_MAX_ALG_NAME,
			     "%s(%s)", tmpl->name, alg->cra_driver_name) >=
		    CRYPTO_MAX_ALG_NAME)
			goto err_drop_alg;
	}

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_GENIV;
	inst->alg.cra_flags |= alg->cra_flags & CRYPTO_ALG_ASYNC;
	inst->alg.cra_priority = alg->cra_priority;
	inst->alg.cra_blocksize = alg->cra_blocksize;
	inst->alg.cra_alignmask = alg->cra_alignmask;
	inst->alg.cra_type = &crypto_aead_type;

	inst->alg.cra_aead.ivsize = alg->cra_aead.ivsize;
	inst->alg.cra_aead.maxauthsize = alg->cra_aead.maxauthsize;
	inst->alg.cra_aead.geniv = alg->cra_aead.geniv;

	inst->alg.cra_aead.setkey = alg->cra_aead.setkey;
	inst->alg.cra_aead.setauthsize = alg->cra_aead.setauthsize;
	inst->alg.cra_aead.encrypt = alg->cra_aead.encrypt;
	inst->alg.cra_aead.decrypt = alg->cra_aead.decrypt;
=======
	crypto_set_aead_spawn(spawn, aead_crypto_instance(inst));
	err = crypto_grab_aead(spawn, name, type, mask);
	if (err)
		goto err_free_inst;

	alg = crypto_spawn_aead_alg(spawn);

	ivsize = crypto_aead_alg_ivsize(alg);
	maxauthsize = crypto_aead_alg_maxauthsize(alg);

	err = -EINVAL;
	if (ivsize < sizeof(u64))
		goto err_drop_alg;

	err = -ENAMETOOLONG;
	if (snprintf(inst->alg.base.cra_name, CRYPTO_MAX_ALG_NAME,
		     "%s(%s)", tmpl->name, alg->base.cra_name) >=
	    CRYPTO_MAX_ALG_NAME)
		goto err_drop_alg;
	if (snprintf(inst->alg.base.cra_driver_name, CRYPTO_MAX_ALG_NAME,
		     "%s(%s)", tmpl->name, alg->base.cra_driver_name) >=
	    CRYPTO_MAX_ALG_NAME)
		goto err_drop_alg;

	inst->alg.base.cra_flags = alg->base.cra_flags & CRYPTO_ALG_ASYNC;
	inst->alg.base.cra_priority = alg->base.cra_priority;
	inst->alg.base.cra_blocksize = alg->base.cra_blocksize;
	inst->alg.base.cra_alignmask = alg->base.cra_alignmask;
	inst->alg.base.cra_ctxsize = sizeof(struct aead_geniv_ctx);

	inst->alg.setkey = aead_geniv_setkey;
	inst->alg.setauthsize = aead_geniv_setauthsize;

	inst->alg.ivsize = ivsize;
	inst->alg.maxauthsize = maxauthsize;
>>>>>>> v4.9.227

out:
	return inst;

err_drop_alg:
	crypto_drop_aead(spawn);
err_free_inst:
	kfree(inst);
	inst = ERR_PTR(err);
	goto out;
}
EXPORT_SYMBOL_GPL(aead_geniv_alloc);

<<<<<<< HEAD
void aead_geniv_free(struct crypto_instance *inst)
{
	crypto_drop_aead(crypto_instance_ctx(inst));
=======
void aead_geniv_free(struct aead_instance *inst)
{
	crypto_drop_aead(aead_instance_ctx(inst));
>>>>>>> v4.9.227
	kfree(inst);
}
EXPORT_SYMBOL_GPL(aead_geniv_free);

<<<<<<< HEAD
int aead_geniv_init(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = (void *)tfm->__crt_alg;
	struct crypto_aead *aead;

	aead = crypto_spawn_aead(crypto_instance_ctx(inst));
	if (IS_ERR(aead))
		return PTR_ERR(aead);

	tfm->crt_aead.base = aead;
	tfm->crt_aead.reqsize += crypto_aead_reqsize(aead);

	return 0;
}
EXPORT_SYMBOL_GPL(aead_geniv_init);

void aead_geniv_exit(struct crypto_tfm *tfm)
{
	crypto_free_aead(tfm->crt_aead.base);
}
EXPORT_SYMBOL_GPL(aead_geniv_exit);

static int crypto_nivaead_default(struct crypto_alg *alg, u32 type, u32 mask)
{
	struct rtattr *tb[3];
	struct {
		struct rtattr attr;
		struct crypto_attr_type data;
	} ptype;
	struct {
		struct rtattr attr;
		struct crypto_attr_alg data;
	} palg;
	struct crypto_template *tmpl;
	struct crypto_instance *inst;
	struct crypto_alg *larval;
	const char *geniv;
	int err;

	larval = crypto_larval_lookup(alg->cra_driver_name,
				      CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_GENIV,
				      CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_GENIV);
	err = PTR_ERR(larval);
	if (IS_ERR(larval))
		goto out;

	err = -EAGAIN;
	if (!crypto_is_larval(larval))
		goto drop_larval;

	ptype.attr.rta_len = sizeof(ptype);
	ptype.attr.rta_type = CRYPTOA_TYPE;
	ptype.data.type = type | CRYPTO_ALG_GENIV;
	/* GENIV tells the template that we're making a default geniv. */
	ptype.data.mask = mask | CRYPTO_ALG_GENIV;
	tb[0] = &ptype.attr;

	palg.attr.rta_len = sizeof(palg);
	palg.attr.rta_type = CRYPTOA_ALG;
	/* Must use the exact name to locate ourselves. */
	memcpy(palg.data.name, alg->cra_driver_name, CRYPTO_MAX_ALG_NAME);
	tb[1] = &palg.attr;

	tb[2] = NULL;

	geniv = alg->cra_aead.geniv;

	tmpl = crypto_lookup_template(geniv);
	err = -ENOENT;
	if (!tmpl)
		goto kill_larval;

	inst = tmpl->alloc(tb);
	err = PTR_ERR(inst);
	if (IS_ERR(inst))
		goto put_tmpl;

	if ((err = crypto_register_instance(tmpl, inst))) {
		tmpl->free(inst);
		goto put_tmpl;
	}

	/* Redo the lookup to use the instance we just registered. */
	err = -EAGAIN;

put_tmpl:
	crypto_tmpl_put(tmpl);
kill_larval:
	crypto_larval_kill(larval);
drop_larval:
	crypto_mod_put(larval);
out:
	crypto_mod_put(alg);
	return err;
}

struct crypto_alg *crypto_lookup_aead(const char *name, u32 type, u32 mask)
{
	struct crypto_alg *alg;

	alg = crypto_alg_mod_lookup(name, type, mask);
	if (IS_ERR(alg))
		return alg;

	if (alg->cra_type == &crypto_aead_type)
		return alg;

	if (!alg->cra_aead.ivsize)
		return alg;

	crypto_mod_put(alg);
	alg = crypto_alg_mod_lookup(name, type | CRYPTO_ALG_TESTED,
				    mask & ~CRYPTO_ALG_TESTED);
	if (IS_ERR(alg))
		return alg;

	if (alg->cra_type == &crypto_aead_type) {
		if ((alg->cra_flags ^ type ^ ~mask) & CRYPTO_ALG_TESTED) {
			crypto_mod_put(alg);
			alg = ERR_PTR(-ENOENT);
		}
		return alg;
	}

	BUG_ON(!alg->cra_aead.ivsize);

	return ERR_PTR(crypto_nivaead_default(alg, type, mask));
}
EXPORT_SYMBOL_GPL(crypto_lookup_aead);
=======
int aead_init_geniv(struct crypto_aead *aead)
{
	struct aead_geniv_ctx *ctx = crypto_aead_ctx(aead);
	struct aead_instance *inst = aead_alg_instance(aead);
	struct crypto_aead *child;
	int err;

	spin_lock_init(&ctx->lock);

	err = crypto_get_default_rng();
	if (err)
		goto out;

	err = crypto_rng_get_bytes(crypto_default_rng, ctx->salt,
				   crypto_aead_ivsize(aead));
	crypto_put_default_rng();
	if (err)
		goto out;

	ctx->sknull = crypto_get_default_null_skcipher2();
	err = PTR_ERR(ctx->sknull);
	if (IS_ERR(ctx->sknull))
		goto out;

	child = crypto_spawn_aead(aead_instance_ctx(inst));
	err = PTR_ERR(child);
	if (IS_ERR(child))
		goto drop_null;

	ctx->child = child;
	crypto_aead_set_reqsize(aead, crypto_aead_reqsize(child) +
				      sizeof(struct aead_request));

	err = 0;

out:
	return err;

drop_null:
	crypto_put_default_null_skcipher2();
	goto out;
}
EXPORT_SYMBOL_GPL(aead_init_geniv);

void aead_exit_geniv(struct crypto_aead *tfm)
{
	struct aead_geniv_ctx *ctx = crypto_aead_ctx(tfm);

	crypto_free_aead(ctx->child);
	crypto_put_default_null_skcipher2();
}
EXPORT_SYMBOL_GPL(aead_exit_geniv);
>>>>>>> v4.9.227

int crypto_grab_aead(struct crypto_aead_spawn *spawn, const char *name,
		     u32 type, u32 mask)
{
<<<<<<< HEAD
	struct crypto_alg *alg;
	int err;

	type &= ~(CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_GENIV);
	type |= CRYPTO_ALG_TYPE_AEAD;
	mask &= ~(CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_GENIV);
	mask |= CRYPTO_ALG_TYPE_MASK;

	alg = crypto_lookup_aead(name, type, mask);
	if (IS_ERR(alg))
		return PTR_ERR(alg);

	err = crypto_init_spawn(&spawn->base, alg, spawn->base.inst, mask);
	crypto_mod_put(alg);
	return err;
=======
	spawn->base.frontend = &crypto_aead_type;
	return crypto_grab_spawn(&spawn->base, name, type, mask);
>>>>>>> v4.9.227
}
EXPORT_SYMBOL_GPL(crypto_grab_aead);

struct crypto_aead *crypto_alloc_aead(const char *alg_name, u32 type, u32 mask)
{
<<<<<<< HEAD
	struct crypto_tfm *tfm;
	int err;

	type &= ~(CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_GENIV);
	type |= CRYPTO_ALG_TYPE_AEAD;
	mask &= ~(CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_GENIV);
	mask |= CRYPTO_ALG_TYPE_MASK;

	for (;;) {
		struct crypto_alg *alg;

		alg = crypto_lookup_aead(alg_name, type, mask);
		if (IS_ERR(alg)) {
			err = PTR_ERR(alg);
			goto err;
		}

		tfm = __crypto_alloc_tfm(alg, type, mask);
		if (!IS_ERR(tfm))
			return __crypto_aead_cast(tfm);

		crypto_mod_put(alg);
		err = PTR_ERR(tfm);

err:
		if (err != -EAGAIN)
			break;
		if (signal_pending(current)) {
			err = -EINTR;
			break;
		}
	}

	return ERR_PTR(err);
}
EXPORT_SYMBOL_GPL(crypto_alloc_aead);

=======
	return crypto_alloc_tfm(alg_name, &crypto_aead_type, type, mask);
}
EXPORT_SYMBOL_GPL(crypto_alloc_aead);

static int aead_prepare_alg(struct aead_alg *alg)
{
	struct crypto_alg *base = &alg->base;

	if (max3(alg->maxauthsize, alg->ivsize, alg->chunksize) >
	    PAGE_SIZE / 8)
		return -EINVAL;

	if (!alg->chunksize)
		alg->chunksize = base->cra_blocksize;

	base->cra_type = &crypto_aead_type;
	base->cra_flags &= ~CRYPTO_ALG_TYPE_MASK;
	base->cra_flags |= CRYPTO_ALG_TYPE_AEAD;

	return 0;
}

int crypto_register_aead(struct aead_alg *alg)
{
	struct crypto_alg *base = &alg->base;
	int err;

	err = aead_prepare_alg(alg);
	if (err)
		return err;

	return crypto_register_alg(base);
}
EXPORT_SYMBOL_GPL(crypto_register_aead);

void crypto_unregister_aead(struct aead_alg *alg)
{
	crypto_unregister_alg(&alg->base);
}
EXPORT_SYMBOL_GPL(crypto_unregister_aead);

int crypto_register_aeads(struct aead_alg *algs, int count)
{
	int i, ret;

	for (i = 0; i < count; i++) {
		ret = crypto_register_aead(&algs[i]);
		if (ret)
			goto err;
	}

	return 0;

err:
	for (--i; i >= 0; --i)
		crypto_unregister_aead(&algs[i]);

	return ret;
}
EXPORT_SYMBOL_GPL(crypto_register_aeads);

void crypto_unregister_aeads(struct aead_alg *algs, int count)
{
	int i;

	for (i = count - 1; i >= 0; --i)
		crypto_unregister_aead(&algs[i]);
}
EXPORT_SYMBOL_GPL(crypto_unregister_aeads);

int aead_register_instance(struct crypto_template *tmpl,
			   struct aead_instance *inst)
{
	int err;

	err = aead_prepare_alg(&inst->alg);
	if (err)
		return err;

	return crypto_register_instance(tmpl, aead_crypto_instance(inst));
}
EXPORT_SYMBOL_GPL(aead_register_instance);

>>>>>>> v4.9.227
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Authenticated Encryption with Associated Data (AEAD)");
