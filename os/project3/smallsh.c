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

#ifndef WORD_LENGTH
#define WORD_LENGTH 100
#endif

#ifndef NUM_PROCESSES
#define NUM_PROCESSES 50
#endif

// precondition: all args are filled with default values indicating empty states
//               in other words, will only fill values into pointers if discovered

void doAtExit() 
{
   /* * Your cd command should support both absolute and relative paths. When smallsh terminates, */ 
   /* * the original shell it was launched from will still be in its original working directory, */ 
   /* * despite your use of chdir() in smallsh. Your shell's working directory begins in whatever */ 
   /* * directory your shell's executible was launched from. *1/ */
}

void stripNewline(char* inputBuffer) 
{
  int i = 0; 
  while (inputBuffer[i] != '\n') i++;
  inputBuffer[i] = '\0';
}

void updateArgs(char** newArgs, char** argsArray) 
{

}

void deleteArgs(char** args) 
{
  int a = 0; 
  while (args[a] != NULL) 
  {
    free(args[a]);
    args[a] = NULL;
    a++;
  }
}

void addArg(char** args, char* newArg) 
{
  // find index of next spot in args array
  int a = 0;
  while (args[a] != NULL) a++;

  // allocate memory and copy characters into new pointer
  args[a] = malloc(sizeof(char) * WORD_LENGTH);
  strcpy(args[a], newArg);
}

// in/out/bg are flags used in main to determine whether to do file redirection or backgrounding
int setFlags(char** args, int* in, int* out, int* bg) {

}

// splits raw inputBuffer into args string array
int parse(char* inputBuffer, char** args) {
  
  // clean up buffers and pointers before parsing inputBuffer
  deleteArgs(args);

  int i = 0;
  char* token = strtok(inputBuffer, " "); 
  while (token) 
  {
    args[i] = malloc(sizeof(char));
    strcpy(args[i], token);
    i++;
  }

  // THIS IS WHERE I LAST LEFT

  cmd[0] = '\0';
  in[0] = '\0';
  out[0] = '\0';
  error[0] = '\0';
  *background = 0;

  // fill command with first word if exists 
  char* token = strtok(inputBuffer, " "); 
  if (token) strcpy(cmd, token);

  // loop for args
  int numArgs = 0;
  token = strtok(NULL, " ");
  while (token)
  {
    if (token[0] != '<' && token[0] != '>' && token[0] != '&') 
    {
      addArg(args, token);
      numArgs++;
    }
    // special case where redirection symbol or background symbol are part of arg 
    else if (token[1] != '\0') 
    {
      addArg(args, token);
      numArgs++;
    }
    // in this case, have reached redirection or background symbols
    else {
      break;
    }
    token = strtok(NULL, " ");
  }
  
  // loop for redirection / background
  int inputFound = 0;
  int outputFound = 0;

  while (token) 
  {
    if (token[0] == '<') 
    {
      // cannot have this symbol twice in string
      if (inputFound)  
      {
        strcpy(error, "Redirection symbol detected twice");
        return -1;
      }

      // there must exist another word after that names the file for stdin redirection
      token = strtok(NULL, " ");
      if (token) strcpy(in, token);
      else 
      {
        strcpy(error, "Redirection symbol without filename");
         return -1;
      }

      inputFound = 1;
    }

    else if (token[0] == '>') 
    {
      // cannot have this symbol twice in string
      if (outputFound) 
      {
        strcpy(error, "Redirection symbol detected twice");
         return -1;
      }

      // there must exist another word after that names the file for stdout redirection
      token = strtok(NULL, " ");
      if (token) strcpy(out, token);
      else 
      {
        strcpy(error, "Redirection symbol without filename");
         return -1;
      }
        
      outputFound = 1;
    }

    else if (token[0] == '&') 
    {
      // must be last word of command 
      token = strtok(NULL, " ");
      if (token) 
      {
        strcpy(error, "Background symbol must be last word");
         return -1;
      }

      *background = 1;
      return 0;
    }

    // only valid symbols after args are redirection or background
    else 
    {
      strcpy(error, "Invalid argument placement");
      return -1;
    }

    token = strtok(NULL, " ");
  }
  return 0; 
}

