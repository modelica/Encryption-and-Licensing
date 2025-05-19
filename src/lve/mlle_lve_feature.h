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

#ifndef MLLE_LVE_FEATURE_H_
#define MLLE_LVE_FEATURE_H_

#define _XOPEN_SOURCE 700
#include "mlle_lve.h"
#include "mlle_protocol.h"
#include "mlle_error.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
mlle_lve_feature(struct mlle_lve_ctx *lve_ctx,
                 const struct mlle_command *command,
                 const int is_in_checkout_feature_without_tool_mode);

int
mlle_lve_returnfeature(struct mlle_lve_ctx *lve_ctx,
                       const struct mlle_command *command);

int
mlle_lve_setup_licensing(struct mlle_lve_ctx *lve_ctx, struct mlle_error **error);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_LVE_FEATURE_H_ */
