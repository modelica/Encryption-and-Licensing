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

#define _XOPEN_SOURCE 700
#include <stddef.h>
#include "mlle_protocol.h"

const size_t mlle_nbr_tokens_for_form[MLLE_PROTOCOL_MSG_FORM_SIZE] = {
        0, 1, 2, 2, MLLE_PROTOCOL_MAX_TOKENS_PER_MSG
};


const struct mlle_command_info mlle_command_info[MLLE_PROTOCOL_COMMAND_ID_SIZE] = {
    { MLLE_PROTOCOL_UNDEFINED_CMD,     MLLE_PROTOCOL_UNDEFINED_MSG_FORM,         "[unknown_command]" },

    /* Commands of simple form. */
    { MLLE_PROTOCOL_NOTSIMPLE_CMD,     MLLE_PROTOCOL_SIMPLE_MSG_FORM,            "NOTSIMPLE" },
    { MLLE_PROTOCOL_TOOLS_CMD,         MLLE_PROTOCOL_SIMPLE_MSG_FORM,            "TOOLS" },
    { MLLE_PROTOCOL_YES_CMD,           MLLE_PROTOCOL_SIMPLE_MSG_FORM,            "YES" },

    /* Commands of number form. */
    { MLLE_PROTOCOL_VERSION_CMD,       MLLE_PROTOCOL_NUMBER_MSG_FORM,            "VERSION" },

    /* Commands of length form. */
    { MLLE_PROTOCOL_FEATURE_CMD,       MLLE_PROTOCOL_LENGTH_MSG_FORM,            "FEATURE" },
    { MLLE_PROTOCOL_FILE_CMD,          MLLE_PROTOCOL_LENGTH_MSG_FORM,            "FILE" },
    { MLLE_PROTOCOL_FILECONT_CMD,      MLLE_PROTOCOL_LENGTH_MSG_FORM,            "FILECONT" },
    { MLLE_PROTOCOL_LIB_CMD,           MLLE_PROTOCOL_LENGTH_MSG_FORM,            "LIB" },
    { MLLE_PROTOCOL_LICENSE_CMD,       MLLE_PROTOCOL_LENGTH_MSG_FORM,            "LICENSE" },
    { MLLE_PROTOCOL_NO_CMD,            MLLE_PROTOCOL_LENGTH_MSG_FORM,            "NO" },
    //{ MLLE_PROTOCOL_PUBKEY_CMD,        MLLE_PROTOCOL_LENGTH_MSG_FORM,            "PUBKEY" },
    { MLLE_PROTOCOL_RETURNFEATURE_CMD, MLLE_PROTOCOL_LENGTH_MSG_FORM,            "RETURNFEATURE" },
    { MLLE_PROTOCOL_RETURNLICENSE_CMD, MLLE_PROTOCOL_LENGTH_MSG_FORM,            "RETURNLICENSE" },
    { MLLE_PROTOCOL_TOOLLIST_CMD,      MLLE_PROTOCOL_LENGTH_MSG_FORM,            "TOOLLIST" },

    /* Commands of number and length form. */
    { MLLE_PROTOCOL_ERROR_CMD,         MLLE_PROTOCOL_NUMBER_AND_LENGTH_MSG_FORM, "ERROR" },
};

