# Plaid Shell

<br/>

## Name : Plaid Shell (A Minimalistic Shell very similar to bash)
## Author : Okemawo Aniyikaiye Obaodfin (OAO)
## Date : 19th November 2022
#### Description : plaidsh is like other shells, it can run any program on the system, including important commands such as ls, echo, and grep.

#### Build Instructions : After copying the C file to your terminal, use the "make" command to compile the c programs and generate a 'plaidsh' executable file. Run the executable to initialze the shell. Alternatively, an already generated plaidsh executable can be found in this repository and ran directly without the need for calling "make". The functions that are used to make the shell operational are enumerated below.

<br/>


#### 1. Reverse in place: Reverses a null-terminated string in place, converting it to all lower case in the process.
  
   Examples:
  
    "Carnegie Mellon" "nollem eigenrac"
    "Four"            "ruof"
    " One"            "eno "
  

<br/>


#### 2. Reverse by word: Individually reverses each word of a null-terminated string in place. Whitespace characters (as identified by the C isspace() function) are passed through unchanged.
 
   Examples:
   
     Carnegie␣Mellon            eigenraC␣nolleM
     \tCarnegie␣␣Mellon         \teigenraC␣␣nolleM
     Four                       ruoF
     ␣One                       ␣enO
     My␣heart␣is␣in␣␣the␣work   yM␣traeh␣si␣ni␣␣eht␣krow

<br/>  

#### 3. Is prefix: Returns true if str begins with prefix, and false otherwise. Both prefix and str are null-terminated strings. Note the empty string “” is a valid prefix for all strings.
 
 
  Examples:
  
    prefix           str                 Returns
    C                Carnegie␣Mellon     true
    Carnegie         Carnegie␣Mellon     true
    Carnegie␣Mellon  Carnegie␣Mellon     true
    Cab              Carnegie␣Mellon     false
  

<br/>  


#### 4. Remove Last Substring: Removes the last occurrence of substr from str, modifying the result in place. Returns the character position where the removal occurred, or -1 if substr was not found in str. Note that substr need not be a full word.
 
 
   Examples:
  
    str                  substr              Result              Returns
    Carnegie␣Mellon      Carnegie␣           Mellon              0
    Carnegie␣Mellon      Kiltie              Carnegie␣Mellon     -1
    Carnegie␣Mellon      “”                  Carnegie␣Mellon     14
    one␣two␣one␣three    one␣                one␣two␣three       8
    one␣two␣one␣three    hr                  one␣two␣one␣tee     13
  

<br/>  

 

#### 5. First Word: Returns the first word from input, removing leading whitespace. We will built upon this function in a subsequent assignment as we write our own Linux shell. When called, word points to a buffer of word_len length. After skipping leading whitespace, the first_word function copies the first word from input into the word buffer and returns the number of characters that were processed from input. In the case where input holds more than one word, it is possible to read the next word by calling first_word again with the pointer input+return_value. In the case that the word buffer is not long enough, first_word places the error message “Word too long” into the buffer and returns -1.
 
 
   Examples:
  
    input                    Result (in word)                    Returns
    ␣␣␣echo␣                 echo                                7
    grep                     grep                                4
    \techo␣one␣two␣three     echo                                5
    (empty string)           (empty string)                      0

<br/>  
