#ifndef OS_ARCHIVE_H
#define OS_ARCHIVE_H

#include "string-2-json.h"

/**
 * sprintDir - рекурсивно пишет информацию о папке в файл
 * @param filedescr - дескриптор файла
 * @param dir - название папки
 * @param str - указатель буффер
 * @param jason - указатель на структуру папки
 * @param count - указатель на счетчик кол-ва записей в файле
 * @param deep - глубина текущей папки
 * @return - код успеха операции
 */
int sprintDir(int filedescr, char *dir, char *str, json *jason, unsigned int *count, int deep);

/**
 * Функция архивирует указанную папку
 * @param dir - Абсолютный или относительный путь до папки, которую надо заархивировать
 * @param archName - Имя выходного файла
 * @return - код успеха операции
 */
int archive(char *dir, char *archName);

#endif