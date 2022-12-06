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

#include "parser.h"

#define MAX_ARGS 20

/*
 * Handles the exit or quit commands, by exiting the shell. Does not
 * return.
 *
 * Parameters:
 *   argc     The length of the argv vector
 *   argv[]   The argument vector
 */
int
builtin_exit(int argc, char *argv[])
{
  _exit(0); 
}


/*
 * Handles the author command, by printing the author of this program
 * to stdout
 *
 * Parameters:
 *   argc     The length of the argv vector
 *   argv[]   The argument vector, which is ignored
 *
 * Returns:
 *   0 on success, 1 on failure
 */
int
builtin_author(int argc, char *argv[])
{
  printf("Author: Okemawo Aniyikaiye Obadofin (OAO)\n");
  return 0;
}


/*
 * Handles the cd builtin, by setting cwd to argv[1], which must exist.
 *
 * Parameters:
 *   argc     The length of the argv vector
 *   argv[]   The argument vector, which must have either 1 or 2 arguments.
 *
 * Returns:
 *   0 on success, 1 on failure
 */
int
builtin_cd(int argc, char *argv[])
{
  if (argv[1] == NULL) {
    printf("no path supplied\n");
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
 * Parameters (which are all ignored):
 *   argc     The length of the argv vector
 *   argv[]   The argument vector
 *
 * Returns:
 *   Always returns 0, since it always succeeds
 */
int
builtin_pwd(int argc, char *argv[])
{
  char s[100];
  printf("%s\n", getcwd(s, 100));

  return 0;
}


/*
 * Process an external (non built-in) command, by forking and execing
 * a child process, and waiting for the child to terminate
 *
 * Parameters:
 *   argc      The length of the argv vector
 *   argv[]    The argument vector
 *
 * Returns:
 *   The child's exit value, or -1 on error
 */
int
forkexec_external_cmd(int argc, char *argv[])
{
  pid_t pid_child;

  pid_child = fork();
  
  if (pid_child == 0) {
    execvp(argv[0], argv);

  } else {
      int exit_status;
     
      waitpid(pid_child, &exit_status, 0);
      
      if (!(WIFEXITED(exit_status)))
        fprintf(stderr, "Child %d exited with status %d", pid_child, exit_status);
      
  }
  
  return 0;
}



/*
 * Parses one input line, and executes it
 *
 * Parameters:
 *   argc     The length of the argv vector, which must be >= 1
 *   argv[]   The argument vector
 */
void
execute_command(int argc, char *argv[])
{
  assert(argc >= 1);

    // Checks the first arguement to determine the command to call
    if (!strcmp(argv[0],"cd"))
      builtin_cd(argc, argv);

    else if (!strcmp(argv[0],"pwd"))
      builtin_pwd(argc, argv);

    else if (!strcmp(argv[0],"author"))
      builtin_author(argc, argv);

    else if (!strcmp(argv[0],"exit"))
      builtin_exit(argc, argv);
    
    else
      forkexec_external_cmd(argc, argv);
}


/*
 * The main loop for the shell.
 */
void mainloop()
{
  // welcome message
  fprintf(stdout, "Welcome to Plaid Shell Hommies!\n");

  char *input = NULL;
  char *argv[MAX_ARGS];
  int argc = 0;
  const char *prompt = "#> ";

  while (1) {
    input = readline(prompt);

    if (input == NULL)
      exit(0);
    if (*input == '\0')
      continue;

    argc = parse_input(input, argv, MAX_ARGS);

    if (argc == -1) { 
      // handle parsing error
      printf(" Error: %s\n", argv[0]);
      free(argv[0]);
    }
    else{
      // check for command to execute 
      execute_command(argc, argv);
    }

    // free all the malloc'd memory
    free(input);

    for (int i=0; i < argc; i++) {
      free(argv[i]);
    }
  }
}


int main(int argc, char *argv[])
{
  mainloop();
  return 0;
}
