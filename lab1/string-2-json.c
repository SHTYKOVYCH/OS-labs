#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "string-2-json.h"
#include "parse-args.h"
#include "error_codes.h"

int string2json(const char *string, json *jason) {
    char buff[255];
    memset(buff, 0, 255);

    for (int i = 1; i < strlen(string) - 1; ++i) {
        buff[strlen(buff)] = string[i];    // Грузим символ
        if (!strcmp(buff, "name"))    // Если встретили параметр
        {
            i += 3;
            memset(buff, 0, strlen(buff));

            for (; i < strlen(string); ++i) {
                char c = string[i];
                if ((c == '\\') && (string[i + 1] == '\"'))    // Когда за \ следует ", запоминаем " без \ и идём дальше
                {
                    buff[strlen(buff)] = string[i + 1];
                    i++;
                    continue;
                }
                if (c == '\"')    // Если просто ", закрываемся
                {
                    jason->name = (char *) malloc(strlen(buff));
                    if (jason->name == NULL) {
                        perror("Error on allocating memory");
                        return ERROR;
                    }

                    strcpy(jason->name, parseFilename(buff, 'f'));
                    break;
                }
                buff[strlen(buff)] = c;
            }

            i += 1;
        } else if (!strcmp(buff, "type")) {
            i += 3;

            jason->type = string[i];

            i += 2;
        } else if (!strcmp(buff, "parentDir")) {
            i += 3;
            memset(buff, 0, strlen(buff));

            for (; i < strlen(string); ++i)    // Некоторые скажут: "Коля, но это же дублирование кода"
            {                                // На что я брошу загадочный взгляд в мексиканской шляпи и отвечу: "Да."
                char c = string[i];
                if ((c == '\\') && (string[i + 1] == '\"')) {
                    buff[strlen(buff)] = string[i + 1];
                    i++;
                    continue;
                }
                if (c == '\"') {
                    jason->parentDir = (char *) malloc(strlen(buff));
                    if (jason->parentDir == NULL) {
                        perror("Error on allocating memory");
                        return ERROR;
                    }

                    strcpy(jason->parentDir, parseFilename(buff, 'd'));
                    break;
                }
                buff[strlen(buff)] = c;
            }

            i += 1;
        } else if (!strcmp(buff, "size")) {
            i += 3;
            memset(buff, 0, strlen(buff));

            for (; i < strlen(string); ++i) {
                char c = string[i];
                if (c == '\"') {
                    jason->size = (unsigned int) atoi(buff);
                    i += 1;
                    break;
                } else {
                    buff[strlen(buff)] = c;
                }
            }
        } else if (!strcmp(buff, "deep")) {
            i += 3;
            memset(buff, 0, strlen(buff));

            for (; i < strlen(string); ++i) {
                char c = string[i];
                if (c == '\"') {
                    jason->deep = (unsigned) atoi(buff);
                    i += 1;
                    break;
                } else {
                    buff[strlen(buff)] = c;
                }
            }
        } else {
            continue;
        }
        memset(buff, 0, strlen(buff));
    }
    return SUCCESS;
}