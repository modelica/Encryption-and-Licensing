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

#include "mlle_ssl_tool.h"

#include <stdio.h>
#include <stdlib.h>
#include "mlle_lve.h"
#include "mlle_ssl.h"
#include "mlle_error.h"
#include <openssl/ossl_typ.h>
#include <openssl/asn1t.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509v3.h>

#include "mlle_portability.h"
#include "private_key_tool.h"

#ifdef INCLUDE_OPENSSL_APPLINK
#ifndef __INCLUDE_OPENSSL_APPLINK
#define __INCLUDE_OPENSSL_APPLINK
#include <openssl/applink.c>
#endif /* __INCLUDE_OPENSSL_APPLINK */
#endif /* INCLUDE_OPENSSL_APPLINK */


/********************************************************
 * Setup the SSL structure for the Tool (client).
 *
 * Parameters:
 *      lve - I/O and information about the connection.
 *      error - structure for error messages.
 *
 * Returns:
 *      1 - the setup was successful.
 *      0 - setup failed.
 *******************************************************/
int ssl_setup_tool(struct mlle_connections **lve, struct mlle_error **error)
{
    SSL *ssl = NULL;
    SSL_CTX *ctx = NULL;
    BIO *bioWrite = NULL;
    BIO *bioRead = NULL;
    RSA *rsa = NULL;
    X509 *x509;
    int result = 0;
    DECLARE_PRIVATE_KEY_TOOL();

    // Initiate ssl.
    init_ssl();

    // Initiate CTX structure as a client.
    INITIALIZE_PRIVATE_KEY_TOOL();
    if ( (ctx = create_CTX((char *) PRIVATE_KEY_TOOL, CLIENT)) == NULL)
    {
        mlle_error_set(error, 1, 1, "SSL: Failed to create client CTX structure.");
        goto cleanup;
    }

    // Create RSA with the private key.
    if ( (rsa = get_rsa((char *) PRIVATE_KEY_TOOL)) == NULL)
    {
        mlle_error_set(error, 1, 2, "SSL: Failed to create client RSA structure.");
        goto cleanup;
    }
    CLEAR_PRIVATE_KEY_TOOL();

    // Create self-signed certificate to get
    // the public key to the server.
    if ( (x509 = generate_X509(rsa)) == NULL)
    {
        mlle_error_set(error, 1, 3, "SSL: Failed to create client X509 certificate.");
        goto cleanup;
    }

    // Setup the SSL structure.
    ssl = SSL_new(ctx);

    // Setup input/output using Pipe descriptors.
    bioWrite = BIO_new_fd((*lve)->fd_to_child, BIO_NOCLOSE);
    bioRead = BIO_new_fd((*lve)->fd_from_child, BIO_NOCLOSE);

    if ( (bioWrite == NULL) || (bioRead == NULL) )
    {
        mlle_error_set(error, 1, 4, "SSL: Failed to create BIO input/output descriptors.");
        goto cleanup;
    }

    // Add certificate to SSL.
    if (SSL_use_certificate(ssl, x509) == 0)
    {
        mlle_error_set(error, 1, 5, "SSL: Failed to add X509 certificate.");
        goto cleanup;
    }

    // Set to non-blocking(1). 0 = blocking.
    BIO_set_nbio(bioWrite, 1);
    BIO_set_nbio(bioRead, 1);

    // Connect SSL with read and write BIO.
    SSL_set_bio(ssl, bioRead, bioWrite );

    // Run new handshakes in the background.
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    (*lve)->ssl = ssl;
    result = 1;

cleanup:
    CLEAR_PRIVATE_KEY_TOOL();

    return result;
}


/**********************************************************
 * Tool (client) performs a TLS handshake and trying to
 * connect with the LVE (server) using SSL.
 *
 * Parameters:
 *      lve - I/O and information about the connection.
 *      error - structure for error messages.
 *
 * Returns:
 *      1 - handshake was successful.
 *      0 - handshake failed.
 **********************************************************/
int tool_perform_handshake(struct mlle_connections **lve, struct mlle_error **error)
{
    int result = 0;
    int errorCode = 0;
    char errorString[SSL_ERROR_BUF_LEN];

    // Validate parameters.
    if ( (lve == NULL) || (*lve == NULL) || ((*lve)->ssl == NULL) )
    {
        return 0;
    }

    // Try to connect with LVE (server).
    result = SSL_connect((*lve)->ssl);
    if (result <= 0)
    {
        // Get reason for handshake error.
        errorCode = SSL_get_error((*lve)->ssl, result);
        ssl_get_error_string(errorCode, errorString, SSL_ERROR_BUF_LEN);
        mlle_error_set(error, 1, 2, "SSL: Handshake failed. Reason: %s", errorString);
        return 0;
    }

    return 1;
}
