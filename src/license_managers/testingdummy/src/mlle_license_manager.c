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

#include <stddef.h>
#include <string.h>

/* mlle_portability.h must be first */
#include "mlle_portability.h"

#include "mlle_error.h"
#include "mlle_license_manager.h"

struct mlle_license {
    int dummy;
};

static struct mlle_license dummy_lic_mgr = { 0 };

#ifdef GCC
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

struct mlle_license *
mlle_license_new(const char *libpath,
                 struct mlle_error **error)
{
    return &dummy_lic_mgr;
}

void
mlle_license_free(struct mlle_license *mlic)
{
    // Do nothing.
}

int
mlle_license_checkout_feature(struct mlle_license *mlic,
                              size_t feature_length,
                              const char *feature,
                              struct mlle_error **error)
{
    if (!strcmp(feature, "test_licensed_feature")) {
        return 1;
    } else {
        *error = mlle_error_new(1, 3, "Feature not licensed");
        return 0;
    }
}

int
mlle_license_checkin_feature(struct mlle_license *mlic,
                             size_t feature_length,
                             const char *feature,
                             struct mlle_error **error)
{
    return 1;
}
