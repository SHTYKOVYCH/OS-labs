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
#include <errno.h>
#include <fcntl.h>

struct client_s
{
    int socket;
    pthread_t readThread;
};

struct client_s client;

pthread_mutex_t consoleAwaible;

void exitFunc(int sig) {
    if (client.readThread < 0) {
        pthread_cancel(client.readThread);
    }
    close(client.socket);

    exit(EXIT_SUCCESS);
}

void *clientReader() {
    do {
        char sender[256] = {0};
        char message[4096] = {0};

        if (!read(client.socket, sender, 255)) {
            perror("client: ");
            close(client.socket);
            exit(EXIT_SUCCESS);
        }

        if (!read(client.socket, message, 4096)) {
            perror("client: ");
            close(client.socket);
            exit(EXIT_SUCCESS);
        }

        printf("\r%s : %s", sender, message);
    } while (1);
}

int main(int argc, char **argv) {
    if (argc == 1 || argc == 2) {
        printf("usage: client <adrr> <port>");

        exit(EXIT_FAILURE);
    }

    client.readThread = -1;

    int sock;

    struct sockaddr_in socketParams;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    socketParams.sin_family = AF_INET;
    socketParams.sin_addr.s_addr = inet_addr(argv[1]);
    socketParams.sin_port = htons((short) atol(argv[2]));

    int length = sizeof(socketParams);

    int result = connect(sock, (struct sockaddr *) &socketParams, length);

    if (result == -1) {
        printf("client: %d", errno);
        exit(EXIT_FAILURE);
    }

    char buffer;

    if (!read(sock, &buffer, 1)) {
        printf("client: %d", errno);
        close(sock);
        exit(EXIT_SUCCESS);
    }

    if (buffer != 't') {
        close(sock);
        printf("client: server error\n");
        exit(EXIT_SUCCESS);
    }

    char name[255] = {0};
    printf("Enter your name: ");
    scanf("%s", name);

    write(sock, name, strlen(name) + 1);

    if (!read(sock, &buffer, 1)) {
        printf("client: %d", errno);
        close(sock);
        exit(EXIT_SUCCESS);
    }

    if (buffer != 't') {
        printf("client: %d", errno);
        close(sock);
        exit(EXIT_SUCCESS);
    }


    int numOfRooms;
    rooms_printing:
    if (!read(sock, &numOfRooms, 4)) {
        printf("client: %d", errno);
        close(sock);
        exit(EXIT_SUCCESS);
    }

    if (numOfRooms == 0) {
        printf("No rooms to display!\n");
    } else {
        printf("Rooms total %d\n", numOfRooms);
        for (int i = 0; i < numOfRooms; ++i) {
            int roomId;
            char roomName[256] = {0};
            if (!read(sock, &roomId, 4)) {
                close(sock);
                printf("client: server error\n");
                exit(EXIT_SUCCESS);
            }

            if (!read(sock, roomName, 255)) {
                close(sock);
                printf("client: server error\n");
                exit(EXIT_SUCCESS);
            }

            printf("Room %d - %s\n", roomId, roomName);
        }
    }

    int userChoice;

    if (numOfRooms == 0) {
        printf("Commands:\n1 - Create room\n");
    } else {
        printf("Commands:\n1 - Create room\n2 - Join room\n");
    }

    do {
        printf(" > ");
        scanf("%d", &userChoice);
    } while (userChoice != 1 && userChoice != 2);

    if (userChoice == 1) {
        char roomName[256];
        printf("Enter new room's name: ");
        scanf("%s", roomName);

        write(sock, "CR", 2);
        write(sock, roomName, strlen(roomName) + 1);

        if (!read(sock, &buffer, 1)) {
            perror("client");
            exit(EXIT_SUCCESS);
        }

        if (buffer != 't') {
            close(sock);
            printf("client: server error\n");
            exit(EXIT_SUCCESS);
        }

        goto rooms_printing;
    }

    int roomId;
    printf("Enter room's number: ");
    scanf("%d", &roomId);

    write(sock, &roomId, 4);

    if (!read(sock, &buffer, 1)) {
        perror("client");
        exit(EXIT_SUCCESS);
    }

    if (buffer != 't') {
        close(sock);
        printf("client: server error\n");
        exit(EXIT_SUCCESS);
    }

    client.socket = sock;

    pthread_attr_t readThreadParams;
    pthread_attr_init(&readThreadParams);
    pthread_attr_setdetachstate(&readThreadParams, PTHREAD_CREATE_DETACHED);

    if (pthread_create(&(client.readThread), &readThreadParams, clientReader, NULL)) {
        printf("client: %d", errno);
        exit(EXIT_FAILURE);
    }

    getchar();
    do {
        char message[4096] = {0};
        int needle = -1;

        do {
            needle += 1;
            message[needle] = getchar();
        } while (message[needle] != '\n');

        message[needle] = 0;

        if (needle == 0) {
            continue;
        }

        write(sock, message, needle);
    } while (1);
}