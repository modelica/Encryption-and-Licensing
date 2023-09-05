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

#set(CMAKE_VERBOSE_MAKEFILE ON)

# -------------------------------------------------------------------
# This is the make-file for the public version of LibraryEncryption.
# -------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)
project(MLLE)


# set platform
if (WIN32)
elseif ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin")
    set(DARWIN TRUE)
elseif ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Linux")
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
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
if(NOT "${GLOBAL_LICENSE_FEATURE}" STREQUAL "")
	message(STATUS "Using GLOBAL_LICENSE_FEATURE: ${GLOBAL_LICENSE_FEATURE}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMLLE_GLOBAL_LICENSE_FEATURE=${GLOBAL_LICENSE_FEATURE}")
endif()

# Platforms
# - Bitness
message(STATUS "CMAKE_SIZEOF_VOID_P=${CMAKE_SIZEOF_VOID_P}" )
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8" OR (WIN32 AND CMAKE_GENERATOR MATCHES "Win64$"))
    message(STATUS "Target is 64 bits")
    set(BITS64 1)
    set(BITS 64)
else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    message(STATUS "Target is 32 bits")
    set(BITS64 0)
    set(BITS 32)
endif()


if (WIN32)
    set(LVETARGET "lve_win${BITS}")
elseif (DARWIN)
    set(LVETARGET "lve_darwin${BITS}")
else()
    set(LVETARGET "lve_linux${BITS}")
endif()

# the following commands are needed to fix a problem with the libraries
# for linux 64 bits
if(NOT WIN32)
    message(STATUS "x86_64 architecture detected - setting flag -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -fPIC")
endif()

set(ssl_libs ssl crypto)
if(WIN32)
    set(extra_ssl_libs      crypt32 ws2_32)
else()
    set(extra_ssl_libs      dl pthread)
endif()
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/openssl ${CMAKE_CURRENT_BINARY_DIR}/openssl)

if(WIN32)
    set(tool_platform_c     ${CMAKE_CURRENT_LIST_DIR}/tool/mlle_spawn_windows.c)
else()
    set(tool_platform_c     ${CMAKE_CURRENT_LIST_DIR}/tool/mlle_spawn_posix.c)
endif()

# Libraries
message(STATUS "Current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")
include_directories(${CONAN_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_LIST_DIR}/encrypt_decrypt
                    ${CMAKE_CURRENT_LIST_DIR}/decryptors/include
                    ${CMAKE_CURRENT_LIST_DIR}/license_managers/include
                    ${CMAKE_CURRENT_LIST_DIR}/obfuscators/include
                    ${CMAKE_CURRENT_LIST_DIR}/embedfile
                    ${CMAKE_CURRENT_LIST_DIR}/common
                    ${CMAKE_CURRENT_LIST_DIR}/lve
                    ${CMAKE_CURRENT_LIST_DIR}/tool
                    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/uthash
                    ${CMAKE_CURRENT_BINARY_DIR})


# Compile obfuscate program.
add_executable(obfuscate
               ${CMAKE_CURRENT_LIST_DIR}/embedfile/obfuscate.c)
add_library(obfuscate_utils  
               ${CMAKE_CURRENT_LIST_DIR}/embedfile/obfuscate_utils.c)
target_link_libraries(   obfuscate_utils obfuscator_module        )
target_link_libraries(obfuscate obfuscate_utils
                      ${ssl_libs}
                      ${extra_ssl_libs} )


# Compile the randomize key program.
add_executable(randomize_key
               ${CMAKE_CURRENT_LIST_DIR}/embedfile/randomize_key.c
               )
target_link_libraries(randomize_key
                      ${ssl_libs}
                      ${extra_ssl_libs}
                      obfuscate_utils)

# ---------------
# ENCRYPTION KEYS
# ---------------

# TOOL private key for testing
# ----------------------------
set(PRIVATE_KEY_TOOL ${TOOL_PRIVATE_KEY_DIRECTORY}/private_key_tool.pem)

