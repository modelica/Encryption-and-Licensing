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

#ifndef MLLE_PARSE_COMMAND_H_
#define MLLE_PARSE_COMMAND_H_

#define _XOPEN_SOURCE 700
#include <stddef.h>
#include "mlle_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum mlle_grammar_error_t {
    LE_VALID_GRAMMAR,
    LE_UNKNOWN_ERROR,
    LE_NO_TOKENS,
    LE_TOO_MANY_TOKENS,
    LE_TOO_FEW_TOKENS,
    LE_UNKNOWN_CMD,
    LE_NOT_AN_INT,
    LE_NEGATIVE_LENGTH,
    LE_GRAMMAR_ERROR_T_SIZE
};

size_t
mlle_tokenize(char *string,
              char *tokens[MLLE_PROTOCOL_MAX_TOKENS_PER_MSG]);

enum mlle_grammar_error_t
mlle_parse_command(char *string,
                   struct mlle_command *command,
                   char *error_msg,
                   size_t error_length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_PARSE_COMMAND_H_ */
