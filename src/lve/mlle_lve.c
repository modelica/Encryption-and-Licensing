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

// Disable "deprecated" warning.
#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _XOPEN_SOURCE 700
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mlle_error.h"
#include "mlle_io.h"
#include "mlle_parse_command.h"
#include "mlle_lve_tools.h"
#include "mlle_lve_libpath.h"
#include "mlle_lve_pubkey.h"
#include "mlle_lve_license.h"
#include "mlle_lve_feature.h"
#include "mlle_lve_file.h"
#include "mlle_license_manager.h"
#include "mlle_lve.h"

#include "mlle_types.h"
#include "openssl/err.h"
#include "mlle_portability.h"
#include <openssl/rsa.h>
#include <openssl/x509v3.h>
#include <openssl/evp.h>

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif


/***********************************************************
 * Parameters:
 *      lve_ctx - container about the connection.
 *      current_state - the state.
 *      command - container for the command..
 *      error_msg - container for error messages.
 *      error_length - length of the error message.
 *      buffer - the buffer containing the full message.
 ***********************************************************/
enum mlle_lve_state
mlle_lve_handle_command(struct mlle_lve_ctx *lve_ctx,
                        enum mlle_lve_state current_state,
                        struct mlle_command *command,
                        char *error_msg,
                        size_t error_length,
                        char *buffer,
                        size_t buffer_len)
{
    enum mlle_lve_state next_state = MLLE_LVE_STATE_INVALID;

    // Get next state.
    next_state = mlle_next_state(current_state, command->id, error_msg, error_length);


    if (next_state == MLLE_LVE_STATE_INVALID) {
        mlle_send_error(lve_ctx->ssl, MLLE_PROTOCOL_COMMAND_NOT_UNDERSTOOD_ERROR,
                error_msg);
        return current_state;
    }

    // Is this a command with a data part.
    if (mlle_command_info[command->id].msg_form == MLLE_PROTOCOL_LENGTH_MSG_FORM)
    {
        // Extract data from message.
        extract_data(&command, buffer, buffer_len);
    }

    switch (current_state) {
    long tool_protocol_max_version = 0;

    case MLLE_LVE_STATE_INVALID:
        mlle_send_error(lve_ctx->ssl, MLLE_PROTOCOL_UNDEFINED_ERROR,
                "Internal error in protocol state management.");
        break;
    case MLLE_LVE_STATE_VERSION:
        tool_protocol_max_version = command->number;
        if (tool_protocol_max_version < MIN_PROTOCOL_VERSION)
        {
            snprintf(error_msg, error_length,
                     "Minimum supported protocol version is %d", MIN_PROTOCOL_VERSION);
            mlle_send_error(lve_ctx->ssl, MLLE_PROTOCOL_VERSION_TOO_LOW_ERROR,
                    error_msg);
            next_state = MLLE_LVE_STATE_VERSION;
        }
        else
        {
            mlle_send_number_form(lve_ctx->ssl, MLLE_PROTOCOL_VERSION_CMD,
                    MAX_PROTOCOL_VERSION);
        }
        break;
    case MLLE_LVE_STATE_TOOLS:
        if (command->id == MLLE_PROTOCOL_TOOLS_CMD)
        {
            // TODO Fix this. Tools doesn't return anything yet.
            mlle_lve_tools(lve_ctx);
        }
        else
        {
            mlle_lve_libpath(lve_ctx, command);
        }
        break;
    case MLLE_LVE_STATE_LIB:
        mlle_lve_libpath(lve_ctx, command);
        break;

    case MLLE_LVE_STATE_LICENSE:
        if (command->id == MLLE_PROTOCOL_FEATURE_CMD) {
            mlle_lve_feature(lve_ctx, command);
        } else if (command->id == MLLE_PROTOCOL_RETURNFEATURE_CMD) {
            mlle_lve_returnfeature(lve_ctx, command);
        } else if (command->id == MLLE_PROTOCOL_LICENSE_CMD) {
            mlle_lve_license(lve_ctx, command);
        } else if (command->id == MLLE_PROTOCOL_RETURNLICENSE_CMD) {
            mlle_lve_returnlicense(lve_ctx, command);
        } else {
            mlle_lve_file(lve_ctx, command);   // command->id == MLLE_PROTOCOL_FILECONT_CMD
        }
        break;
    case MLLE_LVE_STATE_SIZE:
        mlle_send_error(lve_ctx->ssl, MLLE_PROTOCOL_UNDEFINED_ERROR,
                "Internal error in protocol state management.");
        break;
    }

    if (command->data != NULL) {
        free(command->data);
        command->data = NULL;
    }

    return next_state;
}


