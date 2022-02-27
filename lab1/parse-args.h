#ifndef OS_PARSE_ARGS_H
#define OS_PARSE_ARGS_H

struct Args_s
{
	int depack;
	char *inputFile;
	char *outputFile;
};

typedef struct Args_s Args;

char* parseFilename(char* filename);
int parseArgs(int argc, char **argv, Args *args);

#endif