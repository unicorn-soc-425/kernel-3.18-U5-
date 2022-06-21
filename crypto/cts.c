/*
 * CTS: Cipher Text Stealing mode
 *
 * COPYRIGHT (c) 2008
 * The Regents of the University of Michigan
 * ALL RIGHTS RESERVED
 *
 * Permission is granted to use, copy, create derivative works
 * and redistribute this software and such derivative works
 * for any purpose, so long as the name of The University of
 * Michigan is not used in any advertising or publicity
 * pertaining to the use of distribution of this software
 * without specific, written prior authorization.  If the
 * above copyright notice or any other identification of the
 * University of Michigan is included in any copy of any
 * portion of this software, then the disclaimer below must
 * also be included.
 *
 * THIS SOFTWARE IS PROVIDED AS IS, WITHOUT REPRESENTATION
 * FROM THE UNIVERSITY OF MICHIGAN AS TO ITS FITNESS FOR ANY
 * PURPOSE, AND WITHOUT WARRANTY BY THE UNIVERSITY OF
 * MICHIGAN OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING
 * WITHOUT LIMITATION THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE
 * REGENTS OF THE UNIVERSITY OF MICHIGAN SHALL NOT BE LIABLE
 * FOR ANY DAMAGES, INCLUDING SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, WITH RESPECT TO ANY CLAIM ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OF THE SOFTWARE, EVEN
 * IF IT HAS BEEN OR IS HEREAFTER ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES.
 */

/* Derived from various:
 *	Copyright (c) 2006 Herbert Xu <herbert@gondor.apana.org.au>
 */

/*
 * This is the Cipher Text Stealing mode as described by
 * Section 8 of rfc2040 and referenced by rfc3962.
 * rfc3962 includes errata information in its Appendix A.
 */

<<<<<<< HEAD
#include <crypto/algapi.h>
=======
#include <crypto/internal/skcipher.h>
>>>>>>> v4.9.227
#include <linux/err.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/log2.h>
#include <linux/module.h>
#include <linux/scatterlist.h>
#include <crypto/scatterwalk.h>
#include <linux/slab.h>

struct crypto_cts_ctx {
<<<<<<< HEAD
	struct crypto_blkcipher *child;
};

static int crypto_cts_setkey(struct crypto_tfm *parent, const u8 *key,
			     unsigned int keylen)
{
	struct crypto_cts_ctx *ctx = crypto_tfm_ctx(parent);
	struct crypto_blkcipher *child = ctx->child;
	int err;

	crypto_blkcipher_clear_flags(child, CRYPTO_TFM_REQ_MASK);
	crypto_blkcipher_set_flags(child, crypto_tfm_get_flags(parent) &
				       CRYPTO_TFM_REQ_MASK);
	err = crypto_blkcipher_setkey(child, key, keylen);
	crypto_tfm_set_flags(parent, crypto_blkcipher_get_flags(child) &
				     CRYPTO_TFM_RES_MASK);
	return err;
}

static int cts_cbc_encrypt(struct crypto_cts_ctx *ctx,
			   struct blkcipher_desc *desc,
			   struct scatterlist *dst,
			   struct scatterlist *src,
			   unsigned int offset,
			   unsigned int nbytes)
{
	int bsize = crypto_blkcipher_blocksize(desc->tfm);
	u8 tmp[bsize], tmp2[bsize];
	struct blkcipher_desc lcldesc;
	struct scatterlist sgsrc[1], sgdst[1];
	int lastn = nbytes - bsize;
	u8 iv[bsize];
	u8 s[bsize * 2], d[bsize * 2];
	int err;

	if (lastn < 0)
		return -EINVAL;

	sg_init_table(sgsrc, 1);
	sg_init_table(sgdst, 1);

	memset(s, 0, sizeof(s));
	scatterwalk_map_and_copy(s, src, offset, nbytes, 0);

	memcpy(iv, desc->info, bsize);

	lcldesc.tfm = ctx->child;
	lcldesc.info = iv;
	lcldesc.flags = desc->flags;

	sg_set_buf(&sgsrc[0], s, bsize);
	sg_set_buf(&sgdst[0], tmp, bsize);
	err = crypto_blkcipher_encrypt_iv(&lcldesc, sgdst, sgsrc, bsize);

	memcpy(d + bsize, tmp, lastn);

	lcldesc.info = tmp;

	sg_set_buf(&sgsrc[0], s + bsize, bsize);
	sg_set_buf(&sgdst[0], tmp2, bsize);
	err = crypto_blkcipher_encrypt_iv(&lcldesc, sgdst, sgsrc, bsize);

	memcpy(d, tmp2, bsize);

	scatterwalk_map_and_copy(d, dst, offset, nbytes, 1);

	memcpy(desc->info, tmp2, bsize);

	return err;
}

