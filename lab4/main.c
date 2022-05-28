#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int main() {
    int pipes[2];

    pid_t encoderPid, decoderPid;

    if (pipe(pipes)) {
        perror("piper:");
        exit(EXIT_FAILURE);
    }

    decoderPid = fork();

    switch (decoderPid) {
        case(-1):
            printf("piper: couldn't fork\n");
            close(pipes[0]);
            close(pipes[1]);
            exit(EXIT_FAILURE);
        case(0):
            close(0);
            dup(pipes[0]);
            close(pipes[0]);
            close(pipes[1]);
            execl("./encoder/encoder", "./encoder/encoder", NULL);
            printf("decoder didn't starts\n");
            exit(EXIT_FAILURE);
        default:
            break;
    }

    encoderPid = fork();

    switch (encoderPid) {
        case(-1):
            printf("piper: couldn't fork\n");
            close(pipes[0]);
            close(pipes[1]);
            kill(decoderPid, 9);
            exit(EXIT_FAILURE);
        case(0):
            close(1);
            dup(pipes[1]);
            close(pipes[1]);
            close(pipes[0]);
            execl("./encoder/encoder", "./encoder/encoder", NULL);
            printf("encoder didn't starts\n");
            exit(EXIT_FAILURE);
        default:
            break;
    }

    close(pipes[0]);
    close(pipes[1]);
    waitpid(decoderPid, NULL, 0);
    exit(EXIT_SUCCESS);
}