#ifndef MESSENGER_ROMO_H
#define MESSENGER_ROMO_H

#include <pthread.h>

#include "client.h"

struct message {
    char message[4097];
    char senderName[256];
};

struct room {
    int id;
    char name[256];

    pthread_mutex_t roomAwaible;

    struct client** clients;
    int numOfClients;

    struct message** messages;
    int numOfMessages;
};

struct rooms_s {
    int last_id;
    struct room** rooms;
    int numOfRooms;
    pthread_mutex_t roomsAwaible;
};

extern struct rooms_s rooms;

struct room* createRoom(char* name);
void deleteRoom(struct room*);
struct room* connectToRoom(struct room*, struct client*);
struct room* disconnectFromRoom(struct room*, struct client*);
struct room* sendMessage(struct room*, struct message*);

#endif