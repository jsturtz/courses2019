#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#ifndef MAX_ARGS
#define MAX_ARGS 512
#endif

#ifndef CMD_LENGTH
#define CMD_LENGTH 2048
#endif

// will return -1 for any invalid command
// precondition: all args are filled with default values indicating empty states
//               in other words, will only fill values into pointers if discovered

int parse(char* inputBuffer, char* cmd, char** args, char* in, char* out, int* background) {
 
  // fill command with first word if exists 
  char* token = strtok(inputBuffer, " "); 
  if (token) strcpy(cmd, token);
  else 
  {
    return -1;
  }

  // loop for args
  int numArgs = 0;
  token = strtok(NULL, " ");
  while (token)
  {
    if (token[0] != '<' && token[0] != '>' && token[0] != '&') 
    {
      args[numArgs] = token;
      numArgs++;
    }
    // special case where redirection symbol or background symbol are part of arg 
    else if (token[1] != '\0') 
    {
      args[numArgs] = token;
      numArgs++;
    }
    // in this case, have reached redirection or background symbols
    else {
      break;
    }
    token = strtok(NULL, " ");
  }
  
  // loop for redirection / background
  int inputFound, outputFound = 0;

  while (token) 
  {
    if (token[0] == '<') 
    {
      // cannot have this symbol twice in string
      if (inputFound)  
      {
        return -1;
      }

      // there must exist another word after that names the file for stdin redirection
      token = strtok(NULL, " ");
      if (token) strcpy(in, token);
      else return -1;
      inputFound = 1;
    }

    else if (token[0] == '>') 
    {
      // cannot have this symbol twice in string
      if (outputFound) 
      {
        return -1;
      }

      // there must exist another word after that names the file for stdout redirection
      token = strtok(NULL, " ");
      if (token) strcpy(out, token);
      else return -1;
      outputFound = 1;
    }

    else if (token[0] == '&') 
    {
      // must be last word of command 
      token = strtok(NULL, " ");
      if (token) return -1;

      *background = 1;
      return 0;
    }

    // only valid symbols after args are redirection or background
    else 
    {
      return -1;
    }

    token = strtok(NULL, " ");
  }
  return 0; 
}

// this is what we want the program to do
// once handled. A ptr to this function
// will be passed to the sigaction() func
void catchSIGINT(int signo) {
  char* message = "Caught SIGINT\n";
  write(STDOUT_FILENO, message, 30);
}

void catchSIGTSTP(int signo) {
  char* message = "Caught SIGINT\n";
  write(STDOUT_FILENO, message, 30);
}

int main() {

  // create structs needed for sigaction
  struct sigaction SIGINT_action = {0};
  struct sigaction SIGTSTP_action = {0};

  // function handlers
  SIGINT_action.sa_handler = catchSIGINT;
  SIGTSTP_action.sa_handler = catchSIGTSTP;

  // block all signals while handler active
  sigfillset(&SIGINT_action.sa_mask);
  sigfillset(&SIGTSTP_action.sa_mask);

  // no special options
  SIGINT_action.sa_flags = 0;
  SIGTSTP_action.sa_flags = 0;
  
  // call sigaction to register handlers
  sigaction(SIGINT, &SIGINT_action, NULL);
  sigaction(SIGTSTP, &SIGTSTP_action, NULL);


  char inputBuffer[CMD_LENGTH];
  char* cmd[CMD_LENGTH];
  char* args[MAX_ARGS];
  char in[CMD_LENGTH];
  char out[CMD_LENGTH];
  in[0] = out[0] = 

  FILE* outFile = stdout;
  FILE* inFile = stdin;

  int background = 0;

  while(1) {

    // get input from user
    memset(inputBuffer, CMD_LENGTH, '\0');
    fgets(inputBuffer, CMD_LENGTH, stdin);  // IS THIS HOW FGETS WORKS? DOES IT NOT WRITE ANY CHARS IF USER PRESSES ENTER?

    // this is how we should print to screen since printf only writes to buffer. To guarantee that it
    // writes that buffer to the screen, use fflush to stdout. 
    printf(":");
    fflush(stdout);

  // file redirection
  int targetFD = open("Somefile.txt", O_WRONLY);
  // since 1 = stdout, will redirect
  // from stdout to targetFD
  int result = dup2(targetFD, 1);
    
    if (inputBuffer[0] != '\0' && inputBuffer[0] != '#') 
    {

      // fill variables with parsed instructions 
      parse(inputBuffer, cmd, args, in, out, &background); 

      // set input redirection
      if (in[0]) 
      {
        inFile = fopen(in, "r");
        dup2(inFile, 0);
      }
      
      // set output redirection
      if (out[0]) 
      {
        outFile = fopen(in, "w");
        dup2(outFile, 1);
      }
      
      // background process? 
      if (background) 
      {
        // background the process?
      }

      if (builtIn(inputBuffer)) 
      {
        // i.e. if the first arg is one of the following: exit, cd, or status
      }
    
      else 
      {
        /* Whenever a non-built in command is received, have the parent fork() off a child. This child then does any needed input/output redirection before running exec() on the command given. Note that when doing redirection, that after using dup2() to set up the redirection, the redirection symbol and redirection destination/source are NOT passed into the following exec command (i.e., if the command given is ls > junk, then you handle the redirection to "junk" with dup2() and then simply pass ls into exec() ). */
      }
    }
  }  

  /* Your shell will allow for the redirection of standard input and standard output and it will support both foreground 
   * and background processes (controllable by the command line and by receiving signals). */

  /* Your shell will support three built in commands: exit, cd, and status. 
   * It will also support comments, which are lines beginning with the # character. */


  /* Use the colon : symbol as a prompt for each command line. Be sure you flush out the output buffers each time you print, as the text that you're outputting may not reach the screen until you do in this kind of interactive program. To do this, call fflush() immediately after each and every time you output text. */


  /* Your shell must support command lines with a maximum length of 2048 characters, and a maximum of 512 arguments. You do not need to do any error checking on the syntax of the command line. */

  /* Finally, your shell should allow blank lines and comments.  Any line that begins with the # character is a comment line and should be ignored (mid-line comments, such as the C-style //, will not be supported).  A blank line (one without any commands) should also do nothing. Your shell should just re-prompt for another command when it receives either a blank line or a comment line. */
}
