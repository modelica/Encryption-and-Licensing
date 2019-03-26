#
#    Copyright (C) 2015 Modelon AB
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the BSD style license.
#
#     This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    BSD_License.txt file for more details.
#
#    You should have received a copy of the BSD_License.txt file
#    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
#
cmake_minimum_required(VERSION 2.8)
project(MLLECr)

# Create source file for encryption key for Modelica files.
set(ENCRYPTION_KEY_H "${MLLE_BINARY_DIR}/random_key_file.h")
add_custom_command(
    OUTPUT "${ENCRYPTION_KEY_H}"
    COMMAND randomize_key "${ENCRYPTION_KEY_H}" MLLE_CR_KEY
    DEPENDS randomize_key openssl
    )


string(REPLACE "\\" "/" ENCRYPTION_KEY_H ${ENCRYPTION_KEY_H})

# Add all required source files to library "license_manager"
add_library(decryptor
            ${ENCRYPTION_KEY_H}
            mlle_cr_decrypt.c
            mlle_cr_encrypt.c
)
include_directories(${MLLE_LIST_DIR}
                    ${MLLE_LIST_DIR}/decryptors/include
                    ${MLLE_BINARY_DIR})
