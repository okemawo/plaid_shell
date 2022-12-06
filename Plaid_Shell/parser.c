/*
 * parser.c
 *
 * Code to parse and tokenize a command line
 *
 * Author: Okemawo Aniyikaiye Obadofin (OAO)
 */
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

/*
 * Documented in .h file
 */
int
read_word(const char *input, char *word, size_t word_len)
{
  assert(input);
  assert(word);

  const char *in = input;
  char *w = word;
  bool in_quote = false;

  // comsume any leading whitespace
  while (isspace(*in))
    in++;

  while(*in) {
    
    // breaks loop of the next character is a space and inquote is false
    if (isspace(*in) && !in_quote) {
      break;

    } else if (*in == '"') {
      // handle double quote
      in_quote = !in_quote;
      in++;

    } else if (*in == '\\') {
      // handle escape character
      switch ( *(in+1) ) {
      case 'n':
        *w++ = '\n';
        break;

      case 'r':
        *w++ = '\r';
        break;

      case 't':
        *w++ = '\t';
        break;

      case '\"':
        *w++ = '\"';
        break;

      case '\\':
        *w++ = '\\';
        break;

      case ' ':
        *w++ = ' ';
        break;

      case '$':
        *w++ = '$';
        break;

      default:     // illegal escape character
        sprintf(word, "Illegal escape character: %c", *(in+1));
        return -1;
      }

      in += 2;

    } else {
      // just add character to word
      *w++ = *in++;
    }

    // handle the error case: word is too long
    if (w >= word + word_len) {
      strcpy(word, "Word too long");
      return -1;
    }
  }

  *w = '\0';

  if (in_quote) {
    strcpy(word, "Unterminated quote");
    return -1;
  }

  return in - input;
}

/*
 * Documented in .h file
 */
int
parse_input(const char *input, char *argv[], int argv_max)
{
  int argc = 0;
  memset(argv, 0, argv_max);

  assert(input);

  int char_count = 0;
  char *word = NULL;
  int i = 0;
  const char *inp = input;

  while(*inp) {
    // consume leading white spaces
    while (isspace(*inp))
      inp +=1;

    if (*inp =='\0') {
      break;
    }
    
    // create the dynamic memory for word using malloc
    word = (char *) malloc(sizeof(char) * 25);
    
    // save the count of characters processed from the readword result
    char_count = read_word(inp, word, 25);

    // check for an error of unterminated quote form readword
    if (!strcmp(word, "Unterminated quote")) {
      strcpy(argv[0], "Unterminated quote");
      argc = -1;
      break;
    }
    
    // shift pointer position to new location 
    inp += char_count;
    argc += 1;

    // check if the maximum arguements has been reached 
    if (argv_max <= argc) {
      strcpy(argv[0], "Too many arguments");
      argv[++i] = NULL;
      argc = -1;
      break;
    }
    else {
      argv[i++] = word;
    }
      argv[i] = NULL;
  }
  return argc;
}
