/* PKCS#7 crypto data parser
 *
 * Copyright (C) 2012 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */

<<<<<<< HEAD
=======
#ifndef _CRYPTO_PKCS7_H
#define _CRYPTO_PKCS7_H

#include <linux/verification.h>
#include <crypto/public_key.h>

>>>>>>> v4.9.227
struct key;
struct pkcs7_message;

/*
 * pkcs7_parser.c
 */
extern struct pkcs7_message *pkcs7_parse_message(const void *data,
						 size_t datalen);
extern void pkcs7_free_message(struct pkcs7_message *pkcs7);

extern int pkcs7_get_content_data(const struct pkcs7_message *pkcs7,
				  const void **_data, size_t *_datalen,
<<<<<<< HEAD
				  bool want_wrapper);
=======
				  size_t *_headerlen);
>>>>>>> v4.9.227

/*
 * pkcs7_trust.c
 */
extern int pkcs7_validate_trust(struct pkcs7_message *pkcs7,
<<<<<<< HEAD
				struct key *trust_keyring,
				bool *_trusted);
=======
				struct key *trust_keyring);
>>>>>>> v4.9.227

/*
 * pkcs7_verify.c
 */
<<<<<<< HEAD
extern int pkcs7_verify(struct pkcs7_message *pkcs7);
=======
extern int pkcs7_verify(struct pkcs7_message *pkcs7,
			enum key_being_used_for usage);

extern int pkcs7_supply_detached_data(struct pkcs7_message *pkcs7,
				      const void *data, size_t datalen);

#endif /* _CRYPTO_PKCS7_H */
>>>>>>> v4.9.227
