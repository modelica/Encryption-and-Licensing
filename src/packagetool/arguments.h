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

#include <stdio.h>
#include <stddef.h>

#define NO_OF_MANDATORY_ARGUMENTS 3
#define NO_HELP_ARGUMENTS         2
#define MAX_ARGUMENTS             14

#define ARGUMENT_LIBRARY_PATH      "librarypath"
#define ARGUMENT_ENABLED           "enabled"
#define ARGUMENT_TITLE             "title"
#define ARGUMENT_DESCRIPTION       "description"
#define ARGUMENT_LIBRARY_VERSION   "version"
#define ARGUMENT_BUILD_NUMBER      "build"
#define ARGUMENT_BUILD_DATE        "date"
#define ARGUMENT_LANGUAGE_VERSION  "language"
#define ARGUMENT_COPYRIGHT         "copyright"
#define ARGUMENT_LICENSE           "license"
#define ARGUMENT_ENCRYPT           "encrypt"
#define ARGUMENT_ICON_PATH         "icon"
#define ARGUMENT_TOOLS_FILE        "tools"
#define ARGUMENT_DEPENDENCIES_FILE "dependencies"
#define ARGUMENT_HELP              "--help"
#define ARGUMENT_SHORT_HELP        "-h"


/**************************************************
 * Print out the argument array and it's values.
 * Not in use but can be helpful when debugging.
 *************************************************/
void printArray();

/*****************************************
 * Check if an argument exists.
 *
 * Parameters:
 *      key - the argument to look for.
 *
 * Returns:
 *      1 - the argument exists.
 *      0 - the argument didn't exist.
 ***************************************/
int containsKey(char *key);


/****************************************************************
 * Validate the arguments from the command line.
 * This method aborts if an argument is not valid or if an
 * argument is missing a value.
 *
 * Parameters:
 *      noOfArguments - number of arguments from command line.
 *      arguments - arguments from command line.
 *
 * Returns:
 *      1 - validation was successful.
 *      0 - validation failed.
 ***************************************************************/
int validateArguments(int noOfArguments, char **arguments);


/******************************************************************
 * Get the value for an argument.
 *
 * Parameters:
 *      key - the argument whose value we want.
 *
 * Returns:
 *      The value of the argument or NULL if argument doesn't
 *      have a value or if the argument doesn't exist.
 *****************************************************************/
char *getValueOf(const char *key);

/*********************************************************
 * Validate the mandatory arguments.
 *
 * Returns:
 *      1 - every mandatory argument is accounted for.
 *      0 - one or more mandatory argument is missing.
 ********************************************************/
int validateMandatoryArguments();

/**********************************************************
 * Validate the path to the top-level library.
 * This path must exist otherwise we can't do anything.
 *
 * Returns:
 *      1 - the library path exists and it's a directory.
 *      0 - path doesn't exist or is not a directory.
 ***********************************************************/
int validateLibraryPath();

/**************************************************************
 * Check if any encryption executables exists.
 * If the encrypt arguments is used, executables must exist
 * to handle decryption later on.
 * For now, only four types of executables exists.
 *      - "lve_win32.exe"
 *      - "lve_win64.exe"
 *      - "lve_linux32.exe"
 *      - "lve_linux64.exe"
 * These files must reside in folder LVE relative to where
 * this program is executed from.
 *
 * Returns:
 *      1 or higher - some LVE was found OR encrypt argument
 *                    not used.
 *      0 - encrypt argument used and no LVE was found.
 *************************************************************/
int validateEncryption();


/***************************************
 * Validate the path to an xml file.
 *
 * Returns:
 *      1 - the path and file exists.
 *      0 - failed to find the file.
 ***************************************/
int validateXmlFile(char *key);


/************************************************************
 * Try to locate the icon file in the top-level directory.
 *
 * Returns:
 *      1 - the icon file was located.
 *      0 - failed to locate icon file.
 ***********************************************************/
int validateIcon();



/*******************************************************
 * Get the library name.
 * The library name is the last folder name in the
 * library path.
 *
 * Returns:
 *      Library name on success, NULL otherwise.
 ******************************************************/
char *getLibraryName();

/****************************************************************
 * Print argument help information to output.
 *
 ***************************************************************/
void printArgumentHelp();

/****************************************************************
 * Check if the user requested help info to be printed.
 *
 ***************************************************************/
int helpRequested(int noOfArguments, char **arguments);
