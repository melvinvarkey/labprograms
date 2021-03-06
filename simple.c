/*
 * This code implemenFts a simple shell program
 * At this time it supports just simple commands with 
 * any number of args.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

//#include "input.h"
//#include "myShell.h"
//#include "BackgroundStack.h"

/*
 * The main shell function
 */ 
main() {
    char *buff[20];
    char *inputString;

    BackgroundStack *bgStack = malloc(sizeof(BackgroundStack));
    initBgStack(bgStack);

    struct sigaction new_act;
    new_act.sa_handler = sigIntHandler;
    sigemptyset ( &new_act.sa_mask );
    new_act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &new_act, NULL);

    // Loop forever
    while(1) {
        const char *chPath;

        doneBgProcesses(bgStack);

        // Print out the prompt and get the input
        printPrompt();

        inputString = get_my_args(buff);
        if (buff[0] == NULL) continue;

        if (buff[0][0] == '#') continue;

        switch (getBuiltInCommand(buff[0])) {
            case EXIT:
                exit(0);
                break;
            case CD:
                chPath = (buff[1]==NULL) ? getenv("HOME") : buff[1];
                if (chdir(chPath) < 0) {
                    perror(": cd");
                }
                break;
            default:
                do_command(buff, bgStack);
        }

        //free up the malloced memory
        free(inputString);
    }// end of while(1)
}

static void sigIntHandler (int signum) {}

/* 
 * Do the command
 */
int do_command(char **args, BackgroundStack *bgStack) {
    int status, statusb;  
    pid_t child_id, childb_id;
    char **argsb;
    int pipes[2];

    int isBgd = isBackgrounded(args);
    int hasPipe = hasAPipe(args);

    if (isBgd) removeBackgroundCommand(args);
    if (hasPipe) {
        int cmdBi = getSecondCommandIndex(args);
        args[cmdBi-1] = NULL;
        argsb = &args[cmdBi];
        pipe(pipes);
    }

    // Fork the child and check for errors in fork()
    if((child_id = fork()) == -1) {
        switch(errno) {
            case EAGAIN:
                perror("Error EAGAIN: ");
                return;
            case ENOMEM:
                perror("Error ENOMEM: ");
                return;
        }
    }

    if (hasPipe && child_id != 0) {
        childb_id = fork();
        if(childb_id == -1) {
            switch(errno) {
                case EAGAIN:
                    perror("Error EAGAIN: ");
                    return;
                case ENOMEM:
                    perror("Error ENOMEM: ");
                    return;
            }
        }
    }

    if(child_id == 0 || (childb_id == 0 && hasPipe)) {
        if (child_id != 0 && hasPipe) args = argsb;
        if (child_id == 0 && isBgd) {
            struct sigaction new_act;
            new_act.sa_handler = SIG_IGN;
            sigaction(SIGINT, &new_act, 0);
        }

        if (child_id == 0 && hasPipe) {
            if (dup2(pipes[1], 1) != 1) fatalPerror(": Pipe Redirection Output Error");
            close(pipes[0]);
            close(pipes[1]);
        }
        if (child_id != 0 && hasPipe) {
            if (dup2(pipes[0], 0) != 0) fatalPerror(": Pipe Redirection Input Error");
            close(pipes[0]);
            close(pipes[1]);
            waitpid(child_id, NULL, 0);
        }

        if ((child_id != 0 && hasPipe) || !hasPipe) {
            if (hasAReOut(args)) {
                char outFile[100];
                getOutFile(args, outFile);

                int reOutFile = open(outFile, O_RDWR|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE);
                if (reOutFile<0) fatalPerror(": Redirection Output Error");

                if (dup2(reOutFile,1) != 1) fatalPerror(": Redirection Output Error");
                close(reOutFile);
            }
        }

        if ( (child_id == 0 && hasPipe) || !hasPipe) {
            if (hasAReIn(args)) {
                char inFle[100];
                getInFile(args, inFle);

                int reInFile = open(inFle, O_RDWR);
                if (reInFile<0) fatalPerror(": Redirection Input Error");

                if (dup2(reInFile,0) != 0) fatalPerror(": Redirection Input Error");
                close(reInFile);
            } else if (isBgd && !hasPipe) {
                int bgReInFile = open("/dev/null", O_RDONLY);
                if (bgReInFile<0) fatalPerror(": /dev/null Redirection Input Error");

                if (dup2(bgReInFile,0) != 0) fatalPerror(": /dev/null Redirection Input Error");
                close(bgReInFile);
            }
        }

        // Execute the command
        execvp(args[0], args);
        perror(args[0]);

        exit(-1);
    }

    // Wait for the child process to complete, if necessary
    if (!isBgd) waitpid(child_id, &status, 0);
    else if (!hasPipe) {
        printf("Child %ld started\n", (long)child_id);
        BackgroundProcess *bgPrs = malloc(sizeof(BackgroundProcess)); 
        bgPrs->pid = child_id;
        bgPrs->exitStatus = -1;

        addProcessToBgStack(bgStack, bgPrs);
    }
    if (hasPipe) waitpid(childb_id, &statusb, 0);
    if ( WIFSIGNALED(status) && !isBgd )    printf("Child %ld terminated due to signal %d\n", (long)child_id, WTERMSIG(status) );
    if ( hasPipe && WIFSIGNALED(statusb) ) printf("Child %ld terminated due to signal %d\n", (long)childb_id, WTERMSIG(status) );

} // end of do_command