if("${TOOL_PRIVATE_KEY_DIRECTORY}" STREQUAL "" OR NOT EXISTS "${PRIVATE_KEY_TOOL}")

  message( FATAL_ERROR "\
Error: TOOL private key doesn't exist '${PRIVATE_KEY_TOOL}'
  variable TOOL_PRIVATE_KEY_DIRECTORY = ${TOOL_PRIVATE_KEY_DIRECTORY}
    passed to CMake as -DTOOL_PRIVATE_KEY_DIRECTORY=private_keys_lve_path

  the TOOL_PRIVATE_KEY_DIRECTORY directory must contains:
  
    - the private key for the test tool
	    to generate a keys run the following commands:
		
          openssl genrsa -out \"private_key_tool_path/private_key_tool.pem\" 4096
          dos2unix \"private_key_tool_path/private_key_tool.pem\"

  the same set of keys (and random.h) must be used in all builds or the generated LVEs will not be compatible
")

else()

  # Create header file for TOOL private key.
  set(PRIVATE_KEY_TOOL_H "${CMAKE_CURRENT_BINARY_DIR}/private_key_tool.h")
  set(PRIVATE_KEY_TOOL_H_COMMAND obfuscate "${PRIVATE_KEY_TOOL_H}" "${PRIVATE_KEY_TOOL}" PRIVATE_KEY_TOOL TOOL_PRIVATE)
  
  add_custom_command(
    OUTPUT "${PRIVATE_KEY_TOOL_H}"
    COMMAND ${PRIVATE_KEY_TOOL_H_COMMAND}
    DEPENDS "${PRIVATE_KEY_TOOL}" obfuscate
    COMMENT "Running: ${PRIVATE_KEY_TOOL_H_COMMAND}"
    )
	
  set_source_files_properties("${PRIVATE_KEY_TOOL_H}" PROPERTIES HEADER_FILE_ONLY TRUE)
  
endif()


# LVE private key
# ---------------
set(PRIVATE_KEY_LVE ${LVE_KEYS_DIRECTORY}/private_key_lve.pem)

if( "${LVE_KEYS_DIRECTORY}" STREQUAL "" OR NOT EXISTS "${PRIVATE_KEY_LVE}")

  message( FATAL_ERROR "\
Error: LVE private key doesn't exist '${PRIVATE_KEY_LVE}'
  variable LVE_KEYS_DIRECTORY = ${LVE_KEYS_DIRECTORY}
    passed to CMake as -DLVE_KEYS_DIRECTORY=private_keys_lve_path

  the LVE_KEYS_DIRECTORY directory must contains:
  
    - the private key for the generated LVE
	    to generate a keys run the following commands:
		
          openssl genrsa -out \"private_key_lve_path/private_key_lve.pem\" 4096
          dos2unix \"private_key_lve_path/private_key_lve.pem\"

  the same set of keys (and random.h) must be used in all builds or the generated LVEs will not be compatible
")

else()

  # Create header file for LVE private key.
  set(PRIVATE_KEY_LVE_H "${CMAKE_CURRENT_BINARY_DIR}/private_key_lve.h")
  set(PRIVATE_KEY_LVE_H_COMMAND obfuscate "${PRIVATE_KEY_LVE_H}" "${PRIVATE_KEY_LVE}" PRIVATE_KEY_LVE LVE_PRIVATE)

  add_custom_command(
    OUTPUT "${PRIVATE_KEY_LVE_H}"
    COMMAND ${PRIVATE_KEY_LVE_H_COMMAND}
    DEPENDS "${PRIVATE_KEY_LVE}" obfuscate
    COMMENT "Running: ${PRIVATE_KEY_LVE_H_COMMAND}"
    )
	
  set_source_files_properties("${PRIVATE_KEY_LVE_H}" PROPERTIES HEADER_FILE_ONLY TRUE)

endif()

# TOOLS public keys recognized by LVE
# -----------------------------------
set(PUBLIC_KEY_TOOL_LIST ${TOOLS_PUBLIC_KEYS_DIRECTORY}/public_key_tools.txt)

if("${TOOLS_PUBLIC_KEYS_DIRECTORY}" STREQUAL "" OR NOT EXISTS "${PUBLIC_KEY_TOOL_LIST}")

  message( FATAL_ERROR "\
Error: ${PUBLIC_KEY_TOOL_LIST} doesn't exist.
  variable TOOLS_PUBLIC_KEYS_DIRECTORY = ${TOOLS_PUBLIC_KEYS_DIRECTORY}
    passed to CMake as -DTOOLS_PUBLIC_KEYS_DIRECTORY=public_keys_path

  the TOOLS_PUBLIC_KEYS_DIRECTORY directory must contains:
  
    - the public keys for each of the tools supported by the generated LVE
	    to generate a set of keys run the following commands:
		
          openssl genrsa -out \"private_key_path/private_key_tool.pem\" 4096
          openssl rsa -pubout -in \"private_key_path/private_key_tool.pem\" -out \"public_keys_path/public_key_tool.pem\"
          dos2unix \"public_keys_path/public_key_tool.pem\"

	- the file 'public_key_tools.txt' which containst a list of the public keys to use
	    one public key file per line
 
  the same set of keys (and random.h) must be used in all builds or the generated LVEs will not be compatible
")

else()

  # Generate header files for tool authorized with this lve.
  file(STRINGS "${PUBLIC_KEY_TOOL_LIST}" PUBLIC_KEY_TOOL_FILES)
  list(LENGTH PUBLIC_KEY_TOOL_FILES PUBLIC_KEY_TOOL_NUM)
  
  if(NOT PUBLIC_KEY_TOOL_NUM)
    message( FATAL_ERROR "Error: no tool public keys in ${PUBLIC_KEY_TOOL_LIST}")  
  endif()
  
  set(TOOLI 0)
  foreach(KEYFILE ${PUBLIC_KEY_TOOL_FILES})
    set(PUBLIC_KEY_TOOLI_H "${CMAKE_CURRENT_BINARY_DIR}/public_key_tool${TOOLI}.h")
	set(PUBLIC_KEY_TOOLI_H_COMMAND obfuscate "${PUBLIC_KEY_TOOLI_H}" "${TOOLS_PUBLIC_KEYS_DIRECTORY}/${KEYFILE}" PUBLIC_KEY_TOOL${TOOLI} TOOL_PUBLIC)
	
    list(APPEND PUBLIC_KEY_TOOL_H_FILES "${PUBLIC_KEY_TOOLI_H}")
	
    message(STATUS "Adding rule for TOOLI=${TOOLI} KEYFILE=${KEYFILE}")
    add_custom_command(
        OUTPUT "${PUBLIC_KEY_TOOLI_H}"
        COMMAND ${PUBLIC_KEY_TOOLI_H_COMMAND}
        DEPENDS "${TOOLS_PUBLIC_KEYS_DIRECTORY}/${KEYFILE}" obfuscate
        COMMENT "Running: ${PUBLIC_KEY_TOOLI_H_COMMAND}"
    )
    set_source_files_properties("${PUBLIC_KEY_TOOLI_H}" PROPERTIES HEADER_FILE_ONLY TRUE)
    math(EXPR TOOLI ${TOOLI}+1)
  endforeach()

  set(PUBLIC_KEY_TOOL_H "${CMAKE_CURRENT_BINARY_DIR}/public_key_tool.h")
  message(STATUS "Adding rule for ${PUBLIC_KEY_TOOL_H}")
  
  add_custom_command(
    OUTPUT "${PUBLIC_KEY_TOOL_H}"
    COMMAND "${CMAKE_COMMAND}" 
        -D PUBLIC_KEY_TOOL_NUM=${PUBLIC_KEY_TOOL_NUM} 
        -D PUBLIC_KEY_TOOL_H="${PUBLIC_KEY_TOOL_H}" 
        -P "${CMAKE_CURRENT_LIST_DIR}/GenPublicKeyToolH.cmake"
    DEPENDS obfuscate "${CMAKE_CURRENT_LIST_DIR}/GenPublicKeyToolH.cmake" "${PUBLIC_KEY_TOOL_LIST}" ${PUBLIC_KEY_TOOL_H_FILES}
    )
  set_source_files_properties("${PUBLIC_KEY_TOOL_H}" PROPERTIES HEADER_FILE_ONLY TRUE)

endif()

ENABLE_TESTING()

# Modules
set(MLLE_LIST_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(MLLE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(MLLE_THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/../ThirdParty") # TODO: Need to be included?

# - License manager
if(NOT TEST_LICENSED_FEATURE)
    set(TEST_LICENSED_FEATURE test_licensed_feature)
endif()
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/license_managers ${CMAKE_CURRENT_BINARY_DIR}/license_managers)

# - Decryptor
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/decryptors ${CMAKE_CURRENT_BINARY_DIR}/decryptors)

if (USE_INSTALLED_OPENSSL)
elseif (USE_CUSTOM_OPENSSL_SUBDIRECTORY)
else()
    add_dependencies(decryptor  openssl)
endif()

# - Obfuscator
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/obfuscators ${CMAKE_CURRENT_BINARY_DIR}/obfuscators)

# --------------
# Create common library.
# --------------
add_library(mlle_common
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_error.c
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_io.c
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_parse_command.c
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_protocol.c
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_ssl.c
    ${CMAKE_CURRENT_LIST_DIR}/common/mlle_utils.c
    ${CMAKE_CURRENT_LIST_DIR}/common/libcrypto-compat.c
    ${CMAKE_CURRENT_LIST_DIR}/common/libcrypto-compat.h)

if (USE_INSTALLED_OPENSSL)
elseif (USE_CUSTOM_OPENSSL_SUBDIRECTORY)
    target_link_libraries(mlle_common ${ssl_libs})
else()
    add_dependencies(mlle_common  openssl)
endif()

# --------------
# Create lve.
# --------------
add_executable(${LVETARGET}
    ${PRIVATE_KEY_LVE_H}
    ${PUBLIC_KEY_TOOL_H}
    ${CMAKE_CURRENT_LIST_DIR}/lve/lve.c

    ${CMAKE_CURRENT_LIST_DIR}/../ThirdParty/uthash/uthash.h

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

target_link_libraries(${LVETARGET} license_manager mlle_common decryptor ${ssl_libs} ${extra_ssl_libs})
if(MSVC )
        # add NODEFAULTLIB:libcmt in Debug
        # into string
        set_target_properties(${LVETARGET} PROPERTIES LINK_FLAGS_Debug  "/NODEFAULTLIB:libcmt")
endif()

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
    ${CMAKE_CURRENT_LIST_DIR}/common/libcrypto-compat.c
    ${CMAKE_CURRENT_LIST_DIR}/common/libcrypto-compat.h
    ${tool_platform_c}
)

if (USE_CUSTOM_OPENSSL_SUBDIRECTORY)
    target_link_libraries(tool ${ssl_libs})
endif()

add_executable(test_tool
    ${PUBLIC_KEY_TOOL_H}
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_tool.c
)

target_link_libraries(test_tool tool obfuscate_utils ${ssl_libs} ${extra_ssl_libs})

file(COPY ${CMAKE_CURRENT_LIST_DIR}/tool/mlle_licensing.h
          ${CMAKE_CURRENT_LIST_DIR}/tool/mlle_types.h
          ${CMAKE_CURRENT_LIST_DIR}/common/mlle_error.h
     DESTINATION include)

# --------------------
# Create packagetool.
# --------------------

# preprocessor defines
if (DARWIN)
    set_property(
    SOURCE
    packagetool/utils.c
    APPEND
    PROPERTY COMPILE_DEFINITIONS
    DARWIN
    )
endif()

add_executable(packagetool
    ${CMAKE_CURRENT_LIST_DIR}/packagetool/packagetool.c
    ${CMAKE_CURRENT_LIST_DIR}/packagetool/arguments.c
    ${CMAKE_CURRENT_LIST_DIR}/packagetool/manifest.c
    ${CMAKE_CURRENT_LIST_DIR}/packagetool/utils.c
    ${CMAKE_CURRENT_LIST_DIR}/packagetool/utils.h
)

target_link_libraries(packagetool decryptor mlle_common ${ssl_libs} ${extra_ssl_libs})
# --------------------
# Create encrypt_file.
# --------------------
add_executable(encrypt_file
    ${CMAKE_CURRENT_LIST_DIR}/encrypt_decrypt/encrypt_file.c
    ${CMAKE_CURRENT_LIST_DIR}/common/libcrypto-compat.c
)
target_link_libraries(encrypt_file  decryptor mlle_common ${ssl_libs} ${extra_ssl_libs})

# --------------------
# Create decrypt_file.
# --------------------
add_executable(decrypt_file
    ${CMAKE_CURRENT_LIST_DIR}/encrypt_decrypt/decrypt_file.c
    ${CMAKE_CURRENT_LIST_DIR}/common/libcrypto-compat.c    
)
target_link_libraries(decrypt_file  decryptor mlle_common ${ssl_libs} ${extra_ssl_libs})

if(WIN32)
    if (MSVC)
        # Setting /SUBSYSTEM:WINDOWS to force build with WinMain and avoid command window pop-up at start
        set_target_properties(${LVETARGET} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")

        # ignore missing pdb warning for openssl
        set_target_properties(${LVETARGET} PROPERTIES LINK_FLAGS "/ignore:4099")
        set_target_properties(obfuscate PROPERTIES LINK_FLAGS "/ignore:4099")
        set_target_properties(randomize_key PROPERTIES LINK_FLAGS "/ignore:4099")
        set_target_properties(decrypt_file PROPERTIES LINK_FLAGS "/ignore:4099")
        set_target_properties(packagetool PROPERTIES LINK_FLAGS "/ignore:4099")
        set_target_properties(encrypt_file PROPERTIES LINK_FLAGS "/ignore:4099")
        set_target_properties(test_tool PROPERTIES LINK_FLAGS "/ignore:4099")
        set_target_properties(tool PROPERTIES LINK_FLAGS "/ignore:4099")
    endif (MSVC)
endif(WIN32)

add_custom_command( TARGET ${LVETARGET}
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E remove_directory $<TARGET_FILE_DIR:packagetool>/LVE
        COMMAND "${CMAKE_COMMAND}" -E make_directory $<TARGET_FILE_DIR:packagetool>/LVE
        COMMAND "${CMAKE_COMMAND}" -E touch ${CMAKE_CURRENT_BINARY_DIR}/test_library.mol
        COMMAND "${CMAKE_COMMAND}" -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/test_library.mol
        COMMAND "${CMAKE_COMMAND}" -E echo
        "Copying: $<TARGET_FILE:${LVETARGET}> to $<TARGET_FILE_DIR:packagetool>/LVE"
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:${LVETARGET}> $<TARGET_FILE_DIR:packagetool>/LVE/
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)

file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/test_library) 
file(COPY           ${CMAKE_CURRENT_LIST_DIR}/tests/test_library DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/test_facit) 
file(RENAME         ${CMAKE_CURRENT_BINARY_DIR}/test_library ${CMAKE_CURRENT_BINARY_DIR}/test_facit)

file(REMOVE         ${CMAKE_CURRENT_BINARY_DIR}/test_facit/package.in) 
configure_file(     ${CMAKE_CURRENT_LIST_DIR}/tests/test_library/package.in ${CMAKE_CURRENT_BINARY_DIR}/test_facit/package.mo  @ONLY NEWLINE_STYLE UNIX)

add_custom_target(test_library_mol
    DEPENDS
        packagetool
        ${LVETARGET}
        ${CMAKE_CURRENT_BINARY_DIR}/test_library.mol
        )

set(PACKAGE_TEST_LIB_CMD $<TARGET_FILE:packagetool>
        -librarypath ${CMAKE_CURRENT_BINARY_DIR}/test_library -version "2.0" -language "3.2" -encrypt "true")

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/test_library.mol
	
    COMMAND "${CMAKE_COMMAND}" -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/test_library.mol
    COMMAND "${CMAKE_COMMAND}" -E echo  Running: ${PACKAGE_TEST_LIB_CMD}
    COMMAND "${CMAKE_COMMAND}" -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/test_library
    COMMAND "${CMAKE_COMMAND}" -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/test_facit ${CMAKE_CURRENT_BINARY_DIR}/test_library
    COMMAND ${PACKAGE_TEST_LIB_CMD}
    COMMAND "${CMAKE_COMMAND}" -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/test_library
	
    # TODO: fix, dependencies are in ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/tests/test_library/binary.gif
        packagetool
        ${LVETARGET}
		
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

if(NOT LONG_FOLDER_NAME)
  set(LONG_FOLDER_NAME "X01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789/0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789")
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${LONG_FOLDER_NAME})
add_custom_target(test_library_long_path
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/test_library_long_path_marker
        )

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/test_library_long_path_marker
    DEPENDS 
        ${CMAKE_CURRENT_BINARY_DIR}/test_library/.library/manifest.xml
    COMMAND "${CMAKE_COMMAND}" -E echo "Copying test_library into folder with long name"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory test_library ${LONG_FOLDER_NAME}/test_library
    COMMAND "${CMAKE_COMMAND}" -E touch test_library_mol_marker
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
        )


add_custom_target(
    test_library
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/test_library/.library/manifest.xml
)

if(UNIX)
    set(CHMOD chmod +x)
else()
    set(CHMOD "${CMAKE_COMMAND}" -E echo )
endif()

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/test_library/.library/manifest.xml
    COMMAND "${CMAKE_COMMAND}" -E remove_directory test_library
    COMMAND "${CMAKE_COMMAND}" -E echo "Extracting from test_library.mol"
    COMMAND "${CMAKE_COMMAND}" -E tar xvz test_library.mol
    COMMAND "${CMAKE_COMMAND}" -E copy_directory test_library/Module test_library/${LONG_FOLDER_NAME}    
    COMMAND ${CHMOD} ${CMAKE_CURRENT_BINARY_DIR}/test_library/.library/${LVETARGET}

    DEPENDS
        test_library_mol
        packagetool
        ${LVETARGET}
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)

