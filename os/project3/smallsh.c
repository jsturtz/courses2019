#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

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

// global needed for handling CTRL-Z signal
int   pauseProgram = 0;

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

// splits raw inputBuffer into args string array
int parse(char* inputBuffer, char** args, char* in, char* out, int* bg) {
  
  // frees all pointers in args, sets to Null
  deleteArgs(args);

  // reset in/out/bg
  in[0] = '\0';
  out[0] = '\0';
  *bg = 0;

  char* token = strtok(inputBuffer, " "); 
  while (token) 
  {
    if (token[0] == '<' && token[1] == '\0')
    {
      token = strtok(NULL, " ");        // skip the next word
      strcpy(in, token);
    }

    else if (token[0] == '>' && token[1] == '\0')
    {
      token = strtok(NULL, " ");        // skip the next word
      strcpy(out, token);
    }

    else if (token[0] == '&' && token[1] == '\0')
    {
      *bg = 1;
      return 0;
    }
    else 
    {
      addArg(args, token); // allocates space for array, fills with contents of token
    }
    token = strtok(NULL, " "); 
  }
}

// function that handles built-in "cd" command
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

// function that handles built-in "exit" command
void exitProgram(int* children, int childrenCount) 
{
  int c = 0;
  for (int c = 0; c < childrenCount; c++) 
  {
    kill(children[c], SIGKILL);
  }
  exit(0);
}

// parent handlers will not terminate shell
void parentSIGINT(int signo) {
  int childExitStatus = -5;
  wait(&childExitStatus);
  
  // must display to user which signal killed child process if any
  if (WIFSIGNALED(childExitStatus))
  {
    char message[24];
    sprintf(message, "terminated by signal %d\n", WTERMSIG(childExitStatus));
    write(STDOUT_FILENO, message, 24);
  }
}

void parentSIGTSTP(int signo) {
  char message[31];
  if (pauseProgram) 
  {
    strcpy(message, "Exiting foreground-only mode\n");
    write(STDOUT_FILENO, message, 30);
  }
  else 
  {
    strcpy(message, "Entering foreground-only mode\n");
    write(STDOUT_FILENO, message, 31);
  }
  pauseProgram = (pauseProgram == 0); // toggle global pause variable
}

// need this global variable because signal handling requires it

