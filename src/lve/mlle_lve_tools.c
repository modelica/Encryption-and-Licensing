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

// Disable "deprecated" warning.
#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _XOPEN_SOURCE 700
#include "mlle_lve.h"
#include "mlle_lve_tools.h"
#include "mlle_io.h"

int
mlle_lve_tools(struct mlle_lve_ctx *lve_ctx)
{
    // TODO Just temporary. Should sent back something else
    // but nothing is implemented right now.
    mlle_send_number_form(lve_ctx->ssl, MLLE_PROTOCOL_VERSION_CMD,
                       4);
    return 1;
}
