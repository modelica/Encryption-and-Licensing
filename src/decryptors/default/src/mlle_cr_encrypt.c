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

#define _XOPEN_SOURCE 700
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* libcrypto-compat.h must be first */
#include "libcrypto-compat.h"

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include "mlle_cr_crypt.h"
#include "random_key_file.h"

#ifdef _WIN32

/* Arbitrary, but should be "enough". */
#define WIN_PRNG_SEED_SIZE (256)

/*
 * Make sure PRNG is seeded properly.
 * Returns 1 on success, and 0 on failure.
 */
static int mlle_cr_seed() {
    HCRYPTPROV win_crypt_prov;
    int win_seed_res;
    char win_seed_buf[WIN_PRNG_SEED_SIZE];

    if (!RAND_status()) {
        CryptAcquireContext(&win_crypt_prov, NULL, NULL, PROV_RSA_FULL, CRYPT_SILENT);
        win_seed_res = CryptGenRandom(win_crypt_prov, WIN_PRNG_SEED_SIZE, win_seed_buf);
        CryptReleaseContext(win_crypt_prov, 0);
        if (!win_seed_res)
            return 0;
        RAND_seed(win_seed_buf, WIN_PRNG_SEED_SIZE);
    }
    return 1;
}
#else
#define mlle_cr_seed() (1)
#endif

/* Length of buffers to use. */
#define READ_BUF_LEN (256)
#define CRYPT_BUF_LEN (READ_BUF_LEN + 32)

/*
 * Read data from stream in until eof, encrypt it, and IV, encrypted data and HMAC (in that order) to stream out.
 *
 * Returns zero on error.
 */
int mlle_cr_encrypt(FILE* in,
                    FILE* out)
{
    /* TODO: Enable better error messages. */
    EVP_CIPHER_CTX *c_ctx;
    HMAC_CTX *h_ctx;
    unsigned char *mac = NULL;
    unsigned char *iv = NULL;
    const EVP_CIPHER *cipher;
    int iv_len;
    const EVP_MD *hash;
    unsigned int mac_len;
    unsigned char read_buf[READ_BUF_LEN];
    unsigned char crypt_buf[CRYPT_BUF_LEN];
    int res = 0;
    int read_len;
    int crypt_len;
    DECLARE_MLLE_CR_KEY();

    /* Init structures. */
    c_ctx = EVP_CIPHER_CTX_new();
    h_ctx = HMAC_CTX_new();
    EVP_CIPHER_CTX_init(c_ctx);

    /* Select algorithms and get parameters. */
    cipher = MLLE_CR_CIPHER;
    iv_len = EVP_CIPHER_iv_length(cipher);
    hash = MLLE_CR_HASH;
    mac_len = (unsigned int) EVP_MD_size(hash);

    if (!mlle_cr_seed())
        goto error;

    /* Create IV and write it to file. */
    iv = (unsigned char*) malloc(iv_len);
    RAND_bytes(iv, iv_len);
    if (fwrite(iv, iv_len, 1, out) < 1)
        goto error;

    /* Set up encryption and HMAC calculation. */
    INITIALIZE_MLLE_CR_KEY();
    if (!EVP_EncryptInit_ex(c_ctx, cipher, NULL, MLLE_CR_KEY, iv))
        goto error;
    mac = (unsigned char*) malloc(mac_len);
    if (!mac)
        goto error;
    if (!HMAC_Init_ex(h_ctx, MLLE_CR_KEY, MLLE_CR_KEY_LEN, hash, NULL))
        goto error;
    if (!HMAC_Update(h_ctx, iv, iv_len))
        goto error;

    /* Process file. */
    while (!feof(in)) {
        /* Read cleartext data. */
        read_len = (int) fread(read_buf, 1, READ_BUF_LEN, in);
        if (read_len < READ_BUF_LEN && ferror(in))
            goto error;

        /* Encrypt and write data. */
        if (!EVP_EncryptUpdate(c_ctx, crypt_buf, &crypt_len, read_buf, read_len))
            goto error;
        if (fwrite(crypt_buf, 1, crypt_len, out) < (size_t) crypt_len)
            goto error;

        /* Update HMAC calculation. */
        if (!HMAC_Update(h_ctx, read_buf, read_len))
            goto error;
    }

    /* Finalize encryption and write final data. */
    if (!EVP_EncryptFinal_ex(c_ctx, crypt_buf, &crypt_len))
        goto error;
    if (fwrite(crypt_buf, 1, crypt_len, out) < (size_t) crypt_len)
        goto error;

    /* Finalize HMAC calculation and write to file. */
    if (!HMAC_Final(h_ctx, mac, &mac_len))
        goto error;
    if (fwrite(mac, mac_len, 1, out) < 1)
        goto error;

    /* Operation succeded. */
    res = 1;

    /* Cleanup. */
error:
    CLEAR_MLLE_CR_KEY();
    EVP_CIPHER_CTX_free(c_ctx);
    HMAC_CTX_free(h_ctx);
    free(mac);
    free(iv);

    return res;
}
