#
#    Copyright (C) 2017 Modelon AB
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

cmake_minimum_required(VERSION 3.20)
project(DummyObfuscator)

# Add all required source files to library "obfuscator_module"
add_library(obfuscator_module 
            obfuscator.c
)
include_directories(${MLLE_LIST_DIR}/obfuscators/include)
