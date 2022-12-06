/*
 * parser.h
 *
 * Code to parse and tokenize a command line
 *
 * Author: Howdy Pierce <howdy@cardinalpeak.com>
 */

#ifndef _PARSER_H_
#define _PARSER_H_

/*
 * Returns the first word from input, removing leading whitespace,
 * handling double quotes, and translating escaped characters.
 *
 * When called, word points to a buffer of word_len length. The
 * read_word function places the translated word from input into the
 * word buffer and returns the number of characters that were
 * processed from input.  In the case where input holds more than one
 * word, it is possible to read the next word by calling read_word
 * again with the pointer input+return_value.
 *
 * Normally, a word ends with unescaped whitespace.
 *
 * However, if an unescaped double quote is encountered, then the
 * characters from that double quote up to the next double quote are
 * included in the word, regardless of whether there is whitespace
 * between the quotes. The quotes themselves are removed.  If input
 * contains an unterminated quote, read_word places the error message
 * “Unterminated quote” into the word buffer and returns -1.
 *
 * The function converts escape sequences as follows:
 *    \n        newline
 *    \t        tab
 *    \r        carriage return
 *    \(space)  a literal space, which does not cause a word break
 *    \"        a literal double quote
 *    \\        a literal backslash
 *    \$        a literal dollar sign (does not start a variable)
 *
 * If an escape sequence other than those listed is encountered, the
 * function places the error message “Illegal escape character:
 * <char>” in the word buffer and returns -1.
 *
 * In the case that the word buffer is not long enough, read_word
 * places the error message “Word too long” into the buffer and
 * returns -1.
 *
 * Some examples:
 *
 *   'echo'             -> 'echo', returns 4
 *   '   echo'          -> 'echo', returns 7
 *   '   echo   '       -> 'echo', returns 7
 *   '"New York"'       -> 'New York', returns 10
 *   ' New\ York'       -> 'New York', returns 10
 *   '"echo"'           -> 'echo', returns 6
 *
 * Parameters:
 *   input     Unprocessed input line, which must be null terminated
 *   word      Buffer to be filled in with the word read
 *   word_len  Size of word buffer
 *
 * Returns:
 *   The number of characters from input that were consumed. Parsing
 *   for the next word can start at (input + return_value). Of course,
 *   that may be at the end of input if the entire thing was consumed.
 *
 *   In case of error, returns -1 and places an error message in
 *   word buffer.
 */
int read_word(const char *input, char *word, size_t word_len);


/*
 * Parses an input line into an argv vector by segmenting the input
 * into words that are bounded by unquoted whitespace. Double quotes
 * are used to group words and are eliminated from the input. For
 * instance, the line
 *       echo one "two three" four
 * is parsed into the four words "echo", "one", "two three", and
 * "four".
 *
 * A single backslash is an escape character and results in
 * substitutions as described in the read_word() documentation.
 *
 * Parameters:
 *   input      Input line as typed by the user
 *   argv       Vector to be filled out
 *   argv_max   Size of the argv vector
 *
 * Returns:
 *
 *    On success, returns the count of arguments processed (argc). In
 *    this case, each word will be returned in malloc’d memory, which
 *    must be explicitly freed by the caller.
 *
 *    On failure, returns -1. In this case, an error message string is
 *    placed in argv[0], which will have been malloc’d. This memory
 *    must be explicitly freed by the caller.
 */
int parse_input(const char *input, char *argv[], int argv_max);

#endif /* _PARSER_H_ */