static int crypto_cts_encrypt(struct blkcipher_desc *desc,
			      struct scatterlist *dst, struct scatterlist *src,
			      unsigned int nbytes)
{
	struct crypto_cts_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	int bsize = crypto_blkcipher_blocksize(desc->tfm);
	int tot_blocks = (nbytes + bsize - 1) / bsize;
	int cbc_blocks = tot_blocks > 2 ? tot_blocks - 2 : 0;
	struct blkcipher_desc lcldesc;
	int err;

	lcldesc.tfm = ctx->child;
	lcldesc.info = desc->info;
	lcldesc.flags = desc->flags;

	if (tot_blocks == 1) {
		err = crypto_blkcipher_encrypt_iv(&lcldesc, dst, src, bsize);
	} else if (nbytes <= bsize * 2) {
		err = cts_cbc_encrypt(ctx, desc, dst, src, 0, nbytes);
	} else {
		/* do normal function for tot_blocks - 2 */
		err = crypto_blkcipher_encrypt_iv(&lcldesc, dst, src,
							cbc_blocks * bsize);
		if (err == 0) {
			/* do cts for final two blocks */
			err = cts_cbc_encrypt(ctx, desc, dst, src,
						cbc_blocks * bsize,
						nbytes - (cbc_blocks * bsize));
		}
	}

	return err;
}

static int cts_cbc_decrypt(struct crypto_cts_ctx *ctx,
			   struct blkcipher_desc *desc,
			   struct scatterlist *dst,
			   struct scatterlist *src,
			   unsigned int offset,
			   unsigned int nbytes)
{
	int bsize = crypto_blkcipher_blocksize(desc->tfm);
	u8 tmp[bsize];
	struct blkcipher_desc lcldesc;
	struct scatterlist sgsrc[1], sgdst[1];
	int lastn = nbytes - bsize;
	u8 iv[bsize];
	u8 s[bsize * 2], d[bsize * 2];
	int err;

	if (lastn < 0)
		return -EINVAL;

	sg_init_table(sgsrc, 1);
	sg_init_table(sgdst, 1);

	scatterwalk_map_and_copy(s, src, offset, nbytes, 0);

	lcldesc.tfm = ctx->child;
	lcldesc.info = iv;
	lcldesc.flags = desc->flags;

	/* 1. Decrypt Cn-1 (s) to create Dn (tmp)*/
	memset(iv, 0, sizeof(iv));
	sg_set_buf(&sgsrc[0], s, bsize);
	sg_set_buf(&sgdst[0], tmp, bsize);
	err = crypto_blkcipher_decrypt_iv(&lcldesc, sgdst, sgsrc, bsize);
	if (err)
		return err;
	/* 2. Pad Cn with zeros at the end to create C of length BB */
	memset(iv, 0, sizeof(iv));
	memcpy(iv, s + bsize, lastn);
	/* 3. Exclusive-or Dn (tmp) with C (iv) to create Xn (tmp) */
	crypto_xor(tmp, iv, bsize);
	/* 4. Select the first Ln bytes of Xn (tmp) to create Pn */
	memcpy(d + bsize, tmp, lastn);

	/* 5. Append the tail (BB - Ln) bytes of Xn (tmp) to Cn to create En */
	memcpy(s + bsize + lastn, tmp + lastn, bsize - lastn);
	/* 6. Decrypt En to create Pn-1 */
	memzero_explicit(iv, sizeof(iv));

	sg_set_buf(&sgsrc[0], s + bsize, bsize);
	sg_set_buf(&sgdst[0], d, bsize);
	err = crypto_blkcipher_decrypt_iv(&lcldesc, sgdst, sgsrc, bsize);

	/* XOR with previous block */
	crypto_xor(d, desc->info, bsize);

	scatterwalk_map_and_copy(d, dst, offset, nbytes, 1);

	memcpy(desc->info, s, bsize);
	return err;
}

