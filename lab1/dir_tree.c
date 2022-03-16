//
// Created by dimitis on 04.03.2022.
//

#include <stdlib.h>
#include <string.h>

#include "dir_tree.h"

treeNode* createDirTree(char* dirName, treeNode* parentDir)
{
    treeNode* dirRoot = malloc(sizeof(treeNode));

    dirRoot->name = malloc(strlen(dirName)+1);
    strcpy(dirRoot->name, dirName);
    dirRoot->parentNode = parentDir;
    dirRoot->childNodes = NULL;
    dirRoot->numOfChildren = 0;

    return dirRoot;
}

treeNode* addDir(treeNode* node, treeNode* child)
{
    node->childNodes = realloc(node->childNodes, node->numOfChildren*sizeof(treeNode*) + sizeof(treeNode*));

    node->childNodes[node->numOfChildren] = child;
    child->parentNode = node;
    node->numOfChildren += 1;

    return child;
}

treeNode* findDir(treeNode* root, char* dirName)
{
    if (!strcmp(root->name, dirName)) {
        return root;
    }

    if (root->numOfChildren > 0) {
        for (int i = root->numOfChildren - 1; i > -1; --i) {
            treeNode* result = findDir(root->childNodes[i], dirName);

            if (result != NULL) {
                return result;
            }
        }
    }

    return NULL;
}

void deleteTree(treeNode* root) {

    if (root->childNodes != NULL) {
        for (int i = 0; i < root->numOfChildren; ++i) {
            deleteTree(root->childNodes[i]);
        }

        free(root->childNodes);
    }

    free(root->name);
    free(root);
}