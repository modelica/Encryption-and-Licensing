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

#ifndef MLLE_SPAWN_H_
#define MLLE_SPAWN_H_

#include "mlle_types.h"
#include "mlle_error.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct mlle_connections *
mlle_spawn(const char *exec_name,
           struct mlle_error **error);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_SPAWN_H_ */
