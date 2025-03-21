/*
    Copyright (C) 2022 Modelica Association
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


/*
 * This file is for testing the functions in the file mlle_ssl.c.
 * Test frameworks is Check (http://check.sourceforge.net/).
 */

#include "check.h"

#include "mlle_ssl.h"
#include "mlle_types.h"
#include "private_key_tool.h"

/*************************************************************
 * Test if we can read a private/public key file (which is
 * compiled into the executable) and create a RSA structure
 * from it.
 *************************************************************/
START_TEST (test_get_rsa)
{
    RSA *rsa = NULL;
    int result = 0;
    size_t key_len;

    key_len = strlen(private_key_tool) + 1;
    char *private_key = malloc(key_len);
    snprintf(private_key, key_len, "%s", private_key_tool);

    // Create RSA structure.
    rsa = get_rsa(private_key);
    ck_assert_ptr_ne(rsa, NULL);

    free(private_key);
    RSA_free(rsa);
}
END_TEST


/****************************************************************
 * Test if we can extract the public key from an RSA structure.
 ***************************************************************/
START_TEST (test_get_public_key)
{
    RSA *rsa = NULL;
    char *public_key = NULL;
    size_t key_len;

    key_len = strlen(private_key_tool) + 1;
    char *private_key = malloc(key_len);
    snprintf(private_key, key_len, "%s", private_key_tool);

    // Create RSA structure.
    rsa = get_rsa(private_key);

    // Get the public key.
    public_key = get_public_key(rsa);
    ck_assert_ptr_ne(public_key, NULL);

    free(public_key);
    free(private_key);
    RSA_free(rsa);
}
END_TEST


/***********************************************
 * Test if we can generate a X509 certificate.
 **********************************************/
START_TEST (test_generate_X509)
{
    RSA *rsa = NULL;
    RSA *rsaNew = NULL;
    X509 *certificate = NULL;
    size_t key_len;

    key_len = strlen(private_key_tool) + 1;
    char *private_key = malloc(key_len);
    snprintf(private_key, key_len, "%s", private_key_tool);

    // Create RSA structure.
    rsa = get_rsa(private_key);
    ck_assert_ptr_ne(rsa, NULL);

    // Create certificate.
    certificate = generate_X509(rsa);

    ck_assert_ptr_ne(certificate, NULL);

    free(private_key);
    X509_free(certificate);
    RSA_free(rsa);
}
END_TEST

/********************************
 * Test to create a client CTX.
 *******************************/
START_TEST (test_create_ctx)
{
    SSL_CTX *client_ctx = NULL;

    // Create client CTX.
    client_ctx = create_CTX((char *)private_key_tool, 1);

    ck_assert_ptr_ne(client_ctx, NULL);

    SSL_CTX_free(client_ctx);
}
END_TEST


/**********************************
 * Test to get an error string.
 *********************************/
START_TEST (test_get_error_string)
{
    char errorString[100];
    char *ptr = NULL;

    ssl_get_error_string(SSL_ERROR_SYSCALL, errorString);

    ptr = strstr(errorString, "SSL_ERROR_SYSCALL.");

    ck_assert_ptr_ne(ptr, NULL);
}
END_TEST


Suite* suite_ssl (void)
{
    Suite *suite = suite_create("test_ssl");
    TCase *tcase = tcase_create("ssl");

    init_ssl();
    tcase_add_test(tcase, test_get_rsa);
    tcase_add_test(tcase, test_get_public_key);
    tcase_add_test(tcase, test_generate_X509);
    tcase_add_test(tcase, test_create_ctx);
    tcase_add_test(tcase, test_get_error_string);

    suite_add_tcase(suite, tcase);
    return suite;
}
