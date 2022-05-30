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

#include <gtk/gtk.h>

struct client_s
{
    int socket;
    pthread_t readThread;
};

struct client_s client;

struct login_screen
{
    GtkWidget *window;
    GtkWidget *main_box;
    GtkEntryBuffer *host;
    GtkEntryBuffer *port;
    GtkEntryBuffer *name;
};

struct rooms_screen
{
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *rooms_box;
    GtkEntryBuffer *create_room_buffer;
};

struct messenger_screen
{
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *message_box;
    GtkWidget *message_scroll;
    GtkEntryBuffer *message_buffer;
};

struct login_screen *screen_1;
struct rooms_screen *screen_2;
struct messenger_screen *screen_3;

void *clientReader() {


    fd_set socket;

    FD_ZERO(&socket);
    FD_SET((client.socket), &socket);

    do {
        int result = select(FD_SETSIZE, &socket, NULL, NULL, NULL);

        char sender[256] = {0};
        char message[4097] = {0};

        if (read(client.socket, sender, 256) == 0) {
            printf("client error: connection dropped\n");
            close(client.socket);
            gtk_window_destroy(screen_1->window);
            return (void*) NULL;
        }
        g_print("Sender received: %s\n", sender);

        if (read(client.socket, message, 4097) == 0) {
            printf("client error: connection dropped\n");
            close(client.socket);
            gtk_window_destroy(screen_1->window);
            return (void*) NULL;
        }
        g_print("Message received: %s\n", message);

        GtkWidget *messageBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
        GtkWidget *senderLabel = gtk_label_new(sender);
        GtkWidget *messageLabel = gtk_label_new(message);

        gtk_box_append(messageBox, senderLabel);
        gtk_box_append(messageBox, messageLabel);

        gtk_box_append(screen_3->message_box, messageBox);
    } while (1);
}

static void sendMessage() {
    if (!gtk_entry_buffer_get_length(screen_3->message_buffer)) {
        return;
    }

    char message[4097] = {0};
    strcpy(message, gtk_entry_buffer_get_text(screen_3->message_buffer));

    write(client.socket, message, 4097);
    gtk_entry_buffer_set_text(screen_3->message_buffer, "", 0);
}

void connectToRoom(GtkButton *btn, gpointer roomId) {
    write(client.socket, "RC", 2);

    int room = *(int*) roomId;

    g_print("client: room id %d\n", room);
    char buffer[4096] = {0};

    screen_3 = malloc(sizeof(struct messenger_screen));

    write(client.socket, &room, 4);

    if (!read(client.socket, &buffer, 1)) {
        perror("network error: joining room");
        exit(EXIT_SUCCESS);
    }

    if (strncmp(buffer, "t", 1)) {
        g_print("client error: connection to server dropped\n");
        close(client.socket);
        gtk_window_destroy(screen_1->window);
        return;
    }

    if (strncmp(buffer, "t", 1)) {
        g_print("client error: connection to server dropped\n");
        close(client.socket);
        gtk_window_destroy(screen_1->window);
        return;
    }

    screen_3->window = screen_2->window;
    screen_3->main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    screen_3->message_scroll = gtk_scrolled_window_new();
    screen_3->message_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    gtk_scrolled_window_set_child(screen_3->message_scroll, screen_3->message_box);
    gtk_box_append(screen_3->main_box, screen_3->message_scroll);

    screen_3->message_buffer = gtk_entry_buffer_new("", 0);
    GtkWidget *inputBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *input = gtk_entry_new_with_buffer(screen_3->message_buffer);
    GtkWidget *button = gtk_button_new_with_label("Send");

    gtk_box_append(inputBox, input);
    gtk_box_append(inputBox, button);

    gtk_box_append(screen_3->main_box, inputBox);

    g_signal_connect(button, "clicked", G_CALLBACK(sendMessage), NULL);

    gtk_window_set_child(screen_3->window, screen_3->main_box);

    pthread_attr_t readThreadParams;
    pthread_attr_init(&readThreadParams);
    pthread_attr_setdetachstate(&readThreadParams, PTHREAD_CREATE_DETACHED);

    if (pthread_create(&(client.readThread), &readThreadParams, clientReader, NULL)) {
        printf("client: %d", errno);
        exit(EXIT_FAILURE);
    }
}

