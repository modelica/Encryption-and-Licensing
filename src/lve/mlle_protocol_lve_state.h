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

#ifndef MLLE_PROTOCOL_LVE_STATE_H_
#define MLLE_PROTOCOL_LVE_STATE_H_

#define _XOPEN_SOURCE 700
#include <stddef.h>
#include "mlle_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* States are named after the ("earliest") command it accepts. */
enum mlle_lve_state {
    MLLE_LVE_STATE_INVALID,
    MLLE_LVE_STATE_VERSION,
    MLLE_LVE_STATE_TOOLS,
    MLLE_LVE_STATE_LIB,
    //MLLE_LVE_STATE_PUBKEY,
    MLLE_LVE_STATE_LICENSE,
    MLLE_LVE_STATE_SIZE
};

extern const enum mlle_lve_state
mlle_lve_valid_command_for_state[MLLE_PROTOCOL_COMMAND_ID_SIZE][MLLE_LVE_STATE_SIZE];

enum mlle_lve_state
mlle_next_state(enum mlle_lve_state current_state,
                enum mlle_protocol_command_id command_id,
                char *error_msg,
                size_t error_length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_PROTOCOL_LVE_STATE_H_ */
