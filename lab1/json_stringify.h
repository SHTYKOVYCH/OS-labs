//
// Created by dimitis on 03.03.2022.
//

#ifndef OS_JSON_STRINGIFY_H
#define OS_JSON_STRINGIFY_H

#include "string-2-json.h"

void itoa(int number, char* buffer);
void escapeString(char* buffer, char* string);
void jsonStringify(json* JSON, char* string);

#endif //OS_JSON_STRINGIFY_H
