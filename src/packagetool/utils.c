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
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "utils.h"
#include "arguments.h"

#ifdef WIN32
#else
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <limits.h>
#endif

#include "mlle_portability.h"

// Encryption
#include "mlle_cr_encrypt.h"

// Zip.
#include "../../ThirdParty/miniz/miniz.c"



// ------------------------------------
// These are the LVE's currently used.
// ------------------------------------
char *lve[NO_OF_LVE] = {"lve_win32.exe", "lve_win64.exe", "lve_linux32", "lve_linux64"};

// ----------------------
// List of copied lve's.
// ----------------------
char **lveList;

// ---------------------------------------------
// Path to the .library in top-level directory.
// ---------------------------------------------
char *dotLibraryPath = NULL;

// -----------------------
// Path to the icon file.
// -----------------------
char *pathToIcon = NULL;


// ------------------------------------
// Path to the copied source directory.
// ------------------------------------
char* pathToCopiedSource = NULL;


/*************************
 * Free allocated space.
 ************************/
void cleanUp()
{
    int j;

    free(dotLibraryPath);
    if (lveList != NULL)
    {
        for(j = 0; j < NO_OF_LVE; ++j )
        {
            free(lveList[j]);
        }
    }
    free(lveList);
    free(pathToIcon);
    free(pathToCopiedSource);

    dotLibraryPath = NULL;
    lveList = NULL;
    pathToIcon = NULL;
    pathToCopiedSource = NULL;
}

/********************************************
 * Returns path to the .library directory.
 *******************************************/
char *getDotLibraryPath()
{
    return dotLibraryPath;
}


/**************************************************************
 * Returns array with the lve's that was copied to .library.
 *************************************************************/
char **getLveList()
{
    return lveList;
}


/***********************************
 * Returns path to the icon file.
 **********************************/
char *getIconPath()
{
    return pathToIcon;
}



/****************************************
 * Returns path to the folder where the
 * copied source files are.
 ***************************************/
char *getCopiedSourcePath()
{
    return pathToCopiedSource;
}


/*********************************************************
 * Check if the a path is a directory.
 *
 * Parameters:
 *      path - path to check.
 *
 * Returns:
 *      1 - path is a directory.
 *      0 - path is not a directory.
 ********************************************************/
int isDirectory(char *path)
{
    struct stat info;

    if (stat(path, &info) != 0)
    {
        // Directory does not exist.
        printf("Library path %s does not exist.\n", path);
        return 0;
    }
    else if (info.st_mode & S_IFDIR )
    {
        // It's a directory.
    }
    else
    {
        printf("Library path %s is not a directory.", path);
        return 0;
    }

    return 1;
}


/********************************************************
 * Validate a path.
 *
 * Parameters:
 *      path - path to check.
 *      errorMsg - array to store any error messages.
 *
 * Returns:
 *      1 - path is valid.
 *      0 - path is not valid.
 *******************************************************/
int validatePath(char *path)
{
    size_t path_size = 0;
    char last = '\0';

    // Is there something to check.
    if ( (path == NULL) || (strlen(path) == 0) )
    {
        printf("Path is empty!\n");
        return 0;
    }

    path_size = strlen(path);

    // Trim trailing slash
    last = path[path_size - 1];
    while (path_size > 1 && (last == '\\' || last == '/')) {
        path_size--;
        path[path_size] = '\0';
        last = path[path_size - 1];
    }

    // Validate the directory path.
    if (!isDirectory(path))
    {
        return 0;
    }

    return 1;
}

/************************************
 * Converts a string to lower case.
 ***********************************/
char *stringToLower(char *str)
{
    int i = 0;

    while( str[i] )
    {
        str[i] = tolower(str[i]);
        i++;
   }
    return str;
}


/**************************************
 * Get the current working directory.
 *************************************/
int getCurrentDirectory(char **cwd)
{
    // Space is allocated automatically.
#ifdef WIN32
    if ( ( *cwd = _getcwd(NULL, 0)) == NULL)
#else
	if ( ( *cwd = getcwd(NULL, 0)) == NULL)
#endif
    {
        printf("Current working directory not found.\n");
        return 0;
    }

    return 1;
}


/************************************
 * Get the directory where the
 * executable is running from.
 ***********************************/
char *getExecutableDirectory()
{
    char *exePath = NULL;
    char pathAndFilename[MAX_PATH_LENGTH + 1];

#ifdef WIN32

    // When passing NULL to GetModuleHandle, it returns handle of exe itself.
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule != NULL)
    {
        GetModuleFileName(hModule, pathAndFilename, MAX_PATH_LENGTH + 1);

        // Extract the path.
        if (pathAndFilename[0] != '\0')
        {
            exePath = extractPath(pathAndFilename);
        }
    }
#else
    char dest[MAX_PATH_LENGTH + 1];
    ssize_t len;
    struct stat info;

    pid_t pid = getpid();
    snprintf(pathAndFilename, MAX_PATH_LENGTH + 1, "/proc/%d/exe", pid);

    if ((len = readlink(pathAndFilename, dest, MAX_PATH_LENGTH)) > -1)
    {
        dest[len] = '\0';
        exePath = extractPath(dest);
    }
#endif

    return exePath;
}


/***************************
 * Count number of LVE's.
 **************************/
