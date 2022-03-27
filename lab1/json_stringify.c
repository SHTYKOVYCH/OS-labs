//
// Created by dimitis on 03.03.2022.
//

#include <string.h>
#include <stdio.h>

#include "json_stringify.h"

void itoa(int number, char *buffer) {
    sprintf(buffer, "%d", number);
}

void escapeString(char *buffer, char *string) {
    int i;
    for (i = 0; i < strlen(string); i += 1) {
        if (string[i] == '\"') {
            strcat(buffer, "\\");
        }

        buffer[strlen(buffer) + 1] = 0;
        buffer[strlen(buffer)] = string[i];
    }
}

void jsonStringify(json *JSON, char *string) {
    memset(string, 0, 1024);

    strcat(string, "{");

    strcat(string, "name:\"");
    escapeString(string, JSON->name);
    strcat(string, "\",");

    strcat(string, "parentDir:\"");
    escapeString(string, JSON->parentDir);
    strcat(string, "\",");

    strcat(string, "type:\"");
    if (JSON->type == 'f') {
        strcat(string, "f");
    } else {
        strcat(string, "d");
    }
    strcat(string, "\",");

    char buff[256];

    if (JSON->type == 'f') {
        memset(buff, 0, 256);
        strcat(string, "size:\"");
        itoa(JSON->size, buff);
        strcat(string, buff);
        strcat(string, "\",");
    }

    memset(buff, 0, 256);
    strcat(string, "deep:\"");
    itoa(JSON->deep, buff);
    strcat(string, buff);
    strcat(string, "\",");

    strcat(string, "}");
}