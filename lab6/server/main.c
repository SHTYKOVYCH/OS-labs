#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "client.h"
#include "room.h"

struct rooms_s rooms;

void sigInt(int sig) {
    for (int i = 0; i < rooms.numOfRooms; ++i) {
        deleteRoom(rooms.rooms[i]);
    }

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("usage: server <port>");
        exit(EXIT_FAILURE);
    }

    struct sigaction act;
    act.sa_handler = sigInt;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);

    pthread_mutex_init(&rooms.roomsAwaible, NULL);
    rooms.numOfRooms = 0;
    rooms.last_id = 0;

    int connectionSocket;
    struct sockaddr_in connectionSocketParams;

    connectionSocket = socket(AF_INET, SOCK_STREAM, 0);

    connectionSocketParams.sin_family = AF_INET;
    connectionSocketParams.sin_addr.s_addr = htonl(INADDR_ANY);
    connectionSocketParams.sin_port = htons((short) atol(argv[1]));



    bind(connectionSocket, (struct sockaddr *) &connectionSocketParams, sizeof(connectionSocketParams));
    listen(connectionSocket, 4096);

    printf("Listening on port: %s\n", argv[1]);
    while (1) {
        struct sockaddr_in clientSocketParams;

        unsigned int clientLength = sizeof(clientSocketParams);

        int clientSocket = accept(connectionSocket, (struct sockaddr *) &clientSocketParams,
                                  &clientLength);

        acceptClient(clientSocket);
    }

    close(connectionSocket);
}