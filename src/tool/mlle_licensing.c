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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mlle_types.h"
#include "mlle_protocol.h"
#include "mlle_error.h"
#include "mlle_spawn.h"
#include "mlle_portability.h"
#include "mlle_ssl.h"
#include "mlle_ssl_tool.h"
#include "mlle_io.h"
#include "mlle_parse_command.h"
#include "mlle_utils.h"
#include "mlle_licensing.h"

#ifdef INCLUDE_OPENSSL_APPLINK
#ifndef __INCLUDE_OPENSSL_APPLINK
#define __INCLUDE_OPENSSL_APPLINK
#include <openssl/applink.c>
#endif /* __INCLUDE_OPENSSL_APPLINK */
#endif /* INCLUDE_OPENSSL_APPLINK */


void
mlle_connections_free(struct mlle_connections **connections)
{
    if (connections != NULL && *connections != NULL)
    {
        if ((*connections)->ssl != NULL) {
            // Close SSL connection.
            SSL_shutdown ((*connections)->ssl);
            SSL_free ((*connections)->ssl);
        }
        close((*connections)->fd_to_child);
        close((*connections)->fd_from_child);
        free(*connections);
        *connections = NULL;
    }
}

struct mlle_connections *
mlle_start_executable(const char *exec_name,
                      struct mlle_error **error)
{
    struct mlle_connections *lve = NULL;
    struct mlle_connections *result = NULL;

    // Startup LVE.
    lve = mlle_spawn(exec_name, error);
    if (lve == NULL) {
        goto cleanup;
    }

    // Setup SSL
    // Setup SSL
    if (!ssl_setup_tool(&lve, error))
    {
        goto cleanup;
    }

    // Perform handshake with LVE
    if (!tool_perform_handshake(&lve, error))
    {
        goto cleanup;
    }

    result = lve;

cleanup:
    if (result == NULL) {
        mlle_connections_free(&lve);
    }
    return result;
}


static int
mlle_expect_command(SSL *ssl,
                    enum mlle_protocol_command_id expected_command_id,
                    struct mlle_command *command,
                    struct mlle_error **error)
{
    int success = 0;

    success = mlle_read_command(ssl, command, error);

    if (!success) {
        return 0;
    }
    assert(command->id != MLLE_PROTOCOL_UNDEFINED_CMD);

    if (command->id == MLLE_PROTOCOL_ERROR_CMD) {
        mlle_error_set(error, MLLE_ERROR_DOMAIN_TOOL, 1,
                "Got ERROR command with message: %s", command->data);

        free(command->data);
        command->data = NULL;
        return 0;
    }
    if (command->id != expected_command_id) {
        mlle_error_set(error, MLLE_ERROR_DOMAIN_TOOL, 1,
                "Unexpected command. Expected command %s, but got command %s.",
                mlle_command_info[expected_command_id].name,
                mlle_command_info[command->id].name);
        return 0;
    }

    return 1;
}

static int
mlle_expect_commands(SSL *ssl,
                    size_t ncommands,
                    enum mlle_protocol_command_id *expected_command_ids,
                    struct mlle_command *command,
                    struct mlle_error **error)
{
    int success = 0;
    size_t i = 0;
    int expected = 0;

    assert(ncommands > 0);

    success = mlle_read_command(ssl, command, error);
    if (!success) {
        return 0;
    }
    assert(command->id != MLLE_PROTOCOL_UNDEFINED_CMD);

    if (command->id == MLLE_PROTOCOL_ERROR_CMD) {
        mlle_error_set(error, MLLE_ERROR_DOMAIN_TOOL, 1,
                "Got ERROR command with message: %s", command->data);
        free(command->data);
        command->data = NULL;
        return 0;
    }

    for (i = 0; i < ncommands; i++) {
        if (command->id == expected_command_ids[i]) {
            expected = 1;
            break;
        }
    }
    if (!expected) {
        // 100 is enough for the constant string below
        size_t alloc_size = 100 + (MLLE_PROTOCOL_MAX_CMD_LENGTH + 2) * (ncommands + 1);
        char *err_msg = malloc(alloc_size);
        int written_bytes = 0;

        if (err_msg != NULL) {
            written_bytes = snprintf(err_msg, alloc_size,
                    "Unexpected command %s. Expected one of the following commands: %s",
                    mlle_command_info[command->id].name,
                    mlle_command_info[expected_command_ids[0]].name);
            for (i = 1; i < ncommands; i++) {
                written_bytes += snprintf(&err_msg[written_bytes], alloc_size,", %s",
                        mlle_command_info[expected_command_ids[i]].name);
            }
        }
        mlle_error_set_literal(error, MLLE_ERROR_DOMAIN_TOOL, 1, err_msg);
        free(err_msg);
        return 0;
    }