static int crypto_cts_decrypt(struct blkcipher_desc *desc,
			      struct scatterlist *dst, struct scatterlist *src,
			      unsigned int nbytes)
{
	struct crypto_cts_ctx *ctx = crypto_blkcipher_ctx(desc->tfm);
	int bsize = crypto_blkcipher_blocksize(desc->tfm);
	int tot_blocks = (nbytes + bsize - 1) / bsize;
	int cbc_blocks = tot_blocks > 2 ? tot_blocks - 2 : 0;
	struct blkcipher_desc lcldesc;
	int err;

	lcldesc.tfm = ctx->child;
	lcldesc.info = desc->info;
	lcldesc.flags = desc->flags;

	if (tot_blocks == 1) {
		err = crypto_blkcipher_decrypt_iv(&lcldesc, dst, src, bsize);
	} else if (nbytes <= bsize * 2) {
		err = cts_cbc_decrypt(ctx, desc, dst, src, 0, nbytes);
	} else {
		/* do normal function for tot_blocks - 2 */
		err = crypto_blkcipher_decrypt_iv(&lcldesc, dst, src,
							cbc_blocks * bsize);
		if (err == 0) {
			/* do cts for final two blocks */
			err = cts_cbc_decrypt(ctx, desc, dst, src,
						cbc_blocks * bsize,
						nbytes - (cbc_blocks * bsize));
		}
	}
	return err;
}

static int crypto_cts_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = (void *)tfm->__crt_alg;
	struct crypto_spawn *spawn = crypto_instance_ctx(inst);
	struct crypto_cts_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_blkcipher *cipher;

	cipher = crypto_spawn_blkcipher(spawn);
=======
	struct crypto_skcipher *child;
};

struct crypto_cts_reqctx {
	struct scatterlist sg[2];
	unsigned offset;
	struct skcipher_request subreq;
};

static inline u8 *crypto_cts_reqctx_space(struct skcipher_request *req)
{
	struct crypto_cts_reqctx *rctx = skcipher_request_ctx(req);
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct crypto_cts_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct crypto_skcipher *child = ctx->child;

	return PTR_ALIGN((u8 *)(rctx + 1) + crypto_skcipher_reqsize(child),
			 crypto_skcipher_alignmask(tfm) + 1);
}

static int crypto_cts_setkey(struct crypto_skcipher *parent, const u8 *key,
			     unsigned int keylen)
{
	struct crypto_cts_ctx *ctx = crypto_skcipher_ctx(parent);
	struct crypto_skcipher *child = ctx->child;
	int err;

	crypto_skcipher_clear_flags(child, CRYPTO_TFM_REQ_MASK);
	crypto_skcipher_set_flags(child, crypto_skcipher_get_flags(parent) &
					 CRYPTO_TFM_REQ_MASK);
	err = crypto_skcipher_setkey(child, key, keylen);
	crypto_skcipher_set_flags(parent, crypto_skcipher_get_flags(child) &
					  CRYPTO_TFM_RES_MASK);
	return err;
}

static void cts_cbc_crypt_done(struct crypto_async_request *areq, int err)
{
	struct skcipher_request *req = areq->data;

	if (err == -EINPROGRESS)
		return;

	skcipher_request_complete(req, err);
}

static int cts_cbc_encrypt(struct skcipher_request *req)
{
	struct crypto_cts_reqctx *rctx = skcipher_request_ctx(req);
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct skcipher_request *subreq = &rctx->subreq;
	int bsize = crypto_skcipher_blocksize(tfm);
	u8 d[bsize * 2] __attribute__ ((aligned(__alignof__(u32))));
	struct scatterlist *sg;
	unsigned int offset;
	int lastn;

	offset = rctx->offset;
	lastn = req->cryptlen - offset;

	sg = scatterwalk_ffwd(rctx->sg, req->dst, offset - bsize);
	scatterwalk_map_and_copy(d + bsize, sg, 0, bsize, 0);

	memset(d, 0, bsize);
	scatterwalk_map_and_copy(d, req->src, offset, lastn, 0);

	scatterwalk_map_and_copy(d, sg, 0, bsize + lastn, 1);
	memzero_explicit(d, sizeof(d));

	skcipher_request_set_callback(subreq, req->base.flags &
					      CRYPTO_TFM_REQ_MAY_BACKLOG,
				      cts_cbc_crypt_done, req);
	skcipher_request_set_crypt(subreq, sg, sg, bsize, req->iv);
	return crypto_skcipher_encrypt(subreq);
}

