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
#include <unistd.h>
#include <glob.h>
#include "parser.h"
#include "command.h"


/*
 * Documented in .h file
 */
int
read_word(const char *input, char *word, size_t word_len)
{
  assert(input);
  assert(word);

  const char *in = input;
  
  char env[64];
  char *en = env;

  char var[64];
  char *va = var; 

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

        case '>':
          *w++ = '>';
          break;


        case '<':
          *w++ = '<';
          break;

        default:     // illegal escape character
          sprintf(word, "Illegal escape character: %c", *(in+1));
          return -1;
      }

      in += 2;
    
    // Handles variable expansion
    } else if (*in == '$') {
      
      in++;
      
      // Check if it is a valid variable expansion and copies it to a temporary variable env
      while(isalnum(*in) || *in == '_'){
        *en++ = *in++;
      }
      *en = '\0';
      
      // Print error when enviroment varible is not found
      if (getenv(env) == NULL) {
        sprintf(word, "Undefined variable: '%s'", env);
        return -1;

      } else {
        // Copy enviroment varible to temporary variable after expanding it
        strcpy(var, getenv(env));

        // Copy Enviroment variable to word 
        while(*va){
          *w++ = *va++;
        }
      }

      // Handle case of output redirection character
    } else if (*in == '>' && !in_quote) {
      // Detach character from previous character
      if(isalnum(*(in-1))){
        break;
      }
      *w++ = *in++;

      //clean spaces
      while(isspace(*in))
        in++;
      
      // Check if there was a file after the output redirection character
      if (*in == '\0' || isspace(*in)){
        sprintf(word, "Redirection without filename");
        return -1;
      }
      
      // Copies the output redirection file to the word buffer
      while(*in && *in != '<' && *in != '>' && *in != '$' && !isspace(*in)){
        *w++ = *in++;
      }
  
      // Handles input redirection character
    } else if (*in == '<' && !in_quote) {
      // Detach character from previous word
      if(isalnum(*(in-1))){
        break;
      }
      *w++ = *in++;

      // Clean Spaces
      while(isspace(*in))
        in++;
      
      // Check if there was a file after the input redirection
      if (*in == '\0' || isspace(*in)){
        sprintf(word, "Redirection without filename");
        return -1;
      }
      
      // Copies the output redirection file to the word buffer
      while(*in && *in != '>' && *in != '<' && *in != '$' && !isspace(*in)){
        *w++ = *in++;
      }

    } else {
      // clean spaces 
      while(isspace(*in) && !in_quote)
        in++;
      // just add character to word
      *w++ = *in++;
    }
    // handle the error case: word is too long
    if (w >= word + word_len) {
      strcpy(word, "Word too long");
      return -1;
    }
  }

  // Add the null terminating character
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
command_t *
parse_input(const char *input, char *err_msg, size_t err_msg_len)
{
  int chars_read = 0;
  
  // word buffer
  char word[512];
  char *w = word;

  // Glob Struct for globbing
  glob_t globst;
  int ret_glob;
  
  command_t *cmd = command_new();

  while (1) {
    chars_read = read_word(input, word, sizeof(word));
    input += chars_read;
    
    // Checks for error return value and copies error to err_msg
    if (chars_read == -1) {
      command_free(cmd);
      strncpy(err_msg, word, err_msg_len);
      return NULL;
    }
  
    if (chars_read == 0)        // end of input string
      break;

    if (word[0] == '\0') {       // whitespace only 
      break;

      // Handle setting of input redirection file
    } else if (word[0] == '<') {
      w = word;
      w++;
      
      //  Checks if the input has already been set, returns an error if true
      if (command_get_input(cmd) != NULL) {
        command_free(cmd);
        strncpy(err_msg, "Multiple redirections not allowed", err_msg_len);
        return NULL;
      }
      command_set_input(cmd, w); // Set input file

      // Handle setting of output redirection file
    } else if (word[0] == '>') {
      w = word;
      w++;


      //  Checks if the output has already been set, returns an error if true
      if (command_get_output(cmd) != NULL) {
        command_free(cmd);
        strncpy(err_msg, "Multiple redirections not allowed", err_msg_len);
        return NULL;
      }
      command_set_output(cmd, w); // Set output file
   
      // Handle Globbing
    } else if (!(word[1] == '>' && word[0] == '>') || !(word[1] == '<' && word[0] == '<')){
      
      // Globs for general matches
      ret_glob = glob(word, GLOB_NOCHECK, NULL, &globst);

      // Glob for tilde
      if (word[0] == '~') {
        ret_glob = glob(word, GLOB_TILDE_CHECK, NULL, &globst);
      } 
      //Glob for braces
      if (word[0] == '{') {
        ret_glob = glob(word, GLOB_BRACE, NULL, &globst);
      }
      
      // Appends arguements when match not found
      if( ret_glob == GLOB_NOMATCH ) {
        command_append_arg(cmd, word);
      }
      
      // Appends arguements in glob vector to command arguement vector
      for (int x = 0; x < globst.gl_pathc; x++) {
        if (word[strlen(word) - 1] == '/') {
          command_append_arg(cmd, word);
          break;
        }
        else {
          command_append_arg(cmd, globst.gl_pathv[x]);
        }
      }
      globfree(&globst); // free glob call

    } else{
      w = word;
      command_append_arg(cmd, w);
    }

    if (command_get_argc(cmd) == 0) {
      strncpy(err_msg, "Missing command", err_msg_len);
      return NULL;
    }
  }

  return cmd;
}
