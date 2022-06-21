/*
 * Authenc: Simple AEAD wrapper for IPsec
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
#include <crypto/aead.h>
#include <crypto/internal/hash.h>
#include <crypto/internal/skcipher.h>
#include <crypto/authenc.h>
=======
#include <crypto/internal/aead.h>
#include <crypto/internal/hash.h>
#include <crypto/internal/skcipher.h>
#include <crypto/authenc.h>
#include <crypto/null.h>
>>>>>>> v4.9.227
#include <crypto/scatterwalk.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rtnetlink.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

<<<<<<< HEAD
typedef u8 *(*authenc_ahash_t)(struct aead_request *req, unsigned int flags);

struct authenc_instance_ctx {
	struct crypto_ahash_spawn auth;
	struct crypto_skcipher_spawn enc;
};

struct crypto_authenc_ctx {
	unsigned int reqoff;
	struct crypto_ahash *auth;
	struct crypto_ablkcipher *enc;
};

struct authenc_request_ctx {
	unsigned int cryptlen;
	struct scatterlist *sg;
	struct scatterlist asg[2];
	struct scatterlist cipher[2];
	crypto_completion_t complete;
	crypto_completion_t update_complete;
=======
struct authenc_instance_ctx {
	struct crypto_ahash_spawn auth;
	struct crypto_skcipher_spawn enc;
	unsigned int reqoff;
};

struct crypto_authenc_ctx {
	struct crypto_ahash *auth;
	struct crypto_skcipher *enc;
	struct crypto_skcipher *null;
};

struct authenc_request_ctx {
	struct scatterlist src[2];
	struct scatterlist dst[2];
>>>>>>> v4.9.227
	char tail[];
};

static void authenc_request_complete(struct aead_request *req, int err)
{
	if (err != -EINPROGRESS)
		aead_request_complete(req, err);
}

int crypto_authenc_extractkeys(struct crypto_authenc_keys *keys, const u8 *key,
			       unsigned int keylen)
{
	struct rtattr *rta = (struct rtattr *)key;
	struct crypto_authenc_key_param *param;

	if (!RTA_OK(rta, keylen))
		return -EINVAL;
	if (rta->rta_type != CRYPTO_AUTHENC_KEYA_PARAM)
		return -EINVAL;
<<<<<<< HEAD
	if (RTA_PAYLOAD(rta) < sizeof(*param))
		return -EINVAL;
=======

	/*
	 * RTA_OK() didn't align the rtattr's payload when validating that it
	 * fits in the buffer.  Yet, the keys should start on the next 4-byte
	 * aligned boundary.  To avoid confusion, require that the rtattr
	 * payload be exactly the param struct, which has a 4-byte aligned size.
	 */
	if (RTA_PAYLOAD(rta) != sizeof(*param))
		return -EINVAL;
	BUILD_BUG_ON(sizeof(*param) % RTA_ALIGNTO);
>>>>>>> v4.9.227

	param = RTA_DATA(rta);
	keys->enckeylen = be32_to_cpu(param->enckeylen);

<<<<<<< HEAD
	key += RTA_ALIGN(rta->rta_len);
	keylen -= RTA_ALIGN(rta->rta_len);
=======
	key += rta->rta_len;
	keylen -= rta->rta_len;
>>>>>>> v4.9.227

	if (keylen < keys->enckeylen)
		return -EINVAL;

	keys->authkeylen = keylen - keys->enckeylen;
	keys->authkey = key;
	keys->enckey = key + keys->authkeylen;

	return 0;
}
EXPORT_SYMBOL_GPL(crypto_authenc_extractkeys);

