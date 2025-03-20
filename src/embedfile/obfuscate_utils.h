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

#ifndef OBFUSCATE_UTILS_H_
#define OBFUSCATE_UTILS_H_

#include "obfuscator.h"

/***********************************************
 * Open a file or exit on error.
 *
 * Parameters:
 *      fname - name of file to open.
 *      mode - attribute to open file with.
 *
 * Returns:
 *      A FILE pointer.
 **********************************************/
FILE* open_or_exit(const char* fname, const char* mode);


/****************************************************
 * Create a h-file with the relevant macros.
 *
 * Parameters:
 *      name     - the name of the array variable to use.
 *      data     - array with the data to encode.
 *      len      - length of data.
 *      filename - name of the h file.
 ***************************************************/
void create_header_file(char* name, key_type type, unsigned char *data, size_t len, char *filename);


#endif /* OBFUSCATE_UTILS_H_ */