static void crypto_cts_encrypt_done(struct crypto_async_request *areq, int err)
{
	struct skcipher_request *req = areq->data;

	if (err)
		goto out;

	err = cts_cbc_encrypt(req);
	if (err == -EINPROGRESS ||
	    (err == -EBUSY && req->base.flags & CRYPTO_TFM_REQ_MAY_BACKLOG))
		return;

out:
	skcipher_request_complete(req, err);
}

static int crypto_cts_encrypt(struct skcipher_request *req)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct crypto_cts_reqctx *rctx = skcipher_request_ctx(req);
	struct crypto_cts_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct skcipher_request *subreq = &rctx->subreq;
	int bsize = crypto_skcipher_blocksize(tfm);
	unsigned int nbytes = req->cryptlen;
	int cbc_blocks = (nbytes + bsize - 1) / bsize - 1;
	unsigned int offset;

	skcipher_request_set_tfm(subreq, ctx->child);

	if (cbc_blocks <= 0) {
		skcipher_request_set_callback(subreq, req->base.flags,
					      req->base.complete,
					      req->base.data);
		skcipher_request_set_crypt(subreq, req->src, req->dst, nbytes,
					   req->iv);
		return crypto_skcipher_encrypt(subreq);
	}

	offset = cbc_blocks * bsize;
	rctx->offset = offset;

	skcipher_request_set_callback(subreq, req->base.flags,
				      crypto_cts_encrypt_done, req);
	skcipher_request_set_crypt(subreq, req->src, req->dst,
				   offset, req->iv);

	return crypto_skcipher_encrypt(subreq) ?:
	       cts_cbc_encrypt(req);
}

static int cts_cbc_decrypt(struct skcipher_request *req)
{
	struct crypto_cts_reqctx *rctx = skcipher_request_ctx(req);
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct skcipher_request *subreq = &rctx->subreq;
	int bsize = crypto_skcipher_blocksize(tfm);
	u8 d[bsize * 2] __attribute__ ((aligned(__alignof__(u32))));
	struct scatterlist *sg;
	unsigned int offset;
	u8 *space;
	int lastn;

	offset = rctx->offset;
	lastn = req->cryptlen - offset;

	sg = scatterwalk_ffwd(rctx->sg, req->dst, offset - bsize);

	/* 1. Decrypt Cn-1 (s) to create Dn */
	scatterwalk_map_and_copy(d + bsize, sg, 0, bsize, 0);
	space = crypto_cts_reqctx_space(req);
	crypto_xor(d + bsize, space, bsize);
	/* 2. Pad Cn with zeros at the end to create C of length BB */
	memset(d, 0, bsize);
	scatterwalk_map_and_copy(d, req->src, offset, lastn, 0);
	/* 3. Exclusive-or Dn with C to create Xn */
	/* 4. Select the first Ln bytes of Xn to create Pn */
	crypto_xor(d + bsize, d, lastn);

	/* 5. Append the tail (BB - Ln) bytes of Xn to Cn to create En */
	memcpy(d + lastn, d + bsize + lastn, bsize - lastn);
	/* 6. Decrypt En to create Pn-1 */

	scatterwalk_map_and_copy(d, sg, 0, bsize + lastn, 1);
	memzero_explicit(d, sizeof(d));

	skcipher_request_set_callback(subreq, req->base.flags &
					      CRYPTO_TFM_REQ_MAY_BACKLOG,
				      cts_cbc_crypt_done, req);

	skcipher_request_set_crypt(subreq, sg, sg, bsize, space);
	return crypto_skcipher_decrypt(subreq);
}

static void crypto_cts_decrypt_done(struct crypto_async_request *areq, int err)
{
	struct skcipher_request *req = areq->data;

	if (err)
		goto out;

	err = cts_cbc_decrypt(req);
	if (err == -EINPROGRESS ||
	    (err == -EBUSY && req->base.flags & CRYPTO_TFM_REQ_MAY_BACKLOG))
		return;

out:
	skcipher_request_complete(req, err);
}

