#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "seek-file.h"
#include "error_codes.h"
#include "error_handler.h"

int seekFile(int file, int offset)
{
	int position = lseek(file, offset, SEEK_CUR);

	if (position == -1)
	{
		return SEEKING_FILE_ERROR;
	}
	return SUCCESS;
}