# --------------------
# POST_BUILD
# --------------------

if (USE_CUSTOM_OPENSSL_SUBDIRECTORY)
    # Ensure that OpenSSL DLLs can be found when running 'ctest' by copying them to the same directory as the executables.
    # Ensure that OpenSSL shared libraries can be found when packaging by copying them to the same directory as the executables.
    add_custom_command(TARGET packagetool POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            ${LIBSSL_IMPORTED_LOCATION}
            ${LIBCRYPTO_IMPORTED_LOCATION}
            $<TARGET_FILE_DIR:packagetool>
    )
    if (WIN32)
        add_custom_command(TARGET packagetool POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                ${LIBSSL_IMPORTED_IMPLIB}
                ${LIBCRYPTO_IMPORTED_IMPLIB}
                $<TARGET_FILE_DIR:packagetool>
        )
    endif()
endif()

# --------------------
# TEST
# --------------------
if(DEFINED TEST_NOT_LICENSED_FEATURE)
    set(TEST_NOT_LICENSED_FEATURE_OPTION --no-feature ${TEST_NOT_LICENSED_FEATURE})
endif()

file(MAKE_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_encrypt_files)
file(MAKE_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_decrypt_files)
file(MAKE_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_encrypt_files/Module)
file(MAKE_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/test_decrypt_files/Module)

