/* Asymmetric public-key algorithm definitions
 *
 * See Documentation/crypto/asymmetric-keys.txt
 *
 * Copyright (C) 2012 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */

#ifndef _LINUX_PUBLIC_KEY_H
#define _LINUX_PUBLIC_KEY_H

<<<<<<< HEAD
#include <linux/mpi.h>
#include <keys/asymmetric-type.h>
#include <crypto/hash_info.h>

enum pkey_algo {
	PKEY_ALGO_DSA,
	PKEY_ALGO_RSA,
	PKEY_ALGO__LAST
};

extern const char *const pkey_algo_name[PKEY_ALGO__LAST];
extern const struct public_key_algorithm *pkey_algo[PKEY_ALGO__LAST];

/* asymmetric key implementation supports only up to SHA224 */
#define PKEY_HASH__LAST		(HASH_ALGO_SHA224 + 1)

enum pkey_id_type {
	PKEY_ID_PGP,		/* OpenPGP generated key ID */
	PKEY_ID_X509,		/* X.509 arbitrary subjectKeyIdentifier */
	PKEY_ID_TYPE__LAST
};

extern const char *const pkey_id_type_name[PKEY_ID_TYPE__LAST];

=======
>>>>>>> v4.9.227
/*
 * Cryptographic data for the public-key subtype of the asymmetric key type.
 *
 * Note that this may include private part of the key as well as the public
 * part.
 */
struct public_key {
<<<<<<< HEAD
	const struct public_key_algorithm *algo;
	u8	capabilities;
#define PKEY_CAN_ENCRYPT	0x01
#define PKEY_CAN_DECRYPT	0x02
#define PKEY_CAN_SIGN		0x04
#define PKEY_CAN_VERIFY		0x08
	enum pkey_algo pkey_algo : 8;
	enum pkey_id_type id_type : 8;
	union {
		MPI	mpi[5];
		struct {
			MPI	p;	/* DSA prime */
			MPI	q;	/* DSA group order */
			MPI	g;	/* DSA group generator */
			MPI	y;	/* DSA public-key value = g^x mod p */
			MPI	x;	/* DSA secret exponent (if present) */
		} dsa;
		struct {
			MPI	n;	/* RSA public modulus */
			MPI	e;	/* RSA public encryption exponent */
			MPI	d;	/* RSA secret encryption exponent (if present) */
			MPI	p;	/* RSA secret prime (if present) */
			MPI	q;	/* RSA secret prime (if present) */
		} rsa;
	};
};

extern void public_key_destroy(void *payload);
=======
	void *key;
	u32 keylen;
	const char *id_type;
	const char *pkey_algo;
};

extern void public_key_free(struct public_key *key);
>>>>>>> v4.9.227

/*
 * Public key cryptography signature data
 */
struct public_key_signature {
<<<<<<< HEAD
	u8 *digest;
	u8 digest_size;			/* Number of bytes in digest */
	u8 nr_mpi;			/* Occupancy of mpi[] */
	enum pkey_algo pkey_algo : 8;
	enum hash_algo pkey_hash_algo : 8;
	union {
		MPI mpi[2];
		struct {
			MPI s;		/* m^d mod n */
		} rsa;
		struct {
			MPI r;
			MPI s;
		} dsa;
	};
};

struct key;
extern int verify_signature(const struct key *key,
			    const struct public_key_signature *sig);

struct asymmetric_key_id;
extern struct key *x509_request_asymmetric_key(struct key *keyring,
					       const struct asymmetric_key_id *kid,
					       bool partial);
=======
	struct asymmetric_key_id *auth_ids[2];
	u8 *s;			/* Signature */
	u32 s_size;		/* Number of bytes in signature */
	u8 *digest;
	u8 digest_size;		/* Number of bytes in digest */
	const char *pkey_algo;
	const char *hash_algo;
};

extern void public_key_signature_free(struct public_key_signature *sig);

extern struct asymmetric_key_subtype public_key_subtype;

struct key;
struct key_type;
union key_payload;

extern int restrict_link_by_signature(struct key *trust_keyring,
				      const struct key_type *type,
				      const union key_payload *payload);

extern int verify_signature(const struct key *key,
			    const struct public_key_signature *sig);

int public_key_verify_signature(const struct public_key *pkey,
				const struct public_key_signature *sig);
>>>>>>> v4.9.227

#endif /* _LINUX_PUBLIC_KEY_H */
