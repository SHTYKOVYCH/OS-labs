#ifndef OS_READ_FILE_H
#define OS_READ_FILE_H

/**
 * Функция читает nread байт из файда
 * @param file - дескриптор файла
 * @param buffer - буффер, в который будет записан результат
 * @param nread - кол-во байтов, которые нужно считать
 * @return - код успеха
 */
int readFile(int file, char *buffer, int nread);

#endif