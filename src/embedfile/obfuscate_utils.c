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
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "mlle_portability.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "obfuscate_utils.h"

/***********************************
 * Open a file or exit on error.
 **********************************/
FILE* open_or_exit(const char* fname, const char* mode)
{
    FILE* f = fopen(fname, mode);

    if (f == NULL)
    {
        fprintf(stderr, "Could not open file '%s' in mode '%s'.", fname, mode);
        exit(1);
    }

    return f;
}


#ifdef WIN32
#define SIZE_T_FORMAT "%Iu"
#else
#define SIZE_T_FORMAT "%zu"
#endif

/****************************************************
 * Create a h-file with macros for accessing the key.
 *
 * Parameters:
 *      name     - the name of the array variable to use.
 *      data     - array with the data to encode.
 *      len      - length of data.
 *      filename - name of the h file.
 ***************************************************/
void create_header_file(char* name, key_type type, unsigned char *data, size_t len, char *filename)
{
    size_t i = 0, j = 0;
    FILE *out = NULL;

    /* Remove \r from the string to support windows generated pem files */
    for (; i < len; ++i) {
        if (data[i] == '\r')
            continue;
        if (i != j)
            data[j] = data[i];
        ++j;
    }
    if (j < len)
        data[j] = 0;

    out = open_or_exit(filename, "wb");

    fprintf(out, "#define DECLARE_%s() unsigned char %s[" SIZE_T_FORMAT "]\n", name, name, len + 1);

    fprintf(out, "#define %s_LEN (%d)\n", name, j);

    fprintf(out, "#define INITIALIZE_%s() do { ", name, name);
    write_initialize_key_macro_body(out, type, name, data, len);
    fprintf(out, "%s[%d] = '\\0'; } while (0)\n", name, len);

    fprintf(out, "#define CLEAR_%s() memset(%s, 0, %s_LEN)\n", name, name, name);

    write_header_extra(out, type);

    fclose(out);
}
