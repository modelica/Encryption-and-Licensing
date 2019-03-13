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

#include "libcrypto-compat.h"

#define _XOPEN_SOURCE 700

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include "mlle_cr_crypt.h"
#include "random_key_file.h"
#include "mlle_cr_decrypt.h"

/*
 * Decrypt the data pointed to by in to out, where in_len is the length of the data pointed to by in.
 *
 * The data pointed to by in is assumed to consist of IV, encrypted data and HMAC, in that order.
 * The buffer pointed to by out must be large enough to hold the decrypted data, which is always shorter than in_len.
 * The in and out buffers may be the same.
 *
 * Returns the length of the decrypted data, or -1 on an error.
 */
int mlle_cr_decrypt(char* in,
                    int in_len,
                    char* out)
{
    /* TODO: Enable better error messages. */
    EVP_CIPHER_CTX *c_ctx;
    HMAC_CTX *h_ctx;
    unsigned char *iv_in;
    unsigned char *enc_in;
    unsigned char *mac_in;
    unsigned char *out_u = (unsigned char*) out;
    unsigned char *mac = NULL;
    const EVP_CIPHER *cipher;
    int iv_len;
    const EVP_MD *hash;
    unsigned int mac_len;
    int enc_len = 0;
    int out_len = 0;
    int dec_len = 0;
    int res = -1;
    DECLARE_MLLE_CR_KEY();

    /* Init structures. */
    c_ctx = EVP_CIPHER_CTX_new(); 
    h_ctx = HMAC_CTX_new(); 
    EVP_CIPHER_CTX_init(c_ctx);
    HMAC_CTX_init(h_ctx);

    /* Select algorithms and get parameters. */
    cipher = MLLE_CR_CIPHER;
    iv_len = EVP_CIPHER_iv_length(cipher);
    hash = MLLE_CR_HASH;
    mac_len = (unsigned int) EVP_MD_size(hash);

    /* Split incoming data. */
    iv_in   = (unsigned char*) in;
    enc_in = iv_in + iv_len;
    mac_in  = iv_in + in_len - mac_len;
    enc_len = in_len - iv_len - mac_len;

    /* Set up decryption and HMAC calculation. */
    INITIALIZE_MLLE_CR_KEY();
    if (!EVP_DecryptInit_ex(c_ctx, cipher, NULL, MLLE_CR_KEY, iv_in))
        goto error;
    mac = (unsigned char*) malloc(mac_len);
    if (!mac)
        goto error;
    if (!HMAC_Init_ex(h_ctx, MLLE_CR_KEY, MLLE_CR_KEY_LEN, hash, NULL))
        goto error;
    if (!HMAC_Update(h_ctx, iv_in, iv_len))
        goto error;

    /* Decrypt data. */
    if (!EVP_DecryptUpdate(c_ctx, out_u, &dec_len, enc_in, enc_len))
        goto error;
    out_len = dec_len;
    if (!EVP_DecryptFinal_ex(c_ctx, out_u + out_len, &dec_len))
        goto error;
    out_len += dec_len;

    /* Calculate and check HMAC. */
    if (!HMAC_Update(h_ctx, out_u, out_len))
        goto error;
    if (!HMAC_Final(h_ctx, mac, &mac_len))
        goto error;
    if (memcmp(mac, mac_in, mac_len))
        goto error;
    res = out_len;

    /* Cleanup. */
error:
    CLEAR_MLLE_CR_KEY();
    EVP_CIPHER_CTX_cleanup(c_ctx);
    EVP_CIPHER_CTX_free(c_ctx);
    HMAC_CTX_free(h_ctx);
    free(mac);

    return res;
}
