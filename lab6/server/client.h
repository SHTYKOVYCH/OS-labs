#ifndef MESSENGER_CLIENT_H
#define MESSENGER_CLIENT_H

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "room.h"

struct client {
    int socket;
    char name[255];
    struct room* room;
    pthread_t readThread;
};

struct client* acceptClient(int);
void *clientReader(void*);

#endif //MESSENGER_CLIENT_H
