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
project(Decryptor)

# Use default SSL RSA decryptor if none is selected
if(NOT DEFINED DECRYPTOR)
   set(DECRYPTOR default)
endif()

set(DECRYPTOR_SRC_FOLDER ${CMAKE_SOURCE_DIR}/decryptors/${DECRYPTOR}/src)
set(DECRYPTOR_BIN_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/decryptors/${DECRYPTOR}/src)
# Check that given decryptor exists, else list available ones
if(NOT EXISTS ${DECRYPTOR_SRC_FOLDER}/CMakeLists.txt)
    if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/${DECRYPTOR}/src/CMakeLists.txt)
        file(GLOB children RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/*)
        file(GLOB children2 RELATIVE ${CMAKE_SOURCE_DIR}/decryptors ${CMAKE_SOURCE_DIR}/decryptors/*)
        set(children "${children};${children2}")
        set(dirlist "")
        set(found 0)
        foreach(child ${children})
            if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${child}/src/CMakeLists.txt)
                set(dirlist "${dirlist} ${child}")
                set(found 1)
            elseif(EXISTS ${CMAKE_SOURCE_DIR}/decryptors/${child}/src/CMakeLists.txt)
                set(dirlist "${dirlist} ${child}")
                set(found 1)
            endif()
        endforeach()
        if(found)
            message(FATAL_ERROR "No such decryptor: ${DECRYPTOR}
    Available decryptors are: ${dirlist}")
        else()
            message(FATAL_ERROR "No decryptors found!")
        endif()
    else()
        set(DECRYPTOR_SRC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/${DECRYPTOR}/src)
        set(DECRYPTOR_BIN_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/${DECRYPTOR}/src)
    endif()
endif()

# Include chosen decryptor
message(STATUS "Using decryptor: ${DECRYPTOR}")
add_subdirectory(${DECRYPTOR_SRC_FOLDER} ${DECRYPTOR_BIN_FOLDER})