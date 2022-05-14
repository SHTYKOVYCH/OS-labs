#include <malloc.h>

#include "room.h"

struct room* createRoom(char* name) {
    pthread_mutex_lock(&rooms.roomsAwaible);

    if (!rooms.rooms) {
        rooms.rooms = malloc(sizeof(struct room*));

        if (!rooms.rooms) {
            perror("Error on creating room");
            return NULL;
        }
    }

    struct room* newRoom = malloc(sizeof(struct room));

    if (!newRoom) {
        perror("room creation fail");
        return NULL;
    }

    for (int i = 0; i < 255; ++i) {
        newRoom->name[i] = 0;
    }

    strcpy(newRoom->name, name);

    newRoom->clients = NULL;
    newRoom->numOfClients = 0;
    newRoom->messages = NULL;
    newRoom->numOfMessages = 0;

    pthread_mutex_init(&(newRoom->roomAwaible), NULL);

    struct room** tmp_rooms = malloc(sizeof(rooms)*rooms.numOfRooms + 1);

    if (!tmp_rooms) {
        perror("Error on creating room");
        free(newRoom);
        return NULL;
    }

    for (int i = 0; i < rooms.numOfRooms; ++i) {
        tmp_rooms[i] = rooms.rooms[i];
    }

    free(rooms.rooms);
    rooms.rooms = tmp_rooms;

    rooms.rooms[rooms.numOfRooms] = newRoom;
    rooms.numOfRooms += 1;
    newRoom->id = rooms.last_id;
    rooms.last_id += 1;

    pthread_mutex_unlock(&rooms.roomsAwaible);

    return newRoom;
}

void deleteRoom(struct room* someRoom) {
    pthread_mutex_lock(&someRoom->roomAwaible);

    if (someRoom->clients) {
        printf("Deleting users\n");
        for (int i = 0; i < someRoom->numOfClients; ++i) {
            pthread_cancel(someRoom->clients[i]->readThread);

            char goodbuyString[4096] = "the room was deleted";
            char sender[255] = "system";
            write(someRoom->clients[i]->socket, sender, 255);
            write(someRoom->clients[i]->socket, goodbuyString, 4096);
            close(someRoom->clients[i]->socket);

            free(someRoom->clients[i]);
        }
        free(someRoom->clients);
    }

    if (someRoom->messages) {
        printf("Deleting messages\n");
        for (int i = 0; i < someRoom->numOfMessages; ++i) {
            free(someRoom->messages[i]);
        }

        free(someRoom->messages);
    }

    printf("Deleting room\n");
    pthread_mutex_lock(&rooms.roomsAwaible);
    int searchIndex;
    for (searchIndex = 0; searchIndex < rooms.numOfRooms; ++searchIndex) {
        if (rooms.rooms[searchIndex]->id == someRoom->id) {
            break;
        }
    }

    for (; searchIndex < rooms.numOfRooms - 1; ++searchIndex) {
        rooms.rooms[searchIndex] = rooms.rooms[searchIndex + 1];
    }

    rooms.numOfRooms -= 1;

    pthread_mutex_unlock(&rooms.roomsAwaible);

    pthread_mutex_unlock(&someRoom->roomAwaible);

    free(someRoom);
}

struct room* connectToRoom(struct room* someRoom, struct client* client) {
    pthread_mutex_lock(&someRoom->roomAwaible);

    struct client** tmp_arr = malloc(sizeof(struct client*)*someRoom->numOfClients + 1);

    if (!tmp_arr) {
        perror("Error on subscribing");
        return NULL;
    }

    for (int i = 0; i < someRoom->numOfClients; ++i) {
        tmp_arr[i] = someRoom->clients[i];
    }

    free(someRoom->clients);

    someRoom->clients = tmp_arr;
    someRoom->clients[someRoom->numOfClients] = client;
    someRoom->numOfClients += 1;

    client->room = someRoom;

    for (int i = 0; i < someRoom->numOfMessages; ++i) {
        write(client->socket, someRoom->messages[i]->senderName, 255);
        write(client->socket, someRoom->messages[i]->message, 4096);
    }


    pthread_mutex_unlock(&someRoom->roomAwaible);

    return someRoom;
}

struct room* disconnectFromRoom(struct room* someRoom, struct client* client) {
    pthread_mutex_lock(&someRoom->roomAwaible);

    int searchIndex;
    for (searchIndex = 0; searchIndex < someRoom->numOfClients; ++searchIndex) {
        if (someRoom->clients[searchIndex]->socket == client->socket) {
            break;
        }
    }

    for (int i = searchIndex; i < someRoom->numOfClients - 1; ++i) {
        someRoom->clients[i] = someRoom->clients[i + 1];
    }

    someRoom->numOfClients -= 1;

    pthread_mutex_unlock(&someRoom->roomAwaible);

    return someRoom;
}

struct room* sendMessage(struct room* someRoom, struct message* someMessage) {
    pthread_mutex_lock(&someRoom->roomAwaible);

    struct message** messages = malloc(sizeof(struct message*)*(someRoom->numOfMessages + 1));

    if (!messages) {
        perror("sending message error");
        return NULL;
    }

    for (int i = 0; i < someRoom->numOfMessages; ++i) {
        messages[i] = someRoom->messages[i];
    }

    free(someRoom->messages);

    someRoom->messages = messages;

    someRoom->messages[someRoom->numOfMessages] = someMessage;
    someRoom->numOfMessages += 1;

    for (int i = 0; i < someRoom->numOfClients; ++i) {
        printf("sending message to %d\n", someRoom->clients[i]->socket);
        write(someRoom->clients[i]->socket, someMessage->senderName, 255);
        write(someRoom->clients[i]->socket, someMessage->message, 4096);
    }

    pthread_mutex_unlock(&someRoom->roomAwaible);

    return someRoom;
}