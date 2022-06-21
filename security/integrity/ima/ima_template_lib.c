/*
 * Copyright (C) 2013 Politecnico di Torino, Italy
 *                    TORSEC group -- http://security.polito.it
 *
 * Author: Roberto Sassu <roberto.sassu@polito.it>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2 of the
 * License.
 *
 * File: ima_template_lib.c
 *      Library of supported template fields.
 */
<<<<<<< HEAD
#include <crypto/hash_info.h>
=======
>>>>>>> v4.9.227

#include "ima_template_lib.h"

static bool ima_template_hash_algo_allowed(u8 algo)
{
	if (algo == HASH_ALGO_SHA1 || algo == HASH_ALGO_MD5)
		return true;

	return false;
}

enum data_formats {
	DATA_FMT_DIGEST = 0,
	DATA_FMT_DIGEST_WITH_ALGO,
	DATA_FMT_STRING,
	DATA_FMT_HEX
};

static int ima_write_template_field_data(const void *data, const u32 datalen,
					 enum data_formats datafmt,
					 struct ima_field_data *field_data)
{
	u8 *buf, *buf_ptr;
	u32 buflen = datalen;

	if (datafmt == DATA_FMT_STRING)
		buflen = datalen + 1;

	buf = kzalloc(buflen, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	memcpy(buf, data, datalen);

	/*
	 * Replace all space characters with underscore for event names and
	 * strings. This avoid that, during the parsing of a measurements list,
	 * filenames with spaces or that end with the suffix ' (deleted)' are
	 * split into multiple template fields (the space is the delimitator
	 * character for measurements lists in ASCII format).
	 */
	if (datafmt == DATA_FMT_STRING) {
		for (buf_ptr = buf; buf_ptr - buf < datalen; buf_ptr++)
			if (*buf_ptr == ' ')
				*buf_ptr = '_';
	}

	field_data->data = buf;
	field_data->len = buflen;
	return 0;
}

static void ima_show_template_data_ascii(struct seq_file *m,
					 enum ima_show_type show,
					 enum data_formats datafmt,
					 struct ima_field_data *field_data)
{
	u8 *buf_ptr = field_data->data;
	u32 buflen = field_data->len;

	switch (datafmt) {
	case DATA_FMT_DIGEST_WITH_ALGO:
		buf_ptr = strnchr(field_data->data, buflen, ':');
		if (buf_ptr != field_data->data)
			seq_printf(m, "%s", field_data->data);

		/* skip ':' and '\0' */
		buf_ptr += 2;
		buflen -= buf_ptr - field_data->data;
	case DATA_FMT_DIGEST:
	case DATA_FMT_HEX:
		if (!buflen)
			break;
		ima_print_digest(m, buf_ptr, buflen);
		break;
	case DATA_FMT_STRING:
		seq_printf(m, "%s", buf_ptr);
		break;
	default:
		break;
	}
}

static void ima_show_template_data_binary(struct seq_file *m,
					  enum ima_show_type show,
					  enum data_formats datafmt,
					  struct ima_field_data *field_data)
{
	u32 len = (show == IMA_SHOW_BINARY_OLD_STRING_FMT) ?
	    strlen(field_data->data) : field_data->len;

	if (show != IMA_SHOW_BINARY_NO_FIELD_LEN)
		ima_putc(m, &len, sizeof(len));

	if (!len)
		return;

	ima_putc(m, field_data->data, len);
}

static void ima_show_template_field_data(struct seq_file *m,
					 enum ima_show_type show,
					 enum data_formats datafmt,
					 struct ima_field_data *field_data)
{
	switch (show) {
	case IMA_SHOW_ASCII:
		ima_show_template_data_ascii(m, show, datafmt, field_data);
		break;
	case IMA_SHOW_BINARY:
	case IMA_SHOW_BINARY_NO_FIELD_LEN:
	case IMA_SHOW_BINARY_OLD_STRING_FMT:
		ima_show_template_data_binary(m, show, datafmt, field_data);
		break;
	default:
		break;
	}
}

void ima_show_template_digest(struct seq_file *m, enum ima_show_type show,
			      struct ima_field_data *field_data)
{
	ima_show_template_field_data(m, show, DATA_FMT_DIGEST, field_data);
}

void ima_show_template_digest_ng(struct seq_file *m, enum ima_show_type show,
				 struct ima_field_data *field_data)
{
	ima_show_template_field_data(m, show, DATA_FMT_DIGEST_WITH_ALGO,
				     field_data);
}

void ima_show_template_string(struct seq_file *m, enum ima_show_type show,
			      struct ima_field_data *field_data)
{
	ima_show_template_field_data(m, show, DATA_FMT_STRING, field_data);
}

void ima_show_template_sig(struct seq_file *m, enum ima_show_type show,
			   struct ima_field_data *field_data)
{
	ima_show_template_field_data(m, show, DATA_FMT_HEX, field_data);
}

static int ima_eventdigest_init_common(u8 *digest, u32 digestsize, u8 hash_algo,
				       struct ima_field_data *field_data)
{
	/*
	 * digest formats:
	 *  - DATA_FMT_DIGEST: digest
	 *  - DATA_FMT_DIGEST_WITH_ALGO: [<hash algo>] + ':' + '\0' + digest,
	 *    where <hash algo> is provided if the hash algoritm is not
	 *    SHA1 or MD5
	 */
	u8 buffer[CRYPTO_MAX_ALG_NAME + 2 + IMA_MAX_DIGEST_SIZE] = { 0 };
	enum data_formats fmt = DATA_FMT_DIGEST;
	u32 offset = 0;