static int crypto_cts_decrypt(struct skcipher_request *req)
{
	struct crypto_skcipher *tfm = crypto_skcipher_reqtfm(req);
	struct crypto_cts_reqctx *rctx = skcipher_request_ctx(req);
	struct crypto_cts_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct skcipher_request *subreq = &rctx->subreq;
	int bsize = crypto_skcipher_blocksize(tfm);
	unsigned int nbytes = req->cryptlen;
	int cbc_blocks = (nbytes + bsize - 1) / bsize - 1;
	unsigned int offset;
	u8 *space;

	skcipher_request_set_tfm(subreq, ctx->child);

	if (cbc_blocks <= 0) {
		skcipher_request_set_callback(subreq, req->base.flags,
					      req->base.complete,
					      req->base.data);
		skcipher_request_set_crypt(subreq, req->src, req->dst, nbytes,
					   req->iv);
		return crypto_skcipher_decrypt(subreq);
	}

	skcipher_request_set_callback(subreq, req->base.flags,
				      crypto_cts_decrypt_done, req);

	space = crypto_cts_reqctx_space(req);

	offset = cbc_blocks * bsize;
	rctx->offset = offset;

	if (cbc_blocks <= 1)
		memcpy(space, req->iv, bsize);
	else
		scatterwalk_map_and_copy(space, req->src, offset - 2 * bsize,
					 bsize, 0);

	skcipher_request_set_crypt(subreq, req->src, req->dst,
				   offset, req->iv);

	return crypto_skcipher_decrypt(subreq) ?:
	       cts_cbc_decrypt(req);
}

static int crypto_cts_init_tfm(struct crypto_skcipher *tfm)
{
	struct skcipher_instance *inst = skcipher_alg_instance(tfm);
	struct crypto_skcipher_spawn *spawn = skcipher_instance_ctx(inst);
	struct crypto_cts_ctx *ctx = crypto_skcipher_ctx(tfm);
	struct crypto_skcipher *cipher;
	unsigned reqsize;
	unsigned bsize;
	unsigned align;

	cipher = crypto_spawn_skcipher2(spawn);
>>>>>>> v4.9.227
	if (IS_ERR(cipher))
		return PTR_ERR(cipher);

	ctx->child = cipher;
<<<<<<< HEAD
	return 0;
}

static void crypto_cts_exit_tfm(struct crypto_tfm *tfm)
{
	struct crypto_cts_ctx *ctx = crypto_tfm_ctx(tfm);
	crypto_free_blkcipher(ctx->child);
}

static struct crypto_instance *crypto_cts_alloc(struct rtattr **tb)
{
	struct crypto_instance *inst;
	struct crypto_alg *alg;
	int err;

	err = crypto_check_attr_type(tb, CRYPTO_ALG_TYPE_BLKCIPHER);
	if (err)
		return ERR_PTR(err);

	alg = crypto_attr_alg(tb[1], CRYPTO_ALG_TYPE_BLKCIPHER,
				  CRYPTO_ALG_TYPE_MASK);
	if (IS_ERR(alg))
		return ERR_CAST(alg);

	inst = ERR_PTR(-EINVAL);
	if (!is_power_of_2(alg->cra_blocksize))
		goto out_put_alg;

	inst = crypto_alloc_instance("cts", alg);
	if (IS_ERR(inst))
		goto out_put_alg;

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_BLKCIPHER;
	inst->alg.cra_priority = alg->cra_priority;
	inst->alg.cra_blocksize = alg->cra_blocksize;
	inst->alg.cra_alignmask = alg->cra_alignmask;
	inst->alg.cra_type = &crypto_blkcipher_type;

	/* We access the data as u32s when xoring. */
	inst->alg.cra_alignmask |= __alignof__(u32) - 1;

	inst->alg.cra_blkcipher.ivsize = alg->cra_blocksize;
	inst->alg.cra_blkcipher.min_keysize = alg->cra_blkcipher.min_keysize;
	inst->alg.cra_blkcipher.max_keysize = alg->cra_blkcipher.max_keysize;

	inst->alg.cra_blkcipher.geniv = "seqiv";

	inst->alg.cra_ctxsize = sizeof(struct crypto_cts_ctx);

	inst->alg.cra_init = crypto_cts_init_tfm;
	inst->alg.cra_exit = crypto_cts_exit_tfm;

	inst->alg.cra_blkcipher.setkey = crypto_cts_setkey;
	inst->alg.cra_blkcipher.encrypt = crypto_cts_encrypt;
	inst->alg.cra_blkcipher.decrypt = crypto_cts_decrypt;

out_put_alg:
	crypto_mod_put(alg);
	return inst;
}

static void crypto_cts_free(struct crypto_instance *inst)
{
	crypto_drop_spawn(crypto_instance_ctx(inst));
	kfree(inst);
=======

	align = crypto_skcipher_alignmask(tfm);
	bsize = crypto_skcipher_blocksize(cipher);
	reqsize = ALIGN(sizeof(struct crypto_cts_reqctx) +
			crypto_skcipher_reqsize(cipher),
			crypto_tfm_ctx_alignment()) +
		  (align & ~(crypto_tfm_ctx_alignment() - 1)) + bsize;

	crypto_skcipher_set_reqsize(tfm, reqsize);

	return 0;
}

