//
// Created by dimitis on 02.03.2022.
//

#include <unistd.h>
#include <stdio.h>

#include "error_codes.h"
#include "write_file.h"


int writeFile(int fileId, char *buffer, unsigned long numOfBytes) {
    if (write(fileId, buffer, numOfBytes) == -1) {
        perror("Error on writing to file");
        return ERROR;
    }

    return SUCCESS;
}