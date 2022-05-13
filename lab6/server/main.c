#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

struct clientInfoStruct
{
    int socket;
};

void *clientReader(void* info) {
    int socket = ((struct clientInfoStruct *) info)->socket;

    char str[4096] = {0};
    while(read(socket, str, 4096)) {
        printf("read from client: %s\n", str);
    }
    pthread_exit("");
}

void *clientWriter(void* info) {
    int socket = ((struct clientInfoStruct *) info)->socket;

    const char* greetings = "Hello, user!";
    write(socket, greetings, strlen(greetings) + 1  );

    pthread_exit("");
}

void *serveClient(void *info) {
    int socket = ((struct clientInfoStruct *) info)->socket;

    pthread_t readThread, writeThread;

    pthread_attr_t readThreadParams;
    pthread_attr_init(&readThreadParams);
    pthread_attr_setdetachstate(&readThreadParams, PTHREAD_CREATE_DETACHED);

    pthread_create(&readThread, NULL, clientReader, info);
    pthread_create(&writeThread, &readThreadParams, clientWriter, info);

    pthread_join(readThread, NULL);

    close(socket);

    pthread_exit("");
}

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("usage: server <port>");
        exit(EXIT_FAILURE);
    }

    int connectionSocket;
    struct sockaddr_in connectionSocketParams;

    connectionSocket = socket(AF_INET, SOCK_STREAM, 0);

    connectionSocketParams.sin_family = AF_INET;
    connectionSocketParams.sin_addr.s_addr = htonl(INADDR_ANY);
    connectionSocketParams.sin_port = htons((short)atol(argv[1]));

    bind(connectionSocket, (struct sockaddr *) &connectionSocketParams, sizeof(connectionSocketParams));
    listen(connectionSocket, 4096);

    printf("Listening on port: %s\n", argv[1]);
    while (1) {

        struct sockaddr_in clientSocketParams;

        unsigned int clientLength = sizeof(clientSocketParams);
        int clientSocket = accept(connectionSocket, (struct sockaddr *) &clientSocketParams,
                                  &clientLength);

        struct clientInfoStruct clientInfo;
        clientInfo.socket = clientSocket;

        pthread_t clientThreadId;
        pthread_attr_t clientServerAttr;
        pthread_attr_init(&clientServerAttr);
        pthread_attr_setdetachstate(&clientServerAttr, PTHREAD_CREATE_DETACHED);
        pthread_create(&clientThreadId, &clientServerAttr, serveClient, &clientInfo);
    }

    close(connectionSocket);
}