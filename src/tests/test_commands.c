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


/*
 * This file is for unit testing the functions in the file mlle_parse_command.c.
 * Test frameworks is Check (http://check.sourceforge.net/).
 */


#include "check.h"
#include "mlle_parse_command.h"

/*
 * Test parsing a non existing command.
 */
START_TEST (test_parse_nonexisting_command)
{
    size_t errorLength = 100;
    char command[] = "UNKNOWN_COMMAND";
    char error_msg[100] = {0};
    enum mlle_grammar_error_t output;
    struct mlle_command command_struct = {0, 0, 0, 0};
	char *commandPtr = command;

    output = mlle_parse_command(commandPtr, &command_struct, error_msg, errorLength);

    // Check return values.
    ck_assert_int_eq(command_struct.id, 0);
    ck_assert_int_eq(output, LE_UNKNOWN_CMD);
}
END_TEST

/*
 * Test parsing empty command.
 */
START_TEST (test_parse_empty_command)
{
    size_t errorLength = 100;
    char *command = "";
    char error_msg[100] = {0};
    enum mlle_grammar_error_t output;
    struct mlle_command command_struct = {0, 0, 0, 0};

    output = mlle_parse_command(command, &command_struct, error_msg, errorLength);

    // Check return values.
    ck_assert_int_eq(command_struct.id, 0);
    ck_assert_int_eq(output, LE_NO_TOKENS);
}
END_TEST


/*
 * Test parsing the version command with no parameter.
 */
START_TEST (test_parse_version_command_no_parameter)
{
    size_t errorLength = 100;
    char command[] = "VERSION";
    char error_msg[100] = {0};
    enum mlle_grammar_error_t output;
    struct mlle_command command_struct = {0, 0, 0, 0};
	char *commandPtr = command;

    output = mlle_parse_command(commandPtr, &command_struct, error_msg, errorLength);

    // Check return values.
    ck_assert_int_eq(command_struct.id, 0);
    ck_assert_int_eq(output, LE_TOO_FEW_TOKENS);
}
END_TEST



/*
 * Test parsing the version command with to many parameters.
 */
START_TEST (test_parse_version_command_to_many_parameters)
{
    size_t errorLength = 100;
    char command[] = "VERSION 1 2";
    char error_msg[100] = {0};
    enum mlle_grammar_error_t output;
    struct mlle_command command_struct = {0, 0, 0, 0};

	char *commandPtr = command;

    output = mlle_parse_command(commandPtr, &command_struct, error_msg, errorLength);

    // Check return values.
    ck_assert_int_eq(command_struct.id, 0);
    ck_assert_int_eq(output, LE_TOO_MANY_TOKENS);
}
END_TEST


/*
 * Test parsing the version command with parameter that is not an integer.
 */
START_TEST (test_parse_version_command_no_int_parameters)
{
    size_t errorLength = 100;
    char command[] = "VERSION AA";
    char error_msg[100] = {0};
    enum mlle_grammar_error_t output;
    struct mlle_command command_struct = {0, 0, 0, 0};
	char *commandPtr = command;

    output = mlle_parse_command(commandPtr, &command_struct, error_msg, errorLength);

    // Check return values.
    ck_assert_int_eq(command_struct.id, 0);
    ck_assert_int_eq(output, LE_NOT_AN_INT);
}
END_TEST


/*
 * Test parsing the version command with valid parameter.
 */
START_TEST (test_parse_version_command_with_valid_parameter)
{
    size_t errorLength = 100;
    char command[] = "VERSION 3";
    char error_msg[100] = {0};
    enum mlle_grammar_error_t output;
    struct mlle_command command_struct = {0, 0, 0, 0};
	char *commandPtr = command;

    output = mlle_parse_command(commandPtr, &command_struct, error_msg, errorLength);

    // Check return values.
    ck_assert_int_eq(command_struct.id, 4);
    ck_assert_int_eq(command_struct.number, 3);
    ck_assert_int_eq(output, LE_VALID_GRAMMAR);
}
END_TEST


/*
 * Test parsing the error command with valid parameters.
 * This is the only command with 3 parameters.
 */
START_TEST (test_parse_error_command_with_valid_parameters)
{
    size_t errorLength = 100;
    char command[] = "ERROR 7 56LN987";
    char error_msg[100] = {0};
    enum mlle_grammar_error_t output;
    struct mlle_command command_struct = {0, 0, 0, 0};
	char *commandPtr = command;

    output = mlle_parse_command(commandPtr, &command_struct, error_msg, errorLength);

    // Check return values.
    ck_assert_int_eq(command_struct.number, 7);
    ck_assert_int_eq(command_struct.length, 56);
    ck_assert_int_eq(output, LE_VALID_GRAMMAR);
}
END_TEST



Suite* suite_commands(void)
{
    Suite *suite = suite_create("test_command");
    TCase *tcase = tcase_create("test_command_case");
    tcase_add_test(tcase, test_parse_nonexisting_command);	
    tcase_add_test(tcase, test_parse_empty_command);	
	tcase_add_test(tcase, test_parse_version_command_no_parameter);
    tcase_add_test(tcase, test_parse_version_command_to_many_parameters);	
    tcase_add_test(tcase, test_parse_version_command_no_int_parameters);	
	tcase_add_test(tcase, test_parse_version_command_with_valid_parameter);
    tcase_add_test(tcase, test_parse_error_command_with_valid_parameters);
	
    suite_add_tcase(suite, tcase);
    return suite;
}