    return 1;
}


/**********************************************************
 * Send command VERSION from Tool to LVE and expecting the
 * LVE's version in return.
 *
 * Parameters:
 *      connections - communication information.
 *      min_protocol_version - minimum version number.
 *      max_protocol_version - maximum version number.
 *      error - structure for reporting errors.
 *
 * Returns:
 *      1 - Operation was successful.
 *      0 - Operation failed.
 *********************************************************/
int mlle_tool_version(const struct mlle_connections *connections,
                  int min_protocol_version,
                  int max_protocol_version,
                  struct mlle_error **error)
{
    struct mlle_command command = { 0 };
    long protocol_version = -1;

    if (mlle_send_number_form(connections->ssl, MLLE_PROTOCOL_VERSION_CMD, 
        max_protocol_version) < 0) {
        return 0;
    }
    if (!mlle_expect_command(connections->ssl, MLLE_PROTOCOL_VERSION_CMD, &command, error)) {
                return 0;
    }

    protocol_version = command.number;
    if (protocol_version < min_protocol_version)
    {
        mlle_error_set(error, MLLE_ERROR_DOMAIN_TOOL, 1,
                "Protocol version from LVE too low.");
                return 0;
    }
    else if (protocol_version > max_protocol_version)
    {
        mlle_error_set(error, MLLE_ERROR_DOMAIN_TOOL, 1,
                "Protocol version from LVE too high.");
                return 0;
    }
    return 1;
}


/**********************************************************
 * Send command LIB from Tool to LVE and expecting the
 * LVE's version in return.
 *
 * Parameters:
 *      connections - communication information.
 *      min_protocol_version - minimum version number.
 *      max_protocol_version - maximum version number.
 *      error - structure for reporting errors.
 *
 * Returns:
 *      1 - Operation was successful.
 *      0 - Operation failed.
 *********************************************************/
int mlle_tool_libpath(const struct mlle_connections *connections,
                  const char *absolute_path,
                  struct mlle_error **error)
{
    struct mlle_command command = { 0 };

    assert(absolute_path != NULL);

    mlle_send_string(connections->ssl, MLLE_PROTOCOL_LIB_CMD, absolute_path);
    if (!mlle_expect_command(connections->ssl, MLLE_PROTOCOL_YES_CMD, &command, error)) {
        return 0;
    }

    return 1;
}


/**********************************************************
 * Send command FEATURE from Tool to LVE.
 * Expect YES or NO in return depending on if the LVE has
 * the feature or not.
 *
 * Parameters:
 *      connections - communication information.
 *      feature - the feature to look up.
 *      error - structure for reporting errors.
 *
 * Returns:
 *      1 - Operation was successful.
 *      0 - Operation failed.
 *********************************************************/
int mlle_tool_feature(const struct mlle_connections *connections,
                  const char *feature,
                  struct mlle_error **error)
{
    struct mlle_command command = { 0 };
    enum mlle_protocol_command_id expected_commands[2] = {
            MLLE_PROTOCOL_YES_CMD,
            MLLE_PROTOCOL_NO_CMD
    };

    assert(feature != NULL);

    mlle_send_string(connections->ssl, MLLE_PROTOCOL_FEATURE_CMD, feature);
    if (!mlle_expect_commands(connections->ssl, 2, expected_commands,
            &command, error))
    {
        return 0;
    }

    if (command.id == MLLE_PROTOCOL_YES_CMD) {
        return 1;
    } else if (command.id == MLLE_PROTOCOL_NO_CMD) {
        mlle_error_set(error, MLLE_ERROR_DOMAIN_TOOL, MLLE_TOOL_ERROR_NO_LICENSE,
                "%s", command.data);
        return 0;
    } else {
        /* Internal error. */
        assert(0);
        return 0;
    }

    return 1;
}

int
mlle_tool_returnfeature(const struct mlle_connections *connections,
                        const char *feature,
                        struct mlle_error **error)
{
    struct mlle_command command = { 0 };

    assert(feature != NULL);

