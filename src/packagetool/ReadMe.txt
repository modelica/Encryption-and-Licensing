
Packagetool
-----------

* This is a command-line tool for packaging a Modelica file structure into a zipped file.

* The code for this application has been compiled and executed successfully on Windows 7 and Ubuntu 14.04.

* Code has been compiled with CMake 3.2.

Zip-tool
--------

The zipped file is created using the miniz, a lossless, high performance data compression library 
in a single source file that implements the zlib (RFC 1950) and Deflate (RFC 1951) compressed data 
format specification standards.

It's completely free: Public domain in jurisdictions that recognize copyright laws, with a license 
patterned after the public domain SQLite project, see unlicense.org.

It can be downloaded from https://code.google.com/p/miniz/

It contains only one c-file so you only need to include this with "#include "../../ThirdParty/miniz/miniz.c" 
in the source file where you want to use it. In this project it's included in utils.c. 
So you don't have to add it in the CMakeLists.txt.

The sourcefile is the folder ThirdParty/miniz.



Windows
-------
1. Go to the source folder for the test files src/packagetool.

2. Start a MSVC compilation shell and type "cmake -G "NMake Makefiles".

3. Type "nmake" to compile the tool.

4. Execute the tool from the shell with something like:

  packagetool.exe -librarypath "C:\LibraryEncryption\Modelica" -enabled "true" -title "Example title"^
 -description "Dummy description" -librarynumber "1.0" -build "1.6" -date "2015-06-17"^
 -language "3.2" -copyright "Copyright @ 2015 The company" -license "Some license information."^
 -icon "C:\LibraryEncryption\Icon\Mushroom.ico" -encrypt "true" -tools "C:\LibraryEncryption\xml-files\tools.xml"^
 -dependencies "C:\LibraryEncryption\xml-files\dependencies.xml"
 
  

Linux
-----
1. Go to the source folder for the test files src/tests.

2. Type "cmake .".

3. Type "make".

4. Execute the tool from the shell with something like:

  ./packagetool.exe -librarypath "C:\LibraryEncryption\Modelica" -enabled "true" -title "Example title"^
 -description "Dummy description" -librarynumber "1.0" -build "1.6" -date "2015-06-17"^
 -language "3.2" -copyright "Copyright @ 2015 The company" -license "Some license information."^
 -icon "C:\Temp\Icon\Mushroom.ico" -encrypt "true" -tools "C:\LibraryEncryption\xml-files\tools.xml"^
 -dependencies "C:\LibraryEncryption\xml-files\dependencies.xml"


