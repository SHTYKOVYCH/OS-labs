#include <stdio.h>
#include <string.h>

#include "error_codes.h"
#include "error_handler.h"
#include "parse-args.h"

char* parseFilename(char* filename)
{
	if (strlen(filename) >= 255)
	{
		errorHandler(WRONG_PARAMETER_ERROR, "Имя файла должно содержать менее 255 символов");
		exit(1);
	}
	if (strstr(filename, "/"))	// Исключение запрещённых символов в имени файла 
	{
		errorHandler(WRONG_PARAMETER_ERROR, "Имя файла не должно содержать \'/\'");
		exit(1);
	}
	if (!strcmp(filename, ".") || !strcmp(filename, ".."))	// Зарезервированные имена
	{
		errorHandler(WRONG_PARAMETER_ERROR, "Имя файла зарезервировано");
		exit(1);
	}
	return filename;
}

int parseArgs(int argc, char **argv, Args *args)
{
	args->depack = 0;
	args->outputFile = "archive";
	args->inputFile = "";

	for (int i = 1; i < argc; i++)
    {
    	char *arg = argv[i];
    	if (!strcmp(arg, "-d"))
    	{
    		args->depack = 1;
    	}
    	else if (!strcmp(arg, "-i"))
    	{
    		if (i + 1 == argc)	// Будем читать следующий элемент, если возможно
    		{
                errorHandler(MISSING_PARAMETER_ERROR, "input filename required after flag -i");
    			return MISSING_PARAMETER_ERROR;
    		}
    		else
    		{
    			args->inputFile = parseFilename(argv[i + 1]);
    			++i;
    		}
    	}
    	else if (!strcmp(arg, "-o"))
    	{
    		if (i + 1 == argc)
    		{
                errorHandler(MISSING_PARAMETER_ERROR, "output filename required after flag -o");
    			return MISSING_PARAMETER_ERROR;
    		}
    		else
    		{
    			args->outputFile = parseFilename(argv[i + 1]);
    			++i;
    		}
    	}
    	else
    	{
        	return NOT_EXISTING_PARAMETER_ERROR;
    	}
    }

    if (!strcmp(args->inputFile, ""))	// Если так и не получили имя входного файла
    {
        errorHandler(MISSING_PARAMETER_ERROR, "input filename required");
    	return MISSING_PARAMETER_ERROR;
    }

	return SUCCESS;
}