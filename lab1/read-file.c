#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "read-file.h"
#include "error_codes.h"

int readFile(int file, char *buffer, int nread) {
    int numOfReadedBytes = read(file, buffer, nread);

    if (numOfReadedBytes == -1) {
        perror("Error on reading from file");
        return ERROR;
    }

    if (numOfReadedBytes == 0) {
        return END_OF_FILE;
    }

    return SUCCESS;
}