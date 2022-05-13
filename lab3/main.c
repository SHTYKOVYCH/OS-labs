//
// Created by dimitis on 30.04.22.
//

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>

extern char **environ;

pid_t forecastProcess;

int countArgs(char **args) {
    int count = 0;

    while (args[count]) {
        count += 1;
    }

    return count;
}

void sigINT(int sig) {
    if (!forecastProcess) {
        return;
    }

    kill(forecastProcess, 9);
    printf("\n");
}

void printPath() {
    char path[4096] = {0};
    getcwd(path, 4096);

    int numOfFolders = 0;
    char *folders[1024] = {NULL};
    char *folder = strtok(path, "/");

    while (folder) {
        folders[numOfFolders] = folder;
        numOfFolders += 1;

        folder = strtok(NULL, "/");
    }

    if (numOfFolders == 1) {
        printf("/");
    } else {
        for (int i = numOfFolders - 2 < 0 ? 0 : numOfFolders - 2; i < numOfFolders; ++i) {
            printf("/%s", folders[i]);
        }
    }
}

char **parseCommand(char **buffer, char *command) {
    int numOfLexems = 0;

    char *lexem = strtok(command, " ");

    while (lexem) {
        buffer[numOfLexems] = lexem;

        lexem = strtok(0, " ");
        numOfLexems += 1;
    }

    return buffer;
}

int parseAndExecCommand() {
    char command[4097] = {0};
    int commandNeedle = 0;

    char tmp;
    while ((tmp = getc(stdin)) != '\n') {
        if (tmp == ' ' && !commandNeedle) {
            continue;
        }

        command[commandNeedle] = tmp;
        commandNeedle += 1;
    }

    if (!commandNeedle) {
        return 0;
    }

    char *parsedCommand[4096] = {NULL};

    parseCommand(parsedCommand, command);

    if (!strcmp("exit", parsedCommand[0])) {
        return -1;
    }

    if (!strcmp("cd", parsedCommand[0])) {
        if (countArgs(parsedCommand) == 1) {
            printf("cd: folder path required\n");
            return 0;
        }

        if (chdir(parsedCommand[1])) {
            printf("term: could not change dir\n");
        };
        return 0;
    }

    int numOfArgs = countArgs(parsedCommand);
    char isBackground = 0;

    if (!strcmp(parsedCommand[numOfArgs - 1], "&")) {
        isBackground = 1;
        parsedCommand[numOfArgs - 1] = NULL;
    }

    pid_t childPid = fork();

    switch (childPid) {
        case (-1):
            perror("term:");
            break;
        case (0): {
            if (execvp(parsedCommand[0], parsedCommand) == -1) {
                perror("term");
            }
            exit(EXIT_SUCCESS);
        }
        default: {
            if (!isBackground) {
                forecastProcess = childPid;
                pid_t exitCode = waitpid(forecastProcess, NULL, 0);

                if (WIFEXITED(exitCode)) {
                    printf("term: process finished abnormally with code %d\n", WEXITSTATUS(exitCode));
                }

                forecastProcess = 0;
            }

            return 0;
        }
    }


    return 0;
}

int main() {

    forecastProcess = 0;

    struct sigaction act;
    act.sa_handler = sigINT;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    signal(SIGCHLD, SIG_IGN);

    do {
        printPath();
        printf(" $ ");

        if (parseAndExecCommand()) {
            break;
        }
    } while (1);

    exit(EXIT_SUCCESS);
}