static int crypto_authenc_setkey(struct crypto_aead *authenc, const u8 *key,
				 unsigned int keylen)
{
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct crypto_ahash *auth = ctx->auth;
<<<<<<< HEAD
	struct crypto_ablkcipher *enc = ctx->enc;
=======
	struct crypto_skcipher *enc = ctx->enc;
>>>>>>> v4.9.227
	struct crypto_authenc_keys keys;
	int err = -EINVAL;

	if (crypto_authenc_extractkeys(&keys, key, keylen) != 0)
		goto badkey;

	crypto_ahash_clear_flags(auth, CRYPTO_TFM_REQ_MASK);
	crypto_ahash_set_flags(auth, crypto_aead_get_flags(authenc) &
				    CRYPTO_TFM_REQ_MASK);
	err = crypto_ahash_setkey(auth, keys.authkey, keys.authkeylen);
	crypto_aead_set_flags(authenc, crypto_ahash_get_flags(auth) &
				       CRYPTO_TFM_RES_MASK);

	if (err)
		goto out;

<<<<<<< HEAD
	crypto_ablkcipher_clear_flags(enc, CRYPTO_TFM_REQ_MASK);
	crypto_ablkcipher_set_flags(enc, crypto_aead_get_flags(authenc) &
					 CRYPTO_TFM_REQ_MASK);
	err = crypto_ablkcipher_setkey(enc, keys.enckey, keys.enckeylen);
	crypto_aead_set_flags(authenc, crypto_ablkcipher_get_flags(enc) &
=======
	crypto_skcipher_clear_flags(enc, CRYPTO_TFM_REQ_MASK);
	crypto_skcipher_set_flags(enc, crypto_aead_get_flags(authenc) &
				       CRYPTO_TFM_REQ_MASK);
	err = crypto_skcipher_setkey(enc, keys.enckey, keys.enckeylen);
	crypto_aead_set_flags(authenc, crypto_skcipher_get_flags(enc) &
>>>>>>> v4.9.227
				       CRYPTO_TFM_RES_MASK);

out:
	memzero_explicit(&keys, sizeof(keys));
	return err;

badkey:
	crypto_aead_set_flags(authenc, CRYPTO_TFM_RES_BAD_KEY_LEN);
	goto out;
}

<<<<<<< HEAD
static void authenc_geniv_ahash_update_done(struct crypto_async_request *areq,
					    int err)
{
	struct aead_request *req = areq->data;
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ctx->reqoff);

	if (err)
		goto out;

	ahash_request_set_crypt(ahreq, areq_ctx->sg, ahreq->result,
				areq_ctx->cryptlen);
	ahash_request_set_callback(ahreq, aead_request_flags(req) &
					  CRYPTO_TFM_REQ_MAY_SLEEP,
				   areq_ctx->complete, req);

	err = crypto_ahash_finup(ahreq);
	if (err)
		goto out;

	scatterwalk_map_and_copy(ahreq->result, areq_ctx->sg,
				 areq_ctx->cryptlen,
				 crypto_aead_authsize(authenc), 1);

out:
	authenc_request_complete(req, err);
}

=======
>>>>>>> v4.9.227
static void authenc_geniv_ahash_done(struct crypto_async_request *areq, int err)
{
	struct aead_request *req = areq->data;
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
<<<<<<< HEAD
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ctx->reqoff);
=======
	struct aead_instance *inst = aead_alg_instance(authenc);
	struct authenc_instance_ctx *ictx = aead_instance_ctx(inst);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ictx->reqoff);
>>>>>>> v4.9.227

	if (err)
		goto out;

<<<<<<< HEAD
	scatterwalk_map_and_copy(ahreq->result, areq_ctx->sg,
				 areq_ctx->cryptlen,
=======
	scatterwalk_map_and_copy(ahreq->result, req->dst,
				 req->assoclen + req->cryptlen,
>>>>>>> v4.9.227
				 crypto_aead_authsize(authenc), 1);

out:
	aead_request_complete(req, err);
}

<<<<<<< HEAD
static void authenc_verify_ahash_update_done(struct crypto_async_request *areq,
					     int err)
{
	u8 *ihash;
	unsigned int authsize;
	struct ablkcipher_request *abreq;
	struct aead_request *req = areq->data;
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ctx->reqoff);
	unsigned int cryptlen = req->cryptlen;

	if (err)
		goto out;

	ahash_request_set_crypt(ahreq, areq_ctx->sg, ahreq->result,
				areq_ctx->cryptlen);
	ahash_request_set_callback(ahreq, aead_request_flags(req) &
					  CRYPTO_TFM_REQ_MAY_SLEEP,
				   areq_ctx->complete, req);

	err = crypto_ahash_finup(ahreq);
	if (err)
		goto out;

	authsize = crypto_aead_authsize(authenc);
	cryptlen -= authsize;
	ihash = ahreq->result + authsize;
	scatterwalk_map_and_copy(ihash, areq_ctx->sg, areq_ctx->cryptlen,
				 authsize, 0);

	err = crypto_memneq(ihash, ahreq->result, authsize) ? -EBADMSG : 0;
	if (err)
		goto out;

	abreq = aead_request_ctx(req);
	ablkcipher_request_set_tfm(abreq, ctx->enc);
	ablkcipher_request_set_callback(abreq, aead_request_flags(req),
					req->base.complete, req->base.data);
	ablkcipher_request_set_crypt(abreq, req->src, req->dst,
				     cryptlen, req->iv);

	err = crypto_ablkcipher_decrypt(abreq);

out:
	authenc_request_complete(req, err);
}

