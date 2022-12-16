# Plaid Shell (Update Version 2 16/12/2022)

<br/>

## Name : Plaid Shell (A Minimalistic Shell very similar to bash)
## Author : Okemawo Aniyikaiye Obaodfin (OAO)
## Date : 19th November 2022
#### Description : plaidsh is like other shells, it can run any program on the system, including important commands such as ls, echo, and grep.

#### Build Instructions : After copying the C file to your terminal, use the "make" command to compile the c programs and generate a 'plaidsh' executable file. Run the executable to initialze the shell. Alternatively, an already generated plaidsh executable can be found in this repository and ran directly without the need for calling "make". 

<br/>

#### 🪢 The functions that are used to make the shell operational are enumerated below.

#### 1. Read Word: Returns the first word from input, removing leading whitespace, handling double quotes, and translating escaped character. When called, word points to a buffer of word_len length. The read_word function places the translated word from input into the word buffer and returns the number of characters that were processed from input.
V2 Update : Adds support for recognizing redirection characters

  
   Examples:
  
    "   echo "           "echo"      returns 7
    "one two" three"     "grep"      returns 4
    "echo\ "             "echo "     returns 6

<br/>


#### 2. Parse Input: Parses an input line into an argv vector by segmenting the input into words that are bounded by unquoted whitespace. Double quotes are used to group words and are eliminated from the input. On success, returns the count of arguments processed (argc). In this case, each word will be returned in malloc’d memory, which must be explicitly freed by the caller. On failure, returns -1.
V2 update : Adds support for globbing and redirection 
 
   Examples:
   
     echo one "two three" four      "echo" "one" "two three" four      Returns 4

<br/>  

#### 3. Command_t Struct: command data structure is documented in the command.h file. There are some available functions implemented with the command structure, these can be invsetigated in the command.c or command.h files.

<br/>


#### 🪢 The Builtin functions that are used in the shell are enumerated below, along with their signatures. These functions can be called from plaid shell prompt and perfrom thesame functions as their aliases in bash.

####     1. exit : int builtin_exit(int argc, char *argv[]);
 
####     2. author : int builtin_author(int argc, char *argv[]);

####     3. cd : int builtin_cd(int argc, char *argv[]);

####     4. pwd : int builtin_pwd(int argc, char *argv[]);

####     5. setevn : int builtin_setenv(const char varname, const char valname)
 
 
