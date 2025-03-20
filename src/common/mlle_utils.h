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

#ifndef MLLE_UTILS_H_
#define MLLE_UTILS_H_

#define _XOPEN_SOURCE 700
#include <stddef.h>
#include <stdio.h>
#include "mlle_protocol.h"
#include "mlle_error.h"
#include "mlle_ssl.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/************************************************************
 * Extract the data part from a message.
 *
 * Parameters:
 *      command - the structure where we will place the
 *                extracted data.
 *      buffer - the message containing command and data.
 ***********************************************************/
size_t extract_data(struct mlle_command **command, char *buffer, size_t buffer_len);

int mlle_read_command(SSL *ssl,
                  struct mlle_command *command,
                  struct mlle_error **error);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_UTILS_H_ */