static void crypto_cts_exit_tfm(struct crypto_skcipher *tfm)
{
	struct crypto_cts_ctx *ctx = crypto_skcipher_ctx(tfm);

	crypto_free_skcipher(ctx->child);
}

static void crypto_cts_free(struct skcipher_instance *inst)
{
	crypto_drop_skcipher(skcipher_instance_ctx(inst));
	kfree(inst);
}

static int crypto_cts_create(struct crypto_template *tmpl, struct rtattr **tb)
{
	struct crypto_skcipher_spawn *spawn;
	struct skcipher_instance *inst;
	struct crypto_attr_type *algt;
	struct skcipher_alg *alg;
	const char *cipher_name;
	int err;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
		return PTR_ERR(algt);

	if ((algt->type ^ CRYPTO_ALG_TYPE_SKCIPHER) & algt->mask)
		return -EINVAL;

	cipher_name = crypto_attr_alg_name(tb[1]);
	if (IS_ERR(cipher_name))
		return PTR_ERR(cipher_name);

	inst = kzalloc(sizeof(*inst) + sizeof(*spawn), GFP_KERNEL);
	if (!inst)
		return -ENOMEM;

	spawn = skcipher_instance_ctx(inst);

	crypto_set_skcipher_spawn(spawn, skcipher_crypto_instance(inst));
	err = crypto_grab_skcipher2(spawn, cipher_name, 0,
				    crypto_requires_sync(algt->type,
							 algt->mask));
	if (err)
		goto err_free_inst;

	alg = crypto_spawn_skcipher_alg(spawn);

	err = -EINVAL;
	if (crypto_skcipher_alg_ivsize(alg) != alg->base.cra_blocksize)
		goto err_drop_spawn;

	if (strncmp(alg->base.cra_name, "cbc(", 4))
		goto err_drop_spawn;

	err = crypto_inst_setname(skcipher_crypto_instance(inst), "cts",
				  &alg->base);
	if (err)
		goto err_drop_spawn;

	inst->alg.base.cra_flags = alg->base.cra_flags & CRYPTO_ALG_ASYNC;
	inst->alg.base.cra_priority = alg->base.cra_priority;
	inst->alg.base.cra_blocksize = alg->base.cra_blocksize;
	inst->alg.base.cra_alignmask = alg->base.cra_alignmask;

	/* We access the data as u32s when xoring. */
	inst->alg.base.cra_alignmask |= __alignof__(u32) - 1;

	inst->alg.ivsize = alg->base.cra_blocksize;
	inst->alg.chunksize = crypto_skcipher_alg_chunksize(alg);
	inst->alg.min_keysize = crypto_skcipher_alg_min_keysize(alg);
	inst->alg.max_keysize = crypto_skcipher_alg_max_keysize(alg);

	inst->alg.base.cra_ctxsize = sizeof(struct crypto_cts_ctx);

	inst->alg.init = crypto_cts_init_tfm;
	inst->alg.exit = crypto_cts_exit_tfm;

	inst->alg.setkey = crypto_cts_setkey;
	inst->alg.encrypt = crypto_cts_encrypt;
	inst->alg.decrypt = crypto_cts_decrypt;

	inst->free = crypto_cts_free;

	err = skcipher_register_instance(tmpl, inst);
	if (err)
		goto err_drop_spawn;

out:
	return err;

err_drop_spawn:
	crypto_drop_skcipher(spawn);
err_free_inst:
	kfree(inst);
	goto out;
>>>>>>> v4.9.227
}

static struct crypto_template crypto_cts_tmpl = {
	.name = "cts",
<<<<<<< HEAD
	.alloc = crypto_cts_alloc,
	.free = crypto_cts_free,
=======
	.create = crypto_cts_create,
>>>>>>> v4.9.227
	.module = THIS_MODULE,
};

static int __init crypto_cts_module_init(void)
{
	return crypto_register_template(&crypto_cts_tmpl);
}

static void __exit crypto_cts_module_exit(void)
{
	crypto_unregister_template(&crypto_cts_tmpl);
}

module_init(crypto_cts_module_init);
module_exit(crypto_cts_module_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("CTS-CBC CipherText Stealing for CBC");
MODULE_ALIAS_CRYPTO("cts");
