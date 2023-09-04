include(ExternalProject)

set(openssl_dir ${CMAKE_CURRENT_BINARY_DIR})
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
        if (MSVC)
            set(openssl_conf_cmd    perl Configure no-shared no-idea no-mdc2 no-rc5 --openssldir=. --prefix=${CMAKE_CURRENT_BINARY_DIR} VC-WIN32 )
            set(openssl_conf_extra  perl configdata.pm --dump)
        else()
            set(openssl_conf_cmd    perl Configure mingw)
            set(openssl_conf_extra  make depend)
        endif()
    endif()
    
    if (MSVC)
        set(openssl_make        nmake)
        set(openssl_crypto_lib  libcrypto.lib)
        set(openssl_ssl_lib     libssl.lib)
    else()
        set(openssl_make        make)
        set(openssl_crypto_lib  libcrypto.a)
        set(openssl_ssl_lib     libssl.a)
    endif()
    
    set(extra_ssl_libs      crypt32 ws2_32)

else()
    # Values for Linux, etc
    set(openssl_conf_cmd    ./config shared)
    set(openssl_conf_extra  make depend)
    set(openssl_make        make)
    set(openssl_crypto_lib  libcrypto.a)
    set(openssl_ssl_lib     libssl.a)
    set(extra_ssl_libs      dl pthread)
endif()

# - OpenSSL as external project
if (USE_OPENSSL_10_API)
    message(STATUS "Will build OpenSSL 1.0.2 as external project")
    
    ExternalProject_Add(openssl
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}
        URL ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/openssl-1.0.2
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ${openssl_conf_cmd} no-shared no-idea no-mdc2 no-rc5 --openssldir=. --prefix=${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${openssl_conf_extra}
        BUILD_COMMAND ${openssl_make}
        INSTALL_COMMAND ${openssl_make} install_sw install_ssldirs
        )
elseif(USE_DOWNLOADED_OPENSSL_SOURCE)
    if(NOT DOWNLOADED_OPENSSL_SOURCE_URL)
        set(DOWNLOADED_OPENSSL_SOURCE_URL https://www.openssl.org/source/openssl-3.0.8.tar.gz)
    endif()
    message(STATUS "Will build OpenSSL downloaded from ${DOWNLOADED_OPENSSL_SOURCE_URL} as external project")

    ExternalProject_Add(openssl
        PREFIX ${openssl_dir}
        URL ${DOWNLOADED_OPENSSL_SOURCE_URL}
        TLS_VERIFY true
        NETRC OPTIONAL
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ${openssl_conf_cmd} no-shared no-idea no-mdc2 no-rc5 --openssldir=. --prefix=${openssl_dir}
        COMMAND ${openssl_conf_extra}
        BUILD_COMMAND ${openssl_make}
        INSTALL_COMMAND ${openssl_make} install_sw install_ssldirs
        BUILD_BYPRODUCTS  ${openssl_dir}/lib/${openssl_ssl_lib} ${openssl_dir}/lib/${openssl_crypto_lib}
    )
else()
    message(STATUS "Will build OpenSSL 1.1.1g as external project")
    execute_process(COMMAND "chmod" "a+x" "${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/openssl-1.1.1g/config")
    
    ExternalProject_Add(openssl
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}
        URL  ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/openssl-1.1.1g
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND ${openssl_conf_cmd} no-shared no-idea no-mdc2 no-rc5 --openssldir=. --prefix=${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${openssl_conf_extra}
        BUILD_COMMAND ${openssl_make}
        INSTALL_COMMAND ${openssl_make} install_sw install_ssldirs
        BUILD_BYPRODUCTS  ${openssl_dir}/lib/${openssl_ssl_lib} ${openssl_dir}/lib/${openssl_crypto_lib}
    )
endif()

add_library(ssl STATIC IMPORTED)
add_library(crypto STATIC IMPORTED)
set_property(TARGET ssl PROPERTY IMPORTED_LOCATION ${openssl_dir}/lib/${openssl_ssl_lib})
set_property(TARGET crypto PROPERTY IMPORTED_LOCATION ${openssl_dir}/lib/${openssl_crypto_lib})
add_dependencies(ssl openssl)
add_dependencies(crypto openssl)

set(ssl_libs ssl crypto)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include )