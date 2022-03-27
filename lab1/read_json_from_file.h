//
// Created by dimitis on 02.03.2022.
//

#ifndef OS_READ_JSON_FROM_FILE_H
#define OS_READ_JSON_FROM_FILE_H

/**
 * Функция считывает JSON из файла
 * @param fileId - дескриптор файла
 * @param buffer - буфер, в который будет записан результат
 * @return - код успеха
 */
int readJSONFromFile(int fileId, char *buffer);

/**
 * Функция читает имя поля JSON
 * @param fileId - дескриптор файла
 * @param buffer - буффер, к который будут писаться данные
 * @param bufferIndex - индекс, с которого будут писаться данные
 * @return - код операции
 */
int readFieldName(int fileId, char *buffer, int *bufferIndex);

/**
 * Функция читает значение поля JSON
 * @param fieldId - дескриптор файла
 * @param buffer - буффер, к который будут писаться данные
 * @param bufferIndex - индекс, с которого будут писаться данные
 * @return - код операции
 */
int readFieldContent(int fieldId, char *buffer, int *bufferIndex);

#endif //OS_READ_JSON_FROM_FILE_H