int changeDir(char* path, char* error) 
{
  if (path) 
  {
    if (chdir(path) == -1) 
    {
      sprintf(error, "Path doesn't exist: %s", path);
      return -1;
    }
  }
  else 
  {
    chdir(getenv("HOME"));
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

  /* // create structs needed for sigaction */
  /* struct sigaction SIGINT_action = {0}; */
  /* struct sigaction SIGTSTP_action = {0}; */

  /* // function handlers */
  /* SIGINT_action.sa_handler = catchSIGINT; */
  /* SIGTSTP_action.sa_handler = catchSIGTSTP; */

  /* // block all signals while handler active */
  /* sigfillset(&SIGINT_action.sa_mask); */
  /* sigfillset(&SIGTSTP_action.sa_mask); */

  /* // no special options */
  /* SIGINT_action.sa_flags = 0; */
  /* SIGTSTP_action.sa_flags = 0; */
  
  /* // call sigaction to register handlers */
  /* sigaction(SIGINT, &SIGINT_action, NULL); */
  /* sigaction(SIGTSTP, &SIGTSTP_action, NULL); */


  char  inputBuffer[CMD_LENGTH]     = {'\0'};   // holds entire line read in from stdin
  char  cmd[CMD_LENGTH]             = {'\0'};   // holds command to be executed
  char  in[CMD_LENGTH]              = {'\0'};   // holds filename for stdin redirection
  char  out[CMD_LENGTH]             = {'\0'};   // holds filename for stdout redirection
  char  startDir[CMD_LENGTH]        = {'\0'};   // holds path for current working directory
  char  error[100]                  = {'\0'};   // holds error message from failed function calls
  int  children[NUM_PROCESSES]      = { 0 };    // holds error message from failed function calls

  // args will hold all the pointers to pointers
  char** args = (char**) calloc(MAX_ARGS, sizeof(char*));
  // set all pointers to null (will allocate with addArg and free with deleteArgs)
  for (int i = 0; i < MAX_ARGS; i++) 
  {
    args[i] = NULL;
  }

  FILE* outFile = stdout;           // outFile will point to redirected file
  FILE* inFile = stdin;

  int background = 0;

  while(1) {

    // this is how we should print to screen since printf only writes to buffer. To guarantee that it
    // writes that buffer to the screen, use fflush to stdout. 
    printf(":");
    fflush(stdout);

    // get input from user
    memset(inputBuffer, CMD_LENGTH, '\0');
    fgets(inputBuffer, CMD_LENGTH, stdin);  
    stripNewline(inputBuffer);  

  /* // file redirection */
  /* int targetFD = open("Somefile.txt", O_WRONLY); */
  /* // since 1 = stdout, will redirect */
  /* // from stdout to targetFD */
  /* int result = dup2(targetFD, 1); */

    if (parse(inputBuffer, cmd, args, in, out, &background, error) != -1) 
    {
      // only do something if command exists and doesn't begin with comment char
      if ( cmd[0] != '\0'  && cmd[0] != '#')
      {
        // built-in commands here
        if (strcmp(cmd, "exit") == 0) 
        {
          /* The exit command exits your shell. It takes no arguments. 
           * When this command is run, your shell must kill any other processes or 
           * jobs that your shell has started before it terminates itself. */
          /* exitMyProg(); */
        }

        else if (strcmp(cmd, "status") == 0) 
        {
        
          /* printStatus(); */
        }

        else if (strcmp(cmd, "cd") == 0) 
        {
          /* Your cd command should support both absolute and relative paths. When smallsh terminates, 
           * the original shell it was launched from will still be in its original working directory, 
           * despite your use of chdir() in smallsh. Your shell's working directory begins in whatever 
           * directory your shell's executible was launched from. */
          if (changeDir(args[0], error) == -1) 
          {
            printf("ERROR: %s\n", error);
            fflush(stdout);
          }
        }

        // do the stuff with exec here
        else 
        {
          pid_t spawnPid = -5;
          int childExitStatus = -5;
          spawnPid = fork();
          switch (spawnPid)
          {
            case -1:
              perror("Hull Breach!\n");
              exit(1);
              break;
            case 0:
              
              // set redirection for input
              if (in[0] != '\0') 
              {
                
              }

              // set redirection for input
              if (out[0] != '\0') 
              {

              }

              /* exec(, cmd, args); */

              // the child will have a spawnPID of 0 for itself
              break;
            default:
              // the parent will have a spawnPID of some positive value representing child process
              break;
          }

        /* Whenever a non-built in command is received, have the parent fork() off a child. This child then does any needed input/output redirection before running exec() on the command given. Note that when doing redirection, that after using dup2() to set up the redirection, the redirection symbol and redirection destination/source are NOT passed into the following exec command (i.e., if the command given is ls > junk, then you handle the redirection to "junk" with dup2() and then simply pass ls into exec() ). */
        }
      }
    }

    else 
    {
      printf("ERROR: %s\n", error);
      fflush(stdout);
    }
  }

  // free space
  deleteArgs(args);
  free(args);
}  
