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
 * This file is for unit testing tokens in the file mlle_parse_command.c.
 * Test frameworks is Check (http://check.sourceforge.net/).
 */


#include "check.h"
#include "mlle_parse_command.h"

// ---------------
// Test tokens.
// ---------------

/*
 * Test counting tokens from string with two values.
 */
START_TEST (test_count_tokens)
{
    size_t tokenCount = 2;
    char *tokens[3] = { NULL, NULL, NULL };
    char command[] = "ONE TWO\n";
	char *ptr = command;

    tokenCount = mlle_tokenize(ptr, tokens);

    // Check return values.
    ck_assert_str_eq(tokens[0], "ONE");
    ck_assert_str_eq(tokens[1], "TWO");
    ck_assert_int_eq(tokenCount,  2);
}
END_TEST


/*
 * Test counting tokens with empty string.
 */
START_TEST (test_count_empty_tokens)
{
    size_t tokenCount = 0;
    char *tokens[MLLE_PROTOCOL_MAX_TOKENS_PER_MSG] = { NULL, NULL, NULL };
    char *command = "";

    tokenCount = mlle_tokenize(command, tokens);

    // Check return values.
    ck_assert_int_eq(tokenCount, 0);
}
END_TEST


/*
 * Test counting tokens if string contains to many values.
 */
START_TEST (test_count_to_many_tokens)
{
    size_t tokenCount = 0;
    char *tokens[MLLE_PROTOCOL_MAX_TOKENS_PER_MSG] = { NULL, NULL, NULL };
    char command[] = "ONE TWO THREE FOUR\n";
	char *ptr = command;

    tokenCount = mlle_tokenize(ptr, tokens);

    // Check return values.
    ck_assert_str_eq(tokens[0], "ONE");
    ck_assert_str_eq(tokens[1], "TWO");
    ck_assert_str_eq(tokens[2], "THREE");
    ck_assert_int_eq(tokenCount, MLLE_PROTOCOL_TOO_MANY_TOKENS);
}
END_TEST

/*
 * Test counting tokens with a string with format: <command>.
 */
START_TEST (test_count_one_token)
{
    size_t tokenCount = 0;
    char *tokens[MLLE_PROTOCOL_MAX_TOKENS_PER_MSG] = { NULL, NULL, NULL };
    char command[] = "LICENSEINFO\n";
	char *ptr = command;

    tokenCount = mlle_tokenize(ptr, tokens);

    // Check return values.
    ck_assert_int_eq(tokenCount, 1);
}
END_TEST


/*
 * Test counting tokens with a string with format: <command> <number>NL.
 */
START_TEST (test_count_two_tokens)
{
    size_t tokenCount = 0;
    char *tokens[MLLE_PROTOCOL_MAX_TOKENS_PER_MSG] = { NULL, NULL, NULL };
    char command[] = "VERSION 2\n";
	char *ptr = command;

    tokenCount = mlle_tokenize(ptr, tokens);

    // Check return values.
    ck_assert_int_eq(tokenCount, 2);
}
END_TEST

/*
 * Test counting tokens with a string with format: <command> <number>NL<data>.
 */
START_TEST (test_count_two_tokens_with_data)
{
    size_t tokenCount = 0;
    char *tokens[MLLE_PROTOCOL_MAX_TOKENS_PER_MSG] = { NULL, NULL, NULL };
    char command[] = "PUBKEY 775\nThis is the public key.";
	char *ptr = command;

    tokenCount = mlle_tokenize(ptr, tokens);

    // Check return values.
    ck_assert_int_eq(tokenCount, 2);
}
END_TEST

/*
 * Test counting tokens with a string with format: <command> <number> <length>NL<data>.
 */
START_TEST (test_count_three_tokens_with_data)
{
    size_t tokenCount = 0;
    char *tokens[MLLE_PROTOCOL_MAX_TOKENS_PER_MSG] = { NULL, NULL, NULL };
    char command[] = "ERROR 54 340\nThis is the error message.";
	char *ptr = command;

    tokenCount = mlle_tokenize(ptr, tokens);

    // Check return values.
    ck_assert_int_eq(tokenCount, 3);
}
END_TEST


Suite* suite_tokens (void)
{
    Suite *suite = suite_create("test_tokens");
    TCase *tcase = tcase_create("tokens");	   
	tcase_add_test(tcase, test_count_tokens);	    
	tcase_add_test(tcase, test_count_empty_tokens);
	tcase_add_test(tcase, test_count_to_many_tokens); 
	tcase_add_test(tcase, test_count_one_token);
    tcase_add_test(tcase, test_count_two_tokens);
    tcase_add_test(tcase, test_count_two_tokens_with_data);
    tcase_add_test(tcase, test_count_three_tokens_with_data);

    suite_add_tcase(suite, tcase);
    return suite;
}
