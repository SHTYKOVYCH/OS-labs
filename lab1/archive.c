#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "error_codes.h"
#include "archive.h"
#include "read-file.h"
#include "write_file.h"
#include "seek-file.h"

void sprintDir(char *dir, char* str, int *count)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
 
    if ((dp = opendir(dir)) == NULL)
    {
        perror(dir);
        return;
    }
    
    chdir(dir);
    while ((entry = readdir(dp)) != NULL)
    {
        lstat(entry->d_name, &statbuf);
        if (entry->d_type == 4)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 ) 
            {
                continue;
            } 

            (*count)++;
            sprintDir(entry->d_name, str, count);

            char *buff = malloc(sizeof(char) * 1024);
            memset(buff, 0, 1024);

            sprintf(buff, "{name:\"%s\",parentDir:\"%s\",type=\"d\"}", entry->d_name, dir);
            strcat(str, buff);

            free(buff);
        }
        else
        {
            char *meta = malloc(sizeof(char) * 512);
            memset(meta, 0, 512);
            char *abs_filename = malloc(sizeof(char) * 1024);
            memset(abs_filename, 0, 1024);
            char *contents = malloc(sizeof(char) * 4096);
            memset(contents, 0, 4096);

            getcwd(abs_filename, 1024);
            strcat(abs_filename, "/");
            strcat(abs_filename, entry->d_name);

            sprintf(meta, "{name:\"%s\",parentDir:\"%s\",size:\"%ld\",type=\"f\"}", entry->d_name, dir, statbuf.st_size);
            strcat(str, meta);

            int filedescr = open(abs_filename, O_RDONLY);
            readFile(filedescr, contents, statbuf.st_size);
            close(filedescr);

            strcat(str, contents);

            free(meta);
            free(abs_filename);
            free(contents);

            (*count)++;
        }
    }
    chdir("..");
    closedir(dp);   
}

void archive(char *dir)	// Требуется указывать полный путь до архивируемой папки. К примеру, /home/nikolay/Testdir
{
    int filedescr = open("text.txt", O_RDWR);

    int count = 0;

    char *buff = malloc(sizeof(char) * 65536);
    memset(buff, 0, 65536);
    sprintDir(dir, buff, &count);

    char count_str[12];
    sprintf(count_str, "%d", count);

    writeFile(filedescr, count_str, strlen(count_str));
    seekFile(filedescr, strlen(count_str));
    writeFile(filedescr, buff, strlen(buff));

    free(buff);
    close(filedescr);
}