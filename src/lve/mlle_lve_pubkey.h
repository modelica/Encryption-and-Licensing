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

#ifndef MLLE_LVE_PUBKEY_H_
#define MLLE_LVE_PUBKEY_H_

#define _XOPEN_SOURCE 700
#include "mlle_lve.h"
#include "mlle_protocol.h"
#include "mlle_error.h"
#include "mlle_ssl.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**************************************************************
 * Validates the public key from a Tool (client) with public
 * tool keys the LVE has access to.
 * The outcome of the validation is set in a variable in the
 * lve structure. If validation failed the lve will still
 * function but not every command can be used.
 *
 * Parameters:
 *      lve_ctx - I/O information for the LVE.
 ************************************************************/
void mlle_lve_validate_pubkey(struct mlle_lve_ctx *lve_ctx);


/*************************************************************
 * Validate the Tool public key against a list of
 * trusted public keys. If key is not valid the Tool is not
 * approved and can't use all the library commands.
 *
 * Returns:
 *      1 - the public key is valid.
 *      0 - the public key is not valid.
 ************************************************************/
int validate();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_LVE_PUBKEY_H_ */
