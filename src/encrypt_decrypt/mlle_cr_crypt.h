/*
    Copyright (C) 2015 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    BSD_License.txt file for more details.

    You should have received a copy of the BSD_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#ifndef MLLE_CR_CRYPT_H_
#define MLLE_CR_CRYPT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * The cipher to use. Should be the openSSL function that returns the chosen cipher.
 * Using AES-256.
 */
#define MLLE_CR_CIPHER (EVP_aes_256_cbc())

/*
 * The hash to use. Should be the openSSL function that returns the chosen hash.
 * Using SHA-256.
 */
#define MLLE_CR_HASH (EVP_sha256())


/*
 * Key length in bytes.
 * Using AES-256, so 256 bits -> 32 bytes.
 */
#define MLLE_CR_KEY_LENGTH (32)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_CR_CRYPT_H_ */
