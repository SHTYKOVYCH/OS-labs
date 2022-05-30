#include "client.h"
#include "room.h"

void *acceptClient(void *socket_p) {
    int socket = *(int *) socket_p;
    free(socket_p);

    struct client *newClient = malloc(sizeof(struct client));

    printf("New client is connecting\n");

    if (!newClient) {
        write(socket, "f", 1);
        printf("registration error: memory allocation\n");
        return NULL;
    }

    printf("server: new client is created\n");

    write(socket, "t", 1);

    newClient->socket = socket;

    long readed = read(socket, newClient->name, 255);
    if (readed == 0 || readed == -1) {
        write(socket, "f", 1);
        close(socket);
        printf("server error: reading from socket\n");
        free(newClient);
        pthread_exit("");
    }

    write(socket, "t", 1);
    printf("server: new client is registered\n");

    rooms_printing:
    write(socket, &rooms.numOfRooms, 4);

    for (int i = 0; i < rooms.numOfRooms; ++i) {
        printf("server: sent room %d - %s\n", rooms.rooms[i]->id, rooms.rooms[i]->name);
        write(socket, &rooms.rooms[i]->id, 4);
        write(socket, rooms.rooms[i]->name, 256);
    }

    printf("server: new client received list of rooms\n");

    char command[3] = {0};
    int result;
    reading_command:
    result = read(newClient->socket, command, 2);
    if (result != -1 && result != 0) {
        if (!strcmp(command, "CR")) {

            printf("server: new client sent \"Create room\" command\n");

            int result;

            char roomName[256];
            result = read(socket, roomName, 256);

            if (result == -1 || result == 0) {
                printf("server error: client disconnected");
                close(socket);
                free(newClient);
                return NULL;
            }

            if (!createRoom(roomName)) {
                write(socket, "f", 1);
                printf("server error: creating room");
                goto reading_command;
            }

            write(socket, "t", 1);
            goto rooms_printing;
        }
    } else {
        printf("server error: client disconnected");
        close(socket);
        free(newClient);
        return NULL;
    }

    printf("server: new client sent \"Connect to room\" command\n");

    int roomId;
    readed = read(socket, &roomId, 4);
    if (readed == -1 || readed == 0) {
        printf("server error: client disconnected");
        close(newClient->socket);
        free(newClient);
        return NULL;
    }

    write(socket, "t", 1);
    printf("server: new client sent id of a room - %c\n", roomId);

    for (int i = 0; i < rooms.numOfRooms; ++i) {
        if (rooms.rooms[i]->id == roomId) {
            if (!connectToRoom(rooms.rooms[i], newClient)) {
                write(newClient->socket, "f", 1);
                goto reading_command;
            }
            break;
        }
    }

    printf("server: new client connected to room %d\n", roomId);

    pthread_attr_t readThreadParams;
    pthread_attr_init(&readThreadParams);
    pthread_attr_setdetachstate(&readThreadParams, PTHREAD_CREATE_DETACHED);

    pthread_create(&newClient->readThread, &readThreadParams, clientReader, newClient);

    printf("server: new client is operational\n");
}

void *clientReader(void *info) {
    struct client *someClient = info;
    printf("server: started listening for client %d\n", someClient->socket);

    fd_set socket;

    FD_ZERO(&socket);
    FD_SET((someClient->socket), &socket);

    int result;

    do {
        char message[4097] = {0};

        result = select(FD_SETSIZE, &socket, NULL, NULL, NULL);

        if (result == -1) {
            printf("server error: client %d disconected\n", someClient->socket);
            close(someClient->socket);
            disconnectFromRoom(someClient->room, someClient);
            free(someClient);
            break;
        }

        if (read(someClient->socket, message, 4097) == 0) {
            printf("server error: client %d disconected\n", someClient->socket);
            close(someClient->socket);
            disconnectFromRoom(someClient->room, someClient);
            free(someClient);
            break;
        }
        printf("server: read message from %d\n", someClient->socket);

        struct message *newMessage;
        do {
            newMessage = malloc(sizeof(struct message));
        } while (!newMessage);

        strcpy(newMessage->message, message);
        strcpy(newMessage->senderName, someClient->name);

        printf("server: sending message from %d\n", someClient->socket);

        sendMessage(someClient->room, newMessage);
    } while (1);

    pthread_exit("");
}