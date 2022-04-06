#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "archive.h"
#include "read-file.h"
#include "write_file.h"
#include "json_stringify.h"
#include "error_codes.h"

int sprintDir(int filedescr, char *dir, json *jason, unsigned int *count, int deep) {
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    if ((dp = opendir(dir)) == NULL) {
        perror(dir);
        return ERROR;
    }

    chdir(dir);
    while ((entry = readdir(dp)) != NULL) {
        // Считываем данные о сущности
        if (lstat(entry->d_name, &statbuf)) {
            perror("Error on reading dir stats");
        }

        // Если сущность - папка
        if (entry->d_type == 4) {
            // Отсеиваем ссылки . и ..
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            (*count)++;

            char *meta;
            if ((meta = malloc(sizeof(char) * 1024)) == NULL) {
                perror("Error on allocating memory");
                return ERROR;
            }

            memset(meta, 0, 1024);

            jason->name = entry->d_name;
            jason->parentDir = dir;
            jason->type = 'd';
            jason->deep = deep;

            jsonStringify(jason, meta);

            if (writeFile(filedescr, meta, strlen(meta)) != SUCCESS) {
                return ERROR;
            }

            free(meta);

            if (sprintDir(filedescr, entry->d_name, jason, count, deep + 1) != SUCCESS) {
                return ERROR;
            }
        } else {
            // если файл
            char *abs_filename;
            if ((abs_filename = malloc(sizeof(char) * 1024)) == NULL) {
                perror("Error on allocating memory");
                return ERROR;
            }
            memset(abs_filename, 0, 1024);

            char *meta;
            if ((meta = malloc(sizeof(char) * 1024)) == NULL) {
                perror("Error on allocating memory");
                return ERROR;
            }
            memset(meta, 0, 1024);

            char *contents;
            if ((contents = malloc(sizeof(char) * statbuf.st_size)) == NULL) {
                perror("Error on allocating memory");
                return ERROR;
            }
            memset(contents, 0, statbuf.st_size);

            char *str;
            if ((str = malloc(sizeof(char) * (2048 + statbuf.st_size))) == NULL) {
                perror("Error on allocating memory");
                return ERROR;
            }
            memset(str, 0, (2048 + statbuf.st_size));

            // Путь до файла, чей контент надо считать
            if (getcwd(abs_filename, 1024) == NULL) {
                perror("Error on getting path to file");
                return ERROR;
            }

            strcat(abs_filename, "/");
            strcat(abs_filename, entry->d_name);

            jason->name = entry->d_name;
            jason->parentDir = dir;
            jason->type = 'f';
            jason->size = statbuf.st_size;
            jason->deep = deep;

            jsonStringify(jason, meta);
            strcat(str, meta);

            int file = open(abs_filename, O_RDONLY);
            if (file == -1) {
                perror("Error on opening file");
                free(meta);
                free(abs_filename);
                free(contents);
                free(str);
                close(file);
                return ERROR;
            }

            if (readFile(file, contents, statbuf.st_size) != SUCCESS) {
                free(meta);
                free(abs_filename);
                free(contents);
                free(str);
                close(file);
                return ERROR;
            }
            close(file);

            if (writeFile(filedescr, str, strlen(str)) != SUCCESS) {
                perror("Error on writing to file");
                free(meta);
                free(abs_filename);
                free(contents);
                free(str);
                close(file);
                return ERROR;
            }

            if (writeFile(filedescr, contents, statbuf.st_size) != SUCCESS) {
                perror("Error on writing to file");
                free(meta);
                free(abs_filename);
                free(contents);
                free(str);
                close(file);
                return ERROR;
            }

            free(meta);
            free(abs_filename);
            free(contents);
            free(str);

            (*count)++;
        }
    }
    chdir("..");
    closedir(dp);
}

int archive(char *dir, char *archName) {
    int filedescr = open(archName, O_WRONLY | O_CREAT | O_TRUNC, 0777);

    if (filedescr == -1) {
        perror("Erro on opening file");
        return ERROR;
    }

    unsigned int count = 0;

    // Оставляем место под кол-во записей
    if (write(filedescr, &count, sizeof(count)) == -1) {
        perror("Error on writing to file");
        close(filedescr);
        return ERROR;
    }

    // Пишем информацию о базовой директории
    json *jason = malloc(sizeof(json));
    if (jason == NULL) {
        perror("Error on allocating memory");
        return ERROR;
    }

    jason->name = dir;
    jason->deep = 0;
    jason->parentDir = ".";
    jason->type = 'd';

    char buffer[1024] = {0};
    jsonStringify(jason, buffer);
    if (writeFile(filedescr, buffer, strlen(buffer)) != SUCCESS ||
        sprintDir(filedescr, dir, jason, &count, 1) != SUCCESS) {
        free(jason);
        close(filedescr);
        return ERROR;
    }

    // Пишем в начало кол-во записей в архиве
    if (lseek(filedescr, 0L, SEEK_SET) == -1) {
        perror("Error on moving pointer in file");
        free(jason);
        close(filedescr);
        return ERROR;
    }

    count += 1;

    if (write(filedescr, &count, sizeof(count)) == -1) {
        perror("Error on writing to file");
        free(jason);
        close(filedescr);
        return ERROR;
    }

    free(jason);
    close(filedescr);
}