void getRooms() {
    gtk_box_remove(screen_2->main_box, screen_2->rooms_box);

    screen_2->rooms_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_prepend(screen_2->main_box, screen_2->rooms_box);

    int numOfRooms;
    if (read(client.socket, &numOfRooms, 4) == 0) {
        g_print("client error: connection to server dropped\n");
        close(client.socket);
        gtk_window_destroy(screen_1->window);
        return;
    }

    if (numOfRooms == 0) {
        GtkWidget *label = gtk_label_new("No rooms awaible");
        gtk_box_append(screen_2->rooms_box, label);
    } else {
        for (int i = 0; i < numOfRooms; ++i) {
            int *roomId = malloc(sizeof(int));
            char roomName[256] = {0};

            if (read(client.socket, roomId, 4) == 0) {
                g_print("client error: connection to server dropped\n");
                close(client.socket);
                gtk_window_destroy(screen_1->window);
                return;
            }

            if (read(client.socket, roomName, 256) == 0) {
                g_print("client error: connection to server dropped\n");
                close(client.socket);
                gtk_window_destroy(screen_1->window);
                return;
            }

            GtkWidget *button = gtk_button_new_with_label(roomName);
            gtk_box_append(screen_2->rooms_box, button);
            g_signal_connect(button, "clicked", G_CALLBACK(connectToRoom), roomId);
        }
    }
}

static void createRoom(GtkButton *btn, gpointer data) {
    if (!gtk_entry_buffer_get_length(screen_2->create_room_buffer)) {
        return;
    }

    char buffer[4096] = {0};
    strcpy(buffer, gtk_entry_buffer_get_text(screen_2->create_room_buffer));

    write(client.socket, "CR", 2);
    write(client.socket, buffer, 256);

    if (read(client.socket, buffer, 1) == 0) {
        g_print("client error: connection to server dropped\n");
        close(client.socket);
        gtk_window_destroy(screen_1->window);
        return;
    }

    buffer[1] = 0;
    if (strncmp(buffer, "t", 1)) {
        printf("client error: server reported in error. try again\n");
        return;
    }

    gtk_entry_buffer_set_text(screen_2->create_room_buffer, "", 0);

    getRooms();
}

static void connectToServer(GtkButton *btn, gpointer data) {
    struct login_screen *screen = data;

    if (!(gtk_entry_buffer_get_length(screen->host)
          && gtk_entry_buffer_get_length(screen->port)
          && gtk_entry_buffer_get_length(screen->name))) {
        return;
    }

    int sock;
    struct sockaddr_in socketParams;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    socketParams.sin_family = AF_INET;
    socketParams.sin_addr.s_addr = inet_addr(gtk_entry_buffer_get_text(screen->host));
    socketParams.sin_port = htons((short) atol(gtk_entry_buffer_get_text(screen->port)));
    int length = sizeof(socketParams);
    int result = connect(sock, (struct sockaddr *) &socketParams, length);
    client.socket = sock;

    if (result == -1) {
        g_print("client error: connection to server failed\n");
        close(sock);
        gtk_window_destroy(screen_1->window);
        return;
    }

    char buffer[2];
    if (read(sock, &buffer, 1) == 0) {
        g_print("client error: connection to server dropped\n");
        close(sock);
        gtk_window_destroy(screen_1->window);
        return;
    }

    if (strncmp(buffer, "t", 1)) {
        printf("client error: server reported in error\n");
        close(sock);
        gtk_window_destroy(screen_1->window);
        return;
    }

    char name[256] = {0};
    strcpy(name, gtk_entry_buffer_get_text(screen->name));

    write(sock, name, 255);

    if (read(sock, &buffer, 1) == 0) {
        g_print("client error: server connection dropped");
        close(sock);
        gtk_window_destroy(screen_1->window);
        return;
    }

    if (strncmp(buffer, "t", 1)) {
        printf("client error: server reported in error\n");
        close(sock);
        gtk_window_destroy(screen_1->window);
        return;
    }

    screen_2 = malloc(sizeof(struct rooms_screen));
    screen_2->window = screen_1->window;
    screen_2->main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    screen_2->rooms_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(screen_2->main_box, screen_2->rooms_box);

    getRooms();

    GtkWidget *inputField = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    screen_2->create_room_buffer = gtk_entry_buffer_new("", 0);
    GtkWidget *userInput = gtk_entry_new_with_buffer(screen_2->create_room_buffer);
    gtk_box_append(inputField, userInput);
    GtkWidget *button = gtk_button_new_with_label("Create room");
    gtk_box_append(inputField, button);

    gtk_box_append(screen_2->main_box, inputField);

    gtk_window_set_child(screen_2->window, screen_2->main_box);

    g_signal_connect(button, "clicked", G_CALLBACK(createRoom), screen_2);
}

