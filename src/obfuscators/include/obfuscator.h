/*
    Copyright (C) 2022 Modelica Association
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

#ifndef _OBFUSCATOR_H_
#define _OBFUSCATOR_H_

#include <stdio.h>


typedef enum { 
    ENCRYPT, 
    TOOL_PUBLIC, 
    TOOL_PRIVATE, 
    LVE_PRIVATE 
} key_type;


void write_initialize_key_macro_body(FILE* out, 
                                     key_type type, 
                                     char* buffer_name, 
                                     unsigned char* data, 
                                     size_t data_len);


void write_header_extra(FILE* out, 
                        key_type type);


#endif /* _OBFUSCATOR_H_ */