/************************************************************
 * Receive data from the Tool (client) using SSL.
 *
 * Parameters:
 *      lve_ctx - I/O and information about the connection.
 * 
 * Returns:
 *		Always 1 even when errors occurs.
 ***********************************************************/
int mlle_lve_receive(struct mlle_lve_ctx *lve_ctx)
{
    enum mlle_lve_state state = MLLE_LVE_STATE_VERSION;
    int bytesRead = 0;
    struct mlle_command command = { 0 };
    enum mlle_grammar_error_t grammar_error = LE_UNKNOWN_ERROR;
    char error_msg[ERROR_SIZE] = { '\0' };
    char *messageBuffer = NULL;
    char *tokenBuffer = NULL;
	FILE *fd = NULL;

    // Validate tools public key to see if it's trusted or not.
    mlle_lve_validate_pubkey(lve_ctx);

    // From the read we get a buffer and size of the buffer.
    bytesRead = ssl_read_message(lve_ctx->ssl, &messageBuffer);

    while (bytesRead != LE_EOF)
    {
        if (bytesRead == LE_LINE_TOO_LONG)
        {
            snprintf(error_msg, ERROR_SIZE,
                    "Input error. Message row too long, more than %d characters.",
                    MLLE_PROTOCOL_MAX_COMMAND_LINE_SIZE);
            mlle_send_error(lve_ctx->ssl, MLLE_PROTOCOL_COMMAND_NOT_UNDERSTOOD_ERROR,
                    error_msg);
        }
        else
        {
            // Create a temporary buffer for tokens.
            tokenBuffer = malloc(bytesRead + 1);
            memcpy(tokenBuffer, messageBuffer, bytesRead);
            tokenBuffer[bytesRead] = 0;

            // Extract command from message.
            grammar_error = mlle_parse_command(tokenBuffer, &command, error_msg, ERROR_SIZE);

            free(tokenBuffer);
            tokenBuffer = NULL;

            if (grammar_error == LE_VALID_GRAMMAR)
            {
                // Handle the incoming command.
                state = mlle_lve_handle_command(lve_ctx, state, &command, error_msg, ERROR_SIZE, messageBuffer, (size_t) bytesRead);
            }
            else if (grammar_error == LE_UNKNOWN_ERROR)
            {
                mlle_send_error(lve_ctx->ssl, MLLE_PROTOCOL_COMMAND_NOT_UNDERSTOOD_ERROR,
                        "Internal error in parsing.");
            } else
            {
                mlle_send_error(lve_ctx->ssl, MLLE_PROTOCOL_COMMAND_NOT_UNDERSTOOD_ERROR,
                        error_msg);
            }
        }

        free(messageBuffer);
        messageBuffer = NULL;

        // Keep reading until shutdown signal is received or
		// ssl_read_message returns LE_EOF (-1).
        if (SSL_get_shutdown(lve_ctx->ssl) == 0)
        {
            // Wait for next message.
            bytesRead = ssl_read_message(lve_ctx->ssl, &messageBuffer);
        }
        else
        {
            // Stop receive loop.
            bytesRead = LE_EOF;
        }
    }

    return 1;
}


/*************************************************
 * Shutdown LVE. Close connection to SSL.
 *
 * Parameters:
 *      lve_ctx - container for LVE information.
 *************************************************/
void mlle_lve_shutdown(struct mlle_lve_ctx *lve_ctx)
{
    if (lve_ctx->lic_mgr != NULL) {
        mlle_license_free(lve_ctx->lic_mgr);
    }

    /*
     * TODO: this will try to free constant strings, but without it we might leak memory
    free(lve_ctx->tool_error_msg);
    */
    free(lve_ctx->libpath);

    SSL_shutdown (lve_ctx->ssl);
    SSL_free (lve_ctx->ssl);

    fflush(NULL);
}


//#pragma GCC diagnostic ignored "-Wunused-parameter"

void mlle_lve_init(struct mlle_lve_ctx *lve_ctx)
{

#ifdef _WIN32
    _setmode(_fileno(lve_ctx->in_stream), _O_BINARY);
    _setmode(_fileno(lve_ctx->out_stream), _O_BINARY);
#endif
}


