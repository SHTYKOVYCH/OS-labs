#include <stdio.h>
#include <string.h>

#include "error_codes.h"
#include "error_handler.h"
#include "parse-args.h"

char *parseFilename(char *filename, char type)    // 'd' - директория, 'f' - файл
{
    if (strlen(filename) >= 255) {
        errorHandler(WRONG_PARAMETER_ERROR, "Имя файла/директории должно содержать менее 255 символов");
        exit(1);
    }

    // Исключение запрещённых символов в имени файла. Имя директории не может содержать путь
    if (strstr(filename, "/")) {
        errorHandler(WRONG_PARAMETER_ERROR, "Имя файла/директории не должно содержать \'/\'");
        exit(1);
    }

    // Если директория, она вполне может зваться . или ..
    if (type == 'd') {
        return filename;
    }

    // Зарезервированные имена
    if (!strcmp(filename, ".") || !strcmp(filename, "..")) {
        errorHandler(WRONG_PARAMETER_ERROR, "Имя файла зарезервировано");
        exit(1);
    }

    return filename;
}

int parseArgs(int argc, char **argv, Args *args) {
    args->depack = 0;
    args->outputFile = "archive";
    args->inputFile = "";

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (!strcmp(arg, "-d")) {
            args->depack = 1;
        } else if (!strcmp(arg, "-i")) {
            if (i + 1 == argc)    // Будем читать следующий элемент, если возможно
            {
                printf("Error: input filename required after flag -i");
                return ERROR;
            } else {
                args->inputFile = parseFilename(argv[i + 1], 'f');
                ++i;
            }
        } else if (!strcmp(arg, "-o")) {
            if (i + 1 == argc) {
                printf("Error: output filename required after flag -o");
                return ERROR;
            } else {
                args->outputFile = parseFilename(argv[i + 1], 'f');
                ++i;
            }
        } else {
            return ERROR;
        }
    }

    // Если так и не получили имя входного файла
    if (!strcmp(args->inputFile, "")) {
        printf("Error: input filename required");
        return ERROR;
    }

    return SUCCESS;
}