int countLVE()
{
    int count = 0;
    int i = 0;
    char *dir_path = NULL;
    char *path = NULL;
    size_t path_len = 0;

    // Path to where executable is running from.
    dir_path = getExecutableDirectory();

    if (dir_path == NULL)
    {
        printf("Cannot exctract the executable path.\n");
        return 0;
    }

    for (i = 0; i < NO_OF_LVE; ++i)
    {
        path_len = strlen(dir_path) + strlen(lve[i]) + 5 + 1;
        path = malloc(path_len);
        snprintf(path, path_len, "%s/LVE/%s", dir_path, lve[i]);
        count += fileExists(path);
        free(path);
    }

    return count;
}


/***********************************************************
 * Create the .library folder in the top-level directory.
 **********************************************************/
int createLibraryFolder()
{
    char* copied_src_path = NULL;
    size_t dot_lib_path_len = 0;

    copied_src_path = getCopiedSourcePath();
    dot_lib_path_len = strlen(copied_src_path) + MAX_STRING;
    if ( (dotLibraryPath = malloc(dot_lib_path_len)) == NULL)
    {
        printf("Error: Failed to allocate space for .library path.\n");
        return 0;
    }

    snprintf(dotLibraryPath, dot_lib_path_len, "%s/.library", copied_src_path);

#ifdef WIN32
	// Create folder.
    if (_mkdir(dotLibraryPath) < 0)
#else
	// Create folder with read/write/search permission for owner and group
	// and read/search permissions for others.
	if (mkdir(dotLibraryPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0)
#endif
    {
        printf("Error: Failed to create directory with path %s: %s\n", dotLibraryPath, strerror(errno));
        return 0;
    }

    return 1;
}


/*****************************************
 * Create array to add copied file names.
 ****************************************/
int createCopyArray()
{
    int j;

    if (lveList == NULL)
    {
        if ( (lveList = malloc(NO_OF_LVE * sizeof(char*))) )
        {
            for(j = 0; j < NO_OF_LVE; ++j )
            {
                if ( (lveList[j] = malloc(MAX_STRING)) == NULL)
                {
                    printf("Failed to allocate space for copied files.\n");
                    free(lveList);
                    return 0;
                }
            }
        }
        else
        {
            printf("Failed to allocate space for copied files.\n");
            return 0;
        }
    }
    return 1;
}


/**************************************************************
 * Copy all the LVE's from one folder to the .library folder.
 *************************************************************/
int copyLVE()
{
    int i = 0;
    char *dir_path = NULL;
    char *path = NULL;
    size_t path_len = 0;

    // We are done if we are not using encryption.
    if (!usingEncryption())
    {
        return 1;
    }

    if (!createCopyArray())
    {
        printf("Failed to allocate space for copied files.\n");
        return 0;
    }
	
	// Path to where executable is running from.
    dir_path = getExecutableDirectory();
	if (dir_path == NULL)
	{
		printf("Cannot exctract the executable path.\n");
		return 0;
	}

    for (i = 0; i < NO_OF_LVE; ++i)
    {
        path_len = strlen(dir_path) + strlen(lve[i]) + 5 + 1;
        path = malloc(path_len);
        snprintf(path, path_len, "%s/LVE/%s", dir_path, lve[i]);

        // Check if this LVE exists.
        if (fileExists(path))
        {
            // Copy the LVE to .library directory. Abort if a copy fails.
            if (copyFile(lve[i], path) == 0)
            {
                free(path);
                return 0;
            }

            // Store filename for later use.
            snprintf(lveList[i], MAX_STRING, "%s", lve[i]);
        }
        else
        {
            // Otherwise store empty string.
            lveList[i] ='\0';
        }

        free(path);
    }
	free(dir_path);
	
    return 1;
}



/*********************************************
 * Copy a file from one location to another.
 *********************************************/
int copyFile(char *filename, char *pathFrom)
{
    char *pathTo = NULL;
    char *cwd = NULL;
    char buffer[8192] = {'\0'};
    size_t path_len = 0;

    FILE *fdRead,*fdWrite;
    size_t readBytes = 0;
    size_t writeBytes = 0;
	    
	// Copy to.
    path_len = strlen(getDotLibraryPath()) + strlen(filename) + 2;
    pathTo = malloc(path_len);
    snprintf(pathTo, path_len, "%s/%s", getDotLibraryPath(), filename);

   // Create file descriptors.
   fdRead = fopen(pathFrom, "rb");
   fdWrite = fopen(pathTo, "wb");

   // Copy the file.
   while( (readBytes = fread(buffer, 1, sizeof(buffer), fdRead)) > 0)
   {
       writeBytes = fwrite(buffer, 1, readBytes, fdWrite);

       if (writeBytes < readBytes)
       {
           // Abort on error.
           if (ferror(fdWrite))
           {
               printf("Error while copying from %s to %s.\n", pathFrom, pathTo);
               return 0;
           }
       }
   }

   // Cleanup.
   fclose(fdRead);
   fclose(fdWrite);

   return 1;
}



/***************************************
 * Check if a file or directory exists.
 ***************************/
int fileExists(char *pahtAndFilename)
{
    struct stat fileStat;

   // Check if the file exists.
   if (stat(pahtAndFilename,&fileStat) == 0)
   {
       return 1;
   }

   return 0;
}

int prepareIconFile()
{
	// Is icon an argument.
	if (getValueOf("icon") == NULL)
	{
		return 1;
	}

    if (!locateIconFile())
    {
        printf("Unable to locate the icon file in the source folder.\n");
        return 0;
    }


    if (!cleanUpIconPath())
    {
        printf("Unable to remove tmp or library path from the icon file path.\n");
        return 0;
    }

    return 1;
}


/*************************************************
 * Locate the icon file in the source directory.
 *************************************************/
int locateIconFile()
{
    FileMode filemode;
    char *filename = NULL;
	char *value = NULL;
	int result = 0;
	int i = 0;

	// Get the icons path and file name.
	value = getValueOf("icon");

	// If no value, return and continue with the rest.
	if (!value)
	{
	    return 1;
	}

    // Extract filename from the path.
    filename = extractFilename(value);

    filemode = FIND_FILE;

    // Try to find the file.
#ifdef WIN32
    // Result is always 1.
    result = traverseDirectoryWin32(filename, getCopiedSourcePath(), filemode);
#else
	result = traverseDirectoryLinux(filename, getCopiedSourcePath(), filemode);
#endif

	// Abort if we didn't find the file.
	if (pathToIcon == NULL)
	{
	    result = 0;
	}

	return result;
}



/*************************************
 * Delete the copied source folder.
 ************************************/
int deleteCopiedSourceFolder()
{
    char *command = malloc(MAX_PATH_LENGTH);

    // Don't do anyting if directory doesn't exist,
    if (!fileExists(getCopiedSourcePath()))
    {
        return 1;
    }

#ifdef WIN32
    if (_snprintf(command, MAX_PATH_LENGTH, "rmdir /s /q %s", getCopiedSourcePath()) == -1)
    {
        free(command);
        printf("Filepath too long. Can't delete temporary folder.\n");
        return 0;
    }
    else
    {
        system(command);
        free(command);
    }
#else
    if (snprintf(command, MAX_PATH_LENGTH, "rm -r %s", getCopiedSourcePath()) >= MAX_PATH_LENGTH)
    {
        free(command);
        printf("Filepath too long. Can't delete temporary folder.\n");
        return 0;
    }
    else
    {
        if (system(command) == -1)
	{
		printf("Could not delete temporary folder.\n"); 
	}
        free(command);
    }
#endif

    return 1;
}


/************************************************************
* Traverse a directory structure on Windows and doing work
* depending on what the filemode parameter is set to.
************************************************************/
int traverseDirectoryWin32(char *filename, char *path, FileMode filemode)
{
    int result = 1;

#ifdef WIN32
    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
    char searchPath[MAX_PATH_LENGTH + 1];
    size_t path_len = 0;

    // Find all.
    snprintf(searchPath, MAX_PATH_LENGTH + 1, "%s\\*.*", path);

    // Try to find file in top-level directory.
    if((hFind = FindFirstFile(searchPath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        printf("Traverse directory failed.\n");
        return 0;
    }

    do
    {
        // The first two directories are always "." and "..".
        if(strcmp(fdFile.cFileName, ".") != 0
                && strcmp(fdFile.cFileName, "..") != 0)
        {
            // Build up our file path using the passed in
            // [path] and the file/foldername we just found:
            snprintf(searchPath, MAX_PATH_LENGTH + 1, "%s\\%s", path, fdFile.cFileName);

            // Have we found a folder?
            if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                // Use recursive to check the folder we found.
                traverseDirectoryWin32(filename, searchPath, filemode);
            }
            // Or a regular file?
            else if(fdFile.dwFileAttributes)
            {
                if (filemode == FIND_FILE)
                {
                    // Find a specific file.
                    if (strcmp(fdFile.cFileName, filename) == 0)
                    {
                        // Set path to icon file.
                        if (pathToIcon == NULL)
                        {
                            path_len = strlen(searchPath) + 1;
                            pathToIcon = malloc(path_len);
                            snprintf(pathToIcon, path_len, "%s", searchPath);

                            //printf("pathToIcon före: %s\n", pathToIcon);
                            //printf("searchPath: %s\n", searchPath);
                            //removeTmpFolderName(searchPath, pathToIcon);
                            //printf("pathToIcon efter: %s\n", pathToIcon);
                        }
                        result = 1;
                    }
                }
                else if (filemode == ENCRYPT_FILE)
                {
                    // Encrypt Modelica files.
                    if (isModelicaFile(fdFile.cFileName))
                    {
                        result = encryptFile(searchPath);
                    }
                }
                else if (filemode == DELETE_FILE)
                {
                    // Delete encrypted files when we are done.
                    if (isEncryptedFile(fdFile.cFileName))
                    {
                        // If remove is successful it returns 1.
                        if (remove(searchPath) == 0)
                        {
                            result = 1;
                        }
                        else
                        {
                            result = 0;
                        }
                    }
                }
            }
        }
    } while(FindNextFile(hFind, &fdFile)); //Find the next file

    FindClose(hFind);
#endif

    return result;
}

/***********************************************************
 * Traverse a directory structure on Linux and doing work
 * depending on what the filemode parameter is set to.
 ***********************************************************/
int traverseDirectoryLinux(char *filename, char *path, FileMode filemode)
{
    int result = 0;

#ifdef linux
    DIR *d;
    struct dirent *dir;
    char searchPath[MAX_PATH_LENGTH];
    size_t path_len = 0;

    // Find all.
    snprintf(searchPath, MAX_PATH_LENGTH, "%s", path);

    if ( (d = opendir(searchPath)) )
    {
        while ( (dir = readdir(d)) != NULL)
        {
            // The first two directories are always "." and "..".
            if(strcmp(dir->d_name, ".") != 0
                && strcmp(dir->d_name, "..") != 0)
            {
                // Build up our file path using the passed in
                // [path] and the file/foldername we just found:
                if (path[(strlen(path)-1)] == '/')
                {
                    snprintf(searchPath, MAX_PATH_LENGTH, "%s%s", path, dir->d_name);
                }
                else
                {
                    snprintf(searchPath, MAX_PATH_LENGTH, "%s/%s", path, dir->d_name);
                }

                // Have we found a folder?
                if(dir->d_type == DT_DIR)
                {
                    // Use recursive to check the folder we found.
                    traverseDirectoryLinux(filename, searchPath, filemode);
                }
                // Or a file?
                else if (dir->d_type == DT_REG)
                {
                    if (filemode == FIND_FILE)
                    {
                        // Find a specific file.
                        if (strcmp(dir->d_name, filename) == 0)
                        {
                            if (pathToIcon == NULL)
                            {
                                path_len = strlen(searchPath) + 1;
                                pathToIcon = malloc(path_len);
                                strcpy(pathToIcon, searchPath, path_len);
                            }

                            result = 1;
                        }
                    }
                    else if (filemode == ENCRYPT_FILE)
                    {
                        // Encrypt Modelica files.
                        if (isModelicaFile(dir->d_name))
                        {
                            result = encryptFile(searchPath);
                        }
                    }
                    else if (filemode == DELETE_FILE)
                    {
                        // Delete encrypted files when we are done.
                        if (isEncryptedFile(dir->d_name))
                        {
                            // If remove is successful it returns 1.
                            if (remove(searchPath) == 0)
                            {
                                result = 1;
                            }
                            else
                            {
                                result = 0;
                            }
                        }
                    }
                }
            }
        }
        closedir(d);
    }
    else
    {
        printf("Failed to open directory %s\n", searchPath);
        return 0;
    }
#endif

    return result;
}

/**************************************
 * Extract the filename from a path.
 *************************************/
char *extractFilename(char *path)
{
    char *name;
	char *tmp;

    name = strrchr(path, '/');

#ifdef WIN32
	// If WIN then path sep could be '\'
	if (name == NULL) {
		tmp = strrchr(path, '\\');
	} else {
		// Check if there were mixed '/' and '\'
		tmp = strrchr(name, '\\');
	}
	if (tmp != NULL) {
		name = tmp;
	}
#endif
	
	if (name != NULL) {
		++name;
	} else {
		name = path;
	}

	return name;

}

/**************************************
 * Extract the filename from a path.
 *************************************/
char *extractPath(char *pathAndFilename)
{
    char *ptr1 = NULL;
	char *ptr2 = NULL;
	char *result = NULL;
	size_t len = 0;
	
	// Safety check.
	if (!pathAndFilename)
	{
		return NULL;
	}
	
	ptr2 = pathAndFilename;

#ifdef WIN32
    ptr1 = strrchr(pathAndFilename, '\\');
#else
    ptr1 = strrchr(pathAndFilename, '/');
#endif


    if (ptr1 == NULL)
    {
        len = strlen(ptr2) + 1;
        result = malloc(len);
        snprintf(result, len, "%s", ptr2);
    }
    else
    {
        result = malloc((ptr1 - ptr2) + 1);
        memcpy(result, ptr2, (ptr1 - ptr2));
        result[ptr1 - ptr2] = '\0';
    }

    return result;
}


/*******************************************************
 * Check if a file is a Modelica file (extension .mo).
 ******************************************************/
int isModelicaFile(char *filename)
{
    char *ext;

    if (filename == NULL)
    {
        return 0;
    }

    ext = strrchr(filename, '.');

    if ( (ext != NULL) && (strcmp(stringToLower(ext), ".mo") == 0) )
    {
        return 1;
    }

    return 0;
}

/**************************************************
 * Check if a file is an encrypted Modelica file.
 *************************************************/
int isEncryptedFile(char *filename)
{
    char *ext;

    // Safety check.
    if (filename == NULL)
    {
        return 0;
    }

    // Get file extension.
    ext = strrchr(filename, '.');

    if ( (ext != NULL) && (strcmp(stringToLower(ext), ".moc") == 0) )
    {
        return 1;
    }

    return 0;
}


/*****************************
 * Encrypt a Modelica file.
 ****************************/
int encryptFile(char *file)
{
    FILE *in, *out;
    char *newFile;
    int result = 1;
    size_t path_len;

    path_len = strlen(file) + 2;
    if ( (newFile = malloc(path_len)) == NULL)
    {
        printf("Error: Couldn't allocate space for path to encrypted file.\n");
        result = 0;
    }

    // Add a "c" to end of filename so we get an .moc extension.
    snprintf(newFile, path_len, "%sc", file);

    // Open file to read from.
    if ( result && ((in = fopen(file, "rb")) == NULL) )
    {
        printf("Error: Couldn't open file %s for reading.\n", file);
        result = 0;
    }

    // Open file to write to.
    if ( result && ((out = fopen(newFile, "wb")) == NULL) )
    {
        printf("Error: Couldn't open file %s for writing.\n", newFile);
        result = 0;
    }

    if (result && (!mlle_cr_encrypt(in, out)) )
    {
        printf("Failed to encrypt file.\n");
        result = 0;
    }

    free(newFile);
    fclose(in);
    fclose(out);

    return result;
}


/******************
 * Encrypt files.
 *****************/
int encryptFiles()
{
    int result = 1;
    FileMode filemode;

    filemode = ENCRYPT_FILE;

    if (usingEncryption())
    {
#ifdef WIN32
        result = traverseDirectoryWin32("", getCopiedSourcePath(), filemode);
#else
        result = traverseDirectoryLinux("", getCopiedSourcePath(), filemode);
#endif
    }

    return result;
}


/*****************************
 * Create a zipped archive.
 ****************************/
int createZipArchive()
{
    char *cwd = NULL;
    char archivename[MAX_STRING] = {'\0'};
    char pathToZipfile[MAX_PATH_LENGTH] = {'\0'};
	char currentDir[MAX_PATH_LENGTH] = {'\0'};
    int encrypted = 0;

    // Get last folder name in library path. This will
    // be the name of the archive.
    snprintf(archivename, MAX_STRING, "%s.mol", getLibraryName());

    // Is library encrypted. If so, don't put the .mo-files in the archive.
    if (usingEncryption())
    {
        encrypted = 1;
    }

    // Get current directory.
    if (getCurrentDirectory(&cwd))
    {
        snprintf(pathToZipfile, MAX_PATH_LENGTH, "%s/%s", cwd, archivename);

        // Remove any previous zip-file.
        if (fileExists(pathToZipfile))
        {
            remove(pathToZipfile);
        }
    }

#ifdef WIN32
    zipDirectoryWin32(getCopiedSourcePath(), archivename, encrypted);
#else
    zipDirectoryLinux(getCopiedSourcePath(), archivename, encrypted);
#endif

    return 1;

}

/******************************************************
 * Creates a zipped archive of a directory on Windows.
 *****************************************************/
int zipDirectoryWin32(char *path, char *archiveName, int encrypted)
{
    int result = 0;
#ifdef WIN32
    WIN32_FIND_DATA fData;
    HANDLE hFind = NULL;
    char searchPath[MAX_PATH_LENGTH];

    FILE *fd = NULL;
    struct stat info;
    char *data = NULL;
    size_t bytes = 0;
	int index = 0;
    char *comment = "Modelica_archive";
    long buffertSize = 0;
    char zipPath[MAX_PATH_LENGTH] = {'\0'};

    // Find all.
    snprintf(searchPath, MAX_PATH_LENGTH, "%s\\*.*", path);


    // Try to find file in top-level directory.
    if((hFind = FindFirstFile(searchPath, &fData)) == INVALID_HANDLE_VALUE)
    {
        printf("Error creating archive starting from \"%s\".\n", path);
        return 0;
    }

    do
    {
        // The first two directories are always "." and "..".
        if(strcmp(fData.cFileName, ".") != 0
                && strcmp(fData.cFileName, "..") != 0)
        {
            // Build up our file path using the passed in
            // [path] and the file/foldername we just found:
            snprintf(searchPath, MAX_PATH_LENGTH, "%s\\%s", path, fData.cFileName);

            // Have we found a folder?
            if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                // Use recursive to check the folder we found.
                zipDirectoryWin32(searchPath, archiveName, encrypted);
            }

            // Or a regular file?
            else if(fData.dwFileAttributes)
            {
                // -------------------------------------------------------------
                // Only add file to archive when:
                // 1. Files are not encrypted.
                // 2. Files are encrypted and the file is not a Modelica file.
                // -------------------------------------------------------------
                if ( (!encrypted) || ( (encrypted && !isModelicaFile(fData.cFileName))) )
                {

                    if (stat(searchPath, &info) != 0)
                    {
                        printf("Error: Failed to read information of file \"%s\".\n", searchPath);
                        return 0;
                    }

                    // Get file size.
                    buffertSize = info.st_size;

                    // Allocate data for the whole file.
                    if ( (data = calloc((buffertSize), 1)) == NULL)
                    {
                        printf("Error: Failed to allocate memory for file \"%s\".\n", searchPath);
                        return 0;
                    }

                    // Read the whole file to a buffer.
                    fd = fopen(searchPath, "rb");
                    bytes = fread(data, sizeof(char), buffertSize, fd);
                    fclose(fd);

                    if (bytes < buffertSize)
                    {
                        printf("Error: Failed to copy file \"%s\" to zip-archive.\n", searchPath);
                        free(data);
                        return 0;
                    }

                    // Remove the tmp path.
                    removeTmpFolderName(searchPath, zipPath, MAX_PATH_LENGTH);

					// On Windows we must change backward slash to forward slash 
					// otherwise the zip library fails to add the file.
					for (index = 0; index <= (signed)strlen(zipPath); ++index)
					{
						if (zipPath[index] == '\\')
						{
							zipPath[index] = '/';
						}
					}

                    // Add file to zip archive.
                    result = mz_zip_add_mem_to_archive_file_in_place(archiveName, zipPath, data, buffertSize,
                      comment, strlen(comment), MZ_BEST_COMPRESSION);

                    if (!result)
                    {
                        printf("Error: Failed to add file \"%s\" to zip archive.\n", zipPath);
                    }

                    free(data);
                }
            }
        }
    } while(FindNextFile(hFind, &fData)); //Find the next file

    FindClose(hFind);
#endif

    return result;
}


/******************************************************
 * Creates a zipped archive of a directory on Linux.
 *****************************************************/
int zipDirectoryLinux(char *path, char *archiveName, int encrypted)
{
    int result = 0;

 #ifdef linux
    DIR *d;
    struct dirent *dir;
    char searchPath[MAX_PATH_LENGTH];

    FILE *fd = NULL;
    struct stat info;
    char *data = NULL;
    int bytes = 0;
    char *comment = "Modelica_archive";

    char zipPath[MAX_PATH_LENGTH + 1] = {'\0'};
    long buffertSize = 0;


    // Find all.
    snprintf(searchPath, MAX_PATH_LENGTH + 1, "%s", path);
	//printf("**** searchPath %s\n", searchPath);

    if ( (d = opendir(searchPath)) )
    {
        while ( (dir = readdir(d)) != NULL)
        {
             // The first two directories are always "." and "..".
             if (strcmp(dir->d_name, ".") != 0
                 && strcmp(dir->d_name, "..") != 0)
             {
                 // Build up our file path using the passed in
                 // [path] and the file/foldername we just found:
                 if (path[(strlen(path)-1)] == '/')
                 {
                     snprintf(searchPath, MAX_PATH_LENGTH + 1, "%s%s", path, dir->d_name);
                 }
                 else
                 {
                     snprintf(searchPath, MAX_PATH_LENGTH + 1, "%s/%s", path, dir->d_name);
                 }

				//printf("zip: searchpath: %s\n", searchPath);
 
                 // Have we found a folder?
                 if(dir->d_type == DT_DIR)
                 {
                     // Use recursive to check the folder we found.
                     zipDirectoryLinux(searchPath, archiveName, encrypted);
                 }
                 // Or a file?
                 else if (dir->d_type == DT_REG)
                 {
                     // -----------------------------------------------------------------
                     // Only add file to archive when:
                     // 1. Files are not encrypted.
                     // 2. Files are encrypted and the file is not a Modelica file(.mo).
                     // -----------------------------------------------------------------
					//printf("Filename: %s, encrypted: %d\n", dir->d_name, encrypted);

                     if ( (!encrypted) || ( (encrypted && !isModelicaFile(dir->d_name))) )
                     {
                         if (stat(searchPath, &info) != 0)
                         {
                             printf("Error: Failed to read information of file \"%s\".\n", searchPath);
                             return 0;
                         }

                         buffertSize = info.st_size;

                         // Allocate data for the whole file.
                         if ( (data = calloc((buffertSize), 1)) == NULL)
                         {
                             printf("Error: Failed to allocate memory for file \"%s\".\n", searchPath);
                             return 0;
                         }

                         // Read the whole file to the buffer.
                         fd = fopen(searchPath, "rb");
                         bytes = fread(data, sizeof(char), buffertSize, fd);
                         fclose(fd);

                         if (bytes < buffertSize)
                         {
                             printf("Error: Failed to copy file \"%s\" to zip-archive.\n", searchPath);
                             free(data);
                             return 0;
                         }

						// Remove the tmp path.
						removeTmpFolderName(searchPath, zipPath, MAX_PATH_LENGTH);

                         // Add file to zip archive.
                         result = mz_zip_add_mem_to_archive_file_in_place(archiveName, zipPath, data, buffertSize,
                           comment, strlen(comment), MZ_BEST_COMPRESSION);

                         if (!result)
                         {
                             printf("Error: Failed to add file \"%s\" to zip archive.\n", zipPath);
                         }

                         free(data);
                     }
                 }
             }
        }
        closedir(d);
    }
    else
    {
        printf("Failed to open directory %s\n", searchPath);
    }
 #endif

     return result;
}

/**********************************************************
 * Remove temp path and library name from icon file path.
 *********************************************************/
int cleanUpIconPath()
{
    char *tmpDir = NULL;
    char *tmpBuf = NULL;
    char *ptr1, *ptr2 = NULL;
    size_t path_len = 0;
	int i = 0;

    // Safety check.
    if (pathToIcon == NULL)
    {
        printf("Path to icon is empty.\n");
        return 0;
    }

    tmpBuf = pathToIcon;

    // Get the tmp path.
    #ifdef WIN32
        tmpDir = getenv("TEMP");
    #else
        tmpDir = getenv("TMPDIR");
        if (tmpDir == NULL)
        {
            tmpDir = "/tmp";
        }
    #endif

    // Point to icon path.
    ptr1 = tmpBuf;

    // Step forward to after temp folders.
    ptr1 += strlen(tmpDir) + 1;

    if (ptr1 && *ptr1)
    {
    // Point to next slash.
#ifdef WIN32
    ptr2 = strchr(ptr1, '\\');

	// replace '\\' with '/' according to spec
	while (ptr2[i]) {    
     if (ptr2[i] == '\\') {
		 ptr2[i] = '/';
	 }
	 i++;
	}
#else
    ptr2 = strchr(ptr1, '/');
#endif

        // Step forward to after library name.
        ++ptr2;

        if (ptr2 && *ptr2)
        {
            // Create icon path without tmp or library name.
            path_len = strlen(ptr2) + 1;
            pathToIcon = malloc(path_len);
            snprintf(pathToIcon, path_len, "%s", ptr2);
            return 1;
        }
    }

    free(tmpBuf);

    return 0;
}


/*****************************************************
 * Remove the temp folder path from icon file path.
 * This is done before adding files to zip file.
 ****************************************************/
int removeTmpFolderName(char *searchPath, char *zipPath, size_t zipPathLen)
{
	char *tmpDir = NULL;
	char *ptr = NULL;

	// Get the tmp string.
#ifdef WIN32
    tmpDir = getenv("TEMP");
#else
	tmpDir = getenv("TMPDIR");
    if (tmpDir == NULL)
    {
        tmpDir = "/tmp";
    }
#endif

	// Clear the array. 
	memset(&zipPath[0], 0, strlen(zipPath));

	ptr = searchPath;
	ptr += strlen(tmpDir) + 1;
	
	if (ptr && *ptr)
	{
	    snprintf(zipPath, zipPathLen, "%s", ptr);
	}

	return 1;
}



/***********************************************
 * Check if encryption of files is activated.
 **********************************************/
int usingEncryption()
{
    char *value = getValueOf(ARGUMENT_ENCRYPT);

    // Is encryption activated?
    return (containsKey(ARGUMENT_ENCRYPT) && (strcmp(stringToLower(value), "true") == 0));
}


/***********************************************************************
 * Creates a copy of the folder structure we are making a container of.
 **********************************************************************/
int copyFolderStructure()
{
    char *tempDir = NULL;
    char *folderName = NULL;
    char *copyFromPath = NULL;
    size_t size = 0;

#ifdef WIN32
    tempDir = getenv("TEMP");
#else
    tempDir = getenv("TMPDIR");
    if (tempDir == NULL)
    {
        tempDir = "/tmp";
    }
#endif

    if (tempDir == NULL)
    {
        printf("Unable to find environment variable for temporary folder. Abort!");
        return 0;
    }

    // Get name of top folder.
    folderName = getLibraryName();

    size = strlen(tempDir) + 1 + strlen(folderName) + 1;

    // Create path to top folder.
    pathToCopiedSource = malloc(size);


    // Create folder path.
#ifdef WIN32
    if (_snprintf(getCopiedSourcePath(), size, "%s\\%s", tempDir, folderName) == -1)
    {
        printf("Folderpath to large. Can't copy source folder. Abort!");
        free(pathToCopiedSource);
        return 0;
    }

    // Make sure any previous source folder structure is removed.
    if (!deleteCopiedSourceFolder())
    {
        printf("Error: couldn't remove old temprary folder at %s\n", getCopiedSourcePath());
        return 0;
    }

	// Create top folder.
    if (_mkdir(getCopiedSourcePath()) < -1)
    {
        printf("Failed to create temporary folder. Abort.\n");
        free(pathToCopiedSource);
        return 0;
    }
#else

    if (snprintf(getCopiedSourcePath(), size, "%s/%s", tempDir, folderName) > size)
    {
        printf("Folderpath to large. Can't copy source folder. Abort!");
        free(pathToCopiedSource);
        return 0;
    }

    // Make sure any previous source folder structure is removed.
    if (!deleteCopiedSourceFolder())
    {
        printf("Error: couldn't remove old temprary folder at %s\n", getCopiedSourcePath());
        return 0;
    }

    // Create folder with read/write/search permission for owner and group
    // and read/search permissions for others.
    if (mkdir(getCopiedSourcePath(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0)
    {
        printf("Failed to create temporary folder. Abort.\n");
        free(pathToCopiedSource);
        return 0;
    }
#endif

   
    // Get path to source folder where we will copy files from.
    if ( (copyFromPath = getValueOf("librarypath")) == NULL)
    {
        return 0;
    }

#ifdef WIN32 
    // Copy files and folders to temporary folder on Windows.
    return (copyDirectoryWin32(copyFromPath, getCopiedSourcePath()));
#else
	// Copy files and folders to temporary folder on Linux
	return (copyDirectoryLinux(copyFromPath, getCopiedSourcePath()));
#endif
}


/***********************************************************
 * Copy a directory including files and subdirectory from
 * one location to another.
 **********************************************************/
int copyDirectoryWin32(char *fromPath, char *toPath)
{
    int result = 1;

#ifdef WIN32
    struct stat info;
    int buffSize = 0;
    size_t bytes = 0;
    char *data = NULL;
    FILE *fdRead = NULL;
    FILE *fdWrite = NULL;


    WIN32_FIND_DATA fdFile;
    HANDLE hFind = NULL;
    char searchPath[MAX_PATH_LENGTH];
    char searchPathTo[MAX_PATH_LENGTH];
    char folderName[MAX_PATH_LENGTH];

    // Find all.
    // TODO add check and increase memory if necessary.
    snprintf(searchPath, MAX_PATH_LENGTH, "%s\\*.*", fromPath);

    // Try to find file in top-level directory.
    if((hFind = FindFirstFile(searchPath, &fdFile)) == INVALID_HANDLE_VALUE)
    {
        printf("Traverse directory failed.\n");
        return 0;
    }

    do
    {
        // The first two directories are always "." and "..".
        if(strcmp(fdFile.cFileName, ".") != 0
                && strcmp(fdFile.cFileName, "..") != 0
                && strcmp(fdFile.cFileName, ".library") != 0)
        {

            // Build up our file path using the passed in
            // [path] and the file/foldername we just found:
            snprintf(searchPath, MAX_PATH_LENGTH, "%s\\%s", fromPath, fdFile.cFileName);

            // Have we found a folder?
            if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                _snprintf(searchPathTo, MAX_PATH_LENGTH, "%s\\%s", toPath, fdFile.cFileName);
                // TODO Create folder name.

                if (_mkdir(searchPathTo) == -1)
                {
                    printf("Error: Failed to create folder %s.\n", searchPathTo);
                }

                // Use recursive to check the folder we found.
                copyDirectoryWin32(searchPath, searchPathTo);
            }

            // Or a regular file?
            else if(fdFile.dwFileAttributes)
            {
                {
                    fdRead = fopen(searchPath, "rb");

                    if (stat(searchPath, &info) != 0)
                    {
                        printf("Error: Failed to read information of file \"%s\".\n", searchPath);
                        return 0;
                    }
                    // Get file size.
                    buffSize = info.st_size;

                    // Allocate data for the whole file.
                    if ( (data = calloc((buffSize + 1), 1)) == NULL)
                    {
                        printf("Error: Failed to allocate memory for file \"%s\".\n", searchPath);
                        return 0;
                    }

                    // Read the whole file to a buffer.
                    bytes = fread(data, sizeof(char), buffSize, fdRead);

                    // Write to file in new location.
                    if (_snprintf(folderName, MAX_PATH_LENGTH, "%s\\%s", toPath, fdFile.cFileName) == -1)
                    {
                        printf("Not enough memory to create folder path %s.\n", toPath);
                        return 0;
                    }

                    fdWrite = fopen(folderName, "wb");
                    fwrite(data, sizeof(char), buffSize, fdWrite);

                    fclose(fdRead);
                    fclose(fdWrite);
                    free(data);
                }
            }
        }
    } while(FindNextFile(hFind, &fdFile)); //Find the next file

    FindClose(hFind);

#endif
    return result;
}


/***********************************************************
 * Copy a directory including files and subdirectory from
 * one location to another.
 **********************************************************/
int copyDirectoryLinux(char *fromPath, char *toPath)
{
    int result = 1;
#ifdef linux
    DIR *d;
    struct stat info;
    struct dirent *dir;
    char *data = NULL;
    int bytes = 0;
    long buffSize = 0;
    FILE *fdRead = NULL;
    FILE *fdWrite = NULL;
    char searchPath[MAX_PATH_LENGTH];
    char searchPathTo[MAX_PATH_LENGTH];
    char folderName[MAX_PATH_LENGTH];


    // Find all.
    snprintf(searchPath, MAX_PATH_LENGTH, "%s", fromPath);

	//printf("searchPath: %s\n", searchPath);

    if ( (d = opendir(searchPath)) )
    {
        while ( (dir = readdir(d)) != NULL)
        {
            // The first two directories are always "." and "..".
            if(strcmp(dir->d_name, ".") != 0
                && strcmp(dir->d_name, "..") != 0
                && strcmp(dir->d_name, ".library") != 0)
            {
                // Build up our file path using the passed in
                // [path] and the file/foldername we just found:
                if (fromPath[(strlen(fromPath)-1)] == '/')
                {
                    snprintf(searchPath, MAX_PATH_LENGTH, "%s%s", fromPath, dir->d_name);
                }
                else
                {
                    snprintf(searchPath, MAX_PATH_LENGTH, "%s/%s", fromPath, dir->d_name);
                }

                // Have we found a folder?
                if(dir->d_type == DT_DIR)
                {


                    snprintf(searchPathTo, MAX_PATH_LENGTH, "%s/%s", toPath, dir->d_name);
                    // TODO Create folder name.

                    if (mkdir(searchPathTo, 0777) == -1)
                    {
                        printf("Error: Failed to create folder %s.\n", searchPathTo);
                    }

                    // Use recursive to check the folder we found.
                    copyDirectoryLinux(searchPath, searchPathTo);
                }
                // Or a file?
                else if (dir->d_type == DT_REG)
                {
                    //printf("Filnamn: %s\n", dir->d_name);
                    {
                        //printf("Read - searchPath: %s\n", searchPath);
                        fdRead = fopen(searchPath, "rb");

                        if (stat(searchPath, &info) != 0)
                        {
                            printf("Error: Failed to read information of file \"%s\".\n", searchPath);
                            return 0;
                        }

                        // Get file size.
                        buffSize = info.st_size;

                        // Allocate data for the whole file.
                        if ( (data = calloc((buffSize + 1), 1)) == NULL)
                        {
                            printf("Error: Failed to allocate memory for file \"%s\".\n", searchPath);
                            return 0;
                        }

                        // Read the whole file to a buffer.
                        bytes = fread(data, sizeof(char), buffSize, fdRead);

                        // Write to file in new location.
                        snprintf(folderName, MAX_PATH_LENGTH, "%s/%s", toPath, dir->d_name);

                        fdWrite = fopen(folderName, "wb");
                        fwrite(data, sizeof(char), buffSize, fdWrite);

                        fclose(fdRead);
                        fclose(fdWrite);
                        free(data);
                    }
                }
            }
        }
        closedir(d);
    }
    else
    {
        printf("Failed to open directory %s\n", searchPath);
        return 0;
    }
#endif

    return result;
}





