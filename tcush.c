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
int main( int argc, char *argv[] )
{
  /* local variables */
  int ii;
  char **toks;
  int retval;
  char *prompt;
  char *username;
  

  if ((username = getlogin()) == NULL) {
    fprintf(stderr, "Get of user information failed.\n"); exit(1);
  } else {
    prompt = strcat(username, ", don't break the shell > ");
  }

  /* initialize local variables */
  ii = 0;
  toks = NULL;
  retval = 0;
  // getlogin_r(prompt, promptBufsize);

  /* put signal catching functions here */
  // signal(SIGINT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  
  /* main (infinite) loop */
  while( true ) {

    /* print prompt*/
    printf("%s", prompt);

    /* get arguments */
    toks = gettoks();
    if( toks[0] != NULL )	{
      /* if want to quit, quit first*/
  	  if( !strcmp( toks[0], STRMYQUIT ))
        break;

      push_command(&history_list, join_tokens(toks));

      /* if internal commands, execute them,else handle system calls */ 
      if (!isInternal(toks)) {
        handleExternal(toks);
      }
      

      /* simple loop to echo all arguments */
  	  // for( ii=0; toks[ii] != NULL; ii++ ) {
	    //   printf( "Argument %d: %s\n", ii, toks[ii] );
	    // }
  	}
  }

  /* return to calling environment */
  return(retval);
}

bool isInternal(char **toks) {
  bool flag = true;
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
    flag = false;
  }
  return flag;
}

void handleExternal(char **toks) {
  int pid = fork();

  // when can't fork more processes
  if (pid < 0) {
    printf("Fork error!!!\n");
    return;
  }

  // in background
  if (isBackground(toks)) {
    if (pid == 0) {
      excuteCommand(toks);
    }

  // in foreground
  } else {
    if (pid == 0) {
      excuteCommand(toks);
    } else if (pid > 0){
      int status;
      waitpid(pid, &status, 0);
    }
  }
}

bool isBackground(char **toks) {
  int i;
  bool flag = false;

  // get the last argument
  for (i = 0; toks[i] != '\0'; i++)
    ;
  char *last_command = toks[--i];

  // check the flag. If there is an & in the end, remove the &
  if (strcmp(last_command, "&") == 0 && i >= 1) {
    toks[i] = '\0';
    flag = true;
  }

  return flag;
}

void excuteCommand(char **toks) {
  execvp(toks[0], toks);
  printf("%s -- Command not found\n", toks[0]);
}