static void authenc_verify_ahash_done(struct crypto_async_request *areq,
				      int err)
{
	u8 *ihash;
	unsigned int authsize;
	struct ablkcipher_request *abreq;
	struct aead_request *req = areq->data;
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ctx->reqoff);
	unsigned int cryptlen = req->cryptlen;

	if (err)
		goto out;

	authsize = crypto_aead_authsize(authenc);
	cryptlen -= authsize;
	ihash = ahreq->result + authsize;
	scatterwalk_map_and_copy(ihash, areq_ctx->sg, areq_ctx->cryptlen,
				 authsize, 0);

	err = crypto_memneq(ihash, ahreq->result, authsize) ? -EBADMSG : 0;
	if (err)
		goto out;

	abreq = aead_request_ctx(req);
	ablkcipher_request_set_tfm(abreq, ctx->enc);
	ablkcipher_request_set_callback(abreq, aead_request_flags(req),
					req->base.complete, req->base.data);
	ablkcipher_request_set_crypt(abreq, req->src, req->dst,
				     cryptlen, req->iv);

	err = crypto_ablkcipher_decrypt(abreq);

out:
	authenc_request_complete(req, err);
}

static u8 *crypto_authenc_ahash_fb(struct aead_request *req, unsigned int flags)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct crypto_ahash *auth = ctx->auth;
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ctx->reqoff);
	u8 *hash = areq_ctx->tail;
	int err;

	hash = (u8 *)ALIGN((unsigned long)hash + crypto_ahash_alignmask(auth),
			    crypto_ahash_alignmask(auth) + 1);

	ahash_request_set_tfm(ahreq, auth);

	err = crypto_ahash_init(ahreq);
	if (err)
		return ERR_PTR(err);

	ahash_request_set_crypt(ahreq, req->assoc, hash, req->assoclen);
	ahash_request_set_callback(ahreq, aead_request_flags(req) & flags,
				   areq_ctx->update_complete, req);

	err = crypto_ahash_update(ahreq);
	if (err)
		return ERR_PTR(err);

	ahash_request_set_crypt(ahreq, areq_ctx->sg, hash,
				areq_ctx->cryptlen);
	ahash_request_set_callback(ahreq, aead_request_flags(req) & flags,
				   areq_ctx->complete, req);

	err = crypto_ahash_finup(ahreq);
	if (err)
		return ERR_PTR(err);

	return hash;
}

static u8 *crypto_authenc_ahash(struct aead_request *req, unsigned int flags)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct crypto_ahash *auth = ctx->auth;
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ctx->reqoff);
=======
static int crypto_authenc_genicv(struct aead_request *req, unsigned int flags)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct aead_instance *inst = aead_alg_instance(authenc);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct authenc_instance_ctx *ictx = aead_instance_ctx(inst);
	struct crypto_ahash *auth = ctx->auth;
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ictx->reqoff);
>>>>>>> v4.9.227
	u8 *hash = areq_ctx->tail;
	int err;

	hash = (u8 *)ALIGN((unsigned long)hash + crypto_ahash_alignmask(auth),
			   crypto_ahash_alignmask(auth) + 1);

	ahash_request_set_tfm(ahreq, auth);
<<<<<<< HEAD
	ahash_request_set_crypt(ahreq, areq_ctx->sg, hash,
				areq_ctx->cryptlen);
	ahash_request_set_callback(ahreq, aead_request_flags(req) & flags,
				   areq_ctx->complete, req);

	err = crypto_ahash_digest(ahreq);
	if (err)
		return ERR_PTR(err);

	return hash;
}

static int crypto_authenc_genicv(struct aead_request *req, u8 *iv,
				 unsigned int flags)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct scatterlist *dst = req->dst;
	struct scatterlist *assoc = req->assoc;
	struct scatterlist *cipher = areq_ctx->cipher;
	struct scatterlist *asg = areq_ctx->asg;
	unsigned int ivsize = crypto_aead_ivsize(authenc);
	unsigned int cryptlen = req->cryptlen;
	authenc_ahash_t authenc_ahash_fn = crypto_authenc_ahash_fb;
	struct page *dstp;
	u8 *vdst;
	u8 *hash;

	dstp = sg_page(dst);
	vdst = PageHighMem(dstp) ? NULL : page_address(dstp) + dst->offset;

	if (ivsize) {
		sg_init_table(cipher, 2);
		sg_set_buf(cipher, iv, ivsize);
		scatterwalk_crypto_chain(cipher, dst, vdst == iv + ivsize, 2);
		dst = cipher;
		cryptlen += ivsize;
	}

	if (req->assoclen && sg_is_last(assoc)) {
		authenc_ahash_fn = crypto_authenc_ahash;
		sg_init_table(asg, 2);
		sg_set_page(asg, sg_page(assoc), assoc->length, assoc->offset);
		scatterwalk_crypto_chain(asg, dst, 0, 2);
		dst = asg;
		cryptlen += req->assoclen;
	}

	areq_ctx->cryptlen = cryptlen;
	areq_ctx->sg = dst;

	areq_ctx->complete = authenc_geniv_ahash_done;
	areq_ctx->update_complete = authenc_geniv_ahash_update_done;

	hash = authenc_ahash_fn(req, flags);
	if (IS_ERR(hash))
		return PTR_ERR(hash);

	scatterwalk_map_and_copy(hash, dst, cryptlen,
				 crypto_aead_authsize(authenc), 1);
