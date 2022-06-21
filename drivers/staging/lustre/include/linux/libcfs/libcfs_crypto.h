/* GPL HEADER START
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 only,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License version 2 for more details (a copy is included
 * in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; If not, see http://www.gnu.org/licenses
 *
 * Please  visit http://www.xyratex.com/contact if you need additional
 * information or have any questions.
 *
 * GPL HEADER END
 */

/*
 * Copyright 2012 Xyratex Technology Limited
 */

#ifndef _LIBCFS_CRYPTO_H
#define _LIBCFS_CRYPTO_H

struct cfs_crypto_hash_type {
	char		*cht_name;      /**< hash algorithm name, equal to
					 * format name for crypto api */
	unsigned int    cht_key;	/**< init key by default (valid for
					 * 4 bytes context like crc32, adler */
	unsigned int    cht_size;       /**< hash digest size */
};

enum cfs_crypto_hash_alg {
	CFS_HASH_ALG_NULL       = 0,
	CFS_HASH_ALG_ADLER32,
	CFS_HASH_ALG_CRC32,
	CFS_HASH_ALG_MD5,
	CFS_HASH_ALG_SHA1,
	CFS_HASH_ALG_SHA256,
	CFS_HASH_ALG_SHA384,
	CFS_HASH_ALG_SHA512,
	CFS_HASH_ALG_CRC32C,
<<<<<<< HEAD
	CFS_HASH_ALG_MAX
=======
	CFS_HASH_ALG_MAX,
	CFS_HASH_ALG_UNKNOWN	= 0xff
>>>>>>> v4.9.227
};

static struct cfs_crypto_hash_type hash_types[] = {
	[CFS_HASH_ALG_NULL]    = { "null",     0,      0 },
	[CFS_HASH_ALG_ADLER32] = { "adler32",  1,      4 },
	[CFS_HASH_ALG_CRC32]   = { "crc32",   ~0,      4 },
	[CFS_HASH_ALG_CRC32C]  = { "crc32c",  ~0,      4 },
	[CFS_HASH_ALG_MD5]     = { "md5",      0,     16 },
	[CFS_HASH_ALG_SHA1]    = { "sha1",     0,     20 },
	[CFS_HASH_ALG_SHA256]  = { "sha256",   0,     32 },
	[CFS_HASH_ALG_SHA384]  = { "sha384",   0,     48 },
	[CFS_HASH_ALG_SHA512]  = { "sha512",   0,     64 },
<<<<<<< HEAD
};

/**    Return pointer to type of hash for valid hash algorithm identifier */
static inline const struct cfs_crypto_hash_type *
		    cfs_crypto_hash_type(unsigned char hash_alg)
=======
	[CFS_HASH_ALG_MAX]	= { NULL,	0,	64 },
};

/* Maximum size of hash_types[].cht_size */
#define CFS_CRYPTO_HASH_DIGESTSIZE_MAX	64

/**
 * Return hash algorithm information for the specified algorithm identifier
 *
 * Hash information includes algorithm name, initial seed, hash size.
 *
 * \retval	cfs_crypto_hash_type for valid ID (CFS_HASH_ALG_*)
 * \retval	NULL for unknown algorithm identifier
 */
static inline const struct cfs_crypto_hash_type *
cfs_crypto_hash_type(enum cfs_crypto_hash_alg hash_alg)
>>>>>>> v4.9.227
{
	struct cfs_crypto_hash_type *ht;

	if (hash_alg < CFS_HASH_ALG_MAX) {
		ht = &hash_types[hash_alg];
		if (ht->cht_name)
			return ht;
	}
	return NULL;
}

<<<<<<< HEAD
/**     Return hash name for valid hash algorithm identifier or "unknown" */
static inline const char *cfs_crypto_hash_name(unsigned char hash_alg)
=======
/**
 * Return hash name for hash algorithm identifier
 *
 * \param[in]	hash_alg hash alrgorithm id (CFS_HASH_ALG_*)
 *
 * \retval	string name of known hash algorithm
 * \retval	"unknown" if hash algorithm is unknown
 */
static inline const char *
cfs_crypto_hash_name(enum cfs_crypto_hash_alg hash_alg)
>>>>>>> v4.9.227
{
	const struct cfs_crypto_hash_type *ht;

	ht = cfs_crypto_hash_type(hash_alg);
	if (ht)
		return ht->cht_name;
	return "unknown";
}

<<<<<<< HEAD
/**     Return digest size for valid algorithm identifier or 0 */
static inline int cfs_crypto_hash_digestsize(unsigned char hash_alg)
=======
/**
 * Return digest size for hash algorithm type
 *
 * \param[in]	hash_alg hash alrgorithm id (CFS_HASH_ALG_*)
 *
 * \retval	hash algorithm digest size in bytes
 * \retval	0 if hash algorithm type is unknown
 */
static inline int cfs_crypto_hash_digestsize(enum cfs_crypto_hash_alg hash_alg)
>>>>>>> v4.9.227
{
	const struct cfs_crypto_hash_type *ht;

	ht = cfs_crypto_hash_type(hash_alg);
	if (ht)
		return ht->cht_size;
	return 0;
}