    mlle_send_string(connections->ssl, MLLE_PROTOCOL_RETURNFEATURE_CMD, feature);
    if (!mlle_expect_command(connections->ssl, MLLE_PROTOCOL_YES_CMD,
            &command, error))
    {
        return 0;
    }

    return 1;
}

int
mlle_tool_license(const struct mlle_connections *connections,
                  const char *package,
                  struct mlle_error **error)
{
    struct mlle_command command = { 0 };
    enum mlle_protocol_command_id expected_commands[3] = {
            MLLE_PROTOCOL_YES_CMD,
            MLLE_PROTOCOL_NOTSIMPLE_CMD,
            MLLE_PROTOCOL_NO_CMD
    };

    assert(package != NULL);

    mlle_send_string(connections->ssl, MLLE_PROTOCOL_LICENSE_CMD, package);
    if (!mlle_expect_commands(connections->ssl, 3, expected_commands,
            &command, error))
    {
        return 0;
    }

    if (command.id == MLLE_PROTOCOL_YES_CMD) {
        return 1;
    } else if (command.id == MLLE_PROTOCOL_NOTSIMPLE_CMD) {
        mlle_error_set_literal(error, MLLE_ERROR_DOMAIN_TOOL, MLLE_TOOL_ERROR_NOT_SIMPLIFIED,
                "Library vendor executable doesn't use simplified licensing, use command FEATURE instead.");
        return 0;
    } else if (command.id == MLLE_PROTOCOL_NO_CMD) {
        mlle_error_set(error, MLLE_ERROR_DOMAIN_TOOL, MLLE_TOOL_ERROR_NO_LICENSE,
                "%s", command.data);
        return 0;
    } else {
        /* Internal error. */
        assert(0);
        return 0;
    }
}

int
mlle_tool_returnlicense(const struct mlle_connections *connections,
                        const char *package,
                        struct mlle_error **error)
{
    struct mlle_command command = { 0 };

    assert(package != NULL);

    mlle_send_string(connections->ssl, MLLE_PROTOCOL_RETURNLICENSE_CMD, package);
    if (!mlle_expect_command(connections->ssl, MLLE_PROTOCOL_YES_CMD,
            &command, error))
    {
        return 0;
    }

    return 1;
}

struct mlle_file_contents *
mlle_tool_file(const struct mlle_connections *connections,
               const char *file_path,
               struct mlle_error **error)
{
    struct mlle_command command = { 0 };
    struct mlle_file_contents *file_contents = NULL;

    assert(file_path != NULL);

    mlle_send_string(connections->ssl, MLLE_PROTOCOL_FILE_CMD, file_path);
    if (!mlle_expect_command(connections->ssl, MLLE_PROTOCOL_FILECONT_CMD,
            &command, error))
    {
        return NULL;
    }

    file_contents = calloc(1, sizeof(*file_contents));
    if (file_contents == NULL) {
        mlle_error_set_literal(error, 1, 1, "Out of memory.");
        return NULL;
    }
    file_contents->file_size = command.length;
    file_contents->read_offset = 0;
    file_contents->buffer = command.data;

    return file_contents;
}


void
mlle_file_contents_free(struct mlle_file_contents **file_contents)
{
    if (file_contents != NULL && *file_contents != NULL) {
        if ((*file_contents)->buffer != NULL) {
            free((*file_contents)->buffer);
            (*file_contents)->buffer = NULL;
        }
        free(*file_contents);
        *file_contents = NULL;
    }
}

size_t
mlle_tool_get_file_size(const struct mlle_file_contents *file_contents)
{
    return file_contents->file_size;
}

size_t
mlle_tool_read_bytes(struct mlle_file_contents *file_contents,
                     char *write_buffer,
                     size_t buffer_size)
{
    if (file_contents->buffer == NULL) {
        file_contents->read_offset = file_contents->file_size;
        return -1;
    } else if (file_contents->read_offset >= file_contents->file_size) {
        if (file_contents->buffer != NULL) {
            free(file_contents->buffer);
            file_contents->buffer = NULL;
        }
        return -1;
    } else {
        size_t left_to_write = file_contents->file_size - file_contents->read_offset;
        size_t length = buffer_size < left_to_write ? buffer_size : left_to_write;

        memcpy(write_buffer, file_contents->buffer + file_contents->read_offset, length);
        file_contents->read_offset += length;

        if (file_contents->read_offset >= file_contents->file_size) {
            free(file_contents->buffer);
            file_contents->buffer = NULL;
        }

        return length;
    }
}
