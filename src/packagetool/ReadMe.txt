
Packagetool
-----------

* This is a command-line tool for packaging a Modelica file structure into a zipped file.

* The code for this application has been compiled and executed successfully on Windows 7 and Ubuntu 14.04.

* Code has been compiled with CMake 3.2.


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


