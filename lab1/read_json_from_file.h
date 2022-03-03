//
// Created by dimitis on 02.03.2022.
//

#ifndef OS_READ_JSON_FROM_FILE_H
#define OS_READ_JSON_FROM_FILE_H

int readJSONFromFile(int fileId, char* buffer);
int readFieldName(int fileId, char* buffer, int* bufferIndex);
int readFieldContent(int fieldId, char* buffer, int* bufferIndex);

#endif //OS_READ_JSON_FROM_FILE_H
