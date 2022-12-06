/*
 * test_tokenize.c
 * 
 * Test functions for tokenize.c
 *
 * Author: Howdy Pierce <howdy@cardinalpeak.com>
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

#define MAX_ARGS 20


/*
 * Tests the read_word function
 *
 * Returns:
 *   True if all test cases pass, false otherwise.
 */
static bool
ilse_test_read_word()
{
  typedef struct {
    const char *input;
    const char *exp_word;
    const int exp_pos;
  } test_matrix_t;

  char word_buf[30];
  test_matrix_t tests[] =
    {
      // from assignment writeup
      {"   echo ", "echo", 7},
      {"grep", "grep", 4},
      {"echo one two three", "echo", 4},
      {"", "", 0},
      {"  ", "", 2},
      {"\"one two\" three", "one two", 9},
      {"One\\ Two Three", "One Two", 8},
      {"ec\"ho\" HELLO", "echo", 6},
      {"echo\\ ", "echo ", 6},
      {"echo\\\\ ", "echo\\", 6},
      {"echo\t", "echo", 4},
      {"function() one", "function()", 10},
      {" \"\\\"\"  ", "\"", 5},
      {"\\\"", "\"", 2},

      // other worthwhile test cases
      {" ", "", 1},
      {" \n", "", 2},
      {"  ", "", 2},
      {"\t\n\r   ", "", 6},

      {"     12.34", "12.34", 10},
      {"\techo", "echo", 5},
      {"\t \techo", "echo", 7},
      {"echo", "echo", 4},
      {"echo   ", "echo", 4},
      {"echo\n", "echo", 4},
      {"\"one two\"  ", "one two", 9},
      {"\\\"one\\ two\\\"  ", "\"one two\"", 12},
      {"\\\"one two\\\"  ", "\"one", 5},
      {" two\\\"  ", "two\"", 6},
      {" ec\"ho\"  ", "echo", 7},
      {"ec\"ho \"  ", "echo ", 7},
      {"  echo\\t   ", "echo\t", 8},
      {"  echo\\n   ", "echo\n", 8},
      {"  echo\\r   ", "echo\r", 8},
      {"  echo\\\\   ", "echo\\", 8},
      {"  echo\\   ", "echo ", 8},
      {"  echo\\\"   ", "echo\"", 8},
      {"  echo\\g  ", "Illegal escape character: g", -1},
      {"  \"\\t\"   ", "\t", 6},
      {"  \"\\n\"   ", "\n", 6},
      {"  \"\\r\"   ", "\r", 6},
      {" \\$", "$", 3},
      {"x\\n\\t\\r\\\\\\ \\\"   ", "x\n\t\r\\ \"", 13},
      {" supercalifragilisticexpialidocious ", "Word too long", -1},
      {" snake_case ", "snake_case", 11},
      {" CamelCase@20", "CamelCase@20", 13},
      {"\tfunction() one", "function()", 11},

      // unterminated quotes
      {"\"unterminated quote!", "Unterminated quote", -1},
      {"untermin\"ated quote!", "Unterminated quote", -1},
      {"\"untermina\\\"ted quote!", "Unterminated quote", -1},
      {"unterminated_quote!\"", "Unterminated quote", -1},

      // no variable expansion yet
      {"$FOO", "$FOO", 4},
      {"x$FOO", "x$FOO", 5},
      {"\\$FOO", "$FOO", 5},
      {"\"\\$FOO\"", "$FOO", 7}
    };
  const int num_tests = sizeof(tests) / sizeof(test_matrix_t);
  int tests_passed = 0;

  for(int i=0; i<num_tests; i++) {

    int act_pos = read_word(tests[i].input, word_buf, sizeof(word_buf));
    if (act_pos == tests[i].exp_pos && strcmp(word_buf, tests[i].exp_word) == 0) {
      tests_passed++;
    } else {
      printf("  FAILED: read_word(\"%s\" ...) returned %d, \"%s\"\n",
        tests[i].input, act_pos, word_buf);
    }
    
  }

  printf("%s: PASSED %d/%d\n", __FUNCTION__, tests_passed, num_tests);
  return (tests_passed == num_tests);
}


static int num_parser_tests=0;

/*
 * Tests one test case of the parse_input function. 
 *
 * Parameters:
 *   teststring       Input line to be tested
 *   exp_argc         Expected value of argc
 *   argv, argv, ...  NULL terminated list of expected output arguments
 * 
 * Returns:
 *   True if test passes, false otherwise. 
 */
static bool
test_parser_once(const char *teststring, int exp_argc, ...)
{
  va_list valist;

  bool result = false;
  char *argv[MAX_ARGS];
  int argc;

  num_parser_tests++;

  argc = parse_input(teststring, argv, MAX_ARGS);

  if (argc != exp_argc) {
    printf("Error on test string [%s]: act_argc=%d expected=%d\n", teststring,
        argc, exp_argc);
    goto end;
  }

  if (argc == -1)
    argc = 1;
  
  va_start(valist, exp_argc);
  for (int i=0; i < argc; i++) {
    const char *exp_arg = va_arg(valist, const char *);
    if (strcmp(argv[i], exp_arg) != 0) {
      printf("Error on test string [%s]: argv[%d]=%s, expected=%s\n", teststring,
          i, argv[i], exp_arg);
      goto end;
    }
  }

  result = true;

 end:
  if (argc == -1) {
    free(argv[0]);
  } else {
    for (int i=0; argv[i] != NULL; i++) 
      free(argv[i]);
  }

  return result;
}


/*
 * Tests the parse_input function
 *
 * Returns:
 *   True if all test cases pass, false otherwise.
 */
static bool
ilse_test_parse_input()
{
  int passed = 0;

  passed += test_parser_once("", 0);
  passed += test_parser_once("   ", 0);
  passed += test_parser_once("   \n", 0);
  passed += test_parser_once("echo this is a test", 5, "echo",
      "this", "is", "a", "test");
  passed += test_parser_once("echo  this is  a test   ", 5,
      "echo", "this", "is", "a", "test");
  passed += test_parser_once("echo \tthis  is\ta\ntest   \n", 5,
      "echo", "this", "is", "a", "test");
  passed += test_parser_once("echo one\\n two\\r", 3, "echo",
      "one\n", "two\r");
  passed += test_parser_once("echo \"one two\"", 2, "echo",
      "one two");
  passed += test_parser_once("echo one\\ two", 2, "echo",
      "one two");
  passed += test_parser_once("echo \\\"one   two", 3, "echo",
      "\"one", "two");
  passed += test_parser_once("one \"two three\" four", 3, "one",
      "two three", "four");
  passed += test_parser_once("\\n\\t\\r\\\\\\ \\\"#   ", 1,
      "\n\t\r\\ \"#");

  // test near max arguments
  passed += test_parser_once("grep 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18",
      19, "grep", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13",
      "14", "15", "16", "17", "18", NULL);
  passed += test_parser_once("grep 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19",
      -1, "Too many arguments");

  // Unterminated quote
  passed += test_parser_once("grep \"unterminated ", -1, "Unterminated quote");


  printf("%s: PASSED %d/%d\n", __FUNCTION__, passed, num_parser_tests);
  return (passed == num_parser_tests);
}


int main(int argc, char *argv[])
{
  int success = 1;

  success &= ilse_test_read_word();
  success &= ilse_test_parse_input();

  if (success) {
    printf("Excellent work! All tests succeeded!\n");
    return 0;
  } else {
    printf("NOTE: FAILURES OCCURRED\n");
    return 1;
  }
}
