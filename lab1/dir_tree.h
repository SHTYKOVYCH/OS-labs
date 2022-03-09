//
// Created by dimitis on 04.03.2022.
//

#ifndef OS_DIR_TREE_H
#define OS_DIR_TREE_H

struct treeNode_s{
    char* name;
    struct treeNode_s* parentNode;
    struct treeNode_s** childNodes;
    int numOfChildren;
};

typedef struct treeNode_s treeNode;

treeNode* createDirTree(char* dirName, treeNode* parentDir);

treeNode* addDir(treeNode* node, treeNode* child);

treeNode* findDir(treeNode* node, char* dirName);

void deleteTree(treeNode* dirTree);


#endif //OS_DIR_TREE_H
