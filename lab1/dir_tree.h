//
// Created by dimitis on 04.03.2022.
//

#ifndef OS_DIR_TREE_H
#define OS_DIR_TREE_H


struct treeNode_s
{
    char *name;
    struct treeNode_s *parentNode;
    struct treeNode_s **childNodes;
    int numOfChildren;
    int deep;
};

typedef struct treeNode_s treeNode;

/**
 * Функция создает элемент дерева папок
 * @param dirName - название папки
 * @param parentDir - указатель на базовую папку
 * @return - указатель на созданный элемент
 */
treeNode *createDirTree(char *dirName, treeNode *parentDir);

/**
 * Функция добавляет в дерево данный ей элемент
 * @param node - Базовая папка
 * @param child - Новая дочерняя папка
 * @return - указатель на родительскую папку
 */
treeNode *addDir(treeNode *node, treeNode *child);

/**
 * Функция ищет указанную директорию вверх по дереву
 * Внимание! Только верх, не в соседних
 * @param node - Директоя, от которой начинается поиск
 * @param dirName - Имя искомой директории
 * @param deep - Глубина искомой директории
 * @return - Указатель на найденную директорию или NULL
 */
treeNode *reverseFindDir(treeNode *node, char *dirName, int deep);

/**
 * Функция удаляет дерево директорий
 * @param dirTree - Указатель на корень дерева
 */
void deleteTree(treeNode *dirTree);


#endif //OS_DIR_TREE_H
