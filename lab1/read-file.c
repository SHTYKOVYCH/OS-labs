#include <stdlib.h>
#include <unistd.h>

#include "read-file.h"
#include "error_codes.h"

int readFile(int file, char* buffer, int nread)
{
    int numOfReadedBytes = read(file, buffer, nread);

    if (numOfReadedBytes == 0) {
        return END_OF_FILE;
    }

    if (numOfReadedBytes == -1) {
        return READING_FROM_FILE_ERROR;
    }


    return SUCCESS;
}