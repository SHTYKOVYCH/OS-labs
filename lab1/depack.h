//
// Created by dimitis on 04.03.2022.
//

#ifndef OS_DEPACK_H
#define OS_DEPACK_H

#include "parse-args.h"
#include "dir_tree.h"
#include "string-2-json.h"

/**
 * Функци собирает путь до указаной папки
 *
 * @param node - указатель на элемент дерева папок
 * @param buffer - укзатель на буффер(минимальный размер - 1 байт), который будет содержать в себе путь до папки
 * @return - указатель на динамическую строку - путь до папки
 */
char *assemblePath(treeNode *node, char *buffer);

/**
 * Функция распаковывает архив
 * @param args - входные параметры функции
 * @return - код выполнения операции
 */
int depack(Args *args);

#endif //OS_DEPACK_H
