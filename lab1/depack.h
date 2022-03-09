//
// Created by dimitis on 04.03.2022.
//

#ifndef OS_DEPACK_H
#define OS_DEPACK_H

#include "parse-args.h"
#include "dir_tree.h"

char* assemblePath(treeNode* node, char* buffer);
int depack(Args* args);

#endif //OS_DEPACK_H
