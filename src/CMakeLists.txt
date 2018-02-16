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


# -------------------------------------------------------------------
# This is the make-file for the public version of LibraryEncryption.
# -------------------------------------------------------------------

cmake_minimum_required(VERSION 2.8)
project(MLLE)
include(ExternalProject)


# Flags
set(CMAKE_BUILD_TYPE Release)
if(WIN32 AND NOT DYNAMIC)
    foreach(flag_var
            CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flag_var} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
endif(WIN32 AND NOT DYNAMIC)

# Propagate value of GLOBAL_LICENSE_FEATURE to build
if(DEFINED GLOBAL_LICENSE_FEATURE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLLE_GLOBAL_LICENSE_FEATURE=${GLOBAL_LICENSE_FEATURE}")
endif(DEFINED GLOBAL_LICENSE_FEATURE)

# Platforms
# - Bitness
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8" OR (WIN32 AND CMAKE_GENERATOR MATCHES "Win64$"))
    message(STATUS "Target is 64 bits")
    set(BITS64 1)
    set(BITS 64)
else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    message(STATUS "Target is 32 bits")
    set(BITS64 0)
    set(BITS 32)
endif()
# - OS
if(WIN32)
    # Values for Windows
    if(BITS64)
        if (MSVC)
            set(openssl_conf_cmd    perl Configure VC-WIN64A)
            set(openssl_conf_extra  ms/do_win64a)
        else()
            set(openssl_conf_cmd    perl Configure mingw64)
            set(openssl_conf_extra  make depend)
        endif()
    else()
        if (MSVC)
            set(openssl_conf_cmd    perl Configure VC-WIN32)
            set(openssl_conf_extra  ms/do_nasm)
        else()
            set(openssl_conf_cmd    perl Configure mingw)
            set(openssl_conf_extra  make depend)
        endif()
    endif()
    if (MSVC)
        set(openssl_make        nmake -f ms/nt.mak)
        set(openssl_crypto_lib  libeay32.lib)
        set(openssl_ssl_lib     ssleay32.lib)
    else()
        set(openssl_make        make)
        set(openssl_crypto_lib  libcrypto.a)
        set(openssl_ssl_lib     libssl.a)
    endif()
    set(extra_ssl_libs      crypt32 ws2_32)
    set(tool_platform_c     ${CMAKE_CURRENT_LIST_DIR}/tool/mlle_spawn_windows.c)
else()
    # Values for Linux, etc
    set(openssl_conf_cmd    ./config shared)
    set(openssl_conf_extra  make depend)
    set(openssl_make        make)
    set(openssl_crypto_lib  libcrypto.a)
    set(openssl_ssl_lib     libssl.a)
    set(extra_ssl_libs      dl)
    set(tool_platform_c     ${CMAKE_CURRENT_LIST_DIR}/tool/mlle_spawn_posix.c)
endif()

# Libraries
# - OpenSSL
ExternalProject_Add(openssl
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/openssl
    URL ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/openssl-1.0.2
    BUILD_IN_SOURCE 1 
    CONFIGURE_COMMAND ${openssl_conf_cmd} no-idea no-mdc2 no-rc5 --prefix=${CMAKE_CURRENT_BINARY_DIR}/openssl
    COMMAND ${openssl_conf_extra}
    BUILD_COMMAND ${openssl_make} 
    INSTALL_COMMAND ${openssl_make} install
)
set(openssl_dir ${CMAKE_CURRENT_BINARY_DIR}/openssl)
add_library(ssl STATIC IMPORTED)
add_library(crypto STATIC IMPORTED)
set_property(TARGET ssl PROPERTY IMPORTED_LOCATION ${openssl_dir}/lib/${openssl_ssl_lib})
set_property(TARGET crypto PROPERTY IMPORTED_LOCATION ${openssl_dir}/lib/${openssl_crypto_lib})
add_dependencies(ssl openssl)
add_dependencies(crypto openssl)
message(STATUS "Current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${openssl_dir}/include 
                    ${CMAKE_CURRENT_LIST_DIR}/encrypt_decrypt 
                    ${CMAKE_CURRENT_LIST_DIR}/decryptors/include 
                    ${CMAKE_CURRENT_LIST_DIR}/license_managers/include 
                    ${CMAKE_CURRENT_LIST_DIR}/obfuscators/include 
                    ${CMAKE_CURRENT_LIST_DIR}/embedfile 
                    ${CMAKE_CURRENT_LIST_DIR}/common 
                    ${CMAKE_CURRENT_LIST_DIR}/lve 
                    ${CMAKE_CURRENT_LIST_DIR}/tool 
                    ${CMAKE_CURRENT_BINARY_DIR})


# Compile obfuscate program.
add_executable(obfuscate 
               embedfile/obfuscate.c 
               embedfile/obfuscate_utils.c)
target_link_libraries(obfuscate 
                      ssl 
                      crypto 
                      obfuscator_module)

# Compile the randomize key program.
add_executable(randomize_key 
               embedfile/randomize_key.c 
               embedfile/obfuscate_utils.c)
target_link_libraries(randomize_key 
                      ssl 
                      crypto 
                      ${extra_ssl_libs} 
                      obfuscator_module)



# -------------------------------------------
# Key length to use when generating RSA keys.
# -------------------------------------------
if(NOT DEFINED RSA_KEY_LENGTH)
    set(RSA_KEY_LENGTH 4096)
endif()

# -------------------------------------------------
# Optional directory for the private/public keys. 
# -------------------------------------------------
set(KEYS_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/openssl_keys)

# ------------------------------------------------------------
# Directory to use if keys are missing from source directory.
# ------------------------------------------------------------
set(KEYS_BUILD_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/openssl_keys)

# --------------------------------------
# Use Tool key from source or create a new one.
# --------------------------------------
set(PRIVATE_KEY_TOOL ${KEYS_SOURCE_DIRECTORY}/private_key_tool.pem)
if(EXISTS "${PRIVATE_KEY_TOOL}")
    message("SSL key ${PRIVATE_KEY_TOOL} exists. Using this key.")
else()
    message("SSL key ${PRIVATE_KEY_TOOL} doesn't exist. A new key is created.")
    file(MAKE_DIRECTORY "${KEYS_BUILD_DIRECTORY}")
    set(PRIVATE_KEY_TOOL ${KEYS_BUILD_DIRECTORY}/private_key_tool.pem)
    execute_process(COMMAND openssl genrsa -out ${PRIVATE_KEY_TOOL} ${RSA_KEY_LENGTH})
endif()

# -------------------------------------
# Use LVE key from source or create a new one.
# -------------------------------------
set(PRIVATE_KEY_LVE ${KEYS_SOURCE_DIRECTORY}/private_key_lve.pem)
if(EXISTS "${PRIVATE_KEY_LVE}")
    message("SSL key ${PRIVATE_KEY_LVE} exists. Using this key.")
else()
    message("SSL key ${PRIVATE_KEY_LVE} doesn't exist. A new key is created.")
    file(MAKE_DIRECTORY "${KEYS_BUILD_DIRECTORY}")
    set(PRIVATE_KEY_LVE ${KEYS_BUILD_DIRECTORY}/private_key_lve.pem)
    execute_process(COMMAND openssl genrsa -out ${PRIVATE_KEY_LVE} ${RSA_KEY_LENGTH})
endif()


# ---------------------------------------------
# Use public Tool key from source or extract from private.
# ---------------------------------------------
set(PUBLIC_KEY_TOOL ${KEYS_SOURCE_DIRECTORY}/public_key_tool.pem)
if(EXISTS "${PUBLIC_KEY_TOOL}")
    message("SSL key ${PUBLIC_KEY_TOOL} exists. Using this key.")
else()
    message("SSL key ${PUBLIC_KEY_TOOL} doesn't exist. Public key is extracted from ${PRIVATE_KEY_TOOL}.")
    file(MAKE_DIRECTORY "${KEYS_BUILD_DIRECTORY}")
    set(PUBLIC_KEY_TOOL ${KEYS_BUILD_DIRECTORY}/public_key_tool.pem)
    execute_process(COMMAND openssl rsa -pubout -in "${PRIVATE_KEY_TOOL}" -out "${PUBLIC_KEY_TOOL}")
endif()


# Create header file for tool private key.
set(PRIVATE_KEY_TOOL_H "${CMAKE_CURRENT_BINARY_DIR}/private_key_tool.h")
add_custom_command(
    OUTPUT "${PRIVATE_KEY_TOOL_H}"
    COMMAND obfuscate "${PRIVATE_KEY_TOOL_H}" "${PRIVATE_KEY_TOOL}" PRIVATE_KEY_TOOL LVE_PRIVATE
    DEPENDS "${PRIVATE_KEY_TOOL}" obfuscate
    )
set_source_files_properties("${PRIVATE_KEY_TOOL_H}" PROPERTIES HEADER_FILE_ONLY TRUE)

# Create header file for library private key.
set(PRIVATE_KEY_LVE_H "${CMAKE_CURRENT_BINARY_DIR}/private_key_lve.h")
add_custom_command(
    OUTPUT "${PRIVATE_KEY_LVE_H}"
    COMMAND obfuscate "${PRIVATE_KEY_LVE_H}" "${PRIVATE_KEY_LVE}" PRIVATE_KEY_LVE TOOL_PRIVATE
    DEPENDS "${PRIVATE_KEY_LVE}" obfuscate
    )
set_source_files_properties("${PRIVATE_KEY_LVE_H}" PROPERTIES HEADER_FILE_ONLY TRUE)

# Create header file for tool public key.
set(PUBLIC_KEY_TOOL_H "${CMAKE_CURRENT_BINARY_DIR}/public_key_tool.h")
add_custom_command(
    OUTPUT "${PUBLIC_KEY_TOOL_H}"
    COMMAND obfuscate "${PUBLIC_KEY_TOOL_H}" "${PUBLIC_KEY_TOOL}" PUBLIC_KEY_TOOL TOOL_PUBLIC
    DEPENDS "${PUBLIC_KEY_TOOL}" obfuscate
    )
set_source_files_properties("${PUBLIC_KEY_TOOL_H}" PROPERTIES HEADER_FILE_ONLY TRUE)



add_library(error
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_error.c 
)
# Dummy output which is never actually produced. Anything that depends on
# this will always be rebuilt.
add_custom_target(always_rebuild)


# Modules
set(MLLE_LIST_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(MLLE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(MLLE_THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/../ThirdParty") # TODO: Need to be included?

# - License manager
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/license_managers)

# - Decryptor
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/decryptors)

# - Obfuscator
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/obfuscators)



# --------------
# Create lve.
# --------------
add_executable(lve 
    ${PRIVATE_KEY_LVE_H} 
    ${PUBLIC_KEY_TOOL_H} 
    ${CMAKE_CURRENT_LIST_DIR}/lve/lve.c 
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_error.c 
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_io.c 
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_parse_command.c 
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_protocol.c 
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_ssl.c
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_utils.c
    ${CMAKE_CURRENT_LIST_DIR}/lve/mlle_lve.c 
    ${CMAKE_CURRENT_LIST_DIR}/lve/mlle_protocol_lve_state.c 
    ${CMAKE_CURRENT_LIST_DIR}/lve/mlle_lve_feature.c 
    ${CMAKE_CURRENT_LIST_DIR}/lve/mlle_lve_file.c 
    ${CMAKE_CURRENT_LIST_DIR}/lve/mlle_lve_libpath.c 
    ${CMAKE_CURRENT_LIST_DIR}/lve/mlle_lve_license.c 
    ${CMAKE_CURRENT_LIST_DIR}/lve/mlle_lve_pubkey.c 
    ${CMAKE_CURRENT_LIST_DIR}/lve/mlle_lve_tools.c 
    ${CMAKE_CURRENT_LIST_DIR}/lve/mlle_ssl_lve.c 
)

target_link_libraries(lve license_manager decryptor ssl crypto ${extra_ssl_libs})

if(WIN32)
    if (MSVC)
      set_target_properties(lve PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    endif (MSVC)
endif(WIN32)

# --------------------
# Create tool library.
# --------------------
add_library(tool
    ${PRIVATE_KEY_TOOL_H} 
    ${CMAKE_CURRENT_LIST_DIR}/tool/mlle_licensing.c 
    ${CMAKE_CURRENT_LIST_DIR}/tool/mlle_ssl_tool.c 
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_protocol.c 
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_error.c 
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_io.c 
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_parse_command.c 
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_ssl.c
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_utils.c 
    ${tool_platform_c}
)

add_executable(test_tool 
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_tool.c  
)

target_link_libraries(test_tool tool ssl crypto ${extra_ssl_libs})

file(COPY ${CMAKE_CURRENT_LIST_DIR}/tool/mlle_licensing.h 
          ${CMAKE_CURRENT_LIST_DIR}/tool/mlle_types.h 
          ${CMAKE_CURRENT_LIST_DIR}/common/mlle_error.h 
     DESTINATION include)

# --------------------
# Create packagetool.
# --------------------
add_executable(packagetool
    ${CMAKE_CURRENT_LIST_DIR}/packagetool/packagetool.c
    ${CMAKE_CURRENT_LIST_DIR}/packagetool/arguments.c
    ${CMAKE_CURRENT_LIST_DIR}/packagetool/manifest.c
    ${CMAKE_CURRENT_LIST_DIR}/packagetool/utils.c    
)

target_link_libraries(packagetool ssl crypto ${extra_ssl_libs} decryptor)

# --------------------
# Create encrypt_file.
# --------------------
add_executable(encrypt_file 
    ${CMAKE_CURRENT_LIST_DIR}/encrypt_decrypt/encrypt_file.c 
)
target_link_libraries(encrypt_file ssl crypto ${extra_ssl_libs} decryptor)

# --------------------
# Create decrypt_file.
# --------------------
add_executable(decrypt_file 
    ${CMAKE_CURRENT_LIST_DIR}/encrypt_decrypt/decrypt_file.c 
)
target_link_libraries(decrypt_file ssl crypto ${extra_ssl_libs} decryptor)


# TODO: Install does not work as intended - needs work or maybe it should just be removed?
install(TARGETS lve test_tool encrypt_file packagetool 
        DESTINATION bin 
        ARCHIVE DESTINATION lib)
# Install header files needed when using tool library
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/ 
                  ${openssl_dir}/include/ 
        DESTINATION include)
# install(FILES mlle_licensing.h 
        # DESTINATION include)

# Install OpenSSL libs as well, since they are needed by tool lib
get_target_property(SSL_LOC ssl LOCATION)
get_target_property(CRYPTO_LOC crypto LOCATION)
install(FILES ${SSL_LOC} ${CRYPTO_LOC} DESTINATION lib)
