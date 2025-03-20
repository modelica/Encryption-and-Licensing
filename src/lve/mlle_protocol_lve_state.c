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

#define _XOPEN_SOURCE 700
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

/* libcrypto-compat.h must be first */
#include "libcrypto-compat.h"

#include "mlle_protocol.h"
#include "mlle_protocol_lve_state.h"

const enum mlle_lve_state
mlle_lve_valid_command_for_state[MLLE_PROTOCOL_COMMAND_ID_SIZE][MLLE_LVE_STATE_SIZE] = {
/*                           MLLE_LVE_STATE_INVALID   MLLE_LVE_STATE_VERSION   MLLE_LVE_STATE_TOOLS     MLLE_LVE_STATE_LIB       MLLE_LVE_STATE_LICENSE */
/* LE_UNDEFINED_CMD     */ { MLLE_LVE_STATE_INVALID },
/* LE_NOTSIMPLE_CMD     */ { MLLE_LVE_STATE_INVALID },
/* LE_TOOLS_CMD         */ { MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_LIB,      MLLE_LVE_STATE_INVALID },
/* LE_YES_CMD           */ { MLLE_LVE_STATE_INVALID },
/* LE_VERSION_CMD       */ { MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_TOOLS,    MLLE_LVE_STATE_INVALID },
/* LE_FEATURE_CMD       */ { MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_LICENSE },
/* LE_FILE_CMD          */ { MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_LICENSE },
/* LE_FILECONT_CMD      */ { MLLE_LVE_STATE_INVALID },
/* LE_LIB_CMD           */ { MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_LICENSE,   MLLE_LVE_STATE_LICENSE},
/* LE_LICENSE_CMD       */ { MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_LICENSE },
/* LE_NO_CMD            */ { MLLE_LVE_STATE_INVALID },
/* LE_RETURNFEATURE_CMD */ { MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_LICENSE },
/* LE_RETURNLICENSE_CMD */ { MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_INVALID,  MLLE_LVE_STATE_LICENSE },
/* LE_TOOLLIST_CMD      */ { MLLE_LVE_STATE_INVALID },
/* LE_ERROR_CMD         */ { MLLE_LVE_STATE_INVALID },
};

#define VALID_COMMANDS_LEN (MLLE_PROTOCOL_COMMAND_ID_SIZE * (MLLE_PROTOCOL_MAX_CMD_LENGTH + 2))

enum mlle_lve_state
mlle_next_state(enum mlle_lve_state current_state,
                enum mlle_protocol_command_id command_id,
                char *error_msg,
                size_t error_length)
{
    enum mlle_lve_state next_state = MLLE_LVE_STATE_INVALID;

    assert(current_state > MLLE_LVE_STATE_INVALID && current_state < MLLE_LVE_STATE_SIZE);
    assert(command_id > MLLE_PROTOCOL_UNDEFINED_CMD && command_id < MLLE_PROTOCOL_COMMAND_ID_SIZE);

    next_state = mlle_lve_valid_command_for_state[command_id][current_state];
    assert(next_state < MLLE_LVE_STATE_SIZE);

    if (next_state == MLLE_LVE_STATE_INVALID) {
        enum mlle_lve_state state = MLLE_LVE_STATE_INVALID;
        int is_valid_to_lve = 0;
        enum mlle_protocol_command_id cmd = MLLE_PROTOCOL_UNDEFINED_CMD;
        int n = 0;
        char valid_commands[VALID_COMMANDS_LEN] = { 0 };
        int i = 0;

        for (state = MLLE_LVE_STATE_INVALID + 1; state < MLLE_LVE_STATE_SIZE; state++) {
            if (mlle_lve_valid_command_for_state[command_id][state] != MLLE_LVE_STATE_INVALID) {
                is_valid_to_lve = 1;
                break;
            }
        }
        if (is_valid_to_lve) {
            for (cmd = MLLE_PROTOCOL_UNDEFINED_CMD + 1; cmd < MLLE_PROTOCOL_COMMAND_ID_SIZE; cmd++) {
                if (mlle_lve_valid_command_for_state[cmd][current_state] != MLLE_LVE_STATE_INVALID) {
                    i += snprintf(&valid_commands[i], VALID_COMMANDS_LEN - i, "%s%s",
                            (n > 0 ? ", " : ""), mlle_command_info[cmd].name);
                    n++;
                }
            }
            snprintf(error_msg, error_length,
                     "Protocol error: Command %s is not valid in this state, "
                     "expected command %s%s.", mlle_command_info[command_id].name,
                     (n > 1 ? "is one of " : ""), valid_commands);
        } else {
            snprintf(error_msg, error_length,
                     "Protocol error: Command %s can't be sent to the library vendor executable.",
                     mlle_command_info[command_id].name);
        }
    }

    return next_state;
}
