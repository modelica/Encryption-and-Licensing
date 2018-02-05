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


/*****************************************************************
 * This file handles validation and extraction of arguments and
 * its values and store them for later use when the manifest.xlm
 * file is created.
 ****************************************************************/

#include "arguments.h"
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include <ctype.h>


// -------------------------------------------------------------------------
// This array contains the arguments that can be used on the command line.
// -------------------------------------------------------------------------
char *validArguments[MAX_ARGUMENTS] = {
    ARGUMENT_LIBRARY_PATH, 
    ARGUMENT_ENABLED, 
    ARGUMENT_TITLE, 
    ARGUMENT_DESCRIPTION,
    ARGUMENT_LIBRARY_VERSION, 
    ARGUMENT_BUILD_NUMBER, 
    ARGUMENT_BUILD_DATE, 
    ARGUMENT_LANGUAGE_VERSION, 
    ARGUMENT_COPYRIGHT, 
    ARGUMENT_LICENSE, 
    ARGUMENT_ENCRYPT, 
    ARGUMENT_ICON_PATH, 
    ARGUMENT_TOOLS_FILE, 
    ARGUMENT_DEPENDENCIES_FILE
};


// -------------------------------------------------------------------------------
// This array contains the mandatory arguments that must be on the command line.
// -------------------------------------------------------------------------------
char *mandatoryArguments[NO_OF_MANDATORY_ARGUMENTS] = {
    ARGUMENT_LIBRARY_PATH, 
    ARGUMENT_LIBRARY_VERSION, 
    ARGUMENT_LANGUAGE_VERSION
};

// -----------------------------------------------------------------------
// This array contains the arguments and its value from the command line.
// -----------------------------------------------------------------------
char *manifestData[2][MAX_ARGUMENTS];




/***********************************
 * Get the value from an argument.
 **********************************/
char *getValueOf(const char *key)
{
    int i;

    for (i = 0; i < MAX_ARGUMENTS; ++i)
    {
        if (strcmp(manifestData[0][i], key) == 0)
        {
            return manifestData[1][i];
        }
    }

    return 0;
}

/********************************
 * Check if an argument exists.
 *******************************/
int containsKey(char *key)
{
    int i;

    for (i = 0; i < MAX_ARGUMENTS; ++i)
    {
        if (strcmp(manifestData[0][i], key) == 0)
        {
            return 1;
        }
    }

    return 0;
}


/********************************************************
 * Compare an argument against a list of valid arguments.
 * Only arguments thats in the list are valid.
 *
 * Parameter:
 *      argument - the argument to lookup.
 *
 * Returns:
 *      1 - the argument is valid.
 *      0 - the argument is not valid.
 *******************************************************/
int lookupArgument(char *argument)
{
    int i;

    for (i = 0; i < MAX_ARGUMENTS; ++i)
    {
        if (strcmp(argument, validArguments[i]) == 0)
        {
            // It's a valid argument.
            return 1;
        }
    }

    return 0;
}

/**********************************************************
 * Add an argument and its value to the manifest array.
 *
 * Parameters:
 *      argument - the argument to add.
 *      value - the value of the argument.
 *********************************************************/
void addManifestData(char *argument, char *value)
{
    int index;

    for (index = 0; index < MAX_ARGUMENTS; index++)
    {
        // Find an empty index.
        if (strcmp(manifestData[0][index], "") == 0)
        {
            break;
        }
    }

    if (index < MAX_ARGUMENTS)
    {
        manifestData[0][index] = argument;
        manifestData[1][index] = value;
    }
}


/**************************************************
 * Print out the argument array and it's values.
 * Not in use but can be helpful when debugging.
 *************************************************/
void printArray()
{
    int i;

    printf("\n\n======================================\n");
    printf("MAX_ARGUMENTS = %d\n\n", MAX_ARGUMENTS);

    for (i = 0; i < MAX_ARGUMENTS; ++i)
    {

        printf("%s - %s\n",manifestData[0][i], manifestData[1][i] );
    }

    printf("======================================\n");
}

/*********************************************************
 * Clear the array with data read from the command line.
 ********************************************************/
void clearManifestArray()
{
    int i;

    // Clear array.
    for (i = 0; i < MAX_ARGUMENTS; i++)
    {
        manifestData[0][i] = "";
        manifestData[1][i] = "";
    }
}


/*************************************************
 * Validate the arguments from the command line.
 ************************************************/
