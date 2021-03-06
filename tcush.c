//*********************************************************
//
// PUT YOUR NAME HERE!!!!
// Operating Systems
// Project #1: Writing Your Own Shell: tcush
//
//*********************************************************

//*********************************************************
//
// Includes and Defines
//
//*********************************************************
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "main.h"

#define STRMYQUIT "myquit"

//*********************************************************
//
// Global variables
//
//*********************************************************
struct history_node *history_list = NULL;

//*********************************************************
//
// Main Function
//
//*********************************************************
int main(int argc, char *argv[]) {
  int ii;
  char **toks;
  int retval;
  int input;
  int output;
  char *prompt;
  char *username;
  char *output_filename;
  char *input_filename;

  if ((username = getlogin()) == NULL) {
    fprintf(stderr, "Get of user information failed.\n"); exit(1);
  } else {
    prompt = strcat(username, ", don't break the shell > ");
  }

  /* initialize local variables */
  ii = 0;
  toks = NULL;
  retval = 0;

  /* put signal catching functions here */
  // signal(SIGINT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);

  while( true ) {
    printf("%s", prompt);

    toks = gettoks(); // get arguments
    if (toks[0] == NULL) continue; // No input
    if (strcmp(toks[0], STRMYQUIT) == 0) break; // Dr. Scherger wants out!

    // Handle internal commands
    if (handleInternal(toks)) {
      continue;
    }

    // Check redirected input
    input = redirect_input(toks, &input_filename);

    switch(input) {
    case -1:
      printf("Syntax error!\n");
      continue;
      break;
    case 0:
      break;
    case 1:
      printf("Redirecting input from: %s\n", input_filename);
      break;
    }

    // Check redirected output
    output = redirect_output(toks, &output_filename);

    switch(output) {
    case -1:
      printf("Syntax error!\n");
      continue;
      break;
    case 0:
      break;
    case 1:
      printf("Redirecting output to: %s\n", output_filename);
      break;
    }

    push_command(&history_list, join_tokens(toks));
    handleExternal(toks, input, input_filename, output, output_filename);

    // simple loop to echo all arguments
	  // for( ii=0; toks[ii] != NULL; ii++ ) {
    //   printf( "Argument %d: %s\n", ii, toks[ii] );
    // }
  }

  /* return to calling environment */
  return(retval);
}

bool handleInternal(char **toks) {
  bool isInternal = true;
  char* command = toks[0];

  if (strcmp(command, "history") == 0) {
    print_command(history_list);
  }  else if (strcmp(command, "forweb") == 0) {
    forweb(toks);
  } else if (strcmp(command, "nls") == 0) {
    nls(toks);
  } else if (strcmp(command, "fil") == 0) {
    fil(toks);
  } else {
    isInternal = false;
  }

  return isInternal;
}

void handleExternal(char **toks, int input, char *input_filename, int output,
                    char *output_filename) {
  int pid = fork();

  // when can't fork more processes
  if (pid < 0) {
    printf("Fork error!!!\n");
    return;
  }

  if (pid == 0) {
    // set up redirections
    if (input) freopen(input_filename, "r", stdin);
    if (output) freopen(output_filename, "w+", stdout);

    excuteCommand(toks);
    exit(-1);
  }

  if (!isBackground(toks) && pid > 0) {
    int status;
    waitpid(pid, &status, 0);
  }
}

bool isBackground(char **toks) {
  int i;
  bool isInBackground = false;

  // get the last argument
  for (i = 0; toks[i] != '\0'; i++);
  char *last_command = toks[--i];

  // check the flag. If there is an & in the end, remove the &
  if (strcmp(last_command, "&") == 0 && i >= 1) {
    toks[i] = NULL;
    isInBackground = true;
  }

  return isInBackground;
}

void excuteCommand(char **toks) {
  execvp(toks[0], toks);
  printf("%s -- Command not found\n", toks[0]);
}
