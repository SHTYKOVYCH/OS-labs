#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "seek-file.h"
#include "error_codes.h"
#include "error_handler.h"

void seekFile(int file, int offset)
{
	int position = lseek(file, offset, SEEK_CUR);

	if (position == -1)
	{
		char filedesc_str[10];
		sprintf(filedesc_str, "%d", file);
		
		errorHandler(SEEKING_FILE_ERROR, filedesc_str);
	}
	errorHandler(SUCCESS);
}