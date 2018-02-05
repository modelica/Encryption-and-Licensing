
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


#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef WIN32
    typedef int pid_t;      // Must include this to avoid error in Check test framework.
    typedef int intmax_t;   // And this.
#endif

#include <stdio.h>
#include <check.h>

// Test files.
#include "test_commands.c"
#include "test_ssl.c"
#include "test_tokens.c"


/*
 * Run the tests.
 */
int main (int argc, char *argv[])
{
	int number_failed;

	Suite *suiteTokens = suite_tokens();
	Suite *suiteCommands = suite_commands();
	Suite *suiteSSL = suite_ssl();
	
	SRunner *runner = srunner_create(suiteTokens);
	srunner_add_suite(runner, suiteCommands);
	srunner_add_suite(runner, suiteSSL);
	srunner_run_all(runner, CK_NORMAL);

	number_failed = srunner_ntests_failed(runner);

	srunner_free(runner);
	return number_failed;
}
