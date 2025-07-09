# Copyright (C) 2022 Modelica Association
include(ExternalProject)

set(openssl_dir ${CMAKE_CURRENT_BINARY_DIR}/openssl)
# - OS
if(WIN32)
    # Values for Windows
    if(BITS64)
        if (MSVC)
            set(openssl_conf_cmd    perl Configure VC-WIN64A)
            set(openssl_conf_extra  perl configdata.pm --dump)
        else()
            set(openssl_conf_cmd    perl Configure mingw64)
            set(openssl_conf_extra  make depend)
        endif()
    else()
        message(FATAL_ERROR "Target is 32 bits - Building OpenSSL on 32 bits is not supported")
    endif()
    
    if (MSVC)
        set(openssl_make        nmake)
        set(openssl_crypto_lib  libcrypto.lib)
        set(openssl_ssl_lib     libssl.lib)
        set(openssl_install_prefix "/Program Files/OpenSSL")
    else()
        set(openssl_make        make)
        set(openssl_crypto_lib  libcrypto.a)
        set(openssl_ssl_lib     libssl.a)
        set(openssl_install_prefix "/usr/local")
    endif()
else()
    # Values for Linux, etc
    set(openssl_conf_cmd    ./config shared)
    set(openssl_conf_extra  make depend)
    set(openssl_make        make)
    set(openssl_crypto_lib  libcrypto.a)
    set(openssl_ssl_lib     libssl.a)
    set(openssl_install_prefix "/usr/local")
endif()

if(NOT DOWNLOADED_OPENSSL_SOURCE_URL)
    set(DOWNLOADED_OPENSSL_SOURCE_URL https://github.com/openssl/openssl/releases/download/openssl-3.0.17/openssl-3.0.17.tar.gz)
endif()
if(NOT DOWNLOADED_OPENSSL_SOURCE_URL_HASH_SHA256)
    set(DOWNLOADED_OPENSSL_SOURCE_URL_HASH_SHA256 dfdd77e4ea1b57ff3a6dbde6b0bdc3f31db5ac99e7fdd4eaf9e1fbb6ec2db8ce)
endif()
message(STATUS "Will build OpenSSL downloaded from ${DOWNLOADED_OPENSSL_SOURCE_URL} as external project")

ExternalProject_Add(openssl
    PREFIX ${openssl_dir}
    URL ${DOWNLOADED_OPENSSL_SOURCE_URL}
    URL_HASH SHA256=${DOWNLOADED_OPENSSL_SOURCE_URL_HASH_SHA256}
    TLS_VERIFY true
    NETRC OPTIONAL
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${openssl_conf_cmd} no-shared no-idea no-mdc2 no-rc5 --libdir=lib --openssldir=.
    COMMAND ${openssl_conf_extra}
    BUILD_COMMAND ${openssl_make}
    INSTALL_COMMAND ${openssl_make} install_sw install_ssldirs DESTDIR=${openssl_dir}
    BUILD_BYPRODUCTS "${openssl_dir}${openssl_install_prefix}/lib/${openssl_ssl_lib}" "${openssl_dir}${openssl_install_prefix}/lib/${openssl_crypto_lib}"
)
add_library(ssl STATIC IMPORTED)
add_library(crypto STATIC IMPORTED)
set_property(TARGET ssl PROPERTY IMPORTED_LOCATION "${openssl_dir}${openssl_install_prefix}/lib/${openssl_ssl_lib}")
set_property(TARGET crypto PROPERTY IMPORTED_LOCATION "${openssl_dir}${openssl_install_prefix}/lib/${openssl_crypto_lib}")
set(OPENSSL_INCLUDE_DIR "${openssl_dir}${openssl_install_prefix}/include")
add_dependencies(ssl openssl)
add_dependencies(crypto openssl)

include_directories(${OPENSSL_INCLUDE_DIR})
