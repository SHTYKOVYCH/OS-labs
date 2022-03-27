//
// Created by dimitis on 02.03.2022.
//

#ifndef OS_WRITE_FILE_H
#define OS_WRITE_FILE_H

/**
 * Функция пишет numOfBytes байтов в файл
 * @param fileId - дескриптор файла
 * @param buffer - буффер, который необходимо записать
 * @param numOfBytes - кол-во байт, которые надо записать
 * @return - код упеха операции
 */
int writeFile(int fileId, char *buffer, unsigned long numOfBytes);

#endif //OS_WRITE_FILE_H