int main() {

  char  inputBuffer[CMD_LENGTH]     = {'\0'};   // holds entire line read in from stdin
  char  cmd[CMD_LENGTH]             = {'\0'};   // holds command to be executed
  char  in[CMD_LENGTH]              = {'\0'};   // holds filename for stdin redirection
  char  out[CMD_LENGTH]             = {'\0'};   // holds filename for stdout redirection
  char  startDir[CMD_LENGTH]        = {'\0'};   // holds path for current working directory
  char  error[100]                  = {'\0'};   // holds error message from failed function calls
  int   children[NUM_PROCESSES]     = { 0 };    // holds error message from failed function calls

  int   childrenCount   = 0;                    // holds number of child processes
  int   bg              = 0;                    // flag for whether command should be backgrounded
  int   fgExitStatus    = 0;                    // holds exit status for foregrounded processes that terminate
  
  pid_t spawnPid;               
  int   childExitStatus;  

  struct sigaction SIGINT_action = {0};
  struct sigaction SIGTSTP_action = {0};

  // signal handlers for parent
  // function handlers
  SIGINT_action.sa_handler = parentSIGINT;
  SIGTSTP_action.sa_handler = parentSIGTSTP;

  // block all signals while handler active
  sigfillset(&SIGINT_action.sa_mask);
  sigfillset(&SIGTSTP_action.sa_mask);

  // no special options
  SIGINT_action.sa_flags = 0;
  SIGTSTP_action.sa_flags = 0;
  
  // call sigaction to register handlers
  sigaction(SIGINT, &SIGINT_action, NULL);
  sigaction(SIGTSTP, &SIGTSTP_action, NULL);

  // args will hold all the pointers to pointers, initialized to null
  char** args = (char**) calloc(MAX_ARGS, sizeof(char*));
  for (int i = 0; i < MAX_ARGS; i++) 
  {
    args[i] = NULL;
  }

  while(1) {

    printf(":");
    fflush(stdout);

    // check if children have become zombies
    for (int c = 0; c < childrenCount; c++) 
    {
      if (waitpid(children[c], &childExitStatus, WNOHANG) != 0)
      {
        if (WIFEXITED(childExitStatus))
        {
          int exitStatus = WEXITSTATUS(childExitStatus);
          printf("Process %d completed normally with exit status %d\n", children[c], exitStatus);
          fflush(stdout);
        }
        else
        {
          int exitSignal = WIFSIGNALED(childExitStatus);
          printf("Process %d terminated by signal with exit status %d\n", children[c], exitSignal);
          fflush(stdout);
        }
      }
    }

    // get input from user
    memset(inputBuffer, CMD_LENGTH, '\0');
    fgets(inputBuffer, CMD_LENGTH, stdin);  
    stripNewline(inputBuffer);  

    // fills args with everything needed for execvp
    // fills in/out with filenames if exist, sets bg flag for parent
    parse(inputBuffer, args, in, out, &bg); 
    
    // do something only if (a) command given or (2) not a comment line
    if (args[0] && args[0][0] != '\0' && args[0][0] != '#')
    {
      // built-in commands here
      if (strcmp(args[0], "exit") == 0) 
      {
        exitProgram(children, childrenCount);
      }

      else if (strcmp(args[0], "status") == 0) 
      {
        printf("%d\n", fgExitStatus);
        fflush(stdout);
      }

      else if (strcmp(args[0], "cd") == 0) 
      {
        if (changeDir(args[1], error) == -1) 
        {
          printf("ERROR: %s\n", error);
          fflush(stdout);
        }
      }

      // execute non-built in command with new process
      else 
      {
        spawnPid = -5;
        childExitStatus = -5;
        spawnPid = fork();
        switch (spawnPid)
        {
          case -1:
            perror("Hull Breach!\n");
            exit(1);
            break;

          case 0:

            // set redirection for output
            if (out[0] != '\0') 
            {
              int outFile = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
              if (outFile >= 0) 
              {
                dup2(outFile, 1);
                close(outFile);
              }
              else 
              {
                printf("File could not be opened for writing\n");
                fflush(stdout);
                exit(1);
              }
            }

            // set redirection for input
            if (in[0] != '\0') 
            {
              int inFile = open(in, O_RDONLY, 0666);
              if (inFile >= 0) 
              {
                dup2(inFile, 0);
                close(inFile);
              }
              else 
              {
                printf("File could not be opened for reading\n");
                fflush(stdout);
                exit(1);
              }
            }

            // in this case, stdin & stdout must be redirected to dev/null
            if (bg && !pauseProgram) 
            {
              int devNull = open("/dev/null", O_RDWR);
              dup2(devNull, 0);
              dup2(devNull, 1);
            }

            if (execvp(args[0], args) == -1)
            {
              printf("ERROR: Invalid command\n"); 
              fflush(stdout);
              exit(1);
            }
            break;
          default:
            

            // wait only if backgrounded or paused
            if (bg && !pauseProgram)
            {
              children[childrenCount] = spawnPid;
              childrenCount++;
            }
            else
            {
              // since not backgrounded, wait here
              waitpid(spawnPid, &childExitStatus, 0);
              
              if (WIFEXITED(childExitStatus))
              {
                fgExitStatus = WEXITSTATUS(childExitStatus);
              }
              else if (WIFSIGNALED(childExitStatus))
              {
                fgExitStatus = WTERMSIG(childExitStatus);
              }
            }
            break;
        }
      }
    }
  }

  // free space
  deleteArgs(args);
  free(args);
}  

/* Your program must expand any instance of "$$" in a command into the process ID of the shell itself. Your shell does not otherwise perform variable expansion. This feature makes it easier to create a grading script that keeps your work separate. */

