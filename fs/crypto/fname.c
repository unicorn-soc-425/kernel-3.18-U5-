/*
 * This contains functions for filename crypto management
 *
 * Copyright (C) 2015, Google, Inc.
 * Copyright (C) 2015, Motorola Mobility
 *
 * Written by Uday Savagaonkar, 2014.
 * Modified by Jaegeuk Kim, 2015.
 *
 * This has not yet undergone a rigorous security audit.
 */

#include <linux/scatterlist.h>
#include <linux/ratelimit.h>
<<<<<<< HEAD
#include <crypto/skcipher.h>
#include "fscrypt_private.h"

static inline bool fscrypt_is_dot_dotdot(const struct qstr *str)
{
	if (str->len == 1 && str->name[0] == '.')
		return true;

	if (str->len == 2 && str->name[0] == '.' && str->name[1] == '.')
		return true;

	return false;
=======
#include <linux/fscrypto.h>

/**
 * fname_crypt_complete() - completion callback for filename crypto
 * @req: The asynchronous cipher request context
 * @res: The result of the cipher operation
 */
static void fname_crypt_complete(struct crypto_async_request *req, int res)
{
	struct fscrypt_completion_result *ecr = req->data;

	if (res == -EINPROGRESS)
		return;
	ecr->res = res;
	complete(&ecr->completion);
>>>>>>> v4.9.227
}

/**
 * fname_encrypt() - encrypt a filename
 *
<<<<<<< HEAD
 * The output buffer must be at least as large as the input buffer.
 * Any extra space is filled with NUL padding before encryption.
 *
 * Return: 0 on success, -errno on failure
 */
int fname_encrypt(struct inode *inode, const struct qstr *iname,
		  u8 *out, unsigned int olen)
{
	struct ablkcipher_request *req = NULL;
	DECLARE_CRYPTO_WAIT(wait);
	struct crypto_ablkcipher *tfm = inode->i_crypt_info->ci_ctfm;
	int res = 0;
	char iv[FS_CRYPTO_BLOCK_SIZE];
	struct scatterlist sg;
=======
 * The caller must have allocated sufficient memory for the @oname string.
 *
 * Return: 0 on success, -errno on failure
 */
static int fname_encrypt(struct inode *inode,
			const struct qstr *iname, struct fscrypt_str *oname)
{
	struct skcipher_request *req = NULL;
	DECLARE_FS_COMPLETION_RESULT(ecr);
	struct fscrypt_info *ci = inode->i_crypt_info;
	struct crypto_skcipher *tfm = ci->ci_ctfm;
	int res = 0;
	char iv[FS_CRYPTO_BLOCK_SIZE];
	struct scatterlist sg;
	int padding = 4 << (ci->ci_flags & FS_POLICY_FLAGS_PAD_MASK);
	unsigned int lim;
	unsigned int cryptlen;

	lim = inode->i_sb->s_cop->max_namelen(inode);
	if (iname->len <= 0 || iname->len > lim)
		return -EIO;
>>>>>>> v4.9.227

	/*
	 * Copy the filename to the output buffer for encrypting in-place and
	 * pad it with the needed number of NUL bytes.
	 */
<<<<<<< HEAD
	if (WARN_ON(olen < iname->len))
		return -ENOBUFS;
	memcpy(out, iname->name, iname->len);
	memset(out + iname->len, 0, olen - iname->len);
=======
	cryptlen = max_t(unsigned int, iname->len, FS_CRYPTO_BLOCK_SIZE);
	cryptlen = round_up(cryptlen, padding);
	cryptlen = min(cryptlen, lim);
	memcpy(oname->name, iname->name, iname->len);
	memset(oname->name + iname->len, 0, cryptlen - iname->len);
>>>>>>> v4.9.227

	/* Initialize the IV */
	memset(iv, 0, FS_CRYPTO_BLOCK_SIZE);

	/* Set up the encryption request */
<<<<<<< HEAD
	req = ablkcipher_request_alloc(tfm, GFP_NOFS);
	if (!req)
		return -ENOMEM;
	ablkcipher_request_set_callback(req,
			CRYPTO_TFM_REQ_MAY_BACKLOG | CRYPTO_TFM_REQ_MAY_SLEEP,
			crypto_req_done, &wait);
	sg_init_one(&sg, out, olen);
	ablkcipher_request_set_crypt(req, &sg, &sg, olen, iv);

	/* Do the encryption */
	res = crypto_wait_req(crypto_ablkcipher_encrypt(req), &wait);
	ablkcipher_request_free(req);
	if (res < 0) {
		fscrypt_err(inode->i_sb,
			    "Filename encryption failed for inode %lu: %d",
			    inode->i_ino, res);
		return res;
	}

=======
	req = skcipher_request_alloc(tfm, GFP_NOFS);
	if (!req) {
		printk_ratelimited(KERN_ERR
			"%s: skcipher_request_alloc() failed\n", __func__);
		return -ENOMEM;
	}
	skcipher_request_set_callback(req,
			CRYPTO_TFM_REQ_MAY_BACKLOG | CRYPTO_TFM_REQ_MAY_SLEEP,
			fname_crypt_complete, &ecr);
	sg_init_one(&sg, oname->name, cryptlen);
	skcipher_request_set_crypt(req, &sg, &sg, cryptlen, iv);

	/* Do the encryption */
	res = crypto_skcipher_encrypt(req);
	if (res == -EINPROGRESS || res == -EBUSY) {
		/* Request is being completed asynchronously; wait for it */
		wait_for_completion(&ecr.completion);
		res = ecr.res;
	}
	skcipher_request_free(req);
	if (res < 0) {
		printk_ratelimited(KERN_ERR
				"%s: Error (error code %d)\n", __func__, res);
		return res;
	}

	oname->len = cryptlen;
>>>>>>> v4.9.227
	return 0;
}

/**
 * fname_decrypt() - decrypt a filename
 *
 * The caller must have allocated sufficient memory for the @oname string.
 *
 * Return: 0 on success, -errno on failure
 */
static int fname_decrypt(struct inode *inode,
				const struct fscrypt_str *iname,
				struct fscrypt_str *oname)
{
<<<<<<< HEAD
	struct ablkcipher_request *req = NULL;
	DECLARE_CRYPTO_WAIT(wait);
	struct scatterlist src_sg, dst_sg;
	struct crypto_ablkcipher *tfm = inode->i_crypt_info->ci_ctfm;
	int res = 0;
	char iv[FS_CRYPTO_BLOCK_SIZE];

	/* Allocate request */
	req = ablkcipher_request_alloc(tfm, GFP_NOFS);
	if (!req)
		return -ENOMEM;
	ablkcipher_request_set_callback(req,
		CRYPTO_TFM_REQ_MAY_BACKLOG | CRYPTO_TFM_REQ_MAY_SLEEP,
		crypto_req_done, &wait);
=======
	struct skcipher_request *req = NULL;
	DECLARE_FS_COMPLETION_RESULT(ecr);
	struct scatterlist src_sg, dst_sg;
	struct fscrypt_info *ci = inode->i_crypt_info;
	struct crypto_skcipher *tfm = ci->ci_ctfm;
	int res = 0;
	char iv[FS_CRYPTO_BLOCK_SIZE];
	unsigned lim;

	lim = inode->i_sb->s_cop->max_namelen(inode);
	if (iname->len <= 0 || iname->len > lim)
		return -EIO;

	/* Allocate request */
	req = skcipher_request_alloc(tfm, GFP_NOFS);
	if (!req) {
		printk_ratelimited(KERN_ERR
			"%s: crypto_request_alloc() failed\n",  __func__);
		return -ENOMEM;
	}
	skcipher_request_set_callback(req,
		CRYPTO_TFM_REQ_MAY_BACKLOG | CRYPTO_TFM_REQ_MAY_SLEEP,
		fname_crypt_complete, &ecr);
>>>>>>> v4.9.227

	/* Initialize IV */
	memset(iv, 0, FS_CRYPTO_BLOCK_SIZE);

	/* Create decryption request */
	sg_init_one(&src_sg, iname->name, iname->len);
	sg_init_one(&dst_sg, oname->name, oname->len);
<<<<<<< HEAD
	ablkcipher_request_set_crypt(req, &src_sg, &dst_sg, iname->len, iv);
	res = crypto_wait_req(crypto_ablkcipher_decrypt(req), &wait);
	ablkcipher_request_free(req);
	if (res < 0) {
		fscrypt_err(inode->i_sb,
			    "Filename decryption failed for inode %lu: %d",
			    inode->i_ino, res);
=======
	skcipher_request_set_crypt(req, &src_sg, &dst_sg, iname->len, iv);
	res = crypto_skcipher_decrypt(req);
	if (res == -EINPROGRESS || res == -EBUSY) {
		wait_for_completion(&ecr.completion);
		res = ecr.res;
	}
	skcipher_request_free(req);
	if (res < 0) {
		printk_ratelimited(KERN_ERR
				"%s: Error (error code %d)\n", __func__, res);
>>>>>>> v4.9.227
		return res;
	}

	oname->len = strnlen(oname->name, iname->len);
	return 0;
}

static const char *lookup_table =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+,";

<<<<<<< HEAD
#define BASE64_CHARS(nbytes)	DIV_ROUND_UP((nbytes) * 4, 3)

=======
>>>>>>> v4.9.227
/**
 * digest_encode() -
 *
 * Encodes the input digest using characters from the set [a-zA-Z0-9_+].
 * The encoded string is roughly 4/3 times the size of the input string.
 */
static int digest_encode(const char *src, int len, char *dst)
{
	int i = 0, bits = 0, ac = 0;
	char *cp = dst;

	while (i < len) {
		ac += (((unsigned char) src[i]) << bits);
		bits += 8;
		do {
			*cp++ = lookup_table[ac & 0x3f];
			ac >>= 6;
			bits -= 6;
		} while (bits >= 6);
		i++;
	}
	if (bits)
		*cp++ = lookup_table[ac & 0x3f];
	return cp - dst;
}

static int digest_decode(const char *src, int len, char *dst)
{
	int i = 0, bits = 0, ac = 0;
	const char *p;
	char *cp = dst;

	while (i < len) {
		p = strchr(lookup_table, src[i]);
		if (p == NULL || src[i] == 0)
			return -2;
		ac += (p - lookup_table) << bits;
		bits += 6;
		if (bits >= 8) {
			*cp++ = ac & 0xff;
			ac >>= 8;
			bits -= 8;
		}
		i++;
	}
	if (ac)
		return -1;
	return cp - dst;
}

<<<<<<< HEAD
bool fscrypt_fname_encrypted_size(const struct inode *inode, u32 orig_len,
				  u32 max_len, u32 *encrypted_len_ret)
{
	int padding = 4 << (inode->i_crypt_info->ci_flags &
			    FS_POLICY_FLAGS_PAD_MASK);
	u32 encrypted_len;

	if (orig_len > max_len)
		return false;
	encrypted_len = max(orig_len, (u32)FS_CRYPTO_BLOCK_SIZE);
	encrypted_len = round_up(encrypted_len, padding);
	*encrypted_len_ret = min(encrypted_len, max_len);
	return true;
}

/**
 * fscrypt_fname_alloc_buffer - allocate a buffer for presented filenames
 *
 * Allocate a buffer that is large enough to hold any decrypted or encoded
 * filename (null-terminated), for the given maximum encrypted filename length.
 *
 * Return: 0 on success, -errno on failure
 */
int fscrypt_fname_alloc_buffer(const struct inode *inode,
			       u32 max_encrypted_len,
			       struct fscrypt_str *crypto_str)
{
	const u32 max_encoded_len =
		max_t(u32, BASE64_CHARS(FSCRYPT_FNAME_MAX_UNDIGESTED_SIZE),
		      1 + BASE64_CHARS(sizeof(struct fscrypt_digested_name)));
	u32 max_presented_len;

	max_presented_len = max(max_encoded_len, max_encrypted_len);

	crypto_str->name = kmalloc(max_presented_len + 1, GFP_NOFS);
	if (!crypto_str->name)
		return -ENOMEM;
	crypto_str->len = max_presented_len;
=======
u32 fscrypt_fname_encrypted_size(struct inode *inode, u32 ilen)
{
	int padding = 32;
	struct fscrypt_info *ci = inode->i_crypt_info;

	if (ci)
		padding = 4 << (ci->ci_flags & FS_POLICY_FLAGS_PAD_MASK);
	ilen = max(ilen, (u32)FS_CRYPTO_BLOCK_SIZE);
	return round_up(ilen, padding);
}
EXPORT_SYMBOL(fscrypt_fname_encrypted_size);

/**
 * fscrypt_fname_crypto_alloc_obuff() -
 *
 * Allocates an output buffer that is sufficient for the crypto operation
 * specified by the context and the direction.
 */
int fscrypt_fname_alloc_buffer(struct inode *inode,
				u32 ilen, struct fscrypt_str *crypto_str)
{
	unsigned int olen = fscrypt_fname_encrypted_size(inode, ilen);

	crypto_str->len = olen;
	if (olen < FS_FNAME_CRYPTO_DIGEST_SIZE * 2)
		olen = FS_FNAME_CRYPTO_DIGEST_SIZE * 2;
	/*
	 * Allocated buffer can hold one more character to null-terminate the
	 * string
	 */
	crypto_str->name = kmalloc(olen + 1, GFP_NOFS);
	if (!(crypto_str->name))
		return -ENOMEM;
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL(fscrypt_fname_alloc_buffer);

/**
<<<<<<< HEAD
 * fscrypt_fname_free_buffer - free the buffer for presented filenames
 *
 * Free the buffer allocated by fscrypt_fname_alloc_buffer().
=======
 * fscrypt_fname_crypto_free_buffer() -
 *
 * Frees the buffer allocated for crypto operation.
>>>>>>> v4.9.227
 */
void fscrypt_fname_free_buffer(struct fscrypt_str *crypto_str)
{
	if (!crypto_str)
		return;
	kfree(crypto_str->name);
	crypto_str->name = NULL;
}
EXPORT_SYMBOL(fscrypt_fname_free_buffer);

/**
 * fscrypt_fname_disk_to_usr() - converts a filename from disk space to user
 * space
 *
 * The caller must have allocated sufficient memory for the @oname string.
 *
<<<<<<< HEAD
 * If the key is available, we'll decrypt the disk name; otherwise, we'll encode
 * it for presentation.  Short names are directly base64-encoded, while long
 * names are encoded in fscrypt_digested_name format.
 *
=======
>>>>>>> v4.9.227
 * Return: 0 on success, -errno on failure
 */
int fscrypt_fname_disk_to_usr(struct inode *inode,
			u32 hash, u32 minor_hash,
			const struct fscrypt_str *iname,
			struct fscrypt_str *oname)
{
	const struct qstr qname = FSTR_TO_QSTR(iname);
<<<<<<< HEAD
	struct fscrypt_digested_name digested_name;
=======
	char buf[24];
>>>>>>> v4.9.227

	if (fscrypt_is_dot_dotdot(&qname)) {
		oname->name[0] = '.';
		oname->name[iname->len - 1] = '.';
		oname->len = iname->len;
		return 0;
	}

	if (iname->len < FS_CRYPTO_BLOCK_SIZE)
		return -EUCLEAN;

	if (inode->i_crypt_info)
		return fname_decrypt(inode, iname, oname);

<<<<<<< HEAD
	if (iname->len <= FSCRYPT_FNAME_MAX_UNDIGESTED_SIZE) {
=======
	if (iname->len <= FS_FNAME_CRYPTO_DIGEST_SIZE) {
>>>>>>> v4.9.227
		oname->len = digest_encode(iname->name, iname->len,
					   oname->name);
		return 0;
	}
	if (hash) {
<<<<<<< HEAD
		digested_name.hash = hash;
		digested_name.minor_hash = minor_hash;
	} else {
		digested_name.hash = 0;
		digested_name.minor_hash = 0;
	}
	memcpy(digested_name.digest,
	       FSCRYPT_FNAME_DIGEST(iname->name, iname->len),
	       FSCRYPT_FNAME_DIGEST_SIZE);
	oname->name[0] = '_';
	oname->len = 1 + digest_encode((const char *)&digested_name,
				       sizeof(digested_name), oname->name + 1);
=======
		memcpy(buf, &hash, 4);
		memcpy(buf + 4, &minor_hash, 4);
	} else {
		memset(buf, 0, 8);
	}
	memcpy(buf + 8, iname->name + ((iname->len - 17) & ~15), 16);
	oname->name[0] = '_';
	oname->len = 1 + digest_encode(buf, 24, oname->name + 1);
>>>>>>> v4.9.227
	return 0;
}
EXPORT_SYMBOL(fscrypt_fname_disk_to_usr);

/**
<<<<<<< HEAD
 * fscrypt_setup_filename() - prepare to search a possibly encrypted directory
 * @dir: the directory that will be searched
 * @iname: the user-provided filename being searched for
 * @lookup: 1 if we're allowed to proceed without the key because it's
 *	->lookup() or we're finding the dir_entry for deletion; 0 if we cannot
 *	proceed without the key because we're going to create the dir_entry.
 * @fname: the filename information to be filled in
 *
 * Given a user-provided filename @iname, this function sets @fname->disk_name
 * to the name that would be stored in the on-disk directory entry, if possible.
 * If the directory is unencrypted this is simply @iname.  Else, if we have the
 * directory's encryption key, then @iname is the plaintext, so we encrypt it to
 * get the disk_name.
 *
 * Else, for keyless @lookup operations, @iname is the presented ciphertext, so
 * we decode it to get either the ciphertext disk_name (for short names) or the
 * fscrypt_digested_name (for long names).  Non-@lookup operations will be
 * impossible in this case, so we fail them with ENOKEY.
 *
 * If successful, fscrypt_free_filename() must be called later to clean up.
 *
 * Return: 0 on success, -errno on failure
 */
int fscrypt_setup_filename(struct inode *dir, const struct qstr *iname,
			      int lookup, struct fscrypt_name *fname)
{
	int ret;
	int digested;
=======
 * fscrypt_fname_usr_to_disk() - converts a filename from user space to disk
 * space
 *
 * The caller must have allocated sufficient memory for the @oname string.
 *
 * Return: 0 on success, -errno on failure
 */
int fscrypt_fname_usr_to_disk(struct inode *inode,
			const struct qstr *iname,
			struct fscrypt_str *oname)
{
	if (fscrypt_is_dot_dotdot(iname)) {
		oname->name[0] = '.';
		oname->name[iname->len - 1] = '.';
		oname->len = iname->len;
		return 0;
	}
	if (inode->i_crypt_info)
		return fname_encrypt(inode, iname, oname);
	/*
	 * Without a proper key, a user is not allowed to modify the filenames
	 * in a directory. Consequently, a user space name cannot be mapped to
	 * a disk-space name
	 */
	return -ENOKEY;
}
EXPORT_SYMBOL(fscrypt_fname_usr_to_disk);

int fscrypt_setup_filename(struct inode *dir, const struct qstr *iname,
			      int lookup, struct fscrypt_name *fname)
{
	int ret = 0, bigname = 0;
>>>>>>> v4.9.227

	memset(fname, 0, sizeof(struct fscrypt_name));
	fname->usr_fname = iname;

<<<<<<< HEAD
	if (!IS_ENCRYPTED(dir) || fscrypt_is_dot_dotdot(iname)) {
=======
	if (!dir->i_sb->s_cop->is_encrypted(dir) ||
				fscrypt_is_dot_dotdot(iname)) {
>>>>>>> v4.9.227
		fname->disk_name.name = (unsigned char *)iname->name;
		fname->disk_name.len = iname->len;
		return 0;
	}
	ret = fscrypt_get_encryption_info(dir);
<<<<<<< HEAD
	if (ret)
		return ret;

	if (dir->i_crypt_info) {
		if (!fscrypt_fname_encrypted_size(dir, iname->len,
						  dir->i_sb->s_cop->max_namelen,
						  &fname->crypto_buf.len))
			return -ENAMETOOLONG;
		fname->crypto_buf.name = kmalloc(fname->crypto_buf.len,
						 GFP_NOFS);
		if (!fname->crypto_buf.name)
			return -ENOMEM;

		ret = fname_encrypt(dir, iname, fname->crypto_buf.name,
				    fname->crypto_buf.len);
=======
	if (ret && ret != -EOPNOTSUPP)
		return ret;

	if (dir->i_crypt_info) {
		ret = fscrypt_fname_alloc_buffer(dir, iname->len,
							&fname->crypto_buf);
		if (ret)
			return ret;
		ret = fname_encrypt(dir, iname, &fname->crypto_buf);
>>>>>>> v4.9.227
		if (ret)
			goto errout;
		fname->disk_name.name = fname->crypto_buf.name;
		fname->disk_name.len = fname->crypto_buf.len;
		return 0;
	}
	if (!lookup)
		return -ENOKEY;

	/*
	 * We don't have the key and we are doing a lookup; decode the
	 * user-supplied name
	 */
<<<<<<< HEAD
	if (iname->name[0] == '_') {
		if (iname->len !=
		    1 + BASE64_CHARS(sizeof(struct fscrypt_digested_name)))
			return -ENOENT;
		digested = 1;
	} else {
		if (iname->len >
		    BASE64_CHARS(FSCRYPT_FNAME_MAX_UNDIGESTED_SIZE))
			return -ENOENT;
		digested = 0;
	}

	fname->crypto_buf.name =
		kmalloc(max_t(size_t, FSCRYPT_FNAME_MAX_UNDIGESTED_SIZE,
			      sizeof(struct fscrypt_digested_name)),
			GFP_KERNEL);
	if (fname->crypto_buf.name == NULL)
		return -ENOMEM;

	ret = digest_decode(iname->name + digested, iname->len - digested,
=======
	if (iname->name[0] == '_')
		bigname = 1;
	if ((bigname && (iname->len != 33)) || (!bigname && (iname->len > 43)))
		return -ENOENT;

	fname->crypto_buf.name = kmalloc(32, GFP_KERNEL);
	if (fname->crypto_buf.name == NULL)
		return -ENOMEM;

	ret = digest_decode(iname->name + bigname, iname->len - bigname,
>>>>>>> v4.9.227
				fname->crypto_buf.name);
	if (ret < 0) {
		ret = -ENOENT;
		goto errout;
	}
	fname->crypto_buf.len = ret;
<<<<<<< HEAD
	if (digested) {
		const struct fscrypt_digested_name *n =
			(const void *)fname->crypto_buf.name;
		fname->hash = n->hash;
		fname->minor_hash = n->minor_hash;
=======
	if (bigname) {
		memcpy(&fname->hash, fname->crypto_buf.name, 4);
		memcpy(&fname->minor_hash, fname->crypto_buf.name + 4, 4);
>>>>>>> v4.9.227
	} else {
		fname->disk_name.name = fname->crypto_buf.name;
		fname->disk_name.len = fname->crypto_buf.len;
	}
	return 0;

errout:
<<<<<<< HEAD
	kfree(fname->crypto_buf.name);
	return ret;
}
EXPORT_SYMBOL(fscrypt_setup_filename);
=======
	fscrypt_fname_free_buffer(&fname->crypto_buf);
	return ret;
}
EXPORT_SYMBOL(fscrypt_setup_filename);

void fscrypt_free_filename(struct fscrypt_name *fname)
{
	kfree(fname->crypto_buf.name);
	fname->crypto_buf.name = NULL;
	fname->usr_fname = NULL;
	fname->disk_name.name = NULL;
}
EXPORT_SYMBOL(fscrypt_free_filename);
>>>>>>> v4.9.227