if(NOT SKIP_SEMLA_SRC_TESTS)
    add_test( NAME encrypt_top_level_package
            COMMAND encrypt_file ${CMAKE_CURRENT_BINARY_DIR}/test_facit/package.mo package.moc ${CMAKE_CURRENT_BINARY_DIR}/test_encrypt_files)

    add_test( NAME encrypt_sub_package
            COMMAND encrypt_file ${CMAKE_CURRENT_LIST_DIR}/tests/test_library/Module/package.mo Module/package.moc ${CMAKE_CURRENT_BINARY_DIR}/test_encrypt_files)

    SET_TESTS_PROPERTIES ( encrypt_sub_package PROPERTIES DEPENDS encrypt_top_level_package)

    add_test( NAME encrypt_submodel
            COMMAND encrypt_file ${CMAKE_CURRENT_LIST_DIR}/tests/test_library/Module/testInPackage.mo Module/testInPackage.moc ${CMAKE_CURRENT_BINARY_DIR}/test_encrypt_files)

    SET_TESTS_PROPERTIES ( encrypt_submodel PROPERTIES DEPENDS encrypt_sub_package)

    add_test( NAME decrypt_top_level_package
            COMMAND decrypt_file  ${CMAKE_CURRENT_BINARY_DIR}/test_encrypt_files package.moc ${CMAKE_CURRENT_BINARY_DIR}/test_decrypt_files/package.mo)
    SET_TESTS_PROPERTIES (
        decrypt_top_level_package PROPERTIES DEPENDS encrypt_top_level_package)

    add_test( NAME decrypt_sub_package
            COMMAND decrypt_file  ${CMAKE_CURRENT_BINARY_DIR}/test_encrypt_files Module/package.moc ${CMAKE_CURRENT_BINARY_DIR}/test_decrypt_files/Module/package.mo)
    SET_TESTS_PROPERTIES (
        decrypt_sub_package PROPERTIES DEPENDS encrypt_sub_package)

    add_test( NAME decrypt_submodel
            COMMAND decrypt_file  ${CMAKE_CURRENT_BINARY_DIR}/test_encrypt_files Module/testInPackage.moc ${CMAKE_CURRENT_BINARY_DIR}/test_decrypt_files/Module/testInPackage.mo)
    SET_TESTS_PROPERTIES (
        decrypt_submodel PROPERTIES DEPENDS encrypt_submodel)

    add_test( NAME run_test_tool COMMAND test_tool --lve ${LVETARGET} --feature ${TEST_LICENSED_FEATURE} ${TEST_NOT_LICENSED_FEATURE_OPTION}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
            
    add_test(NAME package_test_lib
        COMMAND "${CMAKE_COMMAND}" --build . --target test_library --config $<CONFIGURATION>
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


    SET_TESTS_PROPERTIES (run_test_tool PROPERTIES DEPENDS package_test_lib)

    if(DO_LONG_FILENAME_TEST OR UNIX)
        add_test( NAME run_test_tool_single_file 
                COMMAND test_tool --lve ${LVETARGET}  --feature ${TEST_LICENSED_FEATURE} ${TEST_NOT_LICENSED_FEATURE_OPTION}
                        --libpath ${CMAKE_CURRENT_BINARY_DIR}/test_library 
                        --file 
                        Module/testLongName_01_567890_02_567890_03_567890_04_567890_05_567890_06_567890_07_567890_08_567890_09_567890_10_567890_11_567890_12_567890_13_567890_14_567890_15_567890.moc 
                        Module/testLongName_01_567890_02_567890_03_567890_04_567890_05_567890_06_567890_07_567890_08_567890_09_567890_10_567890_11_567890_12_567890_13_567890_14_567890_15_567890.mo
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

        SET_TESTS_PROPERTIES (
            run_test_tool_single_file PROPERTIES DEPENDS package_test_lib)

        add_test( NAME run_test_tool_long_path 
                COMMAND test_tool --lve ${LVETARGET} --feature ${TEST_LICENSED_FEATURE} ${TEST_NOT_LICENSED_FEATURE_OPTION}
                    --libpath ${CMAKE_CURRENT_BINARY_DIR}/${LONG_FOLDER_NAME}/test_library
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

        add_test(NAME package_test_lib_long_path
            COMMAND "${CMAKE_COMMAND}" --build . --target test_library_long_path --config $<CONFIGURATION>
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
            
        SET_TESTS_PROPERTIES (
            run_test_tool_long_path PROPERTIES DEPENDS package_test_lib_long_path)
    else()
        file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/test_facit/Module/testLongName_01_567890_02_567890_03_567890_04_567890_05_567890_06_567890_07_567890_08_567890_09_567890_10_567890_11_567890_12_567890_13_567890_14_567890_15_567890.mo) 
    endif()
endif()

# --------------------
# INSTALL
# --------------------
# TODO: Install does not work as intended - needs work or maybe it should just be removed?
install(TARGETS test_tool encrypt_file packagetool decrypt_file
        DESTINATION bin
        ARCHIVE DESTINATION lib)

install(TARGETS ${LVETARGET} DESTINATION bin/LVE)

# Install header files needed when using tool library
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
        DESTINATION include)

install(TARGETS tool DESTINATION lib)