=======
	ahash_request_set_crypt(ahreq, req->dst, hash,
				req->assoclen + req->cryptlen);
	ahash_request_set_callback(ahreq, flags,
				   authenc_geniv_ahash_done, req);

	err = crypto_ahash_digest(ahreq);
	if (err)
		return err;

	scatterwalk_map_and_copy(hash, req->dst, req->assoclen + req->cryptlen,
				 crypto_aead_authsize(authenc), 1);

>>>>>>> v4.9.227
	return 0;
}

static void crypto_authenc_encrypt_done(struct crypto_async_request *req,
					int err)
{
	struct aead_request *areq = req->data;

<<<<<<< HEAD
	if (!err) {
		struct crypto_aead *authenc = crypto_aead_reqtfm(areq);
		struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
		struct authenc_request_ctx *areq_ctx = aead_request_ctx(areq);
		struct ablkcipher_request *abreq = (void *)(areq_ctx->tail
							    + ctx->reqoff);
		u8 *iv = (u8 *)abreq - crypto_ablkcipher_ivsize(ctx->enc);

		err = crypto_authenc_genicv(areq, iv, 0);
	}

	authenc_request_complete(areq, err);
}

static int crypto_authenc_encrypt(struct aead_request *req)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct crypto_ablkcipher *enc = ctx->enc;
	struct scatterlist *dst = req->dst;
	unsigned int cryptlen = req->cryptlen;
	struct ablkcipher_request *abreq = (void *)(areq_ctx->tail
						    + ctx->reqoff);
	u8 *iv = (u8 *)abreq - crypto_ablkcipher_ivsize(enc);
	int err;

	ablkcipher_request_set_tfm(abreq, enc);
	ablkcipher_request_set_callback(abreq, aead_request_flags(req),
					crypto_authenc_encrypt_done, req);
	ablkcipher_request_set_crypt(abreq, req->src, dst, cryptlen, req->iv);

	memcpy(iv, req->iv, crypto_aead_ivsize(authenc));

	err = crypto_ablkcipher_encrypt(abreq);
	if (err)
		return err;

	return crypto_authenc_genicv(req, iv, CRYPTO_TFM_REQ_MAY_SLEEP);
}

static void crypto_authenc_givencrypt_done(struct crypto_async_request *req,
					   int err)
{
	struct aead_request *areq = req->data;

	if (!err) {
		struct skcipher_givcrypt_request *greq = aead_request_ctx(areq);

		err = crypto_authenc_genicv(areq, greq->giv, 0);
	}

	authenc_request_complete(areq, err);
}

static int crypto_authenc_givencrypt(struct aead_givcrypt_request *req)
{
	struct crypto_aead *authenc = aead_givcrypt_reqtfm(req);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct aead_request *areq = &req->areq;
	struct skcipher_givcrypt_request *greq = aead_request_ctx(areq);
	u8 *iv = req->giv;
	int err;

	skcipher_givcrypt_set_tfm(greq, ctx->enc);
	skcipher_givcrypt_set_callback(greq, aead_request_flags(areq),
				       crypto_authenc_givencrypt_done, areq);
	skcipher_givcrypt_set_crypt(greq, areq->src, areq->dst, areq->cryptlen,
				    areq->iv);
	skcipher_givcrypt_set_giv(greq, iv, req->seq);

	err = crypto_skcipher_givencrypt(greq);
	if (err)
		return err;

	return crypto_authenc_genicv(areq, iv, CRYPTO_TFM_REQ_MAY_SLEEP);
}

static int crypto_authenc_verify(struct aead_request *req,
				 authenc_ahash_t authenc_ahash_fn)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	u8 *ohash;
	u8 *ihash;
	unsigned int authsize;

	areq_ctx->complete = authenc_verify_ahash_done;
	areq_ctx->update_complete = authenc_verify_ahash_update_done;

	ohash = authenc_ahash_fn(req, CRYPTO_TFM_REQ_MAY_SLEEP);
	if (IS_ERR(ohash))
		return PTR_ERR(ohash);

	authsize = crypto_aead_authsize(authenc);
	ihash = ohash + authsize;
	scatterwalk_map_and_copy(ihash, areq_ctx->sg, areq_ctx->cryptlen,
				 authsize, 0);
	return crypto_memneq(ihash, ohash, authsize) ? -EBADMSG : 0;
}

