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

/*************************************************************
 * This file creates a manifest.xml file from the arguments
 * that was read from the command line.
 ************************************************************/

// Disable "deprecated" warning.
#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <string.h>
#include <stdio.h>
#include "manifest.h"
#include "arguments.h"
#include "utils.h"
#include "mlle_portability.h"

// The output file.
FILE *fd;
/* Current indentation level in XML output. */
static int xml_indent;
/* Current XML generation state, true means we are adding attributes (i.e. ending ">" not printed yet). */
static int xml_in_attributes;
/* Current XML generation state, true means we are have just printed a newline. */
static int xml_on_new_line;

#define XML_INDENT_STR "\t"


/******************************************************************
 * Print a string to the output file, with all XML reserved chars
 * encoded as XML entities. Helper function for xml_* functions.
 *****************************************************************/ 
static void print_escaped_text(const char* text)
{
    int i;
    size_t len;
    
    for (i = 0, len = strlen(text); i < len; i++) {
        switch (text[i]) {
        case '<':
            fprintf(fd, "&lt;");
            break;
        case '>':
            fprintf(fd, "&gt;");
            break;
        case '"':
            fprintf(fd, "&quot;");
            break;
        case '&':
            fprintf(fd, "&amp;");
            break;
        default:
            fprintf(fd, "%c", text[i]);
            break;
        }
    }
}


/******************************************************************
 * Print current XML indent. Helper function for xml_* functions.
 *****************************************************************/ 
static void add_xml_indent()
{
    int i;
    if (xml_on_new_line) {
        for (i = 0; i < xml_indent; i++) {
            fprintf(fd, XML_INDENT_STR);
        }
    }
    xml_on_new_line = 0;
}


/******************************************************************
 * If currently in XML attributes add end of tag. 
 * Helper function for xml_* functions.
 *****************************************************************/ 
static void end_xml_attributes(int add_newline)
{
    if (xml_in_attributes) {
        fprintf(fd, ">");
        xml_in_attributes = 0;
        if (add_newline) {
            fprintf(fd, "\n");
            xml_on_new_line = 1;
        }
    }
}


/******************************************************************
 * Open XML element.
 *****************************************************************/
static void xml_open(const char* elem)
{
    end_xml_attributes(1);
    add_xml_indent();
    fprintf(fd, "<%s", elem);
    xml_in_attributes = 1;
    xml_on_new_line = 0;
    xml_indent++;
}


/******************************************************************
 * Close XML element.
 *****************************************************************/
static void xml_close(const char* elem)
{
    xml_indent--;
    if (xml_in_attributes) {
        fprintf(fd, "/>\n");
        xml_in_attributes = 0;
    } else {
        add_xml_indent();
        fprintf(fd, "</%s>\n", elem);
    }
    xml_on_new_line = 1;
}


/******************************************************************
 * Add XML attribute.
 * Will give invalid XML syntax if xml_close() or xml_text() has 
 * been called since the last call to xml_open().
 *****************************************************************/
static void xml_attribute(const char* attr, const char* value)
{
    fprintf(fd, " %s=\"", attr);
    print_escaped_text(value);
    fprintf(fd, "\"");
}


/******************************************************************
 * Add text inside XML element.
 *****************************************************************/
static void xml_text(const char* text, int separate_line)
{
    end_xml_attributes(separate_line);
    add_xml_indent();
    print_escaped_text(text);
    if (separate_line) {
        fprintf(fd, "\n");
        xml_on_new_line = 1;
    }
}


/************************************
 * Initialize XML generation state.
 ***********************************/
static void xml_init()
{
    xml_indent = 0;
    xml_in_attributes = 0;
    xml_on_new_line = 1;
}


/******************************
 * Create the manifest file.
 *****************************/
int createManifestFile()
{
    char *path = NULL;
    char pathAndFile[MAX_PATH_LENGTH + 1];

    // Add filename to path to .library.
    path = getDotLibraryPath();
    snprintf(pathAndFile, MAX_PATH_LENGTH + 1, "%s/manifest.xml", path);

    fd = fopen(pathAndFile, "w");

    // Add data to file.
    xml_init();
    addHeader();
    addLibraryHeader();
    addEncryption();
    addIcon();
    addTools();
    addDependencies();
    addFooter();

    fclose(fd);
    return 1;
}


/*****************************************
 * Add XML attribute for a value, if set.
 ****************************************/
void add_value_attribute(const char* element, const char* argument)
{
    char *value = NULL;

    if ( (value = getValueOf(argument)) )
    {
        xml_attribute(element, value);
    }
}


/**********************************************************
 * Add XML element with text content for a value, if set.
 *********************************************************/
void add_value_element_and_text(const char* element, const char* argument, int separate_line)
{
    char *value = NULL;

    if ( (value = getValueOf(argument)) )
    {
        xml_open(element);
        xml_text(value, separate_line);
        xml_close(element);
    }
}


/**********************************************************
 * Add XML element with an attribute for a value, if set.
 *********************************************************/
void add_value_element_and_attribute(const char* element, const char* attribute, const char* argument)
{
    char *value = NULL;

    if ( (value = getValueOf(argument)) )
    {
        xml_open(element);
        xml_attribute(attribute, value);
        xml_close(element);
    }
}


/****************************
 * Add header to the file.
 ***************************/
