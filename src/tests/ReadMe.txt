
Tests for LibraryEncryption
---------------------------

* The tests has been compiled and executed on Windows 7 and Ubuntu 14.04.

* Test and test unit framework are compiled with CMake 3.2.

* To be able to use these tests you must first compile the LibraryEncryption project.
  You might need to change path to lib and such in the tests/CMakeLists.txt.

Setup information
------------------
This test is using "Check - Unit testing framework for C" which is a open source project.
It can be downloaded from http://check.sourceforge.net/. On this page you also find
information about installing Check framework.

This text applies to version 0.9.14.


After compiling and installing check, if you using Windows, you must make a change in the ch_assert_int
method in the check.h file. Otherwise you will get a "seg fault" error if the values to check is not equal.

Change intmax_t to long and %jd to %ld. 

    /* Signed and unsigned integer comparison macros with improved output compared to ck_assert(). */
    /* OP may be any comparison operator. */
    #define _ck_assert_int(X, OP, Y) do { \
        long _ck_x = (X); \
        long _ck_y = (Y); \
        ck_assert_msg(_ck_x OP _ck_y, "Assertion '%s' failed: %s==%ld, %s==%ld", #X#OP#Y, #X, _ck_x, #Y, _ck_y); \
    } while (0)

It is also necessary to include these two typedefs in the file running the tests to avoid error.

    typedef int pid_t; // Must include this to avoid error.
    typedef int intmax_t; // And this

  
The code contains a "#pragma warning (disable : 4996 )" to get rid of "deprecated warnings". This might 
just have something to do with Microsofts include files. 


Running and compiling the tests
-------------------------------

Windows
-------
1. Go to the source folder for the test files src/tests.

2. Start a MSVC compilation shell and type "cmake -G "NMake Makefiles".

3. Type "nmake".

4. Run the test with "ctest -V".

Linux
-----
1. Go to the source folder for the test files src/tests.

2. Type "cmake .".

3. Type "make".

4. Run the test with "ctest -V".