static int crypto_authenc_iverify(struct aead_request *req, u8 *iv,
				  unsigned int cryptlen)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct scatterlist *src = req->src;
	struct scatterlist *assoc = req->assoc;
	struct scatterlist *cipher = areq_ctx->cipher;
	struct scatterlist *asg = areq_ctx->asg;
	unsigned int ivsize = crypto_aead_ivsize(authenc);
	authenc_ahash_t authenc_ahash_fn = crypto_authenc_ahash_fb;
	struct page *srcp;
	u8 *vsrc;

	srcp = sg_page(src);
	vsrc = PageHighMem(srcp) ? NULL : page_address(srcp) + src->offset;

	if (ivsize) {
		sg_init_table(cipher, 2);
		sg_set_buf(cipher, iv, ivsize);
		scatterwalk_crypto_chain(cipher, src, vsrc == iv + ivsize, 2);
		src = cipher;
		cryptlen += ivsize;
	}

	if (req->assoclen && sg_is_last(assoc)) {
		authenc_ahash_fn = crypto_authenc_ahash;
		sg_init_table(asg, 2);
		sg_set_page(asg, sg_page(assoc), assoc->length, assoc->offset);
		scatterwalk_crypto_chain(asg, src, 0, 2);
		src = asg;
		cryptlen += req->assoclen;
	}

	areq_ctx->cryptlen = cryptlen;
	areq_ctx->sg = src;

	return crypto_authenc_verify(req, authenc_ahash_fn);
=======
	if (err)
		goto out;

	err = crypto_authenc_genicv(areq, 0);

out:
	authenc_request_complete(areq, err);
}

static int crypto_authenc_copy_assoc(struct aead_request *req)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	SKCIPHER_REQUEST_ON_STACK(skreq, ctx->null);

	skcipher_request_set_tfm(skreq, ctx->null);
	skcipher_request_set_callback(skreq, aead_request_flags(req),
				      NULL, NULL);
	skcipher_request_set_crypt(skreq, req->src, req->dst, req->assoclen,
				   NULL);

	return crypto_skcipher_encrypt(skreq);
}

static int crypto_authenc_encrypt(struct aead_request *req)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct aead_instance *inst = aead_alg_instance(authenc);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct authenc_instance_ctx *ictx = aead_instance_ctx(inst);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct crypto_skcipher *enc = ctx->enc;
	unsigned int cryptlen = req->cryptlen;
	struct skcipher_request *skreq = (void *)(areq_ctx->tail +
						  ictx->reqoff);
	struct scatterlist *src, *dst;
	int err;

	src = scatterwalk_ffwd(areq_ctx->src, req->src, req->assoclen);
	dst = src;

	if (req->src != req->dst) {
		err = crypto_authenc_copy_assoc(req);
		if (err)
			return err;

		dst = scatterwalk_ffwd(areq_ctx->dst, req->dst, req->assoclen);
	}

	skcipher_request_set_tfm(skreq, enc);
	skcipher_request_set_callback(skreq, aead_request_flags(req),
				      crypto_authenc_encrypt_done, req);
	skcipher_request_set_crypt(skreq, src, dst, cryptlen, req->iv);

	err = crypto_skcipher_encrypt(skreq);
	if (err)
		return err;

	return crypto_authenc_genicv(req, aead_request_flags(req));
}

static int crypto_authenc_decrypt_tail(struct aead_request *req,
				       unsigned int flags)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct aead_instance *inst = aead_alg_instance(authenc);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct authenc_instance_ctx *ictx = aead_instance_ctx(inst);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ictx->reqoff);
	struct skcipher_request *skreq = (void *)(areq_ctx->tail +
						  ictx->reqoff);
	unsigned int authsize = crypto_aead_authsize(authenc);
	u8 *ihash = ahreq->result + authsize;
	struct scatterlist *src, *dst;

	scatterwalk_map_and_copy(ihash, req->src, ahreq->nbytes, authsize, 0);

	if (crypto_memneq(ihash, ahreq->result, authsize))
		return -EBADMSG;

	src = scatterwalk_ffwd(areq_ctx->src, req->src, req->assoclen);
	dst = src;

	if (req->src != req->dst)
		dst = scatterwalk_ffwd(areq_ctx->dst, req->dst, req->assoclen);

	skcipher_request_set_tfm(skreq, ctx->enc);
	skcipher_request_set_callback(skreq, aead_request_flags(req),
				      req->base.complete, req->base.data);
	skcipher_request_set_crypt(skreq, src, dst,
				   req->cryptlen - authsize, req->iv);

	return crypto_skcipher_decrypt(skreq);
}

