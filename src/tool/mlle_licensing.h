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

#ifndef MLLE_LICENSING_H_
#define MLLE_LICENSING_H_


#define _XOPEN_SOURCE 700
#include <stddef.h>
#include <stdio.h>
#include "mlle_error.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MLLE_ERROR_DOMAIN_TOOL (2)

struct mlle_connections;

//struct mlle_file_contents;

struct mlle_file_contents {
    size_t  file_size;
    size_t  read_offset;
    char    *buffer;
};

enum mlle_tool_error {
    MLLE_TOOL_ERROR_UNDEFINED,
    MLLE_TOOL_ERROR_PROTOCOL,
    MLLE_TOOL_ERROR_FAILED_TO_SPAWN,
    MLLE_TOOL_ERROR_NO_LICENSE,
    MLLE_TOOL_ERROR_NOT_SIMPLIFIED,
    MLLE_TOOL_ERROR_SIZE,
};

struct mlle_connections *
mlle_start_executable(const char *exec_name,
                      struct mlle_error **error);

void
mlle_connections_free(struct mlle_connections **connections);


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
                  struct mlle_error **error);


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
                  struct mlle_error **error);


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
                  struct mlle_error **error);



int
mlle_tool_returnfeature(const struct mlle_connections *connections,
                        const char *feature,
                        struct mlle_error **error);

int
mlle_tool_license(const struct mlle_connections *connections,
                  const char *package,
                  struct mlle_error **error);

int
mlle_tool_returnlicense(const struct mlle_connections *connections,
                        const char *package,
                        struct mlle_error **error);

struct mlle_file_contents *
mlle_tool_file(const struct mlle_connections *connections,
               const char *file_path,
               struct mlle_error **error);

void
mlle_file_contents_free(struct mlle_file_contents **file_contents);

size_t
mlle_tool_get_file_size(const struct mlle_file_contents *file_contents);

size_t
mlle_tool_read_bytes(struct mlle_file_contents *file_contents,
                     char *write_buffer,
                     size_t buffer_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_LICENSING_H_ */
