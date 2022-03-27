//
// Created by dimitis on 02.03.2022.
//
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "read_json_from_file.h"
#include "read-file.h"
#include "error_codes.h"

int readFieldName(int fileId, char *buffer, int *bufferIndex) {
    while (1) {
        int errorCode;

        errorCode = readFile(fileId, &buffer[*bufferIndex], 1);

        if (errorCode != SUCCESS) {
            return errorCode;
        }

        if (buffer[*bufferIndex] == ':') {
            *bufferIndex += 1;
            return SUCCESS;
        }

        *bufferIndex += 1;
    }
}

int readFieldContent(int fileId, char *buffer, int *bufferIndex) {
    int errorCode = readFile(fileId, &buffer[*bufferIndex], 1);

    if (errorCode != SUCCESS) {
        return errorCode;
    }

    if (buffer[*bufferIndex] != '\"') {
        printf("Error: wrong file structure");
        return ERROR;
    }

    *bufferIndex += 1;

    while (1) {
        errorCode = readFile(fileId, &buffer[*bufferIndex], 1);

        if (errorCode != SUCCESS) {
            return errorCode;
        }

        if (buffer[*bufferIndex] == '\\') {
            *bufferIndex += 1;
            continue;
        }

        if (buffer[*bufferIndex] == '\"' && buffer[*bufferIndex - 1] != '\\') {
            *bufferIndex += 1;
            return SUCCESS;
        }
        *bufferIndex += 1;
    }
}

int readJSONFromFile(int fileId, char *accumulator) {
    memset(accumulator, 0, 1024);

    int errorCode = readFile(fileId, accumulator, 1);

    if (errorCode != SUCCESS) {
        return errorCode;
    }

    if (accumulator[0] != '{') {
        printf("Error: wrong file structure");
        return ERROR;
    }

    int bufferIndex = 1;
    while (1) {
        errorCode = readFile(fileId, &accumulator[bufferIndex], 1);

        if (errorCode != SUCCESS && errorCode != END_OF_FILE) {
            return errorCode;
        }

        if (accumulator[bufferIndex] == '}') {
            return SUCCESS;
        }

        bufferIndex += 1;
        errorCode = readFieldName(fileId, accumulator, &bufferIndex);

        if (errorCode == END_OF_FILE) {
            if (accumulator[bufferIndex - 1] != '}') {
                printf("Error: wrong file structure");
                return ERROR;
            }

            return SUCCESS;
        }

        if (errorCode != SUCCESS) {
            return errorCode;
        }

        errorCode = readFieldContent(fileId, accumulator, &bufferIndex);

        if (errorCode != SUCCESS) {
            return errorCode;
        }

        errorCode = readFile(fileId, &accumulator[bufferIndex], 1);

        if (errorCode != SUCCESS) {
            return errorCode;
        }

        if (accumulator[bufferIndex] != ',') {
            printf("Error: wrong file structure");
            return ERROR;
        }

        bufferIndex += 1;
    }
}