static void authenc_verify_ahash_done(struct crypto_async_request *areq,
				      int err)
{
	struct aead_request *req = areq->data;

	if (err)
		goto out;

	err = crypto_authenc_decrypt_tail(req, 0);

out:
	authenc_request_complete(req, err);
>>>>>>> v4.9.227
}

static int crypto_authenc_decrypt(struct aead_request *req)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
<<<<<<< HEAD
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct ablkcipher_request *abreq = aead_request_ctx(req);
	unsigned int cryptlen = req->cryptlen;
	unsigned int authsize = crypto_aead_authsize(authenc);
	u8 *iv = req->iv;
	int err;

	if (cryptlen < authsize)
		return -EINVAL;
	cryptlen -= authsize;

	err = crypto_authenc_iverify(req, iv, cryptlen);
	if (err)
		return err;

	ablkcipher_request_set_tfm(abreq, ctx->enc);
	ablkcipher_request_set_callback(abreq, aead_request_flags(req),
					req->base.complete, req->base.data);
	ablkcipher_request_set_crypt(abreq, req->src, req->dst, cryptlen, iv);

	return crypto_ablkcipher_decrypt(abreq);
}

static int crypto_authenc_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = crypto_tfm_alg_instance(tfm);
	struct authenc_instance_ctx *ictx = crypto_instance_ctx(inst);
	struct crypto_authenc_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_ahash *auth;
	struct crypto_ablkcipher *enc;
=======
	unsigned int authsize = crypto_aead_authsize(authenc);
	struct aead_instance *inst = aead_alg_instance(authenc);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct authenc_instance_ctx *ictx = aead_instance_ctx(inst);
	struct crypto_ahash *auth = ctx->auth;
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ictx->reqoff);
	u8 *hash = areq_ctx->tail;
	int err;

	hash = (u8 *)ALIGN((unsigned long)hash + crypto_ahash_alignmask(auth),
			   crypto_ahash_alignmask(auth) + 1);

	ahash_request_set_tfm(ahreq, auth);
	ahash_request_set_crypt(ahreq, req->src, hash,
				req->assoclen + req->cryptlen - authsize);
	ahash_request_set_callback(ahreq, aead_request_flags(req),
				   authenc_verify_ahash_done, req);

	err = crypto_ahash_digest(ahreq);
	if (err)
		return err;

	return crypto_authenc_decrypt_tail(req, aead_request_flags(req));
}

static int crypto_authenc_init_tfm(struct crypto_aead *tfm)
{
	struct aead_instance *inst = aead_alg_instance(tfm);
	struct authenc_instance_ctx *ictx = aead_instance_ctx(inst);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(tfm);
	struct crypto_ahash *auth;
	struct crypto_skcipher *enc;
	struct crypto_skcipher *null;
>>>>>>> v4.9.227
	int err;

	auth = crypto_spawn_ahash(&ictx->auth);
	if (IS_ERR(auth))
		return PTR_ERR(auth);

<<<<<<< HEAD
	enc = crypto_spawn_skcipher(&ictx->enc);
=======
	enc = crypto_spawn_skcipher2(&ictx->enc);
>>>>>>> v4.9.227
	err = PTR_ERR(enc);
	if (IS_ERR(enc))
		goto err_free_ahash;

<<<<<<< HEAD
	ctx->auth = auth;
	ctx->enc = enc;

	ctx->reqoff = ALIGN(2 * crypto_ahash_digestsize(auth) +
			    crypto_ahash_alignmask(auth),
			    crypto_ahash_alignmask(auth) + 1) +
		      crypto_ablkcipher_ivsize(enc);

	tfm->crt_aead.reqsize = sizeof(struct authenc_request_ctx) +
				ctx->reqoff +
				max_t(unsigned int,
				crypto_ahash_reqsize(auth) +
				sizeof(struct ahash_request),
				sizeof(struct skcipher_givcrypt_request) +
				crypto_ablkcipher_reqsize(enc));

	return 0;

=======
	null = crypto_get_default_null_skcipher2();
	err = PTR_ERR(null);
	if (IS_ERR(null))
		goto err_free_skcipher;

	ctx->auth = auth;
	ctx->enc = enc;
	ctx->null = null;

	crypto_aead_set_reqsize(
		tfm,
		sizeof(struct authenc_request_ctx) +
		ictx->reqoff +
		max_t(unsigned int,
		      crypto_ahash_reqsize(auth) +
		      sizeof(struct ahash_request),
		      sizeof(struct skcipher_request) +
		      crypto_skcipher_reqsize(enc)));

	return 0;

err_free_skcipher:
	crypto_free_skcipher(enc);
>>>>>>> v4.9.227
err_free_ahash:
	crypto_free_ahash(auth);
	return err;
}

