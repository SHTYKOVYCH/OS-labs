//
// Created by dimitis on 02.03.2022.
//

#include <unistd.h>

#include "error_codes.h"
#include "write_file.h"

int writeFile(int fileId, char* buffer, int numOfBytes)
{
    if (write(fileId, buffer, numOfBytes) != numOfBytes) {
        return WRITING_TO_FILE_ERROR;
    }

    return SUCCESS;
}