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

// Disable "deprecated" warning.
#ifdef WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#define _XOPEN_SOURCE 700
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
/* libcrypto-compat.h must be first */
#include "libcrypto-compat.h"
#include "mlle_protocol.h"
#include "mlle_parse_command.h"

#include <stdlib.h>

static const char *expected_form_template[MLLE_PROTOCOL_MSG_FORM_SIZE] = {
    "[This should never be used.]",
    "%s",
    "%s <number>",
    "%s <length>",
    "%s <number> <length>",
};




/*******************************************************************
 * Divide a string into tokens. We are only interested in the
 * tokens up to the first newline. The rest is the data part which
 * is handled later.
 *
 * Parameters:
 *      string - the string to divide.
 *      tokens - the array where command/parameter are stored.
 *
 * Returns:
 *      Number of tokens found or MLLE_PROTOCOL_TOO_MANY_TOKENS
 *      if string contained too many tokens.
 ******************************************************************/
size_t mlle_tokenize(char *string,
              char *tokens[MLLE_PROTOCOL_MAX_TOKENS_PER_MSG])
{
    size_t ntokens = 0;
    char *next_char = string;
    char *current_token = NULL;
    char *current_string = NULL;

    int token_count = 0;

    if ( (string == NULL) || (strlen(string) == 0) )
    {
        return 0;
    }

    // Get a substring up to the newline character.
    // This is the command part we want to tokenize.
    current_string = strtok_r(string, "\n", &next_char);

    // Get the tokens.
    current_token = strtok_r(current_string, " ", &next_char);
    while (current_token != NULL)
    {
        if (ntokens < MLLE_PROTOCOL_MAX_TOKENS_PER_MSG) {
            tokens[ntokens] = current_token;

            ntokens++;
        }
        else
        {
            ntokens = MLLE_PROTOCOL_TOO_MANY_TOKENS;
            break;
        }

        current_token = strtok_r(NULL, " ", &next_char);
    }

    return ntokens;
}



static int
parse_int(long *n,
          const char *int_token,
          const char *cmd_token,
          char *error_msg,
          size_t error_length)
{
    int ints_scanned = 0;

    *n = 0;
    ints_scanned = sscanf(int_token, "%ld", n);
    if (ints_scanned < 0) {
        // XXX: Read error. What else? Check this!
    } else if (ints_scanned == 0) {
        snprintf(error_msg, error_length,
                 "Parse error. Argument %s to command %s is not an integer.",
                 int_token, cmd_token);
    }

    return ints_scanned <= 0;
}


/************************************************
 * Parse a command.
 *
 * Returns:
 *      0 - if command is valid.
 *      1 or higher if something went wrong.
 ***********************************************/
enum mlle_grammar_error_t
mlle_parse_command(char *string,
                   struct mlle_command *command,
                   char *error_msg,
                   size_t error_length)
{
    size_t ntokens = 0;
    char *tokens[MLLE_PROTOCOL_MAX_TOKENS_PER_MSG] = { NULL, NULL, NULL };
    enum mlle_protocol_command_id id = MLLE_PROTOCOL_UNDEFINED_CMD;
    enum mlle_protocol_msg_form msg_form = MLLE_PROTOCOL_UNDEFINED_MSG_FORM;
    int i = 0;
    size_t expected_nbr_tokens = 0;
    enum mlle_grammar_error_t grammar_error = LE_UNKNOWN_ERROR;

    command->id = MLLE_PROTOCOL_UNDEFINED_CMD;
    command->number = 0;
    command->length = 0;
    command->data = NULL;

    ntokens = mlle_tokenize(string, tokens);

    if (ntokens == 0) {
        snprintf(error_msg, error_length, "No tokens in message.");
        return LE_NO_TOKENS;
    }

    /* Try to match first token with command string. */
    for (i = MLLE_PROTOCOL_UNDEFINED_CMD + 1; i < MLLE_PROTOCOL_COMMAND_ID_SIZE; i++) {
        if (strcmp(tokens[0], mlle_command_info[i].name) == 0) {
            id = mlle_command_info[i].id;
            msg_form = mlle_command_info[i].msg_form;
            break;
        }
    }
    assert(id < MLLE_PROTOCOL_COMMAND_ID_SIZE);
    assert(msg_form < MLLE_PROTOCOL_MSG_FORM_SIZE);
    if (id == MLLE_PROTOCOL_UNDEFINED_CMD) {
        snprintf(error_msg, error_length, "Unknown command %s.", tokens[0]);
        return LE_UNKNOWN_CMD;
    }

    /* Check that the command has the correct number of tokens. */
    expected_nbr_tokens = mlle_nbr_tokens_for_form[msg_form];
    if (ntokens != expected_nbr_tokens) {

        int print_length = 0;

        print_length = snprintf(error_msg, error_length,
                "Too %s arguments for command %s. Expected form is ",
                ntokens > expected_nbr_tokens ? "many" : "few", tokens[0]);
        if (print_length > 0) {
            snprintf(&error_msg[print_length], error_length - print_length,
                    expected_form_template[msg_form], tokens[0]);
        }

        return ntokens > expected_nbr_tokens
               ? LE_TOO_MANY_TOKENS : LE_TOO_FEW_TOKENS;
    }

    /* Parse additionals tokens after the first (for message forms other than
     * simple form).
     */
    if (msg_form == MLLE_PROTOCOL_SIMPLE_MSG_FORM) {
        command->id = id;
        grammar_error = LE_VALID_GRAMMAR;
    } else if (msg_form == MLLE_PROTOCOL_NUMBER_MSG_FORM || msg_form == MLLE_PROTOCOL_LENGTH_MSG_FORM
               || msg_form == MLLE_PROTOCOL_NUMBER_AND_LENGTH_MSG_FORM)
    {
        long n1 = 0;
        long n2 = 0;
        int error = 0;

        /* Parse second token. */
        error = parse_int(&n1, tokens[1], tokens[0], error_msg, error_length);
        if (error) {
            return LE_NOT_AN_INT;
        }
        if (msg_form == MLLE_PROTOCOL_NUMBER_AND_LENGTH_MSG_FORM) {
            /* Parse third token. */
            error |= parse_int(&n2, tokens[2], tokens[0], error_msg, error_length);
            if (error) {
                return LE_NOT_AN_INT;
            }
        }

        /* Assign values to command struct. */
        if (msg_form == MLLE_PROTOCOL_NUMBER_MSG_FORM){
            command->id = id;
            command->number = n1;
            grammar_error = LE_VALID_GRAMMAR;
        } else if (msg_form == MLLE_PROTOCOL_LENGTH_MSG_FORM || msg_form == MLLE_PROTOCOL_NUMBER_AND_LENGTH_MSG_FORM) {
            long length = 0;
            if (msg_form == MLLE_PROTOCOL_LENGTH_MSG_FORM) {
                length = n1;
            } else {
                length = n2;
            }
            if (length < 0) {
                snprintf(error_msg, error_length,
                         "Parse error. Length argument %ld to command %s is negative.",
                         length, tokens[0]);
                return LE_NEGATIVE_LENGTH;
            }

            command->id = id;
            if (msg_form == MLLE_PROTOCOL_NUMBER_AND_LENGTH_MSG_FORM) {
                command->number = n1;
            }
            command->length = length;
            grammar_error = LE_VALID_GRAMMAR;
        }
    }

    return grammar_error;
}
