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
/* libcrypto-compat.h must be first */
#include "libcrypto-compat.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include "mlle_cr_encrypt.h"

int main(int argc, char** argv)
{
    FILE* in = NULL;
    FILE* out = NULL;
    int res = 1;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <cleartext file> <encrypted file>\n", argv[0]);
        return 1;
    }

    /* OpenSSL initialization stuff. */
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);

    /* Open input and output files. */
    in = fopen(argv[1], "rb");
    if (in == NULL)
        fprintf(stderr, "Could not open file %s for reading. Error message was: %s\n", argv[1], strerror(errno));
    out = fopen(argv[2], "wb");
    if (out == NULL)
        fprintf(stderr, "Could not open file %s for writing. Error message was: %s\n", argv[2], strerror(errno));

    /* Encrypt file. */
    if (in && out) {
        if (mlle_cr_encrypt(in, out))
            res = 0;
        else
            fprintf(stderr, "Encryption failed for file %s.\n", argv[2]);
    }

    /* Close files. */
    if (in != NULL)
        fclose(in);
    if (out != NULL)
        fclose(out);
    if (out != NULL && res != 0)
        remove(argv[2]);

    /* OpenSSL cleanup stuff. */
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();

    return res;
}
