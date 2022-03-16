#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "error_codes.h"
#include "archive.h"
#include "read-file.h"
#include "write_file.h"
#include "seek-file.h"
#include "json_stringify.h"

void sprintDir(int filedescr, char *dir, char* str, json* jason, int *count)
{
    DIR *dp;    // Указатель на поток директории
    struct dirent *entry;   // Структура данных директории
    struct stat statbuf; // Статусная информация о файле в директории
 
    if ((dp = opendir(dir)) == NULL)
    {
        perror(dir);
        return;
    }
    
    chdir(dir); // Проникаем в рассматриваемую директорию
    while ((entry = readdir(dp)) != NULL)   // Идём в глубину
    {
        lstat(entry->d_name, &statbuf); // Считываем данные о сущности
        if (entry->d_type == 4) // Тут дело такое. Сущность - файл: код 4, сущность - директория: код 8
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)    // Директории . и .. тоже попадают в обработчик, но они нам не нужны
            {
                continue;
            } 

            (*count)++;

            char *meta = malloc(sizeof(char) * 1024); // Далее: читаем мету, переводим в джейсон и загружаем в буфер
            memset(meta, 0, 1024);

            jason->name = entry->d_name;
            jason->parentDir = dir;
            jason->type = 'd';

            jsonStringify(jason, meta);
            strcat(str, meta);

            writeFile(filedescr, str, strlen(str)); // Сразу пишем в архив и чистим буфер

            free(meta);
            memset(str, 0, 4096);
            
            sprintDir(filedescr, entry->d_name, str, jason, count); // Ре курсия
        }
        else
        {
            char *abs_filename = malloc(sizeof(char) * 1024);
            memset(abs_filename, 0, 1024);
            char *meta = malloc(sizeof(char) * 1024);
            memset(meta, 0, 1024);
            char *contents = malloc(sizeof(char) * 4096);
            memset(contents, 0, 4096);

            getcwd(abs_filename, 1024); // Путь до файла, чей контент надо считать
            strcat(abs_filename, "/");
            strcat(abs_filename, entry->d_name);

            jason->name = entry->d_name;    // Так же работаем с метой
            jason->parentDir = dir;
            jason->type = 'f';
            jason->size = statbuf.st_size;

            jsonStringify(jason, meta);
            strcat(str, meta);

            int file = open(abs_filename, O_RDONLY);    // Собственно читаем контент, и пишем в буфер
            readFile(file, contents, statbuf.st_size);
            close(file);

            strcat(str, contents);

            writeFile(filedescr, str, strlen(str)); // Всю инфу в архив

            free(meta);
            free(abs_filename);
            free(contents);
            memset(str, 0, 4096);

            (*count)++;
        }
    }
    chdir("..");
    closedir(dp);   
}

void archive(char *dir, char *archName) // Можно указать полный или относительный путь до архивируемой папки. archName - имя файла, который программа будет выплевывать как архив
{
    int filedescr = open(archName, O_WRONLY | O_CREAT, 0777);   // Создаём архив

    unsigned int count = 0;

    write(filedescr, &count, sizeof(count));    // Пробел в начале файла для числа файлов в архиве

    char *buff = malloc(sizeof(char) * 4096);
    memset(buff, 0, 4096);

    json *jason = malloc(sizeof(json));

    sprintDir(filedescr, dir, buff, jason, &count);

    lseek(filedescr, 0L, SEEK_SET);
    write(filedescr, &count, sizeof(count));    // Дописываем в начало обещанное число файлов

    free(buff);
    free(jason);
    close(filedescr);
}