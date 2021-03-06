#ifndef OS_STRING_TO_JSON_H
#define OS_STRING_TO_JSON_H

struct json_s
{
    char *name;
    char *parentDir;
    char type;
    unsigned int deep;
    unsigned int size;
};

typedef struct json_s json;

/**
 * Функция переводит строку в структуру JSON
 * @param string - строка, которую надо перевести в структуру
 * @param jason - указатель на структуру
 * @return - код успеха операции
 */
int string2json(const char *string, json *jason);

#endif