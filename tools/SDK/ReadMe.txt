#    Copyright (C) 2015 - 2022 Modelon AB, 2022 Modelica Association
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

SDK for Library encryption
==========================

This folder contains a NSIS script file that puts together a installer file (Windows) for 
the Library Encryption project.

It also contains a installer for the applications below. 
The installer contains the 64-bit version of Strawberry Perl.

The installer contains:
    - cmake-3.3.0-rc4-win32-x86
    - nasm-2.11.08-win32
    - strawberry-perl-5.22.0.1-64bit
    
If you need Strawberry Perl 32-bits, then you have to download strawberry-perl-5.22.0.1-32bit
and change the script and recompile it.    

Strawberry Perl contains a installer of its own which will appear during the installation.
The installer will not set the environment variable PATH to the bin-folder for Strawberry Perl 
so you must do this yourself.


Compile the SDK
===============

If you need to recompile or make changes to the installer, you can do the following.

* Download and install NSIS from http://nsis.sourceforge.net/Main_Page.

* Create a folder where you copy the installer script to. 

* Default folder in the script is: C:\SDK_LibraryEncryption so you might want to change this.

* In the script folder, create a folder called applications where you download and put 
  the files mentioned above. If you put the files in another folder, make sure you change the path 
  in the script.

* Right click on the script and choose "Compile NSIS Script" in the menu to create the installer.

* And hopefully you will now have a installer.


