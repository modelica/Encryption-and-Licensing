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

#ifndef MLLE_LICENSE_MANAGER_H_
#define MLLE_LICENSE_MANAGER_H_

#include <stddef.h>
#include "mlle_error.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LICENSE_DOMAIN                       42

#define LICENSE_ERROR_INVALID_REFERENCE       0
#define LICENSE_ERROR_INITIALIZATION_FAILURE  1
#define LICENSE_ERROR_NOT_INITIALIZED         2
#define LICENSE_ERROR_CHECKOUT_FAILURE        3
#define LICENSE_ERROR_CHECKIN_FAILURE         4
#define LICENSE_ERROR_INTERNAL                5
#define LICENSE_ERROR_BAD_FEATURE             6

#define MLLE_LIC_SUCCESS 1
#define MLLE_LIC_FAILURE (!(MLLE_LIC_SUCCESS))

struct mlle_license;

struct mlle_license *
mlle_license_new(const char *libpath,
                 struct mlle_error **error);

void
mlle_license_free(struct mlle_license *mlic);

int
mlle_license_checkout_feature(struct mlle_license *mlic,
                              size_t feature_length,
                              const char *feature,
                              struct mlle_error **error);

int
mlle_license_checkin_feature(struct mlle_license *mlic,
                             size_t feature_length,
                             const char *feature,
                             struct mlle_error **error);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLLE_LICENSE_MANAGER_H_ */
