//
// Created by dimitis on 23.02.2022.
//

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "error_codes.h"
#include "error_handler.h"

/**
 * Функция принимает код ошибки, а так же дополнительну строку с информацией.
 *
 * @param error
 * @param additionalString
 */

void errorHandler(int error, const char* additionalString)
{
    if (error == SUCCESS) {
        return;
    }

    char* errorString = malloc(sizeof(char) * 500);
    memset(errorString, 0, 500);

    strcat(errorString, "Error: ");

    switch (error) {
        case (NOT_EXISTING_PARAMETER_ERROR):
            strcat(errorString, "parameter: ");
            strcat(errorString, additionalString);
            strcat(errorString, " doesn't exists\n");
            break;
        case (READING_FROM_FILE_ERROR):
            strcat(errorString, "on reading from file ");
            strcat(errorString, additionalString);
            strcat(errorString, "\n");
            break;
        case (WRITING_TO_FILE_ERROR):
            strcat(errorString, "on writing to file ");
            strcat(errorString, additionalString);
            strcat(errorString, "\n");
            break;
        case (FILE_STRUCTURE_ERROR):
            strcat(errorString, "wrong file structure\n");
            break;
        case (WRONG_PARAMETER_ERROR):
        case (MISSING_PARAMETER_ERROR):
            strcat(errorString, additionalString);
            strcat(errorString, "\n");
            break;
    }

    write(2, errorString, strlen(errorString));
    free(errorString);
}