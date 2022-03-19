//
// Created by dimitis on 04.03.2022.
//

#ifndef OS_DEPACK_H
#define OS_DEPACK_H

#include "parse-args.h"
#include "dir_tree.h"
#include "string-2-json.h"

char* assemblePath(treeNode* node, char* buffer);
void createDir(json* record, treeNode* dirTree);
void createFile(json* record, treeNode* dirTree, int fileId);
int depack(Args* args);

#endif //OS_DEPACK_H
