#ifndef OS_PARSE_ARGS_H
#define OS_PARSE_ARGS_H

struct Args_s
{
    int depack;
    char *inputFile;
    char *outputFile;
};

typedef struct Args_s Args;

/**
 * Функция парсит переданное имя, определяя его корректность
 * @param filename - название записи
 * @param type - тип записи
 * @return - string - имя файла
 */
char *parseFilename(char *filename, char type);

/**
 * Функция парсит входные аргументы
 * @param argc - кол-во аргументов
 * @param argv - аргументы
 * @param args - выходная структура с распарсеными аргументами
 * @return - bool - успешность операции
 */
int parseArgs(int argc, char **argv, Args *args);

#endif