/*
 * This contains encryption functions for per-file encryption.
 *
 * Copyright (C) 2015, Google, Inc.
 * Copyright (C) 2015, Motorola Mobility
 *
 * Written by Michael Halcrow, 2014.
 *
 * Filename encryption additions
 *	Uday Savagaonkar, 2014
 * Encryption policy handling additions
 *	Ildar Muslukhov, 2014
 * Add fscrypt_pullback_bio_page()
 *	Jaegeuk Kim, 2015.
 *
 * This has not yet undergone a rigorous security audit.
 *
 * The usage of AES-XTS should conform to recommendations in NIST
 * Special Publication 800-38E and IEEE P1619/D16.
 */

#include <linux/pagemap.h>
#include <linux/mempool.h>
#include <linux/module.h>
#include <linux/scatterlist.h>
#include <linux/ratelimit.h>
<<<<<<< HEAD
#include <linux/dcache.h>
#include <linux/namei.h>
#include <crypto/aes.h>
#include <crypto/skcipher.h>
#include "fscrypt_private.h"
=======
#include <linux/bio.h>
#include <linux/dcache.h>
#include <linux/namei.h>
#include <linux/fscrypto.h>
>>>>>>> v4.9.227

static unsigned int num_prealloc_crypto_pages = 32;
static unsigned int num_prealloc_crypto_ctxs = 128;

module_param(num_prealloc_crypto_pages, uint, 0444);
MODULE_PARM_DESC(num_prealloc_crypto_pages,
		"Number of crypto pages to preallocate");
module_param(num_prealloc_crypto_ctxs, uint, 0444);
MODULE_PARM_DESC(num_prealloc_crypto_ctxs,
		"Number of crypto contexts to preallocate");

static mempool_t *fscrypt_bounce_page_pool = NULL;

static LIST_HEAD(fscrypt_free_ctxs);
static DEFINE_SPINLOCK(fscrypt_ctx_lock);

static struct workqueue_struct *fscrypt_read_workqueue;
static DEFINE_MUTEX(fscrypt_init_mutex);

static struct kmem_cache *fscrypt_ctx_cachep;
struct kmem_cache *fscrypt_info_cachep;

<<<<<<< HEAD
void fscrypt_enqueue_decrypt_work(struct work_struct *work)
{
	queue_work(fscrypt_read_workqueue, work);
}
EXPORT_SYMBOL(fscrypt_enqueue_decrypt_work);

=======
>>>>>>> v4.9.227
/**
 * fscrypt_release_ctx() - Releases an encryption context
 * @ctx: The encryption context to release.
 *
 * If the encryption context was allocated from the pre-allocated pool, returns
 * it to that pool. Else, frees it.
 *
 * If there's a bounce page in the context, this frees that.
 */
