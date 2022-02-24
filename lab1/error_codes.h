//
// Created by dimitis on 23.02.2022.
//

#ifndef OS_ERROR_CODES_H
#define OS_ERROR_CODES_H

enum ERRORS {
    SUCCESS,                        // Ошибок не произошло
    WRONG_PARAMETER_ERROR,          // Параметр передан неверно, например, передано некоректное имя файла
    NOT_EXISTING_PARAMETER_ERROR,   // Передан несуществующий параметр
    MISSING_PARAMETER_ERROR,        // Не передан какой-то из обязательных параметров или после флага не передано название файла
    READING_FROM_FILE_ERROR,
    WRITING_TO_FILE_ERROR,
    FILE_STRUCTURE_ERROR,           // Ошибка в структуре файла или в структуре JSON
};

#endif //OS_ERROR_CODES_H