<<<<<<< HEAD
static void crypto_authenc_exit_tfm(struct crypto_tfm *tfm)
{
	struct crypto_authenc_ctx *ctx = crypto_tfm_ctx(tfm);

	crypto_free_ahash(ctx->auth);
	crypto_free_ablkcipher(ctx->enc);
}

static struct crypto_instance *crypto_authenc_alloc(struct rtattr **tb)
{
	struct crypto_attr_type *algt;
	struct crypto_instance *inst;
	struct hash_alg_common *auth;
	struct crypto_alg *auth_base;
	struct crypto_alg *enc;
=======
static void crypto_authenc_exit_tfm(struct crypto_aead *tfm)
{
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(tfm);

	crypto_free_ahash(ctx->auth);
	crypto_free_skcipher(ctx->enc);
	crypto_put_default_null_skcipher2();
}

static void crypto_authenc_free(struct aead_instance *inst)
{
	struct authenc_instance_ctx *ctx = aead_instance_ctx(inst);

	crypto_drop_skcipher(&ctx->enc);
	crypto_drop_ahash(&ctx->auth);
	kfree(inst);
}

static int crypto_authenc_create(struct crypto_template *tmpl,
				 struct rtattr **tb)
{
	struct crypto_attr_type *algt;
	struct aead_instance *inst;
	struct hash_alg_common *auth;
	struct crypto_alg *auth_base;
	struct skcipher_alg *enc;
>>>>>>> v4.9.227
	struct authenc_instance_ctx *ctx;
	const char *enc_name;
	int err;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
<<<<<<< HEAD
		return ERR_CAST(algt);

	if ((algt->type ^ CRYPTO_ALG_TYPE_AEAD) & algt->mask)
		return ERR_PTR(-EINVAL);

	auth = ahash_attr_alg(tb[1], CRYPTO_ALG_TYPE_HASH,
			       CRYPTO_ALG_TYPE_AHASH_MASK);
	if (IS_ERR(auth))
		return ERR_CAST(auth);
=======
		return PTR_ERR(algt);

	if ((algt->type ^ CRYPTO_ALG_TYPE_AEAD) & algt->mask)
		return -EINVAL;

	auth = ahash_attr_alg(tb[1], CRYPTO_ALG_TYPE_HASH,
			      CRYPTO_ALG_TYPE_AHASH_MASK |
			      crypto_requires_sync(algt->type, algt->mask));
	if (IS_ERR(auth))
		return PTR_ERR(auth);
>>>>>>> v4.9.227

	auth_base = &auth->base;

	enc_name = crypto_attr_alg_name(tb[2]);
	err = PTR_ERR(enc_name);
	if (IS_ERR(enc_name))
		goto out_put_auth;

	inst = kzalloc(sizeof(*inst) + sizeof(*ctx), GFP_KERNEL);
	err = -ENOMEM;
	if (!inst)
		goto out_put_auth;

<<<<<<< HEAD
	ctx = crypto_instance_ctx(inst);

	err = crypto_init_ahash_spawn(&ctx->auth, auth, inst);
	if (err)
		goto err_free_inst;

	crypto_set_skcipher_spawn(&ctx->enc, inst);
	err = crypto_grab_skcipher(&ctx->enc, enc_name, 0,
				   crypto_requires_sync(algt->type,
							algt->mask));
	if (err)
		goto err_drop_auth;

	enc = crypto_skcipher_spawn_alg(&ctx->enc);

	err = -ENAMETOOLONG;
	if (snprintf(inst->alg.cra_name, CRYPTO_MAX_ALG_NAME,
		     "authenc(%s,%s)", auth_base->cra_name, enc->cra_name) >=
	    CRYPTO_MAX_ALG_NAME)
		goto err_drop_enc;

	if (snprintf(inst->alg.cra_driver_name, CRYPTO_MAX_ALG_NAME,
		     "authenc(%s,%s)", auth_base->cra_driver_name,
		     enc->cra_driver_name) >= CRYPTO_MAX_ALG_NAME)
		goto err_drop_enc;

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_AEAD;
	inst->alg.cra_flags |= enc->cra_flags & CRYPTO_ALG_ASYNC;
	inst->alg.cra_priority = enc->cra_priority *
				 10 + auth_base->cra_priority;
	inst->alg.cra_blocksize = enc->cra_blocksize;
	inst->alg.cra_alignmask = auth_base->cra_alignmask | enc->cra_alignmask;
	inst->alg.cra_type = &crypto_aead_type;

	inst->alg.cra_aead.ivsize = enc->cra_ablkcipher.ivsize;
	inst->alg.cra_aead.maxauthsize = auth->digestsize;

