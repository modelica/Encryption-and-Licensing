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

#include "arguments.h"

/************************************
 * Create the manifest file.
 *
 * Returns:
 *      1 - file was created.
 *      0 - failed to create file.
 ***********************************/
int createManifestFile();

/***************************
 * Add header to the file.
 **************************/
void addHeader();


/****************************************
 * Add library information to the file.
 ***************************************/
void addLibraryHeader();


/******************************************************
 * Add path to icon file.
 * This path is relative to the top-level directory
 * of the library.
 *****************************************************/
void addIcon();


/********************************************
 * Add LVE's needed to decrypt the library.
 *******************************************/
void addEncryption();

/**************************************************************
 * This method add tools that the library is compatible with.
 * These tools are read from an xml file and pasted into
 * the manifest.xml file.
 *************************************************************/
void addTools();

/****************************************************************
 * This method add libraries that library depends on.
 * These dependencies are read from an xml file and pasted into
 * the manifest.xml file.
 ***************************************************************/
void addDependencies();

/*******************************
 * Add the footer to the file.
 ******************************/
void addFooter();

/************************************************
 * Extract platform from the filename.
 *
 * Parameters:
 *      filename - the string to extract from.
 *
 * Returns:
 *      Extracted string or 0 if failed.
 ***********************************************/
char *extractPlatform(char *filename);

/****************************************************************
 * Copy everything from an xlm-file apart from the first line
 * and paste it into the manifest.xml file.
 *
 * Parameters:
 *      path - path, including filename, to the xml-file.
 ***************************************************************/
void copyFileInformation(char *path);
