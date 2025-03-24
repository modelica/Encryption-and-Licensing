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
#include <stddef.h>
/* libcrypto-compat.h must be first */
#include "libcrypto-compat.h"

#include "mlle_lve.h"
#include "mlle_protocol.h"
#include "mlle_error.h"
#include "mlle_io.h"
#include "mlle_license_manager.h"
#include "mlle_lve_feature.h"

int
mlle_lve_setup_licensing(struct mlle_lve_ctx *lve_ctx, struct mlle_error **error)
{
    if (lve_ctx->lic_mgr == NULL) {
        lve_ctx->lic_mgr = mlle_license_new(lve_ctx->libpath, error);
    }

    return lve_ctx->lic_mgr != NULL;
}

int
mlle_lve_setup_licensing_check_error(struct mlle_lve_ctx *lve_ctx)
{
    struct mlle_error *error = NULL;

    if (!mlle_lve_setup_licensing(lve_ctx, &error)) {
        mlle_send_error(lve_ctx->ssl, MLLE_PROTOCOL_LICENSE_ERROR,
                mlle_error_get_message(error));
        mlle_error_free(&error);
        return 0;
    }

    return 1;
}

int
mlle_lve_feature(struct mlle_lve_ctx *lve_ctx,
                 const struct mlle_command *command,
                 const int is_in_checkout_feature_without_tool_mode)
{
    struct mlle_error *error = NULL;
    int success = 0;

    if (!mlle_lve_setup_licensing_check_error(lve_ctx)) {
        return 0;
    }

    success = mlle_license_checkout_feature(lve_ctx->lic_mgr, command->length,
            command->data, &error);
    if (success) {
        if (!is_in_checkout_feature_without_tool_mode) {
            mlle_send_simple_form(lve_ctx->ssl, MLLE_PROTOCOL_YES_CMD);
        }
        return 1;
    } else {
        if (!is_in_checkout_feature_without_tool_mode) {
            mlle_send_string(lve_ctx->ssl, MLLE_PROTOCOL_NO_CMD,
                    mlle_error_get_message(error));
        }
        mlle_error_free(&error);
    
        return 0;
    }
}

int
mlle_lve_returnfeature(struct mlle_lve_ctx *lve_ctx,
                       const struct mlle_command *command)
{
    struct mlle_error *error = NULL;
    int success = 0;

    if (!mlle_lve_setup_licensing_check_error(lve_ctx)) {
        return 0;
    }

    success = mlle_license_checkin_feature(lve_ctx->lic_mgr, command->length,
            command->data, &error);
    if (success) {
        mlle_send_simple_form(lve_ctx->ssl, MLLE_PROTOCOL_YES_CMD);
        return 1;
    } else {
        mlle_send_error(lve_ctx->ssl, MLLE_PROTOCOL_LICENSE_ERROR,
                mlle_error_get_message(error));
        mlle_error_free(&error);
        return 0;
    }
}
