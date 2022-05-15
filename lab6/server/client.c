#include "client.h"
#include "room.h"

struct client* acceptClient(int socket) {
    struct client* newClient = malloc(sizeof(struct client));

    printf("New client is connecting\n");

    if (!newClient) {
        write(socket, "f", 1);
        perror("connection error");
        return NULL;
    }

    printf("New client is created\n");

    write(socket, "t", 1);

    newClient->socket = socket;

    long readed = read(socket, newClient->name, 255);
    if (readed == 0 || readed == -1) {
        write(socket, "f", 1);
        perror("connection error");
        free(newClient);
        return NULL;
    }

    write(socket, "t", 1);

    printf("New client is registered\n");

rooms_printing:
    write(socket, &rooms.numOfRooms, 4);

    for (int i = 0; i < rooms.numOfRooms; ++i) {
        printf("Sended client %d - %s\n", rooms.rooms[i]->id, rooms.rooms[i]->name);
        write(socket, &rooms.rooms[i]->id, 4);
        write(socket, rooms.rooms[i]->name, strlen(rooms.rooms[i]->name) + 1);
    }

    printf("New client received list of rooms\n");

    char command[3] = {0};

    if (read(newClient->socket, command, 2)) {
        if (!strcmp(command, "CR")) {
            printf("New client sended \"Create room\" command\n");

            char roomName[255];
            if (!read(socket, roomName, 255)) {
                perror("room creation - reading name");
                free(newClient);
                return NULL;
            }

            if (!createRoom(roomName)) {
                write(socket, "f", 1);
                perror("room creation - creating room");
                close(socket);
                return NULL;
            }

            write(socket, "t", 1);
            goto rooms_printing;
        }
    } else {
        perror("Reading from client error");
        close(socket);
        return NULL;
    }

    printf("New client sended \"Connect to room\" command\n");

    int roomId;
    readed = read(socket, &roomId, 4);
    if (readed == 0 || readed == -1) {
        write(socket, "f", 1);
        perror("Reading from client");
        close(newClient->socket);
        free(newClient);
        return NULL;
    }

    write(socket, "t", 1);

    printf("New client sended id of a room\n");

    for (int i = 0; i < rooms.numOfRooms; ++i) {
        if (rooms.rooms[i]->id == roomId) {
            if (!connectToRoom(rooms.rooms[i], newClient)) {
                write(newClient->socket, "f", 1);
                close(newClient->socket);
                free(newClient);
                return NULL;
            }
        }
    }

    printf("New client connected to room %d\n", roomId);

    newClient->read_socket = dup(newClient->socket);

    pthread_attr_t readThreadParams;
    pthread_attr_init(&readThreadParams);
    pthread_attr_setdetachstate(&readThreadParams, PTHREAD_CREATE_DETACHED);

    pthread_create(&newClient->readThread, &readThreadParams, clientReader, newClient);

    printf("New client is operational\n");

    return newClient;
}

void *clientReader(void* info) {
    struct client* someClient = info;
    printf("Started listening for client %d\n", someClient->read_socket);

    char buffer[4056] = {0};

    do {
        char message[4097] = {0};

        if (!read(someClient->read_socket, message, 4096)) {
            printf("client %d disconected\n", someClient->read_socket);
            close(someClient->read_socket);
            close(someClient->socket);
            disconnectFromRoom(someClient->room, someClient);
            free(someClient);
            break;
        }
        printf("Readed message from %d\n", someClient->read_socket);

        struct message* newMessage;
        do {
            newMessage = malloc(sizeof(struct message));
        } while (!newMessage);

        strcpy(newMessage->message, message);
        strcpy(newMessage->senderName, someClient->name);

        printf("Sending message from %d\n", someClient->read_socket);

        sendMessage(someClient->room, newMessage);
    } while(1);

    pthread_exit("");
}