	inst->alg.cra_ctxsize = sizeof(struct crypto_authenc_ctx);

	inst->alg.cra_init = crypto_authenc_init_tfm;
	inst->alg.cra_exit = crypto_authenc_exit_tfm;

	inst->alg.cra_aead.setkey = crypto_authenc_setkey;
	inst->alg.cra_aead.encrypt = crypto_authenc_encrypt;
	inst->alg.cra_aead.decrypt = crypto_authenc_decrypt;
	inst->alg.cra_aead.givencrypt = crypto_authenc_givencrypt;

out:
	crypto_mod_put(auth_base);
	return inst;
=======
	ctx = aead_instance_ctx(inst);

	err = crypto_init_ahash_spawn(&ctx->auth, auth,
				      aead_crypto_instance(inst));
	if (err)
		goto err_free_inst;

	crypto_set_skcipher_spawn(&ctx->enc, aead_crypto_instance(inst));
	err = crypto_grab_skcipher2(&ctx->enc, enc_name, 0,
				    crypto_requires_sync(algt->type,
							 algt->mask));
	if (err)
		goto err_drop_auth;

	enc = crypto_spawn_skcipher_alg(&ctx->enc);

	ctx->reqoff = ALIGN(2 * auth->digestsize + auth_base->cra_alignmask,
			    auth_base->cra_alignmask + 1);

	err = -ENAMETOOLONG;
	if (snprintf(inst->alg.base.cra_name, CRYPTO_MAX_ALG_NAME,
		     "authenc(%s,%s)", auth_base->cra_name,
		     enc->base.cra_name) >=
	    CRYPTO_MAX_ALG_NAME)
		goto err_drop_enc;

	if (snprintf(inst->alg.base.cra_driver_name, CRYPTO_MAX_ALG_NAME,
		     "authenc(%s,%s)", auth_base->cra_driver_name,
		     enc->base.cra_driver_name) >= CRYPTO_MAX_ALG_NAME)
		goto err_drop_enc;

	inst->alg.base.cra_flags = (auth_base->cra_flags |
				    enc->base.cra_flags) & CRYPTO_ALG_ASYNC;
	inst->alg.base.cra_priority = enc->base.cra_priority * 10 +
				      auth_base->cra_priority;
	inst->alg.base.cra_blocksize = enc->base.cra_blocksize;
	inst->alg.base.cra_alignmask = auth_base->cra_alignmask |
				       enc->base.cra_alignmask;
	inst->alg.base.cra_ctxsize = sizeof(struct crypto_authenc_ctx);

	inst->alg.ivsize = crypto_skcipher_alg_ivsize(enc);
	inst->alg.chunksize = crypto_skcipher_alg_chunksize(enc);
	inst->alg.maxauthsize = auth->digestsize;

	inst->alg.init = crypto_authenc_init_tfm;
	inst->alg.exit = crypto_authenc_exit_tfm;

	inst->alg.setkey = crypto_authenc_setkey;
	inst->alg.encrypt = crypto_authenc_encrypt;
	inst->alg.decrypt = crypto_authenc_decrypt;

	inst->free = crypto_authenc_free;

	err = aead_register_instance(tmpl, inst);
	if (err)
		goto err_drop_enc;

out:
	crypto_mod_put(auth_base);
	return err;
>>>>>>> v4.9.227

err_drop_enc:
	crypto_drop_skcipher(&ctx->enc);
err_drop_auth:
	crypto_drop_ahash(&ctx->auth);
err_free_inst:
	kfree(inst);
out_put_auth:
<<<<<<< HEAD
	inst = ERR_PTR(err);
	goto out;
}

static void crypto_authenc_free(struct crypto_instance *inst)
{
	struct authenc_instance_ctx *ctx = crypto_instance_ctx(inst);

	crypto_drop_skcipher(&ctx->enc);
	crypto_drop_ahash(&ctx->auth);
	kfree(inst);
}

static struct crypto_template crypto_authenc_tmpl = {
	.name = "authenc",
	.alloc = crypto_authenc_alloc,
	.free = crypto_authenc_free,
=======
	goto out;
}

static struct crypto_template crypto_authenc_tmpl = {
	.name = "authenc",
	.create = crypto_authenc_create,
>>>>>>> v4.9.227
	.module = THIS_MODULE,
};

static int __init crypto_authenc_module_init(void)
{
	return crypto_register_template(&crypto_authenc_tmpl);
}

static void __exit crypto_authenc_module_exit(void)
{
	crypto_unregister_template(&crypto_authenc_tmpl);
}

module_init(crypto_authenc_module_init);
module_exit(crypto_authenc_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple AEAD wrapper for IPsec");
MODULE_ALIAS_CRYPTO("authenc");
