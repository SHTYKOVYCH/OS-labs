#ifndef OS_STRING_TO_JSON_H
#define OS_STRING_TO_JSON_H

struct json_s
{
	char *name;
	char *parentDir;
	char type;
	unsigned int size;
};

typedef struct json_s json;

int string2json(const char* string, json *jason);

#endif