<<<<<<< HEAD
/**     Return hash identifier for valid hash algorithm name or 0xFF */
static inline unsigned char cfs_crypto_hash_alg(const char *algname)
{
	unsigned char   i;

	for (i = 0; i < CFS_HASH_ALG_MAX; i++)
		if (!strcmp(hash_types[i].cht_name, algname))
			break;
	return (i == CFS_HASH_ALG_MAX ? 0xFF : i);
}

/**     Calculate hash digest for buffer.
 *      @param alg	    id of hash algorithm
 *      @param buf	    buffer of data
 *      @param buf_len	buffer len
 *      @param key	    initial value for algorithm, if it is NULL,
 *			    default initial value should be used.
 *      @param key_len	len of initial value
 *      @param hash	   [out] pointer to hash, if it is NULL, hash_len is
 *			    set to valid digest size in bytes, retval -ENOSPC.
 *      @param hash_len       [in,out] size of hash buffer
 *      @returns	      status of operation
 *      @retval -EINVAL       if buf, buf_len, hash_len or alg_id is invalid
 *      @retval -ENODEV       if this algorithm is unsupported
 *      @retval -ENOSPC       if pointer to hash is NULL, or hash_len less than
 *			    digest size
 *      @retval 0	     for success
 *      @retval < 0	   other errors from lower layers.
 */
int cfs_crypto_hash_digest(unsigned char alg,
=======
/**
 * Find hash algorithm ID for the specified algorithm name
 *
 * \retval	hash algorithm ID for valid ID (CFS_HASH_ALG_*)
 * \retval	CFS_HASH_ALG_UNKNOWN for unknown algorithm name
 */
static inline unsigned char cfs_crypto_hash_alg(const char *algname)
{
	enum cfs_crypto_hash_alg hash_alg;

	for (hash_alg = 0; hash_alg < CFS_HASH_ALG_MAX; hash_alg++)
		if (strcmp(hash_types[hash_alg].cht_name, algname) == 0)
			return hash_alg;

	return CFS_HASH_ALG_UNKNOWN;
}

int cfs_crypto_hash_digest(enum cfs_crypto_hash_alg hash_alg,
>>>>>>> v4.9.227
			   const void *buf, unsigned int buf_len,
			   unsigned char *key, unsigned int key_len,
			   unsigned char *hash, unsigned int *hash_len);

/* cfs crypto hash descriptor */
struct cfs_crypto_hash_desc;

<<<<<<< HEAD
/**     Allocate and initialize descriptor for hash algorithm.
 *      @param alg	    algorithm id
 *      @param key	    initial value for algorithm, if it is NULL,
 *			    default initial value should be used.
 *      @param key_len	len of initial value
 *      @returns	      pointer to descriptor of hash instance
 *      @retval ERR_PTR(error) when errors occurred.
 */
struct cfs_crypto_hash_desc*
	cfs_crypto_hash_init(unsigned char alg,
			     unsigned char *key, unsigned int key_len);

/**    Update digest by part of data.
 *     @param desc	      hash descriptor
 *     @param page	      data page
 *     @param offset	    data offset
 *     @param len	       data len
 *     @returns		 status of operation
 *     @retval 0		for success.
 */
int cfs_crypto_hash_update_page(struct cfs_crypto_hash_desc *desc,
				struct page *page, unsigned int offset,
				unsigned int len);

/**    Update digest by part of data.
 *     @param desc	      hash descriptor
 *     @param buf	       pointer to data buffer
 *     @param buf_len	   size of data at buffer
 *     @returns		 status of operation
 *     @retval 0		for success.
 */
int cfs_crypto_hash_update(struct cfs_crypto_hash_desc *desc, const void *buf,
			   unsigned int buf_len);

/**    Finalize hash calculation, copy hash digest to buffer, destroy hash
 *     descriptor.
 *     @param desc	      hash descriptor
 *     @param hash	      buffer pointer to store hash digest
 *     @param hash_len	  pointer to hash buffer size, if NULL
 *			      destroy hash descriptor
 *     @returns		 status of operation
 *     @retval -ENOSPC	  if hash is NULL, or *hash_len less than
 *			      digest size
 *     @retval 0		for success
 *     @retval < 0	      other errors from lower layers.
 */
int cfs_crypto_hash_final(struct cfs_crypto_hash_desc *desc,
			  unsigned char *hash, unsigned int *hash_len);
/**
 *      Register crypto hash algorithms
 */
int cfs_crypto_register(void);

/**
 *      Unregister
 */
void cfs_crypto_unregister(void);

/**     Return hash speed in Mbytes per second for valid hash algorithm
 *      identifier. If test was unsuccessful -1 would be returned.
 */
int cfs_crypto_hash_speed(unsigned char hash_alg);
=======
struct cfs_crypto_hash_desc *
cfs_crypto_hash_init(enum cfs_crypto_hash_alg hash_alg,
		     unsigned char *key, unsigned int key_len);
int cfs_crypto_hash_update_page(struct cfs_crypto_hash_desc *desc,
				struct page *page, unsigned int offset,
				unsigned int len);
int cfs_crypto_hash_update(struct cfs_crypto_hash_desc *desc, const void *buf,
			   unsigned int buf_len);
int cfs_crypto_hash_final(struct cfs_crypto_hash_desc *desc,
			  unsigned char *hash, unsigned int *hash_len);
int cfs_crypto_register(void);
void cfs_crypto_unregister(void);
int cfs_crypto_hash_speed(enum cfs_crypto_hash_alg hash_alg);
>>>>>>> v4.9.227
#endif
