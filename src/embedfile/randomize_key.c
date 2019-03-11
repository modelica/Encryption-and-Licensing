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

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "obfuscate_utils.h"
#include "obfuscator.h"
#include "mlle_cr_crypt.h"
#include "mlle_portability.h"
#include <openssl/rand.h>

char *generate_random_bytes();

/***************************************************************************
/*
 * This program generates (and if implemented obfuscates) an array of cryptographically 
 * secure random bytes to be used as an encryption key.
 *
 * Usage:
 *   randomize_key <output file> <name of variable>
 *
 * Example:
 *   % randomize_key random_key.h MLLE_CR_KEY
 *
 * The example above will generate:
 *   random_key.h
 *
 * The macros in the generated h-file is can be used in a function to access 
 * the key as <name of variable> with size <name of variable>_LEN (32) bytes.
 *
 * Usage in a c-file (assuming arguments above):
 *   #import "random_key.h"
 *   DECLARE_MLLE_CR_KEY();
 *   INITIALIZE_MLLE_CR_KEY();
 *   <use key in MLLE_CR_KEY with length MLLE_CR_KEY_LEN>
 *   CLEAR_MLLE_CR_KEY();
 *
 * The generated h-file does not include any ifndef/define construct - 
 * do not include in a header file.
 ***************************************************************************/
int main(int argc, char **argv)
{
    char *output_filename = NULL;
    char *name = NULL;
    unsigned char *data = NULL;

    if (argc != 3) {
        fprintf(stderr,
                "Usage: randomize_key <output file> <name of variable>\n");
        return EXIT_FAILURE;
    }
    
    output_filename = argv[1];
    name = argv[2];
    
    // Get the random bytes.
    data = generate_random_bytes(MLLE_CR_KEY_LENGTH);
    if (data == NULL)
    {
        return EXIT_FAILURE;
    }
    
    // Create the h-file.
    create_header_file(name, ENCRYPT, data, MLLE_CR_KEY_LENGTH, output_filename);

    free(data);

    return 0;
}


/********************************************************
 * Generate an array of len cryptographically 
 * secure random bytes using SSL.
 *
 * Returns:
 *      An array with random data, or NULL if it fails to
 *      generate data.
 *******************************************************/
char *generate_random_bytes(int len)
{
    char *data = calloc(len, sizeof(char));
    
    // Generate random bytes.
    if (RAND_bytes(data, len) < 1)
    {
        fprintf(stderr, "Failed to randomize bytes.\n");
        free(data);
        return NULL;
    }
    
    return data;
}
