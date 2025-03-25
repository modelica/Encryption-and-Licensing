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

#ifndef PUBLIC_SRC_TOOL_MLLE_SSL_TOOL_H_
#define PUBLIC_SRC_TOOL_MLLE_SSL_TOOL_H_


#include "mlle_types.h"

// Forward declaration to avoid compiler warnings.
struct mlle_error;


/*****************************************************
 * Setup the SSL structure for the Tool (client).
 *
 * Parameters:
 *      lve - information structure.
 *
 * Returns:
 *      1 - the setup was successful.
 *      0 - setup failed.
 ****************************************************/
int ssl_setup_tool(struct mlle_connections **lve, struct mlle_error **error);


/**********************************************************
 * The Tool (client) starts a handshake and wait for the
 * LVE (server) to accept.
 *
 * Parameters:
 *      lve - information structure.
 *
 * Returns:
 *      1 - handshake was successful.
 *      0 - handshake failed.
 **********************************************************/
int tool_perform_handshake(struct mlle_connections **lve, struct mlle_error **error);


#endif /* PUBLIC_SRC_TOOL_MLLE_SSL_TOOL_H_ */
