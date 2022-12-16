/*
 * plaidsh.c
 *
 * A small shell
 *
 * Author: Okemawo Aniyikaiye Obadofin (OAO)
 */

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "parser.h"

#define MAX_ARGS 20

/*
 * Handles the exit or quit commands, by exiting the shell. Does not
 * return.
 * Parameters:
 *   command_ t cmd:
 *      argv - Arguement vector
 *      argc - Length of Arguement Vector
 */
  int
builtin_exit(command_t *cmd)
{
  _exit(0); 
}


/*
 * Handles the author command, by printing the author of this program
 * to stdout
 *
 * Parameters:
 *   command_ t cmd:
 *      argv - Arguement vector
 *      argc - Length of Arguement Vector
 *
 * Returns:
 *   0 on success, 1 on failure
 */
  int
builtin_author(command_t *cmd)
{
  printf("Author: Okemawo Aniyikaiye Obadofin (OAO)\n");
  return 0;
}


/*
 * Handles the cd builtin, by setting cwd to argv[1], which must exist.
 *
 * cd <path>
 * 
 * Parameters:
 *   command_ t cmd:
 *      argv - Arguement vector
 *      argc - Length of Arguement Vector
 *
 * Returns:
 *   0 on success, 1 on failure
 */
  int
builtin_cd(command_t *cmd)
{
  // Retrieve arguement vector and count from command_t struct
  int argc = command_get_argc(cmd);
  char * const *argv = command_get_argv(cmd);

  // Defaults to home directory when no arguement is supplied 
  if (argv[1] == NULL) {
    chdir(getenv("HOME"));  //chdir system call
    return 0;
  }  

  for (int i=1; i < argc; i++) {
    chdir(argv[i]);
  }
  return 0;
}


/*
 * Handles the pwd builtin, by printing the cwd to the supplied file
 * descriptor
 *
 * pwd <> 
 *
 * Parameters:
 *   command_ t cmd:
 *      argv - Arguement vector
 *      argc - Length of Arguement Vector
 *
 * Returns:
 *   Always returns 0, since it always succeeds
 */
  int
builtin_pwd(command_t *cmd)
{
  char s[100];
  printf("%s\n", getcwd(s, 100));

  return 0;
}

/*
 * Sets an enviroment variable to a value by using the setenv() function
 * 
 * setenv <varname> <valname >
 *
 * Parameters:
 *   command_ t cmd:
 *      argv - Arguement vector
 *      argc - Length of Arguement Vector
 * 
 * Returns:
 *   Always returns 0 on success
 *   Returns -1 on failure
 */
int
builtin_setenv(command_t *cmd) {

  // Retrieve arguement vector and count from command_t struct
  char * const *argv = command_get_argv(cmd);
  int argc = command_get_argc(cmd);

  // Returns -1 and prints error if the arguements supplied are less than 3
  if (argc < 3 ) {
    fprintf(stderr, "Incomplete Arguements\n");
    return -1;
  }
  char *value = argv[1];

  // Check characters to avoid using an illegal variable name
  while(*value) {
    if (isalnum(*value) || *value == '_') {
      value++;
      continue;
    } else {
      fprintf(stderr, "Illegal variable name: <%s> \n", argv[1]);
      return -1;
    }
    value++;
  }
  // Sets enviroment variable
  setenv(argv[1], argv[2], 1);
  return 0;
}


/*
 * Process an external (non built-in) command, by forking and execing
 * a child process, and waiting for the child to terminate
 *
 * Parameters:
 *   command_ t cmd:
 *      argv - Arguement vector
 *      argc - Length of Arguement Vector
 *
 * Returns:
 *   The child's exit value, or -1 on error
 */
  int
forkexec_external_cmd(command_t *cmd)
{
  // Retrieve arguement vector
  char * const *argv = command_get_argv(cmd);

  pid_t pid_child;

  pid_child = fork();

  // if successfully forked, launch exec command
  if (pid_child == 0) {
    execvp(argv[0], argv);

  } else {
    int exit_status;

    waitpid(pid_child, &exit_status, 0);

    if (!(WIFEXITED(exit_status)))
      fprintf(stderr, "Child %d exited with status %d\n", pid_child, exit_status);

  }

  return 0;
}



/*
 * Parses one input line, and executes it
 *
 * Parameters:
 *   command_ t cmd:
 *       argc - The length of the argv vector, which must be >= 1
 *       argv - Arguement Vector
 */
  void
execute_command(command_t *cmd)
{
  // Retrieve arguement vector and arguement count 
  int argc = command_get_argc(cmd);
  char * const *argv = command_get_argv(cmd);

  // Verify that number of arguements are more than 1
  assert(argc >= 1);
  
  // Check if the ouput file is set not set to null before changing STDOUT
  if (command_get_output(cmd) != NULL) {
    int create_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    int fd = open(command_get_output(cmd), O_RDWR | O_CREAT | O_TRUNC, create_mode);

    dup2(fd, STDOUT_FILENO);
    close(fd);
  }
  // Checks if the input file is not set to null to changing STDIN
  if (command_get_input(cmd) != NULL) {
    int fd = open(command_get_input(cmd), O_RDONLY | O_CLOEXEC);

    dup2(fd, STDIN_FILENO);
    close(fd);
  }

  // Checks the first arguement to determine the command to call
  if (!strcmp(argv[0],"cd"))
    builtin_cd(cmd);

  else if (!strcmp(argv[0],"pwd"))
    builtin_pwd(cmd);

  else if (!strcmp(argv[0],"author"))
    builtin_author(cmd);

  else if (!strcmp(argv[0],"exit"))
    builtin_exit(cmd);

  else if (!strcmp(argv[0],"setenv"))
    builtin_setenv(cmd);

  else
    forkexec_external_cmd(cmd);
}


/*
 * The main loop for the shell.
 */
void mainloop()
{
  // welcome message
  fprintf(stdout, "Welcome to Plaid Shell Hommies!\n");

  char *input = NULL;
  char err_msg[512];

  const char *prompt = "plaid-shell#> ";

  while (1) {
    input = readline(prompt);
    add_history(input);

    if (input == NULL)
      exit(0);
    if (*input == '\0')
      continue;
    
    // parse the imput stream
    command_t *cmd = parse_input(input, err_msg, sizeof(err_msg));


    if (cmd == NULL) { 
      // handle parsing error
      printf(" Error: %s\n", err_msg);
    }
    else{
      // check for command to execute 
      execute_command(cmd);
    }

    // free all the malloc'd memory
    free(input);

    command_free(cmd);
  }
}


int main(int argc, char *argv[])
{
  mainloop();
  return 0;
}