void addHeader()
{
    fprintf(fd, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    xml_open("archive");
    xml_open("manifest");
    xml_attribute("version", "1.0");
    xml_close("manifest");
}



/****************************************
 * Add library information to the file.
 ***************************************/
void addLibraryHeader()
{
    char *value = NULL;

    xml_open("library");
    xml_attribute("id", getLibraryName());
    add_value_attribute("enabled", ARGUMENT_ENABLED);
    add_value_element_and_text("title", ARGUMENT_TITLE, 0);
    add_value_element_and_text("description", ARGUMENT_DESCRIPTION, 1);

    // Version (number, build(optional), date(optional) ).
    if ( (value = getValueOf(ARGUMENT_LIBRARY_VERSION)) )
    {
        xml_open("version");
        xml_attribute("number", value);
        add_value_attribute("build", ARGUMENT_BUILD_NUMBER);
        add_value_attribute("date", ARGUMENT_BUILD_DATE);
        xml_close("version");
    }

    add_value_element_and_attribute("language", "version", ARGUMENT_LANGUAGE_VERSION);
    add_value_element_and_text("copyright", ARGUMENT_COPYRIGHT, 1);
    add_value_element_and_text("license", ARGUMENT_LICENSE, 1);
}

/********************************************
 * Add LVE's needed to decrypt the library.
 *******************************************/
void addEncryption()
{
    int i = 0;
    char **lveList = NULL;
    char *platform = NULL;
    char path_buf[MAX_PATH_LENGTH];

    // Don't add anything if we are not using encryption.
    if (!usingEncryption())
    {
        return;
    }

    // Get list of lve's copied to .library.
    lveList = getLveList();

    xml_open("encryption");

    for (i = 0; i < NO_OF_LVE; ++i)
    {
        // The list contains 4 elements.
        // One or more element can be empty.
        if (lveList[i] == NULL)
        {
            continue;
        }

        platform = extractPlatform(lveList[i]);
        snprintf(path_buf, MAX_PATH_LENGTH, ".library/%s", lveList[i]);

        xml_open("executable");
        xml_attribute("path", path_buf);
        xml_attribute("platform", platform);
        xml_attribute("licensing", "true");
        xml_close("executable");

        free(platform);
    }

    xml_close("encryption");
}

/******************************************************
 * Add path to icon file.
 * This path is relative to the top-level directory
 * of the library.
 *****************************************************/
void addIcon()
{
    if (containsKey(ARGUMENT_ICON_PATH))
    {
        xml_open("icon");
        xml_attribute("file", getIconPath());
        xml_close("icon");
    }
}

/**************************************************************
 * This method add tools that the library is compatible with.
 * These tools are read from an xml file and pasted into
 * the manifest.xml file.
 *************************************************************/
void addTools()
{
    char *path = NULL;

    // Get value from command line argument.
    if ( (containsKey(ARGUMENT_TOOLS_FILE)) && (path = getValueOf(ARGUMENT_TOOLS_FILE)) )
    {
        copyFileInformation(path);
    }
}


/****************************************************************
 * This method add libraries that library depends on.
 * These dependencies are read from an xml file and pasted into
 * the manifest.xml file.
 ***************************************************************/
void addDependencies()
{
    char *path = NULL;

    // Get value from command line argument.
    if ( (containsKey(ARGUMENT_DEPENDENCIES_FILE)) && (path = getValueOf(ARGUMENT_DEPENDENCIES_FILE)) )
    {
        copyFileInformation(path);
    }
}

/********************************
 * Add the footer to the file.
 *******************************/
void addFooter()
{
    xml_close("library");
    xml_close("archive");
}

/****************************************
 * Extract platform from the filename.
 ***************************************/
char *extractPlatform(char *fileName)
{
    char *platForm = NULL;
    char *ptr1 = NULL;

    if ( (platForm = calloc(MAX_STRING, 1)) == NULL)
    {
        printf("Failed to allocate memory for platform string.\n");
        return 0;
    }

    // Locate '_'.
    ptr1 = strchr(fileName, '_');

    if (ptr1 == NULL)
    {
        // Return empty string.
        return platForm;
    }

	// Step past '_'.
    ++ptr1;
	strcpy(platForm, ptr1);

	// Remove any ".exe" suffix
    ptr1 = strchr(platForm, '.');
    if (ptr1 != NULL)
    {
        // Crop string here
        *ptr1 = '\0';
    }

    return platForm;
}



/****************************************************************
 * Copy everything from an xml-file apart from the first line
 * and paste it into the manifest.xml file.
 ***************************************************************/
void copyFileInformation(char *path)
{
    FILE *xmlfile;
    char ch;
    int index = 0;
    int firstLine = 1;
    char lineBuffer[200] = {'\0'};

    if ( (xmlfile = fopen(path, "r")))
    {
        ch = getc(xmlfile);

        while (ch != EOF)
        {
            if (ch != '\n')
            {
                lineBuffer[index] = ch;
                ++index;
            }
            else
            {
                // Skip first line in the file.
                if (!firstLine)
                {
                    add_xml_indent();
                    fprintf(fd, "%s\n", lineBuffer);
                }

                memset(&lineBuffer[0], 0, sizeof(lineBuffer));
                index = 0;
                firstLine = 0;
            }
            ch = getc(xmlfile);
        }
    }
    fclose(xmlfile);
    xml_on_new_line = 1;
}
