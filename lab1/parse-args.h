#ifndef OS_PARSE_ARGS_H
#define OS_PARSE_ARGS_H

struct Args
{
	int depack;
	char *inputFile;
	char *outputFile;
};

char* parseFilename(char* filename, char type);
int parseArgs(int argc, char **argv, Args *args);

#endif