static void
app_activate(GApplication *app, gpointer user_data) {
    GtkWidget *win;

    win = gtk_application_window_new(GTK_APPLICATION (app));
    gtk_window_set_title(GTK_WINDOW (win), "Client");

    screen_1 = malloc(sizeof(struct login_screen));
    screen_1->window = win;

    screen_1->main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    screen_1->host = gtk_entry_buffer_new("", 0);
    screen_1->port = gtk_entry_buffer_new("", 0);
    screen_1->name = gtk_entry_buffer_new("", 0);

    GtkWidget *inputBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *label = gtk_label_new("host");
    GtkWidget *inputField = gtk_entry_new_with_buffer(screen_1->host);
    gtk_box_append(inputBox, label);
    gtk_box_append(inputBox, inputField);
    gtk_box_append(screen_1->main_box, inputBox);
    gtk_widget_set_halign(inputBox, GTK_ALIGN_END);


    inputBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    label = gtk_label_new("port");
    inputField = gtk_entry_new_with_buffer(screen_1->port);
    gtk_box_append(inputBox, label);
    gtk_box_append(inputBox, inputField);
    gtk_box_append(screen_1->main_box, inputBox);
    gtk_widget_set_halign(inputBox, GTK_ALIGN_END);


    inputBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    label = gtk_label_new("username");
    inputField = gtk_entry_new_with_buffer(screen_1->name);
    gtk_box_append(inputBox, label);
    gtk_box_append(inputBox, inputField);
    gtk_box_append(screen_1->main_box, inputBox);
    gtk_widget_set_halign(inputBox, GTK_ALIGN_END);


    GtkWidget *button = gtk_button_new_with_label("Connect");
    gtk_box_append(screen_1->main_box, button);

    g_signal_connect(button, "clicked", G_CALLBACK(connectToServer), screen_1);

    gtk_window_set_child(screen_1->window, screen_1->main_box);
    gtk_widget_show(win);
}

int main(int argc, char **argv) {
    screen_1 = screen_2 = screen_3 = NULL;

    GtkApplication *app;
    int stat;

    app = gtk_application_new("dmitrii.messenger.client", G_APPLICATION_NON_UNIQUE);
    g_signal_connect (app, "activate", G_CALLBACK(app_activate), NULL);
    stat = g_application_run(G_APPLICATION (app), argc, argv);
    g_object_unref(app);

    close(client.socket);

    if (screen_1) {
        free(screen_1);
    }
    if (screen_2) {
        free(screen_2);
    }
    if (screen_3) {
        free(screen_3);
    }

    if (client.readThread >= 0) {
        pthread_cancel(client.readThread);
    }
    close(client.socket);
    return stat;
}