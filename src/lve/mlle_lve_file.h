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

#ifndef MLLE_LVE_FILE_H_
#define MLLE_LVE_FILE_H_

#define _XOPEN_SOURCE 700
#include "mlle_lve.h"
#include "mlle_protocol.h"
#include "mlle_error.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MLLE_ENCRYPTED_MODELICA_FILE_EXTENSION ("moc")

int
mlle_lve_file(struct mlle_lve_ctx *lve_ctx,
              const struct mlle_command *command);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_LVE_FILE_H_ */
