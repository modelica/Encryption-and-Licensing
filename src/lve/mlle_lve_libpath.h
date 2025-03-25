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

#ifndef MLLE_LVE_LIBPATH_H_
#define MLLE_LVE_LIBPATH_H_

#define _XOPEN_SOURCE 700
#include "mlle_lve.h"
#include "mlle_protocol.h"
#include "mlle_error.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int
mlle_lve_libpath(struct mlle_lve_ctx *lve_ctx,
                 const struct mlle_command *command);


/*************************************************************
 * Validate the path and check if it is a directory.
 *
 * Parameters:
 *      lve_ctx - structure containing the path to validate.
 *
 * Returns:
 *      1 - validation succeeded.
 *      0 - validation failed.
 ************************************************************/
int validate_directory(struct mlle_lve_ctx **lve_ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_LVE_LIBPATH_H_ */