	if (hash_algo < HASH_ALGO__LAST) {
		fmt = DATA_FMT_DIGEST_WITH_ALGO;
		offset += snprintf(buffer, CRYPTO_MAX_ALG_NAME + 1, "%s",
				   hash_algo_name[hash_algo]);
		buffer[offset] = ':';
		offset += 2;
	}

	if (digest)
		memcpy(buffer + offset, digest, digestsize);
	else
		/*
		 * If digest is NULL, the event being recorded is a violation.
		 * Make room for the digest by increasing the offset of
		 * IMA_DIGEST_SIZE.
		 */
		offset += IMA_DIGEST_SIZE;

	return ima_write_template_field_data(buffer, offset + digestsize,
					     fmt, field_data);
}

/*
 * This function writes the digest of an event (with size limit).
 */
<<<<<<< HEAD
int ima_eventdigest_init(struct integrity_iint_cache *iint, struct file *file,
			 const unsigned char *filename,
			 struct evm_ima_xattr_data *xattr_value, int xattr_len,
=======
int ima_eventdigest_init(struct ima_event_data *event_data,
>>>>>>> v4.9.227
			 struct ima_field_data *field_data)
{
	struct {
		struct ima_digest_data hdr;
		char digest[IMA_MAX_DIGEST_SIZE];
	} hash;
	u8 *cur_digest = NULL;
	u32 cur_digestsize = 0;
	struct inode *inode;
	int result;

	memset(&hash, 0, sizeof(hash));

<<<<<<< HEAD
	if (!iint)		/* recording a violation. */
		goto out;

	if (ima_template_hash_algo_allowed(iint->ima_hash->algo)) {
		cur_digest = iint->ima_hash->digest;
		cur_digestsize = iint->ima_hash->length;
		goto out;
	}

	if (!file)		/* missing info to re-calculate the digest */
		return -EINVAL;

	inode = file_inode(file);
	hash.hdr.algo = ima_template_hash_algo_allowed(ima_hash_algo) ?
	    ima_hash_algo : HASH_ALGO_SHA1;
	result = ima_calc_file_hash(file, &hash.hdr);
	if (result) {
		integrity_audit_msg(AUDIT_INTEGRITY_DATA, inode,
				    filename, "collect_data",
=======
	if (event_data->violation)	/* recording a violation. */
		goto out;

	if (ima_template_hash_algo_allowed(event_data->iint->ima_hash->algo)) {
		cur_digest = event_data->iint->ima_hash->digest;
		cur_digestsize = event_data->iint->ima_hash->length;
		goto out;
	}

	if (!event_data->file)	/* missing info to re-calculate the digest */
		return -EINVAL;

	inode = file_inode(event_data->file);
	hash.hdr.algo = ima_template_hash_algo_allowed(ima_hash_algo) ?
	    ima_hash_algo : HASH_ALGO_SHA1;
	result = ima_calc_file_hash(event_data->file, &hash.hdr);
	if (result) {
		integrity_audit_msg(AUDIT_INTEGRITY_DATA, inode,
				    event_data->filename, "collect_data",
>>>>>>> v4.9.227
				    "failed", result, 0);
		return result;
	}
	cur_digest = hash.hdr.digest;
	cur_digestsize = hash.hdr.length;
out:
	return ima_eventdigest_init_common(cur_digest, cur_digestsize,
					   HASH_ALGO__LAST, field_data);
}

/*
 * This function writes the digest of an event (without size limit).
 */
<<<<<<< HEAD
int ima_eventdigest_ng_init(struct integrity_iint_cache *iint,
			    struct file *file, const unsigned char *filename,
			    struct evm_ima_xattr_data *xattr_value,
			    int xattr_len, struct ima_field_data *field_data)
=======
int ima_eventdigest_ng_init(struct ima_event_data *event_data,
			    struct ima_field_data *field_data)
>>>>>>> v4.9.227
{
	u8 *cur_digest = NULL, hash_algo = HASH_ALGO_SHA1;
	u32 cur_digestsize = 0;

<<<<<<< HEAD
	/* If iint is NULL, we are recording a violation. */
	if (!iint)
		goto out;

	cur_digest = iint->ima_hash->digest;
	cur_digestsize = iint->ima_hash->length;

	hash_algo = iint->ima_hash->algo;
=======
	if (event_data->violation)	/* recording a violation. */
		goto out;

	cur_digest = event_data->iint->ima_hash->digest;
	cur_digestsize = event_data->iint->ima_hash->length;

	hash_algo = event_data->iint->ima_hash->algo;
>>>>>>> v4.9.227
out:
	return ima_eventdigest_init_common(cur_digest, cur_digestsize,
					   hash_algo, field_data);
}

<<<<<<< HEAD
static int ima_eventname_init_common(struct integrity_iint_cache *iint,
				     struct file *file,
				     const unsigned char *filename,
=======
static int ima_eventname_init_common(struct ima_event_data *event_data,
>>>>>>> v4.9.227
				     struct ima_field_data *field_data,
				     bool size_limit)
{
	const char *cur_filename = NULL;
	u32 cur_filename_len = 0;

<<<<<<< HEAD
	BUG_ON(filename == NULL && file == NULL);

	if (filename) {
		cur_filename = filename;
		cur_filename_len = strlen(filename);
=======
	BUG_ON(event_data->filename == NULL && event_data->file == NULL);

	if (event_data->filename) {
		cur_filename = event_data->filename;
		cur_filename_len = strlen(event_data->filename);
>>>>>>> v4.9.227

		if (!size_limit || cur_filename_len <= IMA_EVENT_NAME_LEN_MAX)
			goto out;
	}

<<<<<<< HEAD
	if (file) {
		cur_filename = file->f_dentry->d_name.name;
=======
	if (event_data->file) {
		cur_filename = event_data->file->f_path.dentry->d_name.name;
>>>>>>> v4.9.227
		cur_filename_len = strlen(cur_filename);
	} else
		/*
		 * Truncate filename if the latter is too long and
		 * the file descriptor is not available.
		 */
		cur_filename_len = IMA_EVENT_NAME_LEN_MAX;
out:
	return ima_write_template_field_data(cur_filename, cur_filename_len,
					     DATA_FMT_STRING, field_data);
}

/*
 * This function writes the name of an event (with size limit).
 */
<<<<<<< HEAD
int ima_eventname_init(struct integrity_iint_cache *iint, struct file *file,
		       const unsigned char *filename,
		       struct evm_ima_xattr_data *xattr_value, int xattr_len,
		       struct ima_field_data *field_data)
{
	return ima_eventname_init_common(iint, file, filename,
					 field_data, true);
=======
int ima_eventname_init(struct ima_event_data *event_data,
		       struct ima_field_data *field_data)
{
	return ima_eventname_init_common(event_data, field_data, true);
>>>>>>> v4.9.227
}

/*
 * This function writes the name of an event (without size limit).
 */
<<<<<<< HEAD
int ima_eventname_ng_init(struct integrity_iint_cache *iint, struct file *file,
			  const unsigned char *filename,
			  struct evm_ima_xattr_data *xattr_value, int xattr_len,
			  struct ima_field_data *field_data)
{
	return ima_eventname_init_common(iint, file, filename,
					 field_data, false);
=======
int ima_eventname_ng_init(struct ima_event_data *event_data,
			  struct ima_field_data *field_data)
{
	return ima_eventname_init_common(event_data, field_data, false);
>>>>>>> v4.9.227
}

/*
 *  ima_eventsig_init - include the file signature as part of the template data
 */
<<<<<<< HEAD
int ima_eventsig_init(struct integrity_iint_cache *iint, struct file *file,
		      const unsigned char *filename,
		      struct evm_ima_xattr_data *xattr_value, int xattr_len,
		      struct ima_field_data *field_data)
{
	enum data_formats fmt = DATA_FMT_HEX;
=======
int ima_eventsig_init(struct ima_event_data *event_data,
		      struct ima_field_data *field_data)
{
	enum data_formats fmt = DATA_FMT_HEX;
	struct evm_ima_xattr_data *xattr_value = event_data->xattr_value;
	int xattr_len = event_data->xattr_len;
>>>>>>> v4.9.227
	int rc = 0;

	if ((!xattr_value) || (xattr_value->type != EVM_IMA_XATTR_DIGSIG))
		goto out;

	rc = ima_write_template_field_data(xattr_value, xattr_len, fmt,
					   field_data);
out:
	return rc;
}
