//
// Created by dimitis on 04.03.2022.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dir_tree.h"
#include "error_codes.h"


treeNode *createDirTree(char *dirName, treeNode *parentDir) {
    treeNode *dirRoot;
    if ((dirRoot = malloc(sizeof(treeNode))) == NULL) {
        perror("Error on creating dir tree:");
        return NULL;
    }

    dirRoot->name = malloc(strlen(dirName) + 1);
    strcpy(dirRoot->name, dirName);
    dirRoot->parentNode = parentDir;
    dirRoot->childNodes = NULL;
    dirRoot->numOfChildren = 0;
    dirRoot->deep = 0;

    return dirRoot;
}

treeNode *addDir(treeNode *node, treeNode *child) {
    node->childNodes = realloc(node->childNodes, node->numOfChildren * sizeof(treeNode *) + sizeof(treeNode *));
    if (node->childNodes == NULL) {
        perror("Error on allocating memory");
        return NULL;
    }

    node->childNodes[node->numOfChildren] = child;
    node->numOfChildren += 1;

    child->parentNode = node;
    child->deep = node->deep + 1;

    return child;
}

treeNode *reverseFindDir(treeNode *root, char *dirName, int deep) {
    if (root->parentNode != NULL && root->deep > -1) {
        if (!strcmp(root->parentNode->name, dirName)) {
            return root->parentNode;
        }

        return reverseFindDir(root->parentNode, dirName, deep);
    }

    return NULL;
}

void deleteTree(treeNode *root) {
    if (root == NULL) {
        return;
    }

    if (root->childNodes != NULL) {
        for (int i = 0; i < root->numOfChildren; ++i) {
            deleteTree(root->childNodes[i]);
        }

        free(root->childNodes);
    }

    free(root->name);
    free(root);
}