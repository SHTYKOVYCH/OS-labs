#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int sock;

    struct sockaddr_in socketParams;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    socketParams.sin_family = AF_INET;
    socketParams.sin_addr.s_addr = inet_addr("127.0.0.1");
    socketParams.sin_port = htons(9526);

    int length = sizeof(socketParams);

    int result = connect(sock, (struct sockaddr*)&socketParams, length);

    if (result == -1) {
        perror("client");
        exit(EXIT_FAILURE);
    }

    char serverString[4096] = {0};
    read(sock, serverString, 4096);
    printf("Read from server: %s\n", serverString);

    const char* greetings = "hello server";

    write(sock, greetings, strlen(greetings) + 1);

    close(sock);
}