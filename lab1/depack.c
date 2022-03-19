//
// Created by dimitis on 04.03.2022.
//

#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "error_codes.h"
#include "error_handler.h"
#include "depack.h"
#include "read-file.h"
#include "read_json_from_file.h"
#include "string-2-json.h"
#include "write_file.h"

char *assemblePath(treeNode *node, char *buffer) {

    if (node->parentNode != NULL) {
        assemblePath(node->parentNode, buffer);
    }

    int newSize = strlen(buffer) + strlen(node->name) + 2;
    buffer = realloc(buffer, newSize);

    strcat(buffer, node->name);
    strcat(buffer, "/");

    return buffer;
}

int depack(Args *args) {
    int fileId = open(args->inputFile, O_RDONLY);

    if (fileId < 0) {
        errorHandler(OPENING_FILE_ERROR, "no file with given name found");
        return OPENING_FILE_ERROR;
    }

    int numOfFiles;

    if (read(fileId, &numOfFiles, 4) != 4) {
        errorHandler(READING_FROM_FILE_ERROR, args->inputFile);
        return READING_FROM_FILE_ERROR;
    }

    mkdir(args->outputFile, 0777);

    treeNode *dirTree = createDirTree(args->outputFile, NULL);
    treeNode *parentDir = dirTree;

    treeNode* leaf = createDirTree(".", dirTree);

    addDir(parentDir, leaf);
    leaf->deep = 0;

    parentDir = leaf;

    json record;

    char buffer[1024] = {0};
    char *path = malloc(1);
    path[0] = 0;

    for (int i = 0; i < numOfFiles; ++i) {
        int errorCode = 0;

        errorCode = readJSONFromFile(fileId, buffer);

        if (errorCode != SUCCESS && errorCode != END_OF_FILE) {
            errorHandler(errorCode, args->inputFile);
            return errorCode;
        }

        string2json(buffer, &record);

        if (strcmp(parentDir->name, record.parentDir) || record.deep < parentDir->deep) {
            parentDir = reverseFindDir(parentDir, record.parentDir, record.deep);

            if (parentDir == NULL) {
                return 0;
            }
        }

        if (record.type == 'f') {
            char *bufferForFile = malloc(record.size);
            errorCode = readFile(fileId, bufferForFile, record.size);

            if (errorCode != SUCCESS && errorCode != END_OF_FILE) {
                errorHandler(errorCode, args->inputFile);
                free(path);
                free(bufferForFile);
                deleteTree(dirTree);
                return errorCode;
            }

            assemblePath(parentDir, path);
            path = realloc(path, strlen(path) + strlen(record.name) + 2);
            strcat(path, record.name);

            int newFileId = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0777);

            if (newFileId == -1) {
                errorHandler(OPENING_FILE_ERROR, path);
                free(path);
                free(bufferForFile);
                deleteTree(dirTree);
                return OPENING_FILE_ERROR;
            }

            errorCode = writeFile(newFileId, bufferForFile, record.size);

            if (errorCode != SUCCESS) {
                errorHandler(errorCode, record.name);
                free(path);
                free(bufferForFile);
                deleteTree(dirTree);
                return errorCode;
            }

            free(bufferForFile);
            close(newFileId);
        } else {
            treeNode *newDir = createDirTree(record.name, NULL);
            addDir(parentDir, newDir);
            parentDir = newDir;

            assemblePath(newDir, path);

            mkdir(path, 0777);
        }
        path[0] = 0;
    }

    deleteTree(dirTree->parentNode);

    free(path);

    close(fileId);

    return SUCCESS;
}