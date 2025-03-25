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
/* libcrypto-compat.h must be first */
#include "libcrypto-compat.h"

#include "mlle_protocol.h"
#include "mlle_error.h"
#include "mlle_io.h"
#include "mlle_lve.h"
#include "mlle_lve_license.h"

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif /* __GNUC__ */

int
mlle_lve_license(struct mlle_lve_ctx *lve_ctx,
                 const struct mlle_command *command)
{
    mlle_send_simple_form(lve_ctx->ssl, MLLE_PROTOCOL_NOTSIMPLE_CMD);
    return 0;
}

int
mlle_lve_returnlicense(struct mlle_lve_ctx *lve_ctx,
                       const struct mlle_command *command)
{
    mlle_send_simple_form(lve_ctx->ssl, MLLE_PROTOCOL_NOTSIMPLE_CMD);
    return 0;
}
