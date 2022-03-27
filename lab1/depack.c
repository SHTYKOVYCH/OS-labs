//
// Created by dimitis on 04.03.2022.
//

#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
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
    if (!node) {
        return buffer;
    }

    if (node->parentNode != NULL) {
        assemblePath(node->parentNode, buffer);
    }

    int newSize = strlen(buffer) + strlen(node->name) + 2;
    if ((buffer = realloc(buffer, newSize)) == NULL) {
        perror("Error on allocating memory");
        return NULL;
    };

    strcat(buffer, node->name);
    strcat(buffer, "/");

    return buffer;
}

int depack(Args *args) {
    int fileId = open(args->inputFile, O_RDONLY);

    if (fileId < 0) {
        perror("Error on opening archive");
        return ERROR;
    }

    int numOfFiles;

    if (read(fileId, &numOfFiles, 4) == -1) {
        perror("Error on reading archive: ");
        close(fileId);
        return ERROR;
    }

    if (mkdir(args->outputFile, 0777)) {
        perror("Error on creating base directory:");
        close(fileId);
        return ERROR;
    }

    treeNode *dirTree = createDirTree(args->outputFile, NULL);
    if (dirTree == NULL) {
        close(fileId);
        return ERROR;
    }
    treeNode *parentDir = dirTree;
    treeNode *root = dirTree;

    treeNode *leaf = createDirTree(".", dirTree);
    if (leaf == NULL) {
        close(fileId);
        deleteTree(dirTree);
        return ERROR;
    }

    if (addDir(parentDir, leaf) == NULL) {
        close(fileId);
        deleteTree(dirTree);
        return ERROR;
    }
    leaf->deep = 0;

    parentDir = leaf;

    json record;

    char buffer[1024] = {0};
    char *path = malloc(1);
    if (path == NULL) {
        perror("Error on allocating memory");
        return ERROR;
    }

    for (int i = 0; i < numOfFiles; ++i) {
        path[0] = 0;
        int errorCode = 0;

        errorCode = readJSONFromFile(fileId, buffer);

        if (errorCode != SUCCESS && errorCode != END_OF_FILE) {
            free(path);
            deleteTree(root);
            return errorCode;
        }

        if (string2json(buffer, &record) != SUCCESS) {
            printf("Error: unexpected error on parsing file");
            free(path);
            deleteTree(root);

            if (record.name == NULL) {
                free(record.name);
            }

            if (record.parentDir == NULL) {
                free(record.parentDir);
            }

            return ERROR;
        }


        if (strcmp(parentDir->name, record.parentDir) || record.deep < parentDir->deep) {
            parentDir = reverseFindDir(parentDir, record.parentDir, record.deep);

            if (parentDir == NULL) {
                printf("Unenspected error on searching parent directory");
                return ERROR;
            }
        }

        if (record.type == 'f') {
            char *bufferForFile = malloc(record.size);
            if (bufferForFile == NULL) {
                perror("Error on allocating memory");
                free(path);
                deleteTree(root);
                if (record.name == NULL) {
                    free(record.name);
                }

                if (record.parentDir == NULL) {
                    free(record.parentDir);
                }
                return ERROR;
            }

            errorCode = readFile(fileId, bufferForFile, record.size);

            if (errorCode != SUCCESS && errorCode != END_OF_FILE) {
                perror("Error on reading file");

                free(path);
                deleteTree(root);
                free(bufferForFile);
                if (record.name == NULL) {
                    free(record.name);
                }

                if (record.parentDir == NULL) {
                    free(record.parentDir);
                }
                close(fileId);
                return errorCode;
            }

            assemblePath(parentDir, path);
            path = realloc(path, strlen(path) + strlen(record.name) + 2);
            strcat(path, record.name);

            int newFileId = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0777);

            if (newFileId == -1) {
                perror("Error on opening file");

                free(path);
                deleteTree(root);
                free(bufferForFile);
                if (record.name == NULL) {
                    free(record.name);
                }

                if (record.parentDir == NULL) {
                    free(record.parentDir);
                }
                close(fileId);
                return OPENING_FILE_ERROR;
            }

            errorCode = writeFile(newFileId, bufferForFile, record.size);

            if (errorCode != SUCCESS) {
                perror("Error on writing file");

                free(path);
                deleteTree(root);
                free(bufferForFile);
                if (record.name == NULL) {
                    free(record.name);
                }

                if (record.parentDir == NULL) {
                    free(record.parentDir);
                }
                close(fileId);
                return errorCode;
            }

            free(bufferForFile);
            close(newFileId);
        } else {
            treeNode *newDir = createDirTree(record.name, NULL);
            if (newDir == NULL) {
                free(path);
                deleteTree(root);
                if (record.name == NULL) {
                    free(record.name);
                }

                if (record.parentDir == NULL) {
                    free(record.parentDir);
                }
                close(fileId);
                return ERROR;
            }

            if (addDir(parentDir, newDir) == NULL) {
                free(path);
                deleteTree(root);
                if (record.name == NULL) {
                    free(record.name);
                }

                if (record.parentDir == NULL) {
                    free(record.parentDir);
                }
                close(fileId);
                return ERROR;
            }

            parentDir = newDir;

            path = assemblePath(newDir, path);

            if (mkdir(path, 0777) == -1) {
                perror("Error on creating dir");
                free(path);
                deleteTree(root);
                if (record.name == NULL) {
                    free(record.name);
                }

                if (record.parentDir == NULL) {
                    free(record.parentDir);
                }
                close(fileId);
                return ERROR;
            }
        }
    }

    deleteTree(root);

    free(path);

    if (record.name == NULL) {
        free(record.name);
    }

    if (record.parentDir == NULL) {
        free(record.parentDir);
    }

    close(fileId);

    return SUCCESS;
}