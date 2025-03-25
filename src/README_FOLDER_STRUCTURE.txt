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

Library Encryption - Overview of the folder structure
=====================================================


Public
    |-----src
        |
        |-----> decryptors (modules to decrypt encrypted .mo files)
            |
            |----> include (interface (header file) for definition of a decryptor module)
        |
        |-----> embedfile  (convert PEM files (ssl-keys) to c-file and generate random encrypt/decrypt key.)
            |
            |-----> obfuscators (modules to obfuscate cryptographic keys to make them harder to extract from LVE or tool)
                |
                |----> include (interface (header file) for definition of an obfuscator module)
        |
        |-----> encrypt_decrypt (generic encryption/decryption files.)
        |
        |-----> licens_managers (modules to license library)
            |
            |----> include (interface (header file) for definition of license_managers)
        |
        |-----> openssl_keys (folder for ssl-keys. Can be empty.)
        |
        |-----> packagetool (Command line tool for packaging a Modelica file structure into a zipped file)
        |
        |-----> tests (Test files using the "Check - Unit testing framework for C") 
        |
    |-----tools (some scripts)
        |
        |-----> SDK (NSIS installer for necessary tools on Windows)


In addition to the Public folder you should create your own Proprietary folder with
the following structure:

Proprietary
    |-----src
       |
       |----> decryptors (decryptor for encrypted files should be placed in this folder and 
                          it should follow the interface in Public/src/decryptors/include
                          optional - the default decryptor module should be sufficient for 
                          most applications)
       |----> license_managers (license manager modules should be placed in this folder and it 
                          should follow the interface in Public/src/license_managers/include)
       |----> obfuscators (obfuscator modules should be placed in this folder and it 
                          should follow the interface in Public/src/obfuscators/include)
       |----> openssl_keys (folder for ssl-keys. Can be empty.)
       CMakeLists.txt
       
The CMakeLists.txt needs to be provided and should include the CMakeLists from Public/src. It should also
set the EMBEDFILE_FOLDER to the path to embedfile. Note that DECRYPTOR and LICENSE_MANAGER are intended to
be defined on command line.
