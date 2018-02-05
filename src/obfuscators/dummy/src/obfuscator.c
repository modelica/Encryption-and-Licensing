/*
    Copyright (C) 2017 Modelon AB

    This program is free software: you can redistribute it and/or modify
    it under the terms of the BSD style license.

     This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    BSD_License.txt file for more details.

    You should have received a copy of the BSD_License.txt file
    along with this program. If not, contact Modelon AB <http://www.modelon.com>.
*/

#include "obfuscator.h"

#include <stdio.h>

void write_initialize_key_macro_body(FILE* out, 
                                     key_type type, 
                                     char* buffer_name, 
                                     unsigned char* data, 
                                     size_t data_len) {
    int i;
    
    for (i = 0; i < data_len; i++) {
        fprintf(out, "%s[%u] = '\\x%x'; ", buffer_name, i, (unsigned int) data[i]);
    }
}

void write_header_extra(FILE* out, 
                        key_type type) {
}
