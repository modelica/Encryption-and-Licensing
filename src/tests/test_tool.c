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

// Disable "deprecated" warning.
#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _XOPEN_SOURCE 700
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "mlle_licensing.h"
#include "mlle_portability.h"
#include "mlle_ssl.h"
#include "mlle_types.h"

#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/x509v3.h>
#include <openssl/evp.h>
#include <openssl/rand.h>


#define N_TEST_FILES 3
const char* FILES[N_TEST_FILES] = { "test.mo", "test.moc", "binary.gif" };
const char* CORRECT_FILES[N_TEST_FILES] = { "test.mo", "test.mo", "binary.gif" };


void check(int success,
           struct mlle_error *error)
{
    if (success) {
        puts("Success!");
    } else if (error) {
        puts(mlle_error_get_message(error));
    } else {
        puts("Failure without error message!");
    }
}


void get_file_and_compare(const char* get_file,
                         const char* correct_file,
                         const char* lib_path,
                         struct mlle_connections *lve,
                         struct mlle_error **error)
{
    struct mlle_file_contents *file = NULL;
    FILE* correct;
    size_t size_file = 0;
    size_t size_correct = 0;
    char *file_buf = NULL;
    char *correct_buf = NULL;
    char correct_path[PATH_MAX+1];
    int equals;
    int i = 0;

    printf("\nGet file(\"%s\")\n", get_file);
    file = mlle_tool_file(lve, get_file, error);
    check(file != NULL, *error);
    if (file) {
        // Read contents from LVE
        size_file = mlle_tool_get_file_size(file);
        file_buf = malloc(size_file);
        mlle_tool_read_bytes(file, file_buf, size_file);
        mlle_file_contents_free(&file);

        // Read correct file
        snprintf(correct_path, PATH_MAX, "%s/%s", lib_path, correct_file);
        correct = fopen(correct_path, "rb");
        if (correct == NULL) {
            printf("Could not open file \"%s\" for comparison.\n", correct_path);
            return;
        }
        fseek(correct, 0, SEEK_END);
        size_correct = ftell(correct);
        correct_buf = malloc(size_correct);
        fseek(correct, 0, SEEK_SET);
        fread(correct_buf, 1, size_correct, correct);
        fclose(correct);

        // Compare contents
        // TODO: more info on errors
        equals = size_file == size_correct;
        for (i = 0; equals && (size_t) i < size_file; i++) {
            equals = file_buf[i] == correct_buf[i];
        }
        if (equals) {
            puts("File contents match.\n");
        } else {
            puts("Failure: file contents do not match!\n");
        }

        free(file_buf);
        free(correct_buf);
    }
    mlle_error_free(error);
}


// Try to run the commands using the methods in licensing.
int tool_run_command_methods(struct mlle_connections *lve,
        struct mlle_error **error)
{
    int success = 0;
    int i = 0;
    char *buf = NULL;
    struct mlle_file_contents *file = NULL;
    size_t size = 0;
    size_t noOfBytes = 0;
    char libPath[1024] = {'\0'};
	
	//FILE *fp = NULL;


    if (getcwd(libPath, sizeof(libPath) - 10) != NULL)
    {
        strcat(libPath, "/testfiles");
    }
   

    // -----------------------------
    // Send command VERSION to LVE.
    // -----------------------------
    printf("\nSend command VERSION.\n");
    success = mlle_tool_version(lve, 1, 6, error);
    check(success, *error);
    mlle_error_free(error);

    // -------------------------
    // Send command LIB to LVE.
    // -------------------------
    printf("\nSend command LIB.\n");
    success = mlle_tool_libpath(lve, libPath, error);
    check(success, *error);
    mlle_error_free(error);

    // -----------------------------
    // Send command FEATURE to LVE.
    // -----------------------------
    printf("\nSend command FEATURE.\n");
    success = mlle_tool_feature(lve, "is_licensed", error);
    check(success, *error);
    mlle_error_free(error);
   
	
    // ---------------------------
    // Retrieve files from LVE
    // ---------------------------
    for (i = 0; i < N_TEST_FILES; i++) {
        get_file_and_compare(FILES[i], CORRECT_FILES[i], libPath, lve, error);
    }


    // -----------------------------------
    // Send command FEATURE again to LVE.
    // -----------------------------------
    printf("\nSend command FEATURE.\n");
    success = mlle_tool_feature(lve, "MATLAB", error);
    check(success, *error);
    mlle_error_free(error);


    return 1;
}

int
main(void)
{
    struct mlle_connections *lve = NULL;
    struct mlle_error *error = NULL;
	
    // Startup LVE, setup SSL and start communication.
    puts("mlle_start_executable()");
    lve = mlle_start_executable("./lve", &error);
    check(lve != NULL, error);
    mlle_error_free(&error);

    if (lve) {
        // Start sending commands to LVE.
        tool_run_command_methods(lve, &error);

        // Close connections.
        mlle_connections_free(&lve);
    }

    return 0;
}
