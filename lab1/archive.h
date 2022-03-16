#ifndef OS_ARCHIVE_H
#define OS_ARCHIVE_H

#include "string-2-json.h"

void sprintDir(int filedescr, char *dir, char* str, json* jason, int *count);
void archive(char *dir, char *archName);

#endif