int validateArguments(int noOfArguments, char **arguments)
{
    int i;
    char *ptr;
    clearManifestArray();

    // Iterate over all arguments.
    for (i = 1; i < noOfArguments; i++)
    {
        ptr = arguments[i];
        ++ptr;

        // Is the argument valid.
        if (ptr && *ptr && lookupArgument(ptr))
        {
            // Safety check. If user trying to add an argument twice.
            if (containsKey(ptr))
            {
                printf("Error: Can not add argument %s twice.\n", ptr);
                return 0;
            }
            // Does the argument have a value.
            if ( (i < (noOfArguments - 1)) && (strcmp(arguments[++i], "") != 0) )
            {
                // Store argument and value.
                addManifestData(ptr, arguments[i]);
            }
            else
            {
                // Abort if argument is missing value.
                printf("Error: Argument %s is missing a value.\n", ptr);
                return 0;
            }

            ptr = NULL;
            continue;
        }
        else
        {
            // Abort if argument is invalid.
            printf("Error: Argument %s is not valid.\n", arguments[i]);
            return 0;
        }
    }

    return 1;
}



/************************************************
 * Validate the path to the top-level library.
 ***********************************************/
int validateLibraryPath()
{
    char *value;

    // Get the library path.
    if ( (value = getValueOf(ARGUMENT_LIBRARY_PATH)) )
    {
        if (!validatePath(value))
        {
            return 0;
        }
    }
    else
    {
        printf("Error: No path for the top-level library exists.");
        return 0;
    }

    return 1;
}

/**************************************
 * Validate the mandatory arguments.
 *************************************/
int validateMandatoryArguments()
{
    int i = 0;

    for (i = 0; i < NO_OF_MANDATORY_ARGUMENTS; ++i)
    {

        if (!containsKey(mandatoryArguments[i]))
        {
            printf("Mandatory argument %s is missing.\n", mandatoryArguments[i]);
            return 0;
        }
    }

    return 1;
}

/************************************************
 * Check if any encryption executables exists.
 ***********************************************/
int validateEncryption()
{
    char *value;

    // Is encryption an argument.
    if (containsKey(ARGUMENT_ENCRYPT))
    {
        value = getValueOf(ARGUMENT_ENCRYPT);
        {
            // Should we use encryption.
            if (strcmp(stringToLower(value), "true") == 0)
            {
                // Count number of LVE's.
                if (countLVE() == 0)
                {
                    printf("No LVE was found. Encryption not possible.\n");
                    return 0;
                }
            }
        }
    }

    return 1;
}


/***************************************
 * Validate the path to an xml file.
 ***************************************/
int validateXmlFile(char *argument)
{
    char *value;

    // Is this argument given?
    if (containsKey(argument))
    {
        // Get the value of the argument - the path to the XML file.
        value = getValueOf(argument);

        if (!fileExists(value))
        {
            // Abort if file is missing.
            printf("Xml-file \"%s\" was not found.\n", value);
            return 0;
        }
    }
    return 1;
}


/*********************************************************
 * Try to locate the icon file in the library structure.
 ********************************************************/
int validateIcon()
{
    char *value = NULL;
    int result = 0;

    // Is icon an argument.
    if (containsKey(ARGUMENT_ICON_PATH))
    {
        // Get the icons path and file name.
        value = getValueOf(ARGUMENT_ICON_PATH);

        // Check if the icon exist.
        if (fileExists(value))
        {
            return 1;
            /*
            // Can we find this icon file somewhere in the top level library.
            result = locateIconFile(value, getValueOf(ARGUMENT_LIBRARY_PATH));

            if (result == 0)
            {
                printf("Error: The icon file %s does not exits anywhere in folder %s.\n", 
                    value, getValueOf(ARGUMENT_LIBRARY_PATH));
                return 0;
            }
            */
        }
        else
        {
            printf("Icon file %s doesn't exist.\n", value);
            return 0;
        }
    }

    return 1;
}


/****************************
 * Get the library number.
 ***************************/
char *getLibraryName()
{
    char *libPath = NULL;
    char *number = NULL;
    char *number2 = NULL;

    if ( (libPath = getValueOf(ARGUMENT_LIBRARY_PATH)) != NULL)
    {
        number = strrchr(libPath, '/');
#ifdef WIN32
        number2 = strrchr(libPath, '\\');
        if (number == NULL || (number2 != NULL && number2 > number)) {
            number = number2;
        }
#else
        // Remove last character if the string ends 
        // with a '/, like in '/home/Modelica/'.
        if (libPath[(strlen(libPath)-1)] == '/')
        {
            libPath[(strlen(libPath)-1)] = '\0';
        }
#endif
        // We only get result here if we are using a path with more
        // than one folder. Otherwise number will be null.
        if (number != NULL)
        {
            // Step forward to get past '\' or '/',
            ++number;
        }
        else
        {
            number = libPath;
        }
    }

    return number;
}

