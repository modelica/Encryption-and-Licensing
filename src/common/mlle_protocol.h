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

#ifndef MLLE_PROTOCOL_H_
#define MLLE_PROTOCOL_H_

#define _XOPEN_SOURCE 700
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 1023 is just an initial guess */
#define MLLE_PROTOCOL_MAX_COMMAND_LINE_SIZE (1023)
#define MLLE_PROTOCOL_MAX_TOKENS_PER_MSG (3)
#define MLLE_PROTOCOL_TOO_MANY_TOKENS (-1)
#define MLLE_PROTOCOL_MAX_CMD_LENGTH (17)

enum mlle_protocol_msg_form {
    MLLE_PROTOCOL_UNDEFINED_MSG_FORM,
    MLLE_PROTOCOL_SIMPLE_MSG_FORM,
    MLLE_PROTOCOL_NUMBER_MSG_FORM,
    MLLE_PROTOCOL_LENGTH_MSG_FORM,
    MLLE_PROTOCOL_NUMBER_AND_LENGTH_MSG_FORM,

    /* This value MUST be the last in the enum or allocation of buffers will be too small! */
    MLLE_PROTOCOL_MSG_FORM_SIZE
};

enum mlle_protocol_command_id {
    MLLE_PROTOCOL_UNDEFINED_CMD,
    MLLE_PROTOCOL_NOTSIMPLE_CMD,
    MLLE_PROTOCOL_TOOLS_CMD,
    MLLE_PROTOCOL_YES_CMD,
    MLLE_PROTOCOL_VERSION_CMD,
    MLLE_PROTOCOL_FEATURE_CMD,
    MLLE_PROTOCOL_FILE_CMD,
    MLLE_PROTOCOL_FILECONT_CMD,
    MLLE_PROTOCOL_LIB_CMD,
    MLLE_PROTOCOL_LICENSE_CMD,
    MLLE_PROTOCOL_NO_CMD,
    MLLE_PROTOCOL_RETURNFEATURE_CMD,
    MLLE_PROTOCOL_RETURNLICENSE_CMD,
    MLLE_PROTOCOL_TOOLLIST_CMD,
    MLLE_PROTOCOL_ERROR_CMD,

    /* This value MUST be the last in the enum or allocation of buffers will be too small! */
    MLLE_PROTOCOL_COMMAND_ID_SIZE
};

enum mlle_protocol_error_id {
    MLLE_PROTOCOL_UNDEFINED_ERROR,
    MLLE_PROTOCOL_COMMAND_NOT_UNDERSTOOD_ERROR,
    MLLE_PROTOCOL_VERSION_TOO_LOW_ERROR,
    MLLE_PROTOCOL_LVE_NOT_LICENSING_ERROR,
    MLLE_PROTOCOL_FILE_NOT_FOUND_ERROR,
    MLLE_PROTOCOL_TOOL_NOT_ALLOWED_ERROR,
    MLLE_PROTOCOL_FILE_IO_ERROR,
    MLLE_PROTOCOL_LICENSE_ERROR,
    MLLE_PROTOCOL_OTHER_ERROR,
    MLLE_PROTOCOL_SSL_ERROR,

    /* This value MUST be the last in the enum or allocation of buffers will be too small! */
    MLLE_PROTOCOL_ERROR_ID_SIZE
};

struct mlle_command_info {
    enum mlle_protocol_command_id  id;
    enum mlle_protocol_msg_form    msg_form;
    const char                     *name;
};

struct mlle_command {
    enum mlle_protocol_command_id id;
    long                          number;
    size_t                        length;
    char                          *data;
};

extern const size_t
mlle_nbr_tokens_for_form[MLLE_PROTOCOL_MSG_FORM_SIZE];

extern const struct mlle_command_info
mlle_command_info[MLLE_PROTOCOL_COMMAND_ID_SIZE];


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_PROTOCOL_H_ */
