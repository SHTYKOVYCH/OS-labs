//
// Created by dimitis on 23.02.2022.
//

#include <stdio.h>
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

    printf("Error: ");

    switch (error) {
        case (NOT_EXISTING_PARAMETER_ERROR):
            printf("parameter %s doesn't exists\n", additionalString);
            exit(1);

        case (READING_FROM_FILE_ERROR):
            printf("on reading from file\n");
            exit(1);

        case (WRITING_TO_FILE_ERROR):
            printf("on writing to file\n");
            exit(1);

        case (FILE_STRUCTURE_ERROR):
            printf("wrong file structure\n");
            exit(1);

        case (WRONG_PARAMETER_ERROR):
        case (MISSING_PARAMETER_ERROR):
            printf("%s\n", additionalString);

            exit(1);
    }
}
