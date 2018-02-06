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

#ifndef PUBLIC_SRC_LVE_MLLE_SSL_LVE_H_
#define PUBLIC_SRC_LVE_MLLE_SSL_LVE_H_


#include "mlle_types.h"
#include "mlle_lve.h"

// Forward declaration to avoid compiler warnings.
struct mlle_error;



/*****************************************************
 * Setup the SSL structure for the LVE (server).
 *
 * Parameters:
 *      lve_ctx - information about the connection.
 *
 * Returns:
 *      1 - the setup was successful.
 *      0 - setup failed.
 ****************************************************/
int ssl_setup_lve(struct mlle_lve_ctx *lve_ctx);


/**********************************************************
 * The LVE (server) starts a handshake and wait for the
 * Tool (client) to connect.
 *
 * Parameters:
 *      lve_ctx - information structure.
 *
 * Returns:
 *      1 - handshake was successful.
 *      0 - handshake failed.
 **********************************************************/
int lve_perform_handshake(struct mlle_lve_ctx *lve_ctx);


#endif /* PUBLIC_SRC_LVE_MLLE_SSL_LVE_H_ */
