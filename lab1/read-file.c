#include <stdlib.h>
#include <unistd.h>

#include "read-file.h"
#include "error_codes.h"

int readFile(int file, char* buffer, int nread)
{
    int mread = read(file, buffer, nread);

    if (mread <= nread)
    {
    	return READING_FROM_FILE_ERROR;
    }

    return SUCCESS;
}