void fscrypt_release_ctx(struct fscrypt_ctx *ctx)
{
	unsigned long flags;

<<<<<<< HEAD
	if (ctx->flags & FS_CTX_HAS_BOUNCE_BUFFER_FL && ctx->w.bounce_page) {
=======
	if (ctx->flags & FS_WRITE_PATH_FL && ctx->w.bounce_page) {
>>>>>>> v4.9.227
		mempool_free(ctx->w.bounce_page, fscrypt_bounce_page_pool);
		ctx->w.bounce_page = NULL;
	}
	ctx->w.control_page = NULL;
	if (ctx->flags & FS_CTX_REQUIRES_FREE_ENCRYPT_FL) {
		kmem_cache_free(fscrypt_ctx_cachep, ctx);
	} else {
		spin_lock_irqsave(&fscrypt_ctx_lock, flags);
		list_add(&ctx->free_list, &fscrypt_free_ctxs);
		spin_unlock_irqrestore(&fscrypt_ctx_lock, flags);
	}
}
EXPORT_SYMBOL(fscrypt_release_ctx);

/**
 * fscrypt_get_ctx() - Gets an encryption context
 * @inode:       The inode for which we are doing the crypto
 * @gfp_flags:   The gfp flag for memory allocation
 *
 * Allocates and initializes an encryption context.
 *
 * Return: An allocated and initialized encryption context on success; error
 * value or NULL otherwise.
 */
<<<<<<< HEAD
struct fscrypt_ctx *fscrypt_get_ctx(const struct inode *inode, gfp_t gfp_flags)
=======
struct fscrypt_ctx *fscrypt_get_ctx(struct inode *inode, gfp_t gfp_flags)
>>>>>>> v4.9.227
{
	struct fscrypt_ctx *ctx = NULL;
	struct fscrypt_info *ci = inode->i_crypt_info;
	unsigned long flags;

	if (ci == NULL)
		return ERR_PTR(-ENOKEY);

	/*
	 * We first try getting the ctx from a free list because in
	 * the common case the ctx will have an allocated and
	 * initialized crypto tfm, so it's probably a worthwhile
	 * optimization. For the bounce page, we first try getting it
	 * from the kernel allocator because that's just about as fast
	 * as getting it from a list and because a cache of free pages
	 * should generally be a "last resort" option for a filesystem
	 * to be able to do its job.
	 */
	spin_lock_irqsave(&fscrypt_ctx_lock, flags);
	ctx = list_first_entry_or_null(&fscrypt_free_ctxs,
					struct fscrypt_ctx, free_list);
	if (ctx)
		list_del(&ctx->free_list);
	spin_unlock_irqrestore(&fscrypt_ctx_lock, flags);
	if (!ctx) {
		ctx = kmem_cache_zalloc(fscrypt_ctx_cachep, gfp_flags);
		if (!ctx)
			return ERR_PTR(-ENOMEM);
		ctx->flags |= FS_CTX_REQUIRES_FREE_ENCRYPT_FL;
	} else {
		ctx->flags &= ~FS_CTX_REQUIRES_FREE_ENCRYPT_FL;
	}
<<<<<<< HEAD
	ctx->flags &= ~FS_CTX_HAS_BOUNCE_BUFFER_FL;
=======
	ctx->flags &= ~FS_WRITE_PATH_FL;
>>>>>>> v4.9.227
	return ctx;
}
EXPORT_SYMBOL(fscrypt_get_ctx);

<<<<<<< HEAD
int fscrypt_do_page_crypto(const struct inode *inode, fscrypt_direction_t rw,
			   u64 lblk_num, struct page *src_page,
			   struct page *dest_page, unsigned int len,
			   unsigned int offs, gfp_t gfp_flags)
{
	struct {
		__le64 index;
		u8 padding[FS_IV_SIZE - sizeof(__le64)];
	} iv;
	struct ablkcipher_request *req = NULL;
	DECLARE_CRYPTO_WAIT(wait);
	struct scatterlist dst, src;
	struct fscrypt_info *ci = inode->i_crypt_info;
	struct crypto_ablkcipher *tfm = ci->ci_ctfm;
	int res = 0;

	BUG_ON(len == 0);

	BUILD_BUG_ON(sizeof(iv) != FS_IV_SIZE);
	BUILD_BUG_ON(AES_BLOCK_SIZE != FS_IV_SIZE);
	iv.index = cpu_to_le64(lblk_num);
	memset(iv.padding, 0, sizeof(iv.padding));

	if (ci->ci_essiv_tfm != NULL) {
		crypto_cipher_encrypt_one(ci->ci_essiv_tfm, (u8 *)&iv,
					  (u8 *)&iv);
	}

	req = ablkcipher_request_alloc(tfm, gfp_flags);
	if (!req)
		return -ENOMEM;

	ablkcipher_request_set_callback(
		req, CRYPTO_TFM_REQ_MAY_BACKLOG | CRYPTO_TFM_REQ_MAY_SLEEP,
		crypto_req_done, &wait);

	sg_init_table(&dst, 1);
	sg_set_page(&dst, dest_page, len, offs);
	sg_init_table(&src, 1);
	sg_set_page(&src, src_page, len, offs);
	ablkcipher_request_set_crypt(req, &src, &dst, len, &iv);
	if (rw == FS_DECRYPT)
		res = crypto_wait_req(crypto_ablkcipher_decrypt(req), &wait);
	else
		res = crypto_wait_req(crypto_ablkcipher_encrypt(req), &wait);
	ablkcipher_request_free(req);
	if (res) {
		fscrypt_err(inode->i_sb,
			    "%scryption failed for inode %lu, block %llu: %d",
			    (rw == FS_DECRYPT ? "de" : "en"),
			    inode->i_ino, lblk_num, res);
=======
/**
 * page_crypt_complete() - completion callback for page crypto
 * @req: The asynchronous cipher request context
 * @res: The result of the cipher operation
 */
static void page_crypt_complete(struct crypto_async_request *req, int res)
{
	struct fscrypt_completion_result *ecr = req->data;

	if (res == -EINPROGRESS)
		return;
	ecr->res = res;
	complete(&ecr->completion);
}

typedef enum {
	FS_DECRYPT = 0,
	FS_ENCRYPT,
} fscrypt_direction_t;

static int do_page_crypto(struct inode *inode,
			fscrypt_direction_t rw, pgoff_t index,
			struct page *src_page, struct page *dest_page,
			gfp_t gfp_flags)
{
	struct {
		__le64 index;
		u8 padding[FS_XTS_TWEAK_SIZE - sizeof(__le64)];
	} xts_tweak;
	struct skcipher_request *req = NULL;
	DECLARE_FS_COMPLETION_RESULT(ecr);
	struct scatterlist dst, src;
	struct fscrypt_info *ci = inode->i_crypt_info;
	struct crypto_skcipher *tfm = ci->ci_ctfm;
	int res = 0;

	req = skcipher_request_alloc(tfm, gfp_flags);
	if (!req) {
		printk_ratelimited(KERN_ERR
				"%s: crypto_request_alloc() failed\n",
				__func__);
		return -ENOMEM;
	}

	skcipher_request_set_callback(
		req, CRYPTO_TFM_REQ_MAY_BACKLOG | CRYPTO_TFM_REQ_MAY_SLEEP,
		page_crypt_complete, &ecr);

	BUILD_BUG_ON(sizeof(xts_tweak) != FS_XTS_TWEAK_SIZE);
	xts_tweak.index = cpu_to_le64(index);
	memset(xts_tweak.padding, 0, sizeof(xts_tweak.padding));

	sg_init_table(&dst, 1);
	sg_set_page(&dst, dest_page, PAGE_SIZE, 0);
	sg_init_table(&src, 1);
	sg_set_page(&src, src_page, PAGE_SIZE, 0);
	skcipher_request_set_crypt(req, &src, &dst, PAGE_SIZE, &xts_tweak);
	if (rw == FS_DECRYPT)
		res = crypto_skcipher_decrypt(req);
	else
		res = crypto_skcipher_encrypt(req);
	if (res == -EINPROGRESS || res == -EBUSY) {
		BUG_ON(req->base.data != &ecr);
		wait_for_completion(&ecr.completion);
		res = ecr.res;
	}
	skcipher_request_free(req);
	if (res) {
		printk_ratelimited(KERN_ERR
			"%s: crypto_skcipher_encrypt() returned %d\n",
			__func__, res);
>>>>>>> v4.9.227
		return res;
	}
	return 0;
}

<<<<<<< HEAD
struct page *fscrypt_alloc_bounce_page(struct fscrypt_ctx *ctx,
				       gfp_t gfp_flags)
=======
static struct page *alloc_bounce_page(struct fscrypt_ctx *ctx, gfp_t gfp_flags)
>>>>>>> v4.9.227
{
	ctx->w.bounce_page = mempool_alloc(fscrypt_bounce_page_pool, gfp_flags);
	if (ctx->w.bounce_page == NULL)
		return ERR_PTR(-ENOMEM);
<<<<<<< HEAD
	ctx->flags |= FS_CTX_HAS_BOUNCE_BUFFER_FL;
=======
	ctx->flags |= FS_WRITE_PATH_FL;
>>>>>>> v4.9.227
	return ctx->w.bounce_page;
}

/**
 * fscypt_encrypt_page() - Encrypts a page
<<<<<<< HEAD
 * @inode:     The inode for which the encryption should take place
 * @page:      The page to encrypt. Must be locked for bounce-page
 *             encryption.
 * @len:       Length of data to encrypt in @page and encrypted
 *             data in returned page.
 * @offs:      Offset of data within @page and returned
 *             page holding encrypted data.
 * @lblk_num:  Logical block number. This must be unique for multiple
 *             calls with same inode, except when overwriting
 *             previously written data.
 * @gfp_flags: The gfp flag for memory allocation
 *
 * Encrypts @page using the ctx encryption context. Performs encryption
 * either in-place or into a newly allocated bounce page.
 * Called on the page write path.
 *
 * Bounce page allocation is the default.
 * In this case, the contents of @page are encrypted and stored in an
 * allocated bounce page. @page has to be locked and the caller must call
 * fscrypt_restore_control_page() on the returned ciphertext page to
 * release the bounce buffer and the encryption context.
 *
 * In-place encryption is used by setting the FS_CFLG_OWN_PAGES flag in
 * fscrypt_operations. Here, the input-page is returned with its content
 * encrypted.
 *
 * Return: A page with the encrypted content on success. Else, an
 * error value or NULL.
 */
struct page *fscrypt_encrypt_page(const struct inode *inode,
				struct page *page,
				unsigned int len,
				unsigned int offs,
				u64 lblk_num, gfp_t gfp_flags)

{
	struct fscrypt_ctx *ctx;
	struct page *ciphertext_page = page;
	int err;

	BUG_ON(len % FS_CRYPTO_BLOCK_SIZE != 0);

	if (inode->i_sb->s_cop->flags & FS_CFLG_OWN_PAGES) {
		/* with inplace-encryption we just encrypt the page */
		err = fscrypt_do_page_crypto(inode, FS_ENCRYPT, lblk_num, page,
					     ciphertext_page, len, offs,
					     gfp_flags);
		if (err)
			return ERR_PTR(err);

		return ciphertext_page;
	}

	BUG_ON(!PageLocked(page));
=======
 * @inode:          The inode for which the encryption should take place
 * @plaintext_page: The page to encrypt. Must be locked.
 * @gfp_flags:      The gfp flag for memory allocation
 *
 * Allocates a ciphertext page and encrypts plaintext_page into it using the ctx
 * encryption context.
 *
 * Called on the page write path.  The caller must call
 * fscrypt_restore_control_page() on the returned ciphertext page to
 * release the bounce buffer and the encryption context.
 *
 * Return: An allocated page with the encrypted content on success. Else, an
 * error value or NULL.
 */
struct page *fscrypt_encrypt_page(struct inode *inode,
				struct page *plaintext_page, gfp_t gfp_flags)
{
	struct fscrypt_ctx *ctx;
	struct page *ciphertext_page = NULL;
	int err;

	BUG_ON(!PageLocked(plaintext_page));
>>>>>>> v4.9.227

	ctx = fscrypt_get_ctx(inode, gfp_flags);
	if (IS_ERR(ctx))
		return (struct page *)ctx;

	/* The encryption operation will require a bounce page. */
<<<<<<< HEAD
	ciphertext_page = fscrypt_alloc_bounce_page(ctx, gfp_flags);
	if (IS_ERR(ciphertext_page))
		goto errout;

	ctx->w.control_page = page;
	err = fscrypt_do_page_crypto(inode, FS_ENCRYPT, lblk_num,
				     page, ciphertext_page, len, offs,
				     gfp_flags);
=======
	ciphertext_page = alloc_bounce_page(ctx, gfp_flags);
	if (IS_ERR(ciphertext_page))
		goto errout;

	ctx->w.control_page = plaintext_page;
	err = do_page_crypto(inode, FS_ENCRYPT, plaintext_page->index,
					plaintext_page, ciphertext_page,
					gfp_flags);
>>>>>>> v4.9.227
	if (err) {
		ciphertext_page = ERR_PTR(err);
		goto errout;
	}
	SetPagePrivate(ciphertext_page);
	set_page_private(ciphertext_page, (unsigned long)ctx);
	lock_page(ciphertext_page);
	return ciphertext_page;

errout:
	fscrypt_release_ctx(ctx);
	return ciphertext_page;
}
EXPORT_SYMBOL(fscrypt_encrypt_page);

/**
<<<<<<< HEAD
 * fscrypt_decrypt_page() - Decrypts a page in-place
 * @inode:     The corresponding inode for the page to decrypt.
 * @page:      The page to decrypt. Must be locked in case
 *             it is a writeback page (FS_CFLG_OWN_PAGES unset).
 * @len:       Number of bytes in @page to be decrypted.
 * @offs:      Start of data in @page.
 * @lblk_num:  Logical block number.
=======
 * f2crypt_decrypt_page() - Decrypts a page in-place
 * @page: The page to decrypt. Must be locked.
>>>>>>> v4.9.227
 *
 * Decrypts page in-place using the ctx encryption context.
 *
 * Called from the read completion callback.
 *
 * Return: Zero on success, non-zero otherwise.
 */
<<<<<<< HEAD
int fscrypt_decrypt_page(const struct inode *inode, struct page *page,
			unsigned int len, unsigned int offs, u64 lblk_num)
{
	if (!(inode->i_sb->s_cop->flags & FS_CFLG_OWN_PAGES))
		BUG_ON(!PageLocked(page));

	return fscrypt_do_page_crypto(inode, FS_DECRYPT, lblk_num, page, page,
				      len, offs, GFP_NOFS);
}
EXPORT_SYMBOL(fscrypt_decrypt_page);

=======
int fscrypt_decrypt_page(struct page *page)
{
	BUG_ON(!PageLocked(page));

	return do_page_crypto(page->mapping->host,
			FS_DECRYPT, page->index, page, page, GFP_NOFS);
}
EXPORT_SYMBOL(fscrypt_decrypt_page);

int fscrypt_zeroout_range(struct inode *inode, pgoff_t lblk,
				sector_t pblk, unsigned int len)
{
	struct fscrypt_ctx *ctx;
	struct page *ciphertext_page = NULL;
	struct bio *bio;
	int ret, err = 0;

	BUG_ON(inode->i_sb->s_blocksize != PAGE_SIZE);

	ctx = fscrypt_get_ctx(inode, GFP_NOFS);
	if (IS_ERR(ctx))
		return PTR_ERR(ctx);

	ciphertext_page = alloc_bounce_page(ctx, GFP_NOWAIT);
	if (IS_ERR(ciphertext_page)) {
		err = PTR_ERR(ciphertext_page);
		goto errout;
	}

	while (len--) {
		err = do_page_crypto(inode, FS_ENCRYPT, lblk,
					ZERO_PAGE(0), ciphertext_page,
					GFP_NOFS);
		if (err)
			goto errout;

		bio = bio_alloc(GFP_NOWAIT, 1);
		if (!bio) {
			err = -ENOMEM;
			goto errout;
		}
		bio->bi_bdev = inode->i_sb->s_bdev;
		bio->bi_iter.bi_sector =
			pblk << (inode->i_sb->s_blocksize_bits - 9);
		bio_set_op_attrs(bio, REQ_OP_WRITE, 0);
		ret = bio_add_page(bio, ciphertext_page,
					inode->i_sb->s_blocksize, 0);
		if (ret != inode->i_sb->s_blocksize) {
			/* should never happen! */
			WARN_ON(1);
			bio_put(bio);
			err = -EIO;
			goto errout;
		}
		err = submit_bio_wait(bio);
		if ((err == 0) && bio->bi_error)
			err = -EIO;
		bio_put(bio);
		if (err)
			goto errout;
		lblk++;
		pblk++;
	}
	err = 0;
errout:
	fscrypt_release_ctx(ctx);
	return err;
}
EXPORT_SYMBOL(fscrypt_zeroout_range);

>>>>>>> v4.9.227
/*
 * Validate dentries for encrypted directories to make sure we aren't
 * potentially caching stale data after a key has been added or
 * removed.
 */
static int fscrypt_d_revalidate(struct dentry *dentry, unsigned int flags)
{
	struct dentry *dir;
	int dir_has_key, cached_with_key;

	if (flags & LOOKUP_RCU)
		return -ECHILD;

	dir = dget_parent(dentry);
<<<<<<< HEAD
	if (!IS_ENCRYPTED(d_inode(dir))) {
=======
	if (!d_inode(dir)->i_sb->s_cop->is_encrypted(d_inode(dir))) {
>>>>>>> v4.9.227
		dput(dir);
		return 0;
	}

<<<<<<< HEAD
=======
	/* this should eventually be an flag in d_flags */
>>>>>>> v4.9.227
	spin_lock(&dentry->d_lock);
	cached_with_key = dentry->d_flags & DCACHE_ENCRYPTED_WITH_KEY;
	spin_unlock(&dentry->d_lock);
	dir_has_key = (d_inode(dir)->i_crypt_info != NULL);
	dput(dir);

	/*
	 * If the dentry was cached without the key, and it is a
	 * negative dentry, it might be a valid name.  We can't check
	 * if the key has since been made available due to locking
	 * reasons, so we fail the validation so ext4_lookup() can do
	 * this check.
	 *
	 * We also fail the validation if the dentry was created with
	 * the key present, but we no longer have the key, or vice versa.
	 */
	if ((!cached_with_key && d_is_negative(dentry)) ||
			(!cached_with_key && dir_has_key) ||
			(cached_with_key && !dir_has_key))
		return 0;
	return 1;
}

const struct dentry_operations fscrypt_d_ops = {
	.d_revalidate = fscrypt_d_revalidate,
};
<<<<<<< HEAD
=======
EXPORT_SYMBOL(fscrypt_d_ops);

/*
 * Call fscrypt_decrypt_page on every single page, reusing the encryption
 * context.
 */
static void completion_pages(struct work_struct *work)
{
	struct fscrypt_ctx *ctx =
		container_of(work, struct fscrypt_ctx, r.work);
	struct bio *bio = ctx->r.bio;
	struct bio_vec *bv;
	int i;

	bio_for_each_segment_all(bv, bio, i) {
		struct page *page = bv->bv_page;
		int ret = fscrypt_decrypt_page(page);

		if (ret) {
			WARN_ON_ONCE(1);
			SetPageError(page);
		} else {
			SetPageUptodate(page);
		}
		unlock_page(page);
	}
	fscrypt_release_ctx(ctx);
	bio_put(bio);
}

void fscrypt_decrypt_bio_pages(struct fscrypt_ctx *ctx, struct bio *bio)
{
	INIT_WORK(&ctx->r.work, completion_pages);
	ctx->r.bio = bio;
	queue_work(fscrypt_read_workqueue, &ctx->r.work);
}
EXPORT_SYMBOL(fscrypt_decrypt_bio_pages);

void fscrypt_pullback_bio_page(struct page **page, bool restore)
{
	struct fscrypt_ctx *ctx;
	struct page *bounce_page;

	/* The bounce data pages are unmapped. */
	if ((*page)->mapping)
		return;

	/* The bounce data page is unmapped. */
	bounce_page = *page;
	ctx = (struct fscrypt_ctx *)page_private(bounce_page);

	/* restore control page */
	*page = ctx->w.control_page;

	if (restore)
		fscrypt_restore_control_page(bounce_page);
}
EXPORT_SYMBOL(fscrypt_pullback_bio_page);
>>>>>>> v4.9.227

void fscrypt_restore_control_page(struct page *page)
{
	struct fscrypt_ctx *ctx;

	ctx = (struct fscrypt_ctx *)page_private(page);
	set_page_private(page, (unsigned long)NULL);
	ClearPagePrivate(page);
	unlock_page(page);
	fscrypt_release_ctx(ctx);
}
EXPORT_SYMBOL(fscrypt_restore_control_page);

static void fscrypt_destroy(void)
{
	struct fscrypt_ctx *pos, *n;

	list_for_each_entry_safe(pos, n, &fscrypt_free_ctxs, free_list)
		kmem_cache_free(fscrypt_ctx_cachep, pos);
	INIT_LIST_HEAD(&fscrypt_free_ctxs);
	mempool_destroy(fscrypt_bounce_page_pool);
	fscrypt_bounce_page_pool = NULL;
}

/**
 * fscrypt_initialize() - allocate major buffers for fs encryption.
<<<<<<< HEAD
 * @cop_flags:  fscrypt operations flags
=======
>>>>>>> v4.9.227
 *
 * We only call this when we start accessing encrypted files, since it
 * results in memory getting allocated that wouldn't otherwise be used.
 *
 * Return: Zero on success, non-zero otherwise.
 */
<<<<<<< HEAD
int fscrypt_initialize(unsigned int cop_flags)
{
	int i, res = -ENOMEM;

	/* No need to allocate a bounce page pool if this FS won't use it. */
	if (cop_flags & FS_CFLG_OWN_PAGES)
		return 0;

=======
int fscrypt_initialize(void)
{
	int i, res = -ENOMEM;

>>>>>>> v4.9.227
	mutex_lock(&fscrypt_init_mutex);
	if (fscrypt_bounce_page_pool)
		goto already_initialized;

	for (i = 0; i < num_prealloc_crypto_ctxs; i++) {
		struct fscrypt_ctx *ctx;

		ctx = kmem_cache_zalloc(fscrypt_ctx_cachep, GFP_NOFS);
		if (!ctx)
			goto fail;
		list_add(&ctx->free_list, &fscrypt_free_ctxs);
	}

	fscrypt_bounce_page_pool =
		mempool_create_page_pool(num_prealloc_crypto_pages, 0);
	if (!fscrypt_bounce_page_pool)
		goto fail;

already_initialized:
	mutex_unlock(&fscrypt_init_mutex);
	return 0;
fail:
	fscrypt_destroy();
	mutex_unlock(&fscrypt_init_mutex);
	return res;
}
<<<<<<< HEAD

void fscrypt_msg(struct super_block *sb, const char *level,
		 const char *fmt, ...)
{
	static DEFINE_RATELIMIT_STATE(rs, DEFAULT_RATELIMIT_INTERVAL,
				      DEFAULT_RATELIMIT_BURST);
	struct va_format vaf;
	va_list args;

	if (!__ratelimit(&rs))
		return;

	va_start(args, fmt);
	vaf.fmt = fmt;
	vaf.va = &args;
	if (sb)
		printk("%sfscrypt (%s): %pV\n", level, sb->s_id, &vaf);
	else
		printk("%sfscrypt: %pV\n", level, &vaf);
	va_end(args);
}
=======
EXPORT_SYMBOL(fscrypt_initialize);
>>>>>>> v4.9.227

/**
 * fscrypt_init() - Set up for fs encryption.
 */
static int __init fscrypt_init(void)
{
	/*
	 * Use an unbound workqueue to allow bios to be decrypted in parallel
	 * even when they happen to complete on the same CPU.  This sacrifices
	 * locality, but it's worthwhile since decryption is CPU-intensive.
	 *
	 * Also use a high-priority workqueue to prioritize decryption work,
	 * which blocks reads from completing, over regular application tasks.
	 */
	fscrypt_read_workqueue = alloc_workqueue("fscrypt_read_queue",
						 WQ_UNBOUND | WQ_HIGHPRI,
						 num_online_cpus());
	if (!fscrypt_read_workqueue)
		goto fail;

	fscrypt_ctx_cachep = KMEM_CACHE(fscrypt_ctx, SLAB_RECLAIM_ACCOUNT);
	if (!fscrypt_ctx_cachep)
		goto fail_free_queue;

	fscrypt_info_cachep = KMEM_CACHE(fscrypt_info, SLAB_RECLAIM_ACCOUNT);
	if (!fscrypt_info_cachep)
		goto fail_free_ctx;

	return 0;

fail_free_ctx:
	kmem_cache_destroy(fscrypt_ctx_cachep);
fail_free_queue:
	destroy_workqueue(fscrypt_read_workqueue);
fail:
	return -ENOMEM;
}
module_init(fscrypt_init)

/**
 * fscrypt_exit() - Shutdown the fs encryption system
 */
static void __exit fscrypt_exit(void)
{
	fscrypt_destroy();

	if (fscrypt_read_workqueue)
		destroy_workqueue(fscrypt_read_workqueue);
	kmem_cache_destroy(fscrypt_ctx_cachep);
	kmem_cache_destroy(fscrypt_info_cachep);
<<<<<<< HEAD

	fscrypt_essiv_cleanup();
=======
>>>>>>> v4.9.227
}
module_exit(fscrypt_exit);

MODULE_LICENSE("GPL");
