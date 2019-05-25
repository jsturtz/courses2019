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


int main() {
    
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
            /* execvp("ls", NULL); */
            execlp("ls", "ls", "-a", NULL);
            break;
        default:
            waitpid(spawnPid, &childExitStatus, 0);
            break;
    }
    exit(0);
}
