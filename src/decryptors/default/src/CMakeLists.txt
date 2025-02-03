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
cmake_minimum_required(VERSION 3.20)
project(MLLECr)
# preprocessor defines
if (DISABLE_DEMASK_KEY)
    set_property(
    SOURCE
    mlle_cr_decrypt.c
    mlle_cr_encrypt.c
    APPEND
    PROPERTY COMPILE_DEFINITIONS
    DISABLE_DEMASK_KEY
    )
endif()
message(STATUS "MLLE_LIST_DIR = ${MLLE_LIST_DIR}")
message(STATUS "MLLE_BINARY_DIR = ${MLLE_BINARY_DIR}")

set(ENCRYPTION_KEY_H "${MLLE_BINARY_DIR}/random_key_file.h")
if(EXISTS ${RANDOM_KEY_DIRECTORY}/random_key_file.h)
	message("Encryption key ${RANDOM_KEY_DIRECTORY}/random_key_file.h exists, making local copy to build dir")
	add_custom_command(
		OUTPUT "${ENCRYPTION_KEY_H}"
		COMMAND "${CMAKE_COMMAND}" -E remove -f ${MLLE_BINARY_DIR}/random_key_file.h
		COMMAND "${CMAKE_COMMAND}" -E copy  ${RANDOM_KEY_DIRECTORY}/random_key_file.h ${MLLE_BINARY_DIR}/
        WORKING_DIRECTORY ${MLLE_BINARY_DIR}
	)
else()
	message("Could not find Encryption key ${RANDOM_KEY_DIRECTORY}/random_key_file.h, generating new one")
    if (USE_DOWNLOADED_OPENSSL_BUILD)
        set(RANDOMIZE_KEY_COMMAND_DEPENDS randomize_key)
    elseif (USE_CUSTOM_OPENSSL_SUBDIRECTORY)
        set(RANDOMIZE_KEY_COMMAND_DEPENDS randomize_key)
    else()
        set(RANDOMIZE_KEY_COMMAND_DEPENDS randomize_key openssl)
    endif()
	add_custom_command(
		OUTPUT "${ENCRYPTION_KEY_H}"
		COMMAND "${CMAKE_COMMAND}" -E remove -f ${MLLE_BINARY_DIR}/random_key_file.h
		COMMAND randomize_key "${ENCRYPTION_KEY_H}" MLLE_CR_KEY
		DEPENDS ${RANDOMIZE_KEY_COMMAND_DEPENDS}
		)
endif()


include_directories(${MLLE_LIST_DIR}
                    ${MLLE_LIST_DIR}/common
                    ${MLLE_LIST_DIR}/decryptors/include
                    ${MLLE_BINARY_DIR})
                    
# Add all required source files to library "license_manager"
add_library(decryptor
            ${ENCRYPTION_KEY_H}
            mlle_cr_decrypt.c
            mlle_cr_encrypt.c
            mlle_cr_context.h
            ../../include/mlle_cr_decrypt.h
            ../../include/mlle_cr_encrypt.h
)
