//
// Created by dimitis on 03.03.2022.
//

#ifndef OS_JSON_STRINGIFY_H
#define OS_JSON_STRINGIFY_H

#include "string-2-json.h"

/**
 *
 * @param number
 * @param buffer
 */
void itoa(int number, char *buffer);

/**
 * Функция обрабатывает входную строку, добавляя \ перед некоторыми символами
 * @param buffer - буффер, в который будет помещена строка
 * @param string - строка, которую надо обработать
 */
void escapeString(char *buffer, char *string);

/**
 * Функция переводит JSON в строку
 * @param JSON - структура JSON, котрую надо перевести в строку
 * @param string - буффер, в который будет переведена структура
 */
void jsonStringify(json *JSON, char *string);

#endif //OS_JSON_STRINGIFY_H
